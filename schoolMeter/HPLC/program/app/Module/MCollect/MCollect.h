//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	
//��������	
//����		
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __MCOLLECT_H
#define __MCOLLECT_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define LLBYTE(w)		((BYTE)(w))  //ȡһ��DWORD������ֽ�
#define LHBYTE(w)		((BYTE)((WORD)(w) >> 8))//ȡһ��DWORD�Ĵε��ֽ�
#define HLBYTE(w)		((BYTE)((DWORD)(w) >> 16))//ȡһ��DWORD�Ĵθ��ֽ�
#define HHBYTE(w)		((BYTE)((DWORD)(w) >>24))//ȡһ��DWORD������ֽ�
#define LOBYTE(w)       ((BYTE)(w))
#define HIBYTE(w)       ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define HIWORD(l)		(WORD)((DWORD)(l) >> 16)//ȡһ��DWORD�ĸ�WORD

#define ADDR_BITS	0x0F		//��ַ����

/**********************************************
*brief ���ݱ�ʶ
***********************************************/
#define    REQ_UIK	   	  0x04001F00		//���õ�ѹ����ϵ��
#define    REQ_RES        0x04001F01        //��ȡ���ɱ�ʶ���
#define    REQ_TIME       0x04001F20        //����ʱ��
#define    REQ_ENG698	  0x00000200		//����й��ܵ���(698)
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct t_SwitchIap
{
	WORD IapFlag;	 // ������־
	WORD IapStep;	 // ��������
	WORD IapFrameNo; // ֡���
	WORD RetryNo;	 // ���Դ���
} tSwIapTrans;

//APDU����
typedef enum APDU_TYPE{
	ATE_NULL = 0,
		GET_Request=0x5,			//��ȡ����
		SET_Request,				//��������	
		ACTION_Request,				//��������
		REPORT_Response,			//�ϱ�Ӧ��
		PROXY_Request,				//��������
		Security_Request=0x10,		//��ȫ��֤����
		Security_Response=144,		//��ȫ��֤��Ӧ
} APDU_TYPE;

typedef struct  
{
	BYTE	byCtrl;				//������
	BYTE	byAPDU;				//APDU
	BYTE	byPIID;				//PIID
	BYTE	byCA;				//�ͻ�����ַ
	BYTE	byType;
	BYTE    bySecurity;         //�Ƿ�ȫ����
	BYTE	byRec;				//��¼��	
	DWORD	dwDI;				//���ݱ�ʶ
	BYTE	byNum;				//OAD����
	DWORD	dwOAD[15];			//OAD�б�
}TTransPara;

//��������������Ϣ(���ز���/ά��485�յ������ݷ���ǰǰ���ڴ˴�)
typedef struct {
	BYTE	byValid;
	BYTE    bySourceNo;         //ԭ���ݶ˿�
	BYTE    byPortFlag;         //�����˿�
	BYTE	byGyType;			//0-07��Լ��1-97��Լ
	WORD	wRptr;				//��ָ��
	WORD	wWptr;				//дָ��
	BYTE    byBaudSelect;       //������ѡ�����
	BYTE    TransBuf[256];      //������	
    TTransPara transPara;
}TJLTxBuf;

typedef struct _TGetPara{
	BYTE *pTxBuff;
	WORD wBufLen;
	WORD wNum;
	BOOL bUseSafeMode;
	BYTE Rand[16];	
	BYTE byType;	
}TGetPara;

typedef struct {
    
	WORD wFlagValid;
	WORD EventCount[8];//0����������1��������Ͷ�ˡ�2բλ�仯��3�쳣�澯��4ʧ���硢5�¶ȸ澯
}TSYSData;

typedef struct {
	
	DWORD dwSaveTime;		//����ʱ��	
	DWORD dwYCFlagValid;	//ң��������Ч��ʶ
	DWORD YC[7];			//0~2A~C��ѹ��3~5A~C������6�й�����
    DWORD dwCIFlagValid;    //����������Ч��ʶ
	DWORD CI[2];            //0���С�1����
	
	//Ӧ�Ե��Ժ̨���Ե��¼��������¼����������ֳ�
    DWORD TestFlagVaild;
	DWORD TestData[16];     //�ݴ���������(���16����)
	DWORD TestEngFlag;
	DWORD TestEngData[16];  //�ݴ���������(���16����)
	DWORD RR[16];           //ÿ�����ݵ�����ϵ��
	BYTE  RRCount;
}TMetData;

typedef struct {
	
	TMetData mpData[SEARCH_METER_MAX_NUM];
	DWORD JcTestFlagValid;
	DWORD JcTestData[16]; //�ݴ潻������
	DWORD JcTestFlagEngValid;
	DWORD JcTestEngData[16]; //�ݴ潻������
}TMPSData;

typedef struct _TMCJCtl{
	DWORD m_OldCjTime;  //�ϴβɼ�ʱ��
	BYTE  m_CjNo;       //��ǰ�ɼ�������
	BYTE  m_RetryNum[SEARCH_METER_MAX_NUM]; //�ط�����
	WORD  m_CjFlag[SEARCH_METER_MAX_NUM];     //�ɼ���ʶ
}TMCJCtl;

#pragma pack(1)

/**********************************************
*brief 
***********************************************/
typedef struct _TTime{
    WORD	wYear;	/*��*/
    BYTE    Mon;	/*��*/
    BYTE    Day;    /*��*/
    BYTE    Hour;   /*ʱ*/
    BYTE    Min;	/*��*/
    BYTE    Sec;	/*��*/
    WORD    wMSec;	/*����*/
}TTime;

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
typedef struct _TSA{
	union{
		BYTE	AF;
		struct{
			BYTE	len	 : 4;
			BYTE	log_addr : 2;
			BYTE	type : 2;
		};
	};
	BYTE addr[16];	// ��ַ��
}TSA;

typedef struct tag_DATETIME_S{
	WORD	year;		// ��
	BYTE	month;		// ��
	BYTE	day;		// ��
	BYTE	hour;		// ʱ
	BYTE	minute;		// ��
	BYTE	second;		// ��
}DATETIME_S;

//����֡β��֡����
typedef struct OBJ_EXCEPT_TAIL{
	BYTE  byStart;				//�����ַ�0X68
	WORD  wFmLen;				//֡��
	BYTE  byCtrl;				//������C
	BYTE  byAddrF;
	BYTE  _byAddrStart;
}OBJ_EXCEPT_TAIL;

typedef struct OBJ_TAIL{
	WORD wCheckCS;				//У���
	BYTE byEnd;					//������
}OBJ_TAIL;

typedef struct _TFrame698_FULL_BASE{
	OBJ_EXCEPT_TAIL *pFrame;
	BYTE *pDataAPDU;
	TSA  *pObjTsa;
	OBJ_TAIL *pTail;
}TFrame698_FULL_BASE;

//////////////////////////////////////////////////////////////////////////
typedef struct _TOAD_DI 
{
	DWORD	dwOAD;
	DWORD	dwDI;
}TOAD_DI;

typedef struct
{
	BYTE	byS68;				//��ʼ68�ַ�
	BYTE	Len[2];				//����
	BYTE    byCtrl;				//������
}T698Head;

typedef struct
{
	BYTE	byHCS;				//У��
	BYTE	byLCS;				//У��
}T698HCS;					//֡ͷУ��

typedef T698HCS T698FCS;		//��֡У��

//////////////////////////////////////////////////////////////////////////

typedef struct _T_DIINFO 
{
	BYTE	byFlag; //0����һ��ʶ�� 1���ɱ��ʶ���ֽ�ΪFF ���� ������жϣ�
	DWORD	dwDI;
}T_DIINFO;

#pragma pack()

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern tSwIapTrans SwitchIapTrans;
//-----------------------------------------------
// 				��������
//-----------------------------------------------
BOOL objSmockRead( TFrame698_FULL_BASE *pAfnBase, BYTE *Buf );
WORD gdw698buf_OAD(BYTE *pBuf,DWORD *pdword,TRANS_DIR td) ;
WORD gdw698buf_TSA(BYTE *pBuf,TSA *psa,TRANS_DIR td,BOOL bAddHead) ;
DWORD MDW(BYTE HH,BYTE HL,BYTE LH,BYTE LL);
WORD MW(BYTE Hi,BYTE Lo);
void GetSwIapData(void);
void api_TxSwitchIap(BYTE Type);
// WORD ReadEventCount(DWORD dwOAD);
#endif//#ifndef TOPO_H
