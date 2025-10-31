#ifndef __COS_OVER
#define __COS_OVER

#if( SEL_EVENT_COS_OVER == YES )

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct TCosoverData_t
{
#if(MAX_PHASE == 1)
	// 第一层：正反有功总电能
	// 第二层：开始、结束时刻
	DWORD	Energy[2][2];
	BYTE	Reserved[10];		// 预留 
#else
	// 第一层：正反有功、组合无功1,2
	// 第二层：开始、结束时刻
	DWORD	Energy[4][2];
	BYTE	Reserved[10];		// 预留	
#endif
}TCosoverData;

// 功率因数超限事件
typedef struct TCosoverRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TCosoverData)];
}TCosoverRom;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
BYTE GetCosoverStatus(BYTE Phase);
 void FactoryInitCosOverPara( void );
#endif

#endif
