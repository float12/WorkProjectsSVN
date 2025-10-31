/*
**********************************************************************************************************
*                                              HT6XXX
*                                          Library Function
*
*                                   Copyright 2013, Hi-Trend Tech, Corp.
*                                        All Rights Reserved
*                                         
*
* Project      : HT6xxx
* File         : ht6xxx_flash.h
* By           : SocTeam
* Version      : CurrentVersion
* Description  : 
**********************************************************************************************************
*/

#ifndef __HT6XXX_FLASH_H__
#define __HT6XXX_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif 
    
typedef enum
{
  FLASH_BUSY = 1,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;
  
/*
*********************************************************************************************************
*                                           全局宏/结构体
*********************************************************************************************************
*/
/************************  Bit definition for FLASHCON register of HT_CMU_TypeDef ***********************/
#define  CMU_FLASHCON                      	 	 ((uint32_t)0x0007)           /*!<Flash控制寄存器         */
#define  CMU_FLASHCON_FOP                        ((uint32_t)0x0003)           /*!<Flash操作模式         */
#define  CMU_FLASHCON_FOP_READ                   ((uint32_t)0x0000)           /*!<Flash处于只读模式     */
#define  CMU_FLASHCON_FOP_WRITE                  ((uint32_t)0x0001)           /*!<Flash处于写模式       */
#define  CMU_FLASHCON_FOP_PAGEERASE              ((uint32_t)0x0002)           /*!<Flash处于页擦除模式   */
#define  CMU_FLASHCON_FOP_CHIPERASE              ((uint32_t)0x0003)           /*!<Flash处于全擦除模式   */
                                                                                                        
#define  CMU_FLASHCON_BUSY                       ((uint32_t)0x0004)           /*!<Flash状态 0:空闲 1:忙 */

/************************  Bit definition for FLASHLOCK register of HT_CMU_TypeDef **********************/
#define  CMU_FLASHLOCK                           ((uint32_t)0xffff)           /*!<Flash锁定寄存器       */
#define  CMU_FLASHLOCK_UnLocked                  ((uint32_t)0x7A68)           /*!<Flash写/擦除解锁      */
#define  CMU_FLASHLOCK_Locked                    ((uint32_t)0x0000)           /*!<Flash锁定             */
                                                                              /*!<0x7A68 : Unlock       */
/**************************  Bit definition for WPREG register of HT_CMU_TypeDef ************************/
#define  CMU_WPREG                               ((uint32_t)0xffff)           /*!<写保护寄存器          */
#define  CMU_WPREG_UnProtected                   ((uint32_t)0xA55A)           /*!<关闭写保护功能        */
#define  CMU_WPREG_Protected                     ((uint32_t)0x0000)           /*!<打开写保护功能        */
                                                                              /*!<0xA55A: Unprotected   */
                                                                              /*!<other : Protected     */                                                                              /*!<other  : Lock         */

/*
*********************************************************************************************************
*                                             全局变量
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           全局函数申明
*********************************************************************************************************
*/

  
  
BOOL HT_Flash_ByteWrite(const uint8_t* pWriteByte, uint32_t Address, uint32_t Num);
void HT_Flash_HalfWordWrite(const uint16_t* pWriteHalfWord, uint32_t Address, uint32_t Num);
void HT_Flash_WordWrite(const uint32_t* pWriteWord, uint32_t Address, uint32_t Num);
void HT_Flash_ByteRead(uint8_t* pReadByte, uint32_t Address, uint32_t Num);
void HT_Flash_HalfWordRead(uint16_t* pReadHalfWord, uint32_t Address, uint32_t Num);
void HT_Flash_WordRead(uint32_t* pReadWord, uint32_t Address, uint32_t Num);
BOOL HT_Flash_PageErase(uint32_t EraseAddress);
void HT_Flash_ChipErase(void);






#ifdef __cplusplus
}
#endif

#endif /* __HT60xx_FLASH_H__ */ 
