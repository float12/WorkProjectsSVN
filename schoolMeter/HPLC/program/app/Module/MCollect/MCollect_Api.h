//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼--20250415 lsc ɾ������Ҫ��������
//----------------------------------------------------------------------

#ifndef __MCOLLECT_API_H
#define __MCOLLECT_API_H
//--------------------------------------

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define  IDF_RES_NUM			10 		//��������
#define  IDF_LOOP_NUM			2 		//�ڼ���·
#define  MAX_SEND_DELAY   	 (150)    	//1.5��
#define  MAX_TX_COMMIDLETIME (80)		//800����
#define  MAX_RX_COMMIDLETIME (100)		//1��   

#define ARRAY_SZ(X) (sizeof((X)) / sizeof((X)[0]))
/**********************************************
*brief 485-2 �ɼ���ʶ
***********************************************/
#define		MSF_AP			0			//�����й����ܣ������ʣ�
#define     MSF_AR          1
#define     MSF_RP          2                   
#define     MSF_RR          3
#define     MSF_PA          4
/**********************************************
*brief ���� �ɼ���ʶ
***********************************************/
#define 	MSF_VOL_CUR_PARA	5			//��ѹ����ϵ��
#define 	MSF_IDENTIFY_PARA	6			//ʶ����
#define 	MSF_TIME_PARA		7			//ʱ��

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eVolTime = 0,  // �����ѹ��ʱ��
	eAjust,
	eInit
} eF460SpiCommCmd;
#pragma pack(1)
typedef struct TElectricityEnergy_t
{
	BYTE Time;		//ʱ��
	DWORD Energy;	//����
}TElectricityEnergy;

typedef struct TElectricityCommonData_t
{
	BYTE ADRR[7];									//�����ַ
	DWORD Power;									//����֮��
	TElectricityEnergy ElectricitySwtichEnergy;		//���ص���
	TElectricityEnergy ElectricityCombEnergy;		//��ϵ���
	BYTE MeterNum;									//�������
	BYTE MeterAdrr[8];							//�����ַ-֧��20̨��
}TElectricityExtData;
#pragma pack()


//��������
typedef struct TOverIPara_t
{
	BYTE 		Warn;		//�澯Ͷ��
	BYTE 		Out;		//����Ͷ��
	SDWORD 		Value;		//��ֵ
	DWORD 		Time;		//ʱ��
	DWORD 		CRC32;		//У��
} TOverIPara;

typedef struct TUnitSParaSafeRom_t
{
	TOverIPara 	OverIPara[4];		//����������ÿ���4�ֽ�CRC
	BYTE EquipmCtrlWord[8+4];		//8�ֽ����� + 4�ֽ�CRC
}TUnitSParaSafeRom;

#pragma pack(1)
typedef struct TElectIdentify_t
{
	BYTE  ucMessageID;	//��x�����
	BYTE  ucCode;		//��������
	BYTE  ucdevNum;		//�����豸��
	SWORD ucPower;		//��������
}TElectIdentify;
typedef struct
{
	unsigned short	ApplClassID;	//�豸���
	unsigned char 	ApplStatus;		//�豸״̬
	unsigned long 	ApplP;			//�豸�й�����
}T_ApplCurData;
typedef struct
{
	unsigned short	ApplClassID;	//�豸���
	unsigned long 	ApplFrzCylP;	//�����������й��õ���
	unsigned long 	ApplFrzCylAvgP;	//�����������豸����ƽ������
	unsigned char	StartTime;		//����ʱ��
	unsigned char	EndTime;		//ֹͣʱ��
	unsigned char	SECount;		//�ϲ���ͣ��
}T_ApplFrzData;

typedef struct AMessage
{
	TRealTimer ucTime;				//ʱ��
	BYTE  ucMessageID;				//�������
	T_ApplFrzData ucData[10];		//��������
}AMessage_T;
//���Բɼ������շ����ĵĴ���
typedef struct TCollectSendRec_t
{
	DWORD Sec_Send_Success;
	DWORD Sec_Rec_Success;
	DWORD Sec_Rec_Failure;
	DWORD Min_Send_Success;
	DWORD Min_Rec_Success;
	DWORD Min_Rec_Failure;
}TCollectSendRec;
#pragma pack()

typedef enum TRANS_DIR{ TD12, TD21 } TRANS_DIR; //ת������: TD12:1�β�->2�β�; TD21:2->1

typedef struct TTriggerViolationSign_t
{
	BYTE  ReasonFlag;	//����ԭ���־
	BYTE  DurationTime;	//��־��������ʱ��
}TTriggerViolationSign;
#if(SEL_F415 == YES)
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern BYTE Address_645[6];							//415�ش��ĵ�ַ��ͬ6025ͨ�ŵ�ַһ��
//TSendRecĿǰ������ֻ�Ǽ����·uart
extern TCollectSendRec TSendRec[2];
extern BYTE ControlResetStatus;						//�Ƿ�����415��λ��0��Ĭ������1��������415��λ

extern AMessage_T MessageData;						//415���ص�ʶ����--��ҪΪ�綯��ʶ����
//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ���˿��Ƿ��ڼ���״̬
//
//����: 	��
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL IsJLing(BYTE Ch, BYTE *pGyType,BYTE *pSourceCh);
//-----------------------------------------------
//��������: �������˿�д����
//
//����: 	��
//			
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void JLWrite(BYTE destCh,BYTE *hBuf,DWORD dwLen,BYTE byGyType,BYTE sourceCh);
//-----------------------------------------------
//��������: ��������ж˿��յ��ı��ģ��Ƿ���Ҫת������
//
//����: 	sourchCh-Դ�˿ڣ�pDestCh-Ŀ��˿�
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL IsNeedTransmit(BYTE sourceCh,BYTE byGy, BYTE *pBuf,BYTE *pDestCh);
BOOL IsGyTransDI(DWORD DI);
BOOL GyRxMonitor(BYTE Ch,BYTE GyType);
void api_PowerUpCollect(void);
void  api_ReadIdentResult(BYTE i, BYTE* Buf );
//--------------------------------------------------
//��������:  �ñ�־
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void SetCJFlag_m(BYTE Ch,BYTE Flag);
//--------------------------------------------------
//��������: 6025_to_415�ɼ�����״̬�Ķ�ȡ
//
//����:			0:�ɼ�״̬��ȡ
//				1:�ɼ�״̬����
//
//����ֵ:		BYTE
//
//��ע:415����ʱ���ɼ�����رգ�415ִ������ʱ���ɼ������
//			IsUpdateF415		�ɼ�����״̬
//				FALSE  			  1--����
//				TRUE  			  0--�ر�
//--------------------------------------------------
BYTE api_GetCollectionTaskStatus415(void);
//--------------------------------------------------
//��������: 6025_to_415�ɼ�����״̬������
//
//����:			TYPE : 0�������������ɼ�״̬�ر�----415������������ִ��
//				TYPE : 1�������������ɼ�״̬����----415��������ܾ�ִ��
//
//����ֵ:		BYTE
//
//��ע:
//--------------------------------------------------
void api_SetCollectionTaskStatus415(BYTE TYPE);
//-----------------------------------------------
//��������: �ɼ���ѭ������
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_MCollectTask(void);
#endif//#if(SEL_F415 == YES)
#endif // __MCOLLECT_API_H