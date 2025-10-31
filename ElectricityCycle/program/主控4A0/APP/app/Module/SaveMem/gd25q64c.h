//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	尹西科
//创建日期	2017-03-02
//描述		GD25Q64C和GD25Q32C的头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __GD25Q64C_H
#define __GD25Q64C_H

#if( (THIRD_MEM_CHIP==CHIP_GD25Q256C) || (THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//最大允许写入重试次数
#define FLASH_MAX_WRITE_COUNT	4
//一页大小
#define GD25Q64C_PAGE_SIZE	256

/************ 命令定义 ************/
//Enable wirte
#define GD25Q64C_CMD_WREN		0x06
//Disable wite
#define GD25Q64C_CMD_WRDI		0x04
//Read Status Register
#define GD25Q64C_CMD_RDSR		0x05 //0x05-S0:S7 or 0x35-S8:S15 or 0x15-S16:S23  
//Write Status Register
#define GD25Q64C_CMD_WRSR		0x01 //0x01-S0:S7 or 0x31-S8:S15 or 0x11-S16:S23 
//Read Data Bytes
#define GD25Q64C_CMD_READ		0x03
//Read Data Bytes at Higher Speed
#define GD25Q64C_CMD_FREAD		0x0B
//Page Program
#define GD25Q64C_CMD_PP			0x02
//Sector Erase
#define	GD25Q64C_CMD_SE			0x20
//32KB Block Erase
#define GD25Q64C_CMD_32BE		0x52
//64KB Block Erase
#define GD25Q64C_CMD_64BE		0xD8
//Chip Erase(CE)
#define GD25Q64C_CMD_CE			0x60 //or 0xC7
//Enable Reset
#define GD25Q64C_CMD_ER			0x66
//Reset
#define GD25Q64C_CMD_RE			0x99

//
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//
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
static WORD WaitFlashChipDoOver( BYTE No );

//-----------------------------------------------
//函数功能: 故障情况下，复位Flash芯片，内部函数
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:有些表，该芯片的复位管脚和采样芯片的复位管脚连在一起了   
//-----------------------------------------------
static void ResetFlash( void );

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
static BOOL WriteFlashPage( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf );

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
static BOOL WriteFlashPageLogic( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 擦除FLASH块，模块内部函数
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
//BOOL EraseExtFlashBlock(WORD No, DWORD Addr); 暂不需要

//-----------------------------------------------
//函数功能: 擦除FLASH全部，模块内部函数
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
//BOOL EraseExtFlashAll(WORD No, DWORD Addr); 暂不需要


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
BOOL ReadExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf);

//-----------------------------------------------
//函数功能: 写Flash函数，模块内部函数
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
BOOL WriteExtFlash(BYTE No, DWORD Addr, WORD Len, BYTE * pBuf);
//-----------------------------------------------
//函数功能: 写Flash操作，外部函数不擦除
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
BOOL WriteExtFlashUpDate( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf );

#endif//#if( (THIRD_MEM_CHIP==CHIP_GD25Q256C) || (THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )

#endif//#ifndef __GD25Q64C_H

