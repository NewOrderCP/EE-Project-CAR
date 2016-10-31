#ifndef __WAVES_H
#define __WAVES_H	

void NVIC1_Configuration(void);
void TIM_Configuration(void);
void GPIO1_Configuration(void);
float Sensor_using(void);
void delay(void);
float waves(void);
void waves_Init(void);
void TIM2_IRQHandler(void);

#endif
