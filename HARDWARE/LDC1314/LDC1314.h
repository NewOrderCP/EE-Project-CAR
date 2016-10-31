#ifndef __LDC1314_H_
#define __LDC1314_H_
#include "stm32f10x.h"

#define DATA_CH0							0x00		//Channel 0 Conversion Result and Error Status													通道0转换结果和错误状态
#define DATA_CH1							0x02		//Channel 1 Conversion Result and Error Status												
#define DATA_CH2							0x04		//Channel 2 Conversion Result and Error Status (LDC1314 only)
#define DATA_CH3							0x06		//Channel 3 Conversion Result and Error Status (LDC1314 only)
#define RCOUNT_CH0						0x08		//Reference Count setting for Channel 0																	设置通道0参考计数  和下面的不知道有什么区别...
#define RCOUNT_CH1						0x09		//Reference Count setting for Channel 1
#define RCOUNT_CH2						0x0A		//Reference Count setting for Channel 2. (LDC1314 only)
#define RCOUNT_CH3						0x0B		//Reference Count setting for Channel 3.(LDC1314 only)
#define OFFSET_CH0						0x0C		//Offset value for Channel 0																						通道0偏移值
#define OFFSET_CH1						0x0D		//Offset value for Channel 1
#define OFFSET_CH2						0x0E		//Offset value for Channel 2 (LDC1314 only)
#define OFFSET_CH3						0x0F		//Offset value for Channel 3 (LDC1314 only)
#define SETTLECOUNT_CH0				0x10		//Channel 0 Settling Reference Count																		设置通道0参考计数
#define SETTLECOUNT_CH1				0x11		//Channel 1 Settling Reference Count
#define SETTLECOUNT_CH2				0x12		//Channel 2 Settling Reference Count (LDC1314 only)
#define SETTLECOUNT_CH3				0x13		//Channel 3 Settling Reference Count (LDC1314 only)
#define CLOCK_DIVIDERS_CH0		0x14		//Reference and Sensor Divider settings for Channel 0										通道0的分频设置
#define CLOCK_DIVIDERS_CH1		0x15		//Reference and Sensor Divider settings for Channel 1
#define CLOCK_DIVIDERS_CH2		0x16		//Reference and Sensor Divider settings for Channel 2 (LDC1314 only)
#define CLOCK_DIVIDERS_CH3		0x17		//Reference and Sensor Divider settings for Channel 3 (LDC1314 only)
#define STATUS								0x18		//Device Status Report																									状态报告
#define Device Status Report	0x19		//Error Reporting Configuration																					错误报告配置
#define CONFIG								0x1A		//Conversion Configuration																							转换配置
#define MUX_CONFIG						0x1B		//Channel Multiplexing Configuration																		信道复用配置
#define RESET_DEV							0x1C		//Reset Device																													复位设备
#define CLOCK_CONFIG					0x1D
#define DRIVE_CURRENT_CH0			0x1E		//Channel 0 sensor current drive configuration													通道0当前驱动配置
#define DRIVE_CURRENT_CH1			0x1F		//Channel 1 sensor current drive configuration
#define DRIVE_CURRENT_CH2			0x20		//Channel 2 sensor current drive configuration (LDC1314 only)
#define DRIVE_CURRENT_CH3			0x21		//Channel 3 sensor current drive configuration (LDC1314 only)
#define MANUFACTURER_ID				0x7E		//Manufacturer ID																												制造商ID
#define DEVICE_ID							0x7F		//Device ID																															设备ID

#define LDC1314_SLAVE_ADDRESS		0x2A	

#define EVM_DEFAULTS_SIZE 24 // 13 registers, 0x08 - 0x14

int devldc(u16 d[4]);
void LDC1314_INT(void);
u8 LDC1314_Init (void);
void LDC1314_GetDat (u16 DatBuf[]);
u8 LDC1314_ReadData (u8 SlaveAddress, u8 reg_add,u16 *Read);
u8 LDC1314_WriteData (u8 SlaveAddress, u8 REG_Address,uint16_t REG_data);

#endif
