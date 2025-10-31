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
BYTE *TransdataPtr = NULL;
BYTE PowerOnReadMeter = 0; //上电抄表成功标志
uint64_t dwCurTicks = 0, dwCurTicksHis = 0;

static nwy_uartdcb_t dcb = {
	.baudrate = BAUDRATE_9600,
	.databits = 8,
	.stopbits = 1,
	.parity = PB_EVEN,
	.flowctrl = FC_NONE};
//扩展规约数据标识
const WORD EventOI[] = {0x1700};
const WORD PtandCt[] = {0x1118, 0x1119, 0x111A, 0x111B}; //CT1,CT2,pt1,pt2
const WORD Position[] = {0x1300};
const WORD Ep212Tcp[] = {0xFFFE};
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
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
	// nwy_gpio_value_set(TX_LIGHT, 0);//！！！
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

	nwy_timer_para_t TimeTImerPara = //！！！后续全部替换
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_uart_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};
	TimeTImerPara.expired_time = 1000;

	nwy_sdk_timer_start(uart_timer, &TimeTImerPara); //1s定时
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
					//重试三次失败 如果是服务端参数从sys回复
					if ((i >= (CycleReadMeterNum + eBIT_TCPNET)) && (i <= (CycleReadMeterNum + eBIT_MQTTUSER)))
					{
						api_ReadSystemFiletoRam(i - CycleReadMeterNum);
						//从文件系统恢复代表表内数据异常，闪灯提示？
					}
					if (i == (CycleReadMeterNum+eBIT_ADDR))//！！！
					{
						dcb.baudrate = BAUDRATE_115200;
						nwy_uart_dcb_set(UART_HD, &dcb);
						nwy_uart_rx_frame_timeout_set(UART_HD, BYTE_DANCE_TIMEOUT1);
						qwReadMeterFlag[0] |= (1 << (CycleReadMeterNum + eBIT_ADDR));
					} //可能需要防护一下 防止地址始终无法探测出
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
#if (CYCLE_METER_READING == YES)
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
	// WORD i;
	nwy_ext_echo("\r\nenter uart");
	nwy_ext_echo("\r\nlen:%d ", length);
	// for (i = 0; i < length; i++)
	// {
	// 	nwy_ext_echo("%02x ", str[i]);
	// }
	// if (1) //212协议纯透传  //上电抄读基表参数 全部完成后 置 212纯透传标志
	// {
		//直接丢给tcp  tcp判断格式 是212的话  取出通道ID  发送
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
	nwy_sdk_timer_start(uart_timer, &OneTimeTImerPara); ////212透传 通道空闲
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
	int hd = -1;
 	char dev_name_list[4][8] = {0};

	nwy_dev_name_list_get(NWY_DEV_TYPE_UART,dev_name_list);
	// nwy_ext_echo("\r\nuart dev_name_list0:%s", dev_name_list[0]);//打印是uart1，对应开发板uart4,uart1会一直输出数据
	// nwy_ext_echo("\r\nuart dev_name_list1:%s", dev_name_list[1]);//打印是uart2，对应开发板uart1
	// nwy_ext_echo("\r\nuart dev_name_list2:%s", dev_name_list[2]);//打印是uart3
	// nwy_ext_echo("\r\nuart dev_name_list3:%s", dev_name_list[3]);
	hd = nwy_uart_open(dev_name_list[1], BAUDRATE_9600, 0); 
	nwy_uart_dcb_set(hd, &dcb);
	nwy_uart_rx_frame_timeout_set(hd, BYTE_DANCE_TIMEOUT2);

	InitPoint(&Serial);
	memset((char *)&Serial.ProBuf[0], 0, UART_BUFF_MAX_LEN);
	memset(bHisReadMeter, 0xFF, sizeof(bHisReadMeter));
	UART_HD = hd;
	nwy_uart_rx_cb_register(hd, nwy_uart_recv_handle);
	for (j = CycleReadMeterNum; j < (CycleReadMeterNum + POWERON_READRPARA_NUM); j++) //置上电要抄读的bit
	{
		qwReadMeterFlag[0] |= (1 << j);
	}
	if ((eBIT_TOTAL + CycleReadMeterNum) > (sizeof(qwReadMeterFlag) * 8)) //标志不能超过DWORD*8
	{
		for (;;)
		{
			nwy_thread_sleep(1000);
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
		else //接受到完整报文
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
		//组包直接发送
		CommWithMeter_DBDF(0x14,1,(BYTE*)&ID,&temp);
		//api_SetSysStatus(eSYS_STASUS_START_COLLECT);//如果不判断回复帧的话 就不用锁uart！！！
		nwy_sdk_timer_stop(uart_timer);
		// nwy_start_timer(uart_timer, 2000); //10s定时
		nwy_ext_echo("\r\nrecv TCP STATUS Changed ");
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
	nwy_event_msg_t event;
	TFlagBytes FlagBytes;
	nwy_heapinfo_t heapinfo;
	char buf[10];//！！！路径不知道什么作用
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
	nwy_sdk_timer_start(general_timer, &para); //周期性定时 一秒一次
	
	nwy_ext_echo("\r\nUart_Task ing");
	while (1)
	{

		dwCurTicks = nwy_uptime_get();

		if ((dwCurTicks - dwCurTicksHis) > 10000) //如超过10s未进定时器。重新打开定时器
		{
			dwCurTicksHis = dwCurTicks;
			nwy_sdk_timer_stop(general_timer); //关闭定时器，重新打开
			nwy_thread_sleep(50);
			nwy_sdk_timer_start(general_timer, &para); //周期性定时 一秒一次
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
