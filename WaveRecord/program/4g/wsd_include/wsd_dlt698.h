//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з��� 
//������	
//��������	
//����		DLT698ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __DLT698_H
#define __DLT698_H
#include "wsd_uart.h"

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define	MAX_PRO_BUF						(512)	//֡���� ͨѶ����apdu�����ߴ�


//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

typedef enum
{
	edouble_long = 0,
	efloat32,
} eDataType698;
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern char MessageBuf[512];
extern BYTE InstantData698Frame[INSTANT_DATA_698_FRAME_LEN];
extern eDataType698 InstantDataFormatTable[DATA_ITEM_NUM_PER_PHASE];

//-----------------------------------------------
// 				��������
//-----------------------------------------------
void  Send_Heartbeatframe_698( tTranData *transdata,BYTE bLink );
void DoSearch_68_DLT698(TSerial *p);
WORD JudgeDlt698_45_HCS_FCS(BYTE Type, TSerial *p);
WORD DoReceProc_Dlt698_UART(BYTE ReceByte, TSerial *p);
WORD api_fcs16(BYTE *cp, WORD len);
void Compose_InstantData_698(void);
#endif//#if( READMETER_PROTOCOL ==  PROTOCOL_698)

