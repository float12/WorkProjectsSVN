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

#define TBD	0xFFFF		//未定义
#define HH	3			//DWORD的最高8位或者WORD的最高4位
#define HL	2
#define LH	1
#define	LL	0

//28个事件
#define EVENT_GL_A			0	//A相过流
#define EVENT_GL_B			1	//B相过流
#define EVENT_GL_C			2	//C相过流		
#define EVENT_OPEN_COVER	3	//开表盖		
#define EVENT_CCGR			4	//恒定磁场干绕
#define EVENT_CLEAR			5	//电表清0
#define EVENT_POWER_OFF		6	//电表掉电
#define EVENT_POWER_ERR		7	//电表电源异常
#define EVENT_OPEN_COVEREX	8	//开端钮盖
#define EVENT_SY_A			9	//A相失压
#define EVENT_SY_B			10	//B相失压
#define EVENT_SY_C			11	//C相失压
#define EVENT_SL_A			12	//A相失流
#define EVENT_SL_B			13	//B相失流
#define EVENT_SL_C			14	//C相失流
#define EVENT_V_ERRPHASE	15	//电压逆相序
#define EVENT_V_UNBAL		16	//电压不平衡
#define EVENT_SETTIME		17	//对时
#define EVENT_LOWV_A		18	//A相欠压
#define EVENT_LOWV_B		19	//B相欠压
#define EVENT_LOWV_C		20	//C相欠压
#define EVENT_GV_A			21	//A相过压
#define EVENT_GV_B			22	//B相过压
#define EVENT_GV_C			23	//C相过压
#define EVENT_SY_ALL		24	//全失压
#define EVENT_DX_A			25	//A相断相
#define EVENT_DX_B			26	//B相断相
#define EVENT_DX_C			27	//C相断相

//写命令
#define CTRL_W_BROADCAST	0x08//广播校时	

//控制字
#define NODATA				0x40
		
#pragma pack(1)
		
typedef struct _TMDlt645Fm{
	BYTE	byS68;				//起始68字符
	BYTE	Addr[6];
	BYTE    byE68;				//结束68字符
	BYTE    byCtrl;				//控制字
	BYTE	byDataLen;			//数据域长度
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
