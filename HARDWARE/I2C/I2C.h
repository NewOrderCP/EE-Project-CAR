#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

#define RCC_I2C_GPIOX 		RCC_APB2Periph_GPIOB			//修改此处可以修改I2C的GPIOx  注：须与下一行同时修改
#define RCC_GPIOX_I2C GPIOB													//修改此处可以修改I2C的GPIOx
#define I2C_SCL_PIN GPIO_Pin_8										//修改此处可以修改I2C的SCL引脚
#define I2C_SDA_PIN GPIO_Pin_9  										//修改此处可以修改I2C的SDA引脚

#define I2C_SCL_1()  RCC_GPIOX_I2C->BSRR = I2C_SCL_PIN							//将SCL引脚置1
#define I2C_SCL_0()  RCC_GPIOX_I2C->BRR = I2C_SCL_PIN								//将SCL置0

#define I2C_SDA_1()  RCC_GPIOX_I2C->BSRR = I2C_SDA_PIN							//将SDA引脚置1
#define I2C_SDA_0()  RCC_GPIOX_I2C->BRR = I2C_SDA_PIN								//将SDA置0

#define I2C_SDA_READ()  ((RCC_GPIOX_I2C->IDR & I2C_SDA_PIN) != 0)		//读取SDA引脚值


void i2c_Init (void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NAck(void);
u8 IIC_Wait_Ack(void);
u8 IIC_Read_Byte(unsigned char ack);
void IIC_Send_Byte(u8 txd);

#endif

