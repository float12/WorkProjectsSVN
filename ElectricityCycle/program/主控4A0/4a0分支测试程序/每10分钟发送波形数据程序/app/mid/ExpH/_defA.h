#ifndef __DEFINE_A_H__
#define __DEFINE_A_H__

#include "mid.h"
#define BYTE	Mid_BYTE
#define WORD    Mid_WORD 
#define SWORD   Mid_SWORD
#define DWORD	Mid_DWORD
#define SWDORD  long long 
#define BOOL	Mid_BOOL
#define	QWORD   Mid_QWORD 
#define SQWORD	Mid_SQWORD

#define TRUE    (1)
#define FALSE   (!TRUE)


#ifndef NULL
    #define NULL        (0)
#endif

#define NULL_PTR    ((void *)NULL)
extern void *pMoudleRAM_01;
//扩展功能A全局变量
typedef struct TMoudleARamData_t	
{
	CommonDataFunction s_GetOADDatfun;
	Mid_QWORD g_RemoteP[3];
	Mid_BYTE s_SysStatus[8 + 1];
	Mid_BYTE s_TimerTask[108 + 1];
	Mid_DWORD CurrTimer[10];
	Mid_DWORD ProxyRequestInfo[10];
}TMoudleARamData;
typedef struct OAD
{
	unsigned char IO_H;
	unsigned char IO_L;
	unsigned char PIID;
	unsigned char Type;
}OAD;
#endif

