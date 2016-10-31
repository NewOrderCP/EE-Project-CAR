#include "sys.h"
#include "motor.h"
#include "PID.h"
#include "inf.h"
#include "delay.h"
#include "timer.h" 
#include "MPU6050.h"
#include "HMC5883L.h"

extern float angle;

float dev(int expect)
{
	int devang=0;
	int yaw1=0;
	float devout=0;
	
	if(angle<0)
		yaw1=(int)angle+360;
	else
		yaw1=(int)angle;
	
	devang=(yaw1-expect)%360;
	
	if(devang>180)
		devang=devang-360;
	
	devout=(float)devang/20;
	
	return devout;
}

	
