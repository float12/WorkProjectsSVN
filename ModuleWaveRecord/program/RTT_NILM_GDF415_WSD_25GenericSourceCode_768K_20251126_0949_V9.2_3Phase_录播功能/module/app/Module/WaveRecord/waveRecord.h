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
//功能、相数选择
#define WAVE_FRAME_SIZE 		778
#define WAVE_QUEUE_SIZE 		4
#define POINT_PER_CYCLE 		128
#define SIZE_OF_UIPOINT 		6 // 电压加电流大小
//缓冲和文件大小
#define SAVE_TO_SD_CYCLE_NUM 	12
#define BUFF_COUNT 				6 // 缓冲区个数
#define SINGLE_FILE_FULL_TIME   30 //一个文件一直存储直到满所需时间，min
#define MAX_FILE_SIZE 			WAVE_FRAME_SIZE*MAX_PHASE*50*60*SINGLE_FILE_FULL_TIME // 文件最大大小
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct
{
	BYTE Buf[WAVE_FRAME_SIZE * MAX_PHASE];
}TWaveData;//一个周波的数据
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
extern BYTE WAVE_Buffer[BUFF_COUNT][SAVE_TO_SD_CYCLE_NUM][MAX_PHASE][WAVE_FRAME_SIZE]; 
extern BYTE SWAVE_BufferFullFlag[BUFF_COUNT]; 
extern BYTE CreateNewFileFlag; 
extern BYTE TFKeySmoothTimer;
extern BYTE isPressed;
extern BYTE ReverAddr[6];
extern sTopoWaveTypedef sTopoWave;
extern BYTE WaveReocodFlag;
extern DWORD OneBufFullCnt;
extern DWORD TwoBufFullCnt;
extern DWORD ThreeBufFullCnt;
extern DWORD CycleNumb; // 全局周期计数
extern DWORD WriteToBufSkippedHalfWave;
extern DWORD WrongHalfWaveCount;
extern TWaveData WaveDataTmp[WAVE_QUEUE_SIZE];

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//
//-----------------------------------------------
void api_PowerDownWaveRecord(void);
BYTE WaveRecordInit(void);
void RecordWaveDataTask(void*);
#endif //#ifndef __WAVERECORD_H


