//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.5
//描述		需量模块Api头文件
//修改记录	
//----------------------------------------------------------------------
#if( SEL_DEMAND_2022 == YES )

#ifndef __DEMAND_API_H
#define __DEMAND_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//存储需量的换算
#define MAX_DEMAND_DOT			(5)

//读取需量传入的dot值异常时，是用的默认值
#define DEFAULT_DEMAND_DOT		(4)
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//占12字节
typedef struct TDemandData_t
{
	DWORD DemandValue;
	TRealTimer DemandTime;
	
}TDemandData;


typedef struct TDemandDBase_t
{
	//有功（正向、反向）
	TDemandData Active[2];

	//四象限无功
	#if( SEL_REACTIVE_DEMAND == YES )
	TDemandData RActive[4];
	#endif

	//校验
	DWORD CRC32;
	
}TDemandDBase;


// 安全空间需量结构定义 只存当前的
typedef struct TDemandSafeRom_t
{
	TDemandDBase TotalDemand;				// 总最大需量
	TDemandDBase RatioDemand[MAX_RATIO];	// 费率最大需量

}TDemandSafeRom;

typedef enum
{
   	eClearRamDemand = 0,         	//清Ram需量，包括当前需量与最大需量
   	eClearAllDemand = 0x55          //清全部需量，包括当前需量与最大需量以及e2中的需量
}eClearDemandType;//数据加TAG标志

//暂停需量计量标志枚举
typedef enum
{
	eDemandRun = 0,//执行需量计量
	eDemandPause = 0x55,//暂停需量计量
}eDemandPauseFlag;

//6种需量类型枚举
typedef enum
{
	ePActiveDemand = 0,//正向有功
	eNActiveDemand = 1,//反向有功
	eReactive1Demand = 2,//1象限无功
	eReactive2Demand = 3,//2象限无功
	eReactive3Demand = 4,//3象限无功
	eReactive4Demand = 5,//4象限无功
	eMaxDemand6Type,//需量类型最大数量
}eDemand6Type;

//需量滑差计数类型枚举
typedef enum
{
	eTotalDemand = 0,//总需量差计数类型
	eCurRateDemand = 1,//当前费率需量差计数类型
	eMaDemandRateType,//需量滑差计数类型最大值
}eDemandRateType;


//需量计算用的功率的方向
typedef enum
{
	eDemandPForward = 0,//功率方向为正，枚举值需要为0
	eDemandPReverse = 1,//功率方向为负
}eDemandPDir;

//需量计算用的功率状态
typedef struct TDemandPStatus_t
{
	BOOL IsStart;//TRUE/FALSE 是/否超起动门限
	eDemandPDir PDir;//功率方向
}TDemandPStatus;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------


//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 需量计算的主函数，每秒执行一次
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_DemandTask( void );

//-----------------------------------------------
//函数功能: 需量清零
//
//参数: 	eType[in]：		要清需量的类型
//      eClearRamDemand	清当前需量、清内存中当前最大需量
//	    eClearAllDemand	清eeprom中当前最大需量		
//
//			InDemandPauseFlag[in]: 暂停需量计量标志
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_ClrDemand( eClearDemandType eType, eDemandPauseFlag InDemandPauseFlag );


//-----------------------------------------------
//函数功能: 需量模块初始化
//
//参数:		
//                    
//返回值:     无
//
//备注:   
//-----------------------------------------------
void api_InitDemand(       );

//-----------------------------------------------
//函数功能: 需量模块上电初始化
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_PowerUpDemand( void );

//-----------------------------------------------
//函数功能: 读当前总需量（不支持费率需量）
//
//参数:
//	Type[in]:	1  	有功
//				2	无功
//				3	视在
//	DataType[in]:	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//	Dot[in]:	小数点位数
//	pDemand[out]:输出数据
//
//返回值:	无

//备注: DATA_BCD格式最高位位符号位
//-----------------------------------------------
void api_GetCurrDemandData( WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *pDemand );

//-----------------------------------------------
//函数功能: 读最大需量包括发生时间
//
//参数: 
//	Type[in]:	Bit15		1--冻结周期最大需量	0--非冻结周期最大需量
//				P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//  Ratio[in]:	费率
//				1-MAX_RATIO  	 其它个费率
//				0				 各费率总计             
//	Dot[in]:	小数点位数
//	Buf[out]:	输出数据
//
//返回值:	TRUE/FALSE

//备注: 
//-----------------------------------------------
BOOL api_GetDemand(WORD Type, BYTE DataType, WORD Ratio, BYTE Dot, BYTE * Buf);

//-----------------------------------------------
//函数功能: 读RAM中缓存的6种当前总需量（包括正向有功、反向有功、1~4象限，不支持费率需量）
//
//参数: 
//	Type[in]:	需量类型，包括正向有功、反向有功、1~4象限无功
//				
//	DataType[in]:	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)      
//	Dot[in]:	小数点位数
//	Len[in]:    读取需量长度
//	pDemand[out]:输出数据
//
//返回值:	无

//备注: DATA_BCD格式最高位位符号位
//-----------------------------------------------
void api_Get6TypeCurrDemandData(eDemand6Type Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE * pDemand);

//-----------------------------------------------
//函数功能: 更新6个需量通道对应的电能脉冲增量及时间增量
//
//参数:		Type[in]：	ePActiveDemand --正向有功
//						eNActiveDemand --反向有功
//						eReactive1Demand --1象限无功
//						eReactive2Demand --2象限无功
//						eReactive3Demand --3象限无功
//						eReactive4Demand --4象限无功
//			PulseNum[in]:		脉冲增量
//返回值: 	无
//
//备注:   
//-----------------------------------------------

void api_UpdateDemandEnergyPulse( eDemand6Type Type, WORD PulseNum );


#endif//#ifndef __DEMAND_API_H
#endif
