#include "DTU.h"

//char rxUart2[20];
//uint8_t failue = 0;
//uint16_t tLimt_rxTime = 0;
//uint8_t download_ok = FALSE;
//uint8_t rxend = FALSE;
//uint32_t cntData = 0;

//volatile uint8_t countRx2;

///************************************************************************************
//* º¯ Êı Ãû £ºTime_up_Return
//* º¯Êı½éÉÜ£ºÑÓÊ±º¯Êı£¬µ±ÑÓÊ±´ïµ½n*100msÊ±·µ»Ø0£¬ÓÃÓÚÍË³öwhileÑ­»·
//* ÊäÈë²ÎÊı£ºn £¨ÑÓÊ±Ê±¼ä£ºn*100ms£©
//* ·µ »Ø Öµ £ºÎŞ
//************************************************************************************/
//uint8_t Time_up_Return(uint16_t n)
//{
//	if (tLimt_rxTime == n)
//	{
//		failue = 1;
//		return 0;
//	}
//	else
//	{
//		return 1;
//	}
//}

///************************************************************************************
//* º¯ Êı Ãû £ºDTU_Configuration
//* º¯Êı½éÉÜ £ºDTUÅäÖÃº¯Êı£¬ÓÃÀ´ÉèÖÃDTUµÄTCPÍ¨µÀÒÔÁ¬½ÓÔÆ·şÎñÆ÷
//* ÊäÈë²ÎÊı £ºÎŞ
//* ·µ »Ø Öµ £ºÎŞ
//************************************************************************************/
//uint8_t DTU_Configuration(void)
//{
//	const char *pdata = rxUart2;
//	const char *enter_config = "AT+ENTERCFG\r\n";
//	const char *set_ip = "AT+SET=9,124.70.191.189\r\n";
//	const char *set_port = "AT+SET=10,22\r\n";
//	const char *set_datasource = "AT+SET=12,2\r\n";
//	const char *exit_config = "AT+EXITCFG\r\n";

//	memset(rxUart2, '\0', sizeof(rxUart2)); //Çå³ı»º´æÊı×é
//	countRx2 = 0;

//	USART2_TransmitString(enter_config); //½øÈëÅäÖÃÄ£Ê½
//	tLimt_rxTime = 0;
//	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(300)))
//		;		//30sÄÚÎ´·µ»ØOK£¬ÍË³öµÈ´ı
//	if (failue) //ÈÎÒâÒ»²½ÅäÖÃ´íÎóÖ±½ÓÍË³ö£¬±ÜÃâ²»±ØÒªµÄ²Ù×÷
//	{
//		return 0;
//	}
//	memset(rxUart2, '\0', sizeof(rxUart2));
//	countRx2 = 0;
//	//_delay_ms(100);

//	USART2_TransmitString(set_ip); //ÉèÖÃÊı¾İÖĞĞÄ2µÄipµØÖ·
//	tLimt_rxTime = 0;
//	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
//		;		//10sÄÚÎ´·µ»ØOK£¬ÍË³öµÈ´ı
//	if (failue) //ÈÎÒâÒ»²½ÅäÖÃ´íÎóÖ±½ÓÍË³ö£¬±ÜÃâ²»±ØÒªµÄ²Ù×÷
//	{
//		return 0;
//	}
//	memset(rxUart2, '\0', sizeof(rxUart2));
//	countRx2 = 0;
//	//_delay_ms(100);

//	USART2_TransmitString(set_port); //ÉèÖÃÊı¾İÖĞĞÄ2µÄ¶Ë¿Ú
//	tLimt_rxTime = 0;
//	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
//		;		//10sÄÚÎ´·µ»ØOK£¬ÍË³öµÈ´ı
//	if (failue) //ÈÎÒâÒ»²½ÅäÖÃ´íÎóÖ±½ÓÍË³ö£¬±ÜÃâ²»±ØÒªµÄ²Ù×÷
//	{
//		return 0;
//	}
//	memset(rxUart2, '\0', sizeof(rxUart2));
//	countRx2 = 0;
//	//_delay_ms(100);

//	USART2_TransmitString(set_datasource); //ÉèÖÃÊı¾İÖĞĞÄ2µÄÊı¾İÔ´
//	tLimt_rxTime = 0;
//	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
//		;		//10sÄÚÎ´·µ»ØOK£¬ÍË³öµÈ´ı
//	if (failue) //ÈÎÒâÒ»²½ÅäÖÃ´íÎóÖ±½ÓÍË³ö£¬±ÜÃâ²»±ØÒªµÄ²Ù×÷
//	{
//		return 0;
//	}
//	memset(rxUart2, '\0', sizeof(rxUart2));
//	countRx2 = 0;
//	//_delay_ms(100);

//	USART2_TransmitString(exit_config); //ÍË³öÅäÖÃÄ£Ê½
//	tLimt_rxTime = 0;
//	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
//		;		//10sÄÚÎ´·µ»ØOK£¬ÍË³öµÈ´ı
//	if (failue) //ÈÎÒâÒ»²½ÅäÖÃ´íÎóÖ±½ÓÍË³ö£¬±ÜÃâ²»±ØÒªµÄ²Ù×÷
//	{
//		return 0;
//	}
//	memset(rxUart2, '\0', sizeof(rxUart2));
//	countRx2 = 0;

//	return 1;
//}

//void Connect_to_Server(void)
//{
//	const char *stationcode = "014469014469\r\n";    //³µÕ¾´úÂë

//	while((strstr(rxUart2, "OK") == NULL))
//	{
//		USART2_TransmitString(stationcode);  //DTUÏÈ´«ËÍ³µÕ¾´úÂë
//		delay_ms(1000);
//	}
//	USART2_TransmitString("OK\r\n");  //DTU»Ø¸´OK,ËµÃ÷³É¹¦Á¬ÉÏÔÆ·şÎñÆ÷,½ÓÏÂÀ´¿ªÊ¼ÏÂÔØµØÃæÊı¾İ
//	memset(rxUart2, '\0', sizeof(rxUart2));
//	countRx2=0;

//	while((strstr(rxUart2, "END") == NULL))    //Êı¾İÏÂÔØÍê³É,Í¨Öª·şÎñÆ÷
//	{
//		;   //µÈ´ıËùÓĞÊı¾İ·¢ËÍÍê³É
//	}
//	download_ok = TRUE;
//	rxend = TRUE;
//	USART2_TransmitString("OK\r\n");
//	memset(rxUart2, '\0', sizeof(rxUart2));
//	countRx2=0;
//}

const char* cfg[12] = {
	"loc.starcart.cn\r\n",    // NTRIP CasteråŸŸåæˆ–IP
	"2101\r\n",               // NTRIP Casterç«¯å£
	"1\r\n",                  // NTRIPç±»å‹[1:Client/2:Server] 
	"abc234567\r\n",          // NTRIPè´¦å·
	"123456\r\n",             // NTRIPå¯†ç 
	"\r\n",                   // æŒ‚è½½ç‚¹[1:RTCM32_GGB/2:RTCM30_GG]
	"NETRTK32\r\n",           // è‡ªå®šä¹‰æŒ‚è½½ç‚¹
	"1\r\n",                  // æ•°æ®æº[0:æ— /1:COM1/2:COM2]
	"124.70.191.189\r\n",     // æ•°æ®ä¸­å¿ƒ2åŸŸåæˆ–IP
	"22\r\n",                 // æ•°æ®ä¸­å¿ƒ2ç«¯å£
	"N\r\n",                  // ä¸­å¿ƒ2è¿æ¥mServer[Y/N]
	"2\r\n",                  // ä¸­å¿ƒ2æ•°æ®æº[0:æ— /1:COM1/2:COM2]
}; 

