//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	刘骞
//创建日期	2021.7.27
//描述		缩位搜表
//修改记录
//----------------------------------------------------------------------

#ifndef __SEARCHCOLLECTMETER_API_H
#define __SEARCHCOLLECTMETER_API_H
//--------------------------------------

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define SEARCH_DI					0x00010000
#define COLLECT_HOUR_FRZ_DI			0x05040101	// 上一次整点冻结
#define SEARCH_METER_MAX_NUM		1			// 最多1台表  对应GDF415
#define COLLECT_METER_RETRY_TIMES	2			// 采表重试次数
#define SEARCH_CYCLE_TIMES			10			// 周期搜表 周期数
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)
typedef struct
{
	BYTE AA[6];
	BYTE byAutoCheck; //BIT0-07规约识别，BIT1--698规约识别，BIT2--97规约识别
	BYTE byAutoLevel;
	BYTE byAutoAck;
	BYTE byAutoBaud;  //BIT0 -1200 BIT1-2400 BIT2-4800 BIT3-9600 BIT4-115200
	BYTE byAATry;     //全AA通配符识别
	BYTE byAATryNum;
	DWORD dwBaud;
	BYTE byGyType;
	BYTE byFirst;	  //开始或切换波特率或切换规约抄表的第一帧
	BYTE byLastNo;	  //上一个被验证的Flash中存在的表序号
	BYTE byRxOK;	  //收到报文
	BYTE byComplete;  //验证完成
	BOOL bSKComplete; //搜表完成
} TSKMeter;

// 搜表相关 安全空间
typedef struct TMeterInfoRom_t
{
	DWORD dwRecordNo;
	DWORD CRC32;
} TMeterInfoRom;

typedef struct TSearchMeterMem_t
{
	TMeterInfoRom MeterInfoRom;
} TSearchMeterRom;

//周期搜表参数 + 用电异常控制字
typedef struct TCycleSInfoRom_t
{
	BYTE    CllType;                        //采集器模式
	BYTE 	SearchType; 					//周期搜表开关
	BYTE	Time[SEARCH_CYCLE_TIMES][3]; 	//搜表周期 时间 时分秒
	//BYTE 	byCtrl; 						//用电异常控制字  bit0窃电；bit1超差
	DWORD 	CRC32;
}TCycleSInfoRom;

typedef struct TCycleSearchRom_t
{
	TCycleSInfoRom CycleSInfoRom; //周期搜表参数
}TCycleSearchRom;

// 抄表相关 安全空间
typedef struct TClctTypeRom_t
{
	DWORD dwType; // 抄表类型，整点冻结，日冻结
	DWORD CRC32;
} TClctTypeRom;

typedef struct TClctMeterMem_t
{
	TClctTypeRom ClctTypeRom;
} TClctMeterRom;

// 搜到的表的信息
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
//	eVolTime = 0,	// 传输电压和时间
//	eAjust,
//	eInit
//} eF460SpiCommCmd;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern TSKMeter SKMeter;
// extern TSchedMeterInfo tSearchedMeter[SEARCH_METER_MAX_NUM];
extern TCycleSInfoRom tCycleSInfoRom;
extern const TCycleSInfoRom CycleSInfoRomConst;
//-----------------------------------------------
//				函数定义
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