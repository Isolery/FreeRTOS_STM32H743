FreeRTOS学习笔记：



# Cortex-M特性



## 中断屏蔽寄存器



### PRIMASK

这个寄存器只有一个位，置1后，将关闭所有可屏蔽中断的异常，只剩NMI和硬fault，默认值为0；
操作指令：

```assembly
MRS R0, PRIMASK        ; R0=PRIMASK
MSR PRIMASK, R0        ; PRIMASK=R0
CPSID I                ; PRIMASK=1
CPSIE I                ; PRIMASK=0
```



### FAULTMASK

这个寄存器也只有一位，置1后，屏蔽除NMI外的所有异常（包括硬fault），默认值为0；
操作指令：

```assembly
MRS R0, FAULTMASK       ; R0=FAULTMASK
MSR FAULTMASK, R0       ; FAULTMASK=R0
CPSID F                 ; FAULTMASK=1
CPSIE F                 ; FAULTMASK=0
```



### BASEPRI

这个寄存器有9位，它定义了被屏蔽优先级的阈值；当它被设定为某个值后，所有优先级号大于等于此值得中断都被关闭，若设为0，则不关闭任何中断，默认值为0；

```assembly
MRS R0, BASEPRI       ; R0=BASEPRI
MSR BASEPRI, R0       ; BASEPRI=R0
```







SysTick_Handler ==>  xPortSysTickHandler (port.c)  ==>  vPortRaiseBASEPRI (portmacro.h) 

```assembly
static portFORCE_INLINE void vPortRaiseBASEPRI( void )
{
uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

	__asm
	{
		/* Set BASEPRI to the max syscall priority to effect a critical
		section. */
		cpsid i                         // 设置PRIMASK(禁止中断)
		msr basepri, ulNewBASEPRI       // 
		dsb
		isb
		cpsie i
	}
}
```



​	