#include "stm32f10x.h"
#include "MPU6050.h"
#include "motor.h"
#include "control.h"
#include <math.h>

void NVIC_Configuration(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

/********************注册定时器4中断处理函数***********************/

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/******************注册串口1中断服务函数************************/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//配置串口中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//占先式优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //副优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;//中断禁止
	NVIC_Init(&NVIC_InitStructure);//中断初始化


}
void Control_Init(void)
{
	NVIC_Configuration();
}


/*****************卡尔曼滤波**************************************************/
