//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.10.8
//描述		采样芯片HT7038通信链路层文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7032L.h"

#if(SAMPLE_CHIP==CHIP_HT7032L)

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

//-----------------------------------------------
//函数功能: 读HT7038寄存器
//
//参数:		Addr[in]	地址	 
//			pBuf[out]	读出数据 4字节 高在后，低在前,第4字节固定清为0
//返回值: 	
//		    TRUE/FALSE
//备注: 
//-----------------------------------------------
BOOL Link_ReadSampleReg(WORD Addr, BYTE *pBuf)
{
	BYTE Buf[7]={0};
	Buf[0] = ((Addr | 0x6000) >> 8);
	Buf[1] = Addr & 0xFF;
	
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[0] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[1] );

	api_Delay10us( 1 );//速度大于200k时需要延时3us再读数据

	pBuf[3] = Buf[2] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[2] = Buf[3] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[1] = Buf[4] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[0] = Buf[5] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	Buf[6] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	DoSPICS(CS_SPI_SAMPLE, FALSE );
	//HT表器件类型不起作用，因此不受影响；
	//ST表，计量单独一路SPI时，复位ESAM与FLASH无区别
	//ST表，计量与FLASH共用一路SPI时应复位FLASH—SPI，复位ESAM-SPI会导致FLASH写失败
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 1 );
    
	if(Buf[6] ==(BYTE)( ~(lib_CheckSum(&Buf[0],6))))
	{
		return TRUE;
	}
	else
	{
		memset( pBuf, 0x00, 4);
		return FALSE;
	}
	
}

//-----------------------------------------------
//函数功能: 写HT7038寄存器
//
//参数:		Addr[in]	地址	 
//			pBuf[in]	写入数据 3字节 高在后，低在前
//返回值: 	
//		    
//备注: 
//-----------------------------------------------
static void Link_WriteSampleReg(WORD Addr, BYTE *pBuf)
{
	BYTE Buf[7];
	
	Buf[0] = ((Addr | 0x8000) >> 8);
	Buf[1] = Addr & 0xFF;
	Buf[2] = pBuf[3];
	Buf[3] = pBuf[2];
	Buf[4] = pBuf[1];
	Buf[5] = pBuf[0];
	Buf[6] = ~(lib_CheckSum(&Buf[0],6));
	
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
	
	DoSPICS( CS_SPI_SAMPLE, TRUE );//片选

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[0] );
	
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[1] );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[2] );//数据
	
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[3] );//数据

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[4] );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[5] );
	
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[6] );

	DoSPICS( CS_SPI_SAMPLE, FALSE);
	
	//HT表器件类型不起作用，因此不受影响；
	//ST表，计量单独一路SPI时，复位ESAM与FLASH无区别
	//ST表，计量与FLASH共用一路SPI时应复位FLASH—SPI，复位ESAM-SPI会导致FLASH写失败
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 2 );
}
//-----------------------------------------------
//函数功能: 连续读HT7038寄存器
//
//参数:		Addr[in]	地址	 
//			Num[in]		寄存器个数	
//			pBuf[out]	读出数据 4字节 高在后，低在前,第4字节固定清为0
//返回值: 	
//		    TRUE/FALSE
//备注: 
//-----------------------------------------------
BOOL Link_ReadSampleRegCont(WORD Addr, WORD Num, DWORD *pBuf)
{
	BYTE Buf[4]={0};
	WORD i;
	DWORD Cmd;
	BYTE Cks=0;
	
	if((Addr>0xFFF) || ( ( Addr+Num-1) > 0xFFF))//地址超限
	{
		return FALSE;
	}
	if(Num >0x1FF)//长度超限
	{
		return FALSE;
	}
	Cmd = ((1<<21) | ((Num-1)<<12) | (Addr&0xFFF));	//32bit
	
	Buf[0] = (( Cmd>>16) & 0xFF);
	Buf[1] = (( Cmd>>8) & 0xFF);
	Buf[2] = (Cmd & 0xFF);
	Cks= Buf[0]+Buf[1]+Buf[2];
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[0] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[1] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[2] );

	api_Delay10us( 1 );//速度大于200k时需要延时3us再读数据
	
	for(i=0;i<Num;i++)
	{
		Buf[3] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		Buf[2] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		Buf[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		Buf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		memcpy(&pBuf[i],Buf,4);
	}
	Buf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	
	DoSPICS(CS_SPI_SAMPLE, FALSE );
	//HT表器件类型不起作用，因此不受影响；
	//ST表，计量单独一路SPI时，复位ESAM与FLASH无区别
	//ST表，计量与FLASH共用一路SPI时应复位FLASH—SPI，复位ESAM-SPI会导致FLASH写失败
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 1 );
    Cks += lib_CheckSum((BYTE*)pBuf,(Num*4));
	if(Cks == (BYTE)(~Buf[0]))
	{
		return TRUE;
	}
	else
	{
		memset( pBuf, 0x00, (Num*4));
		return FALSE;
	}
	
}

//-----------------------------------------------
//函数功能: 连续读HT7038寄存器
//
//参数:		Addr[in]	地址	 
//			Num[in]		寄存器个数	
//			pBuf[out]	读出数据 4字节 高在后，低在前,第4字节固定清为0
//返回值: 	
//		    TRUE/FALSE
//备注: 
//-----------------------------------------------
BOOL ReadSampleBuffCont(WORD Addr, WORD Num, SWORD *pBuf)
{
	BYTE Buf[4]={0};
	WORD i;
	DWORD Cmd;
	BYTE Cks=0;
	
	if((Addr>0xFFF) || ( ( (Addr+Num/2)-1) > 0xFFF))//地址超限
	{
		return FALSE;
	}
	if((Num/2) >0x1FF)//长度超限
	{
		return FALSE;
	}
	Cmd = ((1<<21) | (((Num/2)-1)<<12) | (Addr&0xFFF));	//32bit
	
	Buf[0] = (( Cmd>>16) & 0xFF);
	Buf[1] = (( Cmd>>8) & 0xFF);
	Buf[2] = (Cmd & 0xFF);
	Cks= Buf[0]+Buf[1]+Buf[2];
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[0] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[1] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[2] );

	api_Delay10us( 1 );//速度大于200k时需要延时3us再读数据
	
	for(i=0;i<Num;i++)
	{
		Buf[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		Buf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		memcpy(&pBuf[i],Buf,2);
	}
	Buf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	
	DoSPICS(CS_SPI_SAMPLE, FALSE );
	//HT表器件类型不起作用，因此不受影响；
	//ST表，计量单独一路SPI时，复位ESAM与FLASH无区别
	//ST表，计量与FLASH共用一路SPI时应复位FLASH—SPI，复位ESAM-SPI会导致FLASH写失败
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 1 );
    Cks += lib_CheckSum((BYTE*)pBuf,(Num*2));
	if(Cks == (BYTE)(~Buf[0]))
	{
		return TRUE;
	}
	else
	{
		memset( pBuf, 0x00, (Num*2));
		return FALSE;
	}
	
}

////-----------------------------------------------
////函数功能: 判断当前芯片处于的状态 是可以读测量数据还是校表数据
////
////参数:		无	 
////			
////返回值: 	TRUE--处于读测量数据
////		    FALSE--处于读校表数据
////备注: 
////-----------------------------------------------
//static BOOL JudgeReadStatus( void )
//{
//	DWORD tdw;
//
//	Link_ReadSampleReg(0,(BYTE*)&tdw);
//
//	if( tdw == 0xaaaa )
//	{
//		return FALSE;
//	}
//	else
//	{
//		return TRUE;
//	}
//}
//
//
////-----------------------------------------------
////函数功能: 切换到读测量数据寄存器
////
////参数:		无	 
////			
////返回值: 	无
////		    
////备注: 
////-----------------------------------------------
//static void SwitchToReadMeasureData( void )
//{
//	BYTE i;
//	DWORD Data;
//	
//	for(i=0;i<3;i++)
//	{
//		if( TRUE == JudgeReadStatus() )
//		{
//			break;
//		}
//
//		//在Link_WriteSampleReg中写地址时加80，所以此处减80
//		Data = 0xa5a5a5;
//		Link_WriteSampleReg( 0xc6-0x80, (BYTE*)&Data );
//	}
//}
//
//
////-----------------------------------------------
////函数功能: 切换到读校表数据寄存器
////
////参数:		无	 
////			
////返回值: 	无
////		    
////备注: 
////-----------------------------------------------
//static void SwitchToReadCalibrateData( void )
//{
//	BYTE i;
//	DWORD Data;
//	
//	for(i=0;i<3;i++)
//	{
//		if( FALSE == JudgeReadStatus() )
//		{
//			break;
//		}
//
//		//在Link_WriteSampleReg中写地址时加80，所以此处减80
//		Data = 0x00005a;
//		Link_WriteSampleReg( 0xc6-0x80, (BYTE*)&Data );
//	}
//}

//-----------------------------------------------
//函数功能: 读测量数据寄存器
//
//参数:		Addr[in] 寄存器地址	 
//			
//返回值: 	返回读出的数据，最高字节固定为0
//		    
//备注: 
//-----------------------------------------------
DWORD ReadSampleData( WORD Addr )
{
	DWORD tdw,tdw1;
	Link_ReadSampleReg(Addr,(BYTE*)&tdw);
	//回读
	Link_ReadSampleReg(r_BckReg,(BYTE*)&tdw1);

	return tdw;
}


////-----------------------------------------------
////函数功能: 读校表数据寄存器
////
////参数:		Addr[in] 寄存器地址	 
////			
////返回值: 	返回读出的数据，最高字节固定为0
////		    
////备注: 
////-----------------------------------------------
//DWORD ReadCalibrateData( WORD Addr )
//{
//	DWORD tdw,tdw1;
//
//	Link_ReadSampleReg(Addr,(BYTE*)&tdw);
//	//回读
//	Link_ReadSampleReg(r_BckReg,(BYTE*)&tdw1);
//
//	return tdw;
//}


//-----------------------------------------------
//函数功能: 写校表数据寄存器
//
//参数:		Addr[in] 寄存器地址	 
//			Data[in] 要写入的数据,写入低三个字节，最高字节没用
//返回值: 	TRUE/FALSE
//		    
//备注: 
//-----------------------------------------------
BOOL WriteSampleReg( WORD Addr, DWORD Data )
{
	DWORD tdw;
	BYTE RepeatTime;
	WORD Status;

	for(RepeatTime=0; RepeatTime<3; RepeatTime++)
	{
		Link_WriteSampleReg( Addr, (BYTE*)&Data );

		//回读
		Link_ReadSampleReg(r_BckReg,(BYTE*)&tdw);

		if( tdw != Data )
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
//函数功能: 对HT7038的特殊命令操作
//
//参数:		Cmd[in]	 
//					CH=0xa6,写允许。与单相表统一函数。
//					单相校表寄存器分两个写保护开关，三相一个开关可以开所有寄存器
//					CH=0x00,写禁止          
//返回值: 	无
//		   
//备注: 
//-----------------------------------------------
void SampleSpecCmdOp(BYTE Cmd)
{
	if( Cmd == 0x00 )
	{
		WriteSampleReg(w_EMUWPREG, ~0xA99A);
	}
	else if( Cmd == 0xa6 )
	{
        WriteSampleReg(w_EMUWPREG, 0xA99A);
	}
	else
	{
		;
	}
}

#endif//#if(SAMPLE_CHIP==CHIP_HT7032L)


