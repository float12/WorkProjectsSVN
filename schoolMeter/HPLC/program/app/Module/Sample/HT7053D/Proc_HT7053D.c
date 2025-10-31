//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.7.30
//����		����оƬHT7017�������ļ�
//�޸ļ�¼	!!!!!!�Ժ�Ҫ��ӵ�ѹӰ�������� ȱд������
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7053D.h"
#include "AHour.h"
#include "statistics_voltage.h"

#if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
TRemoteValue		RemoteValue;		//�˶����� ��˲ʱ������
TSampleCorr			SampleCorr;			//˲ʱ��ϵ��
TSampleAdjustReg    SampleAdjustReg;	//����оƬУ��������
TSampleCtrlReg      SampleCtrlReg;      //����оƬ����������
TSampleVolCorr		SampleVolCorr;		//��ѹӰ����ϵ��

//��ѹӰ������ѹ��Χ 80% 85% 90% 110% 115% 65%~130%
const WORD DefVolCorrectRange[5][2] =
{
	{ 1430, 1790 },	//0.8Un  - 1760
	{ 1790, 1930 }, //0.85Un - 1870
	{ 1930, 2010 }, //0.9Un  - 1980
	{ 2390, 2450 },	//1.1Un  - 2420
	{ 2450, 2860 }, //1.15Un - 2530
};

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
float			gfPaverage;						//����1����ƽ������
BYTE			byEnergyPulse;					//�����������
BYTE			byEnergyPulseBak;				//���������������
BYTE			byPaverageCnt;					//ƽ�������ۼӴ���	
BYTE			byBackFlagCnt;					//�����жϼ���
BYTE			bySampleStep;					//��ѭ��ˢ��Զ������Э������ÿ���һ�Σ�ÿ���ڴ�ѭ�����ˢ�¸���Զ������
BYTE			EMU_flag;						//����оƬ״̬�Ĵ���
BYTE 			byReWriteSampleChipRegCount;	//�ж�оƬ���������
BYTE			byLastVolCorrectLevel;			//��һ�ε�ѹӰ���������ȼ�
BYTE			byVoltageErrCount;				//��ѹ�쳣�Ĵ���
//��������
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void CheckSampleChipCtrlReg(void);
static void CheckSampleChipAdjustReg(void);

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ����һ����ƽ������
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:����RemoteValue.Pav��ΪHEX��   
//-----------------------------------------------
static void CalPaverage(void)
{	
	if(byPaverageCnt == 0) 
	{
		return;
	}
	
	RemoteValue.Pav.l = (long)(gfPaverage/(float)byPaverageCnt);
	
	gfPaverage = 0;
	byPaverageCnt = 0;
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
	BYTE Result, Cmd;
	TFourByteUnion ComData;
				
	if( bySampleStep == 0xff )
	{
		return;
	}
	
	//����оƬ���Ϻ�ǰ55�벻���м���ͨѶ ��ֹ���ٳ��� ��5��ǿ�ƽ���ͨѶ 
	if( api_CheckError(ERR_CHECK_5460_1) == TRUE )
	{
		if( RealTimer.Sec < 55 )//����ʱ��ÿ���ӵĺ�5����м���ͨѶ
		{
			bySampleStep = 0xff;
			return;
		}
	}
	
	Cmd = bySampleStep; 
	
	switch(Cmd)
	{
		case 0x00:				
			Result = Link_ReadSampleReg(EmuStatus,ComData.b);
			if( Result == TRUE ) 
			{
				EMU_flag = ComData.b[0];
			}	
			else 
			{
				EMU_flag = 0xff;
			}
			break;
		case 0x01:	//U
			Result = Link_ReadSampleReg(URMS,ComData.b);//�����ȡ����Ӧ����ô����@@@@
			if (Result == TRUE)
			{
				RemoteValue.U.d = ComData.d*1000/SampleCorr.UGain.d;
			}
			if(RemoteValue.U.d > 450000) 
			{
				RemoteValue.U.d = 0;//��ѹ>450VΪ�쳣		
			}
			break;		
		case 0x02:	//P
			Result = Link_ReadSampleReg(PowerPA,ComData.b);
			if (Result == TRUE)
			{
				Result = ComData.b[2];
				if( Result & 0x80 )
				{
					ComData.d = 0x01000000 - ComData.d;
				}
				RemoteValue.P.l = (long)((double)ComData.d*1000000.0/SampleCorr.P1Gain.d);

				if( Result & 0x80 )
				{
					RemoteValue.P.l *= -1;
				}
			}
			
			if( EMU_flag != 0xff )
			{	
				//�ж��Ƿ������������ͬʱ���ʷ���λΪ1
				if( (EMU_flag & 0x04) == 0 )
				{
					if( RemoteValue.P.l < 0 )
					{
						if( byBackFlagCnt < 2 ) 
						{
							byBackFlagCnt++;
						}
						else
						{
							//���ú��෴���־
							api_SetSysStatus(eSYS_STATUS_OPPOSITE_P);
							//����A�෴���־
							api_SetSysStatus(eSYS_STATUS_OPPOSITE_P_A);
						}	
					}
					else if( RemoteValue.P.l > 0 )
					{
						byBackFlagCnt = 0;
						//����෴���־
						api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P);
						//��A�෴���־
						api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P_A);
					}
				}
			}
			else
			{
				byBackFlagCnt = 0;
				//����෴���־
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P);
				//��A�෴���־
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P_A);
			}
			
			break;
		case 0x03:	//IA Ӧ���ȶ������ٶ����� ��Ϊ�����ķ�����Ҫ���ݹ����ж�
			Result = Link_ReadSampleReg(IARMS,ComData.b);
			if (Result == TRUE)
			{
				RemoteValue.I[0].l = (long)((double)ComData.d*1000.0/SampleCorr.I1Gain.d);
			}
			
			if(RemoteValue.I[0].l > 1600000) 
			{
				RemoteValue.I[0].l = 0;//����>160AΪ�쳣
			}
			
			//���������빦�ʱ���һ��
			if( RemoteValue.P.l<0 )
			{
				RemoteValue.I[0].l *= -1;
			}
			break;			
		case 0x04:	//IB  
			Link_ReadSampleReg(IBRMS,ComData.b);
			RemoteValue.I[1].l = (long)((double)ComData.d*1000.0/SampleCorr.I2Gain.d);

			if( (RemoteValue.I[1].l>1600000) || (RemoteValue.I[1].l<Def_Shielding_I2) )
			{ 
				RemoteValue.I[1].l = 0;//����>160AΪ�쳣
			}
			
			//�����й����ʶ�ȡ��ȷ, ���ߵ������������߹��ʷ���Ϊ׼
			if( Link_ReadSampleReg(PowerPB,ComData.b) != FALSE )
			{
				//���ߵ�������������--����Ϊ������Ϊ��
				if( ZeroCTPositive == YES )
				{
					if( ComData.b[2] & 0x80 )
					{
						RemoteValue.I[1].l *= -1;
					}
				}//���ߵ�������������--����Ϊ������Ϊ��
				else
				{
					if( !(ComData.b[2] & 0x80) )
					{
						RemoteValue.I[1].l *= -1;
					}
				}
			}//�����й����ʶ�ȡ����, ���ߵ��������Ի��߹��ʷ���Ϊ׼
			else
			{
				if( RemoteValue.P.l < 0 )
				{
					RemoteValue.I[1].l *= -1;
				}
			}
			break;
			
		case 0x05:	//S				
			RemoteValue.S.l = (RemoteValue.U.l/100)*(RemoteValue.I[0].l/10)/10;
			break;
			
		case 0x06:	//Cos
			if(RemoteValue.S.d)
			{
				RemoteValue.Cos.l = (long)(RemoteValue.P.l*10000.0/RemoteValue.S.l);
				if(labs(RemoteValue.Cos.l) > 9900)	
				{
					RemoteValue.Cos.l = 10000;
				}
			}
			else 
			{
				RemoteValue.Cos.l = 10000;
			}
			if(RemoteValue.P.l < 0)
			{
				RemoteValue.Cos.l *= -1;
			}
			break;
			
		case 0x07:	//PAV
			if( EMU_flag != 0xff )
			{
				if( (EMU_flag&0x04) == 0 ) 
				{
					gfPaverage += RemoteValue.P.l;	//�ۼ�ÿ����ƽ������ֵ
				}
				byPaverageCnt ++;
			}
			break;
			
		case 0x08:	//Freq
			//6M/6/2/Freq_U,2λС��
			Link_ReadSampleReg(Freq_U,ComData.b);
			// RemoteValue.Freq.l = 50000000/ComData.d;	
			RemoteValue.Freq.l = 46080000/ComData.d;
			if( SampleCtrlReg.FrequencyFlag == 0x5555 )
			{
				RemoteValue.Freq.l *= 2;
			}
			break;
		case 0x09:
			Result = Link_ReadSampleReg(HFConst,ComData.b);
			if( Result != TRUE ) 
			{
				break;
			}
				
			if( (ComData.w[0]!=SampleAdjustReg.RHFConst.w)
				&& (SampleAdjustReg.RHFConst.w!=0xffff) 
				&& (ComData.w[0]!=0xffff) )
			{
				if(byReWriteSampleChipRegCount < 12)
				{
					byReWriteSampleChipRegCount++;
					if(byReWriteSampleChipRegCount == 10)
					{
						// SoftResetSample(); //��λ����оƬ
						HardResetSample(); //Ӳ��λ����оƬ
						api_SetError(ERR_CHECK_5460_1);
					}
				}
				
				CheckSampleChipCtrlReg();  //�������ȸ�λ����оƬ �ٽ��мĴ���У��
				CheckSampleChipAdjustReg();

				if( byReWriteSampleChipRegCount > 1 )
				{
					api_WriteSysUNMsg(SAMPLE_CALI_PARA_ERR);
				}
			}
			else
			{
				byReWriteSampleChipRegCount = 0;
				api_ClrError(ERR_CHECK_5460_1);
			}
			break;
			
		default: //U, I[0], P, I2��Чֵ
			break;
	}	
	
	bySampleStep++;

	if( bySampleStep >= 0x0a )
	{
		//�ж��Ƿ����㶨ʱ�������幤��
		bySampleStep = 0xff;
	}	
		
	if( EMU_flag != 0xff )
	{       	
		//���������ж�
		if(EMU_flag & 0x04)
		{
			if( labs(RemoteValue.I[0].l) < Def_Shielding_I1 )
			{
				RemoteValue.I[0].l = 0;
				RemoteValue.S.l = 0;
				RemoteValue.Cos.l = 10000;
			}
			RemoteValue.P.l = 0;
		}
	}
}


//-----------------------------------------------
//��������: ���ݵ�ѹ״̬�ж��Ƿ�ص��������
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:�˺�����ûд��   
//-----------------------------------------------
static void Judge_U_Data(void)
{	
	//�쳣����(�ر��������)	�˴���δ�������������
	if( RemoteValue.U.l < Defendenergyvoltage )
	{
		//������
	}
	else
	{
		//������
	}
	
	//��ѹΪ������ѹ��ʮ��֮һ��Ϊ��ѹ����
	if(RemoteValue.U.l < (wStandardVoltageConst * 10)) //��ѹ������λС�� ������ѹ����һλС�� ����Ҫ��ʮ
	{
		if(byVoltageErrCount < 17)
		{
			byVoltageErrCount++;
			if(byVoltageErrCount == 15) //��У��HFConst�ӳ�5�� ��ֹһ��λоƬ
			{
				// SoftResetSample(); //��λ����оƬ
				HardResetSample(); //Ӳ��λ����оƬ
				CheckSampleChipCtrlReg();  //�������ȸ�λ����оƬ �ٽ��мĴ���У��
				CheckSampleChipAdjustReg();
			}
		}
	}
	else
	{
		byVoltageErrCount = 0;
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
static void CheckEmuErr(void)
{
	static BYTE	EmuErrCnt;                      //����оƬͨ�Ŵ��������
	BOOL Result;
	TFourByteUnion ComData;

	//7017 UARTͨѶ��У�� �����жϳ������Ƿ���ȷ ���ж�HFConstֵ ��Read_UIP�����ж�
	//�����ȡ������������FF��Ҳ������0�� Ҳ��������оƬ���ϵļ�¼
	//�����ѹ���� ���ּ�¼оƬ��λ ����CPUδ��λ�����ʵ�ּ���оƬ������
	Result = Link_ReadSampleReg(HFConst,ComData.b);
	
	if( Result != TRUE )
	{
		if(EmuErrCnt < 12 )
		{
			EmuErrCnt++;
			if( EmuErrCnt == 10 )
			{
				// SoftResetSample(); //��λ����оƬ
				HardResetSample(); //Ӳ��λ����оƬ
				api_SetError(ERR_CHECK_5460_1); //�ü���оƬͨ�Ź���
			}
		}
	}
	else
	{
		EmuErrCnt = 0;
		api_ClrError(ERR_CHECK_5460_1);
	}
}

//-----------------------------------------------
//��������: ����оƬUIPϵ��У��
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void CheckSampleCorr(void)			
{
	BOOL Result;
	
	if( SampleCorr.CRC32 != lib_CheckCRC32((BYTE*)&SampleCorr,sizeof(TSampleCorr)-sizeof(DWORD)) )
	{
		Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCorr),sizeof(TSampleCorr),(BYTE*)&SampleCorr);
		if(Result != TRUE)
		{
			SampleCorr.UGain.d = METER_Uk+Uoffset;
			SampleCorr.I1Gain.d = METER_I1k;
			SampleCorr.I2Gain.d = METER_I2k;
			SampleCorr.P1Gain.d = METER_Pk;
		}
	}
}


//-----------------------------------------------
//��������: ram�б���ļ���оƬУ��Ĵ���У��
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void CheckSampleAdjustRegRam(void)
{	
	BOOL Result;
	
	if( SampleAdjustReg.CRC32 != lib_CheckCRC32((BYTE*)&SampleAdjustReg,sizeof(TSampleAdjustReg)-sizeof(DWORD)) )
	{
		Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg),sizeof(TSampleAdjustReg),(BYTE*)&SampleAdjustReg);
		if(Result != TRUE)
		{
			memcpy((BYTE*)&SampleAdjustReg,(BYTE*)SampleAdjustDefData,sizeof(TSampleAdjustReg)-sizeof(DWORD));
		}
	}
}

//-----------------------------------------------
//��������: ram�б���ļ���оƬ�Ĵ���У��
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void CheckSampleCtrlRegRam(void)
{	
	BOOL Result;
	TFourByteUnion ComData;
	
	if( SampleCtrlReg.CRC32 != lib_CheckCRC32((BYTE*)&SampleCtrlReg,sizeof(TSampleCtrlReg)-sizeof(DWORD)) )
	{
		Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg),sizeof(TSampleCtrlReg),(BYTE*)&SampleCtrlReg);
		if(Result != TRUE)
		{
        	Link_ReadSampleReg(DeviceID,ComData.b);
        	if( ComData.d == 0x705304 )//5000:1оƬ
        	{
        		memcpy( (BYTE *)&SampleCtrlReg, (BYTE *)&SampleCtrlDefData, sizeof(TSampleCtrlReg)-sizeof(DWORD) );
        	}
        	else//8000:1оƬ
        	{
              //����
        		memcpy( (BYTE *)&SampleCtrlReg, (BYTE *)&SampleCtrlDefData8000, sizeof(TSampleCtrlReg)-sizeof(DWORD) );
        	}
		}
	}
}

//---------------------------------------------------------------
//��������: RAM�б���ĵ�ѹӰ��������У��
//
//����:     ��
//                   
//����ֵ:    ��
//
//��ע:   
//---------------------------------------------------------------
static  void CheckSampleVolRam( void )
{
	BOOL Result;
	BYTE i;
	
	if( SampleVolCorr.CRC32 != lib_CheckCRC32((BYTE*)&SampleVolCorr,sizeof(TSampleVolCorr)-sizeof(DWORD)) )
	{
		Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleVolCorr),sizeof(TSampleVolCorr),(BYTE*)&SampleVolCorr);
		if(Result != TRUE)
		{
			//�����EEPROM�ָ����� ����5% 1.0��Ĭ��ϵ��
			for( i = 0; i < MAX_VOLCORR_OFFSET;i++)
			{
				SampleVolCorr.RP1OFFSET[i].w = SampleAdjustReg.RP1OFFSET.w;
				SampleVolCorr.RP1OFFSETL[i].w = SampleAdjustReg.RP1OFFSETL.w;
			}
			for( i = 0; i < MAX_VOLCORR_RGPHS; i++ )
			{
				SampleVolCorr.RGPhs1[i].w = SampleAdjustReg.RGPhs1.w;
			}
		}
   }
}
//-----------------------------------------------
//��������: Ч�����оƬ�Ŀ���������
//
//����:		��
//
//����ֵ: 	��
//
//��ע:   ����������Ϊ�̶�ֵ��eeprom��û��
//-----------------------------------------------
static void CheckSampleChipCtrlReg(void)	
{
	BYTE i;
	BYTE ErrCount = 0;
	BOOL Result;
	TFourByteUnion ComData;
	
	CLEAR_WATCH_DOG;
	
	CheckSampleCtrlRegRam();
	
	for( i = 0; i < sizeof(SampleCtrlAddr); i++ )
	{
		if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
		{
			return;
		}
		
		Result = Link_ReadSampleReg(SampleCtrlAddr[i],ComData.b);
		if( Result != TRUE )
		{
			ErrCount++;

			//��������ͨѶ���� �˳����μĴ���У�� �������оƬͨѶ�쳣��Ῠ�ٵ�����
			if( ErrCount > 3 ) //ͨ�Ŵ������3��
			{
				return;
			}
				
			continue;			
		}
		else
		{
			ErrCount = 0;
		}

		if((ComData.b[0]!=SampleCtrlReg.EMUCFG.b[i*2]) || (ComData.b[1]!=SampleCtrlReg.EMUCFG.b[i*2+1]))
		{
			if( SampleCtrlAddr[i] < 0x50 ) 
			{
				SampleSpecCmdOp(0xbc);
			}
			else 
			{
				SampleSpecCmdOp(0xa6);
			}
			Link_WriteSampleReg(SampleCtrlAddr[i], (BYTE*)&SampleCtrlReg.EMUCFG.b[i*2]);
			SampleSpecCmdOp(0x00);
		}
	}
}

//---------------------------------------------------------------
//��������: �õ���ѹӰ���������ȼ�
//
//����: 	Mode:
//			 0x00-��У���� ���ص�ǰ��ѹ�ȼ�
//			 0x01-�������� �����ж����� ��ֹ��ѹ����
//
//����ֵ:   0���޵�ѹ���� ������220V
//         ���---------����-----��������----��ѹ�ȼ�
//         1~2  |  Imin  -  5%   | 1.0  | 90%/110%
//         3~5  |  Itr   -  10%  | 1.0  | 80%/85%/115%
//         6~7  |  Itr   -  10%  | 0.5L | 90%/110%
//         8~9  |  10Itr -  100% | 0.5L | 90%/110%
//         10~11|  Imax  -  Imax | 0.5L | 90%/110%
//
//��ע:
//---------------------------------------------------------------
BYTE GetVolCorrectLevel( BYTE Mode )
{
	BYTE i;
	BYTE VolLevel, CalCorrectLevel;
	long I_Abs;
	static BYTE LastCmpLevel;		//��һ�αȽ�ֵ �����ж����εõ�����ֵ
	static BYTE	CurCorrectLevel;	//��ǰ����ֵ ���״̬δ�� ���ص�ǰ����ֵ
	
	if( CurCorrectLevel > MAX_VOLCORR_STEP )
	{
		CurCorrectLevel = 0;
	}
	
	//��ѹ�ж�
	VolLevel = 0;
	CalCorrectLevel = 0;
	
	//���-��ѹ
	// 1 - 0.8Un 
	// 2 - 0.85Um 
	// 3 - 0.9Un 
	// 4 - 1.1Un
	// 5 - 1.15Un
	for( i = 0; i < (sizeof(DefVolCorrectRange) / sizeof(DefVolCorrectRange[0])); i++ )
	{
		if( (RemoteValue.U.l > (DefVolCorrectRange[i][0]*100)) && (RemoteValue.U.l <= DefVolCorrectRange[i][1]*100) )
		{
			VolLevel = i + 1;
			break;
		}
	}
	//�����͹��������ж�
	I_Abs = labs( RemoteValue.I[0].l );
	
	if( VolLevel == 0 )
	{
		CalCorrectLevel = 0;
	}
	else
	{
		//0.5L
		if( (labs(RemoteValue.Cos.l) > 4000) && (labs(RemoteValue.Cos.l) < 6000) ) //�жϹ��������Ƿ�Ϊ0.5L
		{
			//0.5L��ֻ�� 0.9Un��1.1Un
			if( (VolLevel == 3) || (VolLevel == 4) )
			{
				//Itr - 10% 5000
				if( (I_Abs > 4000) && (I_Abs < 7500) )
				{
					CalCorrectLevel = VolLevel + 3; 
				}
				//10Itr - 100% 50000
				else if( (I_Abs > 40000) && (I_Abs < 60000) )
				{
					CalCorrectLevel = VolLevel + 5;
				}
				//Imax 600000
				else if( I_Abs > 500000 )
				{
					CalCorrectLevel = VolLevel + 7;
				}
			}
		}
		//1.0
		else if( labs(RemoteValue.Cos.l) > 9000 ) 
		{
			//Imin 5% 2500
			if( (I_Abs > 2000) && (I_Abs < 3000) )
			{
				//0.9Un 1.1Un
				if( (VolLevel == 3) || (VolLevel == 4) )
				{
					CalCorrectLevel = VolLevel - 2;
				}
			}
			//Itr 10% 5000
			else if( (I_Abs > 4000) && (I_Abs < 7500) )
			{
				//0.8Un
				if( VolLevel == 1 )
				{
					CalCorrectLevel = 3;
				}
				//0.85Un
				else if( VolLevel == 2 )
				{
					CalCorrectLevel = 4; 
				}
				//1.15Un
				else if( VolLevel == 5 )
				{
					CalCorrectLevel = 5;
				}
			}
		}
	}
	
	//������ ����������ֵ ��Ϊ0
	if( CalCorrectLevel > MAX_VOLCORR_STEP )
	{
		CalCorrectLevel = 0; 
	}

	//ֻ�����������жϽ��һ�� ����Ϊ�����Ѿ��л�
	if( LastCmpLevel == CalCorrectLevel )
	{
		CurCorrectLevel = CalCorrectLevel;
	}
	else
	{
		LastCmpLevel  = CalCorrectLevel;
	}
	//��У����ֱ�ӷ��ص�ǰ��ѹ�ȼ� �����������ж�
	if( Mode == 0 )
	{
		return CalCorrectLevel;
	}
	
	return CurCorrectLevel;
}

//---------------------------------------------------------------
//��������: ��ѹӰ������������
//
//����:    ��
//
//����ֵ:  ��
//
//��ע:    ����1�����һ��
//         ���---------����-----��������----��ѹ�ȼ�
//         1~2  |  Imin  -  5%   | 1.0  | 90%/110%
//         3~5  |  Itr   -  10%  | 1.0  | 80%/85%/115%
//         6~7  |  Itr   -  10%  | 0.5L | 90%/110%
//         8~9  |  10Itr -  100% | 0.5L | 90%/110%
//         10~11|  Imax  -  Imax | 0.5L | 90%/110%
//---------------------------------------------------------------
static void CorrectVolInflu( void )
{
	BYTE VolCorrectLevel;	//��ѹ�����ȼ�
	
	if( SelVolCorrectConst != YES )
	{
		return;
	}
	
	//��ѹӰ��������У��
	CheckSampleVolRam();
	VolCorrectLevel = GetVolCorrectLevel( 1 );
	
	//�ϴ��Ѿ���У�� �����ٴε�У
	if( VolCorrectLevel == byLastVolCorrectLevel )
	{
		return;
	}

	//1.0���� ����OFFSET
	if( (VolCorrectLevel > 0) && (VolCorrectLevel <= MAX_VOLCORR_OFFSET) ) //1-5
	{
		SampleSpecCmdOp( 0xa6 );
		Link_WriteSampleReg( P1OFFSET, SampleVolCorr.RP1OFFSET[VolCorrectLevel-1].b );
		Link_WriteSampleReg( P1OFFSETL, SampleVolCorr.RP1OFFSETL[VolCorrectLevel-1].b );
		SampleSpecCmdOp(0x00);
	}
	else
	{
		SampleSpecCmdOp(0xa6);
		Link_WriteSampleReg( P1OFFSET, SampleAdjustReg.RP1OFFSET.b );
		Link_WriteSampleReg( P1OFFSETL, SampleAdjustReg.RP1OFFSETL.b );
		SampleSpecCmdOp(0x00);	
	}
	
	//0.5L���� ����Gphse1
	if( (VolCorrectLevel > MAX_VOLCORR_OFFSET) && (VolCorrectLevel <= MAX_VOLCORR_STEP) ) //6-11
	{
		SampleSpecCmdOp( 0xa6 );
		Link_WriteSampleReg( GPhs1, SampleVolCorr.RGPhs1[VolCorrectLevel - MAX_VOLCORR_OFFSET - 1].b );
		SampleSpecCmdOp(0x00);
	}
	else
	{
		SampleSpecCmdOp( 0xa6 );
		Link_WriteSampleReg( GPhs1, SampleAdjustReg.RGPhs1.b );
		SampleSpecCmdOp(0x00);
	}

	byLastVolCorrectLevel = VolCorrectLevel;	//��¼�ϴε�ѹ�����ȼ�
}

//-----------------------------------------------
//��������: Ч�����оƬ��У��������
//
//����:		��
//
//����ֵ: 	��
//
//��ע:   У�������ݱ�����eeprom�У�ram��Ҳ��һ��
//-----------------------------------------------
static void CheckSampleChipAdjustReg(void)
{
	BYTE i, SetFlag;
	BYTE VolCorrectLevel;	//��ѹ�����ȼ�
	BYTE ErrCount = 0;
	BOOL Result;
	TFourByteUnion ComData;
	TTwoByteUnion RegData;
	
	CLEAR_WATCH_DOG;
	
	CheckSampleAdjustRegRam();
	
	for(i=0;i<sizeof(SampleAdjustAddr);i++)
	{
		if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
		{
			return;
		}

		//5000:1��HT7017У��Ĵ���0x58(�޹���λУ��)��0x64(�Ʋ�������λУ��)������д�����оƬ
		if( (SampleAdjustAddr[i] == QPhsCal) || (SampleAdjustAddr[i] == DEC_Shift) )
		{
			//��оƬID
			Result = Link_ReadSampleReg( DeviceID, ComData.b );
			//��ȡʧ��, ��������ѭ��
			if( Result != TRUE )
			{
				continue;
			}
			else//��ȡ�ɹ�
			{
				if( ComData.d == 0x705304 ) //5000:1оƬ
				{
					continue;//��������ѭ��
				}
				else //8000:1оƬ
				{
					//������, ��������ִ��
				}
			}
		}
		
		Result = Link_ReadSampleReg( SampleAdjustAddr[i], ComData.b );
		if( Result != TRUE )
		{	
			ErrCount++;
            
            if( ErrCount > 3 ) //ͨ�Ŵ������3��
            {
                return;
            }
            
			continue;			
		}
		else
		{
			ErrCount = 0;
		}
						  
		//������ѹӰ���� �ڶ�Ӧ�� P1OFFSET��P1OFFSETL��GPhs1���⴦��
		if( SelVolCorrectConst == YES )
		{
			SetFlag = 0;
			VolCorrectLevel = GetVolCorrectLevel( 1 );
			
			if( (VolCorrectLevel > 0)&&(VolCorrectLevel <= MAX_VOLCORR_OFFSET) )
			{
				if( SampleAdjustAddr[i] == P1OFFSET )
				{
					SetFlag = 0xAA; 
					if( ComData.w[0] != SampleVolCorr.RP1OFFSET[VolCorrectLevel-1].w )
					{
						SetFlag = 0x55;
						RegData.w = SampleVolCorr.RP1OFFSET[VolCorrectLevel-1].w;
					}
				}
				else if( SampleAdjustAddr[i] == P1OFFSETL )
				{
					SetFlag = 0xAA; 
					if( ComData.w[0] != SampleVolCorr.RP1OFFSETL[VolCorrectLevel - 1].w )
					{
						SetFlag = 0x55;
						RegData.w = SampleVolCorr.RP1OFFSETL[VolCorrectLevel-1].w;
					}
				}
			}
			else if( ((VolCorrectLevel > MAX_VOLCORR_OFFSET) && (VolCorrectLevel <= MAX_VOLCORR_STEP)) && (SampleAdjustAddr[i] == GPhs1) )
			{
				SetFlag = 0xAA; 
				if( ComData.w[0] != SampleVolCorr.RGPhs1[VolCorrectLevel - 1 - MAX_VOLCORR_OFFSET].w )
				{
					SetFlag = 0x55;
					RegData.w = SampleVolCorr.RGPhs1[VolCorrectLevel - 1 - MAX_VOLCORR_OFFSET].w;
				}
			}
			
			if( SetFlag == 0x55 )
			{
				SampleSpecCmdOp(0xa6);
				Link_WriteSampleReg( SampleAdjustAddr[i], RegData.b );
				SampleSpecCmdOp(0x00);
			}
            
			if( SetFlag != 0 )
            {
               continue;
            }
		}
		
		if( (ComData.b[0]!=SampleAdjustReg.GP1.b[i*2]) || (ComData.b[1]!=SampleAdjustReg.GP1.b[i*2+1]) )
		{
			SampleSpecCmdOp(0xa6);
			Link_WriteSampleReg(SampleAdjustAddr[i],(BYTE*)&SampleAdjustReg.GP1.b[i*2]);
			SampleSpecCmdOp(0x00);
		}
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
	byEnergyPulse++;
}


//-----------------------------------------------
//��������: �����������
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void ProcEnergyPulse(void)
{
	BYTE IntStatus;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	BYTE byEnergyPulseBak;               //���������������
	BYTE DataSign,CurRatio;
	DWORD TotalEnergy1,TotalEnergy2,RatioEnergy1,RatioEnergy2;
	#endif
	
	if( byEnergyPulse > MaxEnergyPulseConst )
	{
		IntStatus = api_splx(0);
		byEnergyPulse = 0;
		api_splx(IntStatus);
	}
	
	if( byEnergyPulse != 0 )
	{
		IntStatus = api_splx(0);
		byEnergyPulseBak = byEnergyPulse;
		byEnergyPulse = 0;
		api_splx(IntStatus);
		
		#if( PREPAY_MODE == PREPAY_LOCAL )
		CurRatio = api_GetCurRatio();
		if(CurRatio > MAX_RATIO)
		{
			CurRatio = DefCurrRatioConst;
		}
		#endif
		
		if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_A ) == FALSE) 
		{			
			#if( PREPAY_MODE == PREPAY_LOCAL )
			DataSign = P_ACTIVE;
			api_GetCurrEnergyData( PHASE_ALL + P_ACTIVE, DATA_HEX, 0, 2, (BYTE *)&TotalEnergy1 );
			api_GetCurrEnergyData( PHASE_ALL + P_ACTIVE, DATA_HEX, CurRatio, 2, (BYTE *)&RatioEnergy1 );
			#endif
			api_WritePulseEnergy( PHASE_ALL + P_ACTIVE, byEnergyPulseBak ); // ����
			#if( PREPAY_MODE == PREPAY_LOCAL )
			api_GetCurrEnergyData( PHASE_ALL + P_ACTIVE, DATA_HEX, 0, 2, (BYTE *)&TotalEnergy2 );
			api_GetCurrEnergyData( PHASE_ALL + P_ACTIVE, DATA_HEX, CurRatio, 2, (BYTE *)&RatioEnergy2 );
			#endif
		}
		else
		{		
			#if( PREPAY_MODE == PREPAY_LOCAL )
			DataSign = N_ACTIVE;
			api_GetCurrEnergyData( PHASE_ALL + N_ACTIVE, DATA_HEX, 0, 2, (BYTE *)&TotalEnergy1 );
			api_GetCurrEnergyData( PHASE_ALL + N_ACTIVE, DATA_HEX, CurRatio, 2, (BYTE *)&RatioEnergy1 );
			#endif
			api_WritePulseEnergy( PHASE_ALL + N_ACTIVE, byEnergyPulseBak ); // ����
			#if( PREPAY_MODE == PREPAY_LOCAL )
			api_GetCurrEnergyData( PHASE_ALL + N_ACTIVE, DATA_HEX, 0, 2, (BYTE *)&TotalEnergy2 );
			api_GetCurrEnergyData( PHASE_ALL + N_ACTIVE, DATA_HEX, CurRatio, 2, (BYTE *)&RatioEnergy2 );
			#endif
		}

		#if( PREPAY_MODE == PREPAY_LOCAL )
		if(TotalEnergy2 > TotalEnergy1)
		{
			TotalEnergy1 = TotalEnergy2 - TotalEnergy1;
		}
		else
		{
			TotalEnergy1 = 0;
		}

		if(RatioEnergy2 > RatioEnergy1)
		{
			RatioEnergy1 = RatioEnergy2 - RatioEnergy1;
		}
		else
		{
			RatioEnergy1 = 0;
		}

		if((TotalEnergy1 != 0)||(RatioEnergy1 != 0))
		{
			api_ReduceRemainMoney( DataSign, TotalEnergy1, RatioEnergy1 );
		}
		#endif

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
BOOL api_InitSampleChip( void )
{
    return TRUE;
}
//--------------------------------------------------
//��������:  ��ʼ������оƬ���������������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_InitSampleWave( void )
{
	TTwoByteUnion SPIOpData;

	//���ô��������һ֡�����а����Ĳ������ݵ���
	Link_OpenSampleUartReg(0x01);
	// Link_OpenSampleUartReg(0x2);

	SampleSpecCmdOp(0xbc);

	//�����ܲ���������----Ŀ��һ�ܲ�128�㣬5.5296M��7053D��Ĭ������921600���
	// SPIOpData.w = 0x90;
	SPIOpData.w = 0x120;
	Link_WriteSampleReg( 0x39,SPIOpData.b);
	//������Ƶ��������
	SPIOpData.w = 0x0803;
	Link_WriteSampleReg( 0x38,SPIOpData.b);

	//�������������
	SPIOpData.w = 0x860;
	Link_WriteSampleReg( 0x4A, SPIOpData.b);
	SPIOpData.w = 0x869;
	// // SPIOpData.w = 0x889;
	// SPIOpData.w = 0x809; //����Uͨ��
	Link_WriteSampleReg( 0x4A, SPIOpData.b);

	SPIOpData.w = 0x00; //41�Ĵ���
	Link_WriteSampleReg( 0x41, SPIOpData.b);

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
	BYTE AdcCon[2] = {0x00, 0x00}; 
	TTwoByteUnion SPIOpData;

	byReWriteSampleChipRegCount = 0;
	byLastVolCorrectLevel = 0xFF;
		
	#if( SEL_STAT_V_RUN == YES )//�ָ���ѹ�ϸ���
	PowerUpVRunTime( );
	#endif
	// �ο��𹤣�����λ�Ĵ���
	SoftResetSample();
	api_Delayms(10);
	
	CheckSampleChipCtrlReg();
	CheckSampleChipAdjustReg();
	CheckSampleCorr();	

	SampleSpecCmdOp(0xa6);
	Link_WriteSampleReg( 0x59, AdcCon);
	SampleSpecCmdOp(0x00);

	AdcCon[0] = SampleAdjustReg.RADCCON.b[0];
	AdcCon[1] = SampleAdjustReg.RADCCON.b[1];
	
	SampleSpecCmdOp(0xa6);
	Link_WriteSampleReg( 0x59, AdcCon);	
	SampleSpecCmdOp(0x00);

	#if( SEL_WAVE_OUTPUT == YES )
	api_InitSampleWave();	
	#endif
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
	memset(RemoteValue.U.b,0x00,sizeof(TRemoteValue));
	RemoteValue.Cos.l = 10000;
	
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

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);

	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )//5s����е�ѹ�ϸ����ۼƣ�������ʱ�䲻Ϊ1440
	{
		CheckSampleChipCtrlReg();
		#if( SEL_STAT_V_RUN == YES )//�����ѹ�ϸ��ʲ���		
		StatVRunTime();
		#endif
	}
	
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+1) )
	{
		CheckSampleChipAdjustReg();		
	}
	
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )
	{
		CheckSampleCorr();
		//ÿ���Ӽ���һ��ƽ������
		CalPaverage();	
		
		#if( SEL_AHOUR_FUNC == YES )
		if(t.Min == 26)
		{
			SwapAHour();
		}
		#endif
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

	//����оƬ���ϼ��
	//�����ѹ�������ּ���оƬ��λ ����CPUû�и�λ����� ���ܻ���ּ���оƬ���ϵ���
	CheckEmuErr();  //���ܷŵ�Read_UIP ��Ϊ���ּ���ͨѶ����Read_UIPִ�зǳ�����

	Judge_U_Data();
	
	//��ѹӰ��������
	CorrectVolInflu();
	
	#if( SEL_AHOUR_FUNC == YES )
	AccumulateAHour();
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
void api_SampleTask(void)
{	
	TRealTimer t;
	TTwoByteUnion  SPIOpData2;
	
	ProcEnergyPulse();
	
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
	//��˲ʱUIPֵ	
	Read_UIP();	
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
//				5:N������				PHASE_N2
//				6:A��				����PHASE_A
//				7:B��				����PHASE_B	
//				8:C��				����PHASE_C	
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
//						0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F	10
const BYTE MaxDot[] = 	{3,	4,	1,	1,	6,	6,	6,	6,	6,	6,	4,	2,	2,	2,	2,	2,	4};
const BYTE DefDot[] = 	{1,	3,	1,	1,	4,	4,	4,	4,	4,	4,	3,	2,	2,	2,	2,	2,	3};
const BYTE MaxLen[] = 	{4,	4,	2,	2,	4,	4,	4,	4,	4,	4,	4,	2,	2,	2,	2,	2,	4};
BOOL api_GetRemoteData(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *Buf)
{
	TFourByteUnion td;
	BYTE ReadDot,BackFlag;
	
	//�����ֻ֧���ܺ�A��,��Ϊ����������ΪPHASE_N
	if( ((Type&0xf000)!=PHASE_ALL) && ((Type&0xf000)!=PHASE_A) && (Type!=(REMOTE_I+PHASE_RA)) && (Type!=(REMOTE_I+PHASE_N)) )
	{
		return FALSE;
	}
	
	if( Len > MaxLen[Type&0x001f] )
	{
		return FALSE;
	}
	
	td.d = 0;
	
	switch( Type & 0x00ff )
	{
		case REMOTE_U:
			memcpy(td.b,RemoteValue.U.b,4);
			break;
		case REMOTE_I:			
			if(( (Type&0xf000) == PHASE_A ) || ( (Type&0xf000) == PHASE_RA ))//����������������ȫ������
			{
				td.d = RemoteValue.I[0].l;
			}
			else if((Type&0xf000) == PHASE_N)//����������ߵ�������
			{
				td.d = RemoteValue.I[1].l;
			}	
			else			
			{				
				return FALSE;				
			}
			break;
		case REMOTE_P:
			td.d = RemoteValue.P.l;
			break;
		case REMOTE_S:
			memcpy(td.b,RemoteValue.S.b,4);
			break;
		case REMOTE_P_AVE:
			memcpy(td.b,RemoteValue.Pav.b,4);
			break;
		case REMOTE_COS:
			td.d = RemoteValue.Cos.l;
			break;
		case REMOTE_HZ:
			memcpy(td.b,RemoteValue.Freq.b,4);
			break;
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
	WORD tw;
	
	tw = 0;
	
	if(Type == 0x20)
	{
		if(EMU_flag&0x04)
		{
			tw = 1;
		}	
	}
	else if( Type == 0x40 )
	{
        if( labs(RemoteValue.S.l) >= (220*5*4) )//U*I*0.004
        {
            return 0;
        }
        else
        {
            return 1;
        }
	}
	else
	{
		ASSERT(0,0);
		tw = 0xffff;
	}
	
	return tw;
}


#endif// #if( SAMPLE_CHIP == CHIP_HT7017 )
