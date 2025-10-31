//----------------------------------------------------------------------
//Copyright (C) 2003-2024 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	高朋辉
//创建日期	2024.7.1
//描述		拓扑识别处理主文件
//修改记录
//----------------------------------------------------------------------
#define _TOPOAPP_GLOBALS //不可移动不可删除，可能影响TopLib.a文件的结构
#include "AllHead.h"
#include "topoApp.h"
#include "Dlt698_45.h"
#if (SEL_TOPOLOGY == YES)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define PI 3.1415926
#define TOPO_CURR_R 4																					 // 电流通道采样电阻阻值
#define TOPO_CURR_CT 5000																				 // 电流通道变比 	//使用500A:400mV的罗氏线圈，使用5000:1互感器，采样电阻4Ω做等效替代
#define Topo_VsFifoLenCal(Front, Rear) ((Rear >= Front) ? (Rear - Front) : (Rear + TP_FIFO_LEN - Front)) /*"队列间隔计算"*/
#define Topo_VsFifoPtrACC(Ptr)                      \
	{                                               \
		Ptr++;                                      \
		(Ptr = (Ptr >= TP_FIFO_LEN) ? (0) : (Ptr)); \
	} /*"队列指针加法器"*/
#define Topo_VsFifoPtrSUB(Ptr)                     \
	{                                              \
		((Ptr > 0) ? (Ptr--) : (TP_FIFO_LEN - 1)); \
	} /*"队列指针减法器"*/

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//谐波计算数据
BYTE rawData[1152 * 2] = {0};
BYTE RawDataHalfWaveCnt = 0;

TopoIdentiResult_t TopoIdentiResultRAM = {0};
TopoCharactCurIdentiInfo_t TopoIdentiSignalData = {0};
sTopoWaveTypedef sTopoWave = {0};
BOOL TopoEventFlag = FALSE;									// 事件发生标志
DWORD TopoIdentiCCTimeoutCnt = TOPO_IDENTI_RESULT_TIME_OUT; //  特征电流信号超时时间（s）
DWORD TopoSignalSendIntervalCnt = TOPO_SIGNAL_SEND_INTERVAL;
volatile DWORD TopoSignalValidFlag = 0;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

TRealTimer RealTimeTmp = {.Mon = 1, .Day = 1, .Hour = 0, .Min = 0, .Sec = 0};
sTopoPara_TypeDef TopoPara;
sTopoStatus_TypeDef TopoStatus[TOPO_PHASE];
ModulatSignalInfo_t ModulatSignalInfoRAM = {0};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void TopoFreqCheck(float fFreq);
//以下函数定义在TopLib.a中,无法更改
extern uint32_t fnSliceCal(uint32_t *pAdrA, uint32_t *pAdrB, uint32_t *pAdrC, uint32_t uOffset, uint32_t Lenth);
extern void fnTopo_Drive_Init(uint8_t uFqNum, uint8_t uFqBpfNum, uint8_t uFqSinCosNum);
extern WORD GetRequest645EventData(BYTE *DI, WORD OutBufLen, BYTE *OutBuf);
extern void InitTimerA2_PWM4(WORD wPeriod, WORD wCompare);
extern void TopoPWMEnable(BOOL enNewSta);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能:三字节数据转化为四字节数据
//
//参数: buf:输入缓存
//
//返回值:转化完成的四字节数
//
//备注:
//-----------------------------------------------
static int32_t TopoTrans3bTo4b(uint8_t *buf)
{
	uint32_t tmp = 0;
	int32_t rtn = 0;

	memcpy(&tmp, buf, 3);
	rtn = (tmp & 0x800000) ? (tmp | 0xFF800000) : tmp;

	return rtn;
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
	unsigned int crc_ta[256] = {/* CRC ??? */
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
								0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};
	crc = 0x0;
	while (len-- != 0)
	{
		da = (unsigned char)(crc / 256); /* 以8位二进制数的形式暂存CRC的高8位 */
		crc <<= 8;						 /* 左移8位，相当于CRC的低8位乘以2^8 */
		crc ^= crc_ta[da ^ *ptr];		 /* 高8位和当前字节相加后再查表求CRC，再加上以前的CRC */
		ptr++;
	}
	return (crc);
}

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
	uint16_t iCrc = 0, eCrc = 0;

	iCrc = CalcTopoCrc16(buf, len);
	eCrc = crc[0];
	eCrc <<= 8;
	eCrc |= crc[1];

	if (iCrc == eCrc)
	{
		return TRUE;
	}
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
	while (len-- != 0)
	{
		if (buf[len] != 0x00)
		{
			return TRUE;
		}
	}

	return FALSE;
}
//-----------------------------------------------
//函数功能:字符串倒序
//
//参数:	pSrc:字符串
//		len:字符串长度
//
//返回值:无
//
//备注:
//-----------------------------------------------
static void TopoStrSwap(void *pSrc, uint16_t Len)
{
	uint8_t tmp;
	uint8_t *p1, *p2;

	p1 = (uint8_t *)pSrc;
	p2 = (uint8_t *)pSrc + Len - 1;
	Len >>= 1;

	for (; Len != 0; Len--)
	{
		tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}
//-----------------------------------------------
//函数功能:设置拓扑识别特征码
//
//参数:	TopoInfo1:特征码低字节
//		TopoInfo2:特征码高字节
//
//返回值:无
//
//备注:
//-----------------------------------------------
static void TopoInfoSet(uint8_t TopoInfo1, uint8_t TopoInfo2)
{
	uint8_t u8i;
	uint16_t uTopoInfo = 0;

	TopoPara.Info[0] = TopoInfo1; // 0xe9;
	TopoPara.Info[1] = TopoInfo2; // 0xaa;
	TopoPara.Info[2] = 0;
	TopoPara.Info[3] = 0;
	memcpy((uint8_t *)&uTopoInfo, (uint8_t *)&TopoPara.Info[0], 2);
	TopoPara.Info_1BitNum = 0;
	TopoPara.Info_0BitNum = 0;

	for (u8i = 0; u8i < TP_FRAME_BIT; u8i++)
	{
		if (uTopoInfo & (0x8000 >> u8i))
		{
			TopoPara.FrameSeq[u8i] = 1;
			TopoPara.Info_1BitNum++;
		}
		else
		{
			TopoPara.FrameSeq[u8i] = -1;
			TopoPara.Info_0BitNum++;
		}
	}
}
//-----------------------------------------------
//函数功能:记录识别到的特征信号
//
//参数:		pNum:识别到拓扑的相别
//			uTopoInfo:特征码
//			fAmp_m:783频点幅值
//			fAmp_p:883频点幅值
//			fAmp_noise:背景噪声
//
//返回值:无
//
//备注:
//-----------------------------------------------
static void TopoResultRecord(uint32_t pNum, uint32_t uTopoInfo, float fAmp_m, float fAmp_p, float fAmp_noise)
{
	if (TopoSignalSendIntervalCnt < 12)
	{
		// 防止误识别到自身发出的拓扑信号
		return;
	}

	//	TopoPWMEnable(FALSE);	// 调试时使用，防止打断点时一直发信号
	// 异常情况
	if (TopoIdentiResultRAM.Num >= TOPO_CC_MAX_NUM_IN_IDENTI_RESULT)
	{
		memset(&TopoIdentiResultRAM, 0x00, sizeof(TopoIdentiResultRAM));
		TopoEventFlag = FALSE;
	}
	// 相别处理
	if (pNum <= 2)
	{
		pNum++;
	}
	else
	{
		pNum = 0;
	}

	memcpy(&TopoIdentiSignalData.IdentifTime, (BYTE *)&RealTimer, sizeof(TRealTimer));
	TopoIdentiSignalData.SignalPhase = pNum;
	TopoIdentiSignalData.SignalStrength1 = 2 * fAmp_m * PI * TOPO_CURR_CT / 4194304 / TOPO_CURR_R;
	TopoIdentiSignalData.SignalStrength2 = 2 * fAmp_p * PI * TOPO_CURR_CT / 4194304 / TOPO_CURR_R;
	TopoIdentiSignalData.BackgroundNoise = 2 * fAmp_noise * PI * TOPO_CURR_CT / 4194304 / TOPO_CURR_R;
	TopoIdentiSignalData.SignatureCodeLen = ModulatSignalInfoRAM.SignatureCodeLen;
	memcpy(TopoIdentiSignalData.SignatureCode, (BYTE *)&uTopoInfo, ModulatSignalInfoRAM.SignatureCodeLen);
	TopoIdentiSignalData.ExtInfoLen = ModulatSignalInfoRAM.ExtInfoLen;
	memcpy(&TopoIdentiSignalData.ExtInfo, ModulatSignalInfoRAM.ExtInfo, ModulatSignalInfoRAM.ExtInfoLen);

	// 增加拓扑识别的幅值判断
	if (((DWORD)(TopoIdentiSignalData.SignalStrength1 * 1000) < 150) || ((DWORD)(TopoIdentiSignalData.SignalStrength2 * 1000) < 150))
	{
		return;
	}

	if (TopoIdentiResultRAM.Num == 0)
	{
		TopoIdentiCCTimeoutCnt = 0;
	}
	// 在秒任务中存储拓扑信号
	TopoSignalValidFlag = 0x55AA55AA;

	memcpy(&TopoIdentiResultRAM.IdentiInfo[TopoIdentiResultRAM.Num], &TopoIdentiSignalData, sizeof(TopoCharactCurIdentiInfo_t));
	TopoIdentiResultRAM.Num++;
	if (TopoIdentiResultRAM.Num == TOPO_CC_MAX_NUM_IN_IDENTI_RESULT)
	{
		TopoEventFlag = FALSE;
	}
	else
	{
		TopoEventFlag = TRUE;
	}
}

//-----------------------------------------------
//函数功能:特征电流识别功能函数
//
//参数:		pAdrA:A相电流波形数据地址指针
//			pAdrB:B相电流波形数据地址指针
//			pAdrC:C相电流波形数据地址指针
//			uOffset:三相电流数据地址指针每次增加的偏移
//			Lenth:采样点数，默认128点
//
//返回值:bit0~bit7， bit8~bit15， bit16~bit23 分别 A\B\C 通道的检测结果。以 A 通道为例，
// =1， 检测到有效的 9.6s 高频信号， 检测到的数据内容见 TOPO_FRAME[pNum]变量。=2，等待检测结果中
//
//备注:
//-----------------------------------------------
static uint32_t TopoFrameCheck(uint32_t *pAdrA, uint32_t *pAdrB, uint32_t *pAdrC, uint32_t uOffset, uint16_t Lenth)
{
	uint32_t pNum;
	uint8_t uBitRvr;
	uint32_t uTmp;
	uint32_t i;
	uint32_t now_vs_rear; // 当前时刻对应的vs\vscor 的cnt
	float fTmp;
	int32_t sTmp;
	uint32_t sliceRet;
	uint32_t uRet = 0;
	uint32_t uDatTmpErr;
	uint16_t uTopoInfo = 0;
	uint8_t DetResult, MaxFeatSigDetPhs, recordflag, ErrHighLowFlag;
	uint16_t vsfifolen; /*"当前队列长度"*/
	uint16_t ptr, oldptr;
	int32_t VsAve, VsAve1, MaxFeatSigDet;		/*""*/
	int32_t FeatSig783DetOut, FeatSig883DetOut; /*"特征电流有效值"*/

	sliceRet = fnSliceCal(pAdrA, pAdrB, pAdrC, uOffset, Lenth);

	DetResult = 0;

	for (pNum = 0; pNum < TOPO_PHASE; pNum++)
	{
		uBitRvr = pNum << 3;
		uTmp = (sliceRet >> uBitRvr) & 0xff;
		ErrHighLowFlag = 0;
		if ((uTmp == TOPO_Slc_Refsh) && (TopoStatus[pNum].Vs_Fifo_Wait == 0))
		{ /*"200ms一轮"*/
			// cal vs
			/*"todo 加入去尖峰算法"*/
			fTmp = *(float *)&Tp_vs[pNum];
			sTmp = (int32_t)fTmp;
			// vs fifo
			now_vs_rear = TopoStatus[pNum].Vs_Fifo_Rear;  /*"队尾数据点"*/
			TopoStatus[pNum].Vs_Fifo[now_vs_rear] = sTmp; /*"有效值压入队列"*/

			fTmp = *(float *)&Tp_vs783[pNum]; /*lqs add, 240711*/
			sTmp = (int32_t)fTmp;
			TopoStatus[pNum].Vs_Fifo_783[now_vs_rear] = sTmp; /*""783HZ有效值压入队列"*/
			fTmp = *(float *)&Tp_vs883[pNum];
			sTmp = (int32_t)fTmp;
			TopoStatus[pNum].Vs_Fifo_883[now_vs_rear] = sTmp; /*""883HZ有效值压入队列"*/

			Topo_VsFifoPtrACC(TopoStatus[pNum].Vs_Fifo_Rear); /*"队尾指针累加"*/

			if (TopoStatus[pNum].FifoFullFlag == FIFO_Full)
			{
				Topo_VsFifoPtrACC(TopoStatus[pNum].Vs_Fifo_Front); /*"队头指针累加"*/
			}
			else
			{
				vsfifolen = Topo_VsFifoLenCal(TopoStatus[pNum].Vs_Fifo_Front, now_vs_rear);
				if (vsfifolen >= (TP_FIFO_LEN - 1)) /*判断是否填充满特征电流检测窗口*/
				{
					TopoStatus[pNum].FifoFullFlag = FIFO_Full;
				}
			}
			if (TopoStatus[pNum].FifoFullFlag == FIFO_Full)
			{
				TopoStatus[pNum].NoiseDet_vsSum = 0;
				TopoStatus[pNum].VsSum = 0;
				TopoStatus[pNum].Vs783Sum = 0; /*lqs add ,240711*/
				TopoStatus[pNum].Vs883Sum = 0;

				ptr = ((TopoStatus[pNum].Vs_Fifo_Front + TP_NOISEDETLEN + (TP_OS - 1)) % TP_FIFO_LEN);
				for (i = 0; i < TP_FRAME_BIT; i++)
				{
					if (TopoPara.FrameSeq[i] == 1)
					{
						TopoStatus[pNum].VsSum += TopoStatus[pNum].Vs_Fifo[ptr];
						if ((Tp_FRAME_RmsBit1 == i) || (Tp_FRAME_RmsBit2 == i) || (Tp_FRAME_RmsBit3 == i)) /*lqs add,240711*/
						{
							TopoStatus[pNum].Vs783Sum += TopoStatus[pNum].Vs_Fifo_783[ptr];
							TopoStatus[pNum].Vs883Sum += TopoStatus[pNum].Vs_Fifo_883[ptr];
						}
					}
					else
					{
						TopoStatus[pNum].NoiseDet_vsSum += TopoStatus[pNum].Vs_Fifo[ptr];
					}
					ptr += TP_OS;
					ptr = (ptr >= TP_FIFO_LEN) ? (ptr - TP_FIFO_LEN) : (ptr); //((ptr+TP_OS)%TP_FIFO_LEN);
				}

				/*"计算特征电流有效值"*/
				TopoStatus[pNum].FeatSigDetOut = (int32_t)(TopoStatus[pNum].VsSum / TopoPara.Info_1BitNum);
				/*"计算噪底"*/
				TopoStatus[pNum].NoiseDet_vs = (TopoStatus[pNum].NoiseDet_vsSum / TopoPara.Info_0BitNum);
				VsAve = 0;
				VsAve1 = 0;
				ptr = (TopoStatus[pNum].Vs_Fifo_Front + TP_NOISEDETLEN);
				if (ptr >= TP_FIFO_LEN)
				{
					ptr -= TP_FIFO_LEN;
				}

				for (i = 0; i < (TP_FRAME_LEN); i++)
				{
					if (i < (TP_FRAME_LEN / 2))
					{
						VsAve += TopoStatus[pNum].Vs_Fifo[ptr];
					}
					else
					{
						VsAve1 += TopoStatus[pNum].Vs_Fifo[ptr];
					}

					Topo_VsFifoPtrACC(ptr);
				}
				TopoStatus[pNum].VsDemuThre = VsAve / (TP_FRAME_LEN / 2);
				TopoStatus[pNum].VsDemuThre1 = VsAve1 / (TP_FRAME_LEN / 2);

				ptr = ((TopoStatus[pNum].Vs_Fifo_Front + TP_NOISEDETLEN + (TP_OS - 1)) % TP_FIFO_LEN);
				TopoStatus[pNum].FrameDet = 0;
				for (i = 0; i < TP_FRAME_BIT; i++)
				{
					TopoStatus[pNum].FrameDet <<= 1;
					if (i < (TP_FRAME_BIT / 2))
					{
						if (TopoStatus[pNum].Vs_Fifo[ptr] > (TopoStatus[pNum].VsDemuThre)) //+(TopoPara.FrameSeq[i]*(TopoPara.ThreSchmidt))
						{
							TopoStatus[pNum].FrameDet |= 1;
						}
					}
					else
					{
						if (TopoStatus[pNum].Vs_Fifo[ptr] > TopoStatus[pNum].VsDemuThre1)
						{
							TopoStatus[pNum].FrameDet |= 1;
						}
					}

					if ((i > 0) && (i < (TP_FRAME_BIT))) /*"/2判断引导符高低电平的落差"*/
					{
						if ((TopoPara.FrameSeq[i - 1] > TopoPara.FrameSeq[i]) && (TopoStatus[pNum].Vs_Fifo[oldptr] < (TopoStatus[pNum].Vs_Fifo[ptr] + TopoPara.ThreHighLow)))
						{
							ErrHighLowFlag++;
						}
					}

					oldptr = ptr;
					ptr += TP_OS;
					ptr = (ptr >= TP_FIFO_LEN) ? (ptr - TP_FIFO_LEN) : (ptr);
				}
				memcpy((uint8_t *)&uTopoInfo, (uint8_t *)&TopoPara.Info[0], 2);
				TopoStatus[pNum].ErrBitNum = 0;
				if ((TopoStatus[pNum].FrameDet == uTopoInfo) && (ErrHighLowFlag < 2))
				{
					DetResult |= (1 << pNum);
				}
				else if (ErrHighLowFlag < 2)
				{
					uDatTmpErr = (TopoStatus[pNum].FrameDet ^ uTopoInfo);
					while (uDatTmpErr)
					{
						if (uDatTmpErr & 0x01) /*"与目标值不一致"*/
						{
							TopoStatus[pNum].ErrBitNum++;
						}
						uDatTmpErr = uDatTmpErr >> 1;
					}
					if (TopoStatus[pNum].ErrBitNum <= TopoPara.ErrBitThre)
					{
						DetResult |= (1 << pNum);
					}
				}
			}
		}
		else if (TopoStatus[pNum].Vs_Fifo_Wait != 0)
		{
			TopoStatus[pNum].Vs_Fifo_Wait--;
		}
	}

	MaxFeatSigDet = TopoStatus[0].FeatSigDetOut;
	MaxFeatSigDetPhs = 0;
	if (MaxFeatSigDet < TopoStatus[1].FeatSigDetOut)
	{
		MaxFeatSigDet = TopoStatus[1].FeatSigDetOut;
		MaxFeatSigDetPhs = 1;
	}

	if (MaxFeatSigDet < TopoStatus[2].FeatSigDetOut)
	{
		MaxFeatSigDet = TopoStatus[2].FeatSigDetOut;
		MaxFeatSigDetPhs = 2;
	}
	recordflag = 0;
	for (pNum = 0; pNum < TOPO_PHASE; pNum++)
	{
		if (DetResult & (1 << pNum))
		{
			if (pNum == MaxFeatSigDetPhs) /*"识别相为有效值最大相"*/
			{
				recordflag = 0xaa;
			}
			else if ((recordflag != 0xaa) && ((DetResult & (1 << MaxFeatSigDetPhs)) == 0))
			{
				ptr = ((TopoStatus[MaxFeatSigDetPhs].Vs_Fifo_Front + TP_NOISEDETLEN + (TP_OS - 1)) % TP_FIFO_LEN);
				TopoStatus[MaxFeatSigDetPhs].FrameDet = 0;
				for (i = 0; i < TP_FRAME_BIT; i++)
				{
					TopoStatus[MaxFeatSigDetPhs].FrameDet <<= 1;

					if (i < (TP_FRAME_BIT / 2))
					{
						if (TopoStatus[MaxFeatSigDetPhs].Vs_Fifo[ptr] > (TopoStatus[pNum].VsDemuThre)) //+(TopoPara.FrameSeq[i]*(TopoPara.ThreSchmidt))
						{
							TopoStatus[MaxFeatSigDetPhs].FrameDet |= 1;
						}
					}
					else
					{
						if (TopoStatus[MaxFeatSigDetPhs].Vs_Fifo[ptr] > TopoStatus[pNum].VsDemuThre1)
						{
							TopoStatus[MaxFeatSigDetPhs].FrameDet |= 1;
						}
					}

					ptr += TP_OS;
					ptr = (ptr >= TP_FIFO_LEN) ? (ptr - TP_FIFO_LEN) : (ptr);
				}
				TopoStatus[MaxFeatSigDetPhs].ErrBitNum = 0;
				if (TopoStatus[MaxFeatSigDetPhs].FrameDet == uTopoInfo)
				{
					recordflag = 0xaa;
				}
				else
				{
					uDatTmpErr = (TopoStatus[MaxFeatSigDetPhs].FrameDet ^ uTopoInfo);
					while (uDatTmpErr)
					{
						if (uDatTmpErr & 0x01) /*"与目标值不一致"*/
						{
							TopoStatus[MaxFeatSigDetPhs].ErrBitNum++;
						}
						uDatTmpErr = uDatTmpErr >> 1;
					}
					if (TopoStatus[MaxFeatSigDetPhs].ErrBitNum <= TopoPara.ErrBitThre)
					{
						recordflag = 0xaa;
					}
				}
			}

			/*"成功识别后需要将特征电流窗口数据清除"*/
			TopoStatus[pNum].Vs_Fifo_Rear = TopoStatus[pNum].Vs_Fifo_Front + TP_NOISEDETLEN;
			TopoStatus[pNum].FifoFullFlag = FIFO_NotFull;
			/*"防止第一个200ms识别到信号，导致两个特征信号进入噪底窗口"*/
			TopoStatus[pNum].Vs_Fifo_Wait = TP_OS - 1;
			// 测试时间戳
			//			Topo_TimeStamp(4);
		}
	}

	if (recordflag == 0xaa)
	{
		uBitRvr = MaxFeatSigDetPhs << 3;
		uRet |= (0x1 << uBitRvr);
		FeatSig783DetOut = (int32_t)(TopoStatus[MaxFeatSigDetPhs].Vs783Sum / Tp_FRAME_RmsBitNum); /*lqs add, 240711*/
		FeatSig883DetOut = (int32_t)(TopoStatus[MaxFeatSigDetPhs].Vs883Sum / Tp_FRAME_RmsBitNum);
		/*"只记录信号最强相"*/
		TopoResultRecord(MaxFeatSigDetPhs, uTopoInfo,
						 (float)FeatSig783DetOut, //TopoStatus[MaxFeatSigDetPhs].FeatSigDetOut,
						 (float)FeatSig883DetOut, //TopoStatus[MaxFeatSigDetPhs].FeatSigDetOut,
						 (float)TopoStatus[MaxFeatSigDetPhs].NoiseDet_vs);

		/*"识别到数据后将所有记录清除，防止误报"*/
		for (pNum = 0; pNum < TOPO_PHASE; pNum++)
		{
			/*"成功识别后需要将特征电流窗口数据清除"*/
			TopoStatus[pNum].Vs_Fifo_Rear = TopoStatus[pNum].Vs_Fifo_Front + TP_NOISEDETLEN;
			TopoStatus[pNum].FifoFullFlag = FIFO_NotFull;
			/*"防止第一个200ms识别到信号，导致两个特征信号进入噪底窗口"*/
			TopoStatus[pNum].Vs_Fifo_Wait = TP_OS - 1;
		}
	}

	return (uRet);
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
#if (FRAME_FORMAT == SCHOOL_METER)
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
#elif (FRAME_FORMAT == DLT698)
//-----------------------------------------------
// 函数功能:填充帧头和帧尾信息
//
// 参数:Buf 数据  sec 秒  cycleNumb 周期序号  loop 回路号  frameType 帧类型
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void FillFrameHeadAndTail(BYTE *Buf, DWORD sec, DWORD cycleNumb, BYTE loop,BYTE FrameType)
{
	BYTE head[5] = {0x68, 0x2d, 0x03, 0xc3, 0x05};
	BYTE APDUhead[10] = {0x85, 0x01, 0x00, 0xee, 0xee, 0x44, 0x44, 0x01, 0x02, 0x05};
	WORD len = 0;
	WORD HeadCs = 0;
	WORD Fcs = 0;
	WORD i = 0;
	BYTE temp = 0;
	BYTE * p = NULL;

	memcpy(Buf, head, sizeof(head));

	i += sizeof(head);
	// 取表地址
	p = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);
	lib_ExchangeData(&Buf[i],p, 6);
	i += 6;
	Buf[i++] = 0x00;

	HeadCs = fcs16(&Buf[1], 11);
	memcpy(&Buf[i], &HeadCs, sizeof(HeadCs));
	i += sizeof(HeadCs);

	memcpy(&Buf[i], APDUhead, sizeof(APDUhead));
	i += sizeof(APDUhead);

	//type
	Buf[i++] = 0x0f;
	Buf[i++] = FrameType; //测试暂时用中间帧
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
// 函数功能:crc错误使用上一个半波数据
//
// 参数:phase 相数  dest 目标位置
//
// 返回值:无
//
// 备注:
//-----------------------------------------------
void UseLastHalfWave(BYTE phase, WORD dest, TWaveDataDeal *WaveDataDeal)
{
	BYTE *src = NULL, *p = NULL;
	WORD index = 0;
	BYTE tmpbuf[SIZE_OF_UIPOINT * (POINT_PER_CYCLE / 2)] = {0};
	DWORD srcOffset = 0;
	SDWORD voltage = 0, current = 0;
	SWORD i = 0;

	// 缓冲区第一个半波crc错误，使用尾部上一次数据
	if (&(WaveDataDeal->WAVE_Buffer[WaveDataDeal->WriteToBufCycleCount][phase][dest]) == &(WaveDataDeal->WAVE_Buffer[0][phase][FRAME_HEAD_LEN]))
	{
		src = &(WaveDataDeal->WAVE_Buffer[TEMP_SAVE_CYCLE_NUM - 1][phase][dest + SIZE_OF_UIPOINT * (POINT_PER_CYCLE / 2)]);
	}
	else // 使用前面的上一次数据
	{
		src = &(WaveDataDeal->WAVE_Buffer[WaveDataDeal->WriteToBufCycleCount][phase][dest - SIZE_OF_UIPOINT * (POINT_PER_CYCLE / 2)]);
	}
	// 从后向前处理每组数据
	for (i = 0; i < (POINT_PER_CYCLE / 2); i++)
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
	memcpy(&(WaveDataDeal->WAVE_Buffer[WaveDataDeal->WriteToBufCycleCount][phase][dest]),
		   &tmpbuf[0], (POINT_PER_CYCLE / 2) * SIZE_OF_UIPOINT);
}
//-----------------------------------------------
// 函数功能:保存原始波形数据并添加帧的头尾信息
//
// 参数:无
//
// 返回值:无
//
// 备注:
//-----------------------------------------------
void api_TopWaveHsdcBufTransfer(TWaveDataDeal *WaveDataDeal)
{
	BYTE UorISize = (SIZE_OF_UIPOINT / 2), phase = 0;
	BYTE WrongFlag = 0;
	WORD offset = 0, i = 0, voltageSrc = 0, currentSrc = 0, dest = 0;
	WORD halfWavePoint = (POINT_PER_CYCLE / 2);
	DWORD  Sec = 0;

	
	if (TopoCheckAllZero(&WaveDataDeal->RxBuf[0], 1154) == FALSE)
	{
		WaveDataDeal->ALlZeroCount++;
		ClearSPIError(WaveDataConf[WaveDataDeal->SampleChipNo].DmaConf.SPIInstance);
		return;
	}
	// 半波数据CRC校验 、判断是否为全0，有问题则置错误标志，并填充对应缓冲区数据为全0
	if (TopoCheckCrc(&WaveDataDeal->RxBuf[0], 1152, &WaveDataDeal->RxBuf[1152]) == FALSE)
	{
		WaveDataDeal->WrongHalfWaveCount++;
		ClearSPIError(WaveDataConf[WaveDataDeal->SampleChipNo].DmaConf.SPIInstance);
		// 用于区分正常的拷贝操作，防止错误时下面再拷贝一次接收到的波形数据
		WrongFlag = 1;
	}
	else
	{
		WaveDataDeal->NormalCnt++;
		WrongFlag = 0;
		WaveDataDeal->HSDCTimer = 0; // 错误计时器清零
	}
    
    
	RealTimeTmp.wYear = RealTimer.wYear;
	WaveDataDeal->Sec = api_CalcInTimeRelativeSec(&RealTimer) - api_CalcInTimeRelativeSec(&RealTimeTmp);

	WaveDataDeal->RxCnt++;
	// 计算偏移
	offset = (WaveDataDeal->RxCnt - 1) * HALF_WAVE_SIZE;
	if (WaveDataDeal->RxCnt == 2)
	{
		WaveDataDeal->gCycleNumb++;
	}
	// 半波ui数据正确，写入缓冲区
	for (phase = 0; phase < WAVE_RECORD_PHASE_NUM; phase++)
	{
		// 每相的电压和电流起始位置
		voltageSrc = phase * halfWavePoint * UorISize;
		currentSrc = 3 * halfWavePoint * UorISize + phase * halfWavePoint * UorISize; // 当前计量芯片方案
		// 目标写入位置
		dest = offset + FRAME_HEAD_LEN;
		// 必须有错误标志区分，crc错误后如果拷贝上个半波数据，这里会重复赋值
		if (WrongFlag == 0)
		{
			for (i = 0; i < halfWavePoint; i++)
			{
				// 拷贝电压数据
				memcpy(&WaveDataDeal->WAVE_Buffer[WaveDataDeal->WriteToBufCycleCount][phase][dest + i * SIZE_OF_UIPOINT],
					   &(WaveDataDeal->RxBuf[voltageSrc + i * UorISize]), UorISize);
				// 拷贝电流数据
				memcpy(&WaveDataDeal->WAVE_Buffer[WaveDataDeal->WriteToBufCycleCount][phase][dest + i * SIZE_OF_UIPOINT + UorISize],
					   &(WaveDataDeal->RxBuf[currentSrc + i * UorISize]), UorISize);
			}
		}
		else // 半波有问题使用前一个半波的倒序反向数据，并且保存一次错误序号到监视文件，记录最新的错误序号
		{
			UseLastHalfWave(phase, dest, WaveDataDeal);
		}
	}
	// 判断是否完成一个周期
	if (WaveDataDeal->RxCnt == 2)
	{
#if (SELECT_SEND_WAVE_DATA == YES)
		CheckAndSendWaveData(WaveDataDeal); // 中断中发送波形数据，防止主循环卡顿积攒，一次配置dma只需要4us
#endif
		WaveDataDeal->WriteToBufCycleCount++;
		// 如果完成SAVE_TO_SD_CYCLE_NUM个周期，置标志
		if (WaveDataDeal->WriteToBufCycleCount == TEMP_SAVE_CYCLE_NUM)
		{
			WaveDataDeal->WriteToBufCycleCount = 0; // 重置周波计数
		}
		WaveDataDeal->RxCnt = 0;
	}
}

//函数功能:根据频率调整WAVECNT及调整频率参数
//
//参数:频率
//
//返回值:无
//
//备注:
//-----------------------------------------------
static void TopoFreqCheck(float fFreq)
{
	uint32_t uTmp1;
	uint32_t uWave;
	float fTmp;
	uint16_t i;

	if (TopoPara.FunSwitch != TOPOFUN_ENABLE)
	{
		return;
	}

	// 频率改变小于0.1，不刷新参数
	fTmp = fFreq - FreqLast;
	if ((fTmp > -0.1f) && (fTmp < 0.1f))
	{
		return;
	}
	FreqLast = fFreq;

	if (TopAcv_Topo_Freq == Topo_Check_625hz)
	{
		// 求滤波器及正余弦区间
		for (i = 0; i < (TOPFQ_ZONE625_NUM - 1); i++)
		{
			if (fFreq <= FreqZone_625hz[i])
			{
				uTmp1 = i;
				break;
			}
			else
			{
				uTmp1 = i;
				continue;
			}
		}

		// 更新正余弦参数
		if (uTmp1 != FreqBpfNum)
		{
			FreqBpfNum = uTmp1;
			FreqSinCosNum = uTmp1;
			fnTopo_Drive_Init(TopAcv_Topo_Freq, FreqBpfNum, FreqBpfNum);
		}
	}
	else
	{
		// 求滤波器及正余弦区间
		for (i = 0; i < (TOPFQ_ZONE833_NUM - 1); i++)
		{
			if (fFreq <= FreqZone_833hz[i])
			{
				uTmp1 = i;
				break;
			}
			else
			{
				uTmp1 = i;
				continue;
			}
		}

		// 更新正余弦参数
		if (uTmp1 != FreqBpfNum)
		{
			FreqBpfNum = uTmp1;
			FreqSinCosNum = uTmp1;
			fnTopo_Drive_Init(TopAcv_Topo_Freq, FreqBpfNum, FreqBpfNum);
		}
	}
}

//-----------------------------------------------
//函数功能:初始化拓扑识别功能
//
//参数:		ModulatFreq[in]:调制频率
// 			TopoInfo1[in]:特征码低字节
//			TopoInfo2[in]:特征码高字节
//
//返回值:TRUE FALSE
//
//备注:
//-----------------------------------------------
static BOOL api_TopoInit(DWORD ModulatFreq, uint8_t TopoInfo1, uint8_t TopoInfo2)
{
	uint8_t i;

	if (ModulatFreq == 62500)
	{
		TopAcv_Topo_Freq = Topo_Check_625hz;
	}
	else
	{
		TopAcv_Topo_Freq = Topo_Check_833hz;
	}

	TopoPara.FunSwitch = TOPOFUN_ENABLE;

	for (i = 0; i < TOPO_PHASE; i++)
	{
		Topo_Result_Num[i] = 0; // 事件
		TopoStatus[i].NoiseDetFlag = Tp_NoiseOft_No;
	}

	FreqLast = 0;
	FreqBpfNum = 6;
	FreqSinCosNum = 6;
	fnTopo_Drive_Init(TopAcv_Topo_Freq, FreqBpfNum, FreqSinCosNum);

	TopoPara.SynDetThreCoef = 7; // zhupdebug 7;
	TopoPara.ErrBitThre = TP_ERRBITTHREDEF;
	TopoInfoSet(TopoInfo1, TopoInfo2);

	TopoPara.ThreSchmidt = TP_THRESCHMIDT;
	TopoPara.ThreHighLow = TP_THREHIGHLOW;
	memset((uint8_t *)TopoStatus, 0, sizeof(TopoStatus)); /*"状态清零"*/

	memset(&sTopoWave.RxCnt, 0, sizeof(sTopoWaveTypedef)); //清零拓扑数据传输缓存

	TopoFreqCheck(50.0f);

	return TRUE;
}
//-----------------------------------------------
//函数功能: 拓扑参数初始化
//
//参数:		无
//
//返回值:
//
//备注:
//-----------------------------------------------
void api_TopoParaInit(void)
{
	memcpy((BYTE *)&ModulatSignalInfoRAM, (BYTE *)&ModulatSignalInfoConst, sizeof(ModulatSignalInfo_t));
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ModulatSignalInfoSafeRom), sizeof(ModulatSignalInfo_t), (BYTE *)&ModulatSignalInfoRAM);
}

//-----------------------------------------------
//函数功能: 拓扑事件初始化
//
//参数:
//
//返回值:
//
//备注:
//-----------------------------------------------
void api_TopoInitEventPara(void)
{
	DWORD Inpara = 0;
	api_WriteEventAttribute(0x3E03, 0xff, (BYTE *)&Inpara, 0);
	api_WriteEventAttribute(0x4E07, 0xff, (BYTE *)&Inpara, 0);
	TopoIdentiResultEventDataClear();
}
//-----------------------------------------------
//函数功能: 清空拓扑识别结果事件数据
//
//参数:
//
//返回值:
//
//备注:
//-----------------------------------------------
void TopoIdentiResultEventDataClear(void)
{
	TopoEventFlag = FALSE;
	memset(&TopoIdentiResultRAM, 0x00, sizeof(TopoIdentiResultRAM));
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(TopoIdentiResultConRom), sizeof(TopoIdentiResult_t), (BYTE *)&TopoIdentiResultRAM);
	TopoIdentiCCTimeoutCnt = TOPO_IDENTI_RESULT_TIME_OUT;
}
//-----------------------------------------------
//函数功能: 拓扑检查参数
//
//参数:		无
//
//返回值:
//
//备注:
//-----------------------------------------------
void CheckTopoPara(void)
{
	// 监视当前参数
	if (lib_CheckSafeMemVerify((BYTE *)&ModulatSignalInfoRAM, sizeof(ModulatSignalInfo_t), UN_REPAIR_CRC) == FALSE)
	{
		// 由EEProm恢复参数
		if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ModulatSignalInfoSafeRom), sizeof(ModulatSignalInfo_t), (BYTE *)&ModulatSignalInfoRAM) != TRUE)
		{
			memcpy((BYTE *)&ModulatSignalInfoRAM, (BYTE *)&ModulatSignalInfoConst, sizeof(ModulatSignalInfo_t));
			api_SetError(ERR_TOPO_DATA);
		}
	}
	else
	{
		api_ClrError(ERR_TOPO_DATA);
	}
}

//-----------------------------------------------
//函数功能: 拓扑掉电处理
//
//参数:		无
//
//返回值:
//
//备注:
//-----------------------------------------------
void TopoPowerDown(void)
{
	if (TopoIdentiResultRAM.Num == 0)
	{
		memset(&TopoIdentiResultRAM, 0x00, sizeof(TopoIdentiResultRAM));
	}
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(TopoIdentiResultConRom), sizeof(TopoIdentiResult_t), (BYTE *)&TopoIdentiResultRAM);
}
//-----------------------------------------------
//函数功能: 拓扑上电处理
//
//参数:		无
//
//返回值:
//
//备注:
//-----------------------------------------------
void TopoPowerUp(void)
{
	BYTE Buf[((SUB_EVENT_INDEX_MAX_NUM + 7) / 8) + 20];
	WORD Len;
	DWORD dwAddr;

	// CheckTopoPara();
	api_TopoSetPWMInfo();
	api_TopoInit(ModulatSignalInfoRAM.ModulatFreq, ModulatSignalInfoRAM.SignatureCode[0], ModulatSignalInfoRAM.SignatureCode[1]);

	Len = GET_STRUCT_MEM_LEN(TPowerDownFlag, InstantEventFlag);
	dwAddr = GET_CONTINUE_ADDR(EventConRom.api_PowerDownFlag.InstantEventFlag[0]);
	api_ReadFromContinueEEPRom(dwAddr, Len, Buf);
	// 拓扑事件结束
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(TopoIdentiResultConRom), sizeof(TopoIdentiResult_t), (BYTE *)&TopoIdentiResultRAM);
	// 掉电前事件未开始，但有拓扑识别结果，清空
	if ((Buf[eEVENT_TOPU_NO / 8] & (0x01 << (eEVENT_TOPU_NO % 8)) == 0) && (TopoIdentiResultRAM.Num != 0))
	{
		TopoIdentiResultEventDataClear();
	}
}
//-----------------------------------------------
//函数功能: 拓扑秒任务
//
//参数:		无
//
//返回值:
//
//备注:
//-----------------------------------------------
void TopoSecondTask(void)
{
	DWORD RelativeSec1, RelativeSec2;

	if (TopoIdentiCCTimeoutCnt >= TOPO_IDENTI_RESULT_TIME_OUT)
	{
		//超时事件结束
		TopoEventFlag = FALSE;
	}
	else
	{
		TopoIdentiCCTimeoutCnt++;
	}

	if (TopoSignalValidFlag == 0x55AA55AA)
	{
		TopoSignalValidFlag = 0;

		if ((api_CheckClock(&TopoIdentiSignalData.IdentifTime) == FALSE) || (TopoIdentiSignalData.SignalPhase > 4) || (TopoIdentiSignalData.SignatureCodeLen != 2))
		{
			memset(&TopoIdentiSignalData, 0x00, sizeof(TopoIdentiSignalData));
			return;
		}
		SaveInstantEventRecord(eEVENT_TOPU_SIGNAL_NO, ePHASE_ALL, EVENT_START, eEVENT_ENDTIME_CURRENT);
		memset(&TopoIdentiSignalData, 0x00, sizeof(TopoIdentiSignalData));
	}

	if (TopoSignalSendIntervalCnt < TOPO_SIGNAL_SEND_INTERVAL)
	{
		TopoSignalSendIntervalCnt++;
	}
	else
	{
		TopoSignalSendIntervalCnt = TOPO_SIGNAL_SEND_INTERVAL;
	}

	if (api_CheckClock(&ModulatSignalInfoRAM.Time) == TRUE)
	{
		RelativeSec1 = api_CalcInTimeRelativeSec(&RealTimer);
		RelativeSec2 = api_CalcInTimeRelativeSec(&ModulatSignalInfoRAM.Time);

		if ((RelativeSec1 == RelativeSec2) || ((RelativeSec1 > RelativeSec2) && (RelativeSec1 - RelativeSec2 <= 3)) //考虑跳秒的情况
		)
		{
			memset(&ModulatSignalInfoRAM.Time, 0x00, sizeof(TRealTimer));
			api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ModulatSignalInfoSafeRom), sizeof(ModulatSignalInfo_t), (BYTE *)&ModulatSignalInfoRAM);
			api_TopoSignalSendStart();
		}
		else if (RelativeSec1 > RelativeSec2)
		{
			memset(&ModulatSignalInfoRAM.Time, 0x00, sizeof(TRealTimer));
			api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ModulatSignalInfoSafeRom), sizeof(ModulatSignalInfo_t), (BYTE *)&ModulatSignalInfoRAM);
		}
		else // (RelativeSec1 < RelativeSec2)
		{
		}
	}
}
//-----------------------------------------------
//函数功能: 读取拓扑事件发生状态
//
//参数:	Phase[in]
//
//返回值:	发生状态
//
//备注:
//-----------------------------------------------
BOOL api_GetTopuEventStatus(BYTE Phase)
{
	return TopoEventFlag;
}

void api_TopoSetPWMInfo(void)
{
	WORD wPeriod, wCompare;

	wPeriod = (WORD)(100000000.0 / ModulatSignalInfoRAM.ModulatFreq);
	wCompare = ModulatSignalInfoRAM.LowLevelPulseWidth; //低电平即高电平时间(反逻辑)

	InitTimerA2_PWM4(wPeriod, wCompare);

	api_TopoInit(ModulatSignalInfoRAM.ModulatFreq, ModulatSignalInfoRAM.SignatureCode[0], ModulatSignalInfoRAM.SignatureCode[1]);
}

BOOL api_TopoSignalSendStart(void)
{
	if ((api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE) || (api_GetSysStatus(eSYS_STATUS_TOPOING) == TRUE) || (TopoSignalSendIntervalCnt < TOPO_SIGNAL_SEND_INTERVAL))
	{
		return FALSE;
	}
	api_TopoSetPWMInfo();

	TopoSignalSendIntervalCnt = 0;
	api_SetSysStatus(eSYS_STATUS_TOPOING);

	return TRUE;
}
//-----------------------------------------------
//函数功能: 读取拓扑的识别事件
//
//参数:	InBuf[in]
//		OutBuf[out]
//返回值:	输出数据长度
//
//备注:
//-----------------------------------------------
WORD api_GetTopoIdentEvent698(TopoIdentiResult_t *InBuf, BYTE *OutBuf)
{
	BYTE i;
	WORD wLen = 0;
	DWORD TmpData;

	if ((InBuf == NULL) || (OutBuf == NULL) || (InBuf->Num > TOPO_CC_MAX_NUM_IN_IDENTI_RESULT))
	{
		return 0;
	}

	for (i = 0; i < InBuf->Num; i++)
	{
		if ((InBuf->IdentiInfo[i].SignatureCodeLen == 0) || (InBuf->IdentiInfo[i].SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN))
		{
			return 0;
		}
		if (InBuf->IdentiInfo[i].ExtInfoLen > TOPO_EXT_INFO_LEN)
		{
			return 0;
		}
	}

	OutBuf[wLen++] = Array_698;
	OutBuf[wLen++] = InBuf->Num;

	for (i = 0; i < InBuf->Num; i++)
	{
		OutBuf[wLen++] = Structure_698;
		OutBuf[wLen++] = 0x07;
		//识别的时间
		OutBuf[wLen++] = DateTime_S_698;
		memcpy(OutBuf + wLen, &InBuf->IdentiInfo[i].IdentifTime, sizeof(TRealTimer));
		lib_InverseData(OutBuf + wLen, 2); //年
		wLen += sizeof(TRealTimer);
		// 信号相位
		OutBuf[wLen++] = Enum_698;
		OutBuf[wLen++] = InBuf->IdentiInfo[i].SignalPhase;
		//信号强度S1
		OutBuf[wLen++] = Double_long_698;
		TmpData = (DWORD)(InBuf->IdentiInfo[i].SignalStrength1 * 1000);
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&TmpData, sizeof(TmpData));
		wLen += sizeof(TmpData);
		//信号强度S2
		OutBuf[wLen++] = Double_long_698;
		TmpData = (DWORD)(InBuf->IdentiInfo[i].SignalStrength2 * 1000);
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&TmpData, sizeof(TmpData));
		wLen += sizeof(TmpData);
		//背景噪声Sn
		OutBuf[wLen++] = Double_long_698;
		TmpData = (DWORD)(InBuf->IdentiInfo[i].BackgroundNoise * 1000);
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&TmpData, sizeof(TmpData));
		wLen += sizeof(TmpData);
		//特征码
		OutBuf[wLen++] = Bit_string_698;
		if (InBuf->IdentiInfo[i].SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN)
		{
			InBuf->IdentiInfo[i].SignatureCodeLen = 0;
		}
		else if (InBuf->IdentiInfo[i].SignatureCodeLen >= 8)
		{
			OutBuf[wLen++] = 0x81;
		}
		OutBuf[wLen++] = 8 * InBuf->IdentiInfo[i].SignatureCodeLen;
		if (InBuf->IdentiInfo[i].SignatureCodeLen != 0)
		{
			lib_ExchangeBit(OutBuf + wLen, InBuf->IdentiInfo[i].SignatureCode, InBuf->IdentiInfo[i].SignatureCodeLen);
			lib_InverseData(OutBuf + wLen, ModulatSignalInfoRAM.SignatureCodeLen);
			wLen += InBuf->IdentiInfo[i].SignatureCodeLen;
		}
		//扩展信息
		OutBuf[wLen++] = Octet_string_698;
		if (InBuf->IdentiInfo[i].ExtInfoLen > TOPO_EXT_INFO_LEN)
		{
			InBuf->IdentiInfo[i].ExtInfoLen = 0;
		}
		OutBuf[wLen++] = InBuf->IdentiInfo[i].ExtInfoLen;
		if (InBuf->IdentiInfo[i].ExtInfoLen != 0)
		{
			lib_ExchangeData(OutBuf + wLen, InBuf->IdentiInfo[i].ExtInfo, InBuf->IdentiInfo[i].ExtInfoLen);
			wLen += InBuf->IdentiInfo[i].ExtInfoLen;
		}
	}

	return wLen;
}

//-----------------------------------------------
//函数功能: 将拓扑识别特征信号输出为698格式
//
//参数:	pTopoIdentiResult[in]
//		OutBuf[out]
//		MaxSignalCnt[in]:允许输出的最大信号数量
//
//返回值: 长度
//
//备注:
//-----------------------------------------------
static WORD FillTopoIdentiSignal698(TopoCharactCurIdentiInfo_t *pTopoCharactCurIdentiInfo, BYTE *OutBuf)
{
	WORD wLen = 0;
	DWORD TmpData;

	if ((pTopoCharactCurIdentiInfo == NULL) || (OutBuf == NULL))
	{
		return 0;
	}

	OutBuf[wLen++] = Structure_698;
	OutBuf[wLen++] = 0x07;
	//识别的时间
	OutBuf[wLen++] = DateTime_S_698;
	memcpy(OutBuf + wLen, &pTopoCharactCurIdentiInfo->IdentifTime, sizeof(TRealTimer));
	lib_InverseData(OutBuf + wLen, 2); //年
	wLen += sizeof(TRealTimer);
	// 信号相位
	OutBuf[wLen++] = Enum_698;
	OutBuf[wLen++] = pTopoCharactCurIdentiInfo->SignalPhase;
	//信号强度S1
	OutBuf[wLen++] = Double_long_698;
	TmpData = (DWORD)(pTopoCharactCurIdentiInfo->SignalStrength1 * 1000);
	lib_ExchangeData(OutBuf + wLen, (BYTE *)&TmpData, sizeof(TmpData));
	wLen += sizeof(TmpData);
	//信号强度S2
	OutBuf[wLen++] = Double_long_698;
	TmpData = (DWORD)(pTopoCharactCurIdentiInfo->SignalStrength2 * 1000);
	lib_ExchangeData(OutBuf + wLen, (BYTE *)&TmpData, sizeof(TmpData));
	wLen += sizeof(TmpData);
	//背景噪声Sn
	OutBuf[wLen++] = Double_long_698;
	TmpData = (DWORD)(pTopoCharactCurIdentiInfo->BackgroundNoise * 1000);
	lib_ExchangeData(OutBuf + wLen, (BYTE *)&TmpData, sizeof(TmpData));
	wLen += sizeof(TmpData);
	//特征码
	OutBuf[wLen++] = Bit_string_698;
	if (pTopoCharactCurIdentiInfo->SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN)
	{
		pTopoCharactCurIdentiInfo->SignatureCodeLen = 0;
	}
	else if (pTopoCharactCurIdentiInfo->SignatureCodeLen >= 8)
	{
		OutBuf[wLen++] = 0x81;
	}
	OutBuf[wLen++] = 8 * pTopoCharactCurIdentiInfo->SignatureCodeLen;
	if (pTopoCharactCurIdentiInfo->SignatureCodeLen != 0)
	{
		lib_ExchangeBit(OutBuf + wLen, pTopoCharactCurIdentiInfo->SignatureCode, pTopoCharactCurIdentiInfo->SignatureCodeLen);
		lib_InverseData(OutBuf + wLen, ModulatSignalInfoRAM.SignatureCodeLen);
		wLen += pTopoCharactCurIdentiInfo->SignatureCodeLen;
	}
	//扩展信息
	OutBuf[wLen++] = Octet_string_698;
	if (pTopoCharactCurIdentiInfo->ExtInfoLen > TOPO_EXT_INFO_LEN)
	{
		pTopoCharactCurIdentiInfo->ExtInfoLen = 0;
	}
	OutBuf[wLen++] = pTopoCharactCurIdentiInfo->ExtInfoLen;
	if (pTopoCharactCurIdentiInfo->ExtInfoLen != 0)
	{
		lib_ExchangeData(OutBuf + wLen, pTopoCharactCurIdentiInfo->ExtInfo, pTopoCharactCurIdentiInfo->ExtInfoLen);
		wLen += pTopoCharactCurIdentiInfo->ExtInfoLen;
	}

	return wLen;
}
//-----------------------------------------------
//函数功能: 读取拓扑的识别结果
//
//参数:		ClassIndex[in]
//			OutBufLen[in]
//			OutBuf[out]
//返回值:	输出数据长度
//
//备注:
//-----------------------------------------------
WORD api_GetTopoIdentResult698(BYTE ClassIndex, WORD OutBufLen, BYTE *OutBuf)
{
	TDLT698RecordPara DLT698RecordPara = {0};
	TEventAttInfo EventAttInfo = {0};
	TEventDataInfo EventDataInfo = {0};
	TEventAddrLen EventAddrLen = {0};
	BYTE ReadBuf[100], TmpOutBuf[100];
	WORD wLen = 0, wResult, SignalCnt;
	DWORD TopoSignalOad = 0X0002084E; //TOPO_SIGNAL_OAD
	TopoCharactCurIdentiInfo_t *pTopoCharactCurIdentiInfo = NULL;

	if ((OutBuf == NULL) || (OutBufLen == 0))
	{
		return 0;
	}
	pTopoCharactCurIdentiInfo = (TopoCharactCurIdentiInfo_t *)&ReadBuf[0];

	DLT698RecordPara.OI = 0x4E07;						 //OI数据
	DLT698RecordPara.Ch = 0;							 //不清上报 固定通道传输
	DLT698RecordPara.pOAD = (BYTE *)&TopoSignalOad;		 //pOAD指针
	DLT698RecordPara.OADNum = sizeof(TopoSignalOad) / 4; //OAD个数
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;		 //选择次数
	DLT698RecordPara.TimeOrLast = 1;					 //绝对时间数
	DLT698RecordPara.Phase = ePHASE_ALL;

	// 获取事件信息
	if (GetEventIndex(DLT698RecordPara.OI, &EventAddrLen.EventIndex) == FALSE)
	{
		return 0;
	}

	EventAddrLen.Phase = DLT698RecordPara.Phase;
	if (GetEventInfo(&EventAddrLen) == FALSE)
	{
		return DLT645_ERR_ID_02;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);

	if (EventDataInfo.RecordNo > EVENT_TOPU_SIGNAL_NUM)
	{
		SignalCnt = EVENT_TOPU_SIGNAL_NUM;
	}
	else
	{
		SignalCnt = EventDataInfo.RecordNo;
	}

	if (ClassIndex == 0)
	{
		OutBuf[0] = Array_698;
		OutBuf[1] = 0;
		wLen = 2;
		if (SignalCnt == 0)
		{
			return wLen;
		}

		for (; OutBuf[1] < SignalCnt; OutBuf[1]++)
		{
			DLT698RecordPara.TimeOrLast = OutBuf[1] + 1;
			wResult = ReadEventRecordByNo(0, &DLT698RecordPara, sizeof(ReadBuf), ReadBuf);
			if ((wResult == 0) || (wResult == 0x8000))
			{
				break;
			}
			wResult = FillTopoIdentiSignal698(pTopoCharactCurIdentiInfo, TmpOutBuf);
			if (((wResult + wLen) > OutBufLen) || (wResult == 0))
			{
				break;
			}
			memcpy(&OutBuf[wLen], TmpOutBuf, wResult);
			wLen += wResult;
		}
	}
	else
	{
		if (ClassIndex > SignalCnt)
		{
			return 0;
		}
		DLT698RecordPara.TimeOrLast = ClassIndex;
		wResult = ReadEventRecordByNo(0, &DLT698RecordPara, sizeof(ReadBuf), ReadBuf);
		if ((wResult == 0) || (wResult == 0x8000))
		{
			return 0;
		}

		wResult = FillTopoIdentiSignal698(pTopoCharactCurIdentiInfo, TmpOutBuf);
		if ((wResult > OutBufLen) || (wResult == 0))
		{
			return 0;
		}
		memcpy(OutBuf, TmpOutBuf, wResult);
		wLen = wResult;
	}
	return wLen;
}
//-----------------------------------------------
//函数功能: 读取拓扑的调制信号信息
//
//参数:		ClassIndex[in]
//			OutBuf[out]
//返回值:	输出数据长度
//
//备注:
//-----------------------------------------------
WORD api_GetTopoModulatSignalInfo698(BYTE ClassIndex, BYTE *OutBuf)
{
	BYTE i;
	WORD wLen = 0;
	DWORD TmpData;

	if (OutBuf == NULL)
	{
		return 0;
	}

	switch (ClassIndex)
	{
	case 0:
		OutBuf[wLen++] = Structure_698;
		OutBuf[wLen++] = 0x06;
		// 位宽时间
		OutBuf[wLen++] = Long_unsigned_698;
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&ModulatSignalInfoRAM.BitWideTime, sizeof(ModulatSignalInfoRAM.BitWideTime));
		wLen += sizeof(ModulatSignalInfoRAM.BitWideTime);
		// 高电平脉宽
		OutBuf[wLen++] = Long_unsigned_698;
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&ModulatSignalInfoRAM.HighLevelPulseWidth, sizeof(ModulatSignalInfoRAM.HighLevelPulseWidth));
		wLen += sizeof(ModulatSignalInfoRAM.HighLevelPulseWidth);
		// 低电平脉宽
		OutBuf[wLen++] = Long_unsigned_698;
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&ModulatSignalInfoRAM.LowLevelPulseWidth, sizeof(ModulatSignalInfoRAM.LowLevelPulseWidth));
		wLen += sizeof(ModulatSignalInfoRAM.LowLevelPulseWidth);
		// 特征码
		OutBuf[wLen++] = Bit_string_698;
		if (ModulatSignalInfoRAM.SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN)
		{
			ModulatSignalInfoRAM.SignatureCodeLen = 0;
		}
		else if (ModulatSignalInfoRAM.SignatureCodeLen >= 8)
		{
			OutBuf[wLen++] = 0x81;
		}
		OutBuf[wLen++] = ModulatSignalInfoRAM.SignatureCodeLen * 8;
		lib_ExchangeBit(OutBuf + wLen, ModulatSignalInfoRAM.SignatureCode, ModulatSignalInfoRAM.SignatureCodeLen);
		lib_InverseData(OutBuf + wLen, ModulatSignalInfoRAM.SignatureCodeLen);
		wLen += ModulatSignalInfoRAM.SignatureCodeLen;
		// 调制频率
		OutBuf[wLen++] = Double_long_unsigned_698;
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&ModulatSignalInfoRAM.ModulatFreq, sizeof(ModulatSignalInfoRAM.ModulatFreq));
		wLen += sizeof(ModulatSignalInfoRAM.ModulatFreq);
		// 扩展信息
		OutBuf[wLen++] = Octet_string_698;
		if (ModulatSignalInfoRAM.ExtInfoLen > TOPO_EXT_INFO_LEN)
		{
			ModulatSignalInfoRAM.ExtInfoLen = 0;
		}
		OutBuf[wLen++] = ModulatSignalInfoRAM.ExtInfoLen;
		lib_ExchangeData(OutBuf + wLen, ModulatSignalInfoRAM.ExtInfo, ModulatSignalInfoRAM.ExtInfoLen);
		wLen += ModulatSignalInfoRAM.ExtInfoLen;
		break;
	case 1: // 位宽时间
		OutBuf[wLen++] = Long_unsigned_698;
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&ModulatSignalInfoRAM.BitWideTime, sizeof(ModulatSignalInfoRAM.BitWideTime));
		wLen += sizeof(ModulatSignalInfoRAM.BitWideTime);
		break;
	case 2: //高电平脉宽
		OutBuf[wLen++] = Long_unsigned_698;
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&ModulatSignalInfoRAM.HighLevelPulseWidth, sizeof(ModulatSignalInfoRAM.HighLevelPulseWidth));
		wLen += sizeof(ModulatSignalInfoRAM.HighLevelPulseWidth);
		break;
	case 3: //低电平脉宽
		OutBuf[wLen++] = Long_unsigned_698;
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&ModulatSignalInfoRAM.LowLevelPulseWidth, sizeof(ModulatSignalInfoRAM.LowLevelPulseWidth));
		wLen += sizeof(ModulatSignalInfoRAM.LowLevelPulseWidth);
		break;
	case 4: //特征码
		OutBuf[wLen++] = Bit_string_698;
		if (ModulatSignalInfoRAM.SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN)
		{
			ModulatSignalInfoRAM.SignatureCodeLen = 0;
		}
		else if (ModulatSignalInfoRAM.SignatureCodeLen >= 8)
		{
			OutBuf[wLen++] = 0x81;
		}
		OutBuf[wLen++] = ModulatSignalInfoRAM.SignatureCodeLen * 8;
		lib_ExchangeBit(OutBuf + wLen, ModulatSignalInfoRAM.SignatureCode, ModulatSignalInfoRAM.SignatureCodeLen);
		lib_InverseData(OutBuf + wLen, ModulatSignalInfoRAM.SignatureCodeLen);
		wLen += ModulatSignalInfoRAM.SignatureCodeLen;
		break;
	case 5: //调制频率
		OutBuf[wLen++] = Double_long_unsigned_698;
		lib_ExchangeData(OutBuf + wLen, (BYTE *)&ModulatSignalInfoRAM.ModulatFreq, sizeof(ModulatSignalInfoRAM.ModulatFreq));
		wLen += sizeof(ModulatSignalInfoRAM.ModulatFreq);
		break;
	case 6: //扩展信息
		OutBuf[wLen++] = Octet_string_698;
		if (ModulatSignalInfoRAM.ExtInfoLen > TOPO_EXT_INFO_LEN)
		{
			ModulatSignalInfoRAM.ExtInfoLen = 0;
		}
		OutBuf[wLen++] = ModulatSignalInfoRAM.ExtInfoLen;
		lib_ExchangeData(OutBuf + wLen, ModulatSignalInfoRAM.ExtInfo, ModulatSignalInfoRAM.ExtInfoLen);
		wLen += ModulatSignalInfoRAM.ExtInfoLen;
		break;
	default:
		wLen = 0;
		break;
	}

	return wLen;
}
//-----------------------------------------------
//函数功能: 设置拓扑的调制信号信息
//
//参数:		ClassIndex[in]
//			pData[in]
//			InfoLen[out]:定时发送使用
//返回值:	设置结果
//
//备注:
//-----------------------------------------------
BYTE api_SetTopoModulatSignalInfo698(BYTE ClassIndex, BYTE *pData, BYTE *InfoLen)
{
	BYTE i, Index = 0;
	WORD wLen = 0;
	ModulatSignalInfo_t TmpModulatSignalInfo = {0};
	BYTE ExtInfoLen;
	BYTE Dar = DAR_Undefined;
	BOOL ParaIsEqual = TRUE;

	if (pData == NULL)
	{
		return DAR_TempFail;
	}

	switch (ClassIndex)
	{
	case 0:
		if ((pData[0] != Structure_698) || (pData[1] != 6) || (pData[2] != Long_unsigned_698) || (pData[5] != Long_unsigned_698) || (pData[8] != Long_unsigned_698) || (pData[11] != Bit_string_698))
		{
			return DAR_WrongType;
		}

		// 位宽时间
		lib_ExchangeData((BYTE *)&TmpModulatSignalInfo.BitWideTime, &pData[3], sizeof(TmpModulatSignalInfo.BitWideTime));
		if (TmpModulatSignalInfo.BitWideTime != 600)
		{
			return DAR_OverRegion;
		}
		// 高电平脉宽
		lib_ExchangeData((BYTE *)&TmpModulatSignalInfo.HighLevelPulseWidth, &pData[6], sizeof(TmpModulatSignalInfo.HighLevelPulseWidth));
		if (TmpModulatSignalInfo.HighLevelPulseWidth != 400)
		{
			return DAR_OverRegion;
		}
		// 低电平脉宽
		lib_ExchangeData((BYTE *)&TmpModulatSignalInfo.LowLevelPulseWidth, &pData[9], sizeof(TmpModulatSignalInfo.LowLevelPulseWidth));
		if (TmpModulatSignalInfo.LowLevelPulseWidth != 800)
		{
			return DAR_OverRegion;
		}
		// 特征码长度
		if (pData[12] > 0x81)
		{
			return DAR_OverRegion;
		}
		else if (pData[12] == 0x81)
		{
			TmpModulatSignalInfo.SignatureCodeLen = pData[13];
			Index = 14;
		}
		else
		{
			TmpModulatSignalInfo.SignatureCodeLen = pData[12];
			Index = 13;
		}

		if ((TmpModulatSignalInfo.SignatureCodeLen % 8) != 0)
		{
			return DAR_OverRegion;
		}

		TmpModulatSignalInfo.SignatureCodeLen /= 8;
		if (TmpModulatSignalInfo.SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN)
		{
			return DAR_OverRegion;
		}
		// 目前特征码只允许设置两个字节
		if (TmpModulatSignalInfo.SignatureCodeLen != 2)
		{
			return DAR_OverRegion;
		}
		// 特征码
		lib_ExchangeBit(TmpModulatSignalInfo.SignatureCode, &pData[Index], TmpModulatSignalInfo.SignatureCodeLen);
		lib_InverseData(TmpModulatSignalInfo.SignatureCode, TmpModulatSignalInfo.SignatureCodeLen);
		Index += TmpModulatSignalInfo.SignatureCodeLen;

		// 特征码固定
		if ((TmpModulatSignalInfo.SignatureCode[0] != 0xE9) || (TmpModulatSignalInfo.SignatureCode[1] != 0xAA))
		{
			return DAR_OverRegion;
		}

		if ((TmpModulatSignalInfo.SignatureCodeLen != ModulatSignalInfoRAM.SignatureCodeLen) || (memcmp(ModulatSignalInfoRAM.SignatureCode, TmpModulatSignalInfo.SignatureCode, TmpModulatSignalInfo.SignatureCodeLen) != 0))
		{
			ParaIsEqual = FALSE;
		}

		// 调制频率
		if ((pData[Index++] != Double_long_unsigned_698))
		{
			return DAR_WrongType;
		}

		lib_ExchangeData((BYTE *)&TmpModulatSignalInfo.ModulatFreq, &pData[Index], sizeof(TmpModulatSignalInfo.ModulatFreq));
		Index += sizeof(TmpModulatSignalInfo.ModulatFreq);

		if ((TmpModulatSignalInfo.ModulatFreq / 100) != 833)
		{
			return DAR_OverRegion;
		}

		if (TmpModulatSignalInfo.ModulatFreq != ModulatSignalInfoRAM.ModulatFreq)
		{
			ParaIsEqual = FALSE;
		}

		// 扩展信息长度
		if ((pData[Index++] != Octet_string_698))
		{
			return DAR_WrongType;
		}
		TmpModulatSignalInfo.ExtInfoLen = pData[Index++];
		if (TmpModulatSignalInfo.ExtInfoLen > TOPO_EXT_INFO_LEN)
		{
			return DAR_OverRegion;
		}
		// 扩展信息
		lib_ExchangeData(TmpModulatSignalInfo.ExtInfo, &pData[Index], TmpModulatSignalInfo.ExtInfoLen);
		Index += TmpModulatSignalInfo.ExtInfoLen;
		// 不设置参数，仅检查参数的正确性
		if (InfoLen != NULL)
		{
			*InfoLen = Index;
			return DAR_Success;
		}

		memcpy(&ModulatSignalInfoRAM, &TmpModulatSignalInfo, sizeof(ModulatSignalInfo_t));
		Dar = DAR_Success;
		break;
	case 1: //位宽时间
		if (pData[0] != Long_unsigned_698)
		{
			return DAR_WrongType;
		}
		lib_ExchangeData((BYTE *)&TmpModulatSignalInfo.BitWideTime, &pData[1], sizeof(TmpModulatSignalInfo.BitWideTime));
		if (TmpModulatSignalInfo.BitWideTime != 600)
		{
			return DAR_OverRegion;
		}
		memcpy((BYTE *)&ModulatSignalInfoRAM.BitWideTime, &TmpModulatSignalInfo.BitWideTime, sizeof(ModulatSignalInfoRAM.BitWideTime));
		Dar = DAR_Success;
		break;
	case 2: //高电平脉宽
		if (pData[0] != Long_unsigned_698)
		{
			return DAR_WrongType;
		}
		lib_ExchangeData((BYTE *)&TmpModulatSignalInfo.HighLevelPulseWidth, &pData[1], sizeof(TmpModulatSignalInfo.HighLevelPulseWidth));
		if (TmpModulatSignalInfo.HighLevelPulseWidth != 400)
		{
			return DAR_OverRegion;
		}
		memcpy((BYTE *)&ModulatSignalInfoRAM.HighLevelPulseWidth, &TmpModulatSignalInfo.HighLevelPulseWidth, sizeof(ModulatSignalInfoRAM.HighLevelPulseWidth));
		Dar = DAR_Success;
		break;
	case 3: //低电平脉宽
		if (pData[0] != Long_unsigned_698)
		{
			return DAR_WrongType;
		}
		lib_ExchangeData((BYTE *)&TmpModulatSignalInfo.LowLevelPulseWidth, &pData[1], sizeof(TmpModulatSignalInfo.LowLevelPulseWidth));
		if (TmpModulatSignalInfo.LowLevelPulseWidth != 800)
		{
			return DAR_OverRegion;
		}
		memcpy((BYTE *)&ModulatSignalInfoRAM.LowLevelPulseWidth, &TmpModulatSignalInfo.LowLevelPulseWidth, sizeof(ModulatSignalInfoRAM.LowLevelPulseWidth));
		Dar = DAR_Success;
		break;
	case 4: //特征码
		if (pData[Index++] != Bit_string_698)
		{
			return DAR_WrongType;
		}

		// 特征码长度
		if (pData[Index] > 0x81)
		{
			return DAR_OverRegion;
		}
		else if (pData[Index] == 0x81)
		{
			Index++;
		}

		TmpModulatSignalInfo.SignatureCodeLen = pData[Index++];
		if ((TmpModulatSignalInfo.SignatureCodeLen % 8) != 0)
		{
			return DAR_OverRegion;
		}

		TmpModulatSignalInfo.SignatureCodeLen /= 8;
		if (TmpModulatSignalInfo.SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN)
		{
			return DAR_OverRegion;
		}
		// 目前特征码只允许设置两个字节
		if (TmpModulatSignalInfo.SignatureCodeLen != 2)
		{
			return DAR_OverRegion;
		}

		// 特征码
		lib_ExchangeBit(TmpModulatSignalInfo.SignatureCode, &pData[Index], TmpModulatSignalInfo.SignatureCodeLen);
		lib_InverseData(TmpModulatSignalInfo.SignatureCode, TmpModulatSignalInfo.SignatureCodeLen);

		// 特征码固定
		if ((TmpModulatSignalInfo.SignatureCode[0] != 0xE9) || (TmpModulatSignalInfo.SignatureCode[1] != 0xAA))
		{
			return DAR_OverRegion;
		}

		if ((TmpModulatSignalInfo.SignatureCodeLen != ModulatSignalInfoRAM.SignatureCodeLen) || (memcmp(ModulatSignalInfoRAM.SignatureCode, TmpModulatSignalInfo.SignatureCode, TmpModulatSignalInfo.SignatureCodeLen) != 0))
		{
			ParaIsEqual = FALSE;
		}

		ModulatSignalInfoRAM.SignatureCodeLen = TmpModulatSignalInfo.SignatureCodeLen;
		memcpy(ModulatSignalInfoRAM.SignatureCode, TmpModulatSignalInfo.SignatureCode, ModulatSignalInfoRAM.SignatureCodeLen);
		Dar = DAR_Success;
		break;
	case 5: //调制频率
		if (pData[0] != Double_long_unsigned_698)
		{
			return DAR_WrongType;
		}
		lib_ExchangeData((BYTE *)&TmpModulatSignalInfo.ModulatFreq, &pData[1], sizeof(TmpModulatSignalInfo.ModulatFreq));
		if ((TmpModulatSignalInfo.ModulatFreq / 100) != 833)
		{
			return DAR_OverRegion;
		}
		memcpy((BYTE *)&ModulatSignalInfoRAM.ModulatFreq, &TmpModulatSignalInfo.ModulatFreq, sizeof(ModulatSignalInfoRAM.ModulatFreq));
		if (TmpModulatSignalInfo.ModulatFreq != ModulatSignalInfoRAM.ModulatFreq)
		{
			ParaIsEqual = FALSE;
		}
		Dar = DAR_Success;
		break;
	case 6: //扩展信息
		// 扩展信息长度
		if ((pData[Index++] != Octet_string_698))
		{
			return DAR_WrongType;
		}
		ExtInfoLen = pData[Index++];
		if (ExtInfoLen > TOPO_EXT_INFO_LEN)
		{
			return DAR_OverRegion;
		}
		ModulatSignalInfoRAM.ExtInfoLen = ExtInfoLen;
		// 扩展信息
		lib_ExchangeData(ModulatSignalInfoRAM.ExtInfo, &pData[Index], ModulatSignalInfoRAM.ExtInfoLen);
		Dar = DAR_Success;
		break;
	default:
		Dar = DAR_OverRegion;
		break;
	}

	if (Dar == DAR_Success)
	{
		if (api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ModulatSignalInfoSafeRom), sizeof(ModulatSignalInfo_t), (BYTE *)&ModulatSignalInfoRAM) == TRUE)
		{
			if (ParaIsEqual == FALSE)
			{
				api_TopoInit(ModulatSignalInfoRAM.ModulatFreq, ModulatSignalInfoRAM.SignatureCode[0], ModulatSignalInfoRAM.SignatureCode[1]);
			}
			return DAR_Success;
		}
		else
		{
			return DAR_HardWare;
		}
	}
	else
	{
		return Dar;
	}
}
//-----------------------------------------------
//函数功能: 拓扑设置参数
//
//参数:		ClassAttribute[in]
//			ClassIndex[in]
//			pData[in]
//
//返回值: 设置结果
//
//备注:
//-----------------------------------------------
BYTE api_SetTopoPara698(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData)
{
	BYTE Dar;

	if (pData == NULL)
	{
		return DAR_TempFail;
	}

	switch (ClassAttribute)
	{
	case 4:
		Dar = api_SetTopoModulatSignalInfo698(ClassIndex, pData, NULL);
		break;
	default:
		Dar = DAR_Undefined;
		break;
	}

	return Dar;
}
//-----------------------------------------------
//函数功能: 将拓扑识别信号输出为645格式
//
//参数:		TopoCharactCurIdentiInfo[in]
//			OutBuf[out]
//
//返回值: 长度
//
//备注:
//-----------------------------------------------
static WORD FillTopoIdentiSignal645(TopoCharactCurIdentiInfo_t *pTopoCharactCurIdentiInfo, BYTE *OutBuf)
{
	WORD wLen = 0;

	if ((pTopoCharactCurIdentiInfo == NULL) || (OutBuf == NULL))
	{
		return 0;
	}

	// 识别时间
	if (lib_IsAllAssignNum((BYTE *)&pTopoCharactCurIdentiInfo->IdentifTime, 0xff, sizeof(TRealTimer)) == TRUE)
	{
		memset(&OutBuf[wLen], 0x00, 6);
		wLen += 6;
	}
	else
	{
		OutBuf[wLen++] = pTopoCharactCurIdentiInfo->IdentifTime.Sec;
		OutBuf[wLen++] = pTopoCharactCurIdentiInfo->IdentifTime.Min;
		OutBuf[wLen++] = pTopoCharactCurIdentiInfo->IdentifTime.Hour;
		OutBuf[wLen++] = pTopoCharactCurIdentiInfo->IdentifTime.Day;
		OutBuf[wLen++] = pTopoCharactCurIdentiInfo->IdentifTime.Mon;
		if (pTopoCharactCurIdentiInfo->IdentifTime.wYear >= 2000)
		{
			OutBuf[wLen++] = pTopoCharactCurIdentiInfo->IdentifTime.wYear - 2000;
		}
		else
		{
			OutBuf[wLen++] = 0;
		}
	}

	// 所属相位
	OutBuf[wLen++] = pTopoCharactCurIdentiInfo->SignalPhase;
	// 信号强度S1
	memcpy(OutBuf + wLen, (BYTE *)&pTopoCharactCurIdentiInfo->SignalStrength1, sizeof(pTopoCharactCurIdentiInfo->SignalStrength1));
	wLen += sizeof(pTopoCharactCurIdentiInfo->SignalStrength1);
	// 信号强度S2
	memcpy(OutBuf + wLen, (BYTE *)&pTopoCharactCurIdentiInfo->SignalStrength2, sizeof(pTopoCharactCurIdentiInfo->SignalStrength2));
	wLen += sizeof(pTopoCharactCurIdentiInfo->SignalStrength2);
	// 背景噪声Sn
	memcpy(OutBuf + wLen, (BYTE *)&pTopoCharactCurIdentiInfo->BackgroundNoise, sizeof(pTopoCharactCurIdentiInfo->BackgroundNoise));
	wLen += sizeof(pTopoCharactCurIdentiInfo->BackgroundNoise);
	// 特征码 //从高位到低位传输，实际为大端
	if (pTopoCharactCurIdentiInfo->SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN)
	{
		pTopoCharactCurIdentiInfo->SignatureCodeLen = TOPO_SIGNATURE_CODE_MAX_LEN;
	}
	lib_ExchangeData(OutBuf + wLen, pTopoCharactCurIdentiInfo->SignatureCode, pTopoCharactCurIdentiInfo->SignatureCodeLen); //长度按照第一个特征信号特征码长度
	wLen += pTopoCharactCurIdentiInfo->SignatureCodeLen;
	// 扩展信息
	memcpy(OutBuf + wLen, pTopoCharactCurIdentiInfo->ExtInfo, TOPO_EXT_INFO_LEN);
	wLen += TOPO_EXT_INFO_LEN;

	return wLen;
}

//-----------------------------------------------
//函数功能: 将拓扑识别结果输出为645格式
//
//参数:	 pTopoIdentiResult[in]
//		  OutBuf[out]
//		  MaxSignalCnt[in]:允许输出的最大信号数量
//
//返回值: 长度
//
//备注:
//-----------------------------------------------
static WORD FillTopoIdentiResult645(TopoIdentiResult_t *pTopoIdentiResult, BYTE *OutBuf, BYTE MaxSignalCnt)
{
	WORD wLen = 0;

	if ((pTopoIdentiResult == NULL) || (OutBuf == NULL) || (MaxSignalCnt > TOPO_CC_MAX_NUM_IN_IDENTI_RESULT))
	{
		return 0;
	}

	if (MaxSignalCnt > pTopoIdentiResult->Num)
	{
		MaxSignalCnt = pTopoIdentiResult->Num;
	}

	for (BYTE i = 0; i < MaxSignalCnt; i++)
	{
		// 识别时间
		if (lib_IsAllAssignNum((BYTE *)&pTopoIdentiResult->IdentiInfo[i].IdentifTime, 0xff, sizeof(TRealTimer)) == TRUE)
		{
			memset(&OutBuf[wLen], 0x00, 6);
			wLen += 6;
		}
		else
		{
			OutBuf[wLen++] = pTopoIdentiResult->IdentiInfo[i].IdentifTime.Sec;
			OutBuf[wLen++] = pTopoIdentiResult->IdentiInfo[i].IdentifTime.Min;
			OutBuf[wLen++] = pTopoIdentiResult->IdentiInfo[i].IdentifTime.Hour;
			OutBuf[wLen++] = pTopoIdentiResult->IdentiInfo[i].IdentifTime.Day;
			OutBuf[wLen++] = pTopoIdentiResult->IdentiInfo[i].IdentifTime.Mon;
			if (pTopoIdentiResult->IdentiInfo[i].IdentifTime.wYear >= 2000)
			{
				OutBuf[wLen++] = pTopoIdentiResult->IdentiInfo[i].IdentifTime.wYear - 2000;
			}
			else
			{
				OutBuf[wLen++] = 0;
			}
		}

		// 所属相位
		OutBuf[wLen++] = pTopoIdentiResult->IdentiInfo[i].SignalPhase;
		// 信号强度S1
		memcpy(OutBuf + wLen, (BYTE *)&pTopoIdentiResult->IdentiInfo[i].SignalStrength1, sizeof(pTopoIdentiResult->IdentiInfo[i].SignalStrength1));
		wLen += sizeof(pTopoIdentiResult->IdentiInfo[i].SignalStrength1);
		// 信号强度S2
		memcpy(OutBuf + wLen, (BYTE *)&pTopoIdentiResult->IdentiInfo[i].SignalStrength2, sizeof(pTopoIdentiResult->IdentiInfo[i].SignalStrength2));
		wLen += sizeof(pTopoIdentiResult->IdentiInfo[i].SignalStrength2);
		// 背景噪声Sn
		memcpy(OutBuf + wLen, (BYTE *)&pTopoIdentiResult->IdentiInfo[i].BackgroundNoise, sizeof(pTopoIdentiResult->IdentiInfo[i].BackgroundNoise));
		wLen += sizeof(pTopoIdentiResult->IdentiInfo[i].BackgroundNoise);
		// 特征码 //从高位到低位传输，实际为大端
		if (pTopoIdentiResult->IdentiInfo[0].SignatureCodeLen > TOPO_SIGNATURE_CODE_MAX_LEN)
		{
			pTopoIdentiResult->IdentiInfo[0].SignatureCodeLen = TOPO_SIGNATURE_CODE_MAX_LEN;
		}
		lib_ExchangeData(OutBuf + wLen, pTopoIdentiResult->IdentiInfo[i].SignatureCode, pTopoIdentiResult->IdentiInfo[0].SignatureCodeLen); //长度按照第一个特征信号特征码长度
		wLen += pTopoIdentiResult->IdentiInfo[0].SignatureCodeLen;
		// 扩展信息
		memcpy(OutBuf + wLen, pTopoIdentiResult->IdentiInfo[i].ExtInfo, TOPO_EXT_INFO_LEN);
		wLen += TOPO_EXT_INFO_LEN;
	}

	return wLen;
}
//-----------------------------------------------
//函数功能: 指定起始时间和起始序号读取识别结果
//
//参数:     ReadPara[in]
//          OutBufLen[in]
//          OutBuf[out]
//
//返回值: 长度
//
//备注:
//-----------------------------------------------
WORD api_GetTopoIdentResult645(BYTE *ReadPara, WORD OutBufLen, BYTE *OutBuf)
{
	TDLT698RecordPara DLT698RecordPara = {0};
	TEventAttInfo EventAttInfo = {0};
	TEventDataInfo EventDataInfo = {0};
	TEventAddrLen EventAddrLen = {0};
	BYTE ReadNum, i;
	WORD ReadStartRecordNo, wResult = DLT645_ERR_ID_02, wLen;
	DWORD RealRecordNo = 0;
	TRealTimer ReadStartTime = {0};
	TopoCharactCurIdentiInfo_t *pTopoCharactCurIdentiInfo = NULL;
	WORD SignalSumCnt = 0;
	BYTE ReadBuf[4 + 6 + sizeof(TopoCharactCurIdentiInfo_t) + 30]; // 4字节记录序号 6字节发生时间
	BYTE Buf[sizeof(TopoEventSignalTable)];
	DWORD AbsTime698;

	pTopoCharactCurIdentiInfo = (TopoCharactCurIdentiInfo_t *)&ReadBuf[10];

	// 解析读取使用的参数
	ReadNum = ReadPara[0];
	if (ReadNum == 0)
	{
		return DLT645_ERR_ID_02;
	}
	if (ReadNum > 4)
	{
		ReadNum = 4;
	}
	memcpy(&ReadStartRecordNo, &ReadPara[1], 2);

	ReadStartTime.Sec = ReadPara[3];
	ReadStartTime.Min = ReadPara[4];
	ReadStartTime.Hour = ReadPara[5];
	ReadStartTime.Day = ReadPara[6];
	ReadStartTime.Mon = ReadPara[7];
	ReadStartTime.wYear = ReadPara[8];
	ReadStartTime.wYear += 2000;
	if (api_CheckClock(&ReadStartTime) == FALSE)
	{
		return DLT645_ERR_ID_02;
	}
	AbsTime698 = api_CalcInTimeRelativeSec(&ReadStartTime); //计算起始时间
	if (AbsTime698 == ILLEGAL_VALUE_8F)
	{
		return DLT645_ERR_ID_02;
	}

	// 读取开始时间后的第一条记录
	memcpy(Buf, TopoEventSignalTable, sizeof(TopoEventSignalTable));

	DLT698RecordPara.OI = 0x4E07;								//OI数据
	DLT698RecordPara.Ch = 0;									//不清上报 固定通道传输
	DLT698RecordPara.pOAD = Buf;								//pOAD指针
	DLT698RecordPara.OADNum = sizeof(TopoEventSignalTable) / 4; //OAD个数
	DLT698RecordPara.eTimeOrLastFlag = eEVENT_START_TIME_FLAG;  //选择次数
	DLT698RecordPara.TimeOrLast = AbsTime698;					//绝对时间数
	DLT698RecordPara.Phase = ePHASE_ALL;
	wResult = ReadEvent645RecordByTime(&DLT698RecordPara, sizeof(ReadBuf), ReadBuf);
	if (wResult == 0x8000)
	{
		return DLT645_ERR_ID_02;
	}
	// 读取开始时间后的第一条记录的实际序号 索引0开始
	memcpy(&RealRecordNo, &ReadBuf[0], sizeof(RealRecordNo));
	// 获取事件信息
	if (GetEventIndex(DLT698RecordPara.OI, &EventAddrLen.EventIndex) == FALSE)
	{
		return DLT645_ERR_ID_02;
	}

	EventAddrLen.Phase = DLT698RecordPara.Phase;
	if (GetEventInfo(&EventAddrLen) == FALSE)
	{
		return DLT645_ERR_ID_02;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);

	if ((RealRecordNo + ReadStartRecordNo) >= EventDataInfo.RecordNo)
	{
		return DLT645_ERR_ID_02;
	}

	wLen = 0;
	// 信号总数量
	SignalSumCnt = EventDataInfo.RecordNo - RealRecordNo;
	memcpy(&OutBuf[wLen], &SignalSumCnt, sizeof(SignalSumCnt));
	wLen += sizeof(SignalSumCnt);
	// 本次回复的识别信号个数,跳过，后面填充
	wLen++;
	// 起始序号
	memcpy(&OutBuf[wLen], &ReadStartRecordNo, sizeof(ReadStartRecordNo));
	wLen += sizeof(ReadStartRecordNo);
	// 事件开始时间
	memcpy(&OutBuf[wLen], &ReadPara[3], 6);
	wLen += 6;
	// 特征码长度   按照一个特征信号特征码长度
	OutBuf[wLen++] = pTopoCharactCurIdentiInfo->SignatureCodeLen;
	// 识别结果历史记录
	for (i = 0; i < ReadNum; i++)
	{
		DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;
		DLT698RecordPara.TimeOrLast = EventDataInfo.RecordNo - (RealRecordNo + ReadStartRecordNo + i);
		wResult = api_ReadEvent645RecordByNo(&DLT698RecordPara, sizeof(ReadBuf), ReadBuf);
		if (wResult == 0x8000)
		{
			if (i == 0)
			{
				return DLT645_ERR_ID_02;
			}
			else
			{
				break;
			}
		}

		wResult = FillTopoIdentiSignal645(pTopoCharactCurIdentiInfo, &OutBuf[wLen]);
		if (wResult == 0)
		{
			return DLT645_ERR_ID_02;
		}
		wLen += wResult;
	}
	// 本次回复的识别信号个数
	OutBuf[2] = i;

	return wLen;
}
//-----------------------------------------------
//函数功能: 读取拓扑识别结果
//
//参数:	 PortType[in]
//		  DI[in]
//		  OutBufLen[in]
//		  OutBuf[out]
//
//返回值: 长度
//
//备注:
//-----------------------------------------------
WORD api_GetRequest645Topology(eSERIAL_TYPE PortType, BYTE *DI, WORD OutBufLen, BYTE *OutBuf)
{
	WORD wResult = DLT645_ERR_ID_02;
	TopoIdentiResult_t *pTopoIdentiResult;
	DWORD wLen = 0;

	if ((DI == NULL) || (OutBuf == NULL) || (OutBufLen == 0))
	{
		return DLT645_ERR_ID_02;
	}

	if (DI[2] == 0x08)
	{
		if ((DI[1] == 0x00) && (DI[0] == 0x18)) // 识别结果
		{
			wResult = api_GetTopoIdentResult645(&gPr645[PortType].pDataAddr[0], OutBufLen, OutBuf);
		}
		else if (DI[1] == 0x05) // 识别结果事件
		{
			BYTE ReadBuf[4 + 6 + sizeof(TopoIdentiResult_t) + 30]; //4字节记录序号 6字节发生时间
			wResult = GetRequest645EventData(DI, sizeof(ReadBuf), ReadBuf);

			if (DI[0] == 0) // 识别结果事件发生次数
			{
				memcpy(OutBuf, ReadBuf, 3);
				return wResult;
			}

			if (wResult > OutBufLen)
			{
				return DLT645_ERR_ID_02;
			}

			pTopoIdentiResult = (TopoIdentiResult_t *)&ReadBuf[10];

			if ((wResult & 0x8000) == 0)
			{
				wResult = 0;
				// 信号数量
				if (pTopoIdentiResult->Num > TOPO_CC_MAX_NUM_IN_IDENTI_RESULT)
				{
					return DLT645_ERR_ID_02;
				}
				OutBuf[wResult++] = pTopoIdentiResult->Num;
				// 特征码长度   按照第一个特征信号特征码长度
				OutBuf[wResult++] = pTopoIdentiResult->IdentiInfo[0].SignatureCodeLen;

				wLen = FillTopoIdentiResult645(pTopoIdentiResult, &OutBuf[wResult], pTopoIdentiResult->Num);

				if (wLen == 0)
				{
					return DLT645_ERR_ID_02;
				}
				wResult += wLen;
			}
		}
	}

	return wResult;
}
#endif //#if( SEL_TOPOLOGY == YES )
