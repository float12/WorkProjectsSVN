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
#include "hc32_ll_dma.h"
#include "bsp_cpu_flash.h"

#if (CPU_TYPE == CPU_HC32F4A0)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//ADC Config
//ʱ�ӵ��ADC
#define ADC_UNIT_1 (CM_ADC1)
#define ADC_SEQ_1 (ADC_SEQ_A)
#define ADC_CH_1 (ADC_CH11)
//������ADC
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
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//SPI ���ܹܽŶ�Ӧ��ϵ,��ecomponent˳��һ��
const MCU_IO Spi_IO[][3] =
	{
		{
			{PortB, Pin00, Func40_Spi1_Sck},  //CLK
			{PortB, Pin01, Func41_Spi1_Mosi}, //Mosi
			{PortB, Pin02, Func42_Spi1_Miso}, //Miso  ����
		},
		{
			{PortC, Pin04, Func43_Spi2_Sck},  //CLK
			{PortC, Pin05, Func44_Spi2_Mosi}, //Mosi hsdc1
			{PortB, Pin12, Func19_Spi2_Nss0_2},
		},
		{
			{PortB, Pin04, Func46_Spi3_Sck},  //CLK
			{PortB, Pin05, Func47_Spi3_Mosi}, //Mosi hsdc2
			{PortB, Pin15, Func49_Spi3_Nss0}, //cs  ����ѡ��Ϊ��Ӧspi��nss0
		},
		{
			{PortC, Pin06, Func43_Spi4_Sck},  //CLK
			{PortC, Pin07, Func44_Spi4_Mosi}, //Mosi hsdc3
			{PortD, Pin00, Func46_Spi4_Nss0}, //cs  ����ѡ��Ϊ��Ӧspi��nss0
		},
		{
			{PortA, Pin08, Func46_Spi6_Sck},  //CLK
			{PortA, Pin09, Func47_Spi6_Mosi}, //Mosi hsdc4
			{PortA, Pin11, Func49_Spi6_Nss0}, //cs  ����ѡ��Ϊ��Ӧspi��nss0
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

//ADC ���ܹܽŶ�Ӧ��ϵ
const MCU_IO Adc_IO[]=
{
		{PortC, Pin01, Func0_Gpio}, //�ϵ��ѹ���
		// {PortC, Pin03, Func0_Gpio}, //��ص�ѹ���
};
//const Fun FunctionConst = api_DelayNop;
BYTE ReceiveProgStatus[8]; // 128�ֽ�һ֡��һ������8K����Ҫ8*8��bit
WORD ProgFlashPW;

BYTE SpiTxBuffer[6] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
BYTE bSPIExIntFlag = 0;

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void InitWatchDog(void);
static void InitADC(void);
static void SPI4_IRQ_Send(void);
static void SPI4_IRQ_Rcv(void);
static void TIMEA_1_PWM8_Init(void);
static void ExtInt11_IRQHandler(void);
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
BYTE api_GetCpuVersion(void)
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
//��ע:    48M��Ƶʱʵ��Լ��ʱ1.01ms��72M��Ƶʱʵ��Լ��ʱ1ms��96M��Ƶʱʵ��Լ��ʱ1ms��
//		ע�⣺��ʱԽ������ʱʱ���������;��������벻���������ʱ������ !!!!!!
//		ms=500ʱ��48M��Ƶ��ʵ��Լ591ms;72M��Ƶ��ʵ��Լ557ms;96M��Ƶ��ʵ��Լ542ms;
//-----------------------------------------------
void api_Delayms(WORD ms)
{
	DWORD TargetValue, CurrentValue;
	DWORD LastValue, TempValue;
	DWORD CpuClockMHz;
	DWORD SysTickLoad;

	CpuClockMHz = SystemCoreClock / 1000; //���ڵδ���10ms�ж�
	TargetValue = ms * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;

	do
	{
		TempValue = SysTick->VAL;
		if (LastValue >= TempValue) //Systick�����¼�����
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick��װ��ֵ�Ǽ�1��
		}
		LastValue = TempValue;
	} while (CurrentValue < TargetValue);
}

//-----------------------------------------------
//��������: 100΢����ʱ����
//
//����: 	us[in]		��Ҫ��ʱ��100΢����
//
//����ֵ:  	��
//
//��ע:    48M��Ƶʱʵ��Լ��ʱ103us��72M��Ƶʱʵ��Լ��ʱ101.75us��96M��Ƶʱʵ��Լ��ʱ101.3us��
//-----------------------------------------------
void api_Delay100us(WORD us)
{
	DWORD TargetValue, CurrentValue;
	DWORD LastValue, TempValue;
	DWORD CpuClockMHz;
	DWORD SysTickLoad;

	CpuClockMHz = SystemCoreClock / 10000; //���ڵδ���10ms�ж�
	TargetValue = us * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;

	do
	{
		TempValue = SysTick->VAL;
		if (LastValue >= TempValue) //Systick�����¼�����
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick��װ��ֵ�Ǽ�1��
		}
		LastValue = TempValue;
	} while (CurrentValue < TargetValue);
}

//-----------------------------------------------
//��������: 10΢����ʱ����
//
//����: 	us[in]		��Ҫ��ʱ��10΢����
//
//����ֵ:  	��
//
//��ע:    48M��Ƶ��ʵ��Լ12us;72M��Ƶʱʵ��Լ��ʱ11us��96M��Ƶʱʵ��Լ��ʱ10.5us��
//		ע�⣺��ʱԽ������ʱʱ���������
//		us=5ʱ��48M��Ƶ��ʵ��Լ49us;72M��Ƶ��ʵ��Լ48us;96M��Ƶ��ʵ��Լ47us;
//-----------------------------------------------
void api_Delay10us(WORD us)
{
	DWORD TargetValue, CurrentValue;
	DWORD LastValue, TempValue;
	DWORD CpuClockMHz;
	DWORD SysTickLoad;

	CpuClockMHz = SystemCoreClock / 100000; //���ڵδ���10ms�ж�
	TargetValue = us * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;

	do
	{
		TempValue = SysTick->VAL;
		if (LastValue >= TempValue) //Systick�����¼�����
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick��װ��ֵ�Ǽ�1��
		}
		LastValue = TempValue;
	} while (CurrentValue < TargetValue);
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
#pragma optimize = size low //�м��Ż�������£����a_byDefaultData�Ż���
//�̶�λ�÷���̱�������
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
//��������: CPUϵͳʱ�ӳ�ʼ������
//
//����: 	Type[in]		ePowerOnMode:�����ϵ�  ePowerDownMode:�͹���
//
//����ֵ:  	��
//
//��ע:
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
    /* Ŀǰ192MHZ*/
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
//��������:     systick��ʱ����ʼ������
//
//����: 	 	Type[in]		ePowerOnMode:�����ϵ�   ePowerDownMode:�͹���
//
//����ֵ:  	��
//
//��ע:       ePowerDownModeģʽ �����ڵ���
//-----------------------------------------------
void api_InitSysTickTimer(ePOWER_MODE Type)
{
	SysTick_Init(100); // ����Ϊ10ms�δ�ʱ��
}

void InitTimer4(void)
{
}

//-----------------------------------------------
//��������: ��ȡ�����ʲ���
//
//����: 	Type[in]		0:��ʼ��ʱ�ò����� 1��ƽʱ��������
//          SciPhNum[in]	�����ں�
//
//����ֵ:  	�����ʲ���ֵ
//
//��ע:
//-----------------------------------------------
BYTE api_GetBaudRate(BYTE Type, BYTE SciPhNum)
{
	BYTE i;
	BYTE BpsBak, Status;
	Status = FALSE;

	if (Type == 0)
	{
		//�ж�FPara2��У��
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
					BpsBak = CommParaConst.I485; //9600 1ֹͣλ żУ��
				}
			}
			else if (SerialMap[i].SerialType == eIR) //����
			{
				BpsBak = 0x42; //����ڹ̶�1200
			}
            else if (SerialMap[i].SerialType == e4gWaveData) //��4g���Ͳ�������
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
					BpsBak = CommParaConst.CanBaud; //Ĭ��125KBPS
				}
			}
			break;
		}
	}

	return BpsBak;
}

//-----------------------------------------------
//��������: ������Ĳ����ʺ�Ĵ��ڳ�ʼ��
//
//����: 	Channel[in]		�߼����ں� 0����һ485  1������  2��ģ��
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void DealSciFlag(BYTE Channel)
{
	//�иĲ��ز��������Ӧ�����ٸĲ�����
	if (Serial[Channel].OperationFlag & BIT0)
	{
		Serial[Channel].OperationFlag &= ~BIT0;
		SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
	}
}

//-----------------------------------------------
//��������: ���ڳ�ʼ��
//
//����:
//			Channel[in]		�߼����ں� 0����һ485  1������  2��ģ��
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_InitSci(BYTE Channel)
{
	SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
}

//-----------------------------------------------
//��������: ��ָֹ�����ڽ���
//
//����:
//			p[in]		��Ҫ��ֹ���ڵ�ָ��
//
//����ֵ:  	��
//
//��ע:
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
//��������: ����ָ���ӿڽ���
//
//����:
//			p[in]		��Ҫ�����ڵ�ָ��
//
//����ֵ:  	��
//
//��ע:
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
//��������: �㲥������Ӧ�������´��ڻָ�����
//
//����:
//			Channel[in]		�߼����ں� 0����һ485  1������  2��ģ��
//
//����ֵ:  	��
//
//��ע:
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
//��������: CPU��ʼ��
//
//����: 	��
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_InitCPU(void)
{
	//api_Delay100us(10);
	//������дʹ��
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);
	
	FunctionConst(INIT_BASE_TIMER);
	//��ʼ��ϵͳʱ��
	api_MCU_SysClockInit(ePowerOnMode);
	//��ʼ�����Ź�
	InitWatchDog();
	CLEAR_WATCH_DOG;
	//ϵͳ��ʱ����ʼ����Tick��ʱ����
	api_InitSysTickTimer(ePowerOnMode);
	//ADC��ʼ��
	InitADC();
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
//��������: RTC���ж�ʹ��
//
//����: 	Type[in]		0:�� �� ʱ�жϿ���  1:���жϿ���
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_MCU_RTCInt(void)
{
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
		SPI_ClearStatus(SPI_SAMPLE_UNIT, SPI_FLAG_OVERLOAD); //��������־����ֹ�д���ʱ���ܿ���SPI
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
		stcSpiInit.u32WireMode = SPI_3_WIRE; // ������
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
		SPI_ClearStatus(SPI_FLASH_UNIT, SPI_FLAG_OVERLOAD); //��������־����ֹ�д���ʱ���ܿ���SPI
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
		stcSpiInit.u32WireMode = SPI_3_WIRE; // ������
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
		SPI_ClearStatus(SPI_SAMPLE1_TOPO_UNIT, SPI_FLAG_OVERLOAD); //��������־����ֹ�д���ʱ���ܿ���SPI
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
		stcSpiInit.u32WireMode = SPI_4_WIRE; // ������
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
		SPI_ClearStatus(SPI_SAMPLE2_TOPO_UNIT, SPI_FLAG_OVERLOAD); //��������־����ֹ�д���ʱ���ܿ���SPI
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
		stcSpiInit.u32WireMode = SPI_4_WIRE; // ������
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
		// stcSpiInit.u32ModeFaultDetect = SPI_MD_FAULT_DETECT_DISABLE;  // ʹ��ģʽ���ϼ��
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
		SPI_ClearStatus(SPI_SAMPLE3_TOPO_UNIT, SPI_FLAG_OVERLOAD); //��������־����ֹ�д���ʱ���ܿ���SPI
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
		stcSpiInit.u32WireMode = SPI_4_WIRE; // ������
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
		// stcSpiInit.u32ModeFaultDetect = SPI_MD_FAULT_DETECT_DISABLE;  // ʹ��ģʽ���ϼ��
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
		SPI_ClearStatus(SPI_SAMPLE4_TOPO_UNIT, SPI_FLAG_OVERLOAD); //��������־����ֹ�д���ʱ���ܿ���SPI
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
		stcSpiInit.u32WireMode = SPI_4_WIRE; // ������
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
		// stcSpiInit.u32ModeFaultDetect = SPI_MD_FAULT_DETECT_DISABLE;  // ʹ��ģʽ���ϼ��
		// stcSpiInit.u32SuspendMode = SPI_COM_SUSP_FUNC_OFF;

		// stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_5SCK;

		SPI_Init(SPI_SAMPLE4_TOPO_UNIT, &stcSpiInit);
		// SPI_DelayTimeConfig(SPI_SAMPLE2_TOPO_UNIT, &stcSpiDelay);
		SPI_Cmd(SPI_SAMPLE4_TOPO_UNIT, ENABLE);
	}
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
	else //8306��������һ·SPIû��д����
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
//��������: ��ʼ��ADC
//
//����:		��
//
//����ֵ:  	��
//
//��ע: �����⼰��ص�ѹ���
//-----------------------------------------------
static void InitADC(void)
{
	stc_clock_xtal_init_t stcXtalCfg;
	stc_clock_pll_init_t stcUpllCfg;
	stc_adc_init_t stcAdcInit;
	uint8_t au8Adc1SaSampTime[3] = {0xff, 0xff};
	BYTE i;
	stc_gpio_init_t stc_port = {
        .u16PinState = PIN_STAT_RST,      /* PIN_STATE_SET �� PIN_STATE_RESET */
        .u16PinDir = PIN_DIR_IN,            /* PIN_DIR_IN �� PIN_DIR_OUT */
        .u16PinOutputType = PIN_OUT_TYPE_CMOS, /* PIN_OUT_TYPE_CMOS �� PIN_OUT_TYPE_NMOS */
        .u16PinDrv = PIN_MID_DRV,           /* PIN_LOW_DRV, PIN_MID_DRV, PIN_HIGH_DRV */
        .u16Latch = PIN_LATCH_OFF,          /* PIN_LATCH_ON �� PIN_LATCH_OFF */
        .u16PullUp = PIN_PU_OFF,            /* PIN_PU_ON �� PIN_PU_OFF */
        .u16Invert = PIN_INVT_OFF,          /* PIN_INVT_ON �� PIN_INVT_OFF */
        .u16ExtInt = PIN_EXTINT_OFF,        /* PIN_EXTINT_ON �� PIN_EXTINT_OFF */
        .u16PinInputType = PIN_IN_TYPE_CMOS, /* PIN_IN_TYPE_CMOS �� PIN_IN_TYPE_SCHMIT */
        .u16PinAttr = PIN_ATTR_ANALOG       /* PIN_ATTR_DIGITAL �� PIN_ATTR_ANALOG */
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
//��������: ADC����ֵתΪ��ѹֵ
//
//����: 	Data[in]:ADC����ֵ
//
//����ֵ: 	��ѹֵ
//
//��ע:
//-----------------------------------------------
static float AdcDataToVolData(WORD Data)
{
	return ((float)Data * 3.300 / 4096);
}
//-----------------------------------------------
//��������: ��ȡAD����
//
//����:	Type:AD����
//
//����ֵ:  	��
//
//��ע:eOther_AD_PWR��eOther_AD_BAT:��Դ����ص�ѹ���
//-----------------------------------------------
WORD api_GetADData(eType_ADC Type)
{
	//	WORD wData[3] = {0};
	//	if( Type >= eOther_AD_Max )
	//	{
	//		return 0;
	//	}
	//	//��ȡADֵ
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
	//��ȡADֵ
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
//��������: ��ȡ����SPIָ��
//
//����:
//
//����ֵ:  	Point
//
//��ע:
//-----------------------------------------------
// WORD api_GetSPIRcvPoint( void )
// {
// 	DWORD dwAddr;
// 	WORD Point;

// 	dwAddr = (*(volatile uint32_t *)((uint32_t)(&SPI_DMA_UNIT->MONDAR0) + ((SPI_RX_DMA_CH) * 0x40ul))); //����Ŀ���ַ�Ĵ���
// 	Point = (WORD)(dwAddr - (uint32_t)(&Serial_SPI.ProBuf[0]));

// 	return Point;
// }
//-----------------------------------------------
//��������:��ʼ��PWM���
//
//����: wPeriod[in]:����(us),wCompare[in]:�ߵ�ƽʱ��(us)
//
//����ֵ:  ��
//
//��ע: ��������ʶ���͸����źţ����������ʼ��������
//-----------------------------------------------
void api_TIMEA_2_PWM4_Init(WORD wPeriod, WORD wCompare)
{
}

//-----------------------------------------------
//��������: ����cpu Flash���ۼӺͣ�����word���ģʽ
//
//����: 	��
//
//����ֵ:  	�����word�ۼӺ�
//
//��ע:
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
//��������: ����cpu Flash���ۼӺͣ�����word���ģʽ
//
//����: 	��
//
//����ֵ:  	�����word�ۼӺ�
//
//��ע:  ־���㷨У��
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
//��������: ��ȡcpu��ram��Ƭ��flash����
//
//����: 	Type[in]	0--Ram 	1--flash
//          Addr[in]	��ַ
//			Len[in]		��ȡ����
//			Buf[out]	�������
//
//����ֵ:  	��ȡ���ݵĳ��ȣ����Ϊ0����ʾ��ȡʧ��
//
//��ע:   ROM:0~0x7FFFF;0x03000C00~0x03000FFB(δʹ��)
//        RAM��0x1fff'8000~0x1fff'ffff    (δʹ��)
//             0x20000000~0x2001FFFF
// 				0x20020000~0x20026FFF ��ջ
// 				0x200F0000~0x200F0FFF(δʹ��)
//  			δ�ж�add+len�ĺϷ���
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
//��������: ��������flash
//
//����: 	��
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
static void UpdateProg(void)
{
	BYTE i, ProgMessageNum, ProgData[PROGRAM_FLASH_SECTOR]; // һ������1024���ֽ�
	BYTE IntStatus;
	WORD j;
	DWORD ProgAddr;
	TUpdateProgMessage TmpUpdateProgMessage;
	TUpdateProgHead TmpUpdateProgHead;
	BYTE *pData;

	FunctionConst(UPDATEPROG_TASK);
	// ��ȡ����
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

		if (TmpUpdateProgMessage.ProgFrameLen) // �·�ʽ֡���Ȳ�Ϊ0
		{
			memcpy(&ProgData[TmpUpdateProgMessage.ProgOffset], &TmpUpdateProgMessage.ProgData[0], TmpUpdateProgMessage.ProgFrameLen);
		}
		else
		{
			memcpy(&ProgData[sizeof(TmpUpdateProgMessage.ProgData) * i], &TmpUpdateProgMessage.ProgData[0], sizeof(TmpUpdateProgMessage.ProgData));
		}
	}

	// ��������
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

		// ����д�ɹ� ����ѭ��
		if (j == PROGRAM_FLASH_SECTOR)
		{
			break;
			;
		}
	}

	// ����дʧ��
	if ((i == 2) && (j != PROGRAM_FLASH_SECTOR))
	{
		return;
	}

	// ��eeprom�еı���ͷ��Ϊ��Ч
	TmpUpdateProgHead.ProgAddr = 0x3fc01;
	TmpUpdateProgHead.ProgMessageNum = (PROGRAM_FLASH_SECTOR / (sizeof(TmpUpdateProgMessage) - sizeof(DWORD))) + 1;
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TmpUpdateProgHead), (BYTE *)&TmpUpdateProgHead);
}

//-----------------------------------------------
//��������: ��Լд����������Ϣͷ
//
//����: 	pBuf[in] ���뻺��
//
//����ֵ:  	��
//
//��ע: ��Լ����������ƽ̨��չ��Լ����¼C
//-----------------------------------------------
void api_WriteUpdateProgHead(BYTE *pBuf)
{
	TUpdateProgHead TmpUpdateProgHead;

	memcpy((BYTE *)&TmpUpdateProgHead.ProgAddr, pBuf, 4);
	TmpUpdateProgHead.ProgMessageNum = pBuf[4];

	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE *)&TmpUpdateProgHead);

	//��״̬λ��һ�γ���������������磬���������е�����Ҫ��������
	memset(ReceiveProgStatus, 0x00, sizeof(ReceiveProgStatus));
}

//-----------------------------------------------
//��������: ��Լд������������
//
//����: 	pBuf[in] ���뻺��
//
//����ֵ:  	TRUE/FALSE
//
//��ע: ��Լ����������ƽ̨��չ��Լ����¼C
//-----------------------------------------------
BOOL api_WriteUpdateProgMessage(BYTE *pBuf)
{
	TTwoByteUnion tw;
	TUpdateProgMessage TmpUpdateProgMessage;

	memcpy(tw.b, pBuf, 2); //֡���
	if (tw.w >= (PROGRAM_FLASH_SECTOR / sizeof(TmpUpdateProgMessage.ProgData)))
	{
		return FALSE;
	}

	// ��������ʽ��֡���ȡ�ƫ�Ƶ�ַ��Ϊ 0
	TmpUpdateProgMessage.ProgFrameLen = 0; // ֡����
	TmpUpdateProgMessage.ProgOffset = 0;   // ƫ�Ƶ�ַ

	// 128�ֽ����ݣ�������
	memcpy(TmpUpdateProgMessage.ProgData, pBuf + 2, sizeof(TmpUpdateProgMessage.ProgData));

	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32(TmpUpdateProgMessage.ProgData, sizeof(TmpUpdateProgMessage) - sizeof(DWORD));

	//���ܶԵ�֡��д��ֻ����ͷ��ʼ��д
	if ((ReceiveProgStatus[tw.w / 8] & (0x01 << (tw.w % 8))) == 0)
	{
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE *)&TmpUpdateProgMessage);
		ReceiveProgStatus[tw.w / 8] |= (0x01 << (tw.w % 8));
	}

	return TRUE;
}
//-----------------------------------------------
// ��������: ��Լд������������
//
// ����: 	pBuf[in] ���뻺��
//
// ����ֵ:  	TRUE/FALSE
//
// ��ע: ��Լ����������ƽ̨��չ��Լ����¼C
//-----------------------------------------------

BOOL api_WriteUpdateProgMessageNew(BYTE *pBuf)
{
	TTwoByteUnion tw;
	TTwoByteUnion len;
	TUpdateProgMessage TmpUpdateProgMessage;

	tw.w = 0;
	len.w = 0;

	memcpy(tw.b, pBuf, 2); // ֡���
	if (tw.w >= (PROGRAM_FLASH_SECTOR / (sizeof(TmpUpdateProgMessage.ProgData))))
	{
		return FALSE;
	}

	memcpy(len.b, pBuf + 2, 2); // ֡����
	if ((len.w > (sizeof(TmpUpdateProgMessage.ProgData))) || (len.w == 0))
	{
		return FALSE;
	}
	TmpUpdateProgMessage.ProgFrameLen = len.w; // ֡����

	memcpy((BYTE *)&TmpUpdateProgMessage.ProgOffset, pBuf + 4, 2); // ƫ�Ƶ�ַ
	if ((TmpUpdateProgMessage.ProgFrameLen + TmpUpdateProgMessage.ProgOffset) > PROGRAM_FLASH_SECTOR)
	{
		return FALSE;
	}

	memset(TmpUpdateProgMessage.ProgData, 0, sizeof(TmpUpdateProgMessage.ProgData));
	memcpy(TmpUpdateProgMessage.ProgData, pBuf + 6, len.w);

	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32((BYTE *)&TmpUpdateProgMessage, sizeof(TmpUpdateProgMessage) - sizeof(DWORD));

	// ���ܶԵ�֡��д��ֻ����ͷ��ʼ��д
	if ((ReceiveProgStatus[tw.w / 8] & (0x01 << (tw.w % 8))) == 0)
	{
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE *)&TmpUpdateProgMessage);
		ReceiveProgStatus[tw.w / 8] |= (0x01 << (tw.w % 8));
	}
	return TRUE;
}

//-----------------------------------------------
//��������: ������������
//
//����: 	pBuf[in] ���뻺��
//
//����ֵ:  	�˺�������ִ������ת�漰��λ
//
//��ע: ��Լ����������ƽ̨��չ��Լ����¼C
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

	//��һ�µ��籣�� !!!!!!
	api_PowerDownEnergy();

	ProgFlashPW = 0xa55a;

	UpdateProg();

	ProgFlashPW = 0;
	memset(ReceiveProgStatus, 0x00, sizeof(ReceiveProgStatus));

	//RESET
	//Reset_CPU();
}

/////////////////////////////////////////////////////////////////////
//�������ж���������
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// �����жϷ����ӳ���,�ķ������Ҫ���жϲ�����Ƕ��
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------
//��������: ���ڽ����жϺ���
//
//����: 	SCI_Ph_Num[in]	�����ں�
//          Data[in]		���յ�������
//
//����ֵ:  	��
//
//��ע:
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
				//������ܵ����ݵ��ڴ������ֵ ����Э���ﻹһ���ֽڶ�û���� ǿ�Ƶ���һ�� �������512�ֽ����ݵ���и��ʲ��ظ�������
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
//��������: ���ڷ����жϺ���
//
//����: 	SCI_Ph_Num[in]	�����ں�
//          USARTx[in]		��Ӧ�Ĵ���ָ��
//
//����ֵ:  	��
//
//��ע:
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

				USART_FuncCmd(USARTx, USART_INT_TX_EMPTY, DISABLE); // �رշ��Ϳ��ж�
				USART_FuncCmd(USARTx, USART_INT_TX_CPLT, ENABLE);   // �򿪷�������ж�
				Serial[i].SendToSendDelay = 100;
			}
			break;
		}
		
	}
}
#endif //#if ( CPU_TYPE == CPU_HC32F460 )
