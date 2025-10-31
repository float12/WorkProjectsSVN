/**
 * 中间件头文件
 * @作者 jwh
 * @版权 
 * @版本 1.0
 * @时间 2023年03月08日
**/
#ifdef __cplusplus
 extern "C" {
#endif

#ifndef __Mid_H__
#define __Mid_H__
#include <string.h>

//数据类型定义
#define Mid_BYTE	unsigned char
#define Mid_WORD    unsigned short
#define Mid_SWORD   short
#define Mid_DWORD	unsigned long
#define Mid_BOOL	unsigned char
#define	Mid_QWORD   unsigned long long
#define Mid_SQWORD	long long

#define True		1
#define False		(!True)
//中间件支持的最大扩展数（扩展功能）
#define Expand_functin_Max_Num	3

//单一中间件最大注册oi个数（扩展功能）
#define Expand_Logon_OI_Max_Num	30

//单一中间件权限数据最大长度（扩展功能）
#define Expand_DataInfo_Max_Len	500

//中间件支持的最大oi个数（扩展功能）
#define Mid_OI_Data_Num	(Expand_Logon_OI_Max_Num+Expand_functin_Max_Num)

//最大的定时任务个数
#define Max_Time_Task_Num	(Max_Expand_Time_Task_Num*Expand_functin_Max_Num)

//单一扩展功能最大定时任务不能超过3个
#define Max_Expand_Time_Task_Num	5


typedef enum
{
	eMidLoopMode = 0,	//循环任务
   	eMidSecMode, 		//秒任务
   	eMidMinMode, 		//分钟任务
}eMidTimeTaskType;

//中间件定时任务结构体
typedef struct TMidTimeTask_t   
{
	eMidTimeTaskType  TimeTaskType;	//定时任务类型
	void (*MidTimeTask)();			//定时任务函数
}TMidTimeTask;

typedef struct TMidAndExpandTimeTask_t   
{
	Mid_BYTE Num;								//定时任务个数
	TMidTimeTask TimeTask[Max_Time_Task_Num];	//定时任务函数列表
}TMidAndExpandTimeTask;

typedef struct TOIData_t   	
{
	Mid_WORD 	OI;				//OI
	Mid_BYTE	OIClass;		//OI类型
	Mid_BYTE 	OIAuth;			//OI权限
}TOIData;

typedef struct TMidOIData_t   	
{
	Mid_WORD 	OINum;						//OI个数
	TOIData 	OIList[Mid_OI_Data_Num];	//OI数据列表
}TMidOIData;

typedef struct TMidInfoData_t   	
{
	TMidOIData OIData;								//支持的获取OI数据
}TMidInfoData;

typedef struct TMidParaData_t   	
{
	Mid_BYTE ControlByte;				//控制字节
	Mid_BYTE AddressLen;				//地址长度
	Mid_BYTE ClientAddress; 			//客户机地址
	Mid_BYTE AddressType;				//地址类型
	Mid_BYTE* Address;					//地址(最大长度10字节)
	Mid_WORD OI;						//扩展应用的唯一OI，仅在透明转发时使用，其他无意义
	Mid_WORD ProxyFlag;					//代理标志 0x00 非代理报文 0x55 代理报文
	Mid_BYTE* pAPDU; 					//应用层起始指针
	Mid_WORD pAPDULen;					//应用层数据长度
	Mid_WORD  APDUMAXLen; 				//应用层数据最大长度
	void* pReserve; 					//预留指针
}TMidParaData;

//通用数据获取函数定义-导轨表和扩展功能都要实现此函数
typedef Mid_WORD (*CommonDataFunction)( TMidParaData* ExpandParaData );

typedef struct TMidFunction_t   	
{
	//中间件接口函数-定时函数
	TMidAndExpandTimeTask MidAndExpandTimeTask;
	//中间件接口函数-获取指定OAD数据
	CommonDataFunction MidDataFunction;
}TMidFunction;

typedef struct TMeterFun_t//导轨表提供的回调函数   	
{
	//导轨表本体实现此函数的方式为Mid_WORD *MeterGetDataFun( eDataType DataType, Mid_BYTE *Type, Mid_BYTE *pOAD, Mid_WORD BufLen, Mid_BYTE *Buf )
	CommonDataFunction MeterDataFun;
}TMeterFun;

//MeterFun 导轨表回调函数结构体
//MidOIData 中间件返回的数据结构体
//MidFunction 中间件返回的函数结构体
typedef Mid_BOOL (*MidEnterFunction)( TMeterFun MeterFun,TMidInfoData *MidInfoData, TMidFunction *MidFunction);

//--------------------------------------------扩展功能相关定义-----------------------------------------------------//

typedef struct TExpandData_t   	
{
	TMidOIData  ExpandOIData;
	CommonDataFunction Function;
}TExpandData;

typedef struct TExpandTimeTask_t   
{
	Mid_BYTE Num;												//定时任务个数
	TMidTimeTask TimeTask[Max_Expand_Time_Task_Num];			//定时任务列表
}TExpandTimeTask;

typedef struct TExpandFunction_t   	
{
	Mid_BYTE ExpandNum;											//扩展功能个数
	
	TMidOIData ExpandOIDataList[Expand_functin_Max_Num];		//扩展功能加密数据列表

	TExpandTimeTask ExpandTimeTask[Expand_functin_Max_Num];		//扩展功能定时任务列表
	//中间件接口函数-获取指定OAD数
	CommonDataFunction ExpandFunction[Expand_functin_Max_Num];	//扩展功能函数列表
}TExpandFunction;

typedef struct TExpandOneFunction_t   	
{
	TMidOIData ExpandOIData;

	TExpandTimeTask ExpandTimeTask;
	//中间件接口函数-获取指定OAD数据
	//DataType 
	//pOAD为4字节698OAD
	//Type为获取本体或其他数据 
	//buf为源数据
	//return 返回数据长度
	CommonDataFunction ExpandOnceFunction;
}TExpandOneFunction;

//扩展功能入口函数类型
typedef void (*ExpandOneEnterFunction)( CommonDataFunction MibFun, TExpandOneFunction *ExpandFunction);

//--------------------------------------------中间件相关定义------------------------------------------------------//

extern Mid_BOOL MidFunction( TMeterFun MeterFun,TMidInfoData *MidInfoData, TMidFunction * MidFunction);

//--------------------------------------------扩展功能相关定义-----------------------------------------------------//

extern const MidEnterFunction MidEnterFunctionConst;
extern void ExpandFunction( CommonDataFunction MidFun, TExpandOneFunction *ExpandOneFunction);

//--------------------------------------------导轨表本体接口-------------------------------------------------------//

//中间件入口函数宏定义调用此宏定义加载中间件入口函数
#define	LoadMidEnterFunction( MeterFun, MidInfoData, MidFunction) ((MidEnterFunction)MidEnterFunctionConst)( MeterFun, MidInfoData, MidFunction)

#endif

#ifdef __cplusplus
}
#endif
