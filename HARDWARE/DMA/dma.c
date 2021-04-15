#include "dma.h"
#include "usart.h"

DMA_HandleTypeDef UART2TxDMA_Handler; //DMA句柄

void DMA_Config(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE(); //DMA1时钟使能

    //Tx DMA配置
    UART2TxDMA_Handler.Instance = DMA1_Stream6;                        //数据流选择
    UART2TxDMA_Handler.Init.Request = DMA_REQUEST_USART2_TX;           //USART6发送DMA
    UART2TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;          //存储器到外设
    UART2TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;              //外设非增量模式
    UART2TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;                  //存储器增量模式
    UART2TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; //外设数据长度:8位
    UART2TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    //存储器数据长度:8位
    UART2TxDMA_Handler.Init.Mode = DMA_NORMAL;                         //外设流控模式
    UART2TxDMA_Handler.Init.Priority = DMA_PRIORITY_MEDIUM;            //中等优先级
    UART2TxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    UART2TxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    UART2TxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;    //存储器突发单次传输
    UART2TxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE; //外设突发单次传输

    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn,0,0);
	HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    
    HAL_DMA_DeInit(&UART2TxDMA_Handler);
    HAL_DMA_Init(&UART2TxDMA_Handler);
    __HAL_LINKDMA(&UART2_Handler, hdmatx, UART2TxDMA_Handler); //将DMA与USART6联系起来(发送DMA)

    //Rx DMA配置

}

void DMA1_Stream6_IRQHandler(void)
{
    //if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) != RESET)
    __HAL_UART_CLEAR_IDLEFLAG(&UART2_Handler);  //清除标志位 
    HAL_DMA_IRQHandler(&UART2TxDMA_Handler);
    printf("DMA1_Stream6_IRQHandler\n");
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    printf("HAL_UART_TxCpltCallback\n");
}
