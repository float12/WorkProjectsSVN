//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		连续空间驱动程序
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
//连续存储管理结构
typedef struct TConMemStruct_t
{
	//芯片类型
	WORD ChipType;
	//此片内开始物理地址
	DWORD ConMemPhysicalAddr;
	//能够作为连续空间的物理长度
	DWORD ConMemPhysicalSize;
	//逻辑的连续空间地址
	DWORD ConMemLogicAddr;
	//逻辑的连续空间长度
	DWORD ConMemLogicSize;
}TConMemStruct;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//连续空间存储管理
const TConMemStruct ConMemStruct[] =
{
	//芯片类型，			芯片上物理开始地址，		能够作为连续空间的物理长度，	逻辑的连续空间地址，                          	逻辑的连续空间长度

	//第一个基本存储芯片
	{MASTER_MEMORY_CHIP, 	LAST_ADDR_256401, 			SINGLE1_CONTINUE_SIZE, 			0,               								(SINGLE1_CONTINUE_SIZE)},

	//第二个基本存储芯片
	{SLAVE_MEM_CHIP, 		LAST_ADDR_256402, 			SINGLE2_CONTINUE_SIZE, 			SINGLE1_CONTINUE_SIZE,   						(SINGLE1_CONTINUE_SIZE+SINGLE2_CONTINUE_SIZE)},

};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 读写连续空间，内部函数
//
//参数: 
//			Addr[in]		要读写的起始地址
//			Length[in]		要读写的长度
//			pBuf[in/out]	写和读操作时的缓冲
//			Type[in]		WRITE:写操作  READ：读操作
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:   
//-----------------------------------------------
static BOOL DoContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf, WORD Type )
{
	short TempLen;
	WORD ChipCs,i,DoLength;
	BOOL (*pFunc)(WORD No, DWORD Addr, WORD Length, BYTE * Buf);

	//检查错误 这样可以避免从后面又写回去 负荷曲线是以记录个数为单位的
	if( (Addr+Length) > CONTINUE_TOTAL_SIZE )//详细见故障信息录入:FLASH末地址数据无法写入故障
	{
		//记录异常事件
		api_WriteSysUNMsg( OVERWRITE_CONTINUE_SPACE );

		return FALSE;
	}

	TempLen = Length;

	//查询所有存储芯片
	for(i=0; i<(sizeof(ConMemStruct)/sizeof(TConMemStruct)); i++)
	{
		//若开始地址小于此片包含的连续空间长度，则需要写入该片
		if( Addr < ConMemStruct[i].ConMemLogicSize )
		{
			//写入全部完成
			if( TempLen <= 0 )
			{
				break;
			}

			//考察写入长度
			if( (Addr+TempLen) <= ConMemStruct[i].ConMemLogicSize )
			{
				//写入全部数据
				DoLength = TempLen;
			}
			else
			{
				//写入部分数据，长度为芯片总大小减去物理的开始地址
				DoLength = ConMemStruct[i].ConMemPhysicalSize-(Addr-ConMemStruct[i].ConMemLogicAddr);
			}

			if( Type == WRITE )
			{
				pFunc = UWriteAt25640;
			}
			else
			{
				pFunc = UReadAt25640;
			}

		#if((MAX_PHASE == 1) && (SLAVE_MEM_CHIP != CHIP_NO))
			#warning "连续空间中, 贝岭作为第一片, 复旦E2作为第二片; 参数大部分处于第一片E2, (复旦E2特定工况下, 数据变位)"
			if( i == 0 )
			{
				ChipCs = CS_SPI_256402;
			}
			else if( i == 1 )
			{
				ChipCs = CS_SPI_256401;
			}
		#else
			if( i == 0 )
			{
				ChipCs = CS_SPI_256401;
			}
			else if( i == 1 )
			{
				ChipCs = CS_SPI_256402;
			}
		#endif
			else
			{
				return FALSE;
			}

			//写入数据
			//芯片序号 － 按照物理序号
			//芯片类型 － 给定类型
			//开始地址 － 当前开始地址-本片的逻辑开始地址+本片逻辑开始地址
			//写入长度 － Len
			if( (*pFunc)( ChipCs, (Addr-ConMemStruct[i].ConMemLogicAddr+ConMemStruct[i].ConMemPhysicalAddr), DoLength, pBuf) != TRUE )
			{
				return FALSE;
			}

			pBuf += DoLength;
			TempLen -= DoLength;
			Addr += DoLength;
		}
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 写连续空间函数
//
//参数: 
//			Addr[in]		要写的起始地址
//			Length[in]		要写的长度
//			pBuf[in/out]	写操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:   
//-----------------------------------------------
BOOL api_WriteToContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf)
{
	return DoContinueEEPRom(Addr, Length, pBuf, WRITE);
}


//-----------------------------------------------
//函数功能: 从连续空间读数据的函数
//
//参数: 
//			Addr[in]		要读的起始地址
//			Length[in]		要读的长度
//			pBuf[in/out]	读操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:   
//-----------------------------------------------
BOOL api_ReadFromContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf)
{
	BYTE i,j,DoLength,Times,Result;
	BYTE ReadBuf[64];
		
	Result = TRUE;
	Times = Length/sizeof(ReadBuf);
	Times += 1;
	
	for(i=0; i<Times; i++)
	{
		if( i == (Times-1) )//最后一次
		{
			DoLength = Length-(WORD)sizeof(ReadBuf)*(Times-1);
		}
		else
		{
			DoLength = sizeof(ReadBuf);
		}
		
		for(j=0; j<2; j++)//最多重试2遍
		{
			DoContinueEEPRom(Addr, DoLength, pBuf, READ);
			DoContinueEEPRom(Addr, DoLength, ReadBuf, READ);
			if( memcmp( pBuf, ReadBuf, DoLength ) == 0 )//读两次进行比较，一致则认为读出正确
			{
				break;
			}
		}
				
		if( j == 2 )
		{
			ASSERT( 0, 0 );
			Result = FALSE;
		}
		
		Addr += DoLength;
		pBuf += DoLength;
	}
	
	return Result;
}


//-----------------------------------------------
//函数功能: 把大片的连续空间写成0，主要为事件清零等操作使用。不判断是否操作成功
//
//参数: 
//			Addr[in]		要清除的起始地址
//			Length[in]		要清除的长度
//                    
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void api_ClrContinueEEPRom(DWORD Addr, WORD Length)
{
	WORD i, j, k;
	BYTE Array[128];

	i = ( Length / sizeof(Array) );
	j = ( Length % sizeof(Array) );

	memset( (void *)&Array, 0, sizeof(Array) );

	for(k=0; k<i; k++)
	{
		api_WriteToContinueEEPRom( Addr, sizeof(Array), Array );

		//取地址
		Addr += sizeof(Array);

		CLEAR_WATCH_DOG;
	}

	api_WriteToContinueEEPRom( Addr, j, Array );
}


//-----------------------------------------------
//函数功能: 读连续空间的数据及备份,要求长度必须是结构的长度，包括校验在内，
//			第二份取反存储，校验固定认为是CRC32
//
//参数: 
//			Addr1[in]				第一份在连续空间的起始地址
//			Addr2[in]				第二份在连续空间的起始地址,若为0，则不读第二份
//			Length[in]				数据长度，整个结构的长度，包括校验
//			pBuf[in/out]			存放读出的数据，包括校验
//                    
//返回值:  	TRUE:		有一份对则认为是对
//			FALSE：		两份都不对
//备注:   
//-----------------------------------------------
WORD api_VReadContinueMem( DWORD Addr1, DWORD Addr2, BYTE Length, BYTE * pBuf )
{
	DoContinueEEPRom(Addr1, Length, pBuf, READ);
	
	if( lib_CheckSafeMemVerify( pBuf, Length, UN_REPAIR_CRC ) == TRUE )
	{
		return TRUE;
	}
	
	if( Addr2 != 0 )
	{
		DoContinueEEPRom( Addr2, Length, pBuf, READ );

		ReverseInData( pBuf, Length );

		if( lib_CheckSafeMemVerify( pBuf, Length, UN_REPAIR_CRC ) == TRUE )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

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
//备注:此函数只读取Flash中冻结事件数据区
//-----------------------------------------------
BOOL api_ReadMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	return ReadExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
}

//-----------------------------------------------
//函数功能: 写冻结事件数据区,带地址校验
//
//参数: 
//			Addr[in]		要写的起始地址
//			Length[in]		要写的长度
//			pBuf[in/out]	写操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:此函数只写Flash中冻结事件数据区
//-----------------------------------------------
BOOL api_WriteMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	return WriteExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
}





