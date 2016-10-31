#include "waves.h"
#include "stm32f10x.h"
#include <math.h>

#define module_interval 25    //定义超声波模块接收发射端间距(单位：mm)
#define object_distance 100.0   //检测物体的距离

int overflow=0;               //用于检测是否定时器已经溢出

void waves_Init(void)
{

  /*system clock init*/
  SystemInit();
  
  /*interrupt init*/
  NVIC1_Configuration();
  
  /*I\O init*/
  GPIO1_Configuration();
  
  /*TIM初始化*/
  TIM_Configuration();
	
}

float waves(void)
{
	return(Sensor_using());
}

void NVIC1_Configuration(void)
{
  /*CAN receive interrupt enable*/
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void GPIO1_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  /*Echo pin define*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /*Trigle pin define*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void TIM_Configuration(void)
{
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_DeInit(TIM2);

  TIM_TimeBaseStructure.TIM_Period=5000;
  TIM_TimeBaseStructure.TIM_Prescaler=0;
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_PrescalerConfig(TIM2,359,TIM_PSCReloadMode_Immediate);//时钟频率为72MHz/(359+1)=200000
  TIM_ARRPreloadConfig(TIM2, DISABLE);
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

  TIM_Cmd(TIM2, DISABLE);
}

/*返回值为毫米级*/
float Sensor_using(void)
{
  float distance=0;
  u16 TIM=0;
  
  GPIO_SetBits(GPIOA,GPIO_Pin_0);
  delay();
  GPIO_ResetBits(GPIOA,GPIO_Pin_0);
  TIM2->CNT=0;
  while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) && overflow==0);
  TIM_Cmd(TIM2, ENABLE);
  while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) && overflow==0);
  TIM_Cmd(TIM2, DISABLE);
  
  if(overflow!=0)
  {
	overflow=0;
	return 0.0;
  }
  TIM=TIM_GetCounter(TIM2);
  distance=(float)sqrt((TIM/20.0*17)*(TIM/20.0*17)-module_interval*module_interval/4.0)+12.0; //12.0为误差补偿
  
  return(distance);
}

void delay(void)
{
  u32 i;
  i=100000;
  while(i--);
}


void TIM2_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
  {
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    overflow++;
  }
}
