//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.24
//描述		需量超限事件头文件
//修改记录	
//----------------------------------------------------------------------

#ifndef __DEMANDOver_H
#define __DEMANDOver_H

#if( SEL_DEMAND_OVER == YES )
//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
// 正向有功需量超限事件
typedef struct TDemandOverRom_t
{
	TEventOADCommonData		EventOADCommonData;			// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TEventNormal)];
	BYTE					Data[sizeof(TDemandData)];	// 超限期间正向有功需量最大值及发生时间
}TDemandOverRom;


//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
BOOL GetPADemandOverStatus( BYTE DemandOverType );

BOOL GetNADemandOverStatus( BYTE DemandOverType );

BOOL GetQDemandOverStatus( BYTE DemandOverType );

void FactoryInitDemandOverPara( void );

void DealDemandOverMaxDemand( void );
#endif//#if( SEL_DEMAND_OVER == YES )

#endif//__DEMANDOver_H

