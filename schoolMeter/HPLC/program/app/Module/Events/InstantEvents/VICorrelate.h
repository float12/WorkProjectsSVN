//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.10.27
//����		���ѹ��������ص��¼���¼
//�޸ļ�¼	
//---------------------------------------------------------------------- 

#ifndef __VI_CORRELATE
#define __VI_CORRELATE

#include "event.h"
//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
// �����¼����ݽṹ��
typedef struct TVICorrelateData_t
{
	// ����Ŀռ����ڶ������⣬Ӧ�ñ�ʵ��ռ�õĿռ�Ҫ��
	#if( MAX_PHASE == 1)
	DWORD	Energy[4]; 			// ����������ʱ�������й��ܵ���
	WORD	Voltage;			// ����ʱ�̵�ѹ
	DWORD	Current[2];			// ����ʱ�̵���
	DWORD	ActPower[2];		// ����ʱ���й�����
	WORD	Cos[2];				// ����ʱ�̹�������
	BYTE	Reserved[12];		// Ԥ��
	#else
	DWORD	AllEnergy[4*2]; 	// ����,����ʱ�������й��ܵ��ܣ�����޹�1,2�ܵ���
	DWORD	SepEnergy[4*3*2]; 	// ����,����ʱ��A,B,C �����й����ܣ�����޹�1,2����
	WORD	Voltage[3];			// ����ʱ�̵�ѹ
	DWORD	Current[4];			// ����ʱ�̵���
	DWORD	ActPower[4];		// ����ʱ���й�����
	DWORD	ReactPower[4];		// ����ʱ���޹�����
	WORD	Cos[4];				// ����ʱ�̹�������
	DWORD	AHour[4];			// ����ʱ�̰�ʱֵ
	BYTE	Reserved[10];		// Ԥ��
	#endif
}TVICorrelateData;

#if( SEL_EVENT_LOST_V == YES )
// ʧѹ�¼�
typedef struct TLostVRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TVICorrelateData)];
}TLostVRom;
#endif//#if( SEL_EVENT_LOST_V == YES )

#if( SEL_EVENT_WEAK_V == YES )
// Ƿѹ�¼�
typedef struct TWeakVRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TVICorrelateData)];
}TWeakVRom;
#endif//#if( SEL_EVENT_OVER_I == YES )

#if( SEL_EVENT_OVER_V == YES )
// ��ѹ�¼�
typedef struct TOverVRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TVICorrelateData)];
}TOverVRom;
#endif//#if( SEL_EVENT_OVER_V == YES )

#if( SEL_EVENT_BREAK == YES )
// �����¼�
typedef struct TBreakRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TVICorrelateData)];
}TBreakRom;
#endif//#if( SEL_EVENT_BREAK == YES )

#if( SEL_EVENT_LOST_I == YES )
// ʧ���¼�
typedef struct TLostIRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TVICorrelateData)];
}TLostIRom;
#endif//#if( SEL_EVENT_LOST_I == YES )

#if( SEL_EVENT_BREAK_I == YES )
// �����¼�
typedef struct TBreakIRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TVICorrelateData)];
}TBreakIRom;
#endif//#if( SEL_EVENT_BREAK_I == YES )

#if( SEL_EVENT_OVER_I == YES )
// �����¼�
typedef struct TOverIRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TVICorrelateData)];
}TOverIRom;
#endif//#if( SEL_EVENT_OVER_I == YES )

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------
void FactoryInitVICorrelatePara( void );

#if( SEL_EVENT_LOST_V == YES )
// ʧѹ��¼��״̬���
BYTE GetLostVStatus(BYTE Phase);
#endif//#if( SEL_EVENT_LOST_V == YES )

#if( SEL_EVENT_WEAK_V == YES )
// Ƿѹ��¼��״̬���
BYTE GetWeakVStatus(BYTE Phase);
#endif//#if( SEL_EVENT_WEAK_V == YES )

#if( SEL_EVENT_OVER_V == YES )
// ��ѹ��¼��״̬���
BYTE GetOverVStatus(BYTE Phase);
#endif//#if( SEL_EVENT_OVER_V == YES )

#if( SEL_EVENT_BREAK == YES )
// �����¼��״̬���
BYTE GetBreakStatus(BYTE Phase);
#endif//#if( SEL_EVENT_BREAK == YES )

#if( SEL_EVENT_LOST_I == YES )
// ʧ����¼��״̬���
BYTE GetLostIStatus(BYTE Phase);
#endif//#if( SEL_EVENT_LOST_I == YES )

#if( SEL_EVENT_BREAK_I == YES )
// ������¼��״̬���
BYTE GetBreakIStatus(BYTE Phase);
#endif//#if( SEL_EVENT_BREAK_I == YES )

#if( SEL_EVENT_OVER_I == YES )
// ������¼��״̬���
BYTE GetOverIStatus(BYTE Phase);
#endif//#if( SEL_EVENT_OVER_I == YES )

#endif
