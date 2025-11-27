//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __ALGORITHM_THREAD_DKY_PH1_H__
#define __ALGORITHM_THREAD_DKY_PH1_H__
#include "nilm_FeatureExtraction.h"
#include "nilm_Dbscan.h"
#include "nilm_Processing.h"
#include "nilm_Thread.h"

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define NILM_POWER_MIN 				50 				//算法认定不能识别的功率，低于此功率不识别
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern int         Count_features_002s;  //存15min数据计数，每COUNT=0.02s
extern uint8_t                     Flag_15min;  //存够15min数据标志,'1'有效
static uint8_t ftmpCount = 0;  //用来统计有多少个feature（s）因为15min计算在进行被存进feature_buffer
extern rt_sem_t proc_flag_15min_sem;
extern date_time_s time_15min;
extern BYTE PhaseNum;										//算法使用的相位总数
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
/* 
 * 特征算法线程入口函数 
 * 参数：
 *      uint16_t WAVELEN  -- 周波点数（128、256）
 *      uint8_t phase     -- 单相 1，三相 3
 *      float * U_data    -- 电压
 *      float * I_data    -- 电流
 */
extern void AlgoThreadEntry(uint16_t WAVELEN, uint8_t phase, float U_data[MAX_PHASE][G_SAMPLE_NUM], float  I_data[MAX_PHASE][G_SAMPLE_NUM]);
/* 
 * 算法线程初始化
 */
extern void proc_init(void);
/* 
 * 15min负荷识别算法线程入口函数 
 */
extern void proc_thread_entry(void);
//--------------------------------------------------
//功能描述:  负荷辨识任务的参数初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  NILMParaInit( void );
//--------------------------------------------------
//功能描述:  获取时间
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void GetTime(date_time_s *time);
#endif //#ifndef __ALGORITHM_THREAD_DKY_PH1_H__