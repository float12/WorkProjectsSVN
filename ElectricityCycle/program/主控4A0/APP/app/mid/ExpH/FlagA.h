#ifndef __FLAG_A_H__
#define __FALG_A_H__

typedef enum
{//							���	����䷶Χ
 	eModuleA = 0,	//0		(3~5)s

	
	////!!!!!!����������󣬲�ҪŲ��,��������Ϊ16��Ŀǰֻ������һ��WORD������Ҫ�ٳ�������!!!!!!
	eTASK_ID_NUM_T
	
}TTaskIDNum;

//ÿ�������־����
typedef enum
{
	//���ڸ��ٱ�־
	eFLAG_10_MS = 0,
	//500ms��־
	eFLAG_500_MS,
	eFLAG_SECOND,
	eFLAG_MINUTE,
	eFLAG_HOUR,
    eFLAG_DAY,
	eFLAG_TIME_CHANGE,			//ʱ��ı��־
	eFLAG_SWITCH_JUDGE,			//�����л�ʱ���־
	
	eFLAG_TIMER_T,////!!!!!!����������󣬲�ҪŲ��!!!!!!

}eTaskTimeFlag;

typedef enum
{
	//ϵͳ�Ƿ��ϵ�
	eSYS_STATUS_POWER_ON = 0,
	
	//!!!!!!����������󣬲�ҪŲ��!!!!!!
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
