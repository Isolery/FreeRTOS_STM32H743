#include "rtc.h"
#include "delay.h"

//等待RSF同步
//返回值:0,成功;1,失败;
u8 RTC_Wait_Synchro(void)
{ 
	u32 retry=0XFFFFF; 
	//关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	RTC->ISR&=~(1<<5);		//清除RSF位 
	while(retry&&((RTC->ISR&(1<<5))==0x00))retry--;//等待影子寄存器同步 
    if(retry==0)return 1;	//同步失败 
	RTC->WPR=0xFF;			//使能RTC寄存器写保护  
	return 0;
}
//RTC进入初始化模式
//返回值:0,成功;1,失败;
u8 RTC_Init_Mode(void)
{ 
	u32 retry=0XFFFFF; 
	if(RTC->ISR&(1<<6))return 0; 
	RTC->ISR|=1<<7;			//进入RTC初始化模式
	while(retry&&((RTC->ISR&(1<<6))==0x00))retry--;//等待进入RTC初始化模式成功 
    if(retry==0)return 1;	//同步失败
	else return 0; 			//同步成功 
}
//RTC写入后备区域SRAM
//BKPx:后备区寄存器编号,范围:0~31
//data:要写入的数据,32位长度
void RTC_Write_BKR(u32 BKRx,u32 data)
{
	u32 temp=0;  
	temp=RTC_BASE+0x50+BKRx*4;   
	(*(u32*)temp)=data; 
}
//RTC读取后备区域SRAM
//BKPx:后备区寄存器编号,范围:0~19
//返回值:读取到的数据
u32 RTC_Read_BKR(u32 BKRx)
{
	u32 temp=0; 
	temp=RTC_BASE+0x50+BKRx*4;  
	return (*(u32*)temp);		//返回读取到的值
}

//十进制转换为BCD码
//val:要转换的十进制数
//返回值:BCD码
u8 RTC_DEC2BCD(u8 val)
{
	u8 bcdhigh = 0; 
	while(val>=10)
	{
		bcdhigh++;
		val-=10;
	} 
	return ((u8)(bcdhigh<<4)|val);
}
//BCD码转换为十进制数据
//val:要转换的BCD码
//返回值:十进制数据
u8 RTC_BCD2DEC(u8 val)
{
	u8 temp=0;
	temp=(val>>4)*10;
	return (temp+(val&0X0F));
}
//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:AM/PM,0=AM/24H,1=PM.
//返回值:0,成功
//       1,进入初始化模式失败 
u8 RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	u32 temp=0;
	//关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	if(RTC_Init_Mode())return 1;//进入RTC初始化模式失败
	temp=(((u32)ampm&0X01)<<22)|((u32)RTC_DEC2BCD(hour)<<16)|((u32)RTC_DEC2BCD(min)<<8)|(RTC_DEC2BCD(sec));
	RTC->TR=temp;
	RTC->ISR&=~(1<<7);			//退出RTC初始化模式 
	return 0; 
}
//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:0,成功
//       1,进入初始化模式失败 
u8 RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	u32 temp=0;
 	//关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	if(RTC_Init_Mode())return 1;//进入RTC初始化模式失败
	temp=(((u32)week&0X07)<<13)|((u32)RTC_DEC2BCD(year)<<16)|((u32)RTC_DEC2BCD(month)<<8)|(RTC_DEC2BCD(date)); 
	RTC->DR=temp;
	RTC->ISR&=~(1<<7);			//退出RTC初始化模式 
	return 0; 
}
//获取RTC时间
//*hour,*min,*sec:小时,分钟,秒钟 
//*ampm:AM/PM,0=AM/24H,1=PM.
void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm)
{
	u32 temp=0;
 	while(RTC_Wait_Synchro());	//等待同步  	 
	temp=RTC->TR;
	*hour=RTC_BCD2DEC((temp>>16)&0X3F);
	*min=RTC_BCD2DEC((temp>>8)&0X7F);
	*sec=RTC_BCD2DEC(temp&0X7F);
	*ampm=temp>>22; 
}
//获取RTC日期
//*year,*mon,*date:年,月,日
//*week:星期
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week)
{
	u32 temp=0;
 	while(RTC_Wait_Synchro());	//等待同步  	 
	temp=RTC->DR;
	*year=RTC_BCD2DEC((temp>>16)&0XFF);
	*month=RTC_BCD2DEC((temp>>8)&0X1F);
	*date=RTC_BCD2DEC(temp&0X3F);
	*week=(temp>>13)&0X07; 
}
//RTC初始化
//默认尝试使用LSE,当LSE启动失败后,切换为LSI.
//通过BKP寄存器0的值,可以判断RTC使用的是LSE/LSI:
//当BKP0==0X5050时,使用的是LSE
//当BKP0==0X5051时,使用的是LSI
//返回值:0,初始化成功;
//       1,进入初始化模式失败;
//注意:切换LSI/LSE将导致时间/日期丢失,切换后需重新设置.
u8 RTC_Init(void)
{  
	u16 ssr;
	u16 bkpflag=0;
	u16 retry=200; 
	u32 tempreg=0; 
	PWR->CR1|=1<<8;					//DBP=1,后备区域写使能 
	bkpflag=RTC_Read_BKR(0);		//读取BKP0的值
	if(bkpflag!=0X5050)				//之前使用的不是LSE
	{ 
		RCC->CSR|=1<<0;				//LSI总是使能
		while(!(RCC->CSR&0x02));	//等待LSI就绪 
		RCC->BDCR|=1<<0;			//尝试开启LSE 
		while(retry&&((RCC->BDCR&0X02)==0))//等待LSE准备好
		{
			retry--;
			delay_ms(5);
		}
		tempreg=RCC->BDCR;			//读取BDCR的值
		tempreg&=~(3<<8);			//清零8/9位 
		if(retry==0)tempreg|=1<<9;	//LSE开启失败,启动LSI. 
		else tempreg|=1<<8;			//选择LSE,作为RTC时钟   
		tempreg|=1<<15;				//使能RTC时钟 
		RCC->BDCR=tempreg;			//重新设置BDCR寄存器
 		//关闭RTC寄存器写保护
		RTC->WPR=0xCA;
		RTC->WPR=0x53; 
		RTC->CR=0;
		if(RTC_Init_Mode())
		{
			RCC->BDCR=1<<16;		//复位BDCR 
			delay_ms(10);
			RCC->BDCR=0;			// 结束复位
			return 2;				//进入RTC初始化模式失败
		}
		RTC->PRER=0XFF;				//RTC同步分频系数(0~7FFF),必须先设置同步分频,再设置异步分频,Frtc=Fclks/((Sprec+1)*(Asprec+1))
		RTC->PRER|=0X7F<<16;		//RTC异步分频系数(1~0X7F)
		RTC->CR&=~(1<<6);			//RTC设置为,24小时格式
		RTC->ISR&=~(1<<7);			//退出RTC初始化模式
		RTC->WPR=0xFF;				//使能RTC寄存器写保护  
		if(bkpflag!=0X5050)			//BKP0的内容既不是0X5050,也不是0X5051,说明是第一次配置,需要设置时间日期.
		{
			RTC_Set_Time(16,20,00,0);	//设置时间
			RTC_Set_Date(21,4,8,3);	//设置日期
		}
		if(retry==0)RTC_Write_BKR(0,0X5050);	//标记已经初始化过了,使用LSI
		else RTC_Write_BKR(0,0X5050);			//标记已经初始化过了,使用LSE
	}else
	{
		retry=10;		//连续10次SSR的值都没变化,则LSE死了.
		ssr=RTC->SSR;	//读取初始值
		while(retry)	//检测ssr寄存器的动态,来判断LSE是否正常
		{ 
			delay_ms(10);
			if(ssr==RTC->SSR)retry--;	//对比
			else break;
		}  
		if(retry==0)	//LSE挂了,清除配置等待下次进入重新设置
		{
			RTC_Write_BKR(0,0XFFFF);	//标记错误的值
			RCC->BDCR=1<<16;			//复位BDCR 
			delay_ms(10);
			RCC->BDCR=0;				//结束复位
		}
	}
	//RTC_Set_WakeUp(4,0);				//配置WAKE UP中断,1秒钟中断一次 
	return 0;
}

u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//year,month,day：公历年月日 
//返回值：星期号(1~7,代表周1~周日)																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	temp2%=7;
	if(temp2==0)temp2=7;
	return temp2;
}	













