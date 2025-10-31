//----------------------------------------------------------------------
// Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
// ������
// ��������
// ����
// �޸ļ�¼
//----------------------------------------------------------------------
#ifndef __HARMONIC_WAVE_H__
#define __HARMONIC_WAVE_H__

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

// г���������
#define MAX_NP      					150              // ��������ȶ����й����ʣ�150�ܲ�����һ��
#define I_INCONTROL_VALUE 				0.012            // 5A��4%��,�ڿ�60%��Ϊ5A*0.004*60% = 0.012A
#define UPLOIAD_HARM_NUM 				12               // �ϴ���վ��г������
#define UPLOIAD_VI_CHARACTERISTIC_NUM 	10               // �ϴ���վ��������
#define RFFT_NUM 						128              // ���ܲ����ٸ���Ҷ�任�ܲ�����
#define RFFT_DOUBLENUM 					256              // ���ܲ����ٸ���Ҷ�任��������
#define RFFT_HARMNUM 					(1 + 64)         // ���ܲ����ٸ���Ҷ�任г������
#define WAVE_IDATA_SIZE 				5                // Ҫ�õ��ܲ���
#define MAX_RELAY_NUM 					3               // ����·��
#define SAMPLE_NUM 						128              // ���ܲ���������
#define VI_CHARACTERISTIC_NUM 			10              // vi������
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern float Cur_harm_content[MAX_RELAY_NUM][RFFT_HARMNUM]; // ����г������
extern float Cur_harm_mag[MAX_RELAY_NUM][RFFT_HARMNUM];     // ����г����Чֵ
extern float UI_Trait_data[MAX_RELAY_NUM][VI_CHARACTERISTIC_NUM];
extern TSampleUIK UIFactor;
extern BYTE rawData[1152 * 2]; // ���ڴ洢ԭʼ��������
extern BYTE RawDataHalfWaveCnt; // ���ڴ洢ԭʼ�������ݵ�
//-----------------------------------------------
// 				��������
//-----------------------------------------------
void HarmAndVICharacCal_Task(void);

#endif //__HARMONIC_WAVE_H__

