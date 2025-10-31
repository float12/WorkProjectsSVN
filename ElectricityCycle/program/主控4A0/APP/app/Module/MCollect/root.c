#include "AllHead.h"
#include "flats.h"
#include "./GDW698/GDW698DataDef.h"

//--------------------------------------------------------
// extern void InitBadBlockTab(void);
extern void WatchDog(void);
extern void InitMPs(void);
extern void InitSysCfg(void);
extern void InitMPFlags(void);
// extern void InitDase(void);
extern void gdw698APP_Init(BYTE Ch);
extern BOOL MGyApp_Init(BYTE Ch);
extern void GetMeterAddr(BYTE *pAddr);
extern void lib_InverseData( BYTE *Buf, WORD Len );
// BOOL sys_Para698_Read(DWORD oad_index,HPARA hPARA);
// BOOL sys_Para698_Write(DWORD oad_index,HPARA hPARA);
//--------------------------------------------------------
#define MAX_COMMU_BUF_LEN   (MAX_PRO_BUF+EXTRA_BUF)

static TPort _s_PortArray[MAX_VPORT_NUM];
TPort *gpPorts;
// TVar gVar;
static BYTE _GyRxBuffer[MAX_DWNPORT_NUM][MAX_COMMU_BUF_LEN+4]; //端口接收缓冲区
static BYTE _GyTxBuffer[MAX_DWNPORT_NUM][MAX_COMMU_BUF_LEN+4]; //端口发送缓冲区
//--------------------------------------------------------
static void _InitVars(void)
{
TTime Time;

	//gpPorts	= MemAlloc(sizeof(TPort)*MAX_VPORT_NUM);
	gpPorts = &_s_PortArray[0];
	
	// memset(&gVar,0,sizeof(TVar));
	
	//更新系统启动时间
	// GetTime(&Time);	
	// gVar.dwStartTime= Time2Sec(&Time);	//使用最后一次的时间目的是为了好擦除块管理

	InitMPFlags();
}

// void ParaSync(void)
// {
// 	TOad40010200 oad;
// 	BYTE MetAddr[6];
	
// 	GetMeterAddr(MetAddr);
// 	lib_InverseData(MetAddr,6);
// 	sys_Para698_Read(0x40010200,&oad);
// 	if ((oad.addr.AF != 5) || memcmp(oad.addr.addr,MetAddr,6))
// 	{
// 		oad.addr.AF = 5;
// 		memcpy(oad.addr.addr,MetAddr,6);
// 		sys_Para698_Write(0x40010200,&oad);
// 	}
// }

BOOL PT_AddPort(DWORD dwID,UPortCfg *pPC,WORD wRxBufSize,WORD wTxBufSize,BYTE *pbyNo)
{//向端口队列中增加一个端口
	BYTE byI;
	TPort *pPort=gpPorts;
	BOOL bRc=FALSE;
	
	if(NULL == gpPorts)
		return bRc;
	//1、查找有效端口
	//1.1 检查端口是否已经被打开了
	for(byI=0;byI<MAX_VPORT_NUM;byI++,pPort++)
	{
		if(pPort->byOpen)
		{
			if(pPort->dwID == dwID)
			{
// 				if(pPort->Tx.wSize < wTxBufSize)
// 				{
// 					MemFree(pPort->Tx.pBuf);
// 					pPort->Tx.pBuf	= MemAlloc(wTxBufSize);
// 					pPort->Tx.wSize = wTxBufSize;
// 				}
// 				if(pPort->Rx.wSize < wRxBufSize)
// 				{
// 					MemFree(pPort->Rx.pBuf);
// 					pPort->Rx.pBuf	= MemAlloc(wRxBufSize);
// 					pPort->Rx.wSize = wRxBufSize;
// 				}
				*pbyNo	= byI;
				return TRUE;
			}	
		}
	}
	//1.2 打开指定的端口
	pPort=gpPorts;
	for(byI=0;byI<MAX_VPORT_NUM;byI++,pPort++)
	{
		if(pPort->byOpen == 0)
			break;
	}
	if(byI >= MAX_VPORT_NUM)
		return FALSE;
	
	//2、初始化端口信息
	memset(pPort,0,sizeof(TPort));
	pPort->dwID		= dwID;
	if(wTxBufSize)
	{
		//pPort->Tx.pBuf	= MemAlloc(wTxBufSize);
		if(byI < MAX_DWNPORT_NUM)
			pPort->Tx.pBuf = &_GyTxBuffer[byI][0];
		else
			pPort->Tx.pBuf = NULL;
	}
	pPort->Tx.wSize	= wTxBufSize;
	if(wRxBufSize)	
	{
		//pPort->Rx.pBuf	= MemAlloc(wRxBufSize);
		if(byI < MAX_DWNPORT_NUM)
			pPort->Rx.pBuf = &_GyRxBuffer[byI][0];
		else
			pPort->Rx.pBuf = NULL;
	}
	pPort->Rx.wSize	= wRxBufSize;
	pPort->byOpen	= 1;
#if 0
	bRc	= PT_Open(pPort,pPC);
#else
	bRc = TRUE;
#endif
	if(bRc)
	{		
		*pbyNo			= byI;
	}
	else
	{
		pPort->byOpen	= 0;				
// 		if(pPort->Tx.pBuf)
// 			MemFree(pPort->Tx.pBuf);
// 		if(pPort->Rx.pBuf)
// 			MemFree(pPort->Rx.pBuf);				
	}
	return bRc;
}

TPort *pGetPort(BYTE byNo)
{
	return &gpPorts[byNo];
}

static void appRoot(void)
{//应用任务入口
	UPortCfg PC={0};
	BYTE byNo;
	
	InitSysCfg();	
	
	InitMPs();	
	
	//初始化数据库
	// InitDase();	
	
//	ParaSync();

	PT_AddPort(MDW(MPT_SERIAL,1,0,0),&PC,MAX_COMMU_BUF_LEN,MAX_COMMU_BUF_LEN,&byNo); //eRS485_I
	// gdw698APP_Init(byNo);
	MGyApp_Init(byNo);
	PT_AddPort(MDW(MPT_SERIAL,2,0,0),&PC,MAX_COMMU_BUF_LEN,MAX_COMMU_BUF_LEN,&byNo); //eCR
	// gdw698APP_Init(byNo);
	MGyApp_Init(byNo);
	PT_AddPort(MDW(MPT_SERIAL,3,0,0),&PC,MAX_COMMU_BUF_LEN,MAX_COMMU_BUF_LEN,&byNo); //eBlueTooth
	// gdw698APP_Init(byNo);
	MGyApp_Init(byNo);
	PT_AddPort(MDW(MPT_SERIAL,4,0,0),&PC,MAX_COMMU_BUF_LEN,MAX_COMMU_BUF_LEN,&byNo); //eCAN
	// gdw698APP_Init(byNo);
	MGyApp_Init(byNo);
	PT_AddPort(MDW(MPT_SERIAL,5,0,0),&PC,MAX_COMMU_BUF_LEN,MAX_COMMU_BUF_LEN,&byNo); //扩展功能
	// gdw698APP_Init(byNo);
	MGyApp_Init(byNo);
}

void root_init(void)
{
	WatchDog();
	
	//初始化坏块替换表
	// InitBadBlockTab();
	
	WatchDog();
	//初始化应用用全局变量
	_InitVars();
	
	//调用用户appRoot
	appRoot();
	
	WatchDog();
}
