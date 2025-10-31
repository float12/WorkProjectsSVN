//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		at45db161和at45db321的头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __AT45DB161_H
#define __AT45DB161_H

#if( (THIRD_MEM_CHIP == CHIP_AT45DB161) || (THIRD_MEM_CHIP == CHIP_AT45DB321) )

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//最大允许写入重试次数
#define FLASH_MAX_WRITE_COUNT	2
//一个扇区大小
#define AT45DB161B_PAGE_SIZE	528

//Continuous Array Read
#define AT45DB161_CMD_CAR		0xe8
//Main Memory Page Read
#define AT45DB161_CMD_MMPR		0xd2
//Buffer 1 Read
#define AT45DB161_CMD_B1R		0xd4
//Buffer 2 Read
#define AT45DB161_CMD_B2R		0xd6
//Status Register Read
#define AT45DB161_CMD_SRR		0xd7

//Buffer 1 Write
#define AT45DB161_CMD_B1W		0x84
//Buffer 2 Write
#define AT45DB161_CMD_B2W		0x87
//Buffer 1 to Main Memory Page Program with Built-in Erase
#define AT45DB161_CMD_B1MPBE	0x83
//Buffer 2 to Main Memory Page Program with Built-in Erase
#define AT45DB161_CMD_B2MPBE	0x86
//Buffer 1 to Main Memory Page Program without Built-in Erase
#define AT45DB161_CMD_B1MPOBE	0x88
//Buffer 2 to Main Memory Page Program without Built-in Erase
#define AT45DB161_CMD_B2MPOBE	0x89
//Page Erase
#define AT45DB161_CMD_PE		0x81
//Block Erase
#define AT45DB161_CMD_BE		0x50
//Main Memory Page Prgram through Buffer 1
#define AT45DB161_CMD_MPTB1		0x82
//Main Memory Page Prgram through Buffer 2
#define AT45DB161_CMD_MPTB2		0x85

//Main Memory Page to Buffer 1 Transter
#define AT45DB161_CMD_MPB1T		0x53
//Main Memory Page to Buffer 2 Transter
#define AT45DB161_CMD_MPB2T		0x55
//Main Memory Page to Buffer 1 compare
#define AT45DB161_CMD_MPB1C		0x60
//Main Memory Page to Buffer 2 compare
#define AT45DB161_CMD_MPB2C		0x61
//Auto Page Rewrite through Buffer 1
#define AT45DB161_CMD_APRB1		0x58
//Auto Page Rewrite through Buffer 2
#define AT45DB161_CMD_APRB2		0x59

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
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
BOOL WriteExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf);

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
void PowerUpResetExtFlash( BYTE Type );


#endif//#if( (THIRD_MEM_CHIP == CHIP_AT45DB161) || (THIRD_MEM_CHIP == CHIP_AT45DB321) )

#endif//#ifndef __AT45DB161_H


