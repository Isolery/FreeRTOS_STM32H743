#include "App.h"

BYTE ReadBuffer[39];

uint8_t res=0;

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
    u32 total,free;
	u8 res=0;
	uint32_t wcnt;
	uint32_t cnt;
	
	printf("USBH_UserProcess\n");
	
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_DISCONNECTION:
            f_mount(0,"1:",1); 	//卸载U盘	   
            printf("Connect...\r\n");
            break;
        case HOST_USER_CLASS_ACTIVE:
            printf("Device Connect successed!.\r\n");	
            f_mount(fs[1],"1:",1); 	//重新挂载U盘
            res=exf_getfree("1:",&total,&free);
        	if(res==0)
            {	   
                printf("FATFS OK!\r\n");	
                printf("U Disk Total Size:%d MB\r\n", total);	 
                printf("U Disk  Free Size:%d MB\r\n", free);	

//				STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)flashdata,2);
//				file1point = flashdata[0];
//				file2point = flashdata[1];

//				cnt = file1point - file2point;

				f_open(file2, "1:/22data1.txt", FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 
				f_open(file1, "0:/22data1.txt", FA_READ|FA_WRITE);
//				while(cnt--)
//				{
//					f_lseek(file1, (file1point-cnt)*39);
//					f_lseek(file2, file2point*39);
//					file2point++;
					f_read(file1, ReadBuffer, 39, &br);
					f_write(file2,(void*)ReadBuffer,sizeof(ReadBuffer),&wcnt);
//				}
				f_close(file1);
				f_close(file2);
//				
//				flashdata[1] = file2point;
//				
//				STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)flashdata,2);
            }
            else
            {
                printf("U Disk Cpitacy get error\r\n");
            }
            break;
        case HOST_USER_CONNECTION:
            break;
        default:
            break;
    }
}

void App_Init(void)
{
	printf("exfuns_init() = %d\n", exfuns_init());	//为fatfs相关变量申请内存
	
	res = f_mount(fs[0],"0:",1); 		            //挂载NAND FLASH.  
	
	if(res == 0X0D)   //NAND FLASH磁盘,FAT文件系统错误,重新格式化NAND FLASH
	{
		printf("NAND Disk Formatting...\n");        //格式化NAND
		res = f_mkfs("0:",FM_ANY,0,fatbuf,FF_MAX_SS);	//格式化FLASH,2,盘符;1,不需要引导区,8个扇区为1个簇
		if(res == 0)
		{
			f_setlabel((const TCHAR *)"0:NANDDISK");	//设置Flash磁盘的名字为：NANDDISK
			printf("NAND Disk Format Finish\n");		//格式化完成		
		}
		else 
			printf("NAND Disk Format Error \n");	//格式化失败
	}  
	
	else if(res == FR_OK)
	{	
		res = f_open(file1,(const TCHAR*)"0:/22data1.txt", FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 	//创建文件
		f_close(file1);									//结束写入
		printf("f_mount Nand OK!\n");
	}
	
	printf("connect...\n");	
	
	USBH_Init(&hUSBHost, USBH_UserProcess, 0);
    USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
    USBH_Start(&hUSBHost);
    HAL_PWREx_EnableUSBVoltageDetector();
}
