//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.7.30
//����		����оƬ Apiͷ�ļ�
//�޸ļ�¼	
//�õ���ȫ�ֵ� ���峣��
//----------------------------------------------------------------------
#ifndef __SAMPLE_API_H
#define __SAMPLE_API_H

#if( SAMPLE_CHIP == CHIP_HT7017 )
#include ".\HT7017\HT7017.h"
#endif

#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
#include ".\HT7038\HT7038.h"
#endif
#if( SAMPLE_CHIP == CHIP_HT7627 )
#include ".\HT7627S\HT7627.h"
#endif
#if( SAMPLE_CHIP == CHIP_RN8302B )
#include ".\RN8302B\RN8302B.h"
#endif
//-----------------------------------------------
//				�궨��
//-----------------------------------------------						

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

//������ȫ�ռ�
//ע�⣺����ee�еĴ˽ṹ������Ҫ�ȴ�eSYS_STATUS_EN_WRITE_SAMPLEPARA��־��
typedef struct TSamplePara_t
{
	#if( SAMPLE_CHIP == CHIP_HT7017 )          
	TSampleAdjustReg    SampleAdjustReg;
	TSampleCtrlReg      SampleCtrlReg;
	TSampleCorr		    SampleCorr;
	TSampleVolCorr	    SampleVolCorr;
	BYTE			    Reserve[72];		//Ԥ���ռ� size0f(TSampleSafeRom) = 188
	#endif//#if( SAMPLE_CHIP == CHIP_HT7017 )
	
	#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
	TSampleOffsetPara 			SampleOffsetPara;
	TSampleCtrlPara 			SampleCtrlPara;
	TSampleAdjustPara 			SampleAdjustPara;
	TSampleAdjustParaDiv		SampleAdjustParaDiv;
	TSampleManualModifyPara		SampleManualModifyPara;
	TSampleHalfWavePara			SampleHalfWavePara;
	TSampleHalfWaveTimePara		SampleHalfWaveTimePara;
	BYTE						Reserve[148];	//Ԥ���ռ�
	#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
	#if( SAMPLE_CHIP==CHIP_HT7627 )
	THT7627SAdjustPara			HT7627SAdjustPara;
	BYTE						Reserve[120];	//Ԥ���ռ�
	#endif
	
	#if (SAMPLE_CHIP == CHIP_RN8302B)
	TSampleAdjustReg SampleAdjustReg;
	TSampleCtrlReg SampleCtrlReg;
	TSampleCorr SampleCorr;
	TEDTDataPara EDTDataPara;
	TSampleChipPara SampleChipPara;
    TSampleUIK	SampleUIKSafeRom;
	BYTE Reserve[80]; // Ԥ���ռ�
	#endif				  // #if( SAMPLE_CHIP == CHIP_RN2026 )
}TSampleSafeRom;


#if( SEL_AHOUR_FUNC == YES)
// ��ʱֵ�����ṹ��
typedef struct TAHour_t
{
	QWORD		AHour[MAX_PHASE+1]; //��λ0.01As
	DWORD		Rsv;                //��Ҫ�����������8����,Ҫ��֤CRC32�������4���ֽ�
	DWORD		CRC32;
}TAHour;

#endif



//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ������������������жϵ���
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע: ������������жϣ�������ö��Ĵ������˴�Ϊ�պ�����  
//-----------------------------------------------
void api_AddSamplePulse(void);


//-----------------------------------------------
//��������: ��ʼ������оƬ
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
BOOL api_InitSampleChip( void );

#if( MAX_PHASE != 1 )//������д˹���
//-----------------------------------------------
//��������: 	��¼�ϵ��ѹ
//
//����:		�� 
//                 
//����ֵ:	 	��
//		   
//��ע:   
//-----------------------------------------------
void api_RecordPowerUpVoltage(void);
#endif
//-----------------------------------------------
//��������: �ϵ��ʼ������оƬ
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_PowerUpSample(void);


//-----------------------------------------------
//��������: ���紦�����оƬ
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_PowerDownSample(void);

//-----------------------------------------------
//��������: ��ȡSAG״̬
//
//����:		�� 
//                 
//����ֵ: 	TRUE/FALSE
//		   
//��ע:   
//-----------------------------------------------
BYTE api_GetSAGStatus(void);

//-----------------------------------------------
//��������: ����оƬ��ѭ������
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_SampleTask(void);


//-----------------------------------------------
//��������: ��ȡ����оƬԶ������
//
//����:	
//	Type[in]:	
//		D15-D12 0:��				PHASE_ALL
//				1:A��				PHASE_A
//				2:B��				PHASE_B	
//				3:C��				PHASE_C	
//				4:N��				PHASE_N				
//		D07-D00										��λ	�ֽڳ���	С����λ��
//				0:��ѹ				REMOTE_U		V			4			3					
//				1:����				REMOTE_I		A 			4			4
//				2:��ѹ���			REMOTE_PHASEU	��			2			1	
//				3:��ѹ�������		REMOTE_PHASEI	��			2			1							
//				4:�й�����			REMOTE_P		kW			4			6
//				5:�޹�����			REMOTE_Q		kVAR		4			6
//				6:���ڹ���			REMOTE_S		kVA			4			6
//				7:һ����ƽ���й�����REMOTE_P_AVE	kW			4			6						
//				8:һ����ƽ���޹�����REMOTE_Q_AVE	kVAR		4			6	
//				9:һ����ƽ�����ڹ���REMOTE_S_AVE	kVA			4			6	
//				A:��������			REMOTE_COS					4			4
//				B:��ѹ����ʧ����					%			2			2 							
//				C:��������ʧ����					%			2			2			
//				D:��ѹг��������					%			2			2			
//				E:����г��������					%			2			2		
//				F:����Ƶ��			REMOTE_HZ		HZ			2			2
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//	Dot[in]			С��λ�� ��Ϊff��ΪĬ��С��λ��
//	Len[in]			���ݳ���
//  Buf[out] 		�������       
//����ֵ: 	TRUE:��ȷ����ֵ		FALSE���쳣
//		   
//��ע:  
//-----------------------------------------------
BOOL api_GetRemoteData(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *Buf);


//-----------------------------------------------
//��������: ��ȡ����оƬ����
//
//����:		Type[in]	
//				0x00--��ѹ��ƽ���� 
//				0x01--������ƽ����
//				0x02--������
//				0x10--��ѹ״̬
//				0x11--����״̬
//				0x20--��Ǳ��״̬
//				0x3X--��ǰ���޹����� 0x30~0x33 �ֱ������/A/B/C���޹���������
//����ֵ: 	0xffff--��֧�ִ�����
//				0x00--��ѹ��ƽ����	�������ͣ�long-unsigned����λ��%�����㣺-2 
//              0x01--������ƽ����	�������ͣ�long-unsigned����λ��%�����㣺-2
//				0x02--������		�������ͣ�long-unsigned����λ��%�����㣺-2
//				0x10--��ѹ״̬		Bit15 1--��ѹ����״̬��bit0~2	1--A/B/C���ѹС��0.6Un
//				0x11--����״̬		Bit15 1--��������״̬��bit0~2	1--A/B/C�����С��5%Ib
//				0x20--��Ǳ��״̬	1--Ǳ��		0--��
//				0x3X--��ǰ���޹����� ����1~4������һ����~������
//��ע:   
//-----------------------------------------------
WORD api_GetSampleStatus(BYTE Type);

//---------------------------------------------------------------
//��������: ���²��������� �������õ���ѹ���ʱ����
//
//����: 	��
//                   
//����ֵ:   ��
//
//��ע:   
//---------------------------------------------------------------
void api_InitSamplePara( void );

//-----------------------------------------------
//��������: У�����
//
//����:		AdjType[in] 	0xFF��	У���ʼ��
//							0x00��	��Ư
//							0x01:	��������� 1.0(L)
//							0x02:	С�������� 1.0(L)
//							0x51:	�������� 0.5L(L)
//							0x52:	С������� 0.5L(L)
//							0x81:	��������� N�� 1.0(N)
//							0xD1:	�������� N�� 0.5L(N)
//			Buf[in]			���ֽ��ں󣬵��ֽ���ǰ	AA HF���� AA A��˲ʱ�� AA B��˲ʱ�� AA C��˲ʱ�� AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA 	
//          
//����ֵ: 	TRUE/FALSE
//		   
//��ע:У���Լ����     
//-----------------------------------------------
WORD api_ProcSampleAdjust(BYTE AdjType, BYTE *Buf);


//-----------------------------------------------
//��������: ͨѶ����У��ϵ��
//
//����:		Operation[in]	READ/WRITE
//          Addr[in]		0xFFFF			У���ʼ��
//							0x0000~0x8FFF	����оƬ�Ĵ���
//							0x9000~0x9FFF	˲ʱ��ϵ�� 
//							0xA000~0xAFFF	��ѹӰ����ϵ��
//							0xB000~0xBFFF	���Ȳ���ϵ��
//			Len[in]			�������ݳ���
//			Buf[in/out]		�����򣬸��ں󣬵���ǰ
//
//����ֵ: 	TRUE/FALSE
//		   
//��ע: 7038��0000~3FFFΪУ���Ĵ���	4000~8fffΪ�����Ĵ���   
//-----------------------------------------------
BOOL api_ProcSampleCorr(BYTE Operation, WORD Addr, BYTE Len, BYTE *Buf);
#if(SAMPLE_CHIP != CHIP_HT7627)
//-----------------------------------------------
//��������: ��ʼ�������뾫���޹ز���
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_FactoryInitSample( void );
#endif
#if( SEL_STAT_V_RUN == YES )
// �����ǰ����
//Type 	BIT0��������		BIT1��������
void api_ClearVRunTimeNow( BYTE Type );
#endif

#if( SEL_AHOUR_FUNC == YES)
//-----------------------------------------------
//��������: �尲ʱֵ
//
//����:  ��
//
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
void api_ClrAHour( void );

//-----------------------------------------------
//��������: ����ʱֵ
//
//����:  Type[in]: ePHASE_TYPE
//		DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
// 		pBuf[out]: ָ�򷵻����ݵ�ָ��
//
//����ֵ:	��
//
//��ע:   TRUE   ��ȷ
//		  FALSE  ����
//-----------------------------------------------
BOOL api_GetAHourData( BYTE Type, BYTE DataType, BYTE *pBuf );

#endif//#if( SEL_AHOUR_FUNC == YES)
//-----------------------------------------------
//��������: ��ȡ������·��
//
//����:	��	
//
//����ֵ: ��·��	
//��ע:   
//-----------------------------------------------
eMeasurementMode api_GetMeasureMentMode(void);
//-----------------------------------------------
//��������: ���ü�����·��
//
//����:	��	
//
//����ֵ: �ɹ�/ʧ��	
//��ע:   
//-----------------------------------------------
BYTE api_SetSampleChip(BYTE bCount);
//-----------------------------------------------
//��������: ��ȡ����оƬУ������Ч���
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע:
//-----------------------------------------------
void GetSampleChipCheckSum(void);

//--------------------------------------------------
//��������:  ��ȡ�¶�
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
SWORD  api_GetTemp(void);
#endif//#ifndef __SAMPLE_API_H
