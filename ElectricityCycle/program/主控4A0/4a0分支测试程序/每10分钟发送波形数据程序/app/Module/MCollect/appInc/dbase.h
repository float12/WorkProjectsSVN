///////////////////////////////////////////////////////////////
//	�� �� �� :dbase.h
//	�ļ����� :���ݿ���ض���
//	��    �� : jiangjy
//	����ʱ�� : 2015��9��11��
//	��Ŀ���� ��DF6203
//	����ϵͳ : 
//	��    ע :
//	ֱ������FLASH��д�ӿڣ���һ��2K�飬дһ��2K�飬����128K�飩115M
//	��ʷ��¼�� : 
///////////////////////////////////////////////////////////////
#ifndef __DBASE_H__
#define __DBASE_H__
#include "mp.h"
#include "sysdata.h"
//#include "dbFrz.h"
// #include "dbReal.h"
#include "mpflags.h"
//�����������ʶ����
// #define BUF_DFRZ			0x12345678
// #define BUF_MFRZ			0x23456789
// #define BUF_CURFRZ			0x34567890
// #define BUF_MPRUNINFO		0x67890000//���ֽڲ������
// #define BUF_NOVALID			0x5A5AA5A5

//#define  MAX_RATIO			4		
//�������ͱ�ʶ
// #define	FLAG_SYS			BIT0
// #define	FLAG_METER			BIT1
// #define	FLAG_MP_EX			BIT2
// #define	FLAG_FKSYS			BIT3
// #define	FLAG_TASK			BIT4
// #define FLAG_DATASET		BIT5
// #define FLAG_SYS_EX			BIT6
// #define FLAG_ESAM			BIT7

//698Э���������
// typedef struct _TBase698ParaCfg{ 
// 	BYTE base_para_buf[4096];
// 	BOOL saveflag;
// }TBase698ParaCfg;
 
typedef struct _TMP698ParaCfg{ 
	BYTE rtu_para_buf[4096];
	BYTE nowPageNum;
	BOOL saveflag;
}TMP698ParaCfg;
 
// typedef struct _TTask698ParaCfg{ 
// 	//.BYTE para_task_buf[((MAX_698_TASK_NUM*sizeof(TOad60130200)+2047)/2048)*2048];
// 	BYTE para_task_buf[((MAX_698_TASK_NUM*(27+MAX_TASK_TIMEREG_NUM*4)+2047)/2048)*2048];
// 	BOOL saveflag;
// }TTask698ParaCfg;

// typedef struct _TScheme698ParaCfg{ 
// 	BYTE para_scheme_buf[2048];
// 	BYTE nowPageNum;
// 	BOOL saveflag;
// }TScheme698ParaCfg;
 
// typedef struct _TCtrl698ParaCfg{
// 	BYTE para_ctrl_buf[2048];
// 	BYTE nowPageNum;
// 	BOOL saveflag;
// }TCtrl698ParaCfg;

// typedef struct _TFrz698ParaCfg{
// 	BYTE para_frz_buf[2048];
// 	BYTE nowPageNum;
// 	BOOL saveflag;
// }TFrz698ParaCfg;

typedef struct _TSysParaCfg{
	// TBase698ParaCfg  base_para_cfg;//ϵͳ���ò����ڴ�(���ñ��棬��������) 
	TMP698ParaCfg    mp_para_cfg;  //�����㵵������2K���� 
	// TTask698ParaCfg  task_para_cfg;//�ն�4K��������ڴ�(���ñ��棬��������)  
	// TScheme698ParaCfg  sheme_para_cfg; //����2Kƽʱ�����ϱ�ռ�� ������ٿռ� 
	// TCtrl698ParaCfg  ctrl_para_cfg;//���Ʋ�������
	// TFrz698ParaCfg	frz_para_cfg;	//�����������
}TSysParaCfg;

extern TSysParaCfg *gpSysPara;

//�������(0~1999),�ն˱����ţ��ܼ����
// #define TERMINAL			2000
// #define GRPNO(x)			(WORD)(2000+x)		//�ܼ����
// #define MP2GrpNo(x)			(x-2000)
// //698ר��
// #define PULSENO(x)			(WORD)(2100+x)		//����������
// #define MP2PulseNo(x)		(x-2100)

// typedef struct _TSaveBuf698{
// 	BYTE DataBuf[SIZE_PAGE];	//���ݻ�����
// 	BYTE byFrzType;				//����ʱ��
// }TSaveBuf698;

//���ڴ��б�����ʵʱ����
// typedef struct _TData_6203{
	// DWORD	dwImagicB;			//���ڼ��NVRAM�Ƿ����仯�ı�ʶͷ

	// BYTE	byACDCFlag;			//��ֱ�������ʶ
	// DWORD	dwLastStopTime;		//���һ��ͣ��ʱ��(�����2000�������)

	// DWORD HourFrzFlag[MAX_DWNPORT_NUM];	//Сʱ���������ǩ(�̶�Ϊ24Сʱ)
	// BYTE DFrzFlag[MAX_DWNPORT_NUM][(DAYS_DFRZ_SAVE+7)/8];
	// BYTE DAutoFrzFlag[(MAX_MP_NUM+7)/8]; //�Զ������ն����ʶ	
//698��չ

	// DWORD OldTaskTime[MAX_DWNPORT_NUM][MAX_698_TASK_NUM];
	// BYTE RealBuf[SIZE_PAGE];	//ʵʱ���ݻ�����
	// WORD wRealWptr;				//��ǰָ��
	// WORD real_page_offset;		//ʵʱ����ҳƫ��(BLOCK_OF_REAL/2048)
	//BYTE CurvBuf698[SIZE_PAGE];	//���ݻ�����
	// BYTE UpType[3];				//ͨ���ϱ�����
	// BYTE safmodeInit[(MAX_MP_NUM+7)/8]; //��ȫģʽ��ʼ����ʶ
	// BYTE safmodeValid[(MAX_MP_NUM+7)/8]; //��ȫģʽ���ñ�ʶ
	// TSaveBuf698 PortBuf[MAX_DWNPORT_NUM];

	// TSaveBuf698 SysBuf;
	// DWORD dwSTime;				//��ʼʱ��ƫ��
	// DWORD dwDFrzTime;			//����ʱ��
	// DWORD dwMFrzTime;			//����ʱ��
    // DWORD	dwLastRunTime;
	// DWORD	dwLastDSTM;			//����ʱ��
	// DWORD	dwLastMSTM;			//����ʱ��
	// DWORD	TaskCheckTime[MAX_DWNPORT_NUM];	
	// DWORD	UpTaskCheckTime[3];	
	// DWORD   dwLastCurvTime[MAX_MP_NUM];	//��1�β����㳭�����ߵĴ洢ʱ�꣬���ڷ���10��ȡ
	// DWORD	dwImagicE;			//���ڼ��NVRAM�Ƿ����仯�ı�ʶβ 
// }TData_6203;

// TData_6203 *pGetDF6203(void);

// void InitDase(void);
// BOOL MPTime2Time(TMPTime *pMPTime,TTime *pTime);
// DWORD GetLastRunMins(void);
// void DB_SaveData(DWORD dwMins);
// void SwitchToNewFrzTime(BYTE byType,DWORD dwTime);
// void MPRxDeal(TPort *pPort,WORD wMPNo);
#endif