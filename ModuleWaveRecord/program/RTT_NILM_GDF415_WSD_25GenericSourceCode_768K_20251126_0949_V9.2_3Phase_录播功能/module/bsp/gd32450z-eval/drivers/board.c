/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>

#include <gd32f4xx.h>
#include <board.h>
#include <drv_usart.h>
#include <sys.h>

extern void SysTick_Handler_Root(void);
extern void SysTimer1_Config(void);
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
    {
    }
    /* USER CODE END Error_Handler */
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    NVIC_SetPriority(SysTick_IRQn, 0);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

   // SysTick_Handler_Root();
    /* leave interrupt */
    rt_interrupt_leave();
}

void TIMER1_IRQHandler(void)
{
  rt_interrupt_enter();
  
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP))
    {
        //添加定时处理代码
        SysTick_Handler_Root();

        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    }
    
     rt_interrupt_leave();
}

/**
 * This function will initial GD32 board.
 */
void rt_hw_board_init()
{
    /* NVIC Configuration */
#define NVIC_VTOR_MASK              0x3FFFFF80
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x10000000 */
    SCB->VTOR  = (0x10000000 & NVIC_VTOR_MASK);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    SCB->VTOR  = (0x08010000 & NVIC_VTOR_MASK);//测试用:用于修改中断向量表偏移，有BOOT功能时修改此值为0x08010000，否则可用0x08000000
#endif

    SystemClock_Config();

    SysTimer1_Config();
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#ifdef BSP_USING_SDRAM
    rt_system_heap_init((void *)EXT_SDRAM_BEGIN, (void *)EXT_SDRAM_END);
#else
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif
}

/*@}*/
