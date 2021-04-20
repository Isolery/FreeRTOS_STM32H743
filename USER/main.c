#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "sys.h"
#include "usart.h"

void System_Init(void);

static void AppTaskCreate(void);
static void LowPriority_Task(void* pvParameters);   /* LowPriority_Task任务实现 */
static void MidPriority_Task(void* pvParameters);   /* MidPriority_Task任务实现 */
static void HighPriority_Task(void* pvParameters);  /* MidPriority_Task任务实现 */

//创建任务句柄
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LowPriority_Task_Handle = NULL;   /* LowPriority_Task任务句柄 */
static TaskHandle_t MidPriority_Task_Handle = NULL;   /* MidPriority_Task任务句柄 */
static TaskHandle_t HighPriority_Task_Handle = NULL;  /* HighPriority_Task任务句柄 */

//任务句柄
static EventGroupHandle_t Event_Handle = NULL; 

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
	
	/* 创建MidPriority_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t  )MidPriority_Task,           // 任务函数
	                      (const char*     )"MidPriority_Task",         // 任务名称
						  (uint16_t        )512,                 // 任务堆栈大小
						  (void*           )NULL,                // 传递给任务函数的参数
						  (UBaseType_t     )3,                   // 任务优先级
						  (TaskHandle_t*   )&MidPriority_Task_Handle);  // 任务控制块指针  
	
	if(pdPASS == xReturn)               
		printf("MidPriority_Task Create Success...\n");					  

	/* 创建HighPriority_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t  )HighPriority_Task,           // 任务函数
	                      (const char*     )"HighPriority_Task",         // 任务名称
						  (uint16_t        )512,                 // 任务堆栈大小
						  (void*           )NULL,                // 传递给任务函数的参数
						  (UBaseType_t     )4,                   // 任务优先级
						  (TaskHandle_t*   )&HighPriority_Task_Handle);  // 任务控制块指针  

	if(pdPASS == xReturn)               
		printf("HighPriority_Task Create Success...\n");

	vTaskDelete(AppTaskCreate_Handle);    //删除AppTaskCreate任务

	taskEXIT_CRITICAL();                  //退出临界区
}

static void LowPriority_Task(void* param)
{
	EventBits_t r_event;

	for(;;)
	{
		r_event = xEventGroupWaitBits(Event_Handle,    /* 事件对象句柄 */
									  (1<<0)|(1<<1),   /* 接收线程感兴趣的事件 */
									  pdTRUE,          /* 退出时清除事件位 */
									  pdFALSE,         /* pdTRUE:满足感兴趣的所有事件 pdFALSE:满足感兴趣的任一事件*/
									  portMAX_DELAY);  /* 指定超时事件,一直等 */

		if((r_event & (1<<0)|(1<<1)) != 0)	
		{
			printf("LowPriority_Task Running...\n");
		}						
		else
		{
			printf("Event Error...\n");
		}

		vTaskDelay(1);
	}
}

static void MidPriority_Task(void* param)
{
	for(;;)
	{
		printf("MidPriority_Task Running...\n");
		vTaskDelay(50);
	}
}

static void HighPriority_Task(void* parameter)
{
	for(;;)
	{
		vTaskDelay(500);

		xEventGroupSetBits(Event_Handle, (1<<2));
		xEventGroupSetBits(Event_Handle, (1<<1));

		vTaskDelay(50);
	}
}

void System_Init(void)
{
	Stm32_Clock_Init(160,5,2,4);  		    // 系统时钟频率选择400MHz
	USART6_Init(115200);
	printf("============Start============\n");
}
