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
	BYTE PassWord[5];//645����
	BYTE Type;
	OAD s_OAD;//�˿ں�
	BYTE TSA[10];//ͨ�ŵ�ַ
	BYTE TSALEngth;//ͨ�ŵ�ַ����
	BaudS bps;//������
}Record;
typedef struct Meter
{
	 DWORD  api_ActivePower;//�й�����
}LMeter;
typedef struct variate_t//�㷨ģ�����б���
{
	 BYTE Flag;
	 TCurrTimer CurrTimer;//ʱ��
	 WORD Voltage[3];//��ѹ
	 DWORD  Electric;//������
	 WORD PowerFactor[4];//��������
	 DWORD  electricity[3];//����
	 DWORD  api_ActivePower[4];//�й�����
	 DWORD  api_ReactivePower[4];//�޹�����
	 DWORD  api_ApparentPower[4];//���ڹ���
	 DWORD ErrTime;//�������
	 LMeter NaxtMeter;
   LMeter NaxtMeter2;
	 WORD RecordNum;//������
	 Record record[2];//ֻ����2������
	 BYTE Data[100];
	 BYTE Parameter[20];//һ�ι̶�20������
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

