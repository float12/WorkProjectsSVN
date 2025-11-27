/*!
    \file    systick.c
    \brief   the systick configuration file

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

#include "gd32f4xx.h"
#include "systick.h"

volatile static uint32_t delay;

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if(SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while(1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds
    \param[out] none
    \retval     none
*/
void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay){
    }
}

/*!
    \brief      delay decrement
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_decrement(void)
{
    if(0U != delay){
        delay--;
    }
}

void DRV_TIM_Config(unsigned int arr, unsigned int psc)
{
    timer_parameter_struct initpara;
    rcu_periph_clock_enable(RCU_TIMER1);//开启时钟
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//对应时钟源选择CFG1->TIMERSEL选择为1
    initpara.prescaler         = psc;//设置用来作为TIMx时钟频率除数的预分频值
    initpara.period            = arr;//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    initpara.alignedmode       = TIMER_COUNTER_EDGE;
    initpara.counterdirection  = TIMER_COUNTER_UP;
    initpara.clockdivision     = TIMER_CKDIV_DIV1;

    timer_init(TIMER1, &initpara);
    //清状态，设置中断
    timer_flag_clear(TIMER1, TIMER_FLAG_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    //定时器中断
    /* enable and set timer interrupt priority */
    nvic_irq_enable(TIMER1_IRQn, 1U, 1U);
    timer_enable(TIMER1);
}


//1ms定时  200MHZ

//定时器中断


    