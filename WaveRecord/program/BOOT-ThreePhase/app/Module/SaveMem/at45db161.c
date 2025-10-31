//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		at45db161和at45db321的驱动文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "at45db161.h"
#include "SaveMem.h"

#if( (THIRD_MEM_CHIP == CHIP_AT45DB161) || (THIRD_MEM_CHIP == CHIP_AT45DB321) )

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
static BYTE WriteDB161ErrorTimes;
static BYTE g_byFlashErrTimes;	//flash存储器故障次数

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 地址转换，地址转换为45DB161格式，内部函数
//
//参数: 
//			Addr[in]		芯片的逻辑地址
//                    
//返回值:  	对芯片进行寻址的地址
//
//备注:   
//-----------------------------------------------
static DWORD Conver45DB116Addr(DWORD Addr)
{
	DWORD TrueAddr;

	//最高两位保留1
	TrueAddr = 0x00c00000;
	//扇区号
	TrueAddr += (Addr / AT45DB161B_PAGE_SIZE) << 10;
	//扇区内寻址
	TrueAddr += (Addr % AT45DB161B_PAGE_SIZE);

	return TrueAddr;
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
	WORD i;
	DWORD TrueAddr;

	ASSERT( (Addr+Len) <= THIRD_MEM_SPACE, 1 );

	TrueAddr = Conver45DB116Addr(Addr);

	//片选
	DoSPICS(No, TRUE);

	//写入命令
	api_UWriteSpi(AT45DB161_CMD_CAR);
	//写入地址
	api_UWriteSpi(TrueAddr>>16);
	api_UWriteSpi(TrueAddr>>8);
	api_UWriteSpi(TrueAddr);
	//写入缓冲字节
    api_UWriteSpi(0xfe);
    api_UWriteSpi(0xfe);
    api_UWriteSpi(0xfe);
    api_UWriteSpi(0xfe);

	//开始读数据
	for(i=0; i<Len; i++)
	{
		pBuf[i] = api_UWriteSpi(0);
	}

	DoSPICS(No, FALSE);

	return TRUE;
}


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
static WORD Wait45DB161DoOver(BYTE No)
{
	BYTE Status;
	WORD i;

	CLEAR_WATCH_DOG;
	DoSPICS(No, TRUE);
	api_UWriteSpi(AT45DB161_CMD_SRR);

	//最长延时200毫秒等待
	for(i=0; i<200; i++)
	{
		Status = api_UWriteSpi(0);

		//已经空闲
		if( Status & 0x80 )
		{
			break;
		}

		api_Delayms(1);
	}

	DoSPICS(No, FALSE);

	CLEAR_WATCH_DOG;// 2014年5月19日胡工说加这个兼容45db321更好

	if( i>= 200 )
	{
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 完成某个命令的子函数，不能操作数据，内部函数
//
//参数: 
//			No[in]			芯片序号，用于片选选择
//			Cmd[in]			命令
//			Addr[in]		操作地址
//                    
//返回值:  	TRUE:正确执行   FALSE:未正确执行   实际返回的还是芯片是否为空闲状态
//
//备注:   
//-----------------------------------------------
static WORD Do45DB161SingleCmd(BYTE No, BYTE Cmd, DWORD Addr)
{
	DoSPICS(No, TRUE);
	api_UWriteSpi(Cmd);
	api_UWriteSpi(Addr>>16);
	api_UWriteSpi(Addr>>8);
	api_UWriteSpi(Addr);
	DoSPICS(No, FALSE);

	//等待执行完成
	return Wait45DB161DoOver(No);
}


//-----------------------------------------------
//函数功能: 写入某个扇区，不能跨页操作，内部函数
//
//参数: 
//			No[in]			芯片序号，用于片选选择
//			Addr[in]		操作地址
//			Len[in]			写入长度（注意不要超过1个扇区）
//			pBuf[in/out]	写入的数据缓冲
//                    
//返回值:  	TRUE:正确执行   FALSE:未正确执行
//
//备注:   
//-----------------------------------------------
static WORD WriteAT45DB161BPage(BYTE No, DWORD Addr, WORD Len, BYTE * pBuf)
{
	BYTE Status;
	DWORD i;
	DWORD TrueAddr = Conver45DB116Addr(Addr);

	//数据读到缓冲
	if( Do45DB161SingleCmd(No, AT45DB161_CMD_MPB1T, TrueAddr) != TRUE )
	{
		return FALSE;
	}

	//修改缓冲
	DoSPICS(No, TRUE);
	api_UWriteSpi(AT45DB161_CMD_B1W);
	api_UWriteSpi(TrueAddr>>16);
	api_UWriteSpi(TrueAddr>>8);
	api_UWriteSpi(TrueAddr);
	//写入数据
	for(i=0; i<Len; i++)
	{
		api_UWriteSpi(pBuf[i]);
	}
	DoSPICS(No, FALSE);

	//缓冲数据写入主存储器
	if( Do45DB161SingleCmd(No, AT45DB161_CMD_B1MPBE, TrueAddr) != TRUE )
	{
		return FALSE;
	}

	//缓冲与主存储器比较
	if( Do45DB161SingleCmd(No, AT45DB161_CMD_MPB1C, TrueAddr) != TRUE )
	{
		return FALSE;
	}

	DoSPICS(No, TRUE);
	api_UWriteSpi(AT45DB161_CMD_SRR);
	Status = api_UWriteSpi(0);
	DoSPICS(No, FALSE);

	if( Status & 0x40 )
	{
		return FALSE;
	}

	return TRUE;
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
static void Reset45DB161B( void )
{	

	if( g_byFlashErrTimes < 200 )
	{
		g_byFlashErrTimes ++;
		if( g_byFlashErrTimes == 200 )
		{
			api_SetFollowReportStatus(eSTATUS_EEPROM_Error);
		}
	}
	//WriteDB161ErrorTimes==0xff说明芯片有错误，不能再复位
	if( WriteDB161ErrorTimes == 0xff )
	{
		return;
	}

	WriteDB161ErrorTimes++;

	if( WriteDB161ErrorTimes >= (FLASH_MAX_WRITE_COUNT+4) )
	{
		WriteDB161ErrorTimes = 0xff;//说明芯片有问题，不能再复位了
		api_SetError( ERR_WRITE_EEPROM3 );
	}
	else
	{
    	PowerUpResetExtFlash( 1 );
	}
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
void PowerUpResetExtFlash( BYTE Type )
{
	if( Type == 0 )
	{
		WriteDB161ErrorTimes = 0;
	}
	
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 );
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
static WORD WriteAT45DB161BPageLogic(BYTE No, DWORD Addr, WORD Len, BYTE * pBuf)
{
	DWORD i;
	
	for(i=0; i<FLASH_MAX_WRITE_COUNT; i++)
	{
		if( WriteAT45DB161BPage(No, Addr, Len, pBuf) == TRUE )
		{
			WriteDB161ErrorTimes = 0;
			g_byFlashErrTimes = 0;
			api_ClrError( ERR_WRITE_EEPROM3 );

			return TRUE;
		}

		//复位芯片
		Reset45DB161B();
	}

	return FALSE;
}


//-----------------------------------------------
//函数功能: 对地址取整，从整页开始，内部函数
//
//参数: 	
//			Addr[in]		未进行页取整的地址
//
//返回值:  	进行页取整后的地址
//
//备注:
//-----------------------------------------------
static DWORD AddrToInterge(DWORD Addr)
{
	DWORD i;
	i = Addr / AT45DB161B_PAGE_SIZE;
	i *= AT45DB161B_PAGE_SIZE;

	return i;
}


//-----------------------------------------------
//函数功能: 写外部Flash函数，模块内部函数
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
BOOL WriteExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf)
{
	DWORD TrueAddr;
	WORD PageLen;
	short WriteLen;

	ASSERT( (Addr+Len) <= THIRD_MEM_SPACE, 1 );

	//开始地址
	TrueAddr = Addr;

	WriteLen = Len;

	CLEAR_WATCH_DOG;
	//第一个扇区中数据的长度
	PageLen = AT45DB161B_PAGE_SIZE - (Addr - AddrToInterge(Addr));
	if( PageLen >= Len )
	{
		//所有写入数据都在一个扇区内
		PageLen = Len;
		WriteLen -= PageLen;
	}

	for(;;)
	{
		//写入一个扇区
		if( WriteAT45DB161BPageLogic(No, TrueAddr, PageLen, pBuf) != TRUE )
		{
			return FALSE;
		}

		//考察是否写入完成
		WriteLen -= PageLen;
		if( WriteLen <= 0 )
		{
			break;
		}

		//操作地址
		TrueAddr += PageLen;
		pBuf += PageLen;

		//计算长度
		if( WriteLen >= AT45DB161B_PAGE_SIZE )
		{
			//若剩余长度大于一个扇区
			PageLen = AT45DB161B_PAGE_SIZE;
		}
		else
		{
			//小于一个扇区则直接写入
			PageLen = WriteLen;
		}
	}

	return TRUE;
}


#endif//#if( (THIRD_MEM_CHIP == CHIP_AT45DB161) || (THIRD_MEM_CHIP == CHIP_AT45DB321) )


