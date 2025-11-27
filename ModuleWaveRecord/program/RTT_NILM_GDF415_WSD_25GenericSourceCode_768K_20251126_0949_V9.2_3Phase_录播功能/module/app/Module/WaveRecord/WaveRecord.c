//----------------------------------------------------------------------
// Copyright (C)
// 创建人
// 创建日期
// 描述		波形记录文件
// 修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "ff.h"
#include "stdio.h"
#include <string.h>
#include "waveRecord.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define PREALLOC_CHUNK   		(8*1024*1024) // 8MB
#define SAFE_MARGIN      		(1*1024*1024) // 1MB
#define MAX_FILE_NAME_LEN 		12//99999-1p.bin 
#define SAMPLE_DOTS_PER_CYCLE 	128
#define MAX_WAVE_FILE_NUM 		99999 // 文件名不能超过8字符
#define FILE_SUFFIX 			".bin"
#define SYNC_FILE_CNT 			64
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//录波相关变量
TWaveData WaveDataTmp[WAVE_QUEUE_SIZE] = {0};//波形数据临时缓冲区
//监视
DWORD WrongHalfWaveCount = 0;
DWORD OneBufFullCnt = 0;
DWORD TwoBufFullCnt = 0;
DWORD ThreeBufFullCnt = 0;
DWORD WriteToBufSkippedHalfWave = 0;// 上一次转存到sd卡未完成导致本次数据抛弃的计数
DWORD CycleNumb = 0;// 全局周期计数

//录波相关
BYTE  WriteToSdCycleCount = 0; // 写到sd卡的周波计数
BYTE  BufNum = 0;				 // 写到sd卡的缓冲区号,从0开始
BYTE WaveReocodFlag = 0; // 是否开始录制 1开始 0停止
BYTE ReverAddr[6] = {0};
// 存储五个周期的三相缓冲区
BYTE SWAVE_BufferFullFlag[BUFF_COUNT] = {0};
BYTE CreateNewFileFlag = 0; // 标记是否创建新文件，1表示创建了，需要在rtc中断里闪灯
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
TRealTimer RealTimeTmp= {.Mon=1,.Day=1,.Hour=0,.Min=0,.Sec=0};//用来计算年初到现在的秒
FATFS FileSystem;	  // 文件系统工作区
FIL DataFile; // 数据文件结构体
FIL monitorFile;
BYTE currentFileName[MAX_FILE_NAME_LEN] = {0}; // 当前文件名
DWORD BufWriteToTFSucCount = 0; // 写入TF卡成功的次数计数
DWORD BufWriteToTFErrCount = 0;
DWORD WriteToSdSkipCount = 0;
BYTE WriteToSDBufNum = 0;
BYTE FirstPhaseBuffer[SAVE_TO_SD_CYCLE_NUM * WAVE_FRAME_SIZE] = {0}; // A相用于串口dma发送的缓冲区
BYTE isPressed = 0; // 按下按键为1
static BYTE LastKeyLevel = 0;
BYTE TFKeySmoothTimer = 0;

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern BOOL IsLeapYear(BYTE Year);


//-----------------------------------------------
// 函数功能:上电或再次按下按钮创建新文件
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
FRESULT CreateNextWaveFile(void)
{
    FRESULT result = FAT32_FR_OK;
    BYTE fileName[MAX_FILE_NAME_LEN] = {0};
    DWORD lastNumber = 0;
    FIL nameFile;//保存最新文件名的文件
    UINT bytesRead = 0;
    UINT bytesWrite = 0;
	FRESULT preRes;

    // 尝试打开保存文件名的文件
    result = f_open(&nameFile, "lastfile.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    if(result == FR_NO_FILE)
    {
        // 文件不存在,创建名字文件
        snprintf((char *)currentFileName, sizeof(currentFileName), "1-%dp%s", MAX_PHASE, FILE_SUFFIX);
        
        // 创建并保存文件名
        result = f_open(&nameFile, "lastfile.txt", FA_CREATE_NEW | FA_WRITE);
        if(result == FAT32_FR_OK)
        {
            result = f_write(&nameFile, currentFileName, strlen((char *)currentFileName), &bytesWrite);
            result = f_close(&nameFile);
        }
    }
    else if(result == FAT32_FR_OK)
    {
        // 读取上一次的文件名
        f_read(&nameFile, fileName, sizeof(fileName), &bytesRead);
        // 解析文件编号并加1
        if(sscanf((char *)fileName, "%d-", &lastNumber) == 1)
        {
            lastNumber++;
            snprintf((char *)currentFileName, sizeof(currentFileName), "%d-%dp%s", lastNumber, MAX_PHASE, FILE_SUFFIX);
            // 保存新文件名
			result = f_lseek(&nameFile, 0);
            result = f_write(&nameFile, currentFileName, sizeof(currentFileName), &bytesWrite);
            result = f_close(&nameFile);
        }
    }
	memset(&DataFile, 0, sizeof(DataFile));
    // 创建波形数据文件
    result = f_open(&DataFile, (char *)currentFileName, FA_CREATE_NEW | FA_WRITE);
    if(result == FAT32_FR_OK)
    {
        CreateNewFileFlag = 1;
    }
    return result;
}

//-----------------------------------------------
// 函数功能:检查并预分配文件空间
//
// 参数: 
//
// 返回值: 
//
// 备注:
//-----------------------------------------------
void CheckAndPreAlloc(FIL *fp)
{
    QWORD pos = f_tell(fp);
    QWORD size = f_size(fp);
	QWORD new_size = 0;
	FRESULT res = FR_INT_ERR;
	BYTE b = 0;
    UINT bw = 0;
    // 如果剩余空间不够 SAFE_MARGIN，就扩一段
    if (size - pos < SAFE_MARGIN)
    {
        new_size = size + PREALLOC_CHUNK;
        res = f_lseek(fp, new_size);
        if (res == FAT32_FR_OK)
        {
            // 写一个字节确保真正分配，不留洞
            f_write(fp, &b, 1, &bw);
            f_lseek(fp, pos);
        }
    }
}
//-----------------------------------------------
// 函数功能:关闭波形文件，截去预分配未使用的空间
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void CloseWaveFile(FIL *file)
{
	QWORD actual_size = f_tell(file);
	// 回到实际长度位置
	f_lseek(file, actual_size);
	// 截断文件释放后面预分配的簇
	f_truncate(file);
	f_close(file);
}
//-----------------------------------------------
// 函数功能:写数据到文件
//
// 参数: file 文件指针 buf 数据 Size 数据大小
//
// 返回值: true 成功 ；false 失败
//
// 备注:
//-----------------------------------------------
BYTE WriteDataToFile(FIL *file, const BYTE *buf, DWORD Size)
{
	DWORD freeSpace = MAX_FILE_SIZE - (file->fptr);
	DWORD lastNumber = 0;
	BYTE BoolResult = FALSE;
	DWORD writtenBytes = 0, readBytes = 0;
	FRESULT fr_result = FAT32_FR_OK;
	static WORD WriteCnt = 0;
	// 如果剩余空间不足以写入一次缓存数据，创建新文件并继续写入
	if ((freeSpace < WAVE_FRAME_SIZE * MAX_PHASE * SAVE_TO_SD_CYCLE_NUM) || (freeSpace == 0))
	{
		// 关闭当前文件
		CloseWaveFile(file);
		// toggleTestPin();
		fr_result = CreateNextWaveFile();
		// toggleTestPin();
		// if (sscanf((char *)currentFileName, "%d-", &lastNumber) == 1)
		// {
		// 	// 计算下一个文件的编号
		// 	lastNumber++;
		// 	memset(currentFileName, 0, sizeof(currentFileName));
		// 	sprintf((char *)currentFileName, "%d-%dp%s", lastNumber, MAX_PHASE, FILE_SUFFIX);
		// }
		// // 重新打开新文件
		// fr_result = f_open(&DataFile, (char *)currentFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		if (fr_result != FAT32_FR_OK)
		{
			// 错误处理，无法打开新文件
			return FALSE;
		}
	}
	CheckAndPreAlloc(file);
	fr_result = f_write(file, buf, Size, (UINT *)&writtenBytes);
	if ((fr_result != FAT32_FR_OK) || (writtenBytes != Size))
	{
		// 错误处理，写入失败
		return FALSE;
	}
	else
	{
		WriteCnt++;
		if (WriteCnt >= SYNC_FILE_CNT)
		{
			f_sync(file);//同步文件，防止数据丢失或者耗时过大
            WriteCnt = 0;
		}
	}
	return TRUE;
}
//-----------------------------------------------
// 函数功能:初始化波形记录功能
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
BYTE WaveRecordInit(void)
{
	FRESULT result = FAT32_FR_OK;
	BYTE * p = NULL;

	memset(&FileSystem, 0, sizeof(FileSystem));
	// 尝试加载保存的文件名
	if (f_mount(0, &FileSystem) != 0) // 加载初始化FATFS文件系统（返回值不为0说明初始化失败）
	{
		return FALSE;
	}
	// 初始化sdio比较耗时，防止后续来数据了再初始化可能占满一个缓冲区
	result = f_open(&monitorFile, "monitor.txt", FA_OPEN_ALWAYS | FA_WRITE);
	result = f_close(&monitorFile);
	memset(&monitorFile, 0, sizeof(monitorFile));
	return result;
}

//-----------------------------------------------
// 函数功能:停止录波处理
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
//-----------------------------------------------
void StopRecording(void)
{
	CloseWaveFile(&DataFile);
	memset(&DataFile, 0, sizeof(DataFile));
	memset(&FileSystem, 0, sizeof(FileSystem));
	f_mount(0, NULL); // 卸载文件系统
}

//-----------------------------------------------
// 函数功能:掉电保存监视数据，防止断电后按键未弹起丢失监视数据
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void api_PowerDownWaveRecord(void)
{
	if (WaveReocodFlag == 1)
	{
		WaveReocodFlag = 0;
		StopRecording();
	}
}
//-----------------------------------------------
// 函数功能:初始化监视变量
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void InitMonitorVariable(void)
{
	BufWriteToTFSucCount = 0;
	BufWriteToTFErrCount = 0;
	WrongHalfWaveCount = 0;
	OneBufFullCnt = 0;
	TwoBufFullCnt = 0;
	ThreeBufFullCnt = 0;
	WriteToBufSkippedHalfWave = 0;
}
//-----------------------------------------------
// 函数功能:开始录波处理
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void StartRecording(void)
{
	memset(&FileSystem, 0, sizeof(FileSystem));
	memset(&DataFile, 0, sizeof(DataFile));
	f_mount(0, &FileSystem);
	CreateNextWaveFile();
	InitMonitorVariable();
}


//-----------------------------------------------
// 函数功能:检查录波开关并处理
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void CheckRecordSign(void)
{
	static BYTE LastWaveReocodFlag = 0;
	if(LastWaveReocodFlag != WaveReocodFlag)
	{
		LastWaveReocodFlag = WaveReocodFlag;
		if(WaveReocodFlag == 1)
		{
			StartRecording();
		}
		else if(WaveReocodFlag == 0)
		{
			StopRecording();
		}
	}
}

void TransferDataPerFlag(void)
{
	BYTE result = 0;
	BYTE i = 0;

	for(i = 0; i < (mq_WaveData->entry); i++)
	{
		api_QueueRecv(mq_WaveData, (BYTE *)&WaveDataTmp[i], sizeof(WaveDataTmp[0]));
	}
	//遇到多缓冲满就一次写多次
	if ((DataFile.fs != NULL) && (WaveReocodFlag == 1))
	{
		result = WriteDataToFile(&DataFile, (BYTE *)&WaveDataTmp[0].Buf[0],sizeof(WaveDataTmp[0])*(mq_WaveData->entry));
		if (result == TRUE)
		{
			BufWriteToTFSucCount++;
		}
		else
		{
			BufWriteToTFErrCount++;
		}
	}
	else // 跳过写入
	{
		WriteToSdSkipCount++;
	}
}
//-----------------------------------------------
// 函数功能:波形任务
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void RecordWaveDataTask(void* para)
{
	CheckRecordSign();
	TransferDataPerFlag();
	api_WriteFreeEvent(EVENT_TF_BUF_FULL, (WORD)WriteToBufSkippedHalfWave);
}
