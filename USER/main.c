#include "sys.h"
#include "usart.h" 

int main(void)
{
	HAL_Init();				        		//��ʼ��HAL��
	Stm32_Clock_Init(160,5,2,4);  		    //����ʱ��,400Mhz 
	USART6_Init(115200);
	
	printf("Hello World!\n");
	
	return 0;
}
