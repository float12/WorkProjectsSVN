//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------
//���Ƿ�������ȫ�������Ƕ�������ȫ��
static char device_user_topics[20][64];
// //ͨ�÷���������
// static char user_pub_topic[64] = "/devices/HY_pub";
// //ͨ�ö���������
// static char user_sub_topic[64] = "/devices/Test_sub";
//��������β
static char user_pub_topics_tail[][64] = 
{
	"/reportHeart",
	"/getParaAck",
	"/setParaAck",
	"/getFreezeAck",
	"/upGradeAck",
	"/getRealTimeDataAck",
};
//��������β��topics��˳��
typedef enum
{
	eReportHeart,
	eGetParaAck,
	eSetParaAck,
	eGetFreezeAck,
	eUpGradeAck,
	eGetRealTimeDataAck,
}eUserPubTopicIndex;
// ��������β
static char user_sub_topics_tail[][64] = 
{
	"/getPara",
	"/setPara",
	"/upGrade",
	"/getRealTimeData",
};

char user_sub_topic_addr[PAHO_TOPIC_LEN_MAX+1];
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
// ��������:	�жϼ̵���״̬
//
// ����:
//
// ����ֵ:
//
// ��ע:
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
// ��������:���̵���״̬��ʱ���ص�
//
// ����:
//
// ����ֵ:
//
// ��ע:
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
// ��������:����̵�������
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
static void HandleRelayCommand(const char* cmdStr, cJSON *pRoot, BYTE isOpenCmd)
{
    BYTE channel = 0;
	BYTE channelStr[1] = {0};
	TReadMeterInfo ReadMeterInfo = {0};
	BYTE buf[7] = {0x00,0x18,0x54,0x15,0x09,0x10,0x50};//���Ƽ̵���ʱ�� ��Ҫ���ڵ�ǰʱ�䣬ĿǰΪ2050��
    // ��ȡ channel �ֶ�
	cJSON_GetObjectValue(pRoot, "channel", ecJSON_String, channelStr, sizeof(channelStr));
	channel = atoi((char *)channelStr);
	nwy_ext_echo("\r\n channel:%d", channel);
	// ����RelayCmd
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
	// ״̬����д����
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
// ��������: user��Ϣ���ջص�
//
// ����:
//
// ����ֵ:
//
// ��ע:
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
// ��������: ָ���汾�������������Ϣ�ص�
//
// ����:
//
// ����ֵ:
//
// ��ע:
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
// ��������: user��Ϣ����
//
// ����:
//
// ����ֵ:
//
// ��ע:
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

	// ׼������
	memset(paho_mqtt_user_param.topic, 0, sizeof(paho_mqtt_user_param.topic));
	strncpy(paho_mqtt_user_param.topic, topic_p, strlen(topic_p));

	paho_mqtt_user_param.qos = 0;		  // atoi(sptr);
	paho_mqtt_user_param.retained = 10; //
	// ׼����Ϣ
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
	// ��������
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
// ��������:  ����������־������Ӧ����,Ϊ0������������
//
// ����:
//
// ����ֵ:
//
// ��ע:
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

// ���������� "HH:MM" -> Hour/Minute ��֤
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
//��������: ����value :{"Time":"01:01","TimeSegTable":1},...]
//         
//����:      
//         
//����ֵ:    ����������
//         
//��ע:  
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
//��������:  ����mqtt�û�����վ����������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
	unsigned int time[7] = {0};//�������涨��Ϊint
	int i = 0;
	// �����û���վ��MQTT֡
	if (nwy_get_msg_que(MqttResUserMessageQueue, &msg, 0xffffffff)) // �ȴ�һ��
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
			if (strcmp(pParaItem->valuestring, "TimeZoneNum") == 0)
			{
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				ReadMeterInfo.Standard645ID = TIME_ZONE_NUM;
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegTableNum") == 0)
			{
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				ReadMeterInfo.Standard645ID = TIME_SEG_TABLE_NUM;
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegNum") == 0)
			{
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				ReadMeterInfo.Standard645ID = TIME_SEG_NUM;
			}
			else if (strcmp(pParaItem->valuestring, "RatioNum") == 0)
			{
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				ReadMeterInfo.Standard645ID = RATIO_NUM;
			}
			else if (strcmp(pParaItem->valuestring, "TTimeAreaTable") == 0)
			{
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				ReadMeterInfo.Standard645ID = TTIME_AREA_TABLE;
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegTable") == 0)
			{
				ReadMeterInfo.Type = eREAD_METER_STANDARD;
				for (i = 0; i < MAX_TIME_SEGTABLE_NUM; i++)
				{
					ReadMeterInfo.Standard645ID = TIME_SEG_TABLE_DAY1 + i;
					nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
				}
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
				
				SafeMem.ReportPara.reportfre = (WORD)pValue->valuedouble;
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
				// IP ��ַ
				tmp = cJSON_GetObjectItem(pValue, "ip");
				strncpy(ServerPara.ServerIP, tmp->valuestring, strlen(tmp->valuestring) + 1);
				nwy_ext_echo("\r\nuser_ip: %s", ServerPara.ServerIP);
				// �˿�
				tmp = cJSON_GetObjectItem(pValue, "port");
				ServerPara.ServerPort = (WORD)tmp->valuedouble;
				nwy_ext_echo("\r\nuser_port: %d", ServerPara.ServerPort);
				// �û���
				tmp = cJSON_GetObjectItem(pValue, "user");
				strncpy(ServerPara.ServerUserName, tmp->valuestring, strlen(tmp->valuestring) + 1);
				nwy_ext_echo("\r\nServerUserName: %s", ServerPara.ServerUserName);
				// ����
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
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegTableNum") == 0)
			{
				ReadMeterInfo.Data.TimeSegTableNum = (BYTE)pValue->valueint;
				ReadMeterInfo.Standard645ID = TIME_SEG_TABLE_NUM;
				ReadMeterInfo.DataLen = 1;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
			}
			else if (strcmp(pParaItem->valuestring, "TimeSegNum") == 0)
			{
				ReadMeterInfo.Data.TimeSegTableNum = (BYTE)pValue->valueint;
				ReadMeterInfo.Standard645ID = TIME_SEG_NUM;
				ReadMeterInfo.DataLen = 1;
				nwy_put_msg_que(MQTTUserToUartMsgQue, &ReadMeterInfo, 0xffffffff);
			}
			else if (strcmp(pParaItem->valuestring, "RatioNum") == 0)
			{
				ReadMeterInfo.Data.RatioNum = (BYTE)pValue->valueint;
				ReadMeterInfo.Standard645ID = RATIO_NUM;
				ReadMeterInfo.DataLen = 1;
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
					ReadMeterInfo.Standard645ID = TTIME_AREA_TABLE;
					ReadMeterInfo.DataLen = cnt * 3;
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
						// ȷ��date����
						if ((date <= 0) || (date > MAX_TIME_SEGTABLE_NUM)) // MAX_TIME_SEG_DAY_NUM��Ҫ����Ϊ֧�ֵ��������
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
							nwy_ext_echo("\r\n put msg set TimeSegTable:%d", ReadMeterInfo.Standard645ID);
						}
						else
						{
							nwy_ext_echo("\r\n put msg set TimeSegTable failed:%d", ReadMeterInfo.Standard645ID);
						}
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
				ReadMeterInfo.Data.SetTime[i] = (BYTE)(time[i] & 0xFF);
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
		{ // ����
			cJSON_GetObjectValue(pRoot, "ftp_name", ecJSON_String, ftpupdatepara.filename, sizeof(ftpupdatepara.filename));
			cJSON_GetObjectValue(pRoot, "ftp_ip", ecJSON_String, ftpupdatepara.ftp_param.host, sizeof(ftpupdatepara.ftp_param.host));
			cJSON_GetObjectValue(pRoot, "ftp_port", ecJSON_Int, &GetTemp, sizeof(GetTemp));
			ftpupdatepara.ftp_param.port = GetTemp;
			cJSON_GetObjectValue(pRoot, "ftp_user", ecJSON_String, ftpupdatepara.ftp_param.username, sizeof(ftpupdatepara.ftp_param.username));
			cJSON_GetObjectValue(pRoot, "ftp_pass", ecJSON_String, ftpupdatepara.ftp_param.passwd, sizeof(ftpupdatepara.ftp_param.passwd));

			// ׷����������ʶ���Ƿ���������
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
						tIap_645.dwChip = CHIP_415_UPGRADE; // �쳣 ʱ ��������Ϻ�  ȫ������
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
		cJSON_Delete(pRoot);
		nwy_ext_echo("\r\nfree");
		free(msg);
	}
}
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
char RealTimeDataSign[][10]	=
{
	{"HZ" 	  },		//Ƶ��
	{"UA" 	  },		//A���ѹ
	{"UB" 	  },		//B���ѹ
	{"UC" 	  },		//C���ѹ
	{"LUAB"	  },		//AB���ѹ
	{"LUBC"	  },		//BC���ѹ
	{"LUCA"	  },		//CA���ѹ
	{"IA" 	  },		//A�����
	{"IB" 	  },		//B�����
	{"IC" 	  },		//C�����
	{"PT" 	  },		//���й�
	{"PA" 	  },		//A���й�����
	{"PB" 	  },		//B���й�����
	{"PC" 	  },		//C���й�����
	{"QT" 	  },		//���޹�	
	{"QA" 	  },		//A���޹�����
	{"QB" 	  },		//B���޹�����
	{"QC" 	  },		//C���޹�����
	{"ST" 	  },		//�����ڹ���	
	{"SA" 	  },		//A�����ڹ���
	{"SB" 	  },		//B�����ڹ���
	{"SC" 	  },		//C�����ڹ���
	{"PFT"	  },		//�ܹ�������	
	{"PFA"	  },		//A�๦������
	{"PFB"	  },		//B�๦������
	{"PFC"	  },		//C�๦������
	{"EPT"	  },		//�����й�������	
	{"EP1"	  },		//�����й����ܷ���1
	{"EP2"	  },		//�����й����ܷ���2
	{"EP3"	  },		//�����й����ܷ���3
	{"EP4"	  },		//�����й����ܷ���4
	{"EP5"	  },		//�����й����ܷ���5
	{"ENT"	  },		//�����й�������	
	{"EN1"	  },		//�����й����ܷ���1
	{"EN2"	  },		//�����й����ܷ���2
	{"EN3"	  },		//�����й����ܷ���3
	{"EN4"	  },		//�����й����ܷ���4
	{"EN5"	  },		//�����й����ܷ���5
	{"ERC1"	  },		//����޹�1�ܵ���	
	{"ERC2"	  },		//����޹�2�ܵ���	
	{"QD_TIME"},		//����ʱ��
	{"QD_MAX" },		//�������
};
		
void  pub_realtimedatatoUser( MQTTClient *UserClient ,double* dataAddr )
{
	cJSON *pJsonRoot,*ds;
	char *c_message;
	MQTTMessage pubmsg = {0};
	TRealTimer tTime = {0};
	BYTE i;

	nwy_ext_echo("\r\n realtime success recved");
	get_N176_time(&tTime);
	memset(paho_mqtt_user_param.topic, 0, sizeof(paho_mqtt_user_param.topic));
	strncpy(paho_mqtt_user_param.topic, &device_user_topics[eGetRealTimeDataAck][0], strlen(device_user_topics[eGetRealTimeDataAck]));
	paho_mqtt_user_param.qos = 1;
	paho_mqtt_user_param.retained = 10;
	pJsonRoot = cJSON_CreateObject();
	ds = cJSON_CreateObject();
	if (NULL == pJsonRoot)
		return;

	cJSON_AddNumberToObject(pJsonRoot, "T", getmktimems(&tTime));
	cJSON_AddStringToObject(pJsonRoot, "type", "CMD_DEVICE_REALTIME_DATA");
	for (i = 0; i < sizeof(RealTimeDataSign)/sizeof(RealTimeDataSign[10]); i++)
	{
		nwy_ext_echo("\r\n CMD_DEVICE_REALTIME_DATA value is [%f]",*dataAddr);
		cJSON_AddNumberToObject(ds, &RealTimeDataSign[i][0], *dataAddr);
		dataAddr++;
	}	
	cJSON_AddItemToObject(pJsonRoot, "DS", ds);
	c_message = cJSON_PrintUnformatted(pJsonRoot);
	// ��������
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
//��������:  
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  pub_freezedatatoUser( MQTTClient *UserClient ,double* dataAddr )
{
	cJSON *pJsonRoot;
	char *c_message;
	MQTTMessage pubmsg = {0};
	TRealTimer tTime = {0};
	BYTE i;

	nwy_ext_echo("\r\n realtime success recved");
	get_N176_time(&tTime);
	memset(paho_mqtt_user_param.topic, 0, sizeof(paho_mqtt_user_param.topic));
	strncpy(paho_mqtt_user_param.topic, &device_user_topics[eGetFreezeAck][0], strlen(device_user_topics[eGetFreezeAck]));
	paho_mqtt_user_param.qos = 1;
	paho_mqtt_user_param.retained = 10;
	pJsonRoot = cJSON_CreateObject();
	if (NULL == pJsonRoot)
		return;

	cJSON_AddNumberToObject(pJsonRoot, "T", getmktimems(&tTime));
	cJSON_AddStringToObject(pJsonRoot, "type", "CMD_DEVICE_FREEZE");
	for (i = 0; i < sizeof(RealTimeDataSign)/sizeof(RealTimeDataSign[10]); i++)
	{
		nwy_ext_echo("\r\n CMD_DEVICE_REALTIME_DATA value is [%f]",*dataAddr);
		cJSON_AddNumberToObject(pJsonRoot, &RealTimeDataSign[i][0], *dataAddr);
		dataAddr++;
	}	
	c_message = cJSON_PrintUnformatted(pJsonRoot);
	// ��������
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
//��������:  �ظ�mqtt�û��˷���������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  MqttRepytoUser( MQTTClient *UserClient )
{
	TUartToMqttData UartToMqttData = {0};
	char  timeStr[64] = {0};

	if (nwy_get_msg_que(UartReplyToMqttMsgQue, &UartToMqttData, 0xffffffff)) // ��MQTT֡���з���
	{
		switch (UartToMqttData.Type)
		{
			case eRealTimeData:
				pub_realtimedatatoUser(UserClient,UartToMqttData.Data.RealTimeDataAddr);
				break;
			case eFreezeData:
				pub_freezedatatoUser(UserClient,UartToMqttData.Data.FreezeDataAddr);
				break;
			case eRelayStatusData:
				UserRemessageF("CMD_DEVICE_SETTIME", &device_user_topics[eSetParaAck][0], "s", "Result", "success");
				break;
			case eReadTimeData:
				sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d", UartToMqttData.Data.ReadTimeData.wYear, UartToMqttData.Data.ReadTimeData.Mon, UartToMqttData.Data.ReadTimeData.Day, UartToMqttData.Data.ReadTimeData.Hour, UartToMqttData.Data.ReadTimeData.Min, UartToMqttData.Data.ReadTimeData.Sec);
				UserRemessageF("CMD_DEVICE_GETTIME", &device_user_topics[eGetParaAck][0], "s", "DateTime", timeStr);
				break;
		}
	}

}
//--------------------------------------------------
//��������:  ��ʼ��mqtt�û���Ҫ����������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void InitMqttTopic_user(void)
{
	char c_devic[] = "/devices/";
	int len = 0, i = 0;
	int pub_topic_num = 0;

	memset(&device_user_topics[0][0], 0, sizeof(device_user_topics));
	// topic ��ʼ��
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
//��������:  �����û�������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  �û��˱��뷢����������Ȩ�޲��ܶ��ģ�������ɶ���ʧ�ܵ��¸�λ
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
	//���ĸ��ݰ汾����������
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