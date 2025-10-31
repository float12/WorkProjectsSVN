///////////////////////////////////////////////////////////////
//	文 件 名 :usros.h
//	文件功能 :操作系统隔离层接口
//	作    者 : jiangjy
//	创建时间 : 2015年7月21日
//	项目名称 ：
//	操作系统 : 
//	备    注 :
//	历史记录： : 
///////////////////////////////////////////////////////////////
#ifndef _USROS_H_
#define _USROS_H_
#ifdef __cplusplus
 #if __cplusplus
	extern "C" {
 #endif
#endif

#if(MOS_MSVC)
#define DATA_PATH		"C:/DF6203FS"
#define ZK_PATH			"./vcLib/VLinux"
#else
#define DATA_PATH		"/DF6203FS"
#define ZK_PATH			"/usr/lib"
#endif

// BOOL OS_GetTaskName(char *pszTaskName);		
//void  SM_P(DWORD dwPVID);
//void SM_V(DWORD dwPVID);
// BOOL OS_SetTimer(DWORD events,DWORD dwMSTimes);
// void OS_KillTimer(DWORD events);

//任务优先级定义
#ifdef _MSC_VER
#define MTPRI_WATCHDOG	-13         //WATCHDOG优先级
#define MTPRI_ROOT		-12         //ROOT优先级
#define MTPRI_HIGH		-10         //高优先级
#define MTPRI_NORMAL	0			//中优先级
#define MTPRI_LOW		13          //低优先级
#define MTPRI_SYSTEM	MTPRI_HIGH  //默认系统任务优先级
#define MTPRI_USER		MTPRI_NORMAL//默认用户任务优先级
#elif(MOS_UCOSII)//uCOSII系统
#define MTPRI_WATCHDOG	0			//WATCHDOG优先级
#define MTPRI_ROOT		1			//ROOT优先级
#define MTPRI_HIGH		2			//高优先级
#define MTPRI_NORMAL	5			//中优先级
#define MTPRI_LOW		20          //低优先级
#define MTPRI_SYSTEM	MTPRI_HIGH  //默认系统任务优先级
#define MTPRI_USER		MTPRI_NORMAL//默认用户任务优先级
#elif(MOS_LINUX)
#define MTPRI_WATCHDOG	90			//WATCHDOG优先级
#define MTPRI_ROOT		85			//WATCHDOG优先级
#define MTPRI_HIGH		80			//高优先级
#define MTPRI_NORMAL	50			//中优先级
#define MTPRI_LOW		20          //低优先级
#define MTPRI_SYSTEM	MTPRI_HIGH  //默认系统任务优先级
#define MTPRI_USER		MTPRI_NORMAL//默认用户任务优先级
#endif

///////////////////////////////////////////////////////////////
//	错误返回码
///////////////////////////////////////////////////////////////
#define TASK_NO_ERR				0	//没有错误
#define TASK_ERR_CREATE			1	
#define TASK_ERR_INVALIDENTRY	2	//入口函数错误
#define TASK_ERR_NOPCB			3	//没有足够的控制块
//普通任务(安装后需要等待内核平台初始化后才可运行的任务)，系统任务(安装即可运行的任务)		
typedef enum{TASK_GEN,TASK_SYS}TASK_ATTR;
// typedef struct _TTKI{
// 	char *iszName;
// 	FV_H ifTaskEntry;
// 	DWORD iPri;
// 	DWORD idwStackSize;
// 	HPARA iArg;
// 	DWORD dwMaxTimeSlice;			//任务最长时间片（ms）,0代表禁止
// 	TASK_ATTR  Attr;				//任务属性
// }TTKI;
// INT InstallTask(TTKI *pTKI);
// void OS_Run(void);
typedef enum{MCMD_ONTIMEOUT=1}COMMAND_TASK;
// void OS_OnCommand(COMMAND_TASK CMDID,HPARA fFun);
//设置当前任务寄存器值
// BOOL OS_SetReg(BYTE byRegNo, DWORD dwRegVal);
// BOOL OS_GetReg(BYTE byRegNo, DWORD *pdwRegVal);
// void OS_Sleep(DWORD dwMs);
//系统运行毫秒数
// DWORD OS_GetRunTime(void);

// typedef enum{
// 	HK_RESET,
// 	HK_HALF_SEC
// }HOOK_TYPE;


//TRACE信息相关
// INT DF_sprintf(char *buf, char *format, ...);
#ifdef __cplusplus
 #if __cplusplus
	}
 #endif
#endif
#endif
