//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE gRelayStatus[METER_PHASE_NUM] = {0}; //�̵���״̬

//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------
#if (CYCLE_METER_READING == PROTOCOL_645)
double MeterData[MAX_SAMPLE_CHIP][COLLECT_DATA_ITEM_NUM]; //���ɼ������ݴ�Ŵ�

//���ڳ�����-645
const tMeterRead MeterReadOI[] =
	{
		{0x0201FF00, 3, 2, 1, PT_MULTI, &MeterData[0][0]},										//��ѹ���ݿ�
		// {0x0211FF00, 3, 2, 1, PT_MULTI, &MeterData[0][3]},									//�ߵ�ѹ���ݿ�
		{0x0202FF00, 3, 3, 3, SIGNED | CT_MULTI, &MeterData[0][3]},								//�������ݿ�
		{0x02800001, 1, 3, 3, SIGNED | CT_MULTI, &MeterData[0][6]},								//�������
		{0x020CFF00, 4, 4, 4, SIGNED | PT_CT_MULTI , &MeterData[0][7]},							//�й��������ݿ� //��׼645��ȡ3���ֽ�,��չ645����4���ֽ�
		{0x020DFF00, 4, 4, 4, SIGNED | PT_CT_MULTI , &MeterData[0][11]},						//�޹��������ݿ�
		{0x020EFF00, 4, 4, 4, SIGNED | PT_CT_MULTI , &MeterData[0][15]},						//���ڹ������ݿ�
		{0x0206FF00, 4, 2, 3, SIGNED | PT_CT_MULTI , &MeterData[0][19]},						//�����������ݿ�
		{0x00000000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM]},			//����й��ܵ���
		{0x00010000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM + 1]},		//�����й��ܵ���
		{0x00020000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM + 2]},		//�����й��ܵ���
		{0x00030000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM + 3]},		//����޹�1
		{0x00040000, 1, 4, 2, SIGNED | PT_CT_MULTI , &MeterData[0][SINGLE_LOOP_ITEM + 4]},		//����޹�2
		// {0x00150000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 5]},	 				//a�������й�
		// {0x00160000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 6]},	 				//a�෴���й�
		// {0x00170000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 7]},	 				//a������޹�1
		// {0x00180000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 8]},	 				//a������޹�2
		// {0x00290000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 9]},	 				//b�������й�
		// {0x002A0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 10]},					//b�෴���й�
		// {0x002B0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 11]},					//b������޹�1
		// {0x002C0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 12]},					//b������޹�2
		// {0x003D0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 13]},					//c�������й�
		// {0x003E0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 14]},					//c�෴���й�
		// {0x003F0000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 15]},					//c������޹�1
		// {0x00400000, 1, 4, 2, SIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 16]},					//c������޹�2
		// {0x02A00200, 1, 2, 1, UNSIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 17]},				//�¶�1
		// {0x02A00300, 1, 2, 1, UNSIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 18]},				//�¶�2
		// {0x02A00400, 1, 2, 1, UNSIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 19]},				//�¶�3
		// {0x02A00500, 1, 2, 1, UNSIGNED, &MeterData[0][SINGLE_LOOP_ITEM + 20]},				//�¶�4
};
#endif //if(CYCLE_METER_READING == PROTOCOL_645)
#if (CYCLE_METER_READING == PROTOCOL_645)
const BYTE CycleReadMeterNum = sizeof(MeterReadOI) / sizeof(tMeterRead);
#else
const BYTE CycleReadMeterNum = 0;
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
#if (CYCLE_METER_READING == PROTOCOL_645)
//-----------------------------------------------
//��������:  �������ݽ���������
//
//����:      pBuf[in]  dataBuf[in] ���ݱ���λ��
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void DataAnalyseandSave(BYTE *pBuf, double *dataBuf, BYTE Len, tMeterRead tMeterReadOIRam)
{
	BYTE ID[4];
	BYTE i, j, bStart, bEnd, bOffset, bSymbol, bDot;
	int symbol[4] = {1, 1, 1, 1};
	SDWORD temp[MAX_RATIO] = {0}; //�����С������������
	char Buf[MAX_RATIO][10] = {0};
	int value = 0;
	double dwData = 0;
	j = 0;

	memcpy((BYTE *)&ID[0], (BYTE *)&tMeterReadOIRam.OI, sizeof(DWORD));
	bStart = tMeterReadOIRam.Slen - 1;
	bOffset = tMeterReadOIRam.Slen;

	if (Len < (tMeterReadOIRam.Slen * tMeterReadOIRam.Num)) //�������ֵ�����
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
			if ((pBuf[i] & 0x80) == 0x80) //���λ�ķ��Ŵ���
			{
				pBuf[i] &= 0x7F; //���Ŵ���
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
		//����֮����ܻ���ϱ�� ���double����// �Ļ�·����չ��Լ����ȣ�����·��
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

		//sprintf();���ϱ�Ⱥ� ĩλ���޾�С�� ͳһ���̶�С��
		// nwy_ext_echo("\r\n %f",dwData);
		memcpy(dataBuf, &dwData, sizeof(double));
		dataBuf++;
		i = i + bOffset;
		j++;
	}
}
//--------------------------------------------------
//��������: �������ݴ��͸���ͬ�ϱ���֡����
//
//����:
//
//����ֵ:
//
//��ע:
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
	// ʱ���
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