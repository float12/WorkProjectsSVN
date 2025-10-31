//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.26
//����		��Դ�쳣�¼������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PowerErr.h"
#include "HT_LowPower_Three.h"

#if( SEL_EVENT_POWER_ERR == YES )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
__no_init DWORD wPowerErrFlag;//ģ���ڱ���

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static const DWORD PowerErrDefaultOadConst[] =
{
	0x02,			//��������ĸ���:2
	0x00102201,    	//�¼�����ʱ�������й��ܵ���
	0x00202201,   	//�¼�����ʱ�̷����й��ܵ���
};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
#if(MAX_PHASE == 3)
static void TestPowerErr( void )
{
	DWORD tdw;
	
	if( g_byDownPowerTime == START_TEST_POWER_ERR )
	{
        InitPort(ePowerWakeUpMode);
		POWER_SAMPLE_OPEN;
		POWER_FLASH_OPEN;
		//POWER_ESAM_OPEN;
		api_Delayms(10);
		CLEAR_WATCH_DOG;
		api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
		api_Delayms(10);

		if( api_InitSampleChip() == FALSE )
		{
			RESET_STACK_POINTER;
			api_EnterLowPower( eFromDetectEnterDownMode );
		}
	}

	if( g_byDownPowerTime == (START_TEST_POWER_ERR+2) )
	{
		#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
		api_ProcSampleCorr(READ, 0x4000+0x2c, 4, (BYTE*)&tdw);
		if( (tdw&0x00000007) != 0x00000007 )
		{
			//�õ�Դ�쳣��־
			wPowerErrFlag = 0x5aa55aa5;
		}
		#endif

		RESET_STACK_POINTER;
		api_EnterLowPower( eFromDetectEnterDownMode );
	}
}

void PowerErrCheck( BYTE Type )
{
	if( IsExitLowPower == 20 )
	{
		if( ((Type ==eFromOnRunEnterDownMode)||(Type==eFromDetectEnterDownMode)) 
			&& (g_byDownPowerTime<(START_TEST_POWER_ERR+10)) )
		{
			if( !( (api_GetRelayStatus(1) != 0) && (RelayTypeConst == RELAY_INSIDE) ) )
			{
				//���е�Դ�쳣���
				switch( g_byDownPowerTime )
				{
				case START_TEST_POWER_ERR:
					api_MCU_SysClockInit( ePowerDownMode );//�л���5.5Mʱ��
					TestPowerErr();
					break;
				case (START_TEST_POWER_ERR+1):
				case (START_TEST_POWER_ERR+2):
					TestPowerErr();
					break;
				default:
					RESET_STACK_POINTER;
					api_EnterLowPower( eFromDetectEnterDownMode );
				}
			}
			else
			{
				IsExitLowPower = 0;
			}
		}
		else
		{
			IsExitLowPower = 0;
		}
	}
}

#else//#if(MAX_PHASE == 3)

void PowerErrCheck( BYTE Type )
{
	static BYTE s_CheckCount;
	WORD i,m,n;
	
	if( Type != eFromOnRunEnterDownMode )
	{
		return;
	}
	
	if(s_CheckCount >= 2)
	{
		return;
	}
	s_CheckCount++;

	//������Դ�쳣���жϼ̵�������բ״̬����Ϊ��բ�󣬵�����޷�����Դ�Ƿ��쳣
	
	m = 0;
	n = 0;

	RELAY_CHECK_STATUS;
	for(i=0;i<54;i++)//ʵ��22����
	{
		if(RELAY_STATUS_OPEN)
		{
			m++;
		}
		else
		{
			n++;
		}
	}
	RELAY_LOWPOWER_STATUS;
	
	wPowerErrFlag <<= 16;
	if( m > 14 )//��������22���룬���54�Σ�̼Ĥ�������ߵ�ƽ��20 -24֮�䡣
	{
		//�õ�Դ�쳣��־ Ϊ0x5aa55aa5, ���μ��Ϊ��Դ�쳣��Ϊ��Դ�쳣
		wPowerErrFlag |= 0x5aa5;
	}
}

#endif//#if(MAX_PHASE == 3)


void PowerUpDealPowerErr( void )
{
	if( wPowerErrFlag == 0x5aa55aa5 )
	{
		SaveInstantEventRecord( eEVENT_POWER_ERR_NO, 0, EVENT_START, eEVENT_ENDTIME_PWRDOWN );
		SaveInstantEventRecord( eEVENT_POWER_ERR_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
	}
	wPowerErrFlag = 0;
}


//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣���Դ�쳣
//------------------------------------------------------------------------------------------------------
// �жϵ�Դ�쳣�Ƿ���
BYTE GetPowerErrStatus(BYTE Phase)
{
	//�˴������Ƿ������̵��ã�ʵ�ʲ�����������ͣ�ϵ�ʱ����
	return 0;
}

void FactoryInitPowerErrPara( void )
{
	api_WriteEventAttribute( 0x302c, 0xff, (BYTE *)&PowerErrDefaultOadConst[0], sizeof(PowerErrDefaultOadConst)/sizeof(DWORD) );
}

#endif

