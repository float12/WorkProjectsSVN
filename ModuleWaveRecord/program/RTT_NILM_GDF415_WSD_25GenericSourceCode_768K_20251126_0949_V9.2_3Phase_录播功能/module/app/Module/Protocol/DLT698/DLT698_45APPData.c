//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.23
//描述		DL/T 698.45面向对象协议 读取数据C文件
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
extern const BYTE A_4800_4_ApplFrzData;
extern const BYTE A_4800_4_ApplFrzData_NotElecHot;
//-----------------------------------------------
//				全局使用的变量，常量
const TScaler_Unit SU_1V		= 	{ -1, UNIT_V	};	
const TScaler_Unit SU_4V		= 	{ -1, UNIT_V	};	
const TScaler_Unit SU_3A 		= 	{ -3, UNIT_A 	};	
const TScaler_Unit SU_4A 		= 	{ -3, UNIT_A 	};
const TScaler_Unit SU_10 		= 	{ -1, UNIT_0 	};
const TScaler_Unit SU_1W 		= 	{ -1, UNIT_W 	};
const TScaler_Unit SU_4W 		= 	{ -1, UNIT_W 	};
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

const TScaler_Unit SU_5A 		= 	{ -4, UNIT_A 	};		//wxy
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
WORD GetBasicTypeLen( BYTE DataType, BYTE *Tag )
{
	WORD Len,Len1,i;
	TTwoByteUnion TmpLen;//协议不定长数据处理
	BYTE LenOffest;
	
	Len = 0;
	Len1 = 0;
	
	switch( Tag[0] )//不支持TSA、MS、Region、RSD
	{
	case INVOID_698:
			Len = ( (DataType==eData) ? 1 : 1 );
			break;
		case NULL_698:
			Len = ( (DataType==eData) ? 1 : 1 );
			break;
		case Region_698:
			Len = ( (DataType==eData) ? 7 : 8 );
			break;
		case Boolean_698:
		case Integer_698:
		case Unsigned_698:
		case Enum_698:
			Len = ( (DataType==eData) ? 1 : 2 );
			break;	
		
		case Bit_string_698://根据协议默认Bit_string为8的倍数，如果不是8的倍数会舍掉剩余的位数 暂未考虑不为8的倍数的情况 支持超过127个字节
            if( DataType != eProtocolData )
            {
                TmpLen.w = Tag[1];
                LenOffest = Deal_698DataLenth( Tag+1, TmpLen.b, eUNION_OFFSET_TYPE ); 
                Len = ( (DataType==eData) ? (TmpLen.w/8) : (TmpLen.w/8+LenOffest+1) );
            }
            else//协议模式只获取加TAG长度
            {
                LenOffest = Deal_698DataLenth( Tag+1, TmpLen.b, ePROTOCOL_TYPE ); 
                Len = (TmpLen.w/8+LenOffest+1);
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
		else if(Tag[2] == Array_698) //wxy 20210806
		{
			Len = GetTypeLen(DataType,(BYTE*)&Tag[2]);
			if( DataType == eTagData )
			{
				Len = ((Len + 2)*Tag[3]+2);
			}
			else
			{
				Len = Len*Tag[3];
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
WORD GetTypeLen4804( BYTE DataType, const BYTE *Tag, BYTE *InBuf)
{
	BYTE i,n,Num,j,ClassID;
	WORD Len,Len1 = 0;	
	
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
			for(j = 0; j<Num; j++)
			{
				ClassID = InBuf[j*sizeof(T_ApplFrzData)+1];
				#if(EB_EXPMSG == 0)
				if((ClassID == 0x03) || (ClassID == 0x04) || (ClassID == 0x05) || \
					(ClassID == 0x06) || (ClassID == 0x07) || (ClassID == 0xA1))//只有热水器、热水壶、电饭锅、电烤箱、电暖器、电热类，这6个类别的数据需要存，其余不存。
				#elif(EB_EXPMSG == 1)
				if((ClassID == 0x03) || (ClassID == 0x04) || (ClassID == 0x05) || \
					(ClassID == 0x06) || (ClassID == 0x07) || (ClassID == 0xA1) || (ClassID == 0x31))//只有热水器、热水壶、电饭锅、电烤箱、电暖器、电热类，这6个类别的数据需要存，其余不存。北京军区电动车增加了电动车
				#endif 
				{
					Tag = (BYTE *)&A_4800_4_ApplFrzData;
				}
				else
				{
					Tag = (BYTE *)&A_4800_4_ApplFrzData_NotElecHot;
				}
				n = 0;
                Len = 0;
				for( i = 0; i < Tag[3]; i++ )
				{
					Len += GetBasicTypeLen( DataType, (BYTE*)&Tag[4]+n);
					if( Len > 0x8000 )
					{
						return 0x8000;
					}
					n = n+2;
				}
				if( DataType == eTagData )
				{
					Len1 += (Len + 2);//structure 元素个数
				}
				else
				{
					Len1 += Len;
				}
			}

			if( DataType == eTagData )
			{
				Len = Len1+2;//array 和个数
			}
 			else
			{
				Len = Len1;
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
			ClassID = InBuf[i*sizeof(T_ApplFrzData)+1];
			#if(EB_EXPMSG == 0)
			if((ClassID == 0x03) || (ClassID == 0x04) || (ClassID == 0x05) || \
				(ClassID == 0x06) || (ClassID == 0x07) || (ClassID == 0xA1))//只有热水器、热水壶、电饭锅、电烤箱、电暖器、电热类，这6个类别的数据需要存，其余不存。
			#elif(EB_EXPMSG == 1)
			if((ClassID == 0x03) || (ClassID == 0x04) || (ClassID == 0x05) || \
				(ClassID == 0x06) || (ClassID == 0x07) || (ClassID == 0xA1) || (ClassID == 0x31))//只有热水器、热水壶、电饭锅、电烤箱、电暖器、电热类，这6个类别的数据需要存，其余不存。北京军区加入电动车
			#endif
			{
				Tag = (BYTE *)&A_4800_4_ApplFrzData;
			}
			else
			{
				Tag = (BYTE *)&A_4800_4_ApplFrzData_NotElecHot;
			}
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
	return Len;
}

WORD GetTypeLen2( BYTE DataType, const BYTE *Tag , int nSum)
{
	BYTE n,Num=0,StructNum=0;
	WORD Len,nLenght;	
	BOOL bList=FALSE;

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

	while(1)
	{
		switch(Tag[n])
		{
		case Array_698:
			n += 1;
			Num = Tag[n];
			n += 1;
			nSum -= 2;
			bList = TRUE;
			if( DataType == eTagData )
				Len += 2;

			continue;
		case Structure_698:
			n += 1;
			StructNum = Tag[n];
			n += 1;
			nSum -= 2;
			if( DataType == eTagData )
			{
				if(bList)
					Len += 2*Num;
				else
					Len += 2;
			}
			continue;
		default:
			nLenght = GetBasicTypeLen( DataType, (BYTE*)&Tag[n] );
			if(bList)
			{
				nLenght = nLenght*Num;
			}
			Len += nLenght;
			n += 2;
			nSum -= 2;
			if(StructNum-- == 0)
				bList = FALSE;
			break;
		}
		if(Tag[n] == 0)
			break;
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
		case INVOID_698:
			*(OutBuf++) = 0;
			TagBuf.InBufLenth += 1;
			TagBuf.OutBufLenth += 1;
			break;
		case NULL_698:
			*(OutBuf++) = 0;
			TagBuf.InBufLenth += 1;
			TagBuf.OutBufLenth += 1;
			break;
		case Bit_string_698://bit_string进行特殊处理
			*(OutBuf++) = Tag[0];
			if( Tag[1] == 0xff )
			{
				LenOffest = Deal_698DataLenth(OutBuf, TagPara.Lenth.b, eUNION_TYPE );
			    OutBuf += LenOffest;
				Result = (TagPara.Lenth.w/8);
			}
			else
			{
			    TmpLen.w = Tag[1];
			    LenOffest = Deal_698DataLenth(OutBuf, TmpLen.b, eUNION_TYPE );
			    OutBuf += LenOffest;
				Result = (Tag[1]/8);
			}
			
			lib_ExchangeBit(OutBuf, InBuf, Result);
			
			TagBuf.InBufLenth += Result;
			TagBuf.OutBufLenth += (Result+1+LenOffest);
			break;
		case Region_698:
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
		
		default:
			return 0x8000;
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
static WORD AddStructTag4804( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Num,i,n,ClassID;
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
		ClassID = InBuf[i*sizeof(T_ApplFrzData)+1];
		#if(EB_EXPMSG == 0)
		if((ClassID == 0x03) || (ClassID == 0x04) || (ClassID == 0x05) || \
			(ClassID == 0x06) || (ClassID == 0x07) || (ClassID == 0xA1))//只有热水器、热水壶、电饭锅、电烤箱、电暖器、电热类，这6个类别的数据需要存，其余不存。
		#elif(EB_EXPMSG == 1)
		if((ClassID == 0x03) || (ClassID == 0x04) || (ClassID == 0x05) || \
			(ClassID == 0x06) || (ClassID == 0x07) || (ClassID == 0xA1) || (ClassID == 0x31))//只有热水器、热水壶、电饭锅、电烤箱、电暖器、电热类，这6个类别的数据需要存，其余不存。北京军区加入电动车
		#endif
		{
			Tag = (BYTE *)&A_4800_4_ApplFrzData;
		}
		else
		{
			Tag = (BYTE *)&A_4800_4_ApplFrzData_NotElecHot;
		}
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
static WORD AddArrayTag4804( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Num,i,ClassID;
	WORD WithTagLen,Len;
	int nTagNum = 0;

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
			ClassID = InBuf[i*sizeof(T_ApplFrzData)+1];
			#if(EB_EXPMSG == 0)
			if((ClassID == 0x03) || (ClassID == 0x04) || (ClassID == 0x05) || \
				(ClassID == 0x06) || (ClassID == 0x07) || (ClassID == 0xA1))//只有热水器、热水壶、电饭锅、电烤箱、电暖器、电热类，这6个类别的数据需要存，其余不存。
			#elif(EB_EXPMSG == 1)
			if((ClassID == 0x03) || (ClassID == 0x04) || (ClassID == 0x05) || \
				(ClassID == 0x06) || (ClassID == 0x07) || (ClassID == 0xA1) || (ClassID == 0x31))//只有热水器、热水壶、电饭锅、电烤箱、电暖器、电热类，这6个类别的数据需要存，其余不存。北京军区加入电动车
			#endif
			{
				Tag = (BYTE *)&A_4800_4_ApplFrzData;
			}
			else
			{
				Tag = (BYTE *)&A_4800_4_ApplFrzData_NotElecHot;
			}
			WithTagLen = AddStructTag((BYTE *)&Tag[2], InBuf, OutBuf);
			nTagNum += Tag[3]*2 + 2 ;
		}
		
		if( WithTagLen == 0x8000 )
		{
			return WithTagLen;
		}
		Len += WithTagLen;
	}
	*InBuf = nTagNum;

	return Len;
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
int nTagNum = 0;

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
			nTagNum += Tag[3]*2 + 2 ;
		}
		else if(Tag[2] == Array_698 )//wxy add 20210806
		{
			/*OutBuf[TagBuf.OutBufLenth] = Tag[2];
			OutBuf[TagBuf.OutBufLenth+1] = Tag[3];
			TagBuf.OutBufLenth +=2;
			Tag +=2;
			Len += 2;
			TagBuf.OutBufLenth +=2;
			WithTagLen = 2;*/
			TagBuf.OutBufLenth =0;
			WithTagLen = AddArrayTag((BYTE *)&Tag[2], InBuf, &OutBuf[Len]);
			Tag += (*InBuf + 2);
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
	*InBuf = nTagNum;

	return Len;
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
WORD GetRequestAddTag4804( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf )
{
	WORD Len;

	memset( (BYTE*)&TagBuf.InBufLenth, 0x00, sizeof(TTagBuf) );
	
	if( Tag[0] == Array_698 )
	{
		Len = AddArrayTag4804( Tag, InBuf, OutBuf );
	}
	else if( Tag[0] == Structure_698 )
	{
		Len = AddStructTag4804(Tag, InBuf, OutBuf );
	}

	return Len;
}

//--------------------------------------------------
//功能描述:  698获取单个OAD数据
//         
//参数:	
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
WORD GetProOadData( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute;
	TTwoByteUnion OI,Len;	
	int ClassIndex;
        
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	memset( (BYTE*)&TagBuf.InBufLenth, 0x00, sizeof(TagBuf) );
	memset( (BYTE*)&TagPara.Array_Struct_Num, 0x00, sizeof(TagPara) );
	
	ClassIndex = pOAD[3];
	if( ClassAttribute == 1 )//读属性1：逻辑名，在此统一处理，不再做到各个读函数里
	{
		return GetRequestLogicalName( DataType, pOAD, InBuf, OutBufLen, OutBuf );
	}
	
	if( (OI.w>=0x2000) && (OI.w<=0x2600) )//变量类
	{
		Len.w = GetRequestVariable( Ch, DataType, Dot, pOAD, InBuf,  OutBufLen, OutBuf);
	}
 	else if( (OI.w>=0x3000) && (OI.w<=0x334B) )//事件类// wxy else if( (OI.w>=0x3039) && (OI.w<=0x334B) )//事件类
	{
		Len.w = GetProEventRecord( Ch,DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}

	else if( (OI.w>=0x4000) && (OI.w<=0x4FFF) && (OI.w != 0x4400))//参变量
	{
		Len.w = GetRequestMeterPara( DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( (OI.w>=0x5000) && (OI.w<=0x5011) )//冻结类
	{
		Len.w = GetProFreezeRecord( DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf );
	}
	else if( (OI.w == 0xF001) ||  (OI.w == 0xF402))// 文件类
	{
		if(ClassAttribute == 4)	//属性4
		{
			Len.w = GetRequestFilePara(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
		}
		else if((OI.w == 0xF001) && (ClassAttribute == 5))//属性5
		{
			Len.w = GetRequestFileFlag(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
		}
		else if((OI.w == 0xF402) && (ClassAttribute == 2) && (ClassIndex == 2)) //属性2
			Len.w = GetRequestFileF402Ver2(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
		else
			Len.w = GetRequestMeterPara( DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	
	}
	else if(  (OI.w == 0xF402))
	{
		Len.w = GetRequestFileF402(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( (OI.w == 0x4027)  )// 文件类
	{
		Len.w = GetRequestFileParaF4027(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( (OI.w == 0xF100)  )// ESAM  秘钥类
	{
		Len.w = GetRequestESamPara(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( (OI.w == 0xF101)  )// ESAM  秘钥类
	{
		Len.w = GetRequestSafetyPara(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if((OI.w == 0xFF01))
	{
		Len.w = GetRequestChannelPara( DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if(OI.w == 0x4400)
	{
		Len.w = GetRequestAppLink(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if(OI.w == 0xF20D)
	{
		Len.w = GetRequestF20DPara(  DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
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
WORD api_GetProOadData( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
    return GetProOadData( 0x55, DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
}

//--------------------------------------------------
//功能描述: 获取单个OAD对应数据的长度
//         
//参数:    	DataType[in]: 	eData/eTagData
//         	pOAD[in]：获取数据长度的OAD
//			Number[in]：Array的个数  
//
//返回值:   数据长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容: Number:只有获取事件类长度时需要配置此参数，获取其他类长度时，此参数直接填写0
//--------------------------------------------------
WORD api_GetProOADLen(BYTE DataType, BYTE *pOAD, BYTE Number)
{
	BYTE ClassAttribute;
	TTwoByteUnion OI,Len;	
	
	Len.w = 0x8000;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	
    if( (DataType != eData) 
     &&(DataType != eTagData) 
     &&(DataType != eMaxData) )
	{
		return 0x8000;
	}
	
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
	
	if( (OI.w>=0x2000) && (OI.w<=0x2600) )
	{
		Len.w = GetRequestVariableLen(DataType,pOAD,0x55); //wxy 20230912 add
	}
	else if( (OI.w>=0x3000) && (OI.w<=0x330E) )//事件类
	{
		Len.w = GetProEventUnitLen(DataType,pOAD,Number);
	}
	else if( (OI.w>=0x4000) && (OI.w<=0x4FFF) )//OI:4000~4FFF 为参变量
	{
		Len.w = GetRequestMeterParaLen(DataType,pOAD);
	}
	else if( (OI.w == 0xF001) ||  (OI.w == 0xF402))// 文件类
	{
	//	Len.w = GetRequestFileParaLen( DataType,pOAD);
			Len.w = GetRequestMeterParaLen(DataType,pOAD);
	}
	else if( (OI.w == 0xF100)  )// ESAM  秘钥类
	{
		//Len.w = GetRequestESamPara( DataType,pOAD);
	}
	
	return Len.w;
}



WORD GetRequestAddTagEx( const BYTE *Tag, int nSum, BYTE *InBuf, BYTE *OutBuf )
{
//	WORD Len;
	BYTE i,n,Num=0,StructNum=0;
	WORD nLenght;	
	// BOOL bList=FALSE;
	int nOutLen=0;

//	memset( (BYTE*)&TagBuf.InBufLenth, 0x00, sizeof(TTagBuf) );
	
	n = 0;
	
	while(nSum)
	{
		switch(Tag[n])
		{
		case 0:
			n = 0;
			break;
		case Array_698:
			Num = Tag[n+1];
			OutBuf[nOutLen++] = Array_698;
			OutBuf[nOutLen++] = Num;
			// bList = TRUE;
			n += 2;
			for(i=0; i<Num; i++)
			{
				nOutLen += GetRequestAddTagEx((BYTE*)&Tag[n], 1, InBuf, &OutBuf[nOutLen]);
			}
			n += 2;
			//if(nSum == 1)
				nSum -= 1;
			continue;
		case Structure_698:
			StructNum = Tag[n+1];	
			OutBuf[nOutLen++] = Structure_698;
			OutBuf[nOutLen++] = StructNum;
			n += 2;

			nOutLen += GetRequestAddTagEx((BYTE*)&Tag[n], StructNum, InBuf, &OutBuf[nOutLen]);
			n += 2*StructNum;
			nSum -= 1;
			continue;
		default:
			TagBuf.OutBufLenth = 0;
			nLenght = AddNormalTag((BYTE*)&Tag[n], InBuf, &OutBuf[nOutLen]);
			nOutLen += nLenght;
			n += 2;
			nSum -= 1;
			break;
		}
		if(nOutLen >= 0x8000)
			break;
		if(Tag[n] == 0)
			break;
	}
	TagBuf.OutBufLenth = nOutLen;

	return nOutLen;

}
WORD GetTypeLenEx( BYTE DataType, const BYTE *Tag , int nSum)
{
	BYTE i,n,Num=0,StructNum=0;
	WORD Len,nLenght;	
	BOOL bList=FALSE;
	
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
	
	while(nSum)
	{
		switch(Tag[n])
		{
		case Array_698:
			Num = Tag[n+1];
			n += 2;
			if( DataType == eTagData )
				Len += 2;
			
			bList = TRUE;
			for(i=0; i<Num; i++)
			{
				Len += GetTypeLenEx(DataType, (BYTE*)&Tag[n], 1);
			}
			n += 2;
		//	if(nSum == 1)
				nSum -= 1;
			continue;
		case Structure_698:
			StructNum = Tag[n+1];
			n += 2;
			if( DataType == eTagData )
			{
				if(bList)
					Len += 2*Num;
				else
					Len += 2;
			}
			Len += GetTypeLenEx(DataType,(BYTE*)&Tag[n], StructNum);
			n += 2*StructNum;
			nSum -= 1;
			continue;
		default:
			nLenght = GetBasicTypeLen( DataType, (BYTE*)&Tag[n] );
			Len += nLenght;
			n += 2;
			nSum -= 1;
			break;
		}
		if(Len >= 0x8000)
			break;
		if(Tag[n] == 0)
			break;
	}
	
	return Len;
}
