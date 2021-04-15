#include "DTU.h"

char rxUart2[20];
uint8_t failue = 0;
uint16_t tLimt_rxTime = 0;
uint8_t download_ok = FALSE;
uint8_t rxend = FALSE;
uint32_t cntData = 0;

volatile uint8_t countRx2;

/************************************************************************************
* 函 数 名 ：Time_up_Return
* 函数介绍：延时函数，当延时达到n*100ms时返回0，用于退出while循环
* 输入参数：n （延时时间：n*100ms）
* 返 回 值 ：无
************************************************************************************/
uint8_t Time_up_Return(uint16_t n)
{
	if (tLimt_rxTime == n)
	{
		failue = 1;
		return 0;
	}
	else
	{
		return 1;
	}
}

/************************************************************************************
* 函 数 名 ：DTU_Configuration
* 函数介绍 ：DTU配置函数，用来设置DTU的TCP通道以连接云服务器
* 输入参数 ：无
* 返 回 值 ：无
************************************************************************************/
uint8_t DTU_Configuration(void)
{
	const char *pdata = rxUart2;
	const char *enter_config = "AT+ENTERCFG\r\n";
	const char *set_ip = "AT+SET=9,124.70.191.189\r\n";
	const char *set_port = "AT+SET=10,22\r\n";
	const char *set_datasource = "AT+SET=12,2\r\n";
	const char *exit_config = "AT+EXITCFG\r\n";

	memset(rxUart2, '\0', sizeof(rxUart2)); //清除缓存数组
	countRx2 = 0;

	USART2_TransmitString(enter_config); //进入配置模式
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(300)))
		;		//30s内未返回OK，退出等待
	if (failue) //任意一步配置错误直接退出，避免不必要的操作
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;
	//_delay_ms(100);

	USART2_TransmitString(set_ip); //设置数据中心2的ip地址
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
		;		//10s内未返回OK，退出等待
	if (failue) //任意一步配置错误直接退出，避免不必要的操作
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;
	//_delay_ms(100);

	USART2_TransmitString(set_port); //设置数据中心2的端口
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
		;		//10s内未返回OK，退出等待
	if (failue) //任意一步配置错误直接退出，避免不必要的操作
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;
	//_delay_ms(100);

	USART2_TransmitString(set_datasource); //设置数据中心2的数据源
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
		;		//10s内未返回OK，退出等待
	if (failue) //任意一步配置错误直接退出，避免不必要的操作
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;
	//_delay_ms(100);

	USART2_TransmitString(exit_config); //退出配置模式
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
		;		//10s内未返回OK，退出等待
	if (failue) //任意一步配置错误直接退出，避免不必要的操作
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;

	return 1;
}

void Connect_to_Server(void)
{
	const char *stationcode = "014469014469\r\n";    //车站代码

	while((strstr(rxUart2, "OK") == NULL))
	{
		USART2_TransmitString(stationcode);  //DTU先传送车站代码
		delay_ms(1000);
	}
	USART2_TransmitString("OK\r\n");  //DTU回复OK,说明成功连上云服务器,接下来开始下载地面数据
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2=0;

	while((strstr(rxUart2, "END") == NULL))    //数据下载完成,通知服务器
	{
		;   //等待所有数据发送完成
	}
	download_ok = TRUE;
	rxend = TRUE;
	USART2_TransmitString("OK\r\n");
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2=0;
}

