//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.7.30
//描述		采样芯片HT7017校表文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7053D.h"

#if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
TSampleUIK	TF415UIK;
TTimeTem	TF415TimeTem;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------


//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 校表系数保存到EEPROM，并置刷新标志
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void WriteEEpromSampleAdjustReg(void)			
{
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg),sizeof(TSampleAdjustReg),(BYTE*)&SampleAdjustReg);
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
}

//-----------------------------------------------
//函数功能: 控制系数保存到EEPROM，并置刷新标志
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void WriteEEpromSampleCtrlReg(void)			
{
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg),sizeof(TSampleCtrlReg),(BYTE*)&SampleCtrlReg);
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
}

//-----------------------------------------------
//函数功能: 瞬时量系数保存到EEPROM
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void WriteEEpromSampleCorr( void )
{
	api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleCorr ), sizeof(TSampleCorr), SampleCorr.UGain.b );
	api_ClrSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
}
//---------------------------------------------------------------
//函数功能: 电压影响量系数保存到EEPROM
//
//参数:    无
//                   
//返回值:  无
//
//备注:   
//---------------------------------------------------------------
static void WriteEEpromSampleVolCorr( void )
{
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleVolCorr),sizeof(TSampleVolCorr),(BYTE*)&SampleVolCorr);
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
}
//-----------------------------------------------
//函数功能: 复位对应采样寄存器
//
//参数:		Reg_Addr[in]	寄存器地址
//
//返回值: 	无
//
//备注:
//-----------------------------------------------
void ResetSampleReg(BYTE Reg_Addr)
{
    BYTE Buf[2];

	Buf[0] = 0;
	Buf[1] = 0;
	Link_WriteSampleReg( Reg_Addr, Buf );
}

//-----------------------------------------------
//函数功能: 根据读出的采样芯片功率值与理论功率值计算误差
//
//参数:		Buf[in]			高字节在后，低字节在前	AA HF常数 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//			CF_f[in]	计算误差的平均次数
//          
//返回值: 	误差，浮点数
//		   
//备注:   
//-----------------------------------------------
static float Get_Err_Data(BYTE *Buf, BYTE CF_f)
{
	TFourByteUnion K1, K2, K3;
	TFourByteUnion td;
	float Err_Data;
	WORD Result;
	BYTE i;	
	
	//功率	
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
	
	K1.d = (DWORD)((double)td.d*1000000.0/SampleCorr.P1Gain.d);//计算出实际功率值
	
	Err_Data = ((float)K1.d - K3.d)/K3.d;	
	return(Err_Data);
}

//--------------------------------------------------
//功能描述:  对报文接收的误差数据进行处理
//         
//参数:      Buf[in]要写入的数据 两字节，高在后，低在前
//		     
//         
//返回值:    误差，浮点数
//         
//备注:  该函数适用于误差校表法，buf指向误差数据  数据长度为2个字节
//--------------------------------------------------
static float  GetProtolErrData( BYTE  *Buf)
{
	float Err_Data;
	TTwoByteUnion K1;

	K1.w = 0;
	memcpy(K1.b, Buf, 2);
	
	// 上位机多乘10000
	Err_Data = (float)(K1.sw / 1000000.0);

	return(Err_Data);
}

//-----------------------------------------------
//函数功能: 校表初始化
//
//参数:		InBuf[in]	传入的HFConst值
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void InitSamplePara( BYTE *InBuf )
{	
    TFourByteUnion ComData;
	WORD ANAEN = 0, ANACON = 0;

	//初始化计量参数
	memcpy(SampleAdjustReg.GP1.b, (BYTE*)&SampleAdjustDefData, sizeof(TSampleAdjustReg)-sizeof(DWORD));
	if( (InBuf[0] != 0x00) || (InBuf[1] != 0x00) )
	{
		SampleAdjustReg.RHFConst.b[0] = InBuf[0];	//HFConst的值
		SampleAdjustReg.RHFConst.b[1] = InBuf[1];
	}
	WriteEEpromSampleAdjustReg();//校表系数备份处理
	
	//送测时开启电压影响量, 不初始化这两个寄存器, 防止做自热实验修改寄存器的值后又不小心初始化
	//供货时关闭电压影响量, 初始化寄存器
	if( SelVolCorrectConst == YES )
	{
		ANAEN = SampleCtrlReg.ANAEN.w;
		ANACON = SampleCtrlReg.ANACON.w;
	}
    Link_ReadSampleReg(DeviceID,ComData.b);
    if( ComData.d == 0x705304 )//5000:1芯片
    {
        memcpy( (BYTE *)&SampleCtrlReg, (BYTE *)&SampleCtrlDefData, sizeof(TSampleCtrlReg)-sizeof(DWORD) );
    }
    else//8000:1芯片
    {
        memcpy( (BYTE *)&SampleCtrlReg, (BYTE *)&SampleCtrlDefData8000, sizeof(TSampleCtrlReg)-sizeof(DWORD) );
    }
	if( SelVolCorrectConst == YES )
	{
		SampleCtrlReg.ANAEN.w = ANAEN;
		SampleCtrlReg.ANACON.w = ANACON;
	}
    
    WriteEEpromSampleCtrlReg();//控制参数备份处理
    
	//初始化瞬时量系数
	SampleCorr.UGain.d = METER_Uk+Uoffset;
	SampleCorr.I1Gain.d = METER_I1k;
	SampleCorr.I2Gain.d = METER_I2k;
	SampleCorr.P1Gain.d = METER_Pk;
	WriteEEpromSampleCorr();
	
	//初始化电压影响量系数
	if( SelVolCorrectConst == YES )
	{
		memset( &SampleVolCorr, 0x00, sizeof(TSampleVolCorr) );
		WriteEEpromSampleVolCorr();
	}
	
	//初始化自热系数
	
	//重新初始化计量芯片
	api_PowerUpSample();
}

//-----------------------------------------------
//函数功能: 电压、电流、功率系数计算
//
//参数:		Buf[in]			高字节在后，低字节在前	AA HF常数 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA       
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void UIP_K(BYTE *Buf)
{
	BYTE i;
	WORD Result;
	TFourByteUnion td;	
	TFourByteUnion RegData;
	TFourByteUnion K[3];
	float tf;
	
	//标准电压
	memcpy(K[0].b,Buf+4,4);
	//标准电流
	memcpy(K[1].b,Buf+8,4);

	Link_ReadSampleReg(URMS,RegData.b);
	SampleCorr.UGain.d = RegData.d*1000/K[0].d;			//电压系数
	
	Link_ReadSampleReg(IARMS,RegData.b);
	SampleCorr.I1Gain.d = RegData.d*1000/K[1].d;			//电流A系数
        
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
	SampleCorr.P1Gain.d = (DWORD)(tf*td.d);	//计算功率系数

	td.d = wCaliVoltageConst * wMeterBasicCurrentConst / 10; 		//计算实际功率 保留6位小数
	td.d = (DWORD)((double)td.d * SampleCorr.P1Gain.d / 1000000.0); //计算实际功率值对应的寄存器值
	td.d = td.d * 3 / 1000;                 //防潜动阈值 = 0.3%*P
	SampleAdjustReg.RPStart.w = td.w[0];
	Link_WriteSampleReg( PStart, td.b );

	WriteEEpromSampleCorr();
}


//-----------------------------------------------
//函数功能: 零线电流系数计算
//
//参数:		Buf[in]			高字节在后，低字节在前	AA HF常数 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void Ib_K(BYTE *Buf)
{
	TFourByteUnion K1;
	TFourByteUnion td;	
	
	//标准零线电流
	memcpy(K1.b,Buf+8,4);
	
	Link_ReadSampleReg(IBRMS,td.b);
	
	SampleCorr.I2Gain.d = td.d*1000/K1.d;		//IB系数
	
	WriteEEpromSampleCorr();
}


//-----------------------------------------------
//函数功能: 有效值增益校正(100%Ib,1.0)
//
//参数:		Buf[in]			高字节在后，低字节在前	AA HF常数 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          AdjType[in]     BASE_ADJ : 粗调（功率校表）
// 							ACCURATE_ADJ ：精调（误差校表）
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void Gain_Adjust( BYTE AdjType, BYTE *Buf)
{
	TFourByteUnion td;
	float Err_Data;
	TFourByteUnion regValue;

	if (AdjType == BASE_ADJ)
	{
		UIP_K(Buf);		//电压电流系数校正

		//校表前先把寄存器数据清零
		ResetSampleReg( GPA );
		ResetSampleReg( GQA );
		ResetSampleReg( GSA );

		CLEAR_WATCH_DOG;
		api_Delayms( 800 );	
		CLEAR_WATCH_DOG;

		Err_Data = Get_Err_Data(Buf, 3);//计算误差

	}
	else
	{
		Err_Data = GetProtolErrData(Buf);
	}
	
	if(fabs(Err_Data) > ((float)Def_MaxError/100)) //取绝对值 当误差大于30% 不允许校正 防止点错按钮 排查硬件故障
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
//函数功能: 相位校正(100%Ib,0.5L)
//
//参数:		Buf[in]			高字节在后，低字节在前	AA HF常数 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//         AdjType[in]     	BASE_ADJ : 粗调（功率校表）
// 							ACCURATE_ADJ ：精调（误差校表）
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void PH_Adjust( BYTE AdjType, BYTE *Buf)
{
	BYTE i, VolCorrectLevel;	//电压补偿等级
	TFourByteUnion td;
	TFourByteUnion regValue;
	float Err_Data;
	
	if (AdjType == BASE_ADJ)
	{
		ResetSampleReg( GPhs1 );

		CLEAR_WATCH_DOG;	
		api_Delayms( 800 );
		CLEAR_WATCH_DOG;

		Err_Data = Get_Err_Data(Buf, 3);//计算误差
	}
	else
	{
		Err_Data = GetProtolErrData(Buf);
	}
	
	if(fabs(Err_Data) > ((float)Def_MaxError/100)) //取绝对值 当误差大于30% 不允许校正 防止点错按钮 排查硬件故障
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

	// 读取出原来的寄存器的数值
	Link_ReadSampleReg(GPhs1, regValue.b);

	td.d += regValue.d;	

	VolCorrectLevel = 0; //必须先清零

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
		
		//基本点校正 把电压影响量系数全部改为基本点校正值 防止某个点未校出现大误差
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
//函数功能: 有功功率偏置修正(5%Ib,1.0)
//
//参数:		Buf[in]			高字节在后，低字节在前	AA HF常数 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//        AdjType[in]     	BASE_ADJ : 粗调（功率校表）
// 							ACCURATE_ADJ ：精调（误差校表）         
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void Offset_Adjust( BYTE AdjType, BYTE *Buf)
{
	BYTE i;
	BYTE VolCorrectLevel;	//电压补偿等级
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

		//功率	
		memcpy(P1.b,Buf+12,4);

		Err_Data = Get_Err_Data(Buf, 20);//计算误差		
	}
	else
	{
		P1.d = RemoteValue.P.d;
		Err_Data = GetProtolErrData(Buf);//计算误差	
	}
	
	if(fabs(Err_Data) > ((float)Def_MaxError/100)) //取绝对值 当误差大于30% 不允许校正 防止点错按钮 排查硬件故障
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
	//电压影响量补偿
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
	//基本校准补偿 特别注意 5% 1.0功率校表必须在电压影响量补偿之前
	if( VolCorrectLevel == 0 )
	{
		SampleAdjustReg.RP1OFFSET.b[0] = td.b[1];
		SampleAdjustReg.RP1OFFSET.b[1] = 0x00;
		Link_WriteSampleReg(P1OFFSET,SampleAdjustReg.RP1OFFSET.b);
		
		SampleAdjustReg.RP1OFFSETL.b[0] = td.b[0];
		SampleAdjustReg.RP1OFFSETL.b[1] = 0x00;
		Link_WriteSampleReg(P1OFFSETL,SampleAdjustReg.RP1OFFSETL.b);
		
		//5% 1.0基本点校正 把电压影响量系数全部改为基本点校正值 防止某个点未校出现大误差
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
//函数功能: 电流A/B有效值偏置修正(220V,I=0,1.0)
//
//参数:		Ichannel[in] 	1:A通道	其他值：B通道
//          
//返回值: 	无
//		   
//备注:   
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
		
		api_Delayms(300);	//延时300ms
	}
	
	CLEAR_WATCH_DOG;
	
	Temp_B32.d = Temp_B32.d/6;

	td.d = (DWORD)((float)Temp_B32.d*Temp_B32.d+16384)/32768;//四舍五入

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
//函数功能: 校表操作
//
//参数:		AdjType[in] 	0xFF：	校表初始化
//							0x00：	零漂
//							0x01:	大电流增益 1.0(L)
//							0x02:	小电流增益 1.0(L)
//							0x51:	大电流相角 0.5L(L)
//							0x52:	小电流相角 0.5L(L)
//							0x81:	大电流增益 N线 1.0(N)
//							0xD1:	大电流相角 N线 0.5L(N)
//			Buf[in]			高字节在后，低字节在前	AA HF常数 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA 	
//          
//返回值: 	TRUE/FALSE
//		   
//备注:校表规约调用     
//-----------------------------------------------
WORD api_ProcSampleAdjust(BYTE AdjType, BYTE *Buf)
{	
	DWORD TmpOAD;
	
	if( (Buf[0]!=0xaa) || (Buf[3]!=0xaa) || (Buf[16]!=0xaa) )
	{
		return FALSE;
	}
	
	if(api_CheckEEPRomAddr(0xFF) == FALSE)//检查E2地址
	{
		return FALSE;
	}
	
	//显示开始校表 插入显示
	api_DisplayAdjust();
	
	SampleSpecCmdOp(0xa6);	//开写保护

	switch( AdjType )
	{
		case 0xff://初始化						
			InitSamplePara( Buf+1 );
			break;
		
		case 0x00://空载校正(L)
			IxRMSOS_Adjust(1);		//电流通道A有效值调整			
			IxRMSOS_Adjust(2);		//电流通道B有效值调整
			break;
			
		case 0x01://100%Ib，1.0(L)
			Gain_Adjust( BASE_ADJ,Buf);	//有效值增益校正
			break;
			
		case 0x51://100%Ib，0.5L(L)
			PH_Adjust( BASE_ADJ,Buf);	//相位校正
			break;
			
		case 0x02://5%Ib，1.0(L)
			Offset_Adjust( BASE_ADJ, Buf);//有功功率偏置修正
			break;
		
		case 0x81://100%Ib，1.0(N)
			Ib_K(Buf);		//零线电流系数
			TmpOAD = 0X00040120;
			api_WriteLcdEchoInfo( 1, (BYTE*)&TmpOAD, 0, 60 );
			break;
			
		default: 
			break;
	}
	SampleSpecCmdOp(0xbc);	//关写保护
	
	api_WriteFreeEvent(EVENT_ADJUST_METER, (WORD)AdjType);

	if( AdjType != 0xff )//在计量初始化里已经写过EEPROM 无需重复
	{
		WriteEEpromSampleAdjustReg();//校表系数备份处理
		
		//保存电压影响量系数
		if( (SelVolCorrectConst == YES)&&((AdjType == 0x02) || (AdjType == 0x51)) )
		{
			WriteEEpromSampleVolCorr();
		}
	}
	// 关闭背光
	CLOSE_BACKLIGHT;

	return TRUE;
	
}

//---------------------------------------------------------------
//函数功能: 电表精调 （B000、B001、B002）
//
//参数: 	AdjType[in] : 	0xB000:	大电流增益 1.0(L)
//							0xB001:	大电流相角 0.5L(L)
//							0xB002:	小电流增益 1.0(L)	    
// 			Buf[in]			两字节，高在后，低在前
// 
//返回值:  TRUE/FALSE
//
//备注:   30%误差范围
//---------------------------------------------------------------
BOOL SetCorrectCorrCarefully( WORD AdjType, BYTE *Buf )
{
	WORD Offset;
	float Err_Data;

	if(api_CheckEEPRomAddr(0xFF) == FALSE)//检查E2地址
	{
		return FALSE;
	}

	//显示开始校表 插入显示
	api_DisplayAdjust();

	SampleSpecCmdOp(0xa6);	//开写保护

	switch (AdjType)
	{
		case 0xB000://100%Ib，1.0(L)--有效值增益校正
			Gain_Adjust( ACCURATE_ADJ, Buf);
			break;

		case 0xB001://100%Ib，0.5L(L)--相位校正
			PH_Adjust( ACCURATE_ADJ, Buf);
			break;

		case 0xB002://5%Ib，1.0(L) 有功功率偏置修正
			Offset_Adjust(ACCURATE_ADJ, Buf);
			break;

		default:
			break;
	}

	SampleSpecCmdOp(0xbc);	//关写保护

	api_WriteFreeEvent(EVENT_ADJUST_METER, AdjType);

	WriteEEpromSampleAdjustReg();//校表系数备份处理
	//保存电压影响量系数
	if( (SelVolCorrectConst == YES)&&((AdjType == 0xB001) || (AdjType == 0xB002)) )
	{
		WriteEEpromSampleVolCorr();
	}

	return TRUE;
}

//--------------------------------------------------
//功能描述: 根据误差计算处对应的写入寄存器的数值 
// 
//参数:      Buf[in]: 数据域 误差值 2个字节
//
//         
//返回值:   TRUE/FALSE
//         
//备注:  
//--------------------------------------------------
static BYTE  SetSampleVolErrAdjust( BYTE  *Buf)
{
	BYTE VolCorrectLevel;	//电压补偿等级

	if( SelVolCorrectConst != YES )
	{
		return FALSE;
	}

	VolCorrectLevel = GetVolCorrectLevel( 0 );

	// 校正偏移量 电压等级1-5
	if( (VolCorrectLevel > 0) && (VolCorrectLevel <= MAX_VOLCORR_OFFSET) )
	{
		SampleSpecCmdOp(0xa6);	//开写保护
		Offset_Adjust(ACCURATE_ADJ, Buf);
		SampleSpecCmdOp(0xbc);	//关写保护
	}
	// 校正相位  电压等级6-11
	else if( (VolCorrectLevel > MAX_VOLCORR_OFFSET) && (VolCorrectLevel <= MAX_VOLCORR_STEP) )
	{
		SampleSpecCmdOp(0xa6);	//开写保护
		PH_Adjust( ACCURATE_ADJ, Buf);
		SampleSpecCmdOp(0xbc);	//关写保护
	}
	else
	{
		return FALSE;
	}

	WriteEEpromSampleVolCorr();
	return TRUE;
}

//-----------------------------------------------
//函数功能: 通讯读一个寄存器
//
//参数:		DI0[in/out]	
//          	in:Buf[0]寄存器地址
//				out:Buf[0]寄存器地址,后面返回对应寄存器读出的数据
//返回值: 	TRUE/FALSE
//		   
//备注:   
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
	//兼容以前上位机，小端模式改为大
	Buf[3] = InBuf[0];
	lib_InverseData( Buf, 4 );
	memcpy(OutBuf, Buf, 4);

	return TRUE;
}

//-----------------------------------------------
//函数功能: 通讯写一个寄存器
//
//参数:		WriteEepromFlag 0x00:只写RAM和计量芯片，不写EEPROM 0xFF:三个地方都写
//			Addr[in]寄存器地址
//			Buf[in]	要写入的数据 两字节，高在后，低在前
//返回值: 	TRUE/FALSE
//		   
//备注:   
//-----------------------------------------------
BOOL SetSampleReg(BYTE WriteEepromFlag,BYTE Addr,BYTE *Buf)
{
	BYTE i, Result;

	//校表初始化操作
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
	else//写寄存器操作
	{				
		for(i=0; i<sizeof(SampleAdjustAddr); i++)
		{
			if( Addr == SampleAdjustAddr[i] ) 
			{
				Result = 0;
				
				if( (SelVolCorrectConst == YES) && ((Addr == P1OFFSET) || (Addr == P1OFFSETL) || (Addr == GPhs1)) )
				{
					Result = GetVolCorrectLevel( 0 );//获取电压影响量等级
					
					//小电流点电压补偿
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
					//大电流点电压补偿
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
					
					//数据存EEPROM
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
				else//不写EEPROM 但是更新RAM校验 上电自动恢复
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
				else//不写EEPROM 但是更新RAM校验 上电自动恢复
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
	//其它50H以后寄存器可写，但不保存到EE
	Link_WriteSampleReg(Addr,Buf);
	SampleSpecCmdOp(0x00);
	
	api_WriteFreeEvent( EVENT_WRITE_REG, (WORD)((WriteEepromFlag << 8)|Addr) );
	
	// 应该记录告警事件
	api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+Addr);
	
	return TRUE;
}
//---------------------------------------------------------------
//函数功能: 通讯写电压影响量系数
//
//参数: 	Addr[in]寄存器地址
//		    Buf[in]	要写入的数据 两字节，高在后，低在前
//                   
//返回值:  
//
//备注:   
//---------------------------------------------------------------
BOOL SetSampleVolCorrectCorr( WORD Addr, BYTE *Buf )
{
	BYTE i;
	WORD Offset;
	
	if( SelVolCorrectConst != YES )
	{
		return FALSE;
	}
	
	//全部系数
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
	//国网电压影响量  A000 -- A00A
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
//函数功能: 通讯读电压影响量系数
//
//参数: 		Addr[in]寄存器地址
//				Buf[in]	要写入的数据 2字节
//返回值: 		TRUE/FALSE 
//
//备注:   
//---------------------------------------------------------------
BOOL ReadSampleVolCorrectCorr( WORD Addr, BYTE *Buf )
{
	BYTE i;
	WORD Offset;
	
	//全部系数
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
//函数功能: 通讯写瞬时量系数
//
//参数:		Addr[in]寄存器地址
//			Buf[in]	要写入的数据 四字节，高在后，低在前
//返回值: 	TRUE/FALSE
//
//备注:
//-----------------------------------------------
BOOL SetSampleCorr( WORD Addr, BYTE *Buf )
{
	BYTE Result;
	
	switch( Addr )
	{
		case 0x9000://电压系数
		case 0x90FF://电压系数数据块
			memcpy( SampleCorr.UGain.b, Buf, 4 );
			Result = TRUE;
			break;
		case 0x9100://火线电流系数
			memcpy( SampleCorr.I1Gain.b, Buf, 4 );
			Result = TRUE;
			break;
		case 0x9103://零线电流系数
			memcpy( SampleCorr.I2Gain.b, Buf, 4 );
			Result = TRUE;
			break;
		case 0x91FF://电流系数数据块
			memcpy( SampleCorr.I1Gain.b, Buf, 8 );
			Result = TRUE;
			break;
		case 0x9200://功率系数
		case 0x92FF://功率系数数据块
			memcpy( SampleCorr.P1Gain.b, Buf, 4 );
			Result = TRUE;
			break;
		case 0x9F00://电压电流功率瞬时量数据块
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
//函数功能: 通讯读瞬时量系数
//
//参数:		Addr[in]寄存器地址
//			Buf[in]	要写入的数据 四字节，高在后，低在前
//返回值: 	TRUE/FALSE
//		   
//备注:   
//-----------------------------------------------
BOOL ReadSampleCorr( WORD Addr, BYTE *Buf )
{
	BYTE Result;
	
	switch( Addr )
	{
		case 0x9000://电压系数
		case 0x90FF://电压系数数据块
			memcpy( Buf, SampleCorr.UGain.b, 4 );
			Result = TRUE;
			break;
		case 0x9100://火线电流系数
			memcpy( Buf, SampleCorr.I1Gain.b, 4 );
			Result = TRUE;
			break;
		case 0x9103://零线电流系数
			memcpy( Buf, SampleCorr.I2Gain.b, 4 );
			Result = TRUE;
			break;
		case 0x91FF://电流系数数据块
			memcpy(Buf, SampleCorr.I1Gain.b, 8);
			Result = TRUE;
			break;
		case 0x9200://功率系数
		case 0x92FF://功率系数数据块
			memcpy( Buf, SampleCorr.P1Gain.b, 4 );
			Result = TRUE;
			break;
		case 0x9F00://电压电流功率瞬时量数据块
			memcpy( Buf, SampleCorr.UGain.b, 16 );
			Result = TRUE;
			break;
		default:
			Result = FALSE;
	}
	
	if( (Result == TRUE) && ((Addr&0x00FF) == 0x00FF) )//数据块填充FF
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
//函数功能: 通讯操作校表系数
//
//参数:		Operation[in]	READ/WRITE
//          Addr[in]		0xFFFF			校表初始化
//							0x0000~0x8FFF	采样芯片寄存器
//							0x9000~0x9FFF	瞬时量系数
//							0xA000~0xAFFF	电压影响量系数
//							0xB000~0xBFFF	自热补偿系数
//			Len[in]			返回数据长度
//			Buf[in/out]		数据域，高在后，低在前
//
//返回值: 	TRUE/FALSE
//
//备注:
//-----------------------------------------------
BOOL api_ProcSampleCorr(BYTE Operation, WORD Addr, BYTE Len, BYTE *Buf)
{
	WORD i,Result;
	
	if( Operation == WRITE )
	{
		//设置采样芯片寄存器
		if( Addr < 0x9000 )
		{
			if( Addr < 0x2000 )
			{
				//只保存到ram及采样芯片，不保存到eeprom，重新上电时恢复原值
				Result = SetSampleReg( 0x00, Addr, Buf );
			}
			else
			{
				//保存到ram及采样芯片及eeprom
				Result = SetSampleReg( 0xFF, Addr, Buf );
				
				api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+Addr);
			}
			
			if( Result == FALSE )
			{
				return FALSE;
			}
		}
		
		//设置瞬时量系数
		else if( Addr < 0xA000 )
		{
			Result = SetSampleCorr( Addr, Buf );
			if( Result == FALSE )
			{
				return FALSE;
			}
		}
		
		//设置电压影响量系数
		else if( ( Addr >= 0xA000 ) && ( Addr <= 0xA0FF ) )
		{
			Result = SetSampleVolCorrectCorr( Addr, Buf );
			if( Result == FALSE )
			{
				return FALSE;
			}
		}
		// 单相电表精调 
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
		//读取采样芯片寄存器
		if( Addr < 0x9000 )
		{
			//数据块方式读出
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
		
		//读取瞬时量系数
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
					//电压系数
					*Buf = 0x01;
					Buf += 1;
					memcpy(Buf,SampleCorr.UGain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
					//火线电流系数
					*Buf = 0x11;
					Buf += 1;
					memcpy(Buf,SampleCorr.I1Gain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
					//零线电流系数
					*Buf = 0x14;
					Buf += 1;
					memcpy(Buf,SampleCorr.I2Gain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
					//火线功率系数
					*Buf = 0x21;
					Buf += 1;
					memcpy(Buf,SampleCorr.P1Gain.b,sizeof(TFourByteUnion));
					Buf += sizeof(TFourByteUnion);
					//零线功率系数 暂时保留
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
		//读电压影响量系数
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
//函数功能: 初始化计量与精度无关参数
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_FactoryInitSample( void )
{
}

#endif// #if( SAMPLE_CHIP == CHIP_HT7017 )
