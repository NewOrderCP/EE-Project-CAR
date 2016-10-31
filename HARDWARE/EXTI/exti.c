#include "exti.h"
#include "delay.h"
#include "motor.h"
#include "sys.h"

int irq=0;

//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	 	EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTAʱ��
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
		GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC 15
	
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
		GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA 11 12

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��

    //GPIOC.15 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
}

 
void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);    //����
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
			
		EXTI_ClearITPendingBit(EXTI_Line15);  //���EXTI15��·����λ		
	}

}
