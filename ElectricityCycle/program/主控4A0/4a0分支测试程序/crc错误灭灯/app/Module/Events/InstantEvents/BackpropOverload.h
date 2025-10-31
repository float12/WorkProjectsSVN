#ifndef __BACKPROP_OVERLOAD
#define __BACKPROP_OVERLOAD

#if((SEL_EVENT_BACKPROP==YES) || (SEL_EVENT_OVERLOAD==YES))

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//  ���๦�ʷ��򡢹����¼��ĸ�������
typedef struct TBackpropOverloadData_t
{
#if(MAX_PHASE == 1)
	// �ܵ���
	// ��һ�㣺�����й�
	// �ڶ��㣺��ʼ������ʱ��
	DWORD Energy[2][2];
	BYTE  Reserved[10];		// Ԥ��	
#else
	// �ܵ���
	// ��һ�㣺�����й�������޹�1������޹�2
	// �ڶ��㣺��ʼ������ʱ��
	DWORD Energy[4][2];
	
	// A�����
	// ��һ�㣺�����й�������޹�1������޹�2
	// �ڶ��㣺��ʼ������ʱ��
	DWORD EnergyA[4][2];
	
	// B�����
	// ��һ�㣺�����й�������޹�1������޹�2
	// �ڶ��㣺��ʼ������ʱ��
	DWORD EnergyB[4][2];
	
	// C�����
	// ��һ�㣺�����й�������޹�1������޹�2
	// �ڶ��㣺��ʼ������ʱ��
	DWORD EnergyC[4][2];

	BYTE	Reserved[10];		// Ԥ��	
#endif	
}TBackpropOverloadData;

#endif

#if( SEL_EVENT_OVERLOAD == YES )
// �����¼�
typedef struct TOverloadRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TBackpropOverloadData)];
}TOverloadRom;
#endif

#if( SEL_EVENT_BACKPROP == YES )
// ���ʷ����¼�
typedef struct TBackpropRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TBackpropOverloadData)];
}TBackpropRom;
#endif

//-----------------------------------------------
// 				��������
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
