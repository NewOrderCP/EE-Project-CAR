#include "stm32f10x.h"
struct PID
	{
	float Kp;
	float Ki;
	float Kd;
	float error_0;//��������
	float error_1;//һ��г������
	float error_2;//����г������
	long  Sum_error;
	int OutputValue;//ʵ�������
	float OwenValue;//�����ʱ�ı�׼�����
	};
int PID_calculate( struct PID *Control,float CurrentValue_left);      //λ��PID����