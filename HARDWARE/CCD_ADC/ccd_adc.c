#include "stm32f10x.h"
#include <stdio.h>
#include "sys.h"
#include "delay.h"
#include "Usart.h"
#include "motor.h"
#include "MPU6050.h"
#include "encoder.h"
#include "control.h"
#include "led.h"
#include "key.h"
#include "lcd.h" 
#include "timer.h" 
#include "ccd_adc.h"
#include "PID.H"
//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEKս��STM32������
//����2������CCD(TSL1401)����
//*********˵�����˳���ʹ��PB10��PB12�����������CLK��SI��ʹ��ADC1��ͨ��һ��ȡAO�����CCDʹ��3.3V����********
//*************���ߣ���TSL1401��SI���Ž�PB12��CLK���Ž�PB10��AO���Ž�PA1��VCC��3.3V��GND�ӵ�**************
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//��ʼ��GPIO
//������PB10��PB12����Ϊ�������״̬
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIOInitstructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIOInitstructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIOInitstructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12;
	GPIOInitstructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init (GPIOB,&GPIOInitstructure);
}		 
		   
//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3		
u16 ao_d[128];
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}

//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

//��β�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
	}
	return temp_val/times;
} 	 

//��PB12���ſ���SI
void SI(int a)
{
		 GPIO_Config();
	if(a==1)
	{	
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
	}
	else if(a==0)
	{	
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	}
}

//��PB10���ſ���CLK
void CLK(int a)
{
		 GPIO_Config();
	if(a==1)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_10);
	}
	else if(a==0)
	{	
		GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	}		
}
extern u16 find;
extern u32 sum;

//��һ�����ڲ��ɼ�,�൱��׼������������������
void CCD_Prepare(void)
{
	u16 n;

	SI(0);
	CLK(0);
  delay_us2(1);   //ccd��ÿ��AO�ɼ�ʱ�䲻�ɵ���0.125us������SI��CLK����ͳһʹ��2us
	SI(1);
	for(n=0;n<128;n++)
	{
		delay_us2(1);
		CLK(1);		
		SI(0);
		delay_us2(1);
		CLK(0);
	}
	delay_us2(1);
  CLK(1);
	delay_us2(1);
	CLK(0);	
}

//CCDѲ�ߺ���������CCD���������ؼ�⵽�ĺ���������������
//Exposure_timΪ�ع�ʱ�䣬��λʱ��Ϊ2us������Ϊ9879��Ϊ�ع�20ms���ع�ʱ�����Ϊ100ms
void CCD_Find(u16 Exposure_tim)
{
	
	u16 n,max,min,mid,bla,e=0;

	SI(0);
	CLK(0);
  delay_us2(1);   //ccd��ÿ��AO�ɼ�ʱ�䲻�ɵ���0.125us������SI��CLK����ͳһʹ��2us
	SI(1);
	for(n=0;n<128;n++)
	{
		delay_us2(1);
		CLK(1);		
		SI(0);
		delay_us2(1);
		ao_d[n]=Get_Adc_Average(ADC_Channel_0,1);
		CLK(0);
	}
	delay_us2(1);
  CLK(1);
	delay_us2(1);
	CLK(0);
  for(n=0;n<Exposure_tim;n++)   //�����ع���ʱ��ͨ���ı�Exposure_timʵ�֣����������
	{
		delay_us2(1);
		CLK(1);		
		delay_us2(1);
		CLK(0);		
  }
	//���ռ��������ݽ��ж�ֵ������
	max=0;
	sum=0;
	for(n=10;n<118;n++)
		{
			if(max<ao_d[n])
		    max=ao_d[n];
			sum=sum+ao_d[n-1];
		}			
	min=max;
		sum=sum/108;
	for(n=18;n<110;n++)
		if(min>ao_d[n])    
		    min=ao_d[n];
	 mid=(max+min)/2;
	if((max-min)>500)   //�ж�Ѳ��׼ȷ��
	{
		e=0, bla=0;
	  for(n=105;n>23;n--)
	  {
		  if(ao_d[n-1]<sum-600)
		  { 	
		    e+=n;
		    bla++;
		  }		
	  }
	  e=e/bla;
		if(e>1)
			find=e;
//		printf("%d\t",sum);
	
		LED1=0;
	}
	else
		LED1=1;
}




void SendImageData(u16 * ImageData) 
{


    unsigned char i;
    unsigned char crc = 0;

    /* Send Data */
     printf("%c",'*');
     printf("%c",'L');
     printf("%c",'D');

     printf("%x",0);
     printf("%x",0);
     printf("%x",0);
     printf("%x",0);
 
    for(i=0; i<128; i++) {
       printf("%x",*ImageData++/30);
    }

    printf("%c",crc);
    printf("%c",'#');
}
 




















