//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.26
//描述		电源异常事件处理文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PowerErr.h"
#include "HT_LowPower_Three.h"

#if( SEL_EVENT_POWER_ERR == YES )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
__no_init DWORD wPowerErrFlag;//模块内变量

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static const DWORD PowerErrDefaultOadConst[] =
{
	0x02,			//关联对象的个数:2
	0x00102201,    	//事件发生时刻正向有功总电能
	0x00202201,   	//事件发生时刻反向有功总电能
};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
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
			//置电源异常标志
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
				//进行电源异常检测
				switch( g_byDownPowerTime )
				{
				case START_TEST_POWER_ERR:
					api_MCU_SysClockInit( ePowerDownMode );//切换到5.5M时钟
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

	//单相表电源异常不判断继电器跳合闸状态，因为跳闸后，单相表无法检测电源是否异常
	
	m = 0;
	n = 0;

	RELAY_CHECK_STATUS;
	for(i=0;i<54;i++)//实测22毫秒
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
	if( m > 14 )//理论推算22毫秒，检测54次，碳膜按键：高电平在20 -24之间。
	{
		//置电源异常标志 为0x5aa55aa5, 两次检测为电源异常则为电源异常
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
// 各个事件处理专用部分－电源异常
//------------------------------------------------------------------------------------------------------
// 判断电源异常是否发生
BYTE GetPowerErrStatus(BYTE Phase)
{
	//此处仅仅是方便流程调用，实际操作部分是在停上电时处理
	return 0;
}

void FactoryInitPowerErrPara( void )
{
	api_WriteEventAttribute( 0x302c, 0xff, (BYTE *)&PowerErrDefaultOadConst[0], sizeof(PowerErrDefaultOadConst)/sizeof(DWORD) );
}

#endif

