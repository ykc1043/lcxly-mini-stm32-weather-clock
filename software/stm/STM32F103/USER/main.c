#include "main.h"

#include "delay.h"
#include "sys.h"
#include "myiic.h"
#include "Timer.h"
#include "led.h"
#include "key.h"
#include "OLED.h"
#include "usart_1.h"

// #include "lcd_init.h"
// #include "lcd.h"
// #include "pic.h"
// #include "W25QXX.h"

volatile struct Sysdata sysdata = {0};
extern struct Sysdata sysData;

// ��������
void Init_Sysdata(volatile struct Sysdata *value);
void scheduler(void);
void every_second_task(void);
void every_minute_task(void);
void every_hour_task(void);

// ȫ�ֱ�������

volatile uint32_t seconds = 0;
volatile uint8_t F_Tim1s = 0;
volatile uint8_t F_Tim1m = 0;
volatile uint8_t F_Tim1h = 0;

float temperature;
float humidity;

typedef enum
{
	FAILED = 0,
	PASSED = !FAILED
} TestStatus;

// ����ԭ������
TestStatus Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);

int main(void)
{
	//	__IO uint32_t Flash_Size = 0;
	delay_init();
	Timer_Init();
	LED_Init(); // LED��ʼ��
	Key_Init();
	//Usart1_Init(115200);
	uart2_init(115200);
	//	SPI_FLASH_Init();
	IIC_Init();
	OLED_Init();
	OLED_ShowString(1,1,"O");
	LED = 0;
	IIC_Scan_Devices();

	Init_Sysdata(&sysdata);

#ifdef USEB11
	printf("USEB11\r\n");
#else
	printf("USEB7\r\n");
#endif
	printf("SERIAL_NUMBER:0x%x\r\n", SHT40_Read_Serial_Number());
	delay_ms(3000);

	while (1)
	{
		uint8_t keyget = Key_GetNum();
		(keyget == 1) ? (sysdata.ScreenReflesh = true) : (keyget == 2 ? OLED_Toggle() : 0);

		
		debug_read();
		scheduler();
		
	}
}

TestStatus Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
	{
		if (*pBuffer1 != *pBuffer2)
		{
			return FAILED;
		}

		pBuffer1++;
		pBuffer2++;
	}
	return PASSED;
}
// �������ݣ���ʼ��ϵͳ�����ṹ��
// ����ֵ����
void Init_Sysdata(volatile struct Sysdata *value)
{
	value->ScreenReflesh=true;
	value->timestamp = 0;						  // ��ʼ��ʱ���Ϊ��ǰʱ��
	strcpy((char *)value->ip_address, "0.0.0.0"); // ��ʼ�� IP ��ַΪĬ��ֵ
	value->ShtTemp = 0.0;						  // ��ʼ�� SHT �¶�Ϊ 0.0
	value->ShtHumi = 0.0;						  // ��ʼ�� SHT ʪ��Ϊ 0.0
	value->CurWeather = 0;						  // ��ʼ����ǰ����Ϊ 0
	value->CurTemp = 0.0;						  // ��ʼ����ǰ�¶�Ϊ 0.0
	strcpy((char *)value->adcode, "100000");	  // ��ʼ����ǰADCODE
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{

		F_Tim1s = 1;
		seconds++;

		if (seconds % 60 == 0)
		{
			F_Tim1m = 1;
		}

		if (seconds % 3600 == 0)
		{
			F_Tim1h = 1;
		}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void scheduler(void)
{
	if (F_Tim1s)
	{
		F_Tim1s = 0;
		every_second_task();
	}

	if (F_Tim1m)
	{
		F_Tim1m = 0;
		every_minute_task();
	}

	if (F_Tim1h)
	{
		F_Tim1h = 0;
		every_hour_task();
	}
}

// ÿ��ִ�е�����
void every_second_task(void)
{
	sysdata.timestamp++;
	SHT40_Read_RHData(&temperature, &humidity);
	sysdata.ShtTemp = temperature;
	sysdata.ShtHumi = humidity;
	//	OLED_ShowNum(1,1,seconds,4);
	if(sysdata.ScreenReflesh==true)
	{
		OLED_ShowData(&sysdata);
		sysdata.ScreenReflesh=false;
	}
	F_Tim1s = 0;
}

// ÿ����ִ�е�����
void every_minute_task(void)
{
	sysdata.ScreenReflesh=true;
}

// ÿСʱִ�е�����
void every_hour_task(void)
{
	
}
