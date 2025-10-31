//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.31
//����		����CPU��ͷ�ļ�
//�޸ļ�¼	
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
//				�궨��
//-----------------------------------------------
//���������ʶ����
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
//				�ṹ�壬�����壬ö��
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
	TUpdateProgMessage UpdateProgMessage[PROGRAM_FLASH_SECTOR/128];//�޶�1024�ֽ�

}TUpdateProgSafeRom;


//-----------------------------------------------
//				��������
//-----------------------------------------------
extern WORD ProgFlashPW;

//-----------------------------------------------
// 				��������
//-----------------------------------------------

#endif//#ifndef __CPU_API_H

