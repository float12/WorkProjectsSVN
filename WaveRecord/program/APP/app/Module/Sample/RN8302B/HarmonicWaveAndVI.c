//----------------------------------------------------------------------
// Copyright (C)
// 创建人
// 创建日期
// 描述     谐波计算和VI特征计算
// 修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "arm_math.h"
#include "HarmonicWaveAndVI.h"
//-----------------------------------------------
//      本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//      全局使用的变量，常量
//-----------------------------------------------


//谐波计算数据
BYTE rawData[1152 * 2] = {0};
BYTE RawDataHalfWaveCnt = 0;


// 以下是谐波计算有关的变量定义					校园表采样率6400HZ,基波 50HZ,单周波20ms,采样128点
arm_rfft_instance_f32 S;
arm_cfft_radix4_instance_f32 S_CFFT;
// static float pSrc[RFFT_HARMNUM]={0.0};				//实数快速傅里叶变换的输入信号，此处我们用电流数据代替
static float pDst[RFFT_DOUBLENUM] = {0.0};
static float Cur_harm_amplitude[RFFT_HARMNUM] = {0.0};	 // 电流谐波幅值
float Cur_harm_mag[MAX_RELAY_NUM][RFFT_HARMNUM] = {0.0}; // 电流谐波有效值
double total_Cur_harm_mag;
float Cur_harm_content[MAX_RELAY_NUM][RFFT_HARMNUM] = {0.0}; // 电流谐波含量
float Cur_harm_contentk[MAX_RELAY_NUM] = {0.0};				 // 电流谐波含量
float V_data[MAX_PHASE][SAMPLE_NUM];						 // 电压报文解析数据
float I_data[MAX_PHASE][SAMPLE_NUM];						 // 电流报文解析数据
float V_data2[MAX_PHASE][SAMPLE_NUM];						 // VI特征计算所需数据
float I_data2[MAX_PHASE][SAMPLE_NUM];						 // VI特征计算所需数据
float UI_Trait_data[MAX_PHASE][VI_CHARACTERISTIC_NUM];				 // VI特征数据
//-----------------------------------------------
//      本文件使用的变量，常量
//-----------------------------------------------
TSampleUIK UIFactor =
{
		{0.00026546, 0.00026546, 0.00026546},
		{0.00024744, 0.00024744, 0.00024744},
		{0x00000000}
};//默认值
//-----------------------------------------------
//      内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//      函数定义
//-----------------------------------------------

//--------------------------------------------------
// 功能描述:  电压解析
//
// 参数:      *rawdate：原始数据指针， *voltagedatebuff：存储电压数据的指针
//
// 返回值:
//
// 备注:
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
			// voltagedatebuff[ph][i] = (float)(SWORD)(ptemp_v * UIFactor.Uk[ph] * 10) / 10;//可以去零飘

			ptemp_c = (r_date[temp3 + temp1 + 0] << 24) + (r_date[temp3 + temp1 + 1] << 16) + (r_date[temp3 + temp1 + 2] << 8);
			ptemp_c >>= 8;
			currentdatebuff[ph][i] = (float)(ptemp_c * UIFactor.Ik[ph]);
			// currentdatebuff[ph][i] = (float)(SWORD)(ptemp_c * UIFactor.Ik[ph] * 10) / 10;//可以去零飘
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
			// voltagedatebuff[ph][i+SAMPLE_NUM/2] = (float)(SWORD)(ptemp_v * UIFactor.Uk[ph] * 10) / 10;//可以去零飘

			ptemp_c = (r_date[temp3 + temp1 + 0] << 24) + (r_date[temp3 + temp1 + 1] << 16) + (r_date[temp3 + temp1 + 2] << 8);
			ptemp_c >>= 8;
			currentdatebuff[ph][i + SAMPLE_NUM / 2] = (float)(ptemp_c * UIFactor.Ik[ph]);
			// currentdatebuff[ph][i+SAMPLE_NUM/2] = (float)(SWORD)(ptemp_c * UIFactor.Ik[ph] * 10) / 10;//可以去零飘
		}
		// }
	}
	RecTimes += 1;
	if (RecTimes == 2)
	{
		RecTimes = 0; // 两个半波处理完后清零
	}
#endif // #if(MAX_PHASE == 1)
}

//--------------------------------------------------
// 功能描述:  电流谐波计算任务
//
// 参数:		ph:回路		data：一周波的电流数据
//
// 返回值:
//
// 备注:  计算得到
//--------------------------------------------------
void rfft_test(BYTE ph, float *data)
{
	BYTE i, j;
	float32_t fft_out[RFFT_HARMNUM] = {0.0}; // rfft结果输出

	total_Cur_harm_mag = 0.0;

	arm_rfft_init_f32(&S, &S_CFFT, RFFT_NUM, 0, 1);
	arm_rfft_f32(&S, data, pDst);

	// 得到谐波幅值与谐波有效值
	for (j = 0; j < RFFT_HARMNUM; j++)
	{
		arm_sqrt_f32(pDst[2 * j] * pDst[2 * j] + pDst[2 * j + 1] * pDst[2 * j + 1], &fft_out[j]);
		if (j == 0)
		{
			Cur_harm_amplitude[j] = fft_out[j] / 128; // 直流分量需要特殊处理
			Cur_harm_mag[ph][j] = Cur_harm_amplitude[j];
		}
		else
		{
			Cur_harm_amplitude[j] = fft_out[j] / 64; // 交流分量
			Cur_harm_mag[ph][j] = Cur_harm_amplitude[j] / sqrt(2);
		}
	}
	// 得到谐波含量
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
			// 将基波含量放缩到100%，得到比例系数K。
			Cur_harm_contentk[ph] = 1.0 / Cur_harm_content[ph][1];
		}
		Cur_harm_content[ph][i] = Cur_harm_content[ph][i] * Cur_harm_contentk[ph];
	}
}
//--------------------------------------------------
// 功能描述:  归一化float数组
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void NormalizeFloatArray(float *array, DWORD size)
{
	float max = 0; // 初始化为最小浮点值
	float min = 0;  // 初始化为最大浮点值
	WORD i = 0;

	if (array == NULL || size == 0)
	{
		return;
	}
	// 找到数组的最大值和最小值
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
	// 检查是否所有值相等（避免除以零）
	if (max == min)
	{
		for ( i = 0; i < size; i++)
		{
			array[i] = 0.0f; // 将所有元素设置为 0
		}
		return;
	}
	// 归一化数组
	for ( i = 0; i < size; i++)
	{
		array[i] = (array[i] - min) / (max - min);
	}
}

//--------------------------------------------------
// 功能描述:  计算VI特征
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void CalculateVICharac(void)
{
    BYTE i = 0;
    float *pV = NULL, *pI = NULL; 
    TIResult result1;
	TVIarlpa result2;
	TVISdPed resul3;
    memset(&result1, 0, sizeof(TIResult));
	//先计算不需要归一化的特征
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
// 功能描述:  计算谐波和VI特征任务
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void HarmAndVICharacCal_Task(void)
{
	void *smp_p = NULL;
	BYTE i, j, k;
	BYTE buf[128 * 6];

	// if(_get_smp_points(&smp_p))
	// {
	get_voltage_current(rawData, V_data, I_data); // 计算电压，计算两路
	memcpy(&V_data2[0][0],V_data,sizeof(V_data));
	memcpy(&I_data2[0][0],I_data,sizeof(I_data));
	// 单周波计算瞬时量
	// for (i = 0; i < MAX_RELAY_NUM; i++)
	// {
	// 	Get_Vrms((float *)&Vrms[i], i);
	// 	Get_Irms((float *)&Irms[i], i);
	// 	Get_ActP((float *)&ActP[i], i);
	// 	Get_ActQ(i);					// 一直累加无功采样值
	// 	Get_ReaP((float *)&ActQ[i], i); // 传统方法计算无功

	// 	cycles_vdata[i][cycles_p] = Vrms[i];
	// 	cycles_idata[i][cycles_p] = Irms[i];
	// 	cycles_pdata[i][cycles_p] = ActP[i];
	// }
	// 每20周波调用一次谐波计算任务
	// if ((cycles_p) % 20 == 0)
	// {
	for (i = 0; i < MAX_RELAY_NUM; i++)
	{
		if (I_data[i][0] != 0.0)
		{
			rfft_test(i, (float *)&I_data[i][0]); // 谐波计算任务函数
		}
	} 
	CalculateVICharac();
	// }

	// cycles_p++;

	// //每150周波计算处理（主要为计算无功功率服务），MAX_NP不要轻易改动
	// if (cycles_p == MAX_NP)
	// {
	// 	//计算稳定值
	// 	Get_nVrms();		//计算稳定电压
	// 	Get_nIrms();		//计算稳定电流
	// 	Get_ActnP();		//计算稳定有功功率，暂定3s-150周波。
	// 	Get_ActnQ();		//计算稳定无功功率，暂定3s-150周波。
	// 	I_Internal_control();//启动电流门限处理

	// 	cycles_p = 0;
	// }

	// api_SampleTask();		//第一路算法识别任务
	// api_DataProc();			//第二路算法识别任务
	// }
}