//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		所有头文件统一包含的头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __ALLHEAD_H
#define __ALLHEAD_H

#include "sys.h"
#include "__def.h"
#include "root.h"
#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"

#include "rtthread.h"
#include "rtdevice.h"


#include "dbase.h"

#include "CommFunc_API.h"
#include "flag_API.h"
#include "dsp_time.h"
#if(!WIN32)
#include "board_gd32f450.h"
#include "cpustm32f4xx.h"
#else
#include "board_vs.h"
//#include "cpustm32h7.h"
#endif
#include "RootTask_API.h"
#include "Protocol.h"
#include "Dlt698_45_API.h"
#include "dlt645_2007_API.h"
#include "Algorithm_API.h"
#include "Freeze_API.h"
#include "event_Api.h"
#include "sysEvent_API.h"
#include "datadeal_API.h"
#include "savemem_API.h"
#include "Exp_API.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#include "WaveRecord.h"
//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern volatile DWORD BufferLen;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

#endif//#ifndef __ALLHEAD_H


