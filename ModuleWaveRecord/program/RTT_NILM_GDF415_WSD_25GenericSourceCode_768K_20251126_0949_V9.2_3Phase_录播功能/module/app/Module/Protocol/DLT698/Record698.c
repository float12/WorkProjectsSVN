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

extern TReportMode gReportMode;
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

typedef struct TEventRecordObj_t
{
	WORD				OI;				//698对象标识：OI对象标识
	BYTE         		class;			//归属类
	const BYTE			*ConfigPara;
}TEventRecordObj;

extern TReportPara ReportPara;
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

static const BYTE T_EventClearTable2[]	= 
{ 
	Array_698,		0XFF,
	OI_698  ,		2,
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
	DateTime_S_698,			    7,
	DateTime_S_698,			    7,
};
#if(MAX_PHASE == 3)
static const BYTE T_EventNowTable24_1[]	= 
{ 
	Array_698,					0xff,
		Structure_698,				2,
		Double_long_unsigned_698,	4,		//事件发生次数
		Double_long_unsigned_698,	4,		//事件累计时间
};
#endif

//事件配置参数
static const BYTE T_EventLU[]			= 
{ 
	Structure_698,	2,
	Long_698,		2,
	Unsigned_698,	1,
};
static const BYTE T_EventLU2[]			= 
{ 
	Structure_698,			2,
	Long_unsigned_698,		2,
	Long_unsigned_698,		2,
};
static const BYTE T_EventLU3[]			= 
{ 
	Structure_698,	2,
	Long_unsigned_698,		2,
	Long_unsigned_698,	2,
};
static const BYTE T_EventLU7[]			= 
{ 
	Structure_698,	1,
	Unsigned_698,	1,
};

static const BYTE T_EventLU_NULL[]			= 
{ 

	NULL_698,	1,
};
#if(PROTOCOL_VERSION == 25)
//特定设备事件属性6
static const BYTE T_SpecificEventLU6[]			= 
{ 
	Array_698,				0xff,
	Long_unsigned_698,		2,
};
#endif
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
	{  OAD_HARMONIC_U_OVERRUN, 		eclass24,  T_EventLU	},
	{  OAD_HARMONIC_I_OVERRUN, 		eclass24,  T_EventLU	},
	{  OAD_HARMONIC_P_OVERRUN, 		eclass24,  T_EventLU	},
	{  OAD_INTER_HARMONIC_U_OVERRUN, 	eclass24,  T_EventLU	},
	{  OAD_INTER_HARMONIC_I_OVERRUN, 	eclass24,  T_EventLU	},
	{  OAD_INTER_HARMONIC_P_OVERRUN, 	eclass24,  T_EventLU	},
	{  OAD_SHORT_FLICKER_U_OVERRUN, 	eclass24,  T_EventLU2	},
	{  OAD_LONG_FLICKER_U_OVERRUN, 		eclass24,  T_EventLU2	},
	{  OAD_SAG_U, 				eclass24,  T_EventLU3	},
	{  OAD_SWELL_U, 			eclass24,  T_EventLU3	},
	{  OAD_INTERRUPTION_U, 			eclass24,  T_EventLU3	},

	{  OAD_METER_CLEAR, 			eclass24,  T_EventLU_NULL	},
	{  OAD_JLCHIP_FAIL, 			eclass24,  T_EventLU7	},
	{  OAD_APP_UPDATE, 			eclass24,  T_EventLU_NULL	},
	{  OAD_ADJUST_TIME, 				eclass24,  T_EventLU_NULL	},
	#if(PROTOCOL_VERSION == 25)
	{  OAD_UNKNOWN_DEV, 				eclass24,  T_EventLU_NULL	},
	{  OAD_SPECIFIC_DEV, 				eclass24,  T_SpecificEventLU6	},
	#endif
	{  0x3320, 				eclass8,  T_EventRTable},

};

extern const TEventInfoTab EventInfoTab[];
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern BYTE GetSubEventIndex2(BYTE EventIndex);

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
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Result = 0x8000;
	
	switch( ClassAttribute )
	{
		case 3://冻结关联对象属性表
			Result = api_ReadFreezeAttribute( OI.w,ClassIndex,(20*12+50),pBuf );
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
//-----------------------------------------------
//函数功能: 读取事件上报模式字
//
//参数: 	
//			OI[in]:				事件OI
//			byReportMode[out]:	事件上报标识{不上报(0),事件发生上报(1),事件恢复上报(2),事件发生恢复均上报(3)}
//
//返回值:	TRUE/FALSE
//			
//备注:接口类7的属性8，接口类24的属性11
//-----------------------------------------------
BOOL ReadEventReportMode( WORD OI, BYTE byClass, BYTE *pBuf )
{
	BYTE i;
	
	if( GetEventIndex( OI, &i ) == FALSE )
	{
		return FALSE;
	}
	#if(PROTOCOL_VERSION == 25)
	if( (OI ==0x3013) || (OI == 0x302F) || ( OI == 0x3036) || ( OI == 0x3016) || ( OI == 0x309A) || ( OI == 0x309B))//zh
	#elif(PROTOCOL_VERSION == 24)
	if( (OI ==0x3013) || (OI == 0x302F) || ( OI == 0x3036) || ( OI == 0x3016))
	#endif
	{
		if(byClass == 8)
			pBuf[0] = gReportMode.byReportFlag[i];

		if(byClass == 11)
			pBuf[0] = gReportMode.byReportMode[i];

		if(byClass == 9)
			pBuf[0] = gReportMode.byEventValid[i];
	}
	else
	{
		if(byClass == 11)
			pBuf[0] = gReportMode.byReportFlag[i];
		
		if(byClass == 15)
			pBuf[0] = gReportMode.byReportMode[i];

		if(byClass == 12)
			pBuf[0] = gReportMode.byEventValid[i];
	}
	
	return TRUE;
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
			Result = api_ReadEventAttribute( OI.w, ClassIndex, MAX_EVENT_PROBUF_LEN, pBuf );
			if( (Result==0) || (Result==0x8000) )
			{
				return Result;
			}
			
			TagPara.Array_Struct_Num = (Result/sizeof(DWORD));
			pBuf += Result;
			break;

		case 3://当前记录数
			#if(PROTOCOL_VERSION == 25)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) || (OI.w == OAD_SPECIFIC_DEV))//zh
			#elif(PROTOCOL_VERSION == 24)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))//zh
			#endif
			{
				Result = api_ReadEventAttribute(OI.w, ClassIndex, MAX_EVENT_PROBUF_LEN, pBuf);
				if( (Result == 0) || (Result == 0x8000) )
				{
					return Result;
				}
				TagPara.Array_Struct_Num = (Result/sizeof(DWORD));
				pBuf += Result;
				break;
			}
					
LABLE:		if( ClassIndex > T_EventNowNum24[1] )
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
					Result = api_ReadEventRecordNo( OI.w,i+1,eEVENT_CURRENT_RECORD_NO, pBuf );
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
			#if(PROTOCOL_VERSION == 25)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) || (OI.w == OAD_SPECIFIC_DEV))//zh
			#elif(PROTOCOL_VERSION == 24)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))//zh
			#endif
			{
				goto LABLE;
			}

			{
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
			}
			break;
		case 5://配置参数
			#if(PROTOCOL_VERSION == 25)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) || (OI.w == OAD_SPECIFIC_DEV))//zh
			#elif(PROTOCOL_VERSION == 24)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))//zh
			#endif
			{
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
			}
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
		case 6:
			#if(PROTOCOL_VERSION == 25)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) || (OI.w == OAD_SPECIFIC_DEV))//zh
			#elif(PROTOCOL_VERSION == 24)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))//zh
			#endif
			{
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
			}
			break;
		case 7: //属性7，事件类
			#if(PROTOCOL_VERSION == 25)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) || (OI.w == OAD_SPECIFIC_DEV))//zh
			#elif(PROTOCOL_VERSION == 24)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))//zh
			#endif
			{
				if((ClassIndex == 0) || (ClassIndex == 1))
				{
					#if(PROTOCOL_VERSION == 25)
					if(OI.w == OAD_SPECIFIC_DEV)
					{
						Result = api_ReadCurrentRecordTable(OI.w, 1,1,pBuf);
						if(Result == 0)
						{
							return 0x8000;
						}
						pBuf += Result;
					}
					else
					#endif //#if(PROTOCOL_VERSION == 25)
					{
						*(pBuf++) = 0;
						Result = api_ReadCurrentRecordTable(OI.w, 1,1,pBuf);
						if(Result == 0)
						{
							return 0x8000;
						}
						pBuf += 6;
					}
				}
				else
				{
					*(pBuf++) = 0;
				}
			}
			
			break;
		case 10://当前值记录表
			if( ClassIndex > 4 )//默认当前值记录表不能超过4
			{
				return 0x8000;
			}
			
			//功率反向事件和无功需量超限事件当前记录数包含四项
			#if(PROTOCOL_VERSION == 25)
			if( (OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) || (OI.w == OAD_SPECIFIC_DEV))//zh
			#elif(PROTOCOL_VERSION == 24)
			if( (OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))
			#endif
			{
				TagPara.Array_Struct_Num = 1;
				Num = ( ClassIndex==0 ) ? 1 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				Lenth = GetTypeLen( eData, (BYTE*)&T_EventNowTable24[2]);
				if( Lenth == 0x8000 )
				{
					return 0x8000;
				}
				
					Result = api_ReadCurrentRecordTable(OI.w,1,0,pBuf);
					if( Result == 0 )
					{
						return 0x8000;
					}
					pBuf += Result;
			
			}
			else
			{
				#if( MAX_PHASE != 1 )	//加TAG特殊处理		
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				Lenth =  GetTypeLen( eData, (BYTE*)&T_EventNowTable24_1[2]);
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
		case 8:
		case 9:
		case 11://上报标识
		case 15:
		case 12:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			Result = ReadEventReportMode(OI.w,ClassAttribute,pBuf);
			if(Result == FALSE)
			{
				return 0x8000;
			}
			
			pBuf += 1;
			break;

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
	#if(PROTOCOL_VERSION == 25)
	TSpecificDevice SpcDev;
	#endif
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	pBufBak = OutBuf;
	switch( ClassAttribute )
	{
		case 2://关联对象属性表
LABALE1:                 
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
			#if(PROTOCOL_VERSION == 25)
			if( (OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) || (OI.w == OAD_SPECIFIC_DEV))//zh
			#elif(PROTOCOL_VERSION == 24)
			if( (OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))
			#endif
			{
				goto LABALE1;
			}
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
			else if(OI.w==0x3320)
			{
				Tag = T_EventClearTable2;
			
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
			if( ClassIndex > 1 )
			{
				return 0x8000;
			}
			#if(PROTOCOL_VERSION == 25)
			if ((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) )//zh
			#elif(PROTOCOL_VERSION == 24)
			if ((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))
			#endif
			{
				TagPara.Array_Struct_Num =1;
				if(ClassIndex == 0)
				{
					if( OutBufLen < 21 )
					{
						return 0;
					}
					
					Lenth = 7;
					InBuf+=1;
									
					//王选友 add 2022.109.30
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 1;
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					*(OutBuf++) = 0; //事件发生源
					
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					if( api_CheckClock((TRealTimer *)InBuf) == FALSE )
					{
						*(OutBuf++) = 0;
						//OutBuf += 1;
						//InBuf += 1;
						Lenth += 1;
					}
					else
					{
						*(OutBuf++) = DateTime_S_698;
						Lenth += 1;
						memcpy( (void *)OutBuf, (void *)InBuf, 7 );
						lib_InverseData(OutBuf, 2);
						OutBuf += 7;
						InBuf += 7;

						Lenth += 7;
					}
					if(OI.w == OAD_METER_CLEAR)
					{
						*(OutBuf++) = 0;
						//OutBuf += 1;
						//InBuf += 1;
						//Lenth = 16;
						Lenth += 1;
					}
					else
					{
						if( api_CheckClock((TRealTimer *)InBuf) == FALSE )
						{
							*(OutBuf++) = 0;
							//OutBuf += 1;
							//InBuf += 1;
							Lenth += 1;
						}
						else
						{
							*(OutBuf++) = DateTime_S_698;
							Lenth += 1;
							memcpy( (void *)OutBuf, (void *)InBuf, 7 );
							lib_InverseData(OutBuf, 2);
							OutBuf += 7;
							InBuf += 7;
							Lenth += 7;
						}
					}
				}
				else if( ClassIndex == 1 )
				{
					if( OutBufLen < 19 )
					{
						return 0;
					}
					
					Lenth = 5;
					InBuf+=1;
									
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					*(OutBuf++) = 0; //事件发生源
					
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					if( api_CheckClock((TRealTimer *)InBuf) == FALSE )
					{
						*(OutBuf++) = 0;
						//OutBuf += 1;
						//InBuf += 1;
						Lenth += 1;
					}
					else
					{
						*(OutBuf++) = DateTime_S_698;
						Lenth += 1;
						memcpy( (void *)OutBuf, (void *)InBuf, 7 );
						lib_InverseData(OutBuf, 2);
						OutBuf += 7;
						InBuf += 7;

						Lenth += 7;
					}
					if(OI.w == OAD_METER_CLEAR)
					{
						*(OutBuf++) = 0;
						//OutBuf += 1;
						//InBuf += 1;
						//Lenth = 16;
						Lenth += 1;
					}
					else
					{
						if( api_CheckClock((TRealTimer *)InBuf) == FALSE )
						{
							*(OutBuf++) = 0;
							//OutBuf += 1;
							//InBuf += 1;
							Lenth += 1;
						}
						else
						{
							*(OutBuf++) = DateTime_S_698;
							Lenth += 1;
							memcpy( (void *)OutBuf, (void *)InBuf, 7 );
							lib_InverseData(OutBuf, 2);
							OutBuf += 7;
							InBuf += 7;
							Lenth += 7;
						}
					}
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
				break;
			 }
			
			#if(PROTOCOL_VERSION == 25)
			else if (OI.w == OAD_SPECIFIC_DEV)
			{
				TagPara.Array_Struct_Num =2;
				if( ClassIndex == 0 )
				{
					if( OutBufLen < 23 )
					{
						return 0;
					}
					
					Lenth = 9;
					
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 1;	
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;

					*(OutBuf++) = Long_unsigned_698;
					memcpy(&OutBuf[0],&InBuf[0],2); //事件发生源

					OutBuf +=2;
					InBuf +=2;
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					if( api_CheckClock((TRealTimer *)InBuf) == FALSE )
					{
						*(OutBuf++) = 0;
						//OutBuf += 1;
						//InBuf += 1;
						Lenth += 1;
						
					}
					else
					{
						*(OutBuf++) = DateTime_S_698;
						Lenth += 1;
						memcpy( (void *)OutBuf, (void *)InBuf, 7 );
						lib_InverseData(OutBuf, 2);
						OutBuf += 7;
						InBuf += 7;

						Lenth += 7;
					}
					if(OI.w == OAD_METER_CLEAR)
					{
						*(OutBuf++) = 0;
						//OutBuf += 1;
						//InBuf += 1;
						//Lenth = 16;
						Lenth += 1;
					}
					else
					{
						if( api_CheckClock((TRealTimer *)InBuf) == FALSE )
						{
							*(OutBuf++) = 0;
							//OutBuf += 1;
							//InBuf += 1;
							Lenth += 1;
							
						}
						else
						{
							*(OutBuf++) = DateTime_S_698;
							Lenth += 1;
							memcpy( (void *)OutBuf, (void *)InBuf, 7 );
							lib_InverseData(OutBuf, 2);
							OutBuf += 7;
							InBuf += 7;
							Lenth += 7;
						}
					}
				}
				else if(ClassIndex == 1)
				{
					if( OutBufLen < 21 )
					{
						return 0;
					}
					
					Lenth = 7;
									
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;

					*(OutBuf++) = Long_unsigned_698;
					memcpy(&OutBuf[0],&InBuf[0],2); //事件发生源

					OutBuf +=2;
					InBuf +=2;
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					if( api_CheckClock((TRealTimer *)InBuf) == FALSE )
					{
						*(OutBuf++) = 0;
						//OutBuf += 1;
						//InBuf += 1;
						Lenth += 1;
						
					}
					else
					{
						*(OutBuf++) = DateTime_S_698;
						Lenth += 1;
						memcpy( (void *)OutBuf, (void *)InBuf, 7 );
						lib_InverseData(OutBuf, 2);
						OutBuf += 7;
						InBuf += 7;

						Lenth += 7;
					}
					if(OI.w == OAD_METER_CLEAR)
					{
						*(OutBuf++) = 0;
						//OutBuf += 1;
						//InBuf += 1;
						//Lenth = 16;
						Lenth += 1;
					}
					else
					{
						if( api_CheckClock((TRealTimer *)InBuf) == FALSE )
						{
							*(OutBuf++) = 0;
							//OutBuf += 1;
							//InBuf += 1;
							Lenth += 1;
							
						}
						else
						{
							*(OutBuf++) = DateTime_S_698;
							Lenth += 1;
							memcpy( (void *)OutBuf, (void *)InBuf, 7 );
							lib_InverseData(OutBuf, 2);
							OutBuf += 7;
							InBuf += 7;
							Lenth += 7;
						}
					}
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
				break;
			}
			#endif //#if(PROTOCOL_VERSION == 25)
			
			else
				TagPara.Array_Struct_Num =4;
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
			#if(PROTOCOL_VERSION == 25)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV) || (OI.w == OAD_SPECIFIC_DEV))
			#elif(PROTOCOL_VERSION == 24)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))
			#endif
			{
				Tag = T_UNLong;//T_EventLU7;
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
			}
		case 6:
			if( ClassIndex > EventRecordObj[Sch].ConfigPara[1] )
			{
				return 0x8000;
			}

			#if(PROTOCOL_VERSION == 25)
			if(OI.w == OAD_SPECIFIC_DEV)
			{
				api_GetSpecificDeviceID(&SpcDev); //从eep获取
				TagPara.Array_Struct_Num = SpcDev.bySpecificDeviceNum;
			}
			#endif
			
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
		case 8:
		case 9:
		case 11://上报标识
		case 12://有效标识
		case 15:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( ClassAttribute == 4 )
			{
				Tag = T_UNLong;
			}
			else if( ClassAttribute == 11 || ClassAttribute == 8 || ClassAttribute == 15)
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
        case 7://加Tag属性7，事件类
			#if(PROTOCOL_VERSION == 25)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME) || (OI.w == OAD_UNKNOWN_DEV))//zh
			#elif(PROTOCOL_VERSION == 24)
			if((OI.w == OAD_METER_CLEAR) || (OI.w == OAD_JLCHIP_FAIL) || (OI.w == OAD_APP_UPDATE) || (OI.w == OAD_ADJUST_TIME))
			#endif
			{
				TagPara.Array_Struct_Num = 2;
				if(ClassIndex == 0)
				{
					if(OutBufLen < 17)
					{
					return 0;
					}
					
					Lenth = 17;
					
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 1;
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					
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
				}
				else if(ClassIndex == 1)
				{
					if(OutBufLen < 15)
					{
					return 0;
					}
					
					Lenth = 15;
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					
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
				}
				else
				{
					if(OutBufLen < 1)
						{
						return 0;
						}
					Lenth = 1;
					*(OutBuf++) = 0;
				}
			}
			
			#if(PROTOCOL_VERSION == 25)
			else if(OI.w == OAD_SPECIFIC_DEV)
			{
				TagPara.Array_Struct_Num = 2;
				if(ClassIndex == 0)
				{
					if(OutBufLen < 19)
					{
					return 0;
					}
					
					Lenth = 19;
					
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 1;
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					
					*(OutBuf++) = Long_unsigned_698;
					memcpy(&OutBuf[0],&InBuf[0],2); //事件发生源

					OutBuf +=2;
					InBuf +=2;
					
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
				else if(ClassIndex == 1)
				{
					if(OutBufLen < 17)
					{
					return 0;
					}
					
					Lenth = 17;
					
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					
					*(OutBuf++) = Long_unsigned_698;
					memcpy(&OutBuf[0],&InBuf[0],2); //事件发生源

					OutBuf +=2;
					InBuf +=2;
					
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
					if(OutBufLen < 1)
					{
					return 0;
					}
					Lenth = 1;
					*(OutBuf++) = 0;
				}
			}
			#endif //#if(PROTOCOL_VERSION == 25)
			
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

TFollowReportStatusRam	FollowReportStatusRam;//电能表跟随上报状态字
TReportIndex	ReportIndex[MAX_COM_CHANNEL_NUM];//3320属性2（新增上报事件列表，只读）∷= array OAD
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 将事件列表中连续事件之间的0xff去掉，往最新数据方向移动为紧排列数据
//
//参数: 
//		bBuf[in/out]:	事件列表缓冲，排序的数据源和结果
//  	DeadEvtInd[in]:		被标记为无效事件的索引	
//		NewEvtInd[in]:		最新事件的索引	
//  	                		
//返回值:	无
//			
//备注: bBuf为长度= SUB_EVENT_INDEX_MAX_NUM 
//-----------------------------------------------
static void SortReportIndex(BYTE *bBuf,BYTE DeadEvtInd,BYTE NewEvtInd)
{
	BYTE  i,Dst,Src;
	
	Dst=DeadEvtInd;
	
	//将ff的去掉，重新排序
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		// 事件列表是向高索引增长的，推前一个事件的索引
		Src =((Dst== 0)?(SUB_EVENT_INDEX_MAX_NUM-1):(Dst-1));	
		
		// 循环必然从break退出,因为整个列表中已经确认DeadEvtInd处的数据会被覆盖
		if( (Src==NewEvtInd) || (bBuf[Src]==0xff) )
		{
			break;
		}	
		
		// 数据移动		
		bBuf[Dst] = bBuf[Src];
		
		// 更新一个位置		
		Dst=Src;
	}
	// 无论如何，上面移动之后必定会有一个需要设置为0xff的
	bBuf[Dst] = 0xff;// 没有把旧的记录抹掉，会造成结尾处都是旧的数据
}
//-----------------------------------------------
//函数功能: 刷新RAM中新增上报事件列表的CRC校验
//
//参数: 	
//  	Ch:0xff--所有通道 其它--单个通道  
//              	
//返回值:	无
//			
//备注:
//-----------------------------------------------
static void RefreshReportIndexRamCRC( BYTE Ch )
{
	BYTE i;
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( (Ch == 0xff) || (i == Ch) )
		{
			//3320属性2（新增上报事件列表，只读）∷= array OAD
			ReportIndex[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportIndex[i],sizeof(TReportIndex)-sizeof(DWORD));
		}
	}
}
//-----------------------------------------------
//函数功能: 清跟RAM中新增上报事件列表
//
//参数: 	无
//  	Ch:0xff--所有通道 其它--单个通道 
//        	
//返回值:	无
//			
//备注:
//-----------------------------------------------
void ClearReportIndexRam( BYTE Ch )
{
	BYTE i;	
	
	if( Ch == 0xff )
	{
		memset((BYTE *)&(ReportIndex[0]), 0xff, (sizeof(TReportIndex)*MAX_COM_CHANNEL_NUM));
	}
	else if( Ch < MAX_COM_CHANNEL_NUM )
	{
		memset((BYTE *)&(ReportIndex[Ch]), 0xff, sizeof(TReportIndex));
	}
	else
	{
		return;
	}		

	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( (Ch == 0xff) || (i == Ch) )
		{	
			ReportIndex[i].Pointer = 0;			
		}
	}
	RefreshReportIndexRamCRC(Ch);
}
//-----------------------------------------------
//函数功能: 判断RAM中新增上报事件列表中CRC校验是否正确，如果不正确清零队列
//
//参数: 	无
//  	Ch:0xff--所有通道 其它--单个通道 
//        	
//返回值:	无
//			
//备注:
//-----------------------------------------------
void ProJudgeReportIndexRamCRC( BYTE Ch )
{
	BYTE i;	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( (Ch == 0xff) || (i == Ch) )
		{
			if( ReportIndex[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportIndex[i],sizeof(TReportIndex)-sizeof(DWORD)) )
			{
				ClearReportIndexRam(i);
			}
		}
	}
}
//-----------------------------------------------
//函数功能: 设置新增事件列表
//
//参数: 
//		inSubEventIndex[in]:	时间索引号
//  	BeforeAfter[in]:		EVENT_START	BIT0	
//  	                		EVENT_END	BIT1
//返回值:	无
//			
//备注:3320的属性2，不是发生结束都具备的输入 EVENT_START+EVENT_END
//-----------------------------------------------
void SetReportIndex( BYTE inSubEventIndex, BYTE BeforeAfter )
{
	BYTE i,j,tEventIndex;
	
	if(inSubEventIndex == eSUB_EVENT_METER_CLEAR)//清零后，所有oad不上报
	{
		ClearReportIndexRam(0xff);
		return;
	}

	if( inSubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
	{
		return;
	}
    
    tEventIndex = SubEventInfoTab[inSubEventIndex].EventIndex;

	//是否为需上报OI
	if((gReportMode.byReportFlag[tEventIndex]&BeforeAfter) == 0x00 )
	{
		return;
	}
	
	ProJudgeReportIndexRamCRC(0xff);

	//添加新增事件列表不要判断跟随上报标志及事件上报标识				
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		//读出此通道所有新增事件
		
		//指针加1，将此事件置到此指针位置
		ReportIndex[i].Pointer++;
		if( ReportIndex[i].Pointer >= SUB_EVENT_INDEX_MAX_NUM )
		{
			ReportIndex[i].Pointer = 0;
		}
		
		//查找新增事件列表中是否有此事件
		for(j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++)
		{
			if(ReportIndex[i].Index[j] == inSubEventIndex)
			{
				ReportIndex[i].Index[j] = 0xff;
				ReportIndex[i].Index[ ReportIndex[i].Pointer ] = inSubEventIndex;
				break;
			}
		}
				
		if( j == SUB_EVENT_INDEX_MAX_NUM )
		{
			//新增序列里面没有的需要添加
			ReportIndex[i].Index[ReportIndex[i].Pointer] = inSubEventIndex;
		}
		else
		{
			SortReportIndex(ReportIndex[i].Index,j,ReportIndex[i].Pointer);//重新排序
		}
	}
	
	RefreshReportIndexRamCRC(0xff);
}

//-----------------------------------------------
//函数功能: 清指定通道新增事件列表中的指定事件
//
//参数: 
//		Ch[in]:				通道号
//  	inSubEventIndex[in]:事件索引号	
//  	                	
//返回值:	无
//			
//备注:单通道
//-----------------------------------------------
void ClrReportIndexChannel( BYTE Ch, BYTE inSubEventIndex )
{
	BYTE i;
	
	if( inSubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
	{
		return;
	}
	
	if( Ch >= MAX_COM_CHANNEL_NUM )
	{
		return;
	}
	
	ProJudgeReportIndexRamCRC(Ch);
		
	//读出对应通道的新增事件列表
			
	//找到要删除的事件序号，将其置为ff
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		if(ReportIndex[Ch].Index[i] == inSubEventIndex)
		{
			ReportIndex[Ch].Index[i] = 0xff;
			break;
		}
	}
	
	//若没找到对应事件，退出
	if( i == SUB_EVENT_INDEX_MAX_NUM )
	{
		return;
	}
	
	//若清掉的是当前指针，需要指针减1
	if( i == ReportIndex[Ch].Pointer )
	{
		if(ReportIndex[Ch].Pointer == 0)
		{
			ReportIndex[Ch].Pointer = SUB_EVENT_INDEX_MAX_NUM-1;
		}
		else
		{
			ReportIndex[Ch].Pointer--;
		}
		
		ReportIndex[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportIndex[Ch],sizeof(TReportIndex)-sizeof(DWORD));
	}
	else
	{
		SortReportIndex(ReportIndex[Ch].Index,i,ReportIndex[Ch].Pointer);//重新排序
	}
	
	RefreshReportIndexRamCRC(Ch);

}

//-----------------------------------------------
//函数功能: 清跟RAM中新增上报事件列表中的主动上报oad
//
//参数: 	无
//  	Ch:0xff--所有通道 其它--单个通道 
//        	
//返回值:	无
//			
//备注:
//-----------------------------------------------
void ClearAutoReportIndexRam( BYTE Ch )
{
	BYTE i = 0;
	TFourByteUnion tw;
	BYTE bEventIndex = 0;

		
	if(( Ch == 0xff ) || ( Ch < MAX_COM_CHANNEL_NUM ))
	{
		for(i = 0; i < ReportPara.OAD_Num; i++)
		{
			memcpy(&tw.w[0],&ReportPara.ReportOAD[i],sizeof(DWORD));

			if(GetEventIndex(tw.w[1], &bEventIndex) == TRUE) //找oi
			{
				ClrReportIndexChannel(Ch,GetSubEventIndex2(bEventIndex)); //目前负荷辨识模组单相不存在分相所以通过index找subindex是可以的。需要考虑三相模组的分相事件，这种情况是不唯一的，index可以找到多个subindex
			}
		}
	}
	else
	{
		return;
	}
}

//-----------------------------------------------
//函数功能: 刷新RAM中跟随上报状态字的CRC校验
//
//参数: 	无
//  	                	
//返回值:	无
//			
//备注:
//-----------------------------------------------
void api_RefreshFollowReportStatusCRC( void )
{
	FollowReportStatusRam.CRC32 = lib_CheckCRC32((BYTE*)&FollowReportStatusRam,sizeof(FollowReportStatusRam)-sizeof(DWORD));
}
//-----------------------------------------------
//函数功能: 清跟随上报状态字
//
//参数: 	无
//  	                	
//返回值:	无
//			
//备注:清所有通道的跟随上报状态字
//-----------------------------------------------
void ClearFollowReportStatusRam( void )
{
	memset( (void *)&FollowReportStatusRam, 0x00, sizeof(FollowReportStatusRam) );//清零RAM中的跟随上报状态字
	api_RefreshFollowReportStatusCRC();
}
//-----------------------------------------------
//函数功能: 清上报数据
//
//参数: 	无
//  	                	
//返回值:	无
//			
//备注:清所有通道的新增事件列表、指针
//-----------------------------------------------
void ClearReport( BYTE Type )
{
	ClearReportIndexRam(0xff);
	ClearFollowReportStatusRam();	
	
    if( Type == eEXCEPT_CLEAR_METER )//不清电表清零 产生新的电表主动上报
    {
        #if( SEL_PRG_INFO_CLEAR_METER == YES ) //电能表清零事件
        SetReportIndex( eSUB_EVENT_PRG_CLEAR_METER, EVENT_START+EVENT_END );
        #endif
    }
    else if( Type == eEXCEPT_CLEAR_METER_CLEAR_EVENT )
    {
        #if( SEL_PRG_INFO_CLEAR_EVENT == YES )//电能表事件清零事件
        SetReportIndex( eSUB_EVENT_PRG_CLEAR_EVENT, EVENT_START+EVENT_END );
        #endif
    }
}

//-----------------------------------------------
//函数功能: 根据事件索引号计算对应OAD
//
//参数: 	
//		Index[in]:	事件索引号
//		Buf[out]：	计算出的单个OAD 
//           	
//返回值:	无
//			
//备注:
//-----------------------------------------------
static void CalReportOad(BYTE Index, BYTE *Buf)
{
	BYTE Phase;
	TFourByteUnion tdw;
	
	if(Index >= SUB_EVENT_INDEX_MAX_NUM)
	{
		return;
	}
	
	Phase = SubEventInfoTab[Index].Phase;
	tdw.w[1] = EventInfoTab[ SubEventInfoTab[Index].EventIndex].OI;
	
	#if(PROTOCOL_VERSION == 25)
	if( (tdw.w[1]==0x3013) || (tdw.w[1]==0x302F) || (tdw.w[1]==0x3036)|| (tdw.w[1]==0x3016) || (tdw.w[1]==0x309A)|| (tdw.w[1]==0x309B))//zh
	#elif(PROTOCOL_VERSION == 24)
	if( (tdw.w[1]==0x3013) || (tdw.w[1]==0x302F) || (tdw.w[1]==0x3036)|| (tdw.w[1]==0x3016))
	#endif
	{
		//属性7
		tdw.b[1] = 0x02;
	}
	else
	{
		//属性24
		tdw.b[1] = Phase+0x06;
	}
	
	tdw.b[0] = 0x00;
	
	lib_ExchangeData(Buf,tdw.b,sizeof(DWORD));
}

//-----------------------------------------------
//函数功能: 获取跟随上报OAD
//
//参数: 	
//		Type[in]:	0--跟随上报调用	1--EVENTOUT使用
//		Ch[in]:		通道号
//		Buf[out]：	计算出的单个OAD 
//           	
//返回值:	TRUE--有跟随上报	FALSE--无跟随上报
//			
//备注:
//-----------------------------------------------
BOOL GetFirstReportOad2(BYTE Type, BYTE Ch, BYTE *Buf)
{
	BYTE i,Pointer;
	
	//判断通道Ch是否属于4300电气设备的属性10. 上报通道array OAD
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		//0x000202F2, 	//eIR
		//0x000209F2,	//eCR 
	/*wxy	if( ((gReportMode.ReportChannelOAD[i]<<8) == 0x0202f200) &&( (ChannelOAD[Ch]<<8) == 0x0202f200) )
		{
			break;
		}
		if( ((gReportMode.ReportChannelOAD[i]<<8) == 0x0209f200) &&( (ChannelOAD[Ch]<<8) == 0x0209f200) )
		{
			break;
		}	
		if( gReportMode.ReportChannelOAD[i] == ChannelOAD[Ch] )
		{
			break;
		}*/
	}

	if( i >= MAX_COM_CHANNEL_NUM )
	{
		return FALSE;		
	}
	
	ProJudgeReportIndexRamCRC(Ch);
	
	Pointer = ReportIndex[Ch].Pointer;
	
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{				
		if( ReportIndex[Ch].Index[Pointer] != 0xff )
		{
			//判断是否开启
			//tEventIndex = SubEventInfoTab[ tReport.Index[Pointer] ].EventIndex;
			//if(ReportMode.byReportMode[tEventIndex] & 0x03)
			{
				if(Type == 0)
				{
					CalReportOad(ReportIndex[Ch].Index[Pointer], Buf);
				}
				return TRUE;
			}
		}
		
		if(Pointer == 0)
		{
			Pointer = SUB_EVENT_INDEX_MAX_NUM-1;
		}
		else
		{
			Pointer--;
		}
	}
	
	return FALSE;
}
//-----------------------------------------------
//函数功能: 获取新增事件列表OAD
//
//参数: 	
//		Ch[in]:		通道号
//		Len[in]:	输入数据长度
//		Buf[out]：	保存数据的缓冲
//           	
//返回值:	返回数据长度 0x8000--输入Buf长度不足
//			
//备注:新增事件列表 属性2
//-----------------------------------------------
WORD GetReportOadList(BYTE Ch, WORD Len, BYTE *Buf)
{
	BYTE i,Pointer;
	WORD Offset;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return 0x8000;
    }
    
	ProJudgeReportIndexRamCRC(Ch);
	
	Offset = 0;
	Pointer = ReportIndex[Ch].Pointer;	
	
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		if( (Offset+sizeof(DWORD)) > Len )
		{
			return 0x8000;
		}
		
		if( ReportIndex[Ch].Index[Pointer] != 0xff )
		{
			CalReportOad(ReportIndex[Ch].Index[Pointer], Buf+Offset);
			Offset += sizeof(DWORD);
		}
		else//  SetReportIndex 中已经保证了无效0xff仅出现在结尾 ,所以这里一旦发现0xff就应该退出循环
		{
			break;
		}
		
		if(Pointer == 0)
		{
			Pointer = SUB_EVENT_INDEX_MAX_NUM-1;
		}
		else
		{
			Pointer--;
		}
	}
	
	return Offset;
}

//-----------------------------------------------
//函数功能: 获取需上报事件对象列表 OI
//
//参数: 	
//		Len[in]:	输入数据长度
//		Buf[out]：	array OI 
//           	
//返回值:	返回数据长度 0x8000--输入Buf长度不足
//			
//备注:新增事件列表3320 属性3 需上报事件对象列表
//-----------------------------------------------
WORD GetReportOIList(WORD Len, BYTE *Buf)
{
	BYTE i;
	WORD Offset;
	TTwoByteUnion tw;
	
	Offset = 0;
	
	for(i=0; i<eNUM_OF_EVENT_PRG; i++)
	{
		if(gReportMode.byReportFlag[i] & 0x03)
		{
			if( (Offset+sizeof(DWORD)) > Len )
			{
				return 0x8000;
			}
			if(EventInfoTab[i].OI != 0x3013)//最后33200300显示时不添加3013即可，不要改动gReportMode.byReportFlag
            {
                tw.w = EventInfoTab[i].OI;
                lib_ExchangeData(Buf+Offset, tw.b, sizeof(WORD));
                Offset += sizeof(WORD);
            }

		}
	}
	
	return Offset;
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
	BYTE ClassAttribute;
	WORD Result;
	BYTE* pBufBak = pBuf;
	
	ClassAttribute = (pOAD[2]&0x0f);
	
	switch( ClassAttribute )
	{
	case 2://事件关联对象属性表
	case 3://需上报事件对象列表
		if( ClassAttribute == 2 )
		{
			Result = GetReportOadList( Ch, MAX_EVENT_PROBUF_LEN, pBuf );
			TagPara.Array_Struct_Num = (Result/sizeof(DWORD));
		}
		else
		{
			Result = GetReportOIList( MAX_EVENT_PROBUF_LEN, pBuf );
			TagPara.Array_Struct_Num = (Result/sizeof(WORD));
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
			else if(EventRecordObj[Sch].class == eclass8)
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

