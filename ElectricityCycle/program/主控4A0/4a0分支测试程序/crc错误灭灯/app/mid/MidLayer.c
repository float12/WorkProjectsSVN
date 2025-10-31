//----------------------------------------------------------------------
// Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表低压台区产品部
//
// 创建人
//
// 创建日期
//
// 描述
//
// 修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "Mid_api.h"
#include "mid.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
TMeterFun tMeterFun;         // 导轨表本体提供的回调函数
TMidInfoData tMidInfoData;   // 扩展功能支持的OI(个数+列表)
TMidFunction tMidFunction;   // 扩展功能函数指针
TMOD_EVENT_OI_OIMapping tOI_OIMapping;
BYTE ModuleEventBuf[0x304];
WORD wExtOI;

//-----------------------------------------------
//			全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//			本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//			内部函数声明
//-----------------------------------------------
BOOL SaveModuleRecord(BYTE Type, BYTE No, BYTE *pOad, WORD OI);
void CheckOI_OIMapping(void);
extern void api_ExtRxMonitor(TMidParaData *ParaData);
extern void api_ExtAgent(TMidParaData *MidParaData);
//-----------------------------------------------
//			函数定义
//-----------------------------------------------

//-----------------------------------------------
// 函数功能: 判定报文是否为终端业务
//
// 参数:
//
// 返回值:   成功/失败
//
// 备注:  实现通过报文格式的结构体，可以获取数据
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
// 函数功能: 回调数据功能实现函数
//
// 参数: 	MidParaData[in/out]:输入结构体
//
// 返回值:   成功/失败
//
// 备注:  实现通过报文格式的结构体，可以获取数据
//-----------------------------------------------
Mid_WORD MeterDataFun(TMidParaData *MidParaData)
{
//	if( (MidParaData->pAPDU[0] == 0x09) 
//	/*&& (MidParaData->pAPDU[1] == 0x07)
//	&& (MidParaData->ProxyFlag == 0x55) */)	//代理
//	{
//		// 转发
//		// api_ExtAgent(MidParaData);
//		wExtOI = MidParaData->OI;
//	}
//	// else if(IsTerminalPro(MidParaData->pAPDU) == TRUE)
//	//{
//	//	api_ExtRxMonitor(MidParaData);
//	// }
//	// else
////	{
////		LinkData[eEXP_PRO].ControlByte   = MidParaData->ControlByte;   // 填充控制字节
////		LinkData[eEXP_PRO].AddressType   = (eAddressType)MidParaData->AddressType;
////		LinkData[eEXP_PRO].AddressLen    = MidParaData->AddressLen;   // 填充地址长度
////		LinkData[eEXP_PRO].ClientAddress = MidParaData->ClientAddress;
////		LinkData[eEXP_PRO].eLinkDataFlag = eAPP_OK;
////		LinkData[eEXP_PRO].pAPP          = MidParaData->pAPDU;
////		LinkData[eEXP_PRO].pAPPLen.w     = MidParaData->pAPDULen;
//		// 处理报文
//		memset(&APPData[eEXP_PRO].APPFlag, 0x00, sizeof(TDLT698APPData));   // 初始化应用层结构体
//		InitDlt698APP(eEXP_PRO);
//		ProcDlt698Apdu(eEXP_PRO);
//
//		if(APPData[eEXP_PRO].APPFlag != APP_NO_DATA)
//		{
//			// MidParaData->ControlByte = APPData[eEXP_PRO].ControlByte;		//填充控制字节
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
// 函数功能: 初始化
//
// 参数: 	无
//
// 返回值:   无
//
// 备注:   完成函数注册
//-----------------------------------------------
void api_MidLayerInit(void)
{
	// 完成回调函数实现函数
	tMeterFun.MeterDataFun = MeterDataFun;

	wExtOI = 0;

	// 初始化OI结构体
	memset(&tMidInfoData, 0x00, sizeof(tMidInfoData));
	// 初始化事件标志
	memset(ModuleEventBuf, 0x00, sizeof(ModuleEventBuf));
	// 获取事件OI对应表
	CheckOI_OIMapping();
	// 调用接口函数
	LoadMidEnterFunction(tMeterFun, &tMidInfoData, &tMidFunction);
}

//-----------------------------------------------
// 函数功能: 任务函数
//
// 参数: 	无
//
// 返回值:   无
//
// 备注:   执行多个任务函数
//-----------------------------------------------
void api_MidLayerTask(void)
{
	BYTE i, bTaskType;
	eMidTimeTaskType TimeType;

	CLEAR_WATCH_DOG;

	// 循环任务
	bTaskType = (1 << eMidLoopMode);

	// 秒任务
	if(api_GetTaskFlag(eTASK_MID_ID, eFLAG_SECOND) == TRUE)
	{
		bTaskType |= (1 << eMidSecMode);
		api_ClrTaskFlag(eTASK_MID_ID, eFLAG_SECOND);
	}

	// 分钟任务
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
// 功能描述:  获取拓展模块数据
//
// 参数:
//
// 返回值:
//
// 备注:
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
// 功能描述:  扩展透传报文
//
// 参数:
//
// 返回值:
//
// 备注:
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
// 功能描述:返回数据长度
//
// 参数:	BYTE *ROAD    ROAD/OAD
//			BYTE size     ROAD字节数
//			BYTE DataType
//
// 返回值:
//
// 备注:
//--------------------------------------------------
WORD GetOILen(BYTE *ROAD, BYTE DataType, BYTE size)
{
	BYTE Addr[10];
	BYTE APDUbufs[200];
	BYTE len             = 0;
	BYTE GetCmd[]        = {0x07, 0x01, 0x00, 0x73, 0x03, 0x02, 0x00};
	BYTE SimpleROADbuf[] = {0x52, 0x00, 0x00, 0x00, 0x00, 0x00};

	memcpy(APDUbufs, GetCmd, sizeof(GetCmd));
	// 处理OAD情况
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
// 功能描述:  判断是否为拓展模块OI
//
// 参数:      WORD OI  eModuleOIType
//
// 返回值:    BOOL
//
// 备注:
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
// 功能描述:  获取对应存储事件OI
//
// 参数:		WORD *OI,
//				BYTE Type	若不存在 0返回0xff	1创建后返回对应OI
//
// 返回值:  模块设计序号
//
// 备注:
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
	if(Type == 1)   // 未查找到  创建并初始化ee空间
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
// 功能描述:  获取对应真实OI
//
// 参数:		WORD *OI,
//
// 返回值:  模块设计序号
//
// 备注:
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
// 功能描述:  处理模块操作
//
// 参数:	BYTE* pOMD
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BYTE ModuleEventFunc(BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI, DataLen;
	TFourByteUnion StartAddr;
	BYTE *pData;
	BYTE No;
	BYTE Outbuf[2];
	BYTE EESavetemp[PARA_SAVE_SIZE];   // 每个程序1k存储空间
	lib_ExchangeData(OI.b, pOMD, 2);   // 取OI
	switch(OI.w)
	{
	case 0x20EE:                                         // 事件
		pData = pOMD + 4;                                // 取data
		lib_ExchangeData(OI.b, (pData + 3), 2);          // 取对象标识OI
		No = FindThisEventSaveOI(&OI.w, 1, eEventMap);   // 获取OI映射
		if(No == 0xff)
		{
			Outbuf[0] = DAR_RefuseOp;
			api_DLT698AddBuf(Ch, 0x55, Outbuf, 1);
			return 3;   // eAPP_ERR_RESPONSE
		}
		lib_ExchangeData(OI.b, (pData + 3), 2);   // 重取对象标识OI
		if(*(pData + 5) == 0x00)                  // 事件发生记录单元
		{
			SaveModuleRecord(EVENT_START, No, pData + 6, OI.w);
		}
		else if(*(pData + 5) == 0x01)   // 事件结束记录单元
		{
			SaveModuleRecord(EVENT_END, No, pData + 6, OI.w);
		}
		Outbuf[0] = Outbuf[1] = DAR_Success;
		return api_DLT698AddBuf(Ch, 0x55, Outbuf, 2);
		break;
	case 0x7303:
		pData = pOMD + 4;                                // 取data
		lib_ExchangeData(OI.b, (pData + 3), 2);          // 取对象标识OI
		lib_ExchangeData(StartAddr.b, (pData + 6), 8);   // 取起始地址
		lib_ExchangeData(DataLen.b, (pData + 15), 2);    // 取操作长度
		No = FindThisEventSaveOI(&OI.w, 1, eParaMap);    // 获取OI映射
		if((No == 0xff) || (OI.w < 0xBE00) || (OI.w > 0xBE09) || ((StartAddr.d + DataLen.w) > PARA_SAVE_SIZE))
		{
			Outbuf[0] = DAR_RefuseOp;
			api_DLT698AddBuf(Ch, 0x55, Outbuf, 1);
			return 3;   // eAPP_ERR_RESPONSE
		}
		CLEAR_WATCH_DOG;
		api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(ExtModuleEEpara[OI.w - 0xBE00]), sizeof(EESavetemp), (BYTE *)&EESavetemp);

		if(pOMD[2] == 0x03)   // 存
		{
			CLEAR_WATCH_DOG;
			memcpy(EESavetemp + StartAddr.d, (pData + 19), DataLen.w);
			api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(ExtModuleEEpara[OI.w - 0xBE00]), sizeof(EESavetemp), (BYTE *)&EESavetemp);
			Outbuf[0] = Outbuf[1] = DAR_Success;
			return api_DLT698AddBuf(Ch, 0x55, Outbuf, 2);
		}
		else if(pOMD[2] == 0x04)   // 取
		{
			Outbuf[0] = DAR_Success;
			Outbuf[1] = 1;   // 有返回数据
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
	// 处理可操作OI
	if(IsModuleOI(OI.w, eModuleOrdOI))
	{
		memcpy(EESavetemp, pOMD - 3, LinkData[Ch].pAPPLen.w);
		EESavetemp[LinkData[Ch].pAPPLen.w] = 0;
		api_RequestModule(EESavetemp, LinkData[Ch].pAPPLen.w + 1, sizeof(EESavetemp));   //+时间标签00
		if(EESavetemp[7] == 0)
		{
			Outbuf[0] = Outbuf[1] = DAR_Success;
			return api_DLT698AddBuf(Ch, 0x55, Outbuf, 2);
		}
	}

	return DAR_RefuseOp;
}

//-----------------------------------------------
// 函数功能: 保存事件记录
//
// 参数:
//	Type[in]:	EVENT_START:	事件开始，设置数据时置开始
//				EVENT_END:		事件结束，断开应用连接或者掉电时置结束
//  No[in]:		拓展事件编号
//	buf[in]:	指向数据的指针
//
//  返回值:	TRUE   正确
//			FALSE  错误
//
// 备注:
//-----------------------------------------------
BOOL SaveModuleRecord(BYTE Type, BYTE No, BYTE *buf, WORD OI)
{
	// 获取事件记录单元长度
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
	if(buf[1] == 2)   // 兼容struct
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

	// *(WORD*)ModuleEventBuf = GetOILen(ROAD, eTagData, sizeof(ROAD)) + 2; // 增加前两位为数据长度
	// if(*(WORD*)ModuleEventBuf==2)
	// {
	// 	return FALSE;
	// }

	memcpy(ModuleEventBuf + 2, buf, *(WORD *)ModuleEventBuf - 2);   // 转存数据
	SaveInstantEventRecord(eSUB_EVENT_MODULE_0_NO + No, ePHASE_ALL, Type, eEVENT_ENDTIME_CURRENT);
	return TRUE;
}
//--------------------------------------------------
// 功能描述:  初始化ee中的对应关系参数
//
// 参数:
//
// 返回值:
//
// 备注:
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
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void CheckOI_OIMapping(void)
{
	if(!(tOI_OIMapping.CRC == lib_CheckCRC32((BYTE *)&tOI_OIMapping, sizeof(tOI_OIMapping) - 4)))
	{
		api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem, Mod_Event_OI_OIMapping), sizeof(tOI_OIMapping), (BYTE *)&tOI_OIMapping);
	}
}
