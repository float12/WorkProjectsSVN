//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.7.30
//����		����оƬHT7017�ײ������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7053D.h"

#if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

extern TTimeTem					TF415TimeTem;				//����ʱ����¶�
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
//��������: ��HT7017�Ĵ���
//
//����:		Addr[in]	��ַ	 
//			pBuf[out]	�������� 4�ֽ� ���ں󣬵���ǰ,��4�ֽڹ̶���Ϊ0
//����ֵ: 	
//		    TRUE/FALSE
//��ע: 
//-----------------------------------------------
BOOL Link_ReadSampleReg(BYTE Addr, BYTE *pBuf)
{
	BYTE i,j;
	BYTE tempaddr;
	WORD Sum;
	TTwoByteUnion SampleComBuf;

	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );
	//������ʱ3us!!!!!!
	api_Delay10us( 1 );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Addr );

	api_Delay10us( 1 );//�ٶȴ���200kʱ��Ҫ��ʱ3us�ٶ�����

	pBuf[3] = 0;
	pBuf[2] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	
	//����У��
	Sum = pBuf[0]+pBuf[1]+pBuf[2] + Addr;
	DoSPICS(CS_SPI_SAMPLE, FALSE );
    api_Delay10us( 1 );//�ٶȴ���200kʱ��Ҫ��ʱ3us�ٶ�����
	//��ȡУ��
    api_Delay10us( 3 );
	DoSPICS( CS_SPI_SAMPLE, TRUE );
	api_Delay10us( 1 );
	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, COMCHECKSUM );
	api_Delay10us( 3 );//�ٶȴ���200kʱ��Ҫ��ʱ3us�ٶ�����
    tempaddr = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	SampleComBuf.b[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	SampleComBuf.b[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );

	DoSPICS(CS_SPI_SAMPLE, FALSE );
	api_Delay10us( 1 );
	// �Ƚ�У��
	if (Sum == SampleComBuf.w )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//HT���������Ͳ������ã���˲���Ӱ�죻
	//ST����������һ·SPIʱ����λESAM��FLASH������
	//ST��������FLASH����һ·SPIʱӦ��λFLASH��SPI����λESAM-SPI�ᵼ��FLASHдʧ��
}
//-----------------------------------------------
//��������: дHT7017�Ĵ���
//
//����:		Addr[in]	��ַ	 
//			pBuf[in]	д������ 2�ֽ� ���ں󣬵���ǰ
//����ֵ: 	
//		    TRUE/FALSE
//��ע: 
//-----------------------------------------------
BOOL Link_WriteSampleReg(BYTE Addr, BYTE *pBuf)
{
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
	
	DoSPICS( CS_SPI_SAMPLE, TRUE );//Ƭѡ
	//������ʱ3us!!!!!!
	api_Delay10us( 1 );

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80+Addr );//����

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x00);//����

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,pBuf[1]);

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,pBuf[0]);

	DoSPICS( CS_SPI_SAMPLE, FALSE);

	api_Delay10us( 2 );
    
    return TRUE;

	//������Ӧ����д���ݵ�У��
}

//-----------------------------------------------
//��������: д7053D�Ĵ������ٴ����������
//
//����:		��
//����ֵ: 	��
//		    
//��ע: 
//-----------------------------------------------
void Link_OpenSampleUartReg( WORD Len )
{
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
	
	DoSPICS( CS_SPI_SAMPLE, TRUE );//Ƭѡ

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0xc6);//����

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x00);//����

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80);

	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,(BYTE)(Len-1));
    
	DoSPICS( CS_SPI_SAMPLE, FALSE);
	api_Delay10us( 2 );
}
////-----------------------------------------------
////��������: д7053D���ٵڶ��������ã�HUART_CFG������
////
////����:		��
////����ֵ: 	��
////		    
////��ע: 
////-----------------------------------------------
//void Link_OpenSampleUartReg( WORD Len )
//{
//	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
//	
//	DoSPICS( CS_SPI_SAMPLE, TRUE );//Ƭѡ
//
//	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80+0xc6);//����
//
//	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x00);//����
//
//	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80);
//
//	api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,(BYTE)(Len-1));
//	DoSPICS( CS_SPI_SAMPLE, FALSE);
//	api_Delay10us( 2 );
//
//	// DoSPICS( CS_SPI_SAMPLE, TRUE );//Ƭѡ
//	// // api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x80+0x4A);//����
//	// // api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x62);//����
//	// // api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x08);//����
//	// // api_UWriteSpi(eCOMPONENT_SPI_SAMPLE,0x62);//����
//	// DoSPICS( CS_SPI_SAMPLE, FALSE);
//
//	// api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
//	
//	// api_Delay10us( 2 );
//    
//}
//-----------------------------------------------
//��������: ��HT7017�������������
//
//����:		CH[in]	 
//					CH=0xbc,35H~45H,4AH~4FHд����
//					CH=0xa6,50H~7EHд����
//					CH=0x00,д��ֹ          
//����ֵ: 	��
//		   
//��ע: 
//-----------------------------------------------
void SampleSpecCmdOp(BYTE Cmd)
{
	BYTE Buf[2];
	
	Buf[0] = Cmd;
	Buf[1] = 0x00;

	Link_WriteSampleReg(0x32,Buf);
}

//---------------------------------------------------------------
//��������: ��λ����оƬ
//
//����:   ��
//
//����ֵ: ��
//
//��ע: SRSTREG �Ĵ������д�� 0x55 �ᵼ��оƬ������λ����λ��üĴ�����0
//---------------------------------------------------------------
void SoftResetSample( void )
{
	BYTE Buf[2];

	Buf[0] = 0x55;
	Buf[1] = 0x00;
	
	Link_WriteSampleReg( 0x33, Buf );
	api_Delayms( 4 );	 //�ֲ�˵�� ��λ����Ҫ�ȴ� 2ms �ſ��Բ����Ĵ�����
}
//---------------------------------------------------------------
//��������: Ӳ��λ����оƬ
//
//����:   ��
//
//����ֵ: ��
//
//��ע: Ӳ��λֻ֧��Ӳ��֧�ֵ������ʹ��
//---------------------------------------------------------------
void HardResetSample( void )
{
	POWER_SAMPLE_CLOSE;
	api_Delayms( 4 );
	POWER_SAMPLE_OPEN;
}
#endif// #if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )
