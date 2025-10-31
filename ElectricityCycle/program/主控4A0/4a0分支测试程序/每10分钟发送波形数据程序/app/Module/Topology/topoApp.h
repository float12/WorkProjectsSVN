//----------------------------------------------------------------------
//Copyright (C) 2003-2024 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	高朋辉
//创建日期	2024.7.1
//描述		拓扑识别模块内头文件
//修改记录
//----------------------------------------------------------------------
#ifndef _TOPOAPP_H_
#define _TOPOAPP_H_
#if( SEL_TOPOLOGY == YES )
#include <stdint.h>

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define TOPO_PHASE  3  				//相别，单相为1，三相为3

//不可移动不可删除
#ifdef _TOPOAPP_GLOBALS
	#define TOPOAPP_EXT
#else
	#define TOPOAPP_EXT extern
#endif
#define TOPOFUN_DISABLE 0    //"关闭拓扑识别"
#define TOPOFUN_ENABLE  0xaa //"开启拓扑识别"

#define TP_OS  					3  							//最终处理的数据数，N_Coh * N_NonCoh* N_Coh必须满足等于60
#define TP_FRAME_BIT         	16                             //"特诊电流监测BIT位数"
#define Tp_FRAME_RmsBit1		8		/*"特诊电流有效值计算位置"*/
#define Tp_FRAME_RmsBit2		9		/*"特诊电流有效值计算位置"*/
#define Tp_FRAME_RmsBit3		10		/*"特诊电流有效值计算位置"*/
#define Tp_FRAME_RmsBitNum		3		/*"特诊电流有效值计算位数"*/
#define TP_NOISEDETLEN       	0                               // zhupdebug(1 << Tp_Noise_N)//"监测噪底长度"
#define TP_FRAME_LEN        	(TP_FRAME_BIT * TP_OS)          //"监测特征电流长度"
#define TP_FIFO_LEN         	(TP_NOISEDETLEN + TP_FRAME_LEN) //"FIFO 长度"
#define TP_ERRBITTHREDEF     	1   //"默认允许出错BIT位数"
#define TP_THRESCHMIDT       	0   // zhupdebug 10//"默认阈值施密特值"
#define TP_THREHIGHLOW       	5   // 高低BIT电平阈值---需根据实际应用调整

//频率点数
#define TOPFQ_ZONE625_NUM  12  
#define TOPFQ_ZONE833_NUM  12 

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	TOPO_BIT_IDLE = 0,   // 未接收到1
	TOPO_BIT_REV = 1,    // 接收到1个bit，正在接收剩余帧
} defTopoBitStatus;

typedef enum
{
	TOPO_Dynamic_Thrd = 0,   // 动态门限
	TOPO_Fixed_Thrd = 1,     // 固定门限
} defTopoThread;

typedef enum
{
	FIFO_NotFull = 0, //"队列没有填充满"
	FIFO_Full = 1,    //"队列填充满"
} eFifoFullFlag_TypeDef;

typedef enum
{
	Tp_NoiseOft_No = 0,      // 未计算出第1个数据
	Tp_NoiseOft_Ready = 1,   // 已产生数据
} defTopoNoiseFlag_TypeDef;

typedef enum 
{
	TOPO_Slc_Refsh = 0x1,
	TOPO_Slc_WAIT = 0x0,		
}defTopoSliceRefsh;

typedef enum 
{
	Topo_Check_625hz = 0,		
	Topo_Check_833hz = 1,	
	Topo_Check_666hz = 2, 		//保留，暂不支持
	Topo_Check_1333hz = 3,		//保留，暂不支持
}defTopoCheckFreq;

//"TOPO参数"
typedef struct
{
	uint8_t FunSwitch; //"拓扑识别使能标志"

	uint8_t Info[4]; //"特征码（默认设置为：0xaae9)"
	int8_t FrameSeq[TP_FRAME_BIT];
	uint8_t Info_1BitNum; //"特征码1个数"
	uint8_t Info_0BitNum; //"特征码0个数"

	uint8_t ThreSchmidt;      //"阈值施密特配置"
	uint32_t SynDetThreCoef;  //"同步阈值典型配置"
	uint32_t ErrBitThre;      //"容错余量"
	uint32_t DemuSymDeltaLen; //"解调余量"
	uint32_t ThreHighLow;     //"高低电平阈值"
} sTopoPara_TypeDef;

//"TOPO状态"
typedef struct
{
	uint16_t Vs_Fifo_Wait; //"等待特征值过去，防止进入噪底窗口"

	uint16_t Vs_Fifo_Front;       //队头"
	uint16_t Vs_Fifo_Rear;        //队尾,新的数据进入队尾"
	uint16_t Vs_Fifo_Len;         //当前队列长度"
	int32_t Vs_Fifo[TP_FIFO_LEN]; //"有效值队列"
	int32_t Vs_Fifo_783[TP_FIFO_LEN]; /*"783HZ有效值队列"*/
	int32_t Vs_Fifo_883[TP_FIFO_LEN]; /*"883HZ有效值队列"*/	
	eFifoFullFlag_TypeDef FifoFullFlag;    //"队列填充满"
	defTopoNoiseFlag_TypeDef NoiseDetFlag; //"背景噪声是否生成的标志"

	int32_t NoiseDet_vsSum; //"噪底有效值矢量和"
	int32_t NoiseDet_vs;    //"噪底"

	int32_t VsSum; //"对特征信号积分"
	int32_t Vs783Sum;	/*"对特征信号积分"*/
	int32_t Vs883Sum;	/*"对特征信号积分"*/	

	int32_t VsDemuThre;  //"判断阈值"
	int32_t VsDemuThre1; //"判断阈值"

	int32_t PeakSynDet;    //"峰值"
	int32_t FeatSigDetOut; //"特征电流有效值"

	uint16_t FrameDet;  //"监测到电网的信号"
	uint16_t ErrBitNum; //"与目标值误差"
} sTopoStatus_TypeDef;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
TOPOAPP_EXT uint8_t Topo_Result_Num[TOPO_PHASE];   // 事件记录（当前序号)

TOPOAPP_EXT float FreqLast;   // 上1次频率

extern uint8_t FreqSinCosNum;//=6;  			//当前正余弦所在的频率组
extern uint8_t FreqBpfNum;//=6;	 				//当前滤波器参数所在的频率组

extern defTopoCheckFreq TopAcv_Topo_Freq;

extern uint32_t Tp_vs783[TOPO_PHASE];			//"N_NonCoh点平均值"
extern uint32_t Tp_vs883[TOPO_PHASE];
extern uint32_t Tp_vs[TOPO_PHASE];			//"883 783频点平均值"

extern const float FreqZone_625hz[TOPFQ_ZONE625_NUM-1];
extern const float FreqZone_833hz[TOPFQ_ZONE833_NUM-1];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------




#endif //#if( SEL_TOPOLOGY == YES )
#endif//#ifndef _TOPOAPP_H_
