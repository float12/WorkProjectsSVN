//----------------------------------------------------------------------
// Copyright (C) 2022-2028 烟台东方威思顿电气股份有限公司低压台区产品部
// 创建人
// 创建日期
// 描述

// 修改记录
//----------------------------------------------------------------------
#ifndef __CANBUS_H
#define __CANBUS_H

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define MAX_CANBUS_ID_NUM       (32)   // 最多32个接收ID
#define MAX_CHARGING_NUM        (16)
#define MAX_CHARGING_W_BITNUM   (15)
#define MAX_CHARGING_E_BITNUM   (59)
#define MAX_CHARGING_PE_BITNUM  (25)
#define BIT_NEED_BYTE(X)		((X / 8) + ((X % 8) ? 1 : 0))
#define MAX_CHARGING_W_BYTENUM  (BIT_NEED_BYTE(MAX_CHARGING_W_BITNUM))
#define MAX_CHARGING_E_BYTENUM  (BIT_NEED_BYTE(MAX_CHARGING_E_BITNUM))
#define MAX_CHARGING_PE_BYTENUM (BIT_NEED_BYTE(MAX_CHARGING_PE_BITNUM))
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eADDID,
	eDELID,
	eCLEANID
} eIDDOtype;
typedef enum
{
	eChargingGet,
	eChargingSet
} eCharging698type;
#pragma pack(1)
// 保护控制字1
typedef union TCanBusID_t
{
	struct
	{
		BYTE TxID      : 8;   // 发送方ID
		BYTE RxID      : 8;   // 接收方ID
		BYTE Rev1      : 7;   // 保留
		BYTE Next      : 1;   // 后续帧标志
		BYTE Direction : 1;   // 方向
		BYTE Broadcast : 1;   // 广播
		BYTE Priority  : 3;   // 优先级
		BYTE Rev2      : 3;   // 保留
	} ID_Bit;
	DWORD ID;
} TCanBusID;
typedef struct TCanBusIDMap_t
{
	BYTE ID;
	BYTE USE_FLAG;
	BYTE CHARGING_NO;
} TCanBusIDMap;

typedef struct TCanBusPara_t
{
	BYTE bSelfNode;                                    // 自身ID
	BYTE bRxNum;                                       // 有效个数
	BYTE bRxNode[MAX_CANBUS_ID_NUM];                   // 接收ID
	TCanBusIDMap tCanBusIDMap[MAX_CHARGING_NUM + 1];   // 16枪加充电桩
	BYTE OnlyListen;
	DWORD dwCrc;
} TCanBusPara;

typedef struct TCanBusInfo_t
{
	BYTE bDelay;                     // 总线空闲延时
	BYTE bSelfID;                    // 自身ID
	BYTE bRxID[MAX_CANBUS_ID_NUM];   // 接收ID
	BYTE bTxFail;                    // 发送失败标志
	BYTE TryNum;                     // 发送充实次数

} TCanBusInfo;

typedef struct ChargingData_t
{
	BYTE PF11H[4];
	BYTE PF13H[4];
	BYTE PF15H[8];
	BYTE PF20H[36];
	BYTE PF21H[8];
	BYTE PF22H[8];
	BYTE PF23H[8];
	BYTE PF40H[8];
	BYTE PF41H[8];
	BYTE PF51H[8];
	BYTE PF52H[8];
	BYTE PF61H[37];
	BYTE Safe[20];
} TChargingData;

#pragma pack()
extern TChargingData ChargingData[MAX_CHARGING_NUM];
extern TCanBusPara CanBusPara;
extern BYTE ChargingSaveEventBuf[50];
extern const BYTE ListenType;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
// 函数功能: 上电初始化信息结构体
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void api_PowerUpCanBus(void);

//-----------------------------------------------
// 函数功能: 判断是否为有效报文(链路层)
//
// 参数: 	tInfo[in]:ID信息
//
// 返回值:  TRUE/FLASE
//
// 备注:
//-----------------------------------------------
BYTE api_JudgeCanBusPro(TCanBusID tInfo);

//-----------------------------------------------
// 函数功能: 校验CAN总线参数
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void api_CheckCanBusID(void);
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
//BOOL api_ChargingCan(stc_can_rxframe_t cantemp);
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void api_ChargingCanTask(BYTE ChargingNo);
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void FactoryInitChargingPara(void);
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL ChangeID(eIDDOtype iddotype,BYTE *para ,BYTE Size);
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
WORD Charging698(eCharging698type type, BYTE *buf, WORD OI, BYTE ClassAttribute, BYTE ChargingNo, BYTE ClassIndex);
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BYTE Charging698AddTag(BYTE ClassAttribute, WORD OI, BYTE *Inbuf, BYTE *Outbuf, BYTE ClassIndex);
//--------------------------------------------------
// 功能描述:  设置充电桩参数
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BYTE SetCharging(BYTE *pOAD, BYTE *pbuf);
//--------------------------------------------------
// 功能描述:  查找序号
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL ChargingIDToNum(BYTE *ID);
//--------------------------------------------------
// 功能描述:  添加序号
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL ADDChargingMap(BYTE ID);
//--------------------------------------------------
// 功能描述:  删除序号
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL DeleteChargingMap(BYTE ID);
//--------------------------------------------------
// 功能描述:  查找保存OI
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL ChargingReallyOI2SaveOI(WORD *OI, BYTE ID);
//--------------------------------------------------
// 功能描述:  查找真实OI
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL ChargingSaveOI2ReallyOI(WORD *OI);
//--------------------------------------------------
// 功能描述:  设置CAN总线通信ID
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void api_SetCanID(BYTE bType, BYTE bTar);
//--------------------------------------------------
// 功能描述:  获取CAN总线通信ID
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
DWORD api_GetCanID(void);
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void ChangeCanMod(void);
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_SetCanMod( BYTE para );
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE  api_GetCanMod( void );
//--------------------------------------------------
//功能描述:  清除事件时同时清除瞬时量
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_FactoryInitChargingEvent(BYTE eChargeingIndex);
#endif