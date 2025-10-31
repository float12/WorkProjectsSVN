//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.30
//描述		需量计算文件
//修改记录	
//----------------------------------------------------------------------

#include "AllHead.h"


#if( MAX_PHASE == 3 )//只有三相表才打开需量功能
#if( SEL_DEMAND_2022 == YES )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//需量周期对滑差时间的最大倍数
#define MAX_DEMANDPERIOD_MUM		60

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

typedef struct TDemandRam_t
{
	//总最大需量
	TDemandDBase TotalDemand;
	
	//RAM中当前费率的最大需量
	TDemandDBase RatioDemand;
	
	//冻结周期内最大需量
	#if(SEL_FREEZE_PERIOD_DEMAND == YES)
	TDemandDBase FreezePeriodDemand;
	#endif
	
	//当前需量 正向有功 反向有功 1~4象限无功
	DWORD CurrentDemand[eMaxDemand6Type];
	
}TDemandRam;

#pragma pack(1)
typedef struct TMinDemandUnit_t
{
	WORD ValidPulseInMin;//电能增量，脉冲个数
	BYTE ValidSecInMin;//一个分钟内超过起动功率的的累计时间，单位秒
}TMinDemandUnit;
#pragma pack()

//当前需量方向结构体
typedef struct TDemandDir_t
{
	BOOL DemandBackFlag;//是否反向(eDemandPForward = 0,//功率方向为正，枚举值需要为0,eDemandPReverse = 1,//功率方向为负) 
	BYTE ChangeCnt;//方向改变计数器，每秒+1
}TDemandDir;



TMinDemandUnit CurDemandMinData[eMaxDemand6Type];//当前分钟的需量数据，电能增量和超过起动功率的的累计时间

//正向有功、反向有功、各象限无功各自在最近60分钟内的脉冲增量,与时间
static TMinDemandUnit MinDemand[MAX_DEMANDPERIOD_MUM][eMaxDemand6Type];

static DWORD DemandRecordMinCnt;//需量存储序号--每分钟累加,过结算日，掉电等清零

static DWORD CurRateDemandRecordMinCnt;//费率需量存储序号,总需量清零，清零费率切换等清零

TDemandDir DemandDir[2];//当前需量方向，[0]有功需量，[1]无功需量

// 需量周期步数
static BYTE PeriodReal;//需量周期分钟数//加注释****lpk
// 步长(滑差)的分钟数
static WORD StepLength;

//内存中的需量
static TDemandRam DemandRam;

// 当前费率
static BYTE RecordTariff;

DWORD RealTimerSecBak[2];//实时时间备份，[0]有功需量使用的时间；[1]无功需量使用的时间
//需量计量暂停标志
static eDemandPauseFlag DemandPauseFlag;

//需量计量用的功率状态 [0]-有功功率 [1]-无功功率
static TDemandPStatus DemandPStatus[2];

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static BOOL DbReadDemandBin(WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand);
static BOOL DbReadPNRActiveDemand( WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand );
static void CountDemand(BYTE tar);
static void CalPeriod( void );
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

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
	BYTE inDot;
	inDot = Dot;
	
	if( inDot > MAX_DEMAND_DOT )
	{
		inDot = DEFAULT_DEMAND_DOT;
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
		if( Dot == 6 )
		{
			TmpDemandData.DemandValue *= (long)lib_MyPow10(Dot-MAX_DEMAND_DOT);
		}
		else
		{
			TmpDemandData.DemandValue /= (long)lib_MyPow10(MAX_DEMAND_DOT-inDot);
		}
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
//函数功能: 读正反无功最大需量
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
	BYTE inDot;
	inDot = Dot;

	if(inDot > MAX_DEMAND_DOT)//如果需量小数位数大于MAX_DEMAND_DOT,需量按照DEFAULT_DEMAND_DOT位小数返回
	{
		inDot = DEFAULT_DEMAND_DOT;
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
			if( Dot == 6 )
			{
				TmpDemandData2.DemandValue *= (long)lib_MyPow10(Dot-MAX_DEMAND_DOT);
			}
			else
			{
				TmpDemandData2.DemandValue /= (long)lib_MyPow10(MAX_DEMAND_DOT-inDot);
			}
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
//函数功能: 需量计算的分钟任务，每分钟执行1次
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void DemandMinTask( void )
{
	TRealTimer t;

	memset( &t ,0, sizeof(t) );

	if( DemandPauseFlag == eDemandPause )
	{
		//上电、清零、需量周期变更、滑差时间变更时，DemandPauseFlag置位，并在下一个整分钟清零
		DemandPauseFlag = eDemandRun;
		
		//开始计算需量前，清零电能增量和时间增量，并更新DemandCnt.RealTimerSecBak
		memset((BYTE*)&CurDemandMinData, 0x00, sizeof(CurDemandMinData));
		memset((BYTE*)&MinDemand, 0x00, sizeof(MinDemand));
		DemandRecordMinCnt = 0;
		CurRateDemandRecordMinCnt = 0;
		
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
		
		RealTimerSecBak[0] = api_CalcInTimeRelativeSec(&t)-1;//解决上电第一分钟脉冲少一秒的问题
		RealTimerSecBak[1] = RealTimerSecBak[0];
		
		//直接返回
		//例如0分钟30秒需量清零，1分钟整时，如果不在此return，则会计算0~1分的需量，按照规范，应在第2分钟才进行计算
		return;
	}
	else
	{
		//数据同步
		memcpy((BYTE*)&MinDemand[DemandRecordMinCnt%MAX_DEMANDPERIOD_MUM], (BYTE*)&CurDemandMinData, sizeof(MinDemand[0]));//需测试
		memset((BYTE*)&CurDemandMinData, 0x00, sizeof(CurDemandMinData));
		if(DemandRecordMinCnt != 0xffffffff)
		{
			DemandRecordMinCnt++;
		}
		else
		{
			DemandRecordMinCnt = MAX_DEMANDPERIOD_MUM+(DemandRecordMinCnt%MAX_DEMANDPERIOD_MUM)+1;
		}
		if(CurRateDemandRecordMinCnt != 0xffffffff)
		{
			CurRateDemandRecordMinCnt++;
			}
		else
			{
			CurRateDemandRecordMinCnt = MAX_DEMANDPERIOD_MUM+(CurRateDemandRecordMinCnt%MAX_DEMANDPERIOD_MUM)+1;
		}
	}
	//定期同步需量周期步数及滑差时间
	CalPeriod();

	#if(SEL_FREEZE_PERIOD_DEMAND == YES)
	DealFreezePeriodDemand();
	#endif
	//记录总需量
	if( ( DemandRecordMinCnt >= PeriodReal )
	&& ( (DemandRecordMinCnt % StepLength) == 0))
	{
		CountDemand(0);//结算，i对应总或当前费率需量

		api_MultiFunPortCtrl( eDEMAND_PERIOD_OUTPUT );

	}
	//当前费率需量
	if( ( CurRateDemandRecordMinCnt >= PeriodReal )
	&& ( DemandRecordMinCnt >= PeriodReal )
	&& ( CurRateDemandRecordMinCnt % StepLength == 0) )
	{
		CountDemand(1);//结算，i对应总或当前费率需量

	}
}

//-----------------------------------------------
//函数功能: 需量计算的主函数
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_DemandTask( void )
{
	if( api_GetTaskFlag( eTASK_DEMAND_ID, eFLAG_MINUTE ) == TRUE )
	{
		DemandMinTask();//需量分钟任务
		api_ClrTaskFlag(eTASK_DEMAND_ID,eFLAG_MINUTE);//清分钟标志
	}
	
	//费率发生变化
	if( RecordTariff != api_GetCurRatio() )//清费率的结构体
	{
		CurRateDemandRecordMinCnt = 0;//清零费率需量序号
		memset( &DemandRam.RatioDemand, 0x00, sizeof(DemandRam.RatioDemand) );
		DemandRam.RatioDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
	}

	//记录费率
	RecordTariff = api_GetCurRatio();
}


//-----------------------------------------------
//函数功能: 读总或分费率最大需量数据
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
	BYTE inDot;
	inDot = Dot;
	
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
	
	if( inDot > MAX_DEMAND_DOT)
	{
		inDot = DEFAULT_DEMAND_DOT;
	}

	pDemandData = (BYTE *)&TmpDemandDBase;
	pDemandData += GetTDemandDbaseOffset( Type );
	
	if( inDot == DEFAULT_DEMAND_DOT )//小数位数为4位进行进位
	{   
	    //若开关打开，则将最后一位进位，四舍五入
    	if( api_GetProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) == TRUE )
    	{
    	    if( (((TDemandData *)pDemandData)->DemandValue) != 0 )
    	    {
    		    ((TDemandData *)pDemandData)->DemandValue += (5*(MAX_DEMAND_DOT-DEFAULT_DEMAND_DOT));
    	    }
    	}
	}

	DemandValue = (((TDemandData *)pDemandData)->DemandValue);
	
	if(  DemandValue != 0xffffffff )
	{
		if( Dot == 6 ) //抄读小数位数为6位时，需量扩大10倍；抄读小数位数大于6位按照4位处理；
		{
			((TDemandData *)pDemandData)->DemandValue *= (long)lib_MyPow10(Dot-MAX_DEMAND_DOT);
		}
		else
		{
			((TDemandData *)pDemandData)->DemandValue /= (long)lib_MyPow10(MAX_DEMAND_DOT-inDot);
		}
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
		DbWriteFreezePeriodDemand( Type, pDemand );//@@冻结期间需量暂未评估及修改
		#endif
		
		//写入当前需量（瞬时量）
		switch( Type )
		{
		case P_ACTIVE:
			DemandRam.CurrentDemand[ePActiveDemand] = pDemand->DemandValue;
			break;
		case N_ACTIVE:
			DemandRam.CurrentDemand[eNActiveDemand] = pDemand->DemandValue;
			break;
		#if( SEL_REACTIVE_DEMAND == YES )
		case RACTIVE1:
			DemandRam.CurrentDemand[eReactive1Demand] = pDemand->DemandValue;
			break;
		case RACTIVE2:
			DemandRam.CurrentDemand[eReactive2Demand] = pDemand->DemandValue;
			break;
		case RACTIVE3:
			DemandRam.CurrentDemand[eReactive3Demand] = pDemand->DemandValue;
            break;
		case RACTIVE4:
			DemandRam.CurrentDemand[eReactive4Demand] = pDemand->DemandValue;
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
//参数:		tar[in]		费率类型 0:总的  1:各费率的
//                    
//返回值: 	无
//
//备注: 需量保存6位小数到EEPROM  
//-----------------------------------------------
static void CountDemand(BYTE tar)
{
	BYTE i = 0;
	BYTE j = 0;
	TDemandData	TmpDemandData;
	WORD Type = 0;
	DWORD dwTmpDemand = 0;
	WORD wTmpValidTime = 0;
	DWORD dwActiveConstant = 0;

	memset( &TmpDemandData, 0, sizeof(TmpDemandData) );
	//异常防护，
	if(DemandRecordMinCnt < PeriodReal)
	{
		DemandRecordMinCnt = PeriodReal;
	}
	 
	for(i=0; i<eMaxDemand6Type; i++)
	{
	
		dwTmpDemand = 0;
		wTmpValidTime = 0;
		for( j = 0; j < PeriodReal; j++ )
		{
		
			dwTmpDemand += (DWORD)(MinDemand[(DemandRecordMinCnt-j-1)%MAX_DEMANDPERIOD_MUM][i].ValidPulseInMin);//需量周期内电能脉冲数增量
			wTmpValidTime += (WORD)(MinDemand[(DemandRecordMinCnt-j-1)%MAX_DEMANDPERIOD_MUM][i].ValidSecInMin);//需量周期内超起动功率时间，单位秒
		}

		//累计时间不足一个滑差时间时，按照一个滑差时间赋值
		if( (wTmpValidTime<(WORD)(StepLength*60)) && (wTmpValidTime>0) )
		{
			wTmpValidTime = (WORD)(StepLength*60);
		}

		dwActiveConstant = api_GetActiveConstant();//脉冲常数

		//需量=(电能脉冲数/脉冲常数)/需量周期，单位Kw，换算-MAX_DEMAND_DOT
		//lib_MyPow10(MAX_DEMAND_DOT)，换算(-MAX_DEMAND_DOT)
		//3600,Kwh换算为Kws
		if( (wTmpValidTime!=0) && (dwActiveConstant!=0) )
		{
			TmpDemandData.DemandValue = (DWORD)( ( ((QWORD)dwTmpDemand * (lib_MyPow10(MAX_DEMAND_DOT)) * 3600) / dwActiveConstant ) / wTmpValidTime );
		}
		else
		{
			TmpDemandData.DemandValue = 0;
		}

		api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );

		//保存需量
		Type = (i<2) ? (i+1) : (i+3);
		DbWriteDemand(Type, RecordTariff*tar, &TmpDemandData );
	
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
	if( Flag == FALSE )
	{
		//RAM中的不对采用保护区默认参数
		PeriodReal = EnereyDemandModeConst.DemandPeriod;
		StepLength = EnereyDemandModeConst.DemandSlip;
	}
	else
	{
		//为了保持周期与滑差时间不能整除的情况，
		//例如周期15分钟滑差7分钟；需量按照14分；21分出需量
		PeriodReal = (Period/Step)*Step;
		StepLength = Step;
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
	BYTE i = 0;
	TRealTimer t;

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );

	DemandRecordMinCnt = 0;
	CurRateDemandRecordMinCnt = 0;
	//有功需量、无功需量的方向默认为正
	for( i = 0; i < (sizeof(DemandDir)/sizeof(TDemandDir)); i++ )
	{
		DemandDir[i].DemandBackFlag = eDemandPForward;//功率方向为正
		DemandDir[i].ChangeCnt = 0;
	}
	RealTimerSecBak[0] = api_CalcInTimeRelativeSec(&t);
	RealTimerSecBak[1] = RealTimerSecBak[0];

	memset( MinDemand, 0, sizeof(MinDemand) );
	//清零当前
	memset((BYTE*)&CurDemandMinData, 0x00, sizeof(CurDemandMinData));
}
//-----------------------------------------------
//函数功能: 需量模块初始化
//
//参数:		InDemandPauseFlag[in]：暂停需量计量标志
//                    
//返回值:     无
//
//备注:   
//-----------------------------------------------
void api_InitDemand( )
{
	// 确定需量周期、滑差
	CalPeriod();
	
	ClearDemandRamData();

	//清内存中当前最大需量和当前需量
	api_ClrDemand( eClearRamDemand, eDemandPause);

	// 记录费率
	RecordTariff = api_GetCurRatio();
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
	//上电默认需量累计用的功率为未起动、方向正
	DemandPStatus[0].IsStart = FALSE;
	DemandPStatus[0].PDir = eDemandPForward;
	DemandPStatus[1].IsStart = FALSE;
	DemandPStatus[1].PDir = eDemandPForward;
	
	// 确定需量周期、滑差
	CalPeriod();
	
	ClearDemandRamData();

	//清内存中当前最大需量和当前需量
	api_ClrDemand( eClearRamDemand, eDemandPause );

	// 记录费率
	RecordTariff = api_GetCurRatio();
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
	BYTE DataSign = 0x00;
	BYTE DemandIndex = 0;//要读取的当前需量在数据中的位置
	BYTE TmpQuadrant = 0;
	BYTE BackFlag = 0;//反向标志
	BYTE inDot;//反向标志
	inDot = Dot;
	tl.l = 0;

	if( inDot > MAX_DEMAND_DOT )
	{
		inDot = DEFAULT_DEMAND_DOT;
	}
	
	if( (Len != 3)&&(Len != 4) ) 
	{
		return;//长度非法时，不写pDemand
	}

	if( (Type<1) || (Type>2) ) 
	{
		memset( (BYTE*)pDemand, 0, Len );
        return;
	}

	DataSign = 0;
	//判断有功需量是否反向
	if( DemandDir[0].DemandBackFlag == eDemandPReverse )
	{
		DataSign |= 0x08;
		BackFlag = 1;
		DemandIndex = eNActiveDemand;
	}
	else
	{
		BackFlag = 0;
		DemandIndex = ePActiveDemand;
	}

	if( Type == 2 )
	{
		//判断无功需量是否反向
		if( DemandDir[1].DemandBackFlag == eDemandPReverse )
		{
			DataSign |= 0x80;
			BackFlag = 1;
		}
		else
		{
			BackFlag = 0;
		}

		switch( DataSign )
		{
		case 0x00://一象限
			DemandIndex = eReactive1Demand;
			break;
		case 0x08://二象限
			DemandIndex = eReactive2Demand;
			break;
		case 0x88://三象限
			DemandIndex = eReactive3Demand;
			break;
		case 0x80://四象限
			DemandIndex = eReactive4Demand;
			break;
		default:
			DemandIndex = eReactive1Demand;
			break;
		}
	}
	
	//DemandRam.CurrentDemand中数据为无符号数
	tl.l = (long)DemandRam.CurrentDemand[DemandIndex];
	if(Dot == 6) //抄读小数位数为6位时，需量扩大10倍；抄读小数位数大于6位按照4位处理；
	{
		tl.l *= (long)lib_MyPow10(Dot-MAX_DEMAND_DOT);
	}
	else
	{
		tl.l /= (long)lib_MyPow10(MAX_DEMAND_DOT-inDot);
	}

	if( tl.l == 0 )//如果数值为0按照0返回，不返回-0
	{
		BackFlag = 0;
	}
	
	if( BackFlag )
	{
		tl.l *= (-1);
	}

	if( DataType == DATA_BCD )
	{
		if( BackFlag )
		{
			tl.l *= -1;
		}
		tl.d = lib_DWBinToBCD( tl.d );
		
		//反向最高位置符号
		if( BackFlag )
		{
			tl.b[Len - 1] |= 0x80;
		}
	}
	else if( DataType == DATA_HEX )
	{
		if( BackFlag )
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
//函数功能: 读RAM中缓存的6种当前总需量（包括正向有功、反向有功、1~4象限，不支持费率需量）
//
//参数: 
//	Type[in]:	需量类型，包括正向有功、反向有功、1~4象限无功
//				
//	DataType[in]:	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)      
//	Dot[in]:	小数点位数
//	Len[in]:    读取需量长度
//	pDemand[out]:输出数据
//
//返回值:	无

//备注: DATA_BCD格式最高位位符号位
//-----------------------------------------------
void api_Get6TypeCurrDemandData(eDemand6Type Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE * pDemand)
{
	DWORD dwDemand = 0;

	if( (Dot > MAX_DEMAND_DOT)||(Dot < 3) )
	{
		Dot = 4;
	}
	
	if( (Len != 3)&&(Len != 4) )
	{
		return;//长度非法时，不写pDemand
	}

	if( Type >= eMaxDemand6Type ) 
	{
		memset( (BYTE*)pDemand, 0, Len );
        return;
	}
	
	//DemandRam.CurrentDemand中数据为无符号数
	dwDemand = (DWORD)DemandRam.CurrentDemand[Type];
	dwDemand /= (DWORD)lib_MyPow10(MAX_DEMAND_DOT-Dot);

	if( DataType == DATA_BCD )
	{
		dwDemand = lib_DWBinToBCD( dwDemand );
	}
	
	memcpy( (BYTE*)pDemand, &dwDemand, Len );
}

//-----------------------------------------------
//函数功能: 需量清零
//
//参数: 	eType[in]：		要清需量的类型
//      eClearRamDemand	清当前需量、清内存中当前最大需量
//	    eClearAllDemand	清eeprom中当前最大需量		
//
//			InDemandPauseFlag[in]: 暂停需量计量标志
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_ClrDemand( eClearDemandType eType, eDemandPauseFlag InDemandPauseFlag )
{
	BYTE i = 0;
	TDemandDBase TmpDemandDBase;
	memset( &TmpDemandDBase, 0, sizeof(TmpDemandDBase) );
	
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
		// 记录费率
		RecordTariff = api_GetCurRatio();
	}

	DemandPauseFlag = InDemandPauseFlag;
}

//-----------------------------------------------
//函数功能: 更新需量计算使用的相对秒数备份
//
//参数:		t[in]：	当前日期时间
//	
//返回值: 	无
//
//备注:把当前时间备份到当前
//-----------------------------------------------
void api_UpdateDemandRealTimerSecBak( TRealTimer t )
{
	RealTimerSecBak[0] = api_CalcInTimeRelativeSec(&t);
	RealTimerSecBak[1] = RealTimerSecBak[0];
}

//-----------------------------------------------
//函数功能: 更新6个需量通道对应的电能脉冲增量及时间增量
//
//参数:		Type[in]：	有功/无功类型 0：有功；1：无功
//			PulseNum[in]:		脉冲增量
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_UpdateDemandEnergyPulse( BYTE Type, BYTE PulseNum )
{
	BYTE DataSign = 0x00;
	TRealTimer t;
	DWORD CurRealTimerSec = 0;//当前时间对应的相对秒数
	WORD SecIncr[2] = {0};//每次更新时的时间增量

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );

	if( Type > 1 )
	{
		return;
	}

	CurRealTimerSec = api_CalcInTimeRelativeSec(&t);
	SecIncr[Type] = (WORD)(CurRealTimerSec - RealTimerSecBak[Type]);//每次电能脉冲刷新时，计算两次刷新之间的时间间隔
	if( SecIncr[Type] > 60 )
	{
		//限值保护，程序不发生阻塞时，api_UpdateDemandEnergyPulse有功约每秒调用1次，无功约每秒调用1次，SecIncr[Type]为1秒；发生阻塞时（如插卡），一般为个位数
		SecIncr[Type] = 60;
	}
    
	RealTimerSecBak[Type] = CurRealTimerSec;

	//有功为正，在一四象限，有功为反，在二三象限
	if( DemandPStatus[0].PDir == eDemandPReverse )//反向
	{
		DataSign |= 0x08;

		if( DemandPStatus[0].IsStart == TRUE )//有功功率超起动功率
		{
			//累计反向有功电能和时间
			if( Type == 0 )
			{
				if((CurDemandMinData[eNActiveDemand].ValidSecInMin + SecIncr[Type])<=255)
				{
					CurDemandMinData[eNActiveDemand].ValidPulseInMin += PulseNum;
					CurDemandMinData[eNActiveDemand].ValidSecInMin += SecIncr[Type];
				}
			}	
		}
	}
	else//正向
	{
		if( DemandPStatus[0].IsStart == TRUE )//有功功率超起动功率
		{
			//累计正向有功电能和时间
			if( Type == 0 )
			{
				if((CurDemandMinData[ePActiveDemand].ValidSecInMin + SecIncr[Type])<=255)
				{
					CurDemandMinData[ePActiveDemand].ValidPulseInMin += PulseNum;
					CurDemandMinData[ePActiveDemand].ValidSecInMin += SecIncr[Type];
				}
			}
		}
	}

	if( (DemandPStatus[1].IsStart==TRUE) && (Type==1) )//无功功率超起动功率 且需要本次函数调用处理的功率类型是无功
	{
		//无功为正，在一二象限，无功为反，在三四象限
		if( DemandPStatus[1].PDir == eDemandPReverse ) 
		{
			DataSign |= 0x80;
		}

		//累计无功功电能和时间
		switch( DataSign )
		{
		case 0x00://一象限
			if((CurDemandMinData[eReactive1Demand].ValidSecInMin + SecIncr[Type])<=255)
			{
				CurDemandMinData[eReactive1Demand].ValidPulseInMin += PulseNum;
				CurDemandMinData[eReactive1Demand].ValidSecInMin += SecIncr[Type];
			}
			break;
		case 0x08://二象限
			if((CurDemandMinData[eReactive2Demand].ValidSecInMin + SecIncr[Type])<=255)
			{
				CurDemandMinData[eReactive2Demand].ValidPulseInMin += PulseNum;
				CurDemandMinData[eReactive2Demand].ValidSecInMin += SecIncr[Type];
			}
			break;
		case 0x88://三象限
			if((CurDemandMinData[eReactive3Demand].ValidSecInMin + SecIncr[Type])<=255)
			{
				CurDemandMinData[eReactive3Demand].ValidPulseInMin += PulseNum;
				CurDemandMinData[eReactive3Demand].ValidSecInMin += SecIncr[Type];
			}
			break;
		case 0x80://四象限
			if((CurDemandMinData[eReactive4Demand].ValidSecInMin + SecIncr[Type])<=255)
			{
				CurDemandMinData[eReactive4Demand].ValidPulseInMin += PulseNum;
				CurDemandMinData[eReactive4Demand].ValidSecInMin += SecIncr[Type];
			}
			break;
		default:
			break;
		}
	}
	
    //判断有功需量、无功需量的方向变化,需量方向发生变化时，进行计时
	if( DemandDir[Type].DemandBackFlag != DemandPStatus[Type].PDir)
	{
		//超起动门限方向改变持续60秒以上时，需量方向切换
		if( DemandDir[Type].ChangeCnt < 60 )
		{
			DemandDir[Type].ChangeCnt += SecIncr[Type];
		}
		else
		{
			DemandDir[Type].DemandBackFlag = DemandPStatus[Type].PDir;
			DemandDir[Type].ChangeCnt = 0;
		}
	}
	else
	{
		DemandDir[Type].ChangeCnt = 0;
	}
}

//-----------------------------------------------
//函数功能: 更新当前需量状态，包括方向和是否起动
//
//参数:		Type[in]：			有功/无功类型 0：有功；1：无功
//			StartFlg[in]：		是否起动：0:潜动 ； 1:起动
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_UptCurDemandStatus( BYTE Type, BYTE StartFlg )
{
	DWORD OnePhasePLimit;
	
	if( Type > 1 )
	{
		return;
	}

	if( Type == 0 )
	{
		//设置合相反向标志
		if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ))
		{
			DemandPStatus[Type].PDir = eDemandPReverse;
		}
		else
		{
			DemandPStatus[Type].PDir = eDemandPForward;
		}
	}
	else
	{
		//设置合相反向标志
		if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q ))
		{
			DemandPStatus[Type].PDir = eDemandPReverse;
		}
		else
		{
			DemandPStatus[Type].PDir = eDemandPForward;
		}
	}
	if( StartFlg != 0 )
	{
		DemandPStatus[Type].IsStart = TRUE;
	}
	else
	{
		DemandPStatus[Type].IsStart = FALSE;
	}
}
#endif//#if( MAX_PHASE == 3 )
#endif
