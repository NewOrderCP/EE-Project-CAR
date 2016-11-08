#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "hc04.h"

#define SIDE 3

u16 cnt;    //�����޶�������  ��hcrs04.h�пɼ�
u32 tim;		//��ʱʱ��    ��hcrs04.h�пɼ�
float dis1;				        //����

void HC_SR04_Init(void)										
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;					
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;		//ECHO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;		//TRIG	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//ģ�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOA, GPIO_Pin_4|GPIO_Pin_5);
	
	Tim2_Init(10, 71);
}

void hc04_running(void)
{
		PAout(4)=1;
		delay_us(15);
		PAout(4)=0;
		while(PAin(5) == 0);
		TIM_Cmd(TIM2, ENABLE);
		while(PAin(5) == 1);
		TIM_Cmd(TIM2, DISABLE);
		tim= TIM_GetCounter(TIM2);
		tim=tim+cnt*10;
		dis1=tim*0.017;//-SIDE;
		TIM2->CNT=0;											
		cnt=0; 	
}

void Tim2_Init(u16 arr,u16 psc)	//arr���Զ���װֵ��	 psc��ʱ��Ԥ��Ƶ��
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc;  //72MHz/7200=10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
	//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ITConfig(TIM2,	TIM_IT_Update, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

/*�жϷ�����򣺲�ѯ�жϱ�־λ�ж��Ƿ��ж�ʱ���жϡ�����жϹ���λ*/
void TIM2_IRQHandler(void)   //TIM2�жϷ����ӳ���
{
	/*���ָ����TIM�жϷ������:TIM �ж�Դ(ʵ���ϲ��ж�Ҳ�ǿ��Եģ�*/
	if (TIM_GetITStatus(TIM2, TIM_IT_Update ) != RESET)  
	{
		
		/*���TIMx���жϴ�����λ:TIM �ж�Դ */
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  
		cnt++;

		if(cnt == 2000)			//�����2000*10*0.00017=3.4m
		{
			cnt=0;
		}
	}
}