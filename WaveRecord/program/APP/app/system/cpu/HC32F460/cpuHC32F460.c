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
#include "hc32f460_dmac.h"
#include "bsp_cpu_flash.h"

#if ( CPU_TYPE == CPU_HC32F460 )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// Sample SPI Config
/* SPI unit and clock definition */
#define SPI_SAMPLE_UNIT					(M4_SPI4)
#define SPI_SAMPLE_CLOCK                (PWC_FCG1_PERIPH_SPI4)

// ESAM SPI Config 
/* SPI unit and clock definition */
#define SPI_ESAM_UNIT					(M4_SPI3)
#define SPI_ESAM_CLOCK           		(PWC_FCG1_PERIPH_SPI3)

// Flash SPI Config
/* SPI unit and clock definition */
#define SPI_FLASH_UNIT					(M4_SPI4)
#define SPI_FLASH_CLOCK					(PWC_FCG1_PERIPH_SPI4)

// Flash_2 SPI Config
/* SPI unit and clock definition */
// #define SPI_FLASH2_UNIT					(M4_SPI2)
// #define SPI_FLASH2_CLOCK				(PWC_FCG1_PERIPH_SPI2)

// Sampleout SPI Config
/* SPI unit and clock definition */
#define SPI_SAMPLETOPO_UNIT				(M4_SPI1)
#define SPI_SAMPLETOPO_CLOCK			(PWC_FCG1_PERIPH_SPI1)

// DMA 保护ADC
#define ADC1_SA_DMA_UNIT            	(M4_DMA1)
#define ADC1_SA_DMA_CH              	(DmaCh2)
#define ADC1_SA_DMA_PWC             	(PWC_FCG0_PERIPH_DMA1)
#define ADC1_SA_DMA_TRGSRC          	(EVT_ADC1_EOCA)
// DMA 其他ADC
#define ADC1_SB_DMA_UNIT            	(M4_DMA1)
#define ADC1_SB_DMA_CH              	(DmaCh3)
// DMA SPI
#define SPI_DMA_UNIT            		(M4_DMA2)
#define SPI_TX_DMA_CH             		(DmaCh1)
#define SPI_RX_DMA_CH              		(DmaCh0)
#define SPI_DMA_DMA_PWC             	(PWC_FCG0_PERIPH_DMA2)
//DMA 8306吐数算拓扑
#define SPI_TOPO_DMA_UNIT				(M4_DMA1)
#define SPI_TOPO_TX_DMA_CH				(DmaCh1)
#define SPI_TOPO_RX_DMA_CH				(DmaCh0)
#define SPI_TOPO_DMA_CLOCK				(PWC_FCG0_PERIPH_DMA1)

#define SPIWAVE_DMA_TX_TRIG_SOURCE          (EVT_SPI1_SPTI)
#define SPIWAVE_DMA_RX_TRIG_SOURCE          (EVT_SPI1_SPRI)

//SPI-CS
#define SPI_CS_PORT             		(PortD)
#define SPI_CS_PIN          			(Pin11)
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//SPI 功能管脚对应关系
const MCU_IO Spi_IO[][3] =
{
	{
		{ PortE, Pin01, Func_Spi4_Sck},			//CLK
		{ PortE, Pin00, Func_Spi4_Mosi},		//Mosi
		{ PortB, Pin09, Func_Spi4_Miso},		//Miso  计量和FALSH
	},
	
	{
		{ PortE, Pin01, Func_Spi4_Sck},			//CLK
		{ PortE, Pin00, Func_Spi4_Mosi},		//Mosi
		{ PortB, Pin09, Func_Spi4_Miso},		//Miso  计量和FALSH
	},
	
	// {
	// 	{ PortE, Pin05, Func_Spi3_Sck},			//CLK
	// 	{ PortE, Pin03, Func_Spi3_Mosi},		//Mosi
	// 	{ PortE, Pin04, Func_Spi3_Miso},		//Miso  ESAM
	// },
	// {
	// 	{ PortC, Pin02, Func_Spi2_Sck},			//CLK
	// 	{ PortC, Pin01, Func_Spi2_Mosi},		//Mosi
	// 	{ PortC, Pin03, Func_Spi2_Miso},		//Miso	FLASH2
	// },
	{
		{ PortA, Pin07, Func_Spi1_Sck},			//CLK
		{ PortC, Pin04, Func_Spi1_Mosi}			//Mosi 从机模式 拓扑
		//没有Miso
	}
};


//ADC 功能管脚对应关系
const MCU_IO Adc_IO[] =
{
	{ PortC, Pin00, Func_Gpio},			//上电电压监测//工装从PA2改为PC0
	{ PortB, Pin01, Func_Gpio},			//电池电压监测
	// { PortA, Pin05, Func_Gpio},			//罗氏线圈电压监测
};
//const Fun FunctionConst = api_DelayNop;
BYTE	ReceiveProgStatus[8];// 128字节一帧，一个扇区8K，需要8*8个bit
WORD 	ProgFlashPW;

BYTE SpiTxBuffer[6] = {0x55,0x55,0x55,0x55,0x55,0x55};
BYTE bSPIExIntFlag=0;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void InitWatchDog(void);
static void InitADC(void);
static void SPI4_IRQ_Send(void);
static void SPI4_IRQ_Rcv(void);
static void TIMEA_1_PWM8_Init( void );
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
//备注:    48M主频时实测约延时1.01ms；72M主频时实测约延时1ms；96M主频时实测约延时1ms；
//		注意：延时越长，延时时间会有增加;仿真测试与不仿真测试延时有区别 !!!!!!
//		ms=500时：48M主频下实测约591ms;72M主频下实测约557ms;96M主频下实测约542ms;
//-----------------------------------------------
void api_Delayms(WORD ms)
{
	DWORD TargetValue,CurrentValue;
	DWORD LastValue,TempValue;  
	DWORD CpuClockMHz;
	DWORD SysTickLoad;
	
	CpuClockMHz = SystemCoreClock / 1000;	//基于滴答是10ms中断
	TargetValue  = ms * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;
	
	do
	{
		TempValue = SysTick->VAL;
		if( LastValue >= TempValue ) //Systick是向下计数的
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick的装载值是减1的
		}
		LastValue = TempValue;
	}while( CurrentValue < TargetValue );
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
	DWORD TargetValue,CurrentValue;
	DWORD LastValue,TempValue;  
	DWORD CpuClockMHz;
	DWORD SysTickLoad;
	
	CpuClockMHz = SystemCoreClock / 10000;	//基于滴答是10ms中断
	TargetValue  = us * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;
	
	do
	{
		TempValue = SysTick->VAL;
		if( LastValue >= TempValue ) //Systick是向下计数的
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick的装载值是减1的
		}
		LastValue = TempValue;
	}while( CurrentValue < TargetValue );
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
	DWORD TargetValue,CurrentValue;
	DWORD LastValue,TempValue;  
	DWORD CpuClockMHz;
	DWORD SysTickLoad;
	
	CpuClockMHz = SystemCoreClock / 100000;	//基于滴答是10ms中断
	TargetValue  = us * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;
	
	do
	{
		TempValue = SysTick->VAL;
		if( LastValue >= TempValue ) //Systick是向下计数的
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick的装载值是减1的
		}
		LastValue = TempValue;
	}while( CurrentValue < TargetValue );
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
#pragma optimize = size low//中级优化的情况下，会把a_byDefaultData优化掉
//固定位置方便短报文升级
void api_DelayNop(BYTE Step)@"FUN_CONST"
{
	BYTE i;
	
	i = a_byDefaultData[0];
	
	for(i=0; i<Step; i++)
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
		
	#if(SPD_HDSC_MCU == SPD_48000K)
		stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;    // 最高200M
		stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;   // 最高100M
		stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // 最高200M	48M
		stcSysClkCfg.enPclk1Div = ClkSysclkDiv4; // 最高100M    12M
		stcSysClkCfg.enPclk2Div = ClkSysclkDiv1;  // 最高60M	12M
		stcSysClkCfg.enPclk3Div = ClkSysclkDiv4; // 最高50M     12M
		stcSysClkCfg.enPclk4Div = ClkSysclkDiv1;  // 最高100M	48M
	#elif(SPD_HDSC_MCU == SPD_72000K)
		stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;    // 最高200M
		stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;   // 最高100M
		stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // 最高200M	72M
		stcSysClkCfg.enPclk1Div = ClkSysclkDiv4; // 最高100M    18M
		stcSysClkCfg.enPclk2Div = ClkSysclkDiv1;  // 最高60M	18M
		stcSysClkCfg.enPclk3Div = ClkSysclkDiv4; // 最高50M     18M
		stcSysClkCfg.enPclk4Div = ClkSysclkDiv1;  // 最高100M	72M
	#elif(SPD_HDSC_MCU == SPD_96000K)
		stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;    // 最高200M
		stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;   // 最高100M
		stcSysClkCfg.enPclk0Div = ClkSysclkDiv2;  // 最高200M	48M
		stcSysClkCfg.enPclk1Div = ClkSysclkDiv1; // 最高100M    96M
		stcSysClkCfg.enPclk2Div = ClkSysclkDiv2;  // 最高60M	48M
		stcSysClkCfg.enPclk3Div = ClkSysclkDiv8; // 最高50M     12M
		stcSysClkCfg.enPclk4Div = ClkSysclkDiv2;  // 最高100M	48M
	#endif
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
	#if(SPD_HDSC_MCU == SPD_48000K)
	EFM_SetLatency(EFM_LATENCY_1);
	#elif((SPD_HDSC_MCU == SPD_72000K) ||(SPD_HDSC_MCU == SPD_96000K))
	EFM_SetLatency(EFM_LATENCY_2);
	#else
	配置有问题，需重新配置
	#endif
	EFM_InstructionCacheCmd(Enable);
	EFM_Lock();

	/* MPLL config (XTAL / pllmDiv * plln / PllpDiv = 48M). XTAL: 12M*/
	#if(SPD_HDSC_MCU == SPD_48000K)
		stcMpllCfg.pllmDiv = 1ul;
		stcMpllCfg.plln = 32ul;
		stcMpllCfg.PllpDiv = 8ul;
		stcMpllCfg.PllqDiv = 8ul;
		stcMpllCfg.PllrDiv = 8ul;
	#elif(SPD_HDSC_MCU == SPD_72000K)
		stcMpllCfg.pllmDiv = 1ul;
		stcMpllCfg.plln = 30ul;
		stcMpllCfg.PllpDiv = 5ul;
		stcMpllCfg.PllqDiv = 5ul;
		stcMpllCfg.PllrDiv = 5ul;
	#elif(SPD_HDSC_MCU == SPD_96000K)
		stcMpllCfg.pllmDiv = 1ul;
		stcMpllCfg.plln = 32ul;
		stcMpllCfg.PllpDiv = 4ul;
		stcMpllCfg.PllqDiv = 4ul;
		stcMpllCfg.PllrDiv = 4ul;
	#endif

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

    if(Type == 0)
    {
        //判断FPara2的校验
        if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == TRUE )
        {
            Status = TRUE;
        }
    }
    else
    {
        Status = TRUE;
    }

	for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
    	if(SerialMap[i].SCI_Ph_Num == SciPhNum)
        {
            if(SerialMap[i].SerialType == eRS485_I)//485
            {
            	if( Status == TRUE )
                {
                 	BpsBak = FPara2->CommPara.I485;
                }
                else
                {
                  	BpsBak = CommParaConst.I485;//9600 1停止位 偶校验
                }
            }
            else if(SerialMap[i].SerialType == eIR)//红外
            {
            	BpsBak = 0x42;//红外口固定1200
            }
            else if(SerialMap[i].SerialType == eCR)
            {
            	if( Status == TRUE )
                {
                  	BpsBak = FPara2->CommPara.ComModule;
                }
                else
                {
                  	BpsBak = CommParaConst.ComModule;//9600bps
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
			// else if(SerialMap[i].SerialType == eCAN)
			// {
			// 	if( Status == TRUE )
            //     {
            //      	BpsBak = FPara2->CommPara.CanBaud;
            //     }
            //     else
            //     {
            //       	BpsBak = CommParaConst.CanBaud;//默认125KBPS
            //     }
			// }  
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
void DealSciFlag( BYTE Channel )
{
    //有改波特操作，电表应答完再改波特率
    if( Serial[Channel].OperationFlag & BIT0 )
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
void api_InitSci( BYTE Channel )
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
void api_ProSciTimer( BYTE Channel )
{
	BYTE IntStatus;

    if(Serial[Channel].BroadCastFlag==0xff)
	{
		DealSciFlag( Channel );
		SerialMap[Channel].SCIEnableRcv(SerialMap[Channel].SCI_Ph_Num);

		IntStatus = api_splx(0);
		api_InitSciStatus( Channel );
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
void api_InitCPU( void )
{
	//api_Delay100us(10);

	FunctionConst(INIT_BASE_TIMER);
	//初始化系统时钟
	api_MCU_SysClockInit( ePowerOnMode );
	//初始化看门狗
	InitWatchDog();
	CLEAR_WATCH_DOG;
	//系统定时器初始化（Tick定时器）
	api_InitSysTickTimer( ePowerOnMode );
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

	// 按照PCLK3  12M计算
	// 16384/(12*1000*1000 /2048) = 2.796s
	stcWdtInit.enClkDiv = WdtPclk3Div2048;
	stcWdtInit.enCountCycle = WdtCountCycle16384;
	stcWdtInit.enRefreshRange = WdtRefresh100Pct;
	stcWdtInit.enSleepModeCountEn = Disable;
	stcWdtInit.enRequestType = WdtTriggerResetRequest;
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
void api_MCU_RTCInt( void )
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
void api_InitSPI( eCOMPONENT_TYPE Component, eSPI_MODE Type )
{
	stc_spi_init_t stcSpiInit;
	stc_irq_regi_conf_t stcIrqRegiConf;
	BYTE i;

	if (Component == eCOMPONENT_SPI_SAMPLE)
	{
		/* configuration structure initialization */
		MEM_ZERO_STRUCT(stcSpiInit);

		/* Configuration peripheral clock */
		PWC_Fcg1PeriphClockCmd(SPI_SAMPLE_CLOCK, Enable);
		SPI_ClearFlag(SPI_SAMPLE_UNIT,SpiFlagOverloadError); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearFlag(SPI_SAMPLE_UNIT,SpiFlagModeFaultError);
		SPI_ClearFlag(SPI_SAMPLE_UNIT,SpiFlagParityError);
		SPI_ClearFlag(SPI_SAMPLE_UNIT,SpiFlagUnderloadError);

		/* Configuration SPI pin */
		for( i = 0; i < 3; i++)
		{
			PORT_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x, Disable);
		}

		/* Configuration SPI structure */
		#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.enClkDiv = SpiClkDiv256; // PCLK1  96/256=375k 
		#elif(SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.enClkDiv = SpiClkDiv32; // PCLK1  18/32=562.5k
		#endif
		
		stcSpiInit.enFrameNumber = SpiFrameNumber1;
		stcSpiInit.enDataLength = SpiDataLengthBit8;
		stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
		stcSpiInit.enSckPolarity = SpiSckIdleLevelLow;			//  CPOL=0   Mode1
		stcSpiInit.enSckPhase = SpiSckOddChangeEvenSample;		//	CPHA=1
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

		SPI_Init(SPI_SAMPLE_UNIT, &stcSpiInit);
		SPI_Cmd(SPI_SAMPLE_UNIT, Enable);
	}
//	else if (Component == eCOMPONENT_SPI_ESAM) //ESAM
//	{
//		//增加监视初始化时先关闭SPI
//		//SPI_Cmd(SPI_SAMPLE_DATA_UNIT, Disable);
//		/* configuration structure initialization */
//		MEM_ZERO_STRUCT(stcSpiInit);
//
//		/* Configuration peripheral clock */
//		PWC_Fcg1PeriphClockCmd(SPI_ESAM_CLOCK, Enable);
//		SPI_ClearFlag(SPI_ESAM_UNIT,SpiFlagOverloadError); //清除错误标志，防止有错误时不能开启SPI
//		SPI_ClearFlag(SPI_ESAM_UNIT,SpiFlagModeFaultError);
//		SPI_ClearFlag(SPI_ESAM_UNIT,SpiFlagParityError);
//		SPI_ClearFlag(SPI_ESAM_UNIT,SpiFlagUnderloadError);
//
//		/* Configuration SPI pin */
//		for( i = 0; i < 3; i++)
//		{
//			PORT_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x, Disable);
//		}
//		/* Configuration SPI structure */
//		#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
//		stcSpiInit.enClkDiv = SpiClkDiv32; // PCLK1  96/32=3M
//		#elif(SPD_HDSC_MCU == SPD_72000K)
//		stcSpiInit.enClkDiv = SpiClkDiv16; // PCLK1  18/16=1.125M
//		#endif
//		stcSpiInit.enFrameNumber = SpiFrameNumber1;
//		stcSpiInit.enDataLength = SpiDataLengthBit8;
//		stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
//		stcSpiInit.enSckPolarity = SpiSckIdleLevelHigh;			//  CPOL=1   Mode3
//		stcSpiInit.enSckPhase = SpiSckOddChangeEvenSample;		//	CPHA=1
//		stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;
//		stcSpiInit.enWorkMode = SpiWorkMode3Line;				// 三线制
//		stcSpiInit.enTransMode = SpiTransFullDuplex;
//		stcSpiInit.enCommAutoSuspendEn = Disable;
//		stcSpiInit.enModeFaultErrorDetectEn = Disable;
//		stcSpiInit.enParitySelfDetectEn = Disable;
//		stcSpiInit.enParityEn = Disable;
//		stcSpiInit.enParity = SpiParityEven;
//		stcSpiInit.enMasterSlaveMode = SpiModeMaster; 
//		// 三线制 以下无效
//		stcSpiInit.stcDelayConfig.enSsSetupDelayOption = SpiSsSetupDelayCustomValue;
//		stcSpiInit.stcDelayConfig.enSsSetupDelayTime = SpiSsSetupDelaySck1;
//		stcSpiInit.stcDelayConfig.enSsHoldDelayOption = SpiSsHoldDelayCustomValue;
//		stcSpiInit.stcDelayConfig.enSsHoldDelayTime = SpiSsHoldDelaySck1;
//		stcSpiInit.stcDelayConfig.enSsIntervalTimeOption = SpiSsIntervalCustomValue;
//		stcSpiInit.stcDelayConfig.enSsIntervalTime = SpiSsIntervalSck6PlusPck2;
//	
//		SPI_Init(SPI_ESAM_UNIT, &stcSpiInit);
//		SPI_Cmd(SPI_ESAM_UNIT, Enable);
//	}
	else if (Component == eCOMPONENT_SPI_FLASH)
	{
		/* configuration structure initialization */
		MEM_ZERO_STRUCT(stcSpiInit);
		/* Configuration peripheral clock */
		PWC_Fcg1PeriphClockCmd(SPI_FLASH_CLOCK, Enable);
		SPI_ClearFlag(SPI_FLASH_UNIT,SpiFlagOverloadError); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearFlag(SPI_FLASH_UNIT,SpiFlagModeFaultError);
		SPI_ClearFlag(SPI_FLASH_UNIT,SpiFlagParityError);
		SPI_ClearFlag(SPI_FLASH_UNIT,SpiFlagUnderloadError);
		/* Configuration SPI pin */
		for( i = 0; i < 3; i++)
		{
			PORT_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x, Disable);
		}

		/* Configuration SPI structure */
		#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.enClkDiv = SpiClkDiv64; // PCLK1  96/2=1.5M
		#elif(SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.enClkDiv = SpiClkDiv16; // PCLK1  18/16=1.125M
		#endif
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
	else if (Component == eCOMPONENT_SPI_HSDC)
	{
		/* configuration structure initialization */
		MEM_ZERO_STRUCT(stcSpiInit);
		/* Configuration peripheral clock */
		PWC_Fcg1PeriphClockCmd(SPI_SAMPLETOPO_CLOCK, Enable);
		SPI_ClearFlag(SPI_SAMPLETOPO_UNIT,SpiFlagOverloadError); //清除错误标志，防止有错误时不能开启SPI
		SPI_ClearFlag(SPI_SAMPLETOPO_UNIT,SpiFlagModeFaultError);
		SPI_ClearFlag(SPI_SAMPLETOPO_UNIT,SpiFlagParityError);
		SPI_ClearFlag(SPI_SAMPLETOPO_UNIT,SpiFlagUnderloadError);
		/* Configuration SPI pin */
		for( i = 0; i < 2; i++)
		{
			PORT_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x, Disable);
		}

		/* Configuration SPI structure */
		#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
		stcSpiInit.enClkDiv = SpiClkDiv16; // PCLK1  96/16=6M
		#elif(SPD_HDSC_MCU == SPD_72000K)
		stcSpiInit.enClkDiv = SpiClkDiv16; // PCLK1  18/16=1.125M
		#endif
		stcSpiInit.enFrameNumber = SpiFrameNumber1;
		stcSpiInit.enDataLength = SpiDataLengthBit8;
		stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
		stcSpiInit.enSckPolarity = SpiSckIdleLevelLow;			 //  CPOL=0   Mode1
		stcSpiInit.enSckPhase = SpiSckOddChangeEvenSample;		 //  CPHA=1
		stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;
		stcSpiInit.enWorkMode = SpiWorkMode4Line;
		stcSpiInit.enTransMode = SpiTransFullDuplex;
		stcSpiInit.enCommAutoSuspendEn = Disable;
		stcSpiInit.enModeFaultErrorDetectEn = Disable;
		stcSpiInit.enParitySelfDetectEn = Disable;
		stcSpiInit.enParityEn = Disable;
		stcSpiInit.enParity = SpiParityEven;
		stcSpiInit.enMasterSlaveMode = SpiModeSlave;			//从机模式
		stcSpiInit.stcSsConfig.enSsValidBit = SpiSsValidChannel0;
    	stcSpiInit.stcSsConfig.enSs0Polarity = SpiSsLowValid;
		// 三线制 以下无效
		stcSpiInit.stcDelayConfig.enSsSetupDelayOption = SpiSsSetupDelayCustomValue;
		stcSpiInit.stcDelayConfig.enSsSetupDelayTime = SpiSsSetupDelaySck1;
		stcSpiInit.stcDelayConfig.enSsHoldDelayOption = SpiSsHoldDelayCustomValue;
		stcSpiInit.stcDelayConfig.enSsHoldDelayTime = SpiSsHoldDelaySck1;
		stcSpiInit.stcDelayConfig.enSsIntervalTimeOption = SpiSsIntervalCustomValue;
		stcSpiInit.stcDelayConfig.enSsIntervalTime = SpiSsIntervalSck6PlusPck2;

		SPI_Init(SPI_SAMPLETOPO_UNIT, &stcSpiInit);
		SPI_Cmd(SPI_SAMPLETOPO_UNIT, Enable);
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
	M4_SPI_TypeDef *pSpi;
	BYTE temp,retry;

	if( Component == eCOMPONENT_SPI_FLASH )
	{
		pSpi = SPI_FLASH_UNIT;
	}
	else if( Component == eCOMPONENT_SPI_SAMPLE )
	{
		pSpi = SPI_SAMPLE_UNIT;
	}
//	else if( Component == eCOMPONENT_SPI_ESAM )
//	{
//		pSpi = SPI_ESAM_UNIT;
//	}
	else//8306吐数据那一路SPI没有写功能
	{
		ASSERT(FALSE, 0);
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
//函数功能: 拓扑数据用DMA初始化
//
//参数: 
//                    
//返回值:
//
//备注:   
//-----------------------------------------------
void TopWaveSpiDmaConfig(void)
{
	stc_dma_config_t stcDmaCfg;
	
	/* configuration structure initialization */
    MEM_ZERO_STRUCT(stcDmaCfg);

    /* Configuration peripheral clock */
    PWC_Fcg0PeriphClockCmd(SPI_TOPO_DMA_CLOCK, Enable);
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS, Enable);  //PWC_FCG0_PERIPH_AOS

    /* Configure TX DMA */
    stcDmaCfg.u16BlockSize = 1u;
	stcDmaCfg.u16TransferCnt = sizeof(sTopoWave.RxBuf);
	stcDmaCfg.u32SrcAddr = (uint32_t)(&sTopoWave.RxBuf[0]);
    stcDmaCfg.u32DesAddr = (uint32_t)(&SPI_SAMPLETOPO_UNIT->DR);
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressIncrease;
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressFix;
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma8Bit;
    stcDmaCfg.stcDmaChCfg.enIntEn = Enable;
    DMA_InitChannel(SPI_TOPO_DMA_UNIT, SPI_TOPO_TX_DMA_CH, &stcDmaCfg);

    /* Configure RX DMA */
    stcDmaCfg.u16BlockSize = 1u;
    stcDmaCfg.u16TransferCnt = sizeof(sTopoWave.RxBuf);
    stcDmaCfg.u32SrcAddr = (uint32_t)(&SPI_SAMPLETOPO_UNIT->DR);
    stcDmaCfg.u32DesAddr = (uint32_t)(&sTopoWave.RxBuf[0]);
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma8Bit;
    stcDmaCfg.stcDmaChCfg.enIntEn = Enable;
    DMA_InitChannel(SPI_TOPO_DMA_UNIT, SPI_TOPO_RX_DMA_CH, &stcDmaCfg);

    DMA_SetTriggerSrc(SPI_TOPO_DMA_UNIT, SPI_TOPO_TX_DMA_CH, SPIWAVE_DMA_TX_TRIG_SOURCE);
    DMA_SetTriggerSrc(SPI_TOPO_DMA_UNIT, SPI_TOPO_RX_DMA_CH, SPIWAVE_DMA_RX_TRIG_SOURCE);

    /* Enable DMA. */
    DMA_Cmd(SPI_TOPO_DMA_UNIT, Enable);
}

//-----------------------------------------------
//函数功能: 拓扑数据用DMA使能
//
//参数: 
//                    
//返回值:
//
//备注:   
//-----------------------------------------------
void TopWaveSpiDmaEnableInit(void)
{

	SPI_SAMPLETOPO_UNIT->SR = 0;
	SPI_Cmd(SPI_SAMPLETOPO_UNIT, Disable);
	
	DMA_ClearIrqFlag(SPI_TOPO_DMA_UNIT, SPI_TOPO_RX_DMA_CH, TrnCpltIrq);
	DMA_ClearIrqFlag(SPI_TOPO_DMA_UNIT, SPI_TOPO_RX_DMA_CH, TrnCpltIrq);
	
	DMA_SetDesAddress(SPI_TOPO_DMA_UNIT, SPI_TOPO_RX_DMA_CH, (uint32_t)(&sTopoWave.RxBuf[0]) );
	DMA_SetTransferCnt(SPI_TOPO_DMA_UNIT, SPI_TOPO_RX_DMA_CH, sizeof(sTopoWave.RxBuf));
	
	DMA_SetSrcAddress(SPI_TOPO_DMA_UNIT, SPI_TOPO_TX_DMA_CH, (uint32_t)(&sTopoWave.RxBuf[0]) );
	DMA_SetTransferCnt(SPI_TOPO_DMA_UNIT, SPI_TOPO_TX_DMA_CH, sizeof(sTopoWave.RxBuf) );
	
	/* Enable DMA channel */
    DMA_ChannelCmd(SPI_TOPO_DMA_UNIT, SPI_TOPO_TX_DMA_CH, Enable);
    DMA_ChannelCmd(SPI_TOPO_DMA_UNIT, SPI_TOPO_RX_DMA_CH, Enable);
	
	SPI_SAMPLETOPO_UNIT->SR = 0;
	SPI_Cmd(SPI_SAMPLETOPO_UNIT, Enable);
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
	stc_clk_xtal_cfg_t stcXtalCfg;
    stc_clk_upll_cfg_t stcUpllCfg;
    stc_adc_init_t stcAdcInit;
    stc_adc_ch_cfg_t stcChCfg;
    uint8_t au8Adc1SaSampTime[3] = {0xff,0xff,0xff};
	BYTE i;
    stc_port_init_t stc_port = {Pin_Mode_Ana,Disable,Disable,Disable,Disable,Pin_Drv_M,Pin_OType_Od,Enable};
    
	/* Configuration peripheral clock */
	PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_ADC1, Enable);

	for( i = 0; i < sizeof(Adc_IO)/sizeof(MCU_IO); i++)
	{
		PORT_Init(Adc_IO[i].Port,Adc_IO[i].Pin,&stc_port);
	}

    MEM_ZERO_STRUCT(stcXtalCfg);

    /* Use XTAL as UPLL source. */
    stcXtalCfg.enFastStartup = Enable;
    stcXtalCfg.enMode = ClkXtalModeOsc;
    stcXtalCfg.enDrv  = ClkXtalLowDrv;
    CLK_XtalConfig(&stcXtalCfg);
    CLK_XtalCmd(Enable);

    MEM_ZERO_STRUCT(stcUpllCfg);
    
    /* Set UPLL out 240MHz. */
    stcUpllCfg.pllmDiv = 2u;
    /* upll = 12M(XTAL) / pllmDiv * plln */
    stcUpllCfg.plln    = 40u;
    stcUpllCfg.PllpDiv = 6u;
    stcUpllCfg.PllqDiv = 6u;
    stcUpllCfg.PllrDiv = 6u;
    CLK_SetPllSource(ClkPllSrcXTAL);
    CLK_UpllConfig(&stcUpllCfg);
    CLK_UpllCmd(Enable);
    CLK_SetPeriClkSource(ClkPeriSrcUpllr);//选用UPLLR，输出时钟频率40M

    MEM_ZERO_STRUCT(stcAdcInit);

    stcAdcInit.enResolution = AdcResolution_12Bit;
    stcAdcInit.enDataAlign  = AdcDataAlign_Right;
    stcAdcInit.enAutoClear  = AdcClren_Enable;
    stcAdcInit.enScanMode   = AdcMode_SAOnce;
    stcAdcInit.enRschsel    = AdcRschsel_Restart;

    /* 2. Initialize ADC1. */
    ADC_Init(M4_ADC1, &stcAdcInit);

    MEM_ZERO_STRUCT(stcChCfg);

    /**************************** Add ADC1 channels ****************************/
    stcChCfg.u32Channel  = 0x00000600;// 9,10通道               0x00000224;	//第9,5,2通道 
    stcChCfg.u8Sequence  = ADC_SEQ_A;
    stcChCfg.pu8SampTime = au8Adc1SaSampTime;	//采样按照255个周期
    /* 2. Add ADC channel. */
    ADC_AddAdcChannel(M4_ADC1, &stcChCfg);
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
static float AdcDataToVolData( WORD Data )
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
WORD api_GetADData( eType_ADC Type )
{
	WORD wData[3] = {0};
	if( Type >= eOther_AD_Max )
	{
		return 0;
	}
	//获取AD值
	ADC_PollingSa(M4_ADC1,&wData[0],3,1);

	return wData[(BYTE)Type];
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
	DWORD StartZone,EndZone;
    WORD *pData;
    
    CLEAR_WATCH_DOG;
	
	if (Type == 1)//BOOT
	{
		StartZone = 0;		
		EndZone = 0x7FFF;
	}
	else if (Type == 2)//CODE
	{
		StartZone = 0x8000;		
		EndZone = 0x7DFFF;
	}
	else if (Type == 3)//SAFEPARA
	{
		StartZone = 0x7E000;		
		EndZone = 0x7FFFF;
	}
	else//ALL
	{
		StartZone = 0;		
		EndZone = 0x7FFFF;
	}

	Sum = 0;
 
	for( pData = (WORD *)StartZone; pData < (WORD *)EndZone; pData++ )
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
WORD api_CheckCpuFlashZXSum( void )
{
    DWORD Sum;
	DWORD StartZone,EndZone;
    WORD *pData;
    
    CLEAR_WATCH_DOG;
	
	StartZone = 0x78000;		
	EndZone = 0x7FFFF;
	
	Sum = 0;
 
	for( pData = (WORD *)StartZone; pData < (WORD *)EndZone; pData++ )
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
	if( Type > 1 )
	{
		return 0;
	}
	
	if( Type == 0)
	{
		if( (Addr < 0x1fff8000)||(Addr  >0x20026FFF) )
		{
			return 0;
		}
	}
	else if( Type == 1 )
	{
		if( Addr >= 0x80000 )
		{
			return 0;
		}
	}
	
	memcpy(Buf,(BYTE*)Addr,Len);
	
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

		if( bsp_WriteCpuFlash(ProgAddr, ProgData, PROGRAM_FLASH_SECTOR) == FALSE )
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

	memcpy((BYTE*)&TmpUpdateProgHead.ProgAddr,pBuf,4);
	TmpUpdateProgHead.ProgMessageNum = pBuf[4];
	
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE*)&TmpUpdateProgHead );	
	
	//清状态位，一次程序升级不允许掉电，升级过程中掉电需要重新升级
	memset( ReceiveProgStatus, 0x00, sizeof(ReceiveProgStatus));
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
	
	memcpy(tw.b,pBuf,2);	//帧序号
	if (tw.w >= (PROGRAM_FLASH_SECTOR / sizeof(TmpUpdateProgMessage.ProgData)))
	{
		return FALSE;
	}

	// 旧升级方式的帧长度、偏移地址置为 0
	TmpUpdateProgMessage.ProgFrameLen = 0; // 帧长度
	TmpUpdateProgMessage.ProgOffset = 0;   // 偏移地址

	// 128字节数据，不倒序
	memcpy(TmpUpdateProgMessage.ProgData, pBuf + 2, sizeof(TmpUpdateProgMessage.ProgData));

	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32( TmpUpdateProgMessage.ProgData, sizeof(TmpUpdateProgMessage)-sizeof(DWORD) );
	
	//不能对单帧重写，只能重头开始重写
	if ((ReceiveProgStatus[tw.w/8] & (0x01<<(tw.w%8))) == 0)
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE*)&TmpUpdateProgMessage );
		ReceiveProgStatus[tw.w/8] |= (0x01<<(tw.w%8));
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
	if ((ReceiveProgStatus[tw.w/8] & (0x01<<(tw.w%8))) == 0)
	{
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE *)&TmpUpdateProgMessage);
		ReceiveProgStatus[tw.w/8] |= (0x01<<(tw.w%8));
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
	BYTE i,Num;
	TFourByteUnion tdw;	
	TUpdateProgHead TmpUpdateProgHead;
	
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE*)&TmpUpdateProgHead );	
	if(Result != TRUE)
	{
		return;
	}
	
	memcpy(tdw.b,pBuf,4);
	if(tdw.d != TmpUpdateProgHead.ProgAddr)
	{
		return;
	}
	
	if(TmpUpdateProgHead.ProgMessageNum != pBuf[4])
	{
		return;
	}
	
	Num = 0;
	for(i=0;i<TmpUpdateProgHead.ProgMessageNum;i++)
	{
		if (ReceiveProgStatus[i/8] & (0x01<<(i%8)))
		{
			Num++;
		}
	}
	if(Num != TmpUpdateProgHead.ProgMessageNum)
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

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
        {
            if(Serial[ i ].RXWPoint < MAX_PRO_BUF_REAL)
            {
	            Serial[ i ].ProBuf[Serial[ i ].RXWPoint++] = Data;
            }

            if(Serial[ i ].RXWPoint >= MAX_PRO_BUF_REAL)
            {
				//如果接受到数据等于大于最大值 但是协议里还一个字节都没处理 强制调用一次 解决发送512字节数据电表有概率不回复的问题
				if(Serial[i].RXRPoint == 0)
				{
					Proc645( i );
				}
                Serial[ i ].RXWPoint = 0;
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
void USARTx_Send_IRQHandler(BYTE SCI_Ph_Num, M4_USART_TypeDef *USARTx)
{
	BYTE i;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
		{
			if (Serial[i].TXPoint < Serial[i].SendLength)
			{
				USART_SendData(	USARTx, Serial[i].ProBuf[Serial[i].TXPoint++]);
			}
			else
			{
				USART_FuncCmd(USARTx, UsartTxEmptyInt, Disable);// 关闭发送空中断
				USART_FuncCmd(USARTx, UsartTxCmpltInt, Enable);// 打开发送完成中断
				Serial[i].SendToSendDelay = 100;
			}
			break;
		}
	}
}
//-----------------------------------------------
//函数功能: SPI-CS中断函数
//
//参数: 	
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void ExtInt11_IRQHandler(void)
{
    if (Set == EXINT_IrqFlgGet(ExtiCh11))
    {
        bSPIExIntFlag = 1;
        EXINT_IrqFlgClr(ExtiCh11);
    }
}
#endif//#if ( CPU_TYPE == CPU_HC32F460 )
