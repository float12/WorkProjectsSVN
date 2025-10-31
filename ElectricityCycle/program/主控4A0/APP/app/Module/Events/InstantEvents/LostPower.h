#ifndef __LOST_POWER
#define __LOST_POWER

#include "event.h"

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

// 掉电事件
typedef struct TLostPowerRom_t
{
	TEventOADCommonData		EventOADCommonData;							// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TEventNormal)];
}TLostPowerRom;


// 辅助电源掉电事件
typedef struct TLostSecPowerRom_t
{
	TEventDataInfo			EventDataInfo;
	TEventOADCommonData		EventOADCommonData[MAX_EVENTRECORD_NUMBER];							// 每个编程类都有的共同数据
	BYTE					EventData[sizeof(TEventNormal)*MAX_EVENTRECORD_NUMBER];
}TLostSecPowerRom;

typedef struct TLostPowerPara_t
{
	BYTE	Delay;
}TLostPowerPara;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern __no_init DWORD PowerDownLostSecPowerFlag;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 判断是否掉电
//
//参数:  无
//                    
//返回值:	掉电状态 0：未掉电  1：掉电
//
//备注: 
//-----------------------------------------------
BYTE GetLostPowerStatus( BYTE Phase );

//-----------------------------------------------
//函数功能: 掉电事件的上电处理
//
//参数:  无
//                    
//返回值:	无
//
//备注: 
//-----------------------------------------------
void LostPowerPowerOn( void );

//-----------------------------------------------
//函数功能: 清掉电相关数据
//
//参数:  无
//                    
//返回值:	无
//
//备注: 
//-----------------------------------------------
void ClearLostPowerData( void );

//-----------------------------------------------
//函数功能: 设置掉电标志
//
//参数:  无
//                    
//返回值:	无
//
//备注: 
//-----------------------------------------------
void SetLostPowerStatus( void );

//-----------------------------------------------
//函数功能: 清掉电标志
//
//参数:  无
//                    
//返回值:	无
//
//备注: 
//-----------------------------------------------
void ClearLostPowerStatus( void );
void FactoryInitLostPowerPara( void );
void LostPowerPowerDown( void );
void LostSecPowerPowerOff( void );
BYTE GetLostSecPowerStatus(BYTE Phase);
#endif//#ifndef __LOST_POWER
