/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "AllHead.h"
#include "usart.h"

//UART_HandleTypeDef huart1;
//DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Gpio(void)
{
   
    rcu_periph_clock_enable( RCU_GPIOA);

   /* configure USART Rx as alternate function push-pull */
  //  gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP,GPIO_PIN_10);
  //  gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_10);
     
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_9|GPIO_PIN_10);
    gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, GPIO_PIN_9|GPIO_PIN_10);
      
    gpio_bit_set(GPIOA,GPIO_PIN_9);
    gpio_bit_set(GPIOA,GPIO_PIN_10);
    
    api_Delayms(1000);
    gpio_bit_reset(GPIOA,GPIO_PIN_9);
    api_Delayms(1000);
    gpio_bit_set(GPIOA,GPIO_PIN_9);
       api_Delayms(1000);
    gpio_bit_reset(GPIOA,GPIO_PIN_9);
}

void MX_USART1_UART_Init(int nbaud)
{
    nvic_irq_disable(USART0_IRQn);
    
    usart_disable(USART0);
    
    rcu_periph_clock_enable( RCU_GPIOA );

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_10);

    NVIC_SetPriority(USART0_IRQn, 6);
    NVIC_EnableIRQ(USART0_IRQn);
    
    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0,nbaud);
    usart_parity_config(USART0,USART_PM_EVEN);
    usart_word_length_set(USART0,USART_WL_9BIT);
    usart_stop_bit_set(USART0,USART_STB_1BIT);
   // usart_data_first_config(USART0,USART_MSBF_MSB);
    //usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
   // usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
    
    nvic_irq_enable(USART0_IRQn, 2U, 0U);
    
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    
    usart_enable(USART0);
    
     usart_flag_clear(USART0, USART_FLAG_TC);
   
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    
  // usart_interrupt_enable(USART0, USART_INT_TBE);
    
}


void MX_USART1_UART_Init_Dma(void)
{
  	dma_single_data_parameter_struct dma_init_parameter;
	unsigned char Buffer[100];	
        
   //     MX_USART1_UART_Gpio();
   //     return;
        
        MX_USART1_UART_Init(115200);
        
	rcu_periph_clock_enable(RCU_DMA1);

	dma_channel_disable(DMA1, DMA_CH7);
	dma_deinit(DMA1, DMA_CH7);
	
	dma_init_parameter.direction			= DMA_MEMORY_TO_PERIPH;
	dma_init_parameter.periph_memory_width	= DMA_PERIPH_WIDTH_8BIT;
	dma_init_parameter.periph_inc			= DMA_PERIPH_INCREASE_DISABLE;
	dma_init_parameter.memory_inc			= DMA_MEMORY_INCREASE_ENABLE;
	dma_init_parameter.priority 			= DMA_PRIORITY_ULTRA_HIGH;
	dma_init_parameter.circular_mode		= DMA_CIRCULAR_MODE_DISABLE;
	dma_single_data_mode_init(DMA1, DMA_CH7, &dma_init_parameter);
	
	usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
		
	dma_circulation_disable(DMA1, DMA_CH7);
	dma_channel_subperipheral_select(DMA1, DMA_CH7, DMA_SUBPERI4);
        
        nvic_irq_enable(DMA1_Channel7_IRQn, 2U, 0U);
        
       // _drv_uart_dma_start(Buffer,10);
}


int _drv_uart_dma_start( uint8_t *buf,   size_t size )
{
	dma_single_data_parameter_struct dma_init_parameter; 
		
	dma_flag_clear(DMA1,DMA_CH7, DMA_INTC_FEEIFC|DMA_INTC_SDEIFC|DMA_INTC_TAEIFC|DMA_INTC_HTFIFC|DMA_INTC_FTFIFC);

	dma_memory_address_config(DMA1, DMA_CH7, DMA_MEMORY_0, (uint32_t)buf);
	dma_periph_address_config(DMA1, DMA_CH7, (uint32_t)&USART_DATA(USART0));
	dma_transfer_number_config(DMA1, DMA_CH7, size);
		
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	dma_interrupt_enable(DMA1, DMA_CH7, DMA_CHXCTL_FTFIE);	

	dma_channel_enable(DMA1, DMA_CH7);

	return size;
}

#if 0


void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Stream1;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_USART1_TX;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    /* DMA1_Stream1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  }
}

#if 0
void MX_USART1_UART_Init(void)
{
    nvic_irq_disable(USART0_IRQn);
    
    usart_disable(USART0);
    
    rcu_periph_clock_enable( GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_10);

    NVIC_SetPriority(USART0_IRQn, 6);
    NVIC_EnableIRQ(USART0_IRQn);
    
    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0,9600U);
    usart_parity_config(USART0,USART_PM_EVEN);
    usart_word_length_set(USART0,USART_WL_8BIT);
    usart_stop_bit_set(USART0,USART_STB_1BIT);
    usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
    
    nvic_irq_enable(USART0_IRQn, 2U, 0U);
    
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    
    usart_enable(USART0);
    
     usart_flag_clear(USART0, USART_FLAG_TC);
   
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    
   usart_interrupt_enable(USART0, USART_INT_TBE);
    
}

#endif

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Stream1;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_USART1_TX;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    /* DMA1_Stream1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  }
}
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


#endif
