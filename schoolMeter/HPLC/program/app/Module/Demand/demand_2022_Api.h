//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.5
//����		����ģ��Apiͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#if( SEL_DEMAND_2022 == YES )

#ifndef __DEMAND_API_H
#define __DEMAND_API_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
//�洢�����Ļ���
#define MAX_DEMAND_DOT			(5)

//��ȡ���������dotֵ�쳣ʱ�����õ�Ĭ��ֵ
#define DEFAULT_DEMAND_DOT		(4)
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//ռ12�ֽ�
typedef struct TDemandData_t
{
	DWORD DemandValue;
	TRealTimer DemandTime;
	
}TDemandData;


typedef struct TDemandDBase_t
{
	//�й������򡢷���
	TDemandData Active[2];

	//�������޹�
	#if( SEL_REACTIVE_DEMAND == YES )
	TDemandData RActive[4];
	#endif

	//У��
	DWORD CRC32;
	
}TDemandDBase;


// ��ȫ�ռ������ṹ���� ֻ�浱ǰ��
typedef struct TDemandSafeRom_t
{
	TDemandDBase TotalDemand;				// ���������
	TDemandDBase RatioDemand[MAX_RATIO];	// �����������

}TDemandSafeRom;

typedef enum
{
   	eClearRamDemand = 0,         	//��Ram������������ǰ�������������
   	eClearAllDemand = 0x55          //��ȫ��������������ǰ��������������Լ�e2�е�����
}eClearDemandType;//���ݼ�TAG��־

//��ͣ����������־ö��
typedef enum
{
	eDemandRun = 0,//ִ����������
	eDemandPause = 0x55,//��ͣ��������
}eDemandPauseFlag;

//6����������ö��
typedef enum
{
	ePActiveDemand = 0,//�����й�
	eNActiveDemand = 1,//�����й�
	eReactive1Demand = 2,//1�����޹�
	eReactive2Demand = 3,//2�����޹�
	eReactive3Demand = 4,//3�����޹�
	eReactive4Demand = 5,//4�����޹�
	eMaxDemand6Type,//���������������
}eDemand6Type;

//���������������ö��
typedef enum
{
	eTotalDemand = 0,//���������������
	eCurRateDemand = 1,//��ǰ�����������������
	eMaDemandRateType,//������������������ֵ
}eDemandRateType;


//���������õĹ��ʵķ���
typedef enum
{
	eDemandPForward = 0,//���ʷ���Ϊ����ö��ֵ��ҪΪ0
	eDemandPReverse = 1,//���ʷ���Ϊ��
}eDemandPDir;

//���������õĹ���״̬
typedef struct TDemandPStatus_t
{
	BOOL IsStart;//TRUE/FALSE ��/��������
	eDemandPDir PDir;//���ʷ���
}TDemandPStatus;
//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: �����������������ÿ��ִ��һ��
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_DemandTask( void );

//-----------------------------------------------
//��������: ��������
//
//����: 	eType[in]��		Ҫ������������
//      eClearRamDemand	�嵱ǰ���������ڴ��е�ǰ�������
//	    eClearAllDemand	��eeprom�е�ǰ�������		
//
//			InDemandPauseFlag[in]: ��ͣ����������־
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_ClrDemand( eClearDemandType eType, eDemandPauseFlag InDemandPauseFlag );


//-----------------------------------------------
//��������: ����ģ���ʼ��
//
//����:		
//                    
//����ֵ:     ��
//
//��ע:   
//-----------------------------------------------
void api_InitDemand(       );

//-----------------------------------------------
//��������: ����ģ���ϵ��ʼ��
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_PowerUpDemand( void );

//-----------------------------------------------
//��������: ����ǰ����������֧�ַ���������
//
//����:
//	Type[in]:	1  	�й�
//				2	�޹�
//				3	����
//	DataType[in]:	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//	Dot[in]:	С����λ��
//	pDemand[out]:�������
//
//����ֵ:	��

//��ע: DATA_BCD��ʽ���λλ����λ
//-----------------------------------------------
void api_GetCurrDemandData( WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *pDemand );

//-----------------------------------------------
//��������: �����������������ʱ��
//
//����: 
//	Type[in]:	Bit15		1--���������������	0--�Ƕ��������������
//				P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//  Ratio[in]:	����
//				1-MAX_RATIO  	 ����������
//				0				 �������ܼ�             
//	Dot[in]:	С����λ��
//	Buf[out]:	�������
//
//����ֵ:	TRUE/FALSE

//��ע: 
//-----------------------------------------------
BOOL api_GetDemand(WORD Type, BYTE DataType, WORD Ratio, BYTE Dot, BYTE * Buf);

//-----------------------------------------------
//��������: ��RAM�л����6�ֵ�ǰ�����������������й��������й���1~4���ޣ���֧�ַ���������
//
//����: 
//	Type[in]:	�������ͣ����������й��������й���1~4�����޹�
//				
//	DataType[in]:	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)      
//	Dot[in]:	С����λ��
//	Len[in]:    ��ȡ��������
//	pDemand[out]:�������
//
//����ֵ:	��

//��ע: DATA_BCD��ʽ���λλ����λ
//-----------------------------------------------
void api_Get6TypeCurrDemandData(eDemand6Type Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE * pDemand);

//-----------------------------------------------
//��������: ����6������ͨ����Ӧ�ĵ�������������ʱ������
//
//����:		Type[in]��	ePActiveDemand --�����й�
//						eNActiveDemand --�����й�
//						eReactive1Demand --1�����޹�
//						eReactive2Demand --2�����޹�
//						eReactive3Demand --3�����޹�
//						eReactive4Demand --4�����޹�
//			PulseNum[in]:		��������
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------

void api_UpdateDemandEnergyPulse( eDemand6Type Type, WORD PulseNum );


#endif//#ifndef __DEMAND_API_H
#endif
