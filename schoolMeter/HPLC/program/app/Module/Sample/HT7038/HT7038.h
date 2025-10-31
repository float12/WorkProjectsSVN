//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.8
//����		����оƬHT7038 ģ����ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __HT7038_H
#define __HT7038_H

#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define SEL_VOL_ANGLE_FUNC	YES
#define SEL_MEASURE_FREQ	YES

//���ߵ���ͨ������ѡ��
#define CUR_ZERO_CURRENT_GAIN	16		//ֻ��ѡ1,2,8,16
//��ѹֵ ��λ0.01mv 9��169k 1��33k 1��1k
#define	VOL_VALUE		( ((DWORD)(wCaliVoltageConst/10)*1*SimpleVolConst/10) * wSampleVolGainConst )
//����ֵ 
#define	CURRENT_VALUE	((WORD)(wSampleCurGainConst*SimpleCurrConst))
#define	CURRENT_MIN_VALUE	((WORD)(wSampleMinCurGainConst*SimpleCurrConst))

//���ߵ�������оƬADC����ֵ
#define ZERO_CURRENT_VALUE	((float)CUR_ZERO_CURRENT_GAIN * (float)ZeroSampleCurrConst / 1000.0)
#define	UN_IB_EC		((float)(wCaliVoltageConst/10)*((float)wMeterBasicCurrentConst/1000)*(api_GetActiveConstant()))
//����2��3��4��6��12�ܹ����� ���Գ���12�ٳ���12
//HFConst��INT[2.592*10^10*G*G*Vu*Vi/(EC*Un*Ib)] G=1.163 ϵ��Ϊ2.592 * 1.163 * 1.163 = 3.505858848
//ϵ������35058.58848����VOL_VALUE����100������ϵ������100
#define	HFCONST_DEF		( (DWORD)(((float)350.5858848*VOL_VALUE*CURRENT_VALUE/UN_IB_EC+6)/12)*12 )//��֤���Ա�12����

#define	HFCONST_MIN_DEF		( (DWORD)(wSampleMinCurGainConst*HFCONST_DEF/wSampleCurGainConst))//��֤���Ա�12����(С�������汶����һ��ʱʹ�ã����⴦��)


#define TWO_POW24		(DWORD)0x01000000
#define TWO_POW23		(DWORD)0x00800000
#define TWO_POW22		(DWORD)0x00400000
#define TWO_POW21		(DWORD)0x00200000
#define TWO_POW20		(DWORD)0x00100000
#define TWO_POW19		(DWORD)0x00080000
#define TWO_POW18		(DWORD)0x00040000
#define TWO_POW17		(DWORD)0x00020000
#define TWO_POW16		(DWORD)0x00010000
#define TWO_POW15		(DWORD)0x00008000
#define TWO_POW14		(DWORD)0x00004000
#define TWO_POW13		(DWORD)0x00002000
#define TWO_POW12		(DWORD)0x00001000
#define TWO_POW11		(DWORD)0x00000800
#define TWO_POW10		(DWORD)0x00000400
#define TWO_POW09		(DWORD)0x00000200
#define TWO_POW08		(DWORD)0x00000100
#define TWO_POW07		(DWORD)0x00000080
#define TWO_POW06		(DWORD)0x00000040
#define TWO_POW05		(DWORD)0x00000020
#define TWO_POW04		(DWORD)0x00000010
#define TWO_POW03		(DWORD)0x00000008
#define TWO_POW02		(DWORD)0x00000004
#define TWO_POW01		(DWORD)0x00000002
#define TWO_POW00		(DWORD)0x00000001


#define r_DeviceID	0x00
#define r_PA		0x01//�й�����
#define r_PB		0x02
#define r_PC		0x03
#define r_PT		0x04
#define r_QA		0x05//�޹�����
#define r_QB		0x06
#define r_QC		0x07
#define r_QT		0x08
#define r_SA		0x09//���ڹ���
#define r_SB		0x0a
#define r_SC		0x0b
#define r_ST		0x0c

#define r_UARMS		0x0d//��ѹ��Чֵ
#define r_UBRMS		0x0e
#define r_UCRMS		0x0f
#define r_IARMS		0x10//������Чֵ
#define r_IBRMS		0x11
#define r_ICRMS		0x12
#define r_ITRMS		0x13

#define r_PFA		0x14//��������
#define r_PFB		0x15
#define r_PFC		0x16
#define r_PFT		0x17
#define r_PGA		0x18//�����͵�ѹ�н�
#define r_PGB		0x19
#define r_PGC		0x1a

#define r_INTFlag	0x1b//�жϱ�־�������Զ�����

#define r_Freq		0x1c//Ƶ��

#define r_EFlag		0x1d//���ܼĴ����Ĺ���״̬����������
#define r_EPA		0x1e//A���й����ܣ�������Ϊ�������㣩
#define r_EPB		0x1f
#define r_EPC		0x20
#define r_EPT		0x21
#define r_EQA		0x22
#define r_EQB		0x23
#define r_EQC		0x24
#define r_EQT		0x25

#define r_YUAUB		0x26//A���ѹ��B���ѹ�ļн�
#define r_YUAUC		0x27
#define r_YUBUC		0x28

#define r_I0RMS		0x29//���ߵ�����Чֵ

#define r_TPSD		0x2a//�¶�

#define r_UTRMS		0x2b//�����ѹʸ����

#define r_SFlag		0x2c//���ࡢ����ȱ�־״̬

#define r_BckReg	0x2d//ͨѶ���ݼĴ���

#define r_ComChkSum	0x2e//ͨѶУ��ͼĴ���

#define r_Sample_IA	0x2f//����ֵ�Ĵ���
#define r_Sample_IB	0x30
#define r_Sample_IC	0x31
#define r_Sample_UA	0x32
#define r_Sample_UB	0x33
#define r_Sample_UC	0x34

#define r_ESA		0x35//���ڵ���
#define r_ESB		0x36
#define r_ESC		0x37
#define r_EST		0x38
#define r_FstCntA	0x39//�����������
#define r_FstCntB	0x3a
#define r_FstCntC	0x3b
#define r_FstCntT	0x3c

#define r_LineIaRrms	0x4b//����/г��������Чֵ
#define r_LineIbRrms	0x4c
#define r_LineIcRrms	0x4d


#define r_PFlag		0x3d//���ʷ��� 0��1��

#define r_ChkSum	0x3e//У������У���

#define r_Sample_I0	0x3f//���ߵ�����������

#define r_ChkSum1	0x5e//�°���չУ������У���

//HT7034(7036)�¼ӼĴ���
#define w_WaveBuffTx	0x7E
#define w_WaveData		0x7F

//0x00 ����
#define w_ModeCfg	0x01
#define w_PGACtrl	0x02
#define w_EMUCfg	0x03

#define w_PGainA	0x04
#define w_PGainB	0x05
#define w_PGainC	0x06
#define w_QGainA	0x07
#define w_QGainB	0x08
#define w_QGainC	0x09
#define w_SGainA	0x0a
#define w_SGainB	0x0b
#define w_SGainC	0x0c

#define w_PhSregApq0	0x0d
#define w_PhSregBpq0	0x0e
#define w_PhSregCpq0	0x0f
#define w_PhSregApq1	0x10
#define w_PhSregBpq1	0x11
#define w_PhSregCpq1	0x12
#define w_POffsetA		0x13
#define w_POffsetB		0x14
#define w_POffsetC		0x15

#define w_QPhsCal		0x16

#define w_UGainA		0x17
#define w_UGainB		0x18
#define w_UGainC		0x19
#define w_IGainA		0x1a
#define w_IGainB		0x1b
#define w_IGainC		0x1c

#define w_IStartup		0x1d

#define w_HFConst		0x1e

#define w_FailVoltage	0x1f

#define w_GainADC7		0x20

#define w_QOffsetA		0x21
#define w_QOffsetB		0x22
#define w_QOffsetC		0x23
#define w_UARMSOffset	0x24
#define w_UBRMSOffset	0x25
#define w_UCRMSOffset	0x26
#define w_IARMSOffset	0x27
#define w_IBRMSOffset	0x28
#define w_ICRMSOffset	0x29
#define w_UOffsetA		0x2a
#define w_UOffsetB		0x2b
#define w_UOffsetC		0x2c
#define w_IOffsetA		0x2d
#define w_IOffsetB		0x2e
#define w_IOffsetC		0x2f

#define w_EMUIE			0x30
#define w_ModuleCfg		0x31
#define w_AllGain		0x32
#define w_HFDouble		0x33
//0x34 ��
#define w_PinCtrl		0x35
#define w_PStart		0x36

#define w_IRegion		0x37

#define w_POffsetAL		0x64
#define w_POffsetBL		0x65
#define w_POffsetCL		0x66
#define w_QOffsetAL		0x67
#define w_QOffsetBL		0x68
#define w_QOffsetCL		0x69

#define w_ItRmsOffset	0x6A
//Toffset У���Ĵ���(У�����0x6B) 
#define w_Toffset		0x6B

//Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB��TCcoffC
#define w_TCcoffA		0x6D
#define w_TCcoffB		0x6E
#define w_TCcoffC		0x6F

//�����㷨���ƼĴ���
#define w_EMCfg			0x70
//0x38 ����
//0x39 ����


//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//оƬ���Ʋ���
typedef struct TSampleCtrlPara_t
{
	WORD ModeCfg;

	WORD PGACtrl;

	WORD EMUCfg;

	WORD QPhsCal;

	WORD IStartup;

	WORD HFConst;

	WORD FailVoltage;

	//WORD GainADC7;

	WORD EMUIE;

	WORD ModuleCfg;

	WORD AllGain;

	WORD HFDouble;

	WORD PinCtrl;

	WORD PStart;

	WORD IRegion;
	
	WORD EMCfg;

	DWORD CRC32;//4�ֽ�У��

}TSampleCtrlPara;


typedef struct TSampleAdjustPara_t
{
	WORD PGain[3];//�й���������ϵ��

	WORD QGain[3];

	WORD SGain[3];

	WORD PhsReg[2][3];//�Ƕ�����ϵ��

	WORD UGain[3];//��ѹ����ϵ��

	WORD IGain[3];//��������ϵ��
	
	WORD NGain;	//���ߵ�������ϵ��

	DWORD CRC32;//4�ֽ�У��

}TSampleAdjustPara;

typedef struct TSampleOffsetPara_t
{
	WORD POffset[3];//�й�����ƫ��

	WORD POffsetL[3];//�й�����ƫ��
	
	WORD QOffset[3];
	
	WORD QOffsetL[3];

	WORD URMSOffset[3];

	WORD IRMSOffset[3];

	WORD ItRmsOffset;//����ʸ����ƫ��
	
	WORD I0RMSOffset;//���ߵ���ƫ��
	
	DWORD CRC32;//4�ֽ�У��

}TSampleOffsetPara;

//����ϵ�� �ֶβ���ϵ��
typedef struct TSampleAdjustParaDiv_t
{
	WORD PGainDiv[3][10];					//��չ9������ϵ��

	WORD PhsRegDiv[3][10];					//��չ9�νǲ�ϵ��

	DWORD CRC32;
}TSampleAdjustParaDiv;

//�ֶ�����ϵ��
typedef struct TSampleManualModifyPara_t
{
	BYTE		ManualModifyChipToffset;	//����оƬ��TOffset�²���ֵ������25���²����Ĵ���0x2A������ֵ
	signed char ManualModifyForWard[9];		//�����й����������������������沿��
	signed char ManualModifyForRA[9];  		//�����й����������������������沿��
	signed char ManualModifyQGain[3][9];	//�ֶ������޹������洢����ֻ���޹������������
	WORD 		VolUnbalanceGain[3];		//��ѹ��ƽ�ⲹ��ϵ��
	//WORD 		VolChangeGain[3];			//��ѹ�ı䲹��ϵ��
	DWORD CRC32;
}TSampleManualModifyPara;

//ֱ��ż��г��ϵ��
typedef struct TSampleHalfWavePara_t
{
	WORD		Coe[3][2];					//0.5Lֱ��ż��г�� [][0]У׼ֵ [][1]����ֵ
	WORD		Gain[3][2];					//1.0Lֱ��ż��г�� [][0]У׼ֵ [][1]����ֵ
	DWORD		CRC32;
}TSampleHalfWavePara;

//ֱ��ż��г�����ʱ��
typedef struct TSampleHalfWaveTimePara_t
{
	BYTE		Hours;						//Сʱ
	DWORD		CRC32;
}TSampleHalfWaveTimePara;

//Զ������
typedef struct TRemoteValue_t
{
	TFourByteUnion U[3];				//�޷���ԭ���ʽ
	TFourByteUnion I[5];				//�����ʽ	��λA ����3λС�� A B C N1 N2 
	TFourByteUnion P[4];				//0������ֵ//�����ʽ	
	TFourByteUnion Q[4];				//�����ʽ		
	TFourByteUnion S[4];				//���ڹ���//�޷���ԭ���ʽ
	TFourByteUnion Cos[4];				//�����ʽ	
	
	#if( SEL_VOL_ANGLE_FUNC == YES )
	TFourByteUnion YUaUbUcAngle[3];		//�����ʽ	
	TFourByteUnion YUIAngle[3];			//�����ʽ
	#endif
	
	TFourByteUnion Freq;				//�޷���ԭ���ʽ
	TFourByteUnion PQSAverage[3][4];	//һ����ƽ������PQS
	
	TTwoByteUnion Uunbalance;			//��ѹ��ƽ����
	TTwoByteUnion Iunbalance;			//������ƽ����	
	
	TFourByteUnion RI[3];				//�����ʽ	�¼��ж�ʹ�õĵ��� 
	//�¶�
	short Temperature;	
	
}TRemoteValue;

//�ֶ�У���ͻ�����������
typedef struct TCurrenttValue_t
{
	DWORD Upper;
	DWORD Lower;
}TCurrenttValue;

//����Ĭ������ʹ�С���ϵ��
typedef struct TOriginalPara_t
{
	WORD AngCoe[2][MAX_PHASE];						//ԭʼ��λϵ��
	WORD PGainCoe[MAX_PHASE];						//ԭʼ�й�����ϵ��
	
	DWORD CRC32;

}TOriginalPara;

//ֱ��ż��г������
typedef enum
{
	eAD_HALFWAVE_NO = 0,	//���ǰ벨
	eAD_HALFWAVE_YES,		//�ǰ벨
}eGetHalfWaveStatus;

//-----------------------------------------------
//				��������
//-----------------------------------------------

extern TRemoteValue 			RemoteValue;				//Զ������
extern TSampleCtrlPara			SampleCtrlPara;
extern TSampleAdjustPara		SampleAdjustPara;
extern TSampleOffsetPara		SampleOffsetPara;
extern TSampleAdjustParaDiv 	SampleAdjustParaDiv;		//�ֶβ�������
extern TSampleManualModifyPara	SampleManualModifyPara;		//�ֶ�����ϵ��
extern TSampleHalfWavePara		SampleHalfWavePara;			//ֱ��ż��г��ϵ��
extern TSampleHalfWaveTimePara	SampleHalfWaveTimePara;		//ֱ��ż��г��Ҫ����ʱ��(Сʱ)
extern TOriginalPara			OriginalPara;	
extern TCurrenttValue			CurrentValue[9];
extern BYTE 	bySamplePowerOnTimer;
extern DWORD	AdjustDivCurrent[10];		//�ֶ�У�������׼ֵ
extern DWORD	dwBigStandardCurrent;		//�����У����Ӧ����
extern WORD		wSmallStandardCurrent;		//С���У����Ӧ����
extern BYTE 	g_ucNowSampleDivisionNo[MAX_PHASE];
extern BOOL 	UnBalanceGainFlag[3];			//��ѹ��ƽ�ⲹ����־ TRUE-�Ѳ��� FALSE-δ����
//-----------------------------------------------
// 				��������
//-----------------------------------------------
//����������
DWORD ReadMeasureData( BYTE Addr );
//��У������
DWORD ReadCalibrateData( BYTE Addr );
BOOL WriteSampleReg( BYTE Addr, DWORD Data );
//��ȡ����оƬ�ļĴ���Ч���
void GetSampleChipCheckSum(void);
void UseSampleDefaultPara(WORD Type);
BOOL WriteSampleChipOffsetPara(void);
BOOL WriteSampleChipCtrlPara(void);
BOOL WriteSampleChipAdjustPara(void);
void InitSampleCaliUI( void );
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,BYTE Addr, BYTE *Buf);
BOOL Link_ReadSampleReg(BYTE Addr, BYTE *pBuf);
//7038У��Ĵ���д��������
void SampleSpecCmdOp(BYTE Cmd);
BYTE GetRowSampleSegmentNo( BYTE ucPhase );
BYTE GetRowSampleSegmentAdjustNo( BYTE ucPhase );
BOOL JudgeUnBalance( BYTE Phase );
void FreshCurrent( void );


#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )


#endif//#ifndef __HT7038_H

