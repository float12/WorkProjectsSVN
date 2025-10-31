
#ifndef _APPCFGS_H_
#define _APPCFGS_H_

#include "configs/DF6203Cfg.h"
#include "paradef.h"
#include "appbsp.h"

//标准内部定时器标识(使用 OS_SetTimer引用 )
#define MTID_DRIVER			BIT0	//驱动
#define	MTID_USER			BIT1				
//系统用PV操作定义
#define MPV_PARA_TK			1		//终端临时切换参数内存（任务）
#define MPV_PARA_MPB		2		//终端临时切换参数内存（测量点参数）
#define MPV_PARA_FK			3		//终端临时切换参数内存（负控参数）
#define MPV_PARA_F38		4		//测量点采集标识F3839
#define MPV_PARA_MP			5		//测量点档案共享2K参数
#define MPV_PARA_SYS		6		//系统参数，用于同步写
#define MPV_HIS_BUF			7		//历史数据缓冲区
#define MPV_MPREAL			8		//测量点实时数据
#define MPV_MPCURV			9		//测量点曲线数据
#define MPV_FRZ				10		//测量点冻结数据
#define MPV_EVENT			11		//终端事件数据
#define MPV_EVENTUP			12		//终端主动上报事件数据
#define MPV_PARA_ESAM		13		//ESAM参数，用于同步写
#define MPV_PARA_USER		14		//用户特殊参数，用于同步写
#define MPV_698_MEM			15		//698共享内存

//通用功能
#include "usros.h"
#include "port.h"
#include "usrtime.h"
#include "usrfun.h"
#include <string.h>

//专用应用头文件
#include "mpruninfo.h"
#include "sysCfg.h"
#include "dbase.h"
#include "sysdata.h"
#include "mp.h"
#include "cgy.h"
#include "__def.h"


#endif



