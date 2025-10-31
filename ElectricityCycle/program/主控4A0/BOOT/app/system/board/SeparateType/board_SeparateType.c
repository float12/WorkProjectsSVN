//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		支持698.45规约的单相表印制板驱动文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

#if (BOARD_TYPE == BOARD_HC_MEASURINGSWITCH)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define GPIO_DEFAULT	0xFF
//电平高低
#define HIGH			1
#define LOW				0

#define SYSTEM_POWER_VOL(vol)	((float)vol * 3.3 / 4096)

#define SYSTEM_VOL_MIN			0.86

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
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
//				内部函数声明
//-----------------------------------------------

// 只做了需要配置的IO，其他的用到后再添加
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
		{.enPort = PortB, Pin15, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 45.		SPI_CS		HSPI2（test）
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

		//12个外部引脚
		{.enPort = PortA, Pin01, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortA, Pin02, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortA, Pin03, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortA, Pin04, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortA, Pin05, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortE, Pin13, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortE, Pin14, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortB, Pin10, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		// {.enPort = PortD, Pin13, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		//暂时改为测试时间引脚
		{.enPort = PortD, Pin13, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_OFF, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortD, Pin10, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortD, Pin09, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚
		{.enPort = PortD, Pin08, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST 引脚

		{.enPort = PortE, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample1电源控制
		{.enPort = PortB, Pin09, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample2电源控制
		{.enPort = PortB, Pin08, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample3电源控制
		{.enPort = PortB, Pin06, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample4电源控制
		{.enPort = PortB, Pin03, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		ESAM电源控制
		{.enPort = PortA, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		FLASH电源控制


		//4G
		{.enPort = PortE, Pin01, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		4G电源控制

		{.enPort = PortB, Pin13, Func32_Usart1_Tx , HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		给4g发波形数据
		{.enPort = PortB, Pin14, Func33_Usart1_Rx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3

		{.enPort = PortC, Pin02, Func33_Usart3_Rx, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		4g读参数
		{.enPort = PortC, Pin00, Func32_Usart3_Tx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3
		
		//485
		{.enPort = PortD, Pin05, Func33_Usart4_Rx, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		485
		{.enPort = PortD, Pin06, Func32_Usart4_Tx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3
		{.enPort = PortD, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},//3. T/R-1 RS485方向控制
		// 8025+EEPROM
		{.enPort = PortD, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 79.PC11		8025SCL   开漏输出  高阻态
		{.enPort = PortD, Pin14, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 80.PC12		8025SDA   开漏输出  高阻态

		{.enPort = PortE, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 79.PC11		EEPROM SCL   开漏输出  高阻态
		{.enPort = PortE, Pin08, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 80.PC12		EEPROM SDA   开漏输出  高阻态
		
		{.enPort = PortA, Pin06, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 97.PE0  		E2-WC1	第1片EEPROM写保护	低有效
		{.enPort = PortA, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},  // 98.PE1  		E2-WC2 	第2片EEPROM写保护	低有效
};

//SPI置位管脚配置
const port_init_t TSPISetOI_Config[eCOMPONENT_TOTAL_NUM][4] =
{
	{
        {.enPort = PortB, Pin02, Func42_Spi1_Miso, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 47.		SPI_MISO	
        {.enPort = PortB, Pin01, Func41_Spi1_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	
        {.enPort = PortB, Pin00, Func40_Spi1_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	
        {.enPort = PortC, Pin10, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.		SPI_CS		
	}
};
//SPI复位管脚配置 加电源控制，否则上电初始化电源控制不上
const port_init_t TSPIResetOI_Config[eCOMPONENT_TOTAL_NUM][5] =
{
	//eCOMPONENT_SPI_FLASH
	{
		{.enPort = PortB, Pin02, Func0_Gpio, LOW, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 47.PB10		SPI_MISO	
		{.enPort = PortB, Pin01, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	
		{.enPort = PortB, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	
		{.enPort = PortC, Pin10, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		
		{.enPort = PortA, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		电源控制
	}
};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static volatile WORD SysTickCounter;
static volatile WORD SysTickCounter1;//用作拓扑信息发送使用
BYTE CommLed_Time = 0; //通信灯亮时间
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 系统电源检测
//
//参数: 	
//          Type[in]:	
//				eOnInitDetectPowerMode		上电系统电源检测
//				eOnRunDetectPowerMode		正常运行期间的系统电源检测
//				eWakeupDetectPowerMode		低功耗唤醒期间系统电源检测
//				eSleepDetectPowerMode		低功耗休眠期间系统电源检测
//返回值: 	TRUE:有电   FALSE:没有电
//
//备注:   上电vcc和比较器都判，掉电只判LVDIN0比较器，因为vcc上有超级电容
//-----------------------------------------------
BOOL api_CheckSysVol( eDETECT_POWER_MODE Type )
{
	WORD wSystemVol = 0;
	//暂无电压检测
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
//函数功能: CPU管脚初始化，统一管理。
//
//参数: 	
//          Type[in]:	ePowerOnMode	正常上电进行的管脚初始化
//						ePowerDownMode	低功耗唤醒进行的管脚初始化
//返回值: 	无
//
//备注:   
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
				if(TGPIO_Config[i].GpioReg.u16PinDir == PIN_DIR_OUT)	//设置输出状态
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
//功能描述:  与单相函数对应，因三相SPI复用，无法关闭esam电源，空函数
//         
//参数  : 无
//
//返回值:    
//         
//备注内容:  空函数 无作用
//--------------------------------------------------
void ESAMSPIPowerDown( ePOWER_MODE PowerMode )
{

}
//--------------------------------------------------
//功能描述:  对SPI下挂载的设备进行复位
//
//参数:     eCOMPONENT_TYPE  Type[in]  eCOMPONENT_SPI_SAMPLE/eCOMPONENT_SPI_FLASH/eCOMPONENT_SPI_LCD/eCOMPONENT_SPI_ESAM/eCOMPONENT_SPI_CPU
//         BYTE  Time[in]  延时时间（MS）
//
//返回值:    BOOL  TRUE: 成功 FALSE: 失败
//
//备注:	  Flash，计量芯片 两个SPI独立
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
			if (TSPIResetOI_Config[Type][i].GpioReg.u16PinDir == PIN_DIR_OUT) //设置输出状态
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

	if( Type == eCOMPONENT_SPI_FLASH ) //pe14 关闭flash CS
	{

		//FLASH_CS_DISABLE;
		POWER_FLASH_CLOSE;//重新打开电源
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
			if (TSPISetOI_Config[Type][i].GpioReg.u16PinDir == PIN_DIR_OUT) //设置输出状态
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
//函数功能: 板件初始化函数
//
//参数:		无
//                    
//返回值: 无
//		   
//备注:   
//-----------------------------------------------
void InitBoard( void )
{
	CLEAR_WATCH_DOG;
    
    InitPort( ePowerOnMode );//正常初始化
    
	ResetSPIDevice(eCOMPONENT_SPI_FLASH, 5); //对flash spi进行复位
    
    

}


//-----------------------------------------------
//函数功能: SPI片选驱动函数
//
//参数:		
//		No[in]		需要选择的芯片
// 			CS_SPI_FLASH    - FLASH
// 			CS_SPI_SAMPLE   - 计量芯片
// 			CS_SPI_ESAM     - ESAM
//      Do[in]
// 			TRUE：使能片选
// 			FALSE：禁止片选
//
//返回值: 无
//		   
//备注: 使用不是共用SPI时，不需要关闭其他的SPI_CS 
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
//中断服务程序
////////////////////////////////////////////////////////////////////////
//-----------------------------------------------
//函数功能: systick中断服务程序，10ms中断一次
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void SysTick_Handler(void)
{
 
}


#endif //#if ( BOARD_HC_MEASURINGSWITCH)
