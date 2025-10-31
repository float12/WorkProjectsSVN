#ifndef __RELAYERREVENT
#define __RELAYERREVENT

#if( SEL_EVENT_RELAY_ERR == YES )

typedef struct TRelayErrData_t
{
	// 第一层：正反有功总电能
	// 第二层：负荷开关误动作前后
	DWORD Energy[2][2];
	BYTE	Reserved[10];		// 预留
}TRelayErrData;

// 负荷开关误动作事件
typedef struct TRelayErrRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TRelayErrData)];
}TRelayErrRom;

void FactoryInitRelayErrPara( void );
BYTE GetRelayErrStatus(BYTE Phase);
#endif

#endif
