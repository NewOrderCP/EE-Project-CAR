#include "mpu6050.h"
#include "delay.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "usart.h"
#include "inv_mpu.h"
#include "stm32f10x.h"
#include "mpuiic.h"   
#include "OLED_I2C.h"

// #define UART_SEND_CONTROL 0
// #define UART_RATE 500000
 //���������UART_SEND_CONTROL,���������ڵ�������
 //UART_RATE����UART��ʼ���Ĳ�����

	float pitch,roll,yaw; 		//ŷ����
	short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
	short gyrox,gyroy,gyroz;	//������ԭʼ����
//������ȫ�ֱ��������жϳ���ˢ�£��жϷ������ڳ�ʼ����������


void mpu_exti_init(void);

u8 display5[16];

//*******************************MPU6050��ʼ��****************************//
//��ʼ��MPU6050����ʼ��DMP
//����int the_fifo_rate:����MPU6050ˢ��FIFO���ݣ������жϣ����ٶ�,
//                     Ҳ��STM32ˢ��ȫ�ֱ������ٶ�
u8 MPU_Init(int the_fifo_rate)  
{ 
	u8 res;//��������MPU6050��ַ
	MPU_IIC_Init();//��ʼ��IIC����
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//��λMPU6050
	delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//����MPU6050 
	MPU_Set_Gyro_Fsr(3);					//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);					//���ٶȴ�����,��2g
	MPU_Set_Rate(50);						//���ò�����50Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//�ر������ж�
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C��ģʽ�ر�
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT���ŵ͵�ƽ��Ч
	
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);  //��һ��mpu��ID��ȷ���Ѿ���ȷ����
	if(res==MPU_ADDR)//����ID��ȷ
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//����CLKSEL,PLL X��Ϊ�ο�
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//���ٶ��������Ƕ�����
	  while(mpu_dmp_init());//����DMP  ����	
	 dmp_set_fifo_rate(the_fifo_rate);//����FIFOˢ���ٶ�
	 mpu_exti_init();//���ò������ж�
 	}else return 1;
	return 0;
}



//*****************************�жϷ�����*****************************//
 //ÿ��DMP׼����һ�����ݺ󣬾ͽ�����һ���жϳ���
 //�ж�������Ƶ��ͨ��set_fiforate������
void EXTI9_5_IRQHandler()   
{
static unsigned char count_to_delay=0;
count_to_delay=(count_to_delay+1)%50;

if(EXTI_GetITStatus(EXTI_Line8))
{   
   EXTI_ClearITPendingBit(EXTI_Line8);//����ж�  

//		GPIO_SetBits(GPIOA,GPIO_Pin_12);
//		sprintf((char*)display5,"MPU");
//		OLED_ShowStr(0,4,display5,2);

   MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
   MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
   mpu_dmp_get_data(&pitch,&roll,&yaw);    //�õ���̬
// #ifdef UART_SEND_CONTROL
//   usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));	
// #endif    
// //�����Ҫ���ڵ��ԣ�������̬
//    GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}

}


//***************************STM32�ж�����*****************************//
void mpu_exti_init()    //����MPU6050���ж�����
{
GPIO_InitTypeDef GPIO_InitStruct;
EXTI_InitTypeDef EXTI_InitStruct;
NVIC_InitTypeDef  NVIC_InitStruct;

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //GPIOA   

GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
GPIO_InitStruct.GPIO_Pin= GPIO_Pin_8;      //PIN8 
GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStruct);     //GPIOA

//GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
//GPIO_InitStruct.GPIO_Pin= GPIO_Pin_12;      //PIN12
//GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
//GPIO_Init(GPIOA, &GPIO_InitStruct);     //GPIOA


GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);  //GPIOA,PIN8

EXTI_InitStruct.EXTI_LineCmd=ENABLE;
EXTI_InitStruct.EXTI_Line=EXTI_Line8;       //pin8
EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
EXTI_Init(&EXTI_InitStruct);

NVIC_InitStruct.NVIC_IRQChannel= EXTI9_5_IRQn;     //EXTI9_5_IRQHandler
NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;   //��ռ���ȼ�1
NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;          //��Ӧ���ȼ�1
NVIC_Init(&NVIC_InitStruct);
}



//************************ΪMPU��ʼ������ĺ���******************************//
u8 MPU_Set_Gyro_Fsr(u8 fsr)//���������������̷�Χ  
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);
}

u8 MPU_Set_Accel_Fsr(u8 fsr)//���ü��ٶȴ����������̷�Χ  
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);
}


//����MPU6050�����ֵ�ͨ�˲���
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}



//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}




//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
short MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}


//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}


//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;
}



//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i; 
    MPU_IIC_Start(); 
	MPU_IIC_Send_Byte((addr<<1)|0);//����������ַ+д����	
	if(MPU_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		MPU_IIC_Stop();		 
		return 1;		
	}
    MPU_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ��
	for(i=0;i<len;i++)
	{
		MPU_IIC_Send_Byte(buf[i]);	//��������
		if(MPU_IIC_Wait_Ack())		//�ȴ�ACK
		{
			MPU_IIC_Stop();	 
			return 1;		 
		}		
	}    
    MPU_IIC_Stop();	 
	return 0;	
} 


//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
 	MPU_IIC_Start(); 
	MPU_IIC_Send_Byte((addr<<1)|0);//����������ַ+д����	
	if(MPU_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		MPU_IIC_Stop();		 
		return 1;		
	}
    MPU_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ��
    MPU_IIC_Start();
	MPU_IIC_Send_Byte((addr<<1)|1);//����������ַ+������	
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	while(len)
	{
		if(len==1)*buf=MPU_IIC_Read_Byte(0);//������,����nACK 
		else *buf=MPU_IIC_Read_Byte(1);		//������,����ACK  
		len--;
		buf++; 
	}    
    MPU_IIC_Stop();	//����һ��ֹͣ���� 
	return 0;	
}


//IICдһ���ֽ� 
//reg:�Ĵ�����ַ
//data:����
u8 MPU_Write_Byte(u8 reg,u8 data) 				 
{ 
    MPU_IIC_Start(); 
	MPU_IIC_Send_Byte((MPU_ADDR<<1)|0);//����������ַ+д����	
	if(MPU_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		MPU_IIC_Stop();		 
		return 1;		
	}
    MPU_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	MPU_IIC_Send_Byte(data);//��������
	if(MPU_IIC_Wait_Ack())	//�ȴ�ACK
	{
		MPU_IIC_Stop();	 
		return 1;		 
	}		 
    MPU_IIC_Stop();	 
	return 0;
}
//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
u8 MPU_Read_Byte(u8 reg)
{
	u8 res;
    MPU_IIC_Start(); 
	MPU_IIC_Send_Byte((MPU_ADDR<<1)|0);//����������ַ+д����	
	MPU_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
    MPU_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ��
    MPU_IIC_Start();
	MPU_IIC_Send_Byte((MPU_ADDR<<1)|1);//����������ַ+������	
    MPU_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	res=MPU_IIC_Read_Byte(0);//��ȡ����,����nACK 
    MPU_IIC_Stop();			//����һ��ֹͣ���� 
	return res;		
}


//***************************���ڵ�����������*****************************//
//�����UART_SEND_CONTROL����ִ����һ����ı���
#ifdef UART_SEND_CONTROL
void usart1_send_char(u8 c)
{   	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	USART_SendData(USART1,c);  
} 
//�������ݸ�����������λ�����(V2.6�汾)
//fun:������. 0XA0~0XAF
//data:���ݻ�����,���28�ֽ�!!
//len:data����Ч���ݸ���
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//���28�ֽ����� 
	send_buf[len+3]=0;	//У��������
	send_buf[0]=0X88;	//֡ͷ
	send_buf[1]=fun;	//������
	send_buf[2]=len;	//���ݳ���
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//��������
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//����У���	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//�������ݵ�����1 
}


//ͨ������1�ϱ���������̬���ݸ�����
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
//roll:�����.��λ0.01�ȡ� -18000 -> 18000 ��Ӧ -180.00  ->  180.00��
//pitch:������.��λ 0.01�ȡ�-9000 - 9000 ��Ӧ -90.00 -> 90.00 ��
//yaw:�����.��λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//��0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//�ɿ���ʾ֡,0XAF
}  
#endif



