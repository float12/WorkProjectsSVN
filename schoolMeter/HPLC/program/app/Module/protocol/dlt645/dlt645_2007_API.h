//------------------------------------------------------------------------------------------
//	名    称：dlt645_2007_API.h
//	功    能:  DL/T645-2007 扩展结构描述文件
//	作    者: 姜文浩
//	创建时间:  2007-11-6 19:54
//	更新时间:
//	备    注:	，把结构提出来
//	修改记录:
//------------------------------------------------------------------------------------------
#ifndef __Dlt645_2007_API_H
#define __Dlt645_2007_API_H

#if( SEL_DLT645_2007 == YES )


//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define MAX_SIZE_FOR_IAP						(256 * 1024)									// 用来升级的空间  
#define NUM_OF_SECTOR_FOR_IAP					128												// 一个扇区4096字节，先预留512K 给升级用
#define LEN_OF_ONE_FRAME						192												// 一帧报文的长度
#define FRAMENUM_OF_ONE_SECTOR					(SECTOR_SIZE / LEN_OF_ONE_FRAME)				//一个扇区能够存的报文数 21
#define FRAMESIZE_OF_ONE_SECTOR					(FRAMENUM_OF_ONE_SECTOR * LEN_OF_ONE_FRAME) 	//一个扇区能够存字节数
#define SECTOR_NUM_FOR_IAP						((MAX_SIZE_FOR_IAP / FRAMESIZE_OF_ONE_SECTOR)+1)// 存MAX_SIZE_FOR_IAP的升级数据，需要多少个扇区
#define IAP_CODE_ADDR							((2048 - SECTOR_NUM_FOR_IAP) * SECTOR_SIZE)		// 扇区号 0~2047
#define IAP_INFO_ADDR							((2048 - SECTOR_NUM_FOR_IAP- 1) * SECTOR_SIZE )	// 扇区号 0~2047 存放升级信息
#define MAX_FRAME_NUM							(MAX_SIZE_FOR_IAP / LEN_OF_ONE_FRAME)			// 帧序号最大值
#define IAP_FLAG								0xA55AA55A


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct
{
	DWORD dwFileSize; //程序文件大小（Byte）
	WORD wAllFrameNo; //程序文件分割后的总帧数
	WORD wCrc16;	  //整个程序文件的CRC16校验码
	DWORD dwIapFlag;  //升级标志位  0xA55AA55A 有效，其他无效
	DWORD dwCrc32;	  //结构体校验
} T_Iap;

#pragma pack(1)
typedef struct TResetDelayPara_t
{
	BYTE	ResetDelay;//延时升级时间(s)
	DWORD	CRC32;
}TResetDelayPara;
#pragma pack()
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern TResetDelayPara ResetDelayPara;
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量


//-----------------------------------------------
//				api函数声明
//-----------------------------------------------
//--------------------------------------------------
//功能描述:断开身份认证
//         
//参数  :   无
//
//返回值:    无   
//         
//备注内容:  无
//--------------------------------------------------
void api_Release645_Auth( void );
//--------------------------------------------------
//功能描述:  计算CRC16校验
//
//参数:
//
//返回值:    CRC16校验
//
//备注内容:
//--------------------------------------------------
unsigned short CalCRC16( unsigned char *puchMsg, unsigned short usDataLen );
//--------------------------------------------------
//功能描述:  计算整个文件的CRC16校验
//
//参数:
//
//返回值:    CRC16校验
//
//备注内容:
//--------------------------------------------------
unsigned short FileCalCRC16(unsigned char *puchMsg, unsigned short usDataLen);


#endif//#if( SEL_DLT645_2007 == YES )

#endif//对应文件开始的 __DLT_645_2007_STRUCT



