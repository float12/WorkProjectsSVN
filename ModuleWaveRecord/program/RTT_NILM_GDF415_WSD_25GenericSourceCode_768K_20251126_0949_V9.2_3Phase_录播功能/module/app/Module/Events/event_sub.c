#include "AllHead.h"
#include "event.h"

// 电能质量监测量越限事件记录判断阈值
#define THR_THD_VOL             500         // 电压谐波总畸变率超限阈值 (2位小数)   5%
#define THR_THD_CUR             2000        // 电流谐波总畸变率超限阈值 (2位小数)   20%
#define THR_HARMCONTENT_VOL     500         // 谐波电压含量超限阈值 (2位小数)       // TODO: 规范未规定阈值
#define THR_HARMCONTENT_CUR     2000        // 谐波电流含量超限阈值 (2位小数)       // TODO: 规范未规定阈值
#define THR_HARMCONTENT_POWER   100         // 谐波功率含量超限阈值 (2位小数)       // TODO: 规范未规定阈值
#define THR_IHARMCONTENT_VOL    500         // 间谐波电压含量超限阈值 (2位小数)     // TODO: 规范未规定阈值
#define THR_IHARMCONTENT_CUR    2000        // 间谐波电压含量超限阈值 (2位小数)     // TODO: 规范未规定阈值
#define THR_IHARMCONTENT_POWER  100         // 间谐波电压含量超限阈值 (2位小数)     // TODO: 规范未规定阈值
#define THR_PST                 10000       // 电压短时闪变超限阈值 (4位小数)       1.0
#define THR_PLT                 8000        // 电压长时闪变超限阈值 (4位小数)       0.8
#define THR_VOL_DIP             9000        // 电压暂降阈值 (2位小数)               90%
#define THR_VOL_SWL             11000       // 电压暂升阈值 (2位小数)               110%
#define THR_VOL_INT             1000        // 电压短时中断阈值 (2位小数)           10%

BYTE gbyJLChipFlag=0;
TEventPara gEventPara;
TReportMode gReportMode;


BOOL GetDefaultThresholdValue(BYTE byType)
{
	TEventPara tempEventPara;
	TReportMode tempReportMode;
	WORD Result = FALSE;
	
    Result = api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&tempEventPara);
	if(byType)
		Result = 0;

	if(Result == 0)
	{
		tempEventPara.sHrmUDistorThreshold = THR_THD_VOL;
		tempEventPara.HrmUDistorDelayTime = 60;
		
		tempEventPara.sHrmIDistorThreshold = THR_THD_CUR;
		tempEventPara.HrmIDistorDelayTime = 60;

		tempEventPara.wShortFlkThreshold = THR_PST;
		tempEventPara.wShortFlkDelayTime = 600;

		tempEventPara.wLongFlkThreshold = THR_PLT;
		tempEventPara.wLongFlkDelayTime = 600;

		tempEventPara.wSagUThreshold = THR_VOL_DIP;
		tempEventPara.wSagUDelayTime = 10;

		tempEventPara.wSwellUThreshold = THR_VOL_SWL;
		tempEventPara.wSwellUDelayTime = 10;

		tempEventPara.wInterruptUThreshold = THR_VOL_INT;
		tempEventPara.wInterruptUDelayTime = 10;
/////////////////////
		tempEventPara.sHrmUContentThreshold = THR_HARMCONTENT_VOL;			//谐波电压含量超限阈值  long（单位：%，换算：-2），
		tempEventPara.HrmUContentDelayTime = 60;			//延时判定时间 		 unsigned（单位：s，换算：0）
		
		tempEventPara.sHrmIContentThreshold = THR_HARMCONTENT_CUR;			//谐波电流含量超限阈值  long（单位：%，换算：-2），
		tempEventPara.HrmIContentDelayTime = 60;			//延时判定时间 		 unsigned（单位：s，换算：0）
		
		tempEventPara.sHrmPContentThreshold = THR_HARMCONTENT_POWER;			//谐波功率超限超限阈值  long（单位：%，换算：-2），
		tempEventPara.HrmPContentDelayTime = 60;			//延时判定时间 		 unsigned（单位：s，换算：0）
		
		tempEventPara.sInterHrmUContentThreshold = THR_IHARMCONTENT_VOL;		//间谐波电压含量超限阈值  long（单位：%，换算：-2），
		tempEventPara.InterHrmUContentDelayTime = 60;		//延时判定时间 		 unsigned（单位：s，换算：0）
		
		tempEventPara.sInterHrmIContentThreshold = THR_IHARMCONTENT_CUR;		//间谐波电流含量超限阈值  long（单位：%，换算：-2），
		tempEventPara.InterHrmIContentDelayTime = 60;		//延时判定时间 		 unsigned（单位：s，换算：0）
		
		tempEventPara.sInterHrmPContentThreshold = THR_IHARMCONTENT_POWER;		//间谐波功率超限阈值  long（单位：%，换算：-2），
		tempEventPara.InterHrmPContentDelayTime = 60;		//延时判定时间 		 unsigned（单位：s，换算：0）
		tempEventPara.SPIDelayTime = 60;		//延时判定时间 		 unsigned（单位：s，换算：0）
	
		tempEventPara.nUartBps = UART_DEFAULT_BAUD;
		tempEventPara.wBodongThreshold = 500;
	}
	
    Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.EventPara), sizeof(TEventPara), (BYTE *)&tempEventPara);

	memcpy(&gEventPara, &tempEventPara, sizeof(TEventPara));

	Result = api_VReadSafeMem(GET_CONTINUE_ADDR(EventConRom.ReportMode), sizeof(TReportMode), (BYTE *)&tempReportMode);
	if(byType)
		Result = 0;

	if(Result == 0)
	{
		memset(&tempReportMode,0,sizeof(TReportMode));// 默认全部关掉

		// 下面几个事件打开，不上报vvvvvvvvvvvvv
		tempReportMode.byEventValid[eSUB_EVENT_UPDATE_NO]=0x01;
		tempReportMode.byReportFlag[eSUB_EVENT_UPDATE_NO]=0;
		tempReportMode.byReportMode[eSUB_EVENT_UPDATE_NO]=0;

		tempReportMode.byEventValid[eSUB_EVENT_METER_CLEAR_NO]=0x01;
		tempReportMode.byReportFlag[eSUB_EVENT_METER_CLEAR_NO]=0;
		tempReportMode.byReportMode[eSUB_EVENT_METER_CLEAR_NO]=0;

		tempReportMode.byEventValid[eSUB_EVENT_JLCHIP_FAIL_NO]=0x01;
		tempReportMode.byReportFlag[eSUB_EVENT_JLCHIP_FAIL_NO]=0;
		tempReportMode.byReportMode[eSUB_EVENT_JLCHIP_FAIL_NO]=0;

		tempReportMode.byEventValid[eSUB_EVENT_ADJUST_TIME_NO] = 0x01;
		tempReportMode.byReportFlag[eSUB_EVENT_ADJUST_TIME_NO] = 0;
		tempReportMode.byReportMode[eSUB_EVENT_ADJUST_TIME_NO] = 0;

		#if(PROTOCOL_VERSION == 25)	
		tempReportMode.byEventValid[eSUB_EVENT_UNKNOWN_DEV_NO] = 0x01;
		tempReportMode.byReportFlag[eSUB_EVENT_UNKNOWN_DEV_NO] = 0;
		tempReportMode.byReportMode[eSUB_EVENT_UNKNOWN_DEV_NO] = 0;

		tempReportMode.byEventValid[eSUB_EVENT_SPECIFIC_DEV_NO] = 0x01;
		tempReportMode.byReportFlag[eSUB_EVENT_SPECIFIC_DEV_NO] = 0;
		tempReportMode.byReportMode[eSUB_EVENT_SPECIFIC_DEV_NO] = 0;
		#endif
		// 事件打开，不上报^^^^^^^^^^^^^^

		tempReportMode.g_EnableFollowReportFlag = 0;
		tempReportMode.g_EnableAutoReportFlag = 0;
	}
	
	Result = api_VWriteSafeMem(GET_CONTINUE_ADDR(EventConRom.ReportMode), sizeof(TReportMode), (BYTE *)&tempReportMode);

	memcpy(&gReportMode, &tempReportMode, sizeof(TReportMode));
	return Result;
}

BYTE EVENT_STATU;

BOOL UpdateThresholdValue(BYTE* tempEventPara)
{
	memcpy(&gEventPara, tempEventPara, sizeof(TEventPara));
    return TRUE;
}
//电能表计量芯片故障事件
static const DWORD JLChipFailOad[] =
{
	0,		// 个数
				
};
extern BYTE LED_SPIEvent;
BYTE GetJLChipFailStatus(BYTE Phase)
{
	static int nCount=0;

	if(LED_SPIEvent == 0)
			nCount++;
	else
			nCount=0;

	if(LED_SPIEvent == 1)
			LED_SPIEvent = 0;
		
	EVENT_STATU = 0;
	if(nCount > 5)    // 
	{
		api_SetSysStatus(eSYS_STATUS_METERING_FAULT2);
		EVENT_STATU = 1;
	}
	else
	{
		api_ClrSysStatus(eSYS_STATUS_METERING_FAULT2);
	}
	return EVENT_STATU | api_GetSysStatus(eSYS_STATUS_METERING_FAULT);
	// return api_GetSysStatus(eSYS_STATUS_METERING_FAULT);
}

void FactoryInitJLChipFailPara( void )
{
	api_WriteEventAttribute( OAD_JLCHIP_FAIL, 0xFF, (BYTE *)&JLChipFailOad[0], sizeof(JLChipFailOad)/sizeof(DWORD) );
}

//电能表清零事件
static const DWORD MeterClearOad[] =
{
	0,		// 个数		
};

BYTE GetMeterClearStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitMeterClearPara( void )
{
	api_WriteEventAttribute( OAD_METER_CLEAR, 0xFF, (BYTE *)&MeterClearOad[0], sizeof(MeterClearOad)/sizeof(DWORD) );
}

// 校时事件
static const DWORD AdjustTimeOad[] =
{
		0, // 个数
};

void FactoryInitAdjustTimePara(void)
{
	api_WriteEventAttribute(OAD_ADJUST_TIME, 0xFF, (BYTE *)&AdjustTimeOad[0], sizeof(AdjustTimeOad) / sizeof(DWORD));
}
#if(PROTOCOL_VERSION == 25)	
//辨识未知设备事件
static const DWORD UnknownDevOad[] =
{
	1, // 个数

	0x48002800,		//事件发生前的负荷特征数据
};

void FactoryInitUnknownDevPara(void)
{
	api_WriteEventAttribute(OAD_UNKNOWN_DEV, 0xFF, (BYTE *)&UnknownDevOad[0], sizeof(UnknownDevOad) / sizeof(DWORD));
}

//辨识未知设备事件
static const DWORD SpecificDevOad[] =
{
	2, // 个数
	
	0x48012201,		//事件发生前的设备类别
	0x48012203,		//事件发生前的设备有功功率
};

void FactoryInitSpecificDevPara(void)
{
	api_WriteEventAttribute(OAD_SPECIFIC_DEV, 0xFF, (BYTE *)&SpecificDevOad[0], sizeof(SpecificDevOad) / sizeof(DWORD));
}
#endif //#if(PROTOCOL_VERSION == 25)	
//升级事件
static const DWORD UpdateAppOad[] =
{
	5,		// 个数
		
	0x40272300,		//—升级操作类型  //wxy 2022.11.08 由40270300 改为 40272300
	0xF0012500,		//—下载方的标识
	0xF4022204,		//—升级前软件版本号
	0xF4028204,		//—升级后软件版本号
	// 0xF4028202,		//—升级应用名称 //zh
	0x40278200,		//—升级结果		
	
};


BYTE GetUpdateAppStatus(BYTE Phase)
{
	return 0;

}

void FactoryInitUpdateAppPara( void )
{
	api_WriteEventAttribute( OAD_APP_UPDATE, 0xFF, (BYTE *)&UpdateAppOad[0], sizeof(UpdateAppOad)/sizeof(DWORD) );
}
//@@@@@@@@测试@@@@@@@@

//电压谐波总畸变率超限事件
static const DWORD UHarDistorOad[] =
{
	10,		// 个数

	0x20002200,		//——事件发生时刻电压
	0x20012500,		//——事件发生时刻电流
	0x200A2200,		//——事件发生时刻功率因数
	0x200F2200,		//——事件发生时刻频率
	0x200B2200,		//——事件发生时刻电压谐波总畸变率
	0x20008200,		//——事件结束后电压
	0x20018500,		//——事件结束后电流
	0x200A8200,		//——事件结束后功率因数
		
	0x200F8200,		//——事件结束后频率
	0x200B8200,		//——事件结束后电压谐波总畸变率		

};

#define TEST_EVENT 0
BYTE GetUHarDistorStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitUHarDistorPara( void )
{
	api_WriteEventAttribute( OAD_HARMONIC_U_DISTORTION, 0xFF, (BYTE *)&UHarDistorOad[0], sizeof(UHarDistorOad)/sizeof(DWORD) );
}


//电流谐波总畸变率超限事件
static const DWORD IHarDistorOad[] =
{
	10,		// 个数

	0x20002200,		//——事件发生时刻电压
	0x20012500,		//——事件发生时刻电流
	0x200A2200,		//——事件发生时刻功率因数
	0x200F2200,		//——事件发生时刻频率
	0x200C2200,		//——事件发生时刻电流谐波总畸变率
	0x20008200,		//——事件结束后电压
	0x20018500,		//——事件结束后电流
	0x200A8200,		//——事件结束后功率因数
	0x200F8200,		//——事件结束后频率
	0x200C8200,		//——事件结束后电流谐波总畸变率

};

BYTE GetIHarDistorStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitIHarDistorPara( void )
{
	api_WriteEventAttribute( OAD_HARMONIC_I_DISTORTION, 0xFF, (BYTE *)&IHarDistorOad[0], sizeof(IHarDistorOad)/sizeof(DWORD) );
}


//谐波电压含量超限事件
static const DWORD UHarOverRun[] =
{
	10,		// 个数
	
	0x20002200,		//——事件发生时刻电压
	0x20012500,		//——事件发生时刻电流
	0x200A2200,		//——事件发生时刻功率因数
	0x200F2200,		//——事件发生时刻频率
	0x200C2200,		//——事件发生时刻电流谐波总畸变率
	0x20008200,		//——事件结束后电压
	0x20018500,		//——事件结束后电流
	0x200A8200,		//——事件结束后功率因数
	0x200F8200,		//——事件结束后频率
	0x200C8200,		//——事件结束后电流谐波总畸变率

/*	0x20002200, // 发生时刻电压
	0x20012200, // 发生时刻电流
	0x200F2200, // 发生时刻频率
	*/
};

BYTE GetUHarOverRunStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitUHarOverRunPara( void )
{
	api_WriteEventAttribute( OAD_HARMONIC_U_OVERRUN, 0xFF, (BYTE *)&UHarOverRun[0], sizeof(UHarOverRun)/sizeof(DWORD) );
}


//谐波电流含量超限事件
static const DWORD IHarOverRun[] =
{
	0x03,		// 个数
	0x20002200, // 发生时刻电压
	0x20012200, // 发生时刻电流
	0x200F2200, // 发生时刻频率
};

BYTE GetIHarOverRunStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitIHarOverRunPara( void )
{
	api_WriteEventAttribute( OAD_HARMONIC_I_OVERRUN, 0xFF, (BYTE *)&IHarOverRun[0], sizeof(IHarOverRun)/sizeof(DWORD) );
}


//谐波功率超限事件
static const DWORD PHarOverRun[] =
{
	0x03,		// 个数
	0x20002200, // 发生时刻电压
	0x20012200, // 发生时刻电流
	0x200F2200, // 发生时刻频率
};

BYTE GetPHarOverRunStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitPHarOverRunPara( void )
{
	api_WriteEventAttribute( OAD_HARMONIC_P_OVERRUN, 0xFF, (BYTE *)&PHarOverRun[0], sizeof(PHarOverRun)/sizeof(DWORD) );
}



//间谐波电压含量超限事件
static const DWORD UInterHarOverRun[] =
{
	0x03,		// 个数
	0x20002200, // 发生时刻电压
	0x20012200, // 发生时刻电流
	0x200F2200, // 发生时刻频率
};

BYTE GetUInterHarOverRunStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitUInterHarOverRunPara( void )
{
	api_WriteEventAttribute( OAD_INTER_HARMONIC_U_OVERRUN, 0xFF, (BYTE *)&UInterHarOverRun[0], sizeof(UInterHarOverRun)/sizeof(DWORD) );
}


//间谐波电流含量超限事件
static const DWORD IInterHarOverRun[] =
{
	0x03,		// 个数
	0x20002200, // 发生时刻电压
	0x20012200, // 发生时刻电流
	0x200F2200, // 发生时刻频率
};

BYTE GetIInterHarOverRunStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitIInterHarOverRunPara( void )
{
	api_WriteEventAttribute( OAD_INTER_HARMONIC_I_OVERRUN, 0xFF, (BYTE *)&IInterHarOverRun[0], sizeof(IInterHarOverRun)/sizeof(DWORD) );
}


//间谐波功率超限事件
static const DWORD PInterHarOverRun[] =
{
	0x03,		// 个数
	0x20002200, // 发生时刻电压
	0x20012200, // 发生时刻电流
	0x200F2200, // 发生时刻频率
};

BYTE GetPInterHarOverRunStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitPInterHarOverRunPara( void )
{
	api_WriteEventAttribute( OAD_INTER_HARMONIC_P_OVERRUN, 0xFF, (BYTE *)&PInterHarOverRun[0], sizeof(PInterHarOverRun)/sizeof(DWORD) );
}


//电压短时闪变超限事件
static const DWORD UShortFlicker[] =
{
	10,		// 个数

	0x20002200,		//——事件发生时刻电压
	0x20012500,		//——事件发生时刻电流
	0x200A2200,		//——事件发生时刻功率因数
	0x200F2200,		//----频率
	0x20372200,		//——事件发生时刻电压闪变
	0x20008200,		//——事件结束后电压
	0x20018500,		//——事件结束后电流
	0x200A8200,		//——事件结束后功率因数
	0x200F8200,		//——事件结束后频率
	0x20378200,		//——事件结束后电压闪变
};

BYTE GetUShortFlickerStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitUShortFlickerPara( void )
{
	api_WriteEventAttribute( OAD_SHORT_FLICKER_U_OVERRUN, 0xFF, (BYTE *)&UShortFlicker[0], sizeof(UShortFlicker)/sizeof(DWORD) );
}


//电压长时闪变超限事件
static const DWORD ULongFlicker[] =
{
	10,		// 个数

	0x20002200,		//——事件发生时刻电压
	0x20012500,		//——事件发生时刻电流
	0x200A2200,		//——事件发生时刻功率因数
	0x200F2200,		//——事件发生时刻频率
	0x20382200,		//——事件发生时刻电压闪变
	0x20008200,		//——事件结束后电压
	0x20018500,		//——事件结束后电流
	0x200A8200,		//——事件结束后功率因数
	0x200F8200,		//——事件结束后频率
	0x20388200,		//——事件结束后电压闪变
};

BYTE GetULongFlickerStatus(BYTE Phase)
{
    return 0;
}

void FactoryInitULongFlickerPara( void )
{
	api_WriteEventAttribute( OAD_LONG_FLICKER_U_OVERRUN, 0xFF, (BYTE *)&ULongFlicker[0], sizeof(ULongFlicker)/sizeof(DWORD) );
}


//电压暂降事件
static const DWORD USag[] =
{
	8,		// 个数
	
	0x20002200,		//——事件发生时刻电压
	0x20012500,		//——事件发生时刻电流
	0x200A2200,		//——事件发生时刻功率因数
	0x200F2200,		//——事件发生时刻频率
	0x20008200,		//——事件结束后电压
	0x20018500,		//——事件结束后电流
	0x200A8200,		//——事件结束后功率因数
	0x200F8200,		//——事件结束后频率

};

BYTE GetUSagStatus(BYTE Phase)
{
    return 0;
}

void FactoryInitUSagPara( void )
{
	api_WriteEventAttribute( OAD_SAG_U, 0xFF, (BYTE *)&USag[0], sizeof(USag)/sizeof(DWORD) );
}


//电压暂升事件
static const DWORD USwell[] =
{
	8,		// 个数
	
	0x20002200,		//——事件发生时刻电压
	0x20012500,		//——事件发生时刻电流
	0x200A2200,		//——事件发生时刻功率因数
	0x200F2200,		//——事件发生时刻频率
	0x20008200,		//——事件结束后电压
	0x20018500,		//——事件结束后电流
	0x200A8200,		//——事件结束后功率因数
	0x200F8200,		//——事件结束后频率
};

BYTE GetUSwellStatus(BYTE Phase)
{
    return 0;
}

void FactoryInitUSwellPara( void )
{
	api_WriteEventAttribute( OAD_SWELL_U, 0xFF, (BYTE *)&USwell[0], sizeof(USwell)/sizeof(DWORD) );
}

//电压中断事件
static const DWORD UInterruption[] =
{
	8,		// 个数

	0x20002200,		//——事件发生时刻电压
	0x20012500,		//——事件发生时刻电流
	0x200A2200,		//——事件发生时刻功率因数
	0x200F2200,		//——事件发生时刻频率
	0x20008200,		//——事件结束后电压
	0x20018500,		//——事件结束后电流
	0x200A8200,		//——事件结束后功率因数
	0x200F8200,		//——事件结束后频率
};

BYTE GetUInterruptionStatus(BYTE Phase)
{
	return 0;
}

void FactoryInitUInterruptionPara( void )
{
	api_WriteEventAttribute( OAD_INTERRUPTION_U, 0xFF, (BYTE *)&UInterruption[0], sizeof(UInterruption)/sizeof(DWORD) );
}

//电压偏差超限事件
static const DWORD UDev[] =
{
	8,		// 个数
		
		0x20002200,		//——事件发生时刻电压
		0x20012500,		//——事件发生时刻电流
		0x200A2200,		//——事件发生时刻功率因数
		0x200F2200,		//——事件发生时刻频率
		0x20008200,		//——事件结束后电压
		0x20018500,		//——事件结束后电流
		0x200A8200,		//——事件结束后功率因数
		0x200F8200,		//——事件结束后频率
};

BYTE GetUDevStatus(BYTE Phase)
{
    return 0;
}


void FactoryInitUDevPara( void )
{
	api_WriteEventAttribute( OAD_U_DEV, 0xFF, (BYTE *)&UDev[0], sizeof(UDev)/sizeof(DWORD) );
}

//频率超限事件
static const DWORD FDev[] =
{
	8,		// 个数
		
		0x20002200,		//——事件发生时刻电压
		0x20012500,		//——事件发生时刻电流
		0x200A2200,		//——事件发生时刻功率因数
		0x200F2200,		//——事件发生时刻频率
		0x20008200,		//——事件结束后电压
		0x20018500,		//——事件结束后电流
		0x200A8200,		//——事件结束后功率因数
		0x200F8200,		//——事件结束后频率
};

BYTE GetFDevStatus(BYTE Phase)
{
	return 0;
}



void FactoryInitFDevPara( void )
{
	api_WriteEventAttribute( OAD_U_DEV, 0xFF, (BYTE *)&FDev[0], sizeof(FDev)/sizeof(DWORD) );
}


//电压不平衡度超限事件
static const DWORD UUnbalance[] =
{
	8,		// 个数
		
		0x20002200,		//——事件发生时刻电压
		0x20012500,		//——事件发生时刻电流
		0x200A2200,		//——事件发生时刻功率因数
		0x200F2200,		//——事件发生时刻频率
		0x20008200,		//——事件结束后电压
		0x20018500,		//——事件结束后电流
		0x200A8200,		//——事件结束后功率因数
		0x200F8200,		//——事件结束后频率
};
BYTE GetUUnbalanceStatus(BYTE Phase)
{
	return 0;
}



void FactoryInitUUnbalancePara( void )
{
	api_WriteEventAttribute( OAD_U_DEV, 0xFF, (BYTE *)&UUnbalance[0], sizeof(UUnbalance)/sizeof(DWORD) );
}

//电流不平衡度超限
static const DWORD IUnbalance[] =
{
	8,		// 个数
		
		0x20002200,		//——事件发生时刻电压
		0x20012500,		//——事件发生时刻电流
		0x200A2200,		//——事件发生时刻功率因数
		0x200F2200,		//——事件发生时刻频率
		0x20008200,		//——事件结束后电压
		0x20018500,		//——事件结束后电流
		0x200A8200,		//——事件结束后功率因数
		0x200F8200,		//——事件结束后频率
};

BYTE GetIUnbalanceStatus(BYTE Phase)
{
	return 0;
}


void FactoryInitIUnbalancePara( void )
{
	api_WriteEventAttribute( OAD_U_DEV, 0xFF, (BYTE *)&IUnbalance[0], sizeof(IUnbalance)/sizeof(DWORD) );
}


//电压波动超限
static const DWORD UChangePara[] =
{
	8,		// 个数
		
		0x20002200,		//——事件发生时刻电压
		0x20012500,		//——事件发生时刻电流
		0x200A2200,		//——事件发生时刻功率因数
		0x200F2200,		//——事件发生时刻频率
		0x20008200,		//——事件结束后电压
		0x20018500,		//——事件结束后电流
		0x200A8200,		//——事件结束后功率因数
		0x200F8200,		//——事件结束后频率
};
BYTE GetUChangeStatus(BYTE Phase)
{
	return 0;
}


void FactoryInitIUChangePara( void )
{
	api_WriteEventAttribute( OAD_U_DEV, 0xFF, (BYTE *)&UChangePara[0], sizeof(UChangePara)/sizeof(DWORD) );
}



