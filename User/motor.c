#include "stm32f10x.h"
#include "motor.h"
#include "sys.h"
#include "delay.h"
#include "math.h"

u8 t1=1,t2=1;

void GPIO_Configuration(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6|GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;       //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;       //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


}

void PWM_Configuration(void)	 //����PWM���
{
  TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

  TIM1_TimeBaseStructure.TIM_Period = 999;                           //���  
  TIM1_TimeBaseStructure.TIM_Prescaler = 71;                         
  TIM1_TimeBaseStructure.TIM_ClockDivision =0x0;
  TIM1_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      
  TIM_TimeBaseInit(TIM3, &TIM1_TimeBaseStructure);  	

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
  TIM_OCInitStructure.TIM_Pulse = 0;                                 //ʹ����������ת��
  TIM_OC1Init(TIM3,&TIM_OCInitStructure);                     			
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
  TIM_OCInitStructure.TIM_Pulse = 0;                                 //ʹ����������ת��
  TIM_OC2Init(TIM3,&TIM_OCInitStructure);                     			
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
  TIM_OCInitStructure.TIM_Pulse = 0;                                 //ʹ���ҵ������ת��
  TIM_OC3Init(TIM3,&TIM_OCInitStructure);                     			
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
  TIM_OCInitStructure.TIM_Pulse = 0;                                 //ʹ���ҵ������ת��
  TIM_OC4Init(TIM3,&TIM_OCInitStructure);                     			
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
 
  TIM_ARRPreloadConfig(TIM3, ENABLE);                                     
  TIM_Cmd(TIM3,ENABLE);
	
}

void Pulse(int m1,int m2)
{	
	if(m1>=0)
	{
		if(t1==0)
		{	
			TIM3->CCR1=0;
			TIM3->CCR2=0;
			delay_ms(10);
			t1=1;
		}
		TIM3->CCR1=m1;
	}
	else
	{
		if(t1==1)
		{
			TIM3->CCR1=0;
			TIM3->CCR2=0;
			delay_ms(10);
			t1=0;
		}
		TIM3->CCR3=-m1;
	}
	
	if(m2>=0)
	{
		if(t2==0)
		{
			TIM3->CCR3=0;
			TIM3->CCR4=0;
			delay_ms(10);
			t2=1;
		}
		TIM3->CCR3=m2;
	}
	else
	{	
		if(t2==1)
		{
			TIM3->CCR3=0;
			TIM3->CCR4=0;
			delay_ms(10);
			t2=0;
		}
		TIM3->CCR4=-m2;
	};
}

void go(int x,int pwm)	
{
	int m,n;
	m=pwm-x;
	n=pwm+x;
	if(m<-1000)
		m=-1000;
	if(m>1000)
		m=1000;
	if(n<-1000)
		n=-1000;
	if(n>1000)
		n=1000;
	Pulse(m,n);
//	TIM3->CCR1=m;
//	TIM3->CCR3=n;
}
