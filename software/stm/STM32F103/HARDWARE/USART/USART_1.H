#ifndef __USART_1_H
#define __USART_1_H

#include "sys.h"
#include <stdio.h>
#include <string.h>



extern unsigned char rev_cnt;
#define UART_RX_MAX 512

typedef struct
{
	unsigned int in;
	unsigned int out;
	unsigned char   buf[UART_RX_MAX];
}UART_S;

void uart_init(void);
void uart_it_cb(void);
void debug_read(void);



void Usart1_Init(unsigned int bound);
void Usart1_SendString(unsigned char *str);
void Usart1_SendPackage(unsigned char *data, unsigned short len);

void uart2_init( u32 bound );

#endif
