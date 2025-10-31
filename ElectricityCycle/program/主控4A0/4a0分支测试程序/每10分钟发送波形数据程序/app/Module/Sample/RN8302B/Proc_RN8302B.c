//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	������
//��������	2016.7.30
//����		����оƬRN2026�������ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "RN8302B.h"
#include "AHour.h"
#include "statistics_voltage.h"
#include "Sample_API.h"

#if (SAMPLE_CHIP == CHIP_RN8302B)

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define WAVE_CRC_ERR_RECORD_CNT 	5
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eUpDateSumCheck,
	eSumCheck
} eCheckType;
#pragma pack(1)
typedef struct
{
	eIntegratorMode eintegratormode;
	eMeasurementMode emeasurementmode;
	DWORD CRC32;
} SampleRunPara;
#pragma pack()
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE SampleNo = CS_SPI_SAMPLE1;//csѡ��
TRemoteValue RemoteValue;						//�˶����� ��˲ʱ������
TRemoteValue RemoteValueArr[SAMPLE_CHIP_NUM];					//��·
TSampleCorr SampleCorr;							//˲ʱ��ϵ��
TSampleAdjustReg SampleAdjustReg[SAMPLE_CHIP_NUM];				//����оƬУ��������
TSampleCtrlReg SampleCtrlReg;					//����оƬ����������
TCurrenttValue CurrentValue[9];					//�ֶβ����ͻ�����������
TSampleAdjustParaDiv SampleAdjustParaDiv;		//�ֶβ�������
TSampleManualModifyPara SampleManualModifyPara; //�ֶ�����ϵ��
TOriginalPara OriginalPara;						//����Ĭ������ʹ�С���ϵ�� ���ĳ����δУ Ĭ�ϲ��ø�ϵ��
TEDTDataPara EDTDataPara;						//EDT��׼����
BYTE byHarEnergyPulse;							//��������г���������
BYTE bySampleStep;								//��ѭ��ˢ��Զ������Э������ÿ���һ�Σ�ÿ���ڴ�ѭ�����ˢ�¸���Զ������
TExtremumMax ExtremumMax;
TExtremumMin ExtremumMin;
TExtremumMax ExtremumMaxHis; //����0��ת������
TExtremumMin ExtremumMinHis;
TXtalConRom XtalPara;
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
TPAverage Paverage[SAMPLE_CHIP_NUM][3][4]; //ƽ������

DWORD VMAXLimit;				  //��ѹ���ޱ���ֵ
DWORD IMINLimit;				  //�������ޱ���ֵ
DWORD IMAXLimit;				  //�������ޱ���ֵ
BYTE bySamplePowerOnTimer;		  //�ϵ絹��ʱ������
BYTE byReWriteSampleChipRegCount[SAMPLE_CHIP_NUM]; //�ж�оƬЧ�鲻�Ի�д������

DWORD dwSamplePFlag[SAMPLE_CHIP_NUM];	   //�й����޹�����
DWORD dwSampleSFlag[SAMPLE_CHIP_NUM];	   //Ǳ�����𶯱�־
BOOL UnBalanceGainFlag[3]; //��ѹ��ƽ�ⲹ����־ TRUE-�Ѳ��� FALSE-δ����
BYTE TopoErrFlag = 0;
//�ֶβ����йز���
BYTE g_ucNowSampleDivisionNo[MAX_PHASE]; //�ֶβ�����ǰ����
BYTE ChecksumFlag;//δʹ��
static DWORD dwtempMaxU[3];//δʹ��
static DWORD dwtempMinU[3];//δʹ��
static DWORD dwtempMaxI[3];//δʹ��
static DWORD dwtempMinI[3];//δʹ��
static BYTE RecoverNum[SAMPLE_CHIP_NUM] = {0};
static QWORD _success_cnt_iap[2] = {0, 0};//δʹ��
static QWORD _fail_cnt_iap[2] = {0, 0};//δʹ��
const DWORD bLock3[2] = {0x000000EA, 0x00000000};
SampleRunPara samplerunpara = {
	.eintegratormode = eUnuseIntegrator,
	.emeasurementmode = eThreePhase,
	.CRC32 = 0,
};
const DWORD SampleCtrlUnuseIntegrator[][3] =
	{
		{ROS_CFG, 0x00000000, 4},
};
const DWORD SampleCtrlUseIntegrator[][3] =
	{
		{ROS_DCATTC, 0x00007fdf, 4},
		{ROS_CFG, 0x00000007, 4},
};
const DWORD SampleCtrlOnePhase[][3] =
	{
		{EMUCON_EMU, 0x00001111, 3},
		{SYSCFG, 0xEA03BA36, 4},
};
const DWORD SampleCtrlThreePhase[][3] =
	{
		{EMUCON_EMU, 0x00007777, 3},
		{SYSCFG, 0xEA03BA00, 4},
};
const DWORD SampleCtrlTemp[][3] =
	{
		{ECT_WREN, 0x000000EA, 4},
		{SAR_CTL0, 0x8787CE10, 4},
		{SAR_CTL1, 0000000000, 4},
		{SAR_EN, 0000000001, 1},
		{ECT_WREN, 0x00000000, 4},
};
const DWORD SampleCtrleAngle[][3] =
	{
		{PHSUA_EMU, 0xA7000200, 4},
		{PHSUA_EMU, 0xA7000200, 4},
		{PHSUB_EMU, 0x00000200, 4},
		{PHSUC_EMU, 0x00000200, 4},
};

#if (SEL_TOPOLOGY == YES)
// const DWORD SampleCtrlTopo[][3] =
// 	{
// 		{HSDCCTL_EMU, 0x00000004, 4},  // spi����
// 		{WSAVECON_EMU, 0x00000010, 1}, // ����������
// 		{DMA_WAVECON3_EMU, 0x00000000, 4},
// 		{SOFTRST_EMU, 0x000000E1, 1},
// 		{SOFTRST_EMU, 0x0000004C, 1},
// 		{DMA_WAVECON_EMU, 0x00000000, 4},  // �ı䲨�β�����
// 		{WSAVECON_EMU, 0x000000A0, 1},	   // ͬ������ 128�㣬BUFCFG[3��0] = 0�����λ������ã�����abc����
// 		{HSDCCTL_EMU, 0x0000021D, 4},	   // spi���ã�bit9�Ǳ���λ��Ӱ��spi���õ�����0�ᵼ�²������ݴ���
// 		{DMA_WAVECON3_EMU, 0x00000001, 4}, // �����������棬BUFCFG[4] = 0
// 		{DMA_WAVECON2_EMU, 0x00000200, 4}, // bit9Ϊ1ʹ�������Զ����ڣ�bit10Ϊ1ʹ��г��˥��������
// // bit8Ϊ1�� ͨ��ֱ�� OFFSET У���Ĵ��� DCOSx(1AH~20H)���� DMA ͬ������ͨ����ֱ�� OFFSET У��
// };
const DWORD SampleCtrlTopo[][3]=
{
	{HSDCCTL_EMU,0x00000004,4},
	{SYSCFG_EMU,0xEA00BA00,4},			//0X0178
	{DMA_WAVECON3_EMU,0x00000000,4},
	{SOFTRST_EMU,0x000000E1,1},
	{SOFTRST_EMU,0x0000004C,1},
	{DMA_WAVECON_EMU,0x00000000,4},
	{WSAVECON_EMU,0x000000A0,1},		//0x0163
	{HSDCCTL_EMU,0x0000021D,4},
	{DMA_WAVECON3_EMU,0x00000001,4},
	{DMA_WAVECON2_EMU,0x00000200,4},
};
#endif

const DWORD SampleGetTemp[][3] =
	{
		{ECT_WREN, 0x000000EA, 4},
		{SAR_CTL1, 0000000001, 4},
		{ECT_WREN, 0x00000000, 4},
};
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void HardResetSampleChip(BYTE chipNo);
static void CheckSampleRam(eSampleType Type, BYTE chipNo);
//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
//��������:  �жϾ����쳣
//
//����:
//
//����ֵ:    TRUE  ���ھ����쳣״̬
//
//��ע:
//--------------------------------------------------
BOOL IsCrystalErr(BYTE chipNo)
{
	DWORD Testaddrmsg[4];
    BYTE res = 0;
	MEM_ZERO_ARRAY(Testaddrmsg);
	res = Link_ReadSampleReg(SYSSR, (BYTE *)&Testaddrmsg[0], 2, chipNo);

	if ((Testaddrmsg[0] >> 14) == 1)
	{
		res = Link_ReadSampleReg(CFCFG_EMU, (BYTE *)&Testaddrmsg[1], 3, chipNo);
		res = Link_ReadSampleReg(EMUCFG_EMU, (BYTE *)&Testaddrmsg[2], 3, chipNo);
		res = Link_ReadSampleReg(EMUCON_EMU, (BYTE *)&Testaddrmsg[3], 3, chipNo);
		if ((Testaddrmsg[1] == Testaddrmsg[2]) && (Testaddrmsg[1] == Testaddrmsg[3]))
		{
			return TRUE;
		}
	}
	return FALSE;
}
//--------------------------------------------------
//��������:  ����оƬ����ʱ���ݻָ�
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void ChipRecovery(BYTE chipNo)
{
	RecoverNum[chipNo]++;
	if (RecoverNum[chipNo] >= 10)
	{
		RecoverNum[chipNo] = 10;
		HardResetSampleChip(chipNo);
	}
	UseSampleDefaultPara(eALL, chipNo);
	CheckSampleRam(eALL, chipNo);
	if (CheckSampleChipReg(chipNo))
	{
		GetSampleChipCheckSum(chipNo);
	}
}
//--------------------------------------------------
//��������: ��EE��ȡ������·��
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
BYTE GetCountFormEE(void)
{
	TSampleChipPara SampleChipPara;

	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleChipPara), sizeof(TSampleChipPara), (BYTE *)&SampleChipPara) != TRUE)
	{
		SampleChipPara.bCount = 3;
	}
	return SampleChipPara.bCount;
}
//--------------------------------------------------
//��������:  У�����в���
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void CheckSampleRunPara(void)
{
	if (lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD)) != samplerunpara.CRC32)
	{
		samplerunpara.eintegratormode = eUnuseIntegrator;
		samplerunpara.emeasurementmode = eThreePhase;
		samplerunpara.CRC32 = lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD));
	}
}
//-----------------------------------------------
//��������: дĳ��У��ϵ����EEPROM
//
//����:		WriteEepromFlag[in]	bit0 SampleCtrlAddr 0--дram��дee	1--дramдee
//								bit1 SampleAdjustAddr 0--дram��дee	1--дramдee
//			Addr[in]	��ַ
//			Buf[in]		д�����ݣ����ֽڣ����ں󣬵���ǰ
//����ֵ:
//		    TRUE/FALSE
//��ע:
//-----------------------------------------------
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag, DWORD Addr, BYTE *Buf, BYTE chipNo)
{
	BYTE i, DivCurrentNo;
	DWORD *p;
	short QGainTmp, PGainTmp;

	for (i = 0; i < (sizeof(SampleCtrlAddr) / sizeof(SampleCtrlAddr[0])); i++)
	{
		if (Addr == SampleCtrlAddr[i][0])
		{
			p = (DWORD *)&SampleCtrlReg;
			memcpy((void *)&p[i], Buf, sizeof(DWORD));

			if ((WriteEepromFlag & 0x01) == 0x00)
			{
				SampleCtrlReg.CRC32 = lib_CheckCRC32((BYTE *)&SampleCtrlReg, sizeof(TSampleCtrlReg) - sizeof(DWORD));
			}
			else
			{
				//��дeeУ��ϵ������
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg), sizeof(TSampleCtrlReg), (BYTE *)&SampleCtrlReg);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}
			return TRUE;
		}
	}

	for (i = 0; i < (sizeof(SampleAdjustAddr) / sizeof(SampleAdjustAddr[0])); i++)
	{
		if (Addr == SampleAdjustAddr[i][0])
		{
			p = (DWORD *)&SampleAdjustReg[chipNo];
			memcpy((void *)&p[i], Buf, sizeof(DWORD));

			//ֻ����RAM�ͼ���оƬ ��дEE
			if ((WriteEepromFlag & 0x02) == 0x00)
			{
				SampleAdjustReg[chipNo].CRC32 = lib_CheckCRC32((BYTE *)&SampleAdjustReg[chipNo], sizeof(TSampleAdjustReg) - sizeof(DWORD));
			}
			else
			{
				//��дeeУ��ϵ������
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg[chipNo]), sizeof(TSampleAdjustReg), (BYTE *)&SampleAdjustReg[chipNo]);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}

			return TRUE;
		}
	}

	return FALSE;
}
//--------------------------------------------------
// ��������:  Ч�����оƬУ��Ĵ���
//
// ����:    eCheckType
//						eUpDateSumCheck  ����У��
//						eSumCheck		���У��
//
// ����ֵ:    FALSE ����оƬ�쳣
//
// ��ע:
//--------------------------------------------------
BOOL CheckSampleSumRegPro(eCheckType echecktype, BYTE chipNo)
{
	static DWORD dwSumChecks[SAMPLE_CHIP_NUM][5] = {ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F};
	DWORD dwSumChecksTemp = 0;
	DWORD dwSumChecksTemp2 = 0;
	BYTE i = 0, j = 0;
	WORD addrs[5] = {CKSUM_EMU1, CKSUM_EMU2, CKSUM_EMU3, CKSUM_EMU4, SYSCFG};
	BYTE lens[5] = {3, 2, 4, 4, 4};

	if (IsCrystalErr(chipNo))
	{
		return FALSE;
	}

	if (echecktype == eUpDateSumCheck)
	{
		for (i = 0; i < (sizeof(dwSumChecks[0]) / sizeof(dwSumChecks[0][0])); i++)
		{
			for (j = 0; j < 3; j++)
			{
				dwSumChecksTemp = 0;
				dwSumChecksTemp2 = 0;
				if (Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp, lens[i], chipNo) == TRUE)
				{
					if (Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp2, lens[i], chipNo) == TRUE)
					{
						if (dwSumChecksTemp == dwSumChecksTemp2)
						{
							dwSumChecks[chipNo][i] = dwSumChecksTemp;
							break;
						}
					}
				}
			}
		}
	}
	else if (echecktype == eSumCheck)
	{
		for (i = 0; i < (sizeof(dwSumChecks[0]) / sizeof(dwSumChecks[0][0])); i++)
		{
			dwSumChecksTemp = 0;
			if (Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp, lens[i], chipNo) == TRUE)
			{
				if (dwSumChecks[chipNo][i] != dwSumChecksTemp)
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}
//-----------------------------------------------
//��������: ��ȡ����оƬУ������Ч���
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
BYTE GetSampleChipCheckSum(BYTE chipNo)
{
	return CheckSampleSumRegPro(eUpDateSumCheck, chipNo);
}
//-----------------------------------------------
//��������: ����һ����ƽ������
//
//����:		��
//
//����ֵ: 	��
//
//��ע:����RemoteValue.Pav��ΪHEX��
//-----------------------------------------------
// void api_CalPaverage(void)
// {
// 	BYTE i, j;
// 	static BYTE PowerUPFlag = 0X55; //�ϵ��һ��ƽ������Ϊ0

// 	for (i = 0; i < 3; i++)
// 	{
// 		for (j = 0; j < 4; j++)
// 		{
// 			if (((MeterTypesConst == METER_3P3W) && (j == 2)) //3P3W��B��
// 				|| (Paverage[chipNo][i][j].Cnt == 0)				  //����ֵΪ0
// 				|| (PowerUPFlag == 0X55))					  //�ϵ��һ�μ���
// 			{
// 				//���ϴμ�����
// 				continue;
// 			}
// 			else
// 			{
// 				RemoteValueArr[chipNo].Pav[i][j].l = (long)(Paverage[chipNo][i][j].SumValue / (float)Paverage[chipNo][i][j].Cnt);
// 			}
// 		}
// 	}

// 	memset((BYTE *)(Paverage), 0, sizeof(Paverage));

// 	PowerUPFlag = 0;
// }

//-----------------------------------------------
//��������: �����ѹ��������ƽ����
//
//����:		Type[in]	0--��ѹ	1--����
//			Buf[in]	���������ѹֵ���ߵ���ֵ
//
//����ֵ: 	��ƽ���� �������ͣ�long-unsigned����λ��%�����㣺-2
//
//��ע:ˢ��C������ʱͬ��ˢ�²�ƽ����
//-----------------------------------------------
static WORD CaleUnbalanceRate(eUIPType Type, long *Buf)
{
	DWORD dwMax, dwMin;
	DWORD dwData[3];
	WORD Result;

	dwData[0] = labs(Buf[0]);
	if (MeterTypesConst == METER_3P3W)
	{
		//������ʱ��dwData[1]��A��ֵ��ͬ
		dwData[1] = dwData[0];
	}
	else
	{
		dwData[1] = labs(Buf[1]);
	}
	dwData[2] = labs(Buf[2]);

	if (Type == eVOLTAGE)
	{
		//�����ٽ��ѹ 60%Un��3λС����
		dwMin = wStandardVoltageConst * 100 * 6 / 10;
	}
	else
	{
		//����5%Ib��4λС����
		dwMin = dwMeterBasicCurrentConst * 10 * 5 / 100;
	}

	//�������С��ʱ�����㲻ƽ����
	if ((dwData[0] < dwMin) && (dwData[1] < dwMin) && (dwData[2] < dwMin))
	{
		return 0;
	}

	dwMax = dwData[0];
	dwMin = dwData[0];

	//�������ֵ
	if (dwMax < dwData[1])
	{
		dwMax = dwData[1];
	}
	if (dwMax < dwData[2])
	{
		dwMax = dwData[2];
	}

	//������Сֵ
	if (dwMin > dwData[1])
	{
		dwMin = dwData[1];
	}
	if (dwMin > dwData[2])
	{
		dwMin = dwData[2];
	}

	if (dwMax == 0)
	{
		Result = 0;
	}
	else
	{
		Result = (WORD)((long long)(dwMax - dwMin) * 10000 / dwMax);
	}

	if (Result > 10000)
	{
		Result = 10000;
	}

	return Result;
}

//-----------------------------------------------
//��������: ˢ��˲ʱ�����ݲ�ˢ�±�־U, I[0], P, I[1], F, S, Cos, Pav
//
//����:		chipNo ��0��ʼ
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
static void Read_UIP(BYTE chipNo)
{
	BYTE i, DataSign;
	BYTE CurRatio;
	BYTE bySamplePulseNum;
	DWORD tdw, TotalEnergy1, TotalEnergy2, RatioEnergy1, RatioEnergy2;
	DWORD ILimit, VoltageLimit,VMINLimit;
	long tl;
	float tf;
	BYTE Result, Cmd, Count;
	TFourByteUnion ComData; //������

	if (bySampleStep == 0xff)
	{
		return;
	}
	Cmd = bySampleStep;
	ComData.d = 0;
	bySamplePulseNum = 0;

	switch (Cmd)
	{
	case 0x00: //Ǳ���𶯱�־
		Result = Link_ReadSampleReg(Status_EMU, ComData.b, 2, chipNo);
		if (Result == FALSE)
		{
			break; //�����ȡ����оʧ�ܣ�ʹ����һ������
		}
		//��־
		dwSampleSFlag[chipNo] = ComData.d;
		//dwSamplePFlag = ComData.d;
		break;
		//��ѹ
	case UA_EMU:
	case UB_EMU:
	case UC_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		tdw = (DWORD)((QWORD)ComData.d * 1000 / SampleCorr.UGain[Cmd - UA_EMU].d);
		VMINLimit = wStandardVoltageConst * 4; // 4%Un
		if (tdw > VMAXLimit)
		{
			tdw = VMAXLimit;
		}
		if (tdw < VMINLimit)
		{
			tdw = 0;
		}
		RemoteValueArr[chipNo].U[Cmd - UA_EMU].d = tdw;

		//��ѹ��ƽ����
		if (Cmd == 3)
		{
			RemoteValueArr[chipNo].Uunbalance.w = CaleUnbalanceRate(eVOLTAGE, (long *)&RemoteValueArr[chipNo].U[0].l);
		}
		break;
	case PA_EMU:
	case PB_EMU:
	case PC_EMU:
		//P�й����ʣ�6λС��,��λKW
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		tl = (long)((double)ComData.l * 100000.0 / (double)SampleCorr.PGain[Cmd - PA_EMU + 1].d);
		//�ж��Ƿ������������ͬʱ���ʷ���λΪ1
		if ((dwSampleSFlag[chipNo] & (P_CREEP << (Cmd - PA_EMU))) != 0)
		{
			tl = 0;
		}
		RemoteValueArr[chipNo].P[Cmd - PA_EMU + 1].l = tl;

		if (tl < 0)
		{
			//���÷��෴���־
			api_SetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - PA_EMU + 8 * chipNo);
		}
		else if (tl > 0)
		{
			api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - PA_EMU + 8 * chipNo);
		}

		Paverage[chipNo][0][Cmd - PA_EMU + 1].SumValue += (float)RemoteValueArr[chipNo].P[Cmd - PA_EMU + 1].l;
		Paverage[chipNo][0][Cmd - PA_EMU + 1].Cnt++;

		break;
	case PT_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		tl = (long)((double)ComData.l * 100000.0 / (double)SampleCorr.PGain[0].d);
		if (((MeterTypesConst == METER_3P3W) && ((dwSampleSFlag[chipNo] & 0x00000005) == 0x00000005)) || ((MeterTypesConst != METER_3P3W) && ((dwSampleSFlag[chipNo] & 0x00000007) == 0x00000007)))
		{
			tl = 0;
		}
		RemoteValueArr[chipNo].P[0].l = tl;

		if (tl < 0)
		{
			//���ú��෴���־
			api_SetSysStatus(eSYS_STATUS_OPPOSITE_P + 8 * chipNo);
		}
		else if (tl > 0)
		{
			api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P + 8 * chipNo);
		}

		Paverage[chipNo][0][0].SumValue += (float)RemoteValueArr[chipNo].P[0].l;
		Paverage[chipNo][0][0].Cnt++;

#if (SEL_DEMAND_2022 == YES)
#if (MAX_PHASE != 1)
		if (tl != 0)
		{
			tl = 1; //��ֹ�ض�
		}
		api_UptCurDemandStatus(0, tl); //���µ�ǰ�й�����״̬,���ڸ��±�־֮��
#endif
#endif

		break;

	case QA_EMU:
	case QB_EMU:
	case QC_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		tl = (long)((double)ComData.l * 100000.0 / (double)SampleCorr.PGain[Cmd - QA_EMU + 1].d);
		//�ж��Ƿ������������ͬʱ���ʷ���λΪ1
		if ((dwSampleSFlag[chipNo] & (Q_CREEP << (Cmd - QA_EMU))) != 0)
		{
			tl = 0;
		}
		RemoteValueArr[chipNo].Q[Cmd - QA_EMU + 1].l = tl;

		if (tl < 0)
		{
			//���÷��෴���־
			api_SetSysStatus(eSYS_STATUS_OPPOSITE_Q_A + Cmd - QA_EMU + 8 * chipNo);
		}
		else if (tl > 0)
		{
			api_ClrSysStatus(eSYS_STATUS_OPPOSITE_Q_A + Cmd - QA_EMU + 8 * chipNo);
		}

		Paverage[chipNo][1][Cmd - QA_EMU + 1].SumValue += (float)RemoteValueArr[chipNo].Q[Cmd - QA_EMU + 1].l;
		Paverage[chipNo][1][Cmd - QA_EMU + 1].Cnt++;
		break;

	case QT_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		tl = (long)((double)ComData.l * 100000.0 / (double)SampleCorr.PGain[0].d);
		if (((MeterTypesConst == METER_3P3W) && ((dwSampleSFlag[chipNo] & 0x00000050) == 0x00000050)) || ((MeterTypesConst != METER_3P3W) && ((dwSampleSFlag[chipNo] & 0x00000070) == 0x00000070)))
		{
			tl = 0;
		}
		RemoteValueArr[chipNo].Q[0].l = tl;

		if (tl < 0)
		{
			//���÷��෴���־
			api_SetSysStatus(eSYS_STATUS_OPPOSITE_Q + 8 * chipNo);
		}
		else if (tl > 0)
		{
			api_ClrSysStatus(eSYS_STATUS_OPPOSITE_Q + 8 * chipNo);
		}
#if (SEL_DEMAND_2022 == YES)
#if (MAX_PHASE != 1)
		if (tl != 0)
		{
			tl = 1; //��ֹ�ض�
		}
		api_UptCurDemandStatus(1, tl); //���µ�ǰ�й�����״̬,���ڸ��±�־֮��
#endif
#endif
		Paverage[chipNo][1][0].SumValue += (float)RemoteValueArr[chipNo].Q[0].l;
		Paverage[chipNo][1][0].Cnt++;
		break;

	//S���ڹ��ʣ�6λС��,��λKW
	case SA_EMU:
	case SB_EMU:
	case SC_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		RemoteValueArr[chipNo].S[Cmd - SA_EMU + 1].l = (long)((double)ComData.l * 100000.0 / (double)SampleCorr.PGain[Cmd - SA_EMU + 1].d);

		if ((RemoteValueArr[chipNo].P[Cmd - SA_EMU + 1].l == 0) && (RemoteValueArr[chipNo].Q[Cmd - SA_EMU + 1].l == 0))
		{
			RemoteValueArr[chipNo].S[Cmd - SA_EMU + 1].l = 0;
		}
		if (RemoteValueArr[chipNo].P[Cmd - SA_EMU + 1].l < 0)
		{
			RemoteValueArr[chipNo].S[Cmd - SA_EMU + 1].l *= -1;
		}
		Paverage[chipNo][2][Cmd - SA_EMU + 1].SumValue += (float)RemoteValueArr[chipNo].S[Cmd - SA_EMU + 1].l;
		Paverage[chipNo][2][Cmd - SA_EMU + 1].Cnt++;
		break;

	case STA_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		RemoteValueArr[chipNo].S[0].l = (long)((double)ComData.l * 1000.0 / (double)SampleCorr.PGain[0].d * 100);
		if ((RemoteValueArr[chipNo].P[0].l == 0) && (RemoteValueArr[chipNo].Q[0].l == 0))
		{
			RemoteValueArr[chipNo].S[0].l = 0;
		}
		if (RemoteValueArr[chipNo].P[0].l < 0)
		{
			RemoteValueArr[chipNo].S[0].l *= -1;
		}

		Paverage[chipNo][2][0].SumValue += (float)RemoteValueArr[chipNo].S[0].l;
		Paverage[chipNo][2][0].Cnt++;
		break;

	//������4λС��,��λA
	//Ӧ���ȶ������ٶ����� ��Ϊ�����ķ�����Ҫ���ݹ����ж�
	case IA_EMU:
	case IB_EMU:
	case IC_EMU:
	case IN_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		RemoteValueArr[chipNo].I[Cmd - IA_EMU].l = (long)((double)ComData.d * 10000.0 / SampleCorr.IGain[Cmd - IA_EMU].d);
		if ((RemoteValueArr[chipNo].I[Cmd - IA_EMU].l > IMAXLimit) || (RemoteValueArr[chipNo].I[Cmd - IA_EMU].l < IMINLimit))
		{
			RemoteValueArr[chipNo].I[Cmd - IA_EMU].l = 0;
		}
		if (Cmd == IN_EMU) //���ߵ���
		{
			if (api_GetSysStatus(eSYS_STATUS_OPPOSITE_P + 8 * chipNo) == TRUE)
			{
				RemoteValueArr[chipNo].I[3].l *= -1;
			}
		}
		else
		{
			if (api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - IA_EMU + 8 * chipNo) == TRUE)
			{
				RemoteValueArr[chipNo].I[Cmd - IA_EMU].l *= -1;
			}
			//������ƽ����
			if (Cmd == 0x12)
			{
				RemoteValueArr[chipNo].Iunbalance.w = CaleUnbalanceRate(eCURRENT, (long *)&RemoteValueArr[chipNo].I[0].l);
			}
		}
		break;

	case IN2_EMU: //����
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 4, chipNo);
		RemoteValueArr[chipNo].I[4].l = (long)((double)ComData.d * 10000.0 / SampleCorr.IGain[0].d);
		if ((RemoteValueArr[chipNo].I[4].l > IMAXLimit) || (RemoteValueArr[chipNo].I[4].l < IMINLimit))
		{
			RemoteValueArr[chipNo].I[4].l = 0;
		}

		RemoteValueArr[chipNo].I[4].l *= 4;
		break;

	case PfA_EMU:
		//Cos��������,4λС��
		//���������Ĵ�������ʳ�����ʱʹ�ù���ֵ���㹦������   δ������������
		for (i = 0; i < 4; i++)
		{
			if (RemoteValueArr[chipNo].S[i].d)
			{
				RemoteValueArr[chipNo].Cos[i].l = (long)(RemoteValueArr[chipNo].P[i].l * 10000.0 / RemoteValueArr[chipNo].S[i].l);
				if (labs(RemoteValueArr[chipNo].Cos[i].l) > 10000)
				{
					RemoteValueArr[chipNo].Cos[i].l = 10000;
				}
			}
			else
			{
				RemoteValueArr[chipNo].Cos[i].l = 10000;
			}

			if ((RemoteValueArr[chipNo].Q[i].l == 0) && (RemoteValueArr[chipNo].P[i].l == 0))
			{
				RemoteValueArr[chipNo].Cos[i].l = 10000;
			}

			if (i == 0)
			{
				if ((RemoteValueArr[chipNo].I[0].l == 0) && (RemoteValueArr[chipNo].I[1].l == 0) && (RemoteValueArr[chipNo].I[2].l == 0))
				{
					RemoteValueArr[chipNo].Cos[i].l = 10000;
				}
			}
			else
			{
				if (RemoteValueArr[chipNo].I[i - 1].l == 0)
				{
					RemoteValueArr[chipNo].Cos[i].l = 10000;
				}
			}

			if (RemoteValueArr[chipNo].P[i].l < 0)
			{
				RemoteValueArr[chipNo].Cos[i].l *= -1;
			}
		}
		break;
	//Ƶ��
	case Freq_U_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 3, chipNo);
		RemoteValueArr[chipNo].Freq.l = 6553600000 / ComData.d;
		break;

	//��ѹ���
	case YUA_EMU:
	case YUB_EMU:
	case YUC_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 3, chipNo);
		RemoteValueArr[chipNo].YUAngle[Cmd - YUA_EMU].d = (DWORD)((double)ComData.d * 100 * 360 / 16777216 / 10);
		break;
	//�������
	case YIA_EMU:
	case YIB_EMU:
	case YIC_EMU:
		Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 3, chipNo);
		RemoteValueArr[chipNo].YIAngle[Cmd - YIA_EMU].d = (DWORD)((double)ComData.d * 100 * 360 / 16777216 / 10);

		if ((Cmd == YIC_EMU) && ((dwSampleSFlag[chipNo] % 0x00000008) == 0x00000000))
		{
			Count = 0;

			if (RemoteValueArr[chipNo].YIAngle[1].d > RemoteValueArr[chipNo].YIAngle[0].d)
			{
				Count++;
			}
			if (RemoteValueArr[chipNo].YIAngle[2].d > RemoteValueArr[chipNo].YIAngle[1].d)
			{
				Count++;
			}
			if (RemoteValueArr[chipNo].YIAngle[2].d > RemoteValueArr[chipNo].YIAngle[0].d)
			{
				Count++;
			}
			api_ClrSysStatus(eSYS_STATUS_REVERSE_PHASE_I);
			if ((Count == 0) || (Count == 2))
			{
				api_SetSysStatus(eSYS_STATUS_REVERSE_PHASE_I);
			}
		}

		break;
	default:
		break;
	}


	if (bySampleStep >= 0x60) //@@@@@��ʱ�Ȳ��ģ�����ȷ��
	{
		bySampleStep = 0xff;
	}
}


//-----------------------------------------------
//��������: �����Ĵ�������ΪĬ����ֵ
//
//����:		eSampleType Type
//
//����ֵ: 	��
//
//��ע:����ʹ��eALL��Ŀǰֻ�г�ʼ��ʹ��
//-----------------------------------------------
void UseSampleDefaultPara(eSampleType Type, BYTE chipNo)
{
	BYTE i;
	WORD TempData;

	InitSampleCaliUI();
	if ((Type == eCORR) || (Type == eALL))
	{
		for (i = 0; i < (sizeof(SampleUIPGain) / sizeof(DWORD)); i++)
		{
			if (i < 3)
			{
				SampleCorr.UGain[i].d = SampleUIPGain[i];
			}
			else if (i < 7)
			{
				SampleCorr.IGain[i - 3].d = SampleUIPGain[i];
			}
			else if (i < 11)
			{
				SampleCorr.PGain[i - 7].d = SampleUIPGain[i];
			}
			else
			{
			}
		}
		//SampleCorr.CRC32 = lib_CheckCRC32((BYTE*)&SampleCorr,sizeof(TSampleCorr)-sizeof(DWORD));
	}

	if ((Type == eCRTL) || (Type == eALL)) //�������� TSampleCtrlPara
	{
		memcpy((BYTE *)&SampleCtrlReg, (BYTE *)SampleCtrlDefData, sizeof(TSampleCtrlReg) - sizeof(DWORD));

		//��ѹ�ݽ��ͣ����ѹ��40%����ֵ�����5%��������������
		SampleCtrlReg.SAGCFG.w[0] = (WORD)(((float)wStandardVoltageConst / 10.0) * (SampleCorr.UGain[0].d) / 4096 * 1.414 * 0.4);
		//����ʱ��10���ܲ�
		SampleCtrlReg.SAGCFG.w[1] = 10 * 2;

		//�����������޵�60%���� !!!!!!
		SampleCtrlReg.PSTART.d = (DWORD)(((float)dwMeterBasicCurrentConst / 1000.0) * ((float)wStandardVoltageConst / 10.0) * (float)(SampleCorr.PGain[0].d / 100) * 0.6 * 0.001 / 256.0);
		if (MeterTypesConst == METER_ZT)
		{
			//��ת��Ϊ100A��2��
			SampleCtrlReg.PSTART.d *= 2;
		}

		//��ѹ��ǣ���Ƶ��ֵ�����ѹ��30%��Ҫ��0.35��
		TempData = (WORD)(((float)wStandardVoltageConst / 10.0) * (SampleCorr.UGain[0].d) / 4096 * 0.3);
		SampleCtrlReg.ZXOTU.d = TempData; //��ѹ��ֵ

		//������ǣ��������0.1%
		TempData = (WORD)(((float)dwMeterBasicCurrentConst / 1000.0) * (float)(SampleCorr.IGain[0].d) * 0.001 / 4096.0);
		SampleCtrlReg.ZXOT.d = TempData; //������ֵ

		//SampleCtrlReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleCtrlReg,sizeof(TSampleCtrlReg)-sizeof(DWORD));
	}

	if ((Type == eADJUST) || (Type == eALL)) //TSampleAdjustPara
	{
		memcpy((BYTE *)&SampleAdjustReg[chipNo], (BYTE *)SampleAdjustDefData, sizeof(TSampleAdjustReg) - sizeof(DWORD));
		//SampleAdjustReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleAdjustReg,sizeof(TSampleAdjustReg)-sizeof(DWORD));
	}
}
//-----------------------------------------------
//��������: ram�б���ļ���оƬ�Ĵ���У��
//
//����:		eSampleType Type
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
static void CheckSampleRam(eSampleType Type, BYTE chipNo)
{
	BOOL Result;

	CheckSampleRunPara(); // У�����
	if ((Type == eCORR) || (Type == eALL))
	{
		if (SampleCorr.CRC32 != lib_CheckCRC32((BYTE *)&SampleCorr, sizeof(TSampleCorr) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCorr), sizeof(TSampleCorr), (BYTE *)&SampleCorr);
			if (Result != TRUE)
			{
				UseSampleDefaultPara(eCORR, chipNo);
			}
		}
	}
	// У��Ĳ���У��
	if ((Type == eADJUST) || (Type == eALL))
	{
		if (SampleAdjustReg[chipNo].CRC32 != lib_CheckCRC32((BYTE *)&SampleAdjustReg[chipNo], sizeof(TSampleAdjustReg) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg[chipNo]), sizeof(TSampleAdjustReg), (BYTE *)&SampleAdjustReg[chipNo]);
			if (Result != TRUE)
			{
				UseSampleDefaultPara(eADJUST, chipNo);
			}
		}
	}
	// ���Ʋ���У��
	if ((Type == eCRTL) || (Type == eALL))
	{
		if (SampleCtrlReg.CRC32 != lib_CheckCRC32((BYTE *)&SampleCtrlReg, sizeof(TSampleCtrlReg) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg), sizeof(TSampleCtrlReg), (BYTE *)&SampleCtrlReg);
			if (Result != TRUE)
			{
				UseSampleDefaultPara(eCRTL, chipNo);
			}
		}
	}
}

//--------------------------------------------------
//��������:  ���ü���оƬ����ģʽ
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void api_SetSampleRunPara(eIntegratorMode inmode, eMeasurementMode memode, BYTE chipNo)
{
	if (eNoneIntegrator != inmode)
	{
		samplerunpara.eintegratormode = inmode;
	}
	if (eNonePhase != memode)
	{
		samplerunpara.emeasurementmode = memode;
	}
	if (lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD)) != samplerunpara.CRC32) //���ݸı�
	{
		samplerunpara.CRC32 = lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD));
		UseSampleDefaultPara(eALL, chipNo);
		CheckSampleRam(eALL, chipNo);
		if (CheckSampleChipReg(chipNo))
		{
			GetSampleChipCheckSum(chipNo);
		}
	}
}
//--------------------------------------------------
//��������:  д��Ĵ������
//
//����:      Combop ����ָ��
//			ComboSize ��������
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
BOOL WriteSampleCombo(DWORD *Combop, BYTE ComboSize,BYTE chipNo)
{
	BYTE i;
	BOOL Rtresult = TRUE;

	for (i = 0; i < ComboSize; i += 3)
	{
		SampleSpecCmdOp(Combop[i], CmdWriteOpen, chipNo);
		Rtresult &= WriteSampleReg(Combop[i], (BYTE *)&Combop[i + 1], Combop[i + 2],chipNo);
		SampleSpecCmdOp(Combop[i], CmdWriteClose, chipNo);
	}
	return Rtresult;
}
//-----------------------------------------------
//��������: Ч�����оƬ�ļĴ���
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
BOOL CheckSampleChipReg(BYTE chipNo)
{
	BYTE i;
	BOOL Rtresult = TRUE, Result;
	BYTE lock[2] = {0xE5, 0xDC};
	TFourByteUnion ComData;
	DWORD TEMP;

	CLEAR_WATCH_DOG;
	// �����ж�
	if (api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE)
	{
		return FALSE;
	}
	SampleSpecCmdOp(SYSCFG, CmdWriteClose, chipNo); //�����Ĵ�����λ����
	// ���ƼĴ���
	for (i = 0; i < (sizeof(SampleCtrlAddr) / sizeof(SampleCtrlAddr[0])); i++)
	{
		// ע�⣡������
		// ����+����оƬ������Ҫÿ���ж��Ƿ���磬��ֹ�ڴ˵���ѵ�ص����Ĺ�
		// SOC�����ɲ�����ѹ
		if (api_CheckSysVol(eOnRunDetectPowerMode) == FALSE)
		{
			return FALSE;
		}

		CLEAR_WATCH_DOG;
		ComData.d = 0;
		Result = Link_ReadSampleReg(SampleCtrlAddr[i][0], ComData.b, SampleCtrlAddr[i][1], chipNo);
		if (Result != TRUE)
		{
			Rtresult &= Result;
			continue;
		}

		if (ComData.d != *(((DWORD *)&SampleCtrlReg.CFCFG + i)))
		{
			if (SampleCtrlAddr[i][0] == ZXOTCFG_EMU) //�üĴ���ĳЩλΪ����ֵ ĳЩλΪ��ȡֵ
			{
				if ((ComData.d & 0X0000001F) == *(((DWORD *)&SampleCtrlReg.CFCFG + i)))
				{
					continue;
				}
			}
			ComData.d = *((DWORD *)&SampleCtrlReg.CFCFG.d + i);
			SampleSpecCmdOp(SampleCtrlAddr[i][0], CmdWriteOpen, chipNo);
			Rtresult &= WriteSampleReg(SampleCtrlAddr[i][0], ComData.b, SampleCtrlAddr[i][1], chipNo);
			SampleSpecCmdOp(SampleCtrlAddr[i][0], CmdWriteClose, chipNo);
		}
	}
	if (samplerunpara.eintegratormode == eUnuseIntegrator) // ����������У��
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlUnuseIntegrator, sizeof(SampleCtrlUnuseIntegrator) / sizeof(DWORD), chipNo);
	}
	else
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlUseIntegrator, sizeof(SampleCtrlUseIntegrator) / sizeof(DWORD), chipNo);
	}
	if (samplerunpara.emeasurementmode == eOnePhase)
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlOnePhase, sizeof(SampleCtrlOnePhase) / sizeof(DWORD), chipNo);
	}
	else
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlThreePhase, sizeof(SampleCtrlThreePhase) / sizeof(DWORD), chipNo);
	}
	Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlTemp, sizeof(SampleCtrlTemp) / sizeof(DWORD), chipNo);
	Rtresult &= WriteSampleCombo((DWORD *)SampleCtrleAngle, sizeof(SampleCtrleAngle) / sizeof(DWORD), chipNo);

	// У׼�Ĵ���
	for (i = 0; i < (sizeof(SampleAdjustAddr) / sizeof(SampleAdjustAddr[0])); i++)
	{
		if (api_CheckSysVol(eOnRunDetectPowerMode) == FALSE)
		{
			return FALSE;
		}

		CLEAR_WATCH_DOG;
		ComData.d = 0;
		Result = Link_ReadSampleReg(SampleAdjustAddr[i][0], ComData.b, SampleAdjustAddr[i][1], chipNo);
		if (Result != TRUE)
		{
			Rtresult &= Result;
			continue;
		}

		if (ComData.d != *(((DWORD *)&SampleAdjustReg[chipNo].GP[0] + i)))
		{
			ComData.d = *((DWORD *)&SampleAdjustReg[chipNo].GP[0] + i);
			SampleSpecCmdOp(SampleAdjustAddr[i][0], CmdWriteOpen, chipNo);
			Rtresult &= WriteSampleReg(SampleAdjustAddr[i][0], ComData.b, SampleAdjustAddr[i][1], chipNo);
			SampleSpecCmdOp(SampleAdjustAddr[i][0], CmdWriteClose, chipNo);
		}
	}

//��չ�Ĵ���
#if (SEL_TOPOLOGY == YES)
	if (__get_PRIMASK() == 0) //ȫ���жϴ�
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD), chipNo);
	}
#endif

	return Rtresult;
}

//-----------------------------------------------
//��������: ��λ����оƬ
//
//����:		��
//
//����ֵ: 	��
//
//��ע:ֱ�ӿ��Ƹ�λ�ܽ�
//-----------------------------------------------
static void HardResetSampleChip(BYTE chipNo)
{
	//�ֲ�Ҫ��1ms
	//	RESET_SAMPLE_OPEN;
	//	api_Delayms(10);
	//	RESET_SAMPLE_CLOSE;
	ResetSPIDevice(eCOMPONENT_SPI_HSDC1 + chipNo, 200);
	CLEAR_WATCH_DOG;
}
//-----------------------------------------------
//��������: Ч�����оƬУ��Ĵ���
//
//����:		��
//
//����ֵ: 	��
//
//��ע:�ϵ�ʱ��û����Ч�飬�ϵ粻Ҫ���ô˺�����ÿ���ӵ�ʱ����ü���
//-----------------------------------------------
static void SampleChipMinVerification(BYTE chipNo)
{
	DWORD tdw1, tdw2, dwSum;
	BYTE Flag, i;

	//�����ж�
	if (api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE)
	{
		return;
	}

	Flag = 0;

	if (CheckSampleSumRegPro(eSumCheck, chipNo) != TRUE)
	{
		api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);
		Flag = 0x55;
	}
	//������������ѹΪ0������Ϊ��ʱ����оƬ�쳣��λ����Ҫ���³�ʼ��
	// if (bySamplePowerOnTimer <= 25)
	// {
	// 	for (i = 0; i < 3; i++)
	// 	{
	// 		if (RemoteValueArr[chipNo].U[i].d > 0)
	// 		{
	// 			break;
	// 		}
	// 	}
	// 	if (i == 3)
	// 	{
	// 		Flag = 0x55;

	// 		// Ӧ�ü�¼�澯�¼�
	// 		api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);
	// 	}
	// }

	if (Flag == 0x55)
	{
		if (byReWriteSampleChipRegCount[chipNo] < 12)
		{
			byReWriteSampleChipRegCount[chipNo]++;
			if (byReWriteSampleChipRegCount[chipNo] == 10)
			{
				//HardResetSampleChip();
				if (chipNo == 3)
				{
					api_SetError(ERR_CHECK_5460_4);
				}
				else
				{
					api_SetError(ERR_CHECK_5460_1 + chipNo);
				}
			}
		}
		ChipRecovery(chipNo);
	}
	else
	{
		byReWriteSampleChipRegCount[chipNo] = 0;
		RecoverNum[chipNo] = 0;
		if (chipNo == 3)
		{
			api_ClrError(ERR_CHECK_5460_4);
		}
		else
		{
			api_ClrError(ERR_CHECK_5460_1 + chipNo);
		}
	}
}
//-----------------------------------------------
//��������: ��ʼ������оƬ
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
BOOL api_InitSampleChip(void)
{
	return TRUE;
}
//--------------------------------------------------
//��������:  ��ȡ�¶�
//
//����:
//
//����ֵ:    SWORD һλС�� HEX  ����
//
//��ע:
//--------------------------------------------------
SWORD api_GetTemp(BYTE chipNo)
{
	SWORD Temp = 0;
	BYTE i = 0;
	DWORD buf = 0x00000001;

	WriteSampleCombo((DWORD *)SampleGetTemp, sizeof(SampleGetTemp) / sizeof(DWORD), chipNo);
	api_Delayms(1); //�ȴ�1ms
	while (i < 3)
	{
		if (Link_ReadSampleReg(0x00F7, (BYTE *)&buf, 4, chipNo))
		{
			if (buf & BIT10) //������Ч
			{
				if (buf & BIT9)
				{
					Temp = ((SWORD)(buf | 0xFFFFFE00)) * 10 / 4;
				}
				else
				{
					Temp = (buf - BIT10) * 10 / 4;
				}
				break;
			}
		}
		i++;
	}
	if (i == 3)
	{
		return 0;
	}
	return Temp - 15; // �¶�У׼-1.5���϶�
}
//-----------------------------------------------
//��������: �ϵ��ʼ������оƬ
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
void api_PowerUpSample(BYTE chipNo)
{
	DWORD dwSum;
	BYTE i;
	BOOL Result;

	CheckSampleRam(eALL, chipNo);

	//��ʼ������
	InitSampleCaliUI();
	VMAXLimit = wStandardVoltageConst * 220;				 //CalMaxLimiteValue( eVOLTAGE );
	IMINLimit = (DWORD)dwMeterBasicCurrentConst * 15 / 1000; //�������ޱ���һ��
	IMAXLimit = (DWORD)dwMeterMaxCurrentConst * 15;			 //1.5Imax
	bySamplePowerOnTimer = 30;
	dwSampleSFlag[chipNo] = 0;
	dwSamplePFlag[chipNo] = 0;
	byReWriteSampleChipRegCount[chipNo] = 0;
	Result = CheckSampleChipReg(chipNo);

	Result = GetSampleChipCheckSum(chipNo); //��У���

#if (SEL_STAT_V_RUN == YES) //�ָ���ѹ�ϸ���
	PowerUpVRunTime();
#endif

	//�ϵ������ѹ ���� ����
	//PowerUpReadVoltage();
	//byPowerOnFlag = 0;

#if (SEL_AHOUR_FUNC == YES)
	PowerUpAHour();
#endif
}

//-----------------------------------------------
//��������: ���紦�����оƬ
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
void api_PowerDownSample(void)
{
	memset((BYTE *)&RemoteValueArr, 0x00, sizeof(RemoteValueArr));
}

//-----------------------------------------------
//��������: ����оƬͨ�Ź��ϼ��
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
static void CheckEmuErr(BYTE chipNo)
{
	static BYTE EmuErrCnt[SAMPLE_CHIP_NUM] = {0};
	DWORD RegData = 0;

	//��ȡHFConst�ķ�ʽ�жϼ���оƬ����
	//����ʹ���ж�У��͵ķ�ʽ ��ΪSPIû��У�� ���SPI���߶̽ӵ�VCC����GND �������������ǹ̶���
	//У���Ҳ�ᱻˢ��ΪȫFF����ȫ00 ���¹����޷������
	Link_ReadSampleReg(HFConst_EMU, (BYTE *)&RegData, 2, chipNo);
	if (RegData != SampleCtrlReg.HFCONST.d)
	{
		if (EmuErrCnt[chipNo] < 12)
		{
			EmuErrCnt[chipNo]++;

			//�������5�γ���HFConstֵ����Ӧ ����У��һ��HFConst
			//��һ��ֹRAM�����ݳ��ִ��� �ڶ��������оƬ���ָ�λ ÿ����5��Ż���мĴ���У�� ������󱨵�����
			if (EmuErrCnt[chipNo] == 5)
			{
				ChipRecovery(chipNo);
			}

			// ����10�δ��� ��¼����оƬ����
			if (EmuErrCnt[chipNo] == 10)
			{
				api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);
				api_SetError(ERR_CHECK_5460_1 + chipNo); // �ü���оƬͨ�Ź���
			}
		}
	}
	else
	{
		//����ָ��� ����֮ǰ����������10�����ϵĴ��� ���¼��һ�����мĴ������� �ڲ���api_ClrError
		if (EmuErrCnt[chipNo] > 10)
		{
			SampleChipMinVerification(chipNo);
		}
		EmuErrCnt[chipNo] = 0;
	}
}

//-----------------------------------------------
//��������: ����оƬ������
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
static void Task_Sec_Sample(void)
{
	TRealTimer t;
	bySampleStep = 0;
	BYTE i = 0;
    BYTE res = 0;
	static DWORD LastWrongHalfWaveCount[SAMPLE_CHIP_NUM];

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t);
	if (t.Sec == (eTASK_SAMPLE_ID * 3 + 3 + 2)) //5s����е�ѹ�ϸ����ۼƣ�������ʱ�䲻Ϊ1440
	{
		//�ϵ�30s�����Ч�飬���⻹û��������оƬ��Ч���
		if (bySamplePowerOnTimer == 0)
		{
			for (i = 0; i < SAMPLE_CHIP_NUM; i++)
			{
				CheckSampleRam(eALL,i);
				SampleChipMinVerification(i);
			}
		}

#if (SEL_STAT_V_RUN == YES) // �����ѹ�ϸ��ʲ���
		StatVRunTime();
#endif
	}

	if (t.Sec == (eTASK_SAMPLE_ID * 3 + 3 + 2))
	{
		//ÿ���Ӽ���һ��ƽ������
		// api_CalPaverage[chipNo]();
		//ÿ�����ٳ�ʼ��һ�£�������ѹ����������������������ȫʧѹ�л���
		InitSampleCaliUI();
#if (SEL_AHOUR_FUNC == YES)
		if (t.Min == 26)
		{
			SwapAHour();
		}
#endif
	}

	if (bySamplePowerOnTimer)
	{
		bySamplePowerOnTimer--;
#if (SEL_TOPOLOGY == YES)
		if (bySamplePowerOnTimer == HSDC_OPEN_TIME)
		{
			for(i = 0;i< SAMPLE_CHIP_NUM;i++)
			{
				//��HSDC�ӿڱ�����ڴ�ȫ���ж�֮�󣬷���ᷢ��Ƿ��
				res = WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD), i);
				res = CheckSampleSumRegPro(eUpDateSumCheck, i); //����У���
			}
		}
#endif
	}
	//������޴���İ벨
	for (i = 0; i < SAMPLE_CHIP_NUM; i++)
	{
		if ((WaveDataDeal[i].WrongHalfWaveCount > 0) && (LastWrongHalfWaveCount[i] != WaveDataDeal[i].WrongHalfWaveCount))
		{
			LastWrongHalfWaveCount[i] = WaveDataDeal[i].WrongHalfWaveCount;
			if (WaveDataDeal[i].WriteCrcEventCnt >= WAVE_CRC_ERR_RECORD_CNT)
			{
				continue;
			}
			// ��¼����
			api_WriteSysUNMsg(SYSUN_WAVE_DATA1_CRC_ERR + i);
			WaveDataDeal[i].WriteCrcEventCnt++;
		}
	}
	if(SendDataLostFlag == 1)
	{
		SendDataLostFlag = 0;
		api_WriteSysUNMsg(SYSUN_SEND_WAVE_DATA_LOST);
	}
#if (SEL_STAT_V_RUN == YES)
	if (ClrVRunMonTimer != 0)
	{
		if (ClrVRunMonTimer > 30) //����ֵ�ж�
		{
			ClrVRunMonTimer = 3;
		}

		ClrVRunMonTimer--;
		if (ClrVRunMonTimer == 0)
		{
			//ÿ��1��0ʱ���µ�ѹ�ϸ���
			api_ClearVRunTimeNow(BIT1);
		}
	}

	if (ClrVRunDayTimer != 0)
	{
		if (ClrVRunDayTimer > 30) //����ֵ�ж�
		{
			ClrVRunDayTimer = 3;
		}

		ClrVRunDayTimer--;
		if (ClrVRunDayTimer == 0)
		{
			//ÿ��0ʱ���յ�ѹ�ϸ���
			api_ClearVRunTimeNow(BIT0);
		}
	}
#endif

#if (SEL_AHOUR_FUNC == YES)
	AccumulateAHour();
#endif
	for (i = 0; i < SAMPLE_CHIP_NUM; i++)
	{
		// ����оƬ���ϼ��
		CheckEmuErr(i);
	}
}
//-----------------------------------------------
//��������: ��ȡSAG״̬
//
//����:		��
//
//����ֵ: 	TRUE/FALSE
//
//��ע:
//-----------------------------------------------
BYTE api_GetSAGStatus(BYTE chipNo)
{
	TTwoByteUnion ComData;
	BOOL Result;

	if (Link_ReadSampleReg(EMMIF_EMU, ComData.b, 2, chipNo) == TRUE)
	{
		if ((ComData.w & 0x4000) == 0x0000)
		{
			return TRUE;
		}
	}

	ComData.w = 0x4000;
	SampleSpecCmdOp(EMMIF_EMU, CmdWriteOpen, chipNo);
	Result = WriteSampleReg(EMMIF_EMU, ComData.b, 2, chipNo);
	SampleSpecCmdOp(EMMIF_EMU, CmdWriteClose, chipNo);

	return FALSE;
}
//-----------------------------------------------
//��������: ���hsdc�Ƿ�����������������������
//
//����:		ChipNo ������оƬ�ţ���0��ʼ
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
void CheckHSDCTimer(TWaveDataDeal *pWaveData)
{
	TFourByteUnion RegData = {0};

	if (pWaveData->HSDCTimer >= 20)
	{
		if (pWaveData->TopoErrFlag < 20)
		{
			pWaveData->TopoErrFlag++;
		} 
		pWaveData->HSDCTimer = 0;
		IsRecWaveData &= ~(1<<(pWaveData->SampleChipNo));
		api_TopWaveSpiInit(pWaveData->SampleChipNo);
		Link_ReadSampleReg(HSDCCTL_EMU, RegData.b, 4, pWaveData->SampleChipNo);
		if((RegData.d & 0x00000001) == 0)
		{
			api_WriteSysUNMsg(HSDC1_ERR + pWaveData->SampleChipNo);
			//��HSDC�ӿڱ�����ڴ�ȫ���ж�֮�󣬷���ᷢ��Ƿ��
			WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD), pWaveData->SampleChipNo);
			CheckSampleSumRegPro(eUpDateSumCheck, pWaveData->SampleChipNo); //����У���
		}
	}
}
//-----------------------------------------------
//��������: ����оƬ��ѭ������
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
void api_SampleTask(void)
{
	TRealTimer t;
	BYTE i = 0;
    BYTE Result = FALSE;
    static WORD HourCnt = 0;
	
	for(i = 0;i < SAMPLE_CHIP_NUM;i++)
	{
		Read_UIP(i);
	}
	bySampleStep++;

	if (api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_10_MS) == TRUE)
	{
		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_10_MS);
#if (SEL_TOPOLOGY == YES)
		for(i = 0; i < SAMPLE_CHIP_NUM; i++)
		{
			CheckHSDCTimer(&WaveDataDeal[i]);
		}
#endif
	}

	if (api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_SECOND) == TRUE)
	{
		Task_Sec_Sample();
		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_SECOND);
	}

	if (api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_HOUR) == TRUE)
	{
		HourCnt++;
		if (HourCnt >= 24) // ÿСʱ����һ��
		{
			HourCnt = 0;
			// ÿСʱ����crcд�¼�����
			for (i = 0; i < SAMPLE_CHIP_NUM; i++)
			{
				WaveDataDeal[i].WriteCrcEventCnt = 0;
			}
		}

		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_HOUR);
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t);
#if (SEL_STAT_V_RUN == YES)
		if (t.Hour == 0)
		{
			// ��ʱ3���壬��ֹ�ڶ����ʱ������Ϊ0
			if (t.Day == 1)
			{
				ClrVRunMonTimer = 3;
			}

			ClrVRunDayTimer = 3;
		}
		else
		{
			// ÿСʱת���ѹ�ϸ���
			SwapVRunTime();
		}
#endif
	}
}

//-----------------------------------------------
//��������: ������������������жϵ���
//
//����:		��
//
//����ֵ: 	��
//
//��ע: ������������жϣ�������ö��Ĵ������˴�Ϊ�պ�����
//-----------------------------------------------
void api_AddSamplePulse(void)
{
}
//-----------------------------------------------
//��������: �͹���״̬��ȡ��ѹ����
//
//����:		Phase[in]:A B C
//          Type[in]: U I
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
DWORD api_LowPowerGetUI(eUIPType Type, BYTE Phase, BYTE chipNo)
{
	TFourByteUnion ComData;

	ComData.d = 0;

	//U��ѹ
	if (Type == eVOLTAGE)
	{
		if (Phase >= MAX_PHASE)
		{
			return 0;
		}
		if (Link_ReadSampleReg(UA_EMU + 4 * Phase, ComData.b, 4, chipNo) == TRUE)
		{
			ComData.d = (QWORD)ComData.d * 1000 / SampleCorr.UGain[Phase].d;
		}
	}
	else //����
	{
		if (Phase >= MAX_PHASE + 1)
		{
			return 0;
		}

		if (Link_ReadSampleReg(IA_EMU + 4 * Phase, ComData.b, 4, chipNo) == TRUE)
		{
			ComData.d = (DWORD)((double)ComData.d * 10000.0 / SampleCorr.IGain[Phase].d);
		}
	}

	return ComData.d;
}

//-----------------------------------------------
//��������: ��ȡ����оƬԶ������
//
//����:
//	Type[in]:
//		D15-D12 0:��				PHASE_ALL
//				1:A��				PHASE_A
//				2:B��				PHASE_B
//				3:C��				PHASE_C
//				4:N��				PHASE_N
//		D07-D00										��λ	�ֽڳ���	С����λ��
//				0:��ѹ				REMOTE_U		V			4			3
//				1:����				REMOTE_I		A 			4			4
//				2:��ѹ���			REMOTE_PHASEU	��			2			1
//				3:��ѹ�������		REMOTE_PHASEI	��			2			1
//				4:�й�����			REMOTE_P		kW			4			6
//				5:�޹�����			REMOTE_Q		kVAR		4			6
//				6:���ڹ���			REMOTE_S		kVA			4			6
//				7:һ����ƽ���й�����REMOTE_P_AVE	kW			4			6
//				8:һ����ƽ���޹�����REMOTE_Q_AVE	kVAR		4			6
//				9:һ����ƽ�����ڹ���REMOTE_S_AVE	kVA			4			6
//				A:��������			REMOTE_COS					4			4
//				B:��ѹ����ʧ����					%			2			2
//				C:��������ʧ����					%			2			2
//				D:��ѹг��������					%			2			2
//				E:����г��������					%			2			2
//				F:����Ƶ��			REMOTE_HZ		HZ			2			2
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//	Dot[in]			С��λ�� ��Ϊff��ΪĬ��С��λ��
//	Len[in]			���ݳ���
//  Buf[out] 		�������
//����ֵ: 	TRUE:��ȷ����ֵ		FALSE���쳣
//
//��ע:
//-----------------------------------------------
//						0	1	2	3	4	5	6	7	8	9	A	B		C		D		E	F	10
const BYTE MaxDot[] = {3, 4, 1, 1, 6, 6, 6, 6, 6, 6, 4, 2, 2, 2, 2, 2, 4};
const BYTE DefDot[] = {1, 3, 1, 1, 4, 4, 4, 4, 4, 4, 3, 2, 2, 2, 2, 2, 3};
const BYTE MaxLen[] = {4, 4, 2, 2, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 4};
const BYTE MinPhase[] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0xff, 0xff, 0, 0};
const BYTE MaxPhase[] = {3, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0xff, 0xff, 0, 0};

BOOL api_GetRemoteData(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *Buf)
{
	// TFourByteUnion td;
	// BYTE Phase;
	// BYTE ReadDot;
	// BYTE BackFlag;
	
	// if( Len > MaxLen[Type&0x001f] )
	// {
	// 	return FALSE;
	// }
	
	// Phase = (Type>>12);
	// if(api_GetMeasureMentMode() == eOnePhase)//����ģʽ�ܷ���A��
	// {
	// 	if(Phase == 0)
	// 	{
	// 		Phase = 1;
	// 	}
	// }

	// if( (Phase<MinPhase[Type&0x001f]) || (Phase>MaxPhase[Type&0x001f]) )
	// {
	// 	return FALSE;
	// }

	// td.d = 0;
	// switch( Type & 0x00ff )
	// {
	// 	case REMOTE_U:
	// 		memcpy(td.b,RemoteValue.U[Phase-1].b,4);
	// 		break;
	// 	case REMOTE_I:
	// 		if(MeterTypesConst == METER_3P3W )
	// 		{
	// 			//�������߱�֧�����ߵ���
	// 			if( Type == (PHASE_N+REMOTE_I) )
	// 			{
	// 				return FALSE;
	// 			}
	// 		}
	// 		td.d = RemoteValue.I[Phase-1].d;
	// 		break;		
	// 	case REMOTE_P:
	// 		td.d = RemoteValue.P[Phase].d;
	// 		break;
	// 	case REMOTE_Q:
	// 		td.d = RemoteValue.Q[Phase].d;
	// 		break;
	// 	case REMOTE_S:
	// 		memcpy(td.b,RemoteValue.S[Phase].b,4);
	// 		break;
	// 	case REMOTE_P_AVE:
	// 		memcpy(td.b,RemoteValue.Pav[0][Phase].b,4);
	// 		break;
	// 	case REMOTE_Q_AVE:
	// 		memcpy(td.b,RemoteValue.Pav[1][Phase].b,4);
	// 		break;
	// 	case REMOTE_S_AVE:
	// 		memcpy(td.b,RemoteValue.Pav[2][Phase].b,4);
	// 		break;
	// 	case REMOTE_COS:
	// 		td.d = RemoteValue.Cos[Phase].l;
	// 		break;
	// 	case REMOTE_HZ:
	// 		memcpy(td.b,RemoteValue.Freq.b,4);
	// 		break;
	// #if( SEL_VOL_ANGLE_FUNC == YES )
	// 	case REMOTE_PHASEU:
	// 		memcpy(td.b,RemoteValue.YUAngle[Phase-1].b,4);
	// 		break;
	// 	case REMOTE_PHASE:
	// 		td.l = RemoteValue.YIAngle[Phase-1].d - RemoteValue.YUAngle[Phase-1].d;
	// 		//��ѹ�����Ϊ0ʱ�������
	// 		if( (RemoteValue.U[Phase-1].d == 0) || (RemoteValue.I[Phase-1].d == 0) )
	// 		{
	// 			td.l = 0;
	// 		}
	// 		if( td.l < 0 )
	// 		{
	// 			td.l += 3600;
	// 		}
	// 		break;
	// #endif

	// 	default:
	// 		return FALSE;
	// }
	
	// if( Dot > MaxDot[Type&0x001f] )
	// {
	// 	ReadDot = MaxDot[Type&0x001f] - DefDot[Type&0x001f];
	// }
	// else
	// {
	// 	ReadDot = MaxDot[Type&0x001f] - Dot;
	// }
	
	// if( ReadDot <= MaxDot[Type&0x001f] )
	// {
	// 	td.l = td.l/(long)lib_MyPow10(ReadDot);
	// }
	// else
	// {
	// 	return FALSE;
	// }
	
	// if( (DataType==DATA_BCD) || (DataType==DATA_HEX) )
	// {
	// 	BackFlag = 0;
	// 	if( td.l < 0 )
	// 	{
	// 		td.l *= -1;
	// 		BackFlag = 1;
	// 	}
	// 	if( DataType==DATA_BCD )
	// 	{
	// 		td.d = lib_DWBinToBCD(td.d);
			
	// 		//�������λ�÷���
	// 		if( BackFlag == 1 )
	// 		{
	// 			td.b[Len - 1] |= 0x80;
	// 		}
	// 	}
	// }

	// if(api_GetMeasureMentMode() == eOnePhase)
	// {
	// 	if( (Phase == 2) || (Phase == 3) )
	// 	{
	// 		td.d = 0;
	// 	}
	// }
	
	// memcpy(Buf,td.b,Len);
	
	// return TRUE;
}
//�������汾
BOOL api_GetRemoteData2(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *Buf, TRemoteValue *pRemoteValue)
{
	TFourByteUnion td;
	BYTE Phase;
	BYTE ReadDot;
	BYTE BackFlag;

	if (Len > MaxLen[Type & 0x001f])
	{
		return FALSE;
	}

	Phase = (Type >> 12);
	if (api_GetMeasureMentMode() == eOnePhase) //����ģʽ�ܷ���A��
	{
		if (Phase == 0)
		{
			Phase = 1;
		}
	}

	if ((Phase < MinPhase[Type & 0x001f]) || (Phase > MaxPhase[Type & 0x001f]))
	{
		return FALSE;
	}

	td.d = 0;
	switch (Type & 0x00ff)
	{
	case REMOTE_U:
		memcpy(td.b, pRemoteValue->U[Phase - 1].b, 4);
		break;
	case REMOTE_I:
		if (MeterTypesConst == METER_3P3W)
		{
			//�������߱�֧�����ߵ���
			if (Type == (PHASE_N + REMOTE_I))
			{
				return FALSE;
			}
		}
		td.d = pRemoteValue->I[Phase - 1].d;
		break;
	case REMOTE_P:
		td.d = pRemoteValue->P[Phase].d;
		break;
	case REMOTE_Q:
		td.d = pRemoteValue->Q[Phase].d;
		break;
	case REMOTE_S:
		memcpy(td.b, pRemoteValue->S[Phase].b, 4);
		break;
	case REMOTE_P_AVE:
		memcpy(td.b, pRemoteValue->Pav[0][Phase].b, 4);
		break;
	case REMOTE_Q_AVE:
		memcpy(td.b, pRemoteValue->Pav[1][Phase].b, 4);
		break;
	case REMOTE_S_AVE:
		memcpy(td.b, pRemoteValue->Pav[2][Phase].b, 4);
		break;
	case REMOTE_COS:
		td.d = pRemoteValue->Cos[Phase].l;
		break;
	case REMOTE_HZ:
		memcpy(td.b, pRemoteValue->Freq.b, 4);
		break;
#if (SEL_VOL_ANGLE_FUNC == YES)
	case REMOTE_PHASEU:
		memcpy(td.b, pRemoteValue->YUAngle[Phase - 1].b, 4);
		break;
	case REMOTE_PHASE:
		td.l = pRemoteValue->YIAngle[Phase - 1].d - pRemoteValue->YUAngle[Phase - 1].d;
		//��ѹ�����Ϊ0ʱ�������
		if ((pRemoteValue->U[Phase - 1].d == 0) || (pRemoteValue->I[Phase - 1].d == 0))
		{
			td.l = 0;
		}
		if (td.l < 0)
		{
			td.l += 3600;
		}
		break;
#endif

	default:
		return FALSE;
	}

	if (Dot > MaxDot[Type & 0x001f])
	{
		ReadDot = MaxDot[Type & 0x001f] - DefDot[Type & 0x001f];
	}
	else
	{
		ReadDot = MaxDot[Type & 0x001f] - Dot;
	}

	if (ReadDot <= MaxDot[Type & 0x001f])
	{
		td.l = td.l / (long)lib_MyPow10(ReadDot);
	}
	else
	{
		return FALSE;
	}

	if ((DataType == DATA_BCD) || (DataType == DATA_HEX))
	{
		BackFlag = 0;
		if (td.l < 0)
		{
			td.l *= -1;
			BackFlag = 1;
		}
		if (DataType == DATA_BCD)
		{
			td.d = lib_DWBinToBCD(td.d);

			//�������λ�÷���
			if (BackFlag == 1)
			{
				td.b[Len - 1] |= 0x80;
			}
		}
	}

	if (api_GetMeasureMentMode() == eOnePhase)
	{
		if ((Phase == 2) || (Phase == 3))
		{
			td.d = 0;
		}
	}

	memcpy(Buf, td.b, Len);

	return TRUE;
}

//-----------------------------------------------
//��������: ��ȡ����оƬ����
//
//����:		Type[in]
//				0x00--��ѹ��ƽ����
//				0x01--������ƽ����
//				0x02--������
//				0x10--��ѹ״̬
//				0x11--����״̬
//				0x20--��Ǳ��״̬
//				0x3X--��ǰ���޹����� 0x30~0x33 �ֱ������/A/B/C���޹���������
//              0x40--��ǰ�����ڹ����Ƿ���ڶ���ʵ�ǧ��֮��
//����ֵ: 	0xffff--��֧�ִ�����
//				0x00--��ѹ��ƽ����	�������ͣ�long-unsigned����λ��%�����㣺-2
//              0x01--������ƽ����	�������ͣ�long-unsigned����λ��%�����㣺-2
//				0x02--������		�������ͣ�long-unsigned����λ��%�����㣺-2
//				0x10--��ѹ״̬		Bit15 1--��ѹ����״̬��bit0~2	1--A/B/C���ѹС��0.6Un
//				0x11--����״̬		Bit15 1--��������״̬��bit0~2	1--A/B/C�����С��5%Ib
//				0x20--��Ǳ��״̬	bit0~2	1--A/B/C�� 1--Ǳ��		0--��
//				0x3X--��ǰ���޹����� ����1~4������һ����~������
//              0x40--��ǰ�����ڹ����Ƿ���ڶ���ʵ�ǧ��֮��                 0--����	1--С��
//��ע:
//-----------------------------------------------
WORD api_GetSampleStatus(BYTE Type, BYTE chipNo)
{
	BYTE PQDirect;
	WORD i, tw;
	TFourByteUnion td;
	long tl;

	tw = 0;

	switch (Type)
	{
	case 0x00: //��ѹ��ƽ����
		tw = RemoteValueArr[chipNo].Uunbalance.w;
		break;
	case 0x01: //������ƽ����
		tw = RemoteValueArr[chipNo].Iunbalance.w;
		break;
	case 0x02: //������
		tl = labs(RemoteValueArr[chipNo].P[0].l);
		tw = tl / (wStandardVoltageConst / 10) / (dwMeterMaxCurrentConst) / 3;
		break;
	case 0x10:
		//��ѹ״̬	1--����0.6Un	0--����0.6Un
		for (i = 0; i < 3; i++)
		{
			if ((MeterTypesConst == METER_3P3W) && (i == 1))
			{
				continue;
			}

			if (RemoteValueArr[chipNo].U[i].d < (wStandardVoltageConst * 100 * 6 / 10))
			{
				tw |= (0x0001 << i);
			}
		}

		Link_ReadSampleReg(PHASES_EMU, td.b, 2, chipNo);

		if ((td.b[0] & 0x08) != 0)
		{
			tw |= 0x8000;
		}
		break;
	case 0x11:
		//����״̬	1--����5%Ib	0--����5%Ib
		for (i = 0; i < 3; i++)
		{
			if ((MeterTypesConst == METER_3P3W) && (i == 1))
			{
				continue;
			}

			if (labs(RemoteValueArr[chipNo].I[i].l) < ((DWORD)dwMeterBasicCurrentConst * 10 * 5 / 100))
			{
				tw |= (0x0001 << i);
			}
		}

		if (api_GetSysStatus(eSYS_STATUS_REVERSE_PHASE_I) == TRUE)
		{
			tw |= 0x8000;
		}

		break;
	case 0x20: //��Ǳ��״̬
		//bit0,bit1,bit2�ֱ����ABC����״̬ 0--�� 1--Ǳ��
		tw = (WORD)(dwSampleSFlag[chipNo] & 0x00000007);
		break;
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
		PQDirect = 0;
		tw = 0;
		for (i = 0; i < 4; i++)
		{
			//�й����ʵķ���
			if (api_GetSysStatus(eSYS_STATUS_OPPOSITE_P + i) == TRUE)
			{
				PQDirect |= (0x01 << i);
			}
		}
		for (i = 0; i < 4; i++)
		{
			//�޹����ʵķ���
			if (api_GetSysStatus(eSYS_STATUS_OPPOSITE_Q + i) == TRUE)
			{
				PQDirect |= (0x10 << i);
			}
		}

		//��λ���޹� ��λ���й� 0���� 1�Ǹ�
		PQDirect = (PQDirect >> (Type - 0x30)) & 0x11;
		switch (PQDirect)
		{
		case 0x01:
			tw = 2;
			break;
		case 0x10:
			tw = 4;
			break;
		case 0x11:
			tw = 3;
			break;
		default:
			tw = 1;
			break;
		}
		break;

	case 0x40:																								//���ڹ����ж�
		if (labs(RemoteValueArr[chipNo].S[0].l) >= (wStandardVoltageConst * dwMeterBasicCurrentConst * 4 * 3 / 10000)) //U*I*0.004*3
		{
			return 0;
		}
		else
		{
			return 1;
		}
		break;
	default:
		tw = 0xffff;
		break;
	}

	return tw;
}
//-----------------------------------------------
//��������: ��ȡ��������
//
//����:		bBuf[in]
//
//����ֵ: 	TRUE
//��ע:   bit0~bit1��ʾA�࣬bit2~bit3��ʾB�࣬bit4~bit5��ʾC�ࣻ
//  ��Ӧ�ֽ�λΪ00B��ʾ���ʣ��������ӵ��������û���01B��ʾ���ʣ����������û������������
//  11B��ʾ�����ز�֧�ֹ��ʷ����壬����ֵ��Ч��
//-----------------------------------------------
WORD api_GetSampleCurrMinus(BYTE *bBuf, BYTE chipNo)
{
	BYTE i, bTemp = 0;
	WORD Result;

	for (i = 0; i < MAX_PHASE; i++)
	{
		if (RemoteValueArr[chipNo].I[i].l < 0)
		{
			bTemp |= (0x01 << (2 * i));
		}
	}
	bBuf[0] = bTemp;

	return TRUE;
}

//-----------------------------------------------
//��������: ��ȡ������·��
//
//����:	��
//
//����ֵ: ��·��
//��ע:
//-----------------------------------------------
eMeasurementMode api_GetMeasureMentMode(void)
{
	return samplerunpara.emeasurementmode;
}

//-----------------------------------------------
//��������: ���ü�����·��
//
//����:	��
//
//����ֵ: �ɹ�/ʧ��
//��ע:
//-----------------------------------------------
BYTE api_SetSampleChip(BYTE bCount, BYTE chipNo)
{
	TSampleChipPara SampleChipPara;

	if ((bCount != 1) && (bCount != 3))
	{
		return FALSE;
	}
	memset(&SampleChipPara, 0x00, sizeof(TSampleChipPara));

	SampleChipPara.bCount = bCount;

	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleChipPara), sizeof(TSampleChipPara), (BYTE *)&SampleChipPara);

	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

	api_SetSampleRunPara(eNoneIntegrator, ((bCount == 1) ? eOnePhase : eThreePhase), chipNo);

	return TRUE;
}

//-----------------------------------------------
//��������: ��ʼ�������뾫���޹ز���
//
//����: ��
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_FactoryInitSample(BYTE chipNo)
{
	api_SetSampleChip(3, chipNo); //��ʼ��Ϊ����
}

#endif // #if( SAMPLE_CHIP == CHIP_RN8302B )
