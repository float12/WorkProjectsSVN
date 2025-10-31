//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ�
//��������	2016.01.09
//����		DL/T 698.45�������Э����ܶ�ȡC�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
//������ݱȽϳ�������������
//#define MAX_OTHER_BUF_LENTH         (MAX_LCD_KEY_ITEM*10+30)//Һ��������ʾΪ���BUF����
#define MAX_OTHER_BUF_LENTH         (99*10+30)//Һ��������ʾΪ���BUF����

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

typedef struct TOtherObj_t
{
	WORD				OI;					//698�����ʶ��OI�����ʶ
	BYTE         		Class;				//������
	BYTE         		MAXClassAttribute;	//�������
}TOtherObj;


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//�˿������� ascii��
const char PortDescript0[] 	= "RS4851";
const char PortDescript1[] 	= "INFRA";
const char PortDescript2[]	= "HPLC+HRF";
const char PortDescript3[] 	= "RS4852";
const char PortDescript4[] 	= "RS485";
const char PortDescript5[] 	= "CAN";
const char Relayscript[]   	= "RELAY";
//ң������2	
static const BYTE T_RemoteControl[]		= 
{ 
	Structure_698,				2,
	Double_long_unsigned_698,	4,
	Long_unsigned_698,			2,
};

//��ȫģʽ����3
static const BYTE T_SafeMode3[]		= 
{ 
	Array_698,			0xff,
	Structure_698,		2,
	OI_698,				2,
	Long_unsigned_698,	2,
};

//RS485����2
static const BYTE T_RS485[]		= 
{ 
	Array_698,			0xff,
	Structure_698,		3,
	Visible_string_698,	0xff,
	COMDCB_698,			5,
	Enum_698,			1,
};
//�ز�����
static const BYTE T_CR[]		= 
{ 
	Array_698,			0xff,
	Structure_698,		3,
	Visible_string_698,	0xff,
	COMDCB_698,			5,
	NULL_698,			1,
};
//CAN����2
static const BYTE T_CAN[]		=
{
	Array_698,			1,
	Structure_698,		2,
	Visible_string_698,	0xff,
	COMDCB_698,			5,
};

//��������2
static const BYTE T_IR[]		= 
{ 
	Array_698,			1,
	Structure_698,		2,
	Visible_string_698,	0xff,
	COMDCB_698,			5,
};
//�̵������
static const BYTE T_RelayOut[]		= 
{ 
	Array_698,			1,
	Structure_698,		4,
	Visible_string_698,	0xff,
	Enum_698,			1,
	Enum_698,			1,
	Enum_698,			1,
};
//�๦�ܶ���
static const BYTE T_MultiFunPort[]		= 
{ 
	Array_698,			1,
	Enum_698,			1,
};
//ң������2
static const BYTE T_SwitchIn_2[]	=
{
	Array_698,		1,
	Structure_698,		2,
	Unsigned_698,		1,
	Unsigned_698,		1,
};
//ң������4
static const BYTE T_SwitchIn_4[]	=
{
	Structure_698,		2,
	Bit_string_698,		8,
	Bit_string_698,		8,
};
//��ʾ����
static const BYTE T_Display[]		= 
{ 
	Structure_698,	2,
	Unsigned_698,	1,
	Unsigned_698,	1,
};

//ESAM������
static const BYTE T_NowCount[]		= 
{ 
	Structure_698,				    3,
	Double_long_unsigned_698,		4,
	Double_long_unsigned_698,		4,
	Double_long_unsigned_698,		4,
};

//ESAM�ն˻Ự����
static const BYTE T_TerminalLimit[]		= 
{ 
	Structure_698,				    2,
	Double_long_unsigned_698,		4,
	Double_long_unsigned_698,		4,
};
static const BYTE T_TransfeStatus[] = // ����״̬��
{
	Bit_string_698,		0xFF,
};
static const BYTE T_DownloadID[] = // ���ط���ʶ
{
	Visible_string_698,	0xFF,
};
static const BYTE T_BlockSize[] = // �����ߴ�
{
	Long_unsigned_698,	2,
};
static const BYTE T_UpdatTime[] = // ����ʱ��
{
	DateTime_S_698,	7,
};

// const BYTE T_BluetoothModule[] = //����ģ�� ����2
// {
// 	Structure_698,				5,
// 	Unsigned_698,				1,		//ģ�����

// 	Structure_698,				2,		//BluetoothInfo		T_BluetoothInfo
// 	Visible_string_698,			0x09,	//����������
// 	Octet_string_698,			0x06,	//����MAC��ַ

// 	Visible_string_698,			0x06,	//�������
// 	Enum_698,					1,		//���ģʽ
// 	Octet_string_698,			20,		//�������к�
// };

// const BYTE T_BluetoothDevice[] = //��������4�� �������豸 
// {
// 	Structure_698,				2,
// 	Octet_string_698,			0x06,	//����MAC��ַ
// 	Octet_string_698,			0x10,	//�����ʲ���
// };

// const BYTE T_BluetoothWork[] =	//��������6  ��������
// {
// 	Structure_698,			4,		
// 	OAD_698, 				4,		//�˿ں�
// 	Unsigned_698, 			4,		//���书�ʵ�
// 	Long_unsigned_698,		2,		//�㲥���
// 	Long_unsigned_698,		2,		//ɨ����
// };

//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
static const TOtherObj OtherObj[] =
{
	{  0x8000, 	eclass8,	5	},	//ң��
	{  0x8001, 	eclass8,	2	},	//����
	{  0xF100, 	eclass21,	21	},	//ESAM
	{  0xF101, 	eclass8,	3	},	//��ȫģʽ����
	{  0xF201, 	eclass22,	2	},	//RS485
	{  0xF202, 	eclass22,	2	},	//����
	{  0xF203, 	eclass22,	4	},	//ң��
	{  0xF205, 	eclass22,	2	},	//�̵������
	{  0xF207, 	eclass22,	2	},	//�๦�ܶ���
	{  0xF209, 	eclass22,	2	},	//�ز�/΢�������߽ӿ�
	// {  0xF20B, 	eclass22,	7	},	//����ģ��
	{  0xF221, 	eclass22,	7	},	//CAN
	{  0xF300, 	eclass17,	4	},	//�Զ�����
	{  0xF301, 	eclass17,	4	},	//��������
	{  0xF001, 	eclass18,	7	},	//�ļ��ֿ鴫��
};


//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//--------------------------------------------------
//��������:  ���ݱ����Ҳ���OAD����
//         
//����:      OI[in]��OI
//         
//����ֵ:    ��������
//         
//��ע����:  ��
//--------------------------------------------------
static BYTE SearchOtherOAD( WORD OI )
{
	BYTE i,Num;//����������ܳ���255

	Num = (sizeof(OtherObj)/sizeof(TOtherObj));
	if( Num >= 0x80 )//������ѭ��
	{
		return 0x80;
	}
	
	for(i=0; i<Num; i++)
	{
		if( OI == OtherObj[i].OI )
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
//��������: ��ȡ������8000��8001��F101����class8���������ݣ�����Tag��
//         
//����:
//			Dot[in]��	С���� ��δ�õ���    
//          *pOAD[in]��	OAD         
//          Sch[in]��   OtherObj�е��¼�����      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: OI 	8000 ����2 ����4 ����5
//				8001 ����2
//				F101 ����2 ����3
//--------------------------------------------------
static WORD GetOtherClass8Data( BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num;
	TTwoByteUnion OI;	
	WORD Result,Lenth;
	BYTE *pBufBak = pBuf;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( OI.w )
	{
	case 0x8000://Զ�̿���
		switch( ClassAttribute )
		{
		case 0x02:
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}
			
			Tag = T_RemoteControl;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eData, Tag );
			if( Lenth == 0x8000 )
			{
                return 0x8000;
			}
			
			i = (ClassIndex == 0) ? 0 : (ClassIndex-1);
			Num = (ClassIndex == 0) ? 2 : 1;
			Num += i;

			for( ; i<Num; i++ )
			{
				Result = ReadRelayPara(i,pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}

				if( i == 0 )
				{
					pBuf += 4;
				}
				else
				{
					pBuf += 2;
				}
			}
			break;
		case 0x04:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( api_GetRelayStatus(3) == FALSE )
			{
				*(pBuf++) = 0x00;
			}
			else //Զ�̱���
			{
				*(pBuf++) = 0x01;
			}
			break;
		case 0x05:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			//�����բ
			#if(PREPAY_MODE == PREPAY_LOCAL)
			if( api_GetRelayStatus(7) == 0 )
			#else
			if( api_GetRelayStatus(0) == 0 )
			#endif
			{
				*(pBuf++) = 0x00;
			}
			else //������բ
			{
				*(pBuf++) = 0x01;
			}
			break;
		default:
			return 0x8000;
		}
		break;
	case 0x8001://����
		if( ClassAttribute == 0x02 )
		{
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( api_GetRelayStatus(2) == FALSE )
			{
				*(pBuf++) = 0x00;
			}
			else //����״̬
			{
				*(pBuf++) = 0x01;
			}
		}
		else
		{
			return 0x8000;
		}
		break;
	#if(SEL_ESAM_TYPE != 0)
	case 0xF101://��ȫģʽ����
		if( ClassAttribute == 0x02 )
		{
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			*(pBuf++) = 0x01;
		}
		else if( ClassAttribute == 0x03 )
		{
			if( ClassIndex > SafeModePara.Num )
			{
				return 0x8000;
			}
			
			i = (ClassIndex == 0) ? 0 : (ClassIndex-1);
			Num = (ClassIndex == 0) ? SafeModePara.Num : 1;
			TagPara.Array_Struct_Num = Num;
			Num += i;

			for( ; i<Num; i++ )
			{
				*(pBuf++) = SafeModePara.Sub[i].OI/0x100;//OI�ĸ��ֽ� OI�ȴ����ֽڣ�
				*(pBuf++) = SafeModePara.Sub[i].OI&0xff;//OI�ĵ��ֽ�
				*(pBuf++) = SafeModePara.Sub[i].wSafeMode&0xff;//safemode�ĵ��ֽ�  ��ȫģʽ�ȴ����ֽڣ�����EEPROM�д洢���ɣ�jwh�Ǳ߸���Э�鵹���� wlk
				*(pBuf++) = SafeModePara.Sub[i].wSafeMode/0x100;//safemode�ĸ��ֽ�
			}
		}
		else
		{
			return 0x8000;
		}
		break;
	#endif
	default:
		break;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//��������: �Բ�����8000��8001��F101����class8���������Tag
//         
//����:
//         	*pOAD[in]��		OAD
//			Sch[in]��		OtherObj�е��¼�����         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: OI 	8000 ����2 ����4 ����5
//				8001 ����2
//				F101 ����2 ����3
//--------------------------------------------------
static WORD AddTagOtherClass8Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( OI.w )
	{
		case 0x8000:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				
				Tag = T_RemoteControl;
				Tag += GetTagOffsetAddr(ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else if( (ClassAttribute==0x04) || (ClassAttribute==0x05) )
			{
			    if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Tag = T_BitString8;
				Lenth = GetTypeLen( eTagData, T_BitString8 );	
    		    if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}
			break;
		case 0x8001:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Tag = T_Enum;
				Lenth = GetTypeLen( eTagData, T_Enum );	
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}			
			break;
		case 0xf101:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Tag = T_Enum;
				Lenth = GetTypeLen( eTagData, T_Enum );	
    		    if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else if( ClassAttribute == 0x03 )
			{
				if( ClassIndex > SafeModePara.Num )
				{
					return 0x8000;
				}
				if(SafeModePara.Num == 0)//�㽭�û�Ҫ��Ϊ��ʱ����0100
				{
					OutBuf[0]=0x01;
					OutBuf[1]=0x00;
					return 2;
				}
				
				TagPara.Array_Struct_Num = (ClassIndex == 0) ? SafeModePara.Num : 1;
				Tag = T_SafeMode3;
				Tag += GetTagOffsetAddr(ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
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

//--------------------------------------------------
//��������: ��ȡ������8000��8001��F101����class8�����ݳ���
//         
//����:     
//          DataType[in]��eData/eTagData
//          *pOAD[in]     OAD    
//         
//����ֵ:   �������ݳ���
//         
//��ע����: OI 	8000 ����2 ����4 ����5
//				8001 ����2
//				F101 ����2 ����3
//--------------------------------------------------
static WORD GetRequestOtherClass8Len( BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( OI.w )
	{
		case 0x8000:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				
				Tag = T_RemoteControl;
				Tag += GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( DataType, Tag );
    		    if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else if( (ClassAttribute==0x04) || (ClassAttribute==0x05) )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Lenth = GetTypeLen( DataType, T_BitString8 );	
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}
			break;
		case 0x8001:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Lenth = GetTypeLen( DataType, T_Enum );	
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}
			break;
		case 0xF101:
			if( ClassAttribute == 0x02 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Lenth = GetTypeLen( DataType, T_Enum );
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else if( ClassAttribute == 0x03 )
			{
				if( ClassIndex > SafeModePara.Num )
				{
					return 0x8000;
				}
				
				TagPara.Array_Struct_Num = (ClassIndex == 0) ? SafeModePara.Num : 1;
				Tag = T_SafeMode3;
				Tag += GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( DataType, Tag );
    			if( Lenth == 0x8000 )
    			{
                    return 0x8000;
    			}
			}
			else
			{
				return 0x8000;
			}
			break;
		default:
			break;
	}
	
	return Lenth;
}

//--------------------------------------------------
//��������: ��ȡESAM������F100����class21���������ݣ�����Tag��
//         
//����:
//			Dot[in]��	С���� ��δ�õ���    
//          *pOAD[in]��	OAD         
//          Sch[in]��   OtherObj�е��¼�����      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD GetOtherClass21Data( BYTE Ch, BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{	
	BYTE ClassAttribute,ClassIndex,i,Value,Index;
	BYTE Tag[] = {0,0,2,1,4,5,5,6,8,0x11,0x12,0x13,0x14};
	WORD Result;
	TTwoByteUnion OI,DataLen;
	TFourByteUnion ConnectValidTime;
	BYTE *pBufBak = pBuf;		
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1F);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	
	switch( ClassAttribute )
	{
		case 0x02: 	//��ȡESAM���к�
		case 0x03: 	//��ȡESAM�汾��
		case 0x04: 	//��ȡ�Գ���Կ�汾
		case 0x05: 	//��ȡ�ỰʱЧ����
	    case 0x06: 	//��ȡ�Ựʣ��ʱ��
		case 0x07: 	//��ȡ������
		case 17:  	//��ȡ���ؼ���ָ��ʹ��Ȩ��---��������ı�ֵ-jwh
		case 18: 	//��ȡ�ն˵�ַ
		case 19: 	//��ȡ�ն˹㲥������
		case 20: 	//��ȡ�ն�����Ự������
		case 21: 	//��ȡ�ն˻Ự����
			if( ClassAttribute == 7 )//��վ������
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}
			}
			else if( ClassAttribute == 21 )//�ն˻Ự����
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
			}
			else
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
			}
			
			Index = ClassAttribute;
			if( Index >= 17 )//��������ı�ֵ-jwh
			{
				Index -= 9;
			}
			
			DataLen.w = api_GetEsamInfo( Tag[Index], pBuf );
			if( DataLen.w > (MAX_OTHER_BUF_LENTH - 2) )
			{
				return 0x8000;
			}
			
			if( (ClassAttribute == 5) || (ClassAttribute == 6))//�ỰʱЧ���ޡ��Ựʣ��ʱ��
			{
			    if( (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE) && (ClassAttribute == 6) )//δ����Ӧ������ʱ �Ựʣ��ʱ��Ϊ0
    			{
    				memset( pBuf, 0x00, 4 );
    			}
    			else
    			{
    				//�˴�ʹ��exchange����Ϊ���ݱ�ʶλdoublelong����tagʱ����е���Ϊ��ֹ���ݲ���������Դ�����е���
    				lib_ExchangeData( pBuf, pBuf+4*(ClassAttribute-5), 4 );
    			}
				pBuf += 4;
			}
			else if( (ClassAttribute == 19) || (ClassAttribute == 20) )//�ն˹㲥���������ն�����Ự������
			{
				//�˴�ʹ��exchange����Ϊ���ݱ�ʶλdoublelong����tagʱ����е���Ϊ��ֹ���ݲ���������Դ�����е���
				lib_InverseData( pBuf, 4 );
				pBuf += 4;
			}
			else if( ClassAttribute == 7 )//��վ�Ự������
			{
				if( ClassIndex == 0 )
				{
					for( i=0; i < 3; i++ )
					{
						lib_InverseData( pBuf, 4 );
						pBuf += 4;
					}
					
				}
				else
				{
					lib_ExchangeData( pBuf, pBuf+4*(ClassIndex-1), 4 );
					pBuf += 4;
				}				
			}
			else if( ClassAttribute == 21 )//�ն˻Ự����
			{
				if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE )//δ����Ӧ������ʱ �Ựʣ��ʱ��Ϊ0
    			{
    				memset( pBuf+4, 0x00, 4 );
    			}
    			
				if( ClassIndex == 0 )
				{
					for( i=0; i < 2; i++ )
					{
						lib_InverseData( pBuf, 4 );
						pBuf += 4;
					}
				}
				else
				{
					lib_ExchangeData( pBuf, pBuf+4*(ClassIndex-1), 4 );
					pBuf += 4;
				}	
			}
			else
			{
				pBuf += DataLen.w;
			}
			break;
		
		case 0x0E://������֤ʱЧ����
            if( ClassIndex != 0 )
            {
                return 0x8000;
            }
			
			api_ReadPrePayPara( eIRTime, pBuf );
            pBuf += 4;
            
		    break;
		    
		case 0x0F://������֤ʱЧʣ��ʱ��
            if( ClassIndex != 0 )
            {
                return 0x8000;
            }
			
			if( CURR_COMM_TYPE( Ch ) == COMM_TYPE_TERMINAL )
			{
				ConnectValidTime.l = (APPConnect.TerminalAuthTime / 60);
				Value = (APPConnect.TerminalAuthTime%60);				
			}
			else
			{
				ConnectValidTime.l = (APPConnect.IRAuthTime / 60);
				Value = (APPConnect.IRAuthTime%60);
			}
            if( Value != 0 )//��������������1����
            {
                ConnectValidTime.l += 1;
            }
            
            memcpy( pBuf, ConnectValidTime.b, 4 );//��Ϊ��double long�������� �����е��� �ڼ�TAGʱ����е���

            pBuf += 4;
		    break;
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//��������: ��ESAM������F100����class21�������������Tag
//         
//����:
//         	*pOAD[in]��		OAD
//			Sch[in]��		OtherObj�е��¼�����         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD AddTagOtherClass21Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1F);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( ClassAttribute )
	{
		case 0x02: //��ȡESAM���к�
		case 0x03: //��ȡESAM�汾��
		case 0x04: //��ȡ�Գ���Կ�汾
		case 17:  //���ؼ���ָ��ʹ��Ȩ��
		case 18:  //�ն˵�ַ
		case 0x05: //��ȡ�ỰʱЧ����
		case 0x06: //��ȡ�Ựʣ��ʱ��
		case 0x0E: //��ȡ������֤ʱЧ
		case 0x0F: //��ȡ������֤ʱЧʣ��ʱ��
		case 19:  //��ȡ�ն˹㲥������
		case 20:  //��ȡ�ն�����Ự������
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( ClassAttribute == 2 )//esam���к�
			{
				Tag = T_OctetString8;
			}
			else if( ClassAttribute == 3 )//esam�汾��
			{
				Tag = T_OctetString5;
			}
			else if( ClassAttribute == 4 )//�Գ���Կ�汾
			{
				Tag = T_OctetString16;
			}
			else if( ClassAttribute == 17 )//���ؼ���ָ��ʹ��Ȩ��
			{
				Tag = T_Unsigned;
			}
			else if( ClassAttribute == 18 )//�ն˵�ַ
			{
				Tag = T_OctetString8;
			}
			else//�ỰʱЧ���ޡ��Ựʣ��ʱ�䡢������֤ʱЧ��������֤ʱЧʣ��ʱ�䡢�ն˹㲥���������ն�����Ự������
			{
				Tag = T_UNDoubleLong;
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
			
		case 0x07: //��ȡ������
		case 21: //��ȡ�ն˻Ự����
			if( ClassAttribute == 0x07 )
			{
				if( ClassIndex > T_NowCount[1] )
				{
					return 0x8000;
				}
				Tag = T_NowCount;
			}
			else
			{
				if( ClassIndex > T_TerminalLimit[1] )
				{
					return 0x8000;
				}
				Tag = T_TerminalLimit;
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
			break;

		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//��������: ��ȡESAM������F100����class21�����ݳ���
//         
//����:     
//          DataType[in]��eData/eTagData
//          *pOAD[in]     OAD    
//         
//����ֵ:   �������ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD GetRequestOtherClass21Len( BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1F);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( ClassAttribute )
	{
		case 0x02: //��ȡESAM���к�
		case 0x03: //��ȡESAM�汾��
		case 0x04: //��ȡ�Գ���Կ�汾
		case 17:  //���ؼ���ָ��ʹ��Ȩ��
		case 18:  //�ն˵�ַ
		case 0x05: //��ȡ�ỰʱЧ����
		case 0x06: //��ȡ�Ựʣ��ʱ��
		case 0x0E: //��ȡ������֤ʱЧ
		case 0x0F: //��ȡ������֤ʱЧʣ��ʱ��
		case 19:  //��ȡ�ն˹㲥������
		case 20:  //��ȡ�ն�����Ự������
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( ClassAttribute == 2 )//esam���к�
			{
				Tag = T_OctetString8;
			}
			else if( ClassAttribute == 3 )//esam�汾��
			{
				Tag = T_OctetString5;
			}
			else if( ClassAttribute == 4 )//�Գ���Կ�汾
			{
				Tag = T_OctetString16;
			}
			else if( ClassAttribute == 17 )//���ؼ���ָ��ʹ��Ȩ��
			{
				Tag = T_Unsigned;
			}
			else if( ClassAttribute == 18 )//�ն˵�ַ
			{
				Tag = T_OctetString8;
			}
			else//�ỰʱЧ���ޡ��Ựʣ��ʱ�䡢������֤ʱЧ��������֤ʱЧʣ��ʱ�䡢�ն˹㲥���������ն�����Ự������
			{
				Tag = T_UNDoubleLong;
			}

			Lenth = GetTypeLen( DataType, Tag );
			
			break;
			
		case 0x07: //��ȡ������
		case 21: //��ȡ�ն˻Ự����
			if( ClassAttribute == 0x07 )
			{
				if( ClassIndex > T_NowCount[1] )
				{
					return 0x8000;
				}
				Tag = T_NowCount;
			}
			else
			{
				if( ClassIndex > T_TerminalLimit[1] )
				{
					return 0x8000;
				}
				Tag = T_TerminalLimit;
			}
			
			Tag +=GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( DataType, Tag );
			break;
		
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//��������: ��ȡ��������豸��class22���������ݣ�����Tag��
//         
//����:
//			Dot[in]��	С���� ��δ�õ���    
//          *pOAD[in]��	OAD         
//          Sch[in]��   OtherObj�е��¼�����      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD GetOtherClass22Data( BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num,Flag,TempCommPara;
	TTwoByteUnion OI;	
	WORD Result;
	BYTE *pBufBak = pBuf;
//	T_ble_para pPara_inf;
	// TSafeMem_dev_work dev_work;

	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Flag = 0;
	
	switch( ClassAttribute )
	{
		case 0x02: //�豸�����б�
			switch( OI.w )
			{
				case 0xF201://RS485
					if( ClassIndex > 2 )
					{
						return 0x8000;
					}

					for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
					{
						if((SerialMap[i].SerialType==eRS485_I) /*|| (SerialMap[i].SerialType == eRS485_II)*/ )
						{
							Flag++;
						}
					}
					if( Flag == 0 )
					{
						return 0x8000;
					}
					
					TagPara.Array_Struct_Num = Flag;
					#if( MAX_PHASE != 1)
					TagPara.Lenth.w = (sizeof(PortDescript4)-1);
					#else
                    TagPara.Lenth.w = (sizeof(PortDescript4)-1);
					#endif
					i = (ClassIndex == 0) ? 0 : (ClassIndex-1);
					Num = (ClassIndex == 0) ? Flag : 1;
					Num += i;

					for( ; i < Num; i++ )
					{
						if( i == 0 )
						{
						    #if( MAX_PHASE != 1)
							memcpy(pBuf,PortDescript4,(sizeof(PortDescript4)-1));//�˿�������
							pBuf += (sizeof(PortDescript4)-1);
							#else
							memcpy(pBuf,PortDescript4,(sizeof(PortDescript4)-1));//�˿�������
							pBuf += (sizeof(PortDescript4)-1);
							#endif
							TempCommPara = FPara2->CommPara.I485;//�˿ڲ���
						}
						else
						{
							memcpy(pBuf,PortDescript3,(sizeof(PortDescript3)-1));//�˿�������
							pBuf += (sizeof(PortDescript3)-1);
							TempCommPara = FPara2->CommPara.II485;//�˿ڲ���
						}
						
						*(pBuf++) = (TempCommPara & 0x0F);//������
						*(pBuf++) = ((TempCommPara>>5) & 0x03);//У��λ
						*(pBuf++) = 8;//����λ
						*(pBuf++) = (((TempCommPara>>4) & 0x01)+1);//ֹͣλ
						*(pBuf++) = 0;//����
						*(pBuf++) = 0;//�˿ڹ���
					}
					break;	
				case 0xF202://IR
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					TagPara.Lenth.w = (sizeof(PortDescript1)-1);
					memcpy(pBuf,PortDescript1,(sizeof(PortDescript1)-1));//�˿�������
					pBuf += (sizeof(PortDescript1)-1);

					//����ȫ��Ϊ�̶�ֵ������ҪȡComPara������ֱ������
					*(pBuf++) = 2;//������: 1200bps
					*(pBuf++) = 2;//У��λ: żУ��
					*(pBuf++) = 8;//����λ: 8λ����λ
					*(pBuf++) = 1;//ֹͣλ: 1λֹͣλ
					*(pBuf++) = 0;//����:   ��
					break;
				#if(SEL_EVENT_DI_CHANGE == YES)	
				case 0xF203:
					*(pBuf++) = api_ReadDIStatus();//ң��״̬
					*(pBuf++) = api_ReadDIChange();//ң�ű�λ״̬					
					break;
				#endif
				case 0xF205://�̵������
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					TagPara.Lenth.w = (sizeof(Relayscript)-1);
					memcpy(pBuf,Relayscript,(sizeof(Relayscript)-1));//�˿�������
					pBuf += (sizeof(Relayscript)-1);
					//��ǰ״̬  enum{δ�����0���������1��}
					if( api_GetRelayStatus(1) == 0 )
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 1;
					}

					Result = ReadRelayPara(3,pBuf);//��������  enum{����ʽ��0��������ʽ��1��}��
					if(Result == FALSE)
					{
						return 0x8000;
					}
					pBuf += 1;

					*(pBuf++) = 0;//����״̬  enum{���루0����δ���루1) }
					break;
				case 0xF207://�๦�ܶ���
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					*(pBuf++)= GlobalVariable.g_byMultiFunPortType;
					break;
				case 0xF209://�ز���΢�������߽ӿ�
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}

					for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
					{
						if( SerialMap[i].SerialType == eCR)
						{
							Flag++;
						}
					}
					
					if(Flag != 1)
					{
						return 0x8000;
					}
					memcpy(pBuf,PortDescript2,(sizeof(PortDescript2)-1));//�˿�������
					pBuf += (sizeof(PortDescript2)-1);
					
					TagPara.Lenth.w = sizeof(PortDescript2);
					//*(pBuf++) = 0x00;

					TempCommPara = FPara2->CommPara.ComModule;//�˿ڲ���
					*(pBuf++) = (TempCommPara & 0x0F);//������
					*(pBuf++) = ((TempCommPara>>5) & 0x03);//У��λ
					*(pBuf++) = 8;//����λ
					*(pBuf++) = (((TempCommPara>>4) & 0x01)+1);//ֹͣλ
					*(pBuf++) = 0;//����

					*(pBuf++) = NULL;//�汾��ϢΪ��
					break;
//				case 0xF20B:	//���� ����2 ����ģ��
//					i = api_GetBlueToothModule698(pBuf);
//					pBuf += i;
				case 0xF221:	//CAN���� ����2
					if( ClassIndex > 2 )
					{
						return 0x8000;
					}
					TagPara.Lenth.w = (sizeof(PortDescript5)-1);
					memcpy(pBuf,PortDescript5,(sizeof(PortDescript5)-1));//�˿�������
					pBuf += (sizeof(PortDescript5)-1);
					TempCommPara = FPara2->CommPara.CanBaud;//�˿ڲ���
					*(pBuf++) = TempCommPara ;//������	
					*(pBuf++) = 0;//У��λ
					*(pBuf++) = 0;//����λ
					*(pBuf++) = 0;//ֹͣλ
					*(pBuf++) = 0;//����	
					break;
				default:
					return 0x8000;
			}
			break;

		case 3:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( OI.w == 0xf201 )
			{
				for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
				{
					if((SerialMap[i].SerialType==eRS485_I) /*|| (SerialMap[i].SerialType == eRS485_II)*/ )
					{
						Flag++;
					}
				}

				*(pBuf++) = Flag;
			}
			else if( OI.w == 0xf202 )
			{
				*(pBuf++) = 1;
			}
			else if( OI.w == 0xf205 )
			{
				*(pBuf++) = 1;
			}
			else if( OI.w == 0xf207 )
			{
				*(pBuf++) = 1;
			}
			else
			{
				for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
				{
					if( SerialMap[i].SerialType == eCR)
					{
						Flag++;
					}
				}
				*(pBuf++) = Flag;
			}
			break;
		case 4:
			// if(OI.w == 0xf20B)	//�������豸
			// {
			// 	api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work);
			// 	memcpy(pBuf,dev_work.tDownMessage,sizeof(dev_work.tDownMessage));
			// 	pBuf+=sizeof(dev_work.tDownMessage);
			// }
			// else 
			if(OI.w == 0xf203)	//����������
			{
				api_ReadDIPara(1,pBuf);
				api_ReadDIPara(2,pBuf+1);
				break;
			}
			else
			{
				return 0x8000;
			}
			break;
		case 5:
//			if(OI.w == 0xf20B)	//����������Ϣ
//			{
//				*(pBuf++) = 0xF2;//OAD
//				*(pBuf++) = 0x0B;
//				*(pBuf++) = 0x02;
//				*(pBuf++) = 0x01;
//				api_mt_get_ble_para_info(&pPara_inf);
//				for ( i = 0; i < BLE_COM_NUM; i++)
//				{
//					if (pPara_inf.connect_info.status&(BIT0<<i))
//					{
//						if (i<2)
//						{
//							*(pBuf++) = 'N';
//							*(pBuf++) = 'U';
//							*(pBuf++) = 'L';
//							*(pBuf++) = 'L';
//							pBuf+=5;
//						}
//						else
//						{
//							*(pBuf++) = 'B';
//							*(pBuf++) = 'T';
//							*(pBuf++) = '_';
//							*(pBuf++) = 'M';
//							*(pBuf++) = 'E';
//							*(pBuf++) = 'T';
//							*(pBuf++) = 'E';
//							*(pBuf++) = 'R';
//							pBuf++;
//						}
//						memcpy(pBuf,pPara_inf.connect_info.dev[i],6);
//						pBuf+=15;
//					}
//				}
//				
//			}
//			else
			{
				return 0x8000;
			}
			break;
		case 6:
			if (OI.w==0xF221)
			{
				*(pBuf++)=CanBusPara.bSelfNode;
			}
			// else if(OI.w == 0xf20B)	//����������Ϣ
			// {
			// 	*(pBuf++) = 0xF2;//OAD
			// 	*(pBuf++) = 0x0B;
			// 	*(pBuf++) = 0x02;
			// 	*(pBuf++) = 0x01;
			// 	api_mt_get_ble_para_info(&pPara_inf);
			// 	lib_ExchangeData(pPara_inf.work_para.adv_interval,pPara_inf.work_para.adv_interval,2);
			// 	lib_ExchangeData(pPara_inf.work_para.scan_interval,pPara_inf.work_para.scan_interval,2);
			// 	memcpy(pBuf,&pPara_inf.work_para,sizeof(pPara_inf.work_para));
			// 	pBuf+=sizeof(pPara_inf.work_para);

			// }
			else
			{
				return 0x8000;
			}
			break;
		case 7:
			if (OI.w==0xF221)
			{
				for ( i = 0; i < CanBusPara.bRxNum; i++)
				{
					*(pBuf++)=CanBusPara.bRxNode[i];
				}
				
			}
			// else if(OI.w == 0xf20B)	//���� �����б�
			// {
			// 	for (BYTE ii = 0; ii < 100; ii++)
			// 	{
			// 		*(pBuf++) = ii;
			// 	}
			// }
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
//��������:  ��������TAG
//
//����: pTag[in]: TAG��ַ�ĵ�ַ
//		InBuf[in]: ��Ҫ��TAG�����ݵĵ�ַ �ĵ�ַ
//		OutBuf[in]: �������
//		OutLen[in]: �������ĳ���
//����ֵ: ����TAG�ĳ���
//
//��ע: �ݹ����
//--------------------------------------------------
// static BYTE s_AddTagBluethCount = 0;
// static WORD AddTagBlueth1(const BYTE **pTag, BYTE **InBuf, BYTE *OutBuf, WORD OutLen)
// {
// 	const BYTE *Tag;
// 	BYTE i, Num;
// 	WORD Lenth = 0, Offset = 0;

// 	s_AddTagBluethCount++;
// 	if(s_AddTagBluethCount > 100)
// 	{
// 		return 0x8000;
// 	}

// 	Tag = pTag[0];
// 	if(Tag[0] == Structure_698)
// 	{
// 		OutBuf[Lenth++] = Structure_698;
// 		OutBuf[Lenth++] = Tag[1];
// 		Num = Tag[1];
// 		Tag += 2;

// 		for (i = 0; i < Num; i++)
// 		{
// 			Lenth += AddTagBlueth1(&Tag, InBuf, &OutBuf[Lenth], OutLen - Lenth);
// 			if(Lenth >= 0x8000)
// 			{
// 				return 0x8000;
// 			}
// 		}
// 	}
// 	else if(Tag[0] == Array_698)
// 	{
// 		return 0x8000;
// 	}
// 	else 
// 	{
// 		Lenth = GetTypeLen( eTagData, Tag );
// 		if( Lenth == 0x8000 )
// 		{
// 			return 0x8000;
// 		}
		
// 		if( OutLen < Lenth )
// 		{
// 			return 0;
// 		}

// 		Lenth = GetRequestAddTag( Tag, InBuf[0], OutBuf );
// 		if( Lenth == 0x8000 )
// 		{
// 			return 0x8000;
// 		}

// 		extern TTagBuf TagBuf;
// 		InBuf[0] += TagBuf.InBufLenth;

// 		Tag += 2;
// 	}
// 	pTag[0] = Tag;

// 	return Lenth;
// }

// //--------------------------------------------------
// //��������:  ��������TAG
// //
// //����: pTag[in]: TAG��ַ
// //		InBuf[in]: ��Ҫ��TAG�����ݵĵ�ַ �ĵ�ַ
// //		OutBuf[in]: �������
// //		OutLen[in]: �������ĳ���
// //����ֵ: ����TAG�ĳ���
// //
// //��ע: �ݹ����
// //--------------------------------------------------
// static WORD AddTagBlueth(const BYTE *Tag, BYTE **InBuf, BYTE *OutBuf, WORD OutLen)
// {
// 	s_AddTagBluethCount = 0;
// 	return AddTagBlueth1(&Tag, InBuf, OutBuf, OutLen);
// }

//--------------------------------------------------
//��������: ����������豸��class22�������������Tag
//         
//����:
//         	*pOAD[in]��		OAD
//			Sch[in]��		OtherObj�е��¼�����         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD AddTagOtherClass22Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex,i,Num,Flag,End,Start;
	WORD Result,Lenth;
	const BYTE *Tag;
//	T_ble_para pPara_inf;

	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	Flag = 0;
	
	switch( ClassAttribute )
	{
		case 0x02: //�豸�����б�
			switch( OI.w )
			{
				case 0xF201://RS485
				case 0xF202://IR
				case 0xF203://ң��
				case 0xF205://�̵������
				case 0xF207://�๦�ܶ���
                case 0xF221://CAN
					if( OI.w == 0xf201 )
					{
						
                        for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
                        {
                            if((SerialMap[i].SerialType==eRS485_I) /*|| (SerialMap[i].SerialType == eRS485_II)*/ )
                            {
                                Flag++;
                            }
                        }
                        
                        if( Flag == 0 )
                        {
                            return 0x8000;
                        }

                        TagPara.Array_Struct_Num = Flag;
                        #if( MAX_PHASE != 1)
                        TagPara.Lenth.w = (sizeof(PortDescript4)-1);
                        #else
                        TagPara.Lenth.w = (sizeof(PortDescript4)-1);
                        #endif
                        
						if( ClassIndex > Flag )
						{
							return 0x8000;
						}

						Tag = T_RS485;
					}
					else if( OI.w == 0xf202 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
						TagPara.Lenth.w = (sizeof(PortDescript1)-1);
						
						Tag = T_IR;
					}
					else if( OI.w == 0xF203 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						Tag = T_SwitchIn_2;	
					}	
					else if( OI.w == 0xf205 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
                        TagPara.Lenth.w = (sizeof(Relayscript)-1);

						Tag = T_RelayOut;
					}
					else if( OI.w == 0xF221 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						TagPara.Lenth.w = (sizeof(PortDescript5)-1);
						
						Tag = T_CAN;
					}
					else
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}

						Tag = T_MultiFunPort;
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
					
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}		
					break;	

				case 0xF209://�ز���΢�������߽ӿ�
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					TagPara.Array_Struct_Num = 1;
					TagPara.Lenth.w = ( sizeof(PortDescript2) - 1 );
					
					Tag = T_CR;
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
				// case 0xF20B:		//��������2 ����ģ��
				// 	if(ClassIndex != 0)
				// 	{
				// 		Lenth = AddTagBlueth(T_BluetoothModule, &InBuf, OutBuf, OutBufLen);
				// 	}
				// 	else
				// 	{
				// 		Num = 0x01;	//����ģ��ĸ���
				// 		OutBuf[Lenth++]= Array_698;
				// 		OutBuf[Lenth++] = Num;	//����ģ��ĸ���
				// 		for (i = 0; i < Num; i++)
				// 		{
				// 			Lenth += AddTagBlueth(T_BluetoothModule, &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
				// 			if(Lenth >= 0x8000 )
				// 			{
				// 				return 0x8000;
				// 			}
				// 		}
				// 	}
				// 	break;
				default:
					return 0x8000;
			}
			break;	

		case 3:
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			Tag = T_Unsigned;
			Lenth = GetTypeLen( eTagData, Tag );
			if( Lenth == 0x8000 )
			{
                return 0x8000;
			}
			
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}		
			break;
		case 4:
			// if(OI.w == 0xF20B)		//��������4 ���豸�б�
			// {
			// 	if(ClassIndex != 0)
			// 	{
			// 		Lenth = AddTagBlueth(T_BluetoothDevice, &InBuf, OutBuf, OutBufLen);
			// 	}
			// 	else
			// 	{
			// 		Num = 3;	//����ģ��ĸ���
			// 		OutBuf[Lenth++]= Array_698;
			// 		OutBuf[Lenth++] = Num;	//����ģ��ĸ���
			// 		for (i = 0; i < Num; i++)
			// 		{
			// 			Lenth += AddTagBlueth(T_BluetoothDevice, &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
			// 			if(Lenth >= 0x8000 )
			// 			{
			// 				return 0x8000;
			// 			}
			// 		}
			// 	}
			// }
			// else 
			if(OI.w == 0xF203)
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				else 
				{
					Tag = T_SwitchIn_4;	
					Lenth = GetTypeLen( eTagData, Tag );
					if( Lenth == 0x8000 )
					{
                		return 0x8000;
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
				return 0x8000;
			}
			break;
		case 5:
//			if(OI.w == 0xF20B)		//��������5, ������Ϣ ֻ��
//			{
//				const BYTE T_ConnectInfo[]=
//				{
//					OAD_698,		4,	//�˿ں�
//				};
//
//				if(ClassIndex == 0)
//				{
//					i = 0;
//					Num = 0x01;	//����ģ��ĸ���
//					OutBuf[Lenth++]= Array_698;
//					OutBuf[Lenth++] = Num;	//����ģ��ĸ���
//				}
//				else
//				{
//					i = 0;			//��ʼ���ص�������Ϣ����
//					Num = i + 1;	//�������ص�������Ϣ����
//				}
//				api_mt_get_ble_para_info(&pPara_inf);
//				for (i = i; i < Num; i++)
//				{
//					OutBuf[Lenth++] = Structure_698;
//					OutBuf[Lenth++] = 3;
//
//					//�˿ں� OAD
//					Lenth += AddTagBlueth(&T_ConnectInfo[0], &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
//					if(Lenth >= 0x8000)
//					{
//						return 0x8000;
//					}
//
//					End = 0;
//					for ( Start = 0; Start < 2; Start++)
//					{
//						if (pPara_inf.connect_info.status&(BIT0<<Start))
//						{
//							End++;
//						}
//						
//					}
//					//���ӵ����豸 BluetoothInfo 
//					OutBuf[Lenth++] = Array_698;
//					OutBuf[Lenth++] = End;
//					for (Start = 0; Start < End; Start++)	
//					{
//						Lenth += AddTagBlueth(&T_BluetoothModule[4], &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
//						if(Lenth >= 0x8000)
//						{
//							return 0x8000;
//						}
//					}
//
//					End = 0;
//					for ( Start = 2; Start < 5; Start++)
//					{
//						if (pPara_inf.connect_info.status&(BIT0<<Start))
//						{
//							End++;
//						}
//						
//					}
//					//���ӵĴ��豸 BluetoothInfo
//					OutBuf[Lenth++] = Array_698;
//					OutBuf[Lenth++] = End;
//					for (Start = 0; Start < End; Start++)	
//					{
//						Lenth += AddTagBlueth(&T_BluetoothModule[4], &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
//						if(Lenth >= 0x8000)
//						{
//							return 0x8000;
//						}
//					}
//				}
//			}
//			else
			{
				return 0x8000;
			}
			break;
		case 6:
			if (OI.w==0xF221)
			{
				*(OutBuf++)=Unsigned_698;
				*(OutBuf++)=*(InBuf);
				Lenth=2;
			}
			// else if(OI.w == 0xF20B)	//��������6�� ��������
			// {
			// 	if(ClassIndex != 0)
			// 	{
			// 		Lenth = AddTagBlueth(T_BluetoothWork, &InBuf, OutBuf, OutBufLen);
			// 	}
			// 	else
			// 	{
			// 		Num = 0x01;	//����ģ��ĸ���
			// 		OutBuf[Lenth++]= Array_698;
			// 		OutBuf[Lenth++] = Num;	//����ģ��ĸ���
			// 		for (i = 0; i < Num; i++)
			// 		{
			// 			Lenth += AddTagBlueth(T_BluetoothWork, &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
			// 			if(Lenth >= 0x8000 )
			// 			{
			// 				return 0x8000;
			// 			}
			// 		}
			// 	}
			// }
			else
			{
				return 0x8000;
			}
			break;
		case 7:
		if (OI.w==0xF221)
		{
			if(ClassIndex != 0)
			{
				if(ClassIndex > CanBusPara.bRxNum)
				{
					*(OutBuf++) = NULL_698;
					Lenth = 1;
				}
				else
				{
					*(OutBuf++) = Unsigned_698;
					*(OutBuf++) = *(InBuf + ClassIndex - 1);
					Lenth = 2;
				}
			}
			else
			{
				*(OutBuf++) = Array_698;
				*(OutBuf++) = CanBusPara.bRxNum;
				for(i = 0; i < CanBusPara.bRxNum; i++)
				{
					*(OutBuf++) = Unsigned_698;
					*(OutBuf++) = *(InBuf++);
				}
				Lenth = 2 * (CanBusPara.bRxNum + 1);
			}
		}
		// else if(OI.w == 0xF20B)		//��������7 �����б�
		// {
		// 	if(ClassIndex != 0)
		// 	{
		// 		Lenth = AddTagBlueth(T_BluetoothDevice, &InBuf, OutBuf, OutBufLen);
		// 	}
		// 	else
		// 	{
		// 		Num = 0x01;	//����ģ��ĸ���
		// 		OutBuf[Lenth++]= Array_698;
		// 		OutBuf[Lenth++] = Num;	//����ģ��ĸ���
		// 		for (i = 0; i < Num; i++)
		// 		{
		// 			Lenth += AddTagBlueth(T_BluetoothDevice, &InBuf, &OutBuf[Lenth], OutBufLen - Lenth);
		// 			if(Lenth >= 0x8000 )
		// 			{
		// 				return 0x8000;
		// 			}
		// 		}
		// 	}
		// }
		else
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
//��������: ��ȡ��������豸��class22�����ݳ���
//         
//����:     
//          DataType[in]��eData/eTagData
//          *pOAD[in]     OAD    
//         
//����ֵ:   �������ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD GetRequestOtherClass22Len( BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,ClassIndex,i,Flag;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
    Flag = 0;

	switch( ClassAttribute )
	{
		case 0x02: //�豸�����б�
			switch( OI.w )
			{
				case 0xF201://RS485
				case 0xF202://IR
				//case 0xF203://����������
				case 0xF205://�̵������
				case 0xF207://�๦�ܶ���
				//case 0xF20B://����
					if( OI.w == 0xf201 )
					{
						if( ClassIndex > 2 )
						{
							return 0x8000;
						}
						
                        for(i=0;i<MAX_COM_CHANNEL_NUM;i++)
                        {
                            if((SerialMap[i].SerialType==eRS485_I) /*|| (SerialMap[i].SerialType == eRS485_II)*/ )
                            {
                                Flag++;
                            }
                        }

                        if( Flag == 0 )
                        {
                            return 0x8000;
                        }

                        TagPara.Array_Struct_Num = Flag;
                        #if( MAX_PHASE != 1)
                        TagPara.Lenth.w = (sizeof(PortDescript4)-1);
                        #else
                        TagPara.Lenth.w = (sizeof(PortDescript4)-1);
                        #endif

						Tag = T_RS485;
					}
					else if( OI.w == 0xf202 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
						TagPara.Lenth.w = (sizeof(PortDescript1)-1);
						
						Tag = T_IR;
					}
					else if( OI.w == 0xf205 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
                        TagPara.Lenth.w = (sizeof(Relayscript)-1);

						Tag = T_RelayOut;
					}
					else if( OI.w == 0xf221 )
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}
						
                        TagPara.Lenth.w = (sizeof(PortDescript5)-1);

						Tag = T_CAN;
					}
					else
					{
						if( ClassIndex > 1 )
						{
							return 0x8000;
						}

						Tag = T_MultiFunPort;
					}

					Tag +=GetTagOffsetAddr( ClassIndex, Tag);
					Lenth = GetTypeLen( DataType, Tag );
					break;	
				case 0xF209://�ز���΢�������߽ӿ�
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}

					// if( ClassIndex == 0 )
					// {
					// 	Lenth = (DataType == eData) ? ( 1+5+1) : ( 14 );
					// }
					// else
					// {
					// 	Lenth = (DataType == eData) ? ( 1+5+1) : ( 12 );
					// }
					TagPara.Array_Struct_Num = 1;
					TagPara.Lenth.w = (sizeof(PortDescript2)-1);
					Tag = T_CR;
					Tag +=GetTagOffsetAddr( ClassIndex, Tag);
					Lenth = GetTypeLen( DataType, Tag );
					break;
				
				default:
					return 0x8000;
			}
			break;
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//��������: ��ȡ��ʾ�ӿ��ࣨclass17���������ݣ�����Tag��
//         
//����:
//			Dot[in]��	С���� ��δ�õ���    
//          *pOAD[in]��	OAD         
//          Sch[in]��   OtherObj�е��¼�����      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD GetOtherClass17Data( BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num;
	TTwoByteUnion OI;	
	WORD Type,Result;
	BYTE *pBufBak = pBuf;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	
	switch( ClassAttribute )
	{
		case 0x02: //��ʾ�����б�		
			Type = (OI.w - 0xf300);

			if( ClassIndex > FPara1->LCDPara.DispItemNum[Type] )
			{
				return 0x8000;
			}
			
			i = (ClassIndex==0) ? 0 : (ClassIndex-1);
			Num = (ClassIndex==0) ? FPara1->LCDPara.DispItemNum[Type] : 1;
			Num += i;
			
			for( ; i<Num; i++ )
			{
				Result = api_ReadLcdItem( Type,i,pBuf);
				if(Result == FALSE)
				{
					return 0x8000;
				}

				pBuf += 10;
			}
			break;

		case 0x03://ÿ��������ʾʱ��
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( OI.w == 0xf300 )
			{
				memcpy( pBuf, (BYTE*)&FPara1->LCDPara.LoopDisplayTime, 2 );
				pBuf += 2;
			}
			else
			{
				memcpy( pBuf, (BYTE*)&FPara1->LCDPara.KeyDisplayTime, 2 );
				pBuf += 2;
			}		
			break;

		case 0x04://��ʾ����
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}

			if( (ClassIndex==0) || (ClassIndex==1) )
			{
				if( OI.w == 0xf300 )
				{
					*(pBuf++) = FPara1->LCDPara.DispItemNum[0];
				}
				else
				{
					*(pBuf++) = FPara1->LCDPara.DispItemNum[1];
				}
			}

			if( (ClassIndex==0) || (ClassIndex==2) )
			{
				if( OI.w == 0xf300 )
				{
					*(pBuf++) = MAX_LCD_LOOP_ITEM;
				}
				else
				{
					*(pBuf++) =	MAX_LCD_KEY_ITEM;
				}
			}
			break;
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//��������: ��ȡ�ļ��ӿ��ࣨclass18���������ݣ�����Tag��
//         
//����:
//			Ch[in]��	ͨ��   
//          *pOAD[in]��	OAD         
//          Sch[in]��   OtherObj�е��¼�����      
//          *pBuf[out]���������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD GetOtherClass18Data( BYTE Ch, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	BYTE ClassAttribute,ClassIndex;
	TTwoByteUnion OI;	
	WORD Len = 0,wTemp;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	
	if(OI.w != 0xF001)
	{
		return 0x8000;
	}

	switch( ClassAttribute )
	{
		case 0x04: //�����״̬�� bit_string
			if (tIapInfo.dwFlag != IAP_START)
			{
				return 0x8000;
			}
			Len = ((tIap.wAllFrameNo-1)/8)+1;
			memcpy(pBuf, IapStatus, Len);
			lib_InverseData(pBuf, Len);
			break;
		case 0x05: //���ط��ı�ʶ octet-string
	
			break;
		case 0x06: //֧�ִ�����С long unsigned

			break;
		case 0x07: //ִ������ʱ�� date time s

			break;
		default:
			return 0x8000;
	}

	return Len;
}

//--------------------------------------------------
//��������: ����ʾ�ӿ��ࣨclass17�������������Tag
//         
//����:
//         	*pOAD[in]��		OAD
//			Sch[in]��		OtherObj�е��¼�����         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD AddTagOtherClass17Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num,LenOffest;
	TTwoByteUnion OI,Len;
	WORD Result,Lenth,Type1;
	BYTE *pBufBak = OutBuf;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 0x02: //��ʾ�����б�		
			Type1 = (OI.w - 0xf300);
			
			if( ClassIndex > FPara1->LCDPara.DispItemNum[Type1] )
			{
				return 0x8000;
			}
	
			if( ClassIndex == 0 )
			{
				
				
				*(OutBuf++) = Array_698;
				Len.w = FPara1->LCDPara.DispItemNum[Type1];
				LenOffest = Deal_698DataLenth( OutBuf, Len.b, eUNION_TYPE ); 
				OutBuf += LenOffest;
			}
			else
			{
				if( OutBufLen < 15 )
				{
					return 0;
				}
			}
			
			Num = (ClassIndex==0) ? FPara1->LCDPara.DispItemNum[Type1] : 1;
			
			for( i=0; i<Num; i++ )
			{
				if((OutBufLen-(WORD)(OutBuf-pBufBak))<15)
				{
					return 0;
				}
				*(OutBuf++) = Structure_698;
				*(OutBuf++) = 2;
				if( InBuf[10*i+8] == 1 )
				{
					*(OutBuf++) = CSD_698;
					*(OutBuf++) = 0;
					memcpy( OutBuf, InBuf+(10*i), 4 );
					OutBuf += 4;
				}
				else
				{
					*(OutBuf++) = CSD_698;
					*(OutBuf++) = 1;
					memcpy( OutBuf, InBuf+(10*i), 4 );
					OutBuf += 4;
					*(OutBuf++) = 1;
					memcpy( OutBuf, InBuf+(10*i+4), 4 );
					OutBuf += 4;
				}

				*(OutBuf++) = Unsigned_698;
				*(OutBuf++) = InBuf[10*i+9];
			}
			
			Lenth = (WORD)(OutBuf-pBufBak);			
			break;

		case 0x03://ÿ��������ʾʱ��
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
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
			break;

		case 0x04://��ʾ����
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}
			
			Tag = T_Display;
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
			break;
		
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//��������: ����ʾ�ӿ��ࣨclass17�������������Tag
//         
//����:
//         	*pOAD[in]��		OAD
//			Sch[in]��		OtherObj�е��¼�����         
//          *InBuf[in]��	��Ҫ���tag������         
//          OutBufLen[in]��	����Ļ��峤��         
//          *OutBuf[in]��	�������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD AddTagOtherClass18Data(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute, ClassIndex;
	TTwoByteUnion OI;
	WORD Result, Lenth;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 0x04: //�����״̬�� bit_string
			Tag = T_TransfeStatus;
//			TagPara.Lenth.w = (((tIap.wAllFrameNo-1)/8)+1)*8;
            TagPara.Lenth.w = tIap.wAllFrameNo;
			break;
		case 0x05: //���ط��ı�ʶ octet-string
			TagPara.Lenth.w = 0x10;	//octet-string�ĳ���

			Tag = T_DownloadID;
			break;
		case 0x06: //֧�ִ�����С long unsigned
			Tag = T_BlockSize;
			break;
		case 0x07: //ִ������ʱ�� date time s	
			Tag = T_UpdatTime;
			break;
		default:
			return 0x8000;
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

	return Lenth;
}

//--------------------------------------------------
//��������: ��ȡ��ʾ�ӿ��ࣨclass17�����ݳ���
//         
//����:     
//          DataType[in]��eData/eTagData
//          *pOAD[in]     OAD    
//         
//����ֵ:   �������ݳ���
//         
//��ע����: 
//--------------------------------------------------
static WORD GetRequestOtherClass17Len( BYTE DataType, BYTE *pOAD)
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Lenth,Type1;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0;

	switch( ClassAttribute )
	{
		case 0x02: //��ʾ�����б�		
			Type1 = (OI.w - 0xf300);
			
			if( ClassIndex > FPara1->LCDPara.DispItemNum[Type1] )
			{
				return 0x8000;
			}
	
			if( ClassIndex == 0 )
			{
				Lenth = ( DataType == eData) ? (10*FPara1->LCDPara.DispItemNum[Type1]) : (15*FPara1->LCDPara.DispItemNum[Type1]+2);
			}
			else
			{
				Lenth = ( DataType == eData) ? 10 : 15;
			}				
			break;

		case 0x03://ÿ��������ʾʱ��
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}	
			
			Lenth = GetTypeLen( DataType, T_UNLong );	
			break;

		case 0x04://��ʾ����
			if( ClassIndex > 2 )
			{
				return 0x8000;
			}
			
			Tag = T_Display;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( DataType, Tag );
			break;
		
		default:
			return 0x8000;
	}	
	
	return Lenth;
}

//--------------------------------------------------
//��������: ��ȡ�����ӿ�����������
//         
//����:
//         	DataType[in]��	eData/eTagData/eAddTag
//         	Dot[in]:		С����        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: OtherObj��֧�ֵ�����
//--------------------------------------------------
WORD GetOtherData( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	BYTE Sch,ClassAttribute;
	BYTE Buf[MAX_OTHER_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1F);//bit0��bit4�����ʾ�������Ա�ţ�
	
	Sch = SearchOtherOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}

	if( ClassAttribute > OtherObj[Sch].MAXClassAttribute )
	{
		return 0x8000;
	}
	
	if( (DataType==eData) || (DataType==eTagData) )
	{
		if( OtherObj[Sch].Class == eclass8 )
		{
			Lenth = GetOtherClass8Data( Dot, pOAD, Sch, Buf );
		}
		#if(SEL_ESAM_TYPE != 0)
		else if( OtherObj[Sch].Class == eclass21 )
		{
			Lenth = GetOtherClass21Data( Ch, Dot, pOAD, Sch, Buf );
		}
		#endif
		else if( OtherObj[Sch].Class == eclass22 )
		{
			Lenth = GetOtherClass22Data( Dot, pOAD, Sch, Buf );
		}
		// else if( OtherObj[Sch].Class == eclass17 )
		// {
		// 	Lenth = GetOtherClass17Data( Dot, pOAD, Sch, Buf );
		// }
		else if( OtherObj[Sch].Class == eclass18 )
		{
			Lenth = GetOtherClass18Data( Ch, pOAD, Sch, Buf );
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
			if( OtherObj[Sch].Class == eclass8 )
			{
				Lenth = AddTagOtherClass8Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			}
			else if( OtherObj[Sch].Class == eclass21 )
			{
				Lenth = AddTagOtherClass21Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			}
			else if( OtherObj[Sch].Class == eclass22 )
			{
				Lenth = AddTagOtherClass22Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			}
			// else if( OtherObj[Sch].Class == eclass17 )
			// {
			// 	Lenth = AddTagOtherClass17Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			// }    	
			else if( OtherObj[Sch].Class == eclass18 )
			{
				Lenth = AddTagOtherClass18Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
			}    
			else
    	    {
                return 0x8000;
    	    }
		}
	}
	else if( DataType == eAddTag )//�����಻֧�ֵ�����TAG
	{
		if( InBuf == NULL )
		{
			return 0x8000;
		}
		
		if( OtherObj[Sch].Class == eclass8 )
		{
			Lenth = AddTagOtherClass8Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
		}
		else if( OtherObj[Sch].Class == eclass21 )
		{
			Lenth = AddTagOtherClass21Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
		}
		else if( OtherObj[Sch].Class == eclass22 )
		{
			Lenth = AddTagOtherClass22Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
		}
		// else if( OtherObj[Sch].Class == eclass17 )
		// {
		// 	Lenth = AddTagOtherClass17Data(pOAD, Sch, InBuf, OutBufLen, OutBuf);
		// }
		else if( OtherObj[Sch].Class == eclass18 )
		{
			Lenth = AddTagOtherClass18Data(pOAD, Sch, Buf, OutBufLen, OutBuf);
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

	return Lenth; 
}

//--------------------------------------------------
//��������: ��ȡ�����ӿ�������ݳ���
//         
//����:     
//          DataType[in]��eData/eTagData
//          *pOAD[in]     OAD    
//         
//����ֵ:   �������ݳ���
//         
//��ע����: 
//--------------------------------------------------
WORD GetOtherDataLen( BYTE DataType, BYTE *pOAD)
{
	TTwoByteUnion OI,Len;
	BYTE Sch;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	Sch = SearchOtherOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}
	
	if( OtherObj[Sch].Class == eclass8 )
	{
		Len.w = GetRequestOtherClass8Len( DataType, pOAD );
	}
	else if( OtherObj[Sch].Class == eclass21 )
	{
		Len.w = GetRequestOtherClass21Len( DataType, pOAD );
	}
	else if( OtherObj[Sch].Class == eclass22 )
	{
		Len.w = GetRequestOtherClass22Len( DataType, pOAD );
	}
	// else if( OtherObj[Sch].Class == eclass17 )
	// {
	// 	Len.w = GetRequestOtherClass17Len( DataType, pOAD );
	// }
	else
	{
		return 0x8000;
	}

	return Len.w;
}

#endif//#if ( SEL_DLT698_2016_FUNC == YES)

