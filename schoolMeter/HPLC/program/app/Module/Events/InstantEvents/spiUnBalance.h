#ifndef __SP_I_UNBALANCE
#define __SP_I_UNBALANCE

// ����������ƽ���¼ 
#if( SEL_EVENT_SP_I_UNBALANCE == YES )
typedef struct TSPIUnbalanceData_t
{
	// ����ʱ�̵���1��2ֵ 
	WORD I[2];
	
}TSPIUnbalanceData;
#endif
#if(SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void FactoryInitNeutralCurrentErrPara( void );
//-----------------------------------------------
//��������: �ж��Ƿ����ߵ�����ƽ��
//
//����:  ��
//                    
//����ֵ:	����״̬		FALSE������
//					TRUE�����ߵ�����ƽ��״̬
//
//��ע: 
//-----------------------------------------------
BYTE GetNeutralCurrentErrStatus(BYTE Phase);
#endif

#endif
