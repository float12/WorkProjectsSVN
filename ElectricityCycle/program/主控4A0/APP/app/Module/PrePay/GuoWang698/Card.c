//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.8.17
//描述		

//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"


#if( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------



//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------																						

//设置信息文件参数
typedef enum
{
	eSetRateSwitchTime = 0, //两套分时费率切换时间
	eSetLadderSwitchTime,   //两套阶梯切换时间
	eSetAlarmLimit1,        //报警金额1
	eSetAlarmLimit2,        //报警金额2
	eSetCTCoe,              //电流变比
	eSetPTCoe,              //电压变比
	eSetMeterNo,            //表号
	eSetCustomCode,         //客户编号
}eSetInforParaType; 

//主要供购电过程使用
typedef struct TFourByteMoneyBag_t
{
	TFourByteUnion		Remain;        //剩余金额
	DWORD				BuyTimes;      //购电次数
}TFourByteMoneyBag;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
static BYTE 	CardCheck;                      //卡检测
static BYTE		CardType;						//卡类型
static BYTE		Ic_Err_Reg;                     //IC卡操作错误标识
static BYTE		UpdateFlag;                     //更新标志
static BYTE 	InsertCardSta;                  //插卡状态字
BYTE			CommandHead[7];					//暂存命令头,第一个字节代表ESAM（后面6个字节命令头）、CARD（后面5个字节命令头）。

//参数信息文件各个参数对应api_WritePrePayPara函数的枚举号
const ePrePayParaType ParaFileEnum[4] = { eAlarm_Limit1, eAlarm_Limit2, eCTCoe, ePTCoe };

static TFourByteMoneyBag 	FourByteNewBuyMoneyBag; 
static TFourByteMoneyBag 	FourByteOldBuyMoneyBag; //为记录购电记录准备的结构体

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 选择文件
//
//参数:
//			F1[in]	文件编号
//			F2[in]	文件编号
//返回值:	TRUE/FALSE
//
//备注:
//-----------------------------------------------
static BYTE SelectFile( BYTE F1, BYTE F2 )
{
	BYTE Result;

	BYTE Buf[10];
	Buf[0] = F1;  //DATA0
	Buf[1] = F2;  //DATA1
	Result = Link_Card_TxRx( 0x00, 0xa4, 0x00, 0x00, 0x02, WRITE, Buf );

	return Result;
}

//读卡和ESAM中二进制文件
static BYTE ReadBinFile( BYTE P1, BYTE P2, BYTE P3, BYTE *peeerom )
{
	BYTE Result;

	Result = Link_Card_TxRx( 0x00, 0xb0, P1, P2, P3, READ, peeerom );

	return Result;
}

//-----------------------------------------------
//函数功能: 带MAC读取卡片中二进制数据
//
//参数:
//			CH[in]
// 				01:读取用户卡和预置卡中除了钱包之外的文件
// 				21:读取预置卡中钱包文件
// 				31:读取用户卡中钱包文件
//			P1[in],P2[in],M1[in],M2[in]
// 				指令流的格式，见参数手册说明
//			Len[in]
// 				从卡中读出数据的长度+4字节mac的长度(Lc+4)
//返回值:	成功/失败
//
//备注:
//
//-----------------------------------------------
static BYTE ReadBinWithMacCard( BYTE CH, BYTE P1, BYTE P2, BYTE M1, BYTE M2, BYTE Len, BYTE *peerom )
{
	BYTE Result;
	BYTE Buf[10];

	Result = FALSE;

	memcpy( Buf, SafeAuPara.EsamRand, 4 );
	Buf[4] = 0x83;
	Buf[5] = 0x2A;
	Buf[6] = M1;
	Buf[7] = M2;
	Buf[8] = Len;

	if( CH == 0x21 )
	{
		Buf[5] = 0x3E;
	}
	else if( CH == 0x31 )
	{
		Buf[4] = 0x80;
		Buf[5] = 0x42;
	}
	Result = Link_Card_TxRx( 0x04, 0xb0, P1, P2, 0x09, WRITE, Buf );
	if( Result != TRUE )
	{
		return FALSE;
	}
	CLEAR_WATCH_DOG;
	Result = Link_Card_TxRx( 0x00, 0xC0, 0x00, 0x00, Len, READ, peerom ); //返回data+MAC
	if( Result != TRUE )
	{
		return FALSE;
	}

	return Result;
}

//-----------------------------------------------
//函数功能: 取CARD随机数
//
//参数:		Len[in]		随机数长度 4/8/10
//			peeerom[out]	输出长度+数据（长度两字节，高在前，低在后）
//返回值:	TRUE/FALSE
//
//备注:
//-----------------------------------------------
BOOL ReadRandomCARD( BYTE Len, BYTE *peeerom )
{
	BYTE Result;

	Result = Link_Card_TxRx( 0x00, 0x84, 0x00, 0x00, Len, READ, peeerom );
	if( Result != TRUE )
	{
		return Result;
	}
	return Result;
}

//-----------------------------------------------
//函数功能: 读非法插卡次数
//
//参数:  		Buf[out] 输出非法插卡次数 
//
//返回值:		无
//		   
//备注:			
//-----------------------------------------------
void api_ReadIllegalCardNum(BYTE *Buf)
{
	memset( (void *)Buf, 0x00, 4 );
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(PrePayConMem.dwIllegalCardNum), sizeof(DWORD), Buf);	
	//lib_InverseData(Buf, 4 );
}

//-----------------------------------------------
//函数功能:     异常插卡和非法插卡记录记录
//
//参数:  		无
//
//返回值:		无
//		   
//备注:			异常插卡记录和非法插卡记录都在此记录，非法插卡记录只记次数
//-----------------------------------------------	
static void Card_Err_Record(void)
{
	BYTE ErrRecordBuf[18];
	DWORD dwData;
	
	//if( Ic_Err_Reg == IC_IC_RST_ERR )
	//{
		//南网费控电能表系列技术标准答疑(二).pdf:当插入的为电能表不能识别的介质（如铁片或反插卡）时，电能表无需进行非法插卡和异常插卡记录。
		//国网功能规范：电能表对所插入的介质不能完成正常操作的行为，异常插卡包含非法插卡。注：所有触动卡座触点但操作不成功的行为都属于异常插卡。 
		//return;
	//}
	if((!CARD_IN_SLOT)&& (Ic_Err_Reg==IC_NOIN_ERR))
	{
		Ic_Err_Reg = IC_PREIC_ERR;	//提前拔卡
	}
	
	memcpy( ErrRecordBuf, SafeAuPara.CardRst8B, 8);	//卡序列号
	ErrRecordBuf[8]= lib_BBCDToBin(Ic_Err_Reg);					//错误信息字
	
	if( (Ic_Err_Reg==IC_ESAM_RDWR_ERR)
	 || (Ic_Err_Reg == IC_NOFX_ERR)
	 || (Ic_Err_Reg == IC_MAC_ERR) 
	 || (Ic_Err_Reg == IC_NOIN_ERR)
	 || (Ic_Err_Reg == IC_EXAUTH_ERR)
	 || (Ic_Err_Reg == IC_TYPE_ERR) )
	{
		//命令头保存时保存 CH,Cla,Ins,P1,P2,P31,P32,其中CH:0x01--ESAM 0x00--CARD,esam命令头为CH后的6字节，卡为CH后的5字节
		lib_ExchangeData( ErrRecordBuf+9, CommandHead, 7 );		//操作命令头
		ErrRecordBuf[16] = Err_Sw12[1];						//错误响应状态SW2
		ErrRecordBuf[17] = Err_Sw12[0];						//错误响应状态SW1
	}
	else
	{
		memset(ErrRecordBuf+9, 0x00, 9);
	}
	
	//api_SavePrgOperationRecord 中读出记录数据从 PreProgramData.Data[4] 开始，所以下行从 PreProgramData.Data[4] 保存
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[4] ), sizeof(ErrRecordBuf), ErrRecordBuf );
	api_SavePrgOperationRecord( ePRG_ABR_CARD_NO ); ////电能表异常插卡事件

	if( (Ic_Err_Reg == IC_AUTH_ERR) || (Ic_Err_Reg == IC_EXAUTH_ERR) || (Ic_Err_Reg == IC_MAC_ERR)|| (Ic_Err_Reg == IC_KEY_ERR) || (Ic_Err_Reg == IC_IC_RST_ERR) )
    {
		api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwIllegalCardNum ), sizeof(DWORD), (BYTE *)&dwData );
		dwData++;
		api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwIllegalCardNum ), sizeof(DWORD), (BYTE *)&dwData );
    }

}
//-----------------------------------------------
//函数功能: 保存用户卡序列号和开户标志
//
//参数:  		Type[in] 用户卡类型：1 开户卡 3 补卡 0 远程开户 
//
//返回值:		无
//		   
//备注:
//-----------------------------------------------								
static void UpdateUsercardSerial( BYTE Type )
{
	BYTE Flag;

	if( Type == 1)					//开户卡
	{
		Flag = eOpenAccount;                //本地开户标志
		api_WritePrePayPara( eLocalAccountFlag, (BYTE *)&Flag );
		api_WritePrePayPara(eSerialNo, SafeAuPara.CardRst8B);
	}
	else if( Type == 3)				//补卡
	{
		Flag = eOpenAccount;
		api_WritePrePayPara( eLocalAccountFlag, (BYTE *)&Flag );
		api_WritePrePayPara(eSerialNo, SafeAuPara.CardRst8B);
	}
	else if( Type == 0)				//远程开户	
	{
		Flag = eOpenAccount;                //远程开户标志
		api_WritePrePayPara( eRemoteAccountFlag, (BYTE *)&Flag );
	}

}

//---------------------------------------------------------------
//函数功能: 判断切换时间合法性 - 暂时放到这里
//
//参数: 	Buf[in]  年月日时分
//                   
//返回值:   
// 		   0x00 - 全0
// 		   0x01 - 时间错误
//		   0x02 - 时间正确
// 		   0x03 - 全9
//
//备注:   
//---------------------------------------------------------------
BYTE CheckSwitchTime( BYTE *Buf )
{
	BYTE i, result;

	if( lib_IsAllAssignNum( Buf, 0x00, 5 ) == TRUE )
	{
		return 0x00;//合法特殊时间(全0x00)
	}
	
	for( i = 0; i < 5; i++ )
	{
		if( Buf[i] != 0x99 )
		{
			break;
		}
	}
	
	if( i < 5 )
	{
		if( api_CheckClockBCD( 1, 5+0x80, Buf ) == TRUE )
		{
			return 0x02;
		}
		else
		{
			return 0x01;		   
		}
	}
	else
	{
		return 0x03;		//合法特殊时间(全0x99)
	}
}


//-----------------------------------------------
//函数功能: 带MAC写保存在ESAM 中的参数
//
//参数:  		Ch[in]		00:插卡写参数文件、电价文件
//						01: 用户卡更新ESAM钱包文件
//						02:预置卡更新ESAM钱包文件
//						03:更新运行信息文件
//			Type[in]	0x04--参数信息文件里面的参数更新
//						0x85--当前套电价更新
//						0x86--备用套电价更新
//						0x07--运行信息文件
//			P2[in]		偏移地址
//			LC[in]		输入InData长度
//			InData[in]	Data+Mac（均为正序）
//返回值:	数据长度，bit15为1代表错误
//		 
//备注:698版本ESAM 供645规约写参数(参数、当前套、备用套)调用,还可以用来更新ESAM钱包文件
//-----------------------------------------------
static WORD WriteEsamWithMac(BYTE Ch, BYTE Type, BYTE P2, WORD LC, BYTE *InData)
{
	BOOL Result;
	BYTE CmdBuf[9];
	BYTE CmdLen;
	BYTE OutBuf[20];
	
	if( (Ch == 0x00 )&&( (Type!=0x84) && (Type!=0x85) && (Type!=0x86) &&(Type!=0x8F)) )
	{
		return 0x8000;
	}
	
	//MAC挂起--不再判断MAC挂起
	
	if(Ch == 0x01)//用户卡更新ESAM钱包文件
	{	
		CmdBuf[0] = 0x80;
		CmdBuf[1] = 0x42;
		CmdBuf[2] = Type;
		CmdBuf[3] = P2;
		CmdBuf[4] = LC>>8;
		CmdBuf[5] = LC;
		CmdLen=6;
	}
	else if(Ch == 0x02)//预置卡更新ESAM钱包文件
	{	
		CmdBuf[0] = 0x83;
		CmdBuf[1] = 0x3E;
		CmdBuf[2] = Type;
		CmdBuf[3] = P2;
		CmdBuf[4] = LC>>8;
		CmdBuf[5] = LC;
		CmdLen=6;
	}
	else if( Ch ==0x00 )//插卡写参数文件、电价文件
	{
		CmdBuf[0] = 0x83;
		CmdBuf[1] = 0x2a;
		CmdBuf[2] = Type;
		CmdBuf[3] = P2;
		CmdBuf[4] = LC>>8;
		CmdBuf[5] = LC;
		CmdLen=6;
	}
	else if( Ch == 0x03 )//更新运行信息文件
	{
		CmdBuf[0] = 0x80;
		CmdBuf[1] = 0x2a;
		CmdBuf[2] = 0x00;
		CmdBuf[3] = Type;
		CmdBuf[4] = LC>>8;
		CmdBuf[5] = LC+3;
		CmdBuf[6] = 0x00;
		CmdBuf[7] = P2>>8;
		CmdBuf[8] = P2;
		CmdLen=9;	
	}
	else//目前留着，可能远程能用得到，如果用不到可以删除此情况
	{
		CmdBuf[0] = 0x83;
		CmdBuf[1] = 0x2a;
		CmdBuf[2] = Type;
		CmdBuf[3] = P2>>8;
		CmdBuf[4] = P2;
		CmdBuf[5] = LC>>8;
		CmdBuf[6] = LC;
		CmdLen=7;
	}	
	
	Result = Link_ESAM_TxRx(CmdLen,CmdBuf,LC,InData,OutBuf);
	if(Result == FALSE)
	{
		return 0x8000;
	}
	
	return 1;
}

//-----------------------------------------------
//函数功能: 系统内部认证
//
//参数:  		无
//
//返回值:		TRUE/错误类型
//		   
//备注:
//-----------------------------------------------
static WORD InterAuth( void )
{
	WORD Result;
	BYTE Buf[32];
	BYTE K1[10];
	BYTE K2[8];
 
	Result = api_ReadEsamRandom(0x08,Buf);//从ESAM取8字节随机数
	if( Result != TRUE )
	{
		return IC_ESAM_RDWR_ERR  ;	//操作esam错误；err31
	}

	api_ProcPrePayCommhead( Buf, 0x80, 0x08, 0x08, 0x01, 0x00, 0x10);//取esam加密结果K1
	memcpy( Buf+6, SafeAuPara.CardRst8B, 8 );//添加分散因子
	memcpy( Buf+14, SafeAuPara.EsamRand, 8 );//添加随机数1	
	Result = Link_ESAM_TxRx(6,Buf,16,Buf+6,K1);
	if( Result != TRUE )
	{
		return IC_AUTH_ERR;  //身份认证错误(通讯成功但密文不匹配)err32
	}
	 	
	Result = Link_Card_TxRx(0x00, 0x88, 0x00, 0x01, 0x08, WRITE,SafeAuPara.EsamRand );//取卡片加密结果K2
	if( Result != TRUE )
	{
		return IC_NOIN_ERR; //操作卡片通讯错误(读写卡片失败)；err35
	}
	Result = Link_Card_TxRx(0x00, 0xC0, 0x00, 0x00, 0x08, READ,K2 );
	if( Result != TRUE )
	{
		return IC_NOIN_ERR;
	}
	
	if( memcmp( K1+2, K2, 8) != 0 ) 
	{
		return IC_AUTH_ERR;//身份认证错误(通信成功但密文不匹配);err32
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 卡片外部认证
//
//参数:  		无
//
//返回值:		TRUE/错误类型
//		   
//备注:
//-----------------------------------------------
static WORD CardExtAuth( void )
{
	WORD Result;
	BYTE CmdBuf[26];
	BYTE K3[10];

	Result = ReadRandomCARD( 8, CmdBuf+14 );			//从卡片取8字节随机数
	if( Result != TRUE )
	{
		return IC_NOIN_ERR;
	}

	api_ProcPrePayCommhead( CmdBuf, 0x80, 0x08, 0x08, 0x02, 0x00, 0x10);
	memcpy( CmdBuf+6, SafeAuPara.CardRst8B, 8 );					//添加分散因子
	 	
	Result = Link_ESAM_TxRx(6,CmdBuf,16,CmdBuf+6,K3);
	if( Result != TRUE )
	{
		return IC_ESAM_RDWR_ERR;
	}
	
	Result = Link_Card_TxRx(0x00, 0x82, 0x00, 0x02, 0x08, WRITE,K3+2 );//卡片做外部认证
	if( Result != TRUE )
	{
		return IC_EXAUTH_ERR;	
	}

	return TRUE;
}
//-----------------------------------------------
//函数功能: 阶梯结算日合法性判断:设置需要判断升序,阶梯值非全0但是4个阶梯结算日全部无效则不允许设置
//
//参数:  		type  用户卡卡片类型  / FF:预置卡
//
//返回值:		TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
static BYTE Judge_Ladder_Data(BYTE Type)
{
	BYTE Result;
	BYTE i;
	BYTE CurLadderBuf[LADDER_BUF_LENGTH];
	BYTE BakLadderBuf[LADDER_BUF_LENGTH];
	BYTE TmpFlag;

	if((Type == BUY_CARD)||(Type == LOST_CARD))
	{
		TmpFlag = (UpdateFlag&0x7A);
	}
	else
	{
		TmpFlag = UpdateFlag;
	}
	
	//根据参数更新标志位bit2读卡片阶梯文件1，更新表内当前套阶梯电量+阶梯电价参数
	if( TmpFlag & UPDATE_CUR_LADDER )
	{		
		Result = ReadBinWithMacCard( 0x01, USER_CARD_CUR_RATE_FILE, 132, ESAM_CUR_RATE_FILE, 132, LADDER_BUF_LENGTH, CurLadderBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
		if( lib_IsMultiBcd( CurLadderBuf, 52 ) == FALSE) 
		{	
			return FALSE; //阶梯电量和阶梯电价非法
		}	
		for( i=0; i<4; i++ ) 
		{	
			lib_InverseData(CurLadderBuf+52+3*i,3);//年结算日字节交换顺序
		}
		if( (lib_IsAllAssignNum( CurLadderBuf, 0x00,24 )==FALSE) 
			&& (api_JudgeYearClock( CurLadderBuf+52 ) == FALSE) ) 
		{	
			return FALSE; //阶梯值非全0但是4个阶梯结算日全部无效则不允许设置
		}
	}
	
	//根据参数更新标志位bit3读卡片阶梯文件2，更新表内备用套阶梯电量+阶梯电价参数
	if( TmpFlag & UPDATE_BAK_LADDER )
	{
		Result = ReadBinWithMacCard( 0x01, USER_CARD_BAK_RATE_FILE, 132, ESAM_BAK_RATE_FILE, 132, LADDER_BUF_LENGTH, BakLadderBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
		if( lib_IsMultiBcd( BakLadderBuf, 52 ) == FALSE ) 
		{	
			return FALSE; //阶梯电量和阶梯电价非法
		}	
		for( i=0; i<4; i++ ) 
		{	
			lib_InverseData(BakLadderBuf+52+3*i,3);//年结算日字节交换顺序
		}
		if( (lib_IsAllAssignNum( BakLadderBuf, 0x00,24 )==FALSE) 
			&& (api_JudgeYearClock( BakLadderBuf+52 ) == FALSE) ) 
		{	
			return FALSE; //阶梯值非全0但是4个阶梯结算日全部无效则不允许设置
		}
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 根据参数更新标志位从卡中一次读取出来参数、费率电价、阶梯电价
//
//参数:  		无
//
//返回值:		TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
static WORD ReadDataFromCardWithMac( TCardParaUpdate *pCardPara )
{
	WORD Result;
	
	//带MAC读出参数更新标志位
	Result = ReadBinWithMacCard( 0x01, USER_CARD_PARA_FILE, 5, ESAM_PARA_FILE, 5, UPDATE_FLAG_LENGTH, pCardPara->FlagBuf );
	if( Result != TRUE ) 
	{
		return FALSE;
	}
	
	//根据参数更新标志位bit7读写报警金额1、2+电压、电流互感器变比+MAC
	if( UpdateFlag & UPDATE_PARA_FILE )
	{
		Result = ReadBinWithMacCard( 0x01, USER_CARD_PARA_FILE, 16, ESAM_PARA_FILE, 16, PARA_BUF_LENGTH, pCardPara->ParaBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
	}
	
	//根据参数更新标志位bit0读卡片费率文件1，更新表内当前套费率电价参数
	if( UpdateFlag & UPDATE_CUR_RATE )
	{
		Result = ReadBinWithMacCard( 0x01, USER_CARD_CUR_RATE_FILE, 0, ESAM_CUR_RATE_FILE, 0, RATE_BUF_LENGTH, pCardPara->CurRateBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}	
	}
	
	//根据参数更新标志位bit1读卡片费率文件2，更新表内备用套费率电价参数
	if( UpdateFlag & UPDATE_BAK_RATE )
	{		
		Result = ReadBinWithMacCard( 0x01, USER_CARD_BAK_RATE_FILE, 0, ESAM_BAK_RATE_FILE, 0,  RATE_BUF_LENGTH, pCardPara->BakRateBuf );
		if( Result != TRUE )  
		{
			return FALSE;
		}
		
		Result = ReadBinWithMacCard( 0x01, USER_CARD_PARA_FILE, 10,  ESAM_PARA_FILE, 10, TIME_BUF_LENGTH, pCardPara->BakRateTimeBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
	}
	
	//根据参数更新标志位bit2读卡片阶梯文件1，更新表内当前套阶梯电量+阶梯电价参数
	if( UpdateFlag & UPDATE_CUR_LADDER )
	{		
		Result = ReadBinWithMacCard( 0x01, USER_CARD_CUR_RATE_FILE, 132, ESAM_CUR_RATE_FILE, 132, LADDER_BUF_LENGTH, pCardPara->CurLadderBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}		
	}
	
	//根据参数更新标志位bit3读卡片阶梯文件2，更新表内备用套阶梯电量+阶梯电价参数
	if( UpdateFlag & UPDATE_BAK_LADDER )
	{
		Result = ReadBinWithMacCard( 0x01, USER_CARD_BAK_RATE_FILE, 132, ESAM_BAK_RATE_FILE, 132, LADDER_BUF_LENGTH, pCardPara->BakLadderBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}		
		
		//备用套起用时间
		Result = ReadBinWithMacCard( 0x01, USER_CARD_BAK_RATE_FILE, 196, ESAM_BAK_RATE_FILE, 196, TIME_BUF_LENGTH, pCardPara->BakLadderTimeBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
	}
	return TRUE;
}

//---------------------------------------------------------------
//函数功能: 带MAC设置参数信息（写到ESAM和EEPROM）
//
//参数:
//		MacMode[in] - 是否带MAC
// 			NO_MAC   -- 不带MAC 直接写EEPROM
// 			WITH_MAC -- 带MAC 先写ESAM 再写EEPROM
//      Type[in] - 设置数据类型 详见eSetInforParaType
//      pBuf[in] - 输入数据
//      Len - 数据长度（不带MAC）
//
//返回值:
//
//备注:  传入数据和ESAM格式对应 大端模式.
//		若 MacMode == WITH_MAC，则将参数写入ESAM，同时写入EEPROM
//		若 MacMode == NO_MAC，则只写入EEPROM
//---------------------------------------------------------------
static BOOL SetSingleParaWithMac( BYTE MacMode, eSetInforParaType Type, BYTE *pBuf, BYTE Len )
{
	WORD 			Result;
	BYTE            File, Offset, ParaType,Buf[sizeof(TSwitchTimePara)];
    TFourByteUnion  TmpData;
    TRealTimer      TmpRealTimer;
	//参数信息文件各个参数的在ESAM文件中的起始位置
	BYTE ParaFileOffset[8] =
	{
		10,     //eSetRateSwitchTime 	两套分时费率切换时间
		196,    //eSetLadderSwitchTime	两套阶梯切换时间
		16,     //eSetAlarmLimit1		报警金额1
		20,     //eSetAlarmLimit2		报警金额2
		24,     //eSetCTCoe				电流变比
		27,     //eSetPTCoe				电压变比
		30,     //eSetMeterNo			表号
		36      //eSetCustomCode		客户编号
	}; 
    
    TmpData.d = 0;
    
	//判断输入参数合法性
	if( (Type > eSetCustomCode) || (Len > 6 ) )
	{
		return FALSE;
	}

	//将参数写到ESAM中
	if( MacMode == WITH_MAC )
	{
		if( Type == eSetLadderSwitchTime )
		{
			File = ESAM_BAK_RATE_FILE;
		}
		else
		{
			File =  ESAM_PARA_FILE;
		}
		Offset = ParaFileOffset[Type];
		
		//将参数写到ESAM中
		Result = WriteEsamWithMac( 0x00, File, Offset, Len + 4, pBuf );
		if( Result == 0x8000 )
		{
			return FALSE;
		}
	}
	
	//数据格式和ESAM对应 ESAM是大端模式 所以需要倒序数据
	lib_InverseData( pBuf, Len );

    switch( Type )
    {
        case eSetRateSwitchTime:
        case eSetLadderSwitchTime:
            //判断切换时间是否合法
            if( CheckSwitchTime( pBuf ) == 0x01 )
            {
                return FALSE;
            }

            //拷贝顺序，年月日时分
			api_ConvertYYMMDDhhmm_TRealTimer( &TmpRealTimer, pBuf );
            if( Type == eSetRateSwitchTime )
            {
                ParaType = 2;
            }
            else
            {
                ParaType = 3;
            }
			
			if( api_ReadAndWriteSwitchTime( WRITE, ParaType, (BYTE *)&TmpRealTimer ) == FALSE )
			{
				return FALSE;
			}
            break;
        case eSetAlarmLimit1:
        case eSetAlarmLimit2:
        case eSetCTCoe:
        case eSetPTCoe:
            if( lib_IsMultiBcd( pBuf, Len ) == FALSE )
            {
                return FALSE;
            }
            if( Len > sizeof(DWORD) )
			{
				return FALSE;
			}
            memcpy( TmpData.b, pBuf, Len );
            TmpData.d = lib_DWBCDToBin( TmpData.d );
            
            ParaType = ParaFileEnum[Type - eSetAlarmLimit1];
			if( api_WritePrePayPara( (ePrePayParaType)ParaType, TmpData.b ) == FALSE )
			{
				return FALSE;
			}
            break;
        case eSetMeterNo:
        case eSetCustomCode: 
            if( lib_IsMultiBcd( pBuf, Len ) == FALSE )
            {
                return FALSE;
            }
            lib_InverseData( pBuf, 6 );
            if( Type == eSetMeterNo )
            {
                ParaType = eMeterMeterNo;
            }
            else
            {
                ParaType = eMeterCustomCode;
            }
			if( api_ProcMeterTypePara( WRITE, ParaType, pBuf ) == FALSE )
			{
				return FALSE;
			} 
            break;
        default:
            return FALSE;
    }
    
    return TRUE;
}

//---------------------------------------------------------------
//函数功能: 更新参数信息文件内容
//
//参数:
//		InBuf[in] -- 输入数据
//
//返回值:
//
//备注: 用于卡和交互终端更改参数信息文件（报警金额1、报警金额2、PT、CT）
//---------------------------------------------------------------
static BOOL SetAllParaWithMac( BYTE *InBuf )
{
    BYTE i, Offset;
    BYTE ParaLen[4] = { 4, 4, 3, 3, };
    eSetInforParaType ParaAddr[4] = { eSetAlarmLimit1, eSetAlarmLimit2, eSetCTCoe, eSetPTCoe };
    TFourByteUnion ParaData;
	WORD Result;
    
    Offset = 0;
	
	Result = WriteEsamWithMac( 0x00, ESAM_PARA_FILE, 16, PARA_BUF_LENGTH, InBuf );
	if( Result == 0x8000 )
	{
		return FALSE;
	}
	
	for( i = 0; i < 4; i++ )
    {
		ParaData.d = 0;
		memcpy( ParaData.b, InBuf + Offset, ParaLen[i] );
		
		if( SetSingleParaWithMac( NO_MAC, ParaAddr[i], ParaData.b, ParaLen[i] ) != TRUE )
		{
			return FALSE;
		}
		Offset += ParaLen[i];
    }
	
	return TRUE;
}

//---------------------------------------------------------------
//函数功能: 带MAC设置电价（写到ESAM和EEPROM）
//
//参数:
//			MacMode[in] - 是否带MAC
// 				NO_MAC   -- 不带MAC 直接写EEPROM
// 				WITH_MAC -- 带MAC 先写ESAM 再写EEPROM
//          Type[in] - 设置电价类型
//              eCurRate             当前套费率电价
//	            eBackRate            备用套费率电价
//              eCurLadder1          当前套第一张阶梯电价
//	            eCurLadder2          当前套第二张阶梯电价
//	            eBackLadder1         备用套第一张阶梯电价
//	            eBackLadder2         备用套第二张阶梯电价
//          pBuf[in] - 电价数据(BCD) + MAC(已经倒好序)
//			Len  - 数据长度（不算MAC）
//返回值:  TRUE/FALSE
//
//备注:
//---------------------------------------------------------------
static BOOL SetPriceWithMac( BYTE MacMode, ePriceType Type, BYTE *pBuf, BYTE Len )
{
    BYTE i;
    BYTE File_Name;
    BYTE File_Offset;
    ePrePayParaType SetPriceNum;
    TFourByteUnion Tmpbuf;
	
	//参数有效性检查
	if( Type > eBackLadder )
	{
		return FALSE;
	}

    switch( Type )
    {
        case eCurRate://当前套费率电价
			if( (Len / 4) > MAX_RATE_PRICE  )
            {
                return FALSE;
            }
            File_Name = ESAM_CUR_RATE_FILE;
            File_Offset = 4;
            SetPriceNum = eCurRateTable; //用于调用EEPROM设置
            break;
        case eBackRate://备用套费率电价
			if( (Len / 4) > MAX_RATE_PRICE  )
            {
                return FALSE;
            }
            File_Name = ESAM_BAK_RATE_FILE;
            File_Offset = 4;
            SetPriceNum = eBackupRateTable; //用于调用EEPROM设置
            break;
        case eCurLadder://当前套阶梯电价
            File_Name = ESAM_CUR_RATE_FILE;
            File_Offset = 52;
            SetPriceNum = eCurLadderTable;
            break;
        case eBackLadder://备用套阶梯电价
            File_Name = ESAM_BAK_RATE_FILE;
            File_Offset = 52;
            SetPriceNum = eBackupLadderTable;
            break;
        default:
            return FALSE;
            break;
    }
    
    //将电价设置到ESAM中----远程设置一类参数使用，698程序暂时用不到这块
	//if( MacMode == WITH_MAC )
	{
		//memcpy( Ic_Comm_RxBuf, pBuf, Len + 4 );
		//if( WriteBinWithMac( ESAM, File_Name, File_Offset, Len + 4 ) == FALSE )
		{
		//	return FALSE;
		}
	}

    //费率电价转换为HEX
    if( (Type == eCurRate)||(Type == eBackRate) )
    {
        for( i=0; i<(Len/4); i++ ) 
        {
            lib_InverseData( &pBuf[4 * i], 4 );
            memcpy( Tmpbuf.b, &pBuf[4 * i], 4 );
            Tmpbuf.d = lib_DWBCDToBin( Tmpbuf.d );
            memcpy( &pBuf[4 * i], Tmpbuf.b, 4 );
        }
    }
    else//阶梯电价转换为HEX
    {
        for( i=0; i<13; i++ ) 
		{
            lib_InverseData( &pBuf[4 * i], 4 );
            memcpy( Tmpbuf.b, &pBuf[4 * i], 4 );
            Tmpbuf.d = lib_DWBCDToBin( Tmpbuf.d );
            memcpy( &pBuf[4 * i], Tmpbuf.b, 4 );
		}
		for( i=0; i<MAX_YEAR_BILL; i++ ) 
		{
            memcpy( Tmpbuf.b, &pBuf[52 + 3 * i], 3 );
			//卡设置阶梯结算日需要判断是否是BCD才能进行转换 否则0xFF会转换错误 后期改@@@@！！！！
			lib_MultipleBBCDToBin(Tmpbuf.b, &pBuf[52 + 3 * i], 0x03 );
		}
	}
    
    //将电价设置到EEPROM中
    if( api_WritePrePayPara( SetPriceNum, pBuf ) == FALSE )
    {
        return FALSE;
    }
    
    return TRUE;
}


//-----------------------------------------------
//函数功能: (卡\交互终端)费率表、阶梯表编程记录
//
//参数: 
//		TYPE  -- 参数更新标志位
// 			UPDATE_CUR_RATE    -- 当前套费率   
//          UPDATE_BAK_RATE    -- 备用套费率   
//          UPDATE_CUR_LADDER  -- 当前套阶梯   
//          UPDATE_BAK_LADDER  -- 备用套阶梯
//
//返回值:		无
//		   
//备注:			
//-----------------------------------------------
static void ProcRateLadderRecord( BYTE ParaUpdateFlag ) //本函数还没完善，需要继续添加
{	
	//注意，下面的12、34不能合并，必须分别写!!!BUY BTC AND HOLD ,YOU'LL BE RICH
	if( (ParaUpdateFlag&UPDATE_CUR_RATE) != 0x00 ) //费率表编程记录--当前套费率电价
	{		
		api_WritePreProgramData( 0,0x00221840 );//40182200——编程前当前套费率电价 
		api_SavePrgOperationRecord( ePRG_TARIFF_TABLE_NO ); //电能表费率参数表编程事件
	}
	
	if( (ParaUpdateFlag&UPDATE_BAK_RATE) != 0x00 ) //费率表编程记录--备用套费率电价
	{
		api_WritePreProgramData( 0,0x00221940 );//40192200——编程前备用套费率电价 
		api_SavePrgOperationRecord( ePRG_TARIFF_TABLE_NO ); //电能表费率参数表编程事件
	}
	
	if( (ParaUpdateFlag&UPDATE_CUR_LADDER) != 0x00 ) //阶梯表编程记录--当前套阶梯电价
	{
		api_WritePreProgramData( 0,0x00221A40 );//401A2200——编程前当前套阶梯电价 
		api_SavePrgOperationRecord( ePRG_LADDER_TABLE_NO ); //电能表阶梯表编程事件
	}
	
	if( (ParaUpdateFlag&UPDATE_BAK_LADDER) != 0x00 ) //阶梯表编程记录--备用套阶梯电价
	{
		api_WritePreProgramData( 0,0x00221B40 );//401B2200——编程前备用套阶梯电价 
		api_SavePrgOperationRecord( ePRG_LADDER_TABLE_NO ); //电能表阶梯表编程事件
	}
}


//---------------------------------------------------------------
//函数功能: 卡设置一类参数
//
//参数:
//		OpType[in] -- 操作类型
// 			CARD_MODE      -- 卡槽插卡设置参数
//          TERMINAL_MODE  -- 交互终端插卡设置参数
//		inCardType[in] -- 卡类型
// 			PRESET_CARD_COMMAND -- 预置卡
// 			USER_CARD_COMMAND -- 用户卡
// 		UpdateFlag  -- 参数更新标志
// 			UPDATE_PARA_FILE   -- 参数信息文件
//          UPDATE_CUR_RATE    -- 当前套费率
//          UPDATE_BAK_RATE    -- 备用套费率
//          UPDATE_CUR_LADDER  -- 当前套阶梯
//          UPDATE_BAK_LADDER  -- 备用套阶梯
// 		InBuf[in]
// 			输入数据
//返回值:
//
//备注: 交互终端设置文件需要整个文件带MAC写入，已在交互终端函数处理
//---------------------------------------------------------------
static BOOL CardSetFirstData( BYTE OpType, BYTE inCardType, BYTE inUpdateFlag, TCardParaUpdate *pCardPara )
{
	BOOL Result;
	BYTE Buf[10];
	TBackupYearBill YearBillBuf;
	
	Result = TRUE;
	
	if( (inCardType != PRESET_CARD_COMMAND)&&(inCardType != USER_CARD_COMMAND) )
	{
		return FALSE;
	}
	
	//带MAC写参数更新标志位
	Result = WriteEsamWithMac( 0x00, ESAM_PARA_FILE, 5, UPDATE_FLAG_LENGTH, pCardPara->FlagBuf );
	if( Result &0x8000 ) 
	{
		return FALSE;
	}
	
	//根据参数更新标志位bit7读写参数+MAC
	if( (inUpdateFlag&UPDATE_PARA_FILE) != 0 )
	{
		//报警金额1、2+电压、电流互感器变比+MAC
		if( SetAllParaWithMac( pCardPara->ParaBuf ) == FALSE )
		{
			return FALSE;
		}
	}
	
	//根据参数更新标志位bit0，更新表内当前套费率电价参数
	if( (inUpdateFlag&UPDATE_CUR_RATE) != 0 )
	{
		//插卡设置费率点击 必须从头开始读取数据 不能从费率电价开始 否则不成功 所以需要单独写ESAM
		Result = WriteEsamWithMac( 0x00, ESAM_CUR_RATE_FILE, 0,  RATE_BUF_LENGTH, pCardPara->CurRateBuf );
		if( Result &0x8000 ) 
		{
			return FALSE;
		}

		//用户卡处理时记录费率，阶梯编程记录 保持与国网一致 如果当前套和备用套同时更新 先记录 后更新-yxk
		if( inCardType == USER_CARD_COMMAND ) //直接插卡和交互终端插卡
		{
			ProcRateLadderRecord( UPDATE_CUR_RATE );
		}

		//从头开始读取 需要去掉4个字节的头部数据才是费率电价
		memmove( pCardPara->CurRateBuf, pCardPara->CurRateBuf + 4, RATE_BUF_LENGTH - 8 );
		
		if( SetPriceWithMac( NO_MAC, eCurRate, pCardPara->CurRateBuf, RATE_BUF_LENGTH - 8 ) == FALSE )
		{
			return FALSE;
		}
	}
	
	//根据参数更新标志位bit1，更新表内备用套费率电价参数
	if( (inUpdateFlag&UPDATE_BAK_RATE) != 0 )
	{
		Result = WriteEsamWithMac( 0x00, ESAM_BAK_RATE_FILE, 0,  RATE_BUF_LENGTH, pCardPara->BakRateBuf );
		if( Result &0x8000 ) 
		{
			return FALSE;
		}

		//用户卡处理时记录费率，阶梯编程记录 保持与国网一致 如果当前套和备用套同时更新 先记录 后更新-yxk
		if( inCardType == USER_CARD_COMMAND ) //直接插卡和交互终端插卡
		{
			ProcRateLadderRecord( UPDATE_BAK_RATE );
		}
		//从头开始读取 需要去掉4个字节的头部数据才是费率电价
		memmove( pCardPara->BakRateBuf, pCardPara->BakRateBuf + 4, RATE_BUF_LENGTH - 8 );

		Result &= SetSingleParaWithMac( WITH_MAC, eSetRateSwitchTime, pCardPara->BakRateTimeBuf, TIME_BUF_LENGTH - 4 ); //数据长度没有MAC
		Result &= SetPriceWithMac( NO_MAC, eBackRate, pCardPara->BakRateBuf, RATE_BUF_LENGTH - 8 );
		if( Result != TRUE )
		{
			return FALSE;
		}
	}
	
	//根据参数更新标志位bit2读卡片阶梯文件1，更新表内当前套阶梯电量+阶梯电价+年结算日参数
	if( (inUpdateFlag&UPDATE_CUR_LADDER) != 0 )
	{		
		Result = WriteEsamWithMac( 0x00, ESAM_CUR_RATE_FILE, 132, LADDER_BUF_LENGTH, pCardPara->CurLadderBuf );
		if( Result &0x8000 ) 
		{
			return FALSE;
		}

		//用户卡处理时记录费率，阶梯编程记录 保持与国网一致 如果当前套和备用套同时更新 先记录 后更新-yxk
		if( inCardType == USER_CARD_COMMAND ) //直接插卡和交互终端插卡
		{
			ProcRateLadderRecord( UPDATE_CUR_LADDER );
		}
		
		Result &= SetPriceWithMac( NO_MAC, eCurLadder, pCardPara->CurLadderBuf, LADDER_BUF_LENGTH - 4 );

		if( Result != TRUE  )
		{
			return FALSE;
		}
		
		if( inCardType == PRESET_CARD_COMMAND )
		{
			//若预置卡更新当前套阶梯，先根据当前置的阶梯数值判断一下当前的阶梯模式，避免产生冻结数据
			InitJudgeBillMode();
			
			memcpy( (void *)&YearBillBuf, pCardPara->CurLadderBuf + 52, 3 * MAX_YEAR_BILL ); //备份卡中当前套年结算日，后面判断如果是预置卡保存到EEPROM
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&YearBillBuf.YearBill );
		}
		else
		{
			//用户卡修改当前套阶梯需要判断阶梯模式切换
			api_SetUpdatePriceFlag( eChangeCurLadderPrice ); 
		}
	}
	
	//根据参数更新标志位bit3读卡片阶梯文件2，更新表内备用套阶梯电量+阶梯电价+年结算日+启动时间参数
	if( (inUpdateFlag&UPDATE_BAK_LADDER) != 0 )
	{					
		Result = WriteEsamWithMac( 0x00, ESAM_BAK_RATE_FILE, 132, LADDER_BUF_LENGTH, pCardPara->BakLadderBuf );
		if( Result &0x8000 ) 
		{
			return FALSE;
		}

		//用户卡处理时记录费率，阶梯编程记录 保持与国网一致 如果当前套和备用套同时更新 先记录 后更新-yxk
		if( inCardType == USER_CARD_COMMAND ) //直接插卡和交互终端插卡
		{
			ProcRateLadderRecord( UPDATE_BAK_LADDER );
		}
		
		Result &= SetPriceWithMac( NO_MAC, eBackLadder, pCardPara->BakLadderBuf, LADDER_BUF_LENGTH - 4 );
		
		//阶梯切换时间
		if( Result == TRUE )
		{
			Result &= SetSingleParaWithMac( WITH_MAC, eSetLadderSwitchTime, pCardPara->BakLadderTimeBuf, TIME_BUF_LENGTH - 4 );
		}
		if( Result != TRUE  )
		{
			return FALSE;
		}
	}
	
	
	//编程记录 只记录插卡操作 交互终端处在其他地方已经记录	
	if( (inCardType == USER_CARD_COMMAND)&&(inUpdateFlag != UPDATE_NO_PARA) ) //用户卡
	{		
		Buf[0] = 0x99;				//标识
		Buf[1] = 0x98;				//私钥
		Buf[2] = inCardType;            //电卡类型
		Buf[3] = inUpdateFlag;      //更新标志
		//使用卡片设置参数时，编程事件记录中的数据标识为 9998＋（卡片格式中）命令码＋更新标志位，操作者代码为卡片序列号的低 4字节。
		lib_ExchangeData( Buf+4, SafeAuPara.CardRst8B+4, 4);	//操作者代码（卡序号后4字节）
        api_SaveProgramRecord( EVENT_START, 0x51, Buf );
        api_SaveProgramRecord( EVENT_END, 0x51, Buf );
	}
	
	
	return TRUE;
}

//-----------------------------------------------
//函数功能:带MAC写用户卡返写文件
//
//参数:		无
//
//返回值:	成功/失败
//
//备注:
//
//-----------------------------------------------
static BYTE WriteCardReturnDataWithMac( void )
{
	BYTE Result;
	BYTE Buf[64];
	BYTE CmdBuf[6];
	BYTE DataBuf[22];
	BYTE Buf_EsamRunFile[ESAM_RUN_FILE_LENTH+4];			//多申请4字节，防处理错误冲内存
	BYTE Buf_Backup[50];                                    //备份运行信息文件
	BYTE CmdBufEsam[9];
	BYTE CmdEsamLen;
	TFourByteUnion td;

	api_ReadEsamData( ESAM_RUN_INFO_FILE, 0, ESAM_RUN_FILE_LENTH, Buf_Backup );

	memset( Buf_EsamRunFile, 0x00, ESAM_RUN_FILE_LENTH );                   //先把buf全清零

	Buf_EsamRunFile[0] = 0x68;
	Buf_EsamRunFile[1] = 0x11;
	Buf_EsamRunFile[2] = 0x00;
	Buf_EsamRunFile[3] = 0x2b;

	if( CurMoneyBag.Money.Remain.ll >= 0 )
	{
		td.d = CurMoneyBag.Money.Remain.ll / 10000;                                               //剩余金额   23-34
		lib_ExchangeData( Buf_EsamRunFile + 23, td.b, 4 );
		lib_ExchangeData( Buf_EsamRunFile + 27, (BYTE *)&CurMoneyBag.Money.BuyTimes, 4 );         //购电次数
		td.d = 0;                                                                                       //透支(BCD)
		memcpy( Buf_EsamRunFile + 31, td.b, 4 );
	}
	else
	{
		td.d = 0;                                                                                       //剩余金额	 23-34
		lib_ExchangeData( Buf_EsamRunFile + 23, td.b, 4 );
		lib_ExchangeData( Buf_EsamRunFile + 27, (BYTE *)&CurMoneyBag.Money.BuyTimes, 4 );         //购电次数
		td.d = ((-1) * CurMoneyBag.Money.Remain.ll) / 10000;                                               //透支(BCD)
		td.d = lib_DWBinToBCD( td.d );
		//memcpy( Buf_EsamRunFile + 31, td.b, 4 );
		lib_ExchangeData(Buf_EsamRunFile + 31, td.b, 4);
	}
	//35-38--保留默认为00
	api_ReadIllegalCardNum( td.b );                                                       //非法卡次数
	td.d = lib_DWBinToBCD( td.d );
	lib_InverseData( td.b, 4 );
	memcpy( Buf_EsamRunFile + 38, td.b, 4 );

	Buf_EsamRunFile[42] = (BYTE)lib_WBinToBCD( RealTimer.wYear );                       //卡处理需要返写时间,远程通讯读07不返写
	Buf_EsamRunFile[43] = lib_BBinToBCD( RealTimer.Mon );                                 //年月日时分
	Buf_EsamRunFile[44] = lib_BBinToBCD( RealTimer.Day );
	Buf_EsamRunFile[45] = lib_BBinToBCD( RealTimer.Hour );
	Buf_EsamRunFile[46] = lib_BBinToBCD( RealTimer.Min );

	td.d = PrePayPara.CTCoe;                                                                //电压变比  11--22需要转化为bcd两个变比都需要
	td.d = lib_DWBinToBCD( td.d );
	lib_ExchangeData( Buf_EsamRunFile + 5, td.b, 3 );
	td.d = PrePayPara.PTCoe;                                                                //电流变比
	td.d = lib_DWBinToBCD( td.d );
	lib_ExchangeData( Buf_EsamRunFile + 8, td.b, 3 );

	memcpy( Buf_EsamRunFile + 11, FPara1->MeterSnPara.MeterNo, 12 );                         //表号客户编号

	Buf_EsamRunFile[ESAM_RUN_FILE_LENTH - 2] = lib_CheckSum( Buf_EsamRunFile + 1, ESAM_RUN_FILE_LENTH - 3 ); //更新CS
	Buf_EsamRunFile[ESAM_RUN_FILE_LENTH - 1] = 0x16;

	if( memcmp( Buf_EsamRunFile, Buf_Backup, ESAM_RUN_FILE_LENTH ) != 0 ) //判断07文件数据是否发生变化,如果变化了才改写07文件
	{
		if( WriteEsamWithMac( 0x03, ESAM_RUN_INFO_FILE, 0, ESAM_RUN_FILE_LENTH, Buf_EsamRunFile ) != TRUE )
		{
			return FALSE;
		}
	}

	Result = ReadRandomCARD( 4, SafeAuPara.CardRand );  //重新取卡片随机数
	if( Result != TRUE )
	{
		return FALSE;
	}

	CmdBuf[0] = 0x83;
	CmdBuf[1] = 0x2c;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x07;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x16;                       //后面的所有数据的长度

	DataBuf[0] = 0x48;
	memcpy( DataBuf + 1, SafeAuPara.CardRst8B, 8 );
	memcpy( DataBuf + 9, SafeAuPara.CardRand, 4 );
	DataBuf[13] = 0x00;                     //P3
	DataBuf[14] = 0x00;                     //P3
	DataBuf[15] = 0x00;                     //LEN
	DataBuf[16] = ESAM_RUN_FILE_LENTH;      //LEN
	DataBuf[17] = 0x04;
	DataBuf[18] = 0xd6;
	DataBuf[19] = 0x85;
	DataBuf[20] = 0x00;                     //P2
	DataBuf[21] = ESAM_RUN_FILE_LENTH + 4;    //LC

	Result = Link_ESAM_TxRx( 6, CmdBuf, 22, DataBuf, Buf );
	if( Result != TRUE )
	{
		return FALSE;
	}

	memmove( Buf, Buf + 2, ESAM_RUN_FILE_LENTH + 4 );
	
	Result = Link_Card_TxRx( 0x04, 0xd6, USER_CARD_RETURN_WRITE_FILE, 0x00, ESAM_RUN_FILE_LENTH+4, WRITE, Buf );

	if( Result != TRUE )
	{
		if( (Err_Sw12[0] == 0x69)&&(Err_Sw12[1] == 0x89) )
		{
			Result = IC_MAC_ERR; //MAC校验错误(6989)
		}
		else
		{
			Result = IC_NOIN_ERR; //操作卡片通讯错误(读写卡片失败);	err35
		}
	}

	return Result;

}

//-----------------------------------------------
//函数功能: 预制卡钱包处理
//
//参数:  		无
//
//返回值:		TRUE/错误类型
//		   
//备注:
//-----------------------------------------------
static WORD ProcPresetCardMoney(void)
{
	WORD Result;
	BYTE MoneyBuf[8];//MoneyBuf	4字节money+4字节MAC	
	
	//带MAC读取卡钱包文件，更新到ESAM钱包--充值金额+MAC
	Result = ReadBinWithMacCard( 0x21, 0x82, 0x00, 0x00, 0x00, 4+4 , MoneyBuf);
	if( Result != TRUE )
	{
		return IC_NOIN_ERR;
	}
	
	//读回来只有金额和MAC，所以不判断次数了--新发卡系统发的预置卡购电次数只能为0						
	Result = WriteEsamWithMac( 0x02, 0x00, 0x00, 4+4, MoneyBuf);	
	if( Result !=1 ) 
	{
		return IC_ESAM_RDWR_ERR;		
	}
	
	//清购电记录数据中透支金额，购电次数,购电前剩余金额
	memset(FourByteNewBuyMoneyBag.Remain.b, 0x00, sizeof(FourByteNewBuyMoneyBag));	    //清购电记录数据中透支金额，购电次数,购电前剩余金额
    memset(FourByteOldBuyMoneyBag.Remain.b, 0x00, sizeof(FourByteOldBuyMoneyBag));	        //清购电记录数据中透支金额，购电次数,购电前剩余金额
	//================RAM中购电剩余金额预置为MoneyBuf，计算crc校验								
	lib_ExchangeData(FourByteNewBuyMoneyBag.Remain.b,MoneyBuf,4);
	 
	Result = api_UpdataRemainMoney( eInitMoneyMode, FourByteNewBuyMoneyBag.Remain.d, 0 );	

	return OK_CARD_PROC;
}

//-----------------------------------------------
//函数功能: 预置卡处理
//
//参数:  		无
//
//返回值:		TRUE/错误类型
//		   
//备注:
//-----------------------------------------------
static WORD Proc_PresetCard(void)
{
	WORD Result;
	TCardParaUpdate CardPara;

	if( Judge_Ladder_Data( 0xff ) == FALSE )
	{
		return IC_FORMAT_ERR;
	}
	
	//预置卡钱包处理	
	Result = ProcPresetCardMoney();
	if( Result != OK_CARD_PROC ) 
	{
		return Result;
	}
	
	//带MAC读取相关参数到RAM备份数据中
	Result = ReadDataFromCardWithMac( &CardPara );
	if( Result != TRUE ) 
	{
		return IC_NOIN_ERR;
	}
	
	//带MAC更新相关参数
	Result = CardSetFirstData( CARD_MODE, PRESET_CARD_COMMAND, UpdateFlag, &CardPara );
	if( Result != TRUE )
	{
		return IC_ESAM_RDWR_ERR;	//操作ESAM错
	}	

	return OK_CARD_PROC;
}

 //-----------------------------------------------
 //函数功能: 用户卡钱包处理
 //  
 //参数:  		UserCardMoney[in] 用户卡购电信息 
 //             InMoneyBuf[in]    购电金额(4)+购电次数(4)+MAC(4)
 //
 //返回值:		TRUE/错误类型
 //		   
 //备注:   
 //-----------------------------------------------
static WORD Proc_UserCardMoney(TUserCardMoney	*UserCardMoney, BYTE *InMoneyBuf)
{
	TFourByteUnion Temp32;
	BYTE Buf[20],UserCardSerialBit;
	WORD Result;

	UserCardSerialBit = 0;
	if ( memcmp( SafeAuPara.CardRst8B, CardInfo.SerialNo, 8) != 0 )//判断卡序列号是否相等
	{
        UserCardSerialBit = 1;
	}
	
	//================ 表号判断 ===============
	if ( memcmp( FPara1->MeterSnPara.MeterNo, UserCardMoney->MeterNo, 6) != 0 ) 
	{
		return IC_ID_ERR;	//表号不相等
	}	
	
	//========== 卡片序号及开户判断 ===========

	if( (UserCardMoney->UserCardType!=OPEN_CARD) 
		&& (UserCardMoney->UserCardType!=BUY_CARD) 
		&& (UserCardMoney->UserCardType!=LOST_CARD) ) 
	{
		return IC_TYPE_ERR;			//卡类型错误
	}

	if( (CardInfo.LocalFlag == eOpenAccount) || (CardInfo.RemoteFlag==eOpenAccount) )	//本地或远程已开户
	{
		Result = memcmp( FPara1->MeterSnPara.CustomCode, UserCardMoney->CustomCode, 6 );
		if( Result != 0 ) 			//已开户的表，客户编号不相等
		{
			return IC_HUHAO_ERR;	//客户编号错
		}
				
		if( ( UserCardMoney->UserCardType == BUY_CARD ) && ( UserCardSerialBit == 1 ) ) 
		{
			return IC_SERIAL_ERR;	//购电卡卡序号错
		}
	}
	else							//未开户
	{
		if( UserCardMoney->UserCardType == BUY_CARD )
		{
			return IC_NOOPEN_ERR; 	//购电卡插入未开户表
		}
		else if( UserCardMoney->UserCardType == LOST_CARD )
		{
			return IC_NOOPEN2_ERR; 	//补卡插入未开户表
		}
	}
	
	if( FourByteNewBuyMoneyBag.BuyTimes > 1 )
	{
		if( UserCardMoney->UserCardType == OPEN_CARD ) 
		{
			return IC_TIMES_ERR;	//购电次数错(开户卡购电次数只能是0和1)
		}
	}
	
	//======== 判断购电次数和临时充值 ==========	
	Result = ReadEsamMoneyBag( Buf );                                 //读钱包购电次数
	if( Result != TRUE ) 
	{
		return IC_ESAM_RDWR_ERR;
	}
	FourByteOldBuyMoneyBag.Remain.l = (CurMoneyBag.Money.Remain.ll/10000);						//剩余、透支HEX放到备份区
	lib_ExchangeData( Temp32.b, Buf+4, 4 );										//Temp32暂存esam购电次数，购电次数以esam为准
	
	//卡片购电次数小于表内值 
	if( FourByteNewBuyMoneyBag.BuyTimes < Temp32.d )
	{	//南网费控电能表系列技术标准答疑(二) 补卡次数小于表内购电次数 保存卡序列号	
		if(( UserCardMoney->UserCardType == OPEN_CARD ) || ( UserCardMoney->UserCardType == LOST_CARD )) 
		{
			UpdateUsercardSerial( UserCardMoney->UserCardType );    //更新本地开户标志及卡序列号
		}
		UserCardMoney->BuyTimesFlag = 0xff;							//卡片购电次数小于表内值 	
	}
	else
	{
		Temp32.d = FourByteNewBuyMoneyBag.BuyTimes-Temp32.d;			//卡片购电次数 - 表内值
		
		if( Temp32.d == 0 ) 					//购电次数相等
		{
			UserCardMoney->BuyTimesFlag = 0;	//购电次数相等
			if( Judge_Ladder_Data(UserCardMoney->UserCardType) == FALSE )
			{
				return IC_FORMAT_ERR;
			}
			
		}
		else if( Temp32.d == 1 ) 				//购电次数差1
		{									
			UserCardMoney->BuyTimesFlag = 1;	//购电次数差 1
			
			//开户卡，购电次数大1，返写信息不为空时重新返写并提示错误
			//南网费控电能表系列技术标准答疑(二) 所有卡如果次数大1 返写不为空 均返写报错
			if( UserCardMoney->ReturnFileStatus == 1 )
			{
				return OK_CARD_PROC;
			}
   			
			// 剩余+充值			
			if( (FourByteOldBuyMoneyBag.Remain.l+FourByteNewBuyMoneyBag.Remain.l) > 99999999L )//相加后溢出了(超过999999.99元)
			{
				return IC_OVERMAX_ERR;
			}
			//判断囤积金额
			if( LimitMoney.RegrateLimit.d )
			{
				if( (FourByteOldBuyMoneyBag.Remain.l+FourByteNewBuyMoneyBag.Remain.l) > (long)LimitMoney.RegrateLimit.d ) //L
				{
					return IC_OVERMAX_ERR;
				}
			}
			if( Judge_Ladder_Data(UserCardMoney->UserCardType) == FALSE )
			{
				return IC_FORMAT_ERR;
			}	
		}
		else 									//购电次数差>=2
		{
			UserCardMoney->BuyTimesFlag = 2;	
		}
	}
	
	//----------------------------------------------------------------------------
   	if( UserCardMoney->BuyTimesFlag == 1 )
	{
		if( ((UserCardMoney->UserCardType==OPEN_CARD) && (UserCardMoney->ReturnFileStatus == 0)) 
			|| (UserCardMoney->UserCardType==BUY_CARD)
			|| (UserCardMoney->UserCardType==LOST_CARD) )
		{
			//钱包文件充值
			Result = WriteEsamWithMac( 0x01, 0x00, 0x00, 8+4, InMoneyBuf);	//更新钱包金额--充值金额+MAC				

			if(Result != TRUE) return IC_MAC_ERR;
			
			api_UpdataRemainMoney( eChargeMoneyMode, FourByteNewBuyMoneyBag.Remain.d, FourByteNewBuyMoneyBag.BuyTimes );
			
		}
	}   
	
	//开户卡：卡内购电次数小于表内购电次数  前面处理了 这种情况肯定会有户号，因此不用再存储户号
	//开户卡：卡内购电次数比表内购电次数大1，且返写文件为空
	//开户卡：卡内购电次数和表内购电次数相等，且返写文件为空
	//条文解释：对于未开户或已远程开户的电能表，在插开户卡时，电能表需要判断开户卡的返写信息
	//文件是否为空，只有为空时，才允许开户卡开户。 
	if( UserCardMoney->UserCardType == OPEN_CARD )//开户卡 到这里就和次数无关了，因为开户卡次数限制了为0或1
	{
		if(UserCardMoney->ReturnFileStatus == 0)	//返写为空的开户卡或者补卡,保存卡序号及开户标志
		{
			memcpy(Buf, UserCardMoney->CustomCode, 6+4);//复制RAM备份区数据
			Result = WriteEsamWithMac(0x00,0x8F,0x06,0x0A,Buf);//带MAC写入ESAM相关参数
			if( Result&0x8000 ) 
			{
				return IC_ESAM_RDWR_ERR;	
			}
			//保存客户编号、更新卡序列号和开户标志	
			api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, MeterSnPara.CustomCode ),GET_STRUCT_MEM_LEN( TFPara1, MeterSnPara.CustomCode ), Buf );//复制RAM备份区数据		
			UpdateUsercardSerial( UserCardMoney->UserCardType );
		}
	}	
	
	//补卡：卡内购电次数和表内相等或者大1，且返写文件为空
	//开户卡和无返写的补卡，保存卡序号
	if( UserCardMoney->UserCardType == LOST_CARD )//补卡
	{
		if( ( !UserCardMoney->ReturnFileStatus ) && ( UserCardMoney->BuyTimesFlag<2 ) ) 
		{
			UpdateUsercardSerial( UserCardMoney->UserCardType );
		}
	}
		
	if( ( UserCardMoney->UserCardType==OPEN_CARD )||( UserCardMoney->UserCardType==LOST_CARD ) )
	{
		//次数相等，返写不为空，卡序号不等，报错.考虑到有连续补两次卡的情况，只识别最新一次补的卡 所以在返写不为空的时候判断卡序列号。
		if( (UserCardMoney->ReturnFileStatus!=0) 
			&& (UserCardMoney->BuyTimesFlag==0) 
			&& (UserCardSerialBit==1) ) 
		{
			return IC_SERIAL_ERR;	
		}
	}
	
	return OK_CARD_PROC;
}

//-----------------------------------------------
//函数功能: 用户卡处理
//  
//参数:  		
//			Type[in]: 01开户, 02充值, 03补卡
//          InBuf[in]：购电金额+购电次数+MAC1, 客户编号+MAC2, 表号(本地插卡)
//
//返回值:		TRUE/错误类型
//		   
//备注:
//-----------------------------------------------
static WORD Proc_UserCard( BYTE *InBuf )
{
	WORD Result;
	BYTE InMoneyBuf[12];
	BYTE Buf[8];
	TCardParaUpdate ParaBuf;
	TUserCardMoney	UserCardMoney;
	
	UserCardMoney.UserCardType = InBuf[0x2a];
	memcpy(UserCardMoney.MeterNo,InBuf+0x1e,6);
		
	//=========== CARD 电表外部认证 ===========
	Result = CardExtAuth();
	if( Result != TRUE ) 
	{
		return Result;
	}	
	
	//=============== 数据准备 =============
	//============= 返写文件判断 ===========
	Link_Card_TxRx( 0x00, 0xb0, USER_CARD_RETURN_WRITE_FILE, 0x00, 8 , READ, Buf );
	if( (Buf[0] == 0x68) || (Buf[5] != 0) || (Buf[6] != 0) || (Buf[7] != 0) ) //电流互感器变比不全为0
	{
		UserCardMoney.ReturnFileStatus = 1;	//返写文件不为空
	}
	else
	{
		UserCardMoney.ReturnFileStatus = 0;	//返写文件为空
	}

	//============带MAC读取卡中钱包文件 ===========
	ReadBinWithMacCard ( 0x31, 0x82, 0x00, 0x00, 0x00, 8+4 ,InMoneyBuf);//读取购电金额(4)+购电次数(4)+MAC(4)					
	lib_ExchangeData( FourByteNewBuyMoneyBag.Remain.b, InMoneyBuf, 4 );							//卡内购电金额HEX
	lib_ExchangeData( (BYTE *)&FourByteNewBuyMoneyBag.BuyTimes, InMoneyBuf+4, 4 );					//卡内购电次数HEX			
	ReadBinWithMacCard( 0x01, USER_CARD_PARA_FILE, 0x24, 0x8F, 0x06, 6+4 ,(BYTE *)&UserCardMoney.CustomCode);//读取户号(6)+MAC(4)	

	//用户卡钱包处理
	Result = Proc_UserCardMoney(&UserCardMoney,InMoneyBuf);
	if( Result != OK_CARD_PROC ) 
	{
		return Result;
	}
		
	//开户卡，返写不为空，购电次数大1，只返写并报错 
	//南网费控电能表系列技术标准答疑(二) 所有卡如果次数大1 返写不为空 均返写报错
	if( (UserCardMoney.ReturnFileStatus==1) && (UserCardMoney.BuyTimesFlag==1) )
	{
	}
	else
	{	
		//购电卡和补卡，只允许更新备用套;
		if( UserCardMoney.UserCardType != OPEN_CARD )
		{
			UpdateFlag &= 0x7A;	
		} 	
		
		//(返写文件为空，且购电次数相等)  或者大1时可更新参数
		if( ((!UserCardMoney.ReturnFileStatus)&&(UserCardMoney.BuyTimesFlag==0)) || (UserCardMoney.BuyTimesFlag==1) ) 
		{		
			//带MAC读取相关参数
			Result = ReadDataFromCardWithMac(&ParaBuf);
			if( Result != TRUE ) 
			{
				return IC_NOIN_ERR;
			}
	
			//更新ESAM和EEPROM参数
			Result = CardSetFirstData( CARD_MODE, USER_CARD_COMMAND, UpdateFlag, &ParaBuf );
			if( Result != TRUE )
			{
				return( IC_ESAM_RDWR_ERR );	//操作ESAM错
			}	
		}
	}
	
	//================ 返写卡片 =============
	Result =WriteCardReturnDataWithMac();
	if( Result != TRUE ) 
	{
		return IC_NOIN_ERR;
	}
		
	if(UserCardMoney.BuyTimesFlag == 2) 
	{
		return IC_TIMES_ERR;			//卡中购电次数比表内大2以上，返写卡片，提示“购电次数错”
	}
	
	if((UserCardMoney.BuyTimesFlag == 1) && (UserCardMoney.ReturnFileStatus)) 
	{
		return IC_NONULL_ERR;			//购电次数差1，返写不为空，返回返写不为空错误
	}
	
	return OK_CARD_PROC;
}


//-----------------------------------------------
//函数功能: 卡片处理
//  
//参数:  		无
//
//返回值:		TRUE/错误类型
//		   
//备注:
//-----------------------------------------------
static WORD Card_Esam_Proc( void )
{
	BYTE i, Length;
	BYTE Buf[46];
	WORD Result;
	
	CLEAR_WATCH_DOG;
	
	Ic_Err_Reg  = 0x00;
	UpdateFlag = 0x00;	
	memset( SafeAuPara.CardRst8B, 0xff, 8 );//如费控电能表不能正常读出 CPU 卡序列号，记录中的卡序列号应记为全FF---规范 7.13 c) 1)	

	//=========== 卡片复位 ================
	for(i=0; i<3; i++)
	{
		//读卡复位同时复位ESAM和SPI
		Result = Link_CardReset(Buf); 
		if( Result == TRUE ) 
		{
			break;
		}	
		api_Delayms(100);
		CLEAR_WATCH_DOG;
	}

	if( i == 0x03 )  
	{
		return IC_IC_RST_ERR; 	//复位CARD失败
	}
	
	//=========== 复位ESAM ===============
	//为了减少复位ESAM的次数 只有第一遍卡复位成功后才能复位ESAM 防止插入非法卡(铁片等) 导致ESAM复位
	api_ResetEsamSpi(); 

	//=========== 卡片复位 ================
	//因为CARD电源来自ESAM电源 而复位ESAM是通过将ESAM掉电实现的 所以卡也会掉电 所以复位完ESAM之后再复位卡一次
	for(i=0; i<3; i++)
	{
		Result = Link_CardReset(Buf); 
		if( Result == TRUE ) 
		{
			break;
		}	
		api_Delayms(100);
		CLEAR_WATCH_DOG;
	}

	if( i == 0x03 )  
	{
		return IC_IC_RST_ERR; 	//复位CARD失败
	}
	memcpy( SafeAuPara.CardRst8B, Buf+5, 8);

	//此处要添加结束编程1次记录的代码--songchen
	api_SaveProgramRecord( EVENT_END, 0x51, NULL );
	
	//=========== 选择文件 ================
	Result = SelectFile( 0x3f, 0x00 );
	if(Result != TRUE)   
	{
		return IC_TYPE_ERR;
	}	
	Result = SelectFile(0xdf, 0x01 );
	if(Result != TRUE)   
	{
		return IC_TYPE_ERR;
	}			
	//=========== 判断卡片类型 ================
	Result = ReadBinFile( 0x81, 0x00, 0x04, Buf );
	if(Result != TRUE) 
	{	
		if( (Err_Sw12[0] == 0x6b)&&(Err_Sw12[1] == 00) )
		{
			return IC_NOFX_ERR;         //未发行的卡片
		}
		else
		{
			return IC_NOIN_ERR;         //卡片操作错误
		}
	}	
	//这么判断其实不对，但是返回的错误恰好从0x02开始，所以没出错

	CardType = Buf[1];

	if( CardType == USER_CARD_COMMAND )
	{
		//如果采用此行，异常插卡错误信息字记09--卡片未授权（公钥下插用户卡，私钥下插预置卡），否则记05--身份认证失败
		if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == TRUE)
		{
			return IC_KEY_ERR;
		}
		Length = UserCardLength;	 		//用户卡
		//钱包扣减及更新运行信息中的剩余金额
		MeteringUpdateMoneybag();
	}
	else if( CardType == PRESET_CARD_COMMAND )
	{
		//如果采用此行，异常插卡错误信息字记09--卡片未授权（公钥下插用户卡，私钥下插预置卡），否则记05--身份认证失败
		if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == FALSE)
		{
			return IC_KEY_ERR;
		}
		Length = PresetCardLength;			//预置卡
	}
	else 
	{
		return IC_TYPE_ERR;
	}
		
	api_Delayms(5);

	//=========== 系统内部认证=================
	Result = InterAuth();
	if( Result != TRUE ) //这么判断其实不对，但是返回的错误恰好从0x02开始，所以没出错
	{
		return Result;
	}
	
	Result = api_ReadEsamRandom( 4, Buf);		//从ESAM取4字节随机数，用于带mac读写钱包文件
	if(Result != TRUE) 
	{	
		return IC_ESAM_RDWR_ERR;
	}
	//=========== 判断指令文件格式=============
	Result = ReadBinFile(0x81, 0x00, Length+6, Buf );
	if( Result != TRUE )  
	{
		return IC_NOIN_ERR; 
	}

	Result = lib_CheckSum( Buf+1, Length+3);
	if( (Buf[0]!=0x68) || (Buf[Length+4]!=Result) || (Buf[Length+5]!=0x16) ) 
	{
		return IC_FORMAT_ERR;   	//卡片文件格式不合法
	}
		
	UpdateFlag = Buf[5];			//保存参数更新标志位				
	if( UpdateFlag == 0xff ) 
	{
		UpdateFlag = 0;				//保证新表兼容旧卡
	}
	//=============== 卡片处理=================	
	if( CardType == PRESET_CARD_COMMAND )//--------------预置卡
	{
		Result = Proc_PresetCard();
		if( Result != OK_CARD_PROC ) 
		{
			return Result;
		}
	}//-----------用户卡	
	else if( CardType == USER_CARD_COMMAND )
	{					
		Result = Proc_UserCard( Buf );
		if( Result != OK_CARD_PROC ) 
		{
			return Result;
		}
	}
	else 
	{
		return IC_TYPE_ERR;
	}
	
	return OK_CARD_PROC;
}

//-----------------------------------------------
//函数功能: 新卡片插入
//  
//参数:  		无
//
//返回值:		TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
static WORD SearchCard(void)
{
	WORD Result;
	static BYTE bDelayms = 0;
	Result = FALSE;

	if( CARD_IN_SLOT ) 
	{
		if( bDelayms < 10 )
		{
			bDelayms++;
		}
	}
	else
	{
		bDelayms = 0;
	}
	
	if( bDelayms >2 ) 
	{
		if( (CardCheck == 0) )
		{
			//有新卡片插入
			CardCheck = 1;	
			Result = TRUE;		
		}
	}
	else
	{
		CardCheck = 0;	
	}
	return Result;
}
//-----------------------------------------------
//函数功能: 获取插卡状态字
//
//参数: 	无
//                   
//返回值:   01 成功   02 失败 
//
//备注:   
//-----------------------------------------------
void api_GetCardInsertStatus(BYTE *OutBuf)
{
	WORD InsertCardStaBuf;
	InsertCardStaBuf = InsertCardSta;
	InsertCardSta = 0;	//插卡状态字要求读取和上电后置未知 Bit1Bit0:00 未知，01成功，10失败
	lib_ExchangeData(OutBuf,(BYTE *)&InsertCardStaBuf,2);
}	
//-----------------------------------------------
//函数功能: 插卡处理程序
//	
//参数:		无
//
//返回值:		无
//		   
//备注:
//-----------------------------------------------
void Card_Proc( void )
{
	WORD Result,twU;
	BYTE i, OptBuf[4];
    TFourByteUnion dwMoney;
	
	//掉电不处理,程序直接返回
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )
	{
		return;							 
	}
	
	Result = SearchCard();
	if(Result != TRUE)
	{
		return;
	}
	CardType = 0xFF;
	dwMoney.l = (CurMoneyBag.Money.Remain.ll / 10000);
	
	//供异常插卡记录使用，先清零，在操作ESAM或CARD链路层函数中保存命令头，若插卡失败，用此保存数据记录异常插卡命令头
	memset( CommandHead, 0x00, sizeof(CommandHead) );
	
	//电压是否大于0.75Un	
	twU = wStandardVoltageConst / 10 * 75 / 10;//0.75Un
	for(i=0;i<MAX_PHASE;i++)
	{
		api_GetRemoteData(PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, 2, (BYTE*)&Result);
		if( Result > twU )
		{
			break;
		}
	}
	if( i < MAX_PHASE )
	{	
		api_ReadCardDisplay( 0x00, Ic_Err_Reg, dwMoney.l );
		Ic_Err_Reg = Card_Esam_Proc();		//卡片处理
		api_CardCloseRelay( dwMoney.l );			//插卡闭合继电器
	}	
	else
	{ 
		Ic_Err_Reg = IC_VLOW_ERR;			//电压过低
	}
	
	if( Ic_Err_Reg == OK_CARD_PROC )
	{
		if( CardType == PRESET_CARD_COMMAND )
		{	
			lib_ExchangeData(OptBuf,SafeAuPara.CardRst8B+4,4); //电表清零,保存清零记录,卡序列号后四位为操作者代码
			api_SetClearFlag( eCLEAR_MONEY, 0 ); //设置清零标志
			api_SavePrgOperationRecord( ePRG_CLEAR_METER_NO ); // 清零记录
		}
		else
		{	
		}
		
		InsertCardSta = 0x01;			//插卡成功
	}
	else
	{
		InsertCardSta = 0x02;			//插卡失败
		
		Card_Err_Record();				//卡片处理结束到卡片错误记录中间不能操作卡片和ESAM
		
		if(Ic_Err_Reg < 0x04) Ic_Err_Reg = 0x31;
		else if(Ic_Err_Reg < 0x11) Ic_Err_Reg = 0x32;
		else if(Ic_Err_Reg < 0x14) Ic_Err_Reg = 0x33;
		else if(Ic_Err_Reg < 0x19) Ic_Err_Reg = 0x34;
		else if(Ic_Err_Reg < 0x21) Ic_Err_Reg = 0x35;
		else Ic_Err_Reg = 0x36;
	}
	
	api_ReadCardDisplay( CardType, Ic_Err_Reg, dwMoney.l );
	api_CloseCard();//卡片操作完，关闭卡
	#if( MAX_PHASE == 1 )
	CLOSE_CARD_RESET;
	#endif//#if( MAX_PHASE == 1 )
}

//-----------------------------------------------
//函数功能: 清卡信息
//
//参数:  		无
//
//返回值:		无
//
//备注:
//-----------------------------------------------
void ClrCardInfo( void )
{
	BYTE Buf[16]; //实际只用到8个字节

	memset( Buf, 0, 16 );       //初始化卡序号
	api_WritePrePayPara( eLocalAccountFlag, Buf );
	api_WritePrePayPara( eSerialNo, Buf );
	api_WritePrePayPara( eRemoteAccountFlag, Buf );
	//清非法插卡次数
	api_ClrContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwIllegalCardNum ), sizeof(DWORD) ); 
}

//-----------------------------------------------
//函数功能:     ESAM卡验证
//
//参数:		无
//
//返回值:	    成功/失败
//
//备注:
//
//-----------------------------------------------
BYTE api_TestEsamCard( void )
{
	BYTE Buf[64];
	BYTE CmdBuf[6];
	BYTE DataBuf[22];
	BYTE Buf_EsamRunFile[50];
	BYTE Buf_Backup[50];                                    //备份运行信息文件
	BYTE CmdBufEsam[9];
	BYTE CmdEsamLen;
	TFourByteUnion td;

	api_ReadEsamData( ESAM_RUN_INFO_FILE, 0, ESAM_RUN_FILE_LENTH, Buf_Backup );

	memset( Buf_EsamRunFile, 0x00, ESAM_RUN_FILE_LENTH );                   //先把buf全清零

	Buf_EsamRunFile[0] = 0x68;
	Buf_EsamRunFile[1] = 0x11;
	Buf_EsamRunFile[2] = 0x00;
	Buf_EsamRunFile[3] = 0x2b;

	if( CurMoneyBag.Money.Remain.ll >= 0 )
	{
		td.d = CurMoneyBag.Money.Remain.ll / 10000;                                               //剩余金额   23-34
		lib_ExchangeData( Buf_EsamRunFile + 23, td.b, 4 );
		lib_ExchangeData( Buf_EsamRunFile + 27, (BYTE *)&CurMoneyBag.Money.BuyTimes, 4 );         //购电次数
		td.d = 0;                                                                                       //透支(BCD)
		td.d = lib_DWBinToBCD( td.d );
		memcpy( Buf_EsamRunFile + 31, td.b, 4 );
	}
	else
	{
		td.d = 0;                                                                                       //剩余金额	 23-34
		lib_ExchangeData( Buf_EsamRunFile + 23, td.b, 4 );
		lib_ExchangeData( Buf_EsamRunFile + 27, (BYTE *)&CurMoneyBag.Money.BuyTimes, 4 );         //购电次数
		td.d = ((-1) * CurMoneyBag.Money.Remain.ll) / 10000;                                               //透支(BCD)
		td.d = lib_DWBinToBCD( td.d );
		memcpy( Buf_EsamRunFile + 31, td.b, 4 );
	}
	//35-38--保留默认为00
	api_ReadIllegalCardNum( td.b );                                                       //非法卡次数
	td.d = lib_DWBinToBCD( td.d );
	lib_InverseData( td.b, 4 );
	memcpy( Buf_EsamRunFile + 38, td.b, 4 );

	Buf_EsamRunFile[42] = (BYTE)lib_WBinToBCD( RealTimer.wYear );                       //卡处理需要返写时间,远程通讯读07不返写
	Buf_EsamRunFile[43] = lib_WBinToBCD( RealTimer.Mon );                                 //年月日时分
	Buf_EsamRunFile[44] = lib_WBinToBCD( RealTimer.Day );
	Buf_EsamRunFile[45] = lib_WBinToBCD( RealTimer.Hour );
	Buf_EsamRunFile[46] = lib_WBinToBCD( RealTimer.Min );

	td.d = PrePayPara.CTCoe;                                                                //电压变比  11--22需要转化为bcd两个变比都需要
	td.d = lib_DWBinToBCD( td.d );
	lib_ExchangeData( Buf_EsamRunFile + 5, td.b, 3 );
	td.d = PrePayPara.PTCoe;                                                                //电流变比
	td.d = lib_DWBinToBCD( td.d );
	lib_ExchangeData( Buf_EsamRunFile + 8, td.b, 3 );

	memcpy( Buf_EsamRunFile + 11, FPara1->MeterSnPara.MeterNo, 12 );                         //表号客户编号

	Buf_EsamRunFile[ESAM_RUN_FILE_LENTH - 2] = lib_CheckSum( Buf_EsamRunFile + 1, ESAM_RUN_FILE_LENTH - 3 ); //更新CS
	Buf_EsamRunFile[ESAM_RUN_FILE_LENTH - 1] = 0x16;

	if( WriteEsamWithMac( 0x03, ESAM_RUN_INFO_FILE, 0, ESAM_RUN_FILE_LENTH, Buf_EsamRunFile ) != TRUE )
	{
		return FALSE;
	}

	return TRUE;
}

#endif//#if( PREPAY_MODE == PREPAY_LOCAL )

