#include "__define.h"
#include "cgy.h"
#include "usros.h"
#include "port.h"
#include <string.h>
#include "module.h"
extern TPort *gpPorts;

///////////////////////////////////////////////////////////////
//	裸机下使用!!!!!，指定当前工作端口
///////////////////////////////////////////////////////////////
static BYTE _run_port_no = 0;

void SetRunPort(BYTE No)
{
	if (No <= 5)
	{ //MAX_VPORT_NUM
		
		_run_port_no = No;
	}
}


///////////////////////////////////////////////////////////////
//	函 数 名 : SethGy
//	函数功能 : 设置当前规约句柄
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年9月8日
//	返 回 值 : void
//	参数说明 : HPARA hGy
///////////////////////////////////////////////////////////////
// void SethGy(HPARA hGy)
// {
// 	OS_SetReg(TR_GY, (DWORD)hGy);
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : GethGy
//	函数功能 : 获取当前规约句柄
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年9月8日
//	返 回 值 : HPARA
//	参数说明 : void
///////////////////////////////////////////////////////////////
// HPARA GethGy(void)
// {
// DWORD dwTmp;

// 	OS_GetReg(TR_GY,&dwTmp);
//    	return (HPARA)dwTmp;
// }

BYTE GetPortNo(void)
{
	return _run_port_no;
}

HPARA GethPort(void)
{//获得当前操作端口
#if 0
TGy *pGy=(TGy *)GethGy();

	return &gpPorts[pGy->byNo];
#else
    return &gpPorts[_run_port_no];
#endif
}

///////////////////////////////////////////////////////////////
//	函 数 名 : GethDB
//	函数功能 : 下行规约临时数据库
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年9月9日
//	返 回 值 : HPARA
//	参数说明 : void
///////////////////////////////////////////////////////////////
HPARA GethDB(void)
{
TPort *pPort=(TPort *)GethPort();

	return pPort->GyRunInfo.hGyDB;
}

// HPARA GethGyApp(void)
// {
// TPort *pPort=(TPort *)GethPort();

// #if 0	
// 	if(pPort->byMode == MGYM_MASTER)
// 		return NULL;
// 	return (HPARA)pPort->aSGyApp[pPort->byGyNo];
// #else
// 	return (HPARA)pPort->aSGyApp[0];
// #endif
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : GetTx
//	函数功能 : 获得当前任务发送缓冲区
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年9月9日
//	返 回 值 : TTx*
//	参数说明 : void
///////////////////////////////////////////////////////////////
TTx* GetTx(void)
{
TPort *pPort=(TPort *)GethPort();

	return &pPort->Tx;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : *GetRx
//	函数功能 : 获得当前任务接收缓冲区
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年9月9日
//	返 回 值 : TRx
//	参数说明 : void
///////////////////////////////////////////////////////////////
TRx *GetRx(void)
{
TPort *pPort=(TPort *)GethPort();

	return &pPort->Rx;
}

HPARA GetRxFm(void)
{
TRx *pRx=GetRx();

	return &pRx->pBuf[pRx->wRptr];
}

// BOOL SwitchToAddr(HPARA hPort,BYTE *Addr,BYTE byAdrLen)
// {
// TPort *pPort=(TPort *)hPort;
// BOOL bRc;

// 	if(pPort->fAddrDaemon)
// 	{
// 		bRc	= pPort->fAddrDaemon(hPort,Addr,byAdrLen,0);
// 		return bRc;
// 	}
// 	else
// 		return (memcmp(Addr,pPort->GyRunInfo.Addr,byAdrLen) == 0);
// }
