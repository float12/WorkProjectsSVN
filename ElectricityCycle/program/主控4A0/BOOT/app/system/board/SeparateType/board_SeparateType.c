//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.30
//����		֧��698.45��Լ�ĵ����ӡ�ư������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

#if (BOARD_TYPE == BOARD_HC_MEASURINGSWITCH)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define GPIO_DEFAULT	0xFF
//��ƽ�ߵ�
#define HIGH			1
#define LOW				0

#define SYSTEM_POWER_VOL(vol)	((float)vol * 3.3 / 4096)

#define SYSTEM_VOL_MIN			0.86

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
enum 
{
    TRESET = 0U, 
    TSET = !TRESET
};

typedef struct
{
	MCU_IO IO;
	uint16_t DIR;
} SYS_IO_Config;


//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

// ֻ������Ҫ���õ�IO���������õ��������
const port_init_t TGPIO_Config[] =
	{
		//PORT			PIN    		FUNC      		PinState      Dir      		OutputType    	Drv    		Latch    	PullUp   Invert   ExtInt     InputType			Attr
		
		{.enPort = PortB, Pin00, Func40_Spi1_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	sample
		{.enPort = PortD, Pin01, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS1		sample
		{.enPort = PortD, Pin02, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS2		sample
		{.enPort = PortD, Pin03, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS3		sample
		{.enPort = PortD, Pin04, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS4		sample
		{.enPort = PortB, Pin01, Func41_Spi1_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	sample
		{.enPort = PortB, Pin02, Func42_Spi1_Miso, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 47.PB10		SPI_MISO	sample
		//hsdc1
		{.enPort = PortC, Pin04, Func43_Spi2_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI1
		{.enPort = PortC, Pin05, Func44_Spi2_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI1
		{.enPort = PortB, Pin12, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		HSPI1
		//hsdc2
		{.enPort = PortB, Pin04, Func46_Spi3_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	HSPI2
		{.enPort = PortB, Pin05, Func47_Spi3_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	HSPI2
		{.enPort = PortB, Pin15, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 45.		SPI_CS		HSPI2��test��
		//hsdc3
		{.enPort = PortC, Pin06, Func43_Spi4_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	HSPI3
		{.enPort = PortC, Pin07, Func44_Spi4_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	HSPI3
		{.enPort = PortD, Pin00, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 45.		SPI_CS		HSPI3
		//hsdc4
		{.enPort = PortA, Pin08, Func46_Spi6_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	HSPI4
		{.enPort = PortA, Pin09, Func47_Spi6_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	HSPI4
		{.enPort = PortA, Pin11, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 45.		SPI_CS		HSPI4
		
		{.enPort = PortE, Pin02, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		ESAM CS		
		{.enPort = PortC, Pin10, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		FLASH CS		

		//12���ⲿ����
		{.enPort = PortA, Pin01, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin02, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin03, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin04, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin05, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortE, Pin13, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortE, Pin14, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortB, Pin10, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		// {.enPort = PortD, Pin13, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		//��ʱ��Ϊ����ʱ������
		{.enPort = PortD, Pin13, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_OFF, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortD, Pin10, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortD, Pin09, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortD, Pin08, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����

		{.enPort = PortE, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample1��Դ����
		{.enPort = PortB, Pin09, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample2��Դ����
		{.enPort = PortB, Pin08, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample3��Դ����
		{.enPort = PortB, Pin06, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample4��Դ����
		{.enPort = PortB, Pin03, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		ESAM��Դ����
		{.enPort = PortA, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		FLASH��Դ����


		//4G
		{.enPort = PortE, Pin01, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		4G��Դ����

		{.enPort = PortB, Pin13, Func32_Usart1_Tx , HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		��4g����������
		{.enPort = PortB, Pin14, Func33_Usart1_Rx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3

		{.enPort = PortC, Pin02, Func33_Usart3_Rx, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		4g������
		{.enPort = PortC, Pin00, Func32_Usart3_Tx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3
		
		//485
		{.enPort = PortD, Pin05, Func33_Usart4_Rx, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		485
		{.enPort = PortD, Pin06, Func32_Usart4_Tx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3
		{.enPort = PortD, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},//3. T/R-1 RS485�������
		// 8025+EEPROM
		{.enPort = PortD, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 79.PC11		8025SCL   ��©���  ����̬
		{.enPort = PortD, Pin14, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 80.PC12		8025SDA   ��©���  ����̬

		{.enPort = PortE, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 79.PC11		EEPROM SCL   ��©���  ����̬
		{.enPort = PortE, Pin08, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 80.PC12		EEPROM SDA   ��©���  ����̬
		
		{.enPort = PortA, Pin06, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 97.PE0  		E2-WC1	��1ƬEEPROMд����	����Ч
		{.enPort = PortA, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},  // 98.PE1  		E2-WC2 	��2ƬEEPROMд����	����Ч
};

//SPI��λ�ܽ�����
const port_init_t TSPISetOI_Config[eCOMPONENT_TOTAL_NUM][4] =
{
	{
        {.enPort = PortB, Pin02, Func42_Spi1_Miso, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 47.		SPI_MISO	
        {.enPort = PortB, Pin01, Func41_Spi1_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	
        {.enPort = PortB, Pin00, Func40_Spi1_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	
        {.enPort = PortC, Pin10, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.		SPI_CS		
	}
};
//SPI��λ�ܽ����� �ӵ�Դ���ƣ������ϵ��ʼ����Դ���Ʋ���
const port_init_t TSPIResetOI_Config[eCOMPONENT_TOTAL_NUM][5] =
{
	//eCOMPONENT_SPI_FLASH
	{
		{.enPort = PortB, Pin02, Func0_Gpio, LOW, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 47.PB10		SPI_MISO	
		{.enPort = PortB, Pin01, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	
		{.enPort = PortB, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	
		{.enPort = PortC, Pin10, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		
		{.enPort = PortA, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		��Դ����
	}
};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static volatile WORD SysTickCounter;
static volatile WORD SysTickCounter1;//����������Ϣ����ʹ��
BYTE CommLed_Time = 0; //ͨ�ŵ���ʱ��
//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ϵͳ��Դ���
//
//����: 	
//          Type[in]:	
//				eOnInitDetectPowerMode		�ϵ�ϵͳ��Դ���
//				eOnRunDetectPowerMode		���������ڼ��ϵͳ��Դ���
//				eWakeupDetectPowerMode		�͹��Ļ����ڼ�ϵͳ��Դ���
//				eSleepDetectPowerMode		�͹��������ڼ�ϵͳ��Դ���
//����ֵ: 	TRUE:�е�   FALSE:û�е�
//
//��ע:   �ϵ�vcc�ͱȽ������У�����ֻ��LVDIN0�Ƚ�������Ϊvcc���г�������
//-----------------------------------------------
BOOL api_CheckSysVol( eDETECT_POWER_MODE Type )
{
	WORD wSystemVol = 0;
	//���޵�ѹ���
	return TRUE;
	// api_GetADData(&wSystemVol);

	// if(SYSTEM_POWER_VOL(wSystemVol) > SYSTEM_VOL_MIN)
	// {
	// 	return TRUE;
	// }
	// else
	// {
	// 	return FALSE;
	// }
	
}


//-----------------------------------------------
//��������: CPU�ܽų�ʼ����ͳһ����
//
//����: 	
//          Type[in]:	ePowerOnMode	�����ϵ���еĹܽų�ʼ��
//						ePowerDownMode	�͹��Ļ��ѽ��еĹܽų�ʼ��
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void InitPort(ePOWER_MODE Type)
{
	BYTE i;
	
	if(Type == ePowerOnMode)
	{
		GPIO_SetDebugPort(GPIO_PIN_TDO, DISABLE);
		GPIO_SetDebugPort(GPIO_PIN_TRST, DISABLE);
		GPIO_SetDebugPort(GPIO_PIN_TDI, DISABLE);

		for(i = 0; i < (sizeof(TGPIO_Config) / sizeof(port_init_t)); i++)
		{
			if(TGPIO_Config[i].FuncReg != GPIO_DEFAULT)
			{
				GPIO_Init(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin,&TGPIO_Config[i].GpioReg);
				if(TGPIO_Config[i].GpioReg.u16PinDir == PIN_DIR_OUT)	//�������״̬
				{
					if(TGPIO_Config[i].GpioReg.u16PinState == HIGH)
					{
						GPIO_SetPins(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin);
					}
					else if(TGPIO_Config[i].GpioReg.u16PinState == LOW)
					{
						GPIO_ResetPins(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin);
					}
					else
					{
						
					}
				}	
				if(TGPIO_Config[i].FuncReg != Func0_Gpio)
				{
					GPIO_SetFunc(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin,(en_port_func_t)TGPIO_Config[i].FuncReg);
				}
			}
		}
	}
}

//--------------------------------------------------
//��������:  �뵥�ຯ����Ӧ��������SPI���ã��޷��ر�esam��Դ���պ���
//         
//����  : ��
//
//����ֵ:    
//         
//��ע����:  �պ��� ������
//--------------------------------------------------
void ESAMSPIPowerDown( ePOWER_MODE PowerMode )
{

}
//--------------------------------------------------
//��������:  ��SPI�¹��ص��豸���и�λ
//
//����:     eCOMPONENT_TYPE  Type[in]  eCOMPONENT_SPI_SAMPLE/eCOMPONENT_SPI_FLASH/eCOMPONENT_SPI_LCD/eCOMPONENT_SPI_ESAM/eCOMPONENT_SPI_CPU
//         BYTE  Time[in]  ��ʱʱ�䣨MS��
//
//����ֵ:    BOOL  TRUE: �ɹ� FALSE: ʧ��
//
//��ע:	  Flash������оƬ ����SPI����
//--------------------------------------------------
BOOL ResetSPIDevice( eCOMPONENT_TYPE Type, BYTE Time )
{
	BYTE i;

	if( Type >= eCOMPONENT_TOTAL_NUM )
	{
		return FALSE;
	}
	
	for (i = 0; i < (sizeof(TSPIResetOI_Config[Type]) / sizeof(port_init_t)); i++)
	{
		if (TSPIResetOI_Config[Type][i].FuncReg != GPIO_DEFAULT)
		{
			GPIO_SetFunc(TSPIResetOI_Config[Type][i].enPort, TSPIResetOI_Config[Type][i].u16Pin, (en_port_func_t)TSPIResetOI_Config[Type][i].FuncReg);
			GPIO_Init(TSPIResetOI_Config[Type][i].enPort, TSPIResetOI_Config[Type][i].u16Pin, &TSPIResetOI_Config[Type][i].GpioReg);
			if (TSPIResetOI_Config[Type][i].GpioReg.u16PinDir == PIN_DIR_OUT) //�������״̬
			{
				if (TSPIResetOI_Config[Type][i].GpioReg.u16PinState == HIGH)
				{
					GPIO_SetPins(TSPIResetOI_Config[Type][i].enPort, TSPIResetOI_Config[Type][i].u16Pin);
				}
				else if (TSPIResetOI_Config[Type][i].GpioReg.u16PinState == LOW)
				{
					GPIO_ResetPins(TSPIResetOI_Config[Type][i].enPort, TSPIResetOI_Config[Type][i].u16Pin);
				}
				else
				{
				}
			}
		}
	}

	if( Type == eCOMPONENT_SPI_FLASH ) //pe14 �ر�flash CS
	{

		//FLASH_CS_DISABLE;
		POWER_FLASH_CLOSE;//���´򿪵�Դ
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		api_Delayms(Time);
		//FLASH_CS_ENABLE;
	}

	for (i = 0; i < (sizeof(TSPISetOI_Config[Type]) / sizeof(port_init_t)); i++)
	{
		if (TSPISetOI_Config[Type][i].FuncReg != GPIO_DEFAULT)
		{
			GPIO_SetFunc(TSPISetOI_Config[Type][i].enPort, TSPISetOI_Config[Type][i].u16Pin, (en_port_func_t)TSPISetOI_Config[Type][i].FuncReg);
			GPIO_Init(TSPISetOI_Config[Type][i].enPort, TSPISetOI_Config[Type][i].u16Pin, &TSPISetOI_Config[Type][i].GpioReg);
			if (TSPISetOI_Config[Type][i].GpioReg.u16PinDir == PIN_DIR_OUT) //�������״̬
			{
				if (TSPISetOI_Config[Type][i].GpioReg.u16PinState == HIGH)
				{
					GPIO_SetPins(TSPISetOI_Config[Type][i].enPort, TSPISetOI_Config[Type][i].u16Pin);
				}
				else if (TSPISetOI_Config[Type][i].GpioReg.u16PinState == LOW)
				{
					GPIO_ResetPins(TSPISetOI_Config[Type][i].enPort, TSPISetOI_Config[Type][i].u16Pin);
				}
				else
				{
				}
			}
		}
	}

	if(Type == eCOMPONENT_SPI_FLASH)
	{
		api_InitSPI(eCOMPONENT_SPI_FLASH, eSPI_MODE_3);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
	
}
//-----------------------------------------------
//��������: �����ʼ������
//
//����:		��
//                    
//����ֵ: ��
//		   
//��ע:   
//-----------------------------------------------
void InitBoard( void )
{
	CLEAR_WATCH_DOG;
    
    InitPort( ePowerOnMode );//������ʼ��
    
	ResetSPIDevice(eCOMPONENT_SPI_FLASH, 5); //��flash spi���и�λ
    
    

}


//-----------------------------------------------
//��������: SPIƬѡ��������
//
//����:		
//		No[in]		��Ҫѡ���оƬ
// 			CS_SPI_FLASH    - FLASH
// 			CS_SPI_SAMPLE   - ����оƬ
// 			CS_SPI_ESAM     - ESAM
//      Do[in]
// 			TRUE��ʹ��Ƭѡ
// 			FALSE����ֹƬѡ
//
//����ֵ: ��
//		   
//��ע: ʹ�ò��ǹ���SPIʱ������Ҫ�ر�������SPI_CS 
//-----------------------------------------------
void DoSPICS(WORD No, WORD Do)
{
	if( Do == FALSE )
	{
		switch( No )
		{
			case CS_SPI_ESAM:
				ESAM_CS_DISABLE;
				break;
			case CS_SPI_FLASH:
				FLASH_CS_DISABLE;
				break;
			case CS_SPI_SAMPLE:
				SAMPLE_CS_DISABLE;
				break;
			default:
				break;
		}
		return;
	}
	
	switch( No )
	{
		case CS_SPI_ESAM:
			ESAM_CS_ENABLE;
			break;
		case CS_SPI_FLASH:
			FLASH_CS_ENABLE;
			break;
		case CS_SPI_SAMPLE:
			SAMPLE_CS_ENABLE;
			break;
		default:
			break;
	}
	
	api_Delay10us(1);
	
	return;
}


///////////////////////////////////////////////////////////////////////
//�жϷ������
////////////////////////////////////////////////////////////////////////
//-----------------------------------------------
//��������: systick�жϷ������10ms�ж�һ��
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void SysTick_Handler(void)
{
 
}


#endif //#if ( BOARD_HC_MEASURINGSWITCH)
