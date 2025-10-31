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
#if( ENCRYPT_MODE == ENCRYPT_ESAM )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------																						


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE				Err_Sw12[2];

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
extern BYTE			CommandHead[7];

//-----------------------------------------------
//				��������
//-----------------------------------------------

#if(ESAM_COM_TYPE == TYPE_SPI)
//-----------------------------------------------
//��������: дESAMһ���ֽ�����
//
//����:		Data[in]	д������
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void Drv_SendESAMSPI_Byte(BYTE Data)
{
	api_UWriteSpi( eCOMPONENT_SPI_ESAM, Data );
}


//-----------------------------------------------
//��������: SPI����ESAMһ���ֽ�����
//
//����:		��
//						
//����ֵ:	���ض�������
//		   
//��ע:
//-----------------------------------------------
BYTE Drv_ReceiveESAMSPI_Byte(void)
{
	return( api_UWriteSpi( eCOMPONENT_SPI_ESAM, 0xff ) );
}

//-----------------------------------------------
//��������: ��дESAM����
//
//����:		CmdLen[in]	����ͷ���ݳ���
//			Cmd[in]		����ͷ
//			BufLen[in]	�������ݳ���
//			InBuf[in]	��������
//			OutBuf[out]	������� ������ֽڳ��ȼ����ݣ����ȸ���ǰ�����ں�
//����ֵ:	TRUE/FALSE
//
//��ע:OutBuf���ǰ��2�ֽ�Ϊ���ȣ�����Ϊ����
//-----------------------------------------------
BOOL Link_ESAM_TxRx1(BYTE CmdLen, BYTE *Cmd, WORD BufLen, BYTE *InBuf, BYTE *OutBuf)
{
	WORD i;
	TTwoByteUnion len;
	BYTE LCRC;
	
	#if (SEL_ESAM_TYPE == 0)
	return FALSE;
	#else
	LCRC = 0x00;
	CLEAR_WATCH_DOG;
	
	DoSPICS(CS_SPI_ESAM, YES);
	api_Delay10us(5);
	
	//�ȼ�¼������ͷ��������ʧ��ʱ��¼�쳣�忨��¼��
	#if( PREPAY_MODE == PREPAY_LOCAL )
	CommandHead[0] = ESAM;
	CommandHead[1] = Cmd[0];
	CommandHead[2] = Cmd[1];
	CommandHead[3] = Cmd[2];
	CommandHead[4] = Cmd[3];
	CommandHead[5] = Cmd[4];
	CommandHead[6] = Cmd[5];
	#endif
	
	//���㷢��CRC
	for( i=0; i<CmdLen; i++)
	{
		LCRC ^= Cmd[i] ;
	}
	for( i=0; i<BufLen; i++)
	{
		LCRC ^= InBuf[i] ;
	}
	LCRC ^= 0xff;//ȡ��crc

	//��������
	Drv_SendESAMSPI_Byte(0x55);  
	//��������ͷ
	for(i=0; i<CmdLen; i++)
	{
		Drv_SendESAMSPI_Byte(Cmd[i]);
	}
	//����������	
	for(i=0; i<BufLen; i++)
	{
		Drv_SendESAMSPI_Byte(InBuf[i]);
	}
	//����crc
	Drv_SendESAMSPI_Byte( LCRC );
	api_Delay10us(1);
	DoSPICS(CS_SPI_ESAM, NO);
	api_Delay10us(2);
	CLEAR_WATCH_DOG;
	DoSPICS(CS_SPI_ESAM, YES);	
	api_Delay10us(6);
	//��ʼ����
	Err_Sw12[0] = 0;
	Err_Sw12[1] = 0;
	LCRC = 0x00;
	//�ȴ�ʱ�䣬��ʱ�˳�
	for( i=0; i<10000; i++ )
	{
		if( Drv_ReceiveESAMSPI_Byte() == 0x55 )
		{
			break;
		}
		api_Delay10us(4);
	}
	if( i == 10000 )
	{
		DoSPICS(CS_SPI_ESAM, NO);
		return FALSE;
	}
	
	//����״̬λ������Ϊ9000
	Err_Sw12[0] = Drv_ReceiveESAMSPI_Byte();
	LCRC ^= Err_Sw12[0];
	Err_Sw12[1] = Drv_ReceiveESAMSPI_Byte();
	LCRC ^= Err_Sw12[1];
	if( (Err_Sw12[0]!=0x90) || (Err_Sw12[1]!=0x00) )
	{
		//macЧ�����
		if( (Err_Sw12[0]==0x69) || (Err_Sw12[1]==0x89) )
		{
			if(MacErrCounter < 202)
			{
				MacErrCounter++;
				if(MacErrCounter == 200)	
				{
					api_SetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG);
				}
			}
			
		}
		DoSPICS(CS_SPI_ESAM, NO);
		return FALSE;
	}
	
	//�������ݳ���
	len.b[1] = Drv_ReceiveESAMSPI_Byte();	
	len.b[0] = Drv_ReceiveESAMSPI_Byte();	
	
	OutBuf[0] = len.b[1];
	LCRC ^= OutBuf[0];
	OutBuf[1] = len.b[0];
	LCRC ^= OutBuf[1];	

	if( len.w > MAX_APDU_SIZE )//�Գ��Ƚ��м���ֵ�ж� ���ܳ���apdu����󳤶�
	{
		return FALSE;
	}
	
	//����������
	for( i=0; i<len.w; i++) 
	{
		OutBuf[2+i] = Drv_ReceiveESAMSPI_Byte();	
		LCRC ^= OutBuf[2+i];
	}
	LCRC ^= 0xff;
	//�жϽ���CRC�Ƿ���ȷ
	if( LCRC != (Drv_ReceiveESAMSPI_Byte()) ) 
	{
		DoSPICS(CS_SPI_ESAM, NO);
		return FALSE;
	}

	DoSPICS(CS_SPI_ESAM, NO);
	//��ֹESAM����ͨ���޷�ʶ�������
	api_Delay10us(2);
	//���ؽ������ݳ���
	return TRUE ;
	#endif
	
}

//-----------------------------------------------
//��������: ��дESAM����
//
//����:		CmdLen[in]	����ͷ���ݳ���
//			Cmd[in]		����ͷ
//			BufLen[in]	�������ݳ���
//			InBuf[in]	��������
//			OutBuf[out]	������� ������ֽڳ��ȼ����ݣ����ȸ���ǰ�����ں�
//����ֵ:	TRUE/FALSE
//
//��ע:OutBuf���ǰ��2�ֽ�Ϊ���ȣ�����Ϊ����
//-----------------------------------------------
BOOL Link_ESAM_TxRx(BYTE CmdLen, BYTE *Cmd, WORD BufLen, BYTE *InBuf, BYTE *OutBuf)
{
	BYTE Result = 0;
	
    if( ESAM_POWER_IS_CLOSE )//esam��Դδ��
    {
//        api_ResetEsamSpi();
//        if((APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime == 0)//��ǰʣ��ʱ��ȫΪ0
//        &&(APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime == 0)
//        &&(APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime == 0) )
//        {
//        	if( ESAMPowerControlConst == YES )//����3��ر�esam
//        	{
//				APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 5;
//				APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 5;
//        	}
//        	else
//        	{
//				APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 120;
//				APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 120;
//        	}
//        }
    }
	
	#if(BOARD_TYPE == BOARD_HT_SINGLE_78201602)
	//��������ESAM��Դʱ����ƿ���, ESAM SPI��ģʽ3
	if( (ESAMPowerControlConst == YES) && (MAX_PHASE == 1) )
	{
		api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
	}
	
	Result = Link_ESAM_TxRx1( CmdLen, Cmd, BufLen, InBuf, OutBuf );
	
	//��������ESAM��Դʱ����ƿ���, Flash SPI��ģʽ0
	if( (ESAMPowerControlConst == YES) && (MAX_PHASE == 1) )
	{
		api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_0 );
	}
	#else
	Result = Link_ESAM_TxRx1( CmdLen, Cmd, BufLen, InBuf, OutBuf );
	#endif
	
    return Result;
}
//--------------------------------------------------
//��������:  ��λESAMspi
//         
//����  :   ��
//
//����ֵ:    ��  
//         
//��ע����:  ��
//--------------------------------------------------
void api_ResetEsamSpi( void )
{
	BYTE Buf[20],CmdBuf[6],Result;

	#if (SEL_ESAM_TYPE == 0)
	return;
	#else
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x36;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x01;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x00;
	
	ResetSPIDevice( eCOMPONENT_SPI_ESAM, 20 ); //��ʱ20ms
	
	#if(BOARD_TYPE == BOARD_HT_SINGLE_78201602)
	//��������ESAM��Դʱ����ƿ���, ESAM SPI��ģʽ3
	if( (ESAMPowerControlConst == YES) && (MAX_PHASE == 1) )
	{
		api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
	}
	
	Result = Link_ESAM_TxRx1(6,CmdBuf,0,NULL,Buf);
	
	//��������ESAM��Դʱ����ƿ���, Flash SPI��ģʽ0
	if( (ESAMPowerControlConst == YES) && (MAX_PHASE == 1) )
	{
		api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_0 );
	}
	#else
	Result = Link_ESAM_TxRx1(6,CmdBuf,0,NULL,Buf);
	#endif
	
	if( Result == FALSE )//���ʧ�ܽ���110ms��ʱ��λ
	{
		api_WriteSysUNMsg( ESAM_FIRST_RESET_ERR );//��¼ESAM��λʧ���쳣ʱ��
		ResetSPIDevice( eCOMPONENT_SPI_ESAM, 150 );
	}
	#endif //#if (SEL_ESAM_TYPE != 0)
}

#else
7816ͨ�ŷ�ʽ
#endif//#if(ESAM_COM_TYPE == TYPE_SPI)





#if( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//   ����: Send_Command
//   ����: ��Ƭ/ESAM���������ַ���
//   ����: EC=0��ʾ�Կ�Ƭ����;EC=1��ʾ��ESAM����
//	 CLA ����ֽ�
//	 INS ָ���ֽ�
//	 P1,P2 ָ����ض�����
//	 P3 ��INS�ı�����������Ǳ�ʾ�������͸�IC�������ݣ����ǵȴ���IC����Ӧ��������ݳ���
//   ���: N/A
//-----------------------------------------------
BYTE Send_Command(BYTE Cla,BYTE Ins,BYTE P1,BYTE P2,BYTE P3)
{
	BYTE Rins=0;
	
	api_OpenTxCard(Cla);
	api_SendCardByte(Ins);
	api_SendCardByte(P1);
	api_SendCardByte(P2);
	api_SendCardByte(P3);
	#if( MAX_PHASE == 3 )
	//�ж����һ���ֽ��Ƿ������ �����ж����һ���ֽ��Ƿ��ͳɹ� ��ȻתΪ���ջ�����Է����յ����
	if( api_UartIsSendEnd( 4 ) == FALSE )
	{
		return FALSE;
	}
	#endif
	api_OpenRxCard();
    api_ReceiveCardByte(&Rins);
	if(Rins!=Ins)
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��дCPU������
//
//����:		Cla	Ins P1 P2 P3 ����ͷ
//			RW[in]		����READ,д��WRITE
//			peerom[in]	д����߶���������ָ��
//			
//					
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL Link_Card_TxRx(BYTE Cla,BYTE Ins,BYTE P1,BYTE P2,BYTE P3,BYTE RW,BYTE *peerom)
{
	BYTE i=0,SW1,SW2,Result,RecResult;
	BOOL IntStatus;
	
	//�ȼ�¼������ͷ��������ʧ��ʱ��¼�쳣�忨��¼��
	CommandHead[0] = CARD;
	CommandHead[1] = Cla;
	CommandHead[2] = Ins;
	CommandHead[3] = P1;
	CommandHead[4] = P2;
	CommandHead[5] = P3;
	CommandHead[6] = 0x00;//Ϊ���ֺͼ�¼��ESAM�����ֳ���һ�£��˴���Ϊ0

	Result = FALSE;
	RecResult = FALSE;
	Err_Sw12[0] = 0;
	Err_Sw12[1] = 0;
	SW1 = 0xFF;
	SW2 = 0xFF;
	IntStatus = api_splx(0);
	
	if(Send_Command(Cla,Ins,P1,P2,P3)!=TRUE)
	{
		api_CloseCard();
		api_splx(IntStatus);
		return FALSE;
	}

	if( RW == READ )
	{
		api_OpenRxCard();
		for( i=0; i<P3; i++)
	  	{
			RecResult = api_ReceiveCardByte(peerom+i);
			if(RecResult == FALSE)
			{
				api_splx(IntStatus);
				return FALSE;
			}
	  	} 
	}
	else
	{	
		#if(MAX_PHASE == 3)
		api_Delayms( 1 ); //�˴���Ҫ��ʱ1ms ��Ȼ��ͽ��յ����һ�ֽ�������֡ �������ݷ���ʧ��
		#endif
		api_OpenTxCard(peerom[0]);
		for( i=0; i<(P3-1); i++)
		{
			api_SendCardByte(*(peerom+1));
			peerom++;
		}
		#if(MAX_PHASE == 3)
		//�ж����һ���ֽ��Ƿ������ �����ж����һ���ֽ��Ƿ��ͳɹ� ��ȻתΪ���ջ�����Է����յ����
		if( api_UartIsSendEnd( 4 ) == FALSE )
		{
			return FALSE;
		}
		#endif
	}
	i=0;
	do
	{
		api_OpenRxCard();
		RecResult = api_ReceiveCardByte(&SW1);
		if(i++>10)
		{
            break;
		}
	}while( RecResult==FALSE );
	
    api_ReceiveCardByte(&SW2);

	if((SW1==0x61)||(SW1==0x90))
	{
		Result = TRUE;
	}
	Err_Sw12[0] = SW1;
	Err_Sw12[1] = SW2;//��ȷ����9000 �� 6108
	api_splx(IntStatus);
	return Result;
}

//-----------------------------------------------
//��������: ��λ��Ƭ,��ȡ��λ��Ϣ
//
//����:		Buf���ظ�λ��Ϣ
//						
//����ֵ:	TRUE or FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL Link_CardReset( BYTE *Buf )
{
	BYTE i;
	BOOL IntStatus;
	BOOL Result;
	//BYTE Buf[20];
	
	//memset( Reset_Data, 0, sizeof(Reset_Data) );
	//IntStatus = api_splx(0);
	
	api_ResetCard();

	api_OpenRxCard();
	for(i=0; i<13; i++)
	{
		Result = api_ReceiveCardByte(Buf+i);
		if( Result == FALSE )
		{
			return FALSE;
		}
	}
	//api_splx(IntStatus);
	//
	if( ( Buf[0] == 0x3b ) && ( Buf[1] == 0x69 ) )
	{
		return TRUE;
	}
	api_CloseCard();
	return FALSE;
}

#endif//#if( PREPAY_MODE == PREPAY_LOCAL )


#endif//#if( ENCRYPT_MODE == ENCRYPT_ESAM )

#endif//#if( PREPAY_MODE != PREPAY_NO )

