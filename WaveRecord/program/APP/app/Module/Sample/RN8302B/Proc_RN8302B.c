//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.7.30
//描述		采样芯片RN2026处理主文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "RN8302B.h"
#include "AHour.h"
//#include "statistics_voltage.h"

#if( SAMPLE_CHIP == CHIP_RN8302B )


//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eUpDateSumCheck,
	eSumCheck
}eCheckType;
#pragma pack(1)
typedef struct 
{
	eIntegratorMode eintegratormode;
	eMeasurementMode emeasurementmode;
	DWORD CRC32;
}SampleRunPara;
#pragma pack()
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
TRemoteValue					RemoteValue;				//运动数据 即瞬时量数据
TSampleCorr						SampleCorr;				//瞬时量系数
TSampleAdjustReg    			SampleAdjustReg;			//计量芯片校表类数据
TSampleCtrlReg      			SampleCtrlReg;			//计量芯片控制类数据
TCurrenttValue					CurrentValue[9];			//分段补偿滞回区间上下限
TSampleAdjustParaDiv 			SampleAdjustParaDiv;		//分段补偿数据
TSampleManualModifyPara			SampleManualModifyPara;		//手动修正系数
TOriginalPara					OriginalPara;				//精调默认增益和大小相角系数 如果某个点未校 默认采用该系数
TEDTDataPara 			        EDTDataPara;		        //EDT基准参数
BYTE							byHarEnergyPulse;			//新增正向谐波脉冲个数
BYTE 							bySampleStep;				//大循环刷新远动数据协调器，每秒读一次，每次在大循环多次刷新各个远动数据
TExtremumMax 					ExtremumMax;
TExtremumMin 					ExtremumMin;
TExtremumMax 					ExtremumMaxHis;				//冻结0点转存数据
TExtremumMin 					ExtremumMinHis;
TXtalConRom						XtalPara;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
TPAverage		Paverage[3][4];					//平均功率

DWORD 			VMAXLimit;						//电压上限保护值
DWORD 			IMINLimit;						//电流下限保护值
DWORD 			IMAXLimit;						//电流上限保护值
BYTE 			bySamplePowerOnTimer;			//上电倒计时计数器
BYTE 			byReWriteSampleChipRegCount;	//判断芯片效验不对回写计数器

DWORD 			dwSamplePFlag;					//有功、无功方向
DWORD			dwSampleSFlag;					//潜动，起动标志
BOOL 			UnBalanceGainFlag[3];			//电压不平衡补偿标志 TRUE-已补偿 FALSE-未补偿
BYTE			TopoErrFlag = 0;
//分段补偿有关参数
BYTE 			g_ucNowSampleDivisionNo[MAX_PHASE];		//分段补偿当前段区
BYTE 			ChecksumFlag;
static DWORD 	dwtempMaxU[3];
static DWORD 	dwtempMinU[3];
static DWORD 	dwtempMaxI[3];
static DWORD 	dwtempMinI[3];
static BYTE RecoverNum = 0;
static QWORD 	_success_cnt_iap[2] = {0, 0};
static QWORD 	_fail_cnt_iap[2] = {0, 0};
const DWORD bLock3[2]= {0x000000EA,0x00000000};
SampleRunPara samplerunpara={
	.eintegratormode=eUnuseIntegrator,
	.emeasurementmode=eThreePhase,
	.CRC32=0,
};
const DWORD SampleCtrlUnuseIntegrator[][3]=
{
	{ROS_CFG,0x00000000,4},
};
const DWORD SampleCtrlUseIntegrator[][3]=
{
	{ROS_DCATTC,0x00007fdf,4},
	{ROS_CFG,0x00000007,4},
};
const DWORD SampleCtrlOnePhase[][3]=
{
	{EMUCON_EMU,0x00001111,3},
	{SYSCFG,0xEA03BA36,4},
};
const DWORD SampleCtrlThreePhase[][3]=
{
	{EMUCON_EMU,0x00007777,3},
	{SYSCFG,0xEA03BA00,4},
};
const DWORD SampleCtrlTemp[][3]=
{
	{ECT_WREN,0x000000EA,4},
	{SAR_CTL0,0x8787CE10,4},
	{SAR_CTL1,0000000000,4},
	{SAR_EN,0000000001,1},
	{ECT_WREN,0x00000000,4},
};
const DWORD SampleCtrleAngle[][3]=
{
	{PHSUA_EMU,0xA7000200,4},
	{PHSUA_EMU,0xA7000200,4},
	{PHSUB_EMU,0x00000200,4},
	{PHSUC_EMU,0x00000200,4},
};


const DWORD SampleCtrlTopo[][3] =
	{
		{HSDCCTL_EMU, 0x00000004, 4},  // spi配置
		{WSAVECON_EMU, 0x00000010, 1}, // 缓冲区清零
		{SYSCFG_EMU, 0xEA00BA00, 4},   // 比拓扑相比多出此项
		{DMA_WAVECON3_EMU, 0x00000000, 4},
		{SOFTRST_EMU, 0x000000E1, 1},
		{SOFTRST_EMU, 0x0000004C, 1},
		{DMA_WAVECON_EMU, 0x00000000, 4},  // 改变波形采样率
		{WSAVECON_EMU, 0x00000080, 1},	   // 同步采样 128点，BUFCFG[3：0] = 0，波形缓冲配置，发送abc波形
		{HSDCCTL_EMU, 0x0000021D, 4},	   // spi配置，bit9是校验方式，新版芯片手册上有
		{DMA_WAVECON3_EMU, 0x00000001, 4}, // 启动连续缓存，BUFCFG[4] = 0
		{DMA_WAVECON2_EMU, 0x00000200, 4}, // bit9使能增益自动调节，bit10使能谐波衰减补偿，
// bit8 通道直流 OFFSET 校正寄存器 DCOSx(1AH~20H)参与 DMA 同步采样通道的直流 OFFSET 校正
};


const DWORD SampleGetTemp[][3] =
	{
		{ECT_WREN, 0x000000EA, 4},
		{SAR_CTL1, 0000000001, 4},
		{ECT_WREN, 0x00000000, 4},
};
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void HardResetSampleChip(void);
static void CheckSampleRam(eSampleType Type);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  判断晶振异常
//         
//参数:      
//         
//返回值:    TRUE  处于晶振异常状态
//         
//备注:  
//--------------------------------------------------
BOOL  IsCrystalErr( void )
{
	DWORD Testaddrmsg[4];
	MEM_ZERO_ARRAY(Testaddrmsg);
	Link_ReadSampleReg(SYSSR, (BYTE*)&Testaddrmsg[0], 2);

	if((Testaddrmsg[0] >> 14) == 1)
	{
		Link_ReadSampleReg(CFCFG_EMU, (BYTE*)&Testaddrmsg[1], 3);
		Link_ReadSampleReg(EMUCFG_EMU, (BYTE*)&Testaddrmsg[2], 3);
		Link_ReadSampleReg(EMUCON_EMU, (BYTE*)&Testaddrmsg[3], 3);
		if ((Testaddrmsg[1]==Testaddrmsg[2])&&(Testaddrmsg[1]==Testaddrmsg[3]))
		{
			return TRUE;
		}
	}
	return FALSE;
}
//--------------------------------------------------
//功能描述:  计量芯片故障时内容恢复
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  ChipRecovery( void )
{
	RecoverNum++;
	if(RecoverNum >= 10)
	{
		RecoverNum = 10;
		HardResetSampleChip();
	}
	UseSampleDefaultPara(eALL);
	CheckSampleRam(eALL);
	if(CheckSampleChipReg())
	{
		GetSampleChipCheckSum();
	}
}
//--------------------------------------------------
//功能描述: 从EE获取计量回路数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE  GetCountFormEE( void )
{
	TSampleChipPara SampleChipPara;

	if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleChipPara),sizeof(TSampleChipPara),(BYTE*)&SampleChipPara) != TRUE)
	{
		SampleChipPara.bCount = 3;
	}
	return SampleChipPara.bCount;
}
//--------------------------------------------------
//功能描述:  校验运行参数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  CheckSampleRunPara( void )
{
	if(lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD)) != samplerunpara.CRC32)
	{
		samplerunpara.eintegratormode= eUnuseIntegrator;
		samplerunpara.emeasurementmode=eThreePhase;
		samplerunpara.CRC32=lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD));
	}
}
//-----------------------------------------------
//函数功能: 写某个校正系数到EEPROM
//
//参数:		WriteEepromFlag[in]	bit0 SampleCtrlAddr 0--写ram不写ee	1--写ram写ee
//								bit1 SampleAdjustAddr 0--写ram不写ee	1--写ram写ee
//			Addr[in]	地址	 
//			Buf[in]		写入数据，两字节，高在后，低在前
//返回值: 	
//		    TRUE/FALSE
//备注: 
//-----------------------------------------------
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,DWORD Addr, BYTE *Buf)
{
	BYTE i,DivCurrentNo, Result;
	DWORD *p;
	short QGainTmp,PGainTmp;

	for(i=0;i<(sizeof(SampleCtrlAddr)/sizeof(SampleCtrlAddr[0]));i++)
	{
		if(Addr == SampleCtrlAddr[i][0])
		{
			p = (DWORD*)&SampleCtrlReg;
			memcpy((void*)&p[i],Buf,sizeof(DWORD));
			
			if( (WriteEepromFlag&0x01) == 0x00 )
			{
				SampleCtrlReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleCtrlReg,sizeof(TSampleCtrlReg)-sizeof(DWORD));				
			}
			else
			{
				//开写ee校正系数开关
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg), sizeof(TSampleCtrlReg), (BYTE*)&SampleCtrlReg);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			}
			return TRUE;
		}
	}
	
	for(i=0;i<(sizeof(SampleAdjustAddr)/sizeof(SampleAdjustAddr[0]));i++)
	{
		if(Addr == SampleAdjustAddr[i][0])
		{
			p = (DWORD *)&SampleAdjustReg;
			memcpy((void*)&p[i],Buf,sizeof(DWORD));
			
			//只更改RAM和计量芯片 不写EE
			if( (WriteEepromFlag&0x02) == 0x00 )
			{
				SampleAdjustReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleAdjustReg,sizeof(TSampleAdjustReg)-sizeof(DWORD));				
			}
			else
			{
				//开写ee校正系数开关
				api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg), sizeof(TSampleAdjustReg), (BYTE*)&SampleAdjustReg);
				api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);	
			}
			
			return TRUE;
		}
	}
	
	return FALSE;
}
//--------------------------------------------------
// 功能描述:  效验采样芯片校验寄存器
//
// 参数:    eCheckType
//						eUpDateSumCheck  更新校验
//						eSumCheck		检查校验
//
// 返回值:    FALSE 计量芯片异常
//
// 备注:
//--------------------------------------------------
BOOL CheckSampleSumRegPro(eCheckType echecktype)
{
	static DWORD dwSumChecks[5] = {ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F, ILLEGAL_VALUE_8F};
	DWORD dwSumChecksTemp = 0;
	DWORD dwSumChecksTemp2 = 0;
	BYTE i = 0, j = 0;
	WORD addrs[5] = {CKSUM_EMU1, CKSUM_EMU2, CKSUM_EMU3, CKSUM_EMU4, SYSCFG};
	BYTE lens[5] = {3, 2, 4, 4, 4};

	if(IsCrystalErr())
	{
		return FALSE;
	}

	if(echecktype == eUpDateSumCheck)
	{
		for(i = 0; i < (sizeof(dwSumChecks) / sizeof(dwSumChecks[0])); i++)
		{
			for(j = 0; j < 3; j++)
			{
				dwSumChecksTemp = 0;
				dwSumChecksTemp2 = 0;
				if(Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp, lens[i]) == TRUE)
				{
					if(Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp2, lens[i]) == TRUE)
					{
						if(dwSumChecksTemp == dwSumChecksTemp2)
						{
							dwSumChecks[i] = dwSumChecksTemp;
							break;
						}
					}
				}
			}
		}
	}
	else if(echecktype == eSumCheck)
	{
		for(i = 0; i < (sizeof(dwSumChecks) / sizeof(dwSumChecks[0])); i++)
		{
			dwSumChecksTemp = 0;
			if(Link_ReadSampleReg(addrs[i], (BYTE *)&dwSumChecksTemp, lens[i]) == TRUE)
			{
				if(dwSumChecks[i] != dwSumChecksTemp)
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
	CheckSampleSumRegPro(eUpDateSumCheck);
}
//-----------------------------------------------
//函数功能: 计算一分钟平均功率
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:更新RemoteValue.Pav，为HEX码 
//-----------------------------------------------
void api_CalPaverage(void)
{
	BYTE i,j;
	static BYTE PowerUPFlag = 0X55;//上电第一次平均功率为0

	for(i=0;i<3;i++)
	{
		for(j=0;j<4;j++)
		{
			if(( (MeterTypesConst==METER_3P3W) && (j==2) )//3P3W的B相
				||( Paverage[i][j].Cnt == 0 )//计数值为0
				||( PowerUPFlag == 0X55 ))//上电第一次计算
			{
				//用上次计算结果
				continue;
			}
			else
			{
				RemoteValue.Pav[i][j].l = (long)(Paverage[i][j].SumValue/(float)Paverage[i][j].Cnt);
			}
		}
	}

	memset( (BYTE *)(Paverage), 0 ,sizeof(Paverage));
	
	PowerUPFlag = 0;
}

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
static WORD CaleUnbalanceRate(eUIPType Type, long *Buf)
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
	
	if( Type == eVOLTAGE )
	{
		//计算临界电压 60%Un（3位小数）
		dwMin = wStandardVoltageConst * 100 * 6 / 10;
	}
	else
	{
		//计算5%Ib（4位小数）
		dwMin = dwMeterBasicCurrentConst * 10 * 5 / 100;
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
    BYTE i,DataSign;
	BYTE CurRatio;
	BYTE bySamplePulseNum;	
	DWORD tdw,TotalEnergy1,TotalEnergy2,RatioEnergy1,RatioEnergy2;
	DWORD ILimit,VoltageLimit,VMINLimit;
	long tl;
	float tf;
    BYTE Result,Cmd,Count;
	TFourByteUnion ComData; //按最大存
    
	if( bySampleStep == 0xff )
	{
		return;
	}
	Cmd = bySampleStep;
	ComData.d = 0;
    bySamplePulseNum = 0;
	
	switch(Cmd)
	{
		case 0x00://潜动起动标志
			Result = Link_ReadSampleReg(Status_EMU,ComData.b,2);
			if(Result == FALSE)
			{
				break; //如果读取计量芯失败，使用上一次数据
			}
			//标志
			dwSampleSFlag = ComData.d;		
			//dwSamplePFlag = ComData.d;		
        break;
			//电压	
		case UA_EMU:
		case UB_EMU:
		case UC_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			
			tdw = (DWORD)((QWORD)ComData.d*1000/SampleCorr.UGain[Cmd - UA_EMU].d);
			VMINLimit = wStandardVoltageConst * 4; //4%Un
			if(tdw > VMAXLimit) 
			{
				tdw = VMAXLimit;
			}
			if(tdw < VMINLimit)
			{
				tdw = 0;
			}
			RemoteValue.U[Cmd - UA_EMU].d = tdw;

			//电压不平衡率
			if( Cmd == 3 )
			{
				RemoteValue.Uunbalance.w = CaleUnbalanceRate( eVOLTAGE, (long*)&RemoteValue.U[0].l );
			}
			break;
		case PA_EMU:
		case PB_EMU:
		case PC_EMU:
			//P有功功率，6位小数,单位KW
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			tl = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[Cmd-PA_EMU+1].d);
			//判断是否大于启动功率同时功率符号位为1
			if((dwSampleSFlag&(P_CREEP<<(Cmd - PA_EMU)))!=0)
			{
				tl = 0;
			}
			RemoteValue.P[Cmd-PA_EMU+1].l = tl;

			if( tl < 0 )
			{
				//设置分相反向标志
				api_SetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - PA_EMU);
			}
			else if( tl > 0 )
			{
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - PA_EMU);
			}

			Paverage[0][Cmd-PA_EMU+1].SumValue += (float)RemoteValue.P[Cmd-PA_EMU+1].l;
			Paverage[0][Cmd-PA_EMU+1].Cnt++;
			
			break;
		case PT_EMU:
			Result = Link_ReadSampleReg( (WORD)Cmd,ComData.b,4);
			tl = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[0].d);
			if( ( (MeterTypesConst==METER_3P3W) && ((dwSampleSFlag&0x00000005)==0x00000005) )
			||	( (MeterTypesConst!=METER_3P3W) && ((dwSampleSFlag&0x00000007)==0x00000007) ) )
			{
				tl = 0;
			}
			RemoteValue.P[0].l = tl;	

			if( tl < 0 )
			{
				//设置合相反向标志
				api_SetSysStatus(eSYS_STATUS_OPPOSITE_P);
			}
			else if( tl > 0 )
			{
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_P);
			}
			
			Paverage[0][0].SumValue += (float)RemoteValue.P[0].l;
			Paverage[0][0].Cnt++;

			#if( SEL_DEMAND_2022 == YES )
			#if( MAX_PHASE != 1 )
			if(tl != 0)
			{
				tl = 1;//防止截断
			}
		    api_UptCurDemandStatus( 0, tl);//更新当前有功需量状态,放在更新标志之后
			#endif
			#endif
			
			break;
			
		case QA_EMU:
		case QB_EMU:
		case QC_EMU:
			Result = Link_ReadSampleReg( (WORD)Cmd,ComData.b,4);
			tl = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[Cmd - QA_EMU + 1].d);
			//判断是否大于启动功率同时功率符号位为1
			if((dwSampleSFlag&(Q_CREEP<<(Cmd - QA_EMU)))!=0)
			{
				tl = 0;
			}
			RemoteValue.Q[Cmd-QA_EMU+1].l = tl;

			if( tl < 0 )
			{
				//设置分相反向标志
				api_SetSysStatus(eSYS_STATUS_OPPOSITE_Q_A + Cmd - QA_EMU);
			}
			else if( tl > 0 )
			{
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_Q_A + Cmd - QA_EMU);
			}

			Paverage[1][Cmd - QA_EMU + 1].SumValue += (float)RemoteValue.Q[Cmd - QA_EMU + 1].l;
			Paverage[1][Cmd - QA_EMU + 1].Cnt++;
			break;
			
		case QT_EMU:
			Result = Link_ReadSampleReg( (WORD)Cmd,ComData.b,4);
			tl = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[0].d);
			if( ( (MeterTypesConst==METER_3P3W) && ((dwSampleSFlag&0x00000050)==0x00000050) )
			||	( (MeterTypesConst!=METER_3P3W) && ((dwSampleSFlag&0x00000070)==0x00000070) ) )
			{
				tl = 0;
			}
			RemoteValue.Q[0].l = tl;

			if( tl < 0 )
			{
				//设置分相反向标志
				api_SetSysStatus(eSYS_STATUS_OPPOSITE_Q);
			}
			else if( tl > 0 )
			{
				api_ClrSysStatus(eSYS_STATUS_OPPOSITE_Q);
			}
			#if( SEL_DEMAND_2022 == YES )
			#if( MAX_PHASE != 1 )
			if(tl != 0)
			{
				tl = 1;//防止截断
			}
			api_UptCurDemandStatus( 1, tl);//更新当前有功需量状态,放在更新标志之后
			#endif
			#endif		
			Paverage[1][0].SumValue += (float)RemoteValue.Q[0].l;
			Paverage[1][0].Cnt++;
			break;
			
		//S视在功率，6位小数,单位KW			
		case SA_EMU:
		case SB_EMU:
		case SC_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			RemoteValue.S[Cmd - SA_EMU + 1].l = (long)((double)ComData.l*100000.0/(double)SampleCorr.PGain[Cmd - SA_EMU + 1].d);

			if((RemoteValue.P[Cmd - SA_EMU + 1].l == 0)&&(RemoteValue.Q[Cmd - SA_EMU + 1].l == 0))
			{
				RemoteValue.S[Cmd - SA_EMU + 1].l = 0;
			}
			if(RemoteValue.P[Cmd - SA_EMU + 1].l < 0)
			{
				RemoteValue.S[Cmd - SA_EMU + 1].l *= -1;
			}
			Paverage[2][Cmd - SA_EMU + 1].SumValue += (float)RemoteValue.S[Cmd - SA_EMU + 1].l;
			Paverage[2][Cmd - SA_EMU + 1].Cnt++;
			break;
			
		case STA_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			RemoteValue.S[0].l = (long)((double)ComData.l*1000.0/(double)SampleCorr.PGain[0].d*100);
			if((RemoteValue.P[0].l == 0)&&(RemoteValue.Q[0].l == 0))
			{
				RemoteValue.S[0].l = 0;
			}
			if(RemoteValue.P[0].l < 0)
			{
				RemoteValue.S[0].l *= -1;
			}

			Paverage[2][0].SumValue += (float)RemoteValue.S[0].l;
			Paverage[2][0].Cnt++;
			break;
			
		//电流，4位小数,单位A
		//应该先读功率再读电流 因为电流的方向需要根据功率判断
		case IA_EMU:
		case IB_EMU:
		case IC_EMU:
		case IN_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			RemoteValue.I[Cmd - IA_EMU].l = (long)((double)ComData.d*10000.0/SampleCorr.IGain[Cmd - IA_EMU].d);
			if((RemoteValue.I[Cmd - IA_EMU].l > IMAXLimit)||(RemoteValue.I[Cmd - IA_EMU].l < IMINLimit)) 
			{
				RemoteValue.I[Cmd - IA_EMU].l = 0;
			}
			if( Cmd == IN_EMU )//零线电流
			{
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P) == TRUE )
				{
					RemoteValue.I[3].l *= -1;
				}
			}
			else
			{
				if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - IA_EMU) == TRUE )
				{
					RemoteValue.I[Cmd - IA_EMU].l *= -1;
				}
				//电流不平衡率
				if( Cmd == 0x12 )
				{
					RemoteValue.Iunbalance.w = CaleUnbalanceRate( eCURRENT, (long*)&RemoteValue.I[0].l );
				}
			}
		break;
		
		case IN2_EMU:	//零序
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,4);
			RemoteValue.I[4].l = (long)((double)ComData.d*10000.0/SampleCorr.IGain[0].d);
			if((RemoteValue.I[4].l > IMAXLimit)||(RemoteValue.I[4].l < IMINLimit)) 
			{
				RemoteValue.I[4].l = 0;
			}
			
			RemoteValue.I[4].l *= 4;
		break;
		
		case PfA_EMU:
			//Cos功率因数,4位小数	
			//功率因数寄存器会概率出错，暂时使用功率值推算功率因数   未考虑三相三线
			for( i = 0; i<4; i++ )
			{
				if(RemoteValue.S[i].d)
				{
					RemoteValue.Cos[i].l = (long)(RemoteValue.P[i].l*10000.0/RemoteValue.S[i].l);
					if(labs(RemoteValue.Cos[i].l) > 10000)	
					{
						RemoteValue.Cos[i].l = 10000;
					}
				}
				else 
				{
					RemoteValue.Cos[i].l = 10000;
				}
				
				if((RemoteValue.Q[i].l == 0)&&(RemoteValue.P[i].l == 0 ))
				{
					RemoteValue.Cos[i].l = 10000;
				}

				if(i == 0)
				{
					if((RemoteValue.I[0].l == 0)&&(RemoteValue.I[1].l == 0)&&(RemoteValue.I[2].l == 0))
					{
						RemoteValue.Cos[i].l = 10000;
					}
				}
				else
				{
					if(RemoteValue.I[i-1].l == 0)
					{
						RemoteValue.Cos[i].l = 10000;
					}
				}
				
				if(RemoteValue.P[i].l < 0)
				{
					RemoteValue.Cos[i].l *= -1;
				}
			}
			break;
		//频率
		case Freq_U_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,3);
			RemoteValue.Freq.l = 6553600000/ComData.d;	
			break;

		//电压相角
		case YUA_EMU:
		case YUB_EMU:
		case YUC_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,3);
			RemoteValue.YUAngle[Cmd - YUA_EMU].d = (DWORD)((double)ComData.d*100*360/16777216/10);
			break;
		//电流相角
		case YIA_EMU:
		case YIB_EMU:
		case YIC_EMU:
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,3);
			RemoteValue.YIAngle[Cmd - YIA_EMU].d = (DWORD)((double)ComData.d*100*360/16777216/10);

			if(( Cmd == YIC_EMU ) && ((dwSampleSFlag%0x00000008)==0x00000000))
			{
				Count = 0;
				
				if( RemoteValue.YIAngle[1].d > RemoteValue.YIAngle[0].d )
				{
					Count++;
				}
				if( RemoteValue.YIAngle[2].d > RemoteValue.YIAngle[1].d )
				{
					Count++;
				}
				if( RemoteValue.YIAngle[2].d > RemoteValue.YIAngle[0].d )
				{
					Count++;
				}
				api_ClrSysStatus( eSYS_STATUS_REVERSE_PHASE_I );
				if((Count == 0)||(Count == 2))
				{
					api_SetSysStatus( eSYS_STATUS_REVERSE_PHASE_I );
				}
				
			}
			
			break;
		// 有功电能，脉冲个数
	    case EPT_EMU:   // 总有功电能
		#if(SEL_SEPARATE_ENERGY == YES)
	    case EPA_EMU:   // A相有功总电能
	    case EPB_EMU:   // B相有功总电能
	    case EPC_EMU:   // C相有功总电能
		#endif// #if( SEL_SEPARATE_ENERGY == YES )
		    Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 3);
		    if(ComData.d > 0xff)
		    {
			    Result = FALSE;
		    }
		    if((ComData.d != 0) && (Result == TRUE))
		    {
			    if(Cmd == EPT_EMU)
			    {
				    CurRatio = api_GetCurRatio();
				    if(CurRatio > MAX_RATIO)
				    {
					    CurRatio = DefCurrRatioConst;
				    }
				    // 有功功率未大于起动功率，电能方向按上秒功率方向
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P) == TRUE)
				    {
					    api_WritePulseEnergy(PHASE_ALL + N_ACTIVE, ComData.b[0]);   // 反向
				    }
				    else
				    {
					    api_WritePulseEnergy(PHASE_ALL + P_ACTIVE, ComData.b[0]);   // 正向
				    }
			    }
			    else
			    {
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - EPA_EMU) == TRUE)
				    {
					    api_WritePulseEnergy((Cmd - EPA_EMU + 1) * PHASE_A + N_ACTIVE, ComData.b[0]);
				    }
				    else
				    {
					    api_WritePulseEnergy((Cmd - EPA_EMU + 1) * PHASE_A + P_ACTIVE, ComData.b[0]);
				    }
			    }
		    }
		    if(Cmd == EPT_EMU)
		    {
				// 更新需量累计用的的有功电能脉冲数增量及超起动门限时间增量
				// 注，即使脉冲数增量为0，也要更新，因为超起动门限时间可能增加
				#if(SEL_DEMAND_2022 == YES)
				#if(MAX_PHASE != 1)
				if(Result == FALSE)
				{
					ComData.d = 0;
				}
				api_UpdateDemandEnergyPulse(0, ComData.b[0]);   // 使用高频脉冲
				#endif
				#endif
		    }
		    break;
		// 无功电能，脉冲个数
		#if(SEL_SEPARATE_ENERGY == YES)
	    case EQA_EMU:
	    case EQB_EMU:
	    case EQC_EMU:
		#endif
	    case EQT_EMU:
		    Result = Link_ReadSampleReg((WORD)Cmd, ComData.b, 3);
		    if(ComData.d > 0xff)
		    {
			    Result = FALSE;
		    }
		    if((ComData.d != 0) && (Result == TRUE))
		    {
			    if(Cmd == EQT_EMU)
			    {
				    // 根据功率方向，判断无功电能象限
				    DataSign = 0x00;
				    // 有功为正，在一四象限
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P) == TRUE)
				    {
					    DataSign |= 0x08;
				    }
				    // 无功为正，在一二象限
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_Q) == TRUE)
				    {
					    DataSign |= 0x80;
				    }
				    switch(DataSign)
				    {
				    case 0x00:   // 一象限
					    api_WritePulseEnergy(PHASE_ALL + RACTIVE1, ComData.b[0]);
					    break;
				    case 0x08:   // 二象限
					    api_WritePulseEnergy(PHASE_ALL + RACTIVE2, ComData.b[0]);
					    break;
				    case 0x88:   // 三象限
					    api_WritePulseEnergy(PHASE_ALL + RACTIVE3, ComData.b[0]);
					    break;
				    case 0x80:   // 四象限
					    api_WritePulseEnergy(PHASE_ALL + RACTIVE4, ComData.b[0]);
					    break;
				    default:
					    break;
				    }
			    }
			    else
			    {
				    // 根据功率方向，判断无功电能象限
				    DataSign = 0x00;
				    // 有功为正，在一四象限
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + Cmd - EQA_EMU) == TRUE)
				    {
					    DataSign |= 0x01;
				    }
				    // 无功为正，在一二象限
				    if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_Q_A + Cmd - EQA_EMU) == TRUE)
				    {
					    DataSign |= 0x10;
				    }
				    switch(DataSign)
				    {
				    case 0x00:   // 一象限
					    api_WritePulseEnergy((Cmd - EQA_EMU + 1) * PHASE_A + RACTIVE1, ComData.b[0]);
					    break;
				    case 0x01:   // 二象限
					    api_WritePulseEnergy((Cmd - EQA_EMU + 1) * PHASE_A + RACTIVE2, ComData.b[0]);
					    break;
				    case 0x11:   // 三象限
					    api_WritePulseEnergy((Cmd - EQA_EMU + 1) * PHASE_A + RACTIVE3, ComData.b[0]);
					    break;
				    case 0x10:   // 四象限
					    api_WritePulseEnergy((Cmd - EQA_EMU + 1) * PHASE_A + RACTIVE4, ComData.b[0]);
					    break;
				    default:
					    break;
				    }
				    break;
			    }
		    }
		    if(Cmd == EQT_EMU)
		    {
				// 更新需量累计用的的有功电能脉冲数增量及超起动门限时间增量
				// 注，即使脉冲数增量为0，也要更新，因为超起动门限时间可能增加
				#if(SEL_DEMAND_2022 == YES)
				#if(MAX_PHASE != 1)
				if(Result == FALSE)
				{
					ComData.d = 0;
				}
				api_UpdateDemandEnergyPulse(1, ComData.b[0]);   // 使用高频脉冲
				#endif
				#endif
		    }
		    break;
		//视在电能，脉冲个数
		case ESA_EMU:
		case ESB_EMU:	
		case ESC_EMU:
		case ESTA_EMU:			
			Result = Link_ReadSampleReg((WORD)Cmd,ComData.b,3);
			if(ComData.d > 0xff)
		    {
			    Result = FALSE;
		    }
			
			if((ComData.d != 0)&&(Result == TRUE))
			{
				if( Cmd == ESTA_EMU )
				{
					if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE)
					{
						api_WritePulseEnergy(PHASE_ALL+APPARENT_N,ComData.b[0]);
					}
					else
					{
						api_WritePulseEnergy(PHASE_ALL+APPARENT_P,ComData.b[0]);
					}
				}
				else
				{
					if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_A+Cmd-ESA_EMU ) == TRUE)
					{
						api_WritePulseEnergy((Cmd - ESA_EMU + 1)*PHASE_A+APPARENT_N,ComData.b[0]);
					}
					else
					{
						api_WritePulseEnergy((Cmd - ESA_EMU + 1)*PHASE_A+APPARENT_P,ComData.b[0]);
					}
				}
			}
			break;
		default:
			break;	

	}
	bySampleStep++;

	if( bySampleStep >= 0x60 )//@@@@@暂时先不改，后期确定
	{
		bySampleStep = 0xff;
	}	

/*	//自定义能量寄存器，谐波电能，脉冲个数
	Result = Link_ReadSampleReg(D2FE01_EMU,ComData.b);
	if((ComData.d != 0)&&(Result == TRUE))
	{	
		api_WritePulseEnergy(PHASE_ALL+HARM_WAVE+P_ACTIVE,ComData.b[0]);// 正向谐波电能
	}
	Result = Link_ReadSampleReg(D2FE00_EMU,ComData.b);
	if((ComData.d != 0)&&(Result == TRUE))
	{												
		api_WritePulseEnergy(PHASE_ALL+HARM_WAVE+N_ACTIVE,ComData.b[0]);// 反向谐波电能					
	}
*/
}
//-----------------------------------------------
//函数功能: 采样寄存器设置为默认数值
//
//参数:		eSampleType Type
//			
//返回值: 	无
//		    
//备注:谨慎使用eALL，目前只有初始化使用
//-----------------------------------------------
void UseSampleDefaultPara( eSampleType Type )
{
	BYTE i;
    WORD TempData;
	
	InitSampleCaliUI();
	if(( Type == eCORR )||( Type == eALL ))
	{
		for(i=0; i<(sizeof(SampleUIPGain)/sizeof(DWORD)); i++)
		{
			if(i<3)
			{
				SampleCorr.UGain[i].d = SampleUIPGain[i];
			}
			else if(i<7)
			{
				SampleCorr.IGain[i-3].d = SampleUIPGain[i];
			}
			else if(i<11)
			{
				SampleCorr.PGain[i-7].d = SampleUIPGain[i];
			}
			else
			{

			}
		}
		//SampleCorr.CRC32 = lib_CheckCRC32((BYTE*)&SampleCorr,sizeof(TSampleCorr)-sizeof(DWORD));
	}
	
	if(( Type == eCRTL )||( Type == eALL ))//控制设置 TSampleCtrlPara
	{
	  	memcpy((BYTE*)&SampleCtrlReg,(BYTE*)SampleCtrlDefData,sizeof(TSampleCtrlReg)-sizeof(DWORD));

		//电压暂降低，额定电压的40%，阈值需大于5%，否则会出现误判
		SampleCtrlReg.SAGCFG.w[0] = (WORD)(((float)wStandardVoltageConst / 10.0)*(SampleCorr.UGain[0].d)/4096*1.414*0.4);
		//持续时间10个周波
		SampleCtrlReg.SAGCFG.w[1] = 10*2;

		//按照启动门限的60%设置 !!!!!!
		SampleCtrlReg.PSTART.d = (DWORD)(((float)dwMeterBasicCurrentConst / 1000.0)*((float)wStandardVoltageConst / 10.0)*(float)(SampleCorr.PGain[0].d/100)*0.6*0.001/256.0);
		if( MeterTypesConst == METER_ZT )
		{
			//起动转换为100A的2‰
			SampleCtrlReg.PSTART.d *= 2;
		}

		//电压相角，测频阈值，额定电压的30%（要求到0.35）
		TempData = (WORD)(((float)wStandardVoltageConst / 10.0)*(SampleCorr.UGain[0].d)/4096*0.3);
		SampleCtrlReg.ZXOTU.d = TempData;//电压阈值

		//电流相角，额定电流的0.1%
		TempData = (WORD)(((float)dwMeterBasicCurrentConst / 1000.0)*(float)(SampleCorr.IGain[0].d)*0.001/4096.0);
		SampleCtrlReg.ZXOT.d = TempData;//电流阈值

		//SampleCtrlReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleCtrlReg,sizeof(TSampleCtrlReg)-sizeof(DWORD));
	}
	
	if(( Type == eADJUST )||( Type == eALL ))//TSampleAdjustPara
	{
		memcpy((BYTE*)&SampleAdjustReg,(BYTE*)SampleAdjustDefData,sizeof(TSampleAdjustReg)-sizeof(DWORD));
		//SampleAdjustReg.CRC32 = lib_CheckCRC32((BYTE*)&SampleAdjustReg,sizeof(TSampleAdjustReg)-sizeof(DWORD));
	}
}
//-----------------------------------------------
//函数功能: ram中保存的计量芯片寄存器校验
//
//参数:		eSampleType Type
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void CheckSampleRam(eSampleType Type)
{
	BOOL Result;

	CheckSampleRunPara();   // 校验参数
	if((Type == eCORR) || (Type == eALL))
	{
		if(SampleCorr.CRC32 != lib_CheckCRC32((BYTE *)&SampleCorr, sizeof(TSampleCorr) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCorr), sizeof(TSampleCorr), (BYTE *)&SampleCorr);
			if(Result != TRUE)
			{
				UseSampleDefaultPara(eCORR);
			}
		}
	}
	// 校表寄参数校验
	if((Type == eADJUST) || (Type == eALL))
	{
		if(SampleAdjustReg.CRC32 != lib_CheckCRC32((BYTE *)&SampleAdjustReg, sizeof(TSampleAdjustReg) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleAdjustReg), sizeof(TSampleAdjustReg), (BYTE *)&SampleAdjustReg);
			if(Result != TRUE)
			{
				UseSampleDefaultPara(eADJUST);
			}
		}
	}
	// 控制参数校验
	if((Type == eCRTL) || (Type == eALL))
	{
		if(SampleCtrlReg.CRC32 != lib_CheckCRC32((BYTE *)&SampleCtrlReg, sizeof(TSampleCtrlReg) - sizeof(DWORD)))
		{
			Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleCtrlReg), sizeof(TSampleCtrlReg), (BYTE *)&SampleCtrlReg);
			if(Result != TRUE)
			{
				UseSampleDefaultPara(eCRTL);
			}
		}
	}
}

//--------------------------------------------------
//功能描述:  设置计量芯片工作模式
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_SetSampleRunPara( eIntegratorMode inmode , eMeasurementMode memode )
{
	if (eNoneIntegrator!=inmode)
	{
		samplerunpara.eintegratormode=inmode;
	}
	if (eNonePhase!=memode)
	{
		samplerunpara.emeasurementmode=memode;
	}
	if(lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD)) != samplerunpara.CRC32)//内容改变
	{
		samplerunpara.CRC32=lib_CheckCRC32((BYTE *)&samplerunpara, sizeof(samplerunpara) - sizeof(DWORD));
		UseSampleDefaultPara(eALL);
		CheckSampleRam(eALL);
		if(CheckSampleChipReg())
		{
			GetSampleChipCheckSum();
		}
	}
}
//--------------------------------------------------
//功能描述:  写入寄存器组合
//         
//参数:      Combop 数组指针 
//			ComboSize 参数个数
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL WriteSampleCombo(DWORD *Combop, BYTE ComboSize)
{
	BYTE i;
	BOOL Rtresult = TRUE;

	for(i = 0; i < ComboSize; i += 3)
	{
		SampleSpecCmdOp(Combop[i], CmdWriteOpen);
		Rtresult &= WriteSampleReg(Combop[i], (BYTE *)&Combop[i + 1], Combop[i + 2]);
		SampleSpecCmdOp(Combop[i], CmdWriteClose);
	}
	return Rtresult;
}
//-----------------------------------------------
//函数功能: 效验采样芯片的寄存器
//
//参数:		无
//
//返回值: 	无
//
//备注:   
//-----------------------------------------------
BOOL CheckSampleChipReg(void)
{
	BYTE i;
	BOOL Rtresult = TRUE, Result;
	BYTE lock[2] = {0xE5, 0xDC};
	TFourByteUnion ComData;
	DWORD TEMP;

	CLEAR_WATCH_DOG;
	// 掉电判断
	if(api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE)
	{
		return FALSE;
	}
	SampleSpecCmdOp(SYSCFG,CmdWriteClose);//开启寄存器扩位功能
	// 控制寄存器
	for(i = 0; i < (sizeof(SampleCtrlAddr) / sizeof(SampleCtrlAddr[0])); i++)
	{
		// 注意！！！！
		// 主控+计量芯片方案需要每次判断是否掉电，防止在此掉电把电池电量耗光
		// SOC方案可不检查电压
		if(api_CheckSysVol(eOnRunDetectPowerMode) == FALSE)
		{
			return FALSE;
		}

		CLEAR_WATCH_DOG;
		ComData.d = 0;
		Result = Link_ReadSampleReg(SampleCtrlAddr[i][0], ComData.b, SampleCtrlAddr[i][1]);
		if(Result != TRUE)
		{
			Rtresult &= Result;
			continue;
		}

		if(ComData.d != *(((DWORD *)&SampleCtrlReg.CFCFG + i)))
		{
			if(SampleCtrlAddr[i][0] == ZXOTCFG_EMU)//该寄存器某些位为配置值 某些位为读取值
			{
				if((ComData.d & 0X0000001F) == *(((DWORD *)&SampleCtrlReg.CFCFG + i)))
				{
					continue;
				}
			}
			ComData.d = *((DWORD *)&SampleCtrlReg.CFCFG.d + i);
			SampleSpecCmdOp(SampleCtrlAddr[i][0], CmdWriteOpen);
			Rtresult &= WriteSampleReg(SampleCtrlAddr[i][0], ComData.b, SampleCtrlAddr[i][1]);
			SampleSpecCmdOp(SampleCtrlAddr[i][0], CmdWriteClose);
		}
	}
	if(samplerunpara.eintegratormode == eUnuseIntegrator)   // 积分器功能校验
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlUnuseIntegrator, sizeof(SampleCtrlUnuseIntegrator) / sizeof(DWORD));
	}
	else
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlUseIntegrator, sizeof(SampleCtrlUseIntegrator) / sizeof(DWORD));
	}
	if(samplerunpara.emeasurementmode == eOnePhase)
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlOnePhase, sizeof(SampleCtrlOnePhase) / sizeof(DWORD));
	}
	else
	{
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlThreePhase, sizeof(SampleCtrlThreePhase) / sizeof(DWORD));
	}
	Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlTemp, sizeof(SampleCtrlTemp) / sizeof(DWORD));
	Rtresult &= WriteSampleCombo((DWORD *)SampleCtrleAngle, sizeof(SampleCtrleAngle) / sizeof(DWORD));

	// 校准寄存器
	for(i = 0; i < (sizeof(SampleAdjustAddr) / sizeof(SampleAdjustAddr[0])); i++)
	{
		if(api_CheckSysVol(eOnRunDetectPowerMode) == FALSE)
		{
			return FALSE;
		}

		CLEAR_WATCH_DOG;
		ComData.d = 0;
		Result = Link_ReadSampleReg(SampleAdjustAddr[i][0], ComData.b, SampleAdjustAddr[i][1]);
		if(Result != TRUE)
		{
			Rtresult &= Result;
			continue;
		}

		if(ComData.d != *(((DWORD *)&SampleAdjustReg.GP[0] + i)))
		{
			ComData.d = *((DWORD *)&SampleAdjustReg.GP[0] + i);
			SampleSpecCmdOp(SampleAdjustAddr[i][0], CmdWriteOpen);
			Rtresult &= WriteSampleReg(SampleAdjustAddr[i][0], ComData.b, SampleAdjustAddr[i][1]);
			SampleSpecCmdOp(SampleAdjustAddr[i][0], CmdWriteClose);
		}
	}

	//扩展寄存器
	if( __get_PRIMASK() == 0 )//全局中断打开
	{		
		Rtresult &= WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD));
	}

	return Rtresult;
}

//-----------------------------------------------
//函数功能: 复位采样芯片
//
//参数:		无 
//			
//返回值: 	无
//		    
//备注:直接控制复位管脚
//-----------------------------------------------
static void HardResetSampleChip(void)
{		
	//手册要求1ms
//	RESET_SAMPLE_OPEN;
//	api_Delayms(10);
//	RESET_SAMPLE_CLOSE;
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 200 );
	CLEAR_WATCH_DOG;
}
//-----------------------------------------------
//函数功能: 效验采样芯片校验寄存器
//
//参数:		无 
//			
//返回值: 	无
//		    
//备注:上电时还没读到效验，上电不要调用此函数，每分钟的时候调用即可
//-----------------------------------------------
static void SampleChipMinVerification(void)
{
	DWORD tdw1, tdw2, dwSum;
	BYTE Flag,i;

	//掉电判断
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )	
	{
		return;
	}	
	
	Flag = 0;

	if( CheckSampleSumRegPro(eSumCheck) != TRUE)
	{
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
				//HardResetSampleChip();
				api_SetError(ERR_CHECK_5460_1);
			}
		}
		ChipRecovery();
	}
	else
	{
		byReWriteSampleChipRegCount = 0;
		RecoverNum = 0;
		api_ClrError(ERR_CHECK_5460_1);
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
BOOL api_InitSampleChip( void  )
{
    return TRUE;
}
//--------------------------------------------------
//功能描述:  获取温度
//         
//参数:      
//         
//返回值:    SWORD 一位小数 HEX  补码
//         
//备注:  
//--------------------------------------------------
SWORD  api_GetTemp(void)
{
	SWORD Temp=0;
	BYTE i = 0;
	DWORD buf=0x00000001;

	WriteSampleCombo((DWORD *)SampleGetTemp,sizeof(SampleGetTemp)/sizeof(DWORD));
	api_Delayms(1);//等待1ms
	while (i<3)
	{
		if (Link_ReadSampleReg(0x00F7,(BYTE*)&buf,4))
		{
			if (buf&BIT10)//数据有效
			{
				if (buf&BIT9)
				{
					Temp=((SWORD)(buf|0xFFFFFE00))*10/4;
				}
				else
				{
					Temp=(buf-BIT10)*10/4;
				}
				break;
			}
		}
		i++;
	}
	if(i == 3)
	{
		return 0;
	}
	return Temp - 15;   // 温度校准-1.5摄氏度
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
	DWORD dwSum;
	BYTE i;
	BOOL Result;

	CheckSampleRam( eALL );
	
	//初始化参数
	InitSampleCaliUI();
	VMAXLimit = wStandardVoltageConst*220;//CalMaxLimiteValue( eVOLTAGE );
	IMINLimit = (DWORD)dwMeterBasicCurrentConst  * 15 / 1000;//与起动门限保持一致
	IMAXLimit = (DWORD)dwMeterMaxCurrentConst  * 15 ; //1.5Imax
	bySamplePowerOnTimer = 30;
	dwSampleSFlag = 0;
	dwSamplePFlag = 0;		
	byReWriteSampleChipRegCount = 0;
	CheckSampleChipReg();

	GetSampleChipCheckSum();//读校验和
	
	#if( SEL_STAT_V_RUN == YES )//恢复电压合格率
	PowerUpVRunTime( );
	#endif
	
	//上电读出电压 电流 功率
	//PowerUpReadVoltage();
    //byPowerOnFlag = 0;
	
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
	memset((BYTE*)&RemoteValue,0x00,sizeof(TRemoteValue));
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
	DWORD RegData = 0;

	//读取HFConst的方式判断记量芯片故障 
	//不能使用判断校验和的方式 因为SPI没有校验 如果SPI总线短接到VCC或者GND 读出来的数据是固定的
	//校验和也会被刷新为全FF或者全00 导致故障无法被检出
	Link_ReadSampleReg(HFConst_EMU,(BYTE*)&RegData,2 );
	if(RegData != SampleCtrlReg.HFCONST.d)
	{
		if(EmuErrCnt < 12)
		{
			EmuErrCnt++;
			
			//如果连续5次出现HFConst值不对应 重新校验一下HFConst
			//第一防止RAM中数据出现错误 第二如果计量芯片出现复位 每分钟5秒才会进行寄存器校验 会出现误报的问题
			if(EmuErrCnt == 5)
			{
				ChipRecovery();
			}
			
			//连续10次错误 记录计量芯片故障
			if(EmuErrCnt == 10)
			{
				api_WriteSysUNMsg(CHECKSUM_CHANGE_7022);
				api_SetError( ERR_CHECK_5460_1 ); //置计量芯片通信故障
			}
		}
	}
	else
	{
		//如果恢复了 并且之前连续出现了10次以上的错误 重新检查一下所有寄存器数据 内部有api_ClrError
		if(EmuErrCnt > 10)
		{
			SampleChipMinVerification();
		}
		EmuErrCnt = 0; 
	}
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
	//三相才计算谐波和特征
	#if  SELECT_HARMONY_VI_CAL == 1
	if (RawDataHalfWaveCnt == 2)
	{
		HarmAndVICharacCal_Task();
		RawDataHalfWaveCnt = 0;
	}
	#endif
	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )//5s后进行电压合格率累计，避免监测时间不为1440
	{	
		//上电30s后才起动效验，避免还没读出采样芯片的效验和
		if( bySamplePowerOnTimer == 0 )
		{
			CheckSampleRam(eALL);
			SampleChipMinVerification();
        }

		#if( SEL_STAT_V_RUN == YES )//计算电压合格率参数		
		StatVRunTime();
		#endif
	}
	
	if( t.Sec == (eTASK_SAMPLE_ID*3+3+2) )
	{
		//每分钟计算一次平均功率
	  	// api_CalPaverage();
		//每分钟再初始化一下，基本电压、基本电流这两个参数在全失压中会用
		InitSampleCaliUI();
		#if( SEL_AHOUR_FUNC == YES )
		if(t.Min == 26)
		{
			SwapAHour(); 
		}
		#endif
	}
	
	if(bySamplePowerOnTimer)
	{
		bySamplePowerOnTimer--;
		if(bySamplePowerOnTimer == HSDC_OPEN_TIME)
		{
			//打开HSDC接口必须放在打开全局中断之后，否则会发生欠载
			WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD));
			CheckSampleSumRegPro(eUpDateSumCheck);//计算校验和
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
	
	
	
	#if( SEL_AHOUR_FUNC == YES )
	AccumulateAHour();
	#endif

	//计量芯片故障检查
	CheckEmuErr();
}
//-----------------------------------------------
//函数功能: 获取SAG状态
//
//参数:		无 
//                 
//返回值: 	TRUE/FALSE
//		   
//备注:   
//-----------------------------------------------
BYTE api_GetSAGStatus(void)
{
	TTwoByteUnion ComData;
	BOOL Result;
	
	if( Link_ReadSampleReg(EMMIF_EMU,ComData.b,2) == TRUE)
	{
		if((ComData.w & 0x4000) == 0x0000)
		{	
			return TRUE;
		}
	}

	ComData.w=0x4000;
	SampleSpecCmdOp(EMMIF_EMU,CmdWriteOpen);
	Result = WriteSampleReg(EMMIF_EMU,ComData.b,2);
	SampleSpecCmdOp(EMMIF_EMU,CmdWriteClose);

	return FALSE;
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
	TFourByteUnion RegData = {0};

	Read_UIP();
	if(api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_10_MS) == TRUE)
	{
		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_10_MS);
		if( HSDCTimer >= 20 )
		{
			if(TopoErrFlag < 20)
			{
				TopoErrFlag++;
			}
			HSDCTimer = 0;
			api_TopWaveSpiInit();
			Link_ReadSampleReg(HSDCCTL_EMU, RegData.b, 4);
			if ((RegData.d & 0x00000001) == 0)
			{
				api_WriteSysUNMsg(HSDC_ERR);
				// 打开HSDC接口必须放在打开全局中断之后，否则会发生欠载
				WriteSampleCombo((DWORD *)SampleCtrlTopo, sizeof(SampleCtrlTopo) / sizeof(DWORD));
				CheckSampleSumRegPro(eUpDateSumCheck); // 计算校验和
			}
		}
	}

	if(api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_SECOND) == TRUE)
	{
		Task_Sec_Sample();
		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_SECOND);
	}

	if(api_GetTaskFlag(eTASK_SAMPLE_ID, eFLAG_HOUR) == TRUE)
	{
		api_ClrTaskFlag(eTASK_SAMPLE_ID, eFLAG_HOUR);
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t);
	#if(SEL_STAT_V_RUN == YES)
		if(t.Hour == 0)
		{
			// 延时3秒清，防止在冻结的时候数据为0
			if(t.Day == 1)
			{
				ClrVRunMonTimer = 3;
			}

			ClrVRunDayTimer = 3;
		}
		else
		{
			// 每小时转存电压合格率
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
//函数功能: 低功耗状态获取电压电流
//
//参数:		Phase[in]:A B C
//          Type[in]: U I       
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
DWORD api_LowPowerGetUI( eUIPType Type, BYTE Phase)
{	
	TFourByteUnion ComData;

	ComData.d = 0;
	
	//U电压
	if( Type == eVOLTAGE )
	{
		if( Phase >= MAX_PHASE )
		{
			return 0;
		}
		if( Link_ReadSampleReg(UA_EMU+4*Phase,ComData.b,4) == TRUE )
		{
			ComData.d = (QWORD)ComData.d*1000/SampleCorr.UGain[Phase].d;	
		}
	}
	else//电流
	{
		if( Phase >= MAX_PHASE+1 )
		{
			return 0;
		}

		if( Link_ReadSampleReg(IA_EMU+4*Phase,ComData.b,4) == TRUE )
		{
			ComData.d = (DWORD)((double)ComData.d*10000.0/SampleCorr.IGain[Phase].d);	
		}
	}

	return ComData.d;
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
const BYTE MinPhase[] = {1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	1,		1,	0xff,	0xff,0,	0};
const BYTE MaxPhase[] = {3,	5,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,		3,	0xff,	0xff,0,	0};
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
	if(api_GetMeasureMentMode() == eOnePhase)//单项模式总返回A相
	{
		if(Phase == 0)
		{
			Phase = 1;
		}
	}

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
			if(MeterTypesConst == METER_3P3W )
			{
				//三相三线表不支持零线电流
				if( Type == (PHASE_N+REMOTE_I) )
				{
					return FALSE;
				}
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
			memcpy(td.b,RemoteValue.Pav[0][Phase].b,4);
			break;
		case REMOTE_Q_AVE:
			memcpy(td.b,RemoteValue.Pav[1][Phase].b,4);
			break;
		case REMOTE_S_AVE:
			memcpy(td.b,RemoteValue.Pav[2][Phase].b,4);
			break;
		case REMOTE_COS:
			td.d = RemoteValue.Cos[Phase].l;
			break;
		case REMOTE_HZ:
			memcpy(td.b,RemoteValue.Freq.b,4);
			break;
	#if( SEL_VOL_ANGLE_FUNC == YES )
		case REMOTE_PHASEU:
			memcpy(td.b,RemoteValue.YUAngle[Phase-1].b,4);
			break;
		case REMOTE_PHASE:
			td.l = RemoteValue.YIAngle[Phase-1].d - RemoteValue.YUAngle[Phase-1].d;
			//电压或电流为0时相角置零
			if( (RemoteValue.U[Phase-1].d == 0) || (RemoteValue.I[Phase-1].d == 0) )
			{
				td.l = 0;
			}
			if( td.l < 0 )
			{
				td.l += 3600;
			}
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

	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			td.d = 0;
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
//				0x20--起动潜动状态	bit0~2	1--A/B/C相 1--潜动		0--起动
//				0x3X--当前相无功象限 返回1~4，代表一象限~四象限
//              0x40--当前总视在功率是否大于额定功率的千分之四                 0--大于	1--小于
//备注:   
//-----------------------------------------------
WORD api_GetSampleStatus(BYTE Type)
{
	BYTE PQDirect;
	WORD i,tw;
	TFourByteUnion td;
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
			tw = tl / (wStandardVoltageConst/10) / (dwMeterMaxCurrentConst) / 3;
			break;
		case 0x10:
			//电压状态	1--低于0.6Un	0--高于0.6Un
			for(i=0;i<3;i++)
			{
				if( (MeterTypesConst == METER_3P3W) && ( i == 1 ) )
				{
					continue;
				}
				
				if(RemoteValue.U[i].d < (wStandardVoltageConst * 100 * 6 / 10))
				{
					tw |= (0x0001<<i);
				}
			}

			Link_ReadSampleReg( PHASES_EMU, td.b ,2);
			
			if((td.b[0]&0x08) != 0 )
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
				
				if(labs( RemoteValue.I[i].l ) < ((DWORD)dwMeterBasicCurrentConst * 10 * 5 / 100))
				{
					tw |= (0x0001<<i);
				}
			}
			
			if( api_GetSysStatus( eSYS_STATUS_REVERSE_PHASE_I ) == TRUE )
			{
				tw |= 0x8000;
			}
			
			break;
		case 0x20://起动潜动状态
			//bit0,bit1,bit2分别代表ABC三相状态 0--起动 1--潜动
			tw = (WORD)(dwSampleSFlag&0x00000007);
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
			if(labs( RemoteValue.S[0].l ) >= (wStandardVoltageConst * dwMeterBasicCurrentConst * 4 * 3 / 10000)) //U*I*0.004*3
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
//-----------------------------------------------
//函数功能: 获取电流方向
//
//参数:		bBuf[in]	
//
//返回值: 	TRUE
//备注:   bit0~bit1表示A相，bit2~bit3表示B相，bit4~bit5表示C相；
//  对应字节位为00B表示功率（电流）从电网流向用户，01B表示功率（电流）从用户侧流向电网，
//  11B表示本开关不支持功率方向定义，其他值无效。
//-----------------------------------------------
WORD api_GetSampleCurrMinus(BYTE *bBuf)
{
	BYTE i,bTemp=0;
	WORD Result;
	
	for(i = 0; i < MAX_PHASE; i++)
	{
		if( RemoteValue.I[i].l < 0 )
		{
			bTemp |= (0x01<<(2*i));
		}
	}
	bBuf[0] = bTemp;
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 获取计量回路数
//
//参数:	无	
//
//返回值: 回路数	
//备注:   
//-----------------------------------------------
eMeasurementMode api_GetMeasureMentMode(void)
{	
	return samplerunpara.emeasurementmode;
}

//-----------------------------------------------
//函数功能: 设置计量回路数
//
//参数:	无	
//
//返回值: 成功/失败	
//备注:   
//-----------------------------------------------
BYTE api_SetSampleChip(BYTE bCount)
{	
	TSampleChipPara SampleChipPara;

	if( (bCount != 1) && (bCount != 3))
	{
		return FALSE;
	}
	memset(&SampleChipPara,0x00,sizeof(TSampleChipPara));

	SampleChipPara.bCount = bCount;
	
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleChipPara),sizeof(TSampleChipPara),(BYTE*)&SampleChipPara);

	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);

	api_SetSampleRunPara(eNoneIntegrator, ((bCount==1)?eOnePhase:eThreePhase));

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
	api_SetSampleChip(3);//初始化为三相
}

#endif// #if( SAMPLE_CHIP == CHIP_RN8302B )
