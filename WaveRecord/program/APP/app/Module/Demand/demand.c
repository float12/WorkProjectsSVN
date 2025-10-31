//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.30
//描述		需量计算文件
//修改记录	
//----------------------------------------------------------------------

#include "AllHead.h"

#if( MAX_PHASE == 3 )//只有三相表才打开需量功能
#if( SEL_DEMAND_2022 == NO )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#if( SEL_REACTIVE_DEMAND == YES )
	// 有功、各象限无功
	#define DEMAND_ITEM			2
#else
	// 有功
	#define DEMAND_ITEM			1
#endif

#if( SEL_APPARENT_DEMAND == YES )
	#undef DEMAND_ITEM
	// 有功、各象限无功、视在
	#define DEMAND_ITEM		3
#endif

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct TDemandRam_t
{
	//总需量
	TDemandDBase TotalDemand;
	
	//当前费率需量
	TDemandDBase RatioDemand;
	
	//冻结周期内最大需量
	#if(SEL_FREEZE_PERIOD_DEMAND == YES)
	TDemandDBase FreezePeriodDemand;
	#endif
	
	//当前需量 有功 无功 视在需量
	long CurrentDemand[DEMAND_ITEM];
	
}TDemandRam;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//内存中的需量
static TDemandRam DemandRam;

// 步长的秒数
static WORD SecondLength;

// 需量周期步数
static BYTE DemandMultiple;

// 当前费率
static BYTE RecordTariff;

// 电能方向标志位
//d0 不用
//d1 不用
//d2 不用
//d3 不用
//d4 有功方向 0：正向；1：反向
//d5 无功方向 0：正向；1：反向
static BYTE RecordFlag[2];//0:历史标志 1：当前标志

// 秒计数器
static WORD CalPoint[DEMAND_ITEM][2];

//有功、各象限无功、视在的总的及各费率的分步需量累积单元
static long StepDemand[DEMAND_ITEM][2][15];

//步长的累积功率
// 第一层：有功、各象限无功、视在
// 第二层：总的及各费率
static long CumulatePower[DEMAND_ITEM][2];

// 各量需量周期步数：
// 第一层：有功、各象限无功、视在
// 第二层：总的及各费率
static BYTE MinuteStep[DEMAND_ITEM][2];

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static BOOL DbReadDemandBin(WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand);
static BOOL DbReadPNRActiveDemand( WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand );
static void CountDemand(BYTE erg,BYTE tar);
static void CalPeriod( void );
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 记录备份上次有功和无功电能方向及费率
//
//参数: 	Type[in]	0:记录历史标志  1：记录当前标志
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void BackupEnergyFlag( BYTE Type )
{
	ASSERT( Type < 2, 0 );
	
	// 记录有功电能方向 0正1负
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE )
	{
		RecordFlag[Type] |= (0x10);
	}
	else
	{
		RecordFlag[Type] &= ~(0x10);
	}

	// 记录无功电能方向
	#if( SEL_REACTIVE_DEMAND == YES )
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q ) == TRUE )
	{
		RecordFlag[Type] |= 0x20;
	}
	else
	{
		RecordFlag[Type] &= ~(0x20);
	}
	#endif

	// 记录当前费率
	if( Type == 0 )
	{
		RecordTariff = api_GetCurRatio();
	}
}


//-----------------------------------------------
//函数功能: 获取具体类型需量在对应需量基本单元的偏移地址
//
//参数: 
//	Type[in]:	P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//              APPARENT_P	10		// 正向视在
//				APPARENT_N	11		// 反向视在
//返回值: 	偏移地址
//
//备注:   
//-----------------------------------------------
static WORD GetTDemandDbaseOffset(BYTE Type)
{
 	WORD Addr = 0;

 	switch(Type)
 	{
 		case P_ACTIVE:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Active[0] );
 			break;
 		case N_ACTIVE:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Active[1] );
 			break;

 		#if( SEL_REACTIVE_DEMAND == YES )
 		case RACTIVE1:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, RActive[0] );
 			break;
 		case RACTIVE2:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, RActive[1] );
 			break;
 		case RACTIVE3:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, RActive[2] );
 			break;
 		case RACTIVE4:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, RActive[3] );
 			break;
 		#endif

 		#if( SEL_APPARENT_DEMAND == YES )
 		case APPARENT_P:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Apparent[0] );
 			break;
 		case APPARENT_N:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Apparent[1] );
 			break;
 		#endif

 		default:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Active[0] );
 			ASSERT( 0!=0, 0 );
 			break;
 	}

 	return Addr;
}


#if(SEL_FREEZE_PERIOD_DEMAND == YES)
//-----------------------------------------------
//函数功能: 判断期间需量冻结周期时间是否到，时间到后需量清零
//
//参数: 	无
//
//返回值: 	无
//
//备注:   每分钟调用一次
//-----------------------------------------------
static void DealFreezePeriodDemand( void )
{
	//单位如果不为分钟
	if( FPara2->EnereyDemandMode.IntervalDemandFreezePeriod.Unit != 1 )
	{
		return;
	}
	
	//限制周期不大于1天
	if( FPara2->EnereyDemandMode.IntervalDemandFreezePeriod.FreezePeriod > 1440 )
	{
		return;
	}
	
	//限制周期不能为0
	if( FPara2->EnereyDemandMode.IntervalDemandFreezePeriod.FreezePeriod == 0 )
	{
		return;
	}
	
	//判断期间需量冻结周期时间是否到，时间到置当前需量
	if( (g_RelativeMin%FPara2->EnereyDemandMode.IntervalDemandFreezePeriod.FreezePeriod) == 0 )
	{
		memcpy( (void *)&DemandRam.FreezePeriodDemand, (void *)&DemandRam.TotalDemand, sizeof(DemandRam.FreezePeriodDemand) );
	}
}


//-----------------------------------------------
//函数功能: 更新期间需量
//
//参数: 	
//	Type[in]:	P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//              APPARENT_P	10		// 正向视在
//				APPARENT_N	11		// 反向视在
//	pDemand[in]	需量基本类型
//
//返回值: 	无
//
//备注:   每次更新需量，同步判断是否需要更新期间需量
//-----------------------------------------------
static void DbWriteFreezePeriodDemand( BYTE Type, TDemandData *pDemand )
{
	BYTE *pDemandData=0;
	
	pDemandData = (BYTE *)&DemandRam.FreezePeriodDemand;
	pDemandData += GetTDemandDbaseOffset( Type );
	if( pDemand->DemandValue > ((TDemandData *)pDemandData)->DemandValue )
	{
		memcpy( pDemandData, pDemand, sizeof(TDemandData) );
		DemandRam.FreezePeriodDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.FreezePeriodDemand), sizeof(TDemandDBase)-sizeof(DWORD));
	}
}


//-----------------------------------------------
//函数功能: 读期间最大需量包括发生时间
//
//参数: 
//	Type[in]:	P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//              APPARENT_P	10		// 正向视在
//				APPARENT_N	11		// 反向视在
//	Dot[in]:	小数点位数
//	pDemand[out]:输出数据
//
//返回值:	TRUE/FALSE

//备注: 
//-----------------------------------------------
static BOOL DbReadFreezePeriodDemandHex(WORD Type, BYTE Dot, BYTE * pDemand)
{
	BYTE *pDemandData=0;
	TDemandData TmpDemandData;
	
	if( Dot == 0xff )
	{
		Dot = 4;
	}
	ASSERT( (Dot<=6)&&(Dot>=3), 0 );//按kw 4 5 6考虑，eeprom是w 三位小数
	if( Dot > 6 )
	{
		Dot = 4;
	}
	
	#if( SEL_REACTIVE_DEMAND == YES )
	if( (Type == P_RACTIVE) || (Type == N_RACTIVE) )
	{
		return DbReadPNRActiveDemand( Type|0x8000, 0, Dot, pDemand );
	}
	#endif

	pDemandData = (BYTE *)&DemandRam.FreezePeriodDemand;
	pDemandData += GetTDemandDbaseOffset( Type );
	memcpy( (BYTE *)&TmpDemandData, (BYTE*)pDemandData, sizeof(TmpDemandData) );
	if( TmpDemandData.DemandValue != 0xffffffff )
	{
		TmpDemandData.DemandValue /= (long)lib_MyPow10(6-Dot);
	}

	memcpy( (void*)pDemand, (void*)&TmpDemandData, sizeof(TmpDemandData) );

	return TRUE;
}
#endif//#if(SEL_FREEZE_PERIOD_DEMAND == YES)


#if( SEL_REACTIVE_DEMAND == YES )
//-----------------------------------------------
//函数功能: 判断给定象限是否属于无功模式字内
//
//参数: 
//	Mode[in]:		无功组合模式字
//	Quadrant[in]:	指定象限
//
//返回值:	TRUE--给定象限属于无功模式字内
//			FALSE--给定象限不属于无功模式字内
//备注: 
//-----------------------------------------------
static BOOL QuadrantIsInMode( BYTE Mode, BYTE Quadrant )
{
	if( ((Mode>>(2*Quadrant))&0x03) == 0 )
	{
		return FALSE;
	}
	
	return TRUE;
}


//-----------------------------------------------
//函数功能: 获取需量类型
//
//参数: 
//	erg[in]:	有功/无功/视在 类型
//	tar[in]:	总/费率
//
//返回值:	需量类型	正有/反有/1无/2无/3无/4无/正视在/反视在
//			
//备注: 
//-----------------------------------------------
BYTE GetDemandType(BYTE erg,BYTE tar)
{
	BYTE i,Type;
	long lTmpDemand;
	
	//根据所传的参数确定当前结算的是什么量，从而加载相应的数据
	if(erg == 0)// 有功
	{
		if(RecordFlag[0]&0x10)
		{
			Type = N_ACTIVE;
		}
		else
		{
			Type = P_ACTIVE;
		}
	}
	#if( SEL_REACTIVE_DEMAND == YES )
	else if(erg == 1)// 各象限无功
	{
		switch(RecordFlag[0]&0x30)
		{
		case 0x00://一相限
			Type = RACTIVE1;
			break;
		case 0x10://二相限
			Type = RACTIVE2;
			break;
		case 0x20://四相限
			Type = RACTIVE4;
			break;
		default://三相限
			Type = RACTIVE3;
		}
	}
	#endif
	#if( SEL_APPARENT_DEMAND == YES )
	else// 视在
	{
		if(RecordFlag[0]&0x10)
		{
			Type = APPARENT_N;
		}
		else
		{
			Type = APPARENT_P;
		}
	}
	#endif
		
	
	if( api_GetProHardFlag( ePRO_HARD_DEMAND_CONTINUE_MEASURE ) == TRUE )
	{
		lTmpDemand = 0;
		for(i=0; i<DemandMultiple; i++)
		{
			lTmpDemand += StepDemand[erg][tar][i];
		}
			
		if( erg == 0 )
		{
			if( lTmpDemand < 0 )
			{
				Type = N_ACTIVE;
			}
			else
			{
				Type = P_ACTIVE;
			}
		}
		#if( SEL_APPARENT_DEMAND == YES )
		else if( erg == 2 )
		{
			if( lTmpDemand < 0 )
			{
				Type = APPARENT_N;
			}
			else
			{
				Type = APPARENT_P;
			}
		}
		#endif
	}
	
	return Type;
}


//-----------------------------------------------
//函数功能: 读正反无功需量
//
//参数: 
//	Type[in]:		无功组合模式字
//						bit7为1表示读冻结周期需量， 0--表示读普通需量
//						P_ACTIVE	1
//						N_ACTIVE	2
//						P_RACTIVE	3
//						N_RACTIVE	4
//						RACTIVE1	5
//						RACTIVE2	6
//						RACTIVE3	7
//						RACTIVE4	8
//		              	APPARENT_P	10		// 正向视在
//						APPARENT_N	11		// 反向视在
//	Ratio[in]:		费率
//	Dot[in]:		小数位数
//	pDemand[out]:	需量数据
//
//返回值:	TRUE/FALSE
//备注: 不再存储正反无功数据，直接从四象限中获取
//-----------------------------------------------
static BOOL DbReadPNRActiveDemand( WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand )
{
	BYTE i,ReactiveMode,byPNRactiveDemandDir,DemandType;
    long DemandValue;
	TDemandData TmpDemandData1,TmpDemandData2;

	if(Dot > 6)//如果需量小数位数大于6,需量按照4位小数返回
	{
		Dot = 4;
	}
	DemandType = (Type&0x7f);
	if( DemandType == P_RACTIVE )
	{
		if( FPara2->EnereyDemandMode.PReactiveMode == 0x00 )
		{
			ReactiveMode = 0x01;//模式字为0，读组合无功1则读第一象限的需量
		}
		else
		{
			ReactiveMode = FPara2->EnereyDemandMode.PReactiveMode;
		}
	}
	else if( DemandType == N_RACTIVE )
	{
		if( FPara2->EnereyDemandMode.NReactiveMode == 0x00 )
		{
			ReactiveMode = 0x10;//模式字为0，读组合无功2则读第三象限的需量
		}
		else
		{
			ReactiveMode = FPara2->EnereyDemandMode.NReactiveMode;
		}
	}
	else
	{
        return FALSE;
	}

	//组合无功需量方向先赋0，表示正向
	byPNRactiveDemandDir = 0;//0-正向 1-负向 读时置位，用后复零
	memset( (void*)&TmpDemandData1, 0x00, sizeof(TDemandData) );//先清零
	for(i=0; i<4; i++)
	{
		//组合无功1需量、组合无功2需量按组合无功的方式判断
		if( QuadrantIsInMode( ReactiveMode, i ) == FALSE )
		{
			continue;
		}

		#if( ( SEL_REACTIVE_DEMAND == YES )&&( SEL_FREEZE_PERIOD_DEMAND == YES ) ) 
		if(Type & 0x8000)
		{
			memcpy( (BYTE *)&TmpDemandData2, (BYTE *)&DemandRam.FreezePeriodDemand.RActive[i], sizeof(TDemandData));
			TmpDemandData2.DemandValue /= (long)lib_MyPow10(6-Dot);
		}
		else
		#endif
		{
			DbReadDemandBin( RACTIVE1+i, Ratio, Dot, (BYTE *)&TmpDemandData2 );
		}
		
		if( TmpDemandData2.DemandValue > TmpDemandData1.DemandValue )
		{
			TmpDemandData1 = TmpDemandData2;

			//组合无功需量方向
			if( i <= 1 )//I象限 II象限
			{
				byPNRactiveDemandDir = 0;//0-正向 1-负向 读时置位，用后复零
			}
			else//III象限 IV象限
			{
				byPNRactiveDemandDir = 1;//0-正向 1-负向 读时置位，用后复零
			}
		}
	}

	if( TmpDemandData1.DemandValue == 0xffffffff )
	{
		memset( (void*)&TmpDemandData1, 0xff, sizeof(TDemandData) );
	}
	else
	{
		DemandValue = TmpDemandData1.DemandValue;
		if( byPNRactiveDemandDir == 1 )
		{
			DemandValue *= (-1);
		}
		TmpDemandData1.DemandValue = DemandValue;
	}
	
	memcpy( (BYTE*)pDemand, (BYTE*)&TmpDemandData1, sizeof(TDemandData) );

	return TRUE;
}

#endif//#if( SEL_REACTIVE_DEMAND == YES )


//-----------------------------------------------
//函数功能:方向或费率发送变化时，清需量 
//
//参数: 	i[in]	需量类型	0--有功需量 1--无功需量	2--视在需量
//          j[in]	方向		0--正向需量	1--反向需量          
//返回值: 	无
//
//备注:清对应变量，当为总时并清当前需量
//-----------------------------------------------
static void ChangeCount(BYTE i,BYTE j)
{
	BYTE k,Type;
	DWORD dwAddr;
	TDemandData * p1;

	//步长秒计数器清零
	CalPoint[i][j] = 0;
	//步计数器清零
	MinuteStep[i][j] = 0;
	//步长功率累加清零
	CumulatePower[i][j] = 0;
	//各步长平均功率清零
	for(k=0; k<15; k++)
	{
		StepDemand[i][j][k] = 0;
	}

	//若为总，清对应的当前需量
	if( j == 0 )
	{
		memset( (void *)&DemandRam.CurrentDemand[i], 0x00, sizeof(DWORD) );
	}
	else
	{
		Type = GetDemandType(i,j);
		dwAddr = GetTDemandDbaseOffset(Type);
		p1 = (TDemandData *)((DWORD)dwAddr + (DWORD)&DemandRam.RatioDemand);
		memset( (void*)p1, 0x00, sizeof(TDemandData) );
		DemandRam.RatioDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
	}
}


//-----------------------------------------------
//函数功能: 方向未变化计算需量
//
//参数: 	i[in]	需量类型	0--有功需量 1--无功需量	2--视在需量
//          j[in]	方向		0--正向需量	1--反向需量 
//                    
//返回值: 	无
//
//备注:秒任务调用，当判断够一个步长时间时，刷新对应各步长平均功率并计算当前需量   
//-----------------------------------------------
static void NoChangeCount(BYTE i, BYTE j)
{
	CalPoint[i][j]++;
	if( CalPoint[i][j] == SecondLength )
	{
		StepDemand[i][j][MinuteStep[i][j] % DemandMultiple] = ((CumulatePower[i][j]/SecondLength));

		MinuteStep[i][j]++;

		//由于上电后MinuteStep为0，所以不够一个周期的情况下不会计算需量
		if( MinuteStep[i][j] < DemandMultiple )
		{
		}
		else
		{
			//执行过一个需量周期后，把MinuteStep初始化为15，这样以后每个滑差时间都会计算一次数据
			if( MinuteStep[i][j] >= (DemandMultiple*2) )
			{
				MinuteStep[i][j] = DemandMultiple;
			}

			CountDemand(i,j);//结算

			if( (i==0) && (j==0) )
			{
				api_MultiFunPortCtrl( eDEMAND_PERIOD_OUTPUT );
			}
		}

		CumulatePower[i][j] = 0;//清寄存器
		CalPoint[i][j] = 0;//秒计数器清零
	}
}

//-----------------------------------------------
//函数功能: 需量计算的主函数，每秒执行一次
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_DemandTask( void )
{
	BYTE i,j;
	long lTmp[3];
	
	//上电后延时5秒再开始计算需量
	if( g_PowerOnSecTimer >= 25 )
	{
		return;
	}
	
	if( api_GetTaskFlag(eTASK_DEMAND_ID,eFLAG_SECOND) == TRUE )//到了一秒钟
	{
		api_ClrTaskFlag(eTASK_DEMAND_ID,eFLAG_SECOND);//清秒标志
	}
	else
	{
		return;
	}
	
	#if(SEL_FREEZE_PERIOD_DEMAND == YES)
	if( api_GetTaskFlag( eTASK_DEMAND_ID, eFLAG_MINUTE ) == TRUE )
	{
		//清分钟标志
		api_ClrTaskFlag(eTASK_DEMAND_ID,eFLAG_MINUTE);
		//定期同步需量周期步数及滑差时间
		CalPeriod();
		DealFreezePeriodDemand();
	}
	#endif
	
	BackupEnergyFlag( 1 );
	
	//需量数据源统一按照5位小数保存，6位小数按照220v，60A，3相，3位小数，单位w，60秒的和会溢出
	api_GetRemoteData( PHASE_ALL+REMOTE_P, DATA_HEX, 5, 4, (BYTE *)&lTmp[0] );
	api_GetRemoteData( PHASE_ALL+REMOTE_Q, DATA_HEX, 5, 4, (BYTE *)&lTmp[1] );
	api_GetRemoteData( PHASE_ALL+REMOTE_S, DATA_HEX,   5, 4, (BYTE *)&lTmp[2] );
	if( api_GetProHardFlag( ePRO_HARD_DEMAND_CONTINUE_MEASURE ) == TRUE )
	{
		RecordFlag[0] &= ~0x10;//让有功方向不再改变
		if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE )
		{
			RecordFlag[0] |= 0x10;
			lTmp[0] *= -1;
			lTmp[2] *= -1;
		}
		else
		{
			//无操作
		}
	}
	else
	{
		//无操作
	}
	
	// 如果电能方向没变
	if( (RecordFlag[0]&0x30)==(RecordFlag[1]&0x30) )
	{
		// 如果费率没变
		if( RecordTariff == api_GetCurRatio() )
		{
			for(i=0; i<DEMAND_ITEM; i++)
			{
				for(j=0; j<2; j++)
				{
					CumulatePower[i][j] += lTmp[i];
					
					NoChangeCount(i,j);
				}
			}
		}
		else// 如果费率变了
		{
			for(i=0; i<DEMAND_ITEM; i++)
			{
				for(j=0; j<2; j++)
				{	
					//对各费率先结算以前的需量再清零
					if( j==1 )//各费率的
					{
						ChangeCount(i,j);
					}
					else//对总的来说变费率不用变
					{						
						CumulatePower[i][j] += lTmp[i];
						
						NoChangeCount(i,j);
					}
				}
			}
		}
	}
	else// 如果电能方向变了
	{
		// 如果有功没变  无功变了
		if( (RecordFlag[0]&0x10)==(RecordFlag[1]&0x10) )
		{
			// 如果费率没变
			if( RecordTariff == api_GetCurRatio() )
			{
				for(i=0; i<DEMAND_ITEM; i++)
				{
					for(j=0; j<2; j++)
					{
						if(i == 1)// 无功及各相限的
						{
							// 先结算以前的再清零
							ChangeCount(i,j);
						}
						else// 有功，视在方向不变
						{
							CumulatePower[i][j] += lTmp[i];
							
							NoChangeCount(i,j);
						}
					}
				}
			}
			else// 如果费率变了
			{
				for(i=0; i<DEMAND_ITEM; i++)
				{
					for(j=0; j<2; j++)
					{
						// 对有功总的单独处理
						if( (i==0)&&(j==0) )
						{
							CumulatePower[i][j] += lTmp[i];
							
							NoChangeCount(i,j);
						}
						// 视在单独处理
						#if( SEL_APPARENT_DEMAND == YES )
						else if( (i==2)&&(j==0) )
						{
							CumulatePower[i][j] += lTmp[i];
							
							NoChangeCount(i,j);
						}
						#endif
						else
						{
							//否则的话对有功、相限无功、视在各费率的进行先结算后清零
							ChangeCount(i,j);
						}
					}
				}
			}
		}
		else
		{
			//没有正反向无功了，正反向无功从四象限中获取 所以有功变化了，象限必定变化
			for(i=0; i<DEMAND_ITEM; i++)
			{
				for(j=0; j<2; j++)
				{
					ChangeCount(i,j);
				}
			}
		}
	}

	//记录标志
	BackupEnergyFlag( 0 );
}


//-----------------------------------------------
//函数功能: 读当前或分费率最大需量数据
//
//参数: 
//	Type[in]:		P_ACTIVE	1
//					N_ACTIVE	2
//					P_RACTIVE	3
//					N_RACTIVE	4
//					RACTIVE1	5
//					RACTIVE2	6
//					RACTIVE3	7
//					RACTIVE4	8
//		            APPARENT_P	10		// 正向视在
//					APPARENT_N	11		// 反向视在
//	Ratio[in]:		费率
//	Dot[in]:		小数位数
//	pDemand[out]:	需量数据
//
//返回值:	TRUE/FALSE
//备注:当前最大需量都保存在eeprom中 
//-----------------------------------------------
static BOOL DbReadDemandBin(WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand)
{
	DWORD dwAddr,DemandValue;
	TDemandDBase TmpDemandDBase;
	BYTE *pDemandData=0;
	
	ASSERT( Ratio <= MAX_RATIO, 0 );

	if( Ratio == 0 )
	{
		//总需量
		dwAddr = GET_SAFE_SPACE_ADDR( DemandSafeRom.TotalDemand );
	}
	else
	{
		//对应费率需量
		dwAddr = GET_SAFE_SPACE_ADDR( DemandSafeRom.RatioDemand[Ratio-1] );
	}
	
	api_VReadSafeMem( dwAddr, sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
	
	if( Dot == 0xff )//需量默认按照四位小数返回
	{
		Dot = 4;
	}
	if(Dot > 6)//请求需量的小数位数大于6按照默认小数位数返回
	{
		Dot = 4;
	}

	pDemandData = (BYTE *)&TmpDemandDBase;
	pDemandData += GetTDemandDbaseOffset( Type );
	
	if( Dot == 4 )//小数位数为4位进行进位
	{   
	    //若开关打开，则将最后一位进位
    	if( api_GetProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) == TRUE )
    	{
    	    if( (((TDemandData *)pDemandData)->DemandValue) != 0 )
    	    {
    		    ((TDemandData *)pDemandData)->DemandValue += 50;
    	    }
    	}
	}

	DemandValue = (((TDemandData *)pDemandData)->DemandValue);
	
	if(  DemandValue != 0xffffffff )
	{
		((TDemandData *)pDemandData)->DemandValue /= (long)lib_MyPow10(6-Dot);
	}
	
	memcpy( (void*)pDemand, (void*)pDemandData, sizeof(TDemandData) );

	return TRUE;
}


//-----------------------------------------------
//函数功能: 刷新保存的当前最大需量
//
//参数: 
//	Type[in]:		P_ACTIVE	1
//					N_ACTIVE	2
//					P_RACTIVE	3
//					N_RACTIVE	4
//					RACTIVE1	5
//					RACTIVE2	6
//					RACTIVE3	7
//					RACTIVE4	8
//		            APPARENT_P	10		// 正向视在
//					APPARENT_N	11		// 反向视在
//	Ratio[in]:		费率
//	pDemand[in]:	需量数据
//
//返回值:	无
//备注:若当前需量大于ee保存的最大需量，则更新ee保存的对应最大需量
//		若为总，则同步更新期间需量及当前最大需量
//-----------------------------------------------
static void DbWriteDemand(WORD Type, WORD Ratio, TDemandData *pDemand)
{
	DWORD dwAddr;
	TDemandData TmpDemandData;
	TDemandDBase TmpDemandDBase;
	TDemandData * p1;

	//判断是否为当前费率
	if( Ratio==api_GetCurRatio() )
	{
		dwAddr = GetTDemandDbaseOffset(Type);
		p1 = (TDemandData *)((DWORD)dwAddr + (DWORD)&DemandRam.RatioDemand);

		//当前费率处理：直接更新RAM中的最大费率
		if( (pDemand->DemandValue) > (p1->DemandValue) )
		{
			memcpy( (void*)p1, (void*)pDemand, sizeof(TDemandData) );
			//计算校验
			DemandRam.RatioDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
			//直接写eeprom，这样停电就不用再写了
			if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(DemandSafeRom.RatioDemand[Ratio-1]), sizeof(TmpDemandDBase), (BYTE *)&TmpDemandDBase ) != FALSE )
			{
				memcpy( (BYTE *)&TmpDemandData, (BYTE*)(dwAddr+(BYTE*)&TmpDemandDBase), sizeof(TmpDemandData) );
				if( p1->DemandValue > TmpDemandData.DemandValue )
				{
					memcpy( (BYTE*)(dwAddr+(BYTE*)&TmpDemandDBase), (BYTE*)p1, sizeof(TmpDemandData) );
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(DemandSafeRom.RatioDemand[Ratio-1]), sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
				}
			}
		}
	}
	//处理总
	else if( Ratio == 0 )
	{
		dwAddr = GetTDemandDbaseOffset(Type);
		p1 = (TDemandData *)((DWORD)dwAddr + (DWORD)&DemandRam.TotalDemand);

		//当前总需量处理：直接更新RAM中的最大需量
		if( (pDemand->DemandValue) > (p1->DemandValue) )
		{
			memcpy( (void*)p1, (void*)pDemand, sizeof(TDemandData) );
			//计算校验
			DemandRam.TotalDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.TotalDemand), sizeof(TDemandDBase)-sizeof(DWORD));
			//直接写eeprom，这样停电就不用再写了
			if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(DemandSafeRom.TotalDemand), sizeof(TmpDemandDBase), (BYTE *)&TmpDemandDBase ) != FALSE )
			{
				memcpy( (BYTE *)&TmpDemandData, (BYTE*)(dwAddr+(BYTE*)&TmpDemandDBase), sizeof(TmpDemandData) );
				if( p1->DemandValue > TmpDemandData.DemandValue )
				{
					memcpy( (BYTE*)(dwAddr+(BYTE*)&TmpDemandDBase), (BYTE*)p1, sizeof(TmpDemandData) );
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(DemandSafeRom.TotalDemand), sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
				}
			}
		}
		
		#if(SEL_FREEZE_PERIOD_DEMAND == YES)
		DbWriteFreezePeriodDemand( Type, pDemand );
		#endif
		
		//写入当前需量（瞬时量）
		switch( Type )
		{
		case P_ACTIVE:
			DemandRam.CurrentDemand[0] = pDemand->DemandValue;
			break;
		case N_ACTIVE:
			DemandRam.CurrentDemand[0] = ~pDemand->DemandValue;//负数
			DemandRam.CurrentDemand[0] += 1;
			break;
		#if( SEL_REACTIVE_DEMAND == YES )
		case RACTIVE1:
		case RACTIVE2:
			DemandRam.CurrentDemand[1] = pDemand->DemandValue;
			break;
		case RACTIVE3:
		case RACTIVE4:
			DemandRam.CurrentDemand[1] = ~pDemand->DemandValue;//负数
			DemandRam.CurrentDemand[1] += 1;
			break;
		#endif
		#if( SEL_APPARENT_DEMAND == YES )
		case APPARENT_P:
			DemandRam.CurrentDemand[2] = pDemand->DemandValue;
			break;
		case APPARENT_N:
			DemandRam.CurrentDemand[2] = ~pDemand->DemandValue;//负数
			DemandRam.CurrentDemand[2] += 1;
			break;
		#endif
		default:
			break;
		}
	}
}


//-----------------------------------------------
//函数功能: 计算需量值
//
//参数: 	erg[in]		电能类型 0:有功  1:各相限无功  2:视在
//			tar[in]		费率类型 0:总的  1:各费率的
//                    
//返回值: 	无
//
//备注: 需量保存6位小数到EEPROM  
//-----------------------------------------------
static void CountDemand(BYTE erg,BYTE tar)
{
	BYTE i,Type;
	long lTmpDemand;
	TDemandData	TmpDemandData;

	Type = GetDemandType(erg,tar); 

	lTmpDemand = 0;
	for(i=0; i<DemandMultiple; i++)
	{
		lTmpDemand += StepDemand[erg][tar][i];
	}
	
	if( api_GetProHardFlag( ePRO_HARD_DEMAND_CONTINUE_MEASURE ) == TRUE )
	{
		if( (erg==0) || (erg==2) )
		{
			if( lTmpDemand < 0 )
			{
				lTmpDemand *= -1;
			}
		}
	}

	TmpDemandData.DemandValue = lTmpDemand;

	//eeprom中的需量改为存储6位小数点，单位kw
	TmpDemandData.DemandValue *= 10;
	TmpDemandData.DemandValue /= (long)DemandMultiple;
	api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );
	
	DbWriteDemand(Type, RecordTariff*tar, &TmpDemandData );
	if( tar != 0 )
	{
		//把费率需量再向总需量写一次，确保总需量大于费率需量
		DbWriteDemand(Type, 0, &TmpDemandData );
	}
}


//-----------------------------------------------
//函数功能: 上电后确定需量周期、滑差
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   上电后读出参数后及通讯设置参数后调用该函数
//-----------------------------------------------
static void CalPeriod( void )
{
	BYTE    Flag;
	BYTE    Step, Period;

	Step = FPara2->EnereyDemandMode.DemandSlip;
	Period = FPara2->EnereyDemandMode.DemandPeriod;

	Flag = TRUE;

	//滑差不大于60分钟
	if( (Step == 0) || (Step > 60) )
	{
		Flag = FALSE;
	}

	//周期不大于60分钟
	if( (Period == 0) || (Period > 60) )
	{
		Flag = FALSE;
	}

	if( Period < Step )
	{
		Flag = FALSE;
	}

	//要求周期和滑差比不大于15
	if( (Period/Step) > 15 )
	{
		Flag = FALSE;
	}

	if( Flag == FALSE )
	{
		DemandMultiple = 15;
		SecondLength = 60;//60秒
	}
	else
	{
		DemandMultiple = Period/Step;
		SecondLength = Step*60;
	}

	
}

//-----------------------------------------------
//函数功能: 清RAM中的需量数据
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   在上电及规约触发初始化需量时调用
//-----------------------------------------------
static void ClearDemandRamData(void)
{
	BYTE i,j;
	
	memset(StepDemand, 0, sizeof(StepDemand));

	for(i=0;i<DEMAND_ITEM;i++)
	{
		for(j=0;j<2;j++)
		{
			CumulatePower[i][j] = 0;
			CalPoint[i][j] = 0;
			MinuteStep[i][j] = 0;
		}
	}
}
//-----------------------------------------------
//函数功能: 需量模块初始化
//
//参数: 	   无
//                    
//返回值:     无
//
//备注:   
//-----------------------------------------------
void api_InitDemand( void )
{
	// 确定需量周期、滑差
	CalPeriod();
	
	ClearDemandRamData();

	//清内存中当前最大需量和当前需量
	api_ClrDemand( eClearRamDemand);

	// 记录方向标志
	BackupEnergyFlag( 0 );
}
//-----------------------------------------------
//函数功能: 需量模块上电初始化
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_PowerUpDemand( void )
{		
	// 确定需量周期、滑差
	CalPeriod();
	
	ClearDemandRamData();

	//清内存中当前最大需量和当前需量
	api_ClrDemand( eClearRamDemand );

	// 记录方向标志
	BackupEnergyFlag( 0 );
}

//-----------------------------------------------
//函数功能: 读最大需量包括发生时间
//
//参数: 
//	Type[in]:	Bit15		1--冻结周期最大需量	0--非冻结周期最大需量
//				P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//              APPARENT_P	10		// 正向视在
//				APPARENT_N	11		// 反向视在
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//  Ratio[in]:	费率
//				1-MAX_RATIO	 其它个费率
//				0				 各费率总计             
//	Dot[in]:	小数点位数
//	Buf[out]:	输出数据
//
//返回值:	TRUE/FALSE

//备注: 
//-----------------------------------------------
BOOL api_GetDemand(WORD Type, BYTE DataType, WORD Ratio, BYTE Dot, BYTE * Buf)
{		
	BOOL ReturnStatus;
	TDemandData DemandData;
	
	if(Type & 0x8000)
	{
		#if(SEL_FREEZE_PERIOD_DEMAND == YES)
		ReturnStatus = DbReadFreezePeriodDemandHex(Type&(~0x8000), Dot, (BYTE*)&DemandData);
		#else
		return FALSE;
		#endif
	}	
	else if( (Type == P_RACTIVE) || (Type == N_RACTIVE) )
	{
	    #if( SEL_REACTIVE_DEMAND == YES )
		ReturnStatus = DbReadPNRActiveDemand( Type, Ratio, Dot, (BYTE*)&DemandData );
        #else
        return FALSE;
        #endif
	}
	else
	{
		ReturnStatus = DbReadDemandBin( Type, Ratio, Dot, (BYTE*)&DemandData );
	}
	
	if( DataType == DATA_BCD )
	{
		if( DemandData.DemandValue < 0 )
		{
			DemandData.DemandValue *= -1;
		}
		DemandData.DemandValue = lib_DWBinToBCD( DemandData.DemandValue );
		
		DemandData.DemandTime.wYear = lib_WBinToBCD( DemandData.DemandTime.wYear );
		DemandData.DemandTime.Mon = lib_BBinToBCD( DemandData.DemandTime.Mon );
		DemandData.DemandTime.Day = lib_BBinToBCD( DemandData.DemandTime.Day );
		DemandData.DemandTime.Hour = lib_BBinToBCD( DemandData.DemandTime.Hour );
		DemandData.DemandTime.Min = lib_BBinToBCD( DemandData.DemandTime.Min );
		DemandData.DemandTime.Sec = lib_BBinToBCD( DemandData.DemandTime.Sec );
	}
	else if( DataType == DATA_HEX )
	{
		if( DemandData.DemandValue < 0 )
		{
			DemandData.DemandValue *= -1;
		}
	}
	else if( DataType == DATA_HEXCOMP )
	{
	}
	
	memcpy( Buf, (BYTE*)&DemandData, sizeof(TDemandData) );
	
	return ReturnStatus;
}


//-----------------------------------------------
//函数功能: 读当前总需量（不支持费率需量）
//
//参数: 
//	Type[in]:	1  	有功
//				2	无功
//				3	视在 
//	DataType[in]:	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)      
//	Dot[in]:	小数点位数
//	Len[in]:    读取需量长度
//	pDemand[out]:输出数据
//
//返回值:	无

//备注: DATA_BCD格式最高位位符号位
//-----------------------------------------------
void api_GetCurrDemandData(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE * pDemand)
{
	TFourByteUnion tl;
	BYTE BackFlag;
	BYTE Len1;
	if(Dot > 6)
	{
		Dot = 4;
	}
	Len1 = Len;
	if( (Len != 3)&&(Len != 4) ) 
	{
		Len1 = 4;
	}	
	
	tl.l = (long)DemandRam.CurrentDemand[Type-1];
	
	tl.l /= (long)lib_MyPow10(6-Dot);
	
	if( DataType == DATA_BCD )
	{
		BackFlag = 0;
		if( tl.l < 0 )
		{
			tl.l *= -1;
			BackFlag = 1;
		}
		tl.d = lib_DWBinToBCD( tl.d );
		
		//反向最高位置符号
		if( BackFlag == 1 )
		{
			tl.b[Len1 - 1] |= 0x80;
		}
	}
	else if( DataType == DATA_HEX )
	{
		if( tl.l < 0 )
		{
			tl.l *= -1;
		}
	}
	else if( DataType == DATA_HEXCOMP )
	{
		
	}
	
	memcpy( (BYTE*)pDemand, tl.b, Len );
}
//-----------------------------------------------
//函数功能: 需量清零
//
//参数: 	eType[in]：		要清需量的类型
//      eClearRamDemand	清当前需量、清内存中当前最大需量
//	    eClearAllDemand	清eeprom中当前最大需量		
//
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_ClrDemand( eClearDemandType eType )
{
	BYTE i;
	TDemandDBase TmpDemandDBase;
	
	if( (eType == eClearRamDemand) || (eType == eClearAllDemand) )//清除RAM中需量，以及最大需量
	{
		memset( (void *)&DemandRam.CurrentDemand, 0x00, sizeof(DemandRam.CurrentDemand) );
		memset( (void *)&DemandRam.TotalDemand, 0x00, sizeof(TDemandDBase) );
		DemandRam.TotalDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.TotalDemand), sizeof(TDemandDBase)-sizeof(DWORD));
		memset( (void *)&DemandRam.RatioDemand, 0x00, sizeof(TDemandDBase) );
		DemandRam.RatioDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
		#if(SEL_FREEZE_PERIOD_DEMAND == YES)
		memset( (void *)&DemandRam.FreezePeriodDemand, 0x00, sizeof(TDemandDBase) );
		DemandRam.FreezePeriodDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.FreezePeriodDemand), sizeof(TDemandDBase)-sizeof(DWORD));
		#endif
	}
	
	if( eType == eClearAllDemand )//清除e2中的需量
	{
		memset( (void *)&TmpDemandDBase, 0x00, sizeof(TDemandDBase) );
		TmpDemandDBase.CRC32  = lib_CheckCRC32((BYTE *)&(TmpDemandDBase), sizeof(TDemandDBase)-sizeof(DWORD));
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( DemandSafeRom.TotalDemand ), sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
		
		for(i=0; i<MAX_RATIO; i++)
		{
			memset( (void *)&TmpDemandDBase, 0x00, sizeof(TDemandDBase) );
			TmpDemandDBase.CRC32  = lib_CheckCRC32((BYTE *)&(TmpDemandDBase), sizeof(TDemandDBase)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( DemandSafeRom.RatioDemand[i] ), sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
			CLEAR_WATCH_DOG;
		}

		// 确定需量周期、滑差
		CalPeriod();
		//清除步进需量
		ClearDemandRamData();
		// 记录方向标志
		BackupEnergyFlag( 0 );
	}
}


#endif//#if( MAX_PHASE == 3 )
#endif