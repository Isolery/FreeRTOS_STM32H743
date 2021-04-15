#include "crc16.h"

/**********************************************************************************************
* 函 数 名  ：Check_CRC16
* 功能说明：校验函数 
* 输入参数：pucFrame --> 需要校验的数据首地址    len --> 校验长度
* 返 回 值 ：uint16_t --> 返回两个字节的校验码
**********************************************************************************************/
uint16_t Check_CRC16(uint8_t* pucFrame, uint8_t len)
{
    uint8_t CRCH = 0xFF;
		uint8_t CRCL = 0xFF;

    uint16_t index;

    while(len--)
    {
        index = CRCL ^ *(pucFrame++);
        CRCL = (uint8_t)(CRCH ^ CRC_16H[index]);
        CRCH = CRC_16L[index];
    }

    return (uint16_t)(CRCH<< 8 | CRCL);
}

void FrameProcess(uint8_t* dstArray, const uint8_t* srcArray, uint8_t type, uint8_t len)
{
	uint8_t *p_dstArray = dstArray;
	uint16_t checkValue = 0;

	p_dstArray[0] = 0xAA;    //SOF 
	p_dstArray[1] = type;
	p_dstArray[2] = len;
	p_dstArray += 3;

	while(len--)   
	{
		*p_dstArray++ = *srcArray++;
	}

	checkValue = Check_CRC16(dstArray, (3+dstArray[2]));
	*p_dstArray++ = (uint8_t)(checkValue>>8);    //CRC
	*p_dstArray = (uint8_t)(checkValue);    //CRC
}
