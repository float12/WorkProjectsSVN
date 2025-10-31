 /*
*********************************************************************************************************
*
*	模块名称 : cpu内部falsh操作模块
*	文件名称 : bsp_cpu_flash.h
*	版    本 : V1.0
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_CPU_FLASH_H_
#define _BSP_CPU_FLASH_H_

#include "__def.h"
//
#define FLASH_APP_START_ADDR	0x00008000			// 32K  一个扇区8K，一次写8K，需要是8K的整数倍(改为24K 不能仿真)
#define FLASH_APP_END_ADDR		0x00080000			// 512K
#define FLASH_APP_END1_ADDR		0x0007E000			// 512K-8K 8K用于保护区
#define FLASH_BASE_ADDR			0x00000000			/* Flash基地址 */
#define FLASH_SIZE		  		(512*1024)			/* Flash 容量 */
#define FLASH_SECTOR_SIZE		((uint32_t)0x00002000)   //8K

#define SECTOR_MASK	           0xFFFFE000

#define FLASH_IS_EQU		0   /* Flash内容和待写入的数据相等，不需要擦除和写操作 */
#define FLASH_REQ_WRITE		1	/* Flash不需要擦除，直接写 */
#define FLASH_REQ_ERASE		2	/* Flash需要先擦除,再写 */
#define FLASH_PARAM_ERR		3	/* 函数参数错误 */


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

	/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

