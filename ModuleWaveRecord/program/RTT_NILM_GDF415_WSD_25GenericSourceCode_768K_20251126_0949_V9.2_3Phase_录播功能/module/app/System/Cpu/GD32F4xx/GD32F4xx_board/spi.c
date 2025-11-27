/**
  ******************************************************************************
  * File Name          : SPI.c
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
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
#include "spi.h"
#include "task_sample.h"

//SPI_HandleTypeDef hspi1;
//SPI_HandleTypeDef hspi3;
//DMA_HandleTypeDef hdma_spi3_rx;

/* SPI1 init function */
void MX_SPI1_Init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI0);
   
   
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP,  GPIO_OSPEED_25MHZ,GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
   
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_32;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* quad wire SPI_IO2 and SPI_IO3 pin output enable */
    //qspi_io23_output_enable(SPI5);
    spi_crc_polynomial_set(SPI0,10);
    
    /* enable SPI0 */
    spi_enable(SPI0);
 }


/* SPI3 init function */
extern int m_SampleNum ;
extern int nChannel ;//CHMAX;

extern BYTE SPI3DMAFrameBuf[ONE_FRAME_LEN];

void MX_SPI3_Init(void)
{
  
    spi_parameter_struct spi_init_struct;
    dma_single_data_parameter_struct dma_init_struct;
    
    dma_interrupt_disable(DMA0, DMA_CH0, DMA_CHXCTL_FTFIE);  
    
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_SPI2);
    rcu_periph_clock_enable(RCU_DMA0);
    
    gpio_af_set(GPIOC, GPIO_AF_6, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP,  GPIO_OSPEED_50MHZ, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
   
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    //gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    //gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
	nvic_irq_disable(EXTI10_15_IRQn);// 关闭SPI的CS引脚中断
	
    spi_init_struct.trans_mode           = SPI_TRANSMODE_RECEIVEONLY;//SPI_TRANSMODE_RECEIVEONLY;//SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_SLAVE;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_HARD;//SPI_NSS_HARD;//SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_32;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI2, &spi_init_struct);
    
    //spi_ti_mode_disable(SPI2);
    //spi_nss_internal_low(SPI2);
    
    spi_crc_polynomial_set(SPI2,10);
    spi_enable(SPI2);
    
   // i2s_enable(SPI2);
   // spi_i2s_interrupt_enable(SPI2,SPI_I2S_INT_RBNE);
    
    //return;
    
    dma_channel_disable(DMA0, DMA_CH0);
    
    dma_deinit(DMA0,DMA_CH0);
    dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(SPI2);
    dma_init_struct.memory0_addr = (uint32_t)SPI3DMAFrameBuf;
    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;//DMA_MEMORY_TO_PERIPH;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_HIGH;
    dma_init_struct.number = (1+1+2+1+m_SampleNum*CHMAX*3+4+1);//ONE_FRAME_LEN;  (1+1+2+1+m_SampleNum*CHMAX*3+4+1);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//DMA_PERIPH_INCREASE_ENABLE;//;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;//DMA_MEMORY_INCREASE_DISABLE;//;
    dma_single_data_mode_init(DMA0, DMA_CH0,&dma_init_struct);
    //dma_init(DMA0, DMA_CH0,&dma_init_struct);
    memset((void *)SPI3DMAFrameBuf, 0, sizeof(SPI3DMAFrameBuf));
	spi_dma_enable(SPI2, SPI_DMA_RECEIVE);//SPI_DMA_TRANSMIT
		
	dma_circulation_enable(DMA0, DMA_CH0);
	dma_channel_subperipheral_select(DMA0, DMA_CH0, DMA_SUBPERI0);
	
	dma_channel_enable(DMA0, DMA_CH0);
	nvic_irq_enable(DMA0_Channel0_IRQn, 2U, 2U);
	
	dma_interrupt_enable(DMA0, DMA_CH0, DMA_CHXCTL_FTFIE);  
		

    return;
   
}
#if 0
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
    PB4 (NJTRST)     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    PB6     ------> SPI1_CSN
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  }
  else if(spiHandle->Instance==SPI3)
  {
    /* SPI3 clock enable */
    __HAL_RCC_SPI3_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**SPI3 GPIO Configuration
    PA15 (JTDI)     ------> SPI3_NSS
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* SPI3 DMA Init */
    /* SPI3_RX Init */
    hdma_spi3_rx.Instance = DMA1_Stream0;
    hdma_spi3_rx.Init.Request = DMA_REQUEST_SPI3_RX;
    hdma_spi3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi3_rx.Init.Mode = DMA_NORMAL;
    hdma_spi3_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_spi3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi3_rx);

    /* SPI3 interrupt Init */
    HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI3_IRQn);

    /* DMA interrupt init */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, , 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

  }

}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
    PB4 (NJTRST)     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);

  }
  else if(spiHandle->Instance==SPI3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI3_CLK_DISABLE();

    /**SPI3 GPIO Configuration
    PA15 (JTDI)     ------> SPI3_NSS
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);

    /* SPI3 DMA DeInit */
    HAL_DMA_DeInit(spiHandle->hdmarx);

    /* SPI3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI3_IRQn);
  }
	

}
	#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
