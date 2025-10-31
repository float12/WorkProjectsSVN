/**
 * 中间件
 * @作者 jwh
 * @版权 
 * @版本 1.0
 * @时间 2023年03月08日
**/
#ifndef __Mid_api_H__
#define __Mid_api_H__
#include "mid.h"

#ifdef __cplusplus
 extern "C" {
#endif

//只支持IAR和keil版本的编译器，以下宏定义二选一
#define UseIAR
//#define UseKeil
//#define UseEclipse
//#define UseIAR

//--------------------------------------------中间件相关定义-----------------------------------------------------//

extern Mid_BOOL MidFunction( TMeterFun MeterFun,TMidInfoData *MidInfoData, TMidFunction * MidFunction);

//--------------------------------------------扩展功能相关定义-----------------------------------------------------//

extern const MidEnterFunction MidEnterFunctionConst;
extern void ExpandFunction( CommonDataFunction MidFun, TExpandOneFunction *ExpandOneFunction);

//------------------------------导轨表本体接口--------------------------------------------//

//中间件入口函数宏定义调用此宏定义加载中间件入口函数
#define	LoadMidEnterFunction( MeterFun, MidInfoData, MidFunction) ((MidEnterFunction)MidEnterFunctionConst)( MeterFun, MidInfoData, MidFunction)

extern 	void * pMoudleRAM_01;
extern	void * pMoudleRAM_02;
extern	void * pMoudleRAM_03;
extern	void * pMoudleRAM_04;
extern	void * pMoudleRAM_05;
extern	void * pMoudleRAM_06;
extern	void * pMoudleRAM_07;
extern	void * pMoudleRAM_08;
extern	void * pMoudleRAM_09;
extern	void * pMoudleRAM_10;

#ifdef __cplusplus
}
#endif

#endif

























































