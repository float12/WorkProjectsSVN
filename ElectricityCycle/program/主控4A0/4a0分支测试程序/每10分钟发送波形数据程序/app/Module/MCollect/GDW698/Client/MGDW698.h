#ifndef MGDW698_H__
#define MGDW698_H__
#if(MD_BASEUSER	== MD_GDW698)

typedef struct _TGetPara{
	BYTE *pTxBuff;
	WORD wBufLen;
	WORD wNum;
	BOOL bUseSafeMode;
	BYTE *Rand;	
	BYTE byType;	
}TGetPara;

typedef struct _TMGDW698 {		//接受帧属性	
	RSD		rsdd;  
	BYTE	byShemeNo;			//方案编号
	BYTE	byTxTaskType;		//发送任务类型
	BYTE	byReqNext;			//是否需要请求下一帧
	WORD	wRxOKSeq;			//最近一次接收Ok帧序号
	DWORD	dwRandFreshTime;	//随机数刷新时间
	BYTE	Rand[30];			//随机数长度+随机数
//透明任务参数
	// WORD	wCommIdleTime_tran;	//字符最大间隔 
	// DWORD	dwRxDelayMax_tran;	//最长等待时间
	WORD	wAPDUSize;			//APDU长度
//698实时转曲线
	BYTE    byCJTypeFlag;       //置位代表实时转曲线
	DWORD   dwLastTaskTime;
	BYTE    DataBuffer[512];
}TMGDW698;

BOOL GetReponseAPDUDatalen(BYTE *pAPDU,WORD wLen);
WORD _TxFm698Read(TSA *sa,TGetPara *pPara,BYTE *pBuf);
#endif
#endif