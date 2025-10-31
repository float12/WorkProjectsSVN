//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������    ������
//��������  2017��9��6��09:23:07
//����      645��Լ��ȡ����
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "dlt645_2007.h"

#if( SEL_DLT645_2007 == YES )
//---------------------------------------------------------------
//��������: ͨ��ר��HEXתBCD
//
//����: 
//		  Type[in]
// 			  eBYTE_MODE - ���ֽ�ģʽ
//			  eCOMB_MODE - ���ģʽ ���糤��Ϊ2 ����BYTE��ϳ�һ��WORD
// 		  Lenth[in] -  �������ݳ���
// 		  Buf[in] - ���뻺��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL ProBinToBCD( eTYPE_COVER_DATA Type, BYTE Lenth, BYTE *Buf )
{
	BYTE i;
	TFourByteUnion CoverBuf;
	
    CoverBuf.l = 0;
    
	if( Type == eCOMB_MODE )
	{
		if(  Lenth > 4  ) //���ģʽ���֧��4���ֽ�
		{
			return FALSE;
		}
		
		memcpy( CoverBuf.b, Buf, Lenth );
		
		switch( Lenth )
		{
			case 1:
				if( CoverBuf.b[0] > 99 )
				{
					return FALSE;
				}
				CoverBuf.b[0] = lib_BBinToBCD( CoverBuf.b[0] );
				break;
			case 2:
				if( CoverBuf.w[0] > 9999 )
				{
					return FALSE;
				}
				CoverBuf.w[0] = lib_WBinToBCD( CoverBuf.w[0] );
				break;
			case 4:
				if( CoverBuf.d > 99999999 )
				{
					return FALSE;
				}
				CoverBuf.d = lib_DWBinToBCD( CoverBuf.d );
				break;
			default:
				return FALSE;		
		}
		memcpy( Buf, CoverBuf.b, Lenth );
	}
	else
	{
		for( i = 0; i < Lenth; i++ )
		{
			if( Buf[i] > 99 )
			{
				return FALSE;
			}
			Buf[i] = lib_BBinToBCD( Buf[i] );
		}
	}
	
	return TRUE;
}

//---------------------------------------------------------------
//��������: ͨ��ר��HEXתBCD һ�������ģʽ
//
//����: 
//		  Type[in]
// 			  eBYTE_MODE - ���ֽ�ģʽ
//			  eCOMB_MODE - ���ģʽ ���糤��Ϊ2 ����BYTE��ϳ�һ��WORD
// 		  Lenth[in] -  �������ݳ���
// 		  Buf[in] - ���뻺��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL ProBinToBCD645( eTYPE_COVER_DATA Type, BYTE Lenth, BYTE *Buf )
{
	BOOL Result;
	BYTE i,BackFlag;
	TFourByteUnion CoverBuf;
	TEightByteUnion llCoverBuf;
	
	BackFlag = 0;
	
	if( Lenth > 4 )	//�߾��ȵ���
	{
		llCoverBuf.ll = 0;
		CoverBuf.l = 0;
		
		memcpy( llCoverBuf.b, Buf, 8);
		if( llCoverBuf.ll < 0 )
		{
			BackFlag = 1;
			llCoverBuf.ll *= -1;
		}
		
		CoverBuf.l = (llCoverBuf.ll % 100000000);

		Result = ProBinToBCD( eCOMB_MODE, 4, CoverBuf.b);
		if(Result == 0)
		{
			return FALSE;
		}
		else
		{
			memcpy( Buf, CoverBuf.b, 4);
		}

		CoverBuf.l = (llCoverBuf.ll/100000000);		
		Result = ProBinToBCD( eCOMB_MODE, 4, CoverBuf.b);
		if(Result == 0)
		{
			return FALSE;
		}
		else
		{
			if( BackFlag == 1 )
			{
				CoverBuf.b[Lenth-1-4] |= 0x80;
			}
			memcpy( Buf+4, CoverBuf.b, Lenth-4);
		}
		
	}
	else
	{
	    if( Lenth == 0 )
	    {
            return FALSE;
	    }
	    
		CoverBuf.l = 0;
		memcpy( CoverBuf.b, Buf,4 );
		
		if( CoverBuf.l < 0 )
		{
			BackFlag = 1;
			CoverBuf.l *= -1;
		}
		
		Result = ProBinToBCD( eCOMB_MODE, 4, CoverBuf.b);
		if(Result == 0)
		{
			return FALSE;
		}
		else
		{
			if( BackFlag == 1 )
			{
				CoverBuf.b[Lenth-1] |= 0x80;
			}
			memcpy( Buf, CoverBuf.b, Lenth );				
		}
	}
	
	return TRUE;
}

//---------------------------------------------------------------
//��������: ��ȡ���ɼ�¼����
//
//����: 
//			PortType[in]-ͨѶ��
//		    DI[in]     - �������ݱ�ʶ
//		    OutBufLen  - ���뻺�泤��	
// 			OutBuf[out]- ������ݻ���                  
//����ֵ:  
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//��ע:   
//---------------------------------------------------------------
WORD GetRequest645Lpf( eSERIAL_TYPE PortType, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	// WORD Result,Lenth;
	// BYTE FollowFlag;
	
	// FollowFlag =0;
	// Result = DLT645_OK_00;	
	// Lenth = api_ReadLpfRecord645( PortType, gPr645[PortType].byReceiveControl , DI, gPr645[PortType].pMessageAddr+15, gPr645[PortType].pMessageAddr[14], OutBuf );

	// //���ݷ��س��ȣ�ȷ��Ӧ�÷��ص�״̬�ֽ�
	// if( Lenth == 0x8000 ) //����0x8000 ˵�����ݳ���
	// {
	// 	Result = DLT645_ERR_ID_02;
	// }
	// else
	// {
	//     if(Lenth&BIT14)
	//     {
    //         FollowFlag = 0X55;
	//     }
	    
	//     Lenth = Lenth&(~BIT14);
	    
	// 	if( Lenth > OutBufLen )
	// 	{
	// 		Result = DLT645_ERR_ID_02;			
	// 	}
	// 	else
	// 	{
	// 		Result = Lenth;
	// 		if( gPr645[PortType].pMessageAddr[8] == 0x11 )//����������11H
	// 		{	
	// 		    if( FollowFlag == 0X55 )
	// 		    {
	// 		        DLT645APPFollow[PortType].FollowFlag = eLOAD_FOLLOW;
	// 		    }
	// 		}
	// 		else
	// 		{
    //             if( FollowFlag == 0 )
    //             {
    //                 DLT645APPFollow[PortType].FollowFlag = eLOAD_FOLLOW_END;
    //             }

	// 		}
	// 	}
	// }
	// return Result;
}

//---------------------------------------------------------------
//��������: ��ȡ�¼�������
//
//����: 
//		    DI[in]      - �������ݱ�ʶ
//		    OutBufLen   - ���뻺�泤��	 
// 			OutBuf[out] - ������ݻ���                 
//����ֵ:  
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//��ע:    
//---------------------------------------------------------------
WORD GetRequest645EventData( BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	WORD Result,Lenth;
	TFourByteUnion DataID;
	
	Result = DLT645_OK_00;	

	//�������ݱ�ʶ
	memcpy( DataID.b, DI, 4 );

	// Lenth = api_ReadEventRecord645( DataID.b, OutBuf );

	//���ݷ��س��ȣ�ȷ��Ӧ�÷��ص�״̬�ֽ�
	if( Lenth == 0x8000 ) //����0x8000 ˵�����ݳ���
	{
		Result = DLT645_ERR_ID_02;
	}
	else
	{
		if( Lenth > OutBufLen )
		{
			Result = DLT645_ERR_ID_02;
		}
		else
		{
			Result = Lenth;
		}
	}
	return Result;
}

//---------------------------------------------------------------
//��������: ��ȡ����������
//
//����: 
//		    DI[in]     - �������ݱ�ʶ
//		    OutBufLen  - ���뻺�泤��	 
// 			OutBuf	   - ������ݻ���                 
//����ֵ:  
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//��ע:   
//---------------------------------------------------------------
WORD GetRequest645FreezeData( BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	// WORD Lenth;
	
	// Lenth = api_ReadFreezeRecord645( DI, OutBuf );
	// if( Lenth >= 0x8000 )
	// {
		return DLT645_ERR_ID_02;
	// }
	// else if( (Lenth == 0x00) || (Lenth > OutBufLen) )
	// {//���ݳ��ȳ���
	// 	return DLT645_ERR_ID_02;
	// }
	// else
	// {
	// 	return Lenth;
	// }
}

//---------------------------------------------------------------
//��������: ��ȡĳ�����ݱ�ʶ��Ӧ������
//
//����:
//			Dot[in]        - ��ȡ����С��λ��
//		    ID[in]         - �������ݱ�ʶ
//		    OutBufLen[in]  - ���뻺�泤��
// 			OutBuf[out]    - ���뻺��
//
//����ֵ:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//
//��ע:
//---------------------------------------------------------------
WORD GetPro645IDData( eSERIAL_TYPE PortType,BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	WORD wResult;
	
	if( OutBuf == NULL )
	{
		return DLT645_ERR_ID_02;
	}

	wResult = DLT645_ERR_ID_02;
	
	switch( DI[3] )
	{
		// case DLT645_ENERGY_00H://������
		// 	wResult = GetRequest645Energy( Dot, DI, OutBufLen, OutBuf );
		// 	break;
		#if( MAX_PHASE == 3 )
		case DLT645_DEMAND_01H://������
			wResult = GetRequest645Demand( Dot, DI, OutBufLen, OutBuf );
			break;
		#endif//#if( MAX_PHASE == 3 )
		case DLT645_VARIABLE_02H://������
			wResult = Get645RequestVariable( Dot, DI, OutBufLen, OutBuf );
			break;
		
		case DLT645_PARA_04H://�α�����
			wResult = GetPro645RequestMeterPara( PortType,DI, OutBufLen, OutBuf );
			break;
		
		// case DLT645_FREEZE_05H://������
		// 	wResult = GetRequest645FreezeData( DI, OutBufLen, OutBuf );
		// 	break;
			
		#if( SEL_DLT645_2007 == YES )
		// case DLT645_LOAD_06H://���ɼ�¼�� !!!!!!��δ����
		// 	wResult = GetRequest645Lpf( PortType, DI, OutBufLen, OutBuf );
		// 	break;
		#if( SEL_TOPOLOGY == YES )
        case DLT645_TOPOLOGY_09H:
            wResult = api_GetRequest645Topology( PortType, DI, OutBufLen, OutBuf );
            break;
        #endif
		case DLT645_EVENT_03H://��Կ���¼�¼���쳣�忨�¼����������� !!!!!!!��ʱδд
		case DLT645_EVENT_19H://�����¼�
		case DLT645_EVENT_1DH://��բ�¼�
		case DLT645_EVENT_1EH://��բ�¼�
		case DLT645_EVENT_10H://ʧѹ�¼�
		case DLT645_EVENT_18H://ʧ���¼�
			// wResult = GetRequest645EventData( DI, OutBufLen, OutBuf );
			break;
		#endif//#if( (SEL_DLT645_2007 == YES)	
		default:
			break;
	}
	return wResult;
}


//---------------------------------------------------------------
//��������: ��ȡĳ�����ݱ�ʶ��Ӧ������
//
//����:     
//			Dot[in]    - ��ȡ����С��λ��
//		    ID[in]     - �������ݱ�ʶ
//		    OutBufLen  - ���뻺�泤��
// 			OutBuf[in] - ���뻺��
//
//����ֵ:	TRUE/FALSE
//
//��ע:
//---------------------------------------------------------------
BOOL api_GetPro645IDData( eSERIAL_TYPE PortType, BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	WORD Result;
	
	Result = GetPro645IDData( PortType, Dot, DI, OutBufLen, OutBuf );
	
	if( Result >= DLT645_ERR_DATA_01 )
	{
		return FALSE;
	}

	return TRUE;
}

#endif//#if( SEL_DLT645_2007 == YES )

