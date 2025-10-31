#ifndef __MDLT645_H__
#define __MDLT645_H__
#include "__def.h"
#include "port.h"
#include "usrfun.h"
#include "sysdefs.h"
#include "cgy.h"
#include "mgydbase.h"
//#include <cstring>
#include "mpruninfo.h"
#ifdef __cplusplus
	#if __cplusplus
	extern "C" {
	#endif	
#endif
#define MIN_RX645FM_SIZE	12
#define LEN_645FM_HEAD		10
#define LEN_645FM_TAIL		2
#define	MAX_RX645FM_SIZE	(230+LEN_645FM_HEAD+LEN_645FM_TAIL)

#define TBD	0xFFFF		//δ����
#define HH	3			//DWORD�����8λ����WORD�����4λ
#define HL	2
#define LH	1
#define	LL	0

//28���¼�
#define EVENT_GL_A			0	//A�����
#define EVENT_GL_B			1	//B�����
#define EVENT_GL_C			2	//C�����		
#define EVENT_OPEN_COVER	3	//�����		
#define EVENT_CCGR			4	//�㶨�ų�����
#define EVENT_CLEAR			5	//�����0
#define EVENT_POWER_OFF		6	//������
#define EVENT_POWER_ERR		7	//����Դ�쳣
#define EVENT_OPEN_COVEREX	8	//����ť��
#define EVENT_SY_A			9	//A��ʧѹ
#define EVENT_SY_B			10	//B��ʧѹ
#define EVENT_SY_C			11	//C��ʧѹ
#define EVENT_SL_A			12	//A��ʧ��
#define EVENT_SL_B			13	//B��ʧ��
#define EVENT_SL_C			14	//C��ʧ��
#define EVENT_V_ERRPHASE	15	//��ѹ������
#define EVENT_V_UNBAL		16	//��ѹ��ƽ��
#define EVENT_SETTIME		17	//��ʱ
#define EVENT_LOWV_A		18	//A��Ƿѹ
#define EVENT_LOWV_B		19	//B��Ƿѹ
#define EVENT_LOWV_C		20	//C��Ƿѹ
#define EVENT_GV_A			21	//A���ѹ
#define EVENT_GV_B			22	//B���ѹ
#define EVENT_GV_C			23	//C���ѹ
#define EVENT_SY_ALL		24	//ȫʧѹ
#define EVENT_DX_A			25	//A�����
#define EVENT_DX_B			26	//B�����
#define EVENT_DX_C			27	//C�����

//д����
#define CTRL_W_BROADCAST	0x08//�㲥Уʱ	

//������
#define NODATA				0x40
		
#pragma pack(1)
		
typedef struct _TMDlt645Fm{
	BYTE	byS68;				//��ʼ68�ַ�
	BYTE	Addr[6];
	BYTE    byE68;				//����68�ַ�
	BYTE    byCtrl;				//������
	BYTE	byDataLen;			//�����򳤶�
}TMDlt645Fm;

#pragma pack()

// void Tx_WriteFm(BOOL bDlt645_2007,BYTE byCtrl,DWORD dwDI,BYTE *pData,BYTE byDataLen,BYTE *pAddr,TTx *pTx);
void _Tx_Fm645Read(BOOL bDlt645_2007,DWORD dwDI,BYTE *pData,BYTE byDataLen,BYTE *pAddr,TTx *pTx);
// void Tx_Fm645Read(BOOL bDlt645_2007,DWORD dwDI,BYTE *pAddr,TTx *pTx);
DWORD Search645Frame(HPARA hBuf,DWORD dwLen,BYTE *pAddr,BOOL bDirect);
BYTE MDlt645_RxMonitor(BOOL bIsDlt6452007,BYTE *pBuf,DWORD *pdwRxDI,BYTE *pbyErr);
BOOL Sunitem(BOOL bIsDlt6452007,DWORD dwDI,DWORD dwFlagNo,BYTE byMaxNum,BYTE byPlace,BYTE byStarNum,DWORD *pdwDI);
void SunitemFlag(DWORD dwFlagNo,BOOL bIsSetFlag);
void SetSunitemFlag(BOOL bIs07,DWORD dwDI,BOOL bIsTx);
// BYTE MaterEvent(DWORD dwFlagNo,BYTE byMaxNum);
void WriteCurvEx(TTime *pTime,BYTE byStep,DWORD dwNo,long dwDD);
void WriteCurvXL(TTime *pTime,BYTE byNo,DWORD dwDD);
void Rx_DD_Ex(BYTE *pBuf,BYTE byLen,WORD wNo);
// void Tx_645Broadcast(BYTE byCtrl,BYTE *pBuf,BYTE byLen);
void Rx_Date(BYTE *pBuf);
void Rx_Time(BYTE *pBuf);
void Rx_DD(BYTE *pBuf,BYTE byLen,WORD wNo);
void Rx_MonDD(BYTE *pBuf,BYTE byLen,BYTE byStartNo);
void Rx_AutoDay(BYTE *pBuf,BYTE byLen,BYTE byNo);
// void Rx_BatTime(BYTE *pBuf,BYTE byLen);
// void Rx_Constant(BYTE *pBuf,BYTE byLen,BYTE byNo);
// void Rx_TimeSeg(BYTE *pBuf,BYTE byLen,BYTE byNo);
// void Rec_Pre(BYTE *pBuf, BYTE byLen,WORD wNo);
#ifndef DEBUG645
#define DEBUG645 YES 
#endif 

#ifdef __cplusplus
	#if __cplusplus
	}
	#endif
#endif
#endif
