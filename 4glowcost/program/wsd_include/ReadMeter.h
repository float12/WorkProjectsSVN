//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		READMETER头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __READMETER_H
#define __READMETER_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define UNSIGNED 	0x00 //无符号
#define SIGNED 		0x80   //有符号
#define PT_MULTI 	0x01 //上传数据乘PT
#define CT_MULTI 	0x02
#define PT_CT_MULTI 0x03
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct tMeterRead_t
{
	DWORD OI;	//645数据标识
	BYTE Num;	//一个数据块有几个数据项
	BYTE Slen;   //单个数据项长度
	BYTE Dot;	//接收数据小数位
	BYTE Symbol; //是否需要处理符号位 bit7 符号判断 bit1 bit0 区分变比
	double *Buf; //数据存放地址
} tMeterRead;


typedef enum
{
	eUa = 0, // 电压数据块
	eUb,
	eUc,
	eIa,  // 电流数据块
	eIb,
	eIc,
	eIzs, // 零序电流
	eP1, // 有功功率数据块
	eP2,
	eP3,
	eP4,
	eQ1, // 无功功率数据块
	eQ2,
	eQ3,
	eQ4,
	eS1, // 视在功率数据块
	eS2,
	eS3,
	eS4,
	ePF1, // 功率因数数据块
	ePF2,
	ePF3,
	ePF4,
	eComActive, //组合有功总电能
	ePActiveAll, // 正向有功总电能
	eNActiveAll, // 反向有功总电能
	eRActive1All, // 无功1总电能
	eRActive2All, // 无功2总电能
	ePa_PActive1, //a相正向有功
	ePa_NActive1, //a相反向有功
	ePa_ComRActive1, //a相组合无功1
	ePa_ComRActive2, //a相组合无功2
	ePb_PActive1, //b相正向有功
	ePb_NActive1, //b相反向有功
	ePb_ComRActive1, //b相组合无功1
	ePb_ComRActive2, //b相组合无功2
	ePc_PActive1, //c相正向有功
	ePc_NActive1, //c相反向有功
	ePc_ComRActive1, //c相组合无功1
	ePc_ComRActive2, //c相组合无功2
	eTemp1, //温度1
	eTemp2, //温度2
	eTemp3, //温度3
	eTemp4, //温度4
	eLineUa,
	eLineUb,
	eLineUc,
	eNullType,
} eValueType;//  修改MeterReadOI抄表项，对应修改枚举偏移 
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern const tMeterRead MeterReadOI[];
extern const BYTE CycleReadMeterNum;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
#if (CYCLE_METER_READING == YES)
//--------------------------------------------------
//功能描述:  645 连续抄读
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Dlt645_Tx_ContinueRead( BYTE bStep, BYTE *bBuf);
//-----------------------------------------------
//功能描述:  抄表数据解析并保存
//
//参数:      pBuf[in]  dataBuf[in] 数据保存位置
//
//返回值:
//
//备注:
//--------------------------------------------------
void DataAnalyseandSave(BYTE *pBuf, double *dataBuf, BYTE Len, tMeterRead tMeterReadOIRam);
//--------------------------------------------------
//功能描述: 计量数据传送给不同上报组帧任务
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void SampleDatatoReport(TRealTimer *pTime);
#endif
#endif //#ifndef __READMETER_H
