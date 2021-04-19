#include "FreeRTOS.h"
#include "task.h"
#include "sys.h"
#include "usart.h"

void System_Init(void);
static void AppTaskCreate(void);
static void Task1_Entry(void* param);
static void Task2_Entry(void* param);

//创建任务句柄
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Task1_Handle = NULL;
static TaskHandle_t Task2_Handle = NULL;

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

	/* 创建任务1 */
	xReturn = xTaskCreate((TaskFunction_t  )Task1_Entry,           // 任务函数
	                      (const char*     )"Task1_Entry",         // 任务名称
						  (uint16_t        )512,                   // 任务堆栈大小
						  (void*           )NULL,                  // 传递给任务函数的参数
						  (UBaseType_t     )2,                     // 任务优先级
						  (TaskHandle_t*   )&Task1_Handle);        // 任务控制块指针  

	if(pdPASS == xReturn)               
		printf("Task1 Create Success...\n");
	
	/* 创建任务2 */
	xReturn = xTaskCreate((TaskFunction_t  )Task2_Entry,           // 任务函数
	                      (const char*     )"Task2_Entry",         // 任务名称
						  (uint16_t        )512,                   // 任务堆栈大小
						  (void*           )NULL,                  // 传递给任务函数的参数
						  (UBaseType_t     )3,                     // 任务优先级
						  (TaskHandle_t*   )&Task2_Handle);        // 任务控制块指针  

	if(pdPASS == xReturn)               
		printf("Task2 Create Success...\n");

	vTaskDelete(AppTaskCreate_Handle);    //删除AppTaskCreate任务

	taskEXIT_CRITICAL();                  //退出临界区
}

static void Task1_Entry(void* param)
{
	for(;;)
	{
		printf("Task1 Running...\n");
		vTaskSuspend(Task2_Handle);    // 挂起Task2
		vTaskDelay(100);    // 系统的时钟节拍设置为10ms中断一次，因此延时100个时钟节拍的时间是1s
		vTaskResume(Task2_Handle);     // 恢复Task2, 该函数可以恢复已经删除的任务
		
		//vTaskDelete(Task2_Handle);
	}
}

static void Task2_Entry(void* param)
{
	static portTickType PreviousWakeTime;
	const portTickType TimeInncrement = pdMS_TO_TICKS(10);
	
	PreviousWakeTime = xTaskGetTickCount();
	
	for(;;)
	{
		printf("Task2 Running...\n");
//		vTaskDelay(10);
		vTaskDelayUntil(&PreviousWakeTime, TimeInncrement);
	}
}

void System_Init(void)
{
	Stm32_Clock_Init(160,5,2,4);  		    // 系统时钟频率选择400MHz
	USART6_Init(115200);
	printf("============Start============\n");
}
