/**
 * �м��
 * @���� jwh
 * @��Ȩ 
 * @�汾 1.0
 * @ʱ�� 2023��03��08��
**/
#ifndef __Mid_api_H__
#define __Mid_api_H__
#include "mid.h"

#ifdef __cplusplus
 extern "C" {
#endif

//ֻ֧��IAR��keil�汾�ı����������º궨���ѡһ
#define UseIAR
//#define UseKeil
//#define UseEclipse
//#define UseIAR

//--------------------------------------------�м����ض���-----------------------------------------------------//

extern Mid_BOOL MidFunction( TMeterFun MeterFun,TMidInfoData *MidInfoData, TMidFunction * MidFunction);

//--------------------------------------------��չ������ض���-----------------------------------------------------//

extern const MidEnterFunction MidEnterFunctionConst;
extern void ExpandFunction( CommonDataFunction MidFun, TExpandOneFunction *ExpandOneFunction);

//------------------------------�������ӿ�--------------------------------------------//

//�м����ں����궨����ô˺궨������м����ں���
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

























































