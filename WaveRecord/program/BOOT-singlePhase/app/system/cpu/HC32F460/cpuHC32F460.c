//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.30
//����		CPU�������� //����ļ�Ҫֻ�������Ĳ��֣�������Ҫ�ú��ж�
//�޸ļ�¼	
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
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
// #define C_SysTickLoad   (((22020096>>SPD_MCU) / (1000/SYS_TICK_PERIOD_MS)) - 1)

// //6015��HRC��14M(6025����11.01M) ���Ե͹��Ļ��ѵ�ϵͳʱ��Ϊ7M ����5.5M SysTick���ٶȻ�ӿ�
// #define C_SingleLowPowerSysTickLoad   (((22020096>>SINGLE_LOW_POWER_SPD_MCU) / (1000/SYS_TICK_PERIOD_MS)) - 1)

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

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
	Ddl_Delay1ms(ms);
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
		Ddl_Delay1us(100);
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
		Ddl_Delay1us(10);
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
	stc_clk_sysclk_cfg_t stcSysClkCfg;
	stc_clk_xtal_cfg_t stcXtalCfg;
	stc_clk_mpll_cfg_t stcMpllCfg;
	stc_sram_config_t stcSramConfig;

	MEM_ZERO_STRUCT(stcSysClkCfg);
	MEM_ZERO_STRUCT(stcXtalCfg);
	MEM_ZERO_STRUCT(stcMpllCfg);
	MEM_ZERO_STRUCT(stcSramConfig);

	/* Set bus clk div. */
	stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;    // ���200M
	stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;   // ���100M
	stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // ���200M
	stcSysClkCfg.enPclk1Div = ClkSysclkDiv16; // ���100M     192/16=12M
	stcSysClkCfg.enPclk2Div = ClkSysclkDiv4;  // ���60M
	stcSysClkCfg.enPclk3Div = ClkSysclkDiv16; // ���50M     192/16=12M
	stcSysClkCfg.enPclk4Div = ClkSysclkDiv2;  // ���100M
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
	SRAM_Init(&stcSramConfig);// 2��CPU���ڣ���ֹECC

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

	// ����PCLK3  12M����
	// 12*1024*1024 /2048*16384 = 2.66s
	stcWdtInit.enClkDiv = WdtPclk3Div2048;
	stcWdtInit.enCountCycle = WdtCountCycle16384;
	stcWdtInit.enRefreshRange = WdtRefresh100Pct;
	stcWdtInit.enSleepModeCountEn = Disable;
	stcWdtInit.enRequestType = WdtTriggerResetRequest;
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
//��ע:
//-----------------------------------------------

void api_InitSPI( eCOMPONENT_TYPE Component, eSPI_MODE Type )
{
	stc_spi_init_t stcSpiInit;

	/* configuration structure initialization */
	MEM_ZERO_STRUCT(stcSpiInit);

	/* Configuration peripheral clock */
	PWC_Fcg1PeriphClockCmd(SPI_FLASH_CLOCK, Enable);
	SPI_ClearFlag(SPI_FLASH_UNIT, SpiFlagOverloadError); // ��������־����ֹ�д���ʱ���ܿ���SPI
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
	stcSpiInit.enWorkMode = SpiWorkMode3Line;				// ������
	stcSpiInit.enTransMode = SpiTransFullDuplex;
	stcSpiInit.enCommAutoSuspendEn = Disable;
	stcSpiInit.enModeFaultErrorDetectEn = Disable;
	stcSpiInit.enParitySelfDetectEn = Disable;
	stcSpiInit.enParityEn = Disable;
	stcSpiInit.enParity = SpiParityEven;
	stcSpiInit.enMasterSlaveMode = SpiModeMaster;
	// ������ ������Ч
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
	CLK_SetPeriClkSource(ClkPeriSrcUpllr); // ѡ��UPLLR�����ʱ��Ƶ��40M

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
	stcChCfg.u32Channel = 0x00000600; // ��9,10ͨ��
	stcChCfg.u8Sequence = ADC_SEQ_A;
	stcChCfg.pu8SampTime = au8Adc1SaSampTime; // ��������255������
	/* 2. Add ADC channel. */
	ADC_AddAdcChannel(M4_ADC1, &stcChCfg);
}
//-----------------------------------------------
//��������: ��ȡAD����
//
//����:	Type:AD����
//			
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
WORD api_GetADData(eType_ADC Type)
{
	WORD wData[3] = {0};
	if (Type >= eOther_AD_Max)
	{
		return 0;
	}
	// ��ȡADֵ
	ADC_PollingSa(M4_ADC1, &wData[0], 3, 1);
	return wData[(BYTE)Type];
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


#endif//#if ( CPU_TYPE == CPU_HC32F460 )
