#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "sys.h"
#include "usart.h"

void System_Init(void);
static void AppTaskCreate(void);
static void Receive_Task(void* param);
static void Send_Task(void* param);

//创建任务句柄
static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Receive_Task_Handle = NULL;
static TaskHandle_t Send_Task_Handle = NULL;

//创建计数信号量句柄
//计数信号量可以用于资源管理，允许多个任务获取信号量访问共享资源，但会限制任
//务的最大数目。访问的任务数达到可支持的最大数目时，会阻塞其他试图获取该信号量的
//任务，直到有任务释放了信号量
SemaphoreHandle_t CountSem_Handle = NULL;

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
	CountSem_Handle = xSemaphoreCreateCounting(5, 5);

	if(CountSem_Handle != NULL)
		printf("Create Count Semaphore success...\n");
	
	/* 创建Receive_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t  )Receive_Task,           // 任务函数
	                      (const char*     )"Receive_Task",         // 任务名称
						  (uint16_t        )512,                    // 任务堆栈大小
						  (void*           )NULL,                   // 传递给任务函数的参数
						  (UBaseType_t     )2,                      // 任务优先级
						  (TaskHandle_t*   )&Receive_Task_Handle);  // 任务控制块指针  

	if(pdPASS == xReturn)               
		printf("Receive_Task Create Success...\n");
	
	/* 创建任务2 */
	xReturn = xTaskCreate((TaskFunction_t  )Send_Task,           // 任务函数
	                      (const char*     )"Send_Task",         // 任务名称
						  (uint16_t        )512,                 // 任务堆栈大小
						  (void*           )NULL,                // 传递给任务函数的参数
						  (UBaseType_t     )3,                   // 任务优先级
						  (TaskHandle_t*   )&Send_Task_Handle);  // 任务控制块指针  

	if(pdPASS == xReturn)               
		printf("Send_Task Create Success...\n");

	vTaskDelete(AppTaskCreate_Handle);    //删除AppTaskCreate任务

	taskEXIT_CRITICAL();                  //退出临界区
}

static void Receive_Task(void* param)
{
	BaseType_t xReturn = pdPASS;

	for(;;)
	{
		xReturn = xSemaphoreTake(CountSem_Handle, 0);

		if(xReturn == pdTRUE)
			printf("CountSem Get Success...\n");
		else
			printf("CountSem Get Failure...\n");

		vTaskDelay(1);
	}
}

static void Send_Task(void* param)
{
	BaseType_t xReturn = pdPASS;
	
	vTaskSuspend(Receive_Task_Handle);

	for(;;)
	{
		while(1)
		{
			xReturn = xSemaphoreGive(CountSem_Handle);     // 给出计数信号量
			if(xReturn == pdTRUE)
				printf("CountSem Release Success...\n");
			else
			{
				printf("CountSem Release Failure...\n");
				break;
			}	

			vTaskDelay(100);
		}
		
		vTaskResume(Receive_Task_Handle);
		vTaskDelay(10);
	}
}

void System_Init(void)
{
	Stm32_Clock_Init(160,5,2,4);  		    // 系统时钟频率选择400MHz
	USART6_Init(115200);
	printf("============Start============\n");
}
