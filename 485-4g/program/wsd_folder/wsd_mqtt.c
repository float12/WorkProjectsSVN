//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"

#define TIME_MSEC_OFFSET (8 * 60 * 60 * 1000)
#define MAX_DATA_NUM 51+4+12//4个温度12个电能
//采集数据保存相关
#define SAVE_TO_FILE_MAX_ITEM_NUM 200 //按400KB计算,每条1672字节，239条
#define COLLECTED_DATA_FILE_PATH "/collected_data.bin"
#define UPDATE_HEAD_INFO_WRITE_CNT 15 //更新头部信息写入的数据条数
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct
{
	WORD SaveDataNum; //有效数据块数量
	WORD WriteOffset; //下一条写入的偏移序号 在0到最大数据块数量之间
	WORD ReadOffset;  //最老的一条数据块的偏移序号 在0到最大数据块数量之间
	WORD crc16;
} SaveDataInfo;

#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_MQTT)
typedef struct
{
	struct T_MqttData *TMqttData[MAX_DATA_NUM];
} TMqttArrayData;
typedef union {
	char *StringValue;
	double DoubleValue;
	cJSON *CJsonValue;
	TMqttArrayData *MqttDataArray;
	void *Bak;
} MqttDataValue;
typedef enum
{
	eNullValue = 0,
	eStringValue,
	eDoubleValue,
	eCJsonValue,
	eArrayValue,
	eFromCollection,
	eCollectionToData, //数组内包数组嵌套特殊数据
	eGetMeterAddr,	 //数据直接从电表获取
	eGetTimeMs,		   //获取当前时间戳
	eGetImei,		   //模块imei号
	eSelfName,		   //调用上层id
	eSelfValueType,	//调用上层value type
} eMqttDataType;
typedef struct T_MqttData
{
	char *DataName;
	eMqttDataType DataType;
	eValueType ValueType;
	MqttDataValue DataValue;
	QWORD MKTime;
} T_MqttData;
typedef struct TDataTable
{
	void *CJsonFunc;
	T_MqttData MqttData;
	struct TDataTable *const NextTable[MAX_DATA_NUM];
	void *Bak;
} TDataTable;

TDataTable JLXFDataTable = {
	.CJsonFunc = cJSON_CreateObject,
	.NextTable[0] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "clientId",
		.MqttData.DataValue.StringValue = ReportPara.mqttClientld,
	},
	.NextTable[1] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "time",
		.MqttData.DataType = eGetTimeMs,
	},
	.NextTable[2] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "deviceId",
		.MqttData.DataType = eGetMeterAddr,
	},
	.NextTable[6] = &(TDataTable){
		.CJsonFunc = cJSON_AddArrayToObject,
		.MqttData.DataName = "data",
		.NextTable[0] = &(TDataTable){
			.CJsonFunc = cJSON_AddItemToArray,
			.MqttData.DataType = eArrayValue,
			.MqttData.DataValue.MqttDataArray = &(TMqttArrayData){},
		},
	},

};
TDataTable CollectionToData = {
	.CJsonFunc = cJSON_CreateObject,
	.NextTable[0] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "1",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eUa,
	},
	.NextTable[1] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "2",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eUb,
	},
	.NextTable[2] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "3",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eUc,
	},
	.NextTable[3] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "4",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eLineUa,
	},
	.NextTable[4] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "5",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eLineUb,
	},
	.NextTable[5] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "6",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eLineUc,
	},
	.NextTable[6] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "7",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eIa,
	},
	.NextTable[7] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "8",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eIb,
	},
	.NextTable[8] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "9",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eIc,
	},
	.NextTable[9] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "10",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eIzs,
	},
	.NextTable[10] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "11",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eP1,
	},
	.NextTable[11] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "12",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eP2,
	},
	.NextTable[12] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "13",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eP3,
	},
	.NextTable[13] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "14",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eP4,
	},
	.NextTable[14] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "15",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eQ1,
	},
	.NextTable[15] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "16",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eQ2,
	},
	.NextTable[16] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "17",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eQ3,
	},
	.NextTable[17] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "18",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eQ4,
	},
	.NextTable[18] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "19",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eS1,
	},
	.NextTable[19] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "20",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eS2,
	},
	.NextTable[20] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "21",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eS3,
	},
	.NextTable[21] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "22",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eS4,
	},
	.NextTable[22] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "23",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePF1,
	},
	.NextTable[23] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "24",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePF2,
	},
	.NextTable[24] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "25",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePF3,
	},
	.NextTable[25] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "26",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePF4,
	},
	.NextTable[26] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "27",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eComActive,
	},
	.NextTable[27] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "28",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePActiveAll,
	},
	.NextTable[28] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "29",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eNActiveAll,
	},
	.NextTable[29] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "30",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eRActive1All,
	},
	.NextTable[30] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "31",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eRActive2All,
	},
	.NextTable[31] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "32",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePa_PActive1,
	},
	.NextTable[32] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "33",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePa_NActive1,
	},
	.NextTable[33] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "34",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePa_ComRActive1,
	},
	.NextTable[34] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "35",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePa_ComRActive2,
	},
	.NextTable[35] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "36",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePb_PActive1,
	},
	.NextTable[36] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "37",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePb_NActive1,
	},
	.NextTable[37] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "38",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePb_ComRActive1,
	},
	.NextTable[38] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "39",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePb_ComRActive2,
	},
	.NextTable[39] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "40",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePc_PActive1,
	},
	.NextTable[40] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "41",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePc_NActive1,
	},
	.NextTable[41] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "42",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePc_ComRActive1,
	},
	.NextTable[42] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "43",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = ePc_ComRActive2,
	},
	.NextTable[43] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "44",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eTemp1,
	},
	.NextTable[44] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "45",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eTemp2,
	},
	.NextTable[45] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "46",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eTemp3,
	},
	.NextTable[46] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "47",
		.MqttData.DataType = eFromCollection,
		.MqttData.ValueType = eTemp4,
	},
};

TDataTable YGDYDataTable = {
	.CJsonFunc = cJSON_CreateObject,
	.NextTable[0] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "clientId",
		.MqttData.DataValue.StringValue = ReportPara.mqttClientld,
	},
	.NextTable[1] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "countId",
		.MqttData.DataValue.StringValue = "0",
	},
	.NextTable[2] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "breakpointResume",
		.MqttData.DataValue.StringValue = "0",
	},
	.NextTable[3] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "time",
		.MqttData.DataType = eGetTimeMs,
	},
	.NextTable[4] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "commandType",
		.MqttData.DataValue.StringValue = "1",
	},
	.NextTable[5] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "deviceId",
		.MqttData.DataValue.StringValue = ReportPara.device[0],
	},
	.NextTable[6] = &(TDataTable){
		.CJsonFunc = cJSON_AddArrayToObject,
		.MqttData.DataName = "data",
		.NextTable[0] = &(TDataTable){
			.CJsonFunc = cJSON_AddItemToArray,
			.MqttData.DataType = eArrayValue,
			.MqttData.DataValue.MqttDataArray = &(TMqttArrayData){
				// .TMqttData[0]  = &(T_MqttData){.DataType = eCollectionToData,},
				// .TMqttData[1]  = &(T_MqttData){.DataType = eCollectionToData,},
				// .TMqttData[2]  = &(T_MqttData){.DataType = eCollectionToData,},
				// .TMqttData[3]  = &(T_MqttData){.DataType = eCollectionToData,},
			},
		},
	},
};
#endif//#endif( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_MQTT)
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
nwy_sim_result_type imei = {0};
ReportPara_txt ReportPara;


tTranData TouartTranData;
tTranData UploadTranData;

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
BYTE UserMqttConnectStatus = 0;
unsigned char *g_nwy_paho_readbuf = NULL;
unsigned char *g_nwy_paho_writebuf = NULL;
//用户端mqtt  缓存
unsigned char *g_nwy_user_readbuf = NULL;
unsigned char *g_nwy_user_writebuf = NULL;

MQTTClient paho_mqtt_client;
nwy_osiThread_t *nwy_paho_task_id = NULL;
Network *paho_network = NULL;
nwy_paho_mqtt_at_param_type paho_mqtt_at_param = {0};

#if (MQTT_USER == YES)
//断点续传相关
SaveDataInfo RamHeader;
BYTE UserMqttReconnectTimerFlag = 1; //用户mqtt重连定时器标志,取消睡眠等待，因为要保存断连数据，上电默认开启
WORD SavedNum = 0;					 //保存数据条数,判断上线后需要发送的数据条数,最大和SAVE_TO_FILE_MAX_ITEM_NUM一致
BYTE IsMQTTConnected = 0;

MQTTClient paho_user_client;
nwy_osiThread_t *nwy_paho_usertask_id = NULL;
Network *paho_user_network = NULL;
nwy_paho_mqtt_at_param_type paho_mqtt_user_param = {0};
#endif
const char ServerIP[] = "218.201.129.20"; //"121.41.60.84";   //"119.167.163.7";//"58.210.240.122";////"mqtt.xnkiot.com"
char MqttID[sizeof(imei.nImei)];
const char MqttIDConst[] = "861996071005257";
const char RequestID[] = "093780FC691C0540B5F047B9C516E318";
const char ServerUserName[] = "wisdom";
const char ServerUserPwd[] = "wisdom2024";
const uint16 ServerPort = 6211; // 6161;//6183;
//先是发布主题全名，后是订阅主题全名
static char device_full_name_topics[20][64];
//通用发布主题名
static char general_pub_topic[64] = "/devices/HY_pub";
//通用订阅主题名
static char general_sub_topic[64] = "/devices/Test_sub";
//发布主题尾
static char device_pub_topics_tail[][64] = 
{
	"/resetAck",
	"/cycleData",
	"/transReply",
	"/getTimeAck",
	"/setTimeAck",
	"/setServerParaAck",
	"/setReportParaAck",
	"/upGradeAck",
};
//发布主题尾在topics的顺序
typedef enum
{
	eResetAck,
	eCycleData,
	eTransReply,
	eGetTimeAck,
	eSetTimeAck,
	eSetServerParaAck,
	eSetReportParaAck,
	eUpGradeAck,
}ePubTopicIndex;
// 订阅主题尾
static char device_sub_topics_tail[][64] = 
{
	"/appVersion",
	"/upGrade",
	"/reset",
	"/getTime",
	"/setTime",
	"/setPara",
};

//mqtt维护端服务器 订阅透传专用主题
const char TransitCommonTopic[] = "WSD/Trans/Common/2015";
static char TransitSingleTopic[64] = "WSD/Trans/Single/";
// 发布透传回复主题
const char TransitRespondTopic[] = "WSD/Trans/Single/Reply";


#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_MQTT)
double DoubleData[MAX_SAMPLE_CHIP][COLLECT_DATA_ITEM_NUM];
#endif


#if (MQTT_USER == YES)			//有关mqtt的事件上报 需要特殊主题
#if (EVENT_REPORT == YES)	
static char c_tail_eventReport[] = "/devices/JLXF/event/warning";
#endif
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
int FilterZeroData(cJSON *item);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
// 功能描述:  过滤0值
//
// 参数:	cJSON *item  需要过滤的json对象
// 返回值:
//
// 备注:
//--------------------------------------------------
int FilterZeroData(cJSON *item)
{
	if (!item)
		return 0;

	if (cJSON_IsObject(item))
	{
		cJSON *child = item->child;
		while (child)
		{
			cJSON *next = child->next;
			if ((cJSON_IsString(child) && strcmp(child->valuestring, "0.00") == 0) || (cJSON_IsString(child) && strcmp(child->valuestring, "-0.00") == 0))
			{
				cJSON_DeleteItemFromObject(item, child->string);
			}
			else
			{
				FilterZeroData(child);
			}
			child = next;
		}
	}
	else if (cJSON_IsArray(item))
	{
		cJSON *child = item->child;
		while (child)
		{
			FilterZeroData(child);
			child = child->next;
		}
	}
	return 1;
}
#if (MQTT_USER == YES)
#if (EVENT_REPORT == YES)
static BOOL pub_reportEventdata(Eventmessage EventData)
{
	cJSON *pJsonRoot;
	char *c_message;
	MQTTMessage pubmsg = {0};
	char Addru[30] = {0};

	memcpy(Addru, LtuAddr, sizeof(LtuAddr));
	memset(paho_mqtt_at_param.topic, 0, sizeof(paho_mqtt_at_param.topic));
	strncpy(paho_mqtt_at_param.topic, &c_tail_eventReport[0], strlen(c_tail_eventReport));
	paho_mqtt_at_param.qos = 1;
	paho_mqtt_at_param.retained = 10;
	pJsonRoot = cJSON_CreateObject();
	if (NULL == pJsonRoot)
		return FALSE;
	cJSON_AddStringToObject(pJsonRoot, "WARNING", EventData.Warning);
	cJSON_AddStringToObject(pJsonRoot, "ID", Addru);
	cJSON_AddNumberToObject(pJsonRoot, "T", EventData.TimeMs);
	cJSON_AddNumberToObject(pJsonRoot, "NO", EventData.Number);

	c_message = cJSON_PrintUnformatted(pJsonRoot);
	// 发布部分
	memset(paho_mqtt_at_param.message, 0, sizeof(paho_mqtt_at_param.message));
	strncpy(paho_mqtt_at_param.message, c_message, strlen(c_message));
	nwy_ext_echo("\r\nmqttpub param retained = %d, qos = %d, topic = %s, msg = %s", paho_mqtt_at_param.retained, paho_mqtt_at_param.qos, paho_mqtt_at_param.topic,
				 paho_mqtt_at_param.message);
	memset(&pubmsg, 0, sizeof(pubmsg));
	pubmsg.payload = (void *)paho_mqtt_at_param.message;
	pubmsg.payloadlen = strlen(paho_mqtt_at_param.message);
	pubmsg.qos = paho_mqtt_at_param.qos;
	pubmsg.retained = paho_mqtt_at_param.retained;
	pubmsg.dup = 0;
	int rc = nwy_MQTTPublish(&paho_user_client, paho_mqtt_at_param.topic, &pubmsg);
	if (rc)
	{
		cJSON_Delete(pJsonRoot);
		free(c_message);
		return FALSE;
	}
	cJSON_Delete(pJsonRoot);
	free(c_message);
	return TRUE;
}
#endif
#endif
#if (CYCLE_METER_READING == YES)
#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_MQTT)
//--------------------------------------------------
// 功能描述:  数据转换为cJSON
//
// 参数:	char* name  特殊处理调用上层name
//			int valuetype 	特殊处理 调用上层valuetype
//			byte LOOP 回路数0-3
//			QWORD qwTms	 时间戳
//			char *addr	 设备地址
// 返回值:
//
// 备注:
//--------------------------------------------------
cJSON *DataToCJson(TDataTable *DataTable, char *name, int ValueType, BYTE bLoop, cJSON *cJsonOut, QWORD qwTms, char *addr)
{
	cJSON *cJsonTemp = NULL;
	char strTemp[64] = {0};
	DWORD i = 0;
	double dbtemp = 0;

	if (DataTable->CJsonFunc == cJSON_CreateObject)
	{
		cJsonOut = cJSON_CreateObject();
		if (NULL == cJsonOut)
		{
			return NULL;
		}
	}
	else if (DataTable->CJsonFunc == cJSON_AddObjectToObject)
	{
		cJsonOut = cJSON_AddObjectToObject(cJsonOut, DataTable->MqttData.DataName);
		if (NULL == cJsonOut)
		{
			return NULL;
		}
	}
	else if (DataTable->CJsonFunc == cJSON_AddStringToObject)
	{
		if (DataTable->MqttData.DataType == eFromCollection)
		{
			dbtemp = DoubleData[bLoop][DataTable->MqttData.ValueType];
			sprintf(strTemp, "%0.2f", dbtemp); //后期乘变比后，字符串小数位可能很长！！！ 小数位可能需统一处理
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, strTemp);
		}
		else if (DataTable->MqttData.DataType == eSelfName)
		{
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, name);
		}
		else if (DataTable->MqttData.DataType == eSelfValueType)
		{
			dbtemp = DoubleData[bLoop][ValueType];
			sprintf(strTemp, "%f", dbtemp); //后期乘变比后，字符串小数位可能很长！！！ 小数位可能需统一处理
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, strTemp);
		}
		else if (DataTable->MqttData.DataType == eGetMeterAddr)
		{
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, addr);
		}
		else if (DataTable->MqttData.DataType == eGetImei)
		{
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, MqttID);
		}
		else if (DataTable->MqttData.DataType == eGetTimeMs)
		{
			sprintf(strTemp, "%lld", qwTms);
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, strTemp);
		}
		else
		{
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, DataTable->MqttData.DataValue.StringValue);
		}
		if (NULL == cJsonOut)
		{
			return NULL;
		}
	}
	else if (DataTable->CJsonFunc == cJSON_AddNumberToObject)
	{
		if (DataTable->MqttData.DataType == eFromCollection)
		{
			DataTable->MqttData.DataValue.DoubleValue = DoubleData[bLoop][DataTable->MqttData.ValueType];
			cJsonOut = cJSON_AddNumberToObject(cJsonOut, DataTable->MqttData.DataName, DataTable->MqttData.DataValue.DoubleValue);
		}
		else if (DataTable->MqttData.DataType == eGetTimeMs)
		{
			cJsonOut = cJSON_AddNumberToObject(cJsonOut, DataTable->MqttData.DataName, qwTms);
		}
		else
		{
			cJsonOut = cJSON_AddNumberToObject(cJsonOut, DataTable->MqttData.DataName, DataTable->MqttData.DataValue.DoubleValue);
		}
		if (NULL == cJsonOut)
		{
			return NULL;
		}
	}
	else if (DataTable->CJsonFunc == cJSON_AddArrayToObject)
	{
		cJsonOut = cJSON_AddArrayToObject(cJsonOut, DataTable->MqttData.DataName);
		if (NULL == cJsonOut)
		{
			return NULL;
		}
	}
	else if (DataTable->CJsonFunc == cJSON_AddItemToArray)
	{
		for (i = 0; i < MAX_DATA_NUM; i++)
		{
			if (DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i] != NULL)
			{
				if (DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataType == eStringValue)
				{
					cJsonTemp = cJSON_CreateString(DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataValue.StringValue);
					if (NULL == cJsonTemp)
					{
						return NULL;
					}
					cJSON_AddItemToArray(cJsonOut, cJsonTemp);
				}
				else if (DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataType == eDoubleValue)
				{
					cJsonTemp = cJSON_CreateNumber(DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataValue.DoubleValue);
					if (NULL == cJsonTemp)
					{
						return NULL;
					}
					cJSON_AddItemToArray(cJsonOut, cJsonTemp);
				}
				else if (DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataType == eCJsonValue)
				{
					cJSON_AddItemToArray(cJsonOut, DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataValue.CJsonValue);
				}
				else if (DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataType == eArrayValue)
				{
					nwy_ext_echo("\r\n enter arry");
				}
				else if (DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataType == eCollectionToData) //嵌套同类型结构
				{
					cJsonTemp = DataToCJson(&CollectionToData, DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataName, DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->ValueType, bLoop, cJsonTemp, qwTms, addr);
					cJSON_AddItemToArray(cJsonOut, cJsonTemp);
				}
			}
		}
	}
	for (i = 0; i < MAX_DATA_NUM; i++)
	{
		if (DataTable->NextTable[i] != NULL)
		{
			DataToCJson(DataTable->NextTable[i], name, ValueType, bLoop, cJsonOut, qwTms, addr);
		}
	}
	return cJsonOut;
}
#endif//#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_MQTT)
#endif //#if( CYCLE_METER_READING ==  YES)

static void init_topic(void)
{
	char c_devic[] = "/devices/";
	int len = 0, i = 0;
	int pub_topic_num = 0;

	memset(&device_full_name_topics[0][0], 0, sizeof(device_full_name_topics));
	// topic 初始化
	for (i = 0; i < 20; i++)
	{
		memcpy(&device_full_name_topics[i][0], c_devic, sizeof(c_devic));
		len = strlen(&device_full_name_topics[i][0]);
		memcpy(&device_full_name_topics[i][len], MqttID, sizeof(MqttID));
	}
	len = strlen(&device_full_name_topics[0][0]);
	pub_topic_num = sizeof(device_pub_topics_tail)/sizeof(device_pub_topics_tail[0]);

	for (i = 0; i < pub_topic_num; i++)
	{
		memcpy(&device_full_name_topics[i][len], &device_pub_topics_tail[i][0], strlen(device_pub_topics_tail[i]));
	}

	for (i = pub_topic_num; i < pub_topic_num + sizeof(device_sub_topics_tail)/sizeof(device_sub_topics_tail[0]); i++)
	{
		memcpy(&device_full_name_topics[i][len], &device_sub_topics_tail[i-pub_topic_num][0], strlen(device_sub_topics_tail[i-pub_topic_num]));
	}


	for (i = 0; i < 20; i++)
		nwy_ext_echo("init_topic:[%d]%s   \r\n", i, &device_full_name_topics[i][0]);
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL RemessageF(const char *type, char *topic_p, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const char *fs;
	MQTTMessage pubmsg = {0};
	char *c_message;
	cJSON *pJsonRoot;
	TRealTimer tTime = {0};

	// 准备主题
	memset(paho_mqtt_at_param.topic, 0, sizeof(paho_mqtt_at_param.topic));
	strncpy(paho_mqtt_at_param.topic, topic_p, strlen(topic_p));

	paho_mqtt_at_param.qos = 0;		  // atoi(sptr);
	paho_mqtt_at_param.retained = 10; //
	// 准备消息
	pJsonRoot = cJSON_CreateObject();
	if (NULL == pJsonRoot)
		return FALSE;
	get_N176_time(&tTime);
	cJSON_AddStringToObject(pJsonRoot, "deviceId", MqttID);
	cJSON_AddStringToObject(pJsonRoot, "type", type);
	while (*fmt != '\0')
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
	va_end(args);
	cJSON_AddNumberToObject(pJsonRoot, "timestamp", getmktimems(&tTime));
	cJSON_AddStringToObject(pJsonRoot, "meterVersion", MeterVersion);
	cJSON_AddStringToObject(pJsonRoot, "meterAddr", LtuAddr);
	c_message = cJSON_PrintUnformatted(pJsonRoot);
	// 发布部分
	memset(paho_mqtt_at_param.message, 0, sizeof(paho_mqtt_at_param.message));
	strncpy(paho_mqtt_at_param.message, c_message, strlen(c_message));
	nwy_ext_echo("\r\nmqttpub param retained = %d, qos = %d, topic = %s, msg = %s", paho_mqtt_at_param.retained, paho_mqtt_at_param.qos, paho_mqtt_at_param.topic,
				 paho_mqtt_at_param.message);
	memset(&pubmsg, 0, sizeof(pubmsg));
	pubmsg.payload = (void *)paho_mqtt_at_param.message;
	pubmsg.payloadlen = strlen(paho_mqtt_at_param.message);
	pubmsg.qos = paho_mqtt_at_param.qos;
	pubmsg.retained = paho_mqtt_at_param.retained;
	pubmsg.dup = 0;
	int rc = nwy_MQTTPublish(&paho_mqtt_client, paho_mqtt_at_param.topic, &pubmsg);
	if (rc)
	{
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
// 功能描述:  回复透传数据给主站
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void pubTranData(tTranData *data)
{
	MQTTMessage pubmsg = {0};
	// 准备主题
	memset(paho_mqtt_at_param.topic, 0, sizeof(paho_mqtt_at_param.topic));
	strncpy(paho_mqtt_at_param.topic, TransitRespondTopic, strlen(TransitRespondTopic));

	paho_mqtt_at_param.qos = 0;		  // atoi(sptr);
	paho_mqtt_at_param.retained = 10; //

	// 发布部分
	memset(paho_mqtt_at_param.message, 0, sizeof(paho_mqtt_at_param.message));
	memcpy(paho_mqtt_at_param.message, data->buf, data->len);
	memset(&pubmsg, 0, sizeof(pubmsg));
	pubmsg.payload = (void *)paho_mqtt_at_param.message;
	pubmsg.payloadlen = data->len;
	pubmsg.qos = paho_mqtt_at_param.qos;
	pubmsg.retained = paho_mqtt_at_param.retained;
	pubmsg.dup = 0;
	int rc = nwy_MQTTPublish(&paho_mqtt_client, paho_mqtt_at_param.topic, &pubmsg);
	if (rc)
	{
		nwy_ext_echo("\r\n publish failed");
	}
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
void StartUpgradeRemessage(FtpUpdatePara *ftpupdatepara)
{
	RemessageF("CMD_DEVICE_UPGRADE", &device_full_name_topics[eUpGradeAck][0], "ds", "code", 200.0l, "msg", "SUCCESS");
	RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "ds", "code", 200.0l, "msg", "SUCCESS");
	if (nwy_put_msg_que(FtpUpdateMessageQueue, ftpupdatepara, 0xffffffff))
	{
		nwy_ext_echo("\r\n put message success");
	}
	else
	{
		nwy_ext_echo("\r\n put message fail");
	}
}
//--------------------------------------------------
// 功能描述:  mqtt业务处理服务
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void MQTT_Services(void)
{
	cJSON *pRoot, *pType;
	int GetTemp,ServerNo = 0;
	BYTE UpgradeReply = 0;
	char *msg = NULL;
	FtpUpdatePara ftpupdatepara;
	TSafeMem SafeMem;
	DWORD port = 0;
	char reqbuf[512] = {0};

	memset(&ftpupdatepara, 0, sizeof(ftpupdatepara));
	// 接收主站的MQTT帧
	if (nwy_get_msg_que(MqttResMessageQueue, &msg, 0xffffffff)) // 等待一秒
	{
		nwy_ext_echo("msg:%s", msg);
		pRoot = cJSON_Parse(msg);
		if (NULL == pRoot)
		{
			nwy_ext_echo("pRootNULL");
			return;
		}
		pType = cJSON_GetObjectItem(pRoot, "type");
		if (NULL == pType)
		{
			nwy_ext_echo("pTypeNULL");
			cJSON_Delete(pRoot);
			return;
		}
		nwy_ext_echo("\r\n rx:[%s] \r\n", pType->valuestring);
		if (strstr(pType->valuestring, "CMD_DEVICE_UPGRADE"))
		{ // 升级
			cJSON_GetObjectValue(pRoot, "ftp_name", ecJSON_String, ftpupdatepara.filename, sizeof(ftpupdatepara.filename));
			cJSON_GetObjectValue(pRoot, "ftp_ip", ecJSON_String, ftpupdatepara.ftp_param.host, sizeof(ftpupdatepara.ftp_param.host));
			cJSON_GetObjectValue(pRoot, "ftp_port", ecJSON_Int, &GetTemp, sizeof(GetTemp));
			ftpupdatepara.ftp_param.port = GetTemp;
			cJSON_GetObjectValue(pRoot, "ftp_user", ecJSON_String, ftpupdatepara.ftp_param.username, sizeof(ftpupdatepara.ftp_param.username));
			cJSON_GetObjectValue(pRoot, "ftp_pass", ecJSON_String, ftpupdatepara.ftp_param.passwd, sizeof(ftpupdatepara.ftp_param.passwd));

			//追加命令用来识别是否给电表升级
			cJSON_GetObjectValue(pRoot, "Mode_switch", ecJSON_String, reqbuf, sizeof(reqbuf));
			if (strcmp(reqbuf, "Meter_upgrade_698") == 0)
			{
				if (tIap_698.dwIapFlag == 0)
				{
					tIap_698.dwIapFlag = DOWNLOAD_METERFILE;
					StartUpgradeRemessage(&ftpupdatepara);
				}
				else //处于升级过程中 回复upgrade in progess
				{
					RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "ds", "code", 200.0l, "msg", "698 upgrade in progess");
				}
			}
			else if (strcmp(reqbuf, "Meter_upgrade_645") == 0)
			{
				if (tIap_645.dwIapFlag == 0)
				{
					tIap_645.dwIapFlag = DOWNLOAD_METERFILE;
					memset(reqbuf,0,sizeof(reqbuf));
					cJSON_GetObjectValue(pRoot, "Chip_Type", ecJSON_String, reqbuf, sizeof(reqbuf));
					if (strcmp(reqbuf,"415") == 0)
					{
						tIap_645.dwChip = CHIP_415_UPGRADE;		//异常 时 或升级完毕后  全部清零
						nwy_ext_echo("\r\n recv 415");
					}
					else 
					{
						tIap_645.dwChip = 0;
					}
					StartUpgradeRemessage(&ftpupdatepara);
				}
				else
				{
					RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "ds", "code", 200.0l, "msg", "645 upgrade in progess");
				}
			}
			else
			{
				if (api_GetModuleUpGradeStatus() == TRUE)
				{
					StartUpgradeRemessage(&ftpupdatepara);
				}
				else
				{
					RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "ds", "code", 200.0l, "msg", "4G upgrade in progess");
				}
			}
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_APPVERSION"))
		{ // 读版本
			char APP_BUILD_TIME[65] = {0};
			sprintf(APP_BUILD_TIME, "%s,%s", __DATE__, __TIME__);
			RemessageF("CMD_DEVICE_APPVERSION", &general_pub_topic[0], "s", "apptime", APP_BUILD_TIME);
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_GETTIME"))
		{ // 读时钟
			RemessageF("CMD_DEVICE_GETTIME", &device_full_name_topics[eGetTimeAck][0], "s", "DateTime", get_N176_time_stringp());
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_SETTIME"))
		{
			// 设置时间
			cJSON_GetObjectValue(pRoot, "DateTime", ecJSON_String, reqbuf, sizeof(reqbuf));
			set_N176_time_stringp(reqbuf);
			RemessageF("CMD_DEVICE_SETTIME", &device_full_name_topics[eSetTimeAck][0], "");
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_GETRAM"))
		{
			nwy_dm_get_device_heapinfo(reqbuf);
			RemessageF("CMD_DEVICE_GETRAM", &general_pub_topic[0], "s", "DeviceRam", reqbuf);
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_RESET"))
		{
			RemessageF("CMD_DEVICE_RESET", &general_pub_topic[0], "s", "Status", "StartReset");
			RemessageF("CMD_DEVICE_RESET", &device_full_name_topics[eResetAck][0], "s", "Status", "StartReset");
			nwy_power_off(2);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_SINGAL"))
		{
			BYTE bcsq;
			nwy_nw_get_signal_csq(&bcsq);
			double temp = bcsq;
			RemessageF("CMD_DEVICE_SIGNAL_STRENGTH", &general_pub_topic[0], "d","Signal",temp );
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_SET_SERVER_PARA"))
		{
			cJSON_GetObjectValue(pRoot, "ServerNo", ecJSON_Int, &ServerNo, sizeof(ServerNo));
			nwy_ext_echo("\r\nServerNo: %d", ServerNo);
			if((ServerNo != 0) && (ServerNo != 1))
			{
				RemessageF("CMD_DEVICE_SET_SERVER_PARA", &general_pub_topic[0], "s", "Status", "ServerNo error");
				cJSON_Delete(pRoot);
			}
			else
			{
				api_OperateFileSystem(READ,0,(BYTE*)&SafeMem,sizeof(SafeMem));

				memset(&SafeMem.Serverpara[ServerNo],0,sizeof(SafeMem.Serverpara[ServerNo]));

				cJSON_GetObjectValue(pRoot, "user_ip", ecJSON_String, SafeMem.Serverpara[ServerNo].ServerIP, strlen(cJSON_GetObjectItem(pRoot, "user_ip")->valuestring) + 1);
				nwy_ext_echo("\r\nuser_ip: %s", SafeMem.Serverpara[ServerNo].ServerIP);

				cJSON_GetObjectValue(pRoot, "user_port", ecJSON_Int, &port, sizeof(port));
				SafeMem.Serverpara[ServerNo].ServerPort = port;
				nwy_ext_echo("\r\nuser_port: %d", SafeMem.Serverpara[ServerNo].ServerPort);
				
				
				cJSON_GetObjectValue(pRoot, "ServerUserName", ecJSON_String, SafeMem.Serverpara[ServerNo].ServerUserName, strlen(cJSON_GetObjectItem(pRoot, "ServerUserName")->valuestring) + 1);
				nwy_ext_echo("\r\nServerUserName: %s", SafeMem.Serverpara[ServerNo].ServerUserName);
				
				cJSON_GetObjectValue(pRoot, "ServerUserPwd", ecJSON_String, SafeMem.Serverpara[ServerNo].ServerUserPwd, strlen(cJSON_GetObjectItem(pRoot, "ServerUserPwd")->valuestring) + 1);
				nwy_ext_echo("\r\nServerUserPwd: %s", SafeMem.Serverpara[ServerNo].ServerUserPwd);


				api_OperateFileSystem(WRITE,0,(BYTE*)&SafeMem,sizeof(SafeMem));
				RemessageF("CMD_DEVICE_SET_SERVER_PARA", &general_pub_topic[0], "s", "Status", "CMD_DEVICE_SET_SERVER_PARA");
				RemessageF("CMD_DEVICE_SET_SERVER_PARA", &device_full_name_topics[eSetServerParaAck][0], "s", "Status", "CMD_DEVICE_SET_SERVER_PARA");
				nwy_power_off(2);
			}
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_SET_REPORT_PARA"))
		{ 
			api_OperateFileSystem(READ,0,(BYTE*)&SafeMem,sizeof(SafeMem));
			memset(&SafeMem.ReportPara,0,sizeof(SafeMem.ReportPara));
			
			cJSON_GetObjectValue(pRoot, "cycle_pub_topic", ecJSON_String, SafeMem.ReportPara.cycleDataTopic, strlen(cJSON_GetObjectItem(pRoot, "cycle_pub_topic")->valuestring) + 1);
			nwy_ext_echo("\r\ncycle_pub_topic: %s", SafeMem.ReportPara.cycleDataTopic);
			
			cJSON_GetObjectValue(pRoot, "report_fre", ecJSON_Int, &SafeMem.ReportPara.reportfre, sizeof(SafeMem.ReportPara.reportfre));
			nwy_ext_echo("\r\nreport_fre: %d", SafeMem.ReportPara.reportfre);
			api_OperateFileSystem(WRITE,0,(BYTE*)&SafeMem,sizeof(SafeMem));
			
			RemessageF("CMD_DEVICE_SET_REPORT_PARA", &general_pub_topic[0], "s", "Status", "CMD_DEVICE_SET_REPORT_PARA");
			RemessageF("CMD_DEVICE_SET_REPORT_PARA", &device_full_name_topics[eSetReportParaAck][0], "s", "Status", "CMD_DEVICE_SET_REPORT_PARA");
			nwy_power_off(2);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_GET_PARA"))
		{ 
			api_OperateFileSystem(READ,0,(BYTE*)&SafeMem,sizeof(SafeMem));
			// 读参数
			RemessageF("CMD_DEVICE_GET_PARA", &general_pub_topic[0], "sdsssdssdsd", 
				"para0_user_ip", SafeMem.Serverpara[0].ServerIP,
				"para0_user_port", (double)SafeMem.Serverpara[0].ServerPort,
				"para0_ServerUserName", SafeMem.Serverpara[0].ServerUserName,
				"para0_ServerUserPwd", SafeMem.Serverpara[0].ServerUserPwd,
				"para1_user_ip", SafeMem.Serverpara[1].ServerIP,
				"para1_user_port", (double)SafeMem.Serverpara[1].ServerPort,
				"para1_ServerUserName", SafeMem.Serverpara[1].ServerUserName,
				"para1_ServerUserPwd", SafeMem.Serverpara[1].ServerUserPwd,
				"report_fre", (double)SafeMem.ReportPara.reportfre,
				"cycle_pub_topic", SafeMem.ReportPara.cycleDataTopic,
				"user_connect_status",(double)UserMqttConnectStatus);
			cJSON_Delete(pRoot);
		}
		else
		{
			cJSON_Delete(pRoot);
		}

		nwy_ext_echo("free");
		free(msg);
		// 升级-》创建升级任务
		// 参数设置-》重新连接？
		// 对时回复、读版本回复
		// 透传请求-》填充uart消息队列或事件
		// 其他
	}
	if (nwy_get_msg_que(TranDataToMqttServerMsgQue, &UploadTranData, 0xffffffff))
	{
		nwy_ext_echo("\r\nmqtt get uploadLen from uart first:%d", UploadTranData.len);
		pubTranData(&UploadTranData);
	}
	if (nwy_get_msg_que(UpgradeResultMessageQueue, &UpgradeReply, 0xffffffff))
	{
		switch (UpgradeReply)
		{
		case eUpgradeExeSuc:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "execute Upgrade successful");
			break;
		case eUpgrdeFrameTimeoutErr:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "Frame timeout error");
			break;
		case eUpgradeCheckResultErr:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "Check result error");
			break;
		case eUpgradeVeriErr:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "file Verification error");
			break;
		case eUpgradeDARErr:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "DAR error");
			break;
		case eUpgradeWholeProTimeoutErr:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "Whole process timeout error");
			break;
		case eFtpLoginSuccess:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "ftp login success");
			break;
		case eFtpDownLoadSuccess:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "ftp download success");
			break;
		case eFtpModuleUpgradeFail:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "ftp module upgrade fail");
			break;
			break;
		case eFtpFailExit:
			RemessageF("CMD_DEVICE_UPGRADE", &general_pub_topic[0], "s", "upgradeResult", "ftp module upgrade fail exit");
			break;
		default:
			break;
		}
	}
}
//--------------------------------------------------
//功能描述:  mqtt 透传接收函数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void TransmessageArrived(MessageData *md)
{
	memset(&TouartTranData, 0, sizeof(TouartTranData));

	memcpy(TouartTranData.buf, md->message->payload, md->message->payloadlen);
	TouartTranData.len = md->message->payloadlen;
	TouartTranData.TranType = eTRAN_MQTT;
	if (!nwy_put_msg_que(TranDataToUartMessageQueue, &TouartTranData, 0xffffffff))
	{
		nwy_ext_echo("mqtt recv failed");
	}
}

void messageArrived(MessageData *md)
{
	char *msg = NULL;
	char temp[64] = {0};

	memcpy(temp,md->topicName->lenstring.data,md->topicName->lenstring.len);
	nwy_ext_echo("\r\n sub{%s}",temp);
	// if ((memcmp(temp,SubTransitSingleTopic,sizeof(SubTransitSingleTopic)) == 0)
	// ||memcmp(temp,TransitCommonTopic,sizeof(TransitCommonTopic) == 0))
	// {
	// 	TransmessageArrived(md);
	// }
	// else
	{
		nwy_ext_echo("\r\nmessage Arrived");
		if (MqttResMessageQueue == NULL)
		{
			nwy_ext_echo("Queue==NULL");
			return;
		}
		if (!nwy_get_queue_spaceevent_cnt(MqttResMessageQueue))
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
		if (!nwy_put_msg_que(MqttResMessageQueue, &msg, 0xffffffff))
		{
			free(msg);
		}
	}
}

void nwy_paho_cycle(void *para)
{
	while (1)
	{
		while (MQTTIsConnected(&paho_mqtt_client))
		{
			MQTTYield(&paho_mqtt_client, 500);
			nwy_sleep(200);
		}
		nwy_ext_echo("\r\nMQTT disconnect ,Out paho cycle");
		
		nwy_suspend_thread(nwy_paho_task_id);
	}
	nwy_sleep(200);
}
nwy_osiThread_t *nwy_paho_yeild_task_init(void)
{
	if (nwy_paho_task_id == NULL)
	{
		nwy_osiThread_t *task_id = nwy_create_thread("neo_paho_yeild_task", nwy_paho_cycle, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 8, 4);
		nwy_paho_task_id = task_id;
	}
	else
		nwy_resume_thread(nwy_paho_task_id);
	return nwy_paho_task_id;
}
//--------------------------------------------------
// 功能描述:  mqtt连接
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void MQTT_connet(void)
{
	char Sub_topic[64] = {0};
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	/*code*/
	// 连接服务器
	memcpy(paho_mqtt_at_param.host_name, ServerIP, sizeof(Serverpara[1].ServerIP));
	paho_mqtt_at_param.port = ServerPort;
	memcpy(paho_mqtt_at_param.clientID, MqttID, sizeof(MqttID));
	memcpy(paho_mqtt_at_param.username, Serverpara[1].ServerUserName, sizeof(Serverpara[1].ServerUserName));
	memcpy(paho_mqtt_at_param.password, Serverpara[1].ServerUserPwd, sizeof(Serverpara[1].ServerUserPwd));
	paho_mqtt_at_param.paho_ssl_tcp_conf.sslmode = 0; // nossl
	if (g_nwy_paho_writebuf != NULL)				  // 改为数组
	{
		free(g_nwy_paho_writebuf);
		g_nwy_paho_writebuf = NULL;
	}
	if (NULL == (g_nwy_paho_writebuf = (unsigned char *)malloc(PAHO_PLAYOAD_LEN_MAX)))//PAHO_PLAYOAD_LEN_MAX 更新SDK后  宏定义需修改 否则上传数据量过多时异常！！！
	{
		nwy_ext_echo("\r\nmalloc buffer g_nwy_paho_writebuf error");
		return;
	}
	if (g_nwy_paho_readbuf != NULL)
	{
		free(g_nwy_paho_readbuf);
		g_nwy_paho_readbuf = NULL;
	}
	if (NULL == (g_nwy_paho_readbuf = (unsigned char *)malloc(PAHO_PLAYOAD_LEN_MAX)))
	{
		nwy_ext_echo("\r\nmalloc buffer g_nwy_paho_readbuf error");
		return;
	}
	memset(g_nwy_paho_readbuf, 0, PAHO_PLAYOAD_LEN_MAX);
	memset(g_nwy_paho_writebuf, 0, PAHO_PLAYOAD_LEN_MAX);
	if (paho_network != NULL)
	{
		NetworkDisconnect(paho_network);
		free(paho_network);
		paho_network = NULL;
	}
	if (NULL == (paho_network = (Network *)malloc(sizeof(Network))))
	{
		nwy_ext_echo("\r\nmalloc buffer paho_network error");
		return;
	}
	memset(paho_network, 0, sizeof(Network));
	NetworkInit(paho_network);
	paho_mqtt_at_param.cleanflag = 0;
	paho_mqtt_at_param.keepalive = 60;
	paho_network->cid = 0;
	int rc = NetworkConnect(paho_network, paho_mqtt_at_param.host_name, paho_mqtt_at_param.port);
	if (rc < 0)
	{
		nwy_ext_echo("\r\nNetworkConnect err rc=%d", rc);
		NetworkDisconnect(paho_network);
		return;
	}
	MQTTClientInit(&paho_mqtt_client, paho_network, 10000, g_nwy_paho_writebuf, PAHO_PLAYOAD_LEN_MAX, g_nwy_paho_readbuf, PAHO_PLAYOAD_LEN_MAX);
	MQTTClientInit_defaultMessage(&paho_mqtt_client, messageArrived);
	data.clientID.cstring = paho_mqtt_at_param.clientID;
	if (0 != strlen((char *)paho_mqtt_at_param.username) && 0 != strlen((char *)paho_mqtt_at_param.password))
	{
		data.username.cstring = paho_mqtt_at_param.username;
		data.password.cstring = paho_mqtt_at_param.password;
	}
	data.keepAliveInterval = paho_mqtt_at_param.keepalive;
	data.cleansession = paho_mqtt_at_param.cleanflag;
	if (0 != strlen((char *)paho_mqtt_at_param.willtopic))
	{
		memset(&data.will, 0x0, sizeof(data.will));
		data.willFlag = 1;
		data.will.retained = paho_mqtt_at_param.willretained;
		data.will.qos = paho_mqtt_at_param.willqos;
		if (paho_mqtt_at_param.willmessage_len != 0)
		{
			data.will.topicName.lenstring.data = paho_mqtt_at_param.willtopic;
			data.will.topicName.lenstring.len = strlen((char *)paho_mqtt_at_param.willtopic);
			data.will.message.lenstring.data = paho_mqtt_at_param.willmessage;
			data.will.message.lenstring.len = paho_mqtt_at_param.willmessage_len;
		}
		else
		{
			data.will.topicName.cstring = paho_mqtt_at_param.willtopic;
			data.will.message.cstring = paho_mqtt_at_param.willmessage;
		}
		nwy_ext_echo("\r\nMQTT will ready");
	}
	nwy_ext_echo("\r\nConnecting MQTT");
	rc = nwy_MQTTConnect(&paho_mqtt_client, &data);
	if (rc)
	{
		nwy_ext_echo("\r\nFailed to create client, return code %d", rc);
		MQTTDisconnect(&paho_mqtt_client);
		NetworkDisconnect(paho_network);
		return;
	}
	else
	{
		nwy_ext_echo("\r\nMQTT connect ok");
		nwy_osiThread_t *task_id = nwy_paho_yeild_task_init();
		if (task_id == NULL)
		{
			nwy_ext_echo("\r\npaho yeid task create failed ");
			MQTTDisconnect(&paho_mqtt_client);
			NetworkDisconnect(paho_network);
			return;
		}
		else
			nwy_ext_echo("\r\npaho yeid task create ok ");
	}
	if (MQTTIsConnected(&paho_mqtt_client))
	{
		init_topic();
		for (int i = 0; i <sizeof(device_sub_topics_tail)/sizeof(device_sub_topics_tail[0]); i++)
		{
			memset(paho_mqtt_at_param.topic, 0, sizeof(paho_mqtt_at_param.topic));
			memcpy(Sub_topic, &device_full_name_topics[i + sizeof(device_pub_topics_tail)/sizeof(device_pub_topics_tail[0])][0], 64);
			strncpy(paho_mqtt_at_param.topic, Sub_topic, strlen(Sub_topic));
			nwy_ext_echo("\r\n topic_sub:[%s][%s]\r\n", paho_mqtt_at_param.topic, Sub_topic);

			paho_mqtt_at_param.qos = 0; //..atoi(sptr);
			rc = MQTTSubscribe(&paho_mqtt_client, (char *)paho_mqtt_at_param.topic, paho_mqtt_at_param.qos, messageArrived);
			if (rc == SUCCESS)
			{
				nwy_ext_echo("\r\n-------------------MQTT Sub ok \r\n ");
			}
			else
				nwy_ext_echo("\r\nMQTT Sub error:%d", rc);
		}
		//订阅general_sub_topic
		MQTTSubscribe(&paho_mqtt_client, general_sub_topic, paho_mqtt_at_param.qos, messageArrived);

		//订阅 基表地址为主题 只用来 批量或同时透传设置多个 基表参数
		nwy_ext_echo("\r\n ltu:[%s]\r\n",LtuAddr);
		memcpy(&TransitSingleTopic[strlen(TransitSingleTopic)],LtuAddr,sizeof(LtuAddr));
		nwy_ext_echo("\r\n TransitSingleTopic:[%s]\r\n",TransitSingleTopic);
		
		//MQTTSubscribe 函数参数2 底层 会调用地址 新申请函数回调函数时需传入不同地址的主题
		MQTTSubscribe(&paho_mqtt_client, TransitSingleTopic, paho_mqtt_at_param.qos, TransmessageArrived);
		rc = MQTTSubscribe(&paho_mqtt_client, TransitCommonTopic, paho_mqtt_at_param.qos, TransmessageArrived);
		if (rc == SUCCESS)
		{
			nwy_ext_echo("\r\n-------------------MQTT Sub TransitCommonTopic ok \r\n ");
		}
		else
		nwy_ext_echo("\r\nMQTT Sub TransitCommonTopic error:%d", rc);
	}
}
//--------------------------------------------------
// 功能描述:	维护使用
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void WSD_MQTT_Task(void *param)
{
	nwy_sleep(600);
	nwy_sim_get_imei(&imei);
	memcpy(MqttID, imei.nImei, sizeof(imei.nImei));
	DWORD Connectfre = 0;

	while (1)
	{
		if (0 != nwy_ext_check_data_connect())
		{
			if (MQTTIsConnected(&paho_mqtt_client) != 1)
			{
				MQTT_connet();								 // 退出机制
				if (MQTTIsConnected(&paho_mqtt_client) != 1) //没有连接成功，再进行延时，连接成功不再进行延时
				{
					if (Connectfre < 20)
					{
						Connectfre++;
						nwy_sleep(6000 * Connectfre);
					}
					else
					{
						nwy_sleep(3600000 * 4); // 4小时重试一次连接 每次连接消耗560字节
					}
				}
				else
				{
					Connectfre = 0;
				}
			}
			else
			{
				Connectfre = 0;
				MQTT_Services();
			}
		}
		nwy_sleep(500);
	}
}

#if (MQTT_USER == YES)
//--------------------------------------------------
// 功能描述: 断电保存ram头部信息
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void PowerDownSaveRamHeader(void)
{
	int fd = -1;

	if (RamHeader.crc16 != CalCRC16((unsigned char *)&RamHeader, sizeof(RamHeader) - sizeof(RamHeader.crc16)))
	{
		nwy_ext_echo("\r\npower down ram header crc16 error");
		return;
	}
	if (nwy_sdk_fexist(COLLECTED_DATA_FILE_PATH))
	{
		fd = nwy_sdk_fopen(COLLECTED_DATA_FILE_PATH, NWY_RDWR);
		if (fd >= 0)
		{
			nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
			nwy_ext_echo("\r\nheader crc16 is %04X", RamHeader.crc16);
			nwy_sdk_fwrite(fd, &RamHeader, sizeof(RamHeader));
			nwy_sdk_fclose(fd);
		}
		else
		{
			nwy_ext_echo("\r\nPowerDownSaveRamHeader open file  error");
		}
	}
	else
	{
		nwy_ext_echo("\r\npower down,collected data file not exist");
	}
}
//--------------------------------------------------
// 功能描述: 创建新文件
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
int CreateNewFile(void)
{
	int fd = -1;

	fd = nwy_sdk_fopen(COLLECTED_DATA_FILE_PATH, NWY_CREAT | NWY_RDWR);
	memset(&RamHeader, 0, sizeof(SaveDataInfo));
	RamHeader.crc16 = CalCRC16((unsigned char *)&RamHeader, sizeof(RamHeader) - sizeof(RamHeader.crc16));
	nwy_sdk_fwrite(fd, &RamHeader, sizeof(SaveDataInfo));
	if (fd < 0)
	{
		nwy_ext_echo("\r\ncreate new file error");
	}
	return fd;
}
//--------------------------------------------------
// 功能描述: 检查ram头部信息校验
//
// 参数: fd - 文件描述符
//
// 返回值: true 校验或恢复正确，false 校验错误，且删除文件
//
// 备注:
//--------------------------------------------------
BYTE CheckRamCrc(int fd)
{
	//ram头部信息校验错误从文件恢复
	if (RamHeader.crc16 != CalCRC16((unsigned char *)&RamHeader, sizeof(RamHeader) - sizeof(RamHeader.crc16)))
	{
		nwy_ext_echo("\r\nram header crc16 error");
		nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
		if (nwy_sdk_fread(fd, &RamHeader, sizeof(SaveDataInfo)) == sizeof(SaveDataInfo))
		{
			nwy_ext_echo("\r\nrecover ram header from file ok");
		}
		else
		{
			nwy_ext_echo("\r\nrecover ram header from file error");
			nwy_sdk_fclose(fd);
			nwy_sdk_file_unlink(COLLECTED_DATA_FILE_PATH);
			return FALSE;
		}
	}
	return TRUE;
}
//--------------------------------------------------
// 功能描述: 保存采集数据到文件
//
// 参数: RamCollectedData - 采集数据
//
// 返回值: true 成功，false 失败
//
// 备注:
//--------------------------------------------------
BYTE WriteCollectedDataToFile(CollectionDatas *CollectData)
{
	int fd = -1;
	static WORD WriteCnt = 0;

	// nwy_ext_echo("\r\nwrite data, data time is %lld", CollectData->TimeMs);
	if (nwy_sdk_fexist(COLLECTED_DATA_FILE_PATH))
	{
		fd = nwy_sdk_fopen(COLLECTED_DATA_FILE_PATH, NWY_RDWR);
		if (CheckRamCrc(fd) == FALSE)
		{
			fd = CreateNewFile();
		}
	}
	else
	{
		fd = CreateNewFile();
	}
	if (fd >= 0)
	{
		// 定位到数据区写入新数据，注意偏移计算：头部大小 + 当前偏移 * 单条数据大小
		nwy_sdk_fseek(fd, (sizeof(SaveDataInfo) + RamHeader.WriteOffset * sizeof(CollectionDatas)), NWY_SEEK_SET);
		if (nwy_sdk_fwrite(fd, CollectData, sizeof(CollectionDatas)) != sizeof(CollectionDatas))
		{
			nwy_ext_echo("\r\nwrite data error");
			nwy_sdk_fclose(fd);
			return FALSE;
		}
		else //写入成功
		{
			WriteCnt++;
			// 更新头部信息：条数递增，到最大值后保持，此时每写入一条读取点后移1个位置
			if (RamHeader.SaveDataNum <= (SAVE_TO_FILE_MAX_ITEM_NUM - 1))
			{
				RamHeader.SaveDataNum++;
			}
			else if (RamHeader.SaveDataNum == SAVE_TO_FILE_MAX_ITEM_NUM)
			{
				RamHeader.ReadOffset = (RamHeader.ReadOffset + 1) % SAVE_TO_FILE_MAX_ITEM_NUM;
			}
			RamHeader.WriteOffset = (RamHeader.WriteOffset + 1) % SAVE_TO_FILE_MAX_ITEM_NUM;
			RamHeader.crc16 = CalCRC16((unsigned char *)&RamHeader, sizeof(RamHeader) - sizeof(RamHeader.crc16));

			nwy_ext_echo("\r\n\r\nafter write ,SaveDataNum is %d,ReadOffset is %d,WriteOffset is %d",
						 RamHeader.SaveDataNum, RamHeader.ReadOffset, RamHeader.WriteOffset);
			if (WriteCnt >= UPDATE_HEAD_INFO_WRITE_CNT)
			{
				nwy_ext_echo("\r\nupdate head info");
				nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
				nwy_sdk_fwrite(fd, &RamHeader, sizeof(SaveDataInfo));
				WriteCnt = 0;
			}
			nwy_sdk_fclose(fd);
			return TRUE;
		}
	}
	else
	{
		nwy_ext_echo("\r\nopen  data file error");
		return FALSE;
	}
}
//-----------------------------------------------
// 函数功能: 从循环滚动记录的历史记录中读取最旧的一条记录,每条记录只能读取一次
//
// 参数: pCollectData - 指向用于存储读取记录的内存地址
//
// 返回值: true 成功读取一条记录，false 无数据
//
// 备注:
//-----------------------------------------------
BYTE ReadOldestRecord(CollectionDatas *CollectData)
{
	int fd = -1;
	WORD ReadBytes = 0;
	static WORD ReadCnt = 0;

	// 以读写模式打开文件
	fd = nwy_sdk_fopen(COLLECTED_DATA_FILE_PATH, NWY_RDWR);
	if (fd == -1)
	{
		nwy_ext_echo("\r\nopen file error");
		return FALSE;
	}
	else
	{
		if (CheckRamCrc(fd) == FALSE)
		{
			fd = CreateNewFile();
		}
	}
	if (fd >= 0)
	{
		// 如果记录数为0，则返回无数据状态,创建新文件后也会返回无数据状态
		if (RamHeader.SaveDataNum == 0)
		{
			nwy_ext_echo("\r\nno data , error");
			nwy_sdk_fclose(fd);
			return FALSE;
		}
		// 定位到数据区中对应的最旧记录位置
		nwy_sdk_fseek(fd, (sizeof(SaveDataInfo) + RamHeader.ReadOffset * sizeof(CollectionDatas)), NWY_SEEK_SET);
		ReadBytes = nwy_sdk_fread(fd, CollectData, sizeof(CollectionDatas));
		if (ReadBytes != sizeof(CollectionDatas))
		{
			nwy_ext_echo("\r\nread collect data error,read bytes is %d,expect bytes is %d",
						 ReadBytes, sizeof(CollectionDatas));
			nwy_sdk_fclose(fd);
			return FALSE;
		}
		else
		{
			ReadCnt++;
			// 更新头部信息：记录数递减
			RamHeader.SaveDataNum--;
			RamHeader.ReadOffset = (RamHeader.ReadOffset + 1) % SAVE_TO_FILE_MAX_ITEM_NUM;
			nwy_ext_echo("\r\n\r\nafter read ,SaveDataNum is %d,read offset is %d,WriteOffset is %d",
						 RamHeader.SaveDataNum, RamHeader.ReadOffset, RamHeader.WriteOffset);
			RamHeader.crc16 = CalCRC16((unsigned char *)&RamHeader, sizeof(RamHeader) - sizeof(RamHeader.crc16));
			if (ReadCnt >= UPDATE_HEAD_INFO_WRITE_CNT)
			{
				nwy_ext_echo("\r\nupdate head info");
				nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
				nwy_sdk_fwrite(fd, &RamHeader, sizeof(SaveDataInfo));
				ReadCnt = 0;
			}
			nwy_sdk_fclose(fd);
			return TRUE;
		}
	}
	else
	{
		nwy_ext_echo("\r\nopen file error");
		return FALSE;
	}
}
#if (CYCLE_METER_READING == YES)
//--------------------------------------------------
// 功能描述:	周期上报数据到用户主站
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
static BOOL pub_cycledatatoUser(CollectionDatas *pCollectData) //大数据用指针传参
{
	MQTTMessage pubmsg = {0};
	char *c_message;
	cJSON *pJsonRoot;
	cJSON *pJsonRoot1;
	cJSON *pJsonTemp1;

	BYTE i = 0;
	char Addru[30];

	// 准备主题
	memset(Addru, 0, sizeof(Addru));
	memset(paho_mqtt_user_param.topic, 0, sizeof(paho_mqtt_user_param.topic));
	strncpy(paho_mqtt_user_param.topic, &ReportPara.cycleDataTopic[0], strlen(&ReportPara.cycleDataTopic[0]));
	// nwy_ext_echo("\r\nADDRIS:%s", pCollectData->Addr);
	paho_mqtt_user_param.qos = 1;		// atoi(sptr);
	paho_mqtt_user_param.retained = 10; //
	// 准备消息
	memcpy(Addru, pCollectData->Addr, sizeof(Addru));
	// memcpy(Addru, ReportPara.id, sizeof(ReportPara.id));//ZH上传ID 与表地址无关联
	// nwy_ext_echo("\r\n pCollectData time : %lld", pCollectData->TimeMs);
	pJsonRoot1 = DataToCJson(&JLXFDataTable, NULL, 0, i, NULL, pCollectData->TimeMs, Addru);
	pJsonTemp1 = cJSON_GetObjectItem(pJsonRoot1, "data");

	for (i = 0; i < bUsedChannelNum; i++)
	{
		// Addru[11] = '1' + i;//当前地址++仅适用于青岛联通
		memcpy((double *)&DoubleData[i][0], (double *)&pCollectData->CollectMeter[i][0], sizeof(double) * (COLLECT_DATA_ITEM_NUM));
		pJsonRoot = DataToCJson(&CollectionToData, NULL, 0, i, NULL, pCollectData->TimeMs, (char *)&ReportPara.device[i + 1][0]);
		// FilterZeroData(pJsonRoot);
		cJSON_AddItemToArray(pJsonTemp1, pJsonRoot);
	}
	c_message = cJSON_PrintUnformatted(pJsonRoot1);
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
		free(c_message);
		cJSON_Delete(pJsonRoot1); //只需要删除根结点即可
		nwy_ext_echo("\r\npublish err,rc=%d", rc);
		return FALSE;
	}
	nwy_ext_echo("\r\npublish suc");
	free(c_message);
	cJSON_Delete(pJsonRoot1);
	nwy_sleep(1000);

	return TRUE;
}
#endif
void MQTT_UserServices(void)
{
	#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_MQTT)
	CollectionDatas CollectData;
	#endif
	#if (EVENT_REPORT == YES)
	Eventmessage EventData;
	#endif
	memset(&CollectData, 0, sizeof(CollectionDatas));

	#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_MQTT)
	if (nwy_get_msg_que(CollectMessageQueue, &CollectData, 0xffffffff)) // 组MQTT帧进行发送
	{
		#if (CYCLE_METER_READING == YES)
		pub_cycledatatoUser(&CollectData);
		#endif
	}
	#endif
	#if (EVENT_REPORT == YES)
	if (nwy_get_msg_que(EventReportMessageQueue, &EventData, 0xffffffff)) //事件上报
	{
		pub_reportEventdata(EventData);
	}
#endif
}
#endif
#if(MQTT_USER == YES)
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
	if (!nwy_put_msg_que(MqttResUserMessageQueue, &msg, 0xffffffff))
	{
		free(msg);
	}
}

void nwy_paho_usercycle(void *para)
{
	while (1)
	{
		while (MQTTIsConnected(&paho_user_client))
		{
			MQTTYield(&paho_user_client, 500);
			nwy_sleep(200);
		}
		nwy_ext_echo("\r\nUSER_MQTT disconnect ,Out paho cycle");
		api_WriteSysUNMsg(SYSUN_MQTT_DISCON);
		nwy_suspend_thread(nwy_paho_usertask_id);
	}
	nwy_sleep(200);
}
nwy_osiThread_t *nwy_paho_yeild_usertask_init(void)
{
	if (nwy_paho_usertask_id == NULL)
	{
		nwy_osiThread_t *task_id = nwy_create_thread("neo_paho_yeild_task", nwy_paho_usercycle, NULL, NWY_OSI_PRIORITY_NORMAL, 1024 * 8, 4);
		nwy_paho_usertask_id = task_id;
	}
	else
		nwy_resume_thread(nwy_paho_usertask_id);
	return nwy_paho_usertask_id;
}

//--------------------------------------------------
// 功能描述:  mqtt连接用户主站
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void MQTT_connetuser(void)
{
	#if (EVENT_REPORT == YES)
	QWORD qwMs;
	Eventmessage EventData;
	TRealTimer tTime = {0};
	BYTE eventbuf[] ={"0000000000000000"};
	#endif

	MQTTPacket_connectData datatemp = MQTTPacket_connectData_initializer;

	// 连接服务器
	memcpy(paho_mqtt_user_param.host_name, Serverpara[0].ServerIP, sizeof(Serverpara[1].ServerIP));
	paho_mqtt_user_param.port = Serverpara[0].ServerPort;
	memcpy(paho_mqtt_user_param.clientID, ReportPara.mqttClientld, sizeof(ReportPara.mqttClientld));
	nwy_ext_echo("\r\nReportPara.mqttClientld is %s", ReportPara.mqttClientld);
	memcpy(paho_mqtt_user_param.username, Serverpara[0].ServerUserName, sizeof(Serverpara[1].ServerUserName));
	memcpy(paho_mqtt_user_param.password, Serverpara[0].ServerUserPwd, sizeof(Serverpara[1].ServerUserPwd));
	paho_mqtt_user_param.paho_ssl_tcp_conf.sslmode = 0; // nossl
	if (g_nwy_user_writebuf != NULL)					// 改为数组
	{
		free(g_nwy_user_writebuf);
		g_nwy_user_writebuf = NULL;
	}
	if (NULL == (g_nwy_user_writebuf = (unsigned char *)malloc(PAHO_PLAYOAD_LEN_MAX)))
	{
		nwy_ext_echo("\r\nmalloc buffer g_nwy_user_writebuf error");
		return;
	}
	if (g_nwy_user_readbuf != NULL)
	{
		free(g_nwy_user_readbuf);
		g_nwy_user_readbuf = NULL;
	}
	if (NULL == (g_nwy_user_readbuf = (unsigned char *)malloc(PAHO_PLAYOAD_LEN_MAX)))
	{
		nwy_ext_echo("\r\nmalloc buffer g_nwy_user_readbuf error");
		return;
	}
	memset(g_nwy_user_readbuf, 0, PAHO_PLAYOAD_LEN_MAX);
	memset(g_nwy_user_writebuf, 0, PAHO_PLAYOAD_LEN_MAX);
	if (paho_user_network != NULL)
	{
		NetworkDisconnect(paho_user_network);
		free(paho_user_network);
		paho_user_network = NULL;
	}
	if (NULL == (paho_user_network = (Network *)malloc(sizeof(Network))))
	{
		nwy_ext_echo("\r\nmalloc buffer paho_user_network error");
		return;
	}
	memset(paho_user_network, 0, sizeof(Network));
	NetworkInit(paho_user_network);
	paho_mqtt_user_param.cleanflag = 0;
	paho_mqtt_user_param.keepalive = 60;
	paho_user_network->cid = 0;
	int rc = NetworkConnect(paho_user_network, paho_mqtt_user_param.host_name, paho_mqtt_user_param.port);
	if (rc < 0)
	{
		nwy_ext_echo("\r\nNetworkConnect err rc=%d", rc);
		NetworkDisconnect(paho_user_network);
		return;
	}
	MQTTClientInit(&paho_user_client, paho_user_network, 10000, g_nwy_user_writebuf, PAHO_PLAYOAD_LEN_MAX, g_nwy_user_readbuf, PAHO_PLAYOAD_LEN_MAX);
	MQTTClientInit_defaultMessage(&paho_user_client, UsermessageArrived);
	datatemp.clientID.cstring = paho_mqtt_user_param.clientID;
	if (0 != strlen((char *)paho_mqtt_user_param.username) && 0 != strlen((char *)paho_mqtt_user_param.password))
	{
		datatemp.username.cstring = paho_mqtt_user_param.username;
		datatemp.password.cstring = paho_mqtt_user_param.password;
	}
	datatemp.keepAliveInterval = paho_mqtt_user_param.keepalive;
	datatemp.cleansession = paho_mqtt_user_param.cleanflag;
	if (0 != strlen((char *)paho_mqtt_user_param.willtopic))
	{
		memset(&datatemp.will, 0x0, sizeof(datatemp.will));
		datatemp.willFlag = 1;
		datatemp.will.retained = paho_mqtt_user_param.willretained;
		datatemp.will.qos = paho_mqtt_user_param.willqos;
		if (paho_mqtt_user_param.willmessage_len != 0)
		{
			datatemp.will.topicName.lenstring.data = paho_mqtt_user_param.willtopic;
			datatemp.will.topicName.lenstring.len = strlen((char *)paho_mqtt_user_param.willtopic);
			datatemp.will.message.lenstring.data = paho_mqtt_user_param.willmessage;
			datatemp.will.message.lenstring.len = paho_mqtt_user_param.willmessage_len;
		}
		else
		{
			datatemp.will.topicName.cstring = paho_mqtt_user_param.willtopic;
			datatemp.will.message.cstring = paho_mqtt_user_param.willmessage;
		}
		nwy_ext_echo("\r\nMQTT will ready");
	}
	nwy_ext_echo("\r\nConnecting MQTT");
	rc = nwy_MQTTConnect(&paho_user_client, &datatemp);
	if (rc)
	{
		nwy_ext_echo("\r\nFailed to create client, return code %d", rc);
		MQTTDisconnect(&paho_user_client);
		NetworkDisconnect(paho_user_network);
		return;
	}
	else
	{
		nwy_ext_echo("\r\nuser MQTT connect ok");
		nwy_osiThread_t *task_id = nwy_paho_yeild_usertask_init();
		if (task_id == NULL)
		{
			nwy_ext_echo("\r\npaho yeid task create failed ");
			MQTTDisconnect(&paho_user_client);
			NetworkDisconnect(paho_user_network);
			return;
		}
		else
		{
			nwy_ext_echo("\r\npaho yeid task create ok ");
			api_UpdateForInte(); // 连接用户主站成功 进行网络校时
			//清除3相断路标志
			#if (EVENT_REPORT == YES)
			get_N176_time(&tTime);
			qwMs = getmktimems(&tTime);
			EventData.TimeMs = qwMs;
			EventData.Number = 0;
			memcpy(EventData.Warning,(char*)&eventbuf,sizeof(EventData.Warning));
			if (nwy_get_queue_spaceevent_cnt(EventReportMessageQueue) != 0)//上电清除所有告警标志
			{
				nwy_put_msg_que(EventReportMessageQueue, &EventData, 0xffffffff);
			}
			#endif
		}
	}
}
void nwy_User_MQTT_Reconnect_timer_cb(void *type)
{
	UserMqttReconnectTimerFlag = 1;
}
//--------------------------------------------------
// 功能描述:	登录用户主站
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void USER_MQTT_Task(void *param)
{
	nwy_sleep(10000);
	DWORD Connectfre = 0;

	while (1)
	{
		if (UserMqttReconnectTimerFlag == 1)
		{
			if (0 != nwy_ext_check_data_connect())
			{
				if (MQTTIsConnected(&paho_user_client) != 1)
				{
					UserMqttConnectStatus = 0;
					MQTT_connetuser();							 // 退出机制
					if (MQTTIsConnected(&paho_user_client) != 1) //没有连接成功，再进行延时，连接成功不再进行延时
					{
						if (Connectfre < 20)
						{
							Connectfre++;
							UserMqttReconnectTimerFlag = 0;
							nwy_start_timer(User_MQTT_Reconnect_timer, 6000 * Connectfre);
						}
						else
						{
							UserMqttReconnectTimerFlag = 0;
							nwy_start_timer(User_MQTT_Reconnect_timer, 3600000 * 4); // 4小时重试一次连接 每次连接消耗560字节
						}
					}
					else
					{
						Connectfre = 0;
					}
				}
				else
				{
					UserMqttConnectStatus = 1;
					Connectfre = 0;
					MQTT_UserServices();
				}
			}
		}

		nwy_sleep(500);
	}
}
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
BOOL api_GetMqttConStatus( BYTE bNum )
{
	if (bNum == 0)
	{
		if (MQTTIsConnected(&paho_mqtt_client) == 1)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}
	else if(bNum == 1)
	{
		#if (MQTT_USER == YES)
		if (MQTTIsConnected(&paho_user_client) == 1)
		{
			return TRUE;
		}
		else
		#endif
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

}
