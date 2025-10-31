//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з��� 
//������	
//��������	
//����		UARTͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __UART_H
#define __UART_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define SINGLE_LOOP_ITEM	23			//һ��·�ܹ�����˲ʱ��������ĸ���
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eUpgradeExeSuc = 0,			//ִ�������ɹ�
	eUpgrdeFrameTimeoutErr,		//����֡��ʱ
	eUpgradeCheckResultErr,		//���������
	eUpgradeVeriErr,			//У�����
	eUpgradeDARErr,				//DAR��������
	eUpgradeWholeProTimeoutErr, //ȫ���̳�ʱ����
	eFtpLoginSuccess,			//��¼ftp�ɹ�
	eFtpDownLoadSuccess,		//�����ļ��ɹ�
	eFtpModuleUpgradeFail,		//ģ������ʧ��
	eFtpModuleUpgradeVerifyFail,//ģ������У��ʧ��
	eFtpFailExit,				//ģ���쳣�˳�ftp����
	e645FrameNormal,			//645��֡��������
} eUpgradeResult;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern int UART_HD;
extern int IntervalTime;			//�ɼ����ʱ��
extern TRealTimer tTimer;
extern DWORD MeterBaud;
//-----------------------------------------------
// 				��������
//-----------------------------------------------
void  Uart_Task(void *parameter);
void InitPoint(TSerial * p);
BYTE api_CalRXD_CheckSum(WORD ProtocolType, TSerial * p);
void  api_DoReceMoveData( TSerial * p,BYTE ProtocolType);
#endif //#ifndef __UART_H
