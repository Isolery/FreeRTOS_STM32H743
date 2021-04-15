#include "DTU.h"

char rxUart2[20];
uint8_t failue = 0;
uint16_t tLimt_rxTime = 0;
uint8_t download_ok = FALSE;
uint8_t rxend = FALSE;
uint32_t cntData = 0;

volatile uint8_t countRx2;

/************************************************************************************
* �� �� �� ��Time_up_Return
* �������ܣ���ʱ����������ʱ�ﵽn*100msʱ����0�������˳�whileѭ��
* ���������n ����ʱʱ�䣺n*100ms��
* �� �� ֵ ����
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
* �� �� �� ��DTU_Configuration
* �������� ��DTU���ú�������������DTU��TCPͨ���������Ʒ�����
* ������� ����
* �� �� ֵ ����
************************************************************************************/
uint8_t DTU_Configuration(void)
{
	const char *pdata = rxUart2;
	const char *enter_config = "AT+ENTERCFG\r\n";
	const char *set_ip = "AT+SET=9,124.70.191.189\r\n";
	const char *set_port = "AT+SET=10,22\r\n";
	const char *set_datasource = "AT+SET=12,2\r\n";
	const char *exit_config = "AT+EXITCFG\r\n";

	memset(rxUart2, '\0', sizeof(rxUart2)); //�����������
	countRx2 = 0;

	USART2_TransmitString(enter_config); //��������ģʽ
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(300)))
		;		//30s��δ����OK���˳��ȴ�
	if (failue) //����һ�����ô���ֱ���˳������ⲻ��Ҫ�Ĳ���
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;
	//_delay_ms(100);

	USART2_TransmitString(set_ip); //������������2��ip��ַ
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
		;		//10s��δ����OK���˳��ȴ�
	if (failue) //����һ�����ô���ֱ���˳������ⲻ��Ҫ�Ĳ���
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;
	//_delay_ms(100);

	USART2_TransmitString(set_port); //������������2�Ķ˿�
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
		;		//10s��δ����OK���˳��ȴ�
	if (failue) //����һ�����ô���ֱ���˳������ⲻ��Ҫ�Ĳ���
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;
	//_delay_ms(100);

	USART2_TransmitString(set_datasource); //������������2������Դ
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
		;		//10s��δ����OK���˳��ȴ�
	if (failue) //����һ�����ô���ֱ���˳������ⲻ��Ҫ�Ĳ���
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;
	//_delay_ms(100);

	USART2_TransmitString(exit_config); //�˳�����ģʽ
	tLimt_rxTime = 0;
	while ((strstr(pdata, "OK") == NULL) && (Time_up_Return(100)))
		;		//10s��δ����OK���˳��ȴ�
	if (failue) //����һ�����ô���ֱ���˳������ⲻ��Ҫ�Ĳ���
	{
		return 0;
	}
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2 = 0;

	return 1;
}

void Connect_to_Server(void)
{
	const char *stationcode = "014469014469\r\n";    //��վ����

	while((strstr(rxUart2, "OK") == NULL))
	{
		USART2_TransmitString(stationcode);  //DTU�ȴ��ͳ�վ����
		delay_ms(1000);
	}
	USART2_TransmitString("OK\r\n");  //DTU�ظ�OK,˵���ɹ������Ʒ�����,��������ʼ���ص�������
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2=0;

	while((strstr(rxUart2, "END") == NULL))    //�����������,֪ͨ������
	{
		;   //�ȴ��������ݷ������
	}
	download_ok = TRUE;
	rxend = TRUE;
	USART2_TransmitString("OK\r\n");
	memset(rxUart2, '\0', sizeof(rxUart2));
	countRx2=0;
}

