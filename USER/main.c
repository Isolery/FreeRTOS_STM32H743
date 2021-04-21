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
	uint32_t r_event = 0;
	//uint32_t last_event = 0;
	BaseType_t xReturn = pdTRUE;
	
	for(;;)
	{
		/* BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, 
									  uint32_t ulBitsToClearOnExit, 
									  uint32_t *pulNotificationValue, 
									  TickType_t xTicksToWait ); 
		* ulBitsToClearOnEntry：当没有接收到任务通知的时候将任务通知值与此参数的取
		反值进行按位与运算，当此参数为Oxfffff或者ULONG_MAX的时候就会将任务通知值清零。
		* ulBits ToClearOnExit：如果接收到了任务通知，在做完相应的处理退出函数之前将
		任务通知值与此参数的取反值进行按位与运算，当此参数为0xfffff或者ULONG MAX的时候
		就会将任务通知值清零。
		* pulNotification Value：此参数用来保存任务通知值。
		* xTick ToWait：阻塞时间。
		*
		* 返回值：pdTRUE：获取到了任务通知。pdFALSE：任务通知获取失败。
		*/
		//获取任务通知 ,没获取到则一直等待
		xReturn = xTaskNotifyWait(0x0,
								  0xFFFFFFFF,
		                          &r_event,
		                          portMAX_DELAY);
		
		if(xReturn == pdTRUE)
		{
			printf("r_event = %X\n", r_event);
		}
		
		vTaskDelay(10);
	}
}

static void MidPriority_Task(void* param)
{
	for(;;)
	{
		
		vTaskDelay(10);
	}
}

static void HighPriority_Task(void* parameter)
{
	for(;;)
	{
		/* 原型:BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, 
									   uint32_t ulValue, 
									   eNotifyAction eAction ); 
		* eNoAction = 0，通知任务而不更新其通知值。
		* eSetBits，     设置任务通知值中的位。
		* eIncrement，   增加任务的通知值。
		* eSetvaluewithoverwrite，覆盖当前通知
		* eSetValueWithoutoverwrite 不覆盖当前通知
		* 
		* pdFAIL：当参数eAction设置为eSetValueWithoutOverwrite的时候，
		* 如果任务通知值没有更新成功就返回pdFAIL。
		* pdPASS: eAction 设置为其他选项的时候统一返回pdPASS。
		*/
		xTaskNotify((TaskHandle_t )LowPriority_Task_Handle,
				    (uint32_t)(1<<0),
		            (eNotifyAction)eSetBits);
		
		xTaskNotify((TaskHandle_t )LowPriority_Task_Handle,
				    (uint32_t)(1<<1),
		            (eNotifyAction)eSetBits);
		
		vTaskDelay(10);
	}
}

void System_Init(void)
{
	Stm32_Clock_Init(160,5,2,4);  		    // 系统时钟频率选择400MHz
	USART6_Init(115200);
	printf("============Start============\n");
}
