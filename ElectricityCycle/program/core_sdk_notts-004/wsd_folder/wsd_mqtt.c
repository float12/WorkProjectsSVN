//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"

#define TIME_MSEC_OFFSET (8 * 60 * 60 * 1000)
#define DATA_MAX_NUM 31

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define MQTT_RECONN_RESET_TIMES 	(60*4) // 4小时链接不上 模块复位
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
#if (CYCLE_REPORT == YES)
typedef enum
{
	eUa = 0, // 电压数据块
	eUb,
	eUc,
	eLineUa, // 线电压数据块
	eLineUb,
	eLineUc,
	eIzs, // 零序电流
	eIa,  // 电流数据块
	eIb,
	eIc,
	eP1, // 有功功率数据块
	eP2,
	eP3,
	eP4,
	eQ1, // 无功功率数据块
	eQ2,
	eQ3,
	eQ4,
	eS1, // 视在功率数据块
	eS2,
	eS3,
	eS4,
	ePF1, // 功率因数数据块
	ePF2,
	ePF3,
	ePF4,
	eFre,		 // 频率
	ePActiveAll, // 正向有功总电能
	ePActive1,
	ePActive2,
	ePActive3,
	ePActive4,
	ePActive5,
	eNActiveAll, // 反向有功总电能
	eNActive1,
	eNActive2,
	eNActive3,
	eNActive4,
	eNActive5,
	eRActive1All, // 无功1总电能
	eRActive1R1,
	eRActive1R2,
	eRActive1R3,
	eRActive1R4,
	eRActive1R5,
	eRActive2All, // 无功2总电能
	eRActive2R1,
	eRActive2R2,
	eRActive2R3,
	eRActive2R4,
	eRActive2R5,
	eNullType,
} eValueType;
typedef struct
{
	struct T_MqttData *TMqttData[DATA_MAX_NUM];
} TMqttArrayData;
typedef union
{
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
	eCollectionToData, // 数组内包数组嵌套特殊数据
	eGetMeterAddr,	   // 数据直接从电表获取
	eGetTimeMs,		   // 获取当前时间戳
	eGetImei,		   // 模块imei号
	eSelfName,		   // 调用上层id
	eSelfValueType,	   // 调用上层value type
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
	struct TDataTable *const NextTable[DATA_MAX_NUM];
	void *Bak;
} TDataTable;

TDataTable CollectionToData = 
{
	.CJsonFunc = cJSON_CreateObject,
	.NextTable[0] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "meteId",
		.MqttData.DataType = eSelfName,
	},
	.NextTable[1] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "value",
		.MqttData.DataType = eSelfValueType,
	},
	.NextTable[2] = &(TDataTable){
		.CJsonFunc = cJSON_AddNumberToObject,
		.MqttData.DataName = "timestamp",
		.MqttData.DataType = eGetTimeMs,
	},
};

TDataTable QdMqttUpDataTable = 
{
	.CJsonFunc = cJSON_CreateObject,
	.NextTable[0] = &(TDataTable){
		.CJsonFunc = cJSON_AddObjectToObject,
		.MqttData.DataName = "dataList",
		.NextTable[0] = &(TDataTable){
			.CJsonFunc = cJSON_AddObjectToObject,
			.MqttData.DataName = "telemeter",
			.NextTable[0] = &(TDataTable){
				.CJsonFunc = cJSON_AddStringToObject,
				.MqttData.DataName = "MeterAddr",
				.MqttData.DataType = eGetMeterAddr,
			},
			.NextTable[1] = &(TDataTable){
				.CJsonFunc = cJSON_AddArrayToObject,
				.MqttData.DataName = "data",
				.NextTable[0] = &(TDataTable){
					.CJsonFunc = cJSON_AddItemToArray,
					.MqttData.DataType = eArrayValue,
					.MqttData.DataValue.MqttDataArray = &(TMqttArrayData){
						.TMqttData[0] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321112C00001",
							.ValueType = eP1,
						},
						.TMqttData[1] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321112400001",
							.ValueType = ePF1,
						},
						.TMqttData[2] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113900001",
							.ValueType = eFre,
						},
						.TMqttData[3] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321110100001",
							.ValueType = eUa,
						},
						.TMqttData[4] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321110200001",
							.ValueType = eUb,
						},
						.TMqttData[5] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321110300001",
							.ValueType = eUc,
						},
						.TMqttData[6] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321110900001",
							.ValueType = eIa,
						},
						.TMqttData[7] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321110A00001",
							.ValueType = eIb,
						},
						.TMqttData[8] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321110B00001",
							.ValueType = eIc,
						},
						.TMqttData[9] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113400001",
							.ValueType = ePActiveAll,
						},
						.TMqttData[10] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113401001",
							.ValueType = ePActive1,
						},
						.TMqttData[11] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113402001",
							.ValueType = ePActive2,
						},
						.TMqttData[12] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113403001",
							.ValueType = ePActive3,
						},
						.TMqttData[13] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113404001",
							.ValueType = ePActive4,
						},
						.TMqttData[14] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113405001",
							.ValueType = ePActive5,
						},
						.TMqttData[15] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113400002",
							.ValueType = eNActiveAll,
						},
						.TMqttData[16] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113401002",
							.ValueType = eNActive1,
						},
						.TMqttData[17] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113402002",
							.ValueType = eNActive2,
						},
						.TMqttData[18] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113403002",
							.ValueType = eNActive3,
						},
						.TMqttData[19] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113404002",
							.ValueType = eNActive4,
						},
						.TMqttData[20] = &(T_MqttData){
							.DataType = eCollectionToData,
							.DataName = "321113405002",
							.ValueType = eNActive5,
						},
					},
				},
			},
		},
	},
	.NextTable[1] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "deviceId",
		.MqttData.DataType = eGetImei,
	},
	.NextTable[2] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "requestId",
		.MqttData.DataValue.StringValue = "093780FC691C0540B5F047B9C516E318",
	},
	.NextTable[3] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "type",
		.MqttData.DataValue.StringValue = "CMD_REPORT_CYCLEDATA",
	},
	.NextTable[4] = &(TDataTable){
		.CJsonFunc = cJSON_AddNumberToObject,
		.MqttData.DataName = "timestamp",
		.MqttData.DataType = eGetTimeMs,
	}};

TDataTable ZhMqttUpDataTable = {
	.CJsonFunc = cJSON_CreateObject,
	.NextTable[0] = &(TDataTable){
		.CJsonFunc = cJSON_AddStringToObject,
		.MqttData.DataName = "ID",
		.MqttData.DataType = eGetMeterAddr,
	},
	.NextTable[1] = &(TDataTable){
		.CJsonFunc = cJSON_AddNumberToObject,
		.MqttData.DataName = "T",
		.MqttData.DataType = eGetTimeMs,
	},
	.NextTable[2] = &(TDataTable){
		.CJsonFunc = cJSON_AddObjectToObject,
		.MqttData.DataName = "DS",
		.NextTable[0] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0001",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eUa,

		},
		.NextTable[1] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0002",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eUb,
		},
		.NextTable[2] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0003",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eUc,
		},
		.NextTable[3] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1121",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eLineUa,

		},
		.NextTable[4] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1122",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eLineUb,
		},
		.NextTable[5] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1123",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eLineUc,
		},
		.NextTable[6] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1116",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eIzs,

		},
		.NextTable[7] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0004",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eIa,
		},
		.NextTable[8] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0005",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eIa,
		},
		.NextTable[9] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0006",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eIc,
		},
		.NextTable[10] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0012",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eP1,
		},
		.NextTable[11] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "000C",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eP2,
		},
		.NextTable[12] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "000D",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eP3,
		},
		.NextTable[13] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "000E",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eP4,
		},
		.NextTable[14] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0013",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eQ1,
		},
		.NextTable[15] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "000F",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eQ2,
		},
		.NextTable[16] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0010",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eQ3,
		},
		.NextTable[17] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0011",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eQ4,
		},
		.NextTable[18] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1468",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eS1,
		},
		.NextTable[19] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1465",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eS2,
		},
		.NextTable[20] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1466",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eS3,
		},
		.NextTable[21] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1467",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eS4,
		},
		.NextTable[22] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0008",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = ePF1,
		},
		.NextTable[23] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0009",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = ePF2,
		},
		.NextTable[24] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "000A",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = ePF3,
		},
		.NextTable[25] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "000B",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = ePF4,
		},
		.NextTable[26] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0007",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eFre,
		},
		.NextTable[27] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0014",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = ePActiveAll,
		},
		.NextTable[28] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1233",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eNActiveAll,
		},
		.NextTable[29] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "0015",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eRActive1All,
		},
		.NextTable[30] = &(TDataTable){
			.CJsonFunc = cJSON_AddStringToObject,
			.MqttData.DataName = "1234",
			.MqttData.DataType = eFromCollection,
			.MqttData.ValueType = eRActive2All,
		},
	}};
#endif // #if( CYCLE_REPORT ==  YES)

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
nwy_sim_result_type imei = {0};
ReportPara_txt ReportPara;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
MQTTClient paho_mqtt_client;
unsigned char *g_nwy_paho_readbuf = NULL;
unsigned char *g_nwy_paho_writebuf = NULL;
nwy_osiThread_t *nwy_paho_task_id = NULL;
Network *paho_network = NULL;
nwy_paho_mqtt_at_param_type paho_mqtt_at_param = {0};
TRealTimer LastConnectTime = {0};

int mqtt_sub_flag = 0, mqtt_unsub_flag = 0, mqtt_pub_flag = 0;
const char ServerIP[] = "218.201.129.20"; //"121.41.60.84";   //"119.167.163.7";//"58.210.240.122";////
char MqttID[sizeof(imei.nImei)];
const char MqttIDConst[] = "861996071005257";
const char RequestID[] = "093780FC691C0540B5F047B9C516E318";
const char ServerUserName[] = "wisdom";
const char ServerUserPwd[] = "wisdom2024";
const uint16 ServerPort = 6211; // 6161;//6183;
//
static char topic_pub[20][64];
char c_tail_upAllGradeAck[] = "/waveRecord/blecs/wsd2024/upGradeAck";
char c_tail_appVersionAck[] = "/waveRecord/blecs/wsd2024/appVersionAck";
char c_tail_resetAck[] = "/resetAck";
char c_tail_cycleData[] = "/cycleData";
char c_tail_changedata[] = "/transReply";
char c_tail_getTimeAck[] = "/getTimeAck";
char c_tail_setTimeAck[] = "/setTimeAck";
char c_tail_dataConfigAck[] = "/dataConfigAck";
char c_tail_upGradeAck[] = "/upGradeAck";

// 订阅
char c_tail_deviceappVersion[] = "/appVersion";
char c_tail_upGrade[] = "/upGrade";
char c_tail_reset[] = "/reset";
char c_tail_getTime[] = "/getTime";
char c_tail_setTime[] = "/setTime";
char c_tail_AllupGrade[] = "/waveRecord/BleandGps/wsd20241204/AllUpGrade";

double DoubleData[MAX_SAMPLE_CHIP][SINGLE_LOOP_ITEM + MAX_RATIO * 4];

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
#if (EVENT_REPORT == YES)
static BOOL pub_reportEventdata(Eventmessage EventData)
{
	cJSON *pJsonRoot;
	char *c_message;
	MQTTMessage pubmsg = {0};
	char Addru[30] = {0};

	memcpy(Addru, ReportPara.id, sizeof(ReportPara.id)); // 上传ID 与表地址无关联
	memset(paho_mqtt_at_param.topic, 0, sizeof(paho_mqtt_at_param.topic));
	strncpy(paho_mqtt_at_param.topic, &topic_pub[16][0], strlen(&topic_pub[16][0]));
	paho_mqtt_at_param.qos = 1;
	paho_mqtt_at_param.retained = 10;
	pJsonRoot = cJSON_CreateObject();
	if (NULL == pJsonRoot)
		return FALSE;
	cJSON_AddNumberToObject(pJsonRoot, "DI", EventData.DIStatus);
	cJSON_AddStringToObject(pJsonRoot, "ID", Addru);
	cJSON_AddNumberToObject(pJsonRoot, "T", EventData.TimeMs);

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
		cJSON_Delete(pJsonRoot);
		free(c_message);
		return FALSE;
	}
	cJSON_Delete(pJsonRoot);
	free(c_message);
	return TRUE;
}
#endif
#if (CYCLE_REPORT == YES)
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
			sprintf(strTemp, "%f", dbtemp); // 后期乘变比后，字符串小数位可能很长！！！ 小数位可能需统一处理
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, strTemp);
		}
		else if (DataTable->MqttData.DataType == eSelfName)
		{
			cJsonOut = cJSON_AddStringToObject(cJsonOut, DataTable->MqttData.DataName, name);
		}
		else if (DataTable->MqttData.DataType == eSelfValueType)
		{
			dbtemp = DoubleData[bLoop][ValueType];
			sprintf(strTemp, "%f", dbtemp); // 后期乘变比后，字符串小数位可能很长！！！ 小数位可能需统一处理
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
		for (i = 0; i < DATA_MAX_NUM; i++)
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
				else if (DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataType == eCollectionToData) // 嵌套同类型结构
				{
					cJsonTemp = DataToCJson(&CollectionToData, DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->DataName, DataTable->MqttData.DataValue.MqttDataArray->TMqttData[i]->ValueType, bLoop, cJsonTemp, qwTms, addr);
					cJSON_AddItemToArray(cJsonOut, cJsonTemp);
				}
			}
		}
	}
	for (i = 0; i < DATA_MAX_NUM; i++)
	{
		if (DataTable->NextTable[i] != NULL)
		{
			DataToCJson(DataTable->NextTable[i], name, ValueType, bLoop, cJsonOut, qwTms, addr);
		}
	}
	return cJsonOut;
}

static BOOL pub_cycledata(CollectionDatas CollectData)
{
	MQTTMessage pubmsg = {0};
	char *c_message;
	cJSON *pJsonRoot;
	#if (EVENT_REPORT == YES)
	cJSON *pJsonTemp;
	#endif

	BYTE i = 0;
	char Addru[30];

	// 准备主题
	memset(Addru, 0, sizeof(Addru));
	memset(paho_mqtt_at_param.topic, 0, sizeof(paho_mqtt_at_param.topic));
	strncpy(paho_mqtt_at_param.topic, &topic_pub[3][0], strlen(&topic_pub[3][0]));
	nwy_ext_echo("ADDRIS:%s", CollectData.Addr);
	paho_mqtt_at_param.qos = 1;		  // atoi(sptr);
	paho_mqtt_at_param.retained = 10; //
	// 准备消息
	// memcpy(Addru, CollectData.Addr, sizeof(Addru));
	memcpy(Addru, ReportPara.id, sizeof(ReportPara.id)); // ZH上传ID 与表地址无关联
	for (i = 0; i < MAX_SAMPLE_CHIP; i++)
	{
		// Addru[11] = '1' + i;//当前地址++仅适用于青岛联通
		memcpy((double *)&DoubleData[i][0], (double *)&CollectData.CollectMeter[i][0], sizeof(double) * (SINGLE_LOOP_ITEM + MAX_RATIO * 4));
		pJsonRoot = DataToCJson(&ZhMqttUpDataTable, NULL, 0, i, NULL, CollectData.TimeMs, Addru);
		#if (EVENT_REPORT == YES)
		pJsonTemp = cJSON_GetObjectItem(pJsonRoot, "DS");
		cJSON_AddNumberToObject(pJsonTemp, "0090", CollectData.bDI);
		#endif
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
			#if (EVENT_REPORT == YES)
			cJSON_Delete(pJsonTemp);
			#endif
			return FALSE;
		}
		free(c_message);
		cJSON_Delete(pJsonRoot);
		#if (EVENT_REPORT == YES)
		cJSON_Delete(pJsonTemp);
		#endif
		nwy_sleep(1000);
	}
	return TRUE;
}
#endif // #if( CYCLE_REPORT ==  YES)
static void init_topic(void)				//初始化主题  和订阅主题 需要修改 方便后期增加或删减主题
{
	char c_devic[] = "/waveRecord/";
	int i_pos = 0;
	int len = 0, i = 0, j = 0;

	memset(&topic_pub[0][0], 0, sizeof(topic_pub));
	// topic 初始化
	for (i = 0; i < 20; i++)
	{
		memcpy(&topic_pub[i][0], c_devic, sizeof(c_devic));
		len = strlen(&topic_pub[i][0]);
		memcpy(&topic_pub[i][len], MqttID, sizeof(MqttID));
	}
	i = 0;
	len = strlen(&topic_pub[i_pos][0]);
	// 0设备注册
	memcpy(&topic_pub[i++][0], c_tail_upAllGradeAck, sizeof(c_tail_upAllGradeAck));
	// 1设备心跳
	memcpy(&topic_pub[i++][0], c_tail_appVersionAck, sizeof(c_tail_appVersionAck));
	// 2设备接入
	memcpy(&topic_pub[i++][len], c_tail_resetAck, sizeof(c_tail_resetAck));
	// 3数据周期
	memcpy(&topic_pub[i++][0], c_tail_resetAck, sizeof(c_tail_resetAck));
	// 4数据变化
	memcpy(&topic_pub[i++][len], c_tail_changedata, sizeof(c_tail_changedata));
	// 5告警
	memcpy(&topic_pub[i++][len], c_tail_getTimeAck, sizeof(c_tail_getTimeAck));
	// 6请求数据响
	memcpy(&topic_pub[i++][len], c_tail_setTimeAck, sizeof(c_tail_setTimeAck));
	// 8参数配置上报
	memcpy(&topic_pub[i++][len], c_tail_dataConfigAck, sizeof(c_tail_dataConfigAck));
	// 9升级回复
	memcpy(&topic_pub[i++][len], c_tail_upGradeAck, sizeof(c_tail_upGradeAck));

	// 订阅 全部升级
	memcpy(&topic_pub[i++][0], c_tail_AllupGrade, sizeof(c_tail_AllupGrade));
	// 订阅 设备注册响应
	memcpy(&topic_pub[i++][len], c_tail_deviceappVersion, sizeof(c_tail_deviceappVersion));
	// 订阅 请求数据
	memcpy(&topic_pub[i++][len], c_tail_upGrade, sizeof(c_tail_upGrade));
	// 订阅 设置参数上报
	memcpy(&topic_pub[i++][len], c_tail_reset, sizeof(c_tail_reset));
	// 订阅 获取时间
	memcpy(&topic_pub[i++][len], c_tail_getTime, sizeof(c_tail_getTime));
	// 订阅 设置时间
	memcpy(&topic_pub[i++][len], c_tail_setTime, sizeof(c_tail_setTime));
	// 遥信事件上报
	// memcpy(&topic_pub[16][0], ReportPara.eventTopic, sizeof(ReportPara.eventTopic));
	// 设备数据上报
	for (j = 0; j < i; j++)
		nwy_ext_echo("\r\ninit_topic:[%d]%s  ", j, &topic_pub[j][0]);
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
	cJSON_AddStringToObject(pJsonRoot, "requestId", RequestID);
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
	cJSON_AddStringToObject(pJsonRoot, "wave_meterAddr", LtuAddr);
	// cJSON_AddStringToObject(pJsonRoot, "moduleVersion", ModuleVersion);
	
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
#if (TRANS_SWITCH == MQTT_TRANS)
//--------------------------------------------------
// 功能描述:  上传透传数据
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL pubTranData(tTranData *data)
{
	char output[UART_BUFF_MAX_LEN * 2 + 1] = {0}; // 字符串最大长度
	BYTE i = 0;

	if ((data->buf[0] == '0') && (data->buf[1] == '0') && (data->buf[2] == '0') && (data->buf[3] == '0') && (data->buf[4] == '0'))
	{
		if (RemessageF("CMD_DEVICE_TRANDATA", &topic_pub[0][0], "s", "RecData", "error"))
		{
			nwy_ext_echo("\r\ntranData err");
			return TRUE;
		}
		else
		{
			nwy_ext_echo("\r\nremessage err");
			return FALSE;
		}
	}
	else
	{
		for (i = 0; i < (data->len); i++)
		{
			sprintf(output + i * 2, "%02X", (unsigned char)data->buf[i]);
		}
		output[(data->len) * 2] = '\0';
		if (RemessageF("CMD_DEVICE_TRANDATA", &topic_pub[0][0], "s", "RecData", output))
		{
			nwy_ext_echo("\r\nUploadTranData %s", output);
			return TRUE;
		}
		else
		{
			nwy_ext_echo("\r\nremessage err");
			return FALSE;
		}
	}
}
#endif
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
	RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[8][0], "ds", "code", 200.0l, "msg", "SUCCESS!!!");		//临时修改
	RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "ds", "code", 200.0l, "msg", "SUCCESS");
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
	int GetTemp;
	BYTE UpgradeReply = 0;
	char *msg = NULL;
	FtpUpdatePara ftpupdatepara;
	CollectionDatas CollectData;
	char reqbuf[2048] = {0};
	#if (EVENT_REPORT == YES)
	Eventmessage EventData;
	#endif
	#if (TRANS_SWITCH == MQTT_TRANS)
	tTranData TouartTranData;
	tTranData UploadTranData;

	memset(&UploadTranData, 0, sizeof(UploadTranData));
	memset(&TouartTranData, 0, sizeof(TouartTranData));
	#endif
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

			// 追加命令用来识别是否给电表升级
			cJSON_GetObjectValue(pRoot, "Mode_switch", ecJSON_String, reqbuf, sizeof(reqbuf));
			if (strcmp(reqbuf, "Meter_upgrade_698") == 0)
			{
				if (tIap_698.dwIapFlag == 0)
				{
					tIap_698.dwIapFlag = DOWNLOAD_METERFILE;
					tIap_698.IsModuleUpgrade = 0;
					StartUpgradeRemessage(&ftpupdatepara);
				}
				else // 处于升级过程中 回复upgrade in progess
				{
					RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "ds", "code", 200.0l, "msg", "698 upgrade in progess");
				}
			}
			else if (strcmp(reqbuf, "Meter_upgrade_698_module") == 0)
			{
				if (tIap_698.dwIapFlag == 0)
				{
					nwy_ext_echo("\r\nrec module upgrade");
					tIap_698.dwIapFlag = DOWNLOAD_METERFILE;
					tIap_698.IsModuleUpgrade = 1;
					StartUpgradeRemessage(&ftpupdatepara);
				}
				else
				{
					RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "ds", "code", 200.0l, "msg", "698 module upgrade in progess");
				}
			}
			else if (strcmp(reqbuf, "Meter_upgrade_645") == 0)
			{
				if (tIap_645.dwIapFlag == 0)
				{
					tIap_645.dwIapFlag = DOWNLOAD_METERFILE;
					StartUpgradeRemessage(&ftpupdatepara);
				}
				else
				{
					RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "ds", "code", 200.0l, "msg", "645 upgrade in progess");
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
					RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "ds", "code", 200.0l, "msg", "4G upgrade in progess");
				}
			}
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_APPVERSION"))
		{ // 读版本
			char APP_BUILD_TIME[65] = {0};
			sprintf(APP_BUILD_TIME, "%s,%s", __DATE__, __TIME__);
			RemessageF("CMD_DEVICE_APPVERSION", &topic_pub[1][0], "s", "apptime", APP_BUILD_TIME);
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_GETTIME"))
		{ // 读时钟
			RemessageF("CMD_DEVICE_GETTIME", &topic_pub[5][0], "s", "DateTime", get_N176_time_stringp());
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_SETTIME"))
		{
			// 设置时间
			cJSON_GetObjectValue(pRoot, "DateTime", ecJSON_String, reqbuf, sizeof(reqbuf));
			set_N176_time_stringp(reqbuf);
			RemessageF("CMD_DEVICE_SETTIME", &topic_pub[6][0], "");
			cJSON_Delete(pRoot);
		}
		#if (TRANS_SWITCH == MQTT_TRANS)
		else if (strstr(pType->valuestring, "CMD_DEVICE_TRANDATA"))
		{
			nwy_ext_echo("\r\nmqtt service rec CMD_DEVICE_TRANDATA");
			cJSON_GetObjectValue(pRoot, "data", ecJSON_String, reqbuf, sizeof(reqbuf));
			TouartTranData.TranType = SERVER_TRAN;
			TouartTranData.len = (strlen(reqbuf) / 2) + 1;
			for (BYTE i = 0; i < (strlen(reqbuf) / 2); i++)
			{
				sscanf(&reqbuf[i * 2], "%2hhx", &(TouartTranData.buf[i]));
			}
			if (nwy_put_msg_que(TranDataToUartMessageQueue, &TouartTranData, 0xffffffff))
			{
				nwy_ext_echo("\r\nmqtt put tran message to uart success");
			}
			else
			{
				nwy_ext_echo("\r\nmqtt put tran message to uart fail");
			}
			cJSON_Delete(pRoot);
		}
		#endif
		else if (strstr(pType->valuestring, "CMD_DEVICE_GETRAM"))
		{
			nwy_dm_get_device_heapinfo(reqbuf);
			RemessageF("CMD_DEVICE_GETRAM", &topic_pub[0][0], "s", "DeviceRam", reqbuf);
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_RESET"))
		{
			RemessageF("CMD_DEVICE_RESET", &topic_pub[0][0], "s", "Status", "StartReset");
			RemessageF("CMD_DEVICE_RESET", &topic_pub[2][0], "s", "Status", "StartReset");
			ResetFlag = 1;
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_MODULEVERSION"))
		{
			dwReadMeterFlag[0] |= (1 << (eBIT_LTO_MODULEVERSIONtoMQTT));
			cJSON_Delete(pRoot);
			nwy_sleep(1000);
			RemessageF("CMD_DEVICE_GETRAM", &topic_pub[0][0], "s", "ModuleVersion", ModuleVersion);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_SINGAL"))
		{
			BYTE bcsq;
			nwy_nw_get_signal_csq(&bcsq);
			double temp = bcsq;
			RemessageF("CMD_DEVICE_SIGNAL_STRENGTH", &topic_pub[0][0], "d","Signal",temp );
			cJSON_Delete(pRoot);
		}
		else if (strstr(pType->valuestring, "CMD_DEVICE_INITTF"))
		{
			if(api_DeleteAllWaveFile() == TRUE)
			{
				RemessageF("CMD_DEVICE_INITTF_REPLY", &topic_pub[0][0], "s","Result","Success" );
			}
			else
			{
				RemessageF("CMD_DEVICE_INITTF_REPLY", &topic_pub[0][0], "s","Result","Fail" );
			}
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
	#if (TRANS_SWITCH == MQTT_TRANS)
	if (nwy_get_msg_que(TranDataToTcpUserServerMsgQue, &UploadTranData, 0xffffffff))
	{
		nwy_ext_echo("\r\nmqtt get uploadLen from uart first:%d", UploadTranData.len);
		pubTranData(&UploadTranData);
	}
	#endif
	if (nwy_get_msg_que(CollectMessageQueue, &CollectData, 0xffffffff)) // 组MQTT帧进行发送
	{
	#if (CYCLE_REPORT == YES)
		pub_cycledata(CollectData);
	#endif
	}
	#if (EVENT_REPORT == YES)
	if (nwy_get_msg_que(EventReportMessageQueue, &EventData, 0xffffffff)) // 事件上报
	{

		pub_reportEventdata(EventData);
	}
	#endif
	if (nwy_get_msg_que(UpgradeResultMessageQueue, &UpgradeReply, 0xffffffff))
	{
		switch (UpgradeReply)
		{
		case eUpgradeExeSuc:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "execute Upgrade successful");
			break;
		case eUpgrdeFrameTimeoutErr:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "Frame timeout error");
			break;
		case eUpgradeCheckResultErr:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "Check result error");
			break;
		case eUpgradeVeriErr:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "file Verification error");
			break;
		case eUpgradeDARErr:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "DAR error");
			break;
		case eUpgradeWholeProTimeoutErr:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "Whole process timeout error");
			break;
		case eFtpLoginSuccess:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "ftp login success");
			break;
		case eFtpDownLoadSuccess:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "ftp download success");
			break;
		case eFtpModuleUpgradeFail:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "ftp module upgrade fail");
			break;
		case eFtpModuleUpgradeVerifyFail:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "ftp module upgrade verify fail");
			break;
		case eFtpFailExit:
			RemessageF("CMD_DEVICE_UPGRADE", &topic_pub[0][0], "s", "upgradeResult", "ftp module upgrade fail exit");
			break;
		default:
			break;
		}
	}
}

void messageArrived(MessageData *md)
{
	char *msg = NULL;

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
	/*code*/
	// 连接服务器
	memcpy(paho_mqtt_at_param.host_name, ServerIP, sizeof(Serverpara[ePARA_MQTT].ServerIP));
	paho_mqtt_at_param.port = ServerPort;
	memcpy(paho_mqtt_at_param.clientID, MqttID, sizeof(MqttID));
	memcpy(paho_mqtt_at_param.username, Serverpara[ePARA_MQTT].ServerUserName, sizeof(Serverpara[ePARA_MQTT].ServerUserName));
	memcpy(paho_mqtt_at_param.password, Serverpara[ePARA_MQTT].ServerUserPwd, sizeof(Serverpara[ePARA_MQTT].ServerUserPwd));
	paho_mqtt_at_param.paho_ssl_tcp_conf.sslmode = 0; // nossl
	if (g_nwy_paho_writebuf != NULL)				  // 改为数组
	{
		free(g_nwy_paho_writebuf);
		g_nwy_paho_writebuf = NULL;
	}
	if (NULL == (g_nwy_paho_writebuf = (unsigned char *)malloc(PAHO_PLAYOAD_LEN_MAX)))
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
	// paho_network->cid = 0;				//简配版使用时需将次此处屏蔽 待测试
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
		for (int i = 0; i < 6; i++)
		{
			memset(paho_mqtt_at_param.topic, 0, sizeof(paho_mqtt_at_param.topic));
			memcpy(Sub_topic, &topic_pub[9 + i][0], 64);
			strncpy(paho_mqtt_at_param.topic, Sub_topic, strlen(Sub_topic));
			nwy_ext_echo("\r\n topic_sub:[%s]\r\n", paho_mqtt_at_param.topic);

			paho_mqtt_at_param.qos = 0; //..atoi(sptr);
			rc = MQTTSubscribe(&paho_mqtt_client, (char *)paho_mqtt_at_param.topic, paho_mqtt_at_param.qos, messageArrived);
			if (rc == SUCCESS)
			{
				// nwy_ext_echo("\r\n-------------------MQTT Sub ok \r\n ");
			}
			else
				nwy_ext_echo("\r\nMQTT Sub error:%d", rc);
		}
	}
	// nwy_sleep(6000);
	// pub_cycledata(2);
	// nwy_ext_echo("\r\nNetworkConnect ok");
	// // 注册MQTT接收事件中断MQTT_event_handle
	// MQTT_event_handle(NULL,NULL,NULL);
	// 注册topic
	// 等待接收
	// 创建MQTT发送方面业务任务
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

void WSD_MQTT_Task(void *param)
{
	nwy_sleep(6000);
	nwy_sim_get_imei(&imei);
	memcpy(MqttID, imei.nImei, sizeof(imei.nImei));
	DWORD Connectfre = 0;
	static WORD ReconnectTime = 0;

	while (1)
	{
		ThreadRunCnt[eMqttThread]++;
		if (0 != nwy_ext_check_data_connect())
		{
			if (MQTTIsConnected(&paho_mqtt_client) != 1)
			{
				MQTT_connet();								 // 退出机制
				if (MQTTIsConnected(&paho_mqtt_client) != 1) // 没有连接成功，再进行延时，连接成功不再进行延时
				{
					if (Connectfre < 20)
					{
						Connectfre++;
						nwy_sleep(6000 * Connectfre);
					}
					else
					{
						if (ReconnectTime >= MQTT_RECONN_RESET_TIMES)
						{
							ResetFlag = 1;
						}
						nwy_sleep(60000); // 60秒重试一次连接 每次连接消耗560字节
						ReconnectTime++;
					}
				}
				else
				{
					ReconnectTime = 0;
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
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL api_GetMqttConStatus(void)
{
	if (MQTTIsConnected(&paho_mqtt_client) != 1)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}