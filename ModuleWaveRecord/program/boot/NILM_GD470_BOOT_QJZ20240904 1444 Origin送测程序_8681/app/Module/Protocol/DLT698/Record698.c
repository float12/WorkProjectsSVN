//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.01.06
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

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

//当前值记录表 //返回长度需要特殊处理 请注意---jwh
static const BYTE T_EventNowTable24[]	= 
{ 
	Array_698,					0xff,
	Structure_698,				2,
	Double_long_unsigned_698,	4,		//事件发生次数
	Double_long_unsigned_698,	4,		//事件累计时间
};


//事件配置参数
static const BYTE T_EventLU[]			= 
{ 
	Structure_698,	2,
	Long_698,		2,
	Unsigned_698,	1,
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
	{  OAD_HARMONIC_U_DISTORTION, 		eclass24,  T_EventLU	},
	{  OAD_HARMONIC_I_DISTORTION, 		eclass24,  T_EventLU	},
	{  OAD_HARMONIC_U_OVERRUN, 			eclass24,  T_EventLU	},
	{  OAD_HARMONIC_I_OVERRUN, 			eclass24,  T_EventLU	},
	{  OAD_HARMONIC_P_OVERRUN, 			eclass24,  T_EventLU	},
	{  OAD_INTER_HARMONIC_U_OVERRUN, 	eclass24,  T_EventLU	},
	{  OAD_INTER_HARMONIC_I_OVERRUN, 	eclass24,  T_EventLU	},
	{  OAD_INTER_HARMONIC_P_OVERRUN, 	eclass24,  T_EventLU	},
	{  OAD_SHORT_FLICKER_U_OVERRUN, 	eclass24,  T_EventLU	},
	{  OAD_LONG_FLICKER_U_OVERRUN, 		eclass24,  T_EventLU	},
	{  OAD_SAG_U, 						eclass24,  T_EventLU	},
	{  OAD_SWELL_U, 					eclass24,  T_EventLU	},
	{  OAD_INTERRUPTION_U, 				eclass24,  T_EventLU	},
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
	BYTE ClassAttribute;
	// BYTE ClassIndex;
	TTwoByteUnion OI;
	WORD Result;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	// ClassIndex = pOAD[3];
	Result = 0x8000;
	
	switch( ClassAttribute )
	{
		case 3://冻结关联对象属性表
			// Result = api_ReadFreezeAttribute( OI.w,ClassIndex,(20*12+50),pBuf );
            Result = 0;
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
	ClassAttribute = (pOAD[2]&0x0f);
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
	ClassAttribute = (pOAD[2]&0x0f);
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

		default:
			break;
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
	// const BYTE *Tag;
	BYTE *pBufBak = pBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 2://事件关联对象属性表
			// Result = api_ReadEventAttribute( OI.w, ClassIndex, MAX_EVENT_PROBUF_LEN, pBuf );
            Result = 0;
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
			if( (OI.w==0x300b)|| (OI.w==0x3007) )
			{
//				Num = ( ClassIndex==0 ) ? 4 : 1; 
//				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
//				Num += i;

//				for( ; i<Num; i++)
//				{
//				    if( (OI.w==0x3007) && (MeterTypesConst == METER_3P3W) )//三相三线表潮流反向B线特殊处理
//				    {
//                        if( i == 2 )//B相传零
//                        {
//                            *(pBuf++) = 0;
//                            continue;
//                        }
//				    }
//				    
//					Result = api_ReadEventRecordNo( OI.w,i,eEVENT_CURRENT_RECORD_NO, pBuf );
//					if( Result == FALSE )
//					{
//						return 0x8000;
//					}
//					pBuf += 2;
//				}		
			}
			else
			{
				#if( MAX_PHASE != 1 )	//三相读取		
				Num = ( ClassIndex==0 ) ? 3 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				for( ; i<Num; i++)
				{
					// Result = api_ReadEventRecordNo( OI.w,i+1,eEVENT_CURRENT_RECORD_NO, pBuf );
                    Result = 0;
					if( Result == FALSE )
					{
						return 0x8000;
					}
					pBuf += 2;
				}	
				#else//单相读取
				if( (ClassIndex==0) || (ClassIndex==2) )
				{
					// Result = api_ReadEventRecordNo( OI.w,0,eEVENT_CURRENT_RECORD_NO, pBuf );
                    Result = 0;
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
			
			// Result = api_ReadEventRecordNo( OI.w,0,eEVENT_MAX_RECORD_NO, pBuf );
            Result = 0;
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

			// Result = api_ReadEventPara( OI.w, ClassIndex, (BYTE *)&Lenth, pBuf );
            Result = 0;
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
			if( (OI.w==0x300b)|| (OI.w==0x3007) )
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
					// Result = api_ReadCurrentRecordTable(OI.w,i,1,pBuf);
                    Result = 0;
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
					// Result = api_ReadCurrentRecordTable(OI.w,i,1,pBuf);
                    Result = 0;
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
					// Result = api_ReadCurrentRecordTable(OI.w,1,1,pBuf);
                    Result = 0;
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
					// Result = api_ReadCurrentRecordTable(OI.w,1,1,pBuf);
                    Result = 0;
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

//		case 11://上报标识
//			if( ClassIndex != 0 )
//			{
//				return 0x8000;
//			}

//			Result = ReadEventReportMode(OI.w,pBuf);
//			if(Result == FALSE)
//			{
//				return 0x8000;
//			}
//			
//			pBuf += 1;
//			break;

//		case 12://有效标识
//			if( ClassIndex != 0 )
//			{
//				return 0x8000;
//			}

//			pBuf[0] = 1;
//			pBuf += 1;
//			break;
		
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
			if( (OI.w==0x300b)|| (OI.w==0x3007) )
			{
				TagPara.Array_Struct_Num = 4;
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;
				for( ; i<Num; i++)
				{
					// Result = api_ReadCurrentRecordTable(OI.w,i,0,pBuf);
                    Result = 0;
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
					// Result = api_ReadCurrentRecordTable(OI.w,i,0,pBuf);
                    Result = 0;
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
					// Result = api_ReadCurrentRecordTable(OI.w,1,0,pBuf);
                    Result = 0;
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += Result;//14;
				}
				else if( ClassIndex == 2 )
				{
					// Result = api_ReadCurrentRecordTable(OI.w,1,0,pBuf);
                    Result = 0;
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += Result;//14;
				}
				#endif//#if( MAX_PHASE != 1 )
			}
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
	ClassAttribute = (pOAD[2]&0x0f);
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
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;
		
		case 3://当前记录数
			if( ClassIndex > 4 )
			{
				return 0x8000;
			}
			
			if( (OI.w==0x300b) || (OI.w==0x3007) )//潮流反向事件和无功需量超限事件当前记录数包含四项
			{	
			    Tag = T_EventNowNum24;
			    
//			    if( MeterTypesConst == METER_3P3W )
//			    {
//                    if( OI.w == 0x3007 )//潮流反向事件
//                    {
//                        Tag = T_EventNowNum24Null;
//                    }
//			    }
				
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
				if( ClassIndex == 0 )
				{
					if( OutBufLen < 12 )
					{
						return 0;
					}
					Lenth = 12;
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 4;
					*(OutBuf++) = 0;
					for(i=0;i<3;i++)
					{
//					    if( MeterTypesConst == METER_3P3W )
//					    {
//                            if( i == 1 )//B相
//                            {
//                                *(OutBuf++) = 0;
//                                Lenth = 10;
//                                continue;
//                            }
//					    }
						*(OutBuf++) = Long_unsigned_698;
						*(OutBuf++) = InBuf[i*2+1];
						*(OutBuf++) = InBuf[i*2];
					}
				}
				else
				{
					if( OutBufLen < 3 )
					{
						return 0;
					}
					
					Lenth = 3;
					if( ClassIndex == 0 )
					{
                        Lenth = 1;
                        *(OutBuf++) = 0;
					}
//                    else if( (MeterTypesConst == METER_3P3W) && (ClassIndex == 3) )//三相三B相
//                    {
//                        *(OutBuf++) = 0;
//                        Lenth = 1;
//                    }
                    else
                    {
    					*(OutBuf++) = Long_unsigned_698;
    					*(OutBuf++) = InBuf[1];
    					*(OutBuf++) = InBuf[0];
                    }
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
//					    if( MeterTypesConst == METER_3P3W )
//					    {
//                            if( i == 1 )
//                            {
//                                *(OutBuf++) = 0;
//                                InBuf += 8;
//                                Lenth = 28;
//                                continue;
//                                
//                            }
//					    }
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
//				else if( (MeterTypesConst==METER_3P3W) && (ClassIndex==3) )//三相三B相
//				{
//                    if( OutBufLen < 1 )
//                    {
//                        return 0;
//                    }
//                    
//                    Lenth = 1;
//                    
//                    *(OutBuf++) = 0;
//				}
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
			
			if( (OI.w==0x300b) || (OI.w==0x3007) )//潮流反向事件和无功需量超限事件当前记录数包含四项
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
//				    if( (MeterTypesConst==METER_3P3W) && (OI.w==0x3007) )//三相三潮流反向
//				    {
//                        if( i == 2 )//3相3线B相
//                        {
//                            *(OutBuf++) = 0;
//                            for( j=0; j<2; j++ )
//                            {
//                                if( InBuf[0] == 0x00 )//没有发生时间、结束时间
//                                {
//                                    InBuf += 1;
//                                }
//                                else
//                                {
//                                    InBuf += 7;
//                                }
//                            }
//                            continue;
//                        }
//				    }
				    
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
//                        if( MeterTypesConst == METER_3P3W )
//                        {
//                            if( i == 1 )//3相3线B相
//                            {
//                                *(OutBuf++) = 0;
//                                for( j=0; j<2; j++ )
//                                {
//                                    if( InBuf[0] == 0x00 )//没有发生时间、结束时间
//                                    {
//                                        InBuf += 1;
//                                    }
//                                    else
//                                    {
//                                        InBuf += 7;
//                                    }
//                                }
//                                continue;
//                            }
//                        }
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
	ClassAttribute = (pOAD[2]&0x0f);
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
	BYTE Buf[20*12+50];
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

