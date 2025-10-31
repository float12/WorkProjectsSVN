//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		支持698.45规约的单相表印制板驱动文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT_LowPower_Single.h"

#if ( (BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303)\
   || (BOARD_TYPE == BOARD_HT_SINGLE_78202401) || (BOARD_TYPE == BOARD_HT_SINGLE_20250819))
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//GPIO
#define GPIO_NUM				80			//GPIOA..GPIOE(16 * 5 = 80)
#define GPIOA_POSITION			0			//GPIOA查表位置
#define GPIOB_POSITION			16			//GPIOB查表位置
#define GPIOC_POSITION			32			//GPIOC查表位置
#define GPIOD_POSITION			48			//GPIOD查表位置
#define GPIOE_POSITION			64			//GPIOE查表位置



#define SWITCH_VOL_5_MIN			1.1			//3.3V电源 (检测电源5V，分压1/4)
#define MAX_COUNT					150
#define CURR_7P5MV_SAMPLE			15900		//百分之十额定电流下采样值(7.5mV)
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct
{
	long yn1;
	long yn2;
	long yn3;
}
 lpf_vars_t;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static BOOL InitPhSci(BYTE SciPhNum);
static BOOL SciRcvEnable(BYTE SciPhNum);
static BOOL SciRcvDisable(BYTE SciPhNum);
static BOOL SciSendEnable(BYTE SciPhNum);
static BOOL SciBeginSend(BYTE SciPhNum);
static void InitExtInt(void);

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE IsExitLowPower;//模块内变量

// 串口映射图（各个板子不一样）
const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM] = 
{
	// 第一路RS-485
	eRS485_I,
	SCI_MCU_USART2_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable,			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,			// 发送开始
	// 415通信
	ePT_UART1_F415,
	SCI_MCU_USART1_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable,			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,          // 发送开始
	// 载波
	eCR,
	SCI_MCU_USART0_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable, 			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,          // 发送开始
	// 蓝牙
	eBlueTooth,
	SCI_MCU_USART3_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable, 			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,          // 发送开始
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		上电引脚配置
//------------------------------------------------------------------------------------------------------------------------------------------------------
const GPIO_InitTypeDef GPIO_PowerOn[GPIO_NUM] = 
{
  //GPIOA引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PA0	3.3V        载波光耦/蓝牙电源控制--低电平打开载波、蓝牙电源，推挽输出，检测掉电后输出高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA1	SEG1		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PA2	PLCEVE		普通端口 输出 推挽 低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCRST		普通端口 输出 推挽 高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA4	SEG2		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON,	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW },	//PA5	FLASH_PWR_EN		普通端口 输出 推挽 低(低电平有效) 供电
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA6	JTAG		普通端口 输入 浮空  
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PA7	LCDLED		普通端口 输出 推挽 高 低电平背光亮
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	SEG3        第二功能 输入 浮空

  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA9	PF-计量芯片无功脉冲输入  第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	QF-计量芯片有功脉冲输入  第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DISPKEY--翻页按键		普通端口 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA12	SEG15--------第一功能 输入 浮空 无 
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA13	SEG16--------第一功能 输入 浮空 无
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				无此引脚
  
  //GPIOB引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB0	SEG4		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB1	SEG5		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB2	SEG6		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB3	SEG7		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB4	SEG8		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB5	SEG9		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB6	ESAM-CS		普通端口 输出 低   SPI引脚配置SPI时再处理
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB7	ESAM-CLK	普通端口 输出 低
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB8	ESAM-SO		普通端口 输出 低
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB9	ESAM-SI		普通端口 输出 低  
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB10	VESAM		普通端口 输出 低
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB11	SEG10  		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB12	SEG11		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB13	SEG12		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB14	SEG13		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB15	SEG14		第一功能 输入 浮空
  
  //GPIOC引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC0	UTX1--415	第一功能 输出 推挽
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC1	URX1--415	第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC2	PLCRX0		第一功能 输入 浮空 载波模块串口输入
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC3	PLCTX0		第一功能 输出 推挽
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC4	FLASH-SI	第一功能 输出 推挽  
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC5	FLASH-SO	第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC6	FLASK-CLK	第一功能 输出 推挽
  {eGPIO_TYPE_COMMON,  	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW },	//PC7	FLASH_CS	第一功能 输出 推挽--低电平有效
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PC8	CV415		415电源控制: 普通端口 输出 推挽 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC9	RELAYOFF	普通端口 输出 推挽 低--继电器控制
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC10	RELAYON		普通端口 输出 推挽 低--继电器控制
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC11	TX2--485	第一功能 输出 推挽
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC12	RX2--485	第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	EE_SCL		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	EE_SDA		普通端口 输出 开漏 高
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				无此引脚
  
  //GPIOD引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD0	SEG17		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD1	SEG18		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD2	SEG19		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD3	SEG20		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD4	SEG21		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD5	SEG22		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD6	SEG23		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD7	SEG24		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD8	COM1		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD9	COM2		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD10	COM3		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD11	COM4		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD12	COM5		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD13	COM6		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD14	CSJL		计量芯片片选
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD15	RELAY_TEST	普通端口 输入 浮空
  
  //GPIOE引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PE0	CVJL		计量芯片供电控制: 普通端口 输出 推挽 低
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PE1	SEC			秒脉冲/多功能输出	第二功能 输出 推挽
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE2	PULSE_LED	普通端口 输出 推挽 高 低电平脉冲灯亮
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE3	LEDRELAY	普通端口 输出 推挽 高，低电平有效
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE4	RX3			蓝牙接收，第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PE5	TX3			蓝牙发送，第二功能 输出 推挽
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},    //PE6	时钟电池电压 第二功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE8	485_CTRL	普通端口 输出 推挽 高 接收
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE9				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE10				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE11				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE12				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE13				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE15				无此引脚
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		低功耗引脚配置
//------------------------------------------------------------------------------------------------------------------------------------------------------
const GPIO_InitTypeDef GPIO_PowerDown[GPIO_NUM] = 
{
	//由于静电影响, LCD引脚需配置为功能端口(第一功能)--PA2  PB0 - PB10  PB12 - PB15  PD0 - PD13
	
  //GPIOA引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA0	PWR_EN		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA1	SEG1		第一功能 输出 开漏
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCEVE'		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCRST'		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA4	SEG2		第一功能 输出 开漏
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA5	ACTIVEP		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA6	CoverKey	普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA7	LCDLED		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA8	SEG3		第一功能 输出 开漏
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA9	PULSE_LED	普通端口 输出 开漏 高
  #if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	CoverKey	普通端口 输入 浮空
  #else
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA10	ICKEY		普通端口 输出 开漏 高  
  #endif
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DispKey		普通端口 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA12	SEG15		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA13	SEG16		第一功能 输出 开漏
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				无此引脚
  
  //GPIOB引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB0	SEG4		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB1	SEG5		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB2	SEG6		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB3	SEG7		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB4	SEG8		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB5	SEG9		第一功能 输出 开漏
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	ESAM-CS2	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB7	ESAM-CLK2	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB8	ESAM-SO2	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB9	ESAM-SI2	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB10	VESAM		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB11	SEG10		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB12	SEG11		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB13	SEG12		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB14	SEG13		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB15	SEG14		第一功能 输出 开漏
  
  //GPIOC引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC0	TXHW		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC1	RXHW		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC2	PLCRX		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC3	PLCTX		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC4	FLASH-SI	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC5	FLASH-SO	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC6	FLASK-CLK	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC7	FLASH-CS	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC8	LEDRELAY	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC9	RELAYOFF	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC10	RELAYON		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC11	TX485		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC12	RX485		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	EE_SCL		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	EE_SDA		普通端口 输出 开漏 高
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				无此引脚
  
  //GPIOD引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD0	SEG17		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD1	SEG18		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD2	SEG19		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD3	SEG20		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD4	SEG21		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD5	SEG22		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD6	SEG23		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD7	SEG24		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD8	COM1		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD9	COM2		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD10	COM3		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD11	COM4		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD12	COM5		第一功能 输出 开漏
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD13	COM6		第一功能 输出 开漏
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD14	RELAY_CTRL	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD15	RELAY_TEST	普通端口 输出 开漏 高
  
  //GPIOE引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	SEC			普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	ICIO		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	MCUCLK		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE4	METERIC_RX	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE5	METERIC_TX	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE6	ICRST		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE7	LVDIN		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	485_CTRL	普通端口 输出 开漏 高
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE9				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE10				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE11				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE12				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE13				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE15				无此引脚
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
//																	低功耗唤醒引脚配置
//------------------------------------------------------------------------------------------------------------------------------------------------------
const GPIO_InitTypeDef GPIO_WakeUp[GPIO_NUM] = 
{
  //GPIOA引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA0	PWR_EN		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA1	SEG1 		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCEVE'		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCRST'		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA4	SEG2 		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA5	ACTIVEP		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA6	CoverKey	普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA7	LCDLED		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	SEG3 		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA9	PULSE_LED	普通端口 输出 开漏 高
  #if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	CoverKey	普通端口 输入 浮空
  #else
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA10	ICKEY		普通端口 输出 开漏 高  
  #endif
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DispKey		普通端口 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA12	SEG15 		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA13	SEG16 		第一功能 输入 浮空
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				无此引脚
  
  //GPIOB引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB0	SEG4		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB1	SEG5		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB2	SEG6		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB3	SEG7		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB4	SEG8		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB5	SEG9		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	ESAM-CS2	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB7	ESAM-CLK2	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB8	ESAM-SO2	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB9	ESAM-SI2	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB10	VESAM		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB11	SEG10		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB12	SEG11		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB13	SEG12		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB14	SEG13		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB15	SEG14		第一功能 输入 浮空
  
  //GPIOC引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC0	TXHW		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC1	RXHW		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC2	PLCRX		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC3	PLCTX		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC4	FLASH-SI	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC5	FLASH-SO	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC6	FLASK-CLK	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC7	FLASH-CS	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC8	LEDRELAY	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC9	RELAYOFF	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC10	RELAYON		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC11	TX485		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC12	RX485		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	EE_SCL		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	EE_SDA		普通端口 输出 开漏 高
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				无此引脚
  
  //GPIOD引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD0	SEG17		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD1	SEG18		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD2	SEG19		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD3	SEG20		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD4	SEG21		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD5	SEG22		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD6	SEG23		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD7	SEG24		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD8	COM1		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD9	COM2		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD10	COM3		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD11	COM4		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD12	COM5		第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD13	COM6		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD14	RELAY_CTRL	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD15	RELAY_TEST	普通端口 输出 开漏 高
  
  //GPIOE引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	SEC			普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	ICIO		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	MCUCLK		普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE4	METERIC_RX	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE5	METERIC_TX	普通端口 输出 开漏 高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE6	ICRST		普通端口 输出 开漏 高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	485_CTRL	普通端口 输出 开漏 高
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE9				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE10				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE11				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE12				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE13				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE15				无此引脚
};

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//串口波特率配置表，        分别对应300   /600   /1200  /2400  /4800 /7200  /9600  /19200 /38100 /57600 /115200
static const WORD TAB_Baud[]={0x8F5B,0x47AC,0x23D6,0x11EB,0x08F5,0x05F9,0x047A,0x023C,0x011E,0x00BE,0x005F};

static volatile WORD SysTickCounter;
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 系统电源检测
//
//参数: 	
//          Type[in]:	
//				eOnInitDetectPowerMode		上电系统电源检测
//				eOnRunDetectPowerMode		正常运行期间的系统电源检测--只能在大循环中调用,调用时有进入低功耗操作
//				eWakeupDetectPowerMode		低功耗唤醒期间系统电源检测
//				eSleepDetectPowerMode		低功耗休眠期间系统电源检测
//				eOnRunSpeedDetectUnsetFlag	正常运行快速系统电源检测--可在大循环外调用,不置位任何标志,仅做电源检测
//返回值: 	TRUE:有电   FALSE:没有电
//
//备注:   上电vcc和比较器都判，掉电只判LVDIN0比较器，因为vcc上有超级电容
//-----------------------------------------------
BOOL api_CheckSysVol( eDETECT_POWER_MODE Type )
{
	BYTE i,Result;
	
	Result = TRUE;
	
	if( (HT_GPIOE->IOCFG&0x0080) == 0 )
	{
		Result = FALSE;
	}
	
	if( (HT_GPIOE->AFCFG&0x0080) != 0 )
	{
		Result = FALSE;
	}
	
	if( HT_PMU->PMUCON != 0x0007 )
	{
		Result = FALSE;
	}
	
	if( Result == FALSE )
	{		
		HT_GPIOE->IOCFG |= 0x0080;					//复用
		HT_GPIOE->AFCFG &=~0x0080;					//功能1
		EnWr_WPREG();
		HT_PMU->PMUCON  = 0x0007;					//开启LVD_DET,BOR_DET,BOR复位模式
		DisWr_WPREG();
	}
	
	
	HT_PMU->VDETCFG = 0x004D;					//上电比较VCC电压3.3V BOR 2.0伏复位
	HT_PMU->VDETPCFG = 0x003A;		            //推荐配置，影响低功耗上电检测，不可随意更改
	HT_GPIOE->PTOD  |= 0x0080;					//不开漏
	HT_GPIOE->PTUP  |= 0x0080;					//不上拉
	HT_GPIOE->PTDIR &=~0x0080;					//输入
		
	// //低功耗期间要打开PMU中断
	// if( Type == eSleepDetectPowerMode )
	// {
	// 	NVIC_DisableIRQ( PMU_IRQn );                                        	//禁止PMU中断
	// 	HT_PMU->PMUIF	= 0x0000;                                               //清中断标志
	// 	HT_PMU->PMUIE	= 0x0004;                                               //使能VCC,LVD检测中断
	// 	NVIC_ClearPendingIRQ( PMU_IRQn );                                       //清除挂起状态
	// 	NVIC_EnableIRQ( PMU_IRQn );                                             //使能PMU中断
	// }
	// else
	// {
	// 	//禁止PMU中断
	// 	NVIC_DisableIRQ( PMU_IRQn );
	// 	HT_PMU->PMUIF   = 0x0000;
	// 	HT_PMU->PMUIE   = 0x0000; 
	// }
	//禁止PMU中断
	NVIC_DisableIRQ( PMU_IRQn );
	HT_PMU->PMUIF   = 0x0000;
	HT_PMU->PMUIE   = 0x0000; 
	
//	HT_PMU->PMUSTA  = 0x0000;					//清状态标志 该寄存器为只读
	
	if( Type == eOnInitDetectPowerMode ) //上电系统电源检测
	{
		for(i=0; i<50; i++)
		{
			api_Delayms( 1 );
			if( (HT_PMU->PMUSTA&0x05) != 0x05 )
			{
				break;
			}
		}
		if( i >= 50 )//确实上电了
		{
			//置上电标志
			api_SetSysStatus( eSYS_STATUS_POWER_ON );
			return TRUE;
		}
		else
		{
			//清上电标志
			api_ClrSysStatus( eSYS_STATUS_POWER_ON );
			return FALSE;
		}
	}
	else if( Type == eOnRunDetectPowerMode ) //正常运行期间的系统电源检测,只能在大循环中调用,调用时有进入低功耗操作
	{
		if( (HT_PMU->PMUSTA&0x05) == 0x05 )//防止主循环阻塞 有电直接退出
		{
			//置上电标志
			api_SetSysStatus( eSYS_STATUS_POWER_ON );
			return TRUE;
		}
		else
		{
			for(i=0; i<10; i++)
			{
				api_Delayms( 1 );
				
				if( (HT_PMU->PMUSTA&0x05) == 0x05 )
				{
					break;
				}
			}
			if( i >= 10 )//掉电
			{
				//清上电标志
				api_ClrSysStatus( eSYS_STATUS_POWER_ON );
				return FALSE;
			}
			else
			{
				//置上电标志
				api_SetSysStatus( eSYS_STATUS_POWER_ON );
				return TRUE;
			}
		}
	}
	else if( Type == eOnRunSpeedDetectUnsetFlag ) //正常运行快速系统电源检测,可在大循环外调用,不置位任何标志,仅做电源检测
	{
		if( (HT_PMU->PMUSTA&0x05) == 0x05 )//有电直接退出
		{
			return TRUE;
		}
		else
		{
			for(i=0; i<10; i++)
			{
				api_Delay100us( 2 );
				
				if( (HT_PMU->PMUSTA&0x05) == 0x05 )
				{
					break;
				}
			}
			if( i >= 10 )//掉电
			{
				return FALSE;
			}
			else  //有电
			{
				return TRUE;
			}
		}
	}
	else if( Type == eWakeupDetectPowerMode ) //低功耗唤醒期间系统电源检测
	{
		if( (HT_PMU->PMUSTA&0x05) != 0x05 )//没上电
		{
			//清上电标志
			api_ClrSysStatus( eSYS_STATUS_POWER_ON );
			return FALSE;
		}
		else
		{
			for(i=0; i<3; i++)
			{
				api_Delayms( 1 );
				if( (HT_PMU->PMUSTA&0x05) != 0x05 )
				{
					break;
				}
			}
			if( i >= 3 )//确实上电了
			{
				//置上电标志
				api_SetSysStatus( eSYS_STATUS_POWER_ON );
				return TRUE;
			}
			else
			{
				//清上电标志
				api_ClrSysStatus( eSYS_STATUS_POWER_ON );
				return FALSE;
			}
		}
	}
	else //低功耗休眠期间系统电源检测
	{
		if( (HT_PMU->PMUSTA&0x05) != 0x05 )//没上电
		{
			//清上电标志
			api_ClrSysStatus( eSYS_STATUS_POWER_ON );
			return FALSE;
		}
		else
		{
			for(i=0; i<3; i++)
			{
				//api_Delayms( 1 ); 低功耗期间不能这么延时
				api_DelayNop(5);
				if( (HT_PMU->PMUSTA&0x05) != 0x05 )
				{
					break;
				}
			}
			if( i >= 3 )//确实上电了
			{
				//置上电标志
				api_SetSysStatus( eSYS_STATUS_POWER_ON );
				return TRUE;
			}
			else
			{
				//清上电标志
				api_ClrSysStatus( eSYS_STATUS_POWER_ON );
				return FALSE;
			}
		}
	}
}

//---------------------------------------------------------------
//函数功能: GPIO组初始化函数，初始化每组GPIO引脚
//
//参数:		HT_GPIOx：
//					HT_GPIOA..HT_GPIOE
//
//			Type:
//					ePowerOnMode		正常上电进行的管脚初始化
//					ePowerDownMode		低功耗进行的管脚初始化
//					ePowerWakeUpMode	低功耗唤醒进行的管脚初始化
//
//返回值:	无
//
//备注:
//		1、芯片引脚选择 GPIO 功能:
//			若方向寄存器配置为输出, 开漏 OD 功能控制有效, 上拉控制无效
//			若方向寄存器配置为输入, 开漏 OD 功能控制无效, 上拉控制有效
//		2、芯片引脚选择复用功能:
//			复用功能的数字输出引脚都可配开漏功能, 上拉功能配置无效
//			复用功能的数字输入引脚（除 RST/TEST/JTAGWDTEN，这三个引脚内部恒定上拉）都可配上拉功能, 开漏功能无效
//			若配置为模拟输入, 开漏控制和上拉控制都无效
//		3、复用功能为模拟功能: 
//			LVDINx、 LCD_SEG\COM、 POWIN、 ADCINx、 ADCBAT
//			除此之外的其它复用功能为数字功能
//---------------------------------------------------------------
void GPIO_Init( HT_GPIO_TypeDef *HT_GPIOx, ePOWER_MODE Type )
{
	const GPIO_InitTypeDef *GPIO_InitStructure;
	BYTE i = 0;
	WORD HT_NEWAFEN = 0;
	WORD HT_IOCFG = 0, HT_AFCFG = 0;
	WORD HT_PTDIR = 0, HT_PTOD = 0, HT_PTUP = 0;
	WORD HT_PTSET = 0, HT_PTCLR = 0;
	
	if( Type > ePowerWakeUpMode )
	{
		return;
	}
	
	//上电模式
	if( Type == ePowerOnMode )
	{
		if( HT_GPIOx == HT_GPIOA )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOA_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOB )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOB_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOC )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOC_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOD )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOD_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOE )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOE_POSITION];
		}
		else
		{
			return;
		}
	}
	//低功耗模式
	else if( Type == ePowerDownMode )
	{
		if( HT_GPIOx == HT_GPIOA )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOA_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOB )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOB_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOC )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOC_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOD )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOD_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOE )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOE_POSITION];
		}
		else
		{
			return;
		}
	}
	//低功耗唤醒模式
	else
	{
		if( HT_GPIOx == HT_GPIOA )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOA_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOB )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOB_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOC )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOC_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOD )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOD_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOE )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOE_POSITION];
		}
		else
		{
			return;
		}
	}
	
	for(i = 0; i < 16; i++)
	{
		//引脚配置为功能端口
		if (((GPIO_InitStructure + i)->eGPIO_TYPE != eGPIO_TYPE_COMMON) &&
			((GPIO_InitStructure + i)->eGPIO_TYPE != eGPIO_TYPE_NONE))
		{
			//新增复用功能生效
			HT_NEWAFEN |= (WORD)(0x01) << i;
			HT_IOCFG |= (WORD)(0x01) << i;
			//第二复用
			if ((GPIO_InitStructure + i)->eGPIO_TYPE == eGPIO_TYPE_AF_SECOND)
			{
				HT_AFCFG |= (WORD)(0x01) << i;
			}
			//不是第二复用就配置为第一复用
			else
			{
				HT_AFCFG &= ~((WORD)(0x01) << i);
			}
		}
		//引脚配置为GPIO或者引脚不存在
		else
		{
			HT_IOCFG &= ~((WORD)(0x01) << i);
			HT_AFCFG &= ~((WORD)(0x01) << i);
		}
		
		//若为功能端口或GPIO
		if ((GPIO_InitStructure + i)->eGPIO_TYPE != eGPIO_TYPE_NONE)
		{
			//方向配置为输出
			if ((GPIO_InitStructure + i)->eGPIO_DIRECTION == eGPIO_DIRECTION_OUT)
			{
				HT_PTDIR |= (WORD)(0x01) << i;
				//推挽
				if ((GPIO_InitStructure + i)->eGPIO_MODE == eGPIO_MODE_PP)
				{
					HT_PTOD |= (WORD)(0x01) << i;
				}
				//不是推挽就配置为开漏
				else
				{
					HT_PTOD &= ~((WORD)(0x01) << i);
				}
				
				//若为GPIO时，输出高低电平
				if ((GPIO_InitStructure + i)->eGPIO_TYPE == eGPIO_TYPE_COMMON)
				{
					//输出高电平
					if ((GPIO_InitStructure + i)->eGPIO_OUTPUT == eGPIO_OUTPUT_HIGH)
					{
						HT_PTSET |= (WORD)(0x01) << i;
						HT_PTCLR &= ~((WORD)(0x01) << i);
					}
					//输出低电平
					else if ((GPIO_InitStructure + i)->eGPIO_OUTPUT == eGPIO_OUTPUT_LOW)
					{
						HT_PTSET &= ~((WORD)(0x01) << i);
						HT_PTCLR |= (WORD)(0x01) << i;
					}
					//不是高电平或者低电平, 默认状态
					else
					{
						HT_PTSET &= ~((WORD)(0x01) << i);
						HT_PTCLR &= ~((WORD)(0x01) << i);
					}
				}
				//若为功能端口时, 默认状态
				else
				{
					HT_PTSET &= ~((WORD)(0x01) << i);
					HT_PTCLR &= ~((WORD)(0x01) << i);
				}
			}
			//方向配置为输入
			else if ((GPIO_InitStructure + i)->eGPIO_DIRECTION == eGPIO_DIRECTION_IN)
			{
				HT_PTDIR &= ~((WORD)(0x01) << i);
				//浮空
				if ((GPIO_InitStructure + i)->eGPIO_MODE == eGPIO_MODE_FLOATING)
				{
					HT_PTUP |= (WORD)(0x01) << i;
				}
				//不是浮空就配置为上拉
				else
				{
					HT_PTUP &= ~((WORD)(0x01) << i);
				}
			}
			//方向不是输出或者输入, 功能端口配置为开漏输出; GPIO配置为开漏输出, 输出高
			else
			{
				HT_PTDIR |= (WORD)(0x01) << i;
				HT_PTOD &= ~((WORD)(0x01) << i);
				//若为GPIO, 输出高电平
				if ((GPIO_InitStructure + i)->eGPIO_TYPE == eGPIO_TYPE_COMMON)
				{
					HT_PTSET |= (WORD)(0x01) << i;
					HT_PTCLR &= ~((WORD)(0x01) << i);
				}
				//若为功能端口, 默认状态
				else
				{
					HT_PTSET &= ~((WORD)(0x01) << i);
					HT_PTCLR &= ~((WORD)(0x01) << i);
				}
			}
		}
	}
	HT_GPIOx->NEWAFEN = HT_NEWAFEN;
	HT_GPIOx->IOCFG = HT_IOCFG;
	HT_GPIOx->AFCFG = HT_AFCFG;
	HT_GPIOx->PTDIR = HT_PTDIR;
	HT_GPIOx->PTOD = HT_PTOD;
	HT_GPIOx->PTUP = HT_PTUP;
	HT_GPIOx->PTSET = HT_PTSET;
	HT_GPIOx->PTCLR = HT_PTCLR;
}

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
	if( Type > ePowerWakeUpMode )
	{
		return;
	}
	
	GPIO_Init(HT_GPIOA, Type);		//初始化GPIOA
	GPIO_Init(HT_GPIOB, Type);		//初始化GPIOB
	GPIO_Init(HT_GPIOC, Type);		//初始化GPIOC
	GPIO_Init(HT_GPIOD, Type);		//初始化GPIOD
	GPIO_Init(HT_GPIOE, Type);		//初始化GPIOE
}


#if(LCD_DRIVE_CHIP == LCD_CHIP_NO)
BYTE TimerLcdTmp;

//低功耗下电压与LCD对比度，小于对应电压采用对应的对比度
const WORD LowPowerLCDVRSEL[2][7] = { {320,    336,    360,    390,    410,    430,    430   }, 
									  {0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006} };

//-----------------------------------------------
//函数功能: 自动调节液晶对比度
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   此函数仅对采取VRTC和VCC短接方案的表型有效！
//-----------------------------------------------
// 为了第一次掉电后唤醒，液晶对比度过大导致显示不清的问题！
static void AutoAdjustContrast(void)
{
	BYTE i;
	SWORD Vcc = 0;

	Vcc = GetADValue(0x55,SYS_POWER_AD);

	if( TimerLcdTmp > 5 )
	{
		TimerLcdTmp = 0;
	}
	
	if(TimerLcdTmp > 0)
	{
		TimerLcdTmp--;
		if (Vcc > 430)
		{
			HT_LCD->LCDCON = 0x0001;//对比度=92.4%
		}
		else
		{
			HT_LCD->LCDCON = 0x0000;//对比度=100%VLCD
		}
	}
	else
	{
		for(i = 0; i < 6; i++)
		{
			if(Vcc > LowPowerLCDVRSEL[0][5])
			{
				HT_LCD->LCDCON = LowPowerLCDVRSEL[1][6];
				break;
			}
			else if(Vcc <= LowPowerLCDVRSEL[0][i] )
			{
				HT_LCD->LCDCON = LowPowerLCDVRSEL[1][i];
				break;
			}
		}

		if (i >= 6)
		{
			HT_LCD->LCDCON = LowPowerLCDVRSEL[1][0];
		}
	}

}

//-----------------------------------------------
//函数功能: 初始化液晶驱动芯片
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void InitLCDDriver(void)
{
    EnWr_WPREG();
    HT_CMU->CLKCTRL0 |= 0x000002;
	//LCD电压
	HT_LCD->LCDCLK = 0x008C;//1/3偏压,6COM,85.3Hz
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
	{
		#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303))
		//低 功 耗 期 间 单 片 机 供 电 会 在 5V 一 3V 间 变 化 ， 需 要 在 高 电 压 下 无 重 影
		AutoAdjustContrast();//自动调节液晶对比度。
		#else
		HT_LCD->LCDCON = 0x0000;//对比度=100%VLCD
		#endif
	}
	else
	{
		//lcd电压对比度
		HT_LCD->LCDCON = 0x0000;//对比度=99%VLCD
	}

    DisWr_WPREG();
}
#endif//#if(LCD_DRIVE_CHIP == LCD_CHIP_NO)


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
	InitExtInt();
}


//-----------------------------------------------
//函数功能: 外部中断配置
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   hplc-电动车-中断4：无功---中断5：有功
//-----------------------------------------------
static void InitExtInt(void)
{
	NVIC_DisableIRQ(EXTI4_IRQn);
	NVIC_DisableIRQ(EXTI5_IRQn);
	HT_INT->EXTIE = 0;
	HT_INT->EXTIE |= 0x3030; //使能上升沿中断
	HT_INT->PINFLT |= 0x0030; //数字滤波
	HT_INT->EXTIF = 0;
	NVIC_EnableIRQ(EXTI4_IRQn);
	NVIC_EnableIRQ(EXTI5_IRQn);
}


//-----------------------------------------------
//函数功能: 对CPU的AD进行配置
//
//参数: 	Type[in]:ePowerOnMode/ePowerWakeUpMode
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_AdcConfig( ePOWER_MODE Type )
{
	/*
   	//使能VCC、ADC1(时钟电池)、温度测量
	if (HT_TBS->TBSCON	!= 0x655D)		{HT_TBS->TBSCON	 = 0x655D;}			
	//禁止TBS中断
	if (HT_TBS->TBSIE	!= 0x0000)		{HT_TBS->TBSIE	 = 0x0000;}			
	//电池电压测量周期=1s;温度测量周期=1/2s
	if (HT_TBS->TBSPRD	!= 0x0000)		{HT_TBS->TBSPRD	 = 0x0000;}			
	//pdf上没有TBSTEST这个寄存器说明，张国办说默认写0x0200
	//TBS_ADC电流=1uA;ADC_ldo电压=2.7V;
	if (HT_TBS->TBSTEST	!= 0x0200)		{HT_TBS->TBSTEST = 0x0200;}	*/
	
	if( HT_TBS->VBATCMP != 0x0000 )
	{
		HT_TBS->VBATCMP = 0x0000;
	}

	if( HT_TBS->ADC0CMP != 0x0000 )
	{
		HT_TBS->ADC0CMP = 0x0000;
	}

	if( HT_TBS->TRIREQ != 0x0000 )
	{
		HT_TBS->TRIREQ = 0x0000;
	}

	if( HT_TBS->TBSTESTCTRL != 0x0000 )
	{
		HT_TBS->TBSCON |= 0x800000; //关闭写保护
		HT_TBS->TBSTESTCTRL = 0x0000;
		HT_TBS->TBSCON &= (~0x800000); //打开写保护
	}

	if( HT_TBS->TBSINJECTCTRL != 0x0500 )
	{
		HT_TBS->TBSCON |= 0x800000; //关闭写保护
		HT_TBS->TBSINJECTCTRL = 0x0500;
		HT_TBS->TBSCON &= (~0x800000); //打开写保护
	}

	if( Type == ePowerOnMode )
	{
		//使能VCC、时钟电池、温度测量
		if( HT_TBS->TBSCON	!= 0x14553 )  //6553
		{
			HT_TBS->TBSCON	 = 0x14553;
		}
		//禁止TBS中断
		if( HT_TBS->TBSIE	!= 0x0000 )
		{
			HT_TBS->TBSIE	 = 0x0000;
		}
		//电池电压测量周期=2s;温度测量周期=1/2s
		if( HT_TBS->TBSPRD	!= 0x0008 )
		{
			HT_TBS->TBSPRD	 = 0x0008;
		}
		//pdf上没有TBSTEST这个寄存器说明，张国办说默认写0x0200
		//TBS_ADC电流=1uA;ADC_ldo电压=2.7V;
		if( HT_TBS->TBSTEST != 0x0200 )
		{
			HT_TBS->TBSTEST = 0x0200;
		}
	}
	else if( Type == ePowerWakeUpMode )
	{
		//温度测量,Vcc
		if( HT_TBS->TBSCON	!= 0x6551 )
		{
			HT_TBS->TBSCON	 = 0x6551;
		}
		//禁止TBS中断
		if( HT_TBS->TBSIE	!= 0x0000 )
		{
			HT_TBS->TBSIE	 = 0x0000;
		}
		//温度测量周期=64s VCC测量周期1s
		if( HT_TBS->TBSPRD	!= 0x0007 )
		{
			HT_TBS->TBSPRD	 = 0x0007;
		}
		//pdf上没有TBSTEST这个寄存器说明，张国办说默认写0x0200
		//TBS_ADC电流=1uA;ADC_ldo电压=2.7V;
		if( HT_TBS->TBSTEST != 0x0200 )
		{
			HT_TBS->TBSTEST = 0x0200;
		}
	}
}


//-----------------------------------------------
//函数功能: 用于检查维护cpu的寄存器值
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void Maintain_MCU( void )
{
	//TBS 类
	api_AdcConfig(ePowerOnMode);
}


//-----------------------------------------------
//函数功能: 获取AD转换的结果，小数点位数是固定的，如果小数点不合适，需要调用者自己转换
//
//参数:
//				BYTE Mode[in]		基准电压是否校准 0x00 校准 0x55不校准
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
    long Voltage;
    short ReturnValue;
    
    switch( Type )
    {
        case SYS_POWER_AD://系统电压 保留二位小数 单位v
            //计算公式 VCC = 0.1798*VCCDAT+ 0.5943 单位mV
            if( HT_TBS->VCCDAT & 0x8000 )//负数
            {
                Voltage = 0;
            }
            else
            {
                Voltage = HT_TBS->VCCDAT;
				Voltage *= VcccoffConst_K;
				Voltage += VccOffsetConst_K;
				Voltage /= 1000000;//单位V，保留2位小数

				if( Voltage < 0 )
				{
					Voltage = 0;
				}
            }           
            break;
        case SYS_READ_VBAT_AD://停电抄表电池电压
        case SYS_CLOCK_VBAT_AD://时钟电池电压 保留二位小数
            //计算公式 VADCIN1 = 0.0258*ADC1DAT+4.7559 单位mV
			//计算公式 VADCIN4 = 0.0258*ADC1DAT+4.7559 单位mV
            // if( HT_TBS->VBATDAT & 0x8000 )//负数
            // {
            //     Voltage = 0;				
            // }
			if( HT_TBS->ADC4DAT & 0x8000 )//负数
            {
                Voltage = 0;				
            }
            else
            {
				//系数可能不对待定!!!!!!
				//Voltage = HT_TBS->VBATDAT;
				Voltage = HT_TBS->ADC4DAT;
				Voltage *= VBatcoffConst_K;
				Voltage += VBatOffsetConst_K; 
				Voltage /= 1000000L; 
				
				
				if(abs(BatteryPara.ClockBatteryOffset) < (BYTE)(ClockBatteryStandardVoltage*15/100) )//校准偏置，允许最大值0.5V 依据5%的电阻误差，2个电阻最大误差10%，按照15%进行考虑 364*0.12= 43
				{
					Voltage += BatteryPara.ClockBatteryOffset;
				}
				
				if( Voltage >= 368 )
				{
					Voltage = 368;
				}

				if( Voltage < 0 )
				{
					Voltage = 0;
				}
            }           
            break;
        case SYS_TEMPERATURE_AD://温度寄存器 保留一位小数
            //计算公式V = 12.9852 - 0.0028*TMPDAT
            ReturnValue = ((short)HT_TBS->TMPDAT-(short)HT_RTC->Toff);
            Voltage = ((long)ReturnValue * TPScoffConst_K);
            Voltage += TPSOffsetConst_K;
            Voltage /= 10000;
            break;
        default:
            Voltage = 0;
            break;
    }

    ReturnValue = (short)Voltage;
    
    return ReturnValue;
}



//-----------------------------------------------
//函数功能: 采样芯片URAT读取采样芯片一个字节
//
//参数:		无 
//                    
//返回值: 
//		   返回读取的数据，超时返回0
//备注:   
//-----------------------------------------------
BYTE Drv_SampleReadByte(void)
{	
	WORD SampleComTmr;
	
	SampleComTmr = 0;
	
	while(SampleComTmr < 210)
	{
		if(EMU_UARTSTA & UART_UARTSTA_RXIF)	
		{
			EMU_UARTSTA &= ~UART_UARTSTA_RXIF;
	
			return EMU_UARTBUF;
		}
		
		api_Delay100us( 1 );
		SampleComTmr++;
	}
	
	return 0;
}


//-----------------------------------------------
//函数功能: 采样芯片URAT写入采样芯片一个字节
//
//参数:		Data[in] 要写到采样芯片的数据 
//                    
//返回值: 无
//		   
//备注:   
//-----------------------------------------------
void Drv_SampleWriteByte(BYTE Data)
{	
	WORD SampleComTmr;
	
	SampleComTmr = 0;
	
	while(SampleComTmr < 210)
	{
		if(EMU_UARTSTA & UART_UARTSTA_TXIF)
		{
			EMU_UARTSTA &= ~UART_UARTSTA_TXIF;
	
			EMU_UARTBUF = Data;
			
			return;
		}
		
		api_Delay100us( 1 );
		SampleComTmr++;
	}
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
		//打开LCD电源
		POWER_LCD_OPEN;
		POWER_SAMPLE_OPEN;//一定要把操作5v电源放在操作其他电源的最前面
		POWER_FLASH_OPEN;
	    POWER_HALL_OPEN;
		//增加是否使用ESAM的电源控制
	    POWER_ESAM_OPEN;

		#if (SEL_F415 == YES)
		POWER_F415_CV_OPEN;
		#else
		//不使用415的情况在给其关闭命令，注意低配版gpio的引脚配置
		POWER_F415_CV_CLOSE;
		#endif
	}
}

//--------------------------------------------------
//功能描述:  关闭esam电源
//         
//参数  : ePOWER_MODE  POWER_MODE 不起作用与三相表兼容
//
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void ESAMSPIPowerDown( ePOWER_MODE  POWER_MODE )
{
	//PB6	ESAM-CS2
	//PB7	ESAM-CLK2
	//PB8	ESAM-SO2
	//PB9	ESAM-SI2
	//端口功能配置
	HT_GPIOB->IOCFG &= (~(GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9));
	//端口方向
	HT_GPIOB->PTDIR |= (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	//推挽
	HT_GPIOB->PTOD |= (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	//初始为低
	HT_GPIOB->PTCLR = (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	POWER_ESAM_CLOSE;//关闭ESAM电源
	
	api_Delayms(5);
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
	if( Type >= eCOMPONENT_TOTAL_NUM )
	{
		return FALSE;
	}
	
	api_Delayms(5);
	
	if( (Type == eCOMPONENT_SPI_ESAM )  ) //PB6  关闭ESAM CS
	{
		//PB6	ESAM-CS   

		//PB7	ESAM-CLK2
		//PB8	ESAM-SO2
		//PB9	ESAM-SI2
		//端口功能配置
		HT_GPIOB->IOCFG &= (~(GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9));
		//端口方向
		HT_GPIOB->PTDIR |= (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
		//推挽
		HT_GPIOB->PTOD |= (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
		//初始为低
		HT_GPIOB->PTCLR = (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
		
		POWER_ESAM_CLOSE;//重新打开电源
		api_Delayms(20);
		POWER_ESAM_OPEN;
		api_Delayms(Time);

		//CS拉高
		HT_GPIOB->PTSET |= 	GPIO_Pin_6;

		//MISO配置为禁止上拉(浮空)
		HT_GPIOB->PTDIR &= 	(~GPIO_Pin_8);
		HT_GPIOB->PTUP |= GPIO_Pin_8; 

		//api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 ); 
		api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
		//SPI引脚配置为功能引脚
		//PB7           SCLK             	//普通端口 输出 推挽 初始为高
		//PB8           MISO            	//普通端口 输入 上拉
		//PB9           MOSI           		//普通端口 输出 推挽 初始为高
		HT_GPIOB->NEWAFEN |= GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		HT_GPIOB->IOCFG |= 	GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		HT_GPIOB->AFCFG |=  GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; 
	}
	else if( Type == eCOMPONENT_SPI_SAMPLE )
	{
		//PC4	SAMPLE_SI
		//PC5	SAMPLE_SO
		//PC6	SAMPLE_CLK
		//PD14	SAMPLE_CS
		//端口功能配置
		HT_GPIOC->IOCFG &= 	(~(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)); 
		HT_GPIOD->IOCFG &= 	(~(GPIO_Pin_14)); 
		
		//开漏
		// //Flash带上拉 CPU端配置为开漏
		// HT_GPIOC->PTOD &= (~(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)); 
				
		//推挽
		//推挽输出，pc5浮空输入
		HT_GPIOC->PTOD |= ((GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)); 
		HT_GPIOD->PTOD |= 	((GPIO_Pin_14)); 
	
		//初始为低
		HT_GPIOC->PTCLR |= 	GPIO_Pin_4|
							GPIO_Pin_5|
							GPIO_Pin_6;
		HT_GPIOD->PTCLR |= 	GPIO_Pin_14;

		
		POWER_SAMPLE_CLOSE;//重新打开电源
		api_Delayms(Time);
		POWER_SAMPLE_OPEN;
		api_Delayms(Time);
		//CS初始为高
		HT_GPIOD->IOCFG  &= 0xbfff; //重新配置CS 
		// HT_GPIOD->PTSET |=  GPIO_Pin_14;		
		HT_GPIOD->PTDIR  |=	GPIO_Pin_14;
		HT_GPIOD->PTOD   |=	GPIO_Pin_14;
		HT_GPIOD->PTCLR  |= GPIO_Pin_14;
		HT_GPIOD->PTSET |=  GPIO_Pin_14;

		HT_GPIOC->PTDIR |=	GPIO_Pin_4|
							GPIO_Pin_6;
		
		//MISO配置为输入
		HT_GPIOC->PTDIR &= 	(~GPIO_Pin_5);
		//禁止上拉 防止CPU给Flash灌电压
		HT_GPIOC->PTUP |= GPIO_Pin_5; 
		api_Delayms(Time);
		api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 ); 
		
		//SPI配置为功能引脚
		HT_GPIOC->IOCFG |= 	GPIO_Pin_4|
							GPIO_Pin_5|
							GPIO_Pin_6;
	}
	else if( Type == eCOMPONENT_SPI_FLASH )
	{
		//PB9	FLASH_SI    	//普通端口 输出 推挽 初始为低
		//PB8	FLASH_MISO
		//PB7	FLASH_CLK		推挽输出 低
		//PC7	FLASH_CS		
		//端口功能配置
		HT_GPIOB->IOCFG &= 	(~(GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9)); 
		HT_GPIOC->IOCFG &= 	(~(GPIO_Pin_7)); 
		
		//推挽
		HT_GPIOB->PTOD |= (GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9); 
		HT_GPIOC->PTOD |= (GPIO_Pin_7); 
		//初始为低
		HT_GPIOB->PTCLR |= 	GPIO_Pin_7|
							GPIO_Pin_8|
							GPIO_Pin_9;
		HT_GPIOC->PTCLR |= 	GPIO_Pin_7;
		
		POWER_FLASH_CLOSE;//重新打开电源
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		//CS初始为高
        HT_GPIOC->IOCFG  &= 0xff7f; //重新配置CS   					
		HT_GPIOC->PTDIR  |=	GPIO_Pin_7;
		HT_GPIOC->PTOD   |=	GPIO_Pin_7;
		HT_GPIOC->PTCLR  |= GPIO_Pin_7;
		HT_GPIOC->PTSET |=  GPIO_Pin_7;

		HT_GPIOB->PTDIR |=	GPIO_Pin_7|
							GPIO_Pin_9;
		//MISO配置为输入
		HT_GPIOB->PTDIR &= 	(~GPIO_Pin_8);
		//禁止上拉 防止CPU给Flash灌电压
		HT_GPIOB->PTUP |= GPIO_Pin_8; 
		api_Delayms(Time);
		api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 ); 
		//SPI配置为功能引脚
		HT_GPIOB->IOCFG |= 	GPIO_Pin_7|
							GPIO_Pin_8|
							GPIO_Pin_9;
		HT_GPIOB->AFCFG |= 	GPIO_Pin_7|
							GPIO_Pin_8|
							GPIO_Pin_9;
		HT_GPIOB->NEWAFEN |= GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	}
	
	api_Delayms(5);
	
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
	
	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
    CLEAR_WATCH_DOG;
	
	InitPort( ePowerOnMode );//正常初始化
	
	//端口初始化
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 100 ); //对采样芯片进行复位
	ResetSPIDevice( eCOMPONENT_SPI_ESAM, 100 ); //对esam进行复位
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 ); //对Flash进行复位   
    
	PowerCtrl( 0 );
	
	ESAMSPIPowerDown( ePowerOnMode );//关闭单相ESAM电源
	
	api_AdcConfig(ePowerOnMode);
	
	InitINT() ;//关闭不用的中断
	
	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	//各个模块上电初始化，注意不要随便调顺序
	api_PowerUpFlag();
	
	//api_PowerUpRtc有对RTC的复位操作 复位后必须重新配置RTC和TBS相关寄存器 因此需要重新调用一次ADC配置
	api_PowerUpRtc();
	api_AdcConfig(ePowerOnMode); //!!!!特别注意 此处Aapi_AdcConfig不能删 - yxk
	
	api_WriteFreeEvent(EVENT_SYS_START, HT_PMU->RSTSTA);
	if((HT_PMU->RSTSTA&0x0005)==0x0004)//看门狗复位记录异常事件
	{
		api_WriteSysUNMsg( SYSERR_WDTRST_ERR );
	}
	HT_PMU->RSTSTA = 0;
	
	api_PowerUpPara();
	api_PowerUpSave();

	api_PowerUpVersion();
	
	api_PowerUpLcd( ePowerOnMode );
	
	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	api_PowerUpSample();

	api_PowerUpProtocol();
	
	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	api_PowerUpPrePay();
	PowerUpRelay();
	
	api_PowerUpEnergy();
    api_PowerUpFreeze();
	
	//若上电补冻过程中掉电，尽快进入低功耗
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	api_ReflashFreeze_Ram();
    api_PowerUpEvent();
    api_PowerUpSysWatch();//先刷新主动上报模式字后进行电池电压等的相关判断-与event上电处理函数有顺序关系-姜文浩
    	
	#if( MAX_PHASE == 3 )
	api_PowerUpDemand();
	#endif
	
	api_PowerUpVersion();
	
	//系统电压自检，确保系统电压为确定状态
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnRunEnterDownMode );//在此之前上电已初始化完成 此处检测到低功耗按正常掉电处理 用于解决进入掉电但认为是异常复位的问题
	}
	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		SerialMap[i].SCIInit(SerialMap[i].SCI_Ph_Num);
	}

	#if(SEL_F415 == YES)
	api_PowerUpCollect();
	#endif

	//初始化_noinit_标志
	api_InitNoInitFlag();
	
	RESET_PLC_MODUAL;
    api_Delayms(20);
	ENABLE_PLC_PROGRAMM;
}


//-----------------------------------------------
//函数功能: SPI片选驱动函数
//
//参数:		
//		No[in]		需要选择的芯片
// 			CS_SPI_FLASH    - FLASH
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
	if( No == CS_SPI_FLASH )
	{
		FLASH_CS_DISABLE;
	}
	else if( No == CS_SPI_SAMPLE )
	{
		SAMPLE_CS_DISABLE;
	}
	else
	{
		ESAM_CS_DISABLE; 
	}
	
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


//-----------------------------------------------
//函数功能: 串口初始化函数
//
//参数:		SciPhNum[in]		需要初始化的串口物理编号
//
//返回值: 	TRUE：正确初始化   FALSE:没有正确初始化
//		   
//备注:   
//-----------------------------------------------
static BOOL InitPhSci(BYTE SciPhNum)
{
    BYTE BpsBak, IntStatus, Bps, i;
    HT_UART_TypeDef *USARTx;
	IRQn_Type UARTx_IRQn;

    if(SciPhNum == 0xff)
    {
        return TRUE;
    }

	USARTx = NULL;
	BpsBak = 0;
	BpsBak = api_GetBaudRate( 0, SciPhNum ); 

    switch(SciPhNum)
    {
        case 0:	//载波
            USARTx = HT_UART0;
			UARTx_IRQn = UART0_IRQn;
            NVIC_DisableIRQ(UART0_IRQn);                //禁止串口中断
            NVIC_ClearPendingIRQ(UART0_IRQn);           //清除挂起状态
            NVIC_SetPriority(UART0_IRQn, 3);            //设置优先级
            //NVIC_EnableIRQ(UART0_IRQn);                 //使能串口中断
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000010;               //开启串口时钟
            DisWr_WPREG();
            break;
        case 1:	//415
            USARTx = HT_UART1;
			UARTx_IRQn = UART1_IRQn;
            NVIC_DisableIRQ(UART1_IRQn);                //禁止串口中断
            NVIC_ClearPendingIRQ(UART1_IRQn);           //清除挂起状态
            NVIC_SetPriority(UART1_IRQn, 3);            //设置优先级
            //NVIC_EnableIRQ(UART1_IRQn);                 //使能串口中断
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000020;               //开启串口时钟
            #if( COMM_TYPE == COMM_NEAR )||(( COMM_TYPE == COMM_NOAR ))
            HT_UART1->IRCON   = 0x0000;                   //红外不调制
            HT_UART1->IRDUTY  = 0x0000;                   //50%占空比
            #else
            HT_UART1->IRCON   = 0x0001;                   //红外调制
            HT_UART1->IRDUTY  = 0x0001;                   //25%占空比
            #endif
            DisWr_WPREG();
            break;
		case 2:	//485
			if( (BpsBak&0x0F) == 10 ) //115200波特率关闭滤波功能
			{
				HT_GPIOC->FILT &= (~(1 << 12));               //关闭滤波功能
			}
			else
			{
				HT_GPIOC->FILT |= (1 << 12);               //开启滤波功能
			}
			USARTx = HT_UART2;
			UARTx_IRQn = UART2_IRQn;
            NVIC_DisableIRQ(UART2_IRQn);                //禁止串口中断
            NVIC_ClearPendingIRQ(UART2_IRQn);           //清除挂起状态
            NVIC_SetPriority(UART2_IRQn, 3);            //设置优先级
            //NVIC_EnableIRQ(UART2_IRQn);                 //使能串口中断
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000040;               //开启串口时钟
            DisWr_WPREG();
            break;
        case 3://蓝牙
			USARTx = HT_UART3;
			UARTx_IRQn = UART3_IRQn;
            NVIC_DisableIRQ(UART3_IRQn);                //禁止串口中断
            NVIC_ClearPendingIRQ(UART3_IRQn);           //清除挂起状态
            NVIC_SetPriority(UART3_IRQn, 3);            //设置优先级
            NVIC_EnableIRQ(UART3_IRQn);                 //使能串口中断
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x0080;                 //开启串口时钟
            DisWr_WPREG();
            break;
        /*
        case 4:
			USARTx = HT_UART4;
			UARTx_IRQn = UART4_IRQn;
            NVIC_DisableIRQ(UART4_IRQn);                //禁止串口中断
            NVIC_ClearPendingIRQ(UART4_IRQn);           //清除挂起状态
            NVIC_SetPriority(UART4_IRQn, 3);            //设置优先级
            NVIC_EnableIRQ(UART4_IRQn);                 //使能串口中断
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x0100;                 //开启串口时钟
            DisWr_WPREG();
            break;
        */
        default:
            USARTx = NULL;
            break;
    }

    if(USARTx == NULL)
    {
        return FALSE;
    }

    USARTx->MODESEL = 0x0000;//UART功能（非7016功能）
    USARTx->UARTSTA = 0x0000;//清除中断标志位
    USARTx->UARTCON = 0x0000;//清控制寄存器
    //USARTx->IRCON   = 0x0000;//非调制
    //BpsBak = api_GetBaudRate(0, SciPhNum);
    //IntStatus = api_splx(0);
	Bps = BpsBak&0x0F;
	if( Bps >= (sizeof(TAB_Baud) / sizeof(TAB_Baud[0])) )
	{
		Bps = 6;    	//如果数据错误 默认为9600
		BpsBak = 0x46;
	}
	USARTx->SREL = TAB_Baud[Bps];

    //取停止位
    Bps = ((BpsBak>>4) & 0x01);

    switch(Bps)
    {
        case 0x00://1位停止位
            USARTx->UARTCON &= ~0x0100;
            break;
        case 0x01://2位停止位
            USARTx->UARTCON |= 0x0100;
            break;
        default://默认1位停止位
            USARTx->UARTCON &= ~0x0100;
            break;
    }
    //取校验
	Bps = ((BpsBak >> 5)&0x03);
    switch(Bps)
    {            
        case 0x00://无校验
            USARTx->UARTCON &= ~0x0070;
            break;
        case 0x01://允许校验，奇校验
            USARTx->UARTCON &= ~0x0070;
            USARTx->UARTCON |= 0x0030;
            break;
        case 0x02://允许校验，偶校验
            USARTx->UARTCON &= ~0x0070;
            USARTx->UARTCON |= 0x0050;//2016.8.3确定0x0050就是偶校验，矩泉的就是
            break;
        default://默认允许校验，偶校验
            USARTx->UARTCON &= ~0x0070;
            USARTx->UARTCON |= 0x0050;
            break;
    }
    
    USARTx->UARTCON |= 0x0008; //接收中断使能
    USARTx->UARTCON |= 0x0002; //接收使能
    SciRcvEnable(SciPhNum);

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SciPhNum == SerialMap[i].SCI_Ph_Num)
        {
            api_InitSubProtocol(i);
            //串口监视定时器
            Serial[ i ].WatchSciTimer = WATCH_SCI_TIMER;
            break;
        }
    }
	NVIC_EnableIRQ(UARTx_IRQn);                 //使能串口中断
    //api_splx(IntStatus);
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
    switch(SciPhNum)
    {
        case 0:
            ENABLE_HARD_SCI_0_RECE;  
            //api_Delay100us(1);// 延时100us 发送转接收 延时无意义 而且会影响高速通信
            HT_UART0->UARTCON |= 0x000A;
            break;
        case 1:
            ENABLE_HARD_SCI_1_RECE;
            //api_Delay100us(1);// 延时100us
            HT_UART1->UARTCON |= 0x000A;
            break;
        case 2:
            ENABLE_HARD_SCI_2_RECE;
            //api_Delay100us(1);// 延时100us
            HT_UART2->UARTCON |= 0x000A;
            break;
        case 3:
            ENABLE_HARD_SCI_3_RECE;
            //api_Delay100us(1);// 延时100us
            HT_UART3->UARTCON |= 0x000A;
            break;		
        // case 4:
        //     ENABLE_HARD_SCI_4_RECE;
        //     //api_Delay100us(1);// 延时100us
        //     HT_UART4->UARTCON |= 0x000A;
        //     break;
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
    BYTE i = 0;

    switch(SciPhNum)
    {
        case 0:
            HT_UART0->UARTCON &= ~0x000A;//禁止接收，禁止接收中断
            break;
        case 1:
            HT_UART1->UARTCON &= ~0x000A;
            break;
        case 2:
            HT_UART2->UARTCON &= ~0x000A;
            break;
        case 3:
            HT_UART3->UARTCON &= ~0x000A;
            break;
        // case 4:
        //     HT_UART4->UARTCON &= ~0x000A;
        //     break;
        default:
            break;
    }

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SciPhNum == SerialMap[i].SCI_Ph_Num)
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
    switch(SciPhNum)
    {
        case 0:
            ENABLE_HARD_SCI_0_SEND;//
            api_Delay100us(1);          // 延时100us
            break;
        case 1:
            ENABLE_HARD_SCI_1_SEND;
            api_Delay100us(1);          // 延时100us
            break;
        case 2:
            ENABLE_HARD_SCI_2_SEND;
            api_Delay100us(1);          // 延时100us
            break;
        case 3:
            ENABLE_HARD_SCI_3_SEND;
            api_Delay100us(1);          // 延时100us
            break;
        // case 4:
        //     ENABLE_HARD_SCI_4_SEND;
        //     api_Delay100us(1);          // 延时100us
        //     break;
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

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SciPhNum == SerialMap[i].SCI_Ph_Num)
        {
            if(Serial[i].TXPoint == 0)
            {
                switch(SciPhNum)
                {
                    case 0://載波
                        HT_UART0->UARTCON |= 0x0001; //发送使能
                        HT_UART0->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
                        HT_UART0->UARTCON |= 0x0004; //发送中断使能
                        NVIC_EnableIRQ(UART0_IRQn);//开中断
                        break;
                    case 1://415uart
                        HT_UART1->UARTCON |= 0x0001; //发送使能
                        HT_UART1->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
                        HT_UART1->UARTCON |= 0x0004; //发送中断使能
                        NVIC_EnableIRQ(UART1_IRQn);//开中断
                        break;
                    case 2://485
                        HT_UART2->UARTCON |= 0x0001; //发送使能
                        HT_UART2->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
                        HT_UART2->UARTCON |= 0x0004; //发送中断使能
                        NVIC_EnableIRQ(UART2_IRQn);//开中断
                        break;
                    case 3:
                        HT_UART3->UARTCON |= 0x0001; //发送使能
                        HT_UART3->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
                        HT_UART3->UARTCON |= 0x0004; //发送中断使能
                        NVIC_EnableIRQ(UART3_IRQn);//开中断
                        break;
                    // case 4:
                    //     HT_UART4->UARTCON |= 0x0001; //发送使能
                    //     HT_UART4->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
                    //     HT_UART4->UARTCON |= 0x0004; //发送中断使能
                    //     NVIC_EnableIRQ(UART4_IRQn);//开中断
                    //     break;
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
//函数功能: 开启计量模块的串口、初始化串口、发送首字节
//
//参数:		head[in]		要发送的第一个字节
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void Open_EMUUART(BYTE Head)
{
	BYTE Result = TRUE;
	
	if( (HT_CMU->CLKCTRL1&0x000080) == 0 )
	{
		Result = FALSE;
	}
	if( (HT_GPIOE->IOCFG&0x0030) == 0 )
	{
		Result = FALSE;
	}
	if( (HT_GPIOE->AFCFG&0x0030) != 0 )
	{
		Result = FALSE;
	}
	
	if( Result == FALSE )
	{
		EnWr_WPREG();
	    HT_CMU->CLKCTRL1 |= 0x000080;                 //开启串口时钟
		DisWr_WPREG();
	    HT_GPIOE->IOCFG |= 0x0030;
	    HT_GPIOE->AFCFG &= ~0x0030;	    
	    api_Delay100us(2);
	}
		
	NVIC_DisableIRQ(UART3_IRQn);				//禁止串口中断
	HT_UART3->MODESEL = 0x0000;					//UART功能
	HT_UART3->UARTSTA = 0x0000;					//清除中断标志位
	HT_UART3->UARTCON = 0x0053;					//正逻辑,8位数据,偶校验,1停止位,发送,接收使能
	HT_UART3->IRCON   = 0x0000;					//非调制
	HT_UART3->SREL    = TAB_Baud[4];			//波特率:4800bps
	HT_UART3->SBUF    = Head;
}


//-----------------------------------------------
//函数功能: 关闭计量模块的串口
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void Close_EMUUART(void)
{
	NVIC_DisableIRQ(UART3_IRQn);				//禁止串口中断
	HT_UART3->UARTCON = 0x0000;					//禁止串口功能
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
//单相表
//	FALSH - SPI0
//  ESAM  - SPI3
//-----------------------------------------------
void api_InitSPI( eCOMPONENT_TYPE Component, eSPI_MODE Type )
{
	//0x00bX，实测1.4M左右
	//0x00cX，实测680K左右
	//0x00aX，计算2.8M左右
	//0x009X，计算5.6M左右
	//0x008X，计算11M左右
	if((Component == eCOMPONENT_SPI_ESAM)||(Component == eCOMPONENT_SPI_FLASH))
	{
		//flash与essam都是使用SPI3
		EnWr_WPREG();
		HT_CMU->CLKCTRL1 |= 0x020000; //使能SPI3时钟

		//关闭SPI模块
		HT_SPI3->SPICON &= (~BIT0);
		//0x00bX，实测1.4M左右
		//0x00cX，实测680K左右
		//0x00aX，计算2.8M左右
		//0x009X，计算5.6M左右
		//0x008X，计算11M左右
		switch( Type )
		{
			case eSPI_MODE_0:
				HT_SPI3->SPICON = 0x00a2; //ESAM 通讯速率为2.8M
				break;
			case eSPI_MODE_1:
				HT_SPI3->SPICON = 0x00aa;
				break;
			case eSPI_MODE_2:
				HT_SPI3->SPICON = 0x00a6;
				break;
			case eSPI_MODE_3:
				HT_SPI3->SPICON = 0x00ae;
				break;
			default:
				HT_SPI3->SPICON = 0x00ae;
				break;
		}

		//打开SPI
		HT_SPI3->SPICON |= BIT0;

		HT_SPI3->SPISTA = 0x0000; //状态清零
		DisWr_WPREG();
	}
	else 
	{
		//计量芯片
		EnWr_WPREG();
		HT_CMU->CLKCTRL0 |= 0x000004; //使能SPI0时钟

		//关闭SPI模块
		HT_SPI0->SPICON &= (~BIT0);
		switch( Type )
		{
			case eSPI_MODE_0:
				HT_SPI0->SPICON = 0x00b2; //FLASH 通讯速率为1.4M
				break;
			case eSPI_MODE_1:
				HT_SPI0->SPICON = 0x005b; //计量芯片通信330K左右
				break;
			case eSPI_MODE_2:
				HT_SPI0->SPICON = 0x00b6;
				break;
			case eSPI_MODE_3:
				HT_SPI0->SPICON = 0x00be;
				break;
			default:
				HT_SPI0->SPICON = 0x00be;
				break;
		}

		//打开SPI
		HT_SPI0->SPICON |= BIT0;

		HT_SPI0->SPISTA = 0x0000; //状态清零
		DisWr_WPREG();
	}
}


//-----------------------------------------------
//函数功能: 非中断方式下写SPI的函数
//
//参数:
//			Component[in]	参数不处理
//			Data[in]		写入的值
//
//返回值:  	读出的值
//
//备注:
//-----------------------------------------------
BYTE api_UWriteSpi( eCOMPONENT_TYPE Component, BYTE Data )
{
	if( Component == eCOMPONENT_SPI_SAMPLE )
	{
		while( HT_SPI0->SPISTA&0x0008 )
		{
			;
		}

		HT_SPI0->SPIDAT = Data;

		while( !(HT_SPI0->SPISTA&0x0008) )
		{
			;
		}

		return HT_SPI0->SPIDAT;
	}
	else
	{
		//eCOMPONENT_SPI_FLASH 和eCOMPONENT_SPI_ESAM复用spi3
		while( HT_SPI3->SPISTA&0x0008 )
		{
			;
		}

		HT_SPI3->SPIDAT = Data;

		while( !(HT_SPI3->SPISTA&0x0008) )
		{
			;
		}

		return HT_SPI3->SPIDAT;
	}
}

//-----------------------------------------------
//函数功能:     设置多功能端子函数
//
//参数:	    Type[in] 多功能端子的输出类型（时钟脉冲、需量周期、时段切换）
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
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
	else if( ( Type == eDEMAND_PERIOD_OUTPUT ) || ( Type == eSEGMENT_SWITCH_OUTPUT ) )
	{
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
//函数功能:     多功能端子控制函数
//
//参数:	    Type[in] 多功能端子的输出类型（时钟脉冲、需量周期、时段切换）
//          
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_MultiFunPortCtrl( BYTE Type )
{
	//没有5v不进行任何操作，直接退出
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
	{
		return;
	}

	if( GlobalVariable.g_byMultiFunPortType != Type )
	{
		return;
	}

	if( (Type == eDEMAND_PERIOD_OUTPUT)||(Type == eSEGMENT_SWITCH_OUTPUT) )
	{
		GlobalVariable.g_byMultiFunPortCounter = (80 / SYS_TICK_PERIOD_MS); //80ms 由systick定时完成
		MULTI_FUN_PORT_OUT_L;
	}
	else if( Type == eCLOCK_PULS_OUTPUT )
	{
		//切换为此模式时，自动输出，没有调用的地方
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
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
	//上电时执行此函数
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == TRUE )
	{
		api_BeepReadCard();
	}
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
		if (Serial[i].SendToSendDelay > 0)
		{
			Serial[i].SendToSendDelay--;
		}
    }
    
	//主动上报延时减减
	ReportTimeMinus();

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
//函数功能: 外部中断0服务程序，目前用于脉冲采集
//
//参数:		无
//
//返回值: 	无
//		   
//备注:采样芯片发脉冲   
//-----------------------------------------------
void EXTI0IRQ_Service( void )
{	
    // hplc不用此中断
	// //上升沿开启脉冲灯 下降沿关闭脉冲灯 同时定时器监视 双保险
	// if( EMU_ACTIVE )
	// {
	// 	PULSE_LED_ON;
	// 	api_AddSamplePulse();
	// }
	// else
	// {
	// 	PULSE_LED_OFF;
	// }

	HT_INT->EXTIF &= ~0x0101;	//清中断标志
}


//目前没有用
void EXTI1IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x0202;									//清中断标志
}


//目前没有用
void EXTI2IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x0404;									//清中断标志
}


//目前没有用
void EXTI3IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x0808;									//清中断标志
}

//----hplc-单相费控智能电能表-无功脉冲中断
void EXTI4IRQ_Service( void )
{
	//上升沿开启脉冲灯 下降沿关闭脉冲灯 同时定时器监视 双保险
	if( EMU_QCTIVE )
	{
		//可以接收无功脉冲中断，但是表不做无功
		// PULSE_LED_ON;
		// api_AddSamplePulse();
	}
	else
	{
		// PULSE_LED_OFF;
	}
	HT_INT->EXTIF &= ~0x1010;									//清中断标志
}


//---hplc,有功脉冲中断
void EXTI5IRQ_Service( void )
{
	//上升沿开启脉冲灯 下降沿关闭脉冲灯 同时定时器监视 双保险
	if( EMU_ACTIVE )
	{
		PULSE_LED_ON;
		api_AddSamplePulse();
	}
	else
	{
		PULSE_LED_OFF;
	}
	HT_INT->EXTIF &= ~0x2020;									//清中断标志
}


//目前没有用
void EXTI6IRQ_Service( void )
{
	if( HT_INT->EXTIF&0x0040 )
	{
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			if( DOWN_KEY_PRESSED )
			{
				api_Delay100us( 1 ); //按键消抖 实测约35ms
				if( DOWN_KEY_PRESSED )
				{
					IsExitLowPower = 10;
					LcdKeyDetTimer.DownKey = 10;
				}
			}
		}
	}
	HT_INT->EXTIF &= ~0x4040;                                   //清中断标志
}


//-----------------------------------------------
//函数功能: 物理串口0中断服务程序，具体对应哪个逻辑串口，参看SerialMap配置
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void UART0IRQ_Service( void )
{
	if (HT_UART0->UARTSTA & 0x0002)
	{
		HT_UART0->UARTSTA &= ~0x0002;
		if((HT_UART0->UARTSTA & 0x0004)==0)
		{
			USARTx_Rcv_IRQHandler(0, HT_UART0->SBUF);
		}
		else
		{
			HT_UART0->UARTSTA &= ~0x0004;
		}

	}
	if (HT_UART0->UARTSTA & 0x0001)
	{
		HT_UART0->UARTSTA &= ~0x0001;
        USARTx_Send_IRQHandler(0,HT_UART0);
	}
}


//-----------------------------------------------
//函数功能: 物理串口1中断服务程序，具体对应哪个逻辑串口，参看SerialMap配置
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void UART1IRQ_Service( void )
{
	if (HT_UART1->UARTSTA & 0x0002)
	{
		HT_UART1->UARTSTA &= ~0x0002;
		if((HT_UART1->UARTSTA & 0x0004)==0)
		{
			USARTx_Rcv_IRQHandler(1, HT_UART1->SBUF);
		}
		else
		{
			HT_UART1->UARTSTA &= ~0x0004;
		}

	}
	if (HT_UART1->UARTSTA & 0x0001)
	{
		HT_UART1->UARTSTA &= ~0x0001;
        USARTx_Send_IRQHandler(1,HT_UART1);
	}
}


//-----------------------------------------------
//函数功能: 物理串口2中断服务程序，具体对应哪个逻辑串口，参看SerialMap配置
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void UART2IRQ_Service( void )
{
	if (HT_UART2->UARTSTA & 0x0002)
	{
		HT_UART2->UARTSTA &= ~0x0002;
		if((HT_UART2->UARTSTA & 0x0004)==0)
		{
			USARTx_Rcv_IRQHandler(2, HT_UART2->SBUF);
		}
		else
		{
			HT_UART2->UARTSTA &= ~0x0004;
		}

	}
	if (HT_UART2->UARTSTA & 0x0001)
	{
		HT_UART2->UARTSTA &= ~0x0001;
        USARTx_Send_IRQHandler(2,HT_UART2);
	}
}


//目前没有用
void UART3IRQ_Service( void )
{
	if (HT_UART3->UARTSTA & 0x0002)
	{
		HT_UART3->UARTSTA &= ~0x0002;
		if((HT_UART3->UARTSTA & 0x0004)==0)
		{
			USARTx_Rcv_IRQHandler(3, HT_UART3->SBUF);
		}
		else
		{
			HT_UART3->UARTSTA &= ~0x0004;
		}
	}
	if (HT_UART3->UARTSTA & 0x0001)
	{
		HT_UART3->UARTSTA &= ~0x0001;
        USARTx_Send_IRQHandler(3,HT_UART3);
	}
}


//目前没有用
void UART4IRQ_Service( void )
{
}


//目前没有用
void UART5IRQ_Service( void )
{
}


//目前没有用
void TIMER0IRQ_Service( void )
{
	HT_TMR0->TMRIF = 0;
}


//目前没有用
void TIMER1IRQ_Service( void )
{
	HT_TMR1->TMRIF = 0;
}


//目前没有用
void TIMER2IRQ_Service( void )
{
	HT_TMR2->TMRIF = 0;
}


//目前没有用
void TIMER3IRQ_Service( void )
{
	HT_TMR3->TMRIF = 0;
}


//-----------------------------------------------
//函数功能: 时钟中断服务程序，用于产生秒、分、时任务标志
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   不要采用中断，用大循环好，以后要改！！！！！！
//-----------------------------------------------
void RTCIRQ_Service( void )
{
	#if( RTC_CHIP_TYPE == SOC_CLOCK )
	HT_RTC->RTCIF = 0;
	#endif
}


//-----------------------------------------------
//函数功能: PMU中断处理函数
//
//参数:		无
//
//返回值: 	无
//
//备注:
//-----------------------------------------------
void PMUIRQ_Service( void )
{
	//仅用作唤醒 上电判断在低功耗大循环进行
	HT_PMU->PMUIF = 0x0000;
}

//目前没有用
void TIMER4IRQ_Service( void )
{
	HT_TMR4->TMRIF = 0;
}


//-----------------------------------------------
//函数功能: 定时器发脉冲
//
//参数:		无
//
//返回值: 	无
//
//备注:需要定时器发脉冲的时候
//-----------------------------------------------
void TIMER5IRQ_Service( void )
{
	HT_TMR5->TMRIF = 0;
}


//目前没有用
void UART6IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x8080;									//清中断标志
}


//目前没有用
void EXTI7IRQ_Service( void )
{
	HT_INT->EXTIF2 &= ~0x0101;									//清中断标志
}


//目前没有用
void EXTI8IRQ_Service( void )
{
	HT_INT->EXTIF2 &= ~0x0202;									//清中断标志
}


//目前没有用
void EXTI9IRQ_Service( void )
{
	HT_INT->EXTIF2 &= ~0x0404;									//清中断标志
}
#endif//#if (BOARD_TYPE == BOARD_HT_SINGLE_78202201)

