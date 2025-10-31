//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.11.15
//描述		

//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"
#include "Dlt645_2007.h"

#if( PREPAY_MODE != PREPAY_NO )
#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#if( SEL_DLT645_2007 == YES )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//SERR错误字，最高位8代表错误，规约传输中没有此位;
#define		SERR_NO				0x0000	//没有错误;
#define		SERR_OTHER			0x8001	//其他错误;
#define		SERR_RECHARGE		0x8002	//重复充值;
#define		SERR_ESAM_VERIFY	0x8004	//ESAM验证错误;
#define		SERR_AUTH			0x8008	//身份认证错误;
#define		SERR_USER_ID		0x8010	//客户编号不匹配;
#define		SERR_BUY_TIMES		0x8020	//充值次数错误;
#define		SERR_OVER_REGRATE	0x8040	//购电超囤积;
#define		SERR_ADDR_ABR		0x8080	//地址异常;
#define		SERR_HUNGUP			0x8100	//电表挂起;
#define  	SERR_CLOSE_RELAY 	0x8200  //合闸复电失败;
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------																						



//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------




//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 远程身份认证
//
//参数:		ProBuf[in/out]	645报文
//				密文		18~25		in
//				随机数1		26~33		in
//				分散因子	34~41		in
//				随机数2		14~17		out
//				esam序列号	18~25		out		
//返回值:	数据长度，bit15为1代表错误
//		   
//备注:698版本ESAM 供645规约身份认证调用  认证时效及标志没有做!!!!!!
//-----------------------------------------------
static WORD RemoteAuthorization(BYTE *ProBuf,BYTE *OutBuf)
{
// 	BOOL Result;
// 	BYTE Buf[24];
// 	BYTE SecretData1[10];
// 	WORD ReturnLen,EsamLen;

// 	lib_InverseData(ProBuf+18,8);//密文1
// 	lib_InverseData(ProBuf+26,8);//随机数1
// 	lib_InverseData(ProBuf+34,8);//分散因子

// 	if( ProBuf[34] || ProBuf[35] )//分散因子高位必须为零
// 	{
// 		return SERR_OTHER;
// 	}
	
// 	if( memcmp(ProBuf+36,FPara1->MeterSnPara.MeterNo,6) != 0 )//表号必须一致
// 	{
// 		return SERR_OTHER;
// 	}
// 	api_ProcPrePayCommhead( Buf, 0x80, 0x08, 0x08, 0x03, 0x00, 0x10);
// 	memcpy( Buf+6, ProBuf+34, 8 );//添加分散因子
// 	memcpy( Buf+14, ProBuf+26, 8 );//添加随机数1
	
// 	Result = Link_ESAM_TxRx(6,Buf,16,Buf+6,SecretData1);
// 	if(Result == FALSE)
// 	{
// 		return SERR_AUTH;
// 	}
		
// 	ReturnLen = 0;
	
// 	if( memcmp(ProBuf+18,SecretData1+2,8) == 0 )
// 	{
// 		//读随机数
// 		api_ReadEsamRandom(4,Buf);
// 		lib_ExchangeData(OutBuf,Buf+2,4);
// 		ReturnLen += 4;
// 		//ESAM序列号
// 		EsamLen = api_GetEsamInfo( 0x02, Buf );
// 		if( EsamLen > (sizeof(Buf)-2) )
// 		{
// 			return SERR_OTHER;
// 		}
// 		memcpy( SafeAuPara.EsamRst8B, Buf, 8 );
// 		lib_ExchangeData(OutBuf+4,SafeAuPara.EsamRst8B,8);
		
// 	    api_SetSysStatus(eSYS_STATUS_ID_645AUTH);//置位编程允许标志
// 	    api_Release698_Connect( );
// //		api_ReadPrePayPara( eRemoteTime, (BYTE *)&APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime );//645默认是主站建立应用链接
//         APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime = (APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime*60);
//         APPConnect.ConnectInfo[eConnectMaster].ConstConnectValidTime = APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime;

// 		ReturnLen += 8;
		
// //		api_SaveProgramRecord645(EVENT_END, (BYTE *)NULL, (BYTE *)NULL );
// 		return ReturnLen;
// 	}
	
	// return SERR_AUTH;
}

//-----------------------------------------------
//函数功能: 写保存在ESAM 中的参数（此函数没被调用）CmdBuf前面2字节代表长度
//
//参数:		Type[in]	0x04--参数信息文件里面的参数更新
//						0x85--当前套电价更新
//						0x86--备用套电价更新
//			P2[in]		偏移地址
//			LC[in]		输入InData长度
//			InData[in]	Data+Mac（均为正序）
//返回值:	数据长度，bit15为1代表错误
//
//备注:698版本ESAM 供645规约写参数(参数、当前套、备用套)调用
//-----------------------------------------------
static WORD WriteEsamData( BYTE Type, WORD P2, WORD LC, BYTE *InData )
{
	BOOL Result;
	BYTE CmdBuf[7];
	
	if( (Type!=0x04) && (Type!=0x85) && (Type!=0x86) )
	{
		return 0x8000;
	}
	
	//MAC挂起
	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	{
		return 0x8000;
	}
	
	CmdBuf[0] = 0x83;
	CmdBuf[1] = 0x2a;
	CmdBuf[2] = Type;
	CmdBuf[3] = P2>>8;
	CmdBuf[4] = P2;
	CmdBuf[5] = LC>>8;
	CmdBuf[6] = LC;
	
	Result = Link_ESAM_TxRx(7,CmdBuf,LC,InData,CmdBuf);
	if(Result == FALSE)
	{
		return 0x8000;
	}
	
	return 0;
}

//-----------------------------------------------
//函数功能: 解密密文数据
//
//参数:		Type[in]	0x82--远程控制
//						0x83--二类参数更新
//						0x84--电量清零
//						0x85--事件/需量清零
//			LC[in]		输入InData长度
//			InData[in]	密文+Mac（均为正序）
//			OutBuf[out]	解密后的数据				
//返回值:	数据长度，bit15为1代表错误
//		   
//备注:698版本ESAM 供645规约远程控制、写二类参数、电量清零、事件/需量清零调用
//-----------------------------------------------
WORD api_DecipherSecretData(BYTE Type, WORD LC, BYTE *InData, BYTE *OutBuf)
{
	BOOL Result;
	BYTE CmdBuf[6];
	WORD ReturnLen;

	if( (Type<0x82) || (Type>0x85) )
	{
		return 0x8000;
	}
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x12;
	CmdBuf[2] = 0xc0;
	CmdBuf[3] = Type;
	CmdBuf[4] = LC>>8;
	CmdBuf[5] = LC;
	
	ReturnLen = 0;

	Result = Link_ESAM_TxRx(6,CmdBuf,LC,InData,OutBuf);
	if(Result == FALSE)
	{
		return 0x8000;
	}
	
	if(Type == 0x83)
	{
		//正确解密结果为00 LEN (LEN-1) DI DI DI DI DATA DATA ...
		lib_ExchangeData( (BYTE*)&ReturnLen, OutBuf, 2 );
		ReturnLen -= 1;
		memmove( OutBuf, OutBuf+3, ReturnLen );
		lib_InverseData(OutBuf,ReturnLen);
	}
	else
	{
        lib_ExchangeData( (BYTE*)&ReturnLen, OutBuf, 2 );
        memmove( OutBuf, OutBuf+2, ReturnLen );
	}
	
	return ReturnLen;
}

//-----------------------------------------------
//函数功能: 设置身份认证时效
//
//参数:		ProBuf[in/out]	645报文
//				
//				
//				
//				
//						
//返回值:	数据长度，bit15为1代表错误
//		   
//备注:
//-----------------------------------------------
static WORD RemoteSetAuthTime(BYTE *ProBuf,BYTE *OutBuf)
{
	TFourByteUnion Temp32;
	BYTE Buf[12],RecBuf[10];//6+2+4

	Temp32.d = 0;
	
    if(api_GetSysStatus(eSYS_STATUS_ID_645AUTH) == FALSE )
	{
		return SERR_AUTH;
	}
    
    lib_InverseData(ProBuf+18,2);//数据
	//lib_InverseData(ProBuf+20,4);//MAC不往ESAM中存放

	if( (ProBuf[18]==0x00) && (ProBuf[19]== 0x00) )	//安全认证时效不能写入0，若写入0则返回失败。
	{
		return SERR_OTHER;
	}

	if( lib_IsMultiBcd( ProBuf+18, 2 ) == FALSE )
	{
		return SERR_OTHER;
	}

	//写ESAM--不往ESAM中存放
	//api_ProcPrePayCommhead( Buf, 0x83, 0x2A, 0x84, 0x2B, 0x00, 0x06);
	//memcpy( Buf+6, ProBuf+18, 2 );//数据
	//memcpy( Buf+8, ProBuf+20, 4 );//MAC
	//if(Link_ESAM_TxRx(6,Buf,6,Buf+6,RecBuf) == FALSE)
	{
	//	return SERR_ESAM_VERIFY;
	}

	//写EEprom
	lib_InverseData(ProBuf+18,2);
	memcpy(Temp32.b,ProBuf+18,2);
	Temp32.d = lib_DWBCDToBin(Temp32.d);
	api_WritePrePayPara(eRemoteTime,(BYTE *)&Temp32);
	//编程记录!!!!!

	return 2;
}

//-----------------------------------------------
//函数功能: 状态查询
//
//参数:		ProBuf[out]	输出645格式状态查询 已经分项倒序
//									
//返回值:	数据长度，bit15为1代表错误
//		   
//备注:698版本ESAM 供645规约状态查询调用
//-----------------------------------------------
static WORD ReadEsamStatus(BYTE *ProBuf,BYTE *OutBuf)
{
	BOOL Result;
	BYTE CmdBuf[7];
	BYTE DataBuf[20];
	WORD ReturnLen;

	if(api_GetSysStatus(eSYS_STATUS_ID_645AUTH) == FALSE )
	{
		return SERR_AUTH;
	}

	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	{
		return SERR_HUNGUP;
	}
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x48;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x00;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x0d;
	CmdBuf[6] = 0x48;
	//表号
	DataBuf[0] = 0x00;
	DataBuf[1] = 0x00;	
	memcpy(DataBuf+2,FPara1->MeterSnPara.MeterNo,6);
	//随机数
	memcpy(DataBuf+8,SafeAuPara.EsamRand,4);
	
	Result = Link_ESAM_TxRx(7,CmdBuf,12,DataBuf,DataBuf);//
	if(Result == FALSE)
	{
		return SERR_OTHER;
	}
	
	ReturnLen = 0;
	
	//剩余金额
	lib_ExchangeData(OutBuf,DataBuf+2,4);
	ReturnLen += 4;
	//金额MAC
	memset(OutBuf+4,0x00,4);
	ReturnLen += 4;
	//购电次数
	lib_ExchangeData(OutBuf+8,DataBuf+2+4,4);
	ReturnLen += 4;
	//次数MAC
	lib_ExchangeData(OutBuf+12,DataBuf+2+4+4+6,4);
	ReturnLen += 4;
	//客户编号
	lib_ExchangeData(OutBuf+16,DataBuf+2+4+4,6);
	ReturnLen += 6;
	//密钥信息，645格式为4字节，因为698无此数据，直接强制置为全0
	//api_GetEsamInfo(0x04,DataBuf);
	memset(OutBuf+22,0x00,4);
	ReturnLen += 4;
	
	return ReturnLen;
}


//-----------------------------------------------
//函数功能: 数据回抄
//
//参数:	ProBuf[in/out]	645报文
//				文件目录	24~25		in
//				文件号		22~23		in
//				起始地址	20~21		in
//				长度		18~19		in
//				返回数据	14~nn		out						
//返回值:	数据长度，bit15为1代表错误
//		   
//备注:698版本ESAM 供645规约数据回抄调用 此函数指令流有问题!!!!!!
//-----------------------------------------------
static WORD ReadEsamDataWithMac(BYTE *ProBuf,BYTE *OutBuf)
{
	BOOL Result;
	BYTE CmdBuf[7];
	BYTE DataBuf[20];
	WORD ReturnLen;
	//此函数还是有问题--181220-songchen
	if(api_GetSysStatus(eSYS_STATUS_ID_645AUTH) == FALSE )
	{
		return SERR_AUTH;
	}

	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	{
		return SERR_HUNGUP;
	}

	if(ProBuf[22] != 1)
	{
		CmdBuf[0] = 0x83;
		CmdBuf[1] = 0x2c;
		CmdBuf[2] = ProBuf[23];
		CmdBuf[3] = ProBuf[22];
		CmdBuf[4] = ProBuf[21];
		CmdBuf[5] = ProBuf[20];
		CmdBuf[6] = 0x48;
		//表号
		DataBuf[0] = 0x00;
		DataBuf[1] = 0x00;	
		memcpy(DataBuf+2,FPara1->MeterSnPara.MeterNo,6);
		memcpy(DataBuf+8,SafeAuPara.EsamRand,4);
		DataBuf[12] = ProBuf[19];
		DataBuf[13] = ProBuf[18];
		DataBuf[14] = 0x04;
		DataBuf[15] = 0xd6;
		DataBuf[16] = 0x85;
		DataBuf[17] = 0x00;
		memcpy( (BYTE*)&ReturnLen, ProBuf+18, 2 );
		DataBuf[18] = (ReturnLen+4)>>8;
		DataBuf[19] = (ReturnLen+4);
		
		Result = Link_ESAM_TxRx(7,CmdBuf,20,DataBuf,ProBuf+14);
		if(Result == FALSE)
		{
			return SERR_OTHER;
		}
		
		lib_ExchangeData( (BYTE*)&ReturnLen, ProBuf+14, 2 );
		memmove(ProBuf+14,ProBuf+14+2,ReturnLen);
		lib_ExchangeData(OutBuf,ProBuf+14,ReturnLen);
	}
	else
	{
		ReturnLen = ReadEsamStatus(ProBuf,OutBuf);
	}
	
	return ReturnLen;

}

//-----------------------------------------------
//函数功能: 红外查询请求命令
//
//参数:		
//		ProBuf[in/out]		in--输入缓冲645的第一个字节68开头
//							out--输出数据，已经倒序。					
//返回值:	数据长度
//		   
//备注:698版本ESAM 供645规约03命令字调用
//-----------------------------------------------
static WORD Ir_Request( BYTE *UartBuf, BYTE *OutBuf )
{
	BYTE result;
	BYTE Buf[30];
	BYTE K1[10];
	WORD EsamLen;
								
	//返回信息: 表号(6)+ESAM序列号(8)+随机数1密文(8)+随机数2(8)
	api_ResetEsamSpi();			//Esam Reset
	EsamLen = api_GetEsamInfo( 0x02, Buf ); //读取ESAM序列号
	if( EsamLen > (sizeof(Buf)-2) )
	{
		return SERR_OTHER;
	}
	memcpy( SafeAuPara.EsamRst8B, Buf, 8 );
	
	//表号补0000
	Buf[6] = 0x00;
	Buf[7] = 0x00;
	memcpy( Buf+8, FPara1->MeterSnPara.MeterNo, 6 );
	lib_ExchangeData( Buf+14, UartBuf+18, 8 );	//随机数1
	api_ProcPrePayCommhead( Buf, 0x80, 0x08, 0x08, 0x03, 0x00, 0x10);
	
	result = Link_ESAM_TxRx(6,Buf,16,Buf+6,K1);
	if(result != TRUE)
	{
		return SERR_OTHER;
	}

	//读随机数2
	result = api_ReadEsamRandom(8,Buf);
	if(result != TRUE)
	{
		return SERR_OTHER;
	}
	
	lib_ExchangeData( OutBuf, FPara1->MeterSnPara.MeterNo, 6 );//表号, 6bytes
	lib_ExchangeData( OutBuf + 6, SafeAuPara.EsamRst8B, 8 ); //ESAM序列号, 8bytes
	lib_ExchangeData( OutBuf+14, K1+2, 8 );	//随机数1密文, 8bytes
	lib_ExchangeData( OutBuf + 22, SafeAuPara.EsamRand, 8 );   //随机数2, 8bytes

	return 30;
}
//-----------------------------------------------
//函数功能:红外认证
//
//参数:		
//		ProBuf[in/out]		in--输入缓冲645的第一个字节68开头
//							out--输出数据，已经倒序。					
//返回值:	数据长度
//		   
//备注:698版本ESAM 供645规约03命令字调用
//-----------------------------------------------
static WORD Ir_Authorazation( BYTE *ProBuf, BYTE *OutBuf )
{
// 	BYTE result;
// 	BYTE Buf[20];	
									
// 	lib_InverseData( ProBuf+18, 8 );	//随机数2密文
// 	api_ProcPrePayCommhead( Buf, 0x80, 0x06, 0x00, 0x01, 0x00, 0x08);
// 	memcpy( Buf+6, ProBuf+18, 8 );//添加分散因子
		
// 	result = Link_ESAM_TxRx(6,Buf,8,Buf+6,Buf);
// 	if( result != TRUE )
// 	{
// //		APPConnect.IRAuthTime = 0;
// 		api_ClrSysStatus(eSYS_IR_ALLOW_PRG);
// 		return SERR_ESAM_VERIFY;
// 	}				
// //	api_ReadPrePayPara( eIRTime, (BYTE *)&APPConnect.IRAuthTime );
// 	APPConnect.IRAuthTime*=60;
	
// 	if( APPConnect.IRAuthTime == 0 )//如果红外认证时效参数为0，则取保护区的默认红外认证默认30min;
// 	{
// 		APPConnect.IRAuthTime = 1800;
// 	}
// 	api_SetSysStatus(eSYS_IR_ALLOW_PRG);
	
// 	return 0;
}

//-----------------------------------------------
//函数功能: 安全处理函数
//
//参数:		
//		ProBuf[in/out]		in--输入缓冲645的第一个字节68开头
//							out--输出数据，已经倒序?
//		PortType			通道		
//返回值:	数据长度
//		   
//备注:698版本ESAM 供645规约03命令字调用
//-----------------------------------------------
WORD api_ProcProPrePay(eSERIAL_TYPE PortType, BYTE *ProBuf ,BYTE *OutBuf )
{
	WORD wSerr;
	DWORD DI645;
	
	wSerr = SERR_OTHER;
	
	memcpy((BYTE*)&DI645,ProBuf+10,4);
	
	//68H A0 … A5 68H 03H L DIO … DI3 C0 … C3 N1 … Nm CS 16H
	switch( DI645 )
	{
		// case 0x070000ff://身份认证指令
		// 	//if((PortType == eIR)&&(api_GetSysStatus(eSYS_IR_ALLOW_PRG)==FALSE)&&(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY )==FALSE ))
		// 	//{
		// 	//	break;
		// 	//}
		// 	wSerr = RemoteAuthorization(ProBuf,OutBuf);	
		// 	break;
		case 0x070001ff://身份认证时效设置
			wSerr = RemoteSetAuthTime(ProBuf,OutBuf);
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )	//本地费控表
		case 0x070101ff://开户
		case 0x070102ff://充值
		case 0x070103ff://钱包初始化
			//wSerr = Remote_Proc_Account(ProBuf);
			break;
		#endif
		case 0x078001ff://数据回抄
			wSerr = ReadEsamDataWithMac(ProBuf,OutBuf);
			break;
        //case 0x078003ff://红外请求
        	//if((PortType == eIR)||(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ))
        	//{
        	//	wSerr = Ir_Request(ProBuf,OutBuf);
        	//}
            //break;
        //case 0x070003ff://红外认证
        //	if((PortType == eIR)||(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ))
        //	{
        //		wSerr = Ir_Authorazation(ProBuf,OutBuf);
        //	}	
        //    break;	
		case 0x078102ff://状态查询
			wSerr = ReadEsamStatus(ProBuf,OutBuf);
			break;
		
		default:
			break;
	}
	
	return wSerr;
}


//-----------------------------------------------
//函数功能: 继电器控制命令操作
//
//参数:	ProBuf[in/out]	645报文		
//		
//返回值:	执行状态，两个字节
//		   
//备注:698版本ESAM 供645规约1c命令字调用
//-----------------------------------------------
//0拉闸 1允许合闸 2直接合闸 3预跳闸1 4预跳闸2	5保电 6保电解除 7占位 8占位 9占位 10上电 11报警 12报警解除
#if(PREPAY_MODE == PREPAY_LOCAL)
static const BYTE CtrlCmd645[] = {0x1a,0x1b,0x1c,0x1d,0x1e,0x3a,0x3b,0xA5,0xA5,0xA5,0xff,0x2a,0x2b};
#else
static const BYTE CtrlCmd645[] = {0x1a,0x1b,0x1c,0x1d,0x1e,0x3a,0x3b,0xff,0x2a,0x2b};
#endif
WORD api_ProcRemoteRelayCommand645(BYTE *ProBuf)
{
	BYTE CtrlCmd,i;
	BYTE TimeBuf[6];
	WORD wRelayOffWarnTime;
	WORD ReturnStatus;
	WORD wRelayWaitOffTime;
	
	//密码为98级
	if(ProBuf[10] == CLASS_2_PASSWORD_LEVEL)
	{
		//MAC挂起
		if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
		{
			return R_ERR_MAC;
		}
		
		//密文
		lib_InverseData(ProBuf+22,16);
		//MAC
		lib_ExchangeData( ProBuf+22+16, ProBuf+18,  4 );
		//解密
		ReturnStatus = api_DecipherSecretData(0x82, 0x14, ProBuf+22, ProBuf+22);
		if(ReturnStatus & 0x8000)
		{
			return R_ERR_MAC;
		}
		
	}
	else if( (ProBuf[10]==MINGWEN_H_PASSWORD_LEVEL) || (ProBuf[10]==MINGWEN_L_PASSWORD_LEVEL) )
	{	
	}
	else
	{
		return R_ERR_PASSWORD;
	}
	
	CtrlCmd = ProBuf[22];
	if( api_JudgeClock645( ProBuf+24 ) == FALSE )
	{
		return R_ERR_TIME;
	}
	
	//跳闸自恢复时间，单位分钟
	if( (CtrlCmd==0x1d) || (CtrlCmd==0x1e) )
	{
		wRelayOffWarnTime = (DWORD)ProBuf[23]*5;
	}
	else
	{
		wRelayOffWarnTime = 0;
	}
	
	//跳闸延时时间
	if(CtrlCmd == 0x1a)
	{
		ReadRelayPara(2,(BYTE*)&wRelayWaitOffTime);
	}
	else
	{
		wRelayWaitOffTime = 0;
	}
	
	for(i=0;i<sizeof(CtrlCmd645);i++)
	{
		if(CtrlCmd == CtrlCmd645[i])
		{
			break;
		}
	}
	
	ReturnStatus = R_ERR_PASSWORD;

	#if(PREPAY_MODE == PREPAY_LOCAL)
	if((i==7)||(i==8)||(i==9))//CtrlCmd645加入三个占位命令0xa5，如果下发的命令真的是这个，则返回错误
	{
		return ReturnStatus;
	}
	#endif
	
	if( i < sizeof(CtrlCmd645) )
	{
		ReturnStatus = api_Set_RemoteRelayCmd(i, wRelayWaitOffTime, wRelayOffWarnTime);
	}
	
	return ReturnStatus;
}


//-----------------------------------------------
//函数功能: 清零操作解密数据（没有使用此函数）
//
//参数:	ProBuf[in/out]	645报文	，解密后的数据放在从ProBuf+22开始的数据
//		
//返回值:	TRUE/FALSE
//		   
//备注:698版本ESAM 供645规约19/1A/1B命令字调用
//-----------------------------------------------
BOOL api_ProcClrCmd645(BYTE *ProBuf)
{
	BYTE Type;
	WORD ReturnStatus;
	
	if(ProBuf[10] != CLASS_2_PASSWORD_LEVEL)
	{
		memmove( (BYTE*)&ProBuf[22], (BYTE*)&ProBuf[18], ProBuf[9]-8);
		lib_InverseData( (BYTE*)&ProBuf[24], 6 );//对时间进行倒序
		return TRUE;
	}
	
	//MAC挂起
	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	{
		return FALSE;
	}
	
	if(ProBuf[8] == 0x1a)
	{
		Type = 0x84;
	}
	else if(ProBuf[8] == 0x1b)
	{
		Type = 0x85;
	}
	#if( MAX_PHASE == 3 )
	else if(ProBuf[8] == 0x19)
	{
		Type = 0x85;
	}
	#endif
	else
	{
		return FALSE;
	}
	lib_InverseData( ProBuf+22, 16 );
	lib_ExchangeData(ProBuf+22+16,ProBuf+18, 4);
	//解密
	ReturnStatus = api_DecipherSecretData(Type, 0x14, ProBuf+22, ProBuf+22);
	if(ReturnStatus == 0x8000)
	{
		return FALSE;
	}
	
	return TRUE;
}
#endif//#if( SEL_DLT645_2007 == YES )

#if(SEL_DLT698_2016_FUNC == YES )
#if( PREPAY_MODE == PREPAY_LOCAL)

//-----------------------------------------------
//函数功能: 645更新一类参数
//
//参数:		Type[in]	0x84--参数信息文件
//						0x85--当前套电价文件，包括费率和阶梯
//						0x86--备用套电价文件，包括费率和阶梯
//			P1[in]		起始地址
//			LC[in]		输入InData长度
//			InData[in]	明文+Mac（均为正序）			
//返回值:	成功/失败
//		   
//备注:698版本ESAM 供645规约更新esam中一类参数
//-----------------------------------------------
BYTE api_UpdateFirstData645(BYTE Type, BYTE P1,WORD LC, BYTE *InData)
{
	BOOL Result;
	BYTE CmdBuf[6];
	BYTE OutBuf[20];
	
	if( (Type<0x84) || (Type>0x86) )
	{
		return FALSE;
	}
	
	CmdBuf[0] = 0x83;
	CmdBuf[1] = 0x2a;
	CmdBuf[2] = Type;
	CmdBuf[3] = P1;
	CmdBuf[4] = LC>>8;
	CmdBuf[5] = LC;
	

	Result = Link_ESAM_TxRx(6,CmdBuf,LC,InData,OutBuf);

	return Result;
}

//-----------------------------------------------
//函数功能: 645写费率电价、阶梯电价、费率切换时间、阶梯切换时间
//
//参数:		Type[in]	0x82--远程控制
//						0x83--二类参数更新
//						0x84--电量清零
//						0x85--事件/需量清零
//			LC[in]		输入InData长度
//			InData[in]	密文+Mac（均为正序）
//			OutBuf[out]	解密后的数据				
//返回值:	数据长度，bit15为1代表错误
//		   
//备注:698版本ESAM 供645规约写一类参数
//-----------------------------------------------
BYTE api_Proc_uart_Write_FirstData_645( BYTE *pBuf )
{
	BYTE k,i,j,m,n, result,ESAMFile,ESAMOffset;
	TFourByteUnion DI07;
	TFourByteUnion Tmpbuf;
	WORD ParaType,ESAMLenth;
	TRealTimer TmpRealTimer;
	TRatePrice TmpRatePrice;
	TLadderPrice TmpLadderPrice;
	
	memcpy(DI07.b,pBuf+10,4);
	k = 0;
	
	switch( DI07.d )
	{
		#if( PREPAY_MODE == PREPAY_LOCAL )				//本地费控表
		case 0x04000108://两套费率电价切换时间			YYMMDDhhmm	5	年月日时分
		case 0x04000109://两套阶梯电价切换时间			YYMMDDhhmm	5	年月日时分	
			//数据分在最低字节，年在最高字节。如果写入ESAM,数据需要转换
			//判断切换时间是否合法
			if( CheckSwitchTime( pBuf+22 ) == 0x01 )
            {
                break;
            }
				
			if( lib_IsMultiBcd( pBuf+22, 5 ) == FALSE )	
			{	
				break;
			}
			
			//明文数据--倒序后，分在最高字节，年在最低字节
			lib_InverseData( pBuf+22,pBuf[9]-16 );
			//MAC
			lib_InverseData( pBuf+22+pBuf[9]-16, 4 );

			ESAMLenth = 5+4;			//5位数据+4位MAC
			if( DI07.d == 0x04000108 )	//两套费率电价切换时间
            {               
				ESAMOffset = 10;
				ParaType = 2;
				ESAMFile = 0x84;
            }
            else						//两套阶梯电价切换时间
            {  
				ESAMOffset = 196;
				ParaType = 3;
				ESAMFile = 0x86;
            }
			result = api_UpdateFirstData645( ESAMFile, ESAMOffset,ESAMLenth, pBuf+22);//写入ESAM
			if( result != TRUE )	
			{	
				break;
			}
			//明文--倒序后，恢复年在最高字节，分在最低字节
			lib_InverseData( pBuf+22,pBuf[9]-16 );
			
			api_ConvertYYMMDDhhmm_TRealTimer( &TmpRealTimer, pBuf+22 );
            
			if( api_ReadAndWriteSwitchTime( WRITE, ParaType, (BYTE *)&TmpRealTimer ) == FALSE )
			{
				break;
			}	
			
			k = 1;
			break;
		case 0x040502ff:	//设置备用套费率电价			
			//检验报文及数据有效性
			m = (pBuf[9]-16);
			n = m/4;
			if( (m%4) || (n==0) || (n>MAX_RATIO) ) 
			{
				break; //数据长度错误, 退出 
			}

			if( lib_IsMultiBcd( pBuf+22, m ) == FALSE )	
			{	
				break;
			}
			
            //明文数据倒序--为适应ESAM数据要求
            for(i=0;i<n;i++)
			{
				lib_InverseData( pBuf+22+i*4,4 );
            }	
			//MAC倒序
			lib_InverseData( pBuf+22+pBuf[9]-16, 4 );

			ESAMOffset = 4;
			ESAMFile = 0x86;
			ESAMLenth = m+4;
			result = api_UpdateFirstData645( ESAMFile, ESAMOffset,ESAMLenth, pBuf+22);
			if( result != TRUE )
			{
				break;
			}

			for( i=0; i<n; i++ ) 
        	{
				lib_ExchangeData( Tmpbuf.b, (BYTE*)&pBuf[4*i+22], 4 );//获取电价
      		 	TmpRatePrice.Price[i] = lib_DWBCDToBin(Tmpbuf.d);//BCD转化为Hex
        	}
			
			for( i=n; i<MAX_RATIO; i++ )//把不足5费率的，用最后一个费率填上
			{
				lib_ExchangeData( Tmpbuf.b, (BYTE*)&pBuf[22+4*(n-1)], 4 );//获取电价
   				TmpRatePrice.Price[i] = lib_DWBCDToBin(Tmpbuf.d);//BCD转化为Hex
			}
			
			if( api_WritePrePayPara( eBackupRateTable, (BYTE*)&TmpRatePrice.Price ) == FALSE )
		    {
		        break;
		    }
			
			k = 1;
			break;
		case 0x040605FF:	//设置备用套阶梯值+阶梯电价										
			if(pBuf[9] != (4*MAX_LADDER+4*(MAX_LADDER+1)+MAX_YEAR_BILL*3+16) )//数据长度正确性检查
			{
				break;
			}
			if( lib_IsMultiBcd( pBuf+22, (4*MAX_LADDER+4*(MAX_LADDER+1)+MAX_YEAR_BILL*3) ) == FALSE )	//是否bcd检查
			{	
				break;
			}

			for( i=0; i<MAX_YEAR_BILL; i++ )//645非法时间只有99允许，其他不可以；合法时间日期不能超过28日
			{
				if( api_CheckClockBCD( 2, 3+0x80, pBuf+22+4*MAX_LADDER+4*(MAX_LADDER+1) ) == FALSE )
				{
					for( j=0; j<3; j++ )
					{
						if( pBuf[22+4*MAX_LADDER+4*(MAX_LADDER+1)+3*i+j] != 0x99 )
						{
							break;
						}
					}
				}
				else //年结算日天不能超过28
				{
					if( pBuf[22+4*MAX_LADDER+4*(MAX_LADDER+1)+3*i+1] > 0x28 )
					{
						break;
					}
				}
			}
			if( i<MAX_YEAR_BILL )
			{
				break;
			}

		   if((lib_IsAllAssignNum( pBuf+22, 0x00,24 )==FALSE) 
             && (api_JudgeYearClock( pBuf+22+4*MAX_LADDER+4*(MAX_LADDER+1) ) == FALSE)) 
            {	
                break; //阶梯值非全0但是4个阶梯结算日全部无效则不允许设置
            }
			 
            //明文数据分项倒序
            for(i=0;i<13;i++)//阶梯值和阶梯电价
			{
				lib_InverseData( pBuf+22+i*4,4 );
            }
			for(i=0;i<MAX_YEAR_BILL;i++)//年结算日
			{
				lib_InverseData( pBuf+22+4*MAX_LADDER+4*(MAX_LADDER+1)+3*i,3 );
            }
			//MAC倒序
			lib_InverseData( pBuf+22+pBuf[9]-16, 4 );

			ESAMOffset = 132;
			ESAMFile = 0x86;
			ESAMLenth = 64+4;
			result = api_UpdateFirstData645( ESAMFile, ESAMOffset,ESAMLenth, pBuf+22);
			if( result != TRUE )
			{
				break;
			}

			for( i=0; i<MAX_LADDER; i++ )//阶梯电量
		    {  
		  		lib_ExchangeData( Tmpbuf.b, (BYTE*)&pBuf[22+4*i], 4 );//获取阶梯电量
		  		TmpLadderPrice.Ladder_Dl[i] = lib_DWBCDToBin(Tmpbuf.d);//BCD转化为Hex     
		    }

		    for( i=0; i<MAX_LADDER+1; i++ )//阶梯电价
		    {
				lib_ExchangeData( Tmpbuf.b, (BYTE*)&pBuf[22+4*MAX_LADDER+4*i], 4 );//获取阶梯电价
		  		TmpLadderPrice.Price[i] = lib_DWBCDToBin(Tmpbuf.d);//BCD转化为Hex 
		    }

  			//memset( (BYTE*)TmpLadderPrice.YearBill, 99, sizeof(TmpLadderPrice.YearBill) );//先置结算日为无效--这行没什么用处--songchen
			for( i=0; i<MAX_YEAR_BILL; i++ )//年结算日
		    {    
	            for( j=0; j<3;j++ )
	            {
	                TmpLadderPrice.YearBill[i][j] = pBuf[22+4*MAX_LADDER+4*(MAX_LADDER+1)+3*i+j];
	                //if( lib_IsBCD(TmpLadderPrice.YearBill[i][j]) == TRUE )--不需要检查，前面已经检查了是否bcd和合法性--songchen
	                {
	                    TmpLadderPrice.YearBill[i][j] = lib_BBCDToBin( TmpLadderPrice.YearBill[i][j] );
	                }
	            }    
			}
			
			if( api_WritePrePayPara( eBackupLadderTable, (BYTE*)&TmpLadderPrice.Ladder_Dl ) == FALSE )
		    {
		        break;
		    }
			
			k = 1;
			break;	
		#endif//#if( PREPAY_MODE == PREPAY_LOCAL )	//本地费控表								

		default:
			break;
	}
	
	if( k == 0 )
	{
		return FALSE;
	}	
	return TRUE;	
}

BYTE api_RemoteActionMoneybag( BYTE Type, DWORD Money, DWORD BuyTimes, BYTE *pCustomCode, BYTE *pSIDMAC, BYTE *pMeterNo )
{
    WORD Result;
	TFourByteUnion TmpMoneyInMeter;
	DWORD TmpBuyTimesInMeter;
    WORD TmpNewBuyTimes;
    BYTE OpenFlag;

    if((memcmp( FPara1->MeterSnPara.MeterNo, pMeterNo, 6)) != 0 )//表号不相等返回失败
    {
        return DAR_OtherErr;
    }

	//读剩余金额和购电次数
	api_ReadPrePayPara( eRemainMoney, (BYTE*)TmpMoneyInMeter.b );
	api_ReadPrePayPara( eBuyTimes, (BYTE*)&TmpBuyTimesInMeter );
      
    if( (Type == 0) || (Type == 1) )//开户 充值
    {
        if( Type == 0 )//开户 判断购电次数
        {
            if( BuyTimes > 1 )
            {
                return DAR_ChargeTimes;
            }
        }

        if( (CardInfo.LocalFlag == eOpenAccount) || (CardInfo.RemoteFlag == eOpenAccount))//远程或本地已开户
        {
            if((memcmp( FPara1->MeterSnPara.CustomCode, pCustomCode, 6)) != 0 )//判断客户编号必须相等
            {
                return DAR_CustomNo;
            }
        }
        else
        {
            if( Type == 1 )//充值
            {
                return DAR_ChargeTimes;
            }
        }
        
        if( TmpBuyTimesInMeter > BuyTimes )//已购电次数大于购电次数 返回失败
        {
            return DAR_ChargeTimes;
        }
        else
        {
            TmpNewBuyTimes = (BuyTimes - TmpBuyTimesInMeter);
        }

        if( TmpNewBuyTimes > 1 )//购电次数大于1返回失败
        {
            return DAR_ChargeTimes;
        }

        if( TmpNewBuyTimes == 1 )
        {
			// 剩余+充值
			if( (long)(TmpMoneyInMeter.l + Money) > 99999999L ) //相加后溢出了(超过999999.99元)
			{
				return DAR_OverHold;
			}
			//判断囤积金额
			if( LimitMoney.RegrateLimit.d )
			{
				if( (long)(TmpMoneyInMeter.l + Money) > LimitMoney.RegrateLimit.l ) //充值前验证是否超囤积 //隐式转换，有符号转换为无符号运算 - 2018.12.28 - lzn
				{
					return DAR_OverHold;
				}
			}

             //验证ESAM
             Result = api_DealMoneyDataToESAM( Money, BuyTimes, pCustomCode, pSIDMAC );
             if( Result == FALSE )
             {
                return DAR_EsamFial;
             }

             api_UpdataRemainMoney( eChargeMoneyMode, Money , BuyTimes);
			 api_RemoteChargeCloseRelay(TmpMoneyInMeter.l);
        }
        else
        {
			if( Type == 0 )
			{
				//验证ESAM 更新客户编号
				if( CardInfo.RemoteFlag != eOpenAccount )//远程开户不验证esam
				{
					Result = api_DealMoneyDataToESAM( Money, BuyTimes, pCustomCode, pSIDMAC );
					if( Result == FALSE )
					{
					   return DAR_EsamFial;
					}
				}
			}
			else if( Type == 1 ) //远程充值如果次数相等提示重复充值
			{
				return DAR_Recharge;
			}
		}
        

        //更新本地状态

        
        if( Type == 0 )//如果是开户 保存客户编号
        {
            //保存客户编号
            Result = api_ProcMeterTypePara( WRITE, eMeterCustomCode, pCustomCode );
            if( Result == FALSE )//写epprom失败 充值成功！！！！！
            {
               return DAR_OtherErr;
            }

            //置位远程开户状态
            OpenFlag = eOpenAccount;
			api_WritePrePayPara( eRemoteAccountFlag, (BYTE *)&OpenFlag );
        }

    }
    else if( Type == 2 )//退费
    {
        if( TmpBuyTimesInMeter != BuyTimes )
        {
            return DAR_ChargeTimes;
        }
		//退费前，同步ESAM和表内剩余金额
		MeteringUpdateMoneybag();
        //验证ESAM
        Result = api_DealMoneyDataToESAM( Money, BuyTimes, pCustomCode, pSIDMAC );
        if( Result == FALSE )
        {
            return DAR_EsamFial;
        }

        api_UpdataRemainMoney( eReturnMoneyMode, Money , BuyTimes);
    }
    else
    {
        return DAR_EsamFial;
    }

    return DAR_Success;
}
#endif
#endif//#if(SEL_DLT698_2016_FUNC == YES )

#endif//#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#endif//#if( PREPAY_MODE != PREPAY_NO )

