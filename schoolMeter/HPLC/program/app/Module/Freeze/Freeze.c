//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.8.27
//描述		冻结模块源文件
//修改记录	
//---------------------------------------------------------------------- 

#include "AllHead.h"
#include "Freeze.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define FREEZE_ATTRIBUTE_LEN				(2+4+2)				// 冻结周期，OAD，存储深度
#define	FREEZE_RECORD_NO_OAD				0x00022320			// 冻结记录序号OAD
#define	FREEZE_RECORD_TIME_OAD				0x00022120			// 冻结时间OAD
#define MAX_DEMAND_START_OI					0x1010				// 正向有功最大需量
#define MAX_DEMAND_END_OI					0x10A3				// C相反向视在最大需量
#define ZEROCURRRNT_OAD						0x00040120			// 零线电流OAD
#define CURRRNTVECTORSUM_OAD				0x00060120			// 零序电流OAD
#define VALID_OAD_INDEX						20					// OAD索引的最大值
#define OI4800_04_SIZE						13					// 48000400 一个结构体对应的长度
#define FREEZEBUFFERTIME					10					// 分钟冻结数据缓冲时间

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eGET_FREEZE_DATA,				// 获取数据
	eGET_FREEZE_TIME,				// 获取时间
}eGetType;

typedef enum
{
	eTO_FILL_TYPE,				// 要填充数据的类型
	eNOT_TO_FILL_TYPE,			// 不需填充的类型
	eUNSUPPORTED_TYPE			// 不支持的类型
}eFillorNotType;

typedef enum
{
	eALL_OAD_LEN,				// 所有OAD的长度
	eSELECTED_OAD_LEN			// 选中的OAD的长度
}eOadLenType;

typedef enum
{
	eFREEZE_DATA_ADDR,				// 冻结数据地址
	eFREEZE_DEPTH,					// 冻结数据对应的深度
	eFREEZE_CYCLE,					// 每种冻结对应的周期 分钟为单位
}eLastTimeType;

typedef enum
{
	eFREEZE_RECORDNO,				// 读取冻结记录序号
	eFREEZE_SAVE_DOT,			// 读取冻结记录剩余个数
}eReadFreezeType;


typedef struct TTBinarySchInfo_t
{
	DWORD		dwBaseAddr;			//in 此属性对应数据的首地址
	DWORD		dwDepth;			//in 冻结深度
	DWORD		dwSaveDot;			//in 需要传递剩余的冻结记录个数，进行二分法查找时用到
	DWORD		dwRecordNo;			//in 最近一个点的记录号
	DWORD		Time;				//in 要搜索的时间
	DWORD		*pRecordNo;			//out 返回找到的RecordNo
	DWORD		*pTime;				//out 返回找到的时间
	WORD		wLen;				//in 此属性单个数据的长度
	BYTE		FreezeIndex;		//in 冻结类型

	BYTE 		MinPlan;			//分钟冻结方案
	DWORD 		MinBakAddr;			//分钟冻结备份区 x方案的起始地址
}TBinarySchInfo;

// 没存EEP，每次用的时候计算
typedef struct TSectorInfo_t
{		
	WORD		wPointer;		// 寻找点指针
	DWORD		dwAddr;			// 寻找点地址		
	DWORD		dwSaveSpace;	// 此OAD存储空间大小，分钟冻结有用，其他冻结用于更改关联对象属性表时防越界
}TSectorInfo;

typedef struct TReadFreezeRecordInfo_t
{
	BYTE		FreezeIndex;	// 冻结类型
	DWORD 		RecordNo;		// 冻结记录序列号
	DWORD		dwAddr;			// 整条冻结Flash地址		
	WORD		RecordLen;		//整条冻结数据长度

	BYTE 		MinPlan;		// 分钟冻结方案(只有分钟冻结)
	DWORD 		dwBakAddr;		// 分钟冻结 备份地址	
}TReadFreezeRecordInfo;

typedef struct TCompleteFreezeTime_t
{
	TRealTimer	CompleteFreezeClosingTime;
	TRealTimer	CompleteFreezeLadderClosingTime;
	DWORD CRC32;
}TCompleteFreezeTime;



T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];
BYTE minFreezeFlag;
BYTE SecondBufferTime;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];
BYTE minFreezeFlag;
BYTE SecondBufferTime;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//这张表的顺序要和eFreezeType里面定义的顺序一致
const TFreezeInfoTab	FreezeInfoTab[] = 
{
	{0x5005,	eFREEZE_CLOSING,			MAX_CLOSING_FREEZE_ATTRIBUTE,		MAX_CLOSING_FREEZE_DATA_LEN,		MAX_CLOSING_FREEZE_E2_DEPTH,},	//结算日冻结
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{0x500a,	eFREEZE_TARIFF_RATE_CHG,	MAX_TARIFF_RATE_FREEZE_ATTRIBUTE,	MAX_TARIFF_RATE_FREEZE_DATA_LEN,	0,							},	//费率电价切换
	{0x500b,	eFREEZE_LADDER_CHG,			MAX_LADDER_FREEZE_ATTRIBUTE,		MAX_LADDER_FREEZE_DATA_LEN,			0,							},	//阶梯切换 0X0B
	{0x5011,	eFREEZE_LADDER_CLOSING,		MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE,MAX_LADDER_CLOSING_FREEZE_DATA_LEN,0,							},	//阶梯结算
	#endif
	{0x5008,	eFREEZE_TIME_ZONE_CHG,		MAX_TIME_ZONE_FREEZE_ATTRIBUTE,		MAX_TIME_ZONE_FREEZE_DATA_LEN,		0,							},	//时区表切换
	{0x5009,	eFREEZE_DAY_TIMETABLE_CHG,	MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE,	MAX_DAY_TIMETABLE_FREEZE_DATA_LEN, 	0,							},	//日时段表切换
	{0x5000,	eFREEZE_INSTANT,			MAX_INSTANT_FREEZE_ATTRIBUTE,		MAX_INSTANT_FREEZE_DATA_LEN,		0,							},	//瞬时冻结	
	{0x5003,	eFREEZE_HOUR,				MAX_HOUR_FREEZE_ATTRIBUTE,			MAX_HOUR_FREEZE_DATA_LEN,			0,							},	//小时冻结
	{0x5006,	eFREEZE_MON,				MAX_MON_FREEZE_ATTRIBUTE,			MAX_MON_FREEZE_DATA_LEN,			0,							},	//月冻结	
	{0x5004,	eFREEZE_DAY,				MAX_DAY_FREEZE_ATTRIBUTE,			MAX_DAY_FREEZE_DATA_LEN,			MAX_DAY_FREEZE_E2_DEPTH,	},	//日冻结	
	{0x5002,	eFREEZE_MIN,				MAX_MINUTE_FREEZE_ATTRIBUTE,		MAX_MIN_FREEZE_DATA_LEN,			0,							}	//分钟冻结
};

static	DWORD	FreezeFlag;			// 冻结标志上电用

static __no_init WORD	wInstantFreezeTimer;// 瞬时冻结延时时间(秒)
static __no_init WORD	wInstantFreezeTimerBak;// 瞬时冻结延时时间(秒) 取反，作为效验 上电用
__no_init TFreezeDataInfo  MinInfo[MAX_FREEZE_PLANNUM];	// 分钟冻结相关信息
static __no_init TRealTimer	FreezePowerDownTimeBak;	//上电补冻标志，防止异常复位重复补冻
static DWORD PowerUpFreezeMin;//上电补冻时间用
static __no_init TCompleteFreezeTime	CompleteFreezeTime;	//上次补冻结时间
static const BYTE bConstTempBuf[OI4800_04_SIZE]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static WORD BinSearchByTime( TBinarySchInfo *pBinSchInfo );
void SaveFlashMemData(DWORD dwRecordNo, BYTE Type, BYTE* pBuf);
void InitFlashMem(void);
BOOL RepairFreezeUseFlash(void);
BOOL RefreshFreezeUseFlash( DWORD dwRecordNo );

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 获取数据存储的相关信息
//
//参数: 
// 			Len[in]:	对分钟冻结--4字节时间+OAD的长度，其他冻结--整条记录的长度（包括时标）
// 			RecordDepth[in]:	此OAD的深度
// 			inRecordNo[in]:	此OAD数据的RecordNo
// 			pSectorInfo[out]:指向SectorInfo的指针
//返回值:
//	无
//
//备注: 
//-----------------------------------------------
static BOOL GetSectorInfo( WORD Len, WORD RecordDepth, DWORD inRecordNo, TSectorInfo *pSectorInfo )
{
	WORD twSectorNo;			// 此OAD占用的扇区个数	
	WORD twOadNoPerSector;		// 一个扇区能存放的OAD的条数
	WORD twSectorOff;			// 扇区的偏移
	WORD twInSectorOff;			// 扇区内的偏移
	
	if( (Len<4) || (RecordDepth==0) ||(Len > 2000) )
	{
		return FALSE;
	}
	else
	{

		// 一个扇区存的OAD的个数
		twOadNoPerSector = SECTOR_SIZE/Len;
		if( RecordDepth < twOadNoPerSector )
		{
			twOadNoPerSector = RecordDepth;
		}	
		// 此OAD占扇区个数
		if( (RecordDepth%twOadNoPerSector) == 0 )
		{
			twSectorNo = (RecordDepth/twOadNoPerSector)+1;
		}
		else
		{
			twSectorNo = ((RecordDepth/twOadNoPerSector)+1)+1;	// 进位加1
		}
		// 扇区的偏移
		twSectorOff = inRecordNo/twOadNoPerSector;
		twSectorOff %= twSectorNo;
		// 扇区内的偏移
		twInSectorOff = inRecordNo%twOadNoPerSector;

		pSectorInfo->wPointer = twSectorOff*twOadNoPerSector+twInSectorOff;
		pSectorInfo->dwSaveSpace = SECTOR_SIZE*twSectorNo;
		pSectorInfo->dwAddr = (DWORD)twSectorOff*SECTOR_SIZE+twInSectorOff*Len;	

		return TRUE;
	}
}

//-----------------------------------------------
//函数功能:     设置冻结标志
//
//参数: 
//			FreezeNo[in]	冻结枚举号
//                    
//返回值:  	TRUE:设置成功	 FALSE:设置失败
//
//备注:   
//-----------------------------------------------
BOOL SetFreezeFlag(BYTE FreezeNo )
{
    if( FreezeNo > eFREEZE_MAX_NUM )
    {
        return FALSE;
    }

    FreezeFlag |= (0x0001<<FreezeNo);
    
    return TRUE;
}

//-----------------------------------------------
//函数功能:     清零冻结标志
//
//参数: 
//			FreezeNo[in]	冻结枚举号
//                    
//返回值:  	TRUE:设置成功	 FALSE:设置失败
//
//备注:   
//-----------------------------------------------
BOOL ClrFreezeFlag(BYTE FreezeNo )
{
    if( FreezeNo > eFREEZE_MAX_NUM )
    {
        return FALSE;
    }
    
    FreezeFlag &= ~(0x0001<<FreezeNo);
    
    return TRUE;
}



//-----------------------------------------------
//函数功能: 初始化分钟冻结信息
//
//参数:   无
//                    
//返回值: 无
//			
//备注:
//-----------------------------------------------
static void InitMinFreezeInfo( void )
{
	BYTE i;

	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		MinInfo[i].RecordNo = 0;
		MinInfo[i].SaveDot = 0;
		MinInfo[i].CRC32 = lib_CheckCRC32((BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo)-sizeof(DWORD));

		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] );
	}

	#if (SLE_THIRD_MEM_BACKUP == YES)
	//初始化冻结备份
	InitFlashMem();
	#endif
}
//-----------------------------------------------
//函数功能: 检查分钟冻结指针等数据
//
//参数:   无
//                    
//返回值: 无
//			
//备注:
//-----------------------------------------------
static BOOL CheckMinFreezeInfo( void )
{
	BYTE i,Result;
	WORD MinUpTimeFlag;
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;

	MinUpTimeFlag=0;
	Result = TRUE;
	
	//检查MinInfo正确性
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		// 检查RAM的校验
		if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == TRUE )
		{
			continue;
		}
		MinUpTimeFlag |= (0x0001<<i);
	
		// 从EEP恢复
		if(api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] )== TRUE )
		{
			continue;
		}
		
		Result = FALSE;

		memset( (BYTE *)&MinInfo[i], 0x00, sizeof(TFreezeDataInfo) );
		MinInfo[i].CRC32 = lib_CheckCRC32((BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo)-sizeof(DWORD));
	}
	
	if(MinUpTimeFlag != 0)
	{
		//读出更改时间标志
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
		FreezeUpdateTimeFlag.wMinFlag |= MinUpTimeFlag;
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
	}

	return Result;
}
//-----------------------------------------------
//函数功能: 保存分钟冻结指针等数据
//
//参数: 	无
//	
//返回值:	无
//
//备注:	 每1小时调用一次，掉电保存一次
//-----------------------------------------------
void SaveMinFreezePoint( void )
{

	BYTE i;
	BYTE Buf[sizeof(MinInfo)+sizeof(DWORD)];
	//检查分钟冻结指针等数据
	CheckMinFreezeInfo();
	
	memcpy(Buf,&MinInfo[0],sizeof(MinInfo));
	// 写EEP
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[0] ), sizeof(MinInfo)+sizeof(DWORD), Buf );
	
}

//-----------------------------------------------
//函数功能: 获取OI对应的冻结枚举类型
//
//参数: 
//			OI[in]:	冻结OI
//                    
//返回值:	OI对应的枚举类型
//			
//备注:
//-----------------------------------------------
BYTE api_GetFreezeIndex(WORD OI)
{
	BYTE i;
	
	for(i=0;i<sizeof(FreezeInfoTab)/sizeof(TFreezeInfoTab);i++)
	{
		if( OI == FreezeInfoTab[i].OI )
		{
			break;
		}
	}
	
	if( i == sizeof(FreezeInfoTab)/sizeof(TFreezeInfoTab) )
	{
		ASSERT(0,0);
	}
	
	return i;
}


//-----------------------------------------------
//函数功能: 获取Type对应的属性地址，数据地址，最多属性个数，最大数据长度
//
//参数: 
//			inFreezeIndex[in]:		冻结类型 eFreezeType
//			pFreezeAddrLen[out]:    wAllInfoAddr
//									wDataInfoEeAddr
//									dwAttOadEeAddr
//									dwAttCycleDepthEeAddr	
//									dwDataAddr          
//返回值:	TRUE/FALSE
//			
//备注:
//-----------------------------------------------
BOOL GetFreezeAddrInfo( BYTE inFreezeIndex, TFreezeAddrLen *pFreezeAddrLen )
{
	BYTE i,FreezeIndex;
		 
	FreezeIndex = inFreezeIndex;

	if( FreezeIndex > eFREEZE_MAX_NUM )
	{
		ASSERT(0,0);
	}
	
	if( FreezeIndex >= eFREEZE_MAX_NUM )
	{
		FreezeIndex = eFREEZE_CLOSING;
		pFreezeAddrLen->wAllInfoAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeAllInfoRom[eFREEZE_CLOSING] );
	    pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Closing );
		pFreezeAddrLen->dwAttCycleDepthEeAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttCycleDepthRom.Freeze[FreezeIndex] );
		pFreezeAddrLen->wDataInfoEeAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.DataInfo[FreezeIndex] );
		
	
		pFreezeAddrLen->dwDataAddr = FLASH_FREEZE_BASE+(DWORD)FreezeIndex*FLASH_SECTOR_NUM*SECTOR_SIZE;

		
        return FALSE;
	}

	pFreezeAddrLen->wAllInfoAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeAllInfoRom[FreezeIndex] );
	
	if( FreezeIndex == eFREEZE_INSTANT )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Instant );	
	}
	else if( FreezeIndex == eFREEZE_MIN )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Min );
	}
	else if( FreezeIndex == eFREEZE_HOUR )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Hour );
	}
	else if( FreezeIndex == eFREEZE_DAY )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Day );
	
	}
	else if( FreezeIndex == eFREEZE_MON )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Mon );
	}
	else if( FreezeIndex == eFREEZE_TIME_ZONE_CHG )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.TimeZone );
	}
	else if( FreezeIndex == eFREEZE_DAY_TIMETABLE_CHG )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.DayTimeTab );
	}
	#if( PREPAY_MODE == PREPAY_LOCAL )
	else if( FreezeIndex == eFREEZE_TARIFF_RATE_CHG )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.TariffRateChg );	
	}
	else if( FreezeIndex == eFREEZE_LADDER_CHG )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.LadderChg );
	}
	else if( FreezeIndex == eFREEZE_LADDER_CLOSING )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.LadderClosing );	
	}
	#endif
	else if( FreezeIndex == eFREEZE_CLOSING )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Closing );
	
	}
	else
	{
		ASSERT(0,0);
		return FALSE;
	}

	if( FreezeIndex == eFREEZE_MIN )
	{
		pFreezeAddrLen->dwAttCycleDepthEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttCycleDepthRom.Min[0] );
		pFreezeAddrLen->wDataInfoEeAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[0] );

	}
	else
	{
		pFreezeAddrLen->dwAttCycleDepthEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttCycleDepthRom.Freeze[FreezeIndex] );
		pFreezeAddrLen->wDataInfoEeAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.DataInfo[FreezeIndex] );

	}
	pFreezeAddrLen->dwDataAddr = FLASH_FREEZE_BASE; 
	for(i=0;i<FreezeIndex;i++)
	{
		pFreezeAddrLen->dwDataAddr += FreezeInfoTab[i].dwDataLen;
	}


	return TRUE;
}
//--------------------------------------------------
//功能描述:得到深度，重新计算大小
//
//参数: Len[in]  每一条数据长度
//		Space[in/out]  给定的存储空间大小	
//
//返回值: 冻结深度， 举例：Len = 100， Space = 8192，return 40
//		  两个扇区可以存储80条，但是深度是40,两个扇区保证的最大有效条数为40
//
//备注: 冻结放到flash中，至少占用两个扇区，保证一个扇区被擦除后，
//		还有一个扇区中的数据是有效数据
//--------------------------------------------------
static DWORD GetDepthRecaluSpace(DWORD Len, DWORD *Size)
{
	TSectorInfo	SectorInfo;
	DWORD Depth;
	DWORD NumPerSector;
	DWORD Space = Size[0];

	if(Space < (2*SECTOR_SIZE))
	{
		Space = 2 * SECTOR_SIZE;
	}
	NumPerSector = SECTOR_SIZE / Len;
	Depth = ((Space / SECTOR_SIZE) - 1) * NumPerSector;

	GetSectorInfo(Len, Depth, Depth, &SectorInfo);
	Size[0] = SectorInfo.dwSaveSpace;

	return Depth;
}
//--------------------------------------------------
//功能描述:分钟冻结备份信息处理
//
//参数:		Addr[in]	分钟冻结主数据区 起始地址
//			MainMem[in] 分钟冻结主存储区的 冻结周期以及深度
//			BakMem[out]	分钟冻结备份区的 冻结周期以及深度
//			pFreezeMinInfo[in/out] 
//						DataBakAddr[out]  分钟冻结备份区各个方案的起始地址
//返回值:无
//
//备注: 1、 主数据区 有效方案冻结空间占 整个有效方案空间的比例映射到 整个备份区
//		2、 DealMinFreezeInfo只要不返回错误，DealMinBakFreezeInfo不会有错
//--------------------------------------------------
void DealMinBakFreezeInfo(DWORD Addr, TFreezeAttCycleDepth* MainMem, TFreezeAttCycleDepth* BakMem, TFreezeMinInfo* pFreezeMinInfo)
{
	DWORD PlanSaveSpace[MAX_FREEZE_PLANNUM];		//主存储区 各个方案内存大小
	DWORD PlanBakSpace, PlanBakDepth, PlanBakOffset, PlanBakRemainSpace;
	DWORD AllPlanSumSpace;		
	BYTE i, ValidPlanNum;
	TSectorInfo	SectorInfo;	
    
	Addr = Addr + MAX_MIN_FREEZE_DATA_LEN;	//备份空间的起始地址

	//计算主存储区 每个方案所占空间的大小，以及所有有效方案总空间大小
	ValidPlanNum = 0;
    AllPlanSumSpace = 0;
	for(i = 0; i < MAX_FREEZE_PLANNUM; i++)
	{
		if( MainMem[i].Depth == 0 )	//主存储区 i方案的分配空间为0，该方案没有使用
		{
			continue;
		}
		ValidPlanNum++;	//有效方案
		GetSectorInfo(pFreezeMinInfo->AllAttDataLen[i], MainMem[i].Depth, MainMem[i].Depth, &SectorInfo);
		PlanSaveSpace[i] = SectorInfo.dwSaveSpace;
		AllPlanSumSpace += PlanSaveSpace[i];	//主存储区 有效方案内存和
	}

	//计算备份区 每个方案所占空间的大小，以及方案地址
	memcpy(BakMem, MainMem, sizeof(TFreezeAttCycleDepth) * MAX_FREEZE_PLANNUM);
	PlanBakOffset = 0;
	PlanBakRemainSpace = MAX_MINBAK_FREEZE_DATA_LEN;
	for(i = 0; i < MAX_FREEZE_PLANNUM; i++)
	{
		if( MainMem[i].Depth == 0 )	//主存储区 i方案的分配空间为0，该方案没有使用
		{
			continue;
		}
		ValidPlanNum--;
		pFreezeMinInfo->DataBakAddr[i] = Addr + PlanBakOffset;

		//主冻结区 每个方案占 所有方案空间的比例， 在备份区基本保持不变
		PlanBakSpace = (DWORD)(MAX_MINBAK_FREEZE_DATA_LEN * ((float)PlanSaveSpace[i] / AllPlanSumSpace));
		PlanBakDepth = GetDepthRecaluSpace(pFreezeMinInfo->AllAttDataLen[i], &PlanBakSpace);
		
		//保证剩余的 其他的方案至少有两个扇区
		if(PlanBakRemainSpace  < (PlanBakSpace + 2*SECTOR_SIZE*ValidPlanNum))
		{
			PlanBakSpace = PlanBakRemainSpace - (2 * SECTOR_SIZE * ValidPlanNum);
			PlanBakDepth = GetDepthRecaluSpace(pFreezeMinInfo->AllAttDataLen[i], &PlanBakSpace);
		}

		//地址超限时，将深度置为0，写备份flash时，判断深度为0时，不在写flash
		if((pFreezeMinInfo->DataBakAddr[i] + PlanBakSpace) > (FLASH_FREEZE_END_ADDR))
		{
			PlanBakSpace = 0;
			PlanBakDepth = 0;
		}

		BakMem[i].Depth = PlanBakDepth;
		PlanBakOffset += PlanBakSpace;
		PlanBakRemainSpace -= PlanBakSpace;
	}
}
//-----------------------------------------------
//函数功能: 分钟冻结存储信息计算
//
//参数: 
//			OADNum[in]:				分钟冻结OAD个数 
//			pAttOad[in]:			关联对象属性表           
//  		pAttCycleDepth[in]: 	周期深度
//			MinAddr[in]:			存储地址
//			pFreezeAllInfoRom[in]:	方案长度信息                   
//                    
//返回值:		TRUE/FALSE
//
//备注:   添加删除所有数据
//-----------------------------------------------
WORD DealMinFreezeInfo( BYTE OADNum, TFreezeAttOad*	pAttOad, TFreezeAttCycleDepth* pAttCycleDepth, DWORD MinAddr,TFreezeMinInfo* pFreezeMinInfo )
{
	BYTE i,PlanIndex;
	WORD wLen;
	DWORD AllSaveSpace;
	TSectorInfo	SectorInfo;

	memset( pFreezeMinInfo, 0x00, sizeof(TFreezeMinInfo) );//清空分钟冻结地址信息
	pFreezeMinInfo->DataAddr[0] = MinAddr;//方案0 固定为分钟冻结初始地址
	AllSaveSpace = 0;

	for( i = 0; i < OADNum; i++ )
	{
		PlanIndex = CURR_PLAN( pAttOad[i].Oad );//获取方案号
		wLen = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&pAttOad[i].Oad, 0 );
		if( wLen == 0x8000 )
		{
			return FALSE;
		}

		pAttOad[i].AttDataLen = wLen;
		
		if( pFreezeMinInfo->AllAttDataLen[PlanIndex] == 0 )//第一次长度需要加4-代表时间
		{
			pFreezeMinInfo->AllAttDataLen[PlanIndex] = 4;//更新方案总长度
		}
		
		pAttOad[i].Offset = pFreezeMinInfo->AllAttDataLen[PlanIndex];//更新偏移，偏移不包括当前OAD的长度
		pFreezeMinInfo->AllAttDataLen[PlanIndex] += wLen;//更新方案总长度

	}
	for(i = 0; i < MAX_FREEZE_PLANNUM; i++)
	{
		if( pFreezeMinInfo->AllAttDataLen[i] != 0 )
		{
			//分钟冻结主数据区 4字节时标 + 数据 + 4字节CRC
			//		   备份区 4字节时标 + 数据 + 4字节记录序号
			//主数据区CRC不对，读备份。备份记录号与读的记录序号不相等，数据不对
			pFreezeMinInfo->AllAttDataLen[i] += 4;//更新方案总长度
		}
	}

	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		if( pAttCycleDepth[i].Depth == 0 )//周期为0代表无此方案
		{
			continue;
		}
		else
		{
			//获取方案的空间
			if(GetSectorInfo( pFreezeMinInfo->AllAttDataLen[i], pAttCycleDepth[i].Depth, 0, &SectorInfo ) == FALSE)
			{
				return FALSE;
			}
		}
		
		if( i != 0 )//方案0只判断空间是否够就可以 不需要计算地址
		{
			pFreezeMinInfo->DataAddr[i] = (pFreezeMinInfo->DataAddr[0]+AllSaveSpace);
		}

		AllSaveSpace += SectorInfo.dwSaveSpace;
		
		if( AllSaveSpace > FreezeInfoTab[eFREEZE_MIN].dwDataLen )//判断当前方案空间是否超限
		{
			return FALSE;
		}	
		
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 添加冻结关联对象属性
//
//参数: 
//			OI[in]:		0x5000~0x500B,0x5011 冻结类的对象标识OI  
//			pOad[in]:	指向OAD Buffer的指针  2字节冻结周期+4字节OAD+2字节存储深度                    
//  		OadNum[in]: 批量添加OAD的个数
//                    
//返回值:	TRUE/FALSE
//
//备注:   添加删除所有数据
//-----------------------------------------------
BOOL api_AddFreezeAttribue( WORD OI, BYTE *pOad ,BYTE OadNum )
{	
	BYTE i,j,AttNum,tFreezeIndex,PlanIndex;
	WORD tw,tCycle,tRecordDepth,wLen,Result;
	DWORD dwLen,dwOad;
	TSectorInfo	SectorInfo;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//保证比存储在ee中的数据大，有地方存放CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth	MinBakAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TFreezeMinInfo FreezeMinInfo;
	
	memset( (BYTE *)&AttOad[0], 0x00, sizeof(AttOad) );
	memset( (BYTE *)&FreezeMinInfo, 0x00, sizeof(FreezeMinInfo) );

	tFreezeIndex = api_GetFreezeIndex( OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}
	
	// 获得各种属性地址
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return FALSE;
	}

	//读出单条冻结的长度及关联属性的个数
	if( api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo) != TRUE )
	{
		return FALSE;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return FALSE;
	}

	//读出所有的关联属性对象
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);
					
	//若是分钟冻结，要求下发OAD的深度周期与同方案一致
	if( tFreezeIndex == eFREEZE_MIN )	
	{
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
		}
	}
	else
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );
	}
	AttNum = FreezeAllInfo.NumofOad;
	
	//将要添加的所有OAD逐个进行判断
	for(i=0; i<OadNum; i++)
	{	
		//OAD
		memcpy( (BYTE *)&dwOad, (pOad+2+FREEZE_ATTRIBUTE_LEN*i), sizeof(DWORD) );
		//周期
		memcpy( (BYTE *)&tCycle, pOad+i*FREEZE_ATTRIBUTE_LEN, 2);
		//深度
		memcpy( (BYTE *)&tRecordDepth, pOad+6+i*FREEZE_ATTRIBUTE_LEN, 2);
		//设置的关联对象属性表中不允许周期深度为0
		//周期性冻结不允许周期为0
		if((tFreezeIndex > eFREEZE_HOUR) && (tCycle == 0x0000) )	
		{
			return FALSE;
		}
		//设置的关联对象属性表中不允许深度为0
		if(tRecordDepth == 0x0000)
		{
			return FALSE;
		}
		//取方案，分钟冻结其他冻结置0
		if(tFreezeIndex == eFREEZE_MIN)
		{
			PlanIndex = CURR_PLAN(dwOad);
		}
		else
		{
			PlanIndex =0;
		}
		//判断此方案的周期深度不为零
		if((AttCycleDepth[PlanIndex].Cycle == 0x0000) && ((AttCycleDepth[PlanIndex].Depth == 0x0000)))
		{
			AttCycleDepth[PlanIndex].Cycle = tCycle;
			AttCycleDepth[PlanIndex].Depth = tRecordDepth;
		}
		else
		{
			//如果与同方案的周期深度不同返回FALSE
			if( (tCycle!=AttCycleDepth[PlanIndex].Cycle) || (tRecordDepth!=AttCycleDepth[PlanIndex].Depth) )	
			{
				return FALSE;
			}
		}
		//检查是否有和之前的OAD重复的
		if( AttNum != 0 )
		{
			for( j=0; j<AttNum; j++ )
			{
				// 不允许有相同的OAD
				if( AttOad[j].Oad == dwOad )
				{
					break;
				}
			}
			
			if( j != AttNum )
			{
				continue;
			}	
		}			
		
		//判断下发的OAD是否正确
		wLen = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return FALSE;
		}
				
		//填充此属性数据的长度		
		AttOad[AttNum].Oad = dwOad;
								
		if( tFreezeIndex != eFREEZE_MIN )//分钟冻结 空间单独判断
		{
			AttOad[AttNum].AttDataLen = wLen;	
			
			if( AttNum == 0 )	
			{
				FreezeAllInfo.AllAttDataLen = 4+wLen+4;	//时标+数据长度，加4字节CRC数据校验---李丕凯200407
				AttOad[0].Offset = 4;					//前面是时标	
			}
			else
			{
				//更新一条冻结的数据总长度（包括时标） 分钟冻结不用
				FreezeAllInfo.AllAttDataLen += wLen;
				
				//计算此属性数据在整条冻结的偏移地址
				AttOad[AttNum].Offset = AttOad[AttNum-1].Offset+AttOad[AttNum-1].AttDataLen;
			}
		}		

		AttNum++;
		
		if( AttNum > FreezeInfoTab[tFreezeIndex].MaxAttNum )
		{
			return FALSE;
		}
	}

	if( tFreezeIndex != eFREEZE_MIN )//分钟冻结在此之前已经判断储存长度
	{
		if( FreezeAllInfo.AllAttDataLen > 4096 )//不允许跨扇区写
		{
			return FALSE;
		}
		//日冻结数据长度受E2空间大小的限制,长度减4为减去Ee中存储的冻结序列号的长度
		if((tFreezeIndex == eFREEZE_DAY) && (FreezeAllInfo.AllAttDataLen > (MAX_DAY_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
		{
			return FALSE;
		}
		//结算日冻结数据长度受E2空间大小的限制,长度减4为减去Ee中存储的冻结序列号的长度
		if((tFreezeIndex == eFREEZE_CLOSING) && (FreezeAllInfo.AllAttDataLen > (MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
		{
			return FALSE;
		}
		if(GetSectorInfo( FreezeAllInfo.AllAttDataLen, AttCycleDepth[0].Depth, AttCycleDepth[0].Depth, &SectorInfo ) == FALSE )
		{
			return FALSE;  
		}

        if( SectorInfo.dwSaveSpace > FreezeInfoTab[tFreezeIndex].dwDataLen )
        {
            ASSERT(0,1);
            return FALSE;           
        }
	}
	else
	{
		Result = DealMinFreezeInfo( AttNum, (TFreezeAttOad*)&AttOad, (TFreezeAttCycleDepth*)&AttCycleDepth, FreezeAddrLen.dwDataAddr, &FreezeMinInfo );
		if( Result == FALSE )
		{
			return FALSE;
		}
		//处理分钟冻结备份区的 存储信息：备份区的存储深度，以及数据地址
		DealMinBakFreezeInfo(FreezeAddrLen.dwDataAddr, AttCycleDepth, MinBakAttCycleDepth, &FreezeMinInfo);
	}
	
	//有新的OAD添加
	if( AttNum > FreezeAllInfo.NumofOad )
	{				
		//保存属性对象表OAD
		api_VWriteSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0] );	
		//保存新添加的属性对象表 深度 周期
		if( tFreezeIndex == eFREEZE_MIN )
		{
			for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
			{
				api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
				api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (MAX_FREEZE_PLANNUM + i), sizeof(TFreezeAttCycleDepth), (BYTE *)&MinBakAttCycleDepth[i] );
			}
			//更新分钟冻结信息
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo );	
		}
		else if( FreezeAllInfo.NumofOad == 0 )
		{				
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );	
		}
			
		//更新对象表属性个数
		FreezeAllInfo.NumofOad = AttNum;
		//读出单条冻结的长度及关联属性的个数
		api_VWriteSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
		
		//清指针
		if( tFreezeIndex == eFREEZE_MIN )
		{
			InitMinFreezeInfo();
		}
		else
		{
			api_ClrSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo) );			
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 删除冻结关联对象属性
//
//参数: 
//			OI[in]:		0x5000~0x500B,0x5011 冻结类的对象标识OI  
//			Type[in]:	eDELETE_ATT_BY_OAD--删除某个对象 	eDELETE_ATT_TYPE--删除所有对象  
//			pOad[in]:	要删除的OAD,属性标识及特征的bit5~bit7代表各类冻结的冻结方案
//                    
//返回值:	TRUE/FALSE
//
//备注:   删掉一个OAD，所有的重新计，重新分配EEP空间
//-----------------------------------------------
BOOL api_DeleteFreezeAttribue( WORD OI, BYTE Type, BYTE *pOad )
{	
	BYTE i,j,tFreezeIndex,PlanIndex;
	WORD wLen,Result;
	DWORD dwOad;
	TSectorInfo	SectorInfo;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//保证比存储在ee中的数据大，有地方存放CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth	MinBakAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TFreezeMinInfo FreezeMinInfo;

	memset( (BYTE *)&AttOad[0], 0x00, sizeof(AttOad) );
	memset( (BYTE *)&AttCycleDepth[0], 0x00, sizeof(AttCycleDepth) );
	memset( (BYTE *)&FreezeMinInfo, 0x00, sizeof(FreezeMinInfo) );
	
	tFreezeIndex = api_GetFreezeIndex( OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}

	// 获得各种属性地址
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return FALSE;
	}
	
	//读出单条冻结的长度及关联属性的个数
	if( api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo) != TRUE )
	{
		return FALSE;
	}
	
	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return FALSE;
	}

	if( Type == eDELETE_ATT_BY_OAD )
	{
		memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD) );

		//读出所有的关联属性对象 Oad
		api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);	
		//读出所有的关联属性对象 深度周期
		if( tFreezeIndex == eFREEZE_MIN )
		{
			for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
			{
				api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
			}
		}
		else
		{
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );
		}

		for( i=0; i<FreezeAllInfo.NumofOad; i++ )
		{
			// 判断要删除的对象在对象表中是否存在，若存在删除，并将后面的对象前移
			if( AttOad[i].Oad == dwOad )
			{
				for( j=i; j<(FreezeAllInfo.NumofOad-1); j++ )
				{
					memcpy( (BYTE *)&AttOad[j], (BYTE *)&AttOad[j+1], sizeof(TFreezeAttOad) );	
				}
				
				memset( (BYTE *)&AttOad[FreezeAllInfo.NumofOad-1], 0x00, sizeof(TFreezeAttOad) );
				break;
			}
		}

		if( i == FreezeAllInfo.NumofOad )
		{
			return FALSE;
		}

		if( FreezeAllInfo.NumofOad == 0 )//在进行FreezeAllInfo.NumofOad--之前进行超限判断-jwh
		{
            return FALSE;
		}
		
		FreezeAllInfo.NumofOad--;

		//重新算每个OAD的地址
		for( i=0; i<FreezeAllInfo.NumofOad; i++ )
		{
			// 一种冻结方案里的OAD具有唯一性
			dwOad = AttOad[i].Oad;
			wLen = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )
			{
				return FALSE;
			}
			
			if( tFreezeIndex != eFREEZE_MIN )//分钟冻结 单独计算
			{
				AttOad[i].AttDataLen = wLen;
				if( i == 0 )
				{
					FreezeAllInfo.AllAttDataLen = 4+wLen+4; //时标+数据长度，加4字节CRC数据校验---李丕凯200407
					AttOad[i].Offset = 4;					//前面是时标
				}
				else
				{						
					FreezeAllInfo.AllAttDataLen += wLen;
					AttOad[i].Offset = AttOad[i-1].Offset+AttOad[i-1].AttDataLen;
				}

			}				
		}
		
		if( tFreezeIndex == eFREEZE_MIN )
		{
			for( i=0; i<MAX_FREEZE_PLANNUM; i++ )
			{
				for( j=0; j<FreezeAllInfo.NumofOad; j++ )
				{
					PlanIndex = CURR_PLAN(AttOad[j].Oad);//取关联对象属性的方案号
					if(PlanIndex == i)
					{
						break;
					}
				}
				if(j == FreezeAllInfo.NumofOad)
				{
					memset( (BYTE *)&AttCycleDepth[i], 0x00, sizeof(TFreezeAttCycleDepth));
				}
			}
		}	
		
    	if( tFreezeIndex != eFREEZE_MIN )//分钟冻结在此之前已经判断储存长度
    	{
			if( FreezeAllInfo.AllAttDataLen > 4096 )//不允许跨扇区写
			{
				return FALSE;
			}
			//日冻结数据长度受E2空间大小的限制,长度减4为减去Ee中存储的冻结序列号的长度
			if((tFreezeIndex == eFREEZE_DAY) && (FreezeAllInfo.AllAttDataLen > (MAX_DAY_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
			{
				return FALSE;
			}
			//结算日冻结数据长度受E2空间大小的限制,长度减4为减去Ee中存储的冻结序列号的长度
			if((tFreezeIndex == eFREEZE_CLOSING) && (FreezeAllInfo.AllAttDataLen > (MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
			{
				return FALSE;
			}
			if(GetSectorInfo( FreezeAllInfo.AllAttDataLen, AttCycleDepth[0].Depth, AttCycleDepth[0].Depth, &SectorInfo ) == FALSE )
			{
				return FALSE;
			}
			
			if( SectorInfo.dwSaveSpace > FreezeInfoTab[tFreezeIndex].dwDataLen )
			{
				ASSERT(0,1);
				return FALSE;			
			}
    	}
    	else
    	{
			Result = DealMinFreezeInfo( FreezeAllInfo.NumofOad, (TFreezeAttOad*)&AttOad, (TFreezeAttCycleDepth*)&AttCycleDepth, FreezeAddrLen.dwDataAddr, &FreezeMinInfo );
			if( Result == FALSE )
			{
				return FALSE;
			}
			//处理分钟冻结备份区的 存储信息：备份区的存储深度，以及数据地址
			DealMinBakFreezeInfo(FreezeAddrLen.dwDataAddr, AttCycleDepth, MinBakAttCycleDepth, &FreezeMinInfo);
    	}
	}
	else if( Type == eDELETE_ATT_ALL )
	{		
		FreezeAllInfo.NumofOad = 0;
		FreezeAllInfo.AllAttDataLen = 0;
	}
	else
	{
		return FALSE;
	}
	
	//写冻结关联对象属性表
	api_VWriteSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);
	
	api_VWriteSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
			
	if( tFreezeIndex == eFREEZE_MIN )
	{
		//保存属性周期深度
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (MAX_FREEZE_PLANNUM + i), sizeof(TFreezeAttCycleDepth), (BYTE *)&MinBakAttCycleDepth[i] );
		}
		//更新分钟冻结信息
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo );	
		//全清指针	
		InitMinFreezeInfo();
	}
	else
	{		
		//保存属性周期深度
		api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );	
		//全清指针	
		api_ClrSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo) );	
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 设置冻结关联对象属性
//
//参数: 
//		OI[in]:		0x5000~0x500B,0x5011 冻结类的对象标识OI  
//  	Type[in]:	0--规约设置	0xFF--初始化调用，需要对OAD进行倒序
//  	pOad[in]:  	指向OAD Buffer的指针  2字节冻结周期+4字节OAD+2字节存储深度               
//  	OadNum[in]:	OAD的个数
//
//返回值:	TRUE/FALSE
//
//备注:	不支持设置某个属性。所有的重新计，重新分配EEP空间 		
//-----------------------------------------------
BOOL api_WriteFreezeAttribue( WORD OI, BYTE Type, BYTE *pOad, WORD OadNum )
{	
	BYTE i,k,tFreezeIndex,PlanIndex;
	WORD tw,tCycle,tRecordDepth,wLen,Result;
	DWORD dwOad;	
	TSectorInfo	SectorInfo;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//保证比存储在ee中的数据大，有地方存放CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth	MinBakAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;  
	TFreezeMinInfo FreezeMinInfo;

	tFreezeIndex = api_GetFreezeIndex( OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}
	
	// 获得各种属性地址
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return FALSE;
	}
	
	if( (OadNum>FreezeInfoTab[tFreezeIndex].MaxAttNum) || (OadNum==0) )
	{
		return FALSE;
	}

	if( (Type!=0) && (Type!=0xff) )
	{
		return FALSE;
	}

	// 所有的OAD
	memset( (BYTE *)&AttOad[0], 0x00, sizeof(AttOad));
	memset( (BYTE *)&AttCycleDepth[0], 0x00, sizeof(AttCycleDepth));
	memset( (BYTE *)&FreezeAllInfo, 0x00, sizeof(FreezeAllInfo));
	memset( (BYTE *)&FreezeMinInfo, 0x00, sizeof(FreezeMinInfo) );
	
	for( i = 0; i < OadNum; i++ )
	{
		//OAD
		memcpy( (BYTE *)&AttOad[i].Oad, pOad+2+i*FREEZE_ATTRIBUTE_LEN, 4);
		if( Type == 0xFF )
		{
			lib_InverseData((BYTE *)&AttOad[i].Oad,sizeof(DWORD));
		}
		//周期
		memcpy( (BYTE *)&tCycle, pOad+i*FREEZE_ATTRIBUTE_LEN, 2);
		//深度
		memcpy( (BYTE *)&tRecordDepth, pOad+6+i*FREEZE_ATTRIBUTE_LEN, 2);
		//周期性冻结不允许周期为0
		if((tFreezeIndex > eFREEZE_HOUR) && (tCycle == 0x0000) )	
		{
			return FALSE;
		}
		//设置的关联对象属性表中不允许深度为0
		if(tRecordDepth == 0x0000)
		{
			return FALSE;
		}
		//取方案，分钟冻结其他冻结置0
		if(tFreezeIndex == eFREEZE_MIN)
		{
			PlanIndex = CURR_PLAN(AttOad[i].Oad);
		}
		else
		{
			PlanIndex =0;
		}
		//判断此方案的周期深度不为零
		if((AttCycleDepth[PlanIndex].Cycle == 0x0000) && ((AttCycleDepth[PlanIndex].Depth == 0x0000)))
		{
			AttCycleDepth[PlanIndex].Cycle = tCycle;
			AttCycleDepth[PlanIndex].Depth = tRecordDepth;
		}
		else
		{
			//如果与同方案的周期深度不同返回FALSE
			if( (tCycle!=AttCycleDepth[PlanIndex].Cycle) || (tRecordDepth!=AttCycleDepth[PlanIndex].Depth) )	
			{
				return FALSE;
			}
		}
		dwOad = AttOad[i].Oad;
		wLen = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return FALSE;
		}

		// 计算Addr
		if( i == 0 )
		{						
			if( tFreezeIndex != eFREEZE_MIN )
			{
				AttOad[i].AttDataLen = wLen;
				FreezeAllInfo.AllAttDataLen = 4+wLen+4;//时间+数据+4字节CRC--20200407--李丕凯
				AttOad[i].Offset = 4;					
			}
		}
		else
		{
			// 不允许有相同的OAD
			for( k = 0; k < i; k++ )
			{
				if( AttOad[k].Oad == AttOad[i].Oad )
				{
					return FALSE;
				}
			}
			
			if( tFreezeIndex != eFREEZE_MIN )
			{
				AttOad[i].AttDataLen = wLen;
				FreezeAllInfo.AllAttDataLen += wLen;
				AttOad[i].Offset = AttOad[i-1].Offset+AttOad[i-1].AttDataLen;

			}			
		}
	}
	
	FreezeAllInfo.NumofOad = OadNum;

	if( tFreezeIndex != eFREEZE_MIN )//分钟冻结在此之前已经判断储存长度
	{
		if( FreezeAllInfo.AllAttDataLen > 4096 )//不允许跨扇区写
		{
			return FALSE;
		}
		//日冻结数据长度受E2空间大小的限制,长度减4为减去Ee中存储的冻结序列号的长度
		if((tFreezeIndex == eFREEZE_DAY) && (FreezeAllInfo.AllAttDataLen > (MAX_DAY_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
        {
			return FALSE;
		}
		//结算日冻结数据长度受E2空间大小的限制,长度减4为减去Ee中存储的冻结序列号的长度
		if((tFreezeIndex == eFREEZE_CLOSING) && (FreezeAllInfo.AllAttDataLen > (MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
		{
			return FALSE;
		}
		if(GetSectorInfo( FreezeAllInfo.AllAttDataLen, AttCycleDepth[0].Depth, AttCycleDepth[0].Depth, &SectorInfo ) == FALSE )
		{
			return FALSE;
		}

        if( SectorInfo.dwSaveSpace > FreezeInfoTab[tFreezeIndex].dwDataLen )
        {
            ASSERT(0,1);
            return FALSE;           
        }
	}
	else
	{
		Result = DealMinFreezeInfo( FreezeAllInfo.NumofOad, (TFreezeAttOad*)&AttOad, (TFreezeAttCycleDepth*)&AttCycleDepth, FreezeAddrLen.dwDataAddr, &FreezeMinInfo );
		if( Result == FALSE )
		{
			return FALSE;
		}
		//处理分钟冻结备份区的 存储信息：备份区的存储深度，以及数据地址
		DealMinBakFreezeInfo(FreezeAddrLen.dwDataAddr, AttCycleDepth, MinBakAttCycleDepth, &FreezeMinInfo);
	}
	
	api_VWriteSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
	api_VWriteSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);
	
	if( tFreezeIndex == eFREEZE_MIN )
	{
		//保存属性周期深度
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (MAX_FREEZE_PLANNUM + i), sizeof(TFreezeAttCycleDepth), (BYTE *)&MinBakAttCycleDepth[i] );
		}
		//更新分钟冻结信息
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo );	
		//全清指针	
		InitMinFreezeInfo();
	}
	else
	{		
		//保存属性周期深度
		api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );	
		//全清指针	
		api_ClrSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo) );	
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 读冻结关联对象属性
//
//参数: 
//	OI[in]:				0x5000~0x500B,0x5011 冻结类的对象标识OI                 
//  ElementIndex[in]:	关联属性对象表的元素索引（第几个对象）                
//  Len[in]:			规约层传来数据长度，超过此长度返回FALSE	
//  pBuf[out]: 			返回数据缓冲    
//
//备注: 正常数据长度( 在个数为0时，添加1字节的0 返回长度为1)
//-----------------------------------------------
WORD api_ReadFreezeAttribute( WORD OI, BYTE ElementIndex, WORD Len, BYTE *pBuf )
{	
	BYTE i,tFreezeIndex,PlanIndex;
	BYTE TempBuf[FREEZE_ATTRIBUTE_LEN];
	WORD wLenTemp;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//保证比存储在ee中的数据大，有地方存放CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
    TFreezeAllInfoRom FreezeAllInfo;

	wLenTemp = 0;
	
	tFreezeIndex = api_GetFreezeIndex( OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return 0x8000;
	}
	
	// 获得各种属性地址
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	//读出单条冻结的长度及关联属性的个数
	api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		FreezeAllInfo.NumofOad = 0;
	}
	
	//读出所有的关联属性对象 Oad
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);	
	//读出所有的关联属性对象 深度周期
	if( tFreezeIndex == eFREEZE_MIN )
	{
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
		}
	}
	else
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );
	}

	if( ElementIndex == 0 )
	{
		for( i = 0; i < FreezeAllInfo.NumofOad; i++ )
		{
			// 判断是否超出缓冲最大长度
			if( (wLenTemp+FREEZE_ATTRIBUTE_LEN) > Len )
			{
				return 0;
			}
			memcpy( TempBuf+2, (BYTE *)&AttOad[i].Oad, 4 );
			if( tFreezeIndex == eFREEZE_MIN )
			{
				PlanIndex = CURR_PLAN(AttOad[i].Oad);//取关联对象属性的方案号
			}
			else
			{
				PlanIndex = 0;
			}
			memcpy( TempBuf, (BYTE *)&AttCycleDepth[PlanIndex].Cycle, 2 );			
			memcpy( TempBuf+2+4, (BYTE *)&AttCycleDepth[PlanIndex].Depth, 2 );
			memcpy( pBuf+wLenTemp, TempBuf, FREEZE_ATTRIBUTE_LEN );
			wLenTemp += FREEZE_ATTRIBUTE_LEN;
		}

		if( wLenTemp == 0 )
		{
			pBuf[0] = 0;
			return 1;
		}
		else
		{
			return wLenTemp;			
		}
	}
	else
	{	
		if( ElementIndex >  FreezeAllInfo.NumofOad )
		{
			pBuf[0] = 0;
			return 1;
		}
		memcpy( pBuf+2, (BYTE *)&AttOad[ElementIndex-1].Oad, 4 );
		if( tFreezeIndex == eFREEZE_MIN )
		{
			PlanIndex= CURR_PLAN(AttOad[ElementIndex-1].Oad);//取关联对象属性的方案号
		}
		else
		{
			PlanIndex = 0;
		}	
		memcpy( pBuf, (BYTE *)&AttCycleDepth[PlanIndex].Cycle, 2 );
		memcpy( pBuf+2+4, (BYTE *)&AttCycleDepth[PlanIndex].Depth, 2 );
		
		return FREEZE_ATTRIBUTE_LEN;
	}
}

//-----------------------------------------------
//函数功能: 置冻结标志位
//
//参数:
//	inFreezeIndex[in]:		冻结类型
//	Delay[in]:	瞬时冻结的延时，其他冻结填0
//
//返回值:	TRUE/FALSE
//
//备注:   切换冻结,瞬时冻结调用 若调用eFREEZE_CLOSING，需保证是第一结算日且与原参数相比发生改变
//-----------------------------------------------
BOOL api_SetFreezeFlag( eFreezeType inFreezeIndex, WORD Delay )
{
	BYTE tFreezeIndex;

	tFreezeIndex = inFreezeIndex;
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}

	if( tFreezeIndex == eFREEZE_INSTANT )
	{
		if( Delay == 0 )
		{
			SetFreezeFlag(tFreezeIndex);
		}
		else
		{
			wInstantFreezeTimer = Delay;
			wInstantFreezeTimerBak = (WORD)(~wInstantFreezeTimer);
		}
	}
	else if( 	(tFreezeIndex == eFREEZE_TIME_ZONE_CHG)
		#if( PREPAY_MODE == PREPAY_LOCAL )
			||	(tFreezeIndex == eFREEZE_TARIFF_RATE_CHG)
			||	(tFreezeIndex == eFREEZE_LADDER_CHG)
			||	(tFreezeIndex == eFREEZE_LADDER_CLOSING)
		#endif
			||	(tFreezeIndex == eFREEZE_DAY_TIMETABLE_CHG)
			||	(tFreezeIndex == eFREEZE_CLOSING) )
	{
		SetFreezeFlag(tFreezeIndex);
		if( tFreezeIndex == eFREEZE_CLOSING )
		{
		    SetFreezeFlag(eFREEZE_MAX_NUM);//第一结算日标志、清需量
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------
//功能描述: 读取分钟冻结 备份区的数据
//
//参数:	pRecordInfo[in] 分钟冻结备份区的相关信息
//
//返回值:
//
//备注:
//--------------------------------------------------
BOOL ReadMinBakRecordData(TReadFreezeRecordInfo	*pRecordInfo, BYTE * pBuf)
{
	TSectorInfo	SectorInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAttCycleDepth AttCycleDepth;
	DWORD Addr;
	BOOL Ret = FALSE;

	//获得 分钟冻结备份区的 冻结深度
	GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen );
	Ret = api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (pRecordInfo->MinPlan + MAX_FREEZE_PLANNUM), sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth );
	if(Ret == FALSE)
	{
		return FALSE;
	}
	GetSectorInfo(pRecordInfo->RecordLen, AttCycleDepth.Depth, pRecordInfo->RecordNo + 1, &SectorInfo);
	Addr = pRecordInfo->dwBakAddr + SectorInfo.dwAddr;
	api_ReadMemRecordData(Addr, pRecordInfo->RecordLen, pBuf);

	//分钟冻结主数据区 4字节时标 +  数据 + 4字节CRC
	//		   备份区 4字节时标 +  数据 + 4字节记录号
	memcpy(&Addr, pBuf + pRecordInfo->RecordLen - sizeof(DWORD), sizeof(DWORD));
	Ret = FALSE;
	if(Addr == pRecordInfo->RecordNo)
	{
		Ret = TRUE;
	}

	return Ret;
}
//-----------------------------------------------
//函数功能: 读冻结记录数据
//
//参数: 
//			pData[in]	输入信息
//			pBuf[out]	读操作时的缓冲
//			
//			
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:  调用此函数读取冻结数据的整条数据,
//      此函数读取冻结数据区，有备份数据在数据错误情况下会调用备份数据，  
//-----------------------------------------------
BOOL ReadFreezeRecordData(TReadFreezeRecordInfo	*pRecordInfo, BYTE * pBuf)
{
	DWORD dwE2DataAddr;
	DWORD dwDataNo;//E2内存储的序列号
	BOOL Ret = TRUE;

	if( pRecordInfo->FreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}
	if( pRecordInfo->RecordLen > 2000 )
	{
		return FALSE;
	}
	api_ReadMemRecordData(pRecordInfo->dwAddr,pRecordInfo->RecordLen,pBuf);
	//只有日冻结，结算日冻结在CRC校验错误的情况下才从Ee中读取
	if(( (pRecordInfo->FreezeIndex == eFREEZE_DAY) ||(pRecordInfo->FreezeIndex == eFREEZE_CLOSING) )
	&&(lib_CheckSafeMemVerify(pBuf, pRecordInfo->RecordLen, UN_REPAIR_CRC) != TRUE))
	{
		//记录异常事件记录
		api_WriteSysUNMsg(GET_FREEZE_RECORD_ERR);
		if(pRecordInfo->FreezeIndex == eFREEZE_DAY)
		{
			//日冻结E2备份数据的基地址
			dwE2DataAddr = 	GET_CONTINUE_ADDR( FreezeConRom.EeData.Day[0] );
		}
		else
		{
			//结算日冻结E2备份数据的基地址
			dwE2DataAddr = 	GET_CONTINUE_ADDR( FreezeConRom.EeData.Closing[0] );
		}
		dwE2DataAddr += (DWORD)(pRecordInfo->RecordLen+sizeof(DWORD)) * (pRecordInfo->RecordNo% FreezeInfoTab[pRecordInfo->FreezeIndex].wEeDepth);

		//读取序列号，判断Ee中数据是否为所读取数据，不是不读取Ee数据
		api_ReadFromContinueEEPRom(dwE2DataAddr,sizeof(DWORD), (BYTE*)&dwDataNo);
		Ret = FALSE;
		if(dwDataNo == pRecordInfo->RecordNo )
		{
			dwE2DataAddr += sizeof(DWORD);
			api_ReadFromContinueEEPRom(dwE2DataAddr,pRecordInfo->RecordLen, pBuf);

			Ret = lib_CheckSafeMemVerify(pBuf, pRecordInfo->RecordLen, UN_REPAIR_CRC);
		}
	}
	else if( pRecordInfo->FreezeIndex == eFREEZE_MIN )
	{
		Ret = lib_CheckSafeMemVerify(pBuf, pRecordInfo->RecordLen, UN_REPAIR_CRC);
		if( Ret != TRUE)
		{
			Ret = ReadMinBakRecordData(pRecordInfo, pBuf);
		}
	}
	 
	return Ret;
}
//-----------------------------------------------
//函数功能: 读冻结记录时间
//
//参数: 
//			pData[in]	输入信息
//			pBuf[out]	读操作时的缓冲
//			
//			
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:  调用此函数读取冻结数据的时间,
//      此函数读取冻结数据区，有备份数据在数据错误情况下会调用备份数据，  
//-----------------------------------------------
BOOL ReadFreezeRecordTime(TReadFreezeRecordInfo	*pRecordInfo, DWORD* Time)
{
	BOOL Flag = FALSE;
	BYTE  Buf[2030];//申请不定长数据
	
	if(pRecordInfo->RecordLen > 2000)
	{
		return Flag;
	}

	//读出整条数据
	if(ReadFreezeRecordData( pRecordInfo, Buf ) == TRUE)
	{
		Flag = TRUE;
	}
	memcpy( (BYTE *)Time, Buf, sizeof(DWORD) );
	
	return Flag;
	
}
//-----------------------------------------------
//函数功能: 读E2冻结记录数据
//
//参数: 
//			pData[in]	输入信息
//			pBuf[out]	读操作时的缓冲
//			                  
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注: 单相表，掉电情况下使用  
//-----------------------------------------------
#if( MAX_PHASE == 1 )
BOOL ReadFreezeRecordEeData(TReadFreezeRecordInfo	*pRecordInfo, BYTE * pBuf)
{
	DWORD dwE2DataAddr;
	DWORD dwDataNo;//E2内存储的序列号
	if( pRecordInfo->FreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}
	if( pRecordInfo->RecordLen > 2000 )
	{
		return FALSE;
	}
	//只有日冻结，结算日冻结在CRC校验错误的情况下才从Ee中读取
	if( (pRecordInfo->FreezeIndex == eFREEZE_DAY) ||(pRecordInfo->FreezeIndex == eFREEZE_CLOSING) )
	{
		if(pRecordInfo->FreezeIndex == eFREEZE_DAY)
		{
			//日冻结E2备份数据的基地址
			dwE2DataAddr = 	GET_CONTINUE_ADDR( FreezeConRom.EeData.Day[0] );
		}
		else
		{
			//结算日冻结E2备份数据的基地址
			dwE2DataAddr = 	GET_CONTINUE_ADDR( FreezeConRom.EeData.Closing[0] );
		}
		dwE2DataAddr += (DWORD)(pRecordInfo->RecordLen+sizeof(DWORD)) * (pRecordInfo->RecordNo% FreezeInfoTab[pRecordInfo->FreezeIndex].wEeDepth);

		//读取序列号，判断Ee中数据是否为所读取数据，不是不读取Ee数据
		api_ReadFromContinueEEPRom(dwE2DataAddr,sizeof(DWORD), (BYTE*)&dwDataNo);
		if(dwDataNo == pRecordInfo->RecordNo )
		{
			dwE2DataAddr += sizeof(DWORD);
			api_ReadFromContinueEEPRom(dwE2DataAddr,pRecordInfo->RecordLen, pBuf);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}
#endif
//-----------------------------------------------
//函数功能: 写冻结数据区,带地址校验
//
//参数: 
//			Addr[in]		要写的起始地址
//			Length[in]		要写的长度
//			BaseAddr[in]	冻结或事件分配的基地址
//			MemSpace[in]	冻结或事件所分配的空间
//			pBuf[in/out]	写操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:此函数只写Flash事件数据区
//-----------------------------------------------
BOOL WriteFreezeRecordData(DWORD Addr, WORD Length, DWORD BaseAddr, DWORD MemSpace, BYTE * pBuf)
{
	if((Addr >= BaseAddr) && ((Addr+Length) <= (BaseAddr+MemSpace)))
	{
		return api_WriteMemRecordData( Addr, Length, pBuf );
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
//函数功能: 擦扇区写冻结事件数据区
//
//参数: 
//			Addr[in]		要写的起始地址
//			Length[in]		要写的长度
//			pBuf[in/out]	写操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:此函数只允许冻结任务函数调用--因为在任务中pBuf指向4K-Buf,不然有冲内存问题
//-----------------------------------------------
BOOL WriteFreezeRecordData_EraseFlash(DWORD Addr, WORD Length, BYTE * pBuf)
{
	DWORD dwSectorAddr;
	WORD Len;
	
	if( Length == 0)
	{
		dwSectorAddr = (Addr / SECTOR_SIZE) * SECTOR_SIZE;//写的本扇区基地址
		
		//读出此扇区前面的数据，再擦除此页，最后再整体写入
		api_ReadMemRecordData( dwSectorAddr, Addr - dwSectorAddr, pBuf );
		
		Len = Addr - dwSectorAddr;//计算整体长度
		return api_WriteMemRecordData( dwSectorAddr, Len, pBuf );
	}
	else
	{

		if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
		{
			return FALSE;
		}
		dwSectorAddr = (Addr / SECTOR_SIZE) * SECTOR_SIZE;//写的本扇区基地址
	
		//先把本次数据放在4k内存的后面，将本次冻结的数据放在后面
		memmove( pBuf + (Addr - dwSectorAddr), pBuf, Length );
		//读出此扇区前面的数据，再擦除此页，最后再整体写入
		api_ReadMemRecordData( dwSectorAddr, Addr - dwSectorAddr, pBuf );
		
		Len = Length + Addr - dwSectorAddr;//计算整体长度
		if(Len > SECTOR_SIZE)
		{
			return FALSE;
		}
		//更改时间的从扇区起始地址开始写
		return api_WriteMemRecordData( dwSectorAddr, Len, pBuf );

	
	}
	
}
//--------------------------------------------------
//功能描述: 记录序号倒退处理
//
//参数:		No[in] 需要倒退到的序列号
//   	    Depth[in]  冻结深度
//			Len[in] 一整条数据的长度
//			BaseAddr 整个冻结的基地址（分钟冻结为每个方案的起始地址） 
//			Buf[in] 
//返回值: 无
//
//备注:No = 1时，只会保留第1条记录(仅保留第一条记录)
//	   No = 2时，只会保留第1,2条记录(仅保留前2条记录)
//--------------------------------------------------
void ProcFreezeNoRollback(DWORD No, DWORD Depth, DWORD Len, DWORD BaseAddr, BYTE *Buf)
{
	TSectorInfo	SectorInfo;
	DWORD Addr;

	if(Depth == 0)
	{
		return;
	}

	GetSectorInfo(Len, Depth, No, &SectorInfo);
	Addr = BaseAddr + SectorInfo.dwAddr;

 

	if( (Addr % SECTOR_SIZE) != 0)//地址不是扇区起始地址
	{
		WriteFreezeRecordData_EraseFlash(Addr, 0, Buf);
	}
	else
	{
		//No对应的地址 刚好为扇区起始地址时，不需要特殊处理，写flash会自动擦除整个扇区
	}
}


//-----------------------------------------------
//函数功能: 清冻结数据
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void api_ClrFreezeData( void )
{
	BYTE i;
	TFreezeAddrLen FreezeAddrLen;
	
	for( i=0; i<eFREEZE_MAX_NUM; i++ )
	{
		// 获得各种冻结地址
		GetFreezeAddrInfo( i, &FreezeAddrLen );
		
		api_ClrSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo) );	
	}
	
	//清分钟指针
	InitMinFreezeInfo();
}

//-----------------------------------------------
//函数功能: 获取结算日时间
//
//参数: 	pBillingPara[in]: 结算参数
//                    
//返回值:	无
//
//备注:   	已经排好序
//-----------------------------------------------
static BOOL GetClosingTime( TBillingPara *pBillingPara )
{
	BYTE i,j;
	//读出转存参数
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ), sizeof(TBillingPara),(BYTE *)pBillingPara  ) != TRUE )
	{
		return FALSE;
	}

	//检查（参数为Hex）
	for( i = 0; i < MAX_MON_CLOSING_NUM; i++ )
	{
		if( (pBillingPara->MonSavePara[i].Day > MAX_VALID_DAY) || (pBillingPara->MonSavePara[i].Day == 0) || (pBillingPara->MonSavePara[i].Hour > MAX_VALID_HOUR) )
		{
			pBillingPara->MonSavePara[i].Day = 0xFF;
			pBillingPara->MonSavePara[i].Hour = 0xFF;
			continue;
		}
		for(j = 0;j < i;j++)
		{
			if((pBillingPara->MonSavePara[i].Day == pBillingPara->MonSavePara[j].Day) && (pBillingPara->MonSavePara[i].Hour == pBillingPara->MonSavePara[j].Hour))
			{
				pBillingPara->MonSavePara[i].Day = 0xFF;
				pBillingPara->MonSavePara[i].Hour = 0xFF;
			}
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 结算日转存检查
//
//参数:		无
//                    
//返回值:	无
//
//备注:   	小时变化调用
//-----------------------------------------------
static void CheckClosing( void )
{
	BYTE i,Day,Hour;
	TBillingPara Para;	

	if( GetClosingTime(&Para) == FALSE )
	{
		return;
	}

	api_GetRtcDateTime( DATETIME_DD, &Day );
	api_GetRtcDateTime( DATETIME_hh, &Hour );

	for( i = 0; i < MAX_MON_CLOSING_NUM; i++ )
	{
		if( (Para.MonSavePara[i].Day==Day) && (Para.MonSavePara[i].Hour==Hour) )
		{
			api_SwapEnergy();							// 转存电能

			if( i == 0 )
			{
				SetFreezeFlag(eFREEZE_MAX_NUM);	//第一结算日标志、清需量
			}

			SetFreezeFlag(eFREEZE_CLOSING);
			break;
		}
	}
}

//-----------------------------------------------
//函数功能: 更新冻结判断用的时间,并更新RecordNo
//
//参数: 	InOldRelativeMin[in]:修改时间之前的相对时间
//                    
//返回值:	无
//
//备注:
//-----------------------------------------------
void api_UpdateFreezeTime( DWORD InOldRelativeMin )
{
	BYTE i,PlanIndex;
	WORD Result;
	DWORD dwTemp,dwStartTime;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//保证比存储在ee中的数据大，有地方存放CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeDataInfo	DataInfo,SearchDataInfo[MAX_FREEZE_PLANNUM];
	TBinarySchInfo BinarySchInfo;
	TFreezeAllInfoRom FreezeAllInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;
	TFreezeMinInfo FreezeMinInfo;

	if( g_RelativeMin >= InOldRelativeMin )
	{
		return;
	}
	//检查分钟冻结信息
	Result = CheckMinFreezeInfo();

	#if( SLE_THIRD_MEM_BACKUP == YES )
	//上电反写RTC时间会检查并刷新冻结记录序号，增加冻结恢复
	if(( Result == FALSE )&&( InOldRelativeMin == 0xFFFFFFFF ))
	{
		RepairFreezeUseFlash();
	}
	#endif

	//读出修改时间分钟冻结标志
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
	if( Result == FALSE )
	{
		memset( (BYTE*)&FreezeUpdateTimeFlag, 0x00, sizeof(TFreezeUpdateTimeFlag) );
	}
	
	//先处理时日月补冻（整条补冻）
	for(i=eFREEZE_HOUR; i<eFREEZE_MIN; i++)
	{
		// 获得各种属性地址
		GetFreezeAddrInfo( i, &FreezeAddrLen );		
		//读出所有的关联属性对象oad
		api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[i].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);
		//读出关联属性对象 周期深度
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );
		//读出此条的RecordNo
		api_VReadSafeMem(FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo);
		//读出单条冻结的长度及关联属性的个数
		api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
		
		if( FreezeAllInfo.AllAttDataLen > 2000 )
		{
			continue;
		}
		
		dwTemp = DataInfo.RecordNo;
		
		//填写搜索信息
		BinarySchInfo.dwBaseAddr = FreezeAddrLen.dwDataAddr;	
		BinarySchInfo.dwDepth = AttCycleDepth[0].Depth;	
		BinarySchInfo.dwSaveDot = DataInfo.SaveDot;
		BinarySchInfo.dwRecordNo = DataInfo.RecordNo;		
		BinarySchInfo.Time = g_RelativeMin+1;//避免要搜的g_RelativeMin时刻正好有冻结点，下次再冻结会漏掉此时刻的问题
		BinarySchInfo.pRecordNo = &DataInfo.RecordNo;
		BinarySchInfo.pTime = &dwStartTime;
		BinarySchInfo.wLen = FreezeAllInfo.AllAttDataLen;
		BinarySchInfo.FreezeIndex = i;
		if( BinSearchByTime(&BinarySchInfo) == TRUE )
		{			
			//根据之前的RecordNo和新搜的RecordNo计算新的 SaveDot
			if(dwTemp > DataInfo.RecordNo)
			{
				dwTemp = dwTemp-DataInfo.RecordNo;
				if(DataInfo.SaveDot > dwTemp)
				{
					DataInfo.SaveDot = DataInfo.SaveDot-dwTemp;
					FreezeUpdateTimeFlag.wFreezeFlag |= (0x0001<<i);
				}				
				else
				{
					DataInfo.RecordNo = 0;
					DataInfo.SaveDot = 0;
				}
			}
			else if(dwTemp == DataInfo.RecordNo)
			{
				continue;
			}
			else
			{
				DataInfo.RecordNo = 0;
				DataInfo.SaveDot = 0;
			}
			api_VWriteSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo );
		}	

	}
	
	//处理分钟冻结
	//获得各种属性地址
    GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen );
	//读取分钟冻结地址信息
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo ); 
	if( FreezeMinInfo.DataAddr[0] != FreezeAddrLen.dwDataAddr )//方案0地址 与基地址一致
	{
		FreezeMinInfo.DataAddr[0] = FreezeAddrLen.dwDataAddr;
	}
	//读出单条冻结的长度及关联属性的个数
	api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
	//读出所有的关联属性对象
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[eFREEZE_MIN].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);		
	//读出所有的关联属性对象 周期深度
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
	}
	for(PlanIndex=0;PlanIndex<MAX_FREEZE_PLANNUM;PlanIndex++)
	{
		if( AttCycleDepth[PlanIndex].Depth == 0 )//0 代表此方案无冻结 跳过
		{
			continue;
		}
		
		dwTemp = MinInfo[PlanIndex].RecordNo;
		//填写搜索信息
		BinarySchInfo.dwBaseAddr = FreezeMinInfo.DataAddr[PlanIndex];	
		BinarySchInfo.dwDepth = AttCycleDepth[PlanIndex].Depth;	
		BinarySchInfo.dwSaveDot = MinInfo[PlanIndex].SaveDot;
		BinarySchInfo.dwRecordNo = MinInfo[PlanIndex].RecordNo;		
		BinarySchInfo.Time = g_RelativeMin+1;//设置的时间还有秒，不论秒是否为0，都搜索下一个点
		BinarySchInfo.pRecordNo = &MinInfo[PlanIndex].RecordNo;
		BinarySchInfo.pTime = &dwStartTime;
		BinarySchInfo.wLen = FreezeMinInfo.AllAttDataLen[PlanIndex];
		BinarySchInfo.FreezeIndex = eFREEZE_MIN;
		BinarySchInfo.MinBakAddr = FreezeMinInfo.DataBakAddr[PlanIndex];
		BinarySchInfo.MinPlan = PlanIndex;
		if( BinSearchByTime(&BinarySchInfo) == TRUE )
		{			
			//根据之前的RecordNo和新搜的RecordNo计算新的 SaveDot
			if(dwTemp > MinInfo[PlanIndex].RecordNo)
			{
				dwTemp = dwTemp-MinInfo[PlanIndex].RecordNo;
				if(MinInfo[PlanIndex].SaveDot > dwTemp)
				{
					MinInfo[PlanIndex].SaveDot = MinInfo[PlanIndex].SaveDot-dwTemp;
					FreezeUpdateTimeFlag.wMinFlag |= (0x01<<PlanIndex);
				}
				else
				{
					MinInfo[PlanIndex].RecordNo = 0;
					MinInfo[PlanIndex].SaveDot = 0;
				}
			}
			else if(dwTemp == MinInfo[PlanIndex].RecordNo)
			{
				continue;
			}
			else
			{
				MinInfo[PlanIndex].RecordNo = 0;
				MinInfo[PlanIndex].SaveDot = 0;
			}
			MinInfo[PlanIndex].CRC32 = lib_CheckCRC32( (BYTE*)&MinInfo[PlanIndex], sizeof(TFreezeDataInfo)-sizeof(DWORD) );
			api_VWriteSafeMem( FreezeAddrLen.wDataInfoEeAddr+PlanIndex*sizeof(TFreezeDataInfo), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[PlanIndex] );
		}	
		
	}
	
	if( (FreezeUpdateTimeFlag.wMinFlag!=0) || (FreezeUpdateTimeFlag.wFreezeFlag!=0) )
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
	}
}

//-----------------------------------------------
//函数功能: 冻结任务
//
//参数: 	PowerUpFlag[in]:补冻标志，
//						0x55: 补冻 0x00: 正常冻结
//
//返回值:	无
//
//备注:   大循环、补冻调用，分钟冻结指针，次数等信息进行特殊处理，不存EEP
//-----------------------------------------------
void ProcFreezeTask(BYTE tFreezeIndex,BYTE  PowerUpFlag)
{
	BYTE i;
    WORD OI;
	DWORD dwAddr,dwBaseAddr;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//保证比存储在ee中的数据大，有地方存放CRC
	TSectorInfo	SectorInfo;
	TFreezeAttCycleDepth	AttCycleDepth;
	TFreezeDataInfo	DataInfo;
	TFreezeAddrLen FreezeAddrLen;
    TFreezeAllInfoRom FreezeAllInfo;
	BYTE FlashBuf[SECTOR_SIZE];
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;
	DWORD dwDataE2Addr,dwDataE2MaxAddr; //E2中冻结数据存储地址-结算日冻结、日冻结,E2冻结数据存储结束地址
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return;
	}
	// 获得各种属性地址
    GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen );

	//读出单条冻结的长度及关联属性的个数
	if( api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo) != TRUE )
	{
		return;
	}
	
	if( FreezeAllInfo.NumofOad == 0 )
	{
		return;
	}
	
	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return;
	}
	
	if( FreezeAllInfo.AllAttDataLen > 2000 )
	{
		return;
	}

	//读出所有的关联属性对象
	if(api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]) == FALSE )
	{
		return;
	}
	//读出所有的关联属性对象 周期深度
	if(api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth ) == FALSE )
	{
		return;
	}
	//读出此条的RecordNo
	if( api_VReadSafeMem(FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo) != TRUE )
	{
		DataInfo.RecordNo = 0;
		DataInfo.SaveDot = 0;
	}
	//保存基地址，
	dwBaseAddr = FreezeAddrLen.dwDataAddr;
	//计算此数据保存的绝对地址
	if(GetSectorInfo( FreezeAllInfo.AllAttDataLen, AttCycleDepth.Depth, DataInfo.RecordNo, &SectorInfo ) == FALSE )
	{
		return ;
	}
	FreezeAddrLen.dwDataAddr += SectorInfo.dwAddr; 	
	if(tFreezeIndex == eFREEZE_DAY)
	{
		//日冻结E2备份数据的基地址
		dwDataE2Addr = 	(DWORD)GET_CONTINUE_ADDR( FreezeConRom.EeData.Day[0] );
		//日冻结E2备份数据的偏移地址
		dwDataE2Addr += (DWORD)(FreezeAllInfo.AllAttDataLen+sizeof(DWORD)) * (DataInfo.RecordNo% FreezeInfoTab[eFREEZE_DAY].wEeDepth);
		//日冻结E2备份数据最大地址
		dwDataE2MaxAddr = (DWORD)GET_CONTINUE_ADDR( FreezeConRom.EeData.Day[0] ) + MAX_DAY_FREEZE_E2_DATA_LEN;
	}
	else if(tFreezeIndex == eFREEZE_CLOSING)
	{
		//日冻结E2备份数据的基地址
		dwDataE2Addr = (DWORD)	GET_CONTINUE_ADDR( FreezeConRom.EeData.Closing[0] );
		dwDataE2Addr += (DWORD)(FreezeAllInfo.AllAttDataLen+sizeof(DWORD)) * (DataInfo.RecordNo% FreezeInfoTab[eFREEZE_CLOSING].wEeDepth);
		//结算日冻结E2备份数据最大地址
		dwDataE2MaxAddr = (DWORD)GET_CONTINUE_ADDR( FreezeConRom.EeData.Closing[0] ) + MAX_CLOSING_FREEZE_E2_DATA_LEN;
	}
	

	if( tFreezeIndex >= eFREEZE_HOUR)
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag) == TRUE )
		{
			if( FreezeUpdateTimeFlag.wFreezeFlag & (0x0001<<tFreezeIndex) )
			{
				ProcFreezeNoRollback(DataInfo.RecordNo, AttCycleDepth.Depth, FreezeAllInfo.AllAttDataLen, dwBaseAddr, FlashBuf);
				FreezeUpdateTimeFlag.wFreezeFlag &= ~(0x0001<<tFreezeIndex);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
			}
		}
	}

	memset( FlashBuf, 0xff, FreezeAllInfo.AllAttDataLen );
	//不是补冻结用当前时间相对分钟数
	if(PowerUpFlag == 0x55)
	{
		//时标
		memcpy( FlashBuf, (BYTE *)&PowerUpFreezeMin, sizeof(DWORD) );
	}
	else
	{
		//时标
		memcpy( FlashBuf, (BYTE *)&g_RelativeMin, sizeof(DWORD) );
	}
	
	//获取数据
	for(i=0; i<FreezeAllInfo.NumofOad; i++)
	{
		api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&AttOad[i].Oad, NULL, AttOad[i].AttDataLen, FlashBuf+AttOad[i].Offset );
		if(PowerUpFlag == 0x55)
		{
			if( !(FreezeFlag & (0x0001<<eFREEZE_MAX_NUM)) )//掉电跨结算日
			{
				OI = (AttOad[i].Oad&0xff)*0x100+((AttOad[i].Oad>>8)&0xff);
				if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
				{
					//此处判断目地为上电补冻时需量未清零状态下，将补冻中需量数据填充为全0
					//结算日补冻放在最后一个，此时冻完过第一结算日后需量已经清零，不需要标识
					//但需量是否补全F需要标识，故用结算日补冻时此标志标识是否补全FF--李丕凯
					if( tFreezeIndex != eFREEZE_CLOSING )
					{
						// 掉电跨结算日，最大需量及发生时间存0x00
						memset( FlashBuf+AttOad[i].Offset, 0x00, AttOad[i].AttDataLen );
					}
					else
					{
						// 掉电跨结算日，最大需量及发生时间存0xFF
						memset( FlashBuf+AttOad[i].Offset, 0xFF, AttOad[i].AttDataLen );
					}
				}
			}
		}
		else if( tFreezeIndex == eFREEZE_CLOSING )
		{
			if( !(FreezeFlag & (0x0001<<eFREEZE_MAX_NUM)) )//非第一结算日
			{
				OI = (AttOad[i].Oad&0xff)*0x100+((AttOad[i].Oad>>8)&0xff);
				if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
				{
					// 如果是非第一结算日，最大需量及发生时间存0xFF
					memset( FlashBuf+AttOad[i].Offset, 0xFF, AttOad[i].AttDataLen );
				}
			}
		}
	}
	//添加计算4字节校验函数--李丕凯--200407
	lib_CheckSafeMemVerify(FlashBuf,(WORD)FreezeAllInfo.AllAttDataLen, REPAIR_CRC);

	//此条冻结数据写入E2--李丕凯-20407
	if( (FreezeInfoTab[tFreezeIndex].wEeDepth != 0) 
	&& ((dwDataE2Addr+FreezeAllInfo.AllAttDataLen +sizeof(DWORD)) <= dwDataE2MaxAddr)) //防越界极限值判断
	{
		//E2内的备份数据带冻结序列号
		api_WriteToContinueEEPRom( dwDataE2Addr+sizeof(DWORD), FreezeAllInfo.AllAttDataLen, FlashBuf );
		api_WriteToContinueEEPRom( dwDataE2Addr, sizeof(DWORD), (BYTE*)&DataInfo.RecordNo );    
	}

	//写入此条冻结数据，写失败，调用先擦后写函数进行写操作
	if(WriteFreezeRecordData( FreezeAddrLen.dwDataAddr, FreezeAllInfo.AllAttDataLen, dwBaseAddr, FreezeInfoTab[tFreezeIndex].dwDataLen, FlashBuf ) == FALSE)
	{
		WriteFreezeRecordData_EraseFlash(FreezeAddrLen.dwDataAddr, FreezeAllInfo.AllAttDataLen, FlashBuf);
	}
	
	DataInfo.RecordNo++;
	if( DataInfo.SaveDot < AttCycleDepth.Depth )
	{
		DataInfo.SaveDot++;
	}
	//保存此条的RecordNo
	api_VWriteSafeMem(FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo);
}
//--------------------------------------------------
//功能描述:分钟冻结 处理
//
//参数: PlanIndex[in] 分钟冻结方案索引
//		PlanBaseAddr[in]  分钟冻结方案基地址
//		No[in]	主数据区 分钟冻结序列号
//		Len[in]	PlanIndex方案数据长度
//		Buf[in] PlanIndex方案数据
//返回值:无
//
//备注:冻结记录号为No时，主数据区存储在 No地址处，备份存储在No+1处，
//	   为了避免主 备份扇区同时擦除，导致冻结时间过长
//--------------------------------------------------
static void ProcMinBakFreezeTask(DWORD PlanBaseAddr, DWORD Depth, DWORD No, WORD Len,  BYTE *Buf)
{
	BYTE EraseFlashBuf[10];
	DWORD MinBakBaseAddr;
	TSectorInfo	SectorInfo;
	TFreezeAddrLen FreezeAddrLen;

	if( (Depth == 0) || ( Len < sizeof(DWORD)) )
	{
		return;	//每一条有效分钟冻结的长度必定不小于4( 时标4字节+ 4字节CRC/4字节序列号 + N字节数据)
	}

	//获得 分钟冻结备份的 起始地址; 分钟冻结主数据区后面 紧跟 备份区
	GetFreezeAddrInfo(eFREEZE_MIN, &FreezeAddrLen);
	MinBakBaseAddr = FreezeAddrLen.dwDataAddr + MAX_MIN_FREEZE_DATA_LEN;

	if(No == 0)
	{
		memset(EraseFlashBuf, 0xFF, sizeof(EraseFlashBuf));
		WriteFreezeRecordData( PlanBaseAddr, sizeof(EraseFlashBuf), MinBakBaseAddr, MAX_MINBAK_FREEZE_DATA_LEN, EraseFlashBuf );
	}

	//冻结主数据区 最后四个字节为CRC32, 备份区不需要CRC，用冻结序号填充
	memcpy(EraseFlashBuf, Buf + Len - sizeof(DWORD), sizeof(DWORD));
	memcpy(Buf + Len - sizeof(DWORD), &No, sizeof(DWORD));
    GetSectorInfo(Len, Depth, No + 1, &SectorInfo);
   //写失败，调用先擦后写函数进行写操作
	if(WriteFreezeRecordData( PlanBaseAddr+SectorInfo.dwAddr, Len, MinBakBaseAddr, MAX_MINBAK_FREEZE_DATA_LEN, Buf ) == FALSE)
	{
		WriteFreezeRecordData_EraseFlash(PlanBaseAddr+SectorInfo.dwAddr, Len, Buf);
	}
	memcpy(Buf + Len - sizeof(DWORD), EraseFlashBuf, sizeof(DWORD));	
}


//-----------------------------------------------
//函数功能: 处理分钟冻结任务
//
//参数: 	无
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
void ProcMinFreezeTask(void)
{
	BYTE i,k,PlanIndex,bFlag;
	DWORD dwAddr,dwBaseAddr;
	TFreezeAttOad	AttOad[MAX_MINUTE_FREEZE_ATTRIBUTE+1];
	TSectorInfo	SectorInfo;
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth	MinBakAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	BYTE FlashBuf[SECTOR_SIZE];
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;
	TFreezeMinInfo FreezeMinInfo;

	// 获得分钟冻结属性地址
	GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen );
	//读出冻结的关联属性的个数
	if(api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo)== FALSE )
	{
		return;
	}	
	//读出所有的关联属性对象
	if(api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[eFREEZE_MIN].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]) == FALSE )
	{
		return;
	}
	//读出所有的关联属性对象 周期深度
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );	
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (i + MAX_FREEZE_PLANNUM), sizeof(TFreezeAttCycleDepth), (BYTE *)&MinBakAttCycleDepth[i] );
	}
	
	//读取分钟冻结地址信息
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo );	
	//检查分钟冻结指针等数据
	CheckMinFreezeInfo();
	//读出冻结修改时间标志
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);	

	//基地址等于分钟冻结基地址;
	dwBaseAddr =  FreezeAddrLen.dwDataAddr;
	if( FreezeMinInfo.DataAddr[0] != dwBaseAddr )//方案0地址 与基地址一致
	{
		FreezeMinInfo.DataAddr[0] = dwBaseAddr;
	}
	
	for( PlanIndex = 0; PlanIndex < MAX_FREEZE_PLANNUM; PlanIndex++ )
	{
		CLEAR_WATCH_DOG;
		if( AttCycleDepth[PlanIndex].Cycle == 0 )//周期为0代表不支持
		{
			continue;
		}
		
		if( (g_RelativeMin%AttCycleDepth[PlanIndex].Cycle) == 0 )//对应方案 到冻结时间
		{
			if( FreezeMinInfo.AllAttDataLen[PlanIndex] > SECTOR_SIZE )
			{
				continue;
			}
			
			//计算此数据保存的绝对地址
			if(GetSectorInfo( FreezeMinInfo.AllAttDataLen[PlanIndex], AttCycleDepth[PlanIndex].Depth, MinInfo[PlanIndex].RecordNo, &SectorInfo ) == FALSE )
			{
				continue;
			}

			dwAddr = (FreezeMinInfo.DataAddr[PlanIndex] + SectorInfo.dwAddr);
			
			bFlag = 0;
			//判断备份的修分钟冻结改时间标志
			if( FreezeUpdateTimeFlag.wMinFlag & (0x0001<<PlanIndex) )
			{
				//分钟冻结 主数据区与备份区 记录序号差1，
				ProcFreezeNoRollback(MinInfo[PlanIndex].RecordNo, AttCycleDepth[PlanIndex].Depth, FreezeMinInfo.AllAttDataLen[PlanIndex], FreezeMinInfo.DataAddr[PlanIndex], FlashBuf);
				ProcFreezeNoRollback(MinInfo[PlanIndex].RecordNo+1, MinBakAttCycleDepth[PlanIndex].Depth, FreezeMinInfo.AllAttDataLen[PlanIndex], FreezeMinInfo.DataBakAddr[PlanIndex], FlashBuf);
				
				//清除分钟冻结对应方案的修改冻结标志
				FreezeUpdateTimeFlag.wMinFlag &= ~(0x0001<<PlanIndex);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);

				if( PlanIndex == 0 )
				{
					bFlag = 0x55;
				}
			}
			
			memset( FlashBuf, 0xff, FreezeMinInfo.AllAttDataLen[PlanIndex] );
			//组帧时间
			memcpy( FlashBuf, (BYTE *)&g_RelativeMin, sizeof(DWORD) );
			
			for(i=0; i<FreezeAllInfo.NumofOad; i++)
			{	
				k = CURR_PLAN(AttOad[i].Oad);
				if( k != PlanIndex )
				{
					continue;
				}
				
				api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&AttOad[i].Oad, NULL, AttOad[i].AttDataLen, FlashBuf+AttOad[i].Offset );
			}

			lib_CheckSafeMemVerify(FlashBuf,(WORD)FreezeMinInfo.AllAttDataLen[PlanIndex], REPAIR_CRC);
			////写入此条冻结数据，写失败，调用先擦后写函数进行写操作
			if(WriteFreezeRecordData( dwAddr, FreezeMinInfo.AllAttDataLen[PlanIndex], dwBaseAddr, FreezeInfoTab[eFREEZE_MIN].dwDataLen, FlashBuf ) == FALSE)
			{
				WriteFreezeRecordData_EraseFlash( dwAddr, FreezeMinInfo.AllAttDataLen[PlanIndex],FlashBuf);
			}
			ProcMinBakFreezeTask(FreezeMinInfo.DataBakAddr[PlanIndex], MinBakAttCycleDepth[PlanIndex].Depth, MinInfo[PlanIndex].RecordNo, FreezeMinInfo.AllAttDataLen[PlanIndex], FlashBuf);

			MinInfo[PlanIndex].RecordNo++;
			if( MinInfo[PlanIndex].SaveDot < AttCycleDepth[PlanIndex].Depth )
			{
				MinInfo[PlanIndex].SaveDot++;
			}
			MinInfo[PlanIndex].CRC32 = lib_CheckCRC32( (BYTE*)&MinInfo[PlanIndex], sizeof(TFreezeDataInfo)-sizeof(DWORD) );

			#if (SLE_THIRD_MEM_BACKUP == YES)
			if( PlanIndex == 0 )//保存方案0时，同步保存FLASH备份数据
			{
				SaveFlashMemData(MinInfo[0].RecordNo-1, bFlag, FlashBuf);
			}
			#endif
		}
	}
}
//--------------------------------------------------
//功能描述:  针对冻结415辨识结果对分钟冻结的特殊处理
//         
//参数:      无
//         
//返回值:    无
//         
//备注:  此函数仅针对分钟冻结，放在api_FreezeTask秒任务中
//--------------------------------------------------
void  DealMinFreeze( void )
{
	TRealTimer t;

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
	if((t.Sec > 0x05)&&(t.Sec < 0x09))
	{
		if(minFreezeFlag == 0xA5)		//此处标志需要上电初始化minFreezeFlag == 0xA5
		{
			minFreezeFlag = 0x5A;
			SecondBufferTime = FREEZEBUFFERTIME;
			SetFreezeFlag(eFREEZE_MIN);
		}
	}
	if(SecondBufferTime > 0)
	{
		SecondBufferTime --;
	}
	if((SecondBufferTime == 0)&&(minFreezeFlag == 0x5A))
	{
		minFreezeFlag = 0xA5;	
	}
}
//-----------------------------------------------
//函数功能: 处理除分钟冻结之外的冻结任务
//
//参数: 	无
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
void api_FreezeTask( void )
{
	BYTE i;
	TRealTimer t;
	
	// 秒任务
	if( api_GetTaskFlag(eTASK_FREEZE_ID,eFLAG_SECOND) == TRUE )
	{		
		api_ClrTaskFlag(eTASK_FREEZE_ID,eFLAG_SECOND);
		
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
		
		if( (t.Min==0x01) && (t.Sec==(eTASK_FREEZE_ID*3+3+0)) )
		{
			//每小时保存一次 每小时的第一分钟，24~26秒之间
			SaveMinFreezePoint();
		}

		if (t.Sec== 0x05)
		{
			DealMinFreeze();
		}
		
		if( wInstantFreezeTimer != 0 )
		{
			wInstantFreezeTimer--;
			wInstantFreezeTimerBak = (WORD)(~wInstantFreezeTimer);
			if( wInstantFreezeTimer == 0 )
			{
				SetFreezeFlag(eFREEZE_INSTANT);
			}
		}		
	}
	
	//分钟任务
	if( api_GetTaskFlag(eTASK_FREEZE_ID,eFLAG_MINUTE) == TRUE )
	{	
		SetFreezeFlag(eFREEZE_MIN);
		api_ClrTaskFlag(eTASK_FREEZE_ID,eFLAG_MINUTE);
	}
	
	//小时任务
	if( api_GetTaskFlag(eTASK_FREEZE_ID,eFLAG_HOUR) == TRUE )
	{		
		api_ClrTaskFlag(eTASK_FREEZE_ID,eFLAG_HOUR);

		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
		
		// 检查结算日,转存电能,月度用电量转存
		CheckClosing();									
		
		#if( PREPAY_MODE == PREPAY_LOCAL ) 
		//函数功能: 阶梯结算日检查电能，用于保存阶梯结算时电能底码，用于计算阶梯用电量
		if( api_CheckLadderClosing() == TRUE )
		{
			SetFreezeFlag(eFREEZE_LADDER_CLOSING);
		}	
		#endif
		
		//处理小时冻结
		SetFreezeFlag(eFREEZE_HOUR);
		//处理日冻结
		if( t.Hour == 0 )
		{
			SetFreezeFlag(eFREEZE_DAY);
		}
		
		//处理月冻结
		if( (t.Hour==0) && (t.Day==1) )
		{
			SetFreezeFlag(eFREEZE_MON);
		}
	}
	
	if( FreezeFlag != 0 )
	{
	    FunctionConst(FREEZE_RUN_TASK);
	    
		for(i=0; i<=eFREEZE_MAX_NUM; i++)
		{
			//冻结任务处理，此处更改时，记得更改冻结上电处理函数：api_PowerUpFreeze，两处处理应相似----李丕凯
			if( FreezeFlag & (0x0001<<i) )
			{
				if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )//掉电检测
				{
					break;
				}
				
				CLEAR_WATCH_DOG;//清除看门狗
				ClrFreezeFlag(i);
				if( i == eFREEZE_MIN )
				{
					ProcMinFreezeTask();
				}
				else if( i == eFREEZE_MAX_NUM )//第一结算日冻结，需要清需量
				{
					//清需量，在所有的冻结都结束后处理清需量 		
					// 月结算电能，用于月累计电能计算
					api_ClosingMonEnergy(); 				
					#if( MAX_PHASE == 3 )
					#if( SEL_DEMAND_2022 == NO )
					api_ClrDemand(eClearAllDemand);
					#else
					api_ClrDemand(eClearAllDemand,eDemandRun);
					#endif
					#endif
				}
				else
				{
					ProcFreezeTask(i,0x00);
					
					#if( PREPAY_MODE == PREPAY_LOCAL ) 
					if( i == eFREEZE_LADDER_CLOSING )
					{
						api_ClrCurLadderUseEnergy(); //清除当前阶梯结算用电量
					}
					#endif
				}
			}
		}
	}
}


//-----------------------------------------------
//函数功能: 获取所有/指定OAD的长度
//
//参数: 
//	Type[in]:	eALL_OAD_LEN/eSELECTED_OAD_LEN               
//  pData[in]:	
//                
//  返回值:	返回 所有/指定OAD的数据长度
//
//备注:返回长度不带sum和的长度	
//-----------------------------------------------
static WORD GetFreezeAttDataLen( BYTE Type, TFreezeData *pData )
{	
	BYTE i,j,OadIndex;
	WORD wLen,ReturnLen;
	DWORD dwOad,dwTemp;

	ReturnLen = 0;

	if( Type == eSELECTED_OAD_LEN )
	{
		for( i=0; i<pData->pDLT698RecordPara->OADNum; i++ )
		{
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );

			for( j=0; j<pData->pAllInfo->NumofOad; j++ )
			{
				if( 	(dwOad==pData->pAttOad[j].Oad)
					||	(dwOad==FREEZE_RECORD_NO_OAD)
					||	(dwOad==FREEZE_RECORD_TIME_OAD) )
				{
					break;
				}
			}			

			if( j < pData->pAllInfo->NumofOad )
			{
				wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );	
				if( wLen == 0x8000 )
				{
					return 0;
				}
			}
			else						
			{
				//是否是存了索引0，读非0索引
				wLen = 0;
				
				OadIndex = (BYTE)(dwOad>>24);	//取元素索引
				dwOad &= ~0xFF000000;			//将OAD的元素索引置为0

				for( j=0; j<pData->pAllInfo->NumofOad; j++ )
				{
					if( pData->pAttOad[j].Oad == dwOad )
					{
						break;
					}
				}

				if( j < pData->pAllInfo->NumofOad )
				{
					dwTemp = OadIndex;		//元素索引
					dwTemp = (dwTemp<<24);
					dwOad |= dwTemp;		//还原原来带元素索引的OAD
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
					if( wLen == 0x8000 )
					{
						return 0;
					}
				}
				else
				{
					wLen = 1;	// 不支持的OAD，返回一个0,长度为1
				}								
			}	
			
			ReturnLen += wLen;	
		}
	}
	else if( Type == eALL_OAD_LEN )
	{
		for( i=0; i<pData->pAllInfo->NumofOad; i++ )
		{
			dwOad = pData->pAttOad[i].Oad;
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )
			{
				return 0;
			}
			ReturnLen += wLen;		
		}
		dwOad = FREEZE_RECORD_NO_OAD;
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return 0;
		}
		ReturnLen += wLen;

		dwOad = FREEZE_RECORD_TIME_OAD;
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return 0;
		}
		ReturnLen += wLen;
	}

	return ReturnLen;
}

//-----------------------------------------------
//函数功能: 698读取冻结关联对象属性表处理
//
//参数: 
// 	OAD[in]:		要查找的OAD
//	Num[in]:		OAD个数                   
//  AttOad[out]: 	实际的关联对象属性表
//				
//返回值:	查找到的偏移 0x80未查到
//
//备注:	
//-----------------------------------------------
BYTE DealEnergyOADFor698( DWORD OAD, BYTE Num, TFreezeAttOad *pAttOad )
{
	BYTE i;
	WORD OI;
	BYTE *pClassAttribute;
	TFreezeAttOad AttOad[MAX_FREEZE_ATTRIBUTE+1];

	lib_ExchangeData((BYTE*)&OI,(BYTE*)&OAD,2);
	if( OI > 0x1000 )//非电能OAD不搜索
	{
		return 0x80;
	}
	
	if( Num > MAX_FREEZE_ATTRIBUTE )
	{
		return 0x80;
	}

	memcpy( &AttOad, pAttOad, sizeof(TFreezeAttOad)*Num );
	
	for( i = 0; i < Num; i++ )
	{
		lib_ExchangeData((BYTE*)&OI,(BYTE*)&AttOad[i].Oad,2);
		pClassAttribute = (BYTE*)&AttOad[i].Oad+2;
		if( OI < 0x1000 )//电能 将高精度电能转化为普通电能
		{
			if( (*pClassAttribute&0x1f) == 0x04 )//高精度电能
			{
				*pClassAttribute = (*pClassAttribute&0xf8)+0x02;
			}
		}

		if( OAD == AttOad[i].Oad )//与普通电能相等
		{
			return i;
		}
	}

	return 0x80;
}

//-----------------------------------------------
//函数功能: 按照冻结序号查找数据
//
//参数:            
//  pData[in]:	输入信息
//                
//  返回值:	返回数据长度 0--代表空间不足 0x8000--异常
//
//备注:	
//-----------------------------------------------
WORD ReadFreezeByRecordNo( TFreezeData	*pData )
{	
	BYTE i,j,k,OadIndex;
	WORD Offset,wLen,wTempLen,TempOffset,OI,m,Result;
	DWORD dwAddr,dwOad,dwTempOad,dwTemp1;
	BYTE FreezeBuf[2000];//冻结数据
	TSectorInfo	SectorInfo;
	TRealTimer 	Time;
	TReadFreezeRecordInfo FreezeRecordInfo;
	
	Offset = 0;
	
	if( pData->pDLT698RecordPara->OADNum == 0 )
	{		
		wLen = GetFreezeAttDataLen( eALL_OAD_LEN, pData );
		if( wLen > pData->Len )
		{
			return 0;// 空间不足
		}
		
		//分钟冻结RCSD不能为0，因此，此处不会是分钟冻结
		//获取此条数据的地址
		if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, pData->RecordNo, &SectorInfo ) == FALSE )
		{
			return 0x8000;
		}

		if(pData->pAllInfo->AllAttDataLen>2000)
		{
			return 0x8000;
		}
		//记得检查是否有释放缓冲
		FreezeRecordInfo.dwAddr = pData->pFreezeAddrLen->dwDataAddr+SectorInfo.dwAddr;//flash存储地址
		FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//整条长度
		FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
		FreezeRecordInfo.RecordNo = pData->RecordNo;//冻结记录序号
		#if( MAX_PHASE == 1 )
		//单相表掉电从E2读取数据
		if(( api_GetSysStatus(eSYS_STATUS_POWER_ON) != TRUE ) && (pData->pDLT698RecordPara->Ch == 0X55))
		{
			//从E2读出整条数据
			if(ReadFreezeRecordEeData( &FreezeRecordInfo, FreezeBuf ) != TRUE)
			{
				return 0x8000;
			}
		}
		else
		#endif	
		{
			//读出整条数据
			if(ReadFreezeRecordData( &FreezeRecordInfo, FreezeBuf ) != TRUE)
			{
				return 0x8000;
			}
		}
		//填充RecordNo	
		dwOad = FREEZE_RECORD_NO_OAD;
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return 0x8000;
		}
		
		if( pData->Tag == eTagData )
		{
			api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, (BYTE *)&pData->RecordNo, 0xFF, pData->pBuf+Offset );	
		}
		else
		{
			memcpy( pData->pBuf+Offset, (BYTE *)&pData->RecordNo, wLen );
		}
		Offset += wLen;

		//填充Time
		dwOad = FREEZE_RECORD_TIME_OAD;
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return 0x8000;
		}
		memcpy((BYTE *)&dwTemp1, FreezeBuf, sizeof(DWORD) );
		api_ToAbsTime( dwTemp1, (TRealTimer *)&Time );
		if( pData->Tag == eTagData )
		{
			api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, (BYTE *)&Time, 0xFF, pData->pBuf+Offset );	
		}
		else
		{
			memcpy( pData->pBuf+Offset, (BYTE *)&Time, wLen );
		}
		Offset += wLen;
		
		//填充各个属性数据				
		for( i=0;  i<pData->pAllInfo->NumofOad; i++ )
		{
			if( pData->pAttCycleDepth->Depth == 0xFFFF )
			{
				break;					
			}
			
			CLEAR_WATCH_DOG;

			dwOad = pData->pAttOad[i].Oad;
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )
			{
				return 0x8000;
			}
			
            if( (Offset+ wLen) > pData->Len )//避免超限
            {
                return 0; 	// 缓冲满	
            }
            
			ASSERT(pData->pAttOad[i].AttDataLen<2000, 0);
			//若读出的数据是结算需量数据，且数据为ff，则置为null
			m = 0xfffc;//赋初值，避免为0和ffff

			#if( MAX_PHASE == 3 )
			if( ( dwOad == ZEROCURRRNT_OAD ) 
			|| ( dwOad == CURRRNTVECTORSUM_OAD ) )
			{
				for(m=0; m<pData->pAttOad[i].AttDataLen; m++)
				{
					if(FreezeBuf[pData->pAttOad[i].Offset+m] != 0xff)
					{
						break;
					}
				}
			}
			else 
			#endif
			if( pData->FreezeIndex == eFREEZE_CLOSING )
			{
				OI = (dwOad&0xff)*0x100+((dwOad>>8)&0xff);
				if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
				{
					for(m=0; m<pData->pAttOad[i].AttDataLen; m++)
					{
						if(FreezeBuf[pData->pAttOad[i].Offset+m] != 0xff)
						{
							break;
						}
					}
				}
			}
			
			//若读出的数据是结算需量数据，且数据为ff，则置为null
			if( m == pData->pAttOad[i].AttDataLen )
			{
				pData->pBuf[Offset] = 0;
				wLen = 1;
			}
			else
			{
				if( pData->Tag == eTagData )
				{
					api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, FreezeBuf+pData->pAttOad[i].Offset, 0xFF, pData->pBuf+Offset );	
				}
				else
				{
					//电能使用冻结长度传入，电能进行转化后 会将wLen转化为实际长度
					wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );
					//前面判断长度了 可以直接相减
					Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, FreezeBuf+pData->pAttOad[i].Offset, pData->Len -Offset, pData->pBuf+Offset );
					if( (Result == 0x8000) || (Result == 0) )
					{
						return Result;
					}
				}
			}
			
			Offset += wLen;			
		}
	}
	else
	{		
		wLen = GetFreezeAttDataLen( eSELECTED_OAD_LEN, pData );
		if( wLen > pData->Len )
		{
			return 0;// 空间不足
		}
		
		if( pData->FreezeIndex == eFREEZE_MIN )//分钟冻结地址单独计算
		{
			
			//获取此条数据的地址
			if(GetSectorInfo( pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex], pData->pAttCycleDepth->Depth, pData->RecordNo, &SectorInfo ) == FALSE )
			{
				return 0x8000;
			}
			
			FreezeRecordInfo.dwAddr = pData->pMinInfo->DataAddr[pData->MinPlanIndex]+SectorInfo.dwAddr;
			FreezeRecordInfo.RecordLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//整条长度
			FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
			FreezeRecordInfo.RecordNo = pData->RecordNo;//冻结记录序号
			FreezeRecordInfo.MinPlan = pData->MinPlanIndex;
			FreezeRecordInfo.dwBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];		// 分钟冻结 备份地址
		}
		else
		{
			//获取此条数据的地址
			if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, pData->RecordNo, &SectorInfo ) == FALSE )
			{
				return 0x8000;
			}

			if(pData->pAllInfo->AllAttDataLen>2000)
			{
				return 0x8000;
			}
			//记得检查是否有释放缓冲
			FreezeRecordInfo.dwAddr = pData->pFreezeAddrLen->dwDataAddr+SectorInfo.dwAddr;//flash存储地址
			FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//整条长度
			FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
			FreezeRecordInfo.RecordNo = pData->RecordNo;//冻结记录序号

		}

		#if( MAX_PHASE == 1 )
		//单相表掉电从E2读取数据
		if(( api_GetSysStatus(eSYS_STATUS_POWER_ON) != TRUE ) && (pData->pDLT698RecordPara->Ch == 0X55))
		{
			//从E2读出整条数据
			if(ReadFreezeRecordEeData( &FreezeRecordInfo, FreezeBuf ) != TRUE)
			{
				return 0x8000;
			}
		}
		else
		#endif	
		{
			//读出整条数据
			if(ReadFreezeRecordData( &FreezeRecordInfo, FreezeBuf ) != TRUE)
			{
				return 0x8000;
			}
		}


		for( i=0;  i<pData->pDLT698RecordPara->OADNum; i++ )
		{
			CLEAR_WATCH_DOG;
			
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
			
			// 找相应的OAD
			dwTempOad = dwOad & (~0x00E00000);	
			if( (dwTempOad == FREEZE_RECORD_NO_OAD) || (dwTempOad == FREEZE_RECORD_TIME_OAD) )
			{
				dwOad = dwOad & (~0x00E00000);
				j = 0;
				
			}
			else
			{
				for( j=0; j<pData->pAllInfo->NumofOad; j++ )
				{												
					if( dwOad == pData->pAttOad[j].Oad )
					{
						break;
					}
				}
			}

			if( j == pData->pAllInfo->NumofOad )//如果是抄读普通电能，置位oad再搜索一遍
			{
				j = DealEnergyOADFor698( dwOad, pData->pAllInfo->NumofOad, pData->pAttOad );
				if( j == 0x80 )//未搜到
				{
					j = pData->pAllInfo->NumofOad;
				}
			}
			
			if( j < pData->pAllInfo->NumofOad )
			{
				
				if( dwOad == FREEZE_RECORD_NO_OAD )
				{
					//填充RecordNo	
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
					if( wLen == 0x8000 )
					{
						return 0x8000;
					}

					if( (Offset+ wLen) > pData->Len )//避免超限
					{
						return 0; 	// 缓冲满	
					}
                    
					if( pData->Tag == eTagData )
					{
						api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, (BYTE *)&pData->RecordNo, 0xFF, pData->pBuf+Offset );	
					}
					else
					{
						memcpy( pData->pBuf+Offset, (BYTE *)&pData->RecordNo, wLen );
					}
					Offset += wLen;
				}
				else if( dwOad == FREEZE_RECORD_TIME_OAD )
				{
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
					if( wLen == 0x8000 )
					{
						return 0x8000;
					}

					if( (Offset+ wLen) > pData->Len )//避免超限
					{
						return 0; 	// 缓冲满	
					}

					memcpy((BYTE *)&dwTemp1, FreezeBuf, sizeof(DWORD) );
				
					api_ToAbsTime( dwTemp1, (TRealTimer *)&Time );
					if( pData->Tag == eTagData )
					{
						api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, (BYTE *)&Time, 0xFF, pData->pBuf+Offset );	
					}
					else
					{
						memcpy( pData->pBuf+Offset, (BYTE *)&Time, wLen );
					}
					Offset += wLen;
				}
				else
				{
					// 检查存了多少个电器
					k = 0;
					memset((BYTE*)&tApplFrzDataFLASH[0].ApplClassID,0xFF,sizeof(tApplFrzDataFLASH));
					for (wTempLen = 0; wTempLen < NILM_EQUIPMENT_MAX_NO; wTempLen++)
					{
						if( memcmp( FreezeBuf + pData->pAttOad[j].Offset + (WORD)wTempLen*OI4800_04_SIZE, bConstTempBuf,OI4800_04_SIZE) != 0 )
						{
							k++;// 非全0xff
							memcpy((BYTE*)&tApplFrzDataFLASH[wTempLen].ApplClassID,(BYTE *)&FreezeBuf[pData->pAttOad[j].Offset + (WORD)wTempLen*OI4800_04_SIZE],sizeof(T_ApplFrzData));//解决掉电再读取冻结，一些数据不显示问题
						}
						else
						{
							break;
						}
					}
					api_SetTagArrayNum(k);													
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );

					if( wLen == 0x8000 )
					{
						return 0x8000;
					}
					
					if( (Offset+ wLen) > pData->Len )//避免超限
					{
						return 0; 	// 缓冲满	
					}
					m = 0xfffc;//赋初值，避免为0和ffff

					#if( MAX_PHASE == 3 )
					if( ( dwOad == ZEROCURRRNT_OAD ) 
					|| ( dwOad == CURRRNTVECTORSUM_OAD ) )
					{
						for(m=0; m<pData->pAttOad[j].AttDataLen; m++)
						{
							if(FreezeBuf[pData->pAttOad[j].Offset+m] != 0xff)
							{
								break;
							}
						}
					}
					else 
					#endif
					//若读出的数据是结算需量数据，且数据为ff，则置为null						
					if( pData->FreezeIndex == eFREEZE_CLOSING )
					{
						OI = (dwOad&0xff)*0x100+((dwOad>>8)&0xff);
						if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
						{
							for(m=0; m<pData->pAttOad[j].AttDataLen; m++)
							{
								if(FreezeBuf[pData->pAttOad[j].Offset+m] != 0xff)
								{
									break;
								}
							}
						}
					}
						
									
					//若读出的数据是结算需量数据，且数据为ff，则置为null
					if( m == pData->pAttOad[j].AttDataLen )
					{
						pData->pBuf[Offset] = 0;
						wLen = 1;
					}
					else
					{
						if( pData->Tag == eTagData )
						{
							//数据所在位置为基地址加偏移地址
							api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, FreezeBuf+pData->pAttOad[j].Offset, 0xFF, pData->pBuf+Offset );	
							
						}
						else
						{
							//电能使用冻结长度传入
							wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );

							//前面判断长度了 可以直接相减
							Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, FreezeBuf+pData->pAttOad[j].Offset,pData->Len -Offset,pData->pBuf+Offset );
							if( (Result == 0x8000) || (Result == 0) )
							{
								return Result;
							}
						}
					}
					
					Offset += wLen;	
				}	
				
			}
			else		
			{
				//指定OAD在关联属性对象表中没有找到，则将元素索引置为0，再找一遍
				OadIndex = (BYTE)(dwOad>>24);
				dwTempOad = dwOad & (~0xFF000000);
				for( j=0; j<pData->pAllInfo->NumofOad; j++ )
				{
					if( dwTempOad == pData->pAttOad[j].Oad )
					{
						break;
					}
				}
				
				if( j == pData->pAllInfo->NumofOad )//如果是抄读普通电能，置位oad再搜索一遍
				{
					j = DealEnergyOADFor698( dwTempOad, pData->pAllInfo->NumofOad, pData->pAttOad );
					if( j == 0x80 )//未搜到
					{
						j = pData->pAllInfo->NumofOad;
					}
				}
				
				//存了索引0，读非0索引
				if( j < pData->pAllInfo->NumofOad )
				{					
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
					if( wLen == 0x8000 )
					{
						return 0x8000;
					}
					
					if( (Offset+ wLen) > pData->Len )//避免超限
					{
						return 0; 	// 缓冲满	
					}

					//元素索引合法性判断
					if( (OadIndex>VALID_OAD_INDEX) || (OadIndex==0) )
					{
						OadIndex = 1;	// 防止出错
					}
					TempOffset = 0;
		
					//查找指定元素索引对应在存储OAD数据的偏移地址pTemp
					for( k=1; k<OadIndex; k++ ) // pData->OadIndex 从1开始
					{
						dwTemp1 = k;
						dwTempOad = ( (dwOad & (~0xFF000000)) | (dwTemp1<<24) );	
						wTempLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwTempOad, 0 );
						if( wTempLen == 0x8000 )
						{
							return 0x8000;
						}			
						TempOffset += wTempLen;
					}
					
					//若读出的数据是结算需量数据，且数据为ff，则置为null
					m = 0xfffe;//赋初值，避免为0和ffff						
					if( pData->FreezeIndex == eFREEZE_CLOSING )
					{
						OI = (dwOad&0xff)*0x100+((dwOad>>8)&0xff);
						if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
						{
							for(m=0; m<wLen; m++)
							{
								if(FreezeBuf[pData->pAttOad[j].Offset+TempOffset+m] != 0xff)
								{
									break;
								}
							}
						}
					}
					if( m == wLen )
					{
						pData->pBuf[Offset] = 0;
						wLen = 1;
					}
					else
					{
						if( pData->Tag == eTagData )
						{
							//数据所在位置为基地址加偏移地址
							api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, FreezeBuf+pData->pAttOad[j].Offset+TempOffset, 0xFF, pData->pBuf+Offset );

						}
						else
						{
							//电能使用冻结长度传入，电能进行转化后 会将wLen转化为实际长度
							wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );

							//前面判断长度了 可以直接相减
							Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, FreezeBuf+pData->pAttOad[j].Offset+TempOffset, pData->Len -Offset, pData->pBuf+Offset );
							if( (Result == 0x8000) || (Result == 0) )
							{
								return Result;
							}
							
						}
					}
					
					Offset += wLen;	
				}
				else
				{
					pData->pBuf[Offset] = 0x00;
					Offset += 1;	
					continue;
				}
			}
		}
	}
	
	return Offset;
}

//-----------------------------------------------
//函数功能: 读上N次冻结记录
//
//参数:            
//  pData[in]:	输入信息
//                
//  返回值:	返回数据长度 0--代表空间不足 0x8000--异常
//
//备注:	
//-----------------------------------------------
WORD ReadFreezeByLastNum( TFreezeData	*pData )
{		
	if( pData->pDLT698RecordPara->TimeOrLast == 0 )
	{
		return 0x8000;
	}	
	
	//将要读的上几次与已经冻结的条数进行判断
	if( pData->pDLT698RecordPara->TimeOrLast > pData->pDataInfo->SaveDot )
	{
		return 0x8000;
	}
	
	//要查找上几次对应的RecordNo
	pData->RecordNo = pData->pDataInfo->RecordNo - (pData->pDLT698RecordPara->TimeOrLast);	
	
	return ReadFreezeByRecordNo(pData);	
}

//-----------------------------------------------
//函数功能: 按照指定时间查找冻结记录号
//
//参数:            
//  pData[in]:	输入信息
//                
//  返回值:	返回数据长度 0--代表空间不足 0x8000--异常
//
//备注:	
//-----------------------------------------------
BOOL SearchFreezeRecordNoByTime( TFreezeData	*pData )
{
	DWORD				i;
	DWORD				dwTemp,dwAddr,dwBaseAddr,dwEndAddr,dwTimeAddr,dwStartTime,dwEndTime,dwRecordNo;
	DWORD				SearchStartRecordNo,SearchEndRecordNo;
	TFreezeDataInfo		*pDataInfo;
	TSectorInfo			SectorInfo;
	TBinarySchInfo		BinarySchInfo;
	TReadFreezeRecordInfo	FreezeRecordInfo;
	pDataInfo = pData->pDataInfo;
	DWORD RealSaveDot;
	BYTE IsTRUE = TRUE;

	// 有数据的情况
	if( pDataInfo->SaveDot != 0 )
	{							
		if( pData->FreezeIndex < eFREEZE_HOUR )
		{
			//非周期冻结类，在修改时间时不影响之前的数据，继续往下保存，这样时标可能不是单调递增的，
			//因此在按照时间查找的时候从最近的一次事件开始比较，找到最合适的一次就是，不再继续往下找
			dwBaseAddr = pData->pFreezeAddrLen->dwDataAddr;// 数据基地址
			dwStartTime = 0;
			dwEndTime = 0;
			SearchStartRecordNo = pDataInfo->RecordNo-1;
			SearchEndRecordNo = pDataInfo->RecordNo-1;
			
			//从最近一个点往前找
			for(i=0; i<pDataInfo->SaveDot; i++)
			{
				CLEAR_WATCH_DOG;
				if(i >500)//防止死在for中,非周期性冻结深度最大为结算日冻结，最多需要12次，留余量
				{
					break;
				}
				if( pDataInfo->RecordNo >= i )
				{
					SearchStartRecordNo = pDataInfo->RecordNo-i-1;
					
					if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, SearchStartRecordNo, &SectorInfo ) == FALSE)
					{
						break;
					}
					dwAddr = dwBaseAddr + SectorInfo.dwAddr;
					FreezeRecordInfo.dwAddr = dwAddr;//flash存储地址
					FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//整条长度
					FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
					FreezeRecordInfo.RecordNo = SearchStartRecordNo;//冻结记录序号
					
					if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwStartTime )==FALSE)
					{
						//错误情况下不处理
					}
					if( pData->pDLT698RecordPara->TimeOrLast == dwStartTime )
					{
						//这个点正好是要查找的点，返回这个点的RecordNo
						pData->RecordNo = SearchStartRecordNo;								// 找到的序号
						memcpy( (BYTE *)&(pData->SearchTime), (BYTE *)&dwStartTime, sizeof(DWORD) );	// 返回找到的时间
						break;
					}
					else if( 	(pData->pDLT698RecordPara->TimeOrLast > dwStartTime)
						 	&& 	(pData->pDLT698RecordPara->TimeOrLast <= dwEndTime) )
					{
						//要查找的点时间大于这个点的时间，且小于等于上次查找点的时间，返回上次的时间，i为0时肯定进不来
						if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, SearchEndRecordNo, &SectorInfo ) == FALSE)
						{
							return FALSE;
						}
						dwAddr = dwBaseAddr + SectorInfo.dwAddr;
						FreezeRecordInfo.dwAddr = dwAddr;//flash存储地址
						FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//整条长度
						FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
						FreezeRecordInfo.RecordNo = SearchEndRecordNo;//冻结记录序号
						
						if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&(pData->SearchTime) )==FALSE)
						{
							//错误情况下不处理
						}
						pData->RecordNo = SearchEndRecordNo;								// 找到的序号
						break;
					}
					else
					{
						//考虑数据库面时间都一样的情况，返回最近一个时间
						if( dwEndTime != dwStartTime )
						{
							dwEndTime = dwStartTime;
							SearchEndRecordNo = SearchStartRecordNo;
						}
					}														
				}												
			}
			
			//有效点都找完后找不到符合条件的，再判断一下
			if( i == pDataInfo->SaveDot )
			{
				if(pData->pDLT698RecordPara->TimeOrLast < dwEndTime)
				{
					if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, SearchEndRecordNo, &SectorInfo ) == FALSE)
					{
						return FALSE;
					}
					dwAddr = dwBaseAddr + SectorInfo.dwAddr;
					FreezeRecordInfo.dwAddr = dwAddr;//flash存储地址
					FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//整条长度
					FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
					FreezeRecordInfo.RecordNo = SearchEndRecordNo;//冻结记录序号
					
					if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&(pData->SearchTime) )==FALSE)
					{
						//错误情况不处理
					}
					pData->RecordNo = SearchEndRecordNo;									// 找到的序号
				}
				else
				{
					return FALSE;
				}
			}
		}
		else
		{
			//周期冻结类，所有的时标都是单调递增的，采用二分法查找
			// 找结束时间 最近一个数据
			if( pData->FreezeIndex == eFREEZE_MIN )
			{
				//得到基地址
				dwBaseAddr = pData->pMinInfo->DataAddr[pData->MinPlanIndex];
				//计算此数据保存的绝对地址
				if(GetSectorInfo( pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex], pData->pAttCycleDepth->Depth, pDataInfo->RecordNo-1, &SectorInfo ) == FALSE )
				{
					return FALSE;
				}
				FreezeRecordInfo.RecordLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//整条长度
				FreezeRecordInfo.dwBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];
				FreezeRecordInfo.MinPlan = pData->MinPlanIndex;
			}
			else
			{
				// 数据基地址
				dwBaseAddr = pData->pFreezeAddrLen->dwDataAddr;
				GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, pDataInfo->RecordNo-1, &SectorInfo );
				FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//整条长度
				
			}
			dwEndAddr = dwBaseAddr + SectorInfo.dwAddr;

			FreezeRecordInfo.dwAddr = dwEndAddr;//flash存储地址
			FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
			FreezeRecordInfo.RecordNo = pDataInfo->RecordNo-1;//冻结记录序号
			
			if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwEndTime )==FALSE)
			{
				//错误情况下不处理
			}	
			
			// 找起始时间 最开始一个数据
			if( pData->FreezeIndex == eFREEZE_MIN )
			{
				GetSectorInfo( pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex], pData->pAttCycleDepth->Depth, (pDataInfo->RecordNo-pDataInfo->SaveDot), &SectorInfo );
				FreezeRecordInfo.RecordLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//整条长度
				FreezeRecordInfo.dwBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];
				FreezeRecordInfo.MinPlan = pData->MinPlanIndex;
			}
			else
			{
				GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, (pDataInfo->RecordNo-pDataInfo->SaveDot), &SectorInfo );
				FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//整条长度
			}
			dwTimeAddr = dwBaseAddr + SectorInfo.dwAddr;
			FreezeRecordInfo.dwAddr = dwTimeAddr;//flash存储地址
			FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
			FreezeRecordInfo.RecordNo = (pDataInfo->RecordNo-pDataInfo->SaveDot);//冻结记录序号
			
			if(ReadFreezeRecordTime( &FreezeRecordInfo,  (DWORD*)&dwStartTime  ) == FALSE)
			{
				//错误情况下, 将备份区的第一条数据返回
				if(FreezeRecordInfo.FreezeIndex == eFREEZE_MIN)
				{
					TFreezeAddrLen FreezeAddrLen;
					TFreezeAttCycleDepth AttCycleDepth;

					//获得 分钟冻结备份区的 冻结深度
					GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen );
					if(api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (FreezeRecordInfo.MinPlan + MAX_FREEZE_PLANNUM), sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth ) == TRUE)
					{
						if(pDataInfo->RecordNo >= AttCycleDepth.Depth)
						{
							IsTRUE = FALSE;
							FreezeRecordInfo.RecordNo = (pDataInfo->RecordNo - AttCycleDepth.Depth);
							ReadFreezeRecordTime( &FreezeRecordInfo,  (DWORD*)&dwStartTime  );
							RealSaveDot = AttCycleDepth.Depth;
						}
					}
				}
				else if(FreezeInfoTab[FreezeRecordInfo.FreezeIndex].wEeDepth != 0)
				{
					if(pDataInfo->RecordNo >= FreezeInfoTab[FreezeRecordInfo.FreezeIndex].wEeDepth)
					{
						IsTRUE = FALSE;
						FreezeRecordInfo.RecordNo = (pDataInfo->RecordNo - FreezeInfoTab[FreezeRecordInfo.FreezeIndex].wEeDepth);
						ReadFreezeRecordTime( &FreezeRecordInfo,  (DWORD*)&dwStartTime  );
						RealSaveDot = FreezeInfoTab[FreezeRecordInfo.FreezeIndex].wEeDepth;
					}
				}
			}

			dwTemp = 0;

			if( pData->pDLT698RecordPara->TimeOrLast <= dwStartTime )
			{
				//若要找的时间小于起始时间，则将第一个数据返回	
				if(IsTRUE == TRUE)
				{
					pData->RecordNo = (pDataInfo->RecordNo - pDataInfo->SaveDot);				// 第一个数据的序号
				}
				else
				{
					pData->RecordNo = FreezeRecordInfo.RecordNo;
				}
				
				memcpy((BYTE *)&(pData->SearchTime),(BYTE *)&dwStartTime,sizeof(DWORD));	// 返回找到的时间
				
			}
			else if( 	( pData->pDLT698RecordPara->TimeOrLast > dwStartTime ) 
					&& 	( pData->pDLT698RecordPara->TimeOrLast <= dwEndTime ) )
			{
				//若找的时间在起始时间与结束时间之间，则需要二分法查找
				if( pData->FreezeIndex == eFREEZE_MIN )
				{
					BinarySchInfo.wLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//整条长度;
					BinarySchInfo.MinBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];
					BinarySchInfo.MinPlan = pData->MinPlanIndex;
				}
				else
				{
					BinarySchInfo.wLen = pData->pAllInfo->AllAttDataLen;
				}
				BinarySchInfo.dwBaseAddr = dwBaseAddr;
				BinarySchInfo.dwDepth = pData->pAttCycleDepth->Depth;
				BinarySchInfo.dwRecordNo = pDataInfo->RecordNo;
				if( IsTRUE == TRUE )
				{
					BinarySchInfo.dwSaveDot = pDataInfo->SaveDot;
				}
				else
				{
					BinarySchInfo.dwSaveDot = RealSaveDot;
				}
				BinarySchInfo.Time = pData->pDLT698RecordPara->TimeOrLast;
				BinarySchInfo.pRecordNo = &dwRecordNo;
				BinarySchInfo.pTime = &dwTemp;
                BinarySchInfo.FreezeIndex = pData->FreezeIndex;
				if( BinSearchByTime( &BinarySchInfo ) == TRUE )
				{
					pData->RecordNo = dwRecordNo;											// 找到的序号
					memcpy((BYTE *)&(pData->SearchTime), (BYTE*)&dwTemp, sizeof(DWORD));	// 返回找到的时间
				}
				else
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}		
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读指定时间数据
//
//参数:            
//  pData[in]:	输入信息
//                
//  返回值:	返回数据长度 0--代表空间不足 0x8000--异常
//
//备注:	
//-----------------------------------------------
WORD ReadFreezeByTime( TFreezeData	*pData )
{
	BOOL Result;
	
	Result = SearchFreezeRecordNoByTime(pData);
	
	if( Result == TRUE )
	{
		return ReadFreezeByRecordNo(pData);	
	}
	else
	{
		return 0x8000;
	}	
}

//-----------------------------------------------
//函数功能: 二分法找Time对应的地址
//
//参数: 
//	pBinSchInfo[in]:	指向冻结属性表的指针
//			DWORD		dwBaseAddr;			in 此属性对应数据的首地址
//			DWORD		dwDepth;			in 冻结深度
//			DWORD		dwSaveDot;			in 需要传递剩余的冻结记录个数，进行二分法查找时用到
//			DWORD		dwRecordNo;			in 最近一个点的记录号
//			DWORD		Time;				in 要搜索的时间
//			DWORD		*pRecordNo;			out 返回找到的RecordNo
//			DWORD		*pTime;				out 返回找到的时间
//			WORD		wLen;				in 此属性单个数据的长度
//                				
//返回值:	TRUE/FALSE
//
//备注:	
//-----------------------------------------------
static WORD BinSearchByTime( TBinarySchInfo *pBinSchInfo )
{	
	DWORD dwTempTime,dwMidAddr;
	DWORD dwStartRecordNo,dwEndRecordNo,dwMidRecordNo;
	TSectorInfo	SectorInfo;
	TReadFreezeRecordInfo FreezeRecordInfo;
	WORD LoopNum;
	
	if( pBinSchInfo->dwRecordNo == 0 )
	{
		return FALSE;
	}
	
	if( pBinSchInfo->dwRecordNo < pBinSchInfo->dwSaveDot )
	{
		return FALSE;
	}
	
	//找结束记录号
	dwEndRecordNo = pBinSchInfo->dwRecordNo-1;
	
	//计算开始记录号
	dwStartRecordNo = pBinSchInfo->dwRecordNo - pBinSchInfo->dwSaveDot;
	
	LoopNum = 0; 
	//二分法查找
	while( dwStartRecordNo < dwEndRecordNo )
	{
		if((LoopNum++) >50)//防止死在while中,分钟冻结存满28800，最多需要15次，留余量
		{
			break;
		}
		//计算中间记录号，并找到对应记录号数据地址
		dwMidRecordNo = dwStartRecordNo + (dwEndRecordNo-dwStartRecordNo)/2;	
		GetSectorInfo( pBinSchInfo->wLen, pBinSchInfo->dwDepth, dwMidRecordNo, &SectorInfo );
		dwMidAddr = pBinSchInfo->dwBaseAddr+SectorInfo.dwAddr;
		FreezeRecordInfo.dwAddr = dwMidAddr;//flash存储地址
		FreezeRecordInfo.RecordLen = pBinSchInfo->wLen;//整条长度
		FreezeRecordInfo.FreezeIndex = pBinSchInfo->FreezeIndex;//冻结类型
		FreezeRecordInfo.RecordNo = dwMidRecordNo;//冻结记录序号
		FreezeRecordInfo.dwBakAddr = pBinSchInfo->MinBakAddr;
		FreezeRecordInfo.MinPlan = pBinSchInfo->MinPlan;
		//读取此记录号对应时间
		if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwTempTime )==FALSE)
		{
			if(dwStartRecordNo != dwMidRecordNo )
			{
				dwStartRecordNo = dwMidRecordNo ;//保证了会像一个方向偏，二分之一的正确性--李丕凯
			}
			else
			{
				dwStartRecordNo = dwEndRecordNo ;//保证了能够正常退出--李丕凯
			}
			continue;
		}
		if( dwTempTime == pBinSchInfo->Time )
		{
			//返回此数据对应记录号及时标
			*pBinSchInfo->pRecordNo = dwMidRecordNo;
			*pBinSchInfo->pTime = dwTempTime;
			return TRUE;
		}
		else if( dwTempTime > pBinSchInfo->Time )
		{
			dwEndRecordNo = dwMidRecordNo;
		}
		else
		{
			if( dwMidRecordNo == dwStartRecordNo )
			{
				dwStartRecordNo = dwEndRecordNo;
			}
			else
			{  
				dwStartRecordNo = dwMidRecordNo ;
			}
		}
	}

	//若二分法没找到对应时间，则将现在起始记录号对应时间读出
	GetSectorInfo( pBinSchInfo->wLen, pBinSchInfo->dwDepth, dwStartRecordNo, &SectorInfo );
	dwMidAddr = pBinSchInfo->dwBaseAddr+SectorInfo.dwAddr;
	FreezeRecordInfo.dwAddr = dwMidAddr;//flash存储地址
	FreezeRecordInfo.RecordLen = pBinSchInfo->wLen;//整条长度
	FreezeRecordInfo.FreezeIndex = pBinSchInfo->FreezeIndex;//冻结类型
	FreezeRecordInfo.RecordNo = dwStartRecordNo;//冻结记录序号
	FreezeRecordInfo.dwBakAddr = pBinSchInfo->MinBakAddr;
	FreezeRecordInfo.MinPlan = pBinSchInfo->MinPlan;
	//读取此记录号对应时间
	if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwTempTime )==FALSE)
	{
		//错误情况不处理，读取整条数据错误情况下dwTempTime，记录中是啥值返回啥值--李丕凯
	}
	//若此时间大于要搜索的时间，则返回此时间记录号及数据地址
	if( dwTempTime >= pBinSchInfo->Time )
	{
		*pBinSchInfo->pRecordNo = dwStartRecordNo;
		*pBinSchInfo->pTime = dwTempTime;
		return TRUE;
	}
	else
	{
		return FALSE;		
	}
}


//-----------------------------------------------
//函数功能: 按照间隔时间读指定时间区间的冻结数据
//
//参数:            
//  pData[in]:	输入信息
//                
//  返回值:	返回数据长度 0--代表空间不足 0x8000--异常
//
//备注:	
//-----------------------------------------------
static WORD ReadFreezeByTimeInterval( TFreezeData *pData )
{
	BOOL Result;
	WORD wLen,ReturnLen,LoopNum,Mon;
	DWORD i,dwStartRecordNo,dwEndRecordNo;
	DWORD dwStartTime,dwEndTime;
	DWORD dwAddr,dwTimeInterval;
	TSectorInfo		SectorInfo;
	TRealTimer 	Time;
	TReadFreezeRecordInfo FreezeRecordInfo;
	//对周期性冻结，间隔不为0，且TI不为冻结周期时，置标志
	if( pData->FreezeIndex >= eFREEZE_HOUR )
	{
		//当AllCycle标志为55时，不判断间隔
		if( pData->pDLT698RecordPara->TimeIntervalPara.AllCycle != 0x55 )
		{
			if( pData->pDLT698RecordPara->TimeIntervalPara.TI == 0 )
			{
				//不应该出现这种情况，当为0时AllCycle不应该为55
				return 0x8000;
			}
		}
	}

	//若起始时间大于截止时间，返回错误
	if( pData->pDLT698RecordPara->TimeIntervalPara.StartTime >= pData->pDLT698RecordPara->TimeIntervalPara.EndTime )
	{
		return 0x9000;
	}
	
	//将周期性冻结的搜索的起始时间改为能被间隔时间整除的时间
	if( pData->FreezeIndex < eFREEZE_HOUR )
	{
	}
	else
	{
		//下发时间区间间隔为0时 分钟间隔按照分钟冻结周期，其他周期冻结默认为1
		if( pData->pDLT698RecordPara->TimeIntervalPara.AllCycle == 0x55 )
		{
			if( pData->FreezeIndex == eFREEZE_MIN )
			{
				pData->pDLT698RecordPara->TimeIntervalPara.TI = pData->pAttCycleDepth->Cycle;
			}
			else
			{
				pData->pDLT698RecordPara->TimeIntervalPara.TI = 1;
			}
		}
		
		//周期冻结为分钟 小时 日 月冻结
		dwTimeInterval = 1;

		if( pData->FreezeIndex == eFREEZE_MON )
		{
			api_ToAbsTime( pData->pDLT698RecordPara->TimeIntervalPara.StartTime, &Time );
			Mon = (Time.wYear-2000)*12+Time.Mon-1;
			//搜索的月不是正月，则置为下一个整月时间
			if(((Mon % pData->pDLT698RecordPara->TimeIntervalPara.TI) != 0)||
			  (Time.Day != 0x01)||(Time.Hour != 0x00)||(Time.Min != 0x00) )
			{
				Mon = (((Mon/pData->pDLT698RecordPara->TimeIntervalPara.TI)+1)*pData->pDLT698RecordPara->TimeIntervalPara.TI);
				Time.wYear = (2000+(Mon/12));//重新计算年
				Time.Mon = (Mon % 12)+1;
				Time.Day = 0x01;
				Time.Hour = 0x00;
				Time.Min = 0x00;
				pData->pDLT698RecordPara->TimeIntervalPara.StartTime = api_CalcInTimeRelativeMin( &Time );
			}
		}
		else
		{
			//若搜索的时间不能被间隔时间的分钟数整除，则置为下一个能被间隔时间整除的时间
			if( pData->FreezeIndex == eFREEZE_MIN )
			{
				dwTimeInterval = pData->pDLT698RecordPara->TimeIntervalPara.TI;
			}
			else if( pData->FreezeIndex == eFREEZE_HOUR )
			{
				dwTimeInterval = 60L * pData->pDLT698RecordPara->TimeIntervalPara.TI;
			}
			else if( pData->FreezeIndex == eFREEZE_DAY )
			{
				dwTimeInterval = 1440L * pData->pDLT698RecordPara->TimeIntervalPara.TI;
			}
			
			if( (pData->pDLT698RecordPara->TimeIntervalPara.StartTime % dwTimeInterval) != 0 )
			{
				pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime / dwTimeInterval;
				pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime * dwTimeInterval;
				pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime + dwTimeInterval;
			}
		}				
	}
	
	//查找起始时间对应的RecordNo
	dwStartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime;
	pData->pDLT698RecordPara->TimeOrLast = pData->pDLT698RecordPara->TimeIntervalPara.StartTime;	
	Result = SearchFreezeRecordNoByTime(pData);
	if( Result == FALSE )
	{
		return 0x8000;
	}
	dwStartRecordNo = pData->RecordNo;	
    
	//查找截止时间对应的RecordNo
	pData->pDLT698RecordPara->TimeOrLast = pData->pDLT698RecordPara->TimeIntervalPara.EndTime;	
	Result = SearchFreezeRecordNoByTime(pData);
	if(Result == TRUE)
	{
    	if( pData->RecordNo == dwStartRecordNo)//如果起始和结束时间搜索到同一个点 返回失败
    	{
            return 0x8000;
    	}
	}
	
	if( Result == FALSE )
	{
		//若查找不到（一般是大于当前时间），采用上一条记录号
        if( pData->pDataInfo->RecordNo == 0 )
        {
            dwEndRecordNo = 0;
        }
        else
        {
            dwEndRecordNo = pData->pDataInfo->RecordNo-1;
        }
	}
	else
	{
		//搜索时间要求前闭后开
		if( pData->SearchTime < pData->pDLT698RecordPara->TimeIntervalPara.EndTime )
		{
			dwEndRecordNo = pData->RecordNo;
		}
		else
		{
			if( pData->RecordNo == 0 )
			{
				return 0x8000;
			}
			else
			{
				dwEndRecordNo = pData->RecordNo-1;
			}
		}
	}
	
	ReturnLen = 0;
	pData->pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum = 0;
	
	//非周期性不判断间隔时间   当下发的TI为0时有几条记录返回几条记录
	if( (pData->FreezeIndex<eFREEZE_HOUR) || (pData->pDLT698RecordPara->TimeIntervalPara.AllCycle==0x55) )
	{
		for(i=dwStartRecordNo; i<=dwEndRecordNo; i++)
		{
			pData->RecordNo = i;		
			wLen = ReadFreezeByRecordNo(pData);		
			if( wLen == 0x8000 )
			{
				return 0x8000;
			}
			else if( wLen == 0 )
			{
				//第一条数据长度就不够，返回8000
				if( i == dwStartRecordNo )
				{
					return 0;
				}
				else
				{
					break;
				}
			}
			else
			{
				pData->pBuf += wLen;
				ReturnLen += wLen;
				pData->Len -= wLen;
				pData->pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum++;
			}
		}
	}
	else
	{
		//当不按照冻结的周期作为间隔抄读数据时，先按照时间找出recordno，且看找出的时间是否是要抄读的时间，若是，则返回数据，若不是，返回null
		if( pData->FreezeIndex == eFREEZE_MIN )
		{
			//获取此条数据的地址
			if(GetSectorInfo( pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex], pData->pAttCycleDepth->Depth, dwEndRecordNo, &SectorInfo ) == FALSE )
			{
				return 0x8000;
			}
			
			dwAddr = pData->pMinInfo->DataAddr[pData->MinPlanIndex]+SectorInfo.dwAddr;
			FreezeRecordInfo.RecordLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//整条长度
			FreezeRecordInfo.dwBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];
			FreezeRecordInfo.MinPlan = pData->MinPlanIndex;
		}
		else
		{
			GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, dwEndRecordNo, &SectorInfo );
			dwAddr = pData->pFreezeAddrLen->dwDataAddr+SectorInfo.dwAddr;
			FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//整条长度
		}
		FreezeRecordInfo.dwAddr = dwAddr;//flash存储地址
		FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//冻结类型
		FreezeRecordInfo.RecordNo = dwEndRecordNo;//冻结记录序号
		//读取此记录号对应时间
		if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwEndTime )==FALSE)
		{
			//错误情况下不处理
		}
		
		LoopNum = 0;
		i = dwStartTime;
		while(i<=dwEndTime)
		{
			CLEAR_WATCH_DOG;
			//防止循环出不来
			LoopNum++;
			if(LoopNum > 500)
			{
				break;
			}
			pData->pDLT698RecordPara->TimeOrLast = i;
			SearchFreezeRecordNoByTime(pData);
			if( pData->SearchTime > dwEndTime )//搜索到的时间比结束时间大
			{
				break;
			}
			if( pData->FreezeIndex == eFREEZE_MON )//月冻结
			{
				api_ToAbsTime( pData->SearchTime, &Time );
				Mon = (Time.wYear-2000)*12+Time.Mon-1;
				if( (pData->SearchTime != i)//搜索到指定时间 
				&& ((Mon % pData->pDLT698RecordPara->TimeIntervalPara.TI) != 0) )//搜索到的时间可被时间间隔整除              
				{
					Mon = (((Mon/pData->pDLT698RecordPara->TimeIntervalPara.TI)+1)*pData->pDLT698RecordPara->TimeIntervalPara.TI);
					Time.wYear = (2000+(Mon/12));//重新计算年
					Time.Mon = (Mon % 12)+1;
					i  = api_CalcInTimeRelativeMin( &Time );
					continue;
				}
                else
                {
                    i = pData->SearchTime;
                }
			}
			else
			{
			//如过没有找到对应的时间，将找到的时间赋给当前查找时间以跳过空余段
				if(( pData->SearchTime != i ) && ((pData->SearchTime % dwTimeInterval) != 0) )//搜索到的时间可被时间间隔整除
				{
					i = ((pData->SearchTime/dwTimeInterval)+1)*dwTimeInterval;
					continue;
				}
				else
				{
					i = pData->SearchTime;
				}

			}
			wLen = ReadFreezeByRecordNo(pData);
			if( wLen == 0x8000 )
			{
				return 0x8000;
			}
			else if( wLen == 0 )
			{
				//第一条数据长度就不够，返回8000
				if( pData->RecordNo == dwStartRecordNo )
				{
					return 0;
				}
				else
				{
					break;
				}
			}
			else
			{
				if( pData->Len < wLen )//在pData->Len -= wLen之前进行判断，解决时间段读取时会出现pData->Len转圈导致buf超限的问题
				{
					break;
				}
				pData->pBuf += wLen;
				ReturnLen += wLen;
				pData->Len -= wLen;
				pData->pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum++;
				
			}
			if( pData->FreezeIndex != eFREEZE_MON )
			{
				i += dwTimeInterval;
			}
			else
			{
				api_ToAbsTime(i, &Time);
				Mon = (Time.wYear-2000)*12+(Time.Mon-1);
				
				Mon += pData->pDLT698RecordPara->TimeIntervalPara.TI;
				Time.wYear = Mon/12+2000;
				Time.Mon = Mon%12 + 1;
				i = api_CalcInTimeRelativeMin(&Time);
			}
		}		
	}
	
	if( ReturnLen != 0 )
	{
		return ReturnLen;
	}
	else
	{
		//没有找到，返回8000
		return 0x8000;
	}
}

//-----------------------------------------------
//函数功能: 读冻结记录
//
//参数: 
//	Tag[in]:	0 不加Tag 				1 加Tag                
//	pDLT698RecordPara[in]: 读取冻结的参数结构体                
//  Len[in]：输入的buf长度 用来判断buf长度是否够用                
//  pBuf[out]: 返回数据的指针，有的传数据，没有的补0
//				
//	
//返回值:	bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//			bit0~bit14 代表返回的冻结数据长度
//
//备注:	
//-----------------------------------------------
WORD api_ReadFreezeRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	BYTE i,tFreezeIndex,PlanIndex;
	DWORD dwOad,dwTempOad;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];
	TFreezeAttCycleDepth MinAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth AttCycleDepth;
	TFreezeDataInfo	DataInfo;
	TFreezeData 	Data;
	TFreezeAddrLen FreezeAddrLen;
    TFreezeAllInfoRom FreezeAllInfo;
    TFreezeMinInfo FreezeMinInfo;
    
	PlanIndex = 0;
	memset( &FreezeMinInfo, 0x00, sizeof(FreezeMinInfo) );
	
	tFreezeIndex = api_GetFreezeIndex( pDLT698RecordPara->OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return 0x8000;
	}

	// 获得各种地址
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	//读出单条冻结的长度及关联属性的个数
	if( api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo) != 	TRUE )
	{
		return 0x8000;
	}	

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return 0x8000;
	}
	
	//读出所有的关联属性对象
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);		
	
	if( tFreezeIndex == eFREEZE_MIN )
	{
		//读出所有的关联属性对象 周期深度
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&MinAttCycleDepth[i] );
		}
		
		//分钟冻结下发的RCSD不能为0
		if( pDLT698RecordPara->OADNum == 0 )
		{
			return 0x9000;
		}
		//判断下发的OAD是否为同一方案，需要判断元素索引为0，要读的不为0的情况
		PlanIndex = 0xff;//下发OAD的方案号
		for(i=0; i<pDLT698RecordPara->OADNum; i++)
		{
			memcpy( (BYTE*)&dwOad, pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
			dwTempOad = dwOad & (~0x00E00000);	
			if((dwTempOad == FREEZE_RECORD_NO_OAD) || (dwTempOad == FREEZE_RECORD_TIME_OAD))
			{
				if(CURR_PLAN(dwOad) == 0)
				{
					continue;
				}
			}
			if( PlanIndex== 0xff )
			{
				PlanIndex = CURR_PLAN(dwOad);
			}
			else
			{
				if( PlanIndex != CURR_PLAN(dwOad) )
				{
					return 0x9000;
				}	
			}
		}
		
		//若要读取的只有RecordNo和Time，默认按方案0抄读
		if(PlanIndex == 0xff)
		{
			PlanIndex= 0;
		}
		
		//读取分钟冻结地址信息
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo ); 
		if( FreezeMinInfo.DataAddr[0] != FreezeAddrLen.dwDataAddr )//方案0地址 与基地址一致
		{
			FreezeMinInfo.DataAddr[0] = FreezeAddrLen.dwDataAddr;
		}

		//查找分钟冻结下发的对象对应的RecordNo		
		memcpy( (BYTE *)&DataInfo, (BYTE *)&MinInfo[PlanIndex], sizeof(TFreezeDataInfo) );
		//查找分钟冻结下发的对象对应的深度周期		
		memcpy( (BYTE *)&AttCycleDepth, (BYTE *)&MinAttCycleDepth[PlanIndex], sizeof(AttCycleDepth) );
	}
	else
	{
		//读出所有的关联属性对象 周期深度
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth );
		//读出此条的RecordNo
		if( api_VReadSafeMem(FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo) != TRUE )
		{
			return 0x8000;
		}
	}
	
	//还没有产生冻结
	if( DataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}

	Data.MinPlanIndex = PlanIndex;	//只对分钟冻结有用
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pAttOad = &AttOad[0];
	Data.pAttCycleDepth = &AttCycleDepth;
	Data.pDataInfo = &DataInfo;
	Data.pMinInfo = &FreezeMinInfo;
	Data.Tag = Tag;
	Data.pBuf = pBuf;
	Data.pFreezeAddrLen = &FreezeAddrLen;
	Data.pAllInfo = &FreezeAllInfo;
	Data.Len = Len;
	Data.pTime = NULL;
	Data.FreezeIndex = tFreezeIndex;

	if( pDLT698RecordPara->eTimeOrLastFlag == eFREEZE_TIME_FLAG )
	{
		//RSD 方法1：按照时间索引
		return ReadFreezeByTime( &Data );
	}
	else if( pDLT698RecordPara->eTimeOrLastFlag == eNUM_FLAG )
	{
		//RSD 方法9：按照次数索引
		return ReadFreezeByLastNum( &Data );
	}
	else if( (pDLT698RecordPara->eTimeOrLastFlag == eRECORD_TIME_INTERVAL_FLAG) 
	||	( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eFREEZE_TIME_FLAG) ))
	{
		//RSD 方法2：按照时间区间索引
		return ReadFreezeByTimeInterval( &Data );			
	}
	else
	{
		return 0x8000;
	}
}
//-----------------------------------------------
//函数功能: 冻结掉电处理
//
//参数: 
//	无
//	
//  返回值:	
//  无
//
//备注:	 保存192字节
//-----------------------------------------------
void api_PowerDownFreeze( void )
{
	BYTE i;
	BYTE Buf[sizeof(MinInfo)+sizeof(DWORD)];
	
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		// 检查RAM的校验
		if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == FALSE )
		{
			return;
		}
	}
	memcpy(Buf,&MinInfo[0],sizeof(MinInfo));
	// 写EEP
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[0] ), sizeof(MinInfo)+sizeof(DWORD), Buf );
	
	memset( (BYTE*)&CompleteFreezeTime, 0x00, sizeof(TCompleteFreezeTime) );
	CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
}
//-----------------------------------------------
//函数功能: 更新掉电时间备份
//
//参数: 
//	无
//	
//  返回值:	
//  无
//
//备注:	 
//-----------------------------------------------
void api_ReflashFreeze_Ram( void )
{
	TRealTimer		PowerDownTime;
	
	//读取掉电时间
	api_GetPowerDownTime( (TRealTimer*)&PowerDownTime );
	memcpy( (BYTE*)&FreezePowerDownTimeBak, (BYTE*)&PowerDownTime, sizeof(TRealTimer) );//更新备份电表时间
	//将上一次补冻结时间清零
	memset( (BYTE*)&CompleteFreezeTime, 0x00, sizeof(TCompleteFreezeTime) );
	CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
}

//-----------------------------------------------
//函数功能: 上电补日冻结
//
//参数: 	PowerDownTime[in] 掉电时间
//	
//返回值:	无
//
//备注:	 
//-----------------------------------------------
static BYTE PowerUpCompleteFreezeDay( TRealTimer PowerDownTime )
{
	BYTE i,m;
	BYTE TmpOADBuf[10];
	WORD wDownDay,wOnDay,Len;
	DWORD dwClosingTime,dwStartFreezeTime,dwLastFreezeTime,dwPowerDownTime;
	DWORD FreezeTime[7];
	TRealTimer tStartFreezeTime,DayClosingTime,LastFreezeTime;
	TBillingPara	BillingPara;
	TDLT698RecordPara TmpDLT698RecordPara;
	
	CLEAR_WATCH_DOG;
	
	//读取上一次日冻结时间
	memset( (BYTE *)&LastFreezeTime, 0xff, sizeof(TRealTimer) ); 
	TmpOADBuf[0] = 0x20;
	TmpOADBuf[1] = 0x21;
	TmpOADBuf[2] = 0x02;
	TmpOADBuf[3] = 0x00;
	TmpDLT698RecordPara.pOAD = TmpOADBuf;//pOAD指针
	TmpDLT698RecordPara.OADNum = 1;		//OAD个数
	TmpDLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;//选择次数
	TmpDLT698RecordPara.TimeOrLast = 0x01;	//上一次
	TmpDLT698RecordPara.Phase = ePHASE_ALL;
	TmpDLT698RecordPara.OI = 0x5004;					
	Len = api_ReadProRecordData( 0, &TmpDLT698RecordPara, sizeof(TRealTimer), (BYTE*)&LastFreezeTime );

	//如果能够读回的上一次日冻结时间，用上一次日冻结时间
	//没有能够不读回的上一次日冻结时间或者读会时间非法，用掉电时间
	if( (Len == 1) || (api_CheckClock(&LastFreezeTime) == FALSE) )
	{
		if( api_CheckClock(&PowerDownTime) == FALSE )
		{
			return TRUE;
		}
		else
		{
			memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));	
		}
	}
	else
	{
		dwLastFreezeTime = api_CalcInTimeRelativeMin(&LastFreezeTime);
		dwPowerDownTime = api_CalcInTimeRelativeMin(&PowerDownTime);
		if( (dwLastFreezeTime > dwPowerDownTime ) //上一次日冻结时间大于掉电时间，用上一次日冻结时间
		|| ( dwPowerDownTime == 0xFFFFFFFF ))//掉电时间时间非法，用上一次日冻结时间
		{
			memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&LastFreezeTime, sizeof(TRealTimer));
		}
		else
		{
			memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));	
		}
		
	}
	if( api_CheckClock(&PowerDownTime) == FALSE )
	{
		memcpy( (void*)&DayClosingTime, (void*)&tStartFreezeTime, sizeof(TRealTimer) );//获取掉电时间
	}
	else
	{
		memcpy( (void*)&DayClosingTime, (void*)&PowerDownTime, sizeof(TRealTimer) );//获取掉电时间
	}

	//开始补冻结时间相对分钟数
	dwStartFreezeTime = api_CalcInTimeRelativeMin(&tStartFreezeTime);
	
	#if( SEL_STAT_V_RUN == YES )
	if( tStartFreezeTime.Mon != RealTimer.Mon )//清零月电压合格率
	{
		api_ClearVRunTimeNow( BIT1 );
	}
	#endif

	//获取结算日失败用1日0时
	if( GetClosingTime( &BillingPara ) == FALSE )
	{
		BillingPara.MonSavePara[0].Day = 1;
		BillingPara.MonSavePara[0].Hour = 0;
	}
	DayClosingTime.Day = BillingPara.MonSavePara[0].Day;
	DayClosingTime.Hour =BillingPara.MonSavePara[0].Hour;
	DayClosingTime.Min = 0;
	dwClosingTime = api_CalcInTimeRelativeMin(&DayClosingTime);//计算当前月结算日相对分钟数

	
	if( dwStartFreezeTime >= dwClosingTime )//开始补冻结时间过当前结算日
	{
		if( DayClosingTime.Mon == 12 )//月进位
		{
			DayClosingTime.wYear += 1;
			DayClosingTime.Mon = 1;
		}
		else
		{
			DayClosingTime.Mon += 1;
		}
		dwClosingTime = api_CalcInTimeRelativeMin(&DayClosingTime);//计算下一月结算日相对分钟数

	}
	
	//开始补冻结相对日
	wDownDay = dwStartFreezeTime/1440;
	//上电相对日
	wOnDay = g_RelativeMin/1440;
	
    #if( SEL_STAT_V_RUN == YES )
	if( (wOnDay-wDownDay) > 7 )//超过7天清零日电压合格率
	{
		api_ClearVRunTimeNow( BIT0 );
	}
    #endif
	//和正常冻结一样，日冻结不判冻结周期，每次都冻
	memset(&FreezeTime[0], 0x00, sizeof(FreezeTime));
		
	//查找此属性需要补日冻结的次数（相对分钟数），由小到大排序，最多7天
	for(i = 0;i < 7;i++)
	{
		if( wOnDay <= wDownDay )
		{
			break;
		}
		FreezeTime[6-i] = (DWORD)wOnDay*1440;
		
		wOnDay--;
	}
	m=0;
	//开始补日冻结
	for(i=0; i<7; i++)
	{
		if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
		{	
			ClrFreezeFlag(eFREEZE_MAX_NUM);
			return FALSE;
		}
		
		if(FreezeTime[i] == 0)
		{
			continue;
		}	
		CLEAR_WATCH_DOG;
		if( FreezeTime[i] > dwClosingTime )//过结算日
		{
			ClrFreezeFlag(eFREEZE_MAX_NUM);
		}
		else
		{
			SetFreezeFlag(eFREEZE_MAX_NUM);
		}
		PowerUpFreezeMin = FreezeTime[i];
		ProcFreezeTask(eFREEZE_DAY,0x55);
		if( m == 0 )//补存过日冻结后、日电压合格率清零
		{  
            #if( SEL_STAT_V_RUN == YES )
			api_ClearVRunTimeNow( BIT0 );
            #endif
			m=0x55;
		}
		
	}
	return TRUE;
}

//-----------------------------------------------
//函数功能: 上电结算日冻结
//
//参数: 	PowerDownTime[in] 掉电时间
//	
//返回值:	无
//
//备注:	 
//-----------------------------------------------
static BYTE PowerUpCompleteFreezeClosing( TRealTimer PowerDownTime )
{
	BYTE i,j,k,Num,Flag;
	BYTE ClosingFlag[12],ClosingSn[3];
	WORD wDownMonNum,wUpMonNum;
	DWORD dwDownTime,dwFreezeTime, dwClosingTime, dwStartFreezeTime, dwLastFreezeTime ;
	DWORD FreezeTime[12];
	TRealTimer		tStartFreezeTime, DayClosingTime;
	TBillingPara	BillingPara;
	
	CLEAR_WATCH_DOG;
	
	//掉电时间非法，不进行补冻，退出
	if( api_CheckClock(&PowerDownTime) == FALSE )
	{
		return TRUE;
	}
	if( api_CheckClock(&CompleteFreezeTime.CompleteFreezeClosingTime) == FALSE )
	{
		dwLastFreezeTime = 0;
	}
	else
	{
		dwLastFreezeTime = api_CalcInTimeRelativeMin(&CompleteFreezeTime.CompleteFreezeClosingTime);
	}
	//计算掉电时间相对分钟数
	dwDownTime = api_CalcInTimeRelativeMin(&PowerDownTime);
	//上一次结算日冻结时间小于掉电时间，从掉电时间开始补冻
	//上一次结算日冻结时间在掉电时间与当前时间之间，从上一次结算日冻结时间开始补冻
	//上一次结算日冻结时间大于当前时间，从掉电时间开始补冻
	if(dwLastFreezeTime < dwDownTime )
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));
	}
	else if(dwLastFreezeTime <= g_RelativeMin )	
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&CompleteFreezeTime.CompleteFreezeClosingTime, sizeof(TRealTimer));
	}
	else
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));
	}

	memcpy( (void*)&DayClosingTime, (void*)&PowerDownTime, sizeof(TRealTimer) );//获取掉电时间
	
	//开始补冻结相对分钟数
	dwStartFreezeTime = api_CalcInTimeRelativeMin(&tStartFreezeTime);
	//掉电时间相对月数
	wDownMonNum = (tStartFreezeTime.wYear%2000)*12+tStartFreezeTime.Mon-1;
	//上电时间相对月数
	wUpMonNum = (RealTimer.wYear%2000)*12+RealTimer.Mon-1;
	
	//补最近12个结算日 获取结算日
	if( GetClosingTime( &BillingPara ) == FALSE )
	{
		return TRUE;
	}

	DayClosingTime.Day = BillingPara.MonSavePara[0].Day;
	DayClosingTime.Hour =BillingPara.MonSavePara[0].Hour;
	DayClosingTime.Min = 0;
	dwClosingTime = api_CalcInTimeRelativeMin(&DayClosingTime);//计算当前月结算日相对分钟数

	
	if( dwStartFreezeTime >= dwClosingTime )//开始补冻时间过当前结算日
	{
		
		if( DayClosingTime.Mon == 12 )//月进位
		{
			DayClosingTime.wYear += 1;
			DayClosingTime.Mon = 1;
		}
		else
		{
			DayClosingTime.Mon += 1;
		}
		dwClosingTime = api_CalcInTimeRelativeMin(&DayClosingTime);//计算下一月结算日相对分钟数
	}
	// 按照日，时升序冒泡排序(0xFF排到最后)
	ClosingSn[0] = 0;
	ClosingSn[1] = 1;
	ClosingSn[2] = 2;
	for( i = (MAX_MON_CLOSING_NUM-1); i > 0; i-- )
	{
		Flag = 0;
		for( k = 0; k < i; k++ )
		{
			if( (BillingPara.MonSavePara[k].Day > BillingPara.MonSavePara[k+1].Day)||
				( (BillingPara.MonSavePara[k].Day == BillingPara.MonSavePara[k+1].Day)&&(BillingPara.MonSavePara[k].Hour > BillingPara.MonSavePara[k+1].Hour)))
			{
				// 交换
				j = BillingPara.MonSavePara[k].Day;
				BillingPara.MonSavePara[k].Day = BillingPara.MonSavePara[k+1].Day;
				BillingPara.MonSavePara[k+1].Day = j;

				j = BillingPara.MonSavePara[k].Hour;
				BillingPara.MonSavePara[k].Hour = BillingPara.MonSavePara[k+1].Hour;
				BillingPara.MonSavePara[k+1].Hour = j;
				
				j = ClosingSn[k];
				ClosingSn[k] = ClosingSn[k+1];
				ClosingSn[k + 1] = j;

				Flag = 1;
			}
		}
		if( Flag == 0 )
		{
			break;			// 无交换
		}
	}

	//计算一月几次结算
	Num = 0;
	for( i = 0; i < 3; i++)
	{
		if( (BillingPara.MonSavePara[i].Day == 0xFF) || (BillingPara.MonSavePara[i].Hour == 0xFF) )
		{
		
		}
		else if( (BillingPara.MonSavePara[i].Day == 99) || (BillingPara.MonSavePara[i].Hour == 99) )
		{
		
		}
		else//结算日相同的予以剔除
		{
			if( i == 0 )
			{
				Num++;
			}
			else if( i == 1 )
			{
				if( (BillingPara.MonSavePara[1].Day == BillingPara.MonSavePara[0].Day) 
				&& ((BillingPara.MonSavePara[1].Hour == BillingPara.MonSavePara[0].Hour)))
				{

				}
				else
				{
					Num++;
				}
			}
			else if( i == 2 )
			{
				if( (BillingPara.MonSavePara[2].Day == BillingPara.MonSavePara[1].Day) 
				&& ((BillingPara.MonSavePara[2].Hour == BillingPara.MonSavePara[1].Hour)))
				{
				
				}
				else if( (BillingPara.MonSavePara[2].Day == BillingPara.MonSavePara[1].Day) 
				&& ((BillingPara.MonSavePara[2].Hour == BillingPara.MonSavePara[1].Hour)))
				{
				
				}
				else
				{
				Num++;
				}

			}
		}
	}

	memset( &FreezeTime[0], 0x00, sizeof(FreezeTime) );
	
	//当掉电时间超过12个结算日时，修改掉电时间为最近13个结算日的月。
	if( (wUpMonNum-wDownMonNum)*Num > 12 )
	{
		wDownMonNum = wUpMonNum-(12/Num+1);
		tStartFreezeTime.wYear = wDownMonNum/12+2000;
		tStartFreezeTime.Mon = wDownMonNum%12+1;
	}
	
	memset(ClosingFlag,0xff,12);
	
	// 当一月一结时，搜索最近13个月；当一月两结时，搜索最近7个月；当一月三结时，搜索最近5个月
	for( k = 0; k < (12/Num+2); k++ )
	{
		// 一个月num个结算日
		for( i = 0; i < Num; i++ )
		{
			// 月，年保持不变
			tStartFreezeTime.Sec = 0;
			tStartFreezeTime.Min = 0;
			tStartFreezeTime.Hour = BillingPara.MonSavePara[i].Hour;
			tStartFreezeTime.Day = BillingPara.MonSavePara[i].Day;

			dwFreezeTime = api_CalcInTimeRelativeMin( &tStartFreezeTime );

			if( (dwFreezeTime>dwStartFreezeTime) && (dwFreezeTime<=g_RelativeMin) )
			{
				// 保存12个月的结算时间
				memmove( &FreezeTime[0], &FreezeTime[1], 11*sizeof(DWORD) );
				memcpy( &FreezeTime[11], &dwFreezeTime, sizeof(DWORD) );
				memcpy( ClosingFlag, &ClosingFlag[1], 11 );
				ClosingFlag[11] = ClosingSn[i];
			}
			else if( dwFreezeTime > g_RelativeMin )
			{
				k = 12/Num+1;
				break;
			}
		}
		
		//找下一月数据
		wDownMonNum += 1;
		tStartFreezeTime.wYear = wDownMonNum/12+2000;
		tStartFreezeTime.Mon = wDownMonNum%12+1;
	}
	Flag = 0;
	if (FreezeTime[0] > dwClosingTime  )	
	{						
		#if( MAX_PHASE == 3 )
		#if( SEL_DEMAND_2022 == NO )
		api_ClrDemand(eClearAllDemand);
		#else
		api_ClrDemand(eClearAllDemand,eDemandRun);
		#endif
		#endif
		// 月结算电能，用于月累计电能计算	
		api_ClosingMonEnergy();	
		Flag = 0x55;
	}
	for( i = 0; i < 12; i++ )
	{
		if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
		{
			ClrFreezeFlag(eFREEZE_MAX_NUM);
			return FALSE;
		}
		if( FreezeTime[i] == 0x00 )
		{			
			continue;
		}	
		CLEAR_WATCH_DOG;		
		//填充时间
		PowerUpFreezeMin=FreezeTime[i];
		
		//组数据
		if( (ClosingFlag[i]==1) || (ClosingFlag[i]==2) )
		{						
			ClrFreezeFlag(eFREEZE_MAX_NUM);
		}
		else
		{
			SetFreezeFlag(eFREEZE_MAX_NUM);
		}
		ProcFreezeTask(eFREEZE_CLOSING,0x55);
		//记录此处冻结时间
		api_ToAbsTime(PowerUpFreezeMin, &CompleteFreezeTime.CompleteFreezeClosingTime);
		CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
		
		if ((FreezeTime[i] >= dwClosingTime ) && (Flag == 0) )	
		{						
			#if( MAX_PHASE == 3 )
			#if( SEL_DEMAND_2022 == NO )
			api_ClrDemand(eClearAllDemand);
			#else
			api_ClrDemand(eClearAllDemand,eDemandRun);
			#endif
			#endif
			// 月结算电能，用于月累计电能计算	
			api_ClosingMonEnergy();	
			Flag = 0x55;
		}
	}
	ClrFreezeFlag(eFREEZE_MAX_NUM);
	return TRUE;
}	

//-----------------------------------------------
//函数功能: 上电补阶梯结算
//
//参数: 	PowerDownTime[in] 掉电时间
//	
//返回值:	无
//
//备注:	 
//-----------------------------------------------
#if( PREPAY_MODE == PREPAY_LOCAL )
static BYTE PowerUpCompleteFreezeLadderClosing( TRealTimer PowerDownTime  )
{
	
	BYTE i,j,m,Num,Flag;
	WORD wDownMonNum,wUpMonNum;
	DWORD dwDownTime,dwFreezeTime,dwStartFreezeTime, dwLastFreezeTime;
	DWORD FreezeTime[12];
	TRealTimer		tPowerDownTime ,tStartFreezeTime;	
	DWORD dwFreezeTime1;//排序倒时间用
	TLadderBillingPara LadderBillingPara;
	BYTE LadderMode; //阶梯结算模式，0x00无阶梯结算，0x55月结算，0xAA年结算。
	
	//掉电时间非法，不进行补冻，退出
	if( api_CheckClock(&PowerDownTime) == FALSE )
	{
		return TRUE;
	}
	CLEAR_WATCH_DOG;
	
	if( api_CheckClock(&CompleteFreezeTime.CompleteFreezeLadderClosingTime) == FALSE )
	{
		dwLastFreezeTime = 0;
	}
	else
	{
		dwLastFreezeTime = api_CalcInTimeRelativeMin(&CompleteFreezeTime.CompleteFreezeLadderClosingTime);
	}
	//计算掉电时间相对分钟数
	dwDownTime = api_CalcInTimeRelativeMin(&PowerDownTime);
	//上一次结算日冻结时间小于掉电时间，从掉电时间开始补冻
	//上一次结算日冻结时间在掉电时间与当前时间之间，从上一次结算日冻结时间开始补冻
	//上一次结算日冻结时间大于当前时间，从掉电时间开始补冻
	if(dwLastFreezeTime < dwDownTime )
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));
	}
	else if(dwLastFreezeTime < g_RelativeMin )	
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&CompleteFreezeTime.CompleteFreezeLadderClosingTime, sizeof(TRealTimer));
	}
	else
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));
	}
	
	//开始补冻结相对分钟数
	dwStartFreezeTime = api_CalcInTimeRelativeMin(&tStartFreezeTime);
	
	//获取掉电前阶梯结算日表
	LadderMode = api_GetPowerDownYearBill(&LadderBillingPara);
	//只有月阶梯和年阶梯情况下补阶梯冻结
	if((LadderMode == 0x55) ||(LadderMode == 0xAA))
	{
		//清零补冻时间列表
		memset( (BYTE*)&FreezeTime[0], 0x00, sizeof(FreezeTime) );
		//月阶梯，每月一结
		if(LadderMode == 0x55)
		{
			//重新计算掉电时间相对月数
			wDownMonNum = (tStartFreezeTime.wYear%2000)*12+tStartFreezeTime.Mon-1;
			//上电时间相对月数
			wUpMonNum = (RealTimer.wYear%2000)*12+RealTimer.Mon-1;
			//当掉电时间超过4个月阶梯结算日。
			if( (wUpMonNum - wDownMonNum)> 5 )
			{
				wDownMonNum = wUpMonNum-5;
			}
			//需要搜取6次，以保证搜到了最近一个点
			for( i = 0; i < 6; i++ )
			{
				//年和月利用相对月数计算得出
				tPowerDownTime.wYear = wDownMonNum/12+2000;
				tPowerDownTime.Mon = wDownMonNum%12+1;
				tPowerDownTime.Sec = 0;
				tPowerDownTime.Min = 0;
				tPowerDownTime.Hour = LadderBillingPara.LadderSavePara[0].Hour;
				tPowerDownTime.Day = LadderBillingPara.LadderSavePara[0].Day;
			
				dwFreezeTime = api_CalcInTimeRelativeMin( &tPowerDownTime );

				if( (dwFreezeTime>dwDownTime) && (dwFreezeTime<=g_RelativeMin) )
				{
					// 保存4个月的阶梯结算时间
					memmove( &FreezeTime[0], &FreezeTime[1], 4*sizeof(DWORD) );
					memcpy( &FreezeTime[4], &dwFreezeTime, sizeof(DWORD) );
				}
				else if( dwFreezeTime > g_RelativeMin )
				{
					break;
				}
				//找下一月数据
				wDownMonNum += 1;
			}	
		}
		else if(LadderMode == 0xAA)//年阶梯
		{
			
			//计算一年几次阶梯结算
			Num = 0;
			for( i = 0; i < MAX_YEAR_BILL; i++)
			{
				if ((LadderBillingPara.LadderSavePara[i].Mon == 0xFF) 
				|| (LadderBillingPara.LadderSavePara[i].Day == 0XFF) 
				|| (LadderBillingPara.LadderSavePara[i].Hour == 0XFF))  
				{
					break;
				}
	
			}
			Num = i;
			memset( &FreezeTime[0], 0x00, sizeof(FreezeTime) );
			//当掉电时间超过5年时，修改掉电时间为最近5年。
            tPowerDownTime.wYear = tStartFreezeTime.wYear;
			if( (RealTimer.wYear - tStartFreezeTime.wYear) > 5 )
			{
				tPowerDownTime.wYear = RealTimer.wYear-5;
			}
			
			//按最大年份算，最多搜索6年即可
			for( j = 0; j< 6; j++ )
			{
				// 一年num个阶梯结算日
				for( i = 0; i < Num; i++ )
				{
					// 月，年保持不变
					tPowerDownTime.Sec = 0;
					tPowerDownTime.Min = 0;
					tPowerDownTime.Hour = LadderBillingPara.LadderSavePara[i].Hour;
					tPowerDownTime.Day = LadderBillingPara.LadderSavePara[i].Day;
					tPowerDownTime.Mon = LadderBillingPara.LadderSavePara[i].Mon;

					dwFreezeTime = api_CalcInTimeRelativeMin( &tPowerDownTime );

					if( (dwFreezeTime>dwDownTime) && (dwFreezeTime<=g_RelativeMin) )
					{
						for(m=5;m>0;m--)//最多存5个在范围内的最大相对分钟数
						{
							//如果此时间大于存储时间，将此时间放到此位置
							if(dwFreezeTime > FreezeTime[m-1])
							{
								dwFreezeTime1 = FreezeTime[m-1];
								FreezeTime[m-1] = dwFreezeTime;
								dwFreezeTime = dwFreezeTime1;
							}
							else if(dwFreezeTime == FreezeTime[m-1])
							{
								break;
							}
						}	
					}
				}	
				//找下一年数据
				tPowerDownTime.wYear += 1;
			}	
		}
		//跨过的阶梯结算日是否大于4,大于4清当前阶梯结算用电量
		Flag = 0x00;
		if(FreezeTime[0] != 0)//判断是否跨5个阶梯结算
		{
			api_ClrCurLadderUseEnergy(); //清除当前阶梯结算用电量
			Flag = 0x55;
		}
		for( i = 1; i < 5; i++ )
		{
			if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
			{
				return FALSE;
			}
			if( FreezeTime[i] == 0x00 )
			{			
				continue;
			}	
			CLEAR_WATCH_DOG;		
			//填充时间
			PowerUpFreezeMin=FreezeTime[i];
			
			ProcFreezeTask(eFREEZE_LADDER_CLOSING,0x55);
			//记录此处冻结时间
			//记录此处冻结时间
			api_ToAbsTime(PowerUpFreezeMin, &CompleteFreezeTime.CompleteFreezeLadderClosingTime);
			CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
			if (Flag == 0x00) 	
			{						
				api_ClrCurLadderUseEnergy(); //清除当前阶梯结算用电量
				Flag = 0xaa;
			}		
		}
	}
	return TRUE;
}
#endif//( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//函数功能: 上电补冻结
//
//参数: 	无
//	
//返回值:	无
//
//备注:	 
//-----------------------------------------------
static void PowerUpCompleteFreeze( void )
{
	TRealTimer		PowerDownTime;
	DWORD dwDownTime;
	
	if( api_CheckClock(&RealTimer) == FALSE )
	{
		return;	
	}
	//读取掉电时间
	api_GetPowerDownTime( (TRealTimer*)&PowerDownTime );
	
	//掉电时间相对分钟数
	dwDownTime = api_CalcInTimeRelativeMin(&PowerDownTime);
	//判断掉电时间的相对分钟数,如果大于当前时间，不进行补冻
	if( api_CheckClock(&PowerDownTime) == TRUE )
	{
		if( ( dwDownTime >= g_RelativeMin ) || (g_RelativeMin > (dwDownTime+5256000)) )
		{
			return;
		}
	}
	
	//时间相等不进行非周期冻结补冻，避免异常复位补冻
	if( memcmp((BYTE*)&FreezePowerDownTimeBak, (BYTE*)&PowerDownTime, sizeof(TRealTimer)) == 0 )
	{
		return;
	}
	
	//补冻后的补冻结时间CRC不对不用
	if( CompleteFreezeTime.CRC32 != lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD)))
	{
		memset( (BYTE*)&CompleteFreezeTime, 0x00, sizeof(TCompleteFreezeTime) );
		CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
	}
	//日冻结补冻结
	if( PowerUpCompleteFreezeDay(PowerDownTime) == FALSE )
	{
		return;	
	}
	
	//结算日冻结补冻结
	if( PowerUpCompleteFreezeClosing(PowerDownTime) == FALSE )
	{
		return;	
	}
	
	//阶梯结算补冻结
	#if( PREPAY_MODE == PREPAY_LOCAL )
	if( PowerUpCompleteFreezeLadderClosing( PowerDownTime ) == FALSE )
	{
		return;	
	}
	#endif
}

//-----------------------------------------------
//函数功能: 冻结上电处理
//
//参数: 	无
//	
//返回值:	无
//
//备注:	 
//-----------------------------------------------
void api_PowerUpFreeze( void )
{
	if( wInstantFreezeTimerBak != (WORD)(~wInstantFreezeTimer) )//瞬时冻结标志校验
	{
		wInstantFreezeTimer = 0;
		wInstantFreezeTimerBak = 0xffff;
	}
	//检查分钟冻结指针信息
	if( CheckMinFreezeInfo() == FALSE )
	{
		#if (SLE_THIRD_MEM_BACKUP == YES)
		RepairFreezeUseFlash();
		#endif
	}
	PowerUpCompleteFreeze();

	//上电置允许分钟冻结标志
	minFreezeFlag = 0xA5;	
}



//-----------------------------------------------
//函数功能: 初始化冻结参数
//
//参数: 
//	无
//	
//  返回值:	
//  无
//
//备注:	  初始化调用
//-----------------------------------------------
void api_FactoryInitFreeze( void )
{
	api_WriteFreezeAttribue( 0x5000, 0xFF, (BYTE *)&InstantFreeze[1], InstantFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5002, 0xFF, (BYTE *)&MinFreeze[1], MinFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5003, 0xFF, (BYTE *)&HourFreeze[1], HourFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5004, 0xFF, (BYTE *)&DayFreeze[1], DayFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5005, 0xFF, (BYTE *)&ClosingFreeze[1], ClosingFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5006, 0xFF, (BYTE *)&MonFreeze[1], MonFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5008, 0xFF, (BYTE *)&TZChgFreeze[1], TZChgFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5009, 0xFF, (BYTE *)&DTTChgFreeze[1], DTTChgFreeze[0].Cycle );
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	api_WriteFreezeAttribue( 0x500a, 0xFF, (BYTE *)&TariffRateChgFreeze[1], TariffRateChgFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x500b, 0xFF, (BYTE *)&LadderChgFreeze[1], LadderChgFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5011, 0xFF, (BYTE *)&LadderClosingFreeze[1], LadderClosingFreeze[0].Cycle );
	#endif
	FreezeFlag = 0;//初始化后冻结从头开始
	memset( (BYTE*)&FreezePowerDownTimeBak, 0x00, sizeof(TRealTimer) );//置时间为0//初始化后第一次补冻结无论是否是异常复位都会补冻结
	memset( (BYTE*)&CompleteFreezeTime, 0x00, sizeof(TCompleteFreezeTime) );
	CompleteFreezeTime.CRC32 = lib_CheckCRC32((BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
	//清修改时间标志
//	#if(THIRD_MEM_CHIP != CHIP_NO)
	api_ClrSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag) );
//	#endif
}

#if (SLE_THIRD_MEM_BACKUP == YES)
//-----------------------------------------------
// 函数功能: 获取冻结数据
//
// 参数: 	pBuf[in/out]
//
// 返回值:	无
//
// 备注:
//-----------------------------------------------
void api_GetFreezeFlashMemData(TFreezeDataInfo *pBuf)
{
	memcpy((BYTE *)pBuf, (BYTE *)&MinInfo, sizeof(MinInfo));
}

//-----------------------------------------------
// 函数功能:保存FLASH的电能及冻结指针备份数据
//
// 参数: 	dwRecordNo[in] FLASH备份区指针
//        Type[in] 0x55 RecordNo发生改变，0x00 RecordNo未发生改变
//        pBuf[in]
//
// 返回值:	无
//
// 备注:
//-----------------------------------------------
void SaveFlashMemData(DWORD dwRecordNo, BYTE Type, BYTE* pBuf)
{
	DWORD dwAddr;
	TSectorInfo SectorInfo = {0};
	TFlashMem* pFlashMemTemp; //使用指针减少栈开销

	// 计算此数据保存的绝对地址
	if (GetSectorInfo(FLASH_MEM_DATA_LEN, FLASH_MEM_DEPTH, dwRecordNo, &SectorInfo) == FALSE)
	{
		return;
	}

	dwAddr = FLASH_MEM_BASE_ADDR + SectorInfo.dwAddr;
	pFlashMemTemp = (TFlashMem*)pBuf;
	// 获取电能数据
	api_GetEnergyFlashMemData(&pFlashMemTemp->Energy);
	// 获取冻结数据
	api_GetFreezeFlashMemData(&pFlashMemTemp->MinDataInfo[0]);

	// 判断数据有效性
	if (pFlashMemTemp->MinDataInfo[0].RecordNo != (dwRecordNo + 1))
	{
		return;
	}
	
	CLEAR_WATCH_DOG;

	// 保存数据
	if (Type == 0x55)
	{
		WriteFreezeRecordData_EraseFlash(dwAddr, FLASH_MEM_DATA_LEN, pBuf);
	}
	else
	{
		if (WriteFreezeRecordData(dwAddr, FLASH_MEM_DATA_LEN, FLASH_MEM_BASE_ADDR, FLASH_MEM_SIZE, pBuf) == FALSE)
		{
			WriteFreezeRecordData_EraseFlash(dwAddr, FLASH_MEM_DATA_LEN, pBuf);
		}
	}
	
	CLEAR_WATCH_DOG;
	
	// 保存指针
	dwAddr = FLASH_MEM_RECORDNO_ADDR + (dwRecordNo % FLASH_MEM_RECORDNO_DEPTH) * FLASH_MEM_RECORDNO_LEN;

	memcpy( pBuf, (BYTE *)&dwRecordNo, FLASH_MEM_RECORDNO_LEN );

	// 冻结序号发生改变，将FLASH指针清零
	if (Type == 0x55)
	{
		InitFlashMem();
	}

	if (WriteFreezeRecordData(dwAddr, FLASH_MEM_RECORDNO_LEN, FLASH_MEM_RECORDNO_ADDR, FLASH_MEM_RECORDNO_SIZE, pBuf) == FALSE)
	{
		WriteFreezeRecordData_EraseFlash(dwAddr, FLASH_MEM_RECORDNO_LEN, pBuf);
	}
}

//-----------------------------------------------
// 函数功能: 查找FLASH备份数据中的最大值
//
// 参数: pFlashMemNo[in/out] 返回冻结指针
//
// 返回值:
//
// 备注:遍历FLASH区的所有数据，查找指针最大值
//-----------------------------------------------
BOOL FindFlashMemDataMaxNo(DWORD *pFlashMemNo)
{
	BYTE FlashBuf[SECTOR_SIZE];
	DWORD j, TempNo, TempNoBack;

	CLEAR_WATCH_DOG;
	
	pFlashMemNo[0] = ILLEGAL_VALUE_8F;

	if (api_ReadMemRecordData(FLASH_MEM_RECORDNO_ADDR, SECTOR_SIZE, FlashBuf) != TRUE)
	{
		return FALSE;
	}

	if ( lib_IsAllAssignNum( FlashBuf, 0xFF, sizeof(FlashBuf) )== TRUE )
	{
		return FALSE;
	}
	
	TempNo = 0;
	TempNoBack = 0;

	//查找最大值
	for (j = 0; j < FLASH_MEM_RECORDNO_DEPTH; j++)
	{
		memcpy((BYTE *)&TempNo, (BYTE *)&FlashBuf[(j * FLASH_MEM_RECORDNO_LEN)], sizeof(TempNo));

		if (TempNo == ILLEGAL_VALUE_8F)
		{
			continue;
		}

		if( TempNoBack < TempNo )
		{
			TempNoBack = TempNo;
		}
	}

	pFlashMemNo[0] = TempNoBack;

	return TRUE;
}

//-----------------------------------------------
// 函数功能: 查找FLASH备份数据最近1条有效数据的指针
//
// 参数:pFlashMemNo[in]
//
// 返回值:
//
// 备注:
//-----------------------------------------------
BOOL api_FindFlashMemDataNo(DWORD *pFlashMemNo)
{
	BYTE i,Buf[4];
	DWORD dwAddr, FlashMemNo, Offset;
	TFreezeDataInfo TempMinInfo;
	TSectorInfo SectorInfo = {0};

	// 检查RAM的校验
	if (lib_CheckSafeMemVerify((BYTE *)&MinInfo[0], sizeof(TFreezeDataInfo), UN_REPAIR_CRC) == TRUE)
	{
		if (MinInfo[0].RecordNo == 0)
		{
			return FALSE;
		}

		pFlashMemNo[0] = MinInfo[0].RecordNo - 1;

		return TRUE;
	}

	// 查找FLASH中的最大指针
	if (FindFlashMemDataMaxNo(&FlashMemNo) == FALSE)
	{
		FlashMemNo = ILLEGAL_VALUE_8F;
	}
	else
	{
		//检查数据合法性
		if( api_GetFlashMemData(FlashMemNo, GET_STRUCT_ADDR(TFlashMem, MinDataInfo), 4, Buf) == FALSE )
		{
			FlashMemNo = ILLEGAL_VALUE_8F;
		}
	}

	// 从EEP恢复
	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeInfoRom.MinDataInfo[0]), sizeof(TFreezeDataInfo), (BYTE *)&TempMinInfo) == FALSE)
	{
		TempMinInfo.RecordNo = ILLEGAL_VALUE_8F;
	}
	else
	{
		TempMinInfo.RecordNo -= 1;

		//检查数据合法性
		if( api_GetFlashMemData(TempMinInfo.RecordNo, GET_STRUCT_ADDR(TFlashMem, MinDataInfo), 4, Buf) == FALSE )
		{
			TempMinInfo.RecordNo = ILLEGAL_VALUE_8F;
		}
	}

	pFlashMemNo[0] = ILLEGAL_VALUE_8F;

	if ((FlashMemNo == ILLEGAL_VALUE_8F) && (TempMinInfo.RecordNo == ILLEGAL_VALUE_8F))
	{
		// EE,FLASH都错误
	}
	else if ((FlashMemNo == ILLEGAL_VALUE_8F) && (TempMinInfo.RecordNo != ILLEGAL_VALUE_8F))
	{
		// EE正确,FLASH错误
		pFlashMemNo[0] = TempMinInfo.RecordNo;
	}
	else if ((FlashMemNo != ILLEGAL_VALUE_8F) && (TempMinInfo.RecordNo == ILLEGAL_VALUE_8F))
	{
		// EE错误,FLASH正确
		pFlashMemNo[0] = FlashMemNo;
		
		// 强制刷新分钟冻结指针
		RefreshFreezeUseFlash(pFlashMemNo[0]);
	}
	else
	{
		// EE,FLASH都正确，使用大值
		if (FlashMemNo > TempMinInfo.RecordNo)
		{
			// 返回FLASH
			pFlashMemNo[0] = FlashMemNo;

			// 强制刷新分钟冻结指针
			RefreshFreezeUseFlash(pFlashMemNo[0]);
		}
		else
		{
			// 返回EE
			pFlashMemNo[0] = TempMinInfo.RecordNo;
		}
	}

	if (pFlashMemNo[0] == ILLEGAL_VALUE_8F)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

//-----------------------------------------------
// 函数功能:读取FLASH中的备份数据
//
// 参数:dwRecordNo[in]
//		Offset[in]
//		Len[in]
//		pBuf[in/out]
//
// 返回值:	无
//
// 备注:
//-----------------------------------------------
BOOL api_GetFlashMemData(DWORD dwRecordNo, WORD Offset, WORD Len, BYTE *pBuf)
{
	DWORD dwAddr,FlashRecordNo;
	TSectorInfo SectorInfo = {0};

	if ((Len > 1000)||(dwRecordNo == ILLEGAL_VALUE_8F))
	{
		return FALSE;
	}
	CLEAR_WATCH_DOG;
	
	// 计算此数据保存的绝对地址
	if (GetSectorInfo(FLASH_MEM_DATA_LEN, FLASH_MEM_DEPTH, dwRecordNo, &SectorInfo) == FALSE)
	{
		return FALSE;
	}

	dwAddr = FLASH_MEM_BASE_ADDR + SectorInfo.dwAddr + GET_STRUCT_ADDR(TFlashMem, MinDataInfo[0].RecordNo);

	if (api_ReadMemRecordData(dwAddr, 4, (BYTE*)&FlashRecordNo) == FALSE)
	{
		return FALSE;
	}
	
	// 检查数据合法性，判断FLASH中的Record是否一致
	if( FlashRecordNo != (dwRecordNo+1) )
	{
		return FALSE;
	}
	
	dwAddr = FLASH_MEM_BASE_ADDR + SectorInfo.dwAddr + Offset;

	if (api_ReadMemRecordData(dwAddr, Len, pBuf) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
// 函数功能: 通过FALSH中的数据刷新冻结指针
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
BOOL RefreshFreezeUseFlash( DWORD RecordNo )
{
	BYTE i;
	TFreezeDataInfo TempMinDataInfo[MAX_FREEZE_PLANNUM] = {0};
	TFreezeDataInfo TempMinDataInfoEE = {0};
	
	// 读取数据
	if (api_GetFlashMemData(RecordNo, GET_STRUCT_ADDR(TFlashMem, MinDataInfo), sizeof(TempMinDataInfo), (BYTE *)&TempMinDataInfo) != TRUE)
	{
		return FALSE;
	}

	for (i = 0; i < MAX_FREEZE_PLANNUM; i++)
	{
		CLEAR_WATCH_DOG;
		
		// 检查FLASH正确性
		if (TempMinDataInfo[i].CRC32 != lib_CheckCRC32((BYTE *)&TempMinDataInfo[i], (sizeof(TFreezeDataInfo) - sizeof(DWORD))))
		{
			continue;
		}

		// 检查EE正确性
		if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeInfoRom.MinDataInfo[i]), sizeof(TFreezeDataInfo), (BYTE *)&TempMinDataInfoEE) == TRUE)
		{
			if(( i == 0 )&&( TempMinDataInfo[i].RecordNo > TempMinDataInfoEE.RecordNo ))
			{
				// 方案0满足FLASH > EE，则使用FLASH数据覆盖EE
			}
			else
			{
				continue;
			}
		}

		// 修复RAM
		memcpy((BYTE *)&MinInfo[i], (BYTE *)&TempMinDataInfo[i], sizeof(TFreezeDataInfo));

		// 修复EE
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeInfoRom.MinDataInfo[i]), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i]);
	}

	api_WriteSysUNMsg(SYSUN_WRITE_FREEZE_EE);

	return TRUE;
}

//-----------------------------------------------
// 函数功能: 通过FALSH中的数据修复冻结指针
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
BOOL RepairFreezeUseFlash(void)
{
	DWORD RecordNo;
	
	// 获取数据指针
	if (FindFlashMemDataMaxNo(&RecordNo) != TRUE)
	{
		return FALSE;
	}

	RefreshFreezeUseFlash( RecordNo );

	return TRUE;
}

//-----------------------------------------------
// 函数功能: 初始化flash备份参数
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void InitFlashMem(void)
{
	BYTE Temp[4];
	DWORD i;

	memset(Temp, 0xFF, sizeof(Temp));

	for (i = 0; i < FLASH_MEM_RECORDNO_SECTOR_NUM; i++)
	{
		CLEAR_WATCH_DOG;
		
		api_WriteMemRecordData(FLASH_MEM_RECORDNO_ADDR + i * SECTOR_SIZE, sizeof(Temp), Temp);
	}
}
#endif // #if( SLE_THIRD_MEM_BACKUP == YES )
