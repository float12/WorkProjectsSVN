//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з��� 
//������	
//��������	
//����		READMETERͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __READMETER_H
#define __READMETER_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define UNSIGNED 	0x00 //�޷���
#define SIGNED 		0x80   //�з���
#define PT_MULTI 	0x01 //�ϴ����ݳ�PT
#define CT_MULTI 	0x02
#define PT_CT_MULTI 0x03
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct tMeterRead_t
{
	DWORD OI;	//645���ݱ�ʶ
	BYTE Num;	//һ�����ݿ��м���������
	BYTE Slen;   //�����������
	BYTE Dot;	//��������С��λ
	BYTE Symbol; //�Ƿ���Ҫ�������λ bit7 �����ж� bit1 bit0 ���ֱ��
	double *Buf; //���ݴ�ŵ�ַ
} tMeterRead;


typedef enum
{
	eUa = 0, // ��ѹ���ݿ�
	eUb,
	eUc,
	eIa,  // �������ݿ�
	eIb,
	eIc,
	eIzs, // �������
	eP1, // �й��������ݿ�
	eP2,
	eP3,
	eP4,
	eQ1, // �޹��������ݿ�
	eQ2,
	eQ3,
	eQ4,
	eS1, // ���ڹ������ݿ�
	eS2,
	eS3,
	eS4,
	ePF1, // �����������ݿ�
	ePF2,
	ePF3,
	ePF4,
	eComActive, //����й��ܵ���
	ePActiveAll, // �����й��ܵ���
	eNActiveAll, // �����й��ܵ���
	eRActive1All, // �޹�1�ܵ���
	eRActive2All, // �޹�2�ܵ���
	ePa_PActive1, //a�������й�
	ePa_NActive1, //a�෴���й�
	ePa_ComRActive1, //a������޹�1
	ePa_ComRActive2, //a������޹�2
	ePb_PActive1, //b�������й�
	ePb_NActive1, //b�෴���й�
	ePb_ComRActive1, //b������޹�1
	ePb_ComRActive2, //b������޹�2
	ePc_PActive1, //c�������й�
	ePc_NActive1, //c�෴���й�
	ePc_ComRActive1, //c������޹�1
	ePc_ComRActive2, //c������޹�2
	eTemp1, //�¶�1
	eTemp2, //�¶�2
	eTemp3, //�¶�3
	eTemp4, //�¶�4
	eLineUa,
	eLineUb,
	eLineUc,
	eNullType,
} eValueType;//  �޸�MeterReadOI�������Ӧ�޸�ö��ƫ�� 
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern const tMeterRead MeterReadOI[];
extern const BYTE CycleReadMeterNum;
//-----------------------------------------------
// 				��������
//-----------------------------------------------
#if (CYCLE_METER_READING == YES)
//--------------------------------------------------
//��������:  645 ��������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Dlt645_Tx_ContinueRead( BYTE bStep, BYTE *bBuf);
//-----------------------------------------------
//��������:  �������ݽ���������
//
//����:      pBuf[in]  dataBuf[in] ���ݱ���λ��
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void DataAnalyseandSave(BYTE *pBuf, double *dataBuf, BYTE Len, tMeterRead tMeterReadOIRam);
//--------------------------------------------------
//��������: �������ݴ��͸���ͬ�ϱ���֡����
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void SampleDatatoReport(TRealTimer *pTime);
#endif
#endif //#ifndef __READMETER_H
