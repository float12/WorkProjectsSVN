//----------------------------------------------------------------------
//Copyright (C) 2003-2024 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	�����
//��������	2024.7.1
//����		����ʶ��ģ����ͷ�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#ifndef _TOPOAPP_H_
#define _TOPOAPP_H_
#if( SEL_TOPOLOGY == YES )
#include <stdint.h>

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define TOPO_PHASE  3  				//��𣬵���Ϊ1������Ϊ3

//�����ƶ�����ɾ��
#ifdef _TOPOAPP_GLOBALS
	#define TOPOAPP_EXT
#else
	#define TOPOAPP_EXT extern
#endif
#define TOPOFUN_DISABLE 0    //"�ر�����ʶ��"
#define TOPOFUN_ENABLE  0xaa //"��������ʶ��"

#define TP_OS  					3  							//���մ������������N_Coh * N_NonCoh* N_Coh�����������60
#define TP_FRAME_BIT         	16                             //"����������BITλ��"
#define Tp_FRAME_RmsBit1		8		/*"���������Чֵ����λ��"*/
#define Tp_FRAME_RmsBit2		9		/*"���������Чֵ����λ��"*/
#define Tp_FRAME_RmsBit3		10		/*"���������Чֵ����λ��"*/
#define Tp_FRAME_RmsBitNum		3		/*"���������Чֵ����λ��"*/
#define TP_NOISEDETLEN       	0                               // zhupdebug(1 << Tp_Noise_N)//"�����׳���"
#define TP_FRAME_LEN        	(TP_FRAME_BIT * TP_OS)          //"���������������"
#define TP_FIFO_LEN         	(TP_NOISEDETLEN + TP_FRAME_LEN) //"FIFO ����"
#define TP_ERRBITTHREDEF     	1   //"Ĭ���������BITλ��"
#define TP_THRESCHMIDT       	0   // zhupdebug 10//"Ĭ����ֵʩ����ֵ"
#define TP_THREHIGHLOW       	5   // �ߵ�BIT��ƽ��ֵ---�����ʵ��Ӧ�õ���

//Ƶ�ʵ���
#define TOPFQ_ZONE625_NUM  12  
#define TOPFQ_ZONE833_NUM  12 

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	TOPO_BIT_IDLE = 0,   // δ���յ�1
	TOPO_BIT_REV = 1,    // ���յ�1��bit�����ڽ���ʣ��֡
} defTopoBitStatus;

typedef enum
{
	TOPO_Dynamic_Thrd = 0,   // ��̬����
	TOPO_Fixed_Thrd = 1,     // �̶�����
} defTopoThread;

typedef enum
{
	FIFO_NotFull = 0, //"����û�������"
	FIFO_Full = 1,    //"���������"
} eFifoFullFlag_TypeDef;

typedef enum
{
	Tp_NoiseOft_No = 0,      // δ�������1������
	Tp_NoiseOft_Ready = 1,   // �Ѳ�������
} defTopoNoiseFlag_TypeDef;

typedef enum 
{
	TOPO_Slc_Refsh = 0x1,
	TOPO_Slc_WAIT = 0x0,		
}defTopoSliceRefsh;

typedef enum 
{
	Topo_Check_625hz = 0,		
	Topo_Check_833hz = 1,	
	Topo_Check_666hz = 2, 		//�������ݲ�֧��
	Topo_Check_1333hz = 3,		//�������ݲ�֧��
}defTopoCheckFreq;

//"TOPO����"
typedef struct
{
	uint8_t FunSwitch; //"����ʶ��ʹ�ܱ�־"

	uint8_t Info[4]; //"�����루Ĭ������Ϊ��0xaae9)"
	int8_t FrameSeq[TP_FRAME_BIT];
	uint8_t Info_1BitNum; //"������1����"
	uint8_t Info_0BitNum; //"������0����"

	uint8_t ThreSchmidt;      //"��ֵʩ��������"
	uint32_t SynDetThreCoef;  //"ͬ����ֵ��������"
	uint32_t ErrBitThre;      //"�ݴ�����"
	uint32_t DemuSymDeltaLen; //"�������"
	uint32_t ThreHighLow;     //"�ߵ͵�ƽ��ֵ"
} sTopoPara_TypeDef;

//"TOPO״̬"
typedef struct
{
	uint16_t Vs_Fifo_Wait; //"�ȴ�����ֵ��ȥ����ֹ������״���"

	uint16_t Vs_Fifo_Front;       //��ͷ"
	uint16_t Vs_Fifo_Rear;        //��β,�µ����ݽ����β"
	uint16_t Vs_Fifo_Len;         //��ǰ���г���"
	int32_t Vs_Fifo[TP_FIFO_LEN]; //"��Чֵ����"
	int32_t Vs_Fifo_783[TP_FIFO_LEN]; /*"783HZ��Чֵ����"*/
	int32_t Vs_Fifo_883[TP_FIFO_LEN]; /*"883HZ��Чֵ����"*/	
	eFifoFullFlag_TypeDef FifoFullFlag;    //"���������"
	defTopoNoiseFlag_TypeDef NoiseDetFlag; //"���������Ƿ����ɵı�־"

	int32_t NoiseDet_vsSum; //"�����Чֵʸ����"
	int32_t NoiseDet_vs;    //"���"

	int32_t VsSum; //"�������źŻ���"
	int32_t Vs783Sum;	/*"�������źŻ���"*/
	int32_t Vs883Sum;	/*"�������źŻ���"*/	

	int32_t VsDemuThre;  //"�ж���ֵ"
	int32_t VsDemuThre1; //"�ж���ֵ"

	int32_t PeakSynDet;    //"��ֵ"
	int32_t FeatSigDetOut; //"����������Чֵ"

	uint16_t FrameDet;  //"��⵽�������ź�"
	uint16_t ErrBitNum; //"��Ŀ��ֵ���"
} sTopoStatus_TypeDef;

//-----------------------------------------------
//				��������
//-----------------------------------------------
TOPOAPP_EXT uint8_t Topo_Result_Num[TOPO_PHASE];   // �¼���¼����ǰ���)

TOPOAPP_EXT float FreqLast;   // ��1��Ƶ��

extern uint8_t FreqSinCosNum;//=6;  			//��ǰ���������ڵ�Ƶ����
extern uint8_t FreqBpfNum;//=6;	 				//��ǰ�˲����������ڵ�Ƶ����

extern defTopoCheckFreq TopAcv_Topo_Freq;

extern uint32_t Tp_vs783[TOPO_PHASE];			//"N_NonCoh��ƽ��ֵ"
extern uint32_t Tp_vs883[TOPO_PHASE];
extern uint32_t Tp_vs[TOPO_PHASE];			//"883 783Ƶ��ƽ��ֵ"

extern const float FreqZone_625hz[TOPFQ_ZONE625_NUM-1];
extern const float FreqZone_833hz[TOPFQ_ZONE833_NUM-1];
//-----------------------------------------------
// 				��������
//-----------------------------------------------




#endif //#if( SEL_TOPOLOGY == YES )
#endif//#ifndef _TOPOAPP_H_
