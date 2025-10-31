//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.10
//����		at45db161��at45db321�������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "at45db161.h"
#include "SaveMem.h"

#if( (THIRD_MEM_CHIP == CHIP_AT45DB161) || (THIRD_MEM_CHIP == CHIP_AT45DB321) )

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
static BYTE WriteDB161ErrorTimes;
static BYTE g_byFlashErrTimes;	//flash�洢�����ϴ���

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��ַת������ַת��Ϊ45DB161��ʽ���ڲ�����
//
//����: 
//			Addr[in]		оƬ���߼���ַ
//                    
//����ֵ:  	��оƬ����Ѱַ�ĵ�ַ
//
//��ע:   
//-----------------------------------------------
static DWORD Conver45DB116Addr(DWORD Addr)
{
	DWORD TrueAddr;

	//�����λ����1
	TrueAddr = 0x00c00000;
	//������
	TrueAddr += (Addr / AT45DB161B_PAGE_SIZE) << 10;
	//������Ѱַ
	TrueAddr += (Addr % AT45DB161B_PAGE_SIZE);

	return TrueAddr;
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
	WORD i;
	DWORD TrueAddr;

	ASSERT( (Addr+Len) <= THIRD_MEM_SPACE, 1 );

	TrueAddr = Conver45DB116Addr(Addr);

	//Ƭѡ
	DoSPICS(No, TRUE);

	//д������
	api_UWriteSpi(AT45DB161_CMD_CAR);
	//д���ַ
	api_UWriteSpi(TrueAddr>>16);
	api_UWriteSpi(TrueAddr>>8);
	api_UWriteSpi(TrueAddr);
	//д�뻺���ֽ�
    api_UWriteSpi(0xfe);
    api_UWriteSpi(0xfe);
    api_UWriteSpi(0xfe);
    api_UWriteSpi(0xfe);

	//��ʼ������
	for(i=0; i<Len; i++)
	{
		pBuf[i] = api_UWriteSpi(0);
	}

	DoSPICS(No, FALSE);

	return TRUE;
}


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
static WORD Wait45DB161DoOver(BYTE No)
{
	BYTE Status;
	WORD i;

	CLEAR_WATCH_DOG;
	DoSPICS(No, TRUE);
	api_UWriteSpi(AT45DB161_CMD_SRR);

	//���ʱ200����ȴ�
	for(i=0; i<200; i++)
	{
		Status = api_UWriteSpi(0);

		//�Ѿ�����
		if( Status & 0x80 )
		{
			break;
		}

		api_Delayms(1);
	}

	DoSPICS(No, FALSE);

	CLEAR_WATCH_DOG;// 2014��5��19�պ���˵���������45db321����

	if( i>= 200 )
	{
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//��������: ���ĳ��������Ӻ��������ܲ������ݣ��ڲ�����
//
//����: 
//			No[in]			оƬ��ţ�����Ƭѡѡ��
//			Cmd[in]			����
//			Addr[in]		������ַ
//                    
//����ֵ:  	TRUE:��ȷִ��   FALSE:δ��ȷִ��   ʵ�ʷ��صĻ���оƬ�Ƿ�Ϊ����״̬
//
//��ע:   
//-----------------------------------------------
static WORD Do45DB161SingleCmd(BYTE No, BYTE Cmd, DWORD Addr)
{
	DoSPICS(No, TRUE);
	api_UWriteSpi(Cmd);
	api_UWriteSpi(Addr>>16);
	api_UWriteSpi(Addr>>8);
	api_UWriteSpi(Addr);
	DoSPICS(No, FALSE);

	//�ȴ�ִ�����
	return Wait45DB161DoOver(No);
}


//-----------------------------------------------
//��������: д��ĳ�����������ܿ�ҳ�������ڲ�����
//
//����: 
//			No[in]			оƬ��ţ�����Ƭѡѡ��
//			Addr[in]		������ַ
//			Len[in]			д�볤�ȣ�ע�ⲻҪ����1��������
//			pBuf[in/out]	д������ݻ���
//                    
//����ֵ:  	TRUE:��ȷִ��   FALSE:δ��ȷִ��
//
//��ע:   
//-----------------------------------------------
static WORD WriteAT45DB161BPage(BYTE No, DWORD Addr, WORD Len, BYTE * pBuf)
{
	BYTE Status;
	DWORD i;
	DWORD TrueAddr = Conver45DB116Addr(Addr);

	//���ݶ�������
	if( Do45DB161SingleCmd(No, AT45DB161_CMD_MPB1T, TrueAddr) != TRUE )
	{
		return FALSE;
	}

	//�޸Ļ���
	DoSPICS(No, TRUE);
	api_UWriteSpi(AT45DB161_CMD_B1W);
	api_UWriteSpi(TrueAddr>>16);
	api_UWriteSpi(TrueAddr>>8);
	api_UWriteSpi(TrueAddr);
	//д������
	for(i=0; i<Len; i++)
	{
		api_UWriteSpi(pBuf[i]);
	}
	DoSPICS(No, FALSE);

	//��������д�����洢��
	if( Do45DB161SingleCmd(No, AT45DB161_CMD_B1MPBE, TrueAddr) != TRUE )
	{
		return FALSE;
	}

	//���������洢���Ƚ�
	if( Do45DB161SingleCmd(No, AT45DB161_CMD_MPB1C, TrueAddr) != TRUE )
	{
		return FALSE;
	}

	DoSPICS(No, TRUE);
	api_UWriteSpi(AT45DB161_CMD_SRR);
	Status = api_UWriteSpi(0);
	DoSPICS(No, FALSE);

	if( Status & 0x40 )
	{
		return FALSE;
	}

	return TRUE;
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
static void Reset45DB161B( void )
{	

	if( g_byFlashErrTimes < 200 )
	{
		g_byFlashErrTimes ++;
		if( g_byFlashErrTimes == 200 )
		{
			api_SetFollowReportStatus(eSTATUS_EEPROM_Error);
		}
	}
	//WriteDB161ErrorTimes==0xff˵��оƬ�д��󣬲����ٸ�λ
	if( WriteDB161ErrorTimes == 0xff )
	{
		return;
	}

	WriteDB161ErrorTimes++;

	if( WriteDB161ErrorTimes >= (FLASH_MAX_WRITE_COUNT+4) )
	{
		WriteDB161ErrorTimes = 0xff;//˵��оƬ�����⣬�����ٸ�λ��
		api_SetError( ERR_WRITE_EEPROM3 );
	}
	else
	{
    	PowerUpResetExtFlash( 1 );
	}
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
void PowerUpResetExtFlash( BYTE Type )
{
	if( Type == 0 )
	{
		WriteDB161ErrorTimes = 0;
	}
	
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 );
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
static WORD WriteAT45DB161BPageLogic(BYTE No, DWORD Addr, WORD Len, BYTE * pBuf)
{
	DWORD i;
	
	for(i=0; i<FLASH_MAX_WRITE_COUNT; i++)
	{
		if( WriteAT45DB161BPage(No, Addr, Len, pBuf) == TRUE )
		{
			WriteDB161ErrorTimes = 0;
			g_byFlashErrTimes = 0;
			api_ClrError( ERR_WRITE_EEPROM3 );

			return TRUE;
		}

		//��λоƬ
		Reset45DB161B();
	}

	return FALSE;
}


//-----------------------------------------------
//��������: �Ե�ַȡ��������ҳ��ʼ���ڲ�����
//
//����: 	
//			Addr[in]		δ����ҳȡ���ĵ�ַ
//
//����ֵ:  	����ҳȡ����ĵ�ַ
//
//��ע:
//-----------------------------------------------
static DWORD AddrToInterge(DWORD Addr)
{
	DWORD i;
	i = Addr / AT45DB161B_PAGE_SIZE;
	i *= AT45DB161B_PAGE_SIZE;

	return i;
}


//-----------------------------------------------
//��������: д�ⲿFlash������ģ���ڲ�����
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
BOOL WriteExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf)
{
	DWORD TrueAddr;
	WORD PageLen;
	short WriteLen;

	ASSERT( (Addr+Len) <= THIRD_MEM_SPACE, 1 );

	//��ʼ��ַ
	TrueAddr = Addr;

	WriteLen = Len;

	CLEAR_WATCH_DOG;
	//��һ�����������ݵĳ���
	PageLen = AT45DB161B_PAGE_SIZE - (Addr - AddrToInterge(Addr));
	if( PageLen >= Len )
	{
		//����д�����ݶ���һ��������
		PageLen = Len;
		WriteLen -= PageLen;
	}

	for(;;)
	{
		//д��һ������
		if( WriteAT45DB161BPageLogic(No, TrueAddr, PageLen, pBuf) != TRUE )
		{
			return FALSE;
		}

		//�����Ƿ�д�����
		WriteLen -= PageLen;
		if( WriteLen <= 0 )
		{
			break;
		}

		//������ַ
		TrueAddr += PageLen;
		pBuf += PageLen;

		//���㳤��
		if( WriteLen >= AT45DB161B_PAGE_SIZE )
		{
			//��ʣ�೤�ȴ���һ������
			PageLen = AT45DB161B_PAGE_SIZE;
		}
		else
		{
			//С��һ��������ֱ��д��
			PageLen = WriteLen;
		}
	}

	return TRUE;
}


#endif//#if( (THIRD_MEM_CHIP == CHIP_AT45DB161) || (THIRD_MEM_CHIP == CHIP_AT45DB321) )


