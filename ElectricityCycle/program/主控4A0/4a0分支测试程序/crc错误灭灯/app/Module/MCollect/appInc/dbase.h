///////////////////////////////////////////////////////////////
//	文 件 名 :dbase.h
//	文件功能 :数据库相关定义
//	作    者 : jiangjy
//	创建时间 : 2015年9月11日
//	项目名称 ：DF6203
//	操作系统 : 
//	备    注 :
//	直接引用FLASH读写接口（读一个2K块，写一个2K块，擦除128K块）115M
//	历史记录： : 
///////////////////////////////////////////////////////////////
#ifndef __DBASE_H__
#define __DBASE_H__
#include "mp.h"
#include "sysdata.h"
//#include "dbFrz.h"
// #include "dbReal.h"
#include "mpflags.h"
//缓冲区冻结标识类型
// #define BUF_DFRZ			0x12345678
// #define BUF_MFRZ			0x23456789
// #define BUF_CURFRZ			0x34567890
// #define BUF_MPRUNINFO		0x67890000//低字节测量点号
// #define BUF_NOVALID			0x5A5AA5A5

//#define  MAX_RATIO			4		
//参数类型标识
// #define	FLAG_SYS			BIT0
// #define	FLAG_METER			BIT1
// #define	FLAG_MP_EX			BIT2
// #define	FLAG_FKSYS			BIT3
// #define	FLAG_TASK			BIT4
// #define FLAG_DATASET		BIT5
// #define FLAG_SYS_EX			BIT6
// #define FLAG_ESAM			BIT7

//698协议参数配置
// typedef struct _TBase698ParaCfg{ 
// 	BYTE base_para_buf[4096];
// 	BOOL saveflag;
// }TBase698ParaCfg;
 
typedef struct _TMP698ParaCfg{ 
	BYTE rtu_para_buf[4096];
	BYTE nowPageNum;
	BOOL saveflag;
}TMP698ParaCfg;
 
// typedef struct _TTask698ParaCfg{ 
// 	//.BYTE para_task_buf[((MAX_698_TASK_NUM*sizeof(TOad60130200)+2047)/2048)*2048];
// 	BYTE para_task_buf[((MAX_698_TASK_NUM*(27+MAX_TASK_TIMEREG_NUM*4)+2047)/2048)*2048];
// 	BOOL saveflag;
// }TTask698ParaCfg;

// typedef struct _TScheme698ParaCfg{ 
// 	BYTE para_scheme_buf[2048];
// 	BYTE nowPageNum;
// 	BOOL saveflag;
// }TScheme698ParaCfg;
 
// typedef struct _TCtrl698ParaCfg{
// 	BYTE para_ctrl_buf[2048];
// 	BYTE nowPageNum;
// 	BOOL saveflag;
// }TCtrl698ParaCfg;

// typedef struct _TFrz698ParaCfg{
// 	BYTE para_frz_buf[2048];
// 	BYTE nowPageNum;
// 	BOOL saveflag;
// }TFrz698ParaCfg;

typedef struct _TSysParaCfg{
	// TBase698ParaCfg  base_para_cfg;//系统常用参数内存(永久保存，开机更新) 
	TMP698ParaCfg    mp_para_cfg;  //测量点档案共享2K参数 
	// TTask698ParaCfg  task_para_cfg;//终端4K任务参数内存(永久保存，开机更新)  
	// TScheme698ParaCfg  sheme_para_cfg; //方案2K平时主动上报占用 抄表另辟空间 
	// TCtrl698ParaCfg  ctrl_para_cfg;//控制参数任务
	// TFrz698ParaCfg	frz_para_cfg;	//冻结参数配置
}TSysParaCfg;

extern TSysParaCfg *gpSysPara;

//测量点号(0~1999),终端本身点号，总加组号
// #define TERMINAL			2000
// #define GRPNO(x)			(WORD)(2000+x)		//总加组号
// #define MP2GrpNo(x)			(x-2000)
// //698专用
// #define PULSENO(x)			(WORD)(2100+x)		//脉冲测量点号
// #define MP2PulseNo(x)		(x-2100)

// typedef struct _TSaveBuf698{
// 	BYTE DataBuf[SIZE_PAGE];	//数据缓冲区
// 	BYTE byFrzType;				//存贮时间
// }TSaveBuf698;

//在内存中保留的实时数据
// typedef struct _TData_6203{
	// DWORD	dwImagicB;			//用于检测NVRAM是否发生变化的标识头

	// BYTE	byACDCFlag;			//交直流供电标识
	// DWORD	dwLastStopTime;		//最近一次停电时间(相对于2000年的秒数)

	// DWORD HourFrzFlag[MAX_DWNPORT_NUM];	//小时冻结纵向标签(固定为24小时)
	// BYTE DFrzFlag[MAX_DWNPORT_NUM][(DAYS_DFRZ_SAVE+7)/8];
	// BYTE DAutoFrzFlag[(MAX_MP_NUM+7)/8]; //自动抄表日冻结标识	
//698扩展

	// DWORD OldTaskTime[MAX_DWNPORT_NUM][MAX_698_TASK_NUM];
	// BYTE RealBuf[SIZE_PAGE];	//实时数据缓冲区
	// WORD wRealWptr;				//当前指针
	// WORD real_page_offset;		//实时数据页偏移(BLOCK_OF_REAL/2048)
	//BYTE CurvBuf698[SIZE_PAGE];	//数据缓冲区
	// BYTE UpType[3];				//通道上报类型
	// BYTE safmodeInit[(MAX_MP_NUM+7)/8]; //安全模式初始化标识
	// BYTE safmodeValid[(MAX_MP_NUM+7)/8]; //安全模式启用标识
	// TSaveBuf698 PortBuf[MAX_DWNPORT_NUM];

	// TSaveBuf698 SysBuf;
	// DWORD dwSTime;				//起始时间偏移
	// DWORD dwDFrzTime;			//冻结时间
	// DWORD dwMFrzTime;			//冻结时间
    // DWORD	dwLastRunTime;
	// DWORD	dwLastDSTM;			//存盘时间
	// DWORD	dwLastMSTM;			//存盘时间
	// DWORD	TaskCheckTime[MAX_DWNPORT_NUM];	
	// DWORD	UpTaskCheckTime[3];	
	// DWORD   dwLastCurvTime[MAX_MP_NUM];	//上1次测量点抄到曲线的存储时标，用于方法10读取
	// DWORD	dwImagicE;			//用于检测NVRAM是否发生变化的标识尾 
// }TData_6203;

// TData_6203 *pGetDF6203(void);

// void InitDase(void);
// BOOL MPTime2Time(TMPTime *pMPTime,TTime *pTime);
// DWORD GetLastRunMins(void);
// void DB_SaveData(DWORD dwMins);
// void SwitchToNewFrzTime(BYTE byType,DWORD dwTime);
// void MPRxDeal(TPort *pPort,WORD wMPNo);
#endif