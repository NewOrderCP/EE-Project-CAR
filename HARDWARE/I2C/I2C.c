#include "stm32f10x.h"

//*************用户头文件***********//
#include "I2C.h"
//*********************************//

//*************全局变量*************//
//*********************************//

//*************当前变量*************//
//*********************************//


/*
 * 函数名：IIC_Delay
 * 描述  ：I2C短暂延时
 * 输入  ：无
 * 返回  ：无
 */
void IIC_Delay (u8 z)
{
	uint32_t i;
	for (i = 0; i < (z - 0); i++);		//2
}


/*
 * 函数名：IIC_Send_Byte
 * 描述  ：I2C发送一个字节数据
 * 输入  ：Dat:	要发送的数据
 * 返回  ：无
 */
void IIC_Send_Byte(u8 Dat)
{                        
    u8 t;   
	
    for(t=0;t<8;t++)
    {       
				if(Dat & 0x80)
				{
					I2C_SDA_1(); 
				}
				else
				{
					I2C_SDA_0(); 
				}
        Dat<<=1; 	  
				
				IIC_Delay(7);			
				I2C_SCL_1();
				IIC_Delay(7);				
				I2C_SCL_0();
    }	 
} 	   


/*
 * 函数名：IIC_NAck
 * 描述  ：主机应答信号，不再发送下一地址数据
 * 输入  ：无
 * 返回  ：无
 */
void IIC_NAck(void)
{
	I2C_SCL_0();
	I2C_SDA_1();
	IIC_Delay(7);
	I2C_SCL_1();
	IIC_Delay(7);
	I2C_SCL_0();
	I2C_SDA_0();
}	

/*
 * 函数名：IIC_Ack
 * 描述  ：主机应答信号，从机继续发送下一字节数据
 * 输入  ：无
 * 返回  ：无
 */
void IIC_Ack(void)
{
	I2C_SDA_0();
	IIC_Delay(7);
	I2C_SCL_1();
	IIC_Delay(7);
	I2C_SCL_0();	
	I2C_SDA_1();
	IIC_Delay(1);
	I2C_SDA_0();
	I2C_SCL_0();
}

/*
 * 函数名：IIC_Read_Byte
 * 描述  ：读一个字节数据
 * 输入  ：ack:		1:	让从机继续发送下一地址数据
 *								0：	让从机停止发送出具
 * 返回  ：receive	:接受到的数据
 */
u8 IIC_Read_Byte(unsigned char ack)
{
		unsigned char i,receive=0;
	
		I2C_SDA_1();
		for(i=0;i<8;i++ )
		{
			I2C_SCL_0();
			IIC_Delay(7);
			I2C_SCL_1();
			IIC_Delay(3);
      receive<<=1;
      if(I2C_SDA_READ())receive++;   
			IIC_Delay(3);
			I2C_SCL_0();
    }					 
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}


/*
 * 函数名：IIC_Stop
 * 描述  ：IIC停止信号
 * 输入  ：无
 * 返回  ：无
 */
void IIC_Stop(void)
{
	I2C_SCL_0();
	I2C_SDA_0();//STOP:when CLK is high DATA change form low to high
 	IIC_Delay(7);
	I2C_SCL_1();
	I2C_SDA_1();//??I2C??????
	IIC_Delay(7);						   	
}

/*
 * 函数名：IIC_Wait_Ack
 * 描述  ：主机等待从机应答
 * 输入  ：无
 * 返回  ：	1：接收到应答信号
 *					0：未接收到应答信号
 */
u8 IIC_Wait_Ack(void)
{
	u32 ucErrTime = 0;
	I2C_SDA_1();
	IIC_Delay(7);
	I2C_SCL_1();
  IIC_Delay(7);

	while(I2C_SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	I2C_SCL_0();
	I2C_SDA_0();	
	IIC_Delay(7);
	return 0;  
} 


/*
 * 函数名：IIC_Start
 * 描述  ：IIC起始信号
 * 输入  ：无
 * 返回  ：无
 */
void IIC_Start(void)
{
	I2C_SDA_1(); 
	I2C_SCL_1();	
	IIC_Delay(7);
	IIC_Delay(7);
 	I2C_SDA_0();	
	IIC_Delay(7);
	I2C_SCL_0();	
}	  



/*
 * 函数名：i2c_Init
 * 描述  ：I2C初始化
 * 输入  ：无
 * 返回  ：无
 */
void i2c_Init (void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_I2C_GPIOX, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
	GPIO_Init(RCC_GPIOX_I2C, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
	GPIO_Init(RCC_GPIOX_I2C, &GPIO_InitStructure);
}
