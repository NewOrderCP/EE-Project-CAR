#ifndef __HCSR04_H_
#define __HCSR04_H_	 
#include "stm32f10x.h"

void HC_SR04_Init(void);
void Tim2_Init(u16 arr,u16 psc);
void hc04_running(void);

#endif