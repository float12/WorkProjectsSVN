//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	������
//��������	2016.9.9
//����		ʱ���ͨ�ú����������������ʱ��оƬ
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
#include <time.h>
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
const char timeip[]="ntp1.aliyun.com";
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//ʵʱʱ�ӣ�BCD�뷽ʽ��ʱ��
TRealTimer RealTimer;
BYTE g_PowerOnSecTimer; //�ϵ�30�뵹��ʱ �����ϵ���ʱ�Ĳ��� ���øļ����� ֻ���ϵ����30�룬����ʱ������ת��
//ϵͳȫ�ֱ��� ����е�ǰʱ����2000��1��1��0ʱ0��֮���ʱ����λ������ (�ú���api_CalcInTimeRelativeMin()����ֵ)��
DWORD g_RelativeMin;

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//							1   2   3   4   5   6   7   8   9   10  11  12
const BYTE NormalYearDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//					  	1   2   3   4   5   6   7   8   9   10  11  12
const BYTE LeapYearDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
BOOL api_CheckMonDay(WORD wYear, BYTE byMon, BYTE byDay);
extern void InitRtc(void);
extern BOOL ReadOutClockHara(WORD Type);
extern BOOL WriteOutClock(TRealTimer *t);

//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
int GetTimeto698(TRealTimerDate *pTime)
{
	nwy_time_t julian_time = {0};
	int timezone = 0;
	nwy_date_get(&julian_time, &timezone);
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
	int timezone = 0;
	nwy_date_get(&julian_time, &timezone);
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
	int timezone = 0;
	nwy_date_get(&julian_time, &timezone);
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
	nwy_date_set(&julian_time, 8);
	return rtvalue;
}
//  ˢ��ϵͳȫ�ֱ��� ����е�ǰʱ����2000��1��1��0ʱ0��֮���ʱ����λ������
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

	// �������־
	//SecFlag = TRUE;

	// ���1
	RealTimer.Sec++;

	// �ж������
	if (RealTimer.Sec >= 60)
	{
		// ��������
		RealTimer.Sec = 0;

		// �ּ�һ
		RealTimer.Min++;

		// �жϷ����
		if (RealTimer.Min >= 60)
		{
			// ����ּ���
			RealTimer.Min = 0;

			// Сʱ��1
			RealTimer.Hour++;

			// �ж�Сʱ���
			if (RealTimer.Hour >= 24)
			{
				// ���Сʱ
				RealTimer.Hour = 0;

				// ���1
				RealTimer.Day++;

				// ��������
				//RealTimer.Week++;

				// �����Ƿ�
				//if( RealTimer.Week >= 0x7 )
				//{
				//	// �������
				//	RealTimer.Week = 0;
				//}

				// �ж��Ƿ�Ϊ����
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

				// �ж��·��Ƿ�
				if (RealTimer.Day > Buf[RealTimer.Mon - 1])
				{
					// ��ʼ����
					RealTimer.Day = 1;

					// �¼�1
					RealTimer.Mon++;

					// ���Ƿ����
					if (RealTimer.Mon > 12)
					{
						// �³�ʼ��
						RealTimer.Mon = 1;

						// ���1
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
	nwy_date_set(&julian_time, pTime->Timezone);
}
//-----------------------------------------------
//��������: ���ʱ��ת��Ϊ����ʱ�䣬���ʱ��ָ�Է���Ϊ��λ����2000��1��1��0��0��Ϊ��㣬100������Ч
//
//����:
//			Min[in]				���ʱ��ķ�����
//			p[out]				���ؾ���ʱ�䣨������ʱ�����ʽ��
//
//����ֵ:  	TRUE:��ȷת��	   FALSE:û����ȷת��
//��ע:	 ʱ�䶼��Hex��
//-----------------------------------------------
BOOL api_ToAbsTime(DWORD Min, TRealTimer *p)
{
	WORD i;
	DWORD Temp, Temp1, Temp2;
	BYTE Buf[sizeof(NormalYearDay)];

	//��ȷ���ж�
	if (p == NULL)
	{
		return FALSE;
	}

	Temp = Min;

	memset(p, 0, sizeof(TRealTimer));

	//��ֱ����0
	p->Sec = 0;

	//�õ�����
	p->Min = (BYTE)(Temp % 60);

	//ʣ�������
	Temp -= p->Min;

	//ת��ΪСʱ
	Temp /= 60;

	//�õ�Сʱ
	p->Hour = (BYTE)(Temp % 24);

	//ʣ��Сʱ��
	Temp -= p->Hour;

	//ת��Ϊ��
	Temp /= 24;

	//������
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

	//ʣ������
	Temp = Temp - Temp2;

	//������
	Temp1 = 0;

	// �ж��Ƿ�Ϊ����
	if ((p->wYear & 0x0003) == 0) // p->Year ����BCD�������� if( IsLeapYear(p->Year) == TRUE )
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

	//ʣ����
	Temp = Temp - Temp2;

	//������
	p->Day = (BYTE)(Temp + 1);

	p->wYear += 2000; //�����2000

	return TRUE;
}

//-----------------------------------------------
//��������: �������ʱ��ķ���������2000��1��1��0��0��Ϊ���
//
//����:
//			t[in]			����ʱ���ʱ���ʽ��ֵ������Hex��
//
//����ֵ:  	���ʱ��ֵ
//			0xffffffff  ʱ��Ƿ�
//��ע:	 ʱ�䶼��Hex��
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
//��������: ������������ʱ��֮��ķ�����
//
//����:
//			tNew[in]			����ʱ���ʱ���ʽ��ֵ������Hex��
//			tOld[in]			����ʱ���ʱ���ʽ��ֵ������Hex��
//
//����ֵ:  	��������ʱ��֮��ķ����� tNew - tOld
//			0xffffffff: ʱ���в��Ϸ������
//			0:		  tNew < tOld
//��ע:	 ʱ�䶼��Hex��
//-----------------------------------------------
DWORD api_CalcIntervalMinute(TRealTimer *tNew, TRealTimer *tOld)
{
	DWORD MinNew, MinOld;

	//������Է�����Ŀ
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
//��������:  ��ȡʱ���
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
QWORD getmktimems(TRealTimer *t)
{
	nwy_time_t timeinfo = {0};
	nwy_timeval_t timestamp = {0};
	QWORD timetamp = 0;
	// ����ʱ����Ϣ
	timeinfo.year = t->wYear; 
	timeinfo.mon = t->Mon ;	  
	timeinfo.day = t->Day;	
	timeinfo.hour = t->Hour;
	timeinfo.min = t->Min;	
	timeinfo.sec = t->Sec;	

	nwy_date_to_timestamp(&timeinfo,&timestamp); 
	timetamp = (timestamp.tv_sec - t->Timezone*3600ll)*1000;
	return timetamp;
}
static char * test_get_sntp_resp(NWY_SNTP_EVENT_E event)
{
	switch(event)
	{
		case     NWY_SNTP_SUCCESS:
			return "SNTP_SUCCESS";
		case     NWY_SNTP_FAIL:
			return "SNTP_FAIL";
		case     NWY_SNTP_DNS_ERROR:
			return "SNTP_DNS_ERROR";
		case     NWY_SNTP_NET_ERROR:
			return "SNTP_NET_ERROR";
		case     NWY_SNTP_TIMEOUT:
			return "SNTP_TIMEOUT";
		default:
			return "unknown error";
	}
}
static void nwy_cli_sntp_result_cb(nwy_sntp_result_type *event)
{
	char resp[128] = {0};

	if(event == NULL)
	{
		nwy_ext_echo(" \r\nsntp cb error ");
		return;
	}
	nwy_ext_echo("\r\nsntp status:%s ",test_get_sntp_resp(event->event));

	if(event->event == NWY_SNTP_SUCCESS)
	{
		snprintf(resp,sizeof(resp),"date:%d/%d/%d/ %d:%d:%d",event->update_time.year,event->update_time.mon,event->update_time.day,event->update_time.hour,event->update_time.min,event->update_time.sec);
		nwy_ext_echo("\r\n%s",resp);
		nwy_date_set(&event->update_time, 8);
	}
	return;
}

//--------------------------------------------------
//��������:NTP �����ʱ
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void api_UpdateForInte(void)
{
	nwy_error_e ret;
	char timezone[] = "E8";
	nwy_sntp_param_t sntp_param;
	memset(&sntp_param, 0, sizeof(nwy_sntp_param_t));
	sntp_param.cid = 1;
	strncpy(sntp_param.url, timeip, strlen(timeip));
	sntp_param.timeout = 1;
	sntp_param.retry_times = 2;
	strcpy(sntp_param.tz,timezone);

	ret = nwy_sntp_get_time(&sntp_param, nwy_cli_sntp_result_cb);
	if(ret == NWY_SUCCESS)
		nwy_ext_echo("\r\nnwy_sntp_get_time success\r\n");
	else
	nwy_ext_echo("\r\nnwy_sntp_get_time fail %d\r\n", ret);
}
//-----------------------------------------------
//��������: �������ʱ�������
//
//����:
//			t[in]			����ʱ���ʱ���ʽ��ֵ������Hex��
//
//����ֵ:  	���ʱ�������
//			0xffffffff  ʱ��Ƿ�
//��ע:	 ʱ�䶼��Hex��
//-----------------------------------------------
QWORD api_CalcInTimeRelativeSec(TRealTimer *t)
{
	QWORD TotalTime;
	WORD tyear, tmon, tday, i;

	if (api_CheckClock(t) == FALSE)
	{
		return ILLEGAL_VALUE_8F;
	}

	//�����꺬������
	tyear = t->wYear % 100;
	if (tyear != 0)
	{
		TotalTime = (DWORD)365 * tyear + (tyear - 1) / 4 + 1;
	}
	else
	{
		TotalTime = 0;
	}

	//�����º�������
	tmon = t->Mon;
	tday = 0;
	for (i = 0; i < (tmon - 1); i++)
	{
		if ((tyear & 0x03) == 0) //����
		{
			tday += LeapYearDay[i];
		}
		else
		{
			tday += NormalYearDay[i];
		}
	}
	TotalTime += tday;

	//�����캬������
	tday = t->Day;
	tday--;
	TotalTime += tday;

	//��СʱΪ��λ
	TotalTime *= 24;
	TotalTime += t->Hour;

	//�Է���Ϊ��λ
	TotalTime *= 60;
	TotalTime += t->Min;

	//����Ϊ��λ
	TotalTime *= 60;
	TotalTime += t->Sec;

	return (TotalTime);
}

//-----------------------------------------------
//��������: �ж�ʱ����Ч�Եĺ���
//
//����:
//			p[in]				TRealTimer�ṹ�ľ���ʱ�䣬ֵ������Hex��
//
//����ֵ:  	FALSE--���ݷǷ� TRUE--���ݺϷ�
//��ע:	 ʱ�䶼��Hex��
//-----------------------------------------------
BOOL api_CheckClock(TRealTimer *p)
{
	TRealTimer t;

	memcpy((BYTE *)&t, (BYTE *)p, sizeof(TRealTimer));

	//����¡������ݺϷ��ԣ����� byYear, byMon, byDay ���� RealTimer ������һ����Hex��
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
//��������: �����Ӧ��ݵ��¡������ݺϷ��ԣ���������� RealTimer ������һ����Hex��
//
//����:
//			wYear[in]			�ֵ꣬������Hex��
//			byMon[in]			�£�ֵ������Hex��
//			byDay[in]			�գ�ֵ������Hex��
//
//����ֵ:  	FALSE--���ݷǷ� TRUE--���ݺϷ�
//��ע:	 ʱ�䶼��Hex��
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

	if ((wYear & 0x03) == 0) //������
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
