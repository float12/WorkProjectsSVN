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
#define MAX_PRO_BUF (512) //帧长度 通讯缓存apdu层最大尺寸

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
extern char bLtoAddr[6];				//物联网表电表通信地址
extern char MessageBuf[512];
extern BYTE InstantData698Frame[INSTANT_DATA_698_FRAME_LEN];
extern eDataType698 InstantDataFormatTable[DATA_ITEM_NUM_PER_PHASE];
extern BYTE ReadModuleVersion;
extern char ModuleVersion[100];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void Compose_Heartbeatframe_698(BYTE *buf, BYTE bLink);
void DoSearch_68_DLT698(TSerial *p);
WORD JudgeDlt698_45_HCS_FCS(BYTE Type, TSerial *p);
WORD DoReceProc_Dlt698_UART(BYTE ReceByte, TSerial *p);
WORD api_fcs16(BYTE *cp, WORD len);
void Compose_InstantData_698(void);
void  Adrr_Txd_698(void);
void Compose_GetRequsetRecordFreezeMin_698();
void  Compose_SecurityRequsetRecordFreezeMin_698( void );
void  Close_SafeMode( void );
void  Compose_SendRequestfactor_698( float uk,float ik,BYTE *buf,BYTE Loop);
void ComposeReadModuleVersion(void);
void  ComposeReadModuleVersion_security( void );
BYTE  Add698Shell( BYTE *buf,WORD wLen,BYTE *Outbuf );
void  SetLtoTime( void );
void  open_ltofactory( void );
#endif //#if( READMETER_PROTOCOL ==  PROTOCOL_698)
