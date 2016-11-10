#include "mpu6050.h"
#include "delay.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "usart.h"
#include "inv_mpu.h"
#include "stm32f10x.h"
#include "mpuiic.h"   
#include "OLED_I2C.h"

// #define UART_SEND_CONTROL 0
// #define UART_RATE 500000
 //如果定义了UART_SEND_CONTROL,则启动串口调试命令
 //UART_RATE控制UART初始化的波特率

	float pitch,roll,yaw; 		//欧拉角
	short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
//上述是全局变量，在中断程序被刷新，中断服务函数在初始化函数后面


void mpu_exti_init(void);

u8 display5[16];

//*******************************MPU6050初始化****************************//
//初始化MPU6050并初始化DMP
//参数int the_fifo_rate:设置MPU6050刷新FIFO数据（产生中断）的速度,
//                     也是STM32刷新全局变量的速度
u8 MPU_Init(int the_fifo_rate)  
{ 
	u8 res;//用来储存MPU6050地址
	MPU_IIC_Init();//初始化IIC总线
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
	delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050 
	MPU_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
	MPU_Set_Rate(50);						//设置采样率50Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//关闭所有中断
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
	
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);  //读一下mpu的ID，确认已经正确启动
	if(res==MPU_ADDR)//器件ID正确
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//加速度与陀螺仪都工作
	  while(mpu_dmp_init());//启动DMP  ！！	
	 dmp_set_fifo_rate(the_fifo_rate);//设置FIFO刷新速度
	 mpu_exti_init();//设置并启动中断
 	}else return 1;
	return 0;
}



//*****************************中断服务函数*****************************//
 //每当DMP准备好一组数据后，就进入这一个中断程序
 //中断启动的频率通过set_fiforate来控制
void EXTI9_5_IRQHandler()   
{
static unsigned char count_to_delay=0;
count_to_delay=(count_to_delay+1)%50;

if(EXTI_GetITStatus(EXTI_Line8))
{   
   EXTI_ClearITPendingBit(EXTI_Line8);//清除中断  

//		GPIO_SetBits(GPIOA,GPIO_Pin_12);
//		sprintf((char*)display5,"MPU");
//		OLED_ShowStr(0,4,display5,2);

   MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
   MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
   mpu_dmp_get_data(&pitch,&roll,&yaw);    //得到姿态
// #ifdef UART_SEND_CONTROL
//   usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));	
// #endif    
// //如果需要串口调试，则传送姿态
//    GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}

}


//***************************STM32中断设置*****************************//
void mpu_exti_init()    //连接MPU6050的中断引脚
{
GPIO_InitTypeDef GPIO_InitStruct;
EXTI_InitTypeDef EXTI_InitStruct;
NVIC_InitTypeDef  NVIC_InitStruct;

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //GPIOA   

GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
GPIO_InitStruct.GPIO_Pin= GPIO_Pin_8;      //PIN8 
GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStruct);     //GPIOA

//GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
//GPIO_InitStruct.GPIO_Pin= GPIO_Pin_12;      //PIN12
//GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
//GPIO_Init(GPIOA, &GPIO_InitStruct);     //GPIOA


GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);  //GPIOA,PIN8

EXTI_InitStruct.EXTI_LineCmd=ENABLE;
EXTI_InitStruct.EXTI_Line=EXTI_Line8;       //pin8
EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
EXTI_Init(&EXTI_InitStruct);

NVIC_InitStruct.NVIC_IRQChannel= EXTI9_5_IRQn;     //EXTI9_5_IRQHandler
NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;   //抢占优先级1
NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;          //响应优先级1
NVIC_Init(&NVIC_InitStruct);
}



//************************为MPU初始化服务的函数******************************//
u8 MPU_Set_Gyro_Fsr(u8 fsr)//设置陀螺仪满量程范围  
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);
}

u8 MPU_Set_Accel_Fsr(u8 fsr)//设置加速度传感器满量程范围  
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);
}


//设置MPU6050的数字低通滤波器
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}



//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}




//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}


//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}


//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;
}



//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i; 
    MPU_IIC_Start(); 
	MPU_IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	if(MPU_IIC_Wait_Ack())	//等待应答
	{
		MPU_IIC_Stop();		 
		return 1;		
	}
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答
	for(i=0;i<len;i++)
	{
		MPU_IIC_Send_Byte(buf[i]);	//发送数据
		if(MPU_IIC_Wait_Ack())		//等待ACK
		{
			MPU_IIC_Stop();	 
			return 1;		 
		}		
	}    
    MPU_IIC_Stop();	 
	return 0;	
} 


//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
 	MPU_IIC_Start(); 
	MPU_IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	if(MPU_IIC_Wait_Ack())	//等待应答
	{
		MPU_IIC_Stop();		 
		return 1;		
	}
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答
    MPU_IIC_Start();
	MPU_IIC_Send_Byte((addr<<1)|1);//发送器件地址+读命令	
    MPU_IIC_Wait_Ack();		//等待应答 
	while(len)
	{
		if(len==1)*buf=MPU_IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf=MPU_IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
    MPU_IIC_Stop();	//产生一个停止条件 
	return 0;	
}


//IIC写一个字节 
//reg:寄存器地址
//data:数据
u8 MPU_Write_Byte(u8 reg,u8 data) 				 
{ 
    MPU_IIC_Start(); 
	MPU_IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
	if(MPU_IIC_Wait_Ack())	//等待应答
	{
		MPU_IIC_Stop();		 
		return 1;		
	}
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答 
	MPU_IIC_Send_Byte(data);//发送数据
	if(MPU_IIC_Wait_Ack())	//等待ACK
	{
		MPU_IIC_Stop();	 
		return 1;		 
	}		 
    MPU_IIC_Stop();	 
	return 0;
}
//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 MPU_Read_Byte(u8 reg)
{
	u8 res;
    MPU_IIC_Start(); 
	MPU_IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
	MPU_IIC_Wait_Ack();		//等待应答 
    MPU_IIC_Send_Byte(reg);	//写寄存器地址
    MPU_IIC_Wait_Ack();		//等待应答
    MPU_IIC_Start();
	MPU_IIC_Send_Byte((MPU_ADDR<<1)|1);//发送器件地址+读命令	
    MPU_IIC_Wait_Ack();		//等待应答 
	res=MPU_IIC_Read_Byte(0);//读取数据,发送nACK 
    MPU_IIC_Stop();			//产生一个停止条件 
	return res;		
}


//***************************串口调试设置设置*****************************//
//如果定UART_SEND_CONTROL，则执行这一块儿的编译
#ifdef UART_SEND_CONTROL
void usart1_send_char(u8 c)
{   	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	USART_SendData(USART1,c);  
} 
//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X88;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//发送数据到串口1 
}


//通过串口1上报结算后的姿态数据给电脑
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//清0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//飞控显示帧,0XAF
}  
#endif



