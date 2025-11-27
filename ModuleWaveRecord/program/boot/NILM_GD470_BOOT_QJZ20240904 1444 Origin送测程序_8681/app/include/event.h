//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.10.8
//描述		事件模块内部头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __EVENT_H
#define __EVENT_H

//--------------------------------------

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define EVENT_UNIT_LEN				(SECTOR_SIZE/10)	//一个事件最大长度
#define EVENT_PHASE_FOUR			4		// 潮流反向，无功需量超限为4相
#define MAX_EVENTRECORD_NUMBER		10		// 事件记录，编程记录最多支持10次记录
#define MAX_PROGRAM_OAD_NUMBER		10		// 一次编程记录最多支持10个OAD
#define EVENT_MAX_PARA_NUM			4		// 一个事件最多4个参数
#define EVENT_SECTOR_NUM			(2)		// 每一个事件所需的扇区数

#define LOST_POWER_FLAG				0xAA	//掉电标志
#define NO_LOST_FLAG				0		//无全失压、无掉电

#define	EVENT_START					BIT0//事件发生
#define	EVENT_END					BIT1//事件结束

// 所有OAD对应的数据总长度的最大值@@
#define EVENT_VALID_OADLEN			2048				
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
// 事件清零类型
typedef enum
{
	eFACTORY_INIT = 0,					// 清所有事件，包含电表清零记录，事件清零记录
	eEXCEPT_CLEAR_METER,				// 清所有事件，不包含电表清零记录
	eEXCEPT_CLEAR_METER_CLEAR_EVENT		// 清所有事件，不包含电表清零记录，事件清零记录
}eEVENT_CLEAR_TYPE;

// 事件记录数类型
typedef enum
{
	eEVENT_CURRENT_RECORD_NO = 0,	// 当前记录数
	eEVENT_MAX_RECORD_NO			// 最大记录数
}eEVENT_RECORD_NO_TYPE;

// 获得，设置，清零事件状态枚举类型
typedef enum
{
  eCLEAR_EVENT_DELAYED_STATUS = 0,		 //清除事件延时状态
  eSET_EVENT_DELAYED_STATUS,			 //设置事件延时状态
  eGET_EVENT_DELAYED_STATUS,			 //获取事件延时状态
}eDEAL_EVENT_DELAYED_STATUS;

typedef enum
{
  eCLEAR_EVENT_CURRENT_STATUS = 3,	 	//清除事件当前状态
  eSET_EVENT_CURRENT_STATUS,	 		//设置事件当前状态
  eGET_EVENT_CURRENT_STATUS,	 		//获取事件当前状态
}eDEAL_EVENT_CURRENT_STATUS;
 

typedef enum
{
	eEVENT_ENDTIME_CURRENT = 0, // 当前时间
	eEVENT_ENDTIME_PWRDOWN		// 掉电时间
}eEVENT_ENDTIME_TYPE;

// 编程记录状态
typedef enum
{
	ePROGRAM_NOT_YET_START = 0,	// 编程还未开始
	ePROGRAM_STARTED			// 编程已经开始
}ePROGRAM_STATUS;

// 事件门限参数类型
typedef enum
{
	eTYPE_BYTE = 0,	// BYTE类型参数，延时
	eTYPE_WORD,		// WORD类型参数
	eTYPE_DWORD,	// DWORD类型参数
	eTYPE_SPEC1,	// 特殊类型参数1
	eTYPE_SPEC2,	// 特殊类型参数2
	eTYPE_SPEC3,	// 特殊类型参数3
	eTYPE_NULL,		// 参数无效
}eEVENT_PARA_TYPE;

//此定义应与SubEventInfoTab定义顺序一致，与eEVENT_INDEX顺序一致
typedef enum
{	
	//电压谐波总畸变率超限事件
	eSUB_EVENT_HARMONIC_U_DISTORTION_A = 0,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_HARMONIC_U_DISTORTION_B,
	eSUB_EVENT_HARMONIC_U_DISTORTION_C,	
	#endif
	
	//电流谐波总畸变率超限事件
	eSUB_EVENT_HARMONIC_I_DISTORTION_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_HARMONIC_I_DISTORTION_B,
	eSUB_EVENT_HARMONIC_I_DISTORTION_C,
	#endif

	//谐波电压含量超限事件
	eSUB_EVENT_HARMONIC_U_OVERRUN_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_HARMONIC_U_OVERRUN_B,
	eSUB_EVENT_HARMONIC_U_OVERRUN_C,
	#endif

	//谐波电流含量超限事件
	eSUB_EVENT_HARMONIC_I_OVERRUN_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_HARMONIC_I_OVERRUN_B,
	eSUB_EVENT_HARMONIC_I_OVERRUN_C,
	#endif

	//谐波功率含量超限事件
	eSUB_EVENT_HARMONIC_P_OVERRUN_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_HARMONIC_P_OVERRUN_B,
	eSUB_EVENT_HARMONIC_P_OVERRUN_C,
	#endif

	//间谐波电压含量超限事件
	eSUB_EVENT_INTER_HARMONIC_U_OVERRUN_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_INTER_HARMONIC_U_OVERRUN_B,
	eSUB_EVENT_INTER_HARMONIC_U_OVERRUN_C,
	#endif

	//间谐波电流含量超限事件
	eSUB_EVENT_INTER_HARMONIC_I_OVERRUN_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_INTER_HARMONIC_I_OVERRUN_B,
	eSUB_EVENT_INTER_HARMONIC_I_OVERRUN_C,
	#endif

	//间谐波功率含量超限事件
	eSUB_EVENT_INTER_HARMONIC_P_OVERRUN_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_INTER_HARMONIC_P_OVERRUN_B,
	eSUB_EVENT_INTER_HARMONIC_P_OVERRUN_C,
	#endif

	//电压短时闪变超限事件
	eSUB_EVENT_SHORT_FLICKER_U_OVERRUN_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_SHORT_FLICKER_U_OVERRUN_B,
	eSUB_EVENT_SHORT_FLICKER_U_OVERRUN_C,
	#endif

	//电压长时闪变超限事件
	eSUB_EVENT_LONG_FLICKER_U_OVERRUN_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_LONG_FLICKER_U_OVERRUN_B,
	eSUB_EVENT_LONG_FLICKER_U_OVERRUN_C,
	#endif

	//电压暂降事件
	eSUB_EVENT_SAG_U_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_SAG_U_B,
	eSUB_EVENT_SAG_U_C,
	#endif

	//电压暂升事件
	eSUB_EVENT_SWELL_U_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_SWELL_U_B,
	eSUB_EVENT_SWELL_U_C,
	#endif

	//电压中断事件
	eSUB_EVENT_INTERRUPTION_U_A,
	#if(MAX_PHASE == 3)
	eSUB_EVENT_INTERRUPTION_U_B,
	eSUB_EVENT_INTERRUPTION_U_C,
	#endif

	eSUB_EVENT_STATUS_TOTAL,

}eSUB_EVENT_INDEX;

//此定义应与EventInfoTab定义顺序一致，与eSUB_EVENT_INDEX顺序一致
typedef enum
{
	// vvvvvvvvvvv工况类记录vvvvvvvvvvvvvv
	eEVENT_HARMONIC_U_DISTORTION_NO,		//电压谐波总畸变率超限

	eEVENT_HARMONIC_I_DISTORTION_NO,		//电流谐波总畸变率超限

	eEVENT_HARMONIC_U_OVERRUN_NO,			//谐波电压含量超限事件

	eEVENT_HARMONIC_I_OVERRUN_NO,			//谐波电流含量超限事件

	eEVENT_HARMONIC_P_OVERRUN_NO,			//谐波功率含量超限事件

	eEVENT_INTER_HARMONIC_U_OVERRUN_NO,		//间谐波电压含量超限事件

	eEVENT_INTER_HARMONIC_I_OVERRUN_NO,		//间谐波电流含量超限事件

	eEVENT_INTER_HARMONIC_P_OVERRUN_NO,		//间谐波功率含量超限事件	
	
	eEVENT_SHORT_FLICKER_U_OVERRUN_NO,		//电压短时闪变超限事件
	
	eEVENT_LONG_FLICKER_U_OVERRUN_NO,		//电压长时闪变超限事件

	eEVENT_SAG_U_NO,						//电压暂降事件

	eEVENT_SWELL_U_NO,						//电压暂升事件

	eEVENT_INTERRUPTION_U_NO,				//电压中断事件
	
	eNUM_OF_EVENT,							// 工况类的事件数量13
	
	ePRG_RECORD_NO = eNUM_OF_EVENT,			//计量芯不支持编程记录

	eNUM_OF_EVENT_PRG,						// 事件和编程类总数

	// vvvvvvvvvvv之后的事件不存储记录vvvvvvvvvvvvvv

	eNUM_OF_EVENT_ALL,						// 所有的事件总数，包括不存储记录的
	
}eEVENT_INDEX;

#define SUB_EVENT_INDEX_MAX_NUM	eSUB_EVENT_STATUS_TOTAL		// 事件状态位数量
typedef struct TEventStatus_t
{
	BYTE CurrentStatus[(SUB_EVENT_INDEX_MAX_NUM+7)/8];	// 事件实时状态
	BYTE DelayedStatus[(SUB_EVENT_INDEX_MAX_NUM+7)/8]; // 经过延时后的状态
}TEventStatus;

#pragma pack(1)
// 时间数据
typedef struct TEventTime_t
{
	// 发生时刻
	TRealTimer BeginTime;
	// 结束时刻
	TRealTimer EndTime;

}TEventTime;

typedef struct TLostPowerTime_t
{
	TRealTimer	Time;		// 掉电时间
	DWORD		CRC32;
}TLostPowerTime;
#pragma pack()

typedef struct TEventOADCommonData_t
{
	DWORD			EventNo;					//事件记录序号@@
	TEventTime		EventTime;					//事件发生和结束时间
}TEventOADCommonData;

typedef struct TEventDataInfo_t
{
	DWORD	RecordNo;						//事件记录序号(单调递增)
	DWORD	AccTime;						//事件累计时间(只有工况类事件有效)
	DWORD	CRC32;
}TEventDataInfo;

typedef struct TEventAddrLen_t
{
	DWORD	dwAttInfoEeAddr;
	DWORD	dwDataInfoEeAddr;
	DWORD	dwRecordAddr;					//数据存储地址，第三存储空间
	WORD	wDataLen;						//10条记录空间大小
	BYTE	EventIndex;
	BYTE	Phase;
	BYTE	SubEventIndex;
}TEventAddrLen;

typedef struct TSubEventInfoTab_t
{
	eSUB_EVENT_INDEX	SubEventIndex;
	eEVENT_INDEX		EventIndex;	
	ePHASE_TYPE			Phase;
	BYTE 	(*pGetEventStatus)( BYTE Phase );	// 获取事件状态
}TSubEventInfoTab;

typedef struct TEventInfoTab_t
{
	WORD	OI;
	BYTE 	EventIndex;							//Event类型
	BYTE 	NumofPhase;							//相位数  潮流反向，无功需量超限为4相
	BYTE	SpecialProc;						//掉电特殊处理
	BYTE	ExtDataLen;							//扩展数据长度 如事件清零记录的OMD
	BYTE 	ParaNum;							//参数的个数
	BYTE 	ParaType[EVENT_MAX_PARA_NUM];		//参数的类型
}TEventInfoTab;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TEventStatus EventStatus;
extern const TSubEventInfoTab	SubEventInfoTab[];

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 保存工况类事件记录（失压失流等）
//
//参数: 
//	inEventIndex[in]:	eEVENT_INDEX 
//	Phase[in]:			ePHASE_TYPE  
//	BeforeAfter[in]:	事件发生前/发生后 eEVENT_TIME_TYPE
//  					发生时,次数+1,偏移++,结束时指针--  
//	EndTimeType[in]:	eEVENT_ENDTIME_CURRENT  当前时间
// 						eEVENT_ENDTIME_PWRDOWN  掉电时间
//  
//返回值:				TRUE/FALSE  
//备注:
//-----------------------------------------------
BOOL SaveInstantEventRecord( BYTE inEventIndex, BYTE Phase, BYTE BeforeAfter, BYTE EndTimeType );

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
BOOL GetEventInfo( TEventAddrLen *pEventAddrLen );

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
BOOL GetEventIndex( WORD OI, BYTE *pEventIndex );

//-----------------------------------------------
//函数功能: 根据事件类型及相位确定分相事件枚举号
//
//参数: 
//			EventIndex[in]:	事件枚举号                    
//			Phase[in]:		相位                    
//返回值:	分相事件枚举号	
//备注:
//-----------------------------------------------
BYTE GetSubEventIndex(TEventAddrLen *pEventAddrLen);


BYTE GetUHarDistorStatus(BYTE Phase);
BYTE GetIHarDistorStatus(BYTE Phase);
BYTE GetUHarOverRunStatus(BYTE Phase);
BYTE GetIHarOverRunStatus(BYTE Phase);
BYTE GetPHarOverRunStatus(BYTE Phase);
BYTE GetUInterHarOverRunStatus(BYTE Phase);
BYTE GetIInterHarOverRunStatus(BYTE Phase);
BYTE GetPInterHarOverRunStatus(BYTE Phase);
BYTE GetUShortFlickerStatus(BYTE Phase);
BYTE GetULongFlickerStatus(BYTE Phase);
BYTE GetUSagStatus(BYTE Phase);
BYTE GetUSwellStatus(BYTE Phase);
BYTE GetUInterruptionStatus(BYTE Phase);

void FactoryInitUHarDistorPara(void);
void FactoryInitIHarDistorPara(void);
void FactoryInitUHarOverRunPara(void);
void FactoryInitIHarOverRunPara(void);
void FactoryInitPHarOverRunPara(void);
void FactoryInitUInterHarOverRunPara(void);
void FactoryInitIInterHarOverRunPara(void);
void FactoryInitPInterHarOverRunPara(void);
void FactoryInitUShortFlickerPara(void);
void FactoryInitULongFlickerPara(void);
void FactoryInitUSagPara(void);
void FactoryInitUSwellPara(void);
void FactoryInitUInterruptionPara(void);
#endif//对应文件最前面的#define __EVENT_H
