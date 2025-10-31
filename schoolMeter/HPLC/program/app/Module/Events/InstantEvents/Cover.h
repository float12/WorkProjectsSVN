#ifndef __COVER
#define __COVER

#include "event.h"

#if((SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES))

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct TEventMeterCover_t
{
	#if( MAX_PHASE == 1)
	DWORD	Energy[4]; 	// 发生，结束时刻正反有功总电能
	BYTE	Reserved[10];		// 预留
	#else
	DWORD	Energy[12];	// 发生，结束时刻正反有功总电能,四象限无功电能
	BYTE	Reserved[10];		// 预留
	#endif
}TEventCover;
#endif

#if( SEL_EVENT_METERCOVER == YES )
// 开表盖事件
typedef struct TMeterCover_t
{
	TEventOADCommonData		EventOADCommonData;							// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TEventCover)];
}TMeterCoverRom;

typedef struct TLowPowerMeterCoverData_t
{
	//低功耗下开盖数据，上电处理事件
	//0 -- 开始	1 -- 结束
	BYTE MeterCoverStatus[2];
	TRealTimer MeterCoverOpenTime;
	TRealTimer MeterCoverCloseTime;
	DWORD CheckMeterCoverStatus;
}TLowPowerMeterCoverData;
#endif

#if( SEL_EVENT_BUTTONCOVER == YES )
// 开端钮盖
typedef struct TButtonCover_t
{
	TEventOADCommonData		EventOADCommonData;							// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TEventCover)];
}TButtonCoverRom;

typedef struct TLowPowerButtonCoverData_t
{
	//低功耗下开盖数据，上电处理事件
	//0 -- 开始	1 -- 结束
	BYTE ButtonCoverStatus[2];
	TRealTimer ButtonCoverOpenTime;
	TRealTimer ButtonCoverCloseTime;
	DWORD CheckButtonCoverStatus;
}TLowPowerButtonCoverData;

#endif

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
#if( SEL_EVENT_METERCOVER == YES )
// 判断开表盖是否发生
BYTE GetMeterCoverStatus(BYTE Phase);
void MeterCoverPowerOn( void );
void MeterCoverPowerOff( void );
void ProcLowPowerMeterCoverRecord();
#endif

#if( SEL_EVENT_BUTTONCOVER == YES )
// 判断开端钮盖是否发生
BYTE GetButtonCoverStatus(BYTE Phase);
void ButtonCoverPowerOn( void );
void ButtonCoverPowerOff( void );
void ProcLowPowerButtonCoverRecord();
#endif

void FactoryInitCoverPara( void );
#endif
