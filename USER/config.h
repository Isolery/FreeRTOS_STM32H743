#ifndef CONFIG_H
#define CONFIG_H

#define FLASH_SAVE_ADDR  0x080E0000

#define DSQUEUE_SIZE    10
#define STOREDATA_LEN   39

#define FILE_NAND    "0:/test2.txt"
#define FILE_USB     "1:/test2.txt"
#define FILE_CFG     "0:/config.txt"

#define ERASE 0

#define DEBUG 1

#if DEBUG == 1
#define PRINTF printf
#else
#define PRINTF nop_printf

#endif

#endif
