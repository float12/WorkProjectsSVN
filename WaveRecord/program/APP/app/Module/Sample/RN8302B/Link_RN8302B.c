//----------------------------------------------------------------------
//Copyright (C) 2016-2023 烟台东方威思顿电气股份有限公司低压台区产品部
//创建人	   
//创建日期	2023.05.29
//描述		采样芯片RN8302B通信链路层文件
//修改记录	
//----------------------------------------------------------------------

#include "AllHead.h"
#include "RN8302B.h"

#if( SAMPLE_CHIP == CHIP_RN8302B )

//-----------------------------------------------
//      本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//      全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//      本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//      内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//      函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 读RN8302B寄存器
//
//参数:		Addr[in],	地址	 
//			pBuf[out],	读出数据!!!!!!注意该数据不会先清零
//			Len    		长度（读取）
//
//返回值: 	TRUE/FALSE
//
//备注:     无    
//-----------------------------------------------
BOOL Link_ReadSampleReg( WORD Addr, BYTE *pBuf ,BYTE bLen)
{
	BYTE bSum = 0,bData,i;
	
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );

	bData = (BYTE)Addr;
	bSum += bData;
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, bData );

	bData = (((BYTE)(Addr >> 4)) & 0xf0);
	bSum += bData;
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, bData);

	api_Delay10us( 1 );//速度大于200k时需要延时3us再读数据

	for(i = bLen; i > 0; i--)
	{
		pBuf[i - 1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		bSum += pBuf[i - 1];
	}

	bSum = ~bSum;

	if(api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff ) != bSum)
	{
		DoSPICS(CS_SPI_SAMPLE, FALSE );

		api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
		return FALSE;
	}

	DoSPICS(CS_SPI_SAMPLE, FALSE );

	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 1 );
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 写RN8302B寄存器
//
//参数:		Addr[in],	地址	 
//			pBuf[out],	写入数据，4字节（高在后，低在前）
//
//返回值: 	TRUE/FALSE
//
//备注:     写寄存器固定为1字节  
//-----------------------------------------------
BOOL Link_WriteSampleReg( WORD Addr, BYTE *pBuf ,BYTE bLen)
{
	BYTE bSum = 0,bData,i;
	
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
	
	DoSPICS( CS_SPI_SAMPLE, TRUE );//片选
	
	bData = (BYTE)Addr;
	bSum += bData;
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, bData );

	bData = (((BYTE)(Addr >> 4)) & 0xf0) + 0x80;
	bSum += bData;
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, bData);

	for(i = bLen; i > 0; i--)
	{
		api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, pBuf[i - 1] );//数据
		bSum += pBuf[i - 1];
	}

	bSum = ~bSum;
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, bSum );//校验和

	DoSPICS( CS_SPI_SAMPLE, FALSE);
	
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 2 );
        
    return TRUE;
}


//-----------------------------------------------
//函数功能: 写校表数据寄存器
//
//参数:		Addr[in] 寄存器地址	 
//			pBuf[in] 要写入的数据,写入低三个字节，最高字节没用
//			bLen[in] 长度
//返回值: 	TRUE/FALSE
//		    
//备注: 
//-----------------------------------------------
BOOL WriteSampleReg(WORD Addr, BYTE *pBuf, BYTE bLen)
{
	BYTE Datat1[4]={0,0,0,0}, RepeatTime;
	BOOL Status = FALSE;

	if(bLen > 4)
	{
		return FALSE;
	}
	for(RepeatTime = 0; RepeatTime < 3; RepeatTime++)
	{
		Link_WriteSampleReg(Addr, pBuf, bLen);
		Link_ReadSampleReg(0x18D, Datat1, 4);

		if(memcmp(pBuf, Datat1, bLen) != 0)
		{
			Status = FALSE;
		}
		else
		{
			Status = TRUE;
			break;
		}
	}

	return Status;
}
//-----------------------------------------------
//函数功能: 对RN8302的特殊命令操作
//
//参数:		Cmd[in]	 
//					CH=0xa6,写允许。统一函数。
//					CH=0x00,写禁止          
//返回值: 	无
//		   
//备注: 
//-----------------------------------------------
void SampleSpecCmdOp(WORD Addr,eRnAddrCmd Cmd )
{
	BYTE bLock[2]={0xE5,0xDC};
	DWORD bLock2[2]= {0x0000E5AA,0x0000DCAA};

	if( ((Addr >= 0x0100) && (Addr <= 0x015a))
	 || ((Addr >= 0x015f) && (Addr <= 0x0165))
	 || ((Addr >= 0x0170) && (Addr <= 0x0175))
	 || (Addr == 0x0177)
	 || ((Addr >= 0x0181) && (Addr <= 0x0189))
	 || ((Addr >= 0x0190) && (Addr <= 0x0192))
	 || (Addr == 0x0194)
	 || ((Addr >= 0x019e) && (Addr <= 0x01a1))
	 || ((Addr >= 0x01b0) && (Addr <= 0x01bb))
	 || ((Addr >= 0x01f0) && (Addr <= 0x01f1))
	 || ((Addr >= 0x01f3) && (Addr <= 0x01fa)) )
	{
		if( Cmd == CmdWriteClose )
		{
			WriteSampleReg(0x0180,(BYTE*)&bLock[1],1);
		}
		else if( Cmd == CmdWriteOpen )
		{
			WriteSampleReg(0x0180,(BYTE*)&bLock[0],1);
		}
		else
		{
			;
		}
	}
	else if( ((Addr >= 0x015b) && (Addr <= 0x015e))
		  || ((Addr >= 0x016b) && (Addr <= 0x016f))
		  || (Addr == 0x0178) || (Addr == 0x0193)
		  || ((Addr >= 0x0195) && (Addr <= 0x019d))
		  || ((Addr >= 0x01a2) && (Addr <= 0x01ad))
		  || ((Addr >= 0x01c8) && (Addr <= 0x01e3))
		  || ((Addr >= 0x01e5) && (Addr <= 0x01ed))
		  || (Addr == 0x01fc) || (Addr == 0x01fe)
		  || ((Addr >= 0x00d0) && (Addr <= 0x00d2))
		  || ((Addr >= 0x00d4) && (Addr <= 0x00f6))
		  || (Addr == 0x00f8) )
		{
			if( Cmd == CmdWriteClose )
			{
				WriteSampleReg(0x017f,(BYTE*)&bLock2[1],4);
			}
			else if( Cmd == CmdWriteOpen )
			{
				WriteSampleReg(0x017f,(BYTE*)&bLock2[0],4);
			}
			else
			{
				;
			}
		}
		
}
#endif //#if( SAMPLE_CHIP == CHIP_RN8302B )
