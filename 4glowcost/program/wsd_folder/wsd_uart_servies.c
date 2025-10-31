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
#define BAUDRATE_115200 		115200		
#define BYTE_DANCE_TIMEOUT1 	50 			//�ֽڼ䳬ʱ ms 115200 ���50ms 

#define BAUDRATE_9600 			9600		
#define BYTE_DANCE_TIMEOUT2 	200 		//�ֽڼ䳬ʱms 9600 ���200ms
#define TRANSPARENT_TIMEOUT		2000		//͸����ʱʱ��ms   	2s
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
int UART_HD = 0;
int IntervalTime = 60; //�ɼ����ʱ��
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
BYTE UartRecTimeout = 0; //���ճ�ʱ��־
static WORD SysTickCounter;
BYTE gTranType = 0;
TRealTimer tTimer = {0}; //��ֹ ��58�봦������־����һ�ν��к������ ���ԣ���time��Ϊȫ�ֱ��� ��ʼ������ʱ����Ϊ�ϱ�ʱ�䣿����

tTranData Transdata;
BYTE *TransdataPtr = NULL;
BYTE PowerOnReadMeter = 0; //�ϵ糭��ɹ���־
uint64_t dwCurTicks = 0, dwCurTicksHis = 0;

static nwy_uartdcb_t dcb = {
	.baudrate = BAUDRATE_9600,
	.databits = 8,
	.stopbits = 1,
	.parity = PB_EVEN,
	.flowctrl = FC_NONE};
//��չ��Լ���ݱ�ʶ
const WORD EventOI[] = {0x1700};
const WORD PtandCt[] = {0x1118, 0x1119, 0x111A, 0x111B}; //CT1,CT2,pt1,pt2
const WORD Position[] = {0x1300};
const WORD Ep212Tcp[] = {0xFFFE};
//-----------------------------------------------
//				��������
//-----------------------------------------------
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
	#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_212)
	p->BeginPos212 = 0;
	p->Hb212DataLen = 0;
	p->ProStep212 = 0;
	#endif
}

void nwy_meter_upgrade_timer_cb(void *type)
{
	MeterUpgradeTimeUpFlag = 1;
}
void nwy_general_timer_cb(void *type)
{
	// OSI_LOGI(0, "nwy_app_timer_cb");
	// nwy_ext_echo("\r\n nwy_general_timer_cb");
	uint32 FreeQueue;
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
	// nwy_gpio_value_set(TX_LIGHT, 0);//������
	// nwy_gpio_value_set(RX_LIGHT, 0);
	nwy_gpio_value_set(RUN_LIGHT, (SysTickCounter % 2));
	nwy_msg_queue_space_num_get(TimerMessageQueue,&FreeQueue);
	if (FreeQueue !=0)
	{
		nwy_msg_queue_send(TimerMessageQueue, sizeof(TFlagBytes),&FlagBytesRam, NWY_OSA_NO_SUSPEND);
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
//����:      bBtep �����־bitλ
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void ReadMeterTask(BYTE bStep, BYTE *bAddr)
{
	BYTE bTempStep;

	bTempStep = (bStep - CycleReadMeterNum);
	if (bTempStep & 0x80)
	{
		bTempStep = 0xff;
	}
	nwy_ext_echo("\r\nreadMeter step is%d", bStep);
	nwy_ext_echo("\r\nreadMeter bTempstep is%d", bTempStep);
	nwy_gpio_value_set(LOCAL_LIGHT, 0);
	switch (bTempStep)
	{
	case eBIT_ADDR:
		Adrr_Txd();
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
	#if (PT_CT == YES)
	case eBIT__CT1:
		CommWithMeter_DBDF(0x11, 0, (BYTE *)&PtandCt[1], 0);
		break;
	case eBIT__CT2:
		CommWithMeter_DBDF(0x11, 0, (BYTE *)&PtandCt[2], 0);
		break;
	case eBIT_PT1:
		CommWithMeter_DBDF(0x11, 0, (BYTE *)&PtandCt[3], 0);
		break;
	case eBIT_PT2:
		CommWithMeter_DBDF(0x11, 0, (BYTE *)&PtandCt[4], 0);
		break;
	#endif
	#if (GPRS_POSITION == YES)
	case eBIT_POSITION:
		api_CheckorSetPosition(1);
		break;
	#endif
	case eBIT_USEDCHANNEL:
		Dlt645_Tx_Read(USED_CHANNEL_NUM);
		break;
	#if (CYCLE_METER_READING == YES)
	case 0xFF:
		Dlt645_Tx_ContinueRead(bStep,bAddr);
		break;
	#endif
	#if( EVENT_REPORT ==  YES)
	case eBIT_EVENT:
		Dlt645_Tx_Read(WARNING_REPORT);
		break;
	#endif
	case eBIT_OPEN_FACTORY:																																																																																																																																					
		OpenSystemProgram();
		break;
	case eBIT_TCP_USER_PARA:
		CommWithMeter_DBDF(0x11, 0, (BYTE *)&Ep212Tcp[0], 0);
		break;
	default:
		break;
	}

	nwy_sdk_timer_stop(uart_timer);

	nwy_timer_para_t TimeTImerPara = //����������ȫ���滻
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_uart_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};
	TimeTImerPara.expired_time = 1000;

	nwy_sdk_timer_start(uart_timer, &TimeTImerPara); //1s��ʱ
	api_SetSysStatus(eSYS_STASUS_START_COLLECT);
}

//--------------------------------------------------
//��������:  ��������bitλ
//
//����:      bLoop  ��ǰ��·��
//
//����ֵ:
//
//��ע:  ������־λ���б�־��ռ�ô��ڽ��з���
//--------------------------------------------------
BOOL TraverseMeterReadingSigns(BYTE bLoop, BYTE *bAddr)
{
	BYTE i;
	// nwy_ext_echo("\r\nsqReadMeterFlag IS %lx",&qwReadMeterFlag[0]);
	for (i = 0; i < MAX_READMETER; i++)
	{
		if (qwReadMeterFlag[bLoop] & (0x0001 << i))
		{
			nwy_ext_echo("\r\nFlag bit is%d", i);
			nwy_ext_echo("\r\nbHisReadMeter[bLoop] is%d", bHisReadMeter[bLoop]);
			if ((bHisReadMeter[bLoop] == i))
			{
				if (bReadMeterRetry >= 3)
				{
					qwReadMeterFlag[bLoop] &= ~(1 << i);
					bReadMeterRetry = 0;
					//��������ʧ�� ����Ƿ���˲�����sys�ظ�
					if ((i >= (CycleReadMeterNum + eBIT_TCPNET)) && (i <= (CycleReadMeterNum + eBIT_MQTTUSER)))
					{
						api_ReadSystemFiletoRam(i - CycleReadMeterNum);
						//���ļ�ϵͳ�ָ�������������쳣��������ʾ��
					}
					if (i == (CycleReadMeterNum+eBIT_ADDR))//������
					{
						dcb.baudrate = BAUDRATE_115200;
						nwy_uart_dcb_set(UART_HD, &dcb);
						nwy_uart_rx_frame_timeout_set(UART_HD, BYTE_DANCE_TIMEOUT1);
						qwReadMeterFlag[0] |= (1 << (CycleReadMeterNum + eBIT_ADDR));
					} //������Ҫ����һ�� ��ֹ��ַʼ���޷�̽���
					return FALSE;
				}
				bReadMeterRetry++;
			}
			else
			{
				bHisReadMeter[bLoop] = i;
			}
			ReadMeterTask(i, bAddr);
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
		memcpy((BYTE *)&bTempAddr[0], (BYTE *)&bAddr[0], 6);
		for (i = 0; i < bUsedChannelNum; i++)
		{
			bTempAddr[5] = i + bAddr[5];
			if (TraverseMeterReadingSigns(i, bTempAddr) != TRUE)
			{
				break;
			}
		}
	}
}
#if (CYCLE_METER_READING == YES)
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
	TRealTimer TempTimer = {0};

	get_N176_time(&TempTimer);
	if (IntervalTime <= 1)
	{
		bTime = TempTimer.Sec % (IntervalTime * 60);
	}
	else
	{
		bTime = TempTimer.Min % IntervalTime;
	}
	if (bTime == 0)
	{
		if (api_GetSysStatus(eSYS_STASUS_SAMPLE_UPLOAD) == TRUE)
		{
			for (i = 0; i < bUsedChannelNum; i++)
			{
				for (j = 0; j < CycleReadMeterNum; j++)
				{
					qwReadMeterFlag[i] |= (1 << j);
				}
			}
			tTimer = TempTimer;
			if (IntervalTime >= 1)//��֤�ϱ���������ʱ������0
			{
				tTimer.Sec = 0;
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
//��������:
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
static void nwy_uart_recv_handle(const char *str, uint32_t length)
{
	WORD wLen, wLen1;
	// WORD i;
	nwy_ext_echo("\r\nenter uart");
	nwy_ext_echo("\r\nlen:%d ", length);
	// for (i = 0; i < length; i++)
	// {
	// 	nwy_ext_echo("%02x ", str[i]);
	// }
	// if (1) //212Э�鴿͸��  //�ϵ糭��������� ȫ����ɺ� �� 212��͸����־
	// {
		//ֱ�Ӷ���tcp  tcp�жϸ�ʽ ��212�Ļ�  ȡ��ͨ��ID  ����
	// }
	 if (api_GetSysStatus(eSYS_STASUS_START_TRAN))
	{
		memset(&Transdata.buf, 0, sizeof(Transdata.buf));
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
		if (gTranType == eTRAN_TCP)
		{
			if (QueueWrite(&MessageQueueInfoArr[eQUEUE_UART_TO_TCP], &Transdata, sizeof(tTranData), NWY_OSA_NO_SUSPEND) == TRUE)
			{
				nwy_ext_echo("\r\nuart to tcp queue ok");
			}
			else
			{
				nwy_ext_echo("\r\nuart to tcp queue err");
			}
		}
		else if (gTranType == eTRAN_MQTT)
		{
			if (QueueWrite(&MessageQueueInfoArr[eQUEUE_TRAN_TO_MQTT_SERVER], &Transdata, sizeof(tTranData), NWY_OSA_NO_SUSPEND) == TRUE)
			{
				nwy_ext_echo("\r\nuart to mqtt/tcp queue ok");
			}
			else
			{
				nwy_ext_echo("\r\nto mqtt queue err");
			}
		}
		gTranType = 0;
		nwy_sdk_timer_stop(uart_timer);
		api_ClrSysStatus(eSYS_STASUS_START_TRAN); //���� Զ��
	}
	else
	{
		wLen = Serial.RXWPoint + length;
		if (length >= UART_BUFF_MAX_LEN)
		{
			length = UART_BUFF_MAX_LEN;
		}
		if (wLen < UART_BUFF_MAX_LEN)
		{
			memcpy((char *)&Serial.ProBuf[Serial.RXWPoint], str, length);
			Serial.RXWPoint += length;
		}
		else
		{
			wLen1 = UART_BUFF_MAX_LEN - Serial.RXWPoint;
			memcpy((char *)&Serial.ProBuf[Serial.RXWPoint], str, wLen1);
			memcpy((char *)&Serial.ProBuf[0], str + wLen1, length - wLen1);
			Serial.RXWPoint = length - wLen1;
		}
	}
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
void SendTranData(BYTE *buf, int len)
{
	int RetLen = 0;
	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_uart_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};

	RetLen = nwy_uart_write(UART_HD, buf, len);
	nwy_ext_echo("uart send len is  %d", RetLen);
	nwy_sdk_timer_stop(uart_timer);
	// nwy_gpio_value_set(TX_LIGHT, 1);
	OneTimeTImerPara.expired_time = 500;
	nwy_sdk_timer_start(uart_timer, &OneTimeTImerPara); ////212͸�� ͨ������
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
void Init_UartPara_DL645(void)
{
	BYTE j;
	int hd = -1;
 	char dev_name_list[4][8] = {0};

	nwy_dev_name_list_get(NWY_DEV_TYPE_UART,dev_name_list);
	// nwy_ext_echo("\r\nuart dev_name_list0:%s", dev_name_list[0]);//��ӡ��uart1����Ӧ������uart4,uart1��һֱ�������
	// nwy_ext_echo("\r\nuart dev_name_list1:%s", dev_name_list[1]);//��ӡ��uart2����Ӧ������uart1
	// nwy_ext_echo("\r\nuart dev_name_list2:%s", dev_name_list[2]);//��ӡ��uart3
	// nwy_ext_echo("\r\nuart dev_name_list3:%s", dev_name_list[3]);
	hd = nwy_uart_open(dev_name_list[1], BAUDRATE_9600, 0); 
	nwy_uart_dcb_set(hd, &dcb);
	nwy_uart_rx_frame_timeout_set(hd, BYTE_DANCE_TIMEOUT2);

	InitPoint(&Serial);
	memset((char *)&Serial.ProBuf[0], 0, UART_BUFF_MAX_LEN);
	memset(bHisReadMeter, 0xFF, sizeof(bHisReadMeter));
	UART_HD = hd;
	nwy_uart_rx_cb_register(hd, nwy_uart_recv_handle);
	for (j = CycleReadMeterNum; j < (CycleReadMeterNum + POWERON_READRPARA_NUM); j++) //���ϵ�Ҫ������bit
	{
		qwReadMeterFlag[0] |= (1 << j);
	}
	if ((eBIT_TOTAL + CycleReadMeterNum) > (sizeof(qwReadMeterFlag) * 8)) //��־���ܳ���DWORD*8
	{
		for (;;)
		{
			nwy_thread_sleep(1000);
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
	BYTE Result = FALSE;
	WORD ReceNum = 0;
	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = Uart_Recv_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};
	ReceNum = Serial.RXWPoint;
	while (ReceNum != Serial.RXRPoint)
	{
		OneTimeTImerPara.expired_time = 500;
		nwy_sdk_timer_start(Uart_Recv_timer, &OneTimeTImerPara);
		Result = DoReceProc_Dlt645_UART(Serial.ProBuf[Serial.RXRPoint], &Serial);
		if (Result == FALSE)
		{
			Result = DoReceProc_Dlt698_UART(Serial.ProBuf[Serial.RXRPoint], &Serial);
		}
		#if (READMETER_PROTOCOL == PROTOCOL_MODBUS)
		Result = DoReceProc_Modbus_UART(Serial.ProBuf[Serial.RXRPoint], &Serial);
		#endif
		#if (CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
		if (Result == FALSE)
		{
			Result = DoReceProc_EP212_UART(Serial.ProBuf[Serial.RXRPoint], &Serial);
		}
		#endif
		if (Result == FALSE)
		{
			Serial.RXRPoint++;
			if (Serial.RXRPoint >= UART_BUFF_MAX_LEN)
			{
				Serial.RXRPoint = 0;
			}
		}
		else //���ܵ���������
		{
			nwy_sdk_timer_stop(Uart_Recv_timer);
			break;
		}
	}

	if (UartRecTimeout == 1)
	{
		UartRecTimeout = 0;
		InitPoint(&Serial);
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
		if ((qwReadMeterFlag[0] & (1 << j)) == (1 << j))
		{
			return;
		}
	}
	PowerOnReadMeter = 1;
}
void  RecvMsgQueFromTcp( void )
{	
	BYTE temp;
	WORD ID = 0x2001;
	
	#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	if (nwy_msg_queue_recv(Ep212ToUartMsgQue, sizeof(char *), &TransdataPtr, NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
	{
		memcpy(&Transdata, TransdataPtr, sizeof(tTranData));
		//�ж� ���ܱ��ĵĸ�ʽ  flag ��־λ �Ƿ� ��ҪӦ��  �ٶ�Ӧ��uart��Դ����������������վ������
		//ȡ�̶�buf[84] (flag ��־) 
		nwy_ext_echo("recv from user[%s]",Transdata.buf);
		if (((Transdata.buf[84]-0x30)& 1) == 1)
		{
			api_SetSysStatus(eSYS_STASUS_START_COLLECT);
		}
		SendTranData(Transdata.buf, Transdata.len);
	}
	#else
	if (0){}
	#endif
	else if (nwy_msg_queue_recv(TranDataToUartMessageQueue, sizeof(char *), &TransdataPtr, NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
	{
		memcpy(&Transdata, TransdataPtr, sizeof(tTranData));
		api_SetSysStatus(eSYS_STASUS_START_TRAN);
		gTranType = Transdata.TranType;
		SendTranData(Transdata.buf, Transdata.len);
		nwy_ext_echo("\r\ntran data send ok");
	}
	else if (nwy_msg_queue_recv(UserTcpStatusChangeMsgQue, sizeof(BYTE), &temp, NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
	{
		//���ֱ�ӷ���
		CommWithMeter_DBDF(0x14,1,(BYTE*)&ID,&temp);
		//api_SetSysStatus(eSYS_STASUS_START_COLLECT);//������жϻظ�֡�Ļ� �Ͳ�����uart������
		nwy_sdk_timer_stop(uart_timer);
		// nwy_start_timer(uart_timer, 2000); //10s��ʱ
		nwy_ext_echo("\r\nrecv TCP STATUS Changed ");
	}
}
//--------------------------------------------------
//��������:  UART�߳�������
//
//����:
//
//����ֵ:
//
//��ע:  ��Ϊÿ��uart��������ֹʹ�þ�̬����
//--------------------------------------------------
void Uart_Task(void *parameter)
{
	//��·���ڳ�ʼ��
	nwy_event_msg_t event;
	TFlagBytes FlagBytes;
	nwy_heapinfo_t heapinfo;
	char buf[10];//������·����֪��ʲô����
	QWORD iTmp;
	#if (GPRS_POSITION == YES)
	BYTE bmin = 0;
	#endif
	nwy_timer_para_t para;

	memset(&para, 0, sizeof(para));
	para.thread_hdl = Timer_thread;
	para.cb = nwy_general_timer_cb;
	para.cb_para = NULL;
	para.expired_time = 1000;
	para.type = NWY_TIMER_PERIODIC;
	Init_UartPara_DL645();
	memset((BYTE *)&FlagBytesRam, 0, sizeof(TFlagBytes));
	memset((BYTE *)&FlagBytes, 0, sizeof(TFlagBytes));
	dwCurTicks = nwy_uptime_get();
	dwCurTicksHis = dwCurTicks;
	nwy_sdk_timer_start(general_timer, &para); //�����Զ�ʱ һ��һ��
	
	nwy_ext_echo("\r\nUart_Task ing");
	while (1)
	{

		dwCurTicks = nwy_uptime_get();

		if ((dwCurTicks - dwCurTicksHis) > 10000) //�糬��10sδ����ʱ�������´򿪶�ʱ��
		{
			dwCurTicksHis = dwCurTicks;
			nwy_sdk_timer_stop(general_timer); //�رն�ʱ�������´�
			nwy_thread_sleep(50);
			nwy_sdk_timer_start(general_timer, &para); //�����Զ�ʱ һ��һ��
		}

		if (PowerOnReadMeter == 0)
		{
			DetectReadMeterStasus();
		}
		else
		{
			#if (CYCLE_METER_READING == YES)
			ToMqttByCycle();
			#endif
		}
		api_ReceData_UartTask();
		nwy_msg_queue_recv(TimerMessageQueue, sizeof(TFlagBytes),&FlagBytes, NWY_OSA_NO_SUSPEND);

		if (tIap_645.dwIapFlag == BASEMETER_UPGRADES)
		{
			MeterUpgrade645_Task();
		}
		else if (tIap_698.dwIapFlag == BASEMETER_UPGRADES)
		{
			MeterUpgrade698_Task();
		}
		else
		{
			if (api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE) //�жϵ�ǰ�����ϴ�ģʽ�򲻻����͸��
			{
				//��Ϣ���п���ͬʱ���˶��������Լ����жϽ��պ���һ֡͸�����ݣ��ж�˳���ܱ�
				if (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE)
				{
					RecvMsgQueFromTcp();
				}
			}
			if (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE) //͸������,��������г���
			{

				if (api_GetTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_HOUR) == TRUE)
				{
					api_UpdateForInte();
					api_ClrTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_HOUR);
				}
				if (api_GetTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_MINUTE) == TRUE)
				{
					nwy_dm_heapinfo(&heapinfo);
					nwy_vfs_free_size_get(buf,&iTmp);
					// nwy_ext_echo("RAM[%s]ROM[%d]", heapinfo, iTmp);
					#if (GPRS_POSITION == YES)
					if (bmin >= 5)
					{
					}
					else
					{
						bmin++;
						qwReadMeterFlag[0] |= (1 << (CycleReadMeterNum + eBIT_POSITION));
					}
					#endif
					// qwReadMeterFlag[0] |= (1 << (CycleReadMeterNum + eBIT_USEDCHANNEL));
					api_ClrTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_MINUTE);
				}
				if (api_GetTaskFlag(&FlagBytes, eTASK_SAMPLE_ID, eFLAG_SECOND) == TRUE)
				{
					api_ClrTaskFlag(&FlagBytes, eTASK_SAMPLE_ID, eFLAG_SECOND);
					#if (EVENT_REPORT == YES)
					qwReadMeterFlag[0] |= (1 << (CycleReadMeterNum + eBIT_WARNING));
					#endif
				}
				CollectMeterTask();
			}
		}
		nwy_thread_event_wait(uart_thread, &event, 50);
	}
}
