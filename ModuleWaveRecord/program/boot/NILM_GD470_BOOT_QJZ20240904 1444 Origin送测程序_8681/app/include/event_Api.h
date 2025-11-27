//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.10.8
//描述		事件模块Api头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __EVENT_API
#define __EVENT_API

#include "event.h"
//-----------------------------------------------
//				宏定义
//-----------------------------------------------

#define MAX_EVENT_OAD_NUM			20// 最多支持n个OADs

#define OAD_HARMONIC_U_DISTORTION       0X3039	//电压谐波总畸变率超限事件
#define OAD_HARMONIC_I_DISTORTION       0X303A	//电流谐波总畸变率超限事件
#define OAD_HARMONIC_U_OVERRUN          0X3041	//谐波电压含量超限事件
#define OAD_HARMONIC_I_OVERRUN          0X3042	//谐波电流含量超限事件
#define OAD_HARMONIC_P_OVERRUN          0X3043	//谐波功率超限事件
#define OAD_INTER_HARMONIC_U_OVERRUN    0X3044	//间谐波电压含量超限事件
#define OAD_INTER_HARMONIC_I_OVERRUN    0X3045	//间谐波电流含量超限事件
#define OAD_INTER_HARMONIC_P_OVERRUN    0X3046	//间谐波功率超限事件
#define OAD_SHORT_FLICKER_U_OVERRUN     0X3047	//电压短时闪变超限事件
#define OAD_LONG_FLICKER_U_OVERRUN      0X3048	//电压长时闪变超限事件
#define OAD_SAG_U                       0X3049	//电压暂降事件
#define OAD_SWELL_U                     0X304A	//电压暂升事件
#define OAD_INTERRUPTION_U              0X304B	//电压中断事件
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct TEventAttInfo_t
{
	DWORD	Oad[MAX_EVENT_OAD_NUM];			//OAD项目  4字节对齐
	BYTE	NumofOad;						//OAD的个数
	BOOL	bValidFlag;						//有效标识 class7(24) 属性9(12)。不采用，不判断，程序固定按照开启处理
	WORD	OadValidDataLen;				//有效的OAD数据总长度
	WORD	AllDataLen;						//sizeof(TEventOADCommonData)+OadValidDataLen+ExtDataLen
	WORD	OadLen[MAX_EVENT_OAD_NUM];		//OAD项目对应的数据长度	
	WORD	MaxRecordNo;					//一页FLASH最大记录数
}TEventAttInfo;

// 事件参数门限值
typedef struct TEventPara_t
{
	WORD	SetTimeLimit;					// 广播校时范围
}TEventPara;

typedef struct TPowerDownFlag_t
{
	BYTE	ProgramPwrDwn;										// 编程记录掉电标志
	BYTE	InstantEventFlag[(SUB_EVENT_INDEX_MAX_NUM+7)/8];	// 工况类事件掉电标志
}TPowerDownFlag;

typedef struct TEventSafeRom_t
{
	TEventDataInfo			EventDataInfo[SUB_EVENT_INDEX_MAX_NUM];	
	TLostPowerTime			LostPowerTime;
}TEventSafeRom;

typedef struct TEvent_t
{
	TEventPara				EventPara;							// 事件门限参数
	TEventAttInfo			EventAttInfo[eNUM_OF_EVENT_PRG];	// 所有事件（操作类，编程类，工况类）的OAD信息,可以统一
//	TProgramRecordRom		ProgramRecordRom;					// 操作类记录+编程记录
	TPowerDownFlag			PowerDownFlag;						// 掉电保存各事件的标志
	BYTE					AutoWarnAtt[eNUM_OF_EVENT_PRG];		// 主动上报标识
//	TLostAllVData			LostAllVData;						// 低功耗下记录的电压电流
}TEventConRom;

typedef struct TReportPara_t
{
	BYTE		Times;		//上报次数
	BYTE		OAD_Num;	//上报OAD个数
	DWORD		ReportOAD[SUB_EVENT_INDEX_MAX_NUM];
	DWORD		CRC32;
}TReportPara;

typedef struct TReportSafeRom_t
{
	TReportPara ReportPara;
}TReportSafeRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

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
BOOL api_GetPowerDownTime( TRealTimer *pPowerDownTime );

//-----------------------------------------------
//函数功能: 保存操作类事件记录（清电能，时区时段表编程等）
//
//参数: 
//	OI[in]:	OI
//  
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_SavePrgOperationRecord( WORD OI );

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

//-----------------------------------------------
//函数功能: 处理事件延时后的状态
//
//参数: 
//			Type[in]：			eDEAL_EVENT_DELAYED_STATUS
//                    
//			inSubEventIndex[in]	eSUB_EVENT_INDEX
//                    
//返回值: Type为eGET_EVENT_STATUS时，1为事件发生还未结束，0为事件未发生 	
//
//备注:   
//-----------------------------------------------
BYTE api_DealEventDelayedStatus(eDEAL_EVENT_DELAYED_STATUS Type, BYTE inSubEventIndex );

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
BYTE api_DealEventCurrentStatus(eDEAL_EVENT_CURRENT_STATUS Type, BYTE inSubEventIndex );


//-----------------------------------------------
//函数功能: 上电事件处理
//
//参数: 无
//                    
//返回值: 无
//			
//备注: 
//-----------------------------------------------
void api_PowerUpEvent(void);

//-----------------------------------------------
//函数功能: 保存管理新升级事件记录
//
//参数: 
//	pBuf[in]:
//  
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_SaveManageUpDateRecord( BYTE *pBuf );

//-----------------------------------------------
//函数功能: 获取停电期间数据
//
//参数: Type[in]: 	0:全部获取
//					1:停电期间电压
//					2:停电期间电流
//
//返回值:  	数据长度
//
//备注:停电期间数据用于计量芯进入低功耗10秒时保存采样数据，供管理芯上电时读取以判断全失压等事件。
//-----------------------------------------------
WORD api_GetLostPowerDate( BYTE Type, BYTE *InBuf );


//-----------------------------------------------
//函数功能: 设置双芯事件上报列表
//
//参数:
//		inSubEventIndex[in]:	事件索引号
//
//返回值:	无
//			
//-----------------------------------------------
void SetDoubleChipReportIndex( BYTE inSubEventIndex );

//-----------------------------------------------
//功能描述  ：掉电按键检测功能配置
//
//参数:  
//
//返回值: 
//
//备注内容  :   
//-----------------------------------------------
void api_PowerDownSetCoverReg( void );

//-----------------------------------------------
//函数功能: 获取事件主动上报APDU
//
//参数: 
//  
//返回值:				TRUE/FALSE  
//备注:  
//-----------------------------------------------
WORD api_GetReportEventAPDU( BYTE* Buf );


#endif//对应文件最前面的#define __EVENT_API
