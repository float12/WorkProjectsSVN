//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.20
//����		��ѹ�ϸ��ʴ���
//��ע��δ��ȫ���¼�event.c���룻��������event.c���룻���뵽sampleģ����
//----------------------------------------------------------------------

#include "AllHead.h"

// ��ѹ�ϸ��ʴ���
#if( SEL_STAT_V_RUN == YES )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
__no_init DWORD ClrVRunDayTimer;	// ���յ�ѹ�ϸ��ʶ�ʱ������
__no_init DWORD ClrVRunMonTimer;	// ���µ�ѹ�ϸ��ʶ�ʱ������

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//�ܡ�A��B��C
static TStatVRunData DayStatVRunData[4];//��ѹ�պϸ���ͳ��
static TStatVRunData MonStatVRunData[4];//��ѹ�ºϸ���ͳ��

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//����ϸ��ʰٷ���
//Type			0: ��	1����
//Phase:		0����  	1��A	2��B	3��C
static void CalcVolTime( BYTE Type, BYTE Phase )
{
	DWORD tdw;
	TStatVRunData *p;
	
	if( Phase > 3 )
	{
		return;
	}
	
	if( Type == 0 )
	{
		p = &DayStatVRunData[Phase];
	}
	else
	{
		p = &MonStatVRunData[Phase];
	}
	
	if( p->dwMonitorTime == 0 )
	{
		p->wPassRate = 10000;//100%
		p->wOverRate = 0;
	}
	else
	{
		tdw = p->dwOverTime + p->dwBelowTime;
		p->wOverRate = tdw * 10000 / p->dwMonitorTime;
		if( p->wOverRate > 10000 )
		{
			p->wOverRate = 10000;
		}
		p->wPassRate = 10000 - p->wOverRate;
	}
}


//�ϵ����eeprom�д洢������
void PowerUpVRunTime( void )
{
	DWORD dwAddr;
	if( (ClrVRunDayTimer <= 3) && (ClrVRunDayTimer > 0) )//���յ�ѹ�ϸ���
	{
		api_ClearVRunTimeNow( BIT0 );
	}
	
	if( (ClrVRunMonTimer <= 3) && (ClrVRunMonTimer > 0) )//���µ�ѹ�ϸ���
	{
		api_ClearVRunTimeNow( BIT1 );
	}

	ClrVRunDayTimer = 0;
	ClrVRunMonTimer = 0;
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.StatVRunDataRom.DayStatVRunData[0] );
	api_ReadFromContinueEEPRom( dwAddr, sizeof(DayStatVRunData), (BYTE *)&DayStatVRunData[0] );
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.StatVRunDataRom.MonStatVRunData[0] );
	api_ReadFromContinueEEPRom( dwAddr, sizeof(MonStatVRunData), (BYTE *)&MonStatVRunData[0] );
}


// �жϵ�ѹ����״̬
// ����״̬ wStatus
// D0 - 1  A�೬�ϸ�����
// D1 - 1  B�೬�ϸ�����
// D2 - 1  C�೬�ϸ�����
// D3 - 0 �ݲ��ã���0��ԭ�� D3 - 1  ���೬����
// D4 - 1  A�೬�ϸ�����
// D5 - 1  B�೬�ϸ�����
// D6 - 1  C�೬�ϸ�����
// D7 - 0 �ݲ��ã���0��ԭ�� D7 - 1  ���೬����
// D8 - 1  A�ڿ��˷�Χ����
// D9 - 1  B�ڿ��˷�Χ����
// D10 - 1  C�ڿ��˷�Χ����
static WORD GetVolRunStatus(void)
{
	BYTE i;
	WORD wStatus,VolValue;

	wStatus = 0;
	for(i=0; i<3; i++)
	{
		if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
		{
			continue;
		}

		// ����1λС��
		api_GetRemoteData( PHASE_A+0x1000*i+REMOTE_U, DATA_HEX, 1, sizeof(WORD), (BYTE*)&VolValue );

		if( ( VolValue >= gEventPara.wLimit[eSTATISTIC_V_PASS_RATE_NO] )||( VolValue <= gEventPara.VRunLimitLV ) )
		{
			continue;//��������ѳ����˷�Χ���򲻽��к���ĳ����ޡ��������жϣ�������һ���ж�
		}

		// D8 - 1  A�ڿ��˷�Χ����
		// D9 - 1  B�ڿ��˷�Χ����
		// D10 - 1  C�ڿ��˷�Χ����
		wStatus |= ( 0x100 << i );

		// ��ѹ�ϸ��ʳ�����
		if( VolValue > gEventPara.VRunRecoverHV )
		{
			wStatus |= (0x10<<i);
		}

		// ��ѹ�ϸ��ʳ�����
		if( VolValue < gEventPara.VRunRecoverLV )
		{
			wStatus |= (0x01<<i);
		}
	}

	return wStatus;
}
//--------------------------------------------------
//��������:  �����յ�ѹ�ϸ���
//         
//����:     ��
//         
//����ֵ:    �� 
//         
//��ע����:  ��
//--------------------------------------------------
void DayStatVRunTime(void)
{
	WORD i,wStatus;
	
	wStatus = GetVolRunStatus();
	// ���೬����ʱ�䡢������ʱ��
	for(i=0; i<MAX_PHASE; i++)
	{
		if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
		{
			continue;
		}
		
		if( (DayStatVRunData[i+1].dwMonitorTime > 2880)//��ѹ���ʱ�䡢������ʱ�䡢������ʱ�䲻�ܳ���2880
		|| (DayStatVRunData[i+1].dwBelowTime > 2880)
		|| (DayStatVRunData[i+1].dwOverTime > 2880) )
		{
			memset( (BYTE*)&DayStatVRunData[i+1], 0x00, sizeof(TStatVRunData) );
		}
		else if( (DayStatVRunData[i+1].dwMonitorTime >= 1440)//��ѹ���ʱ�䡢������ʱ�䡢������ʱ�䲻�ܳ���1440
			  ||(DayStatVRunData[i+1].dwBelowTime >= 1440)
			  ||(DayStatVRunData[i+1].dwOverTime >= 1440) )
		{
			continue;//�������ۼ�
		}
		else//�����ۼ�
		{

		}

		// D8 - 1  A�ڿ��˷�Χ����
		// D9 - 1  B�ڿ��˷�Χ����
		// D10 - 1  C�ڿ��˷�Χ����
		if( wStatus & ( 0x100 << i ) )
		{
			// ��ѹ���ʱ��
			DayStatVRunData[i + 1].dwMonitorTime++;
		}

		if(wStatus & (0x01 << i))
		{
			// ��ѹ������ʱ��
			DayStatVRunData[i + 1].dwBelowTime++;
		}
		if(wStatus & (0x01 << (i + 4)))
		{
			// ��ѹ������ʱ��
			DayStatVRunData[i + 1].dwOverTime++;
		}
	}
	
	//�������ϸ��ʰٷ���
	for(i=0; i<MAX_PHASE; i++)
	{
		CalcVolTime( 0, 1+i );
	}
	
	//�ܵ�ѹ�ϸ���ͳ��
	if( 	((MeterTypesConst==METER_3P3W) && ((wStatus&0x500)==0x500)) 
		|| 	((MeterTypesConst!=METER_3P3W) && ((wStatus&0x700)==0x700)) )
	{
		// ��ѹ���ʱ�� ��һ���ڿ��˷�Χ���ڣ��ܵļ��ʱ���++ ��ǰ������ 2010-12-14
		// ��ѹ���ʱ��(����)Ҫ�����ѹ���ڵ�ѹ�����ϡ������ڲ�ͳ��ʱ��++ ����Ҫ�� 2010-12-14
		DayStatVRunData[0].dwMonitorTime++;

		// �ܳ�����ʱ��
		// D0 - 1  A�೬����
		// D1 - 1  B�೬����
		// D2 - 1  C�೬����
		// D3 - 0 �ݲ��ã���0
		if(wStatus & 0x07)
		{
			DayStatVRunData[0].dwBelowTime++;
		}
		// �ܳ�����ʱ��
		// D4 - 1  A�೬����
		// D5 - 1  B�೬����
		// D6 - 1  C�೬����
		// D7 - 0 �ݲ��ã���0
		if(wStatus & 0x70)
		{
			DayStatVRunData[0].dwOverTime++;
		}

		//�����ܵ�ѹ�ϸ��ʰٷ���
		CalcVolTime( 0, 0 );
	}

}

//--------------------------------------------------
//��������:  �����յ�ѹ�ϸ���
//         
//����:     ��
//         
//����ֵ:    �� 
//         
//��ע����:  ��
//--------------------------------------------------
void MonStatVRunTime(void)
{
	WORD i,wStatus;
	
	wStatus = GetVolRunStatus();
	
	// ���೬����ʱ�䡢������ʱ��
	for(i=0; i<MAX_PHASE; i++)
	{
		if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
		{
			continue;
		}
		
		if( (MonStatVRunData[i+1].dwMonitorTime > (2880*31))//��ѹ���ʱ�䡢������ʱ�䡢������ʱ�䲻�ܳ���2880
		|| (MonStatVRunData[i+1].dwBelowTime > (2880*31))
		|| (MonStatVRunData[i+1].dwOverTime > (2880*31)) )
		{
			memset( (BYTE*)&MonStatVRunData[i+1], 0x00, sizeof(TStatVRunData) );
		}
		else if( (MonStatVRunData[i+1].dwMonitorTime >= (1440*31))//��ѹ���ʱ�䡢������ʱ�䡢������ʱ�䲻�ܳ���1440
			  ||(MonStatVRunData[i+1].dwBelowTime >= (1440*31))
			  ||(MonStatVRunData[i+1].dwOverTime >= (1440*31)) )
		{
			continue;//�������ۼ�
		}
		else//�����ۼ�
		{

		}
		 
		// D8 - 1  A�ڿ��˷�Χ����
		// D9 - 1  B�ڿ��˷�Χ����
		// D10 - 1	C�ڿ��˷�Χ����
		if( wStatus & ( 0x100 << i ) )
		{
			// ��ѹ���ʱ��
			MonStatVRunData[i + 1].dwMonitorTime++;
		}

		if(wStatus & (0x01 << i))
		{
			// ��ѹ������ʱ��
			MonStatVRunData[i + 1].dwBelowTime++;
		}
		if(wStatus & (0x01 << (i + 4)))
		{
			// ��ѹ������ʱ��
			MonStatVRunData[i + 1].dwOverTime++;
		}
	}
	
	//�������ϸ��ʰٷ���
	for(i=0; i<MAX_PHASE; i++)
	{
		CalcVolTime( 1, 1+i );
	}
	
	//�ܵ�ѹ�ϸ���ͳ��
	if( 	((MeterTypesConst==METER_3P3W) && ((wStatus&0x500)==0x500)) 
		||	((MeterTypesConst!=METER_3P3W) && ((wStatus&0x700)==0x700)) )
	{		
		// ��ѹ���ʱ�� ��һ���ڿ��˷�Χ���ڣ��ܵļ��ʱ���++ ��ǰ������ 2010-12-14
		// ��ѹ���ʱ��(����)Ҫ�����ѹ���ڵ�ѹ�����ϡ������ڲ�ͳ��ʱ��++ ����Ҫ�� 2010-12-14
		MonStatVRunData[0].dwMonitorTime++;

		// �ܳ�����ʱ��
		// D0 - 1  A�೬����
		// D1 - 1  B�೬����
		// D2 - 1  C�೬����
		// D3 - 0 �ݲ��ã���0
		if(wStatus & 0x07)
		{
			MonStatVRunData[0].dwBelowTime++;
		}
		// �ܳ�����ʱ��
		// D4 - 1  A�೬����
		// D5 - 1  B�೬����
		// D6 - 1  C�೬����
		// D7 - 0 �ݲ��ã���0
		if(wStatus & 0x70)
		{
			MonStatVRunData[0].dwOverTime++;
		}

		//�����ܵ�ѹ�ϸ��ʰٷ���
		CalcVolTime( 1, 0 );
	}

}

// ͳ�Ƶ�ѹ�ϸ���ʱ�� ���ӵ���
void StatVRunTime(void)
{
	DayStatVRunTime( );
	MonStatVRunTime( );
}


// ����ѹ�ϸ�������
// Phase -- Ҫ��ȡ���� 0-�ܡ�1-A��2-B��3-C
// No ----- BIT0: ������  BIT1��������
// pBfu --- �洢����������
// ���� --- ʵ�ʶ��������ݳ���, ������岻��������0
BYTE ReadVRunTime(BYTE Phase, BYTE No, BYTE *pBuf)
{
	BYTE Length;

	Length = 0;
	
	if( (No&BIT0) != 0 )
	{
		memcpy( pBuf, &DayStatVRunData[Phase], sizeof(TStatVRunData) );
		Length += sizeof(TStatVRunData);
	}
	
	if( (No&BIT1) != 0 )
	{
		memcpy( pBuf+Length, &MonStatVRunData[Phase], sizeof(TStatVRunData) );
		Length += sizeof(TStatVRunData);
	}
	
	return Length;
}


// �����ǰ����
//Type 	BIT0��������		BIT1��������
void api_ClearVRunTimeNow( BYTE Type )
{
	WORD i;
	
	if( (Type&BIT0) != 0 )
	{
		memset( (void*)&DayStatVRunData[0], 0x00, sizeof(DayStatVRunData) );
		//�������ϸ��ʰٷ���
		for(i=0; i<(MAX_PHASE+1); i++)
		{
			CalcVolTime( 0, i );
		}
	}
	
	if( (Type&BIT1) != 0 )
	{
		memset( (void*)&MonStatVRunData[0], 0x00, sizeof(DayStatVRunData) );
		//�������ϸ��ʰٷ���
		for(i=0; i<(MAX_PHASE+1); i++)
		{
			CalcVolTime( 1, i );
		}
	}

	SwapVRunTime();//����һ��
}


// ��ѹ�ϸ�������ת��
void SwapVRunTime(void)
{
	DWORD dwAddr;
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.StatVRunDataRom.DayStatVRunData[0] );
	api_WriteToContinueEEPRom( dwAddr, sizeof(DayStatVRunData), (BYTE *)&DayStatVRunData[0] );
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.StatVRunDataRom.MonStatVRunData[0] );
	api_WriteToContinueEEPRom( dwAddr, sizeof(MonStatVRunData), (BYTE *)&MonStatVRunData[0] );
}


void FactoryInitStatisticsVPara( void )
{
	//û��OAD����ĳ�ʼ��
}

#endif//#if( SEL_STAT_V_RUN == YES )

