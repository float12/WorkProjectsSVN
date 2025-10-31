///////////////////////////////////////////////////////////////
//	文 件 名 :module.h
//	文件功能 :用电信息终端模块类型选择
//	作    者 : jiangjy
//	创建时间 : 2015年9月2日
//	项目名称 ：DF6203
//	操作系统 : 
//	备    注 : 可选的模块定义见sysdefs.h
//	历史记录： : 
///////////////////////////////////////////////////////////////
#ifndef __MODULE_H__
#define __MODULE_H__
#include "sysdefs.h"

//1、模块类型定义

#define MT_MODULE	MT_DF6203_JZQ_II

//硬件类型 HT_DF6203_A (负控\集中器1) HT_DF6203_B(集中器2)  
#define MT_HARDTYPE		HT_DF6203_D


//2、模块主规约定义
#define MD_BASEUSER		MGY_SDLT645_07//MD_GDW2013//

//选择ESAM类型(和实际硬件相关)
#if(MD_BASEUSER	== MD_GDW2013)
#define MESAM_TYPE		MESAM_376
#elif(MD_BASEUSER	== MD_GDW698)
#define LT_NET			BIT4			//连接类型为网络
#define MESAM_TYPE		MESAM_698
#endif

#ifndef LT_NET
#define LT_NET			0
#endif
#ifndef LT_GPRS
#define LT_GPRS			0
#endif	

#endif