//------------------------------------------------------------------------------------------
//	名    称： dlt645_2007.h
//	功    能:  DL/T645-2007 扩展结构描述文件
//	作    者:  魏灵坤
//	创建时间:  2007-11-6 19:54
//	更新时间:
//	备    注:	，把结构提出来
//	修改记录:
//------------------------------------------------------------------------------------------
#ifndef __Dlt645_2007_H
#define __Dlt645_2007_H

#if( SEL_DLT645_2007 == YES )


//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define DLT_CLOSEING_NO				0X0C		//结算日支持的最大次数

//协议密码定义
#define CLASS_2_PASSWORD_LEVEL				0x98//二类数据加密操作密码级别国网为98H
#define CLASS_1_PASSWORD_LEVEL				0x99//一类数据加密操作密码级别国网为99H
#define MINGWEN_H_PASSWORD_LEVEL			0x02
#define MINGWEN_L_PASSWORD_LEVEL			0x04
#define SYSTEM_UNLOCK_PASSWORD_LEVEL		0xAB

//规约读数据函数返回信息，读成功返回读数据长度
#define DLT645_OK_00							0x0000		//无错误
#define DLT645_ERR_DATA_01						0x8001		//非法数据
#define DLT645_ERR_ID_02						0x8002		//数据标识码错
#define DLT645_ERR_PASSWORD_04					0x8004		//密码错
#define DLT645_CANNOT_CHANGE_BAUD_08			0x8008		//DL/T645-2007 通信速率不能更改
#define DLT645_ERR_OVER_AREA_10					0x8010		//年时区数超
#define DLT645_ERR_OVER_SEG_20					0x8020		//日时段数超
#define DLT645_ERR_OVER_RATIO_40				0x8040		//费率数超
#define DLT645_ERR_PRG_KEY_80					0x8080		//编程按键没有按下
#define DLT645_NO_RESPONSE						0x80FF		//无需回复

//================================================预留特殊标识=====================================================
#define DLT645_SPECIAL_FLAG_00000001H			(0x00000001)//预留1
#define DLT645_SPECIAL_FLAG_00000002H			(0x00000002)//预留1
#define DLT645_SPECIAL_FLAG_00000004H			(0x00000004)//预留1
#define DLT645_SPECIAL_FLAG_00000008H			(0x00000008)//预留1
#define DLT645_SPECIAL_FLAG_00000010H			(0x00000010)//预留1
#define DLT645_SPECIAL_FLAG_00000020H			(0x00000020)//预留1
#define DLT645_SPECIAL_FLAG_00000040H			(0x00000040)//预留1
#define DLT645_SPECIAL_FLAG_00000080H			(0x00000080)//预留1
#define DLT645_SPECIAL_FLAG_00000100H			(0x00000100)//预留1
#define DLT645_SPECIAL_FLAG_00000200H			(0x00000200)//预留1
#define DLT645_SPECIAL_FLAG_00000400H			(0x00000400)//预留1
#define DLT645_SPECIAL_FLAG_00000800H			(0x00000800)//预留1
#define DLT645_SPECIAL_FLAG_00001000H			(0x00001000)//预留1
#define DLT645_SPECIAL_FLAG_00002000H			(0x00002000)//预留1
//================================================厂内模式标识=====================================================
#define DLT645_ENERGY_00H						(0X00)		//DI3 电能标识
#define DLT645_DEMAND_01H						(0X01)		//DI3 最大需量与发生时间标识
#define DLT645_VARIABLE_02H						(0X02)		//DI3 变量标识
#define DLT645_PARA_04H							(0X04)		//DI3 参变量标识
#define DLT645_FREEZE_05H						(0X05)		//DI3 冻结数据标识
#define DLT645_LOAD_06H							(0X06)		//DI3 负荷记录标识
#define DLT645_EVENT_03H						(0X03)		//DI3 密钥更新记录、异常插卡事件标识
#define DLT645_EVENT_10H						(0X10)		//DI3 失压事件标识
#define DLT645_EVENT_11H						(0X11)		//DI3 欠压事件标识
#define DLT645_EVENT_12H						(0X12)		//DI3 过压事件标识
#define DLT645_EVENT_13H						(0X13)		//DI3 断相事件标识
#define DLT645_EVENT_14H						(0X14)		//DI3 电压逆相序事件标识
#define DLT645_EVENT_15H						(0X15)		//DI3 电流逆相序事件标识
#define DLT645_EVENT_16H						(0X16)		//DI3 电压不平衡事件标识
#define DLT645_EVENT_17H						(0X17)		//DI3 电流不平衡事件标识
#define DLT645_EVENT_18H						(0X18)		//DI3 失流事件标识
#define DLT645_EVENT_19H						(0X19)		//DI3 过流事件标识
#define DLT645_EVENT_1AH						(0X1A)		//DI3 断流事件标识
#define DLT645_EVENT_1BH						(0X1B)		//DI3 潮流反向事件标识
#define DLT645_EVENT_1CH						(0X1C)		//DI3 过载事件标识
#define DLT645_EVENT_1DH						(0X1D)		//DI3 跳闸事件标识
#define DLT645_EVENT_1EH						(0X1E)		//DI3 合闸事件标识
#define DLT645_EVENT_1FH						(0X1F)		//DI3 总功率因数事件标识
#define DLT645_EVENT_20H						(0X20)		//DI3 电流严重不平衡事件标识
#define DLT645_EVENT_21H						(0X21)		//DI3 功率方向改变事件标识
#define DLT645_EVENT_22H						(0X22)		//DI3 时钟电池欠压事件标识
#define DLT645_EVENT_23H						(0X23)		//DI3 停电抄表电池欠压事件标识

#define DLT645_FOLLOW_LEN			(196)//后续帧数据长度 不包括数据标识 为什么是196 是因为加数据标识正好200个字节


#define DLT645_FACTORY_ID						(0x04dfdfdb)

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum{
	ePROTOCOL_DLT645_07=0,		// 07规约 
	ePROTOCOL_DLT645_97,  		//97规约
	ePROTOCOL_DLT645_FACTORY,	//厂内规约
	
	ePROTOCOLL_MAX
}eTYPE_PROTOCOL;

typedef enum{
	eBYTE_MODE = 0,      //单字节模式
	eCOMB_MODE			//组合模式 比如长度为2 两个BYTE组合成一个WORD
}eTYPE_COVER_DATA;		//HEX和BCD转换模式

typedef enum{
	eNO_FOLLOW=0,		//无后续帧
	eNORMAL_FOLLOW,  	//正常后续帧
	eLOAD_FOLLOW,		//负荷记录后续帧
	eLOAD_FOLLOW_END,   //后续帧结束标志
	eREPORT_FOLLOW,		//主动上报后续帧
}eFOLLOW_FLAG;

typedef enum{
	eTYPE_ADDR_TYPE_88=0,	//全8地址，通用表地址
	eTYPE_ADDR_TYPE_99,		//全9地址
	eTYPE_ADDR_TYPE_ALLAA,	//全A地址
	eTYPE_ADDR_TYPE_AA,		//通配地址
	eTYPE_ADDR_TYPE_XX,		//普通地址
	
	eTYPE_ADDR_TYPE_MAX
}eTYPE_ADDR_TYPE;

typedef enum{
    eAUTHORITY_TYPE_INSIDE_FACTORY=BIT0,    //厂内权限
	eAUTHORITY_TYPE_CLASS_2=BIT1,	        //二类数据权限
	eAUTHORITY_TYPE_CLASS_1=BIT2,		    //一类数据权限
	eAUTHORITY_TYPE_MINGWEN_H=BIT3,	        //明文高级权限
	eAUTHORITY_TYPE_MINGWEN_L=BIT4,		    //明文低级权限
    eAUTHORITY_TYPE_MAC_OK=BIT5,            //mac未挂起权限
    eAUTHORITY_TYPE_IR_ALLOW=BIT6           //红外认证权限
}eAUTHORITY_TYPE;


typedef enum{
	eCONTROL_RECE_01=0x1,	//Dlt645规约读命令
	eCONTROL_RECE_03=0x3,	//预付费命令
	eCONTROL_RECE_08=0x8,	//广播校时
	eCONTROL_RECE_09=0x9,	//南网寻卡命令
	eCONTROL_RECE_10=0x10,	//清需量
	eCONTROL_RECE_11=0x11,	//规约读命令
	eCONTROL_RECE_12=0x12,	//规约读后续帧命令
	eCONTROL_RECE_13=0x13,	//规约读电表通信地址
	eCONTROL_RECE_14=0x14,	//规约写命令
	eCONTROL_RECE_15=0x15,	//规约写通信地址
	eCONTROL_RECE_16=0x16,	//冻结电能表数据
	eCONTROL_RECE_17=0x17,	//更改通信速率
	eCONTROL_RECE_19=0x19,	//当前最大需量及发生时间数据清零
	eCONTROL_RECE_1A=0x1a,	//清空电能表内电能量、最大需量及发生时间、冻结量、事件记录、负荷记录等数据
	eCONTROL_RECE_1B=0x1b,	//清空电能表内存储的全部或某类事件记录数据
	eCONTROL_RECE_1C=0x1c,	//跳闸、合闸允许、报警、报警解除、保电和保电解除
	eCONTROL_RECE_1D=0x1D	//设置多功能端子输出信号类别
}eTYPE_CONTROL_RECE;

//
typedef struct{
	BYTE *pMessageAddr;					//报文传入地址
	eTYPE_PROTOCOL eProtocolType;		//规约类型
	eTYPE_ADDR_TYPE eAddrType;			//下发报文的地址类型：99，88，AA,正常
	WORD wSerialDataLen;				//串口数据长度
	TFourByteUnion dwDataID;			//数据标识
	BYTE bIDLen;						//数据标识长度
	BYTE byReceiveControl;				//接收报文控制码
//	BYTE bSendControl;					//发送报文控制码  //发送报文控制码可由接受报文控制码转换 是否可以去掉!!!!
	BYTE *pDataAddr;					//数据区地址
	WORD wDataLen;						//数据区长度		
//	BYTE byFollowUpFrameExist;			//是否有后续数据
	BYTE bSEQ;							//后续帧序号 解析但对于组帧无影响 在后续帧的首帧将帧序号置为0
//	BYTE bAddr[6];						//通信地址
	BYTE byOperatorCode[4];				//操作者代码
	BYTE bLevel;						//密码等级
	BYTE bPassword[3];					//下发的密码
	WORD wErrType;						//错误类型
	DWORD dwSpecialFlag;				//特殊标志，保留BIT0：1：厂内模式
	//T_FUNDLT645 fun;//回调函数
}T_DLT645; 

typedef struct TDLT645APPFollow_t   	//链路层结构体
{
	eFOLLOW_FLAG FollowFlag;			//后续帧标志
	BYTE bLastNum;						//list 已处理数据个数
	DWORD dwLastTime;					//储存开始时间的绝对分钟数
	DWORD dwReadTime;//负荷记录读取的时间
	DWORD dwRecordNo;//负荷记录下次开始读的记录序号
	BYTE byLpfBlockNum;//负荷记录剩余点数/剩余块数
	BYTE Buf[17];//用于保存电能数据以便读后续数据时补充数据,[0]:Len [1~16]:Data
}TDLT645APPFollow;

typedef struct TDLT645Sendbuf_t			//645发送缓存 
{
	BYTE *DataBuf;
	WORD DataLen;					//总数据
}TDLT645Sendbuf;

typedef struct TMeteringVaribleTab_t
{
	WORD ID;		//数据标识 DI1 DI2
	WORD Type;		//数据类型 电压 电流 功率等
	WORD Phase;		//相 总 A B C 数据块
	BYTE Lenth;		//长度
	BYTE Dot;		//小数位数
}TMeteringVaribleTab;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern TDLT645APPFollow 	DLT645APPFollow[MAX_COM_CHANNEL_NUM];
extern T_DLT645 			gPr645[MAX_COM_CHANNEL_NUM];
extern TDLT645Sendbuf 		DLT645Sendbuf[MAX_COM_CHANNEL_NUM];
extern __no_init BYTE 		g_645PassWordErrCounter[2];

//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量


//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//PareMeter645.c
//---------------------------------------------------------------
//函数功能: 读出参变量数据
//
//参数:
//                   PortType[in] - 通道
//		    ID[in]      - 传入数据标识
//		    OutBufLen   - 传入缓存长度
//			OutBuf[out] - 传出数据
//
//返回值:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//
//备注:
//---------------------------------------------------------------
WORD GetPro645RequestMeterPara( eSERIAL_TYPE PortType,BYTE *DI, WORD OutBufLen, BYTE *OutBuf );

//dlt645_2007Set_14H.c
WORD DealSetDlt645_2007Data0X14( eSERIAL_TYPE PortType );

//Variable645.c
//---------------------------------------------------------------
//函数功能: 读取变量
//
//参数: 
//                   
//返回值: 
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//
//备注:   
//---------------------------------------------------------------
WORD Get645RequestVariable( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf );
//event645.c
//--------------------------------------------------
//功能描述:     读取645事件接口函数     
//参     数:
//          pDI[in]	    数据标识   
//          OutBuf[in]  输入Buf
//返回值    
//备注内容:     WORD  数据长度 0x8000代表数据出错
//--------------------------------------------------
WORD api_ReadEventRecord645( BYTE* pDI , BYTE* OutBuf );


//Energy.c
//--------------------------------------------------
//功能描述:  读取电能       
//参     数:BYTE Dot[in]		小数位数       
//         BYTE *DI[in]		数据标识                       
//         WORD OutBuf[in]  返回数据        
//         WORD Lenth[in]	读取的长度        
//返回    值:返回状态 成功：8000; 失败：800X           
//备注内容:  无
//--------------------------------------------------
WORD GetRequest645Energy( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf );

//dlt645_2007APPData.c
WORD GetPro645IDData( eSERIAL_TYPE PortType, BYTE Dot, BYTE *ID, WORD OutBufLen, BYTE *OutBuf );
//---------------------------------------------------------------
//函数功能: 通信专用HEX转BCD 一般是组合模式
//
//参数: 
//		  Type[in]
// 			  eBYTE_MODE - 单字节模式
//			  eCOMB_MODE - 组合模式 比如长度为2 两个BYTE组合成一个WORD
// 		  Lenth[in] -  传出数据长度
// 		  Buf[in] - 输入缓存
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL ProBinToBCD645( eTYPE_COVER_DATA Type, BYTE Lenth, BYTE *Buf );

//--------------------------------------------------
//功能描述:  处理升级报文命令
//         
//参数:
//         
//返回值:    DLT645_OK_00--成功 其它--返回失败类型
//         
//备注内容: 
//--------------------------------------------------
WORD ProIap( eSERIAL_TYPE PortType );
//---------------------------------------------------------------
//函数功能: 通信专用HEX转BCD
//
//参数: 
//		  Type[in]
// 			  eBYTE_MODE - 单字节模式
//			  eCOMB_MODE - 组合模式 比如长度为2 两个BYTE组合成一个WORD
// 		  Lenth[in] -  传入数据长度
// 		  Buf[in] - 输入缓存
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL ProBinToBCD( eTYPE_COVER_DATA Type, BYTE Lenth, BYTE *Buf );


//dlt645_2007APP.c
void FactoryInitDLT645( void );
//-----------------------------------------------
// 功能描述  : 645规约预处理
//
// 参数  :    TSerial *p[in]
//
// 返回值:     BOOL （TRUE-需要做转发或者报文重组处理）
// 备注内容  :  无
//-----------------------------------------------
BOOL Pre_Dlt645(TSerial *p);
//-----------------------------------------------
//功能描述  :   初始化698标志
//
//参数  : 	无
//
//返回值:      无
//
//备注内容  :   无
//-----------------------------------------------
void InitDLT645Flag( eSERIAL_TYPE PortType );
//--------------------------------------------------
//功能描述:  698.45上电函数
//         
//参数  :   无
//
//返回值:    无  
//         
//备注内容:  无
//--------------------------------------------------
void PowerUpDlt645(void);

//Demand645.c
//--------------------------------------------------
//功能描述:  读取需量       
//参     数:BYTE Dot[in]		小数位数       
//         BYTE *DI[in]		数据标识                       
//         WORD OutBuf[in]  返回数据        
//         WORD Lenth[in]	读取的长度        
//返回    值:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度    
//备注内容:  无
//--------------------------------------------------
WORD GetRequest645Demand( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf );
//---------------------------------------------------------------
//函数功能: 获取负荷记录数据
//
//参数: 
//			PortType[in]-通讯口
//		    DI[in]     - 传入数据标识
//		    OutBufLen  - 传入缓存长度	
// 			OutBuf[out]- 输出数据缓存                  
//返回值:  
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//备注:   
//---------------------------------------------------------------
WORD GetRequest645Lpf( eSERIAL_TYPE PortType, BYTE *DI, WORD OutBufLen, BYTE *OutBuf );

///////////////////////////////DLT645_2007Link.c/////////////////////////////////////////////////
//--------------------------------------------------
//功能描述:  响应报文处理
//         
//参数:      TYPE_PORT PortType[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
void Proc645LinkDataRequest( eSERIAL_TYPE PortType, BYTE *pBuf );
//--------------------------------------------------
//功能描述:  响应报文处理
//         
//参数:      TYPE_PORT PortType[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
void Proc645LinkDataResponse( eSERIAL_TYPE PortType, WORD wResult);
//-----------------------------------------------
// 功能描述  : 645规约预处理
//
// 参数  :    TSerial *p[in]
//
// 返回值:     BOOL （TRUE-需要做转发或者报文重组处理）
// 备注内容  :  无
//-----------------------------------------------
void ProMessage_Dlt645( TSerial *p );
//---------------------------------------------------------------
//函数功能: 判断下发命令是否广播地址，还有别的地方调用此函数，不能取消
//
//参数: 	pBuf[in] - 传入通信地址
//                   
//返回值:   TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
WORD JudgeBroadcastMeterNo(BYTE *pBuf);
WORD JudgeRecMeterNo_645(BYTE *pBuf);
#endif//#if( SEL_DLT645_2007 == YES )

#endif//对应文件开始的 __DLT_645_2007_STRUCT


