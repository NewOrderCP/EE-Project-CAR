#include "stm32f10x.h"
struct PID
	{
	float Kp;
	float Ki;
	float Kd;
	float error_0;//基波分量
	float error_1;//一次谐波分量
	float error_2;//二次谐波分量
	long  Sum_error;
	int OutputValue;//实际输出量
	float OwenValue;//零误差时的标准输出量
	};
int PID_calculate( struct PID *Control,float CurrentValue_left);      //位置PID计算
