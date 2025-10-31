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
#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )


//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------	


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------																						


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE	PrePayBuf[MAX_PREPAY_LEN];
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//功能描述:  添加命令头函数        
//参数:      BYTE* pbuf[in]       
//           BYTE inc[in]       
//           BYTE com[in]         
//           BYTE P1[in]        
//           BYTE P2[in]        
//           BYTE len1[in]         
//           BYTE len2[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void api_ProcPrePayCommhead(BYTE* pbuf,BYTE inc, BYTE com, BYTE P1, BYTE P2, BYTE len1,BYTE len2)
{
	pbuf[0] = inc;
	pbuf[1] = com;
	pbuf[2] = P1;
	pbuf[3] = P2;
	pbuf[4] = len1;
	pbuf[5] = len2;
}

//-----------------------------------------------
//函数功能: 获取电表安全芯片信息
//
//参数:		Type[in]	ff--全部信息
//			OutBuf[out]	放置要读取的长度及数据，长度两字节，高在前，低在后
//返回值:	数据长度 8000代表错误
//
//备注:OutBuf需比实际计算的长度多两个字节，但在函数内已经处理，返回的数据前面不带长度
//-----------------------------------------------
WORD api_GetEsamInfo(BYTE Type,BYTE *OutBuf)
{
	BOOL Result;
	BYTE CmdBuf[6];
	TTwoByteUnion tw;
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x36;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = Type;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x00;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,0,NULL,OutBuf);
	
	if( Result == FALSE )
	{
		return 0x8000;
	}

	//长度
	tw.b[0] = OutBuf[1];
	tw.b[1] = OutBuf[0];
	if( tw.w > 200 )
	{
		return 0x8000; 
	}
	
	memmove( OutBuf, OutBuf + 2, tw.w );

	return tw.w;
}


//-----------------------------------------------
//函数功能: 建立应用连接
//
//参数:			Mode[in] 0x55: 终端 其他：主站
//				Buf[in/out]	输入时32字节密文1+4字节客户机签名1
//						输出时长度+48字节随机数+4字节客户机签名，长度两字节，高在前，低在后
//返回值:	TRUE/FALSE
//
//备注:在698规约中建立应用连接时使用,Buf前面2字节代表数据长度
//-----------------------------------------------
BOOL api_ConnectMechanismInfo( eConnectMode ConnectModeMode, BYTE *Buf )
{
	BOOL Result;
	BYTE CmdBuf[6];

	if( ConnectModeMode == eConnectTerminal )//终端建立应用链接
	{
		CmdBuf[0] = 0x82;
	}
	else
	{
		CmdBuf[0] = 0x81;
	}

	CmdBuf[1] = 0x02;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x00;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x24;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,0x24,Buf,Buf);
	
	return Result;
}


//-----------------------------------------------
//函数功能: 验证主站下发的数据并返回解析后的数据
//
//参数:		
//			Buf[in/out]输入时：	若数据验证信息为SID_MAC
//							4字节标识+附加数据AttachData+Data+MAC
//								若数据验证信息为SID
//							4字节标识+附加数据AttachData+Data
//						输出时：长度及数据，长度两字节，高在前，低在后
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_AuthDataSID(BYTE *Buf)
{
	WORD Result;
	TTwoByteUnion BufLen;
	
	BufLen.b[0] = Buf[5];
	BufLen.b[1] = Buf[4];
	if( BufLen.w > MAX_PREPAY_LEN )//输入的数据如果大于apdu数据返回失败
	{
		return FALSE;
	}
	
	Result = Link_ESAM_TxRx(6,Buf,BufLen.w,Buf+6,Buf+6);
	if( Result == TRUE )//操作成功后进行数据的搬移
	{
		BufLen.b[0] = Buf[7];
		BufLen.b[1] = Buf[6];
		memmove(Buf,&Buf[6],BufLen.w+2);
	}

	return Result;
}

//-----------------------------------------------
//函数功能: 将电表组织的数据写入ESAM后产生mac,整理后上传
//
//参数:
//			Len[in] 传入的数据长度
//			Buf[in/out]	输入数据
//					数据处理完后输出时 （长度两字节，高在前，低在后）
//						输出长度+mac
//
//返回值:	TRUE/FALSE
//
//备注:Buf前面2字节代表数据长度
//-----------------------------------------------
BOOL api_PackDataWithSafe(WORD Len,BYTE *Buf)
{
	BOOL Result;
	BYTE CmdBuf[6];
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x0e;
	CmdBuf[2] = 0x40;
	CmdBuf[3] = 0x02;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,Buf);
	
	return Result;
}


//-----------------------------------------------
//函数功能: 将密文写入ESAM,整理后上传
//
//参数:		EncryptMode[in] 加密方式 0x00： 主站      0x55：终端
//			Type[in] 1--明文+MAC	2--密文	3--密文+mac
//			Len[in] 传入的数据长度
//			Buf[in/out]	输入数据
//					数据处理完后输出时 （长度两字节，高在前，低在后）
//						Type为1时，输出长度+mac
//						Type为2时，输出长度+密文
//						Type为3时，输出长度+密文+mac
//
//返回值:	TRUE/FALSE
//
//备注:Buf前面2字节代表返回长度
//-----------------------------------------------
BOOL api_PackEsamDataWithSafe(BYTE EncryptMode, BYTE Type,WORD Len,BYTE *Buf)
{
	BOOL Result;
	BYTE CmdBuf[6], CmdBuf1[6],Len1;
	WORD Datalen;

	if( Len > MAX_APDU_SIZE )
	{
		return FALSE;
	}
	
	if( EncryptMode == 0x55 )//终端加密
	{
		CmdBuf[0] = 0x82;
		CmdBuf[1] = 0x24;	
		CmdBuf[2] = 0x00;
		if( (Type == 1) || ( Type == 3 ))
		{
			CmdBuf1[0] = 0x80;
			CmdBuf1[1] = 0x24;
			CmdBuf1[2] = 0x10+Type;
			CmdBuf1[3] = 0x00;
			
			if( Type == 3 )//密文+MAC
			{
				Len1 = (Len + 3);
				if( (Len1 %16) == 0 )
				{ 
					Datalen = (Len1+4);
				}
				else
				{
					Datalen = (((Len1/16)+1)*16+4);
				}
			}
			else
			{
				Datalen= Len+4;
			}

			CmdBuf1[4] = Datalen>>8;
			CmdBuf1[5] = Datalen;
			memcpy( Buf+Len, CmdBuf1, 6 );
			
			Len += 6;
			CmdBuf[3] = 0x10+Type;
			CmdBuf[4] = Len>>8;
			CmdBuf[5] = Len;
		}
		else if( Type == 2 )//密文方式
		{
			CmdBuf[3] = 0x12;
			CmdBuf[4] = Len>>8;
			CmdBuf[5] = Len;
		}		
		else
		{
			return 0;
		}
	}
	else//主站加密
	{
		CmdBuf[0] = 0x81;
		CmdBuf[1] = 0x1c;	
		CmdBuf[2] = 0x00;
		if( Type == 1 )
		{
			CmdBuf[3] = 0x11;
		}
		else if( Type == 2 )
		{
			CmdBuf[3] = 0xa6;
		}
		else if( Type == 3 )
		{
			CmdBuf[3] = 0xa7;
		}
		else
		{
			return 0;
		}

		CmdBuf[4] = Len>>8;
		CmdBuf[5] = Len;
		
	}
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,Buf);
	
	return Result;
}

//-----------------------------------------------
//函数功能:     红外查询指令（获取随机数1密文）
//
//参数:	 	RNBuf[in]     //输入随机数 RNBuf[0](随机数长度)+RNBuf[1]。。（随机数）
//	       OutBuf[in/out] //输出数据 （8字节 不带长度）
//		
//						
//返回值:	    TRUE/FALSE
//		   
//备注: 
//-----------------------------------------------
BOOL api_EsamIRRequest( BYTE *RNBuf, BYTE *OutBuf )
{
	BOOL Result;
	BYTE CmdBuf[6],InBuf[MAX_RN_SIZE+10];

	CmdBuf[0] = 0x80; //组帧命令头
	CmdBuf[1] = 0x08;
	CmdBuf[2] = 0x08;
	CmdBuf[3] = 0x03;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x10;

    InBuf[0] = 0x00;
    InBuf[1] = 0x00;

    Result = api_ReadEsamData(0x0001, 0x0007, 0x0006, InBuf+2);//获取表号
	if( Result == FALSE )
	{
		return FALSE;
	}

    if( RNBuf[0] > MAX_RN_SIZE )//随机数长度不能超过MAX_RN_SIZE
    {
        return FALSE;
    }
    
	memcpy(InBuf+8,RNBuf+1,RNBuf[0]);//获取随机数
	Result = Link_ESAM_TxRx(6,CmdBuf,(8+RNBuf[0]),InBuf,OutBuf);//操作esam
	if( Result == FALSE )
	{
		return FALSE;
	}

	if( (OutBuf[0] != 0x00) || ( OutBuf[1] != 0x08) )//根据协议要求 返回数据为8个字节
	{
        return FALSE;
	}
	
    memmove( OutBuf, OutBuf+2, 8);
    
	return Result;
}
//终端的身份认证 终端的身份认证不需要表号得到密文，所以和IR分开了
BOOL api_EsamTerminalRequest( BYTE *RNBuf, BYTE *OutBuf )
{
	BOOL Result;
	BYTE CmdBuf[6];
	
	CmdBuf[0] = 0x80; //组帧命令头
	CmdBuf[1] = 0x08;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x04;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x08;
    
	Result = Link_ESAM_TxRx(6,CmdBuf,8,RNBuf+1,OutBuf);//操作esam
	if( Result == FALSE )
	{
		return FALSE;
	}

	if( (OutBuf[0] != 0x00) || ( OutBuf[1] != 0x08) )//ESAM 手册显示只有8个字节的输出
	{
        return FALSE;
	}
	
    memmove( OutBuf, OutBuf+2, 8);
    
	return Result;	
}
//-----------------------------------------------
//函数功能:     红外认证指令（获取随机数1密文）
//
//参数:	    INBuf[in]     //输入随机数密文
//
//返回值:	    TRUE/FALSE
//
//备注:
//-----------------------------------------------
BOOL api_EsamIRAuth( BYTE *INBuf )
{
    BYTE CmdBuf[6],OutBuf[12],Result;
	
	CmdBuf[0] = 0x80;//组帧命令头
	CmdBuf[1] = 0x06;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x01;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x08;
    
    Result = Link_ESAM_TxRx(6,CmdBuf,8,INBuf,OutBuf);//进行随机数密文认证 返回值无实际意义
    if( Result == FALSE )
    {
        return FALSE;
    }

    return Result;
}
//-----------------------------------------------
//函数功能: 计算主动上报数据的rn_mac
//
//参数:		
//			Len[in] 	输入长度
//			Buf[in]		输入数据
//			OutBuf[out]	输出数据 rn+mac		
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
WORD api_DealEsamSafeReportRN_MAC( WORD Len,BYTE *Buf,BYTE *OutBuf )
{
	BYTE CmdBuf[11];
	BOOL Result;
	WORD ReturnLen;
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x14;
	CmdBuf[2] = 0x01;
	CmdBuf[3] = 0x03;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,OutBuf);
	if( Result == FALSE )
	{
		return 0x8000;
	}

	lib_ExchangeData( (BYTE*)&ReturnLen, OutBuf, 2 );
	memmove( OutBuf, OutBuf+2, ReturnLen );

	return ReturnLen;
}

//-----------------------------------------------
//函数功能: 验证主动上报应答报文
//
//参数:		
//			Len[in] 	输入长度
//			Buf[in]		输入数据
//			OutBuf[out]	输出数据 rn+mac		
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
WORD api_VerifyEsamSafeReportMAC( BYTE DataLen,BYTE *DataBuf,BYTE *RN_MAC )
{
	WORD Len;	
	BYTE CmdBuf[11];
	BYTE Buf[280];//255+16
	BOOL Result;
	WORD ReturnLen;
	BYTE OutBuf[512];
	
	Len = DataLen+16;
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x0E;
	CmdBuf[2] = 0x40;
	CmdBuf[3] = 0x81;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	memset( Buf, 0x00, sizeof(Buf) );
	memcpy( Buf, RN_MAC, 12 );
	memset( Buf+12, 0x00, 4 );
	memcpy( Buf+16, DataBuf, DataLen );
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,OutBuf);

	return Result;
	
}

//-----------------------------------------------
//函数功能: 读取ESAM文件信息
//
//参数:		FID[in] 	文件标识
//			Offset[in] 	偏移地址
//			Len[in] 	读取长度
//			Buf[out]	输出长度+数据（长度两字节，高在前，低在后）
//						
//返回值:	TRUE/FALSE
//		   
//备注:需要确认主站下发的时候是否有cmd
//-----------------------------------------------
BOOL api_ReadEsamData(WORD FID,WORD Offset,WORD Len,BYTE *Buf)
{
	BOOL Result;
	BYTE CmdBuf[11];
	WORD ReturnLen;
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x2c;
	CmdBuf[2] = FID>>8;
	CmdBuf[3] = FID;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x05;
	CmdBuf[6] = 0x00;
	CmdBuf[7] = Offset>>8;
	CmdBuf[8] = Offset;
	CmdBuf[9] = Len>>8;
	CmdBuf[10] = Len;
	
	Result = Link_ESAM_TxRx(11,CmdBuf,0,NULL,Buf);
	if( Result == FALSE )
	{
		return FALSE;
	}

	lib_ExchangeData( (BYTE*)&ReturnLen, Buf, 2 );
	memmove( Buf, Buf+2, ReturnLen );

	return TRUE;
}

//-----------------------------------------------
//函数功能: 取ESAM随机数
//
//参数:		Len[in]		随机数长度 4/8/10
//			Buf[out]	输出长度+数据（长度两字节，高在前，低在后）					
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_ReadEsamRandom(BYTE Len,BYTE *Buf)
{
	BOOL Result;
	BYTE CmdBuf[6];
	
	if( Len > 8 )
	{
		return FALSE;
	}

	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x04;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = Len;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x00;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,0,NULL,Buf);
	
	memcpy( SafeAuPara.EsamRand, Buf + 2, Len );
	
	return Result;
}

//-----------------------------------------------
//函数功能: 读取钱包
//
//参数:		Buf[out]	输出长度+数据（长度两字节，高在前，低在后）
//						
//返回值:	TRUE/FALSE
//		   
//备注:购电金额（4）+购电次数（4）+客户编号（6）
//-----------------------------------------------
BOOL ReadEsamMoneyBag( BYTE *Buf )
{
	BOOL Result;
	BYTE CmdBuf[7];
	BYTE OutBuf[20];
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x48;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x00;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x01;
	CmdBuf[6] = 0x00;
	
	Result = Link_ESAM_TxRx( 7, CmdBuf, 0, NULL, OutBuf );
	
	memcpy( Buf, OutBuf+2, 14 );
	
	return Result;
}

//-----------------------------------------------
//函数功能: 更新esam运行信息文件(此函数没有使用)
//
//参数:		Offset[in]	偏移
//			Len[in]		长度
//			Buf[in/out]	输入数据,输出为长度+数据					
//返回值:	TRUE/FALSE
//		   
//备注:Buf为输入数据，但是操作完成后buf数据会变,此函数数据流要改@@@@@@
//-----------------------------------------------
BOOL api_UpdateEsamRunFile(BYTE Offset,WORD Len,BYTE *Buf)
{	
	BOOL Result;
	BYTE CmdBuf[6];
	
	CmdBuf[0] = 0x83;
	CmdBuf[1] = 0x2a;
	CmdBuf[2] = 0x88;
	CmdBuf[3] = Offset;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,Buf);
	
	return Result;
}

//--------------------------------------------------
//功能描述:  软件比对命令
//
//参数:      BYTE P2[in]//密钥标识
//
//           BYTE Len[in]//数据长度
//
//           BYTE *Buf[in]//数据buf指针
//
//返回值:
//
//备注内容:  Buf前面2字节代表长度
//--------------------------------------------------
BOOL api_SoftwareComparisonEsam(BYTE P2,WORD Len,BYTE *Buf)
{	
	BOOL Result;
	BYTE CmdBuf[6];
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x0A;
	CmdBuf[2] = 0x48;
	CmdBuf[3] = P2;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,Buf);
	
	return Result;
}

//-----------------------------------------------
//函数功能: 获取密钥状态
//
//参数:		无					
//返回值:	无
//		   
//备注:上电及密钥更新时调用。根据esam中的状态刷新ee中的密钥状态。
//		由于操作ee，禁止频繁调用。
//-----------------------------------------------
void api_FlashKeyStatus(void)
{
	BYTE Buf[20],BufBak[20];
	BYTE i;
	WORD Len;

	for(i = 0;i < 3;i++)//重试3次
	{
		Len = api_GetEsamInfo( 0x04, Buf );
		if( Len <= (sizeof(Buf)-2) )
		{
			break;
		}
		if(i == 1)	//失败两次
		{
			api_ResetEsamSpi();//重置ESAM管脚和SPI通信
		}
	}
	
	if(i == 3)
	{
		if( api_GetRunHardFlag(eRUN_HARD_ESAM_ERR) == FALSE )
		{
			api_SetFollowReportStatus(eSTATUS_ESAM_Error);
		}	
		api_SetRunHardFlag(eRUN_HARD_ESAM_ERR);
		api_ClrRunHardFlag(eRUN_HARD_COMMON_KEY);
		return;
	}
	else
	{
		api_ClrRunHardFlag(eRUN_HARD_ESAM_ERR);
	}

	memset( BufBak, 0x00, sizeof(BufBak) );

	if( memcmp( Buf, BufBak, 16) == 0 )//16字节0为公钥，其他为私钥
	{
		api_SetRunHardFlag(eRUN_HARD_COMMON_KEY);
	}
	else
	{
		api_ClrRunHardFlag(eRUN_HARD_COMMON_KEY);
	}

}

//---------------------------------------------------------------
//函数功能: 刷新身份认证权限标志
//
//参数: 
//                   
//返回值:  
//
//备注:   刷新身份认证权限 标志
//		 刷新身份认证权限 ==1，必须建立对称加密应用连接才可以抄读数据
// 		 身份认证权限打开，必须建立应用连接才可以通过明文+RN抄读数据 
//---------------------------------------------------------------
void api_FlashIdentAuthFlag( void )
{
	BYTE Buf[20];
	WORD Len;
	
	Len = api_GetEsamInfo( 0x08, Buf );
	if( Len != 0x8000 )
	{
		if( Buf[0] == 0 )
		{
			api_ClrRunHardFlag( eRUN_HARD_EN_IDENTAUTH );
		}
		else
		{
			api_SetRunHardFlag( eRUN_HARD_EN_IDENTAUTH );
		}
	}
	else
	{
		//ESAM读取错误，默认权限许可未打开
		api_ClrRunHardFlag( eRUN_HARD_EN_IDENTAUTH );
	}
}



#endif//#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#endif//#if( PREPAY_MODE != PREPAY_NO )


