//----------------------------------------------------------------------
// ��Ȩ���� (C) 2003-20XX ?????????????????????????????��???
// ��������
// ����˵��
// ����ֵ˵��
// �޸ļ�¼
//----------------------------------------------------------------------
#ifndef __UART_H
#define __UART_H

//-----------------------------------------------
//				���峣��
//-----------------------------------------------
#define MAX_COM_CHANNEL_NUM					2										//��·���� �����õ�����	
#define SINGLE_LOOP_ITEM 					27 										// һ�������ڵ�����֡��Ŀ
// �������ݵ���س���		
#define WAVE_UART_QUEUE_SPACE				50
#define WAVE_CYCLE_CNT 						10										// һ�����������ڵ�����֡��Ŀ
#define WAVE_DATA_FRAME_SIZE 				815										// һ������֡�Ĵ�С
#define SEND_WAVE_DATA_LEN 					(WAVE_DATA_FRAME_SIZE*2)
// ʵʱ���ݵ���س�����Ŀǰ��ʽд����δ���ɸ��������޸�
#define MAX_PHASE_NUM 						3										// �����λ��3��
#define DATA_ITEM_NUM_PER_PHASE 			26										// ÿ����λ����������
#define INSTANT_DATA_LEN_PER_PHASE 			(DATA_ITEM_NUM_PER_PHASE * 4)			// һ����λ��ʵʱ���ݴ�С��ÿ��������4���ֽ�
#define MAX_DATA_LEN_PER_FRAME 				194										// 645���ֽ�һ֡�������Ч���ݴ�С
#define INSTANT_DATA_698_FRAME_LEN 			432										// ���������ϴ�698֡�Ĵ�С
//-----------------------------------------------
//				����ṹ�壬ö������
//-----------------------------------------------
typedef struct
{
	BYTE buf[SEND_WAVE_DATA_LEN];
	BYTE type;	  // 0:����֡��1�������ϴ�֡
	BYTE LoopNum; // ����ϵ�������ĸ���·
} WaveDatatoTcp; // �������ݻ������ϴ�����



typedef enum
{
	eUpgradeExeSuc = 0,			 // ִ�������ɹ�
	eUpgrdeFrameTimeoutErr,		 // ���ճ�ʱ
	eUpgradeCheckResultErr,		 // У��������
	eUpgradeVeriErr,			 // ��֤����
	eUpgradeDARErr,				 // DAR����
	eUpgradeWholeProTimeoutErr,	 // ȫ���̳�ʱ
	eFtpLoginSuccess,			 // ��¼ftp�ɹ�
	eFtpDownLoadSuccess,		 // �����ļ��ɹ�
	eFtpModuleUpgradeFail,		 // ģ������ʧ��
	eFtpModuleUpgradeVerifyFail, // ģ��������֤ʧ��
	eFtpFailExit,				 // ģ���쳣�˳�ftp����
} eUpgradeResult;

//-----------------------------------------------
//				����ȫ�ֱ���
//-----------------------------------------------
extern int UART_HD_LTO;
extern int UART_HD_RECVWAVE;
extern int UART_HD_BASEMETER;
extern int IntervalTime; // �������ʱ��
extern TRealTimer tTimer;
extern BYTE PublishInstantData[MAX_PHASE_NUM * INSTANT_DATA_LEN_PER_PHASE + 4]; // 4���ֽ�
extern TSerial Serial[MAX_COM_CHANNEL_NUM];
//-----------------------------------------------
// 				���庯��
//-----------------------------------------------
void Uart_Task(void *parameter);
void InitPoint(TSerial *p);
BYTE api_CalRXD_CheckSum(WORD ProtocolType, TSerial *p);
void api_DoReceMoveData(TSerial *p, BYTE ProtocolType);
#endif // #ifndef __UART_H
