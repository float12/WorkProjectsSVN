//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述
// 修改记录
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
DWORD ThreadRunCnt[eMaxThreadNum] = {0};
DWORD LastThreadRunCnt[eMaxThreadNum] = {0};

nwy_osiTimer_t *ftp_timer = NULL;
nwy_osiTimer_t *meter_upgrade_timer = NULL;//表升级全过程最大时间定时器
nwy_osiTimer_t *general_timer = NULL;
nwy_osiTimer_t *uart_timer = NULL;
nwy_osiTimer_t *Ble_Recv_ByteTimeout_timer = NULL;//蓝牙帧间超时定时器
nwy_osiTimer_t *Uart_Recv_timer = NULL;//uart帧间超时定时器
nwy_osiTimer_t *TF_Requset_timer = NULL;//请示帧 失败重发定时器
#if (TCP_COMM_MODULE == YES)
nwy_osiTimer_t *Tcp_ComModule_timer = NULL;
#endif
nwy_osiThread_t *g_app_thread = NULL;
nwy_osiThread_t *Timer_thread = NULL;
nwy_osiThread_t *Date_call_thread = NULL;
nwy_osiThread_t *tcp_thread = NULL;
nwy_osiThread_t *tcp_user_thread = NULL;
nwy_osiThread_t *mqtt_thread = NULL;
nwy_osiThread_t *uart_thread = NULL;
nwy_osiThread_t *FtpFotaUpdate_thread = NULL;
nwy_osiThread_t *ftp_thread = NULL;
nwy_osiThread_t *factorypara_thread = NULL;
nwy_osiThread_t *location_thread = NULL;
nwy_osiThread_t *Monitor_Thread = NULL;
nwy_osiThread_t *TF_thread = NULL;
nwy_osiMessageQueue_t *MqttResMessageQueue = NULL;
nwy_osiMessageQueue_t *FtpUpdateMessageQueue = NULL;
nwy_osiMessageQueue_t *CollectMessageQueue = NULL;
nwy_osiMessageQueue_t *WaveDataToTFMessageQueue = NULL;
nwy_osiMessageQueue_t *WaveDataUploadMessageQueue = NULL;
nwy_osiMessageQueue_t *TcpUserServertoTFMsgQue = NULL; 
nwy_osiMessageQueue_t *TimerMessageQueue = NULL;
nwy_osiMessageQueue_t *TranDataToTcpUserServerMsgQue = NULL;
nwy_osiMessageQueue_t *TranDataToTcpPrivateServerMsgQue = NULL;
nwy_osiMessageQueue_t *TranDataToUartMessageQueue = NULL;
nwy_osiMessageQueue_t *TranDataToFactoryMessageQueue = NULL;
nwy_osiMessageQueue_t *UpgradeResultMessageQueue = NULL;
nwy_osiMessageQueue_t *RecvTcpUserServerMsgQue = NULL;
nwy_osiMessageQueue_t *TCPResendTmpMsgQue = NULL;
#if (TCP_COMM_MODULE == YES)
nwy_osiMessageQueue_t *TcpDataToModuleMsgQue = NULL;
nwy_osiMessageQueue_t *ModuleDataToTcpMsgQue = NULL;
#endif
#if (EVENT_REPORT == YES)
nwy_osiMessageQueue_t *EventReportMessageQueue = NULL;
#endif
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
void Monitor_task(void *param);
void Timer_Task(void *param);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
#if(SYS_MONITOR == YES)
//--------------------------------------------------
//功能描述:  监控线程运行计数
//
//参数:
//
//返回值:
//
//备注:  
//--------------------------------------------------
void MonitorThreadRunCnt(void)
{
	WORD i = 0;

	nwy_ext_echo("\r\n eMaxThreadNum: %d", eMaxThreadNum);
	for(i = 0; i < eMaxThreadNum; i++)
	{
		nwy_ext_echo("\r\n thread %d run cnt: %d", i, ThreadRunCnt[i]);
		if(LastThreadRunCnt[i] == ThreadRunCnt[i])
		{
			nwy_ext_echo(" err ", i);
		}
		LastThreadRunCnt[i] = ThreadRunCnt[i];
	}
}
void MonitorStackSize()
{
	nwy_thread_info_t ThreadInfo;

	memset((BYTE *)&ThreadInfo, 0, sizeof(nwy_thread_info_t));
	nwy_get_thread_info(uart_thread, &ThreadInfo);
	nwy_ext_echo("\r\n uart_thread allocated stack size:1024*15 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 15 - ThreadInfo.thread_stack_highwatermark * 4);
	
	nwy_get_thread_info(Date_call_thread, &ThreadInfo);
	nwy_ext_echo("\r\n Date_call_thread allocated stack size:1024*4 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 4 - ThreadInfo.thread_stack_highwatermark * 4);

	nwy_get_thread_info(tcp_thread, &ThreadInfo);
	nwy_ext_echo("\r\n tcp_thread allocated stack size:10240 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 10240 - ThreadInfo.thread_stack_highwatermark * 4);

	nwy_get_thread_info(mqtt_thread, &ThreadInfo);
	nwy_ext_echo("\r\n mqtt_thread allocated stack size:10240 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 10 - ThreadInfo.thread_stack_highwatermark * 4);

	nwy_get_thread_info(ftp_thread, &ThreadInfo);
	nwy_ext_echo("\r\n ftp_thread allocated stack size:1024*13 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 13 - ThreadInfo.thread_stack_highwatermark * 4);

	nwy_get_thread_info(factorypara_thread, &ThreadInfo);
	nwy_ext_echo("\r\n factorypara_thread allocated stack size:1024*13 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 13 - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_get_thread_info(location_thread, &ThreadInfo);
	nwy_ext_echo("\r\n location_thread allocated stack size:1024*5 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 5 - ThreadInfo.thread_stack_highwatermark * 4);

	nwy_get_thread_info(tcp_user_thread, &ThreadInfo);
	nwy_ext_echo("\r\n tcp_user_thread allocated stack size:1024 * 10, , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 10 - ThreadInfo.thread_stack_highwatermark * 4);

	nwy_get_thread_info(TF_thread, &ThreadInfo);
	nwy_ext_echo("\r\n TF_thread allocated stack size:1024 * 10, , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 10 - ThreadInfo.thread_stack_highwatermark * 4);
}

void Monitor_task(void *param)
{
	nwy_sleep(30000);
	while (1)
	{
		MonitorStackSize();
		MonitorThreadRunCnt();
		nwy_sleep(20000);//每20s检查运行计数和栈大小
	}
}
#endif
void Timer_Task(void *param)
{
	nwy_osiEvent_t event;

	while (1)
	{
		nwy_wait_thead_event(Timer_thread, &event, 10);
	}
}
void nwy_uart_timer_cb(void *type)
{
	ReadModuleVersion = 0;
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
//功能描述:  初始化外部相关的管脚
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void InitExternalPins(void)
{
	nwy_gpio_set_direction(SD_POWER_ON, nwy_output);
	nwy_gpio_set_direction(DIRECT_CON_485, nwy_output);
	nwy_gpio_set_direction(RUN_LIGHT, nwy_output);
	// nwy_gpio_set_direction(LOCAL_LIGHT, nwy_output);
	nwy_gpio_set_direction(ONLINE, nwy_output);
	
	nwy_gpio_set_value(SD_POWER_ON, 1);
	nwy_gpio_set_value(RUN_LIGHT, 0);
	// nwy_gpio_set_value(LOCAL_LIGHT, 0);
	nwy_gpio_set_value(ONLINE, 0);

	api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
	api_ClrSysStatus(eSYS_STASUS_START_TRAN);
	api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
	api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
	api_ReadSystemFiletoRam(eBIT_TCPNET);
	api_ReadSystemFiletoRam(eBIT_MQTTNET);
	api_PowerOnCreatSyseventTable();
}
static void prvThreadEntry(void *param)
{

	// 创建使用到的消息队列
	MqttResMessageQueue = nwy_create_msg_Que(20, sizeof(char *));
	FtpUpdateMessageQueue = nwy_create_msg_Que(20, sizeof(FtpUpdatePara));
	CollectMessageQueue = nwy_create_msg_Que(20, sizeof(InstantData698Frame));
	WaveDataUploadMessageQueue = nwy_create_msg_Que(5, sizeof(WaveDatatoTcp));
	WaveDataToTFMessageQueue = nwy_create_msg_Que(WAVE_UART_QUEUE_SPACE, WAVE_DATA_FRAME_SIZE);
	TcpUserServertoTFMsgQue = nwy_create_msg_Que(1, sizeof(BYTE));
	TimerMessageQueue = nwy_create_msg_Que(20, sizeof(TFlagBytes));
	TranDataToTcpUserServerMsgQue = nwy_create_msg_Que(5, sizeof(tTranData));
	TranDataToTcpPrivateServerMsgQue = nwy_create_msg_Que(5, sizeof(tTranData));
	TranDataToUartMessageQueue = nwy_create_msg_Que(5, sizeof(tTranData));
	TranDataToFactoryMessageQueue = nwy_create_msg_Que(5, sizeof(tTranData));
	UpgradeResultMessageQueue = nwy_create_msg_Que(10, sizeof(BYTE));
    RecvTcpUserServerMsgQue = nwy_create_msg_Que(5, sizeof(BYTE));
	TCPResendTmpMsgQue = nwy_create_msg_Que(TCP_RESEND_QUEUE_SIZE, SEND_WAVE_DATA_LEN);//暂存3*8个周波数据
	#if (TCP_COMM_MODULE == YES)
	TcpDataToModuleMsgQue = nwy_create_msg_Que(5, sizeof(tTranData));
	ModuleDataToTcpMsgQue = nwy_create_msg_Que(5, sizeof(tTranData));
	#endif
	#if (EVENT_REPORT == YES)
	EventReportMessageQueue = nwy_create_msg_Que(4, sizeof(Eventmessage));
	#endif
	nwy_sleep(8000);
	// 创建使用的任务  注：使用事件的任务需要完成nwy_timer_init
	// DataCall_Task
	int profileID = 1;

	Timer_thread = nwy_create_thread("Timer", Timer_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 5, 16);
	general_timer = nwy_timer_init(Timer_thread, nwy_general_timer_cb, NULL);
	Uart_Recv_timer = nwy_timer_init(Timer_thread, Uart_Recv_timer_cb, NULL);
	uart_timer = nwy_timer_init(Timer_thread, nwy_uart_timer_cb, NULL);
	TF_Requset_timer = nwy_timer_init(Timer_thread, TF_Request_timer_cb, NULL);
	ftp_timer = nwy_timer_init(Timer_thread, nwy_ftp_timer_cb, NULL);
	meter_upgrade_timer = nwy_timer_init(Timer_thread, nwy_meter_upgrade_timer_cb, NULL);
	#if(BLE_UART_WH == YES)
	Ble_Recv_ByteTimeout_timer = nwy_timer_init(Timer_thread, nwy_Ble_Recv_timer_cb, NULL);
	#endif
	#if (TCP_COMM_MODULE == YES)
	Tcp_ComModule_timer = nwy_timer_init(Timer_thread, Tcp_Commodule_timer_cb, NULL);
	#endif
	#if (TCP_SWITCH == YES)
	tcp_user_thread = nwy_create_thread("TcpToUser", TCP_User_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8);
	#endif
	tcp_thread = nwy_create_thread("TcpToUart", TCP_PrivateTask, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8);
	TF_thread = nwy_create_thread("TFTask", TF_Task, NULL, NWY_OSI_PRIORITY_ABOVE_NORMAL, 1024 * 10, 8);
	Date_call_thread = nwy_create_thread("DateCall", DateCall_Task, (void *)&profileID, NWY_OSI_PRIORITY_NORMAL, 1024 * 4, 8);
	// MQTT_Task
	mqtt_thread = nwy_create_thread("MqttTask", WSD_MQTT_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8);
	// Uart_Task
	uart_thread = nwy_create_thread("UartTask", Uart_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 15, 8);
	// Uart_Task
	ftp_thread = nwy_create_thread("FtpTask", Fota_UpdateTask, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 13, 8);
	// Factory_Task
	factorypara_thread = nwy_create_thread("FactoryTask", Factory_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 13, 8);
	#if (GPRS_POSITION == YES)
	location_thread = nwy_create_thread("LocationTask", Location_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 5, 8);
	#endif
	#if (SYS_MONITOR == YES)
	Monitor_Thread = nwy_create_thread("MonitorTask", Monitor_task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8);
	#endif
	nwy_exit_thread();
}
int appimg_enter(void *param)
{
	// warning:app build time length <= 64

	char APP_BUILD_TIME[65] = {0};
	char version[70] = {0};

	sprintf(version, "\"%s\"", APP_VERSION);
	sprintf(APP_BUILD_TIME, "\"%s,%s\"", __DATE__, __TIME__);
	OSI_LOGI(0, "appimg_enter ...");
	if (false == nwy_app_version(version, APP_BUILD_TIME))
	{
		OSI_LOGI(0, "app set version fail");
		return 0;
	}
	InitExternalPins();
	g_app_thread = nwy_create_thread("mythread", prvThreadEntry, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 15, 16);

	return 0;
}
void appimg_exit(void)
{
	OSI_LOGI(0, "application image exit");
}
