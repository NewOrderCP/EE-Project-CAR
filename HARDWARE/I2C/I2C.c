#include "stm32f10x.h"

//*************�û�ͷ�ļ�***********//
#include "I2C.h"
//*********************************//

//*************ȫ�ֱ���*************//
//*********************************//

//*************��ǰ����*************//
//*********************************//


/*
 * ��������IIC_Delay
 * ����  ��I2C������ʱ
 * ����  ����
 * ����  ����
 */
void IIC_Delay (u8 z)
{
	uint32_t i;
	for (i = 0; i < (z - 0); i++);		//2
}


/*
 * ��������IIC_Send_Byte
 * ����  ��I2C����һ���ֽ�����
 * ����  ��Dat:	Ҫ���͵�����
 * ����  ����
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
 * ��������IIC_NAck
 * ����  ������Ӧ���źţ����ٷ�����һ��ַ����
 * ����  ����
 * ����  ����
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
 * ��������IIC_Ack
 * ����  ������Ӧ���źţ��ӻ�����������һ�ֽ�����
 * ����  ����
 * ����  ����
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
 * ��������IIC_Read_Byte
 * ����  ����һ���ֽ�����
 * ����  ��ack:		1:	�ôӻ�����������һ��ַ����
 *								0��	�ôӻ�ֹͣ���ͳ���
 * ����  ��receive	:���ܵ�������
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
 * ��������IIC_Stop
 * ����  ��IICֹͣ�ź�
 * ����  ����
 * ����  ����
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
 * ��������IIC_Wait_Ack
 * ����  �������ȴ��ӻ�Ӧ��
 * ����  ����
 * ����  ��	1�����յ�Ӧ���ź�
 *					0��δ���յ�Ӧ���ź�
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
 * ��������IIC_Start
 * ����  ��IIC��ʼ�ź�
 * ����  ����
 * ����  ����
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
 * ��������i2c_Init
 * ����  ��I2C��ʼ��
 * ����  ����
 * ����  ����
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
