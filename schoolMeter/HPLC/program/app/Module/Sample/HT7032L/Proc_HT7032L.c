//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.30
//����		����оƬHT7038�������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7032L.h"
#include "AHour.h"
#include "statistics_voltage.h"

#if(SAMPLE_CHIP==CHIP_HT7032L)

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define AD_ONEWAVE_SIMPLE_NUM			72 		//һ���ܲ�AD������
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------



//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
TRemoteValue 					RemoteValue;				//Զ������
TSampleCtrlPara					SampleCtrlPara;				//����оƬ������Ĵ�������
TSampleAdjustPara				SampleAdjustPara;			//����оƬ������Ĵ�������
TSampleOffsetPara				SampleOffsetPara;			//����оƬƫ����Ĵ�������
TCurrenttValue					CurrentValue[9];			//�ֶβ����ͻ�����������
TSampleAdjustParaDiv 			SampleAdjustParaDiv;		//�ֶβ�������
TSampleManualModifyPara			SampleManualModifyPara;		//�ֶ�����ϵ��
TSampleHalfWavePara				SampleHalfWavePara;			//ֱ��ż��г��ϵ��
TSampleHalfWaveTimePara			SampleHalfWaveTimePara;		//ֱ��ż��г��Ҫ����ʱ��(Сʱ)
TOriginalPara					OriginalPara;				//����Ĭ������ʹ�С���ϵ�� ���ĳ����δУ Ĭ�ϲ��ø�ϵ��
	
float	gfPowerCorr;										//����ϵ��������P,Q,Sʱ��
BYTE 	bySampleStep;										//��ѭ��ˢ��Զ������Э������ÿ���һ�Σ�ÿ���ڴ�ѭ�����ˢ�¸���Զ������
BYTE 	byPowerOnFlag;                                      //�ϵ��־
//DWORD 	dwSamplePFlag;										//�й����޹�������ÿ�ζ��й�������ʱˢ��
DWORD	dwSampleSFlag;										//���ࡢ����״̬��־����ÿ��ˢ�µĵ�һ��Զ��ֵʱˢ��
BYTE 	byReWriteSampleChipRegCount;						//�ж�оƬЧ�鲻�Ի�д������
BYTE 	bySamplePowerOnTimer;								//�ϵ絹��ʱ������
DWORD 	dwSampleCheckSum[4];									//����оƬЧ��� !!!!!!���ĸ�
float	gfPQSaverage[3][4];									//����1����ƽ������
BYTE	byPQSaverageCnt[3][4];								//ƽ�������ۼӴ���	
BOOL 	UnBalanceGainFlag[3];								//��ѹ��ƽ�ⲹ����־ TRUE-�Ѳ��� FALSE-δ����
BYTE    HalfWaveCoeFlag[3];                                 //ֱ��ż��г������
BYTE	HalfWaveUpdateFlag[3];								//ֱ��ż��г��״̬���±�־
BYTE	ADCSampleCount;										//����оƬADC����ʱ�������
DWORD	HalfWavePowerOnTime;								//ֱ��ż��г���ϵ����ۼ�ʱ��(����)
BOOL	HalfWaveStatusFlag;									//ֱ��ż��г��״̬��־  TRUE: �����ж�  FALSE: �ر��ж�

//�ֶβ����йز���
BYTE 	g_ucNowSampleDivisionNo[MAX_PHASE];					//�ֶβ�����ǰ����

static BYTE bHalfWaveCalTime;								//�����жϰ벨����

//�������ڵĻ�����غ���
static const SWORD TAB_Cos1[AD_ONEWAVE_SIMPLE_NUM] =
{
	1000,  996,  985,  966,  940,  906,  866,  819,  766,  707,  643,  574,  500,  423,  342,  259,  174,   87,
	   0,  -87, -174, -259, -342, -423, -500, -574, -643, -707, -766, -819, -866, -906, -940, -966, -985, -996,
   -1000, -996, -985, -966, -940, -906, -866, -819, -766, -707, -643, -574, -500, -423, -342, -259, -174,  -87,
	   0,   87,  174,  259,  342,  423,  500,  574,  643,  707,  766,  819,  866,  906,  940,  966,  985,  996,
};
static const SWORD TAB_Sin1[AD_ONEWAVE_SIMPLE_NUM] =
{
       0,   87,  174,  259,  342,  423,  500,  574,  643,  707,  766,  819,  866,  906,  940,  966,  985,  996,
	1000,  996,  985,  966,  940,  906,  866,  819,  766,  707,  643,  574,  500,  423,  342,  259,  174,   87,
	   0,  -87, -174, -259, -342, -423, -500, -574, -643, -707, -766, -819, -866, -906, -940, -966, -985, -996,
   -1000, -996, -985, -966, -940, -906, -866, -819, -766, -707, -643, -574, -500, -423, -342, -259, -174,  -87,
};
//�������ڵĶ���г����غ���
static const SWORD TAB_Cos2[AD_ONEWAVE_SIMPLE_NUM] =
{
	1000,  985,  940,  866,  766,  643,  500,  342,  174,    0, -174, -342, -500, -643, -766, -866, -940, -985,
   -1000, -985, -940, -866, -766, -643, -500, -342, -174,    0,  174,  342,  500,  643,  766,  866,  940,  985,
	1000,  985,  940,  866,  766,  643,  500,  342,  174,    0, -174, -342, -500, -643, -766, -866, -940, -985,
   -1000, -985, -940, -866, -766, -643, -500, -342, -174,    0,  174,  342,  500,  643,  766,  866,  940,  985,
};
static const SWORD TAB_Sin2[AD_ONEWAVE_SIMPLE_NUM] =
{
	   0,  174,  342,  500,  643,  766,  866,  940,  985, 1000,  985,  940,  866,  766,  643,  500,  342,  174,
	   0, -174, -342, -500, -643, -766, -866, -940, -985,-1000, -985, -940, -866, -766, -643, -500, -342, -174,
	   0,  174,  342,  500,  643,  766,  866,  940,  985, 1000,  985,  940,  866,  766,  643,  500,  342,  174,
	   0, -174, -342, -500, -643, -766, -866, -940, -985,-1000, -985, -940, -866, -766, -643, -500, -342, -174,
};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static BOOL CheckSampleChipSum(void);
static void Read_UIP(void);
static BOOL JudgeHalfWaveRegulate( void );

//-----------------------------------------------
//				��������
//-----------------------------------------------


//---------------------------------------------------------------
//��������: ����ʱ�ֶβ�����õ�ǰ�������ڵ�����
//
//����: 	ucPhase- 0��A��
//					1��B��
// 					2��C��
//                   
//����ֵ:  0-9����ǰ����
//		  0xFF�������쳣
//
//��ע:   �ж��ͻ����� ���ڲ���
//---------------------------------------------------------------
BYTE GetRowSampleSegmentNo( BYTE ucPhase )
{
	BYTE Num;
	
	if(ucPhase >= MAX_PHASE)
	{
		return 0xff;
	}
	
	for(Num = 0; Num < 9; Num++)
	{
		//��Ϊ�������з��ŵ��� ���ԱȽ�֮����Ҫȡ����ֵ 
		if( labs(RemoteValue.I[ucPhase].l)  > (CurrentValue[Num].Upper*10) )
		{
			return Num;	//Num��
		}
		else if(labs(RemoteValue.I[ucPhase].l) > (CurrentValue[Num].Lower*10) )
		{
			if(g_ucNowSampleDivisionNo[ucPhase] == (Num + 1))	//ԭ����Num+1��
			{	//���ڵ�1��,���л�
				return (Num + 1);	//Num+1��
			}
			else
			{
				return Num;	//Num��
			}
		}
	}
	
	return 9;
}

//---------------------------------------------------------------
//��������: У��ʱ�ֶβ�����õ�ǰ�������ڵ�����
//
//����: 	ucPhase- 0��A��
//					1��B��
// 					2��C��
//                   
//����ֵ:  0-9����ǰ����
//		  0xFF�������쳣
//
//��ע:   ���ж��ͻ����� ����У׼
//---------------------------------------------------------------
BYTE GetRowSampleSegmentAdjustNo(BYTE ucPhase)
{
	BYTE Num;
	
	if(ucPhase >= MAX_PHASE)
	{
		return 0xff;
	}
	
	for(Num = 0; Num < 9; Num++)
	{
		if(labs(RemoteValue.I[ucPhase].l) > (CurrentValue[Num].Upper*10))
		{
			break;
		}
	}
	
	return Num;
}
//-----------------------------------------------
//��������: ����P��Q��Sһ����ƽ������
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void CalPQSaverage(void)
{	
	BYTE i,j;
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<4;j++)
		{
			if( (MeterTypesConst==METER_3P3W) && (j==2) )
			{
				continue;
			}
			
			if(byPQSaverageCnt[i][j] == 0) 
			{
				continue;
			}
			RemoteValue.PQSAverage[i][j].l = (long)(gfPQSaverage[i][j]/(float)byPQSaverageCnt[i][j]);
			
			gfPQSaverage[i][j] = 0;
			byPQSaverageCnt[i][j] = 0;
		}
	}
}

//-----------------------------------------------
//��������: дĳ��У��ϵ����EEPROM
//
//����:		WriteEepromFlag[in]	bit0 SampleCtrlAddr 0--дram��дee	1--дramдee
//								bit1 SampleAdjustAddr 0--дram��дee	1--дramдee
//								bit2 SampleOffsetAddr 0--дram��дee	1--дramдee
//			Addr[in]	��ַ	 
//			Buf[in]		д�����ݣ����ֽڣ����ں󣬵���ǰ
//����ֵ: 	
//		    TRUE/FALSE
//��ע: 
//-----------------------------------------------
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,WORD Addr, BYTE *Buf)
{
	BYTE i, DivCurrentNo, Result;
	DWORD *p;
	short QGainTmp,PGainTmp;
		
	for(i=0;i<(sizeof(SampleCtrlAddr)/sizeof(SampleCtrlAddr[0]));i++)
	{
		if(Addr == SampleCtrlAddr[i])
		{
			p = (DWORD*)&SampleCtrlPara;
			memcpy((void*)&p[i],Buf,sizeof(DWORD));
			
			if( (WriteEepromFlag&0x01) == 0x00 )
			{
				SampleCtrlPara.CRC32 = lib_CheckCRC32((BYTE*)&SampleCtrlPara,sizeof(TSampleCtrlPara)-sizeof(DWORD));				
			}
			else
			{
				//��дeeУ��ϵ������
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlPara), sizeof(TSampleCtrlPara), (BYTE*)&SampleCtrlPara);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}
			return TRUE;
		}
	}
	
	for(i=0;i<(sizeof(SampleAdjustAddr)/sizeof(SampleAdjustAddr[0]));i++)
	{
		if(Addr == SampleAdjustAddr[i])
		{
			
			if(SelDivAdjust == YES)//��������
			{
				if((Addr>=w_GainUA)&&(Addr<= w_GainIN)) 	//�ֶξ��������� ����дUIϵ��
				{
					p = (DWORD *)&SampleAdjustPara;
					memcpy((void*)&p[i],Buf,sizeof(DWORD));

				}
				else if((Addr>=w_GPA)&&(Addr<= w_GSC))
				{
					//��ȡ��ǰ�Ĵ�������Ӧ�������
					DivCurrentNo = GetRowSampleSegmentAdjustNo( (Addr - w_GPA) % 3  );
					if(AdjustDivCurrent[DivCurrentNo] == dwBigStandardCurrent)//���������������У����	
					{
						p = (DWORD *)&SampleAdjustPara;
						memcpy((void*)&p[i],Buf,sizeof(DWORD));
					}
				}
				else if((Addr>=w_GphsA0)&&(Addr<= w_GphsC2))
				{
					DivCurrentNo = GetRowSampleSegmentAdjustNo( (Addr - w_GphsA0) /3  );
					if(AdjustDivCurrent[DivCurrentNo] == dwBigStandardCurrent)//���������������У����	
					{
						if((Addr >= w_GphsA0) && (Addr <= w_GphsA2))
						{
							memcpy( &SampleAdjustPara.PhsReg[0][0], Buf, sizeof(SampleAdjustPara.PhsReg[0][0]) );
						}
						else if((Addr >= w_GphsB0) && (Addr <= w_GphsB2))
						{
							memcpy( &SampleAdjustPara.PhsReg[0][1], Buf, sizeof(SampleAdjustPara.PhsReg[0][1]) );
						}
						else if((Addr >= w_GphsC0) && (Addr <= w_GphsC2))
						{
							memcpy( &SampleAdjustPara.PhsReg[0][2], Buf, sizeof(SampleAdjustPara.PhsReg[0][2]) );
						}
					}
					//�ֶβ�������������� ��С������ǵ� �������·���ַ�Ǵ���ǻ���С��� ȫ��д��С���
					else if(AdjustDivCurrent[DivCurrentNo] == dwSmallStandardCurrent)
					{
						if((Addr >= w_GphsA0) && (Addr <= w_GphsA2))
						{
							memcpy( &SampleAdjustPara.PhsReg[1][0], Buf, sizeof(SampleAdjustPara.PhsReg[1][0]) );
							memcpy( &SampleAdjustPara.PhsReg[2][0], Buf, sizeof(SampleAdjustPara.PhsReg[2][0]) );
						}
						else if((Addr >= w_GphsB0) && (Addr <= w_GphsB2))
						{
							memcpy( &SampleAdjustPara.PhsReg[1][1], Buf, sizeof(SampleAdjustPara.PhsReg[1][1]) );
							memcpy( &SampleAdjustPara.PhsReg[2][1], Buf, sizeof(SampleAdjustPara.PhsReg[2][1]) );
						}
						else if((Addr >= w_GphsC0) && (Addr <= w_GphsC2))
						{
							memcpy( &SampleAdjustPara.PhsReg[1][2], Buf, sizeof(SampleAdjustPara.PhsReg[1][2]) );
							memcpy( &SampleAdjustPara.PhsReg[2][2], Buf, sizeof(SampleAdjustPara.PhsReg[2][2]) );
						}
					}
				}
			}
			else
			{
				p = (DWORD *)&SampleAdjustPara;
				memcpy((void*)&p[i],Buf,sizeof(DWORD));
				//�����ôд�ȼĴ�������������ا��
				#warning: "����:--��ا��";
			}
			//ֻ����RAM�ͼ���оƬ ��дEE
			if( (WriteEepromFlag&0x02) == 0x00 )
			{
				SampleAdjustPara.CRC32 = lib_CheckCRC32((BYTE*)&SampleAdjustPara,sizeof(TSampleAdjustPara)-sizeof(DWORD));				
			}
			else
			{
				//��дeeУ��ϵ������
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustPara), sizeof(TSampleAdjustPara), (BYTE*)&SampleAdjustPara);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				
				if(SelDivAdjust == YES)
				{
					Result = FALSE;
					
					if((Addr >= w_GPA)&&(Addr <= w_GPC))
					{
						memcpy( &SampleAdjustParaDiv.PGainDiv[Addr - w_GPA][DivCurrentNo], Buf, sizeof(DWORD) );
						Result = TRUE;
					}
					#warning: "����://����У��ƫִ���Ƿ�Ӧ�����--��ا��";
					else if((Addr >= w_GQA)&&(Addr <= w_GQA))
					{
						QGainTmp = (Buf[1] << 8) + Buf[0];
						PGainTmp = (short)SampleAdjustParaDiv.PGainDiv[Addr - w_GQA][DivCurrentNo];
						if(PGainTmp == 0) //����õ�ϵ��Ϊ�� ����Ĭ��ϵ��
						{
							PGainTmp = (short)OriginalPara.PGainCoe[Addr - w_GQA];
						}
						QGainTmp = QGainTmp - PGainTmp;
						
						//ƫ��ֻ��һ���ֽ� ����ֵ�� -128~127֮��
						if(QGainTmp > 127)
						{
							QGainTmp = 127;
						}
						else if(QGainTmp < -128)
						{
							QGainTmp = -128;
						}
						if(DivCurrentNo < 9) //�޹�����û�е�10����
						{
							SampleManualModifyPara.ManualModifyQGain[Addr - w_GQA][DivCurrentNo] = (BYTE)QGainTmp;
						}
						
						//ϵ����EEPROM
						api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);			
						api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleManualModifyPara ), sizeof(TSampleManualModifyPara), (BYTE *)&SampleManualModifyPara );
						api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
						
						//�ѵ�ǰ����������������ΪFF ���¿�ʼд�Ĵ���
						memset( g_ucNowSampleDivisionNo, 0xFF, sizeof(g_ucNowSampleDivisionNo) );
					}
					else if((Addr >= w_GphsA0)&&(Addr <= w_GphsA2))
					{
						memcpy( &SampleAdjustParaDiv.PhsRegDiv[0][DivCurrentNo], Buf, sizeof(SampleAdjustParaDiv.PhsRegDiv[0][DivCurrentNo]) );
						Result = TRUE;
					}
					else if((Addr >= w_GphsB0)&&(Addr <= w_GphsB2))
					{
						memcpy( &SampleAdjustParaDiv.PhsRegDiv[1][DivCurrentNo], Buf, sizeof(SampleAdjustParaDiv.PhsRegDiv[1][DivCurrentNo]) );
						Result = TRUE;
					}
					else if((Addr >= w_GphsC0)&&(Addr <= w_GphsC2))
					{
						memcpy( &SampleAdjustParaDiv.PhsRegDiv[2][DivCurrentNo], Buf, sizeof(SampleAdjustParaDiv.PhsRegDiv[2][DivCurrentNo]) );
						Result = TRUE;
					}
					//����ϵ��д��EEPROM
					if(Result == TRUE)
					{
						api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
						api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustParaDiv), sizeof(TSampleAdjustParaDiv), (BYTE*)&SampleAdjustParaDiv);
						api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA); 
					}
				}
			}
			
			//У��ϵ�������ı�����У�龫��Ĭ��ϵ��
			if(SelDivAdjust == YES)
			{
				if((Addr >= w_GPA)&&(Addr <= w_GphsC2))
				{
					memcpy( OriginalPara.PGainCoe, SampleAdjustPara.PGain, sizeof(OriginalPara.PGainCoe) );
					memcpy( OriginalPara.AngCoe, SampleAdjustPara.PhsReg, sizeof(OriginalPara.AngCoe) );
					OriginalPara.CRC32 = lib_CheckCRC32((BYTE *)&OriginalPara,(sizeof(OriginalPara) - sizeof(DWORD)) );
				}
			}
			
			return TRUE;
		}
	}
	
	for(i=0;i<(sizeof(SampleOffsetAddr)/sizeof(SampleOffsetAddr[0]));i++)
	{
		if(Addr == SampleOffsetAddr[i])
		{
			p = (DWORD*)&SampleOffsetPara;
			memcpy((void*)&p[i],Buf,sizeof(DWORD));
			
			if( (WriteEepromFlag&0x04) == 0x00 )
			{
				SampleOffsetPara.CRC32 = lib_CheckCRC32((BYTE*)&SampleOffsetPara,sizeof(TSampleOffsetPara)-sizeof(DWORD));				
			}
			else
			{
				//��дeeУ��ϵ������
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleOffsetPara), sizeof(TSampleOffsetPara), (BYTE*)&SampleOffsetPara);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}
			return TRUE;
		}
	}	
	
	return FALSE;
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
	BYTE i,j;
	DWORD tdw1, tdw2;

	
	
	for(i=0;i<3;i++)
	{
		dwSampleCheckSum[i] = 0;
		for(j=0;j<3;j++)
		{
			tdw1 = ReadSampleData( r_ChkSum0+i );
			tdw2 = ReadSampleData( r_ChkSum0+i);
			if(tdw1 == tdw2)
			{
				dwSampleCheckSum[i] = tdw1;
				break;
			}
		}
	}
}

//-----------------------------------------------
//��������: ������оƬУ������Ч���
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע:
//-----------------------------------------------
BOOL CheckSampleChipCheckSum(void)
{
	BYTE i;
	DWORD tdw1, tdw2;

	for(i=0;i<3;i++)
	{
		tdw1 = ReadSampleData( r_ChkSum0+i );
		tdw2 = ReadSampleData( r_ChkSum0+i);
		if( (tdw1!=dwSampleCheckSum[i]) && (tdw2!=dwSampleCheckSum[i]) )
		{
			return FALSE;
		}
		
	}
	return TRUE;
}
//-----------------------------------------------
//��������: ���ݲ�ͬ����ͺŻ�ȡ����ѹֵ
//
//����:		��	 
//			
//����ֵ: 	��������ѹֵ
//		    
//��ע:���ѹ��2.2����3λС����ÿ�θ��µ�ѹʱ����
//-----------------------------------------------
DWORD CalMaxVoltage(void)
{
	DWORD VoltageLimit;
	
	//���޵�ѹ�޶�Ϊ���ѹ��2.2��
	if( MeterTypesConst == METER_3P4W )
	{
		VoltageLimit = 484000;
	}
	else if( MeterTypesConst == METER_3P3W )
	{
		if(MeterVoltageConst == METER_380V)
		{
			VoltageLimit = 836000;
		}
		else
		{
			VoltageLimit = 484000;
		}
	}
	else//ZT 
	{
		VoltageLimit = 484000;
	}
	
	return VoltageLimit;
}


//-----------------------------------------------
//��������: �ϵ��ȡ��ѹ(A��B��C��)
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע:
//-----------------------------------------------
static void PowerUpReadVoltage(void)
{
	BYTE Cmd;
	DWORD tdw,VoltageLimit;
	bySampleStep = 0x04;
	Read_UIP();
	bySampleStep = 0;
}


//-----------------------------------------------
//��������: ��ȡ����оƬ���ܼĴ���
//
//����:		Type[in] ���ܼĴ�����ַ	 
//			
//����ֵ: 	�����������
//		    
//��ע:��෵��һ���ֽڣ��������80A��3200���峣����Ӧ��������Ҫ��
//-----------------------------------------------
//static BYTE GetSampleEnergyPulse(BYTE Type)
//{
//	DWORD tdw1,tdw2;
//	
//	//Type��r_BckReg������ReadMeasureData��ȡ��r_BckReg���ص���7126A0���˴���Ҫȷ�� !!!!!!
//	tdw1 = ReadMeasureData( Type );
//	Link_ReadSampleReg(r_BckReg,(BYTE*)&tdw2);
//	
//	if( (tdw1==tdw2) && (tdw1<0xff) )
//	{
//		return ((BYTE)tdw1);
//	}
//	else
//	{
//		api_WriteSysUNMsg(EVENT_7026E_PULS_ERR);
//	}
//	
//	return 0;
//}

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
static WORD CaleUnbalanceRate(BYTE Type, long *Buf)
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
	
	if( Type == 0 )
	{
		//�����ٽ��ѹ 60%Un��3λС����
		dwMin = wStandardVoltageConst * 100 * 6 / 10;
	}
	else
	{
		//����5%Ib��4λС����
		dwMin = wMeterBasicCurrentConst * 10 * 5 / 100;
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

//---------------------------------------------------------------
//��������: ��ʼ��оƬ�¶��Զ���������
//
//����:     ��
//                   
//����ֵ:   FALSE
//		   TRUE
//
//��ע:   
//---------------------------------------------------------------
BYTE InitMChipVrefgain(void)
{
	BYTE Result=TRUE;
	#warning: "����://δд--��ا��";
	return Result;
}
//---------------------------------------------------------------
//��������: �ж������Ƿ��ڵ�ѹ��ƽ��״̬
//
//����:    ���� 0 1 2��A\B\C��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL JudgeUnBalance( BYTE Phase )
{
	if( Phase >= MAX_PHASE )
	{
		return FALSE;
	}
	
	//�����ѹ����Un���� ������ 0.95Un - 1.05Un
	if((RemoteValue.U[Phase].d < (wStandardVoltageConst * 100 * 95 / 100))
	   ||(RemoteValue.U[Phase].d > (wStandardVoltageConst * 100 * 105 / 100)))
	{
		return FALSE;
	}
	//��ѹ��ƽ����10Itr����⣬
	if( MeterTypesConst == METER_ZT )
	{
		//�����������10Itr(Ib)���� ������ 0.95Ib - 1.05Ib
		if((RemoteValue.I[Phase].d < (wMeterBasicCurrentConst * 10 * 95 / 100))
		   ||(RemoteValue.I[Phase].d > (wMeterBasicCurrentConst * 10 * 105 / 100)))
		{
			return FALSE;
		}
	}
	else
	{
		//�����������10Itr(50%Ib)���� ������ 0.95Ib - 1.05Ib
		if((RemoteValue.I[Phase].d < (wMeterBasicCurrentConst / 2 * 10 * 95 / 100))
		   ||(RemoteValue.I[Phase].d > (wMeterBasicCurrentConst / 2 * 10 * 105 / 100)))
		{
			return FALSE;
		}
	}
	
	//�������С��0.85 ������
	if(labs( RemoteValue.Cos[Phase + 1].l ) < 8500) //���Cos[0]��������� ������Ҫ��һ
	{
		return FALSE;
	}

	switch(Phase)
	{
		case 0: //A��
			//���������ѹ����һ�೬��10% �����в���
			if((RemoteValue.U[1].d > (wStandardVoltageConst * 100 * 10 / 100))
		     ||(RemoteValue.U[2].d > (wStandardVoltageConst * 100 * 10 / 100)))
			{
				return FALSE;
			}
			//���������������һ�೬��10% �����в���
			if((RemoteValue.I[1].d > (wMeterBasicCurrentConst * 10 * 10 / 100))
			 ||(RemoteValue.I[2].d > (wMeterBasicCurrentConst * 10 * 10 / 100)))
			{
				return FALSE;
			}
			break;
			
		case 1: //B��
			//���������ѹ����һ�೬��10% �����в���
			if((RemoteValue.U[0].d > (wStandardVoltageConst * 100 * 10 / 100))
		     ||(RemoteValue.U[2].d > (wStandardVoltageConst * 100 * 10 / 100)))
			{
				return FALSE;
			}
			//���������������һ�೬��10% �����в���
			if((RemoteValue.I[0].d > (wMeterBasicCurrentConst * 10 * 10 / 100))
			 ||(RemoteValue.I[2].d > (wMeterBasicCurrentConst * 10 * 10 / 100)))
			{
				return FALSE;
			}
			break;
			
		case 2: //C��
			//���������ѹ����һ�೬��10% �����в���
			if((RemoteValue.U[0].d > (wStandardVoltageConst * 100 * 10 / 100))
			 ||(RemoteValue.U[1].d > (wStandardVoltageConst * 100 * 10 / 100)))
			{
				return FALSE;
			}
			//���������������һ�೬��10% �����в���
			if((RemoteValue.I[0].d > (wMeterBasicCurrentConst * 10 * 10 / 100))
			 ||(RemoteValue.I[1].d > (wMeterBasicCurrentConst * 10 * 10 / 100)))
			{
				return FALSE;
			}
			break;
		default:
			return FALSE;
			break;
	}
	
	return TRUE;
}
//---------------------------------------------------------------
//��������: ����ֱ��ż��г���ֶβ���ϵ��
//
//����: 	Type��
//				0��0.5L   1��1.0L
//			Slope��
//				һ�κ���б��
//			Offset
//				һ�κ���ƫ��
//                   
//����ֵ:  
//
//��ע:   
//---------------------------------------------------------------
static void CalHD2Para( BYTE Type, float *Slope, SWORD *Offset )
{
	BYTE i = 0, EffectivePoint = 0;
	SWORD Data = 0, Current = 0;
	WORD ( *Pointer )[2];
	
	//0.5L
	if( Type == 0 )
	{
		Pointer = SampleHalfWavePara.Coe;
	}//1.0L
	else
	{
		Pointer = SampleHalfWavePara.Gain;
	}
	
	EffectivePoint = 0;
	for( i = 0; i < 3; i++ )
	{
		if( ((*(*(Pointer + i) + 1)) >= HD2CurrentRangeMin)
			&& ((*(*(Pointer + i) + 1)) <= HD2CurrentRangeMax) )
		{
			EffectivePoint++;
		}
	}
	//1��У��ȫ��ʹ�õ�һ����Ĳ���ֵ
	if( EffectivePoint == 1 )
	{
		Slope[0] = 0;
		Offset[0] = (SWORD)(*(*Pointer));
		Slope[1] = 0;
		Offset[1] = (SWORD)(*(*Pointer));
	}//2��У������һ��ֱ��
	else if( EffectivePoint == 2 )
	{
		Data = (SWORD)(*(*(Pointer + 1))) - (SWORD)(*(*Pointer));
		Current = (SWORD)(*(*(Pointer + 1) + 1)) - (SWORD)(*(*(Pointer) + 1));
		Slope[0] = (float)Data / Current;
		Offset[0] = (SWORD)((SWORD)(*(*Pointer)) - (Slope[0] * (SWORD)(*(*(Pointer) + 1))));
		Slope[1] = Slope[0];
		Offset[1] = Offset[0];
	}//3��У����������ֱ��
	else if( EffectivePoint == 3 )
	{
		Data = (SWORD)(*(*(Pointer + 1))) - (SWORD)(*(*Pointer));
		Current = (SWORD)(*(*(Pointer + 1) + 1)) - (SWORD)(*(*(Pointer) + 1));
		Slope[0] = (float)Data / Current;
		Offset[0] = (SWORD)((SWORD)(*(*Pointer)) - (Slope[0] * (SWORD)(*(*(Pointer) + 1))));
		
		Data = (SWORD)(*(*(Pointer + 2))) - (SWORD)(*(*(Pointer + 1)));
		Current = (SWORD)(*(*(Pointer + 2) + 1)) - (SWORD)(*(*(Pointer + 1) + 1));
		Slope[1] = (float)Data / Current;
		Offset[1] = (SWORD)((SWORD)(*(*(Pointer + 1))) - (Slope[1] * (SWORD)(*(*(Pointer + 1) + 1))));
	}
	else
	{
		Slope[0] = 0;
		Offset[0] = 0;
		Slope[1] = 0;
		Offset[1] = 0;
	}
}

//---------------------------------------------------------------
//��������: �����ֶβ������޹��������¶Ȳ�������
//
//����: 	��
//                   
//����ֵ:    ��
//
//��ע:   
//---------------------------------------------------------------
void SampleDivisionRegulate(void)
{
	SWORD 		Coe[MAX_PHASE], Gain[MAX_PHASE], TempGain, GainOffset[2], CoeOffset[2];
	BYTE 		uci, ucDivNo[MAX_PHASE], bTmpFlag,TmpCosGainFlag,Num9Count;
	BOOL 		bTmp[MAX_PHASE], OpenUnBalance, OpenHalfWave[3];
	float		GainSlope[2], CoeSlope[2];
	static long LastCurrentValue[3] = { 0, 0, 0 };
	WORD wTemp = 0,wTemp1 = 0;
	static BYTE DelaySwtichFlag = FALSE;
	BYTE FlagSampleChipSum = 0;
	
	if((HalfWaveStatusFlag == TRUE) && (MeterTypesConst == METER_ZT))
	{
		//����ֱ��ż��г������ϵ��
		CalHD2Para( 1, GainSlope, GainOffset );
		//����ֱ��ż��г�����ϵ��
		CalHD2Para( 0, CoeSlope, CoeOffset );
	}
	
	
    TmpCosGainFlag = FALSE;
    Num9Count = 0;
    if( CheckSampleChipSum() == FALSE)
	{
		FlagSampleChipSum = 0x55;	
	}	
	
	for(uci = 0; uci < MAX_PHASE; uci++)
	{
		bTmp[uci] = FALSE;
		
		if( (MeterTypesConst==METER_3P3W) && (uci==1) )
		{
			continue;
		}
		
		ucDivNo[uci] = GetRowSampleSegmentNo(uci);
		if(ucDivNo[uci] == 0xff)
		{
			return;
		}
		
		//�ж��Ƿ���Ҫ��������ȡ��������ѹ��ƽ��
		OpenUnBalance = FALSE;
		if(SelVolUnbalanceGain == YES)
		{
			if(((JudgeUnBalance( uci ) == TRUE)&&(UnBalanceGainFlag[uci]==FALSE))
			 ||((JudgeUnBalance( uci ) == FALSE)&&(UnBalanceGainFlag[uci]==TRUE)))
			{
				OpenUnBalance = TRUE;
			}
		}
		
		//�ж��Ƿ���Ҫ�����벨���� 
		if(( HalfWaveStatusFlag == TRUE )&& (MeterTypesConst == METER_ZT))
		{
			OpenHalfWave[uci] = FALSE;
			if(uci == 0)
			{
				//ֱ��ż��г���ж�
				JudgeHalfWaveRegulate();
			}
			//����ϴεĲ���״̬�ͱ��εõ���״̬��һ�� ����һ�β���
			if( (HalfWaveUpdateFlag[uci] != HalfWaveCoeFlag[uci]) 
				|| ( labs(labs( RemoteValue.I[uci].l ) - LastCurrentValue[uci]) >= HD2CurrentDiffValue ) )
			{
				OpenHalfWave[uci] = TRUE;
			}
		}
		else
		{
			HalfWaveUpdateFlag[uci] = FALSE;
			OpenHalfWave[uci] = FALSE; 
		}

		//��ȡ����ϵ��
		if( (ucDivNo[uci] != g_ucNowSampleDivisionNo[uci]) || (OpenUnBalance == TRUE) || (OpenHalfWave[uci] == TRUE) || (TmpCosGainFlag == TRUE))
		{		
			//�ֶβ���
			if(SelDivAdjust == YES)
			{
				//���油��
				Gain[uci] = SampleAdjustParaDiv.PGainDiv[uci][ucDivNo[uci]];
				if(Gain[uci] == 0) //����õ�ϵ��Ϊ�� ����Ĭ��ϵ��
				{
					Gain[uci] = (short)OriginalPara.PGainCoe[uci];
				}
				
				//��ǲ���
				Coe[uci] = SampleAdjustParaDiv.PhsRegDiv[uci][ucDivNo[uci]];
				if(Coe[uci] == 0) //����õ�ϵ��Ϊ�� ����Ĭ��ϵ��
				{
					if( ( (ucDivNo[uci] < 1) && ( MeterCurrentTypesConst == CURR_100A ))
					 || ( (ucDivNo[uci] < 1) && ( MeterCurrentTypesConst == CURR_60A ) ) 
					 || ( (ucDivNo[uci] < 2) && ( MeterCurrentTypesConst == CURR_1A ) )
					 || ( (ucDivNo[uci] < 4) && ( MeterCurrentTypesConst == CURR_6A ) ) )  //�����
					{
						Coe[uci] = (short)OriginalPara.AngCoe[0][uci];
					}
					else
					{
						Coe[uci] = (short)OriginalPara.AngCoe[1][uci];
					}
				}
			}
			//��ѹ��ƽ�ⲹ��
			if(SelVolUnbalanceGain == YES)
			{
				//ֻҪ���ڵ�ѹ��ƽ��״̬ ��Ҫ���� ��Ȼ�ᱻд��ȥ
				if(JudgeUnBalance( uci ) == TRUE)
				{
					UnBalanceGainFlag[uci] = TRUE;
					if(SampleManualModifyPara.VolUnbalanceGain[uci] != 0)
					{
						Gain[uci] += SampleManualModifyPara.VolUnbalanceGain[uci];
						bTmp[uci] = TRUE;
					}
				}
				else
				{
					UnBalanceGainFlag[uci] = FALSE;
				}
			}
			//ֱ��ż��г������
			if((HalfWaveStatusFlag == TRUE) && (MeterTypesConst == METER_ZT))
			{
				if(HalfWaveUpdateFlag[uci] != 0)
				{
					//1.0L���油��
					if( HalfWaveUpdateFlag[uci] == 2 )
					{
						if( (labs( RemoteValue.I[uci].l ) / 1000) > SampleHalfWavePara.Gain[1][1] )
						{
							Gain[uci] += (SWORD)(GainSlope[1] * (labs( RemoteValue.I[uci].l ) / 1000) + GainOffset[1]);
						}
						else
						{
							Gain[uci] += (SWORD)(GainSlope[0] * (labs( RemoteValue.I[uci].l ) / 1000) + GainOffset[0]);
						}
					}//0.5L��ǲ���
					else if( HalfWaveUpdateFlag[uci] == 1 )
					{
						if( (labs( RemoteValue.I[uci].l ) / 1000) > SampleHalfWavePara.Coe[1][1] )
						{
							Coe[uci] += (SWORD)(CoeSlope[1] * (labs( RemoteValue.I[uci].l ) / 1000) + CoeOffset[1]);
						}
						else
						{
							Coe[uci] += (SWORD)(CoeSlope[0] * (labs( RemoteValue.I[uci].l ) / 1000) + CoeOffset[0]);
						}
					}
					//������һ�β���ʱ�ĵ���ֵ
					LastCurrentValue[uci] = labs( RemoteValue.I[uci].l );
				}
				
				HalfWaveCoeFlag[uci] = HalfWaveUpdateFlag[uci];
			}
			g_ucNowSampleDivisionNo[uci] = ucDivNo[uci];
			bTmp[uci] = TRUE; 
		}	
	}

	
	bTmpFlag = FALSE;
	for(uci = 0; uci < MAX_PHASE; uci++)
	{
		if(bTmp[uci] == TRUE)
		{
			//����ͷ����й���������ƫ������
			if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE ) //����
			{
				if(ucDivNo[uci] < 9)
				{
					Gain[uci] += SampleManualModifyPara.ManualModifyForRA[ucDivNo[uci]];
				}
			}
			else //����
			{
				if(ucDivNo[uci] < 9)
				{
					Gain[uci] += SampleManualModifyPara.ManualModifyForWard[ucDivNo[uci]];
				}
			}
			SampleSpecCmdOp( 0xa6 );
			//����ϵ��д��Ĵ���
			WriteSampleReg( w_GPA + uci, Gain[uci] );
			
			//���ϵ��д��Ĵ���
			WriteSampleReg( w_GphsA0 + uci*3, Coe[uci] );
			WriteSampleReg( w_GphsA1 + uci*3, Coe[uci] );
            WriteSampleReg( w_GphsA2 + uci*3, Coe[uci] );
			
			//�޹�����
			TempGain = Gain[uci];
			TempGain += SampleManualModifyPara.ManualModifyQGain[ uci ][ ucDivNo[uci] ];
			WriteSampleReg( w_GPA + uci,(DWORD)TempGain); //����ϵ��д���޹��Ĵ���
			SampleSpecCmdOp(0x00);
			bTmpFlag = TRUE;
		}
	}
	//�ϵ����������һ����Ҫ����ʱ���¼���оƬУ���
	if((bySamplePowerOnTimer <= 28) && (bTmpFlag == TRUE))
	{
		//�Ƿ��м���ʹ��󣬱�����������о�Ĵ�������δ������
		if( FlagSampleChipSum != 0x55)
		{
			//���¼���оƬУ���
			GetSampleChipCheckSum();
		}
	}
}

//-----------------------------------------------
//��������: ˢ���˶�����
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע:ÿ������ÿ��ˢ��һ�Σ�һ�δ�ѭ��ˢ��һ������,ȫ����һ�飬��ʱ13.96ms��680kbps��
//-----------------------------------------------
static void Read_UIP(void)
{
	BYTE i,DataSign;
	BYTE bySamplePulseNum;	
	DWORD tdw,tdBuf[16];
	DWORD UpLimit,DownLimit;
	long tl;
	float tf;
	if( bySampleStep == 0xff )
	{
		return;
	}
	
	switch( bySampleStep )
	{
		case 0x00://���ࡢ����ȱ�־״̬,�ȶ���ˢ��P,Q,Sʱ����	
			dwSampleSFlag = ReadSampleData( r_Noload );
			// �жϷ��ţ�����������
			break;
		case 0x01://���й����� �����ʽ
			Link_ReadSampleRegCont( r_PA, 4, tdBuf);
			//���ʵ�λW������0.001W,����ϵ�� K=2.592*10^10/(HFconst*EC*2^23)
			//��ϵ���������Q,Sʱ��Ҫ��
			gfPowerCorr = 386.238 * 1000 / HFCONST_DEF / (api_GetActiveConstant());
			for(i=0;i<4;i++)
			{
				if(i==0)
				{
					tdw = tdBuf[3];
				}
				else
				{
					tdw = tdBuf[1-1];
				}
				
				if( tdw > TWO_POW31 )
				{
					tdw = TWO_POW32 - tdw;
					tl = (long)tdw*(-1);					
				}
				else
				{
					tl = tdw;
				}
				tl = (long)(gfPowerCorr * tl);
				if(i==0)
				{
					tl *=2;//������*2
					if( MeterTypesConst == METER_ZT )
					{
						DownLimit = (DWORD)wStandardVoltageConst*wMeterBasicCurrentConst * 2 * 6 / 100000;
					}
					else
					{
						DownLimit = (DWORD)wStandardVoltageConst*wMeterBasicCurrentConst *1* 6 / 100000;
					}
				}
				else
				{
					DownLimit = 0;
				}
				if( labs(tl) > DownLimit )
				{
					if( tl < 0 )
					{
						//���ú��෴���־
						api_SetSysStatus( eSYS_STATUS_OPPOSITE_P+i );
					}
					else if( tl > 0 )
					{
						api_ClrSysStatus( eSYS_STATUS_OPPOSITE_P+i );
					}
					if(i==0)
					{
						api_UptCurDemandStatus( 0, 1);//���µ�ǰ�й�����״̬,���ڸ��±�־֮��
					}
					
				}
				else
				{
					if(i==0)
					{
						api_UptCurDemandStatus( 0, 0 );//���µ�ǰ�й�����״̬,���ڸ��±�־֮��
					}
					tl = 0;
				}
				RemoteValue.P[i].l = tl;
				gfPQSaverage[0][i] += (float)RemoteValue.P[i].l;
				byPQSaverageCnt[0][i]++;
			}
			break;
		case 0x02://���޹����� �����ʽ
			Link_ReadSampleRegCont( r_QA, 4, tdBuf);
			//���ʵ�λW������0.001W,����ϵ�� K=2.592*10^10/(HFconst*EC*2^23)
			//��ϵ���������Q,Sʱ��Ҫ��
			gfPowerCorr = 386.238 * 1000 / HFCONST_DEF / (api_GetActiveConstant());
			for(i=0;i<4;i++)
			{
				if(i==0)
				{
					tdw = tdBuf[3];
				}
				else
				{
					tdw = tdBuf[1-1];
				}
				
				if( tdw > TWO_POW31 )
				{
					tdw = TWO_POW32 - tdw;
					tl = (long)tdw*(-1);					
				}
				else
				{
					tl = tdw;
				}
				tl = (long)(gfPowerCorr * tl);
				if(i==0)
				{
					tl *=2;//������*2
					if( MeterTypesConst == METER_ZT )
					{
						DownLimit = (DWORD)wStandardVoltageConst*wMeterBasicCurrentConst * 2 * 6 / 100000;
					}
					else
					{
						DownLimit = (DWORD)wStandardVoltageConst*wMeterBasicCurrentConst *1* 6 / 100000;
					}
				}
				else
				{
					DownLimit = 0;
				}
				if( labs(tl) > DownLimit )
				{
					if( tl < 0 )
					{
						//���ú��෴���־
						api_SetSysStatus( eSYS_STATUS_OPPOSITE_Q+i );
					}
					else if( tl > 0 )
					{
						api_ClrSysStatus( eSYS_STATUS_OPPOSITE_Q+i );
					}
					if(i==0)
					{
						api_UptCurDemandStatus( 1, 1);//���µ�ǰ�޹�����״̬,���ڸ��±�־֮��
					}
					
				}
				else
				{
					if(i==0)
					{
						api_UptCurDemandStatus( 1, 0 );//���µ�ǰ�޹�����״̬,���ڸ��±�־֮��
					}
					tl = 0;
				}
				RemoteValue.Q[i].l = tl;
				gfPQSaverage[1][i] += (float)RemoteValue.Q[i].l;
				byPQSaverageCnt[1][i]++;
			}		
			break;
		case 0x03://�����ڹ��� �����ʽ
			Link_ReadSampleRegCont( r_SA, 4, tdBuf);
			//���ʵ�λW������0.001W,����ϵ�� K=2.592*10^10/(HFconst*EC*2^23)
			//��ϵ���������Q,Sʱ��Ҫ��
			gfPowerCorr = 386.238 * 1000 / HFCONST_DEF / (api_GetActiveConstant());
			for(i=0;i<4;i++)
			{
				if(i==0)
				{
					tdw = tdBuf[3];
				}
				else
				{
					tdw = tdBuf[1-1];
				}
				
				if( tdw > TWO_POW31 )
				{
					tdw = TWO_POW32 - tdw;
					tl = (long)tdw*(-1);					
				}
				else
				{
					tl = tdw;
				}
				tl = (long)(gfPowerCorr * tl);
				if(i==0)
				{
					tl *=2;//������*2
					//tf = (float)RemoteValue.P[0].l*RemoteValue.P[0].l+(float)RemoteValue.Q[0].l*RemoteValue.Q[0].l;
					//tl = (long)sqrt(tf);
				}
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P+i) == TRUE )
				{
					tl *= -1;
				}
				RemoteValue.S[i].l = tl;
				gfPQSaverage[2][i] += (float)RemoteValue.S[i].l;
				byPQSaverageCnt[2][i]++;
			}
			break;
		case 0x04://����ѹ �����ʽ
			Link_ReadSampleRegCont( r_RMSUA, 3, tdBuf);
			UpLimit = CalMaxVoltage();//�������
			DownLimit =  (DWORD)wCaliVoltageConst * 100 * 2 / 100;//��С����@@@@@������
			for(i=0;i<3;i++)
			{
				tdw = tdBuf[i]*1000/TWO_POW13;
                if(tdw>=UpLimit)
                {
                 tdw = UpLimit;
                }
                if(tdw < DownLimit)
                {
				 tdw = 0;
                }
				RemoteValue.U[i].d = tdw;
			}
			//��ѹ��ƽ����
			RemoteValue.Uunbalance.w = CaleUnbalanceRate( 0, (long*)&RemoteValue.U[0].l );
			break;
		case 0x05://����ֵ �����ʽ
			Link_ReadSampleRegCont( r_RMSIA, 6, tdBuf);
			
			if( MeterTypesConst == METER_ZT )
			{
				DownLimit = (DWORD)wMeterBasicCurrentConst * 10 * 2 / 1000;
			}	
			else 
			{
				DownLimit = (DWORD)wMeterBasicCurrentConst * 10 * 1 / 1000;
			}
			for(i=0;i<3;i++)
			{
				tf = (float)tdBuf[i]*10000/TWO_POW13;
				tf /= ((float)(6 * CURRENT_VALUE) / 5);
				RemoteValue.I[i].l = (long)((tf * wMeterBasicCurrentConst) / 1000 + 0.5);

				if( labs(RemoteValue.I[i].l) < DownLimit )
				{
					RemoteValue.I[i].l = 0;
				}
				
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A+i) == TRUE )
				{
					RemoteValue.I[i].l *= -1;
				}
			}
			//������ƽ����
			RemoteValue.Iunbalance.w = CaleUnbalanceRate( 1, (long*)&RemoteValue.I[0].l );
			//���ߵ���
			if(SelZeroCurrentConst == YES)
			{
				tdw = tdBuf[3];
				tf = (float)tdw * 10000 / TWO_POW13;
				//����50mvʱ��Vrms/2^13=60
				tf /= ((float)(6 * ZERO_CURRENT_VALUE) / 5);
				RemoteValue.I[3].l = (long)(tf * ((float)wMeterBasicCurrentConst / 1000) + 0.5);
				
				if( labs(RemoteValue.I[3].l) < DownLimit )
				{
					RemoteValue.I[3].l = 0;
				}
			}
			else
			{
				RemoteValue.I[3].l = 0;
			}
			if(MeterTypesConst != METER_3P3W)
			{
				tdw = tdBuf[5];
				tf = (float)tdw*10000/TWO_POW12;
				tf /= ((float)(6 * CURRENT_VALUE) / 5);
				RemoteValue.I[4].l = (long)(tf * ((float)wMeterBasicCurrentConst / 1000) + 0.5);
				
				if( labs(RemoteValue.I[4].l) < DownLimit )
				{
					RemoteValue.I[4].l = 0;
				}
			}
			else
			{
				RemoteValue.I[4].l = 0;
			}
			#if( SEL_EVENT_LOST_ALL_V == YES )
			if( byPowerOnFlag != 0x55 )
			{
				api_SetLostAllVCurrent(&RemoteValue.I[0].l);
			}
			#endif
			break;
		#if( SEL_MEASURE_FREQ == YES )
		case 0x06://��Ƶ�� �����ʽ
			tdw = ReadSampleData( r_FreqU );
			tdw = tdw * 100 / TWO_POW13;//����2λС��	
			RemoteValue.Freq.d = tdw;	
			break;
		#endif
		case 0x07://����������ֵ �����ʽ
			Link_ReadSampleRegCont( r_PFA, 4, tdBuf);
			for(i=0;i<4;i++)
			{
				tdw = tdBuf[i];
				//800000ʱӦ��֤�Ǹ�ֵ������Ӧ��
				if( tdw >= TWO_POW23 )
				{
					tl = (long)tdw - TWO_POW24;
				}
				else
				{
					tl = tdw;
				}
				//����4λС��
				tl = (long)((float)tl*10000 / TWO_POW23);
				if( labs(tl) > 10000 )
				{
					tl = 10000;
				}
				if( i<3 )
				{
					if( ((RemoteValue.Q[i+1].l == 0)&&(RemoteValue.P[i+1].l == 0 )) 
					|| (RemoteValue.I[i].l == 0) )
					{
						RemoteValue.Cos[i+ 1].l = 10000;
					}
					else
					{
						RemoteValue.Cos[i+1].l = tl;   
					}
				}
				else
				{
					if( ( (RemoteValue.Q[0].l == 0)&&(RemoteValue.P[0].l == 0))
					|| ( (RemoteValue.I[0].l == 0)&&(RemoteValue.I[1].l == 0)&&(RemoteValue.I[2].l == 0)) )
					{
						RemoteValue.Cos[0].l = 10000;
					}
					else
					{
						RemoteValue.Cos[0].l = tl;
					}
				}
			}
			break;
		#if( SEL_VOL_ANGLE_FUNC == YES )
		case 0x08://����ѹ������� �����ʽ
			Link_ReadSampleRegCont( r_PAUA, 6, tdBuf);
			for(i=0;i<3;i++)
			{
				tdw = tdBuf[i];	
				tf = (float)tdw * 10 / TWO_POW20;//����1λС��
				RemoteValue.YUaUbUcAngle[i].l = (long)(tf*180);
			}
			for(i=0;i<3;i++)
			{
				tdw = tdBuf[3+i];	
				tf = (float)tdw * 10 / TWO_POW20;//����1λС��
				tl = (long)(tf*180);
				tl = tl-RemoteValue.YUaUbUcAngle[i].l;
				RemoteValue.YUIAngle[i].l = (long)((tl % 3600) + 3600) % 3600;
			}
			break;
		#endif
		case 0x09://�������й����� �����ʽ
			Link_ReadSampleRegCont( r_EPPA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			if(bySamplePulseNum != 0)
			{			
				api_WritePulseEnergy(PHASE_ALL+P_ACTIVE,bySamplePulseNum);// ����
			}

			//���������ۼ��õĵ��й�������������������������ʱ������
			//ע����ʹ����������Ϊ0��ҲҪ���£���Ϊ��������ʱ���������
			api_UpdateDemandEnergyPulse( 0, bySamplePulseNum );//ʹ�ø�Ƶ����
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				api_WritePulseEnergy(P_ACTIVE+(i<<12),bySamplePulseNum);	// ����				
			}
			#endif
			break;
		case 0x10://�������й����� �����ʽ
			Link_ReadSampleRegCont( r_ENPA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			if(bySamplePulseNum != 0)
			{			
				api_WritePulseEnergy(PHASE_ALL+N_ACTIVE,bySamplePulseNum);// ����
			}

			//���������ۼ��õĵ��й�������������������������ʱ������
			//ע����ʹ����������Ϊ0��ҲҪ���£���Ϊ��������ʱ���������
			api_UpdateDemandEnergyPulse( 0, bySamplePulseNum );//ʹ�ø�Ƶ����
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				api_WritePulseEnergy(N_ACTIVE+(i<<12),bySamplePulseNum);	// ����				
			}
			#endif
			break;
		case 0x11://�������޹����� �����ʽ
			Link_ReadSampleRegCont( r_EPQA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			
			//�޹�Ϊ�����й�Ϊ����һ���ޣ��й�Ϊ���ڶ����ޣ�
			if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE) 
			{
				api_WritePulseEnergy(PHASE_ALL+RACTIVE2,bySamplePulseNum);
			}
			else
			{
				api_WritePulseEnergy(PHASE_ALL+RACTIVE1,bySamplePulseNum);	
			}
			//���������ۼ��õĵ��޹�������������������������ʱ������
			//ע����ʹ����������Ϊ0��ҲҪ���£���Ϊ��������ʱ���������
			api_UpdateDemandEnergyPulse( 1, bySamplePulseNum );
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				
				//�޹�Ϊ�����й�Ϊ����һ���ޣ��й�Ϊ���ڶ�����
				if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P+i ) == TRUE)
				{
					api_WritePulseEnergy((i<<12)+RACTIVE2,bySamplePulseNum);
				}
				else
				{
					api_WritePulseEnergy((i<<12)+RACTIVE1,bySamplePulseNum);
				}				
			}
			#endif
			break;
		case 0x12://�������޹����� �����ʽ
			Link_ReadSampleRegCont( r_ENQA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			
			//�޹�Ϊ�����й�Ϊ���������ޣ��й�Ϊ���������ޣ�
			if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE) 
			{
				api_WritePulseEnergy(PHASE_ALL+RACTIVE3,bySamplePulseNum);
			}
			else
			{
				api_WritePulseEnergy(PHASE_ALL+RACTIVE4,bySamplePulseNum);	
			}
			//���������ۼ��õĵ��޹�������������������������ʱ������
			//ע����ʹ����������Ϊ0��ҲҪ���£���Ϊ��������ʱ���������
			api_UpdateDemandEnergyPulse( 1, bySamplePulseNum );
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				
				//�޹�Ϊ�����й�Ϊ���������ޣ��й�Ϊ���������ޣ�
				if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P+i ) == TRUE) 
				{
					api_WritePulseEnergy((i<<12)+RACTIVE3,bySamplePulseNum);
				}
				else
				{
					api_WritePulseEnergy((i<<12)+RACTIVE4,bySamplePulseNum);	
				}				
			}
			#endif
			break;		
		case 0x13://�����ڵ��� �����ʽ
			Link_ReadSampleRegCont( r_ESA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			
			//���ڵ��ܷ�����й����ܷ���һ��
			if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE)
			{
				api_WritePulseEnergy(PHASE_ALL+APPARENT_N,bySamplePulseNum);
			}
			else
			{
				api_WritePulseEnergy(PHASE_ALL+APPARENT_P,bySamplePulseNum);
			}
			//���������ۼ��õĵ��޹�������������������������ʱ������
			//ע����ʹ����������Ϊ0��ҲҪ���£���Ϊ��������ʱ���������
			api_UpdateDemandEnergyPulse( 1, bySamplePulseNum );
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				
				//���ڵ��ܷ�����й����ܷ���һ��
				if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P+i ) == TRUE)
				{
					api_WritePulseEnergy((i<<12)+APPARENT_N,bySamplePulseNum);
				}
				else
				{
					api_WritePulseEnergy((i<<12)+APPARENT_P,bySamplePulseNum);
				}				
			}
			#endif
			break;
		case 14://�¶�
			tdw = ReadSampleData( r_TPSD );
			tdw&=0x00FF;
			//��ʵ�¶�ֵ = -0.0028*TPS_data + 16��
			if(tdw>128)
			{
				tdw = 256-tdw;
				tdw = (DWORD)(2500+(0.726*tdw)*100);
			}
			else
			{
				tdw = (DWORD)(2500-(0.726*tdw)*100);
			}

			RemoteValue.Temperature = tdw;
            break;

		default:
			break;

	}
	
	bySampleStep++;

	if( bySampleStep >= 0x40 )//�Ĵ�����ַ�������0x40
	{
		bySampleStep = 0xff;
	}
	
	return;
}

//---------------------------------------------------------------
//��������: ˢ�µ���ֵ ����У��
//
//����:     ��
//                   
//����ֵ:   ��
//
//��ע:   
//---------------------------------------------------------------
void FreshCurrent( void )
{
	BYTE i;
	
	bySampleStep = r_RMSIA;
	for(i = 0; i < 3; i++)
	{
		Read_UIP();
	}
}

//-----------------------------------------------
//��������: �����Ĵ�������ΪĬ����ֵ
//
//����:		Type[in] 0--ƫ���� 1--������ 2--������	 
//			
//����ֵ: 	��
//		    
//��ע:
//-----------------------------------------------
void UseSampleDefaultPara(WORD Type)
{
	BYTE i;
	DWORD dwPara = 0;
	
	InitSampleCaliUI();
	
	if( Type == 0 )//ƫ������ TSampleOffsetPara
	{
		//CRCУ����ȷ��У���ʼ������ʼ�����ߵ���ƫ�ã���ֹ����У���ʼ���˲���
		if( lib_CheckSafeMemVerify( (BYTE *)&SampleOffsetPara, sizeof(TSampleOffsetPara), UN_REPAIR_CRC ) == TRUE )
		{
           	for(i=0;i<(sizeof(SampleAdjustAddr)/sizeof(SampleAdjustAddr[0]));i++)//��������У׼
			{
				if(SampleAdjustAddr[i] == w_RmsOffsetIN)
				{
					dwPara = SampleOffsetPara.Offset[i];
					break;
				}
			}
			memcpy( (BYTE*)&SampleOffsetPara, (BYTE*)&SampleOffsetDefData, sizeof(TSampleOffsetPara) );
			SampleOffsetPara.Offset[i] = dwPara;
		}
		else
		{
			memcpy( (BYTE*)&SampleOffsetPara, (BYTE*)&SampleOffsetDefData, sizeof(TSampleOffsetPara) );
		}
		//ʹ��Ĭ��ֵ���ܸ���CRC �����´�У�鲻�ܴ�EEPROM�ָ�
		//SampleOffsetPara.CRC32 = lib_CheckCRC32((BYTE *)&SampleOffsetPara, (sizeof(TSampleOffsetPara) - sizeof(DWORD)) );
	}
	else if( Type == 1 )//�������� TSampleCtrlPara
	{
		memcpy( (BYTE*)&SampleCtrlPara, (BYTE*)&SampleCtrlDefData, sizeof(TSampleCtrlPara) );

		for(i = 0; i < (sizeof(SampleCtrlPara)/sizeof(DWORD)); i++)
		{
			if((SampleCtrlAddr[i] == w_PStart)||(SampleCtrlAddr[i] == w_QStart)||(SampleCtrlAddr[i] == w_HarStart))
			{
				//���ѹ Ub���������� Ib������������ k��,�����������޵�60%����
			//Pstartup=INT[0.6*Ub*Ib*HFconst*EC* k��*2^23/(2.592*10^10)]		
				SampleCtrlPara.CtrlPara[i] = (WORD)((float)1.94180741 * (wStandardVoltageConst/10) * ((float)wMeterBasicCurrentConst / 1000) * HFCONST_DEF * (api_GetActiveConstant()) / 10000000);
				if( MeterTypesConst == METER_ZT )
				{
					//ֱͨ����Ϊ0.2%��������Ϊ0.1%
					SampleCtrlPara.CtrlPara[i] *= 2;
				}
			}
			
			if(SampleCtrlAddr[i] == w_HFConstAF)
			{
				SampleCtrlPara.CtrlPara[i] = HFCONST_DEF;
			}
			if(SampleCtrlAddr[i] == w_PGACON)
			{
				if(wSampleVolGainConst == 1)
				{
					SampleCtrlPara.CtrlPara[i] = 0x0000; //��ѹͨ��ȱʡ1������
				}		
				else if(wSampleVolGainConst == 2)
				{
					SampleCtrlPara.CtrlPara[i] = 0x0200; //��ѹͨ��ȱʡ2������
				}		
				else if(wSampleVolGainConst == 4)
				{
					SampleCtrlPara.CtrlPara[i] = 0x0400; //��ѹͨ��ȱʡ8������
				}		
				else if(wSampleVolGainConst == 8)
				{
					SampleCtrlPara.CtrlPara[i] = 0x0600; //��ѹͨ��ȱʡ8������
				}		
				else
				{
					SampleCtrlPara.CtrlPara[i] = 0x0200; //��ѹͨ��ȱʡ2������
				}	

				if(wSampleCurGainConst == 1)
				{
					SampleCtrlPara.CtrlPara[i] |= 0x0000;
				}
				else if(wSampleCurGainConst == 2)
				{
					SampleCtrlPara.CtrlPara[i] |= 0x0015;
				}
				else if(wSampleCurGainConst == 4)
				{
					SampleCtrlPara.CtrlPara[i] |= 0x002A;
				}
				else if(wSampleCurGainConst == 8)
				{
					SampleCtrlPara.CtrlPara[i] |= 0x003F;
				}
				else
				{
					SampleCtrlPara.CtrlPara[i] |= 0x0015;
				}
				
				//���ߵ���ADC��������
				#if(CUR_ZERO_CURRENT_GAIN == 1)
				SampleCtrlPara.CtrlPara[i] |= 0x0000;
				#elif(CUR_ZERO_CURRENT_GAIN == 4)
				SampleCtrlPara.CtrlPara[i] |= 0x0040;
				#elif(CUR_ZERO_CURRENT_GAIN == 8)
				SampleCtrlPara.CtrlPara[i] |= 0x0080;
				#elif(CUR_ZERO_CURRENT_GAIN == 16)
				SampleCtrlPara.CtrlPara[i] |= 0x00C0;
				#elif(CUR_ZERO_CURRENT_GAIN == 24)
				SampleCtrlPara.CtrlPara[i] |= 0x0100;
				#elif(CUR_ZERO_CURRENT_GAIN == 2)
				SampleCtrlPara.CtrlPara[i] |= 0x0140;
				#else
				���ô���
				#endif
			}
			
			if(SampleCtrlAddr[i] == w_ADCCFG)
			{
				if(MeterTypesConst==METER_3P3W)
				{
					SampleCtrlPara.CtrlPara[i] |= (1<<17);
				}
			}	
			if(SampleCtrlAddr[i] == w_EMUCFG)
			{
				if(MeterTypesConst==METER_3P3W)
				{
					SampleCtrlPara.CtrlPara[i] |= (1<<6);//B�಻��������������
					SampleCtrlPara.CtrlPara[i] |= (1<<10);//B�಻��������ѹ����
					SampleCtrlPara.CtrlPara[i] |= (3<<14);//B�಻������๦�ʼ���
				}
			}
		}
		
	}
	else//TSampleAdjustPara
	{
		//CRCУ����ȷ��У���ʼ������ʼ�����ߵ������棬��ֹ����У���ʼ���˲���
		if( lib_CheckSafeMemVerify( (BYTE *)&SampleAdjustPara, sizeof(TSampleAdjustPara), UN_REPAIR_CRC ) == TRUE )
		{
			dwPara = SampleAdjustPara.NGain;
			memcpy( (BYTE*)&SampleAdjustPara, (BYTE*)&SampleAdjustDefData, sizeof(TSampleAdjustPara) );
			SampleAdjustPara.NGain = dwPara;
		}
		else
		{
			memcpy( (BYTE*)&SampleAdjustPara, (BYTE*)&SampleAdjustDefData, sizeof(TSampleAdjustPara) );
		}
		//ʹ��Ĭ��ֵ���ܸ���CRC �����´�У�鲻�ܴ�EEPROM�ָ�
		//SampleAdjustPara.CRC32 = lib_CheckCRC32((BYTE *)&SampleAdjustPara, (sizeof(TSampleAdjustPara) - sizeof(DWORD)) );
	}
}


//-----------------------------------------------
//��������: ���RAM��У���Ĵ��������Ƿ���ȷ
//
//����:		�� 
//			
//����ֵ: 	��
//		    
//��ע:
//-----------------------------------------------
static void CheckSampleRamPara( void )
{
	WORD Result,Flag;
	
	Flag = 0;
	
	//У��ƫ��ϵ��
	if(SampleOffsetPara.CRC32 != lib_CheckCRC32((BYTE *)&SampleOffsetPara, (sizeof(TSampleOffsetPara) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleOffsetPara), sizeof(TSampleOffsetPara), (BYTE*)&SampleOffsetPara);
		if( Result != TRUE )
		{
			UseSampleDefaultPara(0);
			Flag = 0x55;
		}
	}

	//У��оƬ����ϵ��
	if(SampleCtrlPara.CRC32 != lib_CheckCRC32((BYTE *)&SampleCtrlPara, (sizeof(TSampleCtrlPara) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlPara), sizeof(TSampleCtrlPara), (BYTE*)&SampleCtrlPara);
		if( Result != TRUE )
		{
			UseSampleDefaultPara(1);
			Flag = 0x55;
		}
	}
	
	//У���Уϵ��
	if(SampleAdjustPara.CRC32 != lib_CheckCRC32((BYTE *)&SampleAdjustPara, (sizeof(TSampleAdjustPara) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustPara), sizeof(TSampleAdjustPara), (BYTE*)&SampleAdjustPara);
		if( Result != TRUE )
		{
			UseSampleDefaultPara(2);
			Flag = 0x55;
		}
	}
	
	//У�龫��ϵ��
	if(SelDivAdjust == YES)
	{
		//У�龫��Ĭ��ϵ��
		if(OriginalPara.CRC32 != lib_CheckCRC32((BYTE *)&OriginalPara,(sizeof(TOriginalPara) - sizeof(DWORD))))
		{
			memcpy( OriginalPara.PGainCoe, SampleAdjustPara.PGain, sizeof(OriginalPara.PGainCoe) );
			memcpy( OriginalPara.AngCoe, SampleAdjustPara.PhsReg, sizeof(OriginalPara.AngCoe) );
			OriginalPara.CRC32 = lib_CheckCRC32((BYTE *)&OriginalPara,(sizeof(TOriginalPara) - sizeof(DWORD)) );
		}
		
		if(SampleAdjustParaDiv.CRC32 != lib_CheckCRC32( (BYTE *)&SampleAdjustParaDiv, (sizeof(TSampleAdjustParaDiv) - sizeof(DWORD)) ))
		{
			Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleAdjustParaDiv ), sizeof(TSampleAdjustParaDiv), (BYTE *)&SampleAdjustParaDiv );
			if( Result != TRUE )
			{
				memset( (BYTE *)&SampleAdjustParaDiv, 0x00, sizeof(TSampleAdjustParaDiv) );
				Flag = 0x55;
			}
		}
	}
	
	//У���ֶ�����ϵ��
	if(SampleManualModifyPara.CRC32 != lib_CheckCRC32( (BYTE *)&SampleManualModifyPara, (sizeof(TSampleManualModifyPara) - sizeof(DWORD)) ))
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleManualModifyPara ), sizeof(TSampleManualModifyPara), (BYTE *)&SampleManualModifyPara );
		if(Result != TRUE)
		{
			memset( (BYTE *)&SampleManualModifyPara, 0x00, sizeof(TSampleManualModifyPara) );
		}
	}
	
	//У��ֱ��ż��г��ϵ��
	if(SampleHalfWavePara.CRC32 != lib_CheckCRC32( (BYTE *)&SampleHalfWavePara, (sizeof(TSampleHalfWavePara) - sizeof(DWORD)) ))
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWavePara ), sizeof(TSampleHalfWavePara), (BYTE *)&SampleHalfWavePara );
		if(Result != TRUE)
		{
			memcpy( (BYTE *)&SampleHalfWavePara, (BYTE *)SampleHalfWaveDefData, (sizeof(TSampleHalfWavePara) - sizeof(DWORD)) );
		}
	}
	
	//У��ֱ��ż��г���ϵ���ʱ�����
	if(SampleHalfWaveTimePara.CRC32 != lib_CheckCRC32( (BYTE *)&SampleHalfWaveTimePara, (sizeof(TSampleHalfWaveTimePara) - sizeof(DWORD)) ))
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWaveTimePara ), sizeof(TSampleHalfWaveTimePara), (BYTE *)&SampleHalfWaveTimePara );
		if(Result != TRUE)
		{
			SampleHalfWaveTimePara.Hours = HalfWaveDefCheckTime;
		}
	}
	
	if(Flag == 0x55)
	{
		api_WriteSysUNMsg(SAMPLE_CALI_PARA_ERR);
	}
	
}


//-----------------------------------------------
//��������: д����оƬȫ��ƫ����Ĵ���
//
//����:		�� 
//			
//����ֵ: 	����TRUE/FALSE
//		    
//��ע:
//-----------------------------------------------
BOOL WriteSampleChipOffsetPara(void)
{
	WORD i;
	WORD Result;
	DWORD *p;
	
	p = ((DWORD*)&SampleOffsetPara);

	Result = TRUE;
	
	for(i=0;i<(sizeof(SampleOffsetAddr)/sizeof(SampleOffsetAddr[0]));i++)
	{
		Result &= WriteSampleReg( SampleOffsetAddr[i], p[i] );
	}

	return Result;
}


//-----------------------------------------------
//��������: д����оƬȫ��������Ĵ���
//
//����:		�� 
//			
//����ֵ: 	����TRUE/FALSE
//		    
//��ע:
//-----------------------------------------------
BOOL WriteSampleChipCtrlPara(void)
{
	WORD i;
	WORD Result;
	DWORD *p;
	
	p = ((DWORD*)&SampleCtrlPara);

	Result = TRUE;
	
	for(i=0;i<(sizeof(SampleCtrlAddr)/sizeof(SampleCtrlAddr[0]));i++)
	{
		Result &= WriteSampleReg( SampleCtrlAddr[i], p[i] );
	}

	return Result;
}


//-----------------------------------------------
//��������: д����оƬȫ��������Ĵ���
//
//����:		�� 
//			
//����ֵ: 	����TRUE/FALSE
//		    
//��ע:
//-----------------------------------------------
BOOL WriteSampleChipAdjustPara(void)
{
	WORD i;
	WORD Result;
	DWORD *p;
	
	p = ((DWORD*)&SampleAdjustPara);

	Result = TRUE;
	
	for(i=0;i<(sizeof(SampleAdjustAddr)/sizeof(SampleAdjustAddr[0]));i++)
	{
		Result &= WriteSampleReg( SampleAdjustAddr[i], p[i] );
	}

	return Result;
}


//-----------------------------------------------
//��������: ��λ����оƬ
//
//����:		�� 
//			
//����ֵ: 	��
//		    
//��ע:ֱ�ӿ��Ƶ�Դ
//-----------------------------------------------
static void HardResetSampleChip(void)
{		
	//��Ȫ�Ƽ���λΪ10ms�����ֲ���ûд���ڵ����ϵ�ʱ��ʱ��̿��ܸ�λ���ɹ�
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 10 );
	
	CLEAR_WATCH_DOG;
}


//-----------------------------------------------
//��������: Ч�����оƬ�Ĵ���
//
//����:		�� 
//			
//����ֵ: 	��
//		    
//��ע:�ϵ�ʱ��û����Ч�飬�ϵ粻Ҫ���ô˺�����ÿ���ӵ�ʱ����ü���
//-----------------------------------------------
static void CheckSampleChipReg(void)
{
	DWORD tdw1, tdw2;
	BYTE i,Flag,Result = TRUE;

	//�����ж�
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )	
	{
		return;
	}	
	
	Flag = 0;
	
	if( CheckSampleChipCheckSum() == FALSE )
	{
		// Ӧ�ü�¼�澯�¼�
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
				HardResetSampleChip();
				api_SetError(ERR_CHECK_5460_1);
				byReWriteSampleChipRegCount = 0;
			}
		}
		//�ر�7038д����
		SampleSpecCmdOp(0xa6);
		Result = WriteSampleChipOffsetPara();	
		Result &= WriteSampleChipAdjustPara();	
		Result &= WriteSampleChipCtrlPara();
		//��7038д����
		SampleSpecCmdOp(0x00);
		if(Result == TRUE)
		{
			//У����쳣ʱ������Ч���
			GetSampleChipCheckSum();
		}
		else
		{
			// Ӧ�ü�¼�澯�¼�
			api_WriteSysUNMsg(INIT_SAMPLE_CHIP_FAIL);
		}
		//оƬ������ �ֶβ�����������
		if(SelDivAdjust == YES)
		{
			memset( g_ucNowSampleDivisionNo, 0xFF, sizeof(g_ucNowSampleDivisionNo) );
		}
	}
	else
	{
		byReWriteSampleChipRegCount = 0;
		api_ClrError(ERR_CHECK_5460_1);
	}

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
	DWORD RegData;
	BYTE i;

	//��ȡ���üĴ����ķ�ʽ�жϼ���оƬ���� 
	for(i = 0; i < (sizeof(SampleCtrlPara)/sizeof(DWORD)); i++)
	{

		//����ʹ���ж�У��͵ķ�ʽ ��ΪSPIû��У�� ���SPI���߶̽ӵ�VCC����GND �������������ǹ̶���
		//У���Ҳ�ᱻˢ��ΪȫFF����ȫ00 ���¹����޷������
		RegData = ReadSampleData( SampleCtrlAddr[i] );
		if(RegData != SampleCtrlPara.CtrlPara[i])
		{
			//������п��ƼĴ���
			CheckSampleRamPara();
			if(EmuErrCnt < 40)
			{
				EmuErrCnt++;		
				//�������20�γ���HFConstֵ����Ӧ ����У��һ��HFConst
				//��һ��ֹRAM�����ݳ��ִ��� �ڶ��������оƬ���ָ�λ ÿ����5��Ż���мĴ���У�� ������󱨵�����
				if(EmuErrCnt == 20)
				{
					HardResetSampleChip();//Ӳ��λ
					api_InitSampleChip();//��ʼ������оƬ
				}
				
				//����30�δ��� ��¼����оƬ����
				if(EmuErrCnt == 30)
				{
					api_SetError( ERR_CHECK_5460_1 ); //�ü���оƬͨ�Ź���
				}
			}
		}
		else
		{
			//����ָ��� ����֮ǰ����������10�����ϵĴ��� ���¼��һ�����мĴ������� �ڲ���api_ClrError
			if(EmuErrCnt > 10)
			{
				CheckSampleChipReg();
			}
			EmuErrCnt = 0; 
		}
	}
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
static BOOL CheckSampleChipSum(void)
{

	if( CheckSampleChipCheckSum() ==FALSE )
	{
		// Ӧ�ü�¼�澯�¼�
		api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);

		return FALSE;		
	}
	else
	{
		return TRUE;
	}
}

//-----------------------------------------------
//��������:��������
//
//����: ��
//
//����ֵ:  ��
//
//��ע:
//-----------------------------------------------
static void StartHarmonicSample(void)
{
	//��д����
	SampleSpecCmdOp(0xa6);
    
	//api_Delay10us(1);
	//����A��B��C�����������
	WriteSampleReg(w_Buffer2CFG, 0<<10 | 0<<9 | 0<<8 | 0x18);					//Buffer2���üĴ�������������ʹ�ܡ���ˡ�����ģʽ����������=ADC�������ݣ�
	WriteSampleReg(w_Buffer2CFG, 1<<10 | 0<<9 | 0<<8 | 0x18);					//Buffer2���üĴ�������������ʹ�ܡ���ˡ�����ģʽ����������=ADC�������ݣ�
	api_Delay10us(1);

	//��д��ֹ
	SampleSpecCmdOp(0x00);
}
//-----------------------------------------------
//��������:�жϻ��������Ƿ�׼����ȫ
//
//����: ��
//
//����ֵ:  ��
//
//��ע:
//-----------------------------------------------
static BOOL IsSampleDataReady(void)
{	
	if(ReadSampleData(r_UnreadNum2) < 1023)//w_WaveBuffTx = 0x7E
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
//-----------------------------------------------
//��������:��ȡw_WaveData�л������������
//
//����: 	
//
//����ֵ:  ��
//
//��ע: �˺������泤��Ϊ1024���ֽ� ����Ҫ�鿴�ϲ���û��泤���Ƿ���
//-----------------------------------------------
static BOOL GetAllADCData( SWORD *sADCBuf )
{
	BYTE i;

	
	if(IsSampleDataReady() == TRUE)
	{
		//�ر�7038д����
		SampleSpecCmdOp(0xa6);
		WriteSampleReg(w_SPICFG, 0x3A);//SPI���üĴ���������ģʽ�¹̶���ַ��
		//��7038д����
		SampleSpecCmdOp(0x00);
		//for(i=0;i<2;i++)
		{
			ReadSampleBuffCont(r_Buffer2, 512, &sADCBuf[0]);		//3ͨ��64��16bit/32bit
			ReadSampleBuffCont(r_Buffer2, 512, &sADCBuf[512]);		//3ͨ��64��16bit/32bit
		}//�ر�7038д����
		SampleSpecCmdOp(0xa6);
		WriteSampleReg(w_SPICFG, 0x38);//SPI���üĴ���������ģʽ�¹̶���ַ��
		//��7038д����
		SampleSpecCmdOp(0x00);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------
//��������: ����г��ʵ�����鲿����
//
//����: 	RICurrent:
//				A��B��C�����г��ʵ�����鲿����
//			ADCBuf:
//				�������ADC������
//			Cos:
//				���Ҳ���غ���
//			Sin:
//				���Ҳ���غ���
//
//����ֵ:  ��
//
//��ע:   
//---------------------------------------------------------------
static void CalHarmonic( double *RICurrent, SWORD *ADCBuf, const SWORD *Cos, const SWORD *Sin )
{
	double SumCos = 0, SumSin = 0;			//г��ʵ���������鲿����
	WORD i = 0, j = 0;
	
	CLEAR_WATCH_DOG;
	
	//����г��ʵ���������鲿����
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < AD_ONEWAVE_SIMPLE_NUM; j++)
		{
			SumCos += Cos[j] * ADCBuf[i + j * 3];
			SumSin += Sin[j] * ADCBuf[i + j * 3];
		}
		// / 1000 / 72 * 2 / 2^13 * 2^8 / 1.4142
		SumCos /= 1000;
		SumCos = SumCos / AD_ONEWAVE_SIMPLE_NUM * 2;			//��(N/2) NΪ��������
		SumCos = SumCos / ((double)(6 * CURRENT_VALUE) / 5) * ((double)wMeterBasicCurrentConst / 1000);
		SumCos /= 32;
		SumCos = SumCos / 1.4142;
		if( SumCos < 0 )
		{
			SumCos *= -1;
		}
		RICurrent[i * 2] = SumCos * 10000;
		
		SumSin /= 1000;
		SumSin = SumSin / AD_ONEWAVE_SIMPLE_NUM * 2;			//��(N/2) NΪ��������
		SumSin = SumSin / ((double)(6 * CURRENT_VALUE) / 5) * ((double)wMeterBasicCurrentConst / 1000);
		SumSin /= 32;
		SumSin = SumSin / 1.4142;
		if( SumSin < 0 )
		{
			SumSin *= -1;
		}
		RICurrent[i * 2 + 1] = SumSin * 10000;
	}
}

//-----------------------------------------------
//��������:���㲢�ж��Ƿ��ڰ벨״̬
//
//����: 	
//����ֵ: eAD_HALFWAVE_NO - ���ǰ벨��eAD_HALFWAVE_YES - �ǰ벨�� eAD_DATA_NO_REFRESH - ����δ���½���
//
//��ע:����г������
//-----------------------------------------------
static eGetHalfWaveStatus CalculateHalfWaveStatus1( SWORD *sADCBuf )
{
	double HD2Curr[3][2];								//����������г��ʵ�����鲿����
	DWORD SumCurr[3];									//����������г����Чֵ
	double FirRate[3], HD2Rate[3];						//����������г������
	static double LastFirRate[3] = { 0.0, 0.0, 0.0 };	//��һ�εĻ�������
	static double LastHD2Rate[3] = { 0.0, 0.0, 0.0 };	//��һ�εĶ���г������
	long FirRateDiff[3], HD2RateDiff[3];				//��ǰ����һ�ε�г����ֵ
	BYTE i = 0, j = 0;

	CLEAR_WATCH_DOG;
	
	//��������ܲ�
	for(i = 0; i < 3; i++)
	{
		//��ȡ����г��ʵ�����鲿����
		CalHarmonic( &HD2Curr[0][0], &sADCBuf[i * 300 + 30], TAB_Cos2, TAB_Sin2 );
		//�������г����Чֵ
		SumCurr[0] = (DWORD)sqrt(HD2Curr[0][0] * HD2Curr[0][0] + HD2Curr[0][1] * HD2Curr[0][1]);
		SumCurr[1] = (DWORD)sqrt(HD2Curr[1][0] * HD2Curr[1][0] + HD2Curr[1][1] * HD2Curr[1][1]);
		SumCurr[2] = (DWORD)sqrt(HD2Curr[2][0] * HD2Curr[2][0] + HD2Curr[2][1] * HD2Curr[2][1]);
		//�������г������(�ٷ���)
		for(j = 0; j < 3; j++)
		{
			if( labs( RemoteValue.I[j].l ) > HD2CheckCurrentValueMin )
			{
				HD2Rate[j] = (double)(SumCurr[j]) / labs( RemoteValue.I[j].l ) * 100;
			}
			else
			{
				HD2Rate[j] = 0;
			}
		}
		//���㵱ǰ����һ�εĶ���г����ֵ
		HD2RateDiff[0] = labs( (long)(HD2Rate[0] - LastHD2Rate[0]) );
		HD2RateDiff[1] = labs( (long)(HD2Rate[1] - LastHD2Rate[1]) );
		HD2RateDiff[2] = labs( (long)(HD2Rate[2] - LastHD2Rate[2]) );
		//������һ�εĶ���г������
		LastHD2Rate[0] = HD2Rate[0];
		LastHD2Rate[1] = HD2Rate[1];
		LastHD2Rate[2] = HD2Rate[2];
		
		CLEAR_WATCH_DOG;
		
		//��ȡ����ʵ�����鲿����
		CalHarmonic( &HD2Curr[0][0], &sADCBuf[i * 300 + 30], TAB_Cos1, TAB_Sin1 );
		//���������Чֵ
		SumCurr[0] = (DWORD)sqrt(HD2Curr[0][0] * HD2Curr[0][0] + HD2Curr[0][1] * HD2Curr[0][1]);
		SumCurr[1] = (DWORD)sqrt(HD2Curr[1][0] * HD2Curr[1][0] + HD2Curr[1][1] * HD2Curr[1][1]);
		SumCurr[2] = (DWORD)sqrt(HD2Curr[2][0] * HD2Curr[2][0] + HD2Curr[2][1] * HD2Curr[2][1]);
		//�����������(�ٷ���)
		for(j = 0; j < 3; j++)
		{
			if( labs( RemoteValue.I[j].l ) > HD2CheckCurrentValueMin )
			{
				FirRate[j] = (double)(SumCurr[j]) / labs( RemoteValue.I[j].l ) * 100;
			}
			else
			{
				FirRate[j] = 0;
			}
		}
		//���㵱ǰ����һ�εĻ�����ֵ
		FirRateDiff[0] = labs( (long)(FirRate[0] - LastFirRate[0]) );
		FirRateDiff[1] = labs( (long)(FirRate[1] - LastFirRate[1]) );
		FirRateDiff[2] = labs( (long)(FirRate[2] - LastFirRate[2]) );
		//������һ�εĻ�������
		LastFirRate[0] = FirRate[0];
		LastFirRate[1] = FirRate[1];
		LastFirRate[2] = FirRate[2];
		
		//�������г����������ȫ�����ڷ�Χ�ڣ�����Ϊ���ǰ벨
		if(((HD2Rate[0] < HD2ContentMin) || (HD2Rate[0] > HD2ContentMax))
		   && ((HD2Rate[1] < HD2ContentMin) || (HD2Rate[1] > HD2ContentMax))
		   && ((HD2Rate[2] < HD2ContentMin) || (HD2Rate[2] > HD2ContentMax)))
		{
			return eAD_HALFWAVE_NO;
		}
		//�������г����ֵ����һ���������ֵ, ����Ϊ���ǰ벨
		if((HD2RateDiff[0] > HD2DiffValue) 
		   || (HD2RateDiff[1] > HD2DiffValue) 
		   || (HD2RateDiff[2] > HD2DiffValue))
		{
			return eAD_HALFWAVE_NO;
		}
		//���������ֵ����һ���������ֵ, ����Ϊ���ǰ벨
		if((FirRateDiff[0] > FirstDiffValue) 
		   || (FirRateDiff[1] > FirstDiffValue) 
		   || (FirRateDiff[2] > FirstDiffValue))
		{
			return eAD_HALFWAVE_NO;
		}
	}
	
	return eAD_HALFWAVE_YES;
}
//-----------------------------------------------
//��������:�ж��Ƿ��ڰ벨״̬
//
//����: 	
//����ֵ:  eAD_HALFWAVE_NO - ���ǰ벨��eAD_HALFWAVE_YES - �ǰ벨�� eAD_DATA_NO_REFRESH - ����δ���½���
//
//��ע:ͨ�����ֵ�Ƚϵķ�ʽ,ͨ���۲����ݵã���42.4A�������Ϸ�ֵ���·�ֵ�����2000
//-----------------------------------------------
/*
static eGetHalfWaveStatus JudgeHalfWaveStatus2(void)
{
	BYTE i,j;
	BYTE bWaveNum;
	SWORD sBigData,sSmallData;
	
	if(GetAllADCData() != TRUE)//�������û��������δˢ�£��򷵻�ʧ��
	{
		return eAD_DATA_NO_REFRESH;
	}

	CLEAR_WATCH_DOG;

	//�ܲ���������
	bWaveNum = 0;	

	for(i = 0; i < (AD_BUFFER_NUM/AD_ONEWAVE_SIMPLE_NUM); i++)//�������������ж�
	{
		sBigData = 0;
		sSmallData = 0;
		
		for(j = 0; j < AD_ONEWAVE_SIMPLE_NUM ; j++)//��72������бȽ�,Ѱ�����ֵ����Сֵ
		{
			if(sADCBuf[j + (AD_ONEWAVE_SIMPLE_NUM * i)] > sBigData)
			{
				sBigData = sADCBuf[j + (AD_ONEWAVE_SIMPLE_NUM * i)];
			}
			else if(sADCBuf[j + (AD_ONEWAVE_SIMPLE_NUM * i)] < sSmallData)
			{
				sSmallData = sADCBuf[j + (AD_ONEWAVE_SIMPLE_NUM * i)];
			}
			else
			{
				
			}
		}
		if(abs(abs(sBigData) - abs(sSmallData)) > AD_PEAK_DIF_THRESHOLD)
		{
			bWaveNum++;
			if(bWaveNum >= AD_ABNORMAL_JUDGE_NUM)
			{
				return eAD_HALFWAVE_YES;
			}
		}
		else
		{
			bWaveNum = 0;
		}
	}
	return eAD_HALFWAVE_NO;
}
*/
//-----------------------------------------------
//��������:���ݵ������ؼ�COS�����ж��Ƿ����벨ADC���ݲ���������
//
//����: 	
//
//����ֵ:  ��
//
//��ע:
//-----------------------------------------------
static BOOL CheckHalfWaveStatus( void )
{
	//ֱ����ż��г��ʵ�������ΧΪ5-72A, �벨�����������ΧΪ2.5-36A
	//������������ ����û�� ����Cos[1]ΪA�๦������ I[0]ΪA����� ��Ҫ�ô�
	//�����������һ�����1A�Ż��ж��Ƿ��ڰ벨״̬
	if( (MeterCurrentTypesConst == CURR_60A) 
		&& ((labs(RemoteValue.I[0].l) > HD2CheckCurrentValueMin) 
			|| (labs(RemoteValue.I[1].l) > HD2CheckCurrentValueMin) 
			|| (labs(RemoteValue.I[2].l) > HD2CheckCurrentValueMin)) )
	{
		return TRUE;
	}
	//Ŀǰû��100�ı� ����ֵ�����һ������@@@@
	else if( (MeterCurrentTypesConst == CURR_100A)
		  && ((labs(RemoteValue.I[0].l) > HD2CheckCurrentValueMin) 
			|| (labs(RemoteValue.I[1].l) > HD2CheckCurrentValueMin) 
			|| (labs(RemoteValue.I[2].l) > HD2CheckCurrentValueMin)) )
	{
		return TRUE; 
	}
	
	return FALSE;
}

//---------------------------------------------------------------
//��������: �ж��Ƿ��𶯰벨����
//
//����: 
//                   
//����ֵ:  
//
//��ע:   
//---------------------------------------------------------------
static BOOL JudgeHalfWaveRegulate( void )
{
	SWORD sADCBuf[1028]; //����1024�͹� ����4���ֽ�
	eGetHalfWaveStatus bHalfWaveStatus1;                        //��ǰ��������
    static BOOL StartADFlag = FALSE, ADFalseCount = 0;
	BYTE i = 0;
	
	bHalfWaveStatus1 = eAD_HALFWAVE_NO;
	memset (sADCBuf,0x00,sizeof(sADCBuf));
	if( CheckHalfWaveStatus() == TRUE)
	{
		//�ӿ���AD ��AD���ݻ�ȡ ����һ��������
		if( StartADFlag == TRUE )
		{
			if( GetAllADCData( sADCBuf ) == TRUE ) //�������û������������Ϊ���ǰ벨
			{
				StartADFlag = FALSE; //������ȡ��� ���Ͽ�����һ��AD
				ADFalseCount = 0;
				
				bHalfWaveStatus1 = CalculateHalfWaveStatus1( sADCBuf );
				//bHalfWaveStatus2 = JudgeHalfWaveStatus2();
				
				if(bHalfWaveStatus1 == eAD_HALFWAVE_YES)
				{
					//���������ж��ǰ벨����������
					if(bHalfWaveCalTime < 5)
					{
						bHalfWaveCalTime++;
					}
				}
				else
				{
					//�����⵽�ǰ벨����bHalfWaveCalTime���������㣬�˳��벨����ϵ���������¼������оƬУ��
					bHalfWaveCalTime = 0;
				}
			}
			else
			{
				if( ADFalseCount < 5 )
				{
					ADFalseCount++;
				}

				if( ADFalseCount >= 3 )
				{
					//������ֶ�δ��� ���¿���һ��AD
					StartADFlag = FALSE;
					
					//���AD���λ�ȡʧ�� ����ǰһ���ǲ���״̬ ����������һ ��ֹADһֱʧ���ڲ����������
					if(bHalfWaveCalTime >= 2)
					{
						bHalfWaveCalTime--;
					}
					else//�籾���ڲ���״̬ ֱ��������¼���
					{
						bHalfWaveCalTime = 0;
					}
				}
			}
		}
		
		if( StartADFlag == FALSE ) //һ�뿪��һ��AD
		{
			//ֱ��ż��г������AD����
			//ˢ��һ�β���
			StartHarmonicSample();
			StartADFlag = TRUE;
			ADCSampleCount = 0;
		}		
		
		//���������ǰ벨 ��������
		if(bHalfWaveCalTime >= 2)
		{
			for(i = 0; i < 3; i++)
			{
				//�������ֵ������С�ж�ֵ
				if( labs( RemoteValue.I[i].l ) > HD2CheckCurrentValueMin )
				{
					//1.0L����
					if( labs( RemoteValue.Cos[i + 1].l ) > HD2CosLimitValue )
					{
						HalfWaveUpdateFlag[i] = 2;
					}//0.5L����
					else
					{
						HalfWaveUpdateFlag[i] = 1;
					}
				}
				else
				{
					HalfWaveUpdateFlag[i] = 0;
				}
			}
			return TRUE;
		}
	}
    else
    {
        bHalfWaveCalTime = 0;
    }
	
	HalfWaveUpdateFlag[0] = 0;
	HalfWaveUpdateFlag[1] = 0;
	HalfWaveUpdateFlag[2] = 0;
	
	return FALSE;
}
//static BOOL Check_Harmonic(BOOL *pSta)
//{
//	DWORD	sum1, sum2, fund[3], curr;
//	DWORD	sum1c, sum1s, sum2c, sum2s;
//	DWORD	i, phs, cycle;
//	DWORD	samp[72][MAX_PHASE];										//ADC data:72*3
//	float	rate;
//	
//	for (phs=0; phs<C_PhsNum; phs++)
//	{
//		pSta[phs] = (g_Curr[phs] > TAB_HarLvl.CurrLower)? TRUE: FALSE;
//	}
//	if((pSta[0] == FALSE)
//	&& (pSta[1] == FALSE)
//	&& (pSta[2] == FALSE))
//	{
//		return;
//	}
//	//��ȡ��������--------------------------------------------------------------
//	for (cycle=0; cycle<2; cycle++)											//���ܲ�
//	{
//		if ((cycle == 1)
//		&& (pSta[0] == FALSE)												//��һ�ּ�ⲻ�ڹ���
//		&& (pSta[1] == FALSE)
//		&& (pSta[2] == FALSE))
//		{
//			break;
//		}
//		Read_Reg_Cont(r_Buffer2, 3*C_SampNum/2, (INT16U*)&samp[0][0]);		//3ͨ��64��16bit/32bit
//		for (phs=0; phs<C_PhsNum; phs++)
//		{
//			if (pSta[phs] == FALSE)											//֮ǰ��ⲻ�ڹ���
//			{
//				continue;
//			}
//			sum1c = 0;
//			sum1s = 0;
//			sum2c = 0;
//			sum2s = 0;
//			for (i=0; i<C_SampNum; i++)
//			{
//				sum1c += TAB_Cos1[i]*samp[i][phs];
//				sum1s += TAB_Sin1[i]*samp[i][phs];
//				sum2c += TAB_Cos2[i]*samp[i][phs];
//				sum2s += TAB_Sin2[i]*samp[i][phs];
//			}
//			sum1c >>= 16;													//��λ,�Ա�������
//			sum1s >>= 16;
//			sum2c >>= 16;
//			sum2s >>= 16;
//			sum1 = sum1c*sum1c + sum1s*sum1s;
//			sum2 = sum2c*sum2c + sum2s*sum2s;
//			sum1 = sqrt(sum1);
//			sum2 = sqrt(sum2);
//			rate = 0;
//			curr = (float)sum1/1000*65536/C_SampNum *2 * 256 /1.4142 * TAB_MeterRat.curr;//�������ұ�����1000��ǰ��������С16λ��ADC������С8λ
//			if (curr > TAB_HarLvl.CurrLower)
//			{
//				rate = (float)sum2 / sum1;									//����г��������
//			}
//			if ((rate <= TAB_HarLvl.RateLower)								//����г����������
//			|| (rate >= TAB_HarLvl.RateUpper))								//����г����������
//			{
//				pSta[phs] = FALSE;
//			}
//			if (pSta[phs] == TRUE)											//��ǰ�����жϻ���Ч
//			{
//				if (cycle == 0)												//��һ�β���,�������ֵ
//				{
//					fund[phs] = curr;
//				}
//				else														//�ڶ��β���,����һ�����Ƚ�
//				{
//					rate = (float)curr / fund[phs];
//					if ((rate < 0.9) || (rate > 1.1))
//					{
//						pSta[phs] = FALSE;
//					}
//				}
//			}
//		}
//	}
//	Write_Reg(w_Buffer2CFG, 0<<10 | 0<<9 | 0<<8 | 0x18);					//Buffer2���üĴ�������������ʹ�ܡ���ˡ�����ģʽ����������=ADC�������ݣ�
//	Write_Reg(w_Buffer2CFG, 1<<10 | 0<<9 | 0<<8 | 0x18);					//Buffer2���üĴ�������������ʹ�ܡ���ˡ�����ģʽ����������=ADC�������ݣ�
//}

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
	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
	
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )//5s����е�ѹ�ϸ����ۼƣ�������ʱ�䲻Ϊ1440
	{	
		//�ϵ�30s�����Ч�飬���⻹û��������оƬ��Ч���
		if( bySamplePowerOnTimer == 0 )
		{
			CheckSampleRamPara();			
			CheckSampleChipReg();
		}

		#if( SEL_STAT_V_RUN == YES )//�����ѹ�ϸ��ʲ���		
		StatVRunTime();
		#endif
	}
	
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )
	{
		//ÿ���Ӽ���һ��ƽ������
		CalPQSaverage();
		//ÿ�����ٳ�ʼ��һ�£�������ѹ����������������������ȫʧѹ�л���
		InitSampleCaliUI();
		
		#if( SEL_AHOUR_FUNC == YES )
		if(t.Min == 26)
		{
			SwapAHour(); 
		}
		#endif
		
		//ֱ��ż��г��--ÿ�����ۼ��ϵ���ʱ��
		//���Ϊ��Կ��һֱ����ֱ��ż��г��
		if( MeterTypesConst != METER_ZT )
		{
			HalfWavePowerOnTime = 0;
			HalfWaveStatusFlag = FALSE;
		}
		else
		{
			if( api_GetRunHardFlag( eRUN_HARD_COMMON_KEY ) == TRUE )
			{
				HalfWavePowerOnTime = 0;
				HalfWaveStatusFlag = TRUE;
			}//���Ϊ˽Կ
			else
			{
				//һֱ�ر�
				if( SampleHalfWaveTimePara.Hours == 0 )//��ֱͨ��
				{
					HalfWavePowerOnTime = 0;
					HalfWaveStatusFlag = FALSE;
				}//һֱ����
				else if( SampleHalfWaveTimePara.Hours == 0xFF )
				{
					HalfWavePowerOnTime = 0;
					HalfWaveStatusFlag = TRUE;
				}//ʱ�䵽�ر�
				else
				{
					//����ۼ�ʱ��С���趨ʱ��, ʱ���ۼ�, �����ж�
					if( HalfWavePowerOnTime < (SampleHalfWaveTimePara.Hours * 60) )
					{
						HalfWavePowerOnTime++;
						HalfWaveStatusFlag = TRUE;
					}//�ۼ�ʱ����ڵ����趨ʱ��, �ر��ж�
					else
					{
						HalfWaveStatusFlag = FALSE;
					}
				}
			}
		}
	}
	
	if(bySamplePowerOnTimer)
	{
		bySamplePowerOnTimer--;
		
		//����оƬ����Ч����Ҫһ��ʱ�䣬������ʱһ���ȡ
		if(bySamplePowerOnTimer == 28)
		{
			GetSampleChipCheckSum();
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
	
	//�ֶβ��� �¶Ȳ��� �޹�����
	SampleDivisionRegulate();
	
	//����оƬ���ϼ��
	CheckEmuErr();
	
	#if( SEL_AHOUR_FUNC == YES )
	AccumulateAHour();
	#endif
}
//ȫʧѹ�õ���ˢ��
BOOL api_shuaxin(void)
{
	BYTE i,DataSign;
	BYTE bySamplePulseNum;	
	DWORD tdw,tdBuf[16];
	DWORD UpLimit,DownLimit;
	float tf;
	Link_ReadSampleRegCont( r_RMSIA, 6, tdBuf);
			
	if( MeterTypesConst == METER_ZT )
	{
		DownLimit = (DWORD)wMeterBasicCurrentConst * 10 * 2 / 1000;
	}	
	else 
	{
		DownLimit = (DWORD)wMeterBasicCurrentConst * 10 * 1 / 1000;
	}
	for(i=0;i<3;i++)
	{
		tf = (float)tdBuf[i]*10000/TWO_POW13;
		tf /= ((float)(6 * CURRENT_VALUE) / 5);
		RemoteValue.I[i].l = (long)((tf * wMeterBasicCurrentConst) / 1000 + 0.5);

		if( labs(RemoteValue.I[i].l) < DownLimit )
		{
			RemoteValue.I[i].l = 0;
		}
		
		if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A+i) == TRUE )
		{
			RemoteValue.I[i].l *= -1;
		}
	}
	//���ߵ���
	if(SelZeroCurrentConst == YES)
	{
		tdw = tdBuf[3];
		tf = (float)tdw * 10000 / TWO_POW13;
		//����50mvʱ��Vrms/2^13=60
		tf /= ((float)(6 * ZERO_CURRENT_VALUE) / 5);
		RemoteValue.I[3].l = (long)(tf * ((float)wMeterBasicCurrentConst / 1000) + 0.5);
		
		if( labs(RemoteValue.I[3].l) < DownLimit )
		{
			RemoteValue.I[3].l = 0;
		}
	}
	else
	{
		RemoteValue.I[3].l = 0;
	}
	if(MeterTypesConst != METER_3P3W)
	{
		tdw = tdBuf[5];
		tf = (float)tdw*10000/TWO_POW12;
		tf /= ((float)(6 * CURRENT_VALUE) / 5);
		RemoteValue.I[4].l = (long)(tf * ((float)wMeterBasicCurrentConst / 1000) + 0.5);
		
		if( labs(RemoteValue.I[4].l) < DownLimit )
		{
			RemoteValue.I[4].l = 0;
		}
	}
	else
	{
		RemoteValue.I[4].l = 0;
	}
	#if( SEL_EVENT_LOST_ALL_V == YES )
	if( byPowerOnFlag != 0x55 )
	{
		api_SetLostAllVCurrent(&RemoteValue.I[0].l);
	}
	#endif
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
BOOL api_InitSampleChip( void )
{
	BOOL Result;
	BYTE i;
	
	CheckSampleRamPara();
	//WriteSampleReg(w_SRSTREG,  0xA5);						//��λEMU
	//�ر�7038д����
	SampleSpecCmdOp(0xa6);
	//��ʼ���������------------------------------------------------------------
	WriteSampleReg(w_SPICFG, 0x38);//SPI���üĴ���������ģʽ��������ַ��
	for(i=0; i<10; i++)
	{		
		Result = WriteSampleChipOffsetPara();
		Result &= WriteSampleChipAdjustPara();	
		Result &= WriteSampleChipCtrlPara();
		
		if( Result == TRUE )
		{
			break;
		}
	}
	
	if( i > 1 )
	{
		api_WriteSysUNMsg(INIT_SAMPLE_CHIP_FAIL);
	}
	
	if( i == 10 )
	{
		HardResetSampleChip();
		Result = WriteSampleChipOffsetPara();
		Result &= WriteSampleChipAdjustPara();	
		Result &= WriteSampleChipCtrlPara();		
	}
	
	//оƬ�Զ��¶Ȳ�����ʼ��
	if(SelMChipVrefgain == YES)
	{
		InitMChipVrefgain();
	}
	
	//��7038д����
	SampleSpecCmdOp(0x00);
	
	//У����쳣ʱ������Ч���
	GetSampleChipCheckSum();
	//�ϵ�ֶβ�����ʼ��
	if(SelDivAdjust == YES)
	{
		memset( g_ucNowSampleDivisionNo, 0xFF, sizeof(g_ucNowSampleDivisionNo) );
	}
	
	return Result;
}
void InitHarmonicSample(void)
{
	ADCSampleCount = 10;								//����оƬADC����ʱ�������
	HalfWavePowerOnTime = 0;							//ֱ��ż��г���ϵ����ۼ�ʱ��(����)
	HalfWaveStatusFlag = TRUE;							//ֱ��ż��г��״̬��־  TRUE: �����ж�  FALSE: �ر��ж�
	//�ر�7038д����
	SampleSpecCmdOp(0xa6);
	//��ʼ���������------------------------------------------------------------
	WriteSampleReg(w_DataChnCFG, (7<<11 | 0<<15 | 1<<19));						//����ͨ��ѡ��Ĵ�����3·�����������ݡ�����Դ=SPL�������ݡ�����λ��=16bit��
	WriteSampleReg(w_Buffer2CFG, (1<<10 | 0<<9 | 0<<8 | 0x18));					//Buffer2���üĴ�������������ʹ�ܡ���ˡ�����ģʽ����������=ADC�������ݣ�
	WriteSampleReg(w_Buffer2_AddrLen, 0x200400);									//Buffer2��ʼ��ַ�ͳ������üĴ�������ʼ��ַ=0x200�����ݳ���=0x31D��
	WriteSampleReg(w_UnRLVL_NullNum2, (216<<12));								//Buffer2δ��������ֵ�뻺��Ԥ���������üĴ�����2�ܲ���72��/�ܲ���3ͨ����16bit/32bit��
	//��7038д����
	SampleSpecCmdOp(0x00);
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
	bySamplePowerOnTimer = 30;
	byReWriteSampleChipRegCount = 0;
	dwSampleSFlag = 0;
	bySampleStep = 0xff;		
	byPowerOnFlag = 0x55;

	#if( SEL_STAT_V_RUN == YES )//�ָ���ѹ�ϸ���
	PowerUpVRunTime( );
	#endif
	
	InitSampleCaliUI();
	
	api_InitSampleChip();
	InitHarmonicSample();
	//�ϵ������ѹ ���� ����
	PowerUpReadVoltage();
    byPowerOnFlag = 0;
	//WriteSampleReg( w_CFxRun, 0xFFFFFF );
	#if( SEL_AHOUR_FUNC == YES )
	PowerUpAHour();
	#endif
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
void api_SampleTask( void )
{
	TRealTimer t;
	
	
	Read_UIP();
		
	if( api_GetTaskFlag(eTASK_SAMPLE_ID,eFLAG_SECOND) == TRUE )
	{
		Task_Sec_Sample();
		api_ClrTaskFlag(eTASK_SAMPLE_ID,eFLAG_SECOND);
	}

	if( api_GetTaskFlag(eTASK_SAMPLE_ID,eFLAG_HOUR) == TRUE )
	{	
		api_ClrTaskFlag(eTASK_SAMPLE_ID,eFLAG_HOUR);
		
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
		#if( SEL_STAT_V_RUN == YES )
		if(t.Hour == 0)
		{
			//��ʱ3���壬��ֹ�ڶ����ʱ������Ϊ0
			if(t.Day == 1)
			{
				ClrVRunMonTimer = 3;
			}
			
			ClrVRunDayTimer = 3;
		}
		else
		{
			//ÿСʱת���ѹ�ϸ���
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
//��������: 	��¼�ϵ��ѹ
//
//����:		�� 
//                 
//����ֵ:	 	��
//		   
//��ע:   
//-----------------------------------------------
void api_RecordPowerUpVoltage(void)
{
	BYTE i;
	WORD Result;
	TTwoByteUnion Vlotage,VlotageBak;
  
	Vlotage.w = 0;
	VlotageBak.w =0;
	for( i=0; i<MAX_PHASE; i++ )
	{
		Result = api_GetRemoteData( PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, 2, Vlotage.b );
		if( Result == FALSE )
		{
			break;
		}
	
		if( Vlotage.w > VlotageBak.w )//�����ϵ�����ѹ
		{
			VlotageBak.w = Vlotage.w;
			VlotageBak.b[1] &= 0x3f;//��2λΪ����
			VlotageBak.b[1] |= (i<<6);//��2λΪ����
		}
	}
	//��¼�ϵ��ѹ
	api_WriteFreeEvent( EVENT_SYS_START_VOLTAGE, VlotageBak.w );
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
//				4:N������				PHASE_N
//				5:N������				PHASE_N2	
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
const BYTE MinPhase[] = {1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0xff,0,	0};
const BYTE MaxPhase[] = {3,	5,	3,	3,	3,	3,	3,	3,	3,	3,	3,	0xff,	0xff,	0xff,	0xff,0,	0};
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
			//�ж����ߵ��������������������û�д�
			if( ((SelZeroCurrentConst == NO ) && ((Type&0xFF00) == PHASE_N))
			|| ( (MeterTypesConst == METER_3P3W) && ((Type&0xFF00) == PHASE_N2)) )	
			{
				return FALSE;
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
			memcpy(td.b,RemoteValue.PQSAverage[0][Phase].b,4);
			break;
		case REMOTE_Q_AVE:
			memcpy(td.b,RemoteValue.PQSAverage[1][Phase].b,4);
			break;
		case REMOTE_S_AVE:
			memcpy(td.b,RemoteValue.PQSAverage[2][Phase].b,4);
			break;
		case REMOTE_COS:
			td.d = RemoteValue.Cos[Phase].l;
			break;
		case REMOTE_HZ:
			memcpy(td.b,RemoteValue.Freq.b,4);
			break;
		
		#if( SEL_VOL_ANGLE_FUNC == YES )
		case REMOTE_PHASEU:
			memcpy(td.b,RemoteValue.YUaUbUcAngle[Phase-1].b,4);
			break;
		case REMOTE_PHASE:
			memcpy(td.b,RemoteValue.YUIAngle[Phase-1].b,4);
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
//				0x20--��Ǳ��״̬	1--Ǳ��		0--��
//				0x3X--��ǰ���޹����� ����1~4������һ����~������
//              0x40--��ǰ�����ڹ����Ƿ���ڶ���ʵ�ǧ��֮��                 0--����	1--С��
//��ע:   
//-----------------------------------------------
WORD api_GetSampleStatus(BYTE Type)
{
	BYTE PQDirect;
	WORD i,tw;
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
			tw = tl / (wStandardVoltageConst/10) / (dwMeterMaxCurrentConst / 100) / 3;
			break;
		case 0x10:
			//��ѹ״̬	1--����0.6Un	0--����0.6Un
			for(i=0;i<3;i++)
			{
				if( (MeterTypesConst == METER_3P3W) && ( i == 1 ) )
				{
					continue;
				}
				
				if(RemoteValue.U[i].d < ((DWORD)wStandardVoltageConst * 100 * 6 / 10))
				{
					tw |= (0x0001<<i);
				}
			}
						
			//��ѹ����	1--��ѹ������	0--��ѹ����
			if( dwSampleSFlag & 0x00000008 )
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
				
				if(labs( RemoteValue.I[i].l ) < ((DWORD)wMeterBasicCurrentConst * 10 * 5 / 100))
				{
					tw |= (0x0001<<i);
				}
			}
			
			//��������	1--����������	0--��������
			if( dwSampleSFlag & 0x00000010 )
			{
				tw |= 0x8000;
			}

			break;
		case 0x20://��Ǳ��״̬
			//bit9,bit10,bit11�ֱ����ABC����״̬ 0--�� 1--Ǳ��
			if( (dwSampleSFlag&0x00000e00) == 0x00000e00 )
			{
				tw = 1;
			}
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
			if(labs( RemoteValue.S[0].l ) >= (wStandardVoltageConst * wMeterBasicCurrentConst * 4 * 3 / 10000)) //U*I*0.004*3
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

#endif//#if(SAMPLE_CHIP==CHIP_HT7032L)
