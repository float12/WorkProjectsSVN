//----------------------------------------------------------------------
// Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
// 创建人
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#ifndef __HARMONIC_WAVE_H__
#define __HARMONIC_WAVE_H__

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

// 谐波计算相关
#define MAX_NP      					150              // 计算相对稳定的有功功率，150周波计算一次
#define I_INCONTROL_VALUE 				0.012            // 5A的4%。,内控60%。为5A*0.004*60% = 0.012A
#define UPLOIAD_HARM_NUM 				12               // 上传主站的谐波个数
#define UPLOIAD_VI_CHARACTERISTIC_NUM 	10               // 上传主站的特征数
#define RFFT_NUM 						128              // 单周波快速傅里叶变换周波点数
#define RFFT_DOUBLENUM 					256              // 单周波快速傅里叶变换复数长度
#define RFFT_HARMNUM 					(1 + 64)         // 单周波快速傅里叶变换谐波次数
#define WAVE_IDATA_SIZE 				5                // 要用的周波数
#define MAX_RELAY_NUM 					3               // 最大回路数
#define SAMPLE_NUM 						128              // 单周波采样点数
#define VI_CHARACTERISTIC_NUM 			10              // vi特征数
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern float Cur_harm_content[MAX_RELAY_NUM][RFFT_HARMNUM]; // 电流谐波含量
extern float Cur_harm_mag[MAX_RELAY_NUM][RFFT_HARMNUM];     // 电流谐波有效值
extern float UI_Trait_data[MAX_RELAY_NUM][VI_CHARACTERISTIC_NUM];
extern TSampleUIK UIFactor;
extern BYTE rawData[1152 * 2]; // 用于存储原始波形数据
extern BYTE RawDataHalfWaveCnt; // 用于存储原始波形数据的
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void HarmAndVICharacCal_Task(void);

#endif //__HARMONIC_WAVE_H__

