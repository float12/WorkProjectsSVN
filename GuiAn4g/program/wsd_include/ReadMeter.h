//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з��� 
//������	
//��������	
//����		READMETERͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __READMETER_H
#define __READMETER_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define UNSIGNED 		0x00 //�޷���
#define SIGNED 			0x80   //�з���
#define PT_MULTI 		0x01 //�ϴ����ݳ�PT
#define CT_MULTI 		0x02
#define PT_CT_MULTI 	0x03
#define METER_PHASE_NUM 3 //�������
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct
{
	BYTE RelayCmd;//�̵�������
	BYTE CollcetBit;//�ɼ���־λ
} TRelayControlInfo;
//���뷽��ӱ����п���
#pragma pack(1)
typedef struct
{
	BYTE Time[6];//�̵����仯ʱ��
	BYTE Energe[6][4];//����
	DWORD Reason;//�̵����仯ԭ��
} TRelayChangeReason;
#pragma pack()

typedef struct tMeterRead_t
{
	BYTE IsExtendOI;//�Ƿ�����չ��ʶ
	BYTE IsDoubleData;//�Ƿ���double����
	DWORD OI;	//645���ݱ�ʶ
	BYTE Num;	//һ�����ݿ��м���������
	BYTE Slen;   //�����������
	BYTE Dot;	//��������С��λ
	BYTE Symbol; //�Ƿ���Ҫ�������λ bit7 �����ж� bit1 bit0 ���ֱ��
	void *Buf; //���ݴ�ŵ�ַ
} tMeterRead;


typedef enum
{
	eOPEN_RELAY_LOOP1 = 0x4D,
	eCLOSE_RELAY_LOOP1 = 0x4F,
	eOPEN_RELAY_LOOP2 = 0x8D,
	eCLOSE_RELAY_LOOP2 = 0x8F,
	eOPEN_RELAY_LOOP3 = 0xCD,
	eCLOSE_RELAY_LOOP3 = 0xCF,
	eOPEN_RELAY_ALL_LOOP = 0x0D,
	eCLOSE_RELAY_ALL_LOOP = 0x0F,
}eRELAY_CMD;

typedef enum
{
	eTIMED_RELAY_CHANGE = 0xBB,
	eREMOTE_CMD = 0xff,
	eOVER_POWER = 0xCC,
	eOVER_VOLTAGE = 0xDD,
	eUNKOWN_REASON = 0x5A,
}eRELAY_CHANGE_REASON;

typedef enum
{
	eRELAY_OPEN = 0x01,
	eRELAY_CLOSE = 0x00,
}eRelayStatus;

typedef enum
{
	eFreq = 0,        // Ƶ��
	eUa,              // a���ѹ
	eUb,              // b���ѹ
	eUc,              // c���ѹ
	eIa,              // a�����
	eIb,              // b�����
	eIc,              // c�����
	eActPAll,         // ���й�����
	eActPa,           // a�������й�
	eActPb,           // b�������й�
	eActPc,           // c�������й�
	eReactPAll,       // ���޹�����
	eReactPa,         // a���޹�
	eReactPb,         // b���޹�
	eReactPc,         // c���޹�
	eAppPAll,         // �����ڹ���
	eAppPa,           // a�����ڹ���
	eAppPb,           // b�����ڹ���
	eAppPc,           // c�����ڹ���
	ePFAll,           // �ܹ�������
	ePFa,             // a�๦������
	ePFb,             // b�๦������
	ePFc,             // c�๦������
	eTotalFwdActEp,   // �������й�����
	eFwdActEpa,       // a�������й�����
	eFwdActEpb,       // b�������й�����
	eFwdActEpc,       // c�������й�����
	eTotalRevActEp,   // �ܷ����й�����
	eRevActEpa,       // a�෴���й�����
	eRevActEpb,       // b�෴���й�����
	eRevActEpc,        // c�෴���й�����
	eLoop1RelayStatus,
	eLoop1RelayChangeTime,
	eLoop1RelayChangeReason,
	eLoop2RelayStatus,
	eLoop2RelayChangeTime,
	eLoop2RelayChangeReason,
	eLoop3RelayStatus,
	eLoop3RelayChangeTime,
	eLoop3RelayChangeReason,
} eValueType; //  �޸�MeterReadOI�������Ӧ�޸�ö��ƫ�� 
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern const tMeterRead MeterReadOI[];
extern const BYTE CycleReadMeterNum;
extern BYTE gRelayStatus[METER_PHASE_NUM]; //�̵���״̬
extern TRelayChangeReason RelayChangeReason[METER_PHASE_NUM][2]; //�̵����仯ԭ��
//-----------------------------------------------
// 				��������
//-----------------------------------------------
#if (CYCLE_METER_READING == YES)
//--------------------------------------------------
//��������:  645 ��������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Dlt645_Tx_ContinueRead( BYTE bStep, BYTE *bBuf);
//-----------------------------------------------
//��������:  �������ݽ���������
//
//����:      pBuf[in]  dataBuf[in] ���ݱ���λ��
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void DataAnalyseandSave(BYTE *pBuf, double *dataBuf, BYTE Len, tMeterRead tMeterReadOIRam);
//--------------------------------------------------
//��������: �������ݴ��͸���ͬ�ϱ���֡����
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void SampleDatatoReport(TRealTimer *pTime);
#endif
#endif //#ifndef __READMETER_H
