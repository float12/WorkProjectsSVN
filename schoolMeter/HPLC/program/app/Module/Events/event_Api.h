//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.10.8
//描述		事件模块Api头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __EVENT_API
#define __EVENT_API
//--------------------------------------
#include "VICorrelate.h"
#include "ReversalUnBalance.h"
#include "BackpropOverload.h"
#include "LostPower.h"
#include "LostAllV.h"
#include "Cover.h"
#include "cosover.h"
#include "spiUnBalance.h"
#include "Magnetic.h"
#include "RelayErrEvent.h"
#include "PowerErr.h"
#include "statistics_voltage.h"
#include "PrgRecord.h"
#include "DemandOver.h"
#include "DeviceErr.h"

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

#if( MAX_PHASE == 1)
#define MAX_EVENT_OAD_NUM			(8+3)	// 最多支持N个OAD
#else
#define MAX_EVENT_OAD_NUM			(38+12)	// 最多支持n个OAD
#endif

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
   	eNormalFlag = 0x12341234,//掉电前为正常标志（单、三相）     
   	eLostALLVFlag = 0x5AA55AA5,//掉电前为全失压状态（三相）
    eLostPowerFlag = 0X56785678,//掉电前为掉电状态（三相）
    eOtherFlag = 0XAA55AA55,//其他状态防止异常复位（单相、三相）
}eLostALLV_LostPowerFlag;//数据加TAG标志

typedef struct TEventAttInfo_t
{
	DWORD	Oad[MAX_EVENT_OAD_NUM];			//OAD项目  4字节对齐
	BYTE	NumofOad;						//OAD的个数
	BOOL	bValidFlag;						//有效标识 class7(24) 属性9(12)。不采用，不判断，程序固定按照开启处理
	WORD	OadValidDataLen;				//有效的OAD数据总长度
	WORD	AllDataLen;						//sizeof(TEventOADCommonData)+OadValidDataLen+ExtDataLen
	BYTE	OadLen[MAX_EVENT_OAD_NUM];		//OAD项目对应的数据长度	
	WORD	MaxRecordNo;					//最大记录数
}TEventAttInfo;

//事件的数据区，无flash保存在eeprom，否则保存到flash
typedef struct TInstantEventRecord_t
{	
	//有无flash都要保存在eeprom中的数据
	#if( SEL_EVENT_V_UNBALANCE == YES )
	WORD				RomVUnBalanceRate;		//ROM中暂存的最大不平衡率
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	WORD				RomIUnBalanceRate;		//ROM中暂存的最大不平衡率
	#endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	WORD				RomISUnBalanceRate;		//ROM中暂存的最大不平衡率
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	TDemandData			PADemandOverMaxValue;
	TDemandData			NADemandOverMaxValue;
	TDemandData			QDemandOverMaxValue[EVENT_PHASE_FOUR];
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )
	TLowPowerMeterCoverData LowPowerMeterCoverData;
	#endif

	#if( SEL_EVENT_BUTTONCOVER == YES )
	TLowPowerButtonCoverData LowPowerButtonCoverData;
	#endif
	
	#if( MAX_PHASE != 1 )
	DWORD AHour[4][3][4];//保存安时值,第1下标[4]--失压、欠压、过压、断相;第2下标[3]--A、B、C相发生事件;第3下标[4]--发生事件时总、A、B、C相安时值
	#endif
	BYTE Temp;//防止单相表此结构体为空
}TInstantEventRecord;

// 事件参数门限值
typedef struct TEventPara_t
{
	DWORD	dwLimit[eNUM_OF_EVENT_ALL];		//电流
	WORD	wLimit[eNUM_OF_EVENT_ALL];		//电压
	BYTE	byLimit[eNUM_OF_EVENT_ALL];		//时间
	WORD	LostVRecoverLimitV;	// 失压事件电压恢复下限eEVENT_PARA_TYPE_SPECIAL1
	WORD	VRunLimitLV;	// 电压考核下限   eTYPE_SPEC1
	WORD	VRunRecoverHV;	// 电压合格上限   eTYPE_SPEC2
	WORD	VRunRecoverLV;	// 电压合格下限   eTYPE_SPEC3
	DWORD	LostILimitLI;	// 失流事件电流恢复下限eEVENT_PARA_TYPE_SPECIAL1
}TEventPara;

	
typedef struct TPowerDownFlag_t
{
	BYTE	ProgramPwrDwn;										// 编程记录掉电标志
	BYTE	InstantEventFlag[(SUB_EVENT_INDEX_MAX_NUM+7)/8];	// 工况类事件掉电标志
}TPowerDownFlag;

typedef struct TEventSafeRom_t
{
	TEventDataInfo			EventDataInfo[SUB_EVENT_INDEX_MAX_NUM];
	//645规约的编程记录，自己独立的数据源，单独设置一个记录号
	#if(SEL_PRG_RECORD645 == YES)
	TEventDataInfo			ProRecord645;
	#endif
	TLostPowerTime			LostPowerTime;
}TEventSafeRom;

typedef struct TEvent_t
{
	TEventPara				EventPara;							// 事件门限参数
	TEventAttInfo			EventAttInfo[eNUM_OF_EVENT_PRG];	// 所有事件（操作类，编程类，工况类）的OAD信息,可以统一
	TProgramRecordRom		ProgramRecordRom;					// 操作类记录+编程记录
	TInstantEventRecord		InstantEventRecord;					// 工况类事件记录
	TPowerDownFlag			api_PowerDownFlag;						// 掉电保存各事件的标志
	BYTE					AutoWarnAtt[eNUM_OF_EVENT_PRG];		// 主动上报标识		
	#if( SEL_STAT_V_RUN == YES )
	TStatVRunDataRom 		StatVRunDataRom;					// 电压合格率统计
	#endif
}TEventConRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TEventPara	gEventPara;	// 事件门限及延时参数

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 判断事件状态
//
//参数: 无
//                    
//返回值: 无
//			
//备注: 大循环调用
//-----------------------------------------------
void api_EventTask(void);

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
WORD api_ReadEventAttribute( WORD OI, BYTE No, WORD Len, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 添加事件关联对象属性
//
//参数: 
//	OI[in]: 事件OI
//  
//	pOad[in]:	对象属性设置时的OAD
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:   添加一个OAD，所有的重新计，重新分配EEP空间（默认不会添加相同的OAD）
//-----------------------------------------------
BOOL api_AddEventAttribute( WORD OI, BYTE *pOad );

//-----------------------------------------------
//函数功能: 删除事件关联对象属性
//
//参数: 
//	OI[in]:	事件OI
//  
//	pOad[in]:	要删除的OAD,属性标识及特征的bit5~bit7代表各类冻结的冻结方案
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:   删掉一个OAD，所有的重新计，重新分配EEP空间
//-----------------------------------------------
BOOL api_DeleteEventAttribute( WORD OI, BYTE *pOad );

//-----------------------------------------------
//函数功能: 设置事件关联对象属性
//
//参数: 
//	OI[in]:	事件OI
//  
//  OadType[in]:
//			0 ~ 代表所有的OAD
//			N ~ 属性列表里的第N个OAD  设置第N个OAD时，认为第N个OAD之前是有值的
//	
//  pOad[in]:  指向OAD Buffer的指针  2字节冻结周期+4字节OAD+2字节存储深度
//                
//  OadNum[in]:	OAD的个数
//
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注:	所有的重新计，重新分配EEP空间
// 		
//-----------------------------------------------
BOOL api_WriteEventAttribute( WORD OI, BYTE OadType, BYTE *pOad, WORD OadNum );

//-----------------------------------------------
//函数功能: 获取掉电时间
//
//参数: 
//	PowerDownTime[out]:	掉电时间 
//  
//返回值:				无  
//备注:
//-----------------------------------------------
void api_GetPowerDownTime( TRealTimer *pPowerDownTime );

//-----------------------------------------------
//函数功能: 保存操作类每个事件的编程记录（清电能，时区时段表编程等）
//
//参数:
//	inEventIndex[in]:	编程类型枚举号
//
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_SavePrgOperationRecord( eEVENT_INDEX inEventIndex );

//-----------------------------------------------
//函数功能: 保存编程事件记录
//
//参数: 
//	Type[in]:	EVENT_START:	编程开始，设置数据时置开始
//				EVENT_END:		编程结束，断开应用连接或者掉电时置结束
//  DIType[in]:	0x51--OAD	0X53--OMD
//	pOad[in]:	指向OAD的指针
// 
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注: 只存第一个OAD的时间为开始时间
//		后续其他的OAD只保存OAD，不再存OAD对应数据
//		结束本次事件记录时，pOad无效
//-----------------------------------------------
BOOL api_SaveProgramRecord( BYTE Type, BYTE DIType, BYTE *pOad );

//-----------------------------------------------
//函数功能: 读事件记录
//
//参数: 
//	Tag[in]:	0 不加Tag 
//				1 加Tag
//                
//	pDLT698RecordPara[in]: 读取冻结的参数结构体
//                
//  Len[in]：输入的buf长度 用来判断buf长度是否够用
//                
//  pBuf[out]: 返回数据的指针，有的传数据，没有的补0
//				
//	
//  返回值:	bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//			bit0~bit14 代表返回的冻结数据长度
//
//备注: 只支持上Last条记录
//-----------------------------------------------
WORD api_ReadEventRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_FactoryInitEvent( void );

//-----------------------------------------------
//函数功能: 清事件记录数据
//
//参数: 
//  Type[in]:	eEVENT_CLEAR_TYPE
//	
//返回值:	无
//  
// 备注:
//-----------------------------------------------
BOOL api_ClrAllEvent( BYTE Type );

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
BOOL api_ClrSepEvent( WORD OI );

//-----------------------------------------------
//函数功能: 写事件参数
//
//参数: 
//			OI[in]:					OI
//			No[in]:					0: 全部参数
//									N: 第N个参数
//			pBuf[in]:				写数据的缓冲
//                    
//返回值:  	TRUE   正确
//			FALSE  错误
//
//备注:  	写EEP+RAM  698规约调用
//-----------------------------------------------
BOOL api_WriteEventPara( WORD OI, BYTE No, BYTE *pBuf );

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
BOOL api_ReadEventPara( WORD OI, BYTE No, BYTE *pLen, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 掉电事件处理
//
//参数:  无
//                    
//返回值: 无
//
//备注: 
//-----------------------------------------------
void api_PowerDownEvent(void);

//-----------------------------------------------
//函数功能: 暂存编程记录之前数据项的OAD及数据
//
//参数:  	Type[in]: 0--OAD	1--OMD
//			dwData[in]: 要设置的参数OAD 高字节在前，低字节在后
//                  
//返回值: TRUE/FALSE
//
//备注:规约调用编程记录时，在设置参数之前要先调用此函数。 
//-----------------------------------------------
BOOL api_WritePreProgramData( BYTE Type,DWORD dwData );


//-----------------------------------------------
//函数功能: 读取当前记录数，最大记录数
//
//参数:  
//  OI[in]: OI
//   
//  Phase[in]: 相位0总,1A,2B,3C
//             
//  Type[in]: 参考eEVENT_RECORD_NO_TYPE
//                
//  pBuf[out]：返回数据的缓冲
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注: 
//-----------------------------------------------
BOOL api_ReadEventRecordNo( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 读取当前值记录表(无功组合方式1特征字、无功组合方式2特征字编程次数)
//
//参数:  
//  byType[in]: 	0--无功组合方式1特征字, 1--无功组合方式2特征字
//  pBuf[out]：	返回数据的缓冲
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注: 事件类的属性7  事件发生源+发生次数+累计时间
//-----------------------------------------------
BOOL api_ReadCurrentRecordTable_Q_Combo( BYTE byType, BYTE *pBuf );

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
//备注: 读事件类读最近1次发生时间、结束时间(Class 7有发生源)，或读发生次数+累计时间(Class 7有发生源)
//-----------------------------------------------
WORD api_ReadCurrentRecordTable( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf );
#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//函数功能: 读取645当前值记录表
//
//参数:  
//  OI[in]: 	OI      
//  Phase[in]: 	相位0总,1A,2B,3C 
//	Type[in]:	0--读最近1次发生时间、结束时间
//              1--读发生次数
//              2--读发生次数+累计时间
//  pBuf[out]：	返回数据的缓冲
//                    
//返回值:	0x8000--读取失败，其它--返回读取长度
//
//备注: 
//-----------------------------------------------
WORD api_ReadCurrentRecordTable645( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf );
//-----------------------------------------------
//函数功能: 读事件记录
//
//参数: 
//		Tag[in]:				0 不加Tag 1 加Tag                             
//  	Len[in]：				输入的buf长度 用来判断buf长度是否够用                
//  	pBuf[out]: 				返回数据的指针，有的传数据，没有的补0	
//返回值:			bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//					bit0~bit14 代表返回的冻结数据长度
//备注: 只支持上Last条记录
//-----------------------------------------------
WORD api_ReadEvent645RecordByNo( TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );
#endif
//-----------------------------------------------
//函数功能: 处理事件状态
//
//参数: 
//			Type[in]：			eDEAL_EVENT_STATUS
//                    
//			inSubEventIndex[in]	eSUB_EVENT_INDEX
//                    
//返回值: Type为eGET_EVENT_STATUS时，1为事件发生还未结束，0为事件未发生 	
//
//备注:   
//-----------------------------------------------
BYTE api_DealEventStatus(BYTE Type, BYTE inSubEventIndex );

#if ( SEL_EVENT_LOST_V == YES )
//-----------------------------------------------
//函数功能: 读取失压属性13
//
//参数:  
//  Index[in]:	0 所有 
// 				1 事件发生总次数 
// 				2 事件总累计时间    
// 				3 最近一次失压发生时间
// 				4 最近一次失压结束时间
//   
//  pBuf[out]：返回数据的缓冲
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_ReadEventLostVAtt( BYTE Index, BYTE *pBuf );
#endif
// 上电事件处理
void api_PowerUpEvent(void);


#if( SEL_EVENT_LOST_ALL_V == YES )
//-----------------------------------------------
//函数功能:同步电流值
//
//参数: 
//	Type[in]:	0、1、2、3代表A\B\C\N电流 
//	Value[in]:	电流值 4位小数 
 
//返回值:				无
//备注:每次采样时同步此值，供在处理全失压用
//-----------------------------------------------
void api_SetLostAllVCurrent( BYTE Type, long Value );
#endif

#if(SEL_PRG_RECORD645 == YES)
//-----------------------------------------------
//函数功能: 保存645规约编程事件记录
//
//参数:
//	Type[in]:	EVENT_START:	编程开始，设置数据时置开始
//				EVENT_END:		编程结束，断开应用连接或者掉电时置结束
//	Op[in]:		操作者代码 倒序
//	pDI[in]:	指向DI的指针 倒序
//
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注:设置数据时Type为EVENT_START，在认证结束的调用Type为EVENT_END
//-----------------------------------------------
BOOL api_SaveProgramRecord645( BYTE Type, BYTE *Op, BYTE *pDI );

//-----------------------------------------------
//函数功能: 645规约编程事件记录（03300000~0330000a）
//
//参数:
//	DI[in]:		读取数据的数据标识
//				EVENT_END:		编程结束，断开应用连接或者掉电时置结束
//	pOutBuf[out]:	返回的数据
//
//  返回值:	数据长度 8000代表错误
//
//备注:支持编程次数及最近10次编程记录的读取
//-----------------------------------------------
WORD api_ReadProgramRecord645( BYTE *DI, BYTE *pOutBuf );
#endif//#if(SEL_PRG_RECORD645 == YES)

//-----------------------------------------------
//功能描述  : 进入低功耗前进行开盖数据检查
//参数:
//         
//
//返回值:     
//备注内容:只能在进入低功耗前调用 低功耗下不能调用
//-----------------------------------------------
void api_LowPowerCheckMeterCoverStatus( void );
//-----------------------------------------------
//功能描述  : 进入低功耗前进行开盖数据检查
//参数:
//         
//
//返回值:     
//备注内容:只能在进入低功耗前调用 低功耗下不能调用
//-----------------------------------------------
void api_LowPowerCheckButtonCoverStatus( void );


#endif//对应文件最前面的#define __EVENT_API
