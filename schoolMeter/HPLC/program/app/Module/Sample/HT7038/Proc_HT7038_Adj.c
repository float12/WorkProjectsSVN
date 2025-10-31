//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.6
//����		����оƬHT7038У������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7038.h"

#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )

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
WORD	wSmallStandardCurrent;		//С���У����Ӧ����
DWORD	AdjustDivCurrent[10];		//�ֶ�У�������׼ֵ

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
		wSmallStandardCurrent = 200;
		
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
			AdjustDivCurrent[9] = 1;         //9         0.001A        
		
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
		wSmallStandardCurrent = 300;
		
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
			AdjustDivCurrent[9] = 2;            //9         0.0015A      0.1%Ib     
			
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
	//������� 5(60) ���� ������� 10(100)
	else if( (MeterCurrentTypesConst == CURR_60A) || (MeterCurrentTypesConst == CURR_100A) )
	{
		dwBigStandardCurrent = 60000;
		wSmallStandardCurrent = 6000;
		
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
			AdjustDivCurrent[9] = 5;           //9         0.005A       0.1%Ib      
			
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
										   
			CurrentValue[6].Upper = 150;  
			CurrentValue[6].Lower = 120;  
										   
			CurrentValue[7].Upper = 80;   
			CurrentValue[7].Lower = 60;   
										   
			CurrentValue[8].Upper = 30;   
			CurrentValue[8].Lower = 10;   
		}
	}
}


//-----------------------------------------------
//��������: У���ʼ��
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע: 
//-----------------------------------------------
static void AdjustInit(void)
{
	//��дeeУ��ϵ������
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	
	//����оƬƫ����Ĵ�������
	UseSampleDefaultPara(0);
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleOffsetPara), sizeof(TSampleOffsetPara), (BYTE*)&SampleOffsetPara);
	WriteSampleChipOffsetPara();
	
	//����оƬ������Ĵ�������
	UseSampleDefaultPara(1);
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlPara), sizeof(TSampleCtrlPara), (BYTE*)&SampleCtrlPara);
	WriteSampleChipCtrlPara();
	
	//����оƬ������Ĵ�������
	UseSampleDefaultPara(2);
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustPara), sizeof(TSampleAdjustPara), (BYTE*)&SampleAdjustPara);
	WriteSampleChipAdjustPara();
	
	//�ֶξ�����������
	memset( (BYTE *)&SampleAdjustParaDiv, 0x00, sizeof(TSampleAdjustParaDiv) );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleAdjustParaDiv ), sizeof(TSampleAdjustParaDiv), (BYTE *)&SampleAdjustParaDiv );

	//���Ȳ��� (�¶Ȳ���)��������
	memset( (BYTE *)&SampleManualModifyPara, 0x00, sizeof(TSampleManualModifyPara) );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleManualModifyPara ), sizeof(TSampleManualModifyPara), (BYTE *)&SampleManualModifyPara );
	
	//ֱ��ż��г��ϵ��
	if( lib_CheckSafeMemVerify( (BYTE *)&SampleHalfWavePara, sizeof(TSampleHalfWavePara), UN_REPAIR_CRC ) == FALSE )
	{
		memcpy( (BYTE *)&SampleHalfWavePara, (BYTE *)SampleHalfWaveDefData, (sizeof(TSampleHalfWavePara) - sizeof(DWORD)) );
	}
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWavePara ), sizeof(TSampleHalfWavePara), (BYTE *)&SampleHalfWavePara );
	
	//����Ĭ��ϵ������
	memset( (BYTE *)&OriginalPara, 0x00, sizeof(TOriginalPara) );
		
	
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	
}

//-----------------------------------------------
//��������: ��λ��Ӧ�����Ĵ���
//
//����:		Reg_Addr[in]	�Ĵ�����ַ
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void ResetSampleReg(BYTE Reg_Addr)
{
    DWORD tdw;

	tdw = 0;
	WriteSampleReg( Reg_Addr, tdw );
}


//-----------------------------------------------
//��������: У����ѹ����������
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void UIP_K(BYTE *Buf)
{
	WORD i,j;
	DWORD dwData,dwAdjustU[3],dwAdjustI[3];
	DWORD CaliStandardU[3],CaliStandardI[3];
	float tf;
	
	//��������ı�׼��ѹ����
	for(i=0;i<3;i++)	
	{
		//������λ���������ĵ�ѹ������ֵ
		if( (Buf[1]==0xff) && (Buf[2]==0xff) && (Buf[42]==0xaa) )
		{
			memcpy( (BYTE*)&CaliStandardU[i], Buf+4+i*13, 4 );//3λС��
			memcpy( (BYTE*)&CaliStandardI[i], Buf+8+i*13, 4 );//4λС��
		}
		else
		{
			CaliStandardU[i] = (DWORD)wCaliVoltageConst * 100; //3λС��
			CaliStandardI[i] = dwBigStandardCurrent*10;		//4λС��
		}
	}	
	
	for(i=0; i<3; i++)
	{
		if((MeterTypesConst==METER_3P3W)&&( i == 1 ))
		{
			continue;
		}
		
		//��ʼ���Ĵ���	
		ResetSampleReg(w_UGainA+i);
		ResetSampleReg(w_IGainA+i);
		dwAdjustU[i] = 0;
		dwAdjustI[i] = 0;	
	}
	
	CLEAR_WATCH_DOG;
				
	//1.7hz��������������
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//������׼��ѹ������ε�ƽ��ֵ			
	for(j=0;j<6;j++)
	{
		for(i=0;i<3;i++)
		{
			if((MeterTypesConst==METER_3P3W)&&( i == 1 ))
			{
				continue;
			}	
			
			//��ѹ
			dwData = ReadMeasureData( r_UARMS+i );
			dwAdjustU[i] += dwData*1000/TWO_POW13;
			
			//����
			dwData = ReadMeasureData( r_IARMS+i );
			tf = (float)dwData*10000/TWO_POW13;
			tf /= ((float)(6 * CURRENT_VALUE) / 5);
			dwData = (DWORD)((tf * wMeterBasicCurrentConst) / 1000 + 0.5);
			dwAdjustI[i] += dwData;				
		}			
		
		CLEAR_WATCH_DOG;
		api_Delayms(600);
		CLEAR_WATCH_DOG;
	}
	
	for(i=0; i<3; i++)
	{
		if((MeterTypesConst==METER_3P3W)&&( i == 1 ))
		{
			continue;
		}							
		
		dwAdjustU[i] /= 6;
		dwAdjustI[i] /= 6;

		//�����ѹ���
		tf = (float)CaliStandardU[i]/dwAdjustU[i] - 1;
		if( tf < 0 )
		{
			dwData = TWO_POW16 - (DWORD)(fabs(tf)*(TWO_POW15));
		}
		else
		{
			dwData = (DWORD)(tf*(TWO_POW15));
		}
		//дee
		WriteSampleParaToEE(0xff, w_UGainA+i, (BYTE*)&dwData);
		//д����оƬ
		WriteSampleReg( w_UGainA+i, dwData );
		
		//����������
		tf = (float)CaliStandardI[i]/dwAdjustI[i] - 1;
		if( tf < 0 )
		{
			dwData = TWO_POW16 - (DWORD)(fabs(tf)*(TWO_POW15));
		}
		else
		{
			dwData = (DWORD)(tf*(TWO_POW15));
		}

		//дee
		WriteSampleParaToEE(0xff, w_IGainA+i, (BYTE*)&dwData);
		//д����оƬ
		WriteSampleReg( w_IGainA+i, dwData );
	}
	
	//ˢ�µ��� �ֶξ���У��UIPϵ����Ҫˢ��һ�µ��� У׼�����Ҫ��ȷ�ĵ������ж�����
	if(SelDivAdjust == YES)
	{
		CLEAR_WATCH_DOG;
		api_Delayms(600);
		CLEAR_WATCH_DOG;
		api_Delayms(600);
		CLEAR_WATCH_DOG;
		FreshCurrent();
	}
}

//-----------------------------------------------
//��������: У�����ߵ�������
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void ZeroCurrentGain_Adjust( BYTE *Buf )
{
	WORD i = 0;
	DWORD dwData = 0, dwAdjustI = 0;
	DWORD CaliStandardI = 0;
	float tf = 0;
	
	if( SelZeroCurrentConst == NO )
	{
		return;
	}
	
	if( (Buf[1] == 0xff) && (Buf[2] == 0xff) && (Buf[42] == 0xaa) )
	{
		memcpy( (BYTE *)&CaliStandardI, Buf + 8, 4 ); //4λС��
	}
	else
	{
		CaliStandardI = wSmallStandardCurrent * 10;//4λС��
	}
	
	//��ʼ���Ĵ���
	ResetSampleReg( w_GainADC7 );
	
	//1.7hz��������������
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//������׼���ߵ�����ε�ƽ��ֵ
	for( i = 0; i < 6; i++ )
	{
		//����
		dwData = ReadMeasureData( r_I0RMS );
		tf = (float)dwData * 10000 / TWO_POW13;
		tf /= ((float)(6 * ZERO_CURRENT_VALUE) / 5);
		dwData = (DWORD)((tf * wMeterBasicCurrentConst) / 1000 + 0.5);
		dwAdjustI += dwData;
		
		CLEAR_WATCH_DOG;
		api_Delayms(600);
		CLEAR_WATCH_DOG;
	}
	
	dwAdjustI /= 6;
	
	//����������
	tf = (float)CaliStandardI / dwAdjustI - 1;
	if( tf < 0 )
	{
		dwData = TWO_POW16 - (DWORD)(fabs(tf)*(TWO_POW15));
	}
	else
	{
		dwData = (DWORD)(tf*(TWO_POW15));
	}
	
	//дee
	WriteSampleParaToEE(0xff, w_GainADC7, (BYTE*)&dwData);
	//д����оƬ
	WriteSampleReg( w_GainADC7, dwData );
}

//-----------------------------------------------
//��������: ���ߵ���ƫ��У��
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void I0rmsOffset_Adjust( void )
{
	BYTE i = 0;
	DWORD Irms = 0, IrmsSum = 0, IRmsOffset = 0;
	
	if( SelZeroCurrentConst == NO )
	{
		return;
	}
	
	//���ߵ���ƫ��У��
	for( i = 0; i < 10; i++ )
	{
		Irms = ReadMeasureData( r_I0RMS );
		IrmsSum += Irms;
		CLEAR_WATCH_DOG;
		api_Delayms(600);
	}
	
	CLEAR_WATCH_DOG;
	
	Irms = IrmsSum / 10;
	
	IRmsOffset = Irms;
	
	//дee
	WriteSampleParaToEE( 0xFF, 0xFE, (BYTE *)&IRmsOffset );
}

//-----------------------------------------------
//��������: ���ߵ���ƫ��У��
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void IrmsOffset_Adjust( void )
{
	BYTE i = 0, j = 0;
	DWORD Irms = 0, IrmsSum = 0, IRmsOffset = 0;
	
	//��ʼ��ƫ��У���Ĵ���
	ResetSampleReg( w_ItRmsOffset );
	ResetSampleReg( w_IARMSOffset );
	ResetSampleReg( w_IBRMSOffset );
	ResetSampleReg( w_ICRMSOffset );
	//1.7hz��������������
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//����ʸ����ƫ��У��
	for( i = 0; i < 10; i++ )
	{
		Irms = ReadMeasureData( r_ITRMS );
		IrmsSum += Irms;
		CLEAR_WATCH_DOG;
		api_Delayms(600);
	}
	CLEAR_WATCH_DOG;
	Irms = IrmsSum / 10;
	IRmsOffset = (DWORD)((double)Irms * (double)Irms / (double)TWO_POW15 + 0.5);
	//дee
	WriteSampleParaToEE( 0xFF, w_ItRmsOffset, (BYTE *)&IRmsOffset );
	//д����оƬ
	WriteSampleReg( w_ItRmsOffset, IRmsOffset );

	//A��B��C�����ƫ��У��
	for( i = 0; i < MAX_PHASE; i++ )
	{
		IrmsSum = 0;
		//������Чֵƫ��У��
		for( j = 0; j < 10; j++ )
		{
			Irms = ReadMeasureData( r_IARMS + i );
			IrmsSum += Irms;
			CLEAR_WATCH_DOG;
			api_Delayms(600);
		}
		CLEAR_WATCH_DOG;
		Irms = IrmsSum / 10;
		IRmsOffset = (DWORD)((double)Irms * (double)Irms / (double)TWO_POW15 + 0.5);
		//дee
		WriteSampleParaToEE( 0xFF, w_IARMSOffset + i, (BYTE *)&IRmsOffset );
		//д����оƬ
		WriteSampleReg( w_IARMSOffset + i, IRmsOffset );
	}
}

//-----------------------------------------------
//��������: ����У��
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��          
//����ֵ: 	��
//		   
//��ע:������������0x10����������Ӧ���Լ���֮5   
//-----------------------------------------------
static void Gain_Adjust( BYTE *Buf )
{
	BYTE i,j,DivCurrentNo;
	DWORD tdw;
	DWORD RatedPower[3];
	DWORD SampleP[3];
	float gf;
	
	//��ֹ����� У������ʱ�����
	if( labs(RemoteValue.Cos[0].l) < 8000 )
	{
		return;
	}
	
	for(i=0; i<MAX_PHASE; i++)
	{
		if( (MeterTypesConst==METER_3P3W) && (i==1) )
		{
			continue;
		}
			
		CLEAR_WATCH_DOG;
		
		//��ʼ���Ĵ���	
		ResetSampleReg(w_PGainA+i);
		ResetSampleReg(w_QGainA+i);
		ResetSampleReg(w_SGainA+i);
		
		SampleP[i] = 0;
	}
	
	//1.7hz��������������
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	gf = 3089.90478515625 * 1000 / HFCONST_DEF / (api_GetActiveConstant());//����ϵ�� ����0.001W
	
	//�������Ĺ���
	for(j=0;j<6;j++)
	{
		for(i=0;i<MAX_PHASE;i++)
		{
			if( (MeterTypesConst==METER_3P3W) && (i==1) )
			{
				continue;
			}	
			
			tdw  = ReadMeasureData( r_PA+i );
			if( tdw > TWO_POW23 )
			{
				tdw = TWO_POW24 - tdw;
				tdw = (DWORD)(gf * tdw);
			}
			else
			{
				tdw = (DWORD)(gf * tdw);
			}
			SampleP[i] += tdw;				
		}	
		CLEAR_WATCH_DOG;
		api_Delayms(600);
		CLEAR_WATCH_DOG;
	}
	
	//��������ı�׼����
	for(i=0;i<MAX_PHASE;i++)	
	{
		DivCurrentNo = GetRowSampleSegmentAdjustNo( i );
		//������λ���������Ĺ���ֵ
		if( (Buf[1]==0xff) && (Buf[2]==0xff) && (Buf[42]==0xaa) )
		{
			memcpy( (BYTE*)&RatedPower[i], Buf+12+i*13, 4 );
		}
		else
		{
			if(SelDivAdjust == YES)
			{
				RatedPower[i] = (DWORD)(wCaliVoltageConst/10) * AdjustDivCurrent[DivCurrentNo];
			}
			else
			{
				RatedPower[i] = (DWORD)(wCaliVoltageConst / 10) * dwBigStandardCurrent;
			}
		}
	}	
	
	for(i=0;i<MAX_PHASE;i++)
	{
		if( (MeterTypesConst==METER_3P3W) && (i==1) )
		{
			continue;
		}	
		
		SampleP[i] /= 6;
		
		gf = (float)SampleP[i] - RatedPower[i];

		gf /= RatedPower[i];
	
		gf = (0-gf) / (1+gf);

		if( gf < 0 )
		{
			gf = TWO_POW16 + gf*TWO_POW15;
		}
		else
		{
			gf = gf * TWO_POW15;
		}

		tdw = (DWORD)gf;

		//��ϵ�����浽ee
		if( SelDivAdjust == YES )
		{
			SampleAdjustParaDiv.PGainDiv[i][DivCurrentNo] = tdw;
			
			//��дeeУ��ϵ������
			if( i == ( MAX_PHASE-1) )
			{
				api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustParaDiv), sizeof(TSampleAdjustParaDiv), (BYTE*)&SampleAdjustParaDiv);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}			
		}
		if( (SelDivAdjust != YES) || (AdjustDivCurrent[DivCurrentNo] == dwBigStandardCurrent) )
		{
			WriteSampleParaToEE(0xff, w_PGainA+i, (BYTE *)&tdw );
			WriteSampleParaToEE(0xff, w_QGainA+i, (BYTE*)&tdw);
			WriteSampleParaToEE(0xff, w_SGainA+i, (BYTE*)&tdw);
		}
		//д����оƬ
		WriteSampleReg(w_PGainA+i, tdw);
		WriteSampleReg(w_QGainA+i, tdw);
		WriteSampleReg(w_SGainA+i, tdw);
	}
}

//-----------------------------------------------
//��������: ���У��
//
//����:		Type[in] 0--����� 1--С���
//			Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��           
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void PH_Adjust(BYTE Type,BYTE * Buf)
{	
	BYTE i,j,DivCurrentNo;
	DWORD tdw;
	DWORD RatedPower[3];
	DWORD SampleP[3];
	float gf;
	
	ASSERT( Type < 2, 0 );
	
	//��ֹ����� У�����ʱ������
	if( (labs(RemoteValue.Cos[0].l) > 6500)||(labs(RemoteValue.Cos[0].l) < 3500) )
	{
		return;
	}
	
	for(i=0; i<MAX_PHASE; i++)
	{
		if((MeterTypesConst==METER_3P3W)&&( i == 1 ))
		{
			continue;
		}
						
		//��ʼ���Ĵ���	
		if( Type == 1 )
		{
			ResetSampleReg(w_PhSregApq1+i);
		}
		else
		{
			ResetSampleReg(w_PhSregApq0+i);
			//����򿪷ֶξ��� ��Ҫ��С�������Ҳ��� ��Ϊ�ֶο��ܲ��õ���С�����Ϊ����ֵ
			if(SelDivAdjust == YES)
			{
				ResetSampleReg(w_PhSregApq1+i);
			}
		}
		
		SampleP[i] = 0;
	}
	
	//1.7hz��������������
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//��������ı�׼����
	for(i=0;i<MAX_PHASE;i++)
	{
		DivCurrentNo = GetRowSampleSegmentAdjustNo( i );
		//������λ���������Ĺ���ֵ
		if( (Buf[1]==0xff) && (Buf[2]==0xff) && (Buf[42]==0xaa) )
		{
			memcpy( (BYTE*)&RatedPower[i], Buf+12+i*13, 4 );//��λw��3λС��
		}
		else
		{
			//��λw��3λС��
			if( Type == 1 )
			{
				RatedPower[i] = (DWORD)(wCaliVoltageConst / 10) * wSmallStandardCurrent / 2;
			}
			else
			{
				if(SelDivAdjust == YES)
				{
					RatedPower[i] = (DWORD)(wCaliVoltageConst / 10) * AdjustDivCurrent[DivCurrentNo] / 2;
				}
				else
				{
					RatedPower[i] = (DWORD)(wCaliVoltageConst / 10) * dwBigStandardCurrent / 2;
				}
			}
		}
	}
	
	
	gf = 3089.90478515625 * 1000 / HFCONST_DEF / (api_GetActiveConstant());//����ϵ�� ����0.001W
	
	//�������Ĺ��� ��6��ȥƽ��ֵ
	for(j=0;j<6;j++)
	{
		for(i=0;i<MAX_PHASE;i++)
		{
			if( (MeterTypesConst==METER_3P3W) && (i==1) )
			{
				continue;
			}	
			
			tdw  = ReadMeasureData( r_PA+i );
			if( tdw > TWO_POW23 )
			{
				tdw = TWO_POW24 - tdw;
				tdw = (DWORD)(gf * tdw);
			}
			else
			{
				tdw = (DWORD)(gf * tdw);
			}
			SampleP[i] += tdw;				
		}	
			
		CLEAR_WATCH_DOG;
		api_Delayms(600);
		CLEAR_WATCH_DOG;
	}

		
	for(i=0; i<MAX_PHASE; i++)
	{		
		if( (MeterTypesConst==METER_3P3W) && (i==1) )
		{
			continue;
		}	
		
		SampleP[i] /= 6;
		
		gf = (float)SampleP[i] - RatedPower[i];
		gf /= RatedPower[i];
		
		gf *= -1;
		gf /= 1.7320508;

		if( gf < 0 )
		{
			gf = TWO_POW16 + gf*TWO_POW15;
		}
		else
		{
			gf = gf * TWO_POW15;
		}

		tdw = (DWORD)gf;

		if( Type == 0 )
		{	
			if( SelDivAdjust == YES )
			{
				SampleAdjustParaDiv.PhsRegDiv[i][DivCurrentNo] = tdw;
				
				//����ȫ��У�� �����ݴ���EEPROM
				if( i ==( MAX_PHASE-1) )
				{
					api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustParaDiv), sizeof(TSampleAdjustParaDiv), (BYTE*)&SampleAdjustParaDiv);
					api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				}
				
				//����������С���ʱ��SampleCtrlPara�е�����Ҳˢ��
				if( AdjustDivCurrent[DivCurrentNo] == wSmallStandardCurrent )
				{
					//��ϵ�����浽ee
					WriteSampleParaToEE(0xff, w_PhSregApq1+i, (BYTE*)&tdw);
				}
				//д����оƬ
				WriteSampleReg(w_PhSregApq1+i, tdw);
			}
			
			//����ǽ�ϵ�����浽ee
			if( (SelDivAdjust != YES)||(AdjustDivCurrent[DivCurrentNo] == dwBigStandardCurrent) )
			{
				WriteSampleParaToEE( 0xff, w_PhSregApq0 + i, (BYTE *)&tdw );
			}
			//д����оƬ
			WriteSampleReg(w_PhSregApq0+i, tdw);
		}
		else
		{
			//�����ֶβ��� УС���ʱ����Ӧ��Ҳд��ȥ
			if( SelDivAdjust == YES )
			{
				SampleAdjustParaDiv.PhsRegDiv[i][DivCurrentNo] = tdw;

				//����ȫ��У�� �����ݴ���EEPROM
				if( i == (MAX_PHASE - 1) )
				{
					api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleAdjustParaDiv ), sizeof(TSampleAdjustParaDiv), (BYTE *)&SampleAdjustParaDiv );
					api_ClrSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
				}
			}
			
			//��ϵ�����浽ee
			WriteSampleParaToEE(0xff, w_PhSregApq1+i, (BYTE*)&tdw);
			//д����оƬ
			WriteSampleReg(w_PhSregApq1+i, tdw);
		}
	}
}
//-----------------------------------------------
//��������: ֱ��ż��г����ʼ��
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע: 
//-----------------------------------------------
static void HalfWaveParaInit(void)
{
	//��дeeУ��ϵ������
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	
	//ֱ��ż��г��ϵ��
	memcpy( (BYTE *)&SampleHalfWavePara, (BYTE *)SampleHalfWaveDefData, (sizeof(TSampleHalfWavePara) - sizeof(DWORD)) );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWavePara ), sizeof(TSampleHalfWavePara), (BYTE *)&SampleHalfWavePara );

	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	
}

//---------------------------------------------------------------
//��������: ֱ��ż��г��ϵ����У
//
//����: 	Addr����ַ��ʶ
//			Buf����������
//                   
//����ֵ:  ��
//
//��ע:   
//---------------------------------------------------------------
static void HalfWave_Adjust( WORD Addr, BYTE *Buf )
{
	BYTE i = 0, EffectivePoint = 0;
	SWORD Data = 0, Offset;
	WORD Current = 0;
	WORD ( *Pointer )[2];
	double Error, Slope;
	
	CLEAR_WATCH_DOG;
	
	//1.0L����У����0.5L���У��
	if( (Addr == 0xA139) || (Addr == 0xA13A) )
	{
		//0.5L���У��
		if( Addr == 0xA139 )
		{
			//��ֹ����� У�����ʱ������
			if( (labs(RemoteValue.Cos[0].l) > 6500) || (labs(RemoteValue.Cos[0].l) < 3500) )
			{
				return;
			}
			Pointer = SampleHalfWavePara.Coe;
			//У��ֵ
			memcpy( &Data, Buf, sizeof(Data) );
			Error = (double)Data / 10000.0 / 1.732 * -1;
		}//1.0L����У��
		else
		{
			//��ֹ����� У������ʱ�����
			if( labs(RemoteValue.Cos[0].l) < 8000 )
			{
				return;
			}
			Pointer = SampleHalfWavePara.Gain;
			//У��ֵ
			memcpy( &Data, Buf, sizeof(Data) );
			Error = ((double)Data / 10000.0) / (1 + (double)Data / 10000.0) * -1;
		}
		
		if( Error >= 0 )
		{
			Error = Error * TWO_POW15;
		}
		else
		{
			Error = TWO_POW16 + Error * TWO_POW15;
		}
		EffectivePoint = 0;
		//����
		Current = labs( RemoteValue.I[0].l ) / 1000;
		//�ж���Ч��, ��Ч��Ϊ1��ʾ֮ǰ�Ѿ�У����1����, ��Ч�����Ϊ3��ʾ3����ȫ��У��.
		for( i = 0; i < 3; i++ )
		{
			if( ((*(*(Pointer + i) + 1)) >= HD2CurrentRangeMin)
				&& ((*(*(Pointer + i) + 1)) <= HD2CurrentRangeMax) )
			{
				EffectivePoint++;
			}
		}
		
		//�ظ����ж�(��ǰ���ظ�У��)
		//��Ч��Ϊ1
		if( EffectivePoint == 1 )
		{
			//�����ǰ�����һ������ͬ
			if( (Current > (0.9 * (*(*(Pointer) + 1))))
				&& (Current < (1.1 * (*(*(Pointer) + 1)))) )
			{
				//�����һ����Ĳ���ֵ, ��Ч����Ϊ0, �������¼�¼
				Data = (SWORD)(*(*(Pointer)));
				EffectivePoint = 0;
			}//�����ǰ��͵�һ���㲻ͬ
			else
			{
				//�����һ����Ĳ���ֵ
				Data = (SWORD)(*(*(Pointer)));
			}
		}//��Ч��Ϊ2
		else if( EffectivePoint == 2 )
		{
			//�����ǰ�����һ������ͬ
			if( (Current > (0.9 * (*(*(Pointer) + 1))))
				&& (Current < (1.1 * (*(*(Pointer) + 1)))) )
			{
				//�����һ����Ĳ���ֵ, ȥ����һ����, �������¼�¼
				Data = (SWORD)(*(*(Pointer)));
				(*(*(Pointer))) = (*(*(Pointer + 1)));
				(*(*(Pointer) + 1)) = (*(*(Pointer + 1) + 1));
				(*(*(Pointer + 1))) = 0;
				(*(*(Pointer + 1) + 1)) = 0;
				EffectivePoint = 1;
			}//�����ǰ����ڶ�������ͬ
			else if( (Current > (0.9 * (*(*(Pointer + 1) + 1))))
					 && (Current < (1.1 * (*(*(Pointer + 1) + 1)))) )
			{
				//����ڶ�����Ĳ���ֵ
				Data = (SWORD)(*(*(Pointer + 1)));
				(*(*(Pointer + 1))) = 0;
				(*(*(Pointer + 1) + 1)) = 0;
				EffectivePoint = 1;
			}//�����Ч��Ϊ2���Ҳ���ǰ��������ͬ, ˵����ǰ��Ϊ��������, ����ֱ��ϵ��
			else
			{
				Slope = (double)((SWORD)(*(*(Pointer + 1))) - (SWORD)(*(*Pointer))) / ((SWORD)(*(*(Pointer + 1) + 1)) - (SWORD)(*(*(Pointer)+1)));
				Offset = (SWORD)((SWORD)(*(*Pointer)) - (Slope * (SWORD)(*(*(Pointer) + 1))));
				Data = 0;
			}
		}//��Ч��Ϊ3
		else if( EffectivePoint == 3 )
		{
			//�����ǰ�����һ������ͬ
			if( (Current > (0.9 * (*(*(Pointer) + 1))))
				&& (Current < (1.1 * (*(*(Pointer) + 1)))) )
			{
				//�����һ����Ĳ���ֵ, ȥ����һ����, �������¼�¼
				Slope = 0;
				Offset = 0;
				Data = (SWORD)(*(*(Pointer)));
				(*(*(Pointer))) = (*(*(Pointer + 1)));
				(*(*(Pointer) + 1)) = (*(*(Pointer + 1) + 1));
				(*(*(Pointer + 1))) = (*(*(Pointer + 2)));
				(*(*(Pointer + 1) + 1)) = (*(*(Pointer + 2) + 1));
				(*(*(Pointer + 2))) = 0;
				(*(*(Pointer + 2) + 1)) = 0;
				EffectivePoint = 2;
			}//�����ǰ����ڶ�������ͬ
			else if( (Current > (0.9 * (*(*(Pointer + 1) + 1))))
					 && (Current < (1.1 * (*(*(Pointer + 1) + 1)))) )
			{
				//����ڶ�����Ĳ���ֵ, ȥ���ڶ�����, �������¼�¼
				Slope = 0;
				Offset = 0;
				Data = (SWORD)(*(*(Pointer + 1)));
				(*(*(Pointer + 1))) = (*(*(Pointer + 2)));
				(*(*(Pointer + 1) + 1)) = (*(*(Pointer + 2) + 1));
				(*(*(Pointer + 2))) = 0;
				(*(*(Pointer + 2) + 1)) = 0;
				EffectivePoint = 2;
			}//�����ǰ�������������ͬ
			else if( (Current > (0.9 * (*(*(Pointer + 2) + 1))))
					 && (Current < (1.1 * (*(*(Pointer + 2) + 1)))) )
			{
				//�����������Ĳ���ֵ
				Slope = 0;
				Offset = 0;
				Data = (SWORD)(*(*(Pointer + 2)));
				(*(*(Pointer + 2))) = 0;
				(*(*(Pointer + 2) + 1)) = 0;
				EffectivePoint = 2;
			}
			else
			{
				Data = 0;
			}
		}
		else
		{
			Data = 0;
		}
		
		//���ݼ�¼
		//�����Ч��Ϊ0, ��ǰ��Ϊ��һ����, ��¼��ǰ������
		if( EffectivePoint == 0 )
		{
			(*(*(Pointer))) = (WORD)((SWORD)Error + Data);
			(*(*(Pointer) + 1)) = Current;
		}//�����Ч��Ϊ1, ��ǰ��Ϊ�ڶ�����
		else if( EffectivePoint == 1 )
		{
			Data = Data + (SWORD)Error;
			//�����ǰ�����ֵ���ڵ�һ����ĵ���ֵ
			if( Current > (*(*(Pointer) + 1)) )
			{
				(*(*(Pointer + 1))) = (WORD)Data;
				(*(*(Pointer + 1) + 1)) = Current;
			}
			else
			{
				(*(*(Pointer + 1))) = (*(*(Pointer)));
				(*(*(Pointer + 1) + 1)) = (*(*(Pointer) + 1));
				(*(*(Pointer))) = (WORD)Data;
				(*(*(Pointer) + 1)) = Current;
			}
		}//�����Ч��Ϊ2, ��ǰ��Ϊ��������
		else if( EffectivePoint == 2 )
		{
			Data = (SWORD)(Slope * (labs( RemoteValue.I[0].l ) / 1000) + Offset) + (SWORD)Error + Data;
			//�����ǰ�����ֵ���ڵڶ�����ĵ���ֵ
			if( Current > (*(*(Pointer + 1) + 1)) )
			{
				(*(*(Pointer + 2))) = (WORD)Data;
				(*(*(Pointer + 2) + 1)) = Current;
			}
			else if( Current > (*(*(Pointer) + 1)) )
			{
				(*(*(Pointer + 2))) = (*(*(Pointer + 1)));
				(*(*(Pointer + 2) + 1)) = (*(*(Pointer + 1) + 1));
				(*(*(Pointer + 1))) = (WORD)Data;
				(*(*(Pointer + 1) + 1)) = Current;
			}
			else
			{
				(*(*(Pointer + 2))) = (*(*(Pointer + 1)));
				(*(*(Pointer + 2) + 1)) = (*(*(Pointer + 1) + 1));
				(*(*(Pointer + 1))) = (*(*(Pointer)));
				(*(*(Pointer + 1) + 1)) = (*(*(Pointer) + 1));
				(*(*(Pointer))) = (WORD)Data;
				(*(*(Pointer) + 1)) = Current;
			}
		}//3����Ч�㲻������У��ͬ��
		else
		{
			return;
		}
	}//0.5L��ʼ��
	else if( Addr == 0xA13B )
	{
		memset( SampleHalfWavePara.Coe, 0x00, sizeof(SampleHalfWavePara.Coe) );
	}//1.0L��ʼ��
	else if( Addr == 0xA13C )
	{
		memset( SampleHalfWavePara.Gain, 0x00, sizeof(SampleHalfWavePara.Gain) );
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
//			Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA У��ʽѡ�� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA У��ʽѡ��1 У��ʽѡ��2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA 	
//          У��ʽѡ��	0xffff--��λ���������У��	0x0000--Լ��ֵУ��
//����ֵ: 	TRUE/FALSE
//		   
//��ע:У���Լ����     
//-----------------------------------------------
WORD api_ProcSampleAdjust(BYTE AdjType, BYTE *Buf)
{		
	//�жϹ�Լ��ʽ�Ϸ���
	if( (Buf[0]!=0xaa) || (Buf[3]!=0xaa) )
	{
		return FALSE;
	}

	if(api_CheckEEPRomAddr(0xFF) == FALSE)//���E2��ַ,E2��ַ����ȷ������У��
	{
		return FALSE;
	}
	
	//��ʾ��ʼУ�� ������ʾ
	api_DisplayAdjust();
	
	//��ʼ����׼Դ����ֵ
	InitSampleCaliUI();

	//�ر�У��Ĵ���д����
	SampleSpecCmdOp(0xa6);
	
	switch( AdjType )
	{
		case 0xff://��ʼ��									
			AdjustInit();			
			break;
		case 0xf1://��ʼ��ֱ��ż��г��ϵ��									
			HalfWaveParaInit();			
			break;
		case 0x01://100%Ib��1.0(L)
			UIP_K(Buf);
			Gain_Adjust(Buf);	//��Чֵ����У��			
			break;
		case 0x02:
			Gain_Adjust(Buf);	//��Чֵ���澫��	
			break;
		case 0x51://0.5L(L)����� ��Ǿ���
			PH_Adjust(0,Buf);	//��λУ��
			break;		
		case 0x52://0.5L(L)С���
			PH_Adjust(1,Buf);	//��λУ��
			break;
			
			//���ߵ�����У��������Уƫ��!!!
		case 0x81:
			if(SelZeroCurrentConst == YES)
			{
				ZeroCurrentGain_Adjust( Buf );//���ߵ�������ϵ��У��
			}
			break;
			
		case 0x00:
			if(SelZeroCurrentConst == YES)
			{
				I0rmsOffset_Adjust();//����ƫ��У��
			}
			IrmsOffset_Adjust();//A��B��C�����ƫ��У��
			break;
			
		default: 
			break;
	}	
	
	api_WriteFreeEvent(EVENT_ADJUST_METER, (WORD)AdjType);

	//��У��Ĵ���д����
	SampleSpecCmdOp(0x00);
	
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//����������󣬶���У��ϵ����Ч���
	GetSampleChipCheckSum();
	
	return TRUE;
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
//
//����ֵ: 	TRUE/FALSE
//		   
//��ע: 7038��0000~3FFFΪУ���Ĵ���	4000~8fffΪ�����Ĵ���  
//-----------------------------------------------
BOOL api_ProcSampleCorr(BYTE Operation, WORD Addr, BYTE Len, BYTE *Buf)
{
	WORD i,Result,WRFlag[2];
	DWORD tdw;
	BYTE SampleAddr,DivCurrentNo;
	
	memcpy((BYTE*)&tdw,Buf,4);
	SampleAddr = (Addr&0x00ff);	
		
	if( Operation == WRITE )
	{
		if( Addr < 0x4000 )//У���Ĵ���
		{	
			SampleSpecCmdOp(0xa6);
			Result = WriteSampleReg(SampleAddr,tdw&0x0000ffff);	
			//����򿪾��� �������д��ʱͬʱд��С���ȥ
			if(SelDivAdjust == YES)
			{
				if((SampleAddr >= w_PhSregApq0)&&(SampleAddr <= w_PhSregCpq0))
				{
					Result &= WriteSampleReg( SampleAddr + (w_PhSregApq1 - w_PhSregApq0), tdw&0x0000ffff );
				}
			}
			SampleSpecCmdOp( 0x00 );
			if( Result == FALSE )
			{
				return FALSE;
			}
			
			//��ϵ�����浽ee
			if( Addr < 0x2000 )
			{
				//ֻ���浽ram������оƬ�������浽eeprom�������ϵ�ʱ�ָ�ԭֵ
				WriteSampleParaToEE(0x00, SampleAddr, (BYTE*)&tdw);	
			}
			else
			{
				//���浽ram������оƬ��eeprom
				WriteSampleParaToEE(0xff, SampleAddr, (BYTE*)&tdw);	
				
				api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+SampleAddr);
			}
			
			//����������󣬶���У��ϵ����Ч���
			GetSampleChipCheckSum();
			
			// Ӧ�ü�¼�澯�¼�
			api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+SampleAddr);
		}
		else if((Addr >= 0xA100)&&(Addr <= 0xA1FF))//������ز���
		{
			WRFlag[0] = FALSE;
			WRFlag[1] = FALSE;
			
			if(Addr <= 0xA102) 	//����ֶ�ϵ��
			{
				memcpy( SampleAdjustParaDiv.PGainDiv[Addr - 0xA100], Buf, sizeof(SampleAdjustParaDiv.PGainDiv[0]));
				WRFlag[0] = TRUE;
			}
			else if(Addr == 0xA10F)	//����ֶ�ϵ�����ݿ�
			{
				memcpy( SampleAdjustParaDiv.PGainDiv, Buf, sizeof(SampleAdjustParaDiv.PGainDiv) );
				WRFlag[0] = TRUE;
			}
			else if(Addr <= 0xA112)	//��Ƿֶ�ϵ��
			{
				memcpy( SampleAdjustParaDiv.PhsRegDiv[Addr - 0xA110], Buf, sizeof(SampleAdjustParaDiv.PhsRegDiv[0]) );
				WRFlag[0] = TRUE;
			}
			else if(Addr == 0xA11F)	//��Ƿֶ�ϵ�����ݿ�
			{
				memcpy( SampleAdjustParaDiv.PhsRegDiv, Buf, sizeof(SampleAdjustParaDiv.PhsRegDiv) );
				WRFlag[0] = TRUE;
			}
			else if(Addr <= 0xA122)	//�޹�����ϵ��
			{
				memcpy( SampleManualModifyPara.ManualModifyQGain[Addr - 0xA120], Buf, sizeof(SampleManualModifyPara.ManualModifyQGain[0]) );
				WRFlag[1] = TRUE;
			}
			else if(Addr == 0xA12F)	//�޹�����ϵ�����ݿ�
			{
				memcpy( SampleManualModifyPara.ManualModifyQGain, Buf, sizeof(SampleManualModifyPara.ManualModifyQGain) );
				WRFlag[1] = TRUE;
			}
			else if(Addr == 0xA130)	//�����й�����ϵ��
			{
				memcpy( SampleManualModifyPara.ManualModifyForRA, Buf, sizeof(SampleManualModifyPara.ManualModifyForRA) );
				WRFlag[1] = TRUE;
			}
			else if(Addr == 0xA131)	//����оƬTOffset�²���ֵ
			{
				SampleManualModifyPara.ManualModifyChipToffset = Buf[0];
				WRFlag[1] = TRUE;
			}
			else if( (Addr >= 0xA133) && (Addr <= 0xA135) )
			{
				//���ĸ�ϵ��������ദ�ڵ�ѹ��ƽ���״̬ ��ֹУ��
				if( JudgeUnBalance( Addr - 0xA133 ) == FALSE )
				{
					return FALSE;
				}
				memcpy( &SampleManualModifyPara.VolUnbalanceGain[Addr - 0xA133], Buf, sizeof(SampleManualModifyPara.VolUnbalanceGain[Addr - 0xA133]) );
				WRFlag[1] = TRUE;
				UnBalanceGainFlag[Addr - 0xA133] = FALSE; //��Ϊδˢ�� ���µ�У����д�����оƬ
			}
			else if(Addr == 0xA136) //����ƫ��ϵ��ȫ������
			{
				memcpy( SampleManualModifyPara.ManualModifyForWard, Buf, sizeof(SampleManualModifyPara.ManualModifyForWard) );
				WRFlag[1] = TRUE;
			}
			else if(Addr == 0xA137) //������������ƫ��ϵ��
			{
				//���ø�ϵ��ʱ A����������ڶ�Ӧ������
				DivCurrentNo = GetRowSampleSegmentAdjustNo( 0 );
				if(DivCurrentNo < 9)
				{
					SampleManualModifyPara.ManualModifyForWard[DivCurrentNo] = Buf[0];
					WRFlag[1] = TRUE; 
				}
				else
				{
					return FALSE;
				}
			}
			else if(Addr == 0xA138) //�������÷���ƫ��ϵ��
			{
				//���ø�ϵ��ʱ A����������ڶ�Ӧ������
				DivCurrentNo = GetRowSampleSegmentAdjustNo( 0 ); 
				if(DivCurrentNo < 9)
				{
					SampleManualModifyPara.ManualModifyForRA[DivCurrentNo] = Buf[0];
					WRFlag[1] = TRUE; 
				}
				else
				{
					return FALSE; 
				}
			}//ֱ��ż��г��У��
			else if( (Addr >= 0xA139) && (Addr <= 0xA13E) )
			{
				//дȫ��0.5Lϵ��
				if( Addr == 0xA13D )
				{
					memcpy( SampleHalfWavePara.Coe, Buf, sizeof(SampleHalfWavePara.Coe) );
				}//дȫ��1.0Lϵ��
				else if( Addr == 0xA13E )
				{
					memcpy( SampleHalfWavePara.Gain, Buf, sizeof(SampleHalfWavePara.Gain) );
				}//У��
				else
				{
					HalfWave_Adjust( Addr, Buf );
				}
				//ϵ����EEPROM
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWavePara ), sizeof(TSampleHalfWavePara), (BYTE *)&SampleHalfWavePara );
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}
			else if( Addr == 0xA13F )//ֱ��ż��г��ʱ�����
			{
				memcpy( &SampleHalfWaveTimePara.Hours, Buf, sizeof(SampleHalfWaveTimePara.Hours) );
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWaveTimePara ), sizeof(TSampleHalfWaveTimePara), (BYTE *)&SampleHalfWaveTimePara );
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				return TRUE;
			}
			else
			{
				return FALSE; 
			}

			//ϵ����EEPROM
			api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			if(WRFlag[0] == TRUE)
			{
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleAdjustParaDiv ), sizeof(TSampleAdjustParaDiv), (BYTE *)&SampleAdjustParaDiv );
			}
			if(WRFlag[1] == TRUE)
			{
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleManualModifyPara ), sizeof(TSampleManualModifyPara), (BYTE *)&SampleManualModifyPara );
			}
			api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			
			//�ѵ�ǰ����������������ΪFF ���¿�ʼ����
			memset( g_ucNowSampleDivisionNo, 0xFF, sizeof(g_ucNowSampleDivisionNo) );
		}
	}
	else
	{
		if( Addr < 0x4000 )//У���Ĵ���
		{
			//���ݿ鷽ʽ����
			if(Addr == 0x3fff)
			{
				if( Len < (sizeof(TSampleCtrlPara)+sizeof(TSampleAdjustPara)-sizeof(DWORD)*2)/sizeof(WORD)*3 )
				{
					return FALSE;
				}
				
				for(i=0;i<(sizeof(TSampleCtrlPara)-sizeof(DWORD))/sizeof(WORD);i++)
				{
					*Buf = SampleCtrlAddr[i];
					Buf += 1;
					memcpy(Buf,(BYTE*)&SampleCtrlPara+i*sizeof(WORD),sizeof(WORD));
					Buf += 2;
				}
				for(i=0;i<(sizeof(TSampleAdjustPara)-sizeof(DWORD))/sizeof(WORD);i++)
				{
					*Buf = SampleAdjustAddr[i];
					Buf += 1;
					memcpy(Buf,(BYTE*)&SampleAdjustPara+i*sizeof(WORD),sizeof(WORD));
					Buf += 2;
				}
			}
			else
			{
				tdw = ReadCalibrateData(SampleAddr);	
				memcpy(Buf,(BYTE*)&tdw,Len);
			}
		}
		else if( Addr < 0x9000 )//�����Ĵ���
		{
			tdw = ReadMeasureData(SampleAddr);	
			memcpy(Buf,(BYTE*)&tdw,Len);
		}
		else if((Addr >= 0xA100)&&(Addr <= 0xA1FF))//������������
		{
			//�������ݳ��ȸ����·��ĳ���ȷ�� ������Ȳ��� ���ݳ�����ȫ ������ȳ� ���ȡ��������
			if(Len > 65) //���в������60���ֽ� ���Ƴ���65���ֽ� ��ֹLen�����쳣�����ڴ����
			{
				return FALSE;
			}
			if(Addr <= 0xA102)  //����ֶ�ϵ��
			{
				memcpy( Buf, SampleAdjustParaDiv.PGainDiv[Addr - 0xA100], Len );
			}
			else if(Addr == 0xA10F)	//����ֶ�ϵ�����ݿ�
			{
				memcpy( Buf, SampleAdjustParaDiv.PGainDiv, Len );
			}
			else if(Addr <= 0xA112)	//��Ƿֶ�ϵ��
			{
				memcpy( Buf, SampleAdjustParaDiv.PhsRegDiv[Addr - 0xA110], Len );
			}
			else if(Addr == 0xA11F)	//��Ƿֶ�ϵ�����ݿ�
			{
				memcpy( Buf, SampleAdjustParaDiv.PhsRegDiv, Len );
			}
			else if(Addr <= 0xA122)	//�޹�����ϵ��
			{
				memcpy( Buf, SampleManualModifyPara.ManualModifyQGain[Addr - 0xA120], Len );
			}
			else if(Addr == 0xA12F)	//�޹�����ϵ�����ݿ�
			{
				memcpy( Buf, SampleManualModifyPara.ManualModifyQGain, Len );
			}
			else if(Addr == 0xA130)	//�����й�����ϵ��
			{
				memcpy( Buf, SampleManualModifyPara.ManualModifyForRA, Len );
			}
			else if(Addr == 0xA131)	//����оƬTOffset�²���ֵ
			{
				Buf[0] = SampleManualModifyPara.ManualModifyChipToffset;
			}
			else if( (Addr >= 0xA133) && (Addr <= 0xA135) ) //��ѹ��ƽ��ϵ��
			{
				memcpy( Buf, &SampleManualModifyPara.VolUnbalanceGain[Addr - 0xA133], Len );
			}
			else if(Addr == 0xA136) //�����й�����ϵ��
			{
				memcpy( Buf, SampleManualModifyPara.ManualModifyForWard, Len );
			}
			else if(Addr == 0xA137) //��ȡ��ǰ������ƫ��ϵ��
			{
				DivCurrentNo = GetRowSampleSegmentAdjustNo( 0 );
				if(DivCurrentNo < 9)
				{
					Buf[0] = SampleManualModifyPara.ManualModifyForWard[DivCurrentNo]; 
				}
				else
				{
					return FALSE;
				}
			}
			else if(Addr == 0xA138) //��ȡ��ǰ�η���ƫ��ϵ��
			{
				DivCurrentNo = GetRowSampleSegmentAdjustNo( 0 );
				if(DivCurrentNo < 9)
				{
					Buf[0] = SampleManualModifyPara.ManualModifyForRA[DivCurrentNo]; 
				}
				else
				{
					return FALSE;
				}
			}
			else if( (Addr >= 0xA139) && (Addr <= 0xA13F) ) //ֱ��ż��г������
			{
				//��ȫ��0.5Lϵ��
				if( Addr == 0xA13D )
				{
					memcpy( Buf, SampleHalfWavePara.Coe, sizeof(SampleHalfWavePara.Coe) );
				}//��ȫ��1.0Lϵ��
				else if( Addr == 0xA13E )
				{
					memcpy( Buf, SampleHalfWavePara.Gain, sizeof(SampleHalfWavePara.Gain) );
				}//ֱ��ż��г��ʱ�����
				else if( Addr == 0xA13F )
				{
					memcpy( Buf, &SampleHalfWaveTimePara.Hours, sizeof(SampleHalfWaveTimePara.Hours) );
				}
				else
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
	//ֱ��ż��г��ʱ�����
	SampleHalfWaveTimePara.Hours = HalfWaveDefCheckTime;
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWaveTimePara ), sizeof(TSampleHalfWaveTimePara), (BYTE *)&SampleHalfWaveTimePara );
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
}


#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
