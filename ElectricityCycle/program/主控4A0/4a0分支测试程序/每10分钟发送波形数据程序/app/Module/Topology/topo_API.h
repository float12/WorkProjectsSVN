//----------------------------------------------------------------------
//Copyright (C) 2003-2024 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	�����
//��������	2024.7.1
//����		����ʶ�� apiͷ�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#if (SEL_TOPOLOGY == YES)
#include <stdint.h>

#ifndef _TOPO_API_H_
#define _TOPO_API_H_

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define TOPO_SIGNATURE_CODE_MAX_LEN 16		  //  ���������ź���Ϣ����������󳤶�
#define TOPO_EXT_INFO_LEN 12				  //  ���������ź���Ϣ����չ��Ϣ����
#define TOPO_CC_MAX_NUM_IN_IDENTI_RESULT 4	//  ʶ���������������ź���Ϣ��������
#define TOPO_IDENTI_RESULT_TIME_OUT (15 * 60) //  ʶ������ʱʱ��(s)
#define TOPO_SIGNAL_SEND_INTERVAL (3 * 60)	//  �����źŷ��ͼ��ʱ��(s)
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
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
	float fAmp_m;	 // 783Ƶ���ֵ
	float fAmp_p;	 // 883Ƶ���ֵ
	float fAmp_noise; //"����"
	sDF01 sTime;	  // ʱ��
} defTopoResult_TypeDef;
// �����ź���Ϣ
typedef struct
{
	WORD BitWideTime;								 // λ��ʱ�� ms
	WORD HighLevelPulseWidth;						 // �ߵ�ƽ����
	WORD LowLevelPulseWidth;						 // �͵�ƽ����
	BYTE SignatureCodeLen;							 // �����볤��
	BYTE SignatureCode[TOPO_SIGNATURE_CODE_MAX_LEN]; // ������
	DWORD ModulatFreq;								 // ����Ƶ�� 0.01Hz
	BYTE ExtInfoLen;								 // ��չ��Ϣ����
	BYTE ExtInfo[TOPO_EXT_INFO_LEN];				 // ��չ��Ϣ
	TRealTimer Time;								 // ��ʱ���͵�ʱ��
	DWORD CRC32;
} ModulatSignalInfo_t;
// ���������ź�ʶ����Ϣ
typedef struct
{
	TRealTimer IdentifTime;							 // ʶ��ʱ��
	BYTE SignalPhase;								 //�ź���λ
	float SignalStrength1;							 //�ź�ǿ��S1
	float SignalStrength2;							 //�ź�ǿ��S2
	float BackgroundNoise;							 //��������Sn
	BYTE SignatureCodeLen;							 // �����볤��
	BYTE SignatureCode[TOPO_SIGNATURE_CODE_MAX_LEN]; // ������
	BYTE ExtInfoLen;								 // ��չ��Ϣ����
	BYTE ExtInfo[TOPO_EXT_INFO_LEN];				 // ��չ��Ϣ
} TopoCharactCurIdentiInfo_t;
// ��������ʶ����
typedef struct
{
	BYTE Num; //���������ź�ʶ����Ϣ����
	TopoCharactCurIdentiInfo_t IdentiInfo[TOPO_CC_MAX_NUM_IN_IDENTI_RESULT];
} TopoIdentiResult_t;
#pragma pack()
//-----------------------------------------------
//				��������
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
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������:����ԭʼ��������
//
//����:��
//
//����ֵ:��
//
//��ע:
//-----------------------------------------------
void api_TopWaveHsdcBufTransfer(TWaveDataDeal *WaveDataDeal);

//-----------------------------------------------
//��������:��ʼ������ʶ����ز���
//
//����:��
//
//����ֵ:��
//
//��ע:
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
