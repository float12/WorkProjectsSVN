//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.9.28
//描述		事件模块源文件
//修改记录	
//---------------------------------------------------------------------- 

#include "AllHead.h"
#include "event.h"
#include "sm_task.h"
// 若有事件记录
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define EVENT_COLUMN_OAD_MAX_NUM			7					// 固定列OAD最多个数
// 相对规约反序
#define STARTTIME_OAD					0x00021E20			// 事件开始时间对应OAD
#define ENDTIME_OAD						0x00022020			// 事件结束时间对应OAD
#define RECORDNO_OAD					0x00022220			// 事件记录序号对应OAD
#define GEN_SOURCE_OAD					0x00022420			// 事件发生源对应OAD
#define NOTIFICATION_OAD				0x00020033			// 事件上报状态对应OAD
#define	MAXDEMAND_OAD					0X06020833			// 超限期间需量最大值OAD
#define	MAXDEMAND_TIME_OAD				0X07020833			// 超限期间需量最大值发生时间OAD
#define PRG_OBJ_LIST_OAD				0X06020233			// 编程对象列表
#define CLR_EVENT_LIST_OAD				0X06020c33			// 事件清零列表
#define RESET_EVENT_ADDR				0x40023874 //gd32f415芯片的复位寄存器地址

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct TEventData_t
{
	TDLT698RecordPara 		*pDLT698RecordPara;
	TEventOADCommonData		*pEventOADCommonData;
	TEventAttInfo			*pEventAttInfo;
	TEventAddrLen			*pEventAddrLen;
	DWORD					dwPointer;
	BYTE					*pBuf;
	DWORD					dwOad;
	WORD					Len;
	WORD					off;
	BYTE					Tag;
	BYTE					status;
	BYTE					AllorSep;
	#if(PROTOCOL_VERSION == 25)	
	WORD					Last;
	#elif(PROTOCOL_VERSION == 24)	
	BYTE					Last;
	#endif

	DWORD					dwMaxRecorder;
}TEventData;

typedef struct TEventOadTable_t
{
	WORD	OI;
	WORD	Number;
	DWORD	OadList[EVENT_COLUMN_OAD_MAX_NUM];
}TEventOadTab;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
TEventStatus EventStatus;
TReportPara ReportPara;
TFollowPara FollowPara;
extern TReportMode gReportMode;
extern TEventPara gEventPara;
BYTE gDownFlagLen = 0; //用于存储下载方标识，存储从flash读取出来的数据长度
BYTE gDownFlagFlash = 0;//用于区分是否从flash读取的下载方式标识
BYTE gDownFlagCount = 0;//下载方式标识获取的次数
extern TFileInfo tFileInfo;
BYTE pAllocBuftemp[320];
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
#if(PROTOCOL_VERSION == 25)	
BYTE g_SpeDevSaveCount = 0;	//用于存储特定设备保存事件次数
#endif

// 事件信息表，用于查找事件状态函数  此表格应与eSUB_EVENT_INDEX里面定义的枚举顺序保持一致
const TSubEventInfoTab	SubEventInfoTab[] = 
{
	//电压谐波总畸变率超限事件
	{eSUB_EVENT_HARMONIC_U_DISTORTION_A, 		eEVENT_HARMONIC_U_DISTORTION_NO, 			ePHASE_A, 	GetUHarDistorStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_HARMONIC_U_DISTORTION_B, 		eEVENT_HARMONIC_U_DISTORTION_NO, 			ePHASE_B, 	GetUHarDistorStatus },
	{eSUB_EVENT_HARMONIC_U_DISTORTION_C, 		eEVENT_HARMONIC_U_DISTORTION_NO, 			ePHASE_C, 	GetUHarDistorStatus },
	#endif
	
	//电流谐波总畸变率超限事件
	{eSUB_EVENT_HARMONIC_I_DISTORTION_A, 		eEVENT_HARMONIC_I_DISTORTION_NO, 			ePHASE_A, 	GetIHarDistorStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_HARMONIC_I_DISTORTION_B, 		eEVENT_HARMONIC_I_DISTORTION_NO, 			ePHASE_B, 	GetIHarDistorStatus },
	{eSUB_EVENT_HARMONIC_I_DISTORTION_C, 		eEVENT_HARMONIC_I_DISTORTION_NO, 			ePHASE_C, 	GetIHarDistorStatus },
	#endif
//////////////////////////////////////////////////////////////从下面开始去掉这些事件
#if 1
	//谐波电压含量超限事件
	{eSUB_EVENT_HARMONIC_U_OVERRUN_A, 			eEVENT_HARMONIC_U_OVERRUN_NO, 				ePHASE_A, 	GetUHarOverRunStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_HARMONIC_U_OVERRUN_B, 			eEVENT_HARMONIC_U_OVERRUN_NO, 				ePHASE_B, 	GetUHarOverRunStatus },
	{eSUB_EVENT_HARMONIC_U_OVERRUN_C, 			eEVENT_HARMONIC_U_OVERRUN_NO, 				ePHASE_C, 	GetUHarOverRunStatus },
	#endif

	//谐波电流含量超限事件
	{eSUB_EVENT_HARMONIC_I_OVERRUN_A, 			eEVENT_HARMONIC_I_OVERRUN_NO, 				ePHASE_A, 	GetIHarOverRunStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_HARMONIC_I_OVERRUN_B, 			eEVENT_HARMONIC_I_OVERRUN_NO, 				ePHASE_B, 	GetIHarOverRunStatus },
	{eSUB_EVENT_HARMONIC_I_OVERRUN_C, 			eEVENT_HARMONIC_I_OVERRUN_NO, 				ePHASE_C, 	GetIHarOverRunStatus },
	#endif

	//谐波功率含量超限事件
	{eSUB_EVENT_HARMONIC_P_OVERRUN_A, 			eEVENT_HARMONIC_P_OVERRUN_NO, 				ePHASE_A, 	GetPHarOverRunStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_HARMONIC_P_OVERRUN_B, 			eEVENT_HARMONIC_P_OVERRUN_NO, 				ePHASE_B, 	GetPHarOverRunStatus },
	{eSUB_EVENT_HARMONIC_P_OVERRUN_C, 			eEVENT_HARMONIC_P_OVERRUN_NO, 				ePHASE_C, 	GetPHarOverRunStatus },
	#endif

	//间谐波电压含量超限事件
	{eSUB_EVENT_INTER_HARMONIC_U_OVERRUN_A, 	eEVENT_INTER_HARMONIC_U_OVERRUN_NO, 		ePHASE_A, 	GetUInterHarOverRunStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_INTER_HARMONIC_U_OVERRUN_B, 	eEVENT_INTER_HARMONIC_U_OVERRUN_NO, 		ePHASE_B, 	GetUInterHarOverRunStatus },
	{eSUB_EVENT_INTER_HARMONIC_U_OVERRUN_C, 	eEVENT_INTER_HARMONIC_U_OVERRUN_NO, 		ePHASE_C, 	GetUInterHarOverRunStatus },
	#endif

	//间谐波电流含量超限事件
	{eSUB_EVENT_INTER_HARMONIC_I_OVERRUN_A, 	eEVENT_INTER_HARMONIC_I_OVERRUN_NO, 		ePHASE_A, 	GetIInterHarOverRunStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_INTER_HARMONIC_I_OVERRUN_B, 	eEVENT_INTER_HARMONIC_I_OVERRUN_NO, 		ePHASE_B, 	GetIInterHarOverRunStatus },
	{eSUB_EVENT_INTER_HARMONIC_I_OVERRUN_C, 	eEVENT_INTER_HARMONIC_I_OVERRUN_NO, 		ePHASE_C, 	GetIInterHarOverRunStatus },
	#endif

	//间谐波电流含量超限事件
	{eSUB_EVENT_INTER_HARMONIC_P_OVERRUN_A, 	eEVENT_INTER_HARMONIC_P_OVERRUN_NO, 		ePHASE_A, 	GetPInterHarOverRunStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_INTER_HARMONIC_P_OVERRUN_B, 	eEVENT_INTER_HARMONIC_P_OVERRUN_NO, 		ePHASE_B, 	GetPInterHarOverRunStatus },
	{eSUB_EVENT_INTER_HARMONIC_P_OVERRUN_C, 	eEVENT_INTER_HARMONIC_P_OVERRUN_NO, 		ePHASE_C, 	GetPInterHarOverRunStatus },
	#endif
	#endif
//////////////////////////////////////////////////从上面开始去掉这些事件
	//电压短时闪变超限事件
	{eSUB_EVENT_SHORT_FLICKER_U_OVERRUN_A, 		eEVENT_SHORT_FLICKER_U_OVERRUN_NO, 			ePHASE_A, 	GetUShortFlickerStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_SHORT_FLICKER_U_OVERRUN_B, 		eEVENT_SHORT_FLICKER_U_OVERRUN_NO, 			ePHASE_B, 	GetUShortFlickerStatus },
	{eSUB_EVENT_SHORT_FLICKER_U_OVERRUN_C, 		eEVENT_SHORT_FLICKER_U_OVERRUN_NO, 			ePHASE_C, 	GetUShortFlickerStatus },
	#endif

	//电压长时闪变超限事件
	{eSUB_EVENT_LONG_FLICKER_U_OVERRUN_A, 		eEVENT_LONG_FLICKER_U_OVERRUN_NO, 			ePHASE_A, 	GetULongFlickerStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_LONG_FLICKER_U_OVERRUN_B, 		eEVENT_LONG_FLICKER_U_OVERRUN_NO, 			ePHASE_B, 	GetULongFlickerStatus },
	{eSUB_EVENT_LONG_FLICKER_U_OVERRUN_C, 		eEVENT_LONG_FLICKER_U_OVERRUN_NO, 			ePHASE_C, 	GetULongFlickerStatus },
	#endif
	
	//电压暂降事件
	{eSUB_EVENT_SAG_U_A, 						eEVENT_SAG_U_NO, 							ePHASE_A, 	GetUSagStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_SAG_U_B, 						eEVENT_SAG_U_NO, 							ePHASE_B, 	GetUSagStatus },
	{eSUB_EVENT_SAG_U_C, 						eEVENT_SAG_U_NO, 							ePHASE_C, 	GetUSagStatus },
	#endif

	//电压暂升事件
	{eSUB_EVENT_SWELL_U_A, 						eEVENT_SWELL_U_NO, 							ePHASE_A, 	GetUSwellStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_SWELL_U_B, 						eEVENT_SWELL_U_NO, 							ePHASE_B, 	GetUSwellStatus },
	{eSUB_EVENT_SWELL_U_C, 						eEVENT_SWELL_U_NO, 							ePHASE_C, 	GetUSwellStatus },
	#endif

	//电压中断事件
	{eSUB_EVENT_INTERRUPTION_U_A, 				eEVENT_INTERRUPTION_U_NO, 					ePHASE_A, 	GetUInterruptionStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_INTERRUPTION_U_B, 				eEVENT_INTERRUPTION_U_NO, 					ePHASE_B, 	GetUInterruptionStatus },
	{eSUB_EVENT_INTERRUPTION_U_C, 				eEVENT_INTERRUPTION_U_NO, 					ePHASE_C, 	GetUInterruptionStatus },
	#endif

	//////////////////////////////////////////////////////////////////////////
	//电压偏差超限事件
	{eSUB_EVENT_U_DEV_A, 						eEVENT_U_DEV_NO, 							ePHASE_A, 	GetUDevStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_U_DEV_B, 						eEVENT_U_DEV_NO, 							ePHASE_B, 	GetUDevStatus },
	{eSUB_EVENT_U_DEV_C, 						eEVENT_U_DEV_NO, 							ePHASE_C, 	GetUDevStatus },
	#endif

	//频率偏差超限事件
	{eSUB_EVENT_F_DEV_A, 						eEVENT_F_DEV_NO, 							ePHASE_A, 	GetFDevStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_F_DEV_B, 						eEVENT_F_DEV_NO, 							ePHASE_B, 	GetFDevStatus },
	{eSUB_EVENT_F_DEV_C, 						eEVENT_F_DEV_NO, 							ePHASE_C, 	GetFDevStatus },
	#endif

	//电压不平衡超限事件
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_U_UNBALANCE, 					eEVENT_U_UNBALANCE_NO, 						ePHASE_C, 	GetUUnbalanceStatus },
	#endif

	//频率偏差超限事件
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_I_UNBALANCE, 					eEVENT_I_UNBALANCE_NO, 						ePHASE_B, 	GetIUnbalanceStatus },
	#endif

	//电压波动超限事件
	{eSUB_EVENT_U_CHANGE_A, 					eEVENT_U_CHANGE_NO, 						ePHASE_A, 	GetUChangeStatus },
	#if(MAX_PHASE == 3)
	{eSUB_EVENT_U_CHANGE_B, 					eEVENT_U_CHANGE_NO, 						ePHASE_B, 	GetUChangeStatus },
	{eSUB_EVENT_U_CHANGE_C, 					eEVENT_U_CHANGE_NO, 						ePHASE_C, 	GetUChangeStatus },
	#endif

	//////////////////////////////////////////////////////////////////////////

	//升级事件
	{eSUB_EVENT_UPDATE, 						eSUB_EVENT_UPDATE_NO, 						ePHASE_A, 	NULL },

	//电能表清零事件
	{eSUB_EVENT_METER_CLEAR, 					eSUB_EVENT_METER_CLEAR_NO, 					ePHASE_A, 	NULL },

	//电能表计量芯片故障事件
	{eSUB_EVENT_JLCHIP_FAIL, 					eSUB_EVENT_JLCHIP_FAIL_NO, 					ePHASE_A, 	GetJLChipFailStatus },

	//校时事件
	{eSUB_EVENT_ADJUST_TIME, 					eSUB_EVENT_ADJUST_TIME_NO, 					ePHASE_A, 	NULL },

	#if(PROTOCOL_VERSION == 25)	
	//特定设备事件
	{eSUB_EVENT_SPECIFIC_DEV, 					eSUB_EVENT_SPECIFIC_DEV_NO, 				ePHASE_A, 	NULL },

	//未知设备事件
	{eSUB_EVENT_UNKNOWN_DEV, 					eSUB_EVENT_UNKNOWN_DEV_NO, 					ePHASE_A, 	NULL },
	#endif
};

//此表格应与eEVENT_INDEX里面定义枚举的顺序保持一致
const TEventInfoTab EventInfoTab[] =
{
	//OI								事件枚举号							最大相			掉电特殊处理 			扩展长度		参数个数
	{OAD_HARMONIC_U_DISTORTION,		eEVENT_HARMONIC_U_DISTORTION_NO,	MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_HARMONIC_I_DISTORTION,		eEVENT_HARMONIC_I_DISTORTION_NO,	MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_HARMONIC_U_OVERRUN,		eEVENT_HARMONIC_U_OVERRUN_NO,		MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_HARMONIC_I_OVERRUN,		eEVENT_HARMONIC_I_OVERRUN_NO,		MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_HARMONIC_P_OVERRUN,		eEVENT_HARMONIC_P_OVERRUN_NO,		MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_INTER_HARMONIC_U_OVERRUN,	eEVENT_INTER_HARMONIC_U_OVERRUN_NO,	MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_INTER_HARMONIC_I_OVERRUN,	eEVENT_INTER_HARMONIC_I_OVERRUN_NO,	MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_INTER_HARMONIC_P_OVERRUN,	eEVENT_INTER_HARMONIC_P_OVERRUN_NO,	MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_SHORT_FLICKER_U_OVERRUN,	eEVENT_SHORT_FLICKER_U_OVERRUN_NO,	MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_LONG_FLICKER_U_OVERRUN,	eEVENT_LONG_FLICKER_U_OVERRUN_NO,	MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_SAG_U,						eEVENT_SAG_U_NO,					MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_SWELL_U,					eEVENT_SWELL_U_NO,					MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{OAD_INTERRUPTION_U,			eEVENT_INTERRUPTION_U_NO,			MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},	
///////////////////////////////////////////////////////////////////
	{OAD_U_DEV,						eEVENT_U_DEV_NO,					MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},	
	{OAD_F_DEV,						eEVENT_F_DEV_NO,					MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},	
	{OAD_U_UNBALANCE,				eEVENT_U_UNBALANCE_NO,				MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},	
	{OAD_I_UNBALANCE,				eEVENT_I_UNBALANCE_NO,				MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},	
	{OAD_U_CHANGE,					eEVENT_U_CHANGE_NO,					MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},	

///////////////////////////////////////////////////////////////////
	{OAD_APP_UPDATE,				eSUB_EVENT_UPDATE_NO,				MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},

	{OAD_METER_CLEAR,				eSUB_EVENT_METER_CLEAR_NO,			MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},

	{OAD_JLCHIP_FAIL,				eSUB_EVENT_JLCHIP_FAIL_NO,			MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},

	{OAD_ADJUST_TIME,				eSUB_EVENT_ADJUST_TIME_NO,			MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
//	{OAD_SAG_U,						eEVENT_SAG_U_NO,					MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{0x3320,						eSUB_EVENT_3320,					MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	
	#if(PROTOCOL_VERSION == 25)		
	{OAD_SPECIFIC_DEV,				eSUB_EVENT_SPECIFIC_DEV_NO,			MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	
	{OAD_UNKNOWN_DEV,				eSUB_EVENT_UNKNOWN_DEV_NO,			MAX_PHASE,		NO,				0,		    0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
};

// 所有固定列的OAD
static const DWORD FixedColumnOadTab[] =
{
	RECORDNO_OAD,		// 序号
	STARTTIME_OAD,		// 开始时间
	ENDTIME_OAD,		// 结束时间
	GEN_SOURCE_OAD,		// 发生源
	NOTIFICATION_OAD,	// 上报状态
	MAXDEMAND_OAD,		// 超限期间最大需量
	MAXDEMAND_TIME_OAD,	// 超限期间最大需量发生时间
	PRG_OBJ_LIST_OAD,	// 编程对象列表
	CLR_EVENT_LIST_OAD	// 事件清零列表
};

// OAD 固定列列表 如果事件记录单元为标准事件单元可不加到此数组中，特殊事件记录单元必须加过来
static const TEventOadTab	EventOadTab[] = 
{
	{0xFFFF,4,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD },	// class id 24
};

static const TEventOadTab	EventOadTab7[] = 
{
	{0xFFFF,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },	// class id 7
};

static BYTE 	SubEventTimer[SUB_EVENT_INDEX_MAX_NUM];	// 定时计数器
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern WORD api_GetRtcDateTime( WORD Type, BYTE *pBuf );
extern uint32_t  fal_partition_start(const char *name);
extern void SetReportIndex( BYTE inSubEventIndex, BYTE BeforeAfter );
extern void ClrReportIndexChannel( BYTE Ch, BYTE inSubEventIndex );
#if(PROTOCOL_VERSION == 25)	
extern void FactoryInitUnknownDevPara(void);
extern void FactoryInitSpecificDevPara(void);
#endif
//-----------------------------------------------
//函数功能: 设置事件主动上报
//
//参数: 
//	inEventIndex[in]:	eEVENT_INDEX 
//	Phase[in]:			ePHASE_TYPE  
//  
//返回值:				TRUE/FALSE  
//备注:  
//-----------------------------------------------

BOOL SetReportEventOAD( BYTE inEventIndex, BYTE Phase )
{
	WORD i,j;
	DWORD OAD;

	for( i = 0; i < (sizeof(EventInfoTab)/sizeof(TEventInfoTab)); i++ )
	{
		if( inEventIndex == EventInfoTab[i].EventIndex )
		{	
			//OAD = ((EventInfoTab[i].OI<<16)|((Phase+6)<<8));

			#if(PROTOCOL_VERSION == 25)	
			if( (EventInfoTab[i].OI==0X3013) || (EventInfoTab[i].OI==0X302F) || (EventInfoTab[i].OI==0X3036) || (EventInfoTab[i].OI==0X3016) || (EventInfoTab[i].OI==0X309A) || (EventInfoTab[i].OI==0X309B))
			#elif(PROTOCOL_VERSION == 24)	
			if( (EventInfoTab[i].OI==0X3013) || (EventInfoTab[i].OI==0X302F) || (EventInfoTab[i].OI==0X3036) || (EventInfoTab[i].OI==0X3016))
			#endif
			{
				//属性7
					OAD = ((EventInfoTab[i].OI<<16)|(2<<8));
			}
			else
			{
				//属性24
				OAD = ((EventInfoTab[i].OI<<16)|((Phase+6)<<8));
			}	
			//判断上报列表中是否已有OAD

            if(OAD == 0x0)
              return TRUE;
#if USE_PARA_CTRL
			if(gReportMode.byReportMode[inEventIndex] == 0)
#endif
			{
				for(j=0; j<ReportPara.OAD_Num; j++)
				{
					if( OAD == ReportPara.ReportOAD[j] )
					{
                        ReportPara.Times = 3;
                        ReportPara.CRC32 = lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD));
                        ManageComInfo.EventReportOverTime = 2;
						return TRUE;
					}
				}
				//增加OAD到上报列表
				if(OAD != 0x30130200) //私钥不允许清零，公钥不上报，因此3013不添加到主动上报列表中即可
				{
					ReportPara.Times = 3;
					ReportPara.ReportOAD[ReportPara.OAD_Num%SUB_EVENT_INDEX_MAX_NUM] = OAD;
					ReportPara.OAD_Num++;
					if(ReportPara.OAD_Num >= SUB_EVENT_INDEX_MAX_NUM)
						ReportPara.OAD_Num = SUB_EVENT_INDEX_MAX_NUM-1;
					ReportPara.CRC32 = lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD));
					ManageComInfo.EventReportOverTime = 2;
				}
			}
#if USE_PARA_CTRL
			else
			{
				if(OAD != 0x30130200) //私钥不允许清零，公钥不上报，因此3013不添加到跟随上报列表中即可
				{
					//增加OAD到上报列表
					FollowPara.ReportOAD = OAD;
					FollowPara.OAD_Num = 1;
						
					//FollowPara.ReportOAD = OAD;
					//FollowPara.OldOad = 0xFF;
				}
			}
#endif
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------
//函数功能: 删除事件主动上报
//
//参数: 
//	inEventIndex[in]:	eEVENT_INDEX 
//	Phase[in]:			ePHASE_TYPE  
//  
//返回值:  
//备注:  
//-----------------------------------------------
void DeleteReportEventOAD( WORD OI, BYTE Phase )
{
	BYTE j;
	DWORD OAD;
	
	//OAD = ((OI<<16)|((Phase+6)<<8));
	#if(PROTOCOL_VERSION == 25)	
	if( (OI==0X3013) || (OI==0X302F) || (OI==0X3036) || (OI==0X3016) || (OI==0X309A) || (OI==0X309B))
	#elif(PROTOCOL_VERSION == 24)	
	if( (OI==0X3013) || (OI==0X302F) || (OI==0X3036) || (OI==0X3016))
	#endif
	{
		//属性7
		OAD = ((OI<<16)|(2<<8));
	}
	else
	{
		//属性24
		OAD = ((OI<<16)|((Phase+6)<<8));
	}
	
	for(j=0; j<ReportPara.OAD_Num; j++)
	{
		if( OAD == ReportPara.ReportOAD[j] )
		{
			memset( (BYTE*)&ReportPara.ReportOAD[j], 0X00, 4 );
			memcpy( (BYTE*)&ReportPara.ReportOAD[j], (BYTE*)&ReportPara.ReportOAD[j+1], (ReportPara.OAD_Num-1-j)*4 );
			if(ReportPara.OAD_Num)
				ReportPara.OAD_Num -= 1;
			ReportPara.CRC32 = lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD));

			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ReportSafeRom.ReportPara ), sizeof(TReportPara), (BYTE *)&ReportPara );
			break;
		}
	}
}

void DeleteReportEventOADFollow( WORD OI, BYTE Phase )
{

	DWORD OAD;
	
	//OAD = ((OI<<16)|((Phase+6)<<8));
	#if(PROTOCOL_VERSION == 25)	
	if( (OI==0X3013) || (OI==0X302F) || (OI==0X3036)|| (OI==0X3016)|| (OI==0X309A)|| (OI==0X309B))
	#elif(PROTOCOL_VERSION == 24)
	if( (OI==0X3013) || (OI==0X302F) || (OI==0X3036)|| (OI==0X3016))
	#endif	
	{
		//属性7
		OAD = ((OI<<16)|(2<<8));
	}
	else
	{
		//属性24
		OAD = ((OI<<16)|((Phase+6)<<8));
	}
	
	if( OAD == FollowPara.ReportOAD )
	{
			 FollowPara.ReportOAD = 0;
			 FollowPara.OAD_Num = 0;
	}
}



extern BOOL bIsSecurityMode;
WORD api_MakeSecurityApdu(BYTE byType, BYTE* InBuff, WORD wInLen, BYTE* OutBuffer)
{
	int nLen = 0;
	uint32_t nOutLen = 512;
   BYTE byMAC[4]={0};
	BYTE OutBuff[512]={0};
	 
	if(byType == 1)		//需要加密的
	{
			OutBuff[nLen++] = 0x10;//SECURITY_REQUEST;
		  OutBuff[nLen++] = 0x01;
			
			sm4_Encrypt( TRUE, InBuff, wInLen, &OutBuff[nLen+1], &nOutLen );
		
			memcpy(byMAC,&OutBuff[nLen+1+nOutLen-4],4);
		
			OutBuff[nLen] = nOutLen-4;
			nLen += nOutLen-4;
			nLen += 1;
		
		  OutBuff[nLen++] = 0;//00SID+MAC
		
			OutBuff[nLen++] = 0;//SID
		  OutBuff[nLen++] = 0;
		  OutBuff[nLen++] = 0;
		  OutBuff[nLen++] = 0;
		
		  OutBuff[nLen++] = 0x02;//附加
		  OutBuff[nLen++] = (nOutLen>>8)&0xFF;
		  OutBuff[nLen++] = (nOutLen&0xFF);
		
		  OutBuff[nLen++] = 0x04;//MAC
		  OutBuff[nLen++] = byMAC[0];
		  OutBuff[nLen++] = byMAC[1];
			OutBuff[nLen++] = byMAC[2];
		  OutBuff[nLen++] = byMAC[3];
			
			memcpy(OutBuffer,OutBuff,nLen);
			return nLen;
	}
	else
	{
		memcpy(OutBuffer,InBuff,wInLen);
		return wInLen;
	}
}
//-----------------------------------------------
//函数功能: 获取事件主动上报APDU
//
//参数: 
//  
//返回值:				TRUE/FALSE  
//备注:  
//-----------------------------------------------
WORD api_GetReportEventAPDU( BYTE* Buf )
{
	BYTE i;
	WORD Offset;
#if(USE_PARA_CTRL)	
	if(gReportMode.g_EnableAutoReportFlag == 0)
	{
		return 0x8000;
	}
#endif
    if( ReportPara.CRC32 != lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD)) )
    {
        //return 0x8000;//暂时关闭 wxy
    }

	if(InitComplete == 0) //握手完成后才能主动上报
	{
		return 0x8000;
	}

	if( ReportPara.Times == 0 )
	{
	   // memset( (BYTE *)&ReportPara, 0x00, sizeof(ReportPara)) ;
	   // ReportPara.CRC32 = lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD));
		return 0x8000;
	}

	if( ManageComInfo.EventReportOverTime != 0 )
	{
	    return 0x8000;
	}
	
	if( ReportPara.OAD_Num == 0 )
	{
		return 0x8000;
	}

	ReportPara.Times--;

	Offset = 0;
	Buf[Offset++] = 0x88;
	Buf[Offset++] = 0x01;
	Buf[Offset++] = 0x00;
	Buf[Offset++] = 0x01;
	
	Buf[Offset++] = 0x33;
	Buf[Offset++] = 0x20;
	Buf[Offset++] = 0x02;
	Buf[Offset++] = 0x00;
	
	Buf[Offset++] = 0x01;
	Buf[Offset++] = 0x01;
	Buf[Offset++] = ReportPara.OAD_Num;

	for( i=0; i<ReportPara.OAD_Num; i++ )
	{
		Buf[Offset++] = 0x51;
		lib_ExchangeData(Buf+Offset, (BYTE*)&ReportPara.ReportOAD[i], 4);
		Offset += 4;
	}

	Buf[Offset++] = 0;
	Buf[Offset++] = 0;

        //wxy 密文主动上报
#if(USER_TEST_MODE)
	Offset = api_MakeSecurityApdu(bIsSecurityMode, &Buf[0], Offset, &Buf[0]);
#else
	Offset = api_MakeSecurityApdu(0, &Buf[0], Offset, &Buf[0]);
#endif	

	
	ReportPara.CRC32 = lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD));
	return Offset;
}

//-----------------------------------------------
//函数功能: 根据事件类型及相位确定分相事件枚举号
//
//参数: 
//			EventIndex[in]:	事件枚举号                    
//			Phase[in]:		相位                    
//返回值:	分相事件枚举号	
//备注:
//-----------------------------------------------
BYTE GetSubEventIndex(TEventAddrLen *pEventAddrLen)
{
	BYTE i;
	
	for( i=0; i<(sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab)); i++ )
	{
		if( 	(SubEventInfoTab[i].EventIndex==pEventAddrLen->EventIndex)
			&& 	((SubEventInfoTab[i].Phase==pEventAddrLen->Phase) || pEventAddrLen->Phase==0) )
		{
			break;
		}
	}

	if( i == (sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab)) )//如果没有搜索到 置默认值
	{
        i = 0;
        //ASSERT( FALSE, 0 );
	}
	
	return SubEventInfoTab[i].SubEventIndex;
}

//-----------------------------------------------
//函数功能: 根据事件类型及相位确定分相事件枚举号
//
//参数: 
//			EventIndex[in]:	事件枚举号                    
//			Phase[in]:		相位                    
//返回值:	分相事件枚举号	
//备注:
//-----------------------------------------------
BYTE GetSubEventIndex2(BYTE EventIndex)
{
	BYTE i;
	
	for( i=0; i<(sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab)); i++ )
	{
		if((SubEventInfoTab[i].EventIndex==EventIndex))
		{
			break;
		}
	}

	if( i == (sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab)) )//如果没有搜索到 置默认值
	{
		i = 0;
		//ASSERT( FALSE, 0 );
	}
	
	return SubEventInfoTab[i].SubEventIndex;
}

//-----------------------------------------------
//函数功能: 获取OI对应的EventIndex
//
//参数: 
//	OI[in]:	事件OI
//                    
//	pEventOrder[out]:	指向EventOrder的指针
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//			
//备注:
//-----------------------------------------------
BOOL GetEventIndex( WORD OI, BYTE *pEventIndex )
{
	BYTE i;

	for( i = 0; i < (sizeof(EventInfoTab)/sizeof(TEventInfoTab)); i++ )
	{
		if( OI == EventInfoTab[i].OI )
		{
			*pEventIndex = EventInfoTab[i].EventIndex;
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------
//函数功能: 获取事件记录的地址信息
//
//参数: 		BYTE i[in]
//			pTEventAddrLen[in/out]
//			EventIndex[in]		   --输入事件枚举号
//			Phase  [in] 		   --输入相，编程类均为0
//			dwAttInfoEeAddr[out]   --输出对应类编程记录的关联对象属性
//			dwDataInfoEeAddr[out]  --输出eeprom中的info数据地址
//			dwRecordAddr[out]	   --输出对应类编程记录的固定列数据地址
//			wDataLen[out]                
//返回值:  	TRUE
//
//备注:       以下每个函数为每类事件记录的获取地址函数
//-----------------------------------------------
BOOL GetEventRecordInfo(BYTE i, TEventAddrLen *pTEventAddrLen )
{
	int EventAddr;

	if( pTEventAddrLen->EventIndex > eNUM_OF_EVENT_PRG )//极限值检测
	{
		return FALSE;
	}
	
	if( pTEventAddrLen->SubEventIndex > eSUB_EVENT_STATUS_TOTAL )
	{
		return FALSE;
	}
	
	if( pTEventAddrLen->Phase > ePHASE_C )
	{
		return FALSE;
	}

	if( i > sizeof(EventInfoTab)/sizeof(TEventInfoTab) )
	{
		return FALSE;
	}

	EventAddr = fal_partition_start("evtFrz");//查表FAL_PART_TABLE找到事件记录基地址，替换之前的FLASH_EVENT_BASE
	if(EventAddr < 0x00100000)
		EventAddr = 0x00100000;

	pTEventAddrLen->dwAttInfoEeAddr = GET_CONTINUE_ADDR( EventConRom.EventAttInfo[pTEventAddrLen->EventIndex] );
	pTEventAddrLen->dwDataInfoEeAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.EventDataInfo[pTEventAddrLen->SubEventIndex] );
	//pTEventAddrLen->dwRecordAddr = FLASH_EVENT_BASE+(DWORD)pTEventAddrLen->SubEventIndex*SECTOR_SIZE*EVENT_SECTOR_NUM;
	pTEventAddrLen->dwRecordAddr = EventAddr+(DWORD)pTEventAddrLen->SubEventIndex*SECTOR_SIZE*EVENT_SECTOR_NUM;// 每个事件存2个扇区

	pTEventAddrLen->wDataLen = EVENT_UNIT_LEN*MAX_EVENTRECORD_NUMBER;

	return TRUE;
}

//-----------------------------------------------
//函数功能: 获取Type对应的属性地址，数据信息地址，数据地址，公共数据地址，最大数据长度
//
//参数: 
//	pTEventAddrLen:	TEventAddrLen类型的指针                    
// 		dwAttInfoEeAddr[out]:		OAD信息地址
// 		dwDataInfoEeAddr[out]:		数据信息地址
// 		dwRecordAddr[out]:			公共数据地址
// 		wDataLen[out]:				最大数据长度
// 		EventIndex[in]:				eEVENT_INDEX
// 		Phase[in]:					相位0总,1A,2B,3C
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//			
//备注:
//-----------------------------------------------
BOOL GetEventInfo( TEventAddrLen *pEventAddrLen )
{
	BYTE i;

	for( i = 0; i < sizeof(EventInfoTab)/sizeof(TEventInfoTab); i++ )
	{
		if( EventInfoTab[i].EventIndex == pEventAddrLen->EventIndex )
		{
			pEventAddrLen->SubEventIndex = GetSubEventIndex(pEventAddrLen);
			
			return GetEventRecordInfo( i, pEventAddrLen );
		}
	}

	return FALSE;
}

//-----------------------------------------------
//函数功能: 清零数据
//
//参数: 
// inEventIndex[in]:	eEVENT_INDEX    
//                
//返回值:无
//			
//备注: 清数据对应的Info
//-----------------------------------------------
static void ClearEventData( BYTE inEventIndex )
{	
	BYTE i,tSubEventIndex;
	DWORD dwAddr1;
	BYTE Buf[sizeof(TEventDataInfo)];

	memset(Buf,0x00,sizeof(TEventDataInfo));

	for( i = 0; i < sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab); i++ )
	{
		if( inEventIndex == SubEventInfoTab[i].EventIndex )
		{
			dwAddr1 = GET_SAFE_SPACE_ADDR( EventSafeRom.EventDataInfo[i] );
			api_VWriteSafeMem( dwAddr1, sizeof(TEventDataInfo), Buf );
			// 事件既然找到了就都在这里处理，不必要再搞一次循环
			if(inEventIndex < eNUM_OF_EVENT)// 清相关事件的掉电数据// 需要清除掉电标志中的RAM数据
			{
				// 当前状态
				tSubEventIndex = SubEventInfoTab[i].SubEventIndex;
				api_DealEventCurrentStatus(eCLEAR_EVENT_CURRENT_STATUS, tSubEventIndex);
				// Timer
				SubEventTimer[tSubEventIndex] = 0;

				// 延时后的状态
				api_DealEventDelayedStatus(eCLEAR_EVENT_DELAYED_STATUS, tSubEventIndex);
				
				//此处不能添加break
			}
		}
	}

}

//-----------------------------------------------
//函数功能: 读事件关联对象属性
//参数: 
//		OI[in]:	事件OI                
//  	No[in]:	第几个OAD，0代表全部OAD                
//  	Len[in]:规约层传来数据长度，超过此长度返回FALSE	
//  	pBuf[out]:返回数据的指针 按照冻结周期，OAD，存储深度顺序返回，每个DWORD，总共3*4字节
//
//返回值:	
//		值为0x8000 ：代表出现错误（ OI不支持，读取Epprom错误等问题）
//		值为0x0000 ：代表buf长度不够
//		其他:		正常数据长度( 在个数为0时，添加1字节的0 返回长度为1)
//备注: 
//-----------------------------------------------
WORD api_ReadEventAttribute( WORD OI, BYTE No, WORD Len, BYTE *pBuf )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	WORD wLenTemp;
	BYTE i;
	
	if( GetEventIndex( OI, &i ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.EventIndex = i;
	EventAddrLen.Phase = 0;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	if((OI == 0x3013) && ((EventAttInfo.NumofOad==0xFF) || (EventAttInfo.MaxRecordNo!=0x0A)) )
	{
			FactoryInitMeterClearPara();
			ReportPara.CRC32 = lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ReportSafeRom.ReportPara ), sizeof(TReportPara), (BYTE *)&ReportPara );
	}
	if( No == 0 )
	{
		wLenTemp = 0;
		for( i = 0; i < EventAttInfo.NumofOad; i++ )
		{
			wLenTemp += sizeof(DWORD);
			if( wLenTemp > Len )
			{
				return FALSE;
			}
			memcpy( pBuf+wLenTemp-sizeof(DWORD), (BYTE *)&EventAttInfo.Oad[i], sizeof(DWORD) );
		}

		if( wLenTemp == 0 )
		{
			pBuf[0] = 0;
			return 1;
		}
		else
		{
			return wLenTemp;			
		}
	}
	else
	{
		if( No > EventAttInfo.NumofOad )
		{
			pBuf[0] = 0;
			return 1;
		}
		if( sizeof(DWORD) > Len )
		{
			return FALSE;
		}
		memcpy( pBuf, (BYTE *)&EventAttInfo.Oad[No-1], sizeof(DWORD) );

		return sizeof(DWORD);
	}
}

//-----------------------------------------------
//函数功能: 设置事件关联对象属性
//
//参数: 
//		OI[in]:	事件OI  
//  	OadType[in]:
//				0 ~ 代表所有的OAD
//				N ~ 属性列表里的第N个OAD  设置第N个OAD时，认为第N个OAD之前是有值的
//				0xFF 初始化
//  	pOad[in]:  指向OAD Buffer的指针  2字节冻结周期+4字节OAD+2字节存储深度                
//  	OadNum[in]:	OAD的个数
//返回值:	TRUE   正确
//			FALSE  错误
//备注:	所有的重新计，重新分配EEP空间  只支持设置已有的OAD,相当于修改指定OAD	
//-----------------------------------------------
BOOL api_WriteEventAttribute( WORD OI, BYTE OadType, BYTE *pOad, WORD OadNum )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOad;
	WORD wDataLen;
	BYTE i,j,n,tEventIndex;
	
	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = 0;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	if( OadNum>MAX_EVENT_OAD_NUM )//OadNum =0情况下 勿轻易使用pOad -jwh
	{
		return FALSE;
	}

	if( OadType > MAX_EVENT_OAD_NUM )
	{
		if( OadType != 0xFF )
		{
			return FALSE;
		}
	}

	//api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);//读取事件关联信息

	if( (OadType==0) || (OadType==0xFF) )
	{
		memset( (BYTE *)&EventAttInfo, 0x00, sizeof(TEventAttInfo) );//将事件关联信息清零

		for( i = 0; i < OadNum; i++ )
		{
			if( i == 0 )
			{
				if( OadType == 0xFF )
				{
					//第一个数代表oad个数
					memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD));
					if( dwOad == 0 )
					{
						// 默认无OAD的情况
						EventAttInfo.MaxRecordNo = MAX_EVENTRECORD_NUMBER;						
						break;	
					}					
					continue;
				}
			}
			
			if( OadType == 0xFF )
			{
				j = i-1;
				lib_ExchangeData( (BYTE *)&EventAttInfo.Oad[j], pOad+i*sizeof(DWORD), sizeof(DWORD));
			}
			else
			{
				j = i;
				memcpy( (BYTE *)&EventAttInfo.Oad[j], pOad+i*sizeof(DWORD), sizeof(DWORD));
			}
			
			for( n=0; n<j; n++ )//不允许设置相同的OAD
			{
				if( EventAttInfo.Oad[j] == EventAttInfo.Oad[n] )
				{
					return FALSE;
				}
			}

		//	if( OI != 0x3036 )//升级事件数据长度取决于管理芯，需单独处理
			{
				dwOad = EventAttInfo.Oad[j];
				dwOad &= ~0x00E00000;
				
				// 不允许关联全部时段表,不允许关联全部节假日编程
				if( (dwOad==0x00021640) || (dwOad==0x00021740)  || (dwOad==0x00021140) )
				{
					return FALSE;
				}
				else
				{
					EventAttInfo.OadLen[j] = api_GetProOADLen( eMaxData, (BYTE *)&dwOad, 0 );
				}

				if( EventAttInfo.OadLen[j] == 0x8000 )
				{
					return FALSE;
				}			

				EventAttInfo.OadValidDataLen += EventAttInfo.OadLen[j];
				EventAttInfo.NumofOad++;
			}
		}

		if( OI == 0x3036 )
		{
			EventAttInfo.OadValidDataLen = 300;//保证每个扇区能存10条事件
			EventAttInfo.NumofOad = OadNum - 1;//第一个代表0AD个数
		}
		
		EventAttInfo.AllDataLen = (EventAttInfo.OadValidDataLen+sizeof(TEventOADCommonData)+EventInfoTab[EventAddrLen.EventIndex].ExtDataLen);
		wDataLen = EventAddrLen.wDataLen/EventAttInfo.AllDataLen;
		if( wDataLen > MAX_EVENTRECORD_NUMBER )
		{
			EventAttInfo.MaxRecordNo = MAX_EVENTRECORD_NUMBER;
		}
		else if( wDataLen == 0 )
		{
			return FALSE;
		}
		else
		{
			EventAttInfo.MaxRecordNo = wDataLen;
		}

		#if(PROTOCOL_VERSION == 25)	
		if(OI == OAD_UNKNOWN_DEV)
		{
			EventAttInfo.MaxRecordNo = UNDEV_EVENTRECORD_NUMBER;
		}
		#endif
		api_WriteContinueMem( EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo );
	}
	else
	{
		return FALSE;
	}

	// 数据清零
	ClearEventData( tEventIndex );

	return TRUE;
}


//-----------------------------------------------
//函数功能: 获取掉电时间
//
//参数: 
//	PowerDownTime[out]:	掉电时间 
//  
//返回值:				无  
//备注:
//-----------------------------------------------
BOOL api_GetPowerDownTime(TRealTimer *pPowerDownTime)
{
	DWORD		dwAddr;
	TLostPowerTime LostPowerTime;

	dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.LostPowerTime );
	
	if(api_VReadSafeMem( dwAddr, sizeof(TLostPowerTime) , (BYTE*)&LostPowerTime.Time )==FALSE)
	{
		return FALSE;
	}

	if( api_CheckClock((TRealTimer *)&LostPowerTime.Time) == FALSE )
	{
		return FALSE;
	}
	
	memcpy( (BYTE*)pPowerDownTime, &LostPowerTime.Time, sizeof(TRealTimer));
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 保存工况类事件记录（失压失流等）
//
//参数: 
//	inEventIndex[in]:	eEVENT_INDEX 
//	Phase[in]:			ePHASE_TYPE  
//	BeforeAfter[in]:	事件发生前/发生后 eEVENT_TIME_TYPE 
//	EndTimeType[in]:	eEVENT_ENDTIME_CURRENT  当前时间
// 						eEVENT_ENDTIME_PWRDOWN  掉电时间
//  
//返回值:				TRUE/FALSE  
//备注:  保存原则  先存固定事件单元   再存关联属性
//-----------------------------------------------
BOOL SaveInstantEventRecord( BYTE inEventIndex, BYTE Phase, BYTE BeforeAfter, BYTE EndTimeType )
{
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;	
	BYTE i,Temp;
	BYTE *pAllocBuf;
	TRealTimer	Time;
	WORD SavePointer,Offset;
	DWORD dwOad,dwTemp,dwAddr,dwAddrtest = 0;
	BYTE Buf[16];	//大于TDemandData长度,不小于16

	if( inEventIndex >= eNUM_OF_EVENT)
	{
		return FALSE;
	}
	if(gReportMode.byEventValid[inEventIndex] == FALSE)
		return FALSE;


	EventAddrLen.EventIndex = inEventIndex;
	EventAddrLen.Phase = Phase;
	
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);

	if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
	{
		return FALSE;
	}

	Offset = 0;

	if( BeforeAfter == EVENT_START )
	{
		//第11~20条，保存起始地址是在第二个扇区
		//if( (EventDataInfo.RecordNo%(EventAttInfo.MaxRecordNo*EVENT_SECTOR_NUM)) >= EventAttInfo.MaxRecordNo )
		//{
		//	EventAddrLen.dwRecordAddr += SECTOR_SIZE;
		//}
		//该记录所在扇区的起始地址
		if(EventAttInfo.MaxRecordNo == 0)
		{
			return FALSE;
		}
		#if(PROTOCOL_VERSION == 25)
		if(inEventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
		{
			EventAddrLen.dwRecordAddr += ((EventDataInfo.RecordNo % (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo) * SECTOR_SIZE;
		}
		else
		#endif
		{
			EventAddrLen.dwRecordAddr += ((EventDataInfo.RecordNo % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo) * SECTOR_SIZE;
		}
		SavePointer = EventDataInfo.RecordNo%EventAttInfo.MaxRecordNo;

		memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) );

		EventDataInfo.RecordNo++;// 一直增加
		
		EventOADCommonData.EventNo = EventDataInfo.RecordNo;
		if( EndTimeType == eEVENT_ENDTIME_CURRENT )
		{
			// 以当前时间作为时间发生时间
			#if(PROTOCOL_VERSION == 25)
			if(inEventIndex == eSUB_EVENT_SPECIFIC_DEV_NO) //特定设备时间的时间由算法传递，时间合法性由算法保证
			{
				api_ReadMeterTime(&EventOADCommonData.EventTime.BeginTime );//记录当前时间
				dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );	// 发生时刻对应相对秒数
				dwTemp -= 15*60;	// 发生时刻对应相对秒数-15分钟
				dwTemp += g_DevInof.Message[g_SpeDevSaveCount].tDevTime;//发生时刻对应相对秒数加上真实发生的相对秒数
				api_SecToAbsTime(dwTemp, &EventOADCommonData.EventTime.BeginTime ); // 相对秒数转成绝对时间
			}
			else
			#endif //#if(PROTOCOL_VERSION == 25)
			{
				api_ReadMeterTime(&EventOADCommonData.EventTime.BeginTime );
			}

			#if(PROTOCOL_VERSION == 25)
			if((inEventIndex == eSUB_EVENT_METER_CLEAR_NO) || (inEventIndex == eSUB_EVENT_UNKNOWN_DEV_NO) || (inEventIndex == eSUB_EVENT_SPECIFIC_DEV_NO))
			#elif(PROTOCOL_VERSION == 24)
			if(inEventIndex == eSUB_EVENT_METER_CLEAR_NO)//清零事件特殊处理，防止读取时候指定结束时间读取问题
			#endif
			{
				//事件结束时间，防止读取时候指定结束时间读取问题
				dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );	// 发生时刻对应相对秒数
				dwTemp ++;// 发生时刻对应相对秒数+1
				api_SecToAbsTime(dwTemp, &EventOADCommonData.EventTime.EndTime ); // 相对秒数转成绝对时间
			}
		}
		else
		{
			if(api_GetPowerDownTime( (TRealTimer*)&EventOADCommonData.EventTime.BeginTime )==FALSE)
			{
				api_ReadMeterTime(&EventOADCommonData.EventTime.BeginTime );	
			}
		}

		#if(PROTOCOL_VERSION == 25)
		//309B事件发生源存储
		//TEventOADCommonData结构体自动补齐2字节，这2字节可以理解为事件发生源和通道上报状态，各默认1字节，都填0xff，真正获取数据的时候，这两个字节都是0。
		//309B事件发生源存储正好利用了TEventOADCommonData结构体自动补齐的2字节空间，把这两个字节定义为事件发生源，获取数据的时候，特殊处理，从flash读取。
		if(inEventIndex == eSUB_EVENT_SPECIFIC_DEV_NO)
		{
			EventOADCommonData.EventSource = g_DevInof.Message[g_SpeDevSaveCount].wDevID;
		}
		#endif
		
		// 必须先存时间,记录序号,上报状态，再存数据，否则在指针为0时会清整个扇区
		dwAddr = EventAddrLen.dwRecordAddr + EventAttInfo.AllDataLen*SavePointer;
		dwAddrtest = dwAddr;
		if(api_WriteMemRecordData( dwAddr, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData ) == FALSE)
		{
			return FALSE;
		}
		
		// 存开始OAD对应的数据
		if( EventAttInfo.NumofOad != 0 )
		{
			pAllocBuf = (BYTE*)api_AllocBuf( (WORD*)&EventAttInfo.OadValidDataLen );
			//先全置FF，这样结束数据存为FF，事件结束时再保存结束数据
			memset( pAllocBuf, 0xff, EventAttInfo.OadValidDataLen );

			for( i = 0; i < EventAttInfo.NumofOad; i++ )
			{
				Temp = (BYTE)((EventAttInfo.Oad[i]&0x00E00000)>>21);
				dwOad = EventAttInfo.Oad[i];
				dwOad &= ~0x00E00000;

				//OAD属性特征bit5~7:1：事件发生前 2：事件发生后 3：事件结束前 4：事件结束后
				if( (Temp != 3) && (Temp != 4) )//如果属性特征不为3,4，默认按照1,2,进行处理！！！！！！
				{
 					{
						api_GetProOadData( eData, 0xff, (BYTE *)&dwOad, NULL, MAX_APDU_SIZE, pAllocBuf+Offset );
					}
				}
				//这里针对下载方标识单独处理，原因是这里存的时候用的默认长度，这个默认长度跟实际下发的不符合，导致存储的时候后面的数据把原本下载标识覆盖掉一部分
				//这里测试时，默认值为3，实际为6，因此最后不正确，默认值为什么是3呢？原因是初始化事件的时候，如果下载标识是空，那么就填一个空格，ascll码是0x20，加上tag共计3个字节，写入了默认。
				//因此这里修改，在下载方标识的时候，存储的长度是实际的长度，例如1234，加上tag那就是6个字节。如果是电科院模式，DYK加tag是5个字节。
				if(dwOad == 0x000501F0)
				{
					Offset += strlen(tFileInfo.szDownFlag)+2;
				}
				else
				{
					Offset += EventAttInfo.OadLen[i];
				}

			}
			
			dwAddr = EventAddrLen.dwRecordAddr+sizeof(TEventOADCommonData)+EventAttInfo.AllDataLen*SavePointer;
			// 存开始OAD对应的数据
			if(api_WriteMemRecordData( dwAddr, EventAttInfo.OadValidDataLen, pAllocBuf )==FALSE)
			{
				api_FreeBuf(pAllocBuf);
				return FALSE;
			}
			api_FreeBuf(pAllocBuf);
		}	
		//存EE放在FLASH后，防止擦FLASH进入低功耗
		api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		//置位状态字
		api_DealEventCurrentStatus( eSET_EVENT_CURRENT_STATUS, EventAddrLen.SubEventIndex );
		
	 	api_DealEventDelayedStatus( eSET_EVENT_DELAYED_STATUS, EventAddrLen.SubEventIndex );

	//	#if(SEL_REPORT_FUNC == YES)
		if(EventAddrLen.EventIndex < eNUM_OF_EVENT_PRG)
		{
#if(USE_PARA_CTRL)
			if((gReportMode.byReportFlag[EventAddrLen.EventIndex] == 0x01) || (gReportMode.byReportFlag[EventAddrLen.EventIndex] == 3))
#endif
			{
				SetReportEventOAD( EventAddrLen.EventIndex, EventAddrLen.Phase );
				
				SetReportIndex( EventAddrLen.SubEventIndex, EVENT_START );	//wxy
			}
		}

		//测试用：报文打印
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 6) & 0x0001) == 0x0001)
		{
			rt_kprintf("start: record NO = %d, fixation data addr = 0x%4X, fixation len = %d, relevance data addr = 0x%4X, relevance len = %d, buf[0] = 0X%02X\n",
			EventDataInfo.RecordNo,dwAddrtest,sizeof(TEventOADCommonData),dwAddr,EventAttInfo.OadValidDataLen,pAllocBuf[0]);
		}
		#endif
	
	//	#endif
	}
	else//if( BeforeAfter == EVENT_START )
	{						
		if(EventDataInfo.RecordNo == 0)
		{
			return FALSE;
		}
			
		//第11~20条，保存起始地址是在第二个扇区
		//if( ((EventDataInfo.RecordNo-1)%(EventAttInfo.MaxRecordNo*2)) >= EventAttInfo.MaxRecordNo )
		//{
		//	EventAddrLen.dwRecordAddr += SECTOR_SIZE;
		//}
		if(EventAttInfo.MaxRecordNo == 0)
			return FALSE;
		EventAddrLen.dwRecordAddr += (((EventDataInfo.RecordNo - 1) % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo) * SECTOR_SIZE;
		
		SavePointer = (EventDataInfo.RecordNo-1)%EventAttInfo.MaxRecordNo;

		// 存结束OAD对应的数据
		if( EventAttInfo.NumofOad != 0 )
		{
			pAllocBuf = (BYTE*)api_AllocBuf( (WORD*)&EventAttInfo.OadValidDataLen );
			memset( pAllocBuf, 0xff, EventAttInfo.OadValidDataLen );

			dwAddr = EventAddrLen.dwRecordAddr+sizeof(TEventOADCommonData)+EventAttInfo.AllDataLen*SavePointer;
			api_ReadMemRecordData( dwAddr, EventAttInfo.OadValidDataLen, pAllocBuf );

			for( i = 0; i < EventAttInfo.NumofOad; i++ )
			{
				Temp = (BYTE)((EventAttInfo.Oad[i]&0x00E00000)>>21);
				dwOad = EventAttInfo.Oad[i];
				dwOad &= ~0x00E00000; 

				if( (Temp==3) || (Temp==4) )
				{  
 					{
						api_GetProOadData( eData, 0xff, (BYTE *)&dwOad, NULL, MAX_APDU_SIZE, pAllocBuf + Offset );
					}
				}
				
				if(dwOad == 0x000501F0)
				{
					Offset += strlen(tFileInfo.szDownFlag)+2;
				}
				else
				{
					Offset += EventAttInfo.OadLen[i];
				}
			}
			
			// 存结束OAD对应的数据
			if(api_WriteMemRecordData( dwAddr, EventAttInfo.OadValidDataLen, pAllocBuf )==FALSE)			
			{
				api_FreeBuf(pAllocBuf);
				return FALSE;
			}
			api_FreeBuf(pAllocBuf);
		}
		
		if( EndTimeType == eEVENT_ENDTIME_CURRENT )
		{
			// 当前时间
			api_ReadMeterTime( (TRealTimer*)Buf );									
		}
		else
		{
			// 掉电时间
			if(api_GetPowerDownTime( (TRealTimer*)Buf )==FALSE)
			{
				api_ReadMeterTime( (TRealTimer*)Buf );
			}
		}

		// 存结束时间
		dwAddr = EventAddrLen.dwRecordAddr + EventAttInfo.AllDataLen*SavePointer;
		dwAddr += GET_STRUCT_ADDR( TEventOADCommonData, EventTime.EndTime );
		dwAddrtest = dwAddr;
		if(api_WriteMemRecordData( dwAddr, sizeof(TRealTimer), Buf )==FALSE)																	
		{
			return FALSE;
		}

		dwAddr = EventAddrLen.dwRecordAddr + EventAttInfo.AllDataLen*SavePointer;
		dwAddr += GET_STRUCT_ADDR( TEventOADCommonData, EventTime.BeginTime );
		api_ReadMemRecordData( dwAddr, sizeof(TRealTimer) , (BYTE *)&Time );

		// 计算累计时间
		// 结束时间正确
		if( api_CheckClock((TRealTimer *)Buf) == TRUE )	
		{
			// 开始时间正确
			if( api_CheckClock(&Time) == TRUE )					
			{
				// 结束时间对应相对秒数
				dwTemp = api_CalcInTimeRelativeSec( (TRealTimer *)Buf );			
				// 发生时刻对应相对秒数
				dwAddr = api_CalcInTimeRelativeSec( &Time );							

				if( dwTemp >= dwAddr )
				{
					dwTemp = dwTemp - dwAddr;
				}
				else
				{
					dwTemp = 0;
				}
				EventDataInfo.AccTime += dwTemp;
				api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);	
			}
		}

		//置位状态字
		api_DealEventCurrentStatus( eCLEAR_EVENT_CURRENT_STATUS, EventAddrLen.SubEventIndex );
		
	 	api_DealEventDelayedStatus( eCLEAR_EVENT_DELAYED_STATUS, EventAddrLen.SubEventIndex );
		if(EventAddrLen.EventIndex < eNUM_OF_EVENT_PRG)
		{
#if (USE_PARA_CTRL)
			if((gReportMode.byReportFlag[EventAddrLen.EventIndex] == 0x2) || (gReportMode.byReportFlag[EventAddrLen.EventIndex] == 3))
#endif
			{
				SetReportEventOAD( EventAddrLen.EventIndex, EventAddrLen.Phase );
				
				SetReportIndex( EventAddrLen.SubEventIndex, EVENT_END );	//wxy
			}
		}
		//测试用：报文打印
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 6) & 0x0001) == 0x0001)
		{
			rt_kprintf("end: record NO = %d, fixation data addr = 0x%4X, fixation len = %d\n",
			EventDataInfo.RecordNo,dwAddrtest,sizeof(TRealTimer));
		}
		#endif
	}
	
	// 保存当前的事件状态
	api_WriteContinueMem( GET_CONTINUE_ADDR( EventConRom.PowerDownFlag.InstantEventFlag[0] ), GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag ), (BYTE *)&EventStatus.DelayedStatus[0] );
	return TRUE;
}

//-----------------------------------------------
//函数功能: 根据OAD读数据
//
//参数: 
//		pDLT698RecordPara[in]：指向结构体TDLT698RecordPara的指针              
//		pEventOADCommonData[in]：指向结构体TEventOADCommonData的指针                
//		pEventAttInfo[in]：指向结构体TEventAttInfo的指针                
//		pEventAddrLen[in]：指向结构体TEventAddrLen的指针                
//		dwPointer[in]：地址偏移指针                
//		pBuf[out]：返回数据的指针，有的传数据，没有的补0                
//		dwOad[in]：OAD                
//		Len[in]：输入的buf长度 用来判断buf长度是否够用                
//		Tag[in]：0 不加Tag  1 加Tag                
//		off[in]：已经有的偏移                
//		status[in]：事件结束标识                
//		AllorSep[in]：1 指定OAD 0 所有OAD
// 		Last[in]: 上N次
//返回值:	0xFFFF: 缓冲满或者错误 其他值：返回数据长度             
//备注: 
//-----------------------------------------------
static WORD GetDataByOad( TEventData *pData )
{
	DWORD dwTemp,dwTemp1,dwTemp2,dwOad;
	WORD wLen,wLen1;
	#if(PROTOCOL_VERSION == 25)
	WORD wSour = 0;
	#endif
	BYTE Temp,i,OadIndex;	
	BYTE *pAllocBuf,*pTemp;

	if( pData->dwOad == STARTTIME_OAD )
	{
		//若发生时间都为ff，则置为null
		if( lib_IsAllAssignNum( (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, 0xff, sizeof(TRealTimer) ) == TRUE )
		{
			if( (pData->off+1) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			wLen = 1;
			pData->pBuf[pData->off] = 0;
		}
		else
		{
			wLen = api_GetProOADLen( pData->Tag, (BYTE *)&pData->dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			
			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, MAX_APDU_SIZE, pData->pBuf+pData->off );
			}
			else
			{
				memcpy( pData->pBuf+pData->off, (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, wLen );		
			}
		}				
	}
	else if( pData->dwOad == ENDTIME_OAD )
	{
		//若结束时间都为ff，则置为null eNUM_OF_EVENT
		//校时记录，记录结束时间
		#if(PROTOCOL_VERSION == 25)
		if((lib_IsAllAssignNum((BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, 0xff, sizeof(TRealTimer)) == TRUE ) ||
		  (pData->pEventAddrLen->EventIndex == eSUB_EVENT_METER_CLEAR_NO) ||
		  (pData->pEventAddrLen->EventIndex == eSUB_EVENT_SPECIFIC_DEV_NO) ||
		  (pData->pEventAddrLen->EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO))
		#elif(PROTOCOL_VERSION == 24)
		if((lib_IsAllAssignNum((BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, 0xff, sizeof(TRealTimer)) == TRUE ) ||
		  (pData->pEventAddrLen->EventIndex == eSUB_EVENT_METER_CLEAR_NO))
		#endif //#if(PROTOCOL_VERSION == 25)
		{
			if( (pData->off+1) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			wLen = 1;
			pData->pBuf[pData->off] = 0;
		}
		else
		{
			wLen = api_GetProOADLen( pData->Tag, (BYTE *)&pData->dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			
			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, MAX_APDU_SIZE, pData->pBuf+pData->off );
			}
			else
			{
				memcpy( pData->pBuf+pData->off, (BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, wLen );		
			}
		}
	}
	else if( pData->dwOad == RECORDNO_OAD )
	{
		wLen = api_GetProOADLen( pData->Tag, (BYTE *)&pData->dwOad, 0 );
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// 已满
		}

		if( pData->pEventOADCommonData->EventNo != 0 )
		{
			dwTemp = pData->pEventOADCommonData->EventNo - 1;
		}
		else
		{
			dwTemp = 0;
		}
		
		if( pData->Tag == eTagData )
		{
			api_GetProOadData( eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&dwTemp, MAX_APDU_SIZE, pData->pBuf+pData->off );
		}
		else
		{
			memcpy( pData->pBuf+pData->off, (BYTE *)&dwTemp, wLen );
		}
	}
	else if( pData->dwOad == GEN_SOURCE_OAD )
	{
		// 工况类事件无事件发生源
		//if( ((pData->pDLT698RecordPara->OI>=0x3000) && (pData->pDLT698RecordPara->OI<= 0x3008)) )//接口类24  class_id=24
		if( (pData->pDLT698RecordPara->OI==OAD_HARMONIC_U_DISTORTION) 
			||(pData->pDLT698RecordPara->OI==OAD_HARMONIC_I_DISTORTION)
			||(pData->pDLT698RecordPara->OI==OAD_SHORT_FLICKER_U_OVERRUN)
			||(pData->pDLT698RecordPara->OI==OAD_LONG_FLICKER_U_OVERRUN)
			||(pData->pDLT698RecordPara->OI==OAD_SAG_U)
			||(pData->pDLT698RecordPara->OI==OAD_SWELL_U)
			||(pData->pDLT698RecordPara->OI==OAD_INTERRUPTION_U) )
		{
			if( pData->AllorSep == 0 )
			{
				//wxy  wLen = 0;

				wLen = 1;
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// 已满
				}

				pData->pBuf[pData->off] = 0;	// 只传一个0，代表NULL
			}
			else
			{
				wLen = 1;
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// 已满
				}

				pData->pBuf[pData->off] = 0;	// 只传一个0，代表NULL
			}
		}
		#if(PROTOCOL_VERSION == 25)
		else if(pData->pDLT698RecordPara->OI == OAD_SPECIFIC_DEV)
		{
			dwTemp = pData->dwPointer*pData->pEventAttInfo->AllDataLen;
			dwTemp += pData->pEventAddrLen->dwRecordAddr + sizeof(TEventOADCommonData) - sizeof(wSour);	
			wSour = 0;
			api_ReadMemRecordData(dwTemp, sizeof(wSour), (BYTE*)&wSour);
			
			if( pData->Tag == eTagData)
			{
				wLen = 3;
				if( (pData->off+wLen) > (pData->Len+1) )
				{
					return 0xFFFF;// 已满
				}
				pData->pBuf[pData->off] = 18;//Long_unsigned_698
				lib_ExchangeData(&pData->pBuf[pData->off+1], (BYTE *)&wSour, sizeof(wSour)); // 传入真实事件发生源
			}
			else
			{
				wLen = 2;
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// 已满
				}
				lib_ExchangeData(&pData->pBuf[pData->off], (BYTE *)&wSour, sizeof(wSour)); // 传入真实事件发生源
			}
		}
		#endif
		else
		{
			wLen = 1;
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}

			pData->pBuf[pData->off] = 0;	// 只传一个0，代表NULL
		}
	}
	else if( pData->dwOad == NOTIFICATION_OAD )
	{
		//补遗8要求电表可不支持通道上报，传NULL
		pData->pBuf[pData->off] = 0;
		wLen = 1;
	}
	else
	{
		if(((pData->dwOad == 0x002501F0)||(pData->dwOad == 0x00232740)||\
			(pData->dwOad == 0x042202F4)||(pData->dwOad == 0x048202F4)||\
			(pData->dwOad == 0x00822740)) && (gDownFlagCount == 0))
		{
			gDownFlagCount = 1;
			dwTemp = pData->dwPointer*pData->pEventAttInfo->AllDataLen;
			dwTemp += pData->pEventAddrLen->dwRecordAddr + sizeof(TEventOADCommonData);	
			memset( &pAllocBuftemp[0], 0x00, pData->pEventAttInfo->OadValidDataLen+1 );//升级事件是300
			api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadValidDataLen, &pAllocBuftemp[0]);
			dwTemp = 0;
			gDownFlagLen = pAllocBuftemp[3];//下载标识的长度
		}


		//读关联属性部分  非固定列部分
		dwTemp1 = 0;
		OadIndex = 0;
		for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
		{
			if( pData->pEventAttInfo->Oad[i] == pData->dwOad )
			{
				break;
			}
            if(0x002501F0 == pData->pEventAttInfo->Oad[i])
            {
				gDownFlagFlash = 0x01;
                wLen = api_GetProOADLen( pData->Tag, (BYTE *)&pData->pEventAttInfo->Oad[i], 0 );
                dwTemp1 += wLen;
            }
            else
            {
                dwTemp1 += pData->pEventAttInfo->OadLen[i]; 
            }

		}

		// 没找到指定OAD
		if( i == pData->pEventAttInfo->NumofOad )
		{
			dwOad = pData->dwOad;
			OadIndex = (BYTE)(dwOad>>24);

			if( OadIndex != 0 )
			{
				dwTemp1 = 0;
				dwOad &= ~0xFF000000;
				// 索引置为0再找一遍
				for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
				{
					if( pData->pEventAttInfo->Oad[i] == dwOad )
					{
						Temp = 2;
						break;
					}
					dwTemp1 += pData->pEventAttInfo->OadLen[i];
				}
			}
		}

		// 找到OAD
		if( i != pData->pEventAttInfo->NumofOad )
		{
			dwOad = pData->dwOad;
			if(0x002501F0 == dwOad)
			{
				gDownFlagFlash = 0x01;
				wLen = api_GetProOADLen( pData->Tag, (BYTE *)&dwOad, 0 );
			}
			else
			{
				wLen = api_GetProOADLen( pData->Tag, (BYTE *)&dwOad, 0 );
			}
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}

			if( pData->pEventAttInfo->OadLen[i] > EVENT_VALID_OADLEN )
			{
				return 0xFFFF;
			}

			pAllocBuf = (BYTE *)api_AllocBuf( (WORD*)&(pData->pEventAttInfo->OadLen[i]) );

			if( pData->pEventAddrLen->EventIndex <= ePRG_RECORD_NO )
			{
				Temp = (BYTE)((pData->dwOad&0x00E00000)>>21);
				
				if( (pData->Last==1) && (pData->status==1) && ((Temp==3)||(Temp==4)) )
				{
					{
						pData->pBuf[pData->off] = 0;//null
						api_FreeBuf(pAllocBuf);
						return 1;
					}

				}
				else
				{
					#if(PROTOCOL_VERSION == 25)
                    if(pData->pDLT698RecordPara->OI == OAD_UNKNOWN_DEV)
                    {
                        dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+(pData->dwPointer % MAX_EVENTRECORD_NUMBER)*pData->pEventAttInfo->AllDataLen;
                        dwTemp += dwTemp1;
                    }
                    else
					#endif
                    {
                        dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->dwPointer*pData->pEventAttInfo->AllDataLen;
                        dwTemp += dwTemp1;
                    }
                    
                    if(0x002501F0 == dwOad)
                    {
                        api_ReadMemRecordData(dwTemp, wLen, pAllocBuf);
                    }
                    else
                    {
                        api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], pAllocBuf);
                    }
                 }
			}
			else
			{
				dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->dwPointer*pData->pEventAttInfo->AllDataLen;
				dwTemp += dwTemp1;
				api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], pAllocBuf);
			}

			if( OadIndex != 0 )
			{
				pTemp = pAllocBuf;
				dwTemp = dwOad;

				for( i = 1; i < OadIndex; i++ ) // pData->OadIndex 从1开始
				{
					dwTemp2 = i;
					dwOad = (dwTemp&0X00FFFFFF);//先去掉元素索引
					dwOad |= (dwTemp2<<24);//置元素索引
					wLen1 = api_GetProOADLen( eData, (BYTE *)&dwOad, 0 );
					if( wLen1 == 0x8000 )
					{
						api_FreeBuf(pAllocBuf);
                        return 0x8000;
					}
					pTemp += wLen1;
				}

				dwOad = ((dwOad&0x00ffffff) | (OadIndex<<24));//置元素索引
			}
			else
			{
				pTemp = pAllocBuf;
			}

			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eAddTag, 0xff, (BYTE *)&dwOad, pTemp, MAX_APDU_SIZE, pData->pBuf+pData->off );
			}
			else
			{
				memcpy( pData->pBuf+pData->off, pTemp, wLen );
			}

			api_FreeBuf(pAllocBuf);
		}
		else
		{
			wLen = 1;
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			pData->pBuf[pData->off] = 0;
		}
	}

	return wLen;
}

//-----------------------------------------------
//函数功能: 根据OAD读数据
//
//参数: 
//		pDLT698RecordPara[in]：		指向结构体TDLT698RecordPara的指针              
//		pEventOADCommonData[in]：	指向结构体TEventOADCommonData的指针                
//		pEventAttInfo[in]：			指向结构体TEventAttInfo的指针                
//		pEventAddrLen[in]：			指向结构体TEventAddrLen的指针                
//		dwPointer[in]：				地址偏移指针                
//		pBuf[out]：					返回数据的指针，有的传数据，没有的补0                
//		dwOad[in]：					OAD                
//		Len[in]：					输入的buf长度 用来判断buf长度是否够用                
//		Tag[in]：					0 不加Tag  1 加Tag                
//		off[in]：					已经有的偏移                
//		status[in]：				事件结束标识                
//		AllorSep[in]：				1 指定OAD 0 所有OAD
//返回值:							bit15位 置1 错误 置0正确； 
//									bit0~bit14 代表返回的数据长度             
//备注: 
//-----------------------------------------------
static WORD ReadEventRecordSub( TEventData *pData )
{
	DWORD dwOad,dwTemp;
	WORD wLen;
	// TTwoByteUnion ManageUpDatelen;
	BYTE i,j,Number,Temp;
	BYTE *pTemp,*pAllocBuf;
	
	// 找OI对应固定列的OAD
	for( i = 0; i < (sizeof(EventOadTab)/sizeof(TEventOadTab)); i++ )
	{
		if( pData->pDLT698RecordPara->OI == EventOadTab[i].OI )
		{
			break;
		}
	}

	if( i < (sizeof(EventOadTab)/sizeof(TEventOadTab)) )
	{
		pTemp = (BYTE *)&EventOadTab[i].OadList[0];	// 找到
		Number = EventOadTab[i].Number;
	}
	else
	{
		// 默认Class id 7 工况类事件，操作类编程
		pTemp = (BYTE *)&EventOadTab[0].OadList[0];	
		Number = EventOadTab[0].Number;
	}
    #if(PROTOCOL_VERSION == 25)
    if( (pData->pDLT698RecordPara->OI == OAD_METER_CLEAR)
       || (pData->pDLT698RecordPara->OI == OAD_JLCHIP_FAIL)
       || (pData->pDLT698RecordPara->OI == OAD_APP_UPDATE)
	   || (pData->pDLT698RecordPara->OI == OAD_ADJUST_TIME)
	   || (pData->pDLT698RecordPara->OI == OAD_UNKNOWN_DEV)
	   || (pData->pDLT698RecordPara->OI == OAD_SPECIFIC_DEV))
	#elif(PROTOCOL_VERSION == 24)
    if( (pData->pDLT698RecordPara->OI == OAD_METER_CLEAR)
   || (pData->pDLT698RecordPara->OI == OAD_JLCHIP_FAIL)
   || (pData->pDLT698RecordPara->OI == OAD_APP_UPDATE)
   || (pData->pDLT698RecordPara->OI == OAD_ADJUST_TIME))
	#endif
	{
    	// 默认Class id 7 工况类事件，操作类编程
		pTemp = (BYTE *)&EventOadTab7[0].OadList[0];	
		Number = EventOadTab7[0].Number;
    }
        
        
	pData->off = 0;
    gDownFlagCount = 0;

	if( pData->pDLT698RecordPara->OADNum != 0 )
	{
		// if( pData->pDLT698RecordPara->OI == 0x3036 )
		// {
		// 	//管理芯升级事件只允许RCSD为0
		// 	return 0;
		// }
		
		pData->AllorSep = 1;
		// 指定OAD
		for( i = 0; i < pData->pDLT698RecordPara->OADNum; i++ )
		{		    
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
			pData->dwOad = dwOad;

			//判断此OAD是否为固定列OAD
			for( j = 0; j < sizeof(FixedColumnOadTab)/sizeof(DWORD); j++ )
			{
				if( FixedColumnOadTab[j] == dwOad )
				{
					break;
				}
			}

			if( j < sizeof(FixedColumnOadTab)/sizeof(DWORD) )
			{
				//判断此OAD是否为此事件记录支持的固定列OAD
				for( j = 0; j < Number; j++ )
				{
					memcpy( (BYTE *)&dwTemp, pTemp+sizeof(DWORD)*j, sizeof(DWORD) );
					if( dwTemp == dwOad )
					{
						break;
					}
				}

				if( j < Number )
				{
					// 是此OI对应的固定列
					wLen = GetDataByOad( pData );

					if( wLen == 0xFFFF )
					{
						return 0; 	// 缓冲满
					}
				}
				else
				{
					wLen = 1;
					if( (pData->off + wLen) > pData->Len )
					{
						return 0; 	// 缓冲满	
					}

					*(pData->pBuf+pData->off) = 0;// NULL 不支持的固定列
				}
			}
			else
			{
				// 找OAD列表
				wLen = GetDataByOad( pData );

				if( wLen == 0xFFFF )
				{
					return 0; 	// 缓冲满
				}
			}
			pData->off += wLen;
		}
	}
	else
	{
		// 固定列    非关联属性表部分
		for( i = 0; i < Number; i++ )
		{
			memcpy( (BYTE *)&dwOad, pTemp+sizeof(DWORD)*i, sizeof(DWORD) );
			pData->dwOad = dwOad;
			
			wLen = GetDataByOad( pData );//数据+TAG的长度

			if( wLen == 0xFFFF )
			{
				return 0; 	// 缓冲满
			}

			pData->off += wLen;
		}

		// 一次将数据全部读出，传进来的数据地址已经考虑两个扇区问题
		#if(PROTOCOL_VERSION == 25)
		if(pData->pDLT698RecordPara->OI == OAD_UNKNOWN_DEV)
		{
			dwTemp = (pData->dwPointer % MAX_EVENTRECORD_NUMBER)*pData->pEventAttInfo->AllDataLen;
			dwTemp += pData->pEventAddrLen->dwRecordAddr + sizeof(TEventOADCommonData);	
		}
		else
		#endif
		{
			dwTemp = pData->dwPointer*pData->pEventAttInfo->AllDataLen;
			dwTemp += pData->pEventAddrLen->dwRecordAddr + sizeof(TEventOADCommonData);	
		}
		
		// 关联对象属性列表
		if( pData->pEventAttInfo->NumofOad == 0 )
		{
			return pData->off;
		}

		if( pData->pEventAttInfo->AllDataLen > EVENT_VALID_OADLEN )
		{
			return 0x8000;
		}

	/*wxy	if( pData->pDLT698RecordPara->OI == 0x3036 )
		{
			//管理芯升级事件数据格式与其它事件不同
			api_ReadMemRecordData(dwTemp, 2, &(ManageUpDatelen.b[0]));
			if( (pData->off + ManageUpDatelen.w) > pData->Len )
			{
				return 0;// 已满
			}
			api_ReadMemRecordData(dwTemp+2, ManageUpDatelen.w, pData->pBuf+pData->off);
			pData->off += ManageUpDatelen.w;
			return pData->off;
		}*/
	
		// +1防止为0【时段表编程时可能为0】
		wLen = pData->pEventAttInfo->OadValidDataLen+1;
		pAllocBuf = (BYTE *)api_AllocBuf( (WORD*)&wLen );
		memset( pAllocBuf, 0x00, pData->pEventAttInfo->OadValidDataLen );
		api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadValidDataLen, pAllocBuf);
 
		dwTemp = 0;
		//非固定列  关联属性部分
		for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
		{		    
			dwOad = pData->pEventAttInfo->Oad[i];
			if(dwOad == 0x002501F0)//判断是否为下载方标识
			{
				gDownFlagLen = pAllocBuf[3];//下载标识的长度
				gDownFlagFlash = 0x01;
			}
			wLen = api_GetProOADLen( pData->Tag, (BYTE *)&dwOad, 0 );// 虽然未结束时填null只需要一个字节，这里不修改此函数，唯一的影响是提前分帧
			if( (pData->off+wLen) > pData->Len )
			{
				api_FreeBuf(pAllocBuf);
				return 0;// 已满
			}
			
			if( pData->pEventAddrLen->EventIndex <= ePRG_RECORD_NO )
			{
				Temp = (BYTE)((pData->pEventAttInfo->Oad[i]&0x00E00000)>>21);
				// 上1次 && 事件还未结束 && 结束时刻OAD   返回0
				if( (pData->Last==1) && (pData->status==1) && ((Temp==3)||(Temp==4)) )
				{
					{
						// 事件还没有结束,结束数据置null
						dwTemp+= pData->pEventAttInfo->OadLen[i];
						*(pData->pBuf+pData->off)=0;//NULL
						pData->off+=1;
						continue;
					}
				}
			}

			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eAddTag, 0xff, (BYTE *)&dwOad, pAllocBuf+dwTemp, MAX_APDU_SIZE, pData->pBuf+pData->off );
			}
			else
			{
				memcpy( pData->pBuf+pData->off, pAllocBuf+dwTemp, wLen );
			}
			
            //读取数据的时候这里也要匹配对
            if(0x002501F0 == dwOad)
            {
                dwTemp += wLen;//这个长度是从EEP读出来的，只要写的长度对，这个读出来的就没问题
            }
            else
            {
                dwTemp += pData->pEventAttInfo->OadLen[i];// EEP数据偏移
            }

			pData->off += wLen;
		}
		api_FreeBuf(pAllocBuf);
	}

	return pData->off;
}

//-----------------------------------------------
//函数功能: 以时间方式读工况类事件记录
//
//参数: 
//		Tag[in]:				0 不加Tag 1 加Tag                
//		pDLT698RecordPara[in]: 	读取冻结的参数结构体                
//  	Len[in]：				输入的buf长度 用来判断buf长度是否够用                
//  	pBuf[out]: 				返回数据的指针，有的传数据，没有的补0
//返回值:						bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//								bit0~bit14 代表返回的冻结数据长度
//备注: 在已存的时间里搜索，只搜索时间相等的记录
//-----------------------------------------------
static WORD ReadEventRecordByTime( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventOADCommonData EventOADCommonDataBak;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	DWORD dwTemp,dwPointer,dwPointerBak;
	#if(PROTOCOL_VERSION == 25)
	WORD i,twCurrentRecordNo;
	BYTE j = 0,status;
	#elif(PROTOCOL_VERSION == 24)
	WORD twCurrentRecordNo;
	BYTE i,status;
	#endif
	TRealTimer	Time;
	TRealTimer	EventTime,EventTimeBak;//以后优化为指针更好zym

	dwPointerBak = 0;
	dwPointer = 0;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//根据当前记录号为0，且当前状态为事件没发生
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}
	#if(PROTOCOL_VERSION == 25)
	if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
	{
		if( EventDataInfo.RecordNo > UNDEV_EVENT_DEEP )
		{
			twCurrentRecordNo = UNDEV_EVENT_DEEP;
		}
		else
		{
			twCurrentRecordNo = EventDataInfo.RecordNo;
		}	
	}
	else
	#endif
	{
		if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
		{
			twCurrentRecordNo = EventAttInfo.MaxRecordNo;
		}
		else
		{
			twCurrentRecordNo = EventDataInfo.RecordNo;
		}
	}

	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	
	//事件要读取的时标单位为秒（相对秒数）
	api_ToAbsTime( pDLT698RecordPara->TimeOrLast/60, (TRealTimer *)&Time );
	Time.Sec = pDLT698RecordPara->TimeOrLast%60;
	// 在已有的记录里搜索 可能已经记录了100条 但是上11-99条用户是没办法看到的
	for( i = 1; i <= twCurrentRecordNo; i++ )
	{
		//得到上i条记录的起始地址 1、得该事件基地址 2、得上i条记录所在扇区起始地址 3、上i条记录的起始地址=所在扇区地址+偏移长度

		dwPointer = (EventDataInfo.RecordNo - i) % EventAttInfo.MaxRecordNo;
		//得到上i条记录的起始地址 1、得该事件基地址 2、得上i条记录所在扇区起始地址 3、上i条记录的起始地址=所在扇区地址+偏移长度
		dwTemp = EventAddrLen.dwRecordAddr;
		#if(PROTOCOL_VERSION == 25)
		if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
		{
			dwTemp += (((EventDataInfo.RecordNo - i) % (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo) * SECTOR_SIZE;
		}
		else
		#endif
		{
			dwTemp += (((EventDataInfo.RecordNo - i) % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo) * SECTOR_SIZE;
		}
		dwTemp += EventAttInfo.AllDataLen*dwPointer;
		api_ReadMemRecordData(dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
		
		if( pDLT698RecordPara->eTimeOrLastFlag == eEVENT_START_TIME_FLAG )
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.BeginTime, sizeof(TRealTimer) );
		}
		else
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.EndTime, sizeof(TRealTimer) );
		}
		
		//第一次查找，先判断要读的时间与最近一次的大小关系，若比最近一次记录的时间大，直接返回错误
		if( i == 1 )
		{
			if( api_CompareTwoTime( &Time, &EventTime ) > 0 )
			{
				return 0x8000;// 没找到相应时间
			}
			//第一次查找，初始化EventTimeBak为EventTime
			memcpy( (BYTE *)&EventTimeBak, (BYTE *)&EventTime, sizeof(TRealTimer) );
		}
		
		if( api_CompareTwoTime( &Time, &EventTime ) == 0 )
		{
			//若读出的时间与查找的时间相等，直接中断查询
			break;
		}
		else if( (api_CompareTwoTime( &Time, &EventTime )> 0)
			&& (api_CompareTwoTime( &Time, &EventTimeBak )<= 0 ) )
		{
			//若查找的时间大于此次读出的时间，小于上次读出的时间，则中断查询
			i -= 1;//i肯定不为1
			memcpy( (BYTE *)&EventOADCommonData, (BYTE *)&EventOADCommonDataBak, sizeof(TEventOADCommonData) );		
			dwPointer = dwPointerBak;
			break;
		}

		memcpy( (BYTE *)&EventTimeBak, (BYTE *)&EventTime, sizeof(TRealTimer) );
		memcpy( (BYTE *)&EventOADCommonDataBak, (BYTE *)&EventOADCommonData, sizeof(TEventOADCommonData) );	
		dwPointerBak = 	dwPointer;
	}

	if( i > twCurrentRecordNo )
	{
		//查找完后，若要读的时间小于最后一次的时间，则返回此次的数据
		if( api_CompareTwoTime( &Time, &EventTime ) < 0 )
		{
			i = twCurrentRecordNo;
		}
		else
		{
			return 0x8000;// 没找到相应时间
		}
	}
	
	//第11~20条，保存起始地址是在第二个扇区
	//if( ((EventDataInfo.RecordNo-i)%(MAX_EVENTRECORD_NUMBER*2)) >= MAX_EVENTRECORD_NUMBER )
	//if( ((EventDataInfo.RecordNo-i)%(EventAttInfo.MaxRecordNo*2)) >= EventAttInfo.MaxRecordNo )
	//{
	//	EventAddrLen.dwRecordAddr += SECTOR_SIZE;
	//}
	#if(PROTOCOL_VERSION == 25)
	if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
	{
		EventAddrLen.dwRecordAddr += (((EventDataInfo.RecordNo - i) % (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo) * SECTOR_SIZE;
	}
	else
	#endif
	{
		EventAddrLen.dwRecordAddr += (((EventDataInfo.RecordNo - i) % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo) * SECTOR_SIZE;
	}
	Data.Last = i;
	Data.pEventOADCommonData = &EventOADCommonData;

	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		#if(PROTOCOL_VERSION == 25)
		j = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventDelayedStatus( eGET_EVENT_DELAYED_STATUS, j );
		#elif(PROTOCOL_VERSION == 24)
		i = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventDelayedStatus( eGET_EVENT_DELAYED_STATUS, i );
		#endif	
	}
	
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pEventAttInfo = &EventAttInfo;
	Data.pEventAddrLen = &EventAddrLen;
	Data.dwPointer = dwPointer;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.Tag = Tag;
	Data.status = status;
	Data.AllorSep = 0;

	Data.dwMaxRecorder = EventDataInfo.RecordNo;

	return ReadEventRecordSub(&Data);
}

static WORD ReadEventRecordByTimeInterval( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	DWORD dwTemp,dwPointer;
	#if(PROTOCOL_VERSION == 25)
	WORD i,twCurrentRecordNo;
	BYTE status, Offset;
	#elif(PROTOCOL_VERSION == 24)
	WORD twCurrentRecordNo;
	BYTE i, status, Offset;
	#endif
	TRealTimer	SaveStartTime,SaveEndTime,ProSearchStartTime,ProSearchEndTime,SearchStartTime,SearchEndTime;
	TRealTimer	EventTime;//以后优化为指针更好zym
	WORD wLen = 0;	//读一次事件记录得到的数据长度，若读取的事件未结束，则长度短
	WORD ReturnLen = 0;	//返回数据总长度
	DWORD dwRecordAddrBak;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//根据当前记录号为0，且当前状态为事件没发生
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}

	#if(PROTOCOL_VERSION == 25)
	if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
	{
		if( EventDataInfo.RecordNo > UNDEV_EVENT_DEEP )
		{
			twCurrentRecordNo = UNDEV_EVENT_DEEP;
		}
		else
		{
			twCurrentRecordNo = EventDataInfo.RecordNo;
		}	
	}
	else
	#endif
	{
		if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
		{
			twCurrentRecordNo = EventAttInfo.MaxRecordNo;
		}
		else
		{
			twCurrentRecordNo = EventDataInfo.RecordNo;
		}
	}
	
	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	
	if( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_START_TIME_FLAG) )
	{
		Offset = GET_STRUCT_ADDR(TEventOADCommonData, EventTime.BeginTime);
	}
	else
	{
		Offset = GET_STRUCT_ADDR(TEventOADCommonData, EventTime.EndTime);
	}
			
	//找到事件存储的最近一次时间，根据eTimeOrLastFlag，可以是时间发生时间或结束时间
	dwTemp = EventAddrLen.dwRecordAddr;
	
	//第11~20条，保存起始地址是在第二个扇区
	//if( ((EventDataInfo.RecordNo-1)%(EventAttInfo.MaxRecordNo*2)) >= EventAttInfo.MaxRecordNo )
	//{
	//	dwTemp += SECTOR_SIZE;
	//}
	#if(PROTOCOL_VERSION == 25)
	if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
	{
		dwTemp += ((EventDataInfo.RecordNo - 1) % (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;	
	}
	else
	#endif
	{
		dwTemp += ((EventDataInfo.RecordNo - 1) % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
	}
	dwPointer = (EventDataInfo.RecordNo-1)%EventAttInfo.MaxRecordNo;
	dwTemp += (EventAttInfo.AllDataLen*dwPointer+Offset);
	api_ReadMemRecordData(dwTemp, sizeof(TRealTimer), (BYTE *)&SaveEndTime );
	
	//最开始一条事件的时间，根据eTimeOrLastFlag，可以是时间发生时间或结束时间
	dwTemp = EventAddrLen.dwRecordAddr;
	
	//第11~20条，保存起始地址是在第二个扇区
	//if( ((EventDataInfo.RecordNo-twCurrentRecordNo)%(EventAttInfo.MaxRecordNo*2)) >= EventAttInfo.MaxRecordNo )
	//{
	//	dwTemp += SECTOR_SIZE;
	//}
	#if(PROTOCOL_VERSION == 25)
	if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
	{
		dwTemp += ((EventDataInfo.RecordNo - twCurrentRecordNo) % (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
	}
	else
	#endif
	{
		dwTemp += ((EventDataInfo.RecordNo - twCurrentRecordNo) % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
	}
	dwPointer = (EventDataInfo.RecordNo-twCurrentRecordNo)%EventAttInfo.MaxRecordNo;
	dwTemp += (EventAttInfo.AllDataLen*dwPointer+Offset);
	api_ReadMemRecordData(dwTemp, sizeof(TRealTimer), (BYTE *)&SaveStartTime );
		
	//要查找的起始时间大于等于查找的截止时间，返回错误
	if( pDLT698RecordPara->TimeIntervalPara.StartTime >= pDLT698RecordPara->TimeIntervalPara.EndTime )
	{
		return 0x8000;
	}
	
	//事件要读取的时标单位为秒（相对秒数）
	api_ToAbsTime( pDLT698RecordPara->TimeIntervalPara.StartTime/60, (TRealTimer *)&ProSearchStartTime );
	ProSearchStartTime.Sec = pDLT698RecordPara->TimeIntervalPara.StartTime%60;
	
	api_ToAbsTime( pDLT698RecordPara->TimeIntervalPara.EndTime/60, (TRealTimer *)&ProSearchEndTime );
	ProSearchEndTime.Sec = pDLT698RecordPara->TimeIntervalPara.EndTime%60;
		
	memcpy( (BYTE*)&SearchStartTime, (BYTE*)&ProSearchStartTime, sizeof(TRealTimer) );

	memcpy( (BYTE*)&SearchEndTime, (BYTE*)&ProSearchEndTime, sizeof(TRealTimer) );
	
    Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum = 0;

	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		
        status = api_DealEventDelayedStatus( eGET_EVENT_DELAYED_STATUS, GetSubEventIndex((TEventAddrLen*)&EventAddrLen) );	
    }
	
	// 在已有的记录里搜索
    for( i = twCurrentRecordNo; i >= 1; i-- )
	{
		//按结束时间读，若最近一次事件未结束，则跳过本次循环
		if( (i == 1) && status && (pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_END_TIME_FLAG)) )
		{
			continue;
		}
		
			dwPointer = (EventDataInfo.RecordNo - i) % EventAttInfo.MaxRecordNo;		
			//第11~20条，保存起始地址是在第二个扇区
			//if( ((EventDataInfo.RecordNo-i)%(MAX_EVENTRECORD_NUMBER*2)) >= MAX_EVENTRECORD_NUMBER )
			dwTemp = EventAddrLen.dwRecordAddr;
			//if( ((EventDataInfo.RecordNo-i)%(EventAttInfo.MaxRecordNo*2)) >= EventAttInfo.MaxRecordNo )
			//{
			//	dwTemp += SECTOR_SIZE;
			//}
			#if(PROTOCOL_VERSION == 25)
			if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
			{
				dwTemp += ((EventDataInfo.RecordNo - i) % (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
			}
			else
			#endif
			{
				dwTemp += ((EventDataInfo.RecordNo - i) % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
			}
			dwTemp += EventAttInfo.AllDataLen*dwPointer;
			api_ReadMemRecordData(dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );

		
        memset( (BYTE*)&EventTime, 0x00, sizeof(EventTime) );
        
		if( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_START_TIME_FLAG) )
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.BeginTime, sizeof(TRealTimer) );			
		}
		else
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.EndTime, sizeof(TRealTimer) );
		}

		//判断要读的时间区间（左闭右开）与最近一次的大小关系，若超出时间区间，则直接返回错误		
	    if( (api_CompareTwoTime( &EventTime,&SearchStartTime )<0)
	      || (api_CompareTwoTime( &EventTime, &SearchEndTime )>=0) )
		{
			continue;// 没找到相应时间
		}

		//第11~20条，保存起始地址是在第二个扇区
		//if( ((EventDataInfo.RecordNo-i)%(MAX_EVENTRECORD_NUMBER*2)) >= MAX_EVENTRECORD_NUMBER )
		//if( ((EventDataInfo.RecordNo-i)%(EventAttInfo.MaxRecordNo*2)) >= EventAttInfo.MaxRecordNo )
		//{
		//	EventAddrLen.dwRecordAddr += SECTOR_SIZE;
		//}
		dwRecordAddrBak = EventAddrLen.dwRecordAddr;
		#if(PROTOCOL_VERSION == 25)
		if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
		{
			EventAddrLen.dwRecordAddr += ((EventDataInfo.RecordNo - i) % (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
		}
		else
		#endif
		{
			EventAddrLen.dwRecordAddr += ((EventDataInfo.RecordNo - i) % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
		}
		
		Data.pEventAttInfo = &EventAttInfo;
		Data.pEventAddrLen = &EventAddrLen;
		Data.dwPointer = dwPointer;
		Data.Tag = Tag;
		Data.AllorSep = 0;	
		Data.Last = i;
		Data.pEventOADCommonData = &EventOADCommonData;

		if( i == 1)	//最近一次事件需要判断事件是否结束
		{
			Data.status = status;
		}
		else
		{
			Data.status = 0;	//非最近一次事件默认已结束
		}
			
		wLen = ReadEventRecordSub(&Data);
		if( wLen == 0x8000 )
	    {
            return 0x8000;
	    }

	    if( wLen == 0 )
	    {
            return ReturnLen;
	    }
	    
	    if( (ReturnLen+wLen) > Len )
	    {
            return ReturnLen;
	    }
	    ReturnLen += wLen;
	    
		Data.pBuf += wLen;

		if( Data.Len < wLen )
		{
            return ReturnLen;
		}
		Data.Len -= wLen;
		
		Data.pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum++;
		
		//if( ((EventDataInfo.RecordNo-i)%(EventAttInfo.MaxRecordNo*2)) >= EventAttInfo.MaxRecordNo )
		//{
		//	//若本次循环执行过EventAddrLen.dwRecordAddr += SECTOR_SIZE，则应恢复EventAddrLen.dwRecordAddr的值
		//	EventAddrLen.dwRecordAddr -= SECTOR_SIZE;
		//}
		EventAddrLen.dwRecordAddr = dwRecordAddrBak;
	}

	if( ReturnLen == 0 )
	{
		return 0x8000;// 没找到相应时间
	}
	
	return ReturnLen;
}

//-----------------------------------------------
//函数功能: 读事件记录
//
//参数: 
//		Tag[in]:				0 不加Tag 1 加Tag                
//		pDLT698RecordPara[in]: 	读取冻结的参数结构体                
//  	Len[in]：				输入的buf长度 用来判断buf长度是否够用                
//  	pBuf[out]: 				返回数据的指针，有的传数据，没有的补0	
//返回值:			bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//					bit0~bit14 代表返回的冻结数据长度
//备注: 只支持上Last条记录
//-----------------------------------------------
static WORD ReadEventRecordByNo( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	DWORD dwTemp;
	WORD twCurrentRecordNo,twSavePointer;
	BYTE i,status;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//根据当前记录号为0，且当前状态为事件没发生
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}
	#if(PROTOCOL_VERSION == 25)
	if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
	{
		if( EventDataInfo.RecordNo > UNDEV_EVENT_DEEP )
		{
			twCurrentRecordNo = UNDEV_EVENT_DEEP;
		}
		else
		{
			twCurrentRecordNo = EventDataInfo.RecordNo;
		}	
	}
	else
	#endif
	{
		if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
		{
			twCurrentRecordNo = EventAttInfo.MaxRecordNo;
		}
		else
		{
			twCurrentRecordNo = EventDataInfo.RecordNo;
		}
	}
	
	//清此通道新增事件列表中的指定事件
	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	
	if( (pDLT698RecordPara->TimeOrLast > twCurrentRecordNo) || (pDLT698RecordPara->TimeOrLast==0) )// 包括无数据的情况, 上0次返回无数据
	{
		return 0x8000;//无数据
	}
	
	twSavePointer = EventDataInfo.RecordNo%EventAttInfo.MaxRecordNo;
	#if(PROTOCOL_VERSION == 25)
	if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
	{
		twSavePointer += (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM - pDLT698RecordPara->TimeOrLast);
	}
	else
	{
		twSavePointer += (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM - pDLT698RecordPara->TimeOrLast);
	}
	#elif(PROTOCOL_VERSION == 24)
	twSavePointer += (EventAttInfo.MaxRecordNo - pDLT698RecordPara->TimeOrLast);
	#endif	
	twSavePointer %= EventAttInfo.MaxRecordNo;	
	
	EventDataInfo.RecordNo -= pDLT698RecordPara->TimeOrLast;	
	//第11~20条，保存起始地址是在第二个扇区
	//if( (EventDataInfo.RecordNo%(EventAttInfo.MaxRecordNo*2)) >= EventAttInfo.MaxRecordNo )
	//{
	//	EventAddrLen.dwRecordAddr += SECTOR_SIZE;
	//}
	#if(PROTOCOL_VERSION == 25)
	if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
	{
		EventAddrLen.dwRecordAddr += (EventDataInfo.RecordNo % (EventAttInfo.MaxRecordNo * UNDEV_EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
	}
	else
	#endif
	{
		EventAddrLen.dwRecordAddr += (EventDataInfo.RecordNo % (EventAttInfo.MaxRecordNo * EVENT_SECTOR_NUM)) / EventAttInfo.MaxRecordNo * SECTOR_SIZE;
	}
	
	dwTemp = EventAddrLen.dwRecordAddr + EventAttInfo.AllDataLen*twSavePointer;
	
	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		// 工况类事件
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		i = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventDelayedStatus( eGET_EVENT_DELAYED_STATUS, i );	
	}
	
	api_ReadMemRecordData( dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
	
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pEventOADCommonData = &EventOADCommonData;
	Data.pEventAttInfo = &EventAttInfo;
	//传入考虑两个扇区后的地址
	Data.pEventAddrLen = &EventAddrLen;
	Data.dwPointer = twSavePointer;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.Tag = Tag;
	Data.status = status;
	Data.AllorSep = 0;
	#if(PROTOCOL_VERSION == 25)
	Data.Last = (WORD)pDLT698RecordPara->TimeOrLast;
	#elif(PROTOCOL_VERSION == 24)
	Data.Last = (BYTE)pDLT698RecordPara->TimeOrLast;
	#endif

	Data.dwMaxRecorder = twCurrentRecordNo;

	return ReadEventRecordSub(&Data);
}

//-----------------------------------------------
//函数功能: 读事件记录
//
//参数: 
//	Tag[in]:				0 不加Tag 1 加Tag                
//	pDLT698RecordPara[in]: 	读取冻结的参数结构体                
//  Len[in]：				输入的buf长度 用来判断buf长度是否够用                
//  pBuf[out]: 				返回数据的指针，有的传数据，没有的补0	
//返回值:			bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//					bit0~bit14 代表返回的冻结数据长度
//备注:
//-----------------------------------------------
WORD api_ReadEventRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	BYTE tEventIndex;

	if( GetEventIndex( pDLT698RecordPara->OI, &tEventIndex ) == FALSE )
	{
		return 0x8000;
	}

	DeleteReportEventOAD( pDLT698RecordPara->OI, pDLT698RecordPara->Phase );
	
	DeleteReportEventOADFollow( pDLT698RecordPara->OI, pDLT698RecordPara->Phase );
	
	if( pDLT698RecordPara->eTimeOrLastFlag == eNUM_FLAG )
	{
		// 读工况类，读上N次
		return ReadEventRecordByNo( Tag, pDLT698RecordPara, Len, pBuf );		
	}
	else if( 	(pDLT698RecordPara->eTimeOrLastFlag == eEVENT_START_TIME_FLAG) 
			|| 	(pDLT698RecordPara->eTimeOrLastFlag == eEVENT_END_TIME_FLAG) )
	{
		// 读工况类，以开始时间，结束时间读
		return ReadEventRecordByTime( Tag, pDLT698RecordPara, Len, pBuf );		
	}
	else if( 	( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_START_TIME_FLAG) )
			||	( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_END_TIME_FLAG) ) )
	{
		return ReadEventRecordByTimeInterval( Tag, pDLT698RecordPara, Len, pBuf );	
	}

	return 0x8000;
}

//-----------------------------------------------
//函数功能: 获取事件最近一次状态
//
//参数: 
//  	OI[in]: 	OI   
//  	Phase[in]: 	相位0总,1A,2B,3C    
//返回值:			1：事件发生未结束
//				0：事件未发生
//备注:
//-----------------------------------------------
BYTE api_GetEventLastState(   WORD OI, ePHASE_TYPE Phase)
{
	WORD Len;
	BYTE TmpOADBuf[4],State;
	TDLT698RecordPara TmpDLT698RecordPara;
	TRealTimer TmpRealTimer;

	State = 0;
    memset( (BYTE *)&TmpRealTimer, 0xff, sizeof(TRealTimer) );   
    TmpOADBuf[0] = 0x20;//事件结束时间
    TmpOADBuf[1] = 0x20;
    TmpOADBuf[2] = 0x02;
    TmpOADBuf[3] = 0x00;
    TmpDLT698RecordPara.pOAD = TmpOADBuf;//pOAD指针
    TmpDLT698RecordPara.OADNum = 1;		//OAD个数
    TmpDLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;//选择次数
    TmpDLT698RecordPara.TimeOrLast = 0x01;	//上一次
    TmpDLT698RecordPara.Phase = Phase;
    TmpDLT698RecordPara.OI = OI;//事件OI
    
    Len = api_ReadProRecordData( 0, &TmpDLT698RecordPara, sizeof(TRealTimer), (BYTE*)&TmpRealTimer );

    if((lib_IsAllAssignNum((BYTE*)&TmpRealTimer, 0, sizeof(TRealTimer)) == TRUE) 
    || (Len == 1))//事件最近一条记录结束时间为null
    {
        State = 1;
    }
	return State;
}

//-----------------------------------------------
//函数功能: 读取当前记录数，最大记录数
//
//参数:  
//  	OI[in]: 	OI   
//  	Phase[in]: 	相位0总,1A,2B,3C            
//  	Type[in]: 	参考eEVENT_RECORD_NO_TYPE                
//  	pBuf[out]：	返回数据的缓冲                    
//返回值:	TRUE/FALSE
//
//备注: 
//-----------------------------------------------
BOOL api_ReadEventRecordNo( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf )
{
	TEventAttInfo	EventAttInfo;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	WORD			twCurrentRecordNo;
	BYTE			tEventIndex;

	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}

	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	if( Type == eEVENT_CURRENT_RECORD_NO )
	{
		// 当前记录数
		api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

		#if(PROTOCOL_VERSION == 25)
		if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
		{
			if( EventDataInfo.RecordNo > UNDEV_EVENT_DEEP )
			{
				twCurrentRecordNo = UNDEV_EVENT_DEEP;
			}
			else
			{
				twCurrentRecordNo = EventDataInfo.RecordNo;
			}
		}
		else
		#endif //#if(PROTOCOL_VERSION == 25)
		{
			if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
			{
				twCurrentRecordNo = EventAttInfo.MaxRecordNo;
			}
			else
			{
				twCurrentRecordNo = EventDataInfo.RecordNo;
			}
		}

		memcpy( pBuf, (BYTE *)&twCurrentRecordNo, sizeof(WORD) );		
	}
	else if( Type == eEVENT_MAX_RECORD_NO )
	{
		#if(PROTOCOL_VERSION == 25)
		if(EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO)
		{
			twCurrentRecordNo = UNDEV_EVENT_DEEP;
			memcpy( pBuf, (BYTE *)&twCurrentRecordNo, sizeof(WORD) );
		}
		else
		#endif
		{
			// 最大记录数
			api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
			memcpy( pBuf, (BYTE *)&EventAttInfo.MaxRecordNo, sizeof(WORD) );
		}
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读取当前值记录表
//
//参数:  
//  OI[in]: 	OI      
//  Phase[in]: 	相位0总,1A,2B,3C 
//	Type[in]:	0--读最近1次发生时间、结束时间(Class 7有发生源)  其它--读发生次数+累计时间(Class 7有发生源)
//  pBuf[out]：	返回数据的缓冲
//                    
//返回值:	0--读取失败，其它--返回读取长度
//
//备注: 读事件类Class 7最近1次发生时间、结束时间时，无功组合模式字编程记录不在此处处理，发生源固定为NULL，返回如下：
//			00 00 00--发生源为NULL，发生时间为NULL，结束时间为NULL
//			00 YY YY MM DD hh mm ss 00--发生源为NULL，发生时间有值，结束时间为NULL 
//			00 YY YY MM DD hh mm ss YY YY MM DD hh mm ss--发生源为NULL，发生时间有值，结束时间有值
//		读事件类Class 24最近1次发生时间、结束时间时，返回如下:
//			00 00--发生时间为NULL，结束时间为NULL
//			YY YY MM DD hh mm ss 00--发生时间有值，结束时间为NULL 
//			YY YY MM DD hh mm ss YY YY MM DD hh mm ss--发生时间有值，结束时间有值
//		读事件类Class 7发生次数+累计时间时，返回如下：
//			00 00 00 00 00 00 00 00 00--发生源为NULL，发生次数为4字节00，累计时间为4字节00
//			00 N0 N1 N2 N3 00 00 00 00--发生源为NULL，发生次数为4字节数据，累计时间为4字节00
//			00 N0 N1 N2 N3 T0 T1 T2 T3--发生源为NULL，发生次数为4字节数据，累计时间为4字节数据
//		读事件类Class 24发生次数+累计时间时，返回如下：
//			00 00 00 00 00 00 00 00--发生次数为4字节00，累计时间为4字节00
//			N0 N1 N2 N3 00 00 00 00--发生次数为4字节数据，累计时间为4字节00
//			N0 N1 N2 N3 T0 T1 T2 T3--发生次数为4字节数据，累计时间为4字节数据
//-----------------------------------------------
WORD api_ReadCurrentRecordTable( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf )
{
	WORD wLen;
	#if(PROTOCOL_VERSION == 25)
	WORD wSour = 0;
	#endif
	TDLT698RecordPara DLT698RecordPara;      
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo	EventAttInfo;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOAD[3];
	DWORD	dwTemp,dwTemp1;
	//WORD	twSavePointer;
	BYTE	i,j,status;
	BYTE	*pTemp;
	
	if( GetEventIndex( OI, &i ) == FALSE )//判断事件OI是否支持，不支持返回false
	{
		return 0;
	}
	
	// 只支持分相ABC，读总，填零返回TRUE
	if( Phase == ePHASE_ALL )
	{
		if( EventInfoTab[i].NumofPhase == 3 )//如果对应事件不支持总 返回NULL
		{
			pBuf[0] = 0;
			return 1;
		}
	}

	EventAddrLen.EventIndex = i;
	EventAddrLen.Phase = Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0;
	}

	api_ReadContinueMem(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		
	pTemp = pBuf;

	// 事件发生源
	#if(PROTOCOL_VERSION == 25)
	if( (OI==OAD_HARMONIC_U_DISTORTION)//class_id=24
        ||(OI==OAD_HARMONIC_I_DISTORTION)
        ||(OI==OAD_SHORT_FLICKER_U_OVERRUN)
        ||(OI==OAD_LONG_FLICKER_U_OVERRUN)
        ||(OI==OAD_SAG_U)
        ||(OI==OAD_SWELL_U)
        ||(OI==OAD_INTERRUPTION_U) 
		||(OI==OAD_SPECIFIC_DEV) )
	#elif(PROTOCOL_VERSION == 24)
		if( (OI==OAD_HARMONIC_U_DISTORTION)//class_id=24
        ||(OI==OAD_HARMONIC_I_DISTORTION)
        ||(OI==OAD_SHORT_FLICKER_U_OVERRUN)
        ||(OI==OAD_LONG_FLICKER_U_OVERRUN)
        ||(OI==OAD_SAG_U)
        ||(OI==OAD_SWELL_U)
        ||(OI==OAD_INTERRUPTION_U))
	#endif
    //if (((OI >= 0x3000)&&(OI<=0x3008)) || (OI==0x300b) || ((OI>=0x3039)&&(OI<=0x3050)) ) 	//class_id=24
	{
		//无
		#if(PROTOCOL_VERSION == 25)
		if(OI == OAD_SPECIFIC_DEV)
		{
			dwOAD[0] = 0x00022420;//2024 0200 事件发生源
			
			DLT698RecordPara.OI = OI;                   //OI数据
			DLT698RecordPara.Phase = (ePHASE_TYPE)Phase;
			
			DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOAD指针
			DLT698RecordPara.OADNum = 1;     	//OAD个数
			DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //选择次数
			DLT698RecordPara.TimeOrLast = 1;           //绝对时间数 上1次
			
			wSour = 0;
			//调用函数 读取记录 返回数据 不加TAG
			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, 2, (BYTE*)&wSour );	
			
			memcpy(&pTemp[0],(BYTE*)&wSour,2);
			wLen = 2;
		}
		else
		#endif
		{
			wLen = 0;
		}
	} 
	else	//class_id=7
	{
		pTemp[0] = 0;// 发生源(1)
		if(Type == 1)
		{
			wLen = 0;
		}
		else
		{
			wLen = 1;
		}
	}
	pTemp += wLen;

	if( Type != 0 )//Type[in]:	0--读最近1次发生时间、结束时间(Class 7有发生源)  其它--读发生次数+累计时间(Class 7有发生源)
	{	
		// 事件发生次数
		memcpy( pTemp, (BYTE *)&EventDataInfo.RecordNo, sizeof(DWORD) );//发生次数
		pTemp += sizeof(DWORD);
	}
	
	dwOAD[0] = 0x00021E20;//201E	8	事件发生时间 
	dwOAD[1] = 0x00022020;//2020	8	事件结束时间
	
	DLT698RecordPara.OI = OI;                   //OI数据
	DLT698RecordPara.Phase = (ePHASE_TYPE)Phase;
	
    DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOAD指针
    DLT698RecordPara.OADNum = 2;     	//OAD个数
    DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //选择次数
    DLT698RecordPara.TimeOrLast = 1;           //绝对时间数 上1次

	// 累计时间
	if( (EventAddrLen.EventIndex<eNUM_OF_EVENT_PRG) && (EventAddrLen.EventIndex>=ePRG_RECORD_NO))
	{
		if( Type == 0 )
		{
			//调用函数 读取记录 返回数据 不加TAG
    		wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), pTemp );

			//如果BeginTime中为0，则拷到pTemp也是00 00，两个00表示发生时间、结束时间都为NULL
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				pTemp[0] = 0x00;
				pTemp[1] = 0x00;  
				return (pTemp+2-pBuf);
			}
			else//如果BeginTime中不为0，则拷到pTemp也7字节BeginTime 00，00表示结束时间为NULL
			{					
			    pTemp[7] = 0x00;
				return (pTemp+8-pBuf);
			}		 
		}
		else
		{		
			memset( pTemp, 0x00, sizeof(DWORD) );//操作类编程无累计时间
			return (pTemp+4-pBuf);
		}
	}
	#if(PROTOCOL_VERSION == 25)
	else if ((EventAddrLen.EventIndex == eSUB_EVENT_METER_CLEAR_NO) || (EventAddrLen.EventIndex == eSUB_EVENT_ADJUST_TIME_NO)
	  || (EventAddrLen.EventIndex == eSUB_EVENT_UNKNOWN_DEV_NO) || (EventAddrLen.EventIndex == eSUB_EVENT_SPECIFIC_DEV_NO))
	#elif(PROTOCOL_VERSION == 24)
	else if ((EventAddrLen.EventIndex == eSUB_EVENT_METER_CLEAR_NO) || (EventAddrLen.EventIndex == eSUB_EVENT_ADJUST_TIME_NO))
	#endif
	{
		if(Type == 1)//属性7
		{
			memset( pTemp, 0x00, sizeof(DWORD) );//操作类编程无累计时间
			return (pTemp+4-pBuf);
		}
		else if(Type == 0)//属性10
		{
			// 调用函数 读取记录 返回数据 不加TAG
    		wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), pTemp );
			//如果BeginTime中为0，则拷到pTemp也是00 00，两个00表示发生时间、结束时间都为NULL
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				pTemp[0] = 0x00;
				pTemp[1] = 0x00;  
				return (pTemp+2-pBuf);
			}
			return (pTemp+14-pBuf);
		}
	}
	else
	{
		j = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );	
		
		if( Type == 0 )
		{
			//调用函数 读取记录 返回数据 不加TAG
    		wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), pTemp );

			//如果BeginTime中为0，则拷到pTemp也是00 00，两个00表示发生时间、结束时间都为NULL
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				pTemp[0] = 0x00;
				pTemp[1] = 0x00;  
				return (pTemp+2-pBuf);
			}
			else
			{
				if( api_DealEventDelayedStatus( eGET_EVENT_DELAYED_STATUS, j ) == 1 )//如果事件正在发生
				{
	
					dwOAD[0] = 0x00022020;//2020	8	事件结束时间
				    DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOAD指针
				    DLT698RecordPara.OADNum = 1;     	//OAD个数
					DLT698RecordPara.TimeOrLast = 2;           //绝对时间数 上2次
					//调用函数 读取记录 返回数据 不加TAG
    				wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, 7, (pTemp+7) );	
    				if( (wLen & 0x8000)||(wLen == 1) )
					{
						pTemp[7] = 0x00;
						return (pTemp+8-pBuf);//如果BeginTime中不为0，则拷到pTemp也7字节BeginTime 00，00表示结束时间为NULL
					}
					else
					{
						return (pTemp+14-pBuf);
					}
				}
				else
				{
					return (pTemp+14-pBuf);
				}
			}		
		}
		else
		{
			status = api_DealEventDelayedStatus( eGET_EVENT_DELAYED_STATUS, j );		

			// 发生还未结束
			if( status == 1 )
			{			
				//调用函数 读取记录 返回数据 不加TAG
    			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), (BYTE *)&EventOADCommonData.EventTime.BeginTime );
				
				if( (wLen & 0x8000) == 0x0000 )
				{	
					// 计算累计时间
					dwTemp1 = api_GetRelativeSec();			// 当前时间对应相对秒数
					dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );	// 发生时刻对应相对秒数
					
					if( (dwTemp1!=0xffffffff) && (dwTemp!=0xffffffff) )
					{
						if( dwTemp1 >= dwTemp )
						{
							EventDataInfo.AccTime += (dwTemp1-dwTemp);
						}
						else
						{
							//EventDataInfo.AccTime = 0;
						}
					}
					else
					{
						//EventDataInfo.AccTime = 0; // 错误返回0
					}
				}
			}
			if(OI == OAD_METER_CLEAR)
			{
				EventDataInfo.AccTime = 0;
			}
			memcpy( pTemp, (BYTE *)&EventDataInfo.AccTime, sizeof(DWORD) );
			return (pTemp+4-pBuf);
		}
	}
	return 0;
}

//-----------------------------------------------
//函数功能: 初始化所有相关RAM
//
//参数: 无
//	
//返回值: 无
//
//备注: 上电，总清零调用
//-----------------------------------------------
static void InitEventAllRam( void )
{
	// 清定时计数器
	memset(SubEventTimer, 0x00, sizeof(SubEventTimer));

	memset((BYTE *)&EventStatus, 0x00, sizeof(EventStatus));
}

//-----------------------------------------------
//函数功能: 清分相事件记录数据
//
//参数: 
//  OI[in]:	OI
//	
//返回值:  	TRUE   正确
//			FALSE  错误
//  
// 备注:
//-----------------------------------------------
BOOL api_ClrSepEvent( WORD OI )
{
	BYTE Type;

	if( GetEventIndex( OI, &Type ) == FALSE )
	{
		return FALSE;
	}

	ClearEventData( Type );

	return TRUE;
}

//-----------------------------------------------
//函数功能: 清所有事件记录数据
//
//参数: 
//  Type[in]:	
//				eFACTORY_INIT = 0,				清所有事件，包含电表清零记录，事件清零记录
//				eEXCEPT_CLEAR_METER,			清所有事件，不包含电表清零记录
//				eEXCEPT_CLEAR_METER_CLEAR_EVENT	清所有事件，不包含电表清零记录，事件清零记录
//返回值:	无
//  
// 备注:
//-----------------------------------------------
BOOL api_ClrAllEvent( BYTE Type )
{
	TEventAddrLen	EventAddrLen;
	BYTE i;

	for( i = 0; i < eNUM_OF_EVENT_ALL; i++ )
	{		
		EventAddrLen.EventIndex = i;
		EventAddrLen.Phase = 0;
		if( GetEventInfo( &EventAddrLen ) == FALSE )
		{
			continue;
		}
		ClearEventData( i );
	}

	// 清相关事件的掉电数据
	api_ClrContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.PowerDownFlag.InstantEventFlag[0] ), GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag ) );

	return TRUE;
}


//-----------------------------------------------
//函数功能: 得到事件延时
//
//参数: EventOrder[in]:eEVENT_INDEX
//                    
//返回值: 延时时间，单位秒
//			
//备注: 事件序号从1开始
//-----------------------------------------------
BYTE GetEventDelayTime(BYTE EventOrder)
{
	BYTE DelayTime;

	if(EventOrder == eEVENT_SHORT_FLICKER_U_OVERRUN_NO)
		DelayTime = 60;
	else if((EventOrder == eEVENT_SAG_U_NO) || (EventOrder == eEVENT_SWELL_U_NO) || (EventOrder == eEVENT_INTERRUPTION_U_NO))
	{
		if(EventOrder == eEVENT_SAG_U_NO)
		{
			//if(gEventPara.wSagUDelayTime >= 10)
			//		return 10;
			//else
			return gEventPara.wSagUDelayTime;
		}
		if(EventOrder == eEVENT_SWELL_U_NO)
		{
			//if(gEventPara.wSwellUDelayTime >= 10)
			//	return 10;
			//else
			return gEventPara.wSwellUDelayTime;
		}
		if(EventOrder == eEVENT_INTERRUPTION_U_NO)
		{
			//if(gEventPara.wInterruptUDelayTime >= 10)
			//	return 10;
			//else
			return gEventPara.wInterruptUDelayTime;
		}
	
		DelayTime = 10;			
	}
	else
		DelayTime = 60;

	if(EventOrder == eEVENT_HARMONIC_U_DISTORTION_NO)
		return gEventPara.HrmUDistorDelayTime;

	if(EventOrder == eEVENT_HARMONIC_I_DISTORTION_NO)
		return gEventPara.HrmIDistorDelayTime;

	if(EventOrder == eEVENT_SHORT_FLICKER_U_OVERRUN_NO)
		return gEventPara.wShortFlkDelayTime;

	if(EventOrder == eEVENT_LONG_FLICKER_U_OVERRUN_NO)
		return gEventPara.wLongFlkDelayTime;

	if(EventOrder == eEVENT_U_DEV_NO)
		return gEventPara.UDevDelayTime;

	if(EventOrder == eEVENT_F_DEV_NO)
		return gEventPara.FDevDelayTime;

	if(EventOrder == eEVENT_U_UNBALANCE_NO)
		return gEventPara.UUnbalanceDelayTime;

	if(EventOrder == eEVENT_I_UNBALANCE_NO)
		return gEventPara.IUnbalanceDelayTime;

	if(EventOrder == eEVENT_U_CHANGE_NO)
		return gEventPara.UChangeDelayTime;

	if(EventOrder == eSUB_EVENT_JLCHIP_FAIL_NO)
			return gEventPara.SPIDelayTime;

	return DelayTime;
}


//-----------------------------------------------
//函数功能: 处理事件延时后的状态
//
//参数: 
//			Type[in]：			eCLEAR_EVENT_DELAYED_STATUS							
//								eSET_EVENT_DELAYED_STATUS
//								eGET_EVENT_DELAYED_STATUS
//			inSubEventIndex[in]	eSUB_EVENT_INDEX
//                    
//返回值: Type为eGET_EVENT_STATUS时，1为事件发生还未结束，0为事件未发生 	
//				其他时  固定返回0
//备注:   
//-----------------------------------------------
BYTE api_DealEventDelayedStatus(eDEAL_EVENT_DELAYED_STATUS Type, BYTE inSubEventIndex )
{	
	BYTE Status = 0;
	BYTE i,j;

	i = inSubEventIndex / 8;
	j = inSubEventIndex % 8;

	if( Type == eGET_EVENT_DELAYED_STATUS )//获取事件延时状态
	{
		if( (EventStatus.DelayedStatus[i] & (0x01<<j)) != 0 )
		{
			// 状态有效
			Status = 1;
		}
	}
	else if( Type == eSET_EVENT_DELAYED_STATUS )//设置事件延时状态
	{
		EventStatus.DelayedStatus[i] |= (0x01<<j);
	}
	else if( Type == eCLEAR_EVENT_DELAYED_STATUS )//清除事件延时状态
	{
		EventStatus.DelayedStatus[i] &= ~(0x01<<j);
	}
	else
	{
		//ASSERT(0, 0);
	}

	return Status;
}

//-----------------------------------------------
//函数功能: 处理事件当前状态
//
//参数: 
//			Type[in]：			eGET_EVENT_CURRENT_STATUS							
//								eSET_EVENT_CURRENT_STATUS
//								eCLEAR_EVENT_CURRENT_STATUS
//			inSubEventIndex[in]	eSUB_EVENT_INDEX
//                    
//返回值: Type为eGET_EVENT_STATUS时，1为事件发生还未结束，0为事件未发生 	
//				其他时  固定返回0
//备注:   
//-----------------------------------------------
BYTE api_DealEventCurrentStatus(eDEAL_EVENT_CURRENT_STATUS Type, BYTE inSubEventIndex )
{	
	BYTE Status = 0;
	BYTE i,j;

	i = inSubEventIndex / 8;
	j = inSubEventIndex % 8;

	if( Type == eGET_EVENT_CURRENT_STATUS )//获取事件当前状态
	{
		if( (EventStatus.CurrentStatus[i] & (0x01<<j)) != 0 )
		{
			// 状态有效
			Status = 1;
		}
	}
	else if( Type == eSET_EVENT_CURRENT_STATUS )//设置事件当前状态
	{
		EventStatus.CurrentStatus[i] |= (0x01<<j);
	}
	else if( Type == eCLEAR_EVENT_CURRENT_STATUS )//清除事件当前状态
	{
		EventStatus.CurrentStatus[i] &= ~(0x01<<j);
	}
	else
	{
		//ASSERT(0, 0);
	}

	return Status;
}

//-----------------------------------------------
//函数功能: 事件定时处理，每秒调用一次
//
//参数: 无
//
//返回值: 无
//
//备注:
//-----------------------------------------------
static void EventSecTask( void )
{
	BYTE i,Status,Temp;

	//-------------------------------
	// 处理所有事件记录
	// 事件序号从1开始
	for( i = 0; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{	    
		// 是否处于活动状态
		if( SubEventTimer[i] != 0 )
		{
			// 时间递减
			SubEventTimer[i]--;

			// 是否时间到
			if( SubEventTimer[i] == 0 )
			{
				// 判断记录的状态
				if( EventStatus.CurrentStatus[i/8]&(0x01<<(i%8) ) )
				{
					SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_START, eEVENT_ENDTIME_CURRENT );
				}
				else
				{
					SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_CURRENT );
				}
			}
		}
	}

	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		// 得到状态 非0即1
		if(SubEventInfoTab[i].pGetEventStatus == NULL)
		{
			continue;
		}
		Status = SubEventInfoTab[i].pGetEventStatus( SubEventInfoTab[i].Phase ); 

		// 若没有变化则不处理
		Temp = (BYTE)( EventStatus.CurrentStatus[i/8] & (0x01<<(i%8)) );
		Temp = (Temp>>(i%8));
		
		if( Status == Temp )
		{
			continue;
		}
	
		// 若延时正在继续
		if( SubEventTimer[i] != 0 )
		{
			// 停止计时，因为在延时过程中发生了状态变化
			// 延时状态取消，事件停止处理
			SubEventTimer[i] = 0;
		}
		else
		{
			SubEventTimer[i] = GetEventDelayTime(SubEventInfoTab[i].EventIndex);
		}
	
		if( Status == 0 )
		{
			EventStatus.CurrentStatus[i/8] &= (BYTE)~(0x01<<(i%8));
		}
		else
		{
			EventStatus.CurrentStatus[i/8] |= (BYTE)(0x01<<(i%8));
		}
	}
}

#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  接收消息队列
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  RecMSGTask( void )
{
	BYTE msg_UnKDevFeature_Root[FEATRUE_MSG_SIZE];
	BYTE msg_UnKDevFeature_temp[FEATRUE_MSG_SIZE];  //用于比较全ff
	TDeviceInfoPool msg_DevInof_Root;
	BYTE i = 0,j = 0,k = 0,bLen = 0;
	TSpecificDevice SpcDev;

	//初始化
	memset((BYTE*)&msg_UnKDevFeature_Root[0], 0xFF, sizeof(msg_UnKDevFeature_Root));
	memset((BYTE*)&msg_UnKDevFeature_temp[0], 0xFF, sizeof(msg_UnKDevFeature_temp));
	memset((BYTE*)&msg_DevInof_Root.bDevCount, 0xFF, sizeof(msg_DevInof_Root));

	//获取数据
	if(mq_UnkDevFeature != RT_NULL)
	{
		//接收特征有关消息
		bLen = mq_UnkDevFeature->entry; //当前消息队列中已经发送的消息个数
		if(bLen <= MSG_NUMBER)
		{
			for(i = 0; i < bLen; i++)
			{
				api_QueueRecv(mq_UnkDevFeature, (BYTE*)&msg_UnKDevFeature_Root[0], FEATRUE_MSG_SIZE);
				api_SetFeartureData((BYTE*)&msg_UnKDevFeature_Root[0]);//暂存特征数据
				if(memcmp((BYTE*)&msg_UnKDevFeature_temp[0],(BYTE*)&msg_UnKDevFeature_Root[0],sizeof(msg_UnKDevFeature_temp)) != 0)//非全ff才能写
				{
					SaveInstantEventRecord(eSUB_EVENT_UNKNOWN_DEV_NO, ePHASE_A, EVENT_START, eEVENT_ENDTIME_CURRENT);//存未知事件
				}

				memset((BYTE*)&msg_UnKDevFeature_Root[0], 0xFF, sizeof(msg_UnKDevFeature_Root));
			}
		}
		else
		{
			;//失败
		}
	}
	else
	{
		;//失败
	}
	
	if(mq_DeviceInfo != RT_NULL)
	{
		//接收事件相关的消息
		bLen = mq_DeviceInfo->entry;//当前消息队列中已经发送的消息个数
		if((bLen <= MSG_NUMBER) && (bLen > 0))
		{
			api_GetSpecificDeviceID(&SpcDev); //获取特定设备的指定类别
			for(i = 0; i < bLen; i++)
			{
				api_QueueRecv(mq_DeviceInfo, (BYTE*)&msg_DevInof_Root.bDevCount, sizeof(TDeviceInfoPool));
				if(api_SetSpecificData((TDeviceInfoPool*)&msg_DevInof_Root) == FALSE)
				{
					continue;
				}
				
				for(j = 0; j < g_DevInof.bDevCount; j++) //需要判断全局,代表是合法的设备信息
				{
					for(k = 0; k < SpcDev.bySpecificDeviceNum; k++)
					{
						if((SpcDev.wSpecificDeviceID[k] & 0xFF00) == (g_DevInof.Message[j].wDevID & 0xFF00))//事件需要判断类别再存储事件 ！！！！！！
						{
							g_SpeDevSaveCount = j;
							SaveInstantEventRecord(eSUB_EVENT_SPECIFIC_DEV_NO, ePHASE_A, EVENT_START, eEVENT_ENDTIME_CURRENT);//存特定事件
						}
					}
				}
				memset((BYTE*)&msg_DevInof_Root.bDevCount, 0xFF, sizeof(msg_DevInof_Root));
				g_SpeDevSaveCount = 0;
			}
		}
		else
		{
			;//失败
		}
	}
	else
	{
		;//失败
	}
}
#endif //#if(PROTOCOL_VERSION == 25)

//-----------------------------------------------
//函数功能: 判断事件状态
//
//参数: 无
//                    
//返回值: 无
//			
//备注: 大循环调用
//-----------------------------------------------
void api_EventTask(void)
{
  	TRealTimer t;
	
	#if(PROTOCOL_VERSION == 25)
	//处理接收消息队列
	RecMSGTask();
	#endif
    
	// 秒变化
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_SECOND);
		
		EventSecTask();	
	}
	
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_MINUTE) == TRUE )
	{
        api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_MINUTE);
	}
	
	// 小时变化
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_HOUR);
		
        api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
		//判断过零点
		if(t.Hour == 0)
		{
			SysFREEMsgCounter = 0;
			SysUNMsgCounter = 0;
		}	
	}
}
//--------------------------------------------------
//功能描述:  模组复位事件
//         
//参数:      
//         
//返回值:    
//         
//备注:  RCU基地址：0x40023800,地址偏移：0x74, 复位源/时钟寄存器（RCU_RSTSCK）
//复位值：0x0E00 0000, 所有复位标志位仅在电源复位时被清零，RSTFC/IRC32KEN在系统复位时被清零
//--------------------------------------------------
void api_PowerUpReset( void )
{
	DWORD dwADDR,dwResVal = 0;
	
	dwADDR = RESET_EVENT_ADDR;
	dwResVal = *(DWORD*)dwADDR;

	if((dwResVal >> 30) & 0x01)//窗口看门狗复位
	{
		api_WriteSysUNMsg(RT_RESET_WWDGTRSTF);
	}

	if((dwResVal >> 29) & 0x01)//独立看门狗复位
	{
		api_WriteSysUNMsg(RT_RESET_FWDGTRSTF);
	}

	if((dwResVal >> 28) & 0x01)//软件复位
	{
		api_WriteSysUNMsg(RT_RESET_SWRSTF);
	}

	if((dwResVal >> 27) & 0x01)//电源复位
	{
		api_WriteSysUNMsg(RT_RESET_PORRSTF);//异常事件
		api_WriteFreeEvent(RT_RESET_PORRSTF,0);//自由事件也记录
	}

	if((dwResVal >> 26) & 0x01)//外部引脚复位
	{
		api_WriteSysUNMsg(RT_RESET_EPRSTF);
	}

	if((dwResVal >> 25) & 0x01)//欠压复位
	{
		api_WriteSysUNMsg(RT_RESET_BORRSTF);
	}

	//清除复位标志
	*(DWORD*)dwADDR &= 0xFEFFFFFF;
	*(DWORD*)dwADDR |= (1 << 24); //清除所有复位标志
}
//-----------------------------------------------
//函数功能: 上电事件处理
//
//参数: 无
//                    
//返回值: 无
//			
//备注: 
//-----------------------------------------------
void api_PowerUpEvent(void)
{
	BYTE i;
	WORD Len;
	DWORD dwAddr;

	/* 复位事件记录 */
	api_PowerUpReset();

	#if(PROTOCOL_VERSION == 25)
	memset((BYTE*)&g_UnDevFeature[0], 0xFF, FEATRUE_MSG_SIZE);				//上电置0xFF特征数据
	memset((BYTE*)&g_DevInof.bDevCount, 0xFF, sizeof(TDeviceInfoPool));		//上电置0xFF特定设备信息数据
	#endif

	memset( (BYTE *)&EventStatus, 0, sizeof(EventStatus) );

	InitEventAllRam();

	Len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.PowerDownFlag.InstantEventFlag[0] );
	api_ReadContinueMem( dwAddr, Len, (BYTE *)&EventStatus.DelayedStatus[0] );
	
	//wxy：下面的代码不需要与电能质量一致，电能质量有上电继续保持的事件，例如电压超限有关的 ！！！
	for( i = 0; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		//若掉电时的事件没有结束，则上电时结束
		if( api_DealEventDelayedStatus( eGET_EVENT_DELAYED_STATUS, i ) == 1 )
		{
			if(EventInfoTab[ SubEventInfoTab[i].EventIndex ].SpecialProc == NO)
			{
				#if(PROTOCOL_VERSION == 25)
				if((SubEventInfoTab[i].EventIndex != eSUB_EVENT_UPDATE_NO)  
				&& (SubEventInfoTab[i].EventIndex != eSUB_EVENT_UNKNOWN_DEV_NO) 
				&& (SubEventInfoTab[i].EventIndex != eSUB_EVENT_SPECIFIC_DEV_NO)
				)
				#elif(PROTOCOL_VERSION == 24)
				if(SubEventInfoTab[i].EventIndex != eSUB_EVENT_UPDATE_NO)
				#endif
					SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_PWRDOWN );
			}
		}
	}

	//处理完毕后清除暂存的事件掉电标志
	api_ClrContinueEEPRom( dwAddr, Len );//wxy这部分需要清除，电能质量不清除原因是含有上电继续保持的事件！！！

	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ReportSafeRom.ReportPara ), sizeof(TReportPara), (BYTE *)&ReportPara );
}

void ClearPowerEventFlag(void)
{
	WORD Len;
	DWORD dwAddr;

	memset( (BYTE *)&EventStatus, 0, sizeof(EventStatus) );

	InitEventAllRam();

	Len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.PowerDownFlag.InstantEventFlag[0] );
	//api_ReadContinueMem( dwAddr, Len, (BYTE *)&EventStatus.DelayedStatus[0] );

	api_WriteContinueMem( dwAddr, Len, (BYTE *)&EventStatus.DelayedStatus[0] );
	//处理完毕后清除暂存的事件掉电标志
	api_ClrContinueEEPRom( dwAddr, Len );

	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ReportSafeRom.ReportPara ), sizeof(TReportPara), (BYTE *)&ReportPara );
}

//-----------------------------------------------
//函数功能: 掉电处理
//
//参数: 无
//                    
//返回值: 无
//			
//备注: 
//-----------------------------------------------
void api_PowerDownEvent(void)
{
	BYTE len;
	DWORD dwAddr;
	TLostPowerTime LostPowerTime;
	
	// 记录掉电时间
	api_ReadMeterTime( &(LostPowerTime.Time) );//获取当前时间
	dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.LostPowerTime );
	api_VWriteSafeMem( dwAddr, sizeof(TLostPowerTime) , (BYTE*)&LostPowerTime.Time );
	
	// 保存当前的事件状态
	len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.PowerDownFlag.InstantEventFlag[0] );
	api_WriteContinueMem( dwAddr, len, (BYTE *)&EventStatus.DelayedStatus[0] );

	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ReportSafeRom.ReportPara ), sizeof(TReportPara), (BYTE *)&ReportPara );
}

#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  读取特定设备事件指定电器类型
//         
//参数:      WORD *wSpcDevID 是要读取的特定设备ID的指针
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
WORD  api_GetSpecificDeviceID( TSpecificDevice *wSpcDevID )
{
	// WORD Len = 0;
	BOOL Res = FALSE;
	TSpecificDevice SpcDev;
	DWORD dwCrc = 0;

	Res = api_ReadContinueMem( GET_CONTINUE_ADDR( EventConRom.EventPara.SpecificDevice), sizeof(TSpecificDevice), (BYTE *)&SpcDev );
	
	if(Res == FALSE)
	{
		memset((BYTE*)&wSpcDevID->bySpecificDeviceNum, 0xFF, sizeof(TSpecificDevice));
		wSpcDevID->bySpecificDeviceNum = 0;
		return 0;
	}
	else
	{
		dwCrc = lib_CheckCRC32( (BYTE*)&SpcDev.bySpecificDeviceNum, (sizeof(TSpecificDevice) - sizeof(DWORD)) );
		if(dwCrc != SpcDev.dwCRC)
		{
			memset((BYTE*)&wSpcDevID->bySpecificDeviceNum, 0xFF, sizeof(TSpecificDevice));
			wSpcDevID->bySpecificDeviceNum = 0;
			return 0;
		}
		else
		{
			memcpy((BYTE*)&wSpcDevID->bySpecificDeviceNum, (BYTE*)&SpcDev.bySpecificDeviceNum, sizeof(TSpecificDevice));
		}
	}

	return SpcDev.bySpecificDeviceNum;
}

//--------------------------------------------------
//功能描述:  设置特定设备事件指定电器类型
//         
//参数:      WORD *wSpcDevID 是要设置的特定设备ID的指针
//           BYTE Len,是设置特定设备ID的长度
//			 BYTE bFlag:0x0A正常协议设置，0x0B清零设置
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_SetSpecificDeviceID(BYTE bFlag, TSpecificDevice *wSpcDevID )
{
	BYTE i = 0,j = 0,byCount = 0;
	BOOL Res = FALSE;
	TSpecificDevice SpcDev;
	
	if(bFlag == 0x0A) //正常设置
	{
		if((wSpcDevID->bySpecificDeviceNum > NILM_309B_MAX_NO) || (wSpcDevID->bySpecificDeviceNum == 0))
		{
			return FALSE;
		}

		for(i = 0; i < wSpcDevID->bySpecificDeviceNum; i++)
		{
			for(j = 0; j < sizeof(g_bDevice_Code_List); j++)
			{
				if((wSpcDevID->wSpecificDeviceID[i] >> 8) == g_bDevice_Code_List[j])
				{
					break;
				}
			}
			if(j < sizeof(g_bDevice_Code_List))
			{
				byCount++;
			}
		}

		if(byCount != (wSpcDevID->bySpecificDeviceNum))
		{
			return FALSE;
		}
	}

	memset((BYTE*)&SpcDev.bySpecificDeviceNum, 0xFF, sizeof(TSpecificDevice)- sizeof(DWORD));

	memcpy((BYTE*)&SpcDev.bySpecificDeviceNum, (BYTE*)&wSpcDevID->bySpecificDeviceNum, sizeof(TSpecificDevice) - sizeof(DWORD));
	
	SpcDev.dwCRC = lib_CheckCRC32( (BYTE*)&SpcDev.bySpecificDeviceNum, (sizeof(TSpecificDevice) - sizeof(DWORD)) );
		
	Res = api_WriteContinueMem( GET_CONTINUE_ADDR( EventConRom.EventPara.SpecificDevice), sizeof(TSpecificDevice), (BYTE*)&SpcDev );

	return Res;
}

//--------------------------------------------------
//功能描述:  获取特征数据
//         
//参数:      pBuf是数据指针
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
WORD api_GetFeartureData(BYTE *pBuf)
{
	WORD Len = 0;

	memcpy(pBuf,(BYTE*)&g_UnDevFeature[0],sizeof(g_UnDevFeature));
	Len = sizeof(g_UnDevFeature);

	return Len;
}
//--------------------------------------------------
//功能描述:  设置特征数据
//         
//参数:      pBuf是数据指针
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_SetFeartureData( BYTE *pBuf )
{	
	memcpy((BYTE*)&g_UnDevFeature[0],pBuf,sizeof(g_UnDevFeature));
}

//--------------------------------------------------
//功能描述:  获取特定设备信息
//         
//参数:      pBuf是数据指针
// 			 Offset是偏移量，用来读取不同的数据

//         
//返回值:    BOOL返回是否成功
//         
//备注:  
//--------------------------------------------------
BOOL  api_GetSpecificData( BYTE *pBuf ,BYTE Offset)
{
	if(Offset > 3) //不包含时间
	{
		return FALSE;
	}

	//已经存储的事件个数不能大于上限
	if(g_SpeDevSaveCount > MAX_SPECIFIC_DEV_NUM)
	{
		return FALSE;
	}

	if(Offset == 0)
	{
		memcpy(pBuf,(BYTE*)&g_DevInof.Message[g_SpeDevSaveCount].wDevID,sizeof(TDeviceInfo) - sizeof(WORD));
	}
	else if (Offset == 1)
	{
		memcpy(pBuf,(BYTE*)&g_DevInof.Message[g_SpeDevSaveCount].wDevID,sizeof(WORD));
	}
	else if (Offset == 2)
	{
		memcpy(pBuf,(BYTE*)&g_DevInof.Message[g_SpeDevSaveCount].bDevStatus,sizeof(BYTE));
	}
	else if (Offset == 3)
	{
		memcpy(pBuf,(BYTE*)&g_DevInof.Message[g_SpeDevSaveCount].dwDevAvgPow,sizeof(DWORD));
	}

	return TRUE;
}

//--------------------------------------------------
//功能描述:  设置特定设备信息
//         
//参数:      pDevInfo是数据指针
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_SetSpecificData( TDeviceInfoPool *pDevInfo )
{
	BYTE i = 0,j = 0,bySavepoint = 0;
	
	//个数合法性判断
	if((pDevInfo->bDevCount > MAX_SPECIFIC_DEV_NUM) || (pDevInfo->bDevCount == 0))
	{
		return FALSE;
	}
	//合法性判断
	for(i = 0; i < pDevInfo->bDevCount; i++)
	{
		for(j = 0; j < sizeof(g_bDevice_Code_List); j++)
		{
			if((pDevInfo->Message[i].wDevID >> 8) == g_bDevice_Code_List[j]) //电器类型判断，判断高字节
			{
				if((pDevInfo->Message[i].bDevStatus == 0) || (pDevInfo->Message[i].bDevStatus == 1)) //状态判断，只允许0和1
				{
//					if((pDevInfo->Message[i].dwDevAvgPow >= 0) && (pDevInfo->Message[i].dwDevAvgPow<=0xFFFF)) //功率判断
                    if(pDevInfo->Message[i].dwDevAvgPow<=0xFFFF) //功率判断
					{
//						if((pDevInfo->Message[i].tDevTime >= 0) && (pDevInfo->Message[i].tDevTime <= (60*15))) //时间判断
                        if(pDevInfo->Message[i].tDevTime <= (60*15)) //时间判断
						{
							memcpy((BYTE*)&g_DevInof.Message[bySavepoint].wDevID,(BYTE*)&pDevInfo->Message[i].wDevID,sizeof(TDeviceInfo));//只拷贝正确的
							bySavepoint++;
							break;
						}
					}
				}
			}
		}
	}
	if(bySavepoint > 0)//设置合法的数据
	{
		g_DevInof.bDevCount = bySavepoint; //正确的设备的个数
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
#endif //#if(PROTOCOL_VERSION == 25)
//-----------------------------------------------
//函数功能: 读事件参数
//
//参数: 
//			OI[in]:					OI
//			No[in]:					0: 全部参数
//									N: 第N个参数
//			pLen[out]:				读出数据的长度
//			pBuf[out]:				读数据的缓冲
//                    
//返回值:  	TRUE   正确
//			FALSE  错误
//
//备注:  	读取ram  698规约调用
//-----------------------------------------------
BOOL api_ReadEventPara( WORD OI, BYTE No, BYTE *pLen, BYTE *pBuf )
{
	BYTE tEventIndex;
	#if(PROTOCOL_VERSION == 25)
	TSpecificDevice SpcDev;
	#endif

	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	
	switch (OI)
	{
	case 0X3039: //电压谐波总畸变率超限事件
		if(No == 0)
		{
			pBuf[0] = LOBYTE(gEventPara.sHrmUDistorThreshold);
			pBuf[1] = HIBYTE(gEventPara.sHrmUDistorThreshold);
			pBuf[2] = LOBYTE(gEventPara.HrmUDistorDelayTime);
			//pBuf[3] = HIBYTE(gEventPara.HrmUDistorDelayTime);
			*pLen = 3;
		}
		else if(No == 1)
		{
			pBuf[0] = LOBYTE(gEventPara.sHrmUDistorThreshold);
			pBuf[1] = HIBYTE(gEventPara.sHrmUDistorThreshold);
			*pLen = 2;
		}
		else if(No == 2)
		{
			pBuf[0] = LOBYTE(gEventPara.HrmUDistorDelayTime);
			*pLen = 1;
		}
		break;
	case 0X303A://电流谐波总畸变率超限事件
		if(No == 0)	
		{
			pBuf[0] = LOBYTE(gEventPara.sHrmIDistorThreshold);
			pBuf[1] = HIBYTE(gEventPara.sHrmIDistorThreshold);
			pBuf[2] = LOBYTE(gEventPara.HrmIDistorDelayTime);
			//pBuf[3] = HIBYTE(gEventPara.HrmIDistorDelayTime);
			*pLen = 3;
		}
		else if(No == 1)
		{
			pBuf[0] = LOBYTE(gEventPara.sHrmIDistorThreshold);
			pBuf[1] = HIBYTE(gEventPara.sHrmIDistorThreshold);
			*pLen = 2;
		}
		else if(No == 2)
		{
			pBuf[0] = LOBYTE(gEventPara.HrmIDistorDelayTime);
			*pLen = 1;
		}
		break;
	case 0X3047://电压短时闪变超限事件
		if(No == 0)	
		{
			pBuf[0] = LOBYTE(gEventPara.wShortFlkThreshold/100);
			pBuf[1] = HIBYTE(gEventPara.wShortFlkThreshold/100);
			pBuf[2] = LOBYTE(gEventPara.wShortFlkDelayTime);
			pBuf[3] = HIBYTE(gEventPara.wShortFlkDelayTime);
			*pLen = 4;
		}
		else if(No == 1)
		{
			pBuf[0] = LOBYTE(gEventPara.wShortFlkThreshold/100);
			pBuf[1] = HIBYTE(gEventPara.wShortFlkThreshold/100);
			*pLen = 2;
		}
		else if(No == 2)
		{
			pBuf[0] = LOBYTE(gEventPara.wShortFlkDelayTime);
			pBuf[1] = HIBYTE(gEventPara.wShortFlkDelayTime);
			*pLen = 2;
		}
		break;
	case 0X3048://电压长时闪变超限事件
		if(No == 0)	
		{
			pBuf[0] = LOBYTE(gEventPara.wLongFlkThreshold);
			pBuf[1] = HIBYTE(gEventPara.wLongFlkThreshold);
			pBuf[2] = LOBYTE(gEventPara.wLongFlkDelayTime);
			pBuf[3] = HIBYTE(gEventPara.wLongFlkDelayTime);
			*pLen = 4;
		}
		else if(No == 1)
		{
			pBuf[0] = LOBYTE(gEventPara.wLongFlkThreshold);
			pBuf[1] = HIBYTE(gEventPara.wLongFlkThreshold);
			*pLen = 2;
		}
		else if(No == 2)
		{
			pBuf[0] = LOBYTE(gEventPara.wLongFlkDelayTime);
			pBuf[1] = HIBYTE(gEventPara.wLongFlkDelayTime);
			*pLen = 2;
		}
		break;
	case 0X3049://电压暂降事件
		if(No == 0)	
		{
			pBuf[0] = LOBYTE(gEventPara.wSagUThreshold);
			pBuf[1] = HIBYTE(gEventPara.wSagUThreshold);
			pBuf[2] = LOBYTE(gEventPara.wSagUDelayTime);
			pBuf[3] = HIBYTE(gEventPara.wSagUDelayTime);
			*pLen = 4;
		}
		else if(No == 1)
		{
			pBuf[0] = LOBYTE(gEventPara.wSagUThreshold);
			pBuf[1] = HIBYTE(gEventPara.wSagUThreshold);
			*pLen = 2;
		}
		else if(No == 2)
		{
			pBuf[0] = LOBYTE(gEventPara.wSagUDelayTime);
			pBuf[1] = HIBYTE(gEventPara.wSagUDelayTime);
			*pLen = 2;
		}
		break;
	case 0X304A://电压暂升事件
		if(No == 0)	
		{
			pBuf[0] = LOBYTE(gEventPara.wSwellUThreshold);
			pBuf[1] = HIBYTE(gEventPara.wSwellUThreshold);
			pBuf[2] = LOBYTE(gEventPara.wSwellUDelayTime);
			pBuf[3] = HIBYTE(gEventPara.wSwellUDelayTime);
			*pLen = 4;
		}
		else if(No == 1)
		{
			pBuf[0] = LOBYTE(gEventPara.wSwellUThreshold);
			pBuf[1] = HIBYTE(gEventPara.wSwellUThreshold);
			*pLen = 2;
		}
		else if(No == 2)
		{
			pBuf[0] = LOBYTE(gEventPara.wSwellUDelayTime);
			pBuf[1] = HIBYTE(gEventPara.wSwellUDelayTime);
			*pLen = 2;
		}
		break;
	case 0X304B://电压中断事件
		if(No == 0)	
		{
			pBuf[0] = LOBYTE(gEventPara.wInterruptUThreshold);
			pBuf[1] = HIBYTE(gEventPara.wInterruptUThreshold);
			pBuf[2] = LOBYTE(gEventPara.wInterruptUDelayTime);
			pBuf[3] = HIBYTE(gEventPara.wInterruptUDelayTime);
			*pLen = 4;
		}
		else if(No == 1)
		{
			pBuf[0] = LOBYTE(gEventPara.wInterruptUThreshold);
			pBuf[1] = HIBYTE(gEventPara.wInterruptUThreshold);
			*pLen = 2;
		}
		else if(No == 2)
		{
			pBuf[0] = LOBYTE(gEventPara.wInterruptUDelayTime);
			pBuf[1] = HIBYTE(gEventPara.wInterruptUDelayTime);
			*pLen = 2;
		}
		break;
	case 0x302F://SPI 异常
		pBuf[0] = gEventPara.SPIDelayTime;
		*pLen = 1;
		break;
	#if(PROTOCOL_VERSION == 25)
	case OAD_SPECIFIC_DEV://特定设备事件
		//获取特定设备的指定类别
		api_GetSpecificDeviceID(&SpcDev); //从eep获取
		*pLen = SpcDev.bySpecificDeviceNum * sizeof(WORD);
		if(*pLen > (NILM_309B_MAX_NO * sizeof(WORD)))//最大3个电器
		{
			*pLen = 0;
		}
		memcpy(pBuf, (BYTE*)&SpcDev.wSpecificDeviceID[0], *pLen);
		break;
	#endif

	default:
		*pLen = 0;
		break;
	}

//	*pLen = 0;

	return TRUE;
}

//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_FactoryInitEvent( void )
{
	FactoryInitUHarDistorPara();
	FactoryInitIHarDistorPara();
	FactoryInitUHarOverRunPara();
	FactoryInitIHarOverRunPara();
	FactoryInitPHarOverRunPara();
	FactoryInitUInterHarOverRunPara();
	FactoryInitIInterHarOverRunPara();
	FactoryInitPInterHarOverRunPara();
	FactoryInitUShortFlickerPara();
	FactoryInitULongFlickerPara();
	FactoryInitUSagPara();
	FactoryInitUSwellPara();
	FactoryInitUInterruptionPara();

	FactoryInitUpdateAppPara();//升级

	FactoryInitJLChipFailPara();//计量芯片故障

	FactoryInitAdjustTimePara();//校时
	#if(PROTOCOL_VERSION == 25)
	FactoryInitUnknownDevPara();//未知设备

	FactoryInitSpecificDevPara();//特定设备
	#endif

	//FactoryInitMeterClearPara();
	memset( (BYTE *)&ReportPara, 0x00, sizeof(ReportPara)) ;
	ReportPara.CRC32 = lib_CheckCRC32((BYTE*)&ReportPara, sizeof(ReportPara)-sizeof(DWORD));
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ReportSafeRom.ReportPara ), sizeof(TReportPara), (BYTE *)&ReportPara );

	memset((BYTE*)&FollowPara,0x00,sizeof(FollowPara));
	FollowPara.CRC32 = lib_CheckCRC32((BYTE*)&FollowPara, sizeof(FollowPara)-sizeof(DWORD));
}

void api_UpdateAppEventStart(void)
{
	BYTE i;
	
	for( i = 1; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		if(SubEventInfoTab[i].EventIndex == eSUB_EVENT_UPDATE_NO)
		{
			SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_START, eEVENT_ENDTIME_CURRENT );
		}
	}
	
}

void api_UpdateAppEventEnd(void)
{
	BYTE i;

	for( i = 1; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		if(SubEventInfoTab[i].EventIndex == eSUB_EVENT_UPDATE_NO)
		{
			SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_CURRENT );
		}
	}

}

void api_MeterClearEventStart(void)
{
	BYTE i;
	
	for( i = 1; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		if(SubEventInfoTab[i].EventIndex == eSUB_EVENT_METER_CLEAR_NO)
		{
			SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_START, eEVENT_ENDTIME_CURRENT );
		}
	}
	
}

void api_MeterClearEventEnd(void)
{
	BYTE i;
	
	for( i = 1; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		if(SubEventInfoTab[i].EventIndex == eSUB_EVENT_METER_CLEAR_NO)
		{
			SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_CURRENT );
		}
	}
	
}

void api_AdjustTimeStart(void)
{
	BYTE i;

	for (i = 1; i < SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		if (SubEventInfoTab[i].EventIndex == eSUB_EVENT_ADJUST_TIME_NO)
		{
			SaveInstantEventRecord(SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_START, eEVENT_ENDTIME_CURRENT);
		}
	}
}

void api_AdjustTimeEnd(void)
{
	BYTE i;

	for (i = 1; i < SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		if (SubEventInfoTab[i].EventIndex == eSUB_EVENT_ADJUST_TIME_NO)
		{
			SaveInstantEventRecord(SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_CURRENT);
		}
	}
}
