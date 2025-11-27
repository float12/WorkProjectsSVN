#include <stdlib.h>
#include <stdio.h>
#include "math.h"
#include "AllHead.h"

#define ki50  -0.031415926535898f
#define ki150 -0.094247779607694f
#define PI2   6.283185307179586f

struct feature_typeDef     feature[MAX_PHASE];            //存储0.02s窗口数据的累加
struct features_typeDef    features[MAX_PHASE];           //存储1s窗口数据，长度900
struct featurestmp_typeDef featurestmp[MAX_PHASE] = {0};  //存储1s窗口数据，长度900
float                      maxstd      = 0.0;

//--------------------------------------------------
//功能描述:  特征初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  FeatureInit( void )
{
	memset(&feature,0,sizeof(feature));
	memset(&features,0,sizeof(features));
	memset(&featurestmp,0,sizeof(featurestmp));
	maxstd = 0;
}
/*****************************************************************************
 函 数 名  : Cal_features
 功能描述  : 计算特征值   一个周波的特征值
 输入参数  : u，i
 输出参数  : features
 返 回 值  :
*****************************************************************************/
struct feature_typeDef cal_features(float *u, float *i, uint16_t WAVELEN)
{
	float normk;
	float rms1, rms2;
	float imax, imin;
	//    short i_short[WAVELEN];
	//    for (uint8_t i = 0; i < WAVELEN; i++) {
	//        i_short[i] *= 1000;
	//    }

	imin = arr_min_float(i, WAVELEN); //电流最小值
	imax = arr_max_float(i, WAVELEN); //电流最大值

	arm_rms_f32(u, WAVELEN, &rms1);	//电压有效值
	arm_rms_f32(i, WAVELEN, &rms2); //电流有效值

	if (rms1 > 190) 
	{
		normk = 220 / rms1;  //将结果赋给指针
	} else 
	{
		normk = 1;
	}

	struct feature_typeDef feature1;
	//通过u i计算原始值
	feature1.Praw = active_power(u, i, WAVELEN); //有功
	feature1.Qfft = reactive_power(u, i, WAVELEN, &feature1, normk);
	feature1.D    = distort_power(u, i, feature1.Praw, feature1.Qfft, WAVELEN) * normk * normk / 50;
	feature1.Pmax = (short)(feature1.Praw * normk * normk);
	feature1.Pmin = (short)(feature1.Praw * normk * normk);

	//特征/50，用于计算50个周波的平均值
	// rt_kprintf("**Pmin:%d,Pmax:%d\n",feature1.Pmin,feature1.Pmax);
	feature1.Praw = feature1.Praw / 50;     //每个周波取1/50,50个周波加起来就是1s
	feature1.P    = feature1.Praw * normk * normk;  //电阻类功率归一化 相对于220V的功率
	feature1.Qfft = feature1.Qfft * normk * normk / 50;
	feature1.Q    = sqrt(feature1.Qfft * feature1.Qfft + feature1.D * feature1.D);

	//计算电流不对称度
	// feature1.ICF = ((fabs(imax+imin)/rms2))/50;
	if ((fabs(imax + imin) / rms2) > 0.35 && feature1.P * 50 > 50) 
	{
		// rt_kprintf("ICF:%f,P:%f\n",feature1.ICF,feature1.P*50);
		feature1.ICF = 1.0;  // ICF大于0.1的次数   
	} 
	else 
	{
		feature1.ICF = 0.0;
	}

	return feature1;
}

/*****************************************************************************
 函 数 名  : Sum_features
 功能描述  : 将0.02s特征值累加起来等待1s求平均
 输入参数  : feature1（旧）,feature2（新）
 输出参数  : features
 返 回 值  :
*****************************************************************************/
struct feature_typeDef sum_features(struct feature_typeDef feature1, struct feature_typeDef feature2)
{
    feature1.P += feature2.P;
    feature1.Praw += feature2.Praw;
    feature1.Q += feature2.Q;
    feature1.Qfft += feature2.Qfft;
    feature1.D += feature2.D;
    feature1.Pmax = max_short(feature1.Pmax, feature2.Pmax);
    // rt_kprintf("Pmin:%d,pmin:%d\n",feature1.Pmin,feature2.Pmin);
    feature1.Pmin = min_short(feature1.Pmin, feature2.Pmin);
    // rt_kprintf("**Pmin:%d\n",feature1.Pmin);

    feature1.ICF += feature2.ICF;

    feature1.i50 += feature2.i50;
    feature1.i150 += feature2.i150;
    feature1.i250 += feature2.i250;
    feature1.phi50 += feature2.phi50;
    feature1.phi150 += feature2.phi150;
    feature1.phi250 += feature2.phi250;
    return feature1;
}

/*****************************************************************************
 函 数 名  : Collect_features
 功能描述  : 将1s特征值收集起来等待15min
 输入参数  : feature,n
 输出参数  : features
 返 回 值  :
*****************************************************************************/
void collect_features(int n, struct features_typeDef *feats, struct feature_typeDef *feature1)
{
	char temp[300];

	feats->P[(n + 1) / 50 - 1]    = (short)feature1->P;
	feats->Q[(n + 1) / 50 - 1]    = (short)feature1->Q;
	feats->Qfft[(n + 1) / 50 - 1] = (short)feature1->Qfft;
	feats->D[(n + 1) / 50 - 1]    = (short)feature1->D;
	feats->Pmax[(n + 1) / 50 - 1] = feature1->Pmax;
	feats->Pmin[(n + 1) / 50 - 1] = feature1->Pmin;
	feats->ICF[(n + 1) / 50 - 1]           = (short)(feature1->ICF);
	feats->Praw[(n + 1) / 50 - 1]          = (short)feature1->Praw;
	feats->i50[((n + 1) / 50 - 1) >> 1]    = feature1->i50;
	feats->i150[((n + 1) / 50 - 1) >> 1]   = feature1->i150;
	feats->i250[((n + 1) / 50 - 1) >> 1]   = feature1->i250;
	feats->phi50[((n + 1) / 50 - 1) >> 1]  = feature1->phi50;
	feats->phi150[((n + 1) / 50 - 1) >> 1] = feature1->phi150;
	feats->phi250[((n + 1) / 50 - 1) >> 1] = feature1->phi250;
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf(" %d-%d-%d %d:%d:%d\r\n", time_15min.year, time_15min.mon, time_15min.day, time_15min.hour,
				time_15min.min, time_15min.sec);
	}
	#endif

	snprintf(temp, 299," P:%d Praw:%d Q:%d D:%d ICF:%d Pmin:%d Pmax:%d i50:%.2f i150:%.2f i250:%.2f phi50:%.2f phi150:%.2f "
		"phi250:%.2f \r\n",
		(short)feature1->P, (short)feature1->Praw, (short)feature1->Q, (short)feature1->D, (short)(feature1->ICF),
		(short)feature1->Pmin, (short)feature1->Pmax, feature1->i50, feature1->i150, feature1->i250, feature1->phi50,
		feature1->phi150, feature1->phi250);

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf(temp);
	}
	#endif
}

/*****************************************************************************
 函 数 名  : Collect_featurestmp
 功能描述  : 将1s特征值收集起来等待转移至features
 输入参数  : feattmp,n
 输出参数  :
 返 回 值  :
*****************************************************************************/
void collect_featurestmp(int n, struct featurestmp_typeDef *feattmp, struct feature_typeDef *feature1)
{
	feattmp->P[(n + 1) / 50 - 1]             = (short)feature1->P;
	feattmp->Q[(n + 1) / 50 - 1]             = (short)feature1->Q;
	feattmp->Qfft[(n + 1) / 50 - 1]          = (short)feature1->Qfft;
	feattmp->D[(n + 1) / 50 - 1]             = (short)feature1->D;
	feattmp->Pmax[(n + 1) / 50 - 1]          = feature1->Pmax;
	feattmp->ICF[(n + 1) / 50 - 1]           = (short)feature1->ICF;
	feattmp->Pmin[(n + 1) / 50 - 1]          = feature1->Pmin;
	feattmp->Praw[(n + 1) / 50 - 1]          = (short)feature1->Praw;
	feattmp->i50[((n + 1) / 50 - 1) >> 1]    = feature1->i50;
	feattmp->i150[((n + 1) / 50 - 1) >> 1]   = feature1->i150;
	feattmp->i250[((n + 1) / 50 - 1) >> 1]   = feature1->i250;
	feattmp->phi50[((n + 1) / 50 - 1) >> 1]  = feature1->phi50;
	feattmp->phi150[((n + 1) / 50 - 1) >> 1] = feature1->phi150;
	feattmp->phi250[((n + 1) / 50 - 1) >> 1] = feature1->phi250;

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
	rt_kprintf("tmpP:%d Q:%d D:%d Pmax:%d \r\n", (short)feature1->P, (short)feature1->Qfft, (short)feature1->D,
				feature1->Pmax);
	}
	#endif
}
/*****************************************************************************
 函 数 名  : getfromtmp
 功能描述  : 从featurestmp中复制特征值
 输入参数  : feats,feattmp
 输出参数  : features
 返 回 值  :
*****************************************************************************/
void getfromtmp(struct features_typeDef *feats, struct featurestmp_typeDef *feattmp)
{
    memcpy(&feats->P, &feattmp->P, TMPLEN2);
    memcpy(&feats->Praw, &feattmp->Praw, TMPLEN2);
    memcpy(&feats->Q, &feattmp->Q, TMPLEN2);
    memcpy(&feats->Qfft, &feattmp->Qfft, TMPLEN2);
    memcpy(&feats->D, &feattmp->D, TMPLEN2);
    memcpy(&feats->Pmax, &feattmp->Pmax, TMPLEN2);
    memcpy(&feats->Pmin, &feattmp->Pmin, TMPLEN2);
    memcpy(&feats->ICF, &feattmp->ICF, TMPLEN2);
    memcpy(&feats->i50, &feattmp->i50, TMPLEN2);
    memcpy(&feats->i150, &feattmp->i150, TMPLEN2);
    memcpy(&feats->i250, &feattmp->i250, TMPLEN2);
    memcpy(&feats->phi50, &feattmp->phi50, TMPLEN2);
    memcpy(&feats->phi150, &feattmp->phi150, TMPLEN2);
    memcpy(&feats->phi250, &feattmp->phi250, TMPLEN2);
}

/*****************************************************************************
 函 数 名  : active_power
 功能描述  : 求有功功率。  平均瞬时功率
 输入参数  : u，i
 输出参数  : P
 返 回 值  :
*****************************************************************************/
float active_power(float *u, float *i, uint16_t WAVELEN)
{
	float sumP = 0;
	float P    = 0;

	for (uint16_t n = 0; n < WAVELEN; n++) 
	{
		sumP += u[n] * i[n];
	}
	P = (sumP / WAVELEN);
	return P;
}

/*****************************************************************************
 函 数 名  : reactive_power
 功能描述  : 求无功功率。
 输入参数  : u，i,P
 输出参数  : Q
 返 回 值  :
*****************************************************************************/
float reactive_power(float *U, float *I, uint16_t WAVELEN, struct feature_typeDef *feature1, float normk)
{
	float Q = 0;
	/*******compare arm*********/
	float                      Uout[128] = {0};
	float                      Iout[128] = {0};
	float *                    UItmp     = malloc(WAVELEN * sizeof(float)); //!!!!!! 需要修改，不用malloc

	arm_rfft_fast_instance_f32 S;
	arm_rfft_fast_init_f32(&S, WAVELEN); //初始化FFT,FFT点数（需为2的幂）。

	memcpy(UItmp, U, WAVELEN * sizeof(float));
	arm_rfft_fast_f32(&S, UItmp, Uout, 0); //计算FFT，输入：UItmp（时域信号），输出：Uout（复数频域数据，格式为[实部0, 虚部0, 实部1, 虚部1, ...]），0：表示正向FFT（时域→频域）
	
	memcpy(UItmp, I, WAVELEN * sizeof(float));
	arm_rfft_fast_f32(&S, UItmp, Iout, 0);
	free(UItmp);			//!!!!!! 需要修改，不用free

	
	/*******compare arm*********/

	//有功功率分量 = Ureal * Iimag - Uimag * Ireal
	//累加到Q：对每个频率分量求和。除以 WAVELEN^2：FFT缩放补偿。乘以 2：因仅使用正频率分量（实数FFT的对称性）。
	for (uint8_t i = 0; i < WAVELEN / 2; i++) 
	{
		Q -= (Uout[2 * i] * Iout[2 * i + 1] - Uout[2 * i + 1] * Iout[2 * i]) / WAVELEN / WAVELEN * 2;
	}

	float I_harm2[6] = {0};

	uint8_t j = 0;
	for (uint8_t i = 1; i < 6; i += 2)
	{
		// 谐波电流幅值
		I_harm2[j]     = sqrt(Iout[2 * i] * Iout[2 * i] + Iout[2 * i + 1] * Iout[2 * i + 1]);

		// 电压与电流的相位差 (φ_u - φ_i)
		I_harm2[j + 3] = atan2(Uout[2 * i + 1], Uout[2 * i]) - atan2(Iout[2 * i + 1], Iout[2 * i]);

		// 相位差归一化到[-π, π]
		if (I_harm2[j + 3] > PI)
			I_harm2[j + 3] -= PI2;
		else if (I_harm2[j + 3] < -PI)
			I_harm2[j + 3] += PI2;
		j++;
	}

	// hdi、phi 基波（50Hz）、3次（150Hz）、5次（250Hz）。
	feature1->i50    = I_harm2[0] / 50 / WAVELEN * 2 * normk;  //一次,除以 50：用于最后累计，相当于提前去平均。除以 WAVELEN：补偿FFT幅值缩放。乘以 2：单边谱补偿。normk：自定义校准系数
	feature1->i150   = I_harm2[1] / 50 / WAVELEN * 2 * normk;  //三次谐波
	feature1->i250   = I_harm2[2] / 50 / WAVELEN * 2 * normk;  // wu次谐波
	feature1->phi50  = I_harm2[3] / 50;
	feature1->phi150 = I_harm2[4] / 50;
	feature1->phi250 = I_harm2[5] / 50;
	return Q;
}

/*****************************************************************************
 函 数 名  : distort_power
 功能描述  : 求畸变功率。
 输入参数  : u，i,P
 输出参数  : Q
 返 回 值  :
*****************************************************************************/
float distort_power(float *u, float *i, float P, float Q, uint16_t WAVELEN)
{
    float S = 0;
    float D = 0;
    float rms1, rms2;
    arm_rms_f32(u, WAVELEN, &rms1);
    arm_rms_f32(i, WAVELEN, &rms2);

    S = rms1 * rms2;

    D = sqrt(S * S - P * P - Q * Q);
    return D;
}
/*****************************************************************************
 函 数 名  : max_short
 功能描述  : 求最大值。
 输入参数  : short a,b
*****************************************************************************/
short max_short(short a, short b)
{
    if (a >= b)
        return a;
    else
        return b;
}
/*****************************************************************************
 函 数 名  : max_short
 功能描述  : 求最大值。
 输入参数  : short a,b
*****************************************************************************/
short min_short(short a, short b)
{
    if (a >= b)
        return b;
    else
        return a;
}
/*****************************************************************************
 函 数 名  : max_float
 功能描述  : 求最大值。
 输入参数  : float a,b
*****************************************************************************/
float max_float(float a, float b)
{
    if (a >= b)
        return a;
    else
        return b;
}
/*****************************************************************************
 函 数 名  : min
 功能描述  : 求最小值。
 输入参数  : float a,b
*****************************************************************************/
float min_float(float a, float b)
{
    if (a < b)
        return a;
    else
        return b;
}

uint8_t min_u8(uint8_t a, uint8_t b)
{
    if (a <= b)
        return a;
    else
        return b;
}
uint8_t max_u8(uint8_t a, uint8_t b)
{
	if (a >= b)
		return a;
	else
		return b;
}

/*****************************************************************************
 函 数 名  : arr_mean
 功能描述  : 求数组平均值。
 输入参数  : 数组a，长度len
*****************************************************************************/
float arr_mean(float *a, uint16_t start, uint16_t end)
{
    float sum = 0, mean = 0;
    for (uint16_t i = start; i < end + 1; i++) {
        sum += a[i];
    }
    mean = sum / (end - start + 1);
    return mean;
}

/*****************************************************************************
 函 数 名  : arr_mean_short
 功能描述  : 求数组平均值。
 输入参数  : 数组a，长度len
*****************************************************************************/
float arr_mean_short(short *a, uint16_t start, uint16_t end)
{
    float sum = 0, mean = 0;
    for (uint16_t i = start; i < end + 1; i++) {
        sum += (float)a[i];
    }
    mean = sum / (end - start + 1);
    return mean;
}

/*****************************************************************************
 函 数 名  : arr_abs_sum_short
 功能描述  : 求数组和。
 输入参数  : 数组a，长度len
*****************************************************************************/
short arr_abs_sum_short(short *a, uint16_t start, uint16_t end)
{
    short sum = 0;
    for (uint16_t i = start; i < end; i++) {
        sum += abs(a[i]);
    }
    return sum;
}

/*****************************************************************************
 函 数 名  : cluster_Pstd
 功能描述  : 求某一类的P平均值、P稳定值/标记缓变类。
 输入参数  : cluster（待求类），P[900]
*****************************************************************************/
void cluster_Pstd(float *i50, short *P, short *Praw, short *Q, short *Qfft, short *D,
                  struct cluster_typeDef *cluster, uint8_t added0)
{
	float Psum = 0, Pmean = 0, Qsum = 0, Qmean = 0, Qfftsum = 0, Qfftmean = 0, Dsum = 0, Dmean = 0,
			Prawsum = 0, Prawmean = 0, std = 0, Psum5 = 0, Prawsum5 = 0, Qsum5 = 0, Qfftsum5 = 0, Dsum5 = 0;
	float *Ptemp     = NULL;
	uint16_t    truestart = 0;
	Ptemp            = (float *)malloc((cluster->loc) << 2);
	if (!Ptemp) 
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("Ptemp out of memory");
		}
		#endif
		return;
	}
	for (int i = 0; i < cluster->loc; i++) 
	{
		Psum += P[cluster->t[i]];
		Prawsum += Praw[cluster->t[i]];
		Ptemp[i] = P[cluster->t[i]];
		Qsum += Q[cluster->t[i]];
		Qfftsum += Qfft[cluster->t[i]];
		Dsum += D[cluster->t[i]];
	}
	Pmean               = Psum / cluster->loc;
	Prawmean            = Prawsum / cluster->loc;
	Qmean               = Qsum / cluster->loc;
	Qfftmean            = Qfftsum / cluster->loc;
	Dmean               = Dsum / cluster->loc;
	cluster->P          = (short)Pmean;
	cluster->Psteady    = (short)Pmean;  //非缓变类稳定功率即为平均功率
	cluster->Praw       = (short)Prawmean;
	cluster->Prawsteady = (short)Prawmean;
	cluster->Q          = (short)Qmean;
	cluster->Qfft       = (short)Qfftmean;
	cluster->D          = (short)Dmean;
	arm_std_f32(Ptemp, cluster->loc, &std);  //计算功率std
	cluster->Pstd = std;
	if (isnan(std)) {
		std           = 0;
		cluster->Pstd = 0;
	} else {
		cluster->Pstd = std;
	}

	if (arr_slope_short(P, cluster->t, cluster->loc) > 0.7)  //计算功率斜率
		cluster->flag |= 0x04;
	if (std > 40) {
		cluster->flag = cluster->flag | 1;  //若std>40，则标记当前类为缓变类
		for (uint8_t j = 1; j < 6; j++)          //缓变类稳定功率用最后5个数的平均计算
		{
			Psum5 += P[cluster->t[cluster->loc - j]];
			Prawsum5 += Praw[cluster->t[cluster->loc - j]];
		}
		cluster->Psteady    = (short)(Psum5 / 5);
		cluster->Prawsteady = (short)(Prawsum5 / 5);
		Psum5               = 0;
		Prawsum5            = 0;
	} else if (added0 == 1) {
		for (uint16_t j = 0; j < cluster->loc; j++)  //用非零数计算
		{
			if (i50[cluster->t[j] >> 1] != 0) {
				Psum5 += P[cluster->t[j]];
				Prawsum5 += Praw[cluster->t[j]];
				Qsum5 += Q[cluster->t[j]];
				Qfftsum5 += Qfft[cluster->t[j]];
				Dsum5 += D[cluster->t[j]];
				truestart++;
			}
		}
		if (truestart == 0)
			truestart = 1;
		cluster->Psteady    = (short)(Psum5 / truestart);
		cluster->Prawsteady = (short)(Prawsum5 / truestart);
		cluster->Q          = (short)(Qsum5 / truestart);
		cluster->Qfft       = (short)(Qfftsum5 / truestart);
		cluster->D          = (short)(Dsum5 / truestart);
	}
	free(Ptemp);
    maxstd = max_float(maxstd, std);
}

/*****************************************************************************
 函 数 名  : arr_max_u8
 功能描述  : 求数组最大值。
 输入参数  : arr
*****************************************************************************/
uint8_t arr_max_u8(uint8_t a[900], uint16_t wtlength)
{
	uint8_t max = a[0];

	for (uint16_t i = 1; i < wtlength; i++) 
	{
		if (a[i] > max) 
		{
			max = a[i];
		}
	}
	return max;
}
/*****************************************************************************
 函 数 名  : arr_max_short
 功能描述  : 求数组最大值。
 输入参数  : arr
*****************************************************************************/
short arr_max_short(short *a, uint16_t len)
{
	short max = a[0];

	for (uint16_t i = 1; i < len; i++) 
	{
		if (a[i] > max) 
		{
			max = a[i];
		}
	}
	return max;
}
/*****************************************************************************
 函 数 名  : arr_min_short
 功能描述  : 求数组最大值。
 输入参数  : arr
*****************************************************************************/
short arr_min_short(short *a, uint16_t len)
{
    short min = a[0];
    for (uint16_t i = 1; i < len; i++) {
        if (a[i] < min) {
            min = a[i];
        }
    }
    return min;
}

/*****************************************************************************
 函 数 名  : arr_max_short
 功能描述  : 求数组最大值。
 输入参数  : arr
*****************************************************************************/
float arr_max_float(float *a, uint16_t len)
{
    float max = a[0];
    for (uint16_t i = 1; i < len; i++) {
        if (a[i] > max) {
            max = a[i];
        }
    }
    return max;
}
/*****************************************************************************
 函 数 名  : arr_min_short
 功能描述  : 求数组最大值。
 输入参数  : arr
*****************************************************************************/
float arr_min_float(float *a, uint16_t len)
{
	float min = a[0];

	for (uint16_t i = 1; i < len; i++) 
	{
		if (a[i] < min) 
		{
			min = a[i];
		}
	}
	return min;
}

/*****************************************************************************
 函 数 名  : abs_short
 功能描述  : 求数组最大值。
 输入参数  : arr
*****************************************************************************/
short abs_short(short a)
{
    if (a >= 0)
        return a;
    else
        return (a * (-1));
}
/*****************************************************************************
 函 数 名  : vector_sub
 功能描述  : 求向量差。
 输入参数  : 向量1模A1、角度theta1，向量2模A2、角度theta2
 输出参数  : 差向量的模SUB[0]和差向量的角度SUB[1]
*****************************************************************************/
void vector_sub(float A1, float A2, float theta1, float theta2, float *Sub)
{
    float imag = 0, real = 0;
    imag   = A1 * sin(theta1) - A2 * sin(theta2);
    real   = A1 * cos(theta1) - A2 * cos(theta2);
    Sub[0] = sqrt(real * real + imag * imag);
    Sub[1] = atan2(imag, real);
}

/*****************************************************************************
 函 数 名  : inarr_u8
 功能描述  : 判断一个数是否在数组中
 输入参数  : 数组arr，数a，数组长度n
 输出参数  :
 返 回 值  : 如果在，返回index，不在返回-1
*****************************************************************************/
short inarr_u8(uint8_t *arr, uint8_t a, uint8_t n)
{
	for (uint8_t i = 0; i < n; i++) 
	{
		if (a == arr[i]) 
		{
			return (short)i;
		}
	}
	return (1*(-1));
}

/*****************************************************************************
 函 数 名  : inarr_u8_count
 功能描述  : 判断一个数在数组中的个数
 输入参数  : 数组arr，数a，数组长度n
 输出参数  :
 返 回 值  : 如果在，返回个数，不在返回0
*****************************************************************************/
uint8_t inarr_u8_count(uint8_t *arr, uint8_t a, uint8_t n)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < n; i++) {
        if (a == arr[i]) {
            count++;
        }
    }
    return count;
}

/*****************************************************************************
 函 数 名  : arr_slope_short
 功能描述  : 求数组斜率（目前用前5个值的平均和最后5个值的平均，之后改成线性回归）。
 输入参数  : arr,lastp,t,n
*****************************************************************************/
float arr_slope_short(short *P, uint16_t *t, uint16_t N)
{
    /*float slope=0;
    short startP=0;
    for (uint8_t i=0;i<5;i++)
    {
      startP+=P[t[i]];
    }
    startP=startP/5;
    slope=(lastP-startP)/n;
    return slope;*/
    uint16_t    i = 0;
    double K = 0, A = 0, B = 0, C = 0, D = 0;
    for (i = 0; i < N; i++) {
        A += t[i] * P[t[i]];
        B += t[i];
        C += P[t[i]];
        D += t[i] * t[i];
    }
    K = (N * A - B * C) / (N * D - B * B);
    // b=C/N-K*B/N;
    return K;
}
