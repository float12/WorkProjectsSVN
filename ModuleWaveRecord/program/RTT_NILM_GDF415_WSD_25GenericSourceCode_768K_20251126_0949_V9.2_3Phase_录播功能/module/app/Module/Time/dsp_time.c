#include "AllHead.h"
//#include "task_evt.h"
#include "task_sample.h"
//#include "task_stats.h"


/************************************************************************
 * 全局时钟定义;全局私有日历定义;
 ************************************************************************/
BOOL SyncDspTime = FALSE;

TRealTimer RealTimer;//此处作为timer的主定义  声明为全局变量
static TRealTimer TmNow;	// 王工之前放在api_RtcTask内部，后面改进时可以删掉，参考电表api_RtcTask
volatile DSPTIME PublicTime;//公历

extern TEventPara gEventPara;
extern int gdwUartBps ;
DWORD Origin = 0;
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
		// FirstSyncAlign(sec - PublicTime.sec);
	}
	PublicTime.sec = sec;
	PublicTime.msec = 0;
}

/*获取当前处理点的时间与当前时间的差，使用固定采样率算点间时间 误差跟读写点之间同步有关*/
void get_time_current_Pnt(DSPTIME *tm)
{
    DWORD ms,s;
    // 获取当前时间
    tm->sec = PublicTime.sec;//把系统时间转为秒
    tm->msec= PublicTime.msec;//使用ARM频繁守时，这里只能用timer的ms
    // 获取读写点之间的间隔，找到写采样值时间，采样中断打时标，最多5个点时间差，帧数越多时间精度越差
    ms = GetPrivateTime()-get_R_pos_Time();
    if(ms>1)
    {
        s = ms/1000;
        ms%=1000;//剩余MS时间
        if(tm->msec<ms)
        {
            tm->msec = tm->msec + 1000;
            tm->sec--;
        }
        tm->msec -= ms;
        tm->sec -= s;
    }
}

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

	if(t.wYear == 0)
	{
		return FALSE;
	}

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

	if( api_CheckClock(t) == FALSE )
	{
		return 0;
	}

	memcpy((BYTE *)&RealTimer, (BYTE *)t, sizeof(TRealTimer));

	memcpy((BYTE *)&TmNow, (BYTE *)t, sizeof(TRealTimer));// 打补丁，为了防止改时间同一个时间点存2条冻结记录

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


BOOL api_SecToAbsTime(DWORD dwSec, TRealTimer *p)
{
	WORD i;
	DWORD Temp, Temp1, Temp2;
	BYTE Buf[sizeof(NormalYearDay)];

	//正确性判断
	if( p == NULL )
	{
		return FALSE;
	}

	Temp = dwSec/60;

	memset(p, 0, sizeof(TRealTimer));

	//秒直接置0
	p->Sec = dwSec%60;
	
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

/////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------
//函数功能: RTC任务上电初始化
//
//参数:		无
//
//返回值:	无
//
//备注:
//-----------------------------------------------
extern TSafeModePara SafeModePara;

void api_PowerUpRtc(void)
{
	// BYTE len;
	DWORD		dwAddr;
	TLostPowerTime LostPowerTime;
	TRealTimer	PowerDownTime;


	api_ReadMeterTime( &(LostPowerTime.Time) );//获取当前时间
	dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.LostPowerTime );
	api_VReadSafeMem( dwAddr, sizeof(TLostPowerTime) , (BYTE*)&LostPowerTime.Time );
	

	PowerDownTime.wYear = 2024;
	PowerDownTime.Mon = 4;
	PowerDownTime.Day = 17;
	PowerDownTime.Hour = 13;
	PowerDownTime.Min = 53;
	PowerDownTime.Sec = 00;

	api_WriteMeterTime(&PowerDownTime);

	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE*)&SafeModePara );

	GetDefaultThresholdValue(0); // 初始化波特率gEventPara，上报信息gReportMode

	if((gEventPara.nUartBps != UART_DEFAULT_BAUD) && (gEventPara.nUartBps != UART_DEFAULT_BAUD_SUB))
	{
		gdwUartBps = UART_DEFAULT_BAUD;
		gEventPara.nUartBps = UART_DEFAULT_BAUD;
	}
	else
	{
		gdwUartBps = gEventPara.nUartBps;
	}
}



//日历功能
void TimeRtcFun()
{
	BYTE DayArray[12];
	DWORD present = 0;
	//ClearUsesFlag(); //us也清零
	
	if(((UsePrintfFlag >> 5) & 0x0001) == 0x0001)
	{
		present = Count_features_002s;
		if(abs(present - Origin) >= 50)
		{
			RealTimer.Sec++;
			Origin = present;
		}
	}
	else
	{
		RealTimer.Sec++;
	}

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
	TRealTimer TmpRealTimer;

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
			if(TmNow.Min % 15 == 0)
            {
                api_SetTaskFlag( eTASK_RTC_ID, eFLAG_15_MIN );
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
	//15分钟到
	if (api_GetTaskFlag(eTASK_RTC_ID, eFLAG_15_MIN) == TRUE)
	{
		api_SetAllTaskFlag(eFLAG_15_MIN);
		api_ClrTaskFlag(eTASK_RTC_ID, eFLAG_15_MIN);
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

