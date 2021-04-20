#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
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

//创建消息队列句柄
QueueHandle_t Test_Queue = NULL;

#define  QUEUE_LEN    100   /* 队列的长度，最大可包含多少个消息 */
#define  QUEUE_SIZE   4   /* 队列中每个消息大小（字节） */

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

	/* 创建消息队列Queue */
	Test_Queue = xQueueCreate((UBaseType_t) QUEUE_LEN,    // 消息队列的长度
							  (UBaseType_t) QUEUE_SIZE);  // 消息的大小
	
	if(Test_Queue != NULL)
		printf("Create Test_Queue Success...\n");
	
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
	BaseType_t xReturn = pdTRUE;
	uint32_t recv_data;
	
	for(;;)
	{
		//printf("Receive_Task is running...\n");
		xReturn = xQueueReceive(Test_Queue,      /* 消息队列的句柄 */
							    &recv_data,		 /* 接收的消息内容 */
								portMAX_DELAY);  /* 等待时间一直等 */
		
		if(xReturn == pdTRUE)
			printf("Receive data = %d\n", recv_data);
		else
			printf("Data receive error...\n");
		
		vTaskDelay(1);
	}
}

static void Send_Task(void* param)
{
//	BaseType_t xReturn = pdPASS;
//	uint32_t send_data = 1;
//	
//	vTaskSuspend(Receive_Task_Handle);  
	
	for(;;)
	{
//		printf("Send_data...\n");
//		
//		xReturn = xQueueSend(Test_Queue,   /* 消息队列的句柄 */
//							 &send_data,   /* 发送的消息内容 */
//		                     0);           /* 等待时间 0 */
//		
//		if(xReturn == pdPASS)
//			printf("Send data success...\n");
//		
//		send_data++;
//		
//		if(send_data == 100)
//		{
//			vTaskResume(Receive_Task_Handle); 
//			vTaskSuspend(Send_Task_Handle);  
//			printf("Resume Receive Task...\n");
//		}

		vTaskDelay(1);
	}
}

void EXIT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint32_t send_data = 1;
	
	printf("EXTI15_10_IRQHandler\n");
	
	xQueueSendFromISR(Test_Queue,                           /* 消息队列的句柄 */
					  &send_data,                           /* 发送的消息内容 */
					  &xHigherPriorityTaskWoken);           /* 等待时间 0 */
	
	if(xHigherPriorityTaskWoken)
	{
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
}

void System_Init(void)
{
	Stm32_Clock_Init(160,5,2,4);  		    // 系统时钟频率选择400MHz
	USART6_Init(115200);
	EXIT_Init();
	printf("============Start============\n");
}
