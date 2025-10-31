//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
//创建人	lishicai
//创建日期	2025.01.18
//描述		计算周波数据的VI轨迹特征,暂时不进行数据归一化处理
//修改记录：电流峰值、itc电流跨度、ar面积、lpa循环方向、轨迹长度、yd中轴截距、
//sh中间段斜率（整体电流标准差之和）、D瞬时导纳变化、SD最大/最小点距离、峰值电压与峰值电流的欧几里得距离
//--------------------------------------------------------------------------
#include "AllHead.h"
#include <math.h> 
#include "api_VI.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define vl	0.001f	//电压阈值

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  平均值计算
//         
//参数:		float *array   输入数组
//			BYTE length      数组长度
//         
//返回值:    数组的平均值
//         
//备注:  
//--------------------------------------------------
float calculate_mean(float *array, BYTE length) 
{
	float sum;
	BYTE i;
	
	sum= 0.0;
	for(i = 0; i < length; i++) 
	{
		sum += array[i];
	}

	return sum / length;
}
//--------------------------------------------------
//功能描述:  标准差计算
//         
//参数:		float *array   输入数组
//			BYTE length      数组长度       
//返回值:   数据标准差 
//         
//备注:  
//--------------------------------------------------
float calculate_std_dev(float *array, BYTE length) 
{
	float mean, sum;
	BYTE i;

	if(length < 2) 
	{
		return 0.0; // 避免除以零
	}

	mean = calculate_mean(array, length);
	sum = 0.0;
	for(i = 0; i < length; i++) 
	{
		sum += (array[i] - mean) * (array[i] - mean);
	}

	return sqrt(sum / length);
}
//--------------------------------------------------
//功能描述:  比较函数，排序，比较两个ValueIndex对象的值大小
//         
//参数:	 @param a 指向第一个ValueIndex对象的指针
//		@param b 指向第二个ValueIndex对象的指针
//         
//返回值:    
//         
//备注:	比较两个ValueIndex对象的值大小，如果第一个对象的值小于第二个对象的值，则返回-1；
//		如果第一个对象的值大于第二个对象的值，则返回1；如果两个对象的值相等，则返回0。 
//--------------------------------------------------
int compare( void const *a,  void const *b) 
{
	TValueIndex *va = (TValueIndex *)a;
	TValueIndex *vb = (TValueIndex *)b;
	
	if (va->value < vb->value) 
		return -1;
	if (va->value > vb->value) 
		return 1;

	return 0;
}
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
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
TIResult GetVI_IMAX_ITC(float* input_I,BYTE len)
{  
	float max_I,min_I,itc;	
	BYTE i;
	TIResult result;	

	if (len == 0 || input_I == NULL)
	{
		result.max_I = 0;	//电流峰值
		result.itc = 0;		//itc电流跨度
		// 输入无效，返回错误值
		return result;
	}

	max_I = input_I[0];	//初始化最大电流值
	min_I = input_I[0];	//初始化最小电流值

	//遍历数组，找出最大最小电流值
	for( i=0; i < len; i++ )
	{
		if( input_I[i] > max_I )
		{
			max_I = input_I[i];
		}
		if( input_I[i] < min_I )
		{
			min_I = input_I[i];
		}
	}	
	itc = max_I - min_I;

	result.max_I = max_I;	//电流峰值
	result.itc = itc;	//itc电流跨度

	return result;
}
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
TVIarlpa GetVI_AR_LPA(float* input_V,float* input_I,BYTE len)
{
	float ar,lpa,sum1,sum2,temp; 	// ar面积
	BYTE i,j;
	TVIarlpa result;	

	sum1 = 0.0f;	
	sum2 = 0.0f;	

	if (len == 0 || input_V == NULL || input_I == NULL)
	{
		result.ar = 0.0f;
		result.lpa = 3.0f;		//循环方向
		// 输入无效，返回错误值
		return result;
	}

	// 使用鞋带(Shoelace formula)公式计算面积
	for (i = 0; i < len; i++)
	{
		j = (i + 1) % len;
		//这个点的电压*下一个点的电流-下一个点的电压*这个点的电流;
		//ar面积
		sum1 += fabs(input_V[i] * input_I[j] - input_V[j] * input_I[i]);
		//lpa循环方向
		sum2 += input_V[i] * input_I[j] - input_V[j] * input_I[i];
	}

	ar = (0.5f) * fabs(sum1);
	temp  = (0.5f) * (sum2);

	if( temp > 0 )
	{
		lpa = 1.0f;		//顺时针,电流超前电压
	}
	else if(temp < 0)
	{
		lpa = -1.0f;	//逆时针,电流滞后电压
	}
	else
	{
		lpa = 0.0f;		//电流与电压同相位
	}

	result.ar = ar;		//面积
	result.lpa = lpa;	//循环方向

	return result;
}
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
float GetVI_Loop_Length(float* input_V,float* input_I,BYTE len)
{
	float loop_length,dx,dy,distance;	// 轨迹长度
	BYTE i,j;	

	loop_length = 0.0;
	for (i = 0; i < len - 1; i++) 
	{
		dx = input_V[i + 1] - input_V[i];
		dy = input_I[i + 1] - input_I[i];
		distance = sqrt(dx * dx + dy * dy);
		loop_length += distance;
	}
	// Add distance from last to first point
	dx = input_V[0] - input_V[len - 1];
	dy = input_I[0] - input_I[len - 1];
	distance = sqrt(dx * dx + dy * dy);

	loop_length += distance;
	
	return loop_length;
}
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
float GetVI_SH(float* input_V,float* input_I,BYTE len)
{  
	float sh;

	sh = calculate_std_dev(input_I, len);	// 计算整体电流数据的标准差

	return sh;
}
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
float GetVI_YD(float* input_V,float* input_I,BYTE len)
{  
	float yd,value1,value2;	
	BYTE i, middle1, middle2,originalIndex1,originalIndex2;		


	if (len % 2 != 0) 
	{
		return	0;
	}

	//存储值与索引
	TValueIndex vi[SAMPLE_NUM];
	for ( i = 0; i < len; i++ ) 
	{
		vi[i].value = input_V[i];
		vi[i].index = i;
	}

	qsort(vi, len, sizeof(TValueIndex), compare);

	middle1 = (len / 2) - 1;
	middle2 = len / 2;

	//找到横坐标最中间的两个点的原始索引位置
	originalIndex1 = vi[middle1].index;
	originalIndex2 = vi[middle2].index;

	//找到这两个点对应的电流值
	value1 = input_I[originalIndex1];
	value2 = input_I[originalIndex2];
	//计算yd值
	yd = fabs(value1 - value2);

	return yd;
}
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
float GetVI_D(float* input_V,float* input_I,BYTE len)
{    

	float sum_s;  				// 导纳的总和
	float sum_s2; 				// 导纳平方的总和
	BYTE valid_count; 		// 有效数据点的数量
	BYTE i;
	float s_i;
	float mean_s; 				// 导纳的平均值
	float variance;				// 导纳的方差
	float D;	// 导纳的标准差


	if (len == 0 || input_V == NULL || input_I == NULL)
	{
		// 输入无效，返回错误值
		return -1.0f;
	}

	// 初始化求和变量
	sum_s = 0.0f; 
	sum_s2 = 0.0f; 
	valid_count = 0;

	// 遍历数据，计算瞬时导纳
	for (i = 0; i < len; i++)
	{
		// 检查电压是否大于阈值
		if (fabs(input_V[i]) >= vl)
		{
			// 计算瞬时导纳
			s_i = input_I[i] / input_V[i];
			// 累加导纳和,导纳平方
			sum_s += s_i;
			sum_s2 += s_i * s_i;
			valid_count++;
		}
	}
	// 如果没有有效数据点，返回错误值
	if (valid_count == 0)
	{
		return -1.0f;
	}

	// 计算导纳的平均值和标准差
	mean_s = sum_s / valid_count;
	//方差
	variance = (sum_s2 / valid_count) - (mean_s * mean_s);
	//导纳的标准差
	D = sqrt(variance);

	return D;

}
//-----------------------------------------------
//函数功能: V_I轨迹特征----SD最大/最小点距离、PED
//参数:     float* input_V  周波电压数据
//         float* input_I   周波电流数据
//          Len[in]         数据长度
//          float vi_SD      输出最大/最小点距离、峰值电压与峰值电流所在点的欧几里得距离
//
//返回值: 特征序号9、10
//-----------------------------------------------
TVISdPed GetVI_SD_PED(float* input_V,float* input_I,BYTE len)
{
	float maxV,minV,maxI,minI,dx,dy;
	float x1,x2,y1,y2;
	float vi_SD,ped,temp_V,temp_I;
	BYTE i;
	TVISdPed result;

	
	if (len == 0 || input_V == NULL || input_I == NULL) 
	{
		result.vi_SD = -0.1f;
		result.ped = -0.1f;
		// 处理无效输入
		return result;
	}

	// 初始化最大值和最小值
	maxV = input_V[0];  // input_V 的最大值
	minV = input_V[0];  // input_V 的最小值
	maxI = input_I[0];  // input_I 的最大值
	minI = input_I[0];  // input_I 的最小值

	// 遍历数组，找到每个数组的最大值和最小值
	for ( i = 1; i < len; i++ ) 
	{
		// 处理 input_V
		if (input_V[i] > maxV) 
		{
			maxV = input_V[i];
			x1 = maxV;			//记录峰值电压处的横坐标
			y1 = input_I[i];	//记录峰值电压处的纵坐标

		}
		if (input_V[i] < minV) 
		{
			minV = input_V[i];	
		}

		// 处理 input_I
		if (input_I[i] > maxI) 
		{
			maxI = input_I[i];
			x2 = input_V[i];	//记录峰值电流处的横坐标
			y2 = maxI;			//记录峰值电流处的纵坐标
		}
		if (input_I[i] < minI) 
		{
			minI = input_I[i];
		}
	}

	temp_V = (maxV - minV)*(maxV - minV);
	temp_I = (maxI - minI)*(maxI - minI);
	vi_SD = sqrt(temp_V + temp_I);

	dx = fabs(x1 - x2);
	dy = fabs(y1 - y2);
	ped = sqrt(dx*dx + dy*dy);

	result.vi_SD = vi_SD;
	result.ped = ped;

	return result;
}
