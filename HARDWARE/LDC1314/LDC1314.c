#include "stm32f10x.h"
#include "stdio.h"

//*************用户头文件***********//
#include "LDC1314.h"
#include "I2C.h"
#include "delay.h"
#include "crc8.h"
#include "time.h"
//*********************************//

//*************全局变量*************//
extern u8 Statue;				//各类返回值判断
extern int one[4];					//初始值记录
extern u16 DatBuf[4];
extern int cha[4][4];
//extern float temp1;
//extern float k1,k2;
//extern u16 DataBuf[4];
//*********************************//

//*************当前变量*************//
uint16_t  FlashConfig[EVM_DEFAULTS_SIZE*sizeof(uint16_t)];
//*********************************//
int devldc(u16 d[4])
{
	int back=0;

		if(one[0]*one[1]*one[2]*one[3]==0)
		{
				one[0]=d[0];
				one[1]=d[1];
				one[2]=d[2];
				one[3]=d[3];
		}
				
//		d[0]=fabs(one[0]-d[0]);
//		d[1]=fabs(one[1]-d[1]);
//		d[2]=fabs(one[2]-d[2]);
//		d[3]=fabs(one[3]-d[3]);
//	
//		back=(-d[0]/kkk)+(-d[1]/(3*kkk))+(d[2]/(3*kkk))+(d[3]/kkk);
		
		if(d[2]-one[2]>30||d[3]-one[3]>30)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_5);
			delay_ms(10);
			}
		else 	GPIO_ResetBits(GPIOA,GPIO_Pin_5);		
		
		return back;
}


void LDC1314_INT()
{
		EXTI_InitTypeDef EXTI_InitStructure;	
		NVIC_InitTypeDef NVIC_InitStructure; 
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
			
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
			
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
			
		NVIC_InitStructure.NVIC_IRQChannel =  EXTI1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

void LDC1314_GetDat (u16 DatBuf[])
{	
	do
	{
		Statue = 1;
		
		Statue &= LDC1314_ReadData(LDC1314_SLAVE_ADDRESS, DATA_CH0, &DatBuf[0]);
		Statue &= LDC1314_ReadData(LDC1314_SLAVE_ADDRESS, DATA_CH1, &DatBuf[1]);
		Statue &= LDC1314_ReadData(LDC1314_SLAVE_ADDRESS, DATA_CH2, &DatBuf[2]);
		Statue &= LDC1314_ReadData(LDC1314_SLAVE_ADDRESS, DATA_CH3, &DatBuf[3]);
	}while(Statue == 0);

}

u8 LDC1314_Init (void)
{
	u8 ErrCount = 0;
	u8 Statue = 1;

	GPIO_InitTypeDef GPIO_InitStructure;
	 RCC_MCOConfig(RCC_MCO_PLLCLK_Div2); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	LDC1314_WriteData(LDC1314_SLAVE_ADDRESS, RESET_DEV, 0x8000);		//复位
	delay_ms(10);
	
//	if (calcCRC8((uint8_t *)&FlashConfig[0], EVM_DEFAULTS_SIZE*sizeof(uint16_t))) 
//	{
		do
		{
			if (ErrCount++ > 50)
			{
				return 0;
			}
			
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,RCOUNT_CH0,0x4FFF);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,RCOUNT_CH1,0x4FFF);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,RCOUNT_CH2,0x4FFF);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,RCOUNT_CH3,0x4FFF);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,OFFSET_CH0,0x0000);    
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,OFFSET_CH1,0x0000);//补偿值
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,OFFSET_CH2,0x0000);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,OFFSET_CH3,0x0000);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,SETTLECOUNT_CH0,0x0000); // 1 clock period
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,SETTLECOUNT_CH1,0x0000);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,SETTLECOUNT_CH2,0x0000);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,SETTLECOUNT_CH3,0x0000);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,CLOCK_DIVIDERS_CH0,0x0000); 
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,CLOCK_DIVIDERS_CH1,0X0000);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,CLOCK_DIVIDERS_CH2,0x0000);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,CLOCK_DIVIDERS_CH3,0x0000);
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,CONFIG,0x1601);  
				
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,MUX_CONFIG,0xC20F); 
       	LDC1314_WriteData(LDC1314_SLAVE_ADDRESS, RESET_DEV, 0x0400);		
				Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,DRIVE_CURRENT_CH0,0x5800); //
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,DRIVE_CURRENT_CH1,0x5800); //
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,DRIVE_CURRENT_CH2,0x5800); //
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,DRIVE_CURRENT_CH3,0x6000); //
			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,0x19,0x0001); //
		}while(Statue == 0);
		

//	}
//	else 			// valid crc write defaults
//	{ 		
//		Statue = 1;
//		
//		do 
//		{
//			for (i = 0x08; i < 0x19; i++) 
//			{
//				Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,i,FlashConfig[i-8]);
//			}
//			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,CONFIG,FlashConfig[i-8]);
//			Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,MUX_CONFIG,FlashConfig[i-7]);
//			for (i = 0x1D; i < 0x22; i++) 
//			{
//				Statue &= LDC1314_WriteData(LDC1314_SLAVE_ADDRESS,i,FlashConfig[i-10]);
//			}
//		}while(Statue == 0);
//		
//	}
//	
	return 1;
	
}

/*
 * 函数名：LDC1314_WriteData
 * 描述  ：向LDC1314寄存器写数据
 * 输入  ：SlaveAddress	:	器件地址
 *				 REG_Address	:	寄存器地址
 *				 REG_data			：写入的数据
 * 返回  ：	1:	写入成功
 *					0:	写入失败
 */
u8 LDC1314_WriteData (u8 SlaveAddress, u8 REG_Address,uint16_t REG_data)
{
		static u8 buffer[2];
	
		buffer[0] = (REG_data >> 8);
		buffer[1] = (u8)(REG_data & 0x00ff);
	
  	IIC_Start();
	
    IIC_Send_Byte(SlaveAddress << 1); 
	
    if (IIC_Wait_Ack() == 1)
		{
			return 0;
		}

    IIC_Send_Byte(REG_Address);  
		if (IIC_Wait_Ack() == 1)
		{
			return 0;
		}
		
		IIC_Send_Byte(buffer[0]);
		if (IIC_Wait_Ack() == 1)
		{
			return 0;
		}
		
		IIC_Send_Byte(buffer[1]);
		if (IIC_Wait_Ack() == 1)
		{
			return 0;
		}
		
		IIC_Stop();
		
		return 1;
}


/*
 * 函数名：LDC1314_ReadData
 * 描述  ：从LDC1314寄存器读数据
 * 输入  ：SlaveAddress	:	器件地址
 *				 REG_Address	:	寄存器地址
 *				 Read					：读出的数据存放地址
 * 返回  ：	1:	读出成功
 *					0:	读出失败
 */
u8 LDC1314_ReadData (u8 SlaveAddress, u8 reg_add,u16 *Read)
{
	u8 Dat_L = 0;
	u8 Dat_H = 0;
	
	/* 器件地址 */
	IIC_Start();
	IIC_Send_Byte(SlaveAddress << 1);
	if (IIC_Wait_Ack() == 1)
	{
		return 0;
	}
	
	/* 寄存器地址 */
	IIC_Send_Byte(reg_add);
	if (IIC_Wait_Ack() == 1)
	{
		return 0;
	}
	
	/* 器件地址（读）*/
	IIC_Start();
	
	IIC_Send_Byte((SlaveAddress << 1) + 1);
	if (IIC_Wait_Ack() == 1)
	{
		return 0;
	}
		
	
	Dat_H = IIC_Read_Byte(1);	//ack
	Dat_L = IIC_Read_Byte(0);	//Nack
	
	IIC_Stop();
		
	*Read = ((Dat_H << 8) | Dat_L);
	
	return 1;
}
