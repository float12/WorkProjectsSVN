//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.9
//描述		统一管理所有的板件，实现工程文件的分级，这样可以避免增加板时改动工程
//修改记录	
//----------------------------------------------------------------------
#ifndef __BOARD_API_H
#define __BOARD_API_H

#include ".\ThreeMeter\board_F4A0_698_Three.H"
#include "cpuHC32F4A0.h"
//-----------------------------------------------
//				宏定义
//-----------------------------------------------	
#define IR_WAKEUP_LIMIT_TIME		604800	//7天秒数
#define MEM_ZERO_ARRAY(x)               do {                                   \
                                        memset((void*) (x), 0L, (sizeof(x)));  \
                                        }while(0)
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eFromOnInitEnterDownMode,	//从上电初始化进入低功耗
	eFromOnRunEnterDownMode,	//上电运行后进入低功耗
	eFromWakeupEnterDownMode,	//低功耗唤醒后再进入低功耗
	eFromDetectEnterDownMode,	//低功耗进行电源异常或全失压检测后再进入低功耗
}eENTER_DOWN_MODE;

typedef enum
{
	eOnInitDetectPowerMode,		//上电系统电源检测
	eOnRunDetectPowerMode,		//正常运行期间的系统电源检测
	eWakeupDetectPowerMode,		//低功耗唤醒期间系统电源检测
	eSleepDetectPowerMode,		//低功耗休眠期间系统电源检测
	eOnRunSpeedDetectPowerMode,	//正常运行快速系统电源检测
}eDETECT_POWER_MODE;

typedef struct TLowPowerConRom_t
{
	BYTE DetectFlag;
}TLowPowerConRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern WORD HSDCTimer;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 系统电源检测
//
//参数: 	
//          Type[in]:	
//				eOnInitDetectPowerMode		上电系统电源检测
//				eOnRunDetectPowerMode		正常运行期间的系统电源检测
//				eWakeupDetectPowerMode		低功耗唤醒期间系统电源检测
//				eSleepDetectPowerMode		低功耗休眠期间系统电源检测
//返回值: 	TRUE:有电   FALSE:没有电
//
//备注:   上电vcc和比较器都判，掉电只判LVDIN0比较器，因为vcc上有超级电容
//-----------------------------------------------
BOOL api_CheckSysVol( eDETECT_POWER_MODE Type );

//-----------------------------------------------
//函数功能: 上报载波通道的管脚配置
//
//参数: 	0x00:上报前
//		0x55:上报后
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ReportCRWakeUpConfig(BYTE Type);

//-----------------------------------------------
//函数功能: 进入低功耗及相应模块处理
//
//参数: 	Type[in]
//				eFromOnInitEnterDownMode	从上电初始化进入低功耗
//				eFromOnRunEnterDownMode		上电运行后进入低功耗
//				eFromWakeupEnterDownMode	低功耗唤醒后再进入低功耗
//				eFromDetectEnterDownMode	低功耗进行电源异常或全失压检测后再进入低功耗
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_EnterLowPower(eENTER_DOWN_MODE Type);

//-----------------------------------------------
//函数功能: 在唤醒状态下的低功耗任务，定时进入低功耗状态
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_LowPowerTask(void);

//-----------------------------------------------
//函数功能: 获取掉电是否完成标志
//
//参数: 	无

//返回值:  	正常完成掉电返回TRUE,未完成掉电返回FALSE
//
//备注:   
//-----------------------------------------------
BOOL api_GetPowerDownCompleteFlag( void );

//-----------------------------------------------
//函数功能: 初始化SPI总线
//
//参数:
//			Component[in]	元器件eCOMPONENT_TYPE
//			Type[in]		SPI的模式  eSPI_MODE_0/eSPI_MODE_1/eSPI_MODE_2/eSPI_MODE_3分别对应SPI的模式0/1/2/3
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_InitSPI( eCOMPONENT_TYPE Component, eSPI_MODE Type );

//-----------------------------------------------
//函数功能: 非中断方式下写SPI的函数
//
//参数:
//			Data[in]		写入的值
//
//返回值:  	读出的值
//
//备注:
//-----------------------------------------------
BYTE api_UWriteSpi( eCOMPONENT_TYPE Component, BYTE Data ); 

#if( PREPAY_MODE == PREPAY_LOCAL )
//-----------------------------------------------
//函数功能: 获取掉电阶梯结算次数
//
//参数: 	Buf[out]:掉电期间经过阶梯结算时间

//返回值:  	掉电期间经过阶梯结算次数
//
//备注:   
//-----------------------------------------------
BYTE api_GetPowerUpYearBillInfo( BYTE *Buf );
#endif

//-----------------------------------------------
//函数功能:HDSC—SPI初始化
//
//参数: 
//                    
//返回值: 
//
//备注: 
//-----------------------------------------------
void api_TopWaveSpiInit(BYTE SampleChipNo);

//-----------------------------------------------
//函数功能:充电检测初始化
//
//参数: 
//                    
//返回值: 
//
//备注: 
//-----------------------------------------------
void api_ChargeDetectInit();
//--------------------------------------------------
// 功能描述:  关闭拓扑
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void  api_DisTopoIqr( void );
#endif//#ifndef __BOARD_API_H


