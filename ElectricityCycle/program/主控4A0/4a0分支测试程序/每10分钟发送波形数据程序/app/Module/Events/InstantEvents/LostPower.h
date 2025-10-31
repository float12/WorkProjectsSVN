#ifndef __LOST_POWER
#define __LOST_POWER

#include "event.h"

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

// �����¼�
typedef struct TLostPowerRom_t
{
	TEventOADCommonData		EventOADCommonData;							// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TEventNormal)];
}TLostPowerRom;


// ������Դ�����¼�
typedef struct TLostSecPowerRom_t
{
	TEventDataInfo			EventDataInfo;
	TEventOADCommonData		EventOADCommonData[MAX_EVENTRECORD_NUMBER];							// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TEventNormal)*MAX_EVENTRECORD_NUMBER];
}TLostSecPowerRom;

typedef struct TLostPowerPara_t
{
	BYTE	Delay;
}TLostPowerPara;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern __no_init DWORD PowerDownLostSecPowerFlag;

//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: �ж��Ƿ����
//
//����:  ��
//                    
//����ֵ:	����״̬ 0��δ����  1������
//
//��ע: 
//-----------------------------------------------
BYTE GetLostPowerStatus( BYTE Phase );

//-----------------------------------------------
//��������: �����¼����ϵ紦��
//
//����:  ��
//                    
//����ֵ:	��
//
//��ע: 
//-----------------------------------------------
void LostPowerPowerOn( void );

//-----------------------------------------------
//��������: ������������
//
//����:  ��
//                    
//����ֵ:	��
//
//��ע: 
//-----------------------------------------------
void ClearLostPowerData( void );

//-----------------------------------------------
//��������: ���õ����־
//
//����:  ��
//                    
//����ֵ:	��
//
//��ע: 
//-----------------------------------------------
void SetLostPowerStatus( void );

//-----------------------------------------------
//��������: ������־
//
//����:  ��
//                    
//����ֵ:	��
//
//��ע: 
//-----------------------------------------------
void ClearLostPowerStatus( void );
void FactoryInitLostPowerPara( void );
void LostPowerPowerDown( void );
void LostSecPowerPowerOff( void );
BYTE GetLostSecPowerStatus(BYTE Phase);
#endif//#ifndef __LOST_POWER
