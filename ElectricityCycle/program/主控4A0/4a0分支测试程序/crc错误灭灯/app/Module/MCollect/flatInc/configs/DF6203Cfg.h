///////////////////////////////////////////////////////////////
//	文 件 名 :DF6203Cfg.h
//	文件功能 : DF6203 配置
///////////////////////////////////////////////////////////////
#ifndef _DF6203CFG_H_
#define _DF6203CFG_H_
#include "__define.h"
#include "module.h"

//基本硬件配置
#include "ABC_DF6203.h"

#define	MAX_VPORT_NUM		5		//(最大不能超过32)

#define MAX_DWNPORT_NUM		5	    //485、载波、can、红外

#define	MAX_MP_NUM			(32+1)	

	
//内存中最多任务个数
#define	MAX_698_TASK_NUM	16		//(4096/sizeof(TOad60130200))

#define MAX_TASK_TIMEREG_NUM	4					

#ifndef MD_MDLT645_97
#define MD_MDLT645_97		YES
#endif
#ifndef MD_MDLT645_07
#define MD_MDLT645_07		YES
#endif	


#endif//结束
