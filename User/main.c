#include "stm32f10x.h"
#include <stdio.h>
#include "sys.h"
#include "delay.h"
#include "motor.h"
#include "PID.H"
#include "inf.h"
//#include "exti.h"
#include "Usart.h"
#include "OLED_I2C.h"
#include "Fileter.h"
#include "crc8.h"
#include "adc.h"
#include "usart2.h"
#include "hc05.h"
#include "key.h"
#include "mpuiic.h"
#include "oycq_delay.h"
#include "mpu6050.h"
#include "hc04.h"
#include "timer.h" 

struct PID Control={300,0,250,0,0,0,0,0,0};
extern int times;
int times1,times2,time_now;
float k1=0,k2;
int cmd,cmd0;

extern float angle;
extern float dis1;

int main(void)
{
	u8 t;
	u8 key;
	 
	u8 sendbuf[20];	  
	u8 reclen=0;
	
	u16 adcx;
	unsigned char i;
	float temp;
	u8 display1[16];	
	u8 display2[16];

	delay_init();						//ϵͳSysTick��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	GPIO_Configuration();				//GPIO��ʼ��
	PWM_Configuration();				//���������ʼ��
//	uart_init(9600);					//���ڳ�ʼ��			
	I2C_Configuration();
//	Init_HMC5883L();
	MPU_Init(20);
	HC_SR04_Init();
//	OLED_Init();						//OLED��ʼ��
//	OLED_Fill(0x00);					//OLEDȫ����
	
	while(HC05_Init())
	{
//		sprintf((char*)display1,"BT CONNECT ERROR");
//		OLED_ShowStr(0,0,display1,2);
	}
//	HC05_Set_Cmd("AT+ROLE=0");	
//	HC05_Set_Cmd("AT+NAME=slave");
//	OLED_Fill(0x00);					//OLEDȫ����		
	TIM1_Int_Init(999,7199);			//��ʱ����ʼ��
	
		while(1)
		{

//			//����ʱ����ʾ
//			times1=times/10;
//			times2=times-times1*10;	
//			sprintf((char*)display1,"TIMES: %03d.%d s",times1,times2);	
//			OLED_ShowStr(0,2,display1,2);	

//			//MPU6050��ȡ��ʾ
//			sprintf((char*)display1,"MPU: %f",yaw);
//			OLED_ShowStr(0,4,display1,2);	

//			//�����������ʾ
//			hc04_running();
//			sprintf((char*)display1,"DIS: %f",dis1);
//			OLED_ShowStr(0,6,display1,2);			
			
			//����������ʾ
			if(USART2_RX_STA&0X8000)			//���յ�һ��������
			{
				reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
				USART2_RX_BUF[reclen]=0;	 	//���������
				USART2_RX_STA=0;
				cmd0=100*(USART2_RX_BUF[0]-48)+10*(USART2_RX_BUF[1]-48)+(USART2_RX_BUF[2]-48);	
				if(cmd0==100||cmd0==190||cmd0==180||cmd0==270||cmd0==111||cmd0==222||cmd0==333||cmd0==444||cmd0==555||cmd0==666)
					cmd=cmd0;
//				sprintf((char*)display1,"BT: %d",cmd);				
//				OLED_ShowStr(0,0,display1,2);				
			}
			
			
			//ָ��ִ��
			switch(cmd)
			{
				case 100:								//����0ָ��
					k1=dev(0);
					k2=PID_calculate(&Control,k1);	
					go(k2,800);	
					break;
				case 190:							//����90ָ��
					k1=dev(90);
					k2=PID_calculate(&Control,k1);	
					go(k2,800);	
					break;
				case 180:							//����180ָ��
					k1=dev(180);
					k2=PID_calculate(&Control,k1);	
					go(k2,800);	
					break;
				case 270:							//����270ָ��
					k1=dev(270);
					k2=PID_calculate(&Control,k1);	
					go(k2,800);	
					break;
				
				case 111:							//ָֹͣ��
					TIM3->CCR1=0;
					TIM3->CCR2=0;
					TIM3->CCR3=0;
					TIM3->CCR4=0;
					break;
				
				case 222:							//����ָ������ָ��
					GPIO_SetBits(GPIOB,GPIO_Pin_9);	//����
					delay_ms(1000);
					GPIO_ResetBits(GPIOB,GPIO_Pin_9);
					delay_ms(1000);
					break;
				
				case 333:							//ת����0ָ��
					time_now=times;
					while(times-time_now<2)
					{	
						k1=dev(0);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 444:							//ת����90ָ��
					time_now=times;
					while(times-time_now<2)
					{
						k1=dev(90);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 555:							//ת����180ָ��
					time_now=times;
					while(times-time_now<2)
					{
						k1=dev(180);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 666:							//ת����270ָ��
					time_now=times;
					while(times-time_now<2)
					{
						k1=dev(270);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;		
			}
			
		}
		
}
















