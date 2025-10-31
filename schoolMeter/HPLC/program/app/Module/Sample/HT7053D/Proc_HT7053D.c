//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.7.30
//描述		采样芯片HT7017处理主文件
//修改记录	!!!!!!以后要添加电压影响量程序 缺写脉冲数
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7053D.h"
#include "AHour.h"
#include "statistics_voltage.h"

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
TRemoteValue		RemoteValue;		//运动数据 即瞬时量数据
TSampleCorr			SampleCorr;			//瞬时量系数
TSampleAdjustReg    SampleAdjustReg;	//计量芯片校表类数据
TSampleCtrlReg      SampleCtrlReg;      //计量芯片控制类数据
TSampleVolCorr		SampleVolCorr;		//电压影响量系数

//电压影响量电压范围 80% 85% 90% 110% 115% 65%~130%
const WORD DefVolCorrectRange[5][2] =
{
	{ 1430, 1790 },	//0.8Un  - 1760
	{ 1790, 1930 }, //0.85Un - 1870
	{ 1930, 2010 }, //0.9Un  - 1980
	{ 2390, 2450 },	//1.1Un  - 2420
	{ 2450, 2860 }, //1.15Un - 2530
};

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
float			gfPaverage;						//计算1分钟平均功率
BYTE			byEnergyPulse;					//新增脉冲个数
BYTE			byEnergyPulseBak;				//新增脉冲个数备份
BYTE			byPaverageCnt;					//平均功率累加次数	
BYTE			byBackFlagCnt;					//反向判断计数
BYTE			bySampleStep;					//大循环刷新远动数据协调器，每秒读一次，每次在大循环多次刷新各个远动数据
BYTE			EMU_flag;						//计量芯片状态寄存器
BYTE 			byReWriteSampleChipRegCount;	//判断芯片错误计数器
BYTE			byLastVolCorrectLevel;			//上一次电压影响量补偿等级
BYTE			byVoltageErrCount;				//电压异常寄存器
//待定测试
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void CheckSampleChipCtrlReg(void);
static void CheckSampleChipAdjustReg(void);

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 计算一分钟平均功率
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:更新RemoteValue.Pav，为HEX码   
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
//函数功能: 刷新瞬时量数据并刷新标志U, I[0], P, I[1], F, S, Cos, Pav
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void Read_UIP(void)	
{
	BYTE Result, Cmd;
	TFourByteUnion ComData;
				
	if( bySampleStep == 0xff )
	{
		return;
	}
	
	//计量芯片故障后前55秒不进行计量通讯 防止卡顿出现 后5秒强制进行通讯 
	if( api_CheckError(ERR_CHECK_5460_1) == TRUE )
	{
		if( RealTimer.Sec < 55 )//故障时，每分钟的后5秒进行计量通讯
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
			Result = Link_ReadSampleReg(URMS,ComData.b);//如果读取错误应该怎么处理？@@@@
			if (Result == TRUE)
			{
				RemoteValue.U.d = ComData.d*1000/SampleCorr.UGain.d;
			}
			if(RemoteValue.U.d > 450000) 
			{
				RemoteValue.U.d = 0;//电压>450V为异常		
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
				//判断是否大于启动功率同时功率符号位为1
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
							//设置合相反向标志
							api_SetSysStatus(eSYS_STATUS_OPPOSITE_P);
							//设置A相反向标志
							api_SetSysStatus(eSYS_STATUS_OPPOSITE_P_A);
						}	
					}
					else if( RemoteValue.P.l > 0 )
					{
						byBackFlagCnt = 0;
						//清合相反向标志
						api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P);
						//清A相反向标志
						api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P_A);
					}
				}
			}
			else
			{
				byBackFlagCnt = 0;
				//清合相反向标志
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P);
				//清A相反向标志
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P_A);
			}
			
			break;
		case 0x03:	//IA 应该先读功率再读电流 因为电流的方向需要根据功率判断
			Result = Link_ReadSampleReg(IARMS,ComData.b);
			if (Result == TRUE)
			{
				RemoteValue.I[0].l = (long)((double)ComData.d*1000.0/SampleCorr.I1Gain.d);
			}
			
			if(RemoteValue.I[0].l > 1600000) 
			{
				RemoteValue.I[0].l = 0;//电流>160A为异常
			}
			
			//电流方向与功率保持一致
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
				RemoteValue.I[1].l = 0;//电流>160A为异常
			}
			
			//零线有功功率读取正确, 零线电流方向以零线功率方向为准
			if( Link_ReadSampleReg(PowerPB,ComData.b) != FALSE )
			{
				//零线电流互感器正接--功率为负电流为负
				if( ZeroCTPositive == YES )
				{
					if( ComData.b[2] & 0x80 )
					{
						RemoteValue.I[1].l *= -1;
					}
				}//零线电流互感器反接--功率为正电流为负
				else
				{
					if( !(ComData.b[2] & 0x80) )
					{
						RemoteValue.I[1].l *= -1;
					}
				}
			}//零线有功功率读取错误, 零线电流方向以火线功率方向为准
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
					gfPaverage += RemoteValue.P.l;	//累计每分钟平均功率值
				}
				byPaverageCnt ++;
			}
			break;
			
		case 0x08:	//Freq
			//6M/6/2/Freq_U,2位小数
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
						// SoftResetSample(); //软复位计量芯片
						HardResetSample(); //硬复位计量芯片
						api_SetError(ERR_CHECK_5460_1);
					}
				}
				
				CheckSampleChipCtrlReg();  //必须是先复位计量芯片 再进行寄存器校验
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
			
		default: //U, I[0], P, I2有效值
			break;
	}	
	
	bySampleStep++;

	if( bySampleStep >= 0x0a )
	{
		//判断是否满足定时器发脉冲工况
		bySampleStep = 0xff;
	}	
		
	if( EMU_flag != 0xff )
	{       	
		//启动功率判断
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
//函数功能: 根据电压状态判断是否关掉脉冲输出
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:此函数还没写完   
//-----------------------------------------------
static void Judge_U_Data(void)
{	
	//异常处理(关闭脉冲计量)	此处如何处理？？？？？？
	if( RemoteValue.U.l < Defendenergyvoltage )
	{
		//关脉冲
	}
	else
	{
		//开脉冲
	}
	
	//电压为基本电压的十分之一认为电压错误
	if(RemoteValue.U.l < (wStandardVoltageConst * 10)) //电压保留三位小数 基本电压保留一位小数 所以要乘十
	{
		if(byVoltageErrCount < 17)
		{
			byVoltageErrCount++;
			if(byVoltageErrCount == 15) //比校验HFConst延迟5秒 防止一起复位芯片
			{
				// SoftResetSample(); //软复位计量芯片
				HardResetSample(); //硬复位计量芯片
				CheckSampleChipCtrlReg();  //必须是先复位计量芯片 再进行寄存器校验
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
//函数功能: 计量芯片通信故障检查
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void CheckEmuErr(void)
{
	static BYTE	EmuErrCnt;                      //计量芯片通信错误计数器
	BOOL Result;
	TFourByteUnion ComData;

	//7017 UART通讯有校验 可以判断出数据是否正确 不判断HFConst值 在Read_UIP中有判断
	//如果读取回来的数据是FF（也可能是0） 也不做计量芯片故障的记录
	//解决电压缓降 出现记录芯片复位 但是CPU未复位的情况实现计量芯片故障误报
	Result = Link_ReadSampleReg(HFConst,ComData.b);
	
	if( Result != TRUE )
	{
		if(EmuErrCnt < 12 )
		{
			EmuErrCnt++;
			if( EmuErrCnt == 10 )
			{
				// SoftResetSample(); //软复位计量芯片
				HardResetSample(); //硬复位计量芯片
				api_SetError(ERR_CHECK_5460_1); //置计量芯片通信故障
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
//函数功能: 计量芯片UIP系数校验
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
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
//函数功能: ram中保存的计量芯片校表寄存器校验
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
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
//函数功能: ram中保存的计量芯片寄存器校验
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
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
        	if( ComData.d == 0x705304 )//5000:1芯片
        	{
        		memcpy( (BYTE *)&SampleCtrlReg, (BYTE *)&SampleCtrlDefData, sizeof(TSampleCtrlReg)-sizeof(DWORD) );
        	}
        	else//8000:1芯片
        	{
              //待定
        		memcpy( (BYTE *)&SampleCtrlReg, (BYTE *)&SampleCtrlDefData8000, sizeof(TSampleCtrlReg)-sizeof(DWORD) );
        	}
		}
	}
}

//---------------------------------------------------------------
//函数功能: RAM中保存的电压影响量参数校验
//
//参数:     无
//                   
//返回值:    无
//
//备注:   
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
			//如果从EEPROM恢复错误 采用5% 1.0的默认系数
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
//函数功能: 效验采样芯片的控制类数据
//
//参数:		无
//
//返回值: 	无
//
//备注:   控制类数据为固定值，eeprom中没有
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

			//连续三次通讯错误 退出本次寄存器校验 解决计量芯片通讯异常后会卡顿的问题
			if( ErrCount > 3 ) //通信错误次数3次
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
//函数功能: 得到电压影响量补偿等级
//
//参数: 	Mode:
//			 0x00-调校调用 返回当前电压等级
//			 0x01-补偿调用 连续判断两次 防止电压波动
//
//返回值:   0：无电压补偿 或者在220V
//         序号---------电流-----功率因数----电压等级
//         1~2  |  Imin  -  5%   | 1.0  | 90%/110%
//         3~5  |  Itr   -  10%  | 1.0  | 80%/85%/115%
//         6~7  |  Itr   -  10%  | 0.5L | 90%/110%
//         8~9  |  10Itr -  100% | 0.5L | 90%/110%
//         10~11|  Imax  -  Imax | 0.5L | 90%/110%
//
//备注:
//---------------------------------------------------------------
BYTE GetVolCorrectLevel( BYTE Mode )
{
	BYTE i;
	BYTE VolLevel, CalCorrectLevel;
	long I_Abs;
	static BYTE LastCmpLevel;		//上一次比较值 用于判断两次得到补偿值
	static BYTE	CurCorrectLevel;	//当前补偿值 如果状态未变 返回当前补偿值
	
	if( CurCorrectLevel > MAX_VOLCORR_STEP )
	{
		CurCorrectLevel = 0;
	}
	
	//电压判断
	VolLevel = 0;
	CalCorrectLevel = 0;
	
	//序号-电压
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
	//电流和功率因数判断
	I_Abs = labs( RemoteValue.I[0].l );
	
	if( VolLevel == 0 )
	{
		CalCorrectLevel = 0;
	}
	else
	{
		//0.5L
		if( (labs(RemoteValue.Cos.l) > 4000) && (labs(RemoteValue.Cos.l) < 6000) ) //判断功率因数是否为0.5L
		{
			//0.5L点只有 0.9Un和1.1Un
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
	
	//防错处理 如果大于最大值 置为0
	if( CalCorrectLevel > MAX_VOLCORR_STEP )
	{
		CalCorrectLevel = 0; 
	}

	//只有连续两次判断结果一致 才认为数据已经切换
	if( LastCmpLevel == CalCorrectLevel )
	{
		CurCorrectLevel = CalCorrectLevel;
	}
	else
	{
		LastCmpLevel  = CalCorrectLevel;
	}
	//调校调用直接返回当前电压等级 不进行两次判断
	if( Mode == 0 )
	{
		return CalCorrectLevel;
	}
	
	return CurCorrectLevel;
}

//---------------------------------------------------------------
//函数功能: 电压影响量补偿程序
//
//参数:    无
//
//返回值:  无
//
//备注:    必须1秒调用一次
//         序号---------电流-----功率因数----电压等级
//         1~2  |  Imin  -  5%   | 1.0  | 90%/110%
//         3~5  |  Itr   -  10%  | 1.0  | 80%/85%/115%
//         6~7  |  Itr   -  10%  | 0.5L | 90%/110%
//         8~9  |  10Itr -  100% | 0.5L | 90%/110%
//         10~11|  Imax  -  Imax | 0.5L | 90%/110%
//---------------------------------------------------------------
static void CorrectVolInflu( void )
{
	BYTE VolCorrectLevel;	//电压补偿等级
	
	if( SelVolCorrectConst != YES )
	{
		return;
	}
	
	//电压影响量参数校验
	CheckSampleVolRam();
	VolCorrectLevel = GetVolCorrectLevel( 1 );
	
	//上次已经调校过 无需再次调校
	if( VolCorrectLevel == byLastVolCorrectLevel )
	{
		return;
	}

	//1.0补偿 补偿OFFSET
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
	
	//0.5L补偿 补偿Gphse1
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

	byLastVolCorrectLevel = VolCorrectLevel;	//记录上次电压补偿等级
}

//-----------------------------------------------
//函数功能: 效验采样芯片的校正类数据
//
//参数:		无
//
//返回值: 	无
//
//备注:   校正类数据保存在eeprom中，ram中也有一份
//-----------------------------------------------
static void CheckSampleChipAdjustReg(void)
{
	BYTE i, SetFlag;
	BYTE VolCorrectLevel;	//电压补偿等级
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

		//5000:1的HT7017校表寄存器0x58(无功相位校正)和0x64(移采样点相位校正)参数不写入计量芯片
		if( (SampleAdjustAddr[i] == QPhsCal) || (SampleAdjustAddr[i] == DEC_Shift) )
		{
			//读芯片ID
			Result = Link_ReadSampleReg( DeviceID, ComData.b );
			//读取失败, 跳过本次循环
			if( Result != TRUE )
			{
				continue;
			}
			else//读取成功
			{
				if( ComData.d == 0x705304 ) //5000:1芯片
				{
					continue;//跳过本次循环
				}
				else //8000:1芯片
				{
					//不处理, 继续往下执行
				}
			}
		}
		
		Result = Link_ReadSampleReg( SampleAdjustAddr[i], ComData.b );
		if( Result != TRUE )
		{	
			ErrCount++;
            
            if( ErrCount > 3 ) //通信错误次数3次
            {
                return;
            }
            
			continue;			
		}
		else
		{
			ErrCount = 0;
		}
						  
		//开启电压影响量 在对应点 P1OFFSET、P1OFFSETL和GPhs1特殊处理
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
//函数功能: 新增脉冲个数，脉冲中断调用
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注: 单相采用脉冲中断，三相采用读寄存器（此处为空函数）  
//-----------------------------------------------
void api_AddSamplePulse(void)
{
	byEnergyPulse++;
}


//-----------------------------------------------
//函数功能: 处理脉冲个数
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void ProcEnergyPulse(void)
{
	BYTE IntStatus;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	BYTE byEnergyPulseBak;               //新增脉冲个数备份
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
			api_WritePulseEnergy( PHASE_ALL + P_ACTIVE, byEnergyPulseBak ); // 正向
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
			api_WritePulseEnergy( PHASE_ALL + N_ACTIVE, byEnergyPulseBak ); // 反向
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
//函数功能: 初始化采样芯片
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
BOOL api_InitSampleChip( void )
{
    return TRUE;
}
//--------------------------------------------------
//功能描述:  初始化计量芯片采样波形输出配置
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_InitSampleWave( void )
{
	TTwoByteUnion SPIOpData;

	//配置串口输出的一帧报文中包含的采样数据点数
	Link_OpenSampleUartReg(0x01);
	// Link_OpenSampleUartReg(0x2);

	SampleSpecCmdOp(0xbc);

	//配置周波采样点数----目标一周波128点，5.5296M的7053D，默认速率921600输出
	// SPIOpData.w = 0x90;
	SPIOpData.w = 0x120;
	Link_WriteSampleReg( 0x39,SPIOpData.b);
	//波形随频采样配置
	SPIOpData.w = 0x0803;
	Link_WriteSampleReg( 0x38,SPIOpData.b);

	//波形输出的配置
	SPIOpData.w = 0x860;
	Link_WriteSampleReg( 0x4A, SPIOpData.b);
	SPIOpData.w = 0x869;
	// // SPIOpData.w = 0x889;
	// SPIOpData.w = 0x809; //单独U通道
	Link_WriteSampleReg( 0x4A, SPIOpData.b);

	SPIOpData.w = 0x00; //41寄存器
	Link_WriteSampleReg( 0x41, SPIOpData.b);

	SampleSpecCmdOp(0x00);
}
//-----------------------------------------------
//函数功能: 上电初始化计量芯片
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_PowerUpSample(void)	
{		
	BYTE AdcCon[2] = {0x00, 0x00}; 
	TTwoByteUnion SPIOpData;

	byReWriteSampleChipRegCount = 0;
	byLastVolCorrectLevel = 0xFF;
		
	#if( SEL_STAT_V_RUN == YES )//恢复电压合格率
	PowerUpVRunTime( );
	#endif
	// 参考瑞工，先软复位寄存器
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
//函数功能: 掉电处理计量芯片
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_PowerDownSample(void)
{
	memset(RemoteValue.U.b,0x00,sizeof(TRemoteValue));
	RemoteValue.Cos.l = 10000;
	
}


//-----------------------------------------------
//函数功能: 采样芯片秒任务
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void Task_Sec_Sample(void)
{	
	TRealTimer t;	

	bySampleStep = 0; 

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);

	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )//5s后进行电压合格率累计，避免监测时间不为1440
	{
		CheckSampleChipCtrlReg();
		#if( SEL_STAT_V_RUN == YES )//计算电压合格率参数		
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
		//每分钟计算一次平均功率
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
		if( ClrVRunMonTimer > 30 )//极限值判断
		{
			ClrVRunMonTimer = 3;
		}
		
		ClrVRunMonTimer--;
		if( ClrVRunMonTimer == 0 )
		{
			//每月1日0时清月电压合格率
			api_ClearVRunTimeNow(BIT1);
		}
	}
	
	if( ClrVRunDayTimer != 0 )
	{
		if( ClrVRunDayTimer > 30 )//极限值判断
		{
			ClrVRunDayTimer = 3;
		}
		
		ClrVRunDayTimer--;
		if( ClrVRunDayTimer == 0 )
		{
			//每日0时清日电压合格率
			api_ClearVRunTimeNow(BIT0);
		}
	}
	#endif

	//计量芯片故障检查
	//如果电压缓降出现计量芯片复位 但是CPU没有复位的情况 可能会出现计量芯片故障的误报
	CheckEmuErr();  //不能放到Read_UIP 因为出现计量通讯故障Read_UIP执行非常卡顿

	Judge_U_Data();
	
	//电压影响量补偿
	CorrectVolInflu();
	
	#if( SEL_AHOUR_FUNC == YES )
	AccumulateAHour();
	#endif
}


//-----------------------------------------------
//函数功能: 采样芯片大循环任务
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
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
			//延时3秒清，防止在冻结的时候数据为0
			if(t.Day == 1)
			{
				ClrVRunMonTimer = 3;
			}
			
			ClrVRunDayTimer = 3;
		}
		else
		{
			//每小时转存电压合格率
			SwapVRunTime();
		}
		#endif
	}
	//读瞬时UIP值	
	Read_UIP();	
}

//-----------------------------------------------
//函数功能: 获取采样芯片远动数据
//
//参数:	
//	Type[in]:	
//		D15-D12 0:总				PHASE_ALL
//				1:A相				PHASE_A
//				2:B相				PHASE_B	
//				3:C相				PHASE_C	
//				4:N线				PHASE_N	
//				5:N线零序				PHASE_N2
//				6:A相				基波PHASE_A
//				7:B相				基波PHASE_B	
//				8:C相				基波PHASE_C	
//		D07-D00										单位	字节长度	小数点位数
//				0:电压				REMOTE_U		V			4			3					
//				1:电流				REMOTE_I		A 			4			4
//				2:电压相角			REMOTE_PHASEU	度			2			1	
//				3:电压电流相角		REMOTE_PHASEI	度			2			1							
//				4:有功功率			REMOTE_P		kW			4			6
//				5:无功功率			REMOTE_Q		kVAR		4			6
//				6:视在功率			REMOTE_S		kVA			4			6
//				7:一分钟平均有功功率REMOTE_P_AVE	kW			4			6						
//				8:一分钟平均无功功率REMOTE_Q_AVE	kVAR		4			6	
//				9:一分钟平均视在功率REMOTE_S_AVE	kVA			4			6	
//				A:功率因数			REMOTE_COS					4			4
//				B:电压波形失真率					%			2			2 							
//				C:电流波形失真率					%			2			2			
//				D:电压谐波含有量					%			2			2			
//				E:电流谐波含有量					%			2			2		
//				F:电网频率			REMOTE_HZ		HZ			2			2
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//	Dot[in]			小数位数 若为ff则为默认小数位数
//	Len[in]			数据长度
//  Buf[out] 		输出数据       
//返回值: 	TRUE:正确返回值		FALSE：异常
//		   
//备注:  
//-----------------------------------------------
//						0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F	10
const BYTE MaxDot[] = 	{3,	4,	1,	1,	6,	6,	6,	6,	6,	6,	4,	2,	2,	2,	2,	2,	4};
const BYTE DefDot[] = 	{1,	3,	1,	1,	4,	4,	4,	4,	4,	4,	3,	2,	2,	2,	2,	2,	3};
const BYTE MaxLen[] = 	{4,	4,	2,	2,	4,	4,	4,	4,	4,	4,	4,	2,	2,	2,	2,	2,	4};
BOOL api_GetRemoteData(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *Buf)
{
	TFourByteUnion td;
	BYTE ReadDot,BackFlag;
	
	//单相表只支持总和A相,若为电流，可以为PHASE_N
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
			if(( (Type&0xf000) == PHASE_A ) || ( (Type&0xf000) == PHASE_RA ))//单相表基波电流返回全波电流
			{
				td.d = RemoteValue.I[0].l;
			}
			else if((Type&0xf000) == PHASE_N)//单相表不判零线电流开关
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
			
			//反向最高位置符号
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
//函数功能: 获取采样芯片工况
//
//参数:		Type[in]	
//				0x00--电压不平衡率 
//				0x01--电流不平衡率
//				0x02--负载率
//				0x10--电压状态
//				0x11--电流状态
//				0x20--起动潜动状态
//				0x3X--当前相无功象限 0x30~0x33 分别代表总/A/B/C相无功所处象限
//              0x40--当前总视在功率是否大于额定功率的千分之四
//返回值: 	0xffff--不支持此数据
//				0x00--电压不平衡率	数据类型：long-unsigned，单位：%，换算：-2 
//              0x01--电流不平衡率	数据类型：long-unsigned，单位：%，换算：-2
//				0x02--负载率		数据类型：long-unsigned，单位：%，换算：-2
//				0x10--电压状态		Bit15 1--电压相序状态，bit0~2	1--A/B/C相电压小于0.6Un
//				0x11--电流状态		Bit15 1--电流相序状态，bit0~2	1--A/B/C相电流小于5%Ib
//				0x20--起动潜动状态	1--潜动		0--起动
//				0x3X--当前相无功象限 返回1~4，代表一象限~四象限
//              0x40--当前总视在功率是否大于额定功率的千分之四                 0--大于	1--小于
//备注:   
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
