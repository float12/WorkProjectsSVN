//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		CPU驱动程序 //这个文件要只留公共的部分，尽量不要用宏判断
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "ddl_config.h"
#include "hc32_ddl.h"
#include "cpuHC32F460.h"

#if ( CPU_TYPE == CPU_HC32F460 )

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
// Sample SPI Config

// Flash SPI Config
/* SPI unit and clock definition */
#define SPI_FLASH_UNIT					(M4_SPI4)
#define SPI_FLASH_CLOCK					(PWC_FCG1_PERIPH_SPI4)

/* SPI_SCK Port/Pin definition */
#define SPI_FLASH_SCK_PORT				(PortE)
#define SPI_FLASH_SCK_PIN				(Pin01)
#define SPI_FLASH_SCK_FUNC				(Func_Spi4_Sck)

/* SPI_MOSI Port/Pin definition */
#define SPI_FLASH_MOSI_PORT				(PortE)
#define SPI_FLASH_MOSI_PIN				(Pin00)
#define SPI_FLASH_MOSI_FUNC				(Func_Spi4_Mosi)

/* SPI_MISO Port/Pin definition */
#define SPI_FLASH_MISO_PORT 			(PortB)
#define SPI_FLASH_MISO_PIN 				(Pin09)
#define SPI_FLASH_MISO_FUNC				(Func_Spi4_Miso)

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// #define C_SysTickLoad   (((22020096>>SPD_MCU) / (1000/SYS_TICK_PERIOD_MS)) - 1)

// //6015的HRC是14M(6025的是11.01M) 所以低功耗唤醒的系统时钟为7M 不是5.5M SysTick的速度会加快
// #define C_SingleLowPowerSysTickLoad   (((22020096>>SINGLE_LOW_POWER_SPD_MCU) / (1000/SYS_TICK_PERIOD_MS)) - 1)

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

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
	Ddl_Delay1ms(ms);
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
		Ddl_Delay1us(100);
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
		Ddl_Delay1us(10);
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
	stc_clk_sysclk_cfg_t stcSysClkCfg;
	stc_clk_xtal_cfg_t stcXtalCfg;
	stc_clk_mpll_cfg_t stcMpllCfg;
	stc_sram_config_t stcSramConfig;

	MEM_ZERO_STRUCT(stcSysClkCfg);
	MEM_ZERO_STRUCT(stcXtalCfg);
	MEM_ZERO_STRUCT(stcMpllCfg);
	MEM_ZERO_STRUCT(stcSramConfig);

	/* Set bus clk div. */
	stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;    // 最高200M
	stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;   // 最高100M
	stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // 最高200M
	stcSysClkCfg.enPclk1Div = ClkSysclkDiv16; // 最高100M     192/16=12M
	stcSysClkCfg.enPclk2Div = ClkSysclkDiv4;  // 最高60M
	stcSysClkCfg.enPclk3Div = ClkSysclkDiv16; // 最高50M     192/16=12M
	stcSysClkCfg.enPclk4Div = ClkSysclkDiv2;  // 最高100M
	CLK_SysClkConfig(&stcSysClkCfg);

	/* Config Xtal and Enable Xtal */
	stcXtalCfg.enMode = ClkXtalModeOsc;
	stcXtalCfg.enDrv = ClkXtalLowDrv;
	stcXtalCfg.enFastStartup = Enable;
	CLK_XtalConfig(&stcXtalCfg);
	CLK_XtalCmd(Enable);

	/* sram init include read/write wait cycle setting */
	stcSramConfig.u8SramIdx = Sram12Idx | Sram3Idx | SramHsIdx | SramRetIdx;
	stcSramConfig.enSramRC = SramCycle2;
	stcSramConfig.enSramWC = SramCycle2;
	SRAM_Init(&stcSramConfig);// 2个CPU周期，禁止ECC

	/* flash read wait cycle setting */
	EFM_Unlock();
	EFM_SetLatency(EFM_LATENCY_5);
	EFM_Lock();

	/* MPLL config (XTAL / pllmDiv * plln / PllpDiv = 192M). */
	stcMpllCfg.pllmDiv = 1ul;
	stcMpllCfg.plln = 32ul;
	stcMpllCfg.PllpDiv = 2ul;
	stcMpllCfg.PllqDiv = 2ul;
	stcMpllCfg.PllrDiv = 2ul;
	CLK_SetPllSource(ClkPllSrcXTAL);
	CLK_MpllConfig(&stcMpllCfg);

	/* Enable MPLL. */
	CLK_MpllCmd(Enable);
	/* Wait MPLL ready. */
	while (Set != CLK_GetFlagStatus(ClkFlagMPLLRdy))
	{
		;
	}
	/* Switch driver ability */
	PWC_HS2HP();
	/* Switch system clock source to MPLL. */
	CLK_SetSysClkSource(CLKSysSrcMPLL);
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

	// 按照PCLK3  12M计算
	// 12*1024*1024 /2048*16384 = 2.66s
	stcWdtInit.enClkDiv = WdtPclk3Div2048;
	stcWdtInit.enCountCycle = WdtCountCycle16384;
	stcWdtInit.enRefreshRange = WdtRefresh100Pct;
	stcWdtInit.enSleepModeCountEn = Disable;
	stcWdtInit.enRequestType = WdtTriggerResetRequest;
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
//备注:
//-----------------------------------------------

void api_InitSPI( eCOMPONENT_TYPE Component, eSPI_MODE Type )
{
	stc_spi_init_t stcSpiInit;

	/* configuration structure initialization */
	MEM_ZERO_STRUCT(stcSpiInit);

	/* Configuration peripheral clock */
	PWC_Fcg1PeriphClockCmd(SPI_FLASH_CLOCK, Enable);
	SPI_ClearFlag(SPI_FLASH_UNIT, SpiFlagOverloadError); // 清除错误标志，防止有错误时不能开启SPI
	SPI_ClearFlag(SPI_FLASH_UNIT, SpiFlagModeFaultError);
	SPI_ClearFlag(SPI_FLASH_UNIT, SpiFlagParityError);
	SPI_ClearFlag(SPI_FLASH_UNIT, SpiFlagUnderloadError);

	/* Configuration SPI pin */
	PORT_SetFunc(SPI_FLASH_SCK_PORT, SPI_FLASH_SCK_PIN, SPI_FLASH_SCK_FUNC, Disable);
	PORT_SetFunc(SPI_FLASH_MOSI_PORT, SPI_FLASH_MOSI_PIN, SPI_FLASH_MOSI_FUNC, Disable);
	PORT_SetFunc(SPI_FLASH_MISO_PORT, SPI_FLASH_MISO_PIN, SPI_FLASH_MISO_FUNC, Disable);

	/* Configuration SPI structure */
	stcSpiInit.enClkDiv = SpiClkDiv8; // PCLK1  12/8
	stcSpiInit.enFrameNumber = SpiFrameNumber1;
	stcSpiInit.enDataLength = SpiDataLengthBit8;
	stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
	stcSpiInit.enSckPolarity = SpiSckIdleLevelHigh;			 //  CPOL=1   Mode3
	stcSpiInit.enSckPhase = SpiSckOddChangeEvenSample;		 //  CPHA=1
	stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;
	stcSpiInit.enWorkMode = SpiWorkMode3Line;				// 三线制
	stcSpiInit.enTransMode = SpiTransFullDuplex;
	stcSpiInit.enCommAutoSuspendEn = Disable;
	stcSpiInit.enModeFaultErrorDetectEn = Disable;
	stcSpiInit.enParitySelfDetectEn = Disable;
	stcSpiInit.enParityEn = Disable;
	stcSpiInit.enParity = SpiParityEven;
	stcSpiInit.enMasterSlaveMode = SpiModeMaster;
	// 三线制 以下无效
	stcSpiInit.stcDelayConfig.enSsSetupDelayOption = SpiSsSetupDelayCustomValue;
	stcSpiInit.stcDelayConfig.enSsSetupDelayTime = SpiSsSetupDelaySck1;
	stcSpiInit.stcDelayConfig.enSsHoldDelayOption = SpiSsHoldDelayCustomValue;
	stcSpiInit.stcDelayConfig.enSsHoldDelayTime = SpiSsHoldDelaySck1;
	stcSpiInit.stcDelayConfig.enSsIntervalTimeOption = SpiSsIntervalCustomValue;
	stcSpiInit.stcDelayConfig.enSsIntervalTime = SpiSsIntervalSck6PlusPck2;

	SPI_Init(SPI_FLASH_UNIT, &stcSpiInit);
	SPI_Cmd(SPI_FLASH_UNIT, Enable);
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
	M4_SPI_TypeDef *pSpi;
	BYTE temp,retry;

	if( Component == eCOMPONENT_SPI_FLASH )
	{
		pSpi = SPI_FLASH_UNIT;
	}
	else if( Component == eCOMPONENT_SPI_SAMPLE )
	{
		pSpi = M4_SPI1;
	}
	else
	{
//		ASSERT(FALSE, 0);
		return 0;
	}

	retry = 0;
	/* Wait tx buffer empty */
	while (Reset == SPI_GetFlag(pSpi, SpiFlagSendBufferEmpty))
	{
		retry++;
		if (retry>200)
		{
			return 0;
		}
	}
	/* Send data */
	SPI_SendData8(pSpi, Data);
	/* Wait rx buffer full */
	retry = 0;
	while (Reset == SPI_GetFlag(pSpi, SpiFlagReceiveBufferFull))
	{
		retry++;
		if (retry > 200)
		{
			return 0;
		}
	}
	/* Receive data */
	temp = SPI_ReceiveData8(pSpi);

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
	stc_clk_xtal_cfg_t stcXtalCfg;
	stc_clk_upll_cfg_t stcUpllCfg;
	stc_adc_init_t stcAdcInit;
	stc_adc_ch_cfg_t stcChCfg;
	uint8_t au8Adc1SaSampTime[3] = {0xff, 0xff};
	BYTE i;
	stc_port_init_t stc_port = {Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Enable};

	/* Configuration peripheral clock */
	PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_ADC1, Enable);

	PORT_Init(PortC, Pin00, &stc_port);
	PORT_Init(PortB, Pin01, &stc_port);

	MEM_ZERO_STRUCT(stcXtalCfg);

	/* Use XTAL as UPLL source. */
	stcXtalCfg.enFastStartup = Enable;
	stcXtalCfg.enMode = ClkXtalModeOsc;
	stcXtalCfg.enDrv = ClkXtalLowDrv;
	CLK_XtalConfig(&stcXtalCfg);
	CLK_XtalCmd(Enable);

	MEM_ZERO_STRUCT(stcUpllCfg);

	/* Set UPLL out 240MHz. */
	stcUpllCfg.pllmDiv = 2u;
	/* upll = 12M(XTAL) / pllmDiv * plln */
	stcUpllCfg.plln = 40u;
	stcUpllCfg.PllpDiv = 6u;
	stcUpllCfg.PllqDiv = 6u;
	stcUpllCfg.PllrDiv = 6u;
	CLK_SetPllSource(ClkPllSrcXTAL);
	CLK_UpllConfig(&stcUpllCfg);
	CLK_UpllCmd(Enable);
	CLK_SetPeriClkSource(ClkPeriSrcUpllr); // 选用UPLLR，输出时钟频率40M

	MEM_ZERO_STRUCT(stcAdcInit);

	stcAdcInit.enResolution = AdcResolution_12Bit;
	stcAdcInit.enDataAlign = AdcDataAlign_Right;
	stcAdcInit.enAutoClear = AdcClren_Enable;
	stcAdcInit.enScanMode = AdcMode_SAOnce;
	stcAdcInit.enRschsel = AdcRschsel_Restart;

	/* 2. Initialize ADC1. */
	ADC_Init(M4_ADC1, &stcAdcInit);

	MEM_ZERO_STRUCT(stcChCfg);

	/**************************** Add ADC1 channels ****************************/
	stcChCfg.u32Channel = 0x00000600; // 第9,10通道
	stcChCfg.u8Sequence = ADC_SEQ_A;
	stcChCfg.pu8SampTime = au8Adc1SaSampTime; // 采样按照255个周期
	/* 2. Add ADC channel. */
	ADC_AddAdcChannel(M4_ADC1, &stcChCfg);
}
//-----------------------------------------------
//函数功能: 获取AD数据
//
//参数:	Type:AD类型
//			
//返回值:  	无
//
//备注:
//-----------------------------------------------
WORD api_GetADData(eType_ADC Type)
{
	WORD wData[3] = {0};
	if (Type >= eOther_AD_Max)
	{
		return 0;
	}
	// 获取AD值
	ADC_PollingSa(M4_ADC1, &wData[0], 3, 1);
	return wData[(BYTE)Type];
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


#endif//#if ( CPU_TYPE == CPU_HC32F460 )
