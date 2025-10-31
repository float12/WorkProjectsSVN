//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		DLT698头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __DLT698_H
#define __DLT698_H
#include "wsd_uart.h"

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define	MAX_PRO_BUF						(512)	//帧长度 通讯缓存apdu层最大尺寸


//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

typedef enum
{
	edouble_long = 0,
	efloat32,
} eDataType698;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern char MessageBuf[512];
extern BYTE InstantData698Frame[INSTANT_DATA_698_FRAME_LEN];
extern eDataType698 InstantDataFormatTable[DATA_ITEM_NUM_PER_PHASE];

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void  Send_Heartbeatframe_698( tTranData *transdata,BYTE bLink );
void DoSearch_68_DLT698(TSerial *p);
WORD JudgeDlt698_45_HCS_FCS(BYTE Type, TSerial *p);
WORD DoReceProc_Dlt698_UART(BYTE ReceByte, TSerial *p);
WORD api_fcs16(BYTE *cp, WORD len);
void Compose_InstantData_698(void);
#endif//#if( READMETER_PROTOCOL ==  PROTOCOL_698)

