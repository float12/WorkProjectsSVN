#ifndef __DEF_H__
#define __DEF_H__

#include "../../../system/__def.h"
///////////////////////////////////////////////////////////////
//	宏 名 称 : 常用宏定义S
//	功    能 : 
//	备    注 : 
//	作    者 : 用电信息软件组
//	时    间 : 2015年7月17日
///////////////////////////////////////////////////////////////
#undef  YES
#define YES					1

#undef	NO					
#define NO					0

#undef  ON
#define ON                  1

#undef  OFF
#define OFF                 0

#undef  NULL
#define NULL				0

#define NONE				0

#define FALSE				0		//错误
#define TRUE				1		//正确

#ifdef _MSC_VER  //MSVC编译器
	#undef	MOS_UCOSII
	#define	MOS_UCOSII		NO
	#define MOS_LINUX		NO
	#define MOS_MSVC		YES

#else
	#define MOS_MSVC		NO
	#define MOS_LINUX		NO
	#define	MOS_UCOSII		YES

#endif

#if(MOS_UCOSII)
#define	SIZE_DEFAULT_STACK	0x1800
#else
#define	SIZE_DEFAULT_STACK	0x2000
#endif

///////////////////////////////////////////////////////////////
//	宏 名 称 : 常用格式转换
//	功    能 : 
//	备    注 : 
//	作    者 : 用电信息软件组
//	时    间 : 2015年7月17日
///////////////////////////////////////////////////////////////
#undef  LOBYTE
#define LOBYTE(w)		(BYTE)(w)   //取一个WORD的低字节
#undef  HIBYTE
#define HIBYTE(w)		(BYTE)((WORD)(w) >> 8) //取一个WORD的高字节
#undef  LOWORD
#define LOWORD(l)		(WORD)(l)  //取一个DWORD的低WORD
#undef  HIWORD
#define HIWORD(l)		(WORD)((DWORD)(l) >> 16)//取一个DWORD的高WORD

#define LLBYTE(w)		((BYTE)(w))  //取一个DWORD的最低字节
#define LHBYTE(w)		((BYTE)((WORD)(w) >> 8))//取一个DWORD的次低字节
#define HLBYTE(w)		((BYTE)((DWORD)(w) >> 16))//取一个DWORD的次高字节
#define HHBYTE(w)		((BYTE)((DWORD)(w) >>24))//取一个DWORD的最高字节

#define ArraySize(a)  (sizeof(a)/sizeof(a[0])) //计算数缓大小

///////////////////////////////////////////////////////////////
//	宏 名 称 : 常用类型定义
//	功    能 : 
//	备    注 : 
//	作    者 : 用电信息软件组
//	时    间 : 2015年7月17日
///////////////////////////////////////////////////////////////
//typedef unsigned long       DWORD;
typedef unsigned long       ULONG;
//typedef int                 BOOL;
//typedef unsigned char       BYTE;
//typedef unsigned short      WORD;
typedef void*			    PVOID;
typedef char*			    PCHAR;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

#ifndef _MSC_VER
typedef long long			DLONG;
typedef unsigned long long	UDLONG;
#else
typedef __int64				DLONG;
typedef __int64				UDLONG;
#endif
#if( MOS_MSVC || MOS_UCOSII)
  #define PACK 	
#elif(MOS_LINUX)
  #define PACK __attribute__ ((packed,aligned(1)))  
#endif
typedef struct _TSA{
	union{
		BYTE	AF;
		struct{
			BYTE	len	 : 4;
			BYTE	log_addr : 2;
			BYTE	type : 2;
		};
	};
	BYTE addr[16];	// 地址域
}TSA;

typedef union _UMeterAddr{
	BYTE addr[6];	// 地址域
	TSA	 addr_698;
}UMeterAddr;

#define INVALID(ptr)		((DWORD)ptr == 0x0))

///////////////////////////////////////////////////////////////
//	宏 名 称 : 常用函数指针类型定义
//	功    能 : 
//	备    注 : 
//	作    者 : 用电信息软件组
//	时    间 : 2015年7月17日
///////////////////////////////////////////////////////////////
typedef void*	HNPARA;  //指针与ID智能识别型，
typedef void*	HPARA;

typedef BOOL (*FB_V  )(void);
typedef BOOL (*FB_D  )(DWORD);
typedef BOOL (*FB_DD )(DWORD,DWORD);
typedef BOOL (*FB_H  )(HPARA);
typedef BOOL (*FB_HH )(HPARA,HPARA);
typedef BOOL (*FB_HD )(HPARA,DWORD);
typedef BOOL (*FB_DHD)(DWORD,HPARA,DWORD);
typedef BOOL (*FB_DDH)(DWORD,DWORD,HPARA);
typedef BOOL (*FB_HDD)(HPARA,DWORD,DWORD);
typedef BOOL (*FB_HDH)(HPARA,DWORD,HPARA);
typedef BOOL (*FB_HHDD)(HPARA,HPARA,DWORD,DWORD);
typedef BOOL (*FB_HHH)(HPARA,HPARA,HPARA);
typedef BOOL (*FB_DHH)(DWORD,HPARA,HPARA);
typedef BOOL (*FB_DHHH)(DWORD,HPARA,HPARA,HPARA);
typedef INT	  (*ID_HHI)(HPARA hPort,HPARA hBuf,int nSize);
typedef DWORD (*FD_V  )(void);
typedef DWORD (*FD_H  )(HPARA);
typedef DWORD (*FD_HH )(HPARA,HPARA);
typedef DWORD (*FD_HHD)(HPARA,HPARA,DWORD);
typedef DWORD (*FD_HD )(HPARA,DWORD);
typedef DWORD (*FD_DH )(DWORD,HPARA);
typedef DWORD (*FD_HDD)(HPARA,DWORD,DWORD);
typedef DWORD (*FD_HDH)(HPARA,DWORD,HPARA);
typedef DWORD (*FD_D)(DWORD);
typedef DWORD (*FD_DD)(DWORD,DWORD);
typedef DWORD (*FD_DDD)(DWORD,DWORD,DWORD);

typedef HPARA (*FH_V  )(void);
typedef HPARA (*FH_H  )(HPARA);
typedef HPARA (*FH_HH )(HPARA,HPARA);
typedef HPARA (*FH_HD )(HPARA,DWORD);
typedef HPARA (*FH_HDD)(HPARA,DWORD,DWORD);
typedef HPARA (*FH_HDH)(HPARA,DWORD,HPARA);

typedef void  (*FV_D  )(DWORD);
typedef void  (*FV_B  )(BOOL);
typedef void  (*FV_V  )(void);
typedef void  (*FV_H  )(HPARA);
typedef void  (*FV_HB )(HPARA,BOOL);
typedef void  (*FV_HH )(HPARA,HPARA);
typedef void  (*FV_HHH)(HPARA,HPARA,HPARA);
typedef void  (*FV_HHHH)(HPARA,HPARA,HPARA,HPARA);
typedef void  (*FV_HD )(HPARA,DWORD);
typedef void  (*FV_HDB)(HPARA,DWORD,BYTE);
typedef void  (*FV_HDS)(HPARA,DWORD,short);
typedef void  (*FV_HDD)(HPARA,DWORD,DWORD);
typedef void  (*FV_HDH)(HPARA,DWORD,HPARA);
typedef void  (*FV_HDHH)(HPARA,DWORD,HPARA,HPARA);
typedef void  (*FV_DD)(DWORD,DWORD);
typedef void  (*FV_DDH)(DWORD,DWORD,HPARA);
typedef void  (*FV_DDD)(DWORD,DWORD,DWORD);
typedef WORD  (*FW_DWHWH)(DWORD,WORD,HPARA,WORD,HPARA);
typedef WORD  (*FW_DHWHWH)(DWORD,HPARA,WORD,HPARA,WORD,HPARA);
typedef WORD  (*FW_DHWHWHH)(DWORD,HPARA,WORD,HPARA,WORD,HPARA,HPARA);
typedef WORD  (*FW_BDHWHWHH)(BYTE,DWORD,HPARA,WORD,HPARA,WORD,HPARA,HPARA);
typedef BOOL  (*FB_WWHH)(WORD,WORD,HPARA,HPARA);
typedef BOOL  (*FB_WWH)(WORD,WORD,HPARA);
typedef void*  (*FPV_PV)(void*);

///////////////////////////////////////////////////////////////
//	宏 名 称 : 常用位操作宏定义
//	功    能 : 
//	备    注 : 
//	作    者 : 用电信息软件组
//	时    间 : 2015年7月17日
///////////////////////////////////////////////////////////////
// #define BIT31		0x80000000
// #define BIT30       0x40000000
// #define BIT29       0x20000000
// #define BIT28       0x10000000
// #define BIT27       0x8000000
// #define BIT26       0x4000000
// #define BIT25       0x2000000
// #define BIT24       0x1000000
// #define BIT23       0x800000
// #define BIT22       0x400000
// #define BIT21       0x200000
// #define BIT20       0x100000
// #define BIT19       0x80000
// #define BIT18       0x40000
// #define BIT17       0x20000
// #define BIT16       0x10000
// #define BIT15       0x8000
// #define BIT14       0x4000
// #define BIT13       0x2000
// #define BIT12       0x1000
// #define BIT11       0x800
// #define BIT10       0x400
// #define BIT9        0x200
// #define BIT8        0x100
// #define BIT7        0x80
// #define BIT6        0x40
// #define BIT5        0x20
// #define BIT4        0x10
// #define BIT3        0x08
// #define BIT2        0x04
// #define BIT1        0x02
// #define BIT0        0x01

#ifndef _Abs
#define _Abs(v)	((v)>0?(v):-(v))
#endif

#endif

