#ifndef __RELAYERREVENT
#define __RELAYERREVENT

#if( SEL_EVENT_RELAY_ERR == YES )

typedef struct TRelayErrData_t
{
	// ��һ�㣺�����й��ܵ���
	// �ڶ��㣺���ɿ�������ǰ��
	DWORD Energy[2][2];
	BYTE	Reserved[10];		// Ԥ��
}TRelayErrData;

// ���ɿ��������¼�
typedef struct TRelayErrRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TRelayErrData)];
}TRelayErrRom;

void FactoryInitRelayErrPara( void );
BYTE GetRelayErrStatus(BYTE Phase);
#endif

#endif
