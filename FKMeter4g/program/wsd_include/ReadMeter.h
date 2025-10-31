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
#define UNSIGNED 				0x00 //�޷���
#define SIGNED 					0x80   //�з���
#define PT_MULTI 				0x01 //�ϴ����ݳ�PT
#define CT_MULTI 				0x02
#define PT_CT_MULTI 			0x03
#define METER_PHASE_NUM 		3 //�������
//���ʲ������
#define MAX_TIME_ZONE_NUM       5// ��ʱ��������
#define MAX_TIME_SEGTABLE_NUM   8 // ��ʱ�α�������
#define MAX_TIME_SEG_NUM       	16// ÿ����ʱ�α��ڵ�ʱ���������
#define MAX_RATIO_NUM          	8// ����������
#define TIME_STR_LEN           	6// "HH:MM" + '\0'
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef union
{
	BYTE RelayCmdData[8];//�̵�������
	BYTE SetTime[7];//����ʱ��
	BYTE TimeZoneNum; //ʱ����
	BYTE TimeSegTableNum; //ʱ�α���
	BYTE TimeSegNum; //ʱ����
	BYTE RatioNum; //������
	BYTE TTimeAreaTable[MAX_TIME_ZONE_NUM * 3]; //ʱ����
	BYTE TimeSegTable[MAX_TIME_SEG_NUM * 3]; //ʱ�α�
	BYTE TimeZoneTableToggleTime[5]; //����ʱ�����л�ʱ��
	BYTE TimeSegTableToggleTime[5]; //����ʱ�α��л�ʱ��
} uSetMeterData;//���ñ������

typedef struct
{
	e645Type Type;//��ȡ������
	WORD Extended645ID;//645��չ�ɼ���ʶ  �����ֽ�
	DWORD Standard645ID;//645��׼�ɼ���ʶ
	uSetMeterData Data;//
	BYTE Control;//������
	BYTE DataLen;//���ݳ��ȣ����������ݱ�ʶ������Ͳ����ߴ���
} TReadMeterInfo;

//���ʲ������
typedef struct {
    BYTE Time[TIME_STR_LEN];   // "HH:MM"
    BYTE TimeSegTable;      // ��Ӧ��ʱ����ָ�����ʱ�α��� (1-based)
} TTimeAreaEntry;

typedef struct {
    BYTE Hour;              // ��ѡ�����������㣬���Ǳ����ַ���Ҳ��
    BYTE Minute;
} TimeHM; // ��ѡ������洢

typedef struct {
    BYTE Time[TIME_STR_LEN];   // start time of segment, "HH:MM"
    BYTE Rate;              // ����������1..Ratio��
} TimeSegEntry;

typedef struct {
    BYTE Date;                          // �ڼ���ʱ�α� (1..TimeSegTableNum)
    BYTE SegCount;                      // ��Ч����
    TimeSegEntry Segs[MAX_TIME_SEG_NUM];   // SegCount <= MAX_TIME_SEG_NUM
} TimeSegTableItem;

typedef struct {
    BYTE TimeZoneNum;                   // ��ʱ����
    BYTE TimeSegTableNum;               // ��ʱ�α���
    BYTE TimeSegNum;                    // ÿ��ʱ��������ȫ�����ã�
    BYTE RatioNum;                         // ������
    BYTE TimeAreaCount;                 // ʵ��ʱ��������
    TTimeAreaEntry TimeAreaTable[MAX_TIME_ZONE_NUM];
    BYTE TimeSegTableCount;             // ʵ����ʱ�α�����
    TimeSegTableItem TimeSegTables[MAX_TIME_SEGTABLE_NUM];
} TRatioPara;

typedef struct tMeterRead_t
{
	DWORD OI;	//645���ݱ�ʶ
	BYTE Num;	//һ�����ݿ��м���������
	BYTE Slen;   //�����������
	BYTE Dot;	//��������С��λ
	BYTE Symbol; //�Ƿ���Ҫ�������λ bit7 �����ж� bit1 bit0 ���ֱ��
	double *Buf; //���ݴ�ŵ�ַ
} tMeterRead;

typedef enum
{
	eREAD_METER_STANDARD = 0,
	eREAD_METER_EXTENDED = 1,
	eSET_METER_STANDARD = 2,
	eSET_METER_EXTENDED = 3,
} e645Type;

typedef enum
{
	eOPEN_RELAY_LOOP1 = 0x1A,
	eCLOSE_RELAY_LOOP1 = 0x1C,
	eOPEN_RELAY_LOOP2 = 0x5A,
	eCLOSE_RELAY_LOOP2 = 0x5C,
	eOPEN_RELAY_LOOP3 = 0x9A,
	eCLOSE_RELAY_LOOP3 = 0x9C,
	// eOPEN_RELAY_ALL_LOOP = 0x0D-0x33,
	// eCLOSE_RELAY_ALL_LOOP = 0x0F-0x33,
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
	eUa = 0, // ��ѹ���ݿ�
	eUb,
	eUc,
	eIa,  // �������ݿ�
	eIb,
	eIc,
	eIzs, // �������
	eP1, // �й��������ݿ�
	eP2,
	eP3,
	eP4,
	eQ1, // �޹��������ݿ�
	eQ2,
	eQ3,
	eQ4,
	eS1, // ���ڹ������ݿ�
	eS2,
	eS3,
	eS4,
	ePF1, // �����������ݿ�
	ePF2,
	ePF3,
	ePF4,
	eComActive, //����й��ܵ���
	ePActiveAll, // �����й��ܵ���
	eNActiveAll, // �����й��ܵ���
	eRActive1All, // �޹�1�ܵ���
	eRActive2All, // �޹�2�ܵ���
	ePa_PActive1, //a�������й�
	ePa_NActive1, //a�෴���й�
	ePa_ComRActive1, //a������޹�1
	ePa_ComRActive2, //a������޹�2
	ePb_PActive1, //b�������й�
	ePb_NActive1, //b�෴���й�
	ePb_ComRActive1, //b������޹�1
	ePb_ComRActive2, //b������޹�2
	ePc_PActive1, //c�������й�
	ePc_NActive1, //c�෴���й�
	ePc_ComRActive1, //c������޹�1
	ePc_ComRActive2, //c������޹�2
	eTemp1, //�¶�1
	eTemp2, //�¶�2
	eTemp3, //�¶�3
	eTemp4, //�¶�4
	eLineUa,
	eLineUb,
	eLineUc,
	eNullType,
} eValueType;//  �޸�MeterReadOI�������Ӧ�޸�ö��ƫ�� 
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern const tMeterRead MeterReadOI[];
extern const BYTE CycleReadMeterNum;
extern BYTE gRelayStatus[METER_PHASE_NUM]; //�̵���״̬
//-----------------------------------------------
// 				��������
//-----------------------------------------------
#if (CYCLE_METER_READING == PROTOCOL_645)
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
