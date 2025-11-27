//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef _EB_PROC_H
#define _EB_PROC_H

#if(USE_EBIDF)
#include <string.h>
#include "config.h"
#include "rtthread.h"
#include "sys.h"
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define CYCLES_POINT_10 			10 				//十个周波
#define FEATRUE_SIZE_MINUTE 		60 				//特征可以缓存60s
#define EB_DETECT_TIME 				20 				//检测20s
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)
struct features1min_djc_typeDef //1min特征
{
    float P[FEATRUE_SIZE_MINUTE];
    float D[FEATRUE_SIZE_MINUTE];
};

typedef struct TEBRES_t 
{
	BYTE RevTime[MAX_PHASE];					//相对当前15分钟的偏移分钟数，相对0开始的
	DWORD RevSec[MAX_PHASE];					//当前系统时间的相对s
	float PMAX_PHASE[MAX_PHASE]; 				//电动车加入前后的功率差
	float DMAX_PHASE[MAX_PHASE]; 				//电动车加入前后的畸变功率差
	float BeforPower[MAX_PHASE];				//电动车启动前的功率
	float AverPower[MAX_PHASE]; 				//电动车加入时刻后，80s钟内的平均功率
	BYTE AverCount[MAX_PHASE];					//计算电动车平均值的计数器
	WORD TimeToClose[MAX_PHASE];				//识别到电动车后的标志关闭倒计时
	BYTE EBResultTimes[MAX_PHASE]; 				//记录起始20s内电动车识别的秒数
	BYTE IsCompared[MAX_PHASE];					//最后电动车特殊判断1里是否判断过的标志
	DWORD EBWaitforReport[MAX_PHASE]; 			//等待上报结果的计时器
	float EBWaitforReportP[MAX_PHASE];			//记录电动车加入时刻的有功功率
	float EBWaitforReportD[MAX_PHASE];			//记录电动车加入时刻的畸变功率
}TEBRES;

typedef struct TABNORMEBRES_t 
{
	BYTE StartFlag[MAX_PHASE];					//开始检测的标志
	float Freq[MAX_PHASE];						//一分钟数据的变化频率
	float LastAverP[MAX_PHASE];					//记录上一分钟的平均功率
	BYTE LastAbnormFlag[MAX_PHASE];				//上一次是否异常的标志
	BYTE EndCount[MAX_PHASE];					//倒计时1分钟
	WORD RemaTime[MAX_PHASE];					//取消后的剩余时间
}TABNORMEBRES;
#pragma pack()

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE EBCurrentIndex[MAX_PHASE];
extern TEBRES EBResult[EB_RESULT_NUM]; //电动车识别结果
extern struct features1min_djc_typeDef features_djc_latest1min[MAX_PHASE];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  电动车结果上报、确认是否是电动车
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void EBCalculate(void);
//--------------------------------------------------
//功能描述:  计算电流差、计算特征、判断电动车
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void CurrentSub(float* u12old,float* i12old,float* u10new,float* i10new,BYTE ph);
//--------------------------------------------------
//功能描述:  电动车确认函数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE EBConfirm(BYTE tmpi,struct features1min_djc_typeDef *latest1min,BYTE Phase);

//--------------------------------------------------
//功能描述:  min求最小值。
//         
//参数:      float a,b
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
float min_float(float a,float b);

#endif //#if(USE_EBIDF)
#endif //#ifndef _EB_PROC_H