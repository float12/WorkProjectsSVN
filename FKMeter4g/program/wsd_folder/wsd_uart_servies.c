//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
//创建人	王泉
//创建日期
//描述
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define BAUDRATE_115200 		115200		
#define BYTE_DANCE_TIMEOUT1 	50 			//字节间超时 ms 115200 间隔50ms 

#define BAUDRATE_9600 			9600		
#define BYTE_DANCE_TIMEOUT2 	200 		//字节间超时ms 9600 间隔200ms
#define TRANSPARENT_TIMEOUT		2000		//透传超时时间ms   	2s
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE IsMqttComMeterFlag = 0;//mqtt和表通信标志，用于接收处理
int UART_HD = 0;
int IntervalTime = 60; //采集间隔时间
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
BYTE UartRecTimeout = 0; //接收超时标志
static WORD SysTickCounter;
BYTE gTranType = 0;
TRealTimer tTimer = {0}; //防止 在58秒处开启标志，下一次进行后分钟数 不对，将time设为全局变量 开始抄读的时间作为上报时间？？？

tTranData Transdata;
BYTE PowerOnReadMeter = 0; //上电抄表成功标志
uint64_t dwCurTicks = 0, dwCurTicksHis = 0;

//扩展规约数据标识
const WORD EventOI[] = {0x1700};
const WORD PtandCt[] = {0x1118, 0x1119, 0x111A, 0x111B}; //CT1,CT2,pt1,pt2
const WORD Position[] = {0x1300};
const WORD Ep212Tcp[] = {0xFFFE};
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern int nwy_set_rx_frame_timeout(uint8_t hd, int time);
//初始化规约读写指针,初始化、接收超时、规约处理完三种情况调用，
void InitPoint(TSerial *p)
{
	p->ProStepDlt645 = 0;
	p->BeginPosDlt645 = 0;
	p->Dlt645DataLen = 0;
	p->RXWPoint = 0;
	p->RXRPoint = 0;
	p->RxOverCount = 0;
	// 698初始化
	p->ProStepDlt698_45 = 0;
	// DLT698_45规约报文在Serial[].ProBuf中的开始位置
	p->BeginPosDlt698_45 = 0;
	p->wLen = 0;
	p->Addr_Len = 6; // 通信报文中服务器地址SA长度，地址长度默认6字节
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
	OSI_LOGI(0, "nwy_app_timer_cb");
	SysTickCounter++;
	if (SysTickCounter >= 3600)
	{
		SysTickCounter = 0;
	}
	if ((SysTickCounter % 3600) == 0)
	{
		api_SetAllTaskFlagRam(eFLAG_HOUR); //小时任务
	}
	if ((SysTickCounter % 60) == 0)
	{
		api_SetAllTaskFlagRam(eFLAG_MINUTE); //分钟任务
	}
	if ((SysTickCounter % 30) == 0)
	{
		api_SetAllTaskFlagRam(eFLAG_HALF_MINUTE); //半分钟任务
	}
	api_SetAllTaskFlagRam(eFLAG_SECOND); //秒任务
	nwy_gpio_set_value(RUN_LIGHT,(SysTickCounter%2));
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
	//国标645规约
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
//功能描述:  抄表任务
//
//参数:      bBtep 传入标志bit位
//
//返回值:
//
//备注:
//--------------------------------------------------
void ReadMeterTask(BYTE bStep, BYTE *bAddr)
{
	BYTE bTempStep;
	BYTE BUF[] = {0x07,0xE9,0X0A,0X18,0X0B,0X14,0X23};
	bTempStep = (bStep - CycleReadMeterNum);
	if (bTempStep & 0x80)
	{
		bTempStep = 0xff;
	}
	// nwy_ext_echo("\r\nreadMeter step is%d", bStep);
	// nwy_ext_echo("\r\nreadMeter bTempstep is%d", bTempStep);
	nwy_gpio_set_value(LOCAL_LIGHT, 0);
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
	#if (CYCLE_METER_READING == PROTOCOL_645)
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
	case eBIT_RealTime_698:
		api_GetRequestNormalList();
		// api_GetRequestNormal(testoi,2,0);
		// api_GetRequestRecordMethod1(BUF,NULL);
		break;
	case eBIT_Freeze_698:
		api_GetRequestRecordMethod1(BUF,NULL);
		break;
	default:
		break;
	}

	nwy_stop_timer(uart_timer);
	nwy_start_timer(uart_timer, 1000); //1s定时
	api_SetSysStatus(eSYS_STASUS_START_COLLECT);
}

//--------------------------------------------------
//功能描述:  遍历抄表bit位
//
//参数:      bLoop  当前回路数
//
//返回值:
//
//备注:  遍历标志位，有标志则占用串口进行发送
//--------------------------------------------------
BOOL TraverseMeterReadingSigns(BYTE bLoop, BYTE *bAddr)
{
	BYTE i;
	// nwy_ext_echo("\r\nsqReadMeterFlag IS %lx",&qwReadMeterFlag[0]);
	for (i = 0; i < MAX_READMETER; i++)
	{
		if (qwReadMeterFlag[bLoop] & (1ull << i))
		{
			nwy_ext_echo("\r\nFlag bit is%d", i);
			nwy_ext_echo("\r\nbHisReadMeter[bLoop] is%d", bHisReadMeter[bLoop]);
			if ((bHisReadMeter[bLoop] == i))
			{
				if (bReadMeterRetry >= 3)
				{
					qwReadMeterFlag[bLoop] &= ~(1ull << i);
					bReadMeterRetry = 0;
					//重试三次失败 如果是服务端参数从sys回复
					if ((i >= (CycleReadMeterNum + eBIT_TCPNET)) && (i <= (CycleReadMeterNum + eBIT_MQTTUSER)))
					{
						api_ReadSystemFiletoRam(i - CycleReadMeterNum);
						//从文件系统恢复代表表内数据异常，闪灯提示？
					}
					if (i == (CycleReadMeterNum+eBIT_ADDR))
					{
						nwy_uart_set_baud(UART_HD,BAUDRATE_9600);
						nwy_set_rx_frame_timeout(UART_HD, BYTE_DANCE_TIMEOUT1);
						qwReadMeterFlag[0] |= (1ull << (CycleReadMeterNum + eBIT_ADDR));
					}//可能需要防护一下 防止地址始终无法探测出
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
//功能描述: 多回路采集任务
//
//参数:
//
//返回值:
//
//备注:
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
#if (CYCLE_METER_READING != PROTOCOL_NO)
//--------------------------------------------------
//功能描述:  周期性与mqtt任务交互，目前是60分钟交互一次
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void ToMqttByCycle(void)
{
	BYTE bTime;
	#if (CYCLE_METER_READING == PROTOCOL_645)
	BYTE i, j;
	#endif
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
			#if (CYCLE_METER_READING == PROTOCOL_645)
			for (i = 0; i < bUsedChannelNum; i++)
			{
				for (j = 0; j < CycleReadMeterNum; j++)
				{
					qwReadMeterFlag[i] |= (1ull << j);
				}
			}
			#elif(CYCLE_METER_READING == PROTOCOL_698)
			qwReadMeterFlag[0] |= (1 << eBIT_RealTime_698);
			#endif

			tTimer = TempTimer;
			if (IntervalTime >= 1)//保证上报整分数据时，秒是0
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
//功能描述:
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
static void nwy_uart_recv_handle(const char *str, uint32_t length)
{
	WORD wLen, wLen1;
	WORD i;
	nwy_ext_echo("\r\nenter uart");
	nwy_ext_echo("\r\nlen:%d", length);
	for (i = 0; i < length; i++)
	{
		nwy_ext_echo("%02x ", str[i]);
	}
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
		if (gTranType == eTRAN_BLETOOTH)
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
		else if (gTranType == eTRAN_TCP)
		{
			if (nwy_put_msg_que(UartDataToTcpMsgQue, &Transdata, 0xffffffff))
			{
				nwy_ext_echo("\r\nuart to mqtt/tcp queue ok,length:%d", length);
			}
			else
			{
				nwy_ext_echo("\r\nto mqtt queue err");
			}
		}
		else if (gTranType == eTRAN_MQTT)
		{
			if (nwy_put_msg_que(TranDataToMqttServerMsgQue, &Transdata, 0xffffffff))
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
		api_ClrSysStatus(eSYS_STASUS_START_TRAN); //亮灯 远程
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
	// nwy_gpio_set_value(RX_LIGHT, 1);
	// nwy_ext_echo("\r\n %s",Serial.ProBuf[0]);
}
//--------------------------------------------------
// 功能描述:  发送透传数据
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void SendTranData(BYTE *buf, int len)
{
	int RetLen = 0;

	RetLen = nwy_uart_send_data(UART_HD, buf, len);
	nwy_ext_echo("uart send len is \r\n %d", RetLen);
	nwy_stop_timer(uart_timer);
	// nwy_gpio_set_value(TX_LIGHT, 1);
	nwy_start_timer(uart_timer, TRANSPARENT_TIMEOUT);
}
//--------------------------------------------------
//功能描述:  串口初始化
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void Init_UartPara_DL645(void)
{
	BYTE j;

	int hd = nwy_uart_init(NWY_NAME_UART1, 1); // 初始化
	nwy_uart_set_baud(hd, BAUDRATE_115200);		   // 设置默认波特率
	nwy_uart_set_para(hd, NWY_UART_EVEN_PARITY, NWY_UART_DATA_BITS_8, NWY_UART_STOP_BITS_1, 0);
	nwy_set_rx_frame_timeout(hd, BYTE_DANCE_TIMEOUT1);
	InitPoint(&Serial);
	memset((char *)&Serial.ProBuf[0], 0, UART_BUFF_MAX_LEN);
	memset(bHisReadMeter, 0xFF, sizeof(bHisReadMeter));
	UART_HD = hd;
	nwy_uart_reg_recv_cb(hd, nwy_uart_recv_handle);
	for (j = CycleReadMeterNum; j < (CycleReadMeterNum + POWERON_READRPARA_NUM); j++) //置上电要抄读的bit
	{
		qwReadMeterFlag[0] |= (1ull << j);
	}
	if ((eBIT_TOTAL + CycleReadMeterNum) > (sizeof(qwReadMeterFlag) * 8)) //标志不能超过DWORD*8
	{
		for (;;)
		{
			nwy_sleep(1000);
			nwy_ext_echo("error ,Bit overrun!!!");
		}
	}
}
//--------------------------------------------------
//功能描述:  把数据从报文开始位置 向 以p->ProBuf[0]开始处移
//
//参数:
//
//返回值:
//
//备注:
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
//功能描述:  数据处理任务
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void api_ReceData_UartTask(void)
{
	BYTE Result = FALSE;
	WORD ReceNum = 0;

	ReceNum = Serial.RXWPoint;
	while (ReceNum != Serial.RXRPoint)
	{
		nwy_start_timer(Uart_Recv_timer, 500);
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
		else //接受到完整报文
		{
			nwy_stop_timer(Uart_Recv_timer);
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
//功能描述:  uart接收字节间超时定时器
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void Uart_Recv_timer_cb(void *type)
{
	UartRecTimeout = 1;
}
//--------------------------------------------------
//功能描述:  上电检测检测抄表状态
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void DetectReadMeterStasus(void)
{
	BYTE j;

	for (j = CycleReadMeterNum; j < (CycleReadMeterNum + POWERON_READRPARA_NUM); j++) //置上电要抄读的bit
	{
		if ((qwReadMeterFlag[0] & (1ull << j)) == (1ull << j))
		{
			return;
		}
	}
	PowerOnReadMeter = 1;
}
//--------------------------------------------------
// 功能描述:  接收来自 mqtt user线程的数据
//
// 参数: ReadMeterInfo 读表信息
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void HandleMsgFromMqttUser(TReadMeterInfo *ReadMeterInfo)
{
	if (ReadMeterInfo->Type == eREAD_METER_EXTENDED)
	{
		CommWithMeter_DBDF(READ_METER_CONTROL_BYTE, 0, (BYTE *)&ReadMeterInfo->Extended645ID, NULL);
	}
	else if (ReadMeterInfo->Type == eSET_METER_EXTENDED)
	{
		CommWithMeter_DBDF(SET_METER_CONTROL_BYTE, ReadMeterInfo->DataLen, (BYTE *)&ReadMeterInfo->Extended645ID, (BYTE *)&ReadMeterInfo->Data);
	}
	else if (ReadMeterInfo->Type == eREAD_METER_STANDARD)
	{
		Dlt645_Tx_Read(ReadMeterInfo->Standard645ID);
	}
	else if (ReadMeterInfo->Type == eSET_METER_STANDARD)
	{
		Dlt645_Tx_Write(ReadMeterInfo->Standard645ID, ReadMeterInfo->DataLen, (BYTE *)&ReadMeterInfo->Data, ReadMeterInfo->Control);
	}
}
//--------------------------------------------------
//功能描述:  接收来自 tcp线程的下行数据 
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  RecvMsgQueFromTcp( void )
{	
	TReadMeterInfo ReadMeterInfo = {0};
	BYTE temp = 0;
	WORD ID = 0x2001;

	#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	if (nwy_get_msg_que(Ep212ToUartMsgQue,&Transdata,0xffffffff))
	{
		//判断 接受报文的格式  flag 标志位 是否 需要应答  再对应对uart资源操作锁！！！多主站待测试
		//取固定buf[84] (flag 标志) 
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
	else if (nwy_get_msg_que(TranDataToUartMessageQueue, &Transdata, 0xffffffff))
	{
		api_SetSysStatus(eSYS_STASUS_START_TRAN);
		gTranType = Transdata.TranType;
		SendTranData(Transdata.buf, Transdata.len);
		nwy_ext_echo("\r\ntran data send ok");
	}
	else if (nwy_get_msg_que(UserTcpStatusChangeMsgQue, &temp, 0xffffffff))
	{
		//组包直接发送
		CommWithMeter_DBDF(SET_METER_CONTROL_BYTE,1,(BYTE*)&ID,&temp);
		api_SetSysStatus(eSYS_STASUS_START_COLLECT);//如果不判断回复帧的话 就不用锁uart！！！
		nwy_stop_timer(uart_timer);
		nwy_start_timer(uart_timer, 2000); //10s定时
	}
	else if (nwy_get_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff))
	{
		nwy_ext_echo("\r\n recv from mqtt user:%08x", ReadMeterInfo.Standard645ID);
		api_SetSysStatus(eSYS_STASUS_START_COLLECT);
		HandleMsgFromMqttUser(&ReadMeterInfo);
	}
}
//--------------------------------------------------
//功能描述:  UART线程主任务
//
//参数:
//
//返回值:
//
//备注:  可为每个uart创建，禁止使用静态变量
//--------------------------------------------------
void Uart_Task(void *parameter)
{
	//该路串口初始化
	nwy_osiEvent_t event;
	TFlagBytes FlagBytes;
	#if (GPRS_POSITION == YES)
	BYTE bmin = 0;
	#endif
	Init_UartPara_DL645();
	memset((BYTE *)&FlagBytesRam, 0, sizeof(TFlagBytes));
	memset((BYTE *)&FlagBytes, 0, sizeof(TFlagBytes));
	dwCurTicks = nwy_get_ms();
	dwCurTicksHis = dwCurTicks;
	nwy_start_timer_periodic(general_timer, 1000); //周期性定时 一秒一次

	while (1)
	{

		dwCurTicks = nwy_get_ms();

		if ((dwCurTicks - dwCurTicksHis) > 10000) //如超过10s未进定时器。重新打开定时器
		{
			dwCurTicksHis = dwCurTicks;
			nwy_stop_timer(general_timer); //关闭定时器，重新打开
			nwy_sleep(50);
			nwy_start_timer_periodic(general_timer, 1000); //周期性定时 一秒一次
		}

		if (PowerOnReadMeter == 0)
		{
			DetectReadMeterStasus();
		}
		else
		{
			#if (CYCLE_METER_READING != PROTOCOL_NO)
			ToMqttByCycle();
			#endif
		}
		api_ReceData_UartTask();
		nwy_get_msg_que(TimerMessageQueue, &FlagBytes, 0xffffffff);

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
			if (api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE) //判断当前处于上传模式则不会进行透传
			{
				//消息队列可能同时攒了多条，所以加上判断接收后发下一帧透传数据，判断顺序不能变
				if (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE)
				{
					RecvMsgQueFromTcp();
				}
			}
			if (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE) //透传结束,才允许进行抄表
			{

				if (api_GetTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_HOUR) == TRUE)
				{
					// api_UpdateForInte();
					api_ClrTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_HOUR);
				}
				if (api_GetTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_MINUTE) == TRUE)
				{
					#if (GPRS_POSITION == YES)
					if (bmin >= 5)
					{
					}
					else
					{
						bmin++;
						qwReadMeterFlag[0] |= (1ull << (CycleReadMeterNum + eBIT_POSITION));
					}
					#endif
					qwReadMeterFlag[0] |= (1ull << (CycleReadMeterNum + eBIT_VERSION));
					api_ClrTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_MINUTE);
				}
				if (api_GetTaskFlag(&FlagBytes, eTASK_SAMPLE_ID, eFLAG_SECOND) == TRUE)
				{
				
					api_ClrTaskFlag(&FlagBytes, eTASK_SAMPLE_ID, eFLAG_SECOND);
					#if (EVENT_REPORT == YES)
					qwReadMeterFlag[0] |= (1ull << (CycleReadMeterNum + eBIT_EVENT));
					#endif
				}
				CollectMeterTask();
			}
		}
		nwy_wait_thead_event(uart_thread, &event, 50);
	}
}

// 关于透传 如果正在抄表时，突然来透传报文则透传失败