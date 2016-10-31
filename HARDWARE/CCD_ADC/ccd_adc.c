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
//ALIENTEK战舰STM32开发板
//蓝宙2代线性CCD(TSL1401)代码
//*********说明：此程序使用PB10，PB12推挽输出控制CLK与SI，使用ADC1的通道一读取AO输出，CCD使用3.3V供电********
//*************接线：将TSL1401的SI引脚接PB12，CLK引脚接PB10，AO引脚接PA1，VCC接3.3V，GND接地**************
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//初始化GPIO
//将引脚PB10，PB12设置为推挽输出状态
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIOInitstructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIOInitstructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIOInitstructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12;
	GPIOInitstructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init (GPIOB,&GPIOInitstructure);
}		 
		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3		
u16 ao_d[128];
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
}

//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

//多次采样求平均值
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

//以PB12引脚控制SI
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

//以PB10引脚控制CLK
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

//第一个周期不采集,相当于准备工作，这样做更好
void CCD_Prepare(void)
{
	u16 n;

	SI(0);
	CLK(0);
  delay_us2(1);   //ccd的每个AO采集时间不可低于0.125us，这里SI，CLK周期统一使用2us
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

//CCD巡线函数：控制CCD工作并返回检测到的黑线中心像素坐标
//Exposure_tim为曝光时间，单位时间为2us，设置为9879即为曝光20ms，曝光时间最大为100ms
void CCD_Find(u16 Exposure_tim)
{
	
	u16 n,max,min,mid,bla,e=0;

	SI(0);
	CLK(0);
  delay_us2(1);   //ccd的每个AO采集时间不可低于0.125us，这里SI，CLK周期统一使用2us
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
  for(n=0;n<Exposure_tim;n++)   //控制曝光延时可通过改变Exposure_tim实现，规则见上文
	{
		delay_us2(1);
		CLK(1);		
		delay_us2(1);
		CLK(0);		
  }
	//将收集到的数据进行二值化处理
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
	if((max-min)>500)   //判断巡线准确性
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
 




















