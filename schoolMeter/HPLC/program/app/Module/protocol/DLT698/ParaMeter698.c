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
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
// ������Ϣ��EEPROM�еĵ�ַ��Χ

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];
//-----------------------------------------------
//				���ļ�ʹ�õı���������

#define MAX_PARA_BUF_LENTH         (MAX_TIME_SEG*3*MAX_TIME_SEG_TABLE+30)//ʱ�α�Դ���ݳ���Ϊ������ݳ���

static const BYTE T_MinI[]			= { Visible_string_698,0XFF };//��С������ת�۵���	

static const BYTE T_MeterNO[]		= { Octet_string_698, 0x06 };		//�з����������ݱ�ʶ
static const BYTE T_LCDPara[]		= //LCD����		
{ 
	Structure_698, 	 	7, 
	Unsigned_698, 		1, 
	Long_unsigned_698, 	2, 
	Long_unsigned_698, 	2, 
	Long_unsigned_698, 	2, 
	Unsigned_698,		1, 
	Unsigned_698,		1, 
	Unsigned_698,		1 
};

static const BYTE T_TimezoneNum[]	= //ʱ��ʱ����
{ 
	Structure_698, 	 	5, 
	Unsigned_698, 		1,  
	Unsigned_698,		1, 
	Unsigned_698,		1, 
	Unsigned_698,		1,
	Unsigned_698,		1 
};

static const BYTE T_TimeHolidayTable[]	= //����������
{ 
	Array_698,		0XFF,
	Structure_698,	2,
	Date_698,		5,
	Unsigned_698,	1
};

static const BYTE T_Timezone[]			= //ʱ����
{ 
	Array_698,		0XFF,
	Structure_698,	03,
	Unsigned_698,	1,
	Unsigned_698,	1,
	Unsigned_698,	1
};

static const BYTE T_VPCT[]				= //��ѹ�ϸ���
{ 
	Structure_698,		4,
	Long_unsigned_698,	2,
	Long_unsigned_698,	2,
	Long_unsigned_698,	2,
	Long_unsigned_698,	2,
};

static const BYTE T_BillingPara[]		= //������
{ 
	Array_698,		0xff,
	Structure_698,	2,
	Unsigned_698,	1,
	Unsigned_698,	1,
};

static const BYTE T_ModeInfo[]		= //�汾��Ϣ
{ 
	Structure_698,		6,
	Visible_string_698,	4,//���̴���
	Visible_string_698,	4,//����汾��
	Visible_string_698,	6,//����汾����
	Visible_string_698,	4,//Ӳ���汾��
	Visible_string_698,	6,//Ӳ���汾����
	Visible_string_698,	8,//������չ��Ϣ
};

static const BYTE T_ResportChanel[]		= //�ϱ�ͨ��
{
	Array_698,			0xff,
	OAD_698,			4,
};

static const BYTE T_PositionInfo1[]		= //����λ����Ϣ1
{
	Structure_698, 	4,
	Enum_698,		1,
	Unsigned_698,	1,
	Unsigned_698,	1,
	Unsigned_698,	1,
};

static const BYTE T_APPInfo[]		= //Ӧ���ﾳ��Ϣ
{
	Structure_698, 	      7,
	Long_unsigned_698,	  2,
	Long_unsigned_698,	  2,
	Long_unsigned_698,	  2,
	Long_unsigned_698,	  2,
	Bit_string_698,       64,
	Bit_string_698,       128,
	Double_long_unsigned_698, 4
};

static const BYTE T_OI[]     = //֧���豸�б�
{
    Array_698,   1,
    OI_698,      2,
};

static const BYTE T_ProtoclList[]      = //֧�ֹ�Լ�б�
{
    Array_698,           1,
    Visible_string_698,  0xff,
};

static const BYTE T_Rate[]		= //���ʵ��-���ݵ�۵Ľ���ֵ����ݵ������
{
    Array_698,                  0xff,
    Double_long_unsigned_698,   4,
};

static const BYTE T_LimitMoney[]	= //���������ֵ
{
    Structure_698,             2,
    Double_long_unsigned_698,  4,
    Double_long_unsigned_698,  4,
};

static const BYTE T_OtherLimitMoney[]	= //���������ֵ
{
    Structure_698,             3,
    Double_long_unsigned_698,  4,
    Double_long_unsigned_698,  4,
    Double_long_unsigned_698,  4,
};

static const BYTE T_YearBill[]		= //���ݽ�����
{
    Structure_698,             3,
    Unsigned_698,              1,
    Unsigned_698,              1,
    Unsigned_698,              1,
};

static const BYTE T_TimeBroadCastPara[]		= //�㲥Уʱ����
{
    Structure_698,             2,
    Long_unsigned_698,         2,
    Long_unsigned_698,		   2,
};

static const BYTE Array_OI[] = 
{
    0x01, 0x0a, 0x02, 0x02, 0x50, 0x00, 0x00, 0x02, 0x02, 0x01, 
    0x01, 0x02, 0x02, 0x11, 0x80, 0x16, 0x00, 0x01, 0x01, 0x02, 
    0x02, 0x11, 0x05, 0x03, 0x00, 0x02, 0x02, 0x50, 0x00, 0x80, 
    0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 0x05, 0x16, 0x00, 
    0x01, 0x01, 0x02, 0x02, 0x11, 0x00, 0x03, 0x80, 0x02, 0x02, 
    0x50, 0x00, 0x05, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 
    0x00, 0x16, 0x80, 0x01, 0x01, 0x02, 0x02, 0x11, 0x00, 0x03, 
    0x05, 0x02, 0x02, 0x50, 0x80, 0x00, 0x02, 0x02, 0x01, 0x01, 
    0x02, 0x02, 0x11, 0x00, 0x16, 0x05, 0x01, 0x01, 0x02, 0x02, 
    0x11, 0x00, 0x03, 0x00, 0x02, 0x02, 0x50, 0x05, 0x00, 0x02, 
    0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 0x00, 0x16, 0x00, 0x01, 
    0x01, 0x02, 0x02, 0x11, 0x80, 0x03, 0x00, 0x02, 0x02, 0x50, 
    0x00, 0x00, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 0x80, 
    0x16, 0x00, 0x01, 0x01, 0x02, 0x02, 0x11, 0x05, 0x03, 0x00, 
    0x02, 0x02, 0x50, 0x00, 0x80, 0x02, 0x02, 0x01, 0x01, 0x02, 
    0x02, 0x11, 0x05, 0x16, 0x00, 0x01, 0x01, 0x02, 0x02, 0x11, 
    0x00, 0x03, 0x80, 0x02, 0x02, 0x50, 0x00, 0x00, 0x02, 0x02, 
    0x01, 0x01, 0x02, 0x02, 0x11, 0x6d, 0x16, 0x9a, 0x01, 0x01, 
    0x02, 0x02, 0x11, 0x16, 0x03, 0x03, 0x02, 0x02, 0x50, 0x11, 
    0x0b, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x11, 0x00, 0x16, 
    0x11, 0x01, 0x01, 0x02, 0x02, 0x11, 0x04, 0x03, 0x01, 0x02, 
    0x02, 0x50, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 
    0x11, 0x11, 0x16, 0x08, 0x01, 0x01, 0x02, 0x02, 0x11, 0x11, 
    0x03, 0x00,
};

//0x4800 ����2���壬�õ��豸���
static const BYTE A_4800_2_ApplClsID[] =
{
	Array_698,			0xff,
	Long_unsigned_698,	2,	//�豸���
};

//0x4800 ����3���壬�����豸��ǰ���ݼ�
static const BYTE A_4800_3_ApplCurData[] =
{
	Array_698,			0xff,
	Structure_698, 	 	3,
	Long_unsigned_698,	2,	//�豸���
	Enum_698,			1,	//�豸״̬
	Double_long_698,	4,	//�豸�й�����
};
//0x4800 ����4���壬�����豸�������ݵ�Ԫ
const BYTE A_4800_4_ApplFrzData[] =
{
	Array_698,					0xff,
	Structure_698, 	 			6,
	Long_unsigned_698,			2,	//�豸���
	Double_long_unsigned_698,	4,	//�����������й��õ���
	Double_long_698,			4,	//�����������豸����ƽ������
	Unsigned_698,				1,	//����ʱ��
	Unsigned_698,				1,	//ֹͣʱ��
	Unsigned_698,				1,	//�ϲ���ͣ��
};
//0x4800 ����4���壬�����豸���ᣬ�ǵ�����
const BYTE A_4800_4_ApplFrzData_NotElecHot[] =
{
	Array_698,					0xff,
	Structure_698, 	 			6,
	Long_unsigned_698,			2,	//�豸���
	Double_long_unsigned_698,	4,	//�����������й��õ���
	Double_long_698,			4,	//�����������豸����ƽ������
	NULL_698,					1,	//����ʱ��
	NULL_698,					1,	//ֹͣʱ��
	NULL_698,					1,	//�ϲ���ͣ��
};
//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
static const TCommonObj ParaMeterObj[] =
{
	{  0x4000,  &SU_00,   	T_DateTimS		},//����ʱ��---���ӹ㲥Уʱ���� ��ʽΪT_TimeBroadCastPara
	{  0x4001,  &SU_00,   	T_MeterNO		},//ͨ�ŵ�ַ
	{  0x4002,  &SU_00,   	T_MeterNO		},//���
	{  0x4003,  &SU_00,   	T_MeterNO		},//�ͻ����
	{  0x4004,  &SU_00,   	T_Special		},//�豸����λ�� ���⴦��
	{  0x4007,  &SU_00,   	T_LCDPara		},//LCD����
	{  0x4008,  &SU_00,   	T_DateTimS		},//����ʱ�����л�ʱ��
	{  0x4009,  &SU_00,   	T_DateTimS		},//����ʱ�α��л�ʱ��
	{  0x400C,  &SU_00,   	T_TimezoneNum	},//ʱ��ʱ����
    #if( PREPAY_MODE == PREPAY_LOCAL)
    {  0x400A,  &SU_00,   	T_DateTimS	    },//�����׷�ʱ�����л�ʱ��
    {  0x400B,  &SU_00,   	T_DateTimS	    },//�����׽��ݵ���л�ʱ��
	{  0x400d,  &SU_00,   	T_Unsigned	    },//������
	#endif	
	{  0x400F,  &SU_00,   	T_Unsigned		},//��Կ����
	{  0x4010,  &SU_00,   	T_Unsigned		},//����Ԫ����
	{  0x4011,  &SU_00,   	T_TimeHolidayTable},//����������
	{  0x4012,  &SU_00,   	T_BitString8	},//������������
	{  0x4013,  &SU_00,   	T_Unsigned		},//�����ղ��õ�ʱ�α��
	{  0x4014,  &SU_00,   	T_Timezone		},//��ǰ��ʱ����
	{  0x4015,  &SU_00,   	T_Timezone		},//������ʱ����
	{  0x4016,  &SU_00,   	T_Special		},//��ǰ��ʱ�α� ���⴦��
	{  0x4017,  &SU_00,   	T_Special		},//������ʱ�α� ���⴦��
	{  0x401C,  &SU_2,   	T_UNDoubleLong	},//�������������
	{  0x401D,  &SU_2,   	T_UNDoubleLong	},//��ѹ���������
	#if( PREPAY_MODE == PREPAY_LOCAL)
    {  0x4018,  &SU_00,     T_Rate          },//��ǰ�׷��ʵ��
    {  0x4019,  &SU_00,     T_Rate          },//�����׷��ʵ��
    {  0x401A,  &SU_00,     T_Special       },//��ǰ�׽��ݵ��
    {  0x401B,  &SU_00,     T_Special       },//�����׽��ݵ��,���ڲα�����(class_id=8)��û������3:���㼰��λ,�м䲻�� SU_2Yuan
    {  0x401e,  &SU_00,		T_LimitMoney    },//���������ֵ,���ڲα�����(class_id=8)��û������3:���㼰��λ
    {  0x401F,  &SU_00,  	T_OtherLimitMoney},//���������ֵ        
    {  0x4022,  &SU_00,  	T_BitString16	},//�忨״̬��
    #endif
	{  0x4102,  &SU_0MS,   	T_Unsigned		},//У�����峤��
	{  0x4103,  &SU_00,   	T_VisString32	},//�ʲ��������
	{  0x4104,  &SU_00,   	T_VisString6	},//���ѹ
	{  0x4105,  &SU_00,   	T_VisString6	},//�����/��������
	{  0x4106,  &SU_00,   	T_VisString6	},//������
	{  0x4107,  &SU_00,   	T_VisString4	},//�й�׼ȷ�ȵȼ�
	{  0x4109,  &SU_01Kwh, 	T_UNDoubleLong	},//���ܱ��й�����
	{  0x410B,  &SU_0MS,   	T_VisString32	},//���ܱ��ͺ�
	{  0x4111,  &SU_00,  	T_VisString16	},//���������
	{  0x4112,  &SU_00,   	T_BitString8	},//�й���Ϸ�ʽ��
	{  0x4116,  &SU_00,   	T_BillingPara	},//������
	{  0x4300,  &SU_00,   	T_Special		},//�����豸
    {  0x4400,  &SU_00,   	T_Special		},//Ӧ������
#if( MAX_PHASE != 1 )
	{  0x4030,  &SU_00,   	T_VPCT			},//��ѹ�ϸ���
	{  0x4100,  &SU_0MIN,   T_Unsigned		},//�����������
	{  0x4101,  &SU_0MIN,   T_Unsigned		},//����ʱ��
	{  0x4108,  &SU_00,   	T_VisString4	},//�޹�׼ȷ�ȵȼ�
	{  0x410A,  &SU_01Kwh,  T_UNDoubleLong	},//���ܱ��޹�����
	{  0x4113,  &SU_00,  	T_BitString8	},//�޹���Ϸ�ʽ��1
	{  0x4114,  &SU_00,   	T_BitString8	},//�޹���Ϸ�ʽ��2
	{  0x4117,  &SU_00,   	T_TI			},//�ڼ�������������
#endif
	{  0x4800,  &SU_00,   	A_4800_4_ApplFrzData_NotElecHot		},//ʶ����
};	

//Ĭ����������2019-1-1 12��00��00
 const BYTE ProduceDate[7]={227,7,1,1,12,0,0};
//-----------------------------------------------
static BYTE NumofNILM;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
// ��������:  �����Ӷ�������ʱ�����ڴ����������ȷ������Ҫ��ȡʱ���м���
//
// ����:      [in]��Num
//
// ����ֵ:    ��
//
// ��ע����:  ��
//--------------------------------------------------
void api_SetTagArrayNum(BYTE Num)
{
	NumofNILM = Num;
}
//--------------------------------------------------
//��������:  ���ݱ����Ҳα���OAD
//         
//����:      OI[in]��OI
//         
//����ֵ:    �α�������
//         
//��ע����:  ��
//--------------------------------------------------
static BYTE SearchParaMeterOAD( WORD OI )
{
	BYTE i,Num;//����������ܳ���255

	Num = (sizeof(ParaMeterObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//������ѭ��
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == ParaMeterObj[i].OI )
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
//��������:     ��ȡ�豸����class19���������ݣ�����Tag��//�����豸��
//         
//����:     
//          *pOAD[in]��	OAD             
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: ���������2������� ����Tag
//--------------------------------------------------
static WORD GetMeterClass19Data( BYTE *pOAD, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len,Result;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Len = 0x8000;
	
	switch( ClassAttribute )
	{
		case 2://�豸������
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			Len = 32;
			//����ͺ�
			Result = api_ProcMeterTypePara( READ, eMeterMeterModel, pBuf );
			if(Result == FALSE)
			{
				Len = 0x8000;
				break;
			}
			break;

		case 3://�汾��Ϣ
			if( ClassIndex > T_ModeInfo[1] )
			{
				return 0x8000;
			}

			Tag = T_ModeInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );
			//���̴���
			if( (ClassIndex==0) || (ClassIndex==1) )
			{
				Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.FactoryCode[0] ), 4, pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}
				pBuf += 4;
			}
			//����汾��
			if( (ClassIndex ==0) || (ClassIndex == 2))
			{
				Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.SoftWareVersion[0] ), 4, pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}

				pBuf += 4;
			}

			//����汾���� ASCII ��
			if( (ClassIndex==0) || (ClassIndex==3) )
			{
			    Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.SoftWareDate[0] ), 6, pBuf);
                if(Result == FALSE)
				{
					return 0x8000;
				}
				pBuf += 6;
			}

			//Ӳ���汾��
			if( (ClassIndex==0) || (ClassIndex==4) )
			{
				Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.HardWareVersion[0] ), 4, pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}

				pBuf += 4;
			}

			//Ӳ���汾���� ASCII��
			if( (ClassIndex==0) || (ClassIndex==5) )
			{
				Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.HardWareDate[0] ), 6, pBuf);
    			if(Result == FALSE)
				{
					return 0x8000;
				}
				pBuf += 6;
			}
			
			//������չ��Ϣ
			if( (ClassIndex==0) || (ClassIndex==6) )
			{
				memset( pBuf,0x00,8);
			}			
			break;

		case 4://��������
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( ParaConRom.MeterTypePara.ProduceDate[0] ), 7, pBuf);
            if( api_CheckClock((TRealTimer *)pBuf)!=TRUE )
            {
                memcpy(pBuf,ProduceDate,7);
            }            
			Len = 7;
			break;

        case 5://���豸�б�
			if( ClassIndex > T_OI[1] )
			{
				return 0x8000;
			}

			Tag = T_OI;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );
            pBuf[0] = 10;
            pBuf[1] = 18;

            break;

        case 6://֧�ֹ�Լ�б�
            if( ClassIndex > T_ProtoclList[1] )
			{
				return 0x8000;
			}

			Tag = T_ProtoclList;
			TagPara.Lenth.w = ProtocolVersionConst_698[0];
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );
			memcpy( pBuf, ProtocolVersionConst_698+1, ProtocolVersionConst_698[0] );
            break;     
		case 7://��������ϱ�
		case 8://���������ϱ�
		case 9://��������վͨ��
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( ClassAttribute == 7 )
			{
				pBuf[0] = ReadReportFlag(eFollowReport);
			}
			else if( ClassAttribute == 8 )
			{
				pBuf[0] = ReadReportFlag(eActiveReport);
			}	
			else
			{
				pBuf[0] = 1;
			}			
			Len = 1;	
			break;

		case 10://�ϱ�ͨ��
			Result = ReadReportChannel( pBuf );
			if( Result == FALSE )
			{
				return 0;
			}
			
			if(ClassIndex > (Result/4))
			{
				return 0x8000;
			}
			
			if( ClassIndex != 0 )
			{
				memcpy( pBuf, (BYTE*)&pBuf[4*(ClassIndex-1)],4 );
			}
			TagPara.Array_Struct_Num = (Result/4); 
			Tag = T_ResportChanel;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );			
			break;

		default:
			return 0x8000;
	}

	return Len;
}

//--------------------------------------------------
//��������:  �豸����class19�������������Tag
//         
//����:      
//         	*pOAD[in]��		OAD     
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[out]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: ���������2�������  
//--------------------------------------------------
static WORD AddTagMeterClass19( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,Buf[30];
	WORD Len,Result;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Len = 0x8000;
	
	switch( ClassAttribute )
	{
		case 2://�豸������
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Len = GetTypeLen( eTagData, T_VisString32 );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Tag = T_VisString32;
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		case 3://�汾��Ϣ
			if( ClassIndex > T_ModeInfo[1] )
			{
				return 0x8000;
			}
			
			Tag = T_ModeInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		case 4://��������
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Tag = T_DateTimS;
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;
			
		case 5://���豸�б�
			if( ClassIndex > T_OI[1] )
			{
				return 0x8000;
			}

			Tag = T_OI;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;	
			
        case 6://֧�ֹ�Լ�б�
            if( ClassIndex > T_ProtoclList[1] )
            {
                return 0x8000;
            }
            
            Tag = T_ProtoclList;
            TagPara.Lenth.w = ProtocolVersionConst_698[0];
            Tag += GetTagOffsetAddr( ClassIndex, Tag );
            Len = GetTypeLen( eTagData, Tag );
            if( Len == 0x8000 )
            {
                return 0x8000;
            }
            
            if( OutBufLen < Len )
            {
                return 0;
            }
            Result = GetRequestAddTag( Tag, InBuf, OutBuf );
            if( Result == 0x8000 )
            {
                return 0x8000;
            }

            break;  

			break;	

		case 7://��������ϱ�
		case 8://���������ϱ�
		case 9://��������վͨ��
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Bool;
			Len = GetTypeLen( eTagData, Tag );
		    if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}			
			break;

		case 10://�ϱ�ͨ��
			Result = ReadReportChannel( Buf );
			if( Result == FALSE )
			{
				Result = 0;
			}
			
			if(ClassIndex > (Result/4))
			{
				return 0x8000;
			}
			
			TagPara.Array_Struct_Num = (Result/4); 

			Tag = T_ResportChanel;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
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

	return Len;
}

//--------------------------------------------------
//��������:  ��ȡ�����豸�ࣨclass19�����ݳ���
//         
//����:      
//         	DataType[in]��		DataType[in]��eData/eTagData     
//          *pOAD[in]��			OAD          
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����:  
//--------------------------------------------------
static WORD GetMeterClass19Len( BYTE DataType, BYTE *pOAD )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len;
	const BYTE *Tag;

	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];

	switch( ClassAttribute )
	{
		case 2://�豸������
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Len = GetTypeLen( DataType, T_VisString32 );
			break;

		case 3://�汾��Ϣ
			if( ClassIndex > T_ModeInfo[1] )
			{
				return 0x8000;
			}
			
			Tag = T_ModeInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
			break;

		case 4://��������
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Tag = T_DateTimS;
			Len = GetTypeLen( DataType, Tag );
			break;

        case 5://���豸�б�
            if( ClassIndex > T_OI[1] )
			{
				return 0x8000;
			}
			
			Tag = T_OI;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
            break;
            
        case 6://֧�ֹ�Լ�б�
            if( ClassIndex > T_ProtoclList[1] )
			{
				return 0x8000;
			}
			
			Tag = T_ProtoclList;
			TagPara.Lenth.w = ProtocolVersionConst_698[0];
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
            break;
		case 7://��������ϱ�
		case 8://���������ϱ�
		case 9://��������վͨ��
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Bool;
			Len = GetTypeLen( DataType, Tag );
			break;
                
		case 10://�ϱ�ͨ��
			if(ClassIndex > MAX_COM_CHANNEL_NUM)
			{
				return 0x8000;
			}
			
			TagPara.Array_Struct_Num = 4; //���������з���

			Tag = T_ResportChanel;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			break;

		default:
			return 0x8000;
	}
	return Len;
}


//--------------------------------------------------
//��������:     ��ȡ�豸����class20���������ݣ�����Tag��//Ӧ��������
//         
//����:     
//          *pOAD[in]��	OAD             
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: ���������2������� ����Tag
//--------------------------------------------------
static WORD GetMeterClass20Data( BYTE *pOAD, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len;
	const BYTE *Tag;
	TTwoByteUnion TmpLen;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Len = 0x8000;
	
	switch( ClassAttribute )
	{
	    case 2://�����б�
            memcpy( pBuf, Array_OI, sizeof( Array_OI ) );
            return sizeof( Array_OI );
	        break;
		case 3://Ӧ���ﾳ��Ϣ
			if( ClassIndex > T_APPInfo[1] )
			{
				return 0x8000;
			}

			Tag = T_APPInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eData, Tag );
			//Э��汾��Ϣ
			if( (ClassIndex==0) || (ClassIndex==1) )
			{
                api_ProcMeterTypePara( READ, eMeterProtocolVersion, pBuf );//698.45Э��汾��(��������:WORD)
				pBuf += 2;
			}
			//������apdu�ߴ�
			if( (ClassIndex ==0) || (ClassIndex == 2))
			{
                TmpLen.w= MAX_PRO_BUF;//����������֡���ߴ�    long-unsigned
                //pBuf[0] = TmpLen.b[1];//Ӧ�ò��ȴ����ֽ�
                //pBuf[1] = TmpLen.b[0];
                memcpy( pBuf, TmpLen.b, 2);
                pBuf += 2;
			}

			//�����apdu�ߴ�
			if( (ClassIndex==0) || (ClassIndex==3) )
			{
                TmpLen.w= MAX_PRO_BUF;//����������֡���ߴ�    long-unsigned
                //pBuf[0] = TmpLen.b[1];//Ӧ�ò��ȴ����ֽ�
                //pBuf[1] = TmpLen.b[0];
                memcpy( pBuf, TmpLen.b, 2);
                pBuf += 2;
			}

			//���ɴ���apdu�ߴ�
			if( (ClassIndex==0) || (ClassIndex==4) )
			{
                TmpLen.w = MAX_APDU_SIZE;//���������ɴ���apdu���ߴ�    long-unsigned
                //pBuf[0] = TmpLen.b[1];//Ӧ�ò��ȴ����ֽ�
                //pBuf[1] = TmpLen.b[0];
                memcpy( pBuf, TmpLen.b, 2);
                pBuf += 2;
			}

			//Э��һ���Կ�
			if( (ClassIndex==0) || (ClassIndex==5) )
			{
				pBuf += AddProtocolAndFunctionConformance( 1, pBuf );
			}
			
			//����һ���Կ�
			if( (ClassIndex==0) || (ClassIndex==6) )
			{

			    pBuf += AddProtocolAndFunctionConformance( 2, pBuf );
			}			

	        if( (ClassIndex==0) || (ClassIndex==7) )
			{

			    TmpLen.w = 1800;//��ʱʱ��
                pBuf[0] = TmpLen.b[0];
                pBuf[1] = TmpLen.b[1];
                pBuf[2] = 0x00;
			    pBuf[3] = 0x00;
                pBuf += 4;
			}		

			break;

		case 4://��ǰ���ӵĿͻ�����ַ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

            pBuf[0] = ClientAddress;

			Len = 1;
			break;

		case 5://������֤����
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

		    pBuf[0] = ConnectMode;

			Len = 1;
    		break;

		default:
			return 0x8000;
	}

	return Len;
}

//--------------------------------------------------
//��������:     �豸����class19�������������Tag Ӧ������
//         
//����:      
//         	*pOAD[in]��		OAD     
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[out]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: ���������2�������  
//--------------------------------------------------
static WORD AddTagMeterClass20( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len,Result;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Len = 0x8000;
	
	switch( ClassAttribute )
	{
	    case 2:
	        memcpy( OutBuf, InBuf, sizeof(Array_OI));
	        return sizeof( Array_OI );
	        break;
		case 3://Ӧ���ﾳ��Ϣ
			if( ClassIndex > T_APPInfo[1] )
			{
				return 0x8000;
			}
			
			Tag = T_APPInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( eTagData, Tag );
			if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			break;

		case 4://��ǰ���ӵĿͻ�����ַ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Unsigned;
			Len = GetTypeLen( eTagData, Tag );
		    if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}	

        break;
        case 5://������֤����
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Enum;
			Len = GetTypeLen( eTagData, Tag );
		    if( Len == 0x8000 )
			{
                return 0x8000;
			}
			
			if( OutBufLen < Len )
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

	return Len;
}

//--------------------------------------------------
//��������:     ��ȡ�����豸�ࣨclass19�����ݳ���            Ӧ������
//         
//����:      
//         	DataType[in]��		DataType[in]��eData/eTagData     
//          *pOAD[in]��			OAD          
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����:  
//--------------------------------------------------
static WORD GetMeterClass20Len( BYTE DataType, BYTE *pOAD )
{
	BYTE ClassAttribute,ClassIndex;
	WORD Len;
	const BYTE *Tag;
	
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];

	switch( ClassAttribute )
	{
	    case 2:

            return sizeof( Array_OI );
            break;
		case 3://Ӧ���ﾳ��Ϣ
			if( ClassIndex > T_APPInfo[1] )
			{
				return 0x8000;
			}
			
			Tag = T_APPInfo;
			Tag += GetTagOffsetAddr( ClassIndex, Tag );
			Len = GetTypeLen( DataType, Tag );
			break;

		case 4://��ǰ���ӵĿͻ�����ַ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			Tag = T_Unsigned;
			Len = GetTypeLen( DataType, Tag );
			break;

		case 5://������֤����
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Enum;
			Len = GetTypeLen( DataType, Tag );
			break;
  
		default:
			return 0x8000;
	}
	return Len;
}

//--------------------------------------------------
//��������:  ����λ����Ϣ��class8���������Tag
//         
//����:      
//         	*pOAD[in]��		OAD    
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[out]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: ���ȡ�ά�ȡ��߶�  
//--------------------------------------------------
static WORD AddTagPositionInfo( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassIndex,Len;
	WORD Result;
	
	Len = 0;
	
	ClassIndex = pOAD[3];
	if( ClassIndex > 3 )
	{
		return 0x8000;
	}
	
	if(ClassIndex == 0)
	{
		if( OutBufLen < 27 )//��������С�������򲻽��в���
		{
			return 0;
		}
		Len = 27;
		*(OutBuf++) = Structure_698;
		*(OutBuf++) = 3;
		Result = GetRequestAddTag( T_PositionInfo1, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		OutBuf +=10;
		Result = GetRequestAddTag( T_PositionInfo1, InBuf+4, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		OutBuf +=10;
		Result = GetRequestAddTag( T_UNDoubleLong, InBuf+8, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
	}
	else if( (ClassIndex==1) || (ClassIndex==2) )
	{
		if( OutBufLen < 10 )//��������С�������򲻽��в���
		{
			return 0;
		}
		Len = 10;
		Result = GetRequestAddTag( T_PositionInfo1, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
	}
	else
	{
		if( OutBufLen < 5 )//��������С�������򲻽��в���
		{
			return 0;
		}
		
		Len = 5;
		
		Result = GetRequestAddTag( T_UNDoubleLong, InBuf+8, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}

	}

	return Len;
}

//--------------------------------------------------
//��������:  ʱ�α�class8���������Tag
//         
//����:      
//         	*pOAD[in]��		OAD    
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[out]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: ʱ�α�����8����ʱ�α�14��ʱ�μ��㳤�ȣ�  
//--------------------------------------------------
static WORD AddTagTimeTable( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassIndex,i;
	WORD Result,Len;

	Len = 0x8000;

	ClassIndex = pOAD[3];
	if( ClassIndex > MAX_TIME_SEG_TABLE )
	{
		return 0x8000;
	}

	if( ClassIndex == 0 )
	{
		if( OutBufLen <  (2+8*FPara1->TimeZoneSegPara.TimeSegNum)*FPara1->TimeZoneSegPara.TimeSegTableNum+2)//��������С�������򲻽��в���
		{
			return 0;
		}
		Len = ((2+8*FPara1->TimeZoneSegPara.TimeSegNum)*FPara1->TimeZoneSegPara.TimeSegTableNum+2);
		*(OutBuf++) = Array_698;
		*(OutBuf++) = FPara1->TimeZoneSegPara.TimeSegTableNum;
		
		TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.TimeSegNum;//��TAG����ʵ�ʸ����������
		
		for( i=0; i < FPara1->TimeZoneSegPara.TimeSegTableNum; i++ )
		{
			Result = GetRequestAddTag( T_Timezone, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}

			InBuf += (MAX_TIME_SEG*3);//����buf������󳤶Ƚ�����ת ��ע��-���ĺ�
			OutBuf += Result;
		}
	}
	else
	{
		if( OutBufLen < (2+8*FPara1->TimeZoneSegPara.TimeSegNum))//��������С�������򲻽��в���
		{
			return 0;
		}
		//Len = (2+8*FPara1->TimeZoneSegPara.TimeSegNum);
		
		TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.TimeSegNum;

		Result = GetRequestAddTag( T_Timezone, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}

		Len = Result;
	}

	return Len;
}

#if( PREPAY_MODE == PREPAY_LOCAL )
//--------------------------------------------------
//��������: 	���ݱ�class8���������Tag
//         
//����:      
//         	*pOAD[in]��		OAD    
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[out]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: ���ݱ�  
//--------------------------------------------------
static WORD AddTagLadder( BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassIndex,i;
	WORD Result,Len;

	Len = 0x8000;

	ClassIndex = pOAD[3];
	if( ClassIndex > 3 )
	{
		return 0x8000;
	}

	if( ClassIndex == 0 )
	{
	    if( OutBufLen < (2+6+(2*MAX_LADDER+1)*5+MAX_YEAR_BILL*8) )
	    {
            return 0x8000;
	    }

	    Len = (2+6+(2*MAX_LADDER+1)*5+MAX_YEAR_BILL*8);
        *(OutBuf++) = Structure_698;
        *(OutBuf++) = 3; 
	}

    if( (ClassIndex == 0) || (ClassIndex == 1) )//����ֵ
    {
        if( ClassIndex == 1 )
        {
            if( OutBufLen < (2+MAX_LADDER*5) )
            {
                return 0x8000;
            }

            Len = (2+MAX_LADDER*5);
        }

        TagPara.Array_Struct_Num = MAX_LADDER;

		Result = GetRequestAddTag( T_Rate, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		
        InBuf += GetTypeLen( eData, T_Rate );
		OutBuf += Result;
    }
    
    if( (ClassIndex == 0) || (ClassIndex == 2 ) )//���ݵ��
    {
        if( ClassIndex == 2 )
        {
            if( OutBufLen < (2+(MAX_LADDER+1)*5) )
            {
                return 0x8000;
            }

            Len = (2+(MAX_LADDER+1)*5);
        }

        TagPara.Array_Struct_Num = (MAX_LADDER+1);

		Result = GetRequestAddTag( T_Rate, InBuf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		
        InBuf += GetTypeLen( eData, T_Rate );
		OutBuf += Result;
    }
    
    if( (ClassIndex == 0) || (ClassIndex == 3) )//���ݽ�����
    {
        if( ClassIndex == 3 )
        {
            if( OutBufLen < (2+(MAX_YEAR_BILL)*8) )
            {
                return 0x8000;
            }

            Len = (2+(MAX_YEAR_BILL)*8);
        }

        *(OutBuf++) = Array_698;
        *(OutBuf++) = MAX_YEAR_BILL;
        
        TagPara.Array_Struct_Num = MAX_YEAR_BILL;

        for( i=0; i<MAX_YEAR_BILL; i++ )
        {
    		Result = GetRequestAddTag( T_YearBill, InBuf, OutBuf );
    		if( Result == 0x8000 )
    		{
    			return 0x8000;
    		}

            InBuf += GetTypeLen( eData, T_YearBill );
    		OutBuf += Result;
        }
		
    }

    return Len;
}
#endif
//--------------------------------------------------
//��������:  ��ȡ��������Ϣ��ö�ٺţ�TMeterTypeEnum��
//         
//����:      OI[in]��OI
//         
//����ֵ:    ��������Ϣ��ö�ٺ�
//         
//��ע����:  ��
//--------------------------------------------------
static BYTE GetProMeterType( WORD OI )
{
	switch( OI )
	{
		case 0x4001://ͨѶ��ַ
			return eMeterCommAddr;
			break;
		case 0x4002://���
			return eMeterMeterNo;
			break;
		case 0x4003://�ͻ����
			return eMeterCustomCode;
			break;
		case 0x4004://����λ����Ϣ
			return eMeterMeterPosition;
			break;
		case 0x4103://�ʲ��������
			return eMeterPropertyCode;
			break;
		case 0x4104://���ѹ
			return eMeterRateVoltage;
			break;
		case 0x4105://�����/��������
			return eMeterRateCurrent;
			break;
		case 0x4106://������
			return eMeterMaxCurrent;
			break;
		case 0x4107://�й�׼ȷ�ȼ�
			return eMeterPPrecision;
			break;
		case 0x4108://�޹�׼ȷ�ȼ�
			return eMeterQPrecision;
			break;
		case 0x4109://���ܱ��й�����
			return eMeterActiveConstant;
			break;
		case 0x410A://���ܱ��޹�����
			return eMeterReactiveConstant;
			break;
		case 0x410B://���ܱ��ͺ�
			return eMeterMeterModel;
			break;
		case 0x4111://���������
			return eMeterSoftRecord;
			break;
		case 0x4300://���������
			return eMeterProduceDate;
			break;
		default:
			return 0x80;
	}
}

//--------------------------------------------------
//��������: ��ȡlcd����
//         
//����:      
//         	ClassIndex[in]��Ԫ������
//          Sch[in] ��		ParaMeterObj�е�����        
//          *pBuf[out]��		�������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: OI=4007 LCD���� ����tag
//--------------------------------------------------
static WORD GetProLcdPara( BYTE ClassIndex, BYTE Sch, BYTE *pBuf )
{
	WORD Len,Len2;
	
	Len2= GetTypeLen(eData, ParaMeterObj[Sch].Type2);
    if( Len2 == 0x8000 )
	{
        return 0x8000;
	}
	
	Len = 0;
	memcpy( pBuf, (BYTE*)&(FPara1->LCDPara.PowerOnDispTimer), Len2 );
	
    Len = Len2;
    
	if( ClassIndex != 0 )
	{
		switch( ClassIndex )
		{
			case 0x01://�ϵ�ȫ��ʱ��  unsigned��
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.PowerOnDispTimer),sizeof(BYTE) );
				Len = sizeof(BYTE);
				break;		
			case 0x02://�������ʱ��  long-unsigned(����ʱ�������ʱ��)��
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.BackLightPressKeyTimer),sizeof(WORD) );
				Len = sizeof(WORD);
				break;
			case 0x03://��ʾ�鿴�������ʱ��  long-unsigned��
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.BackLightViewTimer),sizeof(WORD) );
				Len = sizeof(WORD);
				break;
			case 0x04://�޵簴����Ļפ�����ʱ��  long-unsigned�� HEX ��λΪ��
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.LcdSwitchTime),sizeof(WORD) );
				Len = sizeof(WORD);
				break;
			case 0x05://����С����
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.EnergyFloat),sizeof(BYTE) );
				Len = sizeof(BYTE);
				break;
			case 0x06://���ʣ�������С����
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.DemandFloat),sizeof(BYTE) );
				Len = sizeof(BYTE);
				break;
			case 0x07://��ʾ12��������  0:������ʾ��ǰ�ס�������ʱ�� 1��������ʾ��ǰ�ס������׷��ʵ��
				memcpy( pBuf,(BYTE*)&(FPara1->LCDPara.Meaning12),sizeof(BYTE) );
				Len = sizeof(BYTE);
				break;
			default:
			    Len = 0x8000;
				break;
		}
	}

	return Len;
}
WORD GetApplianceFreezData(BYTE Index, BYTE Count, BYTE *Buffer )
{	
	BYTE Num;//��ȡ�ĸ���
	BYTE i;//ȡ������ʼ��
	
	if(Index == 0)
	{
		Num = Count;
		i = 0;
	}
	else
	{
		Num = 1;
		i = Index - 1;
	}
	
	memcpy(Buffer, &MessageData.ucData[i], Num * sizeof(T_ApplFrzData));
	
	return Num * sizeof(T_ApplFrzData);
}
WORD GetFrzorCurLen(BYTE flag )
{	
	BYTE i,res=0;
	T_ApplCurData tempBuf = {0xFFFF,0xFF,0xFFFFFFFF};
	T_ApplFrzData tempBuf1 = {0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF};
	
	for(i=0;i<NILM_EQUIPMENT_MAX_NO;i++)
	{
		if(flag == 2)
		{
			res = NILM_EQUIPMENT_TYPE_NO;
			return res;
		}
		// else if(flag == 3)
		// {
		// 	if(memcmp(&tempBuf.ApplClassID, &MessageData[i].ApplClassID, sizeof(T_ApplCurData)) == 0)
		// 	{
		// 		continue;
		// 	}
		// }
		else if (flag == 4)
		{
			if(memcmp(&tempBuf1.ApplClassID, &MessageData.ucData[i].ApplClassID, sizeof(T_ApplFrzData)) == 0)
			{
				continue;
			}
		}
		res++;
	}
	
	return res;
}
//--------------------------------------------------
//��������:  ��ȡ�α�����class8������
//         
//����:     
//          *pOAD[in]��	OAD         
//          Sch[in]��   ParaMeterObj �е���������      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: ����tag
//--------------------------------------------------
static WORD GetProMeterParaData( BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex,Num,i,MaxIndex;
	WORD Type,Result,Len;
	BYTE *pBufBak = pBuf;
	const BYTE *ObjType;
	TTimeBroadCastPara tmpTimeBroadCastPara;
	#if( PREPAY_MODE == PREPAY_LOCAL)
	TRatePrice				RatePrice;		//��ǰ���ʵ�۱�
    TLadderPrice			LadderPrice;	//��ǰ���ݵ�۱�
    #endif
    
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	ObjType = ParaMeterObj[Sch].Type2;
	Len = 0;
	
	if( OI.w == 0x4300 )//�����豸�ӿ��� ��������
	{
		return GetMeterClass19Data( pOAD, pBuf );
	}
	else if( OI.w == 0x4400 )//Ӧ�����ӽӿ���  Ӧ������
	{
        return GetMeterClass20Data( pOAD, pBuf );
	}
	
	switch( ClassAttribute )
	{
		case 2:
			switch( OI.w )
			{
				case 0x4000://����ʱ��
					if( ClassIndex != 0)
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
        			if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					Result = api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, pBuf );
					if(Result == FALSE)
					{
						return 0x8000;
					}

					pBuf += Len;					
					break;

				case 0x4001://ͨѶ��ַ
				case 0x4002://���
				case 0x4003://�ͻ����
				case 0x4103://�ʲ��������
				case 0x4104://���ѹ
				case 0x4105://�����/��������
				case 0x4106://������
				case 0x4107://�й�׼ȷ�ȼ�
				case 0x4108://�޹�׼ȷ�ȼ�
				case 0x4109://���ܱ��й�����
				case 0x410A://���ܱ��޹�����
				case 0x410B://���ܱ��ͺ�
				case 0x4111://���������
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					
					Type = GetProMeterType( OI.w );
					if( Type == 0x80 )
					{
						return 0x8000;
					}
					
					Len = GetTypeLen( eData, ObjType );
        			if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					Result = api_ProcMeterTypePara( READ, Type, pBuf );
					if(Result == FALSE)
					{
						return 0x8000;
					}
					pBuf += Len;
					break;
				case 0x4004://����λ����Ϣ
					if( ClassIndex > 3 )
					{
						return 0x8000;
					}
					
					Type = GetProMeterType( OI.w );
					if( Type == 0x80 )
					{
						return 0x8000;
					}
					
					Len = (ClassIndex == 0) ? 12 : 4;
					Result = api_ProcMeterTypePara( READ, Type, pBuf );
					if(Result == FALSE)
					{
						Len = 0x8000;
						break;
					}
					if( ClassIndex !=0 )
					{
						memcpy(pBuf,pBuf+(ClassIndex - 1)*4 ,4);
					}
					pBuf += Len;
					break;
				case 0x4007://LCD����
					if( ClassIndex > ObjType[1] )
					{
						return 0x8000;
					}
					Len = GetProLcdPara(ClassIndex,  Sch, pBuf );
					if( Len == 0x8000 )
					{
						return 0x8000;
					}
					pBuf += Len;
					break;

				case 0x4008://������ʱ���л�ʱ��
				case 0x4009://��������ʱ�α��л�ʱ��
				#if(PREPAY_MODE == PREPAY_LOCAL)
				case 0x400a://�����׷�ʱ�����л�ʱ��
				case 0x400b://�����׽��ݵ���л�ʱ��
				#endif
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType);
        			if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					//Result = GetProSwitchTime(OI.w, pBuf);
					if( api_ReadAndWriteSwitchTime( READ, OI.w - 0x4008, pBuf ) == FALSE )
					//if(Result == FALSE)
					{
						return 0x8000;
					}
					
                    pBuf[6] = 0XFF;

					pBuf += Len;
					break;

				case 0x400C://ʱ��ʱ����
					if( ClassIndex > ObjType[1] )
					{
						return 0x8000;
					}
					ObjType += GetTagOffsetAddr( ClassIndex, ObjType);
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					memcpy( pBuf, ((BYTE*)&FPara1->TimeZoneSegPara.TimeZoneNum), sizeof(TTimeZoneSegPara));

					if( ClassIndex != 0 )
					{
						memcpy( pBuf, pBuf+(ClassIndex-1), Len);
					}

					pBuf += Len;
					break;

				case 0x400F://��Կ������
				case 0x401c://CT
				case 0x401d://PT
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					
					if( OI.w == 0x400F )
					{
						Type = eKeyNum;
					}
					else if( OI.w == 0x401c )
					{
						Type = eCTCoe;
					}
					else
					{
						Type = ePTCoe;
					}
					
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					api_ReadPrePayPara((ePrePayParaType)Type,pBuf);
					pBuf += Len;
					break;

				case 0x4010://����Ԫ����
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					
					#if(MAX_PHASE == 1)
					*(pBuf++) = 1;
					#else
					if(MeterTypesConst == METER_3P3W)
					{
						*(pBuf++) = 2;
					}
					else
					{
						*(pBuf++) = 3;
					}
					#endif
					break;
				case 0x4011://�������ձ�
				case 0x4014://��ǰ��ʱ����
				case 0x4015://������ʱ����					
					if( OI.w == 0x4011 )
					{
						if( ClassIndex > MAX_HOLIDAY )
						{
							return 0x8000;
						}
						Type = 0x02;
						TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.HolidayNum;
					}
					else if( OI.w == 0x4014 )
					{
						if( ClassIndex > MAX_TIME_AREA )
						{
							return 0x8000;
						}
						Type = 0x01;
						TagPara.Array_Struct_Num = MAX_TIME_AREA;//FPara1->TimeZoneSegPara.TimeZoneNum;
					}
					else
					{
						if( ClassIndex > MAX_TIME_AREA )
						{
							return 0x8000;
						}
						Type = 0x81;
						TagPara.Array_Struct_Num = MAX_TIME_AREA;//FPara1->TimeZoneSegPara.TimeZoneNum;
					}					
					
					ObjType += GetTagOffsetAddr( ClassIndex, ObjType);
					Len = GetTypeLen( eData, ObjType );
        			if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					if( ClassIndex == 0 )
					{
						ClassIndex = 1;
					}
					
					Result = api_ReadTimeTable(Type, Len*(ClassIndex-1), Len , pBuf);
					pBuf += Len;
					break;
				
				case 0x4016://��ǰ����ʱ�α�
				case 0x4017://������ʱ�α�
					if( ClassIndex > MAX_TIME_SEG_TABLE )
					{
						return 0x8000;
					}
					Type = ( OI.w == 0x4016) ? 0x03 : 0x83;
					i = (( ClassIndex == 0) ? 0 : (ClassIndex-1));
					//Num = (( ClassIndex == 0) ? FPara1->TimeZoneSegPara.TimeSegTableNum : 1);
					Num = (( ClassIndex == 0) ? MAX_TIME_SEG_TABLE : 1);
					Num += i;
					
					if( ClassIndex == 0 )
					{
						ClassIndex = 1;
					}
					
					for( ; i < Num; i++ )
					{
						//Result = api_ReadTimeTable(Type+i, 0, 3*FPara1->TimeZoneSegPara.TimeSegNum, pBuf);
						//pBuf += (3*FPara1->TimeZoneSegPara.TimeSegNum);
						Result = api_ReadTimeTable(Type+i, 0, 3*MAX_TIME_SEG, pBuf);
						pBuf += (3*MAX_TIME_SEG);
					}

					break;
				case 0x4012://������������
				case 0x4013://�����ղ��õ���ʱ�α��
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}

					if( OI.w == 0x4012 )
					{
						*(pBuf++) = FPara1->TimeZoneSegPara.WeekStatus;
					}
					else
					{
						*(pBuf++) = FPara1->TimeZoneSegPara.WeekSeg;
					}
	
					break;
			    #if( PREPAY_MODE == PREPAY_LOCAL)
                case 0x400d://������
                    if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }

                    api_ReadPrePayPara( eLadderNum, pBuf );
                    pBuf++;
                    
                    break;

                case 0x4018://��ǰ�׷��ʵ��
                case 0x4019://�����׷��ʵ��
                    if( ClassIndex > FPara1->TimeZoneSegPara.Ratio )
                    {
                        return 0x8000;
                    }
					if( OI.w == 0x4018 )
					{
						api_ReadPrePayPara(eCurRateTable, (BYTE*)RatePrice.Price);
					}
					else
					{
						api_ReadPrePayPara(eBackupRateTable, (BYTE*)RatePrice.Price);
					}

                    TagPara.Array_Struct_Num = (FPara1->TimeZoneSegPara.Ratio);
                    
                    if( ClassIndex == 0 )
                    {
                        memcpy( pBuf, (BYTE*)RatePrice.Price, 4*(FPara1->TimeZoneSegPara.Ratio) );
                        pBuf += (4*(FPara1->TimeZoneSegPara.Ratio));
                    }
                    else
                    {
                        memcpy( pBuf, (BYTE*)&RatePrice.Price[ClassIndex-1], 4 );
                        pBuf += 4;
                    }
                    
                    break;

                case 0x401A://��ǰ�׽��ݵ��
                case 0x401B://�����׽��ݵ��
                    if( ClassIndex > 3 )
                    {
                        return 0x8000;
                    }
					if( OI.w == 0x401A )
					{
						api_ReadPrePayPara(eCurLadderTable, (BYTE*)LadderPrice.Ladder_Dl );
					}
					else
					{
						api_ReadPrePayPara(eBackupLadderTable, (BYTE*)LadderPrice.Ladder_Dl );
					}

                    if( ClassIndex == 0 )
                    {
                        memcpy( pBuf, (BYTE*)LadderPrice.Ladder_Dl, (sizeof(TLadderPrice)-sizeof(DWORD)) );    
                        pBuf += (sizeof(TLadderPrice)-sizeof(DWORD));
                    }
                    else
                    {
                        if( ClassIndex == 1 )//����ֵ
                        {
                            memcpy( pBuf, (BYTE*)LadderPrice.Ladder_Dl, sizeof(LadderPrice.Ladder_Dl) );
                            pBuf += sizeof(LadderPrice.Ladder_Dl);
                        }
                        else if( ClassIndex == 2 )//���ݵ��
                        {
                            memcpy( pBuf, (BYTE*)LadderPrice.Price, sizeof(LadderPrice.Price) );
                            pBuf += sizeof(LadderPrice.Price);
                        }
                        else
                        {
                            memcpy( pBuf, (BYTE*)LadderPrice.YearBill, sizeof(LadderPrice.YearBill) );
                            pBuf += sizeof(LadderPrice.YearBill);
                        }
                    }
                    break;
                case 0x401e://���������ֵ
                case 0x401f://���������ֵ
                    if( OI.w == 0x401e )
                    {
                        if( ClassIndex > 2 )
                        {
                            return 0x8000;
                        }

                        Num = ((ClassIndex == 0) ? 2 : 1 );
                        Type = ((ClassIndex == 0) ? eAlarm_Limit1 : (eAlarm_Limit1+(ClassIndex-1)) );
                    }
                    else
                    {
                        if( ClassIndex > 3 )
                        {
                            return 0x8000;
                        }
                        
                        Num = ((ClassIndex == 0) ? 3 : 1 );
                        Type = ((ClassIndex == 0) ? eTickLimit : (eTickLimit+(ClassIndex-1)) );
                    }

                    for( i=0; i<Num; i++ )
                    {
                        api_ReadPrePayPara( (ePrePayParaType)(Type+i), pBuf );
                        pBuf += 4;
                    }
                    
                    break;
        		case 0x4022://T_BitString16	},//�忨״̬��
        			if( ClassIndex != 0 )//��������С�������򲻽��в���
					{
						return 0x8000;
					}
                    Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
					api_GetCardInsertStatus(pBuf);
					pBuf += Len;
                    
                    break;	

                #endif
				case 0x4030://��ѹ�ϸ��ʲ���
					if( ClassIndex > ObjType[1] )
					{
						return 0x8000;
					}
					
					ObjType += GetTagOffsetAddr( ClassIndex, ObjType);
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					Result = api_ReadEventPara( OI.w, ClassIndex, (BYTE*)&Len, pBuf );
					if(Result == FALSE)
					{
						return 0x8000;
					}

					pBuf += Len;
					break;

				#if( MAX_PHASE == 3 )
				case 0x4100://�����������
				case 0x4101://����ʱ��
					if( ClassIndex != 0 )//��������С�������򲻽��в���
					{
						return 0x8000;
					}

					if( OI.w == 0x4100 )
					{
						*(pBuf++) = FPara2->EnereyDemandMode.DemandPeriod;
					}
					else if( OI.w == 0x4101 )
					{
						*(pBuf++) = FPara2->EnereyDemandMode.DemandSlip;
					}
					break;
				#endif
				case 0x4102://У��������
					if( ClassIndex != 0 )//��������С�������򲻽��в���
					{
						return 0x8000;
					}
					*(pBuf++) = 80;
					break;

				case 0x4112://�й���Ϸ�ʽ������
					if( ClassIndex != 0 )//��������С�������򲻽��в���
					{
						return 0x8000;
					}
					*(pBuf++) = FPara2->EnereyDemandMode.byActiveCalMode;
					break;
				
				#if(SEL_RACTIVE_ENERGY == YES)
				case 0x4113://�޹���Ϸ�ʽ������1
				case 0x4114://�޹���Ϸ�ʽ������2
					if( ClassIndex != 0 )//��������С�������򲻽��в���
					{
						return 0x8000;
					}

					if( OI.w == 0x4113 )
					{
						*(pBuf++) = FPara2->EnereyDemandMode.PReactiveMode;
					}
					else
					{
						*(pBuf++) = FPara2->EnereyDemandMode.NReactiveMode;
					}
					break;
				#endif

				case 0x4116://������
					if( ClassIndex > MAX_MON_CLOSING_NUM )
					{
						return 0x8000;
					}
					
					TagPara.Array_Struct_Num = MAX_MON_CLOSING_NUM;
					ObjType += GetTagOffsetAddr( ClassIndex, ObjType);
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					Result =ReadSettlementDate(pBuf);
					if(Result == FALSE)
					{
						return 0x8000;
					}

					if( ClassIndex != 0 )
					{
						memcpy(pBuf,pBuf+(ClassIndex-1)*2,2);
						pBuf += 2;
					}
					else
					{
						pBuf+=Len;// 0��ʱ�򷵻�ȫ������
					}
					break;

				#if( MAX_PHASE == 3 )
				case 0x4117://�ڼ�������������
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
        			{
                        return 0x8000;
        			}
        			
					memcpy(pBuf,(BYTE*)&FPara2->EnereyDemandMode.IntervalDemandFreezePeriod,3);
					pBuf += Len;
					break;
				#endif
				default:
					return 0x8000;
			}
			break;
		case 3:
		case 4:
			if( OI.w == 0x4106 )//�������
			{
				
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				if(ClassAttribute == 3)
				{
					Type = eMeterMinCurrent;
				}
				else
				{
					Type = eMeterTurningCurrent;
				}
				Len = GetTypeLen( eData, ObjType );
    			if( Len == 0x8000 )
    			{
                    return 0x8000;
    			}
    			
				Result = api_ProcMeterTypePara( READ, Type, pBuf );
				if(Result == FALSE)
				{
					return 0x8000;
				}
				pBuf += Len;
			}
			else if( OI.w == 0x4800 )///����4/�����豸�������ݵ�Ԫ
			{
				MaxIndex = GetFrzorCurLen(4);	//һ��ʶ��������豸����
				if(ClassIndex != 0)				//index�������
				{
					return 0x8000;
				}
				ObjType = (BYTE *)&A_4800_4_ApplFrzData_NotElecHot;
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				
				Result = TRUE;
				Len = GetApplianceFreezData(ClassIndex, MaxIndex, pBuf);
				pBuf += Len;
			}
			else
			{
				return 0x8000;
			}
			break;
		case 5:
			if( OI.w == 0x4000 )//�㲥Уʱ����
			{
				if( ClassIndex > T_TimeBroadCastPara[1] )
				{
					return 0x8000;
				}
				
				api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara ); 
				if( ClassIndex == 0 )
				{
					memcpy(pBuf,(BYTE*)&tmpTimeBroadCastPara,4);
					pBuf += 4;			
				}
				else if( ClassIndex == 1 )//���ù㲥Уʱ��С��Ӧ����
				{
					memcpy(pBuf,(BYTE*)&tmpTimeBroadCastPara.TimeMinLimit,2);
					pBuf += 2;
				}
				else//��ȡ�㲥Уʱ�����Ӧ����
				{
					memcpy(pBuf,(BYTE*)&tmpTimeBroadCastPara.TimeMaxLimit,2);
					pBuf += 2;
				}
			}
			else
			{
				return 0x8000;
			}
			break;

		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//��������:  �Բα�����class8���������Tag
//         
//����:      
//         	*pOAD[in]��		OAD    
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[out]��	�������
//         
//����ֵ:   ���tag������ݳ���
//         
//��ע����: �α�����class8��  OI=4XXX
//--------------------------------------------------
static WORD AddTagMeterPara(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;

	if( OI.w == 0x4300 )//�����豸��
	{
		return (AddTagMeterClass19( pOAD, InBuf, OutBufLen, OutBuf));
	}
	else if( OI.w == 0x4400 )//Ӧ�����ӽӿ���
	{
        return (AddTagMeterClass20( pOAD, InBuf, OutBufLen, OutBuf));
	}
	
	switch( ClassAttribute )
	{
		case 2://����2
			if( OI.w == 0x4004 )//����λ����Ϣ
			{
				return (AddTagPositionInfo( pOAD, InBuf, OutBufLen, OutBuf));
			}
			else if( (OI.w == 0x4016) || (OI.w == 0x4017))//ʱ�α�
			{
				return (AddTagTimeTable(pOAD,InBuf, OutBufLen,OutBuf));
			}
			#if(PREPAY_MODE == PREPAY_LOCAL)  
			else if( (OI.w == 0x401A) || (OI.w == 0x401B) )//���ݵ��
			{
                return (AddTagLadder( pOAD, InBuf, OutBufLen, OutBuf) );

			}
			#endif
			else if( 	(OI.w == 0x4007)	//LCD���� 
					|| 	(OI.w == 0x400C) 	//ʱ��ʱ����
					|| 	(OI.w == 0x4011)	//�������ձ�
					|| 	(OI.w == 0x4014)	//��ǰ��ʱ����
					|| 	(OI.w == 0x4015) 	//������ʱ����
					#if(PREPAY_MODE == PREPAY_LOCAL)  
                    || 	(OI.w == 0x4018)    //��ǰ�׷��ʵ��
                    || 	(OI.w == 0x4019)    //�����׷��ʵ��
                    || 	(OI.w == 0x401e)    //0x401e:���������ֵ
					|| 	(OI.w == 0x401f)    //0x401f:���������ֵ 
                    #endif//#if(PREPAY_MODE == PREPAY_LOCAL) 
					|| 	(OI.w == 0x4116)	//������
					|| 	(OI.w == 0x4030))	//��ѹ�ϸ���
			{

				if( OI.w == 0x4011 )//�������ձ�
				{
					TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.HolidayNum;
					if( ClassIndex > MAX_HOLIDAY )
					{
						return 0x8000;
					}
				}
				else if( (OI.w == 0x4014) || (OI.w == 0x4015) )//ʱ����
				{
					TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.TimeZoneNum;
					if( ClassIndex > MAX_TIME_AREA )
					{
						return 0x8000;
					}
				}
				#if(PREPAY_MODE == PREPAY_LOCAL)  
				else if( (OI.w == 0x4018) || (OI.w == 0x4019) )//��ǰ�׷��ʵ�ۡ������׷��ʵ��
				{
                    TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.Ratio;
					if( ClassIndex > FPara1->TimeZoneSegPara.Ratio )
					{
						return 0x8000;
					}
				}
				#endif
				else if( OI.w == 0x4116 )//������
				{
					TagPara.Array_Struct_Num = MAX_MON_CLOSING_NUM;
					if( ClassIndex > MAX_MON_CLOSING_NUM )
					{
						return 0x8000;
					}
				}
				else
				{
                    if( ClassIndex > ParaMeterObj[Sch].Type2[1] )
                    {
                        return 0x8000;
                    }
				}
				
				Tag = ParaMeterObj[Sch].Type2;
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

				return Lenth;
			}
			else//��ͨ����
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = ParaMeterObj[Sch].Type2;
				Lenth = GetTypeLen( eTagData, Tag) ;
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
				return Lenth;		
			}
			break;
		case 3:
		case 4:	
			if( OI.w == 0x4106 )//����������
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag =T_MinI;
            	TagPara.Lenth.w = InBuf[0];
            	Lenth = GetTypeLen( eTagData, Tag );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf+1, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;
			}
			else if( OI.w == 0x4800 )//Tag����4
			{
				//TagPara.Array_Struct_Num = MessageData.ucMessageID;
				TagPara.Array_Struct_Num = NumofNILM;
				if( ClassIndex > TagPara.Array_Struct_Num)
				{
					return 0x8000;
				}

				if(ClassIndex != 0)		//index�������
				{
					return 0x8000;
				}
				Tag = (BYTE *)&A_4800_4_ApplFrzData;		//��ʱ
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen4804( eTagData, Tag, InBuf);//���⴦��
				if( Lenth == 0x8000 )
				{
					return 0x8000;
				}
					
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag4804( Tag, InBuf, OutBuf );//���⴦��!!!!!!
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;

			}
			else
			{
				return 0x8000;
			}
			break;

		case 5:
			if( OI.w == 0x4000 )//�㲥Уʱ����
			{
				if( ClassIndex > T_TimeBroadCastPara[1] )
				{
					return 0x8000;
				}
				
				Tag = T_TimeBroadCastPara;
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

				return Lenth;
			}
			else
			{
				return 0x8000;
			}
			break;
		default:
			return 0x8000;
	}	
}

//--------------------------------------------------
//��������:  ��ȡ�α�����class8������ OI=4XXX
//         
//����:  
//         	DataType[in]��	eData/eTagData/eAddTag
//         	Dot[in]:		С����(�ݲ���)        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:  	�������ݳ���
//         
//��ע����: 
//--------------------------------------------------
WORD GetRequestMeterPara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	BYTE Sch;
	BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	Sch = SearchParaMeterOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType==eData) || (DataType==eTagData) )
	{
		Lenth = GetProMeterParaData( pOAD, Sch, Buf );
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
			Lenth = AddTagMeterPara(pOAD, Sch, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		if( InBuf == NULL )
		{
			return 0x8000;
		}
		Lenth = AddTagMeterPara(pOAD, Sch, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth; 
}

//--------------------------------------------------
//��������: ��ȡ�α�����class8�����ݳ��� OI=4XXX
//         
//����:     
//         	DataType[in]��	eData/eTagData
//         	*pOAD[in]��		OAD   
//         
//����ֵ:   �������ݳ��� 
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetRequestMeterParaLen( BYTE DataType, BYTE* pOAD)
{
	BYTE ClassAttribute,Sch,ClassIndex;
	TTwoByteUnion OI,Len;	
	const BYTE *Tag;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Sch = SearchParaMeterOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}
	
	if( OI.w == 0x4300 )//�����豸��
	{
		return GetMeterClass19Len(DataType, pOAD);
	}
	else if( OI.w == 0x4400 )//Ӧ�����ӽӿ���
	{
        return GetMeterClass20Len(DataType, pOAD);
	}
	
	switch( ClassAttribute )
	{
		case 2://�ܼ����������������
			if( OI.w == 0x4004 )//����λ����Ϣ
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}
				
				if( ClassIndex == 0 )
				{
					Len.w = ( DataType == eData) ? 12 : 27 ;
				}
				else if( ClassIndex < 3 )
				{
					Len.w = ( DataType == eData) ? 4 :10 ;
				}
				else
				{
					Len.w = ( DataType == eData) ? 4 : 5;
				}
			}
			else if( (OI.w == 0x4016) || (OI.w == 0x4017))//ʱ�α�
			{
				if( ClassIndex > MAX_TIME_SEG_TABLE )//֧���ȸ�ʱ�α���ʱ�����ı�̼�¼
				{
					return 0x8000;
				}

				if( DataType == eData )//Դ���ݰ���󳤶ȴ�����tag��ʵ�ʳ��ȼ�tag
				{
					//Len.w = ( ClassIndex== 0) ? (FPara1->TimeZoneSegPara.TimeSegTableNum*FPara1->TimeZoneSegPara.TimeSegNum*3) : FPara1->TimeZoneSegPara.TimeSegNum*3;
                    Len.w = ( ClassIndex== 0) ? ( MAX_TIME_SEG_TABLE*MAX_TIME_SEG*3) : (MAX_TIME_SEG*3);
				}
				else
				{
					Len.w = ( ClassIndex == 0) ? ((2+8*FPara1->TimeZoneSegPara.TimeSegNum)*FPara1->TimeZoneSegPara.TimeSegTableNum+2) : (2+8*FPara1->TimeZoneSegPara.TimeSegNum);
					//Len.w = ( ClassIndex == 0) ? ((2+8*MAX_TIME_SEG)*MAX_TIME_SEG_TABLE+2) : (2+8*MAX_TIME_SEG);
				}
			}
			#if(PREPAY_MODE == PREPAY_LOCAL)  
			else if( (OI.w == 0x401A) || (OI.w == 0x401B) )//���ݵ��
			{
                if( ClassIndex == 0 )
                {
                    Len.w = (( DataType== eData) ? ((2*MAX_LADDER+1)*4+MAX_YEAR_BILL*3) : (2+6+(2*MAX_LADDER+1)*5+MAX_YEAR_BILL*8));
                }
                else if( ClassIndex == 1 )//����ֵ
                {
                    Len.w = (( DataType== eData) ? (MAX_LADDER*4) : (2+MAX_LADDER*5) );
                }
                else if( ClassIndex == 2 )//���ݵ��
                {
                    Len.w = (( DataType== eData) ? ((MAX_LADDER+1)*4) : (2+(MAX_LADDER+1)*5) );
                }
                else if( ClassIndex == 3 )//���ݽ���������
                {
                    Len.w = (( DataType== eData) ? (MAX_YEAR_BILL*3) : (2+(MAX_YEAR_BILL)*8) );
                }
			}
			#endif
			else if( 	(OI.w == 0x4007)	//LCD���� 
					|| 	(OI.w == 0x400C) 	//ʱ��ʱ����
					|| 	(OI.w == 0x4011)	//�������ձ�
					|| 	(OI.w == 0x4014)	//��ǰ��ʱ����
					|| 	(OI.w == 0x4015) 	//������ʱ����
                    #if(PREPAY_MODE == PREPAY_LOCAL)  
                    || 	(OI.w == 0x4018)    //��ǰ�׷��ʵ��
                    || 	(OI.w == 0x4019)    //�����׷��ʵ��
                    || 	(OI.w == 0x401e)    //0x401e:���������ֵ
					|| 	(OI.w == 0x401f)    //0x401f:���������ֵ 
                    #endif//#if(PREPAY_MODE == PREPAY_LOCAL)  
					|| 	(OI.w == 0x4116)	//������
					|| 	(OI.w == 0x4030) )	//��ѹ�ϸ���
			{
				if( OI.w == 0x4011 )//�������ձ�
				{
					TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.HolidayNum;
					if( ClassIndex > MAX_HOLIDAY )//֧���ȸĹ������ձ����Ĺ����������ı�̼�¼
					{
						return 0x8000;
					}
				}
				else if( (OI.w == 0x4014) || (OI.w == 0x4015) )//ʱ����
				{
					//Դ���ݰ���󳤶ȴ�����tag��ʵ�ʳ��ȼ�tag
					if( DataType == eData )
					{
                        TagPara.Array_Struct_Num = MAX_TIME_AREA;
					}
					else
					{
                        TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.TimeZoneNum;
					}
					
					if( ClassIndex > MAX_TIME_AREA )
					{
						return 0x8000;
					}
				}
                #if(PREPAY_MODE == PREPAY_LOCAL)
		        else if( (OI.w == 0x4018) || (OI.w == 0x4019) )//��ǰ�׷��ʵ�ۡ������׷��ʵ��
				{
					if( DataType == eData )
					{
						TagPara.Array_Struct_Num = MAX_RATIO;
					}
					else
					{
						TagPara.Array_Struct_Num = FPara1->TimeZoneSegPara.Ratio;
					}
                    
					if( ClassIndex > FPara1->TimeZoneSegPara.Ratio )
					{
						return 0x8000;
					}
				}
                #endif//#if(PREPAY_MODE == PREPAY_LOCAL)
				else if( OI.w == 0x4116 )//������
				{
					TagPara.Array_Struct_Num = MAX_MON_CLOSING_NUM;
					if( ClassIndex > MAX_MON_CLOSING_NUM )
					{
						return 0x8000;
					}
				}
				else
				{
                    if( ClassIndex > ParaMeterObj[Sch].Type2[1] )
                    {
                        return 0x8000;
                    }
				}
			
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( DataType, Tag );
				return Len.w;
			}
			else//��ͨ����
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = ParaMeterObj[Sch].Type2;
				Len.w = GetTypeLen( DataType, Tag) ;
				return Len.w;		
			}		
			break;
		case 4:
			if( OI.w == 0x4800 )//Len����4
			{
				TagPara.Array_Struct_Num = NumofNILM;
				if( ClassIndex > TagPara.Array_Struct_Num)
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4800_4_ApplFrzData;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen4804( eTagData, Tag ,(BYTE*)&tApplFrzDataFLASH[0]);
			}
			break;
		case 5:
			if( OI.w == 0x4000 )//�㲥Уʱ����
			{
				if( ClassIndex > T_TimeBroadCastPara[1] )
				{
					return 0x8000;
				}
				
				Tag = T_TimeBroadCastPara;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( DataType, Tag );
				return Len.w;
			}
			else
			{
				return 0x8000;
			}
			break;

		default:		
			return 0x8000;
	}

	return Len.w;
}

#endif//#if ( SEL_DLT698_2016_FUNC == YES)

