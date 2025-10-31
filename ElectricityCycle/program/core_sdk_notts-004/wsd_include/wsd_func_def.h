//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾��ѹ̨����Ʒ�� 
//������	��Ȫ
//��������	
//����		FUNC_DEFͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __FUNC_DEF_H
#define __FUNC_DEF_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define MAX_SAMPLE_CHIP 						1				//����4��· ������ĩβ��ַ����Ϊ1������
#define MAX_RATIO								6				//������Ҫ���һ�£���һ��������������� ��+ratio5
#define FILENAME_LEM							128
#define READMETER_PROTOCOL						PROTOCOL_645	//���г����Լ
#define BLE_UART_WH								NO				//������uart��Ϊά���˿� �˴�����Ӳ�� ZH �������ϱ��Ŀ���״̬��δ���
#define GPRS_POSITION							NO				//GPRS ����
#define CYCLE_REPORT							NO				//�����ϱ�����	�����ϱ������¼��ϱ�
#define	EVENT_REPORT							NO				//�¼��ϱ�����
#define	TRANS_SWITCH							TCP_TRANS		//͸��Э��
#define PT_CT									NO				//�����ϱ������
#define	SYS_MONITOR								NO				//��ջ����
#define	TCP_SWITCH								YES				//tcp����
#define TCP_COMM_MODULE							YES				//tcp ��ģ��ͨ�� 

#define FLOW_CALCULATE 							YES				//����ͳ��
#define COLLECT_UIP 							NO				//�ɼ�uip
#define UPLOAD_WAVE_DATA_SWITCH 				YES				//�ϴ��������ݿ���
#define LTO_METER_UART							YES
#define EVENT_RECORD							YES				//�쳣�¼���¼

// IO�ܽſ���
#define SD_POWER_ON								9				//�����̵�Ĭ�Ϲرգ����ߺ��
#define	RUN_LIGHT								2				//���е�  ����ʱ1HZ��˸
// #define	LOCAL_LIGHT								78				//���ص�  �ϵ糭��ɹ�����
#define ONLINE									3				//���ߵ�

#define DIRECT_CON_485							0				//485������ƹܽ�

#define ENABLE_HARD_SCI_SEND					nwy_gpio_set_value(DIRECT_CON_485, 1);

#define ENABLE_HARD_SCI_RECV					nwy_gpio_set_value(DIRECT_CON_485, 0);

#define READ									0
#define WRITE									1
#define SERVER_TRAN 							0				//TCP������͸��
#define BLUETOOTH_TRAN 							1 				//����͸��
#define BASEMETER_UPGRADES 						0x5AA55AA5
#define DOWNLOAD_METERFILE 						0xA55AA55A
#define UART_BUFF_MAX_LEN 						(1024 + 30)
#define POWERON_READRPARA_NUM 					eBIT_TIME
#define TCP_RESEND_QUEUE_SIZE 					8


//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct
{
	BYTE Messagetype;
	BYTE *Buf;
} UartSendMessage;

#pragma pack(1)
typedef struct
{
	char filename[128];			//�ļ�����
	char filepath[128];			//�����ļ����·�� ���غ�·���̶�/nwy/�ļ���
	char updateMode;			//1 ȫ������ 0 �������	    
	char ftpupdateStep;			//����д1 �ӵ�¼��ʼ
	char ftp_connect_flag;		//��վ���������־ 1��ʼ���� 0��ֹ����	

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

// ���ӡ��롢ʱ����bitλö��
typedef enum
{
	eBIT_ADDR_WAVE = 0,
	eBIT_WAVE_FACTOR,
	eBIT_VERSION,
	eBIT_LTO_ADDR,
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
	eUART_WHWAVE = 0,		//��Ӧ ¼���� ����id	645͸��
	eUART_LTO				//������� ����id		698͸��
}eUART_ID;

typedef enum
{
	ePARA_TCP = 0,			//tcp����
	ePARA_MQTT,				//mqtt����
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
//				��������
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
extern int BleLogSwitch; //������ӡlog����
extern DWORD ThreadRunCnt[eMaxThreadNum];
//-----------------------------------------------
// 				��������
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
//��������:
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
BYTE SendBleDataInPiece(char *data, WORD total_len);
//--------------------------------------------------
//��������:  
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL api_GetMqttConStatus(void);
//--------------------------------------------------
//��������:  ��ȡ��ǰģ������״̬
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL api_GetModuleUpGradeStatus(void);
//--------------------------------------------------
//��������:  �����ļ�ϵͳ
//         
//����:      Operation 1д���� 0 ������
//           addr Ҫ�������ļ���ַ
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_OperateFileSystem( BYTE Operation,int addr,BYTE*Buf,WORD bLen);
//--------------------------------------------------
//��������:  �ϵ紴�����ʱ��para�ļ�
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL api_PowerOnCreatParaTable( void );
#endif //#ifndef __FUNC_DEF_H
