//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	
//��������	
//����		
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __MCOLLECT_H
#define __MCOLLECT_H

#include "cgy.h"

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define MAXCOUNT_NOACK		            3	//�����Ӧ������������ж�ͨѶ���ϣ�
#define SECURED_TRANS_RN_VERIFY_MAC_EN  0   //������ȫ���䣨����+RN��MACУ��ʹ��

//�ɼ���ʶ
#define STS_CHK_CJ_FLAG		            0	//���ɼ���ʶ	
#define STS_SEL_MP			            1	//ѡ��ɼ�������
#define STS_SEL_CJ_TASK		            2	//ѡ��ɼ�����(698ר��)
#define STS_INIT_MP_CJ		            3	//��ʼ��������ɼ���Ϣ
#define STS_SET_MP_DATA_FALG            4	//���ò��������ݲɼ���ʶ
#define STS_DO_MP_CJ		            5	//���ݲɼ�������
#define STS_CHK_MP_CJ_FLAG	            6	//���ɼ��Ƿ����
#define STS_MP_CJ_OVER		            7	//����������ɼ�����

#define PROTOCOL_3762_MAX_LEN           255 //3762Э��Я�����ĵ���󳤶�

#define TRANS_BUF_LEN                   100	//����͸����󳤶�
#define PROXY_BUF_LEN                   PROTOCOL_3762_MAX_LEN	//�������������󳤶�


/**********************************************
*brief ���ݱ�ʶ
***********************************************/
#define    REQ_ENG698	  0x00000200 		//����й��ܵ���(698) 

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
enum 
{
    JLS_IDLE=0,
    JLS_TX,
    JLS_RX,
    JLS_COMPLETE
};

typedef enum
{
	ePROTOCOL_645_97 = 1,
	ePROTOCOL_645_07,
	ePROTOCOL_698
} ePROTOCOL_CLASS;

typedef struct t_SwitchIap
{
	WORD IapFlag;	 // ������־
	WORD IapStep;	 // ��������
	WORD IapFrameNo; // ֡���
	WORD RetryNo;	 // ���Դ���
} tSwIapTrans;

typedef struct _TMaster{
	BYTE	*pMPFlag;			//�����������Ч��ʶ��
	TSerCfg OldSerCfg;			//�ϴδ�������
	// BYTE	byCJInterval;		//�ɼ����	
	// TPortCjInfo	CjInfo;			//�˿ڲɼ���Ϣ
	// DWORD	dwOldCheckTime;		//���һ�μ��ʱ��
	// DWORD	dwCheckTime;		//���һ�μ��ʱ��
	// DWORD	dwOldReCjTime;		//�ϴ��������²ɼ�ʱ��
	
	//2.0���ݲɼ�
	TTime	Time;	
	WORD	wSetCJFlag;			//��cgy.h
	DWORD	dwLastCJMins;		//���һ�βɼ�ʱ��
	WORD	wEventCjFlag;		//1��2��3���¼��ɼ���ʶ
	BYTE	byCJing;			//��ǰ�ɼ�״̬	
	BYTE	byReqType;
	TTime	aTime_Data[MAX_REQ_TYPE];	//����ʱ��
	// TGyInfo	OldGyInfo;			//�ϴι�Լ��Ϣ

	
	WORD	wAppSize;
	//.BYTE	*pApp;				//APP˽�пռ�	
	BYTE	byTaskNo;			//��ǰ�ɼ��������(698��0~MAX_FKTASK_NUM)

}TMaster;


#pragma pack(1)

/**********************************************
*brief 
***********************************************/


/**********************************************
*brief DLT645
***********************************************/
typedef struct
{
	DWORD	dwFE;				//�����ַ�			
	BYTE	byS68;				//��ʼ68�ַ�
	BYTE	MAddr[6];			//��ַ
	BYTE    byE68;				//����68�ַ�
	BYTE    byCtrl;				//������
	BYTE	byDataLen;			//�����򳤶�
	DWORD	dwDI;				//��������
}T645FmTx07;

typedef struct
{
//	DWORD	dwFE;				//�����ַ�			
	BYTE	byS68;				//��ʼ68�ַ�
	BYTE	MAddr[6];			//��ַ
	BYTE    byE68;				//����68�ַ�
	BYTE    byCtrl;				//������
	BYTE	byDataLen;			//�����򳤶�
	WORD	wDI;				//��������
}T645FmTx97;

typedef struct
{
	BYTE	byS68;				//��ʼ68�ַ�
	BYTE	MAddr[6];			//��ַ
	BYTE    byE68;				//����68�ַ�
	BYTE    byCtrl;				//������
	BYTE	byDataLen;			//�����򳤶�
	DWORD   dwDI;
// 	BYTE	byDI0;				//��������
// 	BYTE	byDI1;				//��������
// 	BYTE	byDI2;				//��������
// 	BYTE	byDI3;				//��������
}T645FmRx;

typedef struct{
	BYTE	byS68;				//��ʼ68�ַ�
	BYTE	MAddr[6];			//��ַ
	BYTE    byE68;				//����68�ַ�
	BYTE    byCtrl;				//������
	BYTE	byDataLen;			//�����򳤶�
}T645FmRxEx;

/**********************************************
*brief DLT698
***********************************************/

#pragma pack()

//-----------------------------------------------
//	      �ṹ�嶨��
//-----------------------------------------------

//��������������Ϣ(���ز���/ά��485�յ������ݷ���ǰǰ���ڴ˴�)
typedef struct {
	BYTE    ModeSwitch;			//�ɼ���ģʽ
	BYTE	byValid;
	BYTE    bySourceNo;         //ԭ���ݶ˿�
	BYTE    byDestNo;           //�����˿�
	BYTE	byGyType;			//0-07��Լ��1-97��Լ
//	ComDCB  Dcb;                //�˿ڲ�������
	WORD    wTimeOut;           //��ʱʱ�䣨�룩
	WORD	wTxWptr;			//дָ��
	WORD    wRxWptr;       
	BYTE    TransTxBuf[TRANS_BUF_LEN];    //������
    BYTE    TransRxBuf[TRANS_BUF_LEN];
	BYTE    byDar;              //�����������
	DWORD   dwRxStartTime;
    //ΪӦ�Դ���͸��2ֻ��Ĳ��������ٻ�����
	BYTE    cacheTransTxBuf[PROXY_BUF_LEN];    //������
    BYTE    cacheTransRxBuf[PROXY_BUF_LEN];    
	WORD	wCacheTxWptr;			//дָ��
	WORD    wCacheRxWptr;  
	BYTE    byCacheDar;               
	//��Լ���
	BYTE    proxy_choice;       //����͸������
	BYTE    proxy_01_listNum;   //ProxyGetRequestList ����������
	BYTE    proxy_01_oadNum;    //ProxyGetRequestList OAD����
	DWORD   proxy_oads[5];      
	BYTE    proxy_listnum_para; //���̱���
	BYTE    Addr[6];
	BYTE    ClientAddr;
	BYTE    PIID;
	BYTE	SafeMode;
}TJLTxBuf;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern tSwIapTrans SwitchIapTrans;
extern TJLTxBuf JLTxBuf;

//-----------------------------------------------
// 				��������
//-----------------------------------------------
void JudgeBaudChange(BYTE SerialNo, DWORD Baud);
void Tx_Collect_Meter(BYTE SerialNo, ePROTOCOL_CLASS ePClass, BYTE *pAddr, DWORD dwID, BYTE byEnumBaud);
// BOOL SwitchToNextTask(TPort *pPort, DWORD dwCheckTime, BYTE *pbyTaskNo);
// BOOL _SwitchToNextMP(TPort *pPort,WORD *pwNo);
void  Min2Time(DWORD dwMins,TTime *pT);
//WORD fcs16(BYTE *cp, WORD len);
WORD MakeFrameHead(TSA *sa,BYTE *pBuf,BYTE byClientAddr);
WORD Add698FrameTail(BYTE byCtrl,WORD wFrameLen,BYTE *pBuf);
BYTE FindMeterMPNo(BYTE *pMeterAddr);
void JLTxBufInit(TJLTxBuf *pJL);
BOOL ProxyGetRequestList( BYTE Ch ,TJLTxBuf *pJL);
BOOL ProxyGetRequestRecord( BYTE Ch ,TJLTxBuf *pJL);
BOOL ProxyTransCommandRequest( BYTE Ch ,TJLTxBuf *pJL);
#endif//#ifndef TOPO_H
