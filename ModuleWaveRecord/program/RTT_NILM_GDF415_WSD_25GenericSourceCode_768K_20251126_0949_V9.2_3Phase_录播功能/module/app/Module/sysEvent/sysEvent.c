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
extern TRealTimer RealTimer;//此处作为timer的主定义  声明为全局变量
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
//函数功能: 根据入口参数读取时间
//
//参数: 
//			Type[in]				需要获取的时间根式
//									bit15: 0：年月日星期时分秒 年在buf[0]且年是小端模式   1：秒分时星期日月年 年是小端模式
//									bit14: 0: Hex码  	1:BCD码
//									bit13-bit8:保留
//									bit7:年是否加2000  1：加2000
//									bit6-bit0:年月日星期时分秒  1:表示需要该位对应的数据
//			pBuf[out]				存放读出的数据
//                    
//返回值:  	返回读取的数据长度
//备注:     时间都是Hex码，年都是word类型
//#define DATETIME_SECOND_FIRST			BIT15//秒在buf[0]
//#define DATETIME_BCD					BIT14//读出数据是bcd
//#define DATETIME_20YY					(BIT7+BIT6)
//#define DATETIME_YY					BIT6
//#define DATETIME_MM					BIT5
//#define DATETIME_DD					BIT4
//#define DATETIME_WW					BIT3
//#define DATETIME_hh					BIT2
//#define DATETIME_mm					BIT1
//#define DATETIME_ss					BIT0
//
//#define DATETIME_YYMMDD				(BIT6+BIT5+BIT4)
//#define DATETIME_20YYMMDD				(BIT7+BIT6+BIT5+BIT4)
//#define DATETIME_YYMMDDWW				(BIT6+BIT5+BIT4+BIT3)
//#define DATETIME_20YYMMDDWW			(BIT7+BIT6+BIT5+BIT4+BIT3)
//#define DATETIME_hhmmss				(BIT2+BIT1+BIT0)
//
//#define DATETIME_MMDDhhmm				(BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_YYMMDDhhmm			(BIT6+BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_20YYMMDDhhmm			(BIT7+BIT6+BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_MMDDhhmmss			(BIT5+BIT4+BIT2+BIT1+BIT0)
//#define DATETIME_YYMMDDhhmmss			(BIT6+BIT5+BIT4+BIT2+BIT1+BIT0)
//#define DATETIME_20YYMMDDhhmmss		(BIT7+BIT6+BIT5+BIT4+BIT2+BIT1+BIT0)
//-----------------------------------------------
WORD api_GetRtcDateTime( WORD Type, BYTE *pBuf )
{
	BYTE i,Length;
	WORD wTmp;
	
	Length = 0;
	for( i=0; i<7; i++)
	{
		if( Type&(0x01<<i) )
		{
			switch( i )
			{
				case 0:
					pBuf[Length++] = RealTimer.Sec;
					break;
				case 1:
					pBuf[Length++] = RealTimer.Min;
					break;
				case 2:
					pBuf[Length++] = RealTimer.Hour;
					break;
				case 3:
					pBuf[Length++] = ((api_CalcInTimeRelativeSec( &RealTimer )/60/1440)+6)%7;
					break;
				case 4:
					pBuf[Length++] = RealTimer.Day;
					break;
				case 5:
					pBuf[Length++] = RealTimer.Mon;
					break;
				case 6:
					pBuf[Length++] = RealTimer.wYear%100;
					break;
			}
		}
	}
	
	if( Type&0x4000 )//要求BCD码格式
	{
		for(i=0; i<Length; i++)
		{
			pBuf[i] = lib_BBinToBCD( pBuf[i] );
		}
	}
	
	if( (Type & 0xc0) == 0xc0 )//处理年是否加2000，这种情况要求年必须也选了
	{
		if( (Type&0x4000) == 0x0000 )//Hex码
		{
			Length -= 1;//到这长度不会为0的
			wTmp = RealTimer.wYear;
			memcpy( &pBuf[Length], &wTmp, sizeof(WORD) );
			Length += 2;
		}
		else//要求BCD码格式 WORD类型同意按小端模式处理
		{
			pBuf[Length] = 0x20;
			Length ++;
		}
	}
	
	if( (Type&0x8000) == 0x0000 )//如果要求 年月日星期时分秒 年在buf[0] 顺序
	{
		lib_InverseData( pBuf, Length );
		if( (Type&0xc0) == 0xc0 )//处理年是否加2000，这种情况要求年必须也选了
		{
			lib_InverseData( pBuf, 2 );
		}
	}
	
	return Length;
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
	api_ReadContinueMem( GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&Point);
	if( Point >= MAX_FREE_EVENT )
	{
		Point = 0;
	}
	
	TempBuf.EventType = EventType;
	//保留扩充
	TempBuf.SubEvent = EventSub;

	//连续的6个字节填充成年：月：日：时：分：秒（5：4：3：2：1：0）
	api_GetRtcDateTime( DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, (BYTE *)&TempBuf.EventTime[0] );
	dwAddr = GET_CONTINUE_ADDR( SysFreeEventConRom ) + sizeof(WORD) + Point*(sizeof(TFreeEvent));
	api_WriteContinueMem(dwAddr, sizeof(TFreeEvent), (BYTE *)&(TempBuf) );

	Point ++;
	api_WriteContinueMem(GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&Point);
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
	api_ReadContinueMem( GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&ReadPoint);	
	if( ReadPoint >= Num )
	{
		ReadPoint = ReadPoint - Num;
	}
	else
	{
		ReadPoint = ReadPoint+MAX_FREE_EVENT-Num;
	}
	
	dwAddr = GET_CONTINUE_ADDR( SysFreeEventConRom ) + sizeof(WORD) + ReadPoint*(sizeof(TFreeEvent));
	api_ReadContinueMem(dwAddr, sizeof(TFreeEvent), Buf );
	
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
	api_ReadContinueMem(GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&Point);
	if( Point >= MAX_SYS_UN_MSG )
	{
		Point = 0;
	}

	TempBuf.EventType = Msg;
	//连续的6个字节填充成年：月：日：时：分：秒（5：4：3：2：1：0）
	api_GetRtcDateTime( DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, (BYTE *)&TempBuf.EventTime[0] );
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + Point*(sizeof(TSysUNMsg));
	api_WriteContinueMem(dwAddr, sizeof(TSysUNMsg), (BYTE *)&(TempBuf) );

	Point ++;
	api_WriteContinueMem( GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&Point );
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
	api_ReadContinueMem(GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&ReadPoint);	
	if( ReadPoint >= Num )
	{
		ReadPoint = ReadPoint - Num;
	}
	else
	{
		ReadPoint = ReadPoint+MAX_SYS_UN_MSG-Num;
	}
	
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + ReadPoint*(sizeof(TSysUNMsg));
	api_ReadContinueMem(dwAddr, sizeof(TSysUNMsg), Buf );
	
	return sizeof(TSysUNMsg);
}


