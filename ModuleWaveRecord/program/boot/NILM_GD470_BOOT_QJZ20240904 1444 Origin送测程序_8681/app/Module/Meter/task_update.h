/*****************************************************************************/
/*
 *项目名称： 物联网表电能质量模块
 *创建人：   wjs
 *日  期：   2021年5月
 *修改人：
 *日  期：
 *描  述：
 *
 *版  本：
 *说  明:    升级程序
 *
 */
/*****************************************************************************/
#ifndef _TASK_UPDATE_H_
#define _TASK_UPDATE_H_

#define RAMCODE_START_ADDR  0x24000000
#define RAMCODE_END_ADDR    0x24040000

#define UPDATEBUFFERLEN     (2048)
#define UPDATERECWAITMS     (1000)




/* base address of the FMC sectors */
#define ADDR_FMC_SECTOR_0		((uint32_t)0x08000000) /*!< base address of sector 0, 16 kbytes */
#define ADDR_FMC_SECTOR_1		((uint32_t)0x08004000) /*!< base address of sector 1, 16 kbytes */
#define ADDR_FMC_SECTOR_2		((uint32_t)0x08008000) /*!< base address of sector 2, 16 kbytes */
#define ADDR_FMC_SECTOR_3		((uint32_t)0x0800C000) /*!< base address of sector 3, 16 kbytes */
#define ADDR_FMC_SECTOR_4		((uint32_t)0x08010000) /*!< base address of sector 4, 64 kbytes */
#define ADDR_FMC_SECTOR_5		((uint32_t)0x08020000) /*!< base address of sector 5, 128 kbytes */
#define ADDR_FMC_SECTOR_6		((uint32_t)0x08040000) /*!< base address of sector 6, 128 kbytes */
#define ADDR_FMC_SECTOR_7		((uint32_t)0x08060000) /*!< base address of sector 7, 128 kbytes */
#define ADDR_FMC_SECTOR_8		((uint32_t)0x08080000) /*!< base address of sector 8, 128 kbytes */
#define ADDR_FMC_SECTOR_9		((uint32_t)0x080A0000) /*!< base address of sector 9, 128 kbytes */
#define ADDR_FMC_SECTOR_10		((uint32_t)0x080C0000) /*!< base address of sector 10,128 kbytes */
#define ADDR_FMC_SECTOR_11		((uint32_t)0x080E0000) /*!< base address of sector 11,128 kbytes */
#define ADDR_FMC_SECTOR_12		((uint32_t)0x08100000) /*!< base address of sector 12, 16 kbytes */
#define ADDR_FMC_SECTOR_13		((uint32_t)0x08104000) /*!< base address of sector 13, 16 kbytes */	
#define ADDR_FMC_SECTOR_14		((uint32_t)0x08108000) /*!< base address of sector 14, 16 kbytes */	
#define ADDR_FMC_SECTOR_15		((uint32_t)0x0810C000) /*!< base address of sector 15, 16 kbytes */	
#define ADDR_FMC_SECTOR_16		((uint32_t)0x08110000) /*!< base address of sector 16, 64 kbytes */	
#define ADDR_FMC_SECTOR_17		((uint32_t)0x08120000) /*!< base address of sector 17, 128 kbytes */		
#define ADDR_FMC_SECTOR_18		((uint32_t)0x08140000) /*!< base address of sector 18, 128 kbytes */
#define ADDR_FMC_SECTOR_19		((uint32_t)0x08160000) /*!< base address of sector 19, 128 kbytes */
#define ADDR_FMC_SECTOR_20		((uint32_t)0x08180000) /*!< base address of sector 20, 128 kbytes */
#define ADDR_FMC_SECTOR_21		((uint32_t)0x081A0000) /*!< base address of sector 21, 128 kbytes */
#define ADDR_FMC_SECTOR_22		((uint32_t)0x081C0000) /*!< base address of sector 22, 128 kbytes */
#define ADDR_FMC_SECTOR_23		((uint32_t)0x081E0000) /*!< base address of sector 23, 128 kbytes */
#define ADDR_FMC_SECTOR_24		((uint32_t)0x08200000) /*!< base address of sector 24, 256 kbytes */
#define ADDR_FMC_SECTOR_25		((uint32_t)0x08240000) /*!< base address of sector 25, 256 kbytes */
#define ADDR_FMC_SECTOR_26		((uint32_t)0x08280000) /*!< base address of sector 26, 256 kbytes */
#define ADDR_FMC_SECTOR_27		((uint32_t)0x082C0000) /*!< base address of sector 27, 256 kbytes */

#ifndef FAL_DEV_NAME_MAX
#define FAL_DEV_NAME_MAX 		24
#endif	


#define ON_FLASH_SECTOR_SIZE   	(16 * 1024)
#define ON_FLASH_SIZE        	(64 * 1024) //指分配给bootloader的，不是整个on-flash大小！
#define ON_FLASH_TOTAL_SIZE	(1 * 1024 * 1024)

#define FLASH_ADDR_START  	(0x08000000)
#define FLASH_ADDR_END		((uint32_t)(FLASH_ADDR_START + ON_FLASH_TOTAL_SIZE - 1))



typedef void (*pFunction)(void);

extern BYTE UpdateBuffer[2][UPDATEBUFFERLEN];
extern BYTE UpdateBuf_No;
extern DWORD UpdateBuf_pos;

extern DWORD UpdateCodeBufferID;
extern DWORD UpdateDataLen;

void api_InitCodeBudder(void);
void api_SetCodeBufferLen(BYTE ID, DWORD Len);
DWORD api_GetCodeBufferLen(BYTE ID);
void api_SetCodeBufferSum(BYTE ID, WORD Sum);
WORD api_GetCodeBufferSum(BYTE ID);
void api_SetUpdateFlag(BOOL state);
BOOL api_GetUpdateFlag(void);
void api_SetCodeBufferFlag(BYTE type, BYTE ID);
BYTE api_GetCodeBufferFlag(BYTE type);
BOOL api_WriteDataToCodeBuffer(BYTE ID, DWORD Offset, WORD Len, BYTE *pBuf);
WORD api_CalcCodeBufferSum(BYTE ID);
BOOL api_CheckCodeBufferSum(BYTE ID);
BOOL api_CheckRAMCodeSum(BYTE ID);
BOOL api_CopyCodeBufferToRAM(BYTE ID);
void api_JumpToRAM(void);

#endif

