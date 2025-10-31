//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.20
//描述		电压合格率头文件
//备注   未完全与事件event.c分离；待后续与event.c分离；融入到sample模块中
//----------------------------------------------------------------------

#ifndef __STATISTICS_VOLTAGE_H
#define __STATISTICS_VOLTAGE_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//电压合格率结构
typedef struct TStatVRunData_t
{
	DWORD dwMonitorTime;//总检测时间
	WORD wPassRate;//合格率
	WORD wOverRate;//超限率
	DWORD dwOverTime;//超上限时间
	DWORD dwBelowTime;//超下限时间

}TStatVRunData;


//电压合格率参数结构
typedef struct TVStatisticsPara_t
{
	WORD VOverHigh;//电压考核上限 单位0.1v
	WORD VOverLow;//电压考核下限 单位0.1v
	WORD VPassHigh;//电压合格上限 单位0.1v
	WORD VPassLow;//电压合格下限 单位0.1v

}TVStatisticsPara;


//电压合格率存储结构
typedef struct TStatVRunDataRom_t
{
	//日数据
	TStatVRunData DayStatVRunData[4];
	//月数据
	TStatVRunData MonStatVRunData[4];

}TStatVRunDataRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern __no_init DWORD ClrVRunDayTimer;	// 清日电压合格率定时计数器
extern __no_init DWORD ClrVRunMonTimer;	// 清月电压合格率定时计数器

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//------------------------------------------------------------------------------------
// 读出事件记录
BYTE ReadVRunTime(BYTE Phase, BYTE No, BYTE *pBuf);
void SwapVRunTime(void);
void StatVRunTime(void);


void FactoryInitStatisticsVPara( void );
void PowerUpVRunTime( void );
#endif//__STATISTICS_VOLTAGE_H

