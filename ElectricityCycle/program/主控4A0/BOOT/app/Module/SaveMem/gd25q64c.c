//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人    尹西科
//创建日期	2017-03-02
//描述		GD25Q64C驱动文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "gd25q64c.h"
#include "SaveMem.h"

#if( (THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )

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
static BYTE WriteFlashErrorTimes;
static BYTE g_byFlashErrTimes;	//flash存储器故障次数

//-----------------------------------------------
//函数功能: 查询芯片是否空闲，内部函数
//
//参数: 
//			No[in]			芯片序号，用于片选选择
//                    
//返回值:  	TRUE:芯片空闲   FALSE:芯片处于忙状态
//
//备注:   
//-----------------------------------------------
static WORD WaitFlashChipDoOver( BYTE No )
{
	BYTE Status;
	WORD i;
	
	CLEAR_WATCH_DOG;
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_RDSR);

	//兆易 最长延时350毫秒等待 扇区擦除5万次前最大150ms 5万次后最大300ms
	for(i=0; i<350; i++)
	{
		Status = api_UWriteSpi(eCOMPONENT_SPI_FLASH, 0);

		if( Status & 0x01 )//忙碌状态
		{
			;
		}
		else//已经空闲
		{
			break;
		}
		
		api_Delayms(1);
	}

	DoSPICS(No, FALSE);

	CLEAR_WATCH_DOG;// 2014年5月19日胡工说加这个兼容45db321更好

	if( i>= 350 )
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 擦除FLASH扇区，模块内部函数
//
//参数: 	
//			No[in]			写入芯片序号
//			Addr[in]		写入数据的起始地址
//			Len[in]			写入数据长度
//			pBuf[in]		存储要写入的数据
//
//返回值:  	TRUE:成功		FALSE：失败
//
//备注:
//-----------------------------------------------
BOOL EraseExtFlashSector( WORD No, DWORD Addr )
{
//	ASSERT( (Addr % SECTOR_SIZE) == 0, 1 );

	//写使能
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_WREN);
	DoSPICS(No, FALSE);	//写完命令必须把CS拉高、
	
	//片选
	DoSPICS(No, TRUE);
	//写入命令
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_SE);
	//写入地址
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 16);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 8);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr);

	DoSPICS(No, FALSE);//写完地址必须把片选拉高 否则不进行擦除操作
	
	if( WaitFlashChipDoOver( No ) != TRUE )
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 对Flash芯片进行断电强制复位，模块内函数
//
//参数: 	
//          Type	复位类型  0：上电     其他：故障调用
//          
//返回值:  	无
//
//备注:
//-----------------------------------------------
void PowerUpResetFlash( BYTE Type )
{
	if( Type == 0 )
	{
		WriteFlashErrorTimes = 0;
	}
	
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 );
}

//-----------------------------------------------
//函数功能: 故障情况下，复位Flash芯片，内部函数
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:有些表，该芯片的复位管脚和采样芯片的复位管脚连在一起了   
//-----------------------------------------------
static void ResetFlash( void )
{
	/** 软复位 **/
	/*
	//芯片复位会使当前的写操作终止，是否需要在复位前检查芯片的忙碌状态？
	//使能复位
	DoSPICS(No, TRUE);
	api_UWriteSpi(GD25Q64C_CMD_ER);
	DoSPICS(No, FALSE);	//写完命令必须把CS拉高
	
	//复位芯片
	DoSPICS(No, TRUE);
	api_UWriteSpi(GD25Q64C_CMD_RE);
	DoSPICS(No, FALSE);	//写完命令必须把CS拉高
	*/
	if( g_byFlashErrTimes < 200 )
	{
		g_byFlashErrTimes ++;
		if( g_byFlashErrTimes == 200 )
		{
//			api_SetFollowReportStatus(eSTATUS_EEPROM_Error);
		}
	}	
	/** 硬复位 **/
	//WriteDB161ErrorTimes==0xff说明芯片有错误，不能再复位
	if( WriteFlashErrorTimes == 0xff )
	{
		return;
	}

	WriteFlashErrorTimes++;

	if( WriteFlashErrorTimes >= (FLASH_MAX_WRITE_COUNT+4) )
	{
		WriteFlashErrorTimes = 0xff;//说明芯片有问题，不能再复位了
//		api_SetError( ERR_WRITE_EEPROM3 );
	}
	else
	{
    	PowerUpResetFlash( 1 );
	}
}

//-----------------------------------------------
//函数功能: 写入某个页，不能跨页操作，内部函数
//
//参数:
//			No[in]			芯片序号，用于片选选择
//			Addr[in]		操作地址
//			Len[in]			写入长度（注意不要超过1个扇区）
//			pBuf[in/out]	写入的数据缓冲
//
//返回值:  	TRUE:成功		FALSE:失败
//
//备注:
//-----------------------------------------------
static BOOL WriteFlashPage( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	BYTE CmpData[GD25Q64C_PAGE_SIZE];
	DWORD i;
	
//	ASSERT( (Addr+Len) <= THIRD_MEM_SPACE, 1 );
	
	if(WaitFlashChipDoOver( No ) != TRUE)
	{
		return FALSE;
	}
	
	//如果地址为扇区的起始地址 将该扇区擦除
	if( (Addr % SECTOR_SIZE) == 0 )
	{
		if( EraseExtFlashSector( No, Addr ) != TRUE )//擦除进行2次重试
		{
			EraseExtFlashSector( No, Addr );
		}
	}
	
	//写使能
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_WREN);
	DoSPICS(No, FALSE);	//写完命令必须把CS拉高
	
	//写地址
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_PP);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 16);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 8);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr);

	//写数据
	for( i = 0; i < Len; i++ )
	{
		api_UWriteSpi(eCOMPONENT_SPI_FLASH, pBuf[i]);
	}
	DoSPICS(No, FALSE);
	
	//禁止写
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_WRDI);
	DoSPICS(No, FALSE);	//写完命令必须把CS拉高
	
	//检查是否写完
	if( WaitFlashChipDoOver( No ) != TRUE )
	{
		return FALSE;
	}
	
	//回读比较
	if( ReadExtFlash( No, Addr, Len, CmpData ) == TRUE )
	{
		if( memcmp( pBuf, CmpData, Len ) != 0 )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 对Flash芯片进行页写操作，含重试，内部函数
//
//参数: 	
//			No[in]			芯片序号，用于片选选择
//			Addr[in]		操作地址
//			Len[in]			写入长度（注意不要超过1个扇区）
//			pBuf[in/out]	写入的数据缓冲
//
//返回值:  	TRUE:成功		FALSE:失败
//
//备注:
//-----------------------------------------------
static BOOL WriteFlashPageLogic( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	DWORD i;
	
	for(i=0; i<FLASH_MAX_WRITE_COUNT; i++)
	{
		if( WriteFlashPage(No, Addr, Len, pBuf) == TRUE )
		{
			WriteFlashErrorTimes = 0;
			g_byFlashErrTimes = 0;
//			api_ClrError( ERR_WRITE_EEPROM3 );

			return TRUE;
		}
		else
		{
			//记录异常事件记录
//			api_WriteSysUNMsg(SYSUN_FALSE_REPET_ERR);
		}
		
		//复位芯片
		ResetFlash();
	}
	
	return FALSE;
}

//---------------------------------------------------------------
//函数功能: 读取一页数据
//
//参数:
//			No[in]			芯片序号，用于片选选择
//			Addr[in]		读数据的起始地址
//			Len[in]			读出长度
//			pBuf[in/out]	读出数据存储缓冲
//
//返回值:  	TRUE:正确读出   FALSE:读出错误  实际没有判断，返回总是正确 
//
//备注:   
//---------------------------------------------------------------
BOOL ReadFlashPage( WORD No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	WORD i;
	
//	ASSERT( (Addr+Len) <= THIRD_MEM_SPACE, 1 );
	
	//片选
	DoSPICS(No, TRUE);
	//写入命令
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_READ);
	//写入地址
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 16);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 8);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr);

	//后续测试此处是否需要延时
	//开始读数据
	for(i=0; i<Len; i++)
	{
		pBuf[i] = api_UWriteSpi(eCOMPONENT_SPI_FLASH, 0);
	}

	DoSPICS( No, FALSE );

	return TRUE;
}
//-----------------------------------------------
//函数功能: 从Flash芯片内读取数据，模块内函数
//
//参数:
//			No[in]			芯片序号，用于片选选择
//			Addr[in]		读数据的起始地址
//			Len[in]			读出长度
//			pBuf[in/out]	读出数据存储缓冲
//
//返回值:  	TRUE:正确读出   FALSE:读出错误  实际没有判断，返回总是正确
//
//备注:
//-----------------------------------------------
BOOL ReadExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf)
{
	BYTE i,j,Times;
	BYTE ReadBuf[GD25Q64C_PAGE_SIZE];
	WORD DoLength;
		
	Times = Len/sizeof(ReadBuf);
	Times += 1;
	
	for(i=0; i<Times; i++)
	{
		if( i == (Times-1) )//最后一次
		{
			DoLength = Len-(WORD)sizeof(ReadBuf)*(Times-1);
		}
		else
		{
			DoLength = sizeof(ReadBuf);
		}
		
		for(j=0; j<2; j++)//最多重试2遍
		{
			ReadFlashPage(No, Addr, DoLength, pBuf);
			ReadFlashPage(No, Addr, DoLength, ReadBuf);
			if( memcmp( pBuf, ReadBuf, DoLength ) == 0 )//读两次进行比较，一致则认为读出正确
			{
				break;
			}
		}
		
//		ASSERT( j<2, 0 );
		
		Addr += DoLength;
		pBuf += DoLength;
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 写Flash操作，外部函数
//
//参数: 
// 			No[in]			芯片序号，用于片选选择
//			Addr[in]		操作地址
//			Len[in]			写入长度（注意不要超过1个扇区）
//			pBuf[in/out]	写入的数据缓冲                  
//返回值:  TRUE:成功		FALSE:失败
//
//备注:   
//-----------------------------------------------
BOOL WriteExtFlash( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	volatile DWORD TrueAddr;
	short WriteLen;
	WORD PageLen;
	
	// 地址取整
	TrueAddr = (Addr / GD25Q64C_PAGE_SIZE) * GD25Q64C_PAGE_SIZE;
	
	// 第一个页中数据长度
	PageLen = GD25Q64C_PAGE_SIZE - (WORD)(Addr - TrueAddr);
	
	// 要写入数据的长度
	WriteLen = (short)Len;
	
	// 是否所有数据都在一个扇区内
	if( PageLen > Len )
	{
		PageLen = Len;
		WriteLen -= PageLen;
	}
	
	// 写入数据的起始地址
	TrueAddr = Addr;
	for(;;)
	{
		// 写入一个页
		if( WriteFlashPageLogic(No, TrueAddr, PageLen, pBuf) != TRUE )
		{
			return FALSE;
		}
		
		// 观察是否写入完成
		WriteLen -= PageLen;
		if( WriteLen <= 0 )
		{
			// 已经写完
			break;
		}

		// 操作地址
		TrueAddr += PageLen;
		pBuf += PageLen;

		// 计算长度
		if( WriteLen >= GD25Q64C_PAGE_SIZE )
		{
			PageLen = GD25Q64C_PAGE_SIZE;
		}
		else
		{
			PageLen = WriteLen;
		}
	}
	return TRUE;
}


#endif//#if( (THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )

