#ifndef __REVERSAL_UNBALANCE
#define __REVERSAL_UNBALANCE

#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )

// ��ƽ���¼��ĸ�������
typedef struct TUnBalanceData_t
{
#if(MAX_PHASE == 1)
	//��ѹ���ƽ����
	WORD MaxUnBalanceRate;
	// �ܵ���
	// ��һ�㣺�����й�
	// �ڶ��㣺��ʼ������ʱ��
	DWORD Energy[2][2];
	BYTE	Reserved[10];		// Ԥ�� 
#else
	// ���ƽ����
	WORD MaxUnBalanceRate;

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

}TUnBalanceData;

// ������ĸ�������
typedef struct TReversalData_t
{
#if(MAX_PHASE == 1)
	// �ܵ���
	// ��һ�㣺�����й�
	// �ڶ��㣺��ʼ������ʱ��
	DWORD Energy[2][2];
	BYTE	Reserved[10];		// Ԥ�� 
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

	BYTE	Reserved[20];		// Ԥ�� 

#endif

}TReversalData;

//-----------------------------------------------
//��������: ˢ�µ�ѹ��������ƽ���¼������ڼ�����ƽ����
//
//����: 			��
//                    
//����ֵ:  	��
//
//��ע:	�¼��������е���
//-----------------------------------------------
void FreshUnBalanceRate(void);
//-----------------------------------------------
//��������: ���ص�ѹ��������ƽ���¼������ڼ����ƽ����
//
//����: Type[in]	:	D7 -- 0 -- RAM�еĲ�ƽ���ʣ�1 -- EEPROM�еĲ�ƽ����
//								0 -- ��ѹ��ƽ����
//								1 -- ������ƽ����
//								2 -- �������ز�ƽ����
//                    
//����ֵ:  	��Ӧ�Ĳ�ƽ����
//
//��ע:
//-----------------------------------------------
WORD GetUnBalanceMaxRate(BYTE Type);
//-----------------------------------------------
//��������: ����洢��ѹ��������ƽ���¼������ڼ�����ƽ����
//
//����: 			��
//                    
//����ֵ:  	��
//
//��ע:	����ʱ����
//-----------------------------------------------
void UnBalanceRatePowerOff(void);

#endif

#if( SEL_EVENT_V_REVERSAL == YES )
// ��ѹ�������¼�
typedef struct TVReversRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TReversalData)];
}TVReversRom;
#endif//#if( SEL_EVENT_V_REVERSAL == YES )

#if( SEL_EVENT_I_REVERSAL == YES )
// �����������¼�
typedef struct TIReversRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TReversalData)];
}TIReversRom;
#endif//#if( SEL_EVENT_I_REVERSAL == YES )

#if( SEL_EVENT_V_UNBALANCE == YES )
// ��ѹ��ƽ���¼�
typedef struct TVUnbalanceRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TUnBalanceData)];
}TVUnbalanceRom;
#endif//#if( SEL_EVENT_V_UNBALANCE == YES )

#if( SEL_EVENT_I_UNBALANCE == YES )
// ������ƽ���¼�
typedef struct TIUnbalanceRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TUnBalanceData)];
}TIUnbalanceRom;
#endif//#if( SEL_EVENT_I_UNBALANCE == YES )

#if( SEL_EVENT_I_S_UNBALANCE == YES )
// �������ز�ƽ���¼�
typedef struct TISUnbalanceRom_t
{
	TEventOADCommonData		EventOADCommonData;					// ÿ������඼�еĹ�ͬ����
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
