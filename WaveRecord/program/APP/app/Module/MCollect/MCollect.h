//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		
//修改记录	
//----------------------------------------------------------------------
#ifndef __MCOLLECT_H
#define __MCOLLECT_H

#include "cgy.h"

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAXCOUNT_NOACK		            3	//最大无应答次数（用于判断通讯故障）
#define SECURED_TRANS_RN_VERIFY_MAC_EN  0   //级联安全传输（明文+RN）MAC校验使能

//采集标识
#define STS_CHK_CJ_FLAG		            0	//检查采集标识	
#define STS_SEL_MP			            1	//选择采集测量点
#define STS_SEL_CJ_TASK		            2	//选择采集任务(698专用)
#define STS_INIT_MP_CJ		            3	//初始化测量点采集信息
#define STS_SET_MP_DATA_FALG            4	//设置测量点数据采集标识
#define STS_DO_MP_CJ		            5	//数据采集进行中
#define STS_CHK_MP_CJ_FLAG	            6	//检查采集是否结束
#define STS_MP_CJ_OVER		            7	//单个测量点采集结束

#define PROTOCOL_3762_MAX_LEN           255 //3762协议携带报文的最大长度

#define TRANS_BUF_LEN                   100	//单次透传最大长度
#define PROXY_BUF_LEN                   PROTOCOL_3762_MAX_LEN	//代理报文允许的最大长度


/**********************************************
*brief 数据标识
***********************************************/
#define    REQ_ENG698	  0x00000200 		//组合有功总电能(698) 

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
enum 
{
    JLS_IDLE=0,
    JLS_TX,
    JLS_RX,
    JLS_COMPLETE
};

typedef enum
{
	ePROTOCOL_645_97 = 1,
	ePROTOCOL_645_07,
	ePROTOCOL_698
} ePROTOCOL_CLASS;

typedef struct t_SwitchIap
{
	WORD IapFlag;	 // 升级标志
	WORD IapStep;	 // 升级步骤
	WORD IapFrameNo; // 帧序号
	WORD RetryNo;	 // 重试次数
} tSwIapTrans;

typedef struct _TMaster{
	BYTE	*pMPFlag;			//测量点序号有效标识组
	TSerCfg OldSerCfg;			//上次串口配置
	// BYTE	byCJInterval;		//采集间隔	
	// TPortCjInfo	CjInfo;			//端口采集信息
	// DWORD	dwOldCheckTime;		//最近一次检查时间
	// DWORD	dwCheckTime;		//最近一次检查时间
	// DWORD	dwOldReCjTime;		//上次数据重新采集时间
	
	//2.0数据采集
	TTime	Time;	
	WORD	wSetCJFlag;			//见cgy.h
	DWORD	dwLastCJMins;		//最近一次采集时间
	WORD	wEventCjFlag;		//1、2、3类事件采集标识
	BYTE	byCJing;			//当前采集状态	
	BYTE	byReqType;
	TTime	aTime_Data[MAX_REQ_TYPE];	//数据时间
	// TGyInfo	OldGyInfo;			//上次规约信息

	
	WORD	wAppSize;
	//.BYTE	*pApp;				//APP私有空间	
	BYTE	byTaskNo;			//当前采集任务序号(698用0~MAX_FKTASK_NUM)

}TMaster;


#pragma pack(1)

/**********************************************
*brief 
***********************************************/


/**********************************************
*brief DLT645
***********************************************/
typedef struct
{
	DWORD	dwFE;				//唤醒字符			
	BYTE	byS68;				//起始68字符
	BYTE	MAddr[6];			//地址
	BYTE    byE68;				//结束68字符
	BYTE    byCtrl;				//控制字
	BYTE	byDataLen;			//数据域长度
	DWORD	dwDI;				//数据类型
}T645FmTx07;

typedef struct
{
//	DWORD	dwFE;				//唤醒字符			
	BYTE	byS68;				//起始68字符
	BYTE	MAddr[6];			//地址
	BYTE    byE68;				//结束68字符
	BYTE    byCtrl;				//控制字
	BYTE	byDataLen;			//数据域长度
	WORD	wDI;				//数据类型
}T645FmTx97;

typedef struct
{
	BYTE	byS68;				//起始68字符
	BYTE	MAddr[6];			//地址
	BYTE    byE68;				//结束68字符
	BYTE    byCtrl;				//控制字
	BYTE	byDataLen;			//数据域长度
	DWORD   dwDI;
// 	BYTE	byDI0;				//数据类型
// 	BYTE	byDI1;				//数据类型
// 	BYTE	byDI2;				//数据类型
// 	BYTE	byDI3;				//数据类型
}T645FmRx;

typedef struct{
	BYTE	byS68;				//起始68字符
	BYTE	MAddr[6];			//地址
	BYTE    byE68;				//结束68字符
	BYTE    byCtrl;				//控制字
	BYTE	byDataLen;			//数据域长度
}T645FmRxEx;

/**********************************************
*brief DLT698
***********************************************/

#pragma pack()

//-----------------------------------------------
//	      结构体定义
//-----------------------------------------------

//级联发送数据信息(由载波口/维护485收到的数据发送前前存于此处)
typedef struct {
	BYTE    ModeSwitch;			//采集器模式
	BYTE	byValid;
	BYTE    bySourceNo;         //原数据端口
	BYTE    byDestNo;           //级联端口
	BYTE	byGyType;			//0-07规约，1-97规约
//	ComDCB  Dcb;                //端口参数控制
	WORD    wTimeOut;           //超时时间（秒）
	WORD	wTxWptr;			//写指针
	WORD    wRxWptr;       
	BYTE    TransTxBuf[TRANS_BUF_LEN];    //缓冲区
    BYTE    TransRxBuf[TRANS_BUF_LEN];
	BYTE    byDar;              //代理错误类型
	DWORD   dwRxStartTime;
    //为应对代理透传2只表的操作，开辟缓冲区
	BYTE    cacheTransTxBuf[PROXY_BUF_LEN];    //缓冲区
    BYTE    cacheTransRxBuf[PROXY_BUF_LEN];    
	WORD	wCacheTxWptr;			//写指针
	WORD    wCacheRxWptr;  
	BYTE    byCacheDar;               
	//规约相关
	BYTE    proxy_choice;       //代理透传类型
	BYTE    proxy_01_listNum;   //ProxyGetRequestList 服务器数量
	BYTE    proxy_01_oadNum;    //ProxyGetRequestList OAD数量
	DWORD   proxy_oads[5];      
	BYTE    proxy_listnum_para; //过程变量
	BYTE    Addr[6];
	BYTE    ClientAddr;
	BYTE    PIID;
	BYTE	SafeMode;
}TJLTxBuf;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern tSwIapTrans SwitchIapTrans;
extern TJLTxBuf JLTxBuf;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void JudgeBaudChange(BYTE SerialNo, DWORD Baud);
void Tx_Collect_Meter(BYTE SerialNo, ePROTOCOL_CLASS ePClass, BYTE *pAddr, DWORD dwID, BYTE byEnumBaud);
// BOOL SwitchToNextTask(TPort *pPort, DWORD dwCheckTime, BYTE *pbyTaskNo);
// BOOL _SwitchToNextMP(TPort *pPort,WORD *pwNo);
void  Min2Time(DWORD dwMins,TTime *pT);
//WORD fcs16(BYTE *cp, WORD len);
WORD MakeFrameHead(TSA *sa,BYTE *pBuf,BYTE byClientAddr);
WORD Add698FrameTail(BYTE byCtrl,WORD wFrameLen,BYTE *pBuf);
BYTE FindMeterMPNo(BYTE *pMeterAddr);
void JLTxBufInit(TJLTxBuf *pJL);
BOOL ProxyGetRequestList( BYTE Ch ,TJLTxBuf *pJL);
BOOL ProxyGetRequestRecord( BYTE Ch ,TJLTxBuf *pJL);
BOOL ProxyTransCommandRequest( BYTE Ch ,TJLTxBuf *pJL);
#endif//#ifndef TOPO_H
