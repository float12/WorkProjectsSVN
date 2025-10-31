//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述		DEF头文件
// 修改记录
//----------------------------------------------------------------------
#ifndef __DEF_H
#define __DEF_H


// 数据类型定义
#define BYTE unsigned char
#define WORD unsigned short
#define SWORD short
#define DWORD unsigned long
#define SDWORD long
#define BOOL unsigned char
#define QWORD unsigned long long
#define SQWORD long long

// 位定义
#define BIT0 0x0001
#define BIT1 0x0002
#define BIT2 0x0004
#define BIT3 0x0008
#define BIT4 0x0010
#define BIT5 0x0020
#define BIT6 0x0040
#define BIT7 0x0080
#define BIT8 0x0100
#define BIT9 0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

////////////////////////////////////
#define ILLEGAL_VALUE_8F 0xffffffff

// 逻辑定义
#define NO 0
#define YES 1

// #define FALSE				0
// #define TRUE				1

#define DATA_BCD 0
#define DATA_HEX 1	 // 16进制原码
#define DATA_HEXCOMP 2 // 16进制补码

#define READ 0
#define WRITE 1
#define CLRDT 2

#define UN_REPAIR_CRC 0 // 不修复CRC
#define REPAIR_CRC 1	// 修复CRC

#define FROM_BASE_METER				0
#define FROM_MODULE					1
////////////////////////////////////
//下行抄表规约定义
#define PROTOCOL_645 		(1)
#define PROTOCOL_698 		(2)
#define PROTOCOL_MODBUS 	(3)
#define PROTOCOL_MQTT		(4)
#define PROTOCOL_212		(5)
#define PROTOCOL_104		(6)
#define TCP_TRANS 0
#define MQTT_TRANS 1 /////////////////////////////////////////////////////////////////////////////////////////mqtt透传用的不多 感觉没必要开4K空间

#define SAFE_SPACE_START_ADDR (0)
#define GET_SAFE_SPACE_ADDR(Addr_Name) ((int)&(((TSafeMem *)SAFE_SPACE_START_ADDR)->Addr_Name))

#define COLLECT_DATA_ITEM_NUM 	(SINGLE_LOOP_ITEM + 21)// 单回路采集数据项数量  
#define NWY_UART_RECV_SINGLE_MAX 4096
#include "wsd_allhead.h"

extern int vsnprintf(char *, size_t, const char *, va_list);

#define NWY_EXT_SIO_RX_MAX (2 * 1024)
#define NWY_EXT_INPUT_RECV_MSG (NWY_APP_EVENT_ID_BASE + 1)
#define NWY_EXT_FOTA_DATA_REC_END (NWY_APP_EVENT_ID_BASE + 2)
#define NWY_EXT_APPIMG_FOTA_DATA_REC_END (NWY_APP_EVENT_ID_BASE + 3)

#define NWY_APPIMG_FOTA_BLOCK_SIZE (2 * 1024)


typedef struct dataRecvContext
{
	unsigned size;
	unsigned pos;
	char data[0];
} dataRecvContext_t;

typedef struct nwy_file_ftp_info_s
{
	int is_vaild;
	char filename[256];
	int pos;
	// int length;
	// int file_size;
} nwy_file_ftp_info_s;
typedef enum
{
	NWY_CUSTOM_IP_TYPE_OR_DNS_NONE = -1,
	NWY_CUSTOM_IP_TYPE_OR_DNS_IPV4 = 0,
	NWY_CUSTOM_IP_TYPE_OR_DNS_IPV6 = 1,
	NWY_CUSTOM_IP_TYPE_OR_DNS_DNS = 2
} nwy_ip_type_or_dns_enum;

/*Begin by zhaoyating to control tripple sim 20220615*/
#define MAX_INPUT_OPTION 4 /* contain '\r' '\n' */
#define STD_SUCCESS 1
#define STD_ERROR 0
#define NWY_IS_DIGIT(x) ((((x - '0') >= 0) && ((x - '0') <= 9)) ? 1 : 0)
#define NWY_IS_HEXSTR(x) (((((x - '0') >= 0) && ((x - '0') <= 9)) || ((x >= 'a') && (x <= 'f')) || ((x >= 'A') && (x <= 'F'))) ? 1 : 0)
static const char APP_VERSION[65] = "WSD_APP_V1.0.1";
void nwy_ext_echo(char *fmt, ...);
int get_N176_time(TRealTimer *pTime);
void set_N176_time(TRealTimer *pTime);
char *get_N176_time_stringp();
BOOL set_N176_time_stringp(char *ctime);
int GetTimeto698(TRealTimerDate *pTime);
#define PACK __attribute__((packed, aligned(1)))


typedef struct collectionDatas_t
{
	char Addr[30];
	QWORD TimeMs;
	double CollectMeter[MAX_SAMPLE_CHIP][COLLECT_DATA_ITEM_NUM];
#if (EVENT_REPORT == YES)
	BYTE bDI;
#endif
} CollectionDatas;
#pragma pack(1)
typedef struct eventmessage_t
{
	WORD Number;
	char Warning[17];
	char Addr[30];
	QWORD TimeMs;
} Eventmessage;
#pragma pack()
#pragma pack(1)
typedef struct reportpara_t
{
	int reportfre;			   //周期上报频率
	char cycleDataTopic[64];   //周期上报主题
	char mqttClientld[64 * 2]; //mqtt client id
	char device[5][10];
	BYTE reserve[200];
} ReportPara_txt;
#pragma pack()
#pragma pack(1)
typedef struct serverpara_t
{
	uint16 ServerPort;
	char ServerIP[32];
	char ServerUserName[32];
	char ServerUserPwd[32];
	char ServerAPN[32]; //apn	以下标[0]的apn 为主
	WORD wCrc;
} ServerParaRam_t;
#pragma pack()

typedef struct TSafeMem_t
{
	ServerParaRam_t Serverpara[2];
	ReportPara_txt ReportPara;
} TSafeMem;

extern ReportPara_txt ReportPara;
extern ServerParaRam_t Serverpara[MAX_SPECIAL_USER_NUM];
#endif // #ifndef __DEF_H
