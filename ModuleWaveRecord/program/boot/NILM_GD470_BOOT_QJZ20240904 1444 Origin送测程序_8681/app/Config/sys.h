//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	wangjunsheng
//创建日期	2020.10.28
//描述		用于配置功能
//修改记录	
//----------------------------------------------------------------------
#ifndef __SYS_H
#define __SYS_H

#define     MODULE_VERSION      0xb003

#define     INIT_CODEBUFFER     NO

#define     ANALOG_SOURCE       NO      // 源类型，是否选择数字源
#define     MAX_PHASE           1       // 单相表为1，三相表为3
#define     SAMPLE_NUM          128     // 单周波采样点数
#define     MMXU_LOW_SMP        1       // 瞬时量计算降采样倍数
#define     HAMONIC_LOW_SMP     1       // 谐波计算降采样倍数
#define     SEL_REPORT_FUNC     NO      // 选择是否开启主动上报

#if(MAX_PHASE == 1)
#define     MODULE_CPU_FREQ     CPU_FREQ_240M
#elif(MAX_PHASE == 3)
#define     MODULE_CPU_FREQ     CPU_FREQ_240M
#else
相别宏定义配置不对
#endif

// 是否选择新规约 DL/T645-2007
#define SEL_DLT645_2007         YES


#define TimeMode    0 //1 // 日历驱动模式, 0:定时器驱动日历,秒中断实现对时. 1:定时器驱动日历的毫秒,秒中断驱动日历的秒以上,秒中断实现对时

#define BOARD_TYPE BOARD_ST_PQM

#endif//对应文件最前面的#ifndef __SYS_H



