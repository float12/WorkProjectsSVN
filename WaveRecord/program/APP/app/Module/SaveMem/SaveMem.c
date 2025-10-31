//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.10
//����		�洢ģ��Ľӿ��ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "gd25q64c.h"
#include "SaveMem.h"

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

//--------------------------------------------------
//��������:  �Ի����ڵ�����ȡ��
//
//����:      p[in] 			���ݵĻ���
//
//           Length[in]		���ݳ���
//
//����ֵ:
//
//��ע����:  ��
//--------------------------------------------------
void ReverseInData( BYTE *p, WORD Length )
{
	WORD i;
	for( i = 0; i < Length; i++ )
	{
		p[i] ^= 0xff;
	}
}

//-----------------------------------------------
//��������: ����ȫ�ռ�,Ҫ�󳤶ȱ����ǽṹ�ĳ��ȣ�����У�����ڣ�
//			���Ұ�ȫ�ռ�ĵڶ���һ����ȡ���洢��У��̶���Ϊ��CRC32
//
//����: 
//			Addr[in]				��ʼ��ַ
//			Length[in]				���ݳ��ȣ������ṹ�ĳ��ȣ�����У��
//			pBuf[in/out]			��Ŷ��������ݣ�����У��
//                    
//����ֵ:  	TRUE:		��һ�ݶ�����Ϊ�Ƕ�
//			FALSE��		���ݶ�����
//��ע:   
//-----------------------------------------------
WORD api_VReadSafeMem( WORD Addr, WORD Length, BYTE * pBuf )
{
    if( (Length == 0) || (Length > 400) )//�ֿ��Сд�벻����Ϊ400�ֽ�
    {
        return FALSE;
    }
	
	if( ReadEEPRom1(Addr, Length, pBuf) != FALSE )
	{
		//��ֹReadEEPRom1��EE����δ�������� ���Ǵ��뻺��ԭֵCRC��ȷ����ȡEE2
		if( lib_CheckSafeMemVerify( pBuf, Length, UN_REPAIR_CRC ) == TRUE )
		{
			return TRUE;
		}
	}
	
	if( ReadEEPRom2(Addr, Length, pBuf) != FALSE )
	{
		ReverseInData( pBuf, Length );
	
		if( lib_CheckSafeMemVerify( pBuf, Length, UN_REPAIR_CRC ) == TRUE )
		{
			return TRUE;
		}
	}
		
	return FALSE;
}


//-----------------------------------------------
//��������: д��ȫ�ռ�,Ҫ�󳤶ȱ����ǽṹ�ĳ��ȣ�����У�����ڣ�
//			���Ұ�ȫ�ռ�ĵڶ�����ȡ���洢,У��̶���Ϊ��CRC32�����У�鲻�ԣ��ú������Զ����
//
//����: 
//			Addr[in]				��ʼ��ַ
//			Length[in]				���ݳ��ȣ������ṹ�ĳ��ȣ�����У��
//			pBuf[in/out]			��Ŷ��������ݣ�����У��
//                    
//����ֵ:  	TRUE:��ȷд������������	FALSE:��һ�ݻ���������û����ȷд��
//
//��ע:   
//-----------------------------------------------
WORD api_VWriteSafeMem( WORD Addr, WORD Length, BYTE * pBuf )
{
	WORD Result[2];
	BYTE Buf[400+30];

	if(  (Length == 0) || (Length > 400) )//�ֿ��Сд�벻����Ϊ400�ֽ�
    {
        return FALSE;
    }
	
	lib_CheckSafeMemVerify( pBuf, Length, REPAIR_CRC );
	memcpy( Buf, pBuf, Length );
	Result[0] = WriteEEPRom1( Addr, Length, pBuf );
	
	ReverseInData( Buf, Length );
	Result[1] = WriteEEPRom2( Addr, Length, Buf );
	
	//ֻ����ƬEEȫ��д�����ŷ��ش��� ���������������ȷ
	if( (Result[0] == FALSE) && (Result[1] == FALSE) )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


//-----------------------------------------------
//��������: �尲ȫ�ռ�,Ҫ�󳤶ȱ����ǽṹ�ĳ��ȣ�����У�����ڣ�
//			���Ұ�ȫ�ռ�ĵڶ�����ȡ���洢,У��̶���Ϊ��CRC32���ú������Զ����У��
//
//����: 
//			Addr[in]				��ʼ��ַ
//			Length[in]				���ݳ��ȣ������ṹ�ĳ��ȣ�����У��
//                    
//����ֵ:  	TRUE:��ȷд������������	FALSE:��һ�ݻ���������û����ȷд��
//
//��ע:   
//-----------------------------------------------
WORD api_ClrSafeMem( WORD Addr, WORD Length )
{
	WORD Result;
	BYTE Buf[2000];//���벻��������
	
	if( Length > 2000 )
	{
		return FALSE;
	}

	// ���뻺��
	memset( Buf, 0x00, Length );
	Result = api_VWriteSafeMem( Addr, Length, Buf );
	
	return Result;
}


//-----------------------------------------------
//��������: ����洢ģ����ϵ��ʼ������
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_PowerUpSave( void )
{
	InitEEPRom();
}

//-----------------------------------------------
//��������: �����д洢оƬָ����ַ���ݣ�Ϊ���ڹ��ܵ���д�ĺ���
//
//����: 
//			No[in]			ָ������һƬEEPROM
//							0x01����һƬEEPROM
//							0x02���ڶ�ƬEEPROM
//							0x11����һƬFlash
//							0x21: ��һƬ����
//							0x31: �������ռ�
//			Addr[in]		Ҫ������ʼ��ַ��оƬ��ƫ�Ƶ�ַ��
//			Length[in]		Ҫ�����ݵĳ���
//			pBuf[out]		Ҫ�������ݵĻ���
//                    
//����ֵ:  	�������ݵĳ��ȣ����Ϊ0����ʾʧ��
//
//��ע:   
//-----------------------------------------------
WORD api_ReadSaveMem( BYTE No, DWORD Addr, WORD Length, BYTE *pBuf )
{
	switch( No )
	{
		case 0x01:
			UReadAt25640( CS_SPI_256401, Addr, Length, pBuf );
			break;
		case 0x02:
			UReadAt25640( CS_SPI_256402, Addr, Length, pBuf );
			break;
		#if( (THIRD_MEM_CHIP==CHIP_GD25Q256C) ||(THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )
		case 0x11:
			ReadExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
			break;
		#endif
		case 0x21://��֧������
			return 0;
		case 0x31:
			api_ReadFromContinueEEPRom( Addr, Length, pBuf );
			break;
		default:
			return 0;
	}
	
	return Length;
}

