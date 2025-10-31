//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	张玉猛
//创建日期	2018.8.13
//描述		冻结模块内部头文件
//修改记录
//----------------------------------------------------------------------
#ifndef __FREEZE_H
#define __FREEZE_H
//--------------------------------------

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define CURR_PLAN(OAD)						(0x07&(OAD>>21))	//取OAD中的方案号

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct TFreezeInfoTab_t
{
	WORD		OI;
	eFreezeType	FreezeIndex;
	BYTE		MaxAttNum;
	DWORD		dwDataLen;
	DWORD		wEeDepth;
}TFreezeInfoTab; 


typedef struct TFreezeAddrLen_t
{
	WORD	wAllInfoAddr;
	WORD	wDataInfoEeAddr;
	DWORD	dwAttOadEeAddr;
	DWORD	dwAttCycleDepthEeAddr;
	DWORD	dwDataAddr; //除分钟冻结外，其他固定使用0 数据存储地址，第三存储空间
}TFreezeAddrLen;
typedef struct TFreezeData_t
{
	TDLT698RecordPara 		*pDLT698RecordPara;
	TFreezeAttOad			*pAttOad;
	TFreezeAttCycleDepth	*pAttCycleDepth;
	TFreezeDataInfo			*pDataInfo;
	TFreezeAddrLen			*pFreezeAddrLen;
	TFreezeAllInfoRom		*pAllInfo;
	TFreezeMinInfo			*pMinInfo;
	BYTE					*pBuf;
	DWORD					*pTime;
	BYTE					MinPlanIndex;     //要读分钟冻结方案
	BYTE					Tag;
	BYTE					FreezeIndex;
	WORD					Len;
	DWORD					SearchTime;         // [out] 找到的时间
	DWORD					RecordNo;           // [out] 找到的序号
}TFreezeData; 

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

BOOL GetFreezeAddrInfo( BYTE inFreezeIndex, TFreezeAddrLen *pFreezeAddrLen );

WORD ReadFreezeByRecordNo( TFreezeData	*pData );


#endif//对应文件最前面的#define __FREEZE_H
