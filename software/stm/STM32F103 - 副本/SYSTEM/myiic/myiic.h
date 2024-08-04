#ifndef __MYIIC_H
#define __MYIIC_H


#include "sys.h"
#include "delay.h"
#include "stdio.h"

#define USEB11//使用PB11的SHT40传感器，注释以使用PB7的SHT40传感器

#ifdef USEB11
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
#define IIC_SCL    PBout(10) // SCL
#define IIC_SDA    PBout(11) // SDA	 
#define READ_SDA   PBin(11)  // 输入SDA 
#else
#define SDA_IN()  {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= (u32)8 << 28;}
#define SDA_OUT() {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= (u32)3 << 28;}
#define IIC_SCL    PBout(6) // SCL
#define IIC_SDA    PBout(7) // SDA	 
#define READ_SDA   PBin(7)  // 输入SDA 
#endif

/**************************SHT40地址****************************/
#define SHT_IIC_ADDR 0x44
//#define SHT40_Write (SHT_IIC_ADDR << 1)      // write address
//#define SHT40_Read ((SHT_IIC_ADDR << 1) + 1) // read address


/**************************SHT40命令****************************/
#define SHT40_MEASURE_TEMPERATURE_HUMIDITY 0xFD  //高精度读取温湿度命令
#define SHT40_READ_SERIAL_NUMBER 0x89                         //读取唯一序列号命令
#define SHT40_HEATER_200mW_1s 0x39                               //200mW加热1秒命令


void IIC_Init(void);                // 初始化IIC的IO口				 
void IIC_Start(void);				// 发送IIC开始信号
void IIC_Stop(void);	  			// 发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			// IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);// IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				// IIC等待ACK信号
void IIC_Ack(void);					// IIC发送ACK信号
void IIC_NAck(void);				// IIC不发送ACK信号

void IIC_Scan_Devices(void);

uint8_t SHT_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);//IIC连续写
uint8_t SHT_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf); //IIC连续读 
uint8_t SHT_Write_Byte(uint8_t reg,uint8_t data);				//IIC写一个字节
uint8_t SHT_Read_Byte(uint8_t reg);						//IIC读一个字节

uint32_t SHT40_Read_Serial_Number(void);
void SHT40_Read_RHData(float *temperature, float *humidity);


#endif
