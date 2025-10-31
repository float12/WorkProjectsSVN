//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.8.15
//描述		

//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"

#if( PREPAY_MODE != PREPAY_NO )
#if( ENCRYPT_MODE == ENCRYPT_ESAM )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------																						


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE				Err_Sw12[2];

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
extern BYTE			CommandHead[7];

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

#if(ESAM_COM_TYPE == TYPE_SPI)
//-----------------------------------------------
//函数功能: 写ESAM一个字节数据
//
//参数:		Data[in]	写入数据
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void Drv_SendESAMSPI_Byte(BYTE Data)
{
	api_UWriteSpi( eCOMPONENT_SPI_ESAM, Data );
}


//-----------------------------------------------
//函数功能: SPI读出ESAM一个字节数据
//
//参数:		无
//						
//返回值:	返回读出数据
//		   
//备注:
//-----------------------------------------------
BYTE Drv_ReceiveESAMSPI_Byte(void)
{
	return( api_UWriteSpi( eCOMPONENT_SPI_ESAM, 0xff ) );
}

//-----------------------------------------------
//函数功能: 读写ESAM数据
//
//参数:		CmdLen[in]	命令头数据长度
//			Cmd[in]		命令头
//			BufLen[in]	输入数据长度
//			InBuf[in]	输入数据
//			OutBuf[out]	输出数据 输出两字节长度及数据，长度高在前，低在后
//返回值:	TRUE/FALSE
//
//备注:OutBuf输出前面2字节为长度，后面为数据
//-----------------------------------------------
BOOL Link_ESAM_TxRx1(BYTE CmdLen, BYTE *Cmd, WORD BufLen, BYTE *InBuf, BYTE *OutBuf)
{
	WORD i;
	TTwoByteUnion len;
	BYTE LCRC;
	
	#if (SEL_ESAM_TYPE == 0)
	return FALSE;
	#else
	LCRC = 0x00;
	CLEAR_WATCH_DOG;
	
	DoSPICS(CS_SPI_ESAM, YES);
	api_Delay10us(5);
	
	//先记录下命令头，供操作失败时记录异常插卡记录用
	#if( PREPAY_MODE == PREPAY_LOCAL )
	CommandHead[0] = ESAM;
	CommandHead[1] = Cmd[0];
	CommandHead[2] = Cmd[1];
	CommandHead[3] = Cmd[2];
	CommandHead[4] = Cmd[3];
	CommandHead[5] = Cmd[4];
	CommandHead[6] = Cmd[5];
	#endif
	
	//计算发送CRC
	for( i=0; i<CmdLen; i++)
	{
		LCRC ^= Cmd[i] ;
	}
	for( i=0; i<BufLen; i++)
	{
		LCRC ^= InBuf[i] ;
	}
	LCRC ^= 0xff;//取反crc

	//发送数据
	Drv_SendESAMSPI_Byte(0x55);  
	//发送命令头
	for(i=0; i<CmdLen; i++)
	{
		Drv_SendESAMSPI_Byte(Cmd[i]);
	}
	//发送数据域	
	for(i=0; i<BufLen; i++)
	{
		Drv_SendESAMSPI_Byte(InBuf[i]);
	}
	//发送crc
	Drv_SendESAMSPI_Byte( LCRC );
	api_Delay10us(1);
	DoSPICS(CS_SPI_ESAM, NO);
	api_Delay10us(2);
	CLEAR_WATCH_DOG;
	DoSPICS(CS_SPI_ESAM, YES);	
	api_Delay10us(6);
	//开始接收
	Err_Sw12[0] = 0;
	Err_Sw12[1] = 0;
	LCRC = 0x00;
	//等待时间，超时退出
	for( i=0; i<10000; i++ )
	{
		if( Drv_ReceiveESAMSPI_Byte() == 0x55 )
		{
			break;
		}
		api_Delay10us(4);
	}
	if( i == 10000 )
	{
		DoSPICS(CS_SPI_ESAM, NO);
		return FALSE;
	}
	
	//接收状态位，必须为9000
	Err_Sw12[0] = Drv_ReceiveESAMSPI_Byte();
	LCRC ^= Err_Sw12[0];
	Err_Sw12[1] = Drv_ReceiveESAMSPI_Byte();
	LCRC ^= Err_Sw12[1];
	if( (Err_Sw12[0]!=0x90) || (Err_Sw12[1]!=0x00) )
	{
		//mac效验错误
		if( (Err_Sw12[0]==0x69) || (Err_Sw12[1]==0x89) )
		{
			if(MacErrCounter < 202)
			{
				MacErrCounter++;
				if(MacErrCounter == 200)	
				{
					api_SetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG);
				}
			}
			
		}
		DoSPICS(CS_SPI_ESAM, NO);
		return FALSE;
	}
	
	//接收数据长度
	len.b[1] = Drv_ReceiveESAMSPI_Byte();	
	len.b[0] = Drv_ReceiveESAMSPI_Byte();	
	
	OutBuf[0] = len.b[1];
	LCRC ^= OutBuf[0];
	OutBuf[1] = len.b[0];
	LCRC ^= OutBuf[1];	

	if( len.w > MAX_APDU_SIZE )//对长度进行极限值判断 不能超过apdu层最大长度
	{
		return FALSE;
	}
	
	//接收数据域
	for( i=0; i<len.w; i++) 
	{
		OutBuf[2+i] = Drv_ReceiveESAMSPI_Byte();	
		LCRC ^= OutBuf[2+i];
	}
	LCRC ^= 0xff;
	//判断接收CRC是否正确
	if( LCRC != (Drv_ReceiveESAMSPI_Byte()) ) 
	{
		DoSPICS(CS_SPI_ESAM, NO);
		return FALSE;
	}

	DoSPICS(CS_SPI_ESAM, NO);
	//防止ESAM连续通信无法识别的问题
	api_Delay10us(2);
	//返回接收数据长度
	return TRUE ;
	#endif
	
}

//-----------------------------------------------
//函数功能: 读写ESAM数据
//
//参数:		CmdLen[in]	命令头数据长度
//			Cmd[in]		命令头
//			BufLen[in]	输入数据长度
//			InBuf[in]	输入数据
//			OutBuf[out]	输出数据 输出两字节长度及数据，长度高在前，低在后
//返回值:	TRUE/FALSE
//
//备注:OutBuf输出前面2字节为长度，后面为数据
//-----------------------------------------------
BOOL Link_ESAM_TxRx(BYTE CmdLen, BYTE *Cmd, WORD BufLen, BYTE *InBuf, BYTE *OutBuf)
{
	BYTE Result = 0;
	
    if( ESAM_POWER_IS_CLOSE )//esam电源未打开
    {
//        api_ResetEsamSpi();
//        if((APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime == 0)//当前剩余时间全为0
//        &&(APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime == 0)
//        &&(APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime == 0) )
//        {
//        	if( ESAMPowerControlConst == YES )//送样3秒关闭esam
//        	{
//				APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 5;
//				APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 5;
//        	}
//        	else
//        	{
//				APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 120;
//				APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 120;
//        	}
//        }
    }
	
	#if(BOARD_TYPE == BOARD_HT_SINGLE_78201602)
	//如果单相表ESAM电源时间控制开启, ESAM SPI用模式3
	if( (ESAMPowerControlConst == YES) && (MAX_PHASE == 1) )
	{
		api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
	}
	
	Result = Link_ESAM_TxRx1( CmdLen, Cmd, BufLen, InBuf, OutBuf );
	
	//如果单相表ESAM电源时间控制开启, Flash SPI用模式0
	if( (ESAMPowerControlConst == YES) && (MAX_PHASE == 1) )
	{
		api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_0 );
	}
	#else
	Result = Link_ESAM_TxRx1( CmdLen, Cmd, BufLen, InBuf, OutBuf );
	#endif
	
    return Result;
}
//--------------------------------------------------
//功能描述:  复位ESAMspi
//         
//参数  :   无
//
//返回值:    无  
//         
//备注内容:  无
//--------------------------------------------------
void api_ResetEsamSpi( void )
{
	BYTE Buf[20],CmdBuf[6],Result;

	#if (SEL_ESAM_TYPE == 0)
	return;
	#else
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x36;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x01;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x00;
	
	ResetSPIDevice( eCOMPONENT_SPI_ESAM, 20 ); //延时20ms
	
	#if(BOARD_TYPE == BOARD_HT_SINGLE_78201602)
	//如果单相表ESAM电源时间控制开启, ESAM SPI用模式3
	if( (ESAMPowerControlConst == YES) && (MAX_PHASE == 1) )
	{
		api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
	}
	
	Result = Link_ESAM_TxRx1(6,CmdBuf,0,NULL,Buf);
	
	//如果单相表ESAM电源时间控制开启, Flash SPI用模式0
	if( (ESAMPowerControlConst == YES) && (MAX_PHASE == 1) )
	{
		api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_0 );
	}
	#else
	Result = Link_ESAM_TxRx1(6,CmdBuf,0,NULL,Buf);
	#endif
	
	if( Result == FALSE )//如果失败进行110ms延时复位
	{
		api_WriteSysUNMsg( ESAM_FIRST_RESET_ERR );//记录ESAM复位失败异常时间
		ResetSPIDevice( eCOMPONENT_SPI_ESAM, 150 );
	}
	#endif //#if (SEL_ESAM_TYPE != 0)
}

#else
7816通信方式
#endif//#if(ESAM_COM_TYPE == TYPE_SPI)





#if( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//   名称: Send_Command
//   功能: 向卡片/ESAM发送命令字符串
//   输入: EC=0表示对卡片操作;EC=1表示对ESAM操作
//	 CLA 类别字节
//	 INS 指令字节
//	 P1,P2 指令附加特定参数
//	 P3 由INS的编码而定，或是表示命令中送给IC卡的数据，或是等待从IC卡响应的最大数据长度
//   输出: N/A
//-----------------------------------------------
BYTE Send_Command(BYTE Cla,BYTE Ins,BYTE P1,BYTE P2,BYTE P3)
{
	BYTE Rins=0;
	
	api_OpenTxCard(Cla);
	api_SendCardByte(Ins);
	api_SendCardByte(P1);
	api_SendCardByte(P2);
	api_SendCardByte(P3);
	#if( MAX_PHASE == 3 )
	//判断最后一个字节是否发送完成 必须判断最后一个字节是否发送成功 不然转为接收会出现自发自收的情况
	if( api_UartIsSendEnd( 4 ) == FALSE )
	{
		return FALSE;
	}
	#endif
	api_OpenRxCard();
    api_ReceiveCardByte(&Rins);
	if(Rins!=Ins)
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读写CPU卡数据
//
//参数:		Cla	Ins P1 P2 P3 命令头
//			RW[in]		读卡READ,写卡WRITE
//			peerom[in]	写入或者读出的数据指针
//			
//					
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL Link_Card_TxRx(BYTE Cla,BYTE Ins,BYTE P1,BYTE P2,BYTE P3,BYTE RW,BYTE *peerom)
{
	BYTE i=0,SW1,SW2,Result,RecResult;
	BOOL IntStatus;
	
	//先记录下命令头，供操作失败时记录异常插卡记录用
	CommandHead[0] = CARD;
	CommandHead[1] = Cla;
	CommandHead[2] = Ins;
	CommandHead[3] = P1;
	CommandHead[4] = P2;
	CommandHead[5] = P3;
	CommandHead[6] = 0x00;//为保持和记录的ESAM命令字长度一致，此处置为0

	Result = FALSE;
	RecResult = FALSE;
	Err_Sw12[0] = 0;
	Err_Sw12[1] = 0;
	SW1 = 0xFF;
	SW2 = 0xFF;
	IntStatus = api_splx(0);
	
	if(Send_Command(Cla,Ins,P1,P2,P3)!=TRUE)
	{
		api_CloseCard();
		api_splx(IntStatus);
		return FALSE;
	}

	if( RW == READ )
	{
		api_OpenRxCard();
		for( i=0; i<P3; i++)
	  	{
			RecResult = api_ReceiveCardByte(peerom+i);
			if(RecResult == FALSE)
			{
				api_splx(IntStatus);
				return FALSE;
			}
	  	} 
	}
	else
	{	
		#if(MAX_PHASE == 3)
		api_Delayms( 1 ); //此处需要延时1ms 不然会和接收的最后一字节数据连帧 导致数据发送失败
		#endif
		api_OpenTxCard(peerom[0]);
		for( i=0; i<(P3-1); i++)
		{
			api_SendCardByte(*(peerom+1));
			peerom++;
		}
		#if(MAX_PHASE == 3)
		//判断最后一个字节是否发送完成 必须判断最后一个字节是否发送成功 不然转为接收会出现自发自收的情况
		if( api_UartIsSendEnd( 4 ) == FALSE )
		{
			return FALSE;
		}
		#endif
	}
	i=0;
	do
	{
		api_OpenRxCard();
		RecResult = api_ReceiveCardByte(&SW1);
		if(i++>10)
		{
            break;
		}
	}while( RecResult==FALSE );
	
    api_ReceiveCardByte(&SW2);

	if((SW1==0x61)||(SW1==0x90))
	{
		Result = TRUE;
	}
	Err_Sw12[0] = SW1;
	Err_Sw12[1] = SW2;//正确返回9000 或 6108
	api_splx(IntStatus);
	return Result;
}

//-----------------------------------------------
//函数功能: 复位卡片,读取复位信息
//
//参数:		Buf返回复位信息
//						
//返回值:	TRUE or FALSE
//		   
//备注:
//-----------------------------------------------
BOOL Link_CardReset( BYTE *Buf )
{
	BYTE i;
	BOOL IntStatus;
	BOOL Result;
	//BYTE Buf[20];
	
	//memset( Reset_Data, 0, sizeof(Reset_Data) );
	//IntStatus = api_splx(0);
	
	api_ResetCard();

	api_OpenRxCard();
	for(i=0; i<13; i++)
	{
		Result = api_ReceiveCardByte(Buf+i);
		if( Result == FALSE )
		{
			return FALSE;
		}
	}
	//api_splx(IntStatus);
	//
	if( ( Buf[0] == 0x3b ) && ( Buf[1] == 0x69 ) )
	{
		return TRUE;
	}
	api_CloseCard();
	return FALSE;
}

#endif//#if( PREPAY_MODE == PREPAY_LOCAL )


#endif//#if( ENCRYPT_MODE == ENCRYPT_ESAM )

#endif//#if( PREPAY_MODE != PREPAY_NO )

