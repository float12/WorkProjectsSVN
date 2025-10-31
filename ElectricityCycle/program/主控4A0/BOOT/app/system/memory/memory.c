//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.9
//描述		特别提醒，本工程其他任何地方都不能再使用malloc函数！！！！！！
//			对Ram进行管理和分配，采用堆的起始作为起始地址，逆栈生长
// 			注意申请、释放缓冲，由于容易忘记释放缓冲，建议最好少用
// 			采用逆栈方式－先分配后释放的原则。
// 			只要缓冲大小足够，可以多次分配，但有最大次数限制。
// 			大循环中要检测，必须为空。
// 			原则上都要采用断言方式查找错误
// 			应当注意边界问题，字对齐还是字节对齐！由于采用的是堆的起始地址，因此不存在对齐问题
// 			当前分配大小，0为无效
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// 最大嵌入分配20次,不宜太多
#define MAX_ALLOC_NUM			20
#define COMMON_RAM_BUF_SIZE 	2048//虽然判断这个大小，但实际应用中如果还有剩余ram，也可以继续用

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static WORD AllocBlockLen[MAX_ALLOC_NUM];
static BYTE *pCommRam;
static BYTE *pCommRamBak;

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 初始化内存分配，获取堆的起始地址作为内存分配的开始地址
//
//参数: 无
//                    
//返回值:  无
//
//备注:   
//-----------------------------------------------
void api_InitAllocBuf(void)
{
	memset( AllocBlockLen, 0, sizeof(AllocBlockLen) );
	pCommRam = malloc( 4 );
	free(pCommRam);
	pCommRamBak = pCommRam;//由于采用的是malloc函数，因此能保证4字节对齐问题
}


//-----------------------------------------------
//函数功能: 申请缓冲
//
//参数: 
//	*BufSize[in]:	申请要分配内存的大小
//                    
//返回值:  申请内存的指针
//
//备注:当申请长度为0时，按照28个字节分配，出去后*BufSize还是为0
//		当申请长度大于COMMON_RAM_BUF_SIZE时，按照28个字节分配，出去后将*BufSize置为0   
//-----------------------------------------------
void *api_AllocBuf(WORD *BufSize)
{
	BYTE i;
	WORD j,wLen;

	ASSERT(*BufSize != 0, 0);
	
	if( *BufSize > COMMON_RAM_BUF_SIZE )
	{
		*BufSize = 0;		
	}
	
	if( *BufSize == 0 )
	{
		wLen = 28;
		ASSERT(0, 0);
	}
	else
	{
		wLen = *BufSize;
	}
	
	//检查一下指针
	if( pCommRam != pCommRamBak )
	{
		api_InitAllocBuf();
		api_WriteSysUNMsg(ALLOCBUF_POINT_ERROR);
		
		Reset_CPU();
	}

	// 取整，以便结构映射，不区分CPU，统一按4字节对齐
	wLen += 3;
	wLen &= 0xfffc;
	
	j = 0;
	for(i=0; i<MAX_ALLOC_NUM; i++)
	{
		if( AllocBlockLen[i] == 0 )
		{
			AllocBlockLen[i] = wLen;
			break;
		}
		else
		{
			j += AllocBlockLen[i];
		}
	}

	// 是否大于最大的分配管理单元
	ASSERT( i < MAX_ALLOC_NUM, 1 );

	// 是否大于最大缓冲
	ASSERT( (j+wLen) <= COMMON_RAM_BUF_SIZE, 1 );

	return (void *)((DWORD)pCommRam+j);
}


//-----------------------------------------------
//函数功能: 释放缓冲
//
//参数: 
//	pBuf[in]:	要释放内存的指针，要释放的大小由内存管理自己负责
//                    
//返回值:  无
//
//备注:   
//-----------------------------------------------
void api_FreeBuf(void *pBuf)
{
	BYTE i;
	WORD j;

	j = 0;
	for(i=0; i<MAX_ALLOC_NUM; i++)
	{
		// 查询最后一块
		if( AllocBlockLen[i] == 0 )
		{
			// 表不能为空
			ASSERT( i != 0, 1 );

			// 去掉最后一次申请的
			if( AllocBlockLen[i-1] == 0xffff )
			{
				j -= 0;
			}
			else
			{
				j -= AllocBlockLen[i-1];
			}

			// 必须为最后一块申请的大小
			ASSERT( pBuf == ((BYTE *)((DWORD)pCommRam+j)), 1 );

			// 最后一块清空
			AllocBlockLen[i-1] = 0;

			// 退出
			break;
		}
		else
		{
			if( AllocBlockLen[i] == 0xffff )
			{
				j += 0;
			}
			else
			{
				j += AllocBlockLen[i];
			}
		}
	}
}


//-----------------------------------------------
//函数功能: 缓冲检查，防止缓冲未释放，导致内存消耗完，在主循环调用
//
//参数: 无
//                    
//返回值:  	TRUE		缓冲检查正常，没有未释放的缓冲
//			FALSE		有未释放的缓冲
//
//备注:   
//-----------------------------------------------
BOOL api_CheckAllocBuf(void)
{
	BYTE i;

	for(i=0; i<MAX_ALLOC_NUM; i++)
	{
		if( AllocBlockLen[i] != 0 )
		{
			// 在调试模式下，直接断言错误
			ASSERT( 0 != 0, 1 );

			memset( AllocBlockLen, 0, sizeof(AllocBlockLen));

			return FALSE;
		}
	}

	return TRUE;
}


