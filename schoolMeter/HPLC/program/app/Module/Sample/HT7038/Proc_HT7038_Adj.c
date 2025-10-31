//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.10.6
//描述		采样芯片HT7038校表程序文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7038.h"

#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )

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
WORD	wSmallStandardCurrent;		//小相角校正对应电流
DWORD	AdjustDivCurrent[10];		//分段校正各点基准值

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
		wSmallStandardCurrent = 200;
		
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
			AdjustDivCurrent[9] = 1;         //9         0.001A        
		
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
		wSmallStandardCurrent = 300;
		
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
			AdjustDivCurrent[9] = 2;            //9         0.0015A      0.1%Ib     
			
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
	//电流规格 5(60) 或者 电流规格 10(100)
	else if( (MeterCurrentTypesConst == CURR_60A) || (MeterCurrentTypesConst == CURR_100A) )
	{
		dwBigStandardCurrent = 60000;
		wSmallStandardCurrent = 6000;
		
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
			AdjustDivCurrent[9] = 5;           //9         0.005A       0.1%Ib      
			
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
//函数功能: 校表初始化
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注: 
//-----------------------------------------------
static void AdjustInit(void)
{
	//开写ee校正系数开关
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	
	//采样芯片偏移类寄存器数据
	UseSampleDefaultPara(0);
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleOffsetPara), sizeof(TSampleOffsetPara), (BYTE*)&SampleOffsetPara);
	WriteSampleChipOffsetPara();
	
	//采样芯片控制类寄存器数据
	UseSampleDefaultPara(1);
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlPara), sizeof(TSampleCtrlPara), (BYTE*)&SampleCtrlPara);
	WriteSampleChipCtrlPara();
	
	//采样芯片增益类寄存器数据
	UseSampleDefaultPara(2);
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustPara), sizeof(TSampleAdjustPara), (BYTE*)&SampleAdjustPara);
	WriteSampleChipAdjustPara();
	
	//分段精调数据清零
	memset( (BYTE *)&SampleAdjustParaDiv, 0x00, sizeof(TSampleAdjustParaDiv) );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleAdjustParaDiv ), sizeof(TSampleAdjustParaDiv), (BYTE *)&SampleAdjustParaDiv );

	//自热补偿 (温度补偿)数据清零
	memset( (BYTE *)&SampleManualModifyPara, 0x00, sizeof(TSampleManualModifyPara) );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleManualModifyPara ), sizeof(TSampleManualModifyPara), (BYTE *)&SampleManualModifyPara );
	
	//直流偶次谐波系数
	if( lib_CheckSafeMemVerify( (BYTE *)&SampleHalfWavePara, sizeof(TSampleHalfWavePara), UN_REPAIR_CRC ) == FALSE )
	{
		memcpy( (BYTE *)&SampleHalfWavePara, (BYTE *)SampleHalfWaveDefData, (sizeof(TSampleHalfWavePara) - sizeof(DWORD)) );
	}
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWavePara ), sizeof(TSampleHalfWavePara), (BYTE *)&SampleHalfWavePara );
	
	//精调默认系数清零
	memset( (BYTE *)&OriginalPara, 0x00, sizeof(TOriginalPara) );
		
	
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
static void ResetSampleReg(BYTE Reg_Addr)
{
    DWORD tdw;

	tdw = 0;
	WriteSampleReg( Reg_Addr, tdw );
}


//-----------------------------------------------
//函数功能: 校正电压、电流增益
//
//参数:		Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void UIP_K(BYTE *Buf)
{
	WORD i,j;
	DWORD dwData,dwAdjustU[3],dwAdjustI[3];
	DWORD CaliStandardU[3],CaliStandardI[3];
	float tf;
	
	//计算输入的标准电压电流
	for(i=0;i<3;i++)	
	{
		//采用上位机带下来的电压，电流值
		if( (Buf[1]==0xff) && (Buf[2]==0xff) && (Buf[42]==0xaa) )
		{
			memcpy( (BYTE*)&CaliStandardU[i], Buf+4+i*13, 4 );//3位小数
			memcpy( (BYTE*)&CaliStandardI[i], Buf+8+i*13, 4 );//4位小数
		}
		else
		{
			CaliStandardU[i] = (DWORD)wCaliVoltageConst * 100; //3位小数
			CaliStandardI[i] = dwBigStandardCurrent*10;		//4位小数
		}
	}	
	
	for(i=0; i<3; i++)
	{
		if((MeterTypesConst==METER_3P3W)&&( i == 1 ))
		{
			continue;
		}
		
		//初始化寄存器	
		ResetSampleReg(w_UGainA+i);
		ResetSampleReg(w_IGainA+i);
		dwAdjustU[i] = 0;
		dwAdjustI[i] = 0;	
	}
	
	CLEAR_WATCH_DOG;
				
	//1.7hz，两个采样周期
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//读出标准电压电流多次的平均值			
	for(j=0;j<6;j++)
	{
		for(i=0;i<3;i++)
		{
			if((MeterTypesConst==METER_3P3W)&&( i == 1 ))
			{
				continue;
			}	
			
			//电压
			dwData = ReadMeasureData( r_UARMS+i );
			dwAdjustU[i] += dwData*1000/TWO_POW13;
			
			//电流
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

		//计算电压误差
		tf = (float)CaliStandardU[i]/dwAdjustU[i] - 1;
		if( tf < 0 )
		{
			dwData = TWO_POW16 - (DWORD)(fabs(tf)*(TWO_POW15));
		}
		else
		{
			dwData = (DWORD)(tf*(TWO_POW15));
		}
		//写ee
		WriteSampleParaToEE(0xff, w_UGainA+i, (BYTE*)&dwData);
		//写采样芯片
		WriteSampleReg( w_UGainA+i, dwData );
		
		//计算电流误差
		tf = (float)CaliStandardI[i]/dwAdjustI[i] - 1;
		if( tf < 0 )
		{
			dwData = TWO_POW16 - (DWORD)(fabs(tf)*(TWO_POW15));
		}
		else
		{
			dwData = (DWORD)(tf*(TWO_POW15));
		}

		//写ee
		WriteSampleParaToEE(0xff, w_IGainA+i, (BYTE*)&dwData);
		//写采样芯片
		WriteSampleReg( w_IGainA+i, dwData );
	}
	
	//刷新电流 分段精调校完UIP系数需要刷新一下电流 校准误差需要正确的电流来判断区段
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
//函数功能: 校正零线电流增益
//
//参数:		Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表          
//返回值: 	无
//		   
//备注:   
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
		memcpy( (BYTE *)&CaliStandardI, Buf + 8, 4 ); //4位小数
	}
	else
	{
		CaliStandardI = wSmallStandardCurrent * 10;//4位小数
	}
	
	//初始化寄存器
	ResetSampleReg( w_GainADC7 );
	
	//1.7hz，两个采样周期
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//读出标准零线电流多次的平均值
	for( i = 0; i < 6; i++ )
	{
		//电流
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
	
	//计算电流误差
	tf = (float)CaliStandardI / dwAdjustI - 1;
	if( tf < 0 )
	{
		dwData = TWO_POW16 - (DWORD)(fabs(tf)*(TWO_POW15));
	}
	else
	{
		dwData = (DWORD)(tf*(TWO_POW15));
	}
	
	//写ee
	WriteSampleParaToEE(0xff, w_GainADC7, (BYTE*)&dwData);
	//写采样芯片
	WriteSampleReg( w_GainADC7, dwData );
}

//-----------------------------------------------
//函数功能: 零线电流偏置校正
//
//参数:		Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void I0rmsOffset_Adjust( void )
{
	BYTE i = 0;
	DWORD Irms = 0, IrmsSum = 0, IRmsOffset = 0;
	
	if( SelZeroCurrentConst == NO )
	{
		return;
	}
	
	//零线电流偏置校正
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
	
	//写ee
	WriteSampleParaToEE( 0xFF, 0xFE, (BYTE *)&IRmsOffset );
}

//-----------------------------------------------
//函数功能: 火线电流偏置校正
//
//参数:		Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void IrmsOffset_Adjust( void )
{
	BYTE i = 0, j = 0;
	DWORD Irms = 0, IrmsSum = 0, IRmsOffset = 0;
	
	//初始化偏置校正寄存器
	ResetSampleReg( w_ItRmsOffset );
	ResetSampleReg( w_IARMSOffset );
	ResetSampleReg( w_IBRMSOffset );
	ResetSampleReg( w_ICRMSOffset );
	//1.7hz，两个采样周期
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//电流矢量和偏置校正
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
	//写ee
	WriteSampleParaToEE( 0xFF, w_ItRmsOffset, (BYTE *)&IRmsOffset );
	//写采样芯片
	WriteSampleReg( w_ItRmsOffset, IRmsOffset );

	//A、B、C相电流偏置校正
	for( i = 0; i < MAX_PHASE; i++ )
	{
		IrmsSum = 0;
		//电流有效值偏置校正
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
		//写ee
		WriteSampleParaToEE( 0xFF, w_IARMSOffset + i, (BYTE *)&IRmsOffset );
		//写采样芯片
		WriteSampleReg( w_IARMSOffset + i, IRmsOffset );
	}
}

//-----------------------------------------------
//函数功能: 增益校正
//
//参数:		Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表          
//返回值: 	无
//		   
//备注:调整功率增益0x10个步长，对应误差约万分之5   
//-----------------------------------------------
static void Gain_Adjust( BYTE *Buf )
{
	BYTE i,j,DivCurrentNo;
	DWORD tdw;
	DWORD RatedPower[3];
	DWORD SampleP[3];
	float gf;
	
	//防止误操作 校正增益时点相角
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
		
		//初始化寄存器	
		ResetSampleReg(w_PGainA+i);
		ResetSampleReg(w_QGainA+i);
		ResetSampleReg(w_SGainA+i);
		
		SampleP[i] = 0;
	}
	
	//1.7hz，两个采样周期
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	gf = 3089.90478515625 * 1000 / HFCONST_DEF / (api_GetActiveConstant());//功率系数 保留0.001W
	
	//读采样的功率
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
	
	//计算输入的标准功率
	for(i=0;i<MAX_PHASE;i++)	
	{
		DivCurrentNo = GetRowSampleSegmentAdjustNo( i );
		//采用上位机带下来的功率值
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

		//将系数保存到ee
		if( SelDivAdjust == YES )
		{
			SampleAdjustParaDiv.PGainDiv[i][DivCurrentNo] = tdw;
			
			//开写ee校正系数开关
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
		//写采样芯片
		WriteSampleReg(w_PGainA+i, tdw);
		WriteSampleReg(w_QGainA+i, tdw);
		WriteSampleReg(w_SGainA+i, tdw);
	}
}

//-----------------------------------------------
//函数功能: 相角校正
//
//参数:		Type[in] 0--大相角 1--小相角
//			Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表           
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void PH_Adjust(BYTE Type,BYTE * Buf)
{	
	BYTE i,j,DivCurrentNo;
	DWORD tdw;
	DWORD RatedPower[3];
	DWORD SampleP[3];
	float gf;
	
	ASSERT( Type < 2, 0 );
	
	//防止误操作 校正相角时点增益
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
						
		//初始化寄存器	
		if( Type == 1 )
		{
			ResetSampleReg(w_PhSregApq1+i);
		}
		else
		{
			ResetSampleReg(w_PhSregApq0+i);
			//如果打开分段精调 需要将小电流相角也清空 因为分段可能采用的是小相角作为补偿值
			if(SelDivAdjust == YES)
			{
				ResetSampleReg(w_PhSregApq1+i);
			}
		}
		
		SampleP[i] = 0;
	}
	
	//1.7hz，两个采样周期
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//计算输入的标准功率
	for(i=0;i<MAX_PHASE;i++)
	{
		DivCurrentNo = GetRowSampleSegmentAdjustNo( i );
		//采用上位机带下来的功率值
		if( (Buf[1]==0xff) && (Buf[2]==0xff) && (Buf[42]==0xaa) )
		{
			memcpy( (BYTE*)&RatedPower[i], Buf+12+i*13, 4 );//单位w，3位小数
		}
		else
		{
			//单位w，3位小数
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
	
	
	gf = 3089.90478515625 * 1000 / HFCONST_DEF / (api_GetActiveConstant());//功率系数 保留0.001W
	
	//读采样的功率 读6次去平均值
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
				
				//三相全部校完 将数据存入EEPROM
				if( i ==( MAX_PHASE-1) )
				{
					api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustParaDiv), sizeof(TSampleAdjustParaDiv), (BYTE*)&SampleAdjustParaDiv);
					api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				}
				
				//精调程序在小相角时将SampleCtrlPara中的数据也刷新
				if( AdjustDivCurrent[DivCurrentNo] == wSmallStandardCurrent )
				{
					//将系数保存到ee
					WriteSampleParaToEE(0xff, w_PhSregApq1+i, (BYTE*)&tdw);
				}
				//写采样芯片
				WriteSampleReg(w_PhSregApq1+i, tdw);
			}
			
			//大相角将系数保存到ee
			if( (SelDivAdjust != YES)||(AdjustDivCurrent[DivCurrentNo] == dwBigStandardCurrent) )
			{
				WriteSampleParaToEE( 0xff, w_PhSregApq0 + i, (BYTE *)&tdw );
			}
			//写采样芯片
			WriteSampleReg(w_PhSregApq0+i, tdw);
		}
		else
		{
			//开启分段补偿 校小相角时将对应段也写进去
			if( SelDivAdjust == YES )
			{
				SampleAdjustParaDiv.PhsRegDiv[i][DivCurrentNo] = tdw;

				//三相全部校完 将数据存入EEPROM
				if( i == (MAX_PHASE - 1) )
				{
					api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleAdjustParaDiv ), sizeof(TSampleAdjustParaDiv), (BYTE *)&SampleAdjustParaDiv );
					api_ClrSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
				}
			}
			
			//将系数保存到ee
			WriteSampleParaToEE(0xff, w_PhSregApq1+i, (BYTE*)&tdw);
			//写采样芯片
			WriteSampleReg(w_PhSregApq1+i, tdw);
		}
	}
}
//-----------------------------------------------
//函数功能: 直流偶次谐波初始化
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注: 
//-----------------------------------------------
static void HalfWaveParaInit(void)
{
	//开写ee校正系数开关
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	
	//直流偶次谐波系数
	memcpy( (BYTE *)&SampleHalfWavePara, (BYTE *)SampleHalfWaveDefData, (sizeof(TSampleHalfWavePara) - sizeof(DWORD)) );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWavePara ), sizeof(TSampleHalfWavePara), (BYTE *)&SampleHalfWavePara );

	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	
}

//---------------------------------------------------------------
//函数功能: 直流偶次谐波系数调校
//
//参数: 	Addr：地址标识
//			Buf：补偿数据
//                   
//返回值:  无
//
//备注:   
//---------------------------------------------------------------
static void HalfWave_Adjust( WORD Addr, BYTE *Buf )
{
	BYTE i = 0, EffectivePoint = 0;
	SWORD Data = 0, Offset;
	WORD Current = 0;
	WORD ( *Pointer )[2];
	double Error, Slope;
	
	CLEAR_WATCH_DOG;
	
	//1.0L增益校正和0.5L相角校正
	if( (Addr == 0xA139) || (Addr == 0xA13A) )
	{
		//0.5L相角校正
		if( Addr == 0xA139 )
		{
			//防止误操作 校正相角时点增益
			if( (labs(RemoteValue.Cos[0].l) > 6500) || (labs(RemoteValue.Cos[0].l) < 3500) )
			{
				return;
			}
			Pointer = SampleHalfWavePara.Coe;
			//校正值
			memcpy( &Data, Buf, sizeof(Data) );
			Error = (double)Data / 10000.0 / 1.732 * -1;
		}//1.0L增益校正
		else
		{
			//防止误操作 校正增益时点相角
			if( labs(RemoteValue.Cos[0].l) < 8000 )
			{
				return;
			}
			Pointer = SampleHalfWavePara.Gain;
			//校正值
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
		//电流
		Current = labs( RemoteValue.I[0].l ) / 1000;
		//判断有效点, 有效点为1表示之前已经校正过1个点, 有效点最多为3表示3个点全都校过.
		for( i = 0; i < 3; i++ )
		{
			if( ((*(*(Pointer + i) + 1)) >= HD2CurrentRangeMin)
				&& ((*(*(Pointer + i) + 1)) <= HD2CurrentRangeMax) )
			{
				EffectivePoint++;
			}
		}
		
		//重复点判断(当前点重复校正)
		//有效点为1
		if( EffectivePoint == 1 )
		{
			//如果当前点与第一个点相同
			if( (Current > (0.9 * (*(*(Pointer) + 1))))
				&& (Current < (1.1 * (*(*(Pointer) + 1)))) )
			{
				//保存第一个点的补偿值, 有效点置为0, 后面重新记录
				Data = (SWORD)(*(*(Pointer)));
				EffectivePoint = 0;
			}//如果当前点和第一个点不同
			else
			{
				//保存第一个点的补偿值
				Data = (SWORD)(*(*(Pointer)));
			}
		}//有效点为2
		else if( EffectivePoint == 2 )
		{
			//如果当前点与第一个点相同
			if( (Current > (0.9 * (*(*(Pointer) + 1))))
				&& (Current < (1.1 * (*(*(Pointer) + 1)))) )
			{
				//保存第一个点的补偿值, 去掉第一个点, 后面重新记录
				Data = (SWORD)(*(*(Pointer)));
				(*(*(Pointer))) = (*(*(Pointer + 1)));
				(*(*(Pointer) + 1)) = (*(*(Pointer + 1) + 1));
				(*(*(Pointer + 1))) = 0;
				(*(*(Pointer + 1) + 1)) = 0;
				EffectivePoint = 1;
			}//如果当前点与第二个点相同
			else if( (Current > (0.9 * (*(*(Pointer + 1) + 1))))
					 && (Current < (1.1 * (*(*(Pointer + 1) + 1)))) )
			{
				//保存第二个点的补偿值
				Data = (SWORD)(*(*(Pointer + 1)));
				(*(*(Pointer + 1))) = 0;
				(*(*(Pointer + 1) + 1)) = 0;
				EffectivePoint = 1;
			}//如果有效点为2并且不和前两个点相同, 说明当前点为第三个点, 计算直线系数
			else
			{
				Slope = (double)((SWORD)(*(*(Pointer + 1))) - (SWORD)(*(*Pointer))) / ((SWORD)(*(*(Pointer + 1) + 1)) - (SWORD)(*(*(Pointer)+1)));
				Offset = (SWORD)((SWORD)(*(*Pointer)) - (Slope * (SWORD)(*(*(Pointer) + 1))));
				Data = 0;
			}
		}//有效点为3
		else if( EffectivePoint == 3 )
		{
			//如果当前点与第一个点相同
			if( (Current > (0.9 * (*(*(Pointer) + 1))))
				&& (Current < (1.1 * (*(*(Pointer) + 1)))) )
			{
				//保存第一个点的补偿值, 去掉第一个点, 后面重新记录
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
			}//如果当前点与第二个点相同
			else if( (Current > (0.9 * (*(*(Pointer + 1) + 1))))
					 && (Current < (1.1 * (*(*(Pointer + 1) + 1)))) )
			{
				//保存第二个点的补偿值, 去掉第二个点, 后面重新记录
				Slope = 0;
				Offset = 0;
				Data = (SWORD)(*(*(Pointer + 1)));
				(*(*(Pointer + 1))) = (*(*(Pointer + 2)));
				(*(*(Pointer + 1) + 1)) = (*(*(Pointer + 2) + 1));
				(*(*(Pointer + 2))) = 0;
				(*(*(Pointer + 2) + 1)) = 0;
				EffectivePoint = 2;
			}//如果当前点与第三个点相同
			else if( (Current > (0.9 * (*(*(Pointer + 2) + 1))))
					 && (Current < (1.1 * (*(*(Pointer + 2) + 1)))) )
			{
				//保存第三个点的补偿值
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
		
		//数据记录
		//如果有效点为0, 则当前点为第一个点, 记录当前点数据
		if( EffectivePoint == 0 )
		{
			(*(*(Pointer))) = (WORD)((SWORD)Error + Data);
			(*(*(Pointer) + 1)) = Current;
		}//如果有效点为1, 则当前点为第二个点
		else if( EffectivePoint == 1 )
		{
			Data = Data + (SWORD)Error;
			//如果当前点电流值大于第一个点的电流值
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
		}//如果有效点为2, 则当前点为第三个点
		else if( EffectivePoint == 2 )
		{
			Data = (SWORD)(Slope * (labs( RemoteValue.I[0].l ) / 1000) + Offset) + (SWORD)Error + Data;
			//如果当前点电流值大于第二个点的电流值
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
		}//3个有效点不允许再校不同点
		else
		{
			return;
		}
	}//0.5L初始化
	else if( Addr == 0xA13B )
	{
		memset( SampleHalfWavePara.Coe, 0x00, sizeof(SampleHalfWavePara.Coe) );
	}//1.0L初始化
	else if( Addr == 0xA13C )
	{
		memset( SampleHalfWavePara.Gain, 0x00, sizeof(SampleHalfWavePara.Gain) );
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
//			Buf[in]			高字节在后，低字节在前	AA 校表方式选择 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA 校表方式选择1 校表方式选择2
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA 	
//          校表方式选择	0xffff--上位机输入参数校表	0x0000--约定值校表
//返回值: 	TRUE/FALSE
//		   
//备注:校表规约调用     
//-----------------------------------------------
WORD api_ProcSampleAdjust(BYTE AdjType, BYTE *Buf)
{		
	//判断规约格式合法性
	if( (Buf[0]!=0xaa) || (Buf[3]!=0xaa) )
	{
		return FALSE;
	}

	if(api_CheckEEPRomAddr(0xFF) == FALSE)//检查E2地址,E2地址不正确不允许校表
	{
		return FALSE;
	}
	
	//显示开始校表 插入显示
	api_DisplayAdjust();
	
	//初始化标准源的数值
	InitSampleCaliUI();

	//关闭校表寄存器写保护
	SampleSpecCmdOp(0xa6);
	
	switch( AdjType )
	{
		case 0xff://初始化									
			AdjustInit();			
			break;
		case 0xf1://初始化直流偶次谐波系数									
			HalfWaveParaInit();			
			break;
		case 0x01://100%Ib，1.0(L)
			UIP_K(Buf);
			Gain_Adjust(Buf);	//有效值增益校正			
			break;
		case 0x02:
			Gain_Adjust(Buf);	//有效值增益精调	
			break;
		case 0x51://0.5L(L)大相角 相角精调
			PH_Adjust(0,Buf);	//相位校正
			break;		
		case 0x52://0.5L(L)小相角
			PH_Adjust(1,Buf);	//相位校正
			break;
			
			//零线电流先校正增益再校偏置!!!
		case 0x81:
			if(SelZeroCurrentConst == YES)
			{
				ZeroCurrentGain_Adjust( Buf );//零线电流增益系数校正
			}
			break;
			
		case 0x00:
			if(SelZeroCurrentConst == YES)
			{
				I0rmsOffset_Adjust();//零线偏置校正
			}
			IrmsOffset_Adjust();//A、B、C相电流偏置校正
			break;
			
		default: 
			break;
	}	
	
	api_WriteFreeEvent(EVENT_ADJUST_METER, (WORD)AdjType);

	//打开校表寄存器写保护
	SampleSpecCmdOp(0x00);
	
	CLEAR_WATCH_DOG;
	api_Delayms(600);
	CLEAR_WATCH_DOG;
	
	//更新完采样后，读出校正系数的效验和
	GetSampleChipCheckSum();
	
	return TRUE;
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
//备注: 7038，0000~3FFF为校正寄存器	4000~8fff为计量寄存器  
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
		if( Addr < 0x4000 )//校正寄存器
		{	
			SampleSpecCmdOp(0xa6);
			Result = WriteSampleReg(SampleAddr,tdw&0x0000ffff);	
			//如果打开精调 往大相角写数时同时写到小相角去
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
			
			//将系数保存到ee
			if( Addr < 0x2000 )
			{
				//只保存到ram及采样芯片，不保存到eeprom，重新上电时恢复原值
				WriteSampleParaToEE(0x00, SampleAddr, (BYTE*)&tdw);	
			}
			else
			{
				//保存到ram及采样芯片及eeprom
				WriteSampleParaToEE(0xff, SampleAddr, (BYTE*)&tdw);	
				
				api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+SampleAddr);
			}
			
			//更新完采样后，读出校正系数的效验和
			GetSampleChipCheckSum();
			
			// 应该记录告警事件
			api_WriteSysUNMsg(MANUAL_MODIFIED_PARA+SampleAddr);
		}
		else if((Addr >= 0xA100)&&(Addr <= 0xA1FF))//精调相关参数
		{
			WRFlag[0] = FALSE;
			WRFlag[1] = FALSE;
			
			if(Addr <= 0xA102) 	//增益分段系数
			{
				memcpy( SampleAdjustParaDiv.PGainDiv[Addr - 0xA100], Buf, sizeof(SampleAdjustParaDiv.PGainDiv[0]));
				WRFlag[0] = TRUE;
			}
			else if(Addr == 0xA10F)	//增益分段系数数据块
			{
				memcpy( SampleAdjustParaDiv.PGainDiv, Buf, sizeof(SampleAdjustParaDiv.PGainDiv) );
				WRFlag[0] = TRUE;
			}
			else if(Addr <= 0xA112)	//相角分段系数
			{
				memcpy( SampleAdjustParaDiv.PhsRegDiv[Addr - 0xA110], Buf, sizeof(SampleAdjustParaDiv.PhsRegDiv[0]) );
				WRFlag[0] = TRUE;
			}
			else if(Addr == 0xA11F)	//相角分段系数数据块
			{
				memcpy( SampleAdjustParaDiv.PhsRegDiv, Buf, sizeof(SampleAdjustParaDiv.PhsRegDiv) );
				WRFlag[0] = TRUE;
			}
			else if(Addr <= 0xA122)	//无功修正系数
			{
				memcpy( SampleManualModifyPara.ManualModifyQGain[Addr - 0xA120], Buf, sizeof(SampleManualModifyPara.ManualModifyQGain[0]) );
				WRFlag[1] = TRUE;
			}
			else if(Addr == 0xA12F)	//无功修正系数数据块
			{
				memcpy( SampleManualModifyPara.ManualModifyQGain, Buf, sizeof(SampleManualModifyPara.ManualModifyQGain) );
				WRFlag[1] = TRUE;
			}
			else if(Addr == 0xA130)	//反向有功修正系数
			{
				memcpy( SampleManualModifyPara.ManualModifyForRA, Buf, sizeof(SampleManualModifyPara.ManualModifyForRA) );
				WRFlag[1] = TRUE;
			}
			else if(Addr == 0xA131)	//计量芯片TOffset温补偿值
			{
				SampleManualModifyPara.ManualModifyChipToffset = Buf[0];
				WRFlag[1] = TRUE;
			}
			else if( (Addr >= 0xA133) && (Addr <= 0xA135) )
			{
				//更改该系数必须该相处于电压不平衡的状态 防止校错
				if( JudgeUnBalance( Addr - 0xA133 ) == FALSE )
				{
					return FALSE;
				}
				memcpy( &SampleManualModifyPara.VolUnbalanceGain[Addr - 0xA133], Buf, sizeof(SampleManualModifyPara.VolUnbalanceGain[Addr - 0xA133]) );
				WRFlag[1] = TRUE;
				UnBalanceGainFlag[Addr - 0xA133] = FALSE; //置为未刷新 最新调校数据写入计量芯片
			}
			else if(Addr == 0xA136) //正向偏移系数全部设置
			{
				memcpy( SampleManualModifyPara.ManualModifyForWard, Buf, sizeof(SampleManualModifyPara.ManualModifyForWard) );
				WRFlag[1] = TRUE;
			}
			else if(Addr == 0xA137) //单个设置正向偏移系数
			{
				//设置该系数时 A相电流必须在对应的区段
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
			else if(Addr == 0xA138) //单个设置反向偏移系数
			{
				//设置该系数时 A相电流必须在对应的区段
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
			}//直流偶次谐波校表
			else if( (Addr >= 0xA139) && (Addr <= 0xA13E) )
			{
				//写全部0.5L系数
				if( Addr == 0xA13D )
				{
					memcpy( SampleHalfWavePara.Coe, Buf, sizeof(SampleHalfWavePara.Coe) );
				}//写全部1.0L系数
				else if( Addr == 0xA13E )
				{
					memcpy( SampleHalfWavePara.Gain, Buf, sizeof(SampleHalfWavePara.Gain) );
				}//校表
				else
				{
					HalfWave_Adjust( Addr, Buf );
				}
				//系数存EEPROM
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWavePara ), sizeof(TSampleHalfWavePara), (BYTE *)&SampleHalfWavePara );
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}
			else if( Addr == 0xA13F )//直流偶次谐波时间参数
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

			//系数存EEPROM
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
			
			//把当前区段数据重新设置为FF 重新开始补偿
			memset( g_ucNowSampleDivisionNo, 0xFF, sizeof(g_ucNowSampleDivisionNo) );
		}
	}
	else
	{
		if( Addr < 0x4000 )//校正寄存器
		{
			//数据块方式读出
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
		else if( Addr < 0x9000 )//计量寄存器
		{
			tdw = ReadMeasureData(SampleAddr);	
			memcpy(Buf,(BYTE*)&tdw,Len);
		}
		else if((Addr >= 0xA100)&&(Addr <= 0xA1FF))//抄读精调数据
		{
			//返回数据长度根据下发的长度确定 如果长度不够 数据抄读不全 如果长度超 会读取其他数据
			if(Len > 65) //下列参数最长的60个字节 限制长度65个字节 防止Len长度异常抄读内存溢出
			{
				return FALSE;
			}
			if(Addr <= 0xA102)  //增益分段系数
			{
				memcpy( Buf, SampleAdjustParaDiv.PGainDiv[Addr - 0xA100], Len );
			}
			else if(Addr == 0xA10F)	//增益分段系数数据块
			{
				memcpy( Buf, SampleAdjustParaDiv.PGainDiv, Len );
			}
			else if(Addr <= 0xA112)	//相角分段系数
			{
				memcpy( Buf, SampleAdjustParaDiv.PhsRegDiv[Addr - 0xA110], Len );
			}
			else if(Addr == 0xA11F)	//相角分段系数数据块
			{
				memcpy( Buf, SampleAdjustParaDiv.PhsRegDiv, Len );
			}
			else if(Addr <= 0xA122)	//无功修正系数
			{
				memcpy( Buf, SampleManualModifyPara.ManualModifyQGain[Addr - 0xA120], Len );
			}
			else if(Addr == 0xA12F)	//无功修正系数数据块
			{
				memcpy( Buf, SampleManualModifyPara.ManualModifyQGain, Len );
			}
			else if(Addr == 0xA130)	//反向有功修正系数
			{
				memcpy( Buf, SampleManualModifyPara.ManualModifyForRA, Len );
			}
			else if(Addr == 0xA131)	//计量芯片TOffset温补偿值
			{
				Buf[0] = SampleManualModifyPara.ManualModifyChipToffset;
			}
			else if( (Addr >= 0xA133) && (Addr <= 0xA135) ) //电压不平衡系数
			{
				memcpy( Buf, &SampleManualModifyPara.VolUnbalanceGain[Addr - 0xA133], Len );
			}
			else if(Addr == 0xA136) //正向有功修正系数
			{
				memcpy( Buf, SampleManualModifyPara.ManualModifyForWard, Len );
			}
			else if(Addr == 0xA137) //读取当前段正向偏移系数
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
			else if(Addr == 0xA138) //读取当前段反向偏移系数
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
			else if( (Addr >= 0xA139) && (Addr <= 0xA13F) ) //直流偶次谐波补偿
			{
				//读全部0.5L系数
				if( Addr == 0xA13D )
				{
					memcpy( Buf, SampleHalfWavePara.Coe, sizeof(SampleHalfWavePara.Coe) );
				}//读全部1.0L系数
				else if( Addr == 0xA13E )
				{
					memcpy( Buf, SampleHalfWavePara.Gain, sizeof(SampleHalfWavePara.Gain) );
				}//直流偶次谐波时间参数
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
	//直流偶次谐波时间参数
	SampleHalfWaveTimePara.Hours = HalfWaveDefCheckTime;
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWaveTimePara ), sizeof(TSampleHalfWaveTimePara), (BYTE *)&SampleHalfWaveTimePara );
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
}


#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
