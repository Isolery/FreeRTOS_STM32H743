#include "FreeRTOS.h"
#include "task.h"
#include "sys.h"
#include "usart.h"

static void AppTaskCreate(void);

//创建任务句柄
static TaskHandle_t AppTaskCreate_Handle;
static TaskHandle_t Task1_Handle;

//创建任务堆栈
static StackType_t AppTaskCreate_Stack[128];
static StackType_t Task1_Stack[128];
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

//创建任务控制块
static StaticTask_t AppTaskCreate_TCB;
static StaticTask_t Task1_TCB;
static StaticTask_t Idle_Task_TCB;	
static StaticTask_t Timer_Task_TCB;

/*
 * 使用了静态分配内存，以下这两个函数是由用户实现，函数在task.c文件中有引用
 * 当且仅当 configSUPPORT_STATIC_ALLOCATION 这个宏定义为 1 的时候才有效
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize);

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize);

static void Task1_Entry(void* param)
{
	for(;;)
	{
		printf("1\n");
		vTaskDelay(500);
		printf("2\n");
		vTaskDelay(500);
	}
}

int main(void)
{
	HAL_Init();				        		// 
	Stm32_Clock_Init(160,5,2,4);  		    // 系统时钟频率选择400MHz
	USART6_Init(115200);
	
	printf("Hello World!\n");
	
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t  )AppTaskCreate,           // 任务函数
	                                 (const char*     )"AppTaskCreate",         // 任务名称
								     (uint32_t        )128,                     // 任务堆栈大小
									 (void*           )NULL,                    // 传递给任务函数的参数
								     (UBaseType_t     )4,                       // 任务优先级
									 (StackType_t*    )AppTaskCreate_Stack,     // 任务堆栈
									 (StaticTask_t*   )&AppTaskCreate_TCB);     // 任务控制块
	
	
	
	if(AppTaskCreate_Handle != NULL)
		vTaskStartScheduler();    // 开启任务调度

	while(1);
									 
	return 0;
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
	taskENTER_CRITICAL();           //进入临界区

	/* 创建任务1 */
	Task1_Handle = xTaskCreateStatic((TaskFunction_t  )Task1_Entry,     // 任务函数
									 (const char*     )"Task1_Entry",   // 任务名称
									 (uint32_t        )128,             // 任务堆栈大小
									 (void*           )NULL,            // 传递给任务函数的参数
									 (UBaseType_t     )4,               // 任务优先级
									 (StackType_t*    )Task1_Stack,     // 任务堆栈
									 (StaticTask_t*   )&Task1_TCB);     // 任务控制块  

	if(NULL != Task1_Handle)               /* 创建成功 */
	printf("Task1 Create Success...\n");
	else
	printf("Task1 Create Failure...\n");

	vTaskDelete(AppTaskCreate_Handle);    //删除AppTaskCreate任务

	taskEXIT_CRITICAL();                  //退出临界区
}

/**
  **********************************************************************
  * @brief  获取空闲任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  **********************************************************************
  */ 
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* 任务控制块内存 */
	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* 任务堆栈内存 */
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* 任务堆栈大小 */
}

/**
  *********************************************************************
  * @brief  获取定时器任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  **********************************************************************
  */ 
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* 任务控制块内存 */
	*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* 任务堆栈内存 */
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* 任务堆栈大小 */
}

