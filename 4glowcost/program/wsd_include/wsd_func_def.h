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
#define UART_WH									YES				//串口作为维护端口
#define GPRS_POSITION							NO				//GPRS 开关
#define	TEST_STACK								YES				//堆栈分析
#define ONLINE_PARA_SECLET						FROM_BASE_METER		//ip端口等参数获取位置

//UART 抄表相关/***********************************************************************/
#define READMETER_PROTOCOL						PROTOCOL_645	//下行抄表规约
#define MAX_SAMPLE_CHIP 						1				//兼容4回路 但基表末尾地址必须为1！！！
#define MAX_RATIO								6				//费率数要与表一致，不一致则读电能有问题 总+ratio5
#define CYCLE_METER_READING						NO				//模块周期抄表
#define CYCLE_REPORT_PROTOCAL					PROTOCOL_212	//周期上报协议（mqt/ep212/104）
#define PT_CT									NO				//周期上报带变比
#define UART_BUFF_MAX_LEN 						(512+30)
#define POWERON_READRPARA_NUM					eBIT_USEDCHANNEL	//上电抄表 截止参数

// 网络层相关/***********************************************************************/
#define TCP_TOTAL_NUM							(MAX_PRIVATE_SERVER_NUM+MAX_USER_TRANS_NUM+MAX_SPECIAL_USER_NUM)//总的tcp的连接个数 
#define USER_SERVER_LIGHT						TCP_USER		//远程上线灯取决于要上哪个主站
	//TCP 协议
#define MAX_PRIVATE_SERVER_NUM					1				//维护端tcp服务器个数 （tcp可最多连接8个，默认0连接维护端主站）
#define MAX_USER_TRANS_NUM						0				//用户端转发功能	  (其他用户端主站需求若也是转发主站数据，对应宏定义递增）
#define MAX_USER_SPECIAL_NUM					5				//用户端特殊功能
#define USER_SPECIAL_START						(MAX_PRIVATE_SERVER_NUM + MAX_USER_TRANS_NUM)		
#define MAX_SPECIAL_USER_NUM					5

#define	TCP_USER								YES				//tcp 连接用户端主站
#define TCP_COMM_MODULE							YES				//tcp 与模块通信 
//MQTT 协议
#define MQTT_USER								NO				//mqtt 连接用户端主站
#define	EVENT_REPORT							NO				//事件上报开关
//212环保协议


// IO管脚控制/***********************************************************************/
#define FLASH_PIN 								5				//FLASH 不用 管脚置低
#define REMOTE_LIGHT							84				//上线绿灯默认关闭，上线后打开
#define REMOTE_LIGHT_OPEN						nwy_gpio_value_set(REMOTE_LIGHT,1);
#define REMOTE_LIGHT_CLOSE						nwy_gpio_value_set(REMOTE_LIGHT,0);
#define	TX_LIGHT								28				//tx
#define	RX_LIGHT								27				//本地灯  上电抄表成功常亮
#define RUN_LIGHT								0				//运行灯
#define	LOCAL_LIGHT								1				//本地灯  上电抄表成功常亮
#define POWER_DOWN_VOLTAGE						500			//掉电阈值 可插拔模块 值为500 内置模块2500 

//升级
#define CHIP_415_UPGRADE						0x415645		//645 进行 对F415芯片进行升级
#define BASEMETER_UPGRADES						0x5AA55AA5		//基表升级
#define	DOWNLOAD_METERFILE						0xA55AA55A		//下载基表文件
#define FILENAME_LEM							128
#define FTP_STRING_SIZE							256//根据n580的ftp结构体定义


#define READ									0
#define WRITE									1
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct {
	const BYTE* buf;
	const DWORD MessageSize;
	DWORD MessageNum;
	DWORD WriteOffset;
	nwy_osi_msg_queue_t* QueueHandle;
}MessageQueueInfo;
typedef enum
{
	eQUEUE_COLLECT = 0,				// 采集数据队列
	eQUEUE_UART_TO_TCP,				// UART到TCP的数据队列
	eQUEUE_TRAN_TO_MQTT_SERVER,		// 透传到MQTT服务器的数据队列
	eQUEUE_TRAN_TO_UART,			// 透传到UART的数据队列
	eQUEUE_TRAN_TO_FACTORY,		// 透传到工厂的数据队列
	#if (TCP_COMM_MODULE == YES)
	eQUEUE_TCP_TO_MODULE,			// TCP到模块的数据队列
	eQUEUE_MODULE_TO_TCP,			// 模块到TCP的数据队列
	#endif
	#if (CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	eQUEUE_EP212_TO_UART,
	eQUEUE_UART_TO_EP212,
	#endif
	eQUEUE_MAX_NUM,
}eQueueType;//元素大小大于256字节的队列
#pragma pack(1)
typedef struct  {
	char filename[128];			//文件名称
	char filepath[128];			//下载文件后的路径 下载后路径固定/nwy/文件名
	char updateMode;			//1 全包升级 0 差分升级	    
	char ftpupdateStep;			//必须写1 从登录开始
	char ftp_connect_flag;		//主站升级命令标志 1开始升级 0禁止升级	
}FtpUpdatePara_t;
#pragma pack()	

typedef struct  {
	nwy_ftp_param_t ftp_param;
	char filename[FILENAME_LEM];
	char host[FTP_STRING_SIZE];//nwy_ftp_param_t 里面只存了指针，不改有方定义情况下需要自己定义数组存储
	char username[FTP_STRING_SIZE];//nwy_ftp_param_t 里面只存了指针，需要自己定义数组存储
	char PassWord[FTP_STRING_SIZE];//nwy_ftp_param_t 里面只存了指针，需要自己定义数组存储
}FtpUpdatePara;

typedef struct tTranData
{
	BYTE buf[4096+30];
	DWORD len;/* data */
	BYTE  TranType;
	BYTE  socketid;
	BYTE  threadid;
}tTranData;

// 分钟、秒、时抄表bit位枚举
typedef enum
{
	eBIT_ADDR = 0,	
	eBIT_TIME,
	eBIT_TCP_USER_PARA,	
	eBIT_USEDCHANNEL,	//基表使用回路数
	eBIT_TCPNET,
	eBIT_VERSION,
	eBIT_OPEN_FACTORY,
	eBIT_MQTTNET,
	eBIT_TCPUSER,
	eBIT_MQTTUSER,
	#if(PT_CT == YES)
	eBIT_CT1,
	eBIT_CT2,
	eBIT_PT1,
	eBIT_PT2,
	#endif
	eBIT_POSITION,
	#if( EVENT_REPORT ==  YES)
	eBIT_EVENT,
	#endif
	eBIT_TOTAL,
}eREAD_METER_BIT;


typedef enum
{
	eTRAN_TCP = 0,			//tcp参数
	eTRAN_BLETOOTH,			//蓝牙参数
	eTRAN_MQTT,
	ePARA_MAX_NUM
}eTransChannel;

typedef enum
{
	eUartThread = 0,
	eFactoryParaThread,
	eMqttThread,
	eTcpThread = 0,
	eTcpUserThread,
	eMaxThreadNum
}eThreadType;

typedef enum
{
	eMETER = 0,
	eMODULE
}MeterorModu;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern MessageQueueInfo MessageQueueInfoArr[eQUEUE_MAX_NUM];
extern nwy_osi_thread_t Timer_thread;
extern nwy_osi_msg_queue_t MqttResMessageQueue;
extern nwy_osi_msg_queue_t MqttTransMessageQueue;
extern nwy_osi_msg_queue_t MqttResUserMessageQueue;
extern nwy_osi_msg_queue_t FtpUpdateMessageQueue;
extern nwy_osi_msg_queue_t CollectMessageQueue;
extern nwy_osi_msg_queue_t TimerMessageQueue;
extern nwy_osi_msg_queue_t UartDataToTcpMsgQue;
extern nwy_osi_msg_queue_t TranDataToMqttServerMsgQue;
extern nwy_osi_msg_queue_t TranDataToUartMessageQueue;
extern nwy_osi_msg_queue_t TranDataToFactoryMessageQueue;
extern nwy_osi_msg_queue_t UpgradeResultMessageQueue;
#if (TCP_COMM_MODULE == YES)
extern nwy_osi_msg_queue_t TcpDataToModuleMsgQue;
extern nwy_osi_msg_queue_t ModuleDataToTcpMsgQue;
#endif
#if(EVENT_REPORT == YES)
extern nwy_osi_msg_queue_t EventReportMessageQueue;
#endif
#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
extern nwy_osi_msg_queue_t Ep212ToUartMsgQue;
extern nwy_osi_msg_queue_t UartToEp212MsgQue;
#endif
extern nwy_osi_msg_queue_t UserTcpStatusChangeMsgQue;//tcp 用户端状态改变通知
extern nwy_osi_mutex_t QueueWriteMutex;
extern nwy_osi_thread_t FtpFotaUpdate_thread;
extern nwy_osi_thread_t location_thread;
// extern nwy_sim_result_type imei;
extern nwy_osi_thread_t g_app_thread;
extern nwy_osi_thread_t ftp_thread;
extern nwy_osi_thread_t uart_thread;
extern nwy_osi_thread_t factorypara_thread;
//定时器 
extern nwy_osi_timer_t g_timer;
extern nwy_osi_timer_t general_timer;
extern nwy_osi_timer_t User_MQTT_Reconnect_timer;
extern nwy_osi_timer_t meter_upgrade_timer;
extern nwy_osi_timer_t ftp_timer;
extern nwy_osi_timer_t uart_timer;
extern nwy_osi_timer_t Ble_Recv_ByteTimeout_timer;
extern nwy_osi_timer_t  Uart_Recv_timer;
#if (TCP_COMM_MODULE == YES)
extern nwy_osi_timer_t Tcp_ComModule_timer;
#endif
extern char PrintLogSign;
extern int BleLogSwitch ;//蓝牙打印log开关
extern DWORD ThreadRunCnt[eMaxThreadNum];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
BYTE QueueWrite(MessageQueueInfo *queueInfo, tTranData *data, DWORD dataSize, SDWORD timeout);
void nwy_uart_timer_cb(void *type);
void nwy_ftp_timer_cb(void *type);
int nwy_ext_check_data_connect();
void WSD_MQTT_Task(void* param);
void USER_MQTT_Task(void *param);
void DataCall_Task(void *profileID);
void Fota_UpdateTask(void *parameter);
void  Factory_Task( void *param );
void  MeterUpgrade645_Task( void  );
void  MeterUpgrade698_Task( void );
void lib_ExchangeData(BYTE *BufA, BYTE *BufB, BYTE Len );
void nwy_ftp_timer_cb(void *type);
void nwy_meter_upgrade_timer_cb(void *type);
void nwy_User_MQTT_Reconnect_timer_cb(void *type);
#if(TCP_COMM_MODULE == YES)
void Tcp_Commodule_timer_cb(void *type);
#endif
void nwy_general_timer_cb(void *type);
void Uart_Recv_timer_cb(void *type);
void nwy_meter_upgrade_timer_cb(void *type);
void  api_ReadSystemFiletoRam( eREAD_METER_BIT eBit );
void  Location_Task( void *param );
BOOL get_gps_position_info(char *c_data,DWORD *addr);
void TCP_User_Task(void *param);
void TCP_PrivateTask(void *param);
void Tcp_send(int Isocket, char *sendbuff, int sendlen, BYTE i);
//--------------------------------------------------
//功能描述: 获取mqtt 链接状态 
//
//参数:  bnum  0 维护端  1 用户端
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL api_GetMqttConStatus( BYTE bNum );
//--------------------------------------------------
//功能描述:  获取当前模块升级状态
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_GetModuleUpGradeStatus( void );
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
//功能描述:  检测主站连接状态
//         
//参数:      bNum 连接tcp主站序号
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_GetTcpConStatus( BYTE bNum );
#endif //#ifndef __FUNC_DEF_H
