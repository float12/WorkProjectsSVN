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
#define SAMPLE_CHIP_NUM                 4//采样芯片数
#define LOOP_CNT                        12//回路数
//功能、相数选择
#define SELECT_SEND_WAVE_DATA           YES //通过dma发送单相波形数据给4g的开关


#define HALF_WAVE_SIZE                  384 //ui数据域的一半大小
#define POINT_PER_CYCLE                 128//每个周期采样点数
#define SIZE_OF_UIPOINT                 6 // 电压加电流大小

//缓冲和文件大小
#define TEMP_SAVE_CYCLE_NUM             4//波形数据保存周期数
#define SINGLE_FILE_FULL_TIME           30//一个文件一直存储直到满所需时间，min
#define WAVE_RECORD_PHASE_NUM           3//相数

//帧格式选择和定义
#define SCHOOL_METER                    0//校园表格式
#define DLT698                          1//698格式

#define FRAME_FORMAT                    DLT698

#if (FRAME_FORMAT == SCHOOL_METER)
#define WAVE_FRAME_SIZE                 782 // 校园表报文帧长度
#define FRAME_HEAD_LEN                  5 // 帧头信息长度
#elif (FRAME_FORMAT == DLT698)
#define WAVE_FRAME_SIZE                 815 // 698报文帧长度
#define FRAME_HEAD_LEN                  42// 698报文帧头到实际数据域的长度
#define FRAME_TYPE_OFFSET               25 //帧类型字节在报文的偏移（起始、中间、结束）
#endif

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct
{
    //波形数据处理
    WORD RxCnt;
    BYTE RxBuf[1154];
    DWORD WrongHalfWaveCount;
    WORD HSDCTimer;
    BYTE TopoErrFlag;
    DWORD gCycleNumb;
    DWORD Sec;
    BYTE WAVE_Buffer[TEMP_SAVE_CYCLE_NUM][WAVE_RECORD_PHASE_NUM][WAVE_FRAME_SIZE]; // 用来保存dma数据
    // BYTE BufNum;                                                                                // 写到sd卡的缓冲区号,从0开始
    BYTE WriteToBufCycleCount;       // 写到sd卡的周波计数
    DWORD WriteToBufSkippedHalfWave; // 上一次转存到sd卡未完成导致本次数据抛弃的计数
    const BYTE SampleChipNo; //从0开始 
} TWaveDataDeal;

typedef enum
{
    eFirstFrame = 1,
    eMiddleFrame,
    eLastFrame,
} eWaveDataFrameType;

typedef enum
{
    eCheckSignAndSendFirstFrame,
    eCheckSignAndSendOtherFrame,
} eSendState;


typedef  enum
{
	eEEProm1 = 0,
	eEEProm2,
	eFlash,
	eRTC,
	eSample1ReadUIP,
	eSample2ReadUIP,
	eSample3ReadUIP,
	eSample4ReadUIP,
}eDeviceStatus;//各器件状态


//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TWaveDataDeal WaveDataDeal[4];
extern WORD IsChargingFlag;
extern BYTE IsRecWaveData;
extern BYTE StartChargeFlag[LOOP_CNT];
extern BYTE gAllowSendWaveData;
extern TChargePara gChargePara;//单次充电最大接收数据时间，分钟
extern DWORD DeviceStatus;//各器件状态
extern TSampleUIK UIFactorArr[SAMPLE_CHIP_NUM];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//
//-----------------------------------------------
void toggleTestPin(void);
void CheckAndSendWaveData(TWaveDataDeal *WaveDataDeal);
void api_PowerUpWaveTxRx(void);
void api_CheckWaveParaAndUIK(void);
#endif // #ifndef __WAVERECORD_H
