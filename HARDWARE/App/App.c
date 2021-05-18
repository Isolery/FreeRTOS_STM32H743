#include "App.h"

USBH_HandleTypeDef hUSBHost;

FRESULT fr;
UINT brs;

BYTE ReadBuffer[39];

uint32_t file1point = 0;
uint32_t file2point = 0;

uint32_t pointerdata[2] = {0x0, 0x0};    // flashdata[0] = datacnt, flashdata[1] = storecnt;

uint8_t res=0;

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
    u32 total,free;
	u8 res=0;
	uint32_t wcnt;
	uint32_t cnt;
	u8 i = 0;
	char point[8];
	
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
				
				//获取file1point, file2point的值
				res = f_open(file1,(const TCHAR*)FILE_CFG, FA_OPEN_EXISTING |FA_READ|FA_WRITE); 	//打开文件
				if(res == FR_OK)
				{
					f_read(file1, point, 8, &br);
					
					// 32位的数据按照小字节序写的，所以读出来后要进行转换
					pointerdata[0] = ((point[3] << 24) | (point[2] << 16) | (point[1] << 8) | point[0]);
					pointerdata[1] = ((point[7] << 24) | (point[6] << 16) | (point[5] << 8) | point[4]);
					
					PRINTF("file1point = 0x%04X\n", pointerdata[0]);
					PRINTF("file2point = 0x%04X\n", pointerdata[1]);
				}
				f_close(file1);
				
				file1point = pointerdata[0];    // file1point指向文件尾
				file2point = pointerdata[1];    // file1point指向文件尾

				cnt = file1point - file2point;

				//f_open(file2, FILE_USB, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
				res = f_open(file2, FILE_USB, FA_OPEN_EXISTING|FA_READ|FA_WRITE);    // 若文件不存在，则打开失败
				if(res == FR_OK)
				{
					// 若文件存在，则在原文件后面继续添加
					//file2point = file2point;
				}
				else
				{
					file2point = 0;    // 若文件不存在，则新建文件并从头开始写
					f_open(file2, FILE_USB, FA_CREATE_NEW|FA_READ|FA_WRITE);
				}
				
				f_open(file1, FILE_NAND, FA_READ|FA_WRITE);
				
				while(cnt--)
				{
					f_lseek(file1, (file1point-cnt-1)*STOREDATA_LEN);
					f_lseek(file2, file2point*STOREDATA_LEN);
					file2point++;
					f_read(file1, ReadBuffer, STOREDATA_LEN, &br);
					f_write(file2, (void*)ReadBuffer, sizeof(ReadBuffer), &wcnt);
					PRINTF("cnt = %d\n", cnt);	
				}
				f_close(file1);
				f_close(file2);
			
				//保存file2point的值
				pointerdata[1] = file2point;
				res = f_open(file1,(const TCHAR*)FILE_CFG, FA_OPEN_EXISTING|FA_READ|FA_WRITE); 	//打开文件
				if(res == FR_OK)
				{
					res = f_write(file1,(void*)pointerdata, sizeof(pointerdata), &wcnt);	//写入数据
					if(res == FR_OK)
					{
						PRINTF("fwrite ok,write data length is:%d byte\n", wcnt);	//打印写入成功提示,并打印写入的字节数			
					}
				}
				f_close(file1);									//结束写入
				
				//////////////////////////////////////////////////////////////////////////////////////
				
				#if COORDINATE == 1    // 是否将U盘中的坐标信息保存到NandFlash
				
				res = f_open(file2, FILE_USB_CORD, FA_OPEN_EXISTING|FA_READ|FA_WRITE);    // 若文件不存在，则打开失败
				if(res == FR_OK)
				{
					PRINTF("FILE_USB_CORD open success...\n");
					
					file1point = 1;
					file2point = 1;
					
					res = f_open(file1,(const TCHAR*)FILE_NAND_CORD, FA_CREATE_ALWAYS|FA_READ|FA_WRITE); 	//总是创建新文件
					
					if(res == FR_OK)
					{
						PRINTF("FILE_NAND_CORD open success...\n");
					}
					
					// copy FILE_USB_CORD to FILE_NAND_CORD...
					while(1)
					{
						f_read(file2, ReadBuffer, COORDINATE_LEN, &br);
						
						printf("br = %d\n", br);
							
						if(br != 0)
						{
							f_write(file1, (void*)ReadBuffer, COORDINATE_LEN, &wcnt);
							
							f_lseek(file1, file1point*COORDINATE_LEN);
							f_lseek(file2, file2point*COORDINATE_LEN);
							
							file1point++;    
							file2point++;    
							
							for(i = 0; i < COORDINATE_LEN; i++)
							{
								PRINTF("%c", ReadBuffer[i]);
							}
							PRINTF("\n");
						}
						else
						{
							break;
						}	
					}
					
					f_close(file1);
					f_close(file2);
				}
				
				file1point = 1;
				
				//test
				//while(1)
				f_open(file1,(const TCHAR*)FILE_NAND_CORD, FA_READ|FA_WRITE);
				
				while(1)
				{	
					f_read(file1, ReadBuffer, COORDINATE_LEN, &br);
					
					if(br != 0)
					{
						f_lseek(file1, file1point*COORDINATE_LEN);
						
						file1point++;
						
						for(cnt = 0; cnt < COORDINATE_LEN; cnt++)
						{
							PRINTF("%c", ReadBuffer[cnt]);
						}
						PRINTF("\n");
						
					}
					else
					{
						break;
					}	
				}
				
				f_close(file1);
				
				#endif
				
				/////////////////////////////////////////////////////////////////////////////////////////

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
	uint32_t wcnt;
	
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

			//格式化完成后创建必要的文件
			
			//创建data文件
			res = f_open(file1,(const TCHAR*)FILE_NAND, FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 	//创建文件
			if(res == FR_OK)
			{
				printf("FILE_NAND File create Success\n");
			}
			f_close(file1);									//结束写入
			
			//创建config文件并写入初始值
			res = f_open(file1,(const TCHAR*)FILE_CFG, FA_OPEN_ALWAYS|FA_READ|FA_WRITE); 	//创建文件
			if(res == FR_OK)
			{
				printf("FILE_CFG File create Success\n");
				
				res = f_write(file1,(void*)pointerdata, sizeof(pointerdata), &wcnt);	//写入数据
				if(res == FR_OK)
				{
					printf("fwrite ok,write data length is:%d byte\n", wcnt);	//打印写入成功提示,并打印写入的字节数			
				}else printf("fwrite error:%d\n", res);	//打印错误代码	
			}
			f_close(file1);									//结束写入
			
		}
		else 
			printf("NAND Disk Format Error \n");	//格式化失败
	}  
	
	else if(res == FR_OK)
	{	
		printf("f_mount Nand OK!\n");
	}
	
	printf("connect...\n");	
	
//	USBH_Init(&hUSBHost, USBH_UserProcess, 0);
//    USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
//    USBH_Start(&hUSBHost);
//    HAL_PWREx_EnableUSBVoltageDetector();
}
