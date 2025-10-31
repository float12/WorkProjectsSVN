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
#include "hc32_ll_dma.h"
#include "bsp_cpu_flash.h"

#if (CPU_TYPE == CPU_HC32F4A0)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//ADC Config
//时钟电池ADC
#define ADC_UNIT_1 (CM_ADC1)
#define ADC_SEQ_1 (ADC_SEQ_A)
#define ADC_CH_1 (ADC_CH11)
//掉电监测ADC
#define ADC_UNIT_2 (CM_ADC1)
#define ADC_SEQ_2 (ADC_SEQ_A)
#define ADC_CH_2 (ADC_CH13)

// Sample SPI Config
/* SPI unit and clock definition */
#define SPI_SAMPLE_UNIT (CM_SPI1)
#define SPI_SAMPLE_CLOCK (FCG1_PERIPH_SPI1)

#define SPI_FLASH_UNIT (CM_SPI1)
#define SPI_FLASH_CLOCK (FCG1_PERIPH_SPI1)

#define SPI_SAMPLE1_TOPO_UNIT (CM_SPI2)
#define SPI_SAMPLE1_TOPO_CLOCK (FCG1_PERIPH_SPI2)

#define SPI_SAMPLE2_TOPO_UNIT (CM_SPI3)
#define SPI_SAMPLE2_TOPO_CLOCK (FCG1_PERIPH_SPI3)

#define SPI_SAMPLE3_TOPO_UNIT (CM_SPI4)
#define SPI_SAMPLE3_TOPO_CLOCK (FCG1_PERIPH_SPI4)

#define SPI_SAMPLE4_TOPO_UNIT (CM_SPI6)
#define SPI_SAMPLE4_TOPO_CLOCK (FCG1_PERIPH_SPI6)

// ESAM SPI Config
/* SPI unit and clock definition */
#define SPI_ESAM_UNIT (CM_SPI1)
#define SPI_ESAM_CLOCK (FCG1_PERIPH_SPI1)

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//SPI 功能管脚对应关系,和ecomponent顺序一致
const MCU_IO Spi_IO[][3] =
	{
		{
			{PortB, Pin00, Func40_Spi1_Sck},  //CLK
			{PortB, Pin01, Func41_Spi1_Mosi}, //Mosi
			{PortB, Pin02, Func42_Spi1_Miso}, //Miso  计量
		},
		{
			{PortC, Pin04, Func43_Spi2_Sck},  //CLK
			{PortC, Pin05, Func44_Spi2_Mosi}, //Mosi hsdc1
			{PortB, Pin12, Func19_Spi2_Nss0_2},
		},
		{
			{PortB, Pin04, Func46_Spi3_Sck},  //CLK
			{PortB, Pin05, Func47_Spi3_Mosi}, //Mosi hsdc2
			{PortB, Pin15, Func49_Spi3_Nss0}, //cs  必须选择为对应spi的nss0
		},
		{
			{PortC, Pin06, Func43_Spi4_Sck},  //CLK
			{PortC, Pin07, Func44_Spi4_Mosi}, //Mosi hsdc3
			{PortD, Pin00, Func46_Spi4_Nss0}, //cs  必须选择为对应spi的nss0
		},
		{
			{PortA, Pin08, Func46_Spi6_Sck},  //CLK
			{PortA, Pin09, Func47_Spi6_Mosi}, //Mosi hsdc4
			{PortA, Pin11, Func49_Spi6_Nss0}, //cs  必须选择为对应spi的nss0
		},
		{
			{PortB, Pin00, Func40_Spi1_Sck},  //CLK
			{PortB, Pin01, Func41_Spi1_Mosi}, //Mosi
			{PortB, Pin02, Func42_Spi1_Miso}, //Miso  ESAM
		},
		{
			{PortB, Pin00, Func40_Spi1_Sck},  //CLK
			{PortB, Pin01, Func41_Spi1_Mosi}, //Mosi
			{PortB, Pin02, Func42_Spi1_Miso}, //Miso  FLASH
		},

};

//ADC 功能管脚对应关系
const MCU_IO Adc_IO[]=
{
		{PortC, Pin01, Func0_Gpio}, //上电电压监测
		// {PortC, Pin03, Func0_Gpio}, //电池电压监测
};
//const Fun FunctionConst = api_DelayNop;
BYTE ReceiveProgStatus[8]; // 128字节一帧，一个扇区8K，需要8*8个bit
WORD ProgFlashPW;

BYTE SpiTxBuffer[6] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
BYTE bSPIExIntFlag = 0;

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void InitWatchDog(void);
static void InitADC(void);
static void SPI4_IRQ_Send(void);
static void SPI4_IRQ_Rcv(void);
static void TIMEA_1_PWM8_Init(void);
static void ExtInt11_IRQHandler(void);
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
BYTE api_GetCpuVersion(void)
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
//备注:    48M主频时实测约延时1.01ms；72M主频时实测约延时1ms；96M主频时实测约延时1ms；
//		注意：延时越长，延时时间会有增加;仿真测试与不仿真测试延时有区别 !!!!!!
//		ms=500时：48M主频下实测约591ms;72M主频下实测约557ms;96M主频下实测约542ms;
//-----------------------------------------------
void api_Delayms(WORD ms)
{
	DWORD TargetValue, CurrentValue;
	DWORD LastValue, TempValue;
	DWORD CpuClockMHz;
	DWORD SysTickLoad;

	CpuClockMHz = SystemCoreClock / 1000; //基于滴答是10ms中断
	TargetValue = ms * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;

	do
	{
		TempValue = SysTick->VAL;
		if (LastValue >= TempValue) //Systick是向下计数的
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick的装载值是减1的
		}
		LastValue = TempValue;
	} while (CurrentValue < TargetValue);
}

//-----------------------------------------------
//函数功能: 100微秒延时函数
//
//参数: 	us[in]		需要延时的100微秒数
//
//返回值:  	无
//
//备注:    48M主频时实测约延时103us；72M主频时实测约延时101.75us；96M主频时实测约延时101.3us；
//-----------------------------------------------
void api_Delay100us(WORD us)
{
	DWORD TargetValue, CurrentValue;
	DWORD LastValue, TempValue;
	DWORD CpuClockMHz;
	DWORD SysTickLoad;

	CpuClockMHz = SystemCoreClock / 10000; //基于滴答是10ms中断
	TargetValue = us * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;

	do
	{
		TempValue = SysTick->VAL;
		if (LastValue >= TempValue) //Systick是向下计数的
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick的装载值是减1的
		}
		LastValue = TempValue;
	} while (CurrentValue < TargetValue);
}

//-----------------------------------------------
//函数功能: 10微秒延时函数
//
//参数: 	us[in]		需要延时的10微秒数
//
//返回值:  	无
//
//备注:    48M主频下实测约12us;72M主频时实测约延时11us；96M主频时实测约延时10.5us；
//		注意：延时越长，延时时间会有缩减
//		us=5时：48M主频下实测约49us;72M主频下实测约48us;96M主频下实测约47us;
//-----------------------------------------------
void api_Delay10us(WORD us)
{
	DWORD TargetValue, CurrentValue;
	DWORD LastValue, TempValue;
	DWORD CpuClockMHz;
	DWORD SysTickLoad;

	CpuClockMHz = SystemCoreClock / 100000; //基于滴答是10ms中断
	TargetValue = us * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;

	do
	{
		TempValue = SysTick->VAL;
		if (LastValue >= TempValue) //Systick是向下计数的
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick的装载值是减1的
		}
		LastValue = TempValue;
	} while (CurrentValue < TargetValue);
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
#pragma optimize = size low //中级优化的情况下，会把a_byDefaultData优化掉
//固定位置方便短报文升级
void api_DelayNop(BYTE Step) @"FUN_CONST"
{
	BYTE i;

	i = a_byDefaultData[0];

	for (i = 0; i < Step; i++)
	{
		__NOP();
	}
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
void api_MCU_SysClockInit(ePOWER_MODE Type)
{
	stc_clock_xtal_init_t stcXtalInit;
    stc_clock_pll_init_t stcPLLHInit;

	CLK_XtalStructInit(&stcXtalInit);
	CLK_PLLStructInit(&stcPLLHInit);

	/* Set bus clk div. */
	#if(SPD_HDSC_MCU == SPD_48000K)
	CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV4 | CLK_PCLK2_DIV1 |
                     CLK_PCLK3_DIV4 | CLK_PCLK4_DIV1 | CLK_EXCLK_DIV2 |
                     CLK_HCLK_DIV1));
	#elif(SPD_HDSC_MCU == SPD_72000K)
	CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV4 | CLK_PCLK2_DIV1 |
                     CLK_PCLK3_DIV4 | CLK_PCLK4_DIV1 | CLK_EXCLK_DIV2 |
                     CLK_HCLK_DIV1));
	#elif(SPD_HDSC_MCU == SPD_96000K)
	CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV2 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV8 |
                     CLK_PCLK3_DIV8 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |
                     CLK_HCLK_DIV1));
	#elif(SPD_HDSC_MCU == SPD_192000K)
	CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV2 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV8 |
                     CLK_PCLK3_DIV8 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |
                     CLK_HCLK_DIV1));
	#endif

	GPIO_AnalogCmd(BSP_XTAL_PORT, BSP_XTAL_IN_PIN | BSP_XTAL_OUT_PIN, ENABLE);

	(void)CLK_XtalStructInit(&stcXtalInit);
    /* Config Xtal and enable Xtal */
    stcXtalInit.u8Mode   = CLK_XTAL_MD_OSC;
    stcXtalInit.u8Drv    = CLK_XTAL_DRV_LOW;
    stcXtalInit.u8State  = CLK_XTAL_ON;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    (void)CLK_XtalInit(&stcXtalInit);

    (void)CLK_PLLStructInit(&stcPLLHInit);
    /* 目前192MHZ*/
    stcPLLHInit.u8PLLState      = CLK_PLL_ON;
    stcPLLHInit.PLLCFGR         = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM  = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN  = 64UL - 1UL;//12*64 = 768MHZ
    stcPLLHInit.PLLCFGR_f.PLLP  = 4UL - 1UL;//768  /4 = 192MHZ
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
void api_InitSysTickTimer(ePOWER_MODE Type)
{
	SysTick_Init(100); // 配置为10ms滴答定时器
}

void InitTimer4(void)
{
}

//-----------------------------------------------
//函数功能: 获取波特率参数
//
//参数: 	Type[in]		0:初始化时用波特率 1：平时读波特率
//          SciPhNum[in]	物理串口号
//
//返回值:  	波特率参数值
//
//备注:
//-----------------------------------------------
BYTE api_GetBaudRate(BYTE Type, BYTE SciPhNum)
{
	BYTE i;
	BYTE BpsBak, Status;
	Status = FALSE;

	if (Type == 0)
	{
		//判断FPara2的校验
		if (lib_CheckSafeMemVerify((BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC) == TRUE)
		{
			Status = TRUE;
		}
	}
	else
	{
		Status = TRUE;
	}

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SerialMap[i].SCI_Ph_Num == SciPhNum)
		{
			if (SerialMap[i].SerialType == eRS485_I) //485
			{
				if (Status == TRUE)
				{
					BpsBak = FPara2->CommPara.I485;
				}
				else
				{
					BpsBak = CommParaConst.I485; //9600 1停止位 偶校验
				}
			}
			else if (SerialMap[i].SerialType == eIR) //红外
			{
				BpsBak = 0x42; //红外口固定1200
			}
            else if (SerialMap[i].SerialType == e4gWaveData) //给4g发送波形数据
			{
                if (Status == TRUE)
				{
					BpsBak = FPara2->CommPara.WaveData4g;
				}
				else
				{
					BpsBak = CommParaConst.WaveData4g; 
				}
			}
			else if (SerialMap[i].SerialType == eCR)
			{
				if (Status == TRUE)
				{
					BpsBak = FPara2->CommPara.ComModule;
				}
				else
				{
					BpsBak = CommParaConst.ComModule; //9600bps
				}
			}
			//            else if(SerialMap[i].SerialType == eRS485_II)
			//            {
			//            	if( Status == TRUE )
			//                {
			//                   	BpsBak = FPara2->CommPara.II485;
			//                }
			//                else
			//                {
			//                  	BpsBak = CommParaConst.II485;//2400bps
			//                }
			//            }
			//			else if(SerialMap[i].SerialType == eBlueTooth)
			//            {
			//            	BpsBak = 0x4A;
			//            }
			else if (SerialMap[i].SerialType == eCAN)
			{
				if (Status == TRUE)
				{
					BpsBak = FPara2->CommPara.CanBaud;
				}
				else
				{
					BpsBak = CommParaConst.CanBaud; //默认125KBPS
				}
			}
			break;
		}
	}

	return BpsBak;
}

//-----------------------------------------------
//函数功能: 处理更改波特率后的串口初始化
//
//参数: 	Channel[in]		逻辑串口号 0：第一485  1：红外  2：模块
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void DealSciFlag(BYTE Channel)
{
	//有改波特操作，电表应答完再改波特率
	if (Serial[Channel].OperationFlag & BIT0)
	{
		Serial[Channel].OperationFlag &= ~BIT0;
		SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
	}
}

//-----------------------------------------------
//函数功能: 串口初始化
//
//参数:
//			Channel[in]		逻辑串口号 0：第一485  1：红外  2：模块
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_InitSci(BYTE Channel)
{
	SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
}

//-----------------------------------------------
//函数功能: 禁止指定串口接收
//
//参数:
//			p[in]		需要禁止串口的指针
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_DisableRece(TSerial *p)
{
	BYTE i;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (p == &Serial[i])
		{
			SerialMap[i].SCIDisableRcv(SerialMap[i].SCI_Ph_Num);
		}
	}
}

//-----------------------------------------------
//函数功能: 允许指定接口接收
//
//参数:
//			p[in]		需要允许串口的指针
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_EnableRece(TSerial *p)
{
	BYTE i;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (p == &Serial[i])
		{
			SerialMap[i].SCIEnableRcv(SerialMap[i].SCI_Ph_Num);
		}
	}
}

//-----------------------------------------------
//函数功能: 广播命令无应答的情况下串口恢复接收
//
//参数:
//			Channel[in]		逻辑串口号 0：第一485  1：红外  2：模块
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_ProSciTimer(BYTE Channel)
{
	BYTE IntStatus;

	if (Serial[Channel].BroadCastFlag == 0xff)
	{
		DealSciFlag(Channel);
		SerialMap[Channel].SCIEnableRcv(SerialMap[Channel].SCI_Ph_Num);

		IntStatus = api_splx(0);
		api_InitSciStatus(Channel);
		api_splx(IntStatus);
	}
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
void api_InitCPU(void)
{
	//api_Delay100us(10);
	//打开外设写使能
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);
	
	FunctionConst(INIT_BASE_TIMER);
	//初始化系统时钟
	api_MCU_SysClockInit(ePowerOnMode);
	//初始化看门狗
	InitWatchDog();
	CLEAR_WATCH_DOG;
	//系统定时器初始化（Tick定时器）
	api_InitSysTickTimer(ePowerOnMode);
	//ADC初始化
	InitADC();
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
//函数功能: RTC的中断使能
//
//参数: 	Type[in]		0:秒 分 时中断开启  1:秒中断开启
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_MCU_RTCInt(void)
{
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
void api_InitSPI(eCOMPONENT_TYPE Component, eSPI_MODE Type)
{
	stc_spi_init_t stcSpiInit;
	stc_spi_delay_t stcSpiDelay;
	BYTE i;

    LL_PERIPH_WE(LL_PERIPH_GPIO);
	if (Component == eCOMPONENT_SPI_SAMPLE)
	{
		/* configuration structure initialization */
		SPI_StructInit(&stcSpiInit);
		SPI_DelayStructInit(&stcSpiDelay);
		/* Configuration peripheral clock */
		FCG_Fcg1PeriphClockCmd(SPI_SAMPLE_CLOCK, ENABLE);
		SPI_ClearStatus(SPI_SAMPLE_UNIT, SPI_FLAG_OVERLOAD); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearStatus(SPI_SAMPLE_UNIT, SPI_FLAG_MD_FAULT);
		SPI_ClearStatus(SPI_SAMPLE_UNIT, SPI_FLAG_PARITY_ERR);
		SPI_ClearStatus(SPI_SAMPLE_UNIT, SPI_FLAG_UNDERLOAD);
		/* Configuration SPI pin */
		for (i = 0; i < 3; i++)
		{
			if(Spi_IO[(BYTE)Component][i].Port != 0)
			{
				GPIO_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x);
			}
		}

		/* Configuration SPI structure */
		stcSpiInit.u32WireMode = SPI_3_WIRE; // 三线制
		stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
		stcSpiInit.u32MasterSlave = SPI_MASTER;
		stcSpiInit.u32Parity = SPI_PARITY_INVD;
		stcSpiInit.u32SpiMode = SPI_MD_1; //
#if ((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV128; // 
#elif (SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; // 
#elif (SPD_HDSC_MCU == SPD_192000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64; // PCLK1  96/64=1.5M
#endif
		stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
		stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
		stcSpiInit.u32FrameLevel = SPI_1_FRAME;

		stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_5SCK;
		SPI_Init(SPI_SAMPLE_UNIT, &stcSpiInit);
		SPI_DelayTimeConfig(SPI_SAMPLE_UNIT, &stcSpiDelay);
		SPI_Cmd(SPI_SAMPLE_UNIT, ENABLE);
	}
	else if (Component == eCOMPONENT_SPI_FLASH)
	{
		/* configuration structure initialization */
		SPI_StructInit(&stcSpiInit);
		SPI_DelayStructInit(&stcSpiDelay);
		/* Configuration peripheral clock */
		FCG_Fcg1PeriphClockCmd(SPI_FLASH_CLOCK, ENABLE);
		SPI_ClearStatus(SPI_FLASH_UNIT, SPI_FLAG_OVERLOAD); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearStatus(SPI_FLASH_UNIT, SPI_FLAG_MD_FAULT);
		SPI_ClearStatus(SPI_FLASH_UNIT, SPI_FLAG_PARITY_ERR);
		SPI_ClearStatus(SPI_FLASH_UNIT, SPI_FLAG_UNDERLOAD);
		/* Configuration SPI pin */
		for (i = 0; i < 3; i++)
		{
			if(Spi_IO[(BYTE)Component][i].Port != 0)
			{
				GPIO_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x);
			}
		}

		/* Configuration SPI structure */
		stcSpiInit.u32WireMode = SPI_3_WIRE; // 三线制
		stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
		stcSpiInit.u32MasterSlave = SPI_MASTER;
		stcSpiInit.u32Parity = SPI_PARITY_INVD;
		stcSpiInit.u32SpiMode = SPI_MD_3; //
		#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV32; // PCLK1  
		#elif(SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; // PCLK1  
		#elif (SPD_HDSC_MCU == SPD_192000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64; // PCLK1  96/64=1.5M
		#endif
		stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
		stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
		stcSpiInit.u32FrameLevel = SPI_1_FRAME;

		stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_5SCK;
		SPI_Init(SPI_FLASH_UNIT, &stcSpiInit);
		SPI_DelayTimeConfig(SPI_FLASH_UNIT, &stcSpiDelay);
		SPI_Cmd(SPI_FLASH_UNIT, ENABLE);
	}
	else if (Component == eCOMPONENT_SPI_HSDC1)
	{
		/* configuration structure initialization */
		SPI_StructInit(&stcSpiInit);
		SPI_DelayStructInit(&stcSpiDelay);
		/* Configuration peripheral clock */
		FCG_Fcg1PeriphClockCmd(SPI_SAMPLE1_TOPO_CLOCK, ENABLE);
		SPI_ClearStatus(SPI_SAMPLE1_TOPO_UNIT, SPI_FLAG_OVERLOAD); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearStatus(SPI_SAMPLE1_TOPO_UNIT, SPI_FLAG_MD_FAULT);
		SPI_ClearStatus(SPI_SAMPLE1_TOPO_UNIT, SPI_FLAG_PARITY_ERR);
		SPI_ClearStatus(SPI_SAMPLE1_TOPO_UNIT, SPI_FLAG_UNDERLOAD);
		/* Configuration SPI pin */
		for (i = 0; i < 3; i++)
		{
			if(Spi_IO[(BYTE)Component][i].Port != 0)
			{
				GPIO_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x);
			}
		}

		/* Configuration SPI structure */
		stcSpiInit.u32WireMode = SPI_4_WIRE; // 三线制
		stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
		stcSpiInit.u32MasterSlave = SPI_SLAVE;
		stcSpiInit.u32Parity = SPI_PARITY_INVD;
		stcSpiInit.u32SpiMode = SPI_MD_1; //
#if ((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; // 
#elif (SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; // 
#elif (SPD_HDSC_MCU == SPD_192000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64; // PCLK1  96/64=1.5M
#endif
		stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
		stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
		stcSpiInit.u32FrameLevel = SPI_1_FRAME;

		stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_5SCK;

		SPI_Init(SPI_SAMPLE1_TOPO_UNIT, &stcSpiInit);
		SPI_DelayTimeConfig(SPI_SAMPLE1_TOPO_UNIT, &stcSpiDelay);
		SPI_Cmd(SPI_SAMPLE1_TOPO_UNIT, ENABLE);
	}
	else if (Component == eCOMPONENT_SPI_HSDC2)
	{
      
		/* configuration structure initialization */
		SPI_StructInit(&stcSpiInit);
//		SPI_DelayStructInit(&stcSpiDelay);
		/* Configuration peripheral clock */
		FCG_Fcg1PeriphClockCmd(SPI_SAMPLE2_TOPO_CLOCK, ENABLE);
		SPI_ClearStatus(SPI_SAMPLE2_TOPO_UNIT, SPI_FLAG_OVERLOAD); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearStatus(SPI_SAMPLE2_TOPO_UNIT, SPI_FLAG_MD_FAULT);
		SPI_ClearStatus(SPI_SAMPLE2_TOPO_UNIT, SPI_FLAG_PARITY_ERR);
		SPI_ClearStatus(SPI_SAMPLE2_TOPO_UNIT, SPI_FLAG_UNDERLOAD);
		/* Configuration SPI pin */
		for (i = 0; i < 3; i++)
		{
			if(Spi_IO[(BYTE)Component][i].Port != 0)
			{
				GPIO_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x);
			}
		}

		/* Configuration SPI structure */
		stcSpiInit.u32WireMode = SPI_4_WIRE; // 四线制
		stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
		stcSpiInit.u32MasterSlave = SPI_SLAVE;
		stcSpiInit.u32Parity = SPI_PARITY_INVD;
		stcSpiInit.u32SpiMode = SPI_MD_1; //
#if ((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; //
#elif (SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; // 
#elif (SPD_HDSC_MCU == SPD_192000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64; // PCLK1  96/64=1.5M
#endif
		stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
		stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
		stcSpiInit.u32FrameLevel = SPI_1_FRAME;
		// stcSpiInit.u32ModeFaultDetect = SPI_MD_FAULT_DETECT_DISABLE;  // 使能模式故障检测
		// stcSpiInit.u32SuspendMode = SPI_COM_SUSP_FUNC_OFF;

		// stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_5SCK;

		SPI_Init(SPI_SAMPLE2_TOPO_UNIT, &stcSpiInit);
		// SPI_DelayTimeConfig(SPI_SAMPLE2_TOPO_UNIT, &stcSpiDelay);
		SPI_Cmd(SPI_SAMPLE2_TOPO_UNIT, ENABLE);
	}
	else if (Component == eCOMPONENT_SPI_HSDC3)
	{
      
		/* configuration structure initialization */
		SPI_StructInit(&stcSpiInit);
//		SPI_DelayStructInit(&stcSpiDelay);
		/* Configuration peripheral clock */
		FCG_Fcg1PeriphClockCmd(SPI_SAMPLE3_TOPO_CLOCK, ENABLE);
		SPI_ClearStatus(SPI_SAMPLE3_TOPO_UNIT, SPI_FLAG_OVERLOAD); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearStatus(SPI_SAMPLE3_TOPO_UNIT, SPI_FLAG_MD_FAULT);
		SPI_ClearStatus(SPI_SAMPLE3_TOPO_UNIT, SPI_FLAG_PARITY_ERR);
		SPI_ClearStatus(SPI_SAMPLE3_TOPO_UNIT, SPI_FLAG_UNDERLOAD);
		/* Configuration SPI pin */
		for (i = 0; i < 3; i++)
		{
			if(Spi_IO[(BYTE)Component][i].Port != 0)
			{
				GPIO_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x);
			}
		}

		/* Configuration SPI structure */
		stcSpiInit.u32WireMode = SPI_4_WIRE; // 四线制
		stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
		stcSpiInit.u32MasterSlave = SPI_SLAVE;
		stcSpiInit.u32Parity = SPI_PARITY_INVD;
		stcSpiInit.u32SpiMode = SPI_MD_1; //
#if ((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; // 
#elif (SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; // 
#elif (SPD_HDSC_MCU == SPD_192000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64; // PCLK1  96/64=1.5M
#endif
		stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
		stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
		stcSpiInit.u32FrameLevel = SPI_1_FRAME;
		// stcSpiInit.u32ModeFaultDetect = SPI_MD_FAULT_DETECT_DISABLE;  // 使能模式故障检测
		// stcSpiInit.u32SuspendMode = SPI_COM_SUSP_FUNC_OFF;

		// stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_5SCK;

		SPI_Init(SPI_SAMPLE3_TOPO_UNIT, &stcSpiInit);
		// SPI_DelayTimeConfig(SPI_SAMPLE2_TOPO_UNIT, &stcSpiDelay);
		SPI_Cmd(SPI_SAMPLE3_TOPO_UNIT, ENABLE);
	}
	else if (Component == eCOMPONENT_SPI_HSDC4)
	{
      
		/* configuration structure initialization */
		SPI_StructInit(&stcSpiInit);
//		SPI_DelayStructInit(&stcSpiDelay);
		/* Configuration peripheral clock */
		FCG_Fcg1PeriphClockCmd(SPI_SAMPLE4_TOPO_CLOCK, ENABLE);
		SPI_ClearStatus(SPI_SAMPLE4_TOPO_UNIT, SPI_FLAG_OVERLOAD); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearStatus(SPI_SAMPLE4_TOPO_UNIT, SPI_FLAG_MD_FAULT);
		SPI_ClearStatus(SPI_SAMPLE4_TOPO_UNIT, SPI_FLAG_PARITY_ERR);
		SPI_ClearStatus(SPI_SAMPLE4_TOPO_UNIT, SPI_FLAG_UNDERLOAD);
		/* Configuration SPI pin */
		for (i = 0; i < 3; i++)
		{
			if(Spi_IO[(BYTE)Component][i].Port != 0)
			{
				GPIO_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x);
			}
		}

		/* Configuration SPI structure */
		stcSpiInit.u32WireMode = SPI_4_WIRE; // 四线制
		stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
		stcSpiInit.u32MasterSlave = SPI_SLAVE;
		stcSpiInit.u32Parity = SPI_PARITY_INVD;
		stcSpiInit.u32SpiMode = SPI_MD_1; //
#if ((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; //
#elif (SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV16; //
#elif (SPD_HDSC_MCU == SPD_192000K)
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV64; // PCLK1  96/64=1.5M
#endif
		stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
		stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
		stcSpiInit.u32FrameLevel = SPI_1_FRAME;
		// stcSpiInit.u32ModeFaultDetect = SPI_MD_FAULT_DETECT_DISABLE;  // 使能模式故障检测
		// stcSpiInit.u32SuspendMode = SPI_COM_SUSP_FUNC_OFF;

		// stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_5SCK;

		SPI_Init(SPI_SAMPLE4_TOPO_UNIT, &stcSpiInit);
		// SPI_DelayTimeConfig(SPI_SAMPLE2_TOPO_UNIT, &stcSpiDelay);
		SPI_Cmd(SPI_SAMPLE4_TOPO_UNIT, ENABLE);
	}
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
	BYTE temp, retry;

	if (Component == eCOMPONENT_SPI_FLASH)
	{
		pSpi = SPI_FLASH_UNIT;
	}
	else if (Component == eCOMPONENT_SPI_SAMPLE)
	{
		pSpi = SPI_SAMPLE_UNIT;
	}
	else if (Component == eCOMPONENT_SPI_ESAM)
	{
		pSpi = SPI_ESAM_UNIT;
	}
	else //8306吐数据那一路SPI没有写功能
	{
		ASSERT(FALSE, 0);
		return 0;
	}

	retry = 0;
	/* Wait tx buffer empty */
	while (RESET == SPI_GetStatus(pSpi, SPI_FLAG_TX_BUF_EMPTY))
	{
		retry++;
		if (retry > 200)
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
	temp = (BYTE)SPI_ReadData(pSpi);

	return temp;
}


//-----------------------------------------------
//函数功能: 初始化ADC
//
//参数:		无
//
//返回值:  	无
//
//备注: 掉电检测及电池电压检测
//-----------------------------------------------
static void InitADC(void)
{
	stc_clock_xtal_init_t stcXtalCfg;
	stc_clock_pll_init_t stcUpllCfg;
	stc_adc_init_t stcAdcInit;
	uint8_t au8Adc1SaSampTime[3] = {0xff, 0xff};
	BYTE i;
	stc_gpio_init_t stc_port = {
        .u16PinState = PIN_STAT_RST,      /* PIN_STATE_SET 或 PIN_STATE_RESET */
        .u16PinDir = PIN_DIR_IN,            /* PIN_DIR_IN 或 PIN_DIR_OUT */
        .u16PinOutputType = PIN_OUT_TYPE_CMOS, /* PIN_OUT_TYPE_CMOS 或 PIN_OUT_TYPE_NMOS */
        .u16PinDrv = PIN_MID_DRV,           /* PIN_LOW_DRV, PIN_MID_DRV, PIN_HIGH_DRV */
        .u16Latch = PIN_LATCH_OFF,          /* PIN_LATCH_ON 或 PIN_LATCH_OFF */
        .u16PullUp = PIN_PU_OFF,            /* PIN_PU_ON 或 PIN_PU_OFF */
        .u16Invert = PIN_INVT_OFF,          /* PIN_INVT_ON 或 PIN_INVT_OFF */
        .u16ExtInt = PIN_EXTINT_OFF,        /* PIN_EXTINT_ON 或 PIN_EXTINT_OFF */
        .u16PinInputType = PIN_IN_TYPE_CMOS, /* PIN_IN_TYPE_CMOS 或 PIN_IN_TYPE_SCHMIT */
        .u16PinAttr = PIN_ATTR_ANALOG       /* PIN_ATTR_DIGITAL 或 PIN_ATTR_ANALOG */
    };
	/* Configuration peripheral clock */
	FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_ADC1, ENABLE);

	for (i = 0; i < sizeof(Adc_IO) / sizeof(MCU_IO); i++)
	{
		GPIO_Init(Adc_IO[i].Port, Adc_IO[i].Pin, &stc_port);
	}

	/* Configures XTAL. PLLH input source is XTAL. */
	/* XTAL is already configured in SystemClockConfig() */

	(void)CLK_PLLStructInit(&stcUpllCfg);

	CLK_SetPeriClockSrc(CLK_PERIPHCLK_PLLR);

	ADC_StructInit(&stcAdcInit);

	stcAdcInit.u16ScanMode = ADC_MD_SEQA_SINGLESHOT;

	/* 2. Initialize ADC1. */
	ADC_Init(CM_ADC1, &stcAdcInit);

	/**************************** Add ADC1 channels ****************************/
	ADC_ChCmd(ADC_UNIT_1, ADC_SEQ_1, ADC_CH_1, ENABLE);
	ADC_SetSampleTime(ADC_UNIT_1, ADC_CH_1, au8Adc1SaSampTime[0]);
	// ADC_ChCmd(ADC_UNIT_2, ADC_SEQ_2, ADC_CH_2, ENABLE);
	// ADC_SetSampleTime(ADC_UNIT_2, ADC_CH_2, au8Adc1SaSampTime[1]);
}
//-----------------------------------------------
//函数功能: ADC采样值转为电压值
//
//参数: 	Data[in]:ADC采样值
//
//返回值: 	电压值
//
//备注:
//-----------------------------------------------
static float AdcDataToVolData(WORD Data)
{
	return ((float)Data * 3.300 / 4096);
}
//-----------------------------------------------
//函数功能: 获取AD数据
//
//参数:	Type:AD类型
//
//返回值:  	无
//
//备注:eOther_AD_PWR，eOther_AD_BAT:电源及电池电压检测
//-----------------------------------------------
WORD api_GetADData(eType_ADC Type)
{
	//	WORD wData[3] = {0};
	//	if( Type >= eOther_AD_Max )
	//	{
	//		return 0;
	//	}
	//	//获取AD值
	//	ADC_PollingSa(M4_ADC1,&wData[0],3,1);
	//
	//	return wData[(BYTE)Type];
	WORD wData = 0;
	BYTE bChannel;
	DWORD dwTimeCount, dwAdcTimeout;
	if (Type >= eOther_AD_Max)
	{
		return 0;
	}
	if (Type == eOther_AD_BAT)
	{
		bChannel = ADC_CH_1;
	}
	else
	{
		bChannel = ADC_CH_2;
	}
	//获取AD值
	ADC_Start(CM_ADC1);
	dwAdcTimeout = 1 * (SystemCoreClock / 10u / 1000u);
	dwTimeCount = 0u;
	while (dwTimeCount < dwAdcTimeout)
	{
		if (ADC_GetStatus(CM_ADC1, ADC_FLAG_EOCA) == SET)
		{
			ADC_ClearStatus(CM_ADC1, ADC_FLAG_EOCA);
			wData = ADC_GetValue(CM_ADC1, (uint8_t)bChannel);
			break;
		}
		dwTimeCount++;
	}

	if (dwTimeCount == dwAdcTimeout)
	{
		wData = 0;
	}

	return wData;
}
//-----------------------------------------------
//函数功能: 获取保护SPI指针
//
//参数:
//
//返回值:  	Point
//
//备注:
//-----------------------------------------------
// WORD api_GetSPIRcvPoint( void )
// {
// 	DWORD dwAddr;
// 	WORD Point;

// 	dwAddr = (*(volatile uint32_t *)((uint32_t)(&SPI_DMA_UNIT->MONDAR0) + ((SPI_RX_DMA_CH) * 0x40ul))); //传输目标地址寄存器
// 	Point = (WORD)(dwAddr - (uint32_t)(&Serial_SPI.ProBuf[0]));

// 	return Point;
// }
//-----------------------------------------------
//函数功能:初始化PWM输出
//
//参数: wPeriod[in]:周期(us),wCompare[in]:高电平时间(us)
//
//返回值:  无
//
//备注: 用于拓扑识别发送负荷信号，这里仅做初始化，不打开
//-----------------------------------------------
void api_TIMEA_2_PWM4_Init(WORD wPeriod, WORD wCompare)
{
}

//-----------------------------------------------
//函数功能: 计算cpu Flash的累加和，采用word相加模式
//
//参数: 	无
//
//返回值:  	计算的word累加和
//
//备注:
//-----------------------------------------------
WORD api_CheckCpuFlashSum(BYTE Type)
{
	DWORD Sum;
	DWORD StartZone, EndZone;
	WORD *pData;

	CLEAR_WATCH_DOG;

	if (Type == 1) //BOOT
	{
		StartZone = 0;
		EndZone = 0x7FFF;
	}
	else if (Type == 2) //CODE
	{
		StartZone = 0x8000;
		EndZone = 0x7DFFF;
	}
	else if (Type == 3) //SAFEPARA
	{
		StartZone = 0x7E000;
		EndZone = 0x7FFFF;
	}
	else //ALL
	{
		StartZone = 0;
		EndZone = 0x7FFFF;
	}

	Sum = 0;

	for (pData = (WORD *)StartZone; pData < (WORD *)EndZone; pData++)
	{
		Sum += *pData;
	}

	CLEAR_WATCH_DOG;

	return (WORD)Sum;
}
//-----------------------------------------------
//函数功能: 计算cpu Flash的累加和，采用word相加模式
//
//参数: 	无
//
//返回值:  	计算的word累加和
//
//备注:  志翔算法校验
//-----------------------------------------------
WORD api_CheckCpuFlashZXSum(void)
{
	DWORD Sum;
	DWORD StartZone, EndZone;
	WORD *pData;

	CLEAR_WATCH_DOG;

	StartZone = 0x78000;
	EndZone = 0x7FFFF;

	Sum = 0;

	for (pData = (WORD *)StartZone; pData < (WORD *)EndZone; pData++)
	{
		Sum += *pData;
	}

	CLEAR_WATCH_DOG;

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
//备注:   ROM:0~0x7FFFF;0x03000C00~0x03000FFB(未使用)
//        RAM：0x1fff'8000~0x1fff'ffff    (未使用)
//             0x20000000~0x2001FFFF
// 				0x20020000~0x20026FFF 堆栈
// 				0x200F0000~0x200F0FFF(未使用)
//  			未判断add+len的合法性
//-----------------------------------------------
BYTE api_ReadCpuRamAndInFlash(BYTE Type, DWORD Addr, BYTE Len, BYTE *Buf)
{
	if (Type > 1)
	{
		return 0;
	}

	if (Type == 0)
	{
		if ((Addr < 0x1fff8000) || (Addr > 0x20026FFF))
		{
			return 0;
		}
	}
	else if (Type == 1)
	{
		if (Addr >= 0x80000)
		{
			return 0;
		}
	}

	memcpy(Buf, (BYTE *)Addr, Len);

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
static void UpdateProg(void)
{
	BYTE i, ProgMessageNum, ProgData[PROGRAM_FLASH_SECTOR]; // 一次升级1024个字节
	BYTE IntStatus;
	WORD j;
	DWORD ProgAddr;
	TUpdateProgMessage TmpUpdateProgMessage;
	TUpdateProgHead TmpUpdateProgHead;
	BYTE *pData;

	FunctionConst(UPDATEPROG_TASK);
	// 获取数据
	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TmpUpdateProgHead), (BYTE *)&TmpUpdateProgHead) != TRUE)
	{
		return;
	}
	ProgAddr = TmpUpdateProgHead.ProgAddr;
	ProgMessageNum = TmpUpdateProgHead.ProgMessageNum;

	if (ProgMessageNum > (PROGRAM_FLASH_SECTOR / (sizeof(TmpUpdateProgMessage.ProgData))))
	{
		return;
	}

	//@	if( (ProgAddr%PROGRAM_FLASH_SECTOR) != 0 )
	//@	{
	//@		return;
	//@	}

	memcpy((void *)ProgData, (BYTE *)ProgAddr, sizeof(ProgData));

	for (i = 0; i < ProgMessageNum; i++)
	{
		if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[i]), sizeof(TmpUpdateProgMessage), (BYTE *)&TmpUpdateProgMessage) != TRUE)
		{
			return;
		}

		if (TmpUpdateProgMessage.ProgFrameLen) // 新方式帧长度不为0
		{
			memcpy(&ProgData[TmpUpdateProgMessage.ProgOffset], &TmpUpdateProgMessage.ProgData[0], TmpUpdateProgMessage.ProgFrameLen);
		}
		else
		{
			memcpy(&ProgData[sizeof(TmpUpdateProgMessage.ProgData) * i], &TmpUpdateProgMessage.ProgData[0], sizeof(TmpUpdateProgMessage.ProgData));
		}
	}

	// 更新数据
	for (i = 0; i < 2; i++)
	{
		IntStatus = api_splx(0);

		if (bsp_EraseOneSector(ProgAddr) == FALSE)
		{
			api_splx(IntStatus);
			return;
		}

		if (bsp_WriteCpuFlash(ProgAddr, ProgData, PROGRAM_FLASH_SECTOR) == FALSE)
		{
			api_splx(IntStatus);
			return;
		}

		api_splx(IntStatus);

		pData = (BYTE *)ProgAddr;
		for (j = 0; j < PROGRAM_FLASH_SECTOR; j++)
		{
			if (ProgData[j] != *(pData + j))
			{
				break;
			}
		}

		// 数据写成功 跳出循环
		if (j == PROGRAM_FLASH_SECTOR)
		{
			break;
			;
		}
	}

	// 数据写失败
	if ((i == 2) && (j != PROGRAM_FLASH_SECTOR))
	{
		return;
	}

	// 把eeprom中的报文头改为无效
	TmpUpdateProgHead.ProgAddr = 0x3fc01;
	TmpUpdateProgHead.ProgMessageNum = (PROGRAM_FLASH_SECTOR / (sizeof(TmpUpdateProgMessage) - sizeof(DWORD))) + 1;
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TmpUpdateProgHead), (BYTE *)&TmpUpdateProgHead);
}

//-----------------------------------------------
//函数功能: 规约写升级程序信息头
//
//参数: 	pBuf[in] 输入缓冲
//
//返回值:  	无
//
//备注: 规约见《电表软件平台扩展规约》附录C
//-----------------------------------------------
void api_WriteUpdateProgHead(BYTE *pBuf)
{
	TUpdateProgHead TmpUpdateProgHead;

	memcpy((BYTE *)&TmpUpdateProgHead.ProgAddr, pBuf, 4);
	TmpUpdateProgHead.ProgMessageNum = pBuf[4];

	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE *)&TmpUpdateProgHead);

	//清状态位，一次程序升级不允许掉电，升级过程中掉电需要重新升级
	memset(ReceiveProgStatus, 0x00, sizeof(ReceiveProgStatus));
}

//-----------------------------------------------
//函数功能: 规约写升级程序数据
//
//参数: 	pBuf[in] 输入缓冲
//
//返回值:  	TRUE/FALSE
//
//备注: 规约见《电表软件平台扩展规约》附录C
//-----------------------------------------------
BOOL api_WriteUpdateProgMessage(BYTE *pBuf)
{
	TTwoByteUnion tw;
	TUpdateProgMessage TmpUpdateProgMessage;

	memcpy(tw.b, pBuf, 2); //帧序号
	if (tw.w >= (PROGRAM_FLASH_SECTOR / sizeof(TmpUpdateProgMessage.ProgData)))
	{
		return FALSE;
	}

	// 旧升级方式的帧长度、偏移地址置为 0
	TmpUpdateProgMessage.ProgFrameLen = 0; // 帧长度
	TmpUpdateProgMessage.ProgOffset = 0;   // 偏移地址

	// 128字节数据，不倒序
	memcpy(TmpUpdateProgMessage.ProgData, pBuf + 2, sizeof(TmpUpdateProgMessage.ProgData));

	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32(TmpUpdateProgMessage.ProgData, sizeof(TmpUpdateProgMessage) - sizeof(DWORD));

	//不能对单帧重写，只能重头开始重写
	if ((ReceiveProgStatus[tw.w / 8] & (0x01 << (tw.w % 8))) == 0)
	{
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE *)&TmpUpdateProgMessage);
		ReceiveProgStatus[tw.w / 8] |= (0x01 << (tw.w % 8));
	}

	return TRUE;
}
//-----------------------------------------------
// 函数功能: 规约写升级程序数据
//
// 参数: 	pBuf[in] 输入缓冲
//
// 返回值:  	TRUE/FALSE
//
// 备注: 规约见《电表软件平台扩展规约》附录C
//-----------------------------------------------

BOOL api_WriteUpdateProgMessageNew(BYTE *pBuf)
{
	TTwoByteUnion tw;
	TTwoByteUnion len;
	TUpdateProgMessage TmpUpdateProgMessage;

	tw.w = 0;
	len.w = 0;

	memcpy(tw.b, pBuf, 2); // 帧序号
	if (tw.w >= (PROGRAM_FLASH_SECTOR / (sizeof(TmpUpdateProgMessage.ProgData))))
	{
		return FALSE;
	}

	memcpy(len.b, pBuf + 2, 2); // 帧长度
	if ((len.w > (sizeof(TmpUpdateProgMessage.ProgData))) || (len.w == 0))
	{
		return FALSE;
	}
	TmpUpdateProgMessage.ProgFrameLen = len.w; // 帧长度

	memcpy((BYTE *)&TmpUpdateProgMessage.ProgOffset, pBuf + 4, 2); // 偏移地址
	if ((TmpUpdateProgMessage.ProgFrameLen + TmpUpdateProgMessage.ProgOffset) > PROGRAM_FLASH_SECTOR)
	{
		return FALSE;
	}

	memset(TmpUpdateProgMessage.ProgData, 0, sizeof(TmpUpdateProgMessage.ProgData));
	memcpy(TmpUpdateProgMessage.ProgData, pBuf + 6, len.w);

	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32((BYTE *)&TmpUpdateProgMessage, sizeof(TmpUpdateProgMessage) - sizeof(DWORD));

	// 不能对单帧重写，只能重头开始重写
	if ((ReceiveProgStatus[tw.w / 8] & (0x01 << (tw.w % 8))) == 0)
	{
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE *)&TmpUpdateProgMessage);
		ReceiveProgStatus[tw.w / 8] |= (0x01 << (tw.w % 8));
	}
	return TRUE;
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
	BOOL Result;
	BYTE i, Num;
	TFourByteUnion tdw;
	TUpdateProgHead TmpUpdateProgHead;

	Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE *)&TmpUpdateProgHead);
	if (Result != TRUE)
	{
		return;
	}

	memcpy(tdw.b, pBuf, 4);
	if (tdw.d != TmpUpdateProgHead.ProgAddr)
	{
		return;
	}

	if (TmpUpdateProgHead.ProgMessageNum != pBuf[4])
	{
		return;
	}

	Num = 0;
	for (i = 0; i < TmpUpdateProgHead.ProgMessageNum; i++)
	{
		if (ReceiveProgStatus[i / 8] & (0x01 << (i % 8)))
		{
			Num++;
		}
	}
	if (Num != TmpUpdateProgHead.ProgMessageNum)
	{
		return;
	}

	//做一下掉电保存 !!!!!!
	api_PowerDownEnergy();

	ProgFlashPW = 0xa55a;

	UpdateProg();

	ProgFlashPW = 0;
	memset(ReceiveProgStatus, 0x00, sizeof(ReceiveProgStatus));

	//RESET
	//Reset_CPU();
}

/////////////////////////////////////////////////////////////////////
//以下是中断向量程序
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 串口中断服务子程序,改服务程序要求中断不允许嵌套
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------
//函数功能: 串口接收中断函数
//
//参数: 	SCI_Ph_Num[in]	物理串口号
//          Data[in]		接收到的数据
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void USARTx_Rcv_IRQHandler(BYTE SCI_Ph_Num, BYTE Data)
{
	BYTE i;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
		{
			if (Serial[i].RXWPoint < MAX_PRO_BUF_REAL)
			{
				Serial[i].ProBuf[Serial[i].RXWPoint++] = Data;
			}

			if (Serial[i].RXWPoint >= MAX_PRO_BUF_REAL)
			{
				//如果接受到数据等于大于最大值 但是协议里还一个字节都没处理 强制调用一次 解决发送512字节数据电表有概率不回复的问题
				if (Serial[i].RXRPoint == 0)
				{
					Proc645(i);
				}
				Serial[i].RXWPoint = 0;
			}
			//Serial[i].SendToSendDelay = 80;
			break;
		}
	}
}

//-----------------------------------------------
//函数功能: 串口发送中断函数
//
//参数: 	SCI_Ph_Num[in]	物理串口号
//          USARTx[in]		对应的串口指针
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void USARTx_Send_IRQHandler(BYTE SCI_Ph_Num, CM_USART_TypeDef *USARTx)
{
	BYTE i;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
		{
			if (Serial[i].TXPoint < Serial[i].SendLength)
			{
				USART_WriteData(USARTx, Serial[i].ProBuf[Serial[i].TXPoint++]);
			}
			else
			{

				USART_FuncCmd(USARTx, USART_INT_TX_EMPTY, DISABLE); // 关闭发送空中断
				USART_FuncCmd(USARTx, USART_INT_TX_CPLT, ENABLE);   // 打开发送完成中断
				Serial[i].SendToSendDelay = 100;
			}
			break;
		}
		
	}
}
#endif //#if ( CPU_TYPE == CPU_HC32F460 )
