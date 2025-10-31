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

#define SYSTEM_VOL_MIN_DOWN				1.77		//��Դ�ж�����(����12V��Դ��ѹ:12V*10/(51+10)=1.967V)
#define SYSTEM_VOL_MIN_UP				1.85		//��Դ�ж�����(����12V��Դ��ѹ:12V*10/(51+10)=1.967V)

#define POWER_CHECK_LOOP_COUNT			10			// 10�δ�ѭ����һ��
#define POWER_CHECK_CONTINUE_COUNT		5			// ����������
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
	en_port_t Port;
	en_pin_t Pin;
	en_port_func_t IoFunc_x;
} MCU_IO;

typedef struct
{
	MCU_IO IO;
	en_pin_mode_t MODE;
} SYS_IO_Config;


//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//ֻ������Ҫ���õ�IO���������õ��������
const port_init_t TGPIO_Config[] =
	{
		// PORT		      PIN    FUNC      Mode           LTE      INTE    INVE      PUU     DRV         NOD             BFE      Level
		{.enPort = PortE, Pin02, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 1. PE02		¼����ʼֹͣ��ⰴť������Ϊ�͵�ƽ��ʼת������
		{.enPort = PortE, Pin03, Func_Spi3_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 2. PE03		SPI_MOSI  ��ESAMͨ�� ��
		{.enPort = PortE, Pin04, Func_Spi3_Miso, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 3. PE04		SPI_MISO  ��ESAMͨ�� ��
		{.enPort = PortE, Pin05, Func_Spi3_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},  // 4. PE05		SPI_SCLK  ��ESAMͨ�� ��
		{.enPort = PortE, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 5. PE06		SPI_CS     ��ESAMͨ�� ��
		{.enPort = PortA, Pin00, Func_Usart1_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 6. PA00		USART_1_RX ����
		{.enPort = PortA, Pin01, Func_Usart1_Tx, Pin_Mode_Out, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},  // 7. PA01		USART_1_TX ����
		// {.enPort = PortA, Pin06, Func_Tima1, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, HIGH},		  // 8. PA06		����PWM���
		{.enPort = PortC, Pin00, Func_Gpio, Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 9. PC00		��Դ���//��װ��������Ŵ�pa2��Ϊpc0!!!!!!
		{.enPort = PortB, Pin01, Func_Gpio, Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 10.PB01		ʱ�ӵ�ص�ѹ���
		{.enPort = PortA, Pin04, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},				  // 11.PA04		�๦�����
		{.enPort = PortB, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 12.PB06		CV8306
		{.enPort = PortB, Pin07, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 13.PB07		CVFLASH
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 14.PB08		SPI_CS		��8306ͨ��  ��
		{.enPort = PortH, Pin02, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 15.PH02		SPI_CS		��FLASHͨ��  ��
		{.enPort = PortB, Pin09, Func_Spi4_Miso, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 16.PB09		SPI_MISO	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin00, Func_Spi4_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 17.PE00		SPI_MOSI	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin01, Func_Spi4_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},  // 18.PE01		SPI_SCLK	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin11, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 19.PE11		ң��1״̬
		{.enPort = PortE, Pin12, Func_Usart3_Tx, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 20.PE12		USART_3_TX 485
		{.enPort = PortE, Pin13, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},				  // 21.PE13		485�������
		{.enPort = PortE, Pin14, Func_Usart3_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 22.PE14		USART_3_RX 485
		{.enPort = PortE, Pin15, Func_Can1_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 23.PE15		CAN-RX  CAN����
		{.enPort = PortB, Pin10, Func_Can1_Tx, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	  // 24.PB10		CAN-TX  CAN����
		{.enPort = PortB, Pin14, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 25.PB14		SDA   ��©���  ����̬(�����EE)
		{.enPort = PortD, Pin10, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 26.PD10		SCL   ��©���  ����̬(�����EE)
		{.enPort = PortD, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 27.PD08		E2-WC2   ������� eppд����������Ч
		{.enPort = PortD, Pin09, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 28.PD09		E2-WC1   ������� eppд����������Ч
		{.enPort = PortB, Pin12, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 29.PB12		SDA   ��©���  ����̬(�����RTC)
		{.enPort = PortB, Pin13, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 30.PB13		SCL   ��©���  ����̬(�����RTC)
		{.enPort = PortA, Pin15, Func_Usart2_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 31.PA15		USART_2_RX ͨѶģ��
		{.enPort = PortC, Pin10, Func_Usart2_Tx, Pin_Mode_Out, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 32.PC10		USART_2_TX ͨѶģ��
		{.enPort = PortC, Pin11, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, HIGH},				  // 33.PC11		MDM_SET ��©��� ��ʼΪ��
		{.enPort = PortC, Pin12, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 34.PC12		MDM_STA ��������
		{.enPort = PortD, Pin00, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, LOW},				  // 35.PD00		MDM_EVE ��©��� ��ʼΪ��
		{.enPort = PortD, Pin01, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, HIGH},				  // 36.PD01		MDM_RST ��©��� ��ʼΪ��
		{.enPort = PortD, Pin02, Func_Tima0, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, HIGH},			  // 37.PD02		������������PWM�����źš���������MOS�ܵ�ͨ
		{.enPort = PortD, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 38.PD03		������������ʹ�ܡ��ߵ�ƽʱ��������MOS
		{.enPort = PortD, Pin05, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 39.PD05		���ǵ͵�ƽ���ոǸߵ�ƽ
		{.enPort = PortB, Pin04, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 40.PB04		ͨ��ָʾ��  ������� ����Ч
		{.enPort = PortD, Pin07, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 41.PD07		TF��ת��ָʾ��  ������� ����Ч
		{.enPort = PortC, Pin05, Func_Gpio, Pin_Mode_In, Disable, Enable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 42.PC05		SPI_CS	8306�����460	�ֲ��е�HSDC
		{.enPort = PortC, Pin04, Func_Spi1_Mosi, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 43.PC04		SPI_MOSI  8306�����460
		{.enPort = PortA, Pin07, Func_Spi1_Sck, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	  // 44.PA07		SPI_SCLK  8306�����460
		{.enPort = PortB, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 45.PB03		���еƣ������л�һ��
        {.enPort = PortA, Pin05, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},	              //46.PA05               SD����������  ������Ч
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
	static BYTE bLoopCount = 0;
	WORD wSystemVol = 0;
	BYTE bResult = 0, i = 0;

	if (Type == eOnInitDetectPowerMode)
	{
		for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
		{
			wSystemVol = api_GetADData(eOther_AD_PWR);

			if (SYSTEM_POWER_VOL(wSystemVol) < SYSTEM_VOL_MIN_UP) // ϵͳ��Դ�ж��Ƿ��ϵ�
			{
				break;
			}
		}

		// �ϵ��⣬POWER_CHECK_CONTINUE_COUNT�ζ��е����Ϊ�е�
		if (i >= POWER_CHECK_CONTINUE_COUNT)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
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
		PORT_DebugPortSetting(TDO_SWO, Disable);
		PORT_DebugPortSetting(TRST, Disable);
		PORT_DebugPortSetting(TDI, Disable);

		for(i = 0; i < (sizeof(TGPIO_Config) / sizeof(port_init_t)); i++)
		{
			if(TGPIO_Config[i].FuncReg != GPIO_DEFAULT)
			{
				PORT_Init(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin,&TGPIO_Config[i].GpioReg);
				if(TGPIO_Config[i].GpioReg.enPinMode == Pin_Mode_Out)	//�������״̬
				{
					if(TGPIO_Config[i].bLevel == HIGH)
					{
						PORT_SetPortData(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin);
					}
					else if(TGPIO_Config[i].bLevel == LOW)
					{
						PORT_ResetPortData(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin);
					}
					else
					{
						
					}
				}
				if(TGPIO_Config[i].FuncReg != Func_Gpio)
				{
					PORT_SetFunc(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin,(en_port_func_t)TGPIO_Config[i].FuncReg,Disable);
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
	if (Type == eCOMPONENT_SPI_FLASH)
	{
		FLASH_CS_DISABLE;
		POWER_FLASH_CLOSE; // ���´򿪵�Դ
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		api_Delayms(Time);
		FLASH_CS_ENABLE;

		api_InitSPI(eCOMPONENT_SPI_FLASH, eSPI_MODE_3);
	}
	else if (Type == eCOMPONENT_SPI_SAMPLE)
	{
		api_InitSPI(eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1);
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

	ResetSPIDevice(eCOMPONENT_SPI_FLASH, 5); // ��flash spi���и�λ
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
//��ע: 
//-----------------------------------------------
void DoSPICS(WORD No, WORD Do)
{
	FLASH_CS_DISABLE;
	SAMPLE_CS_DISABLE;
	ESAM_CS_DISABLE;
	
	if( Do == FALSE )
	{
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
