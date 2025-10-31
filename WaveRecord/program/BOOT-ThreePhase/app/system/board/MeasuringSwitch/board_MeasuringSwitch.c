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

#define SYSTEM_VOL_MIN_DOWN				1.77		//电源判断门限(正常12V电源分压:12V*10/(51+10)=1.967V)
#define SYSTEM_VOL_MIN_UP				1.85		//电源判断门限(正常12V电源分压:12V*10/(51+10)=1.967V)

#define POWER_CHECK_LOOP_COUNT			10			// 10次大循环进一次
#define POWER_CHECK_CONTINUE_COUNT		5			// 连续检测次数
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
//				内部函数声明
//-----------------------------------------------

//只做了需要配置的IO，其他的用到后再添加
const port_init_t TGPIO_Config[] =
	{
		// PORT		      PIN    FUNC      Mode           LTE      INTE    INVE      PUU     DRV         NOD             BFE      Level
		{.enPort = PortE, Pin02, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 1. PE02		录波开始停止捡测按钮，按下为低电平开始转存数据
		{.enPort = PortE, Pin03, Func_Spi3_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 2. PE03		SPI_MOSI  与ESAM通信 主
		{.enPort = PortE, Pin04, Func_Spi3_Miso, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 3. PE04		SPI_MISO  与ESAM通信 主
		{.enPort = PortE, Pin05, Func_Spi3_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},  // 4. PE05		SPI_SCLK  与ESAM通信 主
		{.enPort = PortE, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 5. PE06		SPI_CS     与ESAM通信 主
		{.enPort = PortA, Pin00, Func_Usart1_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 6. PA00		USART_1_RX 红外
		{.enPort = PortA, Pin01, Func_Usart1_Tx, Pin_Mode_Out, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},  // 7. PA01		USART_1_TX 红外
		// {.enPort = PortA, Pin06, Func_Tima1, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, HIGH},		  // 8. PA06		红外PWM输出
		{.enPort = PortC, Pin00, Func_Gpio, Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 9. PC00		电源检测//工装将检测引脚从pa2改为pc0!!!!!!
		{.enPort = PortB, Pin01, Func_Gpio, Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 10.PB01		时钟电池电压检测
		{.enPort = PortA, Pin04, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},				  // 11.PA04		多功能输出
		{.enPort = PortB, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 12.PB06		CV8306
		{.enPort = PortB, Pin07, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 13.PB07		CVFLASH
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 14.PB08		SPI_CS		与8306通信  主
		{.enPort = PortH, Pin02, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 15.PH02		SPI_CS		与FLASH通信  主
		{.enPort = PortB, Pin09, Func_Spi4_Miso, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 16.PB09		SPI_MISO	与8306,FLASH通信 主
		{.enPort = PortE, Pin00, Func_Spi4_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 17.PE00		SPI_MOSI	与8306,FLASH通信 主
		{.enPort = PortE, Pin01, Func_Spi4_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},  // 18.PE01		SPI_SCLK	与8306,FLASH通信 主
		{.enPort = PortE, Pin11, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 19.PE11		遥信1状态
		{.enPort = PortE, Pin12, Func_Usart3_Tx, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 20.PE12		USART_3_TX 485
		{.enPort = PortE, Pin13, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},				  // 21.PE13		485方向控制
		{.enPort = PortE, Pin14, Func_Usart3_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 22.PE14		USART_3_RX 485
		{.enPort = PortE, Pin15, Func_Can1_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 23.PE15		CAN-RX  CAN总线
		{.enPort = PortB, Pin10, Func_Can1_Tx, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	  // 24.PB10		CAN-TX  CAN总线
		{.enPort = PortB, Pin14, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 25.PB14		SDA   开漏输出  高阻态(电表用EE)
		{.enPort = PortD, Pin10, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 26.PD10		SCL   开漏输出  高阻态(电表用EE)
		{.enPort = PortD, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 27.PD08		E2-WC2   推挽输出 epp写保护，低有效
		{.enPort = PortD, Pin09, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 28.PD09		E2-WC1   推挽输出 epp写保护，低有效
		{.enPort = PortB, Pin12, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 29.PB12		SDA   开漏输出  高阻态(电表用RTC)
		{.enPort = PortB, Pin13, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 30.PB13		SCL   开漏输出  高阻态(电表用RTC)
		{.enPort = PortA, Pin15, Func_Usart2_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 31.PA15		USART_2_RX 通讯模块
		{.enPort = PortC, Pin10, Func_Usart2_Tx, Pin_Mode_Out, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 32.PC10		USART_2_TX 通讯模块
		{.enPort = PortC, Pin11, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, HIGH},				  // 33.PC11		MDM_SET 开漏输出 初始为高
		{.enPort = PortC, Pin12, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 34.PC12		MDM_STA 上拉输入
		{.enPort = PortD, Pin00, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, LOW},				  // 35.PD00		MDM_EVE 开漏输出 初始为低
		{.enPort = PortD, Pin01, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, HIGH},				  // 36.PD01		MDM_RST 开漏输出 初始为高
		{.enPort = PortD, Pin02, Func_Tima0, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, HIGH},			  // 37.PD02		特征电流发送PWM驱动信号、低驱动，MOS管导通
		{.enPort = PortD, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 38.PD03		特征电流发送使能、高电平时才能驱动MOS
		{.enPort = PortD, Pin05, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 39.PD05		开盖低电平，闭盖高电平
		{.enPort = PortB, Pin04, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 40.PB04		通信指示灯  推挽输出 高有效
		{.enPort = PortD, Pin07, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 41.PD07		TF卡转存指示灯  推挽输出 高有效
		{.enPort = PortC, Pin05, Func_Gpio, Pin_Mode_In, Disable, Enable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 42.PC05		SPI_CS	8306输出给460	手册中的HSDC
		{.enPort = PortC, Pin04, Func_Spi1_Mosi, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 43.PC04		SPI_MOSI  8306输出给460
		{.enPort = PortA, Pin07, Func_Spi1_Sck, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	  // 44.PA07		SPI_SCLK  8306输出给460
		{.enPort = PortB, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 45.PB03		运行灯，两秒切换一次
        {.enPort = PortA, Pin05, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},	              //46.PA05               SD卡供电引脚  ，低有效
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
	static BYTE bLoopCount = 0;
	WORD wSystemVol = 0;
	BYTE bResult = 0, i = 0;

	if (Type == eOnInitDetectPowerMode)
	{
		for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
		{
			wSystemVol = api_GetADData(eOther_AD_PWR);

			if (SYSTEM_POWER_VOL(wSystemVol) < SYSTEM_VOL_MIN_UP) // 系统电源判断是否上电
			{
				break;
			}
		}

		// 上电检测，POWER_CHECK_CONTINUE_COUNT次都有电才认为有电
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
		PORT_DebugPortSetting(TDO_SWO, Disable);
		PORT_DebugPortSetting(TRST, Disable);
		PORT_DebugPortSetting(TDI, Disable);

		for(i = 0; i < (sizeof(TGPIO_Config) / sizeof(port_init_t)); i++)
		{
			if(TGPIO_Config[i].FuncReg != GPIO_DEFAULT)
			{
				PORT_Init(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin,&TGPIO_Config[i].GpioReg);
				if(TGPIO_Config[i].GpioReg.enPinMode == Pin_Mode_Out)	//设置输出状态
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
	if (Type == eCOMPONENT_SPI_FLASH)
	{
		FLASH_CS_DISABLE;
		POWER_FLASH_CLOSE; // 重新打开电源
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

	ResetSPIDevice(eCOMPONENT_SPI_FLASH, 5); // 对flash spi进行复位
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
//备注: 
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
