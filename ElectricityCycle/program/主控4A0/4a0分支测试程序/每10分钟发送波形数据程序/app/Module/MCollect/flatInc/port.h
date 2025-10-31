#ifndef __PORT_H__
#define __PORT_H__
#ifdef __cplusplus
 #if __cplusplus
  extern "C" {
 #endif
#endif
#include "flags.h"
#include "usrtime.h"

#define MPT_SERIAL		1		//����
#define MPT_ETHERNET	2		//��̫��
#define MPT_376_2		3		//376.2(�ز�/����)
#define MPT_376_3		4		//376.3(GPRS)
	  
/*�ײ������Ĺ���״̬*/
#define MPS_IDLE		0		//���ڿ���״̬
#define MPS_RESET		1		//���ڸ�λ��ʱ״̬
#define MPS_RTS_ON		2		/*���ڿ�RTS��ʱ״̬*/ 
#define MPS_TX			3		/*���ڷ���״̬*/
#define MPS_RTS_OFF		4		/*���ڹ�RTS��ʱ״̬*/

#define	MODULE_CHECK	0		//ģ����
#define	MODULE_INIT		1		//ģ���ʼ��
#define	MODULE_READY	2		//ģ�����
#define	MODULE_LINKING	3		//ģ������
#define	MODULE_ONLINE	4		//ģ������
#define	MODULE_ONOFF	5		//ģ�鿪�ػ�
#define	MODULE_WAITING  6		//ģ��ȴ� ������ģʽ����ͣ

//GPRSģ���ź�ǿ���ٽ�ֵ����
#define MIN_SINGNAL		5
#define MAX_SINGNAL     38
	  
//��ӦATģ�������
#define MT_GPRS			1		//GPRS
#define MT_CDMA			2		//CDMA
#define MT_PSTN			3		//PSTN
#define MT_LTE			4		//4G
	  
	  
//������������
// typedef struct _TDrvCfg{		
// 	FB_H   ChipOpen;			//��ʼ������
// 	BYTE   byNo;				//Uart��USart�����ڶ�Ӧ��ţ�����ն�������2��UART ,����ŷֱ�Ϊ0,1�;���UartX�޹�
// 	char   szDevName[30];		//�����豸��	
// }TDrvCfg;


//�����������
typedef struct _TSerCfg{
	DWORD dwBaud;//������
	BYTE  byFlags;//ֹͣλ��У��λ����Ϣ
		//7,6: 00��1λֹͣλ 01��2λֹͣλ 10��1.5λֹͣλ
		//5,4:  00:��Ч��  10:żУ�� 01����У��  
		//1,0: RTS����ģʽ
	BYTE  byCharSize;//�ַ�λ��
	WORD wRTSOnDelay;
	WORD wRTSOffDelay;
}TSerCfg;

// typedef struct _TGyInfo{
// 	FB_HH	fGetCfg;			//��ù�Լ������Ϣ
// 	FB_H	fInit;				//��ʼ����Լ����ר�ÿռ�
// 	FB_H	fTxMonitor;			//���ʹ���
// 	FD_HHD	fSearchOneFrame;
// 	FV_H	fRxMonitor;			//���մ���	
// }TGyInfo;

//������
typedef struct _TTx_Rx{
	WORD wSize;
	WORD wOldPtr;
	WORD wRptr;
	WORD wWptr;
	BYTE  *pBuf;
	DWORD dwBusy;
}TTx,TRx;

typedef union _UPortCfg{
   TSerCfg   SerCfg;
}UPortCfg; /*�ö˿ڵ����ò���*/

//·��ģ��оƬ����
#define MCHIP_NULL		0		//δ��⵽
#define MCHIP_ES		1		//����оƬ
#define MCHIP_TC		2		//����оƬ
#define MCHIP_XC		3		//����оƬ
#define MCHIP_RC		4		//��˹��оƬ
#define MCHIP_MI		5		//����΢оƬ
#define MCHIP_SS		6		//������оƬ
#define MCHIP_LM        7       //����΢оƬ
#define MCHIP_RL        8       //�������
#define MCHIP_FC        9       //��Ѷ������
#define MCHIP_SR        10      //ɣ������
#define MCHIP_HR        11      //�������
#define MCHIP_MX        12      //��ϣ����
#define MCHIP_SY        13      //��������
#define MCHIP_ZC        14      //�������
#define MCHIP_UNKNOWN	0xFFFF	//δ֪оƬ


// typedef struct _TPortCjInfo{
// 	WORD	wMetNum;			//�˿��µ�����
// 	BYTE	byWorkStatus;		//��ǰ����״̬
// 	WORD	wMetNum_OK;			//�ɼ��ɹ��������
// 	WORD	wVipMetNum_OK;		//�ص��ɼ��ɹ�����
// 	TTime	STime;				//��ʼ����ʱ��
// 	TTime	ETime;				//��������ʱ��
// }TPortCjInfo;

#define	MAX_SGY_NUM		3

typedef struct _TCj698Ctrl{
	BYTE    byTxRowNum;
	BYTE	CSDRptr;			//698��CSD��дָ�룬�����ж��Ƿ����
	BYTE	CSDWptr;
	// WORD	wRxDataNum;			//����Ӧ�����ݸ���
	// WORD	wOKDataNum;			//�����������ݸ���
	WORD    wTransRptr;         //͸�������ı��Ķ�ָ�룬�����ж��Ƿ����
	BYTE    byTransNo;           //͸��������
	// DWORD   dwTransInfo;        //͸��������Ϣ���������ݼ�������2�ֽ� + ֡���2�ֽڣ�
	// DWORD	Offset_Frz;	
	// DWORD	Offset_Frz_Bak;	
	// DWORD	dwFrzTimeMark;		//����ʱ��	�����жϵ��ʱ��
}TCj698Ctrl;

typedef struct _TCurvCjCfg{
	BYTE	byInit;				//0x5A�����Ѿ���ʼ��
	WORD	wNum;
	WORD	wInterval;
	DWORD	dwStart;
	BYTE    byCJInc;			//��698��Լ�Ĳɼ����Ӳ���
	BYTE	byCJStep;
	WORD	wNo;				//��ǰ����		
}TCurvCjCfg;

typedef struct _TReCjCfg{
	BYTE	byInit;				//0x5A�����Ѿ���ʼ��
	WORD	wNum;
	WORD	wInterval;
	DWORD	dwStart;
	BYTE    byReCJInc;			//��698��Լ�Ĳɼ����Ӳ���
	WORD	wReCJStep;
	WORD	wNo;				//��ǰ����		
}TReCjCfg;

typedef struct _TGyRunInfo{
	WORD	wMPSeqNo;			//�ɼ����(�л�ʱ��ֵ)
	TCurvCjCfg CurvCjCfg;
	TReCjCfg ReBuCJCfg;
	// DWORD	dwLastCjTime;		//���һ�βɼ�ʱ��
	// DWORD	dwCJSecs;
	BYTE	byCJStatus;			//�ɼ�״̬	
	BYTE	Addr[6];			//���ͨ�ŵ�ַ
	TSA		sa;
	BYTE	Flag[(MAX_FLAGNO+7)/8];
	BYTE	byTxIDFlag;			//��¼��һ��07���͵�msf
	BYTE	byTxCurvSource;		//��¼���͵��������ͣ�ʵʱת�򳭱��� 0x55-����
	// TGyInfo	GyInfo;				//��Լ���ýӿ�
	// WORD	wOldNo;
	BYTE	byReqType;			//��ǰ��������
	// BYTE	byTDRecSave;		//�Ƿ���Ҫ�ɼ�ͣ���¼�
	BYTE	byReqDataValid;		
	BYTE	byGyStep;			//��Լִ�в���
	TCj698Ctrl cj_698;			//698�ɼ�����
	WORD	wTxCount;
	WORD	wOldTxCount;
	WORD	wRxCount;		
	BYTE	byHisDataOffset;
	HPARA	hGyDB;				//��Լ���ݿ�
	// BYTE	byReqOK;
	DWORD	dwTaskTime;			//����ִ��ʱ��
	BYTE	*psch698;			//698�ķ���	
	// BYTE    by645ReportFlag;
	BYTE	*pApp;				//��Լ��Ӧ�ó���ռ�
}TGyRunInfo;

typedef struct _TPort{
	//1������������Ϣ
	DWORD	dwID;
	// BYTE	byPhyNo;
	// BYTE	byType;				//�˿�����(���硢����)
	// UPortCfg Cfg;
	// BYTE   RTSON;				//RTSON��״̬
	// BYTE   RTSOFF;				//RTSOFF��״̬
	//2��������Ϣ
	// WORD	wIdleMins;			//���з�����	
	BYTE	byOpen;				//�򿪱�ʶ
	BYTE	byMode;				//0 ���й�Լ�˿ڣ���0-��ƶ˿�
	// BYTE	RetryCnt;			//�ط�����
	BYTE	byStatus;			//�˿�״̬
	HPARA	hApp;
	DWORD	dwRunCount;			//���м���	
	// DWORD	dwCtrlTime;			
	// DWORD	dwFrameOkTime;		//���һ���յ�����֡ʱ��
	// BYTE	byGyNo;				//��ǰ�����Լ��(�ӹ�Լ)
	// BYTE	byMGyNo;			//��¼��Լ��(�ӹ�Լ)
	// BYTE	bySGyNum;			//�ӹ�Լ����
	// DWORD	aSGyApp[MAX_SGY_NUM];//�˿���֧�ֵĴӹ�Լ������
	// BYTE	aSGyNo[MAX_SGY_NUM];//�ӹ�Լ���б�	
	//Ӧ���շ�������������ģʽ�����жϼ���	
	WORD	wMetNum;			//�˿��µ�����
	// BYTE	byEnTran;			//�Ƿ�����͸��
	// DWORD	dwTranRunCount;		//͸������
	TTx		Tx;					
	TRx		Rx;					//����
	//�˿�������
	// INT		nfd;
	TGyRunInfo GyRunInfo;			//���ɼ���Ϣ
	// FB_HHDD	fAddrDaemon;		//��ַ���
}TPort;

//�������ýӿ�
#define IOCTRL_INVALID	0xFF
#define	IOCTRL_RESET	0		//��λ�˿�
#define IOCTRL_SETRATE	1		//����ͨ�Ų�����
#define IOCTRL_STARTTX	3		//�����ײ㷢��
#define IOCTRL_GETRATE	4		//��ýӿڵ�ǰ������Ϣ
#define IOCTRL_GETTXSTATUS 5	//��ýӿڵ�ǰ������Ϣ
#define IOCTRL_RTSON	6		//��RTS
#define IOCTRL_RTSOFF	7		//�ر�RTS
#define IOCTRL_CONNECT	8		//��������
#define IOCTRL_DISCONNECT	9	//�ر�����
#define IOCTRL_CLOSE	10		//�ر�ģ��
#define IOCTRL_STS_376	11		//376.3ͨ��״̬
#define IOCTRL_CALLCB	12		//��������
#define IOCTRL_SEARCH	13		//�����ѱ�
#define IOCTRL_STOPCB	14		//ֹͣ����
#define IOCTRL_BROADSETTIME	15	//�㲥Уʱ
#define IOCTRL_REQROUTER 16		//����·��
#define IOCTRL_RECOLLECT 17		//��������


TPort *pGetPort(BYTE byNo);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif
#endif
