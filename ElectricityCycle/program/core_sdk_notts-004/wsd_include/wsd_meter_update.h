//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з��� 
//������	
//��������	
//����		METER_UPDATEͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __METER_UPDATE_H
#define __METER_UPDATE_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eIDLE = 0,
	eWaitResult,
	eSuc,
}eOpenFactoryStep;
typedef struct
{
	DWORD dwFileSize; //�����ļ���С��Byte��
	WORD wAllFrameNo; //�����ļ��ָ�����֡��  
	WORD wCrc16;	  //���������ļ���CRC16У����
	DWORD dwIapFlag;  //������־λ  DOWNLOAD_METERFILE ��Ч��0x5AA55AA5 ������ⵥԪ��Ч��������Ч
	DWORD dwCrc32;	  //�ṹ��У��
} T_Iap_645;

#pragma pack(1)
typedef struct TIap_t
{
	BYTE bDestFile[10];
	BYTE bSourceFile[10];
	DWORD dwFileSize;		// �����ļ���С��Byte��
	BYTE bFileAttr;			// �ļ�����
	BYTE bFileVer[10];		// �ļ��汾
	BYTE bFileType;			// �ļ����
	WORD wFrameSize;		// ������С��Ĭ��200�ֽ�
	WORD wAllFrameNo;		// �����ļ��ָ�����֡��
	WORD wFrameOfOneSector; // һ���������Դ�ŵ�����֡��
	WORD LastFrameSize;		//����0��ʾ���һ֡������֡�������ʾ���һ֡��С
	
	WORD wCrc16;			// ���������ļ���CRC16У����
	WORD wAllUpdate;		// 1:ȫ������  0����������
	DWORD dwIapFlag;		// ������־λ  DOWNLOAD_METERFILE ��Ч��������Ч
	//ģ��������Ϣ
	BYTE CheckResultCnt;
	BYTE AlreadySendExeFrame;//ֻ��һ��ִ��֡
	BYTE IsModuleUpgrade; 	//���ɱ�ʶģ��������ͨ��698����
	DWORD dwCrc32;			// �ṹ��У��
} T_Iap_698;
#pragma pack()

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

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern T_Iap_645 tIap_645;
extern T_Iap_698 tIap_698;
extern BYTE MeterUpgradeTimeUpFlag;
extern char upgrade_file_path_645[FILENAME_LEM];
extern  char upgrade_file_path_698[FILENAME_LEM];
extern BYTE OpenFactoryStep;

//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������  : 645��ԼԤ����,ֻ�����ж�����
//
//����  :    TSerial *p[in]
//
//����ֵ:     BOOL ��TRUE-��Ҫ��ת�����߱������鴦��
//��ע����  :  ��
//-----------------------------------------------
BOOL Pre_Dlt645(TSerial *p);
void EnterUpgrade698(void);
void ExitUpgrade698(eUpgradeResult result);
BOOL GetUpGradeFileMessage(void);
unsigned short CalCRC16( unsigned char *puchMsg, unsigned short usDataLen );
void ComposeFileFrameLen(BYTE *pBuf, WORD *APDULen);
BOOL ProcResponse698(void);
BOOL ProcUpgradeResponse698(TSerial *p);
void SendOpenFactoryFrame();
#endif //#ifndef __METER_UPDATE_H
