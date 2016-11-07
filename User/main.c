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

	delay_init();						//系统SysTick初始化
	GPIO_Configuration();				//GPIO初始化
	PWM_Configuration();				//电机驱动初始化
	uart_init(9600);					//串口初始化
	Adc_Init();				
	I2C_Configuration();
	I2C_GPIO_Config();
	Init_HMC5883L();
	OLED_Init();						//OLED初始化
	OLED_Fill(0x00);					//OLED全屏灭
	
	while(HC05_Init())
	{
		sprintf((char*)display1,"BT CONNECT ERROR");
		OLED_ShowStr(0,0,display1,2);
	}
	OLED_Fill(0x00);					//OLED全屏灭
//	HC05_Set_Cmd("AT+ROLE=0");	
//	HC05_Set_Cmd("AT+NAME=slave");
			
	TIM2_Int_Init(999,7199);			//计时器初始化
	
	//电压检测
	adcx=Get_Adc_Average(ADC_Channel_4,10); 
	temp=(float)adcx*(3.3/4096);
	adcx=temp;
	temp-=adcx;
	temp*=100;
	sprintf((char*)display2,"VOLTAGE: %02d.%02d V",adcx,(int)temp);
	//电压显示
	OLED_ShowStr(0,4,display2,2);
	
		while(1)
		{

			//时间检测
			times1=times/10;
			times2=times-times1*10;	
			sprintf((char*)display1,"TIMES: %03d.%d s",times1,times2);	
			//时间显示
			OLED_ShowStr(0,2,display1,2);				
		
			//蓝牙接收显示
			if(USART2_RX_STA&0X8000)			//接收到一次数据了
			{

				reclen=USART2_RX_STA&0X7FFF;	//得到数据长度
				USART2_RX_BUF[reclen]=0;	 	//加入结束符
				USART2_RX_STA=0;	
				OLED_ShowStr(0,0,USART2_RX_BUF,2);				
			}
			
			//电子罗盘读取显示
			read_hmc5883l();	
			sprintf((char*)display3,"HMC: %f",angle);
			OLED_ShowStr(0,6,display3,2);
			
			//指令读取
			cmd=100*USART2_RX_BUF[0]+10*USART2_RX_BUF[1]+USART2_RX_BUF[2];	
			
			//指令执行
			switch(cmd)
			{
				case 0:								//方向0指令
					k1=dev(0);
					k2=PID_calculate(&Control,k1);	
					go(k2,500);	
					break;
				case 90:							//方向90指令
					k1=dev(90);
					k2=PID_calculate(&Control,k1);	
					go(k2,500);	
					break;
				case 180:							//方向180指令
					k1=dev(180);
					k2=PID_calculate(&Control,k1);	
					go(k2,500);	
					break;
				case 270:							//方向270指令
					k1=dev(270);
					k2=PID_calculate(&Control,k1);	
					go(k2,500);	
					break;
				
				case 111:							//停止指令
					TIM3->CCR1=0;
					TIM3->CCR2=0;
					TIM3->CCR3=0;
					TIM3->CCR4=0;
					break;
				
				case 222:							//到达指定车库指令
					//到达车库，声光报警提示
					delay_ms(3000);
					break;
				
				case 333:							//转向方向0指令
					time_now=times;
					while(times-time_now>20)
					{	
						read_hmc5883l();
						k1=dev(0);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 444:							//转向方向90指令
					time_now=times;
					while(times-time_now>20)
					{
						read_hmc5883l();
						k1=dev(90);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 555:							//转向方向180指令
					time_now=times;
					while(times-time_now>20)
					{
						read_hmc5883l();
						k1=dev(180);
						k2=PID_calculate(&Control,k1);	
						go(k2,0);	
					}
					break;
				case 666:							//转向方向270指令
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
















