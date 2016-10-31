#include "stm32f10x.h"
#include "encoder.h"
/*************************��ʱ��TIM2��ʼ��****************************/
void TIM2_Encoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ��GPIO�ṹ�����
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//����һ����ʱ���ṹ�����
  TIM_ICInitTypeDef TIM_ICInitStructure;	//����һ����ʱ���������ṹ�����

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ�ܶ�ʱ��2
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;		//����PA0->TIM2_CH1,PA1->TIM2_CH2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//����50MHzʱ��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//����Ϊ��������ģʽ
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseStructure.TIM_Period = 0xffff;	//���������ֵ	
  TIM_TimeBaseStructure.TIM_Prescaler = 0;	//ʱ�Ӳ���Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;	// ʹ�õĲ���Ƶ��֮��ķ�Ƶ����
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	//��ʼ����ʱ��2

  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12,TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);
  
	TIM_SetCounter(TIM2, 0x7fff);
 
//  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		//�����ʱ��2�жϱ�־λ
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);

//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);		//�򿪶�ʱ��2�ж�
  TIM_Cmd(TIM2, ENABLE);  //������ʹ�ܣ���ʼ����
}


/*************************��ʱ��TIM3��ʼ��****************************/
void TIM3_Encoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ��GPIO�ṹ�����
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//����һ����ʱ���ṹ�����
  TIM_ICInitTypeDef TIM_ICInitStructure;	//����һ����ʱ���������ṹ�����

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ�ܶ�ʱ��3
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;		//����PA6->TIM3_CH1,PA7->TIM3_CH2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//����50MHzʱ��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//����Ϊ��������ģʽ
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseStructure.TIM_Period = 0xffff;	//���������ֵ	
  TIM_TimeBaseStructure.TIM_Prescaler = 0;	//ʱ�Ӳ���Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;	// ʹ�õĲ���Ƶ��֮��ķ�Ƶ����
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	//��ʼ����ʱ��2

  TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);
  
	TIM_SetCounter(TIM3, 0x7fff);
 
//  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		//�����ʱ��2�жϱ�־λ
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);

//	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);		//�򿪶�ʱ��3�ж�
  TIM_Cmd(TIM3, ENABLE);  //������ʹ�ܣ���ʼ����
}
void Encoder_Init(void)
{
	TIM2_Encoder_Init();	//��ʼ����ʱ��2
	TIM3_Encoder_Init();	//��ʼ����ʱ��3	
}
