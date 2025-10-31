#ifndef MGDW698_H__
#define MGDW698_H__
#if(MD_BASEUSER	== MD_GDW698)

typedef struct _TGetPara{
	BYTE *pTxBuff;
	WORD wBufLen;
	WORD wNum;
	BOOL bUseSafeMode;
	BYTE *Rand;	
	BYTE byType;	
}TGetPara;

typedef struct _TMGDW698 {		//����֡����	
	RSD		rsdd;  
	BYTE	byShemeNo;			//�������
	BYTE	byTxTaskType;		//������������
	BYTE	byReqNext;			//�Ƿ���Ҫ������һ֡
	WORD	wRxOKSeq;			//���һ�ν���Ok֡���
	DWORD	dwRandFreshTime;	//�����ˢ��ʱ��
	BYTE	Rand[30];			//���������+�����
//͸���������
	// WORD	wCommIdleTime_tran;	//�ַ������ 
	// DWORD	dwRxDelayMax_tran;	//��ȴ�ʱ��
	WORD	wAPDUSize;			//APDU����
//698ʵʱת����
	BYTE    byCJTypeFlag;       //��λ����ʵʱת����
	DWORD   dwLastTaskTime;
	BYTE    DataBuffer[512];
}TMGDW698;

BOOL GetReponseAPDUDatalen(BYTE *pAPDU,WORD wLen);
WORD _TxFm698Read(TSA *sa,TGetPara *pPara,BYTE *pBuf);
#endif
#endif