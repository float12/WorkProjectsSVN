//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.8.27
//描述		冻结模块Api头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __FREEZE_API_H
#define __FREEZE_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//分钟冻结支持的方案数
#define MAX_FREEZE_PLAN						(1)

//分钟冻结点数
#define FREEZE_MIN_DEPTH1					(2880)//4*24*30   15分钟存一次，一共30天的数据

//日冻结点数
#define FREEZE_DAY_DEPTH					(30)//(64)//60*24*365

#define FREEZE_MON_DEPTH					(12)//(64)//60*24*365

#define FREEZE_CYCLE_LEN					(sizeof(WORD))			// 冻结周期
#define FREEZE_OAD_LEN						(sizeof(DWORD))			// OAD
#define FREEZE_DEPTH_LEN					(sizeof(WORD))			// 存储深度
#define FREEZE_ATTRIBUTE_LEN				(sizeof(WORD)+sizeof(DWORD)+sizeof(WORD))			// 冻结周期+OAD+存储深度

#define MAX_MIN_FREEZE_DATA_LEN				(300UL*SECTOR_SIZE)

#define NILM_EQUIPMENT_MAX_NO				30			// 一次最多识别，存电器的个数,冻结一次可识别最大数目
#define NILM_EQUIPMENT_TYPE_NO				20			// 可识别的电器类型数量，48000200
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//分钟冻结占用空间最多，放在最后面
typedef enum
{
	eDELETE_ATT_BY_OAD,	// 删除指定OAD
	eDELETE_ATT_ALL		// 按照OI删除所有OAD
}eDELETE_ATT_TYPE;

typedef enum
{
	ePOWERUP_FREEZE_MODE,		// 上电补冻
	eMAINTASK_FREEZE_MODE,		// 主循环中正常执行冻结
	eADJUST_TIME_MODE			// 向未来设置时间
}eFreezeMode;

typedef struct TFreezeDataInfo_t
{
	DWORD	RecordNo;						//记录序号(单调递增)	
	DWORD	SaveDot;
	DWORD	CRC32;
}TFreezeDataInfo;

// 指针
typedef struct TFreezeInfoRom_t
{
	TFreezeDataInfo	MinDataInfo[MAX_FREEZE_PLAN];	//分钟冻结，每个方案有一个RecordNo
	DWORD			CRC32;							//给掉电保存分钟冻结指针用，一次写入
}TFreezeInfoRom;


typedef struct TFreezeUpdateTimeFlag_t
{
	DWORD	Flag;	//修改时间补冻标志,每个bit代表一个方案
	DWORD	CRC32;
}TFreezeUpdateTimeFlag;
#if(PROTOCOL_VERSION == 25)	
#pragma pack(1)
typedef struct TFreezeCycleSafeRom_t
{
	WORD		Cycle;				// 冻结周期
	DWORD		CRC32;
}TFreezeCycleSafeRom;
#pragma pack()
#endif
typedef struct TFreezeSafe_t
{
	TFreezeInfoRom			FreezeInfoRom;						//指针信息
	TFreezeUpdateTimeFlag	FreezeUpdateTimeFlag;				//改时间标志
	#if(PROTOCOL_VERSION == 25)	
	TFreezeCycleSafeRom		FreezeCycleInfo;					//冻结周期等信息
	#endif
}TFreezeSafeRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern WORD SaveFreeze;//存冻结的标志
extern WORD GetFreeze;//取冻结的标志
#if(PROTOCOL_VERSION == 25)	
extern WORD SaveEvent;//存事件的标志
extern const WORD g_Default_Freeze[4];
#endif
extern T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 读冻结关联对象属性
//
//参数: 
//	Type[in]:	0x5000~0x500B,0x5011 冻结类的对象标识OI
//                 
//  No[in]:	0   读所有方案的所有OAD	
//			1~N 从8个方案里读第No个OAD	
//                
//  Len[in]:规约层传来数据长度，超过此长度返回FALSE	
//
//  pBuf[out]: 返回数据的指针
//				按照冻结周期，OAD，存储深度顺序返回，每个DWORD，总共3*4字节
//
//  返回值:	
//    值为0x8000 ：代表出现错误（ OI不支持，读取Epprom错误等问题）
//    值为0x0000 ：代表buf长度不够
//    其他:     正常数据长度( 在个数为0时，添加1字节的0 返回长度为1)
//
//备注: 
//-----------------------------------------------
WORD api_ReadFreezeAttribute( WORD Type, BYTE No, WORD Len, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 清冻结数据
//
//参数: 
//
//                    
//返回值:	TRUE   正确
//			FALSE  错误设置
//
//备注:   
//-----------------------------------------------
void api_ClrFreezeData( void );

//-----------------------------------------------
//函数功能: 冻结任务
//
//参数: 
//	无
//                    
//返回值:
//	无
//
//备注:   大循环调用
//-----------------------------------------------
void api_FreezeTask( void );

//-----------------------------------------------
//函数功能: 读冻结记录
//
//参数: 
//	Tag[in]:	0 不加Tag 
//				1 加Tag
//                
//	pDLT698RecordPara[in]: 读取冻结的参数结构体
//                
//  Len[in]：输入的buf长度 用来判断buf长度是否够用
//                
//  pBuf[out]: 返回数据的指针，有的传数据，没有的补0
//				
//	
//  返回值:	bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//			bit0~bit14 代表返回的冻结数据长度
//
//备注:	
//-----------------------------------------------
WORD api_ReadFreezeRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 读取最后一条冻结时间
//
//参数:            
//                  
//返回值:	相对分钟数,全F表示数据无效
//备注:	RTC上电时调用
//-----------------------------------------------
DWORD api_ReadLastFreezeTime( void );

//-----------------------------------------------
//函数功能: 冻结上电处理
//
//参数: 
//	无
//	
//  返回值:	
//  无
//
//备注:	 
//-----------------------------------------------
void api_PowerUpFreeze( void );

//-----------------------------------------------
//函数功能: 冻结掉电处理
//
//参数: 
//	无
//	
//  返回值:	
//  无
//
//备注:	 
//-----------------------------------------------
void api_PowerDownFreeze( void );

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
void api_FactoryInitFreeze( void );
//-----------------------------------------------
//函数功能: 处理分钟冻结任务
//
//参数: 	FreezeTime[in]:		当前时间
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
void ProcMinFreezeTask(BYTE byType, DWORD TmpRelativeMin, eFreezeMode FreezeMode );

//-----------------------------------------------
//函数功能: 更新冻结判断用的时间,并更新RecordNo
//
//参数: InOldRelativeMin[in]:修改时间之前的相对时间
//                    
//返回值:
//	无
//
//备注:只处理分钟冻结
//-----------------------------------------------
void api_UpdateFreezeTime( DWORD InOldRelativeMin );

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
void api_ReadHisLastTime(void);
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
void api_GetFreezeCycle(BYTE *pBuf );
//--------------------------------------------------
//函数功能: 写冻结关联对象属性
//
//参数: 
//	OI[in]:				0x5000~0x500B,0x5011 冻结类的对象标识OI,目前支持5002，后续其他oad需要调试
//  Len[in]:			规约层传来数据长度，超过此长度返回FALSE	
//  pBuf[out]: 			返回数据缓冲    
//
//备注: 正常数据长度( 在个数为0时，添加1字节的0 返回长度为1)
//--------------------------------------------------
BOOL  api_WriteFreezeAttribute( WORD OI, BYTE *pBuf );
#endif //#if(PROTOCOL_VERSION == 25)	
#endif//__FREEZE_API_H
