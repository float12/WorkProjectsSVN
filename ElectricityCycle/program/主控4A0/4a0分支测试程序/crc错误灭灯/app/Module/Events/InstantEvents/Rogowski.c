//---------------------------------------------
// ������Ȧ������ն˳���ʧ�ܼ�¼
//---------------------------------------------

#include "AllHead.h"
#include "Rogowski.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE bRogowskiChangeFlag=0,bRogowskiFailFlag=0;
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )

// ��������Ȧ��� 0x3053  �Ĺ����������Ա� oad������������
static const DWORD RogowskiChangeDefaultOadConst[] =
{
	2,//��������ĸ���
	0x40332200,	// ���ǰ��������Ȧʶ����
	0x40338200,	// ������������Ȧʶ����
};
#endif

//�ն˳���ʧ�� 0x310F
#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
static const DWORD RogowskiFailDefaultOadConst[] =
{
	0//��������ĸ���	
};
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void FactoryInitRogowskiPara( void )
{
	#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )
	api_WriteEventAttribute( 0x3053, 0xff, (BYTE *)&RogowskiChangeDefaultOadConst[0], sizeof(RogowskiChangeDefaultOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
	api_WriteEventAttribute( 0x310f, 0xff, (BYTE *)&RogowskiFailDefaultOadConst[0], sizeof(RogowskiFailDefaultOadConst)/sizeof(DWORD) );
	#endif
}
BYTE api_GetRogowskiChangEventStatus( BYTE Phase )
{
	return bRogowskiChangeFlag;
}
BYTE api_GetRogowskiFailEventStatus( BYTE Phase )
{
	return bRogowskiFailFlag;
}
