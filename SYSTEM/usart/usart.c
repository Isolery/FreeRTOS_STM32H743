#include "usart.h"
//#include "delay.h"

uint8_t rec_buf[1];
uint16_t rec16[1];

UART_HandleTypeDef UART1_Handler; //UART1���
UART_HandleTypeDef UART2_Handler; //UART2���
UART_HandleTypeDef UART3_Handler; //UART3���
UART_HandleTypeDef UART6_Handler; //UART6���

void USART1_Init(u32 bound)
{
    //UART ��ʼ������
    UART1_Handler.Instance = USART1;                    //USART1
    UART1_Handler.Init.BaudRate = bound;                //������
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; //�ֳ�Ϊ8λ���ݸ�ʽ
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;      //һ��ֹͣλ
    UART1_Handler.Init.Parity = UART_PARITY_NONE;       //����żУ��λ
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //��Ӳ������
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;          //�շ�ģʽ
    HAL_UART_Init(&UART1_Handler);                      //HAL_UART_Init()��ʹ��UART1

    HAL_UART_Receive_IT(&UART1_Handler, rec_buf, 1);
}

void USART2_Init(uint32_t bound)
{
    //UART ��ʼ������
    UART2_Handler.Instance = USART2;                    //USART2
    UART2_Handler.Init.BaudRate = bound;                //������
    UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B; //�ֳ�Ϊ8λ���ݸ�ʽ
    UART2_Handler.Init.StopBits = UART_STOPBITS_1;      //һ��ֹͣλ
    UART2_Handler.Init.Parity = UART_PARITY_NONE;       //����żУ��λ
    UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //��Ӳ������
    UART2_Handler.Init.Mode = UART_MODE_TX_RX;          //�շ�ģʽ
    HAL_UART_Init(&UART2_Handler);                      //HAL_UART_Init()��ʹ��UART2

    HAL_UART_Receive_IT(&UART2_Handler, rec_buf, 1);
}

void USART3_Init(uint32_t bound)
{
    //UART ��ʼ������
    UART3_Handler.Instance = USART3;                    //USART3
    UART3_Handler.Init.BaudRate = bound;                //������
    UART3_Handler.Init.WordLength = UART_WORDLENGTH_8B; //�ֳ�Ϊ8λ���ݸ�ʽ
    UART3_Handler.Init.StopBits = UART_STOPBITS_1;      //һ��ֹͣλ
    UART3_Handler.Init.Parity = UART_PARITY_NONE;       //����żУ��λ
    UART3_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //��Ӳ������
    UART3_Handler.Init.Mode = UART_MODE_TX_RX;          //�շ�ģʽ
    HAL_UART_Init(&UART3_Handler);                      //HAL_UART_Init()��ʹ��UART3

    HAL_UART_Receive_IT(&UART3_Handler, rec_buf, 1);
}

void USART6_Init(uint32_t bound)
{
    //UART ��ʼ������
    UART6_Handler.Instance = USART6;                    //USART6
    UART6_Handler.Init.BaudRate = bound;                //������
    UART6_Handler.Init.WordLength = UART_WORDLENGTH_8B; //�ֳ�Ϊ8λ���ݸ�ʽ
    UART6_Handler.Init.StopBits = UART_STOPBITS_1;      //һ��ֹͣλ
    UART6_Handler.Init.Parity = UART_PARITY_NONE;       //����żУ��λ
    UART6_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //��Ӳ������
    UART6_Handler.Init.Mode = UART_MODE_TX_RX;          //�շ�ģʽ
    HAL_UART_Init(&UART6_Handler);                      //HAL_UART_Init()��ʹ��UART6

    HAL_UART_Receive_IT(&UART6_Handler, rec_buf, 1);

    USART6->CR1 &= ~(1<<0);    //��ֹUSART6

    USART6->CR1 |= (1<<11);    //WAKE: ���������ѷ�ʽ ==> ��ַ���
    USART6->CR2 &= ~0xFF000000;   //��ַ
    USART6->CR2 |= 0xC1000000;
	USART6->CR2 |= (1<<4);    //7λ��ַ���
    USART6->CR1 |= (1<<13);    //��Ĭģʽʹ��
    USART6->RQR |= (1<<2);

    USART6->CR1 |= (1<<0);    //ʹ��USART6

    // printf("USART6->CR1 = 0x%X\n", USART6->CR1);
    // printf("USART6->CR2 = 0x%X\n", USART6->CR2);
    // printf("USART6->ISR = 0x%X\n", USART6->ISR);

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitSturct;

    if (huart->Instance == USART1) //����Ǵ���1�����д���1 MSP��ʼ��
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();  //ʹ��GPIOBʱ��
        __HAL_RCC_USART1_CLK_ENABLE(); //ʹ��USART1ʱ��

        GPIO_InitSturct.Pin = GPIO_PIN_14;             //PB14
        GPIO_InitSturct.Mode = GPIO_MODE_AF_PP;       //�����������
        GPIO_InitSturct.Pull = GPIO_PULLUP;           //����
        GPIO_InitSturct.Speed = GPIO_SPEED_FREQ_HIGH; //����
        GPIO_InitSturct.Alternate = GPIO_AF4_USART1;  //����ΪUSART1
        HAL_GPIO_Init(GPIOB, &GPIO_InitSturct);       //��ʼ��PB14

        GPIO_InitSturct.Pin = GPIO_PIN_15;      //PB15
        HAL_GPIO_Init(GPIOB, &GPIO_InitSturct); //��ʼ��PB15

#if EN_USART1_RX
        HAL_NVIC_EnableIRQ(USART1_IRQn);         //ʹ��USART1�ж�ͨ��
        HAL_NVIC_SetPriority(USART1_IRQn, 1, 1); //��ռ���ȼ�1�������ȼ�1
#endif
    }

    if (huart->Instance == USART2) //����Ǵ���2�����д���2 MSP��ʼ��
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();  //ʹ��GPIOAʱ��
        __HAL_RCC_USART2_CLK_ENABLE(); //ʹ��USART2ʱ��

        GPIO_InitSturct.Pin = GPIO_PIN_2;             //PA2
        GPIO_InitSturct.Mode = GPIO_MODE_AF_PP;       //�����������
        GPIO_InitSturct.Pull = GPIO_PULLUP;           //����
        GPIO_InitSturct.Speed = GPIO_SPEED_FREQ_HIGH; //����
        GPIO_InitSturct.Alternate = GPIO_AF7_USART2;  //����ΪUSART2
        HAL_GPIO_Init(GPIOA, &GPIO_InitSturct);       //��ʼ��PA2

        GPIO_InitSturct.Pin = GPIO_PIN_3;       //PA3
        HAL_GPIO_Init(GPIOA, &GPIO_InitSturct); //��ʼ��PA3

#if EN_USART2_RX
        HAL_NVIC_EnableIRQ(USART2_IRQn);         //ʹ��USART2�ж�ͨ��
        HAL_NVIC_SetPriority(USART2_IRQn, 3, 3); //��ռ���ȼ�3�������ȼ�3
#endif
    }

    if (huart->Instance == USART3) //����Ǵ���3�����д���3 MSP��ʼ��
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();  //ʹ��GPIOBʱ��
        __HAL_RCC_USART3_CLK_ENABLE(); //ʹ��USART3ʱ��

        GPIO_InitSturct.Pin = GPIO_PIN_10;             //PB10
        GPIO_InitSturct.Mode = GPIO_MODE_AF_PP;       //�����������
        GPIO_InitSturct.Pull = GPIO_PULLUP;           //����
        GPIO_InitSturct.Speed = GPIO_SPEED_FREQ_HIGH; //����
        GPIO_InitSturct.Alternate = GPIO_AF7_USART3;  //����ΪUSART3
        HAL_GPIO_Init(GPIOB, &GPIO_InitSturct);       //��ʼ��PB10

        GPIO_InitSturct.Pin = GPIO_PIN_11;       //PB11
        HAL_GPIO_Init(GPIOB, &GPIO_InitSturct); //��ʼ��PB11

#if EN_USART3_RX
        HAL_NVIC_EnableIRQ(USART3_IRQn);         //ʹ��USART2�ж�ͨ��
        HAL_NVIC_SetPriority(USART3_IRQn, 3, 3); //��ռ���ȼ�3�������ȼ�3
#endif
    }

    if (huart->Instance == USART6) //����Ǵ���6�����д���6 MSP��ʼ��
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();  //ʹ��GPIOCʱ��
        __HAL_RCC_USART6_CLK_ENABLE(); //ʹ��USART6ʱ��

        GPIO_InitSturct.Pin = GPIO_PIN_6;             //PC6
        GPIO_InitSturct.Mode = GPIO_MODE_AF_OD;       //�����������
        //GPIO_InitSturct.Pull = GPIO_PULLUP;           //����
        GPIO_InitSturct.Speed = GPIO_SPEED_FREQ_HIGH; //����
        GPIO_InitSturct.Alternate = GPIO_AF7_USART3;  //����ΪUSART6
        HAL_GPIO_Init(GPIOC, &GPIO_InitSturct);       //��ʼ��PC6

        GPIO_InitSturct.Pin = GPIO_PIN_7;       //PC7
        HAL_GPIO_Init(GPIOC, &GPIO_InitSturct); //��ʼ��PC7

#if EN_USART6_RX
        HAL_NVIC_EnableIRQ(USART6_IRQn);         //ʹ��USART2�ж�ͨ��
        HAL_NVIC_SetPriority(USART6_IRQn, 3, 3); //��ռ���ȼ�3�������ȼ�3
#endif
    }

}

void USART1_IRQHandler(void)
{
    uint32_t timeout = 0;
	
	//printf("USART1_IRQHandler\n");

    HAL_UART_IRQHandler(&UART1_Handler); //����HAL���жϴ����ú���

    timeout = 0;

    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY) //�ȴ�����
    {
        timeout++; ////��ʱ����
        if (timeout > HAL_MAX_DELAY)
            break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)rec_buf, 1) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
        if (timeout > HAL_MAX_DELAY)
            break;
    }
}

void USART2_IRQHandler(void)
{
    uint32_t timeout = 0;

    //printf("USART2_IRQHandler\n");

    HAL_UART_IRQHandler(&UART2_Handler); //����HAL���жϴ����ú���

    timeout = 0;

    while (HAL_UART_GetState(&UART2_Handler) != HAL_UART_STATE_READY) //�ȴ�����
    {
        timeout++; ////��ʱ����
        if (timeout > HAL_MAX_DELAY)
            break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART2_Handler, (uint8_t *)rec_buf, 1) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
        if (timeout > HAL_MAX_DELAY)
            break;
    }
}

void USART3_IRQHandler(void)
{
    uint32_t timeout = 0;

    HAL_UART_IRQHandler(&UART3_Handler); //����HAL���жϴ����ú���

    timeout = 0;

    while (HAL_UART_GetState(&UART3_Handler) != HAL_UART_STATE_READY) //�ȴ�����
    {
        timeout++; ////��ʱ����
        if (timeout > HAL_MAX_DELAY)
            break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART3_Handler, (uint8_t *)rec_buf, 1) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
        if (timeout > HAL_MAX_DELAY)
            break;
    }
}

void USART6_IRQHandler(void)
{
    uint32_t timeout = 0;
	
	//printf("USART6_IRQHandler\n");

    HAL_UART_IRQHandler(&UART6_Handler); //����HAL���жϴ����ú���

    timeout = 0;

    while (HAL_UART_GetState(&UART6_Handler) != HAL_UART_STATE_READY) //�ȴ�����
    {
        timeout++; ////��ʱ����
        if (timeout > HAL_MAX_DELAY)
            break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART6_Handler, (uint8_t *)rec_buf, 1) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
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
            ; //�ȴ����ͽ���
    }
}

void USART3_TransmitArray(const uint8_t *Array, uint8_t len, uint8_t addr)
{
    USART3->TDR = addr | 0x100;    //��9λΪ1��ʾ��ַ
    while ((USART3->ISR & 0X40) == 0)
            ; //�ȴ����ͽ���
    
    while(len--)
    {
        USART3->TDR = *Array++;
        while ((USART3->ISR & 0X40) == 0)
            ; //�ȴ����ͽ���
    }
}

void USART6_TransmitArray(const uint8_t *Array, uint8_t len)
{
    while(len--)
    {
        USART6->TDR = *Array++;
        while ((USART6->ISR & 0X40) == 0)
            ; //�ȴ����ͽ���
    }
}


#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while ((USART6->ISR & 0X40) == 0)
        ; //ѭ������,ֱ���������
    USART6->TDR = (uint8_t)ch;
    return ch;
}
