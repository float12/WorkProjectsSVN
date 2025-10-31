//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	������
//��������	2018.8.13
//����		����ģ���ڲ�ͷ�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#ifndef __FREEZE_H
#define __FREEZE_H
//--------------------------------------

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define CURR_PLAN(OAD)						(0x07&(OAD>>21))	//ȡOAD�еķ�����

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TFreezeInfoTab_t
{
	WORD		OI;
	eFreezeType	FreezeIndex;
	BYTE		MaxAttNum;
	DWORD		dwDataLen;
	DWORD		wEeDepth;
}TFreezeInfoTab; 


typedef struct TFreezeAddrLen_t
{
	WORD	wAllInfoAddr;
	WORD	wDataInfoEeAddr;
	DWORD	dwAttOadEeAddr;
	DWORD	dwAttCycleDepthEeAddr;
	DWORD	dwDataAddr; //�����Ӷ����⣬�����̶�ʹ��0 ���ݴ洢��ַ�������洢�ռ�
}TFreezeAddrLen;
typedef struct TFreezeData_t
{
	TDLT698RecordPara 		*pDLT698RecordPara;
	TFreezeAttOad			*pAttOad;
	TFreezeAttCycleDepth	*pAttCycleDepth;
	TFreezeDataInfo			*pDataInfo;
	TFreezeAddrLen			*pFreezeAddrLen;
	TFreezeAllInfoRom		*pAllInfo;
	TFreezeMinInfo			*pMinInfo;
	BYTE					*pBuf;
	DWORD					*pTime;
	BYTE					MinPlanIndex;     //Ҫ�����Ӷ��᷽��
	BYTE					Tag;
	BYTE					FreezeIndex;
	WORD					Len;
	DWORD					SearchTime;         // [out] �ҵ���ʱ��
	DWORD					RecordNo;           // [out] �ҵ������
}TFreezeData; 

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------

BOOL GetFreezeAddrInfo( BYTE inFreezeIndex, TFreezeAddrLen *pFreezeAddrLen );

WORD ReadFreezeByRecordNo( TFreezeData	*pData );


#endif//��Ӧ�ļ���ǰ���#define __FREEZE_H
