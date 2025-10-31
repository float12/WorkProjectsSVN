//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.9
//描述		统一管理所有的板件，实现工程文件的分级，这样可以避免增加板时改动工程
//修改记录	
//----------------------------------------------------------------------
#ifndef __BOARD_API_H
#define __BOARD_API_H


//#include ".\SingleMeter\board_ht_698_Single.H"
//#include ".\ThreeMeter\board_ht_698_Three.H"
//#include ".\ThreeMeter\board_st_698_Three.H"
#include ".\SeparateType\board_SeparateType.H"
//-----------------------------------------------
//				宏定义
//-----------------------------------------------	
#define IR_WAKEUP_LIMIT_TIME		604800	//7天秒数

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
}eDETECT_POWER_MODE;

typedef struct TLowPowerConRom_t
{
	BYTE DetectFlag;
}TLowPowerConRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------


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


#endif//#ifndef __BOARD_API_H


