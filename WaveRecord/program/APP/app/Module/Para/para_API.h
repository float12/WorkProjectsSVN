//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.2
//����		��������ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __PARA_API_H
#define __PARA_API_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	//ʱ���������
	eCLOCK_PULS_OUTPUT=0,
	//�����������
	eDEMAND_PERIOD_OUTPUT,
	//ʱ���л����
	eSEGMENT_SWITCH_OUTPUT
	
}TOutputType;

typedef struct TGlobalVariable_t
{
	//�๦�ܶ����������
	BYTE g_byMultiFunPortType;
	//�����������������
	BYTE g_byMultiFunPortCounter;
	

}TGlobalVariable;


typedef struct TMeterSnPara_t
{
	//ͨѶ��ַ��BCD [0]-������ֽ�
	BYTE CommAddr[6];
	//��ţ�BCD  [0]-������ֽ�
	BYTE MeterNo[6];
	//�ͻ����   [0]-������ֽ�
	BYTE CustomCode[6];
}TMeterSnPara;


#pragma pack(1)
typedef struct TLCDPara_t
{
	//�ϵ�ȫ��ʱ��  unsigned��
	BYTE PowerOnDispTimer;//�ϵ米�����ʱ��Ҳ�����
	//�������ʱ��  long-unsigned(����ʱ�������ʱ��)��
	WORD BackLightPressKeyTimer;
	//��ʾ�鿴�������ʱ��  long-unsigned��
	WORD BackLightViewTimer;
	//�޵簴����Ļפ�����ʱ��  long-unsigned�� HEX ��λΪ��
	WORD LcdSwitchTime;
	//����С����
	BYTE EnergyFloat;
	//���ʣ�������С����
	BYTE DemandFloat;
	//��ʾ12��������  0:������ʾ��ǰ�ס�������ʱ�� 1��������ʾ��ǰ�ס������׷��ʵ��
	BYTE Meaning12;
	
	//�±�:0--ѭ����Ŀ�� 1--������Ŀ�� 
	BYTE DispItemNum[2];
	//�����л���ѭ����ʾ��ʱ�� ��ʾ������3����
	WORD KeyDisplayTime;
	//ѭ����ʾʱÿ������ʾʱ�� ��ʾ������3����
	WORD LoopDisplayTime;
	
}TLCDPara;
#pragma pack()


#pragma pack(1)
typedef struct TTimeZoneSegPara_t
{
	//��ʱ������Hex��
	BYTE TimeZoneNum;
	//��ʱ�α�����Hex��
	BYTE TimeSegTableNum;
	//��ʱ������Hex��
	BYTE TimeSegNum;
	//��������ҪС������������Hex�룬��ǰ�û����õ���������
	BYTE Ratio;
	//������������Hex�� ��Ϊ�¹�ԼҪ��2bytes n <= 254
	BYTE HolidayNum;
	
	//����״̬�֣�1-������0-��Ϣ��
	//D0	����
	//D1	��һ
	//D2	�ܶ�
	//D3	����
	//D4	����
	//D5	����
	//D6	����
	BYTE WeekStatus;
	//�����ղ��õ�ʱ�α�
	BYTE WeekSeg;

}TTimeZoneSegPara;
#pragma pack()


typedef struct TFPara1_t
{
	TMeterSnPara MeterSnPara;

	//Ϊ�����Լ����������ϵĽṹ
	TLCDPara LCDPara;

	TTimeZoneSegPara TimeZoneSegPara;
	
	WORD wClearMeterType; //0x1111--�������(���ݳ�ʼ��) 0x2222--�ָ��������� 0x4444--�¼����� 0x8888--��������

	//У��
	DWORD CRC32;
	
}TFPara1;


#pragma pack(1)
typedef struct TIntervalDemandFreezePeriod_t
{
	BYTE Unit;				//�ڼ��������ڵ�λ	����
	WORD FreezePeriod;		//�ڼ���������
	
}TIntervalDemandFreezePeriod;
#pragma pack()


typedef struct TEnereyDemandMode_t
{
	//�й���Ϸ�ʽ������ ����й�
	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
	//����	����		����	����	�����й�	�����й�	�����й�	�����й�
	//									0����1��	0����1��	0����1��	0����1��
	BYTE byActiveCalMode;
	
	//04000602 04000603 �޹���Ϸ�ʽ1��2������
	//Bit7		Bit6		Bit5		Bit4		Bit3		Bit2		Bit1		Bit0
	//IV����	IV����		III����		III����		II����		II����		I����		I����
	//0����1��	0����1��	0����1��	0����1��	0����1��	0����1��	0����1��	0����1��
	#if(SEL_RACTIVE_ENERGY == YES)
	BYTE PReactiveMode;//�����޹�ģʽ������
	BYTE NReactiveMode;//�����޹�ģʽ������
	#endif
	
	#if( MAX_PHASE != 1 )
	//��������
	BYTE DemandPeriod;	
	//��������ʱ��
	BYTE DemandSlip;
	//�ڼ�������������
	TIntervalDemandFreezePeriod IntervalDemandFreezePeriod;
	#endif

}TEnereyDemandMode;


//ͨѶ���Ʋ���
typedef struct TCommPara_t
{
	//ͨѶ���Ʋ���
	//�߼���һ���п�
	//D3-D0	�����ʿ���
	//		0000 -- 300
	//		0001 -- 600
	//		0010 -- 1200
	//		0011 -- 2400
	//		0100 -- 4800
	//		0101 -- 7200
	//		0110 -- 9600
	//		0111 -- 19200
	// 		1000 -- 38400
	// 		1001 -- 57600
	// 		1010 -- 115200 A
	//		1011 -- 230400 B
	//		1100 -- 460800 C
	//		1101 -- 921600 D
	//D4	ֹͣλ
	//		0 -- 1λֹͣλ
	//		1 -- 2λֹͣλ
	//D6-D5	У�����
	//		00 -- ��У��
	//		01 -- ��У��
	//		10 -- żУ��
	//D7	Ӳ������
	//		0 -- ��Ӳ������
	//		1 -- ��Ӳ������
	
	BYTE I485;
	
	//�߼��������пڣ��ڶ����ڹ̶�Ϊ�����˲��Һ���Ĳ������ǹ̶��ģ�
	BYTE ComModule;
	
	//�߼����Ĵ��п�
	BYTE II485;

	//CAN���߲�����������
	BYTE CanBaud;
	
}TCommPara;


//�ڶ�������
typedef struct TFPara2_t
{
	TEnereyDemandMode EnereyDemandMode;
	
	//ͨѶ���Ʋ���
	TCommPara CommPara;
	
	DWORD ActiveConstant;//HEX data
	DWORD ReactiveConstant;//HEX data

	//У��
	DWORD CRC32;

}TFPara2;



typedef struct TMuchPassword_t
{
	//MAX_PASSWORD_LEVELΪ5���Լ��ݹ���2,4�����룬����3�����룬������0�����룬���±��ʾ
	BYTE Password645[MAX_645PASSWORD_LEVEL][MAX_645PASSWORD_LENGTH];
	BYTE Password698[MAX_698PASSWORD_LENGTH];
	DWORD CRC32;
}TMuchPassword;


//ʱ�Ρ�ʱ�����ã�ע������ʱ��ʱ�α����ܻ�����FlashBuf������������䲻�ܳ���FlashBuf��һ�롣
typedef struct TTimeAreaTable_t
{
	//**************************************************
	//ʱ����ʼ���ڼ�ʱ�α��
	//ÿ���ʽΪ-----�£��գ�ʱ�α�ţ�0��1��2��
	BYTE TimeArea[MAX_TIME_AREA][3];
	DWORD CRC32;
	
}TTimeAreaTable;


typedef struct TTimeSegTable_t
{
	//**************************************************
	//ʱ�α�����
	//ÿ���ʽΪ-----ʱ���֣����ʺţ�0��1��2��
	BYTE TimeSeg[MAX_TIME_SEG][3];
	DWORD CRC32;

}TTimeSegTable;


#pragma pack(1)
typedef struct TTimeHoliday_t
{
	TRealTimerDate RealTimerDate;
	BYTE HolidaySegNum;
	
}TTimeHoliday;


typedef struct TTimeHolidayTable_t
{
	TTimeHoliday TimeHoliday[MAX_HOLIDAY];
	DWORD CRC32;
	
}TTimeHolidayTable;
#pragma pack()


typedef struct TTimeTable_t
{
	//ʱ������
	TTimeAreaTable TimeAreaTable;
	//ʱ�α�����
	TTimeSegTable TimeSegTable[MAX_TIME_SEG_TABLE];

}TTimeTable;


typedef struct TSwitchTimePara_t
{
	//04000106  YYMMDDhhmm		5   ������ʱ��  ����ʱ�����л�ʱ��
	//04000107	YYMMDDhhmm		5	������ʱ��	 ������ʱ���л�ʱ��
	//04000108  YYMMDDhhmm		5	���׷�ʱ�����л�ʱ��
	//04000109	YYMMDDhhmm 		5	�����ݶ��л�ʱ��
	//0400010A	YYMMDDhhmm 		5	����
	//�������ʱ�䣺�л�ʱ����2000��1��1��0ʱ0��֮���ʱ����λ������ (�ú���api_CalcInTimeRelativeMin()����ֵ)��
	DWORD dwSwitchTime[5];
	
	DWORD CRC32;

}TSwitchTimePara;





typedef struct TMonSavePara_t
{
	//DL/T645-2007Ҫ��ÿ�����3�ν��㣬��ÿ�������ն�����ʱ
	//HEX�룬��Ϊ0xFF��Ч����Ӧһ��ÿһ�졣
	BYTE Day;
	//����Сʱ HEX�룬��ΪFF��Ч
	BYTE Hour;

}TMonSavePara;

typedef struct TBillingPara_t
{
	//DL/T645-2007Ҫ��ÿ�����3�ν��㣬��ÿ�������ն�����ʱ
	TMonSavePara MonSavePara[MAX_MON_CLOSING_NUM];
	//У��
	DWORD CRC32;
	
}TBillingPara;
typedef struct TReportModeConst_t
{
  BYTE EventIndex;//���ϱ��¼�
  BYTE ReportMode;//�ϱ�ģʽ
	
}TReportModeConst;

#if(SEL_EVENT_DI_CHANGE == YES)
#pragma pack(1)
typedef struct TDIPara_t
{
	DWORD 	AntiShakeTime;			//ң�ŷ�����ʱ��
	BYTE	bAccessState;			//����״̬
	BYTE	bAttributeState;		//����״̬
	DWORD	CRC32;
}TDIPara;
#pragma pack()
#endif

typedef struct TSafeMeterPara_t
{
	//����
	TMuchPassword MuchPassword;
	
	//��һ��ʱ��ʱ�α�
	TTimeTable FirstTimeTable;
	//�ڶ���ʱ��ʱ�α�
	TTimeTable SecondTimeTable;
	//�ڼ�������
	TTimeHolidayTable TimeHolidayTable;
	//�л�����
	TSwitchTimePara SwitchTimePara;
	
	TFPara1 FPara1;
	TFPara2 FPara2;
	
	//�½������������
	TBillingPara BillingPara;	

	//CAN���߲���
//	TCanBusPara CanBusPara;

	#if(SEL_EVENT_DI_CHANGE == YES)
	TDIPara DIPara;
	#endif

}TParaSafeRom;
////////////////////////////////////////////////////////////////
//�����������ռ����
////////////////////////////////////////////////////////////////
//645-2007�йص���ͺŵȷ���Ĳ���
typedef struct TMeterTypePara_t
{
    //698.45Э��汾��(��������:WORD)  13	 - Ϊ����������⣬�ʷŵ���λ��-jwh
	WORD ProtocolVersion;
	//�ʲ�������� ascii�� 3
	BYTE PropertyCode[32];
	//���ѹ ascii��     4
	BYTE RateVoltage[6];
	//����� ascii��     5
	BYTE RateCurrent[6];
	//������ ascii��     6
	BYTE MaxCurrent[6];
	//��С���� ascii��     7
	BYTE MinCurrent[10];
	//ת�۵��� ascii��     8
	BYTE TurningCurrent[10];
	//�й����ȵȼ� ascii�� 9
	BYTE PPrecision[4];
	//�޹����ȵȼ� ascii�� 10
	BYTE QPrecision[4];
	//����ͺ� ascii��     13
	BYTE MeterModel[32];
	//�������� date_time_s    14 645 698���� ��HEX���ݣ���ASCIIʱתһ�� 
	BYTE ProduceDate[sizeof(TRealTimer)];
	//���λ����Ϣ ���� γ�� �߶� 17
	BYTE MeterPosition[18];
	//��������汾�� ascii�� 19
	BYTE SoftWareVersion[4];
	//��������汾����	20
	BYTE SoftWareDate[6];
	//����Ӳ���汾�� ascii�� 21
	BYTE HardWareVersion[4];
	//����Ӳ���汾��	22
	BYTE HardWareDate[6];
	//���ұ��  ascii��       18
	BYTE FactoryCode[4];
	//���������  ASCII��     23
	BYTE SoftRecord[16];
	
}TMeterTypePara;


typedef struct TContinueMeterPara_t
{
	WORD			ConstParaSum;	//������ee�ı�����У��ͣ�����ȫ����ʼ��ʱ��¼�´���ֵ
	TMeterTypePara MeterTypePara;
	
}TParaConRom;

//4Gģ��
typedef struct
{
	//GPRS���� IP��ַ(4)���˿ں�(2)��APN(16)���ۼƺ�(1)
	BYTE IP_Port_APN[23];
	//GPRS���� ��������(2)���ն˵�ַ(2)���ۼƺ�(1)
	BYTE AreaCode_Ter[5];
	//GPRS���� ģ��APN�û���(32)
	BYTE APN_UseName[32];
	//GPRS���� ģ��APN��¼����(32)���û��������ۼƺ�(1)
	BYTE APN_PassWord[33];
}TGprsTypePara;

typedef struct
{
	TGprsTypePara TcpipPara;
	TGprsTypePara MqttPara;
	BYTE Reserved[128];
}TGprsConRom;
////////////////////////////////////////////////////////////
//���������Ͷ��� 1~15��ֵ��Ҫ�Ķ����Ժ�645��˳��һ�£��������wlk 2017/2/16 16-20˳������ı� -jwh
////////////////////////////////////////////////////////////
typedef enum
{
	//���ͨѶ��ַ          1
	eMeterCommAddr = 1,
	//�����               2
	eMeterMeterNo,
	//�ʲ�������� ascii�� 3
	eMeterPropertyCode,
	//���ѹ ascii��     4
	eMeterRateVoltage,
	//����� ascii��     5
	eMeterRateCurrent,
	//������ ascii��     6
	eMeterMaxCurrent,
	//��С���� ascii��  7
	eMeterMinCurrent,
	//ת�۵��� ascii��     8
	eMeterTurningCurrent,
	//�й����ȵȼ� ascii�� 9
	eMeterPPrecision,
	//�޹����ȵȼ� ascii�� 10
	eMeterQPrecision,
	//�й����峣��         11
	eMeterActiveConstant,
	//�޹����峣��         12
	eMeterReactiveConstant,
	//����ͺ� ascii��  13
	eMeterMeterModel,
	//�������� date_time_s     14
	eMeterProduceDate,
	//698.45Э��汾��(��������:WORD)   15
	eMeterProtocolVersion,
	//�ͻ����             16
	eMeterCustomCode,
	//���λ����Ϣ ���� γ�� �߶� 17
	eMeterMeterPosition,
	//���ұ��  ascii��      18
	eMeterFactoryCode,
	//��������汾�� ascii�� 19
	eMeterSoftWareVersion,
	//��������汾����				20
	eMeterSoftWareDate,
	//����Ӳ���汾�� ascii�� 21
	eMeterHardWareVersion,
	//����Ӳ���汾����				22
	eMeterHardWareDate,

	//���������          23
	eMeterSoftRecord,
	
}TMeterTypeEnum;

typedef enum
{
	//IP��ַ���˿ںš�APN
	eTCPIP_IP_Port_APN = 0,
	//GPRS���� �������롢�ն˵�ַ
	eTCPIP_AreaCode_Ter,
	//GPRS���� ģ��APN�û���
	eTCPIP_APN_UseName,
	//GPRS���� ģ��APN��¼����
	eTCPIP_APN_PassWord,
	//IP��ַ���˿ںš�APN
	eMQTT_IP_Port_APN,
	//GPRS���� �������롢�ն˵�ַ
	eMQTT_AreaCode_Ter,
	//GPRS���� ģ��APN�û���
	eMQTT_APN_UseName,
	//GPRS���� ģ��APN��¼����
	eMQTT_APN_PassWord,
}TGprsTypeEnum;
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern TFPara1* const FPara1;
extern TFPara2* const FPara2;


//System ϵͳ���ò���

//Sample ����Ĭ�ϲ���
extern const WORD Uoffset;		
extern const WORD Defendenergyvoltage;	
extern const WORD Def_Shielding_I1;	
extern const WORD Def_Shielding_I2;	
extern const WORD Def_MaxError;
extern const DWORD	MaxEnergyPulseConst;
extern const WORD SimpleCurrConst;
extern const WORD SimpleVolConst;
extern const WORD IRegionConst;
extern const WORD wSampleVolGainConst;
extern const WORD wSampleCurGainConst;
extern const WORD	wStandardVoltageConst;
extern const WORD	wCaliVoltageConst;
//extern const WORD	wMeterBasicCurrentConst;
extern const WORD wSampleMinCurGainConst; //����оƬ�������汶��
extern const WORD IRegionUPConst;
extern const WORD IRegionDownConst;
extern const DWORD dwMeterBasicCurrentConst;
extern const DWORD	dwMeterMaxCurrentConst;
#if( MAX_PHASE == 3 )
#if( SAMPLE_CHIP==CHIP_HT7627 )
extern const WORD ZeroSampleCurrConst;
extern const BYTE SampleCtrlAddr[15];
extern const BYTE SampleAdjustAddr[22];
extern const BYTE SampleOffsetAddr[19];
extern const WORD SampleCtrlDefData[15];
extern const WORD SampleAdjustDefData[22];
extern const WORD SampleOffsetDefData[20];
extern const WORD SampleHalfWaveDefData[12];
extern const WORD SampleTCcoffDefData[3];
extern const BYTE PhaseBRegAddr[9];
extern const WORD HD2CheckCurrentValueMin;
extern const WORD HD2CosLimitValue;
extern const WORD HD2CurrentDiffValue;
extern const BYTE HD2ContentMin;
extern const BYTE HD2ContentMax;
extern const BYTE HD2DiffValue;
extern const BYTE FirstDiffValue;
extern const WORD HD2CurrentRangeMin;
extern const WORD HD2CurrentRangeMax;
extern const BYTE HalfWaveDefCheckTime;

#elif( SAMPLE_CHIP==CHIP_RN8302B )
extern const WORD ZeroSampleCurrConst;
extern const DWORD SampleCtrlAddr[18][2];
extern const DWORD SampleAdjustAddr[33][2];
//extern const BYTE SampleOffsetAddr[19];
extern const DWORD SampleCtrlDefData[18];
extern const DWORD SampleAdjustDefData[33];
extern const DWORD SampleUIPGain[11];
//extern const WORD SampleOffsetDefData[20];
extern const WORD SampleHalfWaveDefData[12];
extern const WORD SampleTCcoffDefData[3];
extern const BYTE PhaseBRegAddr[9];
extern const WORD HD2CheckCurrentValueMin;
extern const WORD HD2CosLimitValue;
extern const WORD HD2CurrentDiffValue;
extern const BYTE HD2ContentMin;
extern const BYTE HD2ContentMax;
extern const BYTE HD2DiffValue;
extern const BYTE FirstDiffValue;
extern const WORD HD2CurrentRangeMin;
extern const WORD HD2CurrentRangeMax;
extern const BYTE HalfWaveDefCheckTime;
#endif

#else
extern const BYTE SampleCtrlAddr[7];
extern const BYTE SampleAdjustAddr[15];
extern const WORD SampleCtrlDefData[8];
extern const WORD SampleCtrlDefData8000[8];
extern const WORD SampleAdjustDefData[15];
#endif
//LCD ��ʾĬ�ϲ���
extern const TLCDPara LCDParaDef;
#if( LCD_TYPE == LCD_HT_SINGLE_METER )
extern const BYTE LoopItemDef[15];
extern const BYTE KeyItemDef[60];
#endif
#if( LCD_TYPE == LCD_NO )
extern const BYTE LoopItemDef[35];
extern const BYTE KeyItemDef[110];
#endif
extern const BYTE ExpandLcdItemTable[20][10];
//Protocol ͨ��Ĭ�ϲ���
extern const WORD BlePayload;

//Lpf ��������Ĭ�ϲ���
//Demand ����Ĭ�ϲ���
//Event �¼�Ĭ�ϲ���
extern const BYTE	EventLostVTimeConst;
extern const BYTE  EventLostVURateConst;
extern const BYTE  EventLostVIRateConst;
extern const BYTE  EventLostVRecoverLimitVRateConst;
extern const BYTE	EventWeakVTimeConst;
extern const BYTE  EventWeakVURateConst;
extern const BYTE	EventOverVTimeConst;
extern const BYTE  EventOverVURateConst;
extern const BYTE	EventBreakTimeConst;
extern const BYTE  EventBreakURateConst;
extern const BYTE  EventBreakIRateConst;
extern const BYTE	EventLostITimeConst;
extern const BYTE  EventLostIURateConst;
extern const BYTE  EventLostIIRateConst;
extern const BYTE  EventLostILimitLIConst;
extern const BYTE	EventOverITimeConst;
extern const BYTE  EventOverIIRateConst;
extern const BYTE	EventBreakITimeConst;
extern const BYTE  EventBreakIURateConst;
extern const BYTE  EventBreakIIRateConst;
extern const BYTE	EventBackpTimeConst;
extern const BYTE  EventBackpPRateConst;
extern const BYTE	EventOverLoadTimeConst;
extern const BYTE  EventOverLoadPRateConst;
extern const BYTE	EventDemandOverTimeConst;
extern const BYTE  EventDemandOverRateConst;
extern const BYTE	EventCosOverTimeConst;
extern const WORD  EventCosOverRateConst;
extern const BYTE	EventLostAllVTimeConst;
extern const BYTE	EventLostSecPowerTimeConst;
extern const BYTE	EventVReversalTimeConst;
extern const BYTE	EventIReversalTimeConst;
extern const BYTE	EventLostPowerTimeConst;
extern const BYTE	EventVUnbalanceTimeConst;
extern const WORD  EventVUnbalanceRateConst;
extern const BYTE	EventIUnbalanceTimeConst;
extern const WORD  EventIUnbalanceRateConst;
extern const BYTE	EventISUnbalanceTimeConst;
extern const BYTE	EventSampleChipErrTimeConst;   
extern const WORD  EventISUnbalanceRateConst;
extern const BYTE  EventStatVRateConst;
extern const BYTE  EventStatVRunLimitLVConst;
extern const BYTE  EventStatVRunRecoverHVConst;
extern const BYTE  EventStatVRunRecoverLVConst;
extern const BYTE  EventNeutralCurrentErrTimeConst;
extern const DWORD EventNeutralCurrentErrLimitIConst; 
extern const WORD  EventNeutralCurrentErrRatioConst;
//������Ȧ����¼�
#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )
extern const WORD	EventRogowskiADelayConst;
extern const WORD   EventRogowskiBDelayConst;
#endif
//�ն˳���ʧ���¼�
#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
extern const BYTE	EventRetryNoConst;
extern const BYTE   EventCollectNoConst;
#endif

//PrePay Ԥ����Ĭ�ϲ���
//Freeze ����Ĭ�ϲ���

//Energy ����Ĭ�ϲ���
//Para ����Ĭ�ϲ���
extern const DWORD ActiveConstantConst;

extern const TEnereyDemandMode EnereyDemandModeConst;
extern const TCommPara CommParaConst;
extern const BYTE MonBillParaConst[];
extern const TTimeZoneSegPara TimeZoneSegParaConst;
// extern const TTimeAreaTable TimeAreaTableConst1;
// extern const TTimeAreaTable TimeAreaTableConst2;
extern const TTimeSegTable TimeSegTableConst1;
extern const TTimeSegTable TimeSegTableConst2;
extern const TTimeSegTable TimeSegTableConst3;
extern const TTimeSegTable TimeSegTableConst4;
extern const BYTE TimeSegTableFlag[];
// extern const TSwitchTimePara SwitchTimeParaConst;
extern const TMuchPassword MuchPasswordConst;
extern const TWarnDispPara WarnDispParaConst;
extern const BYTE RateVoltageConst[6];
extern const BYTE RateCurrentConst[6];
extern const BYTE MaxCurrentConst[6];
extern const BYTE MinCurrentConst[10];
extern const BYTE TurnCurrentConst[10];
extern const BYTE PPrecisionConst[4];
extern const BYTE QPrecisionConst[4];
extern const BYTE MeterModelConst[32];
extern const BYTE MeterPositionConst[18];
extern const WORD ProtocolVersionConst;
extern const BYTE ProtocolVersionConst_645[16];
extern const BYTE ProtocolVersionConst_698[21];
extern const BYTE SoftWareVersionConst[4];
extern const BYTE SoftWareDateConst[6];
extern const BYTE HardWareVersionConst[4];
extern const BYTE HardWareDateConst[6];
extern const BYTE FactoryCodeConst[4];
extern const BYTE SoftRecordConst[16];
extern const BYTE DefCurrRatioConst;
extern const WORD TimeBroadCastTimeMinLimit; 						//�㲥Уʱ��С��Ӧ���� ��λ��
extern const WORD TimeBroadCastTimeMaxLimit;						//�㲥Уʱ�����Ӧ���� ��λ��
extern const DWORD	IRTimeConst;
extern const BOOL  RelayKeepPowerFlag;
#if( SEL_DLT645_2007 == YES )
extern const DWORD Remote645AuthTimeConst;
#endif
extern const BYTE	ScretKeyNum;   
extern const BYTE	LadderNumConst;
#if( PREPAY_MODE == PREPAY_LOCAL )
extern const BOOL  RelayKeepPowerFlag;
extern const DWORD	TickLimitConst;  
extern const DWORD	RegrateLimitConst;
extern const DWORD	CloseLimitConst;
#endif
//Rtc ʱ��Ĭ�ϲ���
extern const signed short TAB_Temperature[];
extern const unsigned short TAB_DFx_K[10];
extern const long ADCINcoffConst_K;
extern const long ADCINOffsetConst_K;
extern const long VBatcoffConst_K;
extern const long VBatOffsetConst_K;
extern const long VcccoffConst_K;
extern const long VccOffsetConst_K;
extern const long TPScoffConst_K;
extern const long TPSOffsetConst_K;

extern const WORD WaitOSCRunTime;

//LowPower �͹���Ĭ�ϲ���
//SysWatch ����Ĭ�ϲ���
extern const WORD ReadBatteryStandardVoltage;
extern const WORD ClockBatteryStandardVoltage;

//Relay �̵���Ĭ�ϲ���
#if( SEL_DLT645_2007 == YES )
extern const WORD	wRelayWaitOffTime_Def;	
#endif
extern const WORD	OverIProtectTime_Def;	
extern const DWORD	RelayProtectI_Def;	
extern const BYTE	RelayCtrlMode_Def;
extern const BYTE	MeterReportFlagConst;
extern const BYTE	MeterActiveReportFlagConst;
extern const BYTE	MeterReportStatusFlagConst;
//Ĭ���¼��ϱ���ʽΪ�����ϱ����б�
extern const BYTE	ReportActiveMethodConst[15+1];
//Ĭ�����ϱ��¼����ϱ�ʱ���б�
extern const TReportModeConst	ReportModeConst[15+1];
extern const BYTE	FollowReportModeConst[4];
extern const BYTE	FollowReportChannelConst[MAX_COM_CHANNEL_NUM+1];
extern const DWORD ChannelOAD[];
#if(SEL_EVENT_DI_CHANGE == YES)
extern const TDIPara DIParaconst;
#endif
extern TGlobalVariable GlobalVariable;
extern const DWORD HT7627SUConst;
extern const DWORD HT7627SIConst;

//4Gģ��Ĭ�ϲ���
extern const BYTE TCPIP_IP_Port_APNConst[23];
extern const BYTE TCPIP_AreaCode_TerConst[5];
extern const BYTE TCPIP_APN_UseNameConst[32];
extern const BYTE TCPIP_APN_PassWordConst[33];

extern const BYTE MQTT_IP_Port_APNConst[23];
extern const BYTE MQTT_AreaCode_TerConst[5];
extern const BYTE MQTT_APN_UseNameConst[32];
extern const BYTE MQTT_APN_PassWordConst[33];
//-----------------------------------------------
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ˢ�²�����RAM�еĲ���
//
//����: 
//			No[in]					0--FPara1  
//									1--FPara2 
//                    
//����ֵ:  	TRUE: ������ȷ 	FALSE����������
//��ע:  ����eeprom�е����ݸ��µ�ǰram�еĲ��� 
//-----------------------------------------------
BOOL api_FreshParaRamFromEeprom( WORD No );


//-----------------------------------------------
//��������: �Ѳ���д��FPara1��FPara2��������ǰram�����Ҵ洢����Ӧ��eeprom��
//
//����: 
//			No[in]					0��FPara1  1��FPara2
//			Addr[in]				�ṹ�ڵ�ƫ�Ƶ�ַ
//			Length[in]				Ҫд������ݳ���
//			Ptr[in]					���Ҫд�������
//                    
//����ֵ:  	TRUE: ������ȷ 	FALSE����������
//��ע:   
//-----------------------------------------------
BOOL api_WritePara(WORD No, WORD Addr, WORD Length, BYTE * Ptr);

//-----------------------------------------------
//��������: ��д4Gģ����ز���
//
//����: 
//			Operation[in]			������ʽ ֻ����WRITE��READ
//			Type[in]				��Ҫ�����������ͣ�Type���͵���Ŷ��岻Ҫ�ģ��ǰ�645-2007��˳�����
//			pBuf[in]				�������ݵĻ���
//                    
//����ֵ:  	���ز������ݵĳ��ȣ����дʧ�ܣ�����0
//
//��ע:  	
//-----------------------------------------------
BYTE api_ProcGprsTypePara( BYTE Operation, TGprsTypeEnum Type, BYTE *pBuf);

//-----------------------------------------------
//��������: ������һЩ��ز���
//
//����: 
//			Operation[in]			������ʽ ֻ����WRITE��READ
//			Type[in]				��Ҫ�����������ͣ�Type���͵���Ŷ��岻Ҫ�ģ��ǰ�645-2007��˳�����
//			pBuf[in]				�������ݵĻ���
//                    
//����ֵ:  	���ز������ݵĳ��ȣ����дʧ�ܣ�����0
//
//��ע:  	
//-----------------------------------------------
BYTE api_ProcMeterTypePara( BYTE Operation, BYTE Type, BYTE *pBuf );

//-----------------------------------------------
//��������: ����ǰ�������׵�ʱ����ʱ�α��������ձ�
//
//����: 	Type[in]				
//						D7:	0--��һ��ʱ��ʱ�α� 1--�ڶ���ʱ��ʱ�α�
//						D6--D0:
//							1 -- TTimeAreaTable
//							2 -- TTimeHolidayTable
//							3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable
//			Addr[in]	����Ӧ�ṹ�е�ƫ��
//			Length[in]	Ҫ�������ݵĳ���
//			pBuf[out]	�������ݵĻ���
//                    
//����ֵ:  	TRUE:��ȷ	FALSE:����
//
//��ע:  	
//-----------------------------------------------
BOOL api_ReadTimeTable(BYTE Type, WORD Addr, WORD Length, BYTE * Buf);

//-----------------------------------------------
//��������: д�뵱ǰ�������׵�ʱ����ʱ�α��������ձ�
//
//����: 	Type[in]				
//						D7:	0--��һ��ʱ��ʱ�α� 1--�ڶ���ʱ��ʱ�α�
//						D6--D0:
//							1 -- TTimeAreaTable
//							2 -- TTimeHolidayTable
//							3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable
//			Addr[in]	����Ӧ�ṹ�е�ƫ��
//			Length[in]	Ҫд�����ݵĳ���
//			pBuf[in]	д�����ݵĻ���
//                    
//����ֵ:  	TRUE:��ȷ	FALSE:����
//
//��ע:  	
//-----------------------------------------------
BOOL api_WriteTimeTable(BYTE Type, WORD Addr, WORD Length, BYTE * Buf);

//-----------------------------------------------
//��������: �õ���ǰ����
//
//����: 	��
//                   
//����ֵ:   BYTE:���ص�ǰ����
//
//��ע:   
//-----------------------------------------------
BYTE api_GetCurRatio( void );

//-----------------------------------------------
//��������: ������ѭ������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_ParaTask( void );

//-----------------------------------------------
//��������: ���������ϵ��ʼ��
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_PowerUpPara( void );

//-----------------------------------------------
//��������: ���ʱ�α�ÿ���ӵ���һ��
//
//����: 
//			Type[in]				TRUE ת��  FALSE ��ת��
//                    
//����ֵ:  	��
//��ע:   
//-----------------------------------------------
void api_CheckTimeTable(WORD Type);

//-----------------------------------------------
//��������: ����ʼ��
//
//����:		Type[in]	0--��ȫ��ʼ��	1--��ʼ��������ʾ�����ᡢ�¼���Ĳ���
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_FactoryInitTask( void );

//-----------------------------------------------
//��������: �����峣�����кϷ����ж�
//
//����:		��
//						
//����ֵ:	�й����峣��
//		   
//��ע:
//-----------------------------------------------
DWORD api_GetActiveConstant( void );
//-----------------------------------------------
//��������: ��ȡ�������������1
//
//����:		bit--����ڼ�λ
//						
//����ֵ:	���ֹ���(bit0~2)�ֱ����λ���
//		   
//��ע:
//		bit2�����ϱ�ģʽ (0�����ú�����־��1���ú�����־)
//		bit1Һ���٢��������� (0��ʾ1��2��ʱ�Σ�1��ʾ1��2�׷���)
//		bit0���ÿ��ؿ��Ʒ�ʽ(0��ƽ��1���壩
//
//-----------------------------------------------
DWORD api_GetMeterRunFeature1(BYTE bit);
//---------------------------------------------------------------
//��������: �õ��л�ʱ�����Է��������������Ƿ�Խ��
//
//����: 	
//			TmpRealTimer[in] --�л�ʱ�䣬��ʽYYYYMMDDHHmm��HEX				
//			pdwTime[out]--�л�ʱ�䣬�����2000��1��1��0ʱ0�ֵ���Է�����
//                    
//����ֵ:  	TRUE--���ݺϸ�FALSE--����Խ��
//
//��ע:   
//---------------------------------------------------------------
BOOL api_GetSwitchTimeRelativeMin( DWORD *pdwTime, TRealTimer TmpRealTimer );
//---------------------------------------------------------------
//��������: ��д�л�ʱ��
//
//����: 	
//			Operation[in]	������ʽ ֻ����WRITE��READ
//			Type[in]				
//							0--����ʱ�����л�ʱ��
// 							1--������ʱ���л�ʱ��
// 							2--���׷�ʱ�����л�ʱ��
// 							3--���׽����л�ʱ�� 
// 							4--���׽���ʱ�����л�ʱ��(����)
//										
//			pBuf[in]		��������д�����ݵĻ���HEX(YYMMDDhhmm) YYΪ�����ֽ� ��20 ����2017
//                    
//����ֵ:  	TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL api_ReadAndWriteSwitchTime( BYTE Operation, WORD Type, BYTE *pTmpArray );
//-----------------------------------------------
//��������: ��Para1������E2�ָ�
//
//����:		��
//						
//����ֵ:	
//		   
//��ע:�͹����ϵ��дʱ��ǰ����
//-----------------------------------------------
void api_ReadPara1FromEE(void);


#endif//#ifndef __PARA_API_H
