//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ�
//��������	2016.12.23
//����		DL/T 698.45�������Э����ܶ�ȡC�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
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
//-----------------------------------------------
static const TScaler_Unit SU_Active			= 	{ -2,   UNIT_KWH    };	//���ܻ��㵥λ
static const TScaler_Unit SU_HActive 		= 	{ -4,   UNIT_KWH 	};	//�߾��ȵ��ܻ��㵥λ
static const TScaler_Unit SU_TAILActive 	= 	{ -6,   UNIT_KWH   };	//����β�����ܻ��㵥λ

static const TScaler_Unit SU_RActive 		= 	{ -2, 	UNIT_KVARH	};	//�޹����ܻ��㵥λ
static const TScaler_Unit SU_HRActive 		= 	{ -4, 	UNIT_KVARH  };	//�߾����޹����ܻ��㵥λ
static const TScaler_Unit SU_TAILRActive 	= 	{ -6, 	UNIT_KVARH	};	//�޹�����β�����㵥λ


static const TScaler_Unit SU_AActive 		= 	{ -2, 	UNIT_KVAH	};	//���ڵ��ܻ��㵥λ
static const TScaler_Unit SU_HAActive 		= 	{ -4,   UNIT_KVAH   };	//�߾������ڵ��ܻ��㵥λ
static const TScaler_Unit SU_TAILAActive 	= 	{ -6, 	UNIT_KVAH	};	//���ڵ���β�����㵥λ


//�����������ͣ�����ֻΪ��ʽһ�£���ʹ��
static const BYTE T_Active[]            	= { Array_698,0xff,Double_long_698,     4};	//���������ݱ�ʶ
static const BYTE T_HActive[]           	= { Array_698,0xff,Long64_698,          8};	//�߾��ȵ��������ݱ�ʶ
static const BYTE T_TailActive[]			= { Array_698,0xff,Integer_698,      	1};//����β��
static const BYTE T_Unsigned_Active[]		= { Array_698,0xff,Double_long_unsigned_698,  4};//�޷��ŵ��������ݱ�ʶ
static const BYTE T_Unsigned_HActive[]		= { Array_698,0xff,Long64_unsigned_698,     8};//�޷��Ÿ߾��ȵ��������ݱ�ʶ
static const BYTE T_Unsigned_TailActive[]	= { Array_698,0xff,Unsigned_698,      	1};//�޷��ŵ���β��


typedef struct TEnergyObj_t
{
	WORD				OI;				//698�����ʶ��OI�����ʶ
	const TScaler_Unit 	*Scaler_Unit3;	//���ܻ��㵥λ
	const TScaler_Unit 	*Scaler_Unit5;	//�߾��ȵ��ܻ��㵥λ
	const BYTE         	*Type2;			//���������ݱ�ʶ
	const BYTE         	*Type4;			//�߾��ȵ��������ݱ�ʶ
	const TScaler_Unit 	*Scaler_Unit7;	//����β����λ
	const BYTE         	*Type6;			//����β��
}TEnergyObj;

//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
static const TEnergyObj EnergyObj[] =
{
	{ 0x0000, &SU_Active, 	&SU_HActive, 	T_Active, 			T_HActive,			&SU_TAILActive,		T_TailActive	 	 	},//����й�����
	{ 0x0010, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//�����й�����
	{ 0x0020, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//�����й�����
	#if( SEL_SEPARATE_ENERGY == YES )//�Ƿ�֧�ַ������			SEPARATE_ENER֧�ַ������
	{ 0x0011, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//A�������й�����
	{ 0x0012, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//B�������й�����
	{ 0x0013, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//C�������й�����
	{ 0x0021, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//A�෴���й�����
	{ 0x0022, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//B�������й�����
	{ 0x0023, &SU_Active, 	&SU_HActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILActive,		T_Unsigned_TailActive	},//C�������й�����
	#endif
#if( SEL_RACTIVE_ENERGY == YES )//�Ƿ�֧���޹�����	
	{ 0x0030, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//����޹�1����
	{ 0x0040, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//����޹�2����
	{ 0x0050, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//��һ�����޹�����
	{ 0x0060, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//�ڶ������޹�����	
	{ 0x0070, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//���������޹�����
	{ 0x0080, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//���������޹�����
	#if( SEL_SEPARATE_ENERGY == YES )//�Ƿ�֧�ַ������				SEPARATE_ENER֧�ַ������				
	{ 0x0031, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//A������޹�1����
	{ 0x0032, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//B������޹�1����
	{ 0x0033, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//C������޹�1����
	{ 0x0041, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//A������޹�2����
	{ 0x0042, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//B������޹�2����
	{ 0x0043, &SU_RActive, 	&SU_HRActive, 	T_Active, 			T_HActive,			&SU_TAILRActive,	T_TailActive 			},//C������޹�2����
	{ 0x0051, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//A���һ�����޹�����
	{ 0x0052, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//B���һ�����޹�����
	{ 0x0053, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//C���һ�����޹�����
	{ 0x0061, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//A��ڶ������޹�����
	{ 0x0062, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//B��ڶ������޹�����
	{ 0x0063, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//C��ڶ������޹�����
	{ 0x0071, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//A����������޹�����
	{ 0x0072, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//B����������޹�����
	{ 0x0073, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//C����������޹�����
	{ 0x0081, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//A����������޹�����
	{ 0x0082, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//B����������޹�����
	{ 0x0083, &SU_RActive, 	&SU_HRActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILRActive,	T_Unsigned_TailActive	},//C����������޹�����
	#endif
#endif	
#if( SEL_APPARENT_ENERGY == YES )//�Ƿ�֧�����ڵ���
	{ 0x0090, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//�������ڵ���
	{ 0x00A0, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//�������ڵ���
	#if( SEL_SEPARATE_ENERGY == YES )//�Ƿ�֧�ַ������	SEPARATE_ENER֧�ַ������
	{ 0x0091, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//A���������ڵ���
	{ 0x0092, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//B���������ڵ���
	{ 0x0093, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//C���������ڵ���
	{ 0x00A1, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//A�෴�����ڵ���
	{ 0x00A2, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//B�෴�����ڵ���
	{ 0x00A3, &SU_AActive, 	&SU_HAActive, 	T_Unsigned_Active, 	T_Unsigned_HActive,	&SU_TAILAActive,	T_Unsigned_TailActive	},//C�෴�����ڵ���
	#endif
#endif
};								

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
static BYTE SearchEnergyOAD( const WORD OI )
{
	BYTE i,Num;//����������ܳ���255

	Num = (sizeof(EnergyObj)/sizeof(TEnergyObj));
	if( Num >= 0x80 )//������ѭ��
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
//			GetDataType[in]  GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//			Dot[in]:		 ��ȡ���ݵ�С����λ����oxff--��ʾ��ԼĬ�ϵ�С��λ������Լ���ã�
//			*pOAD[in]:		 OAD         
//			EnergyIndex[in]: EnergyObj�е�����         
//			*OutBuf[in]:	�������
//         
//����ֵ:   ���ص������ݳ��� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע: ����tag
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
	
	//��ȡ���������ͣ���ReadEnergyHexʹ��
	EnergyType = ((OI.b[0]&0x0f)*0x1000)+((OI.b[0]&0xf0)>>4);
	Num = ((ClassIndex == 0) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	Ratio = ((ClassIndex == 0) ? 0 : (ClassIndex-1));
	Num = (Num+Ratio);	
	
	#if( SEL_SEPARATE_RATIO_ENERGY == NO )//��֧�ַ�����ʵ���
	if( (OI.b[0]&0x03) != 0 )//��ȡ�������
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
		case 2://�ܼ����ʵ���������		
		case 4://�߾����ܼ����ʵ���������
		case 6://����β��
			if( ClassIndex > (FPara1->TimeZoneSegPara.Ratio+1) )//Ԫ���������ܴ��ڷ�����+1
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

			if( ClassAttribute == 6 )//����β��
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
					EnergyType = (EnergyType | 0x8000);//����8���ֽڣ����ⳬ��
					NumBak = (Num-Ratio);
					for( ; Ratio<Num; Ratio++ )
					{
						Result = api_GetCurrEnergyData(EnergyType, DATA_HEXCOMP, Ratio, Dot, OutBuf+Len);
						if(Result == FALSE)
						{
							return 0x8000;
						}
						Len += 8;//ƫ�Ƶ�ַ
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
			
		case 3://���㼰��λ
		case 5://�߾��Ȼ��㼰��λ
		case 7://����β��
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
//��������:  �Ե������ݽ��м�TAG
//         
//����:     	GetDataType[in]		GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//			*pOAD[in]��			OAD         
//          EnergyIndex[in]:	EnergyObj�е�����         
//          *InBuf[in]��		��Ҫ���tag������         
//          OutBufLen[in]��		OutBuf����       
//          *OutBuf[out]��		�������
//         
//����ֵ:    �������tag�����ݳ��� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע:  	��
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

	#if( SEL_SEPARATE_RATIO_ENERGY == NO )//��֧�ַ�����ʵ���
	if( (OI.b[0]&0x03) != 0 )//��ȡ�������
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
		case 2://�ܼ����ʵ���������
		case 4://�߾����ܼ����ʵ���������
		case 6://����β��
			if( ClassIndex > (MAX_RATIO+1) )
			{
				return 0x8000;
			}
			if( OutBufLen < Lenth )//��������С�������򲻽��в���
			{
				return 0;
			}
			
			if( GetDataType == eGetRecordData )
			{
				if( ClassAttribute != 6 )//����β��������
				{
					if( ClassAttribute == 2 )
					{
						SwitchEnergyType = SwitchNormalEnergy;
					}
					else
					{
						SwitchEnergyType = SwitchHighEnergy;
					}

					memcpy( Buf, InBuf, 5*Num );//ʹ������buf ����inbuf���ٵĿռ䲻�����³���
					api_EnergyDBaseToEnergy( SwitchEnergyType, Num, sizeof(Buf),Buf);//ת��Ϊ����
					InBuf = Buf;//ָ�븳ֵ��Ϊ�˺��洦��ʽһ�£��˴�ʹ��һ��ע�⣬�����������---jwh
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

		case 3://���㼰��λ
		case 5://�߾��Ȼ��㼰��λ
		case 7://����β��
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
//����:     
//			 GetDataType[in] GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//			 DataType[in]��	eData/eTagData/eAddTag        
//           Dot[in] :		С����        
//           *pOAD[in]��	    OAD         
//           *InBuf[in]��	��Ҫ��ӱ�ǩ������         
//           OutBufLen[in]�� ����OutBuf�����ݳ���         
//           *OutBuf[in]��	���صĵ�������
//         
//����ֵ:    ���ص��ܳ��� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ���ʸ������ܳ���12
//--------------------------------------------------
WORD GetRequestEnergy( eGetDataType GetDataType, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE EnergyIndex;
	BYTE Buf[(MAX_RATIO+1)*8+50];//����BUF��󳤶�
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
//��������:  ����TAG��ȡ���ܳ���
//         
//����:      
//			GetDataType[in]	GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//			DataType[in]��	eData/eTagData
//			*pOAD[in]��		OAD
//         
//����ֵ:   ����OAD��Ӧ���ݵĳ���  0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
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
	

	if( DataType == eMaxData )//ʹ�����ֵ
	{
	    DataType = eData;
        Num = MAX_RATIO+1;
	}
	else
	{
        Num = FPara1->TimeZoneSegPara.Ratio+1;
	}
	
	#if( SEL_SEPARATE_RATIO_ENERGY == NO )//��֧�ַ�����ʵ���
	if( (OI.b[0]&0x03) != 0 )//��ȡ�������
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
		case 2://�ܼ����ʵ���������
		case 4://�߾����ܼ����ʵ���������
		case 6://����β��
			if( ClassIndex > (MAX_RATIO+1) )//�˴���Ԫ���������ж�ʹ�õ�ǰ���ʵ��ֵ �������ֵ
			{
				return 0x8000;
			}
			
			if( GetDataType == eGetRecordData )//���̶ܹ����� ������4�ֽ�+1�ֽ� ���⴦��
			{
				if( ClassAttribute != 6 )//����β��������
				{
					if( DataType == eData )//ֱ�ӷ��س���
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
					else//����������TAG����һ��
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

		case 3://���㼰��λ
		case 5://�߾��Ȼ��㼰��λ
		case 7://����β��
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

//-----------------------------------------------
//��������: ��¼�е�����ת��
//
//����:
//		pOAD[in]: ����OAD
//
//		Len[in]: ����bufg����
//
//		Buf[in]:ָ����ܵ�ָ�� ����hex
//
//		OutBufLen[in] ������ݳ���
//
//		OutBuf[in]	���buf
//
//����ֵ:
//
//��ע: ���ܽ����ݷ�д��Buf�У���Ϊ����ĳ��Ȳ������ܻᳬ�ޣ�ֱ�ӻ�дoutbuf---jwh
//-----------------------------------------------
WORD api_RecordDataTransform( BYTE* pOAD, WORD* BufLen , BYTE* Buf, WORD OutBufLen,BYTE* OutBuf )
{
	BYTE ClassAttribute,ClassIndex,Num;
	BYTE tmpBuf[(MAX_RATIO+1)*8+50];//����BUF��󳤶�
	TTwoByteUnion OI;

	lib_ExchangeData(OI.b,pOAD,2);

	if( OI.w < 0x1000 )//�������� ��Ҫ������ת��Ϊ����
	{
		if( *BufLen > (MAX_RATIO+1)*8 )
		{
			return 0x8000;
		}
		
		ClassAttribute = (pOAD[2]&0x1f);
		ClassIndex = pOAD[3];
		if( (ClassAttribute == 2) || (ClassAttribute == 4) )//��ͨ���� �߾��ȵ���
		{
			Num = (*BufLen/5);
			memcpy( tmpBuf, Buf, *BufLen );//ʹ������buf ����inbuf���ٵĿռ䲻�����³���
			if( ClassAttribute == 2 )
			{
				*BufLen = api_EnergyDBaseToEnergy( SwitchNormalEnergy, Num, sizeof(tmpBuf), tmpBuf );//ת��Ϊ����
			}
			else
			{
				*BufLen = api_EnergyDBaseToEnergy( SwitchHighEnergy, Num, sizeof(tmpBuf), tmpBuf );//ת��Ϊ����
			}
			if(*BufLen == 0x8000)
			{
				return 0;
			}
			if( OutBufLen >= *BufLen )//�������ݿռ��㹻
			{
				memcpy( OutBuf, tmpBuf, *BufLen );
			}
			else
			{
				return 0;
			}

		}
		else//�����������Ҫ�жϣ��ϲ㺯�����жϹ��ˣ���֮ǰ����һ��
		{
			memcpy( OutBuf, Buf, *BufLen);
		}
	}
	else//�����������Ҫ�жϣ��ϲ㺯�����жϹ��ˣ���֮ǰ����һ��
	{
		memcpy( OutBuf, Buf, *BufLen);
	}

	return *BufLen;
}


#endif//#if ( SEL_DLT698_2016_FUNC == YES)

