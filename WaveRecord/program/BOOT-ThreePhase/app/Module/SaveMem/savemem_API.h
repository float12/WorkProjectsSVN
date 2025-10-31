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
// 取结构地址
#define GET_STRUCT_ADDR( StructName, ItemName )		(WORD)&( ((StructName *)(0))->ItemName )
#define GET_STRUCT_MEM_LEN( StructName, ItemName )		sizeof( ((StructName *)(0))->ItemName )

#if ( MASTER_MEMORY_CHIP == CHIP_24LC256 )
	#define MASTER_MEM_SPACE		((DWORD)32*1024)
#elif ( MASTER_MEMORY_CHIP == CHIP_24LC512 )
	#define MASTER_MEM_SPACE		((DWORD)64*1024)
#endif//#if ( MASTER_MEMORY_CHIP == CHIP_24LC256 )

#if ( THIRD_MEM_CHIP == CHIP_AT45DB161 )
	#define THIRD_MEM_SPACE			((DWORD)2048*1024)
#elif ( THIRD_MEM_CHIP == CHIP_AT45DB321 )
	#define THIRD_MEM_SPACE			((DWORD)4096*1024)
#elif ( THIRD_MEM_CHIP == CHIP_GD25Q64C )
	#define THIRD_MEM_SPACE			((DWORD)8192*1024)
#elif ( THIRD_MEM_CHIP == CHIP_GD25Q32C )
	#define THIRD_MEM_SPACE			((DWORD)4096*1024)
#elif ( THIRD_MEM_CHIP == CHIP_GD25Q16C )
	#define THIRD_MEM_SPACE			((DWORD)2048*1024)
#elif ( THIRD_MEM_CHIP == CHIP_NO )
	#define THIRD_MEM_SPACE			(0)
#endif//#if ( THIRD_MEM_CHIP == CHIP_AT45DB161 )

//单片存储芯片大小 仅参考第一片 如果第二片没有，则把第一片对半分配
#if( SLAVE_MEM_CHIP == CHIP_NO )
	#define SINGLE_CHIP_SIZE		((DWORD)(MASTER_MEM_SPACE/2))
#else
	#define SINGLE_CHIP_SIZE		MASTER_MEM_SPACE
#endif//#if( SLAVE_MEM_CHIP == CHIP_NO )

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//安全空间分配
typedef struct TSafeMem_t
{
	
	TUpdateProgSafeRom 	UpdateProgSafeRom;
}TSafeMem;

#define SAFE_SPACE_START_ADDR					( 0 )
#define GET_SAFE_SPACE_ADDR( Addr_Name )		( (WORD)&(((TSafeMem *)SAFE_SPACE_START_ADDR)->Addr_Name) )


//第一片25640的末端
#define LAST_ADDR_256401			( sizeof(TSafeMem) )
//第二片25640的末端
#define LAST_ADDR_256402			( sizeof(TSafeMem) )


//以下是连续空间分配
//定义第一片连续空间开始地址
#define SINGLE1_CONTINUE_ADDR		(DWORD)LAST_ADDR_256401
//定义第一片连续空间大小
#define SINGLE1_CONTINUE_SIZE		(DWORD)((DWORD)SINGLE_CHIP_SIZE-LAST_ADDR_256401)

//定义第二片连续空间开始地址
#define SINGLE2_CONTINUE_ADDR		(DWORD)LAST_ADDR_256402
//定义第二片连续空间大小
#define SINGLE2_CONTINUE_SIZE		(DWORD)((DWORD)SINGLE_CHIP_SIZE-LAST_ADDR_256402)

#define CONTINUE_TOTAL_SIZE			(DWORD)((DWORD)SINGLE1_CONTINUE_SIZE+(DWORD)SINGLE2_CONTINUE_SIZE)


//连续空间结构定义
typedef struct TConMem_t
{		
    //低功耗连续空间
    TLowPowerConRom		LowPowerConRom;
    
	BYTE FactoryRcvArea[64];

	//电池工作时间，单位：分
	DWORD BatteryTime;
	//掉电时间
	BYTE LostPowerTime[7];

	#if(SEL_AHOUR_FUNC== YES)
	#if( MAX_PHASE != 1 )
    TAHour				AHourConRom;
	#endif	
	#endif//(SEL_AHOUR_FUNC== YES)
}TConMem;

#define CONTINUE_SPACE_START_ADDR		0
#define GET_CONTINUE_ADDR( Addr_Name )		( (DWORD)&(((TConMem *)CONTINUE_SPACE_START_ADDR)->Addr_Name) )




//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 读安全空间,要求长度必须是结构的长度，包括校验在内，
//			而且安全空间的第二份一定是取反存储，校验固定认为是CRC32
//
//参数: 
//			Addr[in]				起始地址
//			Length[in]				数据长度，整个结构的长度，包括校验
//			pBuf[in/out]			存放读出的数据，包括校验
//                    
//返回值:  	TRUE:		有一份对则认为是对
//			FALSE：		两份都不对
//备注:   
//-----------------------------------------------
WORD api_VReadSafeMem( WORD Addr, WORD Length, BYTE * pBuf );

//-----------------------------------------------
//函数功能: 写安全空间,要求长度必须是结构的长度，包括校验在内，
//			而且安全空间的第二份是取反存储,校验固定认为是CRC32，如果校验不对，该函数会自动添加
//
//参数: 
//			Addr[in]				起始地址
//			Length[in]				数据长度，整个结构的长度，包括校验
//			pBuf[in/out]			存放读出的数据，包括校验
//                    
//返回值:  	TRUE:正确写入了两份数据	FALSE:有一份或两份数据没有正确写入
//
//备注:   
//-----------------------------------------------
WORD api_VWriteSafeMem( WORD Addr, WORD Length, BYTE * pBuf );

//-----------------------------------------------
//函数功能: 清安全空间,要求长度必须是结构的长度，包括校验在内，
//			而且安全空间的第二份是取反存储,校验固定认为是CRC32，该函数会自动添加校验
//
//参数: 
//			Addr[in]				起始地址
//			Length[in]				数据长度，整个结构的长度，包括校验
//                    
//返回值:  	TRUE:正确写入了两份数据	FALSE:有一份或两份数据没有正确写入
//
//备注:   
//-----------------------------------------------
WORD api_ClrSafeMem( WORD Addr, WORD Length );

//-----------------------------------------------
//函数功能: 写连续空间函数
//
//参数: 
//			Addr[in]		要写的起始地址
//			Length[in]		要写的长度
//			pBuf[in/out]	写操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:   
//-----------------------------------------------
BOOL api_WriteToContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf);

//-----------------------------------------------
//函数功能: 从连续空间读数据的函数
//
//参数: 
//			Addr[in]		要读的起始地址
//			Length[in]		要读的长度
//			pBuf[in/out]	读操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:   
//-----------------------------------------------
BOOL api_ReadFromContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf);

//-----------------------------------------------
//函数功能: 把大片的连续空间写成0，主要为事件清零等操作使用。不判断是否操作成功
//
//参数: 
//			Addr[in]		要清除的起始地址
//			Length[in]		要清除的长度
//                    
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void api_ClrContinueEEPRom(DWORD Addr, WORD Length);


//-----------------------------------------------
//函数功能: 读连续空间的数据及备份,要求长度必须是结构的长度，包括校验在内，
//			第二份取反存储，校验固定认为是CRC32
//
//参数: 
//			Addr1[in]				第一份在连续空间的起始地址
//			Addr2[in]				第二份在连续空间的起始地址,若为0，则不读第二份
//			Length[in]				数据长度，整个结构的长度，包括校验
//			pBuf[in/out]			存放读出的数据，包括校验
//                    
//返回值:  	TRUE:		有一份对则认为是对
//			FALSE：		两份都不对
//备注:   
//-----------------------------------------------
WORD api_VReadContinueMem( DWORD Addr1, DWORD Addr2, BYTE Length, BYTE * pBuf );

//-----------------------------------------------
//函数功能: 写连续空间的数据及备份,要求长度必须是结构的长度，包括校验在内，
//			第二份数据取反，校验固定认为是CRC32，如果校验不对，该函数会自动添加
//
//参数: 
//			Addr1[in]				第一份在连续空间的起始地址
//			Addr2[in]				第二份在连续空间的起始地址，若为0，则不写第二份
//			Length[in]				数据长度，整个结构的长度，包括校验
//			pBuf[in/out]			存放读出的数据，包括校验
//                    
//返回值:  	TRUE:正确写入了两份数据	FALSE:有一份或两份数据没有正确写入
//
//备注:   
//-----------------------------------------------
BOOL api_VWriteContinueMem( DWORD Addr1, DWORD Addr2, BYTE Length, BYTE * pBuf );

//-----------------------------------------------
//函数功能: 读冻结事件数据区
//
//参数: 
//			Addr[in]		要读的起始地址
//			Length[in]		要读的长度
//			pBuf[in/out]	读操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:配置flash的读flash区域，没有flash的读eeprom中数据   
//-----------------------------------------------
BOOL api_ReadMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf);

//-----------------------------------------------
//函数功能: 写冻结事件数据区
//
//参数: 
//			Addr[in]		要写的起始地址
//			Length[in]		要写的长度
//			pBuf[in/out]	写操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:配置flash的写到flash区域，没有flash的写到eeprom中   
//-----------------------------------------------
BOOL api_WriteMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf);

//-----------------------------------------------
//函数功能: 读所有存储芯片指定地址数据，为厂内功能单独写的函数
//
//参数: 
//			No[in]			指定读哪一片EEPROM
//							0x01：第一片EEPROM
//							0x02：第二片EEPROM
//							0x11：第一片Flash
//							0x21: 第一片铁电
//							0x31: 读连续空间
//			Addr[in]		要读的起始地址（芯片内偏移地址）
//			Length[in]		要读数据的长度
//			pBuf[out]		要读出数据的缓冲
//                    
//返回值:  	读出数据的长度，如果为0，表示失败
//
//备注: 其他模块尽量不要用此函数  
//-----------------------------------------------
WORD api_ReadSaveMem( BYTE No, DWORD Addr, WORD Length, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 管理存储模块的上电初始化工作
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_PowerUpSave( void );


#endif//#ifndef __SAVEMEM_API_H


