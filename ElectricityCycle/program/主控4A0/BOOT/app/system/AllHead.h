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
#include "DefaultConstPara_Sperate.h"
#include "Specialuser.h"
#include "cpu_API.h"
#include "relation.h"
#include "math.h"
#include "string.h"
#include "ctype.h"
#include "CommFunc_API.h"
#include "relation.h"
#include "stdlib.h"
#include <stdlib.h>
#include <string.h>
#include "savemem_API.h"
#include "memory_API.h"
#include "bsp_cpu_flash.h"
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


