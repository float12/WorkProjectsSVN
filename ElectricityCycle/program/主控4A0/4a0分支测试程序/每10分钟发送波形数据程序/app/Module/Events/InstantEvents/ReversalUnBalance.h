#ifndef __REVERSAL_UNBALANCE
#define __REVERSAL_UNBALANCE

#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )

// 不平衡事件的附属数据
typedef struct TUnBalanceData_t
{
#if(MAX_PHASE == 1)
	//电压最大不平衡率
	WORD MaxUnBalanceRate;
	// 总电能
	// 第一层：正反有功
	// 第二层：开始、结束时刻
	DWORD Energy[2][2];
	BYTE	Reserved[10];		// 预留 
#else
	// 最大不平衡率
	WORD MaxUnBalanceRate;

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

}TUnBalanceData;

// 逆相序的附属数据
typedef struct TReversalData_t
{
#if(MAX_PHASE == 1)
	// 总电能
	// 第一层：正反有功
	// 第二层：开始、结束时刻
	DWORD Energy[2][2];
	BYTE	Reserved[10];		// 预留 
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

	BYTE	Reserved[20];		// 预留 

#endif

}TReversalData;

//-----------------------------------------------
//函数功能: 刷新电压、电流不平衡事件发生期间的最大不平衡率
//
//参数: 			无
//                    
//返回值:  	无
//
//备注:	事件秒任务中调用
//-----------------------------------------------
void FreshUnBalanceRate(void);
//-----------------------------------------------
//函数功能: 返回电压、电流不平衡事件发生期间最大不平衡率
//
//参数: Type[in]	:	D7 -- 0 -- RAM中的不平衡率；1 -- EEPROM中的不平衡率
//								0 -- 电压不平衡率
//								1 -- 电流不平衡率
//								2 -- 电流严重不平衡率
//                    
//返回值:  	对应的不平衡率
//
//备注:
//-----------------------------------------------
WORD GetUnBalanceMaxRate(BYTE Type);
//-----------------------------------------------
//函数功能: 掉电存储电压、电流不平衡事件发生期间的最大不平衡率
//
//参数: 			无
//                    
//返回值:  	无
//
//备注:	掉电时调用
//-----------------------------------------------
void UnBalanceRatePowerOff(void);

#endif

#if( SEL_EVENT_V_REVERSAL == YES )
// 电压逆相序事件
typedef struct TVReversRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TReversalData)];
}TVReversRom;
#endif//#if( SEL_EVENT_V_REVERSAL == YES )

#if( SEL_EVENT_I_REVERSAL == YES )
// 电流逆相序事件
typedef struct TIReversRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TReversalData)];
}TIReversRom;
#endif//#if( SEL_EVENT_I_REVERSAL == YES )

#if( SEL_EVENT_V_UNBALANCE == YES )
// 电压不平衡事件
typedef struct TVUnbalanceRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TUnBalanceData)];
}TVUnbalanceRom;
#endif//#if( SEL_EVENT_V_UNBALANCE == YES )

#if( SEL_EVENT_I_UNBALANCE == YES )
// 电流不平衡事件
typedef struct TIUnbalanceRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TUnBalanceData)];
}TIUnbalanceRom;
#endif//#if( SEL_EVENT_I_UNBALANCE == YES )

#if( SEL_EVENT_I_S_UNBALANCE == YES )
// 电流严重不平衡事件
typedef struct TISUnbalanceRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TUnBalanceData)];
}TISUnbalanceRom;
#endif//#if( SEL_EVENT_I_S_UNBALANCE == YES )

#if( SEL_EVENT_V_REVERSAL == YES )
BYTE GetVReversalStatus(BYTE Phase);
#endif

#if( SEL_EVENT_I_REVERSAL == YES )
BYTE GetIReversalStatus(BYTE Phase);
#endif

#if( SEL_EVENT_V_UNBALANCE == YES )
BYTE GetVUnBalanceStatus(BYTE Phase);
#endif

#if( SEL_EVENT_I_UNBALANCE == YES )
BYTE GetIUnBalanceStatus(BYTE Phase);
#endif

#if( SEL_EVENT_I_S_UNBALANCE == YES )
BYTE GetISUnBalanceStatus(BYTE Phase);
#endif

#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
void FactoryInitReversalUnBalancePara( void );
#endif

#endif
