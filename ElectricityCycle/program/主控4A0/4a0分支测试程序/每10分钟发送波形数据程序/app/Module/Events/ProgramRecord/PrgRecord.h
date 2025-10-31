//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.11.1
//描述		事件模块内部头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __PRGRECORD_H
#define __PRGRECORD_H

#include "event.h"

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAX_PRG_CACHE_LEN  100//编程缓存数据最大长度

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//645编程记录
#if(SEL_PRG_RECORD645 == YES)
typedef struct TPrgProgramRom645_t
{
	BYTE		Time[6];    //编程记录发生时间
	BYTE		Op[4];      //操作者代码
	BYTE		DI[40];		//最近10次数据标识
}TPrgProgramRom645; 
#endif




typedef struct TPrgInfoProgramData_t
{
	#if( MAX_PHASE == 1)
	BYTE	Reserved[8];		// 预留
	#else
	BYTE	Reserved[16];		// 预留
	#endif
}TPrgInfoProgramData;

typedef struct TPrgProgramSubRom_t
{
	DWORD					Oad[MAX_PROGRAM_OAD_NUMBER];			// OAD array 最大为MAX_PROGRAM_OAD_NUMBER个
	BYTE					Type[MAX_PROGRAM_OAD_NUMBER];			// OAD 或 OMD 标识
	BYTE					SaveOadNum;								// 每次编程记录里存编程OAD的个数
}TPrgProgramSubRom;

// 编程记录
typedef struct TPrgProgramRom_t
{
	TEventOADCommonData		FixData;								// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoProgramData)];		// 各个OAD对应的数据
	TPrgProgramSubRom		OadList;
}TPrgProgramRom;


// 电表清零最大存储记录
#if( SEL_PRG_INFO_CLEAR_METER == YES )
typedef struct TPrgInfoClearEnergyData_t
{
	#if( MAX_PHASE == 1)
	DWORD	Energy[2]; 			// 总 正反有功
	BYTE	Reserved[4];		// 预留
	#else
	// 电表清零前的电能数据
	// 第一层：总、A、B、C
	// 第二层：正、反有功、四象限无功
	DWORD	Energy[4][6];
	BYTE	Reserved[8];		// 预留
	#endif
}TPrgInfoClearEnergyData;

typedef struct TPrgClearMeterRom_t
{
	TEventOADCommonData		FixData;	// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoClearEnergyData)];
}TPrgClearMeterRom;
#endif


// 清需量记录附属数据结构
#if( SEL_PRG_INFO_CLEAR_MD == YES )
typedef struct TPrgInfoClearMDData_t
{
	// 清需量前数据
	// 第一层：正、反有功、四象限无功
	// 第二层：最大需量发生时间7+最大需量4
	BYTE	MDData[6][11];
	BYTE	Reserved[20];		// 预留
}TPrgInfoClearMDData;

typedef struct TPrgClearMDRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoClearMDData)];
}TPrgClearMDRom;
#endif


// 事件清零记录要记录事件清零数据标识码(1个DWORD)
#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
typedef struct TPrgInfoClearEventData_t
{
	BYTE	Reserved[8];		// 预留
}TPrgInfoClearEventData;

typedef struct TPrgClearEventRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据	
	BYTE					Data[sizeof(TPrgInfoClearEventData)];
	DWORD					Omd;	//事件清零数据标识码
}TPrgClearEventRom;
#endif


// 校时记录数据
#if( SEL_PRG_INFO_ADJUST_TIME == YES )	
typedef struct TPrgInfoAdjustTimeData_t
{
	TEventTime	AdjustTime;
	BYTE		Reserved[8];		// 预留
}TPrgInfoAdjustTimeData;

typedef struct TPrgAdjustTimeRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoAdjustTimeData)];
}TPrgAdjustTimeRom;
#endif


// 时段表编程数据
#if( SEL_PRG_INFO_TIME_TABLE == YES )
typedef struct TPrgInfoSegTable_t
{
	BYTE	TimeSeg[MAX_TIME_SEG][3];
	BYTE	Reserved[10];		// 预留
}TPrgInfoSegTable;

typedef struct TPrgSegTableRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoSegTable)];
	BYTE					TimeSegChg[MAX_TIME_SEG][3];// 编程前时段表
	DWORD					OAD;// 时段表编程OAD
}TPrgSegTableRom;
#endif


// 时区表编程数据
#if( SEL_PRG_INFO_TIME_AREA == YES )
typedef struct TPrgInfoTimeArea_t
{
	BYTE	TimeArea[MAX_TIME_AREA][3];
	//BYTE	TimeAreaBackUp[MAX_TIME_AREA][3];
	BYTE	Reserved[4];		// 预留
}TPrgInfoTimeArea;

typedef struct TPrgTimeAreaRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoTimeArea)];
}TPrgTimeAreaRom;
#endif


//电能表周休日编程事件
#if( SEL_PRG_INFO_WEEK == YES )	
typedef struct TPrgInfoWeek_t
{
	BYTE	TableNo;
	BYTE	Reserved[4];		// 预留
}TPrgInfoWeek;

typedef struct TPrgWeekRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoWeek)];
}TPrgWeekRom;
#endif


// 结算日编程记录
#if( SEL_PRG_INFO_CLOSING_DAY == YES )
typedef struct TPrgInfoClosingPara_t
{
	TMonSavePara	MonSavePara[MAX_MON_CLOSING_NUM];
	BYTE			Reserved[4];		// 预留
}TPrgInfoClosingPara;

typedef struct TPrgClosingRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoClosingPara)];
}TPrgClosingRom;
#endif


// 拉闸记录
typedef struct TOpenRelayRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TEventNormal)];
}TPrgOpenRelayRom;
// 合闸记录
typedef struct TPrgCloseRelayRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TEventNormal)];
}TPrgCloseRelayRom;


// 节假日编程记录
#if( SEL_PRG_INFO_HOLIDAY == YES )	
typedef struct TPrgInfoHoliday_t
{
	#ifndef MAX_HOLIDAY
	"宏定义错误！"
	#endif
	#if( MAX_HOLIDAY == 0 )
	BYTE byNoUse;
	#else
	// 第一层：254个节假日
	// 第二层：节假日编程
	BYTE Holiday[MAX_HOLIDAY][4];
	#endif
	BYTE	Reserved[10];		// 预留
}TPrgInfoHoliday;

typedef struct TPrgHolidayRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgInfoHoliday)];
 	BYTE					Holiday[6];// 和GetOADLen获取的长度保持一致 
 	DWORD					OAD;// 节假日编程OAD
}TPrgHolidayRom;
#endif


// 有功组合方式编程记录
#if( SEL_PRG_INFO_P_COMBO == YES )	
typedef struct TPrgPcombDataRom_t
{
	BYTE	Data;				//有功组合模式字
	BYTE	Reserved[4];		// 预留
}TPrgPcombDataRom;

typedef struct TPrgPcombRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgPcombDataRom)];
}TPrgPcombRom;	
#endif


// 无功组合方式编程记录
#if( SEL_PRG_INFO_Q_COMBO == YES )	
typedef struct TPrgQcombDataRom_t
{
	BYTE	Data1;				//无功组合模式字1
	BYTE	Data2;				//无功组合模式字2
	BYTE	Reserved[4];		// 预留
}TPrgQcombDataRom;

typedef struct TPrgQcombRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgQcombDataRom)];
}TPrgQcombRom;		
#endif


//电能表密钥更新事件
#if( SEL_PRG_UPDATE_KEY == YES )
typedef struct TPrgUpdateKeyDataRom_t
{
	BYTE	TotalNum;			//密钥总条数
	BYTE	KeyVersion[16];			//密钥版本
	BYTE	Reserved[4];		// 预留
}TPrgUpdateKeyDataRom;
	
typedef struct TPrgUpdateKeyRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgUpdateKeyDataRom)];
}TPrgUpdateKeyRom;
#endif

#if(PREPAY_MODE == PREPAY_LOCAL)
//电能表费率参数表编程事件
typedef struct TPrgTariffDataRom_t
{
	BYTE	Data[12*4*2];		//编程前当前套备用套费率电价表
}TPrgTariffDataRom;
typedef struct TPrgTariffRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgTariffDataRom)];
}TPrgTariffRom;	
//电能表阶梯表编程事件
typedef struct TPrgLadderDataRom_t
{
	BYTE	Data[80*2];		//编程前当前套备用套阶梯电价表
}TPrgLadderDataRom;
typedef struct TPrgLadderRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgLadderDataRom)];
}TPrgLadderRom;	
//电能表异常插卡事件
typedef struct TPrgAbrCardDataRom_t
{
	BYTE	Data[70];		//异常插卡事件记录单元
}TPrgAbrCardDataRom;
typedef struct TPrgAbrCardRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgAbrCardDataRom)];
}TPrgAbrCardRom;
//电能表购电记录
typedef struct TPrgBuyMoneyDataRom_t
{
	BYTE	Data[70];		//电能表购电事件记录单元
}TPrgBuyMoneyDataRom;
typedef struct TPrgBuyMoneyRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgBuyMoneyDataRom)];
}TPrgBuyMoneyRom;
//电能表退费记录
typedef struct TPrgReturnMoneyDataRom_t
{
	BYTE	Data[70];		//退费事件记录单元
}TPrgReturnMoneyDataRom;
typedef struct TPrgReturnMoneyRom_t
{
	TEventOADCommonData		FixData;// 每个编程类都有的共同数据
	BYTE					Data[sizeof(TPrgReturnMoneyDataRom)];
}TPrgReturnMoneyRom;
#endif

typedef struct TPreProgramData_t
{	
	BYTE	Data[MAX_PRG_CACHE_LEN];			//编程前OAD及对应的数据	
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )	
	//只有无功组合模式字有事件发生源，所以放这了。
	BYTE	QcombSource[MAX_EVENTRECORD_NUMBER];	//无功组合模式字事件发生源
	#endif

}TPreProgramData;


typedef struct TProgramRecord_t
{

	#if(SEL_PRG_RECORD645 == YES)
	//645编程记录放在ee中的连续空间
	TPrgProgramRom645		PrgProgramRom645[MAX_EVENTRECORD_NUMBER]; 
	#endif
	//临时保存参数设置之前的数据
	TPreProgramData 		PreProgramData;
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录
	DWORD dwPrgQComBo2Num;//无功组合方式2特征字编程次数
	//当前事件状态表,无功组合方式编程记录只保存发生时刻，下标:无功组合方式1特征字（0），无功组合方式2特征字（1）
	TRealTimer PrgQCombo_CurrentEventStatus[2];//必须紧接在dwPrgQComBo2Num 之后，清零一块清
	#endif	
	
	//临时保存最近一次编程记录的最近10个OAD及指针	
	TPrgProgramSubRom		EeTempOadList;

}TProgramRecordRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE PrgFlag;
extern BYTE PrgFlag645,ProRecord645DiNum;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 获取编程记录状态
//
//参数: 	Phase[in]             
//返回值:  	TRUE/FALSE
//
//备注: 
//-----------------------------------------------
BYTE GetPrgStatus(BYTE Phase);
void ClearPrgRecordRam( void );
BYTE GetPrgRecord( void );
void FactoryInitPrgRecordPara( void );

#endif//__PRGRECORD_H
