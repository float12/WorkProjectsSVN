//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
//先是发布主题全名，后是订阅主题全名
static char device_user_topics[20][64];
// //通用发布主题名
// static char user_pub_topic[64] = "/devices/HY_pub";
// //通用订阅主题名
// static char user_sub_topic[64] = "/devices/Test_sub";
//发布主题尾
static char user_pub_topics_tail[][64] = 
{
	"/reportHeart",
	"/getParaAck",
	"/setParaAck",
	"/getFreezeAck",
	"/upGradeAck",
	"/getRealTimeDataAck",
};
//发布主题尾在topics的顺序
typedef enum
{
	eReportHeart,
	eGetParaAck,
	eSetParaAck,
	eGetFreezeAck,
	eUpGradeAck,
	eGetRealTimeDataAck,
}eUserPubTopicIndex;
// 订阅主题尾
static char user_sub_topics_tail[][64] = 
{
	"/getPara",
	"/setPara",
	"/upGrade",
	"/getRealTimeData",
};

char user_sub_topic_addr[PAHO_TOPIC_LEN_MAX+1];

char RealTimeDataSign[][10]	=
{
	{"HZ" 	  },		//频率
	{"UA" 	  },		//A相电压
	{"UB" 	  },		//B相电压
	{"UC" 	  },		//C相电压
	{"LUAB"	  },		//AB相电压
	{"LUBC"	  },		//BC相电压
	{"LUCA"	  },		//CA相电压
	{"IA" 	  },		//A相电流
	{"IB" 	  },		//B相电流
	{"IC" 	  },		//C相电流
	{"PT" 	  },		//总有功
	{"PA" 	  },		//A相有功功率
	{"PB" 	  },		//B相有功功率
	{"PC" 	  },		//C相有功功率
	{"QT" 	  },		//总无功	
	{"QA" 	  },		//A相无功功率
	{"QB" 	  },		//B相无功功率
	{"QC" 	  },		//C相无功功率
	{"ST" 	  },		//总视在功率	
	{"SA" 	  },		//A相视在功率
	{"SB" 	  },		//B相视在功率
	{"SC" 	  },		//C相视在功率
	{"PFT"	  },		//总功率因数	
	{"PFA"	  },		//A相功率因数
	{"PFB"	  },		//B相功率因数
	{"PFC"	  },		//C相功率因数
	{"EPT"	  },		//正向有功电能总	
	{"EP1"	  },		//正向有功电能费率1
	{"EP2"	  },		//正向有功电能费率2
	{"EP3"	  },		//正向有功电能费率3
	{"EP4"	  },		//正向有功电能费率4
	{"EP5"	  },		//正向有功电能费率5
	{"ENT"	  },		//反向有功电能总	
	{"EN1"	  },		//反向有功电能费率1
	{"EN2"	  },		//反向有功电能费率2
	{"EN3"	  },		//反向有功电能费率3
	{"EN4"	  },		//反向有功电能费率4
	{"EN5"	  },		//反向有功电能费率5
	{"ERC1"	  },		//组合无功1总电能	
	{"ERC2"	  },		//组合无功2总电能	
	{"QD_TIME"},		//需量时标
	{"QD_MAX" },		//最大需量
};
char FreezeDataSign[][10]	=
{
	{"EPT"	  },		//正向有功电能总	
	// {"EP1"	  },		//正向有功电能费率1
	// {"EP2"	  },		//正向有功电能费率2
	// {"EP3"	  },		//正向有功电能费率3
	// {"EP4"	  },		//正向有功电能费率4
	// {"EP5"	  },		//正向有功电能费率5
	{"ENT"	  },		//反向有功电能总	
	// {"EN1"	  },		//反向有功电能费率1
	// {"EN2"	  },		//反向有功电能费率2
	// {"EN3"	  },		//反向有功电能费率3
	// {"EN4"	  },		//反向有功电能费率4
	// {"EN5"	  },		//反向有功电能费率5
	{"ERC1"	  },		//组合无功1总电能	
	{"ERC2"	  },		//组合无功2总电能	
};
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
// 功能描述:	判断继电器状态
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void CheckRelayStatus(BYTE RelayStatus, BYTE *RelayStatusStr)
{
	if(RelayStatus == 0x00)
	{
		strcpy((char*)RelayStatusStr, "close");
	}
	else if(RelayStatus == 0x01)
	{
		strcpy((char*)RelayStatusStr, "open");
	}
}
//--------------------------------------------------
// 功能描述:检查继电器状态定时器回调
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void Check_Relay_Status_Timer_cb(void *type)
{
	TReadMeterInfo ReadMeterInfo = {0};

	ReadMeterInfo.Type = eREAD_METER_EXTENDED;
	ReadMeterInfo.Extended645ID = READ_RELAY_STATUS;
	ReadMeterInfo.Control = READ_METER_CONTROL_BYTE;
	ReadMeterInfo.DataLen = 0;
	nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
}
//--------------------------------------------------
// 功能描述:处理继电器命令
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
static void HandleRelayCommand(const char* cmdStr, cJSON *pRoot, BYTE isOpenCmd)
{
    BYTE channel = 0;
	BYTE channelStr[1] = {0};
	TReadMeterInfo ReadMeterInfo = {0};
	BYTE buf[7] = {0x00,0x18,0x54,0x15,0x09,0x10,0x50};//控制继电器时间 需要大于当前时间，目前为2050年
    // 获取 channel 字段
	cJSON_GetObjectValue(pRoot, "channel", ecJSON_String, channelStr, sizeof(channelStr));
	channel = atoi((char *)channelStr);
	nwy_ext_echo("\r\n channel:%d", channel);
	// 设置RelayCmd
	if (isOpenCmd)
	{
		if (channel == 1)
		{
			ReadMeterInfo.Data.RelayCmdData[0] = eOPEN_RELAY_LOOP1;
		}
		else if (channel == 2)
		{
			ReadMeterInfo.Data.RelayCmdData[0] = eOPEN_RELAY_LOOP2;
		}
		else if (channel == 3)
		{
			ReadMeterInfo.Data.RelayCmdData[0] = eOPEN_RELAY_LOOP3;
		}
	}
	else
	{
		if (channel == 1)
		{
			ReadMeterInfo.Data.RelayCmdData[0] = eCLOSE_RELAY_LOOP1;
		}
		else if (channel == 2)
		{
			ReadMeterInfo.Data.RelayCmdData[0] = eCLOSE_RELAY_LOOP2;
		}
		else if (channel == 3)
		{
			ReadMeterInfo.Data.RelayCmdData[0] = eCLOSE_RELAY_LOOP3;
		}
	}
	memcpy(&ReadMeterInfo.Data.RelayCmdData[1], buf, sizeof(buf));
	// 状态设置写队列
	ReadMeterInfo.Type = eSET_METER_STANDARD;
	ReadMeterInfo.DataLen = sizeof(ReadMeterInfo.Data.RelayCmdData);
	ReadMeterInfo.Control = CONTROL_RELAY_CONTROL_BYTE;
    if(!nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff))
    {
        nwy_ext_echo("\r\n put mqtt msg que failed");
		return;
    }
	nwy_start_timer(Check_Relay_Status_Timer, 4000);
}
//--------------------------------------------------
// 功能描述: user消息接收回调
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void UsermessageArrived(MessageData *md)
{
	char *msg = NULL;

	if (MqttResUserMessageQueue == NULL)
	{
		nwy_ext_echo("Queue==NULL");
		return;
	}
	if (!nwy_get_queue_spaceevent_cnt(MqttResUserMessageQueue))
	{
		nwy_ext_echo("NEED_SPACE");
		return;
	}
	msg = (char *)malloc(md->message->payloadlen);
	if (msg == NULL)
	{
		nwy_ext_echo("NEED_RAM");
		return;
	}
	memset(msg, 0, md->message->payloadlen);
	memcpy(msg, md->message->payload, md->message->payloadlen);
	// msg[md->message->payloadlen] = '\0';
	if (!nwy_put_msg_que(MqttResUserMessageQueue, &msg, 0xffffffff))
	{
		free(msg);
	}
}
//--------------------------------------------------
// 功能描述: 指定版本升级主题接收消息回调
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void VersionUpgradeMessageArr(MessageData *md)
{
	char *msg = NULL;

	if (MqttResUserMessageQueue == NULL)
	{
		nwy_ext_echo("Queue==NULL");
		return;
	}
	if (!nwy_get_queue_spaceevent_cnt(MqttResUserMessageQueue))
	{
		nwy_ext_echo("NEED_SPACE");
		return;
	}
	msg = (char *)malloc(md->message->payloadlen);
	if (msg == NULL)
	{
		nwy_ext_echo("NEED_RAM");
		return;
	}
	memset(msg, 0, md->message->payloadlen);
	memcpy(msg, md->message->payload, md->message->payloadlen);
	// msg[md->message->payloadlen] = '\0';
	if (!nwy_put_msg_que(MqttResUserMessageQueue, &msg, 0xffffffff))
	{
		free(msg);
	}
}
//--------------------------------------------------
// 功能描述: user消息发布
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BYTE UserRemessageF(const char *type, char *topic_p, char *fmt, ...)
{
	va_list args;
	if(fmt != NULL)
	{
		va_start(args, fmt);
	}
	const char *fs;
	MQTTMessage pubmsg = {0};
	char *c_message;
	cJSON *pJsonRoot;
	TRealTimer tTime = {0};

	// 准备主题
	memset(paho_mqtt_user_param.topic, 0, sizeof(paho_mqtt_user_param.topic));
	strncpy(paho_mqtt_user_param.topic, topic_p, strlen(topic_p));

	paho_mqtt_user_param.qos = 0;		  // atoi(sptr);
	paho_mqtt_user_param.retained = 10; //
	// 准备消息
	pJsonRoot = cJSON_CreateObject();
	if (NULL == pJsonRoot)
		return FALSE;
	get_N176_time(&tTime);
	cJSON_AddStringToObject(pJsonRoot, "type", type);
	while ((fmt != NULL) && (*fmt != '\0'))
	{
		if (*fmt == 's')
		{
			fs = va_arg(args, const char *const);
			cJSON_AddStringToObject(pJsonRoot, fs, va_arg(args, const char *const));
		}
		else if (*fmt == 'd')
		{
			fs = va_arg(args, const char *const);
			cJSON_AddNumberToObject(pJsonRoot, fs, va_arg(args, double));
		}
		++fmt;
	}
	if(fmt != NULL)
	{
		va_end(args);
	}
	cJSON_AddNumberToObject(pJsonRoot, "T", getmktimems(&tTime));
	c_message = cJSON_PrintUnformatted(pJsonRoot);
	// 发布部分
	memset(paho_mqtt_user_param.message, 0, sizeof(paho_mqtt_user_param.message));
	strncpy(paho_mqtt_user_param.message, c_message, strlen(c_message));
	nwy_ext_echo("\r\nmqttpub param retained = %d, qos = %d, topic = %s, msg = %s", paho_mqtt_user_param.retained, paho_mqtt_user_param.qos, paho_mqtt_user_param.topic,
				 paho_mqtt_user_param.message);
	memset(&pubmsg, 0, sizeof(pubmsg));
	pubmsg.payload = (void *)paho_mqtt_user_param.message;
	pubmsg.payloadlen = strlen(paho_mqtt_user_param.message);
	pubmsg.qos = paho_mqtt_user_param.qos;
	pubmsg.retained = paho_mqtt_user_param.retained;
	pubmsg.dup = 0;
	int rc = nwy_MQTTPublish(&paho_user_client, paho_mqtt_user_param.topic, &pubmsg);
	if (rc)
	{
		nwy_ext_echo("\r\n publish failed,rc=%d", rc);
		free(c_message);
		cJSON_Delete(pJsonRoot);
		return FALSE;
	}
	nwy_sleep(1000);
	free(c_message);
	cJSON_Delete(pJsonRoot);
	return TRUE;
}

//--------------------------------------------------
// 功能描述:  根据升级标志进行相应处理,为0进入升级流程
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void UserStartUpgradeRemessage(FtpUpdatePara *ftpupdatepara)
{
	UserRemessageF("CMD_DEVICE_UPGRADE",&device_user_topics[eUpGradeAck][0], "s", "Result", "start Upgrade");
	if (nwy_put_msg_que(FtpUpdateMessageQueue, ftpupdatepara, 0xffffffff))
	{
		nwy_ext_echo("\r\n put message success");
	}
	else
	{
		nwy_ext_echo("\r\n put message fail");
	}
}

// 辅助：解析 "HH:MM" -> Hour/Minute 验证
static BYTE ParseTime_hm(const char *ts, BYTE *hour, BYTE *minute)
{
	DWORD h = 0, m = 0;
	if (!ts)
	{
		nwy_ext_echo("\r\n ParseTime_hm failed1:%s", ts);
		return FALSE;
	}
	if (sscanf(ts, "%02x:%2x", (unsigned int *)&h, (unsigned int *)&m) != 2)
	{
		nwy_ext_echo("\r\n ParseTime_hm failed2:%s", ts);
		return FALSE;
	}
	if ((h < 0) || (h > 0x23) || (m < 0) || (m > 0x59))
	{
		nwy_ext_echo("\r\n ParseTime_hm failed3:%s", ts);
		return FALSE;
	}
	*hour = (BYTE)h;
	*minute = (BYTE)m;
	nwy_ext_echo("\r\n ParseTime_hm: %s -> %02x:%02x", ts, h, m);
	return TRUE;
}
//--------------------------------------------------
//功能描述: 解析value :{"Time":"01:01","TimeSegTable":1},...]
//         
//参数:      
//         
//返回值:    解析的项数
//         
//备注:  
//--------------------------------------------------
static BYTE ParseTableItem(char* ItemName,cJSON *value, BYTE *data)
{
	cJSON *item = NULL;
	cJSON *pTime = NULL;
	cJSON *pSegTable = NULL;
	BYTE cnt = 0;
	BYTE hour = 0;
	BYTE minute = 0;
	if (!cJSON_IsArray(value))
	{
		return 0;
	}

	cJSON_ArrayForEach(item, value)
	{
		pTime = cJSON_GetObjectItem(item, "Time");
		pSegTable = cJSON_GetObjectItem(item, ItemName);
		if ((!cJSON_IsString(pTime)) || (!cJSON_IsNumber(pSegTable)))
		{
			nwy_ext_echo("\r\n ParseTableItem failed:%s", ItemName);
			continue;
		}
		ParseTime_hm(pTime->valuestring, &hour, &minute);
		*data++ = (BYTE)pSegTable->valuedouble;
		*data++ = minute;
		*data++ = hour;
		cnt++;
	}
	return cnt;
}
//--------------------------------------------------
// 功能描述:费率数据json组包
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
//
cJSON* TimeAreaTable_to_json(const char* value_name,TTimeAreaEntry *TimeAreaTable, BYTE bNum)
{
	if (!value_name || !TimeAreaTable) {
		return NULL;
	}
	// 创建数组
	cJSON *array = cJSON_CreateArray();
	if (!array) {
		return NULL;
	}
		
	// 填充数组
	for (int i = 0; i < bNum; i++) {
		cJSON *item = cJSON_CreateObject();
		if (item) {
			cJSON_AddStringToObject(item, "Time", TimeAreaTable[i].Time);
			cJSON_AddNumberToObject(item, value_name, TimeAreaTable[i].TimeSegTableorRate);
			cJSON_AddItemToArray(array, item);
		}
	}
	return array;
}
//--------------------------------------------------
//功能描述:  按不同主题回复用户
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  pub_datatoUser( MQTTClient *UserClient ,double* dataAddr,eUserPubTopicIndex PubTopicIndex,BYTE TimeSegorTimeArea)
{
	cJSON *pJsonRoot,*reply = NULL;
	char *c_message;
	MQTTMessage pubmsg = {0};
	TRealTimer tTime = {0};
	BYTE i;

	nwy_ext_echo("\r\n realtime success recved");
	get_N176_time(&tTime);
	memset(paho_mqtt_user_param.topic, 0, sizeof(paho_mqtt_user_param.topic));
	strncpy(paho_mqtt_user_param.topic, &device_user_topics[PubTopicIndex][0], strlen(device_user_topics[PubTopicIndex]));
	paho_mqtt_user_param.qos = 1;
	paho_mqtt_user_param.retained = 10;

	pJsonRoot = cJSON_CreateObject();
	if (NULL == pJsonRoot)
	{
		return;
	}
	cJSON_AddNumberToObject(pJsonRoot, "T", getmktimems(&tTime));
	if (PubTopicIndex == eGetFreezeAck)
	{	
		
		cJSON_AddStringToObject(pJsonRoot, "type", "CMD_DEVICE_FREEZE");
		for (i = 0; i < sizeof(FreezeDataSign)/sizeof(FreezeDataSign[10]); i++)
		{
			nwy_ext_echo("\r\n CMD_DEVICE_REALTIME_DATA value is [%f]",*dataAddr);
			cJSON_AddNumberToObject(pJsonRoot, &FreezeDataSign[i][0], *dataAddr);
			dataAddr++;
		}	
	}
	else if (PubTopicIndex == eGetRealTimeDataAck)
	{
		cJSON *ds;
		ds = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonRoot, "type", "CMD_DEVICE_REALTIME_DATA");
		for (i = 0; i < sizeof(RealTimeDataSign)/sizeof(RealTimeDataSign[10]); i++)
		{
			nwy_ext_echo("\r\n CMD_DEVICE_REALTIME_DATA value is [%f]",*dataAddr);
			cJSON_AddNumberToObject(ds, &RealTimeDataSign[i][0], *dataAddr);
			dataAddr++;
		}	
		//追加 需量和时间标签
		cJSON_AddItemToObject(pJsonRoot, "DS", ds);
	}
	else if (PubTopicIndex == eGetParaAck)
	{
		cJSON_AddStringToObject(pJsonRoot, "type", "CMD_DEVICE_GET_PARA");

		if (TimeSegorTimeArea == 1)//时区表
		{
			cJSON_AddStringToObject(pJsonRoot, "ParaItem", "TTimeAreaTable");
			reply = TimeAreaTable_to_json("TimeSegTable",RatioPara.TimeAreaTable,RatioPara.TimeZoneNum);
			cJSON_AddItemToObject(pJsonRoot, "value", reply);
		}
		else //时段表
		{
			cJSON_AddStringToObject(pJsonRoot, "ParaItem", "TimeSegTable");
			cJSON *value = cJSON_CreateArray();
			for (BYTE i = 0; i < RatioPara.TimeSegTableNum; i++)
			{
				cJSON *item = cJSON_CreateObject();
				cJSON_AddNumberToObject(item, "Date", RatioPara.TimeSegTables[i].Date);
				reply = TimeAreaTable_to_json("Rate",RatioPara.TimeSegTables[i].Segs,RatioPara.TimeSegNum);
				cJSON_AddItemToObject(item, "SegTable", reply);
				cJSON_AddItemToArray(value, item);
			}
			cJSON_AddItemToObject(pJsonRoot, "value", value);
		}
	}

	c_message = cJSON_PrintUnformatted(pJsonRoot);
	// 发布部分
	memset(paho_mqtt_user_param.message, 0, sizeof(paho_mqtt_user_param.message));
	strncpy(paho_mqtt_user_param.message, c_message, strlen(c_message));
	nwy_ext_echo("\r\nmqttpub param retained = %d, qos = %d, topic = %s, msg = %s", paho_mqtt_user_param.retained, paho_mqtt_user_param.qos, paho_mqtt_user_param.topic,
				 paho_mqtt_user_param.message);
	memset(&pubmsg, 0, sizeof(pubmsg));
	pubmsg.payload = (void *)paho_mqtt_user_param.message;
	pubmsg.payloadlen = strlen(paho_mqtt_user_param.message);
	pubmsg.qos = paho_mqtt_user_param.qos;
	pubmsg.retained = paho_mqtt_user_param.retained;
	pubmsg.dup = 0;
	int rc = nwy_MQTTPublish(UserClient, paho_mqtt_user_param.topic, &pubmsg);
	if (rc)
	{
		cJSON_Delete(pJsonRoot);
		free(c_message);
	}
	cJSON_Delete(pJsonRoot);
	free(c_message);
}
//--------------------------------------------------
//功能描述:  接受mqtt用户端主站发来的数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void MqttRecvfromUser(void)
{
	cJSON *pRoot = NULL, *pType = NULL, *pParaItem = NULL, *pValue = NULL,*pDate = NULL,*pSegTable = NULL;
	cJSON *tmp = NULL;
	char *msg = NULL;
	int GetTemp = 0;
	char APP_BUILD_TIME[65] = {0};
	DWORD addr = 0;
	TReadMeterInfo ReadMeterInfo = {0};
	ServerParaRam_t ServerPara = {0};
	FtpUpdatePara ftpupdatepara = {0};
	TSafeMem SafeMem = {0};
	char reqbuf[512] = {0};
	BYTE TimeStr[50] = {0};
	BYTE cnt = 0;
	cJSON *tbl = NULL;
	int date = 0;
	unsigned int time[7] = {0};//消除警告定义为int
	int i = 0;
	// 接收用户主站的MQTT帧
	if (nwy_get_msg_que(MqttResUserMessageQueue, &msg, 0xffffffff)) // 等待一秒
	{
		nwy_ext_echo("\r\nuser  RECV SUCCESS");
		nwy_ext_echo("\r\nmsg:%s", msg);
		pRoot = cJSON_Parse(msg);
		if (NULL == pRoot)
		{
			free(msg);
			nwy_ext_echo("pRootNULL");
			return;
		}
		pType = cJSON_GetObjectItem(pRoot, "type");
		if (NULL == pType)
		{
			free(msg);
			nwy_ext_echo("pTypeNULL");
			cJSON_Delete(pRoot);
			return;
		}
		nwy_ext_echo("\r\n rx:[%s] \r\n", pType->valuestring);
		if (strstr(pType->valuestring, "CMD_DEVICE_OPEN"))
		{
			HandleRelayCommand("CMD_DEVICE_OPEN", pRoot, 1);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_CLOSE"))
		{
			HandleRelayCommand("CMD_DEVICE_CLOSE", pRoot, 0);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_RELAY_STATUS"))
		{
			ReadMeterInfo.Type = eREAD_METER_EXTENDED;
			ReadMeterInfo.Extended645ID = READ_RELAY_STATUS;
			ReadMeterInfo.Control = READ_METER_CONTROL_BYTE;
			ReadMeterInfo.DataLen = 0;
			nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_GET_PARA"))
		{
			pParaItem = cJSON_GetObjectItem(pRoot, "ParaItem");
			if (strcmp(pParaItem->valuestring, "TimeZoneNum") == 0	)
			{
				UserRemessageF("CMD_DEVICE_GET_PARA", &device_user_topics[eGetParaAck][0], "d", "ParaItem",(double)RatioPara.TimeZoneNum );
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegTableNum") == 0)
			{	
				UserRemessageF("CMD_DEVICE_GET_PARA", &device_user_topics[eGetParaAck][0], "d", "ParaItem",(double)RatioPara.TimeSegTableNum );
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegNum") == 0)
			{
				UserRemessageF("CMD_DEVICE_GET_PARA", &device_user_topics[eGetParaAck][0], "d", "ParaItem",(double)RatioPara.TimeSegNum );
			}
			else if (strcmp(pParaItem->valuestring, "RatioNum") == 0)
			{
				UserRemessageF("CMD_DEVICE_GET_PARA", &device_user_topics[eGetParaAck][0], "d", "ParaItem",(double)RatioPara.RatioNum );	
			}
			else if (strcmp(pParaItem->valuestring, "TTimeAreaTable") == 0)
			{
				pub_datatoUser(&paho_user_client,NULL,eGetParaAck,1);		
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegTable") == 0)
			{
				pub_datatoUser(&paho_user_client,NULL,eGetParaAck,0);		
			}
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_SET_PARA"))
		{
			ReadMeterInfo.Type = eSET_METER_STANDARD;
			ReadMeterInfo.Control = SET_METER_CONTROL_BYTE;
			pParaItem = cJSON_GetObjectItem(pRoot, "ParaItem");
			pValue = cJSON_GetObjectItem(pRoot, "value");
			if (strcmp(pParaItem->valuestring, "ReportDataMode") == 0)
			{
				SafeMem.ReportPara.ReportMode = (BYTE)pValue->valuedouble;
				addr = GET_SAFE_SPACE_ADDR(ReportPara.ReportMode);
				nwy_ext_echo("\r\nset ReportMode:%d", SafeMem.ReportPara.ReportMode);
				api_OperateFileSystem(WRITE, addr, (BYTE *)&SafeMem.ReportPara.ReportMode, sizeof(SafeMem.ReportPara.ReportMode));
			}
			else if (strcmp(pParaItem->valuestring, "CycleDataFre") == 0)
			{
				
				SafeMem.ReportPara.reportfre = (WORD)pValue->valuedouble;//更不更新当前刷新周期
				addr = GET_SAFE_SPACE_ADDR(ReportPara.reportfre);
				nwy_ext_echo("\r\nset CycleDataFre:%d", SafeMem.ReportPara.reportfre);
				api_OperateFileSystem(WRITE, addr, (BYTE *)&SafeMem.ReportPara.reportfre, sizeof(SafeMem.ReportPara.reportfre));
			}
			else if (strcmp(pParaItem->valuestring, "FreezeDataFre") == 0)
			{
				SafeMem.FreezeCycleMin = (WORD)pValue->valuedouble;
				addr = GET_SAFE_SPACE_ADDR(FreezeCycleMin);
				nwy_ext_echo("\r\nFreezeCycleMin:%d", SafeMem.FreezeCycleMin);
				api_OperateFileSystem(WRITE, addr, (BYTE *)&SafeMem.FreezeCycleMin, sizeof(SafeMem.FreezeCycleMin));
			}
			else if (strcmp(pParaItem->valuestring, "MqttPara") == 0)
			{
				// IP 地址
				tmp = cJSON_GetObjectItem(pValue, "ip");
				strncpy(ServerPara.ServerIP, tmp->valuestring, strlen(tmp->valuestring) + 1);
				nwy_ext_echo("\r\nuser_ip: %s", ServerPara.ServerIP);
				// 端口
				tmp = cJSON_GetObjectItem(pValue, "port");
				ServerPara.ServerPort = (WORD)tmp->valuedouble;
				nwy_ext_echo("\r\nuser_port: %d", ServerPara.ServerPort);
				// 用户名
				tmp = cJSON_GetObjectItem(pValue, "user");
				strncpy(ServerPara.ServerUserName, tmp->valuestring, strlen(tmp->valuestring) + 1);
				nwy_ext_echo("\r\nServerUserName: %s", ServerPara.ServerUserName);
				// 密码
				tmp = cJSON_GetObjectItem(pValue, "pass");
				strncpy(ServerPara.ServerUserPwd, tmp->valuestring, strlen(tmp->valuestring) + 1);
				nwy_ext_echo("\r\nServerUserPwd: %s", ServerPara.ServerUserPwd);
				addr = GET_SAFE_SPACE_ADDR(Serverpara[1]);
				api_OperateFileSystem(WRITE, addr, (BYTE *)&ServerPara, sizeof(ServerPara));
			}
			else if (strcmp(pParaItem->valuestring, "TimeZoneNum") == 0)
			{
				ReadMeterInfo.Data.TimeZoneNum = (BYTE)pValue->valueint;
				ReadMeterInfo.Standard645ID = TIME_ZONE_NUM;
				ReadMeterInfo.DataLen = 1;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegTableNum") == 0)
			{
				ReadMeterInfo.Data.TimeSegTableNum = (BYTE)pValue->valueint;
				ReadMeterInfo.Standard645ID = TIME_SEGTABLE_NUM;
				ReadMeterInfo.DataLen = 1;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegNum") == 0)
			{
				ReadMeterInfo.Data.TimeSegTableNum = (BYTE)pValue->valueint;
				ReadMeterInfo.Standard645ID = TIME_SEG_NUM;
				ReadMeterInfo.DataLen = 1;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
			}
			else if (strcmp(pParaItem->valuestring, "RatioNum") == 0)
			{
				ReadMeterInfo.Data.RatioNum = (BYTE)pValue->valueint;
				ReadMeterInfo.Standard645ID = TIME_RATIO;
				ReadMeterInfo.DataLen = 1;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
			}
			else if (strcmp(pParaItem->valuestring, "TTimeAreaTable") == 0)
			{
				cnt = ParseTableItem("TimeSegTable", pValue, ReadMeterInfo.Data.TTimeAreaTable);
				if(cnt > MAX_TIME_ZONE_NUM)
				{
					nwy_ext_echo("\r\n TTimeAreaTable count error:%d", cnt);
				}
				else
				{
					ReadMeterInfo.Standard645ID = TIME_ZONE_TABLE;
					ReadMeterInfo.DataLen = cnt * 3;
					nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
					ReadMeterInfo.Type = eREAD_METER_STANDARD;
					nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
				}
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegTable") == 0)
			{
				if (cJSON_IsArray(pValue))
				{
					cJSON_ArrayForEach(tbl, pValue)
					{
						pDate = cJSON_GetObjectItem(tbl, "Date");
						pSegTable = cJSON_GetObjectItem(tbl, "SegTable");
						if (!cJSON_IsNumber(pDate) || !cJSON_IsArray(pSegTable))
						{
							continue;
						}
						date = pDate->valueint;
						nwy_ext_echo("\r\n date: %d", date);
						// 确保date合理
						if ((date <= 0) || (date > MAX_TIME_SEGTABLE_NUM)) // MAX_TIME_SEG_DAY_NUM需要定义为支持的最大天数
						{
							nwy_ext_echo("\r\n date error:%d", date);
							continue;
						}
						ReadMeterInfo.Standard645ID = TIME_SEG_TABLE_DAY1 + date - 1;
						cnt = ParseTableItem("Rate", pSegTable, ReadMeterInfo.Data.TimeSegTable);
						nwy_ext_echo("\r\n cnt: %d", cnt);
						ReadMeterInfo.DataLen = cnt * 3;
						if(nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff))
						{
							// nwy_ext_echo("\r\n put msg set TimeSegTable:%d", ReadMeterInfo.Standard645ID);
						}
						else
						{
							nwy_ext_echo("\r\n put msg set TimeSegTable failed:%d", ReadMeterInfo.Standard645ID);
						}
						ReadMeterInfo.Type = eREAD_METER_STANDARD;
						nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
					}
				}
			}
			UserRemessageF("CMD_DEVICE_RESET", &device_user_topics[eSetParaAck][0], "s", "Result", "success");
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_RESET"))
		{
			UserRemessageF("CMD_DEVICE_RESET", &device_user_topics[eSetParaAck][0], "s", "Result", "success");
			nwy_sleep(2000);
			nwy_power_off(2);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_GETTIME"))
		{
			ReadMeterInfo.Standard645ID = DAY_TIME;
			ReadMeterInfo.Type = eREAD_METER_STANDARD;
			nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_SETTIME"))
		{
			tmp = cJSON_GetObjectItem(pRoot, "DateTime");
			memcpy(TimeStr, cJSON_GetStringValue(tmp), sizeof(TimeStr));
			nwy_ext_echo("\r\n rec set time:%s", TimeStr);
			if (sscanf((char *)TimeStr, "%02x%02x-%02x-%02x-%02x %02x:%02x:%02x", &time[0],&time[6], &time[5], &time[4], &time[3],&time[2], &time[1], &time[0]) == 8)
			{
				ReadMeterInfo.DataLen = 7;
			}
			for (i = 0; i < 7; ++i)
			{
				ReadMeterInfo.Data.SetorGetTime[i] = (BYTE)(time[i] & 0xFF);
			}
			ReadMeterInfo.Type = eSET_METER_STANDARD;
			ReadMeterInfo.Standard645ID = DAY_TIME;
			ReadMeterInfo.Control = SET_METER_CONTROL_BYTE;
			nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
			UserRemessageF("CMD_DEVICE_SETTIME", &device_user_topics[eSetParaAck][0], "s", "Result", "success");
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_APPVERSION"))
		{
			sprintf(APP_BUILD_TIME, "%s,%s", __DATE__, __TIME__);
			UserRemessageF("CMD_DEVICE_APPVERSION", &device_user_topics[eGetParaAck][0],
						   "ss", "appVersion", APP_BUILD_TIME, "meterVersion", MeterVersion);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_UPGRADE"))
		{ // 升级
			cJSON_GetObjectValue(pRoot, "ftp_name", ecJSON_String, ftpupdatepara.filename, sizeof(ftpupdatepara.filename));
			cJSON_GetObjectValue(pRoot, "ftp_ip", ecJSON_String, ftpupdatepara.ftp_param.host, sizeof(ftpupdatepara.ftp_param.host));
			cJSON_GetObjectValue(pRoot, "ftp_port", ecJSON_Int, &GetTemp, sizeof(GetTemp));
			ftpupdatepara.ftp_param.port = GetTemp;
			cJSON_GetObjectValue(pRoot, "ftp_user", ecJSON_String, ftpupdatepara.ftp_param.username, sizeof(ftpupdatepara.ftp_param.username));
			cJSON_GetObjectValue(pRoot, "ftp_pass", ecJSON_String, ftpupdatepara.ftp_param.passwd, sizeof(ftpupdatepara.ftp_param.passwd));

			// 追加命令用来识别是否给电表升级
			cJSON_GetObjectValue(pRoot, "Mode_switch", ecJSON_String, reqbuf, sizeof(reqbuf));
			if (strcmp(reqbuf, "Meter_upgrade_645") == 0)
			{
				if (tIap_645.dwIapFlag == 0)
				{
					tIap_645.dwIapFlag = DOWNLOAD_METERFILE;
					memset(reqbuf, 0, sizeof(reqbuf));
					cJSON_GetObjectValue(pRoot, "Chip_Type", ecJSON_String, reqbuf, sizeof(reqbuf));
					if (strcmp(reqbuf, "415") == 0)
					{
						tIap_645.dwChip = CHIP_415_UPGRADE; // 异常 时 或升级完毕后  全部清零
						nwy_ext_echo("\r\n recv 415");
					}
					else
					{
						tIap_645.dwChip = 0;
					}
					UserStartUpgradeRemessage(&ftpupdatepara);
				}
				else
				{
					UserRemessageF("CMD_DEVICE_UPGRADE", &device_user_topics[eUpGradeAck][0], "ds", "code", 200.0l, "msg", "645 upgrade in progess");
				}
			}
			else
			{
				if (api_GetModuleUpGradeStatus() == TRUE)
				{
					UserStartUpgradeRemessage(&ftpupdatepara);
				}
				else
				{
					UserRemessageF("CMD_DEVICE_UPGRADE", &device_user_topics[eUpGradeAck][0], "ds", "code", 200.0l, "msg", "4G upgrade in progess");
				}
			}
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_FREEZE"))
		{
			int gtbuf[7] = {0};
			char test[25] = {0};

			cJSON_GetObjectValue(pRoot, "StartTime", ecJSON_String, test, sizeof(test));
			sscanf(test, "%d-%d-%d %d:%d:%d", &gtbuf[1],&gtbuf[2], &gtbuf[3], &gtbuf[4], &gtbuf[5],&gtbuf[6]);
			gtbuf[0] = LHBYTE(gtbuf[1]);
			gtbuf[1] = LLBYTE(gtbuf[1]);

			// nwy_ext_echo("\r\n [%d][%d][%d][%d]",gtbuf[0], gtbuf[1], gtbuf[2], gtbuf[3]);
			ReadMeterInfo.Type = eREAD_METER_FREEZE;
			for (i = 0; i < 7; ++i)
			{
				ReadMeterInfo.Data.SetorGetTime[i] = (BYTE)(gtbuf[i] & 0xFF);
			}
			nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_REALTIME_DATA"))
		{
			
		}
		cJSON_Delete(pRoot);
		nwy_ext_echo("\r\nfree");
		free(msg);
	}
}
//--------------------------------------------------
//功能描述:  回复mqtt用户端发来的数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  MqttRepytoUser( MQTTClient *UserClient )
{
	TUartToMqttData UartToMqttData = {0};
	char  timeStr[64] = {0};

	if (nwy_get_msg_que(UartReplyToMqttMsgQue, &UartToMqttData, 0xffffffff)) // 组MQTT帧进行发送
	{
		switch (UartToMqttData.Type)
		{
			case eRealTimeData:
				pub_datatoUser(UserClient,UartToMqttData.Data.RealTimeDataAddr,eGetRealTimeDataAck,0);
				break;
			case eFreezeData:
				pub_datatoUser(UserClient,UartToMqttData.Data.FreezeDataAddr,eGetFreezeAck,0);
				break;
			case eRelayStatusData:
				UserRemessageF("CMD_DEVICE_SETTIME", &device_user_topics[eGetParaAck][0], "s", "status", UartToMqttData.Data.RelayStatusData);//临时上传，未改完
				break;
			case eReadTimeData:
				sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d", UartToMqttData.Data.ReadTimeData.wYear, UartToMqttData.Data.ReadTimeData.Mon, UartToMqttData.Data.ReadTimeData.Day, UartToMqttData.Data.ReadTimeData.Hour, UartToMqttData.Data.ReadTimeData.Min, UartToMqttData.Data.ReadTimeData.Sec);
				UserRemessageF("CMD_DEVICE_GETTIME", &device_user_topics[eGetParaAck][0], "s", "DateTime", timeStr);
				break;
		}
	}

}
//--------------------------------------------------
//功能描述:  初始化mqtt用户端要交互的主题
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void InitMqttTopic_user(void)
{
	char c_devic[] = "/devices/";
	int len = 0, i = 0;
	int pub_topic_num = 0;

	memset(&device_user_topics[0][0], 0, sizeof(device_user_topics));
	// topic 初始化
	for (i = 0; i < 20; i++)
	{
		memcpy(&device_user_topics[i][0], c_devic, sizeof(c_devic));
		len = strlen(&device_user_topics[i][0]);
		memcpy(&device_user_topics[i][len], LtuAddr, sizeof(LtuAddr));
	}
	len = strlen(&device_user_topics[0][0]);
	pub_topic_num = sizeof(user_pub_topics_tail)/sizeof(user_pub_topics_tail[0]);

	for (i = 0; i < pub_topic_num; i++)
	{
		memcpy(&device_user_topics[i][len], &user_pub_topics_tail[i][0], strlen(user_pub_topics_tail[i]));
	}

	for (i = pub_topic_num; i < pub_topic_num + sizeof(user_sub_topics_tail)/sizeof(user_sub_topics_tail[0]); i++)
	{
		memcpy(&device_user_topics[i][len], &user_sub_topics_tail[i-pub_topic_num][0], strlen(user_sub_topics_tail[i-pub_topic_num]));
	}

	for (i = 0; i < 20; i++)
		nwy_ext_echo("init_user_topic:[%d]%s   \r\n", i, &device_user_topics[i][0]);
}

//--------------------------------------------------
//功能描述:  订阅用户端主题
//         
//参数:      
//         
//返回值:    
//         
//备注:  用户端必须发布或开启订阅权限才能订阅，否测会造成订阅失败导致复位
//--------------------------------------------------
void  SubMqttTopic_user( MQTTClient *UserClient )
{
	int rc;
	BYTE qos = 0;
	char Sub_topic[64] = {0};
	TRealTimer Time = {0};
	char BuilTime[20] = {0};

	InitMqttTopic_user();
	for (int i = 0; i <sizeof(user_sub_topics_tail)/sizeof(user_sub_topics_tail[0]); i++)
	{
		memset(user_sub_topic_addr, 0, sizeof(user_sub_topic_addr));
		memcpy(Sub_topic, &device_user_topics[i + sizeof(user_pub_topics_tail)/sizeof(user_pub_topics_tail[0])][0], 64);
		strncpy(user_sub_topic_addr, Sub_topic, strlen(Sub_topic));
		nwy_ext_echo("\r\n topic_sub_user:[%s][%s]\r\n", user_sub_topic_addr, Sub_topic);
		
		rc = MQTTSubscribe(UserClient, user_sub_topic_addr, qos, UsermessageArrived);
		nwy_ext_echo("\r\n sub status is [%d]",rc);
		if (rc == SUCCESS)
		{
			nwy_ext_echo("\r\n-------------------MQTT_USER Sub ok \r\n ");
		}
		else
			nwy_ext_echo("\r\nMQTT_USER Sub error:%d", rc);
	}
	//订阅根据版本升级的主题
	memset(user_sub_topic_addr, 0, sizeof(user_sub_topic_addr));
	strcat(user_sub_topic_addr,"/devices/version");
	api_GetSoftBuildTime(&Time);
	sprintf((char *)BuilTime, "%04d%02d%02d%02d%02d", Time.wYear, Time.Mon, Time.Day, Time.Hour, Time.Min);
	strcat(user_sub_topic_addr, BuilTime);
	strcat(user_sub_topic_addr, "/upGrade");
	nwy_ext_echo("\r\n version upgrad topic_sub_user:[%s]", user_sub_topic_addr);
	rc = MQTTSubscribe(UserClient, user_sub_topic_addr, qos, VersionUpgradeMessageArr);
	if (rc != SUCCESS)
	{
		nwy_ext_echo("\r\nMQTT_USER Sub error:%d", rc);
	}
}