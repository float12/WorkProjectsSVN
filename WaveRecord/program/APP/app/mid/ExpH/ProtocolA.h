#ifndef __PROTOCOL_A_H__
#define __PROTOCOL_A_H__
#include "_defA.h"

#define MODULEA_EVENT_START 0x00
#define MODULEA_EVENT_EVENT 0x01


#define TAG_STRUCT          2
#define TAG_DOUBLE_LONG     5
#define TAG_DOUBLE_LONG_U   6
#define TAG_LONG            16
#define TAG_LONG_U          18

typedef struct TCurrTimer_t
{
    WORD Year;
    BYTE Mon;
    BYTE Day;

    BYTE Hour;
    BYTE Min;
    BYTE Sec;
} TCurrTimer;
typedef struct BPS_t 
{
	BYTE Bauds;
	BYTE Check;
	BYTE DataLength;
	BYTE Stop;
}BaudS;
typedef struct Record_t
{
	BYTE PassWord[5];//645密码
	BYTE Type;
	OAD s_OAD;//端口号
	BYTE TSA[10];//通信地址
	BYTE TSALEngth;//通信地址长度
	BaudS bps;//波特率
}Record;
typedef struct Meter
{
	 DWORD  api_ActivePower;//有功功率
}LMeter;
typedef struct variate_t//算法模块所有变量
{
	 BYTE Flag;
	 TCurrTimer CurrTimer;//时间
	 WORD Voltage[3];//电压
	 DWORD  Electric;//电能量
	 WORD PowerFactor[4];//功率因素
	 DWORD  electricity[3];//电流
	 DWORD  api_ActivePower[4];//有功功率
	 DWORD  api_ReactivePower[4];//无功功率
	 DWORD  api_ApparentPower[4];//视在功率
	 DWORD ErrTime;//错误次数
	 LMeter NaxtMeter;
   LMeter NaxtMeter2;
	 WORD RecordNum;//档案数
	 Record record[2];//只保存2个档案
	 BYTE Data[100];
	 BYTE Parameter[20];//一次固定20个数据
	 BYTE Length; 
	 BYTE TIMEDAT[180];
} variate;
typedef enum eCOMDCB_t
{
    eCR_OAD = 0xF2090201,
    eBlueBooth_OAD = 0xF20B0201,
    eRS4851_OAD = 0xF2010201,
    eRS4852_OAD = 0xF2010202,
    eIR_OAD = 0xF2020201,
    e300bps = 0,
    e600bps,
    e1200bps,
    e2400bps,
    e4800bps,
    e7200bps,
    e9600bps,
    e19200bps,
    e38400bps,
    e57600bps,
    e115200bps,
}eCOMDCB;

WORD api_GetOADDataModuleA(DWORD OAD, BYTE *Buf, WORD BufLen);
WORD api_SetOADDataModuleA(DWORD OAD, BYTE *Buf, WORD BufLen);
WORD api_ActionOADDataModuleA(DWORD OAD, BYTE *Buf, WORD BufLen);

void api_GetCurrTimer(TCurrTimer *tCurrTimer);
void api_SyncCurrTimer(void);
void api_TriggerEventModuleA(WORD OI, BYTE Type);
void api_WriteParaModuleA(WORD OI, DWORD Addr, WORD Size, BYTE *Buf);
BYTE api_ReadParaModuleA(WORD OI, DWORD Addr, WORD Size, BYTE *Buf);

void api_InitProxyInfo(eCOMDCB Ch, eCOMDCB Bps, WORD FrameOverTim, WORD ByteOverTim);
WORD api_ProxyRequestMeterA(BYTE *Frame, WORD FrameLen, WORD BufLen);
WORD api_GetNormalRequestMeterA(DWORD OAD, BYTE *Buf, WORD BufLen);

#endif

