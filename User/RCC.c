#include"stmlib\\stm32f10x.h"
RCC_ClocksTypeDef  RCC_ClockFreq;
/*************************************************************************
**�������ƣ�RCC_Configuration()
**�������ܣ�RCC�Ļ������ã����ʱ�ӵ�����
**��ڲ�������
**���ڲ�������
*************************************************************************/
void RCC_Configuration()
{
	SystemInit();		  //���ô˺������RCC���������ã������ϵͳʱ�ӳ�ʼ��
	RCC_GetClocksFreq(&RCC_ClockFreq); //���ز�ͬƬ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//��GPIOA����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

}
