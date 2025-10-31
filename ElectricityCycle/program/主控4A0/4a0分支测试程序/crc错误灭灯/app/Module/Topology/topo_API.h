//----------------------------------------------------------------------
//Copyright (C) 2003-2024 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	高朋辉
//创建日期	2024.7.1
//描述		拓扑识别 api头文件
//修改记录
//----------------------------------------------------------------------
#if (SEL_TOPOLOGY == YES)
#include <stdint.h>

#ifndef _TOPO_API_H_
#define _TOPO_API_H_

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define TOPO_SIGNATURE_CODE_MAX_LEN 16		  //  特征电流信号信息中特征码最大长度
#define TOPO_EXT_INFO_LEN 12				  //  特征电流信号信息中扩展信息长度
#define TOPO_CC_MAX_NUM_IN_IDENTI_RESULT 4	//  识别结果中特征电流信号信息的最大个数
#define TOPO_IDENTI_RESULT_TIME_OUT (15 * 60) //  识别结果超时时间(s)
#define TOPO_SIGNAL_SEND_INTERVAL (3 * 60)	//  拓扑信号发送间隔时间(s)
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)
typedef struct
{
	uint16_t RxCnt;
	uint16_t CycDot;
	uint8_t RxBuf[1154];
	float Wbuf[3][256];
	float Ubuf[3][256];
} sTopoWaveTypedef;

typedef struct
{
	uint8_t Second;
	uint8_t Minute;
	uint8_t Hour;
	uint8_t Day;
	uint8_t Month : 5;
	uint8_t Week : 3;
	uint8_t Year;
} sDF01;

typedef struct
{
	uint8_t topoInfo[4];
	float fAmp_m;	 // 783频点幅值
	float fAmp_p;	 // 883频点幅值
	float fAmp_noise; //"噪声"
	sDF01 sTime;	  // 时间
} defTopoResult_TypeDef;
// 调制信号信息
typedef struct
{
	WORD BitWideTime;								 // 位宽时间 ms
	WORD HighLevelPulseWidth;						 // 高电平脉宽
	WORD LowLevelPulseWidth;						 // 低电平脉宽
	BYTE SignatureCodeLen;							 // 特征码长度
	BYTE SignatureCode[TOPO_SIGNATURE_CODE_MAX_LEN]; // 特征码
	DWORD ModulatFreq;								 // 调制频率 0.01Hz
	BYTE ExtInfoLen;								 // 扩展信息长度
	BYTE ExtInfo[TOPO_EXT_INFO_LEN];				 // 扩展信息
	TRealTimer Time;								 // 定时发送的时间
	DWORD CRC32;
} ModulatSignalInfo_t;
// 特征电流信号识别信息
typedef struct
{
	TRealTimer IdentifTime;							 // 识别时间
	BYTE SignalPhase;								 //信号相位
	float SignalStrength1;							 //信号强度S1
	float SignalStrength2;							 //信号强度S2
	float BackgroundNoise;							 //背景噪声Sn
	BYTE SignatureCodeLen;							 // 特征码长度
	BYTE SignatureCode[TOPO_SIGNATURE_CODE_MAX_LEN]; // 特征码
	BYTE ExtInfoLen;								 // 扩展信息长度
	BYTE ExtInfo[TOPO_EXT_INFO_LEN];				 // 扩展信息
} TopoCharactCurIdentiInfo_t;
// 特征电流识别结果
typedef struct
{
	BYTE Num; //特征电流信号识别信息个数
	TopoCharactCurIdentiInfo_t IdentiInfo[TOPO_CC_MAX_NUM_IN_IDENTI_RESULT];
} TopoIdentiResult_t;
#pragma pack()
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern sTopoWaveTypedef sTopoWave;
extern TopoIdentiResult_t TopoIdentiResultRAM;
extern TopoCharactCurIdentiInfo_t TopoIdentiSignalData;
extern BOOL TopoEventFlag;
extern DWORD TopoIdentiCCTimeoutCnt;
extern const ModulatSignalInfo_t ModulatSignalInfoConst;
extern ModulatSignalInfo_t ModulatSignalInfoRAM;
extern const DWORD TopoEventTable[3];
extern const DWORD TopoEventSignalTable[3];
extern DWORD TopoSignalSendIntervalCnt;
extern BYTE RawDataHalfWaveCnt;
extern BYTE rawData[1152 * 2];
extern BYTE WrongNoToTFFlag;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能:翻译原始波形数据
//
//参数:无
//
//返回值:无
//
//备注:
//-----------------------------------------------
void api_TopWaveHsdcBufTransfer(TWaveDataDeal *WaveDataDeal);

//-----------------------------------------------
//函数功能:初始化拓扑识别相关参数
//
//参数:无
//
//返回值:无
//
//备注:
//-----------------------------------------------
void CheckTopoPara(void);
void TopoPowerDown(void);
void TopoPowerUp(void);
void api_TopoParaInit(void);
void api_TopoInitEventPara(void);
void TopoIdentiResultEventDataClear(void);
void TopoSecondTask(void);
void api_TopoSetPWMInfo(void);
BOOL api_TopoSignalSendStart(void);
BOOL api_GetTopuEventStatus(BYTE Phase);
WORD api_GetTopoIdentEvent698(TopoIdentiResult_t *InBuf, BYTE *OutBuf);
WORD api_GetTopoIdentResult698(BYTE ClassIndex, WORD OutBufLen, BYTE *OutBuf);
WORD api_GetTopoModulatSignalInfo698(BYTE ClassIndex, BYTE *OutBuf);
BYTE api_SetTopoPara698(BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData);
BYTE api_SetTopoModulatSignalInfo698(BYTE ClassIndex, BYTE *pData, BYTE *InfoLen);
WORD api_GetRequest645Topology(eSERIAL_TYPE PortType, BYTE *DI, WORD OutBufLen, BYTE *OutBuf);
#endif //#if( SEL_TOPOLOGY == YES )
#endif //#ifndef _TOPO_API_H_
