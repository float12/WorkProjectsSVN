//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.20
//����		��ѹ�ϸ���ͷ�ļ�
//��ע   δ��ȫ���¼�event.c���룻��������event.c���룻���뵽sampleģ����
//----------------------------------------------------------------------

#ifndef __STATISTICS_VOLTAGE_H
#define __STATISTICS_VOLTAGE_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//��ѹ�ϸ��ʽṹ
typedef struct TStatVRunData_t
{
	DWORD dwMonitorTime;//�ܼ��ʱ��
	WORD wPassRate;//�ϸ���
	WORD wOverRate;//������
	DWORD dwOverTime;//������ʱ��
	DWORD dwBelowTime;//������ʱ��

}TStatVRunData;


//��ѹ�ϸ��ʲ����ṹ
typedef struct TVStatisticsPara_t
{
	WORD VOverHigh;//��ѹ�������� ��λ0.1v
	WORD VOverLow;//��ѹ�������� ��λ0.1v
	WORD VPassHigh;//��ѹ�ϸ����� ��λ0.1v
	WORD VPassLow;//��ѹ�ϸ����� ��λ0.1v

}TVStatisticsPara;


//��ѹ�ϸ��ʴ洢�ṹ
typedef struct TStatVRunDataRom_t
{
	//������
	TStatVRunData DayStatVRunData[4];
	//������
	TStatVRunData MonStatVRunData[4];

}TStatVRunDataRom;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern __no_init DWORD ClrVRunDayTimer;	// ���յ�ѹ�ϸ��ʶ�ʱ������
extern __no_init DWORD ClrVRunMonTimer;	// ���µ�ѹ�ϸ��ʶ�ʱ������

//-----------------------------------------------
// 				��������
//-----------------------------------------------

//------------------------------------------------------------------------------------
// �����¼���¼
BYTE ReadVRunTime(BYTE Phase, BYTE No, BYTE *pBuf);
void SwapVRunTime(void);
void StatVRunTime(void);


void FactoryInitStatisticsVPara( void );
void PowerUpVRunTime( void );
#endif//__STATISTICS_VOLTAGE_H

