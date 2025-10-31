/*
*********************************************************************************************************
*
*	ģ������ : cpu�ڲ�falsh����ģ��
*	�ļ����� : bsp_cpu_flash.c
*	��    �� : V1.0
*	˵    �� : �ṩ��дCPU�ڲ�Flash�ĺ���
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "AllHead.h"
#include "bsp_cpu_flash.h"
#include "hc32f460_utility.h"
#include "hc32f460_efm.h"
#include "cpuHC32F460.h"

void FLASH_Unlock()
{
    /* Unlock EFM. */
    EFM_Unlock();
    /* Enable flash. */
    EFM_FlashCmd(Enable);
    /* Wait flash ready. */
    while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY))
    {
        ;
    }
}
void FLASH_Lock()
{
    /* Lock EFM. */
    EFM_Lock();
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetSector
*	����˵��: ���ݵ�ַ���������׵�ַ
*	��    �Σ���
*	�� �� ֵ: �����׵�ַ
*********************************************************************************************************
*/
uint32_t bsp_GetSector(uint32_t _ulWrAddr)
{
	uint32_t sector = 0;

	sector = _ulWrAddr & SECTOR_MASK;


	return sector;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_ReadCpuFlash
*	����˵��: ��ȡCPU Flash������
*	��    �Σ�_ucpDst : Ŀ�껺����
*			 _ulFlashAddr : ��ʼ��ַ
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ: 0=�ɹ���1=ʧ��
*********************************************************************************************************
*/
uint8_t bsp_ReadCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize)
{
	uint32_t i;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if ( (_ulFlashAddr + _ulSize) > (FLASH_BASE_ADDR + FLASH_SIZE) )
	{
		return 1;
	}

	/* ����Ϊ0ʱ����������,������ʼ��ַΪ���ַ����� */
	if (_ulSize == 0)
	{
		return 1;
	}

	for (i = 0; i < _ulSize; i++)
	{
		*_ucpDst++ = *(uint8_t *)_ulFlashAddr++;
	}

	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_CmpCpuFlash
*	����˵��: �Ƚ�Flashָ����ַ������.
*	��    ��: _ulFlashAddr : Flash��ַ
*			 _ucpBuf : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ:
*			FLASH_IS_EQU			0   Flash���ݺʹ�д���������ȣ�����Ҫ������д����
*			FLASH_REQ_WRITE		1	Flash����Ҫ������ֱ��д
*			FLASH_REQ_ERASE		2	Flash��Ҫ�Ȳ���,��д
*			FLASH_PARAM_ERR		3	������������
*********************************************************************************************************
*/
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucIsEqu;	/* ��ȱ�־ */
	uint8_t ucByte;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (_ulFlashAddr + _ulSize > FLASH_BASE_ADDR + FLASH_SIZE)
	{
		return FLASH_PARAM_ERR;		/*��������������*/
	}

	/* ����Ϊ0ʱ������ȷ */
	if (_ulSize == 0)
	{
		return FLASH_IS_EQU;		/* Flash���ݺʹ�д���������� */
	}

	ucIsEqu = 1;			/* �ȼ��������ֽںʹ�д���������ȣ���������κ�һ������ȣ�������Ϊ 0 */
	for (i = 0; i < _ulSize; i++)
	{
		ucByte = *(uint8_t *)_ulFlashAddr;

//		if (ucByte != *_ucpBuf)//���ݲ����
//		{
			if (ucByte != 0xFF)
			{
				return FLASH_REQ_ERASE;		/* ��Ҫ��������д */
			}
			else
			{
				ucIsEqu = 0;	/* ����ȣ���Ҫд */
			}
//		}

		_ulFlashAddr++;
		_ucpBuf++;
	}

	if (ucIsEqu == 1)
	{
		return FLASH_IS_EQU;	/* Flash���ݺʹ�д���������ȣ�����Ҫ������д���� */
	}
	else
	{
		return FLASH_REQ_WRITE;	/* Flash����Ҫ������ֱ��д */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_WriteCpuFlash
*	����˵��: д���ݵ�CPU �ڲ�Flash��
*	��    ��: _ulFlashAddr : Flash��ַ
*			 _ucpSrc : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ: 0-�ɹ���1-���ݳ��Ȼ��ַ�����2-дFlash����(����Flash������)
*********************************************************************************************************
*/
BOOL bsp_WriteCpuFlash(DWORD _ulFlashAddr, BYTE *_ucpSrc, DWORD _ulSize)
{
	DWORD i;
	uint8_t ucRet;
	DWORD usTemp;
	FLASH_Status status = FLASH_OK;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if ( (_ulFlashAddr + _ulSize) > (FLASH_BASE_ADDR + FLASH_SIZE) )
	{
		return FALSE;
	}

	/* ����Ϊ0 ʱ����������  */
	if (_ulSize == 0)
	{
		return FALSE;
	}

	/* F460��Сд�뵥λ��4�ֽ�*/
	if ((_ulSize % 4) != 0)
	{
		return FALSE;
	}	

	/* FLASH ���� */
	FLASH_Unlock();

  	/* Clear pending flags (if any) */
//	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);	

	/* ���ֽ�ģʽ��̣�Ϊ���Ч�ʣ����԰��ֱ�̣�һ��д��4�ֽڣ� */
	for (i = 0; i < (_ulSize / 4); i++)
	{	
		usTemp = _ucpSrc[4 * i];
		usTemp |= (_ucpSrc[4 * i + 1] << 8);
		usTemp |= (_ucpSrc[4 * i + 2] << 16);
		usTemp |= (_ucpSrc[4 * i + 3] << 24);

		status = (FLASH_Status)EFM_SingleProgram(_ulFlashAddr,usTemp);
		if (status != FLASH_OK)
		{
			FLASH_Lock();
			return FALSE;
		}
		
		_ulFlashAddr += 4;
	}

  	/* Flash ��������ֹдFlash���ƼĴ��� */
  	FLASH_Lock();

	return TRUE;
}

// ����dwAddr��Ӧ��ʼ��ַ��Ӧ��һ��flash����
BOOL bsp_EraseOneSector( DWORD dwAddr )
{
	FLASH_Status status = FLASH_OK;

	if (dwAddr % (FLASH_SECTOR_SIZE) != 0)
	{
		return FALSE;
	}
	FLASH_Unlock();

	status = (FLASH_Status)EFM_SectorErase(bsp_GetSector(dwAddr));

	if (status != FLASH_OK)
	{
		FLASH_Lock();
		return FALSE;
	}

	FLASH_Lock();

	return TRUE;
}

