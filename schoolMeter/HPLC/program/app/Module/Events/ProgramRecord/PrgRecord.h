//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.11.1
//����		�¼�ģ���ڲ�ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __PRGRECORD_H
#define __PRGRECORD_H

#include "event.h"

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define MAX_PRG_CACHE_LEN  100//��̻���������󳤶�

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

//645��̼�¼
#if(SEL_PRG_RECORD645 == YES)
typedef struct TPrgProgramRom645_t
{
	BYTE		Time[6];    //��̼�¼����ʱ��
	BYTE		Op[4];      //�����ߴ���
	BYTE		DI[40];		//���10�����ݱ�ʶ
}TPrgProgramRom645; 
#endif




typedef struct TPrgInfoProgramData_t
{
	#if( MAX_PHASE == 1)
	BYTE	Reserved[8];		// Ԥ��
	#else
	BYTE	Reserved[16];		// Ԥ��
	#endif
}TPrgInfoProgramData;

typedef struct TPrgProgramSubRom_t
{
	DWORD					Oad[MAX_PROGRAM_OAD_NUMBER];			// OAD array ���ΪMAX_PROGRAM_OAD_NUMBER��
	BYTE					Type[MAX_PROGRAM_OAD_NUMBER];			// OAD �� OMD ��ʶ
	BYTE					SaveOadNum;								// ÿ�α�̼�¼�����OAD�ĸ���
}TPrgProgramSubRom;

// ��̼�¼
typedef struct TPrgProgramRom_t
{
	TEventOADCommonData		FixData;								// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoProgramData)];		// ����OAD��Ӧ������
	TPrgProgramSubRom		OadList;
}TPrgProgramRom;


// ����������洢��¼
#if( SEL_PRG_INFO_CLEAR_METER == YES )
typedef struct TPrgInfoClearEnergyData_t
{
	#if( MAX_PHASE == 1)
	DWORD	Energy[2]; 			// �� �����й�
	BYTE	Reserved[4];		// Ԥ��
	#else
	// �������ǰ�ĵ�������
	// ��һ�㣺�ܡ�A��B��C
	// �ڶ��㣺�������й����������޹�
	DWORD	Energy[4][6];
	BYTE	Reserved[8];		// Ԥ��
	#endif
}TPrgInfoClearEnergyData;

typedef struct TPrgClearMeterRom_t
{
	TEventOADCommonData		FixData;	// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoClearEnergyData)];
}TPrgClearMeterRom;
#endif


// ��������¼�������ݽṹ
#if( SEL_PRG_INFO_CLEAR_MD == YES )
typedef struct TPrgInfoClearMDData_t
{
	// ������ǰ����
	// ��һ�㣺�������й����������޹�
	// �ڶ��㣺�����������ʱ��7+�������4
	BYTE	MDData[6][11];
	BYTE	Reserved[20];		// Ԥ��
}TPrgInfoClearMDData;

typedef struct TPrgClearMDRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoClearMDData)];
}TPrgClearMDRom;
#endif


// �¼������¼Ҫ��¼�¼��������ݱ�ʶ��(1��DWORD)
#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
typedef struct TPrgInfoClearEventData_t
{
	BYTE	Reserved[8];		// Ԥ��
}TPrgInfoClearEventData;

typedef struct TPrgClearEventRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����	
	BYTE					Data[sizeof(TPrgInfoClearEventData)];
	DWORD					Omd;	//�¼��������ݱ�ʶ��
}TPrgClearEventRom;
#endif


// Уʱ��¼����
#if( SEL_PRG_INFO_ADJUST_TIME == YES )	
typedef struct TPrgInfoAdjustTimeData_t
{
	TEventTime	AdjustTime;
	BYTE		Reserved[8];		// Ԥ��
}TPrgInfoAdjustTimeData;

typedef struct TPrgAdjustTimeRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoAdjustTimeData)];
}TPrgAdjustTimeRom;
#endif


// ʱ�α�������
#if( SEL_PRG_INFO_TIME_TABLE == YES )
typedef struct TPrgInfoSegTable_t
{
	BYTE	TimeSeg[MAX_TIME_SEG][3];
	BYTE	Reserved[10];		// Ԥ��
}TPrgInfoSegTable;

typedef struct TPrgSegTableRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoSegTable)];
	BYTE					TimeSegChg[MAX_TIME_SEG][3];// ���ǰʱ�α�
	DWORD					OAD;// ʱ�α���OAD
}TPrgSegTableRom;
#endif


// ʱ����������
#if( SEL_PRG_INFO_TIME_AREA == YES )
typedef struct TPrgInfoTimeArea_t
{
	BYTE	TimeArea[MAX_TIME_AREA][3];
	//BYTE	TimeAreaBackUp[MAX_TIME_AREA][3];
	BYTE	Reserved[4];		// Ԥ��
}TPrgInfoTimeArea;

typedef struct TPrgTimeAreaRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoTimeArea)];
}TPrgTimeAreaRom;
#endif


//���ܱ������ձ���¼�
#if( SEL_PRG_INFO_WEEK == YES )	
typedef struct TPrgInfoWeek_t
{
	BYTE	TableNo;
	BYTE	Reserved[4];		// Ԥ��
}TPrgInfoWeek;

typedef struct TPrgWeekRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoWeek)];
}TPrgWeekRom;
#endif


// �����ձ�̼�¼
#if( SEL_PRG_INFO_CLOSING_DAY == YES )
typedef struct TPrgInfoClosingPara_t
{
	TMonSavePara	MonSavePara[MAX_MON_CLOSING_NUM];
	BYTE			Reserved[4];		// Ԥ��
}TPrgInfoClosingPara;

typedef struct TPrgClosingRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoClosingPara)];
}TPrgClosingRom;
#endif


// ��բ��¼
typedef struct TOpenRelayRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TEventNormal)];
}TPrgOpenRelayRom;
// ��բ��¼
typedef struct TPrgCloseRelayRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TEventNormal)];
}TPrgCloseRelayRom;


// �ڼ��ձ�̼�¼
#if( SEL_PRG_INFO_HOLIDAY == YES )	
typedef struct TPrgInfoHoliday_t
{
	#ifndef MAX_HOLIDAY
	"�궨�����"
	#endif
	#if( MAX_HOLIDAY == 0 )
	BYTE byNoUse;
	#else
	// ��һ�㣺254���ڼ���
	// �ڶ��㣺�ڼ��ձ��
	BYTE Holiday[MAX_HOLIDAY][4];
	#endif
	BYTE	Reserved[10];		// Ԥ��
}TPrgInfoHoliday;

typedef struct TPrgHolidayRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgInfoHoliday)];
 	BYTE					Holiday[6];// ��GetOADLen��ȡ�ĳ��ȱ���һ�� 
 	DWORD					OAD;// �ڼ��ձ��OAD
}TPrgHolidayRom;
#endif


// �й���Ϸ�ʽ��̼�¼
#if( SEL_PRG_INFO_P_COMBO == YES )	
typedef struct TPrgPcombDataRom_t
{
	BYTE	Data;				//�й����ģʽ��
	BYTE	Reserved[4];		// Ԥ��
}TPrgPcombDataRom;

typedef struct TPrgPcombRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgPcombDataRom)];
}TPrgPcombRom;	
#endif


// �޹���Ϸ�ʽ��̼�¼
#if( SEL_PRG_INFO_Q_COMBO == YES )	
typedef struct TPrgQcombDataRom_t
{
	BYTE	Data1;				//�޹����ģʽ��1
	BYTE	Data2;				//�޹����ģʽ��2
	BYTE	Reserved[4];		// Ԥ��
}TPrgQcombDataRom;

typedef struct TPrgQcombRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgQcombDataRom)];
}TPrgQcombRom;		
#endif


//���ܱ���Կ�����¼�
#if( SEL_PRG_UPDATE_KEY == YES )
typedef struct TPrgUpdateKeyDataRom_t
{
	BYTE	TotalNum;			//��Կ������
	BYTE	KeyVersion[16];			//��Կ�汾
	BYTE	Reserved[4];		// Ԥ��
}TPrgUpdateKeyDataRom;
	
typedef struct TPrgUpdateKeyRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgUpdateKeyDataRom)];
}TPrgUpdateKeyRom;
#endif

#if(PREPAY_MODE == PREPAY_LOCAL)
//���ܱ���ʲ��������¼�
typedef struct TPrgTariffDataRom_t
{
	BYTE	Data[12*4*2];		//���ǰ��ǰ�ױ����׷��ʵ�۱�
}TPrgTariffDataRom;
typedef struct TPrgTariffRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgTariffDataRom)];
}TPrgTariffRom;	
//���ܱ���ݱ����¼�
typedef struct TPrgLadderDataRom_t
{
	BYTE	Data[80*2];		//���ǰ��ǰ�ױ����׽��ݵ�۱�
}TPrgLadderDataRom;
typedef struct TPrgLadderRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgLadderDataRom)];
}TPrgLadderRom;	
//���ܱ��쳣�忨�¼�
typedef struct TPrgAbrCardDataRom_t
{
	BYTE	Data[70];		//�쳣�忨�¼���¼��Ԫ
}TPrgAbrCardDataRom;
typedef struct TPrgAbrCardRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgAbrCardDataRom)];
}TPrgAbrCardRom;
//���ܱ����¼
typedef struct TPrgBuyMoneyDataRom_t
{
	BYTE	Data[70];		//���ܱ����¼���¼��Ԫ
}TPrgBuyMoneyDataRom;
typedef struct TPrgBuyMoneyRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgBuyMoneyDataRom)];
}TPrgBuyMoneyRom;
//���ܱ��˷Ѽ�¼
typedef struct TPrgReturnMoneyDataRom_t
{
	BYTE	Data[70];		//�˷��¼���¼��Ԫ
}TPrgReturnMoneyDataRom;
typedef struct TPrgReturnMoneyRom_t
{
	TEventOADCommonData		FixData;// ÿ������඼�еĹ�ͬ����
	BYTE					Data[sizeof(TPrgReturnMoneyDataRom)];
}TPrgReturnMoneyRom;
#endif

typedef struct TPreProgramData_t
{	
	BYTE	Data[MAX_PRG_CACHE_LEN];			//���ǰOAD����Ӧ������	
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )	
	//ֻ���޹����ģʽ�����¼�����Դ�����Է����ˡ�
	BYTE	QcombSource[MAX_EVENTRECORD_NUMBER];	//�޹����ģʽ���¼�����Դ
	#endif

}TPreProgramData;


typedef struct TProgramRecord_t
{

	#if(SEL_PRG_RECORD645 == YES)
	//645��̼�¼����ee�е������ռ�
	TPrgProgramRom645		PrgProgramRom645[MAX_EVENTRECORD_NUMBER]; 
	#endif
	//��ʱ�����������֮ǰ������
	TPreProgramData 		PreProgramData;
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼
	DWORD dwPrgQComBo2Num;//�޹���Ϸ�ʽ2�����ֱ�̴���
	//��ǰ�¼�״̬��,�޹���Ϸ�ʽ��̼�¼ֻ���淢��ʱ�̣��±�:�޹���Ϸ�ʽ1�����֣�0�����޹���Ϸ�ʽ2�����֣�1��
	TRealTimer PrgQCombo_CurrentEventStatus[2];//���������dwPrgQComBo2Num ֮������һ����
	#endif	
	
	//��ʱ�������һ�α�̼�¼�����10��OAD��ָ��	
	TPrgProgramSubRom		EeTempOadList;

}TProgramRecordRom;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern BYTE PrgFlag;
extern BYTE PrgFlag645,ProRecord645DiNum;
//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��ȡ��̼�¼״̬
//
//����: 	Phase[in]             
//����ֵ:  	TRUE/FALSE
//
//��ע: 
//-----------------------------------------------
BYTE GetPrgStatus(BYTE Phase);
void ClearPrgRecordRam( void );
BYTE GetPrgRecord( void );
void FactoryInitPrgRecordPara( void );

#endif//__PRGRECORD_H
