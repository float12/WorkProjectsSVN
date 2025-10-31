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
#define BLE_WH									NO				//������Ϊά���˿� �˴�����Ӳ�� 
#define UART_WH									YES				//������Ϊά���˿�
#define GPRS_POSITION							NO				//GPRS ����
#define	TEST_STACK								NO				//��ջ����

//UART �������/***********************************************************************/
#define READMETER_PROTOCOL						PROTOCOL_645	//���г����Լ
#define MAX_SAMPLE_CHIP 						1				//����4��· ������ĩβ��ַ����Ϊ1������
#define MAX_RATIO								6				//������Ҫ���һ�£���һ��������������� ��+ratio5
#define CYCLE_METER_READING						NO				//ģ�����ڳ���
#define CYCLE_REPORT_PROTOCAL					PROTOCOL_MQTT	//�����ϱ�Э�飨mqt/ep212/104��
#define PT_CT									NO				//�����ϱ������
#define UART_BUFF_MAX_LEN 						(512+30)
#define POWERON_READRPARA_NUM					eBIT_USEDCHANNEL	//�ϵ糭�� ��ֹ����

// ��������/***********************************************************************/
#define TCP_TOTAL_NUM							(MAX_PRIVATE_SERVER_NUM+MAX_USER_SERVER_NUM)//�ܵ�tcp�����Ӹ��� ά����+�û���

#define USER_SERVER_LIGHT						TCP_USER		//Զ�����ߵ�ȡ����Ҫ���ĸ���վ
	//TCP Э��
#define MAX_PRIVATE_SERVER_NUM					2				//��͸��tcp���������� ��tcp���������8����Ĭ��0����ά������վ�������û�����վ������Ҳ��͸��������Ӧ�궨�������
#define MAX_USER_SERVER_NUM						0				//�û���tcp����������
#define	TCP_USER								NO				//tcp �����û�����վ
#define TCP_COMM_MODULE							YES				//tcp ��ģ��ͨ�� 
	//MQTT Э��
#define MQTT_USER								NO				//mqtt �����û�����վ ע ����mqtt�û���ʱ Ҫ���� CYCLE_REPORT_PROTOCAL �ϱ���ʽΪMQTT��ʽ
#define	EVENT_REPORT							NO				//�¼��ϱ�����
	//212����Э��


// IO�ܽſ���/***********************************************************************/
#define FLASH_PIN 								0				//FLASH ���� �ܽ��õ�
#define REMOTE_LIGHT							9				//�����̵�Ĭ�Ϲرգ����ߺ��
#define REMOTE_LIGHT_OPEN						nwy_gpio_set_value(REMOTE_LIGHT,1);
#define REMOTE_LIGHT_CLOSE						nwy_gpio_set_value(REMOTE_LIGHT,0);
#define	RUN_LIGHT								28				//���е�  ����ʱ1HZ��˸
#define	LOCAL_LIGHT								27				//���ص�  �ϵ糭��ɹ�����
#define POWER_DOWN_VOLTAGE						1200			//������ֵ �ɲ��ģ�� ֵΪ1200 ����ģ��2500 

//����
#define CHIP_415_UPGRADE						0x415645		//645 ���� ��F415оƬ��������
#define BASEMETER_UPGRADES						0x5AA55AA5		//��������
#define	DOWNLOAD_METERFILE						0xA55AA55A		//���ػ����ļ�
#define FILENAME_LEM							128



#define READ									0
#define WRITE									1
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct {
	BYTE Messagetype;
	BYTE *Buf;
}UartSendMessage;

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
	nwy_ftp_login_t ftp_param;
	char filename[FILENAME_LEM];
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
	eBIT_VERSION,
	eBIT_OPEN_FACTORY,
	eBIT_TIME,
	eBIT_USEDCHANNEL,	//����ʹ�û�·��
	eBIT_TCPNET,
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
	#if( EVENT_REPORT ==  YES)//�����¼� ��ʶbitͳһ�Ŵ˴�
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
//-----------------------------------------------
//				��������
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
extern nwy_osiThread_t *FtpFotaUpdate_thread;
extern nwy_osiThread_t *location_thread;
extern nwy_sim_result_type imei;
extern nwy_osiThread_t *g_app_thread;
extern nwy_osiThread_t *ftp_thread;
extern nwy_osiThread_t *uart_thread;
extern nwy_osiThread_t *factorypara_thread;
//��ʱ�� 
extern nwy_osiTimer_t *g_timer;
extern nwy_osiTimer_t *general_timer;
extern nwy_osiTimer_t *User_MQTT_Reconnect_timer;
extern nwy_osiTimer_t *meter_upgrade_timer;
extern nwy_osiTimer_t *ftp_timer;
extern nwy_osiTimer_t *uart_timer;
extern nwy_osiTimer_t *Ble_Recv_ByteTimeout_timer;
extern nwy_osiTimer_t * Uart_Recv_timer;
#if (TCP_COMM_MODULE == YES)
extern nwy_osiTimer_t *Tcp_ComModule_timer;
#endif
extern char PrintLogSign;
extern int BleLogSwitch ;//������ӡlog����
extern DWORD ThreadRunCnt[eMaxThreadNum];
//-----------------------------------------------
// 				��������
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
void nwy_ftp_timer_cb(void *type);
void nwy_meter_upgrade_timer_cb(void *type);
void  api_ReadSystemFiletoRam( eREAD_METER_BIT eBit );
void  Location_Task( void *param );
BOOL get_gps_position_info(char *c_data,DWORD *addr);
void TCP_User_Task(void *param);
void TCP_PrivateTask(void *param);
void Tcp_send(int Isocket, char *sendbuff, int sendlen, BYTE i);
#if( BLE_WH== YES)
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
#endif
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
