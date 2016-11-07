#include "PID.H"
#include "stdio.h"
#include "stm32f10x.h"
float MaxValue=1000;//输出最大限幅
float MinValue=-1000;//输出最小限幅
float CurrentValue_left;//采样值
float CurrentValue_right;
char temp1[32];

/************************************************************************
**位置PID的 KP，KI，KD参数;
**在调试过程中，只对KP，KI，进行了调节
**在本组参数中，只设置了KP。
**同时现在给定的理想值是实际的电机转动过程中输出的脉冲数。 
*************************************************************************/
   
 
//struct PID Control_right ={1,0,0,0,0,0,0,0,600};//=&Control;float OwenValue,float CurrentValue

   
int PID_calculate(struct PID *Control_left,float CurrentValue_left )//位置PID计算
{
	
	float Value_Kp;//比例分量
	float Value_Ki;//积分分量
	float Value_Kd;//微分分量
	
	Control_left->error_0     = Control_left->OwenValue-CurrentValue_left ;//基波分量
	Value_Kp                  = Control_left->Kp*Control_left->error_0 ;
	Control_left->Sum_error  += Control_left->error_0;     
	Value_Ki                  = Control_left->Ki*Control_left->Sum_error;
	Value_Kd                  = Control_left->Kd*(Control_left->error_0 + Control_left->error_2 - 2 * Control_left->error_1 );
	Control_left->error_2     = Control_left->error_1;//保存二次谐波
	Control_left->error_1     = Control_left->error_0;//保存一次谐波
	Control_left->OutputValue = (int)Value_Kp + (int)Value_Ki + (int)Value_Kd;//输出值计算，注意加减
	if( Control_left->OutputValue>MaxValue)//限幅
	Control_left->OutputValue=MaxValue;
	if (Control_left->OutputValue<MinValue)
	Control_left->OutputValue= MinValue;
	return Control_left->OutputValue;
}

