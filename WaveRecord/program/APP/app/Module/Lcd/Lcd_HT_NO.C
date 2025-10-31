//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	LXY
//创建日期	2019-0828
//描述		无液晶显示，led灯处理
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Lcd_ht_NO.h"

#if( LCD_TYPE == LCD_NO )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//#define KEY_SMOOTH_TIME					5		//按键去抖动（单位10ms）
#define CURR_N_TEST_TIME				10		//漏电测试时间（单位500ms）
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//按键检测计数器
TLcdKeyDetTimer LcdKeyDetTimer;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static BOOL LcdRunHightSec=TRUE;	// 1秒运行灯闪烁		

// 状态变化后，保持KEY_SMOOTH_TIME毫秒以上才能接受 
static BYTE KeySmoothTimer;
static BYTE SaveKey1;
static BYTE SaveKey2;
static BYTE CRWLedflag=0;
static eLedLightType LightType[eLedMax];
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  设置超时灯
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void setLedflag(Led_Type ledtype, eLedLightType newlighttype)
{
	if(ledtype >= eLedMax)
	{
		return;
	}

	LightType[ledtype] = newlighttype;
}
//-----------------------------------------------
//函数功能: 设置显示项目
//
//参数:		Type[in]		0--循显   1--键显
//			ItemOrder[in]   显示项目序号
//			OadNum        	OAD的个数(1 or 2)
//			*OADBuf		    OAD数据指针(字节顺序 高在前，低在后)
//			SubIndex		分屏号  1--第一屏  2--第二屏					
//返回值:	
//		   	TRUE/FALSE
//备注:返回两个OAD，若只有一个，则第二个OAD置为ff，后面为OadNum及屏号
//-----------------------------------------------
BOOL api_WriteLcdItem(BYTE Type, WORD ItemOrder, BYTE OadNum, BYTE *OADBuf, BYTE SubIndex)
{	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读取显示项目
//
//参数:		Type[in]		0--循显   1--键显
//			ItemOrder[in]   显示项目序号
//			*OADBuf		    OAD数据指针(字节顺序 高在前，低在后)					
//返回值:	
//		   	TRUE/FALSE
//备注:返回两个OAD，若只有一个，则第二个OAD置为ff，后面为OadNum及屏号
//-----------------------------------------------
BOOL api_ReadLcdItem( BYTE Type, WORD ItemOrder, BYTE *OADBuf )
{
	return TRUE;
}

//-----------------------------------------------
//函数功能: 置位通讯标志
//
//参数: Type[in]	eRS485_I       	485通道
//					eIR				红外通道
//					eCR				载波通道
//					eRS485_II		第二路485通道                   
//返回值:	无
//			
//备注:   
//-----------------------------------------------
void api_SetLcdCommunicationFlag(BYTE Type)
{
 	
}

//-----------------------------------------------
//函数功能: 读写无线信号强度
//
//参数:		Operation[in]	READ/WRITE
//			Buf[in]			信号强度，1字节
//返回值:	
//			TRUE/FALSE
//备注:  
//-----------------------------------------------
BOOL api_ProcLcdWirelessPara(BYTE Operation,BYTE *Buf)
{	
	return FALSE;
}

//-----------------------------------------------
//函数功能: 规约控制液晶显示的切换
//
//参数: 	CommKeyType[in]	eUpKey/eDownKey
//			
//返回值:	无	
//			
//备注:  只支持键显的上翻和下翻查看(和魏工确认过了)
//-----------------------------------------------
void api_ProcLcdCommKey(BYTE CommKeyType)
{

}
//-----------------------------------------------
//函数功能: 显示校表ADJUST
//
//参数: 	无
//                    
//返回值:	无
//
//备注:
//-----------------------------------------------
void api_DisplayAdjust(void)
{

}

//-----------------------------------------------
//函数功能: 在液晶上显示断言 A-XXXXXX  
//
//参数: 
//			LineNum[in]--断言所在的行号
// 				   
//返回值:	 
//
//使用举例:	 api_DisplayAssert(12345); 		//显示"  A-012345  "
//-----------------------------------------------
void api_DisplayAssert(WORD LineNum)
{
	
}

//-----------------------------------------------
//函数功能: 插入显示信息
//
//参数:		InOADNum[in]	OAD个数
//			OADBuf[in]		OAD指针
//			SubIndex[in]	0--不需要分屏  1--第一屏  2--第二屏	
//			EchoTimer[in]	遥控液晶显示时间
//返回值:	TRUE
//		   
//备注:
//-----------------------------------------------
WORD api_WriteLcdEchoInfo(BYTE InOADNum, BYTE *OADBuf, BYTE SubIndex ,WORD EchoTimer)
{	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读取设置液晶的显示、小铃铛、ERR显示、继电器控制参数
//
//参数:		Operation[in]	READ/WRITE
//			Type[in]		0--报警继电器
// 							1--LCD小铃铛报警控制参数
// 							2--液晶背光
// 							3--液晶ERR显示报警
//			Len[in]			数据长度
//			pBuf[in]		读或写的数据缓存
//返回值:	TRUE/False
//	
//备注: 
//-----------------------------------------------
BOOL api_ProcLcdCtrlPara(BYTE Operation,BYTE Type,BYTE Len,BYTE * pBuf)
{	
	return TRUE;
}

//-----------------------------------------------
//函数功能:	键显处理
//
//参数:		Keys[in]:按键类型	
//
//返回值:	无
//
//备注: 测试、合闸、分闸
//-----------------------------------------------

//-----------------------------------------------
// 函数功能: 500ms任务
//
// 参数: 	无
//
// 返回值:	无
//
// 备注:
//-----------------------------------------------
static void Lcd500msTask(void)
{
	static BYTE i = 0;
	BYTE runstep;

	if( (api_GetRunHardFlag(eRUN_HARD_CLOCK_BAT_LOW) == TRUE) || (api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE))
	{
		runstep = 4;
	}
	else
	{
		runstep = 2;
	}
	if((i % runstep) == 0)
	{
		if(LcdRunHightSec == TRUE)
		{
			LcdRunHightSec = FALSE;
			LIGHT_RUN_OPEN;
		}
		else
		{
			LcdRunHightSec = TRUE;
			LIGHT_RUN_STOP;
		}
	}

	i++;
}
//-----------------------------------------------
// 函数功能: 指示灯大循环任务
//
// 参数: 	无
//
// 返回值:	无
//
// 备注:
//-----------------------------------------------
void api_LcdTask(void)
{
	// BYTE i = 0;
	// const eSUB_EVENT_INDEX WaringEvent[]
	//     = {eSUB_EVENT_LOSTV_A, eSUB_EVENT_LOSTV_B, eSUB_EVENT_LOSTV_C, eSUB_EVENT_POW_BACK_ALL, eSUB_EVENT_POW_BACK_A, eSUB_EVENT_POW_BACK_B, eSUB_EVENT_POW_BACK_C};
	// WORD wUTemp;
	// 秒任务
	if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_500_MS) == TRUE)
	{
		api_ClrTaskFlag(eTASK_LCD_ID, eFLAG_500_MS);   // 清500ms标志
		Lcd500msTask();
	}
	if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_10_MS) == TRUE)
	{
		api_ClrTaskFlag(eTASK_LCD_ID, eFLAG_10_MS);   // 清10ms标志
		// for(i = 0; i < 3; i++)
		// {
		// 	api_GetRemoteData(PHASE_A + (i * 0x1000) + REMOTE_U, DATA_HEX, 1, sizeof(wUTemp), (BYTE *)&wUTemp);
		// 	if((wUTemp >= (0.8 * 2200)) && (wUTemp <= (1.15 * 2200)))
		// 	{
		// 		LIGHT_U_OPEN(i);
		// 	}
		// 	else
		// 	{
		// 		LIGHT_U_STOP(i);
		// 	}
		// }
		// if(LightType[eLedWaring]==eLightAllOn)
		// {
		// 	WARN_ALARM_LED_ON;
		// }
		// else
		// {
		// 	for(i = 0; i < sizeof(WaringEvent); i++)
		// 	{
		// 		if((EventStatus.DelayedStatus[WaringEvent[i] / 8] & (BIT0 << (WaringEvent[i] % 8))))
		// 		{
		// 			WARN_ALARM_LED_ON;
		// 			break;
		// 		}
		// 	}
		// 	if(i == sizeof(WaringEvent))
		// 	{
		// 		WARN_ALARM_LED_OFF;
		// 	}
		// }
		if(CommLed_Time == 0)
		{
			// if(LightType[eLedTxRx]==eLightAllOn)
			// {
			// 	TX_LED_ON;
			// }
			// else
			{
				TX_LED_OFF;
			}
		}
		else
		{
			CommLed_Time--;
		}
	}
}

//-----------------------------------------------
//函数功能: 液晶显示初始化
//
//参数:    
//Type  	Type[in]	ePowerOnMode	上电初始化LCD
//	     				ePowerDownMode	低功耗处理LCD                   
//返回值:	无
//
//备注:  
//-----------------------------------------------
void api_PowerUpLcd(ePOWER_MODE Type)
{
	memset(LightType,eLightAllNone,eLedMax);
}
//-----------------------------------------------
//函数功能: 掉电液晶处理
//
//参数:		无
//                    
//返回值:	无
//
//备注:  
//-----------------------------------------------
void api_PowerDownLcd(void)
{

}

//-----------------------------------------------
//函数功能: 厂内电表初始化任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_FactoryInitLcd(void)
{
	
}

#endif  //#if( LCD_TYPE == LCD_NO )
