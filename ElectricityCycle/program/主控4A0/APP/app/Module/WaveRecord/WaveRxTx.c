//----------------------------------------------------------------------
// Copyright (C)
// ������
// ��������
// ����		���μ�¼�ļ�
// �޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "stdio.h"
#include <string.h>
#include "WaveRxTx.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define POWER_DOWN_VOL_VALUE 			10//v?
#define WAIT_UIP_REFRESH_TIME			1000//ms
#define CHARGE_FINISH_CNT				100//����������
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef  enum
{
	eCheckSign = 0,
	eWaitUIPRefresh,//�ȴ�uipˢ�£������⵽����uip��δˢ�»��ж�Ϊ������
	eWaitChargeEnd,
} eState;//����־���״̬


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern volatile DWORD gsysTick;
DWORD SendFrameCnt = 0;		//����֡�����������г�ʱ
BYTE SendDataLostFlag = 0;//�������ݶ�ʧ��־
DWORD DeviceStatus = 0;//������״̬,���λ��ʼ������eep��flash��rtc������оƬ��spi�������ݣ���λ����
WORD IsChargingFlag;//��ʮ��λ������ʾ�Ƿ����ڳ��
BYTE IsRecWaveData;//����λ��ʾ�Ƿ��ڽ�������
BYTE gAllowSendWaveData = 1;//�Ƿ������Ͳ�������
TSampleUIK UIFactorArr[SAMPLE_CHIP_NUM];
TChargePara gChargePara;
BYTE OpeSampleRegChipNo = 0;//��д����оƬ�Ĵ����Ĳ���оƬ�ţ�0-3
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
BYTE StartChargeFlag[LOOP_CNT];//��ʼ����־
BYTE BeginSendData = 0;//��ʼ�������ݱ�־
BYTE SendLoop = 0;//���ͻ�·�ţ�1-12

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern BOOL IsLeapYear(BYTE Year);


//-----------------------------------------------
// ��������:��鲨����ز���
//
// ����:
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
void api_CheckWaveParaAndUIK(void)
{
	BYTE i = 0;
	BOOL Result = FALSE;

	// ���gMaxRecWaveTime��Ч���
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
// ��������:��ʼ�����μ�¼����
//
// ����:
//
// ����ֵ:
//
// ��ע:
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
// ��������:����Чֵ�жϳ���Ƿ����
//
// ����:LoopNo ��·��,��0��ʼ
//
// ����ֵ:TRUE ����·��ѹС����ֵ
//
// ��ע:
//-----------------------------------------------
BYTE CheckRemoteVol(BYTE LoopNo)
{
	BYTE ChipNo = LoopNo / 3;  // ����оƬ��
	BYTE PhaseNo = LoopNo % 3; // �������
	TFourByteUnion Voltage;

	memset(&Voltage, 0, sizeof(Voltage));

	if (api_GetRemoteData2((PHASE_A + (PhaseNo * 0x1000)) | REMOTE_U, DATA_HEX, 4, 4, &Voltage.b[0], &RemoteValueArr[ChipNo]) == TRUE)
	{
		if (Voltage.l <= (gChargePara.ChargeFinishVol * 10))//����10����voltage��ȡֵ
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
// ��������: ������־
//
// ����:SampleChipNo 0-3 ����оƬ��
//
//
// ����ֵ:
//
// ��ע:ֻ���һ������оƬ�ĳ���־
//-----------------------------------------------
void CheckChargeSign(BYTE SampleChipNo)
{
	BYTE loopIndex = 0;	
	static eState CheckSignState[LOOP_CNT] = {eCheckSign};//����־���״̬
	static DWORD LastTickTime[LOOP_CNT] = {0};//��⵽���󱣴�ʱ�䣬�ȴ�uipˢ��
    static BYTE ChargeFinishCnt[LOOP_CNT] = {0};//����������
	DWORD TimeGap = 0;

	for( loopIndex = SampleChipNo * 3; loopIndex < (SampleChipNo + 1) * 3; loopIndex++)
	{
		switch (CheckSignState[loopIndex])
		{
		case eCheckSign:
			// һ���ж�һ����·�Ƿ�ʼ���
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
				LastTickTime[loopIndex] = gsysTick; // ��¼��ǰʱ��
			}
			break;
		case eWaitUIPRefresh:
			if (gsysTick >= LastTickTime[loopIndex])
			{
				TimeGap = gsysTick - LastTickTime[loopIndex]; // ����ʱ���
			}
			else // ����ʱ��������
			{
				TimeGap = 0xFFFFFFFF - LastTickTime[loopIndex] + gsysTick; // ����ʱ���
			}
			if (TimeGap >= WAIT_UIP_REFRESH_TIME) // 1����жϳ���Ƿ����
			{
				CheckSignState[loopIndex] = eWaitChargeEnd;
			}
			break;
		case eWaitChargeEnd:
			if (CheckRemoteVol(loopIndex) == TRUE)//ÿ20ms���ж��ж�һ��
			{
				ChargeFinishCnt[loopIndex]++;
				if (ChargeFinishCnt[loopIndex] >= CHARGE_FINISH_CNT)//����2s������ֵ�ж�Ϊ������
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
// ��������: ׼�������Ͳ�������
//
// ����: WaveDataDeal 
//      
//
// ����ֵ: ��
//
// ��ע: ���ݻ�·ѡ��������Դ��ͨ��DMA����
//-----------------------------------------------
void CheckAndSendWaveData(TWaveDataDeal *WaveDataDeal)
{
	BYTE *buf = NULL;
	BYTE SendCycle = 0;
	BYTE SendPhaseIndex = 0;
	static eSendState SendState[SAMPLE_CHIP_NUM] = {eCheckSignAndSendFirstFrame}; // Ϊÿ��ʵ������״̬
	Serial[e4gWaveData].WatchSciTimer = WATCH_SCI_TIMER; //Ҫ��Ϊ���ݴ���

	SendCycle = (WaveDataDeal->WriteToBufCycleCount + TEMP_SAVE_CYCLE_NUM - 2) % TEMP_SAVE_CYCLE_NUM;
	CheckChargeSign(WaveDataDeal->SampleChipNo); // ������־
    // ��������
	SendPhaseIndex = (SendLoop % 3 + 2) % 3;
    
	switch (SendState[WaveDataDeal->SampleChipNo])
	{
	case eCheckSignAndSendFirstFrame:
		if (BeginSendData == 1)
		{
			if ((SendLoop >= (WaveDataDeal->SampleChipNo * 3 + 1)) && (SendLoop <= (WaveDataDeal->SampleChipNo * 3 + 3)))
			{
			}
			else // ���Ǹò���оƬ�Ļ�·
			{
				break;
			}
			
			// ��ǰ���������ڷ���
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
			if (SendFrameCnt >= (50 * gChargePara.MaxRecWaveTime * 60 - 1)) //��ʱ����
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
		else //������
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
