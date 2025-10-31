//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.15
//����		

//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"

#if( PREPAY_MODE != PREPAY_NO )
#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )


//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------	


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------																						


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE	PrePayBuf[MAX_PREPAY_LEN];
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//��������:  �������ͷ����        
//����:      BYTE* pbuf[in]       
//           BYTE inc[in]       
//           BYTE com[in]         
//           BYTE P1[in]        
//           BYTE P2[in]        
//           BYTE len1[in]         
//           BYTE len2[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void api_ProcPrePayCommhead(BYTE* pbuf,BYTE inc, BYTE com, BYTE P1, BYTE P2, BYTE len1,BYTE len2)
{
	pbuf[0] = inc;
	pbuf[1] = com;
	pbuf[2] = P1;
	pbuf[3] = P2;
	pbuf[4] = len1;
	pbuf[5] = len2;
}

//-----------------------------------------------
//��������: ��ȡ���ȫоƬ��Ϣ
//
//����:		Type[in]	ff--ȫ����Ϣ
//			OutBuf[out]	����Ҫ��ȡ�ĳ��ȼ����ݣ��������ֽڣ�����ǰ�����ں�
//����ֵ:	���ݳ��� 8000�������
//
//��ע:OutBuf���ʵ�ʼ���ĳ��ȶ������ֽڣ����ں������Ѿ��������ص�����ǰ�治������
//-----------------------------------------------
WORD api_GetEsamInfo(BYTE Type,BYTE *OutBuf)
{
	BOOL Result;
	BYTE CmdBuf[6];
	TTwoByteUnion tw;
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x36;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = Type;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x00;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,0,NULL,OutBuf);
	
	if( Result == FALSE )
	{
		return 0x8000;
	}

	//����
	tw.b[0] = OutBuf[1];
	tw.b[1] = OutBuf[0];
	if( tw.w > 200 )
	{
		return 0x8000; 
	}
	
	memmove( OutBuf, OutBuf + 2, tw.w );

	return tw.w;
}


//-----------------------------------------------
//��������: ����Ӧ������
//
//����:			Mode[in] 0x55: �ն� ��������վ
//				Buf[in/out]	����ʱ32�ֽ�����1+4�ֽڿͻ���ǩ��1
//						���ʱ����+48�ֽ������+4�ֽڿͻ���ǩ�����������ֽڣ�����ǰ�����ں�
//����ֵ:	TRUE/FALSE
//
//��ע:��698��Լ�н���Ӧ������ʱʹ��,Bufǰ��2�ֽڴ������ݳ���
//-----------------------------------------------
BOOL api_ConnectMechanismInfo( eConnectMode ConnectModeMode, BYTE *Buf )
{
	BOOL Result;
	BYTE CmdBuf[6];

	if( ConnectModeMode == eConnectTerminal )//�ն˽���Ӧ������
	{
		CmdBuf[0] = 0x82;
	}
	else
	{
		CmdBuf[0] = 0x81;
	}

	CmdBuf[1] = 0x02;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x00;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x24;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,0x24,Buf,Buf);
	
	return Result;
}


//-----------------------------------------------
//��������: ��֤��վ�·������ݲ����ؽ����������
//
//����:		
//			Buf[in/out]����ʱ��	��������֤��ϢΪSID_MAC
//							4�ֽڱ�ʶ+��������AttachData+Data+MAC
//								��������֤��ϢΪSID
//							4�ֽڱ�ʶ+��������AttachData+Data
//						���ʱ�����ȼ����ݣ��������ֽڣ�����ǰ�����ں�
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL api_AuthDataSID(BYTE *Buf)
{
	WORD Result;
	TTwoByteUnion BufLen;
	
	BufLen.b[0] = Buf[5];
	BufLen.b[1] = Buf[4];
	if( BufLen.w > MAX_PREPAY_LEN )//����������������apdu���ݷ���ʧ��
	{
		return FALSE;
	}
	
	Result = Link_ESAM_TxRx(6,Buf,BufLen.w,Buf+6,Buf+6);
	if( Result == TRUE )//�����ɹ���������ݵİ���
	{
		BufLen.b[0] = Buf[7];
		BufLen.b[1] = Buf[6];
		memmove(Buf,&Buf[6],BufLen.w+2);
	}

	return Result;
}

//-----------------------------------------------
//��������: �������֯������д��ESAM�����mac,������ϴ�
//
//����:
//			Len[in] ��������ݳ���
//			Buf[in/out]	��������
//					���ݴ���������ʱ ���������ֽڣ�����ǰ�����ں�
//						�������+mac
//
//����ֵ:	TRUE/FALSE
//
//��ע:Bufǰ��2�ֽڴ������ݳ���
//-----------------------------------------------
BOOL api_PackDataWithSafe(WORD Len,BYTE *Buf)
{
	BOOL Result;
	BYTE CmdBuf[6];
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x0e;
	CmdBuf[2] = 0x40;
	CmdBuf[3] = 0x02;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,Buf);
	
	return Result;
}


//-----------------------------------------------
//��������: ������д��ESAM,������ϴ�
//
//����:		EncryptMode[in] ���ܷ�ʽ 0x00�� ��վ      0x55���ն�
//			Type[in] 1--����+MAC	2--����	3--����+mac
//			Len[in] ��������ݳ���
//			Buf[in/out]	��������
//					���ݴ���������ʱ ���������ֽڣ�����ǰ�����ں�
//						TypeΪ1ʱ���������+mac
//						TypeΪ2ʱ���������+����
//						TypeΪ3ʱ���������+����+mac
//
//����ֵ:	TRUE/FALSE
//
//��ע:Bufǰ��2�ֽڴ����س���
//-----------------------------------------------
BOOL api_PackEsamDataWithSafe(BYTE EncryptMode, BYTE Type,WORD Len,BYTE *Buf)
{
	BOOL Result;
	BYTE CmdBuf[6], CmdBuf1[6],Len1;
	WORD Datalen;

	if( Len > MAX_APDU_SIZE )
	{
		return FALSE;
	}
	
	if( EncryptMode == 0x55 )//�ն˼���
	{
		CmdBuf[0] = 0x82;
		CmdBuf[1] = 0x24;	
		CmdBuf[2] = 0x00;
		if( (Type == 1) || ( Type == 3 ))
		{
			CmdBuf1[0] = 0x80;
			CmdBuf1[1] = 0x24;
			CmdBuf1[2] = 0x10+Type;
			CmdBuf1[3] = 0x00;
			
			if( Type == 3 )//����+MAC
			{
				Len1 = (Len + 3);
				if( (Len1 %16) == 0 )
				{ 
					Datalen = (Len1+4);
				}
				else
				{
					Datalen = (((Len1/16)+1)*16+4);
				}
			}
			else
			{
				Datalen= Len+4;
			}

			CmdBuf1[4] = Datalen>>8;
			CmdBuf1[5] = Datalen;
			memcpy( Buf+Len, CmdBuf1, 6 );
			
			Len += 6;
			CmdBuf[3] = 0x10+Type;
			CmdBuf[4] = Len>>8;
			CmdBuf[5] = Len;
		}
		else if( Type == 2 )//���ķ�ʽ
		{
			CmdBuf[3] = 0x12;
			CmdBuf[4] = Len>>8;
			CmdBuf[5] = Len;
		}		
		else
		{
			return 0;
		}
	}
	else//��վ����
	{
		CmdBuf[0] = 0x81;
		CmdBuf[1] = 0x1c;	
		CmdBuf[2] = 0x00;
		if( Type == 1 )
		{
			CmdBuf[3] = 0x11;
		}
		else if( Type == 2 )
		{
			CmdBuf[3] = 0xa6;
		}
		else if( Type == 3 )
		{
			CmdBuf[3] = 0xa7;
		}
		else
		{
			return 0;
		}

		CmdBuf[4] = Len>>8;
		CmdBuf[5] = Len;
		
	}
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,Buf);
	
	return Result;
}

//-----------------------------------------------
//��������:     �����ѯָ���ȡ�����1���ģ�
//
//����:	 	RNBuf[in]     //��������� RNBuf[0](���������)+RNBuf[1]�������������
//	       OutBuf[in/out] //������� ��8�ֽ� �������ȣ�
//		
//						
//����ֵ:	    TRUE/FALSE
//		   
//��ע: 
//-----------------------------------------------
BOOL api_EsamIRRequest( BYTE *RNBuf, BYTE *OutBuf )
{
	BOOL Result;
	BYTE CmdBuf[6],InBuf[MAX_RN_SIZE+10];

	CmdBuf[0] = 0x80; //��֡����ͷ
	CmdBuf[1] = 0x08;
	CmdBuf[2] = 0x08;
	CmdBuf[3] = 0x03;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x10;

    InBuf[0] = 0x00;
    InBuf[1] = 0x00;

    Result = api_ReadEsamData(0x0001, 0x0007, 0x0006, InBuf+2);//��ȡ���
	if( Result == FALSE )
	{
		return FALSE;
	}

    if( RNBuf[0] > MAX_RN_SIZE )//��������Ȳ��ܳ���MAX_RN_SIZE
    {
        return FALSE;
    }
    
	memcpy(InBuf+8,RNBuf+1,RNBuf[0]);//��ȡ�����
	Result = Link_ESAM_TxRx(6,CmdBuf,(8+RNBuf[0]),InBuf,OutBuf);//����esam
	if( Result == FALSE )
	{
		return FALSE;
	}

	if( (OutBuf[0] != 0x00) || ( OutBuf[1] != 0x08) )//����Э��Ҫ�� ��������Ϊ8���ֽ�
	{
        return FALSE;
	}
	
    memmove( OutBuf, OutBuf+2, 8);
    
	return Result;
}
//�ն˵������֤ �ն˵������֤����Ҫ��ŵõ����ģ����Ժ�IR�ֿ���
BOOL api_EsamTerminalRequest( BYTE *RNBuf, BYTE *OutBuf )
{
	BOOL Result;
	BYTE CmdBuf[6];
	
	CmdBuf[0] = 0x80; //��֡����ͷ
	CmdBuf[1] = 0x08;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x04;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x08;
    
	Result = Link_ESAM_TxRx(6,CmdBuf,8,RNBuf+1,OutBuf);//����esam
	if( Result == FALSE )
	{
		return FALSE;
	}

	if( (OutBuf[0] != 0x00) || ( OutBuf[1] != 0x08) )//ESAM �ֲ���ʾֻ��8���ֽڵ����
	{
        return FALSE;
	}
	
    memmove( OutBuf, OutBuf+2, 8);
    
	return Result;	
}
//-----------------------------------------------
//��������:     ������ָ֤���ȡ�����1���ģ�
//
//����:	    INBuf[in]     //�������������
//
//����ֵ:	    TRUE/FALSE
//
//��ע:
//-----------------------------------------------
BOOL api_EsamIRAuth( BYTE *INBuf )
{
    BYTE CmdBuf[6],OutBuf[12],Result;
	
	CmdBuf[0] = 0x80;//��֡����ͷ
	CmdBuf[1] = 0x06;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x01;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x08;
    
    Result = Link_ESAM_TxRx(6,CmdBuf,8,INBuf,OutBuf);//���������������֤ ����ֵ��ʵ������
    if( Result == FALSE )
    {
        return FALSE;
    }

    return Result;
}
//-----------------------------------------------
//��������: ���������ϱ����ݵ�rn_mac
//
//����:		
//			Len[in] 	���볤��
//			Buf[in]		��������
//			OutBuf[out]	������� rn+mac		
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
WORD api_DealEsamSafeReportRN_MAC( WORD Len,BYTE *Buf,BYTE *OutBuf )
{
	BYTE CmdBuf[11];
	BOOL Result;
	WORD ReturnLen;
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x14;
	CmdBuf[2] = 0x01;
	CmdBuf[3] = 0x03;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,OutBuf);
	if( Result == FALSE )
	{
		return 0x8000;
	}

	lib_ExchangeData( (BYTE*)&ReturnLen, OutBuf, 2 );
	memmove( OutBuf, OutBuf+2, ReturnLen );

	return ReturnLen;
}

//-----------------------------------------------
//��������: ��֤�����ϱ�Ӧ����
//
//����:		
//			Len[in] 	���볤��
//			Buf[in]		��������
//			OutBuf[out]	������� rn+mac		
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
WORD api_VerifyEsamSafeReportMAC( BYTE DataLen,BYTE *DataBuf,BYTE *RN_MAC )
{
	WORD Len;	
	BYTE CmdBuf[11];
	BYTE Buf[280];//255+16
	BOOL Result;
	WORD ReturnLen;
	BYTE OutBuf[512];
	
	Len = DataLen+16;
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x0E;
	CmdBuf[2] = 0x40;
	CmdBuf[3] = 0x81;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	memset( Buf, 0x00, sizeof(Buf) );
	memcpy( Buf, RN_MAC, 12 );
	memset( Buf+12, 0x00, 4 );
	memcpy( Buf+16, DataBuf, DataLen );
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,OutBuf);

	return Result;
	
}

//-----------------------------------------------
//��������: ��ȡESAM�ļ���Ϣ
//
//����:		FID[in] 	�ļ���ʶ
//			Offset[in] 	ƫ�Ƶ�ַ
//			Len[in] 	��ȡ����
//			Buf[out]	�������+���ݣ��������ֽڣ�����ǰ�����ں�
//						
//����ֵ:	TRUE/FALSE
//		   
//��ע:��Ҫȷ����վ�·���ʱ���Ƿ���cmd
//-----------------------------------------------
BOOL api_ReadEsamData(WORD FID,WORD Offset,WORD Len,BYTE *Buf)
{
	BOOL Result;
	BYTE CmdBuf[11];
	WORD ReturnLen;
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x2c;
	CmdBuf[2] = FID>>8;
	CmdBuf[3] = FID;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x05;
	CmdBuf[6] = 0x00;
	CmdBuf[7] = Offset>>8;
	CmdBuf[8] = Offset;
	CmdBuf[9] = Len>>8;
	CmdBuf[10] = Len;
	
	Result = Link_ESAM_TxRx(11,CmdBuf,0,NULL,Buf);
	if( Result == FALSE )
	{
		return FALSE;
	}

	lib_ExchangeData( (BYTE*)&ReturnLen, Buf, 2 );
	memmove( Buf, Buf+2, ReturnLen );

	return TRUE;
}

//-----------------------------------------------
//��������: ȡESAM�����
//
//����:		Len[in]		��������� 4/8/10
//			Buf[out]	�������+���ݣ��������ֽڣ�����ǰ�����ں�					
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL api_ReadEsamRandom(BYTE Len,BYTE *Buf)
{
	BOOL Result;
	BYTE CmdBuf[6];
	
	if( Len > 8 )
	{
		return FALSE;
	}

	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x04;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = Len;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x00;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,0,NULL,Buf);
	
	memcpy( SafeAuPara.EsamRand, Buf + 2, Len );
	
	return Result;
}

//-----------------------------------------------
//��������: ��ȡǮ��
//
//����:		Buf[out]	�������+���ݣ��������ֽڣ�����ǰ�����ں�
//						
//����ֵ:	TRUE/FALSE
//		   
//��ע:�����4��+���������4��+�ͻ���ţ�6��
//-----------------------------------------------
BOOL ReadEsamMoneyBag( BYTE *Buf )
{
	BOOL Result;
	BYTE CmdBuf[7];
	BYTE OutBuf[20];
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x48;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x00;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x01;
	CmdBuf[6] = 0x00;
	
	Result = Link_ESAM_TxRx( 7, CmdBuf, 0, NULL, OutBuf );
	
	memcpy( Buf, OutBuf+2, 14 );
	
	return Result;
}

//-----------------------------------------------
//��������: ����esam������Ϣ�ļ�(�˺���û��ʹ��)
//
//����:		Offset[in]	ƫ��
//			Len[in]		����
//			Buf[in/out]	��������,���Ϊ����+����					
//����ֵ:	TRUE/FALSE
//		   
//��ע:BufΪ�������ݣ����ǲ�����ɺ�buf���ݻ��,�˺���������Ҫ��@@@@@@
//-----------------------------------------------
BOOL api_UpdateEsamRunFile(BYTE Offset,WORD Len,BYTE *Buf)
{	
	BOOL Result;
	BYTE CmdBuf[6];
	
	CmdBuf[0] = 0x83;
	CmdBuf[1] = 0x2a;
	CmdBuf[2] = 0x88;
	CmdBuf[3] = Offset;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,Buf);
	
	return Result;
}

//--------------------------------------------------
//��������:  ����ȶ�����
//
//����:      BYTE P2[in]//��Կ��ʶ
//
//           BYTE Len[in]//���ݳ���
//
//           BYTE *Buf[in]//����bufָ��
//
//����ֵ:
//
//��ע����:  Bufǰ��2�ֽڴ�����
//--------------------------------------------------
BOOL api_SoftwareComparisonEsam(BYTE P2,WORD Len,BYTE *Buf)
{	
	BOOL Result;
	BYTE CmdBuf[6];
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x0A;
	CmdBuf[2] = 0x48;
	CmdBuf[3] = P2;
	CmdBuf[4] = Len>>8;
	CmdBuf[5] = Len;
	
	Result = Link_ESAM_TxRx(6,CmdBuf,Len,Buf,Buf);
	
	return Result;
}

//-----------------------------------------------
//��������: ��ȡ��Կ״̬
//
//����:		��					
//����ֵ:	��
//		   
//��ע:�ϵ缰��Կ����ʱ���á�����esam�е�״̬ˢ��ee�е���Կ״̬��
//		���ڲ���ee����ֹƵ�����á�
//-----------------------------------------------
void api_FlashKeyStatus(void)
{
	BYTE Buf[20],BufBak[20];
	BYTE i;
	WORD Len;

	for(i = 0;i < 3;i++)//����3��
	{
		Len = api_GetEsamInfo( 0x04, Buf );
		if( Len <= (sizeof(Buf)-2) )
		{
			break;
		}
		if(i == 1)	//ʧ������
		{
			api_ResetEsamSpi();//����ESAM�ܽź�SPIͨ��
		}
	}
	
	if(i == 3)
	{
		if( api_GetRunHardFlag(eRUN_HARD_ESAM_ERR) == FALSE )
		{
			api_SetFollowReportStatus(eSTATUS_ESAM_Error);
		}	
		api_SetRunHardFlag(eRUN_HARD_ESAM_ERR);
		api_ClrRunHardFlag(eRUN_HARD_COMMON_KEY);
		return;
	}
	else
	{
		api_ClrRunHardFlag(eRUN_HARD_ESAM_ERR);
	}

	memset( BufBak, 0x00, sizeof(BufBak) );

	if( memcmp( Buf, BufBak, 16) == 0 )//16�ֽ�0Ϊ��Կ������Ϊ˽Կ
	{
		api_SetRunHardFlag(eRUN_HARD_COMMON_KEY);
	}
	else
	{
		api_ClrRunHardFlag(eRUN_HARD_COMMON_KEY);
	}

}

//---------------------------------------------------------------
//��������: ˢ�������֤Ȩ�ޱ�־
//
//����: 
//                   
//����ֵ:  
//
//��ע:   ˢ�������֤Ȩ�� ��־
//		 ˢ�������֤Ȩ�� ==1�����뽨���ԳƼ���Ӧ�����Ӳſ��Գ�������
// 		 �����֤Ȩ�޴򿪣����뽨��Ӧ�����Ӳſ���ͨ������+RN�������� 
//---------------------------------------------------------------
void api_FlashIdentAuthFlag( void )
{
	BYTE Buf[20];
	WORD Len;
	
	Len = api_GetEsamInfo( 0x08, Buf );
	if( Len != 0x8000 )
	{
		if( Buf[0] == 0 )
		{
			api_ClrRunHardFlag( eRUN_HARD_EN_IDENTAUTH );
		}
		else
		{
			api_SetRunHardFlag( eRUN_HARD_EN_IDENTAUTH );
		}
	}
	else
	{
		//ESAM��ȡ����Ĭ��Ȩ�����δ��
		api_ClrRunHardFlag( eRUN_HARD_EN_IDENTAUTH );
	}
}



#endif//#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#endif//#if( PREPAY_MODE != PREPAY_NO )


