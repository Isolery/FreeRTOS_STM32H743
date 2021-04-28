#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "sys.h"
#include "usart.h"
#include "crc16.h"
#include "RLM300.h"
#include "App.h"
#include "delay.h"
#include "dsqueue.h"
#include "stmflash.h"
#include "config.h"

extern FIL *file1;	  		//文件1
extern FIL *file2;	  		//文件2

FRESULT fr;
UINT brs;

USBH_HandleTypeDef hUSBHost;

uint8_t data_125K[14] = {0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0D, 0x0A, 0x03};
uint8_t data_900M[20] = {0xAA, 0x11, 0x10, 0x00, 0x30, 0xAA, 0xBB, 0xE1, 0x00, 0x00, 0x00, 0x00, 0x02, 0x51, 0x32, 0x44, 0x02, 0x53, 0x55};
uint8_t EpcData[12] =   {0xBB, 0xE1, 0x0, 0x0, 0x0, 0x0, 0x02, 0x51, 0x32, 0x44, 0x02, 0x53}; 
uint8_t FrameData[20] = {0xAA, 0x11}; 
uint8_t rxUart6[10];
uint8_t storedata[STOREDATA_LEN] = {'[', '2', '0', '2', '1', '/', '0', '3', '/', '2', '4', \
	                      ' ', '1', '4', ':', '2', '5', ':', '1', '5',']', '0', '2', '5', '3', '3', \
                          '2', '4', '4', '0', '2', '5', '3', '0', '0', '1', '1', '\r', '\n'};
uint8_t master_data[5];

extern uint8_t RX_BUFF[USART1_RBUFF_SIZE];

void System_Init(void);

static void AppTaskCreate(void);
static void LowPriority_Task(void* pvParameters);   /* LowPriority_Task任务实现 */
static void ReceiveFromMachineSensor_Task(void* pvParameters);  /* ReceiveFromMachineSensor_Task任务实现 */
static void ProcessData_Task(void* pvParameters);
static void StoreData_Task(void* pvParameters);
static void Queue_Task(void* pvParameters);

//创建任务句柄
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LowPriority_Task_Handle = NULL;   /* LowPriority_Task任务句柄 */
static TaskHandle_t ReceiveFromMachineSensor_Task_Handle = NULL;  /* ReceiveFromMachineSensor_Task任务句柄 */
static TaskHandle_t ProcessData_Task_Handle = NULL;	
static TaskHandle_t StoreData_Task_Handle = NULL;
static TaskHandle_t Queue_Task_Handle = NULL;						  

//二值信号量句柄
SemaphoreHandle_t BinarySem_Handle = NULL;

//消息队列任务句柄
static QueueHandle_t Data125K_Queue = NULL;
static QueueHandle_t Data900M_Queue = NULL;

#define  Data125K_LEN    15    /* 队列的长度，最大可包含多少个消息 */
#define  Data900M_LEN    25    /* 队列的长度，最大可包含多少个消息 */
#define  QUEUE_SIZE      1     /* 队列中每个消息大小（字节） */

//事件组任务句柄
static EventGroupHandle_t Event_Handle = NULL; 

#define DataProcessEnd_Event    (1<<0)
#define UART6ReceiveEnd_Event   (1<<2)

int main(void)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	System_Init();
	
	xReturn = xTaskCreate((TaskFunction_t  )AppTaskCreate,           // 任务函数
	                      (const char*     )"AppTaskCreate",         // 任务名称
						  (uint16_t        )512,                     // 任务堆栈大小
						  (void*           )NULL,                    // 传递给任务函数的参数
						  (UBaseType_t     )1,                       // 任务优先级
						  (TaskHandle_t*   )&AppTaskCreate_Handle);  // 任务控制块指针
	
	if(pdPASS == xReturn)
		vTaskStartScheduler();    // 开启任务调度
	else
		return -1;
	
	while(1);
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	taskENTER_CRITICAL();           //进入临界区

	/* 创建BinarySem */
	BinarySem_Handle = xSemaphoreCreateBinary();

	if(BinarySem_Handle != NULL)
		printf("BinarySem_Handle Create Success...\n");
	
	/* 创建消息队列Data125K_Queue */
	Data125K_Queue = xQueueCreate((UBaseType_t) Data125K_LEN,    // 消息队列的长度
							     (UBaseType_t) QUEUE_SIZE);      // 消息的大小
	
	if(Data125K_Queue != NULL)
		printf("Create Data125K_Queue Success...\n");
	
	/* 创建消息队列Data900M_Queue */
	Data900M_Queue = xQueueCreate((UBaseType_t) Data900M_LEN,    // 消息队列的长度
							     (UBaseType_t) QUEUE_SIZE);      // 消息的大小
	
	if(Data900M_Queue != NULL)
		printf("Create Data900M_Queue Success...\n");
	
	/* 创建Event_Handle */
	Event_Handle = xEventGroupCreate();	 
	
	if(Event_Handle != NULL)
		printf("Event_Handle Create Success...\n");
	
	/* 创建LowPriority_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t  )LowPriority_Task,           // 任务函数
	                      (const char*     )"LowPriority_Task",         // 任务名称
						  (uint16_t        )512,                    // 任务堆栈大小
						  (void*           )NULL,                   // 传递给任务函数的参数
						  (UBaseType_t     )2,                      // 任务优先级
						  (TaskHandle_t*   )&LowPriority_Task_Handle);  // 任务控制块指针  

	if(pdPASS == xReturn)               
		printf("LowPriority_Task Create Success...\n");
	
	/* 创建ReceiveFromMachineSensor_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t  )ReceiveFromMachineSensor_Task,           // 任务函数
	                      (const char*     )"ReceiveFromMachineSensor_Task",         // 任务名称
						  (uint16_t        )512,                 // 任务堆栈大小
						  (void*           )NULL,                // 传递给任务函数的参数
						  (UBaseType_t     )4,                   // 任务优先级
						  (TaskHandle_t*   )&ReceiveFromMachineSensor_Task_Handle);  // 任务控制块指针  
						  
	if(pdPASS == xReturn)               
		printf("ReceiveFromMachineSensor_Task Create Success...\n");					  
						  
	/* 创建ProcessData_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t  )ProcessData_Task,           // 任务函数
	                      (const char*     )"ProcessData_Task",         // 任务名称
						  (uint16_t        )512,                        // 任务堆栈大小
						  (void*           )NULL,                       // 传递给任务函数的参数
						  (UBaseType_t     )4,                          // 任务优先级
						  (TaskHandle_t*   )&ProcessData_Task_Handle);  // 任务控制块指针  
						  
	if(pdPASS == xReturn)               
		printf("ProcessData_Task Create Success...\n");					  

	/* 创建StoreData_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t  )StoreData_Task,             // 任务函数
	                      (const char*     )"StoreData_Task",           // 任务名称
						  (uint16_t        )512,                        // 任务堆栈大小
						  (void*           )NULL,                       // 传递给任务函数的参数
						  (UBaseType_t     )4,                          // 任务优先级
						  (TaskHandle_t*   )&StoreData_Task_Handle);    // 任务控制块指针  							  

	if(pdPASS == xReturn)               
		printf("StoreData_Task Create Success...\n");
	
	/* 创建Queue_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t  )Queue_Task,             // 任务函数
	                      (const char*     )"Queue_Task",           // 任务名称
						  (uint16_t        )1024,                        // 任务堆栈大小
						  (void*           )NULL,                       // 传递给任务函数的参数
						  (UBaseType_t     )4,                          // 任务优先级
						  (TaskHandle_t*   )&Queue_Task_Handle);    // 任务控制块指针  							  

	if(pdPASS == xReturn)               
		printf("Queue_Task Create Success...\n");

	vTaskDelete(AppTaskCreate_Handle);    //删除AppTaskCreate任务

	taskEXIT_CRITICAL();                  //退出临界区
}

static void LowPriority_Task(void* param)
{
	__HAL_UART_ENABLE_IT(&UART1_Handler, UART_IT_IDLE);  
	HAL_NVIC_EnableIRQ(USART6_IRQn);  
	
	for(;;)
	{
		//PRINTF("LowPriority_Task\n");
		USBH_Process(&hUSBHost);
		vTaskDelay(1);
	}
}

// 通过DMA+IDLE的方式接收来自机感的原始数据
static void ReceiveFromMachineSensor_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;
	uint32_t i = 0;
	uint8_t  raw_data[25] = {0};
	uint8_t  data_length = 0;
	
	for(;;)
	{
		xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY);
		
		if(xReturn == pdPASS)
		{
			for(i = 0; i < 25; i++)
			{
				PRINTF("%02X ", RX_BUFF[i]);
				raw_data[i] = RX_BUFF[i];
			}
			PRINTF("\n");
			memset(RX_BUFF, 0, 25);
				
			if(raw_data[0] == 0x02)    // 125K数据
			{
				//进一步判断是否是完整的125K数据
				if(((raw_data[5] == 0x34) || (raw_data[5] == 0x30)) && (raw_data[11] == 0x0d) && (raw_data[12] == 0x0a) && (raw_data[13] == 0x03))
				{
					data_length = 14;
					
					xQueueSendToFront(Data125K_Queue, &data_length, 0);
					
					for(i = 0; i < data_length; i++)
					{
						xQueueSend(Data125K_Queue, &raw_data[i], 0);
					}
				}
			}
				
			else if(raw_data[0] == 0xAA)    // 900M数据
			{
				//进一步判断是否是完整的900M数据
			}
			
			else
			{
				PRINTF("unrecognized data!\n");
			}
		}
		
		vTaskDelay(1);
	}
}

static void ProcessData_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE;
	uint32_t recv_data;
	uint8_t  data_length, i;
	uint8_t  data[14] = {0};
	
	for(;;)
	{
		xReturn = xQueueReceive(Data125K_Queue,      /* 消息队列的句柄 */
							    &recv_data,		     /* 接收的消息内容 */
								portMAX_DELAY);      /* 等待时间一直等 */
		
		if(xReturn == pdTRUE)
		{
			data_length = recv_data;
		}
			
		for(i = 0; i < data_length; i++)
		{
			xReturn = xQueueReceive(Data125K_Queue,      /* 消息队列的句柄 */
									&recv_data,		     /* 接收的消息内容 */
									portMAX_DELAY);      /* 等待时间一直等 */
			 
			if(xReturn == pdTRUE)
			{
				data[i] = recv_data;
			}
			else
			{
				PRINTF("error\n");
				break;
			}
		}
		
		for(i = 0; i < data_length; i++)
		{
			PRINTF("%02X ", data[i]);
		}
		PRINTF("\n");
		
		//接下来开始处理数据
		// 02 ** ** ** ** 34 34 ** ** ** ** 0d 0a 03
		// 02 41 41 00 00 00 00 00 00 00 00 0d 0a 03 //  AA 00 -- 机感握手码
		process_data(data);
		
		FrameProcess(FrameData, EpcData, 0x11, sizeof(EpcData));
		
		for(i = 0; i < 20; i++)
		{
			//printf("%02X ", FrameData[i]);
		}

		storedata[21] = (EpcData[6]/16 > 9) ?  EpcData[6]/16 + 0x37 : EpcData[6]/16 + 0x30;
		storedata[22] = (EpcData[6]%16 > 9) ?  EpcData[6]%16 + 0x37 : EpcData[6]%16 + 0x30;
		storedata[23] = (EpcData[7]/16 > 9) ?  EpcData[7]/16 + 0x37 : EpcData[7]/16 + 0x30;
		storedata[24] = (EpcData[7]%16 > 9) ?  EpcData[7]%16 + 0x37 : EpcData[7]%16 + 0x30;
		storedata[25] = (EpcData[8]/16 > 9) ?  EpcData[8]/16 + 0x37 : EpcData[8]/16 + 0x30;
		storedata[26] = (EpcData[8]%16 > 9) ?  EpcData[8]%16 + 0x37 : EpcData[8]%16 + 0x30;
		storedata[27] = (EpcData[9]/16 > 9) ?  EpcData[9]/16 + 0x37 : EpcData[9]/16 + 0x30;
		storedata[28] = (EpcData[9]%16 > 9) ?  EpcData[9]%16 + 0x37 : EpcData[9]%16 + 0x30;
		storedata[29] = (EpcData[10]/16 > 9) ?  EpcData[10]/16 + 0x37 : EpcData[10]/16 + 0x30;
		storedata[30] = (EpcData[10]%16 > 9) ?  EpcData[10]%16 + 0x37 : EpcData[10]%16 + 0x30;
		storedata[31] = (EpcData[11]/16 > 9) ?  EpcData[11]/16 + 0x37 : EpcData[11]/16 + 0x30;
		storedata[32] = (EpcData[11]%16 > 9) ?  EpcData[11]%16 + 0x37 : EpcData[11]%16 + 0x30;
		
		xEventGroupSetBits(Event_Handle, DataProcessEnd_Event);
		
		vTaskDelay(1);
	}
}

static void Queue_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;
	EventBits_t r_event;
	u8 hour,min,sec,ampm;
	u8 year,month,date,week;
	Queue *q = init();
	
	for(;;)
	{
		r_event = xEventGroupWaitBits(Event_Handle,    /* 事件对象句柄 */
									  DataProcessEnd_Event | UART6ReceiveEnd_Event,   /* 接收线程感兴趣的事件 */
									  pdTRUE,          /* 退出时清除事件位 */
									  pdTRUE,          /* pdTRUE:满足感兴趣的所有事件 pdFALSE:满足感兴趣的任一事件*/
									  portMAX_DELAY);  /* 指定超时事件,一直等 */
		
		if((r_event & (DataProcessEnd_Event|UART6ReceiveEnd_Event)) != 0)
		{
			PRINTF("Queue_Task Running...\n");
			master_data[0] = rxUart6[1];    // 手柄
			master_data[1] = rxUart6[2];    // 速度
			master_data[2] = rxUart6[3];    // 时
			master_data[3] = rxUart6[4];    // 分
			master_data[4] = rxUart6[5];    // 秒
			
			RTC_Get_Time(&hour,&min,&sec,&ampm);
			RTC_Get_Date(&year,&month,&date,&week);

			master_data[2] = (master_data[2] <= 0x17) ? master_data[2] : hour;
			master_data[3] = (master_data[3] <= 0x3B) ? master_data[3] : min;
			master_data[4] = (master_data[4] <= 0x3B) ? master_data[4] : sec;
			
			storedata[3] = year/10 + 0x30;     // 2
			storedata[4] = year%10 + 0x30;     // 1
			storedata[6] = month/10 + 0x30;    // 0
			storedata[7] = month%10 + 0x30;    // 4
			storedata[9] = date/10 + 0x30;     // 0
			storedata[10] = date%10 + 0x30;    // 8
			storedata[12] = master_data[2]/10 + 0x30;    // 时
			storedata[13] = master_data[2]%10 + 0x30;    // 时
			storedata[15] = master_data[3]/10 + 0x30;    // 分
			storedata[16] = master_data[3]%10 + 0x30;    // 分
			storedata[18] = master_data[4]/10 + 0x30;    // sec
			storedata[19] = master_data[4]%10 + 0x30;    // sec
			
			storedata[33] = (master_data[0]/16 >= 10) ? master_data[0]/16 + 0x37 : master_data[0]/16 + 0x30;    // 手柄
			storedata[34] = (master_data[0]%16 >= 10) ? master_data[0]%16 + 0x37 : master_data[0]%16 + 0x30;
			storedata[35] = (master_data[1]/16 >= 10) ? master_data[1]/16 + 0x37 : master_data[1]/16 + 0x30;    // 速度
			storedata[36] = (master_data[1]%16 >= 10) ? master_data[1]%16 + 0x37 : master_data[1]%16 + 0x30;

			push(q, storedata);

			if(!isEmpty(q))
			{
				//任务通知
				xReturn = xTaskNotify(StoreData_Task_Handle,     // 任务句柄
		                             (uint32_t)&q,                     // 发送的数据 
		                             eSetValueWithOverwrite);    // 覆盖当前通知
		
				if(xReturn == pdPASS)
				{
					//printf("TaskNotify Send to StoreData_Task Success...\n");
				}
					
			
			}
		}
		else
		{
			PRINTF("Event Error...\n");
		}
		
		vTaskDelay(1);
	}
}

static void StoreData_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;
	uint32_t *recv;
	Queue * q = NULL;
	uint8_t *data;
	int i;
	uint8_t res;
	uint32_t wcnt;
	char point[8];

	for(;;)
	{
		xReturn = xTaskNotifyWait(0x0,               // 进入函数的时候不清除任务bit
								  0xFFFFFFFF,        // 退出函数的时候清除所有的bit
								  (uint32_t *)&recv, // 保存任务的通知值
		                          portMAX_DELAY);    // 阻塞时间

		if(pdTRUE == xReturn)
		{
			//printf("StoreData_Task receive data is %x \n", *recv);
			q = (Queue *)(*recv);
			//printf("q->front = %d\n", q->front);
			//printf("q->rear = %d\n", q->rear);

			while(!isEmpty(q))
			{
				data = pop(q);
				
				for(i = 0; i < STOREDATA_LEN; i++)
				{
					PRINTF("%c", data[i]);
				}		
				
				PRINTF("\n");
	
				//获取file1point的值
				res = f_open(file1,(const TCHAR*)FILE_CFG, FA_OPEN_EXISTING |FA_READ|FA_WRITE); 	//打开文件
				if(res == FR_OK)
				{
					f_read(file1, point, 8, &br);
					
					// 32位的数据按照小字节序写的，所以读出来后要进行转换
					pointerdata[0] = ((point[3] << 24) | (point[2] << 16) | (point[1] << 8) | point[0]);
					pointerdata[1] = ((point[7] << 24) | (point[6] << 16) | (point[5] << 8) | point[4]);
					
					PRINTF("file1point = 0x%04X\n", pointerdata[0]);
					PRINTF("file2point = 0x%04X\n", pointerdata[1]);
				}
				f_close(file1);
				
				file1point = pointerdata[0];    // file1point指向文件尾
				
				//存储数据
				res = f_open(file1, (const TCHAR*)FILE_NAND, FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 	//创建文件
				if(res == FR_OK)
				{
					f_lseek(file1, file1point*STOREDATA_LEN);
					file1point++;
					res = f_write(file1, (void*)data, STOREDATA_LEN, &wcnt);	//写入数据
					if(res == FR_OK)
					{
						PRINTF("fwrite ok,write data length is:%d byte\r\n\r\n",wcnt);	//打印写入成功提示,并打印写入的字节数			
					}else PRINTF("fwrite error:%d\r\n", res);	//打印错误代码
				}else PRINTF("fopen error:%d\r\n", res);			//打印错误代码
				f_close(file1);									//结束写入
				
				//保存file1point的值
				pointerdata[0] = file1point;
				res = f_open(file1,(const TCHAR*)FILE_CFG, FA_OPEN_EXISTING|FA_READ|FA_WRITE); 	//打开文件
				if(res == FR_OK)
				{
					res = f_write(file1,(void*)pointerdata, sizeof(pointerdata), &wcnt);	//写入数据
					if(res == FR_OK)
					{
						PRINTF("fwrite ok,write data length is:%d byte\n", wcnt);	//打印写入成功提示,并打印写入的字节数			
					}
				}
				f_close(file1);									//结束写入
			}

		}
									
		vTaskDelay(10);
	}
}

void System_Init(void)
{
	Stm32_Clock_Init(160,5,2,4);  		    // 系统时钟频率选择400MHz
	delay_init(400);
	RTC_Init(); 
	USART6_Init(115200);
	USART1_Init(115200);
	USART1_DMA_Config();  
	my_mem_init(SRAMIN);		    		//初始化内部内存池	

	NAND_Init();
	//NAND_EraseChip();
	FTL_Init();
	App_Init();

	
	PRINTF("============Start============\n");
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART6) //信息处理板, 通过地址访问CPU
	{
		volatile static uint8_t temp;
		volatile static uint8_t countRx, storeflag;
		BaseType_t xHigherPriorityTaskWoken;
	
		temp = rec_buf[0];

		if(temp == 0xC1)
		{
			//printf("FrameData...");
			USART6_TransmitArray(FrameData, 17);

			memset(FrameData, '\0', sizeof(FrameData));
			FrameData[0] = 0xAA;
			FrameData[1] = 0x11;   //0x12
		}
		
		if(temp == 0x7A)
		{
			countRx = 0;
			storeflag = 1;
		}
		else if(temp == 0x75)
		{
			storeflag = 0;
			rxUart6[countRx++] = temp; 
			
			xEventGroupSetBitsFromISR(Event_Handle, UART6ReceiveEnd_Event, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		
		if(storeflag == 1)
		{
			rxUart6[countRx++] = temp;    // 时：00,01,02,...,17
										  // 分：00,01,02,...,3B
		}
	}
}
