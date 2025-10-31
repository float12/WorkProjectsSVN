//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		MQTT_USER头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __MQTT_USER_H
#define __MQTT_USER_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
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
    TRealTimer ReadTimeData; //读取时间数据
}uUartToMqttData;

typedef struct
{
    eUartToMqttType Type;
    uUartToMqttData Data;
}TUartToMqttData;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern MQTTClient paho_user_client;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void UsermessageArrived(MessageData *md);
void  SubMqttTopic_user( MQTTClient *UserClient );
//--------------------------------------------------
//功能描述:  接受mqtt用户端主站发来的数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  MqttRecvfromUser( void );
//--------------------------------------------------
//功能描述:  回复mqtt用户端发来的数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  MqttRepytoUser( MQTTClient *UserClient );
#endif //#ifndef __MQTT_USER_H
