//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.8.23
//描述		预付费 内部头文件
//修改记录	

//
//----------------------------------------------------------------------
#ifndef __PREPAY_H
#define __PREPAY_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------						
#define MAX_PREPAY_LEN					2000

typedef struct TSafeAuPara_t
{
	BYTE	EsamRst8B[8];				//ESAM序列号
	BYTE	EsamRand[8];				//ESAM随机数
	BYTE    CardRst8B[8];               //卡序号 分散因子	
	BYTE	CardRand[8];				//卡随机数
	BYTE    PcRst8B[8];				    //PC远程下发的 分散因子 8字节
	BYTE    PcRand[8];				    //PC远程下发的 随机数1  8字节
}TSafeAuPara;	//安全认证相关参数


#if( PREPAY_MODE == PREPAY_LOCAL)
#define DEFAULT_UPDATE_MONEY_TIME		60				//ESAM同步时间限制

//写一类参数是否带MAC
#define WITH_MAC						0x55
#define NO_MAC							0x00

//写一类参数方式
#define CARD_MODE						0x55
#define TERMINAL_MODE					0xA0

//一类参数更新标志位
#define UPDATE_NO_PARA					0x00			//不更新任何参数
#define UPDATE_PARA_FILE				0x80			//参数信息文件
#define UPDATE_CUR_RATE					0x01			//当前套费率
#define UPDATE_BAK_RATE					0x02			//备用套费率
#define UPDATE_CUR_LADDER				0x04			//当前套阶梯
#define UPDATE_BAK_LADDER				0x08			//备用套阶梯

//用户卡类型
#define OPEN_CARD						0x01			//开户卡
#define BUY_CARD						0x02			//购电卡
#define LOST_CARD						0x03			//补卡
#define CLOSE_CARD						0x00			//合闸复电卡

#define		ESAM_RUN_FILE_LENTH			49			//ESAM运行信息文件长度，用户卡返写信息文件长度
#define		ESAM_RUN_INFO_FILE			0x07		//ESAM运行信息文件


#define		UserCardLength		39	//用户卡长度
#define		PresetCardLength	26	//预置卡长度


#define		OK_CARD_PROC		0xff	//卡片操作成功 
#define		IC_VLOW_ERR			0x01	//表计电压过低176V(80%Un) err31
#define		IC_ESAM_RDWR_ERR	0x02	//1操作ESAM错误;	err31
#define		IC_ESAM_RST_ERR		0x03	//1ESAM复位错误	err31
#define		IC_IC_RST_ERR		0x04	//卡片复位错误:反插卡或插铁片	err32
#define		IC_AUTH_ERR			0x05	//1身份认证错误(通信成功但密文不匹配)	err32
#define		IC_EXAUTH_ERR		0x06	//1外部身份认证错误(通信成功但认证不通过);	err32
#define		IC_NOFX_ERR			0x07	//未发行的卡片(读卡片时返回6B00);	err32
#define		IC_TYPE_ERR			0x08	//卡类型错误	err32
#define		IC_KEY_ERR			0x09	//卡片操作未授权：密钥状态不为公钥是插参数预制卡	err32
#define		IC_MAC_ERR			0x10	//1MAC校验错误(6988);	err32
#define		IC_ID_ERR			0x11	//表号不一致	err33
#define		IC_HUHAO_ERR		0x12	//客户编号不一致	err33
#define		IC_SERIAL_ERR		0x13	//卡序号不一致	err33
#define		IC_FORMAT_ERR		0x14	//卡片文件格式不合法	err34
#define		IC_NOOPEN_ERR		0x15	//购电卡插入未开户电表	err34
#define		IC_NOOPEN2_ERR		0x16	//补卡插入未开户电表	err34
#define		IC_TIMES_ERR		0x17	//购电次数错误	err34
#define		IC_NONULL_ERR		0x18	//用户卡返写信息文件不为空	err34
#define		IC_NOIN_ERR			0x19	//1操作卡片通讯错误(读写卡片失败);	err35
#define		IC_PREIC_ERR		0x20	//提前拔卡	err35
#define		IC_OVERMAX_ERR		0x21	//剩余金额超囤积	err36

#define UPDATE_FLAG_LENGTH			(1+4)//参数更新标志位缓存 1个字节数据+4个字节MAC
#define PARA_BUF_LENGTH				(14+4)
#define RATE_BUF_LENGTH				(4+32*4+4)//头+费率+MAC
#define LADDER_BUF_LENGTH			(64+4)
#define TIME_BUF_LENGTH				(5+4)

#define		MAX_RATE_PRICE					32			//允许设置最大费率电价数

#define		ESAM_PARA_FILE				0x84		//ESAM参数信息文件
#define		ESAM_CUR_RATE_FILE			0x85		//ESAM当前套电价文件
#define		ESAM_BAK_RATE_FILE			0x86		//ESAM备用套电价文件

#define		PRESET_CARD_PARA_FILE		0X81		//预置卡参数信息文件
#define		PRESET_CARD_MONEY_FILE		0X85		//预置卡购电信息文件
#define		PRESET_CARD_CUR_RATE_FILE	0X83		//预置卡当前套电价文件
#define		PRESET_CARD_BAK_RATE_FILE	0X84		//预置卡备用套电价文件

#define		USER_CARD_PARA_FILE			0X81		//用户卡参数信息文件
#define		USER_CARD_MONEY_FILE		0X82		//用户卡购电信息文件
#define		USER_CARD_CUR_RATE_FILE		0X83		//用户卡当前套电价文件
#define		USER_CARD_BAK_RATE_FILE		0X84		//用户卡备用套电价文件	
#define		USER_CARD_RETURN_WRITE_FILE	0X85		//用户卡返写信息文件
	

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//仅电价标志
typedef enum
{
	eCurRate = 0,           //当前套费率电价
	eBackRate,              //备用套费率电价
	eCurLadder,             //当前套阶梯电价
	eBackLadder,            //备用套阶梯电价
}ePriceType; 


typedef struct TCurrentPrice_t
{
	TFourByteUnion		Current_L;			//当前阶梯电价
	TFourByteUnion		Current_Lx;			//当前阶梯电价乘以变比后电价
	TFourByteUnion		Current_F;			//当前费率电价
	TFourByteUnion		Current_Fx;			//当前费率电价乘以变比后电价
	TFourByteUnion		Current;			//当前电价
	BYTE				Current_Ladder;		//当前阶梯
	BYTE				Ladder_Mode;		//结算模式字,0x00为月结算,0x55为年结算
}TCurPrice;

typedef struct TUserCardMoney_t
{
	BYTE	MeterNo[6];			//表号
	BYTE 	CustomCode[6+4];	//户号+mac
	BYTE 	ReturnFileStatus;	//用户卡返写状态	1不为空，0为空
	BYTE 	UserCardType;		//用户卡类型
	BYTE 	BuyTimesFlag;		//购电次数差值 0：相等, 1：大1, 2：大于等于2, 0xff：小于;
}TUserCardMoney;

//449字节
typedef struct TCardParaUpdate_t
{
	BYTE FlagBuf[UPDATE_FLAG_LENGTH];		//读出卡中参数更新标志位
	BYTE ParaBuf[PARA_BUF_LENGTH];			//读出卡中参数文件备份
	BYTE CurRateBuf[RATE_BUF_LENGTH];		//读出卡中当前套费率电价备份
	BYTE CurLadderBuf[LADDER_BUF_LENGTH];	//读出卡中当前套阶梯备份
	BYTE BakRateBuf[RATE_BUF_LENGTH];		//读出卡中备用套费率电价备份
	BYTE BakLadderBuf[LADDER_BUF_LENGTH];	//读出卡中备用套阶梯备份
	BYTE BakRateTimeBuf[TIME_BUF_LENGTH];	//备用套费率起用时间
	BYTE BakLadderTimeBuf[TIME_BUF_LENGTH];	//备用套阶梯起用时间
}TCardParaUpdate;
#endif

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE	PrePayBuf[MAX_PREPAY_LEN];

extern __no_init BYTE		MacErrCounter;
extern	TSafeAuPara         SafeAuPara;			//安全认证相关参数
extern TPrePayPara          PrePayPara;          //基本参数
#if( PREPAY_MODE == PREPAY_LOCAL)
extern TRatePrice			CurRateTable;		//当前费率电价表
extern TLadderPrice			CurLadderTable;		//当前阶梯电价表
extern TLimitMoney		    LimitMoney;         //金额门限参数
extern TCurPrice		    CurPrice;			//当前电价 用于扣费
extern TCardInfo   CardInfo; //卡信息（开户状态、卡序列号）
extern __no_init TCurMoneyBag CurMoneyBag;//钱包文件、已扣费脉冲数 
extern BYTE	Err_Sw12[2];					//暂存错误命令返回的SW1和SW2

#endif
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

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
//备注:
//-----------------------------------------------
BOOL Link_ESAM_TxRx(BYTE CmdLen, BYTE *Cmd, WORD BufLen, BYTE *InBuf, BYTE *OutBuf);

#if( PREPAY_MODE == PREPAY_LOCAL )
/********************money.c**************************/

//-----------------------------------------------
//函数功能: 上电金额处理
//
//参数: 	    ePOWER_MODE Type
//                   
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void PowerUpMoney( ePOWER_MODE Type );
//-----------------------------------------------
//函数功能: 掉电金额处理
//
//参数: 	    无
//                   
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void PowerDownMoney( void );

//-----------------------------------------------
//函数功能:     金额分钟任务
//
//参数: 	    无
//                   
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void MoneyMinuteTask( void );
//-----------------------------------------------
//函数功能:     金额秒任务
//
//参数: 	    无
//                   
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void MoneySecTask( void );

/********************card.c**************************/

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
BOOL Link_Card_TxRx(BYTE Cla,BYTE Ins,BYTE P1,BYTE P2,BYTE P3,BYTE RW,BYTE *peerom);

//-----------------------------------------------
//函数功能: 复位卡片,读取复位信息
//
//参数:		Buf返回复位信息
//						
//返回值:		TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL Link_CardReset( BYTE *Buf );

//-----------------------------------------------
//函数功能: 取CARD随机数
//
//参数:		Len[in]		随机数长度 4/8/10
//			Buf[out]	输出长度+数据（长度两字节，高在前，低在后）					
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL ReadRandomCARD(BYTE Len,BYTE *peeerom);

//-----------------------------------------------
//函数功能: 带MAC读取卡片中二进制数据
//
//参数:		CH[in]；01:读取用户卡和预置卡中除了钱包之外的文件  21:读取预置卡中钱包文件  31:	读取用户卡中钱包文件
//			P1[in],P2[in],M1[in],M2[in],指令流的格式，见参数手册说明
//			Len[in]从卡中读出数据的长度+4字节mac的长度(Lc+4)
//返回值:	成功/失败
//		   
//备注:
//		
//-----------------------------------------------
BYTE ReadBinWithMacCard( BYTE CH, BYTE P1, BYTE P2, BYTE M1, BYTE M2, BYTE Len ,BYTE *peerom);

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
BYTE SelectFile( BYTE F1, BYTE F2 );

//-----------------------------------------------
//函数功能: 读卡和ESAM中二进制文件
//	
//参数:		P1[in],P2[in],P3[in] 指令流的格式，见参数手册说明
//			peeerom[in] 		 卡片复位信息
//
//返回值:		无
//		   
//备注:
//----------------------------------------------
BYTE ReadBinFile(BYTE P1, BYTE P2, BYTE P3,BYTE *peeerom);

//-----------------------------------------------
//函数功能: 插卡处理程序
//	
//参数:		无
//
//返回值:		无
//		   
//备注:
//----------------------------------------------
void Card_Proc( void );

//-----------------------------------------------
//函数功能: 上电或阶梯表发生切换重新初始化阶梯表、阶梯模式
//
//参数: 	无
//                   
//返回值:   无
//
//
//备注:		阶梯模式切换 年阶梯 月阶梯
//			需要进行年月阶梯模式切换的情况
//			1：卡，交互终端通过用户卡进行设置当前套阶梯表时要判断
//			2：费控模式进行切换时要判断
//			3：起用备用套阶梯时区表时，需要判断采用当前第几套阶梯表，再判断年月阶梯切换
//			4：起用备用套阶梯表时，需要判断年月阶梯切换
//			5：阶梯时区表内部进行切换时，找到当前阶梯采用哪套表，再判断年月阶梯切换
//			6：设置当前套阶梯时区表时，需要找到当前阶梯采用哪套表，再判断年月阶梯切换
//			7：设置备用套阶梯时区表，备用套阶梯表时，根据大任务里面的判断是否进行年月阶梯切换
//			8: 年结算日发生变化或者年结算日进行切换
//-----------------------------------------------
void ProcLadderModeSwitch(void);//结算方式变换判断及处理

//-----------------------------------------------
//函数功能: 初始化阶梯表、阶梯模式
//
//参数: 	无
//                   
//返回值:   无
//
//备注:     包括上电初始化和预置卡设置当前套阶梯两种情况
//			本函数还要放在上电初始化BasicPara的函数中，如果不放其中，则需要加入写ee和ram的函数
//-----------------------------------------------
void InitJudgeBillMode(void);

//-----------------------------------------------
//函数功能: 清卡信息
//
//参数:  		无
//
//返回值:		无
//
//备注:
//-----------------------------------------------
void ClrCardInfo( void );

//-----------------------------------------------
//函数功能: 读取钱包
//
//参数:		Buf[out]	输出长度+数据（长度两字节，高在前，低在后）
//
//返回值:	TRUE/FALSE
//
//备注:返回长度+购电金额（4）+购电次数（4）+客户编号（6）
//-----------------------------------------------
BOOL ReadEsamMoneyBag( BYTE *Buf ); 

//-----------------------------------------------
//函数功能: 计量钱包同步
//
//参数:     无
//
//返回值:   无
//
//备注:
//-----------------------------------------------
void MeteringUpdateMoneybag( void ); 

//-----------------------------------------------
//函数功能: 本地状态判断
//
//参数:    Type[in]
//           eReturnMoneyMode  -- 扣费或者退费操作
//			 eChargeMoneyMode  -- 充值操作
//           eSetParaMode      -- 更改参数操作
//			 eInitMoneyMode     -- 预置操作
//        ParaType[in]         -- 如果是更改参数 传入参数类型 非更改参数操作传入FF
//			 eTickLimit		   -- 透支金额门限
// 			 eAlarm_Limit1	   -- 报警金额1
// 			 eAlarm_Limit2	   -- 报警金额2
//返回值:  本地状态
//
//备注:
//-----------------------------------------------
void JudgeLocalStatus( eUpdataMoneyMode Type, BYTE ParaType );

//---------------------------------------------------------------
//函数功能: 获得当前阶梯用电量
//
//参数: 	无
//
//返回值:	当前阶梯用电量
//
//备注:   根据组合模式字返回
//---------------------------------------------------------------
DWORD GetCurLadderUserEnergy( void );
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
BYTE CheckSwitchTime( BYTE *Buf );

#endif

#endif//__PREPAY_H






