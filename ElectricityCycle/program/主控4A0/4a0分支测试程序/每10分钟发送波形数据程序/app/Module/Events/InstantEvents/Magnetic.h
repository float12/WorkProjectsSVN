#ifndef __MAGNETIC
#define __MAGNETIC

#if( SEL_EVENT_MAGNETIC_INT == YES )

// �㶨�ų����ż�¼�¼��ĸ�������
typedef struct TMagneticData_t
{
	// ��һ�㣺����ǰ�����Ǻ�
	// �ڶ��㣺�����й�
	DWORD Energy[2][2];
	BYTE	Reserved[10];		// Ԥ��
}TMagneticData;

// ǿ���¼�
typedef struct TMagneticRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TMagneticData)];
}TMagneticRom;

BYTE GetMagneticStatus(BYTE Phase);
void FactoryInitMagneticPara( void );
#endif

#endif
