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
#define BLE_UART_WH								NO				//蓝牙和uart作为维护端口 此处区分硬件 ZH 的周期上报的开关状态还未添加
#define GPRS_POSITION							NO				//GPRS 开关
#define CYCLE_REPORT							NO				//周期上报开关	周期上报包含事件上报
#define	EVENT_REPORT							NO				//事件上报开关
#define	TRANS_SWITCH							TCP_TRANS		//透传协议
#define PT_CT									NO				//周期上报带变比
#define	SYS_MONITOR								NO				//堆栈分析
#define	TCP_SWITCH								YES				//tcp开关
#define TCP_COMM_MODULE							YES				//tcp 与模块通信 

#define FLOW_CALCULATE 							YES				//流量统计
#define COLLECT_UIP 							NO				//采集uip
#define UPLOAD_WAVE_DATA_SWITCH 				YES				//上传波形数据开关
#define LTO_METER_UART							YES
#define EVENT_RECORD							YES				//异常事件记录

// IO管脚控制
#define SD_POWER_ON								9				//上线绿灯默认关闭，上线后打开
#define	RUN_LIGHT								2				//运行灯  运行时1HZ闪烁
// #define	LOCAL_LIGHT								78				//本地灯  上电抄表成功常亮
#define ONLINE									3				//上线灯

#define DIRECT_CON_485							0				//485方向控制管脚

#define ENABLE_HARD_SCI_SEND					nwy_gpio_set_value(DIRECT_CON_485, 1);

#define ENABLE_HARD_SCI_RECV					nwy_gpio_set_value(DIRECT_CON_485, 0);

#define READ									0
#define WRITE									1
#define SERVER_TRAN 							0				//TCP服务器透传
#define BLUETOOTH_TRAN 							1 				//蓝牙透传
#define BASEMETER_UPGRADES 						0x5AA55AA5
#define DOWNLOAD_METERFILE 						0xA55AA55A
#define UART_BUFF_MAX_LEN 						(1024 + 30)
#define POWERON_READRPARA_NUM 					eBIT_TIME
#define TCP_RESEND_QUEUE_SIZE 					8


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
	BYTE buf[1024];
	DWORD len; /* data */
	BYTE TranType;
	BYTE Uart;
} tTranData;

// 分钟、秒、时抄表bit位枚举
typedef enum
{
	eBIT_ADDR_WAVE = 0,
	eBIT_WAVE_FACTOR,
	eBIT_VERSION,
	// eBIT_LTO_ADDR,
	eBIT_LTO_MODULEVERSIONtoMQTT,
	eBIT_LTO_OPEN_FACTORY,
	eBIT_LTO_SET_TIME,
	eBIT_TIME,
	eBIT_LTO_MODULEVERSIONtoTCP,
	eBIT_LTO_SAFE_FREEZE,
	eBIT_TCPNET,
	eBIT_MQTTNET,
	eBIT_TCPUSER,
	eBIT_MQTTUSER,
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
	eBIT_GET_LTO_FREEZE,
	eBIT_OPEN_WAVE_SYS,
	eBIT_SET_WAVE_TIME,
	eBIT_GET_METER_UIP,
	eBIT_TOTAL,
} eREAD_METER_BIT;

typedef enum
{
	eUART_WHWAVE = 0,		//对应 录波表 串口id	645透传
	eUART_LTO				//物联表表 串口id		698透传
}eUART_ID;

typedef enum
{
	ePARA_TCP = 0,			//tcp参数
	ePARA_MQTT,				//mqtt参数
	ePARA_MAX_NUM
}eServerPara;

typedef enum
{
	eTcpThread,
	eTcpUserThread,
	eDateCallThread,
	eMqttThread,
	eUartThread,
	eFactoryParaThread,
	eTfThread,
	eMaxThreadNum
}eThreadType;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern nwy_osiMessageQueue_t *MqttResMessageQueue;
extern nwy_osiMessageQueue_t *FtpUpdateMessageQueue;
extern nwy_osiMessageQueue_t *CollectMessageQueue;
extern nwy_osiMessageQueue_t *WaveDataUploadMessageQueue;
extern nwy_osiMessageQueue_t *TcpUserServertoTFMsgQue;
extern nwy_osiMessageQueue_t *WaveDataToTFMessageQueue;
extern nwy_osiMessageQueue_t *TimerMessageQueue;
extern nwy_osiMessageQueue_t *TranDataToTcpUserServerMsgQue;
extern nwy_osiMessageQueue_t *TranDataToTcpPrivateServerMsgQue;
extern nwy_osiMessageQueue_t *TranDataToUartMessageQueue;
extern nwy_osiMessageQueue_t *TranDataToFactoryMessageQueue;
extern nwy_osiMessageQueue_t *UpgradeResultMessageQueue;
extern nwy_osiMessageQueue_t *RecvTcpUserServerMsgQue;
extern nwy_osiMessageQueue_t *TCPResendTmpMsgQue;
#if (TCP_COMM_MODULE == YES)
extern nwy_osiMessageQueue_t *TcpDataToModuleMsgQue;
extern nwy_osiMessageQueue_t *ModuleDataToTcpMsgQue;
#endif
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
extern nwy_osiTimer_t *general_timer;
extern nwy_osiTimer_t *meter_upgrade_timer;
extern nwy_osiTimer_t *ftp_timer;
extern nwy_osiTimer_t *uart_timer;
extern nwy_osiTimer_t *TF_Requset_timer;
extern nwy_osiTimer_t *Ble_Recv_ByteTimeout_timer;
extern nwy_osiTimer_t *Uart_Recv_timer;
#if (TCP_COMM_MODULE == YES)
extern nwy_osiTimer_t *Tcp_ComModule_timer;
#endif
extern char PrintLogSign;
extern int tcp_connect_flag[2];
extern int BleLogSwitch; //蓝牙打印log开关
extern DWORD ThreadRunCnt[eMaxThreadNum];
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
void lib_InverseData( BYTE *Buf, WORD Len );
void lib_ExchangeData(BYTE *BufA, BYTE *BufB, BYTE Len);
void nwy_ext_send_sig(nwy_osiThread_t *thread, uint16 sig);
void nwy_ftp_timer_cb(void *type);
void nwy_meter_upgrade_timer_cb(void *type);
#if(BLE_UART_WH == YES)
void nwy_Ble_Recv_timer_cb(void *type);
#endif
#if(TCP_COMM_MODULE == YES)
void Tcp_Commodule_timer_cb(void *type);
#endif
void nwy_general_timer_cb(void *type);
void Uart_Recv_timer_cb(void *type);
void TF_Request_timer_cb(void *type);
void api_ReadSystemFiletoRam(eREAD_METER_BIT eBit);
void Location_Task(void *param);
BOOL get_gps_position_info(char *c_data, DWORD *addr);
void TCP_User_Task(void *param);
void TCP_PrivateTask(void *param);
void TF_Task(void *param);
BYTE Tcp_send(BYTE SocketNum, char *sendbuff, int sendlen);
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
//--------------------------------------------------
//功能描述:  操作文件系统
//         
//参数:      Operation 1写操作 0 读操作
//           addr 要操作的文件地址
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_OperateFileSystem( BYTE Operation,int addr,BYTE*Buf,WORD bLen);
//--------------------------------------------------
//功能描述:  上电创建或打开时间para文件
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL api_PowerOnCreatParaTable( void );
#endif //#ifndef __FUNC_DEF_H
