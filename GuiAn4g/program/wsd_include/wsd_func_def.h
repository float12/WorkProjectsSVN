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
#define BLE_WH									NO				//蓝牙作为维护端口 此处区分硬件 
#define UART_WH									YES				//串口作为维护端口
#define GPRS_POSITION							NO				//GPRS 开关
#define	TEST_STACK								NO				//堆栈分析
#define ONLINE_PARA_SECLET						FROM_MODULE		//ip端口等参数获取位置

//UART 抄表相关/***********************************************************************/
#define READMETER_PROTOCOL						PROTOCOL_645	//下行抄表规约
#define MAX_SAMPLE_CHIP 						1				//兼容4回路 但基表末尾地址必须为1！！！
#define MAX_RATIO								6				//费率数要与表一致，不一致则读电能有问题 总+ratio5
#define CYCLE_METER_READING						YES				//模块周期抄表
#define CYCLE_REPORT_PROTOCAL					PROTOCOL_MQTT	//周期上报协议（mqt/ep212/104）
#define PT_CT									NO				//周期上报带变比
#define UART_BUFF_MAX_LEN 						(512+30)
#define POWERON_READRPARA_NUM					eBIT_TCP_USER_PARA	//上电抄表 截止参数

// 网络层相关/***********************************************************************/
//N58 socket最多创建8个  有2个用于mqtt维护和ftp升级 TCP维护端使用一个,TCP用户端最多使用5个
#define TCP_TOTAL_NUM							(MAX_PRIVATE_SERVER_NUM+MAX_USER_TRANS_NUM+MAX_SPECIAL_USER_NUM)//总的tcp的连接个数 
#define USER_SERVER_LIGHT						TCP_USER		//远程上线灯取决于要上哪个主站
	//TCP 协议
#define MAX_PRIVATE_SERVER_NUM					1				//维护端tcp服务器个数 （tcp可最多连接8个，默认0连接维护端主站）
#define MAX_USER_TRANS_NUM						0				//用户端转发功能	  (其他用户端主站需求若也是转发主站数据，对应宏定义递增）
#define MAX_USER_SPECIAL_NUM					5				//用户端特殊功能
#define USER_SPECIAL_START						(MAX_PRIVATE_SERVER_NUM + MAX_USER_TRANS_NUM)		
#define MAX_SPECIAL_USER_NUM					2

#define	TCP_USER								NO				//tcp 连接用户端主站
#define TCP_COMM_MODULE							YES				//tcp 与模块通信 
	//MQTT 协议
#define MQTT_USER								YES				//mqtt 连接用户端主站 注 开启mqtt用户端时 要开启 CYCLE_REPORT_PROTOCAL 上报格式为MQTT格式
#define	EVENT_REPORT							NO				//事件上报开关
	//212环保协议


// IO管脚控制/***********************************************************************/
#define FLASH_PIN_N58 							3				//cvflash N58 可插拔 flash 测试好用
#define FLASH_PIN_CTRLCOST						17				//cvflash N58 费控表
#define FLASH_PIN 								0				//FLASH 不用 管脚置低
#define REMOTE_LIGHT							9				//上线绿灯默认关闭，上线后打开
#define REMOTE_LIGHT_OPEN						nwy_gpio_set_value(REMOTE_LIGHT,1);
#define REMOTE_LIGHT_CLOSE						nwy_gpio_set_value(REMOTE_LIGHT,0);
#define	RUN_LIGHT								28				//运行灯  运行时1HZ闪烁
#define	LOCAL_LIGHT								27				//本地灯  上电抄表成功常亮
#define POWER_DOWN_VOLTAGE						400			//掉电阈值 N58可插拔模块 值为1200 内置模块2500 

//升级
#define CHIP_415_UPGRADE						0x415645		//645 进行 对F415芯片进行升级
#define BASEMETER_UPGRADES						0x5AA55AA5		//基表升级
#define	DOWNLOAD_METERFILE						0xA55AA55A		//下载基表文件
#define FILENAME_LEM							128



#define READ									0
#define WRITE									1
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct {
	BYTE Messagetype;
	BYTE *Buf;
}UartSendMessage;

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
	nwy_ftp_login_t ftp_param;
	char filename[FILENAME_LEM];
}FtpUpdatePara;

typedef struct tTranData
{
	BYTE buf[NWY_UART_RECV_SINGLE_MAX+1];
	DWORD len;/* data */
	BYTE  TranType;
	BYTE  socketid;
	BYTE  threadid;
}tTranData;

// 分钟、秒、时抄表bit位枚举
typedef enum
{
	eBIT_ADDR = 0,	
	eBIT_VERSION,
	eBIT_TCP_USER_PARA,	
	eBIT_USEDCHANNEL,	//基表使用回路数
	eBIT_TCPNET,
	eBIT_TIME,
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
	#if( EVENT_REPORT ==  YES)//抄读事件 标识bit统一放此处
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
extern nwy_osiMessageQueue_t *MqttResMessageQueue;
extern nwy_osiMessageQueue_t *MqttTransMessageQueue;
extern nwy_osiMessageQueue_t *MqttResUserMessageQueue;
extern nwy_osiMessageQueue_t *FtpUpdateMessageQueue;
extern nwy_osiMessageQueue_t *CollectMessageQueue;
extern nwy_osiMessageQueue_t *TimerMessageQueue;
extern nwy_osiMessageQueue_t *UartDataToTcpMsgQue;
extern nwy_osiMessageQueue_t *TranDataToMqttServerMsgQue;
extern nwy_osiMessageQueue_t *TranDataToUartMessageQueue;
extern nwy_osiMessageQueue_t *TranDataToFactoryMessageQueue;
extern nwy_osiMessageQueue_t *UpgradeResultMessageQueue;
#if (TCP_COMM_MODULE == YES)
extern nwy_osiMessageQueue_t *TcpDataToModuleMsgQue;
extern nwy_osiMessageQueue_t *ModuleDataToTcpMsgQue;
#endif
#if(EVENT_REPORT == YES)
extern nwy_osiMessageQueue_t *EventReportMessageQueue;
#endif
#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
extern nwy_osiMessageQueue_t *Ep212ToUartMsgQue;
extern nwy_osiMessageQueue_t *UartToEp212MsgQue;
#endif
extern nwy_osiMessageQueue_t *UserTcpStatusChangeMsgQue;//tcp 用户端状态改变通知
extern nwy_osiMessageQueue_t *MQTTUserToUartMsgQue;//继电器状态改变原因通知
extern nwy_osiMessageQueue_t *UartReplyToMqttMsgQue;
//线程
extern nwy_osiThread_t *FtpFotaUpdate_thread;
extern nwy_osiThread_t *location_thread;
extern nwy_sim_result_type imei;
extern nwy_osiThread_t *g_app_thread;
extern nwy_osiThread_t *ftp_thread;
extern nwy_osiThread_t *uart_thread;
extern nwy_osiThread_t *factorypara_thread;
//定时器 
extern nwy_osiTimer_t *g_timer;
extern nwy_osiTimer_t *general_timer;
extern nwy_osiTimer_t *User_MQTT_Reconnect_timer;
extern nwy_osiTimer_t *meter_upgrade_timer;
extern nwy_osiTimer_t *ftp_timer;
extern nwy_osiTimer_t *uart_timer;
extern nwy_osiTimer_t *Ble_Recv_ByteTimeout_timer;
extern nwy_osiTimer_t * Uart_Recv_timer;
extern nwy_osiTimer_t *Check_Relay_Status_Timer;
#if (TCP_COMM_MODULE == YES)
extern nwy_osiTimer_t *Tcp_ComModule_timer;
#endif
extern char PrintLogSign;
extern int BleLogSwitch ;//蓝牙打印log开关
extern DWORD ThreadRunCnt[eMaxThreadNum];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
int nwy_ext_check_data_connect();
void WSD_MQTT_Task(void* param);
void USER_MQTT_Task(void *param);
void DataCall_Task(void *profileID);
void Fota_UpdateTask(void *parameter);
void  Factory_Task( void *param );
void  MeterUpgrade645_Task( void  );
void  MeterUpgrade698_Task( void );
void lib_ExchangeData(BYTE *BufA, BYTE *BufB, BYTE Len );
void nwy_ext_send_sig(nwy_osiThread_t *thread, uint16 sig);
void nwy_ftp_timer_cb(void *type);
void nwy_meter_upgrade_timer_cb(void *type);
void nwy_User_MQTT_Reconnect_timer_cb(void *type);
#if(BLE_WH== YES)
void nwy_Ble_Recv_timer_cb(void *type);
#endif
#if(TCP_COMM_MODULE == YES)
void Tcp_Commodule_timer_cb(void *type);
#endif
void nwy_general_timer_cb(void *type);
void Uart_Recv_timer_cb(void *type);
void Check_Relay_Status_Timer_cb(void *type);
void nwy_ftp_timer_cb(void *type);
void nwy_meter_upgrade_timer_cb(void *type);
void  api_ReadSystemFiletoRam( eREAD_METER_BIT eBit );
void  Location_Task( void *param );
BOOL get_gps_position_info(char *c_data,DWORD *addr);
void TCP_User_Task(void *param);
void TCP_TranserTask(void *param);
void Tcp_send(int Isocket, char *sendbuff, int sendlen, BYTE i);
#if( BLE_WH== YES)
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
#endif
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
