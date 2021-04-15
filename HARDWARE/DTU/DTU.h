#ifndef __DTU_H__
#define __DTU_H__

#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "usart.h"
#include "delay.h"

extern volatile uint8_t countRx2;
extern uint16_t tLimt_rxTime;
extern uint8_t download_ok;
extern uint8_t rxend;
extern uint32_t cntData;
extern char rxUart2[20];

uint8_t DTU_Configuration(void);
void Connect_to_Server(void);

#endif
