//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部 
//创建人	王泉
//创建日期	
//描述		FUNC_DEF头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __FUNC_DEF_H
#define __FUNC_DEF_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAX_SAMPLE_CHIP 						1				//兼容4回路 但基表末尾地址必须为1！！！
#define MAX_RATIO								6				//费率数要与表一致，不一致则读电能有问题 总+ratio5
#define FILENAME_LEM							128
#define READMETER_PROTOCOL						PROTOCOL_645	//下行抄表规约
#define BLE_UART_WH								YES				//蓝牙和uart作为维护端口 此处区分硬件 ZH 的周期上报的开关状态还未添加
#define GPRS_POSITION							NO				//GPRS 开关
#define CYCLE_REPORT							NO				//周期上报开关	周期上报包含事件上报
#define	EVENT_REPORT							NO				//事件上报开关
#define	TRANS_SWITCH							TCP_TRANS		//透传协议
#define PT_CT									NO				//周期上报带变比
#define	TEST_STACK								NO				//堆栈分析
#define	TCP_SWITCH								YES				//tcp开关
#define FLOW_CALCULATE 							YES				//流量计算
#define COLLECT_UIP 							YES				//录入瞬时
#define UPLOAD_WAVE_DATA_SWITCH 				NO				//上传波形数据
// IO管脚控制
#define FLASH_PIN 								3				//FLASH 不用 管脚置低
#define REMOTE_LIGHT							9				//上线绿灯默认关闭，上线后打开
#define	RUN_LIGHT								28				//运行灯  运行时1HZ闪烁
#define	LOCAL_LIGHT								27				//本地灯  上电抄表成功常亮

#define SERVER_TRAN								0				//主站透传
#define	BLUETOOTH_TRAN							1				//蓝牙透传
#define BASEMETER_UPGRADES						0x5AA55AA5
#define	DOWNLOAD_METERFILE						0xA55AA55A
#define UART_BUFF_MAX_LEN 						(4096+30)
#define POWERON_READRPARA_NUM					eBIT_POSITION

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct
{
	BYTE Messagetype;
	BYTE *Buf;
} UartSendMessage;

#pragma pack(1)
typedef struct
{
	char filename[128];			//文件名称
	char filepath[128];			//下载文件后的路径 下载后路径固定/nwy/文件名
	char updateMode;			//1 全包升级 0 差分升级	    
	char ftpupdateStep;			//必须写1 从登录开始
	char ftp_connect_flag;		//主站升级命令标志 1开始升级 0禁止升级	
} FtpUpdatePara_t;
#pragma pack()

typedef struct
{
	nwy_ftp_login_t ftp_param;
	char filename[FILENAME_LEM];
} FtpUpdatePara;

typedef struct tTranData
{
	BYTE buf[4096 + 30];
	DWORD len; /* data */
	BYTE TranType;
} tTranData;

// 分钟、秒、时抄表bit位枚举
typedef enum
{
	eBIT_ADDR = 0,
	eBIT_TIME,
	eBIT_TCPNET,
	eBIT_MQTTNET,
	eBIT_TCPUSER,
	eBIT_MQTTUSER,
	eBIT_VERSION,
#if (PT_CT == YES)
	eBIT_CT1,
	eBIT_CT2,
	eBIT_PT1,
	eBIT_PT2,
#endif
	eBIT_POSITION,
#if (EVENT_REPORT == YES)
	eBIT_EVENT,
#endif
	eBIT_TOTAL,
} eREAD_METER_BIT;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern nwy_osiMessageQueue_t *MqttResMessageQueue;
extern nwy_osiMessageQueue_t *FtpUpdateMessageQueue;
extern nwy_osiMessageQueue_t *CollectMessageQueue;
extern nwy_osiMessageQueue_t *WaveDataUploadMessageQueue;
extern nwy_osiMessageQueue_t *TCP_RESULT_TO_TF_MessageQueue;
extern nwy_osiMessageQueue_t *WaveDataToTFMessageQueue;
extern nwy_osiMessageQueue_t *TimerMessageQueue;
extern nwy_osiMessageQueue_t *TranDataToServerMessageQueue;
extern nwy_osiMessageQueue_t *TranDataToUartMessageQueue;
extern nwy_osiMessageQueue_t *TranDataToFactoryMessageQueue;
extern nwy_osiMessageQueue_t *UpgradeResultMessageQueue;
#if (EVENT_REPORT == YES)
extern nwy_osiMessageQueue_t *EventReportMessageQueue;
#endif
extern nwy_osiThread_t *FtpFotaUpdate_thread;
extern nwy_osiThread_t *location_thread;
extern nwy_sim_result_type imei;
extern nwy_osiThread_t *g_app_thread;
extern nwy_osiThread_t *ftp_thread;
extern nwy_osiThread_t *uart_thread;
extern nwy_osiThread_t *factorypara_thread;
extern nwy_osiTimer_t *g_timer;
extern nwy_osiTimer_t *general_timer;
extern nwy_osiTimer_t *meter_upgrade_timer;
extern nwy_osiTimer_t *ftp_timer;
extern nwy_osiTimer_t *uart_timer;
extern nwy_osiTimer_t *Ble_Recv_ByteTimeout_timer;
extern nwy_osiTimer_t *Uart_Recv_timer;
extern nwy_osiTimer_t *TF_REQUEST_TIMER;
extern char PrintLogSign;
extern int tcp_connect_flag;
extern int BleLogSwitch ;//蓝牙打印log开关
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
int nwy_ext_check_data_connect();
void WSD_MQTT_Task(void *param);
void DateCall_Task(void *profileID);
void Fota_UpdateTask(void *parameter);
void Factory_Task(void *param);
void MeterUpgrade645_Task(void);
void MeterUpgrade698_Task(void);
void lib_ExchangeData(BYTE *BufA, BYTE *BufB, BYTE Len);
void nwy_ext_send_sig(nwy_osiThread_t *thread, uint16 sig);
void nwy_ftp_timer_cb(void *type);
void nwy_meter_upgrade_timer_cb(void *type);
void nwy_Ble_Recv_timer_cb(void *type);
void nwy_general_timer_cb(void *type);
void Uart_Recv_timer_cb(void *type);
void TF_Request_timer_cb(void *type);
void nwy_ftp_timer_cb(void *type);
void nwy_meter_upgrade_timer_cb(void *type);
void api_ReadSystemFiletoRam(eREAD_METER_BIT eBit);
void Location_Task(void *param);
BOOL get_gps_position_info(char *c_data, DWORD *addr);
void TCP_Task(void *param);
void TF_Task(void *param);
void Tcp_send(int Isocket, char *sendbuff, int sendlen);
//--------------------------------------------------
//功能描述:
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
BYTE SendBleDataInPiece(char *data, WORD total_len);
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL api_GetMqttConStatus(void);
//--------------------------------------------------
//功能描述:  获取当前模块升级状态
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL api_GetModuleUpGradeStatus(void);
#endif //#ifndef __FUNC_DEF_H
