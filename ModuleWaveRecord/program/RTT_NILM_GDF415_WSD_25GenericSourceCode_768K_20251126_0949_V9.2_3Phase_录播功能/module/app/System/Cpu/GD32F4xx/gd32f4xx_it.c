/*!
    \file    gd32f4xx_it.c
    \brief   interrupt service routines

    \version 2022-03-08, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f4xx_it.h"
#include "gd32f450v_start.h"
#include "systick.h"
#include "AllHead.h"
extern BYTE LED_USART1Working;
extern DWORD SPIResetCount;//SPI复位次数计数器
extern BYTE SPIResetConvert;
extern BYTE SPIRecError;
extern void MX_SPI3_Init(void);
extern void HAL_TIM_PeriodElapsedCallback(void);
extern void USARTx_Rcv_IRQHandler(BYTE SCI_Ph_Num, BYTE Data);
extern void HAL_SPI_RxCpltCallback(void);
extern void USARTx_SendComplete_IRQHandler(BYTE SCI_Ph_Num);
/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
//
/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
//void PendSV_Handler(void)
//{
//}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler_Root(void)
{
    HAL_TIM_PeriodElapsedCallback();
    delay_decrement();
}

/*!
    \brief      this function handles external lines 0 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_0)){
       // gd_eval_led_toggle(LED2);
    }

    exti_interrupt_flag_clear(EXTI_0);
}
/*!
    \brief      this function handles external lines 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
// 外部CS引脚中断，用于SPI异常恢复
void EXTI10_15_IRQHandler(void)
{
	if(RESET != exti_interrupt_flag_get(EXTI_15)) 
	{
		MX_SPI3_Init();
		SPIResetCount++;
		SPIResetConvert++;
		SPIRecError = 0;
		exti_interrupt_flag_clear(EXTI_15);
	}
}
/*!
    \brief      this function handles external lines 4 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI4_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_4)){
        //gd_eval_led_toggle(LED2);
    }

    exti_interrupt_flag_clear(EXTI_4);
}

void DMA0_Channel0_IRQHandler(void)
{
   rt_interrupt_enter();
   
   
   if ((dma_flag_get(DMA0, DMA_CH0, DMA_INTF_FTFIF) != RESET))
       //|| (dma_flag_get(DMA0, DMA_CH0, DMA_CHXCTL_HTFIE) != RESET) )
    {
          HAL_SPI_RxCpltCallback();
        //zx_hw_serial_isr(serial, ZX_SERIAL_EVENT_TX_DMADONE);	
    }	
   dma_flag_clear(DMA0, DMA_CH0,DMA_INTF_FTFIF);
   
  //wxy    dma_flag_clear(DMA0, DMA_CH0, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);
   
 /*   if(dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF))
    {
        HAL_SPI_RxCpltCallback();
        dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_FTF);
    }
   */ 
   
   rt_interrupt_leave();
   
}
 void DMA1_Channel2_IRQHandler(void);

extern unsigned char LED_USART1Working;

// UART中断接收
void USART0_IRQHandler_Uart0(void)
{
  //DMA1_Channel2_IRQHandler();
 // return;
    if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET)
    {
          
          
          LED_USART1Working = 1;
          USARTx_Rcv_IRQHandler(0, (uint8_t)usart_data_receive(USART0));
         //usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
    }
/*    
    if ( (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET)
	  && (usart_flag_get(USART0, USART_FLAG_RBNE) != RESET) )
    {
       LED_USART1Working = 1;
          USARTx_Rcv_IRQHandler(0, (uint8_t)usart_data_receive(USART0));
        usart_flag_clear(USART0, USART_FLAG_RBNE);
    }
   */
    
}

          /*old 
 if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET)
    {
          
          
          LED_USART1Working = 1;
          USARTx_Rcv_IRQHandler(0, (uint8_t)usart_data_receive(USART0));
         //usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
    }
*/
void SPI2_IRQHandler(void)
{

}

// UART DMA中断发送
void DMA1_Channel7_IRQHandler(void)
{
   rt_interrupt_enter();
   
    if (dma_flag_get(DMA1, DMA_CH7, DMA_INTF_FTFIF) != RESET)
    {
      USARTx_SendComplete_IRQHandler(0);
        //zx_hw_serial_isr(serial, ZX_SERIAL_EVENT_TX_DMADONE);	
    }	
    dma_flag_clear(DMA1, DMA_CH7, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);
    
    rt_interrupt_leave();
}

//extern unsigned char RxBuffer[512];

 void DMA1_Channel2_IRQHandler(void)
{//
//  int nLen;
//  int i;
  /*
    //if (dma_flag_get(DMA1, DMA_CH2, DMA_INTF_FTFIF) != RESET)
    // if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE) != RESET)
     if (dma_interrupt_flag_get(DMA1, DMA_CH2, DMA_INTF_FTFIF) != RESET)
    {
      nLen = 512 - dma_transfer_number_get(DMA1, DMA_CH2);
      
      for(i=0; i<nLen; i++)
          USARTx_Rcv_IRQHandler(0, (uint8_t)RxBuffer[i]);
     // USARTx_SendComplete_IRQHandler(0);
        //zx_hw_serial_isr(serial, ZX_SERIAL_EVENT_TX_DMADONE);	
    }	
   // dma_flag_clear(DMA1, DMA_CH2, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);
    dma_interrupt_flag_clear(DMA1, DMA_CH2,USART_INT_FLAG_IDLE);
    dma_channel_enable(DMA1, DMA_CH2);
  */
}
