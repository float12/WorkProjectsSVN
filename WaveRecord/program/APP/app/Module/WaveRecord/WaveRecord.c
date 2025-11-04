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
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define MAX_FILE_NAME_LEN 12//99999-1p.bin 
#define SAMPLE_DOTS_PER_CYCLE 128
#define MAX_WAVE_FILE_NUM 99999 // 文件名不能超过8字符
#define FILE_SUFFIX ".bin"
#define SYNC_FILE_CNT 64




//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	ePOWER_DOWN,
	eWRONG_FRAME,
	eKEY_RELEASED,
} eSaveMonitorDataType;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//录波相关变量
//监视
DWORD WrongHalfWaveCount = 0;
DWORD OneBufFullCnt = 0;
DWORD TwoBufFullCnt = 0;
DWORD ThreeBufFullCnt = 0;
DWORD WriteToBufSkippedHalfWave = 0;// 上一次转存到sd卡未完成导致本次数据抛弃的计数
DWORD WrongHalfWaveNo = 0;
sTopoWaveTypedef sTopoWave = {0};
DWORD CycleNumb = 0;// 全局周期计数

//录波相关
BYTE  WriteToSdCycleCount = 0; // 写到sd卡的周波计数
BYTE  BufNum = 0;				 // 写到sd卡的缓冲区号,从0开始

BYTE ReverAddr[6] = {0};
BYTE WAVE_Buffer[BUFF_COUNT][SAVE_TO_SD_CYCLE_NUM][WAVE_RECORD_PHASE_NUM][WAVE_FRAME_SIZE];		// 用来保存dma数据
BYTE WAVE_BufferTo4G[BUFF_COUNT][SAVE_TO_SD_CYCLE_NUM][WAVE_RECORD_PHASE_NUM][WAVE_FRAME_SIZE]; // 用来保存4g需要采集的数据
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
KeyStatus keyStatus = INVALAID_STATUS;
BYTE isPressed = 0; // 按下按键为1
static BYTE LastKeyLevel = 0;
BYTE TFKeySmoothTimer = 0;

FRESULT MonitorResult = FAT32_FR_OK;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern BOOL IsLeapYear(BYTE Year);


//-----------------------------------------------
// 函数功能:监视缓冲区情况
//
// 参数:无
//
// 返回值:无
//
// 备注:
//-----------------------------------------------
void MonitorBuffer(void)
{
	BYTE bufFullCnt = 0, bufIdx = 0;

	for (bufIdx = 0; bufIdx < BUFF_COUNT; bufIdx++)
	{
		if (SWAVE_BufferFullFlag[bufIdx] == 1)
		{
			bufFullCnt++;
		}
	}
	switch (bufFullCnt)
	{
	case 1:
		OneBufFullCnt++;
		break;
	case 2:
		TwoBufFullCnt++;
		break;
	case 3:
		ThreeBufFullCnt++;
		break;
	default:
		break;
	}
}

//-----------------------------------------------
//函数功能:计算CRC16
//
//参数: ptr:输入缓存
//		len:输入缓存的长度
//                    
//返回值:计算出的CRC16
//
//备注: 
//-----------------------------------------------
static uint16_t CalcTopoCrc16(uint8_t *ptr, uint32_t len)
{
	unsigned int crc = 0x0;
	unsigned char da = 0;
	unsigned int crc_ta[256] = { /* CRC ??? */
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
		0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
		0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
		0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
		0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
		0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
		0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
		0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
		0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
		0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
		0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
		0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
		0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
		0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
		0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
		0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
		0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
		0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
		0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
		0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
		0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
		0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
		0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
		0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
		0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
	};
	crc = 0x0;
	while (len-- != 0) {
		da = (unsigned char)(crc / 256);/* 以8位二进制数的形式暂存CRC的高8位 */
		crc <<= 8; 		/* 左移8位，相当于CRC的低8位乘以2^8 */
		crc ^= crc_ta[ da ^ *ptr ]; /* 高8位和当前字节相加后再查表求CRC，再加上以前的CRC */
		ptr++;
	}
	return (crc);
}

//-----------------------------------------------
// 函数功能:crc错误使用上一个半波数据
//
// 参数:phase 相数  dest 目标位置
//
// 返回值:无
//
// 备注:
//-----------------------------------------------
void UseLastHalfWave(BYTE phase, WORD dest)
{
	BYTE *src = NULL, *p = NULL;
	WORD index = 0;
	BYTE tmpbuf[SIZE_OF_UIPOINT * (POINT_PER_CYCLE / 2)] = {0};
	DWORD srcOffset = 0; 
	SDWORD voltage = 0, current = 0;
	SWORD i = 0;

	// 缓冲区第一个半波crc错误，使用尾部上一次数据
	if (&WAVE_Buffer[BufNum][WriteToSdCycleCount][phase][dest] == &WAVE_Buffer[0][0][0][FRAME_HEAD_LEN])
	{
		src =&WAVE_Buffer[BUFF_COUNT-1][SAVE_TO_SD_CYCLE_NUM-1][phase][dest+SIZE_OF_UIPOINT *  (POINT_PER_CYCLE / 2)];
	}
	else // 使用前面的上一次数据
	{
		src = &WAVE_Buffer[BufNum][WriteToSdCycleCount][phase][dest - SIZE_OF_UIPOINT *  (POINT_PER_CYCLE / 2)];
	}
	// 从后向前处理每组数据
	for (i =  0; i < (POINT_PER_CYCLE / 2); i++)
	{
		srcOffset = i * 3 * 2; // 每对数据有电压3字节+电流3字节
		// 拼接电压的3字节
		voltage = ((SDWORD)(src[srcOffset] & 0xFF) << 16) |
				  ((SDWORD)(src[srcOffset + 1] & 0xFF) << 8) |
				  ((SDWORD)(src[srcOffset + 2] & 0xFF));
        //半波波形起始点不确定，不从0点开始，所以错误波形只能使用前面半波反向后再拼接
        voltage = -voltage;
		tmpbuf[index++] = (voltage >> 16) & 0xFF;
		tmpbuf[index++] = (voltage >> 8) & 0xFF;
		tmpbuf[index++] = voltage & 0xFF;

		// 拼接电流的3字节
		current = ((SDWORD)(src[srcOffset + (SIZE_OF_UIPOINT / 2)] & 0xFF) << 16) |
				  ((SDWORD)(src[srcOffset + (SIZE_OF_UIPOINT / 2) + 1] & 0xFF) << 8) |
				  ((SDWORD)(src[srcOffset + (SIZE_OF_UIPOINT / 2) + 2] & 0xFF));
        current = -current;
		tmpbuf[index++] = (current >> 16) & 0xFF;
		tmpbuf[index++] = (current >> 8) & 0xFF;
		tmpbuf[index++] = current & 0xFF;
	}
	memcpy(&WAVE_Buffer[BufNum][WriteToSdCycleCount][phase][dest],
		   &tmpbuf[0], (POINT_PER_CYCLE / 2) * SIZE_OF_UIPOINT);
}
//-----------------------------------------------
//函数功能:计算双字节校验和
//
//参数:ptr 数据  Length 数据长度
//
//返回值:校验和
//
//备注: 
//-----------------------------------------------
WORD CheckSum_WORD(BYTE *ptr, WORD Length)
{
	WORD i = 0;
	WORD Sum = 0;

	for (i = 0; i < Length; i++)
	{
		Sum += *ptr;
		ptr++;
	}
	return Sum;
}

#if  FRAME_FORMAT ==  SCHOOL_METER
//-----------------------------------------------
// 函数功能:填充帧头和帧尾信息
//
// 参数:Buf 数据  sec 秒  mileSec 毫秒  cycleNumb 周期序号
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void FillFrameHeadAndTail(BYTE *Buf, DWORD sec, WORD mileSec, DWORD cycleNumb)
{
	WORD i = 0;
	WORD checkSum = 0;

	Buf[i++] = 0x68;
	Buf[i++] = LLBYTE(sec);
	Buf[i++] = LHBYTE(sec);
	Buf[i++] = HLBYTE(sec);
	Buf[i++] = HHBYTE(sec);

	i = 773; // 帧尾偏移值

	// 两字节毫秒
	Buf[i++] = LLBYTE(mileSec);
	Buf[i++] = LHBYTE(mileSec);

	Buf[i++] = LLBYTE(cycleNumb);
	Buf[i++] = LHBYTE(cycleNumb);
	Buf[i++] = HLBYTE(cycleNumb);
	Buf[i++] = HHBYTE(cycleNumb);

	checkSum = CheckSum_WORD((BYTE *)Buf, WAVE_FRAME_SIZE - 3); // 不包括最后两个字节

	Buf[i++] = LLBYTE(checkSum);
	Buf[i++] = LHBYTE(checkSum);

	Buf[i] = 0x16;
}
#elif FRAME_FORMAT ==  DLT698
//-----------------------------------------------
// 函数功能:填充帧头和帧尾信息
//
// 参数:Buf 数据  sec 秒   cycleNumb 周期序号
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void FillFrameHeadAndTail(BYTE *Buf, DWORD sec, DWORD cycleNumb,BYTE loop)
{
	BYTE head[5] = {0x68,0x2d,0x03,0xc3,0x05};
	BYTE APDUhead [10] ={0x85,0x01,0x00,0xee,0xee,0x44,0x44,0x01,0x02,0x05};
	WORD len = 0;
	WORD HeadCs = 0;
	WORD Fcs = 0;
	WORD i = 0;
	BYTE temp = 0;


	memcpy(Buf, head, sizeof(head));
	i += sizeof(head);

	memcpy(&Buf[i], ReverAddr, sizeof(ReverAddr));
	i += sizeof(ReverAddr);
    Buf[i++] = 0x00;

	HeadCs = fcs16(&Buf[1], 11);
	memcpy(&Buf[i], &HeadCs, sizeof(HeadCs));
	i += sizeof(HeadCs);

	memcpy(&Buf[i], APDUhead, sizeof(APDUhead));
	i += sizeof(APDUhead);

	//type
	Buf[i++] = 0x0f;
	Buf[i++] = 0x02;//测试暂时用中间帧
	//loop
	Buf[i++] = 0x0f;
	Buf[i++] = loop;
	//sec
	Buf[i++] = 0x06;
	Buf[i++] = HHBYTE(sec);
	Buf[i++] = HLBYTE(sec);
	Buf[i++] = LHBYTE(sec);
	Buf[i++] = LLBYTE(sec);
	//No
	Buf[i++] = 0x06;
	Buf[i++] = HHBYTE(cycleNumb);
	Buf[i++] = HLBYTE(cycleNumb);
	Buf[i++] = LHBYTE(cycleNumb);
	Buf[i++] = LLBYTE(cycleNumb);
	Buf[i++] = 0x0c;
	Buf[i++] = 0x82;
	Buf[i++] = 0x03;
	Buf[i++] = 0x00;
	i += POINT_PER_CYCLE * SIZE_OF_UIPOINT;
	Buf[i++] = 0x00;
	Buf[i++] = 0x00;
	Fcs = fcs16(&Buf[1], WAVE_FRAME_SIZE - 4);
	memcpy(&Buf[i], &Fcs, sizeof(Fcs));
	i += sizeof(Fcs);
	Buf[i++] = 0x16;
}
#endif

//-----------------------------------------------
//函数功能:检查半波数据CRC校验
//
//参数:	buf:输入缓存
//		len:输入缓存的长度
//		crc:从计量芯片获得的缓存数据中的CRC
//                    
//返回值:TRUE:正确	FALSE:错误
//
//备注: 
//-----------------------------------------------
static uint8_t TopoCheckCrc(uint8_t *buf, uint16_t len, uint8_t *crc)
{
	uint16_t iCrc=0, eCrc=0;
	
	iCrc = CalcTopoCrc16(buf, len);
	eCrc = crc[0];
	eCrc <<= 8;
	eCrc |= crc[1];
	
	if(iCrc == eCrc)
	{
		return TRUE;
	}
	// printf("\r\nTopo CRC Error: cal Crc=0x%04X, rec Crc=0x%04X", iCrc, eCrc);
	return FALSE;
}
//-----------------------------------------------
//函数功能:检查半波数据是否为全0，全0则认定为异常
//
//参数:	buf:输入缓存
//		len:输入缓存的长度
//                    
//返回值:TRUE:不是全0	FALSE:是全0
//
//备注: 
//-----------------------------------------------
static uint8_t TopoCheckAllZero(uint8_t *buf, uint16_t len)
{
    while(len-- != 0)
    {
        if(buf[len] != 0x00)
        {
            return TRUE;
        }
    }

    return FALSE;
}

//-----------------------------------------------
//函数功能:解析原始波形数据
//
//参数:无
//
//返回值:无
//
//备注: 
//-----------------------------------------------
void api_ParseHsdcWaveBuf(void)
{
	BYTE bufFullCnt = 0, UorISize = (SIZE_OF_UIPOINT / 2), phase = 0, bufIdx = 0;
	BYTE skipFlag = 1, WrongFlag = 0;
	WORD offset = 0, offset2 = 0, i = 0, j = 0, voltageSrc = 0, currentSrc = 0, dest = 0, checkSum = 0;
	WORD halfWavePoint = (POINT_PER_CYCLE / 2);
	DWORD tmp2 = 0, Sec = 0;

	if (TopoCheckAllZero(&sTopoWave.RxBuf[0], 1154) == FALSE)
	{
		return;
	}
	// 半波数据CRC校验 、判断是否为全0，有问题则置错误标志，并填充对应缓冲区数据为全0
	if (TopoCheckCrc(&sTopoWave.RxBuf[0], 1152, &sTopoWave.RxBuf[1152]) == FALSE)
	{
		WrongHalfWaveCount++;
		// 用于区分正常的拷贝操作，防止错误时下面再拷贝一次接收到的波形数据
		WrongFlag = 1;
	}
	else
	{
		WrongFlag = 0;		
		HSDCTimer = 0; // 错误计时器清零
	}

	RealTimeTmp.wYear = RealTimer.wYear;
	Sec = api_CalcInTimeRelativeSec(&RealTimer) - api_CalcInTimeRelativeSec(&RealTimeTmp);
	// 判断有无空缓冲区并置跳过标志，理论上有空缓冲区
	for (bufIdx = 0; bufIdx < BUFF_COUNT; bufIdx++)
	{
		if (SWAVE_BufferFullFlag[bufIdx] == 0)
		{
			skipFlag = 0;
			break;
		}
	}
	sTopoWave.RxCnt++;
	// 计算偏移
	offset = (sTopoWave.RxCnt - 1) * HALF_WAVE_SIZE;
	if (sTopoWave.RxCnt == 2)
	{
		CycleNumb++;
	}
	if (skipFlag == 0)
	{
		#if SELECT_HARMONY_VI_CAL == 1
		// 只能每半波保存谐波计算需要的数据，不能从主循环buf拷贝，因为数据顺序不同
		// 保存一个未处理的周波原始数据
		if (RawDataHalfWaveCnt == 0)
		{
			memcpy(&rawData[0], &sTopoWave.RxBuf[0], 1152);
			RawDataHalfWaveCnt++;
		}
		else if (RawDataHalfWaveCnt == 1)
		{
			memcpy(&rawData[1152], &sTopoWave.RxBuf[0], 1152);
			RawDataHalfWaveCnt++;
		}
		#endif
		// 半波ui数据正确，写入缓冲区
		for (phase = 0; phase < WAVE_RECORD_PHASE_NUM; phase++)
		{
			// 每相的电压和电流起始位置
			voltageSrc = phase * halfWavePoint * UorISize;
			currentSrc = 3 * halfWavePoint * UorISize + phase * halfWavePoint * UorISize;//当前计量芯片方案
			// 目标写入位置
			dest = offset + FRAME_HEAD_LEN;
			// 必须有错误标志区分，crc错误后如果拷贝上个半波数据，这里会重复赋值
			if (WrongFlag == 0)
			{
				for (i = 0; i < halfWavePoint; i++)
				{
					// 拷贝电压数据
					memcpy(&WAVE_Buffer[BufNum][WriteToSdCycleCount][phase][dest + i * SIZE_OF_UIPOINT],
						   &sTopoWave.RxBuf[voltageSrc + i * UorISize], UorISize);
					// 拷贝电流数据
					memcpy(&WAVE_Buffer[BufNum][WriteToSdCycleCount][phase][dest + i * SIZE_OF_UIPOINT + UorISize],
						   &sTopoWave.RxBuf[currentSrc + i * UorISize], UorISize);
				}
			}
			else // 半波有问题使用前一个半波的倒序反向数据，并且保存一次错误序号到监视文件，记录最新的错误序号
			{
				if(sTopoWave.RxCnt == 2)
				{
					WrongHalfWaveNo = CycleNumb;
				}
				else//一个周波未收完 序号还没加所以错误序号需要+1
				{
					WrongHalfWaveNo = CycleNumb + 1;
				}
				UseLastHalfWave(phase, dest);
			}
		}
		// 判断是否完成一个周期
		if (sTopoWave.RxCnt == 2)
		{
			// 填充头尾数据
			for (j = 0; j < WAVE_RECORD_PHASE_NUM; j++)
			{
				#if FRAME_FORMAT == SCHOOL_METER
				FillFrameHeadAndTail(&WAVE_Buffer[BufNum][WriteToSdCycleCount][j][0], Sec, 0, CycleNumb);
				#elif FRAME_FORMAT == DLT698
				FillFrameHeadAndTail(&WAVE_Buffer[BufNum][WriteToSdCycleCount][j][0], Sec, CycleNumb,1);
				#endif
			}
			WriteToSdCycleCount++;
			// 如果完成SAVE_TO_SD_CYCLE_NUM个周期，置标志
			if (WriteToSdCycleCount == SAVE_TO_SD_CYCLE_NUM)
			{
				WriteToSdCycleCount = 0;		  // 重置周波计数
				#if (SELECT_SEND_WAVE_DATA == 1)
				PrepareAndSendWaveData();//中断中发送波形数据，防止主循环卡顿积攒，一次配置dma只需要4us
				#endif
				SWAVE_BufferFullFlag[BufNum] = 1; // 置标志位，在主循环里写卡
				MonitorBuffer();//置完标志再监视当前情况	
				BufNum = (BufNum + 1) % BUFF_COUNT;
			}
		}
		else
		{
		}
	}
	else
	{
		WriteToBufSkippedHalfWave++;
	}
	if (sTopoWave.RxCnt == 2)
	{
		sTopoWave.RxCnt = 0;
	}
}

//-----------------------------------------------
// 函数功能:保存监视数据
//
// 参数:eSaveMonitorDataType
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void SaveMonitorData(eSaveMonitorDataType type)
{
	BYTE buffer[1024] = {0};
	DWORD bytesWritten = 0;

	if (f_open(&monitorFile, "monitor.txt", FA_OPEN_ALWAYS | FA_WRITE) == FAT32_FR_OK)
	{
		if (f_lseek(&monitorFile, ((&monitorFile)->fsize)) == FAT32_FR_OK)
		{
			snprintf((char *)buffer, sizeof(buffer),
			 "\nTime: %04u-%02u-%02u %02u:%02u:%02u\n"
			 "currentFileName = %s\n"
			 "CycleNumb = %lu\n"
			 "BufWriteToTFSucCount = %lu\n"
			 "OneBufFullCnt = %lu\n\n"
			 "TwoBufFullCnt = %lu\n"
			 "ThreeBufFullCnt = %lu\n"
			 "WrongHalfWaveCount = %u\n"
			 "WriteToBufSkippedHalfWave = %lu\n"
			 "BufWriteToTFErrCount = %lu\n"
			 "Write to tf FR_Result = %lu\n"
			 "WrongHalfWaveNo = %lu\n",
			 RealTimer.wYear, RealTimer.Mon, RealTimer.Day, RealTimer.Hour, RealTimer.Min, RealTimer.Sec,
			 currentFileName, CycleNumb, BufWriteToTFSucCount, OneBufFullCnt, TwoBufFullCnt,
			 ThreeBufFullCnt, WrongHalfWaveCount, WriteToBufSkippedHalfWave, BufWriteToTFErrCount,
			 MonitorResult, WrongHalfWaveNo);
					
			if (type == eKEY_RELEASED)
			{
				strcat((char *)buffer, "key released,save monitor data\n\n\n");
			}
			else if (type == ePOWER_DOWN)
			{
				strcat((char *)buffer, "power down,save monitor data\n\n\n");
			}
			// 写入数据到文件
			f_write(&monitorFile, buffer, strlen((char *)buffer), (UINT *)&bytesWritten);
			f_close(&monitorFile);
		}
	}
	memset(&monitorFile, 0, sizeof(monitorFile));
}

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
        snprintf((char *)currentFileName, sizeof(currentFileName), "1-%dp%s", WAVE_RECORD_PHASE_NUM, FILE_SUFFIX);
        
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
            snprintf((char *)currentFileName, sizeof(currentFileName), "%d-%dp%s", lastNumber, WAVE_RECORD_PHASE_NUM, FILE_SUFFIX);
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
	DWORD freeSpace = MAX_FILE_SIZE - (file->fsize);
	DWORD lastNumber = 0;
	BYTE BoolResult = FALSE;
	DWORD writtenBytes = 0, readBytes = 0;
	FRESULT fr_result = FAT32_FR_OK;
	static WORD WriteCnt = 0;
	// 如果剩余空间不足以写入一次缓存数据，创建新文件并继续写入
	if ((freeSpace < WAVE_FRAME_SIZE * WAVE_RECORD_PHASE_NUM * SAVE_TO_SD_CYCLE_NUM) || (freeSpace == 0))
	{
		// 关闭当前文件
		f_close(file);
		// toggleTestPin();
		fr_result = CreateNextWaveFile();
		// toggleTestPin();
		// if (sscanf((char *)currentFileName, "%d-", &lastNumber) == 1)
		// {
		// 	// 计算下一个文件的编号
		// 	lastNumber++;
		// 	memset(currentFileName, 0, sizeof(currentFileName));
		// 	sprintf((char *)currentFileName, "%d-%dp%s", lastNumber, WAVE_RECORD_PHASE_NUM, FILE_SUFFIX);
		// }
		// // 重新打开新文件
		// fr_result = f_open(&DataFile, (char *)currentFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		if (fr_result != FAT32_FR_OK)
		{
			MonitorResult = fr_result;
			// 错误处理，无法打开新文件
			return FALSE;
		}
	}
	fr_result = f_write(file, buf, Size, (UINT *)&writtenBytes);
	if ((fr_result != FAT32_FR_OK) || (writtenBytes != Size))
	{
		MonitorResult = fr_result;
		// 错误处理，写入失败
		return FALSE;
	}
	else
	{
		WriteCnt++;
		if (WriteCnt >= SYNC_FILE_CNT)
		{
			f_sync(file);//同步文件，防止数据丢失或者耗时过大
		}
		WriteCnt = 0;
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

	p = (BYTE *)&FPara1->MeterSnPara.CommAddr;
	// 将表地址反转
	ReverAddr[0] = p[5];
	ReverAddr[1] = p[4];
	ReverAddr[2] = p[3];
	ReverAddr[3] = p[2];
	ReverAddr[4] = p[1];
	ReverAddr[5] = p[0];
#if SELECT_SEND_WAVE_DATA == 1
	UartSendDmaInit();
#endif
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
// 函数功能:	根据读取电平判断按键状态
//
// 参数:	KeyLevel 读取电平
//
// 返回值:	INVALAID_STATUS 当前按键值无效，
//
// 备注:
//-----------------------------------------------
KeyStatus JudgeKeyStatus(BYTE KeyLevel)
{
	if (KeyLevel == PRESSED_LEVEL)
	{
		return PRESSED_STATUS;
	}
	else
	{
		return UNPRESSED_STATUS;
	}
}
//-----------------------------------------------
// 函数功能:	按键处理
//
// 参数:	无
//
// 返回值:	KeyStatus 按键状态
//
// 备注:
//-----------------------------------------------
static KeyStatus DetectKey(void)
{
	BYTE CurrentKeyLevel = 0; // 电平状态
	static BYTE WaitKeyTimerToZeroFlag = 0;

	if (WaitKeyTimerToZeroFlag == 0)
	{
		CurrentKeyLevel = GET_BUTTON_STATE;
		// 按键发生变化systick减消抖时间
		if (CurrentKeyLevel != LastKeyLevel)
		{
			TFKeySmoothTimer = KEY_SMOOTH_TIME;
			WaitKeyTimerToZeroFlag = 1;
			return INVALAID_STATUS;
		}
		else // 未变化返回上一次的电平状态
		{
			return JudgeKeyStatus(LastKeyLevel);
		}
	}
	else // 等待消抖时间到0
	{
		if (TFKeySmoothTimer == 0)
		{
			WaitKeyTimerToZeroFlag = 0;
			CurrentKeyLevel = GET_BUTTON_STATE;
			// 获取到的电平仍然与上一次不一致
			if (CurrentKeyLevel != LastKeyLevel)
			{
				LastKeyLevel = CurrentKeyLevel;
				return JudgeKeyStatus(LastKeyLevel);
			}
			else // 使用之前电平
			{
				return JudgeKeyStatus(LastKeyLevel);
			}
		}
		else
		{
			return INVALAID_STATUS;
		}
	}
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
	if ((keyStatus == PRESSED_STATUS) && (isPressed == 1))
	{
		f_close(&DataFile);
		memset(&DataFile, 0, sizeof(DataFile));
		SaveMonitorData(ePOWER_DOWN);
	}
	TF_LED_LOW;
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
	DISABLE_CPU_INT;
	BufWriteToTFSucCount = 0;
	BufWriteToTFErrCount = 0;
	MonitorResult = FAT32_FR_OK;
	WrongHalfWaveCount = 0;
	OneBufFullCnt = 0;
	TwoBufFullCnt = 0;
	ThreeBufFullCnt = 0;
	WriteToBufSkippedHalfWave = 0;
	WrongHalfWaveNo = 0;
	ENABLE_CPU_INT;
}
//-----------------------------------------------
// 函数功能:按键处理
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void KeyProcess(void)
{

	if ((keyStatus == PRESSED_STATUS) && (isPressed == 0))
	{ // 每次按下后新建下一个文件
		isPressed = 1;
		memset(&FileSystem, 0, sizeof(FileSystem));
		f_mount(0, &FileSystem);
		CreateNextWaveFile();
		InitMonitorVariable();
	}
	else if ((keyStatus == UNPRESSED_STATUS) && (isPressed == 1))
	{
		// 每次按键弹起保存监视数据
		f_close(&DataFile);
		memset(&DataFile, 0, sizeof(DataFile));
		isPressed = 0;
		SaveMonitorData(eKEY_RELEASED);
		f_mount(0, NULL); // 卸载文件系统
	}
}

//-----------------------------------------------
// 函数功能: 准备并发送波形数据
//
// 参数: WriteToSDBufNum - 要处理的缓冲区编号
//       buf - 指向数据缓冲区的指针
//
// 返回值: 无
//
// 备注: 根据相数选择发送数据源并通过DMA发送
//-----------------------------------------------
void PrepareAndSendWaveData(void)
{
	BYTE CycleIndex = 0;
	BYTE *buf = NULL;

    Serial[1].WatchSciTimer = WATCH_SCI_TIMER;    
	if (WAVE_RECORD_PHASE_NUM == 1)
	{
		buf = (BYTE *)&WAVE_Buffer[WriteToSDBufNum][0][0][0];
	}
	else if (WAVE_RECORD_PHASE_NUM == 3)
	{
		// toggleTestPin();
		// 提取第一相数据到连续缓冲
		for (CycleIndex = 0; CycleIndex < SAVE_TO_SD_CYCLE_NUM; CycleIndex++)
		{
			memcpy(&FirstPhaseBuffer[CycleIndex * WAVE_FRAME_SIZE],
				   &WAVE_Buffer[WriteToSDBufNum][CycleIndex][0][0],
				   WAVE_FRAME_SIZE);
		}
		buf = FirstPhaseBuffer;
		// toggleTestPin();
	}
	UartSendWaveDataByDma(buf, SAVE_TO_SD_CYCLE_NUM * WAVE_FRAME_SIZE);
}
void TransferDataPerFlag(void)
{
	BYTE result = 0;
	BYTE i = 0;
	DWORD LastTick = 0;

	//遇到多缓冲满就一次写多次
	for (i = 0; i < BUFF_COUNT; i++)
	{   
		if (SWAVE_BufferFullFlag[WriteToSDBufNum] == 1)
		{	
			if ((DataFile.fs != NULL) && (isPressed == 1))
			{
				LastTick = gsysTick;
				result = WriteDataToFile(&DataFile, (BYTE *)&WAVE_Buffer[WriteToSDBufNum][0][0][0],
										 WAVE_FRAME_SIZE * WAVE_RECORD_PHASE_NUM * SAVE_TO_SD_CYCLE_NUM);
				#if (SEL_DEBUG_VERSION == YES)
				if ((gsysTick - LastTick) > 200)
				{
					// printf("\r\nTF Write Time:%lu ms", gsysTick - LastTick);
					api_WriteFreeEvent(EVENT_TF_WRITE_200MS, (WORD)(gsysTick - LastTick)); // 写入时间超过200ms
				}
				#endif
				if (result == TRUE)
				{
					BufWriteToTFSucCount++;
					TF_TOGGLE_LED;
				}
				else
				{
					TF_LED_LOW;
					BufWriteToTFErrCount++;
				}
			}
			else // 跳过写入
			{
				TF_LED_LOW;
				WriteToSdSkipCount++;
			}
			SWAVE_BufferFullFlag[WriteToSDBufNum] = 0;
			WriteToSDBufNum = (WriteToSDBufNum + 1) % BUFF_COUNT;
		}
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
void api_WAVE_Task(void)
{
	static DWORD LastSkipNum = 0,lastWrongHalfWaveCnt = 0;
	keyStatus = DetectKey();
	KeyProcess();
	TransferDataPerFlag();
	// 检测到跳过次数变化就写记录，传完数据后判断，防止传输中WriteToBufSkippedHalfWave变化
	if ((WriteToBufSkippedHalfWave > 0) && (LastSkipNum != WriteToBufSkippedHalfWave))
	{
		LastSkipNum = WriteToBufSkippedHalfWave;
		api_WriteFreeEvent(EVENT_TF_BUF_FULL, (WORD)WriteToBufSkippedHalfWave);
	}
	if((WrongHalfWaveCount > 0) && (lastWrongHalfWaveCnt != WrongHalfWaveCount))
	{
		lastWrongHalfWaveCnt = WrongHalfWaveCount;
		api_WriteFreeEvent(EVENT_WAVE_CRC_ERR, (WORD)WrongHalfWaveCount);
	}
}
