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
static __no_init BYTE MeterCoverOldStatus;					// 开表盖状态
static __no_init TLowPowerMeterCoverData LowPowerMeterCoverData;		// 低功耗数据
#endif

#if( SEL_EVENT_BUTTONCOVER == YES )
static __no_init BYTE ButtonCoverOldStatus;					// 开端钮盖状态
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
	BYTE Buf[((SUB_EVENT_INDEX_MAX_NUM+7)/8)+20],i;
	WORD Len;
	DWORD dwAddr;

	Len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] );
	api_ReadFromContinueEEPRom( dwAddr, Len, Buf );
	// 掉电前事件已经开始,即已开表盖
	if( ( Buf[eSUB_EVENT_METER_COVER/8] & (0x01<<(eSUB_EVENT_METER_COVER%8)) ) != 0 )
	{
		//检测表盖状态
		for(i=0;i<5;i++)	
		{
			if(!( UP_COVER_IS_OPEN ))
			{
				break;
			}
			api_Delayms(10);
		}
		
		if(i>=5)	// 上电后表盖仍是打开状态，不结束
		{
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
			return;
		}
		else		// 上电后表盖是关闭状态，掉电时间补结束
		{
			SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_PWRDOWN );
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);
		}
	}



	// TRealTimer RealTimerBak;

	// if(LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	// {
	// 	//低功耗期间未发生开盖事件
	// 	//如果当前状态为开盖，保持！
	// 	if(MeterCoverOldStatus == 1)
	// 	{
	// 		EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
	// 		api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
	// 	}
	// 	return;
	// }

	// //开始处理低功耗期间开盖数据
	// //先看是否需要记录开盖事件开始
	// //期间至少发生过表盖变位
	// memcpy(&RealTimerBak, &RealTimer, sizeof(TRealTimer));
	// if( api_CheckClock(&LowPowerMeterCoverData.MeterCoverOpenTime) == TRUE )
	// {
	// 	//记录开始
	// 	RealTimer = LowPowerMeterCoverData.MeterCoverOpenTime;
	// 	SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_START, eEVENT_ENDTIME_CURRENT );
	// 	EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
	// 	api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
	// }
	// if( api_CheckClock(&LowPowerMeterCoverData.MeterCoverCloseTime) == TRUE )
	// {
	// 	//记录结束
	// 	RealTimer = LowPowerMeterCoverData.MeterCoverCloseTime;
	// 	SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
	// 	EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
	// 	api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
	// }
	// memcpy(&RealTimer, &RealTimerBak, sizeof(TRealTimer));//因为前面把时间覆盖了，所以要刷新一下
}

//-----------------------------------------------
//功能描述  :    低功耗开表盖检测
//参数:  
//           BYTE Mode[in] 0：低功耗    0x55：唤醒
//
//返回值:     
//备注内容  :   
//-----------------------------------------------
void ProcLowPowerMeterCoverRecord( BYTE Mode )
{
	BYTE Status;

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

	if( Status != MeterCoverOldStatus )
	{
		LowPowerMeterCoverData.CheckMeterCoverStatus = 0x12345678;
		if( Status == 0 )//新状态是0，开盖结束
		{
			if( LowPowerMeterCoverData.MeterCoverCloseTime.Day != 0xff )
			{
				//之前已经发生过
				MeterCoverOldStatus = Status;
				return;
			}
			//临时记录开盖结束的RAM值
			LowPowerMeterCoverData.MeterCoverStatus[1] = Status;
			
			if( Mode == 0x55 )//唤醒情况下,不需要开E2电源
			{
				api_ReadMeterTime( 0 );
			}
			else
			{
				POWER_FLASH_OPEN;
				api_ReadMeterTime( 0 );
				// POWER_FLASH_CLOSE;
			}

			LowPowerMeterCoverData.MeterCoverCloseTime = RealTimer;
			MeterCoverOldStatus = Status;
			return;
		}
		else//新状态不是0，开盖开始
		{
			if( ( LowPowerMeterCoverData.MeterCoverCloseTime.Day != 0xff ) || ( LowPowerMeterCoverData.MeterCoverOpenTime.Day != 0xff ) )
			{
				//之前已经发生过或者低功耗期间发生过开盖结束
				MeterCoverOldStatus = Status;
				return;
			}
			//临时记录开盖开始的RAM值
			LowPowerMeterCoverData.MeterCoverStatus[0] = Status;
			
			if( Mode == 0x55 )//唤醒情况下,不需要开E2电源
			{
				api_ReadMeterTime( 0 );
			}
			else
			{
				POWER_FLASH_OPEN;
				api_ReadMeterTime( 0 );
				// POWER_FLASH_CLOSE;
			}

			LowPowerMeterCoverData.MeterCoverOpenTime = RealTimer;
		}
	}

	MeterCoverOldStatus = Status;
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
//上电时处理低功耗期间开盖记录
void ButtonCoverPowerOn( void )
{
	TRealTimer RealTimerBak;

	if(LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	{
		//低功耗期间未发生开端钮盖事件
		//如果当前状态为开盖，保持！
		if(ButtonCoverOldStatus == 1)
		{
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER );
		}
		return;
	}
	//开始处理低功耗期间开端钮盖数据
	//先看是否需要记录开盖事件开始
	//期间至少发生过表盖变位
	memcpy(&RealTimerBak, &RealTimer, sizeof(TRealTimer));
	if( api_CheckClock(&LowPowerButtonCoverData.ButtonCoverOpenTime) == TRUE )
	{
		//记录开始
		RealTimer = LowPowerButtonCoverData.ButtonCoverOpenTime;
		SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_START, eEVENT_ENDTIME_CURRENT );
		EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
		api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);
	}
	if( api_CheckClock(&LowPowerButtonCoverData.ButtonCoverCloseTime) == TRUE )
	{
		//记录结束
		RealTimer = LowPowerButtonCoverData.ButtonCoverCloseTime;
		SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
		EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
		api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
	}
	memcpy(&RealTimer, &RealTimerBak, sizeof(TRealTimer));//因为前面把时间覆盖了，所以要刷新一下
}

//-----------------------------------------------
//功能描述  :    低功耗开表盖检测
//参数:  
//           BYTE Mode[in] 0：低功耗    0x55：唤醒
//
//返回值:     
//备注内容  :   
//-----------------------------------------------
void ProcLowPowerButtonCoverRecord( BYTE Mode )
{	
	BYTE Status;

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
		LowPowerButtonCoverData.CheckButtonCoverStatus = 0x12345678;
		if( Status == 0 )//新状态是0，开端钮盖结束
		{
			if( LowPowerButtonCoverData.ButtonCoverCloseTime.Day != 0xff )
			{
				ButtonCoverOldStatus = Status;
				//之前已经发生过
				return;
			}
			//临时记录开端钮盖结束的RAM值
			LowPowerButtonCoverData.ButtonCoverStatus[1] = Status;
			
			if( Mode == 0x55 )//唤醒情况下,不需要开E2电源
			{
				api_ReadMeterTime( 0 );

			}
			else
			{
				POWER_FLASH_OPEN;
				api_ReadMeterTime( 0 );
				POWER_FLASH_CLOSE;
			}

			LowPowerButtonCoverData.ButtonCoverCloseTime = RealTimer;
			ButtonCoverOldStatus = Status;
			return;
		}
		else//新状态不是0，开端钮盖开始
		{
			if( ( LowPowerButtonCoverData.ButtonCoverOpenTime.Day != 0xff ) || ( LowPowerButtonCoverData.ButtonCoverCloseTime.Day != 0xff ) )
			{
				//之前已经发生过或者低功耗期间发生过开端钮盖结束
				ButtonCoverOldStatus = Status;
				return;
			}
			LowPowerButtonCoverData.ButtonCoverStatus[0] = Status;
			
			if( Mode == 0x55 )//唤醒情况下,不需要开E2电源
			{
				api_ReadMeterTime( 0 );
			}
			else
			{
				POWER_FLASH_OPEN;
				api_ReadMeterTime( 0 );
				POWER_FLASH_CLOSE;
			}

			LowPowerButtonCoverData.ButtonCoverOpenTime = RealTimer;
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
