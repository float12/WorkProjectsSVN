//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.9
//描述		自由事件和异常事件记录
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

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
BYTE		SysUNMsgCounter;		//一天异常事件记录记录最大次数
WORD		SysFREEMsgCounter;		//一天自由事件记录记录最大次数

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 只读取时间，不刷新RealTimer
//
//参数: 
//        	pBuf[in]		输出时间buf
//返回值:  	无
//
//备注:  由自由事件调用，解决电表低功耗下复位掉电自由事件无法记录时间问题
//-----------------------------------------------
void SysFreeEventGetRtcTime(BYTE *pBuf)
{
	api_GetRtcDateTime(DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, pBuf);
	if( lib_IsAllAssignNum(pBuf, 0x00, 6) == TRUE )
	{
		api_FreeEventReadClockHara(pBuf);
	}
}

//-----------------------------------------------
//函数功能: 记录自由事件
//
//参数: 
//			EventType[in]		自由事件类型
//        	EventSub[in]		附属数据
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_WriteFreeEvent(WORD EventType, WORD EventSub)
{
	DWORD dwAddr;
	WORD Point;
	//在堆栈中开1个结构的空间
	TFreeEvent TempBuf;
	
	//一天最多记录512次
	if( SysFREEMsgCounter < (MAX_SYS_FREE_MSG) )
	{
		SysFREEMsgCounter++;
	}
	else
	{
		return;
	}

	//读指针
	api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&Point);
	if( Point >= MAX_FREE_EVENT )
	{
		Point = 0;
	}
	
	TempBuf.EventType = EventType;
	//保留扩充
	TempBuf.SubEvent = EventSub;

	//连续的6个字节填充成年：月：日：时：分：秒（5：4：3：2：1：0）
	// api_GetRtcDateTime( DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, (BYTE *)&TempBuf.EventTime[0] );
	SysFreeEventGetRtcTime( (BYTE *)&TempBuf.EventTime[0] );
	dwAddr = GET_CONTINUE_ADDR( SysFreeEventConRom ) + sizeof(WORD) + Point*(sizeof(TFreeEvent));
	api_WriteToContinueEEPRom(dwAddr, sizeof(TFreeEvent), (BYTE *)&(TempBuf) );

	Point ++;
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&Point);
}


//-----------------------------------------------
//函数功能: 读自由事件
//
//参数: 
//			Num[in]		上Num次自由事件记录
//          Buf[out]	输出缓冲	          
//返回值:  	返回数据长度
//
//备注:   
//-----------------------------------------------
WORD api_ReadFreeEvent(WORD Num,BYTE *Buf)
{
	WORD ReadPoint;
	DWORD dwAddr;
	
	if( (Num>MAX_FREE_EVENT) || (Num==0) )
	{
		return 0;
	}
	
	//读指针
	api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&ReadPoint);	
	if( ReadPoint >= Num )
	{
		ReadPoint = ReadPoint - Num;
	}
	else
	{
		ReadPoint = ReadPoint+MAX_FREE_EVENT-Num;
	}
	
	dwAddr = GET_CONTINUE_ADDR( SysFreeEventConRom ) + sizeof(WORD) + ReadPoint*(sizeof(TFreeEvent));
	api_ReadFromContinueEEPRom(dwAddr, sizeof(TFreeEvent), Buf );
	
	return sizeof(TFreeEvent);
}

//-----------------------------------------------
//函数功能: 记录异常事件
//
//参数: 
//			Msg[in]		异常事件类型
//                    
//返回值:  	无
//
//备注: 
//-----------------------------------------------
void api_WriteSysUNMsg(WORD Msg)
{
	DWORD dwAddr;
	WORD Point;

	//在堆栈中开1个结构的空间
	TSysUNMsg TempBuf;
	
	//一次上电最多记录120次
	if( SysUNMsgCounter < (MAX_SYS_UN_MSG-8) )
	{
		SysUNMsgCounter++;
	}
	else
	{
		return;
	}

	//读指针
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&Point);
	if( Point >= MAX_SYS_UN_MSG )
	{
		Point = 0;
	}

	TempBuf.EventType = Msg;
	//连续的6个字节填充成年：月：日：时：分：秒（5：4：3：2：1：0）
	api_GetRtcDateTime( DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, (BYTE *)&TempBuf.EventTime[0] );
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + Point*(sizeof(TSysUNMsg));
	api_WriteToContinueEEPRom(dwAddr, sizeof(TSysUNMsg), (BYTE *)&(TempBuf) );

	Point ++;
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&Point );
}


//-----------------------------------------------
//函数功能: 读异常事件
//
//参数: 
//			Num[in]		上Num次异常事件记录
//          Buf[out]	输出缓冲	          
//返回值:  	返回数据长度
//
//备注:   
//-----------------------------------------------
WORD api_ReadSysUNMsg(WORD Num,BYTE *Buf)
{
	WORD ReadPoint;
	DWORD dwAddr;
	
	if( (Num==0) || (Num>MAX_SYS_UN_MSG) )
	{
		return 0;
	}
	
	//读指针
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&ReadPoint);	
	if( ReadPoint >= Num )
	{
		ReadPoint = ReadPoint - Num;
	}
	else
	{
		ReadPoint = ReadPoint+MAX_SYS_UN_MSG-Num;
	}
	
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + ReadPoint*(sizeof(TSysUNMsg));
	api_ReadFromContinueEEPRom(dwAddr, sizeof(TSysUNMsg), Buf );
	
	return sizeof(TSysUNMsg);
}


