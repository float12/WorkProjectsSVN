//----------------------------------------------------------------------
// Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾����ѹ̨����Ʒ��
//
// ������
//
// ��������
//
// ����
//
// �޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "Mid_api.h"
#include "mid.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
TMeterFun tMeterFun;         // ��������ṩ�Ļص�����
TMidInfoData tMidInfoData;   // ��չ����֧�ֵ�OI(����+�б�)
TMidFunction tMidFunction;   // ��չ���ܺ���ָ��
TMOD_EVENT_OI_OIMapping tOI_OIMapping;
BYTE ModuleEventBuf[0x304];
WORD wExtOI;

//-----------------------------------------------
//			ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//			���ļ�ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//			�ڲ���������
//-----------------------------------------------
BOOL SaveModuleRecord(BYTE Type, BYTE No, BYTE *pOad, WORD OI);
void CheckOI_OIMapping(void);
extern void api_ExtRxMonitor(TMidParaData *ParaData);
extern void api_ExtAgent(TMidParaData *MidParaData);
//-----------------------------------------------
//			��������
//-----------------------------------------------

//-----------------------------------------------
// ��������: �ж������Ƿ�Ϊ�ն�ҵ��
//
// ����:
//
// ����ֵ:   �ɹ�/ʧ��
//
// ��ע:  ʵ��ͨ�����ĸ�ʽ�Ľṹ�壬���Ի�ȡ����
//-----------------------------------------------
BYTE IsTerminalPro(BYTE *APDU)
{
	WORD wOI;

	if(APDU[0] == 07)
	{
		wOI = (WORD)((APDU[3] << 8) + APDU[4]);
		if((wOI == 0x6000) || (wOI == 0x6012) || (wOI == 0x6014))
		{
			return TRUE;
		}
	}
	else if(APDU[0] == 0x05)
	{
		if((APDU[1] == 0x01) || (APDU[1] == 0x03))
		{
			wOI = (WORD)((APDU[3] << 8) + APDU[4]);
			if((wOI == 0x6000) || (wOI == 0x6012) || (wOI == 0x6014))
			{
				return TRUE;
			}
		}
		if(APDU[1] == 0x05)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------
// ��������: �ص����ݹ���ʵ�ֺ���
//
// ����: 	MidParaData[in/out]:����ṹ��
//
// ����ֵ:   �ɹ�/ʧ��
//
// ��ע:  ʵ��ͨ�����ĸ�ʽ�Ľṹ�壬���Ի�ȡ����
//-----------------------------------------------
Mid_WORD MeterDataFun(TMidParaData *MidParaData)
{
//	if( (MidParaData->pAPDU[0] == 0x09) 
//	/*&& (MidParaData->pAPDU[1] == 0x07)
//	&& (MidParaData->ProxyFlag == 0x55) */)	//����
//	{
//		// ת��
//		// api_ExtAgent(MidParaData);
//		wExtOI = MidParaData->OI;
//	}
//	// else if(IsTerminalPro(MidParaData->pAPDU) == TRUE)
//	//{
//	//	api_ExtRxMonitor(MidParaData);
//	// }
//	// else
////	{
////		LinkData[eEXP_PRO].ControlByte   = MidParaData->ControlByte;   // �������ֽ�
////		LinkData[eEXP_PRO].AddressType   = (eAddressType)MidParaData->AddressType;
////		LinkData[eEXP_PRO].AddressLen    = MidParaData->AddressLen;   // ����ַ����
////		LinkData[eEXP_PRO].ClientAddress = MidParaData->ClientAddress;
////		LinkData[eEXP_PRO].eLinkDataFlag = eAPP_OK;
////		LinkData[eEXP_PRO].pAPP          = MidParaData->pAPDU;
////		LinkData[eEXP_PRO].pAPPLen.w     = MidParaData->pAPDULen;
//		// ������
//		memset(&APPData[eEXP_PRO].APPFlag, 0x00, sizeof(TDLT698APPData));   // ��ʼ��Ӧ�ò�ṹ��
//		InitDlt698APP(eEXP_PRO);
//		ProcDlt698Apdu(eEXP_PRO);
//
//		if(APPData[eEXP_PRO].APPFlag != APP_NO_DATA)
//		{
//			// MidParaData->ControlByte = APPData[eEXP_PRO].ControlByte;		//�������ֽ�
//			APPData[eEXP_PRO].pAPPBuf[APPData[eEXP_PRO].APPBufLen.w++] = 0;
//			APPData[eEXP_PRO].pAPPBuf[APPData[eEXP_PRO].APPBufLen.w++] = 0;
//			memcpy(MidParaData->pAPDU, APPData[eEXP_PRO].pAPPBuf, APPData[eEXP_PRO].APPBufLen.w);
//
//			MidParaData->pAPDULen = APPData[eEXP_PRO].APPBufLen.w;
//		}
//	}

	return TRUE;
}

//-----------------------------------------------
// ��������: ��ʼ��
//
// ����: 	��
//
// ����ֵ:   ��
//
// ��ע:   ��ɺ���ע��
//-----------------------------------------------
void api_MidLayerInit(void)
{
	// ��ɻص�����ʵ�ֺ���
	tMeterFun.MeterDataFun = MeterDataFun;

	wExtOI = 0;

	// ��ʼ��OI�ṹ��
	memset(&tMidInfoData, 0x00, sizeof(tMidInfoData));
	// ��ʼ���¼���־
	memset(ModuleEventBuf, 0x00, sizeof(ModuleEventBuf));
	// ��ȡ�¼�OI��Ӧ��
	CheckOI_OIMapping();
	// ���ýӿں���
	LoadMidEnterFunction(tMeterFun, &tMidInfoData, &tMidFunction);
}

//-----------------------------------------------
// ��������: ������
//
// ����: 	��
//
// ����ֵ:   ��
//
// ��ע:   ִ�ж��������
//-----------------------------------------------
void api_MidLayerTask(void)
{
	BYTE i, bTaskType;
	eMidTimeTaskType TimeType;

	CLEAR_WATCH_DOG;

	// ѭ������
	bTaskType = (1 << eMidLoopMode);

	// ������
	if(api_GetTaskFlag(eTASK_MID_ID, eFLAG_SECOND) == TRUE)
	{
		bTaskType |= (1 << eMidSecMode);
		api_ClrTaskFlag(eTASK_MID_ID, eFLAG_SECOND);
	}

	// ��������
	if(api_GetTaskFlag(eTASK_MID_ID, eFLAG_MINUTE) == TRUE)
	{
		bTaskType |= (1 << eMidMinMode);
		api_ClrTaskFlag(eTASK_MID_ID, eFLAG_MINUTE);
	}

	for(i = 0; i < tMidFunction.MidAndExpandTimeTask.Num; i++)
	{
		for(TimeType = eMidLoopMode; TimeType <= eMidMinMode; TimeType++)
		{
			if(tMidFunction.MidAndExpandTimeTask.TimeTask[i].TimeTaskType == TimeType)
			{
				if(bTaskType & (1 << TimeType))
				{
					if (TimeType==eMidLoopMode)
					{
						for (BYTE j = 0; j < 4; j++)
						{
							tMidFunction.MidAndExpandTimeTask.TimeTask[i].MidTimeTask();
						}
						
					}
					else
					tMidFunction.MidAndExpandTimeTask.TimeTask[i].MidTimeTask();
				}
				break;
			}
		}
	}
	bTaskType = 0;
	CLEAR_WATCH_DOG;
}
//--------------------------------------------------
// ��������:  ��ȡ��չģ������
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BYTE api_RequestModule(BYTE *APDU, BYTE APDULen, WORD BufLen)
{
	TMidParaData ExpandParaData;
	BYTE Addr[10];

	memset(Addr, 0x00, sizeof(Addr));
	memset(&ExpandParaData, 0x00, sizeof(ExpandParaData));
	ExpandParaData.ControlByte   = 0x43;
	ExpandParaData.AddressLen    = 0x06;
	ExpandParaData.ClientAddress = 0xA0;
	ExpandParaData.AddressType   = 0x00;
	ExpandParaData.Address       = Addr;
	ExpandParaData.pAPDU         = APDU;
	ExpandParaData.pAPDULen      = APDULen;
	ExpandParaData.APDUMAXLen    = BufLen;

	if(tMidFunction.MidDataFunction(&ExpandParaData))
	{
		if(ExpandParaData.pAPDULen > 10)
		{
			return (ExpandParaData.pAPDULen - 10);
		}
	}

	return 0;
}

//--------------------------------------------------
// ��������:  ��չ͸������
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BYTE api_ExtAgencyPro(BYTE *APDU, BYTE APDULen, WORD BufLen)
{
	TMidParaData ExpandParaData;
	BYTE Addr[10];

	if(wExtOI == 0)
	{
		return FALSE;
	}

	memset(Addr, 0x00, sizeof(Addr));
	memset(&ExpandParaData, 0x00, sizeof(ExpandParaData));
	ExpandParaData.ControlByte   = 0xC3;
	ExpandParaData.AddressLen    = 0x06;
	ExpandParaData.ClientAddress = 0xA0;
	ExpandParaData.ProxyFlag     = 0x55;
	ExpandParaData.AddressType   = 0x00;
	ExpandParaData.OI            = wExtOI;
	ExpandParaData.Address       = Addr;
	ExpandParaData.pAPDU         = APDU;
	ExpandParaData.pAPDULen      = APDULen;
	ExpandParaData.APDUMAXLen    = BufLen;

	tMidFunction.MidDataFunction(&ExpandParaData);
	wExtOI = 0;

	return TRUE;
}

//--------------------------------------------------
// ��������:�������ݳ���
//
// ����:	BYTE *ROAD    ROAD/OAD
//			BYTE size     ROAD�ֽ���
//			BYTE DataType
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
WORD GetOILen(BYTE *ROAD, BYTE DataType, BYTE size)
{
	BYTE Addr[10];
	BYTE APDUbufs[200];
	BYTE len             = 0;
	BYTE GetCmd[]        = {0x07, 0x01, 0x00, 0x73, 0x03, 0x02, 0x00};
	BYTE SimpleROADbuf[] = {0x52, 0x00, 0x00, 0x00, 0x00, 0x00};

	memcpy(APDUbufs, GetCmd, sizeof(GetCmd));
	// ����OAD���
	if(size == 4)
	{
		memcpy((SimpleROADbuf + 1), ROAD, size);
		ROAD = SimpleROADbuf;
		size = sizeof(SimpleROADbuf);
	}

	memcpy((APDUbufs + sizeof(GetCmd)), ROAD, size);
	APDUbufs[sizeof(GetCmd) + size] = 0;
	if(api_RequestModule(APDUbufs, (sizeof(GetCmd) + size + 1), sizeof(APDUbufs)) != 0)
	{
		if(DataType != eMaxData)
		{
			return (APDUbufs[12] * 256 + APDUbufs[13]);
		}
		else
		{
			return (APDUbufs[15] * 256 + APDUbufs[16]);
		}
	}
	else
	{
		return 0;
	}
}
//--------------------------------------------------
// ��������:  �ж��Ƿ�Ϊ��չģ��OI
//
// ����:      WORD OI  eModuleOIType
//
// ����ֵ:    BOOL
//
// ��ע:
//--------------------------------------------------
BOOL IsModuleOI(WORD OI, eModuleOIType ModuleOIType)
{
	BYTE i;
	switch(OI)
	{
	case 0x20EE:
	case 0x7303:
		return TRUE;
		break;

	default:
		break;
	}

	for(i = 0; i < tMidInfoData.OIData.OINum; i++)
	{
		if((OI == tMidInfoData.OIData.OIList[i].OI) && ((ModuleOIType == eModuleAllOI) || (tMidInfoData.OIData.OIList[i].OIClass & ModuleOIType) == ModuleOIType))
		{
			return TRUE;
		}
	}
	return FALSE;
}
//--------------------------------------------------
// ��������:  ��ȡ��Ӧ�洢�¼�OI
//
// ����:		WORD *OI,
//				BYTE Type	�������� 0����0xff	1�����󷵻ض�ӦOI
//
// ����ֵ:  ģ��������
//
// ��ע:
//--------------------------------------------------
BYTE FindThisEventSaveOI(WORD *OI, BYTE Type, eMappingType MappingType)
{
	BYTE i;
	DWORD Inpara     = 0;
	WORD *pReally_OI = NULL, *pSave_OI = NULL;
	CheckOI_OIMapping();
	if(MappingType == eEventMap)
	{
		pReally_OI = tOI_OIMapping.Event_Really_OI;
		pSave_OI   = tOI_OIMapping.Event_Save_OI;
	}
	else
	{
		pReally_OI = tOI_OIMapping.Para_Really_OI;
		pSave_OI   = tOI_OIMapping.Para_Save_OI;
	}
	for(i = 0; i < MODULE_EVENT_NUM; i++)
	{
		if((*OI == pReally_OI[i]))
		{
			*OI = pSave_OI[i];
			return i;
		}
	}
	if(Type == 1)   // δ���ҵ�  ��������ʼ��ee�ռ�
	{
		for(i = 0; i < MODULE_EVENT_NUM; i++)
		{
			if((0xffff == pReally_OI[i]))
			{
				pReally_OI[i] = *OI;
				*OI           = pSave_OI[i];
				api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem, Mod_Event_OI_OIMapping), sizeof(tOI_OIMapping), (BYTE *)&tOI_OIMapping);
				if((*OI >= 0xBD00) || (*OI <= 0xBD09))
				{
					api_WriteEventAttribute(*OI, 0xff, (BYTE *)&Inpara, 0);
				}

				return i;
			}
		}
	}

	return 0XFF;
}
//--------------------------------------------------
// ��������:  ��ȡ��Ӧ��ʵOI
//
// ����:		WORD *OI,
//
// ����ֵ:  ģ��������
//
// ��ע:
//--------------------------------------------------
BYTE FindThisEventReallyOI(WORD *OI)
{
	BYTE i;

	CheckOI_OIMapping();
	for(i = 0; i < MODULE_EVENT_NUM; i++)
	{
		if((*OI == tOI_OIMapping.Event_Save_OI[i]))
		{
			*OI = tOI_OIMapping.Event_Really_OI[i];
			return i;
		}
	}
	return 0XFF;
}
//--------------------------------------------------
// ��������:  ����ģ�����
//
// ����:	BYTE* pOMD
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BYTE ModuleEventFunc(BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI, DataLen;
	TFourByteUnion StartAddr;
	BYTE *pData;
	BYTE No;
	BYTE Outbuf[2];
	BYTE EESavetemp[PARA_SAVE_SIZE];   // ÿ������1k�洢�ռ�
	lib_ExchangeData(OI.b, pOMD, 2);   // ȡOI
	switch(OI.w)
	{
	case 0x20EE:                                         // �¼�
		pData = pOMD + 4;                                // ȡdata
		lib_ExchangeData(OI.b, (pData + 3), 2);          // ȡ�����ʶOI
		No = FindThisEventSaveOI(&OI.w, 1, eEventMap);   // ��ȡOIӳ��
		if(No == 0xff)
		{
			Outbuf[0] = DAR_RefuseOp;
			api_DLT698AddBuf(Ch, 0x55, Outbuf, 1);
			return 3;   // eAPP_ERR_RESPONSE
		}
		lib_ExchangeData(OI.b, (pData + 3), 2);   // ��ȡ�����ʶOI
		if(*(pData + 5) == 0x00)                  // �¼�������¼��Ԫ
		{
			SaveModuleRecord(EVENT_START, No, pData + 6, OI.w);
		}
		else if(*(pData + 5) == 0x01)   // �¼�������¼��Ԫ
		{
			SaveModuleRecord(EVENT_END, No, pData + 6, OI.w);
		}
		Outbuf[0] = Outbuf[1] = DAR_Success;
		return api_DLT698AddBuf(Ch, 0x55, Outbuf, 2);
		break;
	case 0x7303:
		pData = pOMD + 4;                                // ȡdata
		lib_ExchangeData(OI.b, (pData + 3), 2);          // ȡ�����ʶOI
		lib_ExchangeData(StartAddr.b, (pData + 6), 8);   // ȡ��ʼ��ַ
		lib_ExchangeData(DataLen.b, (pData + 15), 2);    // ȡ��������
		No = FindThisEventSaveOI(&OI.w, 1, eParaMap);    // ��ȡOIӳ��
		if((No == 0xff) || (OI.w < 0xBE00) || (OI.w > 0xBE09) || ((StartAddr.d + DataLen.w) > PARA_SAVE_SIZE))
		{
			Outbuf[0] = DAR_RefuseOp;
			api_DLT698AddBuf(Ch, 0x55, Outbuf, 1);
			return 3;   // eAPP_ERR_RESPONSE
		}
		CLEAR_WATCH_DOG;
		api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(ExtModuleEEpara[OI.w - 0xBE00]), sizeof(EESavetemp), (BYTE *)&EESavetemp);

		if(pOMD[2] == 0x03)   // ��
		{
			CLEAR_WATCH_DOG;
			memcpy(EESavetemp + StartAddr.d, (pData + 19), DataLen.w);
			api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(ExtModuleEEpara[OI.w - 0xBE00]), sizeof(EESavetemp), (BYTE *)&EESavetemp);
			Outbuf[0] = Outbuf[1] = DAR_Success;
			return api_DLT698AddBuf(Ch, 0x55, Outbuf, 2);
		}
		else if(pOMD[2] == 0x04)   // ȡ
		{
			Outbuf[0] = DAR_Success;
			Outbuf[1] = 1;   // �з�������
			api_DLT698AddBuf(Ch, 0x55, Outbuf, 2);
			Outbuf[0] = 0x09;   // Octet_Srting_698
			Outbuf[1] = DataLen.w;
			api_DLT698AddBuf(Ch, 0x55, Outbuf, 2);
			return api_DLT698AddBuf(Ch, 0x55, EESavetemp + StartAddr.d, DataLen.w);
		}

		break;
	default:
		break;
	}
	// ����ɲ���OI
	if(IsModuleOI(OI.w, eModuleOrdOI))
	{
		memcpy(EESavetemp, pOMD - 3, LinkData[Ch].pAPPLen.w);
		EESavetemp[LinkData[Ch].pAPPLen.w] = 0;
		api_RequestModule(EESavetemp, LinkData[Ch].pAPPLen.w + 1, sizeof(EESavetemp));   //+ʱ���ǩ00
		if(EESavetemp[7] == 0)
		{
			Outbuf[0] = Outbuf[1] = DAR_Success;
			return api_DLT698AddBuf(Ch, 0x55, Outbuf, 2);
		}
	}

	return DAR_RefuseOp;
}

//-----------------------------------------------
// ��������: �����¼���¼
//
// ����:
//	Type[in]:	EVENT_START:	�¼���ʼ����������ʱ�ÿ�ʼ
//				EVENT_END:		�¼��������Ͽ�Ӧ�����ӻ��ߵ���ʱ�ý���
//  No[in]:		��չ�¼����
//	buf[in]:	ָ�����ݵ�ָ��
//
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
// ��ע:
//-----------------------------------------------
BOOL SaveModuleRecord(BYTE Type, BYTE No, BYTE *buf, WORD OI)
{
	// ��ȡ�¼���¼��Ԫ����
	BYTE ROAD[] = {0x52, 0x00, 0x00, 0x02, 0x00, 0x01, 0x33, 0xE0, 0x00, 0x00};

	ROAD[1] = OI / 256;
	ROAD[2] = OI % 256;
	if(Type == EVENT_START)
	{
		ROAD[8] = 0x06;
	}
	else
	{
		ROAD[8] = 0x07;
	}
	if(buf[1] == 2)   // ����struct
	{
		buf += 3;
	}

	if(buf[1] < 0x80)
	{
		*(WORD *)ModuleEventBuf = buf[1] + 4;
	}
	else if(buf[1] == 0x81)
	{
		*(WORD *)ModuleEventBuf = buf[2] + 1 + 4;
	}
	else if(buf[1] == 0x82)
	{
		*(WORD *)ModuleEventBuf = buf[2] * 256 + buf[3] + 2 + 4;
	}

	// *(WORD*)ModuleEventBuf = GetOILen(ROAD, eTagData, sizeof(ROAD)) + 2; // ����ǰ��λΪ���ݳ���
	// if(*(WORD*)ModuleEventBuf==2)
	// {
	// 	return FALSE;
	// }

	memcpy(ModuleEventBuf + 2, buf, *(WORD *)ModuleEventBuf - 2);   // ת������
	SaveInstantEventRecord(eSUB_EVENT_MODULE_0_NO + No, ePHASE_ALL, Type, eEVENT_ENDTIME_CURRENT);
	return TRUE;
}
//--------------------------------------------------
// ��������:  ��ʼ��ee�еĶ�Ӧ��ϵ����
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL api_ModparaInit(void)
{
	BYTE i;

	memset(&tOI_OIMapping, 0xff, sizeof(tOI_OIMapping));
	for(i = 0; i < MODULE_EVENT_NUM; i++)
	{
		tOI_OIMapping.Event_Save_OI[i] = 0xBD00 + i;
		tOI_OIMapping.Para_Save_OI[i]  = 0xBE00 + i;
	}
	api_ClrContinueEEPRom(GET_CONTINUE_ADDR(ExtModuleEEpara[0]), MODULE_EVENT_NUM * PARA_SAVE_SIZE);
	return api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem, Mod_Event_OI_OIMapping), sizeof(tOI_OIMapping), (BYTE *)&tOI_OIMapping);
}
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void CheckOI_OIMapping(void)
{
	if(!(tOI_OIMapping.CRC == lib_CheckCRC32((BYTE *)&tOI_OIMapping, sizeof(tOI_OIMapping) - 4)))
	{
		api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem, Mod_Event_OI_OIMapping), sizeof(tOI_OIMapping), (BYTE *)&tOI_OIMapping);
	}
}
