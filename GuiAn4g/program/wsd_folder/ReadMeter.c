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
TRelayChangeReason RelayChangeReason[METER_PHASE_NUM][2] = {0}; //继电器变化原因,0下标表示继电器打开,1下标表示继电器关闭
BYTE gRelayStatus[METER_PHASE_NUM] = {0}; //继电器状态

//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
#if (CYCLE_METER_READING == YES)
double MeterData[MAX_SAMPLE_CHIP][COLLECT_DOUBLE_DATA_ITEM_NUM]; //电表采集的数据存放处

//周期抄表项
const tMeterRead MeterReadOI[] =
{
	{0,1,0x02800002, 1, 2, 2, UNSIGNED , &MeterData[0][0]},			//频率
	{0,1,0x0201FF00,3,2,1,SIGNED,&MeterData[0][1]  },	//电压数据块	
	{0,1,0x0202FF00,3,3,3,SIGNED,&MeterData[0][4]  },	//电流数据块
	{0,1,0x0203FF00,4,3,4,SIGNED,&MeterData[0][7]  },	//有功功率数据块 //标准645读取3个字节,扩展645抄读4个字节
	{0,1,0x0204FF00,4,3,4,SIGNED,&MeterData[0][11]  },	//无功功率数据块
	{0,1,0x0205FF00,4,3,4,SIGNED,&MeterData[0][15] },//视在功率数据块
	{0,1,0x0206FF00,4,2,3,SIGNED,&MeterData[0][19] },//功率因数数据块
	{0,1,0x00010000, 1, 4, 2, SIGNED , &MeterData[0][23]},	//总正向有功电能
	{0,1,0x00150000, 1, 4, 2, SIGNED , &MeterData[0][24]},	//a相正向有功电能
	{0,1,0x00290000, 1, 4, 2, SIGNED , &MeterData[0][25]},	//b相正向有功电能
	{0,1,0x003D0000, 1, 4, 2, SIGNED , &MeterData[0][26]},	//c相正向有功电能
	{0,1,0x0010000, 1, 4, 2, SIGNED , &MeterData[0][27]},	//总反向有功电能
	{0,1,0x00160000, 1, 4, 2, SIGNED , &MeterData[0][28]},	//a相反向有功电能
	{0,1,0x002A0000, 1, 4, 2, SIGNED , &MeterData[0][29]},	//b相反向有功电能
	{0,1,0x003E0000, 1, 4, 2, SIGNED , &MeterData[0][30]},	//c相反向有功电能
	{0,0,LOOP1_RELAY_OPEN_INFO,1,34,0,SIGNED,&RelayChangeReason[0][0]},//回路1上一次继电器打开原因
	{0,0,LOOP1_RELAY_CLOSE_INFO,1,34,0,SIGNED,&RelayChangeReason[0][1]},//回路1上一次继电器关闭原因
	{0,0,LOOP2_RELAY_OPEN_INFO,1,34,0,SIGNED,&RelayChangeReason[1][0]},//回路2上一次继电器打开原因
	{0,0,LOOP2_RELAY_CLOSE_INFO,1,34,0,SIGNED,&RelayChangeReason[1][1]},//回路2上一次继电器关闭原因
	{0,0,LOOP3_RELAY_OPEN_INFO,1,34,0,SIGNED,&RelayChangeReason[2][0]},//回路3上一次继电器打开原因
	{0,0,LOOP3_RELAY_CLOSE_INFO,1,34,0,SIGNED,&RelayChangeReason[2][1]},//回路3上一次继电器关闭原因
	{1,0,0xDBDF1126,1,3,0,UNSIGNED,&gRelayStatus[0]  },	//继电器状态
};
#endif //if(CYCLE_METER_READING == YES)
#if (CYCLE_METER_READING == YES)
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
#if (CYCLE_METER_READING == YES)
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
		memcpy((double *)&CollectValue.CollectMeter[i][0], (double *)&MeterData[i][0], (COLLECT_DOUBLE_DATA_ITEM_NUM) * sizeof(double));
		for (BYTE j = 0; j < (COLLECT_DOUBLE_DATA_ITEM_NUM); j++)
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