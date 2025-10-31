 /*
*********************************************************************************************************
*
*	ģ������ : cpu�ڲ�falsh����ģ��
*	�ļ����� : bsp_cpu_flash.h
*	��    �� : V1.0
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_CPU_FLASH_H_
#define _BSP_CPU_FLASH_H_

#include "__def.h"
//
#define FLASH_APP_START_ADDR	0x00008000			// 32K  һ������8K��һ��д8K����Ҫ��8K��������(��Ϊ24K ���ܷ���)
#define FLASH_APP_END_ADDR		0x00080000			// 512K
#define FLASH_APP_END1_ADDR		0x0007E000			// 512K-8K 8K���ڱ�����
#define FLASH_BASE_ADDR			0x00000000			/* Flash����ַ */
#define FLASH_SIZE		  		(512*1024)			/* Flash ���� */
#define FLASH_SECTOR_SIZE		((uint32_t)0x00002000)   //8K

#define SECTOR_MASK	           0xFFFFE000

#define FLASH_IS_EQU		0   /* Flash���ݺʹ�д���������ȣ�����Ҫ������д���� */
#define FLASH_REQ_WRITE		1	/* Flash����Ҫ������ֱ��д */
#define FLASH_REQ_ERASE		2	/* Flash��Ҫ�Ȳ���,��д */
#define FLASH_PARAM_ERR		3	/* ������������ */


typedef enum
{
  FLASH_OK =0,
  FLASH_BUSY = 1,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;


//uint8_t bsp_ReadCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize);
BOOL bsp_WriteCpuFlash(DWORD _ulFlashAddr, BYTE *_ucpSrc, DWORD _ulSize);
//uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize);
BOOL bsp_EraseAppFlash(BYTE Type);
#endif

	/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

