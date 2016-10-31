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

/********************ע�ᶨʱ��4�жϴ�����***********************/

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/******************ע�ᴮ��1�жϷ�����************************/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//���ô����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//ռ��ʽ���ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //�����ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;//�жϽ�ֹ
	NVIC_Init(&NVIC_InitStructure);//�жϳ�ʼ��


}
void Control_Init(void)
{
	NVIC_Configuration();
}


/*****************�������˲�**************************************************/
