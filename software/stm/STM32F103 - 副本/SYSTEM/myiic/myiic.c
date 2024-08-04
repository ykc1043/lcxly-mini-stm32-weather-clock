#include "myiic.h"

#ifdef USEB11
void IIC_Init(void)
{					     
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//先使能外设IO PORTB时钟 
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;	 // 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 
	
  GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);						 //PB10,PB11 输出高	
 
}
#else
void IIC_Init(void)
{					     
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//先使能外设IO PORTB时钟 
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	 // 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 
	
  GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);						 //PB10,PB11 输出高	
 
}
#endif

// 产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     // SDA线输出
	IIC_SDA = 1;	  	  
	IIC_SCL = 1;
	delay_us(4);
 	IIC_SDA = 0; // START:当CLK为高时，DATA从高变为低 
	delay_us(4);
	IIC_SCL = 0; // 钳住I2C总线，准备发送或接收数据 
}	  

// 产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT(); // SDA线输出
	IIC_SCL = 0;
	IIC_SDA = 0; // STOP:当CLK为高时，DATA从低变为高
 	delay_us(4);
	IIC_SCL = 1; 
	IIC_SDA = 1; // 发送I2C总线结束信号
	delay_us(4);							   	
}

// 等待应答信号到来
// 返回值：1，接收应答失败
//         0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	SDA_IN();      // SDA设置为输入  
	IIC_SDA = 1; delay_us(1);	   
	IIC_SCL = 1; delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL = 0; // 时钟输出0	   
	return 0;  
} 

// 产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}

// 不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}					 				     

// IIC发送一个字节
// 返回从机有无应答
// 1，有应答
// 0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL = 0; // 拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {              
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1; 	  
		delay_us(2);   // 对TEA5767这三个延时都是必须的
		IIC_SCL = 1;
		delay_us(2); 
		IIC_SCL = 0;	
		delay_us(2);
    }	 
} 	    

uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	SDA_IN(); // SDA设置为输入
    for(i = 0; i < 8; i++ )
	{
        IIC_SCL = 0; 
        delay_us(2);
		IIC_SCL = 1;
        receive <<= 1;
        if(READ_SDA) receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck(); // 发送nACK
    else
        IIC_Ack(); // 发送ACK   
    return receive;
}









void IIC_Scan_Devices(void)
{
	uint8_t addr = 0;
	uint8_t ack;

	printf("\r\n\r\n");
	for (addr= 0x00; addr < 0xff; ) {
		IIC_Start();
		IIC_Send_Byte(addr);
		ack = IIC_Wait_Ack();
		IIC_Stop();
		if (ack==0) {
			// We got an ack
			printf("%02x ", addr);
		} else {
			printf("-- ");	
		}

		if (addr > 0 && (addr + 1) % 16 == 0) printf("\r\n");//%02x ", addr);

		addr++;
		// Delay_ms(1);
	}
	printf("\r\n\r\n");
	
}


// IIC连续写
// addr:器件地址
// reg:寄存器地址
// len:写入长度
// buf:数据区
// 返回值:0,正常
//     其他,错误代码
uint8_t SHT_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i;
	IIC_Start();
	IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
	if (IIC_Wait_Ack())				// 等待应答
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg); // 写寄存器地址
	IIC_Wait_Ack();		// 等待应答
	for (i = 0; i < len; i++)
	{
		IIC_Send_Byte(buf[i]); // 发送数据
		if (IIC_Wait_Ack())	   // 等待ACK
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_Stop();
	return 0;
}

// IIC连续读
// addr:器件地址
// reg:要读取的寄存器地址
// len:要读取的长度
// buf:读取到的数据存储区
// 返回值:0,正常
//     其他,错误代码
uint8_t SHT_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	IIC_Start();
	IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
	if (IIC_Wait_Ack())				// 等待应答
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg); // 写寄存器地址
	IIC_Wait_Ack();		// 等待应答
	IIC_Start();
	delay_ms(10);//！！！！SHT40这里要10ms延迟，其他不要
	IIC_Send_Byte((addr << 1) | 1); // 发送器件地址+读命令
	IIC_Wait_Ack();					// 等待应答
	while (len)
	{
		if (len == 1)
			*buf = IIC_Read_Byte(0); // 读数据,发送nACK
		else
			*buf = IIC_Read_Byte(1); // 读数据,发送ACK
		len--;
		buf++;
	}
	IIC_Stop(); // 产生一个停止条件
	return 0;
}

// IIC写一个字节
// reg:寄存器地址
// data:数据
// 返回值:0,正常
// 其他,错误代码
uint8_t SHT_Write_Byte(uint8_t reg, uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte((SHT_IIC_ADDR << 1) | 0); // 发送器件地址+写命令
	if (IIC_Wait_Ack())						// 等待应答 若无回答 SHT_IIC_Wait_Ack()==1
	{
		IIC_Stop();
		return 1; // ERROR
	}
	IIC_Send_Byte(reg);	 // 写寄存器地址
	IIC_Wait_Ack();		 // 等待应答
	IIC_Send_Byte(data); // 发送数据
	if (IIC_Wait_Ack())	 // 等待ACK
	{
		IIC_Stop();
		return 1;
	}
	IIC_Stop();
	return 0; // NORMAL
}

// IIC读一个字节
// reg:寄存器地址
// 返回值:读到的数据
uint8_t SHT_Read_Byte(uint8_t reg)
{
	uint8_t res;
	IIC_Start();
	IIC_Send_Byte((SHT_IIC_ADDR << 1) | 0); // 发送器件地址+写命令
	IIC_Wait_Ack();							// 等待应答
	IIC_Send_Byte(reg);						// 写寄存器地址
	IIC_Wait_Ack();							// 等待应答
	IIC_Start();
	IIC_Send_Byte((SHT_IIC_ADDR << 1) | 1); // 发送器件地址+读命令
	IIC_Wait_Ack();							// 等待应答
	res = IIC_Read_Byte(0);					// 读取数据,发送nACK
	IIC_Stop();								// 产生一个停止条件
	return res;
}

/*************************************************************************************************
*	函 数 名: SHT40_Read_Serial_Number
*	入口参数: 无
*	返回值:32bit的序列号
*	函数功能: 读取SHT40的出场唯一序列号
*	说    明:无
*************************************************************************************************/
uint32_t SHT40_Read_Serial_Number(void)
{
	uint32_t Serial_Number;
	uint8_t I2C_Transmit_Data[1];
	I2C_Transmit_Data[0]=SHT40_READ_SERIAL_NUMBER;
	uint8_t I2C_Receive_Data[6]={0};
//	HAL_I2C_Master_Transmit(&hi2c1, SHT30_Write, I2C_Transmit_Data,1,HAL_MAX_DELAY);
	SHT_Write_Len(SHT_IIC_ADDR,I2C_Transmit_Data[0],1,NULL);//IIC连续写
	
//	HAL_I2C_Master_Receive(&hi2c1, SHT30_Read, I2C_Receive_Data,6,HAL_MAX_DELAY);
	SHT_Read_Len(SHT_IIC_ADDR,NULL,6,I2C_Receive_Data); //IIC连续读 	
	for(int i=0;i<6;i++)
	{
		printf("0x%x ",I2C_Receive_Data[i]);
	}
	printf("\r\nSHT40_READ_SERIAL_NUMBER:0x%x\r\n",I2C_Transmit_Data[0]);
	Serial_Number=(I2C_Receive_Data[0] << 24)|
			(I2C_Receive_Data[1] << 16)|
			(I2C_Receive_Data[3] << 8)|
			(I2C_Receive_Data[4] << 0);
	return Serial_Number;
}


void SHT40_Read_RHData(float *temperature, float *humidity)
{
  uint8_t writeData[1] = {SHT40_MEASURE_TEMPERATURE_HUMIDITY};
  uint8_t readData[6] = {0};
  uint32_t tempData = 0;
//  HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)SHT40_Write, (uint8_t *)writeData, 1, HAL_MAX_DELAY);
//  SHT_Write_Len(SHT_IIC_ADDR,0xfd,1,NULL);//IIC连续写
//  
//  delay_ms(10);
//  
  SHT_Read_Len(SHT_IIC_ADDR,writeData[0],6,readData);
//  HAL_I2C_Master_Receive(&hi2c1, (uint16_t)SHT40_Read, (uint8_t *)readData, 6, HAL_MAX_DELAY);
//	for(int i=0;i<6;i++)
//	{
//		printf("0x%x ",readData[i]);
//	}
//	printf("\r\n");

  tempData = readData[0] << 8 | readData[1];
  *temperature = (tempData * 175.0f) / 65535.0f - 45;

  tempData = readData[3] << 8 | readData[4];
  *humidity = (tempData * 125.0f) / 65535.0f - 6;
}

