//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.10.8
//描述		开表盖，开端钮盖模块源文件
//修改记录	
//---------------------------------------------------------------------- 

#include "AllHead.h"
#include "cover.h"
#if((SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES))
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------



//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
#if( SEL_EVENT_METERCOVER == YES )
static __no_init DWORD MeterCoverOldStatus;					// 开表盖状态
static __no_init TLowPowerMeterCoverData LowPowerMeterCoverData;		// 低功耗数据
#endif

#if( SEL_EVENT_BUTTONCOVER == YES )
static __no_init DWORD ButtonCoverOldStatus;					// 开端钮盖状态
static __no_init TLowPowerButtonCoverData LowPowerButtonCoverData;	// 低功耗数据
#endif

// 开表盖 0x301b 开端钮盖 0x301c
static const DWORD CoverOad[] =
{
#if( MAX_PHASE == 1)
	0x04,		// 个数
	0x00102201,//发生时刻正向有功总电能
	0x00202201,//发生时刻反向有功总电能
	0x00108201,//结束时刻正向有功总电能
	0x00208201// 结束时刻正向有功总电能
#else
	0x0c,		// 个数
	0x00102201,//发生时刻正向有功总电能
	0x00202201,//发生时刻反向有功总电能
	0x00502201,//发生时刻第一象限无功总电能
	0x00602201,//发生时刻第二象限无功总电能
	0x00702201,//发生时刻第三象限无功总电能
	0x00802201,//发生时刻第四象限无功总电能
	0x00108201,//结束时刻正向有功总电能
	0x00208201,//结束时刻正向有功总电能
	0x00508201,//结束时刻第一象限无功总电能
	0x00608201,//结束时刻第二象限无功总电能
	0x00708201,//结束时刻第三象限无功总电能
	0x00808201,//结束时刻第四象限无功总电能
#endif
};
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
#if( SEL_EVENT_METERCOVER == YES )
void MeterCoverPowerOn( void )
{
	TRealTimer RealTimerBak;
	DWORD dwAddr;
	//从E2恢复
	BYTE CoverStatus=0,Buf[10],CloseTimeFlag=0,OpenTimeFlag=0;
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_METER_COVER/8] );
	if(api_ReadFromContinueEEPRom( dwAddr, sizeof(BYTE), Buf ) == FALSE)
	{
		CoverStatus = 0;
		EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
		api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
	}
	else
	{
		if(Buf[0] & (0x01<<(eSUB_EVENT_METER_COVER%8)))
		{
			CoverStatus = 1;
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
		}
		else
		{
			CoverStatus = 0;
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
		}
	}
	//RAM数据不对 再从EE恢复
	if( (LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	 && (LowPowerMeterCoverData.CheckMeterCoverStatus != 0xffffffff) )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
		api_ReadFromContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);
	}
	//不可删-防止频繁上下单，RAM乱为时间合法，误记录
	if( LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	{ 
		//如果数据不是全FF 重新设置一下EE 防止更改程序EE位置改变导致开盖记录错误
		memset( &LowPowerMeterCoverData, 0xff, sizeof(TLowPowerMeterCoverData) );
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
		api_WriteToContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);
		return;
	}
	if( api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverOpenTime) == TRUE )
	{
		OpenTimeFlag = 1;
	}

	if(api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverCloseTime) == TRUE )
	{
		CloseTimeFlag = 1;

	}
	
	//开始处理低功耗期间开盖数据
	memcpy(&RealTimerBak, &RealTimer, sizeof(TRealTimer));//写事件记录借用RealTimer，不可删
	if( CoverStatus == 1 )//开盖
	{
		if( CloseTimeFlag == 1 )//合盖时间合法
		{
			//记录结束
			RealTimer = LowPowerMeterCoverData.MeterCoverCloseTime;
			SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
		}
	}
	else//合盖
	{
		if(OpenTimeFlag == 1 )//开盖时间合法
		{
			//记录开始
			RealTimer = LowPowerMeterCoverData.MeterCoverOpenTime;
			SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_START, eEVENT_ENDTIME_CURRENT );
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
			if( CloseTimeFlag == 1 )//合盖时间合法
			{
				//记录结束
				RealTimer = LowPowerMeterCoverData.MeterCoverCloseTime;
				SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
				EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
				api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
			}
		}
	}
	memcpy(&RealTimer, &RealTimerBak, sizeof(TRealTimer));//不可删，因为前面把时间覆盖了，所以要刷新一下
	
	//将RAM和EE数据置为全FF
	memset(&LowPowerMeterCoverData,0xff,sizeof(TLowPowerMeterCoverData));
	dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
	api_WriteToContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);
}

//-----------------------------------------------
//功能描述  : 进入低功耗前进行开盖数据检查
//参数:
//         
//
//返回值:     
//备注内容:只能在进入低功耗前调用 低功耗下不能调用
//-----------------------------------------------
void api_LowPowerCheckMeterCoverStatus( void )
{
	BYTE Buf[8];
	DWORD dwAddr;
		
	//RAM中数据错误从EE恢复
	if( ( MeterCoverOldStatus > 1 )
	|| ( (LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	   &&(LowPowerMeterCoverData.CheckMeterCoverStatus != 0xffffffff)))
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_METER_COVER/8] );
		if(api_ReadFromContinueEEPRom( dwAddr, sizeof(BYTE), Buf ) == FALSE)
		{
			MeterCoverOldStatus = 0;	
		}
		else
		{
			if(Buf[0] & (0x01<<(eSUB_EVENT_METER_COVER%8)))
			{
				MeterCoverOldStatus = 1;
			}
			else
			{
				MeterCoverOldStatus = 0;
			}
		}
		
		//从EE恢复掉电期间开盖记录
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
		api_ReadFromContinueEEPRom(	dwAddr, sizeof(LowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);

		//如果EE数据还是不对 置为全FF
		if( (LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
		 && (LowPowerMeterCoverData.CheckMeterCoverStatus != 0xffffffff) )
		{
			memset(&LowPowerMeterCoverData, 0xff, sizeof(LowPowerMeterCoverData));
		}
	}
}
//-----------------------------------------------
//功能描述  :    低功耗开表盖检测
//参数:
//           BYTE Mode[in] 0：低功耗    0x55：唤醒
//
//返回值:
//备注内容  :
//-----------------------------------------------
void ProcLowPowerMeterCoverRecord(  )
{
	BYTE Status, Mode;
	DWORD dwAddr;
	//RAM中数据合法性判断
	if( ( MeterCoverOldStatus > 1 )
	|| ( (LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	   &&(LowPowerMeterCoverData.CheckMeterCoverStatus != 0xffffffff)))
	{
		//默认置为合盖状态
		MeterCoverOldStatus = 0;
		memset(&LowPowerMeterCoverData, 0xff, sizeof(LowPowerMeterCoverData));
	}
	if( api_CheckMCUCLKIsFlf() == TRUE )//检测当前频率是否为32768
	{
		Mode = 0;
	}
	else
	{
		Mode = 0x55;
	}
	
	//开合盖检测
	if( UP_COVER_IS_OPEN )
	{
		Status = 1;
		if( Status != MeterCoverOldStatus )
		{
			if( Mode == 0x55 )//唤醒情况下进行消抖
			{
				api_Delayms(50);//实测延时时间为50.22ms
			}
			else
			{
				api_Delay100us( 1 );//实测延时时间为48.28ms
			}

			if( UP_COVER_IS_OPEN )
			{
				Status = 1;
			}
			else
			{
				Status = 0;
			}
		}
	}
	else
	{
		Status = 0;
	}

	//开合盖记录
	if( Status != MeterCoverOldStatus )
	{
		MeterCoverOldStatus = Status;//开始就把状态刷新一下 防止后面开EE记录开合盖导致复位
		LowPowerMeterCoverData.CheckMeterCoverStatus = 0x12345678;
		if( Status == 0 )//新状态是0，开盖结束
		{	
			//已经记录合盖 无需重复记录
			if( api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverCloseTime) == TRUE )
			{
				//之前已经发生过或者低功耗期间发生过合盖结束
				return;
			}
			//临时记录开盖结束的RAM值
			LowPowerMeterCoverData.MeterCoverStatus[1] = Status;
			
			if( Mode == 0x55 )//唤醒情况下,不需要开E2电源
			{
				api_ReadMeterTimeLowPower();
			}
			else
			{
				api_ReadMeterTimeLowPower();
			}

			LowPowerMeterCoverData.MeterCoverCloseTime = RealTimer;
		}
		else//新状态不是0，开盖开始
		{
			if( ( api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverCloseTime) == TRUE )
			|| ( api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverOpenTime) == TRUE ))
			{
				//之前已经发生过或者低功耗期间发生过开盖结束
				return;
			}
			//临时记录开盖开始的RAM值
			LowPowerMeterCoverData.MeterCoverStatus[0] = Status;
			
			if( Mode == 0x55 )//唤醒情况下,不需要开E2电源
			{
				api_ReadMeterTimeLowPower();
			}
			else
			{
				api_ReadMeterTimeLowPower();
			}

			LowPowerMeterCoverData.MeterCoverOpenTime = RealTimer;
		}
		
		//数据写入EE
		//一定要写完RAM之后再开启高频HRC 防止功耗高CPU复位导致开盖未记录
		if( Mode != 0x55 )
		{
			api_MCU_SysClockInit( ePowerDownMode );//切换到5.5M时钟
		}
		POWER_EEPROM_OPEN;
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
		api_WriteToContinueEEPRom(	dwAddr, sizeof(LowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);
		EEPROM_INIT_LOW_POWER;
		POWER_EEPROM_CLOSE;	
		
		if( Mode != 0x55 )
		{
			HTMCU_GoToLowSpeed(); //切换到低频32.768时钟
		}
	}
}

//-----------------------------------------------
//函数功能: 判断开表盖是否发生
//
//参数:  无
//                    
//返回值:	无
//
//备注: 
//-----------------------------------------------
BYTE GetMeterCoverStatus(BYTE Phase)
{
	BYTE i;
	//开盖检测
	if( UP_COVER_IS_OPEN )
	{
		if((EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] & (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8))) == 0)
		{
			for(i=0;i<5;i++)	
			{
				if(!( UP_COVER_IS_OPEN ))
				{
					return 0;
				}
				api_Delayms(10);
			}
		}
		return 1;
	}

	return 0;

}

void MeterCoverPowerOff( void )
{
	MeterCoverOldStatus = api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_METER_COVER );

	memset(&LowPowerMeterCoverData,0xff,sizeof(TLowPowerMeterCoverData));
}




#endif//#if( SEL_EVENT_METERCOVER == YES )


#if( SEL_EVENT_BUTTONCOVER == YES )
//上电时处理低功耗期间开端钮盖记录
void ButtonCoverPowerOn( void )
{
	TRealTimer RealTimerBak;
	DWORD dwAddr;
	//从E2恢复
	BYTE CoverStatus=0,Buf[10],CloseTimeFlag=0,OpenTimeFlag=0;
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_BUTTON_COVER/8] );
	if(api_ReadFromContinueEEPRom( dwAddr, sizeof(BYTE), Buf ) == FALSE)
	{
		CoverStatus = 0;
		EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
		api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
	}
	else
	{
		if(Buf[0] & (0x01<<(eSUB_EVENT_BUTTON_COVER%8)))
		{
			CoverStatus = 1;
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);
		}
		else
		{
			CoverStatus = 0;
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
		}
	}
	//RAM数据不对 再从EE恢复
	if( (LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	 && (LowPowerButtonCoverData.CheckButtonCoverStatus != 0xffffffff) )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
		api_ReadFromContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerButtonCoverData);
	}
	//不可删-防止频繁上下单，RAM乱为时间合法，误记录
	if( LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	{ 
		memset( &LowPowerButtonCoverData, 0xff, sizeof(TLowPowerMeterCoverData) );
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
		api_WriteToContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerButtonCoverData);
	}
	if( api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverOpenTime) == TRUE )
	{
		OpenTimeFlag = 1;
	}

	if(api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverCloseTime) == TRUE )
	{
		CloseTimeFlag = 1;

	}
	
	//开始处理低功耗期间开盖数据
	memcpy(&RealTimerBak, &RealTimer, sizeof(TRealTimer));//写事件记录借用RealTimer，不可删
	if( CoverStatus == 1 )//开盖
	{
		if( CloseTimeFlag == 1 )//合盖时间合法
		{
			//记录结束
			RealTimer = LowPowerButtonCoverData.ButtonCoverCloseTime;
			SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
		}
	}
	else//合盖
	{
		if(OpenTimeFlag == 1 )//开盖时间合法
		{
			//记录开始
			RealTimer = LowPowerButtonCoverData.ButtonCoverOpenTime;
			SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_START, eEVENT_ENDTIME_CURRENT );
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);
			if( CloseTimeFlag == 1 )//合盖时间合法
			{
				//记录结束
				RealTimer = LowPowerButtonCoverData.ButtonCoverCloseTime;
				SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
				EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
				api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
			}
		}
	}
	memcpy(&RealTimer, &RealTimerBak, sizeof(TRealTimer));//不可删，因为前面把时间覆盖了，所以要刷新一下
	
	memset(&LowPowerButtonCoverData,0xff,sizeof(TLowPowerButtonCoverData));
	dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
	api_WriteToContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerButtonCoverData);
}
//-----------------------------------------------
//功能描述  : 进入低功耗前进行开盖数据检查
//参数:
//         
//
//返回值:     
//备注内容:只能在进入低功耗前调用 低功耗下不能调用
//-----------------------------------------------
void api_LowPowerCheckButtonCoverStatus( void )
{
	BYTE Buf[8];
	DWORD dwAddr;
		
	//RAM中数据错误从EE恢复
	if( ( ButtonCoverOldStatus > 1 )
	|| ( (LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	   &&(LowPowerButtonCoverData.CheckButtonCoverStatus != 0xffffffff)))
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_BUTTON_COVER/8] );
		if(api_ReadFromContinueEEPRom( dwAddr, sizeof(BYTE), Buf ) == FALSE)
		{
			ButtonCoverOldStatus = 0;	
		}
		else
		{
			if(Buf[0] & (0x01<<(eSUB_EVENT_BUTTON_COVER%8)))
			{
				ButtonCoverOldStatus = 1;
			}
			else
			{
				ButtonCoverOldStatus = 0;
			}
		}
		
		//从EE恢复掉电期间开盖记录
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
		api_ReadFromContinueEEPRom(	dwAddr, sizeof(LowPowerButtonCoverData), (BYTE *)&LowPowerButtonCoverData);

		//如果EE数据还是不对 置为全FF
		if( (LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
		 && (LowPowerButtonCoverData.CheckButtonCoverStatus != 0xffffffff) )
		{
			memset(&LowPowerButtonCoverData, 0xff, sizeof(LowPowerButtonCoverData));
		}
	}
}
//-----------------------------------------------
//功能描述  :    低功耗开端钮盖检测
//参数:  
//           BYTE Mode[in] 0：低功耗    0x55：唤醒
//
//返回值:     
//备注内容  :   
//-----------------------------------------------
void ProcLowPowerButtonCoverRecord(  )
{	
	BYTE Status,Mode;
	DWORD dwAddr;
	
	//RAM中数据错误从EE恢复
	if( ( ButtonCoverOldStatus > 1 )
	|| ( (LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	   &&(LowPowerButtonCoverData.CheckButtonCoverStatus != 0xffffffff)))
	{
		ButtonCoverOldStatus = 0;	
		memset(&LowPowerButtonCoverData, 0xff, sizeof(LowPowerButtonCoverData));
	}
	if( api_CheckMCUCLKIsFlf() == TRUE )//检测当前频率是否为32768
	{
		Mode = 0;
	}
	else
	{
		Mode = 0x55;
	}
	//开合盖检测
	if( END_COVER_IS_OPEN )
	{
		Status = 1;
		if( Status != ButtonCoverOldStatus )
		{
			if( Mode == 0x55 )//唤醒情况下进行消抖
			{
				api_Delayms(50);//实测延时时间为50.22ms
			}
			else
			{
				api_Delay100us( 1 );//实测延时时间为48.28ms
			}

			if( END_COVER_IS_OPEN )
			{
				Status = 1;
			}
			else
			{
				Status = 0;
			}
		}
	}
	else
	{
		Status = 0;
	}
	
	if( Status != ButtonCoverOldStatus )
	{
		ButtonCoverOldStatus = Status;//开始就把状态刷新一下 防止后面开EE记录开合盖导致复位
		LowPowerButtonCoverData.CheckButtonCoverStatus = 0x12345678;
		if( Status == 0 )//新状态是0，开端钮盖结束
		{
			if( api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverCloseTime) == TRUE )
			{
				//之前已经发生过
				return;
			}
			//临时记录开端钮盖结束的RAM值
			LowPowerButtonCoverData.ButtonCoverStatus[1] = Status;
			
			if( Mode == 0x55 )//唤醒情况下,不需要开E2电源
			{
				api_ReadMeterTimeLowPower();
			}
			else
			{
				api_ReadMeterTimeLowPower();
			}

			LowPowerButtonCoverData.ButtonCoverCloseTime = RealTimer;
		}
		else//新状态不是0，开端钮盖开始
		{
			if( ( api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverOpenTime) == TRUE )
			|| ( api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverCloseTime) == TRUE ))
			{
				//之前已经发生过或者低功耗期间发生过开端钮盖结束
				return;
			}
			LowPowerButtonCoverData.ButtonCoverStatus[0] = Status;
			
			if( Mode == 0x55 )//唤醒情况下,不需要开E2电源
			{
				api_ReadMeterTimeLowPower();
			}
			else
			{
				api_ReadMeterTimeLowPower();
			}

			LowPowerButtonCoverData.ButtonCoverOpenTime = RealTimer;
		}
		//数据写入EE
		//一定要写完RAM之后再开启高频HRC 防止功耗高CPU复位导致开盖未记录
		if( Mode != 0x55 )
		{
			api_MCU_SysClockInit( ePowerDownMode );//切换到5.5M时钟
		}
		POWER_EEPROM_OPEN;
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
		api_WriteToContinueEEPRom(	dwAddr, sizeof(LowPowerButtonCoverData), (BYTE *)&LowPowerButtonCoverData);
		EEPROM_INIT_LOW_POWER;
		POWER_EEPROM_CLOSE;	
		
		if( Mode != 0x55 )
		{
			HTMCU_GoToLowSpeed(); //切换到低频32.768时钟
		}
	}
	ButtonCoverOldStatus = Status;
}

// 判断开端钮盒是否发生
BYTE GetButtonCoverStatus(BYTE Phase)
{
	//开端钮盖检测
	if( END_COVER_IS_OPEN )
	{
		return 1;
	}

	return 0;
}

void ButtonCoverPowerOff( void )
{
	ButtonCoverOldStatus = api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER );

	memset(&LowPowerButtonCoverData,0xff,sizeof(TLowPowerButtonCoverData));
}

#endif//#if( SEL_EVENT_BUTTONCOVER == YES )

//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void FactoryInitCoverPara( void )
{
	#if( SEL_EVENT_METERCOVER == YES )
	api_WriteEventAttribute( 0x301b, 0xFF, (BYTE *)&CoverOad[0], sizeof(CoverOad)/sizeof(DWORD) );			// 开表盖
	#endif

	#if( SEL_EVENT_BUTTONCOVER == YES )
	api_WriteEventAttribute( 0x301c, 0xFF, (BYTE *)&CoverOad[0], sizeof(CoverOad)/sizeof(DWORD) );			// 开端钮盖
	#endif
}

#endif//#if((SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES))
