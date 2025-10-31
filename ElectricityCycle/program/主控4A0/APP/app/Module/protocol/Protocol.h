/*****************************************************************************/
/*文件名：Protocol.h
 *Copyright (c) 2008 烟台东方威思顿电气有限公司计量产品开发组
 *All rights reserved
 *
 *创建人：魏灵坤
 *日  期：2008-9-21
 *修改人：魏灵坤
 *日  期：2009-3-26
 *描  述：DTSD/DSSD178全电子式多功能电能表通讯规约头文件
 *
 *版  本：
 *说  明：加入各个函数声明，可供LCD、事件处理、负荷曲线、冻结调用，只要数据是规约格式都可以调用此函数
 *		  如果函数入口参数是 BYTE *ProBuf，则ProBuf是规约缓冲TSerial中的ProBuf，
 *		  如果函数入口参数是 BYTE *pBuf，则 pBuf取决于调用时传输的数据
 */
/*****************************************************************************/

#ifndef __PROTOCOL_INCLUDE
#define __PROTOCOL_INCLUDE

	//通信缓冲区最大长度定义
	#define	MAX_PRO_BUF			(512)	//帧长度 通讯缓存apdu层最大尺寸
	#define EXTRA_BUF			(30)	//额外的数据buf 用来防止数据buf使用
	#define MAX_APDU_SIZE		2000   //apdu层最大尺寸  
	#define MAX_RECE_DATA		(MAX_PRO_BUF)
	#define MAX_SEND_DATA		MAX_PRO_BUF

	#define MAX_PRO_BUF_REAL	(8192)


	//接受超时时间，单位，毫秒，也就是在100毫秒后若没有接受到新的数据，则清接受缓冲, 已放到保护区
	//#define MAX_RX_OVER_TIME	100
	//发送等待时间，单位毫秒，在等待毫秒后，切换到接受状态, 已放到保护区
	//#define MAX_TX_WAIT_TIME	40//20  查已不用
	//485干扰检测时间，单位毫秒，收到启动信号，若没有有效数据，在此时间内切换到232状态, 已放到保护区
	//#define WATCH_485_TIME		100  查已不用
	//232禁止检测485的延时，单位毫秒, 已放到保护区
	//#define MAX_232_TIME		30000   查已不用
	//接收到发送的延时，毫秒为单位，接收到有效帧后，延时此时间在发送（认为程序处理不耗费时间）, 已放到保护区
	//#define RECE_TO_SEND_DELAY 	25
	#define CR_OVER_TIME		1440
	#if(CR_OVER_TIME < 120)
	#error"配置错误"
	#endif
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

typedef struct TSerial_t
{

	// //公共缓冲，在RAM中必须是偶地址
	// BYTE ProBuf[MAX_PRO_BUF+EXTRA_BUF];

	//公共缓冲，在RAM中必须是偶地址
	BYTE ProBuf[MAX_PRO_BUF_REAL+EXTRA_BUF];

	WORD ProStepDlt645;
	//Dlt645规约报文在Serial[].ProBuf中的开始位置
	WORD BeginPosDlt645;

	//645报文数据域数据长度
	WORD Dlt645DataLen;

	#if(SEL_DLT698_2016_FUNC == YES )
	//DLT698_45规约接收过程计数
	WORD ProStepDlt698_45;
	//DLT698_45规约报文在Serial[].ProBuf中的开始位置
	WORD BeginPosDlt698_45;
	WORD wLen;
	BYTE Addr_Len;//通信报文中服务器地址SA长度，地址长度
	#endif
	
#if (SEL_1376_2_FUNC == YES)
	WORD ProStep3762;
	WORD BeginPos3762;
	WORD w3762Len;
#endif
	WORD RXWPoint;
	WORD RXRPoint;

	WORD TXPoint;
	WORD SendLength;//需要发送的数据长度

	//串口监视时间
	BYTE WatchSciTimer;

	BYTE BroadCastFlag;//无需应答标志

	BYTE BeginPoint;//无效值为0xff，每次用完后赋0xff

	//对485通讯的几个时间处理，全部采用大循环计数方式
	//接受超时时间，若收到最后一个字节后，等待此时间没有新的数据收到，无论是接受到多少数据，都要初始化指针。
	WORD RxOverCount;

	//接收到发送的延时
	WORD ReceToSendDelay;

	BYTE byLastRXDCon;//上次通信电表应答控制码
	
	//bit0 相应通道有修改波特率的操作
	BYTE OperationFlag;

	//发送到发送的延时控制（主动端口使用）
	WORD SendToSendDelay;
}TSerial;

//645结构
typedef struct TDlt645Head_t
{
	BYTE Sync1;
	BYTE ID[6];
	BYTE Sync2;
	BYTE Control;
	BYTE Length;
}TDlt645Head;

//698.45安全械
typedef struct  TSafeModeParaSub_t
{
	WORD OI;//例如OI为0x0010 低字节保存:0x10 高字节保存:0x00 wlk
	WORD wSafeMode;//例如wSafeMode为0x8000，低字节保存:0x00,高字节保存0x80 wlk
}TSafeModeParaSub;

#pragma pack(1)
typedef struct TSafeModePara_t
{
	BYTE Num;		                         		//显示安全模式参数条数num
	BYTE Flag;   	                            	//安全模式参数flag:0-不启用 1-启用
	TSafeModeParaSub Sub[MAX_SAFE_MODE_PARA_NUM];
	DWORD CRC32;              						// 校验
}TSafeModePara;

typedef struct TTimeBroadCastPara_t
{
	WORD TimeMinLimit;		                         	//广播校时最小响应限制 单位分钟
	WORD TimeMaxLimit;									//广播校时最大响应限制 单位分钟
	DWORD CRC32;              							// 校验
}TTimeBroadCastPara;

typedef struct
{
	BOOL AllowUnknownAddrTrans;	// 允许未知地址透传到485I
	BYTE Baud;					// 转发到485端口使用的波特率
	BYTE Timeout;				// 透传超时时间配置(s)
	DWORD CRC32;				// 校验
}MessageTransPara_t;

//协议上需要保存到EEPROM的参数
typedef struct TProtocol_Para_t
{
	#if ( SEL_DLT698_2016_FUNC == YES  )
	TSafeModePara SafeModePara;
	TTimeBroadCastPara TimeBroadCastPara;
	#else
	BYTE byNoUse;	
	#endif
	TCanBusPara CanBusPara;
	MessageTransPara_t MessageTransPara;
}TProSafeRom;
#pragma pack()

typedef enum
{
	eCLEAR_METER   = 0x1111,		//电表清零
	eCLEAR_FACTORY = 0x2222,		//电表初始化
	eCLEAR_EVENT   = 0x4444,		//事件清零
	eCLEAR_MONEY   = 0x6666,		//钱包初始化或参数预置卡
	eCLEAR_PARA    = 0x7777,		//初始化参数（不初始化冻结、事件、显示列表）
	eCLEAR_DEMAND  = 0x8888,		//需量清零
}eCLEAR_METER_TYPE;

typedef enum
{
	ePROTOCO_698 = 0,   //698协议
	ePROTOCO_645,       //645协议
}ePROTOCO_TYPE;

extern TSerial Serial[MAX_COM_CHANNEL_NUM];

extern BYTE Reply;//规约通讯是否要应答 TRUE--要应答 FALSE--不要应答
extern BYTE g_byUsePasswordP2;//P2=01H:数据域：控制命令执行状态字 P2≠01H：控制命令按以前命令应答
extern WORD g_wRelayReplyStauts;//控制命令应答状态字
extern WORD FactoryTime;//打开厂内模式时间
extern WORD LowFactoryTime;//打开低等级厂内模式时间

extern BYTE APDUResponseBuf[MAX_COM_CHANNEL_NUM][MAX_APDU_SIZE+EXTRA_BUF];
extern BYTE bBleChannel;
extern DWORD JTOverTime;//静态超时时间
extern MessageTransPara_t MessageTransParaRam;
//新老板公共函数
void InitPoint(TSerial * p);
void api_InitSubProtocol(WORD);
//-----------------------------------------------
//功能描述  :   厂内初始化协议参数
//参数  : 	无
//返回值: 
//备注内容  :  无
//-----------------------------------------------
void FactoryInitProtocol( void );


//新板初始化串口通讯指针
void api_InitSciStatus( BYTE i );
//打开接收，使处于接收状态
void api_EnableRece(TSerial * p);
//禁止接收
void api_DisableRece(TSerial * p);

BYTE CalRXD_CheckSum(WORD ProtocolType, TSerial * p);
//把数据从报文开始位置 向 以p->ProBuf[0]开始处移
void DoReceMoveData(WORD ProtocolType, TSerial * p);
//645规约处理函数
WORD DoReceProc_Dlt645(BYTE ReceByte, TSerial * p);


//取得缓冲是TSerial中的哪个串口
//返回: FALSE--不是串口缓冲 其它--返回的是串口Serial[]的地址
TSerial *GetSerial(BYTE *pBuf);

//---------------------------------------------------------------
//函数功能: 判断接收报文中的通信地址是否全部是给定的数据
//
//参数:    bySpecialData[in]:
//			99--判断下发命令是否广播地址
//			88--判断下发命令是否是东方扩展的6个88，即12个8
//        pBuf[in] - 传入通信地址
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
WORD JudgeRecMeterNoOnlyOneData(BYTE bySpecialData,BYTE *pBuf);


//#if( SEL_DLT645_2007 == YES )
//以下新规约部分 新645 新645(DL/T645-2007) 和  有关
void InitDlt645_2007(void);


WORD JudgeRecMeterNo(BYTE *pBuf);
void Proc645( WORD i );
void api_ProtocolTask(void);
void api_PowerUpProtocol( void );

//--------------------------------------------------
//功能描述:     申请数据填充Buf
//
//参数  :     BYTE Ch[in]                     通道选择
//          ePROTOCO_TYPE ProtocolType[in]  协议类型
//
//返回值:      BYTE* buf指针
//
//备注内容:  无
//--------------------------------------------------
BYTE* AllocResponseBuf( BYTE Ch, ePROTOCO_TYPE ProtocolType );

//-----------------------------------------------
//函数功能: 扩展规约读函数
//
//参数:		pBuf[in]	0--扩展DI0 1--DI1 2--0XDF 3--0XDB	
//						
//返回值:	返回读取数据长度
//		   
//备注:规约见《电表软件平台扩展规约》
//-----------------------------------------------
WORD ReadFactoryExtPro(BYTE *pBuf);

//-----------------------------------------------
//函数功能: 扩展规约写函数
//
//参数:		pBuf[in]	0--扩展DI0 1--DI1 2--0XDF 3--0XDB	
//						4,5,6,7--相对分钟数
//返回值:	返回值bit15为1为错误，其他为正确
//		   
//备注:规约见《电表软件平台扩展规约》
//-----------------------------------------------
WORD WriteFactoryExtPro(BYTE *pBuf);

//---------------------------------------------------------------
//函数功能: 设置清零标志
//
//参数: 
//			eCLEAR_METER_TYPE - 清零标志位
// 					eCLEAR_METER   = 0x1111,	//电表清零
//					eCLEAR_FACTORY = 0x2222		//电表初始化
//					eCLEAR_EVENT   = 0x4444,	//事件清零
//					eCLEAR_MONEY   = 0x6666,	//钱包初始化或参数预置卡
//					eCLEAR_DEMAND  = 0x8888,	//需量清零
// 			BYTE DelayTime_500ms - 清零延时时间 500ms为单位，设置为1表示延时500ms
//                   
//返回值:   TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL api_SetClearFlag( eCLEAR_METER_TYPE ClearType, BYTE DelayTime_500ms );


//--------------------------------------------------
//功能描述:     4G模块复位
//
//参数  :     
//
//返回值:      
//
//备注内容:  无
//--------------------------------------------------
void api_ResetModule4g( void );
#endif//对应文件最前面的 #ifndef __PROTOCOL_INCLUDE


