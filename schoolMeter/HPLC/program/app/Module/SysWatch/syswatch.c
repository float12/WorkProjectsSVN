//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.16
//描述		系统监视文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
DWORD	MainLoopCounter;		//大循环次数
DWORD	MainLoopTimer;			//大循环秒数
DWORD	MainLoopSecCounter;		//大循环每秒循环次数
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//电池电压偏置参数
__no_init TBatteryPara BatteryPara;

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static TSysWatchRamData SysWatchRamData;
static volatile WORD s_SystickRun;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述: 校验电池参数
//         
//参数:     无
//         
//返回值:    无 
//         
//备注内容:  无
//--------------------------------------------------
void CheckBatteryPara( void )
{
	WORD Result;
	
	//校验电池偏移系数
	if(BatteryPara.CRC32 != lib_CheckCRC32((BYTE *)&BatteryPara, (sizeof(TBatteryPara) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SysWatchSafeRom.BatteryPara), sizeof(TBatteryPara), (BYTE*)&BatteryPara);
		if( Result != TRUE ) 
		{
			BatteryPara.ClockBatteryOffset = 20;//赋之前值，与之前保持一致
			BatteryPara.ReadBatteryOffset = 20;
		}
	}
}

//--------------------------------------------------
//功能描述: 校准电池参数
//         
//参数:     无
//         
//返回值:    WORD True 成功； False 失败
//         
//备注内容:  无
//--------------------------------------------------
WORD api_AdjustBatteryVoltage( void )
{
	WORD Result,TmpReadBattery,TmpClockBattery;

	memset( &BatteryPara, 0x00, sizeof(TBatteryPara) );//清零参数

	#if( MAX_PHASE == 3 )//三相校准
	TmpReadBattery = (WORD)GetADValue( 0x55, SYS_READ_VBAT_AD );
	if( abs(ReadBatteryStandardVoltage - TmpReadBattery) < (BYTE)(ReadBatteryStandardVoltage*15/100) )//电压差值小于允许电压差值 电压差值=标准电压*0.15
	{
		BatteryPara.ReadBatteryOffset = (signed char)(ReadBatteryStandardVoltage-TmpReadBattery);
	}
	else
	{	
		//校准失败，恢复原值
		CheckBatteryPara( );
		return FALSE;
	}
	#endif

	TmpClockBattery = (WORD)GetADValue( 0x55, SYS_CLOCK_VBAT_AD );
	if( abs(ClockBatteryStandardVoltage -TmpClockBattery) < (BYTE)(ClockBatteryStandardVoltage*15/100) )//电压差值小于允许电压差值 电压差值=标准电压*0.15
	{
		BatteryPara.ClockBatteryOffset = (signed char)(ClockBatteryStandardVoltage-TmpClockBattery);
	}
	else
	{
		//校准失败，恢复原值
		CheckBatteryPara( );
		return FALSE;
	}
	
	//校准数据写E2
	Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SysWatchSafeRom.BatteryPara), sizeof(TBatteryPara), (BYTE*)&BatteryPara);

	return Result;
}

//刷新停电抄表电池字节
void FreshReadBatt( void )
{
	BYTE i;
	WORD Result;

	//时钟电池状态
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


//刷新时钟电池状态
void FreshClockBatt( void )
{
	BYTE i;
	WORD Result;

	//时钟电池状态
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


//刷新表内温度
void FreshTemperature( void )
{
	short Result;
	
	//温度状态
	Result = GetADValue( 0x55, SYS_TEMPERATURE_AD );
	
	SysWatchRamData.Temperature = (short)Result;
}

//-----------------------------------------------
//函数功能: 系统监视模块的上电初始化工作
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
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
//函数功能: systick不运行复位芯片
//
//参数: 无
//                   
//返回值:  无
//
//备注:   
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
//函数功能: 系统监视模块任务
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SysWatchTask(void)
{
	#if(SEL_DEBUG_VERSION == YES)
	WORD i;
	#endif
	BYTE tSec;
	
	//只有系统真正上电后才系统自检 低功耗检上电在lowpowertask中
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
	
	//是否到1秒
	if( api_GetTaskFlag(eTASK_SYSWATCH_ID,eFLAG_SECOND) == TRUE )
	{
	    api_GetRtcDateTime(DATETIME_ss,&tSec);
		
		if( (tSec%10) == 0 )//每10秒进行一次时钟电池电压刷新 避免现场电池可更换电表更换电池后不能快速检测到
		{
		    FreshClockBatt();    
		    FreshReadBatt();
		}

		#if(RTC_CHIP != RTC_CHIP_8025T)
		//每分钟校验RTC温补系数
		if( tSec == (eTASK_SYSWATCH_ID * 3 + 3 + 0) )
		{
			api_Load_RtcInfoData(); //HT6025 经测试 A系数如果乱成负的最大值 误差可达到+66675个PPM
		}
		#endif	//#if(RTC_CHIP != RTC_CHIP_8025T)
		
		MainLoopTimer++;
		api_ClrTaskFlag(eTASK_SYSWATCH_ID,eFLAG_SECOND);
	}
	
	//是否到1分钟
	if( api_GetTaskFlag(eTASK_SYSWATCH_ID,eFLAG_MINUTE) == TRUE )
	{
		api_ClrTaskFlag(eTASK_SYSWATCH_ID,eFLAG_MINUTE);
		
		CheckBatteryPara();//电压自校准参数校验
		
		FreshTemperature();
		
		api_FreshNewRegister(); //HT6025K新增写保护寄存器维护
		
		MainLoopSecCounter = MainLoopCounter/MainLoopTimer;
		if(MainLoopTimer > 86400)
		{
			MainLoopCounter = 0;
			MainLoopTimer = 0;
		}
	}
}


//-----------------------------------------------
//函数功能: 获取停电抄表电池，时钟电池，温度值
//
//参数: 
//			DataType[in]			DATA_BCD或DATA_HEX码
//			Channel[in]				选择要读数的数据类型
//#define 	SYS_POWER_AD		    		0//系统电压 保留二位小数 单位v
//#define	SYS_CLOCK_VBAT_AD		    	1//时钟电池电压 保留二位小数 单位v
//#define	SYS_READ_VBAT_AD	    		2//抄表电池的电压 保留二位小数 单位v
//#define	SYS_TEMPERATURE_AD		    	3//温度AD采样 保留一位小数
//                    
//返回值:  	读出的数据值,BCD码格式时BIT15为1表示负数，HEX码格式时是补码数据
//
//备注:   
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
			return 0X8000;//不返回false的原因是当电池电压值为0时与False相同不能区分
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
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_FactoryInitSyswatch( void )
{
	//读取正确表示CRC校验正确不需要更改为默认值
	if(api_VReadSafeMem( GET_SAFE_SPACE_ADDR(SysWatchSafeRom.BatteryPara), sizeof(TBatteryPara), (BYTE*)&BatteryPara) == TRUE)
	{
		return;
	}
	BatteryPara.ClockBatteryOffset = 20;//赋之前值，与之前保持一致
	BatteryPara.ReadBatteryOffset = 20;
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(SysWatchSafeRom.BatteryPara), sizeof(TBatteryPara), (BYTE*)&BatteryPara);
}


