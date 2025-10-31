//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	    
//创建日期	
//描述		
//修改记录	
//----------------------------------------------------------------------

#include "AllHead.h"
#include "flats.h"
#include "./GDW698/GDW698DataDef.h"
#include "./GDW698/Client/MGDW698.h"
#include "MCollect.h"
//-----------------------------------------------
//      本文件使用的宏定义
//-----------------------------------------------
#define NODATA				0x40
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//      全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//      本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//      内部函数声明
//-----------------------------------------------
extern WORD fcs16( BYTE *cp, WORD len );
WORD Tx_Fm64507_Read(BYTE *pAddr, DWORD dwCode, BYTE *pOutBuf);
WORD Tx_Fm698_Read(BYTE *pAddr, DWORD dwOad, BYTE *pOutBuf);
WORD Tx_Fm64597_Read(BYTE *pAddr, WORD wCode, BYTE *pOutBuf);
void No2Baud(BYTE *pbyBaudNo,DWORD *pdwBaud,BOOL bDirect);
//-----------------------------------------------
//      函数定义
//-----------------------------------------------
void api_CommuTx(BYTE SerialNo)
{
	TSerial *p = &Serial[SerialNo];

	// 将接收指针清零，用于判断是否收到数据
	p->RXWPoint = 0;
	p->RXRPoint = 0;
	SerialMap[SerialNo].SCIDisableRcv(SerialMap[SerialNo].SCI_Ph_Num);
	SerialMap[SerialNo].SCIEnableSend(SerialMap[SerialNo].SCI_Ph_Num);
}

static void ChangeBaud(BYTE SerialNo, DWORD dwBaud)
{
	BYTE temp1, temp2;

	if (SerialMap[SerialNo].SerialType == eRS485_I)
	{
		temp1 = FPara2->CommPara.I485 & (~0x0f); // 清相应bit  不改变奇偶校验和停止位
	}
	// else if (SerialMap[SerialNo].SerialType == eCAN)
	// {
	// 	temp1 = 0; 
	// }
//	else if (SerialMap[SerialNo].SerialType == eRS485_II)
//	{
//		temp1 = FPara2->CommPara.II485 & (~0x07); // 清相应bit  不改变奇偶校验和停止位
//	}
	else
	{
		return;
	}

	// if (SerialMap[SerialNo].SerialType == eCAN)
	// {
	// 	switch (dwBaud)
	// 	{
	// 	case 10000:
	// 		temp2 = 13;
	// 		break;
	// 	case 25000:
	// 		temp2 = 14;
	// 		break;
	// 	case 50000:
	// 		temp2 = 15;
	// 		break;
	// 	case 125000:
	// 		temp2 = 16;
	// 		break;
	// 	default: // 默认125k
	// 		temp2 = 16;
	// 		break;
	// 	}
	// }
	// else
	// {
		switch (dwBaud)
		{
		case 1200:
			temp2 = 0x02;
			break;
		case 2400:
			temp2 = 0x03;
			break;
		case 4800:
			temp2 = 0x04;
			break;
		case 9600:
			temp2 = 0x06;
			break;
		case 19200:
			temp2 = 0x07;
			break;
		case 38400:
			temp2 = 0x08;
			break;
		case 57600:
			temp2 = 0x09;
			break;
		case 115200:
			temp2 = 0x0A;
			break;
		default: // 默认9600
			temp2 = 0x06;
			break;
		}
	// }
		

	temp1 |= temp2;

	if (SerialMap[SerialNo].SerialType == eRS485_I)
	{
		FPara2->CommPara.I485 = temp1;
		FPara2->CRC32 = lib_CheckCRC32( (BYTE *)(FPara2), sizeof(TFPara2)-sizeof(DWORD) );//不写EEP，只在RAM中更改
	}
	// else if (SerialMap[SerialNo].SerialType == eCAN)
	// {
	// 	FPara2->CommPara.CanBaud = temp1;
	// 	FPara2->CRC32 = lib_CheckCRC32( (BYTE *)(FPara2), sizeof(TFPara2)-sizeof(DWORD) );
	// }
//	else if (SerialMap[SerialNo].SerialType == eRS485_II)
//	{
//		api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.II485), 1, &temp1);
//	}

	Serial[SerialNo].OperationFlag = 1; // 波特率改变
	DealSciFlag(SerialNo);
}

//todo
// 要通信的波特率是否和当前一致，不一致才切换波特率
void JudgeBaudChange(BYTE SerialNo, DWORD Baud)
{
	BYTE temp1, temp2;

	// 当前波特率
	if (SerialMap[SerialNo].SerialType == eRS485_I)
	{
		temp1 = FPara2->CommPara.I485 & 0x0f; // 只判断波特率
	}
	// else if (SerialMap[SerialNo].SerialType == eCAN)
	// {
	// 	temp1 = FPara2->CommPara.CanBaud; 
	// }
//	else if (SerialMap[SerialNo].SerialType == eRS485_II)
//	{
//		temp1 = FPara2->CommPara.II485 & 0x07; // 只判断波特率
//	}

	// 要通信的波特率
	// if (SerialMap[SerialNo].SerialType == eCAN)
	// {
	// 	switch (Baud)
	// 	{
	// 	case 10000:
	// 		temp2 = 13;
	// 		break;
	// 	case 25000:
	// 		temp2 = 14;
	// 		break;
	// 	case 50000:
	// 		temp2 = 15;
	// 		break;
	// 	case 125000:
	// 		temp2 = 16;
	// 		break;
	// 	default: // 默认125k
	// 		temp2 = 16;
	// 		break;
	// 	}
	// }
	// else
	// {
		switch (Baud)
		{
		case 1200:
			temp2 = 0x02;
			break;
		case 2400:
			temp2 = 0x03;
			break;
		case 4800:
			temp2 = 0x04;
			break;
		case 9600:
			temp2 = 0x06;
			break;
		case 19200:
			temp2 = 0x07;
			break;
		case 38400:
			temp2 = 0x08;
			break;
		case 57600:
			temp2 = 0x09;
			break;
		case 115200:
			temp2 = 0x0A;
			break;
		default: // 默认9600
			temp2 = 0x06;
			break;
		}
	// }
		

	if (temp2 != temp1)
	{
		ChangeBaud(SerialNo, Baud);
	}
}

//-----------------------------------------------
// 函数功能: 组帧
//
// 参数:
//		pAddr[in]: 要发送的地址
// 返回值:
//	无
//
// 备注:
//-----------------------------------------------
void Tx_Collect_Meter(BYTE SerialNo, ePROTOCOL_CLASS ePClass, BYTE *pAddr, DWORD dwID, BYTE byEnumBaud)
{
	TSerial *p = &Serial[SerialNo];
    DWORD dwBaud;

    No2Baud(&byEnumBaud,&dwBaud,1);
	JudgeBaudChange(SerialNo, dwBaud);

	p->TXPoint = 0;
	if (ePClass == ePROTOCOL_645_07)
	{
		p->SendLength = Tx_Fm64507_Read(pAddr, dwID, &p->ProBuf[4]);
	}
	else if (ePClass == ePROTOCOL_698)
	{
		p->SendLength = Tx_Fm698_Read(pAddr, dwID, &p->ProBuf[4]);
	}
	else if (ePClass == ePROTOCOL_645_97)
	{
		p->SendLength = Tx_Fm64597_Read(pAddr, (WORD)dwID, &p->ProBuf[4]);
	}
	else
	{
		return;
	}
	memset(p->ProBuf, 0xFE, 4);
	p->SendLength += 4;

	api_CommuTx(SerialNo);
}

WORD Tx_Fm64597_Read(BYTE *pAddr,WORD wCode,BYTE *pOutBuf)
{
T645FmTx97 *pTxFm=(T645FmTx97 *)pOutBuf;
BYTE *pBuf=(BYTE *)(pTxFm+1);
BYTE byHDI,byLDI;
	
	pTxFm->byS68	= 0x68;
	memcpy(pTxFm->MAddr,pAddr,6);
	pTxFm->byE68	= 0x68;
	pTxFm->byCtrl	= 0x01;
	pTxFm->byDataLen= 0x02;
	byHDI           = HIBYTE(wCode) + 0x33;
	byLDI           = LOBYTE(wCode) + 0x33;
	pTxFm->wDI		 = MW(byHDI,byLDI);
	
	pBuf[0]			= lib_CheckSum(&pTxFm->byS68,12);	 
	pBuf[1]			= 0x16;
	return 0x0E;
}

WORD Tx_Fm698_Read(BYTE *pAddr,DWORD dwOad,BYTE *pOutBuf)
{
#if (SEL_SEARCH_METER == YES )
	TSA sa;
	TGetPara para={0};
	BYTE TxBuf[50];
	
	memset(&sa,0,sizeof(TSA));
	memcpy(sa.addr,pAddr,6);
	sa.len = 5;
	if (dwOad == 0x40010200) //搜表
	{
		sa.type = 1; //通配地址
		para.bUseSafeMode = 0;
	}
	else //抄表
	{
		if(dwOad == REQ_ENG698)
		{
			sa.type = 0; //单地址
			para.bUseSafeMode = 0; 		
		}
		else
		{
			sa.type = 0; //单地址
			para.bUseSafeMode = 1;
		}
	}
	para.byType	= 1;
	para.pTxBuff= TxBuf;
	para.wBufLen	+= gdw698buf_OAD(&para.pTxBuff[para.wBufLen],&dwOad,TD21);				
	return _TxFm698Read(&sa,&para,pOutBuf);
#else
	return 0;
#endif //#if (SEL_SEARCH_METER == YES )
}

WORD Tx_Fm64507_Read(BYTE *pAddr,DWORD dwCode,BYTE *pOutBuf)
{
T645FmRx *pTxFm=(T645FmRx *)pOutBuf;
BYTE *pBuf=(BYTE *)(pTxFm+1);
BYTE byHHDI,byHLDI,byLHDI,byLLDI; 

	pTxFm->byS68	= 0x68;
#if 1
	memcpy(pTxFm->MAddr,pAddr,6);
#else
	memset(pTxFm->MAddr,0xAA,6); //调试用
#endif
	pTxFm->byE68	= 0x68;
	pTxFm->byCtrl	= 0x11;
	pTxFm->byDataLen= 0x04;
	byHHDI           = HHBYTE(dwCode) + 0x33;
	byHLDI           = HLBYTE(dwCode) + 0x33;
    byLHDI           = LHBYTE(dwCode) + 0x33;
	byLLDI           = LLBYTE(dwCode) + 0x33;
	pTxFm->dwDI		 = MDW(byHHDI,byHLDI,byLHDI,byLLDI);
	
	pBuf[0]			= lib_CheckSum(&pTxFm->byS68,14);	 
	pBuf[1]			= 0x16;
	return 0x10;
}


