#include "RLM300.h"
#include "config.h"

//定时清除存储的点
#define CLEAR_GAP0      600           //收到预告点后计时1min       CLEAR_GAP0*100ms
#define CLEAR_GAP1      100            //收到正向码后计时1s            CLEAR_GAP1*2 ms
/*CLEAR_GAP2 = 1800, 为调试方便修改成如下*/
#define CLEAR_GAP2      100           //收到正向码后计时3min        CLEAR_GAP2*2 ms 
#define CLEAR_GAP3      70            //收到反向码后计时7s             CLEAR_GAP3*2 ms
#define CLEAR_GAP4      600          //收到反向码后计时数1min     CLEAR_GAP4*2 ms

uint8_t ledType = 0; 

/*标志位相关变量*/
uint8_t flag_NewData = FALSE;    //是否是新数据
uint8_t flag_Store = END;    //是否开始存储串口数据
uint8_t flag_LedON = END;    //是否开始点亮LED
uint8_t flag_RxFinish = FALSE;
uint8_t flag_Rx125K = FALSE;
uint8_t flag_125K_Process_END = FALSE;
uint8_t clearFlag_YGD = END;
uint8_t clearFlag_Forward_DD = END;
uint8_t clearFlag_Rear_DD = END;

/*定时器相关变量*/
uint32_t  tLedCount= 0;    //LED计时计数器
uint32_t  tCountYGD  = 0;			   //预告点计时计数器 
uint32_t  tCountFD  = 0;			    //正向计时计数器 
uint32_t  tCountRD  = 0;			   //反向计时计数器  
uint32_t  tFeedDog = 0;

/*数组定义区*/																		
uint8_t rxUart1[20];
uint8_t rxUart1_125K[20];                                                                   //串口1数据缓存区
uint8_t data_from_125K[20];																	//串口0数据缓存区, 接收RLM传输的数据																	
uint8_t EpcData[12] = {0xBB, 0xE1, 0x0, 0x0, 0x0, 0x0, 0x02, 0x51, 0x32, 0x44, 0x02, 0x53}; //从RLM帧数据中提取有用的数据, E1 -- 有线   E0 -- 无线
uint8_t storeFdirYGD[13];																	//存储正向预告点1
uint8_t storeFdirYGD2[13];																	//存储正向预告点2
uint8_t storeRedirDD[13];																	//存储反向定点
uint8_t storeLastportdata[13];																//存储上次上报的定点数据
uint8_t temp[14];


/**********************************************************************************************
* 函 数 名 ：DealRxData
* 功能说明 ：处理从串口0接收的数据(射频模块传上来的数据)
* 输入参数 ：void
* 返 回 值 ：void
**********************************************************************************************/
void Deal_RLM_Data(void)
{
	if(DecodeProtocol(rxUart1, EpcData))    //对RLM的数据进行校验，只有通过校验的数据才能进行下一步的处理
    {
		//RuleCheck(EpcData);
	}
	else
	{
		return;    //不做任何处理
	}
}

/***************************************************************************************************************
* 函 数 名 ：DecodeProtocol
* 功能说明 ：对机感发送过来的数据进行校验 是否是完整的数据帧以及数据帧中是否包含Epc数据
* 输入参数 ：p_rxUart1 --> rxUart1   p_EpcData --> EpcData
* 返 回 值 ：1 or 0
***************************************************************************************************************/
uint8_t DecodeProtocol(const uint8_t* p_rxUart1, uint8_t* p_EpcData)
{
    uint8_t i;
	const uint8_t* p = p_rxUart1;
	
	if((*p++ == 0xAA) && (*(p+*p) == 0x55))    //完整的数据帧
	{
		if((p_rxUart1[5]==0xAA) && (p_rxUart1[6] == 0xBB))    //是AABB数据？
		{
			for(i= 0; i< 12; i++)
	        {	
		        p_EpcData[i]= p_rxUart1[i+6];    //将串口缓存区的有效数据放到EpcData数组中   
	        }
			return 1;
		}
	}
	return 0;
}


/************************************************************************************************
* 函 数 名 ：RuleCheck
* 功能说明：对Epc数据进行规则检查 通过 --> 通过串口0向上发送数据  不通过 --> 忽略
* 输入参数：p_EpcData --> EpcData
* 返 回 值 ：void
************************************************************************************************/
void RuleCheck(const uint8_t* p_EpcData, uint8_t* flag)
{
	if(p_EpcData[6] == 0x01)    //如果是预告点
	{
		clearFlag_YGD = BEGIN;    //开启预告点计时清0
		tCountYGD = 0;     
		/*检查预告点，正向预告点 or 反向预告点*/             
		check_preportdata(EpcData, storeRedirDD, storeFdirYGD, storeFdirYGD2);
		PRINTF("yugaodian \n");
	}
		 
	if(p_EpcData[6] == 0x02)    //如果是地感
	{
		/*检查地感，正向 or 反向  上报 or 忽略*/
		PRINTF("dingdian \n");
		if(check_portdata(EpcData, storeFdirYGD, storeFdirYGD2, storeRedirDD, storeLastportdata)) 
		{
			//FrameProcess(FrameData, EpcData, 0x11, sizeof(EpcData));       
			ledType = EpcData[7];
			flag_LedON = BEGIN;
			//MPCM_USART1_TransmitFrame(FrameData, 0xC1);    //发送给WirelessCom_1 --> 通信板第一个CPU
			*flag = TRUE;
			PRINTF("dingdian ok \n");
		}
		else
		{
			flag = FALSE;
		}
	}
}

/*********************************************************************************************************************************************************
* 函 数 名 ：check_preportdata
* 功能说明：预告点的检查规则 与反向定点中的数据进行比较
                    匹配 --> 忽略  不匹配 --> 存入正向预告点
* 输入参数：p_EpcData --> EpcData  p_storeRedirDD --> storeRedirDD  p_storeFdirYGD --> storeFdirYGD  p_storeFdirYGD2 --> storeFdirYGD2
* 返 回 值 ：void
*********************************************************************************************************************************************************/
void check_preportdata(uint8_t* p_EpcData, uint8_t* p_storeRedirDD, uint8_t* p_storeFdirYGD, uint8_t* p_storeFdirYGD2)
{
	uint8_t i;  
	uint8_t epc_Code = p_EpcData[10];
	uint8_t epc_Distance = p_EpcData[8];
	/*判断是否是正向预告点，如果是正向的预告点，与反向定点信号机编码不同或距离不同*/
	if( (p_storeRedirDD[10] != epc_Code) || (p_storeRedirDD[8] != epc_Distance) )	  
	{   		                                                                
		/*是正向预告点，判断是否是重复的点，若重复，忽略，不重复存入storeFdirYGD*/	
		if( ( (p_storeFdirYGD[10] != epc_Code) || (p_storeFdirYGD[8] != epc_Distance) ) && ( (p_storeFdirYGD2[10] != epc_Code) || (p_storeFdirYGD2[8] != epc_Distance) ) )
		{
			for(i= 0; i<12; i++)
		    {
				p_storeFdirYGD2[i] = p_storeFdirYGD[i]; 
			    p_storeFdirYGD[i] = p_EpcData[i];
			}
			//printf(" relocated\n ");
		}
		else
		{
			p_EpcData[10] = 0;    //过反向信号灯后，车转向，第一个信号灯会因为新旧数据的判断而被忽略不报
			p_EpcData[8] = 0;		
		}
	}
	/*是反向的预告点*/
	else                            
	{
		p_EpcData[10] = 0;	   //过反向信号灯后，车转向，第一个信号灯会因为新旧数据的判断而被忽略不报   
		p_EpcData[8] = 0;	
	}	
}

/*********************************************************************************************************************************************************
* 函 数 名 ：check_portdata
* 功能说明：地感的检查规则 与正向预告点中的数据进行比较，
                    匹配且不重复 --> 规则通过 
					不匹配 --> 存入反向定点
* 输入参数：p_EpcData --> EpcData  p_storeFdirYGD --> storeFdirYGD  p_storeFdirYGD2 --> storeFdirYGD2  
                    p_storeRedirDD --> storeRedirDD   p_storeLastportdata --> storeLastportdata
* 返 回 值 ：通过-->1 or 不通过-->0
*********************************************************************************************************************************************************/
uint8_t check_portdata(uint8_t* p_EpcData, uint8_t* p_storeFdirYGD, uint8_t* p_storeFdirYGD2, uint8_t* p_storeRedirDD, uint8_t* p_storeLastportdata)
{
	uint8_t temp = 0; 
	uint8_t i;
	uint8_t epc_Code = p_EpcData[10];
	uint8_t epc_Distance = p_EpcData[8]; 
	uint8_t epc_Led = p_EpcData[9];  
	
	// for(i = 0; i<12; i++)
	// 	printf("%02x ", p_EpcData[i]);

	// printf("\n\r");
	
	// for(i = 0; i<12; i++)
	// 	printf("%02x ", p_storeFdirYGD[i]);

	// printf("\n\r");
	// for(i = 0; i<12; i++)
	// 	printf("%02x ", p_storeFdirYGD2[i]);
	
	// printf("\n\r");
	/*判断是否是正向定点*/
	if( (p_storeFdirYGD[10] == epc_Code) && (p_storeFdirYGD[8] == epc_Distance) )
	{ 
    	/*判断是否是相同数据，如果不是，存入storeLastportdata*/
		//printf(" is front dd\n ");
		if( (p_storeLastportdata[10] != epc_Code) || (p_storeLastportdata[8] != epc_Distance) || (p_storeLastportdata[9] != epc_Led) || (p_storeLastportdata[10] != epc_Code))    //同一个卡号只发送一次  //灯码相同是否重复收码
        {
	        for(i= 0; i< 12; i++)
		    {
		        p_storeLastportdata[i] = p_EpcData[i];
		    }
		    temp = 1;	 //上报定点
			clearFlag_Forward_DD = BEGIN;   	 //开启计时，3MIN清除上次储存定点
			tCountFD = 0;
			//clearYGDsave = BEGIN;	 //开启计时，1s清除上次储存预告点和反向码
			//clearflag1 = 0; 	
		}
		/*是相同数据，忽略*/
		else    
		{
		}		  
    //CheckFeed();         
	}
		
	/*判断是否是正向定点，与YGD2匹配*/
	else if( (p_storeFdirYGD2[10] == epc_Code) && (p_storeFdirYGD2[8] == epc_Distance))
	{
	    if( (p_storeLastportdata[10] !=epc_Code ) || (p_storeLastportdata[8] !=epc_Distance) || (p_storeLastportdata[9] !=epc_Led) || (p_storeLastportdata[10] != epc_Code))	    //同一个卡号只发送一次
		{
	        for(i= 0; i< 12; i++)
		    {
		        p_storeLastportdata[i]= p_EpcData[i];
		    }
		    temp= 1;	
			clearFlag_Forward_DD = BEGIN;   	 //开启计时，3MIN清除上次储存定点
			tCountFD = 0;
		}
		else                                                   //与上次上报卡号相同，不在上报//加7S清数据
		{
		}		
	//CheckFeed();      
	}
	/*判断为反向定点*/
	else
	{	
		/*判断是否重复，若重复，存入反向定点*/
		if( (p_storeRedirDD[10] != epc_Code) || (p_storeRedirDD[8] != epc_Distance) )	 //同一个点作为反向，只存储1次
		{
			for( i= 0; i< 12; i++)
			{
				p_storeRedirDD[i] = p_EpcData[i];
    		}		
		}
		clearFlag_Rear_DD = BEGIN;
		tCountRD = 0;                         
	}
	
	return temp;	
} 

/************************************************************************************************
* 函 数 名 ：Time_up_Clear
* 功能说明：定时时间到，按规则清除存储的Epc数据
* 输入参数：void
* 返 回 值 ：void
************************************************************************************************/
void Time_up_Clear(void)
{
	/*收到预告点后开始定时清存储的点*/
	if(tCountYGD >= CLEAR_GAP0)    //计时1min后清0
	{
		clearFlag_YGD = END;
		tCountYGD = 0;
		
		memset(storeFdirYGD, 0, sizeof(storeFdirYGD));
		memset(storeFdirYGD2, 0, sizeof(storeFdirYGD2));
		memset(storeLastportdata, 0, sizeof(storeLastportdata));
	}
		
	/*收到正向码后开始定时清存储的点*/
	if(tCountFD >= CLEAR_GAP1)    //计时1s后清0
	{
		clearFlag_Rear_DD = END; 
		tCountRD = 0;
		clearFlag_YGD = END; 
		tCountYGD = 0;
			
		memset(storeRedirDD, 0, sizeof(storeRedirDD));
		memset(storeFdirYGD, 0, sizeof(storeFdirYGD));
		memset(storeFdirYGD2, 0, sizeof(storeFdirYGD2));
		if(tCountFD >= CLEAR_GAP2)    //计时3min后清0
		{
			clearFlag_Forward_DD = END;
			tCountFD = 0;
				
			memset(storeLastportdata, 0, sizeof(storeLastportdata));
		}
	} 
		
	/*收到反向码后开始定时清存储的点*/
	if(tCountRD >= CLEAR_GAP3)    //计时7s后清0
	{
		memset(storeRedirDD, 0, sizeof(storeRedirDD));
		if(tCountRD >= CLEAR_GAP4)    //计时1min后清0
		{
			clearFlag_Rear_DD = END;
			tCountRD = 0;
				
			memset(storeFdirYGD, 0, sizeof(storeFdirYGD));
			memset(storeFdirYGD2, 0, sizeof(storeFdirYGD2));
			memset(storeLastportdata, 0, sizeof(storeLastportdata));
		}
	}
}

void process_data(const uint8_t* data)
{
	// 02 ** ** ** ** 34 34 ** ** ** ** 0d 0a 03
	// 02 41 41 00 00 00 00 00 00 00 00 0d 0a 03 //  AA 00 -- 机感握手码
	if((data[6] == 0x34) && (((data[2] >= '0') && (data[2] <= '9')) || ((data[2] >= 'A') && (data[2] <= 'F'))))
	{
		transfer(data, 0);
		//ledtype = temp[2];
		//TWISTART();    //TWI发送数据
	}
	
	if((data[1] == 0x41) && (data[2] == 0x41) && (data[3] == 0x30) && (data[4] == 0x30) \
	&& (data[5] == 0x30) && (data[6] == 0x30) && (data[7] == 0x30) && (data[8] == 0x30) \
	&& (data[9] == 0x30) && (data[10] == 0x30) && (data[11] == 0x0d) && (data[12] == 0x0a) \
	&& (data[13] == 0x03))
	{
		transfer(data, 1);
		//ledtype = 0;
		//TWISTART();   //TWI发送数据
	}
}

void transfer(const uint8_t* data, uint8_t handshake)
{
	uint8_t i;
	
	for(i = 1; i < 11; i++)
	{
		if((data[i] >= '0') && (data[i] <= '9'))
		{
			temp[i] = data[i] - 0x30;
		}
		else if((data[i] >= 'A') && (data[i] <= 'F'))
		{
			temp[i] = data[i] - 0x37;
		}
		else
		{
			temp[i] = data[i];
		}
	}
	
	if(handshake)
	{
		;
	}
	else
	{
		EpcData[7]  = (temp[1]<<4)|temp[2];
		EpcData[8]  = (temp[3]<<4)|temp[4];
		EpcData[9]  = (temp[5]<<4)|temp[6];
		EpcData[10] = (temp[7]<<4)|temp[8];
		EpcData[11] = (temp[9]<<4)|temp[10];

		flag_125K_Process_END = TRUE;
	}
}
