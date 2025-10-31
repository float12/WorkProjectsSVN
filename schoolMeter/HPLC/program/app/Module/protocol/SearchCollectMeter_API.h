//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���
//��������	2021.7.27
//����		��λ�ѱ�
//�޸ļ�¼
//----------------------------------------------------------------------

#ifndef __SEARCHCOLLECTMETER_API_H
#define __SEARCHCOLLECTMETER_API_H
//--------------------------------------

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define SEARCH_DI					0x00010000
#define COLLECT_HOUR_FRZ_DI			0x05040101	// ��һ�����㶳��
#define SEARCH_METER_MAX_NUM		1			// ���1̨��  ��ӦGDF415
#define COLLECT_METER_RETRY_TIMES	2			// �ɱ����Դ���
#define SEARCH_CYCLE_TIMES			10			// �����ѱ� ������
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
#pragma pack(1)
typedef struct
{
	BYTE AA[6];
	BYTE byAutoCheck; //BIT0-07��Լʶ��BIT1--698��Լʶ��BIT2--97��Լʶ��
	BYTE byAutoLevel;
	BYTE byAutoAck;
	BYTE byAutoBaud;  //BIT0 -1200 BIT1-2400 BIT2-4800 BIT3-9600 BIT4-115200
	BYTE byAATry;     //ȫAAͨ���ʶ��
	BYTE byAATryNum;
	DWORD dwBaud;
	BYTE byGyType;
	BYTE byFirst;	  //��ʼ���л������ʻ��л���Լ����ĵ�һ֡
	BYTE byLastNo;	  //��һ������֤��Flash�д��ڵı����
	BYTE byRxOK;	  //�յ�����
	BYTE byComplete;  //��֤���
	BOOL bSKComplete; //�ѱ����
} TSKMeter;

// �ѱ���� ��ȫ�ռ�
typedef struct TMeterInfoRom_t
{
	DWORD dwRecordNo;
	DWORD CRC32;
} TMeterInfoRom;

typedef struct TSearchMeterMem_t
{
	TMeterInfoRom MeterInfoRom;
} TSearchMeterRom;

//�����ѱ���� + �õ��쳣������
typedef struct TCycleSInfoRom_t
{
	BYTE    CllType;                        //�ɼ���ģʽ
	BYTE 	SearchType; 					//�����ѱ���
	BYTE	Time[SEARCH_CYCLE_TIMES][3]; 	//�ѱ����� ʱ�� ʱ����
	//BYTE 	byCtrl; 						//�õ��쳣������  bit0�Ե磻bit1����
	DWORD 	CRC32;
}TCycleSInfoRom;

typedef struct TCycleSearchRom_t
{
	TCycleSInfoRom CycleSInfoRom; //�����ѱ����
}TCycleSearchRom;

// ������� ��ȫ�ռ�
typedef struct TClctTypeRom_t
{
	DWORD dwType; // �������ͣ����㶳�ᣬ�ն���
	DWORD CRC32;
} TClctTypeRom;

typedef struct TClctMeterMem_t
{
	TClctTypeRom ClctTypeRom;
} TClctMeterRom;

// �ѵ��ı����Ϣ
typedef struct TSearchMeterInfo_t
{
	BYTE Addr[6];
	WORD wProtocolType;
	DWORD dwBaud;
} TSchedMeterInfo;
#pragma pack()
// typedef struct TSClctMeterInfo_t
// {
// 	BYTE Addr[6];
// 	WORD wProtocolType;
// 	WORD wBaud;
// } TSchedMeterInfo;

typedef enum
{
	ePROTOCOL_645_97 = 1,
	ePROTOCOL_645_07,
	ePROTOCOL_698
} ePROTOCOL_CLASS;

typedef enum
{
	eHourFreeze = 0,
	eDayFreeze,
	// eMinFreeze,
	eMaxFreeze
} eCLCT_METER_INFO_TYPE;

typedef struct TCollectMeterInfo_t
{
	WORD RecordNo;
	WORD Retry;
	WORD Flag;
	WORD RxCounter; // !!!!!! test
	WORD Type;		// eCLCT_METER_INFO_TYPE
} TCollectMeterInfo;

//typedef enum
//{
//	eVolTime = 0,	// �����ѹ��ʱ��
//	eAjust,
//	eInit
//} eF460SpiCommCmd;
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern TSKMeter SKMeter;
// extern TSchedMeterInfo tSearchedMeter[SEARCH_METER_MAX_NUM];
extern TCycleSInfoRom tCycleSInfoRom;
extern const TCycleSInfoRom CycleSInfoRomConst;
//-----------------------------------------------
//				��������
//-----------------------------------------------
// void api_StartSearchMeter(void);
// BOOL api_ProSearchMeter(BYTE SerialNo);
// void api_PowerUpSchClctMeter(void);
// void api_SaveSchedMeterInfo(void);
BOOL api_SaveSchedMeter(/*TSchedMeterInfo *ptMeterInfo*/BYTE byMetIndex);
void api_ClrClctMeterRetry(void);
WORD api_GetSchedMeter(ePROTOCOL_CLASS ePClass, BYTE *pBuf);
WORD api_GetSchedMeter698( BYTE *pBuf ,BYTE* bNum);
void api_CommuTx( BYTE SerialNo );
// BYTE api_Addr2MeterNo(BYTE *pAddr, BYTE byPro, DWORD wBaudRate,BOOL bAutoLearn);
BYTE SwitchToNext(BYTE byStartNo, TSchedMeterInfo *pMetInfo);
void api_InitSchClctMeter(void);
// void api_StopSchMeter(void);
void api_ClearSchMeter(void);
BOOL IsExistMeter(BYTE *pAddr,DWORD *pBaud);
BOOL IsWorkInCllMode(void);
BOOL IsBaudValid(DWORD dwBaud);
// WORD api_AddNewMeter( BYTE* pBuf );
BYTE api_GetMeterNumInSys(void);
// WORD  api_DelNewMeter( BYTE* pbuf );
#endif // __SEARCHCOLLECTMETER_API_H