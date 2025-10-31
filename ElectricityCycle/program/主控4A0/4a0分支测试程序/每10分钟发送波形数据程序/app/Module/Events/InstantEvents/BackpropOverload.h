#ifndef __BACKPROP_OVERLOAD
#define __BACKPROP_OVERLOAD

#if((SEL_EVENT_BACKPROP==YES) || (SEL_EVENT_OVERLOAD==YES))

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//  分相功率反向、过载事件的附属数据
typedef struct TBackpropOverloadData_t
{
#if(MAX_PHASE == 1)
	// 总电能
	// 第一层：正反有功
	// 第二层：开始、结束时刻
	DWORD Energy[2][2];
	BYTE  Reserved[10];		// 预留	
#else
	// 总电能
	// 第一层：正反有功、组合无功1、组合无功2
	// 第二层：开始、结束时刻
	DWORD Energy[4][2];
	
	// A相电能
	// 第一层：正反有功、组合无功1、组合无功2
	// 第二层：开始、结束时刻
	DWORD EnergyA[4][2];
	
	// B相电能
	// 第一层：正反有功、组合无功1、组合无功2
	// 第二层：开始、结束时刻
	DWORD EnergyB[4][2];
	
	// C相电能
	// 第一层：正反有功、组合无功1、组合无功2
	// 第二层：开始、结束时刻
	DWORD EnergyC[4][2];

	BYTE	Reserved[10];		// 预留	
#endif	
}TBackpropOverloadData;

#endif

#if( SEL_EVENT_OVERLOAD == YES )
// 过载事件
typedef struct TOverloadRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TBackpropOverloadData)];
}TOverloadRom;
#endif

#if( SEL_EVENT_BACKPROP == YES )
// 功率反向事件
typedef struct TBackpropRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TBackpropOverloadData)];
}TBackpropRom;
#endif

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
#if( SEL_EVENT_OVERLOAD == YES )
void FactoryInitOverLoadPara( void );
BYTE GetOverloadStatus(BYTE Phase);
#endif

#if( SEL_EVENT_BACKPROP == YES )
void FactoryInitBackPopPara( void );
BYTE GetBackpropStatus(BYTE Phase);
#endif

#endif//#if((SEL_EVENT_BACKPROP==YES) || (SEL_EVENT_OVERLOAD==YES))
