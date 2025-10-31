//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ�
//��������	2016.12.23
//����		DL/T 698.45�������Э�� ��ȡ����C�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "GDW698/GDW698.h"

#if ( SEL_DLT698_2016_FUNC == YES  )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
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
const TScaler_Unit SU_2Yuan 	= 	{ -2, UNIT_OTHER };//ԭ��Ϊ{ -2, UNIT_1Pvarh }���� ֻ��Ǯ���ļ���͸֧���ۼƹ�������
const TScaler_Unit SU_4Yuan 	= 	{ -4, UNIT_OTHER };//ԭ��Ϊ{ -4, UNIT_1Pvarh }���� �����
//�����������ͣ�����ֻΪ��ʽһ�£���ʹ��
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
const BYTE T_OctetString7[]	= { Octet_string_698,		7	};//����ͷ����ʱ���� CH,Cla,Ins,P1,P2,P31,P32,����CH:0x01--ESAM 0x00--CARD,esam����ͷΪCH���6�ֽڣ���ΪCH���5�ֽ�
const BYTE T_OctetString8[]	= { Octet_string_698,		8	};
const BYTE T_OctetString16[]= { Octet_string_698,	    16	};
const BYTE T_OctetStringFF[]= { Octet_string_698,	   0XFF	};
const BYTE T_TI[]			= { TI_698,				    3	};
const BYTE T_Bool[]			= { Boolean_698,			1   };
const BYTE T_Special[]		= { 0xff                        };//���⴦��
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
TTagPara TagPara; 	//��TAGʱʹ�õĲ���
TTagBuf  TagBuf;	//��TAGʱ�õ���buf����								
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
WORD RecursionGetProtocolDataLen( BYTE *Buf, WORD* Count );


//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
//��������:  ��ȡ�����������ͳ���
//         
//����:      DataType[in]��eData/eTagData/eProtocolData ��eData���κ����붼�ǻ�ȡ��TAG����         
//           *Tag[in]		Tag[0]--�������� Tag[1]--���ݳ���
//
//����ֵ:    ���ݳ���
//         
//��ע����:DataType ��=eProtocolData��tag����������ΪTag[0]--�������� Tag[1]--���ݳ���
//        DataType == eProtocolData��tag����������ΪTag[0]--��������+tag����
//--------------------------------------------------
static WORD GetBasicTypeLen( BYTE DataType, BYTE *Tag )
{
	WORD Len,Len1,i;
	TTwoByteUnion TmpLen;//Э�鲻�������ݴ���
	BYTE LenOffest;
	
	Len = 0;
	Len1 = 0;
	
	switch( Tag[0] )//��֧��TSA��MS��Region��RSD
	{
		case NULL_698:
			Len = ( (DataType==eData) ? 1 : 1 );
			break;
		#if (SEL_SEARCH_METER == YES )
		case Array_698://������Ԫ���������ݣ�ֻ���ѱ����õ���ֱ�ӷ���2���ֽ�
			Len = 2;
			break;
		#endif
		case Boolean_698:
		case Integer_698:
		case Unsigned_698:
		case Enum_698:
			Len = ( (DataType==eData) ? 1 : 2 );
			break;	
		
		case Bit_string_698://����Э��Ĭ��Bit_stringΪ8�ı������������8�ı��������ʣ���λ�� ��δ���ǲ�Ϊ8�ı�������� ֧�ֳ���127���ֽ�
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
					TmpLen.w = 1;// ��ֹ�建��
				}
				TmpLen.w = ((TmpLen.w-1)/8)+1;// ���ǲ�Ϊ8���������������Ҫ��+1
                Len = ( (DataType==eData) ? (TmpLen.w) : (TmpLen.w+LenOffest+1) );
            }
            else//Э��ģʽֻ��ȡ��TAG����
            {
                LenOffest = Deal_698DataLenth( Tag+1, TmpLen.b, ePROTOCOL_TYPE );
				if (TmpLen.w == 0)
				{
					TmpLen.w = 1; // ��ֹ�建��
				}
				TmpLen.w = ((TmpLen.w-1)/8)+1; // ���ǲ�Ϊ8���������������Ҫ��+1
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
            else//Э��ģʽֻ��ȡ��TAG����
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
            else//Э��ģʽֻ��ȡ��TAG����
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
            if( DataType == eProtocolData )//Э��ģʽֻ��ȡ��TAG����
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
	        if( DataType == eProtocolData )//Э��ģʽֻ��ȡ��TAG����
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
	        if( DataType == eProtocolData )//Э��ģʽֻ��ȡ��TAG����
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
            if( DataType == eProtocolData )//Э��ģʽֻ��ȡ��TAG����
            {
                LenOffest = Deal_698DataLenth( Tag+(1+4), TmpLen.b, ePROTOCOL_TYPE );//��ȡsid�Ĳ��������ݳ��� 
                Len1 = 1+4+LenOffest+TmpLen.w;
                LenOffest = Deal_698DataLenth( Tag+Len1, TmpLen.b, ePROTOCOL_TYPE ); //��ȡMAC�Ĳ��������ݳ��� 
                Len = Len1+LenOffest+TmpLen.w;
            }
            else
            {
                Len = 0x8000;
            }
	        break;
	        
	    case RCSD_698:
	        if( DataType == eProtocolData )//Э��ģʽֻ��ȡ��TAG����
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
//��������:  �������ͻ�ȡ���ݳ���
//         
//����:      DataType[in]��eData/eTagData        
//         Tag[in]��
//         			    Tag[0]--��������
//         				Tag[1]--��Ա����
//         				Tag[n]--����
//
//����ֵ:     WORD  �������ݳ���
//         
//��ע����: ֧��array �� array+struct+Normal��ʽ ���ڻ�ȡ���֧�ֵ��������ͳ���
//--------------------------------------------------
WORD GetTypeLen( BYTE DataType, const BYTE *Tag )
{
	BYTE i,n,Num;
	WORD Len;	
	
	Len = 0;
	n = 0;
	
	if( DataType == eMaxData )//��ȡ���Դ���ݳ��� ���ܻ�ȡ��TAG��󳤶�
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
	
	if( Tag[0] == Array_698 )//֧��array �� array+struct+Normal��ʽ
	{
		if( Num == 0 )//���������� ��null
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
			if( DataType == eData )//ֻ��ȡԴ����
			{
				Len = (GetBasicTypeLen( DataType, (BYTE*)&Tag[2] )*Num);
			}
			else
			{
				Len = (GetBasicTypeLen( DataType, (BYTE*)&Tag[2] )*Num+2);
			}
		}
	}
	else if( Tag[0] == Structure_698 )//ֻ֧��Structure + Normal
	{
		if( Num == 0 )//����������
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
//��������: �ݹ��ȡDATA��ʵ�����ݳ���
//         
//����:      Buf[in]��  data����
//           Count[in]���ݹ����ƴ���
//
//����ֵ:     WORD �������ݳ���
//         
//��ע����:ע��ݹ�ʹ�� �����޷�����
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
            Len += RecursionGetProtocolDataLen( (BYTE*)&Tag[Len], Count );//������ش���0x8000�϶�����MAX_APDU_SIZE
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
            Len += RecursionGetProtocolDataLen( (BYTE*)&Tag[Len], Count );//������ش���0x8000�϶�����MAX_APDU_SIZE
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
//��������: Э���л�ȡDATA��ʵ�����ݳ���
//         
//����:      Buf[in]��  data����
//
//����ֵ:     WORD  �������ݳ���
//         
//��ע����:ע��ݹ�ʹ�� �����޷����� ��������Ƕ��457��ʵ������ʱ��Ϊ3ms
//--------------------------------------------------
WORD GetProtocolDataLen( BYTE *Tag )
{
    WORD Count;
 
    Count = 500; 
    return RecursionGetProtocolDataLen( Tag, (WORD*)&Count );
}


//--------------------------------------------------
//��������:  ��ȡ�߼���
//         
//����:      
//         	 DataType[in]��	eData/eTagData/eAddTag
//           *pOAD[in] 		OAD       
//           *InBuf[in]		��ָ���������������tag         
//           OutBufLen[in] 	�߼�������        
//           *OutBuf[in]	����߼���	
//         
//����ֵ:    ����OutBuf���峤��
//         
//��ע����:  ��
//--------------------------------------------------
static WORD GetRequestLogicalName( BYTE DataType,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Lenth;

	Lenth = ( (DataType==eData) ? 2 : 4 );
	
	if( OutBufLen < Lenth )//�жϳ���
	{
		return 0x8000;
	}
	
	if( DataType == eData )//��ȡԴ����
	{
		memcpy(OutBuf,pOAD,2);
	}
	else if( DataType == eTagData )//��ȡԴ���ݼ�TAG
	{
		*(OutBuf++) = Octet_string_698;
		*(OutBuf++) = 2;
		memcpy(OutBuf,pOAD,2);
		OutBuf += 2;
	}
	else if( DataType == eAddTag )//��ָ������Դ��TAG
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
//��������:  �������tag����
//         
//����:        DataLen[in]��	TLV��ʽ�е�L                 
//           *pTagLen[out]�� ���TAG�ĳ���
//           *OutBuf[out]��	�������
//         
//����ֵ:    ���������򳤶�
//         
//��ע����:  �������tag���ȵ�OutBuf��������ӳ��ȷ���pTagLen
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
//��������:  �����������ͼ�TAG
//         
//����:       *Tag[in]     Tag[0]--��������  Tag[1]--���ݳ��� Tag[n]--����   
//          *InBuf[in]   ��Ҫ���tag������        
//          *OutBuf[in]  ���tag�����������
//         
//����ֵ:      ���tag����������ݳ���
//         
//��ע����:     ��InBuf�е����ݰ���Tag��ǩ��Ҫ����ӱ�ǩ�������OutBuf 
//			ע�ⲻ�������ݲ�Ҫ����0x7f���ֽ�
//          ��TAGʱ�������������Ϊ�����̶��������ͣ�����getbasictypelen�ĳ���ȡ����
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
		case Array_698://ֻ���ѱ���õ���ֱ�ӷ��ؿ�
			*(OutBuf++) = 1;
			*(OutBuf++) = 0;
			TagBuf.OutBufLenth += 2;
			break;
		#endif
		case Bit_string_698://bit_string�������⴦��
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
			Result = AddNormalLenthTag( Tag[1], (BYTE*)&Len, OutBuf );//TagBuf.OutBufLenth�������ں�������� ʹ��ʱ��ע��
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
//��������:  �ṹ���TAG
//         
//����:      *Tag[in]    Tag[0]--��������  Tag[1]--���ݳ��� Tag[n]--����   
//           *InBuf[in]	 ��Ҫ���tag������        
//           *OutBuf[in] ���tag�����������
//         
//����ֵ:    ���tag����������ݳ���
//         
//��ע����:  ��InBuf�е����ݰ���Tag��ǩ��Ҫ����ӱ�ǩ�������OutBuf 
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
	
	for( i=0; i<Num; i++ )//���ν���normal�������
	{
		//����Tag+Data�ĳ���
		WithTagLen = AddNormalTag( (BYTE*)&Tag[2+n], InBuf, OutBuf );
		if( WithTagLen == 0x8000 )
		{
			return WithTagLen;
		}
		Len += WithTagLen;
		n = n+2;
	}

	return Len;//���ⷵ������
}

//--------------------------------------------------
//��������:  �����TAG
//         
//����:      *Tag[in]    Tag[0]--��������  Tag[1]--���ݳ��� Tag[n]--����   
//           *InBuf[in]	 ��Ҫ���tag������        
//           *OutBuf[in] ���tag�����������
//         
//����ֵ:    ���tag����������ݳ���
//         
//��ע����:  ��InBuf�е����ݰ���Tag��ǩ��Ҫ����ӱ�ǩ�������OutBuf 
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

	if( Num == 0 )//���Ϊarray����Ϊ�� Ĭ�Ϸ���01 00
	{
		OutBuf[0] = NULL_698;
		TagBuf.OutBufLenth +=1;
		return 1;
	}
	
	OutBuf[0] = Tag[0];
	OutBuf[1] = Num;
	Len +=2;
	TagBuf.OutBufLenth +=2;

	for( i=0; i<Num; i++ )//���ν���normal�������
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
//��������:  ��ȡ������
//����	:
//			pBuf[out]
//����ֵ:	TRUE	FALSE
//ע:����ʱ������ΪHEX
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
//��������:  ����Ԫ����������TCommonObj_t��Type2��ƫ�Ƶ�ַ
//         
//����:      ClassIndex[in]��	Ԫ������         
//           *Tag[in]��			��ǩ
//         
//����ֵ:    ���� TCommonObj �� Type2 ��ƫ��
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetTagOffsetAddr(BYTE ClassIndex, const BYTE *Tag)
{
	if( ClassIndex == 0 )
	{
		return 0;
	}
	else if( Tag[0] == Array_698 )//֧��array �� array+struct+Normal��ʽ
	{
		return 2;
	}
	else if( Tag[0] == Structure_698 )//ֻ֧��Structure + Normal
	{
		if( ClassIndex <= Tag[1])//����classindex����
		{
			return (2*(ClassIndex));
		}
	}

	return 0;
}

//--------------------------------------------------
//��������:  �������������Tag
//         
//����:      *Tag[in]    Tag[0]--��������  Tag[1]--���ݳ��� Tag[n]--����   
//           *InBuf[in]	 ��Ҫ���tag������        
//           *OutBuf[in] ���tag�����������
//         
//����ֵ:    ���tag����������ݳ���
//         
//��ע����:  ��InBuf�е����ݰ���Tag��ǩ��Ҫ����ӱ�ǩ�������OutBuf 
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
//��������:  698��ȡ����OAD����
//         
//����:	
//		GetDataType[in]	GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//		Ch[in]:			��Լͨ��
//		DataType[in]:	eData/eTagData/eAddTag
//		Dot[in]:  		��ȡ���ݵ�С����λ����oxff--��ʾ��ԼĬ�ϵ�С��λ������Լ���ã�
//		pOAD[in]��		��ȡ���ݵ�OAD
//		InBuf[in]��		����Ҫ��Tag������(ע:������¼����е� �¼��ϱ�״̬(0x3300)����̶����б�(0x3302)���¼������б�(0x330c)ʱ����һ���ֽ�InBuf[0]��Ҫ���Array�ĸ���Number)
//		OutBufLen[in]��	����OutBufLen�ĳ���
//		OutBuf[out]:  	��ȡ���ݴ��OutBuf��
//
//����ֵ: �������ݳ���	0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
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
	ClassAttribute = (pOAD[2]&0x1F);//bit0��bit4�����ʾ�������Ա��
	memset( (BYTE*)&TagBuf.InBufLenth, 0x00, sizeof(TagBuf) );
	memset( (BYTE*)&TagPara.Array_Struct_Num, 0x00, sizeof(TagPara) );
	
	if( (SelSecPowerConst==NO) && (OI.w==0x300e) )//������Դ�����¼��ں궨��δ��ʱ���ܽ��ж�ȡ
	{
		return 0x8000;
	}
	
	if( ClassAttribute == 1 )//������1���߼������ڴ�ͳһ������������������������
	{
		return GetRequestLogicalName( DataType, pOAD, InBuf, OutBufLen, OutBuf );
	}
	
	if (IsModuleOI(OI.w,eModuleOrdOI)||((pOAD[2]==0x06)&&IsModuleOI(OI.w,eModuleEventOI)))//��ͨ��չģ������¼����ò���
	{
		Len.w = GetModuleRequestVariable(DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( OI.w < 0x1000 )//������
	{
		Len.w = GetRequestEnergy( GetDataType, DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	#if(MAX_PHASE != 1)
	else if( (OI.w>=0x1000) && (OI.w<=0x11A3) )//���������
	{
		Len.w = GetRequestDemand( DataType, Dot, pOAD, InBuf,OutBufLen, OutBuf);
	}
	#endif
	else if( (OI.w>=0x2000) && (OI.w<=0x2709) )//������
	{
		Len.w = GetRequestVariable( Ch, DataType, Dot, pOAD, InBuf,  OutBufLen, OutBuf);
	}
	else if( ((OI.w>=0x3000) && (OI.w<=0x3320))||IsModuleOI(OI.w,eModuleEventOI) 
            #if( SEL_TOPOLOGY == YES )
            || (OI.w ==0x3E03)
            #endif
     )//�¼���
	{
		Len.w = GetProEventRecord( Ch,DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( ((OI.w>=0x4000) && (OI.w<=0x4FFF) ) 
	  #if (SEL_SEARCH_METER == YES )
	  || (OI.w == 0x6002) 
	  #endif
	)//�α���
	{
		Len.w = GetRequestMeterPara( Ch,DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
	}
	else if( (OI.w>=0x5000) && (OI.w<=0x5011) )//������
	{
		Len.w = GetProFreezeRecord( DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf );
	}
	else if(OI.w==0x6000)//�ɼ���
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
	else//������
	{
		Len.w = GetOtherData( Ch, DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf );
	}
	
	return Len.w;
}

//--------------------------------------------------
//��������:  698��ȡ����OAD���ݵĶ���ӿں���
//         
//����:	
//		DataType[in]:	eData/eTagData/eAddTag
//		Dot[in]:  		��ȡ���ݵ�С����λ����oxff--��ʾ��ԼĬ�ϵ�С��λ������Լ���ã�
//		pOAD[in]��		��ȡ���ݵ�OAD
//		InBuf[in]��		����Ҫ��Tag������(ע:������¼����е� �¼��ϱ�״̬(0x3300)����̶����б�(0x3302)���¼������б�(0x330c)ʱ����һ���ֽ�InBuf[0]��Ҫ���Array�ĸ���Number)
//		OutBufLen[in]��	����OutBufLen�ĳ���
//		OutBuf[out]:  	��ȡ���ݴ��OutBuf��
//
//����ֵ: �������ݳ���	0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD api_GetProOadData( eGetDataType GetDataType, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
    return GetProOadData( GetDataType, 0x55, DataType, Dot, pOAD, InBuf, OutBufLen, OutBuf);
}

//--------------------------------------------------
//��������: ��ȡ����OAD��Ӧ���ݵĳ���
//         
//����:    	GetDataType[in]	GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//			DataType[in]: 	eData/eTagData
//         	pOAD[in]����ȡ���ݳ��ȵ�OAD
//			Number[in]��Array�ĸ���  
//
//����ֵ:   ���ݳ���  0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����: Number:ֻ�л�ȡ�¼��೤��ʱ��Ҫ���ô˲�������ȡ�����೤��ʱ���˲���ֱ����д0
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
	
	if( OI.w > 0x11A3 )//�ǵ��ܡ��������Ͳ�֧��eMaxData
	{
        if( DataType == eMaxData )
        {
            DataType = eData;
        }
	}
	
	if( ClassAttribute == 1 )//����1 �߼���
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
	else if( (OI.w>=0x3000) && (OI.w<=0x3315) )//�¼��� ԭΪ330E 2018/6/14��Ϊ3315 wlk
	{
		Len.w = GetProEventUnitLen(DataType,pOAD,Number);
	}
	else if( (OI.w>=0x4000) && (OI.w<=0x4FFF) )//OI:4000~4FFF Ϊ�α���
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

