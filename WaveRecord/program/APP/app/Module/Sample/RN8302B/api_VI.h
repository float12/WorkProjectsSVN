//----------------------------------------------------------------------
//Copyright (C) 2003-2025 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	lishicai
//创建日期	2025.01.22
//描述		__API_V_I_H头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __API_V_I_H
#define __API_V_I_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct TIResult_t
{
	float max_I;
	float itc;
} TIResult;

typedef struct TVIarlpa_t
{
	float ar;
	float lpa;
} TVIarlpa;

typedef struct TValueIndex_t
{
	float value;
	BYTE index;
} TValueIndex;

typedef struct TVISdPed_t
{
	float vi_SD;
	float ped;
} TVISdPed;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//函数功能: V_I轨迹特征---电流峰值、itc电流跨度
//
//参数:     float* input_V  周波电压数据
//         float* input_I   周波电流数据
//          Len[in]         数据长度
//          float max_I、itc      电流峰值、itc电流跨度
//
//返回值: 特征序号1、2---
//备注：电流峰值、电流跨度----输入数据不需要归一化处理
//-----------------------------------------------
TIResult GetVI_IMAX_ITC(float* input_I,BYTE len);
//-----------------------------------------------
//函数功能: V_I轨迹特征---ar面积
//
//参数:     float* input_V  周波电压数据
//         float* input_I   周波电流数据
//          Len[in]         数据长度
// 
//
//返回值: 特征序号3、4
//备注：ar面积--—VI轨迹围成的交叉多边形面积，lpa循环方向
//-----------------------------------------------
TVIarlpa GetVI_AR_LPA(float* input_V,float* input_I,BYTE len);
//-----------------------------------------------
//函数功能: V_I轨迹特征---Loop_Length轨迹长度
//
//参数:     float* input_V  周波电压数据
//         float* input_I   周波电流数据
//          Len[in]         数据长度
//          float loop_length     轨迹长度
//
//返回值: 特征序号5
//备注:Loop_Length轨迹长度
//-----------------------------------------------
float GetVI_Loop_Length(float* input_V,float* input_I,BYTE len);
//-----------------------------------------------
//函数功能: V_I轨迹特征---sh整体电流数据标准差
//
//参数:     float* input_V  周波电压数据
//         float* input_I   周波电流数据
//          Len[in]         数据长度
//          float sh      	电流数据标准差
//
//返回值: 特征序号6
//备注:sh = std(inputI) ---整体电流数据标准差
//-----------------------------------------------
float GetVI_SH(float* input_V,float* input_I,BYTE len);
//-----------------------------------------------
//函数功能: V_I轨迹特征---yd中轴截距
//
//参数:     float* input_V  周波电压数据
//         float* input_I   周波电流数据
//          Len[in]         数据长度
//          float yd      yd=A线、B线中轴两个交电流点之间的距离
//
//返回值: 特征序号7
//备注:Euclidean distance between peak voltage and peak current.
//-----------------------------------------------
float GetVI_YD(float* input_V,float* input_I,BYTE len);
//-----------------------------------------------
//函数功能: V_I轨迹特征----D瞬时导纳变化
//
//参数:     float* input_V  周波电压数据
//         float* input_I   周波电流数据
//          Len[in]         数据长度
//          float D        输出数据
//
//返回值: 特征序号8
//-----------------------------------------------
float GetVI_D(float* input_V,float* input_I,BYTE len);
//-----------------------------------------------
//函数功能: V_I轨迹特征----SD最大/最小点距离、PED
//参数:     float* input_V  周波电压数据
//         float* input_I   周波电流数据
//          Len[in]         数据长度
//          float vi_SD      输出最大/最小点距离、峰值电压与峰值电流所在点的欧几里得距离
//
//返回值: 特征序号9、10
//-----------------------------------------------
TVISdPed GetVI_SD_PED(float* input_V,float* input_I,BYTE len);
#endif //#ifndef __API_V_I_H
