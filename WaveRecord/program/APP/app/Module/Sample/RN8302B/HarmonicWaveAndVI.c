//----------------------------------------------------------------------
// Copyright (C)
// ������
// ��������
// ����     г�������VI��������
// �޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "arm_math.h"
#include "HarmonicWaveAndVI.h"
//-----------------------------------------------
//      ���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//      ȫ��ʹ�õı���������
//-----------------------------------------------


//г����������
BYTE rawData[1152 * 2] = {0};
BYTE RawDataHalfWaveCnt = 0;


// ������г�������йصı�������					У԰�������6400HZ,���� 50HZ,���ܲ�20ms,����128��
arm_rfft_instance_f32 S;
arm_cfft_radix4_instance_f32 S_CFFT;
// static float pSrc[RFFT_HARMNUM]={0.0};				//ʵ�����ٸ���Ҷ�任�������źţ��˴������õ������ݴ���
static float pDst[RFFT_DOUBLENUM] = {0.0};
static float Cur_harm_amplitude[RFFT_HARMNUM] = {0.0};	 // ����г����ֵ
float Cur_harm_mag[MAX_RELAY_NUM][RFFT_HARMNUM] = {0.0}; // ����г����Чֵ
double total_Cur_harm_mag;
float Cur_harm_content[MAX_RELAY_NUM][RFFT_HARMNUM] = {0.0}; // ����г������
float Cur_harm_contentk[MAX_RELAY_NUM] = {0.0};				 // ����г������
float V_data[MAX_PHASE][SAMPLE_NUM];						 // ��ѹ���Ľ�������
float I_data[MAX_PHASE][SAMPLE_NUM];						 // �������Ľ�������
float V_data2[MAX_PHASE][SAMPLE_NUM];						 // VI����������������
float I_data2[MAX_PHASE][SAMPLE_NUM];						 // VI����������������
float UI_Trait_data[MAX_PHASE][VI_CHARACTERISTIC_NUM];				 // VI��������
//-----------------------------------------------
//      ���ļ�ʹ�õı���������
//-----------------------------------------------
TSampleUIK UIFactor =
{
		{0.00026546, 0.00026546, 0.00026546},
		{0.00024744, 0.00024744, 0.00024744},
		{0x00000000}
};//Ĭ��ֵ
//-----------------------------------------------
//      �ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//      ��������
//-----------------------------------------------

//--------------------------------------------------
// ��������:  ��ѹ����
//
// ����:      *rawdate��ԭʼ����ָ�룬 *voltagedatebuff���洢��ѹ���ݵ�ָ��
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void get_voltage_current(void *rawdate, float (*voltagedatebuff)[SAMPLE_NUM], float (*currentdatebuff)[SAMPLE_NUM])
{
	SDWORD ptemp_v, ptemp_c;
	BYTE *r_date = NULL, i = 0, ph = 0;
	WORD temp1 = 0, temp2 = 0, temp3 = 0;
	static BYTE RecTimes = 0;

	r_date = (BYTE *)rawdate;
#if (MAX_PHASE == 1)
// pass
#elif (MAX_PHASE == 3)
	for (ph = 0; ph < MAX_PHASE; ph++)
	{
		// if (RecTimes == 0)
		// {
		temp2 = SAMPLE_NUM / 2 * 3 * ph;
		temp3 = SAMPLE_NUM / 2 * 3 * (ph + MAX_PHASE);
		for (i = 0; i < SAMPLE_NUM / 2; i++)
		{
			temp1 = i * 3;
			ptemp_v = (r_date[temp2 + temp1 + 0] << 24) + (r_date[temp2 + temp1 + 1] << 16) + (r_date[temp2 + temp1 + 2] << 8);
			ptemp_v >>= 8;
			voltagedatebuff[ph][i] = (float)(ptemp_v * UIFactor.Uk[ph]);
			// voltagedatebuff[ph][i] = (float)(SWORD)(ptemp_v * UIFactor.Uk[ph] * 10) / 10;//����ȥ��Ʈ

			ptemp_c = (r_date[temp3 + temp1 + 0] << 24) + (r_date[temp3 + temp1 + 1] << 16) + (r_date[temp3 + temp1 + 2] << 8);
			ptemp_c >>= 8;
			currentdatebuff[ph][i] = (float)(ptemp_c * UIFactor.Ik[ph]);
			// currentdatebuff[ph][i] = (float)(SWORD)(ptemp_c * UIFactor.Ik[ph] * 10) / 10;//����ȥ��Ʈ
		}
		// }
		// else
		// {
		temp2 = SAMPLE_NUM / 2 * 3 * ph + 1152;
		temp3 = SAMPLE_NUM / 2 * 3 * (ph + MAX_PHASE) + 1152;
		for (i = 0; i < SAMPLE_NUM / 2; i++)
		{
			temp1 = i * 3;
			ptemp_v = (r_date[temp2 + temp1 + 0] << 24) + (r_date[temp2 + temp1 + 1] << 16) + (r_date[temp2 + temp1 + 2] << 8);
			ptemp_v >>= 8;
			voltagedatebuff[ph][i + SAMPLE_NUM / 2] = (float)(ptemp_v * UIFactor.Uk[ph]);
			// voltagedatebuff[ph][i+SAMPLE_NUM/2] = (float)(SWORD)(ptemp_v * UIFactor.Uk[ph] * 10) / 10;//����ȥ��Ʈ

			ptemp_c = (r_date[temp3 + temp1 + 0] << 24) + (r_date[temp3 + temp1 + 1] << 16) + (r_date[temp3 + temp1 + 2] << 8);
			ptemp_c >>= 8;
			currentdatebuff[ph][i + SAMPLE_NUM / 2] = (float)(ptemp_c * UIFactor.Ik[ph]);
			// currentdatebuff[ph][i+SAMPLE_NUM/2] = (float)(SWORD)(ptemp_c * UIFactor.Ik[ph] * 10) / 10;//����ȥ��Ʈ
		}
		// }
	}
	RecTimes += 1;
	if (RecTimes == 2)
	{
		RecTimes = 0; // �����벨�����������
	}
#endif // #if(MAX_PHASE == 1)
}

//--------------------------------------------------
// ��������:  ����г����������
//
// ����:		ph:��·		data��һ�ܲ��ĵ�������
//
// ����ֵ:
//
// ��ע:  ����õ�
//--------------------------------------------------
void rfft_test(BYTE ph, float *data)
{
	BYTE i, j;
	float32_t fft_out[RFFT_HARMNUM] = {0.0}; // rfft������

	total_Cur_harm_mag = 0.0;

	arm_rfft_init_f32(&S, &S_CFFT, RFFT_NUM, 0, 1);
	arm_rfft_f32(&S, data, pDst);

	// �õ�г����ֵ��г����Чֵ
	for (j = 0; j < RFFT_HARMNUM; j++)
	{
		arm_sqrt_f32(pDst[2 * j] * pDst[2 * j] + pDst[2 * j + 1] * pDst[2 * j + 1], &fft_out[j]);
		if (j == 0)
		{
			Cur_harm_amplitude[j] = fft_out[j] / 128; // ֱ��������Ҫ���⴦��
			Cur_harm_mag[ph][j] = Cur_harm_amplitude[j];
		}
		else
		{
			Cur_harm_amplitude[j] = fft_out[j] / 64; // ��������
			Cur_harm_mag[ph][j] = Cur_harm_amplitude[j] / sqrt(2);
		}
	}
	// �õ�г������
	for (i = 0; i < RFFT_HARMNUM; i++)
	{
		total_Cur_harm_mag = total_Cur_harm_mag + Cur_harm_mag[ph][i] * Cur_harm_mag[ph][i];
	}
	total_Cur_harm_mag = sqrt(total_Cur_harm_mag);
	for (i = 0; i < RFFT_HARMNUM; i++)
	{
		Cur_harm_content[ph][i] = Cur_harm_mag[ph][i] / total_Cur_harm_mag;
	}
	for (i = 1; i < RFFT_HARMNUM; i++)
	{
		if (i == 1)
		{
			// ����������������100%���õ�����ϵ��K��
			Cur_harm_contentk[ph] = 1.0 / Cur_harm_content[ph][1];
		}
		Cur_harm_content[ph][i] = Cur_harm_content[ph][i] * Cur_harm_contentk[ph];
	}
}
//--------------------------------------------------
// ��������:  ��һ��float����
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void NormalizeFloatArray(float *array, DWORD size)
{
	float max = 0; // ��ʼ��Ϊ��С����ֵ
	float min = 0;  // ��ʼ��Ϊ��󸡵�ֵ
	WORD i = 0;

	if (array == NULL || size == 0)
	{
		return;
	}
	// �ҵ���������ֵ����Сֵ
	for ( i = 0; i < size; i++)
	{
		if (array[i] > max)
		{
			max = array[i];
		}
		if (array[i] < min)
		{
			min = array[i];
		}
	}
	// ����Ƿ�����ֵ��ȣ���������㣩
	if (max == min)
	{
		for ( i = 0; i < size; i++)
		{
			array[i] = 0.0f; // ������Ԫ������Ϊ 0
		}
		return;
	}
	// ��һ������
	for ( i = 0; i < size; i++)
	{
		array[i] = (array[i] - min) / (max - min);
	}
}

//--------------------------------------------------
// ��������:  ����VI����
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void CalculateVICharac(void)
{
    BYTE i = 0;
    float *pV = NULL, *pI = NULL; 
    TIResult result1;
	TVIarlpa result2;
	TVISdPed resul3;
    memset(&result1, 0, sizeof(TIResult));
	//�ȼ��㲻��Ҫ��һ��������
    for (i = 0; i < MAX_RELAY_NUM; i++)
    {
        pV = (float *)&V_data2[i][0]; 
        pI = (float *)&I_data2[i][0];

        result1 = GetVI_IMAX_ITC(pI, SAMPLE_NUM);
        UI_Trait_data[i][0] = result1.max_I; 
        UI_Trait_data[i][1] = result1.itc; 

        // 2. GetVI_AR_LPA
		result2 = GetVI_AR_LPA(pV, pI, SAMPLE_NUM);
        UI_Trait_data[i][2] =  result2.ar;
 		UI_Trait_data[i][3] =  result2.lpa;
        // 3. GetVI_LPA
        // UI_Trait_data[i][3] = GetVI_LPA(pV, pI, SAMPLE_NUM);
    }
    for (i = 0; i < MAX_RELAY_NUM; i++)
    {
        pV = (float *)&V_data2[i][0]; // Point to the voltage array
        pI = (float *)&I_data2[i][0]; // Point to the current array

        // NormalizeFloatArray(pV, SAMPLE_NUM);
        // NormalizeFloatArray(pI, SAMPLE_NUM);

        // 4. GetVI_Loop_Length
        UI_Trait_data[i][4] = GetVI_Loop_Length(pV, pI, SAMPLE_NUM);

        // 5. GetVI_SH
        UI_Trait_data[i][5] = GetVI_SH(pV, pI, SAMPLE_NUM);

        // 6. GetVI_YD
        UI_Trait_data[i][6] = GetVI_YD(pV, pI, SAMPLE_NUM);

        // 7. GetVI_D
        UI_Trait_data[i][7] = GetVI_D(pV, pI, SAMPLE_NUM);
        // 8. GetVI_SD_PED
		resul3 = GetVI_SD_PED(pV, pI, SAMPLE_NUM);
		UI_Trait_data[i][8] =resul3.vi_SD;
        UI_Trait_data[i][9] = resul3.ped;
    }
}


//--------------------------------------------------
// ��������:  ����г����VI��������
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void HarmAndVICharacCal_Task(void)
{
	void *smp_p = NULL;
	BYTE i, j, k;
	BYTE buf[128 * 6];

	// if(_get_smp_points(&smp_p))
	// {
	get_voltage_current(rawData, V_data, I_data); // �����ѹ��������·
	memcpy(&V_data2[0][0],V_data,sizeof(V_data));
	memcpy(&I_data2[0][0],I_data,sizeof(I_data));
	// ���ܲ�����˲ʱ��
	// for (i = 0; i < MAX_RELAY_NUM; i++)
	// {
	// 	Get_Vrms((float *)&Vrms[i], i);
	// 	Get_Irms((float *)&Irms[i], i);
	// 	Get_ActP((float *)&ActP[i], i);
	// 	Get_ActQ(i);					// һֱ�ۼ��޹�����ֵ
	// 	Get_ReaP((float *)&ActQ[i], i); // ��ͳ���������޹�

	// 	cycles_vdata[i][cycles_p] = Vrms[i];
	// 	cycles_idata[i][cycles_p] = Irms[i];
	// 	cycles_pdata[i][cycles_p] = ActP[i];
	// }
	// ÿ20�ܲ�����һ��г����������
	// if ((cycles_p) % 20 == 0)
	// {
	for (i = 0; i < MAX_RELAY_NUM; i++)
	{
		if (I_data[i][0] != 0.0)
		{
			rfft_test(i, (float *)&I_data[i][0]); // г������������
		}
	} 
	CalculateVICharac();
	// }

	// cycles_p++;

	// //ÿ150�ܲ����㴦����ҪΪ�����޹����ʷ��񣩣�MAX_NP��Ҫ���׸Ķ�
	// if (cycles_p == MAX_NP)
	// {
	// 	//�����ȶ�ֵ
	// 	Get_nVrms();		//�����ȶ���ѹ
	// 	Get_nIrms();		//�����ȶ�����
	// 	Get_ActnP();		//�����ȶ��й����ʣ��ݶ�3s-150�ܲ���
	// 	Get_ActnQ();		//�����ȶ��޹����ʣ��ݶ�3s-150�ܲ���
	// 	I_Internal_control();//�����������޴���

	// 	cycles_p = 0;
	// }

	// api_SampleTask();		//��һ·�㷨ʶ������
	// api_DataProc();			//�ڶ�·�㷨ʶ������
	// }
}