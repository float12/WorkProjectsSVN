//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	张玉猛
//创建日期	2018.8.8
//描述		DLT645-2007抄读冻结模块
//修改记录
//----------------------------------------------------------------------

#include "AllHead.h"
#include "Freeze.h"

#if(SEL_DLT645_2007 == YES)

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define	FREEZE_RECORD_TIME_OAD				0x00022120			// 冻结时间OAD 倒序

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct TFreeze645Map_t
{
	BYTE	DIUnit;         //645对应的数据标识
	BYTE	FollowFlag;     //645数据标识是否一个698OAD能表示出来，是否还需要后续OAD一块表示，0--后续没有，1--后续还有
	BYTE	DataLen;        //645对应数据长度，单个数据，不含费率数据块长度
	DWORD	OAD;         	//645对应的698OAD
	BYTE	( *Freeze645TransformFunc )( BYTE *Input,BYTE *Output );   //698数据源转换为645格式函数						  
}TFreeze645Map;

typedef struct TLpfClosing645Map_t
{
	WORD	wDIUnit;         //645对应的数据标识
	BYTE	FollowFlag;     //645数据标识是否一个698OAD能表示出来，是否还需要后续OAD一块表示，0--后续没有，1--后续还有
	BYTE	DataLen;		//645对应数据长度，单个数据，不含费率数据块长度
	DWORD	OAD;            //645对应的698OAD
	BYTE	( *Freeze645TransformFunc)( BYTE *Input, BYTE *Output );   //698数据源转换为645格式函数
}TLpfClosing645Map;
 
typedef struct TFreezeType645Map_t
{
	BYTE	DIUnit;         //645对应的数据标识
	BYTE	FreezeIndex;    //冻结对应的枚举号
}TFreezeType645Map;

typedef struct TLpfRemainFrame_t
{
	BYTE	RemainDotNum;		//此后续帧还需要读的点数
	DWORD	StartTime;    		//此后续帧要读的起始时间
	DWORD	EndTime;          	//此后续帧要读的截止时间
}TLpfRemainFrame;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//负荷曲线后续帧信息 分通道
static TLpfRemainFrame	LpfRemainFrame[MAX_COM_CHANNEL_NUM];

BYTE FreezeTime645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeEnergy645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeU645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeI645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezePower645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeCos645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeUBlock645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeIBlock645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezePowerBlock645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeCosBlock645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeDemandBlock645Transform( BYTE *InBuf, BYTE *OutBuf ); 
BYTE FreezeEnergyBlock645Transform( BYTE *InBuf, BYTE *OutBuf ); 
BYTE FreezeCombEnergyBlock645Transform(BYTE *InBuf, BYTE *OutBuf);
BYTE FreezeCombEnergy645Transform( BYTE *InBuf, BYTE *OutBuf );

//645 DI1与冻结类型对应表格
static const TFreezeType645Map	FreezeType645MapTab[] =
{
	{ 0x01,		eFREEZE_INSTANT,			},	//瞬时冻结
	{ 0x02,		eFREEZE_TIME_ZONE_CHG,		},	//时区表切换冻结
	{ 0x03,		eFREEZE_DAY_TIMETABLE_CHG,	},	//时段表切换冻结
	{ 0x04,		eFREEZE_HOUR,				},  //整点冻结
	#if(PREPAY_MODE == PREPAY_LOCAL)	
	{ 0x05,		eFREEZE_TARIFF_RATE_CHG,	},	//费率表切换冻结
	#endif
	{ 0x06,		eFREEZE_DAY,				},	//日冻结	
	#if(PREPAY_MODE == PREPAY_LOCAL)	
	{ 0x07,		eFREEZE_LADDER_CHG,			},	//阶梯表切换冻结
	#endif
};

//645 DI2数据项信息表格（包括FreezeType645MapTab中除整点冻结外的冻结类型）
static const TFreeze645Map	Freeze645MapTab[] =
{
	{ 0x00,		0,		5,	0x20210200,		 FreezeTime645Transform				},	//冻结时间
	// { 0x01,		0,		4,	0x00100200, 	 FreezeEnergyBlock645Transform 		},  //正向有功电能数据块
	// { 0x02,		0,		4,	0x00200200, 	 FreezeEnergyBlock645Transform 		},  //反向有功电能数据块
	// { 0x03,		0,		4,	0x00300200, 	 FreezeCombEnergyBlock645Transform 	},  //组合无功1电能数据块
	// { 0x04,		0,		4,	0x00400200, 	 FreezeCombEnergyBlock645Transform 	},  //组合无功2电能数据块
	// { 0x05,		0,		4,	0x00500200, 	 FreezeEnergyBlock645Transform 		},  //第一象限无功电能数据块
	// { 0x06,		0,		4,	0x00600200, 	 FreezeEnergyBlock645Transform 		},  //第二象限无功电能数据块
	// { 0x07,		0,		4,	0x00700200, 	 FreezeEnergyBlock645Transform 		},  //第三象限无功电能数据块
	// { 0x08,		0,		4,	0x00800200, 	 FreezeEnergyBlock645Transform 		},  //第四象限无功电能数据块
	// { 0x09,		0,		8,	0x10100200, 	 FreezeDemandBlock645Transform 		},  //正向有功最大需量及发生时间数据
	// { 0x0A,		0,		8,	0x10200200, 	 FreezeDemandBlock645Transform 		},  //反向有功最大需量及发生时间数据
	// { 0x10,		1,		12,	0x20040200, 	 FreezePowerBlock645Transform		},  //变量数据 有功功率
	// { 0x10,		0,		12,	0x20050200, 	 FreezePowerBlock645Transform		},  //变量数据 无功功率
}; 

//645 DI2数据项信息表格（整点冻结）
static const TFreeze645Map	FreezeHour645MapTab[] =
{
	{ 0x00,		0,		5,	0x20210200,		FreezeTime645Transform			},  //冻结时间
	{ 0x01,		0,		4,	0x00100201, 	 FreezeEnergy645Transform 		},  //正向有功总电能数据
	{ 0x02,		0,		4,	0x00200201, 	 FreezeEnergy645Transform 		},  //反向有功总电能数据
}; 

//645中结算冻结DI0 DI1数据项信息表格，包含需量结算数据。不支持上N次数据块，不支持当前到上12次某数据项数据块
//DataLen 为数据块内每个数据的长度,结算时间实际为4，此处定义为5，主要为了通过长度区分出电能和需量（时间与费率个数无关）
static const TLpfClosing645Map	Closing645MapTab[] =
{
	{ 0x00FE,		0,	5,	0x20210200,		FreezeTime645Transform		 		},  //冻结时间
	{ 0x0000,		0,	4,	0x00000200,		FreezeCombEnergyBlock645Transform	},  //组合有功电能数据块
	{ 0x0001,		0,	4,	0x00100200,		FreezeEnergyBlock645Transform 		},  //正向有功电能数据块
	{ 0x0002,		0,	4,	0x00200200,		FreezeEnergyBlock645Transform 		},  //反向有功电能数据块
#if(MAX_PHASE == 3)
	{ 0x0003,		0,	4,	0x00300200,		FreezeCombEnergyBlock645Transform 	},  //组合无功1电能数据块
	{ 0x0004,		0,	4,	0x00400200,		FreezeCombEnergyBlock645Transform 	},  //组合无功2电能数据块
	{ 0x0005,		0,	4,	0x00500200,		FreezeEnergyBlock645Transform 		},  //第一象限无功电能数据块
	{ 0x0006,		0,	4,	0x00600200,		FreezeEnergyBlock645Transform 		},  //第二象限无功电能数据块
	{ 0x0007,		0,	4,	0x00700200,		FreezeEnergyBlock645Transform 		},  //第三象限无功电能数据块
	{ 0x0008,		0,	4,	0x00800200,		FreezeEnergyBlock645Transform 		},  //第四象限无功电能数据块
	{ 0x0009,		0,	4,	0x00900200,		FreezeEnergyBlock645Transform 		},  //正向视在电能数据块
	{ 0x000A,		0,	4,	0x00a00200,		FreezeEnergyBlock645Transform 		},  //反向视在电能数据块
	{ 0x0101,		0,	8,	0x10100200, 	FreezeDemandBlock645Transform 		},  //正向有功最大需量及发生时间数据
	{ 0x0102,		0,	8,	0x10200200, 	FreezeDemandBlock645Transform 		},  //反向有功最大需量及发生时间数据
#endif
};

//负荷曲线DI2 DI3数据项信息表格，只支持补遗4要求
static const TLpfClosing645Map	Lpf645MapTab[] =
{
	{ 0x0101,	0,	2,	0x20000201,	FreezeU645Transform 			},  //A相电压
	// { 0x0201,	0,	3,	0x20010201,	FreezeI645Transform 			},  //A相电流
	// { 0x0300,	0,	3,	0x20040201,	FreezePower645Transform 		},  //总有功功率
	// { 0x0500,	0,	2,	0x200A0201,	FreezeCos645Transform 			},  //总功率因数
	// { 0x0601,	0,	4,	0x00100201,	FreezeEnergy645Transform 		},  //正向有功电能
	// { 0x0602,	0,	4,	0x00200201,	FreezeEnergy645Transform 		},  //反向有功电能
#if(MAX_PHASE == 3)
	// { 0x0102,	0,	2,	0x20000202,	FreezeU645Transform 			},  //B相电压
	// { 0x0103,	0,	2,	0x20000203,	FreezeU645Transform 			},  //C相电压
	// { 0x01ff,	0,	6,	0x20000200,	FreezeUBlock645Transform 		},  //电压数据块	
	// { 0x0202,	0,	3,	0x20010202,	FreezeI645Transform 			},  //B相电流
	// { 0x0203,	0,	3,	0x20010203,	FreezeI645Transform 			},  //C相电流
	// { 0x02ff,	0,	9,	0x20010200,	FreezeIBlock645Transform 		},  //电流数据块	
	// { 0x0301,	0,	3,	0x20040202,	FreezePower645Transform 		},  //A相有功功率
	// { 0x0302,	0,	3,	0x20040203,	FreezePower645Transform 		},  //B相有功功率
	// { 0x0303,	0,	3,	0x20040204,	FreezePower645Transform 		},  //C相有功功率
	// { 0x03ff,	0,	12,	0x20040200,	FreezePowerBlock645Transform 	},  //有功功率数据块
	// { 0x0400,	0,	3,	0x20050201,	FreezePower645Transform 		},  //总无功功率
	// { 0x0401,	0,	3,	0x20050202,	FreezePower645Transform 		},  //A相无功功率
	// { 0x0402,	0,	3,	0x20050203,	FreezePower645Transform 		},  //B相无功功率
	// { 0x0403,	0,	3,	0x20050204,	FreezePower645Transform 		},  //C相无功功率
	// { 0x04ff,	0,	12,	0x20050200,	FreezePowerBlock645Transform 	},  //无功功率数据块	
	// { 0x0501,	0,	2,	0x200A0202,	FreezeCos645Transform 			},  //A相功率因数
	// { 0x0502,	0,	2,	0x200A0203,	FreezeCos645Transform 			},  //B相功率因数
	// { 0x0503,	0,	2,	0x200A0204,	FreezeCos645Transform 			},  //C相功率因数
	// { 0x05ff,	0,	8,	0x200A0200,	FreezeCosBlock645Transform 		},  //功率因数数据块	
	// { 0x0603,	0,	4,	0x00300201,	FreezeCombEnergy645Transform 	},  //组合无功电能1
	// { 0x0604,	0,	4,	0x00400201,	FreezeCombEnergy645Transform 	},  //组合无功电能2
	// { 0x06ff,	1,	4,	0x00100201,	FreezeEnergy645Transform 	 	},  //正向有功电能
	// { 0x06ff,	1,	4,	0x00200201,	FreezeEnergy645Transform 	 	},  //反向有功电能
	// { 0x06ff,	1,	4,	0x00300201,	FreezeCombEnergy645Transform 	},  //组合无功电能1
	// { 0x06ff,	0,	4,	0x00400201,	FreezeCombEnergy645Transform 	},  //组合无功电能2
	// { 0x0701,	0,	4,	0x00500201,	FreezeEnergy645Transform 		},  //第1象限无功总电能
	// { 0x0702,	0,	4,	0x00600201,	FreezeEnergy645Transform 	 	},  //第2象限无功总电能
	// { 0x0703,	0,	4,	0x00700201,	FreezeEnergy645Transform 	 	},  //第3象限无功总电能
	// { 0x0704,	0,	4,	0x00800201,	FreezeEnergy645Transform 	 	},  //第4象限无功总电能
	// { 0x07ff,	1,	4,	0x00500201,	FreezeEnergy645Transform 	 	},  //第1象限无功总电能
	// { 0x07ff,	1,	4,	0x00600201,	FreezeEnergy645Transform 	 	},  //第2象限无功总电能
	// { 0x07ff,	1,	4,	0x00700201,	FreezeEnergy645Transform 	 	},  //第3象限无功总电能
	// { 0x07ff,	0,	4,	0x00800201,	FreezeEnergy645Transform 	 	},  //第4象限无功总电能
	// { 0x0801,	0,	3,	0x20170200,	FreezePower645Transform 		},  //当前有功需量
	// { 0x0802,	0,	3,	0x20180200,	FreezePower645Transform 		},  //当前无功需量
	// { 0x08ff,	1,	3,	0x20170200,	FreezePower645Transform 		},  //当前有功需量
	// { 0x08ff,	0,	3,	0x20180200,	FreezePower645Transform 		},  //当前无功需量
#endif
};

//负荷曲线间隔时间对应关联属性对象表中的间隔时间（645里面每类第一个数据对应的OAD）
static DWORD	Lpf645IntervalTime[] = 
{ 
	0x20000200, //第1类负荷记录间隔时间 电压数据块
	0x20040200, //第2类负荷记录间隔时间 有功功率数据块
	0x200A0200, //第3类负荷记录间隔时间 功率因数数据块
	0x00100201, //第4类负荷记录间隔时间 正向有功电能
	0x00500201, //第5类负荷记录间隔时间 第1象限无功总电能
	0x20170200, //第6类负荷记录间隔时间 当前有功需量
};
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
extern const TFreezeInfoTab	FreezeInfoTab[];
extern TFreezeDataInfo  MinInfo[MAX_FREEZE_PLANNUM];

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
extern WORD ReadFreezeByLastNum( TFreezeData	*pData );
extern WORD ReadFreezeRecord645Sub( eFreezeType inFreezeIndex, TFreezeData *pData );
extern BOOL SearchFreezeRecordNoByTime( TFreezeData	*pData );
extern WORD ReadFreezeByTime( TFreezeData	*pData );

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 将698格式时间转换为645格式（分时日月年 BCD码）
//
//参数: 	InBuf[in]:698格式的时间数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将698格式时间转换为645格式（分时日月年 BCD码）
//-----------------------------------------------
BYTE FreezeTime645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Buf[6];

	api_TimeFormat698To645( (TRealTimer*)InBuf, Buf );
	lib_ExchangeData( OutBuf, Buf, 5 );
	
	return 5;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的电能数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:单个电能数据
//-----------------------------------------------
BYTE FreezeEnergy645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	TFourByteUnion tdw;

	memcpy( tdw.b, InBuf, 4 );

	if( tdw.d == 0xFFFFFFFF ) //如果数据为全FF 说明冻结的历史数据里没有此数据项 把数据置为0 -1的情况不做考虑
	{
		tdw.d = 0;
	}
	else
	{
		tdw.d = lib_DWBinToBCD( tdw.d );
	}
	memcpy( OutBuf, tdw.b, 4 );

	return 4;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的电能数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:单个电能数据，组合电能，带符号
//-----------------------------------------------
BYTE FreezeCombEnergy645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Flag;
	TFourByteUnion tdw;

	memcpy( tdw.b, InBuf, 4 );
	
	if( tdw.d == 0xFFFFFFFF ) //如果数据为全FF 说明冻结的历史数据里没有此数据项 把数据置为0 -1的情况不做考虑
	{
		tdw.d = 0;
	}
	
	if( tdw.l < 0 )
	{
		tdw.l *= -1;
		Flag = 1;
	}
	else
	{
		Flag = 0;
	}

	tdw.d %= (DWORD)(80000000);  //对组合电能进行求余

	//因为前有if( tdw.l < 0 )tdw.l *= -1;执行到此不用判断tdw.d != 0xffffffff
	tdw.d = lib_DWBinToBCD( tdw.d );

	memcpy( OutBuf, tdw.b, 4 );

	if( Flag == 1 )
	{
		OutBuf[3] |= 0x80;
	}

	return 4;
}

//-----------------------------------------------
//函数功能: 将读出的组合有功电能（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的电能数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:电能数据块（根据费率个数）
//-----------------------------------------------
BYTE FreezeCombEnergyBlock645Transform(BYTE *InBuf, BYTE *OutBuf)
{
  BYTE i, tCurrRatio;
  
  tCurrRatio = FPara1->TimeZoneSegPara.Ratio;
  
  if( tCurrRatio > MAX_RATIO )
  {
    tCurrRatio = 4;
  }
  for( i = 0; i < (tCurrRatio+1); i++ )
  {
    FreezeCombEnergy645Transform( InBuf + 4 * i, OutBuf + 4 * i );
  }
  
  return (tCurrRatio+1) * 4;
}


//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的电能数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:电能数据块（根据费率个数）
//-----------------------------------------------
BYTE FreezeEnergyBlock645Transform(BYTE *InBuf, BYTE *OutBuf)
{
	BYTE i, tCurrRatio;
	TFourByteUnion tdw;
	
	tCurrRatio = FPara1->TimeZoneSegPara.Ratio;
	
	if( tCurrRatio > MAX_RATIO )
	{
		tCurrRatio = 4;
	}

	for( i = 0; i < (tCurrRatio+1); i++ )
	{
   		FreezeEnergy645Transform( InBuf + 4 * i, OutBuf + 4 * i ); 
    }
	
	return (tCurrRatio+1) * 4;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将2字节电压转换为645格式2字节电压
//-----------------------------------------------
BYTE FreezeU645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	TTwoByteUnion tw;

	memcpy(tw.b, InBuf, 2 );
	tw.w = lib_WBinToBCD( tw.w );
	memcpy( OutBuf, tw.b, 2 );

	return 2;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将2字节电压转换为645格式2字节电压（ABC三相），只有三相表会用此函数
//-----------------------------------------------
BYTE FreezeUBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i;
	TTwoByteUnion tw;

	for( i = 0; i < 3; i++ )
	{
		memcpy( tw.b, InBuf + 2 * i, 2 );
		tw.w = lib_WBinToBCD( tw.w );
		memcpy( OutBuf + 2 * i, tw.b, 2 );
	}

	return 3 * 2;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将4字节功率转换为645格式3字节功率
//-----------------------------------------------
BYTE FreezePower645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Flag;
	TFourByteUnion tdw;

	memcpy( tdw.b, InBuf, 4 );
	if( tdw.l < 0 )
	{
		tdw.l *= -1;
		Flag = 1;
	}
	else
	{
		Flag = 0;
	}
	
	tdw.d = lib_DWBinToBCD( tdw.d );
		
	memcpy( OutBuf, tdw.b, 3 );

	if( Flag == 1 )
	{
		OutBuf[2] |= 0x80;
	}
	
	return 3;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将4字节功率转换为645格式3字节功率（总ABC相）
//-----------------------------------------------
BYTE FreezePowerBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i,Flag;
	TFourByteUnion tdw;

	for( i = 0; i < 4; i++ )
	{
		memcpy( tdw.b, InBuf + 4 * i, 4 );
				
		if( tdw.l < 0 )
		{
			tdw.l *= -1;
			Flag = 1;
		}
		else
		{
			Flag = 0;
		}
		tdw.d = lib_DWBinToBCD( tdw.d );
		
		memcpy( OutBuf + 3 * i, tdw.b, 3 );
		
		if( Flag == 1 )
		{
			OutBuf[3*i+2] |= 0x80;
		}
		
		#if(MAX_PHASE == 1)
		if( i >= 2 )
		{
			//单相表对不支持的相填充ff
			memset( OutBuf + 3 * i, 0xff, 3 ); 
		}
		#endif
	}

	return 12;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将4字节电流转换为645格式3字节电流
//-----------------------------------------------
BYTE FreezeI645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	TFourByteUnion tdw;
	BYTE	Flag;

	Flag = 0;

	memcpy( tdw.b, InBuf, 4 );
	if( tdw.l < 0 )
	{
		tdw.l *= -1;
		Flag = 1;
	}
	tdw.d = lib_DWBinToBCD( tdw.d );
	memcpy( OutBuf, tdw.b, 3 );
	
	if( Flag == 1 )
	{
		OutBuf[2] |= 0x80;
	}
	
	return 3;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将4字节电流转换为645格式3字节电流（ABC相），只有三相表用此函数
//-----------------------------------------------
BYTE FreezeIBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i,Flag;
	TFourByteUnion tdw;

	for( i = 0; i < 3; i++ )
	{
		memcpy( tdw.b, InBuf + 4 * i, 4 );
		if( tdw.l < 0 )
		{
			tdw.l *= -1;
			Flag = 1;
		}
		else
		{
			Flag = 0;
		}
		tdw.d = lib_DWBinToBCD( tdw.d );
		memcpy( OutBuf + 3 * i, tdw.b, 3 );
		if( Flag == 1 )
		{
			OutBuf[3 * i + 2] |= 0x80;
		}
	}

	return 3 * 3;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将11字节需量及时间转换为645格式8字节需量及时间（根据费率数）
//-----------------------------------------------
BYTE FreezeDemandBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i, tCurrRatio;
	TFourByteUnion tdw;
	BYTE Buf[6]; 

	tCurrRatio = FPara1->TimeZoneSegPara.Ratio;

	if( tCurrRatio > MAX_RATIO )
	{
		tCurrRatio = 4;
	}

	for( i = 0; i < (tCurrRatio + 1); i++ )
	{
		//需量
		memcpy( tdw.b, InBuf + 11 * i, 4 );
        if( tdw.d == 0xffffffff )//645读第2、3结算日需量数据时要用到此
        {
        	memset( OutBuf + 8 * i, 0xff, 8 );
        }
        else
        {	
            tdw.d = lib_DWBinToBCD( tdw.d );
            memcpy( OutBuf + 8 * i, tdw.b, 3 );
            //需量发生时间		
            api_TimeFormat698To645( (TRealTimer *)(InBuf+11*i+4), Buf );
            lib_ExchangeData( OutBuf+8*i+3, Buf, 5 );
        }
	}

	return(tCurrRatio + 1) * 8;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将2字节功率因数转换为645格式2字节功率因数
//-----------------------------------------------
BYTE FreezeCos645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Flag;
	TTwoByteUnion tw;
	
	memcpy( tw.b, InBuf, 2 );
	
	if( tw.sw < 0 )
	{
		tw.sw *= -1;
		Flag = 1;
	}
	else
	{
		Flag = 0;
	}
	
	tw.w = lib_WBinToBCD( tw.w );
	memcpy( OutBuf, tw.b, 2 );
	
	if( Flag == 1 )
	{
		OutBuf[1] |= 0x80;
	}	

	return 2;
}

//-----------------------------------------------
//函数功能: 将读出的数据源（698格式，HEX）转换为645格式（BCD码）
//
//参数: 	InBuf[in]:698格式的数据源
//			OutBuf[out]：645格式的数据
//返回值:	645格式数据长度
//
//备注:将2字节功率因数转换为645格式2字节功率因数（总ABC相），只有三相表用此函数
//-----------------------------------------------
BYTE FreezeCosBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i,Flag;
	TTwoByteUnion tw;
	for( i = 0; i < 4; i++ )
	{
		memcpy( tw.b, InBuf + 2 * i, 2 );
		if( tw.sw < 0 )
		{
			tw.sw *= -1;
			Flag = 1;
		}
		else
		{
			Flag = 0;
		}
		
		tw.w = lib_WBinToBCD( tw.w );
		memcpy( OutBuf + 2 * i, tw.b, 2 );
		if( Flag == 1 )
		{
			OutBuf[2*i+1] |= 0x80;
		}
	}

	return 4 * 2;
}


//-----------------------------------------------
//函数功能: 读冻结记录645（瞬时、约定、整点、日冻结）
//
//参数:
//				DI[in]:	数据标识
//  			pOutBuf[out]: 返回数据
//
//返回值:		bit15位置1 代表冻结无此类型 置0代表数据能正确返回；
//				bit0~bit14 代表返回的冻结数据长度
//
//备注:支持数据块，不支持定时冻结
//-----------------------------------------------
WORD api_ReadFreezeRecord645( BYTE *DI,BYTE *pOutBuf )
{
	BYTE i, j,tFreezeIndex;
	TFourByteUnion tdw,tOAD;
	BYTE Buf[260];
	WORD Len,wReturnLen;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE + 1];
	TFreezeAttCycleDepth AttCycleDepth;
	TFreezeDataInfo	DataInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TDLT698RecordPara DLT698RecordPara;
    TFreezeData Data;
	
	memcpy( tdw.b, DI, 4 );
	wReturnLen = 0;
	Len = 0;
	
	if( tdw.b[3] != 0x05 )
	{
		return 0x8000;
	}
	
	//查找冻结类型
	for( i = 0; i < sizeof(FreezeType645MapTab) / sizeof(TFreezeType645Map); i++ )
	{
		if( FreezeType645MapTab[i].DIUnit == tdw.b[2] )
		{
			break;
		}
	}
	
	if( i == sizeof(FreezeType645MapTab) / sizeof(TFreezeType645Map) )
	{
		return 0x8000;
	}
	
	tFreezeIndex = FreezeType645MapTab[i].FreezeIndex;
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
	if( api_VReadSafeMem( FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo ) != 	TRUE )
	{
		return 0x8000;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return 0x8000;
	}

	//读出所有的关联属性对象
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad) * FreezeInfoTab[tFreezeIndex].MaxAttNum + sizeof(DWORD), (BYTE *)&AttOad[0] );
	//读出所有的关联属性对象 周期深度
	api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth );
	//读出此条的RecordNo
	if( api_VReadSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo ) != TRUE )
	{
		return 0x8000;
	}
	
	//分解按照698的方法9返回数据
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG; 
	DLT698RecordPara.OADNum = 1;
	DLT698RecordPara.TimeOrLast = tdw.b[0];
	Data.pDLT698RecordPara = &DLT698RecordPara; 
	Data.Tag = eData;
	Data.FreezeIndex = tFreezeIndex;
	Data.pBuf = Buf;
	Data.Len = sizeof(Buf);
	Data.pTime = NULL;
	Data.pAttOad = &AttOad[0];
	Data.pAttCycleDepth = &AttCycleDepth;
	Data.pDataInfo = &DataInfo;
	Data.pFreezeAddrLen = &FreezeAddrLen;
	Data.pAllInfo = &FreezeAllInfo;
	
	if( tFreezeIndex == eFREEZE_HOUR )//小时冻结
	{
		//查找此种冻结内的数据项
		if( tdw.b[1] == 0xff )
		{
			for( j = 0; j < sizeof(FreezeHour645MapTab) / sizeof(TFreeze645Map); j++ )
			{
				//要读的点已经产生
				if( tdw.b[0] <= DataInfo.SaveDot )
				{
					tOAD.d = FreezeHour645MapTab[j].OAD;
					lib_InverseData( tOAD.b, 4 );
	
					Data.pDLT698RecordPara->pOAD = tOAD.b;
	
					//RSD 方法9：按照次数索引
					Len = ReadFreezeByLastNum( &Data );
					if( (Len == 0)||(Len == 1)||(Len == 0x8000) )
					{
						return 0x8000;
					}
					
					Len = FreezeHour645MapTab[j].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen ); 
				}
				
				//要读的点还没有产生
				if( tdw.b[0] > DataInfo.SaveDot )
				{
					Len = FreezeHour645MapTab[j].DataLen;
					memset( pOutBuf + wReturnLen, 0x00, Len );
				}
				
				wReturnLen += Len;
				pOutBuf[wReturnLen] = 0xaa;
				wReturnLen++;
			}
		}
		else
		{
			for( i = 0; i < sizeof(FreezeHour645MapTab) / sizeof(TFreeze645Map); i++ )
			{
				if( FreezeHour645MapTab[i].DIUnit == tdw.b[1] )
				{
					break;
				}
			}
			if( i == sizeof(FreezeHour645MapTab) / sizeof(TFreeze645Map) )
			{
				return 0x8000;
			}

			//要读的点已经产生
			if( tdw.b[0] <= DataInfo.SaveDot )
			{
				tOAD.d = FreezeHour645MapTab[i].OAD;
				lib_InverseData( tOAD.b, 4 );
	
				Data.pDLT698RecordPara->pOAD = tOAD.b;
	
				//RSD 方法9：按照次数索引
				Len = ReadFreezeByLastNum( &Data );
				if( (Len == 0)||(Len == 1)||(Len == 0x8000) )
				{
					return 0x8000;
				}
				
				wReturnLen = FreezeHour645MapTab[i].Freeze645TransformFunc( Data.pBuf, pOutBuf ); 
			}
			
			//要读的点还没有产生
			if( tdw.b[0] > DataInfo.SaveDot )
			{
				wReturnLen = FreezeHour645MapTab[i].DataLen;
				memset( pOutBuf, 0x00, wReturnLen );
			}
		}
	}
	else//日冻结
	{
		//查找此种冻结内的数据项
		if( tdw.b[1] == 0xff )
		{
			for( j = 0; j < sizeof(Freeze645MapTab) / sizeof(TFreeze645Map); j++ )
			{
				//要读的点已经产生
				if( tdw.b[0] <= DataInfo.SaveDot )
				{
					tOAD.d = Freeze645MapTab[j].OAD;
					lib_InverseData( tOAD.b, 4 );
		
					Data.pDLT698RecordPara->pOAD = tOAD.b;
		
					//RSD 方法9：按照次数索引
					Len = ReadFreezeByLastNum( &Data );
					if( (Len == 0)||(Len == 1)||(Len == 0x8000) )
					{
						//当返回异常时，此数据直接用AA略过
						if( (Freeze645MapTab[j].OAD == 0x20050200)||(Freeze645MapTab[j].OAD == 0x20040200) )
						{
							memset( pOutBuf + wReturnLen, 0xFF, 12 );
							wReturnLen += 12;

							if( Freeze645MapTab[j].FollowFlag == 0 )
							{					
								pOutBuf[wReturnLen] = 0xaa;
								wReturnLen++;
							}

							continue;
						}
						else
						{
							pOutBuf[wReturnLen] = 0xaa;
							wReturnLen++;
							continue;
						}
					}					
					
				}
				
				Len = Freeze645MapTab[j].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen );
				
				if( tdw.b[0] > DataInfo.SaveDot ) 
				{
					//要读的点还没有产生					
					#if(MAX_PHASE == 1)
					//单相表不支持的数据项aa空过（无功、需量）
					if( (j >= 0x03)&&(j <= 0x0a) )
					{
						pOutBuf[wReturnLen] = 0xaa;
						wReturnLen++;
						continue;
					}
					if( j == 0x0c )//无功功率填充ff
					{
						memset( pOutBuf + wReturnLen, 0xff, Len );
					}
					else if( j == 0x0b ) //有功功率
					{
						memset( pOutBuf + wReturnLen, 0x00, Len );//总 A
						memset( pOutBuf + wReturnLen + 6, 0xff, Len );//B C
					}
					else
					#endif
					{
						memset( pOutBuf + wReturnLen, 0x00, Len );
					}
				}
				
				wReturnLen += Len;
	
				if( Freeze645MapTab[j].FollowFlag == 0 )
				{					
					pOutBuf[wReturnLen] = 0xaa;
					wReturnLen++;
				}
			}
		}
		else
		{
			for( i = 0; i < sizeof(Freeze645MapTab) / sizeof(TFreeze645Map); i++ )
			{
				if( Freeze645MapTab[i].DIUnit == tdw.b[1] )
				{
					break;
				}
			}
			if( i == sizeof(Freeze645MapTab) / sizeof(TFreeze645Map) )
			{
				return 0x8000;
			}
			
			for( j = 0; j < 10; j++ ) //最多查找10次，在表格中没有连续10次的情况，表格中FollowFlag的连续次数
			{
				if ((i + j) > (sizeof(Freeze645MapTab) / sizeof(TFreeze645Map) - 1))
				{
					break;
				}
				//要读的点已经产生
				if( tdw.b[0] <= DataInfo.SaveDot )
				{
					tOAD.d = Freeze645MapTab[i + j].OAD;
					lib_InverseData( tOAD.b, 4 );
			
					Data.pDLT698RecordPara->pOAD = tOAD.b;
					
					//RSD 方法9：按照次数索引
					Len = ReadFreezeByLastNum( &Data );
										
                    if( (Len == 0)||(Len == 1)||(Len == 0x8000) )
					{
					    if( (Freeze645MapTab[i + j].OAD == 0x20050200)||(Freeze645MapTab[i + j].OAD == 0x20040200) )
    					{
							memset( pOutBuf + wReturnLen, 0xFF, 12 );
							wReturnLen += 12;
							continue;
    					}
    					else
    					{
						    return 0x8000;
    					}
					}					
					
				}
		
				Len = Freeze645MapTab[i+j].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen );				

				//要读的点还没有产生
				if( tdw.b[0] > DataInfo.SaveDot )
				{					
					//要读的点还没有产生
					#if(MAX_PHASE == 1)
					//单相表不支持的数据项返回错误（无功、需量）
					if( ((i + j) >= 0x03)&&((i + j) <= 0x0a) )
					{
						return 0x8000;
					}
					else if( (i + j) == 0x0c )//无功功率返回ff
					{
						memset( pOutBuf + wReturnLen, 0xff, Len ); 
					}
					else if( (i + j) == 0x0b ) //有功功率
					{
						memset( pOutBuf + wReturnLen, 0x00, Len ); //总 A
						memset( pOutBuf + wReturnLen + 6, 0xff, Len ); //B C
					}
					else
					#endif
					{
						memset( pOutBuf + wReturnLen, 0x00, Len );
					}
				}
			
				wReturnLen += Len; 
				
				if( Freeze645MapTab[i + j].FollowFlag == 0 )
				{
					break;
				}
			}
		}
	}
	
	return wReturnLen;	
}

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
WORD api_ReadClosingRecord645( BYTE *DI, BYTE *pOutBuf )
{
	BYTE i, j, tFreezeIndex;
	TFourByteUnion tdw, tOAD;
	BYTE Buf[200];
	WORD wReturnLen,Len;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE + 1];
	TFreezeAttCycleDepth AttCycleDepth;
	TFreezeDataInfo	DataInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TDLT698RecordPara DLT698RecordPara;
	TFreezeData Data; 

	memcpy(tdw.b,DI,4);
	wReturnLen = 0;

	if( (tdw.b[3] != 0x00)&&(tdw.b[3] != 0x01) )
	{
		return 0x8000;
	}
	
	if( (tdw.b[1] != 0xff)&&(tdw.b[1] > FPara1->TimeZoneSegPara.Ratio) )
	{
		return 0x8000;
	}
	
	// 获得各种地址
	if( GetFreezeAddrInfo( eFREEZE_CLOSING, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	//读出单条冻结的长度及关联属性的个数
	if( api_VReadSafeMem( FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo ) != 	TRUE )
	{
		return 0x8000;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[eFREEZE_CLOSING].MaxAttNum )
	{
		return 0x8000;
	}

	//读出所有的关联属性对象
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad) * FreezeInfoTab[eFREEZE_CLOSING].MaxAttNum + sizeof(DWORD), (BYTE *)&AttOad[0] );
	//读出所有的关联属性对象 周期深度
	api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth );
	//读出此条的RecordNo
	if( api_VReadSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo ) != TRUE )
	{
		return 0x8000;
	}
	
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;
	DLT698RecordPara.OADNum = 1;
	DLT698RecordPara.TimeOrLast = tdw.b[0];
	Data.pDLT698RecordPara = &DLT698RecordPara;
	Data.Tag = eData;
	Data.FreezeIndex = eFREEZE_CLOSING;
	Data.pBuf = Buf;
	Data.Len = sizeof(Buf);
	Data.pTime = NULL;
	Data.pAttOad = &AttOad[0];
	Data.pAttCycleDepth = &AttCycleDepth;
	Data.pDataInfo = &DataInfo;
	Data.pFreezeAddrLen = &FreezeAddrLen;
	Data.pAllInfo = &FreezeAllInfo;
	
	for( i = 0; i < sizeof(Closing645MapTab) / sizeof(TLpfClosing645Map); i++ )
	{
		if( Closing645MapTab[i].wDIUnit == tdw.w[1] )
		{
			break;
		}
	}
	if( i == sizeof(Closing645MapTab) / sizeof(TLpfClosing645Map) )
	{
		return 0x8000;
	}
		
	if( tdw.b[0] <= DataInfo.SaveDot )
	{	
		tOAD.d = Closing645MapTab[i].OAD;
		lib_InverseData( tOAD.b, 4 );
	
		Data.pDLT698RecordPara->pOAD = tOAD.b;
	
		//RSD 方法9：按照次数索引
		Len = ReadFreezeByLastNum( &Data );
		if( (Len == 0)||(Len == 0x8000) )
		{
			return 0x8000;
		}
		else if(Len == 1)
		{
		  	if((tOAD.d == 0x00021010)||(tOAD.d == 0x00022010))//645读第二第三结算日需量返回0XFF
			{
				memset((BYTE *)Buf,0xFF,Data.Len);
			}
			else
			{
		  		return 0x8000;
			}
		}
		
		wReturnLen = Closing645MapTab[i].Freeze645TransformFunc( Data.pBuf, Buf );
		
		//读分费率电能（DataLen为5代表结算时间）
		if( (Closing645MapTab[i].DataLen != 5)&&(tdw.b[1] != 0xff) )
		{
			memcpy( pOutBuf, Buf + tdw.b[1] * Closing645MapTab[i].DataLen, Closing645MapTab[i].DataLen );
			wReturnLen = Closing645MapTab[i].DataLen;
		}
		else
		{
			if( wReturnLen < 160 )
			{
				if( wReturnLen == 5 )
				{
					//结算时间为4个字节，时日月年，需要处理一下。表格中为了通过长度区分出电能，因此将时间定为5个字节
					memcpy( pOutBuf, Buf+1, 4 ); 
				}
				else
				{
					memcpy( pOutBuf, Buf, wReturnLen );
				}
			}
			else
			{
				return 0x8000;
			}
		}
	}
	else if( tdw.b[0] == 0xff )
	{
		return 0x8000;
	}
	else
	{
		//还没有产生冻结,返回数据0
		if( Closing645MapTab[i].DataLen == 5 )
		{
			//结算时间为4个字节，时日月年，需要处理一下。表格中为了通过长度区分出电能，因此将时间定为5个字节
			wReturnLen = 4;
		}
		else	
		{
			if( tdw.b[1] != 0xff )
			{
				wReturnLen = Closing645MapTab[i].DataLen;
			}
			else
			{
				wReturnLen = Closing645MapTab[i].DataLen * (FPara1->TimeZoneSegPara.Ratio + 1);
				if( wReturnLen > ((MAX_RATIO + 1) * 4) )
				{
					wReturnLen = 20;
				}
			}
		}
		memset( pOutBuf, 0x00, wReturnLen );
	}
	
	return wReturnLen;
}

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
// 				返回0代表规约只返回数据标识
//
//备注:只支持补遗4要求 pOutBuf长度最大为（200-4=196），超过此长度需要分帧
//-----------------------------------------------
WORD api_ReadLpfRecord645( eSERIAL_TYPE Ch, BYTE ReadCmd, BYTE *DI, BYTE *ReadTime, BYTE DotNum, BYTE *pOutBuf )
{
	BYTE i, j,tFreezeIndex, tReadDotNum, tStartOadPosi, tOadNum, SingleDataLen;
	TFourByteUnion tdw, tOAD;
	TFourByteUnion tEnergyBak[4];//补电能用，在表格中最多会有四个连续电能OAD
	BYTE Buf[100],ClassAttribute;
	WORD wReturnLen,Len,wOffset,OI,tEmptyDotNum;
	DWORD dwReadStartTime,dwReadEndTime,SearchTime,dwFirstAddr,dwFirstTime,dwTempOad;
	DWORD dwOad[10];//645负荷记录要读取的OAD，实际最多4个
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE + 1];
	TFreezeAttCycleDepth AttCycleDepth;
	TFreezeAttCycleDepth MinAttCycleDepth[MAX_FREEZE_PLANNUM]; 
	TFreezeDataInfo	DataInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TDLT698RecordPara DLT698RecordPara;
	TFreezeData Data;
	TRealTimer t;
	TFreezeMinInfo FreezeMinInfo;

	memcpy( tdw.b, DI, 4 );
	
	if( tdw.w[1] != 0x0610 )
	{
		return 0x8000;
	}
	
	SingleDataLen = 0;
	wReturnLen = 0; 
	Len = 0;
	memset( tEnergyBak[0].b, 0xff, sizeof(tEnergyBak) );
	memset( (BYTE*)&Data, 0x00, sizeof(TFreezeData) );

	//根据数据标识查找对应的OAD
	for( i = 0; i < sizeof(Lpf645MapTab) / sizeof(TLpfClosing645Map); i++ )
	{
		if( Lpf645MapTab[i].wDIUnit == tdw.w[0] )
		{
			break;
		}
	}
	if( i == sizeof(Lpf645MapTab) / sizeof(TLpfClosing645Map) )
	{
		return 0x8000;
	}

	for( j = 0; j < 10; j++ ) //最多查找10次，在表格中没有连续10次的情况，实际最多4次
	{
		//计算一条数据的长度 若要读四个无功电能，则长度为16
		SingleDataLen += Lpf645MapTab[i + j].DataLen;
		
		if( Lpf645MapTab[i + j].FollowFlag == 0 )
		{
			break;
		}
	}
	
	tStartOadPosi = i;//要搜索OAD在表格中的起始位置  因为不一定是一个OAD
	tOadNum = j+1;		//此数据标识对应OAD的个数

	// 获得各种地址
	if( GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen ) == FALSE )
	{
		return 0;
	}

	//读出单条冻结的长度及关联属性的个数
	if( api_VReadSafeMem( FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo ) != TRUE )
	{
		return 0;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[eFREEZE_MIN].MaxAttNum )
	{
		return 0;
	}

	//读出所有的关联属性对象
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad) * FreezeInfoTab[eFREEZE_MIN].MaxAttNum + sizeof(DWORD), (BYTE *)&AttOad[0] );

	//读取分钟冻结地址信息
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo ); 
	if( FreezeMinInfo.DataAddr[0] != FreezeAddrLen.dwDataAddr )//方案0地址 与基地址一致
	{
		FreezeMinInfo.DataAddr[0] = FreezeAddrLen.dwDataAddr;
	}

	//读出所有的关联属性对象 周期深度
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&MinAttCycleDepth[i] );
	}
	
	Buf[0] = 0xff;//暂用于保存读取的第1个OAD在698关联对象属性表中的位置
	for( i=0; i<tOadNum; i++ )//找到645映射表中OAD对应的698关联对象属性表中OAD，得到698要读取的关联对象属性表中OAD
	{
		if( i >= 4 )
		{
			break;
		}
		
		//按照645标识对应的第一个oad寻找关联属性对象表中对应的对象
		tOAD.d = Lpf645MapTab[tStartOadPosi+i].OAD;//例如读正向有功总电能 Lpf645MapTab[tStartOadPosi].OAD:0x00100201
		lib_InverseData( tOAD.b, 4 );
		tdw.d = tOAD.d&(~0xFF000000);//考虑698关联对象表中配置电压数据块，645只读A相电压
		dwOad[i] = tOAD.d;//要读取的OAD列表先赋645映射表中的,下面for 循环再加698冻结方案号
	
		lib_ExchangeData((BYTE *)&OI,(BYTE *)&dwOad[i],2);
		ClassAttribute =(BYTE)((dwOad[i]&0x001f0000)>>16);
		
		for( j = 0; j < FreezeAllInfo.NumofOad; j++ )
		{
			//例如读正向有功总电能 AttOad[j].d:0x01021000 tOAD.d经过上面lib_InverseData也变为:0x01021000,tdw.d:0x00021000
			if( (tOAD.d == (AttOad[j].Oad&(~0x00E00000)) )
				||( tdw.d == (AttOad[j].Oad&(~0x00E00000)) ) )				
			{
				if( i == 0 )//默认645要读的第1个OAD在698关联对象属性表中存在，现在645读取不支持第1个OAD不在698关联对象属性表中的情况，如果不存在返回无请求数据
				{
					Buf[0] = j;//暂用于保存读取的第1个OAD在698关联对象属性表中的位置
				}
				dwOad[i] = (tOAD.d | (AttOad[j].Oad&0x00E00000) );//读取的OAD从645的映射中取，加上698属性表中的方案号
				break;
			}
		}
		if( j >= FreezeAllInfo.NumofOad )
		{
			if( (OI < 0x1000) && (ClassAttribute == 2))//电能OI且属性2未搜索到
			{
				dwTempOad = dwOad[i]+0x20000;//置位元素索引为04进行搜索
				for( j = 0; j < FreezeAllInfo.NumofOad; j++ )
				{
					if( dwTempOad == (AttOad[j].Oad&(~0x00E00000)) )				
					{
						if( i == 0 )//默认645要读的第1个OAD在698关联对象属性表中存在，现在645读取不支持第1个OAD不在698关联对象属性表中的情况，如果不存在返回无请求数据
						{
							Buf[0] = j;//暂用于保存读取的第1个OAD在698关联对象属性表中的位置
						}
						dwOad[i] = (tOAD.d | (AttOad[j].Oad&0x00E00000) );//读取的OAD从645的映射中取，加上698属性表中的方案号
						break;
					}
				}
			}
		}
	}
	j = Buf[0];//645读取的第1个OAD在698关联对象属性表中的位置
	if( j >= FreezeAllInfo.NumofOad )
	{
		return 0; 		
	}
	
	j = CURR_PLAN(AttOad[j].Oad);//获取方案号
	
	AttCycleDepth.Cycle = MinAttCycleDepth[j].Cycle;
	AttCycleDepth.Depth = MinAttCycleDepth[j].Depth;

	//查找分钟冻结下发的对象对应的RecordNo
	memcpy( (BYTE *)&DataInfo, (BYTE *)&MinInfo[j], sizeof(TFreezeDataInfo) );

	//还没有产生冻结
	if( DataInfo.RecordNo == 0 )
	{
		return 0;
	}
	
	DLT698RecordPara.OADNum = 1; //在搜索时每次都是按照一个OAD进行搜索，因为数据源和645要求的数据长度不一定一样，所以单个OAD搜索
	Data.pDLT698RecordPara = &DLT698RecordPara;
	Data.Tag = eData;
	Data.FreezeIndex = eFREEZE_MIN;
	Data.pBuf = Buf;
	Data.Len = sizeof(Buf);
	Data.pTime = NULL;
	Data.MinPlanIndex = j;
	Data.pAttOad = &AttOad[0];
	Data.pAttCycleDepth = &AttCycleDepth;
	Data.pDataInfo = &DataInfo;
	Data.pFreezeAddrLen = &FreezeAddrLen;
	Data.pAllInfo = &FreezeAllInfo;
	Data.pMinInfo = &FreezeMinInfo;
	//非后续帧读取时，先根据规约下发，记录要读的第一个点RT1及最后一个点对应的时间RT2
	//再读取RT1开始电表记录的第一个点T1,电表记录的最近一个点T2，根据这四个时间进行比较返回数据
	//后续帧时，根据11命令处理完后记录的起始、截止时间记录读取
	if( ReadCmd  == 0x11 )
	{
		memset( (BYTE *)&LpfRemainFrame[Ch], 0X00, sizeof(TLpfRemainFrame) ); 

		//计算要读的起始相对时间
		lib_ExchangeData( Buf, ReadTime, 5 );
		Buf[5] = 0x00; //秒
		api_TimeFormat645To698( Buf, &t );
		dwReadStartTime = api_CalcInTimeRelativeMin( &t );
		if( AttCycleDepth.Cycle == 0 )
		{
			return 0x8000;
		}		
		if( dwReadStartTime % AttCycleDepth.Cycle )
		{
			//若不能被时间间隔整除，则置下一个能被整除的点
			dwReadStartTime = (dwReadStartTime / AttCycleDepth.Cycle)*AttCycleDepth.Cycle + AttCycleDepth.Cycle;
		}
		
		DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;
		DLT698RecordPara.TimeOrLast = Data.pDataInfo->SaveDot;
		DLT698RecordPara.OADNum = 2; 
		tEnergyBak[0].d = FREEZE_RECORD_TIME_OAD;
		tEnergyBak[1].d = dwOad[0];
		DLT698RecordPara.pOAD = tEnergyBak[0].b;
		ReadFreezeByLastNum( &Data );
		wOffset = api_GetProOADLen( eGetNormalData, eData, tEnergyBak[0].b, 0 );
		if( wOffset == 0x8000 )
		{
		 	return 0x8000;
		}
		memcpy((BYTE*)&t,Data.pBuf,wOffset);
		dwFirstTime=api_CalcInTimeRelativeMin( &t );
		//计算要读的截止相对时间
		tReadDotNum = lib_BBCDToBin( DotNum );
		if( dwReadStartTime < dwFirstTime )
		{
			tEmptyDotNum = (dwFirstTime - dwReadStartTime) / Data.pAttCycleDepth->Cycle;
			if( tReadDotNum <= tEmptyDotNum )
			{
				return 0;
			}
			tReadDotNum -= tEmptyDotNum;
			dwReadStartTime = dwFirstTime;
		}
		if( tReadDotNum == 0x00 )
		{
		    return 0x8000;
		}
		
		dwReadEndTime = dwReadStartTime + AttCycleDepth.Cycle * (tReadDotNum-1);
		if( dwReadEndTime > g_RelativeMin )
		{
			dwReadEndTime = (g_RelativeMin / AttCycleDepth.Cycle)*AttCycleDepth.Cycle;
		}
		
		//寻找要读的并且是已经记录下来的第一个点
//        DLT698RecordPara.eTimeOrLastFlag = eFREEZE_TIME_FLAG; 
//		DLT698RecordPara.TimeOrLast = dwReadStartTime;
//		DLT698RecordPara.pOAD = (BYTE *)&dwOad[0];
//		Len = SearchFreezeRecordNoByTime( &Data );
//		if( Len == FALSE )
//		{
//			return 0;
//		}
//		dwReadStartTime = Data.SearchTime;
//
		//寻找最近记录的一个点，不能直接使用Data.SearchTime直接搜，此函数没有用到Data.SearchTime
		DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;
		DLT698RecordPara.TimeOrLast = 1;
		DLT698RecordPara.OADNum = 2; 
		tEnergyBak[0].d = FREEZE_RECORD_TIME_OAD;
		tEnergyBak[1].d = dwOad[0];
		DLT698RecordPara.pOAD = tEnergyBak[0].b;
		Len = ReadFreezeByLastNum( &Data );
		if( Len == FALSE )
		{
			return 0;
		}
		
		//暂存截止相对时间，供可能有的后续帧使用
		LpfRemainFrame[Ch].EndTime = dwReadEndTime;
		LpfRemainFrame[Ch].RemainDotNum = tReadDotNum;
		
		//置返回时间
		api_ToAbsTime( dwReadStartTime, &t );
		api_TimeFormat698To645( &t, pOutBuf + wReturnLen );
		lib_InverseData( pOutBuf + wReturnLen, 5 );
		wReturnLen += 5;
	}
	else
	{
		//计算要读的起始相对时间
		dwReadStartTime = LpfRemainFrame[Ch].StartTime;
		//计算要读的截止相对时间
		dwReadEndTime = LpfRemainFrame[Ch].EndTime;
	}
	
	if( dwReadStartTime > dwReadEndTime )
	{
		return 0;
	}
	
	
	DLT698RecordPara.eTimeOrLastFlag = eFREEZE_TIME_FLAG; 
	DLT698RecordPara.OADNum = 1; 
	DLT698RecordPara.pOAD =  (BYTE *)&dwOad[0];
	
	for( SearchTime = dwReadStartTime; SearchTime <= dwReadEndTime;  )
	{		
		CLEAR_WATCH_DOG; 

		//一条报文的最大长度，超过就要分帧
		if( (wReturnLen + SingleDataLen) > 195 )
		{
			//置后续帧标志
			wReturnLen |= 0x4000;
			//保存此次要搜索的时间，供下次后续帧的起始时间使用
			LpfRemainFrame[Ch].StartTime = SearchTime;
			LpfRemainFrame[Ch].EndTime = dwReadEndTime;
			break;
		}

		if( LpfRemainFrame[Ch].RemainDotNum == 0 )
		{
			return 0;
		}
		
		//返回点数递减
		LpfRemainFrame[Ch].RemainDotNum--;
		
		for( i = 0; i < tOadNum; i++ )
		{
			if( i >= 4 )
			{
				break;
			}
			j = CURR_PLAN(dwOad[i]);//获取方案号
			if(j != Data.MinPlanIndex ) //对于与第一个电能不在同一方案中的电能直接填充全FF
			{
				//对于数据块中不支持的数据项填充ff
				memset( pOutBuf + wReturnLen, 0xff, Lpf645MapTab[tStartOadPosi + i].DataLen ); 
				wReturnLen += Lpf645MapTab[tStartOadPosi+i].DataLen;
				continue;
			}
			
			DLT698RecordPara.TimeOrLast = SearchTime;
			Data.SearchTime = 0x5555AAAA;
			DLT698RecordPara.pOAD = (BYTE *)&dwOad[i];
			Len = ReadFreezeByTime( &Data );
			
			if( (Data.SearchTime == SearchTime)&&(Len != 1) )
			{
				Lpf645MapTab[tStartOadPosi+i].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen );
				if( Lpf645MapTab[tStartOadPosi + i].DataLen == 4 )
				{
					memcpy( tEnergyBak[i].b, pOutBuf + wReturnLen, 4 );
				}
			}
			else		
			{
				if( SearchTime == dwReadStartTime )
				{
					tdw.d = Data.RecordNo;
					//若不是保存的第一个点，则采用上一个RecordNo对应点，否则直接赋值上次读取的数据
					if( Data.SearchTime != 0x5555AAAA )
					{
						if( Data.RecordNo == 0 )
						{
							return 0;
						}
						
						if( Data.RecordNo )
						{
							Data.RecordNo--;
						}
					}
					else
					{
						Data.RecordNo = MinInfo[j].RecordNo;
							
						if( Data.RecordNo )
						{
							Data.RecordNo--;
						}
					}
					
					Len = ReadFreezeByRecordNo( &Data );
					if( (Len == 0x8000)||(Len == 0) )
					{
						return 0;
					}
					Data.RecordNo = tdw.d; 
				}
				
				if( (Len == 1)&&(tOadNum > 1) )
				{
					//对于数据块中不支持的数据项填充ff
					memset( pOutBuf + wReturnLen, 0xff, Lpf645MapTab[tStartOadPosi + i].DataLen ); 
				}
				else
				{
					//补数据
					if( Lpf645MapTab[tStartOadPosi+i].DataLen != 4 )
					{
						memset( pOutBuf + wReturnLen, 0xff, Lpf645MapTab[tStartOadPosi+i].DataLen );
					}
					else
					{
						//补上一个电能,若是读的第一个数（主要是后续帧读的数）
						//则采用上一个RecordNo对应点，否则直接赋值上次读取的数据
						if( SearchTime == dwReadStartTime )
						{
							//若不是保存的第一个点，则采用上一个RecordNo对应点，否则直接赋值上次读取的数据
							Lpf645MapTab[tStartOadPosi + i].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen );
							if( Lpf645MapTab[tStartOadPosi + i].DataLen == 4 )
							{
								memcpy( tEnergyBak[i].b, pOutBuf + wReturnLen, 4 );
							}
						}
						else
						{
							memcpy( pOutBuf + wReturnLen, tEnergyBak[i].b, 4 );
						}
					}
				}
			}
			wReturnLen += Lpf645MapTab[tStartOadPosi+i].DataLen;			
		}
		
		SearchTime += AttCycleDepth.Cycle;
		
	}

	return wReturnLen;
}


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
WORD api_ReadFreezeLpf645IntervalTime( BYTE *DI, BYTE *pOutBuf )
{
	BYTE i;
	TFourByteUnion tdw;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE + 1];
	TFreezeAttCycleDepth MinAttCycleDepth;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	
	memcpy( tdw.b, DI, 4 );
	
	if( (tdw.d < 0x04000a02)||(tdw.d > 0x04000a07) )
	{
		return 0x8000;
	}
		
	tdw.d = Lpf645IntervalTime[DI[0] - 2];
	lib_InverseData( tdw.b, 4 );
	
	// 获得各种地址
	if( GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	//读出单条冻结的长度及关联属性的个数
	if( api_VReadSafeMem( FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo ) != 	TRUE )
	{
		return 0x8000;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[eFREEZE_MIN].MaxAttNum )
	{
		return 0x8000;
	}

	//读出所有的关联属性对象
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad) * FreezeInfoTab[eFREEZE_MIN].MaxAttNum + sizeof(DWORD), (BYTE *)&AttOad[0] );


	//比较关联属性对象 获取周期
	for( i = 0; i < FreezeAllInfo.NumofOad; i++ )
	{
		if( (tdw.d & 0x0000ffff) == (AttOad[i].Oad & 0x0000ffff) )
		{
			//按方案取冻结属性
			i = CURR_PLAN(AttOad[i].Oad);
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&MinAttCycleDepth ); 
			tdw.w[0] = lib_WBinToBCD( MinAttCycleDepth.Cycle );
			memcpy( pOutBuf, tdw.b, 2 );
			return 2;
		}
	}
	
	return 0x8000;	
}


#endif//#if(SEL_DLT645_2007 == YES)


