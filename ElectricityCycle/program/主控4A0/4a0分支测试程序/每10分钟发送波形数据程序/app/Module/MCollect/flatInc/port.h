#ifndef __PORT_H__
#define __PORT_H__
#ifdef __cplusplus
 #if __cplusplus
  extern "C" {
 #endif
#endif
#include "flags.h"
#include "usrtime.h"

#define MPT_SERIAL		1		//串口
#define MPT_ETHERNET	2		//以太网
#define MPT_376_2		3		//376.2(载波/无线)
#define MPT_376_3		4		//376.3(GPRS)
	  
/*底层驱动的工作状态*/
#define MPS_IDLE		0		//处于空闲状态
#define MPS_RESET		1		//处于复位延时状态
#define MPS_RTS_ON		2		/*处于开RTS延时状态*/ 
#define MPS_TX			3		/*处于发送状态*/
#define MPS_RTS_OFF		4		/*处于关RTS延时状态*/

#define	MODULE_CHECK	0		//模块检测
#define	MODULE_INIT		1		//模块初始化
#define	MODULE_READY	2		//模块就绪
#define	MODULE_LINKING	3		//模块连接
#define	MODULE_ONLINE	4		//模块在线
#define	MODULE_ONOFF	5		//模块开关机
#define	MODULE_WAITING  6		//模块等待 服务器模式下暂停

//GPRS模块信号强度临界值定义
#define MIN_SINGNAL		5
#define MAX_SINGNAL     38
	  
//响应AT模块的类型
#define MT_GPRS			1		//GPRS
#define MT_CDMA			2		//CDMA
#define MT_PSTN			3		//PSTN
#define MT_LTE			4		//4G
	  
	  
//串口驱动配置
// typedef struct _TDrvCfg{		
// 	FB_H   ChipOpen;			//初始化函数
// 	BYTE   byNo;				//Uart、USart的组内对应序号，如果终端中用了2个UART ,则序号分别为0,1和具体UartX无关
// 	char   szDevName[30];		//驱动设备名	
// }TDrvCfg;


//串口相关配置
typedef struct _TSerCfg{
	DWORD dwBaud;//波特率
	BYTE  byFlags;//停止位，校验位等信息
		//7,6: 00：1位停止位 01：2位停止位 10：1.5位停止位
		//5,4:  00:无效验  10:偶校验 01：奇校验  
		//1,0: RTS控制模式
	BYTE  byCharSize;//字符位数
	WORD wRTSOnDelay;
	WORD wRTSOffDelay;
}TSerCfg;

// typedef struct _TGyInfo{
// 	FB_HH	fGetCfg;			//获得规约配置信息
// 	FB_H	fInit;				//初始化规约分配专用空间
// 	FB_H	fTxMonitor;			//发送处理
// 	FD_HHD	fSearchOneFrame;
// 	FV_H	fRxMonitor;			//接收处理	
// }TGyInfo;

//缓冲区
typedef struct _TTx_Rx{
	WORD wSize;
	WORD wOldPtr;
	WORD wRptr;
	WORD wWptr;
	BYTE  *pBuf;
	DWORD dwBusy;
}TTx,TRx;

typedef union _UPortCfg{
   TSerCfg   SerCfg;
}UPortCfg; /*该端口的配置参数*/

//路由模块芯片类型
#define MCHIP_NULL		0		//未检测到
#define MCHIP_ES		1		//东软芯片
#define MCHIP_TC		2		//鼎信芯片
#define MCHIP_XC		3		//晓程芯片
#define MCHIP_RC		4		//瑞斯康芯片
#define MCHIP_MI		5		//弥亚微芯片
#define MCHIP_SS		6		//赛世特芯片
#define MCHIP_LM        7       //力合微芯片
#define MCHIP_RL        8       //锐拔无线
#define MCHIP_FC        9       //友讯达无线
#define MCHIP_SR        10      //桑锐无线
#define MCHIP_HR        11      //宏睿无线
#define MCHIP_MX        12      //麦希无线
#define MCHIP_SY        13      //沈阳无线
#define MCHIP_ZC        14      //中宸泓昌
#define MCHIP_UNKNOWN	0xFFFF	//未知芯片


// typedef struct _TPortCjInfo{
// 	WORD	wMetNum;			//端口下电表个数
// 	BYTE	byWorkStatus;		//当前工作状态
// 	WORD	wMetNum_OK;			//采集成功电表数量
// 	WORD	wVipMetNum_OK;		//重点表采集成功数量
// 	TTime	STime;				//开始抄表时间
// 	TTime	ETime;				//结束抄表时间
// }TPortCjInfo;

#define	MAX_SGY_NUM		3

typedef struct _TCj698Ctrl{
	BYTE    byTxRowNum;
	BYTE	CSDRptr;			//698的CSD读写指针，用于判断是否结束
	BYTE	CSDWptr;
	// WORD	wRxDataNum;			//接收应答数据个数
	// WORD	wOKDataNum;			//接收正常数据个数
	WORD    wTransRptr;         //透明方案的报文读指针，用于判断是否结束
	BYTE    byTransNo;           //透明方案号
	// DWORD   dwTransInfo;        //透明方案信息（方案内容集合索引2字节 + 帧序号2字节）
	// DWORD	Offset_Frz;	
	// DWORD	Offset_Frz_Bak;	
	// DWORD	dwFrzTimeMark;		//冻结时标	用来判断电表时间
}TCj698Ctrl;

typedef struct _TCurvCjCfg{
	BYTE	byInit;				//0x5A代表已经初始化
	WORD	wNum;
	WORD	wInterval;
	DWORD	dwStart;
	BYTE    byCJInc;			//非698规约的采集增加步聚
	BYTE	byCJStep;
	WORD	wNo;				//当前个数		
}TCurvCjCfg;

typedef struct _TReCjCfg{
	BYTE	byInit;				//0x5A代表已经初始化
	WORD	wNum;
	WORD	wInterval;
	DWORD	dwStart;
	BYTE    byReCJInc;			//非698规约的采集增加步聚
	WORD	wReCJStep;
	WORD	wNo;				//当前个数		
}TReCjCfg;

typedef struct _TGyRunInfo{
	WORD	wMPSeqNo;			//采集序号(切换时置值)
	TCurvCjCfg CurvCjCfg;
	TReCjCfg ReBuCJCfg;
	// DWORD	dwLastCjTime;		//最近一次采集时间
	// DWORD	dwCJSecs;
	BYTE	byCJStatus;			//采集状态	
	BYTE	Addr[6];			//表计通信地址
	TSA		sa;
	BYTE	Flag[(MAX_FLAGNO+7)/8];
	BYTE	byTxIDFlag;			//记录上一次07发送的msf
	BYTE	byTxCurvSource;		//记录发送的曲线类型，实时转或抄表内 0x55-表内
	// TGyInfo	GyInfo;				//规约配置接口
	// WORD	wOldNo;
	BYTE	byReqType;			//当前请求类型
	// BYTE	byTDRecSave;		//是否需要采集停电事件
	BYTE	byReqDataValid;		
	BYTE	byGyStep;			//规约执行步聚
	TCj698Ctrl cj_698;			//698采集控制
	WORD	wTxCount;
	WORD	wOldTxCount;
	WORD	wRxCount;		
	BYTE	byHisDataOffset;
	HPARA	hGyDB;				//规约数据库
	// BYTE	byReqOK;
	DWORD	dwTaskTime;			//任务执行时间
	BYTE	*psch698;			//698的方案	
	// BYTE    by645ReportFlag;
	BYTE	*pApp;				//规约的应用程序空间
}TGyRunInfo;

typedef struct _TPort{
	//1、基本配置信息
	DWORD	dwID;
	// BYTE	byPhyNo;
	// BYTE	byType;				//端口类型(网络、串口)
	// UPortCfg Cfg;
	// BYTE   RTSON;				//RTSON的状态
	// BYTE   RTSOFF;				//RTSOFF的状态
	//2、运行信息
	// WORD	wIdleMins;			//空闲分钟数	
	BYTE	byOpen;				//打开标识
	BYTE	byMode;				//0 上行规约端口，非0-表计端口
	// BYTE	RetryCnt;			//重发次数
	BYTE	byStatus;			//端口状态
	HPARA	hApp;
	DWORD	dwRunCount;			//运行计数	
	// DWORD	dwCtrlTime;			
	// DWORD	dwFrameOkTime;		//最后一次收到完整帧时间
	// BYTE	byGyNo;				//当前处理规约号(从规约)
	// BYTE	byMGyNo;			//登录规约号(从规约)
	// BYTE	bySGyNum;			//从规约个数
	// DWORD	aSGyApp[MAX_SGY_NUM];//端口下支持的从规约最大个数
	// BYTE	aSGyNo[MAX_SGY_NUM];//从规约号列表	
	//应用收发缓冲区（串口模式用于中断级别）	
	WORD	wMetNum;			//端口下电表个数
	// BYTE	byEnTran;			//是否允许透传
	// DWORD	dwTranRunCount;		//透传计数
	TTx		Tx;					
	TRx		Rx;					//接收
	//端口描述符
	// INT		nfd;
	TGyRunInfo GyRunInfo;			//电表采集信息
	// FB_HHDD	fAddrDaemon;		//地址监控
}TPort;

//串口配置接口
#define IOCTRL_INVALID	0xFF
#define	IOCTRL_RESET	0		//复位端口
#define IOCTRL_SETRATE	1		//设置通信波特率
#define IOCTRL_STARTTX	3		//启动底层发送
#define IOCTRL_GETRATE	4		//获得接口当前配置信息
#define IOCTRL_GETTXSTATUS 5	//获得接口当前发送信息
#define IOCTRL_RTSON	6		//打开RTS
#define IOCTRL_RTSOFF	7		//关闭RTS
#define IOCTRL_CONNECT	8		//建立连接
#define IOCTRL_DISCONNECT	9	//关闭连接
#define IOCTRL_CLOSE	10		//关闭模块
#define IOCTRL_STS_376	11		//376.3通信状态
#define IOCTRL_CALLCB	12		//点名抄表
#define IOCTRL_SEARCH	13		//启动搜表
#define IOCTRL_STOPCB	14		//停止抄表
#define IOCTRL_BROADSETTIME	15	//广播校时
#define IOCTRL_REQROUTER 16		//唤醒路由
#define IOCTRL_RECOLLECT 17		//重启抄表


TPort *pGetPort(BYTE byNo);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif
#endif
