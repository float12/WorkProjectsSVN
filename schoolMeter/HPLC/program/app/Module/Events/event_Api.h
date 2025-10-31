//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.10.8
//����		�¼�ģ��Apiͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __EVENT_API
#define __EVENT_API
//--------------------------------------
#include "VICorrelate.h"
#include "ReversalUnBalance.h"
#include "BackpropOverload.h"
#include "LostPower.h"
#include "LostAllV.h"
#include "Cover.h"
#include "cosover.h"
#include "spiUnBalance.h"
#include "Magnetic.h"
#include "RelayErrEvent.h"
#include "PowerErr.h"
#include "statistics_voltage.h"
#include "PrgRecord.h"
#include "DemandOver.h"
#include "DeviceErr.h"

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

#if( MAX_PHASE == 1)
#define MAX_EVENT_OAD_NUM			(8+3)	// ���֧��N��OAD
#else
#define MAX_EVENT_OAD_NUM			(38+12)	// ���֧��n��OAD
#endif

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
   	eNormalFlag = 0x12341234,//����ǰΪ������־���������ࣩ     
   	eLostALLVFlag = 0x5AA55AA5,//����ǰΪȫʧѹ״̬�����ࣩ
    eLostPowerFlag = 0X56785678,//����ǰΪ����״̬�����ࣩ
    eOtherFlag = 0XAA55AA55,//����״̬��ֹ�쳣��λ�����ࡢ���ࣩ
}eLostALLV_LostPowerFlag;//���ݼ�TAG��־

typedef struct TEventAttInfo_t
{
	DWORD	Oad[MAX_EVENT_OAD_NUM];			//OAD��Ŀ  4�ֽڶ���
	BYTE	NumofOad;						//OAD�ĸ���
	BOOL	bValidFlag;						//��Ч��ʶ class7(24) ����9(12)�������ã����жϣ�����̶����տ�������
	WORD	OadValidDataLen;				//��Ч��OAD�����ܳ���
	WORD	AllDataLen;						//sizeof(TEventOADCommonData)+OadValidDataLen+ExtDataLen
	BYTE	OadLen[MAX_EVENT_OAD_NUM];		//OAD��Ŀ��Ӧ�����ݳ���	
	WORD	MaxRecordNo;					//����¼��
}TEventAttInfo;

//�¼�������������flash������eeprom�����򱣴浽flash
typedef struct TInstantEventRecord_t
{	
	//����flash��Ҫ������eeprom�е�����
	#if( SEL_EVENT_V_UNBALANCE == YES )
	WORD				RomVUnBalanceRate;		//ROM���ݴ�����ƽ����
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	WORD				RomIUnBalanceRate;		//ROM���ݴ�����ƽ����
	#endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	WORD				RomISUnBalanceRate;		//ROM���ݴ�����ƽ����
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	TDemandData			PADemandOverMaxValue;
	TDemandData			NADemandOverMaxValue;
	TDemandData			QDemandOverMaxValue[EVENT_PHASE_FOUR];
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )
	TLowPowerMeterCoverData LowPowerMeterCoverData;
	#endif

	#if( SEL_EVENT_BUTTONCOVER == YES )
	TLowPowerButtonCoverData LowPowerButtonCoverData;
	#endif
	
	#if( MAX_PHASE != 1 )
	DWORD AHour[4][3][4];//���氲ʱֵ,��1�±�[4]--ʧѹ��Ƿѹ����ѹ������;��2�±�[3]--A��B��C�෢���¼�;��3�±�[4]--�����¼�ʱ�ܡ�A��B��C�లʱֵ
	#endif
	BYTE Temp;//��ֹ�����˽ṹ��Ϊ��
}TInstantEventRecord;

// �¼���������ֵ
typedef struct TEventPara_t
{
	DWORD	dwLimit[eNUM_OF_EVENT_ALL];		//����
	WORD	wLimit[eNUM_OF_EVENT_ALL];		//��ѹ
	BYTE	byLimit[eNUM_OF_EVENT_ALL];		//ʱ��
	WORD	LostVRecoverLimitV;	// ʧѹ�¼���ѹ�ָ�����eEVENT_PARA_TYPE_SPECIAL1
	WORD	VRunLimitLV;	// ��ѹ��������   eTYPE_SPEC1
	WORD	VRunRecoverHV;	// ��ѹ�ϸ�����   eTYPE_SPEC2
	WORD	VRunRecoverLV;	// ��ѹ�ϸ�����   eTYPE_SPEC3
	DWORD	LostILimitLI;	// ʧ���¼������ָ�����eEVENT_PARA_TYPE_SPECIAL1
}TEventPara;

	
typedef struct TPowerDownFlag_t
{
	BYTE	ProgramPwrDwn;										// ��̼�¼�����־
	BYTE	InstantEventFlag[(SUB_EVENT_INDEX_MAX_NUM+7)/8];	// �������¼������־
}TPowerDownFlag;

typedef struct TEventSafeRom_t
{
	TEventDataInfo			EventDataInfo[SUB_EVENT_INDEX_MAX_NUM];
	//645��Լ�ı�̼�¼���Լ�����������Դ����������һ����¼��
	#if(SEL_PRG_RECORD645 == YES)
	TEventDataInfo			ProRecord645;
	#endif
	TLostPowerTime			LostPowerTime;
}TEventSafeRom;

typedef struct TEvent_t
{
	TEventPara				EventPara;							// �¼����޲���
	TEventAttInfo			EventAttInfo[eNUM_OF_EVENT_PRG];	// �����¼��������࣬����࣬�����ࣩ��OAD��Ϣ,����ͳһ
	TProgramRecordRom		ProgramRecordRom;					// �������¼+��̼�¼
	TInstantEventRecord		InstantEventRecord;					// �������¼���¼
	TPowerDownFlag			api_PowerDownFlag;						// ���籣����¼��ı�־
	BYTE					AutoWarnAtt[eNUM_OF_EVENT_PRG];		// �����ϱ���ʶ		
	#if( SEL_STAT_V_RUN == YES )
	TStatVRunDataRom 		StatVRunDataRom;					// ��ѹ�ϸ���ͳ��
	#endif
}TEventConRom;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern TEventPara	gEventPara;	// �¼����޼���ʱ����

//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: �ж��¼�״̬
//
//����: ��
//                    
//����ֵ: ��
//			
//��ע: ��ѭ������
//-----------------------------------------------
void api_EventTask(void);

//-----------------------------------------------
//��������: ���¼�������������
//����: 
//		OI[in]:	�¼�OI                
//  	No[in]:	�ڼ���OAD��0����ȫ��OAD                
//  	Len[in]:��Լ�㴫�����ݳ��ȣ������˳��ȷ���FALSE	
//  	pBuf[out]:�������ݵ�ָ�� ���ն������ڣ�OAD���洢���˳�򷵻أ�ÿ��DWORD���ܹ�3*4�ֽ�
//
//����ֵ:	
//		ֵΪ0x8000 ��������ִ��� OI��֧�֣���ȡEpprom��������⣩
//		ֵΪ0x0000 ������buf���Ȳ���
//		����:		�������ݳ���( �ڸ���Ϊ0ʱ�����1�ֽڵ�0 ���س���Ϊ1)
//��ע: 
//-----------------------------------------------
WORD api_ReadEventAttribute( WORD OI, BYTE No, WORD Len, BYTE *pBuf );

//-----------------------------------------------
//��������: ����¼�������������
//
//����: 
//	OI[in]: �¼�OI
//  
//	pOad[in]:	������������ʱ��OAD
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:   ���һ��OAD�����е����¼ƣ����·���EEP�ռ䣨Ĭ�ϲ��������ͬ��OAD��
//-----------------------------------------------
BOOL api_AddEventAttribute( WORD OI, BYTE *pOad );

//-----------------------------------------------
//��������: ɾ���¼�������������
//
//����: 
//	OI[in]:	�¼�OI
//  
//	pOad[in]:	Ҫɾ����OAD,���Ա�ʶ��������bit5~bit7������ඳ��Ķ��᷽��
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:   ɾ��һ��OAD�����е����¼ƣ����·���EEP�ռ�
//-----------------------------------------------
BOOL api_DeleteEventAttribute( WORD OI, BYTE *pOad );

//-----------------------------------------------
//��������: �����¼�������������
//
//����: 
//	OI[in]:	�¼�OI
//  
//  OadType[in]:
//			0 ~ �������е�OAD
//			N ~ �����б���ĵ�N��OAD  ���õ�N��OADʱ����Ϊ��N��OAD֮ǰ����ֵ��
//	
//  pOad[in]:  ָ��OAD Buffer��ָ��  2�ֽڶ�������+4�ֽ�OAD+2�ֽڴ洢���
//                
//  OadNum[in]:	OAD�ĸ���
//
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:	���е����¼ƣ����·���EEP�ռ�
// 		
//-----------------------------------------------
BOOL api_WriteEventAttribute( WORD OI, BYTE OadType, BYTE *pOad, WORD OadNum );

//-----------------------------------------------
//��������: ��ȡ����ʱ��
//
//����: 
//	PowerDownTime[out]:	����ʱ�� 
//  
//����ֵ:				��  
//��ע:
//-----------------------------------------------
void api_GetPowerDownTime( TRealTimer *pPowerDownTime );

//-----------------------------------------------
//��������: ���������ÿ���¼��ı�̼�¼������ܣ�ʱ��ʱ�α��̵ȣ�
//
//����:
//	inEventIndex[in]:	�������ö�ٺ�
//
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:
//-----------------------------------------------
BOOL api_SavePrgOperationRecord( eEVENT_INDEX inEventIndex );

//-----------------------------------------------
//��������: �������¼���¼
//
//����: 
//	Type[in]:	EVENT_START:	��̿�ʼ����������ʱ�ÿ�ʼ
//				EVENT_END:		��̽������Ͽ�Ӧ�����ӻ��ߵ���ʱ�ý���
//  DIType[in]:	0x51--OAD	0X53--OMD
//	pOad[in]:	ָ��OAD��ָ��
// 
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע: ֻ���һ��OAD��ʱ��Ϊ��ʼʱ��
//		����������OADֻ����OAD�����ٴ�OAD��Ӧ����
//		���������¼���¼ʱ��pOad��Ч
//-----------------------------------------------
BOOL api_SaveProgramRecord( BYTE Type, BYTE DIType, BYTE *pOad );

//-----------------------------------------------
//��������: ���¼���¼
//
//����: 
//	Tag[in]:	0 ����Tag 
//				1 ��Tag
//                
//	pDLT698RecordPara[in]: ��ȡ����Ĳ����ṹ��
//                
//  Len[in]�������buf���� �����ж�buf�����Ƿ���
//                
//  pBuf[out]: �������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0
//				
//	
//  ����ֵ:	bit15λ��1 �������޴����� ��0������������ȷ���أ� 
//			bit0~bit14 �����صĶ������ݳ���
//
//��ע: ֻ֧����Last����¼
//-----------------------------------------------
WORD api_ReadEventRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );

//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_FactoryInitEvent( void );

//-----------------------------------------------
//��������: ���¼���¼����
//
//����: 
//  Type[in]:	eEVENT_CLEAR_TYPE
//	
//����ֵ:	��
//  
// ��ע:
//-----------------------------------------------
BOOL api_ClrAllEvent( BYTE Type );

//-----------------------------------------------
//��������: ������¼���¼����
//
//����: 
//  OI[in]:	OI
//	
//����ֵ:  	TRUE   ��ȷ
//			FALSE  ����
//  
// ��ע:
//-----------------------------------------------
BOOL api_ClrSepEvent( WORD OI );

//-----------------------------------------------
//��������: д�¼�����
//
//����: 
//			OI[in]:					OI
//			No[in]:					0: ȫ������
//									N: ��N������
//			pBuf[in]:				д���ݵĻ���
//                    
//����ֵ:  	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:  	дEEP+RAM  698��Լ����
//-----------------------------------------------
BOOL api_WriteEventPara( WORD OI, BYTE No, BYTE *pBuf );

//-----------------------------------------------
//��������: ���¼�����
//
//����: 
//			OI[in]:					OI
//			No[in]:					0: ȫ������
//									N: ��N������
//			pLen[out]:				�������ݵĳ���
//			pBuf[out]:				�����ݵĻ���
//                    
//����ֵ:  	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:  	��ȡram  698��Լ����
//-----------------------------------------------
BOOL api_ReadEventPara( WORD OI, BYTE No, BYTE *pLen, BYTE *pBuf );

//-----------------------------------------------
//��������: �����¼�����
//
//����:  ��
//                    
//����ֵ: ��
//
//��ע: 
//-----------------------------------------------
void api_PowerDownEvent(void);

//-----------------------------------------------
//��������: �ݴ��̼�¼֮ǰ�������OAD������
//
//����:  	Type[in]: 0--OAD	1--OMD
//			dwData[in]: Ҫ���õĲ���OAD ���ֽ���ǰ�����ֽ��ں�
//                  
//����ֵ: TRUE/FALSE
//
//��ע:��Լ���ñ�̼�¼ʱ�������ò���֮ǰҪ�ȵ��ô˺����� 
//-----------------------------------------------
BOOL api_WritePreProgramData( BYTE Type,DWORD dwData );


//-----------------------------------------------
//��������: ��ȡ��ǰ��¼��������¼��
//
//����:  
//  OI[in]: OI
//   
//  Phase[in]: ��λ0��,1A,2B,3C
//             
//  Type[in]: �ο�eEVENT_RECORD_NO_TYPE
//                
//  pBuf[out]���������ݵĻ���
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע: 
//-----------------------------------------------
BOOL api_ReadEventRecordNo( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf );

//-----------------------------------------------
//��������: ��ȡ��ǰֵ��¼��(�޹���Ϸ�ʽ1�����֡��޹���Ϸ�ʽ2�����ֱ�̴���)
//
//����:  
//  byType[in]: 	0--�޹���Ϸ�ʽ1������, 1--�޹���Ϸ�ʽ2������
//  pBuf[out]��	�������ݵĻ���
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע: �¼��������7  �¼�����Դ+��������+�ۼ�ʱ��
//-----------------------------------------------
BOOL api_ReadCurrentRecordTable_Q_Combo( BYTE byType, BYTE *pBuf );

//-----------------------------------------------
//��������: ��ȡ��ǰֵ��¼��
//
//����:  
//  OI[in]: 	OI      
//  Phase[in]: 	��λ0��,1A,2B,3C 
//	Type[in]:	0--�����1�η���ʱ�䡢����ʱ��(Class 7�з���Դ)  ����--����������+�ۼ�ʱ��(Class 7�з���Դ)
//  pBuf[out]��	�������ݵĻ���
//                    
//����ֵ:	0--��ȡʧ�ܣ�����--���ض�ȡ����
//
//��ע: ���¼�������1�η���ʱ�䡢����ʱ��(Class 7�з���Դ)�������������+�ۼ�ʱ��(Class 7�з���Դ)
//-----------------------------------------------
WORD api_ReadCurrentRecordTable( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf );
#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//��������: ��ȡ645��ǰֵ��¼��
//
//����:  
//  OI[in]: 	OI      
//  Phase[in]: 	��λ0��,1A,2B,3C 
//	Type[in]:	0--�����1�η���ʱ�䡢����ʱ��
//              1--����������
//              2--����������+�ۼ�ʱ��
//  pBuf[out]��	�������ݵĻ���
//                    
//����ֵ:	0x8000--��ȡʧ�ܣ�����--���ض�ȡ����
//
//��ע: 
//-----------------------------------------------
WORD api_ReadCurrentRecordTable645( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf );
//-----------------------------------------------
//��������: ���¼���¼
//
//����: 
//		Tag[in]:				0 ����Tag 1 ��Tag                             
//  	Len[in]��				�����buf���� �����ж�buf�����Ƿ���                
//  	pBuf[out]: 				�������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0	
//����ֵ:			bit15λ��1 �������޴����� ��0������������ȷ���أ� 
//					bit0~bit14 �����صĶ������ݳ���
//��ע: ֻ֧����Last����¼
//-----------------------------------------------
WORD api_ReadEvent645RecordByNo( TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );
#endif
//-----------------------------------------------
//��������: �����¼�״̬
//
//����: 
//			Type[in]��			eDEAL_EVENT_STATUS
//                    
//			inSubEventIndex[in]	eSUB_EVENT_INDEX
//                    
//����ֵ: TypeΪeGET_EVENT_STATUSʱ��1Ϊ�¼�������δ������0Ϊ�¼�δ���� 	
//
//��ע:   
//-----------------------------------------------
BYTE api_DealEventStatus(BYTE Type, BYTE inSubEventIndex );

#if ( SEL_EVENT_LOST_V == YES )
//-----------------------------------------------
//��������: ��ȡʧѹ����13
//
//����:  
//  Index[in]:	0 ���� 
// 				1 �¼������ܴ��� 
// 				2 �¼����ۼ�ʱ��    
// 				3 ���һ��ʧѹ����ʱ��
// 				4 ���һ��ʧѹ����ʱ��
//   
//  pBuf[out]���������ݵĻ���
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:
//-----------------------------------------------
BOOL api_ReadEventLostVAtt( BYTE Index, BYTE *pBuf );
#endif
// �ϵ��¼�����
void api_PowerUpEvent(void);


#if( SEL_EVENT_LOST_ALL_V == YES )
//-----------------------------------------------
//��������:ͬ������ֵ
//
//����: 
//	Type[in]:	0��1��2��3����A\B\C\N���� 
//	Value[in]:	����ֵ 4λС�� 
 
//����ֵ:				��
//��ע:ÿ�β���ʱͬ����ֵ�����ڴ���ȫʧѹ��
//-----------------------------------------------
void api_SetLostAllVCurrent( BYTE Type, long Value );
#endif

#if(SEL_PRG_RECORD645 == YES)
//-----------------------------------------------
//��������: ����645��Լ����¼���¼
//
//����:
//	Type[in]:	EVENT_START:	��̿�ʼ����������ʱ�ÿ�ʼ
//				EVENT_END:		��̽������Ͽ�Ӧ�����ӻ��ߵ���ʱ�ý���
//	Op[in]:		�����ߴ��� ����
//	pDI[in]:	ָ��DI��ָ�� ����
//
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:��������ʱTypeΪEVENT_START������֤�����ĵ���TypeΪEVENT_END
//-----------------------------------------------
BOOL api_SaveProgramRecord645( BYTE Type, BYTE *Op, BYTE *pDI );

//-----------------------------------------------
//��������: 645��Լ����¼���¼��03300000~0330000a��
//
//����:
//	DI[in]:		��ȡ���ݵ����ݱ�ʶ
//				EVENT_END:		��̽������Ͽ�Ӧ�����ӻ��ߵ���ʱ�ý���
//	pOutBuf[out]:	���ص�����
//
//  ����ֵ:	���ݳ��� 8000�������
//
//��ע:֧�ֱ�̴��������10�α�̼�¼�Ķ�ȡ
//-----------------------------------------------
WORD api_ReadProgramRecord645( BYTE *DI, BYTE *pOutBuf );
#endif//#if(SEL_PRG_RECORD645 == YES)

//-----------------------------------------------
//��������  : ����͹���ǰ���п������ݼ��
//����:
//         
//
//����ֵ:     
//��ע����:ֻ���ڽ���͹���ǰ���� �͹����²��ܵ���
//-----------------------------------------------
void api_LowPowerCheckMeterCoverStatus( void );
//-----------------------------------------------
//��������  : ����͹���ǰ���п������ݼ��
//����:
//         
//
//����ֵ:     
//��ע����:ֻ���ڽ���͹���ǰ���� �͹����²��ܵ���
//-----------------------------------------------
void api_LowPowerCheckButtonCoverStatus( void );


#endif//��Ӧ�ļ���ǰ���#define __EVENT_API
