//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	wangjunsheng
//创建日期	2020.10.28
//描述		用于配置功能
//修改记录	
//----------------------------------------------------------------------
#ifndef __SYS_H
#define __SYS_H

//1、表计功能相关宏
//-------------------------------------------------------------------------------------------------------------
#define     MAX_PHASE           			1 	// 单相表为1，三相表为3

#define     USER_TEST_MODE       			0 	//定义是电科院送测=1--密文，随机数; 现场用户=0--现场用户支持明文抄读

#define     SEL_WSD_MODE       				1 	//威思顿模式，0:关闭，1：开启

#define 	SEL_DEBUG_VERSION 				0

#define     USE_DEBUG						0 	//测试模式 0:关闭，1：开启

#define     USE_SAFETY_MODE					1

#define     USE_CODE_SAFEMODE				1

#define     USE_PARA_CTRL					1

#define 	USE_PRINTF						1 	// 0不初始化rtt功能，1初始化rtt功能，初始化后的rtt功能具备打印调试功能，默认为1

#define 	USE_WDG_DEBUG 					0	// 0:不初始化WDG，1：初始化WDG,调试代码时候用0，正式供货、电科院测试、测试组用1

#define 	APP_DELAY_S  					5 	// 上电延时5s
//-------------------------------------------------------------------------------------------------------------

//2、算法功能相关宏
//-------------------------------------------------------------------------------------------------------------
#define 	USE_NILM						0 	//启动负荷辨识算法，0是不启用，1是启用

#define 	USE_EBIDF						0 	//启用电动车专用识别程序，负荷辨识模组是0，电动车专版是1

#define 	USE_RECORDWAVE 					1 	//启用录播功能，0是不启用，1是启用

#define		EB_EXPMSG						0	//电动车显示启、停时间、合并启停数,如果有用户需要提供这些信息，则置1

#define 	PROTOCOL_VERSION 				25 	//24代表是已经供货的老规范，25代表的是25年新规范
//-------------------------------------------------------------------------------------------------------------

// 版本号

#define     MODULE_VERSION      			(0xF000+(MAX_PHASE<<8)+(USER_TEST_MODE<<4)+(USE_EBIDF)) //负荷辨识模组默认F000,电动车专版20250905改成F000,20251105改成F001

#define     HARD_VERSION        			(0xB002+(MAX_PHASE<<8))

#define     MODULE_VERSION_T     			"V1.00"

#define     HARD_VERSION_T       			"V1.00"

#define     MODULE_VERSION_F     			"v1.0.0"

#define     HARD_VERSION_F       			"v1.0.0"

#define     HARD_TIME	         			"240606"

#define     UART_DEFAULT_BAUD        		115200U	//96和1152自适应
#define     UART_DEFAULT_BAUD_SUB    		9600U


#define     ANALOG_SOURCE       			NO 		// 源类型，是否选择数字源
#define     SAMPLE_NUM          			256 	// 单周波采样点数,固定256，不能修改,这个是dma缓存最大空间
#define 	G_SAMPLE_NUM  					128 	//采样点数,无论物联表传来的是256、128还是64点，最后都转为128点的数据。
#define     MMXU_LOW_SMP        			1 		// 瞬时量计算降采样倍数
#define     HAMONIC_LOW_SMP     			1 		// 谐波计算降采样倍数
#define     SEL_REPORT_FUNC     			NO 		// 选择是否开启主动上报
#define 	RESULT_NUM     					10 		//负荷辨识算法一个result中设备的数量上限
#define 	EB_RESULT_NUM 					10 		//电动车算法支持的最大识别电动车个数
#if(MAX_PHASE == 1)
#define     MODULE_CPU_FREQ     			CPU_FREQ_240M
#elif(MAX_PHASE == 3)
#define     MODULE_CPU_FREQ     			CPU_FREQ_240M
#else
相别宏定义配置不对
#endif

// 是否选择新规约 DL/T645-2007
#define SEL_DLT645_2007         			YES

#define LOGICADDR							0x03
#define TimeMode    						0 //1 // 日历驱动模式, 0:定时器驱动日历,秒中断实现对时. 1:定时器驱动日历的毫秒,秒中断驱动日历的秒以上,秒中断实现对时

#define BOARD_TYPE 							BOARD_ST_PQM
#endif//对应文件最前面的#ifndef __SYS_H



