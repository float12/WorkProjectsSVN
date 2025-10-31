//----------------------------------------------------------------------
// Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾��ѹ̨����Ʒ��
// ������	��Ȫ
// ��������
// ����
// �޸ļ�¼
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
nwy_osiTimer_t *ftp_timer = NULL;
nwy_osiTimer_t *meter_upgrade_timer = NULL; //������ȫ�������ʱ�䶨ʱ��?
nwy_osiTimer_t *general_timer = NULL;
nwy_osiTimer_t *g_timer = NULL;
nwy_osiTimer_t *uart_timer = NULL;
nwy_osiTimer_t *Ble_Recv_ByteTimeout_timer = NULL; //����֡�䳬ʱ��ʱ��
nwy_osiTimer_t *Uart_Recv_timer = NULL;			   //uart֡�䳬ʱ��ʱ��
nwy_osiTimer_t *TF_REQUEST_TIMER = NULL;//�����ϴ���ʱ��
nwy_osiThread_t *g_app_thread = NULL;
nwy_osiThread_t *Timer_thread = NULL;
nwy_osiThread_t *Date_call_thread = NULL;
nwy_osiThread_t *tcp_thread = NULL;
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
nwy_osiMessageQueue_t *WaveDataUploadMessageQueue = NULL; //��������1 ����һ��ֻ����4096�ֽڣ�7K�ֽڷ����Σ����Է���������
nwy_osiMessageQueue_t *TCP_RESULT_TO_TF_MessageQueue = NULL; //tcp�ϴ�������ݸ�TF�߳�
nwy_osiMessageQueue_t *TimerMessageQueue = NULL;
nwy_osiMessageQueue_t *TranDataToServerMessageQueue = NULL;
nwy_osiMessageQueue_t *TranDataToUartMessageQueue = NULL;
nwy_osiMessageQueue_t *TranDataToFactoryMessageQueue = NULL;
nwy_osiMessageQueue_t *UpgradeResultMessageQueue = NULL;
#if (EVENT_REPORT == YES)
nwy_osiMessageQueue_t *EventReportMessageQueue = NULL;
#endif
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
void Monitor_task(void *param);
void Timer_Task(void *param);
//-----------------------------------------------
//				��������
//-----------------------------------------------
#if (TEST_STACK == YES)
void MonitorStackSize()
{
	nwy_thread_info_t ThreadInfo;

	memset((BYTE *)&ThreadInfo, 0, sizeof(nwy_thread_info_t));
	nwy_get_thread_info(uart_thread, &ThreadInfo);
	nwy_ext_echo("\r\n uart_thread allocated stack size:1024*15 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 1024 * 15 - ThreadInfo.thread_stack_highwatermark * 4);
	nwy_get_thread_info(Monitor_Thread, &ThreadInfo);
	nwy_ext_echo("\r\n Monitor_Thread allocated stack size:10240 , minimum:%d,  maximum use:%d", ThreadInfo.thread_stack_highwatermark * 4, 10240 - ThreadInfo.thread_stack_highwatermark * 4);
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
}

void Monitor_task(void *param)
{
	while (1)
	{
		MonitorStackSize();
		nwy_sleep(50000);
	}
}
#endif
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
//��������:  ��ʼ���ⲿ��صĹܽ�?
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void InitExternalPins(void)
{
	nwy_gpio_set_direction(REMOTE_LIGHT, nwy_output);
	nwy_gpio_set_direction(FLASH_PIN, nwy_output);
	nwy_gpio_set_direction(RUN_LIGHT, nwy_output);
	nwy_gpio_set_direction(LOCAL_LIGHT, nwy_output);
	nwy_gpio_set_value(REMOTE_LIGHT, 0);
	nwy_gpio_set_value(FLASH_PIN, 0);
	nwy_gpio_set_value(RUN_LIGHT, 0);
	nwy_gpio_set_value(LOCAL_LIGHT, 0);
	api_ClrSysStatus(eSYS_STASUS_TIMER_UP); //Ŀǰ�ȷ��Ű�
	api_ClrSysStatus(eSYS_STASUS_START_TRAN);
	api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
	api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
}
static void prvThreadEntry(void *param)
{

	// ����ʹ�õ�����Ϣ����
	MqttResMessageQueue = nwy_create_msg_Que(20, sizeof(char *));
	FtpUpdateMessageQueue = nwy_create_msg_Que(20, sizeof(FtpUpdatePara));
	CollectMessageQueue = nwy_create_msg_Que(20, sizeof(InstantData698Frame));
	WaveDataUploadMessageQueue = nwy_create_msg_Que(5, sizeof(WaveOrRequestData));
	WaveDataToTFMessageQueue = nwy_create_msg_Que(10, sizeof(WaveOrRequestData));
	TCP_RESULT_TO_TF_MessageQueue = nwy_create_msg_Que(1, sizeof(BYTE));
	TimerMessageQueue = nwy_create_msg_Que(20, sizeof(TFlagBytes));
	TranDataToServerMessageQueue = nwy_create_msg_Que(5, sizeof(tTranData));
	TranDataToUartMessageQueue = nwy_create_msg_Que(5, sizeof(tTranData));
	TranDataToFactoryMessageQueue = nwy_create_msg_Que(5, sizeof(tTranData));
	UpgradeResultMessageQueue = nwy_create_msg_Que(10, sizeof(BYTE));
#if (EVENT_REPORT == YES)
	EventReportMessageQueue = nwy_create_msg_Que(4, sizeof(Eventmessage));
#endif
	nwy_sleep(8000);
	// ����ʹ�õ�����  ע��ʹ���¼���������Ҫ���nwy_timer_init
	// DataCall_Task
	int profileID = 1;
	//��ʱ�� �߳�
	Timer_thread = nwy_create_thread("Timer", Timer_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 5, 16);
	general_timer = nwy_timer_init(Timer_thread, nwy_general_timer_cb, NULL);
	Uart_Recv_timer = nwy_timer_init(Timer_thread, Uart_Recv_timer_cb, NULL);
	TF_REQUEST_TIMER = nwy_timer_init(Timer_thread, TF_Request_timer_cb, NULL);
	uart_timer = nwy_timer_init(Timer_thread, nwy_uart_timer_cb, NULL);
	ftp_timer = nwy_timer_init(Timer_thread, nwy_ftp_timer_cb, NULL);
	meter_upgrade_timer = nwy_timer_init(Timer_thread, nwy_meter_upgrade_timer_cb, NULL);
	Ble_Recv_ByteTimeout_timer = nwy_timer_init(Timer_thread, nwy_Ble_Recv_timer_cb, NULL);
//��ʱ�� ���ж�ʱ������һ���߳�
#if (TCP_SWITCH == YES)
	tcp_thread = nwy_create_thread("TcpToUart", TCP_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8); // ��͸������
#endif
	// TF_thread = nwy_create_thread("TFTask", TF_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8);
	Date_call_thread = nwy_create_thread("DateCall", DateCall_Task, (void *)&profileID, NWY_OSI_PRIORITY_NORMAL, 1024 * 4, 8);
	//MQTT_Task
	mqtt_thread = nwy_create_thread("MqttTask", WSD_MQTT_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8);
	//Uart_Task
	uart_thread = nwy_create_thread("UartTask", Uart_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 15, 8);
	//Uart_Task
	ftp_thread = nwy_create_thread("FtpTask", Fota_UpdateTask, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 13, 8);
	//Factory_Task
	// factorypara_thread = nwy_create_thread("FactoryTask", Factory_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 13, 8); //����͸��������Ҫ����ջ��
#if (GPRS_POSITION == YES)
	location_thread = nwy_create_thread("LocationTask", Location_Task, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 5, 8);
#endif
#if (TEST_STACK == YES)
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
