#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

void Adc_Init(void);
u16  Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);
void SI(int a);
void CLK(int a);
void CCD_Prepare(void);
void CCD_Find(u16 Exposure_tim);
void GPIO_Config(void);
void SendImageData(u16 * ImageData) ;
extern u16 ao_d[128];
#endif 
