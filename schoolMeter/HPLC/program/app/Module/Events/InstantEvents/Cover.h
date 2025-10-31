#ifndef __COVER
#define __COVER

#include "event.h"

#if((SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES))

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TEventMeterCover_t
{
	#if( MAX_PHASE == 1)
	DWORD	Energy[4]; 	// ����������ʱ�������й��ܵ���
	BYTE	Reserved[10];		// Ԥ��
	#else
	DWORD	Energy[12];	// ����������ʱ�������й��ܵ���,�������޹�����
	BYTE	Reserved[10];		// Ԥ��
	#endif
}TEventCover;
#endif

#if( SEL_EVENT_METERCOVER == YES )
// ������¼�
typedef struct TMeterCover_t
{
	TEventOADCommonData		EventOADCommonData;							// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TEventCover)];
}TMeterCoverRom;

typedef struct TLowPowerMeterCoverData_t
{
	//�͹����¿������ݣ��ϵ紦���¼�
	//0 -- ��ʼ	1 -- ����
	BYTE MeterCoverStatus[2];
	TRealTimer MeterCoverOpenTime;
	TRealTimer MeterCoverCloseTime;
	DWORD CheckMeterCoverStatus;
}TLowPowerMeterCoverData;
#endif

#if( SEL_EVENT_BUTTONCOVER == YES )
// ����ť��
typedef struct TButtonCover_t
{
	TEventOADCommonData		EventOADCommonData;							// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TEventCover)];
}TButtonCoverRom;

typedef struct TLowPowerButtonCoverData_t
{
	//�͹����¿������ݣ��ϵ紦���¼�
	//0 -- ��ʼ	1 -- ����
	BYTE ButtonCoverStatus[2];
	TRealTimer ButtonCoverOpenTime;
	TRealTimer ButtonCoverCloseTime;
	DWORD CheckButtonCoverStatus;
}TLowPowerButtonCoverData;

#endif

//-----------------------------------------------
// 				��������
//-----------------------------------------------
#if( SEL_EVENT_METERCOVER == YES )
// �жϿ�����Ƿ���
BYTE GetMeterCoverStatus(BYTE Phase);
void MeterCoverPowerOn( void );
void MeterCoverPowerOff( void );
void ProcLowPowerMeterCoverRecord();
#endif

#if( SEL_EVENT_BUTTONCOVER == YES )
// �жϿ���ť���Ƿ���
BYTE GetButtonCoverStatus(BYTE Phase);
void ButtonCoverPowerOn( void );
void ButtonCoverPowerOff( void );
void ProcLowPowerButtonCoverRecord();
#endif

void FactoryInitCoverPara( void );
#endif
