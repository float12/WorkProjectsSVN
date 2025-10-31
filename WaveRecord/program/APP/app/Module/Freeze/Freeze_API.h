//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.8.27
//描述		冻结模块Api头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __FREEZE_API_H
#define __FREEZE_API_H

////存储空间分配宏定义
//#define COLLECT_SAVE_START_SECTORS				2048		//开始存储采集数据区域
//#define COLLECT_SAVE_SECTORS_NUM				( 8 * 1024 - COLLECT_SAVE_START_SECTORS )	//可用存储扇区数
//#define COLLECT_SAVE_REGION_NUM					48			//分为多少份
//#define COLLECT_ONE_REGION_SIZEOF				( (COLLECT_SAVE_SECTORS_NUM / COLLECT_SAVE_REGION_NUM ) + 1)	//每份存储空间大小		
//
////方案及任务配置
//#define COLLECT_SCHEME_NUM						12			//采集方案数
//
//#pragma pack(1)
////采集存储信息结构体
//typedef struct TCollectSaveInfo_t   	
//{
//	WORD wOneRecordLen;										//一条记录最大长度
//	WORD dwRecordNum[SEARCH_METER_MAX_NUM];				//一块表记录条数
//	BYTE bMeterNo[6][SEARCH_METER_MAX_NUM];				//该方案对应的表
//	BYTE bMeterNum;											//该方案表数
//	BYTE dwSaveLump[COLLECT_ONE_REGION_SIZEOF];			//存储块，一个BIT位一个块，最多可以分
//	DWORD dwCRC;											//校验
//}TCollectSaveInfo;
//#pragma pack()
//
////每次确认好哪个方案需要操作时，将存储信息获取出来，进行数据存储处理
//TCollectSaveInfo CollectSaveInfo;

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//配置第三片存储器时，冻结数据的基地址
#define FLASH_FREEZE_BASE					(FLASH_EVENT_BASE+SUB_EVENT_INDEX_MAX_NUM*EVENT_RECORD_LEN + EVENT_RECORD_EXT_LEN)

//扇区分配
#if( MAX_PHASE == 1)
#define FLASH_SECTOR_NUM					(6)		//除日、月、分钟外冻结扇区数
#define MIN_FLASH_SECTOR_NUM				(600)	//分钟冻结扇区数
#define MINBAK_FLASH_SECTOR_NUM				(100)	//分钟冻结扇区备份
#define DAY_FLASH_SECTOR_NUM				(30)	//日冻结扇区数
#define MON_FLASH_SECTOR_NUM				(6)		//月冻结扇区数

#else
//除分钟冻结和日冻结外外每种冻结占用几个扇区
#define FLASH_SECTOR_NUM					(6)		//除日、月、分钟外冻结扇区数
#define MIN_FLASH_SECTOR_NUM				(1200)	//分钟冻结扇区数
#define MINBAK_FLASH_SECTOR_NUM				(80)	//分钟冻结扇区备份
#define DAY_FLASH_SECTOR_NUM				(150)	//日冻结扇区数
#define MON_FLASH_SECTOR_NUM				(20)	//月冻结扇区数
#endif


//分钟冻结点数
#if( MAX_PHASE == 1)
#define MAX_FREEZE_MIN_DEPTH				35040
#else
#define MAX_FREEZE_MIN_DEPTH				35040
#endif

#if( MAX_PHASE == 1)
#define MAX_FREEZE_ATTRIBUTE				10// 所有冻结中支持的属性的最大数
#else
#define MAX_FREEZE_ATTRIBUTE				32	
#endif
#define MAX_FREEZE_PLANNUM					8// 所有冻结中支持的方案的最大数、只有分钟冻结判断


// 瞬时
#if( MAX_PHASE == 1)
#define MAX_INSTANT_FREEZE_ATTRIBUTE		(3+2)// 默认 正反向有功电能，有功功率 3个
#else
#define MAX_INSTANT_FREEZE_ATTRIBUTE		(12+4+4)// 默认 电能*8个，最大需量及发生时间*2，有功功率无功功率	一共12个//不够用加到20个
#endif																		
#define MAX_INSTANT_FREEZE_DATA_LEN			(FLASH_SECTOR_NUM*SECTOR_SIZE) 	//((4+20*8+55*2+16*2)*3+105*3)


// 分钟
#if( MAX_PHASE == 1)
#define MAX_MINUTE_FREEZE_ATTRIBUTE			(7+3)
#define MAX_MIN_FREEZE_DATA_LEN				(MIN_FLASH_SECTOR_NUM*SECTOR_SIZE)	//@@@@@@按1分钟20天考虑			// (4字节Time+4字节数据)*288*6+ (4+4)*288*2+ (4+2*3)*288+(4+4*4)*288+(4+4*4)*288+ (4+2*4)*288
#else
#define MAX_MINUTE_FREEZE_ATTRIBUTE			(17+6)// 默认 正反向有功,1,2,3,4象限无功总电能，有功需量，无功需量，电压，电流，      有功功率，功率因数 12个	
#define MAX_MIN_FREEZE_DATA_LEN				(MIN_FLASH_SECTOR_NUM*SECTOR_SIZE)	//@@@@@@按1分钟20天考虑			// (4字节Time+4字节数据)*288*6+ (4+4)*288*2+ (4+2*3)*288+(4+4*4)*288+(4+4*4)*288+ (4+2*4)*288
#endif	
#define MAX_MINBAK_FREEZE_DATA_LEN			(MINBAK_FLASH_SECTOR_NUM*SECTOR_SIZE)													


// 小时
#define MAX_HOUR_FREEZE_ATTRIBUTE			(2+1)	// 默认 正反向有功总电能 2个
#define MAX_HOUR_FREEZE_DATA_LEN			(FLASH_SECTOR_NUM*SECTOR_SIZE)// (4字节Time+4字节数据)*254*2

// 日
#if( MAX_PHASE == 1)
	#if( PREPAY_MODE == PREPAY_LOCAL )
	#define MAX_DAY_FREEZE_ATTRIBUTE		(5+2)	// 默认 正反向有功总，尖峰平谷电能,有功功率总+A 3个
	#define MAX_DAY_FREEZE_E2_ONCE_DATA_LEN	300//(4L+65*2+8+8+4+4)现有长度158，E2空间按照300个字节开
	#else
	#define MAX_DAY_FREEZE_ATTRIBUTE		(3+2)	// 默认 正反向有功总，尖峰平谷电能,有功功率总+A 3个
	#define MAX_DAY_FREEZE_E2_ONCE_DATA_LEN	300//(4L+65*2+8+8+4+4)现有长度158，E2空间按照300个字节开
	#endif
	//日冻结空间不足扇区更改为4个----李丕凯
	#define MAX_DAY_FREEZE_DATA_LEN			(DAY_FLASH_SECTOR_NUM*SECTOR_SIZE)// (4字节Time+20字节数据)*62*8+       (4+11*5)*62*2+      (4+4*4)*2*62+     (4+16)*3*62
#else
	#define MAX_DAY_FREEZE_ATTRIBUTE		(1)	// 默认 正反向有功,组合无功1,2,第1,2,3,4象限无功总，尖峰平谷电能,正反向有功最大需量及发生时间；有功无功功率,ABC电压合格率、余金额还没加
	//时间4字节、总电能4字节、12费率电能52字节、12费率143字节、功率16字节、电压合格率16字节、剩余金额8字节、透支金额4字节，预留240字节，CRC校验4字节
	#define MAX_DAY_FREEZE_E2_ONCE_DATA_LEN	1000//(4L+4*6+52*8+2*143+16*2+16*3+8+4+4)现有长度936，E2空间按照1300个字节开
	//日冻结空间不足扇区更改为23个----李丕凯
	#define MAX_DAY_FREEZE_DATA_LEN			(DAY_FLASH_SECTOR_NUM*SECTOR_SIZE)// (4字节Time+20字节数据)*62*8+       (4+11*5)*62*2+      (4+4*4)*2*62+     (4+16)*3*62
#endif
#define MAX_DAY_FREEZE_E2_DEPTH				2													//E2备份的日冻结数据深度
#define MAX_DAY_FREEZE_E2_DATA_LEN			(MAX_DAY_FREEZE_E2_ONCE_DATA_LEN*MAX_DAY_FREEZE_E2_DEPTH)

// 月
#if( MAX_PHASE == 1)
#define MAX_MON_FREEZE_ATTRIBUTE			(3+2)	// 默认 组合有功，正反向有功总，尖峰平谷电能，月度用电量(组合有功总) 4个
#else
#define MAX_MON_FREEZE_ATTRIBUTE			(20+7)	// 默认 35个电能，  正向反向有功最大需量及发生时间  ，月度用电量，总ABC电压合格率  42个
#endif
#define MAX_MON_FREEZE_DATA_LEN				(MON_FLASH_SECTOR_NUM*SECTOR_SIZE)	// (4+20)*12*35 + (4+55)*12*2 + (4+16)*12*3 + (4+4)*12*1= 

// 时区表切换
#if( MAX_PHASE == 1)
#define MAX_TIME_ZONE_FREEZE_ATTRIBUTE		(3+2)// 默认 正反向有功总，尖峰平谷电能，有功功率 3个
#else
#define MAX_TIME_ZONE_FREEZE_ATTRIBUTE		(12+4+4)// 默认 电能8个	最大需量以及发生时间2个	有功/无功功率2个	   一共12个	
#endif
#define MAX_TIME_ZONE_FREEZE_DATA_LEN		(FLASH_SECTOR_NUM*SECTOR_SIZE)					// (4字节Time+20字节数据)*8*2+(4+55)*2+(4+4*4)*2 

// 日时段表切换
#if( MAX_PHASE == 1)
#define MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE	(3+2)	// 默认 正反向有功总，尖峰平谷电能，有功功率 3个
#else
#define MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE	(12+4+4)	// 默认 电能8个	最大需量以及发生时间2个	有功/无功功率2个	   一共12个
#endif
#define MAX_DAY_TIMETABLE_FREEZE_DATA_LEN	(FLASH_SECTOR_NUM*SECTOR_SIZE)// (4字节Time+20字节数据)*8*2+(4+11*5)*2*2+(4+4*4)*2*2

// 结算日冻结
#if( MAX_PHASE == 1)
#define MAX_CLOSING_FREEZE_ATTRIBUTE		(4+2)	// 默认 正反向有功总，尖峰平谷电能 2个，月度用电量
#define MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN	300//(4L+65*3+8+4)现有长度207，E2空间按照300个字节开

#else
#define MAX_CLOSING_FREEZE_ATTRIBUTE		(15+5)	// 
//时间4字节、总电能4字节、12费率电能52字节、12费率需量143字节、月度用电量4字节，预留240字节，CRC校验4字节
#define MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN	1000 //(4L+4*3+52*9+143*2+4+240+4)现有长度898，E2空间按照1300个字节开

#endif
#define MAX_CLOSING_FREEZE_DATA_LEN			(FLASH_SECTOR_NUM*SECTOR_SIZE)						// (4字节Time+20字节数据)*12*2= 576 字节
#define MAX_CLOSING_FREEZE_E2_DEPTH			2												//E2备份的结算日冻结数据深度
#define MAX_CLOSING_FREEZE_E2_DATA_LEN		(MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN*MAX_CLOSING_FREEZE_E2_DEPTH)

#if( PREPAY_MODE == PREPAY_LOCAL )
// 费率电价表切换
#if( MAX_PHASE == 1)
#define MAX_TARIFF_RATE_FREEZE_ATTRIBUTE	(3+2)	// 默认 正反向有功总，尖峰平谷电能，有功功率 3个
#else
#define MAX_TARIFF_RATE_FREEZE_ATTRIBUTE	(12+4)	// 默认 电能8个	最大需量以及发生时间2个	有功/无功功率2个	   一共12个
#endif
#define MAX_TARIFF_RATE_FREEZE_DATA_LEN		(FLASH_SECTOR_NUM*SECTOR_SIZE)	// (4字节Time+20字节数据)*8*2+(4+55)*2+(4+4*4)*2 


// 阶梯电价表切换
#if( MAX_PHASE == 1)
#define MAX_LADDER_FREEZE_ATTRIBUTE			(3+2)	// 默认 正反向有功总，尖峰平谷电能，有功功率 3个
#else
#define MAX_LADDER_FREEZE_ATTRIBUTE			(12+4)	// 默认 电能8个	最大需量以及发生时间2个	有功/无功功率2个	   一共12个
#endif
#define MAX_LADDER_FREEZE_DATA_LEN			(FLASH_SECTOR_NUM*SECTOR_SIZE) // (4字节Time+20字节数据)*8*2+(4+55)*2+(4+4*4)*2 


// 阶梯结算冻结
#if( MAX_PHASE == 1)
#define MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE	(1+1)	// 默认 正反向有功总，尖峰平谷电能，有功功率 3个
#else
#define MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE	(1+1)	// 默认 电能8个	最大需量以及发生时间2个	有功/无功功率2个	   一共12个
#endif
#define MAX_LADDER_CLOSING_FREEZE_DATA_LEN	(FLASH_SECTOR_NUM*SECTOR_SIZE)	// (4字节Time+20字节数据)*8*2+(4+55)*2+(4+4*4)*2 
#endif
#if( MAX_INSTANT_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_MINUTE_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_HOUR_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_DAY_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_MON_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_TIME_ZONE_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_CLOSING_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_TARIFF_RATE_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_LADDER_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
配置错误
#endif

#if( MAX_FREEZE_ATTRIBUTE > 34 )
配置错误--安全空间长度限制为350字节--MAX_FREEZE_ATTRIBUTE*sizeof(TFreezeAttOad);
#endif


//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//分钟冻结占用空间最多，放在最后面
//这张表的顺序要和FreezeInfoTab里面定义的顺序一致
typedef enum
{
	//非周期冻结类 eFREEZE_CLOSING固定为第一个
	eFREEZE_CLOSING,			//结算日冻结
	#if( PREPAY_MODE == PREPAY_LOCAL )
	eFREEZE_TARIFF_RATE_CHG,	//费率电价切换
	eFREEZE_LADDER_CHG,		//阶梯切换 0X0B
	eFREEZE_LADDER_CLOSING,	//阶梯结算//阶梯切换冻结与阶梯结算冻结顺序不能修改
	#endif
	eFREEZE_TIME_ZONE_CHG,		//时区表切换
	eFREEZE_DAY_TIMETABLE_CHG,	//日时段表切换	
	eFREEZE_INSTANT,			//瞬时冻结		
	
	//周期冻结类 eFREEZE_HOUR固定为周期冻结的第一个，eFREEZE_MIN固定为最后一个
	eFREEZE_HOUR,				//小时冻结
	eFREEZE_MON,				//月冻结	
	eFREEZE_DAY,				//日冻结	
	eFREEZE_MIN,				//分钟冻结(放在最后面)
	eFREEZE_MAX_NUM				//冻结的最大种类数量          置1代表第一结算日结算
}eFreezeType;

typedef enum
{
	eDELETE_ATT_BY_OAD,	// 删除指定OAD
	eDELETE_ATT_ALL		// 按照OI删除所有OAD
}eDELETE_ATT_TYPE;

#pragma pack(1)
typedef struct  TFreezePara_t
{
    WORD Cycle;
    DWORD OAD;
    WORD Depth;
}TFreezePara;

typedef struct TFreezeAttOad_t
{
	DWORD	Oad;			// OAD  属性标识特征的bit5~bit7有效
	WORD	AttDataLen;		// 此属性对应一个数据的长度,分钟冻结包含时标长度
	DWORD	Offset;			// 对分钟冻结，此属性数据为对应方案记录中的数据偏移
							// 对于其他冻结，此属性数据在整条冻结里面的偏移地址（包含时标）
}TFreezeAttOad;

#pragma pack()

typedef struct TFreezeAttCycleDepth_t
{
	WORD	Cycle;			// 冻结周期 698 long unsigned 为16bit
	WORD	Depth;			// 存储深度 规定深度
	DWORD	CRC32;
}TFreezeAttCycleDepth;

typedef struct TFreezeDataInfo_t
{
	DWORD	RecordNo;					//记录序号(单调递增)	
	DWORD	SaveDot;					//冻结数据剩余的冻结个数，修改时间时，在UpdataFreezeTime函数需要修改
	DWORD	CRC32;
}TFreezeDataInfo;

typedef struct TFreezeAllInfoRom_t
{
	BYTE	NumofOad;			//OAD的个数
	BYTE	Rsv;
	WORD	AllAttDataLen;		//除分钟冻结
	DWORD	CRC32;
}TFreezeAllInfoRom;

typedef struct TFreezeMinInfo_t
{
	DWORD	DataAddr[MAX_FREEZE_PLANNUM];		//分钟冻结 主数据区
	DWORD	DataBakAddr[MAX_FREEZE_PLANNUM];	//分钟冻结 数据备份区
	WORD	AllAttDataLen[MAX_FREEZE_PLANNUM];
	DWORD	CRC32;
}TFreezeMinInfo;

typedef struct TFreezeUpdateTimeFlag_t
{
	WORD 	wFreezeFlag;	//修改时间时日月冻结补冻标志
	WORD 	wMinFlag;		//修改时间分钟冻结各属性补冻标志
	DWORD	CRC32;
}TFreezeUpdateTimeFlag;

// 属性OAD信息
typedef struct TFreezeAttCycleDepthRom_t
{
	TFreezeAttCycleDepth	Freeze[eFREEZE_MIN];		//除分钟冻结外，每种冻结只有一个
	TFreezeAttCycleDepth	Min[MAX_FREEZE_PLANNUM];	//分钟冻结每个属性都有一个
	TFreezeAttCycleDepth	MinBak[MAX_FREEZE_PLANNUM];	//Min与MinBak中间不要插入数据！！！！
}TFreezeAttCycleDepthRom;

//保存在安全空间里面的关联属性信息，最后8个字节是放CRC的，为避免对齐问题，所以开了8个字节。
//在存储时，不一定是按照最大个数保存，因此crc放在紧跟数据后面，不一定在后8个字节里面
typedef struct TFreezeAttOadRom_t
{
	BYTE		Closing[sizeof(TFreezeAttOad)*MAX_CLOSING_FREEZE_ATTRIBUTE+8];	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	BYTE		TariffRateChg[sizeof(TFreezeAttOad)*MAX_TIME_ZONE_FREEZE_ATTRIBUTE+8];	
	BYTE		LadderChg[sizeof(TFreezeAttOad)*MAX_TIME_ZONE_FREEZE_ATTRIBUTE+8];	
	BYTE		LadderClosing[sizeof(TFreezeAttOad)*MAX_TIME_ZONE_FREEZE_ATTRIBUTE+8];	
	#endif	
	BYTE		TimeZone[sizeof(TFreezeAttOad)*MAX_TIME_ZONE_FREEZE_ATTRIBUTE+8];	
	BYTE		DayTimeTab[sizeof(TFreezeAttOad)*MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE+8];	
	BYTE		Instant[sizeof(TFreezeAttOad)*MAX_INSTANT_FREEZE_ATTRIBUTE+8];		
	BYTE		Hour[sizeof(TFreezeAttOad)*MAX_HOUR_FREEZE_ATTRIBUTE+8];		
	BYTE		Mon[sizeof(TFreezeAttOad)*MAX_MON_FREEZE_ATTRIBUTE+8];	
	BYTE		Day[sizeof(TFreezeAttOad)*MAX_DAY_FREEZE_ATTRIBUTE+8];
	BYTE		Min[sizeof(TFreezeAttOad)*MAX_MINUTE_FREEZE_ATTRIBUTE+8];	
}TFreezeAttOadRom;

// 指针
typedef struct TFreezeInfoRom_t
{
	TFreezeDataInfo	DataInfo[eFREEZE_MIN];						//除分钟冻结外，每种冻结有一个RecordNo	
	TFreezeDataInfo	MinDataInfo[MAX_FREEZE_PLANNUM];			//分钟冻结，每个属性有一个RecordNo
	DWORD			CRC32;										//给掉电保存分钟冻结指针用，一次写入
}TFreezeInfoRom;

//地址映射结构体
//数据
typedef struct TFreezeEeData_t
{

	BYTE	Closing[MAX_CLOSING_FREEZE_E2_DATA_LEN];//结算日冻结备份数据E2存储空间	
	BYTE	Day[MAX_DAY_FREEZE_E2_DATA_LEN];//日冻结备份数据E2存储空间

}TFreezeEeData;


typedef struct TFreeze_t
{
	TFreezeEeData			EeData;		// 冻结数据
}TFreezeConRom;


typedef struct TFreezeSafe_t
{
	TFreezeInfoRom			FreezeInfoRom;						//指针信息
	TFreezeAllInfoRom		FreezeAllInfoRom[eFREEZE_MAX_NUM];	//整条信息
	TFreezeUpdateTimeFlag	FreezeUpdateTimeFlag;				//改时间标志
	TFreezeAttCycleDepthRom	AttCycleDepthRom;					//周期 深度
	TFreezeAttOadRom		AttOadRom;							// OAD信息
	TFreezeMinInfo			FreezeMinInfo;						//分钟冻结信息
}TFreezeSafeRom;

#define FLASH_FREEZE_END_ADDR (FLASH_FREEZE_BASE+(DWORD)eFREEZE_MON*FLASH_SECTOR_NUM*SECTOR_SIZE+MAX_MON_FREEZE_DATA_LEN+MAX_DAY_FREEZE_DATA_LEN+MAX_MIN_FREEZE_DATA_LEN+MAX_MINBAK_FREEZE_DATA_LEN)

// 三相Flash里存的数据
// FLASH_FREEZE_BASE开始，存放除了分钟冻结以外的其他冻结数据，每个OAD两个扇区，按照最大的OAD的个数开(SECTOR_SIZE*2*最大OAD个数)
// 分钟冻结放最后，按照每个OAD占用扇区数取整加1存放；(1180L*SECTOR_SIZE)
// 分钟冻结的后面放的是读上N次数据对应的时间信息:
// 由于一个扇区最多可以存放1024条数据对应的时间,除了分钟冻结，其他冻结一种冻结占用两个扇区
// 分钟冻结存28800个点，要占用[28800/1024]+1=30个扇区
//-----------------------------------------------
//				变量声明
//-----------------------------------------------


//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

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
BYTE api_GetFreezeIndex( WORD OI );

//-----------------------------------------------
//函数功能: 添加冻结关联对象属性
//
//参数: 
//	OI[in]:		0x5000~0x500B,0x5011 冻结类的对象标识OI  
//	pOad[in]:	指向OAD Buffer的指针  2字节冻结周期+4字节OAD+2字节存储深度                    
//  OadNum[in]: 批量添加OAD的个数
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:   添加删除所有数据
//-----------------------------------------------
BOOL api_AddFreezeAttribue( WORD OI, BYTE *pOad ,BYTE OadNum );

//-----------------------------------------------
//函数功能: 删除冻结关联对象属性
//
//参数: 
//	OI[in]:	0x5000~0x500B,0x5011 冻结类的对象标识OI
//  
//	Type[in]:	eDELETE_ATT_TYPE
//  
//	pOad[in]:	要删除的OAD,属性标识及特征的bit5~bit7代表各类冻结的冻结方案
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:   删掉一个OAD，所有的重新计，重新分配EEP空间
//-----------------------------------------------
BOOL api_DeleteFreezeAttribue( WORD OI, BYTE Type, BYTE *pOad );

//-----------------------------------------------
//函数功能: 设置冻结关联对象属性
//
//参数: 
//	FreezeOI[in]:	0x5000~0x500B,0x5011 冻结类的对象标识OI
//  
//  OadType[in]:
//			0 ~ 代表所有的OAD
//			N ~ 属性列表里的第N个OAD
//	
//  pOad[in]:  指向OAD Buffer的指针  2字节冻结周期+4字节OAD+2字节存储深度
//                
//  OadNum[in]:	OAD的个数
//
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注:	所有的重新计，重新分配EEP空间
//-----------------------------------------------
BOOL api_WriteFreezeAttribue( WORD FreezeOI, BYTE OadType, BYTE *pOad, WORD OadNum );

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
BOOL api_SetFreezeFlag( eFreezeType inFreezeIndex, WORD Delay );

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
//WORD api_ReadFreezeRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );

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

//-----------------------------------------------
//函数功能: 读结算记录645（电能结算、需量结算）
//
//参数:
//				DI[in]:	数据标识
//  			pOutBuf[out]: 返回数据
//
//返回值:		bit15位置1 代表冻结无此类型 置0代表数据能正确返回；
//				bit0~bit14 代表返回的冻结数据长度
//
//备注:不支持数据块
//-----------------------------------------------
WORD api_ReadClosingRecord645( BYTE *DI, BYTE *pOutBuf );
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
WORD api_ReadFreezeRecord645( BYTE *DI,BYTE *pOutBuf );
//-----------------------------------------------
//函数功能: 读负荷记录645（补遗4）
//
//参数:
//				Ch[in]:	规约通道
// 				ReadCmd[in]:调用命令 0x11--正常读	0x12--后续帧方式读
//				DI[in]:	数据标识
// 				ReadTime[in]:规约下发的时标 分时日月年
// 				DotNum[in]:规约下发要读的点数
//  			pOutBuf[out]: 返回数据
//
//返回值:		bit15位置1 代表冻结无此类型 置0代表数据能正确返回；
//				bit14置1  代表有后续帧，0为没有后续帧
//				bit0~bit13 代表返回的冻结数据长度
//
//备注:只支持补遗4要求 pOutBuf长度最大为（200-4=196），超过此长度需要分帧
//-----------------------------------------------
WORD api_ReadLpfRecord645( eSERIAL_TYPE Ch, BYTE ReadCmd, BYTE *DI, BYTE *ReadTime, BYTE DotNum, BYTE *pOutBuf );

//-----------------------------------------------
//函数功能: 读负荷记录间隔时间
//
//参数:
//				DI[in]:	数据标识
//  			pOutBuf[out]: 返回数据
//
//返回值:		bit15位置1 代表冻结无此类型 置0代表数据能正确返回；
//				bit0~bit14 代表返回的间隔时间长度
//
//备注:由于645 负荷记录间隔时间和698对象表中的间隔时间数据项不一定对应，此处只是为了解决DB1抄读负荷曲线先读间隔TIME，其实可以不支持
//-----------------------------------------------
WORD api_ReadFreezeLpf645IntervalTime( BYTE *DI, BYTE *pOutBuf );
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
void api_ReflashFreeze_Ram( void );
#endif//__FREEZE_API_H

