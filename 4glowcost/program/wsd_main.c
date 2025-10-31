//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
// 
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//线程使用堆栈
#define UART_STACK							(1024 * 15)
#define TCP_USER_STACK						(1024 * 10)
#define TCP_PRIVATE_STACK					(1024 * 10)	
#define MQTT_USER_STACK						(1024 * 10)
#define MQTT_PRIVATE_STACK					(1024 * 10)
#define DATA_CALL_STACK						(1024 * 4)
#define FACTORY_STACK						(1024 * 13)
#define FTP_STACK							(1024 * 13)
#define MONITIOR_STACK						(1024 * 10)
//消息队列消息数目
#define MQTT_RES_MSG_QUEUE_NUM 				10
#define MQTT_TRANS_MSG_QUEUE_NUM 			10
#define MQTT_RES_USER_MSG_QUEUE_NUM 		10
#define FTP_UPDATE_MSG_QUEUE_NUM 			3
#define COLLECT_MSG_QUEUE_NUM 				20
#define TIMER_MSG_QUEUE_NUM 				20
#define UART_TO_TCP_MSG_QUEUE_NUM 			5
#define TRAN_TO_MQTT_SERVER_MSG_QUEUE_NUM 	5
#define TRAN_TO_UART_MSG_QUEUE_NUM 			5
#define TRAN_TO_FACTORY_MSG_QUEUE_NUM 		5
#define UPGRADE_RESULT_MSG_QUEUE_NUM 		10
#define TCP_TO_MODULE_MSG_QUEUE_NUM 		5
#define MODULE_TO_TCP_MSG_QUEUE_NUM 		5
#define EVENT_REPORT_MSG_QUEUE_NUM 			4
#define EP212_MSG_QUEUE_NUM					5
// //-----------------------------------------------
// //		本文件使用的结构体，共用体，枚举
// //-----------------------------------------------

// //-----------------------------------------------
// //				全局使用的变量，常量
// //-----------------------------------------------
// DWORD ThreadRunCnt[eMaxThreadNum] = {0};
// DWORD LastThreadRunCnt[eMaxThreadNum] = {0};
//消息队列数据存储
CollectionDatas CollectionDatasQueue[COLLECT_MSG_QUEUE_NUM];
tTranData UartDataToTcpMsgArr[UART_TO_TCP_MSG_QUEUE_NUM];
tTranData TranDataToMqttServerMsgArr[TRAN_TO_MQTT_SERVER_MSG_QUEUE_NUM];
tTranData TranDataToUartMessageArr[TRAN_TO_UART_MSG_QUEUE_NUM];
tTranData TranDataToFactoryMessageArr[TRAN_TO_FACTORY_MSG_QUEUE_NUM];
#if (TCP_COMM_MODULE == YES)
tTranData TcpDataToModuleMsgArr[TCP_TO_MODULE_MSG_QUEUE_NUM];
tTranData ModuleDataToTcpMsgArr[MODULE_TO_TCP_MSG_QUEUE_NUM];
#endif
#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
tTranData EpDataToUartMsgArr[EP212_MSG_QUEUE_NUM];
tTranData UartDataToEpTcpMsgArr[EP212_MSG_QUEUE_NUM];
#endif
//消息队列属性
MessageQueueInfo MessageQueueInfoArr[] =
{
	{
		.buf = (BYTE*)&CollectionDatasQueue[0],
		.MessageSize = sizeof(CollectionDatas),
		.MessageNum = COLLECT_MSG_QUEUE_NUM,
		.WriteOffset = 0,
		.QueueHandle = &CollectMessageQueue,
	},
	{
		.buf = (BYTE*)&UartDataToTcpMsgArr[0],
		.MessageSize = sizeof(tTranData),
		.MessageNum = UART_TO_TCP_MSG_QUEUE_NUM,
		.WriteOffset = 0,
		.QueueHandle = &UartDataToTcpMsgQue,
	},
	{
		.buf = (BYTE*)&TranDataToMqttServerMsgArr[0],
		.MessageNum = TRAN_TO_MQTT_SERVER_MSG_QUEUE_NUM,
		.MessageSize = sizeof(tTranData),
		.WriteOffset = 0,
		.QueueHandle = &TranDataToMqttServerMsgQue,
	},
	{
		.buf =(BYTE*)&TranDataToUartMessageArr[0],
		.MessageNum = TRAN_TO_UART_MSG_QUEUE_NUM,
		.MessageSize = sizeof(tTranData),
		.WriteOffset = 0,
		.QueueHandle = &TranDataToUartMessageQueue,
	},
	{
		.buf = (BYTE*)&TranDataToFactoryMessageArr[0],
		.MessageNum = TRAN_TO_FACTORY_MSG_QUEUE_NUM,
		.MessageSize = sizeof(tTranData),
		.WriteOffset = 0,
		.QueueHandle = &TranDataToFactoryMessageQueue,
	},
	#if (TCP_COMM_MODULE == YES)
	{
		.buf = (BYTE*)&TcpDataToModuleMsgArr[0],
		.MessageNum = TCP_TO_MODULE_MSG_QUEUE_NUM,
		.MessageSize = sizeof(tTranData),
		.WriteOffset = 0,
		.QueueHandle = &TcpDataToModuleMsgQue,
	},
	{
		.buf = (BYTE*)&ModuleDataToTcpMsgArr[0],
		.MessageNum = MODULE_TO_TCP_MSG_QUEUE_NUM,
		.MessageSize = sizeof(tTranData),
		.WriteOffset = 0,
		.QueueHandle = &ModuleDataToTcpMsgQue,
	},
	#endif
	#if (CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	{
		.buf = (BYTE*)&EpDataToUartMsgArr[0],
		.MessageNum = EP212_MSG_QUEUE_NUM,
		.MessageSize = sizeof(tTranData),
		.WriteOffset = 0,
		.QueueHandle = &Ep212ToUartMsgQue,
	},
	{
		.buf = (BYTE*)&UartDataToEpTcpMsgArr[0],
		.MessageNum = EP212_MSG_QUEUE_NUM,
		.MessageSize = sizeof(tTranData),
		.WriteOffset = 0,
		.QueueHandle = &UartToEp212MsgQue,
	},
	#endif
};

nwy_osi_timer_t ftp_timer = NULL;
nwy_osi_timer_t meter_upgrade_timer = NULL;//表升级全过程最大时间定时器
nwy_osi_timer_t  User_MQTT_Reconnect_timer = NULL;
nwy_osi_timer_t general_timer = NULL;
nwy_osi_timer_t g_timer = NULL;
nwy_osi_timer_t uart_timer = NULL;
nwy_osi_timer_t Ble_Recv_ByteTimeout_timer = NULL;//蓝牙帧间超时定时器
nwy_osi_timer_t Uart_Recv_timer = NULL;//uart帧间超时定时器

#if (TCP_COMM_MODULE == YES)
nwy_osi_timer_t Tcp_ComModule_timer = NULL;
#endif
nwy_osi_mutex_t QueueWriteMutex = NULL;
nwy_osi_thread_t g_app_thread = NULL;
nwy_osi_thread_t Timer_thread = NULL;
nwy_osi_thread_t Date_call_thread = NULL;
nwy_osi_thread_t tcp_thread[TCP_TOTAL_NUM] = {NULL};
nwy_osi_thread_t mqtt_thread = NULL;
nwy_osi_thread_t userserver_mqtt_thread = NULL;
nwy_osi_thread_t uart_thread = NULL;
nwy_osi_thread_t FtpFotaUpdate_thread = NULL;
nwy_osi_thread_t ftp_thread = NULL;
nwy_osi_thread_t factorypara_thread = NULL;
nwy_osi_thread_t location_thread = NULL;
nwy_osi_thread_t Monitor_Thread = NULL;
nwy_osi_msg_queue_t MqttResMessageQueue = NULL;
nwy_osi_msg_queue_t MqttTransMessageQueue = NULL;	//mqtt 专用透传队列
nwy_osi_msg_queue_t MqttResUserMessageQueue = NULL;
nwy_osi_msg_queue_t FtpUpdateMessageQueue = NULL;
nwy_osi_msg_queue_t CollectMessageQueue = NULL;
nwy_osi_msg_queue_t TimerMessageQueue = NULL;
nwy_osi_msg_queue_t UartDataToTcpMsgQue = NULL;
nwy_osi_msg_queue_t TranDataToMqttServerMsgQue = NULL;
nwy_osi_msg_queue_t TranDataToUartMessageQueue = NULL;
nwy_osi_msg_queue_t TranDataToFactoryMessageQueue = NULL;
nwy_osi_msg_queue_t UpgradeResultMessageQueue = NULL;
#if (TCP_COMM_MODULE == YES)
nwy_osi_msg_queue_t TcpDataToModuleMsgQue = NULL;
nwy_osi_msg_queue_t ModuleDataToTcpMsgQue = NULL;
#endif
#if(EVENT_REPORT == YES)
nwy_osi_msg_queue_t EventReportMessageQueue = NULL;
#endif
#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
nwy_osi_msg_queue_t Ep212ToUartMsgQue = NULL;
nwy_osi_msg_queue_t UartToEp212MsgQue = NULL;
#endif
nwy_osi_msg_queue_t UserTcpStatusChangeMsgQue;//tcp 用户端状态改变通知
// //-----------------------------------------------
// //				本文件使用的变量，常量
// //-----------------------------------------------
static BYTE	 PowerDownFlag = 0;
static BYTE	 HisUserTcpConFlag = 0;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
void Monitor_task(void *param);
// void Timer_Task(void *param);
// //-----------------------------------------------
// //				函数定义
// //-----------------------------------------------
// //--------------------------------------------------
void  CheckSysVolandConnectStatus( void )
{
	BYTE i,UserTcpConFlag = 0;

	for (i = USER_SPECIAL_START; i < (g_EpTcpUserNum+ USER_SPECIAL_START); i++)
	{
		if(api_GetTcpConStatus(i))
		{
			//g_EpTcpUserChannel 查表 找MAX_PRIVATE_SERVER_NUM 对应 bit通道关系
			//nwy_ext_echo("\r\n tcp status ture %d",i);
			UserTcpConFlag |= 1<<(api_GetTcpChannelTable(eMETER, i-1));

		}
		else
		{
			//nwy_ext_echo("\r\n tcp status false %d",i);
			UserTcpConFlag &= ~(1<<(api_GetTcpChannelTable(eMETER, i-1)));
		}
	}
	if (HisUserTcpConFlag != UserTcpConFlag)//tcp_recv sleep60 秒 导致检测状态会等60 
	{
		//丢队列 通知基表！！！状态通知 需要考虑 基表未收到的情况吗？感觉不需要判断回复帧
		HisUserTcpConFlag = UserTcpConFlag;
		if (nwy_msg_queue_send(UserTcpStatusChangeMsgQue,1 , &UserTcpConFlag,NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
		{
			nwy_ext_echo("\r\n tcp status changed");
		}
	}
}
// //功能描述:  控灯  掉电检测
// //         
// //参数:      
// //         
// //返回???:    
// //         
// //备注:  
// //--------------------------------------------------
void  CheckSysVolandControlLight( void )
{

	if (nwy_adc_get(NWY_ADC_CHANNEL2, NWY_ADC_SCALE_5V000) < POWER_DOWN_VOLTAGE)
	{
		if (PowerDownFlag == 0)
		{
			nwy_ext_echo("\r\n power down");
			api_WriteSysUNMsg(POWER_DOWN);
			PowerDownFlag = 0xAA;
		}
	}
 	#if(USER_SERVER_LIGHT == TCP_USER)
 	if (api_GetTcpConStatus(0))
 	{
 		REMOTE_LIGHT_OPEN;
 	}
 	#else
 	if (api_GetMqttConStatus(1))
 	{
 		REMOTE_LIGHT_OPEN;
 	}
 	#endif
 	else
 	{
 		REMOTE_LIGHT_CLOSE;
 	}
}
#if (TEST_STACK == YES)
void MonitorStackSize()
{
	nwy_thread_info_t ThreadInfo;
	memset((BYTE *)&ThreadInfo, 0, sizeof(nwy_thread_info_t));
	nwy_thread_info_get(uart_thread, &ThreadInfo);
	nwy_ext_echo("\r\n uart_thread allocated stack size:%d , minimum:%d,  maximum use:%d", UART_STACK * 4, ThreadInfo.thread_stack_highwatermark * 4, UART_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_thread_info_get(Monitor_Thread, &ThreadInfo);
	nwy_ext_echo("\r\n Monitor_Thread allocated stack size:%d , minimum:%d,  maximum use:%d",MONITIOR_STACK * 4, ThreadInfo.thread_stack_highwatermark * 4, MONITIOR_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_thread_info_get(Date_call_thread, &ThreadInfo);
	nwy_ext_echo("\r\n Date_call_thread allocated stack size:%d , minimum:%d,  maximum use:%d", DATA_CALL_STACK * 4,ThreadInfo.thread_stack_highwatermark * 4, DATA_CALL_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_thread_info_get(tcp_thread[0], &ThreadInfo);
	nwy_ext_echo("\r\n tcp_private_thread allocated stack size:%d, minimum:%d,  maximum use:%d", TCP_PRIVATE_STACK * 4, ThreadInfo.thread_stack_highwatermark * 4, TCP_PRIVATE_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
#if (TCP_USER == YES)
	nwy_thread_info_get(tcp_thread[MAX_PRIVATE_SERVER_NUM], &ThreadInfo);
	nwy_ext_echo("\r\n tcp_user_thread allocated stack size:%d, minimum:%d,  maximum use:%d", TCP_USER_STACK * 4, ThreadInfo.thread_stack_highwatermark * 4, TCP_USER_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
#endif
	nwy_thread_info_get(mqtt_thread, &ThreadInfo);
	nwy_ext_echo("\r\n mqtt_private_thread allocated stack size:%d , minimum:%d,  maximum use:%d", MQTT_PRIVATE_STACK * 4, ThreadInfo.thread_stack_highwatermark * 4, MQTT_PRIVATE_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
#if (MQTT_USER == YES)
	nwy_thread_info_get(userserver_mqtt_thread, &ThreadInfo);
	nwy_ext_echo("\r\n mqtt_user_thread allocated stack size:%d , minimum:%d,  maximum use:%d", MQTT_USER_STACK * 4, ThreadInfo.thread_stack_highwatermark * 4, MQTT_USER_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
#endif
	nwy_thread_info_get(ftp_thread, &ThreadInfo);
	nwy_ext_echo("\r\n ftp_thread allocated stack size:%d , minimum:%d,  maximum use:%d", FTP_STACK * 4, ThreadInfo.thread_stack_highwatermark * 4, FTP_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_thread_info_get(factorypara_thread, &ThreadInfo);
	nwy_ext_echo("\r\n factorypara_thread allocated stack size:%d , minimum:%d,  maximum use:%d", FACTORY_STACK * 4, ThreadInfo.thread_stack_highwatermark * 4, FACTORY_STACK * 4 - ThreadInfo.thread_stack_highwatermark * 4);
}
#endif

//--------------------------------------------------
void Monitor_task(void *param)
{
	while (1)
	{
		CheckSysVolandControlLight();
#if (TEST_STACK == YES)
		//  MonitorStackSize();
#endif
		CheckSysVolandConnectStatus();
		
		 nwy_thread_sleep(1000);
	 }
}

void Timer_Task(void *param)
{
	nwy_event_msg_t event = {0};
	nwy_heapinfo_t heapinfo = {0};
	//char buf[10] = {"/"}; //！！！路径不知道什么作用
	QWORD iTmp = 0;
	while (1)
	{
		nwy_thread_event_wait(Timer_thread, &event, 100);
		// nwy_ext_echo("\r\nTimer_Task ing");
		nwy_dm_heapinfo(&heapinfo);
		nwy_vfs_free_size_get(NULL, &iTmp);
		// nwy_ext_echo("RAM[%d][%d][%d][%d]ROM[%lld]", heapinfo.start,heapinfo.size,heapinfo.avail_size,heapinfo.max_block_size,iTmp);
		nwy_thread_sleep(20);
	}
}
void nwy_uart_timer_cb(void *type)
{
	api_SetSysStatus(eSYS_STASUS_TIMER_UP);
	if (api_GetSysStatus(eSYS_STASUS_START_TRAN))
	{
		api_ClrSysStatus(eSYS_STASUS_START_TRAN);
	}
	if (api_GetSysStatus(eSYS_STASUS_START_COLLECT))
	{
		api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
	}
	nwy_ext_echo("\r\n uart timer timeout !");
}
//--------------------------------------------------
//功能描述:  队列写入
//         
//参数:      queueInfo 队列信息 data 写入数据 dataSize 数据大小 timeout 超时时间
//         
//返回值:    TRUE 成功 FALSE 失败
//         
//备注:  
//--------------------------------------------------
BYTE QueueWrite(MessageQueueInfo* queueInfo, tTranData * data, DWORD dataSize, SDWORD timeout)
{
	uint32 free_space = 0;
    int ret = 0;
	BYTE* write_pos = NULL;
	if ((queueInfo == NULL) || (queueInfo->buf == NULL) 
		|| (queueInfo->QueueHandle == NULL) || (data == NULL)
		|| (dataSize != queueInfo->MessageSize))
	{
		nwy_ext_echo("\r\n QueueWrite param error!");
		return FALSE;
	}
	if (nwy_sdk_mutex_lock(QueueWriteMutex, 2000) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n QueueWrite mutex lock fail");
		return FALSE;
	}
	ret = nwy_msg_queue_space_num_get(*(queueInfo->QueueHandle), &free_space);
	if ((ret != NWY_SUCCESS) || (free_space < 1))
	{
		nwy_ext_echo("\r\n QueueWrite queue is full or ret err,ret:%d,free_space:%d", ret, free_space);
		nwy_sdk_mutex_unlock(QueueWriteMutex);
		return FALSE; //  队列满或获取空间失败
	}
	nwy_ext_echo("\r\nwrite offset:%d", queueInfo->WriteOffset);
	write_pos = (BYTE*)(queueInfo->buf + (queueInfo->WriteOffset * queueInfo->MessageSize));
	memcpy(write_pos, data, queueInfo->MessageSize);
	queueInfo->WriteOffset++;
	if (queueInfo->WriteOffset >= queueInfo->MessageNum)
	{
		queueInfo->WriteOffset = 0;
	}
	ret = nwy_msg_queue_send(*(queueInfo->QueueHandle),sizeof(char*), &write_pos, timeout);
	if (ret != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n QueueWrite send error,ret:%d", ret);
		nwy_sdk_mutex_unlock(QueueWriteMutex);
		return FALSE;
	}
	else
	{
		nwy_sdk_mutex_unlock(QueueWriteMutex);
		return TRUE;
	}
}
// //--------------------------------------------------
// //功能描述:  初始化外部相关的管脚 和 置一些标志
// //         
// //参数:      
// //         
// //返回???:    
// //         
// //备注:  
// //--------------------------------------------------
void  InitExternalPinsandFlags( void )
{
	nwy_gpio_direction_set(REMOTE_LIGHT, PIN_DIRECTION_OUT);
	nwy_gpio_direction_set(FLASH_PIN, PIN_DIRECTION_OUT);
	// nwy_gpio_direction_set(TX_LIGHT, PIN_DIRECTION_OUT);
	// nwy_gpio_direction_set(RX_LIGHT, PIN_DIRECTION_OUT);
	nwy_gpio_direction_set(LOCAL_LIGHT, PIN_DIRECTION_OUT);
	nwy_gpio_direction_set(RUN_LIGHT, PIN_DIRECTION_OUT);
	nwy_gpio_value_set(REMOTE_LIGHT,0);
	nwy_gpio_value_set(FLASH_PIN,1);//???件原???
	nwy_gpio_value_set(RUN_LIGHT, 0);
	nwy_gpio_value_set(LOCAL_LIGHT, 0);
	// nwy_gpio_value_set(TX_LIGHT, 0);
	// nwy_gpio_value_set(RX_LIGHT, 0);
	api_ClrSysStatus(eSYS_STASUS_TIMER_UP); //???前先放着???
	api_ClrSysStatus(eSYS_STASUS_START_TRAN);
	api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
	api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
	api_PowerOnCreatSyseventTable();
}
static void prvThreadEntry(void *param)
{
	static int profileID = 1;//????????????????????????datacall??0????????????????????
	BYTE ServerNum = 0;
	nwy_timer_para_t para;
 	memset(&para, 0, sizeof(para));
	// // 创建使用到的消息队列
	nwy_msg_queue_create(&MqttResMessageQueue, sizeof(char *), MQTT_RES_MSG_QUEUE_NUM);
	if (MqttResMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n MqttResMessageQueue creat err");
	}

	// nwy_msg_queue_create(&MqttTransMessageQueue, sizeof(char *), MQTT_TRANS_MSG_QUEUE_NUM);
	// if (MqttTransMessageQueue == NULL)
	// {
	// 	nwy_ext_echo("\r\n MqttTransMessageQueue creat err");
	// }

	nwy_msg_queue_create(&MqttResUserMessageQueue, sizeof(char *), MQTT_RES_USER_MSG_QUEUE_NUM);
	if (MqttResUserMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n MqttResMessageQueue creat err");
	}

	nwy_msg_queue_create(&FtpUpdateMessageQueue, sizeof(char *), FTP_UPDATE_MSG_QUEUE_NUM);
	if (FtpUpdateMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n FtpUpdateMessageQueue creat err");
	}

	nwy_msg_queue_create(&CollectMessageQueue, sizeof(char *), COLLECT_MSG_QUEUE_NUM);
	if (CollectMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n CollectMessageQueue creat err");
	}

	nwy_msg_queue_create(&TimerMessageQueue, sizeof(TFlagBytes), TIMER_MSG_QUEUE_NUM);
	if (TimerMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n TimerMessageQueue creat err");
	}

	nwy_msg_queue_create(&UartDataToTcpMsgQue, sizeof(char *), UART_TO_TCP_MSG_QUEUE_NUM);
	if (UartDataToTcpMsgQue == NULL)
	{
		nwy_ext_echo("\r\n UartDataToTcpMsgQue creat err");
	}

	nwy_msg_queue_create(&TranDataToMqttServerMsgQue, sizeof(char *), TRAN_TO_MQTT_SERVER_MSG_QUEUE_NUM);
	if (TranDataToMqttServerMsgQue == NULL)
	{
		nwy_ext_echo("\r\n TranDataToMqttServerMsgQue creat err");
	}

	nwy_msg_queue_create(&TranDataToUartMessageQueue, sizeof(char *), TRAN_TO_UART_MSG_QUEUE_NUM);
	if (TranDataToUartMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n TranDataToUartMessageQueue creat err");
	}

	nwy_msg_queue_create(&TranDataToFactoryMessageQueue, sizeof(char *), TRAN_TO_FACTORY_MSG_QUEUE_NUM);
	if (TranDataToFactoryMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n TranDataToFactoryMessageQueue creat err");
	}

	nwy_msg_queue_create(&UpgradeResultMessageQueue, sizeof(BYTE), UPGRADE_RESULT_MSG_QUEUE_NUM);
	if (UpgradeResultMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n UpgradeResultMessageQueue creat err");
	}

#if (TCP_COMM_MODULE == YES)
	nwy_msg_queue_create(&TcpDataToModuleMsgQue, sizeof(char *), TCP_TO_MODULE_MSG_QUEUE_NUM);
	if (TcpDataToModuleMsgQue == NULL)
	{
		nwy_ext_echo("\r\n TcpDataToModuleMsgQue creat err");
	}

	nwy_msg_queue_create(&ModuleDataToTcpMsgQue, sizeof(char *), MODULE_TO_TCP_MSG_QUEUE_NUM);
	if (ModuleDataToTcpMsgQue == NULL)
	{
		nwy_ext_echo("\r\n ModuleDataToTcpMsgQue creat err");
	}
#endif

#if (EVENT_REPORT == YES)
	nwy_msg_queue_create(&EventReportMessageQueue, sizeof(Eventmessage), EVENT_REPORT_MSG_QUEUE_NUM);
	if (EventReportMessageQueue == NULL)
	{
		nwy_ext_echo("\r\n EventReportMessageQueue creat err");
	}
#endif

#if (CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	nwy_msg_queue_create(&Ep212ToUartMsgQue, sizeof(char *), EP212_MSG_QUEUE_NUM);
	if (Ep212ToUartMsgQue == NULL)
	{
		nwy_ext_echo("\r\n Ep212DataToUartMsgQue creat err");
	}

	nwy_msg_queue_create(&UartToEp212MsgQue, sizeof(char *), EP212_MSG_QUEUE_NUM);
	if (UartToEp212MsgQue == NULL)
	{
		nwy_ext_echo("\r\n UartDataToEp212MsgQue creat err");
	}
#endif
	nwy_msg_queue_create(&UserTcpStatusChangeMsgQue, sizeof(BYTE), 5);
	if (UserTcpStatusChangeMsgQue == NULL)
	{
		nwy_ext_echo("\r\n UserTcpStatusChangeMsgQue creat err");
	}
	nwy_thread_sleep(1000);
	// 创建使用的任务  注：使用事件的任务需要完成nwy_timer_init
	// DataCall_Task
 	// para.thread_hdl = NWY_TIMER_IN_ISR;
 	para.thread_hdl = Timer_thread;
 	para.cb = nwy_general_timer_cb;
 	para.cb_para = NULL;
 	para.expired_time = 1000;
 	para.type = NWY_TIMER_PERIODIC;
	//定时器 线程 
	nwy_thread_create(&Timer_thread,"Timer",NWY_OSI_PRIORITY_NORMAL, Timer_Task, NULL,8, 1024 * 5, NULL);

	nwy_sdk_timer_create(&general_timer,&para);
	para.type = NWY_TIMER_ONE_TIME;
	para.cb = Uart_Recv_timer_cb;
	if (nwy_sdk_timer_create(&Uart_Recv_timer,&para) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n Uart_Recv_timer create fail");
	}
	para.type = NWY_TIMER_ONE_TIME;
	para.cb = nwy_uart_timer_cb;
	if (nwy_sdk_timer_create(&uart_timer,&para) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n uart_timer create fail");
	}
	para.type = NWY_TIMER_ONE_TIME;
	para.cb = nwy_ftp_timer_cb;
	if (nwy_sdk_timer_create(&ftp_timer,&para) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n ftp_timer create fail");
	}
	para.type = NWY_TIMER_ONE_TIME;
	para.cb = nwy_meter_upgrade_timer_cb;
	if (nwy_sdk_timer_create(&meter_upgrade_timer,&para) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n meter_upgrade_timer create fail");
	}
	#if(MQTT_USER == YES)
	para.type = NWY_TIMER_ONE_TIME;
	para.cb = nwy_User_MQTT_Reconnect_timer_cb;
	if (nwy_sdk_timer_create(&User_MQTT_Reconnect_timer,&para) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n User_MQTT_Reconnect_timer create fail");
	}
	#endif
	#if (TCP_COMM_MODULE == YES)
	para.type = NWY_TIMER_ONE_TIME;
	para.cb = Tcp_Commodule_timer_cb;
	if (nwy_sdk_timer_create(&Tcp_ComModule_timer,&para) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n Tcp_ComModule_timer create fail");
	}
	#endif
	if (nwy_sdk_mutex_create(&QueueWriteMutex) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n QueueWriteMutex create fail");
	}
	//定时器 所有定时器放下一个线程 
	//Uart_Task
	nwy_thread_create(&uart_thread,"UartTask",NWY_OSI_PRIORITY_NORMAL,Uart_Task, NULL,8, UART_STACK, NULL);
	nwy_thread_sleep(5000);
	//Uart_Task
	for ( ServerNum = 0; ServerNum < USER_SPECIAL_START; ServerNum++)
	{
		nwy_thread_create(&tcp_thread[ServerNum],"TcpToUart",NWY_OSI_PRIORITY_NORMAL, TCP_PrivateTask, (void*)ServerNum, 8 ,TCP_PRIVATE_STACK,NULL );
		nwy_thread_sleep(2000);
	}
	#if(TCP_USER == YES)
	for ( ServerNum = USER_SPECIAL_START; ServerNum < (g_EpTcpUserNum + USER_SPECIAL_START); ServerNum++)
	{
		nwy_thread_create(&tcp_thread[ServerNum],"TcpToUser", NWY_OSI_PRIORITY_NORMAL, TCP_User_Task,(void*)ServerNum,8,TCP_USER_STACK, NULL);
		nwy_thread_sleep(2000);
	}
	#endif
	nwy_thread_create(&Date_call_thread,"DateCall",NWY_OSI_PRIORITY_NORMAL, DataCall_Task, (void*)&profileID, 8, DATA_CALL_STACK, NULL);
	//MQTT_Task
	nwy_thread_create(&mqtt_thread,"MqttTask",NWY_OSI_PRIORITY_NORMAL, WSD_MQTT_Task, NULL, 8, MQTT_PRIVATE_STACK,NULL );
	#if(MQTT_USER == YES)
	nwy_thread_create(&userserver_mqtt_thread,"MqttTask",NWY_OSI_PRIORITY_NORMAL, USER_MQTT_Task, NULL, 8, MQTT_USER_STACK, NULL);
	#endif
	nwy_thread_create(&ftp_thread,"FtpTask",NWY_OSI_PRIORITY_NORMAL,Fota_UpdateTask,NULL,30,FTP_STACK,NULL);
	//Factory_Task
	nwy_thread_create(&factorypara_thread,"FactoryTask",NWY_OSI_PRIORITY_NORMAL, Factory_Task, NULL, 8, FACTORY_STACK,NULL );//增加透传数据需要???大栈区
	#if(TEST_STACK == YES)
	nwy_thread_create(&Monitor_Thread,"MonitorTask",NWY_OSI_PRIORITY_NORMAL, Monitor_task, NULL,8 , MONITIOR_STACK,NULL );
	#endif
	// nwy_thread_exit(g_app_thread);
}
int nwy_open_app_entry()
{
	// warning:app build time length <= 64
	// char APP_BUILD_TIME[65] = {0};
	// char version[70] = {0};

	// sprintf(version, "\"%s\"", APP_VERSION);
	// sprintf(APP_BUILD_TIME, "\"%s,%s\"", __DATE__, __TIME__);
	// OSI_LOGI(0, "appimg_enter ...");
	// if(false == nwy_app_version(version, APP_BUILD_TIME))
	// {
		// OSI_LOGI(0, "app set version fail");
		// return 0;
	// }
	InitExternalPinsandFlags();
	api_WriteSysUNMsg(SYSUN_POWER_ON);
	nwy_thread_sleep(5 * 1000);
	char APP_BUILD_TIME[65]= {0};
    char version[70]={0};

    sprintf(version,"\"%s\"", APP_VERSION);
    sprintf(APP_BUILD_TIME,"\"%s,%s\"", __DATE__,__TIME__);

	nwy_ext_echo("\r\nstart run");
    nwy_dm_app_version_set(version,strlen(version));
	nwy_ext_echo("\r\n creat thread is %d",(nwy_thread_create(&g_app_thread,"mythread",NWY_OSI_PRIORITY_NORMAL, prvThreadEntry, NULL,8 , 1024 * 15, NULL)));
// 
	return 0;
}
void appimg_exit(void)
{
	// OSI_LOGI(0, "application image exit");
}

