//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.01.06
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//不使用事件最大数量宏定义是因为最大数量是动态的，事件数量小的情况下会出问题所以使用最大事件数量！！！！！！
#define MAX_EVENT_PROBUF_LEN         (65*5+50)

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

typedef struct TEventRecordObj_t
{
	WORD				OI;				//698对象标识：OI对象标识
	BYTE         		class;			//归属类
	const BYTE			*ConfigPara;
}TEventRecordObj;


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//-----------------------------------------------
//				本文件使用的变量，常量

//class9冻结属性3数据标识	关联对象属性表	
static const BYTE T_FreezeRecord3[]		= 
{ 
	Array_698,			0XFF,
	Structure_698,		3,		
	Long_unsigned_698,	2,				//冻结周期
	OAD_698,			4,				//关联对象属性描述符
	Long_unsigned_698,	2,				//存储深度
};
//事件关联对象列表数据标识+新增事件列表
static const BYTE T_EventRTable[]		= 
{ 
	Array_698,		0XFF,
	OAD_698,		4,
};

//需上报事件对象列表
static const BYTE T_EventReportTable[]	= 
{ 
    Array_698,      0XFF,
    OI_698,         2,
};

//事件清零列表		
static const BYTE T_EventClearTable[]	= 
{ 
	Array_698,		0XFF,
	OMD_698,		4,
};

//当前记录数
static const BYTE T_EventNowNum24[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,				//记录表1当前记录数
	Long_unsigned_698,	2,				//记录表2当前记录数
	Long_unsigned_698,	2,				//记录表3当前记录数
	Long_unsigned_698,	2,				//记录表4当前记录数
};
//当前记录数-NULL
static const BYTE T_EventNowNum24BNull[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,				//记录表1当前记录数
	Long_unsigned_698,	2,				//记录表2当前记录数
	NULL_698,	        1,				//记录表3当前记录数
	Long_unsigned_698,	2,				//记录表4当前记录数
};
//当前记录数-NULL
static const BYTE T_EventNowNum24ABCNull[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,				//记录表1当前记录数
	NULL_698,			1,				//记录表2当前记录数
	NULL_698,	        1,				//记录表3当前记录数
	NULL_698,			1,				//记录表4当前记录数
};
//当前记录数-NULL
static const BYTE T_EventNowNum24Null[]		= 
{ 
	Structure_698,		4,
	NULL_698,			1,				//记录表1当前记录数
	Long_unsigned_698,	2,				//记录表2当前记录数
	Long_unsigned_698,  2,				//记录表3当前记录数
	Long_unsigned_698,	2,				//记录表4当前记录数
};
//当前记录数-NULL
static const BYTE T_EventNowNum24NullBNull[]		= 
{ 
	Structure_698,		4,
	NULL_698,			1,				//记录表1当前记录数
	Long_unsigned_698,	2,				//记录表2当前记录数
	NULL_698,			1,				//记录表3当前记录数
	Long_unsigned_698,	2,				//记录表4当前记录数
};


//当前值记录表 //返回长度需要特殊处理 请注意---jwh
static const BYTE T_EventNowTable24[]	= 
{ 
	Array_698,					0xff,
	Structure_698,				2,
	Double_long_unsigned_698,	4,		//事件发生次数
	Double_long_unsigned_698,	4,		//事件累计时间
};

//当前值记录表 //返回长度需要特殊处理 请注意---jwh
static const BYTE T_EventNowTable7[]	= 
{ 
	Structure_698,				2,
	Double_long_unsigned_698,	4,
	Double_long_unsigned_698,	4,
};
//事件配置参数
static const BYTE T_EventUNLUNLDU[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,
	Long_unsigned_698,	2,
	Double_long_698,	4,
	Unsigned_698,		1,
};
//事件配置参数
static const BYTE T_EventUNLU[]			= 
{ 
	Structure_698,		2,
	Long_unsigned_698,	2,
	Unsigned_698,		1,
};
//事件配置参数
static const BYTE T_EventUNLDU[]		= 
{ 
	Structure_698,		3,
	Long_unsigned_698,	2,
	Double_long_698,	4,
	Unsigned_698,		1,
};
//事件配置参数
static const BYTE T_EventUNLDDU[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,
	Double_long_698,	4,
	Double_long_698,	4,
	Unsigned_698,		1,
};
//事件配置参数
static const BYTE T_EventDU[]			= 
{ 
	Structure_698,		2,
	Double_long_698,	4,
	Unsigned_698,		1,
};
	//事件配置参数
static const BYTE T_EventDLU[]			= 
{ 
	Structure_698,		3,
	Double_long_698,	4,
	Long_unsigned_698,	2,
	Unsigned_698,		1,
};
//事件配置参数
static const BYTE T_EventUNDU[]			= 
{ 
	Structure_698,			    2,
	Double_long_unsigned_698,   4,
	Unsigned_698,		    	1,
};
//事件配置参数
static const BYTE T_EventLU[]			= 
{ 
	Structure_698,	2,
	Long_698,		2,
	Unsigned_698,	1,
};
//事件配置参数
static const BYTE T_EventU[]			= 
{ 
	Structure_698,	1,
	Unsigned_698,	1,
};
//事件配置参数 //获取不加tag时请注意使用
static const BYTE T_EventNull[]			= 
{ 
	Structure_698,	1,
	NULL_698,	    0,
};
//失压统计
static const BYTE T_EventLostVCount[]	= 
{ 
	Structure_698,				4,
	Double_long_unsigned_698,	4,
	Double_long_unsigned_698,	4,
	DateTime_S_698,			    7,
	DateTime_S_698,			    7,
};

//互感器线圈变更
static const BYTE T_EventRogowskiCount[]	= 
{ 
	Structure_698,				2,
	Long_unsigned_698,			2,
	Long_unsigned_698,			2,
};

//终端抄表失败事件
static const BYTE T_EventRCommFailCount[]	= 
{ 
	Structure_698,				2,
	Unsigned_698,				1,
	NULL_698,	    			0,
};

//事件上报状态 //返回长度需要特殊处理 请注意---jwh
static const BYTE T_EventReportStatus[]	= 
{ 
	Array_698,			0xff,
	Structure_698,		2,
	OAD_698,			4,
	Unsigned_698,		1,
};

//增加数据类型请查看源数据和加TAG是否支持
static const TEventRecordObj EventRecordObj[] =
{
#if( MAX_PHASE != 1 )
	{  0x3000, 	eclass24,	T_EventUNLUNLDU	},	//电能表失压事件
	// {  0x3001, 	eclass24,	T_EventUNLU	   	},	//电能表欠压事件
	// {  0x3002, 	eclass24,	T_EventUNLU   	},	//电能表过压事件
	// {  0x3003, 	eclass24,	T_EventUNLDU	},	//电能表断相事件
	// {  0x3004, 	eclass24,	T_EventUNLDDU	},	//电能表失流事件
	// {  0x3006, 	eclass24,	T_EventUNLDU	},	//电能表断流事件
	// {  0x3007, 	eclass24,	T_EventDU		},	//电能表功率反向事件
	// {  0x3008, 	eclass24,	T_EventDU		},	//电能表过载事件
	// {  0x3009, 	eclass7,	T_EventUNDU		},	//电能表正向有功需量超限事件
	// {  0x300A, 	eclass7,	T_EventUNDU		},	//电能表反向有功需量超限事件
	// {  0x300B, 	eclass24,	T_EventUNDU		},	//电能表无功需量超限事件
	// {  0x303B, 	eclass24,	T_EventLU		},	//电能表功率因数超下限事件
	// {  0x300D, 	eclass7,	T_EventNull		},	//电能表全失压事件
	// {  0x300E, 	eclass7,	T_EventU		},	//电能表辅助电源掉电事件
	// {  0x300F, 	eclass7,	T_EventU		},	//电能表电压逆相序事件
	// {  0x3010, 	eclass7,	T_EventU		},	//电能表电流逆相序事件
	// {  0x3014, 	eclass7,	T_EventNull		},	//电能表需量清零事件
	// {  0x301C, 	eclass7,	T_EventNull		},	//电能表开端钮盒事件
	// {  0x301D, 	eclass7,	T_EventLU		},	//电能表电压不平衡事件
	// {  0x301E, 	eclass7,	T_EventLU		},	//电能表电流不平衡事件
	// {  0x3023, 	eclass7,	T_EventNull		},	//电能表无功组合方式编程事件
	// {  0x302A, 	eclass7,	T_EventNull		},	//电能表恒定磁场干扰事件
	// {  0x302D, 	eclass7,	T_EventLU		},	//电能表电流严重不平衡事件	
#endif
    // {  0x3021,  eclass7,    T_EventNull     },  //电能表节假日编程事件
	// {  0x3005, 	eclass24,	T_EventDU		},	//电能表过流事件
	// {  0x3011, 	eclass7,	T_EventU		},	//电能表掉电事件
	// {  0x3012, 	eclass7,	T_EventNull		},	//电能表编程事件
	// {  0x3013, 	eclass7,	T_EventNull		},	//电能表清零事件
	// {  0x3015, 	eclass7,	T_EventNull		},	//电能表事件清零事件
	// {  0x3016, 	eclass7,	T_EventNull		},	//电能表校时事件
	// {  0x3017, 	eclass7,	T_EventNull		},	//电能表时段表编程事件
	// {  0x3018, 	eclass7,	T_EventNull		},	//电能表时区表编程事件
	// {  0x3019, 	eclass7,	T_EventNull		},	//电能表周休日编程事件
	// {  0x301A, 	eclass7,	T_EventNull		},	//电能表结算日编程事件
	// {  0x301B, 	eclass7,	T_EventNull		},	//电能表开盖事件
	// {  0x301F, 	eclass7,	T_EventNull		},	//电能表跳闸事件
	// {  0x3020, 	eclass7,	T_EventNull		},	//电能表合闸事件
	// {  0x3022, 	eclass7,	T_EventNull		},	//电能表有功组合方式编程事件
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{  0x3024, 	eclass7,	T_EventNull		},	//电能表费率参数表编程事件
	{  0x3025, 	eclass7,	T_EventNull		},	//电能表阶梯表编程事件
	#endif
	{  0x3026, 	eclass7,	T_EventNull		},	//电能表密钥更新事件
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{  0x3027, 	eclass7,	T_EventNull		},	//电能表异常插卡事件
	{  0x3028, 	eclass7,	T_EventNull		},	//电能表购电记录
	{  0x3029, 	eclass7,	T_EventNull		},	//电能表退费记录
	#endif	
	// {  0x302B, 	eclass7,	T_EventNull		},	//电能表负荷开关误动作事件
	// {  0x302C, 	eclass7,	T_EventNull		},	//电能表电源异常事件
	// {  0x302E, 	eclass7,	T_EventNull		},	//电能表时钟故障事件
	// {  0x302F, 	eclass7,	T_EventU		},	//电能表计量芯片故障事件
	// {  0x303C, 	eclass7,	T_EventNull		},	//电能表广播校时事件
	// {  0x3040, 	eclass7,	T_EventDLU		},	//电能零线电流不平衡事件
	// {  0x3320, 	eclass8,    T_EventRTable	},	//新增事件列表
	// {  0x3300, 	eclassTag,	T_EventNull		},	//事件通道上报状态
	// {  0x3302, 	eclassTag,  T_EventNull		},	//编程记录事件单元
	// {  0x3308, 	eclassTag,  T_EventNull		},	//电能表需量超限事件单元
	// {  0x330C, 	eclassTag,  T_EventNull		},	//事件清零事件记录单元
	// {  0xBD00, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD01, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD02, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD03, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD04, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD05, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD06, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD07, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD08, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0xBD09, 	eclass7,	T_EventNull		},	//拓展事件
	// {  0x30E0, 	eclass7,	T_EventNull		},	//充电桩告警
	// {  0x30E1, 	eclass7,	T_EventNull		},	//充电桩故障
	// {  0x30E2, 	eclass7,	T_EventNull		},	//充电桩通信异常
	// {  0x3053, 	eclass7,	T_EventRogowskiCount},	//罗氏线圈变更事件
	// {  0x310F, 	eclass7,	T_EventRCommFailCount},	//终端抄表失败事件
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{  0x3310, 	eclassTag,  T_EventNull		},	//3310	8	异常插卡事件记录单元
	{  0x3311, 	eclassTag,  T_EventNull		},	//3311	8	退费事件记录单元
	#endif
	{  0x3104, 	eclass7,	T_EventNull		},	//终端状态量变位事件
    #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
    {  0x3111, 	eclass7,	T_EventNull		}, //发现未知电能表事件
    #endif
    #if (SEL_TOPOLOGY == YES )
    {  0x3E03, 	eclass7,	T_EventNull		}, //特征电流信号识别结果事件
    {  0x4E07, 	eclass7,	T_EventNull		}, //特征电流信号识别结果
    #endif
};
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  获取冻结参数
//         
//参数:      
//          *pOAD[in]：	OAD                 
//          *pBuf[out]：输出数据
//         
//返回值:    返回独居长度及成员个数（Array_Struct_Num）
//         
//备注内容:  关联对象属性表
//--------------------------------------------------
static WORD GetFreezeData9( BYTE *pOAD, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	TTwoByteUnion OI;
	WORD Result;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Result = 0x8000;
	
	switch( ClassAttribute )
	{
		case 3://冻结关联对象属性表
			Result = api_ReadFreezeAttribute( OI.w,ClassIndex,(MAX_FREEZE_ATTRIBUTE*12+50),pBuf );
			if( (Result==0) || (Result==0x8000) )
			{
				return Result;
			}
			
			TagPara.Array_Struct_Num = (Result/8);
			break;

		default:
			return 0x8000;
	}

	return  Result;
}

//--------------------------------------------------
//功能描述:  冻结数据加TAG
//         
//参数:      
//         	*pOAD[in]：		OAD         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容:只对冻结的属性3 关联对象属性表  
//--------------------------------------------------
static WORD AddTagFreezeData9(BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 3://关联对象属性表
			Tag = T_FreezeRecord3;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
            if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			if( TagPara.Array_Struct_Num == 0 )//关联对象属性表单独处理 返回01 00 
			{
				
				OutBuf[0] = Array_698;
				OutBuf[1] = 0;
				Lenth = 2;
			} 
			else
			{
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			
			break;

		default:
			return 0x8000;
	}	

	return Lenth;
}

//--------------------------------------------------
//功能描述:  根据表格查找事件记录OAD
//         
//参数:      OI[in]：OI
//         
//返回值:    事件索引
//         
//备注内容:  无
//--------------------------------------------------
static BYTE SearchEventRecordOAD( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(EventRecordObj)/sizeof(TEventRecordObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == EventRecordObj[i].OI )
		{
			return i;
		}
	}

	if( i == Num )//未找到
	{
		return 0x80;
	}
	
	return 0x80;
}

//--------------------------------------------------
//功能描述: 对事件单元添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD    
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]： 申请的缓冲长度        
//          *OutBuf[in]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容:	事件上报状态
//			编程对象列表
//			超限期间需量最大值
//			事件清零列表
//--------------------------------------------------
static WORD AddTagEventUnit(BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0x8000;
	
	switch( OI.w )
	{
		case 0x3300://事件上报状态
			if( (ClassIndex==0) && (ClassAttribute==2) )//只支持对属性2进行加TAG
			{
				TagPara.Array_Struct_Num = InBuf[0];
				Lenth = GetTypeLen( eTagData, T_EventReportStatus);
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_EventReportStatus, InBuf+1, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			break;

		case 0x3302:
			if( ClassIndex == 6 )//编程对象列表
			{
				TagPara.Array_Struct_Num = InBuf[0];
				Lenth = GetTypeLen( eTagData, T_EventRTable);
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_EventRTable, InBuf+1, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}		
			}
			break;

		case 0x3308:
			if( ClassIndex == 6 )//超限期间需量最大值
			{
				Lenth = GetTypeLen( eTagData, T_UNDoubleLong );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_UNDoubleLong, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
			}
			else if( ClassIndex == 7 )//超限期间需量最大值发生时间
			{
				Lenth = GetTypeLen( eTagData, T_DateTimS );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_DateTimS, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			break;

		case 0x330c:
			if( ClassIndex == 6 )//事件清零列表
			{
				TagPara.Array_Struct_Num = InBuf[0];
				Lenth = GetTypeLen( eTagData, T_EventClearTable);
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_EventClearTable, InBuf+1, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}			
			}
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )
		case 0x3310:
			//0X06021033			//异常插卡事件记录单元0x3310--卡序列号	octet-string，
			if( ClassIndex == 6 )
			{
				Lenth = GetTypeLen( eTagData, T_OctetString8 );	
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_OctetString8, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}	
			}
			//0X07021033			//异常插卡事件记录单元0x3310--插卡错误信息字	unsigned，
			else if( ClassIndex == 7 )
			{
				Lenth = GetTypeLen( eTagData, T_Unsigned );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_Unsigned, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			//命令头保存时保存 CH,Cla,Ins,P1,P2,P31,P32,其中CH:0x01--ESAM 0x00--CARD,esam命令头为CH后的6字节，卡为CH后的5字节
			//0X08021033			//异常插卡事件记录单元0x3310--插卡操作命令头	octet-string，
			else if( ClassIndex == 8 )
			{
				Lenth = GetTypeLen( eTagData, T_OctetString7 );	
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_OctetString7, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}	
			}
			//0X09021033			//异常插卡事件记录单元0x3310--插卡错误响应状态  long-unsigned，
			else if( ClassIndex == 9 )
			{
				Lenth = GetTypeLen( eTagData, T_UNLong );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_UNLong, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}		
			}
			break;
		case 0x3311:
			if( ClassIndex == 6 )//0X06021133--退费事件记录单元0x3311--退费金额      double-long-unsigned（单位：元，换算：-2）
			{
				Lenth = GetTypeLen( eTagData, T_UNDoubleLong );	
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( T_UNDoubleLong, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}	
			}
			break;
		#endif//#if( PREPAY_MODE == PREPAY_LOCAL )	
		default:
			break;
	}
	
	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取事件（class7）属性数据（不带Tag）
//         
//参数:     
//          *pOAD[in]：	OAD         
//          Sch[in]：   EventRecordObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 处理除属性2外的属性
//--------------------------------------------------
static WORD GetEventData7( BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex;
	#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录
	BYTE i, Num;
	#endif
	WORD Type,Result,Lenth;
	BYTE *pBufBak = pBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 3://事件关联对象属性表
			Result = api_ReadEventAttribute( OI.w,ClassIndex,(MAX_EVENT_PROBUF_LEN),pBuf );
			if( (Result==0) || (Result==0x8000) )
			{
				return Result;
			}
			
			TagPara.Array_Struct_Num = (Result/sizeof(DWORD));
			pBuf += Result;
			break;

		case 4://当前记录数
		case 5://最大记录数
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			Type = ClassAttribute-4;
			Result = api_ReadEventRecordNo( OI.w,0,eEVENT_CURRENT_RECORD_NO+Type,pBuf );
			if( Result == FALSE )
			{
				return 0x8000;
			}
			
			Lenth = GetTypeLen( eData, T_UNLong);
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			pBuf += Lenth;
			break;

		case 6://配置参数
			if( ClassIndex > EventRecordObj[Sch].ConfigPara[1] )
			{
				return 0x8000;
			}

			Result = api_ReadEventPara( OI.w, ClassIndex, (BYTE *)&Lenth, pBuf );
			if(Result == FALSE)
			{
				return 0x8000;
			}
			pBuf += Lenth;
			
			break;

		case 7://当前值记录表
			#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录
			if( OI.w == 0x3023 )//电能表无功组合方式编程事件
			{
				if( ClassIndex > 2 )//默认当前值记录表不能超过
				{
					return 0x8000;
				}
				
				Num = ( ClassIndex==0 ) ? 2 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				for( ; i<Num; i++)
				{
					Result = api_ReadCurrentRecordTable_Q_Combo(i,pBuf);//读无功组合方式1特征字、无功组合方式2特征字编程次数；
					if( Result == FALSE )
					{
						return 0x8000;
					}
					pBuf += 9;
				}
				break;	
			}
			else
			#endif//#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录	
			{		
				if( ClassIndex > 1 )//默认当前值记录表不能超过
				{
					return 0x8000;
				}
	
				Result = api_ReadCurrentRecordTable(OI.w,0,1,pBuf);
				if(Result == 0)
				{
					return 0x8000;
				}
				
				pBuf += 9;
			}
			break;

		case 8://上报标识
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			Result = api_ReadEventReportMode( eReportMode, OI.w, pBuf );
			if(Result == FALSE)
			{
				return 0x8000;
			}
			
			pBuf += 1;
			break;

		case 9://有效标识
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			pBuf[0] = 1;			
			pBuf += 1;
			break;
		case 10://时间状态记录表∷=array 时间状态
			#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录
			if( OI.w == 0x3023 )//电能表无功组合方式编程事件
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				Num = ( ClassIndex==0 ) ? 2 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				for( ; i<Num; i++)
				{
					*(pBuf++) = i;
					memset( pBuf, 0x00, 8 );
					Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PrgQCombo_CurrentEventStatus[i] ),sizeof(TRealTimer),pBuf );
					if( Result == FALSE )
					{
						return 0x8000;
					}
					if( (pBuf[0] == 0x00)&&(pBuf[1] == 0x00)&&(pBuf[2] == 0x00)&&(pBuf[3] == 0x00) )//年月日均为0
					{
						pBuf += 2;//发生时间为1字节NULL,结束时间1字节为NULL
					}
					else
					{		
						pBuf += 8;//发生时间7字节,结束时间1字节为NULL
					}
				}
			}
			else
			#endif
			{
				if( ClassIndex > 1 )//默认当前值记录表不能超过
				{
					return 0x8000;
				}
	
				Result = api_ReadCurrentRecordTable(OI.w,0,0,pBuf);
				if(Result == 0)
				{
					return 0x8000;
				}				
				pBuf += Result;//15;//发生源1字节为NULL，发生时间7字节,结束时间暂考虑7字节
			}
			break;
		case 11://上报方式
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Result = api_ReadEventReportMode( eReportMethod, OI.w, pBuf );
			if( Result == FALSE )
			{
				return 0x8000;
			}
			
			pBuf += 1;

			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )	
		case 29://属性11（非法插卡总次数）∷=double-long-unsigned
			if( OI.w != 0x3027 )//如果不是异常插卡
			{
				return 0x8000;
			}	
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			api_ReadIllegalCardNum(pBuf);
			pBuf += 4;
			
			break;	
		#endif	
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}
//--------------------------------------------------
//功能描述:  事件（class7）属性数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD
//			Sch[in]：		EventRecordObj中的事件索引         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 处理除属性2外的属性  
//--------------------------------------------------
static WORD AddTagEventData7(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录
	BYTE i, Num;
	#endif
	BYTE j;
	BYTE *pBufBak;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	pBufBak = OutBuf;
	
	switch( ClassAttribute )
	{
		case 3://关联对象属性表
			Tag = T_EventRTable;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			if( TagPara.Array_Struct_Num == 0 )//关联对象属性表单独处理 返回01 00 
			{
				
				OutBuf[0] = Array_698;
				OutBuf[1] = 0;
				Lenth = 2;
			} 
			else
			{
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			
			break;
		case 4://当前记录数
		case 5://最大记录数
		case 8://上报标识
		case 9://有效标识
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( ClassAttribute < 6 )
			{
				Tag = T_UNLong;
			}
			else if( ClassAttribute == 8 )
			{
				Tag = T_Enum;
			}
			else
			{
                Tag = T_Bool;
			}
			
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		case 6://配置参数
			if( ClassIndex > EventRecordObj[Sch].ConfigPara[1] )
			{
				return 0x8000;
			}

			Tag = EventRecordObj[Sch].ConfigPara;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
                
			if( OutBufLen < Lenth )
			{
				return 0	;
			}
			
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}			
			break;

		case 7://当前值记录表
			#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录
			if( OI.w == 0x3023 )//电能表无功组合方式编程事件
			{
				if( ClassIndex > 2 )//默认当前值记录表不能超过
				{
					return 0x8000;
				}
				
				Num = 1;
				if( ClassIndex == 0 )
				{
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 2;
					Num = 2;
				}
				Lenth = (ClassIndex == 0) ?	34 : 16;
				if( OutBufLen < Lenth )//数据长度不够
				{
	                return 0;
				}
				for( i=0; i<Num; i++ )
				{
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					*(OutBuf++) = Enum_698;
					*(OutBuf++) = InBuf[i*9];
					
					Result = GetRequestAddTag( T_EventNowTable7, (InBuf+1+(i*9)), OutBuf );	
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					OutBuf += 12;
				}
			}
			else
			#endif
			{	
				if( ClassIndex > 1 )//默认当前值记录表不能超过1
				{
					return 0x8000;
				}
	
				if( ClassIndex == 0 )
				{
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 1;
				}
	
				*(OutBuf++) = Structure_698;
				*(OutBuf++) = 2;

				*(OutBuf++) = InBuf[0];
				Lenth = (ClassIndex == 0) ?	17 : 15;	

				if( OutBufLen < Lenth )//数据长度不够
				{
	                return 0;
				}
	
				Result = GetRequestAddTag( T_EventNowTable7, InBuf+1, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}			
			}
			break;
		case 10://时间状态记录表∷=array 时间状态			//@@@@@@@
			if( OutBufLen < (6+8+8) )//缓冲大小按发生时间、结束时间均有考虑
			{
				return 0;
			}
			#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录
			if( OI.w == 0x3023 )//电能表无功组合方式编程事件
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}

				Num = 1;
				if( ClassIndex == 0 )
				{
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 2;
					Num = 2;
					if( OutBufLen < (2+(6+8+8)*2) )//缓冲大小按2相发生时间、结束时间均有考虑
					{
						return 0;
					}
				}
				
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				for( ; i<Num; i++)
				{
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					*(OutBuf++) = Enum_698;
					*(OutBuf++) = i;
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					InBuf++;
					for( j=0; j<2; j++ )
					{
						if( InBuf[0] == 0x00 )//没有发生时间、结束时间
						{
							*(OutBuf++) = 0;
							InBuf += 1;
						}
						else
						{
							*(OutBuf++) = DateTime_S_698;
							memcpy( (void *)OutBuf, (void *)InBuf, 7 );
							lib_InverseData(OutBuf, 2);
							OutBuf += 7;
							InBuf += 7;
						}
					}					
				}
			}
			else
			#endif
			{
				if( ClassIndex > 1 )//默认当前值记录表不能超过1
				{
					return 0x8000;
				}
	
				if( ClassIndex == 0 )
				{
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 1;                    
				}
	
				*(OutBuf++) = Structure_698;
				*(OutBuf++) = 2;
				*(OutBuf++) = InBuf[0];//发生源
                InBuf++;
				*(OutBuf++) = Structure_698;
				*(OutBuf++) = 2;
				for( j=0; j<2; j++ )
				{
					if( InBuf[0] == 0x00 )//没有发生时间、结束时间
					{
						*(OutBuf++) = 0;
						InBuf += 1;
					}
					else
					{
						*(OutBuf++) = DateTime_S_698;
						memcpy( (void *)OutBuf, (void *)InBuf, 7 );
						lib_InverseData(OutBuf, 2);
						OutBuf += 7;
						InBuf += 7;
					}
				}				
			}
			Lenth = OutBuf-pBufBak;
			break;
		case 11://上报方式
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

            Tag = T_Enum;
			
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )	
		case 29://属性11（非法插卡总次数）∷=double-long-unsigned
			if( OI.w != 0x3027 )//如果不是异常插卡
			{
				return 0x8000;
			}
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_UNDoubleLong;
		
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;	
		#endif
		default:
			return 0x8000;
	}	

	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取新增事件列表（class8）属性数据（不带Tag）
//         
//参数:
//			Ch[in]：	规约通道     
//          *pOAD[in]：	OAD         
//          Sch[in]：   EventRecordObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 属性2(新增事件列表),属性3(需上报事件对象列表)
//--------------------------------------------------
static WORD GetNewEventListData( BYTE Ch, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Result;
	BYTE* pBufBak = pBuf;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	
	switch( ClassAttribute )
	{
		case 2://事件关联对象属性表
		case 3://需上报事件对象列表
		    if( ClassAttribute == 2 )
		    {
                Result = GetReportOadList( eGetReportAllList, Ch, MAX_EVENT_PROBUF_LEN, pBuf );
                TagPara.Array_Struct_Num = (Result/sizeof(DWORD));
                
                if( ClassIndex != 0 )
                {
                	if( ClassIndex > TagPara.Array_Struct_Num )
                	{
						TagPara.Array_Struct_Num = 0;
						pBuf[0] = 0x00;
						Result = 1;
                	}
                	else
                	{
	                	TagPara.Array_Struct_Num = 1;
						memmove( pBuf, (pBuf+(sizeof(DWORD)*(ClassIndex-1))), sizeof(DWORD));
					}
                }

		    }
		    else
		    {
                Result = GetReportOIList( MAX_EVENT_PROBUF_LEN, pBuf );
				TagPara.Array_Struct_Num = (Result/sizeof(WORD));
				
				if( ClassIndex != 0 )
                {
                	if( ClassIndex > TagPara.Array_Struct_Num )
                	{
						return 0x8000;
                	}
                	TagPara.Array_Struct_Num = 1;
					memmove( pBuf, (pBuf+(sizeof(WORD)*(ClassIndex-1))), sizeof(WORD));
                }
		    }
			
			if( Result==0x8000 )
			{
				return Result;
			}			
			
			pBuf += Result;
			break;
		
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  新增事件列表数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD
//			Sch[in]：		EventRecordObj中的事件索引         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 属性2(新增事件列表),属性3(需上报事件对象列表)  
//--------------------------------------------------
static WORD AddTagNewEventList(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Result,Lenth;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 2://事件关联对象属性表
		case 3://需上报事件对象列表
		    if( ClassAttribute == 2 )
		    {
                Tag = T_EventRTable;
		    }
		    else
		    {
                Tag = T_EventReportTable;
		    }
			
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}

		    if(TagPara.Array_Struct_Num == 0)   // 新增事件列表单独处理 返回01 00
		    {
			    if(ClassIndex == 0)
			    {
				    OutBuf[0] = Array_698;
			    }
			    OutBuf[1] = 0;
			    Lenth = 2;
		    }
		    else
			{
                Result = GetRequestAddTag( Tag, InBuf, OutBuf );
                if( Result == 0x8000 )
                {
                    return 0x8000;
                }
			}

			break;

		default:
			return 0x8000;
	}	

	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取事件（class24）属性数据（不带Tag）
//         
//参数:     
//          *pOAD[in]：	OAD         
//          Sch[in]：   EventRecordObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 处理除属性6~9外的属性
//--------------------------------------------------
static WORD GetEventData24( BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num;
	TTwoByteUnion OI;	
	WORD Result,Lenth;
	const BYTE *Tag;
	BYTE *pBufBak = pBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 2://事件关联对象属性表
			Result = api_ReadEventAttribute( OI.w, ClassIndex, MAX_EVENT_PROBUF_LEN, pBuf );
			if( (Result==0) || (Result==0x8000) )
			{
				return Result;
			}
			
			TagPara.Array_Struct_Num = (Result/sizeof(DWORD));
			pBuf += Result;
			break;

		case 3://当前记录数
			if( ClassIndex > T_EventNowNum24[1] )
			{
				return 0x8000;
			}
			
			//潮流反向事件和无功需量超限事件当前记录数包含四项
			if( (OI.w==0x300b)|| (OI.w==0x3007)|| (OI.w==0x303B) )
			{
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				for( ; i<Num; i++)
				{
				    if( (OI.w==0x3007) && (MeterTypesConst == METER_3P3W) )//三相三线表潮流反向B线特殊处理
				    {
                        if( i == 2 )//B相传零
                        {
                            *(pBuf++) = 0;
                            continue;
                        }
				    }
				    if( (OI.w==0x303B) && (MeterTypesConst == METER_3P3W) )//三相三线表功率因数超下限ABC固定返回0
				    {
                        if( i != 0 )//A、B、C相传零
                        {
                            *(pBuf++) = 0;
                            continue;
                        }
				    }
					Result = api_ReadEventRecordNo( OI.w,i,eEVENT_CURRENT_RECORD_NO, pBuf );
					if( Result == FALSE )
					{
						return 0x8000;
					}
					pBuf += 2;
				}		
			}
			else
			{
				#if( MAX_PHASE != 1 )	//三相读取		
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;
				for( ; i<Num; i++)
				{
				  	if(MeterTypesConst == METER_3P3W)//三相三线表潮流反向B线特殊处理
				    {
                        if( i == 2 )//B相传零
                        {
                            *(pBuf++) = 0;
                            continue;
                        }
				    }
					if( i == 0 )//总传零
					{
						 *(pBuf++) = 0;
                         continue;
					}
					Result = api_ReadEventRecordNo( OI.w,i,eEVENT_CURRENT_RECORD_NO, pBuf );
					if( Result == FALSE )
					{
						return 0x8000;
					}
					pBuf += 2;
				}	
				#else//单相读取
				if( (ClassIndex==0) || (ClassIndex==2) )
				{
					Result = api_ReadEventRecordNo( OI.w,0,eEVENT_CURRENT_RECORD_NO, pBuf );
					if( Result == FALSE )
					{
						return 0x8000;
					}
					pBuf += 2;
				}
				else
				{
					*(pBuf++) = 0;
				}
				#endif//#if( MAX_PHASE != 1 )
			}
			break;
			
		case 4://最大记录数
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Result = api_ReadEventRecordNo( OI.w,0,eEVENT_MAX_RECORD_NO, pBuf );
			if( Result == FALSE )
			{
				return 0x8000;
			}

			pBuf +=2;
			break;
		case 5://配置参数
			if( ClassIndex > EventRecordObj[Sch].ConfigPara[1] )
			{
				return 0x8000;
			}

			Result = api_ReadEventPara( OI.w, ClassIndex, (BYTE *)&Lenth, pBuf );
			if(Result == FALSE)
			{
				return 0x8000;
			}
			pBuf += Lenth;			
			break;

		case 10://当前值记录表
			if( ClassIndex > 4 )//默认当前值记录表不能超过4
			{
				return 0x8000;
			}
			
			//功率反向事件和无功需量超限事件当前记录数包含四项
			if( (OI.w==0x300b)|| (OI.w==0x3007) || (OI.w==0x303B) )
			{
				TagPara.Array_Struct_Num = 4;
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				Lenth = GetTypeLen( eData, (BYTE*)&T_EventNowTable24[2]);
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                for( ; i <Num; i++)
				{
					Result = api_ReadCurrentRecordTable(OI.w,i,1,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += Lenth;
				}
			}
			else
			{
				#if( MAX_PHASE != 1 )	//加TAG特殊处理		
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				Lenth =  GetTypeLen( eData, (BYTE*)&T_EventNowTable24[2]);
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				for( ; i<Num; i++)
				{
					Result = api_ReadCurrentRecordTable(OI.w,i,1,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					
					if( i == 0 )//对总进行单独处理
					{
						pBuf += 1;
					}
					else
					{
						pBuf += Lenth;
					}
				}
				#else
				if( ClassIndex == 0 )
				{
					*(pBuf++) = 0;
					Result = api_ReadCurrentRecordTable(OI.w,1,1,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += 8;
					*(pBuf++) = 0;
					*(pBuf++) = 0;
				}
				else if( ClassIndex == 2 )
				{
					Result = api_ReadCurrentRecordTable(OI.w,1,1,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += 8;
				}
				else
				{
					*(pBuf++) = 0;
				}
				#endif//#if( MAX_PHASE != 1 )
			}
			break;

		case 11://上报标识
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			Result = api_ReadEventReportMode( eReportMode, OI.w, pBuf );
			if(Result == FALSE)
			{
				return 0x8000;
			}
			
			pBuf += 1;
			break;

		case 12://有效标识
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			pBuf[0] = 1;
			pBuf += 1;
			break;
		
		#if( SEL_EVENT_LOST_V == YES )
		case 13://失压统计(仅限失压事件)
			if( (ClassIndex>T_EventLostVCount[1]) || ( OI.w!=0x3000) )
			{
				return 0x8000;
			}
			
			Tag = T_EventLostVCount;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eData, Tag );
            if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
			
			Result = api_ReadEventLostVAtt(ClassIndex,pBuf);
			if(Result == FALSE)
			{
				return 0x8000;
			}
			
			pBuf += Lenth;
			break;
		#endif
		case 14://时间状态记录表∷=array structure			
			#if( MAX_PHASE != 1 )
			if( ClassIndex > 4 )//时间状态记录表不能超过4
			{
				return 0x8000;
			}
			#else			
			if( ClassIndex > 2 )//单相表时间状态记录表不能超过2
			{
				return 0x8000;
			}
			#endif
			
			//功率反向事件和无功需量超限事件当前记录数包含四项
			if( (OI.w==0x300b) || (OI.w==0x3007) || (OI.w==0x303B) )
			{
				TagPara.Array_Struct_Num = 4;
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;
				for( ; i<Num; i++)
				{
					Result = api_ReadCurrentRecordTable(OI.w,i,0,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += Result;//14;
				}
			}
			else
			{
				#if( MAX_PHASE != 1 )	//加TAG特殊处理		
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;
				for( ; i<Num; i++)
				{
					Result = api_ReadCurrentRecordTable(OI.w,i,0,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					
					if( i == 0 )//对总进行单独处理
					{
						pBuf += 1;
					}
					else
					{
						pBuf += Result;//Lenth;
					}
				}
				#else
				if( ClassIndex == 0 )
				{
					*(pBuf++) = 0;
					Result = api_ReadCurrentRecordTable(OI.w,1,0,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += Result;//14;
				}
				else if( ClassIndex == 2 )
				{
					Result = api_ReadCurrentRecordTable(OI.w,1,0,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += Result;//14;
				}
				#endif//#if( MAX_PHASE != 1 )
			}
			break;
		case 15://上报方式
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Result = api_ReadEventReportMode( eReportMethod, OI.w, pBuf );
			if( Result == FALSE )
			{
				return 0x8000;
			}
			
			pBuf += 1;

			break;
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  事件（class24）属性数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD
//			Sch[in]：		EventRecordObj中的事件索引         
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[in]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 处理除属性6~9外的属性  
//--------------------------------------------------
static WORD AddTagEventData24(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,i;
    BYTE j,Num;
	const BYTE* Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	BYTE *pBufBak;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	pBufBak = OutBuf;
	switch( ClassAttribute )
	{
		case 2://关联对象属性表
			Tag = T_EventRTable;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			if( TagPara.Array_Struct_Num == 0 )//关联对象属性表单独处理 返回01 00 
			{
				
				OutBuf[0] = Array_698;
				OutBuf[1] = 0;
				Lenth = 2;
			} 
			else
			{
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			break;
		
		case 3://当前记录数
			if( ClassIndex > 4 )
			{
				return 0x8000;
			}
			
			if( (OI.w==0x300b) || (OI.w==0x3007) || (OI.w==0x303B))//潮流反向事件和无功需量超限事件当前记录数包含四项
			{	
			    Tag = T_EventNowNum24;
			    
			    if( MeterTypesConst == METER_3P3W )
			    {
                    if( OI.w == 0x3007 )//潮流反向事件
                    {
                        Tag = T_EventNowNum24BNull;
                    }
			    }
				 if( MeterTypesConst == METER_3P3W )
			    {
                    if( OI.w == 0x303B )//潮流反向事件
                    {
                        Tag = T_EventNowNum24ABCNull;
                    }
			    }
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			else
			{
				#if( MAX_PHASE != 1 )
				Tag = T_EventNowNum24Null;
			    if( MeterTypesConst == METER_3P3W )
			    {
                    Tag = T_EventNowNum24NullBNull;
			    }
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				#else
				if( ClassIndex == 0 )
				{
					if( OutBufLen < 8 )
					{
						return 0;
					}
					Lenth = 8;
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 4;
					*(OutBuf++) = 0;
					*(OutBuf++) = Long_unsigned_698;
					*(OutBuf++) = InBuf[1];
					*(OutBuf++) = InBuf[0];
					*(OutBuf++) = 0;
					*(OutBuf++) = 0;
				}
				else if( ClassIndex == 2 )
				{
					if( OutBufLen < 3 )
					{
						return 0;
					}
					
					Lenth = 3;

					*(OutBuf++) = Long_unsigned_698;
					*(OutBuf++) = InBuf[1];
					*(OutBuf++) = InBuf[0];
				}
				else
				{
					if( OutBufLen < 1 )
					{
						return 0;
					}
					
					Lenth = 1;
					*(OutBuf++) = 0;
				}
				#endif//#if( MAX_PHASE != 1 )

			}
			
            break;
		case 10://当前值记录表
			if( ClassIndex > 4 )
			{
				return 0x8000;
			}
			TagPara.Array_Struct_Num = 4;
			
			if( (OI.w==0x300b) || (OI.w==0x3007) || (OI.w==0x303B))//潮流反向事件和无功需量超限事件当前记录数包含四项
			{
			    if( (MeterTypesConst==METER_3P3W) && (OI.w==0x3007) )//三相三潮流反向
			    {
                    if( ClassIndex == 0 )
                    {
                        if( OutBufLen < 39 )
                        {
                            return 0;
                        }
                        
                        Lenth = 39;
                        
                        *(OutBuf++) = Array_698;
                        *(OutBuf++) = 4;
                        for( i = 0; i < 4; i++ )
                        {
                            if( i == 2 )//B相
                            {
                                *(OutBuf++) = 0;
                                 InBuf += 8;
                                 continue;
                            }
                            
                            *(OutBuf++) = Structure_698;
                            *(OutBuf++) = 2;
                            *(OutBuf++) = Double_long_unsigned_698;
                            lib_ExchangeData(OutBuf, InBuf, 4);
                            OutBuf += 4;
                            InBuf += 4;
                            *(OutBuf++) = Double_long_unsigned_698;
                            lib_ExchangeData(OutBuf, InBuf, 4);
                            OutBuf += 4;
                            InBuf += 4;
                        }
                    }
                    else if( ClassIndex == 2 )//B相
                    {
                        if( OutBufLen < 1 )
                        {
                            return 0;
                        }
                        
                        Lenth = 1;
                        
                        *(OutBuf++) = 0;
                    }
                    else
                    {
                        if( OutBufLen < 12 )
                        {
                            return 0;
                        }
                        
                        Lenth = 12;
                        
                        *(OutBuf++) = Structure_698;
                        *(OutBuf++) = 2;
                        *(OutBuf++) = Double_long_unsigned_698;
                        lib_ExchangeData(OutBuf, InBuf, 4);
                        OutBuf += 4;
                        InBuf += 4;
                        *(OutBuf++) = Double_long_unsigned_698;
                        lib_ExchangeData(OutBuf, InBuf, 4);
                        OutBuf += 4;
                        InBuf += 4;
                    }
			    }
			    else
			    {
    				Tag = T_EventNowTable24;
    				Tag += GetTagOffsetAddr( ClassIndex, Tag);
    				Lenth = GetTypeLen( eTagData, Tag );
    				if( Lenth == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
    				if( OutBufLen < Lenth )
    				{
    					return 0	;
    				}
    				
    				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
    				if( Result == 0x8000 )
    				{
    					return 0x8000;
    				}
			    }

			}
			else
			{
#if( MAX_PHASE != 1 )			
				if( ClassIndex == 0 )
				{
					if( OutBufLen < 39 )
					{
						return 0;
					}
					
					Lenth = 39;
					
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 4;
					*(OutBuf++) = InBuf[0];
					InBuf++;
					for( i = 0; i < 3; i++ )
					{
					    if( MeterTypesConst == METER_3P3W )
					    {
                            if( i == 1 )
                            {
                                *(OutBuf++) = 0;
                                InBuf += 8;
                                Lenth = 28;
                                continue;
                                
                            }
					    }
						*(OutBuf++) = Structure_698;
						*(OutBuf++) = 2;
						*(OutBuf++) = Double_long_unsigned_698;
						lib_ExchangeData(OutBuf, InBuf, 4);
						OutBuf += 4;
						InBuf += 4;
						*(OutBuf++) = Double_long_unsigned_698;
						lib_ExchangeData(OutBuf, InBuf, 4);
						OutBuf += 4;
						InBuf += 4;
					}
				}
				else if( ClassIndex == 1 )
				{
					if( OutBufLen < 1 )
					{
						return 0;
					}
					
					Lenth = 1;
					
					*(OutBuf++) = 0;
				}
				else if( (MeterTypesConst==METER_3P3W) && (ClassIndex==3) )//三相三B相
				{
                    if( OutBufLen < 1 )
                    {
                        return 0;
                    }
                    
                    Lenth = 1;
                    
                    *(OutBuf++) = 0;
				}
				else
				{
					if( OutBufLen < 12 )
					{
						return 0;
					}
					
					Lenth = 12;
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					*(OutBuf++) = Double_long_unsigned_698;
					lib_ExchangeData(OutBuf, InBuf, 4);
					OutBuf += 4;
					InBuf += 4;
					*(OutBuf++) = Double_long_unsigned_698;
					lib_ExchangeData(OutBuf, InBuf, 4);
					OutBuf += 4;
					InBuf += 4;
				}
#else
				if( ClassIndex == 0 )
				{
					if( OutBufLen < 17 )
					{
						return 0;
					}
					
					Lenth = 17;
					
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 4;
					*(OutBuf++) = InBuf[0];
					InBuf++;

					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					*(OutBuf++) = Double_long_unsigned_698;
					lib_ExchangeData(OutBuf, InBuf, 4);
					OutBuf += 4;
					InBuf += 4;
					*(OutBuf++) = Double_long_unsigned_698;
					lib_ExchangeData(OutBuf, InBuf, 4);
					OutBuf += 4;
					InBuf += 4;

					*(OutBuf++) = 0;
					*(OutBuf++) = 0;

				}
				else if( ClassIndex == 2 )
				{
					if( OutBufLen < 12 )
					{
						return 0;
					}
					
					Lenth = 12;
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					*(OutBuf++) = Double_long_unsigned_698;
					lib_ExchangeData(OutBuf, InBuf, 4);
					OutBuf += 4;
					InBuf += 4;
					*(OutBuf++) = Double_long_unsigned_698;
					lib_ExchangeData(OutBuf, InBuf, 4);
					OutBuf += 4;
					InBuf += 4;
				}
				else
				{
					if( OutBufLen < 1 )
					{
						return 0;
					}
					
					Lenth = 1;
					
					*(OutBuf++) = 0;					
				}
#endif

			}
			break;
		
		case 5://配置参数
			if( ClassIndex > EventRecordObj[Sch].ConfigPara[1] )
			{
				return 0x8000;
			}

			Tag = EventRecordObj[Sch].ConfigPara;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}			
			break;
		
		case 4://最大记录数
		case 11://上报标识
		case 12://有效标识
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( ClassAttribute == 4 )
			{
				Tag = T_UNLong;
			}
			else if( ClassAttribute == 11 )
			{
				Tag = T_Enum;
			}
			else
			{
                Tag = T_Bool;
			}
			
			Lenth = GetTypeLen( eTagData, Tag );
            if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		case 13://失压统计(仅限失压事件)
			if( (ClassIndex>T_EventLostVCount[1]) || ( OI.w!=0x3000) )
			{
				return 0x8000;
			}
			
			Tag = T_EventLostVCount;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;	
		case 14://时间状态记录表∷=array structure			
			#if( MAX_PHASE != 1 )
			if( ClassIndex > 4 )//时间状态记录表不能超过4
			{
				return 0x8000;
			}
			#else			
			if( ClassIndex > 2 )//单相表时间状态记录表不能超过2
			{
				return 0x8000;
			}
			#endif
			
			if( OutBufLen < (2+8+8) )//缓冲大小先按读一相发生时间、结束时间均有考虑
			{
				return 0;
			}
			
			if( (OI.w==0x300b) || (OI.w==0x3007) || (OI.w==0x303B))//潮流反向事件和无功需量超限事件当前记录数包含四项
			{
				if( ClassIndex == 0 )
				{
					if( OutBufLen < (2+(2+8+8)*4) )//缓冲大小按4相发生时间、结束时间均有考虑
					{
						return 0;
					}
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 4;
				}
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;
				for( ; i<Num; i++)
				{
				    if( (MeterTypesConst==METER_3P3W) && (OI.w==0x3007) )//三相三潮流反向
				    {
                        if( i == 2 )//3相3线B相
                        {
                            *(OutBuf++) = 0;
                            for( j=0; j<2; j++ )
                            {
                                if( InBuf[0] == 0x00 )//没有发生时间、结束时间
                                {
                                    InBuf += 1;
                                }
                                else
                                {
                                    InBuf += 7;
                                }
                            }
                            continue;
                        }
				    }
				    
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					for( j=0; j<2; j++ )
					{
						if( InBuf[0] == 0x00 )//没有发生时间、结束时间
						{
							*(OutBuf++) = 0;
							InBuf += 1;
						}
						else
						{
							*(OutBuf++) = DateTime_S_698;
							memcpy( (void *)OutBuf, (void *)InBuf, 7 );
							lib_InverseData(OutBuf, 2);
							OutBuf += 7;
							InBuf += 7;
						}
					}
				}
			}
			else
			{
				#if( MAX_PHASE != 1 )			
				if( ClassIndex == 0 )
				{
					if( OutBufLen < (3+(2+8+8)*4) )//缓冲大小按3相发生时间、结束时间均有考虑
					{
						return 0;
					}
								
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 4;
					*(OutBuf++) = 0x00;
                    InBuf++;//总记录单相表会返回一个00
				}
				
				if( ClassIndex == 1 )//单独读总的，返回NULL
				{
					//if( OutBufLen < 1 )
					//{
					//	return 0;
					//}
					
					Lenth = 1;
					
					*(OutBuf++) = 0;
				}
				else//ClassIndex=0、2、3、4
				{
					Num = ( ClassIndex==0 ) ? 3 : 1; 
					for( i=0; i<Num; i++)
					{
                        if( MeterTypesConst == METER_3P3W )
                        {
                            if( i == 1 )//3相3线B相
                            {
                                *(OutBuf++) = 0;
                                for( j=0; j<2; j++ )
                                {
                                    if( InBuf[0] == 0x00 )//没有发生时间、结束时间
                                    {
                                        InBuf += 1;
                                    }
                                    else
                                    {
                                        InBuf += 7;
                                    }
                                }
                                continue;
                            }
                        }
						*(OutBuf++) = Structure_698;
						*(OutBuf++) = 2;
						for( j=0; j<2; j++ )
						{
							if( InBuf[0] == 0x00 )//没有发生时间、结束时间
							{
								*(OutBuf++) = 0;
								InBuf += 1;
							}
							else
							{
								*(OutBuf++) = DateTime_S_698;
								memcpy( (void *)OutBuf, (void *)InBuf, 7 );
								lib_InverseData(OutBuf, 2);
								OutBuf += 7;
								InBuf += 7;
							}
						}
					}
				}
				#else
				if( ClassIndex == 0 )
				{
					if( OutBufLen < (3+8+8) )//缓冲大小按总为NULL，1相发生时间、结束时间均有考虑
					{
						return 0;
					}
					
					Lenth = 17;
					
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 4;
					*(OutBuf++) = 0x00;
					InBuf++;//总记录单相表会返回一个00
				}
				
				if( ClassIndex == 1 )
				{
					//if( OutBufLen < 1 )
					//{
					//	return 0;
					//}
					*(OutBuf++) = 0;
					Lenth = 1;
				}
				else//ClassIndex = 0、2
				{	
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					for( j=0; j<2; j++ )
					{
						if( InBuf[0] == 0x00 )//没有发生时间、结束时间
						{
							*(OutBuf++) = 0;
							InBuf += 1;
						}
						else
						{
							*(OutBuf++) = DateTime_S_698;
							memcpy( (void *)OutBuf, (void *)InBuf, 7 );
							lib_InverseData(OutBuf, 2);
							OutBuf += 7;
							InBuf += 7;
						}
					}
					if( ClassIndex == 0 )
					{
						*(OutBuf++) = 0;
						*(OutBuf++) = 0;
					}
					
				}				
				#endif
			}
			Lenth = (OutBuf - pBufBak);
			break;
		case 15://有效标识
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

            Tag = T_Enum;
			
			Lenth = GetTypeLen( eTagData, Tag );
		    if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;
		default:
			return 0x8000;
	}	

	return Lenth;
}

//--------------------------------------------------
//功能描述: 获取只加TAG事件的数据长度
//         
//参数:     
//          DataType[in]：eData/eTagData
//          *pOAD[in]     OAD    
//          Number[in]：  成员数
//         
//返回值:   返回数据长度
//         
//备注内容:	事件上报状态
//			编程对象列表
//			超限期间需量最大值
//			事件清零列表 
//--------------------------------------------------
WORD GetProEventUnitLen( BYTE DataType, BYTE *pOAD, BYTE Number )
{
	BYTE ClassAttribute,ClassIndex;
	TTwoByteUnion OI;	
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0x8000;
	
	switch( OI.w )
	{
		case 0x3300://事件上报状态
			if( (ClassIndex==0) && (ClassAttribute==2) )//只支持对属性2进行加TAG
			{
				TagPara.Array_Struct_Num = Number;
				Lenth = GetTypeLen( DataType, T_EventReportStatus);
			}
			break;

		case 0x3302:
			if( ClassIndex == 6 )//编程对象列表
			{
				TagPara.Array_Struct_Num = Number;
				Lenth = GetTypeLen( DataType, T_EventRTable);
			}
			break;

		case 0x3308:
			if( ClassIndex == 6 )//超限期间需量最大值
			{
				Lenth = GetTypeLen( DataType, T_UNDoubleLong );			
			}
			else if( ClassIndex == 7 )//超限期间需量最大值发生时间
			{
				Lenth = GetTypeLen( DataType, T_DateTimS );
			}
			break;

		case 0x330c:
			if( ClassIndex == 6 )//事件清零列表
			{
				TagPara.Array_Struct_Num = Number;
				Lenth = GetTypeLen( DataType, T_EventClearTable);	
			}
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )
		case 0x3310:
			//0X06021033			//异常插卡事件记录单元0x3310--卡序列号	octet-string，
			if( ClassIndex == 6 )
			{
				Lenth = GetTypeLen( DataType, T_OctetString8 );		
			}
			//0X07021033			//异常插卡事件记录单元0x3310--插卡错误信息字	unsigned，
			else if( ClassIndex == 7 )
			{
				Lenth = GetTypeLen( DataType, T_Unsigned );		
			}
			//698.45 ESAM操作命令头为6字节（P2为两字节），卡操作命令头为5字节， 7816中命令头是5个字节的系列，这5个字节指定为CLA、INS、P1、P2、P3。??
			//0X08021033			//异常插卡事件记录单元0x3310--插卡操作命令头	octet-string，
			else if( ClassIndex == 8 )
			{
				Lenth = GetTypeLen( DataType, T_OctetString7 );		
			}
			//0X09021033			//异常插卡事件记录单元0x3310--插卡错误响应状态  long-unsigned，
			else if( ClassIndex == 9 )
			{
				Lenth = GetTypeLen( DataType, T_UNLong );		
			}
			break;
		case 0x3311:
			if( ClassIndex == 6 )//0X06021133--退费事件记录单元0x3311--退费金额      double-long-unsigned（单位：元，换算：-2）
			{
				Lenth = GetTypeLen( DataType, T_UNDoubleLong );		
			}
			break;
		#endif//#if( PREPAY_MODE == PREPAY_LOCAL )	

		default:
			break;
	}
	
	return Lenth;
}

//--------------------------------------------------
//功能描述:  读取冻结
//         
//参数:     
//         	DataType[in]：	eData/eTagData
//         	Dot[in]:		小数点        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:  	返回数据长度
//         
//备注内容: 只处理class9（属性3 关联对象属性表）
//--------------------------------------------------
WORD GetProFreezeRecord( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE Buf[MAX_FREEZE_ATTRIBUTE*12+50];
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	Lenth = 0x8000;

	if( (DataType==eData) || (DataType==eTagData) )
	{
		Lenth = GetFreezeData9( pOAD, Buf );
		if( Lenth == 0x8000 )
		{
			return Lenth;
		}
		if( DataType == eData )
		{
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			memcpy( OutBuf, Buf, Lenth );
		}
		else
		{
			Lenth = AddTagFreezeData9(pOAD, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )//冻结类不支持单独加TAG
	{
		return 0x8000;
	}
	else
	{
		return 0x8000;
	}

	return Lenth; 
}

//--------------------------------------------------
//功能描述:  读取（处理）事件属性数据
//         
//参数: 
//			Ch[in]：		规约通道    
//         	DataType[in]：	eData/eTagData
//         	Dot[in]:		小数点        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:  	返回数据长度
//         
//备注内容: 不处理 class7（属性2），class24（属性6~9）
//--------------------------------------------------
WORD GetProEventRecord( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE Sch;
	BYTE Buf[MAX_EVENT_PROBUF_LEN];//不使用事件最大数量宏定义是因为最大数量是动态的，事件数量小的情况下会出问题所以使用最大事件数量！！！！！！
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	Lenth = 0x8000;
	FindThisEventSaveOI(&OI.w,0,eEventMap);
	Sch = SearchEventRecordOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType==eData) || (DataType==eTagData) )
	{
		if( EventRecordObj[Sch].class == eclass24 )
		{
			Lenth = GetEventData24( pOAD, Sch, Buf );
		}
		else if( EventRecordObj[Sch].class == eclass7 )
		{
			Lenth = GetEventData7( pOAD, Sch, Buf );
		}
		else if( EventRecordObj[Sch].class == eclass8 )//新增事件列表
	    {
            Lenth = GetNewEventListData( Ch, pOAD, Sch, Buf);
	    }
		else
		{
			return 0x8000;
		}
		
		if( Lenth == 0x8000 )
		{
			return Lenth;
		}
		if( DataType == eData )
		{
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			memcpy( OutBuf, Buf, Lenth );
		}
		else
		{
			if( EventRecordObj[Sch].class == eclass24 )
			{
				Lenth = AddTagEventData24( pOAD, Sch, Buf, OutBufLen, OutBuf );
			}
			else if( EventRecordObj[Sch].class == eclass7 )
			{
				Lenth = AddTagEventData7( pOAD, Sch, Buf, OutBufLen, OutBuf );
			}
			else if( EventRecordObj[Sch].class == eclass8 )//新增事件列表
    	    {
                Lenth = AddTagNewEventList( pOAD, Sch, Buf, OutBufLen, OutBuf );
    	    }
    	    else
    	    {
                return 0x8000;
    	    }
		}
	}
	else if( DataType == eAddTag )//冻结类不支持单独加TAG
	{
		if( EventRecordObj[Sch].class == eclassTag )
		{
			Lenth = AddTagEventUnit(pOAD, InBuf, OutBufLen, OutBuf);
		}
	    else
	    {
            return 0x8000;
	    }
	}
	else
	{
		return 0x8000;
	}

	return  Lenth;
}

//--------------------------------------------------
//功能描述:   	处理主动上报事件列表
//         
//参数: 
//			OutBufLen[out]：	buf长度    
//         	OutBuf[out]：
//返回值:  	返回数据长度
//         
//备注内容: 不处理 class7（属性2），class24（属性6~9）
//--------------------------------------------------
WORD AddActiveReportList(WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Buf[MAX_PRO_BUF+30];
	const BYTE* Tag;
	WORD Lenth,OADLenth,Result;
	
	OADLenth = GetReportOadList( eGetReportActiveList, eCR, MAX_PRO_BUF, Buf );
	if( OADLenth == 0x8000 )
	{
		return 0x8000;
	}
	
    Tag = T_EventRTable;
    TagPara.Array_Struct_Num = (OADLenth/4);
	Lenth = GetTypeLen( eTagData, Tag );
    if( Lenth == 0x8000 )
    {
        return 0x8000;
    }
    
	if( OutBufLen < Lenth )
	{
		return 0x8000;
	}
	
	if( TagPara.Array_Struct_Num == 0 )//新增事件列表单独处理 返回01 00 
	{
        OutBuf[0] = Array_698;
        OutBuf[1] = 0;
        Lenth = 2;
	} 
	else
	{
        Result = GetRequestAddTag( Tag, Buf, OutBuf );
        if( Result == 0x8000 )
        {
            return 0x8000;
        }
	}

	api_BackupReportIndex( );

	return Lenth;
}


#endif//#if ( SEL_DLT698_2016_FUNC == YES)

