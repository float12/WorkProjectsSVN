#ifndef __COS_OVER
#define __COS_OVER

#if( SEL_EVENT_COS_OVER == YES )

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TCosoverData_t
{
#if(MAX_PHASE == 1)
	// ��һ�㣺�����й��ܵ���
	// �ڶ��㣺��ʼ������ʱ��
	DWORD	Energy[2][2];
	BYTE	Reserved[10];		// Ԥ�� 
#else
	// ��һ�㣺�����й�������޹�1,2
	// �ڶ��㣺��ʼ������ʱ��
	DWORD	Energy[4][2];
	BYTE	Reserved[10];		// Ԥ��	
#endif
}TCosoverData;

// �������������¼�
typedef struct TCosoverRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TCosoverData)];
}TCosoverRom;

//-----------------------------------------------
// 				��������
//-----------------------------------------------
BYTE GetCosoverStatus(BYTE Phase);
 void FactoryInitCosOverPara( void );
#endif

#endif
