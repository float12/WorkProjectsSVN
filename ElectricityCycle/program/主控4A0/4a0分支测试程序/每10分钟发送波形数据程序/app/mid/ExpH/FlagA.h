#ifndef __FLAG_A_H__
#define __FALG_A_H__

typedef enum
{//							序号	秒分配范围
 	eModuleA = 0,	//0		(3~5)s

	
	////!!!!!!这个必须放最后，不要挪动,此序号最大为16（目前只分配了一个WORD），不要再超过次数!!!!!!
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
	eFLAG_MINUTE,
	eFLAG_HOUR,
    eFLAG_DAY,
	eFLAG_TIME_CHANGE,			//时间改变标志
	eFLAG_SWITCH_JUDGE,			//设置切换时间标志
	
	eFLAG_TIMER_T,////!!!!!!这个必须放最后，不要挪动!!!!!!

}eTaskTimeFlag;

typedef enum
{
	//系统是否上电
	eSYS_STATUS_POWER_ON = 0,
	
	//!!!!!!这个必须放最后，不要挪动!!!!!!
	eSYS_STATUS_NUM_T,
	
}eSysStatusFlag;

void api_SetSysStatusModuleA(BYTE StatusNo);
void api_ClrSysStatusModuleA(BYTE StatusNo);
BOOL api_GetSysStatusModuleA(BYTE StatusNo);

void api_SetAllTaskFlagModuleA(BYTE FlagID);
void api_SetTaskFlagModuleA( BYTE TaskID, BYTE FlagID );
BOOL api_GetTaskFlagModuleA( BYTE TaskID, BYTE FlagID );
void api_ClrTaskFlagModuleA( BYTE TaskID, BYTE FlagID );

void lib_ExchangeDataModuleA(BYTE *BufA, BYTE *BufB, BYTE Len);
#endif
