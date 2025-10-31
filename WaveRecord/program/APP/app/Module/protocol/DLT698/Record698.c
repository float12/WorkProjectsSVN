//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ�
//��������	2016.01.06
//����		DL/T 698.45�������Э����ܶ�ȡC�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//��ʹ���¼���������궨������Ϊ��������Ƕ�̬�ģ��¼�����С������»����������ʹ������¼�����������������
#define MAX_EVENT_PROBUF_LEN         (65*5+50)

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

typedef struct TEventRecordObj_t
{
	WORD				OI;				//698�����ʶ��OI�����ʶ
	BYTE         		class;			//������
	const BYTE			*ConfigPara;
}TEventRecordObj;


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//-----------------------------------------------
//				���ļ�ʹ�õı���������

//class9��������3���ݱ�ʶ	�����������Ա�	
static const BYTE T_FreezeRecord3[]		= 
{ 
	Array_698,			0XFF,
	Structure_698,		3,		
	Long_unsigned_698,	2,				//��������
	OAD_698,			4,				//������������������
	Long_unsigned_698,	2,				//�洢���
};
//�¼����������б����ݱ�ʶ+�����¼��б�
static const BYTE T_EventRTable[]		= 
{ 
	Array_698,		0XFF,
	OAD_698,		4,
};

//���ϱ��¼������б�
static const BYTE T_EventReportTable[]	= 
{ 
    Array_698,      0XFF,
    OI_698,         2,
};

//�¼������б�		
static const BYTE T_EventClearTable[]	= 
{ 
	Array_698,		0XFF,
	OMD_698,		4,
};

//��ǰ��¼��
static const BYTE T_EventNowNum24[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,				//��¼��1��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��2��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��3��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��4��ǰ��¼��
};
//��ǰ��¼��-NULL
static const BYTE T_EventNowNum24BNull[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,				//��¼��1��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��2��ǰ��¼��
	NULL_698,	        1,				//��¼��3��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��4��ǰ��¼��
};
//��ǰ��¼��-NULL
static const BYTE T_EventNowNum24ABCNull[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,				//��¼��1��ǰ��¼��
	NULL_698,			1,				//��¼��2��ǰ��¼��
	NULL_698,	        1,				//��¼��3��ǰ��¼��
	NULL_698,			1,				//��¼��4��ǰ��¼��
};
//��ǰ��¼��-NULL
static const BYTE T_EventNowNum24Null[]		= 
{ 
	Structure_698,		4,
	NULL_698,			1,				//��¼��1��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��2��ǰ��¼��
	Long_unsigned_698,  2,				//��¼��3��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��4��ǰ��¼��
};
//��ǰ��¼��-NULL
static const BYTE T_EventNowNum24NullBNull[]		= 
{ 
	Structure_698,		4,
	NULL_698,			1,				//��¼��1��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��2��ǰ��¼��
	NULL_698,			1,				//��¼��3��ǰ��¼��
	Long_unsigned_698,	2,				//��¼��4��ǰ��¼��
};


//��ǰֵ��¼�� //���س�����Ҫ���⴦�� ��ע��---jwh
static const BYTE T_EventNowTable24[]	= 
{ 
	Array_698,					0xff,
	Structure_698,				2,
	Double_long_unsigned_698,	4,		//�¼���������
	Double_long_unsigned_698,	4,		//�¼��ۼ�ʱ��
};

//��ǰֵ��¼�� //���س�����Ҫ���⴦�� ��ע��---jwh
static const BYTE T_EventNowTable7[]	= 
{ 
	Structure_698,				2,
	Double_long_unsigned_698,	4,
	Double_long_unsigned_698,	4,
};
//�¼����ò���
static const BYTE T_EventUNLUNLDU[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,
	Long_unsigned_698,	2,
	Double_long_698,	4,
	Unsigned_698,		1,
};
//�¼����ò���
static const BYTE T_EventUNLU[]			= 
{ 
	Structure_698,		2,
	Long_unsigned_698,	2,
	Unsigned_698,		1,
};
//�¼����ò���
static const BYTE T_EventUNLDU[]		= 
{ 
	Structure_698,		3,
	Long_unsigned_698,	2,
	Double_long_698,	4,
	Unsigned_698,		1,
};
//�¼����ò���
static const BYTE T_EventUNLDDU[]		= 
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,
	Double_long_698,	4,
	Double_long_698,	4,
	Unsigned_698,		1,
};
//�¼����ò���
static const BYTE T_EventDU[]			= 
{ 
	Structure_698,		2,
	Double_long_698,	4,
	Unsigned_698,		1,
};
	//�¼����ò���
static const BYTE T_EventDLU[]			= 
{ 
	Structure_698,		3,
	Double_long_698,	4,
	Long_unsigned_698,	2,
	Unsigned_698,		1,
};
//�¼����ò���
static const BYTE T_EventUNDU[]			= 
{ 
	Structure_698,			    2,
	Double_long_unsigned_698,   4,
	Unsigned_698,		    	1,
};
//�¼����ò���
static const BYTE T_EventLU[]			= 
{ 
	Structure_698,	2,
	Long_698,		2,
	Unsigned_698,	1,
};
//�¼����ò���
static const BYTE T_EventU[]			= 
{ 
	Structure_698,	1,
	Unsigned_698,	1,
};
//�¼����ò��� //��ȡ����tagʱ��ע��ʹ��
static const BYTE T_EventNull[]			= 
{ 
	Structure_698,	1,
	NULL_698,	    0,
};
//ʧѹͳ��
static const BYTE T_EventLostVCount[]	= 
{ 
	Structure_698,				4,
	Double_long_unsigned_698,	4,
	Double_long_unsigned_698,	4,
	DateTime_S_698,			    7,
	DateTime_S_698,			    7,
};

//��������Ȧ���
static const BYTE T_EventRogowskiCount[]	= 
{ 
	Structure_698,				2,
	Long_unsigned_698,			2,
	Long_unsigned_698,			2,
};

//�ն˳���ʧ���¼�
static const BYTE T_EventRCommFailCount[]	= 
{ 
	Structure_698,				2,
	Unsigned_698,				1,
	NULL_698,	    			0,
};

//�¼��ϱ�״̬ //���س�����Ҫ���⴦�� ��ע��---jwh
static const BYTE T_EventReportStatus[]	= 
{ 
	Array_698,			0xff,
	Structure_698,		2,
	OAD_698,			4,
	Unsigned_698,		1,
};

//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
static const TEventRecordObj EventRecordObj[] =
{
#if( MAX_PHASE != 1 )
	{  0x3000, 	eclass24,	T_EventUNLUNLDU	},	//���ܱ�ʧѹ�¼�
	// {  0x3001, 	eclass24,	T_EventUNLU	   	},	//���ܱ�Ƿѹ�¼�
	// {  0x3002, 	eclass24,	T_EventUNLU   	},	//���ܱ��ѹ�¼�
	// {  0x3003, 	eclass24,	T_EventUNLDU	},	//���ܱ�����¼�
	// {  0x3004, 	eclass24,	T_EventUNLDDU	},	//���ܱ�ʧ���¼�
	// {  0x3006, 	eclass24,	T_EventUNLDU	},	//���ܱ�����¼�
	// {  0x3007, 	eclass24,	T_EventDU		},	//���ܱ��ʷ����¼�
	// {  0x3008, 	eclass24,	T_EventDU		},	//���ܱ�����¼�
	// {  0x3009, 	eclass7,	T_EventUNDU		},	//���ܱ������й����������¼�
	// {  0x300A, 	eclass7,	T_EventUNDU		},	//���ܱ����й����������¼�
	// {  0x300B, 	eclass24,	T_EventUNDU		},	//���ܱ��޹����������¼�
	// {  0x303B, 	eclass24,	T_EventLU		},	//���ܱ��������������¼�
	// {  0x300D, 	eclass7,	T_EventNull		},	//���ܱ�ȫʧѹ�¼�
	// {  0x300E, 	eclass7,	T_EventU		},	//���ܱ�����Դ�����¼�
	// {  0x300F, 	eclass7,	T_EventU		},	//���ܱ��ѹ�������¼�
	// {  0x3010, 	eclass7,	T_EventU		},	//���ܱ�����������¼�
	// {  0x3014, 	eclass7,	T_EventNull		},	//���ܱ����������¼�
	// {  0x301C, 	eclass7,	T_EventNull		},	//���ܱ���ť���¼�
	// {  0x301D, 	eclass7,	T_EventLU		},	//���ܱ��ѹ��ƽ���¼�
	// {  0x301E, 	eclass7,	T_EventLU		},	//���ܱ������ƽ���¼�
	// {  0x3023, 	eclass7,	T_EventNull		},	//���ܱ��޹���Ϸ�ʽ����¼�
	// {  0x302A, 	eclass7,	T_EventNull		},	//���ܱ�㶨�ų������¼�
	// {  0x302D, 	eclass7,	T_EventLU		},	//���ܱ�������ز�ƽ���¼�	
#endif
    // {  0x3021,  eclass7,    T_EventNull     },  //���ܱ�ڼ��ձ���¼�
	// {  0x3005, 	eclass24,	T_EventDU		},	//���ܱ�����¼�
	// {  0x3011, 	eclass7,	T_EventU		},	//���ܱ�����¼�
	// {  0x3012, 	eclass7,	T_EventNull		},	//���ܱ����¼�
	// {  0x3013, 	eclass7,	T_EventNull		},	//���ܱ������¼�
	// {  0x3015, 	eclass7,	T_EventNull		},	//���ܱ��¼������¼�
	// {  0x3016, 	eclass7,	T_EventNull		},	//���ܱ�Уʱ�¼�
	// {  0x3017, 	eclass7,	T_EventNull		},	//���ܱ�ʱ�α����¼�
	// {  0x3018, 	eclass7,	T_EventNull		},	//���ܱ�ʱ�������¼�
	// {  0x3019, 	eclass7,	T_EventNull		},	//���ܱ������ձ���¼�
	// {  0x301A, 	eclass7,	T_EventNull		},	//���ܱ�����ձ���¼�
	// {  0x301B, 	eclass7,	T_EventNull		},	//���ܱ����¼�
	// {  0x301F, 	eclass7,	T_EventNull		},	//���ܱ���բ�¼�
	// {  0x3020, 	eclass7,	T_EventNull		},	//���ܱ��բ�¼�
	// {  0x3022, 	eclass7,	T_EventNull		},	//���ܱ��й���Ϸ�ʽ����¼�
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{  0x3024, 	eclass7,	T_EventNull		},	//���ܱ���ʲ��������¼�
	{  0x3025, 	eclass7,	T_EventNull		},	//���ܱ���ݱ����¼�
	#endif
	{  0x3026, 	eclass7,	T_EventNull		},	//���ܱ���Կ�����¼�
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{  0x3027, 	eclass7,	T_EventNull		},	//���ܱ��쳣�忨�¼�
	{  0x3028, 	eclass7,	T_EventNull		},	//���ܱ����¼
	{  0x3029, 	eclass7,	T_EventNull		},	//���ܱ��˷Ѽ�¼
	#endif	
	// {  0x302B, 	eclass7,	T_EventNull		},	//���ܱ��ɿ��������¼�
	// {  0x302C, 	eclass7,	T_EventNull		},	//���ܱ��Դ�쳣�¼�
	// {  0x302E, 	eclass7,	T_EventNull		},	//���ܱ�ʱ�ӹ����¼�
	// {  0x302F, 	eclass7,	T_EventU		},	//���ܱ����оƬ�����¼�
	// {  0x303C, 	eclass7,	T_EventNull		},	//���ܱ�㲥Уʱ�¼�
	// {  0x3040, 	eclass7,	T_EventDLU		},	//�������ߵ�����ƽ���¼�
	// {  0x3320, 	eclass8,    T_EventRTable	},	//�����¼��б�
	// {  0x3300, 	eclassTag,	T_EventNull		},	//�¼�ͨ���ϱ�״̬
	// {  0x3302, 	eclassTag,  T_EventNull		},	//��̼�¼�¼���Ԫ
	// {  0x3308, 	eclassTag,  T_EventNull		},	//���ܱ����������¼���Ԫ
	// {  0x330C, 	eclassTag,  T_EventNull		},	//�¼������¼���¼��Ԫ
	// {  0xBD00, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD01, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD02, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD03, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD04, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD05, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD06, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD07, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD08, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0xBD09, 	eclass7,	T_EventNull		},	//��չ�¼�
	// {  0x30E0, 	eclass7,	T_EventNull		},	//���׮�澯
	// {  0x30E1, 	eclass7,	T_EventNull		},	//���׮����
	// {  0x30E2, 	eclass7,	T_EventNull		},	//���׮ͨ���쳣
	// {  0x3053, 	eclass7,	T_EventRogowskiCount},	//������Ȧ����¼�
	// {  0x310F, 	eclass7,	T_EventRCommFailCount},	//�ն˳���ʧ���¼�
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{  0x3310, 	eclassTag,  T_EventNull		},	//3310	8	�쳣�忨�¼���¼��Ԫ
	{  0x3311, 	eclassTag,  T_EventNull		},	//3311	8	�˷��¼���¼��Ԫ
	#endif
	{  0x3104, 	eclass7,	T_EventNull		},	//�ն�״̬����λ�¼�
    #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
    {  0x3111, 	eclass7,	T_EventNull		}, //����δ֪���ܱ��¼�
    #endif
    #if (SEL_TOPOLOGY == YES )
    {  0x3E03, 	eclass7,	T_EventNull		}, //���������ź�ʶ�����¼�
    {  0x4E07, 	eclass7,	T_EventNull		}, //���������ź�ʶ����
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
//��������:  ��ȡ�������
//         
//����:      
//          *pOAD[in]��	OAD                 
//          *pBuf[out]���������
//         
//����ֵ:    ���ض��ӳ��ȼ���Ա������Array_Struct_Num��
//         
//��ע����:  �����������Ա�
//--------------------------------------------------
static WORD GetFreezeData9( BYTE *pOAD, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	TTwoByteUnion OI;
	WORD Result;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Result = 0x8000;
	
	switch( ClassAttribute )
	{
		case 3://��������������Ա�
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
//��������:  �������ݼ�TAG
//         
//����:      
//         	*pOAD[in]��		OAD         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����:ֻ�Զ��������3 �����������Ա�  
//--------------------------------------------------
static WORD AddTagFreezeData9(BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 3://�����������Ա�
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
			if( TagPara.Array_Struct_Num == 0 )//�����������Ա������� ����01 00 
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
//��������:  ���ݱ������¼���¼OAD
//         
//����:      OI[in]��OI
//         
//����ֵ:    �¼�����
//         
//��ע����:  ��
//--------------------------------------------------
static BYTE SearchEventRecordOAD( WORD OI )
{
	BYTE i,Num;//����������ܳ���255

	Num = (sizeof(EventRecordObj)/sizeof(TEventRecordObj));
	if( Num >= 0x80 )//������ѭ��
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

	if( i == Num )//δ�ҵ�
	{
		return 0x80;
	}
	
	return 0x80;
}

//--------------------------------------------------
//��������: ���¼���Ԫ���Tag
//         
//����:      
//         	*pOAD[in]��		OAD    
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]�� ����Ļ��峤��        
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����:	�¼��ϱ�״̬
//			��̶����б�
//			�����ڼ��������ֵ
//			�¼������б�
//--------------------------------------------------
static WORD AddTagEventUnit(BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0x8000;
	
	switch( OI.w )
	{
		case 0x3300://�¼��ϱ�״̬
			if( (ClassIndex==0) && (ClassAttribute==2) )//ֻ֧�ֶ�����2���м�TAG
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
			if( ClassIndex == 6 )//��̶����б�
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
			if( ClassIndex == 6 )//�����ڼ��������ֵ
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
			else if( ClassIndex == 7 )//�����ڼ��������ֵ����ʱ��
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
			if( ClassIndex == 6 )//�¼������б�
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
			//0X06021033			//�쳣�忨�¼���¼��Ԫ0x3310--�����к�	octet-string��
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
			//0X07021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨������Ϣ��	unsigned��
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
			//����ͷ����ʱ���� CH,Cla,Ins,P1,P2,P31,P32,����CH:0x01--ESAM 0x00--CARD,esam����ͷΪCH���6�ֽڣ���ΪCH���5�ֽ�
			//0X08021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨��������ͷ	octet-string��
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
			//0X09021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨������Ӧ״̬  long-unsigned��
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
			if( ClassIndex == 6 )//0X06021133--�˷��¼���¼��Ԫ0x3311--�˷ѽ��      double-long-unsigned����λ��Ԫ�����㣺-2��
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
//��������: ��ȡ�¼���class7���������ݣ�����Tag��
//         
//����:     
//          *pOAD[in]��	OAD         
//          Sch[in]��   EventRecordObj�е��¼�����      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: ���������2�������
//--------------------------------------------------
static WORD GetEventData7( BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex;
	#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼
	BYTE i, Num;
	#endif
	WORD Type,Result,Lenth;
	BYTE *pBufBak = pBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 3://�¼������������Ա�
			Result = api_ReadEventAttribute( OI.w,ClassIndex,(MAX_EVENT_PROBUF_LEN),pBuf );
			if( (Result==0) || (Result==0x8000) )
			{
				return Result;
			}
			
			TagPara.Array_Struct_Num = (Result/sizeof(DWORD));
			pBuf += Result;
			break;

		case 4://��ǰ��¼��
		case 5://����¼��
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

		case 6://���ò���
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

		case 7://��ǰֵ��¼��
			#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼
			if( OI.w == 0x3023 )//���ܱ��޹���Ϸ�ʽ����¼�
			{
				if( ClassIndex > 2 )//Ĭ�ϵ�ǰֵ��¼���ܳ���
				{
					return 0x8000;
				}
				
				Num = ( ClassIndex==0 ) ? 2 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				for( ; i<Num; i++)
				{
					Result = api_ReadCurrentRecordTable_Q_Combo(i,pBuf);//���޹���Ϸ�ʽ1�����֡��޹���Ϸ�ʽ2�����ֱ�̴�����
					if( Result == FALSE )
					{
						return 0x8000;
					}
					pBuf += 9;
				}
				break;	
			}
			else
			#endif//#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼	
			{		
				if( ClassIndex > 1 )//Ĭ�ϵ�ǰֵ��¼���ܳ���
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

		case 8://�ϱ���ʶ
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

		case 9://��Ч��ʶ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			pBuf[0] = 1;			
			pBuf += 1;
			break;
		case 10://ʱ��״̬��¼���=array ʱ��״̬
			#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼
			if( OI.w == 0x3023 )//���ܱ��޹���Ϸ�ʽ����¼�
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
					if( (pBuf[0] == 0x00)&&(pBuf[1] == 0x00)&&(pBuf[2] == 0x00)&&(pBuf[3] == 0x00) )//�����վ�Ϊ0
					{
						pBuf += 2;//����ʱ��Ϊ1�ֽ�NULL,����ʱ��1�ֽ�ΪNULL
					}
					else
					{		
						pBuf += 8;//����ʱ��7�ֽ�,����ʱ��1�ֽ�ΪNULL
					}
				}
			}
			else
			#endif
			{
				if( ClassIndex > 1 )//Ĭ�ϵ�ǰֵ��¼���ܳ���
				{
					return 0x8000;
				}
	
				Result = api_ReadCurrentRecordTable(OI.w,0,0,pBuf);
				if(Result == 0)
				{
					return 0x8000;
				}				
				pBuf += Result;//15;//����Դ1�ֽ�ΪNULL������ʱ��7�ֽ�,����ʱ���ݿ���7�ֽ�
			}
			break;
		case 11://�ϱ���ʽ
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
		case 29://����11���Ƿ��忨�ܴ�������=double-long-unsigned
			if( OI.w != 0x3027 )//��������쳣�忨
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
//��������:  �¼���class7�������������Tag
//         
//����:      
//         	*pOAD[in]��		OAD
//			Sch[in]��		EventRecordObj�е��¼�����         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: ���������2�������  
//--------------------------------------------------
static WORD AddTagEventData7(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼
	BYTE i, Num;
	#endif
	BYTE j;
	BYTE *pBufBak;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	pBufBak = OutBuf;
	
	switch( ClassAttribute )
	{
		case 3://�����������Ա�
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
			if( TagPara.Array_Struct_Num == 0 )//�����������Ա������� ����01 00 
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
		case 4://��ǰ��¼��
		case 5://����¼��
		case 8://�ϱ���ʶ
		case 9://��Ч��ʶ
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

		case 6://���ò���
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

		case 7://��ǰֵ��¼��
			#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼
			if( OI.w == 0x3023 )//���ܱ��޹���Ϸ�ʽ����¼�
			{
				if( ClassIndex > 2 )//Ĭ�ϵ�ǰֵ��¼���ܳ���
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
				if( OutBufLen < Lenth )//���ݳ��Ȳ���
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
				if( ClassIndex > 1 )//Ĭ�ϵ�ǰֵ��¼���ܳ���1
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

				if( OutBufLen < Lenth )//���ݳ��Ȳ���
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
		case 10://ʱ��״̬��¼���=array ʱ��״̬			//@@@@@@@
			if( OutBufLen < (6+8+8) )//�����С������ʱ�䡢����ʱ����п���
			{
				return 0;
			}
			#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼
			if( OI.w == 0x3023 )//���ܱ��޹���Ϸ�ʽ����¼�
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
					if( OutBufLen < (2+(6+8+8)*2) )//�����С��2�෢��ʱ�䡢����ʱ����п���
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
						if( InBuf[0] == 0x00 )//û�з���ʱ�䡢����ʱ��
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
				if( ClassIndex > 1 )//Ĭ�ϵ�ǰֵ��¼���ܳ���1
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
				*(OutBuf++) = InBuf[0];//����Դ
                InBuf++;
				*(OutBuf++) = Structure_698;
				*(OutBuf++) = 2;
				for( j=0; j<2; j++ )
				{
					if( InBuf[0] == 0x00 )//û�з���ʱ�䡢����ʱ��
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
		case 11://�ϱ���ʽ
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
		case 29://����11���Ƿ��忨�ܴ�������=double-long-unsigned
			if( OI.w != 0x3027 )//��������쳣�忨
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
//��������: ��ȡ�����¼��б�class8���������ݣ�����Tag��
//         
//����:
//			Ch[in]��	��Լͨ��     
//          *pOAD[in]��	OAD         
//          Sch[in]��   EventRecordObj�е��¼�����      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: ����2(�����¼��б�),����3(���ϱ��¼������б�)
//--------------------------------------------------
static WORD GetNewEventListData( BYTE Ch, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Result;
	BYTE* pBufBak = pBuf;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	
	switch( ClassAttribute )
	{
		case 2://�¼������������Ա�
		case 3://���ϱ��¼������б�
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
//��������:  �����¼��б��������Tag
//         
//����:      
//         	*pOAD[in]��		OAD
//			Sch[in]��		EventRecordObj�е��¼�����         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: ����2(�����¼��б�),����3(���ϱ��¼������б�)  
//--------------------------------------------------
static WORD AddTagNewEventList(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Result,Lenth;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 2://�¼������������Ա�
		case 3://���ϱ��¼������б�
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

		    if(TagPara.Array_Struct_Num == 0)   // �����¼��б������� ����01 00
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
//��������: ��ȡ�¼���class24���������ݣ�����Tag��
//         
//����:     
//          *pOAD[in]��	OAD         
//          Sch[in]��   EventRecordObj�е��¼�����      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: ���������6~9�������
//--------------------------------------------------
static WORD GetEventData24( BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num;
	TTwoByteUnion OI;	
	WORD Result,Lenth;
	const BYTE *Tag;
	BYTE *pBufBak = pBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 2://�¼������������Ա�
			Result = api_ReadEventAttribute( OI.w, ClassIndex, MAX_EVENT_PROBUF_LEN, pBuf );
			if( (Result==0) || (Result==0x8000) )
			{
				return Result;
			}
			
			TagPara.Array_Struct_Num = (Result/sizeof(DWORD));
			pBuf += Result;
			break;

		case 3://��ǰ��¼��
			if( ClassIndex > T_EventNowNum24[1] )
			{
				return 0x8000;
			}
			
			//���������¼����޹����������¼���ǰ��¼����������
			if( (OI.w==0x300b)|| (OI.w==0x3007)|| (OI.w==0x303B) )
			{
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;

				for( ; i<Num; i++)
				{
				    if( (OI.w==0x3007) && (MeterTypesConst == METER_3P3W) )//�������߱�������B�����⴦��
				    {
                        if( i == 2 )//B�ഫ��
                        {
                            *(pBuf++) = 0;
                            continue;
                        }
				    }
				    if( (OI.w==0x303B) && (MeterTypesConst == METER_3P3W) )//�������߱�������������ABC�̶�����0
				    {
                        if( i != 0 )//A��B��C�ഫ��
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
				#if( MAX_PHASE != 1 )	//�����ȡ		
				Num = ( ClassIndex==0 ) ? 4 : 1; 
				i = ( ClassIndex==0 ) ? 0 : (ClassIndex-1);
				Num += i;
				for( ; i<Num; i++)
				{
				  	if(MeterTypesConst == METER_3P3W)//�������߱�������B�����⴦��
				    {
                        if( i == 2 )//B�ഫ��
                        {
                            *(pBuf++) = 0;
                            continue;
                        }
				    }
					if( i == 0 )//�ܴ���
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
				#else//�����ȡ
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
			
		case 4://����¼��
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
		case 5://���ò���
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

		case 10://��ǰֵ��¼��
			if( ClassIndex > 4 )//Ĭ�ϵ�ǰֵ��¼���ܳ���4
			{
				return 0x8000;
			}
			
			//���ʷ����¼����޹����������¼���ǰ��¼����������
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
				#if( MAX_PHASE != 1 )	//��TAG���⴦��		
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
					
					if( i == 0 )//���ܽ��е�������
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

		case 11://�ϱ���ʶ
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

		case 12://��Ч��ʶ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			pBuf[0] = 1;
			pBuf += 1;
			break;
		
		#if( SEL_EVENT_LOST_V == YES )
		case 13://ʧѹͳ��(����ʧѹ�¼�)
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
		case 14://ʱ��״̬��¼���=array structure			
			#if( MAX_PHASE != 1 )
			if( ClassIndex > 4 )//ʱ��״̬��¼���ܳ���4
			{
				return 0x8000;
			}
			#else			
			if( ClassIndex > 2 )//�����ʱ��״̬��¼���ܳ���2
			{
				return 0x8000;
			}
			#endif
			
			//���ʷ����¼����޹����������¼���ǰ��¼����������
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
				#if( MAX_PHASE != 1 )	//��TAG���⴦��		
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
					
					if( i == 0 )//���ܽ��е�������
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
		case 15://�ϱ���ʽ
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
//��������:  �¼���class24�������������Tag
//         
//����:      
//         	*pOAD[in]��		OAD
//			Sch[in]��		EventRecordObj�е��¼�����         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: ���������6~9�������  
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
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	pBufBak = OutBuf;
	switch( ClassAttribute )
	{
		case 2://�����������Ա�
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
			if( TagPara.Array_Struct_Num == 0 )//�����������Ա������� ����01 00 
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
		
		case 3://��ǰ��¼��
			if( ClassIndex > 4 )
			{
				return 0x8000;
			}
			
			if( (OI.w==0x300b) || (OI.w==0x3007) || (OI.w==0x303B))//���������¼����޹����������¼���ǰ��¼����������
			{	
			    Tag = T_EventNowNum24;
			    
			    if( MeterTypesConst == METER_3P3W )
			    {
                    if( OI.w == 0x3007 )//���������¼�
                    {
                        Tag = T_EventNowNum24BNull;
                    }
			    }
				 if( MeterTypesConst == METER_3P3W )
			    {
                    if( OI.w == 0x303B )//���������¼�
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
		case 10://��ǰֵ��¼��
			if( ClassIndex > 4 )
			{
				return 0x8000;
			}
			TagPara.Array_Struct_Num = 4;
			
			if( (OI.w==0x300b) || (OI.w==0x3007) || (OI.w==0x303B))//���������¼����޹����������¼���ǰ��¼����������
			{
			    if( (MeterTypesConst==METER_3P3W) && (OI.w==0x3007) )//��������������
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
                            if( i == 2 )//B��
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
                    else if( ClassIndex == 2 )//B��
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
				else if( (MeterTypesConst==METER_3P3W) && (ClassIndex==3) )//������B��
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
		
		case 5://���ò���
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
		
		case 4://����¼��
		case 11://�ϱ���ʶ
		case 12://��Ч��ʶ
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

		case 13://ʧѹͳ��(����ʧѹ�¼�)
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
		case 14://ʱ��״̬��¼���=array structure			
			#if( MAX_PHASE != 1 )
			if( ClassIndex > 4 )//ʱ��״̬��¼���ܳ���4
			{
				return 0x8000;
			}
			#else			
			if( ClassIndex > 2 )//�����ʱ��״̬��¼���ܳ���2
			{
				return 0x8000;
			}
			#endif
			
			if( OutBufLen < (2+8+8) )//�����С�Ȱ���һ�෢��ʱ�䡢����ʱ����п���
			{
				return 0;
			}
			
			if( (OI.w==0x300b) || (OI.w==0x3007) || (OI.w==0x303B))//���������¼����޹����������¼���ǰ��¼����������
			{
				if( ClassIndex == 0 )
				{
					if( OutBufLen < (2+(2+8+8)*4) )//�����С��4�෢��ʱ�䡢����ʱ����п���
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
				    if( (MeterTypesConst==METER_3P3W) && (OI.w==0x3007) )//��������������
				    {
                        if( i == 2 )//3��3��B��
                        {
                            *(OutBuf++) = 0;
                            for( j=0; j<2; j++ )
                            {
                                if( InBuf[0] == 0x00 )//û�з���ʱ�䡢����ʱ��
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
						if( InBuf[0] == 0x00 )//û�з���ʱ�䡢����ʱ��
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
					if( OutBufLen < (3+(2+8+8)*4) )//�����С��3�෢��ʱ�䡢����ʱ����п���
					{
						return 0;
					}
								
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 4;
					*(OutBuf++) = 0x00;
                    InBuf++;//�ܼ�¼�����᷵��һ��00
				}
				
				if( ClassIndex == 1 )//�������ܵģ�����NULL
				{
					//if( OutBufLen < 1 )
					//{
					//	return 0;
					//}
					
					Lenth = 1;
					
					*(OutBuf++) = 0;
				}
				else//ClassIndex=0��2��3��4
				{
					Num = ( ClassIndex==0 ) ? 3 : 1; 
					for( i=0; i<Num; i++)
					{
                        if( MeterTypesConst == METER_3P3W )
                        {
                            if( i == 1 )//3��3��B��
                            {
                                *(OutBuf++) = 0;
                                for( j=0; j<2; j++ )
                                {
                                    if( InBuf[0] == 0x00 )//û�з���ʱ�䡢����ʱ��
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
							if( InBuf[0] == 0x00 )//û�з���ʱ�䡢����ʱ��
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
					if( OutBufLen < (3+8+8) )//�����С����ΪNULL��1�෢��ʱ�䡢����ʱ����п���
					{
						return 0;
					}
					
					Lenth = 17;
					
					*(OutBuf++) = Array_698;
					*(OutBuf++) = 4;
					*(OutBuf++) = 0x00;
					InBuf++;//�ܼ�¼�����᷵��һ��00
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
				else//ClassIndex = 0��2
				{	
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 2;
					for( j=0; j<2; j++ )
					{
						if( InBuf[0] == 0x00 )//û�з���ʱ�䡢����ʱ��
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
		case 15://��Ч��ʶ
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
//��������: ��ȡֻ��TAG�¼������ݳ���
//         
//����:     
//          DataType[in]��eData/eTagData
//          *pOAD[in]     OAD    
//          Number[in]��  ��Ա��
//         
//����ֵ:   �������ݳ���
//         
//��ע����:	�¼��ϱ�״̬
//			��̶����б�
//			�����ڼ��������ֵ
//			�¼������б� 
//--------------------------------------------------
WORD GetProEventUnitLen( BYTE DataType, BYTE *pOAD, BYTE Number )
{
	BYTE ClassAttribute,ClassIndex;
	TTwoByteUnion OI;	
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0x8000;
	
	switch( OI.w )
	{
		case 0x3300://�¼��ϱ�״̬
			if( (ClassIndex==0) && (ClassAttribute==2) )//ֻ֧�ֶ�����2���м�TAG
			{
				TagPara.Array_Struct_Num = Number;
				Lenth = GetTypeLen( DataType, T_EventReportStatus);
			}
			break;

		case 0x3302:
			if( ClassIndex == 6 )//��̶����б�
			{
				TagPara.Array_Struct_Num = Number;
				Lenth = GetTypeLen( DataType, T_EventRTable);
			}
			break;

		case 0x3308:
			if( ClassIndex == 6 )//�����ڼ��������ֵ
			{
				Lenth = GetTypeLen( DataType, T_UNDoubleLong );			
			}
			else if( ClassIndex == 7 )//�����ڼ��������ֵ����ʱ��
			{
				Lenth = GetTypeLen( DataType, T_DateTimS );
			}
			break;

		case 0x330c:
			if( ClassIndex == 6 )//�¼������б�
			{
				TagPara.Array_Struct_Num = Number;
				Lenth = GetTypeLen( DataType, T_EventClearTable);	
			}
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )
		case 0x3310:
			//0X06021033			//�쳣�忨�¼���¼��Ԫ0x3310--�����к�	octet-string��
			if( ClassIndex == 6 )
			{
				Lenth = GetTypeLen( DataType, T_OctetString8 );		
			}
			//0X07021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨������Ϣ��	unsigned��
			else if( ClassIndex == 7 )
			{
				Lenth = GetTypeLen( DataType, T_Unsigned );		
			}
			//698.45 ESAM��������ͷΪ6�ֽڣ�P2Ϊ���ֽڣ�������������ͷΪ5�ֽڣ� 7816������ͷ��5���ֽڵ�ϵ�У���5���ֽ�ָ��ΪCLA��INS��P1��P2��P3��??
			//0X08021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨��������ͷ	octet-string��
			else if( ClassIndex == 8 )
			{
				Lenth = GetTypeLen( DataType, T_OctetString7 );		
			}
			//0X09021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨������Ӧ״̬  long-unsigned��
			else if( ClassIndex == 9 )
			{
				Lenth = GetTypeLen( DataType, T_UNLong );		
			}
			break;
		case 0x3311:
			if( ClassIndex == 6 )//0X06021133--�˷��¼���¼��Ԫ0x3311--�˷ѽ��      double-long-unsigned����λ��Ԫ�����㣺-2��
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
//��������:  ��ȡ����
//         
//����:     
//         	DataType[in]��	eData/eTagData
//         	Dot[in]:		С����        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:  	�������ݳ���
//         
//��ע����: ֻ����class9������3 �����������Ա�
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
	else if( DataType == eAddTag )//�����಻֧�ֵ�����TAG
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
//��������:  ��ȡ�������¼���������
//         
//����: 
//			Ch[in]��		��Լͨ��    
//         	DataType[in]��	eData/eTagData
//         	Dot[in]:		С����        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:  	�������ݳ���
//         
//��ע����: ������ class7������2����class24������6~9��
//--------------------------------------------------
WORD GetProEventRecord( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE Sch;
	BYTE Buf[MAX_EVENT_PROBUF_LEN];//��ʹ���¼���������궨������Ϊ��������Ƕ�̬�ģ��¼�����С������»����������ʹ������¼�����������������
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
		else if( EventRecordObj[Sch].class == eclass8 )//�����¼��б�
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
			else if( EventRecordObj[Sch].class == eclass8 )//�����¼��б�
    	    {
                Lenth = AddTagNewEventList( pOAD, Sch, Buf, OutBufLen, OutBuf );
    	    }
    	    else
    	    {
                return 0x8000;
    	    }
		}
	}
	else if( DataType == eAddTag )//�����಻֧�ֵ�����TAG
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
//��������:   	���������ϱ��¼��б�
//         
//����: 
//			OutBufLen[out]��	buf����    
//         	OutBuf[out]��
//����ֵ:  	�������ݳ���
//         
//��ע����: ������ class7������2����class24������6~9��
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
	
	if( TagPara.Array_Struct_Num == 0 )//�����¼��б������� ����01 00 
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

