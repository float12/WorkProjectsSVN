//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	wangjunsheng
//创建日期	2020.10.28
//描述		支持ST芯片的电能质量模块印制板驱动文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "gd32f4xx.h"
#include "usart.h"
#include "spi.h"
#include "tim.h"
//#include "task_sample.h"
#include "LED.h"

//#include <stdint.h>
#if (BOARD_TYPE == BOARD_ST_PQM)

volatile USARTCOUNT USART_Count;

static BYTE USART1RxByte = 0;

//static BYTE SPI3DMAFrameBuf[ONE_FRAME_LEN] __attribute__((section("RW_SRAM_DMA"),aligned (8)));

//__no_init BYTE SPI3DMAFrameBuf[ONE_FRAME_LEN];//@0x20020000;// __attribute__((section("RW_SRAM_DMA"),aligned (8)));

static BOOL InitPhSci(BYTE SciPhNum);
static BOOL SciRcvEnable(BYTE SciPhNum);
static BOOL SciRcvDisable(BYTE SciPhNum);
static BOOL SciSendEnable(BYTE SciPhNum);
static BOOL SciBeginSend(BYTE SciPhNum);

// 串口映射图（各个板子不一样）
const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM] = {
    // 第一路RS-485
    eUSART_I,
    0,                 // MCU 串口号
    &InitPhSci,             // 串口初始化
    &SciRcvEnable,          // 接收允许
    &SciRcvDisable,         // 禁止接收
    &SciSendEnable,         // 发送允许
    &SciBeginSend,          // 开始发送
};

/*********************************
 * 函数功能: 初始化LED指示灯
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void InitLED()
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_RED|LED_GREEN);
    
    gpio_output_options_set(GPIOD,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, LED_RED|LED_GREEN);
    
    api_SetUpLED(LED_RED, 1);
    api_SetUpLED(LED_GREEN, 1);

}

/*********************************
 * 函数功能: 初始化USART1
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void InitUSART1()
{
    /* 使能DMA时钟 */
   // rcu_periph_clock_enable(RCU_DMA1); //__HAL_RCC_DMA1_CLK_ENABLE();
    
    /* 初始化USART1 */
    MX_USART1_UART_Init(115200);

    /* 使能USART1接收中断 */
  //  HAL_UART_Receive_IT(&huart1, &USART1RxByte, 1);

}

/*********************************
 * 函数功能: 初始化SPI1
 * 参数: 无
 * 返回值: 无
 * 备注: SPI1用于读写Flash
 *********************************/
void InitSPI1Flash()
{
    MX_SPI1_Init();
}

/*********************************
 * 函数功能: 初始化SPI3
 * 参数: 无
 * 返回值: 无
 * 备注: SPI3用于与计量芯通讯，
 *       获取采样数据
 *********************************/
void InitSPI3Sample()
{

    /* 使能DMA时钟 */
  //  rcu_periph_clock_enable(RCU_DMA1);

    /* 初始化SPI3 */
   // MX_SPI3_Init();

    /* 使能SPI3接收中断 */
  //  memset((void *)SPI3DMAFrameBuf, 0, sizeof(SPI3DMAFrameBuf));
   
  //  HAL_SPI_Receive_DMA(&hspi3, SPI3DMAFrameBuf, ONE_FRAME_LEN);

}

/*********************************
 * 函数功能: 初始化I2C
 * 参数: 无
 * 返回值: 无
 * 备注: I2C用于读写EEPROM
 *********************************/
void InitI2CEEPROM()
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);
    
     
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, IIC_SCL_Pin|IIC_SDA_Pin);
    
    gpio_output_options_set(GPIOD,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ, IIC_SCL_Pin|IIC_SDA_Pin);
    
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);
    gpio_output_options_set(GPIOD,GPIO_OTYPE_PP,GPIO_OSPEED_25MHZ, GPIO_PIN_0);
       
    gpio_bit_reset(GPIOB,IIC_SCL_Pin);
    gpio_bit_reset(GPIOB,IIC_SDA_Pin);
    
    gpio_bit_reset(GPIOE,GPIO_PIN_0);
    

}

void InitBoard()
{
    /* 初始化LED指示灯 */
    InitLED();

    /* 初始化USART1 */
    InitUSART1();

    /* 初始化SPI1 */
   InitSPI1Flash();

    /* 初始化I2C */
    InitI2CEEPROM();

    /* 初始化SPI3 */
    InitSPI3Sample();
//
}

////////////////////////////////////////////////////////////////
//                          串口部分
////////////////////////////////////////////////////////////////

/*********************************
 * 函数功能: 串口初始化
 * 参数: SciPhNum--物理串口号
 * 返回值: 无
 * 备注: 
 *********************************/
const BYTE WATCH_SCI_TIMER = 65;//7 考虑手动组包读后续数据 wlk 2009-7-16
static BOOL InitPhSci(BYTE SciPhNum)
{
    WORD i;

    UNUSED(SciPhNum);

    //HAL_UART_DeInit(&huart1);

    MX_USART1_UART_Init(115200);

    //wxy HAL_UART_Receive_IT(&huart1, &USART1RxByte, 1);

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        api_InitSubProtocol(i);
        //串口监视定时器
        Serial[ i ].WatchSciTimer = WATCH_SCI_TIMER;
    }

    return TRUE;
}

/*********************************
 * 函数功能: 串口接收允许
 * 参数: SciPhNum--物理串口号
 * 返回值: 无
 * 备注: 
 *********************************/
static BOOL SciRcvEnable(BYTE SciPhNum)
{
#if 0
    UNUSED(SciPhNum);

    SET_BIT((&huart1)->Instance->CR1, USART_CR1_RE );
#endif
    return TRUE;
}

/*********************************
 * 函数功能: 串口禁止接收
 * 参数: SciPhNum--物理串口号
 * 返回值: 无
 * 备注: 
 *********************************/
static BOOL SciRcvDisable(BYTE SciPhNum)
{
    WORD i;

    //wxy CLEAR_BIT((&huart1)->Instance->CR1, USART_CR1_RE );

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SciPhNum == SerialMap[i].SCI_Ph_Num)
        {
            Serial[i].WatchSciTimer = WATCH_SCI_TIMER;
        }
    }

    return TRUE;
}

/*********************************
 * 函数功能: 串口发送允许
 * 参数: SciPhNum--物理串口号
 * 返回值: 无
 * 备注: 
 *********************************/
static BOOL SciSendEnable(BYTE SciPhNum)
{

    UNUSED(SciPhNum);

    return TRUE;
}

/*********************************
 * 函数功能: 串口开始发送
 * 参数: SciPhNum--物理串口号
 * 返回值: 无
 * 备注: 
 *********************************/
static BOOL SciBeginSend(BYTE SciPhNum)
{

    BOOL Result=FALSE;

    LED_USART1Working = 1;
    
    switch (SciPhNum)
    {
    case 0:
        USART_Count.TxBegCount++;
         _drv_uart_dma_start( Serial[SciPhNum].ProBuf, Serial[SciPhNum].SendLength);
         Result = TRUE;
        //usart_data_transmit(USART0, Serial[0].ProBuf[Serial[0].TXPoint++]);
        //usart_interrupt_enable(USART0, USART_INT_FLAG_TC);
        //Result = HAL_UART_Transmit_DMA(&huart1, Serial[SciPhNum].ProBuf, Serial[SciPhNum].SendLength);
        break;
    default:
        break;
    }

    if( Result == TRUE)
    {
        Serial[SciPhNum].SendingFlag = 1;
        return TRUE;
    }
    USART_Count.TxDMAErrorCount++;

    return FALSE;
}

/*********************************
 * 函数功能: 串口接收完成中断处理
 * 参数: huart--串口句柄
 * 返回值: 无
 * 备注: 
 *********************************/
#if 0
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        LED_USART1Working = 1;

        USARTx_Rcv_IRQHandler(0, USART1RxByte);
        
        HAL_UART_Receive_IT(&huart1, &USART1RxByte, 1);
    }
}

/*********************************
 * 函数功能: 串口发送完成中断处理
 * 参数: huart--串口句柄
 * 返回值: 无
 * 备注: 
 *********************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        USART_Count.TxCptCount++;
        USARTx_SendComplete_IRQHandler(0,USART1);
			
				//api_SetUpLED(LED_GREEN, 2);
    }
}

/*********************************
 * 函数功能: 串口错误中断处理
 * 参数: huart--串口句柄
 * 返回值: 无
 * 备注: 
 *********************************/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
    USART_Count.ErrorCallbackCount++;
    api_InitSci(0);
}
#endif
////////////////////////////////////////////////////////////////
//                       SPI1 FLASH 部分
////////////////////////////////////////////////////////////////

/*********************************
 * 函数功能: SPI片选驱动函数
 * 参数: No--需要选择的芯片
 *       Do--操作类型   TRUE-使能片选    FALSE-禁止片选
 * 返回值: 无
 * 备注: 
 *********************************/
void DoSPICS(WORD No, WORD Do)
{

    switch( No )
    {
        case CS_SPI_FLASH:
            FLASH_CS_DISABLE;
            break;
        default:
            break;
    }

    
    if( Do == FALSE )
    {
        return;
    }
    
    switch( No )
    {
        case CS_SPI_FLASH:
            FLASH_CS_ENABLE;
            break;
        default:
            break;
    }
    
    api_Delay10us(1);

    return;
}

////////////////////////////////////////////////////////////////
//                      SPI3 获取采样值部分
////////////////////////////////////////////////////////////////

/*********************************
 * 函数功能: SPI片选驱动函数
 * 参数: No--需要选择的芯片
 *       Do--操作类型   TRUE-使能片选    FALSE-禁止片选
 * 返回值: 无
 * 备注: 
 *********************************/

void HAL_SPI_RxCpltCallback(void)
{

   //     LED_SPI3Working = 1;

    //    DoSPIReceProc(SPI3DMAFrameBuf);
                                                                                            
        //memset((void *)SPI3DMAFrameBuf, 0, sizeof(SPI3DMAFrameBuf));

      //  dma_memory_address_config(DMA0,DMA_CH0,0,(uint32_t)SPI3DMAFrameBuf);
      //  dma_transfer_number_config(DMA0,DMA_CH0, ONE_FRAME_LEN);

}

	
#endif //#if( (BOARD_TYPE == BOARD_ST_PQM) )
