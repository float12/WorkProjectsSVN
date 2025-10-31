//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.5
//描述		需量模块Api头文件
//修改记录	
//----------------------------------------------------------------------
#if( SEL_DEMAND_2022 == NO )

#ifndef __DEMAND_API_H
#define __DEMAND_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//占12字节
typedef struct TDemandData_t
{
	long DemandValue;
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

	#if( SEL_APPARENT_ENERGY == YES )
	// 视在（正向、反向）
	TDemandData Apparent[2];
	#endif

	//校验
	DWORD CRC32;
	
}TDemandDBase;


// 安全空间需量结构定义 只存当前的
typedef struct TDemandSafeRom_t
{
	TDemandDBase TotalDemand;				// 总需量
	TDemandDBase RatioDemand[MAX_RATIO];	// 费率需量

}TDemandSafeRom;

typedef enum
{
   	eClearRamDemand = 0,         	//清Ram需量，包括当前需量与最大需量
   	eClearAllDemand = 0x55          //清全部需量，包括当前需量与最大需量以及e2中的需量
}eClearDemandType;//数据加TAG标志

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
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_ClrDemand( eClearDemandType eType );


//-----------------------------------------------
//函数功能: 需量模块初始化
//
//参数: 	   无

//                    
//返回值:     无
//
//备注:   
//-----------------------------------------------
void api_InitDemand( void );

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
//              APPARENT_P	10		// 正向视在
//				APPARENT_N	11		// 反向视在
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


#endif//#ifndef __DEMAND_API_H
#endif
