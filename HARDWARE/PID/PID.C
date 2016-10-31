#include "PID.H"
#include "stdio.h"
#include "stm32f10x.h"
float MaxValue=800;//�������޷�
float MinValue=-800;//�����С�޷�
float CurrentValue_left;//����ֵ
float CurrentValue_right;
char temp1[32];

/************************************************************************
**λ��PID�� KP��KI��KD����;
**�ڵ��Թ����У�ֻ��KP��KI�������˵���
**�ڱ�������У�ֻ������KP��
**ͬʱ���ڸ���������ֵ��ʵ�ʵĵ��ת��������������������� 
*************************************************************************/
   
 
//struct PID Control_right ={1,0,0,0,0,0,0,0,600};//=&Control;float OwenValue,float CurrentValue

   
int PID_calculate(struct PID *Control_left,float CurrentValue_left )//λ��PID����
{
	
	float Value_Kp;//��������
	float Value_Ki;//���ַ���
	float Value_Kd;//΢�ַ���
	
	Control_left->error_0     = Control_left->OwenValue-CurrentValue_left ;//��������
	Value_Kp                  = Control_left->Kp*Control_left->error_0 ;
	Control_left->Sum_error  += Control_left->error_0;     
	Value_Ki                  = Control_left->Ki*Control_left->Sum_error;
	Value_Kd                  = Control_left->Kd*(Control_left->error_0 + Control_left->error_2 - 2 * Control_left->error_1 );
	Control_left->error_2     = Control_left->error_1;//�������г��
	Control_left->error_1     = Control_left->error_0;//����һ��г��
	Control_left->OutputValue = (int)Value_Kp + (int)Value_Ki + (int)Value_Kd;//���ֵ���㣬ע��Ӽ�
	if( Control_left->OutputValue>MaxValue)//�޷�
	Control_left->OutputValue=MaxValue;
	if (Control_left->OutputValue<MinValue)
	Control_left->OutputValue= MinValue;
	return Control_left->OutputValue;
}
