#include "stm32f10x.h"
#include "HMC5883L.h"
#include  <math.h>    //Keil library  

#define   uchar unsigned char
#define   uint unsigned int	
#define   OSS 0	 //	BMP085ʹ��

#define	HMC5883L_Addr   0x3C	//������ַ
//#define	HMC5883L_Addr_read   0x3d	//������ַ

unsigned char BUF[8];                         //�������ݻ�����
char  test=0; 
int   x,y;
float angle;
u8 ge,shi,bai,qian,wan,shiwan;           //��ʾ����
int A_X,A_Y,A_Z;
//***BMP085ʹ��
short ac1=408;
short ac2=-72; 
short ac3=-14383; 
unsigned short ac4=32741;
unsigned short ac5=32757;
unsigned short ac6=23153;
short b1=6190; 
short b2=4;
short mb=32768;
short mc=-8711;
short md=2868;

long  temperature;
long  pressure;
//********
 //************************************++++++++++++++++++++++++++++++++
/*ģ��IIC�˿�������붨��*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_8
#define SCL_L         GPIOB->BRR  = GPIO_Pin_8 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_9
#define SDA_L         GPIOB->BRR  = GPIO_Pin_9

#define SCL_read      GPIOB->IDR  & GPIO_Pin_8
#define SDA_read      GPIOB->IDR  & GPIO_Pin_9


void conversion(long temp_data)  
{     
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //ȡ������ 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //ȡ������
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //ȡ������
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //ȡ������
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //ȡ������
    ge=temp_data+0x30; 	
}
/*******************************************************************************
* Function Name  : I2C_GPIO_Config
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{		
   u8 i=15; //��������Ż��ٶ�	����������͵�5����д��
   while(i) 
   { 
     i--; 
   }  
}

void delay5ms(void)
{
		
   int i=5000;  
   while(i) 
   { 
     i--; 
   }  
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
int I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return 0;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) return 1;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;
	I2C_delay();
	return 1;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
int I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
	  I2C_delay();
      return 0;
	}
	SCL_L;
	I2C_delay();
	return 1;
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
unsigned char I2C_RadeByte(void)  //���ݴӸ�λ����λ//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
} 
//ZRX          
//���ֽ�д��*******************************************

int Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	if(!I2C_Start())return 0;
    I2C_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop(); return 0;}
    I2C_SendByte(REG_Address );   //���õ���ʼ��ַ      
    I2C_WaitAck();	
    I2C_SendByte(REG_data);
    I2C_WaitAck();   
    I2C_Stop(); 
    delay5ms();
    return 1;
}

//���ֽڶ�ȡ*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
	if(!I2C_Start())return 0;
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop();test=1; return 0;}
    I2C_SendByte((u8) REG_Address);   //���õ���ʼ��ַ      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress+1);
    I2C_WaitAck();

	REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return TRUE;
	return REG_data;

}						      


 /*
********************************************************************************
** �������� �� WWDG_Configuration(void)
** �������� �� ���Ź���ʼ��
** ��    ��	�� ��
** ��    ��	�� ��
** ��    ��	�� ��
********************************************************************************
*/
/*void WWDG_Configuration(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);	
  WWDG_SetPrescaler(WWDG_Prescaler_8);	              //  WWDG clock counter = (PCLK1/4096)/8 = 244 Hz (~4 ms)  
  WWDG_SetWindowValue(0x41);		                 // Set Window value to 0x41
  WWDG_Enable(0x50);		       // Enable WWDG and set counter value to 0x7F, WWDG timeout = ~4 ms * 64 = 262 ms 
  WWDG_ClearFlag();			       // Clear EWI flag
  WWDG_EnableIT();			       // Enable EW interrupt
}
*/

void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}

void Delayms(vu32 m)
{
  u32 i;
  
  for(; m != 0; m--)	
       for (i=0; i<50000; i++);
}

///*
//********************************************************************************
//** �������� �� WWDG_IRQHandler(void)
//** �������� �� ������ǰ�����ж�
//** ��    ��	�� ��
//** ��    ��	�� ��
//** ��    ��	�� ��
//********************************************************************************
//*/ 
//
//void WWDG_IRQHandler(void)
//{
//  /* Update WWDG counter */
//  WWDG_SetCounter(0x50);
//	
//  /* Clear EWI flag */
//  WWDG_ClearFlag();
//  
//}
//******************************************************
void read_hmc5883l(void)
{
       Single_Write(HMC5883L_Addr,0x00,0x14);   //
       Single_Write(HMC5883L_Addr,0x01,0x20);   //���üĴ���B�������1090counts/gauss
       Single_Write(HMC5883L_Addr,0x02,0x00);   //
  	   Delayms(10);

       BUF[1]=Single_Read(HMC5883L_Addr,0x03);//OUT_X_H_A
       BUF[2]=Single_Read(HMC5883L_Addr,0x04);//OUT_X_L_A

	   BUF[3]=Single_Read(HMC5883L_Addr,0x07);//OUT_Y_H_A
       BUF[4]=Single_Read(HMC5883L_Addr,0x08);//OUT_Y_L_A

       x=(BUF[1] << 8) | BUF[2]; //Combine MSB and LSB of X Data output register
       y=(BUF[3] << 8) | BUF[4]; //Combine MSB and LSB of Z Data output register

       if(x>0x7fff)  x-=0xffff;	  
       if(y>0x7fff)  y-=0xffff;	  
       angle= atan2(y,x) * (180 / 3.14159265);// + 180; // angle in degrees
}
void Send_HMC5883L()
{
	conversion(angle);
//	printf(" HMC5883L angle=%f \r\n",angle);
//	printf(" HMC5883L: %d %d %d \r\n",bai,shi,ge);
//	printf(" HMC5883L Over \r\n"); 
}

//****************************
 void  Init_HMC5883L(void)
{
   Single_Write(HMC5883L_Addr,0x00,0x14);   //���üĴ���A������30Hz
   Single_Write(HMC5883L_Addr,0x01,0x20);   //���üĴ���B�������1090counts/gauss
   Single_Write(HMC5883L_Addr,0x02,0x00);   //��������ģʽ
// printf(" HMC5883L_initial \r\n");
}


//int main(void)
//{ 

//  I2C_GPIO_Config();
//  Init_HMC5883L();

//
//  while(1)
// {
//		read_hmc5883l();
//      Send_HMC5883L();
		
//		Delayms(300);	
//  }
//}



