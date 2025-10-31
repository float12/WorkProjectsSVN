//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		支持698.45规约的单相表印制板驱动文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "F460_LowPower_Three.h"

#if( BOARD_TYPE == BOARD_HC_SEPARATETYPE  )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define GPIO_DEFAULT				0xFF

//电平高低
#define HIGH						1
#define LOW							0

#define POWER_CHECK_LOOP_COUNT		10			// 10次大循环进一次
#define POWER_CHECK_CONTINUE_COUNT	5			// 连续检测次数
//串口使用       重要
#define USART_CR1_TCIE             0x00000040 
#define USART_CR1_TXEIE            0x00000080

#define SYSTEM_POWER_VOL(vol)	((float)vol * 3.3 / 4096)

#if (HARDWARE_TYPE == SINGLE_PHASE_BOARD)
#define SYSTEM_VOL_MIN_UP				1.5		//电源判断门限
#define SYSTEM_VOL_MIN_DOWN				1.42		//电源判断门限(正常12V电源分压:12V*10/(51+10)=1.967V)

#elif (HARDWARE_TYPE == THREE_PHASE_BOARD)
#define SYSTEM_VOL_MIN_UP				1.85		//电源判断门限(正常12V电源分压:12V*10/(51+10)=1.967V)
#define SYSTEM_VOL_MIN_DOWN				1.77		//电源判断门限(正常12V电源分压:12V*10/(51+10)=1.967V)
#endif

#define CAN_ERR_TIME_X10MS			5

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
enum 
{
    TRESET = 0U, 
    TSET = !TRESET
};
typedef enum
{
	TimeA5_IrqNo = Int002_IRQn,
	Uart1Rcv_IrqNo = Int003_IRQn,
	Uart1Snd_IrqNo = Int004_IRQn,
	Uart1Snd_Cmplt_IrqNo = Int005_IRQn,
	Uart1_Irq_Err_No = Int006_IRQn,
	Uart2Rcv_IrqNo = Int007_IRQn,
	Uart2Snd_IrqNo = Int008_IRQn,
	Uart2_Irq_Err_No = Int009_IRQn,
	Uart2Snd_Cmplt_IrqNo = Int010_IRQn,
	Uart3Rcv_IrqNo = Int011_IRQn,
	Uart3Snd_IrqNo = Int012_IRQn,
	Uart3_Irq_Err_No = Int013_IRQn,
	Uart3Snd_Cmplt_IrqNo = Int014_IRQn,
	CAN_Rx_IrqNo = Int015_IRQn,
	TopoSpiCs_IrqNo = Int016_IRQn,
	TopoSignalSend_IrqNo = Int017_IRQn,
	UART2_DMA2_IrqNo = Int018_IRQn,
	UART2_DMA2_Err_IrqNo = Int019_IRQn,
} IRQNUM;

typedef struct
{
	MCU_IO IO;
	en_pin_mode_t MODE;
} SYS_IO_Config;

typedef struct
{
	SYS_IO_Config IOConfig[2];
	M4_USART_TypeDef *USART_CH;

	IRQn_Type USART_Rcv_IRQn;
	func_ptr_t UsartRcvIrqCallback;
	uint16_t USART_Rcv_NUM;

	IRQn_Type USART_Err_IRQn;
	func_ptr_t UsartErrIrqCallback;
	uint16_t USART_Err_NUM;

	IRQn_Type USART_Snd_IRQn;
	func_ptr_t UsartSndIrqCallback;
	uint16_t USART_Snd_NUM;

	IRQn_Type USART_Snd_Cmplt_IRQn;
	func_ptr_t UsartSndCmpltIrqCallback;
	uint16_t USART_Snd_Cmplt_NUM;
} SYS_UART_Config;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static BOOL InitPhSci(BYTE SciPhNum);
static BOOL SciRcvEnable(BYTE SciPhNum);
static BOOL SciRcvDisable(BYTE SciPhNum);
static BOOL SciSendEnable(BYTE SciPhNum);
static BOOL SciBeginSend(BYTE SciPhNum);
static BOOL CanInitConfig(BYTE SciPhNum);
static BOOL CanRcvEnable(BYTE SciPhNum);
static BOOL CanRcvDisable(BYTE SciPhNum);
static BOOL CanSendEnable(BYTE SciPhNum);
static BOOL CanBeginSend(BYTE SciPhNum);
static BYTE SimulateInitSci(BYTE SciPhNum);
static BYTE SimulateSciRcvEnable(BYTE SciPhNum);
static BYTE SimulateSciRcvDisable(BYTE SciPhNum);
static BYTE SimulateSciSendEnable(BYTE SciPhNum);
static BYTE SimulateSciBeginSend(BYTE SciPhNum);
void UART1_IRQ_Service(void);
void UART2_IRQ_Service(void);
void UART3_IRQ_Service(void);
void UART4_IRQ_Service(void);
void UART1_IRQ_Rec_Service(void);
void UART2_IRQ_Rec_Service(void);
void UART3_IRQ_Rec_Service(void);
void UART4_IRQ_Rec_Service(void);
void UART1_IRQ_Tx_Service(void);
void UART2_IRQ_Tx_Service(void);
void UART3_IRQ_Tx_Service(void);
void UART4_IRQ_Tx_Service(void);
void UART1_IRQ_TXCMPLT_Service(void);
void UART2_IRQ_TXCMPLT_Service(void);
void UART3_IRQ_TXCMPLT_Service(void);
void UART4_IRQ_TXCMPLT_Service(void);
void UART_IRQ_Err_Service( void);
static void TopoSpiCsIrqInitConfig(void);
static void CAN_RxIrqCallBack(void);
static void TopExtInt05_Callback(void);
static void TopoSignalSendHandle(void);
static void InitTimerA3_PWM1( void );

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE IsExitLowPower;//模块内变量
DWORD LowPowerSecCount;//低功耗累计秒数
//volatile WORD MsTickCounter; //ms计数器
volatile DWORD gsysTick = 0; 
// 串口映射图（各个板子不一样）
const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM] = {
	// 第一路RS-485
	eRS485_I,
	SCI_MCU_USART3_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable,			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,			// 发送禁止

	// 载波、RF
	eCR,
	SCI_MCU_USART2_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable, 			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,			// 发送禁止
	
	// // 蓝牙
	// eBlueTooth,
	// SCI_MCU_USART1_NUM,		// MCU 串口号
	// &InitPhSci,				// 串口初始化
	// &SciRcvEnable,			// 接收允许
	// &SciRcvDisable,			// 禁止接收
	// &SciSendEnable,			// 发送允许
	// &SciBeginSend,			// 发送禁止
	
	// 红外
	// eIR,
	// SCI_MCU_USART1_NUM,		// MCU 串口号
	// &InitPhSci,				// 串口初始化
	// &SciRcvEnable,			// 接收允许
	// &SciRcvDisable,			// 禁止接收
	// &SciSendEnable,			// 发送允许
	// &SciBeginSend,			// 发送禁止


	//CAN总线
	// eCAN,
	// 0xAA,                   // MCU 串口号
	// &CanInitConfig,
	// &CanRcvEnable,
	// &CanRcvDisable,
	// &CanSendEnable,
	// &CanBeginSend,

	// eEXP_PRO,
	// 0xFF, // MCU 串口号
	// &SimulateInitSci,       // 串口初始化
	// &SimulateSciRcvEnable, 	// 接收允许
	// &SimulateSciRcvDisable,	// 禁止接收
	// &SimulateSciSendEnable,	// 发送允许
	// &SimulateSciBeginSend,  // 发送禁止
};
// 用下标SCI_MCU_USARTX_NUM取
SYS_UART_Config Uart_Config[] =
{
	//485-1  SCI_MCU_USART3_NUM
	{
		{	
			{	
				{PortE, Pin14, Func_Usart3_Rx}, {Pin_Mode_Out}, //收
			},
			{
				{PortE, Pin12, Func_Usart3_Tx}, {Pin_Mode_Out}, //发
			}
		},
		//串口序列号
		{M4_USART3},
		//中断编号                            //回调函数                    //中断源
		{(IRQn_Type)Uart3Rcv_IrqNo},		{&UART3_IRQ_Rec_Service},		{INT_USART3_RI},
		//串口接收中断编号                     //串口回调函数                //串口中断源
		{(IRQn_Type)Uart3_Irq_Err_No},		{&UART_IRQ_Err_Service},		{INT_USART3_EI},
		//串口发送中断编号                     //串口回调函数                //串口中断源
		{(IRQn_Type)Uart3Snd_IrqNo},		{&UART3_IRQ_Tx_Service},		{INT_USART3_TI},
		//串口发送完成中断编号                  //串口发送完成回调函数       //串口中断源
		{(IRQn_Type)Uart3Snd_Cmplt_IrqNo},	{&UART3_IRQ_TXCMPLT_Service},	{INT_USART3_TCI},
	},

	//模块通信 SCI_MCU_USART2_NUM
	{
		{
			{
				{PortA, Pin15, Func_Usart2_Rx}, {Pin_Mode_Out}, //收
			},
			{
				{PortC, Pin10, Func_Usart2_Tx}, {Pin_Mode_Out}, //发
			}
		},
		//序列号
		{M4_USART2},
		//串口接收中断编号                   //串口回调函数               /串口中断源
		{(IRQn_Type)Uart2Rcv_IrqNo},		{&UART2_IRQ_Rec_Service},		{INT_USART2_RI},
		//串口接收中断编号                     //串口回调函数              //串口中断源
		{(IRQn_Type)Uart2_Irq_Err_No},		{&UART_IRQ_Err_Service},		{INT_USART2_EI},
		//串口发送中断编号                     //串口回调函数               //串口中断源
		{(IRQn_Type)Uart2Snd_IrqNo},		{&UART2_IRQ_Tx_Service},		{INT_USART2_TI},
		//串口发送完成中断编号                  //串口发送完成回调函数       //串口中断源
		{(IRQn_Type)Uart2Snd_Cmplt_IrqNo},	{&UART2_IRQ_TXCMPLT_Service},	{INT_USART2_TCI},
	},

	//红外  SCI_MCU_USART1_NUM
	{
		{
			{
				{PortA, Pin00, Func_Usart1_Rx}, {Pin_Mode_Out}, //收
			},
			{
				{PortA, Pin01, Func_Usart1_Tx}, {Pin_Mode_Out}, //发
			}
		},
		{M4_USART1},
		//串口中断编号                        //串口回调函数            //串口中断源
		{(IRQn_Type)Uart1Rcv_IrqNo},		{&UART1_IRQ_Rec_Service},		{INT_USART1_RI},
		//串口接收中断编号                     //串口回调函数                //串口中断源
		{(IRQn_Type)Uart1_Irq_Err_No},		{&UART_IRQ_Err_Service},		{INT_USART1_EI},
		//串口发送中断编号                     //串口回调函数               //串口中断源
		{(IRQn_Type)Uart1Snd_IrqNo},		{&UART1_IRQ_Tx_Service},		{INT_USART1_TI},
		//串口发送完成中断编号                  //串口发送完成回调函数       //串口中断源
		{(IRQn_Type)Uart1Snd_Cmplt_IrqNo},	{&UART1_IRQ_TXCMPLT_Service},	{INT_USART1_TCI},
	},
};

//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		上电引脚配置
//------------------------------------------------------------------------------------------------------------------------------------------------------

// 只做了需要配置的IO，其他的用到后再添加
const port_init_t TGPIO_Config[] =
	{
		// PORT		      PIN    FUNC      Mode           LTE      INTE    INVE      PUU     DRV         NOD             BFE      Level
		{.enPort = PortE, Pin02, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 1. PE02		录波开始停止捡测按钮，按下为低电平开始转存数据
		{.enPort = PortC, Pin00, Func_Gpio, Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 9. PC00		电源检测//工装将检测引脚从pa2改为pc0
		{.enPort = PortB, Pin01, Func_Gpio, Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 10.PB01		时钟电池电压检测
		{.enPort = PortA, Pin04, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},				  // 11.PA04		多功能输出
		{.enPort = PortB, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 12.PB06		CV8306
		{.enPort = PortB, Pin07, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 13.PB07		CVFLASH
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 14.PB08		SPI_CS		与8306通信  主
		{.enPort = PortH, Pin02, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 15.PH02		SPI_CS		与FLASH通信  主
		{.enPort = PortB, Pin09, Func_Spi4_Miso, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 16.PB09		SPI_MISO	与8306,FLASH通信 主
		{.enPort = PortE, Pin00, Func_Spi4_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 17.PE00		SPI_MOSI	与8306,FLASH通信 主
		{.enPort = PortE, Pin01, Func_Spi4_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},  // 18.PE01		SPI_SCLK	与8306,FLASH通信 主
		{.enPort = PortE, Pin12, Func_Usart3_Tx, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 20.PE12		USART_3_TX 485
		{.enPort = PortE, Pin13, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},				  // 21.PE13		485方向控制
		{.enPort = PortE, Pin14, Func_Usart3_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 22.PE14		USART_3_RX 485
		#if(HARDWARE_TYPE ==THREE_PHASE_BOARD)	
		{.enPort = PortB, Pin14, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 25.PB14		SDA   开漏输出  高阻态(电表用EE)
		{.enPort = PortD, Pin10, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 26.PD10		SCL   开漏输出  高阻态(电表用EE)
		#elif(HARDWARE_TYPE == SINGLE_PHASE_BOARD)
		{.enPort = PortD, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},			// 25.PD03		SDA   开漏输出 高阻态(电表用EE)
		{.enPort = PortD, Pin02, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},			// 26.PD02		SCL   开漏输出 高阻态(电表用EE)
		#endif
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
		{.enPort = PortB, Pin04, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 40.PB04		通信指示灯  推挽输出 高有效
		{.enPort = PortD, Pin07, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 41.PD07		TF卡转存指示灯  推挽输出 高有效
		{.enPort = PortC, Pin05, Func_Gpio, Pin_Mode_In, Disable, Enable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 42.PC05		SPI_CS	8306输出给460	手册中的HSDC
		{.enPort = PortC, Pin04, Func_Spi1_Mosi, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 43.PC04		SPI_MOSI  8306输出给460
		{.enPort = PortA, Pin07, Func_Spi1_Sck, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	  // 44.PA07		SPI_SCLK  8306输出给460
		{.enPort = PortB, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 45.PB03		运行灯，两秒切换一次
};

//SPI置位管脚配置
const port_init_t TSPISetOI_Config[eCOMPONENT_TOTAL_NUM-1][4] =
{
	// eCOMPONENT_SPI_SAMPLE
	{
		{.enPort = PortB, Pin09, Func_Spi1_Miso, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		// 13.PB09 		SPI_MISO	与8306,FLASH通信 主
		{.enPort = PortE, Pin00, Func_Spi1_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 14.PE00 		SPI_MOSI	与8306,FLASH通信 主
		{.enPort = PortE, Pin01, Func_Spi1_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 15.PE01		SPI_SCLK	与8306,FLASH通信 主
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},				// 12.PB08 		SPI_CS		与8306通信  主
	},
	//eCOMPONENT_SPI_FLASH
	{
		{.enPort = PortB, Pin09, Func_Spi1_Miso, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		// 13.PB09 		SPI_MISO	与8306,FLASH通信 主
		{.enPort = PortE, Pin00, Func_Spi1_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 14.PE00 		SPI_MOSI	与8306,FLASH通信 主
		{.enPort = PortE, Pin01, Func_Spi1_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 15.PE01		SPI_SCLK 	与8306,FLASH通信 主
		{.enPort = PortH, Pin02, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},				// 12.PH02 		SPI_CS		与FLASH通信  主
	},
	//eCOMPONENT_SPI_ESAM
	// {
	// 	{.enPort = PortE, Pin04, Func_Spi3_Miso, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		// 3. PE04 		SPI_MISO  与ESAM通信 主
	// 	{.enPort = PortE, Pin03, Func_Spi3_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 2. PE03 		SPI_MOSI  与ESAM通信 主
	// 	{.enPort = PortE, Pin05, Func_Spi3_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 4. PE05 		SPI_SCLK  与ESAM通信 主
	// 	{.enPort = PortE, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},				// 5. PE06 		SPI_CS 	  与ESAM通信 主
	// },
};
//SPI复位管脚配置
const port_init_t TSPIResetOI_Config[eCOMPONENT_TOTAL_NUM-1][4] =
{
	// eCOMPONENT_SPI_SAMPLE
	{
		{.enPort = PortB, Pin09, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 13.PB09 		SPI_MISO  与8306通信 主
		{.enPort = PortE, Pin00, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 14.PE00 		SPI_MOSI  与8306通信 主
		{.enPort = PortE, Pin01, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 15.PE01		SPI_SCLK  与8306通信 主
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},		// 12.PB08 		SPI_CS  与8306通信  主
	},
	//eCOMPONENT_SPI_FLASH
	{
		{.enPort = PortB, Pin09, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 13.PB09 		SPI_MISO  与8306通信 主
		{.enPort = PortE, Pin00, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 14.PE00 		SPI_MOSI  与8306通信 主
		{.enPort = PortE, Pin01, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 15.PE01		SPI_SCLK  与8306通信 主
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},		// 12.PB08 		SPI_CS  与FLASH通信  主
	},
	//eCOMPONENT_SPI_ESAM
	// {
	// 	{.enPort = PortE, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},		// 2. PE03 		SPI_MOSI  与ESAM通信 主
	// 	{.enPort = PortE, Pin04, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 3. PE04 		SPI_MISO  与ESAM通信 主
	// 	{.enPort = PortE, Pin05, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 4. PE05 		SPI_SCLK  与ESAM通信 主
	// 	{.enPort = PortE, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 5. PE06 		SPI_CS 	  与ESAM通信 主
	// },
};
//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		低功耗引脚配置
//------------------------------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------------------------------------
//																	低功耗唤醒引脚配置
//------------------------------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//串口波特率配置表，        分别对应300   /600   /1200  /2400  /4800 /7200  /9600  /19200 /38100 /57600 /115200
static const WORD TAB_Baud[]={0x8F5B,0x47AC,0x23D6,0x11EB,0x08F5,0x05F9,0x047A,0x023C,0x011E,0x00BE,0x005F};

static volatile WORD SysTickCounter;
WORD HSDCTimer = 0;
static WORD Pulse_Timer = 0;//有功脉冲宽度计时器
BYTE CommLed_Time = 0; //通信灯亮时间
BYTE CanErrTime;
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern void IrPWMEnable(BOOL enNewSta);
//api_GetSAGStatus

//-----------------------------------------------
//函数功能: 系统电源检测
//
//参数: 	
//          	Type[in]:	
//				eOnInitDetectPowerMode		上电系统电源检测
//				eOnRunDetectPowerMode		正常运行期间的系统电源检测
//				eWakeupDetectPowerMode		低功耗唤醒期间系统电源检测
//				eSleepDetectPowerMode		低功耗休眠期间系统电源检测
//				eOnRunSpeedDetectPowerMode	正常运行快速系统电源检测

//返回值: 	TRUE:有电   FALSE:没有电
//
//备注:   上电vcc和比较器都判，掉电只判LVDIN0比较器，因为vcc上有超级电容
//-----------------------------------------------
BOOL api_CheckSysVol( eDETECT_POWER_MODE Type )
{
	static BYTE bLoopCount = 0;
	WORD wSystemVol = 0;
	BYTE bResult = 0, i = 0;

	// 运行中检测，POWER_CHECK_LOOP_COUNT次进1次
	if (Type == eOnRunDetectPowerMode)
	{
		bLoopCount++;
		if ((bLoopCount % POWER_CHECK_LOOP_COUNT) == 0)
		{
			bLoopCount = 0;
		}
		else
		{
			return TRUE;
		}
	}

	if( ( Type == eOnInitDetectPowerMode ) || (Type == eSleepDetectPowerMode) )
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
			api_SetSysStatus(eSYS_STATUS_POWER_ON);
			return TRUE;
		}
		else
		{
			api_ClrSysStatus(eSYS_STATUS_POWER_ON);
			return FALSE;
		}
	}
	else if((Type == eOnRunDetectPowerMode) || (Type == eOnRunSpeedDetectPowerMode) )
	{
		for(i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
		{
			wSystemVol = api_GetADData(eOther_AD_PWR);

			if (SYSTEM_POWER_VOL(wSystemVol) > SYSTEM_VOL_MIN_DOWN) // 系统电源判断是否上电
			{
				break;
			}
			
		}
		
		if( i >= POWER_CHECK_CONTINUE_COUNT )//掉电
		{
			api_ClrSysStatus(eSYS_STATUS_POWER_ON);
			return FALSE;
		}
		else
		{
			api_SetSysStatus(eSYS_STATUS_POWER_ON);
			return TRUE;
		}

	}
	return TRUE;
}
////-----------------------------------------------
////函数功能: 系统电源检测
////
////参数: 	
////          	Type[in]:	
////				eOnInitDetectPowerMode		上电系统电源检测
////				eOnRunDetectPowerMode		正常运行期间的系统电源检测
////				eWakeupDetectPowerMode		低功耗唤醒期间系统电源检测
////				eSleepDetectPowerMode		低功耗休眠期间系统电源检测
////				eOnRunSpeedDetectPowerMode	正常运行快速系统电源检测
//
////返回值: 	TRUE:有电   FALSE:没有电
////
////备注:   上电vcc和比较器都判，掉电只判LVDIN0比较器，因为vcc上有超级电容
////-----------------------------------------------
//BOOL api_CheckSysVol( eDETECT_POWER_MODE Type )
//{
//	static BYTE bLoopCount = 0;
//	WORD wSystemVol = 0;
//	BYTE bResult = 0, i = 0;
//
//	// 运行中检测，POWER_CHECK_LOOP_COUNT次进1次
//	if (Type == eOnRunDetectPowerMode)
//	{
//		bLoopCount++;
//		if ((bLoopCount % POWER_CHECK_LOOP_COUNT) == 0)
//		{
//			bLoopCount = 0;
//		}
//		else
//		{
//			return TRUE;
//		}
//	}
//
//	if( Type == eOnInitDetectPowerMode )
//	{
//		for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
//		{
//			wSystemVol = api_GetADData(eOther_AD_PWR);
//
//			if (SYSTEM_POWER_VOL(wSystemVol) < SYSTEM_VOL_MIN) // 系统电源判断是否上电
//			{
//				break;
//			}
//		}
//
//		// 上电检测，POWER_CHECK_CONTINUE_COUNT次都有电才认为有电
//		if (i >= POWER_CHECK_CONTINUE_COUNT)
//		{
//			api_SetSysStatus(eSYS_STATUS_POWER_ON);
//			return TRUE;
//		}
//		else
//		{
//			api_ClrSysStatus(eSYS_STATUS_POWER_ON);
//			return FALSE;
//		}
//	}
//	else if((Type == eOnRunDetectPowerMode) || (Type == eOnRunSpeedDetectPowerMode) )
//	{
//		if( api_GetSAGStatus() == TRUE )//防止主循环阻塞 有电直接退出
//		{
//			return TRUE;
//		}
//		else
//		{	
//			for(i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
//			{
//				if( Type == eOnRunDetectPowerMode )
//				{
//					api_Delayms( 1 );
//				}
//				else//快速检测
//				{
//					api_Delay100us( 2 );
//				}
//				
//				if( api_GetSAGStatus() == TRUE )
//				{
//					break;
//				}
//			}
//			
//			if( i >= POWER_CHECK_CONTINUE_COUNT )//掉电
//			{
//				api_ClrSysStatus(eSYS_STATUS_POWER_ON);
//				return FALSE;
//			}
//			else
//			{
//				api_SetSysStatus(eSYS_STATUS_POWER_ON);
//				return TRUE;
//			}
//		}
//
//	}
//	else if(Type == eSleepDetectPowerMode)	//SAG和12V同时有效
//	{
//		for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
//		{
//			wSystemVol = api_GetADData(eOther_AD_PWR);
//
//			if( (SYSTEM_POWER_VOL(wSystemVol) < SYSTEM_VOL_MIN) 
//			|| ( api_GetSAGStatus() != TRUE ) )// 系统电源判断是否上电
//			{
//				break;
//			}
//		}
//
//		// 上电检测，POWER_CHECK_CONTINUE_COUNT次都有电才认为有电
//		if (i >= POWER_CHECK_CONTINUE_COUNT)
//		{
//			api_SetSysStatus(eSYS_STATUS_POWER_ON);
//			return TRUE;
//		}
//		else
//		{
//			api_ClrSysStatus(eSYS_STATUS_POWER_ON);
//			return FALSE;
//		}		
//	}
//	return TRUE;
//}

//-----------------------------------------------
//函数功能: CPU管脚初始化，统一管理。
//
//参数: 	
//          Type[in]:	ePowerOnMode		正常上电进行的管脚初始化
//						ePowerDownMode		低功耗进行的管脚初始化
//						ePowerWakeUpMode	低功耗唤醒进行的管脚初始化
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

		#if(SPD_HDSC_MCU == SPD_96000K)
		PORT_PCCRRDWTSetting(WaitCycle2);	//	84M~128M等待2个周期读取时间
		#endif
//		#if( SEL_DEBUG_VERSION == YES )
//		DWORD pTGPIO_Config;
//		scanf("%x",&pTGPIO_Config);
//		if(memcmp((const void *)pTGPIO_Config, TGPIO_Config, sizeof(TGPIO_Config)) != 0)   // BOOT配置不一致
//		{
//			ASSERT(FALSE, 1);
//		}
//		else
//		{
//			/* code */
//		}
//		#endif
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


//-----------------------------------------------
//函数功能: 初始化中断
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void InitINT(void)
{
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);	//配置为2位抢占优先级，2位响应优先级
}

//-----------------------------------------------
//函数功能: 1ms定时中断配置
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
//static void InitTimer(void)
//{
//	stc_timera_base_init_t stcTimeraInit;
//    stc_irq_regi_conf_t stcIrqRegiConf;
//
//    MEM_ZERO_STRUCT(stcTimeraInit);
//    MEM_ZERO_STRUCT(stcIrqRegiConf);
//
//    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA5, Enable);
//
//    stcTimeraInit.enClkDiv = TimeraPclkDiv8;		//PCLK1时钟为12M
//    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
//    stcTimeraInit.enCntDir = TimeraCountDirUp;
//    stcTimeraInit.enSyncStartupEn = Disable;
//    
//	#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
//	stcTimeraInit.u16PeriodVal = 0x05dcu;        			//1.5M  1ms
//	#elif(SPD_HDSC_MCU == SPD_72000K)
//	stcTimeraInit.u16PeriodVal = 0x08cau;        			//2.25M 1ms
//	#endif
//    TIMERA_BaseInit(M4_TMRA5, &stcTimeraInit);
//    TIMERA_IrqCmd(M4_TMRA5, TimeraIrqOverflow, Enable);
//
//    /* Configure interrupt of timera unit 1 */
//    stcIrqRegiConf.enIntSrc = INT_TMRA5_OVF;
//    stcIrqRegiConf.enIRQn = (IRQn_Type)TimeA5_IrqNo;
//    stcIrqRegiConf.pfnCallback = &TimerA5_Handler;
//    enIrqRegistration(&stcIrqRegiConf);
//    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
//
//    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
//    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
//
//    TIMERA_Cmd(M4_TMRA5, Enable);
//}

//-----------------------------------------------
//函数功能: 获取AD转换的结果，小数点位数是固定的，如果小数点不合适，需要调用者自己转换
//
//参数:
//				BYTE Mode[in]		基准电压是否校准 0x00 校准 0x55不校准----HT无此功能
//				Type[in]			获取AD数据的类型
//				SYS_POWER_AD		    		0//系统电压 保留二位小数 单位v
//				SYS_CLOCK_VBAT_AD		    	1//时钟电池电压 保留二位小数 单位v
//				SYS_READ_VBAT_AD	    		2//抄表电池的电压 保留二位小数 单位v
//				SYS_TEMPERATURE_AD		    	3//温度AD采样 保留一位小数
//                    
//返回值:  	AD转换的结果，只有温度支持负值，其他不支持负值。温度的负值采用补码表示
//
//备注:   
//-----------------------------------------------
short GetADValue( BYTE Mode, BYTE Type)
{
	WORD wTemp;
	float fTemp;

	if( Type != SYS_CLOCK_VBAT_AD )
	{
		return 0;
	}
	wTemp = api_GetADData(eOther_AD_BAT);
	fTemp = SYSTEM_POWER_VOL(wTemp);
	wTemp = (WORD)(fTemp * 200); //转为实际电压，2位小数
	if (wTemp < 243)
	{
		return (short)wTemp;
	}
    if(abs(BatteryPara.ClockBatteryOffset) < (BYTE)(ClockBatteryStandardVoltage*15/100) )//校准偏置，允许最大值0.5V 依据5%的电阻误差，2个电阻最大误差10%，按照15%进行考虑 364*0.12= 43
	{
		wTemp += BatteryPara.ClockBatteryOffset;
	}
	if( wTemp > 368 )
	{
		wTemp = 368;
	}
	return (short)wTemp;
}



//-----------------------------------------------
//函数功能: 对外围设备的电源控制
//
//参数:		Type[in] 0:上电时对外部电源的控制  1:其他
//                    
//返回值: 无
//		   
//备注:   
//-----------------------------------------------
void PowerCtrl( BYTE Type )
{
	if( Type == 0 )
	{
		//打开电源
		//POWER_EEPROM_OPEN;
		POWER_BLUETOOTH_OPEN;
		POWER_FLASH_OPEN;
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == TRUE )
		{
			POWER_SAMPLE_OPEN;
			POWER_ESAM_OPEN;
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
//备注:	  不能随意的复位esam spi 随意复位会导致建立应用连接失效
//--------------------------------------------------
BOOL ResetSPIDevice( eCOMPONENT_TYPE Type, BYTE Time )
{
	BYTE i;
	
	if( Type >= eCOMPONENT_TOTAL_NUM )
	{
		return FALSE;
	}

	for(i = 0; i < (sizeof(TSPIResetOI_Config[Type]) / sizeof(port_init_t)); i++)
	{
		if(TSPIResetOI_Config[Type][i].FuncReg != GPIO_DEFAULT)
		{
			PORT_SetFunc(TSPIResetOI_Config[Type][i].enPort,TSPIResetOI_Config[Type][i].u16Pin,(en_port_func_t)TSPIResetOI_Config[Type][i].FuncReg,Disable);
			PORT_Init(TSPIResetOI_Config[Type][i].enPort,TSPIResetOI_Config[Type][i].u16Pin,&TSPIResetOI_Config[Type][i].GpioReg);
			if(TSPIResetOI_Config[Type][i].GpioReg.enPinMode == Pin_Mode_Out)	//设置输出状态
			{
				if(TSPIResetOI_Config[Type][i].bLevel == HIGH)
				{
					PORT_SetPortData(TSPIResetOI_Config[Type][i].enPort,TSPIResetOI_Config[Type][i].u16Pin);
				}
				else if(TSPIResetOI_Config[Type][i].bLevel == LOW)
				{
					PORT_ResetPortData(TSPIResetOI_Config[Type][i].enPort,TSPIResetOI_Config[Type][i].u16Pin);
				}
				else
				{
					
				}
			}	
		
		}
	}
	
//	if( Type == eCOMPONENT_SPI_ESAM ) //  关闭ESAM CS
//	{
//		//ESAM_CS_DISABLE;
//		POWER_ESAM_CLOSE;//重新打开电源
//		#if(SEL_ESAM_TYPE != 0)
//		api_Delayms(20);
//		POWER_ESAM_OPEN;
//		api_Delayms(Time);
//		//ESAM_CS_ENABLE;
//		#endif
//	}
	if( Type == eCOMPONENT_SPI_FLASH ) // 关闭flash CS
	{
		//FLASH_CS_DISABLE;
		POWER_FLASH_CLOSE;//重新打开电源
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		api_Delayms(Time);
		//FLASH_CS_ENABLE;
	}
	else if( Type == eCOMPONENT_SPI_SAMPLE )// 关闭计量芯片 CS
	{

		//SAMPLE_CS_DISABLE;
		POWER_SAMPLE_CLOSE;//重新打开电源
		api_Delayms(Time);
		POWER_SAMPLE_OPEN;
		api_Delayms(Time);
		//SAMPLE_CS_ENABLE;
	}
	
	for(i = 0; i < (sizeof(TSPISetOI_Config[Type]) / sizeof(port_init_t)); i++)
	{
		if(TSPISetOI_Config[Type][i].FuncReg != GPIO_DEFAULT)
		{

			PORT_SetFunc(TSPISetOI_Config[Type][i].enPort,TSPISetOI_Config[Type][i].u16Pin,(en_port_func_t)TSPISetOI_Config[Type][i].FuncReg,Disable);
			PORT_Init(TSPISetOI_Config[Type][i].enPort,TSPISetOI_Config[Type][i].u16Pin,&TSPISetOI_Config[Type][i].GpioReg);
			if(TSPISetOI_Config[Type][i].GpioReg.enPinMode == Pin_Mode_Out)	//设置输出状态
			{
				if(TSPISetOI_Config[Type][i].bLevel == HIGH)
				{
					PORT_SetPortData(TSPISetOI_Config[Type][i].enPort,TSPISetOI_Config[Type][i].u16Pin);
				}
				else if(TSPISetOI_Config[Type][i].bLevel == LOW)
				{
					PORT_ResetPortData(TSPISetOI_Config[Type][i].enPort,TSPISetOI_Config[Type][i].u16Pin);
				}
				else
				{
					
				}
			}	

		}
	}	
	
	if (Type == eCOMPONENT_SPI_FLASH)
	{
		api_InitSPI(eCOMPONENT_SPI_FLASH, eSPI_MODE_3);
	}
	else if (Type == eCOMPONENT_SPI_SAMPLE)
	{
		api_InitSPI(eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1);
	}
//	else if (Type == eCOMPONENT_SPI_ESAM)
//	{
//		#if(SEL_ESAM_TYPE != 0)
//		api_InitSPI(eCOMPONENT_SPI_ESAM, eSPI_MODE_3);
//		#endif
//	}
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
	BYTE i;

	// initTestPin();//测时间引脚

	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
    
    CLEAR_WATCH_DOG;
	
	
	//端口初始化
	// ResetSPIDevice(eCOMPONENT_SPI_ESAM, 100);	// 对esam进行复位
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 200 ); //对计量芯片进行复位
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 ); //对Flash进行复位
    
    InitPort( ePowerOnMode );//正常初始化

	api_TopWaveSpiInit();//锐能微回复必须先初始化片选再初始化SPI和DMA，片选在InitPort中初始化，所以放在InitPort下边

	
	PowerCtrl( 0 );
	
	// ESAMSPIPowerDown(ePowerOnMode); // 关闭ESAM电源，CS，SPI管脚配置为普通
	
	// ADC_Config();
	
	InitINT() ;//关闭不用的中断

	//初始化定时器
	//InitTimer();
	
	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	//各个模块上电初始化，注意不要随便调顺序
	api_PowerUpFlag();
	//api_PowerUpRtc有对RTC的复位操作 复位后必须重新配置RTC和TBS相关寄存器 因此需要重新调用一次ADC配置
	api_PowerUpRtc();
	//ADC_Config(); //!!!!特别注意 此处ADC_Config不能删 - yxk
	api_WriteFreeEvent(EVENT_SYS_START, M4_SYSREG->RMU_RSTF0);
	if ((M4_SYSREG->RMU_RSTF0_f.WDRF==1)||(M4_SYSREG->RMU_RSTF0_f.SWDRF==1))
	{
		api_WriteSysUNMsg( SYSERR_WDTRST_ERR );
	}
	
	M4_SYSREG->RMU_RSTF0_f.CLRF=1;//清除标志
	
	api_PowerUpPara();
	api_PowerUpSave();
	WaveRecordInit();
	// api_PowerUpLcd(ePowerOnMode);
	
	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	CLEAR_WATCH_DOG;
	
	api_PowerUpSample();
	
	api_PowerUpProtocol();
	// api_PowerUpCanBus();
	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	// api_PowerUpPrePay();
	// PowerUpRelay();
	
	// api_PowerUpEnergy();
	// api_PowerUpFreeze();
	
	//若上电补冻过程中掉电，尽快进入低功耗
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	// api_ReflashFreeze_Ram();
#if (SEL_SEARCH_METER == YES)
//	api_PowerUpSchClctMeter();
#endif

    #if (SEL_TOPOLOGY == YES )
    TopoPowerUp();
    #endif

	// api_PowerUpEvent();
	api_PowerUpSysWatch(); // 先刷新主动上报模式字后进行电池电压等的相关判断-与event上电处理函数有顺序关系-姜文浩
#if (MAX_PHASE == 3)
	// api_PowerUpDemand();
#endif

	// api_PowerUpVersion();

	// 系统电压自检，确保系统电压为确定状态
	if (api_CheckSysVol(eOnInitDetectPowerMode) == FALSE)
	{
		api_EnterLowPower( eFromOnRunEnterDownMode );//在此之前上电已初始化完成 此处检测到低功耗按正常掉电处理 用于解决进入掉电但认为是异常复位的问题
	}

	// api_MidLayerInit();

#if (SEL_EVENT_DI_CHANGE == YES)
	// api_PowerUpDI();
#endif

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		SerialMap[i].SCIInit(SerialMap[i].SCI_Ph_Num);
	}

	//初始化_noinit_标志
	api_InitNoInitFlag();
	
	RESET_PLC_MODUAL;
    api_Delayms(20);
	ENABLE_PLC_PROGRAMM;
	
}

void WatchDog(void)
{
	CLEAR_WATCH_DOG;
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
	if( Do == FALSE )
	{
		switch( No )
		{
			case CS_SPI_ESAM:
				ESAM_CS_DISABLE;
				break;
			case CS_SPI_FLASH:	//复用SPI,只有一个CS有效
				FLASH_CS_DISABLE;
				break;
			case CS_SPI_SAMPLE:
				SAMPLE_CS_DISABLE;
				break;
			// case CS_SPI_CPU:
			// 	SAMPLE_HCS_DISABLE;
			// 	break;
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
		// case CS_SPI_CPU:
		// 	SAMPLE_HCS_ENABLE;
		// 	break;
		default:
			break;
	}
	api_Delay10us(1);
	
	return;
}

void UART_IRQ_Err_Service( void)
{
	// UART1
	if (Set == USART_GetStatus(M4_USART1, UsartFrameErr))
	{
		USART_ClearStatus(M4_USART1, UsartFrameErr);
	}

	if (Set == USART_GetStatus(M4_USART1, UsartParityErr))
	{
		USART_ClearStatus(M4_USART1, UsartParityErr);
	}

	if (Set == USART_GetStatus(M4_USART1, UsartOverrunErr))
	{
		USART_ClearStatus(M4_USART1, UsartOverrunErr);
	}

	// UART2
	if (Set == USART_GetStatus(M4_USART2, UsartFrameErr))
	{
		USART_ClearStatus(M4_USART2, UsartFrameErr);
	}

	if (Set == USART_GetStatus(M4_USART2, UsartParityErr))
	{
		USART_ClearStatus(M4_USART2, UsartParityErr);
	}

	if (Set == USART_GetStatus(M4_USART2, UsartOverrunErr))
	{
		USART_ClearStatus(M4_USART2, UsartOverrunErr);
	}

	// UART3
	if (Set == USART_GetStatus(M4_USART3, UsartFrameErr))
	{
		USART_ClearStatus(M4_USART3, UsartFrameErr);
	}

	if (Set == USART_GetStatus(M4_USART3, UsartParityErr))
	{
		USART_ClearStatus(M4_USART3, UsartParityErr);
	}

	if (Set == USART_GetStatus(M4_USART3, UsartOverrunErr))
	{
		USART_ClearStatus(M4_USART3, UsartOverrunErr);
	}

	// UART4
	if (Set == USART_GetStatus(M4_USART4, UsartFrameErr))
	{
		USART_ClearStatus(M4_USART4, UsartFrameErr);
	}

	if (Set == USART_GetStatus(M4_USART4, UsartParityErr))
	{
		USART_ClearStatus(M4_USART4, UsartParityErr);
	}

	if (Set == USART_GetStatus(M4_USART4, UsartOverrunErr))
	{
		USART_ClearStatus(M4_USART4, UsartOverrunErr);
	}
}
// Rx
void UART1_IRQ_Rec_Service( void )
{
	UART1_IRQ_Service();
}
void UART2_IRQ_Rec_Service(void)
{
	UART2_IRQ_Service();
}
void UART3_IRQ_Rec_Service(void)
{
	UART3_IRQ_Service();
}
void UART4_IRQ_Rec_Service(void)
{
	UART4_IRQ_Service();
}
// Tx
void UART1_IRQ_Tx_Service(void)
{
	UART1_IRQ_Service();
}
void UART2_IRQ_Tx_Service(void)
{
	UART2_IRQ_Service();
}
void UART3_IRQ_Tx_Service(void)
{
	UART3_IRQ_Service();
}
void UART4_IRQ_Tx_Service(void)
{
	UART4_IRQ_Service();
}

// Tx Complete
void UART1_IRQ_TXCMPLT_Service(void)
{
	UART1_IRQ_Service();
}
void UART2_IRQ_TXCMPLT_Service(void)
{
	UART2_IRQ_Service();
}
void UART3_IRQ_TXCMPLT_Service(void)
{
	UART3_IRQ_Service();
}
void UART4_IRQ_TXCMPLT_Service(void)
{
	UART4_IRQ_Service();
}
//-----------------------------------------------
//函数功能: 串口转接收函数
//
//参数:		SCI_Ph_Num:物理串口号
//
//返回值: 	无
//		   
//备注: 
//-----------------------------------------------
static void SendToRec(BYTE SCI_Ph_Num)
{
	BYTE i;
	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
		{
			break;
		}
	}
	//合法性判断
	if( i >= MAX_COM_CHANNEL_NUM )
	{
		ASSERT(0,0);
		return ;
	}
	//立转接收状态
	SerialMap[i].SCIEnableRcv(SCI_Ph_Num);
	api_InitSciStatus(i);
}
//--------------------------------------------------
//函数功能:  物理串口485中断服务程序
//
//参数:      无
//
//返回值:    无
//
//备注:      485  1通道
//--------------------------------------------------
void UART1_IRQ_Service(void)
{
	uint8_t ch, j;

	// 收数据
	if (USART_GetStatus(M4_USART1, UsartRxNoEmpty) != Reset)
	{
		ch = USART_RecData(M4_USART1);

//		ble_uart_rx_call_back(ch);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART1_NUM, ch);
	}
	// 发送寄存器空
	if (USART_GetStatus(M4_USART1, UsartTxEmpty) != Reset)
	{
		if ((M4_USART1->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART1_NUM, M4_USART1);
		}
	}
	// 发送完成
	if (USART_GetStatus(M4_USART1, UsartTxComplete) != Reset)
	{
		if( ((M4_USART1->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// 关闭中断
			USART_FuncCmd(M4_USART1, UsartTx, Disable);			//禁止发送
			USART_FuncCmd(M4_USART1, UsartTxCmpltInt, Disable); //禁止发送完成中断

			//如果有波特率改动 发完数据更新波特率 必须每个都遍历到 存在用485_1设置485_2的情况
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART1_NUM);
		}
	}
}

void UART2_IRQ_Service(void)
{
	uint8_t ch,j;

	// 收数据
	if( USART_GetStatus(M4_USART2,UsartRxNoEmpty)!= Reset)
	{
		ch = USART_RecData(M4_USART2);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART2_NUM, ch);
	}
	// 发送寄存器空
	if (USART_GetStatus(M4_USART2, UsartTxEmpty) != Reset)
	{
		if ((M4_USART2->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART2_NUM, M4_USART2);
		}
	}
	// 发送完成
	if (USART_GetStatus(M4_USART2, UsartTxComplete) != Reset)
	{
		if (((M4_USART2->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// 关闭中断
			USART_FuncCmd(M4_USART2, UsartTx, Disable);		 //禁止发送
			USART_FuncCmd(M4_USART2, UsartTxCmpltInt, Disable); //禁止发送完成中断

			//如果有波特率改动 发完数据更新波特率 必须每个都遍历到 存在用485_1设置485_2的情况
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART2_NUM);
		}
	}
}

void UART3_IRQ_Service(void)
{
	uint8_t ch, j;

	// 收数据
	if (USART_GetStatus(M4_USART3, UsartRxNoEmpty) != Reset)
	{
		ch = USART_RecData(M4_USART3);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART3_NUM, ch);
	}
	// 发送寄存器空
	if (USART_GetStatus(M4_USART3, UsartTxEmpty) != Reset)
	{
		if ((M4_USART3->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART3_NUM, M4_USART3);
		}
	}

	// 发送完成
	if (USART_GetStatus(M4_USART3, UsartTxComplete) != Reset)
	{
		if (((M4_USART3->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// 关闭中断
			USART_FuncCmd(M4_USART3, UsartTx, Disable);			//禁止发送
			USART_FuncCmd(M4_USART3, UsartTxCmpltInt, Disable); //禁止发送完成中断

			//如果有波特率改动 发完数据更新波特率 必须每个都遍历到 存在用485_1设置485_2的情况
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART3_NUM);
		}
	}
}

void UART4_IRQ_Service(void)
{
	uint8_t ch, j;

	// 收数据
	if (USART_GetStatus(M4_USART4, UsartRxNoEmpty) != Reset)
	{
		ch = USART_RecData(M4_USART4);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART4_NUM, ch);
	}

	// 发送寄存器空
	if (USART_GetStatus(M4_USART4, UsartTxEmpty) != Reset)
	{
		if ((M4_USART4->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART4_NUM, M4_USART4);
		}
	}

	// 发送完成
	if (USART_GetStatus(M4_USART4, UsartTxComplete) != Reset)
	{
		if (((M4_USART4->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// 关闭中断
			USART_FuncCmd(M4_USART4, UsartTx, Disable);			//禁止发送
			USART_FuncCmd(M4_USART4, UsartTxCmpltInt, Disable); //禁止发送完成中断

			//如果有波特率改动 发完数据更新波特率 必须每个都遍历到 存在用485_1设置485_2的情况
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART4_NUM);
		}
	}
}
//-----------------------------------------------
//函数功能: 串口初始化函数
//
//参数:		SciPhNum[in]		需要初始化的串口物理编号
//
//返回值: 	TRUE：正确初始化   FALSE:没有正确初始化
//		   
//备注:   串口优先级升到高抢占优先级，防止其他中断运行，无法进行串口通信
//-----------------------------------------------
static BOOL InitPhSci(BYTE SciPhNum)
{
	uint32_t u32Baud, u32Fcg1Periph,u32EncodePriority;
	BYTE BpsBak, IntStatus, Bps,i;
	en_result_t enRet = Ok;
	stc_irq_regi_conf_t stcIrqRegiCfg;

	stc_usart_uart_init_t stcInitCfg = {
		UsartIntClkCkNoOutput,
		UsartClkDiv_4, //PCLK1
		UsartDataBits8,
		UsartDataLsbFirst,
		UsartOneStopBit, // 根据参数进行配置
		UsartParityEven, // 根据参数进行配置
		UsartSampleBit8,
		UsartStartBitFallEdge,
		UsartRtsEnable,
	};
	
	if( SciPhNum == SCI_MCU_USART1_NUM)
	{
		u32Fcg1Periph = PWC_FCG1_PERIPH_USART1;
	}
	else if (SciPhNum == SCI_MCU_USART2_NUM)
	{
		u32Fcg1Periph = PWC_FCG1_PERIPH_USART2;
	}
	else if (SciPhNum == SCI_MCU_USART3_NUM)
	{
		u32Fcg1Periph = PWC_FCG1_PERIPH_USART3;
		//PORT_DebugPortSetting(TRST, Disable);// PB4 RX
		//PORT_DebugPortSetting(TDO_SWO, Disable);// PB3 载波复位
	}
	else
	{
		return FALSE;
	}
	


	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SciPhNum == SerialMap[i].SCI_Ph_Num)
		{
			break;
		}
	}
	//合法性判断
	if( i >= (sizeof(Uart_Config)/sizeof(SYS_UART_Config)) )
	{
		ASSERT(0,0);
		return FALSE;
	}

	if(SerialMap[i].SerialType == eIR)//红外
	{
		InitTimerA3_PWM1();
	}

	/* Enable peripheral clock */
	PWC_Fcg1PeriphClockCmd(u32Fcg1Periph, Enable);

	 /* Initialize USART IO */
	 PORT_SetFunc(Uart_Config[i].IOConfig[0].IO.Port, Uart_Config[i].IOConfig[0].IO.Pin, Uart_Config[i].IOConfig[0].IO.IoFunc_x, Disable);
	 PORT_SetFunc(Uart_Config[i].IOConfig[1].IO.Port, Uart_Config[i].IOConfig[1].IO.Pin, Uart_Config[i].IOConfig[1].IO.IoFunc_x, Disable);

	 /* Set USART RX IRQ */
	 stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Rcv_IRQn;
	 stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartRcvIrqCallback;
	 stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Rcv_NUM;
	 enIrqRegistration(&stcIrqRegiCfg);
	 u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);
	 NVIC_SetPriority(stcIrqRegiCfg.enIRQn, u32EncodePriority);
	 NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	 NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	 /* Set USART RX error IRQ */
	 stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Err_IRQn;
	 stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartErrIrqCallback;
	 stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Err_NUM;
	 enIrqRegistration(&stcIrqRegiCfg);
	 u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);
	 NVIC_SetPriority(stcIrqRegiCfg.enIRQn, u32EncodePriority);
	 NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	 NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	 /* Set USART TX IRQ */
	 stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Snd_IRQn;
	 stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartSndIrqCallback;
	 stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Snd_NUM;
	 enIrqRegistration(&stcIrqRegiCfg);
	 u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);
	 NVIC_SetPriority(stcIrqRegiCfg.enIRQn, u32EncodePriority);
	 NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	 NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	 /* Set USART TX complete IRQ */
	 stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Snd_Cmplt_IRQn;
	 stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartSndCmpltIrqCallback;
	 stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Snd_Cmplt_NUM;
	 enIrqRegistration(&stcIrqRegiCfg);
	 u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);
	 NVIC_SetPriority(stcIrqRegiCfg.enIRQn, u32EncodePriority);
	 NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	 NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
	 /* Initialize UART */
	 /* Set baudrate */
	 //		000 -- 300
	 //		001 -- 600
	 //		010 -- 1200
	 //		011 -- 2400
	 //		100 -- 4800
	 //		101 -- 7200
	 //		110 -- 9600
	 //		111 -- 19200
	 BpsBak = api_GetBaudRate(0, SciPhNum);
	 IntStatus = api_splx(0);

	// 停止位
	Bps = ((BpsBak>>4) & 0x01);

	switch (Bps)
	{
	case 0x00: //1位停止位
		stcInitCfg.enStopBit = UsartOneStopBit;
		break;
	case 0x01: //2位停止位
		stcInitCfg.enStopBit = UsartTwoStopBit;
		break;
	default: //默认1位停止位
		stcInitCfg.enStopBit = UsartOneStopBit;
		break;
	}

	//校验
	Bps = ((BpsBak >> 5)&0x03);
	switch (Bps)
	{
	case 0x00: //无校验
		stcInitCfg.enParity = UsartParityNone;
		break;
	case 0x01: //允许校验，奇校验
		stcInitCfg.enParity = UsartParityOdd;
		break;
	case 0x02: //允许校验，偶校验
		stcInitCfg.enParity = UsartParityEven;
		break;
	default: //默认允许校验，偶校验
		stcInitCfg.enParity = UsartParityEven;
		break;
	}

	// // 固化与本体通信的UART以及蓝牙口 一位停止位，偶校验
	// if (SciPhNum == SCI_MCU_USART3_NUM)
	// {
	// 	stcInitCfg.enStopBit = UsartOneStopBit;
	// 	stcInitCfg.enParity = UsartParityEven;
	// }
	Bps = BpsBak & 0x0f;

	if( Bps <= 6 )  //波特率小于等于9600时 64分频
	{
		stcInitCfg.enClkDiv = UsartClkDiv_64;
	}
	enRet = USART_UART_Init(Uart_Config[i].USART_CH, &stcInitCfg);
	
	switch (Bps)
	{
		//case 0: //300BPS
		//	u32Baud = 300;
		//	break;
		//case 1: //600BPS
		//	u32Baud = 600;
		//	break;
		case 2: //1200bps
			u32Baud = 1200;
			break;
		case 3: //2400BPS
			u32Baud = 2400;
			break;
		case 4: //4800BPS
			u32Baud = 4800;
			break;
		case 5: //7200BPS
			u32Baud = 7200;
			break;
		case 6: //9600
			u32Baud = 9600;
			break;
		case 7: //19200
			u32Baud = 19200;
			break;
		case 8: //38400
			u32Baud = 38400;
			break;
		case 9: //57600
			u32Baud = 57600;
			break;
		case 10: //115200
			u32Baud = 115200;
			break;
		case 11: // 230400
			u32Baud = 230400;
			break;
		case 12: // 460800
			u32Baud = 460800;
			break;
		case 13: // 921600
			u32Baud = 921600;
			break;
		default: //默认115200BPS
			u32Baud = 115200;
			break;
	}
	
	enRet = USART_SetBaudrate(Uart_Config[i].USART_CH, u32Baud);
	/*Enable RX && RX interupt && timeout interrupt function*/
	USART_FuncCmd(Uart_Config[i].USART_CH, UsartRx, Enable);
	USART_FuncCmd(Uart_Config[i].USART_CH, UsartRxInt, Enable);

	SciRcvEnable(SciPhNum);

	api_InitSubProtocol(i);
	//串口监视定时器
	Serial[i].WatchSciTimer = WATCH_SCI_TIMER;

	api_splx(IntStatus);

	return TRUE;
}

//-----------------------------------------------
//函数功能: 使能串口接收函数
//
//参数:		SciPhNum[in]		需要初始化的串口物理编号
//
//返回值: 	TRUE：正确使能   FALSE:没有正确使能
//		   
//备注:   
//-----------------------------------------------
static BOOL SciRcvEnable(BYTE SciPhNum)
{
	switch (SciPhNum)
	{
	case SCI_MCU_USART1_NUM:
		ENABLE_HARD_SCI_1_RECE;
		POWER_HONGWAI_TXD_CLOSE;
		///api_Delay100us(1); // 延时100us
		USART_FuncCmd(M4_USART1, UsartRx, Enable);
		USART_FuncCmd(M4_USART1, UsartRxInt, Enable);
		break;
	case SCI_MCU_USART2_NUM:
		ENABLE_HARD_SCI_2_RECE;
		//api_Delay100us(1); // 延时100us
		USART_FuncCmd(M4_USART2, UsartRx, Enable);
		USART_FuncCmd(M4_USART2, UsartRxInt, Enable);
		break;
	case SCI_MCU_USART3_NUM:
		ENABLE_HARD_SCI_3_RECE;
		//api_Delay100us(1); // 延时100us
		USART_FuncCmd(M4_USART3, UsartRx, Enable);
		USART_FuncCmd(M4_USART3, UsartRxInt, Enable);
		break;
	case SCI_MCU_USART4_NUM:
		ENABLE_HARD_SCI_4_RECE;
		//api_Delay100us(1);// 延时100us
		USART_FuncCmd(M4_USART4, UsartRx, Enable);
		USART_FuncCmd(M4_USART4, UsartRxInt, Enable);
		break;
        
	default:
		break;
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 禁止串口接收函数
//
//参数:		SciPhNum[in]		需要初始化的串口物理编号
//
//返回值: 	TRUE：正确禁止   FALSE:没有正确禁止
//		   
//备注:   
//-----------------------------------------------
static BOOL SciRcvDisable(BYTE SciPhNum)
{
	BYTE i;

	switch (SciPhNum)
	{
	case SCI_MCU_USART1_NUM:	//蓝牙接收发送缓存不一致，可以全双工
		USART_FuncCmd(M4_USART1, UsartRx, Disable);
		USART_FuncCmd(M4_USART1, UsartRxInt, Disable);
		break;
	case SCI_MCU_USART2_NUM:
		USART_FuncCmd(M4_USART2, UsartRx, Disable);
		USART_FuncCmd(M4_USART2, UsartRxInt, Disable);
		break;
	case SCI_MCU_USART3_NUM:
		USART_FuncCmd(M4_USART3, UsartRx, Disable);
		USART_FuncCmd(M4_USART3, UsartRxInt, Disable);
		break;
	case SCI_MCU_USART4_NUM:
		USART_FuncCmd(M4_USART4, UsartRx, Disable);
		USART_FuncCmd(M4_USART4, UsartRxInt, Disable);
		break;
	default:
		break;
	}

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SciPhNum == SerialMap[i].SCI_Ph_Num)
		{
			Serial[i].WatchSciTimer = WATCH_SCI_TIMER;
		}
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 使能串口发送函数
//
//参数:		SciPhNum[in]		需要初始化的串口物理编号
//
//返回值: 	TRUE：正确使能   FALSE:没有正确使能
//		   
//备注:   
//-----------------------------------------------
static BOOL SciSendEnable(BYTE SciPhNum)
{
	switch (SciPhNum)
	{
	case SCI_MCU_USART1_NUM:
		ENABLE_HARD_SCI_1_SEND;
		POWER_HONGWAI_TXD_OPEN;
		api_Delay100us(1); // 延时100us
		break;
	case SCI_MCU_USART2_NUM:
		ENABLE_HARD_SCI_2_SEND;
		api_Delay100us(1); // 延时100us
		break;
	case SCI_MCU_USART3_NUM:
		ENABLE_HARD_SCI_3_SEND;
		api_Delay100us(1); // 延时100us
		break;
	case SCI_MCU_USART4_NUM:
		ENABLE_HARD_SCI_4_SEND;
		api_Delay100us(1); // 延时100us
		break;
	default:
		break;
	}
    return TRUE;
}


//-----------------------------------------------
//函数功能: 串口开始发送函数
//
//参数:		SciPhNum[in]		需要初始化的串口物理编号
//
//返回值: 	TRUE：正确开始发送   FALSE:没有正确开始发送
//		   
//备注:   
//-----------------------------------------------
static BOOL SciBeginSend(BYTE SciPhNum)
{
	BYTE i;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SciPhNum == SerialMap[i].SCI_Ph_Num)
		{
			if(Serial[i].SendLength == 0)
			{
				return FALSE;
			}

			if (Serial[i].TXPoint == 0)
			{
				switch (SciPhNum)
				{
				// case 0:
				// 	HT_UART0->UARTCON |= 0x0001;							//发送使能
				// 	HT_UART0->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
				// 	HT_UART0->UARTCON |= 0x0004;							//发送中断使能
				// 	NVIC_EnableIRQ(UART0_IRQn);								//开中断
				// 	break;
				case SCI_MCU_USART1_NUM:
					USART_FuncCmd(M4_USART1, UsartTx, Enable);							//发送使能
					USART_SendData(M4_USART1, Serial[i].ProBuf[Serial[i].TXPoint++]);	//写入数据
					USART_FuncCmd(M4_USART1, UsartTxEmptyInt, Enable);					//发送中断使能
					NVIC_EnableIRQ(Int004_IRQn);										//开中断
					break;
				case SCI_MCU_USART2_NUM:
					USART_FuncCmd(M4_USART2, UsartTx, Enable);						  //发送使能
					USART_SendData(M4_USART2, Serial[i].ProBuf[Serial[i].TXPoint++]); //写入数据
					USART_FuncCmd(M4_USART2, UsartTxEmptyInt, Enable);				  //发送中断使能
					NVIC_EnableIRQ(Int008_IRQn);									  //开中断
					break;
				case SCI_MCU_USART3_NUM:
					USART_FuncCmd(M4_USART3, UsartTx, Enable);						  //发送使能
					USART_SendData(M4_USART3, Serial[i].ProBuf[Serial[i].TXPoint++]); //写入数据
					USART_FuncCmd(M4_USART3, UsartTxEmptyInt, Enable);				  //发送中断使能
					NVIC_EnableIRQ(Int012_IRQn);									  //开中断
					break;
				case SCI_MCU_USART4_NUM:
					USART_FuncCmd(M4_USART4, UsartTx, Enable);						  //发送使能
					USART_SendData(M4_USART4, Serial[i].ProBuf[Serial[i].TXPoint++]); //写入数据
					USART_FuncCmd(M4_USART4, UsartTxEmptyInt, Enable);				  //发送中断使能
					NVIC_EnableIRQ(Int016_IRQn);									  //开中断
					break;
				default:
					break;
				}
			}

			break;
		}
	}

	return TRUE;
}
//-----------------------------------------------
//函数功能:CAN总线初始化
//
//参数: 
//                    
//返回值: 
//
//备注: 
//-----------------------------------------------
static BOOL CanInitConfig(BYTE SciPhNum)
{
	BYTE i,IntStatus,BpsBak,Bps;
    stc_can_init_config_t stcCanInitCfg;
    stc_irq_regi_conf_t     stcIrqRegiConf;
	stc_can_filter_t *astcFilters=NULL;
    // stc_can_filter_t astcFilters[2] = \
    // {
    //     {0x00000000ul, 0x1FFFFFFFul, CanFilterSel1, CanAllFrames},
    //     {0x00000123ul, 0x1FFFFFFFul, CanFilterSel2, CanAllFrames}
    // };

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if(SciPhNum == SerialMap[i].SCI_Ph_Num)
		{
			break;
		}
	}
	//合法性判断
	// if( i != eCAN)
	// {
	// 	ASSERT(0,0);
	// 	return FALSE;
	// }
	
	IntStatus = api_splx(0);
	
    stcIrqRegiConf.enIRQn = (IRQn_Type)CAN_Rx_IrqNo;
    stcIrqRegiConf.enIntSrc = INT_CAN_INT;
    stcIrqRegiConf.pfnCallback = &CAN_RxIrqCallBack;
    enIrqRegistration(&stcIrqRegiConf);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    //<< Enable can peripheral clock and buffer(ram)
    PWC_RamOpMdConfig(HighSpeedMd);
    PWC_RamPwrdownCmd(PWC_RAMPWRDOWN_CAN, Enable);
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_CAN, Enable);

    //打开接收中断，发送暂时使用阻塞发送，后期可以改为中断发送
    CAN_IrqCmd(CanRxIrqEn, Enable);
    CAN_IrqCmd(CanTxPrimaryIrqEn, Enable);
    
    //<< CAN GPIO config
    //PORT_SetFunc(PortE, Pin15, Func_Can1_Rx, Disable);
    //PORT_SetFunc(PortB, Pin10, Func_Can1_Tx, Disable);

    BpsBak = api_GetBaudRate(0, SciPhNum);
   	Bps = BpsBak;
    MEM_ZERO_STRUCT(stcCanInitCfg);
    //<< Can bit time config

	switch (Bps)
	{
	case 0x0D:
		stcCanInitCfg.stcCanBt.PRESC = 100u-1u;	// 0.1   200分频    10k
		break;
	case 0x0E:
		stcCanInitCfg.stcCanBt.PRESC = 40u-1u;	// 0.04  80分频		25k
		break;
	case 0x0F:
		stcCanInitCfg.stcCanBt.PRESC = 20u-1u;	// 0.02	 40分频		50k
		break;
	case 0x10:
		stcCanInitCfg.stcCanBt.PRESC = 8u-1u;	// 0.008  16分频	125K
		break;
	default:
		stcCanInitCfg.stcCanBt.PRESC = 8u-1u;	// 0.008  16分频	默认125K 
		break;
	}

    stcCanInitCfg.stcCanBt.SEG_1 = 9u-2u;
    stcCanInitCfg.stcCanBt.SEG_2 = 3u-1u;
    stcCanInitCfg.stcCanBt.SJW   = 3u-1u;

    stcCanInitCfg.stcWarningLimit.CanErrorWarningLimitVal = 10u;
    stcCanInitCfg.stcWarningLimit.CanWarningLimitVal = 16u-1u;

    stcCanInitCfg.enCanRxBufAll  = CanRxNormal;
    stcCanInitCfg.enCanRxBufMode = CanRxBufNotStored;
    stcCanInitCfg.enCanSAck = CanSelfAckDisable ;	//回环模式，自动回复ACK CanSelfAckEnable 
    stcCanInitCfg.enCanSTBMode   = CanSTBFifoMode;

    stcCanInitCfg.pstcFilter     = astcFilters;
    stcCanInitCfg.u8FilterCount  = 0;

    CAN_Init(&stcCanInitCfg);

   	api_InitSubProtocol(i);
	//串口监视定时器
	Serial[i].WatchSciTimer = WATCH_SCI_TIMER;
	
	api_splx(IntStatus);
    
	//内部回环模式，用于测试
    //CAN_ModeConfig(CanInternalLoopBackMode,Enable);
    //CAN_ModeConfig(CanExternalLoopBackMode, Enable);
	// ChangeCanMod();
	
    return TRUE;
}

//-----------------------------------------------
//函数功能: 使能CAN接收函数
//
//参数:		SciPhNum[in]		
//
//返回值: 	TRUE：正确使能   FALSE:没有正确使能
//		   
//备注:   
//-----------------------------------------------
static BOOL CanRcvEnable(BYTE SciPhNum)
{
	//CAN_IrqCmd(CanRxIrqEn, Enable);
	return TRUE;
}
//-----------------------------------------------
//函数功能: 禁止CAN接收函数
//
//参数:		SciPhNum[in]		
//
//返回值: 	TRUE：正确禁止   FALSE:没有正确禁止
//		   
//备注:   
//-----------------------------------------------
static BOOL CanRcvDisable(BYTE SciPhNum)
{
	//CAN_IrqCmd(CanRxIrqEn, Disable);
	// Serial[eCAN].WatchSciTimer = WATCH_SCI_TIMER;
	return TRUE;
}

//-----------------------------------------------
//函数功能: 禁止CAN接收函数
//
//参数:		SciPhNum[in]		
//
//返回值: 	TRUE：正确禁止   FALSE:没有正确禁止
//		   
//备注:   
//-----------------------------------------------
static BOOL CanSendEnable(BYTE SciPhNum)
{
	//CAN_IrqCmd(CanTxPrimaryIrqEn, Enable);
	return TRUE;
}
//-----------------------------------------------
//函数功能: Can开始发送函数
//
//参数:		SciPhNum[in]		
//
//返回值: 	TRUE：正确开始发送   FALSE:没有正确开始发送
//		   
//备注:   CAN总线使用中断发送
//-----------------------------------------------
static BOOL CanBeginSend(BYTE SciPhNum)
{
	BYTE i;
	static WORD Oldpoint;
	stc_can_txframe_t stcTxFrame;
	
	// if(Serial[eCAN].TXPoint != 0)
	// {
	// 	if (Serial[eCAN].TXPoint!=Oldpoint)
	// 	{
	// 		Oldpoint=Serial[eCAN].TXPoint;
	// 		CanErrTime=0;
	// 	}
	// 	else
	// 	{
	// 		if (CanErrTime>CAN_ERR_TIME_X10MS)//超过500ms
	// 		{
	// 			Serial[eCAN].TXPoint=0;
	// 			Serial[eCAN].SendLength=0;
	// 		}
			
	// 	}
		
	// 	return FALSE;
	// }
	Oldpoint=0;
	
	MEM_ZERO_STRUCT(stcTxFrame);
	//<<Can Tx
	api_SetCanID(2,1);
	stcTxFrame.StdID = api_GetCanID();	//优先级地址
	stcTxFrame.Control_f.DLC = 8;
	stcTxFrame.Control_f.IDE = 1;
	// for(i = 0; i < 8; i++)
	// {
	// 	stcTxFrame.Data[i] = Serial[eCAN].ProBuf[i];	
	// }
	// 可能进入CAN中断修改了SendLength
	// if(Serial[eCAN].SendLength == 0)
	// {
	// 	return FALSE;
	// }
	
	CAN_SetFrame(&stcTxFrame);
	CAN_TransmitCmd(CanPTBTxCmd);

	// Serial[eCAN].TXPoint = 8;
        
	return TRUE;
	
}

//-----------------------------------------------
//函数功能:HDSC—SPI初始化
//
//参数: 
//                    
//返回值: 
//
//备注: 
//-----------------------------------------------
void api_TopWaveSpiInit(void)
{
	TopoSpiCsIrqInitConfig();
	api_InitSPI(eCOMPONENT_SPI_HSDC, eSPI_MODE_1);
	TopWaveSpiDmaConfig();
	TopWaveSpiDmaEnableInit();
	HSDCTimer = 0;
}

//--------------------------------------------------
//功能描述:  关闭拓扑
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_DisTopoIqr( void )
{
	stc_exint_config_t stc;
	stc_irq_regi_conf_t stcIrqRegiConf;

	MEM_ZERO_STRUCT(stc);
	MEM_ZERO_STRUCT(stcIrqRegiConf);
	stc.enExitCh = ExtiCh05;//更改IO口需要更改中断序号，具体中断号需要查看数据手册中2.2引脚功能表
	stc.enExtiLvl = ExIntRisingEdge;
	stc.enFilterEn = Disable;
	stc.enFltClk = Pclk3Div8;
	
	EXINT_Init(&stc);

	stcIrqRegiConf.enIRQn = (IRQn_Type)TopoSpiCs_IrqNo;
	
	/* Select External Int Ch.8 */
    stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ5;//更改IO口需要更改中断序号，具体中断号需要查看数据手册中2.2引脚功能表

    /* Callback function */
    stcIrqRegiConf.pfnCallback = &TopExtInt05_Callback;

    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_DisableIRQ(stcIrqRegiConf.enIRQn);		
}
//-----------------------------------------------
//函数功能:拓扑数据传输SPI-CS片选中断初始化
//
//参数: 
//                    
//返回值: 
//
//备注: 
//-----------------------------------------------
static void TopoSpiCsIrqInitConfig(void)
{
	stc_exint_config_t stc;
	stc_irq_regi_conf_t stcIrqRegiConf;

	MEM_ZERO_STRUCT(stc);
	MEM_ZERO_STRUCT(stcIrqRegiConf);
	
	stc.enExitCh = ExtiCh05;//更改IO口需要更改中断序号，具体中断号需要查看数据手册中2.2引脚功能表
	stc.enExtiLvl = ExIntRisingEdge;
	stc.enFilterEn = Enable;
	stc.enFltClk = Pclk3Div8;
	
	EXINT_Init(&stc);
	
	stcIrqRegiConf.enIRQn = (IRQn_Type)TopoSpiCs_IrqNo;
	
	/* Select External Int Ch.8 */
    stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ5;//更改IO口需要更改中断序号，具体中断号需要查看数据手册中2.2引脚功能表

    /* Callback function */
    stcIrqRegiConf.pfnCallback = &TopExtInt05_Callback;

    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);		
}

//-----------------------------------------------
//函数功能:初始化模拟串口
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateInitSci(BYTE SciPhNum)
{
    return TRUE;
}
//-----------------------------------------------
//函数功能:接收使能
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateSciRcvEnable(BYTE SciPhNum)
{
    return TRUE;
}
//-----------------------------------------------
//函数功能:接收失能
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateSciRcvDisable(BYTE SciPhNum)
{
    
    return TRUE;
}
//-----------------------------------------------
//函数功能:发送使能
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateSciSendEnable(BYTE SciPhNum)
{
    return TRUE;
}
//-----------------------------------------------
//函数功能:发送SCI第一个字节
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateSciBeginSend(BYTE SciPhNum)
{    
    return TRUE;
}
//-----------------------------------------------
//函数功能:     设置多功能端子函数
//
//参数:	    Type[in]		多功能端子的输出类型（时钟脉冲、需量周期、时段切换）
//          
//
//返回值: 	    无
//		   
//备注:   
//-----------------------------------------------
BYTE api_MultiFunPortSet( BYTE Type )
{
    //没有5v不进行任何操作，直接退出
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )
	{
		return FALSE;
	}

	if( Type == eCLOCK_PULS_OUTPUT )
	{
	    api_InitOutClockPuls(0);	//打开时钟芯片脉冲输出
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
	else if( ( Type == eDEMAND_PERIOD_OUTPUT ) || ( Type == eSEGMENT_SWITCH_OUTPUT ) )
	{
	    api_InitOutClockPuls(55);   //关闭时钟芯片脉冲输出
        MULTI_FUN_PORT_OUT_H;
	}
    else
    {
        return FALSE;
    }
	
    GlobalVariable.g_byMultiFunPortType = Type;

    return TRUE;
}

//-----------------------------------------------
//函数功能: 多功能端子控制函数
//
//参数:		Mode[in]        0x00:控制操作        0X55:设置操作
//          Type[in]		多功能端子的输出类型（时钟脉冲、需量周期、时段切换）
//
//返回值: 	无
//
//备注:
//-----------------------------------------------
void api_MultiFunPortCtrl( BYTE Type )
{
	//没有5v不进行任何操作，直接退出
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )
	{
		return;
	}
	
	if( GlobalVariable.g_byMultiFunPortType != Type )
	{	
		return;
	}	
	
	if( (Type == eDEMAND_PERIOD_OUTPUT)||(Type == eSEGMENT_SWITCH_OUTPUT) )
	{
		api_InitOutClockPuls( 55 );    //关闭时钟芯片脉冲输出
		GlobalVariable.g_byMultiFunPortCounter = (80 / SYS_TICK_PERIOD_MS); //80ms 由systick定时完成
		MULTI_FUN_PORT_OUT_L;
	}
	else if( Type == eCLOCK_PULS_OUTPUT )
	{
		//切换为此模式时，自动输出，没有调用的地方
		api_InitOutClockPuls( 0 );    //打开时钟芯片脉冲输出
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
}
//-----------------------------------------------
//函数功能: 获取运行时间
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
DWORD  GetTicks(void)
{
	return gsysTick;
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
void SysTickIRQ_Service(void)
{
    BYTE i;
    SysTickCounter ++;
    CanErrTime++;
    if( SysTickCounter >= 10000 )
    {
    	SysTickCounter = 0;
    }
	
    api_SetAllTaskFlag( eFLAG_10_MS );
    
    if( (SysTickCounter%50) == 0 )
    {
    	api_SetAllTaskFlag( eFLAG_500_MS );
    }
    //读卡用
    #if( PREPAY_MODE == PREPAY_LOCAL )
    api_BeepReadCard();
    #endif
    //通讯延时用
    for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
    {
        if(Serial[i].ReceToSendDelay > 0)//10ms调用一次
        {
            Serial[i].ReceToSendDelay --;
        }
        if(Serial[i].RxOverCount > 1)//10ms调用一次 注意，必须是大于1
        {
            Serial[i].RxOverCount --;
        }
    }
	//主动上报延时减减
	// ReportTimeMinus();
    
    if( GlobalVariable.g_byMultiFunPortCounter != 0 )
    {
    	GlobalVariable.g_byMultiFunPortCounter--;
    	if( GlobalVariable.g_byMultiFunPortCounter == 0 )
    	{
    		MULTI_FUN_PORT_OUT_H;
    	}
    }
}

//-----------------------------------------------
//函数功能: 时钟中断服务程序，用于产生秒、分、时任务标志
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void RTCIRQ_Service( void )
{

}
void SysTick_Handler(void)
{
    BYTE i;
	static BYTE FlashCnt = 0;  //闪烁led进入该中断计数器
	#if( SEL_TOPOLOGY == YES )
	static WORD SysTickCounter1 = 0;//用作拓扑信息发送使用
	#endif
    
	#if( SEL_DEBUG_VERSION == YES )
	//TickDelayTest();
	#endif
	//创建新文件闪灯
    if(CreateNewFileFlag == 1)
	{
		FlashCnt++;
		//每30ms闪烁一次
		if((FlashCnt % 3) == 0)
		{
			TF_TOGGLE_LED;
		}
		if (FlashCnt == 30)
		{
			CreateNewFileFlag = 0;
			FlashCnt = 0;
		}
	}

	gsysTick += 10;
	SysTickCounter ++;
    if( SysTickCounter >= 10000 )
    {
    	SysTickCounter = 0;
    }

	// 按键没有变化，观察延时
	if (TFKeySmoothTimer != 0)
	{
		TFKeySmoothTimer--;
	}

	if(bySamplePowerOnTimer <= HSDC_OPEN_TIME)
	{
		HSDCTimer ++;
	}
	
    api_SetAllTaskFlag( eFLAG_10_MS );
    
    if( (SysTickCounter%50) == 0 )
    {
    	api_SetAllTaskFlag( eFLAG_500_MS );
    }
    
    //通讯延时用
    for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
    {
        if(Serial[i].ReceToSendDelay > 0)//10ms调用一次
        {
            Serial[i].ReceToSendDelay --;
        }
        if(Serial[i].RxOverCount > 1)//10ms调用一次 注意，必须是大于1
        {
            Serial[i].RxOverCount --;
        }
		if(Serial[i].SendToSendDelay > 0)
		{
			Serial[i].SendToSendDelay --;
		}
    }
    
    if( GlobalVariable.g_byMultiFunPortCounter != 0 )
    {
    	GlobalVariable.g_byMultiFunPortCounter--;
    	if( GlobalVariable.g_byMultiFunPortCounter == 0 )
    	{
    		MULTI_FUN_PORT_OUT_H;
    	}
    }
	//主动上报延时减减
	// ReportTimeMinus();
	
	#if(SEL_EVENT_DI_CHANGE == YES)
	api_DISample();
	#endif

	#if( SEL_TOPOLOGY == YES )
	if(api_GetSysStatus(eSYS_STATUS_TOPOING) == TRUE)
	{
		SysTickCounter1++;
		if(SysTickCounter1 >= 60)
		{
			SysTickCounter1 = 0;
			TopoSignalSendHandle();
		}
	}
	#endif
}
//-----------------------------------------------
//函数功能: TimerA5中断服务程序，1ms中断一次
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   执行10.8us
//-----------------------------------------------
//static void TimerA5_Handler(void)
//{
//	static BYTE bSpiCsCount=0;
//	TRealTimer TmpRealTimer;
//	
//	MsTickCounter ++;
//
//	if(MsTickCounter % 2)
//	{
//		MsTickCounter =0;
//	}
//
//    TIMERA_ClearFlag(M4_TMRA5, TimeraFlagOverflow);
//}
//-----------------------------------------------
//函数功能: CAN总线中断函数
//
//参数: 	
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void CAN_RxIrqCallBack(void)
{
	BYTE i,bLen,bLen1,j;
	WORD wPoint;

	stc_can_rxframe_t stcRxFrame;
	stc_can_txframe_t stcTxFrame;	
	
    if(true == CAN_IrqFlgGet(CanRxIrqFlg))
	{
		//CAN_IrqCmd(CanRxIrqEn, Disable);
		CAN_Receive(&stcRxFrame); 
		// if( CanBusPara.bRxNum != 0 )
		// {
		// 	for ( i = 0; i < CanBusPara.bRxNum; i++)
		// 	{
		// 		if (CanBusPara.bRxNode[i]==(stcRxFrame.StdID&0x000000ff))//过滤监视ID
		// 		{
					if (api_ChargingCan(stcRxFrame)==FALSE)//若非充电桩数据
					{
						// if((Serial[eCAN].RXWPoint + stcRxFrame.Cst.Control_f.DLC) < MAX_PRO_BUF_REAL)
						// {
						// 	//获取有效数据
						// 	memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],stcRxFrame.Cst.Control_f.DLC);
						// 	Serial[eCAN].RXWPoint += stcRxFrame.Cst.Control_f.DLC;
						// }
						// else
						{
							// bLen = MAX_PRO_BUF_REAL - Serial[eCAN].RXWPoint;
							// memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],bLen);
							// bLen1 = stcRxFrame.Cst.Control_f.DLC - bLen;
							// memcpy(&Serial[eCAN].ProBuf[0],&stcRxFrame.Data[bLen],bLen1);
							// Serial[eCAN].RXWPoint = bLen1;	
						}
						api_SetCanID(1,(BYTE)stcRxFrame.ExtID);
					}
		// 			break;
		// 		}
		// 	}
		// }
		// else //未设置CAN总线ID参数时当做普通口处理
		// {
		// 	if((Serial[eCAN].RXWPoint + stcRxFrame.Cst.Control_f.DLC) < MAX_PRO_BUF_REAL)
		// 	{
		// 		//获取有效数据
		// 		memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],stcRxFrame.Cst.Control_f.DLC);
		// 		Serial[eCAN].RXWPoint += stcRxFrame.Cst.Control_f.DLC;
		// 	}
		// 	else
		// 	{
		// 		bLen = MAX_PRO_BUF_REAL - Serial[eCAN].RXWPoint;
		// 		memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],bLen);
		// 		bLen1 = stcRxFrame.Cst.Control_f.DLC - bLen;
		// 		memcpy(&Serial[eCAN].ProBuf[0],&stcRxFrame.Data[bLen],bLen1);
		// 		Serial[eCAN].RXWPoint = bLen1;	
		// 	}
		// }
		
		CAN_IrqFlgClr(CanRxIrqFlg);
	}
	if(true == CAN_IrqFlgGet(CanTxPrimaryIrqFlg))
	{
//		if(Serial[eCAN].TXPoint >= Serial[eCAN].SendLength)
//		{
//			//数据发送完成
//			//Serial[eRS485_II].SendToSendDelay = MAX_TX_COMMIDLETIME;
//			CAN_IrqFlgClr(CanTxPrimaryIrqFlg);
//			//如果有波特率改动 发完数据更新波特率 必须每个都遍历到 存在用485_1设置485_2的情况
//			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
//			{
//				DealSciFlag(j);
//			}
//			SendToRec(0xAA);
//			return; 
//		}
		// else if((Serial[eCAN].TXPoint + 8) <= Serial[eCAN].SendLength)
		// {
		// 	bLen = 8;
		// }
//		else
//		{
//			api_SetCanID(2,0);
//			// bLen = Serial[eCAN].SendLength - Serial[eCAN].TXPoint;
//		}

		MEM_ZERO_STRUCT(stcTxFrame);
		//<<Can Tx
		stcTxFrame.StdID = api_GetCanID();	//优先级地址
		stcTxFrame.Control_f.DLC = 8;	//严格按照8个字节回复
		stcTxFrame.Control_f.IDE = 1;
		// for(i = 0; i < bLen; i++)
		// {
		// 	stcTxFrame.Data[i] = Serial[eCAN].ProBuf[Serial[eCAN].TXPoint+i];	
		// }
                
		// Serial[eCAN].TXPoint += i;
                
		if( (i < 8) && (i == bLen) )
		{
			for(i = bLen; i < 8; i++)
			{
				stcTxFrame.Data[i] = 0x00;	
			}			
		}
		
		CAN_SetFrame(&stcTxFrame);
		CAN_TransmitCmd(CanPTBTxCmd);

		CanErrTime=0;

		CAN_IrqFlgClr(CanTxPrimaryIrqFlg);

	}
    if(true == CAN_IrqFlgGet(CanRxOverIrqFlg))
    {
        CAN_IrqFlgClr(CanRxOverIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanRxBufFullIrqFlg))
    {
        CAN_IrqFlgClr(CanRxBufFullIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanRxBufAlmostFullIrqFlg))
    {
        CAN_IrqFlgClr(CanRxBufAlmostFullIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanTxSecondaryIrqFlg))
    {
        CAN_IrqFlgClr(CanTxSecondaryIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanErrorIrqFlg))
    {
        CAN_IrqFlgClr(CanErrorIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanAbortIrqFlg))
    {
        CAN_IrqFlgClr(CanAbortIrqFlg);
    }
}


//-----------------------------------------------
//函数功能:初始化PWM输出（红外38k)（实际为37.807K）
//
//参数: 
//                    
//返回值:  无
//
//备注: 用于红外调制,占空比25%（硬件设计反相，红外发射灯实际输出为76.9%占空比）
//-----------------------------------------------
static void InitTimerA3_PWM1( void )
{
	stc_timera_base_init_t timera_base;
	stc_timera_compare_init_t timera_compare;

	PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA3,Enable);

	MEM_ZERO_STRUCT(timera_base);
	
	TIMERA_DeInit(M4_TMRA3);//初始化第3个TIMERA单元

	timera_base.enClkDiv = TimeraPclkDiv8;					//PCLK1 时钟频率12M  18M  12M
	timera_base.enCntMode = TimeraCountModeSawtoothWave; 	//锯齿波方式
	timera_base.enCntDir = TimeraCountDirUp;				//向上计数
	timera_base.enSyncStartupEn = Disable;					//不与模块1同步
	
	#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
	timera_base.u16PeriodVal = 316;							//接近38kHz
	#elif(SPD_HDSC_MCU == SPD_72000K)
	timera_base.u16PeriodVal = 474;							//接近38kHz
	#endif
	TIMERA_BaseInit(M4_TMRA3,&timera_base);
	MEM_ZERO_STRUCT(timera_compare);
	
	#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
	timera_compare.u16CompareVal = 79;						//25%占空比
	#elif(SPD_HDSC_MCU == SPD_72000K)
	timera_compare.u16CompareVal = 118;						//接近25%占空比
	#endif
	timera_compare.enStartCountOutput = TimeraCountStartOutputHigh;	//开始计数时为高电平
	timera_compare.enStopCountOutput = TimeraCountStopOutputHigh;	//结束计数时为高电平
	timera_compare.enCompareMatchOutput = TimeraCompareMatchOutputReverse;	//比较值匹配时切换为电平转换
	timera_compare.enPeriodMatchOutput = TimeraPeriodMatchOutputHigh;		//等于周期值时切换为高电平
	timera_compare.enSpecifyOutput = TimeraSpecifyOutputInvalid;			//下一周期开始时切换无效
	TIMERA_CompareInit(M4_TMRA3,TimeraCh1,&timera_compare);

	TIMERA_CompareCmd(M4_TMRA3,TimeraCh1,Disable);	//关闭PWM功能，需要时直接开启

	TIMERA_Cmd(M4_TMRA3,Enable);
}

//红外PWM使能
void IrPWMEnable(BOOL enNewSta)
{
	if(enNewSta == TRUE)
	{
		TIMERA_Cmd(M4_TMRA3,Enable);
	}
	else
	{
		TIMERA_Cmd(M4_TMRA3,Disable);
	}

	TIMERA_CompareCmd(M4_TMRA3,TimeraCh1,Enable);		//打开PWM输出且确保定时器关闭时输出高电平
}

//-----------------------------------------------
//函数功能: 拓扑数据接收SPI_CS中断函数
//
//参数: 	
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void TopExtInt05_Callback(void)
{
	if( HSDC_SPI_CS_STATUS )
	{
		api_ParseHsdcWaveBuf();
		memset(&sTopoWave.RxBuf[0], 0, sizeof(sTopoWave.RxBuf));
		TopWaveSpiDmaEnableInit();
	}
}
#if( SEL_TOPOLOGY == YES )
//拓扑PWM使能
void TopoPWMEnable(BOOL enNewSta)
{
	if(enNewSta == TRUE)
	{
		TP_PWMEN_ON;
		TIMERA_Cmd(M4_TMRA2,Enable);
	}
	else
	{
		TP_PWMEN_OFF;
		TIMERA_Cmd(M4_TMRA2,Disable);
	}
	
	TIMERA_CompareCmd(M4_TMRA2,TimeraCh4,(en_functional_state_t)Enable);	//打开PWM输出且确保定时器关闭时输出高电平
}

//-----------------------------------------------
//函数功能: 拓扑发送处理
//
//参数: 	
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void TopoSignalSendHandle(void)
{
	static BYTE AlreadySendBitLen = 0 ,SignatureCodeLen = 0;//用作已处理数据(bit)长度,获取数据长度
	static BYTE SignatureCode[16] = {0}; //用作填写数据缓存
	BYTE ByteIndex, BitIndex;


	if(AlreadySendBitLen == 0)
	{
		SignatureCodeLen = ModulatSignalInfoRAM.SignatureCodeLen;

		if(SignatureCodeLen > sizeof(SignatureCode))
		{
			SignatureCodeLen = sizeof(SignatureCode);
		}

		memcpy(SignatureCode,ModulatSignalInfoRAM.SignatureCode,SignatureCodeLen);
	}

	if( ((AlreadySendBitLen / 8) >= SignatureCodeLen)
	 || (SignatureCodeLen != 2)			//特征码长度固定2字节
	)
	{
		//发送完成
		TopoPWMEnable(FALSE);
		api_ClrSysStatus(eSYS_STATUS_TOPOING);
		AlreadySendBitLen = 0;
		SignatureCodeLen = 0;
		memset(SignatureCode,0x00,sizeof(SignatureCode));
		return;
	}

	ByteIndex = SignatureCodeLen - 1 - AlreadySendBitLen / 8;	//从高字节开始传
	BitIndex  = 7 - (AlreadySendBitLen % 8);				//从高位开始传

	if(SignatureCode[ByteIndex] & (1 << BitIndex))
	{
		TopoPWMEnable(TRUE);
	}
	else
	{
		TopoPWMEnable(FALSE);
	}
	
	AlreadySendBitLen++;
}


//-----------------------------------------------
//函数功能: 初始化拓扑发送PWM相关寄存器
//
//参数: 	
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void InitTimerA2_PWM4(WORD wPeriod, WORD wCompare)
{
	stc_timera_base_init_t timera_base;
	stc_timera_compare_init_t timera_compare;

    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA2,Enable);

	MEM_ZERO_STRUCT(timera_base);
	
	TIMERA_DeInit(M4_TMRA2);//初始化第二个TIMERA单元

	timera_base.enClkDiv = TimeraPclkDiv8;					//PCLK1/8 时钟频率12M
	timera_base.enCntMode = TimeraCountModeSawtoothWave; 	//锯齿波方式
	timera_base.enCntDir = TimeraCountDirUp;				//向上计数
	timera_base.enSyncStartupEn = Disable;				//不与模块1同步
	timera_base.u16PeriodVal = 12 * wPeriod;			//0.0012 * (12*1000*1000) = 14400	
	TIMERA_BaseInit(M4_TMRA2,&timera_base);

	MEM_ZERO_STRUCT(timera_compare);
	timera_compare.u16CompareVal = 12 * wCompare;		//2/3的高电平(反逻辑)
	timera_compare.enStartCountOutput = TimeraCountStartOutputHigh;	//开始计数时为高电平
	timera_compare.enStopCountOutput = TimeraCountStopOutputHigh;	//结束计数时为高电平
	timera_compare.enCompareMatchOutput = TimeraCompareMatchOutputReverse;	//比较值匹配时切换为电平转换
	timera_compare.enPeriodMatchOutput = TimeraPeriodMatchOutputHigh;		//等于周期值时切换为高电平
	timera_compare.enSpecifyOutput = TimeraSpecifyOutputInvalid;			//下一周期开始时切换无效
	TIMERA_CompareInit(M4_TMRA2,TimeraCh4,&timera_compare);

	TIMERA_Cmd(M4_TMRA2,Disable);
	TIMERA_CompareCmd(M4_TMRA2,TimeraCh4,Enable);	//关闭PWM功能，需要时直接开启
}
#endif
//-----------------------------------------------
// 函数功能:     查询端口对应串口
//
// 参数:       Type[in] ：类型 eSERIAL_TYPE
//
//
// 返回值:   查表查得的串口号
//
// 备注:   返回< MAX_COM_CHANNEL_NUM 正确 ；等于MAX_COM_CHANNEL_NUM错误
//-----------------------------------------------
BYTE api_GetSerialIndex(BYTE Type)
{
	BYTE i;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++) // 查表，判断串口号
	{
		if (Type == SerialMap[i].SerialType)
		{
			break;
		}
	}
	if (i == MAX_COM_CHANNEL_NUM) // 未找到该串口 返回
	{
		ASSERT(0, 0);
	}

	return i;
}

#endif//#if( (BOARD_TYPE == BOARD_HC_SEPARATETYPE) )

