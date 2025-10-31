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
TRelayChangeReason RelayChangeReason[METER_PHASE_NUM][2] = {0}; //�̵����仯ԭ��,0�±��ʾ�̵�����,1�±��ʾ�̵����ر�
BYTE gRelayStatus[METER_PHASE_NUM] = {0}; //�̵���״̬

//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------
#if (CYCLE_METER_READING == YES)
double MeterData[MAX_SAMPLE_CHIP][COLLECT_DOUBLE_DATA_ITEM_NUM]; //���ɼ������ݴ�Ŵ�

//���ڳ�����
const tMeterRead MeterReadOI[] =
{
	{0,1,0x02800002, 1, 2, 2, UNSIGNED , &MeterData[0][0]},			//Ƶ��
	{0,1,0x0201FF00,3,2,1,SIGNED,&MeterData[0][1]  },	//��ѹ���ݿ�	
	{0,1,0x0202FF00,3,3,3,SIGNED,&MeterData[0][4]  },	//�������ݿ�
	{0,1,0x0203FF00,4,3,4,SIGNED,&MeterData[0][7]  },	//�й��������ݿ� //��׼645��ȡ3���ֽ�,��չ645����4���ֽ�
	{0,1,0x0204FF00,4,3,4,SIGNED,&MeterData[0][11]  },	//�޹��������ݿ�
	{0,1,0x0205FF00,4,3,4,SIGNED,&MeterData[0][15] },//���ڹ������ݿ�
	{0,1,0x0206FF00,4,2,3,SIGNED,&MeterData[0][19] },//�����������ݿ�
	{0,1,0x00010000, 1, 4, 2, SIGNED , &MeterData[0][23]},	//�������й�����
	{0,1,0x00150000, 1, 4, 2, SIGNED , &MeterData[0][24]},	//a�������й�����
	{0,1,0x00290000, 1, 4, 2, SIGNED , &MeterData[0][25]},	//b�������й�����
	{0,1,0x003D0000, 1, 4, 2, SIGNED , &MeterData[0][26]},	//c�������й�����
	{0,1,0x0010000, 1, 4, 2, SIGNED , &MeterData[0][27]},	//�ܷ����й�����
	{0,1,0x00160000, 1, 4, 2, SIGNED , &MeterData[0][28]},	//a�෴���й�����
	{0,1,0x002A0000, 1, 4, 2, SIGNED , &MeterData[0][29]},	//b�෴���й�����
	{0,1,0x003E0000, 1, 4, 2, SIGNED , &MeterData[0][30]},	//c�෴���й�����
	{0,0,LOOP1_RELAY_OPEN_INFO,1,34,0,SIGNED,&RelayChangeReason[0][0]},//��·1��һ�μ̵�����ԭ��
	{0,0,LOOP1_RELAY_CLOSE_INFO,1,34,0,SIGNED,&RelayChangeReason[0][1]},//��·1��һ�μ̵����ر�ԭ��
	{0,0,LOOP2_RELAY_OPEN_INFO,1,34,0,SIGNED,&RelayChangeReason[1][0]},//��·2��һ�μ̵�����ԭ��
	{0,0,LOOP2_RELAY_CLOSE_INFO,1,34,0,SIGNED,&RelayChangeReason[1][1]},//��·2��һ�μ̵����ر�ԭ��
	{0,0,LOOP3_RELAY_OPEN_INFO,1,34,0,SIGNED,&RelayChangeReason[2][0]},//��·3��һ�μ̵�����ԭ��
	{0,0,LOOP3_RELAY_CLOSE_INFO,1,34,0,SIGNED,&RelayChangeReason[2][1]},//��·3��һ�μ̵����ر�ԭ��
	{1,0,0xDBDF1126,1,3,0,UNSIGNED,&gRelayStatus[0]  },	//�̵���״̬
};
#endif //if(CYCLE_METER_READING == YES)
#if (CYCLE_METER_READING == YES)
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
#if (CYCLE_METER_READING == YES)
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
		memcpy((double *)&CollectValue.CollectMeter[i][0], (double *)&MeterData[i][0], (COLLECT_DOUBLE_DATA_ITEM_NUM) * sizeof(double));
		for (BYTE j = 0; j < (COLLECT_DOUBLE_DATA_ITEM_NUM); j++)
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
	api_SetSysStatus(eSYS_STASUS_UART_AVAILABLE);
}
#endif