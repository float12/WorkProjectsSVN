//---------------------------------------------
// 辅助电源失电、掉电事件记录
//---------------------------------------------

#include "AllHead.h"
#include "lostpower.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//用于把辅助电源失电事件保留到上电进行处理
//如果是0x5aa55aa5 表示掉电前已经发生了掉电事件
//如果是0x12341234 处理完后置为这个值，防止异常复位导致掉电事件重记
__no_init DWORD PowerDownLostSecPowerFlag;
static __no_init DWORD PowerDownLostPowerFlag;

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
#if( SEL_EVENT_LOST_POWER == YES )

// 掉电 0x3011  的关联对象属性表 oad采用正序输入
static const DWORD LostPowerDefaultOadConst[] =
{
	2,//关联对象的个数
	0x00102201,	// 事件发生时刻正向有功电能总
	0x00202201,	// 事件发生时刻反向有功电能总
};
#endif

#if( SEL_EVENT_LOST_SECPOWER == YES )//20121221ma
// 辅助电源掉电 0x300e 的关联对象属性表 oad采用正序输入
static const DWORD LostSecPowerDefaultOadConst[] =
{
	0//关联对象的个数
	
};
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
// 辅助电源失电事件
#if( SEL_EVENT_LOST_SECPOWER == YES )//20121221ma
// 判断辅助电源是否失电
BYTE GetLostSecPowerStatus(BYTE Phase)
{
	if(Phase>ePHASE_C)
	{
		return 0;
	}
	
	if( SelSecPowerConst == YES )
	{
		// 辅助电源失电
		if(SECPOWER_IS_DOWN)
		{
			return 1;
		}
	}

	return 0;
}

void LostSecPowerPowerOff( void )
{
    PowerDownLostSecPowerFlag = 0;
    if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_SECPOWER ) == 0 )   //辅助电源未失电
    {
        PowerDownLostSecPowerFlag = 0x5aa55aa5;
    }
}

#endif//#if( SEL_EVENT_LOST_SECPOWER == YES )//20121221ma


//---------------------------------------------
// 掉电事件
#if( SEL_EVENT_LOST_POWER == YES )

//-----------------------------------------------
//函数功能: 掉电事件的上电处理
//
//参数:  无
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------

void LostPowerPowerOn( void )
{  
	DWORD dwAddr;
	BYTE bLostflag;
	// 掉电事件处理
 	if( PowerDownLostPowerFlag == eOtherFlag )//异常复位不做处理
	{
	}
	else//无论电池是否有电，上电都认为掉过电
	{
		EventStatus.CurrentStatus[eSUB_EVENT_LOST_POWER/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_POWER%8));
		api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_POWER );
        if( PowerDownLostPowerFlag == eLostPowerFlag )//如果掉电前 电表处于掉电状态 结束掉电状态
        {

        }
        else if(PowerDownLostPowerFlag == eNormalFlag)//正常状态
        {
			SaveInstantEventRecord( eEVENT_LOST_POWER_NO, ePHASE_ALL, EVENT_START, eEVENT_ENDTIME_PWRDOWN );//记录事件开始   
        }
		else
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_LOST_POWER/8] );
			api_ReadFromContinueEEPRom( dwAddr, 1, &bLostflag );
			if (bLostflag&(0x01<<(eSUB_EVENT_LOST_POWER%8)))//如果掉电前 电表处于掉电状态 结束掉电状态
			{
				/* code */
			}
			else//正常状态
			{
				SaveInstantEventRecord( eEVENT_LOST_POWER_NO, ePHASE_ALL, EVENT_START, eEVENT_ENDTIME_PWRDOWN );//记录事件开始   
			}
			
			
		}
		
	}
	
	PowerDownLostPowerFlag = eOtherFlag;
	
}

void LostPowerPowerDown( void )
{
	DWORD dwAddr;
	BYTE PowerDownFlag;
	
	if(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 1)
	{
	    PowerDownLostPowerFlag = eLostPowerFlag;
	}
    else
    {
        PowerDownLostPowerFlag = eNormalFlag;
    }
}


//-----------------------------------------------
//函数功能: 判断是否掉电
//
//参数:  无
//                    
//返回值:	掉电状态 0：未掉电  1：掉电
//
//备注: 
//-----------------------------------------------
BYTE GetLostPowerStatus( BYTE Phase )
{
	BYTE Status, i;
	WORD wLimitU,wRemoteU;
	DWORD dwRemoteI;
	Status = 0;

	// 若系统有电
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == TRUE )
	{
#if( MAX_PHASE == 1 )
		// 临界电压
		wLimitU = wStandardVoltageConst * 6 /10;// 60%Un @@
		api_GetRemoteData(PHASE_A+REMOTE_U, DATA_HEX, 1, sizeof(WORD), (BYTE *)&wRemoteU);// 2字节 1位小数

		if( wRemoteU < wLimitU )
		{
			Status = 1;
		}
		return Status;
#else
		// 临界电压国网要求60%Un
		wLimitU = wStandardVoltageConst * 6 /10;// 60%Un @@

		for( i = 0; i < MAX_PHASE; i++ )
		{
			if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
			{
				Status |= 0x02;
				continue;
			}

			api_GetRemoteData(PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, sizeof(WORD), (BYTE *)&wRemoteU);// 2字节 1位小数

			if( wRemoteU < wLimitU )
			{
				// 此相失压
				Status |= (0x01<<i);
			}
		}

		// 如果三相电压都低于临界电压，则判定为掉电
		if(Status == 0x07)
		{
			return 1;
		}
		// 有电
		return 0;
#endif//#if( MAX_PHASE == 1 )
	}

	return 1;
}

#endif//#if( SEL_EVENT_LOST_POWER == YES )

//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void FactoryInitLostPowerPara( void )
{
	#if( SEL_EVENT_LOST_POWER == YES )
	api_WriteEventAttribute( 0x3011, 0xff, (BYTE *)&LostPowerDefaultOadConst[0], sizeof(LostPowerDefaultOadConst)/sizeof(DWORD) );// 掉电
	#endif
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	api_WriteEventAttribute( 0x300e, 0xff, (BYTE *)&LostSecPowerDefaultOadConst[0], sizeof(LostSecPowerDefaultOadConst)/sizeof(DWORD) );// 辅助电源掉电
	#endif
}


