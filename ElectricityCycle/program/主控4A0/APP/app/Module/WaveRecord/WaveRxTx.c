//----------------------------------------------------------------------
// Copyright (C)
// 创建人
// 创建日期
// 描述		波形记录文件
// 修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "stdio.h"
#include <string.h>
#include "WaveRxTx.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define POWER_DOWN_VOL_VALUE 			10//v?
#define WAIT_UIP_REFRESH_TIME			1000//ms
#define CHARGE_FINISH_CNT				100//充电结束计数
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef  enum
{
	eCheckSign = 0,
	eWaitUIPRefresh,//等待uip刷新，否则检测到充电后uip还未刷新会判断为充电结束
	eWaitChargeEnd,
} eState;//充电标志检查状态


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern volatile DWORD gsysTick;
DWORD SendFrameCnt = 0;		//发送帧计数，用来判超时
BYTE SendDataLostFlag = 0;//发送数据丢失标志
DWORD DeviceStatus = 0;//各器件状态,最低位开始依次是eep，flash，rtc，计量芯片，spi接收数据，复位引脚
WORD IsChargingFlag;//低十二位用来表示是否正在充电
BYTE IsRecWaveData;//低四位表示是否在接收数据
BYTE gAllowSendWaveData = 1;//是否允许发送波形数据
TSampleUIK UIFactorArr[SAMPLE_CHIP_NUM];
TChargePara gChargePara;
BYTE OpeSampleRegChipNo = 0;//读写计量芯片寄存器的采样芯片号，0-3
TWaveDataDeal WaveDataDeal[SAMPLE_CHIP_NUM] = {
	[0] = {
		.SampleChipNo = 0,
	},
	[1] = {
		.SampleChipNo = 1,
	},
	[2] = {
		.SampleChipNo = 2,
	},
	[3] = {
		.SampleChipNo = 3,
	}};
BYTE StartChargeFlag[LOOP_CNT];//开始充电标志
BYTE BeginSendData = 0;//开始发送数据标志
BYTE SendLoop = 0;//发送回路号，1-12

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern BOOL IsLeapYear(BYTE Year);


//-----------------------------------------------
// 函数功能:检查波形相关参数
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void api_CheckWaveParaAndUIK(void)
{
	BYTE i = 0;
	BOOL Result = FALSE;

	// 检查gMaxRecWaveTime的效验和
	if (gChargePara.CRC32 != lib_CheckCRC32((BYTE *)&gChargePara, sizeof(gChargePara) - sizeof(DWORD)))
	{
		Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.ChargePara), sizeof(TChargePara), (BYTE *)&gChargePara);
		if (Result != TRUE)
		{
			memcpy((BYTE *)&gChargePara, (BYTE *)&ChargeParaConst, sizeof(TChargePara));
		}
	}

	for (i = 0; i < SAMPLE_CHIP_NUM; i++)
	{
		if (UIFactorArr[i].CRC32 != lib_CheckCRC32((BYTE *)&UIFactorArr[i], sizeof(UIFactorArr[i]) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleUIKArr[i].Uk[0]), sizeof(TSampleUIK), (BYTE *)&UIFactorArr[i]);
			if (Result != TRUE)
			{
				memcpy((BYTE *)&UIFactorArr[i], (BYTE *)&UIFactorConst, sizeof(TSampleUIK));
			}
		}
	}
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
void api_PowerUpWaveTxRx(void)
{
#if (SELECT_SEND_WAVE_DATA == YES)
	UartSendDmaInit();
#endif
	api_ChargeDetectInit();
	api_ResetModule4g();
}

//-----------------------------------------------
// 函数功能:读有效值判断充电是否结束
//
// 参数:LoopNo 回路号,从0开始
//
// 返回值:TRUE 本回路电压小于阈值
//
// 备注:
//-----------------------------------------------
BYTE CheckRemoteVol(BYTE LoopNo)
{
	BYTE ChipNo = LoopNo / 3;  // 计算芯片号
	BYTE PhaseNo = LoopNo % 3; // 计算相号
	TFourByteUnion Voltage;

	memset(&Voltage, 0, sizeof(Voltage));

	if (api_GetRemoteData2((PHASE_A + (PhaseNo * 0x1000)) | REMOTE_U, DATA_HEX, 4, 4, &Voltage.b[0], &RemoteValueArr[ChipNo]) == TRUE)
	{
		if (Voltage.l <= (gChargePara.ChargeFinishVol * 10))//乘以10才是voltage读取值
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
// 函数功能: 检查充电标志
//
// 参数:SampleChipNo 0-3 计量芯片号
//
//
// 返回值:
//
// 备注:只检查一个计量芯片的充电标志
//-----------------------------------------------
void CheckChargeSign(BYTE SampleChipNo)
{
	BYTE loopIndex = 0;	
	static eState CheckSignState[LOOP_CNT] = {eCheckSign};//充电标志检查状态
	static DWORD LastTickTime[LOOP_CNT] = {0};//检测到充电后保存时间，等待uip刷新
    static BYTE ChargeFinishCnt[LOOP_CNT] = {0};//充电结束计数
	DWORD TimeGap = 0;

	for( loopIndex = SampleChipNo * 3; loopIndex < (SampleChipNo + 1) * 3; loopIndex++)
	{
		switch (CheckSignState[loopIndex])
		{
		case eCheckSign:
			// 一次判断一个回路是否开始充电
			if ((StartChargeFlag[loopIndex] == 1))
			{
				StartChargeFlag[loopIndex] = 0;
				if (BeginSendData == 0)
				{
					BeginSendData = 1;
					SendLoop = loopIndex + 1;
				}
				else
				{
				}
				CheckSignState[loopIndex] = eWaitUIPRefresh;
				LastTickTime[loopIndex] = gsysTick; // 记录当前时间
			}
			break;
		case eWaitUIPRefresh:
			if (gsysTick >= LastTickTime[loopIndex])
			{
				TimeGap = gsysTick - LastTickTime[loopIndex]; // 计算时间差
			}
			else // 处理时间溢出情况
			{
				TimeGap = 0xFFFFFFFF - LastTickTime[loopIndex] + gsysTick; // 计算时间差
			}
			if (TimeGap >= WAIT_UIP_REFRESH_TIME) // 1秒后判断充电是否结束
			{
				CheckSignState[loopIndex] = eWaitChargeEnd;
			}
			break;
		case eWaitChargeEnd:
			if (CheckRemoteVol(loopIndex) == TRUE)//每20ms进中断判断一次
			{
				ChargeFinishCnt[loopIndex]++;
				if (ChargeFinishCnt[loopIndex] >= CHARGE_FINISH_CNT)//连续2s低于阈值判断为充电结束
				{
					ChargeFinishCnt[loopIndex] = 0;
					EXTINT_ClearExtIntStatus(ExtIntCof[4 + loopIndex].channel);
					if ((BeginSendData == 1) && (SendLoop == loopIndex + 1))
					{
						BeginSendData = 0;
					}
					CheckSignState[loopIndex] = eCheckSign;
					IsChargingFlag &= ~(1 << (loopIndex));
					api_EnableChargeInt(loopIndex);
				}
			}
			else
			{
				ChargeFinishCnt[loopIndex] = 0;
			}
			break;
		default:
			break;
		}
	}
}
//-----------------------------------------------
// 函数功能: 准备并发送波形数据
//
// 参数: WaveDataDeal 
//      
//
// 返回值: 无
//
// 备注: 根据回路选择发送数据源并通过DMA发送
//-----------------------------------------------
void CheckAndSendWaveData(TWaveDataDeal *WaveDataDeal)
{
	BYTE *buf = NULL;
	BYTE SendCycle = 0;
	BYTE SendPhaseIndex = 0;
	static eSendState SendState[SAMPLE_CHIP_NUM] = {eCheckSignAndSendFirstFrame}; // 为每个实例保存状态
	Serial[e4gWaveData].WatchSciTimer = WATCH_SCI_TIMER; //要改为数据串口

	SendCycle = (WaveDataDeal->WriteToBufCycleCount + TEMP_SAVE_CYCLE_NUM - 2) % TEMP_SAVE_CYCLE_NUM;
	CheckChargeSign(WaveDataDeal->SampleChipNo); // 检查充电标志
    // 计算相数
	SendPhaseIndex = (SendLoop % 3 + 2) % 3;
    
	switch (SendState[WaveDataDeal->SampleChipNo])
	{
	case eCheckSignAndSendFirstFrame:
		if (BeginSendData == 1)
		{
			if ((SendLoop >= (WaveDataDeal->SampleChipNo * 3 + 1)) && (SendLoop <= (WaveDataDeal->SampleChipNo * 3 + 3)))
			{
			}
			else // 不是该采样芯片的回路
			{
				break;
			}
			
			// 往前推两个周期发送
			WaveDataDeal->gCycleNumb = 1;
			buf = &WaveDataDeal->WAVE_Buffer[SendCycle][SendPhaseIndex][0];
			FillFrameHeadAndTail(buf, WaveDataDeal->Sec, WaveDataDeal->gCycleNumb, SendLoop, eFirstFrame);
			SendState[WaveDataDeal->SampleChipNo] = eCheckSignAndSendOtherFrame;
			SendFrameCnt++;
		}
		else
		{
			break;
		}
		break;
	case eCheckSignAndSendOtherFrame:
		if (BeginSendData == 1)
		{
			if (SendFrameCnt >= (50 * gChargePara.MaxRecWaveTime * 60 - 1)) //超时结束
			{
				BeginSendData = 0;
				SendFrameCnt = 0;
				buf = &WaveDataDeal->WAVE_Buffer[SendCycle][SendPhaseIndex][0];
				FillFrameHeadAndTail(buf, WaveDataDeal->Sec, WaveDataDeal->gCycleNumb, SendLoop,eLastFrame);
				SendState[WaveDataDeal->SampleChipNo] = eCheckSignAndSendFirstFrame;
				SendLoop = 0xff;
			}
			else
			{
				buf = &WaveDataDeal->WAVE_Buffer[SendCycle][SendPhaseIndex][0];
				FillFrameHeadAndTail(buf, WaveDataDeal->Sec, WaveDataDeal->gCycleNumb, SendLoop,eMiddleFrame);
                SendFrameCnt++;
            }
		}
		else //充电结束
		{
			SendFrameCnt = 0;
			buf = &WaveDataDeal->WAVE_Buffer[SendCycle][SendPhaseIndex][0];
			FillFrameHeadAndTail(buf, WaveDataDeal->Sec, WaveDataDeal->gCycleNumb, SendLoop, eLastFrame);
			SendState[WaveDataDeal->SampleChipNo] = eCheckSignAndSendFirstFrame;
			SendLoop = 0xff;
		}
		break;
	default:
		break;
	}
	if ((buf != NULL) && (gAllowSendWaveData == 1))
	{
		UartSendWaveDataByDma(buf, WAVE_FRAME_SIZE);
	}
	if(BeginSendData == 0)
	{
		TX_WAVE_LED_OFF;
	}
}
