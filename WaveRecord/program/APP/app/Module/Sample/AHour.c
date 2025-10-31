//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	张玉猛
//创建日期	2018.10.30
//描述		安时值计算处理主文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"

#if( SEL_AHOUR_FUNC == YES )

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

static __no_init TAHour	AhourRam;                           // RAM里的安时值，单位0.01As

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 累计安时值
//
//参数:  无
//
//返回值:	无
//
//备注: 一秒钟累计一次
//-----------------------------------------------
void AccumulateAHour( void )
{
	DWORD	dwCurrent;
	BYTE i;

	// 检查RAM值
	if(lib_CheckSafeMemVerify( (BYTE *)&AhourRam, sizeof(TAHour), UN_REPAIR_CRC ) == FALSE)
	{
		memset( (BYTE *)&AhourRam, 0x00, sizeof(TAHour) );
	}

	for(i = 0; i < MAX_PHASE; i++)
	{
		api_GetRemoteData( PHASE_A + REMOTE_I + 0x1000 * i, DATA_HEX, 2, 4, (BYTE *)&dwCurrent );   // 两位小数

		AhourRam.AHour[1 + i] += dwCurrent;
		AhourRam.AHour[0] += dwCurrent; // 总
	}

	// 计算校验
	AhourRam.CRC32 = lib_CheckCRC32( (BYTE *)&AhourRam, sizeof(TAHour) - sizeof(DWORD) );
}

//-----------------------------------------------
//函数功能: 转存安时值
//
//参数:  无
//
//返回值:	无
//
//备注:（1）掉电转存 （2）1小时转存一次
//-----------------------------------------------
void SwapAHour( void )
{
	// 检查RAM值
	if(lib_CheckSafeMemVerify( (BYTE *)&AhourRam, sizeof(TAHour), UN_REPAIR_CRC ) == FALSE)
	{
		memset( (BYTE *)&AhourRam, 0x00, sizeof(TAHour) );
		AhourRam.CRC32 = lib_CheckCRC32( (BYTE *)&AhourRam, sizeof(TAHour) - sizeof(DWORD) );
	}

	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( AHourConRom ), sizeof(TAHour), (BYTE *)&AhourRam );

}

//-----------------------------------------------
//函数功能: 检查安时值
//
//参数:  无
//
//返回值:	无
//
//备注:
//-----------------------------------------------
void PowerUpAHour( void )
{
	// 检查
	if(lib_CheckSafeMemVerify( (BYTE *)&AhourRam.AHour, sizeof(TAHour), UN_REPAIR_CRC ) == FALSE)
	{
		// 从EEP恢复
		if(api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( AHourConRom ), sizeof(TAHour), (BYTE *)&AhourRam ) == FALSE)
		{
			if(lib_CheckSafeMemVerify( (BYTE *)&AhourRam.AHour, sizeof(TAHour), UN_REPAIR_CRC ) == FALSE)
			{
				// EEP的也不对,则清零
				memset( (BYTE *)&AhourRam.AHour, 0x00, sizeof(TAHour) );
				// 计算校验
				AhourRam.CRC32 = lib_CheckCRC32( (BYTE *)&AhourRam, sizeof(TAHour) - sizeof(DWORD) );
			}
		}
	}
}

//-----------------------------------------------
//函数功能: 清安时值
//
//参数:  无
//
//返回值:	无
//
//备注:
//-----------------------------------------------
void api_ClrAHour( void )
{
	memset( (BYTE *)&AhourRam, 0x00, sizeof(TAHour) );

	// 计算校验
	AhourRam.CRC32 = lib_CheckCRC32( (BYTE *)&AhourRam, sizeof(TAHour) - sizeof(DWORD) );
	// 清0.01Ah
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( AHourConRom ), sizeof(TAHour), (BYTE *)&AhourRam );
}

//-----------------------------------------------
//函数功能: 读安时值
//
//参数:  Type[in]: ePHASE_TYPE 总ABC
//		DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
// 		pBuf[out]: 指向返回数据的指针
//
//返回值:	无
//
//备注:   TRUE   正确
//		   FALSE  错误
//-----------------------------------------------
BOOL api_GetAHourData( BYTE Type, BYTE DataType, BYTE *pBuf )
{
	DWORD dw;

	dw = AhourRam.AHour[Type] / 3600;

	if(DataType == DATA_BCD)
	{
		dw = lib_DWBinToBCD( dw );
	}

	memcpy( pBuf, (BYTE *)&dw, sizeof(DWORD) );

	return TRUE;
}


#endif//#if( SEL_AHOUR_FUNC == YES )

