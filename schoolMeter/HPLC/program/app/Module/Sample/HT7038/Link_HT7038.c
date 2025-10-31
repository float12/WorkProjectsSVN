//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.8
//����		����оƬHT7038ͨ����·���ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT7038.h"

#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )

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
BOOL Link_ReadSampleReg(BYTE Addr, BYTE *pBuf)
{
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );

	DoSPICS( CS_SPI_SAMPLE, TRUE );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, Addr );

	api_Delay10us( 1 );//�ٶȴ���200kʱ��Ҫ��ʱ3us�ٶ�����

	pBuf[3] = 0;
	pBuf[2] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[1] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );
	pBuf[0] = api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0xff );

	DoSPICS(CS_SPI_SAMPLE, FALSE );
	//HT���������Ͳ������ã���˲���Ӱ�죻
	//ST����������һ·SPIʱ����λESAM��FLASH������
	//ST��������FLASH����һ·SPIʱӦ��λFLASH��SPI����λESAM-SPI�ᵼ��FLASHдʧ��
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 1 );
	
	return TRUE;
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
static void Link_WriteSampleReg(BYTE Addr, BYTE *pBuf)
{
	api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
	
	DoSPICS( CS_SPI_SAMPLE, TRUE );//Ƭѡ

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, 0x80+Addr );//����

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, pBuf[2] );//����

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, pBuf[1] );

	api_UWriteSpi( eCOMPONENT_SPI_SAMPLE, pBuf[0] );

	DoSPICS( CS_SPI_SAMPLE, FALSE);
	
	//HT���������Ͳ������ã���˲���Ӱ�죻
	//ST����������һ·SPIʱ����λESAM��FLASH������
	//ST��������FLASH����һ·SPIʱӦ��λFLASH��SPI����λESAM-SPI�ᵼ��FLASHдʧ��
	api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
	
	api_Delay10us( 2 );
}


//-----------------------------------------------
//��������: �жϵ�ǰоƬ���ڵ�״̬ �ǿ��Զ��������ݻ���У������
//
//����:		��	 
//			
//����ֵ: 	TRUE--���ڶ���������
//		    FALSE--���ڶ�У������
//��ע: 
//-----------------------------------------------
static BOOL JudgeReadStatus( void )
{
	DWORD tdw;

	Link_ReadSampleReg(0,(BYTE*)&tdw);

	if( tdw == 0xaaaa )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


//-----------------------------------------------
//��������: �л������������ݼĴ���
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע: 
//-----------------------------------------------
static void SwitchToReadMeasureData( void )
{
	BYTE i;
	DWORD Data;
	
	for(i=0;i<3;i++)
	{
		if( TRUE == JudgeReadStatus() )
		{
			break;
		}

		//��Link_WriteSampleReg��д��ַʱ��80�����Դ˴���80
		Data = 0xa5a5a5;
		Link_WriteSampleReg( 0xc6-0x80, (BYTE*)&Data );
	}
}


//-----------------------------------------------
//��������: �л�����У�����ݼĴ���
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע: 
//-----------------------------------------------
static void SwitchToReadCalibrateData( void )
{
	BYTE i;
	DWORD Data;
	
	for(i=0;i<3;i++)
	{
		if( FALSE == JudgeReadStatus() )
		{
			break;
		}

		//��Link_WriteSampleReg��д��ַʱ��80�����Դ˴���80
		Data = 0x00005a;
		Link_WriteSampleReg( 0xc6-0x80, (BYTE*)&Data );
	}
}


//-----------------------------------------------
//��������: ���������ݼĴ���
//
//����:		Addr[in] �Ĵ�����ַ	 
//			
//����ֵ: 	���ض��������ݣ�����ֽڹ̶�Ϊ0
//		    
//��ע: 
//-----------------------------------------------
DWORD ReadMeasureData( BYTE Addr )
{
	DWORD tdw;

	SwitchToReadMeasureData();

	Link_ReadSampleReg(Addr,(BYTE*)&tdw);

	return tdw;
}


//-----------------------------------------------
//��������: ��У�����ݼĴ���
//
//����:		Addr[in] �Ĵ�����ַ	 
//			
//����ֵ: 	���ض��������ݣ�����ֽڹ̶�Ϊ0
//		    
//��ע: 
//-----------------------------------------------
DWORD ReadCalibrateData( BYTE Addr )
{
	DWORD tdw;

	SwitchToReadCalibrateData();

	Link_ReadSampleReg(Addr,(BYTE*)&tdw);

	return tdw;
}


//-----------------------------------------------
//��������: дУ�����ݼĴ���
//
//����:		Addr[in] �Ĵ�����ַ	 
//			Data[in] Ҫд�������,д��������ֽڣ�����ֽ�û��
//����ֵ: 	TRUE/FALSE
//		    
//��ע: 
//-----------------------------------------------
BOOL WriteSampleReg( BYTE Addr, DWORD Data )
{
	DWORD tdw;
	BYTE RepeatTime;
	WORD Status;

	//��ΪͨѶ���ݼĴ����ǲ�����Ĵ��� ������Ҫ�л�����������Ĵ���
	SwitchToReadMeasureData();
	
	for(RepeatTime=0; RepeatTime<3; RepeatTime++)
	{
		Link_WriteSampleReg( Addr, (BYTE*)&Data );

		//˵��������������ܻض�
		if( (Addr>=0x40) && (Addr<0x60) )
		{
			Status = TRUE;
			break;
		}

		//�ض�
		Link_ReadSampleReg(r_BckReg,(BYTE*)&tdw);

		if( tdw != (Data&0xffffff) )
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
		WriteSampleReg(0xc9-0x80, 0xa5a5a5 );
	}
	else if( Cmd == 0xa6 )
	{
		WriteSampleReg(0xc9-0x80, 0x00005a );
	}
	else
	{
		;
	}
}

#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )


