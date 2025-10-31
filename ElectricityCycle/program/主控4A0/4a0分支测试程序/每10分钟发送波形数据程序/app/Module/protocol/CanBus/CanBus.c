//----------------------------------------------------------------------
// Copyright (C) 2022-2028 烟台东方威思顿电气股份有限公司低压台区产品部
// 创建人
// 创建日期
// 描述
// 修改记录
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
TCanBusPara CanBusPara;
BYTE ChargingSaveEventBuf[50];
static DWORD dwProCanID = 0;
static TCanBusID CanBusID;
static TCanBusInfo CanBusInfo;
static TChargingData ChargingData[MAX_CHARGING_NUM];
static BYTE PF20HBuf[MAX_CHARGING_NUM][64], PF61HBuf[MAX_CHARGING_NUM][64], CRCFlag[MAX_CHARGING_NUM];
static struct ChargingEvent_t
{
	BYTE ChargingW[MAX_CHARGING_W_BYTENUM];
	BYTE ChargingE[MAX_CHARGING_E_BYTENUM];
	BYTE ChargingPE[MAX_CHARGING_PE_BYTENUM];
} tChargingEvent[MAX_CHARGING_NUM + 2];
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL IsOpenChargingNo(BYTE ChargingNo)
{
	if(1 == CanBusPara.tCanBusIDMap[ChargingNo].USE_FLAG)
	{
		return TRUE;
	}

	return FALSE;
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void ChangeCanMod(void)
{
	// BYTE i;
//	// for(i = 0; i < MAX_CHARGING_NUM + 1; i++)
//	// {
//	// 	if(IsOpenChargingNo(i) == TRUE)
//	// 	{
//	api_CheckCanBusID();
//	if(CanBusPara.OnlyListen == 1)
//	{
//		CAN_ModeConfig(CanListenOnlyMode, Enable);
//	}
//	else
//	{
//		CAN_ModeConfig(CanListenOnlyMode, Disable);
//	}
//	return;
//	// 	}
//	// }
//	// M4_CAN->RCTRL_f.SACK=0;
//	// CAN_ModeConfig(CanExternalLoopBackMode,Disable);
//	// CAN_ModeConfig(CanListenOnlyMode, Disable);
	// return;
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL api_SetCanMod(BYTE para)
{
	if(CanBusPara.dwCrc = lib_CheckCRC32((BYTE *)&CanBusPara, sizeof(CanBusPara) - sizeof(DWORD)) != TRUE)
	{
		if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara) != TRUE)
		{
			return FALSE;
		}
	}
	CanBusPara.OnlyListen = para;
	return api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara);
}
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE  api_GetCanMod( void )
{
	api_CheckCanBusID();
	return CanBusPara.OnlyListen;
}
//--------------------------------------------------
// 功能描述:  查找序号
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL ChargingIDToNum(BYTE *ID)
{
	BYTE i = 0;
	for(; i < MAX_CHARGING_NUM; i++)
	{
		if((*ID == CanBusPara.tCanBusIDMap[i].ID) && (1 == CanBusPara.tCanBusIDMap[i].USE_FLAG))
		{
			*ID = CanBusPara.tCanBusIDMap[i].CHARGING_NO;
			return TRUE;
		}
	}
	return FALSE;
}
//--------------------------------------------------
// 功能描述:  添加序号
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL ADDChargingMap(BYTE ID)
{
	BYTE i = 0;

	if((ID == 138) || (ID == 246) || ((ID >= 224) && (ID <= 239)))   // 仅过滤充电桩
	{
		if(ID == 138)   // 充电桩
		{
			if(0 == CanBusPara.tCanBusIDMap[MAX_CHARGING_NUM].USE_FLAG)
			{
				CanBusPara.tCanBusIDMap[MAX_CHARGING_NUM].USE_FLAG = 1;
				CanBusPara.tCanBusIDMap[MAX_CHARGING_NUM].ID = ID;
				// ChangeID(eADDID, ID);
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara);
				// ChangeCanMod();
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		if(ChargingIDToNum(&ID))   // 已经有了
		{
			return TRUE;
		}

		for(; i < MAX_CHARGING_NUM; i++)
		{
			if(0 == CanBusPara.tCanBusIDMap[i].USE_FLAG)
			{
				CanBusPara.tCanBusIDMap[i].USE_FLAG = 1;
				CanBusPara.tCanBusIDMap[i].ID = ID;
				MEM_ZERO_STRUCT(tChargingEvent[i]);
				MEM_ZERO_STRUCT(ChargingData[i]);
				MEM_ZERO_ARRAY(PF20HBuf[i]);
				MEM_ZERO_ARRAY(PF61HBuf[i]);
				CRCFlag[i] = 0;
				// ChangeID(eADDID, ID);
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara);
				// ChangeCanMod();
				return TRUE;
			}
		}
	}
	return FALSE;
}
//--------------------------------------------------
// 功能描述:  删除序号
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL DeleteChargingMap(BYTE ID)
{
	BYTE i = 0;

	if((ID == 138) || (ID == 246) || ((ID >= 224) && (ID <= 239)))   // 仅过滤充电桩
	{
		if(ID == 138)   // 充电桩
		{
			if(1 == CanBusPara.tCanBusIDMap[MAX_CHARGING_NUM].USE_FLAG)
			{
				// ChangeID(eDELID, ID);
				CanBusPara.tCanBusIDMap[MAX_CHARGING_NUM].USE_FLAG = 0;
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara);
				// ChangeCanMod();
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		for(; i < MAX_CHARGING_NUM; i++)
		{
			if((ID == CanBusPara.tCanBusIDMap[i].ID) && (1 == CanBusPara.tCanBusIDMap[i].USE_FLAG))
			{
				// ChangeID(eDELID, ID);
				CanBusPara.tCanBusIDMap[i].USE_FLAG = 0;
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara);
				// ChangeCanMod();
				return TRUE;
			}
		}
	}
	return FALSE;
}

//-----------------------------------------------
// 函数功能: 上电初始化信息结构体
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void api_PowerUpCanBus(void)
{
	memset(&CanBusID, 0x00, sizeof(TCanBusID));
	memset(&CanBusInfo, 0x00, sizeof(TCanBusInfo));
	MEM_ZERO_STRUCT(CanBusPara);
	MEM_ZERO_ARRAY(tChargingEvent);
	MEM_ZERO_ARRAY(PF20HBuf);
	MEM_ZERO_ARRAY(PF61HBuf);
	MEM_ZERO_ARRAY(CRCFlag);
	MEM_ZERO_ARRAY(ChargingSaveEventBuf);
	// 从EE中获取参数
	api_CheckCanBusID(); 
	api_SetCanID(0, CanBusPara.bSelfNode);
}

//-----------------------------------------------
// 函数功能: 校验CAN总线参数
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void api_CheckCanBusID(void)
{
	if(CanBusPara.dwCrc = lib_CheckCRC32((BYTE *)&CanBusPara, sizeof(CanBusPara) - sizeof(DWORD)) != TRUE)
	{
		if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara) != TRUE)
		{
			CanBusPara.bSelfNode = 128;   //	默认自身ID为128
			CanBusPara.bRxNum = 0;        //	有效ID个数为0
			CanBusPara.OnlyListen = 0;
		}
	}
}

//-----------------------------------------------
// 函数功能: 判断是否为有效报文(链路层)
//
// 参数: 	tInfo[in]:ID信息
//
// 返回值:  TRUE/FLASE
//
// 备注:
//-----------------------------------------------
BYTE api_JudgeCanBusPro(TCanBusID tInfo)
{
	BYTE bResult = FALSE;

	if(CanBusID.ID_Bit.Next == 0)   // 无后续帧，则为有效数据
	{
		bResult = TRUE;
	}
	else if((tInfo.ID_Bit.TxID == CanBusID.ID_Bit.TxID) && (tInfo.ID_Bit.RxID == CanBusID.ID_Bit.RxID)
	        && (tInfo.ID_Bit.Direction == CanBusID.ID_Bit.Direction))   // 有后续帧，则判定发送方ID、接收方ID、方向，一致则为有效数据
	{
		bResult = TRUE;
	}
	else
	{
		return FALSE;
	}

	memcpy(&CanBusID, &tInfo, sizeof(TCanBusID));

	return bResult;
}
//--------------------------------------------------
// 功能描述:  拓展报文解析
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL CharginglongData(BYTE *Inbuf, BYTE *Outbuf, WORD Outlen)
{
	BYTE i, j, size;
	WORD len, add, sum = 0;
	BYTE MindBuf[128];

	len = Inbuf[2] + Inbuf[3] * 256;
	size = Inbuf[1];
	// add=Inbuf[3+size+len]+Inbuf[3+size+len+1]*256;

	if((len > Outlen) || (size > (len + 9) / 7.0))
	{
		return FALSE;   // 防止越界
	}

	memcpy(MindBuf, (Inbuf + 4), 4);
	for(i = 1; i < size; i++)
	{
		memcpy(MindBuf + 4 + (i - 1) * 7, Inbuf + i * 8 + 1, 7);
	}
	for(i = 0; i < len; i++)
	{
		sum += MindBuf[i];
	}
	add = MindBuf[i] + MindBuf[i + 1] * 256;
	sum += Inbuf[1] + Inbuf[2] + Inbuf[3];
	if(sum == add)
	{
		memcpy(Outbuf, MindBuf, len);
		return TRUE;
	}
	return FALSE;
}
//--------------------------------------------------
// 功能描述:  充电桩事件处理
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
// data   bit    PF
const BYTE ChargingWMap[3][MAX_CHARGING_W_BITNUM] = {
    { 4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  3,  5},
    { 2,  2,  3,  4,  5,  6,  7,  8,  1,  2,  3,  4,  8,  5,  4},
    {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23}
};
const BYTE ChargingEMap[3][MAX_CHARGING_E_BITNUM] = {
    { 2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  7,  2,  2,  2,  2,  2,  2,  2,  2,  3,
     3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8 },
    { 5,  6,  7,  8,  1,  2,  3,  4,  5,  6,  7,  8,  1,  3,  4,  5,  6,  7,  8,  1,  1,  1,  2,  3,  4,  5,  6,  7,  8,  1,
     2,  3,  4,  6,  7,  8,  1,  2,  3,  4,  5,  6,  7,  8,  1,  2,  1,  2,  3,  4,  5,  6,  7,  8,  1,  2,  3,  4,  5 },
    {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23,
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23}
};
const BYTE ChargingPEMap[3][MAX_CHARGING_PE_BITNUM] = {
    { 2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  2,  2,  /* 2,*/ 2,  2,  2,  2,  3,  3},
    { 2,  3,  4,  5,  6,  7,  8,  1,  2,  3,  4,  5,  6,  7,  8,  1,  2,  1,  2,  /* 4,*/ 5,  6,  7,  8,  1,  2},
    {51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52, /*52,*/ 52, 52, 52, 52, 52, 52}
};

void ChargingEventServices(BYTE ChargingNo)
{
	BYTE *pPFXX = NULL;
	QWORD Changebittemp;
	BYTE i;
	if(CRCFlag[ChargingNo] & BIT2)
	{
		CRCFlag[ChargingNo] -= BIT2;
		MEM_ZERO_ARRAY(tChargingEvent[ChargingNo].ChargingW);
		MEM_ZERO_ARRAY(tChargingEvent[ChargingNo].ChargingE);
		for(i = 0; i < sizeof(ChargingWMap[0]); i++)
		{
			if(ChargingWMap[2][i] == 22)
			{
				pPFXX = ChargingData[ChargingNo].PF22H;
			}
			else
			{
				pPFXX = ChargingData[ChargingNo].PF23H;
			}
			if((pPFXX[ChargingWMap[0][i] - 1] & (0x01 << (ChargingWMap[1][i] - 1))))
			{
				tChargingEvent[ChargingNo].ChargingW[i / 8] |= (0x80 >> (i % 8));
			}
		}
		for(i = 0; i < sizeof(ChargingEMap[0]); i++)
		{
			if(ChargingEMap[2][i] == 22)
			{
				pPFXX = ChargingData[ChargingNo].PF22H;
			}
			else
			{
				pPFXX = ChargingData[ChargingNo].PF23H;
			}
			if((pPFXX[ChargingEMap[0][i] - 1] & (0x01 << (ChargingEMap[1][i] - 1))))
			{
				tChargingEvent[ChargingNo].ChargingE[i / 8] |= (0x80 >> (i % 8));
			}
		}
		if((ChargingData[ChargingNo].PF22H[6] != 0) || (ChargingData[ChargingNo].PF22H[7] != 0))   // 其他类型故障
		{
			tChargingEvent[ChargingNo].ChargingE[20 / 8] |= (0x80 >> (20 % 8));
		}
	}
	if(CRCFlag[ChargingNo] & BIT3)
	{
		CRCFlag[ChargingNo] -= BIT3;
		MEM_ZERO_ARRAY(tChargingEvent[ChargingNo].ChargingPE);
		for(i = 0; i < sizeof(ChargingPEMap[0]); i++)
		{
			if(ChargingPEMap[2][i] == 51)
			{
				pPFXX = ChargingData[ChargingNo].PF51H;
			}
			else
			{
				pPFXX = ChargingData[ChargingNo].PF52H;
			}
			if((pPFXX[ChargingPEMap[0][i] - 1] & (0x01 << ChargingPEMap[1][i] - 1)))
			{
				tChargingEvent[ChargingNo].ChargingPE[i / 8] |= (0x80 >> (i % 8));
			}
		}
		if((ChargingData[ChargingNo].PF52H[1] & BIT2))   // 遥测遥信报文超时有两个 置到一起
		{
			tChargingEvent[ChargingNo].ChargingPE[10 / 8] |= (0x80 >> (10 % 8));
		}
		if((ChargingData[ChargingNo].PF52H[1] & BIT3))   // 遥测遥信报文超时有两个 置到一起
		{
			tChargingEvent[ChargingNo].ChargingPE[11 / 8] |= (0x80 >> (11 % 8));
		}
	}
}
//--------------------------------------------------
// 功能描述:  充电桩协议大循环任务
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void api_ChargingCanTask(BYTE ChargingNo)
{
//	BYTE *pWMP = tChargingEvent[MAX_CHARGING_NUM + 1].ChargingW;
//	BYTE *pEMP = tChargingEvent[MAX_CHARGING_NUM + 1].ChargingE;
//	BYTE *pPEMP = tChargingEvent[MAX_CHARGING_NUM + 1].ChargingPE;
//	BYTE *pWM = tChargingEvent[MAX_CHARGING_NUM].ChargingW;
//	BYTE *pEM = tChargingEvent[MAX_CHARGING_NUM].ChargingE;
//	BYTE *pPEM = tChargingEvent[MAX_CHARGING_NUM].ChargingPE;
//	BYTE Changebittemp[MAX(MAX(MAX_CHARGING_W_BYTENUM, MAX_CHARGING_E_BYTENUM), MAX_CHARGING_PE_BYTENUM)];
//
//	MEM_ZERO_ARRAY(Changebittemp);
//	MEM_ZERO_ARRAY(ChargingSaveEventBuf);
//	MEM_ZERO_STRUCT(tChargingEvent[MAX_CHARGING_NUM]);
//	for(ChargingNo = 0; ChargingNo < MAX_CHARGING_NUM; ChargingNo++)
//	{
//		if(IsOpenChargingNo(ChargingNo) == FALSE)
//		{
//			continue;
//		}
//		if(CRCFlag[ChargingNo] & BIT0)
//		{
//			CRCFlag[ChargingNo] -= BIT0;
//			CharginglongData(PF20HBuf[ChargingNo], ChargingData[ChargingNo].PF20H, sizeof(ChargingData[ChargingNo].PF20H));
//		}
//		if(CRCFlag[ChargingNo] & BIT1)
//		{
//			CRCFlag[ChargingNo] -= BIT1;
//			CharginglongData(PF61HBuf[ChargingNo], ChargingData[ChargingNo].PF61H, sizeof(ChargingData[ChargingNo].PF61H));
//		}
//
//		ChargingEventServices(ChargingNo);
//		lib_OrData(pWM, tChargingEvent[ChargingNo].ChargingW, MAX_CHARGING_W_BYTENUM);
//		lib_OrData(pEM, tChargingEvent[ChargingNo].ChargingE, MAX_CHARGING_E_BYTENUM);
//		lib_OrData(pPEM, tChargingEvent[ChargingNo].ChargingPE, MAX_CHARGING_PE_BYTENUM);
//	}
//	if(memcmp(pWM, pWMP, MAX_CHARGING_W_BYTENUM))
//	{
//		memcpy(ChargingSaveEventBuf, pWM, MAX_CHARGING_W_BYTENUM);
//		lib_XorData(pWMP, pWM, MAX_CHARGING_W_BYTENUM);
//		memcpy(ChargingSaveEventBuf + MAX_CHARGING_W_BYTENUM, pWMP, MAX_CHARGING_W_BYTENUM);
//		memcpy(pWMP, pWM, MAX_CHARGING_W_BYTENUM);
//		SaveInstantEventRecord(eEVENT_CHARGING_WARNING_NO, ePHASE_ALL, EVENT_START, eEVENT_ENDTIME_CURRENT);
//	}
//	if(memcmp(pEM, pEMP, MAX_CHARGING_E_BYTENUM))
//	{
//		memcpy(ChargingSaveEventBuf, pEM, MAX_CHARGING_E_BYTENUM);
//		lib_XorData(pEMP, pEM, MAX_CHARGING_E_BYTENUM);
//		memcpy(ChargingSaveEventBuf + MAX_CHARGING_E_BYTENUM, pEMP, MAX_CHARGING_E_BYTENUM);
//		memcpy(pEMP, pEM, MAX_CHARGING_E_BYTENUM);
//		SaveInstantEventRecord(eEVENT_CHARGING_ERR_NO, ePHASE_ALL, EVENT_START, eEVENT_ENDTIME_CURRENT);
//	}
//	if(memcmp(pPEM, pPEMP, MAX_CHARGING_PE_BYTENUM))
//	{
//		memcpy(ChargingSaveEventBuf, pPEM, MAX_CHARGING_PE_BYTENUM);
//		lib_XorData(pPEMP, pPEM, MAX_CHARGING_PE_BYTENUM);
//		memcpy(ChargingSaveEventBuf + MAX_CHARGING_PE_BYTENUM, pPEMP, MAX_CHARGING_PE_BYTENUM);
//		memcpy(pPEMP, pPEM, MAX_CHARGING_PE_BYTENUM);
//		SaveInstantEventRecord(eEVENT_CHARGING_COM_EXCTION_NO, ePHASE_ALL, EVENT_START, eEVENT_ENDTIME_CURRENT);
//	}
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL api_ChargingCan( )
{
//	TFourByteUnion ID;
//	BYTE ChargingNo;
//	static BYTE LongsizePF20 = 0, LongsizePF61 = 0;
//
//	ID.d = cantemp.ExtID;
//	if((ID.b[0] == 138) || (ID.b[0] == 246) || ((ID.b[0] >= 224) && (ID.b[0] <= 239)))   // 仅过滤充电桩
//	{
//		if(ID.b[0] == 138)
//		{
//			ChargingNo = ID.b[1];
//		}
//		else
//		{
//			ChargingNo = ID.b[0];
//		}
//
//		if(ChargingIDToNum(&ChargingNo))
//		{
//		}
//		else
//		{
//			return TRUE;
//		}
//	}
//	else
//	{
//		return FALSE;
//	}
//
//	switch(ID.b[2])
//	{
//	case 0x11:
//		if(cantemp.Data[0] == 1)
//		{
//			memcpy(ChargingData[ChargingNo].PF11H, cantemp.Data + 4, 4);   // 只取有用信息
//		}
//		break;
//	case 0x13:
//		if(cantemp.Data[0] == 1)
//		{
//			memcpy(ChargingData[ChargingNo].PF13H, cantemp.Data + 4, 4);   // 只取有用信息
//		}
//		break;
//	case 0x20:
//		memcpy(PF20HBuf[ChargingNo] + (cantemp.Data[0] - 1) * 8, cantemp.Data, 8);
//		if(cantemp.Data[0] == 1)
//		{
//			LongsizePF20 = cantemp.Data[1];
//		}
//		else if(cantemp.Data[0] == LongsizePF20)
//		{
//			CRCFlag[ChargingNo] |= BIT0;
//		}
//
//		break;
//	case 0x21:
//		memcpy(ChargingData[ChargingNo].PF21H, cantemp.Data, 8);
//		break;
//	case 0x22:
//		memcpy(ChargingData[ChargingNo].PF22H, cantemp.Data, 8);
//		CRCFlag[ChargingNo] |= BIT2;
//		break;
//	case 0x23:
//		memcpy(ChargingData[ChargingNo].PF23H, cantemp.Data, 8);
//		CRCFlag[ChargingNo] |= BIT2;
//		break;
//	case 0x40:
//		memcpy(ChargingData[ChargingNo].PF40H, cantemp.Data, 8);
//		break;
//	case 0x41:
//		memcpy(ChargingData[ChargingNo].PF41H, cantemp.Data, 8);
//		break;
//	case 0x51:
//		memcpy(ChargingData[ChargingNo].PF51H, cantemp.Data, 8);
//		CRCFlag[ChargingNo] |= BIT3;
//		break;
//	case 0x52:
//		memcpy(ChargingData[ChargingNo].PF52H, cantemp.Data, 8);
//		CRCFlag[ChargingNo] |= BIT3;
//		break;
//	case 0x15:
//		memcpy(ChargingData[ChargingNo].PF15H, cantemp.Data, 8);
//		break;
//	case 0x61:
//		memcpy(PF61HBuf[ChargingNo] + (cantemp.Data[0] - 1) * 8, cantemp.Data, 8);
//		if(cantemp.Data[0] == 1)
//		{
//			LongsizePF61 = cantemp.Data[1];
//		}
//		else if(cantemp.Data[0] == LongsizePF61)
//		{
//			CRCFlag[ChargingNo] |= BIT1;
//		}
//
//		break;
//	default:
//		break;   // 其他未监视报文依旧拦截
//	}
//	return TRUE;
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void FactoryInitChargingPara(void)
{
	BYTE i;
	TCanBusIDMap canbusidmap;
	DWORD Inpara = 0;

	api_WriteEventAttribute(0x30E0, 0xff, (BYTE *)&Inpara, 0);
	api_WriteEventAttribute(0x30E1, 0xff, (BYTE *)&Inpara, 0);
	api_WriteEventAttribute(0x30E2, 0xff, (BYTE *)&Inpara, 0);
	MEM_ZERO_STRUCT(CanBusPara);
	MEM_ZERO_ARRAY(tChargingEvent);
	MEM_ZERO_ARRAY(PF20HBuf);
	MEM_ZERO_ARRAY(PF61HBuf);
	MEM_ZERO_ARRAY(CRCFlag);
	MEM_ZERO_ARRAY(ChargingSaveEventBuf);

	CanBusPara.bSelfNode = 128;   //	默认自身ID为128
	CanBusPara.bRxNum = 0;        //	有效ID个数为0
	CanBusPara.OnlyListen = ListenType;
	for(i = 0; i < MAX_CHARGING_NUM + 1; i++)
	{
		canbusidmap.CHARGING_NO = i;
		canbusidmap.USE_FLAG = 0;
		canbusidmap.ID = 0xFF;
		memcpy(&CanBusPara.tCanBusIDMap[i], &canbusidmap, sizeof(TCanBusIDMap));
	}

	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara);
	ChangeCanMod();
	api_SetCanID(0, CanBusPara.bSelfNode);
}
//--------------------------------------------------
// 功能描述:  清除事件时同时清除瞬时量
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void api_FactoryInitChargingEvent(BYTE eChargeingIndex)
{
	BYTE ChargingNo, i, ClearbittempPF22[sizeof(ChargingData[ChargingNo].PF22H)], ClearbittempPF23[sizeof(ChargingData[ChargingNo].PF23H)],
	    ClearbittempPF51[sizeof(ChargingData[ChargingNo].PF51H)], ClearbittempPF52[sizeof(ChargingData[ChargingNo].PF52H)];

	memset(ClearbittempPF22, 0xff, sizeof(ClearbittempPF22));
	memset(ClearbittempPF23, 0xff, sizeof(ClearbittempPF23));
	memset(ClearbittempPF51, 0xff, sizeof(ClearbittempPF51));
	memset(ClearbittempPF52, 0xff, sizeof(ClearbittempPF52));

	switch(eChargeingIndex)
	{
	case 0:   // eEVENT_CHARGING_WARNING_NO:
		for(i = 0; i < sizeof(ChargingWMap[0]); i++)
		{
			if(ChargingWMap[2][i] == 22)
			{
				ClearbittempPF22[ChargingWMap[0][i] - 1] &= (~(0x01 << (ChargingWMap[1][i] - 1)));
			}
			else
			{
				ClearbittempPF23[ChargingWMap[0][i] - 1] &= (~(0x01 << (ChargingWMap[1][i] - 1)));
			}
		}
		MEM_ZERO_ARRAY(tChargingEvent[MAX_CHARGING_NUM + 1].ChargingW);
		break;
	case 1:   // eEVENT_CHARGING_ERR_NO:
		for(i = 0; i < sizeof(ChargingEMap[0]); i++)
		{
			if(ChargingEMap[2][i] == 22)
			{
				ClearbittempPF22[ChargingEMap[0][i] - 1] &= (~(0x01 << (ChargingEMap[1][i] - 1)));
			}
			else
			{
				ClearbittempPF23[ChargingEMap[0][i] - 1] &= (~(0x01 << (ChargingEMap[1][i] - 1)));
			}
		}
		ClearbittempPF22[6] = 0;
		ClearbittempPF22[7] = 0;
		MEM_ZERO_ARRAY(tChargingEvent[MAX_CHARGING_NUM + 1].ChargingE);
		break;
	case 2:   // eEVENT_CHARGING_COM_EXCTION_NO:
		for(i = 0; i < sizeof(ChargingPEMap[0]); i++)
		{
			if(ChargingPEMap[2][i] == 51)
			{
				ClearbittempPF51[ChargingPEMap[0][i] - 1] &= (~(0x01 << (ChargingPEMap[1][i] - 1)));
			}
			else
			{
				ClearbittempPF52[ChargingPEMap[0][i] - 1] &= (~(0x01 << (ChargingPEMap[1][i] - 1)));
			}
		}
		ClearbittempPF52[1] &= (~BIT2);
		ClearbittempPF52[1] &= (~BIT3);
		MEM_ZERO_ARRAY(tChargingEvent[MAX_CHARGING_NUM + 1].ChargingPE);
		break;
	default:
		break;
	}

	for(ChargingNo = 0; ChargingNo < MAX_CHARGING_NUM; ChargingNo++)
	{
		if(IsOpenChargingNo(ChargingNo) == TRUE)
		{
			switch(eChargeingIndex)   // 清除置位
			{
			case 0:   // eEVENT_CHARGING_WARNING_NO:
				MEM_ZERO_ARRAY(tChargingEvent[ChargingNo].ChargingW);
				break;
			case 1:   // eEVENT_CHARGING_ERR_NO:
				MEM_ZERO_ARRAY(tChargingEvent[ChargingNo].ChargingE);
				break;
			case 2:   // eEVENT_CHARGING_COM_EXCTION_NO:
				MEM_ZERO_ARRAY(tChargingEvent[ChargingNo].ChargingPE);
				break;
			default:
				break;
			}
			lib_AndData(ChargingData[ChargingNo].PF22H, ClearbittempPF22, sizeof(ChargingData[ChargingNo].PF22H));
			lib_AndData(ChargingData[ChargingNo].PF23H, ClearbittempPF23, sizeof(ChargingData[ChargingNo].PF23H));
			lib_AndData(ChargingData[ChargingNo].PF51H, ClearbittempPF51, sizeof(ChargingData[ChargingNo].PF51H));
			lib_AndData(ChargingData[ChargingNo].PF52H, ClearbittempPF52, sizeof(ChargingData[ChargingNo].PF52H));
		}
	}
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL ChangeID(eIDDOtype iddotype, BYTE *para, BYTE Size)
{
	BYTE i, IDTemp, j;
	BOOL returnbuf = FALSE;
	switch(iddotype)
	{
	case eADDID:
		for(i = 0; i < Size; i++)
		{
			returnbuf = TRUE;
			if(CanBusPara.bRxNum >= MAX_CANBUS_ID_NUM)   // 已经满了
			{
				returnbuf = FALSE;
				break;
			}

			for(j = 0; j < CanBusPara.bRxNum; j++)
			{
				if(CanBusPara.bRxNode[j] == para[i])
				{
					break;
				}
			}
			if(j == CanBusPara.bRxNum)   // 不重复 添加
			{
				CanBusPara.bRxNode[CanBusPara.bRxNum] = para[i];
				CanBusPara.bRxNum++;
			}
		}
		break;
	case eDELID:
		for(i = 0; i < Size; i++)
		{
			returnbuf = FALSE;
			if(CanBusPara.bRxNum == 0)   // 已经没了
			{
				break;
			}

			for(j = 0; j < CanBusPara.bRxNum; j++)
			{
				if(CanBusPara.bRxNode[j] == para[i])
				{
					for(; j < (CanBusPara.bRxNum - 1); j++)   // 以此前移
					{
						CanBusPara.bRxNode[j] = CanBusPara.bRxNode[j + 1];
					}
					CanBusPara.bRxNum--;
					returnbuf = TRUE;
					break;
				}
			}
			if(returnbuf != TRUE)   // 没找到
			{
				break;
			}
		}
		break;

	case eCLEANID:
		CanBusPara.bRxNum = 0;
		returnbuf = TRUE;
		break;
	default:
		return FALSE;
		break;
	}
	if(returnbuf == TRUE)
	{
		returnbuf = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara);
	}
	else   // 有错误  恢复原状
	{
		api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara);
	}

	return returnbuf;
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
WORD Charging698(eCharging698type type, BYTE *buf, WORD OI, BYTE ClassAttribute, BYTE ChargingNo, BYTE ClassIndex)
{
	WORD len = 0, i;
	SDWORD TD = 0;

	if(ChargingIDToNum(&ChargingNo) == FALSE)
	{
		return 0x8000;
	}

	switch(ClassAttribute)
	{
	case 2:
		switch(OI)
		{
		case 0x2700:
			if(ClassIndex > 1)
			{
				return 0x8000;
			}
			buf[len++] = ChargingData[ChargingNo].PF20H[2];
			buf[len++] = ChargingData[ChargingNo].PF20H[1];
			break;
		case 0x2701:
			if(ClassIndex > 1)
			{
				return 0x8000;
			}
			TD = (ChargingData[ChargingNo].PF20H[3] + ChargingData[ChargingNo].PF20H[4] * 256 - 4000) * 100;
			lib_ExchangeData(buf, (BYTE *)&TD, 4);
			len += 4;
			break;
		case 0x2702:
			TD = (ChargingData[ChargingNo].PF20H[13] + ChargingData[ChargingNo].PF20H[14] * 256) / 10;
			buf[len++] = TD / 0x100;
			buf[len++] = TD % 0x100;
			break;
		case 0x2704:
			TD = (-50 + ChargingData[ChargingNo].PF20H[26]) * 10;
			lib_ExchangeData(buf, (BYTE *)&TD, 2);
			TD = (-50 + ChargingData[ChargingNo].PF20H[27]) * 10;
			lib_ExchangeData(buf + 2, (BYTE *)&TD, 2);
			TD = (-50 + ChargingData[ChargingNo].PF20H[28]) * 10;
			lib_ExchangeData(buf + 4, (BYTE *)&TD, 2);
			TD = (-50 + ChargingData[ChargingNo].PF20H[29]) * 10;
			lib_ExchangeData(buf + 6, (BYTE *)&TD, 2);
			len = 8;
			break;
		case 0x2705:
			buf[len++] = 0;
			buf[len++] = ChargingData[ChargingNo].PF20H[5];
			break;
		case 0x2706:
			TD = (-50 + ChargingData[ChargingNo].PF20H[6]) * 10;
			lib_ExchangeData(buf, (BYTE *)&TD, 2);
			len = 2;
			break;
		case 0x2707:
			TD = (-50 + ChargingData[ChargingNo].PF20H[12]) * 10;
			lib_ExchangeData(buf, (BYTE *)&TD, 2);
			len = 2;
			break;
		case 0x2708:
			TD = ChargingData[ChargingNo].PF20H[15] + ChargingData[ChargingNo].PF20H[16] * 256;
			buf[len++] = TD / 0x100;
			buf[len++] = TD % 0x100;
			break;
		case 0x2709:
			if(ClassIndex == 0)
			{
				TD = ChargingData[ChargingNo].PF20H[24] + ChargingData[ChargingNo].PF20H[25] * 256;
				buf[len++] = TD / 0x100;
				buf[len++] = TD % 0x100;
				TD = (ChargingData[ChargingNo].PF21H[1] + ChargingData[ChargingNo].PF21H[2] * 256) * 10;
				lib_ExchangeData(buf + 2, (BYTE *)&TD, 4);
				len += 4;
				TD = ChargingData[ChargingNo].PF21H[3] + ChargingData[ChargingNo].PF21H[4] * 256;
				buf[len++] = TD / 0x100;
				buf[len++] = TD % 0x100;
			}
			else if(ClassIndex == 1)
			{
				TD = ChargingData[ChargingNo].PF20H[24] + ChargingData[ChargingNo].PF20H[25] * 256;
				buf[len++] = TD / 0x100;
				buf[len++] = TD % 0x100;
			}
			else if(ClassIndex == 2)
			{
				TD = (ChargingData[ChargingNo].PF21H[1] + ChargingData[ChargingNo].PF21H[2] * 256) * 10;
				lib_ExchangeData(buf + 2, (BYTE *)&TD, 4);
				len += 4;
			}
			else if(ClassIndex == 3)
			{
				TD = ChargingData[ChargingNo].PF21H[3] + ChargingData[ChargingNo].PF21H[4] * 256;
				buf[len++] = TD / 0x100;
				buf[len++] = TD % 0x100;
			}

			break;
		case 0x4600:
			if(ClassIndex == 0)
			{
				TD = ChargingData[ChargingNo].PF61H[29] + ChargingData[ChargingNo].PF61H[30] * 256;
				buf[len++] = TD / 0x100;
				buf[len++] = TD % 0x100;
				TD = ChargingData[ChargingNo].PF61H[31] + ChargingData[ChargingNo].PF61H[32] * 256;
				buf[len++] = TD / 0x100;
				buf[len++] = TD % 0x100;
				TD = (ChargingData[ChargingNo].PF61H[33] + ChargingData[ChargingNo].PF61H[34] * 256 - 4000) * 100;
				lib_ExchangeData(buf + len, (BYTE *)&TD, 4);
				len += 4;
				TD = (ChargingData[ChargingNo].PF61H[35] + ChargingData[ChargingNo].PF61H[36] * 256 - 4000) * 100;
				lib_ExchangeData(buf + len, (BYTE *)&TD, 4);
				len += 4;
			}
			else if(ClassIndex == 1)
			{
				TD = ChargingData[ChargingNo].PF61H[29] + ChargingData[ChargingNo].PF61H[30] * 256;
				buf[len++] = TD / 0x100;
				buf[len++] = TD % 0x100;
			}
			else if(ClassIndex == 2)
			{
				TD = ChargingData[ChargingNo].PF61H[31] + ChargingData[ChargingNo].PF61H[32] * 256;
				buf[len++] = TD / 0x100;
				buf[len++] = TD % 0x100;
			}
			else if(ClassIndex == 3)
			{
				TD = (ChargingData[ChargingNo].PF61H[33] + ChargingData[ChargingNo].PF61H[34] * 256 - 4000) * 100;
				lib_ExchangeData(buf + len, (BYTE *)&TD, 4);
				len += 4;
			}
			else if(ClassIndex == 4)
			{
				TD = (ChargingData[ChargingNo].PF61H[35] + ChargingData[ChargingNo].PF61H[36] * 256 - 4000) * 100;
				lib_ExchangeData(buf + len, (BYTE *)&TD, 4);
				len += 4;
			}

			break;
		case 0x4601:   // 充电枪号 未找到
		case 0x4602:   // 额定充电功率 未找到
		case 0x4603:   // 最小充电功率 未找到
			break;
		case 0x4702:
			if(2 == ChargingData[ChargingNo].PF41H[3])
			{
				buf[len++] = 0;
			}
			else
			{
				buf[len++] = 1;
			}

			break;
		default:
			break;
		}
		break;
	case 3:   // 单位

		switch(OI)
		{
		case 0x4600:
			buf[len++] = ChargingData[ChargingNo].PF61H[7];
			break;
		case 0x4702:
			buf[len++] = ChargingData[ChargingNo].PF40H[2];
			break;

		default:
			break;
		}
		break;
	case 4:
		switch(OI)
		{
		case 0x2706:
			TD = (-50 + ChargingData[ChargingNo].PF20H[7]) * 10;
			lib_ExchangeData(buf, (BYTE *)&TD, 2);
			len = 2;
			break;
		case 0x2708:
			TD = (ChargingData[ChargingNo].PF20H[17] + ChargingData[ChargingNo].PF20H[18] * 256 - 4000) * 100;
			lib_ExchangeData(buf, (BYTE *)&TD, 4);
			len += 4;
			break;
		case 0x4600:
			memcpy(buf, &ChargingData[ChargingNo].PF22H[0], 8);
			len += 8;
			memcpy(buf + len, &ChargingData[ChargingNo].PF23H[0], 8);
			len += 8;
			break;
		case 0x4702:
			if((ChargingData[ChargingNo].PF22H[1] & BIT0) == 0)
			{
				if((ChargingData[ChargingNo].PF22H[1] & BIT1) == 0)
				{
					TD += BIT0;
				}
				else
				{
					TD += BIT3;
				}
			}
			else
			{
				if((ChargingData[ChargingNo].PF22H[1] & BIT1) == 0)
				{
					TD += BIT1;
				}
			}
			if((ChargingData[ChargingNo].PF22H[1] & BIT2) != 0)
			{
				TD += BIT2;
			}
			if(ChargingData[ChargingNo].PF15H[1] == 1)
			{
				TD += BIT4;
			}
			if((ChargingData[ChargingNo].PF23H[2] & BIT4) != 0)
			{
				TD += BIT5;
			}
			if((ChargingData[ChargingNo].PF22H[2] & BIT3) != 0)
			{
				TD += BIT6;
			}
			if((ChargingData[ChargingNo].PF22H[3] & BIT1) != 0)
			{
				TD += BIT7;
			}
			buf[len++] = TD;
			break;
		default:
			break;
		}
		break;
	case 5:
		switch(OI)
		{
		case 0x2706:
			TD = (ChargingData[ChargingNo].PF20H[8] + ChargingData[ChargingNo].PF20H[9] * 256) / 10;
			lib_ExchangeData(buf, (BYTE *)&TD, 2);
			len = 2;
			break;
		case 0x2708:
			buf[len++] = ChargingData[ChargingNo].PF20H[19];
			break;
		case 0x4600:
			buf[len++] = ChargingData[ChargingNo].PF11H[2];
			buf[len++] = ChargingData[ChargingNo].PF11H[3];
			break;
		default:
			break;
		}
		break;
	case 6:
		switch(OI)
		{
		case 0x2706:
			TD = (ChargingData[ChargingNo].PF20H[10] + ChargingData[ChargingNo].PF20H[11] * 256) / 10;
			lib_ExchangeData(buf, (BYTE *)&TD, 2);
			len = 2;
			break;
		case 0x2708:
			TD = ChargingData[ChargingNo].PF20H[20] + ChargingData[ChargingNo].PF20H[21] * 256;
			buf[len++] = TD / 0x100;
			buf[len++] = TD % 0x100;
			break;
		case 0x4600:
			buf[len++] = ChargingData[ChargingNo].PF13H[2];
			buf[len++] = ChargingData[ChargingNo].PF13H[1];
			break;
		default:
			break;
		}
		break;
	case 7:
		switch(OI)
		{
		case 0x2708:
			TD = (ChargingData[ChargingNo].PF20H[22] + ChargingData[ChargingNo].PF20H[23] * 256 - 4000) * 100;
			lib_ExchangeData(buf, (BYTE *)&TD, 4);
			len = 4;
			break;
		case 0x4600:
			memcpy(buf, &ChargingData[ChargingNo].PF51H[0], 4);
			len += 4;
		default:
			break;
		}
		break;
	case 8:
		switch(OI)
		{
		case 0x4600:
			memcpy(buf, &ChargingData[ChargingNo].PF52H[0], 3);
			len += 3;

		default:
			break;
		}
	default:
		break;
	}
	if(len == 0)
	{
		return 0x8000;
	}

	return len;
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BYTE Charging698AddTag(BYTE ClassAttribute, WORD OI, BYTE *Inbuf, BYTE *Outbuf, BYTE ClassIndex)
{
	BYTE len = 0;
	DWORD TD;
	switch(ClassAttribute)
	{
	case 2:
		switch(OI)
		{
		case 0x2700:
			if(ClassIndex == 0)
			{
				Outbuf[len++] = 1;   // Array_698
				Outbuf[len++] = 1;
			}
		case 0x2702:
		case 0x2705:
		case 0x2708:
			Outbuf[len++] = 18;   // Long_unsigned_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			break;
		case 0x2701:
			if(ClassIndex == 0)
			{
				Outbuf[len++] = 1;   // Array_698
				Outbuf[len++] = 1;
			}
			Outbuf[len++] = 5;   // Double_long_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			Outbuf[len++] = Inbuf[2];
			Outbuf[len++] = Inbuf[3];
			break;

		case 0x2704:
			Outbuf[len++] = 1;   // Array_698
			Outbuf[len++] = 4;
			Outbuf[len++] = 16;   // Long_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			Outbuf[len++] = 16;   // Long_698
			Outbuf[len++] = Inbuf[2];
			Outbuf[len++] = Inbuf[3];
			Outbuf[len++] = 16;   // Long_698
			Outbuf[len++] = Inbuf[4];
			Outbuf[len++] = Inbuf[5];
			Outbuf[len++] = 16;   // Long_698
			Outbuf[len++] = Inbuf[6];
			Outbuf[len++] = Inbuf[7];
			break;

		case 0x2706:
		case 0x2707:
			Outbuf[len++] = 16;   // Long_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			break;
		case 0x2709:
			if(ClassIndex == 0)
			{
				Outbuf[len++] = 2;   // Structure_698
				Outbuf[len++] = 3;
				Outbuf[len++] = 18;   // Long_unsigned_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
				Outbuf[len++] = 6;   // Double_long_unsigned_698
				Outbuf[len++] = Inbuf[2];
				Outbuf[len++] = Inbuf[3];
				Outbuf[len++] = Inbuf[4];
				Outbuf[len++] = Inbuf[5];
				Outbuf[len++] = 18;   // Long_unsigned_698
				Outbuf[len++] = Inbuf[6];
				Outbuf[len++] = Inbuf[7];
			}
			else if(ClassIndex == 1)
			{
				Outbuf[len++] = 18;   // Long_unsigned_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
			}
			else if(ClassIndex == 2)
			{
				Outbuf[len++] = 6;   // Double_long_unsigned_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
				Outbuf[len++] = Inbuf[2];
				Outbuf[len++] = Inbuf[3];
			}
			else if(ClassIndex == 3)
			{
				Outbuf[len++] = 18;   // Long_unsigned_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
			}
			break;
		case 0x4600:
			if(ClassIndex == 0)
			{
				Outbuf[len++] = 2;   // Structure_698
				Outbuf[len++] = 4;
				Outbuf[len++] = 18;   // Long_unsigned_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
				Outbuf[len++] = 18;   // Long_unsigned_698
				Outbuf[len++] = Inbuf[2];
				Outbuf[len++] = Inbuf[3];
				Outbuf[len++] = 5;   // Double_long_698
				Outbuf[len++] = Inbuf[4];
				Outbuf[len++] = Inbuf[5];
				Outbuf[len++] = Inbuf[6];
				Outbuf[len++] = Inbuf[7];
				Outbuf[len++] = 5;   // Double_long_698
				Outbuf[len++] = Inbuf[8];
				Outbuf[len++] = Inbuf[9];
				Outbuf[len++] = Inbuf[10];
				Outbuf[len++] = Inbuf[11];
			}
			else if(ClassIndex == 1)
			{
				Outbuf[len++] = 18;   // Long_unsigned_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
			}
			else if(ClassIndex == 2)
			{
				Outbuf[len++] = 18;   // Long_unsigned_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
			}
			else if(ClassIndex == 3)
			{
				Outbuf[len++] = 5;   // Double_long_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
				Outbuf[len++] = Inbuf[2];
				Outbuf[len++] = Inbuf[3];
			}
			else if(ClassIndex == 4)
			{
				Outbuf[len++] = 5;   // Double_long_698
				Outbuf[len++] = Inbuf[0];
				Outbuf[len++] = Inbuf[1];
				Outbuf[len++] = Inbuf[2];
				Outbuf[len++] = Inbuf[3];
			}

			break;
		case 0x4702:
			Outbuf[len++] = 22;   // Enum_698
			Outbuf[len++] = Inbuf[0];
			break;
		default:
			break;
		}
		break;
	case 3:   // 单位
		switch(OI)
		{
		case 0x4600:
		case 0x4702:
			Outbuf[len++] = 22;   // Enum_698
			Outbuf[len++] = Inbuf[0];
			break;

		default:
			break;
		}
		break;
	case 4:
		switch(OI)
		{
		case 0x2706:
			Outbuf[len++] = 16;   // Long_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			break;
		case 0x2708:
			Outbuf[len++] = 5;   // Double_long_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			Outbuf[len++] = Inbuf[2];
			Outbuf[len++] = Inbuf[3];
			break;
		case 0x4600:
			Outbuf[len++] = 1;   // Array_698
			Outbuf[len++] = 1;
			Outbuf[len++] = 4;   // Bit_string_698
			Outbuf[len++] = 0x81;
			Outbuf[len++] = 0x80;
			memcpy(Outbuf + len, Inbuf, 16);
			len += 16;
			break;
		case 0x4702:
			Outbuf[len++] = 17;   // Unsigned_698
			Outbuf[len++] = Inbuf[0];
			break;
		default:
			break;
		}
		break;
	case 5:
		switch(OI)
		{
		case 0x2706:
			Outbuf[len++] = 16;   // Long_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			break;
		case 0x2708:
			Outbuf[len++] = 22;   // Enum_698
			Outbuf[len++] = Inbuf[0];
			break;
		case 0x4600:
			Outbuf[len++] = 1;   // Array_698
			Outbuf[len++] = 1;
			Outbuf[len++] = 2;   // Structure_698
			Outbuf[len++] = 2;
			Outbuf[len++] = 17;   // Unsigned_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = 9;   // Octet_string_698
			Outbuf[len++] = 1;
			Outbuf[len++] = Inbuf[1];
			break;
		default:
			break;
		}
		break;
	case 6:
		switch(OI)
		{
		case 0x2706:
			Outbuf[len++] = 16;   // Long_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			break;
		case 0x2708:
			Outbuf[len++] = 18;   // Long_unsigned_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			break;
		case 0x4600:
			Outbuf[len++] = 1;   // Array_698
			Outbuf[len++] = 1;
			Outbuf[len++] = 2;   // Structure_698
			Outbuf[len++] = 2;
			Outbuf[len++] = 17;   // Unsigned_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = 9;   // Octet_string_698
			Outbuf[len++] = 1;
			Outbuf[len++] = Inbuf[1];
			break;
		default:
			break;
		}
		break;
	case 7:
		switch(OI)
		{
		case 0x2708:
			Outbuf[len++] = 5;   // Double_long_698
			Outbuf[len++] = Inbuf[0];
			Outbuf[len++] = Inbuf[1];
			Outbuf[len++] = Inbuf[2];
			Outbuf[len++] = Inbuf[3];
			break;
		case 0x4600:
			Outbuf[len++] = 1;   // Array_698
			Outbuf[len++] = 1;
			Outbuf[len++] = 4;   // Bit_string_698
			Outbuf[len++] = 32;
			memcpy(Outbuf + len, Inbuf, 4);
			len += 4;
			break;
		default:
			break;
		}
		break;
	case 8:
		switch(OI)
		{
		case 0x4600:
			Outbuf[len++] = 1;   // Array_698
			Outbuf[len++] = 1;
			Outbuf[len++] = 4;   // Bit_string_698
			Outbuf[len++] = 24;
			memcpy(Outbuf + len, Inbuf, 3);
			len += 3;
			break;

		default:
			break;
		}
	default:
		break;
	}
	return len;
}
//--------------------------------------------------
// 功能描述:  设置充电桩参数
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BYTE SetCharging(BYTE *pOAD, BYTE *pbuf)
{
	TTwoByteUnion OI;

	lib_ExchangeData(OI.b, pOAD, 2);

	switch(OI.w)
	{
	case 0xF221:
		if(pOAD[2] == 0x06)
		{
			CanBusPara.bSelfNode = pOAD[5];
			api_SetCanID(0, pOAD[5]);
			if(api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ProSafeRom.CanBusPara), sizeof(TCanBusPara), (BYTE *)&CanBusPara) == TRUE)
			{
				return DAR_Success;
			}
			return DAR_RefuseOp;
		}

		break;

	default:
		break;
	}
	return DAR_Undefined;
}
//--------------------------------------------------
// 功能描述:  设置CAN总线通信ID
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void api_SetCanID(BYTE bType, BYTE bTar)
{
	switch(bType)
	{
	case 0:   // 发送方地址
		dwProCanID &= 0xffffff00;
		dwProCanID |= bTar;
		break;
	case 1:   // 接收方地址
		dwProCanID &= 0xffff00ff;
		dwProCanID |= ((DWORD)bTar * 0x100);
		break;
	case 2:   // 后续帧标志
		dwProCanID &= 0xff7fffff;
		dwProCanID |= ((DWORD)(bTar & 0x01) << 23);
		break;
	case 3:   // 帧方向
		dwProCanID &= 0xfeffffff;
		dwProCanID |= ((DWORD)(bTar & 0x01) << 24);
		break;
	case 4:   // 广播标志
		dwProCanID &= 0xfdffffff;
		dwProCanID |= ((DWORD)(bTar & 0x01) << 25);
		break;
	default:
		break;
	}
}
//--------------------------------------------------
// 功能描述:  获取CAN总线通信ID
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
DWORD api_GetCanID(void)
{
	return dwProCanID;
}
