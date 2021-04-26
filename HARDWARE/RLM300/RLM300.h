#ifndef __RLM300_H__
#define __RLM300_H__ 

#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "usart.h"
#include "timer.h"

extern uint8_t flag_LedON;
extern uint8_t clearFlag_YGD;
extern uint8_t clearFlag_Forward_DD;
extern uint8_t clearFlag_Rear_DD;
extern uint32_t  tLedCount;  
extern uint32_t  tCountYGD;			
extern uint32_t  tCountFD;			    
extern uint32_t  tCountRD;

extern uint8_t flag_RxFinish;
extern uint8_t flag_Rx125K;
extern uint8_t flag_125K_Process_END;
extern uint8_t flag_NewData;

extern uint8_t EpcData[12];
extern uint8_t rxUart1[20];
extern uint8_t data_from_125K[20];

void Deal_RLM_Data(void);
uint8_t DecodeProtocol(const uint8_t* p_rxUart0, uint8_t* p_EpcData);
void RuleCheck(const uint8_t* p_EpcData);
void check_preportdata(uint8_t* p_EpcData, uint8_t* p_storeRedirDD, uint8_t* p_storeFdirYGD, uint8_t* p_storeFdirYGD2);
uint8_t check_portdata(uint8_t* p_EpcData, uint8_t* p_storeFdirYGD, uint8_t* p_storeFdirYGD2, uint8_t* p_storeRedirDD, uint8_t* p_storeLastportdata);
void Time_up_Clear(void);
void process_data(const uint8_t* data);
void transfer(const uint8_t* data, uint8_t handshake);

#endif




