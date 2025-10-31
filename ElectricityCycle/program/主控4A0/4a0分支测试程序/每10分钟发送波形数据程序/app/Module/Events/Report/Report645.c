//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	    姜文浩
//创建日期	    2018.08.09
//描述        国网645规约上报
//修改记录
//---------------------------------------------------------------------- 

#include "AllHead.h"

#if( SEL_DLT645_2007 == YES )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define FollowReportStatusNum  (7)
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
static __no_init TReport645 Report645;
static __no_init TReportTimes645 ReportTimes645;//主动上报次数
TReport645ResetTime Report645ResetTime;


//引用698主动上报数据
extern TReportMode				ReportMode;					//事件上报标识 bit:事件有效标识
extern __no_init TReportData 	ReportData[MAX_COM_CHANNEL_NUM];//不保存红外通道
extern const BYTE  				Report645ResetTimeConst;   //645上报延时

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//事件Sub序号-主动上报序号-上报模式
//不支持无功组合方式编程
//添加非事件类型跟随上报需要同步更新FollowReportStatusNum宏定义，且放在最前面
static const TReport645Table Report645Table[] =
{
    //esam错误
    {eSTATUS_ESAM_Error,                eSUB_STATUS_ESAM_ERR,           eSTATUS_ESAM_Error,                 eSUB_MODE_ESAM_ERR              },

    //电池电压低
    {eSTATUS_ClockBatteryUnderVol,      eSUB_STATUS_CLOCK_BAT_LOW,      eSTATUS_ClockBatteryUnderVol,       eSUB_MODE_CLOCK_BAT_LOW         },

    //epprom故障
    {eSTATUS_EEPROM_Error,              eSUB_STATUS_MEM_ERR,            eSTATUS_EEPROM_Error,               eSUB_MODE_MEM_ERR               },

    //停电抄表电池欠压
    {eSTATUS_ReadMeterBatteryUnderVol,  eSUB_STATUS_READ_BAT_LOW,       eSTATUS_ReadMeterBatteryUnderVol,   eSUB_MODE_READ_BAT_LOW          },

    //透支状态
    {eSTATUS_TickFlag,                  eSUB_STATUS_OVER_DRAFT,         eSTATUS_TickFlag,                   eSUB_MODE_OVER_DRAFT            },

    //跳闸成功
    {eSTATUS_OpenRelaySucceed,          eSUB_STATUS_PRG_OPEN_RELAY,     eSTATUS_OpenRelaySucceed,           eSUB_MODE_PRG_OPEN_RELAY        },

    //合闸成功
    {eSTATUS_CloseRelaySucceed,         eSUB_STATUS_PRG_CLOSE_RELAY,    eSTATUS_CloseRelaySucceed,          eSUB_MODE_PRG_CLOSE_RELAY       },

	//负荷开关误动作
    #if(SEL_EVENT_RELAY_ERR == YES) //45
    {eSUB_EVENT_RELAY_ERR,              eSUB_STATUS_RELAY_ERR,          eEVENT_RELAY_ERR_NO,                eSUB_MODE_RELAY_ERR             },
    #endif

    //时钟故障
    #if(SEL_EVENT_RTC_ERR == YES)
    {eSUB_EVENT_RTC_ERR,                eSUB_STATUS_RTC_ERR,            eEVENT_RTC_ERR_NO,                  eSUB_MODE_RTC_ERR               },
    #endif
    
    //开表盖
    #if(SEL_EVENT_METERCOVER == YES)
    {eSUB_EVENT_METER_COVER,            eSUB_STATUS_METER_COVER,        eEVENT_METERCOVER_NO,               eSUB_MODE_METER_COVER           },
    #endif
    
    //开端钮盖
    #if(SEL_EVENT_BUTTONCOVER == YES)
    {eSUB_EVENT_BUTTON_COVER,           eSUB_STATUS_BUTTON_COVER,       eEVENT_BUTTONCOVER_NO,              eSUB_MODE_BUTTON_COVER          },
    #endif
    
    //恒定磁场干扰
    #if(SEL_EVENT_MAGNETIC_INT == YES)
    {eSUB_EVENT_MAGNETIC_INT,           eSUB_STATUS_MAGNETIC_INT,       eEVENT_MAGNETIC_INT_NO,             eSUB_MODE_MAGNETIC_INT          },
    #endif
    
    //电源异常
    #if(SEL_EVENT_POWER_ERR == YES)	
    {eSUB_EVENT_POWER_ERR,              eSUB_STATUS_POWER_ERR,          eEVENT_POWER_ERR_NO,                eSUB_MODE_POWER_ERR             },
    #endif

    //A相失压
    #if ( SEL_EVENT_LOST_V == YES )
    {eSUB_EVENT_LOSTV_A,                eSUB_STATUS_LOSTV_A,            eEVENT_LOST_V_NO,                   eSUB_MODE_LOSTV                 },
    #endif
    
    //A相欠压
    #if( SEL_EVENT_WEAK_V == YES )
    {eSUB_EVENT_WEAK_V_A,               eSUB_STATUS_WEAK_V_A,           eEVENT_WEAK_V_NO,                   eSUB_MODE_WEAK_V                },
    #endif
    
    //A相过压
    #if( SEL_EVENT_OVER_V == YES )
    {eSUB_EVENT_OVER_V_A,               eSUB_STATUS_OVER_V_A,           eEVENT_OVER_V_NO,                   eSUB_MODE_OVER_V                },
    #endif
    
    //A相失流
    #if( SEL_EVENT_LOST_I == YES )
    {eSUB_EVENT_LOSTI_A,                eSUB_STATUS_LOSTI_A,            eEVENT_LOST_I_NO,                   eSUB_MODE_LOSTI                 },
    #endif
    
    //A相过流
    #if( SEL_EVENT_OVER_I == YES )
    {eSUB_EVENT_OVER_I_A,               eSUB_STATUS_OVER_I_A,           eEVENT_OVER_I_NO,                   eSUB_MODE_OVER_I                },
    #endif
    
    //A相过载
    #if( SEL_EVENT_OVERLOAD == YES )
    {eSUB_EVENT_OVERLOAD_A,             eSUB_STATUS_OVERLOAD_A,         eEVENT_OVERLOAD_NO,                 eSUB_MODE_OVERLOAD              },
    #endif
    
    //A相功率反向
    #if( SEL_EVENT_BACKPROP == YES )
    {eSUB_EVENT_POW_BACK_A,             eSUB_STATUS_POW_BACK_A,         eEVENT_BACKPROP_NO,                 eSUB_MODE_POW_BACK              },
    #endif
    
    //A相断相
    #if( SEL_EVENT_BREAK == YES )
    {eSUB_EVENT_BREAK_A,                eSUB_STATUS_BREAK_A,            eEVENT_BREAK_NO,                    eSUB_MODE_BREAK                 },
    #endif
    
    //A相断流
    #if( SEL_EVENT_BREAK_I == YES )
    {eSUB_EVENT_BREAK_I_A,              eSUB_STATUS_BREAK_I_A,          eEVENT_BREAK_I_NO,                  eSUB_MODE_BREAK_I               },
    #endif
    
    //B相失压
    #if ( SEL_EVENT_LOST_V == YES )
    {eSUB_EVENT_LOSTV_B,                eSUB_STATUS_LOSTV_B,            eEVENT_LOST_V_NO,                   eSUB_MODE_LOSTV                 },
    #endif
    
    //B相欠压
    #if( SEL_EVENT_WEAK_V == YES )
    {eSUB_EVENT_WEAK_V_B,               eSUB_STATUS_WEAK_V_B,           eEVENT_WEAK_V_NO,                   eSUB_MODE_WEAK_V                },
    #endif
    
    //B相过压
    #if( SEL_EVENT_OVER_V == YES )
    {eSUB_EVENT_OVER_V_B,               eSUB_STATUS_OVER_V_B,           eEVENT_OVER_V_NO,                   eSUB_MODE_OVER_V                },
    #endif
    
    //B相失流
    #if( SEL_EVENT_LOST_I == YES )
    {eSUB_EVENT_LOSTI_B,                eSUB_STATUS_LOSTI_B,            eEVENT_LOST_I_NO,                   eSUB_MODE_LOSTI                 },
    #endif
    
    //B相过流
    #if( MAX_PHASE == 3 )
    #if( SEL_EVENT_OVER_I == YES )
    {eSUB_EVENT_OVER_I_B,               eSUB_STATUS_OVER_I_B,           eEVENT_OVER_I_NO,                   eSUB_MODE_OVER_I                },
    #endif
    #endif
    
    //B相过载
    #if( SEL_EVENT_OVERLOAD == YES )
    {eSUB_EVENT_OVERLOAD_B,             eSUB_STATUS_OVERLOAD_B,         eEVENT_OVERLOAD_NO,                 eSUB_MODE_OVERLOAD              },
    #endif

    //B相功率反向
    #if( SEL_EVENT_BACKPROP == YES )
    {eSUB_EVENT_POW_BACK_B,             eSUB_STATUS_POW_BACK_B,         eEVENT_BACKPROP_NO,                 eSUB_MODE_POW_BACK              },
    #endif
    
    //B相断相
    #if( SEL_EVENT_BREAK == YES )
    {eSUB_EVENT_BREAK_B,                eSUB_STATUS_BREAK_B,            eEVENT_BREAK_NO,                    eSUB_MODE_BREAK                 },
    #endif
    
    //B相断流
    #if( SEL_EVENT_BREAK_I == YES )
    {eSUB_EVENT_BREAK_I_B,              eSUB_STATUS_BREAK_I_B,          eEVENT_BREAK_I_NO,                  eSUB_MODE_BREAK_I               },
    #endif
    
    //C相失压
    #if ( SEL_EVENT_LOST_V == YES )
    {eSUB_EVENT_LOSTV_C,                eSUB_STATUS_LOSTV_C,            eEVENT_LOST_V_NO,                   eSUB_MODE_LOSTV                 },
    #endif
    
    //C相欠压
    #if( SEL_EVENT_WEAK_V == YES )
    {eSUB_EVENT_WEAK_V_C,               eSUB_STATUS_WEAK_V_C,           eEVENT_WEAK_V_NO,                   eSUB_MODE_WEAK_V                },
    #endif
    
    //C相过压
    #if( SEL_EVENT_OVER_V == YES )
    {eSUB_EVENT_OVER_V_C,               eSUB_STATUS_OVER_V_C,           eEVENT_OVER_V_NO,                   eSUB_MODE_OVER_V                },
    #endif
    
    //C相失流

    #if( SEL_EVENT_LOST_I == YES )
    {eSUB_EVENT_LOSTI_C,                eSUB_STATUS_LOSTI_C,            eEVENT_LOST_I_NO,                   eSUB_MODE_LOSTI                 },
    #endif
 
    //C相过流
    #if( MAX_PHASE == 3 )
    #if( SEL_EVENT_OVER_I == YES )
    {eSUB_EVENT_OVER_I_C,               eSUB_STATUS_OVER_I_C,           eEVENT_OVER_I_NO,                   eSUB_MODE_OVER_I                },
    #endif
    #endif
    
    //C相过载
    #if( SEL_EVENT_OVERLOAD == YES )
    {eSUB_EVENT_OVERLOAD_C,             eSUB_STATUS_OVERLOAD_C,         eEVENT_OVERLOAD_NO,                 eSUB_MODE_OVERLOAD              },
    #endif
    
    //C相功率反向
    #if( SEL_EVENT_BACKPROP == YES )
    {eSUB_EVENT_POW_BACK_C,             eSUB_STATUS_POW_BACK_C,         eEVENT_BACKPROP_NO,                 eSUB_MODE_POW_BACK              },
    #endif
    
    //C相断相
    #if( SEL_EVENT_BREAK == YES )
    {eSUB_EVENT_BREAK_C,                eSUB_STATUS_BREAK_C,            eEVENT_BREAK_NO,                    eSUB_MODE_BREAK                 },
    #endif
    
    //C相断流
    #if( SEL_EVENT_BREAK_I == YES )
    {eSUB_EVENT_BREAK_I_C,              eSUB_STATUS_BREAK_I_C,          eEVENT_BREAK_I_NO,                  eSUB_MODE_BREAK_I               },
    #endif
    
    //电压逆相序
    #if( SEL_EVENT_V_REVERSAL == YES )
    {eSUB_EVENT_V_REVERSAL,             eSUB_STATUS_V_REVERSAL,         eEVENT_V_REVERSAL_NO,               eSUB_MODE_V_REVERSAL            },
    #endif
    
    //电流逆相序
    #if( SEL_EVENT_I_REVERSAL == YES )
    {eSUB_EVENT_I_REVERSAL,             eSUB_STATUS_I_REVERSAL,         eEVENT_I_REVERSAL_NO,               eSUB_MODE_I_REVERSAL            },
    #endif
    
    //电压不平衡
    #if(SEL_EVENT_V_UNBALANCE == YES)
    {eSUB_EVENT_V_UNBALANCE,            eSUB_STATUS_V_UNBALANCE,        eEVENT_V_UNBALANCE_NO,              eSUB_MODE_V_UNBALANCE           },
    #endif
    
    //电流不平衡
    #if(SEL_EVENT_I_UNBALANCE == YES)
    {eSUB_EVENT_I_UNBALANCE,            eSUB_STATUS_I_UNBALANCE,        eEVENT_I_UNBALANCE_NO,              eSUB_MODE_I_UNBALANCE           },
    #endif
    
    //辅助电源失电
    #if( SEL_EVENT_LOST_SECPOWER == YES )
    {eSUB_EVENT_LOST_SECPOWER,          eSUB_STATUS_LOST_SECPOWER,      eEVENT_LOST_SECPOWER_NO,            eSUB_MODE_LOST_SECPOWER         },
    #endif
    
    //掉电
    #if( SEL_EVENT_LOST_POWER == YES )
    {eSUB_EVENT_LOST_POWER,             eSUB_STATUS_LOST_POWER,         eEVENT_LOST_POWER_NO,               eSUB_MODE_LOST_POWER            },
    #endif
    
    //需量超限（正向）
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_PA_DEMAND_OVER,         eSUB_STATUS_DEMAND_OVER,        eEVENT_PA_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //需量超限（反向）
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_NA_DEMAND_OVER,         eSUB_STATUS_DEMAND_OVER,        eEVENT_NA_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //需量超限（第一象限）
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_QI_DEMAND_OVER,         eSUB_STATUS_DEMAND_OVER,        eEVENT_RE_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //需量超限（第二象限）
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_QII_DEMAND_OVER,        eSUB_STATUS_DEMAND_OVER,        eEVENT_RE_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //需量超限（第三象限）
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_QIII_DEMAND_OVER,       eSUB_STATUS_DEMAND_OVER,        eEVENT_RE_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //需量超限（第四象限）
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_QIV_DEMAND_OVER,        eSUB_STATUS_DEMAND_OVER,        eEVENT_RE_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //总功率因数超限
    #if( SEL_EVENT_COS_OVER == YES )
    {eSUB_EVENT_COS_OVER_ALL,           eSUB_STATUS_COS_OVER_ALL,       eEVENT_COS_OVER_NO,                 eSUB_MODE_COS_OVER_ALL          },
    #endif
    
    //电流严重不平衡
    #if(SEL_EVENT_I_S_UNBALANCE == YES)
    {eSUB_EVENT_S_I_UNBALANCE,          eSUB_STATUS_S_I_UNBALANCE,      eEVENT_I_S_UNBALANCE_NO,            eSUB_MODE_S_I_UNBALANCE         },
    #endif
    
    //潮流反向（总功率反向）
    #if( SEL_EVENT_BACKPROP == YES )
    {eSUB_EVENT_POW_BACK_ALL,           eSUB_STATUS_BACKPROPALL,        eEVENT_BACKPROP_NO,                 eSUB_MODE_BACKPROPALL           },
    #endif
    
    //全失压
    #if( SEL_EVENT_LOST_ALL_V == YES )
    {eSUB_EVENT_LOST_ALL_V,             eSUB_STATUS_LOST_ALL_V,         eEVENT_LOST_POWER_NO,               eSUB_MODE_LOST_POWER            },
    #endif
    
    // 拓扑事件
    #if( SEL_TOPOLOGY == YES )
    {eSUB_EVENT_TOPU,           eSUB_STATUS_TOPO_IDENTI_RESULT,         eEVENT_TOPU_NO,               eSUB_MODE_TOPO_IDENTI_RESULT    },
    #endif
    //编程
    {eSUB_EVENT_PRG_RECORD,             eSUB_STATUS_PRG_RECORD,         ePRG_RECORD_NO,                     eSUB_MODE_PRG_RECORD            },

    //电表清零
    #if( SEL_PRG_INFO_CLEAR_METER == YES )
    {eSUB_EVENT_PRG_CLEAR_METER,        eSUB_STATUS_PRG_CLEAR_METER,    ePRG_CLEAR_METER_NO,                eSUB_MODE_PRG_CLEAR_METER       },
    #endif
    
    //需量清零
    #if( SEL_PRG_INFO_CLEAR_MD == YES )
    {eSUB_EVENT_PRG_CLEAR_MD,           eSUB_STATUS_PRG_CLEAR_MD,       ePRG_CLEAR_MD_NO,                   eSUB_MODE_PRG_CLEAR_MD          },
    #endif
    
    //事件清零
    #if( SEL_PRG_INFO_CLEAR_EVENT == YES )
    {eSUB_EVENT_PRG_CLEAR_EVENT,        eSUB_STATUS_PRG_CLEAR_EVENT,    ePRG_CLEAR_EVENT_NO,                eSUB_MODE_PRG_CLEAR_EVENT       },
    #endif
    
    //校时
    #if( SEL_PRG_INFO_ADJUST_TIME == YES )
    {eSUB_EVENT_PRG_ADJUST_TIME,        eSUB_STATUS_PRG_ADJUST_TIME,    ePRG_ADJUST_TIME_NO,                eSUB_MODE_PRG_ADJUST_TIME       },
    #endif
    
    //时段表编程
    #if( SEL_PRG_INFO_TIME_TABLE == YES )
    {eSUB_EVENT_PRG_TIME_TABLE,         eSUB_STATUS_PRG_TIME_TABLE,     ePRG_TIME_TABLE_NO,                 eSUB_MODE_PRG_TIME_TABLE        },
    #endif
    
    //时区表编程
    #if( SEL_PRG_INFO_TIME_AREA == YES )
    {eSUB_EVENT_PRG_TIME_AREA,          eSUB_STATUS_PRG_TIME_AREA,      ePRG_TIME_AREA_NO,                  eSUB_MODE_PRG_TIME_AREA         },
    #endif
    
    //周休日编程
    #if( SEL_PRG_INFO_WEEK == YES )
    {eSUB_EVENT_PRG_WEEKEND,            eSUB_STATUS_PRG_WEEKEND,        ePRG_WEEKEND_NO,                    eSUB_MODE_PRG_WEEKEND           },
    #endif
    
    //节假日编程
    #if( SEL_PRG_INFO_HOLIDAY == YES )
    {eSUB_EVENT_PRG_HOLIDAY,            eSUB_STATUS_PRG_HOLIDAY,        ePRG_HOLIDAY_NO,                    eSUB_MODE_PRG_HOLIDAY           },
    #endif
    
    //有功组合方式编程
    #if( SEL_PRG_INFO_P_COMBO == YES )
    {eSUB_EVENT_ePRG_P_COMBO,           eSUB_STATUS_PRG_P_COMBO,        ePRG_P_COMBO_NO,                    eSUB_MODE_PRG_P_COMBO           },
    #endif
    //无功组合方式1编程
    //{eSUB_EVENT_PRG_Q_COMBO,            eSUB_STATUS_PRG_Q_COMBO,        ePRG_Q_COMBO_NO,                    eSUB_MODE_PRG_Q_COMBO}
    //无功组合方式2编程
    //{eSUB_EVENT_PRG_Q_COMBO,            eSUB_STATUS_PRG_Q2_COMBO,       ePRG_Q_COMBO_NO,                    eSUB_MODE_PRG_Q_COMBO}

    //结算日编程
    #if( SEL_PRG_INFO_CLOSING_DAY == YES )
    {eSUB_EVENT_PRG_CLOSING_DAY,        eSUB_STATUS_PRG_CLOSING_DAY,    ePRG_CLOSING_DAY_NO,                eSUB_MODE_PRG_CLOSING_DAY       },
    #endif
    
    //密钥更新
    #if( SEL_PRG_UPDATE_KEY == YES )
    {eSUB_EVENT_PRG_UPDATE_KEY,         eSUB_STATUS_PRG_UPDATE_KEY,     ePRG_UPDATE_KEY_NO,                 eSUB_MODE_PRG_UPDATE_KEY        },
    #endif
};
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 更新状态字相关数据在RAM中的CRC校验值
//
//参数:	  
//			Type[in]		- 处理方式：WRITE--写 		READ--读
//			pBuf[in/out]	- [in]--电表运行特征字1缓存				[out]--电表运行特征字1Bit2缓存
//
//返回值:		ReturnLen		- 数据长度 或 FALSE
//
//备注:
//---------------------------------------------------------------	
static void UpdateReport645CRC32(void)
{
    Report645.CRC32 = lib_CheckCRC32( Report645.BakFlag, sizeof(TReport645)-sizeof(DWORD) );
    ReportTimes645.CRC32 = lib_CheckCRC32( (BYTE*)&ReportTimes645.Times[0][0], sizeof(TReportTimes645)-sizeof(DWORD) );

}

//-----------------------------------------------
//函数功能:     645上报crc检测
//
//参数:	    无
//			
//
//返回值:		ReturnLen		- 数据长度 或 FALSE
//
//备注:
//---------------------------------------------------------------	
static void CheckReport645(void)
{
    WORD Result;
    
    if( Report645.CRC32 != lib_CheckCRC32( Report645.BakFlag, sizeof(TReport645)-sizeof(DWORD) ) )
	{
	    memset( Report645.BakFlag, 0x00,sizeof(TReport645) );  
        UpdateReport645CRC32();
	}

    if( ReportTimes645.CRC32 != lib_CheckCRC32( (BYTE*)&ReportTimes645.Times[0][0], sizeof(TReportTimes645)-sizeof(DWORD) ) )
    {
        memset( ReportTimes645.Times, 0x00, sizeof(TReportTimes645) );//清零次数 因为分通道无法准确置1，因此在读取时进行置1判断
        UpdateReport645CRC32();
    } 

    if( Report645ResetTime.CRC32 != lib_CheckCRC32( (BYTE*)&Report645ResetTime.Time, sizeof(TReport645ResetTime)-sizeof(DWORD) ) )
	{
        Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645ResetTime.Time), sizeof(TReport645ResetTime), (BYTE*)&Report645ResetTime.Time);
        if( Result == FALSE )
        {
           Report645ResetTime.Time = 30;//默认为30分钟
           Report645ResetTime.CRC32 = lib_CheckCRC32( (BYTE*)&Report645ResetTime.Time, sizeof(TReport645ResetTime)-sizeof(DWORD) );
        }    
	}
}

//-----------------------------------------------
//函数功能:      设置主动上报复位时间
//
//参数:        BYTE ResetTime[in]
//
//返回值:		
//
//备注:
//---------------------------------------------------------------
WORD  api_SetReportResetTime( BYTE ResetTime )
{
    WORD Result;

    Report645ResetTime.Time = ResetTime;
    Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645ResetTime), sizeof(TReport645ResetTime), (BYTE*)&Report645ResetTime.Time);
    if( Result == FALSE )
    {
        Report645ResetTime.Time = 30;
        Report645ResetTime.CRC32 = lib_CheckCRC32( (BYTE*)&Report645ResetTime.Time, sizeof(TReport645ResetTime)-sizeof(DWORD) );
    }
    return Result;
}

//-----------------------------------------------
//函数功能:      读取主动上报复位时间
//
//参数:        void
//
//返回值:		 BYTE 复位时间  
//
//备注:
//---------------------------------------------------------------
BYTE  api_GetReportResetTime( void )
{
    return Report645ResetTime.Time; 
}

//-----------------------------------------------
//函数功能: 增加上报次数
//
//参数:      BYTE Ch[in]          通道选择
//
//         BYTE Mode[in]        0x00:次数累计加1
//                              0x55:次数重置为1
//
//         BYTE SubEventIndex   增加次数的事件索引
//
//返回值:		Result		- TRUE：启用 		FALSE：不启用
//
//备注:
//---------------------------------------------------------------
WORD  api_AddReportTimes( BYTE Ch, BYTE Mode, BYTE SubEventIndex)
{
    BYTE i;

    if( SubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
    {
        return FALSE;
    }
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return FALSE;
    }
   
    if( Mode == 0x00 )
    {
        ReportTimes645.Times[Ch][SubEventIndex]++;
    }
    else
    {
        ReportTimes645.Times[Ch][SubEventIndex] = 1;
    }
    
    UpdateReport645CRC32();

    return TRUE;
}

//-----------------------------------------------
//函数功能: 清上报次数
//
//参数:      BYTE Mode[in]        0x00:清指定事件
//                              0x55:全清
//         BYTE SubEventIndex   增加次数的事件索引
//
//返回值:		Result		- TRUE：启用 		FALSE：不启用
//
//备注:
//---------------------------------------------------------------
WORD  ClearReport645Times( BYTE Ch, BYTE Mode, BYTE SubEventIndex)
{
    BYTE i;

    if( SubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
    {
        return FALSE;
    }
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return FALSE;
    }
    
    if( Mode == 0x00 )
    {
        ReportTimes645.Times[Ch][SubEventIndex] = 0;
    }
    else
    {
        memset( (BYTE*)&ReportTimes645.Times[0][0], 0x00, sizeof(TReportTimes645) );
    }
    
    UpdateReport645CRC32();

    return TRUE;
}

//-----------------------------------------------
//函数功能: 主动上报状态字读取
//
//参数:		BYTE Ch[in]  - 通道选择
//          Type[in]	 - 读取方式 eREAD_STATUS_PROC--规约读取	         	eREAD_STATUS_OTHER--非规约读取
//			pBuf[out]	 - 主动上报状态字缓存
//返回值:		ReturnLen	 - 数据长度		0：错误
//
//备注:
//---------------------------------------------------------------
WORD api_ReadReportStatusByte( BYTE Ch, eREAD_STATUS_TYPE Type, BYTE *pBuf	)
{
	WORD ReturnLen = 0;
	BYTE i,j,Num;
    BYTE Status[RPT_STATUS_LEN];
	BYTE Times[eREPORT_STATUS_TOTAL];    //主动上报状态字新增次数
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return FALSE;
    }

    memset( Status, 0x00, sizeof(Status) );
    memset( Times, 0x55, sizeof(Times) );//不用0xff 因为非事件上报，次数传0xff
    Num = (sizeof(Report645Table)/sizeof(TReport645Table));
    
    for( i=0; i<eSUB_EVENT_STATUS_TOTAL; i++ )//事件上报进行状态字转换
    {
        if( ReportData[Ch].Index[i] != 0xff )
        {
            for( j=FollowReportStatusNum; j<Num; j++ )
            {
                if( ReportData[Ch].Index[i] == Report645Table[j].Event_SubIndex )
                {
                    Status[(Report645Table[j].Status_Index/8)] |= (0x01<<(Report645Table[j].Status_Index%8));//置位状态字
                    Times[Report645Table[j].Status_Index] = ReportTimes645.Times[Ch][Report645Table[j].Event_SubIndex];//获取长度
                    break;
                }
            }
        }
    }

    for( i=0; i<eMAX_FOLLOW_REPORT_STATUS_NUM; i++ )//跟随上报状态字转换
    {
        if( (ReportData[Ch].Status[i/8]&(1<<i%8)) != 0 )
        {
            for( j=0; j<FollowReportStatusNum; j++ )
            {
                if( Report645Table[j].Event_SubIndex == i )
                {
                    Status[(Report645Table[j].Status_Index/8)] |= (0x01<<(Report645Table[j].Status_Index%8));//置位状态字
                    Times[Report645Table[j].Status_Index] = 0xff;//获取长度
                    break;
                }
            }
        }
    }
    
	memcpy( pBuf, Status, RPT_STATUS_LEN );	//从RAM中读取状态字

	ReturnLen = RPT_STATUS_LEN;
    
	if( Type == eREAD_STATUS_PROC )	//规约读取
	{
		pBuf[ReturnLen] = 0xAA;

		ReturnLen += 1;

		//从RAM中读新增次数，并组包
		for( i = 0; i < eREPORT_STATUS_TOTAL; i++)
		{
			if( Times[i] != 0x55 )
			{
			    if( Times[i] == 0 )
			    {
				    pBuf[ReturnLen] = 1;
			    }
			    else
			    {
                    pBuf[ReturnLen] = Times[i];
			    }
				ReturnLen += 1;
			}
		}

        pBuf[ReturnLen] = 0xAA;

        ReturnLen += 1;
        
        Report645.BakFlag[Ch] = 0x55;
        memcpy( (BYTE*)&Report645.StatusBak[Ch][0], Status, RPT_STATUS_LEN );//保存备份 
        Report645.ResetTime[Ch] = Report645ResetTime.Time;//启动倒计时
	}
	else if( Type == eREAD_STATUS_OTHER )//非规约读取，不读新增次数
	{
		memcpy( pBuf, Status, RPT_STATUS_LEN );	//从RAM中读取状态字
		ReturnLen = RPT_STATUS_LEN;
	}
	
    UpdateReport645CRC32();

	return ReturnLen;
}

//-----------------------------------------------
//函数功能:     主动上报状态字读取
//
//参数:	    BYTE *pBuf[in]  输入buf
//
//备注:
//---------------------------------------------------------------
WORD api_ReadReportStatusMode( BYTE *pBuf	)
{
    BYTE i,j,Num;
    BYTE Mode[RPT_MODE_LEN+20];
    
    Num = (sizeof(Report645Table)/sizeof(TReport645Table));

    memset( Mode, 0x00, sizeof(Mode) );
    
    for( i=0; i<eNUM_OF_EVENT_PRG; i++ )//事件上报模式字转换
    {
        if( ReportMode.byReportMode[i] !=0 )
        {
            for( j=FollowReportStatusNum; j<Num; j++ ) 
            {
                if( Report645Table[j].Event_Index == i )
                {
                    Mode[(Report645Table[j].Mode_Index/8)] |= (0x01<<(Report645Table[j].Mode_Index%8));//置位状态字
                    break;
                }
            }
        }
    }

   for( i=0; i<eMAX_FOLLOW_REPORT_STATUS_NUM; i++ )//跟随上报模式字转换
   {
        if( (ReportMode.byFollowReportMode[i/8]&(1<<i%8)) != 0 )
        {
            for( j=0; j<FollowReportStatusNum; j++ )
            {
                if( Report645Table[j].Event_Index == i )
                {
                    Mode[(Report645Table[j].Mode_Index/8)] |= (0x01<<(Report645Table[j].Mode_Index%8));//置位状态字
                    break;
                }
            }

        }

   }

   memcpy( pBuf, Mode, RPT_MODE_LEN );

   return RPT_MODE_LEN;
   
}

//-----------------------------------------------
//函数功能: 获取主动上报状态字是否启用后续帧
//
//参数:      BYTE Ch[in] 通道
//			
//
//返回值:	
//
//备注:		
//---------------------------------------------------------------
WORD api_GetReportReqFlag( BYTE Ch )
{
    BYTE Buf[RPT_STATUS_LEN+20];
    BYTE i;
    WORD Result;

    Result = FALSE;
	if( api_GetRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS ) == TRUE )//判断特征字是否允许开启上报
	{
        api_ReadReportStatusByte( Ch, eREAD_STATUS_OTHER, Buf	);
        for( i=0; i<RPT_STATUS_LEN; i++ )
        {
            if( Buf[i] != 0 )
            {
                break;
            }

        }

        if( i != RPT_STATUS_LEN )
        {
            Result = TRUE;
        }
	}

	return Result;
}

//-----------------------------------------------
//函数功能: 主动上报状态字复位
//
//参数:		Ch[in]      - 通道选择
//          pBuf[in]	- 复位缓存
//
//返回值:		Result	- TRUE：正确		FALSE：错误
//
//备注:
//---------------------------------------------------------------
WORD api_ResetReportStatusByte( BYTE Ch, BYTE *pBuf )
{
	BYTE i,j,k,Num;
	BYTE TmpBuf[RPT_STATUS_LEN];
	BYTE StatusBuf[RPT_STATUS_LEN];
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return FALSE;
    }
    
    if( Report645.BakFlag[Ch] != 0x55 )
    {
        return FALSE;
    }

    api_ReadReportStatusByte( Ch, eREAD_STATUS_OTHER, StatusBuf );//获取当前通道上报状态字
	
	for( i = 0; i < RPT_STATUS_LEN; i++)
	{
		TmpBuf[i] = ~pBuf[i];

		for( j = 0; j < 8 ; j++ )
		{
			//某1位状态字未被抄读 且 置位 且 收到对该位复位的复位命令
			if( ((Report645.StatusBak[Ch][i]&(0x01<<j))==0) && ((StatusBuf[i]&(0x01<<j))!=0) && ((TmpBuf[i]&(0x01<<j))!=0) )
			{
				break;
			}
		}

        if( j < 8 )
        {
            break;
        }
	}

	//复位状态字有效时，复位被抄读的状态字
	if( i == RPT_STATUS_LEN )
	{
		//清状态字
		for( i = 0; i < RPT_STATUS_LEN; i++)
		{
			Report645.StatusBak[Ch][i] &= pBuf[i];
		}
		
        Num = (sizeof(Report645Table)/sizeof(TReport645Table));
		//清新增次数
		for( i = 0; i < RPT_STATUS_LEN*8; i++)
		{
			if( (~pBuf[i/8]) & (0x01<<(i%8)) )//要复位此位
			{
                for( j=FollowReportStatusNum; j<Num; j++ )//事件上报复位
                {
                    if( Report645Table[j].Status_Index == i )
                    {
                        ClrReportIndexChannel( Ch, Report645Table[j].Event_SubIndex );
                        ClearReport645Times(Ch,0x00, Report645Table[j].Event_SubIndex);
                        break;
                    }
                }
        
                for( j=0; j<FollowReportStatusNum; j++ )//跟随上报复位
                {
                    if( Report645Table[j].Status_Index == i )
                    {
					    ReportData[Ch].Status[Report645Table[j].Event_SubIndex/8] &= ~(1<<Report645Table[j].Event_SubIndex%8);
                        break;
                    }
                }       
			}
		}

		ReportData[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[Ch],sizeof(TReportData)-sizeof(DWORD));		
	}
	else
	{
		return FALSE;
	}
	
    Report645.BakFlag[Ch] = 0;

	UpdateReport645CRC32();
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 主动上报分钟任务
//
//参数:		（无）
//
//返回值:		（无）
//
//备注:		状态字全为0时，EVENTOUT输出高阻态，否则拉低
//---------------------------------------------------------------
void api_Report645MinTask( void )
{
    BYTE i,j;
    BYTE Buf[RPT_STATUS_LEN+20];
    
    CheckReport645();//全局变量进行参数校验

    for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
    {
        if( Report645.ResetTime[i] !=0 )
        {
            Report645.ResetTime[i]--;
            
            if( Report645.ResetTime[i] == 0 )//进行复位主动上报状态字
            {
                for( j=0; j<RPT_STATUS_LEN; j++ )
                {
                    Buf[j]  = ~Report645.StatusBak[i][j];
                }
                api_ResetReportStatusByte( i, Buf );
            }
        }
    }
    
    UpdateReport645CRC32();
}

//-----------------------------------------------
//函数功能: 结束低功耗时从EEPROM恢复主动上报状态字、新增次数、标志位、复位倒计时、故障标志位
//
//参数:		无
//			
//返回值:		无
//
//备注:
//---------------------------------------------------------------
void api_PowerUpReport645( void )
{
	DWORD Addr;
	WORD Result;

	//获取复位上报延时
    Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645ResetTime.Time), sizeof(TReport645ResetTime), (BYTE*)&Report645ResetTime.Time);
    if( Result == FALSE )
    {
       Report645ResetTime.Time = 30;//默认为30分钟
       Report645ResetTime.CRC32 = lib_CheckCRC32( (BYTE*)&Report645ResetTime.Time, sizeof(TReport645ResetTime)-sizeof(DWORD) );
    }    

	if( Report645.CRC32 != lib_CheckCRC32( Report645.BakFlag, sizeof(TReport645)-sizeof(DWORD) ) )
	{
        Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645.BakFlag), sizeof(TReport645), Report645.BakFlag);
        if( Result == FALSE )
        {
	        memset( (BYTE*)&Report645.BakFlag, 0x00,sizeof(TReport645) );
        }
        
	}

    if( ReportTimes645.CRC32 != lib_CheckCRC32( (BYTE*)&ReportTimes645.Times[0][0], sizeof(TReportTimes645)-sizeof(DWORD) ) )
    {
        memset( (BYTE*)&ReportTimes645.Times[0][0], 0x00, sizeof(TReportTimes645) );//清零次数 因为分通道无法准确置1，因此在读取时进行置1判断
    } 
    
    UpdateReport645CRC32();
}
//-----------------------------------------------
//函数功能: 掉电保存故障标志
//
//参数:		Type[in]	-
//			Index[in]	-
//返回值:		无
//
//备注:
//---------------------------------------------------------------
void api_PowerDownReport645( void )
{
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645.BakFlag), sizeof(TReport645), Report645.BakFlag);
}


//-----------------------------------------------
//函数功能:     初始化主动上报所有数据与参数
//
//参数:		无
//			
//返回值:		无
//
//备注:
//---------------------------------------------------------------
void api_FactoryInitReport645( void )
{
    WORD Result;
    
    ClearReport645Times( 0, 0x55, 0 );
    memset( (BYTE*)&Report645.BakFlag, 0x00,sizeof(TReport645) );
    api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645.BakFlag), sizeof(TReport645), Report645.BakFlag);

    Report645ResetTime.Time = Report645ResetTimeConst;
    Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645ResetTime), sizeof(TReport645ResetTime), (BYTE*)&Report645ResetTime.Time);
    
    UpdateReport645CRC32();
}
#endif  //#if( SEL_DLT645_2007 == YES )
