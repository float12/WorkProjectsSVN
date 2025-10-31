//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.2
//描述		参数管理源文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define MAX_PARA_LEN (sizeof(FPara1Ram) > sizeof(FPara2Ram) ? sizeof(FPara1Ram): sizeof(FPara2Ram))


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
TFPara1 FPara1Ram;
TFPara2 FPara2Ram;
//定义数据库
TFPara1* const FPara1 = (TFPara1 *)&FPara1Ram;
TFPara2* const FPara2 = (TFPara2 *)&FPara2Ram;
BYTE CurrArea, CurrSegment, CurrSegTable,CurrRatioBak;
BYTE CurrRatio;

//管理所有全局变量
TGlobalVariable GlobalVariable;

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------


//0x5008--时区表切换冻结
//0x5009--日时段表切换冻结
//0x500a--费率电价切换冻结
//0x500b--阶梯切换冻结
#if( PREPAY_MODE == PREPAY_LOCAL )
const eFreezeType SwitchFreezeIndex[] =
{ 	
	eFREEZE_TIME_ZONE_CHG,
	eFREEZE_DAY_TIMETABLE_CHG, 
	eFREEZE_TARIFF_RATE_CHG, 
	eFREEZE_LADDER_CHG 
};
#else
const eFreezeType SwitchFreezeIndex[] =
{ 
	eFREEZE_TIME_ZONE_CHG, 
	eFREEZE_DAY_TIMETABLE_CHG 
};
#endif

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 刷新参数在RAM中的参数
//
//参数: 
//			No[in]					0--FPara1  
//									1--FPara2 
//                    
//返回值:  	TRUE: 操作正确 	FALSE：操作错误
//备注:  读出eeprom中的数据更新当前ram中的参数 
//-----------------------------------------------
BOOL api_FreshParaRamFromEeprom( WORD No )
{
    WORD	MyAddr, MyLength;
    BOOL	Result;

	if( No == 0 )
	{
        MyAddr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara1);
		MyLength = sizeof(TFPara1);
		Result = api_VReadSafeMem( MyAddr, MyLength, (BYTE *)FPara1 );
	}
	else if( No == 1 )
	{
        MyAddr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2);
		MyLength = sizeof(TFPara2);
		Result = api_VReadSafeMem( MyAddr, MyLength, (BYTE *)FPara2 );
	}
	else
	{
		Result = FALSE;
	}
	
	return Result;
}

//-----------------------------------------------
//函数功能: 把参数写入FPara1和FPara2及其他当前ram，并且存储到相应的eeprom中
//
//参数: 
//			No[in]					0：FPara1  1：FPara2
//			Addr[in]				结构内的偏移地址
//			Length[in]				要写入的数据长度
//			Ptr[in]					存放要写入的数据
//                    
//返回值:  	TRUE: 操作正确 	FALSE：操作错误
//备注:   
//-----------------------------------------------
BOOL api_WritePara(WORD No, WORD Addr, WORD Length, BYTE * Ptr)
{
	WORD	MyAddr, MyLength;
	WORD	Result;
	BYTE	*p,Buf[MAX_PARA_LEN+30];

	if( No == 0 )
	{
		MyAddr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara1);
		MyLength = sizeof(TFPara1);
		p = (BYTE *)FPara1;
	}
	else if( No == 1 )
	{
		MyAddr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2);
		MyLength = sizeof(TFPara2);
		p = (BYTE *)FPara2;
	}
	else
	{
		return FALSE;
	}
	
	if( (Addr+Length) > MyLength )
	{
		ASSERT( FALSE, 1 );
		return FALSE;
	}
	
	api_VReadSafeMem( MyAddr, MyLength, (BYTE *)Buf );
	
	memcpy( Buf+Addr, (void*)(Ptr), Length);
	
	memcpy(p,Buf, MyLength);
	
	
	Result = api_VWriteSafeMem( MyAddr, MyLength, p );
	
	if( Result != TRUE )
	{
		return FALSE;
	}
	// if (No == 1)
	// {
	// 	memcpy(tfpara2bak,FPara2, sizeof(TFPara2));
	// }
	return TRUE;
}
//-----------------------------------------------
//函数功能: 读写4G模块相关参数
//
//参数: 
//			Operation[in]			操作方式 只能是WRITE或READ
//			Type[in]				需要读的数据类型，Type类型的序号定义不要改，是按645-2007的顺序定义的
//			pBuf[in]				读出数据的缓冲
//                    
//返回值:  	返回操作数据的长度，如果写失败，返回0
//
//备注:  	
//-----------------------------------------------
BYTE api_ProcGprsTypePara( BYTE Operation, TGprsTypeEnum Type, BYTE *pBuf)
{
	BYTE ParaLength;
	WORD Result;
	BOOL (*pFunc)(DWORD Addr, WORD Length, BYTE *Buf);
	if( Operation == WRITE )
	{
		pFunc = api_WriteToContinueEEPRom;
	}
	else
	{
		pFunc = api_ReadFromContinueEEPRom;
	}

	Result = TRUE;
	switch ( Type )
	{
		case eTCPIP_IP_Port_APN:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.TcpipPara.IP_Port_APN );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.TcpipPara.IP_Port_APN), ParaLength, pBuf);
			break;
		case eTCPIP_AreaCode_Ter:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.TcpipPara.AreaCode_Ter );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.TcpipPara.AreaCode_Ter), ParaLength, pBuf);
			break;
		case eTCPIP_APN_UseName:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.TcpipPara.APN_UseName );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.TcpipPara.APN_UseName), ParaLength, pBuf);
			break;
		case eTCPIP_APN_PassWord:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.TcpipPara.APN_PassWord );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.TcpipPara.APN_PassWord), ParaLength, pBuf);
			break;

		case eMQTT_IP_Port_APN:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.MqttPara.IP_Port_APN );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.MqttPara.IP_Port_APN), ParaLength, pBuf);
			break;
		case eMQTT_AreaCode_Ter:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.MqttPara.AreaCode_Ter );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.MqttPara.AreaCode_Ter), ParaLength, pBuf);
			break;
		case eMQTT_APN_UseName:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.MqttPara.APN_UseName );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.MqttPara.APN_UseName), ParaLength, pBuf);
			break;
		case eMQTT_APN_PassWord:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.MqttPara.APN_PassWord );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.MqttPara.APN_PassWord), ParaLength, pBuf);
			break;
		default:
			ParaLength = 0;
			break;
	}

	if( Operation == WRITE )
	{
		if( Result == FALSE )
		{
			ParaLength = 0;
		}
	}

	return ParaLength;
}

//-----------------------------------------------
//函数功能: 读电表的一些相关参数
//
//参数: 
//			Operation[in]			操作方式 只能是WRITE或READ
//			Type[in]				需要读的数据类型，Type类型的序号定义不要改，是按645-2007的顺序定义的
//			pBuf[in]				读出数据的缓冲
//                    
//返回值:  	返回操作数据的长度，如果写失败，返回0
//
//备注:  	
//-----------------------------------------------
BYTE api_ProcMeterTypePara( BYTE Operation, BYTE Type, BYTE *pBuf )
{
	BYTE ParaLength;
	WORD Result;
	DWORD Data;
	BOOL (*pFunc)(DWORD Addr, WORD Length, BYTE * Buf);
	
	if( Operation == WRITE )
	{
		pFunc = api_WriteToContinueEEPRom;
	}
	else
	{
		pFunc = api_ReadFromContinueEEPRom;
	}
	
	Result = TRUE;
	switch( Type )
	{
		case eMeterCommAddr://通讯地址
			ParaLength = GET_STRUCT_MEM_LEN( TFPara1, MeterSnPara.CommAddr );
			if( Operation == WRITE )
			{
				Result = api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, MeterSnPara.CommAddr ), ParaLength, pBuf );
				// api_delayinit_ble();
			}
			else
			{
				memcpy( pBuf, FPara1->MeterSnPara.CommAddr, ParaLength );
			}
			break;
		case eMeterMeterNo://表号
			ParaLength = GET_STRUCT_MEM_LEN( TFPara1, MeterSnPara.MeterNo );
			if( Operation == WRITE )
			{
				Result = api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, MeterSnPara.MeterNo ), ParaLength, pBuf );
			}
			else
			{
				memcpy( pBuf, FPara1->MeterSnPara.MeterNo, ParaLength );
			}
			break;
		case eMeterPropertyCode://资产管理编码 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.PropertyCode );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.PropertyCode), ParaLength, pBuf );
			break;
		case eMeterRateVoltage://额定电压 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.RateVoltage );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.RateVoltage), ParaLength, pBuf );
			break;
		case eMeterRateCurrent://额定电流 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.RateCurrent );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.RateCurrent), ParaLength, pBuf );
			break;
		case eMeterMaxCurrent://最大电流 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.MaxCurrent );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.MaxCurrent), ParaLength, pBuf );
			break;
		case eMeterMinCurrent://最小电流 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.MinCurrent );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.MinCurrent), ParaLength, pBuf );
			break;
		case eMeterTurningCurrent://转折电流 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.TurningCurrent );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.TurningCurrent), ParaLength, pBuf );
			break;
		case eMeterPPrecision://有功精度等级 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.PPrecision );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.PPrecision), ParaLength, pBuf );
			break;
		#if(SEL_RACTIVE_ENERGY == YES)
		case eMeterQPrecision://无功精度等级 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.QPrecision );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.QPrecision), ParaLength, pBuf );
			break;
		#endif
		case eMeterActiveConstant://有功脉冲常数
		#if(SEL_RACTIVE_ENERGY == YES)
		case eMeterReactiveConstant://无功脉冲常数
		#endif
			ParaLength = GET_STRUCT_MEM_LEN( TFPara2, ActiveConstant );
			if( Operation == WRITE )
			{
				if( Type == eMeterActiveConstant )
				{
					Result = api_WritePara( 1, GET_STRUCT_ADDR( TFPara2, ActiveConstant ), ParaLength, pBuf );
				}
				else
				{
					Result = api_WritePara( 1, GET_STRUCT_ADDR( TFPara2, ReactiveConstant ), ParaLength, pBuf );
				}
			}
			else
			{
				if( Type == eMeterActiveConstant )
				{
					Data = FPara2->ActiveConstant;
				}
				else
				{
					Data = 	FPara2->ReactiveConstant;
				}
				memcpy( (void*)pBuf, (void*)&Data, ParaLength );
			}
			break;
		case eMeterMeterModel://电表型号 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.MeterModel );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.MeterModel), ParaLength, pBuf );
			break;
		// case eMeterProduceDate://生产日期 ascii码
		// 	ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.ProduceDate );
		// 	Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.ProduceDate), ParaLength, pBuf );
		// 	break;
		case eMeterProtocolVersion://698.45协议版本号(数据类型:WORD)
			//协议版本号返const值 防止EE损坏情况下返回乱值
			pBuf[0] = (BYTE)(ProtocolVersionConst&0x00FF);
			pBuf[1] = (BYTE)((ProtocolVersionConst >> 8)&0x00FF);
			ParaLength = 2;
			break;
		case eMeterCustomCode://客户编号 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TFPara1, MeterSnPara.CustomCode );
			if( Operation == WRITE )
			{
				Result = api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, MeterSnPara.CustomCode ), ParaLength, pBuf );
			}
			else
			{
				memcpy( pBuf, FPara1->MeterSnPara.CustomCode, ParaLength );
			}
			break;
		case eMeterMeterPosition://电能表位置信息
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.MeterPosition );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.MeterPosition), ParaLength, pBuf );
			break;
		case eMeterSoftWareVersion://厂家软件版本号 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.SoftWareVersion );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.SoftWareVersion), ParaLength, pBuf );
			break;
		case eMeterSoftWareDate://厂家软件版本日期
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.SoftWareDate );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.SoftWareDate), ParaLength, pBuf );
			break;
		case eMeterHardWareVersion://厂家硬件版本号 ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.HardWareVersion );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.HardWareVersion), ParaLength, pBuf );
			break;
		case eMeterHardWareDate://厂家硬件版本日期
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.HardWareDate );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.HardWareDate), ParaLength, pBuf );
			break;
		case eMeterFactoryCode://厂家编号  ascii码
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.FactoryCode );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.FactoryCode), ParaLength, pBuf );
			break;
		case eMeterSoftRecord://软件备案号
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.SoftRecord );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.SoftRecord), ParaLength, pBuf );
			break;
		default:
			ParaLength = 0;
			break;
	}
	
	if( Operation == WRITE )
	{
		if( Result == FALSE )
		{
			ParaLength = 0;
		}
	}
	
	return ParaLength;
}


//-----------------------------------------------
//函数功能: 读写时区时段节假日表
//
//参数: 
//			Operation[in]	操作方式 只能是WRITE或READ
//			Type[in]				
//							D7:	0--第一套时区时段表 1--第二套时区时段表
//							D6--D0:
//								1 -- TTimeAreaTable
//								2 -- TTimeHolidayTable
//								3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable			
//			pBuf[in]		读出或者写入数据的缓冲
//                    
//返回值:  	TRUE/FALSE
//
//备注: 时区表，时段表需要缓冲48字节
//		节假日需要缓冲124字节
//-----------------------------------------------
static BOOL ReadAndWriteTimeTable(BYTE Operation, WORD Type, BYTE *pTmpArray)
{
	WORD MyAddr, MyLength;

	//Type D7:0--第一套时区时段表 1--第二套时区时段表
	if( Type & 0x80 )
	{
		Type &= 0x7f;
		MyAddr = GET_SAFE_SPACE_ADDR( ParaSafeRom.SecondTimeTable );
	}
	else
	{
		MyAddr = GET_SAFE_SPACE_ADDR( ParaSafeRom.FirstTimeTable );
	}

	if( Type > (MAX_TIME_SEG_TABLE+2) )
	{
		return FALSE;
	}

	if( Type == 0 )
	{
		return FALSE;
	}

	if( Type == 1 )//时区
	{
		MyAddr += GET_STRUCT_ADDR( TTimeTable, TimeAreaTable );
		MyLength = sizeof(TTimeAreaTable);
	}
	else if( Type == 2 )//公共假日
	{
		MyAddr = GET_SAFE_SPACE_ADDR( ParaSafeRom.TimeHolidayTable );
		MyLength = sizeof(TTimeHolidayTable);
	}
	else//时段表
	{
		MyAddr += GET_STRUCT_ADDR( TTimeTable, TimeSegTable[Type-3] );
		MyLength = sizeof(TTimeSegTable);
	}

	if( Operation == READ )
	{
		return api_VReadSafeMem( MyAddr, MyLength, pTmpArray );
	}
	else
	{
		return api_VWriteSafeMem( MyAddr, MyLength, pTmpArray );
	}
}
//---------------------------------------------------------------
//函数功能: 得到切换时间的相对分钟数，并返回是否越界
//
//参数: 	
//			TmpRealTimer[in] --切换时间，格式YYYYMMDDHHmm，HEX				
//			pdwTime[out]--切换时间，相对于2000年1月1日0时0分的相对分钟数
//                    
//返回值:  	TRUE--数据合格，FALSE--数据越界
//
//备注:   
//---------------------------------------------------------------
BOOL api_GetSwitchTimeRelativeMin( DWORD *pdwTime, TRealTimer TmpRealTimer )
{
	if( TmpRealTimer.wYear == 0 )
	{
		*pdwTime = 0;
	}
	else if( TmpRealTimer.wYear == 9999 )
	{
		*pdwTime = 99999999;
	}
	else if( TmpRealTimer.wYear == 0x9999 )
	{
		*pdwTime = 0x99999999;
	}
	else if( TmpRealTimer.wYear == 0xffff )
	{
		*pdwTime = 0xffffffff;
	}
	else if(( TmpRealTimer.wYear == 2000 ) && ( TmpRealTimer.Mon == 1 ) && ( TmpRealTimer.Day == 1 )
		&& ( TmpRealTimer.Hour == 0 ) && ( TmpRealTimer.Min == 0 ))
	{
		//相对分钟数设为1，设成0判不过不切换
		*pdwTime = 1;
	}
	else
	{
		if( api_CheckMonDay( TmpRealTimer.wYear, TmpRealTimer.Mon, TmpRealTimer.Day ) == FALSE )
		{
			return FALSE;
		}
		if( (TmpRealTimer.Hour > 23) && (TmpRealTimer.Hour != 0xff) )
		{
			return FALSE;
		}
		if( (TmpRealTimer.Min > 59) && (TmpRealTimer.Min != 0xff) )	
		{
			return FALSE;
		}
			
		TmpRealTimer.Sec = 0;//强制置秒为零
			
		*pdwTime = api_CalcInTimeRelativeMin( &TmpRealTimer );//计算绝对时间
		
		if( *pdwTime == ILLEGAL_VALUE_8F )
		{
			return FALSE;
		}		
	}
	
	return TRUE;
}
//---------------------------------------------------------------
//函数功能: 读写切换时间
//
//参数: 	
//			Operation[in]	操作方式 只能是WRITE或READ
//			Type[in]				
//							0--两套时区表切换时间
// 							1--两套日时段切换时间
// 							2--两套分时费率切换时间
// 							3--两套阶梯切换时间 
// 							4--保留
//										
//			pBuf[in]		读出或者写入数据的缓冲HEX(YYMMDDhhmm) YY为两个字节 带20 比如2017
//                    
//返回值:  	TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL api_ReadAndWriteSwitchTime( BYTE Operation, WORD Type, BYTE *pTmpArray )
{
	BYTE i;
	BYTE Buf[sizeof(TSwitchTimePara)];
	TRealTimer TmpRealTimer;
	BYTE TimeBuf[sizeof(TRealTimer)];
	TFourByteUnion dwTime;
	WORD Result;

	if( Type > 4 )
	{
		return FALSE;
	}
	
	memset(Buf,0,sizeof(Buf));
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.SwitchTimePara ), sizeof(TSwitchTimePara), Buf );
	if( Operation == READ )
	{
		if( Result == FALSE )
		{
			return FALSE;
		}

		memcpy( (void *)&dwTime.b, Buf + Type * sizeof(DWORD), sizeof(DWORD) );
		if(dwTime.d == 0)
		{
			memset(pTmpArray,0x00,sizeof(TRealTimer));
		}
		else if( dwTime.d == 99999999 )//贾斌测试软件设置时区表切换时间输入9999/99/99 99:99:99的情况，api_ToAbsTime 不能转换相对时间为99999999的因为超过100年(2018-07-01为9728640)，转换后为2100-13-182 10:39:00
		{
			memset(pTmpArray,99,sizeof(TRealTimer));
			pTmpArray[0] = 0x0f; //9999 = 0x270F
			pTmpArray[1] = 0x27;
		}	
		else if( dwTime.d == 0x99999999 )
		{
			memset(pTmpArray,99,sizeof(TRealTimer));
			pTmpArray[0] = 0x0f; //9999 = 0x270F
			pTmpArray[1] = 0x27;
		}
		else if( dwTime.d == 0xffffffff )
		{
			memset(pTmpArray,0xff,sizeof(TRealTimer));
		}		
		else
		{
			Result = api_ToAbsTime( dwTime.d, &TmpRealTimer );
			if(Result == FALSE)
			{
				return FALSE;
			}
			memcpy( pTmpArray, (void*)&TmpRealTimer, sizeof(TRealTimer) );	//YYMMDDhhmm 没有秒
		}
		
		return TRUE;
	}
	else
	{
		//数据错误后记录一个异常事件 但是还允许设置 防止数据错误后再也不能设置了
		if( Result == FALSE )
		{
			api_WriteSysUNMsg(SWITCH_TIME_CRC_ERR);
		}
		
		memcpy( (void *)&TmpRealTimer, pTmpArray, 6 );
		if( api_GetSwitchTimeRelativeMin( (DWORD *)&(dwTime.d), TmpRealTimer) == FALSE )
		{
			return FALSE;
		}
		
		if( (dwTime.d == 0)
			||(dwTime.d == 99999999L)
			||(dwTime.d == 0x99999999)
			||(dwTime.d == 0xffffffff) )
		{
			api_ClrRunHardFlag( eRUN_HARD_SWITCH_FLAG1 + Type );
		}

		memcpy( Buf + Type * sizeof(DWORD), dwTime.b, sizeof(DWORD) );
		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.SwitchTimePara ), sizeof(TSwitchTimePara), Buf );
		if( Result != TRUE )
		{
			api_WriteSysUNMsg(SWITCH_TIME_CRC_ERR);
			
			return FALSE;
		}
		
		api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
		return Result;
	}


}


//-----------------------------------------------
//函数功能: 读当前、备用套的时区表、时段表、公共假日表
//
//参数: 	Type[in]				
//						D7:	0--第一套时区时段表 1--第二套时区时段表
//						D6--D0:
//							1 -- TTimeAreaTable
//							2 -- TTimeHolidayTable
//							3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable
//			Addr[in]	在相应结构中的偏移
//			Length[in]	要读出内容的长度
//			pBuf[out]	读出数据的缓冲
//                    
//返回值:  	TRUE:正确	FALSE:错误
//
//备注:  	
//-----------------------------------------------
BOOL api_ReadTimeTable(BYTE Type, WORD Addr, WORD Length, BYTE * Buf)
{
	WORD Result;
	BYTE Array[128];

	Result = ReadAndWriteTimeTable(READ, Type, Array);

	if( Result == FALSE )
	{
		return FALSE;
	}

	memcpy((void*)Buf, (void *)&Array[Addr], Length);

	return TRUE;
}


//-----------------------------------------------
//函数功能: 写入当前、备用套的时区表、时段表、公共假日表
//
//参数: 	Type[in]				
//						D7:	0--第一套时区时段表 1--第二套时区时段表
//						D6--D0:
//							1 -- TTimeAreaTable
//							2 -- TTimeHolidayTable
//							3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable
//			Addr[in]	在相应结构中的偏移
//			Length[in]	要写入内容的长度
//			pBuf[in]	写入数据的缓冲
//                    
//返回值:  	TRUE:正确	FALSE:错误
//
//备注:  	
//-----------------------------------------------
BOOL api_WriteTimeTable(BYTE Type, WORD Addr, WORD Length, BYTE *Buf)
{
	BYTE Array[128];

	ReadAndWriteTimeTable(READ, Type, Array);

	//边界判断
	if( Length > sizeof(Array) )
	{
		return FALSE;
	}

	//现在缓冲中的内容的正确的
	//开始刷新
	memcpy((void *)&Array[Addr], (void*)Buf, Length);

	ReadAndWriteTimeTable(WRITE, Type, Array);

	return TRUE;
}

//-----------------------------------------------
//函数功能: 判断切换时间
//
//参数: 
//			byType[in]				
//				0--两套时区表切换冻结 
//				1--两套日时段表切换冻结 
//				2--两套分时费率切换冻结 
//				3--两套梯度切换冻结 
//				4--保留
//         
//返回值:  	TRUE/FALSE
//备注: 
//-----------------------------------------------
static BOOL JudgeTableSwitch(BYTE byType)
{
	WORD Result;
	TSwitchTimePara TmpSwitchTimePara;

	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara), sizeof(TSwitchTimePara), (BYTE *)&TmpSwitchTimePara );
	if( Result == FALSE )
	{
		return FALSE;
	}

	//如果切换时间为0，应该是切换过，也可能初始化为0，没切换过，则不切换
	if( (TmpSwitchTimePara.dwSwitchTime[byType] == 0) || ((TmpSwitchTimePara.dwSwitchTime[byType] > 0x03228D1F)) )//0x03228D1F==2099.12.31.23:59:00。为了保证切换设置为9999999999时也能正常。
	{        
		return FALSE;
	}

	//如果切换时间没到，则不切换
	if( g_RelativeMin < TmpSwitchTimePara.dwSwitchTime[byType] )
	{
		//切换时间有效，液晶显示用
		api_SetRunHardFlag(eRUN_HARD_SWITCH_FLAG1+byType);
		return FALSE;
	}

	api_ClrRunHardFlag(eRUN_HARD_SWITCH_FLAG1+byType);
	TmpSwitchTimePara.dwSwitchTime[byType] = 0;
	
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara), sizeof(TSwitchTimePara), (BYTE *)&TmpSwitchTimePara );

	return TRUE;
}

//-----------------------------------------------
//函数功能: 读出备用套数值切换到当前套
//
//参数: 	Type[in] 0--时区表  1--时段表
//                    
//返回值:  	TRUE/FALSE
//
//备注:   
//-----------------------------------------------
static BOOL CopyBackupToCurrent( WORD Type )
{
	BYTE i,Length,SuitNum;
	BYTE Array[76];//按照南网阶梯表开 

	if( Type == 0 )
	{
		Type = 1;
		Length = sizeof(TTimeAreaTable);
		SuitNum = 1;
	}
	else if( Type == 1 )
	{
		Type = 3;
		Length = sizeof(TTimeSegTable);
		SuitNum = MAX_TIME_SEG_TABLE;
	}
	#if( PREPAY_MODE == PREPAY_LOCAL )
	else if( Type == 2 )//费率切换
	{
		SuitNum = 0;
		Length = sizeof(TRatePrice);
		
		api_ReadPrePayPara( eBackupRateTable, Array );
        api_WritePrePayPara( eCurRateTable, Array );//函数中更改费率电价会自动重新判断电价
	}
	else if( Type == 3 ) //阶梯切换
	{
		SuitNum = 0;
		Length = sizeof(TLadderPrice);
		
		api_ReadPrePayPara( eBackupLadderTable, Array );
        api_WritePrePayPara( eCurLadderTable, Array );//函数中更改费率电价会自动重新判断电价
	}
	#endif
	else
	{
		return FALSE;
	}
	
	for(i=0; i<SuitNum; i++)
	{
		api_ReadTimeTable( 0x80+Type+i, 0, Length, Array );
	
		api_WriteTimeTable( 0x00+Type+i, 0, Length, Array );
	}
	
	return TRUE;
}


//-----------------------------------------------
//函数功能: 每分钟判断切换时间
//
//参数: 	无
//			
//返回值:  	无
//备注: 每分钟调用一次，只置冻结标志，并不冻结数据，冻结数据后面再做，冻结时再把标志清掉
//
//-----------------------------------------------
static void ProcTableSwitchTask(void)
{
	BYTE i;

//	0--两套时区表切换冻结 
//	1--两套日时段表切换冻结 
//	2--两套分时费率切换冻结 
//	3--两套梯度切换冻结 
	#if( PREPAY_MODE == PREPAY_LOCAL )
	for( i=0; i<4; i++ )
	#else
	for( i=0; i<2; i++ )
	#endif
	{
		if( JudgeTableSwitch(i) == TRUE )
		{
			//调用相应的切换函数
			CopyBackupToCurrent( i ); //!!!!! 此处只能进行时区表 日时段表切换，不能进行费率和梯度的切换 此处注意!!!!!! --jwh
			
			// api_SetFreezeFlag( SwitchFreezeIndex[i], 0 );
		}
	}
}


//-----------------------------------------------
//函数功能: 效验当前RAM参数 FPara1/FPara2/g_WarnDispPara
//
//参数: 	
//                    
//返回值:  	TRUE/FALSE
//
//备注: 上电或每分钟效验RAM参数  
//-----------------------------------------------
void WatchPara(void)
{
	//监视当前参数
	//参数区1校验错误
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara1), sizeof(TFPara1), UN_REPAIR_CRC ) == FALSE )
	{
		//由EEProm恢复参数
		if( api_FreshParaRamFromEeprom(0) != TRUE )
		{
			api_SetError(ERR_FPARA1);
		}
	}
	else
	{
		api_ClrError(ERR_FPARA1);
	}
	
	if( FPara1->TimeZoneSegPara.Ratio > MAX_RATIO )
	{
		FPara1->TimeZoneSegPara.Ratio = 4;
	}
	
	//参数区2校验错误
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == FALSE )
	{
		//由EEProm恢复参数
		if( api_FreshParaRamFromEeprom(1) != TRUE )
		{
			memcpy( &FPara2->CommPara, &CommParaConst, sizeof(TCommPara) );	//CRC错误 FPara中的通信参数回复
			api_SetError(ERR_FPARA2);
		}
	}
	else
	{
		api_ClrError(ERR_FPARA2);
	}

	//监视当前费率
	if( CurrRatio != CurrRatioBak )
	{
		//从新计算费率2
		api_CheckTimeTable(TRUE);
		// api_SetError(ERR_RATIO_NOW);
	}
}
//-----------------------------------------------
//函数功能: 得到当前费率
//
//参数: 	无
//                   
//返回值:   BYTE:返回当前费率
//
//备注:   
//-----------------------------------------------
BYTE api_GetCurRatio( void )
{
	return CurrRatio;
}
//-----------------------------------------------
//函数功能: 参数大循环任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_ParaTask( void )
{
	BYTE tSec;
	
	//是否到1秒
	if( api_GetTaskFlag(eTASK_PARA_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PARA_ID,eFLAG_SECOND);
		
		FunctionConst(PARA_TASK1);
		//以下为1分钟执行一次
		api_GetRtcDateTime(DATETIME_ss,&tSec);
		if( tSec == (eTASK_PARA_ID*3+3) )
		{
			//监视Flash中的参数
			WatchPara();
		}
	}
	
	
	if( (api_GetTaskFlag( eTASK_PARA_ID, eFLAG_MINUTE ) == TRUE) || 
	   (api_GetTaskFlag( eTASK_PARA_ID, eFLAG_SWITCH_JUDGE ) == TRUE)|| 
	   (api_GetTaskFlag( eTASK_PARA_ID, eFLAG_TIME_CHANGE ) == TRUE))
	{
		api_ClrTaskFlag( eTASK_PARA_ID, eFLAG_MINUTE );
		api_ClrTaskFlag( eTASK_PARA_ID, eFLAG_SWITCH_JUDGE );
		api_ClrTaskFlag( eTASK_PARA_ID, eFLAG_TIME_CHANGE );
		
		FunctionConst(PARA_TASK2);
		// ProcTableSwitchTask();
		
		//检测是否有时段表符合要求
		//必须放在DealMinSetDlt645_2007FreezeStatus之后，这样时段表切换后就会立刻切换费率，以前会延时1分钟再切换费率
		// api_CheckTimeTable(TRUE);
	}
}

//-----------------------------------------------
//函数功能: 参数任务上电初始化
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_PowerUpPara( void )
{
    BYTE result = 0;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//从E2读取当前阶梯表到掉电前阶梯表,补冻結用，在初始化参数前读取()
	//防止掉电过阶梯结算将E2中当前阶梯表覆盖，不可挪动位置！！！--李丕凯
	api_ReadPowerLadderTable();
	#endif
	//系统自检
	//RAM中的参数（含FPara1、FPara2、CurrRatio）
	WatchPara();
	//读UIk参数
	result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleUIKSafeRom.Uk[0]), sizeof(TSampleUIK), (BYTE *)&UIFactor);

	api_CheckTimeTable(FALSE);
	
	ProcTableSwitchTask();
	
	//检测是否有时段表符合要求
	//必须放在DealMinSetDlt645_2007FreezeStatus之后，这样时段表切换后就会立刻切换费率，以前会延时1分钟再切换费率
	api_CheckTimeTable(TRUE);
	//多功能端子初始化
	api_MultiFunPortSet( eCLOCK_PULS_OUTPUT );
}

//-----------------------------------------------
//函数功能: 判断时区或者时段是否合法
//
//参数: 
//			Type[in]				0：时区表  1：时段表
//          pAreaAndSegment[in]		3字节，当前时区或当前时段
//         
//返回值:  	TRUE/FALSE
//备注: 
//-----------------------------------------------
static BOOL CheckAreaAndSegment( BYTE Type, BYTE *pAreaAndSegment )
{
	BYTE i;

//	for(i=0; i<3; i++)
//	{
//		if( lib_CheckBCD( pAreaAndSegment[i] ) == FALSE )
//		{
//			return FALSE;
//		}
//	}

	if( Type == 0 )//时区
	{
		for(i=0; i<3; i++)
		{
			if( pAreaAndSegment[i] == 0x00 )
			{
				return FALSE;
			}
		}

		if( (pAreaAndSegment[2] > MAX_TIME_SEG_TABLE) 
			|| (pAreaAndSegment[1] > MAX_VALID_DAY) 
			|| (pAreaAndSegment[0] > MAX_VALID_MONTH) )
		{
			return FALSE;
		}
	}
	else//时段
	{
		if( (pAreaAndSegment[2] > MAX_RATIO) 
			|| (pAreaAndSegment[2] == 0x00) 
			|| (pAreaAndSegment[1] > MAX_VALID_MINUTE) 
			|| (pAreaAndSegment[0] > MAX_VALID_HOUR) )
		{
			return FALSE;
		}
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 将时区表或时段表进行排序
//
//参数: 
//			AreaAndSegmentNum[in]	时区表内时区个数 或 时段表内时段个数
//          pAreaAndSegment[in]		时区表或时段表
//         
//返回值:  	无
//备注: 
//-----------------------------------------------
static void SortAreaAndSegment( BYTE AreaAndSegmentNum, BYTE *pAreaAndSegment )
{
	BYTE i,j,i0,i1,i2,j0,j1,j2;
	BYTE Temp;

	for(i=0; i<AreaAndSegmentNum; i++)
	{
		for(j=i; j<AreaAndSegmentNum; j++)
		{
			i0 = i*3+0;
			j0 = j*3+0;
			i1 = i*3+1;
			j1 = j*3+1;
			i2 = i*3+2;
			j2 = j*3+2;

			//如果日大于，或日等于但时大于
			if( (pAreaAndSegment[i0] > pAreaAndSegment[j0])||
				( (pAreaAndSegment[i0] == pAreaAndSegment[j0])&&(pAreaAndSegment[i1] > pAreaAndSegment[j1]) ) )
			{
				Temp = pAreaAndSegment[i0];
				pAreaAndSegment[i0] = pAreaAndSegment[j0];
				pAreaAndSegment[j0] = Temp;

				Temp = pAreaAndSegment[i1];
				pAreaAndSegment[i1] = pAreaAndSegment[j1];
				pAreaAndSegment[j1] = Temp;

				Temp = pAreaAndSegment[i2];
				pAreaAndSegment[i2] = pAreaAndSegment[j2];
				pAreaAndSegment[j2] = Temp;
			}
		}
	}
}

//-----------------------------------------------
//函数功能: 搜索时区表和时段表的公共函数
//
//参数: 
//			Type[in]				0：时区表  1：时段表
//          Num[in]					时区数或时段表数
//			pBuf[in]				时区表或者时段表          
//返回值:  	返回搜索到的时段表号或费率数
//备注: 
//-----------------------------------------------
static BYTE SearchTabel( BYTE Type, BYTE Num, BYTE *pBuf )
{
	BYTE CurrValue,j,k;
	WORD i,l,wDateTime;
	BYTE DateBuf[4];
	BYTE *p;

	p = (BYTE *)pBuf;
	l = FALSE;
	j = Num;

	//--------把非法的时段或时区去掉
	k = 0;
	for(i=0; i<j; i++)
	{
		if( CheckAreaAndSegment( Type, &p[i*3] ) == FALSE )
		{
			continue;
		}
		else
		{
			memcpy( &p[k*3], &p[i*3], 3 );

			k++;
		}
	}

	//--------排序
	j = k;
	SortAreaAndSegment( j, p );

	//--------获取当前月日时分
	api_GetRtcDateTime(DATETIME_MMDDhhmm,DateBuf);	
	if( Type == 0 )//时区表--月日
	{
		wDateTime = DateBuf[0];
		wDateTime <<= 8;
		wDateTime += DateBuf[1];
	}
	else//时段表--时分
	{
		wDateTime = DateBuf[2];
		wDateTime <<= 8;
		wDateTime += DateBuf[3];
	}

	//-------搜当前时段表号或者费率号 CurrValue
	i = p[0];
	i <<= 8;
	i += p[1];

	//当前日期/时间还没到第一时区/时段
	if( wDateTime < i )//处理掉最后一个时区
	{
		l = TRUE;
	}
	//当前日期/时间已经到第一时区/时段
	else
	{
		i = p[3*j-3];
		i <<= 8;
		i += p[3*j-2];
		//当前日期/时间已经到最后一时区/时段
		if( wDateTime >= i )
		{
			l = TRUE;
		}
	}

	//如果当前日期/时间没到第一时区/时段，已经到最后一时区/时段
	if( l == TRUE )
	{
		//k取最后一时区/时段的时段表号或费率数
		k = p[3*j-1];
		//CurrValue 取 时区数/时段表数
		CurrValue = j;
	}
	else
	{
		//搜索整个表
		p += 3;//因为小于第一个时区的情况前面已经判断了

		for(i=1; i<j; i++)//因为前面已经处理了一个时区，所以这里应少判一个时区
		{
			l = p[0];
			l <<= 8;
			l += p[1];
			if( wDateTime < l )
			{
				//当前日期/时间没到这个时区/时段
				//k取上一个时区/时段的时段表号或费率数
				p -= 3;
				k = p[2];
				l = TRUE;

				break;
			}

			p += 3;
		}

		//没找到时区，取第一个时区的时段表号
		if( l == FALSE )
		{
			k = p[2];
			CurrValue = 1;
		}
		else
		{
			CurrValue = i;
		}
	}

	//判断当前时段表号与备份时段表号是否一致
	//判断当前费率号与备份费率号是否一致
	if( Type == 0 )//时区表 
	{
		if( CurrValue != CurrArea )
		{
			CurrArea = CurrValue;
		}
	}
	else//时段表 
	{
		if( CurrSegment != CurrValue )
		{
			api_MultiFunPortCtrl( eSEGMENT_SWITCH_OUTPUT );

			CurrSegment = CurrValue;
		}
	}

	return k;
}


//-----------------------------------------------
//函数功能: 检测时段表，每分钟调用一次
//
//参数: 
//			Type[in]				TRUE 转存  FALSE 不转存
//                    
//返回值:  	无
//备注: 搜索当前节假日表，周休日字，时区表，时段表，找到当前时段  
//-----------------------------------------------
void api_CheckTimeTable(WORD Type)
{
	WORD Addr;
	WORD i, j, k;
	BYTE DateBuf[4];
	BYTE * p;
    BYTE Array[128];

	//时段表号
	k = 0xff00;

	//公共假日处理
	{
		j = FPara1->TimeZoneSegPara.HolidayNum;		//公共假日数
	
		if( (j==0xff) || (j==0xffff) )				//安徽要求：公共假日数设置为FF，表示不采用
		{
			j = 0;
		}
	
		if( j != 0 )
		{		
			if( j > MAX_HOLIDAY )//如果大于最大假日数按最大的处理
			{
				j = MAX_HOLIDAY;
				//给出错信息
			}
			
			//读公共假日表
			//每项格式为-----年(2字节)：月：日：星期 时段标号（3、2、1、0）
			ASSERT( sizeof(TTimeHolidayTable) < sizeof(Array), 1 );
			if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.TimeHolidayTable), sizeof(TTimeHolidayTable), Array) == TRUE )
			{
				p = Array;
		
				//判断是否为公共假日
				api_GetRtcDateTime(DATETIME_20YYMMDD,DateBuf);
				for(i=0; i<j; i++)
				{
					//比较年月日
					if( memcmp( DateBuf, p, 4 ) == 0 )
					{
						k = p[sizeof(TTimeHoliday)-1];
		
						if( k == 0 )
						{
							k = 0xff00;
							continue;
						}
						else
						{
							break;
						}
					}
					
					p += sizeof(TTimeHoliday);
				}
			}
			else//读取失败无假日表
			{
				k = 0xff00;
			}

		}
	}

	//没找到节假日
	//周休日处理
	if( k == 0xff00 )
	{
		//周休日处理 星期天为0
		//判断是否为周休日
		api_GetRtcDateTime( DATETIME_WW, (BYTE*)&j );
		if( ((FPara1->TimeZoneSegPara.WeekStatus>>j) & 0x01) == 0 )
		{
			k = FPara1->TimeZoneSegPara.WeekSeg;

			if( k == 0 )
			{
				k = 0xff00;
			}
		}
	}

	if( k != 0xff00 )//周休日、节假日如果找到时段表，对时段表合法性进行判断，如果不合法按照时区表继续运行
	{
		if( (k==0) || (k>FPara1->TimeZoneSegPara.TimeSegTableNum) )
		{
			k = 0xff00;
		}
	}
	
	//没找到周休日
	//时区处理
	if( k == 0xff00)
	{
		//读年时区数
		j = FPara1->TimeZoneSegPara.TimeZoneNum;

		//时区处理，判断是否到时区
		Addr = GET_STRUCT_ADDR(TTimeTable,TimeAreaTable.TimeArea[0][0]);//起始地址
		//改为了当前套和备用套的方式，因此每次只要读当前套即可
		if( api_VReadSafeMem((GET_SAFE_SPACE_ADDR( ParaSafeRom.FirstTimeTable )+Addr), sizeof(TTimeAreaTable), Array) == FALSE )
		{
			k = 0;//默认使用第一套
			CurrArea = 1;

		}
		else
		{
			if( (j == 0)||(j>MAX_TIME_AREA) )//为零或大于最大时区数均取第一时区
			{
				k = Array[0];
				CurrArea = 1;
			}
			else
			{
				k = SearchTabel( 0, j, Array );
			}
		}
		

	}

	//时段处理
	j = FPara1->TimeZoneSegPara.TimeSegNum;
	if( (j==0)||(j>MAX_TIME_SEG) )//如果时段数为0或大于最大时段个数，则置为最大时段个数
	{
		j = MAX_TIME_SEG;
	}

	//读时段表
	if( (k==0)||(k>FPara1->TimeZoneSegPara.TimeSegTableNum) )//MAX_TIME_SEG_TABLE) )//时段表号为0或大于最大时段表数时取第一时段表
	{
		k = 0;
	}
	else
	{
		k -= 1;
	}
	//当前时段表号
	CurrSegTable = k + 1;

	Addr = GET_STRUCT_ADDR(TTimeTable,TimeSegTable[k].TimeSeg[0][0]);
	if(api_VReadSafeMem((GET_SAFE_SPACE_ADDR( ParaSafeRom.FirstTimeTable )+Addr), sizeof(TTimeSegTable), Array) == FALSE)
	{
		//费率处理
		if( (CurrRatio==0) || (CurrRatio>MAX_RATIO) || (CurrRatio>FPara1->TimeZoneSegPara.Ratio) )//如果费率号为0或大于最大费率数则当费率1处理
		{
			CurrRatio = DefCurrRatioConst;//缺省认为是平时段
			#if( PREPAY_MODE == PREPAY_LOCAL )
			//费率电价重新刷新
			api_SetUpdatePriceFlag( eChangeCurRatePrice );
			#endif
		}
		//不恢复备份，如果费率数与备份不同会导致1分钟判断2次
		return ;
	}

	k = SearchTabel( 1, j, Array );

	//费率处理
	j = FPara1->TimeZoneSegPara.Ratio;
	if( (k==0) || (k>MAX_RATIO) || (k>j) )//如果费率号为0或大于最大费率数则当费率1处理
	{
		k = DefCurrRatioConst;//缺省认为是平时段
	}

	if( CurrRatio != k )
	{
		if( (CurrRatio==0) || (CurrRatio>MAX_RATIO) )//如果费率号为0或大于最大费率数则当费率1处理
		{
			CurrRatio = k;
		}
		if( Type == TRUE )
		{
			api_SwapEnergy();
		}
		
		#if( PREPAY_MODE == PREPAY_LOCAL )
		//费率电价重新刷新
		api_SetUpdatePriceFlag( eChangeCurRatePrice );
		#endif
		
		//切换费率
		CurrRatio = k;
		CurrRatioBak = k;
	}
}

//-----------------------------------------------
//函数功能: 初始化4G模块参数
//
//参数: 	无
//			
//返回值:  	无
//备注: 	只有厂内模式才允许设置只读参数
//-----------------------------------------------
static void FactoryInitGprsPara( void )
{
	BYTE Buf[33];
	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )
	{
		return;
	}
	memset( Buf, 0, sizeof(Buf) );
	//IP地址 端口号 APN
	memcpy(Buf, TCPIP_IP_Port_APNConst,sizeof(TCPIP_IP_Port_APNConst));
	api_ProcGprsTypePara(WRITE, eTCPIP_IP_Port_APN, Buf);
	//行政区码 终端地址
	memcpy(Buf, TCPIP_AreaCode_TerConst, sizeof(TCPIP_AreaCode_TerConst));
	api_ProcGprsTypePara(WRITE, eTCPIP_AreaCode_Ter, Buf);
	//APN用户名
	memcpy(Buf, TCPIP_APN_UseNameConst, sizeof(TCPIP_APN_UseNameConst));
	api_ProcGprsTypePara(WRITE, eTCPIP_APN_UseName, Buf);
	//APN密码
	memcpy(Buf, TCPIP_APN_PassWordConst, sizeof(TCPIP_APN_PassWordConst));
	api_ProcGprsTypePara(WRITE, eTCPIP_APN_PassWord, Buf);

	//IP地址 端口号 APN
	memcpy(Buf, MQTT_IP_Port_APNConst,sizeof(MQTT_IP_Port_APNConst));
	api_ProcGprsTypePara(WRITE, eMQTT_IP_Port_APN, Buf);
	//行政区码 终端地址
	memcpy(Buf, MQTT_AreaCode_TerConst, sizeof(MQTT_AreaCode_TerConst));
	api_ProcGprsTypePara(WRITE, eMQTT_AreaCode_Ter, Buf);
	//APN用户名
	memcpy(Buf, MQTT_APN_UseNameConst, sizeof(MQTT_APN_UseNameConst));
	api_ProcGprsTypePara(WRITE, eMQTT_APN_UseName, Buf);
	//APN密码
	memcpy(Buf, MQTT_APN_PassWordConst, sizeof(MQTT_APN_PassWordConst));
	api_ProcGprsTypePara(WRITE, eMQTT_APN_PassWord, Buf);	
}
//-----------------------------------------------
//函数功能: 初始化与表型有关的参数
//
//参数: 	无
//			
//返回值:  	无
//备注: 	只有厂内模式才允许设置只读参数
//-----------------------------------------------
static void FactoryInitMeterPara( void )
{
	BYTE Buf[32];//不能小于地理位置12字节wlk
	
	//只有厂内模式才允许设置只读参数
	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )
	{
		return;
	}
	memset( Buf, 0, sizeof(Buf) );
	
	//额定电压 ascii码     
	memcpy(Buf,RateVoltageConst,sizeof(RateVoltageConst));
	api_ProcMeterTypePara(WRITE, eMeterRateVoltage, Buf );
	//额定电流 ascii码     
	memcpy(Buf,RateCurrentConst,sizeof(RateCurrentConst));
	api_ProcMeterTypePara(WRITE, eMeterRateCurrent, Buf );
	//最大电流 ascii码     
	memcpy(Buf,MaxCurrentConst,sizeof(MaxCurrentConst));
	api_ProcMeterTypePara(WRITE, eMeterMaxCurrent, Buf );
	//最小电流 ascii码 
	memcpy(Buf,MinCurrentConst,sizeof(MinCurrentConst));
	api_ProcMeterTypePara(WRITE, eMeterMinCurrent, Buf );
	//转折电流 ascii码     
	memcpy(Buf,TurnCurrentConst,sizeof(TurnCurrentConst));
	api_ProcMeterTypePara(WRITE, eMeterTurningCurrent, Buf );
	//有功精度等级 ascii码
	memcpy(Buf,PPrecisionConst,sizeof(PPrecisionConst));
	api_ProcMeterTypePara(WRITE, eMeterPPrecision, Buf );
	//无功精度等级 ascii码 
	#if(SEL_RACTIVE_ENERGY == YES)
	memcpy(Buf,QPrecisionConst,sizeof(QPrecisionConst));
	api_ProcMeterTypePara(WRITE, eMeterQPrecision, Buf );
	#endif
	//有功脉冲常数         
	memcpy(Buf,(BYTE*)&ActiveConstantConst,sizeof(ActiveConstantConst));
	api_ProcMeterTypePara(WRITE, eMeterActiveConstant, Buf );
	#if(SEL_RACTIVE_ENERGY == YES)
	//无功脉冲常数          无功采用和有功一样的数据
	memcpy(Buf,(BYTE*)&ActiveConstantConst,sizeof(ActiveConstantConst));
	api_ProcMeterTypePara(WRITE, eMeterReactiveConstant, Buf );
	#endif
	//电表型号 ascii码     	
	memcpy(Buf,MeterModelConst,sizeof(MeterModelConst));
	api_ProcMeterTypePara(WRITE, eMeterMeterModel, Buf );	
	//电能表位置信息 	
	memcpy(Buf,MeterPositionConst,sizeof(MeterPositionConst));
	api_ProcMeterTypePara(WRITE, eMeterMeterPosition, Buf );
	//698.45协议版本号(数据类型:WORD)   
	memcpy(Buf,(BYTE*)&ProtocolVersionConst,sizeof(ProtocolVersionConst));
	api_ProcMeterTypePara(WRITE, eMeterProtocolVersion, Buf );
	//厂家软件版本号 ascii码 
	memcpy(Buf,SoftWareVersionConst,sizeof(SoftWareVersionConst));
	api_ProcMeterTypePara(WRITE, eMeterSoftWareVersion, Buf );
	//厂家软件版本日期
	memcpy(Buf,SoftWareDateConst,sizeof(SoftWareDateConst));
	api_ProcMeterTypePara(WRITE, eMeterSoftWareDate, Buf );
	//厂家硬件版本号 ascii码 
	memcpy(Buf,HardWareVersionConst,sizeof(HardWareVersionConst));
	api_ProcMeterTypePara(WRITE, eMeterHardWareVersion, Buf );
	//厂家硬件版本日期
	memcpy(Buf,HardWareDateConst,sizeof(HardWareDateConst));
	api_ProcMeterTypePara(WRITE, eMeterHardWareDate, Buf );

	//厂家编号  ascii码       
	memcpy(Buf,FactoryCodeConst,sizeof(FactoryCodeConst));
	api_ProcMeterTypePara(WRITE, eMeterFactoryCode, Buf );
	//软件备案号  ASCII码   
	memcpy(Buf,SoftRecordConst,sizeof(SoftRecordConst));
	api_ProcMeterTypePara(WRITE, eMeterSoftRecord, Buf );
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//客户编号
	memset(Buf,0x00,sizeof(FPara1->MeterSnPara.CustomCode));
	api_ProcMeterTypePara(WRITE, eMeterCustomCode, Buf );
	#endif
}


//-----------------------------------------------
//函数功能: 初始化规约密码
//
//参数: 	无
//			
//返回值:  	无
//
//备注: 	
//-----------------------------------------------
static void FactoryInitDlt645Password(void)
{
	TMuchPassword TmpMuchPassword;

	//密码初始化
	memcpy( &TmpMuchPassword, (void*)&MuchPasswordConst, sizeof(TMuchPassword) );

	//写入密码，自己计算校验
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.MuchPassword ), sizeof(TMuchPassword), (BYTE*)&TmpMuchPassword );
}


//-----------------------------------------------
//函数功能: 切换时间初始化为0
//
//参数: 	无
//			
//返回值:  	无
//
//备注: 	
//-----------------------------------------------
static void FactoryInitTableSwitchTime(void)
{
	// TSwitchTimePara TmpSwitchTimePara;

	// //初始化切换时间都为0，防止初始化时发生切换
	// memcpy( (BYTE *)&TmpSwitchTimePara, (BYTE*)&SwitchTimeParaConst, sizeof(TSwitchTimePara) );
	// //写入切换时间，自己计算校验
	// api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara), sizeof(TSwitchTimePara), (BYTE *)&TmpSwitchTimePara );

}

//-----------------------------------------------
//函数功能: 初始化公共假日
//
//参数: 	无
//			
//返回值:  	无
//
//备注: 	
//-----------------------------------------------
static void FactoryInitHoliday( void )
{
    BYTE i;
	TTimeHolidayTable TmpTimeHolidayTable;
	//初始化公共假日表为FFFFFFFFFFFFFF01
	memset( (BYTE *)&TmpTimeHolidayTable, 0xff, sizeof(TTimeHolidayTable) );
    for( i = 0; i < MAX_HOLIDAY; i++ )
    {
        TmpTimeHolidayTable.TimeHoliday[i].HolidaySegNum = 0x01;
    }
    
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.TimeHolidayTable), sizeof(TTimeHolidayTable), (BYTE *)&TmpTimeHolidayTable );
}


//-----------------------------------------------
//函数功能: 清电池工作时间
//
//参数: 	无
//			
//返回值:  	无
//
//备注: 	
//-----------------------------------------------
static void FactoryInitBatteryTime(void)
{
	DWORD BattTime = 0;
	
	//写入电池工作时间
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(BatteryTime), sizeof(DWORD), (BYTE *)&BattTime);
}

//-----------------------------------------------
//函数功能: 结算日时间初始化
//
//参数: 
//	无
//	
//  返回值:	
//  无
//
//备注:	  初始化调用
//-----------------------------------------------
void FactoryInitBilling(void)
{
	TBillingPara TmpBillingPara;
	
	memcpy( &TmpBillingPara.MonSavePara, &MonBillParaConst, sizeof(TMonSavePara)*MAX_MON_CLOSING_NUM );
	
	//写入切换时间，自己计算校验
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.BillingPara), sizeof(TBillingPara), (BYTE*)&TmpBillingPara );
}


//-----------------------------------------------
//函数功能: 初始化参数到默认值
//
//参数: 	无
//                    
//返回值:  	无
//备注:   
//-----------------------------------------------
void FactoryInitPara(void)
{
	BYTE i;
	WORD Addr;

	api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2), sizeof(TFPara2), (BYTE*)FPara2 );

	memcpy((BYTE*)&FPara2->CommPara.I485,(BYTE*)&CommParaConst,sizeof(TCommPara));
	// memcpy((BYTE*)&tfpara2bak->CommPara.I485,(BYTE*)&CommParaConst,sizeof(TCommPara));//更新该变量
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		SerialMap[i].SCIInit(SerialMap[i].SCI_Ph_Num);
	}
	//有功组合方式特征字 组合有功
	memcpy((BYTE*)&FPara2->EnereyDemandMode.byActiveCalMode, &EnereyDemandModeConst, sizeof(TEnereyDemandMode));
	
	Addr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2);
	api_VWriteSafeMem( Addr, sizeof(TFPara2), (BYTE*)FPara2 );
	CLEAR_WATCH_DOG;

    api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara1), sizeof(TFPara1), (BYTE*)FPara1 );
	
	memcpy((BYTE*)&FPara1->TimeZoneSegPara,(BYTE*)&TimeZoneSegParaConst,sizeof(TTimeZoneSegPara));
	Addr = GET_SAFE_SPACE_ADDR( ParaSafeRom.FPara1 );
	api_VWriteSafeMem( Addr, sizeof(TFPara1), (BYTE*)FPara1 );
	CLEAR_WATCH_DOG;
	
	//写当前套时区表
	// api_WriteTimeTable(0x01, GET_STRUCT_ADDR(TTimeAreaTable,TimeArea[0][0]), sizeof(TTimeAreaTable), (BYTE*)&TimeAreaTableConst1);
	//写备用套时区表
	// api_WriteTimeTable(0x81, GET_STRUCT_ADDR(TTimeAreaTable,TimeArea[0][0]), sizeof(TTimeAreaTable), (BYTE*)&TimeAreaTableConst2);
	
	for(i=0;i<(MAX_TIME_SEG_TABLE*2);i++)
	{
		//写当前套8个日时段表
		if( i < MAX_TIME_SEG_TABLE )
		{
			// if( TimeSegTableFlag[i] == 1 )
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst1);
			// }
			// else if( TimeSegTableFlag[i] == 2 )
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst2);
			// }
			// else if( TimeSegTableFlag[i] == 3 )
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst3);
			// }
			// else if( TimeSegTableFlag[i] == 4 )
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst4);
			// }
			// else
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst1);
			// }
		    

		}
		else//写备用套8个日时段表
		{

			// if( TimeSegTableFlag[i] == 1 )
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst1);
			// }
			// else if( TimeSegTableFlag[i] == 2 )
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst2);
			// }
			// else if( TimeSegTableFlag[i] == 3 )
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst3);
			// }
			// else if( TimeSegTableFlag[i] == 4 )
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst4);
			// }
			// else
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst1);
			// }
		}
	}

	api_FactoryInitRunHardFlag();
	api_FactoryInitProHardFlag();

	//密码初始化,初始化密码
	FactoryInitDlt645Password();
	
	// FactoryInitTableSwitchTime();

	FactoryInitGprsPara();

	FactoryInitMeterPara();
	
	#if( MAX_PHASE == 3 )
	api_InitSamplePara();
	#endif

	#if(SEL_EVENT_DI_CHANGE == YES)
	api_FactoryInitDIPara();
	#endif
		
	FactoryInitHoliday();
	
	// api_ClrProSafeModePara(3);//清零显式安全模式参数

	api_InitMessageTransPara();
	
	FactoryInitBatteryTime();
	
	FactoryInitBilling();
}

//-----------------------------------------------
//函数功能: 电表初始化
//
//参数:
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_FactoryInitTask( void )
{	
	if(SelEESoftAddrConst == YES)
	{
		api_FactoryInitEEPRomAddr();
	}
	//这个要放在LCD厂内初始化前面，否则会把LCD初始化好的参数改写
	FactoryInitPara();
	
	api_FactoryInitSyswatch();

	// api_FactoryInitFreeze();

	// api_FactoryInitEvent();
	// api_FacinitBT();
    
	#if (SEL_SEARCH_METER == YES )
	api_InitSchClctMeter();
	#endif
    #if (SEL_TOPOLOGY == YES )	 
    api_TopoParaInit();
    #endif
	
	// api_FactoryInitLcd();
	
//	ClearReport( eFACTORY_INIT );//清所有主动上报
	
	// FactoryInitRelay();
	
	api_FactoryInitRtc();
	
	// api_FactoryInitPrePay();

	FactoryInitProtocol( );
	
	// api_ModparaInit();

	api_FactoryInitSample();
	//初始化掉电时，不记录初始化自由事件，方便分析车间初始化时掉电问题
	if(api_CheckSysVol(eOnRunSpeedDetectPowerMode) == TRUE)
	{
		api_WriteFreeEvent(EVENT_SYS_INIT_BEGIN, 0);
	}
}


//-----------------------------------------------
//函数功能: 对脉冲常数进行合法性判断
//
//参数:		无
//						
//返回值:	有功脉冲常数
//		   
//备注:
//-----------------------------------------------
DWORD api_GetActiveConstant( void )
{
	if( (FPara2->ActiveConstant<10) 
		|| (FPara2->ActiveConstant>200000) 
		|| ((FPara2->ActiveConstant%10)!=0) )
	{
		ASSERT( 0, 0 );
		return ActiveConstantConst;
	}
	
	return FPara2->ActiveConstant;
}

//-----------------------------------------------
//函数功能: 将Para1参数从E2恢复
//
//参数:		无
//						
//返回值:	
//		   
//备注:低功耗上电后写时间前调用
//-----------------------------------------------
void api_ReadPara1FromEE(void)
{
	//监视当前参数
	//参数区1校验错误
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara1), sizeof(TFPara1), UN_REPAIR_CRC ) == FALSE )
	{
		//由EEProm恢复参数
		if( api_FreshParaRamFromEeprom(0) != TRUE )
		{
			api_SetError(ERR_FPARA1);
		}
	}
	else
	{
		api_ClrError(ERR_FPARA1);
	}
	
	if( FPara1->TimeZoneSegPara.Ratio > MAX_RATIO )
	{
		FPara1->TimeZoneSegPara.Ratio = 4;
	}
}



