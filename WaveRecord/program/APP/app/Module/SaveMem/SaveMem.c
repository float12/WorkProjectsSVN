//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		存储模块的接口文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "gd25q64c.h"
#include "SaveMem.h"

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

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  对缓冲内的数据取反
//
//参数:      p[in] 			数据的缓冲
//
//           Length[in]		数据长度
//
//返回值:
//
//备注内容:  无
//--------------------------------------------------
void ReverseInData( BYTE *p, WORD Length )
{
	WORD i;
	for( i = 0; i < Length; i++ )
	{
		p[i] ^= 0xff;
	}
}

//-----------------------------------------------
//函数功能: 读安全空间,要求长度必须是结构的长度，包括校验在内，
//			而且安全空间的第二份一定是取反存储，校验固定认为是CRC32
//
//参数: 
//			Addr[in]				起始地址
//			Length[in]				数据长度，整个结构的长度，包括校验
//			pBuf[in/out]			存放读出的数据，包括校验
//                    
//返回值:  	TRUE:		有一份对则认为是对
//			FALSE：		两份都不对
//备注:   
//-----------------------------------------------
WORD api_VReadSafeMem( WORD Addr, WORD Length, BYTE * pBuf )
{
    if( (Length == 0) || (Length > 400) )//分块大小写入不够改为400字节
    {
        return FALSE;
    }
	
	if( ReadEEPRom1(Addr, Length, pBuf) != FALSE )
	{
		//防止ReadEEPRom1读EE错误未更新数据 但是传入缓存原值CRC正确不读取EE2
		if( lib_CheckSafeMemVerify( pBuf, Length, UN_REPAIR_CRC ) == TRUE )
		{
			return TRUE;
		}
	}
	
	if( ReadEEPRom2(Addr, Length, pBuf) != FALSE )
	{
		ReverseInData( pBuf, Length );
	
		if( lib_CheckSafeMemVerify( pBuf, Length, UN_REPAIR_CRC ) == TRUE )
		{
			return TRUE;
		}
	}
		
	return FALSE;
}


//-----------------------------------------------
//函数功能: 写安全空间,要求长度必须是结构的长度，包括校验在内，
//			而且安全空间的第二份是取反存储,校验固定认为是CRC32，如果校验不对，该函数会自动添加
//
//参数: 
//			Addr[in]				起始地址
//			Length[in]				数据长度，整个结构的长度，包括校验
//			pBuf[in/out]			存放读出的数据，包括校验
//                    
//返回值:  	TRUE:正确写入了两份数据	FALSE:有一份或两份数据没有正确写入
//
//备注:   
//-----------------------------------------------
WORD api_VWriteSafeMem( WORD Addr, WORD Length, BYTE * pBuf )
{
	WORD Result[2];
	BYTE Buf[400+30];

	if(  (Length == 0) || (Length > 400) )//分块大小写入不够改为400字节
    {
        return FALSE;
    }
	
	lib_CheckSafeMemVerify( pBuf, Length, REPAIR_CRC );
	memcpy( Buf, pBuf, Length );
	Result[0] = WriteEEPRom1( Addr, Length, pBuf );
	
	ReverseInData( Buf, Length );
	Result[1] = WriteEEPRom2( Addr, Length, Buf );
	
	//只有两片EE全部写入错误才返回错误 其他情况均返回正确
	if( (Result[0] == FALSE) && (Result[1] == FALSE) )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


//-----------------------------------------------
//函数功能: 清安全空间,要求长度必须是结构的长度，包括校验在内，
//			而且安全空间的第二份是取反存储,校验固定认为是CRC32，该函数会自动添加校验
//
//参数: 
//			Addr[in]				起始地址
//			Length[in]				数据长度，整个结构的长度，包括校验
//                    
//返回值:  	TRUE:正确写入了两份数据	FALSE:有一份或两份数据没有正确写入
//
//备注:   
//-----------------------------------------------
WORD api_ClrSafeMem( WORD Addr, WORD Length )
{
	WORD Result;
	BYTE Buf[2000];//申请不定长数据
	
	if( Length > 2000 )
	{
		return FALSE;
	}

	// 申请缓冲
	memset( Buf, 0x00, Length );
	Result = api_VWriteSafeMem( Addr, Length, Buf );
	
	return Result;
}


//-----------------------------------------------
//函数功能: 管理存储模块的上电初始化工作
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_PowerUpSave( void )
{
	InitEEPRom();
}

//-----------------------------------------------
//函数功能: 读所有存储芯片指定地址数据，为厂内功能单独写的函数
//
//参数: 
//			No[in]			指定读哪一片EEPROM
//							0x01：第一片EEPROM
//							0x02：第二片EEPROM
//							0x11：第一片Flash
//							0x21: 第一片铁电
//							0x31: 读连续空间
//			Addr[in]		要读的起始地址（芯片内偏移地址）
//			Length[in]		要读数据的长度
//			pBuf[out]		要读出数据的缓冲
//                    
//返回值:  	读出数据的长度，如果为0，表示失败
//
//备注:   
//-----------------------------------------------
WORD api_ReadSaveMem( BYTE No, DWORD Addr, WORD Length, BYTE *pBuf )
{
	switch( No )
	{
		case 0x01:
			UReadAt25640( CS_SPI_256401, Addr, Length, pBuf );
			break;
		case 0x02:
			UReadAt25640( CS_SPI_256402, Addr, Length, pBuf );
			break;
		#if( (THIRD_MEM_CHIP==CHIP_GD25Q256C) ||(THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )
		case 0x11:
			ReadExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
			break;
		#endif
		case 0x21://不支持铁电
			return 0;
		case 0x31:
			api_ReadFromContinueEEPRom( Addr, Length, pBuf );
			break;
		default:
			return 0;
	}
	
	return Length;
}

