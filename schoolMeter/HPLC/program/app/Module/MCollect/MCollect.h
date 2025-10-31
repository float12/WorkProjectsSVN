//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		
//修改记录	
//----------------------------------------------------------------------
#ifndef __MCOLLECT_H
#define __MCOLLECT_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define LLBYTE(w)		((BYTE)(w))  //取一个DWORD的最低字节
#define LHBYTE(w)		((BYTE)((WORD)(w) >> 8))//取一个DWORD的次低字节
#define HLBYTE(w)		((BYTE)((DWORD)(w) >> 16))//取一个DWORD的次高字节
#define HHBYTE(w)		((BYTE)((DWORD)(w) >>24))//取一个DWORD的最高字节
#define LOBYTE(w)       ((BYTE)(w))
#define HIBYTE(w)       ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define HIWORD(l)		(WORD)((DWORD)(l) >> 16)//取一个DWORD的高WORD

#define ADDR_BITS	0x0F		//地址掩码

/**********************************************
*brief 数据标识
***********************************************/
#define    REQ_UIK	   	  0x04001F00		//设置电压电流系数
#define    REQ_RES        0x04001F01        //读取负荷辨识结果
#define    REQ_TIME       0x04001F20        //设置时间
#define    REQ_ENG698	  0x00000200		//组合有功总电能(698)
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct t_SwitchIap
{
	WORD IapFlag;	 // 升级标志
	WORD IapStep;	 // 升级步骤
	WORD IapFrameNo; // 帧序号
	WORD RetryNo;	 // 重试次数
} tSwIapTrans;

//APDU定义
typedef enum APDU_TYPE{
	ATE_NULL = 0,
		GET_Request=0x5,			//读取请求
		SET_Request,				//设置请求	
		ACTION_Request,				//操作请求
		REPORT_Response,			//上报应答
		PROXY_Request,				//代理请求
		Security_Request=0x10,		//安全认证请求
		Security_Response=144,		//安全认证响应
} APDU_TYPE;

typedef struct  
{
	BYTE	byCtrl;				//控制域
	BYTE	byAPDU;				//APDU
	BYTE	byPIID;				//PIID
	BYTE	byCA;				//客户机地址
	BYTE	byType;
	BYTE    bySecurity;         //是否安全请求
	BYTE	byRec;				//记录数	
	DWORD	dwDI;				//数据标识
	BYTE	byNum;				//OAD个数
	DWORD	dwOAD[15];			//OAD列表
}TTransPara;

//级联发送数据信息(由载波口/维护485收到的数据发送前前存于此处)
typedef struct {
	BYTE	byValid;
	BYTE    bySourceNo;         //原数据端口
	BYTE    byPortFlag;         //级联端口
	BYTE	byGyType;			//0-07规约，1-97规约
	WORD	wRptr;				//读指针
	WORD	wWptr;				//写指针
	BYTE    byBaudSelect;       //波特率选择控制
	BYTE    TransBuf[256];      //缓冲区	
    TTransPara transPara;
}TJLTxBuf;

typedef struct _TGetPara{
	BYTE *pTxBuff;
	WORD wBufLen;
	WORD wNum;
	BOOL bUseSafeMode;
	BYTE Rand[16];	
	BYTE byType;	
}TGetPara;

typedef struct {
    
	WORD wFlagValid;
	WORD EventCount[8];//0保护次数、1保护功能投退、2闸位变化、3异常告警、4失复电、5温度告警
}TSYSData;

typedef struct {
	
	DWORD dwSaveTime;		//存盘时间	
	DWORD dwYCFlagValid;	//遥测数据有效标识
	DWORD YC[7];			//0~2A~C电压、3~5A~C电流、6有功功率
    DWORD dwCIFlagValid;    //电能数据有效标识
	DWORD CI[2];            //0正有、1反有
	
	//应对电科院台体窃电事件及超差事件，不适用现场
    DWORD TestFlagVaild;
	DWORD TestData[16];     //暂存连续数据(最近16个点)
	DWORD TestEngFlag;
	DWORD TestEngData[16];  //暂存连续数据(最近16个点)
	DWORD RR[16];           //每个数据点的误差系数
	BYTE  RRCount;
}TMetData;

typedef struct {
	
	TMetData mpData[SEARCH_METER_MAX_NUM];
	DWORD JcTestFlagValid;
	DWORD JcTestData[16]; //暂存交采数据
	DWORD JcTestFlagEngValid;
	DWORD JcTestEngData[16]; //暂存交采数据
}TMPSData;

typedef struct _TMCJCtl{
	DWORD m_OldCjTime;  //上次采集时间
	BYTE  m_CjNo;       //当前采集电表序号
	BYTE  m_RetryNum[SEARCH_METER_MAX_NUM]; //重发次数
	WORD  m_CjFlag[SEARCH_METER_MAX_NUM];     //采集标识
}TMCJCtl;

#pragma pack(1)

/**********************************************
*brief 
***********************************************/
typedef struct _TTime{
    WORD	wYear;	/*年*/
    BYTE    Mon;	/*月*/
    BYTE    Day;    /*日*/
    BYTE    Hour;   /*时*/
    BYTE    Min;	/*分*/
    BYTE    Sec;	/*秒*/
    WORD    wMSec;	/*毫秒*/
}TTime;

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
typedef struct _TSA{
	union{
		BYTE	AF;
		struct{
			BYTE	len	 : 4;
			BYTE	log_addr : 2;
			BYTE	type : 2;
		};
	};
	BYTE addr[16];	// 地址域
}TSA;

typedef struct tag_DATETIME_S{
	WORD	year;		// 年
	BYTE	month;		// 月
	BYTE	day;		// 日
	BYTE	hour;		// 时
	BYTE	minute;		// 分
	BYTE	second;		// 秒
}DATETIME_S;

//不带帧尾的帧定义
typedef struct OBJ_EXCEPT_TAIL{
	BYTE  byStart;				//启动字符0X68
	WORD  wFmLen;				//帧长
	BYTE  byCtrl;				//功能码C
	BYTE  byAddrF;
	BYTE  _byAddrStart;
}OBJ_EXCEPT_TAIL;

typedef struct OBJ_TAIL{
	WORD wCheckCS;				//校验和
	BYTE byEnd;					//结束符
}OBJ_TAIL;

typedef struct _TFrame698_FULL_BASE{
	OBJ_EXCEPT_TAIL *pFrame;
	BYTE *pDataAPDU;
	TSA  *pObjTsa;
	OBJ_TAIL *pTail;
}TFrame698_FULL_BASE;

//////////////////////////////////////////////////////////////////////////
typedef struct _TOAD_DI 
{
	DWORD	dwOAD;
	DWORD	dwDI;
}TOAD_DI;

typedef struct
{
	BYTE	byS68;				//起始68字符
	BYTE	Len[2];				//长度
	BYTE    byCtrl;				//控制字
}T698Head;

typedef struct
{
	BYTE	byHCS;				//校验
	BYTE	byLCS;				//校验
}T698HCS;					//帧头校验

typedef T698HCS T698FCS;		//整帧校验

//////////////////////////////////////////////////////////////////////////

typedef struct _T_DIINFO 
{
	BYTE	byFlag; //0（单一标识） 1（可变标识，字节为FF 代表 需二次判断）
	DWORD	dwDI;
}T_DIINFO;

#pragma pack()

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern tSwIapTrans SwitchIapTrans;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
BOOL objSmockRead( TFrame698_FULL_BASE *pAfnBase, BYTE *Buf );
WORD gdw698buf_OAD(BYTE *pBuf,DWORD *pdword,TRANS_DIR td) ;
WORD gdw698buf_TSA(BYTE *pBuf,TSA *psa,TRANS_DIR td,BOOL bAddHead) ;
DWORD MDW(BYTE HH,BYTE HL,BYTE LH,BYTE LL);
WORD MW(BYTE Hi,BYTE Lo);
void GetSwIapData(void);
void api_TxSwitchIap(BYTE Type);
// WORD ReadEventCount(DWORD dwOAD);
#endif//#ifndef TOPO_H
