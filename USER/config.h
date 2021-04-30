#ifndef CONFIG_H
#define CONFIG_H

#define FLASH_SAVE_ADDR  0x080E0000

#define DSQUEUE_SIZE    10
#define STOREDATA_LEN   39

#define CPU1   1

#define CPU2   2

//手动配置项
#define CURRENTCPU   CPU1         // 当前工作的CPU
#define ERASE      0              // 1: 清除NandFlash和config.txt
#define DEBUG      1              // 1: 打印调试信息
#define UPDATE     1              // 1: 重新设置RTC时间

#if DEBUG == 1
#define PRINTF printf
#else
#define PRINTF nop_printf
#endif

#define FILE_CFG     "0:/config.txt"

#if CURRENTCPU == CPU1
#define FILE_NAND    "0:/data1.txt"
#define FILE_USB     "1:/data1.txt"
#else
#define FILE_NAND    "0:/data2.txt"
#define FILE_USB     "1:/data2.txt"
#endif

#endif
