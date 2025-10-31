//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.23
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
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
static const TScaler_Unit SU_Active			= 	{ -2,   UNIT_KWH    };	//电能换算单位
static const TScaler_Unit SU_HActive 		= 	{ -4,   UNIT_KWH 	};	//高精度电能换算单位
static const TScaler_Unit SU_TAILActive 	= 	{ -6,   UNIT_KWH   };	//电能尾数电能换算单位

static const TScaler_Unit SU_RActive 		= 	{ -2, 	UNIT_KVARH	};	//无功电能换算单位
static const TScaler_Unit SU_HRActive 		= 	{ -4, 	UNIT_KVARH  };	//高精度无功电能换算单位
static const TScaler_Unit SU_TAILRActive 	= 	{ -6, 	UNIT_KVARH	};	//无功电能尾数换算单位


static const TScaler_Unit SU_AActive 		= 	{ -2, 	UNIT_KVAH	};	//视在电能换算单位
static const TScaler_Unit SU_HAActive 		= 	{ -4,   UNIT_KVAH   };	//高精度视在电能换算单位
static const TScaler_Unit SU_TAILAActive 	= 	{ -6, 	UNIT_KVAH	};	//视在电能尾数换算单位


//基本数据类型，长度只为格式一致，不使用
static const BYTE T_Active[]            	= { Array_698,0xff,Double_long_698,     4};	//电能类数据标识
static const BYTE T_HActive[]           	= { Array_698,0xff,Long64_698,          8};	//高精度电能类数据标识
static const BYTE T_TailActive[]			= { Array_698,0xff,Integer_698,      	1};//电能尾数
static const BYTE T_Unsigned_Active[]		= { Array_698,0xff,Double_long_unsigned_698,  4};//无符号电能类数据标识
static const BYTE T_Unsigned_HActive[]		= { Array_698,0xff,Long64_unsigned_698,     8};//无符号高精度电能类数据标识
static const BYTE T_Unsigned_TailActive[]	= { Array_698,0xff,Unsigned_698,      	1};//无符号电能尾数


typedef struct TEnergyObj_t
{
	WORD				OI;				//698对象标识：OI对象标识
	const TScaler_Unit 	*Scaler_Unit3;	//电能换算单位
	const TScaler_Unit 	*Scaler_Unit5;	//高精度电能换算单位
	const BYTE         	*Type2;			//电能类数据标识
	const BYTE         	*Type4;			//高精度电能类数据标识
	const TScaler_Unit 	*Scaler_Unit7;	//电能尾数单位
	const BYTE         	*Type6;			//电能尾数
}TEnergyObj;

//增加数据类型请查看源数据和加TAG是否支持
static const TEnergyObj EnergyObj[] =
{
	{ 0x0000, &SU_Active, 	&SU_HActive, 	T_Active, 			T_HActive,			&SU_TAILActive,		T_TailActive	 	 	},//组合有功电能
	{ 0x0010, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//正向有功电能
	{ 0x0020, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//反向有功电能
	#if( SEL_SEPARATE_ENERGY == YES )//是否支持分相电能			SEPARATE_ENER支持分相电能
	{ 0x0011, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//A相正向有功电能
	{ 0x0012, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//B相正向有功电能
	{ 0x0013, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//C相正向有功电能
	{ 0x0021, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//A相反向有功电能
	{ 0x0022, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//B相正向有功电能
	{ 0x0023, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//C相正向有功电能
	#endif
#if( SEL_RACTIVE_ENERGY == YES )//是否支持无功电能	
	{ 0x0030, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//组合无功1电能
	{ 0x0040, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//组合无功2电能
	{ 0x0050, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//第一象限无功电能
	{ 0x0060, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//第二象限无功电能	
	{ 0x0070, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//第三象限无功电能
	{ 0x0080, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//第四象限无功电能
	#if( SEL_SEPARATE_ENERGY == YES )//是否支持分相电能				SEPARATE_ENER支持分相电能				
	{ 0x0031, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//A相组合无功1电能
	{ 0x0032, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//B相组合无功1电能
	{ 0x0033, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//C相组合无功1电能
	{ 0x0041, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//A相组合无功2电能
	{ 0x0042, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//B相组合无功2电能
	{ 0x0043, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//C相组合无功2电能
	{ 0x0051, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//A相第一象限无功电能
	{ 0x0052, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//B相第一象限无功电能
	{ 0x0053, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//C相第一象限无功电能
	{ 0x0061, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//A相第二象限无功电能
	{ 0x0062, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//B相第二象限无功电能
	{ 0x0063, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//C相第二象限无功电能
	{ 0x0071, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//A相第三象限无功电能
	{ 0x0072, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//B相第三象限无功电能
	{ 0x0073, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//C相第三象限无功电能
	{ 0x0081, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//A相第四象限无功电能
	{ 0x0082, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//B相第四象限无功电能
	{ 0x0083, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//C相第四象限无功电能
	#endif
#endif	
#if( SEL_APPARENT_ENERGY == YES )//是否支持视在电能
	{ 0x0090, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//正向视在电能
	{ 0x00A0, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//反向视在电能
	#if( SEL_SEPARATE_ENERGY == YES )//是否支持分相电能	SEPARATE_ENER支持分相电能
	{ 0x0091, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//A相正向视在电能
	{ 0x0092, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//B相正向视在电能
	{ 0x0093, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//C相正向视在电能
	{ 0x00A1, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//A相反向视在电能
	{ 0x00A2, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//B相反向视在电能
	{ 0x00A3, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//C相反向视在电能
	#endif
#endif
};								

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  根据OI查找对应电能索引
//         
//参数:      OI[in]:	OI
//         
//返回值:    电能数组索引号
//         
//备注:  
//--------------------------------------------------
static BYTE SearchEnergyOAD( const WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(EnergyObj)/sizeof(TEnergyObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for(i=0; i<Num; i++)
	{
		if( OI == EnergyObj[i].OI )
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
//功能描述:  获取电能数据
//         
//参数:      
//			GetDataType[in]  GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//			Dot[in]:		 获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//			*pOAD[in]:		 OAD         
//			EnergyIndex[in]: EnergyObj中的索引         
//			*OutBuf[in]:	输出电能
//         
//返回值:   返回电能数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注: 不带tag
//--------------------------------------------------
static WORD GetEnergyData( eGetDataType GetDataType, BYTE Dot, BYTE *pOAD, BYTE EnergyIndex, BYTE *OutBuf )
{	
	BYTE ClassAttribute,ClassIndex,Num,NumBak,Ratio,Lenth,Len;
	WORD EnergyType,Result;
	const BYTE *Tag;
	TTwoByteUnion OI;
	TEightByteUnion EnergyTail;
	BYTE *pBufBak = OutBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];

	Lenth = 0;
	Len = 0;
	
	//获取读电能类型，供ReadEnergyHex使用
	EnergyType = ((OI.b[0]&0x0f)*0x1000)+((OI.b[0]&0xf0)>>4);
	Num = ((ClassIndex == 0) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	Ratio = ((ClassIndex == 0) ? 0 : (ClassIndex-1));
	Num = (Num+Ratio);	
	
	#if( SEL_SEPARATE_RATIO_ENERGY == NO )//不支持分项费率电能
	if( (OI.b[0]&0x03) != 0 )//读取分项电能
	{
		if( (ClassIndex!=0) && (ClassIndex!=1) )
		{
			return 0x8000;
		}
		
		Num = 1;
		Ratio = 0;
	}
	#endif

	switch( ClassAttribute )
	{
		case 2://总及费率电能量数组		
		case 4://高精度总及费率电能量数组
		case 6://电能尾数
			if( ClassIndex > (FPara1->TimeZoneSegPara.Ratio+1) )//元素索引不能大于费率数+1
			{
				return 0x8000;
			}
			
			if( ClassAttribute == 2 )
			{
				Tag = EnergyObj[EnergyIndex].Type2;
			}
			else if( ClassAttribute == 4 )
			{
				Tag = EnergyObj[EnergyIndex].Type4;
			}
			else
			{
				Tag = EnergyObj[EnergyIndex].Type6;
			}
			
			Lenth = GetTypeLen( eData, Tag+2 );
			
			if( ClassAttribute == 4 )
			{
				EnergyType = (EnergyType | 0x8000);
			}

			if( ClassAttribute == 6 )//电能尾数
			{
				Dot = 6;
				for( ; Ratio<Num; Ratio++ )
				{
					Result = api_GetCurrEnergyData(EnergyType, DATA_HEXCOMP, Ratio, Dot, OutBuf);
					if(Result == FALSE)
					{
						return 0x8000;
					}
					
					OutBuf += Lenth;
				}
			}
			else
			{
				if( GetDataType == eGetRecordData )
				{
					EnergyType = (EnergyType | 0x8000);//返回8个字节，避免超限
					NumBak = (Num-Ratio);
					for( ; Ratio<Num; Ratio++ )
					{
						Result = api_GetCurrEnergyData(EnergyType, DATA_HEXCOMP, Ratio, Dot, OutBuf+Len);
						if(Result == FALSE)
						{
							return 0x8000;
						}
						Len += 8;//偏移地址
					}			

					Result = api_EnergyToEnergyDBase( NumBak, (MAX_RATIO+1)*8+50, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					OutBuf += Result;
				}
				else
				{
					for( ; Ratio<Num; Ratio++ )
					{
						Result = api_GetCurrEnergyData(EnergyType, DATA_HEXCOMP, Ratio, Dot, OutBuf);
						if(Result == FALSE)
						{
							return 0x8000;
						}
						OutBuf += Lenth;
					}
				}
			}

			break;
			
		case 3://换算及单位
		case 5://高精度换算及单位
		case 7://电能尾数
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( ClassAttribute == 3 )
			{
				OutBuf[0] = EnergyObj[EnergyIndex].Scaler_Unit3->Scaler;
				OutBuf[1] = EnergyObj[EnergyIndex].Scaler_Unit3->Unit;
			}
			else if( ClassAttribute == 5)
			{
				OutBuf[0] = EnergyObj[EnergyIndex].Scaler_Unit5->Scaler;
				OutBuf[1] = EnergyObj[EnergyIndex].Scaler_Unit5->Unit;
			}
			else
			{
				OutBuf[0] = EnergyObj[EnergyIndex].Scaler_Unit7->Scaler;
				OutBuf[1] = EnergyObj[EnergyIndex].Scaler_Unit7->Unit;
			}

			OutBuf += 2;
			break;

		default:
			return 0x8000;
	}

	return  (WORD)(OutBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  对电能数据进行加TAG
//         
//参数:     	GetDataType[in]		GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//			*pOAD[in]：			OAD         
//          EnergyIndex[in]:	EnergyObj中的索引         
//          *InBuf[in]：		需要添加tag的数据         
//          OutBufLen[in]：		OutBuf长度       
//          *OutBuf[out]：		输出数据
//         
//返回值:    返回添加tag后数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注:  	无
//--------------------------------------------------
static WORD GetEnergyAddTag( eGetDataType GetDataType,BYTE *pOAD, BYTE EnergyIndex, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,Num,Buf[(MAX_RATIO+1)*8+50];
	eSwitchEnergyType SwitchEnergyType;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	Lenth = 0;

	Num = ((ClassIndex == 0) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);

	#if( SEL_SEPARATE_RATIO_ENERGY == NO )//不支持分项费率电能
	if( (OI.b[0]&0x03) != 0 )//读取分项电能
	{
		if( (ClassIndex!=0) && (ClassIndex!=1) )
		{
			return 0x8000;
		}	
		
		Num = 1;
	}
	#endif
	
	switch( ClassAttribute )
	{
		case 2://总及费率电能量数组
		case 4://高精度总及费率电能量数组
		case 6://电能尾数
			if( ClassIndex > (MAX_RATIO+1) )
			{
				return 0x8000;
			}
			if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
			{
				return 0;
			}
			
			if( GetDataType == eGetRecordData )
			{
				if( ClassAttribute != 6 )//脉冲尾数不处理
				{
					if( ClassAttribute == 2 )
					{
						SwitchEnergyType = SwitchNormalEnergy;
					}
					else
					{
						SwitchEnergyType = SwitchHighEnergy;
					}

					memcpy( Buf, InBuf, 5*Num );//使用自有buf 避免inbuf开辟的空间不够导致超限
					api_EnergyDBaseToEnergy( SwitchEnergyType, Num, sizeof(Buf),Buf);//转化为电能
					InBuf = Buf;//指针赋值，为了后面处理方式一致，此处使用一定注意，避免出现问题---jwh
				}
			}

			if( ClassAttribute == 2 )
			{
				Tag = EnergyObj[EnergyIndex].Type2;
			}
			else if( ClassAttribute == 4 )
			{
				Tag = EnergyObj[EnergyIndex].Type4;
			}
			else
			{
				Tag = EnergyObj[EnergyIndex].Type6;
			}
			
			TagPara.Array_Struct_Num = Num;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			return Lenth;
			break;

		case 3://换算及单位
		case 5://高精度换算及单位
		case 7://电能尾数
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
//功能描述:  读取电能
//         
//参数:     
//			 GetDataType[in] GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//			 DataType[in]：	eData/eTagData/eAddTag        
//           Dot[in] :		小数点        
//           *pOAD[in]：	    OAD         
//           *InBuf[in]：	需要添加标签的数据         
//           OutBufLen[in]： 留给OutBuf的数据长度         
//           *OutBuf[in]：	返回的电能数据
//         
//返回值:    返回电能长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  费率个数不能超过12
//--------------------------------------------------
WORD GetRequestEnergy( eGetDataType GetDataType, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE EnergyIndex;
	BYTE Buf[(MAX_RATIO+1)*8+50];//电能BUF最大长度
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	EnergyIndex = SearchEnergyOAD( OI.w );
	if( EnergyIndex == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType==eData) || (DataType==eTagData) )
	{		
		Lenth = GetEnergyData( GetDataType, Dot, pOAD, EnergyIndex, Buf );
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
			Lenth = GetEnergyAddTag( GetDataType, pOAD, EnergyIndex, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		if( InBuf == NULL )
		{
			return 0x8000;
		}
		
		Lenth = GetEnergyAddTag( GetDataType, pOAD, EnergyIndex, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}

//--------------------------------------------------
//功能描述:  根据TAG获取电能长度
//         
//参数:      
//			GetDataType[in]	GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//			DataType[in]：	eData/eTagData
//			*pOAD[in]：		OAD
//         
//返回值:   返回OAD对应数据的长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestEnergyLen( eGetDataType GetDataType, BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,EnergyIndex,ClassIndex,Num;
	TTwoByteUnion OI,Len;	
	const BYTE *Tag;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	

	if( DataType == eMaxData )//使用最大值
	{
	    DataType = eData;
        Num = MAX_RATIO+1;
	}
	else
	{
        Num = FPara1->TimeZoneSegPara.Ratio+1;
	}
	
	#if( SEL_SEPARATE_RATIO_ENERGY == NO )//不支持分项费率电能
	if( (OI.b[0]&0x03) != 0 )//读取分项电能
	{
		if( (ClassIndex != 0) && (ClassIndex != 1) )
		{
			return 0x8000;
		}	
		Num = 1;
	}
	#endif
	
	EnergyIndex = SearchEnergyOAD( OI.w );
	if( EnergyIndex == 0x80 )
	{
		return 0x8000;
	}

	switch( ClassAttribute )
	{
		case 2://总及费率电能量数组
		case 4://高精度总及费率电能量数组
		case 6://电能尾数
			if( ClassIndex > (MAX_RATIO+1) )//此处对元素索引的判断使用当前表的实际值 不用最大值
			{
				return 0x8000;
			}
			
			if( GetDataType == eGetRecordData )//电能固定长度 脉冲数4字节+1字节 特殊处理
			{
				if( ClassAttribute != 6 )//脉冲尾数不处理
				{
					if( DataType == eData )//直接返回长度
					{
						if( ClassIndex == 0 )
						{
							Len.w = 5*Num;
						}
						else
						{
							Len.w = 5;
						}

						return Len.w;
					}
					else//不做处理，加TAG长度一致
					{}

				}
			}
			
			if( ClassAttribute == 2 )
			{
				Tag = EnergyObj[EnergyIndex].Type2;
			}
			else if( ClassAttribute == 4 )
			{
				Tag = EnergyObj[EnergyIndex].Type4;
			}
			else
			{
				Tag = EnergyObj[EnergyIndex].Type6;
			}
			
			TagPara.Array_Struct_Num = Num;
			
			Tag +=GetTagOffsetAddr( ClassIndex, Tag);
			Len.w = GetTypeLen( DataType, Tag );
		
			break;

		case 3://换算及单位
		case 5://高精度换算及单位
		case 7://电能尾数
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

//-----------------------------------------------
//函数功能: 记录中的数据转化
//
//参数:
//		pOAD[in]: 输入OAD
//
//		Len[in]: 输入bufg长度
//
//		Buf[in]:指向电能的指针 返回hex
//
//		OutBufLen[in] 输出数据长度
//
//		OutBuf[in]	输出buf
//
//返回值:
//
//备注: 不能将数据反写到Buf中，因为申请的长度不够可能会超限，直接回写outbuf---jwh
//-----------------------------------------------
WORD api_RecordDataTransform( BYTE* pOAD, WORD* BufLen , BYTE* Buf, WORD OutBufLen,BYTE* OutBuf )
{
	BYTE ClassAttribute,ClassIndex,Num;
	BYTE tmpBuf[(MAX_RATIO+1)*8+50];//电能BUF最大长度
	TTwoByteUnion OI;

	lib_ExchangeData(OI.b,pOAD,2);

	if( OI.w < 0x1000 )//电能数据 需要脉冲数转化为电能
	{
		if( *BufLen > (MAX_RATIO+1)*8 )
		{
			return 0x8000;
		}
		
		ClassAttribute = (pOAD[2]&0x1f);
		ClassIndex = pOAD[3];
		if( (ClassAttribute == 2) || (ClassAttribute == 4) )//普通电能 高精度电能
		{
			Num = (*BufLen/5);
			memcpy( tmpBuf, Buf, *BufLen );//使用自有buf 避免inbuf开辟的空间不够导致超限
			if( ClassAttribute == 2 )
			{
				*BufLen = api_EnergyDBaseToEnergy( SwitchNormalEnergy, Num, sizeof(tmpBuf), tmpBuf );//转化为电能
			}
			else
			{
				*BufLen = api_EnergyDBaseToEnergy( SwitchHighEnergy, Num, sizeof(tmpBuf), tmpBuf );//转化为电能
			}
			if(*BufLen == 0x8000)
			{
				return 0;
			}
			if( OutBufLen >= *BufLen )//回填数据空间足够
			{
				memcpy( OutBuf, tmpBuf, *BufLen );
			}
			else
			{
				return 0;
			}

		}
		else//此种情况不需要判断，上层函数已判断过了，与之前处理一致
		{
			memcpy( OutBuf, Buf, *BufLen);
		}
	}
	else//此种情况不需要判断，上层函数已判断过了，与之前处理一致
	{
		memcpy( OutBuf, Buf, *BufLen);
	}

	return *BufLen;
}


#endif//#if ( SEL_DLT698_2016_FUNC == YES)

