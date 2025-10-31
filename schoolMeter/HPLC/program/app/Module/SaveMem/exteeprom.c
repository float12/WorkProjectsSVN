//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.10
//����		�����ռ���������
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
//�����洢����ṹ
typedef struct TConMemStruct_t
{
	//оƬ����
	WORD ChipType;
	//��Ƭ�ڿ�ʼ�����ַ
	DWORD ConMemPhysicalAddr;
	//�ܹ���Ϊ�����ռ��������
	DWORD ConMemPhysicalSize;
	//�߼��������ռ��ַ
	DWORD ConMemLogicAddr;
	//�߼��������ռ䳤��
	DWORD ConMemLogicSize;
}TConMemStruct;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//�����ռ�洢����
const TConMemStruct ConMemStruct[] =
{
	//оƬ���ͣ�			оƬ������ʼ��ַ��		�ܹ���Ϊ�����ռ�������ȣ�	�߼��������ռ��ַ��                          	�߼��������ռ䳤��

	//��һ�������洢оƬ
	{MASTER_MEMORY_CHIP, 	LAST_ADDR_256401, 			SINGLE1_CONTINUE_SIZE, 			0,               								(SINGLE1_CONTINUE_SIZE)},

	//�ڶ��������洢оƬ
	{SLAVE_MEM_CHIP, 		LAST_ADDR_256402, 			SINGLE2_CONTINUE_SIZE, 			SINGLE1_CONTINUE_SIZE,   						(SINGLE1_CONTINUE_SIZE+SINGLE2_CONTINUE_SIZE)},

};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��д�����ռ䣬�ڲ�����
//
//����: 
//			Addr[in]		Ҫ��д����ʼ��ַ
//			Length[in]		Ҫ��д�ĳ���
//			pBuf[in/out]	д�Ͷ�����ʱ�Ļ���
//			Type[in]		WRITE:д����  READ��������
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:   
//-----------------------------------------------
static BOOL DoContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf, WORD Type )
{
	short TempLen;
	WORD ChipCs,i,DoLength;
	BOOL (*pFunc)(WORD No, DWORD Addr, WORD Length, BYTE * Buf);

	//������ �������Ա���Ӻ�����д��ȥ �����������Լ�¼����Ϊ��λ��
	if( (Addr+Length) > CONTINUE_TOTAL_SIZE )//��ϸ��������Ϣ¼��:FLASHĩ��ַ�����޷�д�����
	{
		//��¼�쳣�¼�
		api_WriteSysUNMsg( OVERWRITE_CONTINUE_SPACE );

		return FALSE;
	}

	TempLen = Length;

	//��ѯ���д洢оƬ
	for(i=0; i<(sizeof(ConMemStruct)/sizeof(TConMemStruct)); i++)
	{
		//����ʼ��ַС�ڴ�Ƭ�����������ռ䳤�ȣ�����Ҫд���Ƭ
		if( Addr < ConMemStruct[i].ConMemLogicSize )
		{
			//д��ȫ�����
			if( TempLen <= 0 )
			{
				break;
			}

			//����д�볤��
			if( (Addr+TempLen) <= ConMemStruct[i].ConMemLogicSize )
			{
				//д��ȫ������
				DoLength = TempLen;
			}
			else
			{
				//д�벿�����ݣ�����ΪоƬ�ܴ�С��ȥ����Ŀ�ʼ��ַ
				DoLength = ConMemStruct[i].ConMemPhysicalSize-(Addr-ConMemStruct[i].ConMemLogicAddr);
			}

			if( Type == WRITE )
			{
				pFunc = UWriteAt25640;
			}
			else
			{
				pFunc = UReadAt25640;
			}

		#if((MAX_PHASE == 1) && (SLAVE_MEM_CHIP != CHIP_NO))
			#warning "�����ռ���, ������Ϊ��һƬ, ����E2��Ϊ�ڶ�Ƭ; �����󲿷ִ��ڵ�һƬE2, (����E2�ض�������, ���ݱ�λ)"
			if( i == 0 )
			{
				ChipCs = CS_SPI_256402;
			}
			else if( i == 1 )
			{
				ChipCs = CS_SPI_256401;
			}
		#else
			if( i == 0 )
			{
				ChipCs = CS_SPI_256401;
			}
			else if( i == 1 )
			{
				ChipCs = CS_SPI_256402;
			}
		#endif
			else
			{
				return FALSE;
			}

			//д������
			//оƬ��� �� �����������
			//оƬ���� �� ��������
			//��ʼ��ַ �� ��ǰ��ʼ��ַ-��Ƭ���߼���ʼ��ַ+��Ƭ�߼���ʼ��ַ
			//д�볤�� �� Len
			if( (*pFunc)( ChipCs, (Addr-ConMemStruct[i].ConMemLogicAddr+ConMemStruct[i].ConMemPhysicalAddr), DoLength, pBuf) != TRUE )
			{
				return FALSE;
			}

			pBuf += DoLength;
			TempLen -= DoLength;
			Addr += DoLength;
		}
	}

	return TRUE;
}


//-----------------------------------------------
//��������: д�����ռ亯��
//
//����: 
//			Addr[in]		Ҫд����ʼ��ַ
//			Length[in]		Ҫд�ĳ���
//			pBuf[in/out]	д����ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_WriteToContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf)
{
	return DoContinueEEPRom(Addr, Length, pBuf, WRITE);
}


//-----------------------------------------------
//��������: �������ռ�����ݵĺ���
//
//����: 
//			Addr[in]		Ҫ������ʼ��ַ
//			Length[in]		Ҫ���ĳ���
//			pBuf[in/out]	������ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_ReadFromContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf)
{
	BYTE i,j,DoLength,Times,Result;
	BYTE ReadBuf[64];
		
	Result = TRUE;
	Times = Length/sizeof(ReadBuf);
	Times += 1;
	
	for(i=0; i<Times; i++)
	{
		if( i == (Times-1) )//���һ��
		{
			DoLength = Length-(WORD)sizeof(ReadBuf)*(Times-1);
		}
		else
		{
			DoLength = sizeof(ReadBuf);
		}
		
		for(j=0; j<2; j++)//�������2��
		{
			DoContinueEEPRom(Addr, DoLength, pBuf, READ);
			DoContinueEEPRom(Addr, DoLength, ReadBuf, READ);
			if( memcmp( pBuf, ReadBuf, DoLength ) == 0 )//�����ν��бȽϣ�һ������Ϊ������ȷ
			{
				break;
			}
		}
				
		if( j == 2 )
		{
			ASSERT( 0, 0 );
			Result = FALSE;
		}
		
		Addr += DoLength;
		pBuf += DoLength;
	}
	
	return Result;
}


//-----------------------------------------------
//��������: �Ѵ�Ƭ�������ռ�д��0����ҪΪ�¼�����Ȳ���ʹ�á����ж��Ƿ�����ɹ�
//
//����: 
//			Addr[in]		Ҫ�������ʼ��ַ
//			Length[in]		Ҫ����ĳ���
//                    
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void api_ClrContinueEEPRom(DWORD Addr, WORD Length)
{
	WORD i, j, k;
	BYTE Array[128];

	i = ( Length / sizeof(Array) );
	j = ( Length % sizeof(Array) );

	memset( (void *)&Array, 0, sizeof(Array) );

	for(k=0; k<i; k++)
	{
		api_WriteToContinueEEPRom( Addr, sizeof(Array), Array );

		//ȡ��ַ
		Addr += sizeof(Array);

		CLEAR_WATCH_DOG;
	}

	api_WriteToContinueEEPRom( Addr, j, Array );
}


//-----------------------------------------------
//��������: �������ռ�����ݼ�����,Ҫ�󳤶ȱ����ǽṹ�ĳ��ȣ�����У�����ڣ�
//			�ڶ���ȡ���洢��У��̶���Ϊ��CRC32
//
//����: 
//			Addr1[in]				��һ���������ռ����ʼ��ַ
//			Addr2[in]				�ڶ����������ռ����ʼ��ַ,��Ϊ0���򲻶��ڶ���
//			Length[in]				���ݳ��ȣ������ṹ�ĳ��ȣ�����У��
//			pBuf[in/out]			��Ŷ��������ݣ�����У��
//                    
//����ֵ:  	TRUE:		��һ�ݶ�����Ϊ�Ƕ�
//			FALSE��		���ݶ�����
//��ע:   
//-----------------------------------------------
WORD api_VReadContinueMem( DWORD Addr1, DWORD Addr2, BYTE Length, BYTE * pBuf )
{
	DoContinueEEPRom(Addr1, Length, pBuf, READ);
	
	if( lib_CheckSafeMemVerify( pBuf, Length, UN_REPAIR_CRC ) == TRUE )
	{
		return TRUE;
	}
	
	if( Addr2 != 0 )
	{
		DoContinueEEPRom( Addr2, Length, pBuf, READ );

		ReverseInData( pBuf, Length );

		if( lib_CheckSafeMemVerify( pBuf, Length, UN_REPAIR_CRC ) == TRUE )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

//-----------------------------------------------
//��������: �������¼�������
//
//����: 
//			Addr[in]		Ҫ������ʼ��ַ
//			Length[in]		Ҫ���ĳ���
//			pBuf[in/out]	������ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:�˺���ֻ��ȡFlash�ж����¼�������
//-----------------------------------------------
BOOL api_ReadMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	return ReadExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
}

//-----------------------------------------------
//��������: д�����¼�������,����ַУ��
//
//����: 
//			Addr[in]		Ҫд����ʼ��ַ
//			Length[in]		Ҫд�ĳ���
//			pBuf[in/out]	д����ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:�˺���ֻдFlash�ж����¼�������
//-----------------------------------------------
BOOL api_WriteMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	return WriteExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
}





