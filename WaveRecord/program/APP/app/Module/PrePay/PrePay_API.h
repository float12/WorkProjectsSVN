//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.8.16
//描述		预付费 Api头文件
//修改记录	

//
//----------------------------------------------------------------------
#ifndef __PREPAY_API_H
#define __PREPAY_API_H
//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//显示里也要用改定义
#define		OK_CARD_PROC		0xff	//卡片操作成功
#if( PREPAY_STANDARD == PREPAY_NANWANG_15 )//南网
	#define		USER_CARD_COMMAND			0x03		//用户卡命令码
	#define		PRESET_CARD_COMMAND			0x02		//预制卡命令码
#else//( PREPAY_STANDARD == GUO_WANG_STANDARD )//国网
	#define		USER_CARD_COMMAND			0x01		//用户卡命令码
	#define		PRESET_CARD_COMMAND			0x06		//预制卡命令码
#endif

#define	REMOTE_MODE				1				//远程模式
#define	LOCAL_MODE				0				//本地模式
#define LADDER_MONTH_MODE		0x00			//月阶梯模式
#define LADDER_YEAR_MODE		0x55			//年阶梯模式

#define READ_METER_RN	0x01		//明文+RN抄读表数据 / 明文+ MAC回复

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//本地状态
typedef enum
{
	eNoOpenAccount = 0,	    //未开户状态
	eOpenAccount = 0x55,	//已开户状态
}eCardFlag;

//本地状态
typedef enum
{
	eNoAlarm = 0,	//正常状态
	eAlarm1,     	//处于报警金额1状态
	eAlarm2,     	//处于报警金额2状态
	eOvrZero,    	//处于过零状态
	eOvrTick,    	//处于透支状态
}eLOCAL_STATUS;

typedef enum 
{
	eReturnMoneyMode = 0,		//扣减或退费操作
	eChargeMoneyMode,	        //充值操作
	eSetParaMode,				//更改参数操作
	eInitMoneyMode,	            //预制操作
}eUpdataMoneyMode;

//电价变化标志
typedef enum
{
	eNoUpdatePrice = 0,			//无需刷新电价
	ePriceChangeEnergy,         //电价未发生变化 但需要刷新当前电价
	eChangeCurRatePrice,	    //改变了当前费率电价
	eChangeCurLadderPrice,		//改变了当前阶梯电价
}eChangeCurPriceFlag;

typedef enum
{
	eInfoCurLadder,				//当前所处阶梯
	eInfoBackupRateFalg,		//是否有费率电价备用套
	eInfoBackupLadderFlag,		//是否有阶梯电价备用套
}eLocalInfo;

typedef enum
{
	GENERATE_RN = 0,		    //生成随机数
	VERIFY_MAC,	                //明文+RN数据验证MAC
}eProcessMacRN;


//预付费参数
typedef enum//W-可写 R-可读
{
	eKeyNum = 0,                                    //密钥条数-WR
	eCTCoe,                                         //CT变比-WR
	ePTCoe,                                         //PT变比-WR
	eIRTime,                                        //红外认证时间-WR
	eRemoteTime,									//身份认证时间-WR(协议不可读)
#if( PREPAY_MODE == PREPAY_LOCAL)
	eLadderNum,										//阶梯数-WR			
	eTickLimit,										//透支金额门限-WR     
	eRegrateLimit,                                  //囤积金额门限-WR     
	eCloseLimit,                                    //合闸允许金额门限-WR  
	eAlarm_Limit1,									//报警金额1-WR  
	eAlarm_Limit2,                                  //报警金额2-WR  
	eCurPice,										//当前电价-R   
	eCurRatePrice,                                  //当前费率电价-R
	eCurLadderPrice,                                //当前阶梯电价-R
	eCurRateTable,									//当前套费率表-WR(预置卡和开户卡可写)
	eBackupRateTable = (eCurRateTable + 0x80),		//备用套费率表-WR	
	eCurLadderTable,								//当前套阶梯表-WR(预置卡和开户卡可写)
	eBackupLadderTable = (eCurLadderTable + 0x80),	//备用套阶梯表-WR 
	eRemainMoney,									//剩余金额-R
	eBuyTimes,										//购电次数-R
	eLocalAccountFlag,								//本地开户状态-WR
	eRemoteAccountFlag,                             //远程开户状态-WR
	eSerialNo,										//卡序列号-WR
	eIllegalCardNum,								//非法插卡次数-WR
	eLadderUseEnergy,								//当前阶梯结算用电量
#endif//#if( PREPAY_MODE == PREPAY_LOCAL)
}ePrePayParaType;

#if( PREPAY_MODE == PREPAY_LOCAL)
typedef struct TLocalStatus_t
{
	eLOCAL_STATUS  CurLocalStatus;     //当前本地状态
	eLOCAL_STATUS  LastLocalStatus;    //上次本地状态 可用户后期维护查找问题
	DWORD          CRC32;
}TLocalStatus;

//此结构需要pack，里面是8对齐
#pragma pack(1)
typedef struct Tmoney_t
{
	TEightByteUnion  Remain;    //剩余金额
	DWORD BuyTimes;             //购买次数
}TMoney;

typedef struct TLadderUserEnergy_t
{
	DWORD ForwardEnergy;    //正向阶梯用电量
	DWORD BackwardEnergy;   //反向阶梯用电量
}TLadderUserEnergy;

//TMoney若不pack（1），CRC32不是在最后4个字节，计算校验时就出错了
typedef struct TCurMoneyBag_t 
{
    TMoney  			Money;              //剩余金额
	TLadderUserEnergy	UserEnergy;          //用电量
	DWORD          CRC32; 
}TCurMoneyBag;

typedef struct TRatePrice_t
{
	DWORD Price[MAX_RATIO];     //费率电价
	DWORD CRC32;                //校验
}TRatePrice;
typedef struct TLadderSavePara_t
{
	//hex码 年结算日 年阶梯专用
	BYTE Mon;
	//hex码，若为0xFF无效，
	BYTE Day;
	//冻结小时 hex码，如为FF无效
	BYTE Hour;
	
}TLadderSavePara;
typedef struct TLadderBillingPara_t
{
	//阶梯结算最大为4
	TLadderSavePara LadderSavePara[MAX_YEAR_BILL];
	
}TLadderBillingPara;

typedef struct TLadderPrice_t //长度74个字节
{
	DWORD Ladder_Dl[MAX_LADDER];        //阶梯电量
	DWORD Price[MAX_LADDER + 1];        //阶梯电价
	BYTE  YearBill[MAX_YEAR_BILL][3];   //年结算日	//@@@@年结算日改用上面结构体
	DWORD CRC32;                        //校验
}TLadderPrice;

typedef struct TBackupYearBill_t
{
	BYTE  YearBill[MAX_YEAR_BILL][3];//备份年结算日 用于判断阶梯切换时结算日是否改变
	DWORD CRC32;//校验
}TBackupYearBill;

//电价参数
typedef struct TPricePara_t
{
	TRatePrice CurrentRate;         //当前套费率
	TLadderPrice CurrentLadder;     //当前套阶梯
	TRatePrice BackupRate;        	//备用套费率
	TLadderPrice BackupLadder;      //备用套阶梯
	DWORD		   CRC32;
}TPricePara;

typedef struct TCardInfo_t
{
    BYTE LocalFlag;     //本地开户标志
    BYTE RemoteFlag;    //远程开户标志
    BYTE SerialNo[10];  //卡序列号
	TFourByteUnion IllegalCardNum;    //非法插卡总次数
    DWORD CRC32;                    //校验
}TCardInfo;

typedef struct TsafeLimitMoney_t
{
	TFourByteUnion TickLimit;		//透支金额门限
	TFourByteUnion RegrateLimit;	//囤积金额门限
	TFourByteUnion CloseLimit;		//合闸允许金额门限
	TFourByteUnion Alarm_Limit1;	//报警金额1
	TFourByteUnion Alarm_Limit2;	//报警金额2		
	DWORD		   CRC32;
}TLimitMoney;

#endif

typedef struct TPrePayPara_t
{
	//密钥条数
	BYTE ScretKeyNum;
	//阶梯数
	BYTE LadderNum;   	
	// CT变比
	DWORD CTCoe;
	// PT变比
	DWORD PTCoe;
	//红外认证时间
	DWORD IRTime;
	//远程645身份认证时效
	WORD Remote645AuthTime;
	//校验
	DWORD CRC32;
	
}TPrePayPara;

typedef struct TPrePaySafeRom_t
{
    TPrePayPara 			PrePayPara;
    #if( PREPAY_MODE == PREPAY_LOCAL)
    TCurMoneyBag    		CurMoneyBag;		//剩余金额与已扣费电能
	TLimitMoney             LimitMoney;         //金额门限参数 - 透支、囤积、合闸允许、报警金额1、2
	TPricePara              Price;              //当前套和备用套电价文件
	TBackupYearBill 		BackupYearBill; 	//备份年结算日 用于判断阶梯切换时结算日是否改变	
	TLocalStatus            LocalStatus;        //本地状态
	TCardInfo               CardInfo;           //卡信息（开户状态、卡序列号）
	#endif
}TPrePaySafeRom;


// 在连续空间中的预付费信息
typedef struct TPrePayConMem_t
{
	#if( PREPAY_MODE == PREPAY_LOCAL )
	DWORD dwIllegalCardNum;	
	DWORD dwTotalBuyMoney;		//累计购电金额 要不要放在 TMoney or TCurMoneyBag 中wlk
	#else
	BYTE byTemp;
	#endif
}TPrePayConMem;
#pragma pack()

typedef struct 
{
    BYTE *rand;				//随机数长度+随机数
	BYTE *apduMacBuf;		//APDU+MAC buf
	BYTE mailAddr[6];		//通信地址
    WORD apduMacBufLen; 	//APDU+MAC的长度
}verifyMacInfo_t;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE    s_ReadMeterMAC[];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//功能描述:  添加命令头函数
//         
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
void api_ProcPrePayCommhead(BYTE* pbuf,BYTE inc, BYTE com, BYTE P1, BYTE P2, BYTE len1,BYTE len2);

//-----------------------------------------------
//函数功能: 获取电表安全芯片信息
//
//参数:		Type[in]	ff--全部信息
//			OutBuf[out]	放置要读取的长度及数据，长度两字节，高在前，低在后
//返回值:	数据长度 8000代表错误
//
//备注:OutBuf需比实际计算的长度多两个字节，但在函数内已经处理，返回的数据前面不带长度
//-----------------------------------------------
WORD api_GetEsamInfo( BYTE Type, BYTE *OutBuf );

//-----------------------------------------------
//函数功能: 建立应用连接
//
//参数:		Buf[in/out]	输入时32字节密文1+4字节客户机签名1
//						输出时长度+48字节随机数+4字节客户机签名，长度两字节，高在前，低在后
//返回值:	TRUE/FALSE
//
//备注:在698规约中建立应用连接时使用,Buf前面2字节代表数据长度
//-----------------------------------------------
//BOOL api_ConnectMechanismInfo( eConnectMode ConnectModeMode, BYTE *Buf );

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
BOOL api_AuthDataSID(BYTE *Buf);

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
BOOL api_PackDataWithSafe(WORD Len,BYTE *Buf);

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
BOOL api_PackEsamDataWithSafe(BYTE EncryptMode, BYTE Type,WORD Len,BYTE *Buf);

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
BOOL api_ReadEsamData(WORD FID,WORD Offset,WORD Len,BYTE *Buf);

//-----------------------------------------------
//函数功能: 取ESAM随机数
//
//参数:		Len[in]		随机数长度 4/8/10
//			Buf[out]	输出长度+数据（长度两字节，高在前，低在后）					
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_ReadEsamRandom(BYTE Len,BYTE *Buf);

//--------------------------------------------------
//功能描述: 验证MAC
//
//参数:		Type[in]:保留，验证MAC
//				READ_METER_RN: 抄读下级表数据，数据 + MAC
//			MeterNo[in]: 6字节表号
//			rand[in]:随机数
//			Len[in]: Dat长度
//			Dat[in]: 数据
//返回值: FALSE / TRUE
//
//备注:
//--------------------------------------------------
WORD api_VerifyEsamSafeMAC(BYTE Type, BYTE *MeterNo, BYTE *rand,WORD Len, BYTE *Dat);

//--------------------------------------------------
//功能描述: 获得随机数
//
//参数: Type[in]: 随机数类型，随机数用途
//		rand[out]:随机数
//
//返回值: FALSE/TRUE
//
//备注:
//--------------------------------------------------
WORD api_GetEsamSafeRN(BYTE Type,BYTE *rand);

//-----------------------------------------------
//函数功能:     红外认证指令（获取随机数1密文）
//
//参数:	    INBuf[in]     //输入随机数密文	
//						
//返回值:	    TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_EsamIRAuth( BYTE *INBuf );
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
WORD api_DealEsamSafeReportRN_MAC( WORD Len,BYTE *Buf,BYTE *OutBuf );

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
WORD api_VerifyEsamSafeReportMAC( BYTE DataLen,BYTE *DataBuf,BYTE *RN_MAC );

//-----------------------------------------------
//函数功能:     红外查询指令（获取随机数1密文）
//
//参数:	    RNBuf[in]     //输入随机数 RNBuf[0](随机数长度)+RNBuf[1]。。（随机数）
//	        OutBuf[in/out] //输出数据 （8字节 不带长度）
//		
//						
//返回值:	    TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_EsamIRRequest( BYTE *RNBuf, BYTE * OutBuf );
BOOL api_EsamTerminalRequest( BYTE *RNBuf, BYTE *OutBuf );

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
BOOL api_SoftwareComparisonEsam(BYTE P2,WORD Len,BYTE *Buf);

//-----------------------------------------------
//函数功能: 获取密钥状态
//
//参数:		无					
//返回值:	无
//		   
//备注:上电的时候根据esam中的状态刷新ee中的密钥状态
//-----------------------------------------------
void api_FlashKeyStatus(void);

//---------------------------------------------------------------
//函数功能: 刷新身份认证权限标志
//
//参数: 
//                   
//返回值:  
//
//备注:   刷新身份认证权限 标志
//---------------------------------------------------------------
void api_FlashIdentAuthFlag( void );

//-----------------------------------------------
//函数功能: 写保存在ESAM 中的参数
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
WORD api_DecipherSecretData(BYTE Type, WORD LC, BYTE *InData, BYTE *OutBuf);

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
BYTE api_Proc_uart_Write_FirstData_645( BYTE *pBuf );

//-----------------------------------------------
//函数功能: 安全处理函数
//
//参数:		
//		ProBuf[in/out]		in--输入缓冲645的第一个字节68开头
//								out--输出数据，已经倒序。					
//返回值:	数据长度
//		   
//备注:698版本ESAM 供645规约03命令字调用
//-----------------------------------------------
WORD api_ProcProPrePay(eSERIAL_TYPE PortType, BYTE *ProBuf ,BYTE *OutBuf );

//-----------------------------------------------
//函数功能: 继电器控制命令操作
//
//参数:	ProBuf[in/out]	645报文		
//		
//返回值:	执行状态，两个字节
//		   
//备注:698版本ESAM 供645规约1c命令字调用
//-----------------------------------------------
WORD api_ProcRemoteRelayCommand645(BYTE *ProBuf);

//-----------------------------------------------
//函数功能: 预付费任务上电初始化
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_PowerUpPrePay(void);
//-----------------------------------------------
//函数功能: 预付费任务低功耗唤醒任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_LowPowerWakeUpPrePay(void);

//-----------------------------------------------
//函数功能: 预付费掉电处理
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_PowerDownPrePay(void);

//-----------------------------------------------
//函数功能: 预付费大循环任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_PrePayTask(void);

//-----------------------------------------------
//函数功能: 厂内电表初始化任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_FactoryInitPrePay(void);

//-----------------------------------------------
//函数功能: 电表清零
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_ClrPrePay(void);

//--------------------------------------------------
//功能描述:  复位ESAMspi
//         
//参数  :   无
//
//返回值:    无  
//         
//备注内容:  无
//--------------------------------------------------
void api_ResetEsamSpi( void );

//-----------------------------------------------
//函数功能: 读预付费参数
//
//参数:
//          ePrePayPara Type[in]	读写数据 详见枚举定义
//
//			Buf[out]	输出缓冲
//返回值:	    数据长度   长度为0 错误
//
//备注:
//-----------------------------------------------
WORD api_ReadPrePayPara( ePrePayParaType Type, BYTE *Buf );

//-----------------------------------------------
//函数功能: 写预付费参数
//
//参数:		ePrePayPara Type[in]	读写数据 详见枚举定义
//
//			Buf[in]	   输入缓冲 均为HEX格式
//返回值:	    TRUE/FALSE
//
//备注:    此函数更改费率阶梯电价可自动重新判断电价
//-----------------------------------------------
BOOL api_WritePrePayPara( ePrePayParaType Type, BYTE *Buf );
//-----------------------------------------------
//函数功能:	随机数生成，验证MAC
//	
//参数:	 type	    		执行的类型
//		 verifyMacInfo 		需要的参数
//返回值:	TRUE FALSE
//	
//备注:   
//-----------------------------------------------
BOOL api_processMacRN(eProcessMacRN type,verifyMacInfo_t *verifyMacInfo);
#if( PREPAY_MODE == PREPAY_LOCAL)

//-----------------------------------------------
//函数功能: 电费扣减
//
//参数: 	EnergyDirection[in]		电能反向
//			inTotalReduceEnergyNum[in]	新累加总电能 单位0.01kwh
// 			inRatioReduceEnergyNum[in]	新累加当前费率电能 单位0.01kwh
//返回值:   无
//
//备注:
//-----------------------------------------------
void api_ReduceRemainMoney( BYTE EnergyDirection, BYTE inTotalReduceEnergyNum, BYTE inRatioReduceEnergyNum );

//-----------------------------------------------
//函数功能:     设置电价刷新标志位
//
//参数: 	    PriceFlag[in]
//			    eNoUpdatePrice		        电价未发生变化
//			    ePriceChangeEnergy,			电价未发生变化 但需要刷新当前电价
//			    eChangeCurRatePrice,	    改变了当前费率电价
//			    eChangeCurLadderPrice,		改变了当前阶梯电价
//
//返回值:   无
//
//备注:
//-----------------------------------------------
void api_SetUpdatePriceFlag( eChangeCurPriceFlag  PriceFlag );

//-----------------------------------------------
//函数功能: 得到当前本地状态
//
//参数: 	无
//                   
//返回值:  	返回本地状态
//				eNoAlarm,	 正常状态
//				eAlarm1,     处于报警金额1状态
//				eAlarm2,     处于报警金额2状态
//				eOvrZero,    处于过零状态
//				eOvrTick,    处于透支状态
//
//备注:   	用于继电器本地状态查询
//-----------------------------------------------
eLOCAL_STATUS api_GetLocalStatus( void );

//-----------------------------------------------
//函数功能:     充值或更新剩余金额
//
//参数: 	    Type[in] 0x00: 充值金额    0x55: 预制金额                
// 		    ReduceValue[in] 扣除金额
// 			
//                   
//返回值:    无
//
//备注:   
//-----------------------------------------------
BOOL api_UpdataRemainMoney( eUpdataMoneyMode Type, DWORD ReduceValue, DWORD BuyTimes );

//-----------------------------------------------
//函数功能: 阶梯结算日检查电能，用于保存阶梯结算时电能底码，用于计算阶梯用电量
//
//参数:  	无
//	
//返回值:	TRUE--当前月、日 、时正好处于阶梯结算日 FALSE---当前月、日 、时不处于阶梯结算日
//
//备注:   	小时变化调用
//-----------------------------------------------
BOOL api_CheckLadderClosing( void );

//---------------------------------------------------------------
//函数功能: 清当前用电量
//
//参数:   无
//
//返回值: 无
//
//备注:  用于年结算或者月结算时清零当前用电量
//---------------------------------------------------------------
void api_ClrCurLadderUseEnergy( void );

//-----------------------------------------------
//函数功能: 转存金额（包括与ESAM钱包同步）
//
//参数: 	无
//                   
//返回值:  	无
//
//备注: 
//-----------------------------------------------
void api_SwapMoney( void );

//---------------------------------------------------------------
//函数功能: 获取当前处于第几阶梯、当前是否有备用套阶梯表、当前是否有备用套费率
//
//参数: 	Type
//            eInfoCurLadder - 获取当前所处阶梯
//            eInfoBackupRateFalg - 是否有备用套费率
//            eInfoBackupLadderFlag - 是否有备用套阶梯
//
//返回值:  		  0 	返回所处于的阶梯  0xff不显示阶梯符号
//				1  返回0xff不显示 true代表有 FALSE 代表无
//           	2  返回0xff不显示 true代表有 FALSE 代表无
//
//
//备注:   0xff表示远程模式不显示所有阶梯、费率相关的符号
//---------------------------------------------------------------
BYTE api_GetLocalInfo( eLocalInfo Type );

//-----------------------------------------------
//函数功能: 读非法插卡次数
//
//参数:  		Buf[out] 输出非法插卡次数 
//
//返回值:		无
//		   
//备注:			
//-----------------------------------------------
void api_ReadIllegalCardNum(BYTE *Buf);

//-----------------------------------------------
//函数功能: 读累计购电金额
//
//参数:  		Buf[out] 输出累计购电金额
//
//返回值:		无
//		   
//备注:			
//-----------------------------------------------
void api_ReadTotalBuyMoney(BYTE *Buf);
//-----------------------------------------------
//函数功能: 获取插卡状态字
//
//参数: 	无
//                   
//返回值:   01 成功   02 失败 
//
//备注:   
//-----------------------------------------------
void api_GetCardInsertStatus(BYTE *OutBuf);
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
BYTE api_TestEsamCard( void );

//-----------------------------------------------
//函数功能: 获取当前的阶梯结算日表
//
//参数:  		t[in] 当前时间
//
//返回值:		阶梯模式
//			eNoLadderMode	--无阶梯模式
//			eLadderMonMode	--月阶梯模式
//			eLadderYearMode	--年阶梯模式
//
//备注:
//-----------------------------------------------
BYTE api_GetCurLadderYearBillInfo(TLadderBillingPara* LadderBillingPara);
//-----------------------------------------------
//函数功能: 获取掉电前的阶梯结算日表
//
//参数: 		LadderBillingPara[out]:阶梯结算日表
//
//返回值:		阶梯模式
//			eNoLadderMode	--无阶梯模式
//			eLadderMonMode	--月阶梯模式
//			eLadderYearMode	--年阶梯模式
//
//备注:
//-----------------------------------------------
BYTE api_GetPowerDownYearBill(TLadderBillingPara* LadderBillingPara);
//-----------------------------------------------
//函数功能: 从E2读取当前阶梯表到掉电前阶梯表
//
//参数: 	
//
//返回值:	

//
//备注:只给补冻結用，在初始化参数前读取；防止掉电过阶梯结算将E2中当前阶梯表覆盖
//-----------------------------------------------
void api_ReadPowerLadderTable(void);



#if(SEL_DLT698_2016_FUNC == YES )
BYTE api_RemoteActionMoneybag( BYTE Type, DWORD Money, DWORD BuyTimes, BYTE *pCustomCode, BYTE *pSIDMAC, BYTE *pMeterNo );
#endif

#endif//#if( PREPAY_MODE == PREPAY_LOCAL)
#endif//__PREPAY_API_H
