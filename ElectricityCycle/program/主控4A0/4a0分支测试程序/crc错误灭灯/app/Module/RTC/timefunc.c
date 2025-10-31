//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.9
//描述		时间的通用函数，不包括具体的时钟芯片
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//实时时钟，BCD码方式的时钟
TRealTimer RealTimer;
BYTE g_PowerOnSecTimer;//上电30秒倒计时 所有上电延时的操作 都用改计数器 只有上电大于30秒，掉电时才数据转存
//系统全局变量 电表中当前时间与2000年1月1日0时0分之间的时间差，单位：分钟 (用函数api_CalcInTimeRelativeMin()计算值)，
DWORD g_RelativeMin;

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//                            1   2   3   4   5   6   7   8   9   10  11  12
const BYTE NormalYearDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//                      	1   2   3   4   5   6   7   8   9   10  11  12
const BYTE LeapYearDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
BOOL api_CheckMonDay( WORD wYear, BYTE byMon, BYTE byDay );
extern void InitRtc( void );
extern BOOL ReadOutClockHara( WORD Type );
extern BOOL WriteOutClock(TRealTimer * t);

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//  刷新系统全局变量 电表中当前时间与2000年1月1日0时0分之间的时间差，单位：分钟
void RefreshGlobalVariableMinNow(void)
{
	DISABLE_CLOCK_INT;
	g_RelativeMin = api_CalcInTimeRelativeMin( &RealTimer );
	ENABLE_CLOCK_INT;
}
BOOL IsLeapYear(BYTE Year)
{
	if( Year & 0x0003 )
	{
		return FALSE;
	}

	return TRUE;
}
void api_SoftClockSecTsk(void)
{
	BYTE Buf[sizeof(NormalYearDay)];

	// 设置秒标志 
	//SecFlag = TRUE;

	// 秒加1 
	RealTimer.Sec++;
	
	// 判断秒溢出 
	if( RealTimer.Sec >= 60 )
	{
		// 清除秒计数 
		RealTimer.Sec = 0;
		
		// 分加一 
		RealTimer.Min++;
		
		// 判断分溢出 
		if( RealTimer.Min >= 60 )
		{
			// 清除分计数 
			RealTimer.Min = 0;
			
			// 小时加1 
			RealTimer.Hour++;
			
			// 判断小时溢出 
			if( RealTimer.Hour >= 24 )
			{
				// 清除小时 
				RealTimer.Hour = 0;
				
				// 天加1 
				RealTimer.Day++;
				
				// 处理星期 
				//RealTimer.Week++;
				
				// 星期是否到 
				//if( RealTimer.Week >= 0x7 )
				//{
				//	// 清除星期 
				//	RealTimer.Week = 0;
				//}
				
				// 判断是否为闰年 
				if( IsLeapYear(RealTimer.wYear - 2000) == TRUE )
				{
					//p = (BYTE *)LeapYearDayHex;
					memcpy( Buf, LeapYearDay, sizeof(Buf) );
				}
				else
				{
					//p = (BYTE *)NormalDayHex;
					memcpy( Buf, NormalYearDay, sizeof(Buf) );
				}
				
				// 判断月份是否到 
				if( RealTimer.Day > Buf[RealTimer.Mon - 1] )
				{
					// 初始化天 
					RealTimer.Day = 1;
						
					// 月加1 
					RealTimer.Mon++;
						
					// 月是否溢出 
					if( RealTimer.Mon > 12 )
					{
						// 月初始化 
						RealTimer.Mon = 1;
						
						// 年加1 
						RealTimer.wYear++;
						if(RealTimer.wYear > 2099)
						{
							RealTimer.wYear = 0;
							WriteOutClock(&RealTimer);
						}
					}
				}
			}
		}
	}
}

//粗略定时处理，例如1分钟定时，在大循环中调用
void api_RtcTask(void)
{
	TRealTimer TmpRealTimer;

	//500毫秒读一次时间
	if( api_GetTaskFlag( eTASK_RTC_ID, eFLAG_500_MS ) == TRUE )
	{
		api_ClrTaskFlag( eTASK_RTC_ID, eFLAG_500_MS );

		memcpy( (BYTE*)&TmpRealTimer, (BYTE*)&RealTimer, sizeof(TRealTimer) );
		api_ReadMeterTime( 0 );
		if( TmpRealTimer.Sec != RealTimer.Sec )
		{
			//秒变化
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_SECOND );
		}
		if( TmpRealTimer.Min != RealTimer.Min )
		{
			//分钟变化
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_MINUTE );
		}
		if( TmpRealTimer.Hour != RealTimer.Hour )
		{
			//小时变化
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_HOUR );
		}
	}
	//1秒到
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_SECOND) == TRUE )
	{
		api_SetAllTaskFlag(eFLAG_SECOND); // 这样能确保在前面的任务一定先执行
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_SECOND);

		if( g_PowerOnSecTimer != 0x00 )
		{
			g_PowerOnSecTimer--;
		}

		//没有5伏，不向下处理
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			return;
		}
		
		FunctionConst(MAIN_TIMER_SEC_TASK_END);
	}

	//1分钟到
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_MINUTE) == TRUE )
	{
		api_SetAllTaskFlag( eFLAG_MINUTE );
		//清标志
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_MINUTE);

		//刷新系统全局变量 电表中当前时间与2000年1月1日0时0分之间的时间差，单位：分钟
		RefreshGlobalVariableMinNow();

		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			return;
		}
	}
	//hour change
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_HOUR) == TRUE )
	{
		api_SetAllTaskFlag( eFLAG_HOUR );//这样能确保在前面的任务一定先执行
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_HOUR);
	}
}


//-----------------------------------------------
//函数功能: RTC任务上电初始化
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_PowerUpRtc(void)
{
	BOOL		Result;
	DWORD		PowerDownRelativeTime;
	TRealTimer	PowerDownTime;

	g_PowerOnSecTimer = POWER_UP_TIMER;
	
	InitRtc();  //该函数内有复位RTC操作 不可随意调整位置

	Result = api_ReadMeterTime( 0 );
	RefreshGlobalVariableMinNow();
	
	api_GetPowerDownTime(&PowerDownTime);
	PowerDownRelativeTime = api_CalcInTimeRelativeMin(&PowerDownTime);
	if( (PowerDownRelativeTime == ILLEGAL_VALUE_8F) && (Result == TRUE) )//RTC时间未乱且掉电时间不对的情况下不进行时间处理
	{	
		return;
	}

	//时钟错误	上电时间小于掉电时间	上电时间大于（掉电时间+1000天）
	if( (Result != TRUE) || (g_RelativeMin < PowerDownRelativeTime) || (g_RelativeMin > (PowerDownRelativeTime+1440000)) )
	{
		
		Result = api_CheckClock(&PowerDownTime);
		if( Result != TRUE )
		{
			PowerDownTime.wYear = 2020;
			PowerDownTime.Mon = 1;
			PowerDownTime.Day = 1;
			PowerDownTime.Hour = 1;
			PowerDownTime.Min = 1;
			PowerDownTime.Sec = 1;			
		}
		
		g_RelativeMin = 0xFFFFFFFF;//为了强制更新冻结标志，查找周期性冻结序列号是否需要回退，影响RepairFreezeUseFlash函数，修改请注意；
		api_ReadPara1FromEE();//更新E2参数到Ram
		api_PowerUpEnergy();
		api_WriteMeterTime(&PowerDownTime);
		//置RTC上电时间错误
		api_SetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG);//先写时间在置标志，不可颠倒，防止刚置标志被清；
	}
}


//-----------------------------------------------
//函数功能: 读取时间，调用该函数会操作外部时钟，同时刷新表示时间变量，不要频繁操作
//
//参数: 
//			Type[in]			0：读出rtc模块时钟，刷新RealTimer变量 1：只读秒
//                    
//返回值:  	TRUE:正确读取       FALSE:没有正确读取
//备注:     
//-----------------------------------------------
BOOL api_ReadMeterTime( BYTE Type )
{
	WORD i;
	
	for(i=0; i<3; i++)
	{
		if( ReadOutClockHara( Type ) == TRUE )
		{
			break;
		}
	}
	
	if( i == 3 )
	{
		api_SetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG);
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读取时间，调用该函数会操作外部时钟，不会记录固化标志
//
//参数: 
//			Type[in]			0：读出rtc模块时钟，刷新RealTimer变量 1：只读秒
//                    
//返回值:  	TRUE:正确读取       FALSE:没有正确读取
//备注:     
//-----------------------------------------------
BOOL api_ReadMeterTimeLowPower(  )
{
	WORD i;
	
	for(i=0; i<3; i++)
	{
		if( ReadOutClockHara( 0 ) == TRUE )
		{
			break;
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 把TRealTimer结构的时间写到RTC模块中，星期由日期计算
//
//参数: 
//			t[in]				要写入的时间
//                    
//返回值:  	TRUE:正确读取       FALSE:没有正确读取
//备注:     
//-----------------------------------------------
BOOL api_WriteMeterTime(TRealTimer * t)
{
	DWORD OldRelativeMin;
	
	//外部时钟对钟
	if( WriteOutClock(t) == FALSE )
	{
		return FALSE;
	}
	//校时成功清，RTC上电时间错误标志
	api_ClrRunHardFlag(eRUN_HARD_ERR_RTC_FLAG);
	api_ClrRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG);
	//如果日期改变，清广播校时标志
	if( api_GetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG ) == TRUE )
	{
		//如果已经进行过广播校时
		if( (t->Day != RealTimer.Day) || (t->Mon != RealTimer.Mon) || (t->wYear != RealTimer.wYear) )
		{
			api_ClrRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
		}
	}
	//如果日期改变，清明文广播校时故障标志
	if( api_GetRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG ) == TRUE )
	{
		//如果已经进行过广播校时
		if( (t->Day != RealTimer.Day) || (t->Mon != RealTimer.Mon) || (t->wYear != RealTimer.wYear) )
		{
			api_ClrRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );
		}
	}
	
	//禁止时钟
	DISABLE_CLOCK_INT;
	//写入
	memcpy((BYTE *)&RealTimer, (BYTE *)t, sizeof(TRealTimer));
	//打开时钟
	ENABLE_CLOCK_INT;
	
	OldRelativeMin = g_RelativeMin;

	//刷新系统全局变量 电表中当前时间与2000年1月1日0时0分之间的时间差，单位：分钟
	RefreshGlobalVariableMinNow();

	//计算费率
	api_CheckTimeTable(TRUE);

	api_UpdateFreezeTime(OldRelativeMin);
	
	#if( SEL_DEMAND_2022 == YES )
	#if( MAX_PHASE != 1 )
	api_UpdateDemandRealTimerSecBak( RealTimer );
	#endif
	#endif
	
	return TRUE;
}


//-----------------------------------------------
//函数功能: 根据入口参数读取时间
//
//参数: 
//			Type[in]				需要获取的时间根式
//									bit15: 0：年月日星期时分秒 年在buf[0]且年是小端模式   1：秒分时星期日月年 年是小端模式
//									bit14: 0: Hex码  	1:BCD码
//									bit13-bit8:保留
//									bit7:年是否加2000  1：加2000
//									bit6-bit0:年月日星期时分秒  1:表示需要该位对应的数据
//			pBuf[out]				存放读出的数据
//                    
//返回值:  	返回读取的数据长度
//备注:     时间都是Hex码，年都是word类型
//#define DATETIME_SECOND_FIRST			BIT15//秒在buf[0]
//#define DATETIME_BCD					BIT14//读出数据是bcd
//#define DATETIME_20YY					(BIT7+BIT6)
//#define DATETIME_YY					BIT6
//#define DATETIME_MM					BIT5
//#define DATETIME_DD					BIT4
//#define DATETIME_WW					BIT3
//#define DATETIME_hh					BIT2
//#define DATETIME_mm					BIT1
//#define DATETIME_ss					BIT0
//
//#define DATETIME_YYMMDD				(BIT6+BIT5+BIT4)
//#define DATETIME_20YYMMDD				(BIT7+BIT6+BIT5+BIT4)
//#define DATETIME_YYMMDDWW				(BIT6+BIT5+BIT4+BIT3)
//#define DATETIME_20YYMMDDWW			(BIT7+BIT6+BIT5+BIT4+BIT3)
//#define DATETIME_hhmmss				(BIT2+BIT1+BIT0)
//
//#define DATETIME_MMDDhhmm				(BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_YYMMDDhhmm			(BIT6+BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_20YYMMDDhhmm			(BIT7+BIT6+BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_MMDDhhmmss			(BIT5+BIT4+BIT2+BIT1+BIT0)
//#define DATETIME_YYMMDDhhmmss			(BIT6+BIT5+BIT4+BIT2+BIT1+BIT0)
//#define DATETIME_20YYMMDDhhmmss		(BIT7+BIT6+BIT5+BIT4+BIT2+BIT1+BIT0)
//-----------------------------------------------
WORD api_GetRtcDateTime( WORD Type, BYTE *pBuf )
{
	BYTE i,Length;
	WORD wTmp;
	
	Length = 0;
	for( i=0; i<7; i++)
	{
		if( Type&(0x01<<i) )
		{
			switch( i )
			{
				case 0:
					pBuf[Length++] = RealTimer.Sec;
					break;
				case 1:
					pBuf[Length++] = RealTimer.Min;
					break;
				case 2:
					pBuf[Length++] = RealTimer.Hour;
					break;
				case 3:
					pBuf[Length++] = ((api_CalcInTimeRelativeSec( &RealTimer )/60/1440)+6)%7;
					break;
				case 4:
					pBuf[Length++] = RealTimer.Day;
					break;
				case 5:
					pBuf[Length++] = RealTimer.Mon;
					break;
				case 6:
					pBuf[Length++] = RealTimer.wYear%100;
					break;
			}
		}
	}
	
	if( Type&0x4000 )//要求BCD码格式
	{
		for(i=0; i<Length; i++)
		{
			pBuf[i] = lib_BBinToBCD( pBuf[i] );
		}
	}
	
	if( (Type & 0xc0) == 0xc0 )//处理年是否加2000，这种情况要求年必须也选了
	{
		if( (Type&0x4000) == 0x0000 )//Hex码
		{
			Length -= 1;//到这长度不会为0的
			wTmp = RealTimer.wYear;
			memcpy( &pBuf[Length], &wTmp, sizeof(WORD) );
			Length += 2;
		}
		else//要求BCD码格式 WORD类型同意按小端模式处理
		{
			pBuf[Length] = 0x20;
			Length ++;
		}
	}
	
	if( (Type&0x8000) == 0x0000 )//如果要求 年月日星期时分秒 年在buf[0] 顺序
	{
		lib_InverseData( pBuf, Length );
		if( (Type&0xc0) == 0xc0 )//处理年是否加2000，这种情况要求年必须也选了
		{
			lib_InverseData( pBuf, 2 );
		}
	}
	
	return Length;
}


//-----------------------------------------------
//函数功能: 相对时间转化为绝对时间，相对时间指以分钟为单位，以2000年1月1日0点0分为起点，100年内有效
//
//参数: 
//			Min[in]				相对时间的分钟数
//			p[out]				返回绝对时间（年月日时分秒格式）
//                    
//返回值:  	TRUE:正确转换       FALSE:没有正确转换
//备注:     时间都是Hex码
//-----------------------------------------------
BOOL api_ToAbsTime(DWORD Min, TRealTimer *p)
{
	WORD i;
	DWORD Temp, Temp1, Temp2;
	BYTE Buf[sizeof(NormalYearDay)];

	//正确性判断
	if( p == NULL )
	{
		return FALSE;
	}

	Temp = Min;

	memset(p, 0, sizeof(TRealTimer));

	//秒直接置0
	p->Sec = 0;
	
	//得到分钟
	p->Min = (BYTE)(Temp % 60);

	//剩余分钟数
	Temp -= p->Min;

	//转化为小时
	Temp /= 60;

	//得到小时
	p->Hour = (BYTE)(Temp % 24);

	//剩余小时数
	Temp -= p->Hour;

	//转化为天
	Temp /= 24;

	//计算年
	Temp1 = 0;

	for(i=0; i<100; i++)
	{
		Temp2 = Temp1;

		if( i & 0x0003 )
		{
			Temp1 += 365;
		}
		else
		{
			Temp1 += 366;
		}

		if( Temp1 > Temp )
		{
			break;
		}
	}

	p->wYear = (BYTE)i;

	//剩余天数
	Temp = Temp - Temp2;

	//计算月
	Temp1 = 0;

	// 判断是否为闰年
	if( (p->wYear & 0x0003) == 0 )// p->Year 不是BCD，不能用 if( IsLeapYear(p->Year) == TRUE )
	{
		memcpy( Buf, LeapYearDay, sizeof(Buf) );
	}
	else
	{
		memcpy( Buf, NormalYearDay, sizeof(Buf) );
	}

	for(i=0; i<12; i++)
	{
		Temp2 = Temp1;

		Temp1 += Buf[i];

		if( Temp1 > Temp )
		{
			break;
		}
	}

	p->Mon = i + 1;

	//剩余天
	Temp = Temp - Temp2;

	//计算天
	p->Day = (BYTE)(Temp + 1);
	
	p->wYear += 2000;//年加上2000

	return TRUE;
}

//-----------------------------------------------
//函数功能: 将698时间格式转换为645格式
//
//参数: 
//			pTime698[in]				698时间格式 HEX 年为两字节，年在前
//			pTime645[out]				645时间格式 BCD 年为一字节，年在前
//                    
//返回值:  	无
//备注:     
//-----------------------------------------------
void api_TimeFormat698To645(TRealTimer *pTime698, BYTE *pTime645)
{
	if (pTime698->wYear >= 2000 ) 
	{
		pTime645[0] = lib_BBinToBCD(pTime698->wYear - 2000);
	}
	else 
	{
		pTime645[0] = 0;
	}
	
	pTime645[1] = lib_BBinToBCD(pTime698->Mon);
	pTime645[2] = lib_BBinToBCD(pTime698->Day);
	pTime645[3] = lib_BBinToBCD(pTime698->Hour);
	pTime645[4] = lib_BBinToBCD(pTime698->Min);
	pTime645[5] = lib_BBinToBCD(pTime698->Sec);
}
//-----------------------------------------------
//函数功能: 将645时间格式转换为698格式
//
//参数: 
//			pTime645[in]				645时间格式 BCD 年为一字节，年在前
//			pTime698[out]				698时间格式 HEX 年为两字节，年在前
//                    
//返回值:  	无
//备注:     
//-----------------------------------------------
void api_TimeFormat645To698(BYTE *pTime645, TRealTimer *pTime698)
{
	pTime698->wYear = lib_BBCDToBin(pTime645[0]);
	pTime698->wYear += 2000;
	
	pTime698->Mon = lib_BBCDToBin(pTime645[1]);
	pTime698->Day = lib_BBCDToBin(pTime645[2]);
	pTime698->Hour = lib_BBCDToBin(pTime645[3]);
	pTime698->Min = lib_BBCDToBin(pTime645[4]);
	pTime698->Sec = lib_BBCDToBin(pTime645[5]);
}
//-----------------------------------------------
//函数功能: 相对时间转化为绝对时间（645格式），相对时间指以分钟为单位，以00年1月1日0点0分为起点，100年内有效
//
//参数: 
//			Min[in]				相对时间的分钟数
//			p[out]				返回绝对时间（分时日月年格式）
//                    
//返回值:  	TRUE:正确转换       FALSE:没有正确转换
//备注:     时间都是BCD码，年在后，为一个字节
//-----------------------------------------------
BOOL api_ToAbsTime645(DWORD Min, BYTE *p)
{
	TRealTimer Time;
	BYTE Buf[6];
    
    api_ToAbsTime(Min, (void*)&Time);
	api_TimeFormat698To645((void*)&Time, Buf);
	lib_ExchangeData(p, Buf, 5);

	return TRUE;
}


//-----------------------------------------------
//函数功能: 计算相对时间的分钟数，以2000年1月1日0点0分为起点
//
//参数: 
//			t[in]			绝对时间的时间格式，值必须是Hex码
//                    
//返回值:  	相对时间值
//			0xffffffff  时间非法
//备注:     时间都是Hex码
//-----------------------------------------------
DWORD api_CalcInTimeRelativeMin(TRealTimer * t)
{
	DWORD TotalTime;
	
	TotalTime = api_CalcInTimeRelativeSec( t );
	if( TotalTime == ILLEGAL_VALUE_8F )
	{
		return ILLEGAL_VALUE_8F;
	}
	
	TotalTime /= 60;
	
	return(TotalTime);
}

//--------------------------------------------------
//功能描述:  从645的年月日时分转换到 TRealTiemr
//         
//参数:    pBuf[in]:645的年月日时分,传输顺序：分 时 日 月 年
//         
//		   pRealTimer[out]:转换后得到的 TRealTimer		
//
//返回值:   得到的DWORD 
//         
//备注内容:  无
//--------------------------------------------------	
void api_ConvertYYMMDDhhmm_TRealTimer( TRealTimer *pRealTimer, BYTE *pBuf )
{
	BYTE i;
	
	for( i=0; i<5; i++ )
	{
		pBuf[i] = lib_BBCDToBin(pBuf[i]);
	}
	if( lib_IsAllAssignNum( pBuf, 0X00, 5 ) == TRUE )
	{
		memset( pRealTimer, 0x00, sizeof(TRealTimer) );
	}
	else if( lib_IsAllAssignNum( pBuf, 99, 5 ) == TRUE )//前面lib_BBCDToBin把0x99转换成了99.插卡设置切换日期为9999999999
	{
		memset( pRealTimer, 0x99, sizeof(TRealTimer) );
	}
	else
	{
		pRealTimer->wYear = 2000+pBuf[4];
		pRealTimer->Mon = pBuf[3];
		pRealTimer->Day = pBuf[2];
		pRealTimer->Hour = pBuf[1];
		pRealTimer->Min = pBuf[0];
		pRealTimer->Sec = 0;		
	}
}

//-----------------------------------------------
//函数功能: 计算两个绝对时间之间的分钟数
//
//参数: 
//			tNew[in]			绝对时间的时间格式，值必须是Hex码
//			tOld[in]			绝对时间的时间格式，值必须是Hex码
//                    
//返回值:  	两个绝对时间之间的分钟数 tNew - tOld
//			0xffffffff: 时间有不合法的情况
//			0:          tNew < tOld
//备注:     时间都是Hex码
//-----------------------------------------------
DWORD api_CalcIntervalMinute(TRealTimer * tNew, TRealTimer * tOld)
{
	DWORD MinNew,MinOld;

	//计算相对分钟数目
	MinNew = api_CalcInTimeRelativeMin(tNew);
	if( MinNew == ILLEGAL_VALUE_8F )
	{
		return ILLEGAL_VALUE_8F;
	}
	MinOld = api_CalcInTimeRelativeMin(tOld);
	if( MinOld == ILLEGAL_VALUE_8F )
	{
		return ILLEGAL_VALUE_8F;
	}

	if( MinNew > MinOld )
	{
		MinNew -= MinOld;
	}
	else
	{
		MinNew = 0;
	}

	return MinNew;
}


//-----------------------------------------------
//函数功能: 计算相对时间的秒数
//
//参数: 
//			t[in]			绝对时间的时间格式，值必须是Hex码
//                    
//返回值:  	相对时间的秒数
//			0xffffffff  时间非法
//备注:     时间都是Hex码
//-----------------------------------------------
DWORD api_CalcInTimeRelativeSec(TRealTimer * t)
{
	DWORD TotalTime;
	WORD tyear, tmon, tday, i;

	if( api_CheckClock( t ) == FALSE )
	{
		return ILLEGAL_VALUE_8F;
	}

	//计算年含的天数
	tyear = t->wYear%100;
	if(tyear != 0)
	{
		TotalTime = (DWORD)365*tyear + (tyear-1)/4 + 1;
	}
	else
	{
		TotalTime = 0;
	}
	
	//加上月含的天数
	tmon = t->Mon;
	tday = 0;
	for(i=0; i<(tmon-1); i++)
	{
		if( (tyear&0x03) == 0 )//闰年
		{
			tday += LeapYearDay[i];
		}
		else
		{
			tday += NormalYearDay[i];
		}
	}
	TotalTime += tday;

	//加上天含的天数
	tday = t->Day;
	tday--;
	TotalTime += tday;

	//以小时为单位
	TotalTime *= 24;
	TotalTime += t->Hour;
	
	//以分钟为单位
	TotalTime *= 60;
	TotalTime += t->Min;
	
	//以秒为单位
	TotalTime *= 60;
	TotalTime += t->Sec;
	
	return(TotalTime);
}


//-----------------------------------------------
//函数功能: 判断时钟有效性的函数
//
//参数: 
//			p[in]				TRealTimer结构的绝对时间，值必须是Hex码
//                    
//返回值:  	FALSE--数据非法 TRUE--数据合法
//备注:     时间都是Hex码
//-----------------------------------------------
BOOL api_CheckClock(TRealTimer * p)
{
	TRealTimer t;

	DISABLE_CLOCK_INT;
	memcpy((BYTE *)&t, (BYTE *)p, sizeof(TRealTimer));
	ENABLE_CLOCK_INT;

	//检查月、日数据合法性，参数 byYear, byMon, byDay 都和 RealTimer 中数据一样是Hex码
	if( api_CheckMonDay( t.wYear, t.Mon, t.Day ) == FALSE )
	{
		return FALSE;
	}

	if( t.Hour > MAX_VALID_HOUR )
	{
		return FALSE;
	}

	if( t.Min > MAX_VALID_MINUTE )
	{
		return FALSE;
	}

	if( t.Sec >= 60 )
	{
		return FALSE;
	}

	return TRUE;
}
//-----------------------------------------------
//函数功能: 判断时钟有效性的函数含年
//
//参数: 
//			p[in]				TRealTimer结构的绝对时间，值必须是Hex码
//                    
//返回值:  	FALSE--数据非法 TRUE--数据合法
//备注:     时间都是Hex码
//-----------------------------------------------
BOOL api_CheckClockYear(TRealTimer * p)
{
	TRealTimer t;

	DISABLE_CLOCK_INT;
	memcpy((BYTE *)&t, (BYTE *)p, sizeof(TRealTimer));
	ENABLE_CLOCK_INT;
	
	if(( t.wYear >= 2100 )||(t.wYear < 2000))
	{
		return FALSE;
	}
	//判断时钟有效性的函数
	if( api_CheckClock( &t ) == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}
//--------------------------------------------------
//功能描述:  645 时间格式的判断以及与当前时间的比较
//         
//参数:      BYTE* ProBuf[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
BOOL api_JudgeClock645( BYTE* ProBuf)
{
	TRealTimer TmpRealTimer;
	BOOL Result;
	DWORD Time,Time1;
	
	TmpRealTimer.wYear = 2000+lib_BBCDToBin(ProBuf[0]);//年
	TmpRealTimer.Mon = lib_BBCDToBin(ProBuf[1]);//月
	TmpRealTimer.Day = lib_BBCDToBin(ProBuf[2]);//日
	TmpRealTimer.Hour = lib_BBCDToBin(ProBuf[3]);//时
	TmpRealTimer.Min = lib_BBCDToBin(ProBuf[4]);//分
	TmpRealTimer.Sec = lib_BBCDToBin(ProBuf[5]);//秒

	Result = api_CheckClock((TRealTimer*)&TmpRealTimer);//判断格式是否正确
	if( Result == FALSE )
	{
		return FALSE;
	}

	Time = api_CalcInTimeRelativeSec( (TRealTimer*)&TmpRealTimer );
	Time1 = api_CalcInTimeRelativeSec( (TRealTimer*)&RealTimer );

	if( Time >= Time1 )
	{
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------
//函数功能: 检查相应年份的月、日数据合法性，输入参数和 RealTimer 中数据一样是Hex码
//
//参数: 
//			wYear[in]			年，值必须是Hex码
//			byMon[in]			月，值必须是Hex码
//			byDay[in]			日，值必须是Hex码
//                    
//返回值:  	FALSE--数据非法 TRUE--数据合法
//备注:     时间都是Hex码
//-----------------------------------------------
BOOL api_CheckMonDay( WORD wYear, BYTE byMon, BYTE byDay )
{
	wYear %= 100;
	
	if( (byMon == 0) || (byMon > MAX_VALID_MONTH) )
	{
		return FALSE;
	}

	if( byDay == 0 )
	{
		return FALSE;
	}

	if( (wYear&0x03) == 0 )//是闰年
	{
		if( byDay > LeapYearDay[byMon-1] )
		{
			return FALSE;
		}
	}
	else
	{
		if( byDay > NormalYearDay[byMon-1] )
		{
			return FALSE;
		}
	}

	return TRUE;
}

//---------------------------------------------------------------
//函数功能: BCD格式的时间合法性判断
//
//参数: 	
//			Start[in]  开始判断的地址
// 			Ck[in]     传入的时间 不定长 格式：秒、分、时、星期、日、月、年
// 			Len[in]	   比较长度 最高位为1表示没有周
//                   
//返回值:  
//
//备注:   
//---------------------------------------------------------------
BYTE api_CheckClockBCD( BYTE Start, BYTE Len, BYTE *Ck )//周在中间
{
	BYTE  i,j;
	BYTE Max[]={ 0x59,0x59,0x23,0x7,0x31,0x12,0x99 }, Min[]={ 0x00,0x00,0x00,0x00,0x01,0x01,0x00 };

	for( i = 0; i<(Len & 0x7f); i++ )
	{
		j = i + Start;
		if( (j >= 3)&&(Len&0x80) )
		{
			j++;
		}
		
		if( (Ck[i] > Max[j])||(Ck[i] < Min[j]) )
		{
			return FALSE;//合法性检查错误
		}
		if( lib_IsBCD( Ck[i] ) == FALSE )
		{
			return FALSE;	//BCD码检查错误
		}
	}
	return TRUE;//正确
}

//-----------------------------------------------
//函数功能: 只读取时间，不刷新RealTimer
//
//参数: 
//        	pBuf[in]		输出时间buf
//返回值:  	无
//
//备注:  由自由事件调用，解决电表低功耗下复位掉电自由事件无法记录时间问题
//-----------------------------------------------
void api_FreeEventReadClockHara(BYTE *pBuf)
{
	//华大RTC未使用
}

#if( PREPAY_MODE == PREPAY_LOCAL )
//---------------------------------------------------------------
//函数功能: 判断阶梯结算日合法性
//
//参数: 	Buf[in]  传入格式  [0]时[1]日[2]月			
// 			                  
//返回值:  true 存在至少一个合法结算日  false 所有结算日都无效
//
//备注:   
//---------------------------------------------------------------
BYTE api_JudgeYearClock( BYTE *Buf )
{
	BYTE i;
	BYTE Tmp[3*MAX_YEAR_BILL];

	memcpy(Tmp ,Buf ,3*MAX_YEAR_BILL);

	for( i = 0; i < 3*MAX_YEAR_BILL; i++ )
	{
		Tmp[i] = lib_BBCDToBin(Tmp[i]);
	}
	
	for( i = 0; i < MAX_YEAR_BILL; i++ )
	{
		if( ( Tmp[3*i+1] <= 28)&&(Tmp[3*i+1] > 0) //日
			&&(Tmp[3*i] <= 23) ) //时
		{
			return TRUE;
		}
	}
	
	return FALSE;
}
#endif

