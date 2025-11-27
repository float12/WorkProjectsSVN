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
	eTASK_JLINK_ID,			//1		(6~8)s
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
	
	eTASK_VFRATE_ID,					//17
	
	////!!!!!!这个必须放最后，不要挪动,此序号最大为32（目前只分配了一个DWORD），不要再超过次数!!!!!!
	eTASK_ID_NUM_T
	
}TTaskIDNum;

//每个任务标志定义
typedef enum
{
	//用于高速标志
	eFLAG_10_MS = 0,
	//250ms标志
	eFLAG_250_MS,
	//500ms标志
	eFLAG_500_MS,
	eFLAG_SECOND,
    eFLAG_10_S,
	eFLAG_MINUTE,
    eFLAG_5_MIN,
    eFLAG_10_MIN,
	eFLAG_15_MIN,
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
    // 计算序分量标志
    eSYS_STATUS_CALC_SEQ,
    
    // 统计计算标志，顺序请勿改变，需与 STATS_TYPE 保持一致
    eSYS_STATUS_CALC_STATS_HARMVOL,
    eSYS_STATUS_CALC_STATS_HARMCUR,
    eSYS_STATUS_CALC_STATS_IHARMVOL,
    eSYS_STATUS_CALC_STATS_IHARMCUR,
    eSYS_STATUS_CALC_STATS_PST,
    eSYS_STATUS_CALC_STATS_PLT,
    eSYS_STATUS_CALC_STATS_VOLDEV,
    eSYS_STATUS_CALC_STATS_HZDEV,
    eSYS_STATUS_CALC_STATS_VOLUNBALANCE,
    eSYS_STATUS_CALC_STATS_CURUNBALANCE,
    eSYS_STATUS_CALC_STATS_VOLFLUC,
    eSYS_STATUS_CALC_STATS_VOLFLUCF,

    // 电压谐波总畸变率超限
    eSYS_STATUS_U_THD_OVER,
    // 电流谐波总畸变率超限
    eSYS_STATUS_I_THD_OVER,
    // 计量芯片故障
    eSYS_STATUS_METERING_FAULT,
    // 电压短时闪变超限
    eSYS_STATUS_PST_OVER,
    // 电压长时闪变超限
    eSYS_STATUS_PLT_OVER,
    // 电压暂降
    eSYS_STATUS_U_DIP,
    // 电压暂升
    eSYS_STATUS_U_SWL,
    // 电压中断
    eSYS_STATUS_U_INT,

	// 电压偏差超限事件
    eSYS_STATUS_U_DEV,
	// 频率超限事件
    eSYS_STATUS_F_DEV,
	// 电压不平衡度超限事件
    eSYS_STATUS_U_BAL,
	// 电流不平衡度超限
    eSYS_STATUS_I_BAL,
	// 电压波动超限
    eSYS_STATUS_U_CHG,

    eSYS_STATUS_METERING_FAULT2,
	//!!!!!!这个必须放最后，不要挪动!!!!!!
	eSYS_STATUS_NUM_T,
	
}eSysStatusFlag;

#define		MAX_SYS_STATUS_NUM		64		//必须为8的倍数

//EEPROM写入错误（重试次数到），不能反映芯片局部损坏，只有整个损坏才会此报警。
#define ERR_WRITE_EEPROM1					4
#define ERR_WRITE_EEPROM2					5
#define ERR_WRITE_EEPROM3					6
#define ERR_CHECK_5460_1					25
#define MAX_ERROR							64
typedef struct TFlagBytes_t
{
	//任务标志，暂定最大32个任务，即每个时间标志占4个字节
	DWORD TaskTimeFlag[eFLAG_TIMER_T];
    // 系统状态
	BYTE SysStatus[MAX_SYS_STATUS_NUM / 8];
	//错误标志
	BYTE ErrByte[(MAX_ERROR/8)];
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
//-----------------------------------------------
//函数功能: 设置指定的错误
//
//参数: 
//			Err[in]		指定的错误标志号
//			BIT15		0  错误信息要写入EEPROM
//           			1  错误信息不写入EEPROM
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetError(WORD Err);

//-----------------------------------------------
//函数功能: 检查当前是否存在指定的错误
//
//参数: 
//			Err[in]		指定的错误标志号
//                    
//返回值:  	TRUE:存在指定的错误  	FALSE:不存在指定的错误
//
//备注:   
//-----------------------------------------------
BOOL api_CheckError(WORD Err);

//-----------------------------------------------
//函数功能: 清除指定的错误标志号
//
//参数: 
//			Err[in]		指定的错误标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrError(WORD Err);
#endif//#ifndef __FLAG_API_H
