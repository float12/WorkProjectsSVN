//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.30
//����		CPU�������� //����ļ�Ҫֻ�������Ĳ��֣�������Ҫ�ú��ж�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "hc32f4xx_conf.h"
#include "hc32_ddl.h"
#include "cpuHC32F4A0.h"

#if ( CPU_TYPE == CPU_HC32F4A0 )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
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
//ʱ�ӵ��ADC
#define ADC_UNIT_1						(CM_ADC1)
#define ADC_SEQ_1						(ADC_SEQ_A)
#define ADC_CH_1						(ADC_CH12)
//������ADC
#define ADC_UNIT_2						(CM_ADC1)
#define ADC_SEQ_2						(ADC_SEQ_A)
#define ADC_CH_2						(ADC_CH13)

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//SPI ���ܹܽŶ�Ӧ��ϵ
const MCU_IO Spi_IO[][3] =
{
	{
		 PortB, Pin02, Func42_Spi1_Miso,
        PortB, Pin01, Func41_Spi1_Mosi,
        PortB, Pin00, Func40_Spi1_Sck, 
	},
};

//ADC ���ܹܽŶ�Ӧ��ϵ
const MCU_IO Adc_IO[] =
{
	{ PortC, Pin02, Func0_Gpio},			//�ϵ��ѹ���
	{ PortC, Pin03, Func0_Gpio},			//��ص�ѹ���
};
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
DWORD	ReceiveProgStatus;
WORD 	ProgFlashPW;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

static void InitWatchDog(void);

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��ȡоƬ����
//
//����:
//
//����ֵ:   0x55: H��   0x00��G��
//
//��ע:�رն�Ӧ�Ķ�ʱ���ж�
//-----------------------------------------------
BYTE api_GetCpuVersion( void )
{
	return 0;
}

//-----------------------------------------------
//��������: ������ʱ����
//
//����: 	ms[in]		��Ҫ��ʱ�ĺ�����
//                    
//����ֵ:  	��
//
//��ע:    22M��Ƶʵ����ʱ1.01ms 
//-----------------------------------------------
void api_Delayms(WORD ms)
{
	DDL_DelayMS(ms);
}

//-----------------------------------------------
//��������: 100΢����ʱ����
//
//����: 	us[in]		��Ҫ��ʱ��100΢����
//                    
//����ֵ:  	��
//
//��ע:    22M��Ƶʱ ʵ��Լ��ʱ110us   ѭ��ֵ����256��С��256��ʱֵ���ܴ󣬴ӻ�࿴Ҳ��һ��
//		 11M��Ƶʱ ʵ��Լ��ʱ100us
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
//��������: 10΢����ʱ����
//
//����: 	us[in]		��Ҫ��ʱ��10΢����
//                    
//����ֵ:  	��
//
//��ע:    22M��Ƶ��ʵ��Լ13.75us,us = 3,ʵ��32us,us = 10��ʵ��96us
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
//��������: ͨ��NOP������ʱ����
//
//����: 	Step[in]		��Ҫ��ʱ��NOP����
//                    
//����ֵ:  	��
//
//��ע:   !!!!!!�ú�������һ������ָ����÷�ʽ������Ҫ����һ���ط����ú��������÷�ʽ !!!!!!
//-----------------------------------------------
#pragma optimize=size low//�м��Ż�������£����a_byDefaultData�Ż���
void api_DelayNop(BYTE Step)
{

}


//-----------------------------------------------
//��������: CPUϵͳʱ�ӳ�ʼ������
//
//����: 	Type[in]		ePowerOnMode:�����ϵ�  ePowerDownMode:�͹���
//                    
//����ֵ:  	��
//
//��ע:   
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
//��������:     systick��ʱ����ʼ������
//
//����: 	 	Type[in]		ePowerOnMode:�����ϵ�   ePowerDownMode:�͹���
//                    
//����ֵ:  	��
//
//��ע:       ePowerDownModeģʽ �����ڵ���
//-----------------------------------------------
void api_InitSysTickTimer( ePOWER_MODE Type)
{
	SysTick_Init(100); // ����Ϊ10ms�δ�ʱ��
}


void InitTimer4( void )
{

}







//-----------------------------------------------
//��������: CPU��ʼ��
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_InitCPU( void )
{
	api_Delay100us(10);
    
	//��ʼ��ϵͳʱ��
	api_MCU_SysClockInit( ePowerOnMode );
	//��ʼ�����Ź�
	InitWatchDog();
	CLEAR_WATCH_DOG;
	//ϵͳ��ʱ����ʼ����Tick��ʱ����
//	api_InitSysTickTimer( ePowerOnMode );
	//��ʼ����ʱ��4�����ڲ�����ʱ�䣬ʱ��Ϊ32768
	// InitTimer4();
	api_InitAdc();
}


//-----------------------------------------------
//��������: ���Ź���ʼ����HT6025�Ŀ��Ź�Ĭ���ǿ�����
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void InitWatchDog(void)
{
	stc_wdt_init_t stcWdtInit;

	/* configure structure initialization */
	MEM_ZERO_STRUCT(stcWdtInit);

	// ����PCLK3 24M����
	// 1024*65536 / (24*1024*1024) = 2.66s
	stcWdtInit.u32ClockDiv = WDT_CLK_DIV1024;
	stcWdtInit.u32CountPeriod = WDT_CNT_PERIOD65536;
	stcWdtInit.u32RefreshRange = WDT_RANGE_0TO100PCT;
	stcWdtInit.u32LPMCount = WDT_LPM_CNT_STOP;
	stcWdtInit.u32ExceptionType = WDT_EXP_TYPE_RST;
	WDT_Init(&stcWdtInit);
}




//-----------------------------------------------
//��������: �رպͻָ��ж�
//
//����: 
//			Data[in]		0:�ر��ж�  �������ر�ǰ��¼�����ݣ����ָ��ж�
//                    
//����ֵ:  	�ر�ǰ���ж�״̬
//
//��ע:   
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
//��������: ��ʼ��SPI����
//
//����:
//			Component[in]	Ԫ����eCOMPONENT_TYPE
//			Type[in]		SPI��ģʽ  eSPI_MODE_0/eSPI_MODE_1/eSPI_MODE_2/eSPI_MODE_3�ֱ��ӦSPI��ģʽ0/1/2/3
//����ֵ:  	��
//
//��ע: ֻʹ��FLASH
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
	SPI_ClearStatus(SPI_FLASH_UNIT,SPI_FLAG_OVERLOAD); //��������־����ֹ�д���ʱ���ܿ���SPI
	SPI_ClearStatus(SPI_FLASH_UNIT,SPI_FLAG_MD_FAULT);
	SPI_ClearStatus(SPI_FLASH_UNIT,SPI_FLAG_PARITY_ERR);
	SPI_ClearStatus(SPI_FLASH_UNIT,SPI_FLAG_UNDERLOAD);
	/* Configuration SPI pin */
	for( i = 0; i < 3; i++)
	{
		GPIO_SetFunc(Spi_IO[(BYTE)Component][i].Port, Spi_IO[(BYTE)Component][i].Pin, Spi_IO[(BYTE)Component][i].IoFunc_x);
	}

	/* Configuration SPI structure */
	stcSpiInit.u32WireMode = SPI_3_WIRE;				// ������
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
//��������: ���жϷ�ʽ��дSPI�ĺ���
//
//����: 
//			Data[in]		д���ֵ
//                    
//����ֵ:  	������ֵ
//
//��ע:   
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
//��������: ��ʼ��ADC
//
//����:
//			
//����ֵ:  	��
//
//��ע:
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
//��������: ��ȡAD����
//
//����:	wSystemVol[out]:ϵͳ�����ѹ
//			
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_GetADData(WORD* wSystemVol)
{
	DWORD dwTimeCount,dwAdcTimeout;
	
	//��ȡADֵ
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
//��������: ����cpu Flash���ۼӺͣ�����word���ģʽ
//
//����: 	��
//                    
//����ֵ:  	�����word�ۼӺ�
//
//��ע:95ms   
//-----------------------------------------------
WORD api_CheckCpuFlashSum(BYTE Type)
{
    DWORD Sum = 0;
    
    return (WORD)Sum;
}


//-----------------------------------------------
//��������: ��ȡcpu��ram��Ƭ��flash����
//
//����: 	Type[in]	0--Ram 	1--flash
//          Addr[in]	��ַ 
//			Len[in]		��ȡ����    
//			Buf[out]	�������     
//
//����ֵ:  	��ȡ���ݵĳ��ȣ����Ϊ0����ʾ��ȡʧ��
//
//��ע:   
//-----------------------------------------------
BYTE api_ReadCpuRamAndInFlash(BYTE Type, DWORD Addr, BYTE Len, BYTE *Buf)
{	
	return Len;
}


//-----------------------------------------------
//��������: ��������flash
//
//����: 	��     
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void UpdateProg( void )
{

}


#endif//#if ( CPU_TYPE == CPU_HC32F4A0 )
