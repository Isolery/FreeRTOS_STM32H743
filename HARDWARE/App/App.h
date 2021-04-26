#ifndef APP_H
#define APP_H

#include "sys.h"
#include "usart.h"
#include "nand.h"
#include "malloc.h"
#include "ftl.h"
#include "ff.h"
#include "exfuns.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "diskio.h"
#include "rtc.h"
#include "stmflash.h"

extern FIL *file1;	  		//文件1
extern FIL *file2;	  		//文件2

extern USBH_HandleTypeDef hUSBHost;

void App_Init(void);
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

#endif
