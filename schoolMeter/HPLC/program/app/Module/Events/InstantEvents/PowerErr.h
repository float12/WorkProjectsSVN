//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.26
//����		��Դ�쳣�¼������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __POWERERR_H
#define __POWERERR_H

#include "event.h"

#if( SEL_EVENT_POWER_ERR == YES )

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define START_TEST_POWER_ERR		3

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TPrgInfoPowerErrData_t
{
	DWORD Energy[2]; 	// �����й�
	BYTE	Reserved[10];		// Ԥ��
}TPrgInfoPowerErrData;

// ��Դ�쳣
typedef struct TPowerErrRom_t
{
	TEventOADCommonData		EventOADCommonData;								// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TPrgInfoPowerErrData)];
}TPowerErrRom;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern __no_init DWORD wPowerErrFlag;//ģ���ڱ���

//-----------------------------------------------
// 				��������
//-----------------------------------------------
void PowerErrCheck( BYTE Type );
BYTE GetPowerErrStatus(BYTE Phase);
void FactoryInitPowerErrPara( void );
void PowerUpDealPowerErr( void );

#endif//#if( SEL_EVENT_POWER_ERR == YES )

#endif//#ifndef __POWERERR_H
