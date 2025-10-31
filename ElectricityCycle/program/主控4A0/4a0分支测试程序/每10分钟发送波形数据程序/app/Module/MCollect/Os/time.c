
#include "flats.h"

#define HourSeconds        (60*60)		  /*时的总秒数*/
#define DaySeconds         (24*60*60)     /*日的总秒数*/
#define MaxMonth           (31*24*60*60)  /*大月份的总秒数*/
#define MinMonth           (30*24*60*60)  /*小月份的总秒数*/
#define Leap2Month         (29*24*60*60)  /*闰月时二月份的总秒数*/
#define NoneLeap2Month     (28*24*60*60)  /*不闰月时二月份的总秒数*/
#define LeapYearSecond     (366*24*60*60) /*闰年时每年的总秒数*/
#define NoneLeapYearSecond (365*24*60*60) /*不闰年时每年的总秒数*/

const DWORD MonthSeconds[12]={MaxMonth,NoneLeap2Month,MaxMonth,MinMonth,MaxMonth,MinMonth,MaxMonth,MaxMonth,MinMonth,MaxMonth,MinMonth,MaxMonth};
const BYTE MonthDays[12]={31,28,31,30,31,30,31,31,30,31,30,31};

extern WORD api_GetRtcDateTime( WORD Type, BYTE *pBuf );

BOOL GetTime(TTime *pT)
{	
BOOL bRc;
BYTE Buffer[8];

	//	// SM_P2(MPV_TIME);
    api_GetRtcDateTime(0x807F,Buffer);
	pT->Sec = Buffer[0];
	pT->Min = Buffer[1];
	pT->Hour = Buffer[2];
	pT->Day = Buffer[4];
	pT->Mon = Buffer[5];
	pT->wYear = Buffer[6]+2000;
	bRc = TimeIsValid(pT);
	//	// SM_V2(MPV_TIME);

	return bRc;
}

// DWORD GetMonMins(TTime *pTime)
// {
// DWORD dwMins=MonthSeconds[pTime->Mon-1]/60;
	
// 	if(((pTime->wYear%4)==0)
// 		&&(pTime->Mon == 2))
// 		dwMins	+= 1440;
// 	return dwMins;
// }

// BOOL SetTime(TTime *pT )
// {
// BOOL bRc=TRUE;

// 	return bRc;
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : TimeIsValid
//	函数功能 : 判断时间是否是合法时间
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年7月17日
//	返 回 值 : BOOL
//	参数说明 : TTime *pTm
///////////////////////////////////////////////////////////////
BOOL TimeIsValid(TTime *pTm)
{
    if(pTm->Sec>59) 
		return FALSE;
    if(pTm->Min>59)
		return FALSE;
    if(pTm->Hour>23) 
		return FALSE;
    if((BYTE)(pTm->Mon-1)>=12) 
		return FALSE;	
	if( (pTm->wYear <2000) ||(pTm->wYear >2099))
		return FALSE;

	if((pTm->wYear%4)==0)
	{
		if(pTm->Mon == 2)
		{
			if((BYTE)(pTm->Day-1)>=29)
				return FALSE;
		}
		else if((BYTE)(pTm->Day-1)>=MonthDays[pTm->Mon-1])
			return FALSE;
	}
	else if((BYTE)(pTm->Day-1)>=MonthDays[pTm->Mon-1])
		return FALSE;
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : Time2Sec
//	函数功能 : 将时间转换为相对于2000/1/1的秒数
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年7月17日
//	返 回 值 : DWORD
//	参数说明 : TTime *pT，输入项，
///////////////////////////////////////////////////////////////
DWORD Time2Sec(TTime *pT)
{
DWORD dwSeconds=0;
WORD wYear;
BYTE Mon;
DWORD dwTmp;
	
	if(!TimeIsValid(pT))
	{
		return 0;
	}
	for(wYear=2000;wYear<pT->wYear;wYear++)
	{
		if((wYear%4)==0)
			dwTmp=LeapYearSecond;
		else
			dwTmp=NoneLeapYearSecond;
		dwSeconds+=dwTmp;
	}
	for(Mon=0;Mon<pT->Mon-1;Mon++)
	{//闰年2月秒数在非闰年2月秒数基础上加1天
		if((Mon == 1)&&((pT->wYear%4)==0))
			dwTmp=MonthSeconds[Mon]+24*60*60;
		else
			dwTmp=MonthSeconds[Mon];
		dwSeconds+=dwTmp;
	}
	dwSeconds+=(pT->Day-1)*DaySeconds;
	dwSeconds+=pT->Hour*HourSeconds;
	dwSeconds+=pT->Min*60;
	dwSeconds+=pT->Sec;
	return dwSeconds;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : Sec2Time
//	函数功能 : 将相对于2000/1/1的秒数转换为系统时间格式
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年7月17日
//	返 回 值 : void
//	参数说明 : DWORD dwSeconds,相对2000/1/1的秒数
//				TTime *pT，输出项，系统时间地址
///////////////////////////////////////////////////////////////
void  Sec2Time(DWORD dwSeconds,TTime *pT)
{
WORD wYear;
BYTE Mon;
DWORD dwTmp;

	for(wYear=2000;wYear<2100;wYear++)
	{
		if((wYear%4)==0)
			dwTmp=LeapYearSecond;
		else
			dwTmp=NoneLeapYearSecond;
		if(dwSeconds<dwTmp)
			break;
		dwSeconds-=dwTmp;
	}
	pT->wYear = wYear; 
	for(Mon=0;Mon<12;Mon++)
	{
		if((Mon == 1)&&((wYear%4)==0))
			dwTmp=MonthSeconds[Mon]+24*60*60;
		else
			dwTmp=MonthSeconds[Mon];
		
		if(dwSeconds<dwTmp)
			break;
		dwSeconds-=dwTmp;
	}
	pT->Mon=Mon+1; 
	pT->Day=(BYTE)(dwSeconds/DaySeconds+1);
	dwSeconds%=DaySeconds;
	pT->Hour=(BYTE)(dwSeconds/HourSeconds);	
	dwSeconds%=HourSeconds;	
	pT->Min=(BYTE)(dwSeconds/60);
	pT->Sec=(BYTE)(dwSeconds%60);
}

DWORD Time2Min(TTime *pT)
{
	return Time2Sec(pT)/60;
}
 
void  Min2Time(DWORD dwMins,TTime *pT)
{
	Sec2Time(dwMins*60,pT);
}

///////////////////////////////////////////////////////////////
//	函 数 名 : GetWeek
//	函数功能 : 获取星期数
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年7月17日
//	返 回 值 : BYTE 7:周日，1-6，周一至周六
//	参数说明 : TTime *pT
///////////////////////////////////////////////////////////////
BYTE GetWeek(TTime *pT)
{
DWORD dwSecs=Time2Sec(pT);
BYTE byWeek;

	byWeek	= (BYTE)((dwSecs/DaySeconds + 6)%7);
	if(byWeek == 0)
		return 7;
	return byWeek;
}

