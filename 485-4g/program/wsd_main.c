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
//线程 使用堆栈栈空间
#define UART_STACK					(1024 * 15)
#define TCP_USER_STACK				(1024 * 10)
#define TCP_PRIVATE_STACK			(1024 * 10)	
#define MQTT_USER_STACK				(1024 * 10)
#define MQTT_PRIVATE_STACK			(1024 * 10)
#define FACTORY_STACK				(1024 * 13)
#define FTP_STACK					(1024 * 13)

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
nwy_osiTimer_t * User_MQTT_Reconnect_timer = NULL;
nwy_osiTimer_t *general_timer = NULL;
nwy_osiTimer_t *g_timer = NULL;
nwy_osiTimer_t *uart_timer = NULL;
nwy_osiTimer_t *uart_send_complte_timer = NULL;
nwy_osiTimer_t *Ble_Recv_ByteTimeout_timer = NULL;//蓝牙帧间超时定时器
nwy_osiTimer_t *Uart_Recv_timer = NULL;//uart帧间超时定时器

#if (TCP_COMM_MODULE == YES)
nwy_osiTimer_t *Tcp_ComModule_timer = NULL;
#endif

nwy_osiThread_t *g_app_thread = NULL;
nwy_osiThread_t *Timer_thread = NULL;
nwy_osiThread_t *Date_call_thread = NULL;
nwy_osiThread_t *tcp_thread[TCP_TOTAL_NUM] = {NULL};
nwy_osiThread_t *mqtt_thread = NULL;
nwy_osiThread_t *userserver_mqtt_thread = NULL;
nwy_osiThread_t *uart_thread = NULL;
nwy_osiThread_t *FtpFotaUpdate_thread = NULL;
nwy_osiThread_t *ftp_thread = NULL;
nwy_osiThread_t *factorypara_thread = NULL;
nwy_osiThread_t *location_thread = NULL;
nwy_osiThread_t *Monitor_Thread = NULL;
nwy_osiMessageQueue_t *MqttResMessageQueue = NULL;
nwy_osiMessageQueue_t *MqttTransMessageQueue = NULL;	//mqtt 专用透传队列
nwy_osiMessageQueue_t *MqttResUserMessageQueue = NULL;
nwy_osiMessageQueue_t *FtpUpdateMessageQueue = NULL;
nwy_osiMessageQueue_t *CollectMessageQueue = NULL;
nwy_osiMessageQueue_t *TimerMessageQueue = NULL;
nwy_osiMessageQueue_t *UartDataToTcpMsgQue = NULL;
nwy_osiMessageQueue_t *TranDataToMqttServerMsgQue = NULL;
nwy_osiMessageQueue_t *TranDataToUartMessageQueue = NULL;
nwy_osiMessageQueue_t *TranDataToFactoryMessageQueue = NULL;
nwy_osiMessageQueue_t *UpgradeResultMessageQueue = NULL;
#if (TCP_COMM_MODULE == YES)
nwy_osiMessageQueue_t *TcpDataToModuleMsgQue = NULL;
nwy_osiMessageQueue_t *ModuleDataToTcpMsgQue = NULL;
#endif
#if(EVENT_REPORT == YES)
nwy_osiMessageQueue_t *EventReportMessageQueue = NULL;
#endif
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static BYTE	 PowerDownFlag = 0;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
void Monitor_task(void *param);
void Timer_Task(void *param);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  检查开机原因
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  CheckStrartReason( void )
{
	uint32_t causes = nwy_get_boot_causes();
	char string_buf[256];

	memset(string_buf, 0, sizeof(string_buf));
	int idx = sprintf(string_buf, "boot cause[%02lx]:", causes);
	if(causes == NWY_BOOTCAUSE_UNKNOWN)
		idx += sprintf(string_buf + idx, " %s", "UNKNOWN");
	else
	{
		if(causes & NWY_BOOTCAUSE_PWRKEY)
			idx += sprintf(string_buf + idx, " %s", "PWRKEY");
		if(causes & NWY_BOOTCAUSE_PIN_RESET)
			idx += sprintf(string_buf + idx, " %s", "PIN_RESET");
		if(causes & NWY_BOOTCAUSE_ALARM)
			idx += sprintf(string_buf + idx, " %s", "ALARM");
		if(causes & NWY_BOOTCAUSE_CHARGE)
			idx += sprintf(string_buf + idx, " %s", "CHARGE");
		if(causes & NWY_BOOTCAUSE_WDG)
			api_WriteSysUNMsg(WDG_RESET);
		if(causes & NWY_BOOTCAUSE_PIN_WAKEUP)
			idx += sprintf(string_buf + idx, " %s", "PIN_WAKEUP");
		if(causes & NWY_BOOTCAUSE_PSM_WAKEUP)
			idx += sprintf(string_buf + idx, " %s", "PSM_WAKEUP");
	}
	nwy_ext_echo("%s\r\n", string_buf);
}
//--------------------------------------------------
//功能描述:  检查相关编译开关正确性
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  CheckSysLimit( void )
{
	// #if(MQTT_USER == YES)
	// #if(CYCLE_REPORT_PROTOCAL != PROTOCOL_MQTT)
	// for(;;);
	// #endif //#if(CYCLE_REPORT_PROTOCAL != PROTOCOL_MQTT)
	// #endif
}
//--------------------------------------------------
//功能描述:  控灯 和 掉电检测
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  CheckSysVolandControlLight( void )
{
	if (nwy_adc_get(NWY_ADC_CHANNEL3, NWY_ADC_SCALE_5V000) < POWER_DOWN_VOLTAGE)
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
#if(TEST_STACK == YES)
void MonitorStackSize()
{
	nwy_thread_info_t ThreadInfo;

	memset((BYTE*)&ThreadInfo,0,sizeof(nwy_thread_info_t));
	nwy_get_thread_info(uart_thread, &ThreadInfo);
	nwy_ext_echo("\r\n uart_thread allocated stack size:%d , minimum:%d,  maximum use:%d",UART_STACK, ThreadInfo.thread_stack_highwatermark * 4, UART_STACK - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_get_thread_info(Monitor_Thread, &ThreadInfo);
	nwy_ext_echo("\r\n Monitor_Thread allocated stack size:%d , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 10240 - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_get_thread_info(Date_call_thread, &ThreadInfo);
	nwy_ext_echo("\r\n Date_call_thread allocated stack size:%d , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 4 - ThreadInfo.thread_stack_highwatermark * 4);

	nwy_get_thread_info(tcp_thread[0], &ThreadInfo);
	nwy_ext_echo("\r\n tcp_private_thread allocated stack size:%d, minimum:%d,  maximum use:%d",TCP_PRIVATE_STACK, ThreadInfo.thread_stack_highwatermark * 4, TCP_PRIVATE_STACK - ThreadInfo.thread_stack_highwatermark * 4);

	#if(TCP_USER == YES)
	nwy_get_thread_info(tcp_thread[MAX_PRIVATE_SERVER_NUM], &ThreadInfo);
	nwy_ext_echo("\r\n tcp_user_thread allocated stack size:%d, minimum:%d,  maximum use:%d",TCP_USER_STACK, ThreadInfo.thread_stack_highwatermark * 4, TCP_USER_STACK - ThreadInfo.thread_stack_highwatermark * 4);
	#endif

	nwy_get_thread_info(mqtt_thread, &ThreadInfo);
	nwy_ext_echo("\r\n mqtt_private_thread allocated stack size:%d , minimum:%d,  maximum use:%d",MQTT_PRIVATE_STACK, ThreadInfo.thread_stack_highwatermark * 4, MQTT_PRIVATE_STACK - ThreadInfo.thread_stack_highwatermark * 4);

	#if(MQTT_USER == YES)
	nwy_get_thread_info(userserver_mqtt_thread, &ThreadInfo);
	nwy_ext_echo("\r\n mqtt_user_thread allocated stack size:%d , minimum:%d,  maximum use:%d",MQTT_USER_STACK, ThreadInfo.thread_stack_highwatermark * 4, MQTT_USER_STACK - ThreadInfo.thread_stack_highwatermark * 4);
	#endif

	nwy_get_thread_info(ftp_thread, &ThreadInfo);
	nwy_ext_echo("\r\n ftp_thread allocated stack size:%d , minimum:%d,  maximum use:%d",FTP_STACK, ThreadInfo.thread_stack_highwatermark * 4, FTP_STACK - ThreadInfo.thread_stack_highwatermark * 4);

	nwy_get_thread_info(factorypara_thread, &ThreadInfo);
	nwy_ext_echo("\r\n factorypara_thread allocated stack size:%d , minimum:%d,  maximum use:%d",FACTORY_STACK, ThreadInfo.thread_stack_highwatermark * 4, FACTORY_STACK - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_get_thread_info(location_thread, &ThreadInfo);
	nwy_ext_echo("\r\n location_thread allocated stack size:%d , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 5 - ThreadInfo.thread_stack_highwatermark * 4);
}
#endif



void Monitor_task(void *param)
{
    while (1)
    {
        #if(TEST_STACK == YES)
		MonitorStackSize();
		#endif
		// CheckSysVolandControlLight();
        nwy_sleep(5000);
    }
}

void Timer_Task(void *param)
{
	nwy_osiEvent_t event;

	while (1)
	{
		nwy_wait_thead_event(Timer_thread, &event, 100);
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
//功能描述:  初始化外部相关的管脚 和 置一些标志
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  InitExternalPinsandFlags( void )
{
	// nwy_gpio_set_direction(SD_POWER_ON, nwy_output);
	nwy_gpio_set_direction(DIRECT_CON_485, nwy_output);
	nwy_gpio_set_direction(RUN_LIGHT, nwy_output);
	// nwy_gpio_set_direction(LOCAL_LIGHT, nwy_output);
	nwy_gpio_set_value(RUN_LIGHT, 0);
	nwy_gpio_set_direction(COMM_485_LED, nwy_output);
	nwy_gpio_set_value(COMM_485_LED, 0);
	nwy_gpio_set_direction(REMOTE_LIGHT, nwy_output);
	nwy_gpio_set_value(REMOTE_LIGHT, 0);
	// nwy_gpio_set_value(ONLINE, 0);
	
	api_ClrSysStatus(eSYS_STASUS_TIMER_UP);//目前先放着吧
	api_ClrSysStatus(eSYS_STASUS_START_TRAN);
	api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
	api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
	api_PowerOnCreatParaTable();
	api_ReadSystemFiletoRam(eBIT_TCPNET);
	api_ReadSystemFiletoRam(eBIT_MQTTNET);
	api_OperateFileSystem(READ, GET_SAFE_SPACE_ADDR(MeterBaud), (BYTE *)&MeterBaud, sizeof(MeterBaud));
	api_PowerOnCreatSyseventTable();
}
static void prvThreadEntry(void *param)
{
	// 创建使用到的消息队列
	MqttResMessageQueue= nwy_create_msg_Que(20, sizeof(char*));
	MqttTransMessageQueue = nwy_create_msg_Que(20, sizeof(char*));
	MqttResUserMessageQueue= nwy_create_msg_Que(20, sizeof(char*));
	FtpUpdateMessageQueue = nwy_create_msg_Que(20, sizeof(FtpUpdatePara));
	CollectMessageQueue = nwy_create_msg_Que(20,sizeof(CollectionDatas));
	TimerMessageQueue = nwy_create_msg_Que(20,sizeof(TFlagBytes));
	UartDataToTcpMsgQue = nwy_create_msg_Que(5,sizeof(tTranData));
	TranDataToMqttServerMsgQue = nwy_create_msg_Que(5,sizeof(tTranData));
	TranDataToUartMessageQueue = nwy_create_msg_Que(5,sizeof(tTranData));
	TranDataToFactoryMessageQueue = nwy_create_msg_Que(5, sizeof(tTranData));
	UpgradeResultMessageQueue = nwy_create_msg_Que(10, sizeof(BYTE));
	#if (TCP_COMM_MODULE == YES)
	TcpDataToModuleMsgQue = nwy_create_msg_Que(5, sizeof(tTranData));
	ModuleDataToTcpMsgQue = nwy_create_msg_Que(5, sizeof(tTranData));
	#endif
	
	#if(EVENT_REPORT == YES)
	EventReportMessageQueue = nwy_create_msg_Que(4, sizeof(Eventmessage));
	#endif
	nwy_sleep(5000);
	// 创建使用的任务  注：使用事件的任务需要完成nwy_timer_init
	// DataCall_Task
	int profileID=1;
	int ServerNum;
	//定时器 线程 
	Timer_thread = nwy_create_thread("Timer", Timer_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 5, 16);
	general_timer = nwy_timer_init(Timer_thread, nwy_general_timer_cb, NULL);
	Uart_Recv_timer = nwy_timer_init(Timer_thread, Uart_Recv_timer_cb, NULL);
	uart_timer = nwy_timer_init(Timer_thread, nwy_uart_timer_cb, NULL);
	uart_send_complte_timer = nwy_timer_init(Timer_thread, uart_send_complet_timer_cb, NULL);
	ftp_timer = nwy_timer_init(Timer_thread, nwy_ftp_timer_cb, NULL);
	meter_upgrade_timer = nwy_timer_init(Timer_thread, nwy_meter_upgrade_timer_cb,NULL);
	
	CheckStrartReason();
	#if(MQTT_USER == YES)
	User_MQTT_Reconnect_timer = nwy_timer_init(Timer_thread, nwy_User_MQTT_Reconnect_timer_cb,NULL);
	#endif
	#if(BLE_WH== YES)
	Ble_Recv_ByteTimeout_timer = nwy_timer_init(Timer_thread, nwy_Ble_Recv_timer_cb, NULL);
	#endif
	#if (TCP_COMM_MODULE == YES)
	Tcp_ComModule_timer = nwy_timer_init(Timer_thread, Tcp_Commodule_timer_cb, NULL);
	#endif

	//定时器 所有定时器放下一个线程 
	for ( ServerNum = 0; ServerNum < MAX_PRIVATE_SERVER_NUM; ServerNum++)
	{
		tcp_thread[ServerNum] = nwy_create_thread("TcpToUart", TCP_PrivateTask,  (void*)&ServerNum, NWY_OSI_PRIORITY_NORMAL,TCP_PRIVATE_STACK, 8);
		nwy_sleep(2000);
	}
	#if(TCP_USER == YES)
	for ( ServerNum = MAX_PRIVATE_SERVER_NUM; ServerNum < (MAX_USER_SERVER_NUM + MAX_PRIVATE_SERVER_NUM); ServerNum++)
	{
		tcp_thread[ServerNum] = nwy_create_thread("TcpToUser", TCP_User_Task,  (void*)&ServerNum, NWY_OSI_PRIORITY_NORMAL, TCP_USER_STACK, 8);
		nwy_sleep(2000);
	}
	#endif
	Date_call_thread = nwy_create_thread("DateCall", DataCall_Task, (void*)&profileID, NWY_OSI_PRIORITY_NORMAL, 1024 * 4, 8);
	//MQTT_Task
	mqtt_thread = nwy_create_thread("MqttTask", WSD_MQTT_Task, NULL, NWY_OSI_PRIORITY_NORMAL, MQTT_PRIVATE_STACK, 8);
	#if(MQTT_USER == YES)
	userserver_mqtt_thread = nwy_create_thread("MqttTask", USER_MQTT_Task, NULL, NWY_OSI_PRIORITY_NORMAL, MQTT_USER_STACK, 8);
	#endif
	//Uart_Task
	uart_thread = nwy_create_thread("UartTask",Uart_Task, NULL,NWY_OSI_PRIORITY_NORMAL, UART_STACK, 8);
	//Uart_Task
	ftp_thread = nwy_create_thread("FtpTask",Fota_UpdateTask,NULL,NWY_OSI_PRIORITY_NORMAL,FTP_STACK,30);
	//Factory_Task
	factorypara_thread = nwy_create_thread("FactoryTask", Factory_Task, NULL, NWY_OSI_PRIORITY_NORMAL, FACTORY_STACK, 8);//增加透传数据需要增大栈区
	#if(GPRS_POSITION == YES)
	location_thread = nwy_create_thread("LocationTask", Location_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 5, 8);
	#endif
	Monitor_Thread = nwy_create_thread("MonitorTask", Monitor_task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8);
	nwy_exit_thread();
}
int appimg_enter(void *param)
{
	//检查编译系统正确性
	CheckSysLimit();
	// warning:app build time length <= 64
	char APP_BUILD_TIME[65] = {0};
	char version[70] = {0};

	sprintf(version, "\"%s\"", APP_VERSION);
	sprintf(APP_BUILD_TIME, "\"%s,%s\"", __DATE__, __TIME__);
	OSI_LOGI(0, "appimg_enter ...");
	if(false == nwy_app_version(version, APP_BUILD_TIME))
	{
		OSI_LOGI(0, "app set version fail");
		return 0;
	}
	InitExternalPinsandFlags();
	g_app_thread = nwy_create_thread("mythread", prvThreadEntry, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 15, 16);

	return 0;
}
void appimg_exit(void)
{
	OSI_LOGI(0, "application image exit");
}

