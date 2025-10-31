//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.11.14
//����		��Χ�����쳣ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __DEVICEERR_H
#define __DEVICEERR_H

#include "event.h"

#if( (SEL_EVENT_RTC_ERR == YES)||(SEL_EVENT_SAMPLECHIP_ERR == YES) )
//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
#if( SEL_EVENT_RTC_ERR == YES )
// ʱ�ӹ���
typedef struct TRtcErrRom_t
{
	TEventOADCommonData		EventOADCommonData;
	BYTE					EventData[sizeof(TEventNormal)];
}TRtcErrRom;
#endif

#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
// ����оƬ����
typedef struct TSampleChipErrRom_t
{
	TEventOADCommonData		EventOADCommonData;
	BYTE					EventData[sizeof(TEventNormal)];
}TSampleChipErrRom;
#endif
//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------
#if( SEL_EVENT_RTC_ERR == YES )
//-----------------------------------------------
//��������: ��ȡ��ǰʱ�ӹ���״̬
//
//����:  
//  Phase[in]: ��Ч          
//   
//����ֵ:	1   �й���
//			0	�޹���
//
//��ע: 
//-----------------------------------------------
BYTE GetRtcErrStatus(BYTE Phase);
//-----------------------------------------------
//��������: ʱ�ӹ����¼����ϵ紦��
//
//����:  ��
//                    
//����ֵ:	��
//
//��ע:   
//-----------------------------------------------
void RtcErrPowerOn( void );

//-----------------------------------------------
//��������: ��ʼ��ʱ�ӹ����¼�����
//
//����:  ��
//   
//����ֵ:	��
//
//��ע: 
//-----------------------------------------------
void api_FactoryInitRtcErrPara( void );
#endif//#if( SEL_EVENT_RTC_ERR == YES )

#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
BYTE GetSampleChipErrStatus(BYTE Phase);
void FactoryInitSampleChipErrPara( void );
#endif//#if( SEL_EVENT_SAMPLECHIP_ERR == YES )

#endif//#if( (SEL_EVENT_RTC_ERR == YES)||(SEL_EVENT_SAMPLECHIP_ERR == YES) )

#endif//#ifndef __DEVICEERR_H
