//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		连续空间驱动程序
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "at45db161.h"
#include "gd25q64c.h"
#include "SaveMem.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------



//-----------------------------------------------
//函数功能: 读冻结事件数据区
//
//参数: 
//			Addr[in]		要读的起始地址
//			Length[in]		要读的长度
//			pBuf[in/out]	读操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:配置flash的读flash区域，没有flash的读eeprom中数据   
//-----------------------------------------------
BOOL api_ReadMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	#if(THIRD_MEM_CHIP == CHIP_NO)
	return api_ReadFromContinueEEPRom( Addr, Length, pBuf );
	#else
	return ReadExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
	#endif
}

//-----------------------------------------------
//函数功能: 写冻结事件数据区
//
//参数: 
//			Addr[in]		要写的起始地址
//			Length[in]		要写的长度
//			pBuf[in/out]	写操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:配置flash的写到flash区域，没有flash的写到eeprom中   
//-----------------------------------------------
BOOL api_WriteMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	#if(THIRD_MEM_CHIP == CHIP_NO)
	return api_WriteToContinueEEPRom( Addr, Length, pBuf );
	#else
	return WriteExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
	#endif
}





