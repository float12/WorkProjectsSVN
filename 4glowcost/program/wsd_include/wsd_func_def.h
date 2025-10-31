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
#define UART_WH									YES				//������Ϊά���˿�
#define GPRS_POSITION							NO				//GPRS ����
#define	TEST_STACK								YES				//��ջ����
#define ONLINE_PARA_SECLET						FROM_BASE_METER		//ip�˿ڵȲ�����ȡλ��

//UART �������/***********************************************************************/
#define READMETER_PROTOCOL						PROTOCOL_645	//���г����Լ
#define MAX_SAMPLE_CHIP 						1				//����4��· ������ĩβ��ַ����Ϊ1������
#define MAX_RATIO								6				//������Ҫ���һ�£���һ��������������� ��+ratio5
#define CYCLE_METER_READING						NO				//ģ�����ڳ���
#define CYCLE_REPORT_PROTOCAL					PROTOCOL_212	//�����ϱ�Э�飨mqt/ep212/104��
#define PT_CT									NO				//�����ϱ������
#define UART_BUFF_MAX_LEN 						(512+30)
#define POWERON_READRPARA_NUM					eBIT_USEDCHANNEL	//�ϵ糭�� ��ֹ����

// ��������/***********************************************************************/
#define TCP_TOTAL_NUM							(MAX_PRIVATE_SERVER_NUM+MAX_USER_TRANS_NUM+MAX_SPECIAL_USER_NUM)//�ܵ�tcp�����Ӹ��� 
#define USER_SERVER_LIGHT						TCP_USER		//Զ�����ߵ�ȡ����Ҫ���ĸ���վ
	//TCP Э��
#define MAX_PRIVATE_SERVER_NUM					1				//ά����tcp���������� ��tcp���������8����Ĭ��0����ά������վ��
#define MAX_USER_TRANS_NUM						0				//�û���ת������	  (�����û�����վ������Ҳ��ת����վ���ݣ���Ӧ�궨�������
#define MAX_USER_SPECIAL_NUM					5				//�û������⹦��
#define USER_SPECIAL_START						(MAX_PRIVATE_SERVER_NUM + MAX_USER_TRANS_NUM)		
#define MAX_SPECIAL_USER_NUM					5

#define	TCP_USER								YES				//tcp �����û�����վ
#define TCP_COMM_MODULE							YES				//tcp ��ģ��ͨ�� 
//MQTT Э��
#define MQTT_USER								NO				//mqtt �����û�����վ
#define	EVENT_REPORT							NO				//�¼��ϱ�����
//212����Э��


// IO�ܽſ���/***********************************************************************/
#define FLASH_PIN 								5				//FLASH ���� �ܽ��õ�
#define REMOTE_LIGHT							84				//�����̵�Ĭ�Ϲرգ����ߺ��
#define REMOTE_LIGHT_OPEN						nwy_gpio_value_set(REMOTE_LIGHT,1);
#define REMOTE_LIGHT_CLOSE						nwy_gpio_value_set(REMOTE_LIGHT,0);
#define	TX_LIGHT								28				//tx
#define	RX_LIGHT								27				//���ص�  �ϵ糭��ɹ�����
#define RUN_LIGHT								0				//���е�
#define	LOCAL_LIGHT								1				//���ص�  �ϵ糭��ɹ�����
#define POWER_DOWN_VOLTAGE						500			//������ֵ �ɲ��ģ�� ֵΪ500 ����ģ��2500 

//����
#define CHIP_415_UPGRADE						0x415645		//645 ���� ��F415оƬ��������
#define BASEMETER_UPGRADES						0x5AA55AA5		//��������
#define	DOWNLOAD_METERFILE						0xA55AA55A		//���ػ����ļ�
#define FILENAME_LEM							128
#define FTP_STRING_SIZE							256//����n580��ftp�ṹ�嶨��


#define READ									0
#define WRITE									1
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
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
	eQUEUE_COLLECT = 0,				// �ɼ����ݶ���
	eQUEUE_UART_TO_TCP,				// UART��TCP�����ݶ���
	eQUEUE_TRAN_TO_MQTT_SERVER,		// ͸����MQTT�����������ݶ���
	eQUEUE_TRAN_TO_UART,			// ͸����UART�����ݶ���
	eQUEUE_TRAN_TO_FACTORY,		// ͸�������������ݶ���
	#if (TCP_COMM_MODULE == YES)
	eQUEUE_TCP_TO_MODULE,			// TCP��ģ������ݶ���
	eQUEUE_MODULE_TO_TCP,			// ģ�鵽TCP�����ݶ���
	#endif
	#if (CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	eQUEUE_EP212_TO_UART,
	eQUEUE_UART_TO_EP212,
	#endif
	eQUEUE_MAX_NUM,
}eQueueType;//Ԫ�ش�С����256�ֽڵĶ���
#pragma pack(1)
typedef struct  {
	char filename[128];			//�ļ�����
	char filepath[128];			//�����ļ����·�� ���غ�·���̶�/nwy/�ļ���
	char updateMode;			//1 ȫ������ 0 �������	    
	char ftpupdateStep;			//����д1 �ӵ�¼��ʼ
	char ftp_connect_flag;		//��վ���������־ 1��ʼ���� 0��ֹ����	
}FtpUpdatePara_t;
#pragma pack()	

typedef struct  {
	nwy_ftp_param_t ftp_param;
	char filename[FILENAME_LEM];
	char host[FTP_STRING_SIZE];//nwy_ftp_param_t ����ֻ����ָ�룬�����з������������Ҫ�Լ���������洢
	char username[FTP_STRING_SIZE];//nwy_ftp_param_t ����ֻ����ָ�룬��Ҫ�Լ���������洢
	char PassWord[FTP_STRING_SIZE];//nwy_ftp_param_t ����ֻ����ָ�룬��Ҫ�Լ���������洢
}FtpUpdatePara;

typedef struct tTranData
{
	BYTE buf[4096+30];
	DWORD len;/* data */
	BYTE  TranType;
	BYTE  socketid;
	BYTE  threadid;
}tTranData;

// ���ӡ��롢ʱ����bitλö��
typedef enum
{
	eBIT_ADDR = 0,	
	eBIT_TIME,
	eBIT_TCP_USER_PARA,	
	eBIT_USEDCHANNEL,	//����ʹ�û�·��
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
	eTRAN_TCP = 0,			//tcp����
	eTRAN_BLETOOTH,			//��������
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
//				��������
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
extern nwy_osi_msg_queue_t UserTcpStatusChangeMsgQue;//tcp �û���״̬�ı�֪ͨ
extern nwy_osi_mutex_t QueueWriteMutex;
extern nwy_osi_thread_t FtpFotaUpdate_thread;
extern nwy_osi_thread_t location_thread;
// extern nwy_sim_result_type imei;
extern nwy_osi_thread_t g_app_thread;
extern nwy_osi_thread_t ftp_thread;
extern nwy_osi_thread_t uart_thread;
extern nwy_osi_thread_t factorypara_thread;
//��ʱ�� 
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
extern int BleLogSwitch ;//������ӡlog����
extern DWORD ThreadRunCnt[eMaxThreadNum];
//-----------------------------------------------
// 				��������
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
//��������: ��ȡmqtt ����״̬ 
//
//����:  bnum  0 ά����  1 �û���
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL api_GetMqttConStatus( BYTE bNum );
//--------------------------------------------------
//��������:  ��ȡ��ǰģ������״̬
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_GetModuleUpGradeStatus( void );
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
//��������:  �����վ����״̬
//         
//����:      bNum ����tcp��վ���
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_GetTcpConStatus( BYTE bNum );
#endif //#ifndef __FUNC_DEF_H
