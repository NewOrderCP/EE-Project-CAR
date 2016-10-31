#include "oycq_delay.h"
#include "mpu6050.h"



void oycq_delay_ms(u16 oycq_ms)
{
int round_num1,round_num2;
for (round_num1=0;round_num1<oycq_ms;round_num1++)
   for (round_num2=0;round_num2<10350;round_num2++);  
}

void oycq_delay_us(u32 oycq_us)
{
int round_num1,round_num2;
for (round_num1=0;round_num1<oycq_us;round_num1++)
   for (round_num2=0;round_num2<9;round_num2++);  
}

