//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.11
//描述		存储器大小定义、安全空间和连续空间分配
//修改记录	
//----------------------------------------------------------------------
#ifndef __SAVEMEM_API_H
#define __SAVEMEM_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//自由时间记录次数
#define MAX_FREE_EVENT      256
//最大记录的自由异常事件
#define MAX_SYS_FREE_MSG	512
//最大记录的系统异常信息
#define MAX_SYS_UN_MSG		128

#define SECTOR_SIZE  (4096UL)
#define THIRD_MEM_SPACE                             (8192UL*1024UL)  //8MB,2048个扇区
#define RMN_MEN_SPACE                               (64UL*1024UL)   //512K,  500页
#define HALFRMN_MEM_SPACE                           (RMN_MEN_SPACE/2)  

/***************************************************************************************************
 * 程序缓冲区
 */
#define UPDATEFLAG_CONST            (0x5aa5a55a)    // 升级标志值
#define CODEBUFFERFLAG1_CONST       (0x1111)        // 使用第1份缓冲区
#define CODEBUFFERFLAG2_CONST       (0x2222)        // 使用第2份缓冲区
#define CODEBUFFER_SIZE             (400UL*1024UL)  // 缓冲区最大长度

// 缓冲区开始地址
#define FLASH_UPDATEDATA_START_ADDR             (0UL)
// 第1份缓冲区
#define FLASH_CODEBUFFER1_START_ADDR            FLASH_UPDATEDATA_START_ADDR
#define FLASH_CODEBUFFER1_SIZE                  CODEBUFFER_SIZE
#define FLASH_CODEBUFFER1_END_ADDR              (FLASH_CODEBUFFER1_START_ADDR + FLASH_CODEBUFFER1_SIZE)
// 第2份缓冲区
#define FLASH_CODEBUFFER2_START_ADDR            FLASH_CODEBUFFER1_END_ADDR
#define FLASH_CODEBUFFER2_SIZE                  CODEBUFFER_SIZE
#define FLASH_CODEBUFFER2_END_ADDR              (FLASH_CODEBUFFER2_START_ADDR + FLASH_CODEBUFFER2_SIZE)
// 缓冲区结束地址
#define FLASH_UPDATEDATA_END_ADDR               (FLASH_CODEBUFFER2_END_ADDR + SECTOR_SIZE)
/***************************************************************************************************/

//冻结数据地址
#define FLASH_FREEZE_END_ADDR                       THIRD_MEM_SPACE
#define FLASH_FREEZE_SIZE                           (MAX_MIN_FREEZE_DATA_LEN)
#define FLASH_FREEZE_BASE                           (FLASH_FREEZE_END_ADDR - FLASH_FREEZE_SIZE)

//事件记录数据地址
#define FLASH_EVENT_END_ADDR                        FLASH_FREEZE_BASE
#define FLASH_EVENT_SIZE                            (SUB_EVENT_INDEX_MAX_NUM * SECTOR_SIZE * EVENT_SECTOR_NUM)
#define FLASH_EVENT_BASE                            (FLASH_EVENT_END_ADDR-FLASH_EVENT_SIZE)

#pragma pack(push)
#pragma pack(1)
// 程序升级信息结构体
typedef struct TUpdatePara_t
{
    // 两份缓冲区中的程序长度
    DWORD   CodeBufferLen[2];
    // 两份缓冲区校验码
    WORD    CodeBufferSum[2];
    // 缓冲区使用标志，[0]:正式缓冲区标志，[1]:临时缓冲区标志
    WORD    CodeBufferFlag[2];
    // 升级标志
    DWORD   UpdateFlag;
    // 保留
    DWORD   Reserved[2];
    // 校验
    DWORD   CRC32;

}TUpdatePara;

typedef struct TParaInfo_t
{
	BYTE Para[12];
	DWORD CRC32;              						// 校验
}TParaInfoSafeRom;

typedef struct TQJZLicenseSafeRom_t
{
	BYTE QJZLicense[128];
	DWORD CRC32;              						// 校验
}TQJZLicenseSafeRom;

//定义安全空间成员变量
typedef struct TSafeMem_t
{
	// 程序升级信息结构体，此位置需要和BOOT中保持一致，勿动！！！
	TUpdatePara         UpdatePara;

	TEventSafeRom       EventSafeRom;

	TFreezeSafeRom      FreezeSafeRom;

	TReportSafeRom		ReportSafeRom;

	TProSafeRom  		ProSafeRom;

	BYTE 				ReserveSafeRom[256]; 		//预留空间

	TParaInfoSafeRom	ParaInfoSafeRom;			//自用

	TQJZLicenseSafeRom	QJZLicenseSafeRom;			//千居智license

}TSafeMem;

//自由事件记录格式
typedef struct TFreeEvent_t
{
	//事件类型
	WORD EventType;
	//附加记录
	WORD SubEvent;
	//发生时间，要记录到年：月：日：时：分：秒（5、4、3、2、1、0）
	BYTE EventTime[6];

}TFreeEvent;

//事件记录，自由事件记录
typedef struct TEventSave_t
{
	WORD FreeEventPtr;
	//缓冲
	TFreeEvent FreeEvent[MAX_FREE_EVENT];
}TSysFreeEventConRom;

//异常事件记录格式
typedef struct TSysUNMsg_t
{
	//事件类型
	WORD EventType;
	//发生时间，要记录到年：月：日：时：分：秒（5、4、3、2、1、0）
	BYTE EventTime[6];

}TSysUNMsg;

typedef struct TSaveSysMsg_t
{
	WORD SysUnMsgPoint;
	TSysUNMsg SysUNMsg[MAX_SYS_UN_MSG];
}TSysAbrEventConRom;

//定义连续空间成员变量
typedef struct TConMem_t
{
	TEventConRom        EventConRom;
	//异常信息
	TSysAbrEventConRom 	SysAbrEventConRom;
	//自由事件
	TSysFreeEventConRom SysFreeEventConRom;
}TConMem;

#pragma pack(pop)

//获取结构体成员的空间大小
#define GET_STRUCT_MEM_LEN( StructName, ItemName )	sizeof( ((StructName *)(0))->ItemName )
//获取取结构体成员的地址
//一般结构体的成员
#define GET_STRUCT_ADDR( StructName, ItemName )		((DWORD)&( ((StructName *)(0))->ItemName ))
//安全空间的结构体成员
#define SAFE_SPACE_START_ADDR                       ( 0 )
#define GET_SAFE_SPACE_ADDR( Addr_Name )            ( (DWORD)&(((TSafeMem *)SAFE_SPACE_START_ADDR)->Addr_Name) )
//连续空间的结构体成员
#define CONTINUE_SPACE_START_ADDR                   ((DWORD)sizeof(TSafeMem))
#define GET_CONTINUE_ADDR( Addr_Name )              ( (DWORD)&(((TConMem *)CONTINUE_SPACE_START_ADDR)->Addr_Name) )


BOOL api_VWriteSafeMem( DWORD Addr, WORD Length, BYTE * pBuf );

BOOL api_VReadSafeMem( DWORD Addr, WORD Length, BYTE * pBuf );

BOOL api_WriteContinueMem(DWORD Addr, WORD Length, BYTE * pBuf);

BOOL api_ReadContinueMem(DWORD Addr, WORD Length, BYTE * pBuf);

BOOL api_WriteMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf);

BOOL api_ReadMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf);

void api_ClrContinueEEPRom(DWORD Addr, WORD Length);

WORD api_ReadSaveMem( BYTE No, DWORD Addr, WORD Length, BYTE *pBuf );

#endif//#ifndef __SAVEMEM_API_H


