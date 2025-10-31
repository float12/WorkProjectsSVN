//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		CPU驱动程序 //这个文件要只留公共的部分，尽量不要用宏判断
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "hc32f4xx_conf.h"
#include "hc32_ddl.h"
#include "cpuHC32F4A0.h"

#if ( CPU_TYPE == CPU_HC32F4A0 )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// Sample SPI Config
/* SPI unit and clock definition */
#define SPI_SAMPLE_UNIT					(CM_SPI3)
#define SPI_SAMPLE_CLOCK                (FCG1_PERIPH_SPI3)

// Sample SPI Config DATA
/* SPI unit and clock definition */
// #define SPI_SAMPLE_DATA_UNIT			(M4_SPI4)
// #define SPI_SAMPLE_DATA_CLOCK           (PWC_FCG1_PERIPH_SPI4)
// #define SPI_SAMPLE_DATA_TX_INT          (INT_SPI4_SPTI)
// #define SPI_SAMPLE_DATA_RX_INT          (INT_SPI4_SPRI)
// Flash SPI Config
/* SPI unit and clock definition */
#define SPI_FLASH_UNIT					(CM_SPI1)
#define SPI_FLASH_CLOCK					(FCG1_PERIPH_SPI1)

#define SYSTEM_POWER_PORT				(PortB)
#define SYSTEM_POWER_PIN				(Pin01)

//ADC Config
//时钟电池ADC
#define ADC_UNIT_1						(CM_ADC1)
#define ADC_SEQ_1						(ADC_SEQ_A)
#define ADC_CH_1						(ADC_CH12)
//掉电监测ADC
#define ADC_UNIT_2						(CM_ADC1)
#define ADC_SEQ_2						(ADC_SEQ_A)
#define ADC_CH_2						(ADC_CH13)

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//SPI 功能管脚对应关系
const MCU_IO Spi_IO[][3] =
{
	{
		 PortB, Pin02, Func42_Spi1_Miso,
        PortB, Pin01, Func41_Spi1_Mosi,
        PortB, Pin00, Func40_Spi1_Sck, 
	},
};

//ADC 功能管脚对应关系
const MCU_IO Adc_IO[] =
{
	{ PortC, Pin02, Func0_Gpio},			//上电电压监测
	{ PortC, Pin03, Func0_Gpio},			//电池电压监测
};
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
DWORD	ReceiveProgStatus;
WORD 	ProgFlashPW;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

static void InitWatchDog(void);

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 获取芯片类型
//
//参数:
//
//返回值:   0x55: H版   0x00：G版
//
//备注:关闭对应的定时器中断
//-----------------------------------------------
BYTE api_GetCpuVersion( void )
{
	return 0;
}

//-----------------------------------------------
//函数功能: 毫秒延时函数
//
//参数: 	ms[in]		需要延时的毫秒数
//                    
//返回值:  	无
//
//备注:    22M主频实测延时1.01ms 
//-----------------------------------------------
void api_Delayms(WORD ms)
{
	DDL_DelayMS(ms);
}

//-----------------------------------------------
//函数功能: 100微秒延时函数
//
//参数: 	us[in]		需要延时的100微秒数
//                    
//返回值:  	无
//
//备注:    22M主频时 实测约延时110us   循环值大于256和小于256延时值相差很大，从汇编看也不一样
//		 11M主频时 实测约延时100us
//-----------------------------------------------
void api_Delay100us(WORD us)
{
	BYTE i;

	for (i = 0; i < us; i++)
	{
		DDL_DelayUS(100);
	}	
}



//-----------------------------------------------
//函数功能: 10微秒延时函数
//
//参数: 	us[in]		需要延时的10微秒数
//                    
//返回值:  	无
//
//备注:    22M主频下实测约13.75us,us = 3,实测32us,us = 10，实测96us
//-----------------------------------------------
void api_Delay10us(WORD us)
{
	BYTE i;

	for (i = 0; i < us; i++)
	{
		DDL_DelayUS(10);
	}
}



//-----------------------------------------------
//函数功能: 通过NOP进行延时函数
//
//参数: 	Step[in]		需要延时的NOP个数
//                    
//返回值:  	无
//
//备注:   !!!!!!该函数另有一个函数指针调用方式，至少要保留一个地方采用函数名调用方式 !!!!!!
//-----------------------------------------------
#pragma optimize=size low//中级优化的情况下，会把a_byDefaultData优化掉
void api_DelayNop(BYTE Step)
{

}


//-----------------------------------------------
//函数功能: CPU系统时钟初始化函数
//
//参数: 	Type[in]		ePowerOnMode:正常上电  ePowerDownMode:低功耗
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_MCU_SysClockInit( ePOWER_MODE Type )
{
	stc_clock_xtal_init_t stcXtalInit;
    stc_clock_pll_init_t stcPLLHInit;

	CLK_XtalStructInit(&stcXtalInit);
	CLK_PLLStructInit(&stcPLLHInit);

	GPIO_AnalogCmd(BSP_XTAL_PORT, BSP_XTAL_IN_PIN | BSP_XTAL_OUT_PIN, ENABLE);
	/* Set bus clk div. */
	CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV2 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV8 |
                     CLK_PCLK3_DIV8 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |
                     CLK_HCLK_DIV1));
    /* Config Xtal and enable Xtal */
    stcXtalInit.u8Mode   = CLK_XTAL_MD_OSC;
    stcXtalInit.u8Drv    = CLK_XTAL_DRV_LOW;
    stcXtalInit.u8State  = CLK_XTAL_ON;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    (void)CLK_XtalInit(&stcXtalInit);

    (void)CLK_PLLStructInit(&stcPLLHInit);
    /* VCO = (8/1)*100 = 800MHz*/
    stcPLLHInit.u8PLLState      = CLK_PLL_ON;
    stcPLLHInit.PLLCFGR         = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM  = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN  = 64UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
    (void)CLK_PLLInit(&stcPLLHInit);

    /* Highspeed SRAM set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE2, SRAM_WAIT_CYCLE2);
    /* SRAM1_2_3_4_backup set to 2 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE2, SRAM_WAIT_CYCLE2);
    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE5);
    /* 3 cycles for 150 ~ 200 */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT3);
    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);
}

//-----------------------------------------------
//函数功能:     systick定时器初始化函数
//
//参数: 	 	Type[in]		ePowerOnMode:正常上电   ePowerDownMode:低功耗
//                    
//返回值:  	无
//
//备注:       ePowerDownMode模式 仅用于单相
//-----------------------------------------------
void api_InitSysTickTimer( ePOWER_MODE Type)
{
	SysTick_Init(100); // 配置为10ms滴答定时器
}


void InitTimer4( void )
{

}







//-----------------------------------------------
//函数功能: CPU初始化
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_InitCPU( void )
{
	api_Delay100us(10);
    
	//初始化系统时钟
	api_MCU_SysClockInit( ePowerOnMode );
	//初始化看门狗
	InitWatchDog();
	CLEAR_WATCH_DOG;
	//系统定时器初始化（Tick定时器）
//	api_InitSysTickTimer( ePowerOnMode );
	//初始化定时器4，用于查运行时间，时钟为32768
	// InitTimer4();
	api_InitAdc();
}


//-----------------------------------------------
//函数功能: 看门狗初始化，HT6025的看门狗默认是开启的
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void InitWatchDog(void)
{
	stc_wdt_init_t stcWdtInit;

	/* configure structure initialization */
	MEM_ZERO_STRUCT(stcWdtInit);

	// 按照PCLK3 24M计算
	// 1024*65536 / (24*1024*1024) = 2.66s
	stcWdtInit.u32ClockDiv = WDT_CLK_DIV1024;
	stcWdtInit.u32CountPeriod = WDT_CNT_PERIOD65536;
	stcWdtInit.u32RefreshRange = WDT_RANGE_0TO100PCT;
	stcWdtInit.u32LPMCount = WDT_LPM_CNT_STOP;
	stcWdtInit.u32ExceptionType = WDT_EXP_TYPE_RST;
	WDT_Init(&stcWdtInit);
}




//-----------------------------------------------
//函数功能: 关闭和恢复中断
//
//参数: 
//			Data[in]		0:关闭中断  其他（关闭前记录的数据）：恢复中断
//                    
//返回值:  	关闭前的中断状态
//
//备注:   
//-----------------------------------------------
BYTE api_splx(BYTE Data)
{
	BYTE SRValue;

	SRValue = __get_PRIMASK();

	if ((Data & 0x01) == 0x00)
	{
		DISABLE_CPU_INT;
	}
	else
	{
		ENABLE_CPU_INT;
	}

	return (~SRValue) & 0x01;
}

//-----------------------------------------------
//函数功能: 初始化SPI总线
//
//参数:
//			Component[in]	元器件eCOMPONENT_TYPE
//			Type[in]		SPI的模式  eSPI_MODE_0/eSPI_MODE_1/eSPI_MODE_2/eSPI_MODE_3分别对应SPI的模式0/1/2/3
//返回值:  	无
//
//备注: 只使用FLASH
//-----------------------------------------------

void api_InitSPI( eCOMPONENT_TYPE Component, eSPI_MODE Type )
{
	stc_spi_init_t stcSpiInit;
	stc_spi_delay_t stcSpiDelay;
	BYTE i;
	/* configuration structure initialization */
	SPI_StructInit(&stcSpiInit);
	SPI_DelayStructInit(&stcSpiDelay);
	/* Configuration peripheral clock */
	FCG_Fcg1PeriphClockCmd(SPI_FLASH_CLOCK, ENABLE);
	SPI_ClearStatus(SPI_FLASH_UNIT,SPI_FLAG_OVERLOAD); //清除错误标志，防止有错误时不能开启SPI
	SPI_ClearStatus(SPI_FLASH_UNIT,SPI_FLAG_MD_FAULT);
	SPI_ClearStatus(SPI_FLASH_UNIT,SPI_FLAG_PARITY_ERR);
	SPI_ClearStatus(SPI_FLASH_UNIT,SPI_FLAG_UNDERLOAD);
	/* Configuration SPI pin */
	for( i = 0; i < 3; i++)
	{
		GPIO_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x);
	}

	/* Configuration SPI structure */
	stcSpiInit.u32WireMode = SPI_3_WIRE;				// 三线制
	stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
	stcSpiInit.u32MasterSlave = SPI_MASTER;
	stcSpiInit.u32Parity = SPI_PARITY_INVD;
	stcSpiInit.u32SpiMode = SPI_MD_3;			//  CPOL=1  CPHA=1 Mode3	
	stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64; // PCLK1  96/64=1.5M
	stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
	stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
	stcSpiInit.u32FrameLevel = SPI_1_FRAME;

	stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_5SCK;

	SPI_Init(SPI_FLASH_UNIT, &stcSpiInit);
	SPI_DelayTimeConfig(SPI_FLASH_UNIT, &stcSpiDelay);
	SPI_Cmd(SPI_FLASH_UNIT, ENABLE);
}


//-----------------------------------------------
//函数功能: 非中断方式下写SPI的函数
//
//参数: 
//			Data[in]		写入的值
//                    
//返回值:  	读出的值
//
//备注:   
//-----------------------------------------------
BYTE api_UWriteSpi(eCOMPONENT_TYPE Component, BYTE Data)
{
	CM_SPI_TypeDef *pSpi;
	BYTE temp,retry;

	if( Component == eCOMPONENT_SPI_FLASH )
	{
		pSpi = SPI_FLASH_UNIT;
	}
	else
	{
//		ASSERT(FALSE, 0);
		return 0;
	}

	retry = 0;
	/* Wait tx buffer empty */
	while (RESET == SPI_GetStatus(pSpi, SPI_FLAG_TX_BUF_EMPTY))
	{
		retry++;
		if (retry>200)
		{
			return 0;
		}
	}
	/* Send data */
	SPI_WriteData(pSpi, Data);
	/* Wait rx buffer full */
	retry = 0;
	while (RESET == SPI_GetStatus(pSpi, SPI_FLAG_RX_BUF_FULL))
	{
		retry++;
		if (retry > 200)
		{
			return 0;
		}
	}
	/* Receive data */
	temp = SPI_ReadData(pSpi);

	return temp;
}
//-----------------------------------------------
//函数功能: 初始化ADC
//
//参数:
//			
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_InitAdc(void)
{
	stc_clock_xtal_init_t stcXtalCfg;
    stc_clock_pll_init_t stcUpllCfg;
    stc_adc_init_t stcAdcInit;
    uint8_t au8Adc1SaSampTime[3] = {0xff,0xff};
	BYTE i;
    stc_gpio_init_t stc_port = {0xFF, PIN_DIR_IN, DISABLE, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_ANALOG};
    
	/* Configuration peripheral clock */
	FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_ADC1, ENABLE);

	for( i = 0; i < sizeof(Adc_IO)/sizeof(MCU_IO); i++)
	{
		GPIO_Init(Adc_IO[i].Port,Adc_IO[i].Pin,&stc_port);
	}

    /* Configures XTAL. PLLH input source is XTAL. */
    /* XTAL is already configured in SystemClockConfig() */

    (void)CLK_PLLStructInit(&stcUpllCfg);
    /**
     * PLLHx(x=Q, R) = ((PLL_source / PLLM) * PLLN) / PLLx
     * PLLHQ = (8 / 1) * 80 /16 = 40MHz
     * PLLHR = (8 / 1) * 80 /16 = 40MHz
     */
    stcUpllCfg.u8PLLState = CLK_PLL_ON;
    stcUpllCfg.PLLCFGR = 0UL;
    stcUpllCfg.PLLCFGR_f.PLLM = (1UL  - 1UL);
    stcUpllCfg.PLLCFGR_f.PLLN = (80UL - 1UL);
    stcUpllCfg.PLLCFGR_f.PLLP = (4UL  - 1UL);
    stcUpllCfg.PLLCFGR_f.PLLQ = (16UL - 1UL);
    stcUpllCfg.PLLCFGR_f.PLLR = (16UL - 1UL);
    /* stcUpllCfg.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL; */
    (void)CLK_PLLInit(&stcUpllCfg);

	CLK_SetPeriClockSrc(CLK_PERIPHCLK_PCLK);

    MEM_ZERO_STRUCT(stcAdcInit);

    stcAdcInit.u16ScanMode   = ADC_MD_SEQA_SINGLESHOT;

    /* 2. Initialize ADC1. */
    ADC_Init(CM_ADC1, &stcAdcInit);

    /**************************** Add ADC1 channels ****************************/
	ADC_ChCmd(ADC_UNIT_1, ADC_SEQ_1, ADC_CH_1, ENABLE);
    ADC_SetSampleTime(ADC_UNIT_1, ADC_CH_1, au8Adc1SaSampTime[0]);
	ADC_ChCmd(ADC_UNIT_2, ADC_SEQ_2, ADC_CH_2, ENABLE);
    ADC_SetSampleTime(ADC_UNIT_2, ADC_CH_2, au8Adc1SaSampTime[1]);
}
//-----------------------------------------------
//函数功能: 获取AD数据
//
//参数:	wSystemVol[out]:系统掉电电压
//			
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_GetADData(WORD* wSystemVol)
{
	DWORD dwTimeCount,dwAdcTimeout;
	
	//获取AD值
	ADC_Start(ADC_UNIT_2);
	dwAdcTimeout = 1 * (SystemCoreClock / 10u / 1000u);
	dwTimeCount  = 0u;
	while (dwTimeCount < dwAdcTimeout)
	{
		if (ADC_GetStatus(ADC_UNIT_2, ADC_FLAG_EOCA) == SET) 
		{
            ADC_ClearStatus(ADC_UNIT_2, ADC_FLAG_EOCA);
			wSystemVol[0] = ADC_GetValue(ADC_UNIT_2,ADC_CH_2);
            break;
        }
		dwTimeCount++;
	}

	if(dwTimeCount == dwAdcTimeout)
	{
		wSystemVol[0] = 0;
	}
}
//-----------------------------------------------
//函数功能: 计算cpu Flash的累加和，采用word相加模式
//
//参数: 	无
//                    
//返回值:  	计算的word累加和
//
//备注:95ms   
//-----------------------------------------------
WORD api_CheckCpuFlashSum(BYTE Type)
{
    DWORD Sum = 0;
    
    return (WORD)Sum;
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
//备注:   
//-----------------------------------------------
BYTE api_ReadCpuRamAndInFlash(BYTE Type, DWORD Addr, BYTE Len, BYTE *Buf)
{	
	return Len;
}


//-----------------------------------------------
//函数功能: 升级程序flash
//
//参数: 	无     
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void UpdateProg( void )
{

}


#endif//#if ( CPU_TYPE == CPU_HC32F4A0 )
