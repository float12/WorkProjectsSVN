//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.27
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
#if( MAX_PHASE != 1 )
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
static const TScaler_Unit SU_KW			= 	{ -4, UNIT_KW 	};	//KW换算单位
static const TScaler_Unit SU_Kvar 		= 	{ -4, UNIT_KVAR 	};	//Kvar换算单位
static const TScaler_Unit SU_KVA 		= 	{ -4, UNIT_KVA 	};	//KVA换算单位

static const BYTE T_Demand[]			= { Array_698, 0xff, Structure_698, 02, Double_long_698, 4, DateTime_S_698, 7};			//有符号需量数据标识
static const BYTE T_UNDemand[]			= { Array_698, 0xff, Structure_698, 02, Double_long_unsigned_698, 4, DateTime_S_698, 7};//无符号需量数据标识
//增加数据类型请查看源数据和加TAG是否支持
static const TCommonObj DemandObj[] =
{
	{  0x1010,  &SU_KW,   	T_UNDemand },//正向有功最大需量
	{  0x1020,  &SU_KW,   	T_UNDemand },//反向有功最大需量
	{  0x1030,  &SU_Kvar,  	T_Demand   },//组合无功1最大需量
	{  0x1040,  &SU_Kvar,  	T_Demand   },//组合无功2最大需量
	{  0x1050,  &SU_Kvar,  	T_UNDemand },//第一象限最大需量
	{  0x1060,  &SU_Kvar,  	T_UNDemand },//第二象限最大需量
	{  0x1070,  &SU_Kvar,  	T_UNDemand },//第三象限最大需量
	{  0x1080,  &SU_Kvar,  	T_UNDemand },//第四象限最大需量
	#if( SEL_APPARENT_DEMAND == YES )	
	{  0x1090,  &SU_KVA,  	T_UNDemand },//正向视在最大需量
	{  0x10A0,  &SU_KVA,  	T_UNDemand },//反向视在最大需量
	#endif	
	{  0x1110,  &SU_KW,		T_UNDemand },//冻结周期内正向有功最大需量
	{  0x1120,  &SU_KW,  	T_UNDemand },//冻结周期内反向有功最大需量
	{  0x1130,  &SU_Kvar, 	T_Demand   },//冻结周期内组合无功1最大需量
	{  0x1140,  &SU_Kvar, 	T_Demand   },//冻结周期内组合无功2最大需量
	{  0x1150,  &SU_Kvar, 	T_UNDemand },//冻结周期内第一象限最大需量
	{  0x1160,  &SU_Kvar, 	T_UNDemand },//冻结周期内第二象限最大需量
	{  0x1170,  &SU_Kvar, 	T_UNDemand },//冻结周期内第三象限最大需量
	{  0x1180,  &SU_Kvar, 	T_UNDemand },//冻结周期内第四象限最大需量
	#if( SEL_APPARENT_DEMAND == YES )	
	{  0x1190,  &SU_KVA, 	T_UNDemand },//冻结周期内正向视在最大需量
	{  0x11A0,  &SU_KVA, 	T_UNDemand },//冻结周期内反向视在最大需量
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
//功能描述:  根据OI查找对应变量索引
//         
//参数:      OI[in]:	OI
//         
//返回值:    需量数组索引号
//         
//备注:  
//--------------------------------------------------
static BYTE SearchDemandOAD( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(DemandObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == DemandObj[i].OI )
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
//功能描述:  获取需量数据
//         
//参数:      
//			Dot[in]:		获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//			*pOAD[in]:		OAD         
//			DemandIndex[in]:DemandObj中的索引         
//			*OutBuf[in]:	输出需量
//         
//返回值:   返回需量数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注: 不带tag
//--------------------------------------------------
static WORD GetDemandData( BYTE Dot, BYTE *pOAD, BYTE DemandIndex, BYTE *InBuf )
{
	BYTE ClassAttribute,ClassIndex,Num,Ratio;
	TTwoByteUnion OI;	
	WORD DemandType;
	BYTE *pBufBak = InBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	
	switch( ClassAttribute )
	{
		case 2://总及费率最大需量数组
			if( ClassIndex > (FPara1->TimeZoneSegPara.Ratio+1) )
			{
				return 0x8000;
			}
			
			DemandType = (OI.b[0]>>4);
			Num = ((ClassIndex == 0) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
			Ratio = ((ClassIndex == 0) ? 0 : (ClassIndex-1));
			Num = (Num+Ratio);	
			
			if( OI.b[1] == 0x11 )//冻结周期内需量,OI.w = 0x1110时OI.b[1]=0x11
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				DemandType = (DemandType | 0x8000);
				Num = 1;
				Ratio = 0;
			}
			for( ; Ratio<Num; Ratio++ )
			{
				api_GetDemand( DemandType, DATA_HEXCOMP, Ratio, Dot, InBuf);
				InBuf += 11;
			}
			break;
		case 3://换算及单位
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			InBuf[0] = DemandObj[DemandIndex].Scaler_Unit3->Scaler;
			InBuf[1] = DemandObj[DemandIndex].Scaler_Unit3->Unit;
			InBuf += 2;
			break;

		default:
			return 0x8000;
	}

	return  (WORD)(InBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  对需量数据进行加TAG
//         
//参数:      *pOAD[in]：		OAD         
//           DemandIndex[in]:	DemandObj中的索引         
//           *InBuf[in]：		需要添加tag的数据         
//           OutBufLen[in]：	OutBuf长度       
//           *OutBuf[out]：		输出数据
//         
//返回值:    返回添加tag后数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注:  	无
//--------------------------------------------------
static WORD GetDemandAddTag(BYTE *pOAD, BYTE DemandIndex, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,Num;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	Num = ((ClassIndex == 0) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	
	switch( ClassAttribute )
	{
		case 2://需量
			if( ClassIndex > (FPara1->TimeZoneSegPara.Ratio+1) )
			{
				return 0x8000;
			}
			Lenth = (ClassIndex == 0) ? ((FPara1->TimeZoneSegPara.Ratio+1)*15+2) : 15;
			if( OI.b[1] == 0x11 )//期间冻结需量只有总,OI.w = 0x1110时OI.b[1]=0x11
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Lenth = (ClassIndex == 0) ? 17 : 15;
				Num = 1;
			}
			
			if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
			{
				return 0;
			}
			
			Tag = DemandObj[DemandIndex].Type2;
			
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			TagPara.Array_Struct_Num = Num;
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			return Lenth;
			break;

		case 3://换算及单位
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
			return 3;
			break;

		default:
			return 0x8000;
	}
}

//--------------------------------------------------
//功能描述:  读取需量
//         
//参数:      DataType[in]：	eData/eTagData/eAddTag        
//           Dot[in] :		小数点        
//           *pOAD[in]：	OAD         
//           *InBuf[in]：	需要添加标签的数据         
//           OutBufLen[in]：留给OutBuf的数据长度         
//           *OutBuf[in]：	返回的电能数据
//         
//返回值:    返回需量长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  费率个数不能超过12
//--------------------------------------------------
WORD GetRequestDemand( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE DemandIndex;	
	BYTE Buf[(MAX_RATIO+1)*11+30];//需量源数据最大BUF长度
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	DemandIndex = SearchDemandOAD( OI.w );
	if( DemandIndex == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType==eData) || (DataType==eTagData) )
	{
		Lenth = GetDemandData( Dot, pOAD, DemandIndex, Buf );
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
			Lenth = GetDemandAddTag(pOAD, DemandIndex, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		if( InBuf == NULL )
		{
			return 0x8000;
		}
		Lenth = GetDemandAddTag(pOAD, DemandIndex, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}

//--------------------------------------------------
//功能描述:  根据TAG获取需量长度
//         
//参数:      
//			DataType[in]：	eData/eTagData
//			*pOAD[in]：		OAD
//         
//返回值:   返回OAD对应数据的长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestDemandLen( BYTE DataType, BYTE* pOAD)
{
	TTwoByteUnion OI,Len;
	BYTE ClassAttribute,DemandIndex,ClassIndex,Num;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DemandIndex = SearchDemandOAD( OI.w );
	if( DemandIndex == 0x80 )
	{
		return 0x8000;
	}

    if( DataType == eMaxData )//使用最大值
	{
	    DataType = eData;
        Num = MAX_RATIO;
	}
	else
	{
        Num = FPara1->TimeZoneSegPara.Ratio;
	}
	
	switch( ClassAttribute )
	{
		case 2://总及费率最大需量数组
			if( ClassIndex > (FPara1->TimeZoneSegPara.Ratio+1) )
			{
				return 0x8000;
			}
			
			if( OI.b[1] == 0x11 )
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Num = 0;
			}
	
			if( DataType == eData )
			{
				Len.w = (ClassIndex == 0) ? ((Num+1)*11) : 11;
			}
			else
			{
				Len.w = (ClassIndex == 0) ? ((Num+1)*15+2) : 15;
			}
				
			break;

		case 3://换算及单位
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
		default:		
			return 0x8000;
	}

	return Len.w;
}
#endif//#if( MAX_PHASE != 1 )
#endif//#if ( SEL_DLT698_2016_FUNC == YES)

