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
#include "hc32f460_dmac.h"
#include "bsp_cpu_flash.h"

#if ( CPU_TYPE == CPU_HC32F460 )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
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

// DMA ����ADC
#define ADC1_SA_DMA_UNIT            	(M4_DMA1)
#define ADC1_SA_DMA_CH              	(DmaCh2)
#define ADC1_SA_DMA_PWC             	(PWC_FCG0_PERIPH_DMA1)
#define ADC1_SA_DMA_TRGSRC          	(EVT_ADC1_EOCA)
// DMA ����ADC
#define ADC1_SB_DMA_UNIT            	(M4_DMA1)
#define ADC1_SB_DMA_CH              	(DmaCh3)
// DMA SPI
#define SPI_DMA_UNIT            		(M4_DMA2)
#define SPI_TX_DMA_CH             		(DmaCh1)
#define SPI_RX_DMA_CH              		(DmaCh0)
#define SPI_DMA_DMA_PWC             	(PWC_FCG0_PERIPH_DMA2)
//DMA 8306����������
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
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//SPI ���ܹܽŶ�Ӧ��ϵ
const MCU_IO Spi_IO[][3] =
{
	{
		{ PortE, Pin01, Func_Spi4_Sck},			//CLK
		{ PortE, Pin00, Func_Spi4_Mosi},		//Mosi
		{ PortB, Pin09, Func_Spi4_Miso},		//Miso  ������FALSH
	},
	
	{
		{ PortE, Pin01, Func_Spi4_Sck},			//CLK
		{ PortE, Pin00, Func_Spi4_Mosi},		//Mosi
		{ PortB, Pin09, Func_Spi4_Miso},		//Miso  ������FALSH
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
		{ PortC, Pin04, Func_Spi1_Mosi}			//Mosi �ӻ�ģʽ ����
		//û��Miso
	}
};


//ADC ���ܹܽŶ�Ӧ��ϵ
const MCU_IO Adc_IO[] =
{
	{ PortC, Pin00, Func_Gpio},			//�ϵ��ѹ���//��װ��PA2��ΪPC0
	{ PortB, Pin01, Func_Gpio},			//��ص�ѹ���
	// { PortA, Pin05, Func_Gpio},			//������Ȧ��ѹ���
};
//const Fun FunctionConst = api_DelayNop;
BYTE	ReceiveProgStatus[8];// 128�ֽ�һ֡��һ������8K����Ҫ8*8��bit
WORD 	ProgFlashPW;

BYTE SpiTxBuffer[6] = {0x55,0x55,0x55,0x55,0x55,0x55};
BYTE bSPIExIntFlag=0;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void InitWatchDog(void);
static void InitADC(void);
static void SPI4_IRQ_Send(void);
static void SPI4_IRQ_Rcv(void);
static void TIMEA_1_PWM8_Init( void );
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
//��ע:    48M��Ƶʱʵ��Լ��ʱ1.01ms��72M��Ƶʱʵ��Լ��ʱ1ms��96M��Ƶʱʵ��Լ��ʱ1ms��
//		ע�⣺��ʱԽ������ʱʱ���������;��������벻���������ʱ������ !!!!!!
//		ms=500ʱ��48M��Ƶ��ʵ��Լ591ms;72M��Ƶ��ʵ��Լ557ms;96M��Ƶ��ʵ��Լ542ms;
//-----------------------------------------------
void api_Delayms(WORD ms)
{
	DWORD TargetValue,CurrentValue;
	DWORD LastValue,TempValue;  
	DWORD CpuClockMHz;
	DWORD SysTickLoad;
	
	CpuClockMHz = SystemCoreClock / 1000;	//���ڵδ���10ms�ж�
	TargetValue  = ms * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;
	
	do
	{
		TempValue = SysTick->VAL;
		if( LastValue >= TempValue ) //Systick�����¼�����
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick��װ��ֵ�Ǽ�1��
		}
		LastValue = TempValue;
	}while( CurrentValue < TargetValue );
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
	DWORD TargetValue,CurrentValue;
	DWORD LastValue,TempValue;  
	DWORD CpuClockMHz;
	DWORD SysTickLoad;
	
	CpuClockMHz = SystemCoreClock / 10000;	//���ڵδ���10ms�ж�
	TargetValue  = us * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;
	
	do
	{
		TempValue = SysTick->VAL;
		if( LastValue >= TempValue ) //Systick�����¼�����
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick��װ��ֵ�Ǽ�1��
		}
		LastValue = TempValue;
	}while( CurrentValue < TargetValue );
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
	DWORD TargetValue,CurrentValue;
	DWORD LastValue,TempValue;  
	DWORD CpuClockMHz;
	DWORD SysTickLoad;
	
	CpuClockMHz = SystemCoreClock / 100000;	//���ڵδ���10ms�ж�
	TargetValue  = us * CpuClockMHz;

	SysTickLoad = SysTick->LOAD + 1;
	LastValue = SysTick->VAL;
	CurrentValue = 0;
	
	do
	{
		TempValue = SysTick->VAL;
		if( LastValue >= TempValue ) //Systick�����¼�����
		{
			CurrentValue += (LastValue - TempValue);
		}
		else
		{
			CurrentValue += (LastValue - TempValue + SysTickLoad); //Systick��װ��ֵ�Ǽ�1��
		}
		LastValue = TempValue;
	}while( CurrentValue < TargetValue );
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
#pragma optimize = size low//�м��Ż�������£����a_byDefaultData�Ż���
//�̶�λ�÷���̱�������
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
		
	#if(SPD_HDSC_MCU == SPD_48000K)
		stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;    // ���200M
		stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;   // ���100M
		stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // ���200M	48M
		stcSysClkCfg.enPclk1Div = ClkSysclkDiv4; // ���100M    12M
		stcSysClkCfg.enPclk2Div = ClkSysclkDiv1;  // ���60M	12M
		stcSysClkCfg.enPclk3Div = ClkSysclkDiv4; // ���50M     12M
		stcSysClkCfg.enPclk4Div = ClkSysclkDiv1;  // ���100M	48M
	#elif(SPD_HDSC_MCU == SPD_72000K)
		stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;    // ���200M
		stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;   // ���100M
		stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // ���200M	72M
		stcSysClkCfg.enPclk1Div = ClkSysclkDiv4; // ���100M    18M
		stcSysClkCfg.enPclk2Div = ClkSysclkDiv1;  // ���60M	18M
		stcSysClkCfg.enPclk3Div = ClkSysclkDiv4; // ���50M     18M
		stcSysClkCfg.enPclk4Div = ClkSysclkDiv1;  // ���100M	72M
	#elif(SPD_HDSC_MCU == SPD_96000K)
		stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;    // ���200M
		stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;   // ���100M
		stcSysClkCfg.enPclk0Div = ClkSysclkDiv2;  // ���200M	48M
		stcSysClkCfg.enPclk1Div = ClkSysclkDiv1; // ���100M    96M
		stcSysClkCfg.enPclk2Div = ClkSysclkDiv2;  // ���60M	48M
		stcSysClkCfg.enPclk3Div = ClkSysclkDiv8; // ���50M     12M
		stcSysClkCfg.enPclk4Div = ClkSysclkDiv2;  // ���100M	48M
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
	SRAM_Init(&stcSramConfig);// 2��CPU���ڣ���ֹECC

	/* flash read wait cycle setting */
	EFM_Unlock();
	#if(SPD_HDSC_MCU == SPD_48000K)
	EFM_SetLatency(EFM_LATENCY_1);
	#elif((SPD_HDSC_MCU == SPD_72000K) ||(SPD_HDSC_MCU == SPD_96000K))
	EFM_SetLatency(EFM_LATENCY_2);
	#else
	���������⣬����������
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

    if(Type == 0)
    {
        //�ж�FPara2��У��
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
                  	BpsBak = CommParaConst.I485;//9600 1ֹͣλ żУ��
                }
            }
            else if(SerialMap[i].SerialType == eIR)//����
            {
            	BpsBak = 0x42;//����ڹ̶�1200
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
            //       	BpsBak = CommParaConst.CanBaud;//Ĭ��125KBPS
            //     }
			// }  
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
void DealSciFlag( BYTE Channel )
{
    //�иĲ��ز��������Ӧ�����ٸĲ�����
    if( Serial[Channel].OperationFlag & BIT0 )
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
void api_InitSci( BYTE Channel )
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
//��������: ����ָ���ӿڽ���
//
//����: 
//			p[in]		��Ҫ�����ڵ�ָ��
//                    
//����ֵ:  	��
//
//��ע:   
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
//��������: �㲥������Ӧ�������´��ڻָ�����
//
//����: 
//			Channel[in]		�߼����ں� 0����һ485  1������  2��ģ��
//                    
//����ֵ:  	��
//
//��ע:   
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
	//api_Delay100us(10);

	FunctionConst(INIT_BASE_TIMER);
	//��ʼ��ϵͳʱ��
	api_MCU_SysClockInit( ePowerOnMode );
	//��ʼ�����Ź�
	InitWatchDog();
	CLEAR_WATCH_DOG;
	//ϵͳ��ʱ����ʼ����Tick��ʱ����
	api_InitSysTickTimer( ePowerOnMode );
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

	// ����PCLK3  12M����
	// 16384/(12*1000*1000 /2048) = 2.796s
	stcWdtInit.enClkDiv = WdtPclk3Div2048;
	stcWdtInit.enCountCycle = WdtCountCycle16384;
	stcWdtInit.enRefreshRange = WdtRefresh100Pct;
	stcWdtInit.enSleepModeCountEn = Disable;
	stcWdtInit.enRequestType = WdtTriggerResetRequest;
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
void api_MCU_RTCInt( void )
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
		SPI_ClearFlag(SPI_SAMPLE_UNIT,SpiFlagOverloadError); //��������־����ֹ�д���ʱ���ܿ���SPI
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

		SPI_Init(SPI_SAMPLE_UNIT, &stcSpiInit);
		SPI_Cmd(SPI_SAMPLE_UNIT, Enable);
	}
//	else if (Component == eCOMPONENT_SPI_ESAM) //ESAM
//	{
//		//���Ӽ��ӳ�ʼ��ʱ�ȹر�SPI
//		//SPI_Cmd(SPI_SAMPLE_DATA_UNIT, Disable);
//		/* configuration structure initialization */
//		MEM_ZERO_STRUCT(stcSpiInit);
//
//		/* Configuration peripheral clock */
//		PWC_Fcg1PeriphClockCmd(SPI_ESAM_CLOCK, Enable);
//		SPI_ClearFlag(SPI_ESAM_UNIT,SpiFlagOverloadError); //��������־����ֹ�д���ʱ���ܿ���SPI
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
//		stcSpiInit.enWorkMode = SpiWorkMode3Line;				// ������
//		stcSpiInit.enTransMode = SpiTransFullDuplex;
//		stcSpiInit.enCommAutoSuspendEn = Disable;
//		stcSpiInit.enModeFaultErrorDetectEn = Disable;
//		stcSpiInit.enParitySelfDetectEn = Disable;
//		stcSpiInit.enParityEn = Disable;
//		stcSpiInit.enParity = SpiParityEven;
//		stcSpiInit.enMasterSlaveMode = SpiModeMaster; 
//		// ������ ������Ч
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
		SPI_ClearFlag(SPI_FLASH_UNIT,SpiFlagOverloadError); //��������־����ֹ�д���ʱ���ܿ���SPI
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
	else if (Component == eCOMPONENT_SPI_HSDC)
	{
		/* configuration structure initialization */
		MEM_ZERO_STRUCT(stcSpiInit);
		/* Configuration peripheral clock */
		PWC_Fcg1PeriphClockCmd(SPI_SAMPLETOPO_CLOCK, Enable);
		SPI_ClearFlag(SPI_SAMPLETOPO_UNIT,SpiFlagOverloadError); //��������־����ֹ�д���ʱ���ܿ���SPI
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
		stcSpiInit.enMasterSlaveMode = SpiModeSlave;			//�ӻ�ģʽ
		stcSpiInit.stcSsConfig.enSsValidBit = SpiSsValidChannel0;
    	stcSpiInit.stcSsConfig.enSs0Polarity = SpiSsLowValid;
		// ������ ������Ч
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
		pSpi = SPI_SAMPLE_UNIT;
	}
//	else if( Component == eCOMPONENT_SPI_ESAM )
//	{
//		pSpi = SPI_ESAM_UNIT;
//	}
	else//8306��������һ·SPIû��д����
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
//��������: ����������DMA��ʼ��
//
//����: 
//                    
//����ֵ:
//
//��ע:   
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
//��������: ����������DMAʹ��
//
//����: 
//                    
//����ֵ:
//
//��ע:   
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
    CLK_SetPeriClkSource(ClkPeriSrcUpllr);//ѡ��UPLLR�����ʱ��Ƶ��40M

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
    stcChCfg.u32Channel  = 0x00000600;// 9,10ͨ��               0x00000224;	//��9,5,2ͨ�� 
    stcChCfg.u8Sequence  = ADC_SEQ_A;
    stcChCfg.pu8SampTime = au8Adc1SaSampTime;	//��������255������
    /* 2. Add ADC channel. */
    ADC_AddAdcChannel(M4_ADC1, &stcChCfg);
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
static float AdcDataToVolData( WORD Data )
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
WORD api_GetADData( eType_ADC Type )
{
	WORD wData[3] = {0};
	if( Type >= eOther_AD_Max )
	{
		return 0;
	}
	//��ȡADֵ
	ADC_PollingSa(M4_ADC1,&wData[0],3,1);

	return wData[(BYTE)Type];
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
//��������: ����cpu Flash���ۼӺͣ�����word���ģʽ
//
//����: 	��
//                    
//����ֵ:  	�����word�ۼӺ�
//
//��ע:  ־���㷨У��
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

	memcpy((BYTE*)&TmpUpdateProgHead.ProgAddr,pBuf,4);
	TmpUpdateProgHead.ProgMessageNum = pBuf[4];
	
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE*)&TmpUpdateProgHead );	
	
	//��״̬λ��һ�γ���������������磬���������е�����Ҫ��������
	memset( ReceiveProgStatus, 0x00, sizeof(ReceiveProgStatus));
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
	
	memcpy(tw.b,pBuf,2);	//֡���
	if (tw.w >= (PROGRAM_FLASH_SECTOR / sizeof(TmpUpdateProgMessage.ProgData)))
	{
		return FALSE;
	}

	// ��������ʽ��֡���ȡ�ƫ�Ƶ�ַ��Ϊ 0
	TmpUpdateProgMessage.ProgFrameLen = 0; // ֡����
	TmpUpdateProgMessage.ProgOffset = 0;   // ƫ�Ƶ�ַ

	// 128�ֽ����ݣ�������
	memcpy(TmpUpdateProgMessage.ProgData, pBuf + 2, sizeof(TmpUpdateProgMessage.ProgData));

	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32( TmpUpdateProgMessage.ProgData, sizeof(TmpUpdateProgMessage)-sizeof(DWORD) );
	
	//���ܶԵ�֡��д��ֻ����ͷ��ʼ��д
	if ((ReceiveProgStatus[tw.w/8] & (0x01<<(tw.w%8))) == 0)
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE*)&TmpUpdateProgMessage );
		ReceiveProgStatus[tw.w/8] |= (0x01<<(tw.w%8));
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
	if ((ReceiveProgStatus[tw.w/8] & (0x01<<(tw.w%8))) == 0)
	{
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE *)&TmpUpdateProgMessage);
		ReceiveProgStatus[tw.w/8] |= (0x01<<(tw.w%8));
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
				//������ܵ����ݵ��ڴ������ֵ ����Э���ﻹһ���ֽڶ�û���� ǿ�Ƶ���һ�� �������512�ֽ����ݵ���и��ʲ��ظ�������
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
//��������: ���ڷ����жϺ���
//
//����: 	SCI_Ph_Num[in]	�����ں�
//          USARTx[in]		��Ӧ�Ĵ���ָ��
//
//����ֵ:  	��
//
//��ע:   
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
				USART_FuncCmd(USARTx, UsartTxEmptyInt, Disable);// �رշ��Ϳ��ж�
				USART_FuncCmd(USARTx, UsartTxCmpltInt, Enable);// �򿪷�������ж�
				Serial[i].SendToSendDelay = 100;
			}
			break;
		}
	}
}
//-----------------------------------------------
//��������: SPI-CS�жϺ���
//
//����: 	
//
//����ֵ:  	��
//
//��ע:   
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
