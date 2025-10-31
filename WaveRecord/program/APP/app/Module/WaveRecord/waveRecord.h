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
//������ѡ��
#define SINGLE_PHASE_BOARD      0
#define THREE_PHASE_BOARD       1
#define HARDWARE_TYPE           SINGLE_PHASE_BOARD 

//���ܡ�����ѡ��
#define SELECT_SEND_WAVE_DATA   0 //ͨ��dma���͵��ನ�����ݸ�4g�Ŀ���
#define SELECT_HARMONY_VI_CAL   0
#define SELECT_4851_SEND_DATA   0//ͨ��485-1�����������ݣ��رս��չ���
#define WAVE_RECORD_PHASE_NUM 	1
//֡��ʽѡ��Ͷ���
#define SCHOOL_METER            0
#define DLT698                  1
#define FRAME_FORMAT            SCHOOL_METER

#if  (FRAME_FORMAT == SCHOOL_METER)
#define WAVE_FRAME_SIZE 		782
#define FRAME_HEAD_LEN 		    5 // ֡ͷ��Ϣ����
#elif (FRAME_FORMAT == DLT698)
#define WAVE_FRAME_SIZE 		815
#define FRAME_HEAD_LEN 		    42 // 698����֡ͷ��ʵ��������ĳ���
#endif
#define HALF_WAVE_SIZE 			384 //ui�������һ���С
#define POINT_PER_CYCLE 		128
#define SIZE_OF_UIPOINT 		6 // ��ѹ�ӵ�����С

//������ļ���С
#define SAVE_TO_SD_CYCLE_NUM 	12
#define BUFF_COUNT 				6 // ����������
#define SINGLE_FILE_FULL_TIME   30 //һ���ļ�һֱ�洢ֱ��������ʱ�䣬min
#define MAX_FILE_SIZE 			WAVE_FRAME_SIZE*WAVE_RECORD_PHASE_NUM*50*60*SINGLE_FILE_FULL_TIME // �ļ�����С
//����
#if (HARDWARE_TYPE == SINGLE_PHASE_BOARD)
#define PRESSED_LEVEL           1 //�������µ�ƽ״̬
#elif (HARDWARE_TYPE == THREE_PHASE_BOARD)
#define PRESSED_LEVEL           0 //�������µ�ƽ״̬
#endif
#define KEY_SMOOTH_TIME			10 //��ֹ������δ��ȫ���£���ʱ��⵽�ĵ�ƽ�Ѿ��ǰ��µ�ƽ�����ɿ�����Ϊ���¡�ʱ�� 10*10ms�� 

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	PRESSED_STATUS,
	UNPRESSED_STATUS,
	INVALAID_STATUS,
} KeyStatus;

#pragma pack(1)
typedef struct
{
	WORD 	RxCnt;
	WORD 	CycDot;
	BYTE		RxBuf[1154];
}sTopoWaveTypedef;
#pragma pack()
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern BYTE WAVE_Buffer[BUFF_COUNT][SAVE_TO_SD_CYCLE_NUM][WAVE_RECORD_PHASE_NUM][WAVE_FRAME_SIZE]; 
extern BYTE SWAVE_BufferFullFlag[BUFF_COUNT]; 
extern BYTE CreateNewFileFlag; 
extern BYTE TFKeySmoothTimer;
extern BYTE isPressed;
extern BYTE ReverAddr[6];
extern sTopoWaveTypedef sTopoWave;

extern DWORD OneBufFullCnt;
extern DWORD TwoBufFullCnt;
extern DWORD ThreeBufFullCnt;
extern DWORD CycleNumb; // ȫ�����ڼ���
extern DWORD WriteToBufSkippedHalfWave;
extern DWORD WrongHalfWaveCount;
extern DWORD WrongHalfWaveNo;


//-----------------------------------------------
// 				��������
//-----------------------------------------------
//
//-----------------------------------------------
void toggleTestPin(void);
void api_PowerDownWaveRecord(void);
void api_WAVE_Task(void);
BYTE WaveRecordInit(void);
#endif //#ifndef __WAVERECORD_H
