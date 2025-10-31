//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.6
//����		����оƬHT7038У������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7032L.h"

#if(SAMPLE_CHIP==CHIP_HT7032L)

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
		dwSmallStandardCurrent = 6000;
		
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
static void ResetSampleReg(WORD Reg_Addr)
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
			CaliStandardI[i] = (DWORD)dwBigStandardCurrent*10;		//4λС��
		}
	}	
	
	for(i=0; i<3; i++)
	{
		if((MeterTypesConst==METER_3P3W)&&( i == 1 ))
		{
			continue;
		}
		
		//��ʼ���Ĵ���	
		ResetSampleReg(w_GainUA+i);
		ResetSampleReg(w_GainIA+i);
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
			dwData = ReadSampleData( r_RMSUA+i );
			dwAdjustU[i] += dwData*1000/TWO_POW13;
			
			//����
			dwData = ReadSampleData( r_RMSIA+i );
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
		WriteSampleParaToEE(0xff, w_GainUA+i, (BYTE*)&dwData);
		//д����оƬ
		WriteSampleReg( w_GainUA+i, dwData );
		
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
		WriteSampleParaToEE(0xff, w_GainIA+i, (BYTE*)&dwData);
		//д����оƬ
		WriteSampleReg( w_GainIA+i, dwData );
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
		CaliStandardI = dwSmallStandardCurrent * 10;//4λС��
	}
	
	//��ʼ���Ĵ���
	ResetSampleReg( w_GainIN );
	
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
		dwData = ReadSampleData( r_RMSIN );
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
	WriteSampleParaToEE(0xff, w_GainIN, (BYTE*)&dwData);
	//д����оƬ
	WriteSampleReg( w_GainIN, dwData );
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
		Irms = ReadSampleData( r_RMSIN );
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
	ResetSampleReg( w_RmsOffsetIT );
	ResetSampleReg( w_RmsOffsetIA );
	ResetSampleReg( w_RmsOffsetIB );
	ResetSampleReg( w_RmsOffsetIC );
	//1.7hz��������������
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//����ʸ����ƫ��У��
	for( i = 0; i < 10; i++ )
	{
		Irms = ReadSampleData( r_RMSIT );
		IrmsSum += Irms;
		CLEAR_WATCH_DOG;
		api_Delayms(600);
	}
	CLEAR_WATCH_DOG;
	Irms = IrmsSum / 10;
	IRmsOffset = (DWORD)((double)Irms * (double)Irms / (double)TWO_POW15 + 0.5);
	//дee
	WriteSampleParaToEE( 0xFF, w_RmsOffsetIT, (BYTE *)&IRmsOffset );
	//д����оƬ
	WriteSampleReg( w_RmsOffsetIT, IRmsOffset );

	//A��B��C�����ƫ��У��
	for( i = 0; i < MAX_PHASE; i++ )
	{
		IrmsSum = 0;
		//������Чֵƫ��У��
		for( j = 0; j < 10; j++ )
		{
			Irms = ReadSampleData( r_RMSIA + i );
			IrmsSum += Irms;
			CLEAR_WATCH_DOG;
			api_Delayms(600);
		}
		CLEAR_WATCH_DOG;
		Irms = IrmsSum / 10;
		IRmsOffset = (DWORD)((double)Irms * (double)Irms / (double)TWO_POW15 + 0.5);
		//дee
		WriteSampleParaToEE( 0xFF, w_RmsOffsetIA + i, (BYTE *)&IRmsOffset );
		//д����оƬ
		WriteSampleReg( w_RmsOffsetIA + i, IRmsOffset );
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
		ResetSampleReg(w_GPA+i);
		ResetSampleReg(w_GQA+i);
		ResetSampleReg(w_GSA+i);
		
		SampleP[i] = 0;
	}
	
	//1.7hz��������������
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	gf = 386.238 * 1000 / HFCONST_DEF / (api_GetActiveConstant());//����ϵ�� ����0.001W
	
	//�������Ĺ���
	for(j=0;j<6;j++)
	{
		for(i=0;i<MAX_PHASE;i++)
		{
			if( (MeterTypesConst==METER_3P3W) && (i==1) )
			{
				continue;
			}	
			
			tdw  = ReadSampleData( r_PA+i );
			if( tdw > TWO_POW31 )
			{
				tdw = TWO_POW32 - tdw;
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
			WriteSampleParaToEE(0xff, w_GPA+i, (BYTE *)&tdw );
			WriteSampleParaToEE(0xff, w_GQA+i, (BYTE*)&tdw);
			WriteSampleParaToEE(0xff, w_GSA+i, (BYTE*)&tdw);
		}
		//д����оƬ
		WriteSampleReg(w_GPA+i, tdw);
		WriteSampleReg(w_GQA+i, tdw);
		WriteSampleReg(w_GSA+i, tdw);
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
			ResetSampleReg(w_GphsA0+3*i);
			ResetSampleReg(w_GphsA1+3*i);
			ResetSampleReg(w_GphsA2+3*i);
		}
		else
		{
			ResetSampleReg(w_GphsA0+3*i);
			ResetSampleReg(w_GphsA1+3*i);
			ResetSampleReg(w_GphsA2+3*i);
			//����򿪷ֶξ��� ��Ҫ��С�������Ҳ��� ��Ϊ�ֶο��ܲ��õ���С�����Ϊ����ֵ
			if(SelDivAdjust == YES)
			{
				ResetSampleReg(w_GphsA0+3*i);
				ResetSampleReg(w_GphsA1+3*i);
				ResetSampleReg(w_GphsA2+3*i);
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
				RatedPower[i] = (DWORD)(wCaliVoltageConst / 10) * dwSmallStandardCurrent / 2;
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
	
	
	gf = 386.238 * 1000 / HFCONST_DEF / (api_GetActiveConstant());//����ϵ�� ����0.001W
	
	//�������Ĺ��� ��6��ȥƽ��ֵ
	for(j=0;j<6;j++)
	{
		for(i=0;i<MAX_PHASE;i++)
		{
			if( (MeterTypesConst==METER_3P3W) && (i==1) )
			{
				continue;
			}	
			
			tdw  = ReadSampleData( r_PA+i );
			if( tdw > TWO_POW31 )
			{
				tdw = TWO_POW32 - tdw;
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
			//д����оƬ
			WriteSampleReg(w_GphsA0+3*i, tdw);
			//д����оƬ
			WriteSampleReg(w_GphsA1+3*i, tdw);
			//д����оƬ
			WriteSampleReg(w_GphsA2+3*i, tdw);
			//��С���ʱ��SampleCtrlPara�е�����Ҳˢ��
			if( AdjustDivCurrent[DivCurrentNo] == dwSmallStandardCurrent )
			{
				//��ϵ�����浽ee
				WriteSampleParaToEE( 0xff, w_GphsA1+3*i, (BYTE*)&tdw );
			}
			//����ǽ�ϵ�����浽ee
			if(AdjustDivCurrent[DivCurrentNo] == dwBigStandardCurrent)
			{
				WriteSampleParaToEE( 0xff, w_GphsA0+3*i, (BYTE *)&tdw );
			}
		}
		else
		{
			
			
			if(Type == 0)
			{
				//д����оƬ
				WriteSampleReg(w_GphsA0+3*i, tdw);
				WriteSampleParaToEE( 0xff, w_GphsA0+3*i, (BYTE *)&tdw );
			}
			else
			{
				//д����оƬ
				WriteSampleReg(w_GphsA1+3*i, tdw);
				//д����оƬ
				WriteSampleReg(w_GphsA2+3*i, tdw);
				//��ϵ�����浽ee
				WriteSampleParaToEE( 0xff, w_GphsA1+3*i, (BYTE*)&tdw );
			}
		}
		
		
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
	WORD SampleAddr,DivCurrentNo;
	
	memcpy((BYTE*)&tdw,Buf,4);
	SampleAddr = (Addr&0x0fff);	
		
	if( Operation == WRITE )
	{
		if( Addr < 0x4000 )//У���Ĵ���
		{	
			SampleSpecCmdOp(0xa6);
			Result = WriteSampleReg(SampleAddr,tdw);	
			//����򿪾��� �������д��ʱͬʱд��С���ȥ
//			if(SelDivAdjust == YES)
//			{
//				if((SampleAddr >= w_GphsA0)&&(SampleAddr <= w_GphsC1))
//				{
//					Result &= WriteSampleReg( SampleAddr + (w_PhSregApq1 - w_PhSregApq0), tdw&0x0000ffff );
//				}
//			}
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
	}
	else
	{
		tdw = ReadSampleData(SampleAddr);	
		memcpy(Buf,(BYTE*)&tdw,Len);
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
}


#endif//#if(SAMPLE_CHIP==CHIP_HT7032L)