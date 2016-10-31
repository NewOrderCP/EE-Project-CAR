#include"stmlib\\stm32f10x.h"
RCC_ClocksTypeDef  RCC_ClockFreq;
/*************************************************************************
**函数名称：RCC_Configuration()
**函数功能：RCC的基本配置，完成时钟的设置
**入口参数：无
**出口参数：无
*************************************************************************/
void RCC_Configuration()
{
	SystemInit();		  //调用此函数完成RCC的所有设置，即完成系统时钟初始化
	RCC_GetClocksFreq(&RCC_ClockFreq); //返回不同片的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//给GPIOA分配时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

}
