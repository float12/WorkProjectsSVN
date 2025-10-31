//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.9
//����		ʱ���ͨ�ú����������������ʱ��оƬ
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//ʵʱʱ�ӣ�BCD�뷽ʽ��ʱ��
TRealTimer RealTimer;
BYTE g_PowerOnSecTimer;//�ϵ�30�뵹��ʱ �����ϵ���ʱ�Ĳ��� ���øļ����� ֻ���ϵ����30�룬����ʱ������ת��
//ϵͳȫ�ֱ��� ����е�ǰʱ����2000��1��1��0ʱ0��֮���ʱ����λ������ (�ú���api_CalcInTimeRelativeMin()����ֵ)��
DWORD g_RelativeMin;

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//                            1   2   3   4   5   6   7   8   9   10  11  12
const BYTE NormalYearDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//                      	1   2   3   4   5   6   7   8   9   10  11  12
const BYTE LeapYearDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
BOOL api_CheckMonDay( WORD wYear, BYTE byMon, BYTE byDay );
extern void InitRtc( void );
extern BOOL ReadOutClockHara( WORD Type );
extern BOOL WriteOutClock(TRealTimer * t);

//-----------------------------------------------
//				��������
//-----------------------------------------------

//  ˢ��ϵͳȫ�ֱ��� ����е�ǰʱ����2000��1��1��0ʱ0��֮���ʱ����λ������
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

	// �������־ 
	//SecFlag = TRUE;

	// ���1 
	RealTimer.Sec++;
	
	// �ж������ 
	if( RealTimer.Sec >= 60 )
	{
		// �������� 
		RealTimer.Sec = 0;
		
		// �ּ�һ 
		RealTimer.Min++;
		
		// �жϷ���� 
		if( RealTimer.Min >= 60 )
		{
			// ����ּ��� 
			RealTimer.Min = 0;
			
			// Сʱ��1 
			RealTimer.Hour++;
			
			// �ж�Сʱ��� 
			if( RealTimer.Hour >= 24 )
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
				
				// �ж��·��Ƿ� 
				if( RealTimer.Day > Buf[RealTimer.Mon - 1] )
				{
					// ��ʼ���� 
					RealTimer.Day = 1;
						
					// �¼�1 
					RealTimer.Mon++;
						
					// ���Ƿ���� 
					if( RealTimer.Mon > 12 )
					{
						// �³�ʼ�� 
						RealTimer.Mon = 1;
						
						// ���1 
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

//���Զ�ʱ��������1���Ӷ�ʱ���ڴ�ѭ���е���
void api_RtcTask(void)
{
	TRealTimer TmpRealTimer;

	//500�����һ��ʱ��
	if( api_GetTaskFlag( eTASK_RTC_ID, eFLAG_500_MS ) == TRUE )
	{
		api_ClrTaskFlag( eTASK_RTC_ID, eFLAG_500_MS );

		memcpy( (BYTE*)&TmpRealTimer, (BYTE*)&RealTimer, sizeof(TRealTimer) );
		api_ReadMeterTime( 0 );
		if( TmpRealTimer.Sec != RealTimer.Sec )
		{
			//��仯
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_SECOND );
		}
		if( TmpRealTimer.Min != RealTimer.Min )
		{
			//���ӱ仯
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_MINUTE );
		}
		if( TmpRealTimer.Hour != RealTimer.Hour )
		{
			//Сʱ�仯
			api_SetTaskFlag( eTASK_RTC_ID, eFLAG_HOUR );
		}
	}
	//1�뵽
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_SECOND) == TRUE )
	{
		api_SetAllTaskFlag(eFLAG_SECOND); // ������ȷ����ǰ�������һ����ִ��
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_SECOND);

		if( g_PowerOnSecTimer != 0x00 )
		{
			g_PowerOnSecTimer--;
		}

		//û��5���������´���
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			return;
		}
		
		FunctionConst(MAIN_TIMER_SEC_TASK_END);
	}

	//1���ӵ�
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_MINUTE) == TRUE )
	{
		api_SetAllTaskFlag( eFLAG_MINUTE );
		//���־
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_MINUTE);

		//ˢ��ϵͳȫ�ֱ��� ����е�ǰʱ����2000��1��1��0ʱ0��֮���ʱ����λ������
		RefreshGlobalVariableMinNow();

		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			return;
		}
	}
	//hour change
	if( api_GetTaskFlag(eTASK_RTC_ID,eFLAG_HOUR) == TRUE )
	{
		api_SetAllTaskFlag( eFLAG_HOUR );//������ȷ����ǰ�������һ����ִ��
		api_ClrTaskFlag(eTASK_RTC_ID,eFLAG_HOUR);
	}
}


//-----------------------------------------------
//��������: RTC�����ϵ��ʼ��
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_PowerUpRtc(void)
{
	BOOL		Result;
	DWORD		PowerDownRelativeTime;
	TRealTimer	PowerDownTime;

	g_PowerOnSecTimer = POWER_UP_TIMER;
	
	InitRtc();  //�ú������и�λRTC���� �����������λ��

	Result = api_ReadMeterTime( 0 );
	RefreshGlobalVariableMinNow();
	
	api_GetPowerDownTime(&PowerDownTime);
	PowerDownRelativeTime = api_CalcInTimeRelativeMin(&PowerDownTime);
	if( (PowerDownRelativeTime == ILLEGAL_VALUE_8F) && (Result == TRUE) )//RTCʱ��δ���ҵ���ʱ�䲻�Ե�����²�����ʱ�䴦��
	{	
		return;
	}

	//ʱ�Ӵ���	�ϵ�ʱ��С�ڵ���ʱ��	�ϵ�ʱ����ڣ�����ʱ��+1000�죩
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
		
		g_RelativeMin = 0xFFFFFFFF;//Ϊ��ǿ�Ƹ��¶����־�����������Զ������к��Ƿ���Ҫ���ˣ�Ӱ��RepairFreezeUseFlash�������޸���ע�⣻
		api_ReadPara1FromEE();//����E2������Ram
		api_PowerUpEnergy();
		api_WriteMeterTime(&PowerDownTime);
		//��RTC�ϵ�ʱ�����
		api_SetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG);//��дʱ�����ñ�־�����ɵߵ�����ֹ���ñ�־���壻
	}
}


//-----------------------------------------------
//��������: ��ȡʱ�䣬���øú���������ⲿʱ�ӣ�ͬʱˢ�±�ʾʱ���������ҪƵ������
//
//����: 
//			Type[in]			0������rtcģ��ʱ�ӣ�ˢ��RealTimer���� 1��ֻ����
//                    
//����ֵ:  	TRUE:��ȷ��ȡ       FALSE:û����ȷ��ȡ
//��ע:     
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
//��������: ��ȡʱ�䣬���øú���������ⲿʱ�ӣ������¼�̻���־
//
//����: 
//			Type[in]			0������rtcģ��ʱ�ӣ�ˢ��RealTimer���� 1��ֻ����
//                    
//����ֵ:  	TRUE:��ȷ��ȡ       FALSE:û����ȷ��ȡ
//��ע:     
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
//��������: ��TRealTimer�ṹ��ʱ��д��RTCģ���У����������ڼ���
//
//����: 
//			t[in]				Ҫд���ʱ��
//                    
//����ֵ:  	TRUE:��ȷ��ȡ       FALSE:û����ȷ��ȡ
//��ע:     
//-----------------------------------------------
BOOL api_WriteMeterTime(TRealTimer * t)
{
	DWORD OldRelativeMin;
	
	//�ⲿʱ�Ӷ���
	if( WriteOutClock(t) == FALSE )
	{
		return FALSE;
	}
	//Уʱ�ɹ��壬RTC�ϵ�ʱ������־
	api_ClrRunHardFlag(eRUN_HARD_ERR_RTC_FLAG);
	api_ClrRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG);
	//������ڸı䣬��㲥Уʱ��־
	if( api_GetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG ) == TRUE )
	{
		//����Ѿ����й��㲥Уʱ
		if( (t->Day != RealTimer.Day) || (t->Mon != RealTimer.Mon) || (t->wYear != RealTimer.wYear) )
		{
			api_ClrRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
		}
	}
	//������ڸı䣬�����Ĺ㲥Уʱ���ϱ�־
	if( api_GetRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG ) == TRUE )
	{
		//����Ѿ����й��㲥Уʱ
		if( (t->Day != RealTimer.Day) || (t->Mon != RealTimer.Mon) || (t->wYear != RealTimer.wYear) )
		{
			api_ClrRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );
		}
	}
	
	//��ֹʱ��
	DISABLE_CLOCK_INT;
	//д��
	memcpy((BYTE *)&RealTimer, (BYTE *)t, sizeof(TRealTimer));
	//��ʱ��
	ENABLE_CLOCK_INT;
	
	OldRelativeMin = g_RelativeMin;

	//ˢ��ϵͳȫ�ֱ��� ����е�ǰʱ����2000��1��1��0ʱ0��֮���ʱ����λ������
	RefreshGlobalVariableMinNow();

	//�������
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
//��������: ������ڲ�����ȡʱ��
//
//����: 
//			Type[in]				��Ҫ��ȡ��ʱ���ʽ
//									bit15: 0������������ʱ���� ����buf[0]������С��ģʽ   1�����ʱ���������� ����С��ģʽ
//									bit14: 0: Hex��  	1:BCD��
//									bit13-bit8:����
//									bit7:���Ƿ��2000  1����2000
//									bit6-bit0:����������ʱ����  1:��ʾ��Ҫ��λ��Ӧ������
//			pBuf[out]				��Ŷ���������
//                    
//����ֵ:  	���ض�ȡ�����ݳ���
//��ע:     ʱ�䶼��Hex�룬�궼��word����
//#define DATETIME_SECOND_FIRST			BIT15//����buf[0]
//#define DATETIME_BCD					BIT14//����������bcd
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
	
	if( Type&0x4000 )//Ҫ��BCD���ʽ
	{
		for(i=0; i<Length; i++)
		{
			pBuf[i] = lib_BBinToBCD( pBuf[i] );
		}
	}
	
	if( (Type & 0xc0) == 0xc0 )//�������Ƿ��2000���������Ҫ�������Ҳѡ��
	{
		if( (Type&0x4000) == 0x0000 )//Hex��
		{
			Length -= 1;//���ⳤ�Ȳ���Ϊ0��
			wTmp = RealTimer.wYear;
			memcpy( &pBuf[Length], &wTmp, sizeof(WORD) );
			Length += 2;
		}
		else//Ҫ��BCD���ʽ WORD����ͬ�ⰴС��ģʽ����
		{
			pBuf[Length] = 0x20;
			Length ++;
		}
	}
	
	if( (Type&0x8000) == 0x0000 )//���Ҫ�� ����������ʱ���� ����buf[0] ˳��
	{
		lib_InverseData( pBuf, Length );
		if( (Type&0xc0) == 0xc0 )//�������Ƿ��2000���������Ҫ�������Ҳѡ��
		{
			lib_InverseData( pBuf, 2 );
		}
	}
	
	return Length;
}


//-----------------------------------------------
//��������: ���ʱ��ת��Ϊ����ʱ�䣬���ʱ��ָ�Է���Ϊ��λ����2000��1��1��0��0��Ϊ��㣬100������Ч
//
//����: 
//			Min[in]				���ʱ��ķ�����
//			p[out]				���ؾ���ʱ�䣨������ʱ�����ʽ��
//                    
//����ֵ:  	TRUE:��ȷת��       FALSE:û����ȷת��
//��ע:     ʱ�䶼��Hex��
//-----------------------------------------------
BOOL api_ToAbsTime(DWORD Min, TRealTimer *p)
{
	WORD i;
	DWORD Temp, Temp1, Temp2;
	BYTE Buf[sizeof(NormalYearDay)];

	//��ȷ���ж�
	if( p == NULL )
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

	//ʣ������
	Temp = Temp - Temp2;

	//������
	Temp1 = 0;

	// �ж��Ƿ�Ϊ����
	if( (p->wYear & 0x0003) == 0 )// p->Year ����BCD�������� if( IsLeapYear(p->Year) == TRUE )
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

	//ʣ����
	Temp = Temp - Temp2;

	//������
	p->Day = (BYTE)(Temp + 1);
	
	p->wYear += 2000;//�����2000

	return TRUE;
}

//-----------------------------------------------
//��������: ��698ʱ���ʽת��Ϊ645��ʽ
//
//����: 
//			pTime698[in]				698ʱ���ʽ HEX ��Ϊ���ֽڣ�����ǰ
//			pTime645[out]				645ʱ���ʽ BCD ��Ϊһ�ֽڣ�����ǰ
//                    
//����ֵ:  	��
//��ע:     
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
//��������: ��645ʱ���ʽת��Ϊ698��ʽ
//
//����: 
//			pTime645[in]				645ʱ���ʽ BCD ��Ϊһ�ֽڣ�����ǰ
//			pTime698[out]				698ʱ���ʽ HEX ��Ϊ���ֽڣ�����ǰ
//                    
//����ֵ:  	��
//��ע:     
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
//��������: ���ʱ��ת��Ϊ����ʱ�䣨645��ʽ�������ʱ��ָ�Է���Ϊ��λ����00��1��1��0��0��Ϊ��㣬100������Ч
//
//����: 
//			Min[in]				���ʱ��ķ�����
//			p[out]				���ؾ���ʱ�䣨��ʱ�������ʽ��
//                    
//����ֵ:  	TRUE:��ȷת��       FALSE:û����ȷת��
//��ע:     ʱ�䶼��BCD�룬���ں�Ϊһ���ֽ�
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
//��������: �������ʱ��ķ���������2000��1��1��0��0��Ϊ���
//
//����: 
//			t[in]			����ʱ���ʱ���ʽ��ֵ������Hex��
//                    
//����ֵ:  	���ʱ��ֵ
//			0xffffffff  ʱ��Ƿ�
//��ע:     ʱ�䶼��Hex��
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
//��������:  ��645��������ʱ��ת���� TRealTiemr
//         
//����:    pBuf[in]:645��������ʱ��,����˳�򣺷� ʱ �� �� ��
//         
//		   pRealTimer[out]:ת����õ��� TRealTimer		
//
//����ֵ:   �õ���DWORD 
//         
//��ע����:  ��
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
	else if( lib_IsAllAssignNum( pBuf, 99, 5 ) == TRUE )//ǰ��lib_BBCDToBin��0x99ת������99.�忨�����л�����Ϊ9999999999
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
//��������: ������������ʱ��֮��ķ�����
//
//����: 
//			tNew[in]			����ʱ���ʱ���ʽ��ֵ������Hex��
//			tOld[in]			����ʱ���ʱ���ʽ��ֵ������Hex��
//                    
//����ֵ:  	��������ʱ��֮��ķ����� tNew - tOld
//			0xffffffff: ʱ���в��Ϸ������
//			0:          tNew < tOld
//��ע:     ʱ�䶼��Hex��
//-----------------------------------------------
DWORD api_CalcIntervalMinute(TRealTimer * tNew, TRealTimer * tOld)
{
	DWORD MinNew,MinOld;

	//������Է�����Ŀ
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
//��������: �������ʱ�������
//
//����: 
//			t[in]			����ʱ���ʱ���ʽ��ֵ������Hex��
//                    
//����ֵ:  	���ʱ�������
//			0xffffffff  ʱ��Ƿ�
//��ע:     ʱ�䶼��Hex��
//-----------------------------------------------
DWORD api_CalcInTimeRelativeSec(TRealTimer * t)
{
	DWORD TotalTime;
	WORD tyear, tmon, tday, i;

	if( api_CheckClock( t ) == FALSE )
	{
		return ILLEGAL_VALUE_8F;
	}

	//�����꺬������
	tyear = t->wYear%100;
	if(tyear != 0)
	{
		TotalTime = (DWORD)365*tyear + (tyear-1)/4 + 1;
	}
	else
	{
		TotalTime = 0;
	}
	
	//�����º�������
	tmon = t->Mon;
	tday = 0;
	for(i=0; i<(tmon-1); i++)
	{
		if( (tyear&0x03) == 0 )//����
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
	
	return(TotalTime);
}


//-----------------------------------------------
//��������: �ж�ʱ����Ч�Եĺ���
//
//����: 
//			p[in]				TRealTimer�ṹ�ľ���ʱ�䣬ֵ������Hex��
//                    
//����ֵ:  	FALSE--���ݷǷ� TRUE--���ݺϷ�
//��ע:     ʱ�䶼��Hex��
//-----------------------------------------------
BOOL api_CheckClock(TRealTimer * p)
{
	TRealTimer t;

	DISABLE_CLOCK_INT;
	memcpy((BYTE *)&t, (BYTE *)p, sizeof(TRealTimer));
	ENABLE_CLOCK_INT;

	//����¡������ݺϷ��ԣ����� byYear, byMon, byDay ���� RealTimer ������һ����Hex��
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
//��������: �ж�ʱ����Ч�Եĺ�������
//
//����: 
//			p[in]				TRealTimer�ṹ�ľ���ʱ�䣬ֵ������Hex��
//                    
//����ֵ:  	FALSE--���ݷǷ� TRUE--���ݺϷ�
//��ע:     ʱ�䶼��Hex��
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
	//�ж�ʱ����Ч�Եĺ���
	if( api_CheckClock( &t ) == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}
//--------------------------------------------------
//��������:  645 ʱ���ʽ���ж��Լ��뵱ǰʱ��ıȽ�
//         
//����:      BYTE* ProBuf[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
BOOL api_JudgeClock645( BYTE* ProBuf)
{
	TRealTimer TmpRealTimer;
	BOOL Result;
	DWORD Time,Time1;
	
	TmpRealTimer.wYear = 2000+lib_BBCDToBin(ProBuf[0]);//��
	TmpRealTimer.Mon = lib_BBCDToBin(ProBuf[1]);//��
	TmpRealTimer.Day = lib_BBCDToBin(ProBuf[2]);//��
	TmpRealTimer.Hour = lib_BBCDToBin(ProBuf[3]);//ʱ
	TmpRealTimer.Min = lib_BBCDToBin(ProBuf[4]);//��
	TmpRealTimer.Sec = lib_BBCDToBin(ProBuf[5]);//��

	Result = api_CheckClock((TRealTimer*)&TmpRealTimer);//�жϸ�ʽ�Ƿ���ȷ
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
//��������: �����Ӧ��ݵ��¡������ݺϷ��ԣ���������� RealTimer ������һ����Hex��
//
//����: 
//			wYear[in]			�ֵ꣬������Hex��
//			byMon[in]			�£�ֵ������Hex��
//			byDay[in]			�գ�ֵ������Hex��
//                    
//����ֵ:  	FALSE--���ݷǷ� TRUE--���ݺϷ�
//��ע:     ʱ�䶼��Hex��
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

	if( (wYear&0x03) == 0 )//������
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
//��������: BCD��ʽ��ʱ��Ϸ����ж�
//
//����: 	
//			Start[in]  ��ʼ�жϵĵ�ַ
// 			Ck[in]     �����ʱ�� ������ ��ʽ���롢�֡�ʱ�����ڡ��ա��¡���
// 			Len[in]	   �Ƚϳ��� ���λΪ1��ʾû����
//                   
//����ֵ:  
//
//��ע:   
//---------------------------------------------------------------
BYTE api_CheckClockBCD( BYTE Start, BYTE Len, BYTE *Ck )//�����м�
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
			return FALSE;//�Ϸ��Լ�����
		}
		if( lib_IsBCD( Ck[i] ) == FALSE )
		{
			return FALSE;	//BCD�������
		}
	}
	return TRUE;//��ȷ
}

//-----------------------------------------------
//��������: ֻ��ȡʱ�䣬��ˢ��RealTimer
//
//����: 
//        	pBuf[in]		���ʱ��buf
//����ֵ:  	��
//
//��ע:  �������¼����ã�������͹����¸�λ���������¼��޷���¼ʱ������
//-----------------------------------------------
void api_FreeEventReadClockHara(BYTE *pBuf)
{
	//����RTCδʹ��
}

#if( PREPAY_MODE == PREPAY_LOCAL )
//---------------------------------------------------------------
//��������: �жϽ��ݽ����պϷ���
//
//����: 	Buf[in]  �����ʽ  [0]ʱ[1]��[2]��			
// 			                  
//����ֵ:  true ��������һ���Ϸ�������  false ���н����ն���Ч
//
//��ע:   
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
		if( ( Tmp[3*i+1] <= 28)&&(Tmp[3*i+1] > 0) //��
			&&(Tmp[3*i] <= 23) ) //ʱ
		{
			return TRUE;
		}
	}
	
	return FALSE;
}
#endif

