//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.7.30
//����		����оƬHT7017У���ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7053D.h"

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
TSampleUIK	TF415UIK;
TTimeTem	TF415TimeTem;
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------


//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: У��ϵ�����浽EEPROM������ˢ�±�־
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void WriteEEpromSampleAdjustReg(void)			
{
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg),sizeof(TSampleAdjustReg),(BYTE*)&SampleAdjustReg);
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
}

//-----------------------------------------------
//��������: ����ϵ�����浽EEPROM������ˢ�±�־
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void WriteEEpromSampleCtrlReg(void)			
{
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg),sizeof(TSampleCtrlReg),(BYTE*)&SampleCtrlReg);
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
}

//-----------------------------------------------
//��������: ˲ʱ��ϵ�����浽EEPROM
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void WriteEEpromSampleCorr( void )
{
	api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleCorr ), sizeof(TSampleCorr), SampleCorr.UGain.b );
	api_ClrSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
}
//---------------------------------------------------------------
//��������: ��ѹӰ����ϵ�����浽EEPROM
//
//����:    ��
//                   
//����ֵ:  ��
//
//��ע:   
//---------------------------------------------------------------
static void WriteEEpromSampleVolCorr( void )
{
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleVolCorr),sizeof(TSampleVolCorr),(BYTE*)&SampleVolCorr);
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
void ResetSampleReg(BYTE Reg_Addr)
{
    BYTE Buf[2];

	Buf[0] = 0;
	Buf[1] = 0;
	Link_WriteSampleReg( Reg_Addr, Buf );
}

//-----------------------------------------------
//��������: ���ݶ����Ĳ���оƬ����ֵ�����۹���ֵ�������
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA HF���� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//			CF_f[in]	��������ƽ������
//          
//����ֵ: 	��������
//		   
//��ע:   
//-----------------------------------------------
static float Get_Err_Data(BYTE *Buf, BYTE CF_f)
{
	TFourByteUnion K1, K2, K3;
	TFourByteUnion td;
	float Err_Data;
	WORD Result;
	BYTE i;	
	
	//����	
	memcpy(K3.b,Buf+12,4);
	
	K2.d = 0;
	for(i=0; i<CF_f; i++)
	{
		Result = Link_ReadSampleReg(PowerPA,td.b);
		if( Result == FALSE ) 
		{
			return (0xff);
		}
		
		if(td.b[2]&0x80)
		{
			td.d = 0x01000000-td.d;
		}
		
		K2.d += td.d;
		
		CLEAR_WATCH_DOG;
		
		api_Delayms(500);
	}
	
	CLEAR_WATCH_DOG;
	
	td.d = K2.d/CF_f;
	
	K1.d = (DWORD)((double)td.d*1000000.0/SampleCorr.P1Gain.d);//�����ʵ�ʹ���ֵ
	
	Err_Data = ((float)K1.d - K3.d)/K3.d;	
	return(Err_Data);
}

//--------------------------------------------------
//��������:  �Ա��Ľ��յ�������ݽ��д���
//         
//����:      Buf[in]Ҫд������� ���ֽڣ����ں󣬵���ǰ
//		     
//         
//����ֵ:    ��������
//         
//��ע:  �ú������������У����bufָ���������  ���ݳ���Ϊ2���ֽ�
//--------------------------------------------------
static float  GetProtolErrData( BYTE  *Buf)
{
	float Err_Data;
	TTwoByteUnion K1;

	K1.w = 0;
	memcpy(K1.b, Buf, 2);
	
	// ��λ�����10000
	Err_Data = (float)(K1.sw / 1000000.0);

	return(Err_Data);
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
static void InitSamplePara( BYTE *InBuf )
{	
    TFourByteUnion ComData;
	WORD ANAEN = 0, ANACON = 0;

	//��ʼ����������
	memcpy(SampleAdjustReg.GP1.b, (BYTE*)&SampleAdjustDefData, sizeof(TSampleAdjustReg)-sizeof(DWORD));
	if( (InBuf[0] != 0x00) || (InBuf[1] != 0x00) )
	{
		SampleAdjustReg.RHFConst.b[0] = InBuf[0];	//HFConst��ֵ
		SampleAdjustReg.RHFConst.b[1] = InBuf[1];
	}
	WriteEEpromSampleAdjustReg();//У��ϵ�����ݴ���
	
	//�Ͳ�ʱ������ѹӰ����, ����ʼ���������Ĵ���, ��ֹ������ʵ���޸ļĴ�����ֵ���ֲ�С�ĳ�ʼ��
	//����ʱ�رյ�ѹӰ����, ��ʼ���Ĵ���
	if( SelVolCorrectConst == YES )
	{
		ANAEN = SampleCtrlReg.ANAEN.w;
		ANACON = SampleCtrlReg.ANACON.w;
	}
    Link_ReadSampleReg(DeviceID,ComData.b);
    if( ComData.d == 0x705304 )//5000:1оƬ
    {
        memcpy( (BYTE *)&SampleCtrlReg, (BYTE *)&SampleCtrlDefData, sizeof(TSampleCtrlReg)-sizeof(DWORD) );
    }
    else//8000:1оƬ
    {
        memcpy( (BYTE *)&SampleCtrlReg, (BYTE *)&SampleCtrlDefData8000, sizeof(TSampleCtrlReg)-sizeof(DWORD) );
    }
	if( SelVolCorrectConst == YES )
	{
		SampleCtrlReg.ANAEN.w = ANAEN;
		SampleCtrlReg.ANACON.w = ANACON;
	}
    
    WriteEEpromSampleCtrlReg();//���Ʋ������ݴ���
    
	//��ʼ��˲ʱ��ϵ��
	SampleCorr.UGain.d = METER_Uk+Uoffset;
	SampleCorr.I1Gain.d = METER_I1k;
	SampleCorr.I2Gain.d = METER_I2k;
	SampleCorr.P1Gain.d = METER_Pk;
	WriteEEpromSampleCorr();
	
	//��ʼ����ѹӰ����ϵ��
	if( SelVolCorrectConst == YES )
	{
		memset( &SampleVolCorr, 0x00, sizeof(TSampleVolCorr) );
		WriteEEpromSampleVolCorr();
	}
	
	//��ʼ������ϵ��
	
	//���³�ʼ������оƬ
	api_PowerUpSample();
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
//��ע:   
//-----------------------------------------------
static void UIP_K(BYTE *Buf)
{
	BYTE i;
	WORD Result;
	TFourByteUnion td;	
	TFourByteUnion RegData;
	TFourByteUnion K[3];
	float tf;
	
	//��׼��ѹ
	memcpy(K[0].b,Buf+4,4);
	//��׼����
	memcpy(K[1].b,Buf+8,4);

	Link_ReadSampleReg(URMS,RegData.b);
	SampleCorr.UGain.d = RegData.d*1000/K[0].d;			//��ѹϵ��
	
	Link_ReadSampleReg(IARMS,RegData.b);
	SampleCorr.I1Gain.d = RegData.d*1000/K[1].d;			//����Aϵ��
        
	Result = Link_ReadSampleReg(HFConst,td.b);
	if( Result == FALSE ) 
	{
		return;
	}       
        
	K[2].d = api_GetActiveConstant();	
	tf = (float)83886.08*K[2].d/562500;
	#if(SEL_CRYSTALOSCILLATOR_55296M == YES)
	tf = tf/0.9216;
	#endif
	SampleCorr.P1Gain.d = (DWORD)(tf*td.d);	//���㹦��ϵ��

	td.d = wCaliVoltageConst * wMeterBasicCurrentConst / 10; 		//����ʵ�ʹ��� ����6λС��
	td.d = (DWORD)((double)td.d * SampleCorr.P1Gain.d / 1000000.0); //����ʵ�ʹ���ֵ��Ӧ�ļĴ���ֵ
	td.d = td.d * 3 / 1000;                 //��Ǳ����ֵ = 0.3%*P
	SampleAdjustReg.RPStart.w = td.w[0];
	Link_WriteSampleReg( PStart, td.b );

	WriteEEpromSampleCorr();
}


//-----------------------------------------------
//��������: ���ߵ���ϵ������
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
static void Ib_K(BYTE *Buf)
{
	TFourByteUnion K1;
	TFourByteUnion td;	
	
	//��׼���ߵ���
	memcpy(K1.b,Buf+8,4);
	
	Link_ReadSampleReg(IBRMS,td.b);
	
	SampleCorr.I2Gain.d = td.d*1000/K1.d;		//IBϵ��
	
	WriteEEpromSampleCorr();
}


//-----------------------------------------------
//��������: ��Чֵ����У��(100%Ib,1.0)
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA HF���� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          AdjType[in]     BASE_ADJ : �ֵ�������У��
// 							ACCURATE_ADJ �����������У��
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void Gain_Adjust( BYTE AdjType, BYTE *Buf)
{
	TFourByteUnion td;
	float Err_Data;
	TFourByteUnion regValue;

	if (AdjType == BASE_ADJ)
	{
		UIP_K(Buf);		//��ѹ����ϵ��У��

		//У��ǰ�ȰѼĴ�����������
		ResetSampleReg( GPA );
		ResetSampleReg( GQA );
		ResetSampleReg( GSA );

		CLEAR_WATCH_DOG;
		api_Delayms( 800 );	
		CLEAR_WATCH_DOG;

		Err_Data = Get_Err_Data(Buf, 3);//�������

	}
	else
	{
		Err_Data = GetProtolErrData(Buf);
	}
	
	if(fabs(Err_Data) > ((float)Def_MaxError/100)) //ȡ����ֵ ��������30% ������У�� ��ֹ���ť �Ų�Ӳ������
	{
		return;
	}
	
	Err_Data = -Err_Data/(1+Err_Data);
		
	if(Err_Data >= 0) 
	{
		td.d = (DWORD)(32768*Err_Data);
	}
	else 
	{
		td.d = (DWORD)(65536 + 32768*Err_Data);
	}

	Link_ReadSampleReg(GPA, regValue.b);

	td.d += regValue.d;	

	SampleAdjustReg.GP1.w = td.w[0];
	Link_WriteSampleReg(GPA,td.b);
	
	SampleAdjustReg.GQ1.w = td.w[0];
	Link_WriteSampleReg(GQA,td.b);
	
	SampleAdjustReg.GS1.w = td.w[0];
	Link_WriteSampleReg(GSA,td.b);
}


//-----------------------------------------------
//��������: ��λУ��(100%Ib,0.5L)
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA HF���� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//         AdjType[in]     	BASE_ADJ : �ֵ�������У��
// 							ACCURATE_ADJ �����������У��
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void PH_Adjust( BYTE AdjType, BYTE *Buf)
{
	BYTE i, VolCorrectLevel;	//��ѹ�����ȼ�
	TFourByteUnion td;
	TFourByteUnion regValue;
	float Err_Data;
	
	if (AdjType == BASE_ADJ)
	{
		ResetSampleReg( GPhs1 );

		CLEAR_WATCH_DOG;	
		api_Delayms( 800 );
		CLEAR_WATCH_DOG;

		Err_Data = Get_Err_Data(Buf, 3);//�������
	}
	else
	{
		Err_Data = GetProtolErrData(Buf);
	}
	
	if(fabs(Err_Data) > ((float)Def_MaxError/100)) //ȡ����ֵ ��������30% ������У�� ��ֹ���ť �Ų�Ӳ������
	{
		return;
	}
	
	if(Err_Data >= 0) 
	{
		//65536 - ((float)Err_Data*32768/1.732)
		td.d = (DWORD)(65536 - Err_Data * 18919.2);			
	}
	else 
	{
		//((-(float)Err_Data)*32768/1.732)
		td.d = (DWORD)((-Err_Data) * 18919.2);					
	} 

	// ��ȡ��ԭ���ļĴ�������ֵ
	Link_ReadSampleReg(GPhs1, regValue.b);

	td.d += regValue.d;	

	VolCorrectLevel = 0; //����������

	if( SelVolCorrectConst == YES )
	{
		VolCorrectLevel = GetVolCorrectLevel( 0 );
		
		if( (VolCorrectLevel > MAX_VOLCORR_OFFSET) && (VolCorrectLevel <= MAX_VOLCORR_STEP) )
		{
			SampleVolCorr.RGPhs1[VolCorrectLevel - MAX_VOLCORR_OFFSET - 1].w = td.w[0];
			Link_WriteSampleReg( GPhs1, SampleVolCorr.RGPhs1[VolCorrectLevel - MAX_VOLCORR_OFFSET - 1].b );
		}
	}
	
	if( VolCorrectLevel == 0 )
	{
		SampleAdjustReg.RGPhs1.w = td.w[0];
		Link_WriteSampleReg(GPhs1,td.b);
		
		//������У�� �ѵ�ѹӰ����ϵ��ȫ����Ϊ������У��ֵ ��ֹĳ����δУ���ִ����
		if( SelVolCorrectConst == YES )
		{
			for( i = 0; i < MAX_VOLCORR_RGPHS; i++ )
			{
				SampleVolCorr.RGPhs1[i].w = SampleAdjustReg.RGPhs1.w;
			}
		}
	}
}


//-----------------------------------------------
//��������: �й�����ƫ������(5%Ib,1.0)
//
//����:		Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA HF���� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//        AdjType[in]     	BASE_ADJ : �ֵ�������У��
// 							ACCURATE_ADJ �����������У��         
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void Offset_Adjust( BYTE AdjType, BYTE *Buf)
{
	BYTE i;
	BYTE VolCorrectLevel;	//��ѹ�����ȼ�
	TFourByteUnion P1;
	TFourByteUnion td;
	TFourByteUnion OFFSET[2];
	float Err_Data;
	
	if (AdjType == BASE_ADJ)
	{
		ResetSampleReg( P1OFFSET );
		ResetSampleReg( P1OFFSETL );

		CLEAR_WATCH_DOG;
		api_Delayms( 800 );	
		CLEAR_WATCH_DOG;

		//����	
		memcpy(P1.b,Buf+12,4);

		Err_Data = Get_Err_Data(Buf, 20);//�������		
	}
	else
	{
		P1.d = RemoteValue.P.d;
		Err_Data = GetProtolErrData(Buf);//�������	
	}
	
	if(fabs(Err_Data) > ((float)Def_MaxError/100)) //ȡ����ֵ ��������30% ������У�� ��ֹ���ť �Ų�Ӳ������
	{
		return;
	}
	
	Link_ReadSampleReg(HFConst,td.b);
	Link_ReadSampleReg(P1OFFSET, OFFSET[0].b);
	Link_ReadSampleReg(P1OFFSETL, OFFSET[1].b);

	OFFSET[0].d = (OFFSET[0].d<<8)+OFFSET[1].d;

	if(Err_Data >= 0)
	{
		td.d = (DWORD)(0x10000L - (DWORD)((float)P1.d*3.81775*api_GetActiveConstant()*td.d*Err_Data/100000));
	}
	else 
	{
		td.d = (DWORD)((float)P1.d*3.81775*api_GetActiveConstant()*td.d*(-Err_Data)/100000);
	}
	
	td.d += OFFSET[0].d;

	VolCorrectLevel = 0;
	//��ѹӰ��������
	if( SelVolCorrectConst == YES )
	{
		VolCorrectLevel = GetVolCorrectLevel( 0 );
		if( (VolCorrectLevel > 0) && (VolCorrectLevel <= MAX_VOLCORR_OFFSET) )
		{
			SampleVolCorr.RP1OFFSET[VolCorrectLevel - 1].b[0] = td.b[1];
			SampleVolCorr.RP1OFFSET[VolCorrectLevel - 1].b[1] = 0x00;
			Link_WriteSampleReg( P1OFFSET, SampleVolCorr.RP1OFFSET[VolCorrectLevel - 1].b );
			
			SampleVolCorr.RP1OFFSETL[VolCorrectLevel - 1].b[0] = td.b[0];
			SampleVolCorr.RP1OFFSETL[VolCorrectLevel - 1].b[1] = 0x00;
			Link_WriteSampleReg( P1OFFSETL, SampleVolCorr.RP1OFFSETL[VolCorrectLevel - 1].b );
		}
	}
	//����У׼���� �ر�ע�� 5% 1.0����У������ڵ�ѹӰ��������֮ǰ
	if( VolCorrectLevel == 0 )
	{
		SampleAdjustReg.RP1OFFSET.b[0] = td.b[1];
		SampleAdjustReg.RP1OFFSET.b[1] = 0x00;
		Link_WriteSampleReg(P1OFFSET,SampleAdjustReg.RP1OFFSET.b);
		
		SampleAdjustReg.RP1OFFSETL.b[0] = td.b[0];
		SampleAdjustReg.RP1OFFSETL.b[1] = 0x00;
		Link_WriteSampleReg(P1OFFSETL,SampleAdjustReg.RP1OFFSETL.b);
		
		//5% 1.0������У�� �ѵ�ѹӰ����ϵ��ȫ����Ϊ������У��ֵ ��ֹĳ����δУ���ִ����
		if( SelVolCorrectConst == YES )
		{
			for( i = 0; i < MAX_VOLCORR_OFFSET; i++ )
			{
				SampleVolCorr.RP1OFFSET[i].w = SampleAdjustReg.RP1OFFSET.w;
				SampleVolCorr.RP1OFFSETL[i].w = SampleAdjustReg.RP1OFFSETL.w;
			}
		}
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
	TFourByteUnion Temp_B32;
	TFourByteUnion td;
	WORD i;
	BYTE Addr;
	
	if(Ichannel == 1) 
	{
		Addr = IARMS;
		ResetSampleReg( IARMSOS );
	}
	else 
	{
		Addr = IBRMS;
		ResetSampleReg( IBRMSOS );
	}
	
	CLEAR_WATCH_DOG;
	api_Delayms( 800 );
	CLEAR_WATCH_DOG;
	
	Temp_B32.d = 0;
	td.d = 0;
	for(i=0; i<6; i++)
	{		
		Link_ReadSampleReg(Addr,td.b);
	
		Temp_B32.d += td.d;
		
		CLEAR_WATCH_DOG;
		
		api_Delayms(300);	//��ʱ300ms
	}
	
	CLEAR_WATCH_DOG;
	
	Temp_B32.d = Temp_B32.d/6;

	td.d = (DWORD)((float)Temp_B32.d*Temp_B32.d+16384)/32768;//��������

	if(Ichannel == 1) 
	{
		SampleAdjustReg.RI1RMSOFFSET.w = td.w[0];
		Link_WriteSampleReg(IARMSOS,td.b);
	}	
	else 
	{
    	SampleAdjustReg.RI2RMSOFFSET.w = td.w[0];
    	Link_WriteSampleReg(IBRMSOS,td.b);
	}
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
WORD api_ProcSampleAdjust(BYTE AdjType, BYTE *Buf)
{	
	DWORD TmpOAD;
	
	if( (Buf[0]!=0xaa) || (Buf[3]!=0xaa) || (Buf[16]!=0xaa) )
	{
		return FALSE;
	}
	
	if(api_CheckEEPRomAddr(0xFF) == FALSE)//���E2��ַ
	{
		return FALSE;
	}
	
	//��ʾ��ʼУ�� ������ʾ
	api_DisplayAdjust();
	
	SampleSpecCmdOp(0xa6);	//��д����

	switch( AdjType )
	{
		case 0xff://��ʼ��						
			InitSamplePara( Buf+1 );
			break;
		
		case 0x00://����У��(L)
			IxRMSOS_Adjust(1);		//����ͨ��A��Чֵ����			
			IxRMSOS_Adjust(2);		//����ͨ��B��Чֵ����
			break;
			
		case 0x01://100%Ib��1.0(L)
			Gain_Adjust( BASE_ADJ,Buf);	//��Чֵ����У��
			break;
			
		case 0x51://100%Ib��0.5L(L)
			PH_Adjust( BASE_ADJ,Buf);	//��λУ��
			break;
			
		case 0x02://5%Ib��1.0(L)
			Offset_Adjust( BASE_ADJ, Buf);//�й�����ƫ������
			break;
		
		case 0x81://100%Ib��1.0(N)
			Ib_K(Buf);		//���ߵ���ϵ��
			TmpOAD = 0X00040120;
			api_WriteLcdEchoInfo( 1, (BYTE*)&TmpOAD, 0, 60 );
			break;
			
		default: 
			break;
	}
	SampleSpecCmdOp(0xbc);	//��д����
	
	api_WriteFreeEvent(EVENT_ADJUST_METER, (WORD)AdjType);

	if( AdjType != 0xff )//�ڼ�����ʼ�����Ѿ�д��EEPROM �����ظ�
	{
		WriteEEpromSampleAdjustReg();//У��ϵ�����ݴ���
		
		//�����ѹӰ����ϵ��
		if( (SelVolCorrectConst == YES)&&((AdjType == 0x02) || (AdjType == 0x51)) )
		{
			WriteEEpromSampleVolCorr();
		}
	}
	// �رձ���
	CLOSE_BACKLIGHT;

	return TRUE;
	
}

//---------------------------------------------------------------
//��������: ����� ��B000��B001��B002��
//
//����: 	AdjType[in] : 	0xB000:	��������� 1.0(L)
//							0xB001:	�������� 0.5L(L)
//							0xB002:	С�������� 1.0(L)	    
// 			Buf[in]			���ֽڣ����ں󣬵���ǰ
// 
//����ֵ:  TRUE/FALSE
//
//��ע:   30%��Χ
//---------------------------------------------------------------
BOOL SetCorrectCorrCarefully( WORD AdjType, BYTE *Buf )
{
	WORD Offset;
	float Err_Data;

	if(api_CheckEEPRomAddr(0xFF) == FALSE)//���E2��ַ
	{
		return FALSE;
	}

	//��ʾ��ʼУ�� ������ʾ
	api_DisplayAdjust();

	SampleSpecCmdOp(0xa6);	//��д����

	switch (AdjType)
	{
		case 0xB000://100%Ib��1.0(L)--��Чֵ����У��
			Gain_Adjust( ACCURATE_ADJ, Buf);
			break;

		case 0xB001://100%Ib��0.5L(L)--��λУ��
			PH_Adjust( ACCURATE_ADJ, Buf);
			break;

		case 0xB002://5%Ib��1.0(L) �й�����ƫ������
			Offset_Adjust(ACCURATE_ADJ, Buf);
			break;

		default:
			break;
	}

	SampleSpecCmdOp(0xbc);	//��д����

	api_WriteFreeEvent(EVENT_ADJUST_METER, AdjType);

	WriteEEpromSampleAdjustReg();//У��ϵ�����ݴ���
	//�����ѹӰ����ϵ��
	if( (SelVolCorrectConst == YES)&&((AdjType == 0xB001) || (AdjType == 0xB002)) )
	{
		WriteEEpromSampleVolCorr();
	}

	return TRUE;
}

//--------------------------------------------------
//��������: ���������㴦��Ӧ��д��Ĵ�������ֵ 
// 
//����:      Buf[in]: ������ ���ֵ 2���ֽ�
//
//         
//����ֵ:   TRUE/FALSE
//         
//��ע:  
//--------------------------------------------------
static BYTE  SetSampleVolErrAdjust( BYTE  *Buf)
{
	BYTE VolCorrectLevel;	//��ѹ�����ȼ�

	if( SelVolCorrectConst != YES )
	{
		return FALSE;
	}

	VolCorrectLevel = GetVolCorrectLevel( 0 );

	// У��ƫ���� ��ѹ�ȼ�1-5
	if( (VolCorrectLevel > 0) && (VolCorrectLevel <= MAX_VOLCORR_OFFSET) )
	{
		SampleSpecCmdOp(0xa6);	//��д����
		Offset_Adjust(ACCURATE_ADJ, Buf);
		SampleSpecCmdOp(0xbc);	//��д����
	}
	// У����λ  ��ѹ�ȼ�6-11
	else if( (VolCorrectLevel > MAX_VOLCORR_OFFSET) && (VolCorrectLevel <= MAX_VOLCORR_STEP) )
	{
		SampleSpecCmdOp(0xa6);	//��д����
		PH_Adjust( ACCURATE_ADJ, Buf);
		SampleSpecCmdOp(0xbc);	//��д����
	}
	else
	{
		return FALSE;
	}

	WriteEEpromSampleVolCorr();
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
BOOL ReadSampleReg(BYTE *InBuf,BYTE *OutBuf)
{
	BYTE result, i;
	BYTE Buf[4];

	result = Link_ReadSampleReg(InBuf[0],Buf);
	if(result == FALSE) 
	{
	   return FALSE;
	}
	//������ǰ��λ����С��ģʽ��Ϊ��
	Buf[3] = InBuf[0];
	lib_InverseData( Buf, 4 );
	memcpy(OutBuf, Buf, 4);

	return TRUE;
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
BOOL SetSampleReg(BYTE WriteEepromFlag,BYTE Addr,BYTE *Buf)
{
	BYTE i, Result;

	//У���ʼ������
	if(Addr == 0xff)	
	{
		if( WriteEepromFlag == 0xFF )
		{
			InitSamplePara( Buf );
		}
		else
		{
			return FALSE;
		}
	}
	else//д�Ĵ�������
	{				
		for(i=0; i<sizeof(SampleAdjustAddr); i++)
		{
			if( Addr == SampleAdjustAddr[i] ) 
			{
				Result = 0;
				
				if( (SelVolCorrectConst == YES) && ((Addr == P1OFFSET) || (Addr == P1OFFSETL) || (Addr == GPhs1)) )
				{
					Result = GetVolCorrectLevel( 0 );//��ȡ��ѹӰ�����ȼ�
					
					//С�������ѹ����
					if( (Result > 0) && (Result <= MAX_VOLCORR_OFFSET) )
					{
						if( Addr == P1OFFSET )
						{
							memcpy( SampleVolCorr.RP1OFFSET[Result - 1].b, Buf, 2 );
						}
						else if( Addr == P1OFFSETL )
						{
							memcpy( SampleVolCorr.RP1OFFSETL[Result - 1].b, Buf, 2 );
						}
						else
						{
							return FALSE;
						}
					}
					//��������ѹ����
					else if( (Result > MAX_VOLCORR_OFFSET) && (Result <= MAX_VOLCORR_STEP) )
					{
						if( Addr == GPhs1 )
						{
							memcpy( SampleVolCorr.RGPhs1[Result - MAX_VOLCORR_OFFSET - 1].b, Buf, 2 );
						}
						else
						{
							return FALSE;
						}
					}
					
					//���ݴ�EEPROM
					if( Result > 0 )
					{
						if( WriteEepromFlag == 0xFF )
						{
							WriteEEpromSampleVolCorr();
						}
						else
						{
							SampleVolCorr.CRC32 = lib_CheckCRC32( (BYTE *)&SampleVolCorr, sizeof(TSampleVolCorr) - sizeof(DWORD) );
						}
					}
				}
				if( Result == 0 )
				{
					memcpy( (void*)&(SampleAdjustReg.GP1.b[i*2]), Buf, 2 );
				}
				
				if( WriteEepromFlag == 0xFF )
				{
					if( Result == 0 )
					{
						WriteEEpromSampleAdjustReg();
					}
				}
				else//��дEEPROM ���Ǹ���RAMУ�� �ϵ��Զ��ָ�
				{
					SampleAdjustReg.CRC32 = lib_CheckCRC32( (BYTE *)&SampleAdjustReg, sizeof(TSampleAdjustReg) - sizeof(DWORD) );
				}
				break;
			}
		}	

		for( i = 0; i < sizeof(SampleCtrlAddr); i++ )
    	{    		
    		if( Addr == SampleCtrlAddr[i])
    		{
				memcpy( (void*)&(SampleCtrlReg.EMUCFG.b[i*2]), Buf, 2 );
				
				if( WriteEepromFlag == 0xFF )
				{
					WriteEEpromSampleCtrlReg();
				}
				else//��дEEPROM ���Ǹ���RAMУ�� �ϵ��Զ��ָ�
				{
					SampleCtrlReg.CRC32 = lib_CheckCRC32( (BYTE *)&SampleCtrlReg, sizeof(TSampleCtrlReg) - sizeof(DWORD) );
				}
    		}
    	}
	}
	
	if( Addr < 0x50 ) 
	{
		SampleSpecCmdOp(0xbc);
	}
	else 
	{
		SampleSpecCmdOp(0xa6);
	}
	//����50H�Ժ�Ĵ�����д���������浽EE
	Link_WriteSampleReg(Addr,Buf);
	SampleSpecCmdOp(0x00);
	
	api_WriteFreeEvent( EVENT_WRITE_REG, (WORD)((WriteEepromFlag << 8)|Addr) );
	
	// Ӧ�ü�¼�澯�¼�
	api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+Addr);
	
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
	BYTE i;
	WORD Offset;
	
	if( SelVolCorrectConst != YES )
	{
		return FALSE;
	}
	
	//ȫ��ϵ��
	if( Addr == 0xA0FF )
	{
		for( i = 0; i < MAX_VOLCORR_OFFSET; i++ )
		{
			SampleVolCorr.RP1OFFSETL[i].b[0] = Buf[2 * i];
			SampleVolCorr.RP1OFFSET[i].b[0] = Buf[2 * i + 1];
		}
		memcpy( SampleVolCorr.RGPhs1[0].b, &Buf[2 * MAX_VOLCORR_OFFSET], sizeof(SampleVolCorr.RGPhs1) );
		WriteEEpromSampleVolCorr(); 
	}
	//������ѹӰ����  A000 -- A00A
	else if (Addr <= 0xA00A)
	{
		if( SetSampleVolErrAdjust(Buf) != TRUE)
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
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
	BYTE i;
	WORD Offset;
	
	//ȫ��ϵ��
	if( Addr == 0xA0FF )
	{
		for( i = 0; i < MAX_VOLCORR_OFFSET; i++ )
		{
			Buf[2 * i] = SampleVolCorr.RP1OFFSETL[i].b[0];
			Buf[2 * i + 1] = SampleVolCorr.RP1OFFSET[i].b[0];
		}
		
		memcpy( &Buf[2 * MAX_VOLCORR_OFFSET], SampleVolCorr.RGPhs1, sizeof(SampleVolCorr.RGPhs1) );
	}
	else
	{
		return FALSE;
	}

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
	BYTE Result;
	
	switch( Addr )
	{
		case 0x9000://��ѹϵ��
		case 0x90FF://��ѹϵ�����ݿ�
			memcpy( SampleCorr.UGain.b, Buf, 4 );
			Result = TRUE;
			break;
		case 0x9100://���ߵ���ϵ��
			memcpy( SampleCorr.I1Gain.b, Buf, 4 );
			Result = TRUE;
			break;
		case 0x9103://���ߵ���ϵ��
			memcpy( SampleCorr.I2Gain.b, Buf, 4 );
			Result = TRUE;
			break;
		case 0x91FF://����ϵ�����ݿ�
			memcpy( SampleCorr.I1Gain.b, Buf, 8 );
			Result = TRUE;
			break;
		case 0x9200://����ϵ��
		case 0x92FF://����ϵ�����ݿ�
			memcpy( SampleCorr.P1Gain.b, Buf, 4 );
			Result = TRUE;
			break;
		case 0x9F00://��ѹ��������˲ʱ�����ݿ�
			memcpy( SampleCorr.UGain.b, Buf, 16 );
			Result = TRUE;
			break;
		default:
			Result = FALSE;
	}
	
	if( Result == TRUE )
	{
		WriteEEpromSampleCorr();
	}
	return Result;
}
//-----------------------------------------------
//��������: ͨѶ��˲ʱ��ϵ��
//
//����:		Addr[in]�Ĵ�����ַ
//			Buf[in]	Ҫд������� ���ֽڣ����ں󣬵���ǰ
//����ֵ: 	TRUE/FALSE
//		   
//��ע:   
//-----------------------------------------------
BOOL ReadSampleCorr( WORD Addr, BYTE *Buf )
{
	BYTE Result;
	
	switch( Addr )
	{
		case 0x9000://��ѹϵ��
		case 0x90FF://��ѹϵ�����ݿ�
			memcpy( Buf, SampleCorr.UGain.b, 4 );
			Result = TRUE;
			break;
		case 0x9100://���ߵ���ϵ��
			memcpy( Buf, SampleCorr.I1Gain.b, 4 );
			Result = TRUE;
			break;
		case 0x9103://���ߵ���ϵ��
			memcpy( Buf, SampleCorr.I2Gain.b, 4 );
			Result = TRUE;
			break;
		case 0x91FF://����ϵ�����ݿ�
			memcpy(Buf, SampleCorr.I1Gain.b, 8);
			Result = TRUE;
			break;
		case 0x9200://����ϵ��
		case 0x92FF://����ϵ�����ݿ�
			memcpy( Buf, SampleCorr.P1Gain.b, 4 );
			Result = TRUE;
			break;
		case 0x9F00://��ѹ��������˲ʱ�����ݿ�
			memcpy( Buf, SampleCorr.UGain.b, 16 );
			Result = TRUE;
			break;
		default:
			Result = FALSE;
	}
	
	if( (Result == TRUE) && ((Addr&0x00FF) == 0x00FF) )//���ݿ����FF
	{
		if( Addr == 0x91FF )
		{
			memset( Buf + 8, 0xFF, 4 );
		}
		else
		{
			memset( Buf + 4, 0xFF, 8 );
		}
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
//
//����ֵ: 	TRUE/FALSE
//
//��ע:
//-----------------------------------------------
BOOL api_ProcSampleCorr(BYTE Operation, WORD Addr, BYTE Len, BYTE *Buf)
{
	WORD i,Result;
	
	if( Operation == WRITE )
	{
		//���ò���оƬ�Ĵ���
		if( Addr < 0x9000 )
		{
			if( Addr < 0x2000 )
			{
				//ֻ���浽ram������оƬ�������浽eeprom�������ϵ�ʱ�ָ�ԭֵ
				Result = SetSampleReg( 0x00, Addr, Buf );
			}
			else
			{
				//���浽ram������оƬ��eeprom
				Result = SetSampleReg( 0xFF, Addr, Buf );
				
				api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+Addr);
			}
			
			if( Result == FALSE )
			{
				return FALSE;
			}
		}
		
		//����˲ʱ��ϵ��
		else if( Addr < 0xA000 )
		{
			Result = SetSampleCorr( Addr, Buf );
			if( Result == FALSE )
			{
				return FALSE;
			}
		}
		
		//���õ�ѹӰ����ϵ��
		else if( ( Addr >= 0xA000 ) && ( Addr <= 0xA0FF ) )
		{
			Result = SetSampleVolCorrectCorr( Addr, Buf );
			if( Result == FALSE )
			{
				return FALSE;
			}
		}
		// �������� 
		else if( ( Addr >= 0xB000 ) && ( Addr <= 0xB0FF ) )
		{
			Result = SetCorrectCorrCarefully( Addr, Buf );
			if( Result == FALSE )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		//��ȡ����оƬ�Ĵ���
		if( Addr < 0x9000 )
		{
			//���ݿ鷽ʽ����
			if(Addr == 0x3fff)
			{
				if( Len < (sizeof(TSampleAdjustReg)-sizeof(DWORD))/sizeof(TTwoByteUnion)*3 )
				{
					return FALSE;
				}
				
				for(i=0;i<(sizeof(TSampleAdjustReg)-sizeof(DWORD))/sizeof(TTwoByteUnion);i++)
				{
					*Buf = SampleAdjustAddr[i];
					Buf += 1;
					memcpy(Buf,(BYTE*)&SampleAdjustReg+i*sizeof(TTwoByteUnion),sizeof(TTwoByteUnion));
					Buf += 2;
				}
			}
			else
			{
				Result = Link_ReadSampleReg( Addr, Buf );
				if( Result == FALSE )
				{
					return FALSE;
				}
			}
		}
		
		//��ȡ˲ʱ��ϵ��
		else if( Addr < 0xA000 )
		{
			if(Addr == 0x9fff)
			{
				if( Len < (sizeof(TSampleCorr)-sizeof(DWORD))/sizeof(TFourByteUnion)*5 )
				{
					return FALSE;
				}
				
				for(i=0;i<(sizeof(TSampleCorr)-sizeof(DWORD))/sizeof(TFourByteUnion);i++)
				{
					//��ѹϵ��
					*Buf = 0x01;
					Buf += 1;
					memcpy(Buf,SampleCorr.UGain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
					//���ߵ���ϵ��
					*Buf = 0x11;
					Buf += 1;
					memcpy(Buf,SampleCorr.I1Gain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
					//���ߵ���ϵ��
					*Buf = 0x14;
					Buf += 1;
					memcpy(Buf,SampleCorr.I2Gain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
					//���߹���ϵ��
					*Buf = 0x21;
					Buf += 1;
					memcpy(Buf,SampleCorr.P1Gain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
					//���߹���ϵ�� ��ʱ����
					*Buf = 0x24;
					Buf += 1;
					memcpy(Buf,SampleCorr.P2Gain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
				}
			}
			else
			{
				Result = ReadSampleCorr( Addr, Buf );
				if( Result == FALSE )
				{
					return FALSE;
				}
			}
		}
		//����ѹӰ����ϵ��
		else if( Addr == 0xA0FF )
		{
			Result = ReadSampleVolCorrectCorr( Addr, Buf );
			if( Result == FALSE )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
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
}

#endif// #if( SAMPLE_CHIP == CHIP_HT7017 )
