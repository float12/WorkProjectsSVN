//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "./GDW698/GDW698.h"
#ifndef __MCOLLECT_API_H
#define __MCOLLECT_API_H
//--------------------------------------

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define		MAX_SEND_DELAY			(150) //1.5秒
#define		MAX_TX_COMMIDLETIME		(80)  //800毫秒
#define		MAX_RX_COMMIDLETIME		(100) //1秒
#define		ONE_SCHEME_FREEAE_NUM	1

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)

typedef struct TClctTypeRom_t
{
	DWORD dwType; // 抄表类型，整点冻结，日冻结
	DWORD CRC32;
} TClctTypeRom;

typedef struct TClctMeterMem_t
{
	TClctTypeRom ClctTypeRom;
} TClctMeterRom;

// typedef struct  {
// 	DWORD 	ArFastCatTime[BLOCK_OF_REAL/SIZE_PAGE/CATSIZE];
// 	DWORD 	CRC32;
// }TFastCatTime;
#pragma pack()

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern TFPara2* const	tfpara2bak;
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
void api_MCollectTask(void);
BOOL IsJLing(BYTE Ch);
void JLWrite(BYTE destCh,BYTE *hBuf,DWORD dwLen,BYTE byGyType,BYTE sourceCh);
BOOL GyRxMonitor(BYTE Ch,BYTE GyType);
void JLWriteRx(BYTE Ch,BYTE *hBuf,DWORD dwLen);
BOOL IsUpCommuFrame(BYTE byGy, BYTE *pBuf);
BOOL IsCollectAssociateFrame(BYTE *pBuf);
void api_CommuTx(BYTE SerialNo);
void api_PowerDownSave(void);
void CollectorWriteTx(BYTE sourceCh,BYTE *hBuf,BYTE *pMeterAddr,DWORD dwLen);
void InitPara(BYTE byType);
void InitData(void);
BOOL api_analysisClearTextMAC(BYTE *pFrameBuf,BYTE *rand,TTwoByteUnion *frameLen);
#endif // __MCOLLECT_API_H
