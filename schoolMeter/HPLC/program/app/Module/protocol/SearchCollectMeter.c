//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���
//��������	2021.7.27
//����		��λ�ѱ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
							  // �ϵ������RAM�����걣�棬���籣�棻�ٴ���������
TSchedMeterInfo tSearchedMeter[SEARCH_METER_MAX_NUM];// for test ÿ�γ���ʱ��EEP�����������RAM�㹻���ϵ�Ϳ��Զ�����RAM

TSKMeter SKMeter;

TCollectMeterInfo tCltMeterInfo;
TCycleSInfoRom tCycleSInfoRom; //�����ѱ����
TMeterInfoRom tSchMtInfo;
//-----------------------------------------------p
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
void JudgeBaudChange(BYTE SerialNo, DWORD dwBaud);
extern WORD Tx_Fm64507_Read(BYTE *pAddr,DWORD dwCode,BYTE *pOutBuf);
extern WORD Tx_Fm698_Read(BYTE *pAddr,DWORD dwOad,BYTE *pOutBuf);
extern WORD Tx_Fm64597_Read(BYTE *pAddr,WORD wCode,BYTE *pOutBuf);
//-----------------------------------------------
//��������: ��֡
//
//����:	
//		pAddr[in]: Ҫ���͵ĵ�ַ
//����ֵ:
//	��
//
//��ע:
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
		temp1 = FPara2->CommPara.I485 & (~0x0f); //����Ӧbit  ���ı���żУ���ֹͣλ
	}
	else if (SerialMap[SerialNo].SerialType == eRS485_II)
	{
		temp1 = FPara2->CommPara.II485 & (~0x0f); //����Ӧbit  ���ı���żУ���ֹͣλ
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
	default: //Ĭ��9600
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

	Serial[SerialNo].OperationFlag = 1; //�����ʸı�
	DealSciFlag(SerialNo);
}

// Ҫͨ�ŵĲ������Ƿ�͵�ǰһ�£���һ�²��л�������
void JudgeBaudChange(BYTE SerialNo, DWORD dwBaud)
{
	BYTE temp1, temp2;

	// ��ǰ������
	if (SerialMap[SerialNo].SerialType == eRS485_I)
	{
		temp1 = FPara2->CommPara.I485 & 0x0f; //ֻ�жϲ�����
	}
	else if (SerialMap[SerialNo].SerialType == eRS485_II)
	{
		temp1 = FPara2->CommPara.II485 & 0x0f; //ֻ�жϲ�����
	}

	// Ҫͨ�ŵĲ�����
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
	default: //Ĭ��9600
		temp2 = 0x06;
		break;
	}

	if (temp2 != temp1)
	{
		ChangeBaud(SerialNo, dwBaud);
	}
}

//-----------------------------------------------
//				��������
//-----------------------------------------------

void api_CommuTx( BYTE SerialNo )
{
	TSerial *p = &Serial[SerialNo];

	// ������ָ�����㣬�����ж��Ƿ��յ�����
	p->RXWPoint = 0;
	p->RXRPoint = 0;
	SerialMap[SerialNo].SCIDisableRcv(SerialMap[SerialNo].SCI_Ph_Num);
	SerialMap[SerialNo].SCIEnableSend(SerialMap[SerialNo].SCI_Ph_Num);
}
// �ϵ��EEP����RAM������ֱ��ʹ�ã������ѱ������
void api_PowerUpSchClctMeter(void)
{
//	BOOL boResult;
//	TClctTypeRom Temp;
//
//	boResult = TRUE;
//
//	// ��ʼ���ѱ����
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
//	// ��ʼ���ѱ���Ϣ
//	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(tSchMtInfoRom[0]), sizeof(TSchedMeterInfo) * SEARCH_METER_MAX_NUM, (BYTE *)&tSearchedMeter[0]);
//
//	memset((BYTE *)&SKMeter, 0x00, sizeof(TSKMeter));
//
//	// ��ʼ����������
//	memset((BYTE *)&tCltMeterInfo, 0x00, sizeof(TCollectMeterInfo));
//
//	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ClctMeterRom.ClctTypeRom), sizeof(TClctTypeRom), (BYTE *)&Temp) != TRUE)
//	{
//		Temp.dwType = eHourFreeze;
//	}
//
//	tCltMeterInfo.Type = Temp.dwType; // ������Ա��������ʱ��ʼ��
//
//	// ��ȡ�����ѱ����
//	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom) != TRUE)
//	{
//		memcpy((BYTE *)&tCycleSInfoRom, (BYTE *)&CycleSInfoRomConst, sizeof(TCycleSInfoRom));
//	}
}

// ��������ɾ��������eep��Ķ���
void api_ClearSchMeter(void)
{
//	memset((BYTE *)&tSearchedMeter[0],0xff,sizeof(TSchedMeterInfo)*SEARCH_METER_MAX_NUM);
//	
//	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(tSchMtInfoRom[0]), sizeof(TSchedMeterInfo)*SEARCH_METER_MAX_NUM, (BYTE *)&tSearchedMeter[0]);
}
// ��ʼ��
// 1. �����ѱ���Ϣ��������
// 2. ��ʼ���������ͣ����㶳��
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
//��������: �жϸñ��ַ�Ƿ����ѱ�����
//
//����: pAddr[IN] ���ַ
//    : pBaud[OUT] ͨѶ����
//
//����ֵ: BOOL
//	��
//
//��ע:
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
//��������: �ж����ⵥԪ�Ƿ����ڲɼ���ģʽ
//����:
//����ֵ:
//��ע: 
//-----------------------------------------------
BOOL IsWorkInCllMode(void)
{
	return (tCycleSInfoRom.CllType == 1);
}
