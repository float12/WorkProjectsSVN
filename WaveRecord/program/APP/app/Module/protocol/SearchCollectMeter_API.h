//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	刘骞
//创建日期	2021.7.27
//描述		缩位搜表
//修改记录
//----------------------------------------------------------------------

#ifndef __SEARCHCOLLECTMETER_API_H
#define __SEARCHCOLLECTMETER_API_H

#if (SEL_SEARCH_METER == YES )
//--------------------------------------

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define SEARCH_METER_MAX_NUM		32			// 最多32台表
#define SEARCH_CYCLE_TIMES			10			// 周期搜表 周期数
#define SEARCH_METER_POWER_TIME     6           // 上电搜表时间 s !!!必须小于POWER_UP_TIMER
#define SEARCH_METER_CH             eRS485_I
#define SEARCH_METER_CH_OAD         0xF2010201  // SEARCH_METER_CH对应的OAD表示
#define CAN_CH_OAD                  0xF2210201  // CAN通道的OAD表示
#define FIND_UNKNOWN_METER_MAX_NUM  4           // 发现未知电能表事件存储的最大表结果数
#define SEARCH_METER_CAN_NODE_MAX_NUM   (16 + 1)
#if (SEARCH_METER_POWER_TIME>=POWER_UP_TIMER)
    #error SEARCH_METER_POWER_TIME必须小于POWER_UP_TIMER
#endif
//搜表探测报文使用的OAD
#define SEARCH_METER_698_OAD            0x40010200 // 修改此项需要修改gdw698M_RxMonitor()中判断搜表返回报文的部分
#define SEARCH_METER_654_07_OAD         0x00010000
#define SEARCH_METER_654_07_DATA_LEN    0x08
#define SEARCH_METER_654_97_OAD         0x9010
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)
typedef struct
{
	BYTE AA[6];
	BYTE byAutoCheck; //BIT0-698规约识别，BIT1--07规约识别，BIT2--97规约识别
	BYTE byAutoLevel;
	BYTE byAutoAck;
	BYTE byAutoBaud;  //BIT0 -1200 BIT1-2400 BIT2-4800 BIT3-9600 BIT4-115200
	BYTE byAATry;     //全AA通配符识别
	BYTE byAATryNum;
	BYTE byBaud;
	BYTE byGyType;
	BYTE byFirst;	  //开始或切换波特率或切换规约抄表的第一帧
	BYTE byRxOK;	  //收到报文
	BYTE byComplete;  //验证完成
    WORD CheckStep;   //bit15为0表示验证搜表结果，为1表示验证档案,0x00FF表示第一次验证
    WORD dwRxCnt;     //收到的字节数
    DWORD dwSearchMaxTime;   //本次搜表最大时长 s
    DWORD dwSearchTimeCnt;   //本次搜表时间 s
} TSKMeter;

typedef struct
{
    BYTE Time[3]; //时间 时分秒
    WORD Duration;//搜表时长
}SearchMeterCycle_t;
//周期搜表参数 + 用电异常控制字
typedef struct TCycleSInfoRom_t
{
	BYTE CycleSearchFlag;       // 每天周期搜表启用标志
    BYTE AutoUpdateArchives;    // 自动更新采集档案
    BYTE SearchMeterEventFlag;  // 搜表事件生成标志 
    BYTE ClearResultOption;     // 清空搜表结果选项,0不清空，1每天周期搜表前清空，2每次搜表前清空
    BYTE CycleNum;              // 搜表周期数
	SearchMeterCycle_t Cycle[SEARCH_CYCLE_TIMES]; // 搜表周期 
    BYTE SearchMode;            // 搜表模式{周期搜表（0），即装即采（1）}
	BYTE IsPowerUpSearch;       // 上电是否启动搜表 
    WORD PowerUpSearchTime;     // 上电搜表时长（min）
	DWORD CRC32;
} TCycleSInfoRom;

typedef struct TCycleSearchRom_t
{
	TCycleSInfoRom CycleSInfoRom; // 周期搜表参数
} TCycleSearchRom;

// 搜到的表的信息
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
    BYTE  MeterNum;   //  发现的未知电能表数量
    TSchedMeterInfo SearchResult[FIND_UNKNOWN_METER_MAX_NUM];
}FindUnknownMeterEventData_t;
#pragma pack()


//-----------------------------------------------
//				全局使用的变量，常量
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
//				函数定义
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