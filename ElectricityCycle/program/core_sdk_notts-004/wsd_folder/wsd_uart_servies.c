//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾��ѹ̨����Ʒ��
//������	��Ȫ
//��������
//����
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define BAUDRATE 				921600		//������
#define BYTE_DANCE_TIMEOUT 		50 			//�ֽڼ䳬ʱ ms		115200 ��� Ĭ��50ms
#define MAX_COM_CHANNEL_NUM		2
#define UART_RECVWAVE_COUNT		3			//uart���ղ�������֡����
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
int UART_HD_LTO = 0;
int UART_HD_RECVWAVE = 0;
int UART_HD_BASEMETER = 0;

int IntervalTime = 15; //�ɼ����ʱ��
TSerial Serial[MAX_COM_CHANNEL_NUM];
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
nwy_time_t poweronTimer;
BYTE UartRecTimeout = 0;			  //���ճ�ʱ��־
static WORD SysTickCounter;
BYTE gTranType = 0;
TRealTimer tTimer = {0}; //��ֹ ��58�봦������־����һ�ν��к������ ���ԣ���time��Ϊȫ�ֱ��� ��ʼ������ʱ����Ϊ�ϱ�ʱ�䣿����

tTranData Transdata;
BYTE PowerOnReadMeter = 0; //�ϵ糭��ɹ���־
uint64_t dwCurTicks = 0, dwCurTicksHis = 0;
static 	BYTE bHour = 0;
static 	BYTE bMin = 0;
//��չ��Լ���ݱ�ʶ
const WORD  EventOI[]  = { 0x1700 };
const WORD  PtandCt[]  = { 0x1118,0x1119,0x111A,0x111B};//CT1,CT2,pt1,pt2
const WORD  Position[] = { 0x1300};
const WORD  WaveFactor[] = { 0x1411,0x1408};
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern int nwy_set_rx_frame_timeout(uint8_t hd, int time);
extern int nwy_data_get_flowcalc_info(nwy_data_flowcalc_info_t *flowcalc_info);
//��ʼ����Լ��дָ��,��ʼ�������ճ�ʱ����Լ����������������ã�
void InitPoint(TSerial *p)
{
	p->ProStepDlt645 = 0;
	p->BeginPosDlt645 = 0;
	p->Dlt645DataLen = 0;
	p->RXWPoint = 0;
	p->RXRPoint = 0;
	p->RxOverCount = 0;
	// 698��ʼ��
	p->ProStepDlt698_45 = 0;
	// DLT698_45��Լ������Serial[].ProBuf�еĿ�ʼλ��
	p->BeginPosDlt698_45 = 0;
	p->wLen = 0;
	p->Addr_Len = 6; // ͨ�ű����з�������ַSA���ȣ���ַ����Ĭ��6�ֽ�
}

void nwy_meter_upgrade_timer_cb(void *type)
{
	MeterUpgradeTimeUpFlag = 1;
}
void nwy_general_timer_cb(void *type)
{
	OSI_LOGI(0, "nwy_app_timer_cb");
	SysTickCounter++;
	if (SysTickCounter >= 3600)
	{
		SysTickCounter = 0;
	}
	if ((SysTickCounter % 3600) == 0)
	{
		api_SetAllTaskFlagRam(eFLAG_HOUR); //Сʱ����
	}
	if ((SysTickCounter % 60) == 0)
	{
		api_SetAllTaskFlagRam(eFLAG_MINUTE); //��������
	}
	if ((SysTickCounter % 30) == 0)
	{
		api_SetAllTaskFlagRam(eFLAG_HALF_MINUTE); //���������
	}
	api_SetAllTaskFlagRam(eFLAG_SECOND); //������
	nwy_gpio_set_value(RUN_LIGHT, (SysTickCounter % 2));
	if (nwy_get_queue_spaceevent_cnt(TimerMessageQueue) != 0)
	{
		nwy_put_msg_que(TimerMessageQueue, &FlagBytesRam, 0xffffffff);
		api_ClrAllTaskFlagRam();
	}
	dwCurTicksHis = dwCurTicks;
}
BYTE api_CalRXD_CheckSum(WORD ProtocolType, TSerial *p)
{
	WORD i;
	BYTE bySum = 0;

	WORD ProStep, BeginPos;
	//����645��Լ
	if (ProtocolType == 0)
	{
		ProStep = p->ProStepDlt645;
		BeginPos = p->BeginPosDlt645;
	}
	else
	{
		return 0;
	}
	if (ProStep <= 0)
	{
		return 0;
	}

	for (i = 0; i < (ProStep - 1); i++)
	{
		bySum += p->ProBuf[(BeginPos + i) % UART_BUFF_MAX_LEN];
	}
	return bySum;
}

//--------------------------------------------------
//��������:  ��������
//		 
//����:	  bBtep �����־bitλ
//		 
//����ֵ:	
//		 
//��ע:  
//--------------------------------------------------
void  api_ReadMeterTask( BYTE bStep,BYTE* bWaveAddr )
{
	BYTE bTempStep;

	bTempStep = (bStep-CycleReadMeterNum);
	if (bTempStep & 0x80)
	{
		bTempStep = 0xff;
	}
	nwy_ext_echo("\r\nreadMeter step is%d",bStep);
	// nwy_ext_echo("\r\nreadMeter bTempstep is%d",bTempStep);
	// nwy_gpio_set_value(LOCAL_LIGHT,0);
	switch (bTempStep)
	{
		case eBIT_ADDR_WAVE:
			Adrr_Txd_645();
			break;
		case eBIT_TIME:
			Dlt645_Tx_Read(DAY_TIME);
			break;
		case eBIT_TCPNET:
			Dlt645_Tx_Read(TCP_REQ_NET);
			break;
		case eBIT_MQTTNET:
			Dlt645_Tx_Read(MQTT_REQ_NET);
			break;
		case eBIT_TCPUSER:
			Dlt645_Tx_Read(TCP_REQ_USER_PD);
			break;
		case eBIT_MQTTUSER:
			Dlt645_Tx_Read(MQTT_REQ_USER_PD);
			break;
		case eBIT_VERSION:
			Dlt645_Tx_Read_Version();		
			break;
		#if(PT_CT == YES)
		case eBIT__CT1:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[1],0);
			break;
		case eBIT__CT2:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[2],0);
			break;
		case eBIT_PT1:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[3],0);
			break;
		case eBIT_PT2:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[4],0);
			break;
		#endif
		case eBIT_POSITION:
		#if (GPRS_POSITION == YES)
			api_CheckorSetPosition(1);
		#endif
			break;
		#if(CYCLE_REPORT == YES)
		case 0xFF:
			Dlt645_Tx_ContinueRead(bStep,bWaveAddr);
			break;
		#endif
		case eBIT_LTO_ADDR:
			Adrr_Txd_698();
			break;
		case eBIT_LTO_SAFE_FREEZE:
			Compose_SecurityRequsetRecordFreezeMin_698();
			break;
		case eBIT_LTO_MODULEVERSIONtoMQTT:
			ComposeReadModuleVersion();
			break;
		case eBIT_WAVE_FACTOR:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&WaveFactor[0],0);
			break;
		case eBIT_SET_WAVE_TIME:
			CycleSetF4A0_time();
			break;
		case eBIT_OPEN_WAVE_SYS:
			OpenSystemProgram();
			break;
		case eBIT_LTO_MODULEVERSIONtoTCP:
			ComposeReadModuleVersion_security();
			break;
		case eBIT_GET_METER_UIP:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&WaveFactor[1],0);
			break;
		case eBIT_LTO_OPEN_FACTORY:
			open_ltofactory();
			break;
		case eBIT_LTO_SET_TIME:
			SetLtoTime();
			break;
		default:
			break;
	}

	nwy_stop_timer(uart_timer);
	nwy_start_timer(uart_timer,2000);//2s��ʱ
	api_SetSysStatus(eSYS_STASUS_START_COLLECT);
	
}

//--------------------------------------------------
//��������:  ��������bitλ
//
//����:	  bLoop  ��ǰ��·��
//
//����ֵ:
//
//��ע:  ������־λ���б�־��ռ�ô��ڽ��з���
//--------------------------------------------------
BOOL TraverseMeterReadingSigns(BYTE bLoop, BYTE *bAddr)
{
	BYTE i;

	// nwy_ext_echo("\r\nsqReadMeterFlag IS %lx",&dwReadMeterFlag[0]);
	for (i = 0; i < MAX_READMETER; i++)
	{
		if (dwReadMeterFlag[bLoop] & (0x0001 << i))
		{
			// nwy_ext_echo("\r\nFlag bit is%d",i);
			// nwy_ext_echo("\r\nbHisReadMeter[bLoop] is%d",bHisReadMeter[bLoop]);
			if ((bHisReadMeter[bLoop] == i))
			{
				if (bReadMeterRetry >= 3)
				{
					dwReadMeterFlag[bLoop] &= ~(1 << i);
					bReadMeterRetry = 0;
					//��������ʧ�� ����Ƿ���˲�����sys�ظ�
					if ((i >= (CycleReadMeterNum + eBIT_TCPNET)) && (i <= (CycleReadMeterNum + eBIT_MQTTUSER)))
					{
						api_ReadSystemFiletoRam(i - CycleReadMeterNum);
						//���ļ�ϵͳ�ָ�������������쳣��������ʾ��
					}
					return FALSE;
				}
				bReadMeterRetry++;
			}
			else
			{
				bHisReadMeter[bLoop] = i;
			}
			api_ReadMeterTask(i, bAddr);
			return FALSE;
		}
	}
	return TRUE;
}
//--------------------------------------------------
//��������: ���·�ɼ�����
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void CollectMeterTask(void)
{
	BYTE i;
	BYTE bTempAddr[6] = {0};

	if (api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE)
	{
		memcpy((BYTE *)&bTempAddr[0], (BYTE *)&MeterWaveAddr[0], 6);
		for (i = 0; i < MAX_SAMPLE_CHIP; i++)
		{
			bTempAddr[5] += i;
			if (TraverseMeterReadingSigns(i, bTempAddr) != TRUE)
			{
				break;
			}
		}
	}
}
#if (CYCLE_REPORT == YES)
//--------------------------------------------------
//��������:  ��������mqtt���񽻻���Ŀǰ��60���ӽ���һ��
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void ToMqttByCycle(void)
{
	BYTE bTime, i, j;

	get_N176_time(&tTimer);
	bTime = tTimer.Min % IntervalTime; //�˺���Ŀǰֻ֧�� 2����  �������汾δ�ϲ�������
	if (bTime == 0)
	{
		if (api_GetSysStatus(eSYS_STASUS_SAMPLE_UPLOAD) == TRUE)
		{
			for (i = 0; i < MAX_SAMPLE_CHIP; i++)
			{
				for (j = 0; j < CycleReadMeterNum; j++)
				{
					dwReadMeterFlag[i] |= (1 << j);
				}
			}
			api_ClrSysStatus(eSYS_STASUS_SAMPLE_UPLOAD);
			nwy_ext_echo("\r\ntime up");
		}
	}
	else
	{
		api_SetSysStatus(eSYS_STASUS_SAMPLE_UPLOAD);
	}
}
#endif
//--------------------------------------------------
//��������:  ������������15���Ӷ�������
//		 
//����:	  
//		 
//����ֵ:	
//		 
//��ע:  
//--------------------------------------------------
void  GetLtoMinFreeze( void )
{
	BYTE bTime;

	get_N176_time(&tTimer);
	bTime = (tTimer.Min - 1) % 15;//С��0 �����ԣ�����
	if (bTime == 0)
	{
		if (api_GetSysStatus(eSYS_STASUS_SAMPLE_UPLOAD) == TRUE)
		{
			dwReadMeterFlag[0] |= (1<<(CycleReadMeterNum + eBIT_LTO_SAFE_FREEZE));
			dwReadMeterFlag[0] |= (1<<(CycleReadMeterNum + eBIT_LTO_MODULEVERSIONtoTCP));
			api_ClrSysStatus(eSYS_STASUS_SAMPLE_UPLOAD);
			nwy_ext_echo("\r\ntime up");
		}
	}
	else
	{
		api_SetSysStatus(eSYS_STASUS_SAMPLE_UPLOAD);
	}
}
//--------------------------------------------------
//��������:
//
//����:
//
//����ֵ:
//
//��ע:  һ��������4096�ֽڣ��������ݳ��ȱ仯��Ҫ����
//--------------------------------------------------
void nwy_uart_recvwave_handle(const char *str, uint32_t length)
{
	BYTE num;
	DWORD i = 0;
	if(length > 4096)
	{
		nwy_ext_echo("\r\nexceed 4096,is %d",length);
		RecLenExceed4096Flag = 1;
		return;
	}

	num = length/WAVE_DATA_FRAME_SIZE;
	
	if ((length%WAVE_DATA_FRAME_SIZE) != 0)
	{
		RecLenNot815Flag = 1;
	}
	if (length != WAVE_DATA_FRAME_SIZE)
	{
		nwy_ext_echo("\r\nrec len  not 815, %d", length);
		nwy_time_t julian_time = {0};
		char timezone = 0;
		nwy_get_time(&julian_time, &timezone);
		nwy_ext_echo("\r\n%d-%d %d:%d:%d", julian_time.mon, julian_time.day, 
			julian_time.hour, julian_time.min, julian_time.sec);
		int64_t ms = nwy_get_ms();
		nwy_ext_echo("\r\nms: %lld", ms);
		if (length > WAVE_DATA_FRAME_SIZE && length <= 2 * WAVE_DATA_FRAME_SIZE)
		{
			nwy_ext_echo("\r\n10 bytes after 815:");
			for (i = WAVE_DATA_FRAME_SIZE; i < WAVE_DATA_FRAME_SIZE + 10 && i < length; i++)
			{
				nwy_ext_echo("%02x ", (unsigned char)str[i]);
			}
			nwy_ext_echo("\r\n last 10 bytes:");
			for (i = (length >= 10 ? length - 10 : 0); i < length; i++)
			{
				nwy_ext_echo("%02x ", (unsigned char)str[i]);
			}
		}
	}
	for (i = 0; i < num; i++)
	{
		if(nwy_put_msg_que(WaveDataToTFMessageQueue, &str[i*WAVE_DATA_FRAME_SIZE], 0xffffffff) != TRUE)
		{
			WaveQueueFullFlag = 1;
		}
	}	
}

#if (LTO_METER_UART == YES)
//�˴��� �� ������������
void nwy_uart_lto_recv_handle(const char *str, uint32_t length)
{
	WORD wLen, wLen1;

	nwy_ext_echo("\r\nenter ltouart,len:%d\r\n",length);
	// for (i = 0; i < length; i++)
	// {
	// 	nwy_ext_echo("%02x ", str[i]);
	// }
	if (api_GetSysStatus(eSYS_STASUS_START_TRAN))
	{
		memset(&Transdata, 0, sizeof(Transdata));
		if (length > sizeof(Transdata.buf))
		{
			memcpy(Transdata.buf, str, sizeof(Transdata.buf));
			Transdata.len = sizeof(Transdata.buf);
		}
		else
		{
			memcpy(Transdata.buf, str, length);
			Transdata.len = length;
		}
		if (gTranType == BLUETOOTH_TRAN)
		{
			if (nwy_put_msg_que(TranDataToFactoryMessageQueue, &Transdata, 0xffffffff) == TRUE)
			{
				nwy_ext_echo("\r\nuart to factory ok,length");
			}
			else
			{
				nwy_ext_echo("\r\nuart to factory err");
			}
		}
		else if (gTranType == SERVER_TRAN)
		{
			if (nwy_put_msg_que(TranDataToTcpPrivateServerMsgQue, &Transdata, 0xffffffff))
			{
				nwy_ext_echo("\r\nuart to mqtt/tcp queue ok,length:%d", length);
			}
			else
			{
				nwy_ext_echo("\r\nto mqtt queue err");
			}
		}
		gTranType = 0;
		nwy_stop_timer(uart_timer);
		api_ClrSysStatus(eSYS_STASUS_START_TRAN); // ���� Զ��
	}
	else
	{
		wLen = Serial[0].RXWPoint + length;
		if (length >= UART_BUFF_MAX_LEN)
		{
			length = UART_BUFF_MAX_LEN;
		}
		if (wLen < UART_BUFF_MAX_LEN)
		{
			memcpy((char *)&Serial[0].ProBuf[Serial[0].RXWPoint], str, length);
			Serial[0].RXWPoint += length;
		}
		else
		{
			wLen1 = UART_BUFF_MAX_LEN - Serial[0].RXWPoint;
			memcpy((char *)&Serial[0].ProBuf[Serial[0].RXWPoint], str, wLen1);
			memcpy((char *)&Serial[0].ProBuf[0], str + wLen1, length - wLen1);
			Serial[0].RXWPoint = length - wLen1;
		}
	}
	
}
#endif
//�˴��� �� ��¼��������
void nwy_uart_base_meter(const char *str, uint32_t length)
{
	WORD wLen, wLen1;

	// WORD i;
	nwy_ext_echo("\r\nenter whuart,len:%d\r\n",length);
	// for (i = 0; i < length; i++)
	// {
	// nwy_ext_echo("%02x ", str[i]);
	// }
	if (api_GetSysStatus(eSYS_STASUS_START_TRAN))
	{
		memset(&Transdata, 0, sizeof(Transdata));
		if (length > sizeof(Transdata.buf))
		{
			memcpy(Transdata.buf, str, sizeof(Transdata.buf));
			Transdata.len = sizeof(Transdata.buf);
		}
		else
		{
			memcpy(Transdata.buf, str, length);
			Transdata.len = length;
		}
		if (gTranType == BLUETOOTH_TRAN)
		{
			if (nwy_put_msg_que(TranDataToFactoryMessageQueue, &Transdata, 0xffffffff) == TRUE)
			{
				nwy_ext_echo("\r\nuart to factory ok,length");
			}
			else
			{
				nwy_ext_echo("\r\nuart to factory err");
			}
		}
		else if (gTranType == SERVER_TRAN)
		{
			if (nwy_put_msg_que(TranDataToTcpPrivateServerMsgQue, &Transdata, 0xffffffff))
			{
				nwy_ext_echo("\r\nuart to mqtt/tcp queue ok,length:%d", length);
			}
			else
			{
				nwy_ext_echo("\r\nto mqtt queue err");
			}
		}
		gTranType = 0;
		nwy_stop_timer(uart_timer);
		api_ClrSysStatus(eSYS_STASUS_START_TRAN); // ���� Զ��
	}
	else
	{
		wLen = Serial[1].RXWPoint + length;
		if (length >= UART_BUFF_MAX_LEN)
		{
			length = UART_BUFF_MAX_LEN;
		}
		if (wLen < UART_BUFF_MAX_LEN)
		{
			memcpy((char *)&Serial[1].ProBuf[Serial[1].RXWPoint], str, length);
			Serial[1].RXWPoint += length;
		}
		else
		{
			wLen1 = UART_BUFF_MAX_LEN - Serial[1].RXWPoint;
			memcpy((char *)&Serial[1].ProBuf[Serial[1].RXWPoint], str, wLen1);
			memcpy((char *)&Serial[1].ProBuf[0], str + wLen1, length - wLen1);
			Serial[1].RXWPoint = length - wLen1;
		}
	}
}

static void nwy_uart_send_complet_handle(int param)
{
	ENABLE_HARD_SCI_RECV;
	nwy_ext_echo("\r\n nwy_uart_send_complet_handle success!");
}
//--------------------------------------------------
// ��������:  ����͸������
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void SendTranData(BYTE *buf, int len, BYTE UartNum)
{
	int RetLen = 0;

	if (UartNum == eUART_WHWAVE)
	{
		RetLen = nwy_uart_send_data(UART_HD_BASEMETER, buf, len);//�˴����� id���ܲ��� ����ʵ�ʰ��Ӷ�Ӧ
	}
	else
	{
		ENABLE_HARD_SCI_SEND;
		RetLen = nwy_uart_send_data(UART_HD_LTO, buf, len);
	}
	nwy_ext_echo("uart send len is \r\n %d", RetLen);
	nwy_stop_timer(uart_timer);
	nwy_start_timer(uart_timer, 5000); //5s��ʱ
}
//--------------------------------------------------
//��������:  ���ڳ�ʼ��
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Init_UartPara(void)
{
	BYTE i,j;

	// ���ղ������ݴ��� ֻ��������
	int hd = nwy_uart_init(NWY_NAME_UART2, 1); // ��ʼ��
	nwy_ext_echo("\r\ninit uart1 hd:%d", hd);
	nwy_uart_set_baud(hd, BAUDRATE); // ����Ĭ�ϲ�����
	nwy_uart_set_para(hd, NWY_UART_EVEN_PARITY, NWY_UART_DATA_BITS_8, NWY_UART_STOP_BITS_1, 0);
	nwy_set_rx_frame_timeout(hd, 8);
	UART_HD_RECVWAVE = hd;
	nwy_uart_reg_recv_cb(hd, nwy_uart_recvwave_handle);
	// ������������� ���� 
	#if (LTO_METER_UART == YES)
	int hd_lto = nwy_uart_init(NWY_NAME_UART3, 1); // ��ʼ��
	nwy_uart_set_baud(hd_lto, 115200); // ����Ĭ�ϲ�����
	nwy_uart_set_para(hd_lto, NWY_UART_EVEN_PARITY, NWY_UART_DATA_BITS_8, NWY_UART_STOP_BITS_1, 0);
	nwy_set_rx_frame_timeout(hd_lto, BYTE_DANCE_TIMEOUT);
	UART_HD_LTO = hd_lto;
	nwy_uart_reg_recv_cb(hd_lto, nwy_uart_lto_recv_handle);
	nwy_uart_reg_tx_cb(hd_lto, nwy_uart_send_complet_handle );
	#endif
	//ά��¼����װ ��������ͨ��
	int hd_wh = nwy_uart_init(NWY_NAME_UART1, 1); // 1,3��������
	nwy_uart_set_baud(hd_wh, 115200); // ����Ĭ�ϲ�����
	nwy_uart_set_para(hd_wh, NWY_UART_EVEN_PARITY, NWY_UART_DATA_BITS_8, NWY_UART_STOP_BITS_1, 0);
	nwy_set_rx_frame_timeout(hd_wh, BYTE_DANCE_TIMEOUT);
	UART_HD_BASEMETER = hd_wh;
	nwy_uart_reg_recv_cb(hd_wh, nwy_uart_base_meter);

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		InitPoint(&Serial[i]);
		memset((char *)&Serial[i].ProBuf[0], 0, UART_BUFF_MAX_LEN);
	}
	memset(bHisReadMeter, 0xFF, sizeof(bHisReadMeter));
	for (j = CycleReadMeterNum; j < (CycleReadMeterNum + POWERON_READRPARA_NUM); j++) //���ϵ�Ҫ������bit
	{
		dwReadMeterFlag[0] |= (1 << j);
	}
	if ((eBIT_TOTAL + CycleReadMeterNum) > (sizeof(DWORD) * 8)) //��־���ܳ���DWORD*8
	{
		for (;;)
		{
			nwy_sleep(1000);
			nwy_ext_echo("error ,Bit overrun!!!");
		}
	}
}
//--------------------------------------------------
//��������:  �����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ����
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void api_DoReceMoveData(TSerial *p, BYTE ProtocolType)
{
	WORD ProStep, BeginPos;
	WORD i;
	BYTE Buf[UART_BUFF_MAX_LEN + 30];

	if (ProtocolType == PROTOCOL_645)
	{
		ProStep = p->ProStepDlt645;
		BeginPos = p->BeginPosDlt645;
	}
	else if (ProtocolType == PROTOCOL_698)
	{
		ProStep = p->ProStepDlt698_45;
		BeginPos = p->BeginPosDlt698_45;
	}
	else
	{
		return;
	}

	if (BeginPos == 0)
	{
		return;
	}
	if (ProStep >= UART_BUFF_MAX_LEN)
	{
		return;
	}
	if (BeginPos <= (UART_BUFF_MAX_LEN - ProStep))
	{
		for (i = 0; i < ProStep; i++)
		{
			p->ProBuf[i] = p->ProBuf[BeginPos + i];
		}
	}
	else
	{
		for (i = 0; i < ProStep; i++)
		{
			Buf[i] = p->ProBuf[(BeginPos + i) % UART_BUFF_MAX_LEN];
		}
		memcpy((void *)p->ProBuf, (void *)Buf, ProStep);
	}
}
//--------------------------------------------------
//��������:  ���ݴ�������
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void api_ReceData_UartTask(void)
{
	BYTE i,Result;
	WORD ReceNum;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		Result = FALSE;
		ReceNum = Serial[i].RXWPoint;
		while (ReceNum!= Serial[i].RXRPoint)
		{
			nwy_start_timer(Uart_Recv_timer, 500);
			if ((OpenFactoryStep != eWaitResult) && (ReadModuleVersion == 0)) //�򿪳��ڵȴ��������ģ��汾ʱ���ܰ�645��������Ϊ�ظ���698����������645
			{
				Result = DoReceProc_Dlt645_UART(Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i]);
			}
			if (Result == FALSE)
			{
				Result = DoReceProc_Dlt698_UART(Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i]);
			}
			#if (READMETER_PROTOCOL == PROTOCOL_MODBUS)
			Result = DoReceProc_Modbus_UART(Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i]);
			#endif
			if (Result == FALSE)
			{
				Serial[i].RXRPoint++;
				if (Serial[i].RXRPoint >= UART_BUFF_MAX_LEN)
				{
					Serial[i].RXRPoint = 0;
				}
			}
			else //���ܵ���������
			{
				nwy_stop_timer(Uart_Recv_timer);
				break;
			}
		}

		if (UartRecTimeout == 1)
		{
			UartRecTimeout = 0;
			InitPoint(&Serial[i]);
		}
	}
}
//--------------------------------------------------
//��������:  uart�����ֽڼ䳬ʱ��ʱ��
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Uart_Recv_timer_cb(void *type)
{
	UartRecTimeout = 1;
}
//--------------------------------------------------
//��������:  �ϵ����Ⳮ��״̬
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void DetectReadMeterStasus(void)
{
	BYTE j;

	for (j = CycleReadMeterNum; j < (CycleReadMeterNum + POWERON_READRPARA_NUM); j++) //���ϵ�Ҫ������bit
	{
		if ((dwReadMeterFlag[0] & (1 << j)) == (1 << j))
		{
			return;
		}
	}
	PowerOnReadMeter = 1;
}

// ����ͳ�ƹ���
void CalculateFlow(void)
{
	static WORD i = 0;
	i++;
	char rsp[128] = {0};
	QWORD total_bytes = 0;
	nwy_data_flowcalc_info_t flowcalc_info = {0};
	nwy_time_t tTimer;
	// ����ʱ���(��)
	int total_seconds = 0;
	float flow_per_second = 0;
	float flow_per_day = 0;
	BYTE timezone = 8;
	if (i >= 2000)
	{
		i = 0;
		nwy_data_get_flowcalc_info(&flowcalc_info);
		total_bytes = flowcalc_info.tx_bytes + flowcalc_info.rx_bytes;
		snprintf(rsp, sizeof(rsp),
				 "tx_bytes:%llu,rx_bytes:%llu,total_bytes:%llu\r\n"
				 "tx_packets:%lu,rx_packets:%lu",
				 flowcalc_info.tx_bytes, flowcalc_info.rx_bytes, total_bytes,
				 flowcalc_info.tx_packets, flowcalc_info.rx_packets);
		//  nwy_ext_echo("\r\n%s", rsp);
		nwy_get_time(&tTimer, (char*)&timezone);
		// ת��Ϊ������
		total_seconds = (tTimer.hour - poweronTimer.hour) * 3600 +
						(tTimer.min - poweronTimer.min) * 60 +
						(tTimer.sec - poweronTimer.sec);
		// nwy_ext_echo("\r\ntotal_bytes:%llu\r\n", total_bytes);
		if (total_seconds > 0)
		{
			// ����ÿ������(�ֽ�/��)
			flow_per_second = (float)total_bytes / total_seconds;
			// ����ÿ������(MB/��)
			flow_per_day = flow_per_second * 86400 / (1024 * 1024);
			nwy_ext_echo("\r\nflow per second: %.2f bytes/s", flow_per_second);
			nwy_ext_echo("\r\nflow per 30 days: %.2f MB", flow_per_day * 30);
		}
	}
}
//--------------------------------------------------
//��������:  ���ڽ�������Уʱ����4A0��ʱ
//		 
//����:	  
//		 
//����ֵ:	
//		 
//��ע:  
//--------------------------------------------------
void  AdjustTime( void )
{
	bHour++;

	if (0 != nwy_ext_check_data_connect())
	{
		api_UpdateForInte();
		if ((bHour %6 )== 0)
		{
			dwReadMeterFlag[0] |= (1<<(CycleReadMeterNum + eBIT_OPEN_WAVE_SYS));
			dwReadMeterFlag[0] |= (1<<(CycleReadMeterNum + eBIT_SET_WAVE_TIME));
			
			if (bHour == 24)
			{
				bHour = 0;
				dwReadMeterFlag[0] |= (1<<(CycleReadMeterNum + eBIT_LTO_OPEN_FACTORY));
				dwReadMeterFlag[0] |= (1<<(CycleReadMeterNum + eBIT_LTO_SET_TIME));
			}
		}
	}
}
//��ע:  ��Ϊÿ��uart��������ֹʹ�þ�̬����
//--------------------------------------------------
void Uart_Task(void *parameter)
{
	//��·���ڳ�ʼ��
	nwy_osiEvent_t event;
	TFlagBytes FlagBytes;
	tTranData TranDataToMeter;
	#if (FLOW_CALCULATE == YES)
	BYTE timezone = 8;
	nwy_get_time(&poweronTimer, (char*)&timezone);
	#endif
	Init_UartPara();
	memset(&TranDataToMeter, 0, sizeof(TranDataToMeter));
	memset((BYTE *)&FlagBytesRam, 0, sizeof(TFlagBytes));
	memset((BYTE *)&FlagBytes, 0, sizeof(TFlagBytes));
	dwCurTicks = nwy_get_ms();
	dwCurTicksHis = dwCurTicks;
	nwy_start_timer_periodic(general_timer, 1000); //�����Զ�ʱ һ��һ��
	nwy_stop_timer(uart_timer);

	while (1)
	{
		ThreadRunCnt[eUartThread]++;
		dwCurTicks = nwy_get_ms();
		if ((dwCurTicks - dwCurTicksHis) > 10000) //�糬��10sδ����ʱ�������´򿪶�ʱ��
		{
			dwCurTicksHis = dwCurTicks;
			nwy_stop_timer(general_timer); //�رն�ʱ�������´�
			nwy_sleep(50);
			nwy_start_timer_periodic(general_timer, 1000); //�����Զ�ʱ һ��һ��
		}

		if (PowerOnReadMeter == 0)
		{
			DetectReadMeterStasus();
		}
		else
		{
			#if (CYCLE_REPORT == YES)
			ToMqttByCycle();
			#endif
		}
		GetLtoMinFreeze();
		api_ReceData_UartTask();
		nwy_get_msg_que(TimerMessageQueue, &FlagBytes, 0xffffffff);

		if (tIap_645.dwIapFlag == BASEMETER_UPGRADES)
		{
			// MeterUpgrade645_Task();
		}
		else if ((tIap_698.dwIapFlag == BASEMETER_UPGRADES) && (tIap_698.IsModuleUpgrade == 0))
		{
			MeterUpgrade698_Task();
		}
		else if ((tIap_698.dwIapFlag == BASEMETER_UPGRADES) && (tIap_698.IsModuleUpgrade == 1))
		{
			if (OpenFactoryStep == eIDLE)
			{
				SendOpenFactoryFrame(); //�򿪹���ģʽ�ظ������޷�����
				OpenFactoryStep = eWaitResult;
			}
			else if (OpenFactoryStep == eSuc)
			{
				MeterUpgrade698_Task();
			}
		}
		else
		{
			if (api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE) //�жϵ�ǰ�����ϴ�ģʽ�򲻻����͸��
			{
				//��Ϣ���п���ͬʱ���˶��������Լ����жϽ��պ���һ֡͸�����ݣ��ж�˳���ܱ�
				if (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE)
				{
					if (nwy_get_msg_que(TranDataToUartMessageQueue, &TranDataToMeter, 0xffffffff))
					{
						api_SetSysStatus(eSYS_STASUS_START_TRAN);
						gTranType = TranDataToMeter.TranType;
						SendTranData(TranDataToMeter.buf, TranDataToMeter.len,TranDataToMeter.Uart);
						nwy_ext_echo("\r\ntran data send ok");
					}
				}
			}
			if (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE) //͸������,��������г���
			{

				if(api_GetTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_HOUR) == TRUE)
				{
					AdjustTime();
					api_ClrTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_HOUR);
				}
				if (api_GetTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_MINUTE) == TRUE)
				{
					bMin++;
					if (bMin >= 10)
					{
						bMin = 0;
						dwReadMeterFlag[0] |= (1<<(CycleReadMeterNum + eBIT_GET_METER_UIP));
					}
					api_ClrTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_MINUTE);
				}
				#if (COLLECT_UIP == YES)
				CollectInstantDataTask(&FlagBytes);
				#endif		
				CollectMeterTask();
			}
		}
		#if (FLOW_CALCULATE == YES)
		CalculateFlow();
		#endif	
		nwy_wait_thead_event(uart_thread, &event, 5);
	}
}

// ����͸�� ������ڳ���ʱ��ͻȻ��͸��������͸��ʧ��