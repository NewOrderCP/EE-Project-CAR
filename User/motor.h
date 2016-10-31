#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "stm32f10x.h"
#include "sys.h"

#define	 inf1 PAin(2)
#define	 inf2 PAin(3)
#define	 inf3 PAin(4)
#define	 inf4 PAin(5)
#define	 inf5 PAin(6)
#define	 inf6 PAin(7)
#define	 inf7 PBin(8)
#define	 inf8 PBin(9)
#define	 inf9 PBin(10)
#define	 inf10 PBin(11)
#define	 inf11 PBin(12)
#define	 inf12 PBin(13)
#define	 inf13 PBin(14)
#define	 inf14 PBin(15)
#define	 inf15 PAin(8)
	 
void PWM_Configuration(void);	 //≈‰÷√PWM ‰≥ˆ
void go(int x,int pwm);
void Pulse(int c1,int c2);
void GPIO_Configuration(void);
void rebuild(void);
#endif
