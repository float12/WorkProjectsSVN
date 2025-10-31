//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	胡春华
//创建日期	2016.9.9
//描述		时间的通用函数，不包括具体的时钟芯片
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
#include <time.h>
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
const char timeip[]="ntp1.aliyun.com";
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//实时时钟，BCD码方式的时钟
TRealTimer RealTimer;
BYTE g_PowerOnSecTimer; //上电30秒倒计时 所有上电延时的操作 都用改计数器 只有上电大于30秒，掉电时才数据转存
//系统全局变量 电表中当前时间与2000年1月1日0时0分之间的时间差，单位：分钟 (用函数api_CalcInTimeRelativeMin()计算值)，
DWORD g_RelativeMin;

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//							1   2   3   4   5   6   7   8   9   10  11  12
const BYTE NormalYearDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//					  	1   2   3   4   5   6   7   8   9   10  11  12
const BYTE LeapYearDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
BOOL api_CheckMonDay(WORD wYear, BYTE byMon, BYTE byDay);
extern void InitRtc(void);
extern BOOL ReadOutClockHara(WORD Type);
extern BOOL WriteOutClock(TRealTimer *t);

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
int GetTimeto698(TRealTimerDate *pTime)
{
	nwy_time_t julian_time = {0};
	char timezone = 0;
	nwy_get_time(&julian_time, &timezone);
	// nwy_ext_echo("\r\n%d-%d-%d %d:%d:%d UTC:%d", julian_time.year, julian_time.mon, julian_time.day, julian_time.hour, julian_time.min, julian_time.sec, timezone);

	memset(pTime, 0, sizeof(TRealTimer));
	lib_ExchangeData((BYTE *)&pTime->wYear, (BYTE *)&julian_time.year, 2);
	pTime->Mon = julian_time.mon;
	pTime->Day = julian_time.day;
	pTime->Hour = julian_time.hour;
	pTime->Min = julian_time.min;
	pTime->Sec = julian_time.sec;
	pTime->Week = julian_time.wday;
	pTime->Ms = 0;
	return TRUE;
}
int get_N176_time(TRealTimer *pTime)
{
	nwy_time_t julian_time = {0};
	char timezone = 0;
	nwy_get_time(&julian_time, &timezone);
	// nwy_ext_echo("\r\n%d-%d-%d %d:%d:%d UTC:%d", julian_time.year, julian_time.mon, julian_time.day, julian_time.hour, julian_time.min, julian_time.sec, timezone);

	memset(pTime, 0, sizeof(TRealTimer));
	pTime->wYear = julian_time.year;
	pTime->Mon = julian_time.mon;
	pTime->Day = julian_time.day;
	pTime->Hour = julian_time.hour;
	pTime->Min = julian_time.min;
	pTime->Sec = julian_time.sec;
	pTime->Timezone = timezone;
	return TRUE;
}
char timebuf[50];
char *get_N176_time_stringp()
{
	nwy_time_t julian_time = {0};
	char timezone = 0;
	nwy_get_time(&julian_time, &timezone);
	sprintf(timebuf, "%4d-%2d-%2d %2d:%2d:%2d", julian_time.year, julian_time.mon, julian_time.day, julian_time.hour, julian_time.min, julian_time.sec);
	return timebuf;
}
BOOL set_N176_time_stringp(char *ctime)
{
	nwy_time_t julian_time = {0};
	int gtbuf[6] = {0};
	BOOL rtvalue = sscanf(ctime, "%d-%d-%d %d:%d:%d", gtbuf, gtbuf + 1, gtbuf + 2, gtbuf + 3, gtbuf + 4, gtbuf + 5);
	julian_time.year = gtbuf[0];
	julian_time.mon = gtbuf[1];
	julian_time.day = gtbuf[2];
	julian_time.hour = gtbuf[3];
	julian_time.min = gtbuf[4];
	julian_time.sec = gtbuf[5];
	nwy_set_time(&julian_time, 8);
	return rtvalue;
}
//  刷新系统全局变量 电表中当前时间与2000年1月1日0时0分之间的时间差，单位：分钟
void RefreshGlobalVariableMinNow(void)
{
	g_RelativeMin = api_CalcInTimeRelativeMin(&RealTimer);
}
BOOL IsLeapYear(BYTE Year)
{
	if (Year & 0x0003)
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
	if (RealTimer.Sec >= 60)
	{
		// 清除秒计数
		RealTimer.Sec = 0;

		// 分加一
		RealTimer.Min++;

		// 判断分溢出
		if (RealTimer.Min >= 60)
		{
			// 清除分计数
			RealTimer.Min = 0;

			// 小时加1
			RealTimer.Hour++;

			// 判断小时溢出
			if (RealTimer.Hour >= 24)
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
				if (IsLeapYear(RealTimer.wYear - 2000) == TRUE)
				{
					//p = (BYTE *)LeapYearDayHex;
					memcpy(Buf, LeapYearDay, sizeof(Buf));
				}
				else
				{
					//p = (BYTE *)NormalDayHex;
					memcpy(Buf, NormalYearDay, sizeof(Buf));
				}

				// 判断月份是否到
				if (RealTimer.Day > Buf[RealTimer.Mon - 1])
				{
					// 初始化天
					RealTimer.Day = 1;

					// 月加1
					RealTimer.Mon++;

					// 月是否溢出
					if (RealTimer.Mon > 12)
					{
						// 月初始化
						RealTimer.Mon = 1;

						// 年加1
						RealTimer.wYear++;
						if (RealTimer.wYear > 2099)
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

void set_N176_time(TRealTimer *pTime)
{
	nwy_time_t julian_time = {0};

	julian_time.year = pTime->wYear;
	julian_time.mon = pTime->Mon;
	julian_time.day = pTime->Day;
	julian_time.hour = pTime->Hour;
	julian_time.min = pTime->Min;
	julian_time.sec = pTime->Sec;
	nwy_set_time(&julian_time, pTime->Timezone);
}
//-----------------------------------------------
//函数功能: 相对时间转化为绝对时间，相对时间指以分钟为单位，以2000年1月1日0点0分为起点，100年内有效
//
//参数:
//			Min[in]				相对时间的分钟数
//			p[out]				返回绝对时间（年月日时分秒格式）
//
//返回值:  	TRUE:正确转换	   FALSE:没有正确转换
//备注:	 时间都是Hex码
//-----------------------------------------------
BOOL api_ToAbsTime(DWORD Min, TRealTimer *p)
{
	WORD i;
	DWORD Temp, Temp1, Temp2;
	BYTE Buf[sizeof(NormalYearDay)];

	//正确性判断
	if (p == NULL)
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

	for (i = 0; i < 100; i++)
	{
		Temp2 = Temp1;

		if (i & 0x0003)
		{
			Temp1 += 365;
		}
		else
		{
			Temp1 += 366;
		}

		if (Temp1 > Temp)
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
	if ((p->wYear & 0x0003) == 0) // p->Year 不是BCD，不能用 if( IsLeapYear(p->Year) == TRUE )
	{
		memcpy(Buf, LeapYearDay, sizeof(Buf));
	}
	else
	{
		memcpy(Buf, NormalYearDay, sizeof(Buf));
	}

	for (i = 0; i < 12; i++)
	{
		Temp2 = Temp1;

		Temp1 += Buf[i];

		if (Temp1 > Temp)
		{
			break;
		}
	}

	p->Mon = i + 1;

	//剩余天
	Temp = Temp - Temp2;

	//计算天
	p->Day = (BYTE)(Temp + 1);

	p->wYear += 2000; //年加上2000

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
//备注:	 时间都是Hex码
//-----------------------------------------------
DWORD api_CalcInTimeRelativeMin(TRealTimer *t)
{
	DWORD TotalTime;

	TotalTime = api_CalcInTimeRelativeSec(t);
	if (TotalTime == ILLEGAL_VALUE_8F)
	{
		return ILLEGAL_VALUE_8F;
	}

	TotalTime /= 60;

	return (TotalTime);
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
//			0:		  tNew < tOld
//备注:	 时间都是Hex码
//-----------------------------------------------
DWORD api_CalcIntervalMinute(TRealTimer *tNew, TRealTimer *tOld)
{
	DWORD MinNew, MinOld;

	//计算相对分钟数目
	MinNew = api_CalcInTimeRelativeMin(tNew);
	if (MinNew == ILLEGAL_VALUE_8F)
	{
		return ILLEGAL_VALUE_8F;
	}
	MinOld = api_CalcInTimeRelativeMin(tOld);
	if (MinOld == ILLEGAL_VALUE_8F)
	{
		return ILLEGAL_VALUE_8F;
	}

	if (MinNew > MinOld)
	{
		MinNew -= MinOld;
	}
	else
	{
		MinNew = 0;
	}

	return MinNew;
}

//--------------------------------------------------
//功能描述:  获取时间戳
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
QWORD getmktimems(TRealTimer *t)
{
	struct tm timeinfo = {0};

	// 设置时间信息
	timeinfo.tm_year = t->wYear - 1900; // 年份需要减去 1900
	timeinfo.tm_mon = t->Mon - 1;	   // 月份从 0 开始，所以 9 表示十月
	timeinfo.tm_mday = t->Day;		  // 日期
	timeinfo.tm_hour = t->Hour;		 // 小时
	timeinfo.tm_min = t->Min;		   // 分钟
	timeinfo.tm_sec = t->Sec;		   // 秒

	QWORD timestamp = (QWORD)mktime(&timeinfo);
	//nwy_ext_echo("TT:%d",t->Timezone);
	return (QWORD)(timestamp - 3600ll * (t->Timezone)) * 1000;
}
void nwy_update_time_cb(void *handle, nwy_updatetime_errid err_code, nwy_time_t *time)
{
	if (NWY_UPDATETIME_SDK_SUCCESS == err_code)
	{
		nwy_ext_echo("\r\ntimeupdataOK:%d", err_code);
		nwy_set_time(time, 8);
	}
	else
	{
		nwy_ext_echo("\r\ntimeupdataerr:%d", err_code);
	}
}
//--------------------------------------------------
//功能描述:
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void api_UpdateForInte(void)
{
	char timezone[] = "E8";
	nwy_updatetime_ntp((char *)timeip, 20, timezone, 0, nwy_update_time_cb);
}
//-----------------------------------------------
//函数功能: 计算相对时间的秒数
//
//参数:
//			t[in]			绝对时间的时间格式，值必须是Hex码
//
//返回值:  	相对时间的秒数
//			0xffffffff  时间非法
//备注:	 时间都是Hex码
//-----------------------------------------------
QWORD api_CalcInTimeRelativeSec(TRealTimer *t)
{
	QWORD TotalTime;
	WORD tyear, tmon, tday, i;

	if (api_CheckClock(t) == FALSE)
	{
	   
		return ILLEGAL_VALUE_8F;
	}

	//计算年含的天数
	tyear = t->wYear % 100;
	if (tyear != 0)
	{
		TotalTime = (DWORD)365 * tyear + (tyear - 1) / 4 + 1;
	}
	else
	{
		TotalTime = 0;
	}

	//加上月含的天数
	tmon = t->Mon;
	tday = 0;
	for (i = 0; i < (tmon - 1); i++)
	{
		if ((tyear & 0x03) == 0) //闰年
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

	return (TotalTime);
}

//-----------------------------------------------
//函数功能: 判断时钟有效性的函数
//
//参数:
//			p[in]				TRealTimer结构的绝对时间，值必须是Hex码
//
//返回值:  	FALSE--数据非法 TRUE--数据合法
//备注:	 时间都是Hex码
//-----------------------------------------------
BOOL api_CheckClock(TRealTimer *p)
{
	TRealTimer t;

	memcpy((BYTE *)&t, (BYTE *)p, sizeof(TRealTimer));

	//检查月、日数据合法性，参数 byYear, byMon, byDay 都和 RealTimer 中数据一样是Hex码
	if (api_CheckMonDay(t.wYear, t.Mon, t.Day) == FALSE)
	{
		return FALSE;
	}

	if (t.Hour > MAX_VALID_HOUR)
	{
		return FALSE;
	}

	if (t.Min > MAX_VALID_MINUTE)
	{
		return FALSE;
	}

	if (t.Sec >= 60)
	{
		return FALSE;
	}

	return TRUE;
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
//备注:	 时间都是Hex码
//-----------------------------------------------
BOOL api_CheckMonDay(WORD wYear, BYTE byMon, BYTE byDay)
{
	wYear %= 100;
	
	if ((byMon == 0) || (byMon > MAX_VALID_MONTH))
	{
		return FALSE;
	}

	if (byDay == 0)
	{
		return FALSE;
	}

	if ((wYear & 0x03) == 0) //是闰年
	{
		if (byDay > LeapYearDay[byMon - 1])
		{
			return FALSE;
		}
	}
	else
	{
		if (byDay > NormalYearDay[byMon - 1])
		{
			return FALSE;
		}
	}

	return TRUE;
}
