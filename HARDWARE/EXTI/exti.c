#include "exti.h"
#include "delay.h"
#include "motor.h"
#include "sys.h"

int irq=0;

//外部中断初始化函数
void EXTIX_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	 	EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA时钟
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入
		GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC 15
	
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA 11 12

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟

    //GPIOC.15 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
}

 
void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);    //消抖
	if(PCin(15)==1)
	{	  	
			if(PAin(12)==1)
				steer(shu,ping-allread()*0.6);
			if(PAin(12)==0)
				steer(shu,ping-allread()*0.6);
			
			delay_ms(1000);
			
	//		steer1();[]
			
	//		delay_ms(500);
			shoot();
			delay_ms(30000);
			delay_ms(30000);
			delay_ms(30000);
			delay_ms(30000);
			delay_ms(30000);
			delay_ms(30000);
	
			irq=1;
			
		EXTI_ClearITPendingBit(EXTI_Line15);  //清除EXTI15线路挂起位		
	}

}
