#include "stm32f10x.h"
#include <stdio.h>
#include "sys.h"
#include "delay.h"
#include "motor.h"
#include "timer.h" 
#include "PID.H"
#include "inf.h"
#include "exti.h"
#include "Usart.h"
#include "OLED_I2C.h"
#include "Fileter.h"
#include "crc8.h"
#include "adc.h"
#include "usart2.h"
#include "hc05.h"
#include "key.h"
#include "HMC5883L.h"

struct PID Control={18,0,10,0,0,0,0,0,0};
extern int times;
int times1,times2,time_now;
float k1=0,k2;
int cmd;

extern float angle;

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
	u8 display3[16];
	u8 display4[16];

	delay_init();						//ϵͳSysTick��ʼ��
	GPIO_Configuration();				//GPIO��ʼ��
	PWM_Configuration();				//���������ʼ��
	uart_init(9600);					//���ڳ�ʼ��
	Adc_Init();				
	I2C_Configuration();
	I2C_GPIO_Config();
	Init_HMC5883L();
	OLED_Init();						//OLED��ʼ��
	OLED_Fill(0x00);					//OLEDȫ����
	
	while(HC05_Init())
	{
		sprintf((char*)display1,"BT CONNECT ERROR");
		OLED_ShowStr(0,0,display1,2);
	}
	OLED_Fill(0x00);					//OLEDȫ����
//	HC05_Set_Cmd("AT+ROLE=0");	
//	HC05_Set_Cmd("AT+NAME=slave");
			
	TIM2_Int_Init(999,7199);			//��ʱ����ʼ��
	
	//��ѹ���
	adcx=Get_Adc_Average(ADC_Channel_4,10); 
	temp=(float)adcx*(3.3/4096);
	adcx=temp;
	temp-=adcx;
	temp*=100;
	sprintf((char*)display2,"VOLTAGE: %02d.%02d V",adcx,(int)temp);
	//��ѹ��ʾ
	OLED_ShowStr(0,4,display2,2);
	
		while(1)
		{

			//ʱ����
			times1=times/10;
			times2=times-times1*10;	
			sprintf((char*)display1,"TIMES: %03d.%d s",times1,times2);	
			//ʱ����ʾ
			OLED_ShowStr(0,2,display1,2);				
		
			//����������ʾ
			if(USART2_RX_STA&0X8000)			//���յ�һ��������
			{

				reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
				USART2_RX_BUF[reclen]=0;	 	//���������
				USART2_RX_STA=0;	
				OLED_ShowStr(0,0,USART2_RX_BUF,2);				
			}
			
			//�������̶�ȡ��ʾ
			read_hmc5883l();	
			sprintf((char*)display3,"HMC: %f",angle);
			OLED_ShowStr(0,6,display3,2);
			
			//ָ���ȡ
			cmd=100*USART2_RX_BUF[0]+10*USART2_RX_BUF[1]+USART2_RX_BUF[2];	
			
			//ָ��ִ��
			switch(cmd)
			{
				case 0:								//����0ָ��
					k1=dev(0);
					k2=PID_calculate(&Control,k1);	
					go(k2,500);	
					break;
				case 90:							//����90ָ��
					k1=dev(90);
					k2=PID_calculate(&Control,k1);	
					go(k2,500);	
					break;
				case 180:							//����180ָ��
					k1=dev(180);
					k2=PID_calculate(&Control,k1);	
					go(k2,500);	
					break;
				case 270:							//����270ָ��
					k1=dev(270);
					k2=PID_calculate(&Control,k1);	
					go(k2,500);	
					break;
				
				case 111:							//ָֹͣ��
					TIM3->CCR1=0;
					TIM3->CCR2=0;
					TIM3->CCR3=0;
					TIM3->CCR4=0;
					break;
				
				case 222:							//����ָ������ָ��
					//���ﳵ�⣬���ⱨ����ʾ
					delay_ms(3000);
					break;
				
				case 333:							//ת����0ָ��
					time_now=times;
					while(times-time_now>20)
					{	
						read_hmc5883l();
						k1=dev(0);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 444:							//ת����90ָ��
					time_now=times;
					while(times-time_now>20)
					{
						read_hmc5883l();
						k1=dev(90);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 555:							//ת����180ָ��
					time_now=times;
					while(times-time_now>20)
					{
						read_hmc5883l();
						k1=dev(180);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 666:							//ת����270ָ��
					time_now=times;
					while(times-time_now>20)
					{
						read_hmc5883l();
						k1=dev(270);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;		
			}
			
		}
		
}
















