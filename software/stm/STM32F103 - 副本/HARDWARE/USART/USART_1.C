#include "usart_1.h"
#include <string.h>
#include "consoled.h"

UART_S uart;
unsigned char rev_data;
// 添加全局打断标志变量
extern volatile int g_interrupt;
extern unsigned char cmd_prpt_str[];
// 环形缓冲区
void uart_buf_put(unsigned char ch)
{

	uart.buf[uart.in++] = ch;
	if (uart.in == UART_RX_MAX)
	{
		uart.in = 0;
	}
}

int uart_buf_get(unsigned char *ch)
{
	if (uart.in != uart.out)
	{
		*ch = uart.buf[uart.out++];
		if (uart.out == UART_RX_MAX)
		{
			uart.out = 0;
		}
		return 1;
	}
	return 0;
}

void debug_read(void)
{
	unsigned char ch;

	if (uart_buf_get(&ch))
	{

		// printf("%02X\r\n",ch);
		//                USART1_send1_buf_char(ch);
		at_proc(ch);
	}
}

// USART1_TypeDef usart1;

/*********************************************************************************************************
 * 函 数 名 : Usart1_SendString
 * 功能说明 : USART1发送字符串函数
 * 形    参 : str：需要发送的字符串
 * 返 回 值 : 无
 * 备    注 : 无
 *********************************************************************************************************/
void Usart1_SendString(unsigned char *str)
{
	while (*str != 0)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			;							// 等待上一个字节发送完成
		USART_SendData(USART1, *str++); // 发送一个字节
	}
}
/*********************************************************************************************************
 * 函 数 名 : Usart1_SendPackage
 * 功能说明 : USART1数据包发送函数
 * 形    参 : data：需要发送的数据，len：发送的数据的长度
 * 返 回 值 : 无
 * 备    注 : 数据包中间可能会包含‘\0’, 所以需要依赖长度进行发送
 *********************************************************************************************************/
void Usart1_SendPackage(unsigned char *data, unsigned short len)
{
	while (len--)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			;							 // 等待上一个字节发送完成
		USART_SendData(USART1, *data++); // 发送一个字节
	}
}
/*********************************************************************************************************
 * 函 数 名 : USART1_IRQHandler
 * 功能说明 : USART1中断服务函数
 * 形    参 : 无
 * 返 回 值 : 无
 * 备    注 : 接收中断+空闲中断, 每接收到一个字节，就会执行一次
 *********************************************************************************************************/
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) // 如果收到了数据（判断触发了什么中断）
	{
		
		rev_data = USART_ReceiveData(USART1);
		// 处理退格键的情况
		if (rev_data == '\b')
		{
			if (rev_cnt > 0)
			{
				// 如果接收缓冲区不为空，则退格删除最后一个字符
				rev_cnt--;

				// 发送退格字符和空格字符进行擦除
				printf("\b \b");
			}
		}
		else if (rev_data == 0x03) // Ctrl+C，ASCII码为0x03
		{
			g_interrupt = 1; // 设置中断标志，表示接收到Ctrl+C中断信号
		}
		else
		{
			uart_buf_put(rev_data);
			printf("%c",rev_data);
		}
	}
}


/*********************************************************************************************************
 * 函 数 名 : Usart1_Init
 * 功能说明 : 初始化USART1
 * 形    参 : bound：波特率
 * 返 回 值 : 无
 * 备    注 : 无
 *********************************************************************************************************/

void Usart1_Init(unsigned int bound)
{

	GPIO_InitTypeDef gpioinstructure;	// GPIO结构体初始化函数
	USART_InitTypeDef usartinstructure; // USART结构体初始化函数
	NVIC_InitTypeDef nvicinstructure;	// 中断控制器结构体初始化函数

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置中断控制器优先抢占级组

	// 1.配置GPIO、USART、引脚复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 配置GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   // 配置引脚复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // 配置USART时钟

	// 2.配置GPIO结构体

	// 配置PA9 TX 输出引脚
	gpioinstructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 复用推挽输出
	gpioinstructure.GPIO_Pin = GPIO_Pin_9;		   // 引脚9
	gpioinstructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度为50Mhz

	GPIO_Init(GPIOA, &gpioinstructure); // GPIO初始化

	// 配置PA10 RX 接收引脚
	gpioinstructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输出
	gpioinstructure.GPIO_Pin = GPIO_Pin_10;			   // 引脚10

	GPIO_Init(GPIOA, &gpioinstructure); // GPIO初始化

	// 3.配置串口的结构体
	usartinstructure.USART_BaudRate = bound;									 // 波特率为115200
	usartinstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流配置
	usartinstructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				 // 接收模式
	usartinstructure.USART_Parity = USART_Parity_No;							 // 无校验位
	usartinstructure.USART_StopBits = USART_StopBits_1;							 // 一个停止位
	usartinstructure.USART_WordLength = USART_WordLength_8b;					 // 有效数据位为8位

	USART_Init(USART1, &usartinstructure); // 初始化串口1

	USART_Cmd(USART1, ENABLE); // 使能串口1

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 串口中断配置

	// 4.配置中断控制器的结构
	nvicinstructure.NVIC_IRQChannel = USART1_IRQn;		   // 中断通道
	nvicinstructure.NVIC_IRQChannelCmd = ENABLE;		   // 通道使能
	nvicinstructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级配置为1
	nvicinstructure.NVIC_IRQChannelSubPriority = 1;		   // 子优先级配置为1

	NVIC_Init(&nvicinstructure); // 中断控制器初始化
}

/**************************************************************************************************************/

/*****加入以下代码,支持printf函数,而不需要选择use MicroLIB*****/
#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
void _ttywrch(int ch)
{
	ch = ch;
}
// 重定义fputc函数
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		;
	USART_SendData(USART1, ch);
	return ch;
}
/**********************printf support end**********************/
#endif
