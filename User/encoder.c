#include "stm32f10x.h"
#include "encoder.h"
/*************************定时器TIM2初始化****************************/
void TIM2_Encoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//定义一个GPIO结构体变量
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定义一个定时器结构体变量
  TIM_ICInitTypeDef TIM_ICInitStructure;	//定义一个定时器编码器结构体变量

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能定时器2
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;		//配置PA0->TIM2_CH1,PA1->TIM2_CH2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//设置50MHz时钟
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//设置为下拉输入模式
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseStructure.TIM_Period = 0xffff;	//计数器最大值	
  TIM_TimeBaseStructure.TIM_Prescaler = 0;	//时钟不分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;	// 使用的采样频率之间的分频比例
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	//初始化定时器2

  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12,TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);
  
	TIM_SetCounter(TIM2, 0x7fff);
 
//  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		//清除定时器2中断标志位
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);

//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);		//打开定时器2中断
  TIM_Cmd(TIM2, ENABLE);  //计数器使能，开始计数
}


/*************************定时器TIM3初始化****************************/
void TIM3_Encoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//定义一个GPIO结构体变量
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定义一个定时器结构体变量
  TIM_ICInitTypeDef TIM_ICInitStructure;	//定义一个定时器编码器结构体变量

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器3
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;		//配置PA6->TIM3_CH1,PA7->TIM3_CH2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//设置50MHz时钟
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//设置为下拉输入模式
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseStructure.TIM_Period = 0xffff;	//计数器最大值	
  TIM_TimeBaseStructure.TIM_Prescaler = 0;	//时钟不分频
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;	// 使用的采样频率之间的分频比例
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	//初始化定时器2

  TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);
  
	TIM_SetCounter(TIM3, 0x7fff);
 
//  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		//清除定时器2中断标志位
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);

//	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);		//打开定时器3中断
  TIM_Cmd(TIM3, ENABLE);  //计数器使能，开始计数
}
void Encoder_Init(void)
{
	TIM2_Encoder_Init();	//初始化定时器2
	TIM3_Encoder_Init();	//初始化定时器3	
}
