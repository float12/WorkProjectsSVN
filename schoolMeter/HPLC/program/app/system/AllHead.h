//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		所有头文件统一包含的头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __ALLHEAD_H
#define __ALLHEAD_H

//#include <stdio.h>
#include "__def.h"
#include "sys.h"
#include "ConstConfig.h"
#include "board_API.h"
#include "Config_3Phase_Meter.h"
#include "Config_Single_Meter.h"
#include "Specialuser.h"
#include "cpu_API.h"
#include "relation.h"

#include "RTC_API.h"
#include "math.h"
#include "string.h"
#include "ctype.h"

#include "CommFunc_API.h"

#include "assert_API.h"
#include "lcd_API.h"
#include "relation.h"
#include "stdlib.h"
#include <string.h>
#include "syswatch_API.h"
#include "flag_API.h"
#include "SysDescribe.h"
#include "para_API.h"
#include "Protocol.h"
#include "SearchCollectMeter_API.h"
#include "Dlt698_45_API.h"
#include "dlt645_2007_API.h"
#include "MCollect_Api.h"
#include "demand_Api.h"
#include "demand_2022_Api.h"
#include "event_Api.h"
#include "Report645_Api.h"
#include "Report698_Api.h"
#include "sysEvent_API.h"
#include "Sample_API.h"
#include "PrePay_API.h"
#include "Energy_API.h"
#include "Relay_Local_API.h"
#include "Relay_Remote_API.h"
#include "Freeze_API.h"
#include "savemem_API.h"
#include "memory_API.h"

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
	        								
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void MainTask(void);

#endif//#ifndef __ALLHEAD_H


