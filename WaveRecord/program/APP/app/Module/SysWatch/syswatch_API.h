//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.16
//描述		系统监视头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __SYS_WATCH_API_H
#define __SYS_WATCH_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#if( MAX_PHASE == 3 )
//时钟电池空阀值
#define LOW_BATT_CLOCK				210//2.1v
//抄表电池空阀值
#define LOW_BATT_READ				460//4.6v
#else
//时钟电池空阀值
#define LOW_BATT_CLOCK				210//2.1v
//抄表电池空阀值
#define LOW_BATT_READ				210//2.1v
#endif

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct TSysWatchRamData_t
{
	//温度 保留一位小数 单位度
	short Temperature;

	//当前5V电压状态 保留二位小数 单位v
	WORD VolNow;

	//保留二位小数 单位v
	WORD ClockBattery;
	WORD ReadBattery;

}TSysWatchRamData;

#pragma pack(1)
//电池参数
typedef struct TBatteryPara_t
{
	signed char ClockBatteryOffset;//时钟电池电压补偿值
	signed char ReadBatteryOffset;//停电抄表电池电压补偿值
	DWORD CRC32;
}TBatteryPara;
#pragma pack()

typedef struct TSysWatch_t
{
	TBatteryPara BatteryPara;//电池参数

}TSysWatchSafeRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TBatteryPara BatteryPara;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//--------------------------------------------------
//功能描述: 校准电池参数
//         
//参数:     无
//         
//返回值:    WORD True 成功； False 失败
//         
//备注内容:  无
//--------------------------------------------------
WORD api_AdjustBatteryVoltage( void );

//-----------------------------------------------
//函数功能: 获取停电抄表电池，时钟电池，温度值
//
//参数: 
//			DataType[in]			DATA_BCD或DATA_HEX码
//			Channel[in]				选择要读数的数据类型
//				SYS_POWER_AD		    		0//系统电压 保留二位小数 单位v
//				SYS_CLOCK_VBAT_AD		    	1//时钟电池电压 保留二位小数 单位v
//				SYS_READ_VBAT_AD	    		2//抄表电池的电压 保留二位小数 单位v
//				SYS_TEMPERATURE_AD		    	3//温度AD采样 保留一位小数
//                    
//返回值:  	读出的数据值,BCD码格式时BIT15为1表示负数，HEX码格式时是补码数据
//
//备注:   
//-----------------------------------------------
WORD api_GetBatAndTempValue( BYTE DataType, BYTE Channel );

//-----------------------------------------------
//函数功能: 系统监视模块任务
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SysWatchTask(void);

//-----------------------------------------------
//函数功能: 系统监视模块的上电初始化工作
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_PowerUpSysWatch(void);

//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_FactoryInitSyswatch( void );


DWORD  GetTicks(void);

#endif//#ifndef __SYS_WATCH_API_H

