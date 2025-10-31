//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		METER_UPDATE头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __METER_UPDATE_H
#define __METER_UPDATE_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eIDLE = 0,
	eWaitResult,
	eSuc,
}eOpenFactoryStep;
typedef struct
{
	DWORD dwFileSize; //程序文件大小（Byte）
	WORD wAllFrameNo; //程序文件分割后的总帧数  
	WORD wCrc16;	  //整个程序文件的CRC16校验码
	DWORD dwIapFlag;  //升级标志位  DOWNLOAD_METERFILE 无效，0x5AA55AA5 升级监测单元有效，其他无效
	DWORD dwCrc32;	  //结构体校验
} T_Iap_645;

#pragma pack(1)
typedef struct TIap_t
{
	BYTE bDestFile[10];
	BYTE bSourceFile[10];
	DWORD dwFileSize;		// 程序文件大小（Byte）
	BYTE bFileAttr;			// 文件属性
	BYTE bFileVer[10];		// 文件版本
	BYTE bFileType;			// 文件类别
	WORD wFrameSize;		// 传输块大小，默认200字节
	WORD wAllFrameNo;		// 程序文件分割后的总帧数
	WORD wFrameOfOneSector; // 一个扇区可以存放的数据帧数
	WORD LastFrameSize;		//等于0表示最后一帧是完整帧，否则表示最后一帧大小
	
	WORD wCrc16;			// 整个程序文件的CRC16校验码
	WORD wAllUpdate;		// 1:全部升级  0：部分升级
	DWORD dwIapFlag;		// 升级标志位  DOWNLOAD_METERFILE 有效，其他无效
	//模组升级信息
	BYTE CheckResultCnt;
	BYTE AlreadySendExeFrame;//只发一次执行帧
	BYTE IsModuleUpgrade; 	//负荷辨识模组区分普通的698升级
	DWORD dwCrc32;			// 结构体校验
} T_Iap_698;
#pragma pack()

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

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern T_Iap_645 tIap_645;
extern T_Iap_698 tIap_698;
extern BYTE MeterUpgradeTimeUpFlag;
extern char upgrade_file_path_645[FILENAME_LEM];
extern  char upgrade_file_path_698[FILENAME_LEM];
extern BYTE OpenFactoryStep;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//功能描述  : 645规约预处理,只用来判断升级
//
//参数  :    TSerial *p[in]
//
//返回值:     BOOL （TRUE-需要做转发或者报文重组处理）
//备注内容  :  无
//-----------------------------------------------
BOOL Pre_Dlt645(TSerial *p);
void EnterUpgrade698(void);
void ExitUpgrade698(eUpgradeResult result);
BOOL GetUpGradeFileMessage(void);
unsigned short CalCRC16( unsigned char *puchMsg, unsigned short usDataLen );
void ComposeFileFrameLen(BYTE *pBuf, WORD *APDULen);
BOOL ProcResponse698(void);
BOOL ProcUpgradeResponse698(TSerial *p);
void SendOpenFactoryFrame();
#endif //#ifndef __METER_UPDATE_H
