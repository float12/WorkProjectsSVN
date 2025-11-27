//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.15
//描述		管理系统标志、硬件标志、错误标志
//修改记录	
//----------------------------------------------------------------------
#ifndef __FLAG_API_H
#define __FLAG_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

typedef enum
{//							序号	秒分配范围
 	eTASK_SAMPLE_ID = 0,	//0		(3~5)s
	eTASK_LCD_ID,			//1		(6~8)s
	eTASK_PROTOCOL_ID,		//2		(9~11)s
	eTASK_LOADPROFILE_ID,	//3		(12~14)s
	eTASK_DEMAND_ID,		//4		(15~17)s
	eTASK_EVENTS_ID,		//5		(18~20)s
	eTASK_PREPAY_ID,		//6		(21~23)s
	eTASK_FREEZE_ID,		//7		(24~26)s
	eTASK_ENERGY_ID,		//8		(27~29)s
	eTASK_PARA_ID,			//9		(30~32)s
	eTASK_RTC_ID,			//10	(33~35)s
	eTASK_LOWPOWER_ID,		//11	(36~38)s
	eTASK_SYSWATCH_ID,		//12	(39~41)s
	eTASK_RELAY_ID,			//13	(42~44)s

    eTASK_FREQ_ID,          //14
    eTASK_STATS_ID,         //15
    eTASK_FLICK_ID,         //16
	
	
	////!!!!!!这个必须放最后，不要挪动,此序号最大为32（目前只分配了一个DWORD），不要再超过次数!!!!!!
	eTASK_ID_NUM_T
	
}TTaskIDNum;

//每个任务标志定义
typedef enum
{
	//用于高速标志
	eFLAG_10_MS = 0,
	//500ms标志
	eFLAG_500_MS,
	eFLAG_SECOND,
    eFLAG_10_S,
	eFLAG_MINUTE,
    eFLAG_5_MIN,
    eFLAG_10_MIN,
	eFLAG_HOUR,
    eFLAG_2_H,
    eFLAG_24_H,
	eFLAG_TIME_CHANGE,			//时间改变标志
	eFLAG_SWITCH_JUDGE,			//设置切换时间标志
	
	eFLAG_TIMER_T////!!!!!!这个必须放最后，不要挪动!!!!!!

}eTaskTimeFlag;

typedef enum
{
	// 厂内模式字,可执行相关内部操作,（1为厂内可操作）,（0为出厂不可操作）
	eSYS_STATUS_INSIDE_FACTORY,
    // 开始接收升级报文升级
    eSYS_STATUS_START_UPDATE,
    // 串口接收缓冲区满
    eSYS_STATUS_UPDATE_RECBUF_FULL,
    // 数据接收完成
    eSYS_STATUS_UPDATE_REC_COMPLETE,

    // LED状态：无程序
    eSYS_STATUS_LED_NOCODE,
    // LED状态：准备升级
    eSYS_STATUS_LED_READY_UPDATE,
    // LED状态：正在升级
    eSYS_STATUS_LED_UPDATING,

	//!!!!!!这个必须放最后，不要挪动!!!!!!
	eSYS_STATUS_NUM_T,
	
}eSysStatusFlag;

#define		MAX_SYS_STATUS_NUM		64		//必须为8的倍数

typedef struct TFlagBytes_t
{
	//任务标志，暂定最大32个任务，即每个时间标志占4个字节
	DWORD TaskTimeFlag[eFLAG_TIMER_T];
    // 系统状态
	BYTE SysStatus[MAX_SYS_STATUS_NUM / 8];
	
}TFlagBytes;


//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void api_SetAllTaskFlag( BYTE FlagID );

//-----------------------------------------------
//函数功能: 置任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//函数功能: 获取任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	TRUE:相应标志置位了  FALSE:相应标志未置位
//
//备注:   
//-----------------------------------------------
BOOL api_GetTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//函数功能: 清除任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//函数功能: 设置系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetSysStatus(BYTE StatusNo);

//-----------------------------------------------
//函数功能: 清除系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrSysStatus(BYTE StatusNo);

//-----------------------------------------------
//函数功能: 获取系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	TRUE:系统状态置位了 FALSE:系统状态未置位
//
//备注:   
//-----------------------------------------------
BOOL api_GetSysStatus(BYTE StatusNo);

#endif//#ifndef __FLAG_API_H
