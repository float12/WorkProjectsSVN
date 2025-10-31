//----------------------------------------------------------------------
//Copyright (C) 2003-2016
//������
//��������
//����
//�޸ļ�¼
//----------------------------------------------------------------------
#ifndef __WAVERECORD_H
#define __WAVERECORD_H
//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define SAMPLE_CHIP_NUM                 4//����оƬ��
#define LOOP_CNT                        12//��·��
//���ܡ�����ѡ��
#define SELECT_SEND_WAVE_DATA           YES //ͨ��dma���͵��ನ�����ݸ�4g�Ŀ���


#define HALF_WAVE_SIZE                  384 //ui�������һ���С
#define POINT_PER_CYCLE                 128//ÿ�����ڲ�������
#define SIZE_OF_UIPOINT                 6 // ��ѹ�ӵ�����С

//������ļ���С
#define TEMP_SAVE_CYCLE_NUM             4//�������ݱ���������
#define SINGLE_FILE_FULL_TIME           30//һ���ļ�һֱ�洢ֱ��������ʱ�䣬min
#define WAVE_RECORD_PHASE_NUM           3//����

//֡��ʽѡ��Ͷ���
#define SCHOOL_METER                    0//У԰���ʽ
#define DLT698                          1//698��ʽ

#define FRAME_FORMAT                    DLT698

#if (FRAME_FORMAT == SCHOOL_METER)
#define WAVE_FRAME_SIZE                 782 // У԰����֡����
#define FRAME_HEAD_LEN                  5 // ֡ͷ��Ϣ����
#elif (FRAME_FORMAT == DLT698)
#define WAVE_FRAME_SIZE                 815 // 698����֡����
#define FRAME_HEAD_LEN                  42// 698����֡ͷ��ʵ��������ĳ���
#define FRAME_TYPE_OFFSET               25 //֡�����ֽ��ڱ��ĵ�ƫ�ƣ���ʼ���м䡢������
#endif

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct
{
    //�������ݴ���
    WORD RxCnt;
    BYTE RxBuf[1154];
    DWORD WrongHalfWaveCount;
    WORD HSDCTimer;
    BYTE TopoErrFlag;
    DWORD gCycleNumb;
    DWORD Sec;
    BYTE WAVE_Buffer[TEMP_SAVE_CYCLE_NUM][WAVE_RECORD_PHASE_NUM][WAVE_FRAME_SIZE]; // ��������dma����
    // BYTE BufNum;                                                                                // д��sd���Ļ�������,��0��ʼ
    BYTE WriteToBufCycleCount;       // д��sd�����ܲ�����
    DWORD WriteToBufSkippedHalfWave; // ��һ��ת�浽sd��δ��ɵ��±������������ļ���
    const BYTE SampleChipNo; //��0��ʼ 
} TWaveDataDeal;

typedef enum
{
    eFirstFrame = 1,
    eMiddleFrame,
    eLastFrame,
} eWaveDataFrameType;

typedef enum
{
    eCheckSignAndSendFirstFrame,
    eCheckSignAndSendOtherFrame,
} eSendState;


typedef  enum
{
	eEEProm1 = 0,
	eEEProm2,
	eFlash,
	eRTC,
	eSample1ReadUIP,
	eSample2ReadUIP,
	eSample3ReadUIP,
	eSample4ReadUIP,
}eDeviceStatus;//������״̬


//-----------------------------------------------
//				��������
//-----------------------------------------------
extern TWaveDataDeal WaveDataDeal[4];
extern WORD IsChargingFlag;
extern BYTE IsRecWaveData;
extern BYTE StartChargeFlag[LOOP_CNT];
extern BYTE gAllowSendWaveData;
extern TChargePara gChargePara;//���γ������������ʱ�䣬����
extern DWORD DeviceStatus;//������״̬
extern TSampleUIK UIFactorArr[SAMPLE_CHIP_NUM];
//-----------------------------------------------
// 				��������
//-----------------------------------------------
//
//-----------------------------------------------
void toggleTestPin(void);
void CheckAndSendWaveData(TWaveDataDeal *WaveDataDeal);
void api_PowerUpWaveTxRx(void);
void api_CheckWaveParaAndUIK(void);
#endif // #ifndef __WAVERECORD_H
