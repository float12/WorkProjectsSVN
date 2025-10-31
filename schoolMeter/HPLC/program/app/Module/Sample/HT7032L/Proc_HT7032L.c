//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.9.30
//描述		采样芯片HT7038处理主文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7032L.h"
#include "AHour.h"
#include "statistics_voltage.h"

#if(SAMPLE_CHIP==CHIP_HT7032L)

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define AD_ONEWAVE_SIMPLE_NUM			72 		//一个周波AD采样数
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------



//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------


//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
TRemoteValue 					RemoteValue;				//远动数据
TSampleCtrlPara					SampleCtrlPara;				//采样芯片控制类寄存器数据
TSampleAdjustPara				SampleAdjustPara;			//采样芯片增益类寄存器数据
TSampleOffsetPara				SampleOffsetPara;			//采样芯片偏移类寄存器数据
TCurrenttValue					CurrentValue[9];			//分段补偿滞回区间上下限
TSampleAdjustParaDiv 			SampleAdjustParaDiv;		//分段补偿数据
TSampleManualModifyPara			SampleManualModifyPara;		//手动修正系数
TSampleHalfWavePara				SampleHalfWavePara;			//直流偶次谐波系数
TSampleHalfWaveTimePara			SampleHalfWaveTimePara;		//直流偶次谐波要求检测时间(小时)
TOriginalPara					OriginalPara;				//精调默认增益和大小相角系数 如果某个点未校 默认采用该系数
	
float	gfPowerCorr;										//功率系数，计算P,Q,S时用
BYTE 	bySampleStep;										//大循环刷新远动数据协调器，每秒读一次，每次在大循环多次刷新各个远动数据
BYTE 	byPowerOnFlag;                                      //上电标志
//DWORD 	dwSamplePFlag;										//有功、无功方向，在每次读有功电能总时刷新
DWORD	dwSampleSFlag;										//断相、相序状态标志，在每秒刷新的第一个远动值时刷新
BYTE 	byReWriteSampleChipRegCount;						//判断芯片效验不对回写计数器
BYTE 	bySamplePowerOnTimer;								//上电倒计时计数器
DWORD 	dwSampleCheckSum[4];									//采样芯片效验和 !!!!!!有四个
float	gfPQSaverage[3][4];									//计算1分钟平均功率
BYTE	byPQSaverageCnt[3][4];								//平均功率累加次数	
BOOL 	UnBalanceGainFlag[3];								//电压不平衡补偿标志 TRUE-已补偿 FALSE-未补偿
BYTE    HalfWaveCoeFlag[3];                                 //直流偶次谐波补偿
BYTE	HalfWaveUpdateFlag[3];								//直流偶次谐波状态更新标志
BYTE	ADCSampleCount;										//计量芯片ADC采样时间计数器
DWORD	HalfWavePowerOnTime;								//直流偶次谐波上电检测累计时间(分钟)
BOOL	HalfWaveStatusFlag;									//直流偶次谐波状态标志  TRUE: 开启判断  FALSE: 关闭判断

//分段补偿有关参数
BYTE 	g_ucNowSampleDivisionNo[MAX_PHASE];					//分段补偿当前段区

static BYTE bHalfWaveCalTime;								//连续判断半波次数

//两个周期的基波相关函数
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
//两个周期的二次谐波相关函数
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
//				内部函数声明
//-----------------------------------------------
static BOOL CheckSampleChipSum(void);
static void Read_UIP(void);
static BOOL JudgeHalfWaveRegulate( void );

//-----------------------------------------------
//				函数定义
//-----------------------------------------------


//---------------------------------------------------------------
//函数功能: 补偿时分段补偿获得当前电流所在的区域
//
//参数: 	ucPhase- 0：A相
//					1：B相
// 					2：C相
//                   
//返回值:  0-9：当前段数
//		  0xFF：返回异常
//
//备注:   判断滞回区间 用于补偿
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
		//因为电流是有符号的数 所以比较之间需要取绝对值 
		if( labs(RemoteValue.I[ucPhase].l)  > (CurrentValue[Num].Upper*10) )
		{
			return Num;	//Num段
		}
		else if(labs(RemoteValue.I[ucPhase].l) > (CurrentValue[Num].Lower*10) )
		{
			if(g_ucNowSampleDivisionNo[ucPhase] == (Num + 1))	//原先在Num+1段
			{	//相邻的1区,不切换
				return (Num + 1);	//Num+1段
			}
			else
			{
				return Num;	//Num段
			}
		}
	}
	
	return 9;
}

//---------------------------------------------------------------
//函数功能: 校表时分段补偿获得当前电流所在的区域
//
//参数: 	ucPhase- 0：A相
//					1：B相
// 					2：C相
//                   
//返回值:  0-9：当前段数
//		  0xFF：返回异常
//
//备注:   不判断滞回区间 用于校准
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
//函数功能: 计算P、Q、S一分钟平均功率
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
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
//函数功能: 写某个校正系数到EEPROM
//
//参数:		WriteEepromFlag[in]	bit0 SampleCtrlAddr 0--写ram不写ee	1--写ram写ee
//								bit1 SampleAdjustAddr 0--写ram不写ee	1--写ram写ee
//								bit2 SampleOffsetAddr 0--写ram不写ee	1--写ram写ee
//			Addr[in]	地址	 
//			Buf[in]		写入数据，四字节，高在后，低在前
//返回值: 	
//		    TRUE/FALSE
//备注: 
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
				//开写ee校正系数开关
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
			
			if(SelDivAdjust == YES)//开启精调
			{
				if((Addr>=w_GainUA)&&(Addr<= w_GainIN)) 	//分段精调不开启 或者写UI系数
				{
					p = (DWORD *)&SampleAdjustPara;
					memcpy((void*)&p[i],Buf,sizeof(DWORD));

				}
				else if((Addr>=w_GPA)&&(Addr<= w_GSC))
				{
					//获取当前寄存器所对应相的区段
					DivCurrentNo = GetRowSampleSegmentAdjustNo( (Addr - w_GPA) % 3  );
					if(AdjustDivCurrent[DivCurrentNo] == dwBigStandardCurrent)//大电流增益或者相角校正点	
					{
						p = (DWORD *)&SampleAdjustPara;
						memcpy((void*)&p[i],Buf,sizeof(DWORD));
					}
				}
				else if((Addr>=w_GphsA0)&&(Addr<= w_GphsC2))
				{
					DivCurrentNo = GetRowSampleSegmentAdjustNo( (Addr - w_GphsA0) /3  );
					if(AdjustDivCurrent[DivCurrentNo] == dwBigStandardCurrent)//大电流增益或者相角校正点	
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
					//分段补偿开启的情况下 在小电流相角点 无论是下发地址是大相角还是小相角 全部写到小相角
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
				//相角怎么写比寄存器？？？？李丕凯
				#warning: "警告:--李丕凯";
			}
			//只更改RAM和计量芯片 不写EE
			if( (WriteEepromFlag&0x02) == 0x00 )
			{
				SampleAdjustPara.CRC32 = lib_CheckCRC32((BYTE*)&SampleAdjustPara,sizeof(TSampleAdjustPara)-sizeof(DWORD));				
			}
			else
			{
				//开写ee校正系数开关
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
					#warning: "警告://单独校正偏执，是否应在这里？--李丕凯";
					else if((Addr >= w_GQA)&&(Addr <= w_GQA))
					{
						QGainTmp = (Buf[1] << 8) + Buf[0];
						PGainTmp = (short)SampleAdjustParaDiv.PGainDiv[Addr - w_GQA][DivCurrentNo];
						if(PGainTmp == 0) //如果该点系数为零 采用默认系数
						{
							PGainTmp = (short)OriginalPara.PGainCoe[Addr - w_GQA];
						}
						QGainTmp = QGainTmp - PGainTmp;
						
						//偏移只有一个字节 补偿值在 -128~127之间
						if(QGainTmp > 127)
						{
							QGainTmp = 127;
						}
						else if(QGainTmp < -128)
						{
							QGainTmp = -128;
						}
						if(DivCurrentNo < 9) //无功补偿没有第10个点
						{
							SampleManualModifyPara.ManualModifyQGain[Addr - w_GQA][DivCurrentNo] = (BYTE)QGainTmp;
						}
						
						//系数存EEPROM
						api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);			
						api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleManualModifyPara ), sizeof(TSampleManualModifyPara), (BYTE *)&SampleManualModifyPara );
						api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
						
						//把当前区段数据重新设置为FF 重新开始写寄存器
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
					//精调系数写入EEPROM
					if(Result == TRUE)
					{
						api_SetSysStatus( eSYS_STATUS_EN_WRITE_SAMPLEPARA );
						api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustParaDiv), sizeof(TSampleAdjustParaDiv), (BYTE*)&SampleAdjustParaDiv);
						api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA); 
					}
				}
			}
			
			//校表系数发生改变重新校验精调默认系数
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
				//开写ee校正系数开关
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
//函数功能: 获取采样芯片校表数据效验和
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注:
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
//函数功能: 检查采样芯片校表数据效验和
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注:
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
//函数功能: 根据不同表的型号获取最大电压值
//
//参数:		无	 
//			
//返回值: 	返回最大电压值
//		    
//备注:额定电压的2.2倍，3位小数，每次更新电压时调用
//-----------------------------------------------
DWORD CalMaxVoltage(void)
{
	DWORD VoltageLimit;
	
	//超限电压限定为额定电压的2.2倍
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
//函数功能: 上电读取电压(A、B、C相)
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注:
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
//函数功能: 读取采样芯片电能寄存器
//
//参数:		Type[in] 电能寄存器地址	 
//			
//返回值: 	返回脉冲个数
//		    
//备注:最多返回一个字节，按照最大80A表，3200脉冲常数算应该能满足要求
//-----------------------------------------------
//static BYTE GetSampleEnergyPulse(BYTE Type)
//{
//	DWORD tdw1,tdw2;
//	
//	//Type和r_BckReg都采用ReadMeasureData读取，r_BckReg返回的是7126A0，此处需要确认 !!!!!!
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
//函数功能: 计算电压，电流不平衡率
//
//参数:		Type[in]	0--电压	1--电流
//			Buf[in]	输入三相电压值或者电流值	 
//			
//返回值: 	不平衡率 数据类型：long-unsigned，单位：%，换算：-2
//		    
//备注:刷新C相数据时同步刷新不平衡率
//-----------------------------------------------
static WORD CaleUnbalanceRate(BYTE Type, long *Buf)
{
	DWORD dwMax,dwMin;
	DWORD dwData[3];
	WORD Result;
		
	dwData[0] = labs(Buf[0]);
	if( MeterTypesConst == METER_3P3W )
	{
		//三相三时，dwData[1]和A相值相同
		dwData[1] = dwData[0];
	}
	else
	{
		dwData[1] = labs(Buf[1]);
	}	
	dwData[2] = labs(Buf[2]);
	
	if( Type == 0 )
	{
		//计算临界电压 60%Un（3位小数）
		dwMin = wStandardVoltageConst * 100 * 6 / 10;
	}
	else
	{
		//计算5%Ib（4位小数）
		dwMin = wMeterBasicCurrentConst * 10 * 5 / 100;
	}
	
	//当三相均小于时不计算不平衡率
	if( (dwData[0]<dwMin) && (dwData[1]<dwMin) && (dwData[2]<dwMin) )
	{
		return 0;
	}
	
	dwMax = dwData[0];
	dwMin = dwData[0];
	
	//计算最大值
	if( dwMax < dwData[1] )
	{
		dwMax = dwData[1];
	}	
	if( dwMax < dwData[2] )
	{
		dwMax = dwData[2];
	}
	
	//计算最小值
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
//函数功能: 初始化芯片温度自动补偿功能
//
//参数:     无
//                   
//返回值:   FALSE
//		   TRUE
//
//备注:   
//---------------------------------------------------------------
BYTE InitMChipVrefgain(void)
{
	BYTE Result=TRUE;
	#warning: "警告://未写--李丕凯";
	return Result;
}
//---------------------------------------------------------------
//函数功能: 判断现在是否处于电压不平衡状态
//
//参数:    相数 0 1 2（A\B\C）
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL JudgeUnBalance( BYTE Phase )
{
	if( Phase >= MAX_PHASE )
	{
		return FALSE;
	}
	
	//该相电压不在Un附近 不补偿 0.95Un - 1.05Un
	if((RemoteValue.U[Phase].d < (wStandardVoltageConst * 100 * 95 / 100))
	   ||(RemoteValue.U[Phase].d > (wStandardVoltageConst * 100 * 105 / 100)))
	{
		return FALSE;
	}
	//电压不平衡在10Itr处检测，
	if( MeterTypesConst == METER_ZT )
	{
		//该相电流不在10Itr(Ib)附近 不补偿 0.95Ib - 1.05Ib
		if((RemoteValue.I[Phase].d < (wMeterBasicCurrentConst * 10 * 95 / 100))
		   ||(RemoteValue.I[Phase].d > (wMeterBasicCurrentConst * 10 * 105 / 100)))
		{
			return FALSE;
		}
	}
	else
	{
		//该相电流不在10Itr(50%Ib)附近 不补偿 0.95Ib - 1.05Ib
		if((RemoteValue.I[Phase].d < (wMeterBasicCurrentConst / 2 * 10 * 95 / 100))
		   ||(RemoteValue.I[Phase].d > (wMeterBasicCurrentConst / 2 * 10 * 105 / 100)))
		{
			return FALSE;
		}
	}
	
	//该相相角小于0.85 不补偿
	if(labs( RemoteValue.Cos[Phase + 1].l ) < 8500) //相角Cos[0]代表总相角 所以需要加一
	{
		return FALSE;
	}

	switch(Phase)
	{
		case 0: //A相
			//其他两相电压任意一相超过10% 不进行补偿
			if((RemoteValue.U[1].d > (wStandardVoltageConst * 100 * 10 / 100))
		     ||(RemoteValue.U[2].d > (wStandardVoltageConst * 100 * 10 / 100)))
			{
				return FALSE;
			}
			//其他两相电流任意一相超过10% 不进行补偿
			if((RemoteValue.I[1].d > (wMeterBasicCurrentConst * 10 * 10 / 100))
			 ||(RemoteValue.I[2].d > (wMeterBasicCurrentConst * 10 * 10 / 100)))
			{
				return FALSE;
			}
			break;
			
		case 1: //B相
			//其他两相电压任意一相超过10% 不进行补偿
			if((RemoteValue.U[0].d > (wStandardVoltageConst * 100 * 10 / 100))
		     ||(RemoteValue.U[2].d > (wStandardVoltageConst * 100 * 10 / 100)))
			{
				return FALSE;
			}
			//其他两相电流任意一相超过10% 不进行补偿
			if((RemoteValue.I[0].d > (wMeterBasicCurrentConst * 10 * 10 / 100))
			 ||(RemoteValue.I[2].d > (wMeterBasicCurrentConst * 10 * 10 / 100)))
			{
				return FALSE;
			}
			break;
			
		case 2: //C相
			//其他两相电压任意一相超过10% 不进行补偿
			if((RemoteValue.U[0].d > (wStandardVoltageConst * 100 * 10 / 100))
			 ||(RemoteValue.U[1].d > (wStandardVoltageConst * 100 * 10 / 100)))
			{
				return FALSE;
			}
			//其他两相电流任意一相超过10% 不进行补偿
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
//函数功能: 计算直流偶次谐波分段补偿系数
//
//参数: 	Type：
//				0：0.5L   1：1.0L
//			Slope：
//				一次函数斜率
//			Offset
//				一次函数偏置
//                   
//返回值:  
//
//备注:   
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
	//1点校正全部使用第一个点的补偿值
	if( EffectivePoint == 1 )
	{
		Slope[0] = 0;
		Offset[0] = (SWORD)(*(*Pointer));
		Slope[1] = 0;
		Offset[1] = (SWORD)(*(*Pointer));
	}//2点校正计算一条直线
	else if( EffectivePoint == 2 )
	{
		Data = (SWORD)(*(*(Pointer + 1))) - (SWORD)(*(*Pointer));
		Current = (SWORD)(*(*(Pointer + 1) + 1)) - (SWORD)(*(*(Pointer) + 1));
		Slope[0] = (float)Data / Current;
		Offset[0] = (SWORD)((SWORD)(*(*Pointer)) - (Slope[0] * (SWORD)(*(*(Pointer) + 1))));
		Slope[1] = Slope[0];
		Offset[1] = Offset[0];
	}//3点校正计算两条直线
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
//函数功能: 三相表分段补偿、无功补偿、温度补偿函数
//
//参数: 	无
//                   
//返回值:    无
//
//备注:   
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
		//计算直流偶次谐波增益系数
		CalHD2Para( 1, GainSlope, GainOffset );
		//计算直流偶次谐波相角系数
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
		
		//判断是否需要补偿或者取消补偿电压不平衡
		OpenUnBalance = FALSE;
		if(SelVolUnbalanceGain == YES)
		{
			if(((JudgeUnBalance( uci ) == TRUE)&&(UnBalanceGainFlag[uci]==FALSE))
			 ||((JudgeUnBalance( uci ) == FALSE)&&(UnBalanceGainFlag[uci]==TRUE)))
			{
				OpenUnBalance = TRUE;
			}
		}
		
		//判断是否需要开启半波补偿 
		if(( HalfWaveStatusFlag == TRUE )&& (MeterTypesConst == METER_ZT))
		{
			OpenHalfWave[uci] = FALSE;
			if(uci == 0)
			{
				//直流偶次谐波判断
				JudgeHalfWaveRegulate();
			}
			//如果上次的补偿状态和本次得到的状态不一致 开启一次补偿
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

		//获取补偿系数
		if( (ucDivNo[uci] != g_ucNowSampleDivisionNo[uci]) || (OpenUnBalance == TRUE) || (OpenHalfWave[uci] == TRUE) || (TmpCosGainFlag == TRUE))
		{		
			//分段补偿
			if(SelDivAdjust == YES)
			{
				//增益补偿
				Gain[uci] = SampleAdjustParaDiv.PGainDiv[uci][ucDivNo[uci]];
				if(Gain[uci] == 0) //如果该点系数为零 采用默认系数
				{
					Gain[uci] = (short)OriginalPara.PGainCoe[uci];
				}
				
				//相角补偿
				Coe[uci] = SampleAdjustParaDiv.PhsRegDiv[uci][ucDivNo[uci]];
				if(Coe[uci] == 0) //如果该点系数为零 采用默认系数
				{
					if( ( (ucDivNo[uci] < 1) && ( MeterCurrentTypesConst == CURR_100A ))
					 || ( (ucDivNo[uci] < 1) && ( MeterCurrentTypesConst == CURR_60A ) ) 
					 || ( (ucDivNo[uci] < 2) && ( MeterCurrentTypesConst == CURR_1A ) )
					 || ( (ucDivNo[uci] < 4) && ( MeterCurrentTypesConst == CURR_6A ) ) )  //大相角
					{
						Coe[uci] = (short)OriginalPara.AngCoe[0][uci];
					}
					else
					{
						Coe[uci] = (short)OriginalPara.AngCoe[1][uci];
					}
				}
			}
			//电压不平衡补偿
			if(SelVolUnbalanceGain == YES)
			{
				//只要处于电压不平衡状态 就要补偿 不然会被写回去
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
			//直流偶次谐波补偿
			if((HalfWaveStatusFlag == TRUE) && (MeterTypesConst == METER_ZT))
			{
				if(HalfWaveUpdateFlag[uci] != 0)
				{
					//1.0L增益补偿
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
					}//0.5L相角补偿
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
					//保存上一次补偿时的电流值
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
			//正向和反向有功进行整体偏移修正
			if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE ) //反向
			{
				if(ucDivNo[uci] < 9)
				{
					Gain[uci] += SampleManualModifyPara.ManualModifyForRA[ucDivNo[uci]];
				}
			}
			else //正向
			{
				if(ucDivNo[uci] < 9)
				{
					Gain[uci] += SampleManualModifyPara.ManualModifyForWard[ucDivNo[uci]];
				}
			}
			SampleSpecCmdOp( 0xa6 );
			//增益系数写入寄存器
			WriteSampleReg( w_GPA + uci, Gain[uci] );
			
			//相角系数写入寄存器
			WriteSampleReg( w_GphsA0 + uci*3, Coe[uci] );
			WriteSampleReg( w_GphsA1 + uci*3, Coe[uci] );
            WriteSampleReg( w_GphsA2 + uci*3, Coe[uci] );
			
			//无功修正
			TempGain = Gain[uci];
			TempGain += SampleManualModifyPara.ManualModifyQGain[ uci ][ ucDivNo[uci] ];
			WriteSampleReg( w_GPA + uci,(DWORD)TempGain); //修正系数写到无功寄存器
			SampleSpecCmdOp(0x00);
			bTmpFlag = TRUE;
		}
	}
	//上电两秒后并且有一相需要补偿时更新计量芯片校验和
	if((bySamplePowerOnTimer <= 28) && (bTmpFlag == TRUE))
	{
		//是否有检验和错误，避免其他计量芯寄存器错误未被发现
		if( FlagSampleChipSum != 0x55)
		{
			//更新计量芯片校验和
			GetSampleChipCheckSum();
		}
	}
}

//-----------------------------------------------
//函数功能: 刷新运动数据
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注:每个数据每秒刷新一次，一次大循环刷新一个数据,全部读一遍，耗时13.96ms（680kbps）
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
		case 0x00://断相、相序等标志状态,先读，刷新P,Q,S时会用	
			dwSampleSFlag = ReadSampleData( r_Noload );
			// 判断符号，后续电能用
			break;
		case 0x01://读有功功率 补码格式
			Link_ReadSampleRegCont( r_PA, 4, tdBuf);
			//功率单位W，保留0.001W,功率系数 K=2.592*10^10/(HFconst*EC*2^23)
			//此系数后面计算Q,S时还要用
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
					tl *=2;//功率需*2
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
						//设置合相反向标志
						api_SetSysStatus( eSYS_STATUS_OPPOSITE_P+i );
					}
					else if( tl > 0 )
					{
						api_ClrSysStatus( eSYS_STATUS_OPPOSITE_P+i );
					}
					if(i==0)
					{
						api_UptCurDemandStatus( 0, 1);//更新当前有功需量状态,放在更新标志之后
					}
					
				}
				else
				{
					if(i==0)
					{
						api_UptCurDemandStatus( 0, 0 );//更新当前有功需量状态,放在更新标志之后
					}
					tl = 0;
				}
				RemoteValue.P[i].l = tl;
				gfPQSaverage[0][i] += (float)RemoteValue.P[i].l;
				byPQSaverageCnt[0][i]++;
			}
			break;
		case 0x02://读无功功率 补码格式
			Link_ReadSampleRegCont( r_QA, 4, tdBuf);
			//功率单位W，保留0.001W,功率系数 K=2.592*10^10/(HFconst*EC*2^23)
			//此系数后面计算Q,S时还要用
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
					tl *=2;//功率需*2
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
						//设置合相反向标志
						api_SetSysStatus( eSYS_STATUS_OPPOSITE_Q+i );
					}
					else if( tl > 0 )
					{
						api_ClrSysStatus( eSYS_STATUS_OPPOSITE_Q+i );
					}
					if(i==0)
					{
						api_UptCurDemandStatus( 1, 1);//更新当前无功需量状态,放在更新标志之后
					}
					
				}
				else
				{
					if(i==0)
					{
						api_UptCurDemandStatus( 1, 0 );//更新当前无功需量状态,放在更新标志之后
					}
					tl = 0;
				}
				RemoteValue.Q[i].l = tl;
				gfPQSaverage[1][i] += (float)RemoteValue.Q[i].l;
				byPQSaverageCnt[1][i]++;
			}		
			break;
		case 0x03://读视在功率 补码格式
			Link_ReadSampleRegCont( r_SA, 4, tdBuf);
			//功率单位W，保留0.001W,功率系数 K=2.592*10^10/(HFconst*EC*2^23)
			//此系数后面计算Q,S时还要用
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
					tl *=2;//功率需*2
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
		case 0x04://读电压 补码格式
			Link_ReadSampleRegCont( r_RMSUA, 3, tdBuf);
			UpLimit = CalMaxVoltage();//最大限制
			DownLimit =  (DWORD)wCaliVoltageConst * 100 * 2 / 100;//最小限制@@@@@待更改
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
			//电压不平衡率
			RemoteValue.Uunbalance.w = CaleUnbalanceRate( 0, (long*)&RemoteValue.U[0].l );
			break;
		case 0x05://电流值 补码格式
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
			//电流不平衡率
			RemoteValue.Iunbalance.w = CaleUnbalanceRate( 1, (long*)&RemoteValue.I[0].l );
			//零线电流
			if(SelZeroCurrentConst == YES)
			{
				tdw = tdBuf[3];
				tf = (float)tdw * 10000 / TWO_POW13;
				//输入50mv时，Vrms/2^13=60
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
		case 0x06://读频率 补码格式
			tdw = ReadSampleData( r_FreqU );
			tdw = tdw * 100 / TWO_POW13;//保留2位小数	
			RemoteValue.Freq.d = tdw;	
			break;
		#endif
		case 0x07://读功率因数值 补码格式
			Link_ReadSampleRegCont( r_PFA, 4, tdBuf);
			for(i=0;i<4;i++)
			{
				tdw = tdBuf[i];
				//800000时应保证是负值，所以应该
				if( tdw >= TWO_POW23 )
				{
					tl = (long)tdw - TWO_POW24;
				}
				else
				{
					tl = tdw;
				}
				//保留4位小数
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
		case 0x08://读电压电流相角 补码格式
			Link_ReadSampleRegCont( r_PAUA, 6, tdBuf);
			for(i=0;i<3;i++)
			{
				tdw = tdBuf[i];	
				tf = (float)tdw * 10 / TWO_POW20;//保留1位小数
				RemoteValue.YUaUbUcAngle[i].l = (long)(tf*180);
			}
			for(i=0;i<3;i++)
			{
				tdw = tdBuf[3+i];	
				tf = (float)tdw * 10 / TWO_POW20;//保留1位小数
				tl = (long)(tf*180);
				tl = tl-RemoteValue.YUaUbUcAngle[i].l;
				RemoteValue.YUIAngle[i].l = (long)((tl % 3600) + 3600) % 3600;
			}
			break;
		#endif
		case 0x09://读正向有功电能 补码格式
			Link_ReadSampleRegCont( r_EPPA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			if(bySamplePulseNum != 0)
			{			
				api_WritePulseEnergy(PHASE_ALL+P_ACTIVE,bySamplePulseNum);// 正向
			}

			//更新需量累计用的的有功电能脉冲数增量及超起动门限时间增量
			//注，即使脉冲数增量为0，也要更新，因为超起动门限时间可能增加
			api_UpdateDemandEnergyPulse( 0, bySamplePulseNum );//使用高频脉冲
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				api_WritePulseEnergy(P_ACTIVE+(i<<12),bySamplePulseNum);	// 正向				
			}
			#endif
			break;
		case 0x10://读反向有功电能 补码格式
			Link_ReadSampleRegCont( r_ENPA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			if(bySamplePulseNum != 0)
			{			
				api_WritePulseEnergy(PHASE_ALL+N_ACTIVE,bySamplePulseNum);// 反向
			}

			//更新需量累计用的的有功电能脉冲数增量及超起动门限时间增量
			//注，即使脉冲数增量为0，也要更新，因为超起动门限时间可能增加
			api_UpdateDemandEnergyPulse( 0, bySamplePulseNum );//使用高频脉冲
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				api_WritePulseEnergy(N_ACTIVE+(i<<12),bySamplePulseNum);	// 反向				
			}
			#endif
			break;
		case 0x11://读正向无功电能 补码格式
			Link_ReadSampleRegCont( r_EPQA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			
			//无功为正，有功为正在一象限，有功为负在二象限，
			if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE) 
			{
				api_WritePulseEnergy(PHASE_ALL+RACTIVE2,bySamplePulseNum);
			}
			else
			{
				api_WritePulseEnergy(PHASE_ALL+RACTIVE1,bySamplePulseNum);	
			}
			//更新需量累计用的的无功电能脉冲数增量及超起动门限时间增量
			//注，即使脉冲数增量为0，也要更新，因为超起动门限时间可能增加
			api_UpdateDemandEnergyPulse( 1, bySamplePulseNum );
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				
				//无功为正，有功为正在一象限，有功为负在二象限
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
		case 0x12://读反向无功电能 补码格式
			Link_ReadSampleRegCont( r_ENQA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			
			//无功为负，有功为正在四象限，有功为负在三象限，
			if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE) 
			{
				api_WritePulseEnergy(PHASE_ALL+RACTIVE3,bySamplePulseNum);
			}
			else
			{
				api_WritePulseEnergy(PHASE_ALL+RACTIVE4,bySamplePulseNum);	
			}
			//更新需量累计用的的无功电能脉冲数增量及超起动门限时间增量
			//注，即使脉冲数增量为0，也要更新，因为超起动门限时间可能增加
			api_UpdateDemandEnergyPulse( 1, bySamplePulseNum );
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				
				//无功为负，有功为正在四象限，有功为负在三象限，
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
		case 0x13://读视在电能 补码格式
			Link_ReadSampleRegCont( r_ESA, 4, tdBuf);
			bySamplePulseNum = tdBuf[3];
			
			//视在电能方向跟有功电能方向一致
			if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE)
			{
				api_WritePulseEnergy(PHASE_ALL+APPARENT_N,bySamplePulseNum);
			}
			else
			{
				api_WritePulseEnergy(PHASE_ALL+APPARENT_P,bySamplePulseNum);
			}
			//更新需量累计用的的无功电能脉冲数增量及超起动门限时间增量
			//注，即使脉冲数增量为0，也要更新，因为超起动门限时间可能增加
			api_UpdateDemandEnergyPulse( 1, bySamplePulseNum );
			#if( SEL_SEPARATE_ENERGY == YES )	
			for(i=1;i<4;i++)
			{
				bySamplePulseNum = tdBuf[i-1];
				
				//视在电能方向跟有功电能方向一致
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
		case 14://温度
			tdw = ReadSampleData( r_TPSD );
			tdw&=0x00FF;
			//真实温度值 = -0.0028*TPS_data + 16；
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

	if( bySampleStep >= 0x40 )//寄存器地址不会大于0x40
	{
		bySampleStep = 0xff;
	}
	
	return;
}

//---------------------------------------------------------------
//函数功能: 刷新电流值 用于校表
//
//参数:     无
//                   
//返回值:   无
//
//备注:   
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
//函数功能: 采样寄存器设置为默认数值
//
//参数:		Type[in] 0--偏移类 1--控制类 2--增益类	 
//			
//返回值: 	无
//		    
//备注:
//-----------------------------------------------
void UseSampleDefaultPara(WORD Type)
{
	BYTE i;
	DWORD dwPara = 0;
	
	InitSampleCaliUI();
	
	if( Type == 0 )//偏移设置 TSampleOffsetPara
	{
		//CRC校验正确，校表初始化不初始化零线电流偏置，防止后面校表初始化此参数
		if( lib_CheckSafeMemVerify( (BYTE *)&SampleOffsetPara, sizeof(TSampleOffsetPara), UN_REPAIR_CRC ) == TRUE )
		{
           	for(i=0;i<(sizeof(SampleAdjustAddr)/sizeof(SampleAdjustAddr[0]));i++)//查找零线校准
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
		//使用默认值后不能更改CRC 否则下次校验不能从EEPROM恢复
		//SampleOffsetPara.CRC32 = lib_CheckCRC32((BYTE *)&SampleOffsetPara, (sizeof(TSampleOffsetPara) - sizeof(DWORD)) );
	}
	else if( Type == 1 )//控制设置 TSampleCtrlPara
	{
		memcpy( (BYTE*)&SampleCtrlPara, (BYTE*)&SampleCtrlDefData, sizeof(TSampleCtrlPara) );

		for(i = 0; i < (sizeof(SampleCtrlPara)/sizeof(DWORD)); i++)
		{
			if((SampleCtrlAddr[i] == w_PStart)||(SampleCtrlAddr[i] == w_QStart)||(SampleCtrlAddr[i] == w_HarStart))
			{
				//额定电压 Ub，基本电流 Ib，启动电流点 k‰,按照启动门限的60%设置
			//Pstartup=INT[0.6*Ub*Ib*HFconst*EC* k‰*2^23/(2.592*10^10)]		
				SampleCtrlPara.CtrlPara[i] = (WORD)((float)1.94180741 * (wStandardVoltageConst/10) * ((float)wMeterBasicCurrentConst / 1000) * HFCONST_DEF * (api_GetActiveConstant()) / 10000000);
				if( MeterTypesConst == METER_ZT )
				{
					//直通表起动为0.2%，其他表为0.1%
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
					SampleCtrlPara.CtrlPara[i] = 0x0000; //电压通道缺省1倍增益
				}		
				else if(wSampleVolGainConst == 2)
				{
					SampleCtrlPara.CtrlPara[i] = 0x0200; //电压通道缺省2倍增益
				}		
				else if(wSampleVolGainConst == 4)
				{
					SampleCtrlPara.CtrlPara[i] = 0x0400; //电压通道缺省8倍增益
				}		
				else if(wSampleVolGainConst == 8)
				{
					SampleCtrlPara.CtrlPara[i] = 0x0600; //电压通道缺省8倍增益
				}		
				else
				{
					SampleCtrlPara.CtrlPara[i] = 0x0200; //电压通道缺省2倍增益
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
				
				//零线电流ADC增益配置
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
				配置错误
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
					SampleCtrlPara.CtrlPara[i] |= (1<<6);//B相不参与合相电流计算
					SampleCtrlPara.CtrlPara[i] |= (1<<10);//B相不参与合相电压计算
					SampleCtrlPara.CtrlPara[i] |= (3<<14);//B相不参与合相功率计算
				}
			}
		}
		
	}
	else//TSampleAdjustPara
	{
		//CRC校验正确，校表初始化不初始化零线电流增益，防止后面校表初始化此参数
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
		//使用默认值后不能更改CRC 否则下次校验不能从EEPROM恢复
		//SampleAdjustPara.CRC32 = lib_CheckCRC32((BYTE *)&SampleAdjustPara, (sizeof(TSampleAdjustPara) - sizeof(DWORD)) );
	}
}


//-----------------------------------------------
//函数功能: 检查RAM中校正寄存器数据是否正确
//
//参数:		无 
//			
//返回值: 	无
//		    
//备注:
//-----------------------------------------------
static void CheckSampleRamPara( void )
{
	WORD Result,Flag;
	
	Flag = 0;
	
	//校验偏移系数
	if(SampleOffsetPara.CRC32 != lib_CheckCRC32((BYTE *)&SampleOffsetPara, (sizeof(TSampleOffsetPara) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleOffsetPara), sizeof(TSampleOffsetPara), (BYTE*)&SampleOffsetPara);
		if( Result != TRUE )
		{
			UseSampleDefaultPara(0);
			Flag = 0x55;
		}
	}

	//校验芯片控制系数
	if(SampleCtrlPara.CRC32 != lib_CheckCRC32((BYTE *)&SampleCtrlPara, (sizeof(TSampleCtrlPara) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlPara), sizeof(TSampleCtrlPara), (BYTE*)&SampleCtrlPara);
		if( Result != TRUE )
		{
			UseSampleDefaultPara(1);
			Flag = 0x55;
		}
	}
	
	//校验调校系数
	if(SampleAdjustPara.CRC32 != lib_CheckCRC32((BYTE *)&SampleAdjustPara, (sizeof(TSampleAdjustPara) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustPara), sizeof(TSampleAdjustPara), (BYTE*)&SampleAdjustPara);
		if( Result != TRUE )
		{
			UseSampleDefaultPara(2);
			Flag = 0x55;
		}
	}
	
	//校验精调系数
	if(SelDivAdjust == YES)
	{
		//校验精调默认系数
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
	
	//校验手动补偿系数
	if(SampleManualModifyPara.CRC32 != lib_CheckCRC32( (BYTE *)&SampleManualModifyPara, (sizeof(TSampleManualModifyPara) - sizeof(DWORD)) ))
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleManualModifyPara ), sizeof(TSampleManualModifyPara), (BYTE *)&SampleManualModifyPara );
		if(Result != TRUE)
		{
			memset( (BYTE *)&SampleManualModifyPara, 0x00, sizeof(TSampleManualModifyPara) );
		}
	}
	
	//校验直流偶次谐波系数
	if(SampleHalfWavePara.CRC32 != lib_CheckCRC32( (BYTE *)&SampleHalfWavePara, (sizeof(TSampleHalfWavePara) - sizeof(DWORD)) ))
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( SampleSafeRom.SampleHalfWavePara ), sizeof(TSampleHalfWavePara), (BYTE *)&SampleHalfWavePara );
		if(Result != TRUE)
		{
			memcpy( (BYTE *)&SampleHalfWavePara, (BYTE *)SampleHalfWaveDefData, (sizeof(TSampleHalfWavePara) - sizeof(DWORD)) );
		}
	}
	
	//校验直流偶次谐波上电检测时间参数
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
//函数功能: 写采样芯片全部偏移类寄存器
//
//参数:		无 
//			
//返回值: 	返回TRUE/FALSE
//		    
//备注:
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
//函数功能: 写采样芯片全部控制类寄存器
//
//参数:		无 
//			
//返回值: 	返回TRUE/FALSE
//		    
//备注:
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
//函数功能: 写采样芯片全部增益类寄存器
//
//参数:		无 
//			
//返回值: 	返回TRUE/FALSE
//		    
//备注:
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
//函数功能: 复位采样芯片
//
//参数:		无 
//			
//返回值: 	无
//		    
//备注:直接控制电源
//-----------------------------------------------
static void HardResetSampleChip(void)
{		
	//钜泉推荐复位为10ms，但手册上没写，在低温上电时，时间短可能复位不成功
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 10 );
	
	CLEAR_WATCH_DOG;
}


//-----------------------------------------------
//函数功能: 效验采样芯片寄存器
//
//参数:		无 
//			
//返回值: 	无
//		    
//备注:上电时还没读到效验，上电不要调用此函数，每分钟的时候调用即可
//-----------------------------------------------
static void CheckSampleChipReg(void)
{
	DWORD tdw1, tdw2;
	BYTE i,Flag,Result = TRUE;

	//掉电判断
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )	
	{
		return;
	}	
	
	Flag = 0;
	
	if( CheckSampleChipCheckSum() == FALSE )
	{
		// 应该记录告警事件
		api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);

		Flag = 0x55;		
	}

	//如果出现三相电压为0，则认为此时采样芯片异常复位，需要重新初始化
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
			
			// 应该记录告警事件
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
		//关闭7038写保护
		SampleSpecCmdOp(0xa6);
		Result = WriteSampleChipOffsetPara();	
		Result &= WriteSampleChipAdjustPara();	
		Result &= WriteSampleChipCtrlPara();
		//打开7038写保护
		SampleSpecCmdOp(0x00);
		if(Result == TRUE)
		{
			//校验和异常时，更新效验和
			GetSampleChipCheckSum();
		}
		else
		{
			// 应该记录告警事件
			api_WriteSysUNMsg(INIT_SAMPLE_CHIP_FAIL);
		}
		//芯片检查出错 分段补偿重启补偿
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
//函数功能: 计量芯片通信故障检查
//
//参数:		无
//
//返回值: 	无
//
//备注:
//-----------------------------------------------
static void CheckEmuErr( void )
{
	static BYTE EmuErrCnt = 0;
	DWORD RegData;
	BYTE i;

	//读取配置寄存器的方式判断记量芯片故障 
	for(i = 0; i < (sizeof(SampleCtrlPara)/sizeof(DWORD)); i++)
	{

		//不能使用判断校验和的方式 因为SPI没有校验 如果SPI总线短接到VCC或者GND 读出来的数据是固定的
		//校验和也会被刷新为全FF或者全00 导致故障无法被检出
		RegData = ReadSampleData( SampleCtrlAddr[i] );
		if(RegData != SampleCtrlPara.CtrlPara[i])
		{
			//检查所有控制寄存器
			CheckSampleRamPara();
			if(EmuErrCnt < 40)
			{
				EmuErrCnt++;		
				//如果连续20次出现HFConst值不对应 重新校验一下HFConst
				//第一防止RAM中数据出现错误 第二如果计量芯片出现复位 每分钟5秒才会进行寄存器校验 会出现误报的问题
				if(EmuErrCnt == 20)
				{
					HardResetSampleChip();//硬复位
					api_InitSampleChip();//初始化计量芯片
				}
				
				//连续30次错误 记录计量芯片故障
				if(EmuErrCnt == 30)
				{
					api_SetError( ERR_CHECK_5460_1 ); //置计量芯片通信故障
				}
			}
		}
		else
		{
			//如果恢复了 并且之前连续出现了10次以上的错误 重新检查一下所有寄存器数据 内部有api_ClrError
			if(EmuErrCnt > 10)
			{
				CheckSampleChipReg();
			}
			EmuErrCnt = 0; 
		}
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
static BOOL CheckSampleChipSum(void)
{

	if( CheckSampleChipCheckSum() ==FALSE )
	{
		// 应该记录告警事件
		api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);

		return FALSE;		
	}
	else
	{
		return TRUE;
	}
}

//-----------------------------------------------
//函数功能:开启采样
//
//参数: 无
//
//返回值:  无
//
//备注:
//-----------------------------------------------
static void StartHarmonicSample(void)
{
	//打开写允许
	SampleSpecCmdOp(0xa6);
    
	//api_Delay10us(1);
	//开启A、B、C三相电流采样
	WriteSampleReg(w_Buffer2CFG, 0<<10 | 0<<9 | 0<<8 | 0x18);					//Buffer2配置寄存器（缓存数据使能、大端、单次模式、缓存类型=ADC波形数据）
	WriteSampleReg(w_Buffer2CFG, 1<<10 | 0<<9 | 0<<8 | 0x18);					//Buffer2配置寄存器（缓存数据使能、大端、单次模式、缓存类型=ADC波形数据）
	api_Delay10us(1);

	//打开写禁止
	SampleSpecCmdOp(0x00);
}
//-----------------------------------------------
//函数功能:判断缓存数据是否准备完全
//
//参数: 无
//
//返回值:  无
//
//备注:
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
//函数功能:获取w_WaveData中缓存的所有数据
//
//参数: 	
//
//返回值:  无
//
//备注: 此函数缓存长度为1024个字节 扩充要查看上层调用缓存长度是否够用
//-----------------------------------------------
static BOOL GetAllADCData( SWORD *sADCBuf )
{
	BYTE i;

	
	if(IsSampleDataReady() == TRUE)
	{
		//关闭7038写保护
		SampleSpecCmdOp(0xa6);
		WriteSampleReg(w_SPICFG, 0x3A);//SPI配置寄存器（连读模式下固定地址）
		//打开7038写保护
		SampleSpecCmdOp(0x00);
		//for(i=0;i<2;i++)
		{
			ReadSampleBuffCont(r_Buffer2, 512, &sADCBuf[0]);		//3通道64点16bit/32bit
			ReadSampleBuffCont(r_Buffer2, 512, &sADCBuf[512]);		//3通道64点16bit/32bit
		}//关闭7038写保护
		SampleSpecCmdOp(0xa6);
		WriteSampleReg(w_SPICFG, 0x38);//SPI配置寄存器（连读模式下固定地址）
		//打开7038写保护
		SampleSpecCmdOp(0x00);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------
//函数功能: 计算谐波实部和虚部电流
//
//参数: 	RICurrent:
//				A、B、C相二次谐波实部和虚部电流
//			ADCBuf:
//				三相电流ADC采样点
//			Cos:
//				余弦波相关函数
//			Sin:
//				正弦波相关函数
//
//返回值:  无
//
//备注:   
//---------------------------------------------------------------
static void CalHarmonic( double *RICurrent, SWORD *ADCBuf, const SWORD *Cos, const SWORD *Sin )
{
	double SumCos = 0, SumSin = 0;			//谐波实部电流、虚部电流
	WORD i = 0, j = 0;
	
	CLEAR_WATCH_DOG;
	
	//计算谐波实部电流和虚部电流
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < AD_ONEWAVE_SIMPLE_NUM; j++)
		{
			SumCos += Cos[j] * ADCBuf[i + j * 3];
			SumSin += Sin[j] * ADCBuf[i + j * 3];
		}
		// / 1000 / 72 * 2 / 2^13 * 2^8 / 1.4142
		SumCos /= 1000;
		SumCos = SumCos / AD_ONEWAVE_SIMPLE_NUM * 2;			//除(N/2) N为采样点数
		SumCos = SumCos / ((double)(6 * CURRENT_VALUE) / 5) * ((double)wMeterBasicCurrentConst / 1000);
		SumCos /= 32;
		SumCos = SumCos / 1.4142;
		if( SumCos < 0 )
		{
			SumCos *= -1;
		}
		RICurrent[i * 2] = SumCos * 10000;
		
		SumSin /= 1000;
		SumSin = SumSin / AD_ONEWAVE_SIMPLE_NUM * 2;			//除(N/2) N为采样点数
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
//函数功能:计算并判断是否处于半波状态
//
//参数: 	
//返回值: eAD_HALFWAVE_NO - 不是半波，eAD_HALFWAVE_YES - 是半波， eAD_DATA_NO_REFRESH - 数据未更新结束
//
//备注:计算谐波含量
//-----------------------------------------------
static eGetHalfWaveStatus CalculateHalfWaveStatus1( SWORD *sADCBuf )
{
	double HD2Curr[3][2];								//基波、二次谐波实部和虚部电流
	DWORD SumCurr[3];									//基波、二次谐波有效值
	double FirRate[3], HD2Rate[3];						//基波、二次谐波含量
	static double LastFirRate[3] = { 0.0, 0.0, 0.0 };	//上一次的基波含量
	static double LastHD2Rate[3] = { 0.0, 0.0, 0.0 };	//上一次的二次谐波含量
	long FirRateDiff[3], HD2RateDiff[3];				//当前与上一次的谐波差值
	BYTE i = 0, j = 0;

	CLEAR_WATCH_DOG;
	
	//检测三个周波
	for(i = 0; i < 3; i++)
	{
		//获取二次谐波实部和虚部电流
		CalHarmonic( &HD2Curr[0][0], &sADCBuf[i * 300 + 30], TAB_Cos2, TAB_Sin2 );
		//计算二次谐波有效值
		SumCurr[0] = (DWORD)sqrt(HD2Curr[0][0] * HD2Curr[0][0] + HD2Curr[0][1] * HD2Curr[0][1]);
		SumCurr[1] = (DWORD)sqrt(HD2Curr[1][0] * HD2Curr[1][0] + HD2Curr[1][1] * HD2Curr[1][1]);
		SumCurr[2] = (DWORD)sqrt(HD2Curr[2][0] * HD2Curr[2][0] + HD2Curr[2][1] * HD2Curr[2][1]);
		//计算二次谐波含量(百分数)
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
		//计算当前与上一次的二次谐波差值
		HD2RateDiff[0] = labs( (long)(HD2Rate[0] - LastHD2Rate[0]) );
		HD2RateDiff[1] = labs( (long)(HD2Rate[1] - LastHD2Rate[1]) );
		HD2RateDiff[2] = labs( (long)(HD2Rate[2] - LastHD2Rate[2]) );
		//保留上一次的二次谐波含量
		LastHD2Rate[0] = HD2Rate[0];
		LastHD2Rate[1] = HD2Rate[1];
		LastHD2Rate[2] = HD2Rate[2];
		
		CLEAR_WATCH_DOG;
		
		//获取基波实部和虚部电流
		CalHarmonic( &HD2Curr[0][0], &sADCBuf[i * 300 + 30], TAB_Cos1, TAB_Sin1 );
		//计算基波有效值
		SumCurr[0] = (DWORD)sqrt(HD2Curr[0][0] * HD2Curr[0][0] + HD2Curr[0][1] * HD2Curr[0][1]);
		SumCurr[1] = (DWORD)sqrt(HD2Curr[1][0] * HD2Curr[1][0] + HD2Curr[1][1] * HD2Curr[1][1]);
		SumCurr[2] = (DWORD)sqrt(HD2Curr[2][0] * HD2Curr[2][0] + HD2Curr[2][1] * HD2Curr[2][1]);
		//计算基波含量(百分数)
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
		//计算当前与上一次的基波差值
		FirRateDiff[0] = labs( (long)(FirRate[0] - LastFirRate[0]) );
		FirRateDiff[1] = labs( (long)(FirRate[1] - LastFirRate[1]) );
		FirRateDiff[2] = labs( (long)(FirRate[2] - LastFirRate[2]) );
		//保留上一次的基波含量
		LastFirRate[0] = FirRate[0];
		LastFirRate[1] = FirRate[1];
		LastFirRate[2] = FirRate[2];
		
		//如果二次谐波含量三相全都不在范围内，则认为不是半波
		if(((HD2Rate[0] < HD2ContentMin) || (HD2Rate[0] > HD2ContentMax))
		   && ((HD2Rate[1] < HD2ContentMin) || (HD2Rate[1] > HD2ContentMax))
		   && ((HD2Rate[2] < HD2ContentMin) || (HD2Rate[2] > HD2ContentMax)))
		{
			return eAD_HALFWAVE_NO;
		}
		//如果二次谐波差值任意一相大于最大差值, 则认为不是半波
		if((HD2RateDiff[0] > HD2DiffValue) 
		   || (HD2RateDiff[1] > HD2DiffValue) 
		   || (HD2RateDiff[2] > HD2DiffValue))
		{
			return eAD_HALFWAVE_NO;
		}
		//如果基波差值任意一相大于最大差值, 则认为不是半波
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
//函数功能:判断是否处于半波状态
//
//参数: 	
//返回值:  eAD_HALFWAVE_NO - 不是半波，eAD_HALFWAVE_YES - 是半波， eAD_DATA_NO_REFRESH - 数据未更新结束
//
//备注:通过峰峰值比较的方式,通过观测数据得，在42.4A附近，上峰值与下峰值差大于2000
//-----------------------------------------------
/*
static eGetHalfWaveStatus JudgeHalfWaveStatus2(void)
{
	BYTE i,j;
	BYTE bWaveNum;
	SWORD sBigData,sSmallData;
	
	if(GetAllADCData() != TRUE)//如果波形没有收满或未刷新，则返回失败
	{
		return eAD_DATA_NO_REFRESH;
	}

	CLEAR_WATCH_DOG;

	//周波计数清零
	bWaveNum = 0;	

	for(i = 0; i < (AD_BUFFER_NUM/AD_ONEWAVE_SIMPLE_NUM); i++)//整波形数进行判断
	{
		sBigData = 0;
		sSmallData = 0;
		
		for(j = 0; j < AD_ONEWAVE_SIMPLE_NUM ; j++)//对72个点进行比较,寻找最大值及最小值
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
//函数功能:根据电流负载及COS数据判断是否进入半波ADC数据采样及分析
//
//参数: 	
//
//返回值:  无
//
//备注:
//-----------------------------------------------
static BOOL CheckHalfWaveStatus( void )
{
	//直流和偶次谐波实验电流范围为5-72A, 半波整流后电流范围为2.5-36A
	//功率因数有总 电流没有 所有Cos[1]为A相功率因数 I[0]为A相电流 不要用错
	//三相电流任意一相大于1A才会判断是否处于半波状态
	if( (MeterCurrentTypesConst == CURR_60A) 
		&& ((labs(RemoteValue.I[0].l) > HD2CheckCurrentValueMin) 
			|| (labs(RemoteValue.I[1].l) > HD2CheckCurrentValueMin) 
			|| (labs(RemoteValue.I[2].l) > HD2CheckCurrentValueMin)) )
	{
		return TRUE;
	}
	//目前没有100的表 电流值还需进一步测试@@@@
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
//函数功能: 判断是否起动半波补偿
//
//参数: 
//                   
//返回值:  
//
//备注:   
//---------------------------------------------------------------
static BOOL JudgeHalfWaveRegulate( void )
{
	SWORD sADCBuf[1028]; //理论1024就够 多留4个字节
	eGetHalfWaveStatus bHalfWaveStatus1;                        //当前波形特征
    static BOOL StartADFlag = FALSE, ADFalseCount = 0;
	BYTE i = 0;
	
	bHalfWaveStatus1 = eAD_HALFWAVE_NO;
	memset (sADCBuf,0x00,sizeof(sADCBuf));
	if( CheckHalfWaveStatus() == TRUE)
	{
		//从开启AD 到AD数据获取 经过一次秒任务
		if( StartADFlag == TRUE )
		{
			if( GetAllADCData( sADCBuf ) == TRUE ) //如果波形没有收满，则认为不是半波
			{
				StartADFlag = FALSE; //波形收取完毕 马上开启下一次AD
				ADFalseCount = 0;
				
				bHalfWaveStatus1 = CalculateHalfWaveStatus1( sADCBuf );
				//bHalfWaveStatus2 = JudgeHalfWaveStatus2();
				
				if(bHalfWaveStatus1 == eAD_HALFWAVE_YES)
				{
					//连续两次判断是半波，开启补偿
					if(bHalfWaveCalTime < 5)
					{
						bHalfWaveCalTime++;
					}
				}
				else
				{
					//如果检测到非半波，将bHalfWaveCalTime计数器清零，退出半波补偿系数，并重新计算计量芯片校验
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
					//如果出现多次错误 重新开启一次AD
					StartADFlag = FALSE;
					
					//如果AD三次获取失败 并且前一次是补偿状态 将计数器减一 防止AD一直失败在补偿里出不来
					if(bHalfWaveCalTime >= 2)
					{
						bHalfWaveCalTime--;
					}
					else//如本身不在补偿状态 直接清零从新计算
					{
						bHalfWaveCalTime = 0;
					}
				}
			}
		}
		
		if( StartADFlag == FALSE ) //一秒开启一次AD
		{
			//直流偶次谐波补偿AD采样
			//刷新一次采样
			StartHarmonicSample();
			StartADFlag = TRUE;
			ADCSampleCount = 0;
		}		
		
		//连续两次是半波 开启补偿
		if(bHalfWaveCalTime >= 2)
		{
			for(i = 0; i < 3; i++)
			{
				//如果电流值大于最小判定值
				if( labs( RemoteValue.I[i].l ) > HD2CheckCurrentValueMin )
				{
					//1.0L补偿
					if( labs( RemoteValue.Cos[i + 1].l ) > HD2CosLimitValue )
					{
						HalfWaveUpdateFlag[i] = 2;
					}//0.5L补偿
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
//	//读取采样数据--------------------------------------------------------------
//	for (cycle=0; cycle<2; cycle++)											//两周波
//	{
//		if ((cycle == 1)
//		&& (pSta[0] == FALSE)												//第一轮检测不在工况
//		&& (pSta[1] == FALSE)
//		&& (pSta[2] == FALSE))
//		{
//			break;
//		}
//		Read_Reg_Cont(r_Buffer2, 3*C_SampNum/2, (INT16U*)&samp[0][0]);		//3通道64点16bit/32bit
//		for (phs=0; phs<C_PhsNum; phs++)
//		{
//			if (pSta[phs] == FALSE)											//之前检测不在工况
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
//			sum1c >>= 16;													//缩位,以便做运算
//			sum1s >>= 16;
//			sum2c >>= 16;
//			sum2s >>= 16;
//			sum1 = sum1c*sum1c + sum1s*sum1s;
//			sum2 = sum2c*sum2c + sum2s*sum2s;
//			sum1 = sqrt(sum1);
//			sum2 = sqrt(sum2);
//			rate = 0;
//			curr = (float)sum1/1000*65536/C_SampNum *2 * 256 /1.4142 * TAB_MeterRat.curr;//正弦余弦表扩大1000、前述运算缩小16位、ADC数据缩小8位
//			if (curr > TAB_HarLvl.CurrLower)
//			{
//				rate = (float)sum2 / sum1;									//二次谐波含有量
//			}
//			if ((rate <= TAB_HarLvl.RateLower)								//二次谐波含量下限
//			|| (rate >= TAB_HarLvl.RateUpper))								//二次谐波含量上限
//			{
//				pSta[phs] = FALSE;
//			}
//			if (pSta[phs] == TRUE)											//当前工况判断还有效
//			{
//				if (cycle == 0)												//第一次采样,保存基波值
//				{
//					fund[phs] = curr;
//				}
//				else														//第二次采样,与上一次作比较
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
//	Write_Reg(w_Buffer2CFG, 0<<10 | 0<<9 | 0<<8 | 0x18);					//Buffer2配置寄存器（缓存数据使能、大端、单次模式、缓存类型=ADC波形数据）
//	Write_Reg(w_Buffer2CFG, 1<<10 | 0<<9 | 0<<8 | 0x18);					//Buffer2配置寄存器（缓存数据使能、大端、单次模式、缓存类型=ADC波形数据）
//}

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
		//上电30s后才起动效验，避免还没读出采样芯片的效验和
		if( bySamplePowerOnTimer == 0 )
		{
			CheckSampleRamPara();			
			CheckSampleChipReg();
		}

		#if( SEL_STAT_V_RUN == YES )//计算电压合格率参数		
		StatVRunTime();
		#endif
	}
	
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )
	{
		//每分钟计算一次平均功率
		CalPQSaverage();
		//每分钟再初始化一下，基本电压、基本电流这两个参数在全失压中会用
		InitSampleCaliUI();
		
		#if( SEL_AHOUR_FUNC == YES )
		if(t.Min == 26)
		{
			SwapAHour(); 
		}
		#endif
		
		//直流偶次谐波--每分钟累计上电检测时间
		//如果为公钥则一直开启直流偶次谐波
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
			}//如果为私钥
			else
			{
				//一直关闭
				if( SampleHalfWaveTimePara.Hours == 0 )//非直通表
				{
					HalfWavePowerOnTime = 0;
					HalfWaveStatusFlag = FALSE;
				}//一直开启
				else if( SampleHalfWaveTimePara.Hours == 0xFF )
				{
					HalfWavePowerOnTime = 0;
					HalfWaveStatusFlag = TRUE;
				}//时间到关闭
				else
				{
					//如果累计时间小于设定时间, 时间累加, 开启判断
					if( HalfWavePowerOnTime < (SampleHalfWaveTimePara.Hours * 60) )
					{
						HalfWavePowerOnTime++;
						HalfWaveStatusFlag = TRUE;
					}//累计时间大于等于设定时间, 关闭判断
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
		
		//采样芯片计算效验需要一点时间，所以延时一会读取
		if(bySamplePowerOnTimer == 28)
		{
			GetSampleChipCheckSum();
		}
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
	
	//分段补偿 温度补偿 无功补偿
	SampleDivisionRegulate();
	
	//计量芯片故障检查
	CheckEmuErr();
	
	#if( SEL_AHOUR_FUNC == YES )
	AccumulateAHour();
	#endif
}
//全失压用电流刷新
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
	//零线电流
	if(SelZeroCurrentConst == YES)
	{
		tdw = tdBuf[3];
		tf = (float)tdw * 10000 / TWO_POW13;
		//输入50mv时，Vrms/2^13=60
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
	BOOL Result;
	BYTE i;
	
	CheckSampleRamPara();
	//WriteSampleReg(w_SRSTREG,  0xA5);						//软复位EMU
	//关闭7038写保护
	SampleSpecCmdOp(0xa6);
	//初始化缓存输出------------------------------------------------------------
	WriteSampleReg(w_SPICFG, 0x38);//SPI配置寄存器（连读模式下连续地址）
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
	
	//芯片自动温度补偿初始化
	if(SelMChipVrefgain == YES)
	{
		InitMChipVrefgain();
	}
	
	//打开7038写保护
	SampleSpecCmdOp(0x00);
	
	//校验和异常时，更新效验和
	GetSampleChipCheckSum();
	//上电分段补偿初始化
	if(SelDivAdjust == YES)
	{
		memset( g_ucNowSampleDivisionNo, 0xFF, sizeof(g_ucNowSampleDivisionNo) );
	}
	
	return Result;
}
void InitHarmonicSample(void)
{
	ADCSampleCount = 10;								//计量芯片ADC采样时间计数器
	HalfWavePowerOnTime = 0;							//直流偶次谐波上电检测累计时间(分钟)
	HalfWaveStatusFlag = TRUE;							//直流偶次谐波状态标志  TRUE: 开启判断  FALSE: 关闭判断
	//关闭7038写保护
	SampleSpecCmdOp(0xa6);
	//初始化缓存输出------------------------------------------------------------
	WriteSampleReg(w_DataChnCFG, (7<<11 | 0<<15 | 1<<19));						//数据通道选择寄存器（3路电流波形数据、数据源=SPL波形数据、数据位宽=16bit）
	WriteSampleReg(w_Buffer2CFG, (1<<10 | 0<<9 | 0<<8 | 0x18));					//Buffer2配置寄存器（缓存数据使能、大端、单次模式、缓存类型=ADC波形数据）
	WriteSampleReg(w_Buffer2_AddrLen, 0x200400);									//Buffer2起始地址和长度设置寄存器（起始地址=0x200、数据长度=0x31D）
	WriteSampleReg(w_UnRLVL_NullNum2, (216<<12));								//Buffer2未读数据阈值与缓存预留长度设置寄存器（2周波、72点/周波、3通道、16bit/32bit）
	//打开7038写保护
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
	bySamplePowerOnTimer = 30;
	byReWriteSampleChipRegCount = 0;
	dwSampleSFlag = 0;
	bySampleStep = 0xff;		
	byPowerOnFlag = 0x55;

	#if( SEL_STAT_V_RUN == YES )//恢复电压合格率
	PowerUpVRunTime( );
	#endif
	
	InitSampleCaliUI();
	
	api_InitSampleChip();
	InitHarmonicSample();
	//上电读出电压 电流 功率
	PowerUpReadVoltage();
    byPowerOnFlag = 0;
	//WriteSampleReg( w_CFxRun, 0xFFFFFF );
	#if( SEL_AHOUR_FUNC == YES )
	PowerUpAHour();
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
	memset((BYTE*)&RemoteValue,0x00,sizeof(TRemoteValue));
	

}

//-----------------------------------------------
//函数功能: 	记录上电电压
//
//参数:		无 
//                 
//返回值:	 	无
//		   
//备注:   
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
	
		if( Vlotage.w > VlotageBak.w )//计算上电最大电压
		{
			VlotageBak.w = Vlotage.w;
			VlotageBak.b[1] &= 0x3f;//高2位为相限
			VlotageBak.b[1] |= (i<<6);//高2位为相限
		}
	}
	//记录上电电压
	api_WriteFreeEvent( EVENT_SYS_START_VOLTAGE, VlotageBak.w );
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
//				4:N线零线				PHASE_N
//				5:N线零序				PHASE_N2	
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
			//判断零线电流，与零序电流开关有没有打开
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
	BYTE PQDirect;
	WORD i,tw;
	long tl;
	
	tw = 0; 
	
	switch(Type)
	{
		case 0x00://电压不平衡率
			tw = RemoteValue.Uunbalance.w;
			break;
		case 0x01://电流不平衡率
			tw = RemoteValue.Iunbalance.w;
			break;
		case 0x02://负载率
			tl = labs(RemoteValue.P[0].l);
			tw = tl / (wStandardVoltageConst/10) / (dwMeterMaxCurrentConst / 100) / 3;
			break;
		case 0x10:
			//电压状态	1--低于0.6Un	0--高于0.6Un
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
						
			//电压相序	1--电压逆相序	0--电压正常
			if( dwSampleSFlag & 0x00000008 )
			{
				tw |= 0x8000;
			}

			break;
		case 0x11:
			//电流状态	1--低于5%Ib	0--高于5%Ib
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
			
			//电流相序	1--电流逆相序	0--电流正常
			if( dwSampleSFlag & 0x00000010 )
			{
				tw |= 0x8000;
			}

			break;
		case 0x20://起动潜动状态
			//bit9,bit10,bit11分别代表ABC三相状态 0--起动 1--潜动
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
				//有功功率的方向
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P + i) == TRUE )
				{
					PQDirect |= (0x01 << i);
				}
			}
			for(i=0; i<4; i++)
			{
				//无功功率的方向
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_Q + i) == TRUE )
				{
					PQDirect |= (0x10 << i);
				}
			}
		
			//高位是无功 低位是有功 0是正 1是负
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

		case 0x40://视在功率判断
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
