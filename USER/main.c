#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
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

//二值信号量句柄
SemaphoreHandle_t BinarySem_Handle = NULL;

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

	/* 创建CountSem */
	BinarySem_Handle = xSemaphoreCreateBinary();	 
	if(BinarySem_Handle != NULL)
		printf("BinarySem Create Success...\n");

	xReturn = xSemaphoreGive(BinarySem_Handle);  //给出二值信号量
	
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
						  (UBaseType_t     )3,                   // 任务优先级
						  (TaskHandle_t*   )&HighPriority_Task_Handle);  // 任务控制块指针  

	if(pdPASS == xReturn)               
		printf("HighPriority_Task Create Success...\n");

	vTaskDelete(AppTaskCreate_Handle);    //删除AppTaskCreate任务

	taskEXIT_CRITICAL();                  //退出临界区
}

static void LowPriority_Task(void* param)
{
	static uint32_t i;
	BaseType_t xReturn = pdPASS;

	for(;;)
	{
		printf("LowPriority_Task Get Semaphore...\n");

		xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY);

		if(xReturn == pdTRUE)
			printf("LowPriority_Task Running...\n");

		for(i = 0; i < 0xFFFFFF; i++)
		{
			taskYIELD();
		}

		printf("LowPriority_Task Release Semaphore...\n");

		xReturn = xSemaphoreGive(BinarySem_Handle);

		vTaskDelay(50);
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
	BaseType_t xReturn = pdPASS;

	for(;;)
	{
		printf("HighPriority_Task Get Semaphore...\n");

		xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY);

		if(xReturn == pdTRUE)
		{
			printf("HighPriority_Task Running...\n");
		}

		xReturn = xSemaphoreGive(BinarySem_Handle);

		vTaskDelay(50);
	}
}

void System_Init(void)
{
	Stm32_Clock_Init(160,5,2,4);  		    // 系统时钟频率选择400MHz
	USART6_Init(115200);
	printf("============Start============\n");
}
