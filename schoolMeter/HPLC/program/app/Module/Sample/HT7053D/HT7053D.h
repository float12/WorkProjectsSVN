//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.7.30
//����		����оƬHT7017 ģ����ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __HT7053D_H
#define __HT7053D_H

#if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )
//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define SEL_MEASURE_FREQ	YES
#define SEL_CRYSTALOSCILLATOR_55296M	YES				//�ⲿ����,һ��Ĭ��ѡ��6M�ľ���,5.5296M�ľ������⴦��
//��ȡ��˲ʱֵ
#define	IARMS			0x06
#define	IBRMS			0x07
#define	URMS			0x08
#define	Freq_U			0x09
#define	PowerPA			0x0a
#define	PowerPB			0x10
#define	SPower			0x0c
#define	EmuStatus		0x19
#define	DeviceID		0x1c

//����оƬͨ��У��ͼĴ���
#define COMCHECKSUM     0x17
#define CHIPID    		0x1B

//У�����
#define	GPA				0x50
#define	GQA				0x51
#define	GSA				0x52
#define GPB     		0x54
#define	HFConst			0x61
#define	P1OFFSET		0x65
#define	IBGain			0x5b
#define	PStart			0x5f
#define	GPhs1			0x6d
#define	IARMSOS			0x69
#define	IBRMSOS			0x6a
#define P2OFFSET    	0x66
#define GPhs2			0x6e
#define	ITAMP			0x63
#define	P1OFFSETL		0x76
#define QPhsCal			0x58
#define DEC_Shift		0x64

//˲ʱ��ϵ��Ĭ��ֵ
#define METER_Uk				0x1F25		//ԭΪ0x1D92��HPLC�޸�lsc 
#define METER_I1k				0x1325		//ԭΪ0x0F6F��HPLC�޸�lsc 
#define METER_I2k				0x114C
#define METER_Pk				0x9167		 

//�ֵ�������У��
#define BASE_ADJ		0x01
//����(���У��)
#define ACCURATE_ADJ	0x02

//��ѹӰ����֧�ֵĵ���
// 1~2  |  Imin  -  5%   | 1.0  | 90%/110%
// 3~5  |  Itr   -  10%  | 1.0  | 80%/85%/115%
// 6~7  |  Itr   -  10%  | 0.5L | 90%/110%
// 8~9  |  10Itr -  100% | 0.5L | 90%/110%
// 10~11|  Imax  -  Imax | 0.5L | 90%/110%
#define MAX_VOLCORR_OFFSET		5	//��ѹӰ����OFFSETУ������ 1.0
#define MAX_VOLCORR_RGPHS		6	//��ѹӰ����RGPHSУ������ 0.5L
#define MAX_VOLCORR_STEP		( MAX_VOLCORR_OFFSET + MAX_VOLCORR_RGPHS ) //��ѹӰ�����ܵ���

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

typedef struct TRemoteValue_t
{
	TFourByteUnion   U;				//��ѹxxxxxxx.x
	TFourByteUnion   I[2];			//����xxxxx.xxx lL N
	TFourByteUnion   P;				//�й�����xxxx.xxxx
	TFourByteUnion   S;				//���ڹ���xxxx.xxxx
	TFourByteUnion   Cos;			//��������xxxxx.xxx
	TFourByteUnion   Pav;			//һ����ƽ������xxxx.xxxx
	TFourByteUnion   Freq;			//����Ƶ��
}TRemoteValue;

typedef struct TSampleAdjustReg_t
{
	TTwoByteUnion 	GP1;			//50H L���й�����У��
	TTwoByteUnion 	GQ1;			//51H L���޹�����У��
	TTwoByteUnion 	GS1;			//52H L�����ڹ���У��
	TTwoByteUnion 	GP2;			//54H N���й�����У��
	TTwoByteUnion 	RQPhsCal;		//58H �޹���λУ��
	TTwoByteUnion 	RADCCON;		//59H ADCͨ������ѡ��
	TTwoByteUnion 	RPStart;		//5fH �𶯹��ʼĴ���
	TTwoByteUnion 	RHFConst;		//61H �������Ƶ��
	TTwoByteUnion 	RDECShift;		//64H �Ʋ�������λУ��
	TTwoByteUnion 	RP1OFFSET;		//65H L���й�����ƫ��У��	
	TTwoByteUnion 	RI1RMSOFFSET;	//69H L����Чֵ����
	TTwoByteUnion 	RI2RMSOFFSET;	//6aH N����Чֵ����
	TTwoByteUnion 	RGPhs1;			//6dH L����λУ��
	TTwoByteUnion 	RGPhs2;			//6eH N����λУ��
	TTwoByteUnion 	RP1OFFSETL;		//76H L���й�����ƫ��У�����ֽ�
	DWORD 			CRC32;			//4�ֽ�У��
}TSampleAdjustReg;

typedef struct TSampleCtrlReg
{
	TTwoByteUnion 	EMUCFG;			    //40H EMU���üĴ���
	TTwoByteUnion 	FreqCFG;		    //41H ʱ�����üĴ���
	TTwoByteUnion 	ModuleEn;		    //42H ģʽ���ƼĴ���
	TTwoByteUnion 	ANAEN;		        //43H ADC���ؼĴ���
	TTwoByteUnion 	IOCFG;		        //45H ����������üĴ���
	TTwoByteUnion 	ANACON;		        //72H ģ����ƼĴ���
	TTwoByteUnion 	MODECFG;		    //75H MODE���üĴ���	
	WORD            FrequencyFlag;		//Ƶ��ѡ��Ĵ���	
	DWORD 			CRC32;			    //4�ֽ�У��
}TSampleCtrlReg;


typedef struct TSampleCorr_t
{
	TFourByteUnion	UGain;		//��ѹϵ��
	TFourByteUnion	I1Gain;		//���ߵ���ϵ��
	TFourByteUnion  I2Gain;		//���ߵ���ϵ��
	TFourByteUnion  P1Gain;		//���߹���ϵ��
	TFourByteUnion  P2Gain;		//���߹���ϵ�� ��ʱ���� �������߿ɼ�����
	DWORD 			CRC32;		//4�ֽ�У��
}TSampleCorr;

typedef struct TSampleVolCorr_t
{
	TTwoByteUnion 	RP1OFFSET[MAX_VOLCORR_OFFSET];      //65H L���й�����ƫ��У��
	TTwoByteUnion 	RP1OFFSETL[MAX_VOLCORR_OFFSET];     //76H L���й�����ƫ��У�����ֽ�
	TTwoByteUnion 	RGPhs1[MAX_VOLCORR_RGPHS];          //6dH L����λУ��
	DWORD 			CRC32;				//4�ֽ�У��	
}TSampleVolCorr;
#if(SEL_F415 == YES)
typedef struct TSampleUIK_t
{
	float	Uk[3];	//��ѹϵ��
	float	Ik[3];	//����ϵ��
	DWORD	CRC32;
}TSampleUIK;

typedef struct TTimeTem_t
{
	TRealTimer	Time;	//ʱ��
	SWORD		Tem;	//�¶�
}TTimeTem;
#endif
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern TSampleCorr		SampleCorr;	
extern TSampleAdjustReg	SampleAdjustReg;
extern TSampleVolCorr	SampleVolCorr;
extern TSampleCtrlReg   SampleCtrlReg;
extern TRemoteValue		RemoteValue;



//-----------------------------------------------
// 				��������
//-----------------------------------------------
BOOL Link_ReadSampleReg(BYTE addr, BYTE *pBuf);
BOOL Link_WriteSampleReg(BYTE addr, BYTE *pBuf);
void SampleSpecCmdOp(BYTE Cmd);
void SoftResetSample( void );
BYTE GetVolCorrectLevel( BYTE Mode );
//---------------------------------------------------------------
//��������: Ӳ��λ����оƬ
//
//����:   ��
//
//����ֵ: ��
//
//��ע: Ӳ��λֻ֧��Ӳ��֧�ֵ������ʹ��
//---------------------------------------------------------------
void HardResetSample( void );
//-----------------------------------------------
//��������: д7053D�Ĵ������ٴ����������
//
//����:		��
//����ֵ: 	��
//		    
//��ע: 
//-----------------------------------------------
void Link_OpenSampleUartReg( WORD Len );
#endif//#if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )
#endif//#ifndef __HT7053D_H
