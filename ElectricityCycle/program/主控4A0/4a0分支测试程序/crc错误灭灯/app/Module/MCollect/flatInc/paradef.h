#ifndef _PARADEF_H_
#define _PARADEF_H_
#include "usrtime.h"
typedef struct _THisBuf{
	DWORD  dwType;			//数据类型(含测量点号)
	BYTE Buf[2048];
}THisBuf;

#pragma pack(1)
//基本数据结构
typedef struct _TValTime{
	DWORD dwTime;
	DWORD dwVal;
}TValTime;

typedef struct _TMPTime{
	BYTE	Buf[6];				//测量点时间
}TMPTime;

typedef struct _TXL2{
	DWORD dwVal;
	TMPTime Time;
}TXL2;

typedef struct _TMeterRealCI{
	DWORD dwValidFlag;
	DWORD DD[20];
}TMeterRealCI;

typedef struct _TCVXL{
	DWORD XL[2];
	BYTE  byValidFlag;
}TCVXL;

#pragma pack()

typedef struct _TReadDir{
	char *iszName;				//输入项：文件名
	void *ipvBuf;				//输入项：缓冲区
	DWORD idwBufLen;			//输入项：缓冲区大小
	DWORD idwOffset;			//输入项：文件名偏移量
	DWORD odwReadLen;			//输出项：实际读的长度
}TReadDir;

typedef struct{
	DWORD FileLen;				//输出项：文件长度
	TTime Time;					//输出项：  文件时间
}TFileInfo;

//读文件结构
typedef struct{
	char *iszName;				//输入项：文件名
	void *ipvBuf;				//输入项：缓冲区
	DWORD idwBufLen;			//输入项：缓冲区大小
	DWORD idwOffset;			//输入项：文件偏移量
	DWORD odwReadLen;			//输出项：实际读的长度
}TFileRead;

///////////////////////////////////////////////////////////////
//	函 数 名 : FileRead
//	函数功能 : 读文件
//	处理过程 : 
//	备    注 : 
//	返 回 值 : DWORD
//	参数说明 : TFileRead *pR
///////////////////////////////////////////////////////////////
DWORD FileRead(TFileRead *pR);

typedef struct {
	char *iszName; //输入项：文件名
	void *ipvBuf;  //输入项：缓冲区
	DWORD idwBufLen;//输入项：缓冲区大小
	DWORD idwOffset;//输入项：文件偏移量
}TFileWrite;
///////////////////////////////////////////////////////////////
//	函 数 名 : FileWrite
//	函数功能 : 写文件
//	处理过程 : 
//	备    注 : 
//	返 回 值 : DWORD
//	参数说明 : TFileWrite *pW
///////////////////////////////////////////////////////////////
DWORD FileWrite(TFileWrite *pW);
typedef struct {
	char *iszName; //输入项：文件名
	DWORD idwCleanLen;	//输入项：清除长度(128K块的整数倍)
	DWORD idwOffset;//输入项：文件偏移量(128K块的整数倍)
	BOOL bClearBak;	//清除备份区	
}TFileClean;

// typedef struct{
// 	char *iszName; //输入项：文件名
// 	DWORD odwLen;  //输出项：长度
// }TGetFileLen;
// BOOL GetFileLen(TGetFileLen *pGFL);

void ResetBadBlockTab(void);
DWORD FileClean(TFileClean *pC);
// BOOL GetFileIdlePageAddr(char *szName,DWORD dwOffset,DWORD dwMaxLen,DWORD *pdwIdleOffset);
#endif
