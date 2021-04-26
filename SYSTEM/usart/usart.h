#ifndef __USART_H__
#define __USART_H__

#include "sys.h"
#include <stdio.h>

#define EN_USART1_RX    1
#define EN_USART2_RX    1
#define EN_USART3_RX    1
#define EN_USART6_RX    1

#define  USART1_RBUFF_SIZE            25 

extern uint8_t rec_buf[1];
	  		
extern UART_HandleTypeDef UART1_Handler; //UART1¾ä±ú
extern UART_HandleTypeDef UART2_Handler; //UART2¾ä±ú
extern UART_HandleTypeDef UART3_Handler; //UART3¾ä±ú
extern UART_HandleTypeDef UART6_Handler; //UART6¾ä±ú

extern DMA_HandleTypeDef DMA_Handle;

void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void USART3_Init(u32 bound);
void USART6_Init(u32 bound);

void USART2_TransmitString(const char *cString);
void USART3_TransmitArray(const uint8_t *Array, uint8_t len, uint8_t addr);
void USART6_TransmitArray(const uint8_t *Array, uint8_t len);

void USART1_DMA_Config(void);
void USART2_DMA_Config(void);

#endif
