#ifndef _PARADEF_H_
#define _PARADEF_H_
#include "usrtime.h"
typedef struct _THisBuf{
	DWORD  dwType;			//��������(���������)
	BYTE Buf[2048];
}THisBuf;

#pragma pack(1)
//�������ݽṹ
typedef struct _TValTime{
	DWORD dwTime;
	DWORD dwVal;
}TValTime;

typedef struct _TMPTime{
	BYTE	Buf[6];				//������ʱ��
}TMPTime;

typedef struct _TXL2{
	DWORD dwVal;
	TMPTime Time;
}TXL2;

typedef struct _TMeterRealCI{
	DWORD dwValidFlag;
	DWORD DD[20];
}TMeterRealCI;

typedef struct _TCVXL{
	DWORD XL[2];
	BYTE  byValidFlag;
}TCVXL;

#pragma pack()

typedef struct _TReadDir{
	char *iszName;				//������ļ���
	void *ipvBuf;				//�����������
	DWORD idwBufLen;			//�������������С
	DWORD idwOffset;			//������ļ���ƫ����
	DWORD odwReadLen;			//����ʵ�ʶ��ĳ���
}TReadDir;

typedef struct{
	DWORD FileLen;				//�����ļ�����
	TTime Time;					//����  �ļ�ʱ��
}TFileInfo;

//���ļ��ṹ
typedef struct{
	char *iszName;				//������ļ���
	void *ipvBuf;				//�����������
	DWORD idwBufLen;			//�������������С
	DWORD idwOffset;			//������ļ�ƫ����
	DWORD odwReadLen;			//����ʵ�ʶ��ĳ���
}TFileRead;

///////////////////////////////////////////////////////////////
//	�� �� �� : FileRead
//	�������� : ���ļ�
//	������� : 
//	��    ע : 
//	�� �� ֵ : DWORD
//	����˵�� : TFileRead *pR
///////////////////////////////////////////////////////////////
DWORD FileRead(TFileRead *pR);

typedef struct {
	char *iszName; //������ļ���
	void *ipvBuf;  //�����������
	DWORD idwBufLen;//�������������С
	DWORD idwOffset;//������ļ�ƫ����
}TFileWrite;
///////////////////////////////////////////////////////////////
//	�� �� �� : FileWrite
//	�������� : д�ļ�
//	������� : 
//	��    ע : 
//	�� �� ֵ : DWORD
//	����˵�� : TFileWrite *pW
///////////////////////////////////////////////////////////////
DWORD FileWrite(TFileWrite *pW);
typedef struct {
	char *iszName; //������ļ���
	DWORD idwCleanLen;	//������������(128K���������)
	DWORD idwOffset;//������ļ�ƫ����(128K���������)
	BOOL bClearBak;	//���������	
}TFileClean;

// typedef struct{
// 	char *iszName; //������ļ���
// 	DWORD odwLen;  //��������
// }TGetFileLen;
// BOOL GetFileLen(TGetFileLen *pGFL);

void ResetBadBlockTab(void);
DWORD FileClean(TFileClean *pC);
// BOOL GetFileIdlePageAddr(char *szName,DWORD dwOffset,DWORD dwMaxLen,DWORD *pdwIdleOffset);
#endif
