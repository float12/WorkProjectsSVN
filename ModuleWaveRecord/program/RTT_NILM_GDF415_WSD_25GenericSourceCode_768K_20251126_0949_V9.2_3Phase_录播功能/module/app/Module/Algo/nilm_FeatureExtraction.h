#ifndef __ALGORITHM_EXTRACTION_H__
#define __ALGORITHM_EXTRACTION_H__
//#include "arm_std_f32.h"
//#include "arm_rms_f32.h"
#include "arm_math.h"

//#define WAVELEN 128 //一个周波的点数
//#define WAVELEN4 512
//#define WAVELEN8 1024
#ifndef PI
#define PI 3.14159265358979f
#endif

#define TMPLEN         60              // featurestmp长度
#define TMPLEN2        120             // featurestmp长度×short
#define TMPLEN4        240             // featurestmp长度×float
#define FROZEN_CYCLE   15              //冻结周期，单位分钟
#define FROZEN_CYCLE_s 900             //冻结周期秒数
#define BLENAME        "ST_Device-16"  // 4G BLENAME
#pragma pack(1)
struct features_typeDef                // 15min特征值
{
    short P[FROZEN_CYCLE_s];
    short Q[FROZEN_CYCLE_s];

    short D[FROZEN_CYCLE_s];
    short Pmax[FROZEN_CYCLE_s];
    short Pmin[FROZEN_CYCLE_s];
    short ICF[FROZEN_CYCLE_s];

    short Praw[FROZEN_CYCLE_s];
    // float Praw[900];
    float i50[FROZEN_CYCLE_s / 2];
    float i150[FROZEN_CYCLE_s / 2];
    float i250[FROZEN_CYCLE_s / 2];
    float phi50[FROZEN_CYCLE_s / 2];
    float phi150[FROZEN_CYCLE_s / 2];
    float phi250[FROZEN_CYCLE_s / 2];
    short Qfft[FROZEN_CYCLE_s];
};

struct featurestmp_typeDef  // 15min特征值
{
    short P[TMPLEN];
    short Praw[TMPLEN];
    short Q[TMPLEN];
    short Qfft[TMPLEN];
    short D[TMPLEN];
    short Pmax[TMPLEN];
    short Pmin[TMPLEN];
    short ICF[TMPLEN];
    // float Praw[900];
    float i50[TMPLEN / 2];
    float i150[TMPLEN / 2];
    float i250[TMPLEN / 2];
    float phi50[TMPLEN / 2];
    float phi150[TMPLEN / 2];
    float phi250[TMPLEN / 2];
};

struct feature_typeDef  // 1个波形特征值
{
    float P;     //Praw *normk *normk
    float Praw;  //直接通过u i 计算
    float Q;     //Qfft+D
    float Qfft;	 //狭义无功功率
    float D;	 //畸变功率
    short Pmin;
    float ICF;   //电流不对称度
    short Pmax;
    float i50;   //电流谐波幅值
    float i150;
    float i250;
    float phi50; //基波相位（电流相对于电压）
    float phi150;
    float phi250;
};

struct result_typeDef  //存储输出结果的结构体
{
	BYTE	device[RESULT_NUM];  //设备名称
	SWORD 	P[RESULT_NUM];       //设备功率
	float 	Pused[RESULT_NUM];   //设备功耗
	float 	Pdiff[RESULT_NUM];   //设备开启时刻有功功率
	float 	Ddiff[RESULT_NUM];   //设备开启时刻畸变功率
	BYTE 	Ident[RESULT_NUM];    //设备的标识序号，从0开始， 最大255，用来区分不同的设备
};

struct lastfeature_typeDef  // 1个波形特征值
{
    float                 P;
    float                 Praw;
    float                 Psteady;
    float                 Prawsteady;
    float                 Q;
    float                 Qfft;
    float                 D;
    short                 Pmax;
    short                 Pmin;
    float                 ICF;
    float                 i50;
    float                 i150;
    float                 i250;
    float                 phi50;
    float                 phi150;
    float                 phi250;
    uint8_t                    flag;
    short                 nextPmax;
    struct result_typeDef lastresult;
};

struct steady_typeDef  //差分稳态特征值
{
    short P;
    short Q;
    short Qfft;
    short D;
    short Pmax;
    short Pmin;
    short ICF;
    short Praw;
    float Pused;
    short Plast;
    float Pstd;
    float hdi3;
    float hdi5;
    float Pslope; //斜率
    uint8_t    risetime;
    uint8_t    specialswitch;
    short Ptrans;
    uint16_t   dur;
    uint8_t    notslowly;
    float Pslopeold;
};

struct cluster_typeDef {
    uint16_t   t[FROZEN_CYCLE_s];
    short P;
    short Q;
    short Qfft;
    short D;
    short Psteady;
    short Praw;
    short Prawsteady;
    uint8_t    switch_n;
    uint8_t    flag;  // Bit 0:slowly(是缓变类‘1’）,Bit1：switch启停，Bit2：Pslope>0.7 斜率大于0.7, Bit3:当该cluster进行了设备关闭时，用来标记该cluster是否已正确匹配到某设备的关闭
    uint16_t   loc;
    short pair;  //记switch启类的配对基底
    float Pstd;
};
#pragma pack()
extern struct feature_typeDef feature[MAX_PHASE];     //存储0.02s窗口数据的累加,extraction.c
extern struct features_typeDef features[MAX_PHASE];  //存储1s窗口数据，长度900
extern struct featurestmp_typeDef featurestmp[MAX_PHASE];  //声明数据缓存float类型,用于存储15min计算期间生成的1s特征值；

struct feature_typeDef cal_features(float *u, float *i, uint16_t WAVELEN);
struct feature_typeDef sum_features(struct feature_typeDef feature1, struct feature_typeDef feature2);
void                   collect_features(int n, struct features_typeDef *feats, struct feature_typeDef *feature1);
void                   collect_featurestmp(int n, struct featurestmp_typeDef *featurestmp, struct feature_typeDef *feature1);
void                   getfromtmp(struct features_typeDef *feats, struct featurestmp_typeDef *featurestmp);

float active_power(float *u, float *i, uint16_t WAVELEN);
float reactive_power(float *U, float *I, uint16_t WAVELEN, struct feature_typeDef *feature1, float normk);
float distort_power(float *u, float *i, float P, float Q, uint16_t WAVELEN);

short max_short(short a, short b);
short min_short(short a, short b);
float max_float(float a, float b);
float min_float(float a, float b);
uint8_t    min_u8(uint8_t a, uint8_t b);
uint8_t    max_u8(uint8_t a, uint8_t b);
float arr_mean(float *a, uint16_t start, uint16_t end);
float arr_mean_short(short *a, uint16_t start, uint16_t end);
uint8_t    arr_max_u8(uint8_t a[FROZEN_CYCLE_s], uint16_t wtlength);
short arr_max_short(short *a, uint16_t len);
short arr_min_short(short *a, uint16_t len);
float arr_max_float(float *a, uint16_t len);
float arr_min_float(float *a, uint16_t len);
short arr_abs_sum_short(short *a, uint16_t start, uint16_t end);
short inarr_u8(uint8_t *arr, uint8_t a, uint8_t n);
uint8_t    inarr_u8_count(uint8_t *arr, uint8_t a, uint8_t n);
short abs_short(short a);

float arr_slope_short(short *P, uint16_t *t, uint16_t N);
void  cluster_Pstd(float *i50, short *P, short *Praw, short *Q, short *Qfft, short *D,
                   struct cluster_typeDef *cluster, uint8_t added0);
void  vector_sub(float A1, float A2, float theta1, float theta2, float *Sub);
void  FeatureInit( void );

#endif //__ALGORITHM_EXTRACTION_H__
