//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���
//��������	2021.7.27
//����		��λ�ѱ�
//�޸ļ�¼
//----------------------------------------------------------------------

#ifndef __SEARCHCOLLECTMETER_API_H
#define __SEARCHCOLLECTMETER_API_H

#if (SEL_SEARCH_METER == YES )
//--------------------------------------

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define SEARCH_METER_MAX_NUM		32			// ���32̨��
#define SEARCH_CYCLE_TIMES			10			// �����ѱ� ������
#define SEARCH_METER_POWER_TIME     6           // �ϵ��ѱ�ʱ�� s !!!����С��POWER_UP_TIMER
#define SEARCH_METER_CH             eRS485_I
#define SEARCH_METER_CH_OAD         0xF2010201  // SEARCH_METER_CH��Ӧ��OAD��ʾ
#define CAN_CH_OAD                  0xF2210201  // CANͨ����OAD��ʾ
#define FIND_UNKNOWN_METER_MAX_NUM  4           // ����δ֪���ܱ��¼��洢����������
#define SEARCH_METER_CAN_NODE_MAX_NUM   (16 + 1)
#if (SEARCH_METER_POWER_TIME>=POWER_UP_TIMER)
    #error SEARCH_METER_POWER_TIME����С��POWER_UP_TIMER
#endif
//�ѱ�̽�ⱨ��ʹ�õ�OAD
#define SEARCH_METER_698_OAD            0x40010200 // �޸Ĵ�����Ҫ�޸�gdw698M_RxMonitor()���ж��ѱ��ر��ĵĲ���
#define SEARCH_METER_654_07_OAD         0x00010000
#define SEARCH_METER_654_07_DATA_LEN    0x08
#define SEARCH_METER_654_97_OAD         0x9010
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
#pragma pack(1)
typedef struct
{
	BYTE AA[6];
	BYTE byAutoCheck; //BIT0-698��Լʶ��BIT1--07��Լʶ��BIT2--97��Լʶ��
	BYTE byAutoLevel;
	BYTE byAutoAck;
	BYTE byAutoBaud;  //BIT0 -1200 BIT1-2400 BIT2-4800 BIT3-9600 BIT4-115200
	BYTE byAATry;     //ȫAAͨ���ʶ��
	BYTE byAATryNum;
	BYTE byBaud;
	BYTE byGyType;
	BYTE byFirst;	  //��ʼ���л������ʻ��л���Լ����ĵ�һ֡
	BYTE byRxOK;	  //�յ�����
	BYTE byComplete;  //��֤���
    WORD CheckStep;   //bit15Ϊ0��ʾ��֤�ѱ�����Ϊ1��ʾ��֤����,0x00FF��ʾ��һ����֤
    WORD dwRxCnt;     //�յ����ֽ���
    DWORD dwSearchMaxTime;   //�����ѱ����ʱ�� s
    DWORD dwSearchTimeCnt;   //�����ѱ�ʱ�� s
} TSKMeter;

typedef struct
{
    BYTE Time[3]; //ʱ�� ʱ����
    WORD Duration;//�ѱ�ʱ��
}SearchMeterCycle_t;
//�����ѱ���� + �õ��쳣������
typedef struct TCycleSInfoRom_t
{
	BYTE CycleSearchFlag;       // ÿ�������ѱ����ñ�־
    BYTE AutoUpdateArchives;    // �Զ����²ɼ�����
    BYTE SearchMeterEventFlag;  // �ѱ��¼����ɱ�־ 
    BYTE ClearResultOption;     // ����ѱ���ѡ��,0����գ�1ÿ�������ѱ�ǰ��գ�2ÿ���ѱ�ǰ���
    BYTE CycleNum;              // �ѱ�������
	SearchMeterCycle_t Cycle[SEARCH_CYCLE_TIMES]; // �ѱ����� 
    BYTE SearchMode;            // �ѱ�ģʽ{�����ѱ�0������װ���ɣ�1��}
	BYTE IsPowerUpSearch;       // �ϵ��Ƿ������ѱ� 
    WORD PowerUpSearchTime;     // �ϵ��ѱ�ʱ����min��
	DWORD CRC32;
} TCycleSInfoRom;

typedef struct TCycleSearchRom_t
{
	TCycleSInfoRom CycleSInfoRom; // �����ѱ����
} TCycleSearchRom;

// �ѵ��ı����Ϣ
typedef struct TSearchMeterInfo_t
{
	BYTE Addr[6];
	BYTE byProtocolType;
	BYTE byBaud;
    DWORD ChOad;
    TRealTimer Time;
} TSchedMeterInfo;

typedef struct
{
    BYTE  MeterNum;   //  ���ֵ�δ֪���ܱ�����
    TSchedMeterInfo SearchResult[FIND_UNKNOWN_METER_MAX_NUM];
}FindUnknownMeterEventData_t;
#pragma pack()


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern TSKMeter SKMeter;
extern TSchedMeterInfo tSearchedMeter[SEARCH_METER_MAX_NUM];
#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
extern FindUnknownMeterEventData_t  gFindUnknownMeterEventDataRAM;
extern BOOL FindUnknownMeterEventFlag;
#endif
extern TCycleSInfoRom tCycleSInfoRom;
extern const TCycleSInfoRom CycleSInfoRomConst;
//-----------------------------------------------
//				��������
//-----------------------------------------------
void CheckCycleSInfo(void);
void api_StartSearchMeter(WORD SearchMaxtime);
void api_StopSchMeter(void);
void api_PowerUpSchClctMeter(void);
void api_PowerDownSchClctMeter(void);
void api_FindUnknownMeterEventPara(void);
void api_ClearSchMeter(void);
void api_InitSchClctMeter(void);
BYTE api_GetMeterNumInSys(void);
BYTE api_Addr2MeterNo(BYTE *pAddr, BYTE byPro, BYTE byBaudRate,DWORD ChOad,BOOL bAutoLearn);
void api_SearchMeterSecondTask(void);
BOOL api_ProSearchMeter(BYTE SerialNo);
WORD api_GetSchedMeter698(BYTE ClassIndex,BYTE *pBuf ,BYTE* bNum);
WORD api_GetSchedMeterPara698(BYTE ClassIndex,BYTE *pData);
#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
BOOL api_GetFindUnknownMeterEventStatus(BYTE Phase);
WORD api_GetFindUnknownMeter(FindUnknownMeterEventData_t* InBuf,BYTE* OutBuf);
#endif
WORD api_GetSchedMeterStrategy698(BYTE ClassIndex,BYTE *pData);
WORD api_GetSchedMeterCycle698(BYTE ClassIndex,BYTE *pBuf,BYTE* bNum);
BYTE api_SetSchedMeterPara698(BYTE ClassIndex,BYTE *pData);
BYTE api_SetSchedMeterStrategy698(BYTE ClassIndex,BYTE *pData);
BYTE api_SetSchedMeterCycle698(BYTE ClassIndex,BYTE *pData);
BYTE api_SetSchedMeter698(BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData);

#endif //#if (SEL_SEARCH_METER == YES )
#endif // __SEARCHCOLLECTMETER_API_H