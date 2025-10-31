//----------------------------------------------------------------------
//Copyright (C) 2003-2016 
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#ifndef __WAVERECORD_H
#define __WAVERECORD_H
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//板类型选择
#define SINGLE_PHASE_BOARD      0
#define THREE_PHASE_BOARD       1
#define HARDWARE_TYPE           SINGLE_PHASE_BOARD 

//功能、相数选择
#define SELECT_SEND_WAVE_DATA   0 //通过dma发送单相波形数据给4g的开关
#define SELECT_HARMONY_VI_CAL   0
#define SELECT_4851_SEND_DATA   0//通过485-1主动发送数据，关闭接收功能
#define WAVE_RECORD_PHASE_NUM 	1
//帧格式选择和定义
#define SCHOOL_METER            0
#define DLT698                  1
#define FRAME_FORMAT            SCHOOL_METER

#if  (FRAME_FORMAT == SCHOOL_METER)
#define WAVE_FRAME_SIZE 		782
#define FRAME_HEAD_LEN 		    5 // 帧头信息长度
#elif (FRAME_FORMAT == DLT698)
#define WAVE_FRAME_SIZE 		815
#define FRAME_HEAD_LEN 		    42 // 698报文帧头到实际数据域的长度
#endif
#define HALF_WAVE_SIZE 			384 //ui数据域的一半大小
#define POINT_PER_CYCLE 		128
#define SIZE_OF_UIPOINT 		6 // 电压加电流大小

//缓冲和文件大小
#define SAVE_TO_SD_CYCLE_NUM 	12
#define BUFF_COUNT 				6 // 缓冲区个数
#define SINGLE_FILE_FULL_TIME   30 //一个文件一直存储直到满所需时间，min
#define MAX_FILE_SIZE 			WAVE_FRAME_SIZE*WAVE_RECORD_PHASE_NUM*50*60*SINGLE_FILE_FULL_TIME // 文件最大大小
//按键
#if (HARDWARE_TYPE == SINGLE_PHASE_BOARD)
#define PRESSED_LEVEL           1 //按键按下电平状态
#elif (HARDWARE_TYPE == THREE_PHASE_BOARD)
#define PRESSED_LEVEL           0 //按键按下电平状态
#endif
#define KEY_SMOOTH_TIME			10 //防止：按键未完全按下（此时检测到的电平已经是按下电平）又松开被判为按下。时间 10*10ms。 

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	PRESSED_STATUS,
	UNPRESSED_STATUS,
	INVALAID_STATUS,
} KeyStatus;

#pragma pack(1)
typedef struct
{
	WORD 	RxCnt;
	WORD 	CycDot;
	BYTE		RxBuf[1154];
}sTopoWaveTypedef;
#pragma pack()
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE WAVE_Buffer[BUFF_COUNT][SAVE_TO_SD_CYCLE_NUM][WAVE_RECORD_PHASE_NUM][WAVE_FRAME_SIZE]; 
extern BYTE SWAVE_BufferFullFlag[BUFF_COUNT]; 
extern BYTE CreateNewFileFlag; 
extern BYTE TFKeySmoothTimer;
extern BYTE isPressed;
extern BYTE ReverAddr[6];
extern sTopoWaveTypedef sTopoWave;

extern DWORD OneBufFullCnt;
extern DWORD TwoBufFullCnt;
extern DWORD ThreeBufFullCnt;
extern DWORD CycleNumb; // 全局周期计数
extern DWORD WriteToBufSkippedHalfWave;
extern DWORD WrongHalfWaveCount;
extern DWORD WrongHalfWaveNo;


//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//
//-----------------------------------------------
void toggleTestPin(void);
void api_PowerDownWaveRecord(void);
void api_WAVE_Task(void);
BYTE WaveRecordInit(void);
#endif //#ifndef __WAVERECORD_H
