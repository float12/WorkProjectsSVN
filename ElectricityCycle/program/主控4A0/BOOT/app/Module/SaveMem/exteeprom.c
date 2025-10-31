//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.10
//����		�����ռ���������
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "at45db161.h"
#include "gd25q64c.h"
#include "SaveMem.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------



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
//��ע:����flash�Ķ�flash����û��flash�Ķ�eeprom������   
//-----------------------------------------------
BOOL api_ReadMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	#if(THIRD_MEM_CHIP == CHIP_NO)
	return api_ReadFromContinueEEPRom( Addr, Length, pBuf );
	#else
	return ReadExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
	#endif
}

//-----------------------------------------------
//��������: д�����¼�������
//
//����: 
//			Addr[in]		Ҫд����ʼ��ַ
//			Length[in]		Ҫд�ĳ���
//			pBuf[in/out]	д����ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:����flash��д��flash����û��flash��д��eeprom��   
//-----------------------------------------------
BOOL api_WriteMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	#if(THIRD_MEM_CHIP == CHIP_NO)
	return api_WriteToContinueEEPRom( Addr, Length, pBuf );
	#else
	return WriteExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
	#endif
}





