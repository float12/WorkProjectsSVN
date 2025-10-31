#ifndef _CGY_H_
#define _CGY_H_
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
  #include "__def.h"

#include "paradef.h"
#include "usrtime.h"
#include "port.h"

#define TIME_ONDRIVER	20		//����ɨ����
#define TR_GY			0		//�Ĵ�����
//��Լ���շ�����
#define FM_SHIELD		0xFFFF0000//������������
#define FM_OK			0x00010000//��⵽һ��������֡
#define FM_ERR			0x00020000//��⵽һ��У������֡
#define FM_LESS			0x00030000//��⵽һ����������֡����δ���룩

#define MGYM_MASTER		1		//�ʴ�ʽ(����Լ)
#define MGYM_SLAVE		2		//����ʽ(��վͨ�Ź�Լ)
#define MGYM_SYSMASTER	3		//����ʽ(��վͨ�Ź�Լ)

//Э��������Ϣ
// typedef struct _TGyCfg{
// 	WORD  wTxdIdleTime;			//���Ϳ���ʱ��
// 	WORD  wCommIdleTime;		//�ַ��������
// 	DWORD  dwRxDelayMax;		//���յ���һ���ַ�����ȴ�ʱ��
// 	WORD  wGyMode;				//��Լģʽ
// 	WORD  wAPPSize;				//APP�ռ��С(����Լʱ�����ж������APP�ռ��С����)	
// }TGyCfg;

// typedef struct _TGy{
// 	BYTE	byNo;				//��ǰ�����˿����
// 	DWORD	dwPortFlag;			//�˿���Ч��ʶ
// 	HPARA	hAppEx;				//����APP����
// 	TTime	Time;				
// 	DWORD	dwOldSearchTime;	//�ϴ��ѱ�ʱ��
// 	DWORD	dwOldCurvTime;
// 	DWORD	dwCount;
// 	DWORD	dwOldEventCheck;	//�¼����
// }TGy;

//���ʵʱ����
typedef struct _TMeter_Real{
	DWORD DD[57];				//0~39 ���С����С����ޡ����ޡ�һ~�����ޡ�40~43 A���������ޡ�44~47 B���������ޡ�48~51 C���������ޡ�52~56����й� 
	BYTE  DDFlag[8];	
	long  YC[29];				//0~2A~C��ѹ��3~5A~C������6~9�ܼ�A~C�С�10~13�޹��ʡ�14~17�ܼ�A~C�������ء�18���������19~21A~C��ѹ��λ�ǡ�22~24A~C������λ�ǡ�25~28���ڹ���
	BYTE  YCFlag[4];	
	TXL2  XL[20];				//0~19 ���С����С����ޡ���������
	BYTE  XLFlag[3];	
	DWORD XL1[2];				//��ǰ�й����� ��ǰ�޹����������з��ţ�
	BYTE  XLFlag1;
	DWORD Other[2];				//0~���ʱ��,����ʱ��
	BYTE  OtherFlag[1];	
	
	BYTE  XLTmpFlag[3];			//������ʱ��Ч��ʶ��������������������Ч
	DWORD PayInfo[4];			//Ԥ������Ϣ/*PayInfo[3]*/ ����͸֧���
	BYTE  byMetStus[16];
	BYTE  PayInfoFlag;			//Ԥ���ѱ�ʶ	
}TMeter_Real;

//�����������(���ܳ���128�ֽ�)
typedef struct _TMCurv{
	DWORD DD[8];				//���С����С����ޡ����ޡ�һ~������
	BYTE  DDFlag;
	long  YC[18];				//0~2A~C��ѹ��3~5A~C������6~9�ܼ�A~C�С�10~13�޹��ʡ�14~17�ܼ�A~C��������
	BYTE  YCFlag[3];
	DWORD XL[2];                //�й����� �޹�����   //..���ȸı䣿��������������
	BYTE  XLFlag;
}TMCurv;

typedef union _UMeterData{
	TMeter_Real	Real;	
	BYTE Buf[2048];				//��������������
}UMeterData;

#define REQ_REAL		0		//ʵʱ
#define REQ_CURV		1		//����
#define REQ_DFRZ		2		//�ն���
#define REQ_MFRZ		3		//�¶���
#define REQ_CURVEX		4		//���߲���
#define REQ_SETTLEDAY   5       //�����ն���
#define MAX_REQ_TYPE	6		

#define REQ_EVENT_0		7	    //���ڲɼ�

//�����ǲ���Ҫ�ɼ���ֻ��Ҫ���ݱ�ʶ����������
#define REQ_AUTODFRZ	16		//�Զ������ն���(��ʵʱ���ݲɼ���������ר��ѭ������)
#define REQ_ENMCJ		17		//���¶�����������ɼ���ʶ
#define REQ_DFRZEX		18		//�ն����ն�
#define REQ_MFRZEX		19		//�¶����ն�
#define REQ_DFRZDX		20		//�ն������

typedef struct _TGyDbase{
	TTime	Time;
	UMeterData	Data;	
	WORD  data_offset;			//����������ƫ��
	WORD  data_offsetEx[3];		//����������ƫ�������ٻ���	
	// BYTE  byCurvInterval;		//���ߵĲ���
	// BYTE  byCurvPointNum;		//���ߵ���
	TTime DataTime;				//������ʼʱ��
	// BYTE  byOffset;	
	// DWORD  dwFreshBufFlag;		//BIT0-���ʱ仯��BIT1-���ʱ����ģ�BIT2-�����ձ����
								//BIT3-���ܱ����峣������,BIT4-���������ʸ���,BIT5-�����������
								//BIT8-�ų����ŷ����ı�,BIT9-����ǣ�BIT10-����Ŧ��,BIT11-���ܱ�״̬�ֱ�λ
								//BIT12-��̴������ģ�BIT13-���������������,BIT14-�����������,BIT15-ʧѹ�������� BIT16-Уʱʱ����
								//BIT31-�״�ˢ����Ҫ����	
	TMeterRealCI RealCI;
	TCVXL CVXl;
}TGyDbase;

// void SethGy(HPARA hGy);
// HPARA GethGy(void);
HPARA GethPort(void);
BYTE GetPortNo(void);
HPARA GethDB(void);
// HPARA GethGyApp(void);
TTx* GetTx(void);
TRx *GetRx(void);
HPARA GetRxFm(void);
// BOOL SwitchToAddr(HPARA hPort,BYTE *Addr,BYTE byAdrLen);
// void OnDriver(TPort *pPort);
// void TxDirect(TPort *pPort);
// void PortTxClear(TPort *pPort);
// void PortRxDirect(TPort *pPort);
// BOOL GetGyInfo(WORD wDrvID,TGyInfo *pGyInfo);
//WORD GetMGyAppSize(void);
BYTE get_698_task_no(void);
void SetRunPort(BYTE No);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif



