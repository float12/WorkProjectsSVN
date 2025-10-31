//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.9
//����		ͳһ�������еİ����ʵ�ֹ����ļ��ķּ����������Ա������Ӱ�ʱ�Ķ�����
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __BOARD_API_H
#define __BOARD_API_H


//#include ".\SingleMeter\board_ht_698_Single.H"
//#include ".\ThreeMeter\board_ht_698_Three.H"
//#include ".\ThreeMeter\board_st_698_Three.H"
#include ".\SeparateType\board_SeparateType.H"
//-----------------------------------------------
//				�궨��
//-----------------------------------------------	
#define IR_WAKEUP_LIMIT_TIME		604800	//7������

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eFromOnInitEnterDownMode,	//���ϵ��ʼ������͹���
	eFromOnRunEnterDownMode,	//�ϵ����к����͹���
	eFromWakeupEnterDownMode,	//�͹��Ļ��Ѻ��ٽ���͹���
	eFromDetectEnterDownMode,	//�͹��Ľ��е�Դ�쳣��ȫʧѹ�����ٽ���͹���
}eENTER_DOWN_MODE;

typedef enum
{
	eOnInitDetectPowerMode,		//�ϵ�ϵͳ��Դ���
	eOnRunDetectPowerMode,		//���������ڼ��ϵͳ��Դ���
	eWakeupDetectPowerMode,		//�͹��Ļ����ڼ�ϵͳ��Դ���
	eSleepDetectPowerMode,		//�͹��������ڼ�ϵͳ��Դ���
}eDETECT_POWER_MODE;

typedef struct TLowPowerConRom_t
{
	BYTE DetectFlag;
}TLowPowerConRom;

//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ϵͳ��Դ���
//
//����: 	
//          Type[in]:	
//				eOnInitDetectPowerMode		�ϵ�ϵͳ��Դ���
//				eOnRunDetectPowerMode		���������ڼ��ϵͳ��Դ���
//				eWakeupDetectPowerMode		�͹��Ļ����ڼ�ϵͳ��Դ���
//				eSleepDetectPowerMode		�͹��������ڼ�ϵͳ��Դ���
//����ֵ: 	TRUE:�е�   FALSE:û�е�
//
//��ע:   �ϵ�vcc�ͱȽ������У�����ֻ��LVDIN0�Ƚ�������Ϊvcc���г�������
//-----------------------------------------------
BOOL api_CheckSysVol( eDETECT_POWER_MODE Type );

//-----------------------------------------------
//��������: ����͹��ļ���Ӧģ�鴦��
//
//����: 	Type[in]
//				eFromOnInitEnterDownMode	���ϵ��ʼ������͹���
//				eFromOnRunEnterDownMode		�ϵ����к����͹���
//				eFromWakeupEnterDownMode	�͹��Ļ��Ѻ��ٽ���͹���
//				eFromDetectEnterDownMode	�͹��Ľ��е�Դ�쳣��ȫʧѹ�����ٽ���͹���
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_EnterLowPower(eENTER_DOWN_MODE Type);

//-----------------------------------------------
//��������: �ڻ���״̬�µĵ͹������񣬶�ʱ����͹���״̬
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_LowPowerTask(void);

//-----------------------------------------------
//��������: ��ȡ�����Ƿ���ɱ�־
//
//����: 	��

//����ֵ:  	������ɵ��緵��TRUE,δ��ɵ��緵��FALSE
//
//��ע:   
//-----------------------------------------------
BOOL api_GetPowerDownCompleteFlag( void );


#endif//#ifndef __BOARD_API_H


