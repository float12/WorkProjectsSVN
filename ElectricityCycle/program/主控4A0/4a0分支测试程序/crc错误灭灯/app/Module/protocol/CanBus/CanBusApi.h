//----------------------------------------------------------------------
// Copyright (C) 2022-2028 ��̨������˼�ٵ����ɷ����޹�˾��ѹ̨����Ʒ��
// ������
// ��������
// ����

// �޸ļ�¼
//----------------------------------------------------------------------
#ifndef __CANBUS_H
#define __CANBUS_H

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define MAX_CANBUS_ID_NUM       (32)   // ���32������ID
#define MAX_CHARGING_NUM        (16)
#define MAX_CHARGING_W_BITNUM   (15)
#define MAX_CHARGING_E_BITNUM   (59)
#define MAX_CHARGING_PE_BITNUM  (25)
#define BIT_NEED_BYTE(X)		((X / 8) + ((X % 8) ? 1 : 0))
#define MAX_CHARGING_W_BYTENUM  (BIT_NEED_BYTE(MAX_CHARGING_W_BITNUM))
#define MAX_CHARGING_E_BYTENUM  (BIT_NEED_BYTE(MAX_CHARGING_E_BITNUM))
#define MAX_CHARGING_PE_BYTENUM (BIT_NEED_BYTE(MAX_CHARGING_PE_BITNUM))
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eADDID,
	eDELID,
	eCLEANID
} eIDDOtype;
typedef enum
{
	eChargingGet,
	eChargingSet
} eCharging698type;
#pragma pack(1)
// ����������1
typedef union TCanBusID_t
{
	struct
	{
		BYTE TxID      : 8;   // ���ͷ�ID
		BYTE RxID      : 8;   // ���շ�ID
		BYTE Rev1      : 7;   // ����
		BYTE Next      : 1;   // ����֡��־
		BYTE Direction : 1;   // ����
		BYTE Broadcast : 1;   // �㲥
		BYTE Priority  : 3;   // ���ȼ�
		BYTE Rev2      : 3;   // ����
	} ID_Bit;
	DWORD ID;
} TCanBusID;
typedef struct TCanBusIDMap_t
{
	BYTE ID;
	BYTE USE_FLAG;
	BYTE CHARGING_NO;
} TCanBusIDMap;

typedef struct TCanBusPara_t
{
	BYTE bSelfNode;                                    // ����ID
	BYTE bRxNum;                                       // ��Ч����
	BYTE bRxNode[MAX_CANBUS_ID_NUM];                   // ����ID
	TCanBusIDMap tCanBusIDMap[MAX_CHARGING_NUM + 1];   // 16ǹ�ӳ��׮
	BYTE OnlyListen;
	DWORD dwCrc;
} TCanBusPara;

typedef struct TCanBusInfo_t
{
	BYTE bDelay;                     // ���߿�����ʱ
	BYTE bSelfID;                    // ����ID
	BYTE bRxID[MAX_CANBUS_ID_NUM];   // ����ID
	BYTE bTxFail;                    // ����ʧ�ܱ�־
	BYTE TryNum;                     // ���ͳ�ʵ����

} TCanBusInfo;

typedef struct ChargingData_t
{
	BYTE PF11H[4];
	BYTE PF13H[4];
	BYTE PF15H[8];
	BYTE PF20H[36];
	BYTE PF21H[8];
	BYTE PF22H[8];
	BYTE PF23H[8];
	BYTE PF40H[8];
	BYTE PF41H[8];
	BYTE PF51H[8];
	BYTE PF52H[8];
	BYTE PF61H[37];
	BYTE Safe[20];
} TChargingData;

#pragma pack()
extern TChargingData ChargingData[MAX_CHARGING_NUM];
extern TCanBusPara CanBusPara;
extern BYTE ChargingSaveEventBuf[50];
extern const BYTE ListenType;
//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
// ��������: �ϵ��ʼ����Ϣ�ṹ��
//
// ����:
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
void api_PowerUpCanBus(void);

//-----------------------------------------------
// ��������: �ж��Ƿ�Ϊ��Ч����(��·��)
//
// ����: 	tInfo[in]:ID��Ϣ
//
// ����ֵ:  TRUE/FLASE
//
// ��ע:
//-----------------------------------------------
BYTE api_JudgeCanBusPro(TCanBusID tInfo);

//-----------------------------------------------
// ��������: У��CAN���߲���
//
// ����:
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
void api_CheckCanBusID(void);
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
//BOOL api_ChargingCan(stc_can_rxframe_t cantemp);
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void api_ChargingCanTask(BYTE ChargingNo);
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void FactoryInitChargingPara(void);
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL ChangeID(eIDDOtype iddotype,BYTE *para ,BYTE Size);
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
WORD Charging698(eCharging698type type, BYTE *buf, WORD OI, BYTE ClassAttribute, BYTE ChargingNo, BYTE ClassIndex);
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BYTE Charging698AddTag(BYTE ClassAttribute, WORD OI, BYTE *Inbuf, BYTE *Outbuf, BYTE ClassIndex);
//--------------------------------------------------
// ��������:  ���ó��׮����
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BYTE SetCharging(BYTE *pOAD, BYTE *pbuf);
//--------------------------------------------------
// ��������:  �������
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL ChargingIDToNum(BYTE *ID);
//--------------------------------------------------
// ��������:  ������
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL ADDChargingMap(BYTE ID);
//--------------------------------------------------
// ��������:  ɾ�����
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL DeleteChargingMap(BYTE ID);
//--------------------------------------------------
// ��������:  ���ұ���OI
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL ChargingReallyOI2SaveOI(WORD *OI, BYTE ID);
//--------------------------------------------------
// ��������:  ������ʵOI
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL ChargingSaveOI2ReallyOI(WORD *OI);
//--------------------------------------------------
// ��������:  ����CAN����ͨ��ID
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void api_SetCanID(BYTE bType, BYTE bTar);
//--------------------------------------------------
// ��������:  ��ȡCAN����ͨ��ID
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
DWORD api_GetCanID(void);
//--------------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void ChangeCanMod(void);
//--------------------------------------------------
//��������:  
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_SetCanMod( BYTE para );
//--------------------------------------------------
//��������:  
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BYTE  api_GetCanMod( void );
//--------------------------------------------------
//��������:  ����¼�ʱͬʱ���˲ʱ��
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void api_FactoryInitChargingEvent(BYTE eChargeingIndex);
#endif