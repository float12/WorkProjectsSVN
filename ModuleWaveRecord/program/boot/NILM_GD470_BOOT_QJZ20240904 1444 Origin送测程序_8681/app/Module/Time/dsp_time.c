#include "AllHead.h"


/************************************************************************
 * 全局时钟定义;全局私有日历定义;
 ************************************************************************/
BOOL SyncDspTime = FALSE;

TRealTimer RealTimer;//此处作为timer的主定义  声明为全局变量

volatile DSPTIME PublicTime;//公历

/************************************************************************
 * 局部变量定义
 ************************************************************************/
// static DWORD Last24H = 0xFFFFFFFF;
// static DWORD Last2H = 0xFFFFFFFF;
// static DWORD Last10s = 0xFFFFFFFF;
// static DWORD Last5m = 0xFFFFFFFF;
// static DWORD Last10m = 0xFFFFFFFF;

const BYTE NormalYearDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//                      	1   2   3   4   5   6   7   8   9   10  11  12
const BYTE LeapYearDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/*1秒时间到*/
void TimeFun()
{
    PublicTime.msec = 0;
    PublicTime.sec ++;
}
/*每一包报文需要带上时间*/
DWORD GetTimeSec()
{
    return PublicTime.sec;
}

/*ARM对时到DSP 才能保证整10s等
{
   SyncDspTime = TRUE;
   Sec2Time(ComAppHeadRX->Now); //实时校时，到秒级
}*/
void Sec2Time(DWORD sec)
{
    static BOOL FirstSyncFlag=FALSE;
    if(FirstSyncFlag==FALSE)
    {
        FirstSyncFlag = TRUE;
    }
    PublicTime.sec = sec;
    PublicTime.msec = 0;
    //TimerCounterSet(SOC_TMR_0_REGS, TMR_TIMER34, 0);定时器计数器清零
    //对时后，时间就从当前绝对时间开始计算，对时后第一个10分钟数据不要，
    //从当前对时时刻起计算整10s 10min等决对时间  譬如3S 10S 20s有整10S数据，第一个7S数据无效，其他10min 2h类似
    // Last2H  = sec/Sec_2H;
    // Last10m = sec/Sec_10m;
    // Last5m  = sec/Sec_5m;
    // Last10s = sec/Sec_10s;

    //校时后初始化统计
}


// void Init_Time()
// {
//     DWORD sec;
//     sec = PublicTime.sec;
//     Last2H  = sec/Sec_2H;
//     Last10m = sec/Sec_10m;
//     Last5m  = sec/Sec_5m;
//     Last10s = sec/Sec_10s;
// }

// BOOL TimeIs10Sec()// 10 20 30 这种整10S数据
// {
//     DWORD Dsec;
//     Dsec = PublicTime.sec;
//     if((Dsec/Sec_10s)!=Last10s)
//     {
//         Last10s = Dsec/Sec_10s;
//         return TRUE;
//     }
//     return FALSE;
// }

// BOOL TimeIs5Min()//只有 0 5  10 15 20这种整点时间  时间绝对五分钟
// {
//     DWORD Dsec;
//     Dsec = PublicTime.sec;
//     if((Dsec/Sec_5m)!=Last5m)
//     {
//         Last5m = Dsec/Sec_5m;
//         return TRUE;
//     }
//     return FALSE;
// }

// BOOL TimeIs10Min()//只有 0 10  20 30 40这种整点时间  时间绝对十分钟
// {
//     DWORD Dsec;
//     Dsec = PublicTime.sec;
//     if((Dsec/Sec_10m)!=Last10m)
//     {
//         Last10m = Dsec/Sec_10m;
//         return TRUE;
//     }
//     return FALSE;
// }

// BOOL TimeIs2Hour()
// {
//     DWORD Dsec;
//     Dsec = PublicTime.sec;
//     if((Dsec/Sec_2H)!=Last2H)
//     {
//         Last2H = Dsec/Sec_2H;
//         return TRUE;
//     }
//     return FALSE;
// }

// BOOL TimeIs24Hour()
// {
//     DWORD Dsec;
//     Dsec = PublicTime.sec;
//     if((Dsec/Sec_24H)!=Last24H)
//     {
//         Last24H = Dsec/Sec_24H;
//         return TRUE;
//     }
//     return FALSE;
// }


/*获取系统当前时间*/
BOOL get_sys_time(TRealTimer *stime)
{
	if ( NULL == stime )
	{
		return FALSE;
	}

	stime->wYear=RealTimer.wYear;
	stime->Mon=RealTimer.Mon;
	stime->Day=RealTimer.Day;
	stime->Hour=RealTimer.Hour;
	stime->Min=RealTimer.Min;
	stime->Sec=RealTimer.Sec;

	return TRUE;
}

BOOL JudgeLeapYear(WORD year)
{
    if(year % 400 == 0) //判断是否为400的倍数
    {
        return TRUE;
    }
    else if((year % 4 == 0) && (year % 100 !=0))//判断是否为4的倍数的同时是否为100的倍数
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
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

	memcpy((BYTE *)&t, (BYTE *)p, sizeof(TRealTimer));

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


BYTE api_WriteMeterTime(TRealTimer * t)

{
	//STime  *pRealTimer = (STime *)(ProBuf+8*sizeof(BYTE));


	memcpy((BYTE *)&RealTimer, (BYTE *)t, sizeof(TRealTimer));

	Sec2Time(api_CalcInTimeRelativeSec(&RealTimer));//转换为秒

	return 1;
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
//函数功能: 比较两个时间的大小
//
//参数: 
//			tNew[in]			绝对时间的时间格式，值必须是Hex码
//			tOld[in]			绝对时间的时间格式，值必须是Hex码
//                    
//返回值:  	>0	tNew > tOld
//			=0	tNew = tOld
//			<0	tNew < tOld
//
//备注:     时间都是Hex码,返回值为有符号数
//-----------------------------------------------
long api_CompareTwoTime(TRealTimer * tNew, TRealTimer * tOld)
{
	DWORD SecNew,SecOld;

	//计算相对秒数
	SecNew = api_CalcInTimeRelativeSec(tNew);
	SecOld = api_CalcInTimeRelativeSec(tOld);
	
	if(SecNew > SecOld)
	{
	  	return 1;
	}
	else if(SecNew < SecOld)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

//---------------------------------------------------------------
//函数功能: 获取相对时间秒数
//
//参数: 	
//
//返回值:相对时间秒数  
//
//备注:获取电表中当前时间与2000年1月1日0时0分0秒之间的时间差，单位：秒
//---------------------------------------------------------------
DWORD api_GetRelativeSec( void )
{
	return api_CalcInTimeRelativeSec( (TRealTimer *)&RealTimer );
}

//-----------------------------------------------
//函数功能: 读取时间，
//
//参数:
//			
//
//返回值:  	TRUE:正确读取       FALSE:没有正确读取
//备注:
//-----------------------------------------------
BOOL api_ReadMeterTime( TRealTimer* Timer )
{
	memcpy((BYTE*)Timer, (BYTE *)&RealTimer, sizeof(TRealTimer));

	return TRUE;
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

//--------------------------------------------------
//功能描述:获取当前分钟数
//
//参数:
//
//返回值:
//
//备注内容:  无
//--------------------------------------------------
DWORD api_GetRelativeMin( void )
{
	return api_CalcInTimeRelativeMin(&RealTimer);
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

	TRealTimer	PowerDownTime;

	PowerDownTime.wYear = 2017;
	PowerDownTime.Mon = 12;
	PowerDownTime.Day = 1;
	PowerDownTime.Hour = 0;
	PowerDownTime.Min = 0;
	PowerDownTime.Sec = 0;

	api_WriteMeterTime(&PowerDownTime);
}



//日历功能
void TimeRtcFun()
{
    BYTE DayArray[12];

    //ClearUsesFlag(); //us也清零
	RealTimer.Sec++;
	if(RealTimer.Sec >= 60)
	{
		RealTimer.Sec -= 60;
		RealTimer.Min++;

		if(RealTimer.Min >= 60)
		{
			RealTimer.Min -= 60;
			RealTimer.Hour++;

			if(RealTimer.Hour >= 24)
			{
				RealTimer.Hour -= 24;
				RealTimer.Day++;

				// if(RealTimer.Day > ( 365+((RealTimer.wYear%4)==0?1:0) ))
				// {
				// 	RealTimer.Day = 1;
				// }
                // 判断是否为闰年
                if(JudgeLeapYear(RealTimer.wYear))
                {
                    memcpy(DayArray,LeapYearDay,sizeof(DayArray));
                }
                else
                {
                    memcpy(DayArray,NormalYearDay,sizeof(DayArray));
                }
                if(RealTimer.Day > DayArray[RealTimer.Mon-1])
                {
                    RealTimer.Day -= DayArray[RealTimer.Mon-1];
                    RealTimer.Mon++;

                    if(RealTimer.Mon > 12)
                    {
                        RealTimer.Mon -= 12;
                        RealTimer.wYear++;
                    }
                }
			}
		}
	}
}

//粗略定时处理，例如1分钟定时，在大循环中调用
void api_RtcTask(void)
{
	static TRealTimer TmNow;
	static TRealTimer TmpRealTimer;

	//500毫秒读一次时间
	if( api_GetTaskFlag( eTASK_RTC_ID, eFLAG_500_MS ) == TRUE )
	{
		api_ClrTaskFlag( eTASK_RTC_ID, eFLAG_500_MS );

		memcpy( (BYTE*)&TmpRealTimer, (BYTE*)&TmNow, sizeof(TRealTimer) );

		get_sys_time(&TmNow);

		if(TmpRealTimer.Sec!=TmNow.Sec)
		{
			//秒变化
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_SECOND );
            if(TmNow.Sec % 10 == 0)
            {
                api_SetTaskFlag( eTASK_RTC_ID, eFLAG_10_S );
            }
		}
		if( TmpRealTimer.Min != TmNow.Min )
		{
			//分钟变化
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_MINUTE );
            if(TmNow.Min % 5 == 0)
            {
                api_SetTaskFlag( eTASK_RTC_ID, eFLAG_5_MIN );
            }
            if(TmNow.Min % 10 == 0)
            {
                api_SetTaskFlag( eTASK_RTC_ID, eFLAG_10_MIN );
            }
		}
		if( TmpRealTimer.Hour != TmNow.Hour )
		{
			//小时变化
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_HOUR );
            if(TmNow.Hour % 2 == 0)
            {
                api_SetTaskFlag( eTASK_RTC_ID, eFLAG_2_H );
            }
            if(TmNow.Hour == 0)
            {
                api_SetTaskFlag( eTASK_RTC_ID, eFLAG_24_H );
            }
		}
	}

	//1秒到
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_SECOND) == TRUE )
	{
		api_SetAllTaskFlag(eFLAG_SECOND); //这样能确保在前面的任务一定先执行
		api_ClrTaskFlag(eTASK_RTC_ID, eFLAG_SECOND);
	}
    //10秒到
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_10_S) == TRUE )
	{
		api_SetAllTaskFlag(eFLAG_10_S); //这样能确保在前面的任务一定先执行
		api_ClrTaskFlag(eTASK_RTC_ID, eFLAG_10_S);
	}

	//1分钟到
	if (api_GetTaskFlag(eTASK_RTC_ID, eFLAG_MINUTE) == TRUE)
	{
		api_SetAllTaskFlag(eFLAG_MINUTE);
		api_ClrTaskFlag(eTASK_RTC_ID, eFLAG_MINUTE);
	}
    //5分钟到
	if (api_GetTaskFlag(eTASK_RTC_ID, eFLAG_5_MIN) == TRUE)
	{
		api_SetAllTaskFlag(eFLAG_5_MIN);
		api_ClrTaskFlag(eTASK_RTC_ID, eFLAG_5_MIN);
	}
    //10分钟到
	if (api_GetTaskFlag(eTASK_RTC_ID, eFLAG_10_MIN) == TRUE)
	{
		api_SetAllTaskFlag(eFLAG_10_MIN);
		api_ClrTaskFlag(eTASK_RTC_ID, eFLAG_10_MIN);
	}

	//1小时到
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_HOUR) == TRUE )
	{
		api_SetAllTaskFlag( eFLAG_HOUR );//这样能确保在前面的任务一定先执行
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_HOUR);
	}
    //2小时到
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_2_H) == TRUE )
	{
		api_SetAllTaskFlag( eFLAG_2_H );//这样能确保在前面的任务一定先执行
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_2_H);
	}
    //24小时到
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_24_H) == TRUE )
	{
		api_SetAllTaskFlag( eFLAG_24_H );//这样能确保在前面的任务一定先执行
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_24_H);
	}



}

