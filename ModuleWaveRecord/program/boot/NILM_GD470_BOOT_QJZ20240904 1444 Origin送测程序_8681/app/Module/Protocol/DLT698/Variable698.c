//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.27
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
	
static const BYTE T_EventNowData[]		= //事件当前值
{ 
	Structure_698, 				0x02, 
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4
};	

static const BYTE T_U[]	= { Array_698, 50, Double_long_unsigned_698, 4  };//电压数据标识
static const BYTE T_I[] = { Array_698, 50, Double_long_unsigned_698, 4  };//电流数据标识
static const BYTE T_P[]	= { Array_698, 50, Double_long_698, 4  };//功率数据标识

static const BYTE T_ModuleValue[]	= //电能质量模块电压,电流
{ 	
	Array_698,					MAX_PHASE,
	Double_long_unsigned_698,	4,
};

static const BYTE T_ModuleValueCOS[]   = //电能质量模块功率因数
{
    Array_698,                  MAX_PHASE,
    Long_698,                   4,
};
static const BYTE T_ModuleValueChange[]	= //电能质量模块电压波动
{ 	
	Array_698,					MAX_PHASE,
	Structure_698, 	 			2, 
	Double_long_unsigned_698,	4, 
	Double_long_unsigned_698,	4, 
};

static const BYTE T_VI_COUNT[]	= //电压电流统计
{ 
	Array_698, 				50,
	Structure_698, 			0x05, 
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
};

static const BYTE T_VChange_COUNT[]	= //电压电流统计
{ 
	Structure_698, 			0x05, 
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
};


static const BYTE T_P_ALL[]	= 
{ 
	Structure_698, 				2, 
	Double_long_unsigned_698, 	4,  
	Double_long_unsigned_698, 	4,  
};//功率数据标识

//增加数据类型请查看源数据和加TAG是否支持
static const TCommonObj VariableObj[] =
{

	{  0x2000,  &SU_4V,   	T_ModuleValue					},//电压
	{  0x2001,  &SU_4A,     T_ModuleValue                   },//电流
	{  0x200A,  &SU_3,      T_ModuleValueCOS                },//功率因数
	{  0x200F,  &SU_4V,   	T_UNDoubleLong					},//频率
	{  0x2026,  &SU_2PCT, 	T_Long							},//电压不平衡
	{  0x2027,  &SU_2PCT, 	T_Long							},//电流不平衡
	{  0x2035,  &SU_4V,   	T_ModuleValueChange				},//电压波动
	{  0x2036,  &SU_4V,   	T_ModuleValueChange				},//电压闪变
	{  0x2046,  &SU_4V,   	T_U								},//谐波电压
	{  0x2047,  &SU_4A,   	T_I								},//谐波电流
	{  0x2048,  &SU_4W,   	T_P								},//谐波功率
	{  0x2060,  &SU_4A,   	T_U								},//间谐波电流
	{  0x2061,  &SU_4V,   	T_I								},//间谐波电压
	{  0x2062,  &SU_4A,   	T_P								},//间谐波功率
	{  0x2063,  &SU_4V,   	T_VI_COUNT						},//本日谐波电压统计
	{  0x2064,  &SU_4A,   	T_VI_COUNT						},//本日谐波电流统计
	{  0x2065,  &SU_4V,   	T_VI_COUNT						},//本日间谐波电压统计
	{  0x2066,  &SU_4A,   	T_VI_COUNT						},//本日间谐波电流统计
	{  0x2067,  &SU_4A,   	T_VChange_COUNT					},//本日闪变统计
//加TAG专用
	{  0x201e,  &SU_00, 	T_DateTimS						},//事件发生时间
	{  0x2020,  &SU_00, 	T_DateTimS						},//事件结束时间
	{  0x2021,  &SU_00, 	T_DateTimS						},//数据冻结时间
	{  0x2022,  &SU_00, 	T_UNDoubleLong					},//事件记录序号
	{  0x2023,  &SU_00, 	T_UNDoubleLong					},//冻结记录序号
	{  0x2024,  &SU_00, 	T_Enum							},//事件发生源
	{  0x2025,  &SU_00, 	T_EventNowData					},//事件当前值
};								
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  根据OI查找对应变量索引
//         
//参数:      OI[in]:	OI
//         
//返回值:    变量数组索引号
//         
//备注:  
//--------------------------------------------------
static BYTE SearchVariableOAD( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(VariableObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == VariableObj[i].OI )
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
//功能描述:  获取变量数据
//         
//参数:      
//			Dot[in]:		获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//			*pOAD[in]:		OAD         
//			VariableIndex[in]:VariableObj中的索引         
//			*OutBuf[in]:	输出变量
//         
//返回值:   返回变量数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注: 不带tag
//--------------------------------------------------
static WORD GetVariableData( BYTE Ch, BYTE Dot, BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf )
{
	TTwoByteUnion OI;
	TFourByteUnion TempData;
	BYTE ClassAttribute,ClassIndex,Num,i,Phase,Type;//,SpecialFlag;
	WORD VariableType,Len;//,Result
	BYTE *pBufBak = InBuf;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Len =0;
	// SpecialFlag = 0;
	
	switch( ClassAttribute )
	{
		case 2://A相扩展次数
			if( OI.w == 0x2000 )//电压
			{
				if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
				{
					return 0x8000;
				}
				
				Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				
				i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
				Num = ((ClassIndex==0) ? 3 : 1);
				Num = Num+i;
				
				for( i=0; i<Num; i++ )
				{
					Len = api_GetModuleData( U, i, 0, InBuf );
                    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
					InBuf += Len;
				}
				break;
			}
            else if( OI.w == 0x2001 )//电流
			{
				if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
				{
					return 0x8000;
				}
				
				Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				
				i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
				Num = ((ClassIndex==0) ? 3 : 1);
				Num = Num+i;
				
				for( i=0; i<Num; i++ )
				{
					Len = api_GetModuleData( I, i, 0, InBuf );
                    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
					InBuf += Len;
				}
				break;
			}
			else if( OI.w == 0x200F )//频率
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}

				Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[0] );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}

				//调用接口函数
				Len = api_GetModuleData( HZ, 0, 0, InBuf );
                if( Len == 0x8000 )
                {
                    return 0x8000;
                }
				InBuf += Len;
				break;
			}
			else if( (OI.w == 0x2035) || (OI.w == 0x2036) )//电压波动\电压闪变
			{
				if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
				{
					return 0x8000;
				}
				
				if( OI.w == 0x2035 )
				{
					Type = UChange;
				}
				else if(OI.w == 0x2036)
				{
					Type = UFlicker;
				}
				
				Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				
				i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
				Num = ((ClassIndex==0) ? 3 : 1);
				Num = Num+i;
				
				for( i=0; i<Num; i++ )
				{
					
					//---调用接口函数 读取电压波动和电压波动频率
					Len = api_GetModuleData( Type, i, 0, InBuf );
                    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
					InBuf += Len;
				}
				break;
			}
			else if( OI.w == 0x2067 )
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}
				
				Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				
				i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
				Num = ((ClassIndex==0) ? 3 : 1);
				Num = Num+i;
				
				for( i=0; i<Num; i++ )
				{
					
					//---调用接口函数 读取闪变
					Len = api_GetModuleData( UDayFlickerCount, i, 0, InBuf );
                    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
					InBuf += Len;
				}
				break;
			}
			else//进行下面判断，不能break
			{

			}

		case 3://B相扩展次数
		case 4://C相扩展次数
			switch( OI.w )
			{
				case 0x2046://谐波电压
				case 0x2047://谐波电流
				case 0x2048://谐波功率
				case 0x2060://间谐波电压
				case 0x2061://间谐波电流
				case 0x2062://间谐波功率
					if( ClassIndex > VariableObj[VariableIndex].Type2[1])
					{
						return 0x8000;
					}
					
					Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
    			    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
                    if( OI.w < 0x2060 )
                    {
						VariableType = HarmonicU+(OI.w-0x2046);
                    }
                    else
                    {
						VariableType = IHarmonicU+(OI.w-0x2060);
                    }
                    
                    Phase = (ClassAttribute-2);
					Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
					i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
					Num = Num+i;
					for( ; i<Num ; i++ )
					{
						Len = api_GetModuleData( VariableType, Phase, i, InBuf );
                        if( Len == 0x8000 )
                        {
                            return 0x8000;
                        }
						InBuf += Len;
					}
					break;
				
				case 0x2063://本日谐波电压统计
				case 0x2064://本日谐波电流统计
				case 0x2065://本日间谐波电压统计
				case 0x2066://本日间谐波电流统计
					if( ClassIndex > VariableObj[VariableIndex].Type2[1])
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
    			    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
					i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
					Num = Num+i;
					
					VariableType = HarmonicUDayCount+(OI.w-0x2063);
					Phase = (ClassAttribute-2);
					
					for( ; i<Num ; i++ )
					{					
						//调用接口函数
						Len = api_GetModuleData( VariableType, Phase, i, InBuf );
						if( Len == 0x8000 )
                        {
                            return 0x8000;
                        }
						InBuf += Len;
					}
					break;
				default:
					return 0x8000;
			}
			break;
		case 5:
			if( OI.w == 0x2000 )
			{
				Len = api_GetModuleData( U, 0, 0, TempData.b );
				TempData.l = (2200000 - TempData.l)*100/TempData.l;
				memcpy(InBuf, TempData.b, 2);
				InBuf += 2;
			}
			else if( OI.w == 0x200F )
			{
				Len = api_GetModuleData( HZ, 0, 0, TempData.b );
				TempData.l = TempData.d - 500000;
				memcpy(InBuf, TempData.b, 4);
				InBuf += 4;
			}
			break;
		case 6://换算及单位
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			InBuf[0] = VariableObj[VariableIndex].Scaler_Unit3->Scaler;
			InBuf[1] = VariableObj[VariableIndex].Scaler_Unit3->Unit;
			InBuf += 2;
			break;

		case 7://谐波总有功功率
			if( OI.w != 0x2048 )
			{
				return 0x8000;
			}
			
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}
			Tag = T_P_ALL;
			Len = GetTypeLen( eData, Tag+2 );
			if( Len == 0x8000 )
			{
				return 0x8000;
			}

			Len = api_GetModuleData( HarmonicPALL, 0, 0, InBuf );
            if( Len == 0x8000 )
            {
                return 0x8000;
            }
			InBuf += Len;

			break;
		
		default:
			return 0x8000;
	}

	return  (WORD)(InBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  对变量数据进行加TAG
//         
//参数:      *pOAD[in]：		OAD         
//           VariableIndex[in]:	VariableObj中的索引         
//           *InBuf[in]：		变量要添加tag的数据         
//           OutBufLen[in]：	OutBuf长度       
//           *OutBuf[out]：		输出数据
//         
//返回值:    返回添加tag后数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注:  	无
//--------------------------------------------------
static WORD GetVariableAddTag(BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,i;//,Num
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth,Lenth1;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0x8000;

	switch( ClassAttribute )
	{
		case 2://A相扩展次数
			if(( OI.w == 0x200F )||( OI.w == 0x21a0 )||( OI.w == 0x21a1 ))
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
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
			else if( OI.w == 0x2067 )
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}

				Tag = VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				Lenth1 = GetTypeLen( eData, Tag );

				if( ClassIndex == 0 )
				{
					OutBuf[0] = Array_698;
					OutBuf[1] = 0x03;
					OutBuf +=2;
					for( i=0; i<3; i++ )
					{
						OutBuf[0] = Structure_698;
						OutBuf[1] = 0x02;
						OutBuf +=2;
						if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
						{
							return 0;
						}
						
						Result = GetRequestAddTag( Tag, InBuf, OutBuf );
						if( Result == 0x8000 )
						{
							return 0x8000;
						}
						
						InBuf += Lenth1;
						OutBuf += Lenth;
						Result = GetRequestAddTag( Tag, InBuf, OutBuf );
						if( Result == 0x8000 )
						{
							return 0x8000;
						}
						InBuf += Lenth1;
						OutBuf += Lenth;
					}

					Lenth = 2+(Lenth*2+2)*3;
				}
				else
				{
					OutBuf[0] = Structure_698;
					OutBuf[1] = 0x02;
					OutBuf +=2;
					if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
					{
						return 0;
					}
					
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					
					InBuf += Lenth1;
					OutBuf += Lenth;
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					Lenth = Lenth*2+2;
				}	
				break;
			}
			else
			{}
		case 3://B相扩展次数
		case 4://C相扩展次数
			if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
			{
				return 0x8000;
			}
			
			Tag = VariableObj[VariableIndex].Type2;
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

		case 5:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( OI.w == 0x2000 )
			{
				Tag = T_Long;
			}
			else if( OI.w == 0x200F )
			{
				Tag = T_DoubleLong;
			}
			else
			{
				return 0x8000;
			}
			
			Result = GetRequestAddTag( T_ScalerUnit, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			Lenth = 3;
			break;

		case 6://换算及单位
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( OutBufLen < 3 )//如果缓冲大小不够，则不进行操作
			{
				return 0;
			}
			
			Result = GetRequestAddTag( T_ScalerUnit, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			Lenth = 3;
			break;

		case 7://谐波功率
			Tag = T_P_ALL;
			
			if( OI.w != 0x2048 )
			{
				return 0x8000;
			}
			
			if( ClassIndex > Tag[1])
			{
				return 0x8000;
			}
			
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
		
		default:
			return 0x8000;
	}

	return Lenth;	
}

//--------------------------------------------------
//功能描述:  读取变量
//         
//参数:      DataType[in]：	eData/eTagData/eAddTag        
//           Dot[in] :		小数点        
//           *pOAD[in]：	OAD         
//           *InBuf[in]：	需要添加标签的数据         
//           OutBufLen[in]：留给OutBuf的数据长度         
//           *OutBuf[in]：	返回的电能数据
//         
//返回值:    返回变量长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  
//--------------------------------------------------
WORD GetRequestVariable( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE VariableIndex;
	BYTE Buf[3000];//电压合格率源数据长度为32
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	Lenth = 0;
	
	VariableIndex = SearchVariableOAD( OI.w );
	if( VariableIndex == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType == eData) || (DataType == eTagData) )
	{
		Lenth = GetVariableData( Ch, Dot, pOAD, VariableIndex, Buf );
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
			Lenth = GetVariableAddTag(pOAD, VariableIndex, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		Lenth = GetVariableAddTag(pOAD, VariableIndex, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}

//--------------------------------------------------
//功能描述:  根据TAG获取变量长度
//         
//参数:      
//			DataType[in]：	eData/eTagData
//			*pOAD[in]：		OAD
//         
//返回值:   返回OAD对应数据的长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestVariableLen( BYTE DataType, BYTE* pOAD)
{
	BYTE ClassAttribute,VariableIndex,ClassIndex;
	TTwoByteUnion OI,Len;	
	const BYTE *Type;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	VariableIndex = SearchVariableOAD( OI.w );
	if( VariableIndex == 0x80 )
	{
		return 0x8000;
	}
	
	Type = VariableObj[VariableIndex].Type2;
	
	switch( ClassAttribute )
	{
		case 2://A相扩展次数
			if( OI.w == 0x200F )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Len.w = GetTypeLen( eTagData, Type );
				break;
			}
			else if( OI.w == 0x2067 )
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}
				
				Len.w = GetTypeLen( eTagData, Type );

				if( ClassIndex == 0 )
				{

					Len.w = 2+(Len.w*2+2)*3;
				}
				else
				{
					Len.w = Len.w*2+2;
				}	
				break;
			}
		case 3://B相扩展次数
		case 4://C相扩展次数
			if( ClassIndex > Type[1] )
			{
				return 0x8000;
			}
			
			Type +=GetTagOffsetAddr( ClassIndex, Type );
			Len.w = GetTypeLen(DataType, Type);				
			break;

		case 6://换算及单位
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( DataType == eData )//源数据长度
			{
				Len.w = 2;
			}
			else
			{
				Len.w = 3;
			}
			break;
		
		case 7://谐波总功率
			Type = T_P_ALL;
			
			if( OI.w != 0x2048 )
			{
				return 0x8000;
			}
			
			if( ClassIndex > Type[1] )
			{
				return 0x8000;
			}

			Type +=GetTagOffsetAddr( ClassIndex, Type );
			Len.w = GetTypeLen(DataType, Type);

			break;
		
		default:		
			return 0x8000;
	}

	return Len.w;
}

