#ifndef __MAGNETIC
#define __MAGNETIC

#if( SEL_EVENT_MAGNETIC_INT == YES )

// 恒定磁场干扰记录事件的附属数据
typedef struct TMagneticData_t
{
	// 第一层：开盖前、开盖后
	// 第二层：正反有功
	DWORD Energy[2][2];
	BYTE	Reserved[10];		// 预留
}TMagneticData;

// 强磁事件
typedef struct TMagneticRom_t
{
	TEventOADCommonData		EventOADCommonData;					// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TMagneticData)];
}TMagneticRom;

BYTE GetMagneticStatus(BYTE Phase);
void FactoryInitMagneticPara( void );
#endif

#endif
