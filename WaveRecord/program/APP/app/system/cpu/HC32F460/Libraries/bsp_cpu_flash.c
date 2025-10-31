/*
*********************************************************************************************************
*
*	模块名称 : cpu内部falsh操作模块
*	文件名称 : bsp_cpu_flash.c
*	版    本 : V1.0
*	说    明 : 提供读写CPU内部Flash的函数
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-02-01 armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
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
*	函 数 名: bsp_GetSector
*	功能说明: 根据地址计算扇区首地址
*	形    参：无
*	返 回 值: 扇区首地址
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
*	函 数 名: bsp_ReadCpuFlash
*	功能说明: 读取CPU Flash的内容
*	形    参：_ucpDst : 目标缓冲区
*			 _ulFlashAddr : 起始地址
*			 _ulSize : 数据大小（单位是字节）
*	返 回 值: 0=成功，1=失败
*********************************************************************************************************
*/
uint8_t bsp_ReadCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize)
{
	uint32_t i;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if ( (_ulFlashAddr + _ulSize) > (FLASH_BASE_ADDR + FLASH_SIZE) )
	{
		return 1;
	}

	/* 长度为0时不继续操作,否则起始地址为奇地址会出错 */
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
*	函 数 名: bsp_CmpCpuFlash
*	功能说明: 比较Flash指定地址的数据.
*	形    参: _ulFlashAddr : Flash地址
*			 _ucpBuf : 数据缓冲区
*			 _ulSize : 数据大小（单位是字节）
*	返 回 值:
*			FLASH_IS_EQU			0   Flash内容和待写入的数据相等，不需要擦除和写操作
*			FLASH_REQ_WRITE		1	Flash不需要擦除，直接写
*			FLASH_REQ_ERASE		2	Flash需要先擦除,再写
*			FLASH_PARAM_ERR		3	函数参数错误
*********************************************************************************************************
*/
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucIsEqu;	/* 相等标志 */
	uint8_t ucByte;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if (_ulFlashAddr + _ulSize > FLASH_BASE_ADDR + FLASH_SIZE)
	{
		return FLASH_PARAM_ERR;		/*　函数参数错误　*/
	}

	/* 长度为0时返回正确 */
	if (_ulSize == 0)
	{
		return FLASH_IS_EQU;		/* Flash内容和待写入的数据相等 */
	}

	ucIsEqu = 1;			/* 先假设所有字节和待写入的数据相等，如果遇到任何一个不相等，则设置为 0 */
	for (i = 0; i < _ulSize; i++)
	{
		ucByte = *(uint8_t *)_ulFlashAddr;

//		if (ucByte != *_ucpBuf)//数据不相等
//		{
			if (ucByte != 0xFF)
			{
				return FLASH_REQ_ERASE;		/* 需要擦除后再写 */
			}
			else
			{
				ucIsEqu = 0;	/* 不相等，需要写 */
			}
//		}

		_ulFlashAddr++;
		_ucpBuf++;
	}

	if (ucIsEqu == 1)
	{
		return FLASH_IS_EQU;	/* Flash内容和待写入的数据相等，不需要擦除和写操作 */
	}
	else
	{
		return FLASH_REQ_WRITE;	/* Flash不需要擦除，直接写 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_WriteCpuFlash
*	功能说明: 写数据到CPU 内部Flash。
*	形    参: _ulFlashAddr : Flash地址
*			 _ucpSrc : 数据缓冲区
*			 _ulSize : 数据大小（单位是字节）
*	返 回 值: 0-成功，1-数据长度或地址溢出，2-写Flash出错(估计Flash寿命到)
*********************************************************************************************************
*/
BOOL bsp_WriteCpuFlash(DWORD _ulFlashAddr, BYTE *_ucpSrc, DWORD _ulSize)
{
	DWORD i;
	uint8_t ucRet;
	DWORD usTemp;
	FLASH_Status status = FLASH_OK;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if ( (_ulFlashAddr + _ulSize) > (FLASH_BASE_ADDR + FLASH_SIZE) )
	{
		return FALSE;
	}

	/* 长度为0 时不继续操作  */
	if (_ulSize == 0)
	{
		return FALSE;
	}

	/* F460最小写入单位是4字节*/
	if ((_ulSize % 4) != 0)
	{
		return FALSE;
	}	

	/* FLASH 解锁 */
	FLASH_Unlock();

  	/* Clear pending flags (if any) */
//	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);	

	/* 按字节模式编程（为提高效率，可以按字编程，一次写入4字节） */
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

  	/* Flash 加锁，禁止写Flash控制寄存器 */
  	FLASH_Lock();

	return TRUE;
}

// 擦除dwAddr对应起始地址对应的一个flash扇区
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

