//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE gRelayStatus[METER_PHASE_NUM] = {0}; //继电器状态

//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
#if (CYCLE_METER_READING == PROTOCOL_645)
double MeterData[MAX_SAMPLE_CHIP][COLLECT_DATA_ITEM_NUM]; //电表采集的数据存放处

//周期抄表项-645
const tMeterRead MeterReadOI[] =
	{
		{0x0201FF00, 3, 2, 1, PT_MULTI, &MeterData[0][0]},										//电压数据块
		// {0x0211FF00, 3, 2, 1, PT_MULTI, &MeterData[0][3]},									//线电压数据块
		{0x0202FF00, 3, 3, 3, SIGNED | CT_MULTI, &MeterData[0][3]},								//电流数据块
		{0x02800001, 1, 3, 3, SIGNED | CT_MULTI, &MeterData[0][6]},								//零序电流
		{0x020CFF00, 4, 4, 4, SIGNED | PT_CT_MULTI , &MeterData[0][7]},							//有功功率数据块 //标准645读取3个字节,扩展645抄读4个字节
		{0x020DFF00, 4, 4, 4, SIGNED | PT_CT_MULTI , &MeterData[0][11]},						//无功功率数据块
		{0x020EFF00, 4, 4, 4, SIGNED | PT_CT_MULTI , &MeterData[0][15]},						//视在功率数据块
		{0x0206FF00, 4, 2, 3, SIGNED | PT_CT_MULTI , &MeterData[0][19]},						//功率因数数据块
		{0x00000000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM]},			//组合有功总电能
		{0x00010000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM + 1]},		//正向有功总电能
		{0x00020000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM + 2]},		//反向有功总电能
		{0x00030000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM + 3]},		//组合无功1
		{0x00040000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM + 4]},		//组合无功2
		// {0x00150000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 5]},	 				//a相正向有功
		// {0x00160000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 6]},	 				//a相反向有功
		// {0x00170000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 7]},	 				//a相组合无功1
		// {0x00180000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 8]},	 				//a相组合无功2
		// {0x00290000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 9]},	 				//b相正向有功
		// {0x002A0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 10]},					//b相反向有功
		// {0x002B0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 11]},					//b相组合无功1
		// {0x002C0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 12]},					//b相组合无功2
		// {0x003D0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 13]},					//c相正向有功
		// {0x003E0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 14]},					//c相反向有功
		// {0x003F0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 15]},					//c相组合无功1
		// {0x00400000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 16]},					//c相组合无功2
		// {0x02A00200, 1, 2, 1, UNSIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 17]},				//温度1
		// {0x02A00300, 1, 2, 1, UNSIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 18]},				//温度2
		// {0x02A00400, 1, 2, 1, UNSIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 19]},				//温度3
		// {0x02A00500, 1, 2, 1, UNSIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 20]},				//温度4
};
#endif //if(CYCLE_METER_READING == PROTOCOL_645)
#if (CYCLE_METER_READING == PROTOCOL_645)
const BYTE CycleReadMeterNum = sizeof(MeterReadOI) / sizeof(tMeterRead);
#else
const BYTE CycleReadMeterNum = 0;
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
#if (CYCLE_METER_READING == PROTOCOL_645)
//-----------------------------------------------
//功能描述:  抄表数据解析并保存
//
//参数:      pBuf[in]  dataBuf[in] 数据保存位置
//
//返回值:
//
//备注:
//--------------------------------------------------
void DataAnalyseandSave(BYTE *pBuf, double *dataBuf, BYTE Len, tMeterRead tMeterReadOIRam)
{
	BYTE ID[4];
	BYTE i, j, bStart, bEnd, bOffset, bSymbol, bDot;
	int symbol[4] = {1, 1, 1, 1};
	SDWORD temp[MAX_RATIO] = {0}; //数组大小按最大费率数开
	char Buf[MAX_RATIO][10] = {0};
	int value = 0;
	double dwData = 0;
	j = 0;

	memcpy((BYTE *)&ID[0], (BYTE *)&tMeterReadOIRam.OI, sizeof(DWORD));
	bStart = tMeterReadOIRam.Slen - 1;
	bOffset = tMeterReadOIRam.Slen;

	if (Len < (tMeterReadOIRam.Slen * tMeterReadOIRam.Num)) //用于区分单三相
	{
		bEnd = Len;
	}
	else
	{
		bEnd = tMeterReadOIRam.Slen * tMeterReadOIRam.Num;
	}
	bSymbol = tMeterReadOIRam.Symbol;
	bDot = tMeterReadOIRam.Dot;

	for (i = bStart; i < bEnd;)
	{
		if ((bSymbol & 0x80) == 0x80)
		{
			if ((pBuf[i] & 0x80) == 0x80) //最高位的符号处理
			{
				pBuf[i] &= 0x7F; //符号处理
				symbol[j] = -1;
			}
		}
		memcpy((BYTE *)&temp[j], (BYTE *)&pBuf[i - bStart], bOffset);
		sprintf((char *)Buf[j], "%lx", temp[j]);
		if ((bSymbol & 0x80) == 0x80)
		{
			value = atol(Buf[j]) * symbol[j];
		}
		else
		{
			value = atol(Buf[j]);
		}
		dwData = value / (1.0 * pow(10, bDot));
		//在这之后可能会乘上变比 存放double数组// 四回路无扩展规约读变比，单回路有
		if ((bSymbol & 0x0F) == PT_MULTI)
		{
			dwData = dwData * api_GetPtandCT(eType_PT);
		}
		else if ((bSymbol & 0x0F) == CT_MULTI)
		{
			dwData = dwData * api_GetPtandCT(eType_CT);
		}
		else if ((bSymbol & 0x0F) == PT_CT_MULTI)
		{
			dwData = dwData * api_GetPtandCT(eType_PT_CT);
		}

		//sprintf();乘上变比后 末位有无尽小数 统一传固定小数
		// nwy_ext_echo("\r\n %f",dwData);
		memcpy(dataBuf, &dwData, sizeof(double));
		dataBuf++;
		i = i + bOffset;
		j++;
	}
}
//--------------------------------------------------
//功能描述: 计量数据传送给不同上报组帧任务
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void SampleDatatoReport(TRealTimer *pTime)
{
	BYTE i;
	QWORD qwMs;
	CollectionDatas CollectValue;
	char heapinfo[100] = {0};
	int iTmp;
	for (i = 0; i < bUsedChannelNum; i++)
	{
		memcpy((double *)&CollectValue.CollectMeter[i][0], (double *)&MeterData[i][0], (COLLECT_DATA_ITEM_NUM) * sizeof(double));
		for (BYTE j = 0; j < (COLLECT_DATA_ITEM_NUM); j++)
		{
			// nwy_ext_echo("\r\n No %d  : %f", j+1,CollectValue.CollectMeter[i][j]);
		}
	}
	memcpy(CollectValue.Addr, LtuAddr, sizeof(LtuAddr));
	// 时间戳
	// pTime->Sec = 0;
	qwMs = getmktimems(pTime);
	CollectValue.TimeMs = qwMs;
	#if (EVENT_REPORT == YES)
	CollectValue.bDI = bHisDIStatus;
	#endif
	nwy_ext_echo("\r\n hour is %d", pTime->Hour);
	nwy_ext_echo("\r\n min is %d", pTime->Min);
	nwy_dm_get_device_heapinfo(heapinfo);
	iTmp = nwy_sdk_vfs_ls();
	nwy_ext_echo("RAM[%s]ROM[%d]", heapinfo, iTmp);
	if (nwy_get_queue_spaceevent_cnt(CollectMessageQueue) != 0)
	{
		nwy_put_msg_que(CollectMessageQueue, &CollectValue, 0xffffffff);
	}
}
#endif