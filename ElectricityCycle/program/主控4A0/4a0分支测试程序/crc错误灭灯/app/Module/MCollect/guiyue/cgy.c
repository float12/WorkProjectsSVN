#include "__define.h"
#include "cgy.h"
#include "usros.h"
#include "port.h"
#include <string.h>
#include "module.h"
extern TPort *gpPorts;

///////////////////////////////////////////////////////////////
//	�����ʹ��!!!!!��ָ����ǰ�����˿�
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
//	�� �� �� : SethGy
//	�������� : ���õ�ǰ��Լ���
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��8��
//	�� �� ֵ : void
//	����˵�� : HPARA hGy
///////////////////////////////////////////////////////////////
// void SethGy(HPARA hGy)
// {
// 	OS_SetReg(TR_GY, (DWORD)hGy);
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : GethGy
//	�������� : ��ȡ��ǰ��Լ���
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��8��
//	�� �� ֵ : HPARA
//	����˵�� : void
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
{//��õ�ǰ�����˿�
#if 0
TGy *pGy=(TGy *)GethGy();

	return &gpPorts[pGy->byNo];
#else
    return &gpPorts[_run_port_no];
#endif
}

///////////////////////////////////////////////////////////////
//	�� �� �� : GethDB
//	�������� : ���й�Լ��ʱ���ݿ�
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��9��
//	�� �� ֵ : HPARA
//	����˵�� : void
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
//	�� �� �� : GetTx
//	�������� : ��õ�ǰ�����ͻ�����
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��9��
//	�� �� ֵ : TTx*
//	����˵�� : void
///////////////////////////////////////////////////////////////
TTx* GetTx(void)
{
TPort *pPort=(TPort *)GethPort();

	return &pPort->Tx;
}

///////////////////////////////////////////////////////////////
//	�� �� �� : *GetRx
//	�������� : ��õ�ǰ������ջ�����
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��9��
//	�� �� ֵ : TRx
//	����˵�� : void
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
