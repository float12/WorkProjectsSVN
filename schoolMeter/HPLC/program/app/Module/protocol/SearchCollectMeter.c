//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	刘骞
//创建日期	2021.7.27
//描述		缩位搜表
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
							  // 上电读出到RAM，搜完保存，掉电保存；再次搜索清零
TSchedMeterInfo tSearchedMeter[SEARCH_METER_MAX_NUM];// for test 每次抄表时从EEP读出来？如果RAM足够，上电就可以读出到RAM

TSKMeter SKMeter;

TCollectMeterInfo tCltMeterInfo;
TCycleSInfoRom tCycleSInfoRom; //周期搜表参数
TMeterInfoRom tSchMtInfo;
//-----------------------------------------------p
//				本文件使用的变量，常量
//-----------------------------------------------
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
void JudgeBaudChange(BYTE SerialNo, DWORD dwBaud);
extern WORD Tx_Fm64507_Read(BYTE *pAddr,DWORD dwCode,BYTE *pOutBuf);
extern WORD Tx_Fm698_Read(BYTE *pAddr,DWORD dwOad,BYTE *pOutBuf);
extern WORD Tx_Fm64597_Read(BYTE *pAddr,WORD wCode,BYTE *pOutBuf);
//-----------------------------------------------
//函数功能: 组帧
//
//参数:	
//		pAddr[in]: 要发送的地址
//返回值:
//	无
//
//备注:
//-----------------------------------------------
void Tx_Collect_Meter(BYTE SerialNo, ePROTOCOL_CLASS ePClass, BYTE *pAddr, DWORD dwID, DWORD dwBaud)
{
	TSerial *p = &Serial[SerialNo];
	
	JudgeBaudChange(SerialNo, dwBaud);
	
	p->TXPoint = 0;
	if (ePClass == ePROTOCOL_645_07)
	{
		p->SendLength = Tx_Fm64507_Read(pAddr,dwID,&p->ProBuf[4]);
	}
	else if (ePClass == ePROTOCOL_698)
	{
		p->SendLength = Tx_Fm698_Read(pAddr,dwID,&p->ProBuf[4]);
	}
	else if (ePClass == ePROTOCOL_645_97)
	{
		p->SendLength = Tx_Fm64597_Read(pAddr,(WORD)dwID,&p->ProBuf[4]);
	}
	else
	{
		return;
	}
	memset(p->ProBuf,0xFE,4);
	p->SendLength += 4;
	
	api_CommuTx( SerialNo );
}

static void ChangeBaud(BYTE SerialNo, DWORD dwBaud)
{
	BYTE temp1, temp2;

	if (SerialMap[SerialNo].SerialType == eRS485_I)
	{
		temp1 = FPara2->CommPara.I485 & (~0x0f); //清相应bit  不改变奇偶校验和停止位
	}
	else if (SerialMap[SerialNo].SerialType == eRS485_II)
	{
		temp1 = FPara2->CommPara.II485 & (~0x0f); //清相应bit  不改变奇偶校验和停止位
	}
	else
	{
		return;
	}

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
	case 115200:
		temp2 = 0x0a;
		break;
	default: //默认9600
		temp2 = 0x06;
		break;
	}

	temp1 |= temp2;

	if (SerialMap[SerialNo].SerialType == eRS485_I)
	{
		api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.I485), 1, &temp1);
	}
	else if (SerialMap[SerialNo].SerialType == eRS485_II)
	{
		api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.II485), 1, &temp1);
	}

	Serial[SerialNo].OperationFlag = 1; //波特率改变
	DealSciFlag(SerialNo);
}

// 要通信的波特率是否和当前一致，不一致才切换波特率
void JudgeBaudChange(BYTE SerialNo, DWORD dwBaud)
{
	BYTE temp1, temp2;

	// 当前波特率
	if (SerialMap[SerialNo].SerialType == eRS485_I)
	{
		temp1 = FPara2->CommPara.I485 & 0x0f; //只判断波特率
	}
	else if (SerialMap[SerialNo].SerialType == eRS485_II)
	{
		temp1 = FPara2->CommPara.II485 & 0x0f; //只判断波特率
	}

	// 要通信的波特率
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
	case 115200:
		temp2 = 0x0a;
		break;
	default: //默认9600
		temp2 = 0x06;
		break;
	}

	if (temp2 != temp1)
	{
		ChangeBaud(SerialNo, dwBaud);
	}
}

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

void api_CommuTx( BYTE SerialNo )
{
	TSerial *p = &Serial[SerialNo];

	// 将接收指针清零，用于判断是否收到数据
	p->RXWPoint = 0;
	p->RXRPoint = 0;
	SerialMap[SerialNo].SCIDisableRcv(SerialMap[SerialNo].SCI_Ph_Num);
	SerialMap[SerialNo].SCIEnableSend(SerialMap[SerialNo].SCI_Ph_Num);
}
// 上电从EEP读到RAM，可以直接使用；重新搜表则更新
void api_PowerUpSchClctMeter(void)
{
//	BOOL boResult;
//	TClctTypeRom Temp;
//
//	boResult = TRUE;
//
//	// 初始化搜表个数
//	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SearchMeterRom.MeterInfoRom), sizeof(TMeterInfoRom), (BYTE *)&tSchMtInfo) != TRUE)
//	{
//		boResult = FALSE;
//	}
//
//	if (tSchMtInfo.dwRecordNo > SEARCH_METER_MAX_NUM)
//	{
//		boResult = FALSE;
//	}
//
//	if (boResult == FALSE)
//	{
//		tSchMtInfo.dwRecordNo = 0;
//		lib_CheckSafeMemVerify((BYTE *)&tSchMtInfo.dwRecordNo, sizeof(TMeterInfoRom), REPAIR_CRC);
//	}
//
//	// 初始化搜表信息
//	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(tSchMtInfoRom[0]), sizeof(TSchedMeterInfo) * SEARCH_METER_MAX_NUM, (BYTE *)&tSearchedMeter[0]);
//
//	memset((BYTE *)&SKMeter, 0x00, sizeof(TSKMeter));
//
//	// 初始化抄表类型
//	memset((BYTE *)&tCltMeterInfo, 0x00, sizeof(TCollectMeterInfo));
//
//	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ClctMeterRom.ClctTypeRom), sizeof(TClctTypeRom), (BYTE *)&Temp) != TRUE)
//	{
//		Temp.dwType = eHourFreeze;
//	}
//
//	tCltMeterInfo.Type = Temp.dwType; // 其他成员变量抄表时初始化
//
//	// 获取周期搜表参数
//	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom) != TRUE)
//	{
//		memcpy((BYTE *)&tCycleSInfoRom, (BYTE *)&CycleSInfoRomConst, sizeof(TCycleSInfoRom));
//	}
}

// 后续可以删掉，包括eep里的定义
void api_ClearSchMeter(void)
{
//	memset((BYTE *)&tSearchedMeter[0],0xff,sizeof(TSchedMeterInfo)*SEARCH_METER_MAX_NUM);
//	
//	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(tSchMtInfoRom[0]), sizeof(TSchedMeterInfo)*SEARCH_METER_MAX_NUM, (BYTE *)&tSearchedMeter[0]);
}
// 初始化
// 1. 清零搜表信息（次数）
// 2. 初始化抄表类型，整点冻结
void api_InitSchClctMeter(void)
{
//	TClctTypeRom tClctType;
//	
//	tSchMtInfo.dwRecordNo = 0;
//	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SearchMeterRom.MeterInfoRom), sizeof(TMeterInfoRom), (BYTE *)&tSchMtInfo);
//
//	tClctType.dwType = eHourFreeze;
//	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ClctMeterRom.ClctTypeRom), sizeof(TClctTypeRom), (BYTE *)&tClctType);
//
//	memset((BYTE *)&tCltMeterInfo, 0x00, sizeof(TCollectMeterInfo));
//
//	tCltMeterInfo.Type = tClctType.dwType;
//
//	memset((BYTE *)&SKMeter, 0x00, sizeof(TSKMeter));
//	api_ClearSchMeter();
//
//	memcpy((BYTE *)&tCycleSInfoRom,(BYTE *)&CycleSInfoRomConst,sizeof(TCycleSInfoRom));
//	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom);
}


BOOL IsMeterValid(BYTE byNo)
{
	return TRUE;
}

BOOL IsBaudValid(DWORD dwBaud)
{
	switch(dwBaud)
	{
	default: return FALSE;
	case 1200:
	case 2400:
	case 4800:
	case 9600:
	case 115200:
		return TRUE;
	}
}
//-----------------------------------------------
//函数功能: 判断该表地址是否在搜表结果中
//
//参数: pAddr[IN] 表地址
//    : pBaud[OUT] 通讯速率
//
//返回值: BOOL
//	无
//
//备注:
//-----------------------------------------------
BOOL IsExistMeter(BYTE *pAddr,DWORD *pBaud)
{
	BYTE byI;
	
	for(byI=0;byI<SEARCH_METER_MAX_NUM;byI++)
	{
		if( IsMeterValid(byI) )
		{
			if ( memcmp(pAddr, tSearchedMeter[byI].Addr,6) == 0 )
			{
				if (pBaud != NULL)
				{
					*pBaud = tSearchedMeter[byI].dwBaud;
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}





BYTE SwitchToNext(BYTE byStartNo, TSchedMeterInfo *pMetInfo)
{
BYTE byI;
	
    for (byI = byStartNo; byI <SEARCH_METER_MAX_NUM; byI++)
	{
		if ( IsMeterValid(byI) )
		{
			memcpy(pMetInfo, &tSearchedMeter[byI], sizeof(TSchedMeterInfo));
			return byI;
		}
	}
    return 0xFF;
}

void api_ClrClctMeterRetry( void )
{
	tCltMeterInfo.Retry = 0;
}

//-----------------------------------------------
//函数功能: 判断量测单元是否工作在采集器模式
//参数:
//返回值:
//备注: 
//-----------------------------------------------
BOOL IsWorkInCllMode(void)
{
	return (tCycleSInfoRom.CllType == 1);
}
