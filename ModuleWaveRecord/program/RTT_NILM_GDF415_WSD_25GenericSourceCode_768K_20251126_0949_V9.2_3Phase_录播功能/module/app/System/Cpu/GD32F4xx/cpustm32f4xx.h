//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	wangjunsheng
//创建日期	2020.10.28
//描述		cpu头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __CPUSTH7_H
#define __CPUSTH7_H

#if ((CPU_TYPE == CPU_GD32F415_768K) || (CPU_TYPE == CPU_GD32F415_256K))

#define CLEAR_WATCH_DOG IWDG_Feed()

#define FLASH_ADDR_BOOT_VERSION     0x0800ffe0

extern BYTE InitComplete;
extern BYTE LED_SPI3Working;
extern BYTE LED_USART1Working;
extern BYTE LED_SPIEvent;

DWORD GetPrivateTime(void);

void api_InitCPU(void);

void api_InitSci( BYTE Channel );

//void USARTx_Rcv_IRQHandler(BYTE SCI_Ph_Num, BYTE Data);
//void USARTx_SendComplete_IRQHandler(BYTE SCI_Ph_Num, USART_TypeDef* USARTx);

BYTE api_UWriteSpi( eCOMPONENT_TYPE Component, BYTE Data );

////////////////////////////////////////////////////////////////
//                          延时函数
////////////////////////////////////////////////////////////////
void api_DelayNop(BYTE Step);
void api_Delayus(DWORD us);
void api_Delay10us(WORD us);
void api_Delay100us(WORD us);
void api_Delayms(WORD ms);

void IWDG_Feed(void);

WORD api_CheckCpuFlashSum(BYTE Type);
BYTE api_ReadCpuRamAndInFlash(BYTE Type, DWORD Addr, BYTE Len, BYTE *Buf);
void api_StartUpdateProgMessage(BYTE *pBuf);

#endif  // #if ((CPU_TYPE == CPU_GD32F415_768K) || (CPU_TYPE == CPU_GD32F415_256K))

#endif//#ifndef __CPUST091_H
