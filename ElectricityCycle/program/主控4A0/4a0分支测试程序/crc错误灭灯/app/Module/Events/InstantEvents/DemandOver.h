//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.24
//����		���������¼�ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------

#ifndef __DEMANDOver_H
#define __DEMANDOver_H

#if( SEL_DEMAND_OVER == YES )
//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
// �����й����������¼�
typedef struct TDemandOverRom_t
{
	TEventOADCommonData		EventOADCommonData;			// ÿ������඼�еĹ�ͬ����
	BYTE					EventData[sizeof(TEventNormal)];
	BYTE					Data[sizeof(TDemandData)];	// �����ڼ������й��������ֵ������ʱ��
}TDemandOverRom;


//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------
BOOL GetPADemandOverStatus( BYTE DemandOverType );

BOOL GetNADemandOverStatus( BYTE DemandOverType );

BOOL GetQDemandOverStatus( BYTE DemandOverType );

void FactoryInitDemandOverPara( void );

void DealDemandOverMaxDemand( void );
#endif//#if( SEL_DEMAND_OVER == YES )

#endif//__DEMANDOver_H

