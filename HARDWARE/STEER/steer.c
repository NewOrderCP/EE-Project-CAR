#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "stm32f10x_tim.h"
#include "steer.h"

void GPIO_Configuration(void);
void Timer3_configuration(int c3,int c4);

void steer(int c3,int c4)
{ 
	//SystemInit();
	//delay_init(72);		//延时初始化
 	GPIO_Configuration();
	Timer3_configuration(c3,c4);
}

void GPIO_Configuration(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;       //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}


//********************* 舵机 PWM ******************
void Timer3_configuration(int c3,int c4)
{
  TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

  TIM1_TimeBaseStructure.TIM_Period = 999;                           //计数初值     
  TIM1_TimeBaseStructure.TIM_Prescaler = 71;                           //分频
  TIM1_TimeBaseStructure.TIM_ClockDivision =0x0;
  TIM1_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;          //一个更新事件且计数器重新由RCR值（N）开始计数。周期???
  TIM_TimeBaseInit(TIM3, &TIM1_TimeBaseStructure);                      // 高速总线时钟频率36MHz,周期=36M/(36000-1+1)/1000

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = c3;                                 
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC3Init(TIM3,&TIM_OCInitStructure);                     			//使能通道3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = c4; 
  TIM_OC4Init(TIM3,&TIM_OCInitStructure);                      			//使能通道4
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
 
  TIM_ARRPreloadConfig(TIM3, ENABLE);                                     
  TIM_Cmd(TIM3,ENABLE);
  
}