#ifndef __SRTING_H__
#define __SRTING_H__

#include "sys.h"
#include "usart.h"

int my_atoi(const char *str, char len);
const char* my_strncpy(char (*dst)[32], const char *src);
char *my_strcpy(char *dst, const char *src);
uint8_t str2HEX(char *src, uint8_t *dst);
void print1(char (*arr)[32], int row, int col);

#endif
