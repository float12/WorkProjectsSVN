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
#define ONLINE_PARA_SECLET						FROM_MODULE		//ip�˿ڵȲ�����ȡλ��

//UART �������/***********************************************************************/
#define READMETER_PROTOCOL						PROTOCOL_645	//���г����Լ
#define MAX_SAMPLE_CHIP 						1				//����4��· ������ĩβ��ַ����Ϊ1������
#define MAX_RATIO								6				//������Ҫ���һ�£���һ��������������� ��+ratio5
#define CYCLE_METER_READING						PROTOCOL_698	//ģ�����ڳ���
#define CYCLE_REPORT_PROTOCAL					PROTOCOL_MQTT	//�����ϱ�Э�飨mqt/ep212/104��
#define PT_CT									NO				//�����ϱ������
#define UART_BUFF_MAX_LEN 						(512+30)
#define POWERON_READRPARA_NUM					eBIT_USEDCHANNEL	//�ϵ糭�� ��ֹ����

// ��������/***********************************************************************/
//N58 socket��ഴ��8��  ��2������mqttά����ftp���� TCPά����ʹ��һ��,TCP�û������ʹ��5��
#define TCP_TOTAL_NUM							(MAX_PRIVATE_SERVER_NUM+MAX_USER_TRANS_NUM+MAX_SPECIAL_USER_NUM)//�ܵ�tcp�����Ӹ��� 
#define USER_SERVER_LIGHT						TCP_USER		//Զ�����ߵ�ȡ����Ҫ���ĸ���վ
	//TCP Э��
#define MAX_PRIVATE_SERVER_NUM					1				//ά����tcp���������� ��tcp���������8����Ĭ��0����ά������վ��
#define MAX_USER_TRANS_NUM						0				//�û���ת������	  (�����û�����վ������Ҳ��ת����վ���ݣ���Ӧ�궨�������
#define MAX_USER_SPECIAL_NUM					5				//�û������⹦��
#define USER_SPECIAL_START						(MAX_PRIVATE_SERVER_NUM + MAX_USER_TRANS_NUM)		
#define MAX_SPECIAL_USER_NUM					5

#define	TCP_USER								NO				//tcp �����û�����վ
#define TCP_COMM_MODULE							YES				//tcp ��ģ��ͨ�� 
	//MQTT Э��
#define MQTT_USER								YES				//mqtt �����û�����վ ע ����mqtt�û���ʱ Ҫ���� CYCLE_REPORT_PROTOCAL �ϱ���ʽΪMQTT��ʽ
#define	EVENT_REPORT							NO				//�¼��ϱ�����
	//212����Э��


// IO�ܽſ���/***********************************************************************/
#define FLASH_PIN_N58 							3				//cvflash N58 �ɲ�� flash ���Ժ���
#define FLASH_PIN_CTRLCOST						17				//cvflash N58 �ѿر�
#define FLASH_PIN 								0				//FLASH ���� �ܽ��õ�
#define REMOTE_LIGHT							9				//�����̵�Ĭ�Ϲرգ����ߺ��
#define REMOTE_LIGHT_OPEN						nwy_gpio_set_value(REMOTE_LIGHT,1);
#define REMOTE_LIGHT_CLOSE						nwy_gpio_set_value(REMOTE_LIGHT,0);
#define	RUN_LIGHT								28				//���е�  ����ʱ1HZ��˸
#define	LOCAL_LIGHT								27				//���ص�  �ϵ糭��ɹ�����
#define POWER_DOWN_VOLTAGE						1200			//������ֵ N58�ɲ��ģ�� ֵΪ1200 ����ģ��2500 

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
	BYTE buf[NWY_UART_RECV_SINGLE_MAX+1];
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
	eBIT_VERSION,
	eBIT_RealTime_698,
	eBIT_Freeze_698,
	eBIT_USEDCHANNEL,	//����ʹ�û�·��	
	eBIT_TCP_USER_PARA,	
	eBIT_TCPNET,
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

typedef enum
{
	eMETER = 0,
	eMODULE
}MeterorModu;

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
#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
extern nwy_osiMessageQueue_t *Ep212ToUartMsgQue;
extern nwy_osiMessageQueue_t *UartToEp212MsgQue;
#endif
extern nwy_osiMessageQueue_t *UserTcpStatusChangeMsgQue;//tcp �û���״̬�ı�֪ͨ
extern nwy_osiMessageQueue_t *MQTTUserToUartMsgQue;//�̵���״̬�ı�ԭ��֪ͨ
extern nwy_osiMessageQueue_t *UartReplyToMqttMsgQue;

//�߳�
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
extern nwy_osiTimer_t *FactoryRecv_ByteTimeout_timer;
extern nwy_osiTimer_t *Uart_Recv_timer;
extern nwy_osiTimer_t *Check_Relay_Status_Timer;
#if (TCP_COMM_MODULE == YES)
extern nwy_osiTimer_t *Tcp_ComModule_timer;
#endif
extern char PrintLogSign;
extern int BleLogSwitch ;//������ӡlog����
extern DWORD ThreadRunCnt[eMaxThreadNum];

extern nwy_paho_mqtt_at_param_type paho_mqtt_user_param;
//-----------------------------------------------
// 				��������
//-----------------------------------------------
int nwy_ext_check_data_connect();
void WSD_MQTT_Task(void* param);
#if(MQTT_USER == YES)
void USER_MQTT_Task(void *param);
#endif
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
void factory_Recv_timer_cb(void *type);
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
