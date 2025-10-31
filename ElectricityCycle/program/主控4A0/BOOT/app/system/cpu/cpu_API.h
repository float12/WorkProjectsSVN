//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.31
//描述		所有CPU的头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __CPU_API_H
#define __CPU_API_H


#if ( CPU_TYPE == CPU_HT6025 )
  #include ".\HT6025\cpuHT6025.h"
#endif

#if ( CPU_TYPE == CPU_HC32F4A0 )
  #include ".\HC32F4A0\cpuHC32F4A0.h"
#endif

#if ( CPU_TYPE == CPU_ST091 )
  #include ".\STM32F091\cpustm32f091.h"
#endif

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//保护代码标识定义
#define INIT_SYS_END					1
#define SYS_WATCH_TASK					2
#define PROTOCOL_TASK					3
#define LCD_RUN_TASK					4
#define PRAYPAY_TASK					5
#define MAIN_TIMER_TASK					6
#define SAMPLE_TASK						7
#define INIT_BASE_TIMER					8
#define MAIN_TIMER_SEC_TASK_END			9
#define LOWPOWER_TASK				    10
#define PROTOCOL698_TASK1				11
#define PROTOCOL698_TASK2				12
#define PROTOCOL698_TASK3				13
#define PROTOCOL645_TASK				14
#define RTC_RUN_TASK					15
#define FREEZE_RUN_TASK					16
#define EVENT_RUN_TASK				    17
#define RELAY_TASK				        18
#define PARA_TASK1				        19
#define PARA_TASK2				        20
#define ENERGY_TASK1				    21
#define ENERGY_TASK2				    22








//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct TUpdateProgHead_t
{
	DWORD ProgAddr;
	BYTE ProgMessageNum;
	
	DWORD CRC32;

}TUpdateProgHead;


typedef struct TUpdateProgMessage_t
{
	BYTE ProgData[128];
	DWORD CRC32;

}TUpdateProgMessage;


typedef struct TUpdateProgSafeMem_t
{
	TUpdateProgHead UpdateProgHead;
	TUpdateProgMessage UpdateProgMessage[PROGRAM_FLASH_SECTOR/128];//限定1024字节

}TUpdateProgSafeRom;


//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern WORD ProgFlashPW;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

#endif//#ifndef __CPU_API_H

