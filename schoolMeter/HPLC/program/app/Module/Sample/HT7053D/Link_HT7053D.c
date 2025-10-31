//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.7.30
//描述		采样芯片HT7017底层驱动文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7053D.h"

#if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

extern TTimeTem					TF415TimeTem;				//设置时间和温度
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------


//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 读HT7017寄存器
//
//参数:		Addr[in]	地址	 
//			pBuf[out]	读出数据 4字节 高在后，低在前,第4字节固定清为0
//返回值: 	
//		    TRUE/FALSE
//备注: 
//-----------------------------------------------
BOOL Link_ReadSampleReg(BYTE Addr, BYTE *pBuf)
{
	BYTE i,j;
	BYTE tempaddr;
	WORD Sum;
	TTwoByteUnion SampleComBuf;

	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );
	//理论延时3us!!!!!!
	api_Delay10us( 1 );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Addr );

	api_Delay10us( 1 );//速度大于200k时需要延时3us再读数据

	pBuf[3] = 0;
	pBuf[2] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	
	//自行校验
	Sum = pBuf[0]+pBuf[1]+pBuf[2] + Addr;
	DoSPICS(CS_SPI_SAMPLE, FALSE );
    api_Delay10us( 1 );//速度大于200k时需要延时3us再读数据
	//读取校验
    api_Delay10us( 3 );
	DoSPICS( CS_SPI_SAMPLE, TRUE );
	api_Delay10us( 1 );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, COMCHECKSUM );
	api_Delay10us( 3 );//速度大于200k时需要延时3us再读数据
    tempaddr = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	SampleComBuf.b[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	SampleComBuf.b[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );

	DoSPICS(CS_SPI_SAMPLE, FALSE );
	api_Delay10us( 1 );
	// 比较校验
	if (Sum == SampleComBuf.w )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//HT表器件类型不起作用，因此不受影响；
	//ST表，计量单独一路SPI时，复位ESAM与FLASH无区别
	//ST表，计量与FLASH共用一路SPI时应复位FLASH—SPI，复位ESAM-SPI会导致FLASH写失败
}
//-----------------------------------------------
//函数功能: 写HT7017寄存器
//
//参数:		Addr[in]	地址	 
//			pBuf[in]	写入数据 2字节 高在后，低在前
//返回值: 	
//		    TRUE/FALSE
//备注: 
//-----------------------------------------------
BOOL Link_WriteSampleReg(BYTE Addr, BYTE *pBuf)
{
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
	
	DoSPICS( CS_SPI_SAMPLE, TRUE );//片选
	//理论延时3us!!!!!!
	api_Delay10us( 1 );

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80+Addr );//命令

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x00);//数据

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,pBuf[1]);

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,pBuf[0]);

	DoSPICS( CS_SPI_SAMPLE, FALSE);

	api_Delay10us( 2 );
    
    return TRUE;

	//后续理应增加写数据的校验
}

//-----------------------------------------------
//函数功能: 写7053D寄存器高速串口数据输出
//
//参数:		无
//返回值: 	无
//		    
//备注: 
//-----------------------------------------------
void Link_OpenSampleUartReg( WORD Len )
{
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
	
	DoSPICS( CS_SPI_SAMPLE, TRUE );//片选

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0xc6);//命令

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x00);//数据

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80);

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,(BYTE)(Len-1));
    
	DoSPICS( CS_SPI_SAMPLE, FALSE);
	api_Delay10us( 2 );
}
////-----------------------------------------------
////函数功能: 写7053D高速第二串口配置（HUART_CFG）配置
////
////参数:		无
////返回值: 	无
////		    
////备注: 
////-----------------------------------------------
//void Link_OpenSampleUartReg( WORD Len )
//{
//	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
//	
//	DoSPICS( CS_SPI_SAMPLE, TRUE );//片选
//
//	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80+0xc6);//命令
//
//	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x00);//数据
//
//	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80);
//
//	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,(BYTE)(Len-1));
//	DoSPICS( CS_SPI_SAMPLE, FALSE);
//	api_Delay10us( 2 );
//
//	// DoSPICS( CS_SPI_SAMPLE, TRUE );//片选
//	// // api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80+0x4A);//命令
//	// // api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x62);//命令
//	// // api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x08);//命令
//	// // api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x62);//命令
//	// DoSPICS( CS_SPI_SAMPLE, FALSE);
//
//	// api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
//	
//	// api_Delay10us( 2 );
//    
//}
//-----------------------------------------------
//函数功能: 对HT7017的特殊命令操作
//
//参数:		CH[in]	 
//					CH=0xbc,35H~45H,4AH~4FH写允许
//					CH=0xa6,50H~7EH写允许
//					CH=0x00,写禁止          
//返回值: 	无
//		   
//备注: 
//-----------------------------------------------
void SampleSpecCmdOp(BYTE Cmd)
{
	BYTE Buf[2];
	
	Buf[0] = Cmd;
	Buf[1] = 0x00;

	Link_WriteSampleReg(0x32,Buf);
}

//---------------------------------------------------------------
//函数功能: 软复位计量芯片
//
//参数:   无
//
//返回值: 无
//
//备注: SRSTREG 寄存器如果写入 0x55 会导致芯片发生复位，复位后该寄存器清0
//---------------------------------------------------------------
void SoftResetSample( void )
{
	BYTE Buf[2];

	Buf[0] = 0x55;
	Buf[1] = 0x00;
	
	Link_WriteSampleReg( 0x33, Buf );
	api_Delayms( 4 );	 //手册说明 复位后需要等待 2ms 才可以操作寄存器。
}
//---------------------------------------------------------------
//函数功能: 硬复位计量芯片
//
//参数:   无
//
//返回值: 无
//
//备注: 硬复位只支持硬件支持的情况下使用
//---------------------------------------------------------------
void HardResetSample( void )
{
	POWER_SAMPLE_CLOSE;
	api_Delayms( 4 );
	POWER_SAMPLE_OPEN;
}
#endif// #if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )
