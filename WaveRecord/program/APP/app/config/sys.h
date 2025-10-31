//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.27
//描述		用于配置用户功能
//修改记录	
//----------------------------------------------------------------------
#ifndef __SYS_H
#define __SYS_H

//是否把程序选择为DEBUG版本，DEBUG版本支持ASSERT，否则不支持ASSERT
#define SEL_DEBUG_VERSION			YES

//是否打开Flash读保护，供货时开启，调试可关闭
#define SEL_FLASH_PROTECT   FLASH_PROTECT_OPEN
//开启FLASH读保护   FLASH_PROTECT_OPEN
//关闭FLASH读保护   FLASH_PROTECT_COLSE

//板型定义  通过看印制板上的日期或图号选择相应的板型
#define BOARD_TYPE			BOARD_HC_SEPARATETYPE
//单相表
//BOARD_HT_SINGLE_78201602		11602//20规范 共用SPI 9600波特率-默认老E2（新E2更改保护区）
//BOARD_HT_SINGLE_78202201		12201//20规范 独立SPI 115200波特率 碳膜按键-默认新E2（老E2更改保护区）
//BOARD_HT_SINGLE_78202202		12202//20规范 独立SPI 115200波特率 高防护-默认新E2（老E2更改保护区）
//三相表
//#define BOARD_HT_THREE_0134566		34566//20规范三相表+背光控制老方式+6A小电流电阻为5欧-默认老E2（新E2更改保护区）
//#define BOARD_HT_THREE_0131699		31699//20规范三相表+背光控制新方式+6A小电流电阻为5欧-默认老E2（新E2更改保护区）
//#define BOARD_HT_THREE_0130347		30347//20规范三相表+背光控制新方式+小电流电阻加倍+支持115200波特率-默认新E2（老E2更改保护区）



//选择表类型
#define cMETER_TYPES		METER_ZT
//METER_ZT		直通表（必须为220V）
//METER_3P3W	3相3线表（必须为100V）
//METER_3P4W	3相4线表（57V、220V）

#define cMETER_VOLTAGE		METER_220V
//METER_57V		57V电压
//METER_100V	100V电压
//METER_220V	220v电压
//METER_380V	380v电压

//电流类型选择
#if (HARDWARE_TYPE == SINGLE_PHASE_BOARD)
#define cCURR_TYPES			CURR_20A
#elif (HARDWARE_TYPE == THREE_PHASE_BOARD)
#define cCURR_TYPES			CURR_500A
#endif
//CURR_1A		0.3(1.2)A表
//CURR_6A		1.5(6)A表
//CURR_60A		5(60)A
//CURR_100A		10(100)


//预付费模式(智能表选择PREPAY_REMOTE)
#define PREPAY_MODE			PREPAY_NO
//PREPAY_NO					没有预付费功能（暂不 能选择）
//PREPAY_LOCAL				表计预付费（本地）
//PREPAY_REMOTE				主站预付费(远程)


//继电器类型
#define cRELAY_TYPE			RELAY_NO
//RELAY_NO					无继电器
//RELAY_INSIDE				内置继电器
//RELAY_OUTSIDE				外置继电器

//有功等级选择（不影响程序比对，只改变保护区内的电表只读参数）
#define PPRECISION_TYPE		PPRECISION_B
//PPRECISION_A	 		    A级表-2级          
//PPRECISION_B		        B级表-1级
//PPRECISION_C	        	C级表-0.5S
//PPRECISION_D			    D级表-0.2S

//需量计算方式 仅三相表起作用
#define SEL_DEMAND_2022		YES
//NO--采用功率需量法
//YEW-采用电能需量法-新方式-22-1批开始切换

//特殊用户选择
#define cSPECIAL_CLIENTS		SPECIAL_GUO_WANG
//#define SPECIAL_GUO_WANG				0xff

//晶体的种类 （配置程序，请找硬件确认此项目）
#define cOSCILATOR_TYPE		OSCILATOR_JG
//OSCILATOR_JG			0x01	//精工晶体
//OSCILATOR_XTC	        0x02	//西铁城晶体
//OSCILATOR_XTC_3215	0x03	//西铁城晶体_3215(小封装晶体)

#endif//对应文件最前面的#ifndef __SYS_H



