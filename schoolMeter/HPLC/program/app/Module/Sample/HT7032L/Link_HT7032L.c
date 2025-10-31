//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.8
//����		����оƬHT7038ͨ����·���ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7032L.h"

#if(SAMPLE_CHIP==CHIP_HT7032L)

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


//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ��HT7038�Ĵ���
//
//����:		Addr[in]	��ַ	 
//			pBuf[out]	�������� 4�ֽ� ���ں󣬵���ǰ,��4�ֽڹ̶���Ϊ0
//����ֵ: 	
//		    TRUE/FALSE
//��ע: 
//-----------------------------------------------
BOOL Link_ReadSampleReg(WORD Addr, BYTE *pBuf)
{
	BYTE Buf[7]={0};
	Buf[0] = ((Addr | 0x6000) >> 8);
	Buf[1] = Addr & 0xFF;
	
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[0] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[1] );

	api_Delay10us( 1 );//�ٶȴ���200kʱ��Ҫ��ʱ3us�ٶ�����

	pBuf[3] = Buf[2] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[2] = Buf[3] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[1] = Buf[4] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[0] = Buf[5] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	Buf[6] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	DoSPICS(CS_SPI_SAMPLE, FALSE );
	//HT���������Ͳ������ã���˲���Ӱ�죻
	//ST����������һ·SPIʱ����λESAM��FLASH������
	//ST��������FLASH����һ·SPIʱӦ��λFLASH��SPI����λESAM-SPI�ᵼ��FLASHдʧ��
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 1 );
    
	if(Buf[6] ==(BYTE)( ~(lib_CheckSum(&Buf[0],6))))
	{
		return TRUE;
	}
	else
	{
		memset( pBuf, 0x00, 4);
		return FALSE;
	}
	
}

//-----------------------------------------------
//��������: дHT7038�Ĵ���
//
//����:		Addr[in]	��ַ	 
//			pBuf[in]	д������ 3�ֽ� ���ں󣬵���ǰ
//����ֵ: 	
//		    
//��ע: 
//-----------------------------------------------
static void Link_WriteSampleReg(WORD Addr, BYTE *pBuf)
{
	BYTE Buf[7];
	
	Buf[0] = ((Addr | 0x8000) >> 8);
	Buf[1] = Addr & 0xFF;
	Buf[2] = pBuf[3];
	Buf[3] = pBuf[2];
	Buf[4] = pBuf[1];
	Buf[5] = pBuf[0];
	Buf[6] = ~(lib_CheckSum(&Buf[0],6));
	
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
	
	DoSPICS( CS_SPI_SAMPLE, TRUE );//Ƭѡ

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[0] );
	
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[1] );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[2] );//����
	
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[3] );//����

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[4] );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[5] );
	
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[6] );

	DoSPICS( CS_SPI_SAMPLE, FALSE);
	
	//HT���������Ͳ������ã���˲���Ӱ�죻
	//ST����������һ·SPIʱ����λESAM��FLASH������
	//ST��������FLASH����һ·SPIʱӦ��λFLASH��SPI����λESAM-SPI�ᵼ��FLASHдʧ��
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 2 );
}
//-----------------------------------------------
//��������: ������HT7038�Ĵ���
//
//����:		Addr[in]	��ַ	 
//			Num[in]		�Ĵ�������	
//			pBuf[out]	�������� 4�ֽ� ���ں󣬵���ǰ,��4�ֽڹ̶���Ϊ0
//����ֵ: 	
//		    TRUE/FALSE
//��ע: 
//-----------------------------------------------
BOOL Link_ReadSampleRegCont(WORD Addr, WORD Num, DWORD *pBuf)
{
	BYTE Buf[4]={0};
	WORD i;
	DWORD Cmd;
	BYTE Cks=0;
	
	if((Addr>0xFFF) || ( ( Addr+Num-1) > 0xFFF))//��ַ����
	{
		return FALSE;
	}
	if(Num >0x1FF)//���ȳ���
	{
		return FALSE;
	}
	Cmd = ((1<<21) | ((Num-1)<<12) | (Addr&0xFFF));	//32bit
	
	Buf[0] = (( Cmd>>16) & 0xFF);
	Buf[1] = (( Cmd>>8) & 0xFF);
	Buf[2] = (Cmd & 0xFF);
	Cks= Buf[0]+Buf[1]+Buf[2];
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[0] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[1] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[2] );

	api_Delay10us( 1 );//�ٶȴ���200kʱ��Ҫ��ʱ3us�ٶ�����
	
	for(i=0;i<Num;i++)
	{
		Buf[3] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		Buf[2] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		Buf[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		Buf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		memcpy(&pBuf[i],Buf,4);
	}
	Buf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	
	DoSPICS(CS_SPI_SAMPLE, FALSE );
	//HT���������Ͳ������ã���˲���Ӱ�죻
	//ST����������һ·SPIʱ����λESAM��FLASH������
	//ST��������FLASH����һ·SPIʱӦ��λFLASH��SPI����λESAM-SPI�ᵼ��FLASHдʧ��
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 1 );
    Cks += lib_CheckSum((BYTE*)pBuf,(Num*4));
	if(Cks == (BYTE)(~Buf[0]))
	{
		return TRUE;
	}
	else
	{
		memset( pBuf, 0x00, (Num*4));
		return FALSE;
	}
	
}

//-----------------------------------------------
//��������: ������HT7038�Ĵ���
//
//����:		Addr[in]	��ַ	 
//			Num[in]		�Ĵ�������	
//			pBuf[out]	�������� 4�ֽ� ���ں󣬵���ǰ,��4�ֽڹ̶���Ϊ0
//����ֵ: 	
//		    TRUE/FALSE
//��ע: 
//-----------------------------------------------
BOOL ReadSampleBuffCont(WORD Addr, WORD Num, SWORD *pBuf)
{
	BYTE Buf[4]={0};
	WORD i;
	DWORD Cmd;
	BYTE Cks=0;
	
	if((Addr>0xFFF) || ( ( (Addr+Num/2)-1) > 0xFFF))//��ַ����
	{
		return FALSE;
	}
	if((Num/2) >0x1FF)//���ȳ���
	{
		return FALSE;
	}
	Cmd = ((1<<21) | (((Num/2)-1)<<12) | (Addr&0xFFF));	//32bit
	
	Buf[0] = (( Cmd>>16) & 0xFF);
	Buf[1] = (( Cmd>>8) & 0xFF);
	Buf[2] = (Cmd & 0xFF);
	Cks= Buf[0]+Buf[1]+Buf[2];
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[0] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[1] );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Buf[2] );

	api_Delay10us( 1 );//�ٶȴ���200kʱ��Ҫ��ʱ3us�ٶ�����
	
	for(i=0;i<Num;i++)
	{
		Buf[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		Buf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
		memcpy(&pBuf[i],Buf,2);
	}
	Buf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	
	DoSPICS(CS_SPI_SAMPLE, FALSE );
	//HT���������Ͳ������ã���˲���Ӱ�죻
	//ST����������һ·SPIʱ����λESAM��FLASH������
	//ST��������FLASH����һ·SPIʱӦ��λFLASH��SPI����λESAM-SPI�ᵼ��FLASHдʧ��
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 1 );
    Cks += lib_CheckSum((BYTE*)pBuf,(Num*2));
	if(Cks == (BYTE)(~Buf[0]))
	{
		return TRUE;
	}
	else
	{
		memset( pBuf, 0x00, (Num*2));
		return FALSE;
	}
	
}

////-----------------------------------------------
////��������: �жϵ�ǰоƬ���ڵ�״̬ �ǿ��Զ��������ݻ���У������
////
////����:		��	 
////			
////����ֵ: 	TRUE--���ڶ���������
////		    FALSE--���ڶ�У������
////��ע: 
////-----------------------------------------------
//static BOOL JudgeReadStatus( void )
//{
//	DWORD tdw;
//
//	Link_ReadSampleReg(0,(BYTE*)&tdw);
//
//	if( tdw == 0xaaaa )
//	{
//		return FALSE;
//	}
//	else
//	{
//		return TRUE;
//	}
//}
//
//
////-----------------------------------------------
////��������: �л������������ݼĴ���
////
////����:		��	 
////			
////����ֵ: 	��
////		    
////��ע: 
////-----------------------------------------------
//static void SwitchToReadMeasureData( void )
//{
//	BYTE i;
//	DWORD Data;
//	
//	for(i=0;i<3;i++)
//	{
//		if( TRUE == JudgeReadStatus() )
//		{
//			break;
//		}
//
//		//��Link_WriteSampleReg��д��ַʱ��80�����Դ˴���80
//		Data = 0xa5a5a5;
//		Link_WriteSampleReg( 0xc6-0x80, (BYTE*)&Data );
//	}
//}
//
//
////-----------------------------------------------
////��������: �л�����У�����ݼĴ���
////
////����:		��	 
////			
////����ֵ: 	��
////		    
////��ע: 
////-----------------------------------------------
//static void SwitchToReadCalibrateData( void )
//{
//	BYTE i;
//	DWORD Data;
//	
//	for(i=0;i<3;i++)
//	{
//		if( FALSE == JudgeReadStatus() )
//		{
//			break;
//		}
//
//		//��Link_WriteSampleReg��д��ַʱ��80�����Դ˴���80
//		Data = 0x00005a;
//		Link_WriteSampleReg( 0xc6-0x80, (BYTE*)&Data );
//	}
//}

//-----------------------------------------------
//��������: ���������ݼĴ���
//
//����:		Addr[in] �Ĵ�����ַ	 
//			
//����ֵ: 	���ض��������ݣ�����ֽڹ̶�Ϊ0
//		    
//��ע: 
//-----------------------------------------------
DWORD ReadSampleData( WORD Addr )
{
	DWORD tdw,tdw1;
	Link_ReadSampleReg(Addr,(BYTE*)&tdw);
	//�ض�
	Link_ReadSampleReg(r_BckReg,(BYTE*)&tdw1);

	return tdw;
}


////-----------------------------------------------
////��������: ��У�����ݼĴ���
////
////����:		Addr[in] �Ĵ�����ַ	 
////			
////����ֵ: 	���ض��������ݣ�����ֽڹ̶�Ϊ0
////		    
////��ע: 
////-----------------------------------------------
//DWORD ReadCalibrateData( WORD Addr )
//{
//	DWORD tdw,tdw1;
//
//	Link_ReadSampleReg(Addr,(BYTE*)&tdw);
//	//�ض�
//	Link_ReadSampleReg(r_BckReg,(BYTE*)&tdw1);
//
//	return tdw;
//}


//-----------------------------------------------
//��������: дУ�����ݼĴ���
//
//����:		Addr[in] �Ĵ�����ַ	 
//			Data[in] Ҫд�������,д��������ֽڣ�����ֽ�û��
//����ֵ: 	TRUE/FALSE
//		    
//��ע: 
//-----------------------------------------------
BOOL WriteSampleReg( WORD Addr, DWORD Data )
{
	DWORD tdw;
	BYTE RepeatTime;
	WORD Status;

	for(RepeatTime=0; RepeatTime<3; RepeatTime++)
	{
		Link_WriteSampleReg( Addr, (BYTE*)&Data );

		//�ض�
		Link_ReadSampleReg(r_BckReg,(BYTE*)&tdw);

		if( tdw != Data )
		{
			Status = FALSE;
		}
		else
		{
			Status = TRUE;
			break;
		}
	}

	return Status;
}

//-----------------------------------------------
//��������: ��HT7038�������������
//
//����:		Cmd[in]	 
//					CH=0xa6,д�����뵥���ͳһ������
//					����У��Ĵ���������д�������أ�����һ�����ؿ��Կ����мĴ���
//					CH=0x00,д��ֹ          
//����ֵ: 	��
//		   
//��ע: 
//-----------------------------------------------
void SampleSpecCmdOp(BYTE Cmd)
{
	if( Cmd == 0x00 )
	{
		WriteSampleReg(w_EMUWPREG, ~0xA99A);
	}
	else if( Cmd == 0xa6 )
	{
        WriteSampleReg(w_EMUWPREG, 0xA99A);
	}
	else
	{
		;
	}
}

#endif//#if(SAMPLE_CHIP==CHIP_HT7032L)


