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
//#include "statistics_voltage.h"

#if( SAMPLE_CHIP == CHIP_RN8302B )


//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eUpDateSumCheck,
	eSumCheck
}eCheckType;
#pragma pack(1)
typedef struct 
{
	eIntegratorMode eintegratormode;
	eMeasurementMode emeasurementmode;
	DWORD CRC32;
}SampleRunPara;
#pragma pack()
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
TRemoteValue					RemoteValue;				//�˶����� ��˲ʱ������
TSampleCorr						SampleCorr;				//˲ʱ��ϵ��
TSampleAdjustReg    			SampleAdjustReg;			//����оƬУ��������
TSampleCtrlReg      			SampleCtrlReg;			//����оƬ����������
TCurrenttValue					CurrentValue[9];			//�ֶβ����ͻ�����������
TSampleAdjustParaDiv 			SampleAdjustParaDiv;		//�ֶβ�������
TSampleManualModifyPara			SampleManualModifyPara;		//�ֶ�����ϵ��
TOriginalPara					OriginalPara;				//����Ĭ������ʹ�С���ϵ�� ���ĳ����δУ Ĭ�ϲ��ø�ϵ��
TEDTDataPara 			        EDTDataPara;		        //EDT��׼����
BYTE							byHarEnergyPulse;			//��������г���������
BYTE 							bySampleStep;				//��ѭ��ˢ��Զ������Э������ÿ���һ�Σ�ÿ���ڴ�ѭ�����ˢ�¸���Զ������
TExtremumMax 					ExtremumMax;
TExtremumMin 					ExtremumMin;
TExtremumMax 					ExtremumMaxHis;				//����0��ת������
TExtremumMin 					ExtremumMinHis;
TXtalConRom						XtalPara;
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
TPAverage		Paverage[3][4];					//ƽ������

DWORD 			VMAXLimit;						//��ѹ���ޱ���ֵ
DWORD 			IMINLimit;						//�������ޱ���ֵ
DWORD 			IMAXLimit;						//�������ޱ���ֵ
BYTE 			bySamplePowerOnTimer;			//�ϵ絹��ʱ������
BYTE 			byReWriteSampleChipRegCount;	//�ж�оƬЧ�鲻�Ի�д������

DWORD 			dwSamplePFlag;					//�й����޹�����
DWORD			dwSampleSFlag;					//Ǳ�����𶯱�־
BOOL 			UnBalanceGainFlag[3];			//��ѹ��ƽ�ⲹ����־ TRUE-�Ѳ��� FALSE-δ����
BYTE			TopoErrFlag = 0;
//�ֶβ����йز���
BYTE 			g_ucNowSampleDivisionNo[MAX_PHASE];		//�ֶβ�����ǰ����
BYTE 			ChecksumFlag;
static DWORD 	dwtempMaxU[3];
static DWORD 	dwtempMinU[3];
static DWORD 	dwtempMaxI[3];
static DWORD 	dwtempMinI[3];
static BYTE RecoverNum = 0;
static QWORD 	_success_cnt_iap[2] = {0, 0};
static QWORD 	_fail_cnt_iap[2] = {0, 0};
const DWORD bLock3[2]= {0x000000EA,0x00000000};
SampleRunPara samplerunpara={
	.eintegratormode=eUnuseIntegrator,
	.emeasurementmode=eThreePhase,
	.CRC32=0,
};
const DWORD SampleCtrlUnuseIntegrator[][3]=
{
	{ROS_CFG,0x00000000,4},
};
const DWORD SampleCtrlUseIntegrator[][3]=
{
	{ROS_DCATTC,0x00007fdf,4},
	{ROS_CFG,0x00000007,4},
};
const DWORD SampleCtrlOnePhase[][3]=
{
	{EMUCON_EMU,0x00001111,3},
	{SYSCFG,0xEA03BA36,4},
};
const DWORD SampleCtrlThreePhase[][3]=
{
	{EMUCON_EMU,0x00007777,3},
	{SYSCFG,0xEA03BA00,4},
};
const DWORD SampleCtrlTemp[][3]=
{
	{ECT_WREN,0x000000EA,4},
	{SAR_CTL0,0x8787CE10,4},
	{SAR_CTL1,0000000000,4},
	{SAR_EN,0000000001,1},
	{ECT_WREN,0x00000000,4},
};
const DWORD SampleCtrleAngle[][3]=
{
	{PHSUA_EMU,0xA7000200,4},
	{PHSUA_EMU,0xA7000200,4},
	{PHSUB_EMU,0x00000200,4},
	{PHSUC_EMU,0x00000200,4},
};


const DWORD SampleCtrlTopo[][3] =
	{
		{HSDCCTL_EMU, 0x00000004, 4},  // spi����
		{WSAVECON_EMU, 0x00000010, 1}, // ����������
		{SYSCFG_EMU, 0xEA00BA00, 4},   // ��������ȶ������
		{DMA_WAVECON3_EMU, 0x00000000, 4},
		{SOFTRST_EMU, 0x000000E1, 1},
		{SOFTRST_EMU, 0x0000004C, 1},
		{DMA_WAVECON_EMU, 0x00000000, 4},  // �ı䲨�β�����
		{WSAVECON_EMU, 0x00000080, 1},	   // ͬ������ 128�㣬BUFCFG[3��0] = 0�����λ������ã�����abc����
		{HSDCCTL_EMU, 0x0000021D, 4},	   // spi���ã�bit9��У�鷽ʽ���°�оƬ�ֲ�����
		{DMA_WAVECON3_EMU, 0x00000001, 4}, // �����������棬BUFCFG[4] = 0
		{DMA_WAVECON2_EMU, 0x00000200, 4}, // bit9ʹ�������Զ����ڣ�bit10ʹ��г��˥��������
// bit8 ͨ��ֱ�� OFFSET У���Ĵ��� DCOSx(1AH~20H)���� DMA ͬ������ͨ����ֱ�� OFFSET У��
};


const DWORD SampleGetTemp[][3] =
	{
		{ECT_WREN, 0x000000EA, 4},
		{SAR_CTL1, 0000000001, 4},
		{ECT_WREN, 0x00000000, 4},
};
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void HardResetSampleChip(void);
static void CheckSampleRam(eSampleType Type);
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
BOOL  IsCrystalErr( void )
{
	DWORD Testaddrmsg[4];
	MEM_ZERO_ARRAY(Testaddrmsg);
	Link_ReadSampleReg(SYSSR, (BYTE*)&Testaddrmsg[0], 2);

	if((Testaddrmsg[0] >> 14) == 1)
	{
		Link_ReadSampleReg(CFCFG_EMU, (BYTE*)&Testaddrmsg[1], 3);
		Link_ReadSampleReg(EMUCFG_EMU, (BYTE*)&Testaddrmsg[2], 3);
		Link_ReadSampleReg(EMUCON_EMU, (BYTE*)&Testaddrmsg[3], 3);
		if ((Testaddrmsg[1]==Testaddrmsg[2])&&(Testaddrmsg[1]==Testaddrmsg[3]))
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
void  ChipRecovery( void )
{
	RecoverNum++;
	if(RecoverNum >= 10)
	{
		RecoverNum = 10;
		HardResetSampleChip();
	}
	UseSampleDefaultPara(eALL);
	CheckSampleRam(eALL);
	if(CheckSampleChipReg())
	{
		GetSampleChipCheckSum();
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
BYTE  GetCountFormEE( void )
{
	TSampleChipPara SampleChipPara;

	if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleChipPara),sizeof(TSampleChipPara),(BYTE*)&SampleChipPara) != TRUE)
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
void  CheckSampleRunPara( void )
{
	if(lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD)) != samplerunpara.CRC32)
	{
		samplerunpara.eintegratormode= eUnuseIntegrator;
		samplerunpara.emeasurementmode=eThreePhase;
		samplerunpara.CRC32=lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD));
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
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,DWORD Addr, BYTE *Buf)
{
	BYTE i,DivCurrentNo, Result;
	DWORD *p;
	short QGainTmp,PGainTmp;

	for(i=0;i<(sizeof(SampleCtrlAddr)/sizeof(SampleCtrlAddr[0]));i++)
	{
		if(Addr == SampleCtrlAddr[i][0])
		{
			p = (DWORD*)&SampleCtrlReg;
			memcpy((void*)&p[i],Buf,sizeof(DWORD));
			
			if( (WriteEepromFlag&0x01) == 0x00 )
			{
				SampleCtrlReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleCtrlReg,sizeof(TSampleCtrlReg)-sizeof(DWORD));				
			}
			else
			{
				//��дeeУ��ϵ������
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg), sizeof(TSampleCtrlReg), (BYTE*)&SampleCtrlReg);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}
			return TRUE;
		}
	}
	
	for(i=0;i<(sizeof(SampleAdjustAddr)/sizeof(SampleAdjustAddr[0]));i++)
	{
		if(Addr == SampleAdjustAddr[i][0])
		{
			p = (DWORD *)&SampleAdjustReg;
			memcpy((void*)&p[i],Buf,sizeof(DWORD));
			
			//ֻ����RAM�ͼ���оƬ ��дEE
			if( (WriteEepromFlag&0x02) == 0x00 )
			{
				SampleAdjustReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleAdjustReg,sizeof(TSampleAdjustReg)-sizeof(DWORD));				
			}
			else
			{
				//��дeeУ��ϵ������
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg), sizeof(TSampleAdjustReg), (BYTE*)&SampleAdjustReg);
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
BOOL CheckSampleSumRegPro(eCheckType echecktype)
{
	static DWORD dwSumChecks[5] = {ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F};
	DWORD dwSumChecksTemp = 0;
	DWORD dwSumChecksTemp2 = 0;
	BYTE i = 0, j = 0;
	WORD addrs[5] = {CKSUM_EMU1, CKSUM_EMU2, CKSUM_EMU3, CKSUM_EMU4, SYSCFG};
	BYTE lens[5] = {3, 2, 4, 4, 4};

	if(IsCrystalErr())
	{
		return FALSE;
	}

	if(echecktype == eUpDateSumCheck)
	{
		for(i = 0; i < (sizeof(dwSumChecks) / sizeof(dwSumChecks[0])); i++)
		{
			for(j = 0; j < 3; j++)
			{
				dwSumChecksTemp = 0;
				dwSumChecksTemp2 = 0;
				if(Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp, lens[i]) == TRUE)
				{
					if(Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp2, lens[i]) == TRUE)
					{
						if(dwSumChecksTemp == dwSumChecksTemp2)
						{
							dwSumChecks[i] = dwSumChecksTemp;
							break;
						}
					}
				}
			}
		}
	}
	else if(echecktype == eSumCheck)
	{
		for(i = 0; i < (sizeof(dwSumChecks) / sizeof(dwSumChecks[0])); i++)
		{
			dwSumChecksTemp = 0;
			if(Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp, lens[i]) == TRUE)
			{
				if(dwSumChecks[i] != dwSumChecksTemp)
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
void GetSampleChipCheckSum(void)
{
	CheckSampleSumRegPro(eUpDateSumCheck);
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
void api_CalPaverage(void)
{
	BYTE i,j;
	static BYTE PowerUPFlag = 0X55;//�ϵ��һ��ƽ������Ϊ0

	for(i=0;i<3;i++)
	{
		for(j=0;j<4;j++)
		{
			if(( (MeterTypesConst==METER_3P3W) && (j==2) )//3P3W��B��
				||( Paverage[i][j].Cnt == 0 )//����ֵΪ0
				||( PowerUPFlag == 0X55 ))//�ϵ��һ�μ���
			{
				//���ϴμ�����
				continue;
			}
			else
			{
				RemoteValue.Pav[i][j].l = (long)(Paverage[i][j].SumValue/(float)Paverage[i][j].Cnt);
			}
		}
	}

	memset( (BYTE *)(Paverage), 0 ,sizeof(Paverage));
	
	PowerUPFlag = 0;
}

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
	DWORD dwMax,dwMin;
	DWORD dwData[3];
	WORD Result;
	
	dwData[0] = labs(Buf[0]);
	if( MeterTypesConst == METER_3P3W )
	{
		//������ʱ��dwData[1]��A��ֵ��ͬ
		dwData[1] = dwData[0];
	}
	else
	{
		dwData[1] = labs(Buf[1]);
	}	
	dwData[2] = labs(Buf[2]);
	
	if( Type == eVOLTAGE )
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
	if( (dwData[0]<dwMin) && (dwData[1]<dwMin) && (dwData[2]<dwMin) )
	{
		return 0;
	}
	
	dwMax = dwData[0];
	dwMin = dwData[0];
	
	//�������ֵ
	if( dwMax < dwData[1] )
	{
		dwMax = dwData[1];
	}	
	if( dwMax < dwData[2] )
	{
		dwMax = dwData[2];
	}
	
	//������Сֵ
	if( dwMin > dwData[1] )
	{
		dwMin = dwData[1];
	}		
	if( dwMin > dwData[2] )
	{
		dwMin = dwData[2];
	}
	
	if( dwMax == 0 )
	{
		Result = 0;
	}
	else
	{
		Result = (WORD)((long long)(dwMax-dwMin)*10000/dwMax);
	}
	
	if( Result > 10000 )
	{
		Result = 10000;
	}
	
	return Result;
}

//-----------------------------------------------
//��������: ˢ��˲ʱ�����ݲ�ˢ�±�־U, I[0], P, I[1], F, S, Cos, Pav
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void Read_UIP(void)	
{
    BYTE i,DataSign;
	BYTE CurRatio;
	BYTE bySamplePulseNum;	
	DWORD tdw,TotalEnergy1,TotalEnergy2,RatioEnergy1,RatioEnergy2;
	DWORD ILimit,VoltageLimit,VMINLimit;
	long tl;
	float tf;
    BYTE Result,Cmd,Count;
	TFourByteUnion ComData; //������
    
	if( bySampleStep == 0xff )
	{
		return;
	}
	Cmd = bySampleStep;
	ComData.d = 0;
    bySamplePulseNum = 0;
	
	switch(Cmd)
	{
		case 0x00://Ǳ���𶯱�־
			Result = Link_ReadSampleReg(Status_EMU,ComData.b,2);
			if(Result == FALSE)
			{
				break; //�����ȡ����оʧ�ܣ�ʹ����һ������
			}
			//��־
			dwSampleSFlag = ComData.d;		
			//dwSamplePFlag = ComData.d;		
        break;
			//��ѹ	
		case UA_EMU:
		case UB_EMU:
		case UC_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			
			tdw = (DWORD)((QWORD)ComData.d*1000/SampleCorr.UGain[Cmd - UA_EMU].d);
			VMINLimit = wStandardVoltageConst * 4; //4%Un
			if(tdw > VMAXLimit) 
			{
				tdw = VMAXLimit;
			}
			if(tdw < VMINLimit)
			{
				tdw = 0;
			}
			RemoteValue.U[Cmd - UA_EMU].d = tdw;

			//��ѹ��ƽ����
			if( Cmd == 3 )
			{
				RemoteValue.Uunbalance.w = CaleUnbalanceRate( eVOLTAGE, (long*)&RemoteValue.U[0].l );
			}
			break;
		case PA_EMU:
		case PB_EMU:
		case PC_EMU:
			//P�й����ʣ�6λС��,��λKW
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			tl = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[Cmd-PA_EMU+1].d);
			//�ж��Ƿ������������ͬʱ���ʷ���λΪ1
			if((dwSampleSFlag&(P_CREEP<<(Cmd - PA_EMU)))!=0)
			{
				tl = 0;
			}
			RemoteValue.P[Cmd-PA_EMU+1].l = tl;

			if( tl < 0 )
			{
				//���÷��෴���־
				api_SetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - PA_EMU);
			}
			else if( tl > 0 )
			{
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - PA_EMU);
			}

			Paverage[0][Cmd-PA_EMU+1].SumValue += (float)RemoteValue.P[Cmd-PA_EMU+1].l;
			Paverage[0][Cmd-PA_EMU+1].Cnt++;
			
			break;
		case PT_EMU:
			Result = Link_ReadSampleReg( (WORD)Cmd,ComData.b,4);
			tl = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[0].d);
			if( ( (MeterTypesConst==METER_3P3W) && ((dwSampleSFlag&0x00000005)==0x00000005) )
			||	( (MeterTypesConst!=METER_3P3W) && ((dwSampleSFlag&0x00000007)==0x00000007) ) )
			{
				tl = 0;
			}
			RemoteValue.P[0].l = tl;	

			if( tl < 0 )
			{
				//���ú��෴���־
				api_SetSysStatus(eSYS_STATUS_OPPOSITE_P);
			}
			else if( tl > 0 )
			{
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P);
			}
			
			Paverage[0][0].SumValue += (float)RemoteValue.P[0].l;
			Paverage[0][0].Cnt++;

			#if( SEL_DEMAND_2022 == YES )
			#if( MAX_PHASE != 1 )
			if(tl != 0)
			{
				tl = 1;//��ֹ�ض�
			}
		    api_UptCurDemandStatus( 0, tl);//���µ�ǰ�й�����״̬,���ڸ��±�־֮��
			#endif
			#endif
			
			break;
			
		case QA_EMU:
		case QB_EMU:
		case QC_EMU:
			Result = Link_ReadSampleReg( (WORD)Cmd,ComData.b,4);
			tl = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[Cmd - QA_EMU + 1].d);
			//�ж��Ƿ������������ͬʱ���ʷ���λΪ1
			if((dwSampleSFlag&(Q_CREEP<<(Cmd - QA_EMU)))!=0)
			{
				tl = 0;
			}
			RemoteValue.Q[Cmd-QA_EMU+1].l = tl;

			if( tl < 0 )
			{
				//���÷��෴���־
				api_SetSysStatus(eSYS_STATUS_OPPOSITE_Q_A + Cmd - QA_EMU);
			}
			else if( tl > 0 )
			{
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_Q_A + Cmd - QA_EMU);
			}

			Paverage[1][Cmd - QA_EMU + 1].SumValue += (float)RemoteValue.Q[Cmd - QA_EMU + 1].l;
			Paverage[1][Cmd - QA_EMU + 1].Cnt++;
			break;
			
		case QT_EMU:
			Result = Link_ReadSampleReg( (WORD)Cmd,ComData.b,4);
			tl = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[0].d);
			if( ( (MeterTypesConst==METER_3P3W) && ((dwSampleSFlag&0x00000050)==0x00000050) )
			||	( (MeterTypesConst!=METER_3P3W) && ((dwSampleSFlag&0x00000070)==0x00000070) ) )
			{
				tl = 0;
			}
			RemoteValue.Q[0].l = tl;

			if( tl < 0 )
			{
				//���÷��෴���־
				api_SetSysStatus(eSYS_STATUS_OPPOSITE_Q);
			}
			else if( tl > 0 )
			{
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_Q);
			}
			#if( SEL_DEMAND_2022 == YES )
			#if( MAX_PHASE != 1 )
			if(tl != 0)
			{
				tl = 1;//��ֹ�ض�
			}
			api_UptCurDemandStatus( 1, tl);//���µ�ǰ�й�����״̬,���ڸ��±�־֮��
			#endif
			#endif		
			Paverage[1][0].SumValue += (float)RemoteValue.Q[0].l;
			Paverage[1][0].Cnt++;
			break;
			
		//S���ڹ��ʣ�6λС��,��λKW			
		case SA_EMU:
		case SB_EMU:
		case SC_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			RemoteValue.S[Cmd - SA_EMU + 1].l = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[Cmd - SA_EMU + 1].d);

			if((RemoteValue.P[Cmd - SA_EMU + 1].l == 0)&&(RemoteValue.Q[Cmd - SA_EMU + 1].l == 0))
			{
				RemoteValue.S[Cmd - SA_EMU + 1].l = 0;
			}
			if(RemoteValue.P[Cmd - SA_EMU + 1].l < 0)
			{
				RemoteValue.S[Cmd - SA_EMU + 1].l *= -1;
			}
			Paverage[2][Cmd - SA_EMU + 1].SumValue += (float)RemoteValue.S[Cmd - SA_EMU + 1].l;
			Paverage[2][Cmd - SA_EMU + 1].Cnt++;
			break;
			
		case STA_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			RemoteValue.S[0].l = (long)((double)ComData.l*1000.0/(double)SampleCorr.PGain[0].d*100);
			if((RemoteValue.P[0].l == 0)&&(RemoteValue.Q[0].l == 0))
			{
				RemoteValue.S[0].l = 0;
			}
			if(RemoteValue.P[0].l < 0)
			{
				RemoteValue.S[0].l *= -1;
			}

			Paverage[2][0].SumValue += (float)RemoteValue.S[0].l;
			Paverage[2][0].Cnt++;
			break;
			
		//������4λС��,��λA
		//Ӧ���ȶ������ٶ����� ��Ϊ�����ķ�����Ҫ���ݹ����ж�
		case IA_EMU:
		case IB_EMU:
		case IC_EMU:
		case IN_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			RemoteValue.I[Cmd - IA_EMU].l = (long)((double)ComData.d*10000.0/SampleCorr.IGain[Cmd - IA_EMU].d);
			if((RemoteValue.I[Cmd - IA_EMU].l > IMAXLimit)||(RemoteValue.I[Cmd - IA_EMU].l < IMINLimit)) 
			{
				RemoteValue.I[Cmd - IA_EMU].l = 0;
			}
			if( Cmd == IN_EMU )//���ߵ���
			{
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P) == TRUE )
				{
					RemoteValue.I[3].l *= -1;
				}
			}
			else
			{
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - IA_EMU) == TRUE )
				{
					RemoteValue.I[Cmd - IA_EMU].l *= -1;
				}
				//������ƽ����
				if( Cmd == 0x12 )
				{
					RemoteValue.Iunbalance.w = CaleUnbalanceRate( eCURRENT, (long*)&RemoteValue.I[0].l );
				}
			}
		break;
		
		case IN2_EMU:	//����
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			RemoteValue.I[4].l = (long)((double)ComData.d*10000.0/SampleCorr.IGain[0].d);
			if((RemoteValue.I[4].l > IMAXLimit)||(RemoteValue.I[4].l < IMINLimit)) 
			{
				RemoteValue.I[4].l = 0;
			}
			
			RemoteValue.I[4].l *= 4;
		break;
		
		case PfA_EMU:
			//Cos��������,4λС��	
			//���������Ĵ�������ʳ�����ʱʹ�ù���ֵ���㹦������   δ������������
			for( i = 0; i<4; i++ )
			{
				if(RemoteValue.S[i].d)
				{
					RemoteValue.Cos[i].l = (long)(RemoteValue.P[i].l*10000.0/RemoteValue.S[i].l);
					if(labs(RemoteValue.Cos[i].l) > 10000)	
					{
						RemoteValue.Cos[i].l = 10000;
					}
				}
				else 
				{
					RemoteValue.Cos[i].l = 10000;
				}
				
				if((RemoteValue.Q[i].l == 0)&&(RemoteValue.P[i].l == 0 ))
				{
					RemoteValue.Cos[i].l = 10000;
				}

				if(i == 0)
				{
					if((RemoteValue.I[0].l == 0)&&(RemoteValue.I[1].l == 0)&&(RemoteValue.I[2].l == 0))
					{
						RemoteValue.Cos[i].l = 10000;
					}
				}
				else
				{
					if(RemoteValue.I[i-1].l == 0)
					{
						RemoteValue.Cos[i].l = 10000;
					}
				}
				
				if(RemoteValue.P[i].l < 0)
				{
					RemoteValue.Cos[i].l *= -1;
				}
			}
			break;
		//Ƶ��
		case Freq_U_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,3);
			RemoteValue.Freq.l = 6553600000/ComData.d;	
			break;

		//��ѹ���
		case YUA_EMU:
		case YUB_EMU:
		case YUC_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,3);
			RemoteValue.YUAngle[Cmd - YUA_EMU].d = (DWORD)((double)ComData.d*100*360/16777216/10);
			break;
		//�������
		case YIA_EMU:
		case YIB_EMU:
		case YIC_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,3);
			RemoteValue.YIAngle[Cmd - YIA_EMU].d = (DWORD)((double)ComData.d*100*360/16777216/10);

			if(( Cmd == YIC_EMU ) && ((dwSampleSFlag%0x00000008)==0x00000000))
			{
				Count = 0;
				
				if( RemoteValue.YIAngle[1].d > RemoteValue.YIAngle[0].d )
				{
					Count++;
				}
				if( RemoteValue.YIAngle[2].d > RemoteValue.YIAngle[1].d )
				{
					Count++;
				}
				if( RemoteValue.YIAngle[2].d > RemoteValue.YIAngle[0].d )
				{
					Count++;
				}
				api_ClrSysStatus( eSYS_STATUS_REVERSE_PHASE_I );
				if((Count == 0)||(Count == 2))
				{
					api_SetSysStatus( eSYS_STATUS_REVERSE_PHASE_I );
				}
				
			}
			
			break;
		// �й����ܣ��������
	    case EPT_EMU:   // ���й�����
		#if(SEL_SEPARATE_ENERGY == YES)
	    case EPA_EMU:   // A���й��ܵ���
	    case EPB_EMU:   // B���й��ܵ���
	    case EPC_EMU:   // C���й��ܵ���
		#endif// #if( SEL_SEPARATE_ENERGY == YES )
		    Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 3);
		    if(ComData.d > 0xff)
		    {
			    Result = FALSE;
		    }
		    if((ComData.d != 0) && (Result == TRUE))
		    {
			    if(Cmd == EPT_EMU)
			    {
				    CurRatio = api_GetCurRatio();
				    if(CurRatio > MAX_RATIO)
				    {
					    CurRatio = DefCurrRatioConst;
				    }
				    // �й�����δ�����𶯹��ʣ����ܷ������빦�ʷ���
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P) == TRUE)
				    {
					    api_WritePulseEnergy(PHASE_ALL + N_ACTIVE, ComData.b[0]);   // ����
				    }
				    else
				    {
					    api_WritePulseEnergy(PHASE_ALL + P_ACTIVE, ComData.b[0]);   // ����
				    }
			    }
			    else
			    {
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - EPA_EMU) == TRUE)
				    {
					    api_WritePulseEnergy((Cmd - EPA_EMU + 1) * PHASE_A + N_ACTIVE, ComData.b[0]);
				    }
				    else
				    {
					    api_WritePulseEnergy((Cmd - EPA_EMU + 1) * PHASE_A + P_ACTIVE, ComData.b[0]);
				    }
			    }
		    }
		    if(Cmd == EPT_EMU)
		    {
				// ���������ۼ��õĵ��й�������������������������ʱ������
				// ע����ʹ����������Ϊ0��ҲҪ���£���Ϊ��������ʱ���������
				#if(SEL_DEMAND_2022 == YES)
				#if(MAX_PHASE != 1)
				if(Result == FALSE)
				{
					ComData.d = 0;
				}
				api_UpdateDemandEnergyPulse(0, ComData.b[0]);   // ʹ�ø�Ƶ����
				#endif
				#endif
		    }
		    break;
		// �޹����ܣ��������
		#if(SEL_SEPARATE_ENERGY == YES)
	    case EQA_EMU:
	    case EQB_EMU:
	    case EQC_EMU:
		#endif
	    case EQT_EMU:
		    Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 3);
		    if(ComData.d > 0xff)
		    {
			    Result = FALSE;
		    }
		    if((ComData.d != 0) && (Result == TRUE))
		    {
			    if(Cmd == EQT_EMU)
			    {
				    // ���ݹ��ʷ����ж��޹���������
				    DataSign = 0x00;
				    // �й�Ϊ������һ������
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P) == TRUE)
				    {
					    DataSign |= 0x08;
				    }
				    // �޹�Ϊ������һ������
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_Q) == TRUE)
				    {
					    DataSign |= 0x80;
				    }
				    switch(DataSign)
				    {
				    case 0x00:   // һ����
					    api_WritePulseEnergy(PHASE_ALL + RACTIVE1, ComData.b[0]);
					    break;
				    case 0x08:   // ������
					    api_WritePulseEnergy(PHASE_ALL + RACTIVE2, ComData.b[0]);
					    break;
				    case 0x88:   // ������
					    api_WritePulseEnergy(PHASE_ALL + RACTIVE3, ComData.b[0]);
					    break;
				    case 0x80:   // ������
					    api_WritePulseEnergy(PHASE_ALL + RACTIVE4, ComData.b[0]);
					    break;
				    default:
					    break;
				    }
			    }
			    else
			    {
				    // ���ݹ��ʷ����ж��޹���������
				    DataSign = 0x00;
				    // �й�Ϊ������һ������
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - EQA_EMU) == TRUE)
				    {
					    DataSign |= 0x01;
				    }
				    // �޹�Ϊ������һ������
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_Q_A + Cmd - EQA_EMU) == TRUE)
				    {
					    DataSign |= 0x10;
				    }
				    switch(DataSign)
				    {
				    case 0x00:   // һ����
					    api_WritePulseEnergy((Cmd - EQA_EMU + 1) * PHASE_A + RACTIVE1, ComData.b[0]);
					    break;
				    case 0x01:   // ������
					    api_WritePulseEnergy((Cmd - EQA_EMU + 1) * PHASE_A + RACTIVE2, ComData.b[0]);
					    break;
				    case 0x11:   // ������
					    api_WritePulseEnergy((Cmd - EQA_EMU + 1) * PHASE_A + RACTIVE3, ComData.b[0]);
					    break;
				    case 0x10:   // ������
					    api_WritePulseEnergy((Cmd - EQA_EMU + 1) * PHASE_A + RACTIVE4, ComData.b[0]);
					    break;
				    default:
					    break;
				    }
				    break;
			    }
		    }
		    if(Cmd == EQT_EMU)
		    {
				// ���������ۼ��õĵ��й�������������������������ʱ������
				// ע����ʹ����������Ϊ0��ҲҪ���£���Ϊ��������ʱ���������
				#if(SEL_DEMAND_2022 == YES)
				#if(MAX_PHASE != 1)
				if(Result == FALSE)
				{
					ComData.d = 0;
				}
				api_UpdateDemandEnergyPulse(1, ComData.b[0]);   // ʹ�ø�Ƶ����
				#endif
				#endif
		    }
		    break;
		//���ڵ��ܣ��������
		case ESA_EMU:
		case ESB_EMU:	
		case ESC_EMU:
		case ESTA_EMU:			
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,3);
			if(ComData.d > 0xff)
		    {
			    Result = FALSE;
		    }
			
			if((ComData.d != 0)&&(Result == TRUE))
			{
				if( Cmd == ESTA_EMU )
				{
					if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE)
					{
						api_WritePulseEnergy(PHASE_ALL+APPARENT_N,ComData.b[0]);
					}
					else
					{
						api_WritePulseEnergy(PHASE_ALL+APPARENT_P,ComData.b[0]);
					}
				}
				else
				{
					if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_A+Cmd-ESA_EMU ) == TRUE)
					{
						api_WritePulseEnergy((Cmd - ESA_EMU + 1)*PHASE_A+APPARENT_N,ComData.b[0]);
					}
					else
					{
						api_WritePulseEnergy((Cmd - ESA_EMU + 1)*PHASE_A+APPARENT_P,ComData.b[0]);
					}
				}
			}
			break;
		default:
			break;	

	}
	bySampleStep++;

	if( bySampleStep >= 0x60 )//@@@@@��ʱ�Ȳ��ģ�����ȷ��
	{
		bySampleStep = 0xff;
	}	

/*	//�Զ��������Ĵ�����г�����ܣ��������
	Result = Link_ReadSampleReg(D2FE01_EMU,ComData.b);
	if((ComData.d != 0)&&(Result == TRUE))
	{	
		api_WritePulseEnergy(PHASE_ALL+HARM_WAVE+P_ACTIVE,ComData.b[0]);// ����г������
	}
	Result = Link_ReadSampleReg(D2FE00_EMU,ComData.b);
	if((ComData.d != 0)&&(Result == TRUE))
	{												
		api_WritePulseEnergy(PHASE_ALL+HARM_WAVE+N_ACTIVE,ComData.b[0]);// ����г������					
	}
*/
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
void UseSampleDefaultPara( eSampleType Type )
{
	BYTE i;
    WORD TempData;
	
	InitSampleCaliUI();
	if(( Type == eCORR )||( Type == eALL ))
	{
		for(i=0; i<(sizeof(SampleUIPGain)/sizeof(DWORD)); i++)
		{
			if(i<3)
			{
				SampleCorr.UGain[i].d = SampleUIPGain[i];
			}
			else if(i<7)
			{
				SampleCorr.IGain[i-3].d = SampleUIPGain[i];
			}
			else if(i<11)
			{
				SampleCorr.PGain[i-7].d = SampleUIPGain[i];
			}
			else
			{

			}
		}
		//SampleCorr.CRC32 = lib_CheckCRC32((BYTE*)&SampleCorr,sizeof(TSampleCorr)-sizeof(DWORD));
	}
	
	if(( Type == eCRTL )||( Type == eALL ))//�������� TSampleCtrlPara
	{
	  	memcpy((BYTE*)&SampleCtrlReg,(BYTE*)SampleCtrlDefData,sizeof(TSampleCtrlReg)-sizeof(DWORD));

		//��ѹ�ݽ��ͣ����ѹ��40%����ֵ�����5%��������������
		SampleCtrlReg.SAGCFG.w[0] = (WORD)(((float)wStandardVoltageConst / 10.0)*(SampleCorr.UGain[0].d)/4096*1.414*0.4);
		//����ʱ��10���ܲ�
		SampleCtrlReg.SAGCFG.w[1] = 10*2;

		//�����������޵�60%���� !!!!!!
		SampleCtrlReg.PSTART.d = (DWORD)(((float)dwMeterBasicCurrentConst / 1000.0)*((float)wStandardVoltageConst / 10.0)*(float)(SampleCorr.PGain[0].d/100)*0.6*0.001/256.0);
		if( MeterTypesConst == METER_ZT )
		{
			//��ת��Ϊ100A��2��
			SampleCtrlReg.PSTART.d *= 2;
		}

		//��ѹ��ǣ���Ƶ��ֵ�����ѹ��30%��Ҫ��0.35��
		TempData = (WORD)(((float)wStandardVoltageConst / 10.0)*(SampleCorr.UGain[0].d)/4096*0.3);
		SampleCtrlReg.ZXOTU.d = TempData;//��ѹ��ֵ

		//������ǣ��������0.1%
		TempData = (WORD)(((float)dwMeterBasicCurrentConst / 1000.0)*(float)(SampleCorr.IGain[0].d)*0.001/4096.0);
		SampleCtrlReg.ZXOT.d = TempData;//������ֵ

		//SampleCtrlReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleCtrlReg,sizeof(TSampleCtrlReg)-sizeof(DWORD));
	}
	
	if(( Type == eADJUST )||( Type == eALL ))//TSampleAdjustPara
	{
		memcpy((BYTE*)&SampleAdjustReg,(BYTE*)SampleAdjustDefData,sizeof(TSampleAdjustReg)-sizeof(DWORD));
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
static void CheckSampleRam(eSampleType Type)
{
	BOOL Result;

	CheckSampleRunPara();   // У�����
	if((Type == eCORR) || (Type == eALL))
	{
		if(SampleCorr.CRC32 != lib_CheckCRC32((BYTE *)&SampleCorr, sizeof(TSampleCorr) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCorr), sizeof(TSampleCorr), (BYTE *)&SampleCorr);
			if(Result != TRUE)
			{
				UseSampleDefaultPara(eCORR);
			}
		}
	}
	// У��Ĳ���У��
	if((Type == eADJUST) || (Type == eALL))
	{
		if(SampleAdjustReg.CRC32 != lib_CheckCRC32((BYTE *)&SampleAdjustReg, sizeof(TSampleAdjustReg) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg), sizeof(TSampleAdjustReg), (BYTE *)&SampleAdjustReg);
			if(Result != TRUE)
			{
				UseSampleDefaultPara(eADJUST);
			}
		}
	}
	// ���Ʋ���У��
	if((Type == eCRTL) || (Type == eALL))
	{
		if(SampleCtrlReg.CRC32 != lib_CheckCRC32((BYTE *)&SampleCtrlReg, sizeof(TSampleCtrlReg) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg), sizeof(TSampleCtrlReg), (BYTE *)&SampleCtrlReg);
			if(Result != TRUE)
			{
				UseSampleDefaultPara(eCRTL);
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
void  api_SetSampleRunPara( eIntegratorMode inmode , eMeasurementMode memode )
{
	if (eNoneIntegrator!=inmode)
	{
		samplerunpara.eintegratormode=inmode;
	}
	if (eNonePhase!=memode)
	{
		samplerunpara.emeasurementmode=memode;
	}
	if(lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD)) != samplerunpara.CRC32)//���ݸı�
	{
		samplerunpara.CRC32=lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD));
		UseSampleDefaultPara(eALL);
		CheckSampleRam(eALL);
		if(CheckSampleChipReg())
		{
			GetSampleChipCheckSum();
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
BOOL WriteSampleCombo(DWORD *Combop, BYTE ComboSize)
{
	BYTE i;
	BOOL Rtresult = TRUE;

	for(i = 0; i < ComboSize; i += 3)
	{
		SampleSpecCmdOp(Combop[i], CmdWriteOpen);
		Rtresult &= WriteSampleReg(Combop[i], (BYTE *)&Combop[i + 1], Combop[i + 2]);
		SampleSpecCmdOp(Combop[i], CmdWriteClose);
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
BOOL CheckSampleChipReg(void)
{
	BYTE i;
	BOOL Rtresult = TRUE, Result;
	BYTE lock[2] = {0xE5, 0xDC};
	TFourByteUnion ComData;
	DWORD TEMP;

	CLEAR_WATCH_DOG;
	// �����ж�
	if(api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE)
	{
		return FALSE;
	}
	SampleSpecCmdOp(SYSCFG,CmdWriteClose);//�����Ĵ�����λ����
	// ���ƼĴ���
	for(i = 0; i < (sizeof(SampleCtrlAddr) / sizeof(SampleCtrlAddr[0])); i++)
	{
		// ע�⣡������
		// ����+����оƬ������Ҫÿ���ж��Ƿ���磬��ֹ�ڴ˵���ѵ�ص����Ĺ�
		// SOC�����ɲ�����ѹ
		if(api_CheckSysVol(eOnRunDetectPowerMode) == FALSE)
		{
			return FALSE;
		}

		CLEAR_WATCH_DOG;
		ComData.d = 0;
		Result = Link_ReadSampleReg(SampleCtrlAddr[i][0], ComData.b, SampleCtrlAddr[i][1]);
		if(Result != TRUE)
		{
			Rtresult &= Result;
			continue;
		}

		if(ComData.d != *(((DWORD *)&SampleCtrlReg.CFCFG + i)))
		{
			if(SampleCtrlAddr[i][0] == ZXOTCFG_EMU)//�üĴ���ĳЩλΪ����ֵ ĳЩλΪ��ȡֵ
			{
				if((ComData.d & 0X0000001F) == *(((DWORD *)&SampleCtrlReg.CFCFG + i)))
				{
					continue;
				}
			}
			ComData.d = *((DWORD *)&SampleCtrlReg.CFCFG.d + i);
			SampleSpecCmdOp(SampleCtrlAddr[i][0], CmdWriteOpen);
			Rtresult &= WriteSampleReg(SampleCtrlAddr[i][0], ComData.b, SampleCtrlAddr[i][1]);
			SampleSpecCmdOp(SampleCtrlAddr[i][0], CmdWriteClose);
		}
	}
	if(samplerunpara.eintegratormode == eUnuseIntegrator)   // ����������У��
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlUnuseIntegrator, sizeof(SampleCtrlUnuseIntegrator) / sizeof(DWORD));
	}
	else
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlUseIntegrator, sizeof(SampleCtrlUseIntegrator) / sizeof(DWORD));
	}
	if(samplerunpara.emeasurementmode == eOnePhase)
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlOnePhase, sizeof(SampleCtrlOnePhase) / sizeof(DWORD));
	}
	else
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlThreePhase, sizeof(SampleCtrlThreePhase) / sizeof(DWORD));
	}
	Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlTemp, sizeof(SampleCtrlTemp) / sizeof(DWORD));
	Rtresult &= WriteSampleCombo((DWORD *)SampleCtrleAngle, sizeof(SampleCtrleAngle) / sizeof(DWORD));

	// У׼�Ĵ���
	for(i = 0; i < (sizeof(SampleAdjustAddr) / sizeof(SampleAdjustAddr[0])); i++)
	{
		if(api_CheckSysVol(eOnRunDetectPowerMode) == FALSE)
		{
			return FALSE;
		}

		CLEAR_WATCH_DOG;
		ComData.d = 0;
		Result = Link_ReadSampleReg(SampleAdjustAddr[i][0], ComData.b, SampleAdjustAddr[i][1]);
		if(Result != TRUE)
		{
			Rtresult &= Result;
			continue;
		}

		if(ComData.d != *(((DWORD *)&SampleAdjustReg.GP[0] + i)))
		{
			ComData.d = *((DWORD *)&SampleAdjustReg.GP[0] + i);
			SampleSpecCmdOp(SampleAdjustAddr[i][0], CmdWriteOpen);
			Rtresult &= WriteSampleReg(SampleAdjustAddr[i][0], ComData.b, SampleAdjustAddr[i][1]);
			SampleSpecCmdOp(SampleAdjustAddr[i][0], CmdWriteClose);
		}
	}

	//��չ�Ĵ���
	if( __get_PRIMASK() == 0 )//ȫ���жϴ�
	{		
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD));
	}

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
static void HardResetSampleChip(void)
{		
	//�ֲ�Ҫ��1ms
//	RESET_SAMPLE_OPEN;
//	api_Delayms(10);
//	RESET_SAMPLE_CLOSE;
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 200 );
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
static void SampleChipMinVerification(void)
{
	DWORD tdw1, tdw2, dwSum;
	BYTE Flag,i;

	//�����ж�
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )	
	{
		return;
	}	
	
	Flag = 0;

	if( CheckSampleSumRegPro(eSumCheck) != TRUE)
	{
		api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);
		Flag = 0x55;
	}
	//������������ѹΪ0������Ϊ��ʱ����оƬ�쳣��λ����Ҫ���³�ʼ��
	if( bySamplePowerOnTimer <= 25 )
	{
		for(i=0;i<3;i++)
		{
			if(RemoteValue.U[i].d > 0)
			{
				break;
			}
		}
		if(i == 3)
		{			
			Flag = 0x55;
			
			// Ӧ�ü�¼�澯�¼�
			api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);
		}
	}

	if(Flag == 0x55)
	{
		if(byReWriteSampleChipRegCount < 12)
		{
			byReWriteSampleChipRegCount++;
			if( byReWriteSampleChipRegCount == 10 )
			{
				//HardResetSampleChip();
				api_SetError(ERR_CHECK_5460_1);
			}
		}
		ChipRecovery();
	}
	else
	{
		byReWriteSampleChipRegCount = 0;
		RecoverNum = 0;
		api_ClrError(ERR_CHECK_5460_1);
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
BOOL api_InitSampleChip( void  )
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
SWORD  api_GetTemp(void)
{
	SWORD Temp=0;
	BYTE i = 0;
	DWORD buf=0x00000001;

	WriteSampleCombo((DWORD *)SampleGetTemp,sizeof(SampleGetTemp)/sizeof(DWORD));
	api_Delayms(1);//�ȴ�1ms
	while (i<3)
	{
		if (Link_ReadSampleReg(0x00F7,(BYTE*)&buf,4))
		{
			if (buf&BIT10)//������Ч
			{
				if (buf&BIT9)
				{
					Temp=((SWORD)(buf|0xFFFFFE00))*10/4;
				}
				else
				{
					Temp=(buf-BIT10)*10/4;
				}
				break;
			}
		}
		i++;
	}
	if(i == 3)
	{
		return 0;
	}
	return Temp - 15;   // �¶�У׼-1.5���϶�
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
void api_PowerUpSample(void)	
{
	DWORD dwSum;
	BYTE i;
	BOOL Result;

	CheckSampleRam( eALL );
	
	//��ʼ������
	InitSampleCaliUI();
	VMAXLimit = wStandardVoltageConst*220;//CalMaxLimiteValue( eVOLTAGE );
	IMINLimit = (DWORD)dwMeterBasicCurrentConst  * 15 / 1000;//�������ޱ���һ��
	IMAXLimit = (DWORD)dwMeterMaxCurrentConst  * 15 ; //1.5Imax
	bySamplePowerOnTimer = 30;
	dwSampleSFlag = 0;
	dwSamplePFlag = 0;		
	byReWriteSampleChipRegCount = 0;
	CheckSampleChipReg();

	GetSampleChipCheckSum();//��У���
	
	#if( SEL_STAT_V_RUN == YES )//�ָ���ѹ�ϸ���
	PowerUpVRunTime( );
	#endif
	
	//�ϵ������ѹ ���� ����
	//PowerUpReadVoltage();
    //byPowerOnFlag = 0;
	
	#if( SEL_AHOUR_FUNC == YES )
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
	memset((BYTE*)&RemoteValue,0x00,sizeof(TRemoteValue));
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
static void CheckEmuErr( void )
{
	static BYTE EmuErrCnt = 0;
	DWORD RegData = 0;

	//��ȡHFConst�ķ�ʽ�жϼ���оƬ���� 
	//����ʹ���ж�У��͵ķ�ʽ ��ΪSPIû��У�� ���SPI���߶̽ӵ�VCC����GND �������������ǹ̶���
	//У���Ҳ�ᱻˢ��ΪȫFF����ȫ00 ���¹����޷������
	Link_ReadSampleReg(HFConst_EMU,(BYTE*)&RegData,2 );
	if(RegData != SampleCtrlReg.HFCONST.d)
	{
		if(EmuErrCnt < 12)
		{
			EmuErrCnt++;
			
			//�������5�γ���HFConstֵ����Ӧ ����У��һ��HFConst
			//��һ��ֹRAM�����ݳ��ִ��� �ڶ��������оƬ���ָ�λ ÿ����5��Ż���мĴ���У�� ������󱨵�����
			if(EmuErrCnt == 5)
			{
				ChipRecovery();
			}
			
			//����10�δ��� ��¼����оƬ����
			if(EmuErrCnt == 10)
			{
				api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);
				api_SetError( ERR_CHECK_5460_1 ); //�ü���оƬͨ�Ź���
			}
		}
	}
	else
	{
		//����ָ��� ����֮ǰ����������10�����ϵĴ��� ���¼��һ�����мĴ������� �ڲ���api_ClrError
		if(EmuErrCnt > 10)
		{
			SampleChipMinVerification();
		}
		EmuErrCnt = 0; 
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
	//����ż���г��������
	#if  SELECT_HARMONY_VI_CAL == 1
	if (RawDataHalfWaveCnt == 2)
	{
		HarmAndVICharacCal_Task();
		RawDataHalfWaveCnt = 0;
	}
	#endif
	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )//5s����е�ѹ�ϸ����ۼƣ�������ʱ�䲻Ϊ1440
	{	
		//�ϵ�30s�����Ч�飬���⻹û��������оƬ��Ч���
		if( bySamplePowerOnTimer == 0 )
		{
			CheckSampleRam(eALL);
			SampleChipMinVerification();
        }

		#if( SEL_STAT_V_RUN == YES )//�����ѹ�ϸ��ʲ���		
		StatVRunTime();
		#endif
	}
	
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )
	{
		//ÿ���Ӽ���һ��ƽ������
	  	// api_CalPaverage();
		//ÿ�����ٳ�ʼ��һ�£�������ѹ����������������������ȫʧѹ�л���
		InitSampleCaliUI();
		#if( SEL_AHOUR_FUNC == YES )
		if(t.Min == 26)
		{
			SwapAHour(); 
		}
		#endif
	}
	
	if(bySamplePowerOnTimer)
	{
		bySamplePowerOnTimer--;
		if(bySamplePowerOnTimer == HSDC_OPEN_TIME)
		{
			//��HSDC�ӿڱ�����ڴ�ȫ���ж�֮�󣬷���ᷢ��Ƿ��
			WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD));
			CheckSampleSumRegPro(eUpDateSumCheck);//����У���
		}
	}
	
	#if( SEL_STAT_V_RUN == YES )
	if( ClrVRunMonTimer != 0 )
	{
		if( ClrVRunMonTimer > 30 )//����ֵ�ж�
		{
			ClrVRunMonTimer = 3;
		}
		
		ClrVRunMonTimer--;
		if( ClrVRunMonTimer == 0 )
		{
			//ÿ��1��0ʱ���µ�ѹ�ϸ���
			api_ClearVRunTimeNow(BIT1);
		}
	}
	
	if( ClrVRunDayTimer != 0 )
	{
		if( ClrVRunDayTimer > 30 )//����ֵ�ж�
		{
			ClrVRunDayTimer = 3;
		}
		
		ClrVRunDayTimer--;
		if( ClrVRunDayTimer == 0 )
		{
			//ÿ��0ʱ���յ�ѹ�ϸ���
			api_ClearVRunTimeNow(BIT0);
		}
	}
	#endif
	
	
	
	#if( SEL_AHOUR_FUNC == YES )
	AccumulateAHour();
	#endif

	//����оƬ���ϼ��
	CheckEmuErr();
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
BYTE api_GetSAGStatus(void)
{
	TTwoByteUnion ComData;
	BOOL Result;
	
	if( Link_ReadSampleReg(EMMIF_EMU,ComData.b,2) == TRUE)
	{
		if((ComData.w & 0x4000) == 0x0000)
		{	
			return TRUE;
		}
	}

	ComData.w=0x4000;
	SampleSpecCmdOp(EMMIF_EMU,CmdWriteOpen);
	Result = WriteSampleReg(EMMIF_EMU,ComData.b,2);
	SampleSpecCmdOp(EMMIF_EMU,CmdWriteClose);

	return FALSE;
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
	TFourByteUnion RegData = {0};

	Read_UIP();
	if(api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_10_MS) == TRUE)
	{
		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_10_MS);
		if( HSDCTimer >= 20 )
		{
			if(TopoErrFlag < 20)
			{
				TopoErrFlag++;
			}
			HSDCTimer = 0;
			api_TopWaveSpiInit();
			Link_ReadSampleReg(HSDCCTL_EMU, RegData.b, 4);
			if ((RegData.d & 0x00000001) == 0)
			{
				api_WriteSysUNMsg(HSDC_ERR);
				// ��HSDC�ӿڱ�����ڴ�ȫ���ж�֮�󣬷���ᷢ��Ƿ��
				WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD));
				CheckSampleSumRegPro(eUpDateSumCheck); // ����У���
			}
		}
	}

	if(api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_SECOND) == TRUE)
	{
		Task_Sec_Sample();
		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_SECOND);
	}

	if(api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_HOUR) == TRUE)
	{
		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_HOUR);
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t);
	#if(SEL_STAT_V_RUN == YES)
		if(t.Hour == 0)
		{
			// ��ʱ3���壬��ֹ�ڶ����ʱ������Ϊ0
			if(t.Day == 1)
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
DWORD api_LowPowerGetUI( eUIPType Type, BYTE Phase)
{	
	TFourByteUnion ComData;

	ComData.d = 0;
	
	//U��ѹ
	if( Type == eVOLTAGE )
	{
		if( Phase >= MAX_PHASE )
		{
			return 0;
		}
		if( Link_ReadSampleReg(UA_EMU+4*Phase,ComData.b,4) == TRUE )
		{
			ComData.d = (QWORD)ComData.d*1000/SampleCorr.UGain[Phase].d;	
		}
	}
	else//����
	{
		if( Phase >= MAX_PHASE+1 )
		{
			return 0;
		}

		if( Link_ReadSampleReg(IA_EMU+4*Phase,ComData.b,4) == TRUE )
		{
			ComData.d = (DWORD)((double)ComData.d*10000.0/SampleCorr.IGain[Phase].d);	
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
const BYTE MaxDot[] = 	{3,	4,	1,	1,	6,	6,	6,	6,	6,	6,	4,	2,		2,		2,		2,	2,	4};
const BYTE DefDot[] = 	{1,	3,	1,	1,	4,	4,	4,	4,	4,	4,	3,	2,		2,		2,		2,	2,	3};
const BYTE MaxLen[] = 	{4,	4,	2,	2,	4,	4,	4,	4,	4,	4,	4,	2,		2,		2,		2,	2,	4};
const BYTE MinPhase[] = {1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	1,		1,	0xff,	0xff,0,	0};
const BYTE MaxPhase[] = {3,	5,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,		3,	0xff,	0xff,0,	0};
BOOL api_GetRemoteData(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *Buf)
{
	TFourByteUnion td;
	BYTE Phase;
	BYTE ReadDot;
	BYTE BackFlag;
	
	if( Len > MaxLen[Type&0x001f] )
	{
		return FALSE;
	}
	
	Phase = (Type>>12);
	if(api_GetMeasureMentMode() == eOnePhase)//����ģʽ�ܷ���A��
	{
		if(Phase == 0)
		{
			Phase = 1;
		}
	}

	if( (Phase<MinPhase[Type&0x001f]) || (Phase>MaxPhase[Type&0x001f]) )
	{
		return FALSE;
	}

	td.d = 0;
	switch( Type & 0x00ff )
	{
		case REMOTE_U:
			memcpy(td.b,RemoteValue.U[Phase-1].b,4);
			break;
		case REMOTE_I:
			if(MeterTypesConst == METER_3P3W )
			{
				//�������߱�֧�����ߵ���
				if( Type == (PHASE_N+REMOTE_I) )
				{
					return FALSE;
				}
			}
			td.d = RemoteValue.I[Phase-1].d;
			break;		
		case REMOTE_P:
			td.d = RemoteValue.P[Phase].d;
			break;
		case REMOTE_Q:
			td.d = RemoteValue.Q[Phase].d;
			break;
		case REMOTE_S:
			memcpy(td.b,RemoteValue.S[Phase].b,4);
			break;
		case REMOTE_P_AVE:
			memcpy(td.b,RemoteValue.Pav[0][Phase].b,4);
			break;
		case REMOTE_Q_AVE:
			memcpy(td.b,RemoteValue.Pav[1][Phase].b,4);
			break;
		case REMOTE_S_AVE:
			memcpy(td.b,RemoteValue.Pav[2][Phase].b,4);
			break;
		case REMOTE_COS:
			td.d = RemoteValue.Cos[Phase].l;
			break;
		case REMOTE_HZ:
			memcpy(td.b,RemoteValue.Freq.b,4);
			break;
	#if( SEL_VOL_ANGLE_FUNC == YES )
		case REMOTE_PHASEU:
			memcpy(td.b,RemoteValue.YUAngle[Phase-1].b,4);
			break;
		case REMOTE_PHASE:
			td.l = RemoteValue.YIAngle[Phase-1].d - RemoteValue.YUAngle[Phase-1].d;
			//��ѹ�����Ϊ0ʱ�������
			if( (RemoteValue.U[Phase-1].d == 0) || (RemoteValue.I[Phase-1].d == 0) )
			{
				td.l = 0;
			}
			if( td.l < 0 )
			{
				td.l += 3600;
			}
			break;
	#endif

		default:
			return FALSE;
	}
	
	if( Dot > MaxDot[Type&0x001f] )
	{
		ReadDot = MaxDot[Type&0x001f] - DefDot[Type&0x001f];
	}
	else
	{
		ReadDot = MaxDot[Type&0x001f] - Dot;
	}
	
	if( ReadDot <= MaxDot[Type&0x001f] )
	{
		td.l = td.l/(long)lib_MyPow10(ReadDot);
	}
	else
	{
		return FALSE;
	}
	
	if( (DataType==DATA_BCD) || (DataType==DATA_HEX) )
	{
		BackFlag = 0;
		if( td.l < 0 )
		{
			td.l *= -1;
			BackFlag = 1;
		}
		if( DataType==DATA_BCD )
		{
			td.d = lib_DWBinToBCD(td.d);
			
			//�������λ�÷���
			if( BackFlag == 1 )
			{
				td.b[Len - 1] |= 0x80;
			}
		}
	}

	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			td.d = 0;
		}
	}
	
	memcpy(Buf,td.b,Len);
	
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
WORD api_GetSampleStatus(BYTE Type)
{
	BYTE PQDirect;
	WORD i,tw;
	TFourByteUnion td;
	long tl;
	
	tw = 0; 
	
	switch(Type)
	{
		case 0x00://��ѹ��ƽ����
			tw = RemoteValue.Uunbalance.w;
			break;
		case 0x01://������ƽ����
			tw = RemoteValue.Iunbalance.w;
			break;
		case 0x02://������
			tl = labs(RemoteValue.P[0].l);
			tw = tl / (wStandardVoltageConst/10) / (dwMeterMaxCurrentConst) / 3;
			break;
		case 0x10:
			//��ѹ״̬	1--����0.6Un	0--����0.6Un
			for(i=0;i<3;i++)
			{
				if( (MeterTypesConst == METER_3P3W) && ( i == 1 ) )
				{
					continue;
				}
				
				if(RemoteValue.U[i].d < (wStandardVoltageConst * 100 * 6 / 10))
				{
					tw |= (0x0001<<i);
				}
			}

			Link_ReadSampleReg( PHASES_EMU, td.b ,2);
			
			if((td.b[0]&0x08) != 0 )
			{
				tw |= 0x8000;
			}
			break;
		case 0x11:
			//����״̬	1--����5%Ib	0--����5%Ib
			for(i=0;i<3;i++)
			{
				if( (MeterTypesConst == METER_3P3W) && ( i == 1 ) )
				{
					continue;
				}
				
				if(labs( RemoteValue.I[i].l ) < ((DWORD)dwMeterBasicCurrentConst * 10 * 5 / 100))
				{
					tw |= (0x0001<<i);
				}
			}
			
			if( api_GetSysStatus( eSYS_STATUS_REVERSE_PHASE_I ) == TRUE )
			{
				tw |= 0x8000;
			}
			
			break;
		case 0x20://��Ǳ��״̬
			//bit0,bit1,bit2�ֱ����ABC����״̬ 0--�� 1--Ǳ��
			tw = (WORD)(dwSampleSFlag&0x00000007);
			break;
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
			PQDirect = 0;
			tw = 0;
			for(i=0; i<4; i++)
			{
				//�й����ʵķ���
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P + i) == TRUE )
				{
					PQDirect |= (0x01 << i);
				}
			}
			for(i=0; i<4; i++)
			{
				//�޹����ʵķ���
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_Q + i) == TRUE )
				{
					PQDirect |= (0x10 << i);
				}
			}
		
			//��λ���޹� ��λ���й� 0���� 1�Ǹ�
			PQDirect = (PQDirect>>(Type-0x30))&0x11;
			switch( PQDirect )
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

		case 0x40://���ڹ����ж�
			if(labs( RemoteValue.S[0].l ) >= (wStandardVoltageConst * dwMeterBasicCurrentConst * 4 * 3 / 10000)) //U*I*0.004*3
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
WORD api_GetSampleCurrMinus(BYTE *bBuf)
{
	BYTE i,bTemp=0;
	WORD Result;
	
	for(i = 0; i < MAX_PHASE; i++)
	{
		if( RemoteValue.I[i].l < 0 )
		{
			bTemp |= (0x01<<(2*i));
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
BYTE api_SetSampleChip(BYTE bCount)
{	
	TSampleChipPara SampleChipPara;

	if( (bCount != 1) && (bCount != 3))
	{
		return FALSE;
	}
	memset(&SampleChipPara,0x00,sizeof(TSampleChipPara));

	SampleChipPara.bCount = bCount;
	
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleChipPara),sizeof(TSampleChipPara),(BYTE*)&SampleChipPara);

	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

	api_SetSampleRunPara(eNoneIntegrator, ((bCount==1)?eOnePhase:eThreePhase));

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
void api_FactoryInitSample( void )
{
	api_SetSampleChip(3);//��ʼ��Ϊ����
}

#endif// #if( SAMPLE_CHIP == CHIP_RN8302B )
