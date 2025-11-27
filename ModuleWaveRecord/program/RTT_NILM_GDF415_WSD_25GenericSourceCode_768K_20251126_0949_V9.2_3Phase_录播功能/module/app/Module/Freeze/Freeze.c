//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.8.27
//描述		冻结模块源文件
//修改记录	
//---------------------------------------------------------------------- 
#include "AllHead.h"
#include "Freeze.h"
#include "fal.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define OI4800_04_SIZE				13											// 48000400 一个结构体对应的长度
#define	FREEZE_RECORD_NO_OAD		0x00022320									// 冻结记录序号OAD
#define	FREEZE_RECORD_TIME_OAD		0x00022120									// 冻结时间OAD
#define NUM_OAD  					(sizeof(MinFreeze)/sizeof(TFreezePara))		//冻结属性个数
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eALL_OAD_LEN,				// 所有OAD的长度
	eSELECTED_OAD_LEN			// 选中的OAD的长度
}eOadLenType;

typedef struct TTBinarySchInfo_t
{
	DWORD		dwBaseAddr;			//in 此属性对应数据的首地址
	DWORD		dwDepth;			//in 冻结深度
	DWORD		dwRecordNo;			//in 最近一个点的记录号
	DWORD		dwSaveDot;			//in 需要传递剩余的冻结记录个数，进行二分法查找时用到
	DWORD		Time;				//in 要搜索的时间
	DWORD		*pRecordNo;			//out 返回找到的RecordNo
	DWORD		*pTime;				//out 返回找到的时间
	WORD		wLen;				//in 此属性单个数据的长度
}TBinarySchInfo;

// 没存EEP，每次用的时候计算
typedef struct TSectorInfo_t
{		
//	DWORD		wPointer;		// 寻找点指针
	DWORD		dwAddr;			// 寻找点地址		
//	DWORD		dwSaveSpace;	// 此OAD存储空间大小
}TSectorInfo;


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
WORD SaveFreeze = 0;//存冻结的标志
WORD GetFreeze = 0;//取冻结的标志
#if(PROTOCOL_VERSION == 25)	
WORD SaveEvent = 0;//存事件的标志
#endif
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
TFreezeDataInfo	MinInfo[MAX_FREEZE_PLAN];				// 冻结指针

#if( MAX_PHASE == 3 )
#define MinPlanDataLen 		(4 + (OI4800_04_SIZE * NILM_EQUIPMENT_MAX_NO)) // 4字节为时间，2+4+4+1+1+1=13 最多30个电器同时开启
#define DayPlanDataLen		8192 //2088
#else
#define MinPlanDataLen 		(4 + (OI4800_04_SIZE * NILM_EQUIPMENT_MAX_NO)) // 4字节为时间，2+4+4+1+1+1=13 最多30个电器同时开启
#define DayPlanDataLen		4096 //2088
#endif

#define MonPlanDataLen		4096 //2088


#define MinFreezeDensity	15			// 负荷辨识模组15分钟冻结一次数据
#define DayFreezeDensity	1440
#define MonFreezeDensity	(1440*30)

#if( MAX_PHASE == 3 )
TFreezePara MinFreeze[] =
{
	//	Cycle				Oad			Depth				Len			Plan 	Offset 	PlanDataLen 		PlanOffset
	{MinFreezeDensity, 		0x00040048, FREEZE_MIN_DEPTH1, 	1000, 		0, 		4, 		MinPlanDataLen, 	0}, //  48000400—非介入式负荷辨识模块  电器设备冻结数据单元
};
#else
TFreezePara MinFreeze[]=
{
	//	Cycle				Oad			Depth				Len			Plan 	Offset 	PlanDataLen 		PlanOffset 
	{MinFreezeDensity,		0x00040048, FREEZE_MIN_DEPTH1,	1000,		0,		4,		MinPlanDataLen,		0 },//  48000400—非介入式负荷辨识模块  电器设备冻结数据单元
};
#endif


#if( MAX_PHASE == 3 )
 TFreezePara DayFreeze[]=
{
//	Cycle		Oad			Depth			Len		Plan 	Offset 	PlanDataLen PlanOffset 
/*	{DayFreezeDensity,		0x00026320, FREEZE_DAY_DEPTH,	1000,		1,		4,		DayPlanDataLen,		0 },// 20630200—A相当日谐波电压统计（2-25次）20*25=500
	{DayFreezeDensity,		0x00036320, FREEZE_DAY_DEPTH,	1000,		1,	 504,		DayPlanDataLen,		0 },// 20630300—B相当日谐波电压统计（2-25次）
	{DayFreezeDensity,		0x00046320, FREEZE_DAY_DEPTH,	1000,		1,	 1004,		DayPlanDataLen,		0 },// 20630400—C相当日谐波电压统计（2-25次）
	
	{DayFreezeDensity,		0x00026420, FREEZE_DAY_DEPTH,	1000,		1,	 1504,		DayPlanDataLen,		0 },// 20640200—A相当日谐波电流统计（2-25次）
	{DayFreezeDensity,		0x00036420, FREEZE_DAY_DEPTH,	1000,		1,	 2004,		DayPlanDataLen,		0 },// 20640300—B相当日谐波电流统计（2-25次）
	{DayFreezeDensity,		0x00046420, FREEZE_DAY_DEPTH,	1000,		1,	 2504,		DayPlanDataLen,		0 },// 20640400—C相当日谐波电流统计（2-25次）
	
	{DayFreezeDensity,		0x00026520, FREEZE_DAY_DEPTH,	1000,		1,	 3004,		DayPlanDataLen,		0 },// 20650200—A相当日间谐波电压统计（0.5-20.5次）
	{DayFreezeDensity,		0x00036520, FREEZE_DAY_DEPTH,	1000,		1,	 3504,		DayPlanDataLen,		0 },// 20650300—B相当日间谐波电压统计（0.5-20.5次）
	{DayFreezeDensity,		0x00046520, FREEZE_DAY_DEPTH,	1000,		1,	 4004,		DayPlanDataLen,		0 },// 20650400—C相当日间谐波电压统计（0.5-20.5次）
	
	{DayFreezeDensity,		0x00026620, FREEZE_DAY_DEPTH,	1000,		1,	 4504,		DayPlanDataLen,		0 },// 20660200—A相当日间谐波电流统计
	{DayFreezeDensity,		0x00036620, FREEZE_DAY_DEPTH,	1000,		1,	 5004,		DayPlanDataLen,		0 },// 20660300—B相当日间谐波电流统计（0.5-20.5次）
	{DayFreezeDensity,		0x00046620, FREEZE_DAY_DEPTH,	1000,		1,	 5504,		DayPlanDataLen,		0 },// 20660400—C相当日间谐波电流统计

	
	{DayFreezeDensity,		0x00026820, FREEZE_DAY_DEPTH,	1000,		1,	 6004,		DayPlanDataLen,		0 },// 20680200—当日电压偏差统计 20
	{DayFreezeDensity,		0x00026920, FREEZE_DAY_DEPTH,	1000,		1,	 6024,		DayPlanDataLen,		0 },// 20690200—当日频率偏差统计

	{DayFreezeDensity,		0x00046A20, FREEZE_DAY_DEPTH,	1000,		1,	 6044,		DayPlanDataLen,		0 },// 206A0400—当日三相电压不平衡度统计
	{DayFreezeDensity,		0x00046B20, FREEZE_DAY_DEPTH,	1000,		1,	 6064,		DayPlanDataLen,		0 },// 206B0400—当日三相电流不平衡度统计

	{DayFreezeDensity,		0x00026C20, FREEZE_DAY_DEPTH,	1000,		1,	 6084,		DayPlanDataLen,		0 },// 206C0200—当日电压波动统计	√	√
	{DayFreezeDensity,		0x00026720, FREEZE_DAY_DEPTH,	1000,		1,	 6104,		DayPlanDataLen,		0 },// 20670200—当日闪变统计	√	
*/
	//新协议
	//{DayFreezeDensity,		0x00022221, FREEZE_DAY_DEPTH,	1000,		1,	 4,							DayPlanDataLen,		0 },// 	√	
	{DayFreezeDensity,		0x00022221, FREEZE_DAY_DEPTH,	1000,		1,	 4,								DayPlanDataLen,		0 },// 	√	
	{DayFreezeDensity,		0x0D022221, FREEZE_DAY_DEPTH,	1000,		1,	 700,					DayPlanDataLen,		0 },// 	√	
	{DayFreezeDensity,		0x0E022221, FREEZE_DAY_DEPTH,	1000,		1,	 800,					DayPlanDataLen,		0 },// 	√	
	{DayFreezeDensity,		0x0F022221, FREEZE_DAY_DEPTH,	1000,		1,	 900,					DayPlanDataLen,		0 },// 	√	

	{DayFreezeDensity,		0x01022721, FREEZE_DAY_DEPTH,	1000,		1,	 1028,						DayPlanDataLen,		0 },// 	√	
	{DayFreezeDensity,		0x02022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*1),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x03022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*2),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x04022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*3),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x05022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*4),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x06022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*5),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x07022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*6),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x08022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*7),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x09022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*8),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x0A022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*9),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x0B022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*10),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x0C022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*11),		DayPlanDataLen,		0 },// 	

 };
#else
 TFreezePara DayFreeze[]=
{
//	Cycle		Oad 		Depth			Len 	Plan	Offset	PlanDataLen PlanOffset 

	{DayFreezeDensity,		0x00022221, FREEZE_DAY_DEPTH,	1000,		1,	 4,								DayPlanDataLen,		0 },// 	√	
	//{DayFreezeDensity,		0x02022221, FREEZE_DAY_DEPTH,	1000,		1,	 (40*1),					DayPlanDataLen,		0 },// 	√	
	//{DayFreezeDensity,		0x03022221, FREEZE_DAY_DEPTH,	1000,		1,	 (40*2),					DayPlanDataLen,		0 },// 	√	
	//{DayFreezeDensity,		0x04022221, FREEZE_DAY_DEPTH,	1000,		1,	 (40*3),					DayPlanDataLen,		0 },// 	√	
	//{DayFreezeDensity,		0x05022221, FREEZE_DAY_DEPTH,	1000,		1,	 (40*4),					DayPlanDataLen,		0 },// 	√	
		
	{DayFreezeDensity,		0x01022721, FREEZE_DAY_DEPTH,	1000,		1,	 1028,						DayPlanDataLen,		0 },// 	√	
	{DayFreezeDensity,		0x02022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*1),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x03022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*2),		DayPlanDataLen,		0 },// 	
	{DayFreezeDensity,		0x04022721, FREEZE_DAY_DEPTH,	1000,		1,	 (1028+512*3),		DayPlanDataLen,		0 },// 	

};
#endif

 #if( MAX_PHASE == 3 )
 TFreezePara MonFreeze[]=
{
//	Cycle		Oad			Depth			Len		Plan 	Offset 	PlanDataLen PlanOffset 
	{MonFreezeDensity,		0x00023121, FREEZE_MON_DEPTH,	1000,		2,		4,			MonPlanDataLen,		0 },//
	{MonFreezeDensity,		0x00023221, FREEZE_MON_DEPTH,	1000,		2,		204,		MonPlanDataLen,		0 },//
	{MonFreezeDensity,		0x00023321, FREEZE_MON_DEPTH,	1000,		2,		404,		MonPlanDataLen,		0 },//

	{MonFreezeDensity,		0x00020321, FREEZE_MON_DEPTH,	1000,		2,		604,		MonPlanDataLen,		0 },// 
};
#else
TFreezePara MonFreeze[]=
{
//	Cycle		Oad 		Depth			Len 	Plan	Offset	PlanDataLen PlanOffset 
	{MonFreezeDensity,		0x00023121, FREEZE_MON_DEPTH,	1000,		2,	 (4),		MonPlanDataLen,		0 },// 	
	{MonFreezeDensity,		0x00020321, FREEZE_MON_DEPTH,	1000,		2,	 (504),		MonPlanDataLen,		0 },// 	

};
#endif

static const BYTE bConstTempBuf[OI4800_04_SIZE]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
BYTE FlashBuf2[SECTOR_SIZE];
#if(PROTOCOL_VERSION == 25)	
const WORD g_Default_Freeze[4] = {15,30,45,60};//周期
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static WORD BinSearchByTime( TBinarySchInfo *pBinSchInfo );

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern WORD api_ClrSafeMem( WORD Addr, WORD Length );

BYTE GetFreezeTypeFromOI(TDLT698RecordPara *pDLT698RecordPara)
{
	if(pDLT698RecordPara == NULL)
		return 0xFF;

	if(pDLT698RecordPara->OI == 0x5002)				//调用分钟冻结
		return 0;
	else if(pDLT698RecordPara->OI == 0x5004)		//日冻结
		return 1;
	else if(pDLT698RecordPara->OI == 0x5006)        //月冻结
		return 2;
	else
		return 0xFF;
}


TFreezePara* GetFreezeType(BYTE byType, int *npLen, DWORD* pdwFlashAddr)
{
	TFreezePara* p;

	*npLen = 0;
	*pdwFlashAddr = 0;

	switch(byType)
	{
	case 0:
		p = &MinFreeze[0];
		*npLen = (sizeof(MinFreeze)/sizeof(TFreezePara));
		*pdwFlashAddr = fal_partition_start("minFrz");
		break;
	case 1:
		p = &DayFreeze[0];
		*npLen = (sizeof(DayFreeze)/sizeof(TFreezePara));
		*pdwFlashAddr = fal_partition_start("dayFrz");
		break;
	case 2:
		p = &MonFreeze[0];
		*npLen = (sizeof(MonFreeze)/sizeof(TFreezePara));
		*pdwFlashAddr = fal_partition_start("monFrz");
		break;
	default:
		*npLen = 0;
		p = NULL;
		*pdwFlashAddr = 0xFFFFFFFF;
		break;
	}
	return p;
}
BOOL SetFreezePara(BYTE byType,BYTE byIndex, DWORD dwValue)
{
	TFreezePara* pFree;
	int i,nLen;
	DWORD dwMinFlashStart;

	pFree = GetFreezeType(byType, &nLen, &dwMinFlashStart);
	if(pFree == NULL)
		return FALSE;

	for(i=0; i<nLen; i++)
	{
		pFree[i].Cycle = dwValue;
	}
	return TRUE;
}
//-----------------------------------------------
// 函数功能: 获取数据存储的相关信息
//
// 参数:
// 			Len[in]:			整条记录的长度（包括时标）
// 			RecordDepth[in]:	要读取OAD数据的存储深度
// 			inRecordNo[in]:		要读取OAD数据的记录序号
// 			pSectorInfo[out]:	指向SectorInfo的指针
// 返回值:
//	无
//
// 备注:  电能质量分钟冻结一条存一个扇区，负荷辨识模组要存2880条，一个扇区可以存多条分钟冻结
//-----------------------------------------------
BOOL GetSectorInfo( DWORD Len, DWORD RecordDepth, DWORD inRecordNo, TSectorInfo *pSectorInfo )
{
	WORD twSectorNo;	   // 此OAD占用的扇区个数
	WORD twOadNoPerSector; // 一个扇区能存放的OAD的条数
	WORD twSectorOff;	   // 扇区的偏移
	WORD twInSectorOff;	   // 扇区内的偏移

	//ASSERT( Len<2000, 0 );
	//ASSERT( inRecordNo<0x80000000, 0 );
	
	if( (Len==0) || (RecordDepth==0) )
	{
		//ASSERT(0,0);
		return FALSE;
	}
	else
	{
		// 一个扇区存的OAD的个数
		twOadNoPerSector = SECTOR_SIZE / Len;
		if (RecordDepth < twOadNoPerSector)
		{
			twOadNoPerSector = RecordDepth;
		}
		// 此OAD占扇区个数
		if ((RecordDepth % twOadNoPerSector) == 0)
		{
			twSectorNo = (RecordDepth / twOadNoPerSector) + 1;
		}
		else
		{
			twSectorNo = ((RecordDepth / twOadNoPerSector) + 1) + 1; // 进位加1
		}
		// 扇区的偏移
		twSectorOff = inRecordNo / twOadNoPerSector;
		twSectorOff %= twSectorNo;
		// 扇区内的偏移
		twInSectorOff = inRecordNo % twOadNoPerSector;

//		pSectorInfo->wPointer = twSectorOff * twOadNoPerSector + twInSectorOff;
//		pSectorInfo->dwSaveSpace = SECTOR_SIZE * twSectorNo;
		pSectorInfo->dwAddr = (DWORD)twSectorOff * SECTOR_SIZE + twInSectorOff * Len;

		return TRUE;
	}
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
    
	for( i = 0; i < (sizeof(MinInfo)/sizeof(TFreezeDataInfo)); i++ )
	{
		// 检查RAM的校验
		if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == TRUE )
		{
			//校验对，指针合法写入EE
			if(( MinInfo[i].RecordNo != 0 )&&( MinInfo[i].RecordNo != ILLEGAL_VALUE_8F ))
			{
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] );
			}
		}
	}
}

//-----------------------------------------------
//函数功能: 初始化分钟冻结指针信息
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

	for( i = 0; i < (sizeof(MinInfo)/sizeof(TFreezeDataInfo)); i++ )
	{
		MinInfo[i].RecordNo = 0;
		MinInfo[i].SaveDot = 0;
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] );
	}
}

#if(PROTOCOL_VERSION == 25)	
//--------------------------------------------------
//功能描述:  获取冻结周期
//         
//参数:      BYTE Flag：1代表修复冻结深度为默认值，0代表不修复。
//			 BYTE *pBuf：存放冻结周期的指针 
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_GetFreezeCycle(BYTE *pBuf )
{
	BOOL Res = FALSE;	
	TFreezeCycleSafeRom Frzpara;
	
	Res = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeCycleInfo.Cycle), sizeof(TFreezeCycleSafeRom), (BYTE*)&Frzpara);
	if(Res != FALSE)
	{
		memcpy((BYTE*)&pBuf[0],(BYTE*)&Frzpara.Cycle, sizeof(WORD));
	}
	else
	{
		memcpy((BYTE*)&pBuf[0],(BYTE*)&MinFreeze[0].Cycle, sizeof(WORD));//默认值
	}
}
//--------------------------------------------------
//函数功能: 写冻结关联对象属性
//
//参数: 
//	OI[in]:				0x5000~0x500B,0x5011 冻结类的对象标识OI,目前支持5002，后续其他oad需要调试 !!!
//  pBuf[out]: 			返回数据缓冲    
//
//备注: 正常数据长度( 在个数为0时，添加1字节的0 返回长度为1)
//--------------------------------------------------
BOOL  api_WriteFreezeAttribute( WORD OI, BYTE *pBuf )
{
	BYTE Result = 0;
	BOOL DAR;
	TFreezeCycleSafeRom Frzpara;

	DAR = FALSE; 

	if(OI != 0x5002)
		return DAR;

	api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeCycleInfo.Cycle), sizeof(TFreezeCycleSafeRom), (BYTE*)&Frzpara);

	if((memcmp((BYTE *)&Frzpara.Cycle, pBuf, FREEZE_CYCLE_LEN ) == 0) && (Frzpara.Cycle != 0)) //设置值和原来相等
	{
		DAR = TRUE;
	}
	else
	{
		memcpy((BYTE *)&Frzpara.Cycle, pBuf, FREEZE_CYCLE_LEN );

		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeCycleInfo.Cycle), sizeof(TFreezeCycleSafeRom), (BYTE*)&Frzpara);

		if(Result == FALSE)
		{
			DAR = FALSE;
		}
		else
		{
			//算法接口，需要把结果告知
			set_freeze_period(Frzpara.Cycle);
			api_FactoryInitFreeze();//清除冻结
			DAR = TRUE;
		}
	}
	return DAR;
}
#endif //	#if(PROTOCOL_VERSION == 25)	

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
	BYTE i;
	BYTE TempBuf[FREEZE_ATTRIBUTE_LEN];
	WORD wLenTemp;
	#if(PROTOCOL_VERSION == 25)	
	WORD wFreezeCycle = 0;
	#endif
	DWORD	dwMinFlashStart;
	BYTE byType;
	TFreezePara* pFree;
	int nLen;
	
	if(OI != 0x5002)
	{
		return FALSE;
	}

	byType = 0;

	pFree = GetFreezeType(byType, &nLen, &dwMinFlashStart);

	if(pFree == NULL)
		return FALSE;

	wLenTemp = 0;


	if(nLen > 36)
		nLen = 36;

	if(ElementIndex > 1)
	{
		return FALSE;
	}
	#if(PROTOCOL_VERSION == 25)	
	api_GetFreezeCycle((BYTE*)&wFreezeCycle);
	#endif

	if( ElementIndex == 0 )
	{
		for( i = 0; i < nLen; i++ )
		{
			// 判断是否超出缓冲最大长度
			if( (wLenTemp+FREEZE_ATTRIBUTE_LEN) > Len )
			{
				return 0;
			}
			
			memcpy( TempBuf+FREEZE_CYCLE_LEN, (BYTE *)&pFree[i].Oad, FREEZE_OAD_LEN );
			#if(PROTOCOL_VERSION == 25)			
			memcpy( TempBuf, (BYTE *)&wFreezeCycle, FREEZE_CYCLE_LEN );	
			#elif(PROTOCOL_VERSION == 24)	
			memcpy( TempBuf, (BYTE *)&pFree[i].Cycle, FREEZE_CYCLE_LEN );	
			#endif		
			memcpy( TempBuf+FREEZE_CYCLE_LEN+FREEZE_OAD_LEN, (BYTE *)&pFree[i].Depth, FREEZE_DEPTH_LEN );
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
		memcpy( pBuf+FREEZE_CYCLE_LEN, (BYTE *)&pFree[ElementIndex-1].Oad, FREEZE_OAD_LEN );
		#if(PROTOCOL_VERSION == 25)
		memcpy( pBuf, (BYTE *)&wFreezeCycle, FREEZE_CYCLE_LEN );
		#elif(PROTOCOL_VERSION == 24)
		memcpy( pBuf, (BYTE *)&pFree[ElementIndex-1].Cycle, FREEZE_CYCLE_LEN );
		#endif
		memcpy( pBuf+FREEZE_CYCLE_LEN+FREEZE_OAD_LEN, (BYTE *)&pFree[ElementIndex-1].Depth, FREEZE_DEPTH_LEN );
		
		return FREEZE_ATTRIBUTE_LEN;
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
	//清分钟指针
	InitMinFreezeInfo();
	api_ClrSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag) );
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
//函数功能: 更新冻结判断用的时间,并更新RecordNo
//
//参数: 	InOldRelativeMin[in]:修改时间之前的相对时间
//
//返回值:	无
//
//备注:	向未来设时间，补最远一次冻结;
//		向过去设时间覆盖旧数据
//-----------------------------------------------
void api_UpdateFreezeTime(DWORD InOldRelativeMin)
{
	DWORD dwTemp, dwStartTime;
	TBinarySchInfo BinarySchInfo;
	DWORD TmpRelativeMin;
	int i;
	TFreezePara* pFree;
	int nLen;
	DWORD dwMinFlashStart;
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;

	memset( (BYTE*)&FreezeUpdateTimeFlag, 0x00, sizeof(TFreezeUpdateTimeFlag) );
	TmpRelativeMin = api_GetRelativeMin();

	if (TmpRelativeMin < InOldRelativeMin) //修改时间为历史时间，重新记录冻结，覆盖旧数据，可在下次处理冻结时写数据
	{
		for(i=0;i<(sizeof(MinInfo)/sizeof(TFreezeDataInfo));i++)
		{
			if(i != 0) //日冻结和分钟冻结不处理
			{
				continue;
			}
			// 检查冻结指针RecordNo
			if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == FALSE )
			{
				if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] ) == FALSE )
				{
					memset( (BYTE *)&MinInfo[i], 0x00, sizeof(TFreezeDataInfo) );
				}
			}

			pFree = GetFreezeType(i, &nLen, &dwMinFlashStart);
			if(pFree == NULL)
				continue;
			
			dwTemp = MinInfo[i].RecordNo;
			dwStartTime = ILLEGAL_VALUE_8F;
			//填写搜索信息
			BinarySchInfo.dwBaseAddr = dwMinFlashStart + pFree[0].PlanOffset;
			BinarySchInfo.dwDepth = pFree[0].Depth;
			BinarySchInfo.dwRecordNo = MinInfo[i].RecordNo;
			BinarySchInfo.dwSaveDot = MinInfo[i].SaveDot;
			BinarySchInfo.Time = TmpRelativeMin + 1; //设置的时间还有秒，不论秒是否为0，都搜索下一个点
			BinarySchInfo.pRecordNo = &MinInfo[i].RecordNo;
			BinarySchInfo.pTime = &dwStartTime;
			BinarySchInfo.wLen = pFree[0].PlanDataLen;
			if (BinSearchByTime(&BinarySchInfo) == TRUE)
			{
				//根据之前的RecordNo和新搜的RecordNo计算新的 SaveDot
				if (dwTemp > MinInfo[i].RecordNo)
				{
					dwTemp = dwTemp - MinInfo[i].RecordNo;
					if (MinInfo[i].SaveDot > dwTemp)
					{
						MinInfo[i].SaveDot = MinInfo[i].SaveDot - dwTemp;
						FreezeUpdateTimeFlag.Flag |= (0x00000001<<i);
					}
					else
					{
						MinInfo[i].RecordNo = 0;
						MinInfo[i].SaveDot = 0;
					}
				}
				else if (dwTemp < MinInfo[i].RecordNo)
				{
					MinInfo[i].RecordNo = 0;
					MinInfo[i].SaveDot = 0;
				}
				else //if(dwTemp == MinInfo[i].RecordNo)
				{
				
					//continue;
				}
				MinInfo[i].CRC32 = lib_CheckCRC32((BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo) - sizeof(DWORD));
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeInfoRom.MinDataInfo[i]), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i]);
			}
		}
	}
	if(FreezeUpdateTimeFlag.Flag != 0)
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
	}
}



//-----------------------------------------------
//函数功能: 处理分钟冻结任务
//
//参数: 	FreezeTime[in]:		当前时间
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
void ProcMinFreezeTask(BYTE byType, DWORD TmpRelativeMin, eFreezeMode FreezeMode )
{
	BYTE i,bFistOadOfPlan,bPlan;
	WORD TempDataLen;
	DWORD dwAddr,dwBackTime;
	TSectorInfo SectorInfo;
	BYTE *pAllocBuf;
	DWORD	dwMinFlashStart;
	DWORD	dwMinFlashEnd;
	DWORD   dwDepth;
	TFreezePara* pFree;
	int nLen;
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;

	pFree = GetFreezeType(byType, &nLen, &dwMinFlashStart);

	if(byType == 0)
	{
		dwMinFlashStart = fal_partition_start("minFrz"); //	FLASH_FREEZE_BASE_MIN  FAL_PART_TABLE
		dwMinFlashEnd   = fal_partition_end("minFrz");	//	FLASH_FREEZE_END_ADDR_MIN
	}
	else if(byType == 1)
	{
		dwMinFlashStart = fal_partition_start("dayFrz");//	FLASH_FREEZE_BASE_MIN
		dwMinFlashEnd   = fal_partition_end("dayFrz");	//	FLASH_FREEZE_END_ADDR_MIN
	}
	else if(byType == 2)
	{
		dwMinFlashStart = fal_partition_start("monFrz");//	FLASH_FREEZE_BASE_MIN
		dwMinFlashEnd   = fal_partition_end("monFrz");	//	FLASH_FREEZE_END_ADDR_MIN
	}
	 
	CLEAR_WATCH_DOG;
	bPlan = 0;//强制置为0
	bFistOadOfPlan = 0;
	// 检查冻结指针RecordNo
	if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[bPlan], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == FALSE )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[bPlan] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[bPlan] ) == FALSE )
		{
			memset( (BYTE *)&MinInfo[bPlan], 0x00, sizeof(TFreezeDataInfo) );
		}
	}
	//读出冻结修改时间标志
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);	

	TempDataLen = pFree[bFistOadOfPlan].PlanDataLen;	// 一个冻结数据的长度
	dwDepth = pFree[bFistOadOfPlan].Depth;				// 分钟冻结深度

	GetSectorInfo( TempDataLen, dwDepth, MinInfo[bPlan].RecordNo, &SectorInfo );// 找到要存数据对应的地址

	dwAddr = dwMinFlashStart + pFree[bFistOadOfPlan].PlanOffset;// 偏移为0
	dwAddr = dwAddr + SectorInfo.dwAddr;

	if( dwAddr >= dwMinFlashEnd )
	{
		return;
	}

	//判断备份的修分钟冻结改时间标志
	if( FreezeUpdateTimeFlag.Flag & (0x00000001<<bPlan) )
	{
		memset( FlashBuf2, 0xff, sizeof(FlashBuf2));//全局变量，用之前先清除一下
		//分钟冻结 主数据区与备份区 记录序号差1，
		ProcFreezeNoRollback(MinInfo[0].RecordNo, pFree[0].Depth, pFree[0].PlanDataLen, dwMinFlashStart + pFree[0].PlanOffset, FlashBuf2);
			
		//清除分钟冻结对应方案的修改冻结标志
		FreezeUpdateTimeFlag.Flag &= ~(0x00000001<<bPlan);
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
	}

	//申请此条数据的缓存
	pAllocBuf = (BYTE *)api_AllocBuf( (WORD*)&TempDataLen );
	
	memset( pAllocBuf, 0xff, TempDataLen );
	//时标
	if (FreezeMode == eMAINTASK_FREEZE_MODE)
	{
		memcpy( pAllocBuf, (BYTE *)&TmpRelativeMin, sizeof(DWORD) );		
	}
	else
	{
		memcpy( pAllocBuf, (BYTE *)&dwBackTime, sizeof(DWORD) );
	}
	
	CLEAR_WATCH_DOG;

	// 遍历所有OAD  OAD使用MinFreeze里的配置，不再使用关联对象属性表，目前就一个OAD 48000400，4字节为相对时间的长度
	for (i = 0; i < nLen; i++)
	{
		api_GetProOadData( eData, 0xff, (BYTE *)&pFree[i].Oad, NULL, TempDataLen-sizeof(DWORD), pAllocBuf+sizeof(DWORD) );			
	}
	
	//测试用：报文打印
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 7) & 0x0001) == 0x0001)
	{
		rt_kprintf("start: freeze NO = %d, time data addr = 0x%4X, fixation len = %d, freeze data addr = 0x%4X, freeze len = %d ",
		MinInfo[0].RecordNo,dwAddr,sizeof(DWORD),dwAddr+sizeof(DWORD),TempDataLen-sizeof(DWORD));

		for(i = 0; i < 10; i++)
		{
			rt_kprintf(" 0x%02X",pAllocBuf[i]);
		}
		rt_kprintf("\n");
	}
	#endif

	//写入此条冻结数据，写失败，调用先擦后写函数进行写操作
	if(api_WriteMemRecordData( dwAddr, TempDataLen, pAllocBuf ) == FALSE)
	{
		WriteFreezeRecordData_EraseFlash(dwAddr, TempDataLen, pAllocBuf);
	}
	memset(pAllocBuf,0,TempDataLen);
	api_FreeBuf(pAllocBuf);
	
	MinInfo[bPlan].RecordNo++;
	
	if( MinInfo[bPlan].SaveDot < pFree[bFistOadOfPlan].Depth )
	{
		MinInfo[bPlan].SaveDot++;
	}
	
	MinInfo[bPlan].CRC32 = lib_CheckCRC32( (BYTE*)&MinInfo[bPlan], sizeof(TFreezeDataInfo)-sizeof(DWORD) );
}

//-----------------------------------------------
//函数功能: 处理除分钟冻结冻结任务
//
//参数: 	无
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
void api_FreezeTask( void )
{
	TRealTimer t;
	#if(PROTOCOL_VERSION == 25)
	WORD wFreezeCycle = 0;
	DWORD TmpRelativeMin = 0;
	#endif

	if(GetFreeze == 0xAABB)
	{
		//存分钟冻结
		ProcMinFreezeTask(0, api_GetRelativeMin(), eMAINTASK_FREEZE_MODE );
		SaveMinFreezePoint();
		GetFreeze = 0x0000;
	}

	//分钟变化
	if( api_GetTaskFlag( eTASK_FREEZE_ID, eFLAG_MINUTE ) == TRUE )
	{
		api_ClrTaskFlag(eTASK_FREEZE_ID,eFLAG_MINUTE);
		
		get_sys_time(&t);
		#if(PROTOCOL_VERSION == 25)
		api_GetFreezeCycle((BYTE*)&wFreezeCycle);
		
		TmpRelativeMin = api_GetRelativeMin();
		if((TmpRelativeMin % wFreezeCycle) == 0 )
		{
			SaveFreeze = 0xAABB; //存冻结的标志
		}

		if( (t.Min % 15) == 0 )
		{
			SaveEvent = 0xAABB; //存事件冻结的标志
		}

		#elif(PROTOCOL_VERSION == 24)
		if( (t.Min % MinFreezeDensity) == 0 )
		{
			SaveFreeze = 0xAABB; //存冻结的标志
		}
		#endif //#if(PROTOCOL_VERSION == 25)
	}
	
	//24小时任务
	if( api_GetTaskFlag(eTASK_FREEZE_ID,eFLAG_24_H) == TRUE )//eFLAG_24_H eFLAG_5_MIN   eFLAG_24_H
	{
        // 如果统计还没计算，先不进行冻结
        if( api_GetTaskFlag(eTASK_STATS_ID,eFLAG_10_MIN) == TRUE )
        {
             return;
        }

        api_ClrTaskFlag(eTASK_FREEZE_ID,eFLAG_24_H);


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
//备注:	此函数返回的数据长度带有Tag
//-----------------------------------------------
static WORD GetFreezeAttDataLen( BYTE Type, TFreezeData *pData )
{	
	BYTE i,j,OadIndex;
	WORD wLen,ReturnLen;
	DWORD dwOad,dwTemp;

	TFreezePara* pFree;
	int nLen;
	DWORD dwFlashStart;
	pFree = GetFreezeType(GetFreezeTypeFromOI(pData->pDLT698RecordPara), &nLen, &dwFlashStart);

	if(pFree == NULL)
		return 0;

	ReturnLen = 0;

	if((pData->Tag != eTagData)&&(pData->Tag != eMaxData))
	{
		return 0;
	}

	if( Type == eSELECTED_OAD_LEN )
	{
		for( i=0; i<pData->pDLT698RecordPara->OADNum; i++ )
		{
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+FREEZE_OAD_LEN*i, FREEZE_OAD_LEN );

			if( ( (dwOad&(~0x00E00000))==FREEZE_RECORD_NO_OAD )
			   ||( (dwOad&(~0x00E00000))==FREEZE_RECORD_TIME_OAD ) )
			{
				ReturnLen += 5;	
				continue;
			}

			for( j=0; j<nLen; j++ )
			{
				if(dwOad==pFree[j].Oad)
				{
					break;
				}
			}			

			if( j < nLen )
			{
				wLen = api_GetProOADLen( pData->Tag, (BYTE *)&dwOad, 0 );	
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

				for( j=0; j<nLen; j++ )
				{
					if( pFree[j].Oad == dwOad )
					{
						break;
					}
				}

				if( j < nLen )
				{
					dwTemp = OadIndex;		//元素索引
					dwTemp = (dwTemp<<24);
					dwOad |= dwTemp;		//还原原来带元素索引的OAD
					wLen = api_GetProOADLen( pData->Tag, (BYTE *)&dwOad, 0 );
					//@@@@
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
		for( i=0; i<nLen; i++ )
		{
			dwOad = pFree[i].Oad;
			wLen = api_GetProOADLen( pData->Tag, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )
			{
				return 0;
			}
			ReturnLen += wLen;
		}
		//冻结记录序号+冻结记录时间
		ReturnLen += 10;
	}

	return ReturnLen;
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
	BYTE i,j,k,OadIndex,FreeIndex;
	WORD Offset,wTempLen,TempOffset,wLen;
	DWORD dwAddr,dwOad,dwTempOad,dwTemp1;
	TSectorInfo	SectorInfo;
	TRealTimer 	Time;
	BYTE *pAllocBuf;
	WORD wOutLen;
	
	TFreezePara* pFree;
	int nLen;
	DWORD dwFlashStart;
	pFree = GetFreezeType(GetFreezeTypeFromOI(pData->pDLT698RecordPara), &nLen, &dwFlashStart);
	
	if(pFree == NULL)
		return 0;

	Offset = 0;
	FreeIndex = pData->MinAttOadIndex;
	
	if( pData->pDLT698RecordPara->OADNum == 0 )
	{		
		return 0x8000;
	}
	else
	{
		if((pData->ReadFlashFlag == 0x55)&&(pData->pFreeData != NULL))
		{
			//冻结数据已经读出
			pAllocBuf = pData->pFreeData;
			pData->ReadFlashFlag = 0x00;
		}
		else
		{
			//冻结数据从FLASH中获取
			dwAddr = dwFlashStart + pFree[FreeIndex].PlanOffset;//基地址+此方案的偏移地址
			GetSectorInfo(  pFree[FreeIndex].PlanDataLen,  pFree[FreeIndex].Depth, pData->RecordNo, &SectorInfo );
			dwAddr += SectorInfo.dwAddr;//+此条数据在方案中的偏移地址
			//读取此条冻结数据
			pAllocBuf = (BYTE *)api_AllocBuf( (WORD*)&pFree[FreeIndex].PlanDataLen );
			if(api_ReadMemRecordData( dwAddr, pFree[FreeIndex].PlanDataLen, pAllocBuf ) == FALSE)
			{
				return 0x8000;
			}
		}

		//分钟冻结不可设,所以tag作为const变量存在内部Flash中
		//计量芯不带有显示功能，所以读取数据必定带有tag		
		for( i=0;  i<pData->pDLT698RecordPara->OADNum; i++ )
		{
			CLEAR_WATCH_DOG;
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+FREEZE_OAD_LEN*i, FREEZE_OAD_LEN );
			
			if( ((dwOad&(~0x00E00000)) == FREEZE_RECORD_NO_OAD) 
				|| ((dwOad&(~0x00E00000)) == FREEZE_RECORD_TIME_OAD) )
			{
				////若是分钟冻结，直接采用上面查到的a
				//要读的对象只有RecordNo和Time，则返回第一个对象的
				j = pData->MinAttOadIndex;
			}
			else
			{
				for( j=0; j<nLen; j++ )
				{												
					if( dwOad == pFree[j].Oad )
					{
						break;
					}
				}
			}
					
			if( j < nLen )
			{
				if( (dwOad&(~0x00E00000)) == FREEZE_RECORD_NO_OAD )//40us
				{
					//填充RecordNo
					pData->pBuf[Offset] = 0X06;//TAG
					lib_ExchangeData( pData->pBuf+1+Offset,(BYTE *)&pData->RecordNo, 4 );
					Offset += 5;
				}
				else if( (dwOad&(~0x00E00000)) == FREEZE_RECORD_TIME_OAD )//0.27ms
				{
					pData->pBuf[Offset] = 0X1C;//TAG
					Offset +=1;
					memcpy( (BYTE *)&dwTemp1, (BYTE *)&pAllocBuf[0], sizeof(DWORD) );
					api_ToAbsTime( dwTemp1, (TRealTimer *)&Time );
					pData->pBuf[Offset++] = (BYTE)((Time.wYear&0XFF00)>>8);
					pData->pBuf[Offset++] = (BYTE)(Time.wYear&0X00FF);
					memcpy( pData->pBuf+Offset, (BYTE *)&Time.Mon, sizeof(TRealTimer)-2 );
					Offset += (sizeof(TRealTimer)-2);
				}
				else
				{
					// 检查存了多少个电器
					k = 0;
					memset((BYTE*)&tApplFrzDataFLASH[0].ApplClassID,0xFF,sizeof(tApplFrzDataFLASH));
					for (wTempLen = 0; wTempLen < NILM_EQUIPMENT_MAX_NO; wTempLen++)
					{
						if( memcmp( pAllocBuf + pFree[j].Offset + (WORD)wTempLen*OI4800_04_SIZE, bConstTempBuf,OI4800_04_SIZE) != 0 )
						{
							k++;// 非全0xff
							memcpy((BYTE*)&tApplFrzDataFLASH[wTempLen].ApplClassID,(BYTE *)&pAllocBuf[pFree[j].Offset + (WORD)wTempLen*OI4800_04_SIZE],sizeof(T_ApplFrzData));//解决掉电再读取冻结，一些数据不显示问题
						}
						else
						{
							break;
						}
					}
					api_SetTagArrayNum(k);
		
					wLen = api_GetProOADLen( pData->Tag, (BYTE *)&dwOad, 0 );	
					if( wLen == 0x8000 )
					{
						return 0x8000;
					}

					if( (Offset+ wLen) > pData->Len )//避免超限
                    {
                        return 0; 	// 缓冲满	
                    }

					wOutLen = api_GetProOadData( eAddTag, 0xff, (BYTE *)&dwOad, pAllocBuf+pFree[j].Offset, MAX_APDU_SIZE, pData->pBuf+Offset );
					if((dwOad & 0xFFFF) == 0x2221)
						Offset += wOutLen;
					else
						Offset += wLen;
				}			
			}
			else		
			{
				//指定OAD在关联属性对象表中没有找到，则将元素索引置为0，再找一遍
				OadIndex = (BYTE)(dwOad>>24);
				dwTempOad = dwOad & (~0xFF000000);
				for( j=0; j<nLen; j++ )
				{
					if( dwTempOad == pFree[j].Oad )
					{
						break;
					}
				}

				//存了索引0，读非0索引
				if( j < nLen )
				{ 
				    wTempLen = api_GetProOADLen( pData->Tag, (BYTE *)&dwOad, 0 );
                    if( wTempLen == 0x8000 )
                    {
                        return 0x8000;
                    }

                    if( (Offset+ wTempLen) > pData->Len )//避免超限
                    {
                        return 0;   // 缓冲满
                    }

                    //元素索引合法性判断
                    if( (OadIndex>60) || (OadIndex==0) )
                    {
                        OadIndex = 1;   // 防止出错
                    }
                    TempOffset = 0;

                    //查找指定元素索引对应在存储OAD数据的偏移地址pTemp
                    for( k=1; k<OadIndex; k++ ) // pData->OadIndex 从1开始
                    {
                        dwTemp1 = k;
                        dwTempOad = ( (dwOad & (~0xFFE00000)) | (dwTemp1<<24) );
                        wTempLen = api_GetProOADLen( eData, (BYTE *)&dwTempOad, 0 );
                        if( wTempLen == 0x8000 )
                        {
                            return 0x8000;
                        }
                        TempOffset += wTempLen;
                    }

                    wOutLen = api_GetProOadData( eAddTag, 0xff, (BYTE *)&dwOad, pAllocBuf+pFree[j].Offset+TempOffset, 0xFF, pData->pBuf+Offset );
					if((dwOad & 0xFFFF) == 0x2221)
						Offset += wOutLen;
					else
						Offset += wOutLen;// wxy 20230411 wTempLen;
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
	if(( pData->pDLT698RecordPara->TimeOrLast == 0 )
	||( pData->pDLT698RecordPara->TimeOrLast > pData->pDataInfo->SaveDot ) )
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
	DWORD				dwNum,dwTempTime,dwTempDepth,dwBaseAddr,dwEndAddr,dwTimeAddr,dwStartTime,dwEndTime,dwRecordNo,TempPlanLen;
	TFreezeDataInfo		*pDataInfo;
	TSectorInfo			SectorInfo;
	TBinarySchInfo		BinarySchInfo;
	#if(PROTOCOL_VERSION == 25)
	WORD wFreezeCycle = 0;
	#endif
	TFreezePara* pFree;
	int nLen;
	DWORD dwFlashStart;
	pFree = GetFreezeType(GetFreezeTypeFromOI(pData->pDLT698RecordPara), &nLen, &dwFlashStart);
	
	if(pFree == NULL)
		return FALSE;

	pDataInfo = pData->pDataInfo;
	dwTempDepth = pFree[pData->MinAttOadIndex].Depth;
	TempPlanLen = pFree[pData->MinAttOadIndex].PlanDataLen;
	
	// 有数据的情况
	if( pDataInfo->SaveDot != 0 )
	{
		//周期冻结类，所有的时标都是单调递增的，采用二分法查找
		// 数据基地址
		dwBaseAddr = dwFlashStart + pFree[pData->MinAttOadIndex].PlanOffset;

		if( (pData->ReadNextPoint == 0X55) && (pData->RecordNo+1 < pDataInfo->RecordNo) )
		{
			// 方法2先判断下一个点是否满足要求，不满足要求再使用二分法
			#if(PROTOCOL_VERSION == 25)
			api_GetFreezeCycle((BYTE*)&wFreezeCycle);
			dwNum = pData->pDLT698RecordPara->TimeIntervalPara.TI/wFreezeCycle;
			#elif(PROTOCOL_VERSION == 24)
			dwNum = pData->pDLT698RecordPara->TimeIntervalPara.TI/pFree[pData->MinAttOadIndex].Cycle;
			#endif
			dwNum = ((dwNum == 0) ? 1 : dwNum);
			//查找下个数据
			GetSectorInfo( TempPlanLen, dwTempDepth, (pData->RecordNo+dwNum), &SectorInfo );
			dwTimeAddr = dwBaseAddr + SectorInfo.dwAddr;
			//将冻结数据全部读出
			api_ReadMemRecordData( dwTimeAddr, TempPlanLen, (BYTE *)&pData->pFreeData[0] );
			memcpy((BYTE*)&dwStartTime,(BYTE*)&pData->pFreeData[0],sizeof(DWORD));
			if( pData->pDLT698RecordPara->TimeOrLast <= dwStartTime )
			{
				//若要查找目标时间小于等于下一个数据冻结时间，则将下一个数据返回	
				pData->RecordNo += dwNum;	// 下一个数据的序号
				memcpy((BYTE *)&(pData->SearchTime),(BYTE *)&dwStartTime,sizeof(DWORD));	// 返回找到的时间
				pData->ReadFlashFlag = 0x55;
				return TRUE;
			}
		}
		pData->ReadFlashFlag = 0x00;
		// 找结束时间 最近一个数据
		GetSectorInfo( TempPlanLen, dwTempDepth, pDataInfo->RecordNo-1, &SectorInfo );
		dwEndAddr = dwBaseAddr + SectorInfo.dwAddr;
		api_ReadMemRecordData( dwEndAddr, sizeof(DWORD), (BYTE *)&dwEndTime );	
		
		// 找起始时间 最开始一个数据	
		GetSectorInfo( TempPlanLen, dwTempDepth, (pDataInfo->RecordNo-pDataInfo->SaveDot), &SectorInfo );
		dwTimeAddr = dwBaseAddr + SectorInfo.dwAddr;
		api_ReadMemRecordData( dwTimeAddr, sizeof(DWORD), (BYTE *)&dwStartTime );

		dwTempTime = 0;

		if( pData->pDLT698RecordPara->TimeOrLast <= dwStartTime )
		{
			//若要找的时间小于起始时间，则将第一个数据返回	
			pData->RecordNo = (pDataInfo->RecordNo - pDataInfo->SaveDot);				// 第一个数据的序号
			memcpy((BYTE *)&(pData->SearchTime),(BYTE *)&dwStartTime,sizeof(DWORD));	// 返回找到的时间
			
		}
		else if( 	( pData->pDLT698RecordPara->TimeOrLast > dwStartTime ) 
				&& 	( pData->pDLT698RecordPara->TimeOrLast <= dwEndTime ) )
		{
			//若找的时间在起始时间与结束时间之间，则需要二分法查找
			BinarySchInfo.wLen = TempPlanLen;
			BinarySchInfo.dwBaseAddr = dwBaseAddr;
			BinarySchInfo.dwDepth = dwTempDepth;
			BinarySchInfo.dwRecordNo = pDataInfo->RecordNo;
			BinarySchInfo.dwSaveDot = pDataInfo->SaveDot;
			BinarySchInfo.Time = pData->pDLT698RecordPara->TimeOrLast;
			BinarySchInfo.pRecordNo = &dwRecordNo;
			BinarySchInfo.pTime = &dwTempTime;
			if( BinSearchByTime( &BinarySchInfo ) == TRUE )
			{
				pData->RecordNo = dwRecordNo;											// 找到的序号
				memcpy((BYTE *)&(pData->SearchTime), (BYTE*)&dwTempTime, sizeof(DWORD));	// 返回找到的时间
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
	DWORD dwStartRecordNoBak,dwEndRecordNoBak,dwAddr,dwTempTime;
	DWORD dwStartRecordNo,dwEndRecordNo,dwMidRecordNo;
	DWORD dwMidAddr;
	TSectorInfo	SectorInfo;

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

	//备份开始结束记录号
	dwStartRecordNoBak = dwStartRecordNo;
	dwEndRecordNoBak = dwEndRecordNo;
	
	//二分法查找
	while( dwStartRecordNo < dwEndRecordNo )
	{
		//计算中间记录号，并找到对应记录号数据地址
		dwMidRecordNo = dwStartRecordNo + (dwEndRecordNo-dwStartRecordNo)/2;	
		GetSectorInfo( pBinSchInfo->wLen, pBinSchInfo->dwDepth, dwMidRecordNo, &SectorInfo );
		dwMidAddr = pBinSchInfo->dwBaseAddr+SectorInfo.dwAddr;
		
		//读取此记录号对应时间
		api_ReadMemRecordData( dwMidAddr, sizeof(DWORD), (BYTE *)&dwTempTime );
		if( dwTempTime == pBinSchInfo->Time )
		{
			//返回此数据对应记录号及时标
			*pBinSchInfo->pRecordNo = dwMidRecordNo;
			*pBinSchInfo->pTime = dwTempTime;
			return TRUE;
		}
		else if( dwTempTime < pBinSchInfo->Time )
		{
			dwStartRecordNo = dwMidRecordNo + 1;
		}
		else
		{
			if( dwMidRecordNo == 0 )
            {
                dwStartRecordNo = 0;
                break;
            }
            else
            {  
                dwEndRecordNo = dwMidRecordNo - 1;
            }
		}
	}

	//若二分法没找到对应时间，则将现在起始记录号对应时间读出
	GetSectorInfo( pBinSchInfo->wLen, pBinSchInfo->dwDepth, dwStartRecordNo, &SectorInfo );
	dwMidAddr = pBinSchInfo->dwBaseAddr+SectorInfo.dwAddr;
	api_ReadMemRecordData( dwMidAddr, sizeof(DWORD), (BYTE *)&dwTempTime );
	
	//若此时间大于要搜索的时间，则返回此时间记录号及数据地址
	if( dwTempTime >= pBinSchInfo->Time )
	{
		*pBinSchInfo->pRecordNo = dwStartRecordNo;
		*pBinSchInfo->pTime = dwTempTime;
	}
	else
	{
		//现在的起始、结束记录号是否超限
		if( (dwStartRecordNo<dwStartRecordNoBak) || (dwStartRecordNo>dwEndRecordNoBak) )
		{
			//ASSERT( FALSE, 1 );
			return FALSE;		
		}

		//将下一个记录号及对应数据地址返回
		GetSectorInfo( pBinSchInfo->wLen, pBinSchInfo->dwDepth, (dwStartRecordNo+1), &SectorInfo );
		dwAddr = pBinSchInfo->dwBaseAddr+SectorInfo.dwAddr;
		*pBinSchInfo->pRecordNo = (dwStartRecordNo+1);
		api_ReadMemRecordData( dwAddr, sizeof(DWORD), (BYTE *)pBinSchInfo->pTime );
	}

	return TRUE;
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
	WORD wLen,ReturnLen,LoopNum;
	#if(PROTOCOL_VERSION == 25)
	WORD wFreezeCycle = 0;
	#endif
	DWORD i,dwStartRecordNo,dwEndRecordNo,dwStartTime,dwEndTime,dwAddr,tdw;
	TSectorInfo		SectorInfo;

	TFreezePara* pFree;
	int nLen;
	DWORD dwFlashStart;
	pFree = GetFreezeType(GetFreezeTypeFromOI(pData->pDLT698RecordPara), &nLen, &dwFlashStart);
	
	if(pFree == NULL)
		return 0x8000;

	//对周期性冻结，间隔不为0，且TI不为冻结周期时，置标志
	//当AllCycle标志为55时，不判断间隔
	if( pData->pDLT698RecordPara->TimeIntervalPara.AllCycle != 0x55 )
	{
		if( pData->pDLT698RecordPara->TimeIntervalPara.TI == 0 )
		{
			//不应该出现这种情况，当为0时AllCycle不应该为55
			return 0x8000;
		}
	}
		
	//若起始时间大于截止时间，返回错误
	if( pData->pDLT698RecordPara->TimeIntervalPara.StartTime >= pData->pDLT698RecordPara->TimeIntervalPara.EndTime )
	{
		return 0x9000;
	}
	
	//将周期性冻结的搜索的起始时间改为能被间隔时间整除的时间
	
	//下发时间区间间隔为0时 分钟间隔按照分钟冻结周期，其他周期冻结默认为1
	if( pData->pDLT698RecordPara->TimeIntervalPara.AllCycle == 0x55 )
	{
		#if(PROTOCOL_VERSION == 25)
		api_GetFreezeCycle((BYTE*)&wFreezeCycle);
		pData->pDLT698RecordPara->TimeIntervalPara.TI = wFreezeCycle;
		#elif(PROTOCOL_VERSION == 24)
		pData->pDLT698RecordPara->TimeIntervalPara.TI = pFree[pData->MinAttOadIndex].Cycle;
		#endif
	}
	
	//周期冻结为分钟 小时 日 月冻结
	tdw = 1;
	{
		//若搜索的时间不能被间隔时间的分钟数整除，则置为下一个能被间隔时间整除的时间
		tdw = pData->pDLT698RecordPara->TimeIntervalPara.TI;		
		if( (pData->pDLT698RecordPara->TimeIntervalPara.StartTime % tdw) != 0 )
		{
			pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime / tdw;
			pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime * tdw;
			pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime + tdw;
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
	//当不按照冻结的周期作为间隔抄读数据时，先按照时间找出recordno，且看找出的时间是否是要抄读的时间，若是，则返回数据，若不是，返回null
	GetSectorInfo(   pFree[pData->MinAttOadIndex].PlanDataLen, pFree[pData->MinAttOadIndex].Depth, dwEndRecordNo, &SectorInfo );
	dwAddr = dwFlashStart + pFree[pData->MinAttOadIndex].PlanOffset + SectorInfo.dwAddr;
	
	api_ReadMemRecordData( dwAddr, sizeof(DWORD), (BYTE*)&dwEndTime );
		
	LoopNum = 0;
	i = dwStartTime;
	pData->pFreeData = api_AllocBuf((WORD*)&pFree[pData->MinAttOadIndex].PlanDataLen);
	while(i<=dwEndTime)
	{
		//防止循环出不来
		LoopNum++;
		if(LoopNum > 500)
		{
			break;
		}
		pData->pDLT698RecordPara->TimeOrLast = i;	
		
		if(SearchFreezeRecordNoByTime(pData) == FALSE)
		{
			break;
		}

		if( pData->SearchTime == i )//查询到数据
		{
			wLen = ReadFreezeByRecordNo(pData);	
			
			if( wLen == 0x8000 )
			{
				ReturnLen = 0;
				break;
			}
			else
			{
				//第一条数据在下发时已经判断空间大小
				pData->pBuf += wLen;
				ReturnLen += wLen;
				pData->Len -= wLen;
				pData->pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum++;
				//判断剩余的空间是否可容纳下一条数据@@@@
				if( pData->Len < wLen )
				{
					break;
				}
			}
			i = pData->SearchTime + pData->pDLT698RecordPara->TimeIntervalPara.TI;
			pData->ReadNextPoint = 0X55;
		}
		else
		{
			//按时间范围查找，若没有数据则跳过
			if( pData->SearchTime > i )
			{
				tdw = pData->pDLT698RecordPara->TimeIntervalPara.TI;		
				if( (pData->SearchTime % tdw) != 0 )
				{
					pData->SearchTime = pData->SearchTime / tdw;
					pData->SearchTime = pData->SearchTime * tdw;
					pData->SearchTime = pData->SearchTime + tdw;
				}
				i = pData->SearchTime;
				pData->ReadNextPoint = 0X00;
			}
			else
			{
				 break;
			}
		}	
	}		

	api_FreeBuf(pData->pFreeData);
	
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
	BYTE i,j,k,Plan;
	DWORD dwOad,dwTempOad;
	TFreezeDataInfo	DataInfo;
	TFreezeData 	Data;
	
	TFreezePara* pFree;
	int nLen;
	DWORD dwFlashStart;

	pFree = GetFreezeType(GetFreezeTypeFromOI(pDLT698RecordPara), &nLen, &dwFlashStart);
	
	if(pFree == NULL)
		return 0x8000;

	if( pDLT698RecordPara->OI != 0X5004 )
	{
        //wxy return 0x8000;
	}
		
	//分钟冻结下发的RCSD不能为0
	if( pDLT698RecordPara->OADNum == 0 )
	{
		return 0x9000;
	}
	//计量芯获取分钟冻结必须带Tag
	if((Tag != eTagData)&&(Tag != eMaxData))
	{
		return 0x9000;
	}
	//判断下发的OAD对应的周期深度方案号是否一致 需要判断元素索引为0，要读的不是0的情况
	Plan  = 0xff;
	k = 0xff;
	
	for(i=0; i<pDLT698RecordPara->OADNum; i++)
	{
		memcpy( (BYTE*)&dwOad, pDLT698RecordPara->pOAD+FREEZE_OAD_LEN*i, FREEZE_OAD_LEN );
		if( ((dwOad&(~0x00E00000))==FREEZE_RECORD_NO_OAD) 
			|| ((dwOad&(~0x00E00000))==FREEZE_RECORD_TIME_OAD) )
		{
			continue;
		}	
		dwTempOad = dwOad & (~0xFF000000);
		
		for(j=0; j<nLen; j++)
		{
			if( dwOad == pFree[j].Oad )
			{
				break;
			}
			if( dwTempOad == pFree[j].Oad )
			{
				break;
			}
		}
		
		if( j != nLen )
		{
			if( Plan == 0xff )
			{
				//第一个OAD的数据
				Plan  = pFree[j].Plan;
				k = j;
			}
			else
			{
				if( pFree[j].Plan!=Plan )
				{
					return 0x9000;
				}
			}
		}
		else
		{
			//不支持的OAD，返回null
		}
	}

	//未找到对应的关联对象属性表进行赋初值	
	if( (Plan == 0xff) || (k == 0xff) )
	{
		k = 0;
		Plan = GetFreezeTypeFromOI(pDLT698RecordPara);
	}
	
	//查找分钟冻结下发的对象对应的RecordNo		
	memcpy( (BYTE *)&DataInfo, (BYTE *)&MinInfo[Plan], sizeof(TFreezeDataInfo) );
	
	//还没有产生冻结
	if( DataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}

	Data.MinAttOadIndex = k;	//只对分钟冻结有用
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pDataInfo = &DataInfo;
	Data.Tag = Tag;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.ReadFlashFlag = 0x00;
	Data.ReadNextPoint = 0X00;
	Data.pFreeData = NULL;
	//方法1，9数据空间大小及方法2的第1条数据空间大小判断
	Len = GetFreezeAttDataLen( eSELECTED_OAD_LEN, &Data );
	if(( Len > Data.Len )||( Len == Data.Len))
	{
		return 0x8000;// 空间不足
	}
	
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
//返回值:	
//  无
//
//备注:
//-----------------------------------------------


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
	BYTE i;
	
	#if(PROTOCOL_VERSION == 25)
	WORD wFreezeCycle = 0;

	api_GetFreezeCycle((BYTE*)&wFreezeCycle);

	set_freeze_period(wFreezeCycle);
	#endif
	
	for( i = 0; i < (sizeof(MinInfo)/sizeof(TFreezeDataInfo)); i++ )
	{
		// 检查RAM的校验
		if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == TRUE )
		{
			continue;
		}

		// 从EEP恢复
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] );

		// 检查EEP的校验
		if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == TRUE )
		{
			continue;
		}
		else
		{
			memset( (BYTE *)&MinInfo[i], 0x00, sizeof(TFreezeDataInfo) );
			MinInfo[i].CRC32 = lib_CheckCRC32((BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo)-sizeof(DWORD));

			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] );
		}
	}
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
    api_ClrFreezeData();
}

//-------------------------------------------------------------------
//	冻结功能
//	1:分钟冻结(关联属性对象表固化)
//	2:上电，改时间补冻
//	3:冻结读取
//	
//	上电：
//		获取冻结指针,补冻
//
//	任务：
//		分钟任务：分钟冻结
//		小时任务：保存冻结指针
//
//	掉电：
//		保存冻结指针
//
//	其他：
//		规约读取函数
//------------------------------------------------------------------

void api_ReadHisLastTime(void)
{
	DWORD dwAddr,dwTemp1;
	TSectorInfo	SectorInfo;
	TRealTimer 	Time,RealTimer1;
	BYTE *pAllocBuf;
	TFreezePara* pFree;
	int nLen;
	DWORD dwFlashStart;
	TFreezeDataInfo	DataInfo;


	pFree = (TFreezePara*)GetFreezeType(0, &nLen, &dwFlashStart);
	
	if(pFree == NULL)
		return ;

	//查找分钟冻结下发的对象对应的RecordNo		
	memcpy( (BYTE *)&DataInfo, (BYTE *)&MinInfo[0], sizeof(TFreezeDataInfo) );
	
	//还没有产生冻结
	if( DataInfo.RecordNo == 0 )
	{
		return ;
	}
	//冻结数据从FLASH中获取
	dwAddr = dwFlashStart + pFree[0].PlanOffset;//基地址+此方案的偏移地址
	GetSectorInfo(  pFree[0].PlanDataLen,  pFree[0].Depth, DataInfo.RecordNo-1, &SectorInfo );
	dwAddr += SectorInfo.dwAddr;//+此条数据在方案中的偏移地址
	//读取此条冻结数据
	pAllocBuf = (BYTE *)api_AllocBuf( (WORD*)&pFree[0].PlanDataLen );
	if(api_ReadMemRecordData( dwAddr, pFree[0].PlanDataLen, pAllocBuf ) == FALSE)
	{
		return ;
	}

	memcpy( (BYTE *)&dwTemp1, (BYTE *)&pAllocBuf[0], sizeof(DWORD) );
	api_ToAbsTime( dwTemp1, (TRealTimer *)&Time );


	if(api_CheckClock(&Time))
	{
    	api_ReadMeterTime(&RealTimer1);
                
		if(api_CompareTwoTime(&Time, &RealTimer1) > 0)
		{
			api_WriteMeterTime(&Time);
		}
	}
}