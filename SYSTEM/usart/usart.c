#include "usart.h"
//#include "delay.h"

#define  USART2_RBUFF_SIZE            10 
uint8_t  RX_BUFF[USART2_RBUFF_SIZE] = {0};


uint8_t rec_buf[1];
uint16_t rec16[1];

UART_HandleTypeDef UART1_Handler; //UART1句柄
UART_HandleTypeDef UART2_Handler; //UART2句柄
UART_HandleTypeDef UART3_Handler; //UART3句柄
UART_HandleTypeDef UART6_Handler; //UART6句柄

DMA_HandleTypeDef DMA_Handle;

void USART1_Init(u32 bound)
{
    //UART 初始化设置
    UART1_Handler.Instance = USART1;                    //USART1
    UART1_Handler.Init.BaudRate = bound;                //波特率
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;      //一个停止位
    UART1_Handler.Init.Parity = UART_PARITY_NONE;       //无奇偶校验位
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;          //收发模式
    HAL_UART_Init(&UART1_Handler);                      //HAL_UART_Init()会使能UART1

    //HAL_UART_Receive_IT(&UART1_Handler, rec_buf, 1);  // 使用DMA+IDLE中断模式
}

void USART2_Init(uint32_t bound)
{
    //UART 初始化设置
    UART2_Handler.Instance = USART2;                    //USART2
    UART2_Handler.Init.BaudRate = bound;                //波特率
    UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART2_Handler.Init.StopBits = UART_STOPBITS_1;      //一个停止位
    UART2_Handler.Init.Parity = UART_PARITY_NONE;       //无奇偶校验位
    UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART2_Handler.Init.Mode = UART_MODE_TX_RX;          //收发模式
    HAL_UART_Init(&UART2_Handler);                      //HAL_UART_Init()会使能UART2

    HAL_UART_Receive_IT(&UART2_Handler, rec_buf, 1);
}

void USART3_Init(uint32_t bound)
{
    //UART 初始化设置
    UART3_Handler.Instance = USART3;                    //USART3
    UART3_Handler.Init.BaudRate = bound;                //波特率
    UART3_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART3_Handler.Init.StopBits = UART_STOPBITS_1;      //一个停止位
    UART3_Handler.Init.Parity = UART_PARITY_NONE;       //无奇偶校验位
    UART3_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART3_Handler.Init.Mode = UART_MODE_TX_RX;          //收发模式
    HAL_UART_Init(&UART3_Handler);                      //HAL_UART_Init()会使能UART3

    HAL_UART_Receive_IT(&UART3_Handler, rec_buf, 1);
}

void USART6_Init(uint32_t bound)
{
    //UART 初始化设置
    UART6_Handler.Instance = USART6;                    //USART6
    UART6_Handler.Init.BaudRate = bound;                //波特率
    UART6_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART6_Handler.Init.StopBits = UART_STOPBITS_1;      //一个停止位
    UART6_Handler.Init.Parity = UART_PARITY_NONE;       //无奇偶校验位
    UART6_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART6_Handler.Init.Mode = UART_MODE_TX_RX;          //收发模式
    HAL_UART_Init(&UART6_Handler);                      //HAL_UART_Init()会使能UART6

    HAL_UART_Receive_IT(&UART6_Handler, rec_buf, 1);

    USART6->CR1 &= ~(1<<0);    //禁止USART6

    USART6->CR1 |= (1<<11);    //WAKE: 接收器唤醒方式 ==> 地址标记
    USART6->CR2 &= ~0xFF000000;   //地址
    USART6->CR2 |= 0xC1000000;
	USART6->CR2 |= (1<<4);    //7位地址检测
    USART6->CR1 |= (1<<13);    //静默模式使能
    USART6->RQR |= (1<<2);

    USART6->CR1 |= (1<<0);    //使能USART6

    // printf("USART6->CR1 = 0x%X\n", USART6->CR1);
    // printf("USART6->CR2 = 0x%X\n", USART6->CR2);
    // printf("USART6->ISR = 0x%X\n", USART6->ISR);

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitSturct;

    if (huart->Instance == USART1) //如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();  //使能GPIOB时钟
        __HAL_RCC_USART1_CLK_ENABLE(); //使能USART1时钟

        GPIO_InitSturct.Pin = GPIO_PIN_14;             //PB14
        GPIO_InitSturct.Mode = GPIO_MODE_AF_PP;       //复用推挽输出
        GPIO_InitSturct.Pull = GPIO_PULLUP;           //上拉
        GPIO_InitSturct.Speed = GPIO_SPEED_FREQ_HIGH; //高速
        GPIO_InitSturct.Alternate = GPIO_AF4_USART1;  //复用为USART1
        HAL_GPIO_Init(GPIOB, &GPIO_InitSturct);       //初始化PB14

        GPIO_InitSturct.Pin = GPIO_PIN_15;      //PB15
        HAL_GPIO_Init(GPIOB, &GPIO_InitSturct); //初始化PB15

#if EN_USART1_RX
        HAL_NVIC_EnableIRQ(USART1_IRQn);         //使能USART1中断通道
        HAL_NVIC_SetPriority(USART1_IRQn, 6, 0); //抢占优先级6，子优先级0
#endif
    }

    if (huart->Instance == USART2) //如果是串口2，进行串口2 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();  //使能GPIOA时钟
        __HAL_RCC_USART2_CLK_ENABLE(); //使能USART2时钟

        GPIO_InitSturct.Pin = GPIO_PIN_2;             //PA2
        GPIO_InitSturct.Mode = GPIO_MODE_AF_PP;       //复用推挽输出
        GPIO_InitSturct.Pull = GPIO_PULLUP;           //上拉
        GPIO_InitSturct.Speed = GPIO_SPEED_FREQ_HIGH; //高速
        GPIO_InitSturct.Alternate = GPIO_AF7_USART2;  //复用为USART2
        HAL_GPIO_Init(GPIOA, &GPIO_InitSturct);       //初始化PA2

        GPIO_InitSturct.Pin = GPIO_PIN_3;       //PA3
        HAL_GPIO_Init(GPIOA, &GPIO_InitSturct); //初始化PA3

#if EN_USART2_RX
        HAL_NVIC_EnableIRQ(USART2_IRQn);         //使能USART2中断通道
        HAL_NVIC_SetPriority(USART2_IRQn, 6, 0); //抢占优先级6，子优先级0
#endif
    }

    if (huart->Instance == USART3) //如果是串口3，进行串口3 MSP初始化
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();  //使能GPIOB时钟
        __HAL_RCC_USART3_CLK_ENABLE(); //使能USART3时钟

        GPIO_InitSturct.Pin = GPIO_PIN_10;             //PB10
        GPIO_InitSturct.Mode = GPIO_MODE_AF_PP;       //复用推挽输出
        GPIO_InitSturct.Pull = GPIO_PULLUP;           //上拉
        GPIO_InitSturct.Speed = GPIO_SPEED_FREQ_HIGH; //高速
        GPIO_InitSturct.Alternate = GPIO_AF7_USART3;  //复用为USART3
        HAL_GPIO_Init(GPIOB, &GPIO_InitSturct);       //初始化PB10

        GPIO_InitSturct.Pin = GPIO_PIN_11;       //PB11
        HAL_GPIO_Init(GPIOB, &GPIO_InitSturct); //初始化PB11

#if EN_USART3_RX
        HAL_NVIC_EnableIRQ(USART3_IRQn);         //使能USART2中断通道
        HAL_NVIC_SetPriority(USART3_IRQn, 3, 3); //抢占优先级3，子优先级3
#endif
    }

    if (huart->Instance == USART6) //如果是串口6，进行串口6 MSP初始化
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();  //使能GPIOC时钟
        __HAL_RCC_USART6_CLK_ENABLE(); //使能USART6时钟

        GPIO_InitSturct.Pin = GPIO_PIN_6;             //PC6
        GPIO_InitSturct.Mode = GPIO_MODE_AF_OD;       //复用推挽输出
        //GPIO_InitSturct.Pull = GPIO_PULLUP;           //上拉
        GPIO_InitSturct.Speed = GPIO_SPEED_FREQ_HIGH; //高速
        GPIO_InitSturct.Alternate = GPIO_AF7_USART3;  //复用为USART6
        HAL_GPIO_Init(GPIOC, &GPIO_InitSturct);       //初始化PC6

        GPIO_InitSturct.Pin = GPIO_PIN_7;       //PC7
        HAL_GPIO_Init(GPIOC, &GPIO_InitSturct); //初始化PC7

#if EN_USART6_RX
        HAL_NVIC_EnableIRQ(USART6_IRQn);         //使能USART2中断通道
        HAL_NVIC_SetPriority(USART6_IRQn, 3, 3); //抢占优先级3，子优先级3
#endif
    }

}

void USART1_DMA_Config(void)
{
	/*开启DMA时钟*/
	__HAL_RCC_DMA1_CLK_ENABLE(); //DMA1时钟使能
	__HAL_RCC_DMA2_CLK_ENABLE(); //DMA2时钟使能
	
    //Rx DMA配置
    DMA_Handle.Instance=DMA2_Stream7;                           //数据流选择
	DMA_Handle.Init.Request=DMA_REQUEST_USART1_RX;				//USART1发送DMA
    DMA_Handle.Init.Direction=DMA_PERIPH_TO_MEMORY;             //存储器到外设
    DMA_Handle.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
    DMA_Handle.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
    DMA_Handle.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
    DMA_Handle.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //存储器数据长度:8位
    DMA_Handle.Init.Mode=DMA_NORMAL;                            //外设流控模式
    DMA_Handle.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
    DMA_Handle.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    DMA_Handle.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    DMA_Handle.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
    DMA_Handle.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
	
	HAL_DMA_DeInit(&DMA_Handle);
    HAL_DMA_Init(&DMA_Handle);
	
	__HAL_LINKDMA(&UART1_Handler, hdmarx, DMA_Handle); 
	
	HAL_UART_Receive_DMA(&UART1_Handler, RX_BUFF, USART2_RBUFF_SIZE);
	
	__HAL_UART_CLEAR_IT(&UART1_Handler, UART_CLEAR_IDLEF);
	__HAL_UART_ENABLE_IT(&UART1_Handler, UART_IT_IDLE);  
}

void USART1_IRQHandler(void)
{
    uint32_t i = 0;
	
	if((READ_REG(UART1_Handler.Instance->ISR)& USART_ISR_IDLE) != RESET)
	{
		__HAL_DMA_DISABLE(&DMA_Handle);      
		__HAL_DMA_CLEAR_FLAG(&DMA_Handle, DMA_FLAG_TCIF3_7);  
		
		__HAL_DMA_ENABLE(&DMA_Handle); 
		
		for (i = 0; i < 10; i++)
		{
			printf("%x ", RX_BUFF[i]);
		}
		
		__HAL_UART_CLEAR_IT(&UART1_Handler, UART_CLEAR_IDLEF);
	}
}

void USART2_IRQHandler(void)
{
	uint32_t timeout = 0;

    HAL_UART_IRQHandler(&UART2_Handler); //调用HAL库中断处理公用函数

    timeout = 0;

    while (HAL_UART_GetState(&UART2_Handler) != HAL_UART_STATE_READY) //等待就绪
    {
        timeout++; ////超时处理
        if (timeout > HAL_MAX_DELAY)
            break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART2_Handler, (uint8_t *)rec_buf, 1) != HAL_OK) //一次处理完成之后，重新开启中断并设置RxXferCount为1
    {
        timeout++; //超时处理
        if (timeout > HAL_MAX_DELAY)
            break;
    }
}

void USART3_IRQHandler(void)
{
    uint32_t timeout = 0;

    HAL_UART_IRQHandler(&UART3_Handler); //调用HAL库中断处理公用函数

    timeout = 0;

    while (HAL_UART_GetState(&UART3_Handler) != HAL_UART_STATE_READY) //等待就绪
    {
        timeout++; ////超时处理
        if (timeout > HAL_MAX_DELAY)
            break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART3_Handler, (uint8_t *)rec_buf, 1) != HAL_OK) //一次处理完成之后，重新开启中断并设置RxXferCount为1
    {
        timeout++; //超时处理
        if (timeout > HAL_MAX_DELAY)
            break;
    }
}

void USART6_IRQHandler(void)
{
    uint32_t timeout = 0;
	
	//printf("USART6_IRQHandler\n");

    HAL_UART_IRQHandler(&UART6_Handler); //调用HAL库中断处理公用函数

    timeout = 0;

    while (HAL_UART_GetState(&UART6_Handler) != HAL_UART_STATE_READY) //等待就绪
    {
        timeout++; ////超时处理
        if (timeout > HAL_MAX_DELAY)
            break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART6_Handler, (uint8_t *)rec_buf, 1) != HAL_OK) //一次处理完成之后，重新开启中断并设置RxXferCount为1
    {
        timeout++; //超时处理
        if (timeout > HAL_MAX_DELAY)
            break;
    }
}

void USART2_TransmitString(const char *cString)
{
    for (; *cString != '\0';)
    {
        USART2->TDR = *cString++;
        while ((USART2->ISR & 0X40) == 0)
            ; //等待发送结束
    }
}

void USART3_TransmitArray(const uint8_t *Array, uint8_t len, uint8_t addr)
{
    USART3->TDR = addr | 0x100;    //第9位为1表示地址
    while ((USART3->ISR & 0X40) == 0)
            ; //等待发送结束
    
    while(len--)
    {
        USART3->TDR = *Array++;
        while ((USART3->ISR & 0X40) == 0)
            ; //等待发送结束
    }
}

void USART6_TransmitArray(const uint8_t *Array, uint8_t len)
{
    while(len--)
    {
        USART6->TDR = *Array++;
        while ((USART6->ISR & 0X40) == 0)
            ; //等待发送结束
    }
}


#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while ((USART6->ISR & 0X40) == 0)
        ; //循环发送,直到发送完毕
    USART6->TDR = (uint8_t)ch;
    return ch;
}
