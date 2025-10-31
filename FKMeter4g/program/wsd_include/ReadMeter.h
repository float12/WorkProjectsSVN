//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		READMETER头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __READMETER_H
#define __READMETER_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define UNSIGNED 				0x00 //无符号
#define SIGNED 					0x80   //有符号
#define PT_MULTI 				0x01 //上传数据乘PT
#define CT_MULTI 				0x02
#define PT_CT_MULTI 			0x03
#define METER_PHASE_NUM 		3 //电表相数
//费率参数相关
#define MAX_TIME_ZONE_NUM       5// 年时区数上限
#define MAX_TIME_SEGTABLE_NUM   8 // 日时段表数上限
#define MAX_TIME_SEG_NUM       	16// 每个日时段表内的时间段数上限
#define MAX_RATIO_NUM          	8// 费率数上限
#define TIME_STR_LEN           	6// "HH:MM" + '\0'
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef union
{
	BYTE RelayCmdData[8];//继电器命令
	BYTE SetTime[7];//设置时间
	BYTE TimeZoneNum; //时区数
	BYTE TimeSegTableNum; //时段表数
	BYTE TimeSegNum; //时段数
	BYTE RatioNum; //费率数
	BYTE TTimeAreaTable[MAX_TIME_ZONE_NUM * 3]; //时区表
	BYTE TimeSegTable[MAX_TIME_SEG_NUM * 3]; //时段表
	BYTE TimeZoneTableToggleTime[5]; //两套时区表切换时间
	BYTE TimeSegTableToggleTime[5]; //两套时段表切换时间
} uSetMeterData;//设置表的数据

typedef struct
{
	e645Type Type;//读取或设置
	WORD Extended645ID;//645扩展采集标识  低两字节
	DWORD Standard645ID;//645标准采集标识
	uSetMeterData Data;//
	BYTE Control;//控制字
	BYTE DataLen;//数据长度，不包括数据标识、密码和操作者代码
} TReadMeterInfo;

//费率参数相关
typedef struct {
    BYTE Time[TIME_STR_LEN];   // "HH:MM"
    BYTE TimeSegTable;      // 对应年时区所指向的日时段表编号 (1-based)
} TTimeAreaEntry;

typedef struct {
    BYTE Hour;              // 可选，更方便运算，还是保留字符串也行
    BYTE Minute;
} TimeHM; // 可选解析后存储

typedef struct {
    BYTE Time[TIME_STR_LEN];   // start time of segment, "HH:MM"
    BYTE Rate;              // 费率索引（1..Ratio）
} TimeSegEntry;

typedef struct {
    BYTE Date;                          // 第几日时段表 (1..TimeSegTableNum)
    BYTE SegCount;                      // 有效段数
    TimeSegEntry Segs[MAX_TIME_SEG_NUM];   // SegCount <= MAX_TIME_SEG_NUM
} TimeSegTableItem;

typedef struct {
    BYTE TimeZoneNum;                   // 年时区数
    BYTE TimeSegTableNum;               // 日时段表数
    BYTE TimeSegNum;                    // 每日时段数（或全局配置）
    BYTE RatioNum;                         // 费率数
    BYTE TimeAreaCount;                 // 实际时区表项数
    TTimeAreaEntry TimeAreaTable[MAX_TIME_ZONE_NUM];
    BYTE TimeSegTableCount;             // 实际日时段表项数
    TimeSegTableItem TimeSegTables[MAX_TIME_SEGTABLE_NUM];
} TRatioPara;

typedef struct tMeterRead_t
{
	DWORD OI;	//645数据标识
	BYTE Num;	//一个数据块有几个数据项
	BYTE Slen;   //单个数据项长度
	BYTE Dot;	//接收数据小数位
	BYTE Symbol; //是否需要处理符号位 bit7 符号判断 bit1 bit0 区分变比
	double *Buf; //数据存放地址
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
	eUa = 0, // 电压数据块
	eUb,
	eUc,
	eIa,  // 电流数据块
	eIb,
	eIc,
	eIzs, // 零序电流
	eP1, // 有功功率数据块
	eP2,
	eP3,
	eP4,
	eQ1, // 无功功率数据块
	eQ2,
	eQ3,
	eQ4,
	eS1, // 视在功率数据块
	eS2,
	eS3,
	eS4,
	ePF1, // 功率因数数据块
	ePF2,
	ePF3,
	ePF4,
	eComActive, //组合有功总电能
	ePActiveAll, // 正向有功总电能
	eNActiveAll, // 反向有功总电能
	eRActive1All, // 无功1总电能
	eRActive2All, // 无功2总电能
	ePa_PActive1, //a相正向有功
	ePa_NActive1, //a相反向有功
	ePa_ComRActive1, //a相组合无功1
	ePa_ComRActive2, //a相组合无功2
	ePb_PActive1, //b相正向有功
	ePb_NActive1, //b相反向有功
	ePb_ComRActive1, //b相组合无功1
	ePb_ComRActive2, //b相组合无功2
	ePc_PActive1, //c相正向有功
	ePc_NActive1, //c相反向有功
	ePc_ComRActive1, //c相组合无功1
	ePc_ComRActive2, //c相组合无功2
	eTemp1, //温度1
	eTemp2, //温度2
	eTemp3, //温度3
	eTemp4, //温度4
	eLineUa,
	eLineUb,
	eLineUc,
	eNullType,
} eValueType;//  修改MeterReadOI抄表项，对应修改枚举偏移 
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern const tMeterRead MeterReadOI[];
extern const BYTE CycleReadMeterNum;
extern BYTE gRelayStatus[METER_PHASE_NUM]; //继电器状态
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
#if (CYCLE_METER_READING == PROTOCOL_645)
//--------------------------------------------------
//功能描述:  645 连续抄读
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Dlt645_Tx_ContinueRead( BYTE bStep, BYTE *bBuf);
//-----------------------------------------------
//功能描述:  抄表数据解析并保存
//
//参数:      pBuf[in]  dataBuf[in] 数据保存位置
//
//返回值:
//
//备注:
//--------------------------------------------------
void DataAnalyseandSave(BYTE *pBuf, double *dataBuf, BYTE Len, tMeterRead tMeterReadOIRam);
//--------------------------------------------------
//功能描述: 计量数据传送给不同上报组帧任务
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void SampleDatatoReport(TRealTimer *pTime);
#endif
#endif //#ifndef __READMETER_H
