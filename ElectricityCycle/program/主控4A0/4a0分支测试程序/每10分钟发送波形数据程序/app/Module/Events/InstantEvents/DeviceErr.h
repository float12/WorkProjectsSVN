//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.11.14
//描述		外围器件异常头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __DEVICEERR_H
#define __DEVICEERR_H

#include "event.h"

#if( (SEL_EVENT_RTC_ERR == YES)||(SEL_EVENT_SAMPLECHIP_ERR == YES) )
//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
#if( SEL_EVENT_RTC_ERR == YES )
// 时钟故障
typedef struct TRtcErrRom_t
{
	TEventOADCommonData		EventOADCommonData;
	BYTE					EventData[sizeof(TEventNormal)];
}TRtcErrRom;
#endif

#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
// 计量芯片故障
typedef struct TSampleChipErrRom_t
{
	TEventOADCommonData		EventOADCommonData;
	BYTE					EventData[sizeof(TEventNormal)];
}TSampleChipErrRom;
#endif
//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
#if( SEL_EVENT_RTC_ERR == YES )
//-----------------------------------------------
//函数功能: 获取当前时钟故障状态
//
//参数:  
//  Phase[in]: 无效          
//   
//返回值:	1   有故障
//			0	无故障
//
//备注: 
//-----------------------------------------------
BYTE GetRtcErrStatus(BYTE Phase);
//-----------------------------------------------
//函数功能: 时钟故障事件的上电处理
//
//参数:  无
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
void RtcErrPowerOn( void );

//-----------------------------------------------
//函数功能: 初始化时钟故障事件参数
//
//参数:  无
//   
//返回值:	无
//
//备注: 
//-----------------------------------------------
void api_FactoryInitRtcErrPara( void );
#endif//#if( SEL_EVENT_RTC_ERR == YES )

#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
BYTE GetSampleChipErrStatus(BYTE Phase);
void FactoryInitSampleChipErrPara( void );
#endif//#if( SEL_EVENT_SAMPLECHIP_ERR == YES )

#endif//#if( (SEL_EVENT_RTC_ERR == YES)||(SEL_EVENT_SAMPLECHIP_ERR == YES) )

#endif//#ifndef __DEVICEERR_H
