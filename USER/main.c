#include "sys.h"
#include "usart.h" 

int main(void)
{
	HAL_Init();				        		//
	Stm32_Clock_Init(160,5,2,4);  		    //时钟频率工作在400Mhz 
	USART6_Init(115200);
	
	printf("Hello World!\n");
	
	return 0;
}
