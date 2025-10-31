#ifndef _CGY_H_
#define _CGY_H_
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
  #include "__def.h"

#include "paradef.h"
#include "usrtime.h"
#include "port.h"

#define TIME_ONDRIVER	20		//驱动扫描间隔
#define TR_GY			0		//寄存器号
//规约接收返回码
#define FM_SHIELD		0xFFFF0000//控制码屏蔽字
#define FM_OK			0x00010000//检测到一个完整的帧
#define FM_ERR			0x00020000//检测到一个校验错误的帧
#define FM_LESS			0x00030000//检测到一个不完整的帧（还未收齐）

#define MGYM_MASTER		1		//问答式(电表规约)
#define MGYM_SLAVE		2		//被动式(主站通信规约)
#define MGYM_SYSMASTER	3		//主动式(主站通信规约)

//协议配置信息
// typedef struct _TGyCfg{
// 	WORD  wTxdIdleTime;			//发送空闲时间
// 	WORD  wCommIdleTime;		//字符间最大间隔
// 	DWORD  dwRxDelayMax;		//接收到第一个字符的最长等待时间
// 	WORD  wGyMode;				//规约模式
// 	WORD  wAPPSize;				//APP空间大小(主规约时用于判断最大共享APP空间大小设置)	
// }TGyCfg;

// typedef struct _TGy{
// 	BYTE	byNo;				//当前操作端口序号
// 	DWORD	dwPortFlag;			//端口有效标识
// 	HPARA	hAppEx;				//扩充APP管理
// 	TTime	Time;				
// 	DWORD	dwOldSearchTime;	//上次搜表时间
// 	DWORD	dwOldCurvTime;
// 	DWORD	dwCount;
// 	DWORD	dwOldEventCheck;	//事件检测
// }TGy;

//电表实时数据
typedef struct _TMeter_Real{
	DWORD DD[57];				//0~39 正有、反有、正无、反无、一~四象限、40~43 A相正反有无、44~47 B相正反有无、48~51 C相正反有无、52~56组合有功 
	BYTE  DDFlag[8];	
	long  YC[29];				//0~2A~C电压、3~5A~C电流、6~9总及A~C有、10~13无功率、14~17总及A~C功率因素、18零序电流、19~21A~C电压相位角、22~24A~C电流相位角、25~28视在功率
	BYTE  YCFlag[4];	
	TXL2  XL[20];				//0~19 正有、反有、正无、反无需量
	BYTE  XLFlag[3];	
	DWORD XL1[2];				//当前有功需量 当前无功需量（均有符号）
	BYTE  XLFlag1;
	DWORD Other[2];				//0~电表时间,冻结时间
	BYTE  OtherFlag[1];	
	
	BYTE  XLTmpFlag[3];			//需量临时有效标识，用于区分需量数据有效
	DWORD PayInfo[4];			//预付费信息/*PayInfo[3]*/ 增加透支金额
	BYTE  byMetStus[16];
	BYTE  PayInfoFlag;			//预付费标识	
}TMeter_Real;

//电表曲线数据(不能超过128字节)
typedef struct _TMCurv{
	DWORD DD[8];				//正有、反有、正无、反无、一~四象限
	BYTE  DDFlag;
	long  YC[18];				//0~2A~C电压、3~5A~C电流、6~9总及A~C有、10~13无功率、14~17总及A~C功率因素
	BYTE  YCFlag[3];
	DWORD XL[2];                //有功需量 无功需量   //..长度改变？？？？！！！！
	BYTE  XLFlag;
}TMCurv;

typedef union _UMeterData{
	TMeter_Real	Real;	
	BYTE Buf[2048];				//参数存贮缓冲区
}UMeterData;

#define REQ_REAL		0		//实时
#define REQ_CURV		1		//曲线
#define REQ_DFRZ		2		//日冻结
#define REQ_MFRZ		3		//月冻结
#define REQ_CURVEX		4		//曲线补抄
#define REQ_SETTLEDAY   5       //结算日冻结
#define MAX_REQ_TYPE	6		

#define REQ_EVENT_0		7	    //定期采集

//以下是不需要采集，只需要根据标识存贮的类型
#define REQ_AUTODFRZ	16		//自动抄表日冻结(由实时数据采集存贮，无专用循环抄读)
#define REQ_ENMCJ		17		//上月冻结数据允许采集标识
#define REQ_DFRZEX		18		//日冻结终端
#define REQ_MFRZEX		19		//月冻结终端
#define REQ_DFRZDX		20		//日冻结断相

typedef struct _TGyDbase{
	TTime	Time;
	UMeterData	Data;	
	WORD  data_offset;			//缓冲区数据偏移
	WORD  data_offsetEx[3];		//缓冲区数据偏移曲线召唤用	
	// BYTE  byCurvInterval;		//曲线的步长
	// BYTE  byCurvPointNum;		//曲线点数
	TTime DataTime;				//数据起始时间
	// BYTE  byOffset;	
	// DWORD  dwFreshBufFlag;		//BIT0-费率变化，BIT1-编程时间更改，BIT2-抄表日变更，
								//BIT3-电能表脉冲常数更改,BIT4-互感器倍率更改,BIT5-最大需量清零
								//BIT8-磁场干扰发生改变,BIT9-开表盖，BIT10-开端纽盖,BIT11-电能表状态字变位
								//BIT12-编程次数更改，BIT13-需量清零次数更改,BIT14-断相次数更改,BIT15-失压次数更改 BIT16-校时时间变更
								//BIT31-首次刷新需要更新	
	TMeterRealCI RealCI;
	TCVXL CVXl;
}TGyDbase;

// void SethGy(HPARA hGy);
// HPARA GethGy(void);
HPARA GethPort(void);
BYTE GetPortNo(void);
HPARA GethDB(void);
// HPARA GethGyApp(void);
TTx* GetTx(void);
TRx *GetRx(void);
HPARA GetRxFm(void);
// BOOL SwitchToAddr(HPARA hPort,BYTE *Addr,BYTE byAdrLen);
// void OnDriver(TPort *pPort);
// void TxDirect(TPort *pPort);
// void PortTxClear(TPort *pPort);
// void PortRxDirect(TPort *pPort);
// BOOL GetGyInfo(WORD wDrvID,TGyInfo *pGyInfo);
//WORD GetMGyAppSize(void);
BYTE get_698_task_no(void);
void SetRunPort(BYTE No);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif



