//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.26
//����		ȫʧѹ�¼������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __LOST_ALL_V_H
#define __LOST_ALL_V_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define MAX_LOST_ALL_V		1
#define START_TEST_LOST_ALLV	58

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//Ӳ��ȫʧѹ���ܵ�һЩ�ṹ����
typedef struct TLostAllVData_t
{

	TRealTimer LostAllVStartTime;
	DWORD Current[4];// A B C N
	TRealTimer LostAllVEndTime;
	WORD Sum;

}TLostAllVData;


typedef struct TPrgInfoLostAllVData_t
{
	DWORD Current[4]; 	//A B C N����
	BYTE	Reserved[10];		// Ԥ��
}TPrgInfoLostAllVData;


//ȫʧѹ
typedef struct TLostAllVRom_t
{
	TEventOADCommonData		EventOADCommonData;								// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TPrgInfoLostAllVData)];
}TLostAllVRom;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern __no_init DWORD LostAllVFlag;
extern __no_init TLostAllVData LostAllVData[MAX_LOST_ALL_V];

//-----------------------------------------------
// 				��������
//-----------------------------------------------
void AllLostVCheck( BYTE Type );
BYTE GetLostAllVStatus(BYTE Phase);
void PowerUpDealLostAllV( void );
void PowerDownDealLostAllV( void );
void FactoryInitLostAllVPara( void );
#endif//#ifndef __LOST_ALL_V_H
