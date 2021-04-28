#include "RLM300.h"

//��ʱ����洢�ĵ�
#define CLEAR_GAP0      600           //�յ�Ԥ�����ʱ1min       CLEAR_GAP0*100ms
#define CLEAR_GAP1      100            //�յ���������ʱ1s            CLEAR_GAP1*2 ms
/*CLEAR_GAP2 = 1800, Ϊ���Է����޸ĳ�����*/
#define CLEAR_GAP2      100           //�յ���������ʱ3min        CLEAR_GAP2*2 ms 
#define CLEAR_GAP3      70            //�յ���������ʱ7s             CLEAR_GAP3*2 ms
#define CLEAR_GAP4      600          //�յ���������ʱ��1min     CLEAR_GAP4*2 ms

uint8_t ledType = 0; 

/*��־λ��ر���*/
uint8_t flag_NewData = FALSE;    //�Ƿ���������
uint8_t flag_Store = END;    //�Ƿ�ʼ�洢��������
uint8_t flag_LedON = END;    //�Ƿ�ʼ����LED
uint8_t flag_RxFinish = FALSE;
uint8_t flag_Rx125K = FALSE;
uint8_t flag_125K_Process_END = FALSE;
uint8_t clearFlag_YGD = END;
uint8_t clearFlag_Forward_DD = END;
uint8_t clearFlag_Rear_DD = END;

/*��ʱ����ر���*/
uint32_t  tLedCount= 0;    //LED��ʱ������
uint32_t  tCountYGD  = 0;			   //Ԥ����ʱ������ 
uint32_t  tCountFD  = 0;			    //�����ʱ������ 
uint32_t  tCountRD  = 0;			   //�����ʱ������  
uint32_t  tFeedDog = 0;

/*���鶨����*/																		
uint8_t rxUart1[20];
uint8_t rxUart1_125K[20];                                                                   //����1���ݻ�����
uint8_t data_from_125K[20];																	//����0���ݻ�����, ����RLM���������																	
uint8_t EpcData[12] = {0xBB, 0xE1, 0x0, 0x0, 0x0, 0x0, 0x02, 0x51, 0x32, 0x44, 0x02, 0x53}; //��RLM֡��������ȡ���õ�����, E1 -- ����   E0 -- ����
uint8_t storeFdirYGD[13];																	//�洢����Ԥ���1
uint8_t storeFdirYGD2[13];																	//�洢����Ԥ���2
uint8_t storeRedirDD[13];																	//�洢���򶨵�
uint8_t storeLastportdata[13];																//�洢�ϴ��ϱ��Ķ�������
uint8_t temp[14];


/**********************************************************************************************
* �� �� �� ��DealRxData
* ����˵�� ������Ӵ���0���յ�����(��Ƶģ�鴫����������)
* ������� ��void
* �� �� ֵ ��void
**********************************************************************************************/
void Deal_RLM_Data(void)
{
	if(DecodeProtocol(rxUart1, EpcData))    //��RLM�����ݽ���У�飬ֻ��ͨ��У������ݲ��ܽ�����һ���Ĵ���
    {
		//RuleCheck(EpcData);
	}
	else
	{
		return;    //�����κδ���
	}
}

/***************************************************************************************************************
* �� �� �� ��DecodeProtocol
* ����˵�� ���Ի��з��͹��������ݽ���У�� �Ƿ�������������֡�Լ�����֡���Ƿ����Epc����
* ������� ��p_rxUart1 --> rxUart1   p_EpcData --> EpcData
* �� �� ֵ ��1 or 0
***************************************************************************************************************/
uint8_t DecodeProtocol(const uint8_t* p_rxUart1, uint8_t* p_EpcData)
{
    uint8_t i;
	const uint8_t* p = p_rxUart1;
	
	if((*p++ == 0xAA) && (*(p+*p) == 0x55))    //����������֡
	{
		if((p_rxUart1[5]==0xAA) && (p_rxUart1[6] == 0xBB))    //��AABB���ݣ�
		{
			for(i= 0; i< 12; i++)
	        {	
		        p_EpcData[i]= p_rxUart1[i+6];    //�����ڻ���������Ч���ݷŵ�EpcData������   
	        }
			return 1;
		}
	}
	return 0;
}


/************************************************************************************************
* �� �� �� ��RuleCheck
* ����˵������Epc���ݽ��й����� ͨ�� --> ͨ������0���Ϸ�������  ��ͨ�� --> ����
* ���������p_EpcData --> EpcData
* �� �� ֵ ��void
************************************************************************************************/
void RuleCheck(const uint8_t* p_EpcData, uint8_t* flag)
{
	if(p_EpcData[6] == 0x01)    //�����Ԥ���
	{
		clearFlag_YGD = BEGIN;    //����Ԥ����ʱ��0
		tCountYGD = 0;     
		/*���Ԥ��㣬����Ԥ��� or ����Ԥ���*/             
		check_preportdata(EpcData, storeRedirDD, storeFdirYGD, storeFdirYGD2);
		printf("yugaodian \n");
	}
		 
	if(p_EpcData[6] == 0x02)    //����ǵظ�
	{
		/*���ظУ����� or ����  �ϱ� or ����*/
		printf("dingdian \n");
		if(check_portdata(EpcData, storeFdirYGD, storeFdirYGD2, storeRedirDD, storeLastportdata)) 
		{
			//FrameProcess(FrameData, EpcData, 0x11, sizeof(EpcData));       
			ledType = EpcData[7];
			flag_LedON = BEGIN;
			//MPCM_USART1_TransmitFrame(FrameData, 0xC1);    //���͸�WirelessCom_1 --> ͨ�Ű��һ��CPU
			*flag = TRUE;
			printf("dingdian ok \n");
		}
		else
		{
			flag = FALSE;
		}
	}
}

/*********************************************************************************************************************************************************
* �� �� �� ��check_preportdata
* ����˵����Ԥ���ļ����� �뷴�򶨵��е����ݽ��бȽ�
                    ƥ�� --> ����  ��ƥ�� --> ��������Ԥ���
* ���������p_EpcData --> EpcData  p_storeRedirDD --> storeRedirDD  p_storeFdirYGD --> storeFdirYGD  p_storeFdirYGD2 --> storeFdirYGD2
* �� �� ֵ ��void
*********************************************************************************************************************************************************/
void check_preportdata(uint8_t* p_EpcData, uint8_t* p_storeRedirDD, uint8_t* p_storeFdirYGD, uint8_t* p_storeFdirYGD2)
{
	uint8_t i;  
	uint8_t epc_Code = p_EpcData[10];
	uint8_t epc_Distance = p_EpcData[8];
	/*�ж��Ƿ�������Ԥ��㣬����������Ԥ��㣬�뷴�򶨵��źŻ����벻ͬ����벻ͬ*/
	if( (p_storeRedirDD[10] != epc_Code) || (p_storeRedirDD[8] != epc_Distance) )	  
	{   		                                                                
		/*������Ԥ��㣬�ж��Ƿ����ظ��ĵ㣬���ظ������ԣ����ظ�����storeFdirYGD*/	
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
			p_EpcData[10] = 0;    //�������źŵƺ󣬳�ת�򣬵�һ���źŵƻ���Ϊ�¾����ݵ��ж϶������Բ���
			p_EpcData[8] = 0;		
		}
	}
	/*�Ƿ����Ԥ���*/
	else                            
	{
		p_EpcData[10] = 0;	   //�������źŵƺ󣬳�ת�򣬵�һ���źŵƻ���Ϊ�¾����ݵ��ж϶������Բ���   
		p_EpcData[8] = 0;	
	}	
}

/*********************************************************************************************************************************************************
* �� �� �� ��check_portdata
* ����˵�����ظеļ����� ������Ԥ����е����ݽ��бȽϣ�
                    ƥ���Ҳ��ظ� --> ����ͨ�� 
					��ƥ�� --> ���뷴�򶨵�
* ���������p_EpcData --> EpcData  p_storeFdirYGD --> storeFdirYGD  p_storeFdirYGD2 --> storeFdirYGD2  
                    p_storeRedirDD --> storeRedirDD   p_storeLastportdata --> storeLastportdata
* �� �� ֵ ��ͨ��-->1 or ��ͨ��-->0
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
	/*�ж��Ƿ������򶨵�*/
	if( (p_storeFdirYGD[10] == epc_Code) && (p_storeFdirYGD[8] == epc_Distance) )
	{ 
    	/*�ж��Ƿ�����ͬ���ݣ�������ǣ�����storeLastportdata*/
		//printf(" is front dd\n ");
		if( (p_storeLastportdata[10] != epc_Code) || (p_storeLastportdata[8] != epc_Distance) || (p_storeLastportdata[9] != epc_Led) || (p_storeLastportdata[10] != epc_Code))    //ͬһ������ֻ����һ��  //������ͬ�Ƿ��ظ�����
        {
	        for(i= 0; i< 12; i++)
		    {
		        p_storeLastportdata[i] = p_EpcData[i];
		    }
		    temp = 1;	 //�ϱ�����
			clearFlag_Forward_DD = BEGIN;   	 //������ʱ��3MIN����ϴδ��涨��
			tCountFD = 0;
			//clearYGDsave = BEGIN;	 //������ʱ��1s����ϴδ���Ԥ���ͷ�����
			//clearflag1 = 0; 	
		}
		/*����ͬ���ݣ�����*/
		else    
		{
		}		  
    //CheckFeed();         
	}
		
	/*�ж��Ƿ������򶨵㣬��YGD2ƥ��*/
	else if( (p_storeFdirYGD2[10] == epc_Code) && (p_storeFdirYGD2[8] == epc_Distance))
	{
	    if( (p_storeLastportdata[10] !=epc_Code ) || (p_storeLastportdata[8] !=epc_Distance) || (p_storeLastportdata[9] !=epc_Led) || (p_storeLastportdata[10] != epc_Code))	    //ͬһ������ֻ����һ��
		{
	        for(i= 0; i< 12; i++)
		    {
		        p_storeLastportdata[i]= p_EpcData[i];
		    }
		    temp= 1;	
			clearFlag_Forward_DD = BEGIN;   	 //������ʱ��3MIN����ϴδ��涨��
			tCountFD = 0;
		}
		else                                                   //���ϴ��ϱ�������ͬ�������ϱ�//��7S������
		{
		}		
	//CheckFeed();      
	}
	/*�ж�Ϊ���򶨵�*/
	else
	{	
		/*�ж��Ƿ��ظ������ظ������뷴�򶨵�*/
		if( (p_storeRedirDD[10] != epc_Code) || (p_storeRedirDD[8] != epc_Distance) )	 //ͬһ������Ϊ����ֻ�洢1��
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
* �� �� �� ��Time_up_Clear
* ����˵������ʱʱ�䵽������������洢��Epc����
* ���������void
* �� �� ֵ ��void
************************************************************************************************/
void Time_up_Clear(void)
{
	/*�յ�Ԥ����ʼ��ʱ��洢�ĵ�*/
	if(tCountYGD >= CLEAR_GAP0)    //��ʱ1min����0
	{
		clearFlag_YGD = END;
		tCountYGD = 0;
		
		memset(storeFdirYGD, 0, sizeof(storeFdirYGD));
		memset(storeFdirYGD2, 0, sizeof(storeFdirYGD2));
		memset(storeLastportdata, 0, sizeof(storeLastportdata));
	}
		
	/*�յ��������ʼ��ʱ��洢�ĵ�*/
	if(tCountFD >= CLEAR_GAP1)    //��ʱ1s����0
	{
		clearFlag_Rear_DD = END; 
		tCountRD = 0;
		clearFlag_YGD = END; 
		tCountYGD = 0;
			
		memset(storeRedirDD, 0, sizeof(storeRedirDD));
		memset(storeFdirYGD, 0, sizeof(storeFdirYGD));
		memset(storeFdirYGD2, 0, sizeof(storeFdirYGD2));
		if(tCountFD >= CLEAR_GAP2)    //��ʱ3min����0
		{
			clearFlag_Forward_DD = END;
			tCountFD = 0;
				
			memset(storeLastportdata, 0, sizeof(storeLastportdata));
		}
	} 
		
	/*�յ��������ʼ��ʱ��洢�ĵ�*/
	if(tCountRD >= CLEAR_GAP3)    //��ʱ7s����0
	{
		memset(storeRedirDD, 0, sizeof(storeRedirDD));
		if(tCountRD >= CLEAR_GAP4)    //��ʱ1min����0
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
	// 02 41 41 00 00 00 00 00 00 00 00 0d 0a 03 //  AA 00 -- ����������
	if((data[6] == 0x34) && (((data[2] >= '0') && (data[2] <= '9')) || ((data[2] >= 'A') && (data[2] <= 'F'))))
	{
		transfer(data, 0);
		//ledtype = temp[2];
		//TWISTART();    //TWI��������
	}
	
	if((data[1] == 0x41) && (data[2] == 0x41) && (data[3] == 0x30) && (data[4] == 0x30) \
	&& (data[5] == 0x30) && (data[6] == 0x30) && (data[7] == 0x30) && (data[8] == 0x30) \
	&& (data[9] == 0x30) && (data[10] == 0x30) && (data[11] == 0x0d) && (data[12] == 0x0a) \
	&& (data[13] == 0x03))
	{
		transfer(data, 1);
		//ledtype = 0;
		//TWISTART();   //TWI��������
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
