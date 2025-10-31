//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.26
//描述		全失压事件处理文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __LOST_ALL_V_H
#define __LOST_ALL_V_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAX_LOST_ALL_V		1
#define START_TEST_LOST_ALLV	58

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//硬件全失压功能的一些结构定义
typedef struct TLostAllVData_t
{

	TRealTimer LostAllVStartTime;
	DWORD Current[4];// A B C N
	TRealTimer LostAllVEndTime;
	WORD Sum;

}TLostAllVData;


typedef struct TPrgInfoLostAllVData_t
{
	DWORD Current[4]; 	//A B C N电流
	BYTE	Reserved[10];		// 预留
}TPrgInfoLostAllVData;


//全失压
typedef struct TLostAllVRom_t
{
	TEventOADCommonData		EventOADCommonData;								// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TPrgInfoLostAllVData)];
}TLostAllVRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern __no_init DWORD LostAllVFlag;
extern __no_init TLostAllVData LostAllVData[MAX_LOST_ALL_V];

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void AllLostVCheck( BYTE Type );
BYTE GetLostAllVStatus(BYTE Phase);
void PowerUpDealLostAllV( void );
void PowerDownDealLostAllV( void );
void FactoryInitLostAllVPara( void );
#endif//#ifndef __LOST_ALL_V_H
