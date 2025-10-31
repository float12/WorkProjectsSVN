//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������    ������
//��������	2017-03-02
//����		GD25Q64C�����ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "gd25q64c.h"
#include "SaveMem.h"

#if( (THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )

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
static BYTE WriteFlashErrorTimes;
static BYTE g_byFlashErrTimes;	//flash�洢�����ϴ���

//-----------------------------------------------
//��������: ��ѯоƬ�Ƿ���У��ڲ�����
//
//����: 
//			No[in]			оƬ��ţ�����Ƭѡѡ��
//                    
//����ֵ:  	TRUE:оƬ����   FALSE:оƬ����æ״̬
//
//��ע:   
//-----------------------------------------------
static WORD WaitFlashChipDoOver( BYTE No )
{
	BYTE Status;
	WORD i;
	
	CLEAR_WATCH_DOG;
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_RDSR);

	//���� ���ʱ350����ȴ� ��������5���ǰ���150ms 5��κ����300ms
	for(i=0; i<350; i++)
	{
		Status = api_UWriteSpi(eCOMPONENT_SPI_FLASH, 0);

		if( Status & 0x01 )//æµ״̬
		{
			;
		}
		else//�Ѿ�����
		{
			break;
		}
		
		api_Delayms(1);
	}

	DoSPICS(No, FALSE);

	CLEAR_WATCH_DOG;// 2014��5��19�պ���˵���������45db321����

	if( i>= 350 )
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
//��������: ����FLASH������ģ���ڲ�����
//
//����: 	
//			No[in]			д��оƬ���
//			Addr[in]		д�����ݵ���ʼ��ַ
//			Len[in]			д�����ݳ���
//			pBuf[in]		�洢Ҫд�������
//
//����ֵ:  	TRUE:�ɹ�		FALSE��ʧ��
//
//��ע:
//-----------------------------------------------
BOOL EraseExtFlashSector( WORD No, DWORD Addr )
{
//	ASSERT( (Addr % SECTOR_SIZE) == 0, 1 );

	//дʹ��
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_WREN);
	DoSPICS(No, FALSE);	//д����������CS���ߡ�
	
	//Ƭѡ
	DoSPICS(No, TRUE);
	//д������
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_SE);
	//д���ַ
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 16);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 8);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr);

	DoSPICS(No, FALSE);//д���ַ�����Ƭѡ���� ���򲻽��в�������
	
	if( WaitFlashChipDoOver( No ) != TRUE )
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��FlashоƬ���жϵ�ǿ�Ƹ�λ��ģ���ں���
//
//����: 	
//          Type	��λ����  0���ϵ�     ���������ϵ���
//          
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void PowerUpResetFlash( BYTE Type )
{
	if( Type == 0 )
	{
		WriteFlashErrorTimes = 0;
	}
	
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 );
}

//-----------------------------------------------
//��������: ��������£���λFlashоƬ���ڲ�����
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:��Щ����оƬ�ĸ�λ�ܽźͲ���оƬ�ĸ�λ�ܽ�����һ����   
//-----------------------------------------------
static void ResetFlash( void )
{
	/** ��λ **/
	/*
	//оƬ��λ��ʹ��ǰ��д������ֹ���Ƿ���Ҫ�ڸ�λǰ���оƬ��æµ״̬��
	//ʹ�ܸ�λ
	DoSPICS(No, TRUE);
	api_UWriteSpi(GD25Q64C_CMD_ER);
	DoSPICS(No, FALSE);	//д����������CS����
	
	//��λоƬ
	DoSPICS(No, TRUE);
	api_UWriteSpi(GD25Q64C_CMD_RE);
	DoSPICS(No, FALSE);	//д����������CS����
	*/
	if( g_byFlashErrTimes < 200 )
	{
		g_byFlashErrTimes ++;
		if( g_byFlashErrTimes == 200 )
		{
//			api_SetFollowReportStatus(eSTATUS_EEPROM_Error);
		}
	}	
	/** Ӳ��λ **/
	//WriteDB161ErrorTimes==0xff˵��оƬ�д��󣬲����ٸ�λ
	if( WriteFlashErrorTimes == 0xff )
	{
		return;
	}

	WriteFlashErrorTimes++;

	if( WriteFlashErrorTimes >= (FLASH_MAX_WRITE_COUNT+4) )
	{
		WriteFlashErrorTimes = 0xff;//˵��оƬ�����⣬�����ٸ�λ��
//		api_SetError( ERR_WRITE_EEPROM3 );
	}
	else
	{
    	PowerUpResetFlash( 1 );
	}
}

//-----------------------------------------------
//��������: д��ĳ��ҳ�����ܿ�ҳ�������ڲ�����
//
//����:
//			No[in]			оƬ��ţ�����Ƭѡѡ��
//			Addr[in]		������ַ
//			Len[in]			д�볤�ȣ�ע�ⲻҪ����1��������
//			pBuf[in/out]	д������ݻ���
//
//����ֵ:  	TRUE:�ɹ�		FALSE:ʧ��
//
//��ע:
//-----------------------------------------------
static BOOL WriteFlashPage( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	BYTE CmpData[GD25Q64C_PAGE_SIZE];
	DWORD i;
	
//	ASSERT( (Addr+Len) <= THIRD_MEM_SPACE, 1 );
	
	if(WaitFlashChipDoOver( No ) != TRUE)
	{
		return FALSE;
	}
	
	//�����ַΪ��������ʼ��ַ ������������
	if( (Addr % SECTOR_SIZE) == 0 )
	{
		if( EraseExtFlashSector( No, Addr ) != TRUE )//��������2������
		{
			EraseExtFlashSector( No, Addr );
		}
	}
	
	//дʹ��
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_WREN);
	DoSPICS(No, FALSE);	//д����������CS����
	
	//д��ַ
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_PP);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 16);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 8);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr);

	//д����
	for( i = 0; i < Len; i++ )
	{
		api_UWriteSpi(eCOMPONENT_SPI_FLASH, pBuf[i]);
	}
	DoSPICS(No, FALSE);
	
	//��ֹд
	DoSPICS(No, TRUE);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_WRDI);
	DoSPICS(No, FALSE);	//д����������CS����
	
	//����Ƿ�д��
	if( WaitFlashChipDoOver( No ) != TRUE )
	{
		return FALSE;
	}
	
	//�ض��Ƚ�
	if( ReadExtFlash( No, Addr, Len, CmpData ) == TRUE )
	{
		if( memcmp( pBuf, CmpData, Len ) != 0 )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��FlashоƬ����ҳд�����������ԣ��ڲ�����
//
//����: 	
//			No[in]			оƬ��ţ�����Ƭѡѡ��
//			Addr[in]		������ַ
//			Len[in]			д�볤�ȣ�ע�ⲻҪ����1��������
//			pBuf[in/out]	д������ݻ���
//
//����ֵ:  	TRUE:�ɹ�		FALSE:ʧ��
//
//��ע:
//-----------------------------------------------
static BOOL WriteFlashPageLogic( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	DWORD i;
	
	for(i=0; i<FLASH_MAX_WRITE_COUNT; i++)
	{
		if( WriteFlashPage(No, Addr, Len, pBuf) == TRUE )
		{
			WriteFlashErrorTimes = 0;
			g_byFlashErrTimes = 0;
//			api_ClrError( ERR_WRITE_EEPROM3 );

			return TRUE;
		}
		else
		{
			//��¼�쳣�¼���¼
//			api_WriteSysUNMsg(SYSUN_FALSE_REPET_ERR);
		}
		
		//��λоƬ
		ResetFlash();
	}
	
	return FALSE;
}

//---------------------------------------------------------------
//��������: ��ȡһҳ����
//
//����:
//			No[in]			оƬ��ţ�����Ƭѡѡ��
//			Addr[in]		�����ݵ���ʼ��ַ
//			Len[in]			��������
//			pBuf[in/out]	�������ݴ洢����
//
//����ֵ:  	TRUE:��ȷ����   FALSE:��������  ʵ��û���жϣ�����������ȷ 
//
//��ע:   
//---------------------------------------------------------------
BOOL ReadFlashPage( WORD No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	WORD i;
	
//	ASSERT( (Addr+Len) <= THIRD_MEM_SPACE, 1 );
	
	//Ƭѡ
	DoSPICS(No, TRUE);
	//д������
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, GD25Q64C_CMD_READ);
	//д���ַ
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 16);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr >> 8);
	api_UWriteSpi(eCOMPONENT_SPI_FLASH, Addr);

	//�������Դ˴��Ƿ���Ҫ��ʱ
	//��ʼ������
	for(i=0; i<Len; i++)
	{
		pBuf[i] = api_UWriteSpi(eCOMPONENT_SPI_FLASH, 0);
	}

	DoSPICS( No, FALSE );

	return TRUE;
}
//-----------------------------------------------
//��������: ��FlashоƬ�ڶ�ȡ���ݣ�ģ���ں���
//
//����:
//			No[in]			оƬ��ţ�����Ƭѡѡ��
//			Addr[in]		�����ݵ���ʼ��ַ
//			Len[in]			��������
//			pBuf[in/out]	�������ݴ洢����
//
//����ֵ:  	TRUE:��ȷ����   FALSE:��������  ʵ��û���жϣ�����������ȷ
//
//��ע:
//-----------------------------------------------
BOOL ReadExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf)
{
	BYTE i,j,Times;
	BYTE ReadBuf[GD25Q64C_PAGE_SIZE];
	WORD DoLength;
		
	Times = Len/sizeof(ReadBuf);
	Times += 1;
	
	for(i=0; i<Times; i++)
	{
		if( i == (Times-1) )//���һ��
		{
			DoLength = Len-(WORD)sizeof(ReadBuf)*(Times-1);
		}
		else
		{
			DoLength = sizeof(ReadBuf);
		}
		
		for(j=0; j<2; j++)//�������2��
		{
			ReadFlashPage(No, Addr, DoLength, pBuf);
			ReadFlashPage(No, Addr, DoLength, ReadBuf);
			if( memcmp( pBuf, ReadBuf, DoLength ) == 0 )//�����ν��бȽϣ�һ������Ϊ������ȷ
			{
				break;
			}
		}
		
//		ASSERT( j<2, 0 );
		
		Addr += DoLength;
		pBuf += DoLength;
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: дFlash�������ⲿ����
//
//����: 
// 			No[in]			оƬ��ţ�����Ƭѡѡ��
//			Addr[in]		������ַ
//			Len[in]			д�볤�ȣ�ע�ⲻҪ����1��������
//			pBuf[in/out]	д������ݻ���                  
//����ֵ:  TRUE:�ɹ�		FALSE:ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL WriteExtFlash( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	volatile DWORD TrueAddr;
	short WriteLen;
	WORD PageLen;
	
	// ��ַȡ��
	TrueAddr = (Addr / GD25Q64C_PAGE_SIZE) * GD25Q64C_PAGE_SIZE;
	
	// ��һ��ҳ�����ݳ���
	PageLen = GD25Q64C_PAGE_SIZE - (WORD)(Addr - TrueAddr);
	
	// Ҫд�����ݵĳ���
	WriteLen = (short)Len;
	
	// �Ƿ��������ݶ���һ��������
	if( PageLen > Len )
	{
		PageLen = Len;
		WriteLen -= PageLen;
	}
	
	// д�����ݵ���ʼ��ַ
	TrueAddr = Addr;
	for(;;)
	{
		// д��һ��ҳ
		if( WriteFlashPageLogic(No, TrueAddr, PageLen, pBuf) != TRUE )
		{
			return FALSE;
		}
		
		// �۲��Ƿ�д�����
		WriteLen -= PageLen;
		if( WriteLen <= 0 )
		{
			// �Ѿ�д��
			break;
		}

		// ������ַ
		TrueAddr += PageLen;
		pBuf += PageLen;

		// ���㳤��
		if( WriteLen >= GD25Q64C_PAGE_SIZE )
		{
			PageLen = GD25Q64C_PAGE_SIZE;
		}
		else
		{
			PageLen = WriteLen;
		}
	}
	return TRUE;
}


#endif//#if( (THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )

