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
#define UNSIGNED 		0x00 //无符号
#define SIGNED 			0x80   //有符号
#define PT_MULTI 		0x01 //上传数据乘PT
#define CT_MULTI 		0x02
#define PT_CT_MULTI 	0x03
#define METER_PHASE_NUM 3 //电表相数
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eREAD_METER_STANDARD = 0,
	eREAD_METER_EXTENDED = 1,
	eSET_METER_STANDARD = 2,
	eSET_METER_EXTENDED = 3,
	eREAD_METER_FREEZE,
} eReadType;
typedef union
{
	BYTE RelayCmdData[8];//继电器命令
	BYTE SetorGetTime[7];//设置时间或读取冻结时间
} uSetMeterData;//设置表的数据

typedef struct
{
	eReadType Type;//645读取/设置或698抄读
	WORD Extended645ID;//645扩展采集标识  低两字节
	DWORD Standard645ID;//645标准采集标识
	uSetMeterData Data;//
	BYTE Control;//控制字
	BYTE DataLen;//数据长度，不包括数据标识、密码和操作者代码
} TReadMeterInfo;
#pragma pack(1)
typedef struct
{
	BYTE Time[6];//继电器变化时间
	BYTE Energe[6][4];//电能
	DWORD Reason;//继电器变化原因
} TRelayChangeReason;

typedef struct tMeterRead_t
{
	BYTE IsExtendOI;//是否是扩展标识
	BYTE IsDoubleData;//是否是double数据
	DWORD OI;	//645数据标识
	BYTE Num;	//一个数据块有几个数据项
	BYTE Slen;   //单个数据项长度
	BYTE Dot;	//接收数据小数位
	BYTE Symbol; //是否需要处理符号位 bit7 符号判断 bit1 bit0 区分变比
	void *Buf; //数据存放地址
} tMeterRead;


typedef enum
{
	eOPEN_RELAY_LOOP1 = 0x1A,
	eCLOSE_RELAY_LOOP1 = 0x1C,
	eOPEN_RELAY_LOOP2 = 0x5A,
	eCLOSE_RELAY_LOOP2 = 0x5C,
	eOPEN_RELAY_LOOP3 = 0x9A,
	eCLOSE_RELAY_LOOP3 = 0x9C,
	eOPEN_RELAY_ALL_LOOP = 0xDA,
	eCLOSE_RELAY_ALL_LOOP = 0xDC,
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
	eFreq = 0,        // 频率
	eUa,              // a相电压
	eUb,              // b相电压
	eUc,              // c相电压
	eIa,              // a相电流
	eIb,              // b相电流
	eIc,              // c相电流
	eActPAll,         // 总有功功率
	eActPa,           // a相正向有功
	eActPb,           // b相正向有功
	eActPc,           // c相正向有功
	eReactPAll,       // 总无功功率
	eReactPa,         // a相无功
	eReactPb,         // b相无功
	eReactPc,         // c相无功
	eAppPAll,         // 总视在功率
	eAppPa,           // a相视在功率
	eAppPb,           // b相视在功率
	eAppPc,           // c相视在功率
	ePFAll,           // 总功率因数
	ePFa,             // a相功率因数
	ePFb,             // b相功率因数
	ePFc,             // c相功率因数
	eTotalFwdActEp,   // 总正向有功电能
	eFwdActEpa,       // a相正向有功电能
	eFwdActEpb,       // b相正向有功电能
	eFwdActEpc,       // c相正向有功电能
	eTotalRevActEp,   // 总反向有功电能
	eRevActEpa,       // a相反向有功电能
	eRevActEpb,       // b相反向有功电能
	eRevActEpc,        // c相反向有功电能
	eLoop1RelayStatus,
	eLoop1RelayChangeTime,
	eLoop1RelayChangeReason,
	eLoop2RelayStatus,
	eLoop2RelayChangeTime,
	eLoop2RelayChangeReason,
	eLoop3RelayStatus,
	eLoop3RelayChangeTime,
	eLoop3RelayChangeReason,
} eValueType; //  修改MeterReadOI抄表项，对应修改枚举偏移 
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern const tMeterRead MeterReadOI[];
extern const BYTE CycleReadMeterNum;
extern BYTE gRelayStatus[METER_PHASE_NUM]; //继电器状态
extern TRelayChangeReason RelayChangeReason[METER_PHASE_NUM][2]; //继电器变化原因
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
#if (CYCLE_METER_READING == YES)
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
