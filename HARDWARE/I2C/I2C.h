#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

#define RCC_I2C_GPIOX 		RCC_APB2Periph_GPIOB			//�޸Ĵ˴������޸�I2C��GPIOx  ע��������һ��ͬʱ�޸�
#define RCC_GPIOX_I2C GPIOB													//�޸Ĵ˴������޸�I2C��GPIOx
#define I2C_SCL_PIN GPIO_Pin_8										//�޸Ĵ˴������޸�I2C��SCL����
#define I2C_SDA_PIN GPIO_Pin_9  										//�޸Ĵ˴������޸�I2C��SDA����

#define I2C_SCL_1()  RCC_GPIOX_I2C->BSRR = I2C_SCL_PIN							//��SCL������1
#define I2C_SCL_0()  RCC_GPIOX_I2C->BRR = I2C_SCL_PIN								//��SCL��0

#define I2C_SDA_1()  RCC_GPIOX_I2C->BSRR = I2C_SDA_PIN							//��SDA������1
#define I2C_SDA_0()  RCC_GPIOX_I2C->BRR = I2C_SDA_PIN								//��SDA��0

#define I2C_SDA_READ()  ((RCC_GPIOX_I2C->IDR & I2C_SDA_PIN) != 0)		//��ȡSDA����ֵ


void i2c_Init (void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NAck(void);
u8 IIC_Wait_Ack(void);
u8 IIC_Read_Byte(unsigned char ack);
void IIC_Send_Byte(u8 txd);

#endif

