//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.16
//����		ϵͳ�����ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
DWORD	MainLoopCounter;		//��ѭ������
DWORD	MainLoopTimer;			//��ѭ������
DWORD	MainLoopSecCounter;		//��ѭ��ÿ��ѭ������
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//��ص�ѹƫ�ò���
__no_init TBatteryPara BatteryPara;

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static TSysWatchRamData SysWatchRamData;
static volatile WORD s_SystickRun;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

//--------------------------------------------------
//��������: У���ز���
//         
//����:     ��
//         
//����ֵ:    �� 
//         
//��ע����:  ��
//--------------------------------------------------
void CheckBatteryPara( void )
{
	WORD Result;
	
	//У����ƫ��ϵ��
	if(BatteryPara.CRC32 != lib_CheckCRC32((BYTE *)&BatteryPara, (sizeof(TBatteryPara) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SysWatchSafeRom.BatteryPara), sizeof(TBatteryPara), (BYTE*)&BatteryPara);
		if( Result != TRUE ) 
		{
			BatteryPara.ClockBatteryOffset = 20;//��֮ǰֵ����֮ǰ����һ��
			BatteryPara.ReadBatteryOffset = 20;
		}
	}
}

//--------------------------------------------------
//��������: У׼��ز���
//         
//����:     ��
//         
//����ֵ:    WORD True �ɹ��� False ʧ��
//         
//��ע����:  ��
//--------------------------------------------------
WORD api_AdjustBatteryVoltage( void )
{
	WORD Result,TmpReadBattery,TmpClockBattery;

	memset( &BatteryPara, 0x00, sizeof(TBatteryPara) );//�������

	#if( MAX_PHASE == 3 )//����У׼
	TmpReadBattery = (WORD)GetADValue( 0x55, SYS_READ_VBAT_AD );
	if( abs(ReadBatteryStandardVoltage - TmpReadBattery) < (BYTE)(ReadBatteryStandardVoltage*15/100) )//��ѹ��ֵС�������ѹ��ֵ ��ѹ��ֵ=��׼��ѹ*0.15
	{
		BatteryPara.ReadBatteryOffset = (signed char)(ReadBatteryStandardVoltage-TmpReadBattery);
	}
	else
	{	
		//У׼ʧ�ܣ��ָ�ԭֵ
		CheckBatteryPara( );
		return FALSE;
	}
	#endif

	TmpClockBattery = (WORD)GetADValue( 0x55, SYS_CLOCK_VBAT_AD );
	if( abs(ClockBatteryStandardVoltage -TmpClockBattery) < (BYTE)(ClockBatteryStandardVoltage*15/100) )//��ѹ��ֵС�������ѹ��ֵ ��ѹ��ֵ=��׼��ѹ*0.15
	{
		BatteryPara.ClockBatteryOffset = (signed char)(ClockBatteryStandardVoltage-TmpClockBattery);
	}
	else
	{
		//У׼ʧ�ܣ��ָ�ԭֵ
		CheckBatteryPara( );
		return FALSE;
	}
	
	//У׼����дE2
	Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SysWatchSafeRom.BatteryPara), sizeof(TBatteryPara), (BYTE*)&BatteryPara);

	return Result;
}

//ˢ��ͣ�糭�����ֽ�
void FreshReadBatt( void )
{
	BYTE i;
	WORD Result;

	//ʱ�ӵ��״̬
	for(i=0; i<5; i++)
	{
		Result = (WORD)GetADValue( 0x55, SYS_READ_VBAT_AD );
		if( Result > LOW_BATT_READ )
		{
			break;
		}
		api_Delayms( 1 );
	}
	
	SysWatchRamData.ReadBattery = Result;
	
	if( Result < LOW_BATT_READ )
	{
		if( api_GetRunHardFlag(eRUN_HARD_READ_BAT_LOW) == FALSE )
		{
			api_SetFollowReportStatus(eSTATUS_ReadMeterBatteryUnderVol);
		}
		api_SetRunHardFlag( eRUN_HARD_READ_BAT_LOW );
	}
	else if( Result > (LOW_BATT_READ+20) )
	{
		api_ClrRunHardFlag( eRUN_HARD_READ_BAT_LOW );
	}
}


//ˢ��ʱ�ӵ��״̬
void FreshClockBatt( void )
{
	BYTE i;
	WORD Result;

	//ʱ�ӵ��״̬
	for(i=0; i<5; i++)
	{
		Result = (WORD)GetADValue( 0x55, SYS_CLOCK_VBAT_AD );
		if( Result > LOW_BATT_CLOCK )
		{
			break;
		}
		api_Delayms( 1 );
	}
	
	SysWatchRamData.ClockBattery = Result;
	
	if( Result < LOW_BATT_CLOCK )
	{
		if( api_GetRunHardFlag(eRUN_HARD_CLOCK_BAT_LOW) == FALSE )
		{
			api_SetFollowReportStatus(eSTATUS_ClockBatteryUnderVol);
		}
		api_SetRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW );
	}
	else if( Result > (LOW_BATT_CLOCK+20) )
	{
		api_ClrRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW );
	}
}


//ˢ�±����¶�
void FreshTemperature( void )
{
	short Result;
	
	//�¶�״̬
	Result = GetADValue( 0x55, SYS_TEMPERATURE_AD );
	
	SysWatchRamData.Temperature = (short)Result;
}

//-----------------------------------------------
//��������: ϵͳ����ģ����ϵ��ʼ������
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_PowerUpSysWatch(void)
{
	WakeUpTimer = 0;
	CheckBatteryPara();
	
	FreshReadBatt();
	
	FreshClockBatt();
	
	FreshTemperature();
}
//---------------------------------------------------------------
//��������: systick�����и�λоƬ
//
//����: ��
//                   
//����ֵ:  ��
//
//��ע:   
//---------------------------------------------------------------
void SystickRun( void )
{
	s_SystickRun++;
	if(s_SystickRun > 50000)
	{
		api_WriteSysUNMsg( SYSERR_SYSTICK_ERR );
		Reset_CPU();		
	}
}


//-----------------------------------------------
//��������: ϵͳ����ģ������
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SysWatchTask(void)
{
	#if(SEL_DEBUG_VERSION == YES)
	WORD i;
	#endif
	BYTE tSec;
	
	//ֻ��ϵͳ�����ϵ���ϵͳ�Լ� �͹��ļ��ϵ���lowpowertask��
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
	{
		return;
	}
	if( api_GetTaskFlag(eTASK_SYSWATCH_ID,eFLAG_10_MS) == TRUE )
	{
		s_SystickRun = 0;
		api_ClrTaskFlag(eTASK_SYSWATCH_ID,eFLAG_10_MS);
	}
	else
	{
		SystickRun();
	}
	
	MainLoopCounter++;
	
	//�Ƿ�1��
	if( api_GetTaskFlag(eTASK_SYSWATCH_ID,eFLAG_SECOND) == TRUE )
	{
	    api_GetRtcDateTime(DATETIME_ss,&tSec);
		
		if( (tSec%10) == 0 )//ÿ10�����һ��ʱ�ӵ�ص�ѹˢ�� �����ֳ���ؿɸ�����������غ��ܿ��ټ�⵽
		{
		    FreshClockBatt();    
		    FreshReadBatt();
		}

		#if(RTC_CHIP != RTC_CHIP_8025T)
		//ÿ����У��RTC�²�ϵ��
		if( tSec == (eTASK_SYSWATCH_ID * 3 + 3 + 0) )
		{
			api_Load_RtcInfoData(); //HT6025 ������ Aϵ������ҳɸ������ֵ ���ɴﵽ+66675��PPM
		}
		#endif	//#if(RTC_CHIP != RTC_CHIP_8025T)
		
		MainLoopTimer++;
		api_ClrTaskFlag(eTASK_SYSWATCH_ID,eFLAG_SECOND);
	}
	
	//�Ƿ�1����
	if( api_GetTaskFlag(eTASK_SYSWATCH_ID,eFLAG_MINUTE) == TRUE )
	{
		api_ClrTaskFlag(eTASK_SYSWATCH_ID,eFLAG_MINUTE);
		
		CheckBatteryPara();//��ѹ��У׼����У��
		
		FreshTemperature();
		
		api_FreshNewRegister(); //HT6025K����д�����Ĵ���ά��
		
		MainLoopSecCounter = MainLoopCounter/MainLoopTimer;
		if(MainLoopTimer > 86400)
		{
			MainLoopCounter = 0;
			MainLoopTimer = 0;
		}
	}
}


//-----------------------------------------------
//��������: ��ȡͣ�糭���أ�ʱ�ӵ�أ��¶�ֵ
//
//����: 
//			DataType[in]			DATA_BCD��DATA_HEX��
//			Channel[in]				ѡ��Ҫ��������������
//#define 	SYS_POWER_AD		    		0//ϵͳ��ѹ ������λС�� ��λv
//#define	SYS_CLOCK_VBAT_AD		    	1//ʱ�ӵ�ص�ѹ ������λС�� ��λv
//#define	SYS_READ_VBAT_AD	    		2//�����صĵ�ѹ ������λС�� ��λv
//#define	SYS_TEMPERATURE_AD		    	3//�¶�AD���� ����һλС��
//                    
//����ֵ:  	����������ֵ,BCD���ʽʱBIT15Ϊ1��ʾ������HEX���ʽʱ�ǲ�������
//
//��ע:   
//-----------------------------------------------
WORD api_GetBatAndTempValue( BYTE DataType, BYTE Channel )
{
	WORD Result,DataSign;
	
	switch( Channel )
	{
		case SYS_POWER_AD:
			Result = SysWatchRamData.VolNow;
			break;
		case SYS_CLOCK_VBAT_AD:
			Result = SysWatchRamData.ClockBattery;
			break;
		case SYS_READ_VBAT_AD:
			Result = SysWatchRamData.ReadBattery;
			break;
		case SYS_TEMPERATURE_AD:
			Result = SysWatchRamData.Temperature;
			break;
		default://!!!!!!
			return 0X8000;//������false��ԭ���ǵ���ص�ѹֵΪ0ʱ��False��ͬ��������
	}
	
	if( DataType == DATA_HEX )
	{
		return Result;
	}
	
	if( ((short)Result) < 0 )
	{
		DataSign = 1;
		Result = ~Result;
		Result += 1;
	}
	else
	{
		DataSign = 0;
	}
	
	Result = lib_WBinToBCD( Result );
	
	if( DataSign == 1 )
	{
		Result |= 0x8000;
	}
	
	return Result;
}

//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_FactoryInitSyswatch( void )
{
	//��ȡ��ȷ��ʾCRCУ����ȷ����Ҫ����ΪĬ��ֵ
	if(api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SysWatchSafeRom.BatteryPara), sizeof(TBatteryPara), (BYTE*)&BatteryPara) == TRUE)
	{
		return;
	}
	BatteryPara.ClockBatteryOffset = 20;//��֮ǰֵ����֮ǰ����һ��
	BatteryPara.ReadBatteryOffset = 20;
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SysWatchSafeRom.BatteryPara), sizeof(TBatteryPara), (BYTE*)&BatteryPara);
}


