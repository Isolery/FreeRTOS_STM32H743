#include "string.h"

int my_atoi(const char *str, char len)
{
	int value = 0;

	while(len--)
	{
		if(*str>='0' && *str<='9')
		{
			value *= 10;
			value += *str - '0';
			str++;
		}
	}
	return value;
}

const char* my_strncpy(char (*dst)[32], const char *src)
{
	int i = 0;
	while(*src != '#')
	{
		*(*(dst)+i) = *(src++);   //*(*(p+i)+j)
		i++;
	}
	*(*(dst)+i) = '\0';
	return src;
}

char *my_strcpy(char *dst, const char *src)
{
	char *ret = dst;

	while ((*dst++=*src++) != '#');

	*(dst++) = '\0';

	return ret;
}

uint8_t str2HEX(char *src, uint8_t *dst)
{
   	uint8_t i=0, j=1;
	uint8_t byte = 0;
	char *ptr = src;
	uint8_t count = 0;
	
	while(*ptr != '\0')
	//while(*ptr != '#')
	{
		if(count == 2)
		{
			dst[i++] = byte;
			byte = 0;
			j = 1;
			count = 0;
		}
		else
		{
				if(*ptr >= 'a' && *ptr <= 'f')
					byte += (*ptr - '0'-39)*(16%(16+j)+16/(16+j));
				else if(*ptr >= 'A' && *ptr <= 'F')
					byte += (*ptr - '0'-7)*(16%(16+j)+16/(16+j));
			    else if((*ptr >='0') && (*ptr <= '9'))
					byte += (*ptr - '0')*(16%(16+j)+16/(16+j));
				if(j--<=0) j = 1;
				ptr++;
				count++;
		}

		if(*ptr == '\0')//最后一字节
		//if(*ptr == '#')//最后一字节
		{
			dst[i++] = byte;
		}
	}
	
	return i;
 }

void print1(char (*arr)[32], int row, int col)
{
	int i = 0;
	int j = 0;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			printf("%c", arr[i][j]);
		}
		printf("\n");
	}
}
