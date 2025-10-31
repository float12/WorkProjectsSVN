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
#define BLE_UART_WH								YES				//������uart��Ϊά���˿� �˴�����Ӳ�� ZH �������ϱ��Ŀ���״̬��δ���
#define GPRS_POSITION							NO				//GPRS ����
#define CYCLE_REPORT							NO				//�����ϱ�����	�����ϱ������¼��ϱ�
#define	EVENT_REPORT							NO				//�¼��ϱ�����
#define	TRANS_SWITCH							TCP_TRANS		//͸��Э��
#define PT_CT									NO				//�����ϱ������
#define	TEST_STACK								NO				//��ջ����
#define	TCP_SWITCH								YES				//tcp����
#define FLOW_CALCULATE 							YES				//��������
#define COLLECT_UIP 							YES				//¼��˲ʱ
#define UPLOAD_WAVE_DATA_SWITCH 				NO				//�ϴ���������
// IO�ܽſ���
#define FLASH_PIN 								3				//FLASH ���� �ܽ��õ�
#define REMOTE_LIGHT							9				//�����̵�Ĭ�Ϲرգ����ߺ��
#define	RUN_LIGHT								28				//���е�  ����ʱ1HZ��˸
#define	LOCAL_LIGHT								27				//���ص�  �ϵ糭��ɹ�����

#define SERVER_TRAN								0				//��վ͸��
#define	BLUETOOTH_TRAN							1				//����͸��
#define BASEMETER_UPGRADES						0x5AA55AA5
#define	DOWNLOAD_METERFILE						0xA55AA55A
#define UART_BUFF_MAX_LEN 						(4096+30)
#define POWERON_READRPARA_NUM					eBIT_POSITION

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
	BYTE buf[4096 + 30];
	DWORD len; /* data */
	BYTE TranType;
} tTranData;

// ���ӡ��롢ʱ����bitλö��
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
//				��������
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
extern int BleLogSwitch ;//������ӡlog����
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
#endif //#ifndef __FUNC_DEF_H
