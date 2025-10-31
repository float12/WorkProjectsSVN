//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	
//��������	2016.7.30
//����		����оƬRN2026У���ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "RN8302B.h"


#if( SAMPLE_CHIP == CHIP_RN8302B )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
DWORD	dwBigStandardCurrent;		//�����У����Ӧ����
DWORD	dwSmallStandardCurrent;		//С���У����Ӧ����
DWORD	dwMaxCurrent;				//���������У����Ӧ����
DWORD	AdjustDivCurrent[9];		//�ֶ�У�������׼ֵ
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ��ʼ��У���׼Դ�ĵ�ѹ������
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע: 
//-----------------------------------------------
void InitSampleCaliUI( void )
{	
	//��ʼ��У�����	
	//������� 0.3(1.2)
	if( MeterCurrentTypesConst == CURR_1A )
	{
		dwBigStandardCurrent = 1500;
		dwSmallStandardCurrent = 200;
		
		//����򿪷ֶβ���
		if( SelDivAdjust == YES )
		{
			//0.3(1.2)A�ֶ�У׼������   
			AdjustDivCurrent[0] = 1500;		 //0         1.5A        IMax   
			AdjustDivCurrent[1] = 600;       //1         0.6A        0.5IMax
			AdjustDivCurrent[2] = 300;       //2         0.3A        Ib     
			AdjustDivCurrent[3] = 200;       //3         0.20A       50%Ib  
			AdjustDivCurrent[4] = 60;        //4         0.06A       20%Ib  
			AdjustDivCurrent[5] = 30;        //5         0.03A       10%Ib  
			AdjustDivCurrent[6] = 15;        //6         0.015A      5%Ib   
			AdjustDivCurrent[7] = 6;         //7         0.006A      2%Ib   
			AdjustDivCurrent[8] = 3;         //8         0.003A      1%Ib   
		
			//9���ͻ�����������
			CurrentValue[0].Upper = 1089;
			CurrentValue[0].Lower = 859; 
										  
			CurrentValue[1].Upper = 523; 
			CurrentValue[1].Lower = 400; 
										  
			CurrentValue[2].Upper = 243; 
			CurrentValue[2].Lower = 221; 
										  
			CurrentValue[3].Upper = 123; 
			CurrentValue[3].Lower = 86;  
										  
			CurrentValue[4].Upper = 52;  
			CurrentValue[4].Lower = 46;  
										  
			CurrentValue[5].Upper = 21;  
			CurrentValue[5].Lower = 17;  
										  
			CurrentValue[6].Upper = 12;  
			CurrentValue[6].Lower = 9; 
									
			CurrentValue[7].Upper = 5;
			CurrentValue[7].Lower = 4;
										  
			CurrentValue[8].Upper = 2;
			CurrentValue[8].Lower = 1;
		}
	}
	//������� 1.5(6)
	else if( MeterCurrentTypesConst == CURR_6A)
	{
		dwBigStandardCurrent = 1500;
		dwSmallStandardCurrent = 300;
		
		if( SelDivAdjust == YES )
		{
			//У��������ʼ��
			AdjustDivCurrent[0] = 6000;			 //0         6A          IMax     
			AdjustDivCurrent[1] = 3000;          //1         3A          0.5IMax  
			AdjustDivCurrent[2] = 1500;          //2         1.5A        Ib       
			AdjustDivCurrent[3] = 750;           //3         0.75A       50%Ib    
			AdjustDivCurrent[4] = 300;           //4         0.3A        20%Ib    
			AdjustDivCurrent[5] = 150;           //5         0.15A       10%Ib    
			AdjustDivCurrent[6] = 75;            //6         0.075A      5%Ib     
			AdjustDivCurrent[7] = 30;            //7         0.03A       2%Ib     
			AdjustDivCurrent[8] = 15;            //8         0.015A      1%Ib     
			
			//9���ͻ�����������
			CurrentValue[0].Upper = 5477;
			CurrentValue[0].Lower = 5143;
	
			CurrentValue[1].Upper = 2377;
			CurrentValue[1].Lower = 2043;
	
			CurrentValue[2].Upper = 1377;
			CurrentValue[2].Lower = 1243;
	
			CurrentValue[3].Upper = 423; 
			CurrentValue[3].Lower = 347; 
	
			CurrentValue[4].Upper = 240; 
			CurrentValue[4].Lower = 187; 
	
			CurrentValue[5].Upper = 112; 
			CurrentValue[5].Lower = 87;  
	
			CurrentValue[6].Upper = 52;  
			CurrentValue[6].Lower = 42;  
	
			CurrentValue[7].Upper = 22;  
			CurrentValue[7].Lower = 18; 
	
			CurrentValue[8].Upper = 7;  
			CurrentValue[8].Lower = 4; 
		}
	}
	//������� 5(20) ���� ������� 10(100)
	else if(MeterCurrentTypesConst == CURR_20A)
	{
		dwBigStandardCurrent = 5000;
		dwSmallStandardCurrent = 1000;
		
		if( SelDivAdjust == YES )
		{
			//У��������ʼ��
			AdjustDivCurrent[0] = 60000;		//0         60A         IMax      
			AdjustDivCurrent[1] = 30000;        //1         30A         0.5IMax   
			AdjustDivCurrent[2] = 6000;         //2         6A          Ib        
			AdjustDivCurrent[3] = 2500;         //3         2.5A        50%Ib     
			AdjustDivCurrent[4] = 1000;         //4         1A          20%Ib     
			AdjustDivCurrent[5] = 500;          //5         0.5A        10%Ib     
			AdjustDivCurrent[6] = 250;          //6         0.25A       5%Ib      
			AdjustDivCurrent[7] = 100;          //7         0.1A        2%Ib      
			AdjustDivCurrent[8] = 50;           //8         0.05A       1%Ib      
			
			//9���ͻ�����������
			CurrentValue[0].Upper = 41732;
			CurrentValue[0].Lower = 35732;
										   
			CurrentValue[1].Upper = 14320;
			CurrentValue[1].Lower = 11320;
										   
			CurrentValue[2].Upper = 3732; 
			CurrentValue[2].Lower = 3232; 
										   
			CurrentValue[3].Upper = 1732; 
			CurrentValue[3].Lower = 1232; 
										   
			CurrentValue[4].Upper = 773;  
			CurrentValue[4].Lower = 573;  
										   
			CurrentValue[5].Upper = 400;  
			CurrentValue[5].Lower = 340;  
										   
			CurrentValue[6].Upper = 210;  
			CurrentValue[6].Lower = 170;  
										   
			CurrentValue[7].Upper = 80;   
			CurrentValue[7].Lower = 60;   
										   
			CurrentValue[8].Upper = 30;   
			CurrentValue[8].Lower = 10;   
		}
	}
	//������� 50(250) 125A/160A/200A/250A
	else if(MeterCurrentTypesConst == CURR_500A)
	{
		dwMaxCurrent = 500000; //�ݶ�200A
		dwBigStandardCurrent = 50000*2;
		dwSmallStandardCurrent = 10000*2;
		
		if( SelDivAdjust == YES )
		{
			//У��������ʼ��
			AdjustDivCurrent[0] = 500000*2;		//0         IMax      
			AdjustDivCurrent[1] = 250000*2;       //1         0.5IMax   
			AdjustDivCurrent[2] = 100000*2;        //2         Ib        
			AdjustDivCurrent[3] = 50000*2;        //3         50%Ib     
			AdjustDivCurrent[4] = 20000*2;        //4         20%Ib     
			AdjustDivCurrent[5] = 1000*2;         //5         10%Ib     
			AdjustDivCurrent[6] = 5000*2;         //6         5%Ib      
			AdjustDivCurrent[7] = 2000*2;         //7         2%Ib      
			AdjustDivCurrent[8] = 1000*2;          //8         1%Ib      
			
			//9���ͻ�����������
			CurrentValue[0].Upper = 171732*2*2;
			CurrentValue[0].Lower = 165732*2*2;
										   
			CurrentValue[1].Upper = 90320*2*2;
			CurrentValue[1].Lower = 84320*2*2;
										   
			CurrentValue[2].Upper = 39732*2*2; 
			CurrentValue[2].Lower = 36232*2*2; 
										   
			CurrentValue[3].Upper = 19732*2*2; 
			CurrentValue[3].Lower = 16232*2*2; 
										   
			CurrentValue[4].Upper = 7973*2*2;  
			CurrentValue[4].Lower = 7373*2*2;  
										   
			CurrentValue[5].Upper = 3900*2*2;  
			CurrentValue[5].Lower = 3440*2*2;  
										   
			CurrentValue[6].Upper = 1910*2*2;  
			CurrentValue[6].Lower = 1570*2*2;  
										   
			CurrentValue[7].Upper = 800*2*2;   
			CurrentValue[7].Lower = 710*2*2;   
										   
			CurrentValue[8].Upper = 350*2*2;   
			CurrentValue[8].Lower = 200*2*2;   
		}
	}
}


//---------------------------------------------------------------
//��������: EDT��׼ֵ���浽EEPROM //@liang
//
//����:    ��
//
//����ֵ:  ��
//
//��ע:
//---------------------------------------------------------------
void WriteEEpromEDTDataPara( void )
{
	api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.EDTDataPara ), sizeof(TEDTDataPara), (BYTE *)&EDTDataPara );
	api_ClrSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
}

//-----------------------------------------------
//��������: ��λ��Ӧ�����Ĵ���
//
//����:		Reg_Addr[in]	�Ĵ�����ַ
//
//����ֵ: 	��
//
//��ע: ��λ��У������
//-----------------------------------------------
void ResetSampleReg(DWORD Reg_Addr,BYTE len, BYTE chipNo)
{
    DWORD Data;
	
	if((Reg_Addr == PHSIA_EMU)
		||(Reg_Addr == PHSIB_EMU)
		||(Reg_Addr == PHSIC_EMU))
	{
	  	Data = 0x20080200;
	}
	else if((Reg_Addr == PHSUB_EMU)
		||(Reg_Addr == PHSUC_EMU)
		||(Reg_Addr == PHSIN_EMU))
	{
		Data = 0x00000200;
	}
	else if(Reg_Addr == PHSUA_EMU)
	{
		Data = 0xA7000200;
	}
	 else
	{
	  	Data = 0;
	}
	WriteSampleReg( Reg_Addr, (BYTE *)&Data ,len, chipNo);
}
//-----------------------------------------------
//��������: У��ϵ�����浽EEPROM������ˢ�±�־
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void WriteEEpromSamplePara( eSampleType Type ,BYTE chipNo)			
{
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	if(( Type == eADJUST )||( Type == eALL ))
	{
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg[chipNo]),sizeof(TSampleAdjustReg),(BYTE*)&SampleAdjustReg[chipNo]);
	}
	
	if(( Type == eCRTL )||( Type == eALL ))
	{
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg),sizeof(TSampleCtrlReg),(BYTE*)&SampleCtrlReg);
	}

	if(( Type == eCORR )||( Type == eALL ))
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleCorr), sizeof(TSampleCorr), (BYTE*)&SampleCorr);
	}
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

}
//-----------------------------------------------
//��������: У���ʼ��
//
//����:		InBuf[in]	�����HFConstֵ
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void InitSamplePara( BYTE chipNo )
{	
	UseSampleDefaultPara(eALL, chipNo);

	WriteEEpromSamplePara(eALL, chipNo); //д��E2P

	//���³�ʼ������оƬ
	api_PowerUpSample(chipNo);
}
//--------------------------------------------------
// ��������:  У��ͬʱУ8306��ѹ����ϵ��
//
// ����:     
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL Adjust_UIK(float Vol, float Cur, BYTE j, BYTE chipNo)
{
	DWORD RegisterU[3] = {0, 0, 0}, RegisterI[3] = {0, 0, 0};
	BYTE i = 0, cmpval8[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	BOOL Result = FALSE;
	QWORD Sum[2] = {0, 0};
	float Ku = 0, K2 = 0, Gain = 0, Ureg[3] = {0, 0, 0};

	for (i = 0; i < 12; i++)
	{
		Link_ReadSampleReg(UA_EMU + j, (BYTE *)&RegisterU[j], 4, chipNo);
		Link_ReadSampleReg(IA_EMU + j, (BYTE *)&RegisterI[j], 4, chipNo);
		Sum[0] += ((QWORD)RegisterU[j]);
		Sum[1] += ((QWORD)RegisterI[j]);

		CLEAR_WATCH_DOG;

		api_Delayms(260);
	}
	RegisterU[j] = (DWORD)(Sum[0] / 12);
	RegisterI[j] = (DWORD)(Sum[1] / 12);

	// ���㹫ʽ
	if (memcmp(&RegisterU[j], &cmpval8[0], 4) != 0)
	{
		Ku = (float)(Vol / RegisterU[j]); // ��ѹ
		Gain = (float)((SWORD)SampleAdjustReg[chipNo].GSU[j].d / 32768.0);
		K2 = (float)(Ku * 32 * (1 + Gain));
		UIFactorArr[chipNo].Uk[j] = K2;
	}
	else
	{
		return FALSE;
	}

	if (memcmp(&RegisterI[j], &cmpval8[0], 4) != 0)
	{
		Ku = (float)(Cur / RegisterI[j]); // �� ��
		Gain = (float)((SWORD)SampleAdjustReg[chipNo].GSI[j].d / 32768.0);
		K2 = (float)(Ku * 32 * (1 + Gain));
		UIFactorArr[chipNo].Ik[j] = K2;
	}
	else
	{
		return FALSE;
	}

	// дE2P
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleUIKArr[chipNo].Uk[0]), sizeof(TSampleUIK), (BYTE *)&UIFactorArr[chipNo]);
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

	return Result;
}
//--------------------------------------------------
// ��������:  ����8306оƬ�ĵ�ѹ����ϵ��,�洢��E2P
//
// ����:
//
// ����ֵ:
//
// ��ע:��Ҫ����У�����Ϊ 220v
//--------------------------------------------------
BOOL api_UI_Kfor8306(BYTE *Buf, BYTE chipNo)
{
	DWORD dvval, dival, dpval;
	BYTE i = 0, j = 0, cmpval8[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	BYTE *data = Buf + 2;
	BOOL Result = FALSE;
    
	// ����C�ࡢB�ࡢA�������
	for (i = 0; i < 3; i++)
	{
		// ����0xAA�ֽڣ�Ȼ���ȡ��ѹ������������
		if (data[0] != 0xAA)
		{
			return FALSE; // �������0xAA�����ݸ�ʽ����
		}

		// ��ȡ���������ʡ���ѹ
		dpval = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];	 // ��ѹ
		dival = (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8];	 // ����
		dvval = (data[9] << 24) | (data[10] << 16) | (data[11] << 8) | data[12]; // ����

		// У׼�ж�������220V��ѹ��5A����������0.5
		if (((dvval / 1000) == 220) && ((dpval / 1000) == 550))
		{
			j = 3 - i; // C=1, B=2, A=3
			Result = Adjust_UIK((float)((dvval / 1000)), (float)((float)dival / 10000), i,chipNo);
			if (Result == FALSE)
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}

		// ������һ�������ݵ�λ�ã�ÿ����13�ֽڣ�
		data += 13;
	}
	return Result;
}
//-----------------------------------------------
//��������: ��ѹ������������ϵ������
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA HF���� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//����ֵ: 	��
//
//��ע:  		ͨ������ϵ��
//			Ku = ( ( Ui/1000 )*2^27 )/Un,
//			Un-���ѹ:220V��
//			Ui-��������ѹUnʱ�����뵽��ѹͨ��ADC������PGA��Ĳ���ֵ:�Ŵ���1*220/(200k*6+1k)*1k=183mv
//			Ki = ( ( Ii/1000 )*2^27 )/Ib,
//			Ib-�����:5A��
//			Ii-������ΪIbʱ�����뵽����ͨ��ADC������PGA��Ĳ���ֵ:�Ŵ���16*5*0.3=24mv
//			Kp = (2^31*32*EC*HFconst)/3.6*10^12*8.192
//-----------------------------------------------
static void UIP_K(BYTE *Buf, BYTE chipNo)
{
	TFourByteUnion td;	
	DWORD TempData;
	CLEAR_WATCH_DOG;
	UseSampleDefaultPara(eCORR, chipNo);
	CheckSampleChipReg(chipNo);
	
	td.d = wCaliVoltageConst * dwMeterBasicCurrentConst / 10;		//����ʵ�ʹ��� ����6λС��
	td.d = (DWORD)((double)td.d * (SampleCorr.PGain[0].d/100) / 1000.0); //����ʵ�ʹ���ֵ��Ӧ�ļĴ���ֵ	
	//Start_PS=Preg*(0.5~0.8)*K/256
	td.d = td.d * 1 * 6 / 10000 / 256; 	
	if( MeterTypesConst == METER_ZT )
	{
		//����100A��0.2%*0.6��������
		td.d = td.d*2;
	}
	SampleCtrlReg.PSTART.d = td.d;
	WriteSampleReg( PStart_EMU, td.b ,2, chipNo);

	//��λ�ֶ�У׼�ͻ�����( 1: 67-73%  2:134-146% )
	TempData = (WORD)(((float)dwMeterBasicCurrentConst / 1000.0)*(float)(SampleCorr.IGain[0].d)*IRegionUPConst/409600.0);
	SampleCtrlReg.PRTH1H.d = TempData;
	SampleCtrlReg.PRTH2H.d = (WORD)(TempData*2);//2H,2L���ܹ��󣬷���1H,1Lд��ʧ��
	TempData = (WORD)(((float)dwMeterBasicCurrentConst / 1000.0)*(float)(SampleCorr.IGain[0].d)*IRegionDownConst/409600.0);
	SampleCtrlReg.PRTH1L.d = TempData;
	SampleCtrlReg.PRTH2L.d = (WORD)(TempData*2);
	WriteSampleReg( PRTH1H_EMU, SampleCtrlReg.PRTH1H.b ,2, chipNo);
	WriteSampleReg( PRTH2H_EMU, SampleCtrlReg.PRTH2H.b ,2, chipNo);
	WriteSampleReg( PRTH1L_EMU, SampleCtrlReg.PRTH1L.b ,2, chipNo);
	WriteSampleReg( PRTH2L_EMU, SampleCtrlReg.PRTH2L.b ,2, chipNo);

	WriteEEpromSamplePara(eCORR, chipNo);
	WriteEEpromSamplePara(eCRTL, chipNo);
}

//-----------------------------------------------
//��������: ��Чֵ����У��(100%Ib,1.0)
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void Gain_Adjust(BYTE *Buf,BYTE chipNo)
{
	BYTE i,j;
	TFourByteUnion td;
	float Err_Data,Err_DataU,Err_DataI;
	DWORD RatedP[3],RatedU[3],RatedI[3];
	TFourByteUnion RegisterU[3],RegisterI[3];
	QWORD Sum[2][3];

	UIP_K(Buf, chipNo);//��ѹ����ϵ��У��
    SampleSpecCmdOp(0x100,CmdWriteOpen,chipNo);//UIP_K���������д�д�����Ĵ��룬�����������¹ر�д����
	//У��ǰ�ȰѼĴ�����������
	for(i=0; i<MAX_PHASE; i++)
	{
		if( (MeterTypesConst==METER_3P3W) && (i==1) )
		{
			continue;
		}
		ResetSampleReg( GPA_EMU+i ,2, chipNo);
		ResetSampleReg( GQA_EMU+i ,2, chipNo);
		ResetSampleReg( GSA_EMU+i ,2, chipNo);
		ResetSampleReg( GFPA_EMU+i ,2, chipNo);
		ResetSampleReg( GFQA_EMU+i ,2, chipNo);
		ResetSampleReg( GFSA_EMU+i ,2, chipNo);
		ResetSampleReg( GSUA_EMU+i ,2, chipNo);
		ResetSampleReg( GSIA_EMU+i ,2, chipNo);
	}

	CLEAR_WATCH_DOG;
	api_Delayms( 800 );	
	CLEAR_WATCH_DOG;
	api_Delayms( 800 );	
	CLEAR_WATCH_DOG;

	//��������ı�׼��ѹ������������
	for(i=0;i<MAX_PHASE;i++)	
	{
		//������λ���������Ĺ���ֵ
		if( (Buf[1]==0xff) && (Buf[2]==0xff) && (Buf[42]==0xaa) )
		{
			memcpy( (BYTE*)&RatedU[i], Buf+4+i*13, 4 );
			memcpy( (BYTE*)&RatedI[i], Buf+8+i*13, 4 );
		}
		else
		{
			RatedU[i] = wCaliVoltageConst*100;
			RatedI[i] = dwBigStandardCurrent*10;
		}
	}
	memset(Sum,0x00,sizeof(Sum));
	for ( i = 0; i < 12; i++)
	{
		for ( j = 0; j < MAX_PHASE; j++)
		{
			Link_ReadSampleReg(UA_EMU+j,RegisterU[j].b ,4, chipNo);
			
			Link_ReadSampleReg(IA_EMU+j,RegisterI[j].b ,4, chipNo);
			
			Sum[0][j] += ((QWORD)RegisterU[j].d);
				
			Sum[1][j] += ((QWORD)RegisterI[j].d);
		}
		
		CLEAR_WATCH_DOG;

		api_Delayms(260);
	}
	for ( j = 0; j < MAX_PHASE; j++)
	{
		RegisterU[j].d = (DWORD)(Sum[0][j]/12);

		RegisterI[j].d = (DWORD)(Sum[1][j]/12);
	}
	
	
	for(i=0;i<MAX_PHASE;i++)
	{
		if( (MeterTypesConst==METER_3P3W) && (i==1) )
		{
			continue;
		}	
		//ͨ������
		//�����ѹ���
		Err_DataU = ((double)(RegisterU[i].d)*1000/(double)SampleCorr.UGain[i].d-RatedU[i])/((double)RatedU[i]);
		//����������
		Err_DataI = ((double)(RegisterI[i].d)*10000.0/(double)SampleCorr.IGain[i].d-RatedI[i])/((double)RatedI[i]);
		
		//ȡ����ֵ ��������30% ������У�� ��ֹ���ť �Ų�Ӳ������
		if((fabs(Err_DataU) > ((float)Def_MaxError/100))
			||(fabs(Err_DataI) > ((float)Def_MaxError/100))) 
		{
			return;
		}
		Err_DataU = -Err_DataU/(1+Err_DataU);
		Err_DataI = -Err_DataI/(1+Err_DataI);
		
		if(Err_DataU >= 0) 
		{
			td.d = (DWORD)(32768*Err_DataU);
		}
		else 
		{
			td.d = (DWORD)(65536 + 32768*Err_DataU);
		}
		
		SampleAdjustReg[chipNo].GSU[i].d = td.w[0];
		
		WriteSampleReg(GSUA_EMU+i,td.b,2, chipNo);
		if(Err_DataI >= 0) 
		{
			td.d = (DWORD)(32768*Err_DataI);
		}
		else 
		{
			td.d = (DWORD)(65536 + 32768*Err_DataI);
		}
		
		SampleAdjustReg[chipNo].GSI[i].d = td.w[0];
		WriteSampleReg(GSIA_EMU+i,td.b,2, chipNo);
	}
	CLEAR_WATCH_DOG;
}

//-----------------------------------------------
//��������: ��λУ��(100%Ib,0.5L)
//
//����:		Type[in] 0--����� 1--С���
//			Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��   
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------

static void PH_Adjust(BYTE Type, BYTE *Buf, BYTE chipNo)
{
	BYTE i,j;	
	TFourByteUnion td,Originaltd[3];
	float Err_DataP;
	DWORD RatedP[3];
	TFourByteUnion RegisterP[3];
	QWORD Sum[3];
	// if (Type==0)
	// {
	// 	Gain_Adjust(Buf);//����У׼
	// }

	//��ֹ����� У�����ʱ������
//	if( (labs(RemoteValue.Cos[0].l) > 6500)||(labs(RemoteValue.Cos[0].l) < 3500) )
//	{
//		return;
//	}

	for(i=0; i<MAX_PHASE; i++)
	{
		if((MeterTypesConst==METER_3P3W)&&( i == 1 ))
		{
			continue;
		}
		
		//��ȡͨ����λԭʼ����
		Originaltd[i].d = 0x00808080;
		Link_ReadSampleReg(PHSIA_EMU+i,Originaltd[i].b,4, chipNo);
		ResetSampleReg( PHSIA_EMU+i ,4, chipNo);
		ResetSampleReg( PHSUA_EMU+i ,4, chipNo);
		
		//������λ���������Ĺ���ֵ
		if( (Buf[1]==0xff) && (Buf[2]==0xff) && (Buf[42]==0xaa) )
		{
			memcpy( (BYTE*)&RatedP[i], Buf+12+i*13, 4 );
		}
		else
		{
		  	if( Type == 0 )//�����
			{
				if( labs(RemoteValueArr[chipNo].I[i].l/10) > (dwBigStandardCurrent*IRegionUPConst*2/100))
				{
					RatedP[i] = (DWORD)(wCaliVoltageConst / 10) * dwMaxCurrent/2;
				}
				else
				{
					RatedP[i] = (DWORD)(wCaliVoltageConst / 10) * dwBigStandardCurrent/2;
				}
			}
			else//С����
			{
				RatedP[i] = (DWORD)(wCaliVoltageConst / 10) * dwSmallStandardCurrent/2;
			}
		}
	}

	CLEAR_WATCH_DOG;	
	api_Delayms( 800 );
	CLEAR_WATCH_DOG;
	api_Delayms( 800 );
	CLEAR_WATCH_DOG;

	memset(Sum,0x00,sizeof(Sum));
	for ( i = 0; i < 12; i++)
	{
		for ( j = 0; j < MAX_PHASE; j++)
		{
			Link_ReadSampleReg(PA_EMU+j,RegisterP[j].b,4, chipNo);
			
			Sum[j] += ((QWORD)RegisterP[j].d);
		}
		CLEAR_WATCH_DOG;

		api_Delayms(300);
		
	}
	
	for(i=0; i<MAX_PHASE; i++)
	{
		RegisterP[i].d = (DWORD)(Sum[i]/12);
	}	

	for(i=0;i<MAX_PHASE;i++)
	{
		if( (MeterTypesConst==METER_3P3W) && (i==1) )
		{
			continue;
		}	

		//���㹦�����
		Err_DataP = ((double)(RegisterP[i].d)*1000.0/(double)SampleCorr.PGain[i].d*100);
		Err_DataP = ((double)(Err_DataP - RatedP[i]))/((double)RatedP[i]);
		
		//ȡ����ֵ ��������30% ������У�� ��ֹ���ť �Ų�Ӳ������
		if(fabs(Err_DataP) > ((float)Def_MaxError/100))
		{
			return;
		}
		
		Err_DataP = asin(-Err_DataP/1.732)*180.0/3.1415926;
		//��λУ����Χ�� 50HZ�£���2.25�Ȼ� 4.5��
		if( Err_DataP>2.2499)
		{
		  	Err_DataP = 2.2499;
		}
		else if( Err_DataP<-2.2499)
		{
		  	Err_DataP = -2.2499;
		}

		if( Type == 0 )//�����
		{
			if( labs(RemoteValueArr[chipNo].I[i].l/10) > (dwBigStandardCurrent*IRegionUPConst*2/100)) //���������
			{
				td.d = (Originaltd[i].d&0x000FFFFF)+((DWORD)(0x202-(Err_DataP/0.008789)*1024*1024));
				SampleAdjustReg[chipNo].PHSI[i].d = td.d;
				WriteSampleReg(PHSIA_EMU+i,td.b,4, chipNo);
			}
			else
			{
				td.d = (DWORD)(0x202 - (Err_DataP / 0.008789));   // δ�ҵ���ǲ�2-3�̶�ԭ���վ���ֵ��0x200�ϣ�2��������Ʒ����ȦЧ����
				td.d = (((DWORD)(0x201 - (Err_DataP / 0.008789))) + (td.d * 1024) + (td.d * 1024 * 1024));   // �ڴ˴�С������1�̶�Ч����
				SampleAdjustReg[chipNo].PHSI[i].d = td.d;
				WriteSampleReg(PHSIA_EMU+i,td.b,4, chipNo);
			}
			
		}
		else//С����
		{
			td.d = (Originaltd[i].d&0x3FFFFC00)+(DWORD)(0x202-(Err_DataP/0.008789));
			SampleAdjustReg[chipNo].PHSI[i].d = td.d;
			WriteSampleReg(PHSIA_EMU+i,td.b,4, chipNo);
		}
	}
	lib_ExchangeData(Buf, Buf, 45); // ��������
	api_UI_Kfor8306(Buf, chipNo); // ��ѹ����ϵ������
    lib_ExchangeData(Buf, Buf, 45); // ���������µ���������¸�����оƬʹ�ã�������λ���õ�������ֻ����api_UI_Kfor8306����ָ�˳��
	CLEAR_WATCH_DOG;
}
//@@@@���ԣ�֮�������λ�������޸�
static void PH_Adjust2(BYTE *Buf)
{
//	TFourByteUnion k1,k2;
//	
//	k1.d=0;
//	k2.d=0;
//	Link_ReadSampleReg(PAPHSL_EMU,k1.b);
//	memcpy(k2.b,Buf+1,2);
//	k1.w[0] +=  k2.w[0];
//	k1.w[1] = 0;
//	WriteSampleReg(PAPHSL_EMU,k1.b);
}
//-----------------------------------------------
//��������: �й�����ƫ������(5%Ib,1.0)
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA HF���� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void Offset_Adjust(BYTE *Buf, BYTE chipNo)
{
	BYTE i, j;

	TFourByteUnion RegisterI[3];
	QWORD Sum[MAX_PHASE];
	for (i = 0; i < MAX_PHASE; i++)
	{
		ResetSampleReg(PAOS_EMU + i, 2, chipNo);
	}

	CLEAR_WATCH_DOG;
	api_Delayms(400);
	CLEAR_WATCH_DOG;

	memset(Sum,0x00,sizeof(Sum));

	for (i = 0; i < 12; i++)
	{
		for (j = 0; j < MAX_PHASE; j++)
		{
			Link_ReadSampleReg(IA_EMU + j, RegisterI[j].b, 4, chipNo);
			Sum[j] += ((QWORD)RegisterI[j].d);
		}

		CLEAR_WATCH_DOG;

		api_Delayms(260);
	}
	for (j = 0; j < MAX_PHASE; j++)
	{
		RegisterI[j].w[0] = (WORD)(-1 * (DWORD)(Sum[j] / 12) * (Sum[j] / 12)) / 16384;
		SampleAdjustReg[chipNo].IOS[j].w[0]=RegisterI[j].w[0];
		WriteSampleReg(IAOS_EMU + i, RegisterI[j].b, 2, chipNo);
	}
}

//-----------------------------------------------
//��������: ����A/B��Чֵƫ������(220V,I=0,1.0)
//
//����:		Ichannel[in] 	1:Aͨ��	����ֵ��Bͨ��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void IxRMSOS_Adjust(BYTE Ichannel)
{
//	TFourByteUnion Temp_B32;
//	TFourByteUnion td;
//	WORD i;
//	DWORD Addr;
//	SWORD SwTemp;
//	
//	if(Ichannel == 1) 
//	{
//		Addr = IARMS_EMU;
//		ResetSampleReg( IAOS_EMU );
//	}
//	else 
//	{
//		Addr = IBRMS_EMU;
//		ResetSampleReg( IBOS_EMU );
//	}
//	
//	CLEAR_WATCH_DOG;
//	api_Delayms( 800 );
//	CLEAR_WATCH_DOG;
//	
//	Temp_B32.d = 0;
//	td.d = 0;
//	for(i=0; i<6; i++)
//	{		
//		Link_ReadSampleReg(Addr,td.b);
//		
//		if( td.l < 0 )
//		{
//		  	td.l *= -1;
//		}
//	
//		Temp_B32.d += td.d;
//		
//		CLEAR_WATCH_DOG;
//		
//		api_Delayms(400);	//��ʱ400ms
//	}
//	
//	CLEAR_WATCH_DOG;
//	
//	Temp_B32.d = Temp_B32.d/6;

//	td.l = (long)(-((float)Temp_B32.d*Temp_B32.d)/16384);
//	
//	if(Ichannel == 1) 
//	{
//		SampleAdjustReg.I1OS.d = td.l;
//		WriteSampleReg(IAOS_EMU,td.b);
//	}	
//	else 
//	{
//    	SampleAdjustReg.I2OS.d = td.l;
//    	WriteSampleReg(IBOS_EMU,td.b);
//	}
}

//-----------------------------------------------
//��������: EDTУ׼ //@liang
//
//����:   ��
//
//����ֵ:  ��
//
//��ע:   ��
//-----------------------------------------------
#define EDT_CALI_RETRY_MAX          (5)     //EDTУ׼�����ط�����
#define EDT_PARAM_READ_RETRY_MAX    (5)     //EDT�����Ĵ�����ȡ���Դ���
#define EDT_CALI_WAIT_TIME          (8*60) //EDTУ׼�ȴ�ʱ�䣬��λ��
static void EDT_Adjust( BYTE chipNo )//!!!!!!
{
    TFourByteUnion DumpData;
    DWORD Retry;
    WORD i,j;
    BOOL FinishFlag;
    WORD wAddr;
     
    CLEAR_WATCH_DOG;
    
    Retry = 0;
    for( i=0; i<EDT_CALI_RETRY_MAX; i++ )
    {
        DumpData.d = 1;
        if( WriteSampleReg(EDT_CALI, DumpData.b,4, chipNo) == TRUE )///����δ�޸�!!!!!!
        {
            break;
        }
        api_Delayms(50);	
    }
    if( i >=  EDT_CALI_RETRY_MAX )
    {
        return;
    }
    
    Retry = (DWORD)((QWORD)EDT_CALI_WAIT_TIME) * 1000 / 100;
    FinishFlag = FALSE;
    while( Retry-- )
    {
        DumpData.d = 0;
        Link_ReadSampleReg( EDT_STATUS, DumpData.b ,4, chipNo);///����δ�޸�!!!!!!
        if( DumpData.d != 0 )
        {
            FinishFlag = TRUE;
            break;
        }
        
        api_Delayms(100);	
        CLEAR_WATCH_DOG;
    }
    
    CLEAR_WATCH_DOG;
    
    if( FinishFlag == TRUE )
    {
        for( i=0; i<EDT_PARAM_READ_RETRY_MAX; i++ )
        {
            for( j=0; j<(8*MAX_PHASE); j++ )
            {
                DumpData.d = 0;
                wAddr = EDT_PARAM + j;
                if( Link_ReadSampleReg(wAddr, DumpData.b,4, chipNo) == FALSE )///����δ�޸�!!!!!!
                {
                    break;
                }
                
                *((DWORD *)&EDTDataPara + j) = DumpData.d;
            }    
            
            if( i >= EDT_PARAM_READ_RETRY_MAX )
            {
                return;
            }
        }
            
        //���浽EEP
        WriteEEpromEDTDataPara();
        CLEAR_WATCH_DOG;
    }
}

//---------------------------------------------------------------
//��������: ���²��������� �������õ���ѹ���ʱ����
//
//����: 	��
//                   
//����ֵ:  	��
//
//��ע:   
//---------------------------------------------------------------
void api_InitSamplePara( void )
{	
	InitSampleCaliUI();
}

//-----------------------------------------------
//��������: У�����
//
//����:		AdjType[in] 	0xFF��	У���ʼ��
//							0x00��	��Ư
//							0x01:	��������� 1.0(L)
//							0x02:	С�������� 1.0(L)
//							0x51:	�������� 0.5L(L)
//							0x52:	С������� 0.5L(L)
//							0x81:	��������� N�� 1.0(N)
//							0xD1:	�������� N�� 0.5L(N)
//			Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA HF���� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA 	
//          
//����ֵ: 	TRUE/FALSE
//		   
//��ע:У���Լ����     
//-----------------------------------------------
WORD api_ProcSampleAdjust(BYTE AdjType, BYTE *Buf, BYTE chipNo)
{	
	DWORD dwSum;
	BYTE lock[2]={0xE5,0xDC};
	if( (Buf[0]!=0xaa) || (Buf[3]!=0xaa) )
	{
		return FALSE;
	}

	//�ر�EDT
	//PowerDownEDT();
	
	//��ʼ����׼Դ����ֵ
	InitSampleCaliUI();
	//�ر�У��Ĵ���д����
	SampleSpecCmdOp(0x100,CmdWriteOpen, chipNo);
	//WriteSampleReg(0x0180,lock,1);
	//Ŀǰ��г����EDT����Ӱ�죬У��ʽ����ͨ������+ͨ����λ��֮ǰ�ǹ�������+������λ��
	switch( AdjType )
	{
		case 0xff://��ʼ��
			InitSamplePara(chipNo);
			break;
		
		case 0x00://��������У׼

			//�����������ƯдĬ��ֵ
			break;
			
		case 0x01://100%Ib��1.0(L)
			Gain_Adjust(Buf,chipNo); //��Чֵ����У��
			break;
			
		case 0x51://�������0.5L(L)
			PH_Adjust(0,Buf,chipNo); //ͨ����λУ��
			break;
			
		case 0x52://С������0.5L(L)
			PH_Adjust(1,Buf,chipNo); //������λУ��
			break;
			
		case 0x02://5%Ib��1.0(L)
			Offset_Adjust(Buf,chipNo); //�й�����ƫ������
			break;

        case 0x08://100%Ib��1.0(EDT)
            EDT_Adjust(chipNo); //EDT��׼ֵ
            break;    
            
		default: 
			break;
	}	

	api_WriteFreeEvent(EVENT_ADJUST_METER, (WORD)AdjType);

	if( AdjType != 0xff )//�ڼ�����ʼ�����Ѿ�д��EEPROM �����ظ�
	{
		WriteEEpromSamplePara(eADJUST, chipNo);//У��ϵ�����ݴ���
	}
	CheckSampleChipReg(chipNo);//��ֹд�����оƬʧ��Ӱ����һ��У��
	GetSampleChipCheckSum(chipNo);//��У���
	//�ر�У��Ĵ���д����
	SampleSpecCmdOp(0x100,CmdWriteClose, chipNo);
	return TRUE;
	
}

//-----------------------------------------------
//��������: ͨѶ��һ���Ĵ���
//
//����:		DI0[in/out]	
//          	in:Buf[0]�Ĵ�����ַ
//				out:Buf[0]�Ĵ�����ַ,���淵�ض�Ӧ�Ĵ�������������
//����ֵ: 	TRUE/FALSE
//		   
//��ע:   
//-----------------------------------------------
BOOL ReadSampleReg(BYTE *InBuf,BYTE *OutBuf)//δʹ�õ�
{
	// BYTE result, i;
	// BYTE Buf[4];
	// DWORD Addr;
	
	// Addr = *((DWORD *)InBuf);
	
	// result = Link_ReadSampleReg(InBuf[0],Buf,4);
	// if(result == FALSE) 
	// {
	//    return FALSE;
	// }
	// //������ǰ��λ����С��ģʽ��Ϊ��
	// Buf[3] = InBuf[0];
	// lib_InverseData( Buf, 4 );
	// memcpy(OutBuf, Buf, 4);

	// return TRUE;
	return FALSE;
}

//-----------------------------------------------
//��������: ͨѶдһ���Ĵ���
//
//����:		WriteEepromFlag 0x00:ֻдRAM�ͼ���оƬ����дEEPROM 0xFF:�����ط���д
//			Addr[in]�Ĵ�����ַ
//			Buf[in]	Ҫд������� ���ֽڣ����ں󣬵���ǰ
//����ֵ: 	TRUE/FALSE
//		   
//��ע:   
//-----------------------------------------------
BOOL SetSampleReg(BYTE WriteEepromFlag,DWORD Addr,BYTE *Buf)
{
	return TRUE;
}
//---------------------------------------------------------------
//��������: ͨѶд��ѹӰ����ϵ��
//
//����: 	Addr[in]�Ĵ�����ַ
//		    Buf[in]	Ҫд������� ���ֽڣ����ں󣬵���ǰ
//                   
//����ֵ:  
//
//��ע:   
//---------------------------------------------------------------
BOOL SetSampleVolCorrectCorr( WORD Addr, BYTE *Buf )
{
	return TRUE;
}
//---------------------------------------------------------------
//��������: ͨѶ����ѹӰ����ϵ��
//
//����: 		Addr[in]�Ĵ�����ַ
//				Buf[in]	Ҫд������� 2�ֽ�
//����ֵ: 		TRUE/FALSE 
//
//��ע:   
//---------------------------------------------------------------
BOOL ReadSampleVolCorrectCorr( WORD Addr, BYTE *Buf )
{
	return TRUE ;
}

//-----------------------------------------------
//��������: ͨѶд˲ʱ��ϵ��
//
//����:		Addr[in]�Ĵ�����ַ
//			Buf[in]	Ҫд������� ���ֽڣ����ں󣬵���ǰ
//����ֵ: 	TRUE/FALSE
//
//��ע:
//-----------------------------------------------
BOOL SetSampleCorr( WORD Addr, BYTE *Buf )
{
	return TRUE;
}

//-----------------------------------------------
//��������: ͨѶ��˲ʱ��ϵ��
//
//����:		Addr[in]    �Ĵ�����ַ
//			Buf[in]	    Ҫ���������ݣ����ֽڣ����ں󣬵���ǰ����3��
//
//����ֵ: 	TRUE/FALSE
//		   
//��ע:     ��
//-----------------------------------------------
BOOL ReadSampleCorr( WORD Addr, BYTE *Buf, BYTE chipNo)
{
    BYTE Result;
	TFourByteUnion td;
	float Gain,Corr;
    BYTE i;
    
	switch( Addr )
	{
		case 0x9301://��ѹ�Ĵ���ϵ��ת��ֵ-����ʶ��ģ��ʹ��
			//��ʽ�Ƶ�������΢�ṩ
            for( i=0; i<3; i++ )
            {
                td.d = SampleAdjustReg[chipNo].GSU[i].d;
                Corr = (float)SampleCorr.UGain[i].d;
                Corr = 1000000.0/Corr*1000000.0;//8956982
                if(td.w[0]&0x8000) 
                {
                    Gain = (float)(td.w[0]-65536)/32768.0;
                }
                else 
                {
                    Gain = ((float)td.w[0])/32768.0;
                }
                td.d = (DWORD)(Corr*(1+Gain)*64*0.512);
                memcpy( Buf+4*i, td.b, 4 );
            }
			Result = TRUE;
			break;
		case 0x9302://�����Ĵ���ϵ��ת��ֵ-����ʶ��ģ��ʹ��
			//��ʽ�Ƶ�������΢�ṩ
            for( i=0; i<3; i++ )
            {
                td.d = SampleAdjustReg[chipNo].GSI[i].d;
                Corr = (float)SampleCorr.IGain[i].d;
                Corr = 1000000.0/Corr*1000000.0;//1862897
                if(td.w[0]&0x8000) 
                {
                    Gain = (float)(td.w[0]-65536)/32768.0;
                }
                else 
                {
                    Gain = ((float)td.w[0])/32768.0;
                }
                td.d = (DWORD)(Corr*(1+Gain)*64*0.512);
                memcpy( Buf+4*i, td.b, 4 );
            }
			Result = TRUE;
			break;
		default:
			Result = FALSE;
	}
	
	return Result;
}

//-----------------------------------------------
//��������: ͨѶ����У��ϵ��
//
//����:		Operation[in]	READ/WRITE
//          Addr[in]		0xFFFF			У���ʼ��
//							0x0000~0x8FFF	����оƬ�Ĵ���
//							0x9000~0x9FFF	˲ʱ��ϵ��
//							0xA000~0xAFFF	��ѹӰ����ϵ��
//							0xB000~0xBFFF	���Ȳ���ϵ��
//			Len[in]			�������ݳ���
//			Buf[in/out]		�����򣬸��ں󣬵���ǰ
//			chipNo[in]		оƬ���
//����ֵ: 	TRUE/FALSE
//
//��ע:
//-----------------------------------------------
BOOL api_ProcSampleCorr(BYTE Operation, WORD Addr, BYTE Len, BYTE *Buf, BYTE chipNo)
{
	BYTE i;
	DWORD tdw,SampleAddr;
	BOOL Result;

	if(Len <= sizeof(tdw))
	{
		tdw = 0;
		memcpy((BYTE *)&tdw, Buf, Len);
		SampleAddr = (Addr & 0x0fff);
	}

	if( Operation == WRITE )
	{
		SampleSpecCmdOp(SampleAddr,CmdWriteOpen, chipNo);
		Result = WriteSampleReg((WORD)SampleAddr,(BYTE*)&tdw,Len, chipNo);
		SampleSpecCmdOp(SampleAddr,CmdWriteClose, chipNo);
		if( Result == FALSE )
		{
			return FALSE;
		}
		//��ϵ�����浽ee
		if( Addr < 0x2000 )
		{
			//ֻ���浽ram������оƬ�������浽eeprom�������ϵ�ʱ�ָ�ԭֵ
			WriteSampleParaToEE(0x00, SampleAddr, (BYTE*)&tdw, chipNo);	
		}
		else
		{
			//���浽ram������оƬ��eeprom
			WriteSampleParaToEE(0xff, SampleAddr, (BYTE*)&tdw, chipNo);	
			
			api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+SampleAddr);
		}
		GetSampleChipCheckSum(chipNo);//��У���
	}	
	else
	{	
		//���ݿ鷽ʽ����
		if(Addr == 0x3fff)
		{
			for(i=0;i<((sizeof(TSampleCtrlReg)-sizeof(DWORD))/sizeof(DWORD));i++)
			{
				*Buf = SampleCtrlAddr[i][0]%0x100;
				Buf += 1;
				memcpy(Buf,(BYTE*)&SampleCtrlReg+i*sizeof(DWORD),sizeof(DWORD));
				Buf += 4;
			}
		}
		else if(Addr == 0x4fff)
		{		
			for(i=0;i<((sizeof(TSampleAdjustReg)-sizeof(DWORD))/sizeof(DWORD));i++)
			{
				*Buf = SampleAdjustAddr[i][0]%0x100;
				Buf += 1;
				memcpy(Buf,(BYTE*)&SampleAdjustReg[chipNo]+i*sizeof(DWORD),sizeof(DWORD));
				Buf += 4;
			}
		}
		else
		{
			Link_ReadSampleReg( Addr,Buf,Len, chipNo );
		}
		
	}

	return TRUE;
}

#if( SEL_NILM_MODULE == YES )
//-----------------------------------------------
//��������: ��ȡEDT��BCD��ʽ��XXX.XXXX
//
//����:     Type[in]        EDT���ͣ�0xX0Ϊ������0xX1Ϊ��λ��0xFFΪ������
//	        Len[in]			���ݳ���
//          Buf[out] 		�������
//
//����ֵ: 	TRUE:��ȷ����ֵ		FALSE���쳣
//
//��ע:     �ⲿ����
//          0xX0ΪIͨ��
//          0xX1ΪU1ͨ��
//          0xX2ΪU2ͨ��
//          0xX3ΪUnͨ��
//-----------------------------------------------
BOOL api_GetEDTResulteData( BYTE Type, BYTE Len, BYTE *Buf )
{
    TFourByteUnion td;
    WORD EDTReadType;
    BYTE Flag; 
    BYTE out_idx;
    
    if( Type == 0xFF )
    {
        return FALSE;
    }
    else if( ((Type&0x0F) == 0x00) 
          || ((Type&0x0F) == 0x01) )
    {
        if( (Type&0xF0) > 0x30 )
        {
            return FALSE;
        }
        
        if(Len < (4*MAX_PHASE))
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    
    out_idx = 0;

    EDTReadType  = ((Type&0x0F) == 0x00) ? EDT_ERR_AMP : EDT_ERR_PHA;
    EDTReadType += (Type&0xF0);
    for( BYTE i=0; i<MAX_PHASE; i++ )
    {
        if( Link_ReadSampleReg(EDTReadType+i, td.b ,4) == FALSE )
        {
            td.l = 99999999;//return FALSE;
        }
                  
        Flag = 0;
        if( td.l < 0 )
        {
            Flag = 1;
            td.l *= -1;
        }

        td.d = lib_DWBinToBCD(td.d);
        if( Flag == 1 )
        {
            td.b[3] |= 0x10;
        }

        memcpy( (Buf + out_idx), td.b, 4 );
        out_idx += 4;
    }

    return TRUE;
}
#endif //#if( SEL_NILM_MODULE == YES )
#endif //#if( SAMPLE_CHIP == CHIP_RN8302B )
