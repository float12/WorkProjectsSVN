//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з��� 
//������	
//��������	
//����		MQTT_USERͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __MQTT_USER_H
#define __MQTT_USER_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eFreezeData,
	eRealTimeData,
	eRelayStatusData,
    eReadTimeData,
}eUartToMqttType;
typedef union 
{
    double* FreezeDataAddr;
    double* RealTimeDataAddr;
    BYTE RelayStatusData[METER_PHASE_NUM];
    TRealTimer ReadTimeData; //��ȡʱ������
}uUartToMqttData;

typedef struct
{
    eUartToMqttType Type;
    uUartToMqttData Data;
}TUartToMqttData;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern MQTTClient paho_user_client;
//-----------------------------------------------
// 				��������
//-----------------------------------------------
void UsermessageArrived(MessageData *md);
void  SubMqttTopic_user( MQTTClient *UserClient );
//--------------------------------------------------
//��������:  ����mqtt�û�����վ����������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  MqttRecvfromUser( void );
//--------------------------------------------------
//��������:  �ظ�mqtt�û��˷���������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  MqttRepytoUser( MQTTClient *UserClient );
#endif //#ifndef __MQTT_USER_H
