//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "./GDW698/GDW698.h"
#ifndef __MCOLLECT_API_H
#define __MCOLLECT_API_H
//--------------------------------------

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define		MAX_SEND_DELAY			(150) //1.5��
#define		MAX_TX_COMMIDLETIME		(80)  //800����
#define		MAX_RX_COMMIDLETIME		(100) //1��
#define		ONE_SCHEME_FREEAE_NUM	1

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
#pragma pack(1)

typedef struct TClctTypeRom_t
{
	DWORD dwType; // �������ͣ����㶳�ᣬ�ն���
	DWORD CRC32;
} TClctTypeRom;

typedef struct TClctMeterMem_t
{
	TClctTypeRom ClctTypeRom;
} TClctMeterRom;

// typedef struct  {
// 	DWORD 	ArFastCatTime[BLOCK_OF_REAL/SIZE_PAGE/CATSIZE];
// 	DWORD 	CRC32;
// }TFastCatTime;
#pragma pack()

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern TFPara2* const	tfpara2bak;
//-----------------------------------------------
//				��������
//-----------------------------------------------
void api_MCollectTask(void);
BOOL IsJLing(BYTE Ch);
void JLWrite(BYTE destCh,BYTE *hBuf,DWORD dwLen,BYTE byGyType,BYTE sourceCh);
BOOL GyRxMonitor(BYTE Ch,BYTE GyType);
void JLWriteRx(BYTE Ch,BYTE *hBuf,DWORD dwLen);
BOOL IsUpCommuFrame(BYTE byGy, BYTE *pBuf);
BOOL IsCollectAssociateFrame(BYTE *pBuf);
void api_CommuTx(BYTE SerialNo);
void api_PowerDownSave(void);
void CollectorWriteTx(BYTE sourceCh,BYTE *hBuf,BYTE *pMeterAddr,DWORD dwLen);
void InitPara(BYTE byType);
void InitData(void);
BOOL api_analysisClearTextMAC(BYTE *pFrameBuf,BYTE *rand,TTwoByteUnion *frameLen);
#endif // __MCOLLECT_API_H
