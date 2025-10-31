//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.26
//描述		电源异常事件处理文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __POWERERR_H
#define __POWERERR_H

#include "event.h"

#if( SEL_EVENT_POWER_ERR == YES )

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define START_TEST_POWER_ERR		3

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct TPrgInfoPowerErrData_t
{
	DWORD Energy[2]; 	// 正反有功
	BYTE	Reserved[10];		// 预留
}TPrgInfoPowerErrData;

// 电源异常
typedef struct TPowerErrRom_t
{
	TEventOADCommonData		EventOADCommonData;								// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TPrgInfoPowerErrData)];
}TPowerErrRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern __no_init DWORD wPowerErrFlag;//模块内变量

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void PowerErrCheck( BYTE Type );
BYTE GetPowerErrStatus(BYTE Phase);
void FactoryInitPowerErrPara( void );
void PowerUpDealPowerErr( void );

#endif//#if( SEL_EVENT_POWER_ERR == YES )

#endif//#ifndef __POWERERR_H
