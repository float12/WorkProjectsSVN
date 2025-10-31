//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ�
//��������	2016.12.27
//����		DL/T 698.45�������Э����ܶ�ȡC�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
#if( MAX_PHASE != 1 )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


//-----------------------------------------------
//				ȫ��ʹ�õı���������

//-----------------------------------------------
 
//-----------------------------------------------
//				���ļ�ʹ�õı���������
static const TScaler_Unit SU_KW			= 	{ -4, UNIT_KW 	};	//KW���㵥λ
static const TScaler_Unit SU_Kvar 		= 	{ -4, UNIT_KVAR 	};	//Kvar���㵥λ
static const TScaler_Unit SU_KVA 		= 	{ -4, UNIT_KVA 	};	//KVA���㵥λ

static const BYTE T_Demand[]			= { Array_698, 0xff, Structure_698, 02, Double_long_698, 4, DateTime_S_698, 7};			//�з����������ݱ�ʶ
static const BYTE T_UNDemand[]			= { Array_698, 0xff, Structure_698, 02, Double_long_unsigned_698, 4, DateTime_S_698, 7};//�޷����������ݱ�ʶ
//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
static const TCommonObj DemandObj[] =
{
	{  0x1010,  &SU_KW,   	T_UNDemand },//�����й��������
	{  0x1020,  &SU_KW,   	T_UNDemand },//�����й��������
	{  0x1030,  &SU_Kvar,  	T_Demand   },//����޹�1�������
	{  0x1040,  &SU_Kvar,  	T_Demand   },//����޹�2�������
	{  0x1050,  &SU_Kvar,  	T_UNDemand },//��һ�����������
	{  0x1060,  &SU_Kvar,  	T_UNDemand },//�ڶ������������
	{  0x1070,  &SU_Kvar,  	T_UNDemand },//���������������
	{  0x1080,  &SU_Kvar,  	T_UNDemand },//���������������
	#if( SEL_APPARENT_DEMAND == YES )	
	{  0x1090,  &SU_KVA,  	T_UNDemand },//���������������
	{  0x10A0,  &SU_KVA,  	T_UNDemand },//���������������
	#endif	
	{  0x1110,  &SU_KW,		T_UNDemand },//���������������й��������
	{  0x1120,  &SU_KW,  	T_UNDemand },//���������ڷ����й��������
	{  0x1130,  &SU_Kvar, 	T_Demand   },//��������������޹�1�������
	{  0x1140,  &SU_Kvar, 	T_Demand   },//��������������޹�2�������
	{  0x1150,  &SU_Kvar, 	T_UNDemand },//���������ڵ�һ�����������
	{  0x1160,  &SU_Kvar, 	T_UNDemand },//���������ڵڶ������������
	{  0x1170,  &SU_Kvar, 	T_UNDemand },//���������ڵ��������������
	{  0x1180,  &SU_Kvar, 	T_UNDemand },//���������ڵ��������������
	#if( SEL_APPARENT_DEMAND == YES )	
	{  0x1190,  &SU_KVA, 	T_UNDemand },//�������������������������
	{  0x11A0,  &SU_KVA, 	T_UNDemand },//���������ڷ��������������
	#endif	
};								
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
//��������:  ����OI���Ҷ�Ӧ��������
//         
//����:      OI[in]:	OI
//         
//����ֵ:    ��������������
//         
//��ע:  
//--------------------------------------------------
static BYTE SearchDemandOAD( WORD OI )
{
	BYTE i,Num;//����������ܳ���255

	Num = (sizeof(DemandObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//������ѭ��
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

	if( i == Num )//δ�ҵ�
	{
		return 0x80;
	}
	
	return 0x80;
}

//--------------------------------------------------
//��������:  ��ȡ��������
//         
//����:      
//			Dot[in]:		��ȡ���ݵ�С����λ����oxff--��ʾ��ԼĬ�ϵ�С��λ������Լ���ã�
//			*pOAD[in]:		OAD         
//			DemandIndex[in]:DemandObj�е�����         
//			*OutBuf[in]:	�������
//         
//����ֵ:   �����������ݳ��� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע: ����tag
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
		case 2://�ܼ����������������
			if( ClassIndex > (FPara1->TimeZoneSegPara.Ratio+1) )
			{
				return 0x8000;
			}
			
			DemandType = (OI.b[0]>>4);
			Num = ((ClassIndex == 0) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
			Ratio = ((ClassIndex == 0) ? 0 : (ClassIndex-1));
			Num = (Num+Ratio);	
			
			if( OI.b[1] == 0x11 )//��������������,OI.w = 0x1110ʱOI.b[1]=0x11
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
		case 3://���㼰��λ
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
//��������:  ���������ݽ��м�TAG
//         
//����:      *pOAD[in]��		OAD         
//           DemandIndex[in]:	DemandObj�е�����         
//           *InBuf[in]��		��Ҫ���tag������         
//           OutBufLen[in]��	OutBuf����       
//           *OutBuf[out]��		�������
//         
//����ֵ:    �������tag�����ݳ��� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע:  	��
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
		case 2://����
			if( ClassIndex > (FPara1->TimeZoneSegPara.Ratio+1) )
			{
				return 0x8000;
			}
			Lenth = (ClassIndex == 0) ? ((FPara1->TimeZoneSegPara.Ratio+1)*15+2) : 15;
			if( OI.b[1] == 0x11 )//�ڼ䶳������ֻ����,OI.w = 0x1110ʱOI.b[1]=0x11
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Lenth = (ClassIndex == 0) ? 17 : 15;
				Num = 1;
			}
			
			if( OutBufLen < Lenth )//��������С�������򲻽��в���
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

		case 3://���㼰��λ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( OutBufLen < 3 )//��������С�������򲻽��в���
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
//��������:  ��ȡ����
//         
//����:      DataType[in]��	eData/eTagData/eAddTag        
//           Dot[in] :		С����        
//           *pOAD[in]��	OAD         
//           *InBuf[in]��	��Ҫ��ӱ�ǩ������         
//           OutBufLen[in]������OutBuf�����ݳ���         
//           *OutBuf[in]��	���صĵ�������
//         
//����ֵ:    ������������ 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ���ʸ������ܳ���12
//--------------------------------------------------
WORD GetRequestDemand( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE DemandIndex;	
	BYTE Buf[(MAX_RATIO+1)*11+30];//����Դ�������BUF����
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
//��������:  ����TAG��ȡ��������
//         
//����:      
//			DataType[in]��	eData/eTagData
//			*pOAD[in]��		OAD
//         
//����ֵ:   ����OAD��Ӧ���ݵĳ���  0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
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

    if( DataType == eMaxData )//ʹ�����ֵ
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
		case 2://�ܼ����������������
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

		case 3://���㼰��λ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( DataType == eData )//Դ���ݳ���
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

