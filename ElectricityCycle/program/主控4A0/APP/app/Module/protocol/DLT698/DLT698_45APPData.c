//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.23
//描述		DL/T 698.45面向对象协议 读取数据C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "GDW698/GDW698.h"

#if ( SEL_DLT698_2016_FUNC == YES  )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
const TScaler_Unit SU_1V		= 	{ -1, UNIT_V	};	
const TScaler_Unit SU_3A 		= 	{ -3, UNIT_A 	};	
const TScaler_Unit SU_10 		= 	{ -1, UNIT_0 	};
const TScaler_Unit SU_1W 		= 	{ -1, UNIT_W 	};
const TScaler_Unit SU_1VAR 		= 	{ -1, UNIT_VAR 	};
const TScaler_Unit SU_1VA 		= 	{ -1, UNIT_VA 	};
const TScaler_Unit SU_3 		= 	{ -3, UNIT_NO 	};
const TScaler_Unit SU_2 		= 	{ -2, UNIT_NO 	};
const TScaler_Unit SU_2HZ 		= 	{ -2, UNIT_Hz	};
const TScaler_Unit SU_10C 		= 	{ -1, UNIT_0C	};
const TScaler_Unit SU_2v 		= 	{ -2, UNIT_V	};
const TScaler_Unit SU_0MIN 		= 	{  0, UNIT_Min  };
const TScaler_Unit SU_00		= 	{  0, UNIT_NO   };
const TScaler_Unit SU_4KW		= 	{ -4, UNIT_KW	};
const TScaler_Unit SU_4KVAR		= 	{ -4, UNIT_KVAR };
const TScaler_Unit SU_4KVA		= 	{ -4, UNIT_KVA 	};
const TScaler_Unit SU_2PCT		= 	{ -2, UNIT_PCT 	};
const TScaler_Unit SU_2AH		= 	{ -2, UNIT_AH 	};
const TScaler_Unit SU_2kwh		= 	{ -2, UNIT_KWH 	};
const TScaler_Unit SU_0MS		= 	{  0, UNIT_MS 	};
const TScaler_Unit SU_01Kwh		= 	{  0, UNIT_1PWh };
const TScaler_Unit SU_01Kvar	= 	{  0, UNIT_1Pvarh };
const TScaler_Unit SU_2S		= 	{ -2, UNIT_S };
const TScaler_Unit SU_2Yuan 	= 	{ -2, UNIT_OTHER };//原来为{ -2, UNIT_1Pvarh }不对 只有钱包文件、透支金额、累计购电金额用
const TScaler_Unit SU_4Yuan 	= 	{ -4, UNIT_OTHER };//原来为{ -4, UNIT_1Pvarh }不对 电价用
//基本数据类型，长度只为格式一致，不使用
const BYTE T_ScalerUnit[] 	= { Scaler_Unit_698, 		2 	};
const BYTE T_Unsigned[]		= { Unsigned_698, 			1 	};
const BYTE T_UNLong[]		= { Long_unsigned_698, 		2 	};
const BYTE T_Long[]			= { Long_698,				2 	};
const BYTE T_DoubleLong[]	= { Double_long_698, 		4 	};
const BYTE T_UNDoubleLong[]	= { Double_long_unsigned_698,   4   };
const BYTE T_BitString16[]	= { Bit_string_698,			16 	};
const BYTE T_BitString32[]	= { Bit_string_698,			32 	};
const BYTE T_BitString8[]	= { Bit_string_698,			8 	};
const BYTE T_DateTimS[]		= { DateTime_S_698,		    7 	};
const BYTE T_Enum[]			= { Enum_698,			    1 	};
const BYTE T_VisString32[]	= { Visible_string_698,		32	};
const BYTE T_VisString16[]	= { Visible_string_698,		16	};
const BYTE T_VisString6[]	= { Visible_string_698,		6	};
const BYTE T_VisString4[]	= { Visible_string_698,		4	};
const BYTE T_OctetString5[]	= { Octet_string_698,		5	};
const BYTE T_OctetString7[]	= { Octet_string_698,		7	};//命令头保存时保存 CH,Cla,Ins,P1,P2,P31,P32,其中CH:0x01--ESAM 0x00--CARD,esam命令头为CH后的6字节，卡为CH后的5字节
const BYTE T_OctetString8[]	= { Octet_string_698,		8	};
const BYTE T_OctetString16[]= { Octet_string_698,	    16	};
const BYTE T_OctetStringFF[]= { Octet_string_698,	   0XFF	};
const BYTE T_TI[]			= { TI_698,				    3	};
const BYTE T_Bool[]			= { Boolean_698,			1   };
const BYTE T_Special[]		= { 0xff                        };//特殊处理
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
TTagPara TagPara; 	//加TAG时使用的参数
TTagBuf  TagBuf;	//加TAG时用到的buf长度								
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
WORD RecursionGetProtocolDataLen( BYTE *Buf, WORD* Count );


//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  获取基本数据类型长度
//         
//参数:      DataType[in]：eData/eTagData/eProtocolData 除eData外任何输入都是获取加TAG长度         
//           *Tag[in]		Tag[0]--数据类型 Tag[1]--数据长度
//
//返回值:    数据长度
//         
//备注内容:DataType ！=eProtocolData，tag的数据类型为Tag[0]--数据类型 Tag[1]--数据长度
//        DataType == eProtocolData，tag的数据类型为Tag[0]--数据类型+tag数据
//--------------------------------------------------
static WORD GetBasicTypeLen( BYTE DataType, BYTE *Tag )
{
	WORD Len,Len1,i;
	TTwoByteUnion TmpLen;//协议不定长数据处理
	BYTE LenOffest;
	
	Len = 0;
	Len1 = 0;
	
	switch( Tag[0] )//不支持TSA、MS、Region、RSD
	{
		case NULL_698:
			Len = ( (DataType==eData) ? 1 : 1 );
			break;
		#if (SEL_SEARCH_METER == YES )
		case Array_698://基本单元里面有数据，只有搜表结果用到，直接返回2个字节
			Len = 2;
			break;
		#endif
		case Boolean_698:
		case Integer_698:
		case Unsigned_698:
		case Enum_698:
			Len = ( (DataType==eData) ? 1 : 2 );
			break;	
		
		case Bit_string_698://根据协议默认Bit_string为8的倍数，如果不是8的倍数会舍掉剩余的位数 暂未考虑不为8的倍数的情况 支持超过127个字节
            if( DataType != eProtocolData )
            {
                if( Tag[1] == 0xff )
                {
                    TmpLen.w = TagPara.Lenth.w;
                }
                else
                {
                    TmpLen.w = Tag[1];
                }
                
                LenOffest = Deal_698DataLenth( Tag+1, TmpLen.b, eUNION_BITSTRING_TYPE ); 
				if( TmpLen.w == 0 )
				{
					TmpLen.w = 1;// 防止冲缓冲
				}
				TmpLen.w = ((TmpLen.w-1)/8)+1;// 考虑不为8的整数倍的情况，要再+1
                Len = ( (DataType==eData) ? (TmpLen.w) : (TmpLen.w+LenOffest+1) );
            }
            else//协议模式只获取加TAG长度
            {
                LenOffest = Deal_698DataLenth( Tag+1, TmpLen.b, ePROTOCOL_TYPE );
				if (TmpLen.w == 0)
				{
					TmpLen.w = 1; // 防止冲缓冲
				}
				TmpLen.w = ((TmpLen.w-1)/8)+1; // 考虑不为8的整数倍的情况，要再+1
				Len = (TmpLen.w+LenOffest+1);
            }

			break;	
		
		case Double_long_698:
		case Double_long_unsigned_698:
		case Float32_698:
		case OAD_698:
		case OMD_698:
			Len = ( (DataType==eData) ? 4 : 5 );
			break;
		
		case MAC_698:			
		case RN_698:
            if( DataType != eProtocolData )
            {
                if( Tag[1] > 0x7f)
                {
                    return 0;
                }
                Len = ( (DataType==eData) ? Tag[1] : (Tag[1]+2) );
            }
            else//协议模式只获取加TAG长度
            {
                LenOffest = Deal_698DataLenth( Tag+1, TmpLen.b, ePROTOCOL_TYPE ); 
                Len =(1+LenOffest+TmpLen.w);
            }
            
			break;
		
		case Octet_string_698:	    
		case Visible_string_698:    
		case UTF8_string_698:
		case TSA_698:
            if( DataType != eProtocolData )
            {
                if( Tag[1] == 0xff )
                {
                    Len1 = TagPara.Lenth.w;
                }
                else
                {
                    Len1 = Tag[1];
                }
                
                if( Len1 > 0x7f)
                {
                    return 0;
                }
                Len = ( (DataType==eData) ? Len1 : (Len1+2) );

            }
            else//协议模式只获取加TAG长度
            {
                LenOffest = Deal_698DataLenth( Tag+1, TmpLen.b, ePROTOCOL_TYPE ); 
                Len =(TmpLen.w+LenOffest+1);
            }
            
			break;
		
		case Long_698:
		case Long_unsigned_698:
		case OI_698:
		case Scaler_Unit_698:
			Len = ( (DataType==eData) ? 2 : (2+1) );
			break;

		case Long64_698:
		case Long64_unsigned_698:
		case Float64_698:
			Len = ( (DataType==eData) ? 8 : (8+1) );
			break;

		case Date_time_698:
			Len = ( (DataType==eData) ? 10 : (10+1) );
			break;
		
		case Date_698:
		case COMDCB_698:
			Len = ( (DataType==eData) ? 5 : (5+1) );
			break;
		
		case Time_698:
		case TI_698:
			Len = ( (DataType==eData) ? 3 : (3+1) );
			break;
		
		case DateTime_S_698:
			Len = ( (DataType==eData) ? 7 : (7+1) );
			break;
			
        case ROAD_698:
            if( DataType == eProtocolData )//协议模式只获取加TAG长度
            {
                Len += 5;
                LenOffest = Deal_698DataLenth( Tag+Len, TmpLen.b, ePROTOCOL_TYPE ); 
                Len += LenOffest;
                Len += (TmpLen.w*4);
            }
            else
            {
                Len = 0x8000;
            }   
            break;
            
        case CSD_698:    
	        if( DataType == eProtocolData )//协议模式只获取加TAG长度
	        {
	            Len += 2;
                if( Tag[1] == 0 )
                {
                    Len += 4;
                }
                else if( Tag[1] == 1 )
                {
                    Len += 4;
                    LenOffest = Deal_698DataLenth( Tag+Len, TmpLen.b, ePROTOCOL_TYPE ); 
                    Len += LenOffest;
                    Len += (TmpLen.w*4);
                }
                else
                {
                    return 0x8000;
                }
	        }
	        else
	        {   
                Len = 0x8000;
	        }
	        break;
	        
	    case SID_698:
	        if( DataType == eProtocolData )//协议模式只获取加TAG长度
	        {
                LenOffest = Deal_698DataLenth( Tag+(1+4), TmpLen.b, ePROTOCOL_TYPE ); 
                Len = 1+4+LenOffest+TmpLen.w;
	        }
	        else
	        {
                Len = 0x8000;
	        }
	        break;
	        
	    case SID_MAC_698:
            if( DataType == eProtocolData )//协议模式只获取加TAG长度
            {
                LenOffest = Deal_698DataLenth( Tag+(1+4), TmpLen.b, ePROTOCOL_TYPE );//获取sid的不定长数据长度 
                Len1 = 1+4+LenOffest+TmpLen.w;
                LenOffest = Deal_698DataLenth( Tag+Len1, TmpLen.b, ePROTOCOL_TYPE ); //获取MAC的不定长数据长度 
                Len = Len1+LenOffest+TmpLen.w;
            }
            else
            {
                Len = 0x8000;
            }
	        break;
	        
	    case RCSD_698:
	        if( DataType == eProtocolData )//协议模式只获取加TAG长度
	        {
	            LenOffest = Deal_698DataLenth( Tag+1, TmpLen.b, ePROTOCOL_TYPE ); 
	            Len += (1+LenOffest);
	            for( i=0; i < TmpLen.w; i++ )
	            {          
                    if( Tag[Len] == 0 )
                    {
                        Len += 5;
                    }
                    else if( Tag[Len] == 1 )
                    {
                        Len += 5;
                        LenOffest = Deal_698DataLenth( Tag+Len, TmpLen.b, ePROTOCOL_TYPE ); 
                        Len += LenOffest;
                        Len += (TmpLen.w*4);
                    }
                    else
                    {
                        return 0x8000;
                    }
	            }
	        }
	        else
	        {
                Len = 0x8000;
	        }

	        break;
	    default:
			Len = 0x8000;
			break;
	}
    
	return Len;
}

//--------------------------------------------------
//功能描述:  根据类型获取数据长度
//         
//参数:      DataType[in]：eData/eTagData        
//         Tag[in]：
//         			    Tag[0]--数据类型
//         				Tag[1]--成员个数
//         				Tag[n]--……
//
//返回值:     WORD  返回数据长度
//         
//备注内容: 支持array 和 array+struct+Normal方式 用于获取电表支持的数据类型长度
//--------------------------------------------------
WORD GetTypeLen( BYTE DataType, const BYTE *Tag )
{
	BYTE i,n,Num;
	WORD Len;	
	
	Len = 0;
	n = 0;
	
	if( DataType == eMaxData )//获取最大源数据长度 不能获取加TAG最大长度
	{
        DataType = eData; 
	}
	
	if( DataType > eTagData )
	{
		return 0;
	}

    if( Tag[1] == 0xff )
    {
        Num = TagPara.Array_Struct_Num;
    }
    else
    {
        Num = Tag[1];
    }
	
	if( Tag[0] == Array_698 )//支持array 和 array+struct+Normal方式
	{
		if( Num == 0 )//代表无数据 填null
		{
			return 1;
		}
		
		if( Tag[2] == Structure_698 )
		{
			for( i = 0; i < Tag[3]; i++ )
			{
				Len += GetBasicTypeLen( DataType, (BYTE*)&Tag[4+n] );
				if( Len > 0x8000 )
				{
                    return 0x8000;
				}

				n = n+2;
			}

			if( DataType == eTagData )
			{
				Len = ((Len + 2)*Num+2);
			}
			else
			{
				Len = Len*Num;
			}
		}
		else
		{
			if( DataType == eData )//只获取源数据
			{
				Len = (GetBasicTypeLen( DataType, (BYTE*)&Tag[2] )*Num);
			}
			else
			{
				Len = (GetBasicTypeLen( DataType, (BYTE*)&Tag[2] )*Num+2);
			}
		}
	}
	else if( Tag[0] == Structure_698 )//只支持Structure + Normal
	{
		if( Num == 0 )//代表无数据
		{
			return 1;
		}
		
		for( i = 0; i < Num; i++ )
		{
    		Len += GetBasicTypeLen( DataType, (BYTE*)&Tag[2+n] );
            if( Len > 0x8000 )
            {
                return 0x8000;
            }

            n = n+2;
		}

		if( DataType == eTagData )
		{
			Len += 2;
		}
	}
	else
	{
		Len = GetBasicTypeLen( DataType, (BYTE*)Tag);
	}

	return Len;
}

//--------------------------------------------------
//功能描述: 递归获取DATA的实际数据长度
//         
//参数:      Buf[in]：  data数据
//           Count[in]：递归限制次数
//
//返回值:     WORD 返回数据长度
//         
//备注内容:注意递归使用 避免无法跳出
//--------------------------------------------------
WORD RecursionGetProtocolDataLen( BYTE *Tag, WORD* Count )
{
    BYTE i;
    WORD Len = 0;
    
    Count[0] --;
    if( Count[0] == 0 )
    {
        return 0x8000;
    }
    
    if( Tag[0] == Array_698 )
    {
        Len = 2;
        for( i=0; i< Tag[1]; i++ )
        {
            Len += RecursionGetProtocolDataLen( (BYTE*)&Tag[Len], Count );//如果返回错误0x8000肯定大于MAX_APDU_SIZE
            if( Len > MAX_APDU_SIZE )
            {
                return 0x8000;
            }
        }
        
    }
    else if( Tag[0] == Structure_698 )
    {
        Len = 2;
        for( i=0; i< Tag[1]; i++ )
        {
            Len += RecursionGetProtocolDataLen( (BYTE*)&Tag[Len], Count );//如果返回错误0x8000肯定大于MAX_APDU_SIZE
            if( Len > MAX_APDU_SIZE )
            {
                return 0x8000;
            }
        }
    }
    else
    {
        Len = GetBasicTypeLen( eProtocolData, Tag );
    }
    
    return Len;
}

//--------------------------------------------------
//功能描述: 协议中获取DATA的实际数据长度
//         
//参数:      Buf[in]：  data数据
//
//返回值:     WORD  返回数据长度
//         
//备注内容:注意递归使用 避免无法跳出 经过测试嵌套457次实际运行时间为3ms
//--------------------------------------------------
WORD GetProtocolDataLen( BYTE *Tag )
{
    WORD Count;
 
    Count = 500; 
    return RecursionGetProtocolDataLen( Tag, (WORD*)&Count );
}


//--------------------------------------------------
//功能描述:  读取逻辑名
//         
//参数:      
//         	 DataType[in]：	eData/eTagData/eAddTag
//           *pOAD[in] 		OAD       
//           *InBuf[in]		对指定的输入数据添加tag         
//           OutBufLen[in] 	逻辑名长度        
//           *OutBuf[in]	输出逻辑名	
//         
//返回值:    返回OutBuf缓冲长度
//         
//备注内容:  无
//--------------------------------------------------
static WORD GetRequestLogicalName( BYTE DataType,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Lenth;

	Lenth = ( (DataType==eData) ? 2 : 4 );
	
	if( OutBufLen < Lenth )//判断长度
	{
		return 0x8000;
	}
	
	if( DataType == eData )//获取源数据
	{
		memcpy(OutBuf,pOAD,2);
	}
	else if( DataType == eTagData )//获取源数据加TAG
	{
		*(OutBuf++) = Octet_string_698;
		*(OutBuf++) = 2;
		memcpy(OutBuf,pOAD,2);
		OutBuf += 2;
	}
	else if( DataType == eAddTag )//对指定数据源加TAG
	{
		*(OutBuf++) = Octet_string_698;
		*(OutBuf++) = 2;
		memcpy(OutBuf,InBuf,2);
		OutBuf += 2;
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}

//--------------------------------------------------
//功能描述:  添加数据tag长度
//         
//参数:        DataLen[in]：	TLV格式中的L                 
//           *pTagLen[out]： 添加TAG的长度
//           *OutBuf[out]：	输出缓冲
//         
//返回值:    返回数据域长度
//         
//备注内容:  添加数据tag长度到OutBuf，并将添加长度返回pTagLen
//--------------------------------------------------
static WORD AddNormalLenthTag( BYTE DataLen, BYTE *pTagLen, BYTE *OutBuf )
{
    TTwoByteUnion Len;
	if( DataLen != 0xff )
	{
	    Len.w = DataLen;
	}
	else
	{
		if( TagPara.Lenth.w > MAX_APDU_SIZE )
		{
			return 0x8000;
		}
		
		Len.w = TagPara.Lenth.w;
	}

	*pTagLen = Deal_698DataLenth(OutBuf, Len.b, eUNION_TYPE );
	return Len.w;
}

//--------------------------------------------------
//功能描述:  基本数据类型加TAG
//         
//参数:       *Tag[in]     Tag[0]--数据类型  Tag[1]--数据长度 Tag[n]--……   
//          *InBuf[in]   需要添加tag的数据        
//          *OutBuf[in]  添加tag后输出的数据
//         
//返回值:      添加tag后输出的数据长度
//         
//备注内容:     对InBuf中的数据按照Tag标签的要求添加标签，输出到OutBuf 
//			注意不定长数据不要超过0x7f个字节
//          加TAG时，如果数据类型为基本固定长度类型，按照getbasictypelen的长度取数据
//--------------------------------------------------
static WORD AddNormalTag( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf )
{
	BYTE LenOffest;
	WORD Len;
	WORD Result,Len2;
	TTwoByteUnion TmpLen;
	
	InBuf += TagBuf.InBufLenth;
	OutBuf += TagBuf.OutBufLenth;
	Len2 = TagBuf.OutBufLenth;
	Len = 0;
	Result = 0x8000;
	
	switch( Tag[0] )
	{
		case NULL_698:
			*(OutBuf++) = 0;
			TagBuf.InBufLenth += 1;
			TagBuf.OutBufLenth += 1;
			break;
		#if (SEL_SEARCH_METER == YES )
		case Array_698://只有搜表会用到，直接返回空
			*(OutBuf++) = 1;
			*(OutBuf++) = 0;
			TagBuf.OutBufLenth += 2;
			break;
		#endif
		case Bit_string_698://bit_string进行特殊处理
			*(OutBuf++) = Tag[0];
			if( Tag[1] == 0xff )
			{
				LenOffest = Deal_698DataLenth(OutBuf, TagPara.Lenth.b, eUNION_BITSTRING_TYPE );
			    OutBuf += LenOffest;
				if( TagPara.Lenth.w == 0 )
				{
					TagPara.Lenth.w = 1;
				}
				Result = (TagPara.Lenth.w-1)/8+1;
			}
			else
			{
			    TmpLen.w = Tag[1];
			    LenOffest = Deal_698DataLenth(OutBuf, TmpLen.b, eUNION_TYPE );
			    OutBuf += LenOffest;
				if( Tag[1] == 0 )
				{
					Result = 1;
				}
				else
				{
					Result = ((Tag[1]-1)/8+1);
				}
			}

			lib_ExchangeBitWordLen(OutBuf, InBuf, Result);

			TagBuf.InBufLenth += Result;
			TagBuf.OutBufLenth += (Result+1+LenOffest);
			break;
		#if (SEL_SEARCH_METER == YES )
		case TSA_698:
		#endif
		case Octet_string_698:
		case Visible_string_698:
		case UTF8_string_698:
			*(OutBuf++) = Tag[0];
			Result = AddNormalLenthTag( Tag[1], (BYTE*)&Len, OutBuf );//TagBuf.OutBufLenth长度已在函数中添加 使用时请注意
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			
			OutBuf += Len;

			memcpy( OutBuf, InBuf, Result );
			
			TagBuf.InBufLenth += Result;
			TagBuf.OutBufLenth += (Result+1+Len);
			break;
		
		case Double_long_698:
		case Double_long_unsigned_698:
		case Integer_698:
		case Long_698:
		case Long_unsigned_698:
		case Long64_698:
		case Long64_unsigned_698:
		case Float32_698:
		case Float64_698:
		    Len = GetBasicTypeLen( eData, (BYTE*)Tag );
		    if( Len > 0xFF )
		    {
                return 0X8000;
		    }
		    
			*(OutBuf++) = Tag[0];
			lib_ExchangeData( OutBuf, InBuf, Len );
			TagBuf.InBufLenth += Len;
			TagBuf.OutBufLenth += (Len+1);
			break;

		case Date_698:
		case DateTime_S_698:
		    Len = GetBasicTypeLen( eData, (BYTE*)Tag );
		    if( Len > 0xFF )
		    {
                return 0X8000;
		    }
		    
			*(OutBuf++) = Tag[0];
			memcpy( OutBuf, InBuf, Len );
			lib_InverseData(OutBuf, 2);
			TagBuf.InBufLenth += Len;
			TagBuf.OutBufLenth += (Len+1);
			break;
		case Date_time_698:
		    Len = GetBasicTypeLen( eData, (BYTE*)Tag );
		    if( Len > 0xFF )
		    {
               return 0X8000;
		    }
		    
			*(OutBuf++) = Tag[0];
			memcpy( OutBuf, InBuf, Len );
			lib_InverseData(OutBuf, 2);
			lib_InverseData(OutBuf+9, 2);
			TagBuf.InBufLenth += Len;
			TagBuf.OutBufLenth += (Len+1);
			break;
		#if (SEL_SEARCH_METER == YES )
        case Time_698:
		    Len = GetBasicTypeLen( eData, (BYTE*)Tag );
		    if( Len > 0xFF )
		    {
               return 0X8000;
		    }
		    
			*(OutBuf++) = Tag[0];
			memcpy( OutBuf, InBuf, Len );
			TagBuf.InBufLenth += Len;
			TagBuf.OutBufLenth += (Len+1);
			break;
		#endif
		case Unsigned_698:
		case Enum_698:
		case Boolean_698:
		case Scaler_Unit_698:
		case OI_698:
		case OAD_698:
		case OMD_698:
		case COMDCB_698:
		    Len = GetBasicTypeLen( eData, (BYTE*)Tag );
		    if( Len > 0xFF )
		    {
                return 0X8000;
		    }
		    
			*(OutBuf++) = Tag[0];
			memcpy( OutBuf, InBuf, Len );
			TagBuf.InBufLenth += Len;
			TagBuf.OutBufLenth += (Len+1);
			break;

		case TI_698:
		    Len = GetBasicTypeLen( eData, (BYTE*)Tag );
		    if( Len > 0xFF )
		    {
                return 0X8000;
		    }
			*(OutBuf++) = Tag[0];
			memcpy( OutBuf, InBuf, Len );
			lib_InverseData(OutBuf+1, 2);
			TagBuf.InBufLenth += Len;
			TagBuf.OutBufLenth += (Len+1);
			break;
		case ROAD_698:
			return 0x8000;
			break;
		
		case SID_698:
			return 0x8000;
			break;
		
		case SID_MAC_698:
			return 0x8000;
			break;
		
		default:
			return 0x8000;
			break;
	}
	
	return (TagBuf.OutBufLenth-Len2);
}

//--------------------------------------------------
//功能描述:  结构体加TAG
//         
//参数:      *Tag[in]    Tag[0]--数据类型  Tag[1]--数据长度 Tag[n]--……   
//           *InBuf[in]	 需要添加tag的数据        
//           *OutBuf[in] 添加tag后输出的数据
//         
//返回值:    添加tag后输出的数据长度
//         
//备注内容:  对InBuf中的数据按照Tag标签的要求添加标签，输出到OutBuf 
//--------------------------------------------------
static WORD AddStructTag( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Num,i,n;
	WORD WithTagLen,Len;
	
	OutBuf[TagBuf.OutBufLenth] = Tag[0];
	n = 0;
	Len = 0;
	
	if( Tag[1] == 0xff )
	{
		Num = TagPara.Array_Struct_Num;
	}
	else
	{
		Num = Tag[1];
	}

	if( Num == 0 )
	{
        OutBuf[0] = NULL_698;
        TagBuf.OutBufLenth += 1;
        return 1;
	}
	
	OutBuf[TagBuf.OutBufLenth+1] = Num;
	
	Len += 2;
	TagBuf.OutBufLenth += 2;
	
	for( i=0; i<Num; i++ )//依次进行normal数据添加
	{
		//返回Tag+Data的长度
		WithTagLen = AddNormalTag( (BYTE*)&Tag[2+n], InBuf, OutBuf );
		if( WithTagLen == 0x8000 )
		{
			return WithTagLen;
		}
		Len += WithTagLen;
		n = n+2;
	}

	return Len;//随意返回数据
}

//--------------------------------------------------
//功能描述:  数组加TAG
//         
//参数:      *Tag[in]    Tag[0]--数据类型  Tag[1]--数据长度 Tag[n]--……   
//           *InBuf[in]	 需要添加tag的数据        
//           *OutBuf[in] 添加tag后输出的数据
//         
//返回值:    添加tag后输出的数据长度
//         
//备注内容:  对InBuf中的数据按照Tag标签的要求添加标签，输出到OutBuf 
//--------------------------------------------------
static WORD AddArrayTag( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Num,i;
	WORD WithTagLen,Len;

	Len = 0;
	
	if( Tag[1] == 0xff )
	{
		Num = TagPara.Array_Struct_Num;
	}
	else
	{
		Num = Tag[1];
	}

	if( Num == 0 )//如果为array个数为零 默认返回01 00
	{
		OutBuf[0] = NULL_698;
		TagBuf.OutBufLenth +=1;
		return 1;
	}
	
	OutBuf[0] = Tag[0];
	OutBuf[1] = Num;
	Len +=2;
	TagBuf.OutBufLenth +=2;

	for( i=0; i<Num; i++ )//依次进行normal数据添加
	{
		if( Tag[2] == Structure_698 )
		{
			WithTagLen = AddStructTag((BYTE *)&Tag[2], InBuf, OutBuf);
		}
		else
		{
			WithTagLen = AddNormalTag((BYTE *)&Tag[2], InBuf, OutBuf);
		}
		
		if( WithTagLen == 0x8000 )
		{
			return WithTagLen;
		}
		Len += WithTagLen;
	}

	return Len;
}
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  读取结算日
//参数	:
//			pBuf[out]
//返回值:	TRUE	FALSE
//注:返回时间数据为HEX
//--------------------------------------------------
WORD ReadSettlementDate(BYTE *pBuf)
{	
	BYTE i;	
	WORD Result;
    TBillingPara BillingParaBuf;

	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE*)&BillingParaBuf);
	if( Result == FALSE )
	{
		return FALSE;
	}
	else
	{
		for(i=0;i<MAX_MON_CLOSING_NUM;i++)
		{
			*(pBuf++) = BillingParaBuf.MonSavePara[i].Day;
			*(pBuf++) = BillingParaBuf.MonSavePara[i].Hour;
		}
	
		return TRUE;
	}
}

//--------------------------------------------------
//功能描述:  根据元素索引查找TCommonObj_t中Type2的偏移地址
//         
//参数:      ClassIndex[in]：	元素索引         
//           *Tag[in]：			标签
//         
//返回值:    返回 TCommonObj 中 Type2 的偏移
//         
//备注内容:  无
//--------------------------------------------------
WORD GetTagOffsetAddr(BYTE ClassIndex, const BYTE *Tag)
{
	if( ClassIndex == 0 )
	{
		return 0;
	}
	else if( Tag[0] == Array_698 )//支持array 和 array+struct+Normal方式
	{
		return 2;
	}
	else if( Tag[0] == Structure_698 )//只支持Structure + Normal
	{
		if( ClassIndex <= Tag[1])//避免classindex超限
		{
			return (2*(ClassIndex));
		}
	}

	return 0;
}

//--------------------------------------------------
//功能描述:  对输入数据添加Tag
//         
//参数:      *Tag[in]    Tag[0]--数据类型  Tag[1]--数据长度 Tag[n]--……   
//           *InBuf[in]	 需要添加tag的数据        
//           *OutBuf[in] 添加tag后输出的数据
//         
//返回值:    添加tag后输出的数据长度
//         
//备注内容:  对InBuf中的数据按照Tag标签的要求添加标签，输出到OutBuf 
//--------------------------------------------------
WORD GetRequestAddTag( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf )
{
	WORD Len;

	memset( (BYTE*)&TagBuf.InBufLenth, 0x00, sizeof(TTagBuf) );
	
	if( Tag[0] == Array_698 )
	{
		Len = AddArrayTag( Tag, InBuf, OutBuf );
	}
	else if( Tag[0] == Structure_698 )
	{
		Len = AddStructTag(Tag, InBuf, OutBuf );
	}
	else
	{
		Len = AddNormalTag(Tag, InBuf, OutBuf);
	}

	return Len;
}
//--------------------------------------------------
//功能描述:  698获取单个OAD数据
//         
//参数:	
//		GetDataType[in]	GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//		Ch[in]:			规约通道
//		DataType[in]:	eData/eTagData/eAddTag
//		Dot[in]:  		获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//		pOAD[in]：		获取数据的OAD
//		InBuf[in]：		给定要加Tag的数据(注:如果是事件类中的 事件上报状态(0x3300)、编程对象列表(0x3302)、事件清零列表(0x330c)时，第一个字节InBuf[0]需要存放Array的个数Number)
//		OutBufLen[in]：	给定OutBufLen的长度
//		OutBuf[out]:  	获取数据存放OutBuf中
//
//返回值: 返回数据长度	0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetProOadData( eGetDataType GetDataType, BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf)
{
	BYTE ClassAttribute;
	TTwoByteUnion OI,Len;
	DWORD OAD;	
	BOOL bNext;

	Len.w = 0;

	lib_ExchangeData(OI.b,pOAD,2);
	lib_ExchangeData((BYTE*)&OAD,pOAD,4);
	ClassAttribute = (pOAD[2]&0x1F);//bit0…bit4编码表示对象属性编号
	memset( (BYTE*)&TagBuf.InBufLenth, 0x00, sizeof(TagBuf) );
	memset( (BYTE*)&TagPara.Array_Struct_Num, 0x00, sizeof(TagPara) );
	
	if( (SelSecPowerConst==NO) && (OI.w==0x300e) )//辅助电源掉电事件在宏定义未打开时不能进行读取
	{
		return 0x8000;
	}
	
	if( ClassAttribute == 1 )//读属性1：逻辑名，在此统一处理，不再做到各个读函数里
	{
		return GetRequestLogicalName( DataType, pOAD, InBuf, OutBufLen, OutBuf );
	}
	
	if (IsModuleOI(OI.w,eModuleOrdOI)||((pOAD[2]==0x06)&&IsModuleOI(OI.w,eModuleEventOI)))//普通拓展模块类和事件配置参数
	{
		Len.w = GetModuleRequestVariable(DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( OI.w < 0x1000 )//电能类
	{
		Len.w = GetRequestEnergy( GetDataType, DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	#if(MAX_PHASE != 1)
	else if( (OI.w>=0x1000) && (OI.w<=0x11A3) )//最大需量类
	{
		Len.w = GetRequestDemand( DataType, Dot, pOAD, InBuf,OutBufLen, OutBuf);
	}
	#endif
	else if( (OI.w>=0x2000) && (OI.w<=0x2709) )//变量类
	{
		Len.w = GetRequestVariable( Ch, DataType, Dot, pOAD, InBuf,  OutBufLen, OutBuf);
	}
	else if( ((OI.w>=0x3000) && (OI.w<=0x3320))||IsModuleOI(OI.w,eModuleEventOI) 
            #if( SEL_TOPOLOGY == YES )
            || (OI.w ==0x3E03)
            #endif
     )//事件类
	{
		Len.w = GetProEventRecord( Ch,DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( ((OI.w>=0x4000) && (OI.w<=0x4FFF) ) 
	  #if (SEL_SEARCH_METER == YES )
	  || (OI.w == 0x6002) 
	  #endif
	)//参变量
	{
		Len.w = GetRequestMeterPara( Ch,DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( (OI.w>=0x5000) && (OI.w<=0x5011) )//冻结类
	{
		Len.w = GetProFreezeRecord( DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf );
	}
	else if(OI.w==0x6000)//采集类
	{
		Len.w = Class11_Get(OAD,0,OutBuf,OutBufLen,&bNext);
		if (Len.w)
		{
			if (bNext)
			{
				SetNextPara(Ch,(void*)Class11_Get,OAD,OutBuf[1],NULL,1);
			}
		}
		else 
		{
			// Len.w = Class10_Get(OAD,0,OutBuf,OutBufLen,&bNext);
			// if (Len.w)
			// {
			// 	if (bNext)
			// 	{
			// 		SetNextPara(Ch,(void*)Class10_Get,OAD,OutBuf[1],NULL,1);
			// 	}
			// }
			// else
			// {
			// 	if ((OI.w==0x6000)||(OI.w==0x6012)||(OI.w==0x6014))
			// 	{
					OutBuf[0]=NULL_698;
					Len.w = 1;
			// 	}
			// 	else
			// 	{
			// 		return 0x8000;
			// 	}
				
			// }
		}
		

	}
	else//其他类
	{
		Len.w = GetOtherData( Ch, DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf );
	}
	
	return Len.w;
}

//--------------------------------------------------
//功能描述:  698获取单个OAD数据的对外接口函数
//         
//参数:	
//		DataType[in]:	eData/eTagData/eAddTag
//		Dot[in]:  		获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//		pOAD[in]：		获取数据的OAD
//		InBuf[in]：		给定要加Tag的数据(注:如果是事件类中的 事件上报状态(0x3300)、编程对象列表(0x3302)、事件清零列表(0x330c)时，第一个字节InBuf[0]需要存放Array的个数Number)
//		OutBufLen[in]：	给定OutBufLen的长度
//		OutBuf[out]:  	获取数据存放OutBuf中
//
//返回值: 返回数据长度	0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD api_GetProOadData( eGetDataType GetDataType, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
    return GetProOadData( GetDataType, 0x55, DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
}

//--------------------------------------------------
//功能描述: 获取单个OAD对应数据的长度
//         
//参数:    	GetDataType[in]	GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//			DataType[in]: 	eData/eTagData
//         	pOAD[in]：获取数据长度的OAD
//			Number[in]：Array的个数  
//
//返回值:   数据长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容: Number:只有获取事件类长度时需要配置此参数，获取其他类长度时，此参数直接填写0
//--------------------------------------------------
WORD api_GetProOADLen( eGetDataType GetDataType, BYTE DataType, BYTE *pOAD, BYTE Number)
{
	BYTE ClassAttribute;
	TTwoByteUnion OI,Len;	
	BYTE RatioDataType;
	
	Len.w = 0x8000;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	
    if( (DataType != eData) 
     &&(DataType != eTagData) 
     &&(DataType != eMaxData) )
	{
		return 0x8000;
	}
	
	RatioDataType = DataType;
	
	if( OI.w > 0x11A3 )//非电能、需量类型不支持eMaxData
	{
        if( DataType == eMaxData )
        {
            DataType = eData;
        }
	}
	
	if( ClassAttribute == 1 )//属性1 逻辑名
	{
		if( DataType == eData )
		{
			return 2;
		}
		else
		{
			return 4;
		}
	}
	if (IsModuleOI(OI.w,eModuleAllOI))
	{
		Len.w = GetOILen(pOAD,DataType,4);
	}
	else if( OI.w < 0x1000 )
	{
		Len.w = GetRequestEnergyLen( GetDataType, RatioDataType, pOAD );
	}
	#if(MAX_PHASE == 3)
	else if( (OI.w>=0x1000) && (OI.w<=0x11A3) )
	{
		Len.w = GetRequestDemandLen(RatioDataType,pOAD);
	}
	#endif
	else if( (OI.w>=0x2000) && (OI.w<=0x2600) )
	{
		Len.w = GetRequestVariableLen(DataType,pOAD);
	}
	else if( (OI.w>=0x3000) && (OI.w<=0x3315) )//事件类 原为330E 2018/6/14改为3315 wlk
	{
		Len.w = GetProEventUnitLen(DataType,pOAD,Number);
	}
	else if( (OI.w>=0x4000) && (OI.w<=0x4FFF) )//OI:4000~4FFF 为参变量
	{
		Len.w = GetRequestMeterParaLen(DataType,pOAD);
	}
	else
	{
		Len.w = GetOtherDataLen(DataType,pOAD);
	}
	
	return Len.w;
}

#endif//#if ( SEL_DLT698_2016_FUNC == YES  )

