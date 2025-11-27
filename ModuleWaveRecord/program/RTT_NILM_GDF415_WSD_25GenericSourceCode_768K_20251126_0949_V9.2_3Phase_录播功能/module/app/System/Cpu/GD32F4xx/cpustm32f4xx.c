//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	wangjunsheng
//创建日期	2020.10.28
//描述		CPU驱动程序
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "gd32f4xx.h"
#include "usart.h"
#include "spi.h"
#include "tim.h"
#include "LED.h"
#include "task_update.h"
#include "dsp_time.h"

#if ((CPU_TYPE == CPU_GD32F415_768K) || (CPU_TYPE == CPU_GD32F415_256K))

//wxy IWDG_HandleTypeDef hiwdg1;

extern void DRV_TIM_Config(unsigned int arr, unsigned int psc);
static DWORD fac_us=0;      //us延时倍乘数

BYTE InitComplete = 0;
BYTE LED_SPI3Working=0;
BYTE LED_USART1Working=0;
BYTE LED_SPIEvent=0;
BYTE SPIITCount = 0;//进入中断后的计时
volatile DWORD PrivateTime=0;

void Error_Handler(void);

void UNUSED(int n)
{
}
/*********************************
 * 函数功能: 初始化系统时钟  160M
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void api_SystemClockConfig_160M(void)
{
	#if 0
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
  */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** Configure the main internal regulator output voltage
  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }
    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 64;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_SPI3 | RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
		#endif
}

/*********************************
 * 函数功能: 初始化系统时钟  200M
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void api_SystemClockConfig_200M(void)
{
#if 0
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
  */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** Configure the main internal regulator output voltage
  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }
    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 2;
    RCC_OscInitStruct.PLL.PLLN = 32;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_SPI3 | RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
		#endif
}

/*********************************
 * 函数功能: 初始化系统时钟  320M
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void api_SystemClockConfig_320M(void)
{
#if 0
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
  */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** Configure the main internal regulator output voltage
  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }
    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 128;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_SPI3 | RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
		#endif
}

/*********************************
 * 函数功能: 初始化系统时钟  400M
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void api_SystemClockConfig_400M(void)
{
#if 0
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
  */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** Configure the main internal regulator output voltage
  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }
    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 2;
    RCC_OscInitStruct.PLL.PLLN = 64;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_SPI3 | RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
		#endif
}

/*********************************
 * 函数功能: 初始化系统时钟  480M
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void api_SystemClockConfig_480M(void)
{
#if 0
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
  */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** Configure the main internal regulator output voltage
  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }
    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_SPI3 | RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
		#endif
}

/*********************************
 * 函数功能: 初始化独立看门狗
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void api_InitIWDG1(void)
{
    fwdgt_write_enable();
    fwdgt_config(1800,FWDGT_PSC_DIV32);
    fwdgt_enable();
    
}

/*********************************
 * 函数功能: 喂独立看门狗
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void IWDG_Feed(void)
{  
    fwdgt_counter_reload();
}

/*********************************
 * 函数功能: 初始化延时函数
 * 参数: SYSCLK--系统时钟频率，单位MHz
 * 返回值: 无
 * 备注: 
 *********************************/
void api_InitDelay(WORD SYSCLK)
{

   //wxy HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
    fac_us=SYSCLK;						    //不论是否使用OS,fac_us都需要使用

}

/*********************************
 * 函数功能: nop延时函数
 * 参数: Steps--延时的nop数
 * 返回值: 无
 * 备注: 
 *********************************/
void api_DelayNop(BYTE Step)
{
	BYTE i;
    
	for(i=0; i<Step; i++)
	{
       // asm("nop");
	}
}

/*********************************
 * 函数功能: 1微秒延时函数
 * 参数: us--延时时间，单位微秒
 * 返回值: 无
 * 备注: 
 *********************************/
void api_Delayus(DWORD us)
{		
	DWORD ticks;
	DWORD told,tnow,tcnt=0;
	DWORD reload=SysTick->LOAD;				//LOAD的值
	ticks=us*fac_us; 						//需要的节拍数
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}

/*********************************
 * 函数功能: 10微秒延时函数
 * 参数: us--延时时间，单位10微秒
 * 返回值: 无
 * 备注: 
 *********************************/
void api_Delay10us(WORD us)
{
	api_Delayus(10*us);
}

/*********************************
 * 函数功能: 100微秒延时函数
 * 参数: us--延时时间，单位100微秒
 * 返回值: 无
 * 备注: 
 *********************************/
void api_Delay100us(WORD us)
{
	api_Delayus(100*us);
}

/*********************************
 * 函数功能: 1毫秒延时函数
 * 参数: ms--延时时间，单位毫秒
 * 返回值: 无
 * 备注: 
 *********************************/
void api_Delayms(WORD ms)
{

    api_Delayus(1000*ms);//HAL_Delay(ms);

}

/*********************************
 * 函数功能: 初始化TIM1
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void SysTimer1_Config(void)
{
	DRV_TIM_Config(99, 2399);//应该是1ms中断一次
}

/*********************************
 * 函数功能: 停止TIM1
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void api_DeInitTIM2(void)
{
#if 0
    HAL_TIM_Base_Stop_IT(&htim2);
		HAL_TIM_Base_MspDeInit(&htim2);
		#endif
                
    
}


/*********************************
 * 函数功能: 获取私有时间
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
DWORD GetPrivateTime()
{
    return PrivateTime;
}

/*********************************
 * 函数功能: 初始化CPU
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/
void api_InitCPU()
{
	#if 0
    /* 重置所有外围设备，初始化闪存接口和Systick */
    HAL_Init();

    /* 初始化时钟 */
    #if(MODULE_CPU_FREQ == CPU_FREQ_160M)
    api_SystemClockConfig_160M();
    #elif(MODULE_CPU_FREQ == CPU_FREQ_200M)
    api_SystemClockConfig_200M();
    #elif(MODULE_CPU_FREQ == CPU_FREQ_320M)
    api_SystemClockConfig_320M();
    #elif(MODULE_CPU_FREQ == CPU_FREQ_400M)
    api_SystemClockConfig_400M();
    #elif(MODULE_CPU_FREQ == CPU_FREQ_480M)
    api_SystemClockConfig_480M();
    #else
    时钟宏定义配置不对
    #endif

   

  
#endif  
    
    /* 初始化延时 */
    api_InitDelay(MODULE_CPU_FREQ);
    
    /* 初始化定时器2 */
    //api_InitTIM2();
    
     /* 初始化看门狗 */
#if(USE_WDG_DEBUG)
    api_InitIWDG1();
#endif  

}

/*********************************
 * 函数功能: 定时器中断处理
 * 参数: htim--定时器句柄
 * 返回值: 无
 * 备注: 
 *********************************/

void HAL_TIM_PeriodElapsedCallback(void)
{
	static DWORD SysTickCounter=0;
	static WORD SPILEDDelay=0;
	static WORD UARTLEDDelay=0;
	BYTE i;

	if(1)
	{
		SysTickCounter++;//1MS定时器
		if( SysTickCounter >= (10000) )
		{
			SysTickCounter = 0;
		}
		 
		if( (SysTickCounter%250) == 0)
		{
			api_SetAllTaskFlag( eFLAG_250_MS );
		}

		if( (SysTickCounter%500) == 0 )
		{
			SPIITCount ++;
			api_SetAllTaskFlag( eFLAG_500_MS );
		}

		// if( (SysTickCounter%10) == 0 )
		{
			//通讯延时用
			for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
			{
				if(Serial[i].ReceToSendDelay > 0)//10ms调用一次
				{
					Serial[i].ReceToSendDelay --;
				}
				if(Serial[i].RxOverCount > 1)//10ms调用一次 注意，必须是大于1
				{
					Serial[i].RxOverCount --;
				}
			}
		}

		PublicTime.msec++;
		PrivateTime++;
		#if(TimeMode==0)
		if(((UsePrintfFlag >> 5) & 0x0001) == 0x0001)
		{
			if((Count_features_002s != 0) && (PublicTime.msec>=1000))
			{
				TimeFun();
				TimeRtcFun();
			}
			else
			{
				Origin = 0;
			}
		}
		else
		{
			if(PublicTime.msec>=1000)//if(DspGlobalTime.wMSec>=1000)//
			{
				TimeFun();
				TimeRtcFun();
			}
		}
		#endif

		// SPI指示灯闪烁
		if(LED_SPI3Working==1)
		{
			// 为避免灯灭的太快，延迟一段时间再初始化状态
			SPILEDDelay=500;
			LED_SPI3Working=0;
		}
		if(SPILEDDelay>0)
		{
			SPILEDDelay--;
			if( (SysTickCounter%1000) == 0 )
			{
				api_SetUpLED(LED_RED, 2);
			}
		}
		else
		{
			api_SetUpLED(LED_RED, 1);
		}
		// UART指示灯闪烁
		if(LED_USART1Working==1)
		{
			// 为避免灯灭的太快，延迟一段时间再初始化状态
			UARTLEDDelay=500;
			LED_USART1Working=0;
		}
		if(UARTLEDDelay>0)
		{
			UARTLEDDelay--;
			if( (SysTickCounter%125) == 0 )
			{
				api_SetUpLED(LED_GREEN, 2);
			}
		}
		else
		{
			if(InitComplete==0)
			{
				api_SetUpLED(LED_GREEN, 1);
			}
			else
			{
				api_SetUpLED(LED_GREEN, 0);
			}
		}
	}
}

//-----------------------------------------------
//函数功能: 读取cpu内ram及片上flash数据
//
//参数: 	Type[in]	0--Ram 	1--flash
//          Addr[in]	地址 
//			Len[in]		读取长度    
//			Buf[out]	输出缓冲     
//
//返回值:  	读取数据的长度，如果为0，表示读取失败
//
//备注:       flash地址为0x8000000~0x8040000、ram地址为0x20000000~0x20008000
//-----------------------------------------------
BYTE api_ReadCpuRamAndInFlash(BYTE Type, DWORD Addr, BYTE Len, BYTE *Buf)
{
	if( Type > 1 )
	{
		return 0;
	}
	#if(CPU_TYPE == CPU_GD32F415_768K)
	if( (Addr>=0x081FFFFF) && (Addr<0x20000000) )
	{
		return 0;
	}
	if( (Addr>=0x200AFFFF) || (Addr<0x08010000) )
	{
		return 0;
	}
	
	if( ((Addr+Len)>=0x081FFFFF) && ((Addr+Len)<0x20000000) )
	{
		return 0;
	}
	if( ((Addr+Len)>=0x200AFFFF) || ((Addr+Len)<0x08010000) )
	{
		return 0;
	}
	#elif(CPU_TYPE == CPU_GD32F415_256K)
	if( (Addr>=0x080FFFFF) && (Addr<0x20000000) )
	{
		return 0;
	}
	if( (Addr>=0x2002FFFF) || (Addr<0x08010000) )
	{
		return 0;
	}
	
	if( ((Addr+Len)>=0x080FFFFF) && ((Addr+Len)<0x20000000) )
	{
		return 0;
	}
	if( ((Addr+Len)>=0x2002FFFF) || ((Addr+Len)<0x08010000) )
	{
		return 0;
	}
	#endif//	#if(CPU_TYPE == CPU_GD32F415_768K)

	
	memcpy(Buf,(BYTE*)Addr,Len);
	
	return Len;
}

//-----------------------------------------------
//函数功能: 启动程序升级
//
//参数: 	pBuf[in] 输入缓冲     
//
//返回值:  	此函数最后会执行数据转存及软复位
//
//备注: 规约见《电表软件平台扩展规约》附录C  
//-----------------------------------------------
void api_StartUpdateProgMessage(BYTE *pBuf)
{

    api_SetUpdateFlag(TRUE);

    __set_FAULTMASK(1);
    NVIC_SystemReset();

}

////////////////////////////////////////////////////////////////
//                          串口部分
////////////////////////////////////////////////////////////////

/*********************************
 * 函数功能: 处理更改波特率后的串口初始化
 * 参数: Channel--逻辑串口号
 * 返回值: 无
 * 备注: 
 *********************************/
void DealSciFlag( BYTE Channel )
{
    //有改波特操作，电表应答完再改波特率
    if( Serial[Channel].OperationFlag & BIT0 )
    {
    	Serial[Channel].OperationFlag &= ~BIT0;
    	SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
    }
}

/*********************************
 * 函数功能: 串口初始化
 * 参数: Channel--逻辑串口号
 * 返回值: 无
 * 备注: 
 *********************************/
void api_InitSci( BYTE Channel )
{
	SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
}

/*********************************
 * 函数功能: 禁止指定串口接收
 * 参数: p--需要允许串口的指针
 * 返回值: 无
 * 备注: 
 *********************************/
void api_DisableRece(TSerial * p)
{
	BYTE i;
	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( p == &Serial[i] )
		{
			SerialMap[i].SCIDisableRcv(SerialMap[i].SCI_Ph_Num);
		}
	}
}

/*********************************
 * 函数功能: 允许指定接口接收
 * 参数: p--需要允许串口的指针
 * 返回值: 无
 * 备注: 
 *********************************/
void api_EnableRece(TSerial * p)
{
	BYTE i;
	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( p == &Serial[i] )
		{
			SerialMap[i].SCIEnableRcv(SerialMap[i].SCI_Ph_Num);
		}
	}
}

/*********************************
 * 函数功能: 串口接收中断
 * 参数: SCI_Ph_Num--物理串口号
 *       Data--接收到的数据
 * 返回值: 无
 * 备注: 
 *********************************/
void USARTx_Rcv_IRQHandler(BYTE SCI_Ph_Num, BYTE Data)
{
    BYTE i;

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
        {
            if(Serial[ i ].RXWPoint < MAX_PRO_BUF)
            {
	            Serial[ i ].ProBuf[Serial[ i ].RXWPoint++] = Data;
            }

            if(Serial[ i ].RXWPoint >= MAX_PRO_BUF)
            {
                Serial[ i ].RXWPoint = 0;
            }

            break;
        }
    }
}



/*********************************
 * 函数功能: 串口发送完成中断
 * 参数: SCI_Ph_Num--物理串口号
 *       USARTx--对应的串口指针
 * 返回值: 无
 * 备注: 
 *********************************/

void USARTx_SendComplete_IRQHandler(BYTE SCI_Ph_Num)
{
    BYTE i, j;

 //   UNUSED(USARTx);

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
        {
            //如果有波特率改动 发完数据更新波特率 必须每个都遍历到 存在用485_1设置485_2的情况
            for( j = 0; j < MAX_COM_CHANNEL_NUM; j++ )
            {
                DealSciFlag( j );
            }
            
            //立转接收状态
            DealSciFlag(i);
            SerialMap[i].SCIEnableRcv(SerialMap[i].SCI_Ph_Num);
            api_InitSciStatus(i);
            break;
        }
    }
}
#if 0
////////////////////////////////////////////////////////////////
//                       SPI1 FLASH 部分
////////////////////////////////////////////////////////////////

/*********************************
 * 函数功能: 初始化SPI1
 * 参数: Component--SPI各器件类型
 *       Type--ST平台无实际作用，仅为保持与HT平台兼容
 * 返回值: 读出的值
 * 备注: 
 *********************************/
void api_InitSPI( eCOMPONENT_TYPE Component, eSPI_MODE Type )
{
    UNUSED(Type);

    MX_SPI1_Init();

}
#endif
/*********************************
 * 函数功能: 非中断方式下写SPI1的函数
 * 参数: Component--SPI各器件类型
 *       Data--写入的值
 * 返回值: 读出的值
 * 备注: 
 *********************************/
BYTE api_UWriteSpi( eCOMPONENT_TYPE Component, BYTE Data )
{
    BYTE RxData=0;

	if( Component == eCOMPONENT_SPI_SAMPLE )
	{
		//HAL_SPI_TransmitReceive(&hspi3, &Data, &RxData, 1, 300);    //TODO: 此处延时300ms是否合理？
	}
    else if( Component == eCOMPONENT_SPI_FLASH )
    {
        	
          while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));

          spi_i2s_data_transmit(SPI0, Data);
          while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
    
          return (spi_i2s_data_receive(SPI0));
    }

	return RxData;
}


/*********************************
 * 函数功能: 计算cpu Flash的累加和，采用word相加模式
 * 参数: Type--地址范围
 * 返回值: 计算的word累加和
 * 备注: 
 *********************************/
WORD api_CheckCpuFlashSum(BYTE Type)
{   
    DWORD Sum, Len;
	DWORD StartZone,EndZone;
    WORD *pData;
    
    CLEAR_WATCH_DOG;
	
	#if(( CPU_TYPE != CPU_GD32F415_256K ) && ( CPU_TYPE != CPU_GD32F415_768K ))
	换成其他cpu需要重新配置起始截止地址
	#endif

	if( Type == 0 )             // BOOT程序地址范围
	{
		StartZone = 0x08000000;
		EndZone = 0x0800FFE0;
	}
	else if(  Type == 1 )       // app程序地址范围
	{
		Len = api_GetCodeBufferLen(api_GetCodeBufferFlag(1));
		
		if(Len > CODEBUFFER_SIZE)
		{
			Len = CODEBUFFER_SIZE;
		}

		StartZone = 0x08010000;
		EndZone = StartZone + Len;
	}
    // else if(  Type == 2 )       // 参数常量地址范围
	// {
	// 	StartZone = 0x0803f000;
	// 	EndZone = 0x08040000;
	// }
	else                        // 全部FLASH地址范围
	{
		#if( CPU_TYPE == CPU_GD32F415_256K )
		StartZone = 0x20000000;
		EndZone = 0x2002FFFF;
		#elif( CPU_TYPE == CPU_GD32F415_768K )
		StartZone = 0x20000000;
		EndZone = 0x200AFFFF;
		#endif
	}
	
	Sum = 0;
 
	for( pData = (WORD *)StartZone; pData < (WORD *)EndZone; pData++ )
	{
		Sum += *pData;
	}
    
    CLEAR_WATCH_DOG;
    
    return (WORD)Sum;
}

/*********************************
 * 函数功能: 处理错误
 * 参数: 无
 * 返回值: 无
 * 备注: 
 *********************************/

#endif  // #if ((CPU_TYPE == CPU_GD32F415_768K) || (CPU_TYPE == CPU_GD32F415_256K))
