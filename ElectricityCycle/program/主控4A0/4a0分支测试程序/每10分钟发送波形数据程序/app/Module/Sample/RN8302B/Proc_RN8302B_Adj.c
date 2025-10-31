//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	
//创建日期	2016.7.30
//描述		采样芯片RN2026校表文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "RN8302B.h"


#if( SAMPLE_CHIP == CHIP_RN8302B )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------


//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
DWORD	dwBigStandardCurrent;		//大相角校正对应电流
DWORD	dwSmallStandardCurrent;		//小相角校正对应电流
DWORD	dwMaxCurrent;				//最大电流相角校正对应电流
DWORD	AdjustDivCurrent[9];		//分段校正各点基准值
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 初始化校表标准源的电压、电流
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注: 
//-----------------------------------------------
void InitSampleCaliUI( void )
{	
	//初始化校表电流	
	//电流规格 0.3(1.2)
	if( MeterCurrentTypesConst == CURR_1A )
	{
		dwBigStandardCurrent = 1500;
		dwSmallStandardCurrent = 200;
		
		//如果打开分段补偿
		if( SelDivAdjust == YES )
		{
			//0.3(1.2)A分段校准点如下   
			AdjustDivCurrent[0] = 1500;		 //0         1.5A        IMax   
			AdjustDivCurrent[1] = 600;       //1         0.6A        0.5IMax
			AdjustDivCurrent[2] = 300;       //2         0.3A        Ib     
			AdjustDivCurrent[3] = 200;       //3         0.20A       50%Ib  
			AdjustDivCurrent[4] = 60;        //4         0.06A       20%Ib  
			AdjustDivCurrent[5] = 30;        //5         0.03A       10%Ib  
			AdjustDivCurrent[6] = 15;        //6         0.015A      5%Ib   
			AdjustDivCurrent[7] = 6;         //7         0.006A      2%Ib   
			AdjustDivCurrent[8] = 3;         //8         0.003A      1%Ib   
		
			//9个滞回区间上下限
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
	//电流规格 1.5(6)
	else if( MeterCurrentTypesConst == CURR_6A)
	{
		dwBigStandardCurrent = 1500;
		dwSmallStandardCurrent = 300;
		
		if( SelDivAdjust == YES )
		{
			//校表基本点初始化
			AdjustDivCurrent[0] = 6000;			 //0         6A          IMax     
			AdjustDivCurrent[1] = 3000;          //1         3A          0.5IMax  
			AdjustDivCurrent[2] = 1500;          //2         1.5A        Ib       
			AdjustDivCurrent[3] = 750;           //3         0.75A       50%Ib    
			AdjustDivCurrent[4] = 300;           //4         0.3A        20%Ib    
			AdjustDivCurrent[5] = 150;           //5         0.15A       10%Ib    
			AdjustDivCurrent[6] = 75;            //6         0.075A      5%Ib     
			AdjustDivCurrent[7] = 30;            //7         0.03A       2%Ib     
			AdjustDivCurrent[8] = 15;            //8         0.015A      1%Ib     
			
			//9个滞回区间上下限
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
	//电流规格 5(20) 或者 电流规格 10(100)
	else if(MeterCurrentTypesConst == CURR_20A)
	{
		dwBigStandardCurrent = 5000;
		dwSmallStandardCurrent = 1000;
		
		if( SelDivAdjust == YES )
		{
			//校表基本点初始化
			AdjustDivCurrent[0] = 60000;		//0         60A         IMax      
			AdjustDivCurrent[1] = 30000;        //1         30A         0.5IMax   
			AdjustDivCurrent[2] = 6000;         //2         6A          Ib        
			AdjustDivCurrent[3] = 2500;         //3         2.5A        50%Ib     
			AdjustDivCurrent[4] = 1000;         //4         1A          20%Ib     
			AdjustDivCurrent[5] = 500;          //5         0.5A        10%Ib     
			AdjustDivCurrent[6] = 250;          //6         0.25A       5%Ib      
			AdjustDivCurrent[7] = 100;          //7         0.1A        2%Ib      
			AdjustDivCurrent[8] = 50;           //8         0.05A       1%Ib      
			
			//9个滞回区间上下限
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
	//电流规格 50(250) 125A/160A/200A/250A
	else if(MeterCurrentTypesConst == CURR_500A)
	{
		dwMaxCurrent = 500000; //暂定200A
		dwBigStandardCurrent = 50000*2;
		dwSmallStandardCurrent = 10000*2;
		
		if( SelDivAdjust == YES )
		{
			//校表基本点初始化
			AdjustDivCurrent[0] = 500000*2;		//0         IMax      
			AdjustDivCurrent[1] = 250000*2;       //1         0.5IMax   
			AdjustDivCurrent[2] = 100000*2;        //2         Ib        
			AdjustDivCurrent[3] = 50000*2;        //3         50%Ib     
			AdjustDivCurrent[4] = 20000*2;        //4         20%Ib     
			AdjustDivCurrent[5] = 1000*2;         //5         10%Ib     
			AdjustDivCurrent[6] = 5000*2;         //6         5%Ib      
			AdjustDivCurrent[7] = 2000*2;         //7         2%Ib      
			AdjustDivCurrent[8] = 1000*2;          //8         1%Ib      
			
			//9个滞回区间上下限
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
//函数功能: EDT基准值保存到EEPROM //@liang
//
//参数:    无
//
//返回值:  无
//
//备注:
//---------------------------------------------------------------
void WriteEEpromEDTDataPara( void )
{
	api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.EDTDataPara ), sizeof(TEDTDataPara), (BYTE *)&EDTDataPara );
	api_ClrSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
}

//-----------------------------------------------
//函数功能: 复位对应采样寄存器
//
//参数:		Reg_Addr[in]	寄存器地址
//
//返回值: 	无
//
//备注: 复位的校表数据
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
//函数功能: 校表系数保存到EEPROM，并置刷新标志
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
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
//函数功能: 校表初始化
//
//参数:		InBuf[in]	传入的HFConst值
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void InitSamplePara( BYTE chipNo )
{	
	UseSampleDefaultPara(eALL, chipNo);

	WriteEEpromSamplePara(eALL, chipNo); //写入E2P

	//重新初始化计量芯片
	api_PowerUpSample(chipNo);
}
//--------------------------------------------------
// 功能描述:  校表同时校8306电压电流系数
//
// 参数:     
//
// 返回值:
//
// 备注:
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

	// 计算公式
	if (memcmp(&RegisterU[j], &cmpval8[0], 4) != 0)
	{
		Ku = (float)(Vol / RegisterU[j]); // 电压
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
		Ku = (float)(Cur / RegisterI[j]); // 电 流
		Gain = (float)((SWORD)SampleAdjustReg[chipNo].GSI[j].d / 32768.0);
		K2 = (float)(Ku * 32 * (1 + Gain));
		UIFactorArr[chipNo].Ik[j] = K2;
	}
	else
	{
		return FALSE;
	}

	// 写E2P
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleUIKArr[chipNo].Uk[0]), sizeof(TSampleUIK), (BYTE *)&UIFactorArr[chipNo]);
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

	return Result;
}
//--------------------------------------------------
// 功能描述:  调整8306芯片的电压电流系数,存储到E2P
//
// 参数:
//
// 返回值:
//
// 备注:需要设置校表参数为 220v
//--------------------------------------------------
BOOL api_UI_Kfor8306(BYTE *Buf, BYTE chipNo)
{
	DWORD dvval, dival, dpval;
	BYTE i = 0, j = 0, cmpval8[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	BYTE *data = Buf + 2;
	BOOL Result = FALSE;
    
	// 解析C相、B相、A相的数据
	for (i = 0; i < 3; i++)
	{
		// 跳过0xAA字节，然后读取电压、电流、功率
		if (data[0] != 0xAA)
		{
			return FALSE; // 如果不是0xAA，数据格式错误
		}

		// 读取电流、功率、电压
		dpval = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];	 // 电压
		dival = (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8];	 // 电流
		dvval = (data[9] << 24) | (data[10] << 16) | (data[11] << 8) | data[12]; // 功率

		// 校准判断条件：220V电压，5A电流，功率0.5
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

		// 计算下一个相数据的位置（每个相13字节）
		data += 13;
	}
	return Result;
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
//备注:  		通道增益系数
//			Ku = ( ( Ui/1000 )*2^27 )/Un,
//			Un-额定电压:220V，
//			Ui-电表输入电压Un时，输入到电压通道ADC，并经PGA后的采样值:放大倍数1*220/(200k*6+1k)*1k=183mv
//			Ki = ( ( Ii/1000 )*2^27 )/Ib,
//			Ib-额定电流:5A，
//			Ii-电表电流为Ib时，输入到电流通道ADC，并经PGA后的采样值:放大倍数16*5*0.3=24mv
//			Kp = (2^31*32*EC*HFconst)/3.6*10^12*8.192
//-----------------------------------------------
static void UIP_K(BYTE *Buf, BYTE chipNo)
{
	TFourByteUnion td;	
	DWORD TempData;
	CLEAR_WATCH_DOG;
	UseSampleDefaultPara(eCORR, chipNo);
	CheckSampleChipReg(chipNo);
	
	td.d = wCaliVoltageConst * dwMeterBasicCurrentConst / 10;		//计算实际功率 保留6位小数
	td.d = (DWORD)((double)td.d * (SampleCorr.PGain[0].d/100) / 1000.0); //计算实际功率值对应的寄存器值	
	//Start_PS=Preg*(0.5~0.8)*K/256
	td.d = td.d * 1 * 6 / 10000 / 256; 	
	if( MeterTypesConst == METER_ZT )
	{
		//按照100A的0.2%*0.6进行设置
		td.d = td.d*2;
	}
	SampleCtrlReg.PSTART.d = td.d;
	WriteSampleReg( PStart_EMU, td.b ,2, chipNo);

	//相位分段校准滞回区间( 1: 67-73%  2:134-146% )
	TempData = (WORD)(((float)dwMeterBasicCurrentConst / 1000.0)*(float)(SampleCorr.IGain[0].d)*IRegionUPConst/409600.0);
	SampleCtrlReg.PRTH1H.d = TempData;
	SampleCtrlReg.PRTH2H.d = (WORD)(TempData*2);//2H,2L不能过大，否则1H,1L写入失败
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
//函数功能: 有效值增益校正(100%Ib,1.0)
//
//参数:		Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void Gain_Adjust(BYTE *Buf,BYTE chipNo)
{
	BYTE i,j;
	TFourByteUnion td;
	float Err_Data,Err_DataU,Err_DataI;
	DWORD RatedP[3],RatedU[3],RatedI[3];
	TFourByteUnion RegisterU[3],RegisterI[3];
	QWORD Sum[2][3];

	UIP_K(Buf, chipNo);//电压电流系数校正
    SampleSpecCmdOp(0x100,CmdWriteOpen,chipNo);//UIP_K函数里面有打开写保护的代码，所以这里重新关闭写保护
	//校表前先把寄存器数据清零
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

	//计算输入的标准电压，电流，功率
	for(i=0;i<MAX_PHASE;i++)	
	{
		//采用上位机带下来的功率值
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
		//通道增益
		//计算电压误差
		Err_DataU = ((double)(RegisterU[i].d)*1000/(double)SampleCorr.UGain[i].d-RatedU[i])/((double)RatedU[i]);
		//计算电流误差
		Err_DataI = ((double)(RegisterI[i].d)*10000.0/(double)SampleCorr.IGain[i].d-RatedI[i])/((double)RatedI[i]);
		
		//取绝对值 当误差大于30% 不允许校正 防止点错按钮 排查硬件故障
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
//函数功能: 相位校正(100%Ib,0.5L)
//
//参数:		Type[in] 0--大相角 1--小相角
//			Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表   
//
//返回值: 	无
//
//备注:
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
	// 	Gain_Adjust(Buf);//单点校准
	// }

	//防止误操作 校正相角时点增益
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
		
		//获取通道相位原始数据
		Originaltd[i].d = 0x00808080;
		Link_ReadSampleReg(PHSIA_EMU+i,Originaltd[i].b,4, chipNo);
		ResetSampleReg( PHSIA_EMU+i ,4, chipNo);
		ResetSampleReg( PHSUA_EMU+i ,4, chipNo);
		
		//采用上位机带下来的功率值
		if( (Buf[1]==0xff) && (Buf[2]==0xff) && (Buf[42]==0xaa) )
		{
			memcpy( (BYTE*)&RatedP[i], Buf+12+i*13, 4 );
		}
		else
		{
		  	if( Type == 0 )//大电流
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
			else//小电流
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

		//计算功率误差
		Err_DataP = ((double)(RegisterP[i].d)*1000.0/(double)SampleCorr.PGain[i].d*100);
		Err_DataP = ((double)(Err_DataP - RatedP[i]))/((double)RatedP[i]);
		
		//取绝对值 当误差大于30% 不允许校正 防止点错按钮 排查硬件故障
		if(fabs(Err_DataP) > ((float)Def_MaxError/100))
		{
			return;
		}
		
		Err_DataP = asin(-Err_DataP/1.732)*180.0/3.1415926;
		//相位校正范围： 50HZ下，±2.25度或 4.5度
		if( Err_DataP>2.2499)
		{
		  	Err_DataP = 2.2499;
		}
		else if( Err_DataP<-2.2499)
		{
		  	Err_DataP = -2.2499;
		}

		if( Type == 0 )//大电流
		{
			if( labs(RemoteValueArr[chipNo].I[i].l/10) > (dwBigStandardCurrent*IRegionUPConst*2/100)) //最大电流相角
			{
				td.d = (Originaltd[i].d&0x000FFFFF)+((DWORD)(0x202-(Err_DataP/0.008789)*1024*1024));
				SampleAdjustReg[chipNo].PHSI[i].d = td.d;
				WriteSampleReg(PHSIA_EMU+i,td.b,4, chipNo);
			}
			else
			{
				td.d = (DWORD)(0x202 - (Err_DataP / 0.008789));   // 未找到相角差2-3刻度原因按照经验值在0x200上＋2，经测试品研线圈效果好
				td.d = (((DWORD)(0x201 - (Err_DataP / 0.008789))) + (td.d * 1024) + (td.d * 1024 * 1024));   // 在此处小电流补1刻度效果好
				SampleAdjustReg[chipNo].PHSI[i].d = td.d;
				WriteSampleReg(PHSIA_EMU+i,td.b,4, chipNo);
			}
			
		}
		else//小电流
		{
			td.d = (Originaltd[i].d&0x3FFFFC00)+(DWORD)(0x202-(Err_DataP/0.008789));
			SampleAdjustReg[chipNo].PHSI[i].d = td.d;
			WriteSampleReg(PHSIA_EMU+i,td.b,4, chipNo);
		}
	}
	lib_ExchangeData(Buf, Buf, 45); // 数据域倒序
	api_UI_Kfor8306(Buf, chipNo); // 电压电流系数计算
    lib_ExchangeData(Buf, Buf, 45); // 数据域重新倒序回来给下个计量芯片使用，其他点位不用倒序，所以只能在api_UI_Kfor8306后面恢复顺序
	CLEAR_WATCH_DOG;
}
//@@@@测试，之后更改上位机后再修改
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
//函数功能: 有功功率偏置修正(5%Ib,1.0)
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
//		api_Delayms(400);	//延时400ms
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
//函数功能: EDT校准 //@liang
//
//参数:   无
//
//返回值:  无
//
//备注:   无
//-----------------------------------------------
#define EDT_CALI_RETRY_MAX          (5)     //EDT校准命令重发次数
#define EDT_PARAM_READ_RETRY_MAX    (5)     //EDT参数寄存器读取重试次数
#define EDT_CALI_WAIT_TIME          (8*60) //EDT校准等待时间，单位秒
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
        if( WriteSampleReg(EDT_CALI, DumpData.b,4, chipNo) == TRUE )///长度未修改!!!!!!
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
        Link_ReadSampleReg( EDT_STATUS, DumpData.b ,4, chipNo);///长度未修改!!!!!!
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
                if( Link_ReadSampleReg(wAddr, DumpData.b,4, chipNo) == FALSE )///长度未修改!!!!!!
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
            
        //保存到EEP
        WriteEEpromEDTDataPara();
        CLEAR_WATCH_DOG;
    }
}

//---------------------------------------------------------------
//函数功能: 更新采样参数化 用于设置电表电压规格时调用
//
//参数: 	无
//                   
//返回值:  	无
//
//备注:   
//---------------------------------------------------------------
void api_InitSamplePara( void )
{	
	InitSampleCaliUI();
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
WORD api_ProcSampleAdjust(BYTE AdjType, BYTE *Buf, BYTE chipNo)
{	
	DWORD dwSum;
	BYTE lock[2]={0xE5,0xDC};
	if( (Buf[0]!=0xaa) || (Buf[3]!=0xaa) )
	{
		return FALSE;
	}

	//关闭EDT
	//PowerDownEDT();
	
	//初始化标准源的数值
	InitSampleCaliUI();
	//关闭校表寄存器写保护
	SampleSpecCmdOp(0x100,CmdWriteOpen, chipNo);
	//WriteSampleReg(0x0180,lock,1);
	//目前受谐波和EDT参数影响，校表方式采用通道增益+通道相位（之前是功率增益+功率相位）
	switch( AdjType )
	{
		case 0xff://初始化
			InitSamplePara(chipNo);
			break;
		
		case 0x00://分相增益校准

			//三相表互感器零漂写默认值
			break;
			
		case 0x01://100%Ib，1.0(L)
			Gain_Adjust(Buf,chipNo); //有效值增益校正
			break;
			
		case 0x51://大电流，0.5L(L)
			PH_Adjust(0,Buf,chipNo); //通道相位校正
			break;
			
		case 0x52://小电流，0.5L(L)
			PH_Adjust(1,Buf,chipNo); //功率相位校正
			break;
			
		case 0x02://5%Ib，1.0(L)
			Offset_Adjust(Buf,chipNo); //有功功率偏置修正
			break;

        case 0x08://100%Ib，1.0(EDT)
            EDT_Adjust(chipNo); //EDT基准值
            break;    
            
		default: 
			break;
	}	

	api_WriteFreeEvent(EVENT_ADJUST_METER, (WORD)AdjType);

	if( AdjType != 0xff )//在计量初始化里已经写过EEPROM 无需重复
	{
		WriteEEpromSamplePara(eADJUST, chipNo);//校表系数备份处理
	}
	CheckSampleChipReg(chipNo);//防止写入计量芯片失败影响下一步校表
	GetSampleChipCheckSum(chipNo);//读校验和
	//关闭校表寄存器写保护
	SampleSpecCmdOp(0x100,CmdWriteClose, chipNo);
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
BOOL ReadSampleReg(BYTE *InBuf,BYTE *OutBuf)//未使用到
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
	// //兼容以前上位机，小端模式改为大
	// Buf[3] = InBuf[0];
	// lib_InverseData( Buf, 4 );
	// memcpy(OutBuf, Buf, 4);

	// return TRUE;
	return FALSE;
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
BOOL SetSampleReg(BYTE WriteEepromFlag,DWORD Addr,BYTE *Buf)
{
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
	return TRUE;
}

//-----------------------------------------------
//函数功能: 通讯读瞬时量系数
//
//参数:		Addr[in]    寄存器地址
//			Buf[in]	    要读出的数据，四字节，高在后，低在前，共3组
//
//返回值: 	TRUE/FALSE
//		   
//备注:     无
//-----------------------------------------------
BOOL ReadSampleCorr( WORD Addr, BYTE *Buf, BYTE chipNo)
{
    BYTE Result;
	TFourByteUnion td;
	float Gain,Corr;
    BYTE i;
    
	switch( Addr )
	{
		case 0x9301://电压寄存器系数转换值-负荷识别模块使用
			//公式推导由锐能微提供
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
		case 0x9302://电流寄存器系数转换值-负荷识别模块使用
			//公式推导由锐能微提供
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
//			chipNo[in]		芯片编号
//返回值: 	TRUE/FALSE
//
//备注:
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
		//将系数保存到ee
		if( Addr < 0x2000 )
		{
			//只保存到ram及采样芯片，不保存到eeprom，重新上电时恢复原值
			WriteSampleParaToEE(0x00, SampleAddr, (BYTE*)&tdw, chipNo);	
		}
		else
		{
			//保存到ram及采样芯片及eeprom
			WriteSampleParaToEE(0xff, SampleAddr, (BYTE*)&tdw, chipNo);	
			
			api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+SampleAddr);
		}
		GetSampleChipCheckSum(chipNo);//读校验和
	}	
	else
	{	
		//数据块方式读出
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
//函数功能: 获取EDT误差，BCD格式，XXX.XXXX
//
//参数:     Type[in]        EDT类型，0xX0为计量误差，0xX1为相位误差，0xFF为块数据
//	        Len[in]			数据长度
//          Buf[out] 		输出数据
//
//返回值: 	TRUE:正确返回值		FALSE：异常
//
//备注:     外部调用
//          0xX0为I通道
//          0xX1为U1通道
//          0xX2为U2通道
//          0xX3为Un通道
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
