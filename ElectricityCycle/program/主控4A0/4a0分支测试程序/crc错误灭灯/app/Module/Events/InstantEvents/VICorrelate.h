//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.10.27
//描述		与电压、电流相关的事件记录
//修改记录	
//---------------------------------------------------------------------- 

#ifndef __VI_CORRELATE
#define __VI_CORRELATE

#include "event.h"
//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
// 过流事件数据结构体
typedef struct TVICorrelateData_t
{
	// 分配的空间由于对齐问题，应该比实际占用的空间要大
	#if( MAX_PHASE == 1)
	DWORD	Energy[4]; 			// 发生，结束时刻正反有功总电能
	WORD	Voltage;			// 发生时刻电压
	DWORD	Current[2];			// 发生时刻电流
	DWORD	ActPower[2];		// 发生时刻有功功率
	WORD	Cos[2];				// 发生时刻功率因数
	BYTE	Reserved[12];		// 预留
	#else
	DWORD	AllEnergy[4*2]; 	// 发生,结束时刻正反有功总电能，组合无功1,2总电能
	DWORD	SepEnergy[4*3*2]; 	// 发生,结束时刻A,B,C 正反有功电能，组合无功1,2电能
	WORD	Voltage[3];			// 发生时刻电压
	DWORD	Current[4];			// 发生时刻电流
	DWORD	ActPower[4];		// 发生时刻有功功率
	DWORD	ReactPower[4];		// 发生时刻无功功率
	WORD	Cos[4];				// 发生时刻功率因数
	DWORD	AHour[4];			// 发生时刻安时值
	BYTE	Reserved[10];		// 预留
	#endif
}TVICorrelateData;

#if( SEL_EVENT_LOST_V == YES )
// 失压事件
typedef struct TLostVRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TVICorrelateData)];
}TLostVRom;
#endif//#if( SEL_EVENT_LOST_V == YES )

#if( SEL_EVENT_WEAK_V == YES )
// 欠压事件
typedef struct TWeakVRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TVICorrelateData)];
}TWeakVRom;
#endif//#if( SEL_EVENT_OVER_I == YES )

#if( SEL_EVENT_OVER_V == YES )
// 过压事件
typedef struct TOverVRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TVICorrelateData)];
}TOverVRom;
#endif//#if( SEL_EVENT_OVER_V == YES )

#if( SEL_EVENT_BREAK == YES )
// 断相事件
typedef struct TBreakRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TVICorrelateData)];
}TBreakRom;
#endif//#if( SEL_EVENT_BREAK == YES )

#if( SEL_EVENT_LOST_I == YES )
// 失流事件
typedef struct TLostIRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TVICorrelateData)];
}TLostIRom;
#endif//#if( SEL_EVENT_LOST_I == YES )

#if( SEL_EVENT_BREAK_I == YES )
// 断流事件
typedef struct TBreakIRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TVICorrelateData)];
}TBreakIRom;
#endif//#if( SEL_EVENT_BREAK_I == YES )

#if( SEL_EVENT_OVER_I == YES )
// 过流事件
typedef struct TOverIRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TVICorrelateData)];
}TOverIRom;
#endif//#if( SEL_EVENT_OVER_I == YES )

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void FactoryInitVICorrelatePara( void );

#if( SEL_EVENT_LOST_V == YES )
// 失压记录的状态检测
BYTE GetLostVStatus(BYTE Phase);
#endif//#if( SEL_EVENT_LOST_V == YES )

#if( SEL_EVENT_WEAK_V == YES )
// 欠压记录的状态检测
BYTE GetWeakVStatus(BYTE Phase);
#endif//#if( SEL_EVENT_WEAK_V == YES )

#if( SEL_EVENT_OVER_V == YES )
// 过压记录的状态检测
BYTE GetOverVStatus(BYTE Phase);
#endif//#if( SEL_EVENT_OVER_V == YES )

#if( SEL_EVENT_BREAK == YES )
// 断相记录的状态检测
BYTE GetBreakStatus(BYTE Phase);
#endif//#if( SEL_EVENT_BREAK == YES )

#if( SEL_EVENT_LOST_I == YES )
// 失流记录的状态检测
BYTE GetLostIStatus(BYTE Phase);
#endif//#if( SEL_EVENT_LOST_I == YES )

#if( SEL_EVENT_BREAK_I == YES )
// 断流记录的状态检测
BYTE GetBreakIStatus(BYTE Phase);
#endif//#if( SEL_EVENT_BREAK_I == YES )

#if( SEL_EVENT_OVER_I == YES )
// 过流记录的状态检测
BYTE GetOverIStatus(BYTE Phase);
#endif//#if( SEL_EVENT_OVER_I == YES )

#endif
