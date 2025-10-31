//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������    	������
//��������		2017��9��20��09:16:13
//����			645��ȡ����
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "dlt645_2007.h"

#if( SEL_DLT645_2007 == YES )

//����ͨѶ��ȡ������˲ʱ��
#define PRO_PHASE_T				(0x01)		//˲ʱ����
#define PRO_PHASE_A				(0x02)		//˲ʱ��A
#define PRO_PHASE_B				(0x04)		//˲ʱ��B
#define PRO_PHASE_C				(0x08)		//˲ʱ��C
#define PRO_PHASE_N				(0x10)		//˲ʱ��N
#define PRO_PHASE_ABC			(PRO_PHASE_A+PRO_PHASE_B+PRO_PHASE_C)
#define PRO_PHASE_TABC			(PRO_PHASE_T+PRO_PHASE_A+PRO_PHASE_B+PRO_PHASE_C)

static const TMeteringVaribleTab ConstMeteringVaribleTab[] =
{
	//��ѹ
	{ 0x0101, REMOTE_U, PRO_PHASE_A, 2, 1 },
	#if( MAX_PHASE == 3 )
	{ 0x0102, REMOTE_U, PRO_PHASE_B, 2, 1 },
	{ 0x0103, REMOTE_U, PRO_PHASE_C, 2, 1 },
	#endif//#if(MAX_PHASE == 3)
	{ 0x01FF, REMOTE_U, PRO_PHASE_ABC, 2, 1 },

	//����
	{ 0x0201, REMOTE_I, PRO_PHASE_A, 3, 3 },
	#if( MAX_PHASE == 3 )
	{ 0x0202, REMOTE_I, PRO_PHASE_B, 3, 3 },
	{ 0x0203, REMOTE_I, PRO_PHASE_C, 3, 3 },
	#endif//#if(MAX_PHASE == 3)
	{ 0x02FF, REMOTE_I, PRO_PHASE_ABC, 3, 3 },

	//�й�����
	{ 0x0300, REMOTE_P, PRO_PHASE_T, 3, 4},
	{ 0x0301, REMOTE_P, PRO_PHASE_A, 3, 4},
	#if( MAX_PHASE == 3 )
	{ 0x0302, REMOTE_P, PRO_PHASE_B, 3, 4},
	{ 0x0303, REMOTE_P, PRO_PHASE_C, 3, 4},
	#endif//#if(MAX_PHASE == 3)
	{ 0x03FF, REMOTE_P, PRO_PHASE_TABC, 3, 4},

	//�޹�����
	{ 0x0400, REMOTE_Q, PRO_PHASE_T, 3, 4},
	{ 0x0401, REMOTE_Q, PRO_PHASE_A, 3, 4},
	#if( MAX_PHASE == 3 )
	{ 0x0402, REMOTE_Q, PRO_PHASE_B, 3, 4},
	{ 0x0403, REMOTE_Q, PRO_PHASE_C, 3, 4},
	#endif//#if(MAX_PHASE == 3)
	{ 0x04FF, REMOTE_Q, PRO_PHASE_TABC, 3, 4},

	//���ڹ���
	{ 0x0500, REMOTE_S, PRO_PHASE_T, 3, 4},
	{ 0x0501, REMOTE_S, PRO_PHASE_A, 3, 4},
	#if( MAX_PHASE == 3 )
	{ 0x0502, REMOTE_S, PRO_PHASE_B, 3, 4},
	{ 0x0503, REMOTE_S, PRO_PHASE_C, 3, 4},
	#endif//#if(MAX_PHASE == 3)
	{ 0x05FF, REMOTE_S, PRO_PHASE_TABC, 3, 4},

	//��������
	{ 0x0600, REMOTE_COS, PRO_PHASE_T, 2, 3},
	{ 0x0601, REMOTE_COS, PRO_PHASE_A, 2, 3},
	#if( MAX_PHASE == 3 )
	{ 0x0602, REMOTE_COS, PRO_PHASE_B, 2, 3},
	{ 0x0603, REMOTE_COS, PRO_PHASE_C, 2, 3},
	#endif//#if(MAX_PHASE == 3)
	{ 0x06FF, REMOTE_COS, PRO_PHASE_TABC, 2, 3},

	//���
	{ 0x0701, REMOTE_PHASE, PRO_PHASE_A, 2, 1},
	#if( MAX_PHASE == 3 )
	{ 0x0702, REMOTE_PHASE, PRO_PHASE_B, 2, 1},
	{ 0x0703, REMOTE_PHASE, PRO_PHASE_C, 2, 1},
	#endif//#if(MAX_PHASE == 3)
	{ 0x07FF, REMOTE_PHASE, PRO_PHASE_ABC, 2, 1},
};


//---------------------------------------------------------------
//��������: ��ȡ����
//
//����: 
//			Dot[in]		   С��λ��       
//          *DI[in]		   ���ݱ�ʶ                       
//          OutBufLen[in]  ��ȡ�ĳ���       
//          OutBuf[out]	   ��������                   
//����ֵ: 
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//
//��ע:   
//---------------------------------------------------------------
WORD Get645RequestVariable( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE i, Num, tDot;
	BYTE Buf[16];						//����������ĸ��ֽ� ABC�� �ڴ�����һ��
	WORD wResult, Lenth, ReadType[4];	//˲ʱ����ȡ ����ĸ�����
	TFourByteUnion DataID,Time;
	TFourByteUnion FourTemp;			//����4�ֽ����ݵ����⴦��
	TMeteringVaribleTab  MVaribleTab;
	BYTE Sel_HEX2BCD; 					//�Ƿ���ҪHEXתBCD
	BYTE Sel_EXData;					//�Ƿ���Ҫ����
	eTYPE_COVER_DATA CoverMode;			//HEX��BCD����ת����ʽ
	
	//Ĭ��BCDת�� ���赹��
	Sel_HEX2BCD = NO;
	Sel_EXData = NO;
	CoverMode = eBYTE_MODE;
	FourTemp.d = 0;
	
	Lenth = 0;

	//�������ݱ�ʶ
	memcpy( DataID.b, DI, 4 );
	wResult = 0;
	
	MVaribleTab.ID = DataID.b[1] + (DataID.b[2] << 8);
	
//��ȡ���������� ���ݱ�ʶ 02010100 ~ 0208FF00
//========================================================================================//	
	for( i = 0; i < (sizeof(ConstMeteringVaribleTab) / sizeof(TMeteringVaribleTab)); i++ )
	{
		if( MVaribleTab.ID == ConstMeteringVaribleTab[i].ID )
		{
			memcpy( (void *)&MVaribleTab.ID, (void *)&ConstMeteringVaribleTab[i].ID, sizeof(TMeteringVaribleTab) );
			break;
		}
	}	
    
    if( Dot != 0xFF)
	{
       tDot = Dot;
	}
	else
	{
       tDot = MVaribleTab.Dot;
	}
	
	if( i != (sizeof(ConstMeteringVaribleTab) / sizeof(TMeteringVaribleTab)) )
	{	
		Num = 0;
		for( i = 0; i < 4; i++ )
		{
			if( MVaribleTab.Phase & (0x0001 << i) )
			{
				ReadType[Num++] = MVaribleTab.Type + (i << 12);
			}
		}
		
		FourTemp.b[0] = 0;
		for( i = 0; i < Num; i++ )
		{
			if( api_GetRemoteData( ReadType[i], DATA_BCD, tDot, MVaribleTab.Lenth, Buf + MVaribleTab.Lenth * i ) == TRUE )
			{
				FourTemp.b[0]++;	//���øñ����洢���������ݸ���
			}
		}
		if( FourTemp.b[0] == 0 )	//���һ�����ݶ�û�� ���ش���
		{
			return DLT645_ERR_ID_02;
		}
		
		if( MVaribleTab.Lenth*Num < OutBufLen )
		{
			//�������ݿ�������ʽ��ͬ ����û��B���C�� ��Ҫ���FF
			memset( OutBuf, 0xFF, MVaribleTab.Lenth * Num );
			memcpy( OutBuf, Buf, MVaribleTab.Lenth*FourTemp.b[0] );
			FourTemp.b[0] = 0;
			
			return (MVaribleTab.Lenth * Num);
		}
		
		return DLT645_ERR_ID_02;
	}
//========================================================================================//	
    
	if( (DI[2] == 0x80)&&(DI[1] == 0x00) )
	{
		switch( DI[0] )
		{
			case 0x01:	//���ߵ���
				#if( MAX_PHASE == 1 )
				if( api_GetRemoteData( PHASE_N + REMOTE_I, DATA_BCD, 3, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				#else
				if( MeterTypesConst == METER_3P3W)//������������(����ʸ����)����
				{
					Lenth = 3;
					memset( Buf, 0x00, Lenth );
				}
				else if( MeterTypesConst == METER_ZT )//ֱͨ�������ߵ���-��ֱͨ�����������
				{
					if( api_GetRemoteData( PHASE_N + REMOTE_I, DATA_BCD, 3, 3, Buf ) == TRUE )
					{
						Lenth = 3;
					}
				}
				else
				{
					if( api_GetRemoteData( PHASE_N2 + REMOTE_I, DATA_BCD, 3, 3, Buf ) == TRUE )
					{
						Lenth = 3;
					}
				}
				#endif
				break;
			case 0x02:	//����Ƶ��
				if( api_GetRemoteData( REMOTE_HZ, DATA_BCD, 2, 2, Buf ) == TRUE )
				{
					Lenth = 2;
				}
				break;
			case 0x03:	//һ�����й���ƽ������
				if( api_GetRemoteData( REMOTE_P_AVE, DATA_BCD, 4, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
			
			#if( MAX_PHASE == 3 )
			case 0x04:	//��ǰ�й�����
			case 0x05:	//��ǰ�޹�����
			case 0x06:	//��ǰ��������
				api_GetCurrDemandData((DI[0] - 0x03), DATA_BCD, 4, 3, Buf );
				
				Lenth = 3;
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;				
				break;
			#endif//#if( MAX_PHASE == 3 )
			case 0x07:	//�����¶�
				wResult = api_GetBatAndTempValue( DATA_BCD, SYS_TEMPERATURE_AD);
				memcpy( Buf, (void*)&wResult, 2 );
				Lenth = 2;
				break;
			case 0x08:	//ʱ�ӵ�ص�ѹ(�ڲ�)
				wResult = api_GetBatAndTempValue( DATA_BCD, SYS_CLOCK_VBAT_AD);
				memcpy( Buf, (void*)&wResult, 2 );
				Lenth = 2;
				break;
			case 0x09:	//ͣ�糭���ص�ѹ (�ⲿ)
				wResult = api_GetBatAndTempValue ( DATA_BCD, SYS_READ_VBAT_AD);
				memcpy( Buf, (void*)&wResult, 2 );
				Lenth = 2;
				break;
			case 0x0A:	//�ڲ���ع���ʱ��
				if( api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( BatteryTime ), sizeof(DWORD), Buf ) == TRUE )
				{
					Lenth = 4;
					Sel_HEX2BCD = YES;
					CoverMode = eCOMB_MODE;
				}
				break;				
			#if(PREPAY_MODE == PREPAY_LOCAL)	
			case 0x0B://��ǰ���ݵ��	XXXX.XXXX	4	Ԫ/kWh
				api_ReadPrePayPara(eCurLadderPrice, Buf);
				Sel_HEX2BCD = YES;
				Sel_EXData = NO;
				CoverMode = eCOMB_MODE;
				Lenth = 4;
				break;
			#endif				
			case 0x0C:	//���߹�������---����
				if( api_GetRemoteData( PHASE_N + REMOTE_COS, DATA_BCD, 3, 2, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
			case 0x0D:	//�����й�����---���� 
				if( api_GetRemoteData( PHASE_N + REMOTE_P, DATA_BCD, 4, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
				break;
			case 0x0E:	//���ߵ���---����
				if( api_GetRemoteData( PHASE_A + REMOTE_I, DATA_BCD, 3, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
			case 0x0F:  //�����й�����---����
				if( api_GetRemoteData( PHASE_A + REMOTE_P, DATA_BCD, 4, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
			case 0x10:	//���߹�������---����
				if( api_GetRemoteData( PHASE_A + REMOTE_COS, DATA_BCD, 3, 2, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;				
			#if(PREPAY_MODE == PREPAY_LOCAL)	
        	case 0x20: //��ǰ���
				api_ReadPrePayPara(eCurPice, Buf);
				Sel_HEX2BCD = YES;
				Sel_EXData = NO;
				CoverMode = eCOMB_MODE;
				Lenth = 4;
				break;
        	case 0x21: //��ǰ���ʵ��
				api_ReadPrePayPara(eCurRatePrice, Buf);
				Sel_HEX2BCD = YES;
				Sel_EXData = NO;
				CoverMode = eCOMB_MODE;
				Lenth = 4;
				break;
			#endif			
//			case 0x22:	//�����֤ʱЧʣ��ʱ��
//				if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE )
//				{
//					memcpy( Time.b, (BYTE *)&APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime, 4 );
//					if( (Time.d%60) != 0 )
//					{
//						Time.d = (Time.d /60)+1;
//					}
//					else
//					{
//						Time.d = (Time.d /60);
//					}
//				}
//				else
//				{
//					Time.d = 0;
//				}
//
//			    memcpy( Buf, Time.b, 2 );//ǿ��ת��Ϊ2���ֽ�
//				Lenth = 2;
//				Sel_HEX2BCD = YES;
//				CoverMode = eCOMB_MODE;
//				break;
//			case 0x23:	//������֤ʱЧʣ��ʱ��
//			    memcpy( Time.b, (BYTE*)&APPConnect.IRAuthTime, 4); 
//			    if( (Time.d%60) != 0 )
//			    {
//			        Time.d = (Time.d /60)+1;
//			    }
//			    else
//			    {
//                    Time.d = (Time.d /60);
//			    }
//
//			    Buf[0] = Time.b[0];//ǿ��ת��Ϊ1�ֽ�
//			    Lenth = 1;
//				Sel_HEX2BCD = YES;
//				break;
			default:
				break;
		}
	}
	
	//���ݳ��ȳ���
	if( (Lenth == 0x00) || (Lenth > OutBufLen) )
	{
		return DLT645_ERR_ID_02;
	}
	
	//ͳһ����HEXתBCD
	if( Sel_HEX2BCD == YES )
	{
		if( ProBinToBCD( CoverMode, Lenth, Buf ) == FALSE )
		{
			return DLT645_ERR_ID_02;
		}
	}
	
	//ͳһ������
	if( Sel_EXData == YES )
	{
		lib_ExchangeData( OutBuf, Buf, Lenth );
	}
	else
	{
		memcpy( OutBuf, Buf, Lenth );
	}
	
	return Lenth;
}

#endif//#if( (SEL_DLT645_2007 == YES) || (SEL_DLT645_2007_BASIC == YES) )

