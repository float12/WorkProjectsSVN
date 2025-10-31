//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		支持698.45规约的单相表印制板驱动文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT_LowPower_Three.h"

#if( ( BOARD_TYPE == BOARD_HT_THREE_0134566 ) || (BOARD_TYPE == BOARD_HT_THREE_0131699) \
 || (BOARD_TYPE == BOARD_HT_THREE_0130347) || (BOARD_TYPE == BOARD_HT_THREE_0132515) )
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

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

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
DWORD LowPowerSecCount;//低功耗累计秒数

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
	// 红外
	eIR,
	SCI_MCU_USART1_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable,			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,			// 发送禁止
	// 载波、RF
	eCR,
	SCI_MCU_USART0_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable, 			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,			// 发送禁止
	//第二路485
	eRS485_II,
	SCI_MCU_USART2_NUM,		// MCU 串口号
	&InitPhSci,				// 串口初始化
	&SciRcvEnable, 			// 接收允许
	&SciRcvDisable,			// 禁止接收
	&SciSendEnable,			// 发送允许
	&SciBeginSend,			// 发送禁止
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		上电引脚配置
//------------------------------------------------------------------------------------------------------------------------------------------------------
const GPIO_InitTypeDef GPIO_PowerOn[GPIO_NUM] = 
{
  //GPIOA引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PA0	PLCEVE		普通端口 输出 开漏 初始为低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA1	PLCRST		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCSET		普通端口 输出 开漏 初始为高 低电平使能
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA3	PLCSTA		普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PA4	/CV7038		普通端口 输出 推挽 初始为低  采样芯片电源控制 低电平供电
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA5	IR_W		普通端口 输入 浮空 红外唤醒检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA6	JTAG		普通端口 输入 浮空 张国办说不能输出低 最好设置为输入
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA7	COVER		普通端口 输入 浮空 开上盖检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	/TERMINAL	普通端口 输入 浮空 开尾盖检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PA9	EWP3		普通端口 输出 推挽 初始为高 铁电写保护
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	UP_KEY		普通端口 输入 浮空 上翻键检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DOWN_KEY	普通端口 输入 浮空 下翻键检测
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA12	VIN_LC		功能端口 第二功能 输入 浮空 停电抄表电池检测
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA13	BAT_LC		功能端口 第二功能 输入 浮空 时钟电池检测
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				无此引脚
  
  //GPIOB引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB0	DIR485_2	普通端口 输出 推挽 第二路485方向控制 初始为高  高电平使能接收
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PB1	YK			普通端口 输出 推挽 报警继电器 初始为低  高电平报警
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PB2	CV3.3V		普通端口 输出 推挽 光耦等电源控制 初始为低  低电平打开电源
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB3	/CVBL		普通端口 输出 推挽 背光控制 初始为高  高电平点亮背光
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB4	RTC_SCL		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB5	RTC_SDA		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	NC			普通端口 输出 开漏 初始为高 空引脚
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB7	SCLK		功能端口 第二功能 (HT6025K新增) 输出 推挽
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB8	MISO		功能端口 第二功能 (HT6025K新增) 输入 浮空
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB9	MOSI		功能端口 第二功能 (HT6025K新增) 输出 推挽
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB10	FLASH_CS	普通端口 输出 推挽 FLASH片选 初始为高 低电平选中
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB11	NC			普通端口 输出 开漏 初始为高 空引脚
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PB12	/RST_IC		普通端口 输出 开漏 初始为低
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB13	MTMS		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB14	NC			普通端口 输出 开漏 初始为高 空引脚
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB15	MTCK		功能端口 第一功能 输出 推挽
  
  //GPIOC引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC0	IR_T		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC1	IR_R		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC2	PLCRX		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC3	PLCTX		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC4	MOSI		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC5	MISO		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC6	SPI_CLK		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PC7	/CS7038		普通端口 输出 推挽 采集芯片CS 初始为高 低电平选择
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC8	SEC			功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC9	RELAYOFF	普通端口 输出 推挽 初始为低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC10	RELAYON		普通端口 输出 推挽 初始为低
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC11	TX485_2		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC12	RX485_2		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PC13	LCD_SCL		普通端口 输出 开漏 初始为低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PC14	LCD_SDA		普通端口 输出 开漏 初始为低
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				无此引脚
  
  //GPIOD引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD0	EWP2		普通端口 输出 推挽 第一片EEPROM写保护 初始为低 高电平使能写保护
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD1	ESDA		普通端口 输出 开漏 初始为低 EEPROM I2C
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD2	ESCL		普通端口 输出 开漏 初始为低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD3	EWP1		普通端口 输出 推挽 第二片EEPROM写保护 初始为低 高电平使能写保护
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD4	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD5	/CVEPROM	普通端口 输出 推挽 初始为低 存储芯片电源控制 低电平供电
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD6	CEKHALL		普通端口 输入 浮空 强磁检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD7	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD8	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD9	AUX/THZJC	普通端口 输入 浮空 继电器检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD10	/CVIR		普通端口 输出 推挽 红外电源控制 初始为低 低电平打开电源
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD11	/CVESAM		普通端口 输出 推挽 初始为低 ESAM电源控制 低电平供电
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD12	/LED_T		普通端口 输出 推挽 跳闸灯 初始为高 低电平点亮
   #if(BOARD_TYPE == BOARD_HT_THREE_0132515)
   {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD13	VOPTM		普通端口 输出 推挽 初始为低 时钟电池开盖检测控制电源
   #else
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD13	VOPTM		普通端口 输出 推挽 初始为高 模块发送电源
  #endif
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD14	ESAM_CS		普通端口 输出 推挽 ESAM片选 初始为高 低电平选中
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD15	CVLCD		普通端口 输出 推挽 初始为高  液晶电源控制 高电平供电
  
  //GPIOE引脚配置
  #if( PREPAY_MODE == PREPAY_LOCAL )
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		卡表:  普通端口 输出 开漏 初始为高 蜂鸣器 PWM模式
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PE1	IC_TXD		卡表:  功能端口 第一功能 输出 推挽 卡串口发送
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE2	IC_RXD		卡表:  功能端口 第一功能 输入 浮空 卡串口接收
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	/CVIC		卡表:  普通端口 输出 开漏 初始为高 在调用时处理
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE4	RX485_1		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PE5	TX485_1		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE6	DIR485_1	普通端口 输出 推挽 第一路485方向 初始为高 高电平接收
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE8	IC_KEY		卡表:  普通端口 输入 浮空
  #else
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		远程表: 普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	IC_TXD		远程表: 普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	IC_RXD		远程表: 普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	/CVIC		远程表: 普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE4	RX485_1		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PE5	TX485_1		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE6	DIR485_1	普通端口 输出 推挽 第一路485方向 初始为高 高电平接收
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	IC_KEY		远程表: 普通端口 输出 开漏 初始为高
  #endif
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
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA0	PLCEVE		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA1	PLCRST		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCSET		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCSTA		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA4	/CV7038		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA5	IR_W		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA6	JTAG		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA7	COVER		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	/TERMINAL	普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA9	EWP3		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	UP_KEY		第一复用 输入 浮空
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DOWN_KEY	第一复用 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA12	VIN_LC		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA13	BAT_LC		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				无此引脚
  
  //GPIOB引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB0	DIR485_2	普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB1	YK			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB2	CV3.3V		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB3	/CVBL		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB4	RTC_SCL		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB5	RTC_SDA		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB7	SCLK		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB8	MISO		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB9	MOSI		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB10	FLASH_CS	普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB11	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB12	/RST_IC		普通端口 输出 推挽 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB13	MTMS		普通端口 输出 推挽 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB14	NC			普通端口 输出 推挽 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB15	MTCK		普通端口 输出 推挽 初始为高
  
  //GPIOC引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC0	IR_T		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC1	IR_R		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC2	PLCRX		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC3	PLCTX		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC4	MOSI		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC5	MISO		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC6	SPI_CLK		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC7	/CS7038		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC8	SEC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC9	RELAYOFF	普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC10	RELAYON		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC11	TX485_2		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC12	RX485_2		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	LCD_SCL		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	LCD_SDA		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				无此引脚
  
  //GPIOD引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD0	EWP2		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD1	ESDA		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD2	ESCL		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD3	EWP1		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD4	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD5	/CVEPROM	普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD6	CEKHALL		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD7	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD8	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD9	AUX/THZJC	普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD10	/CVIR		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD11	/CVESAM		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD12	/LED_T		普通端口 输出 开漏 初始为高
  #if(BOARD_TYPE == BOARD_HT_THREE_0132515)
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD13	NC			普通端口 输出 开漏 初始为高
  #else
  {eGPIO_TYPE_COMMON,	   eGPIO_DIRECTION_OUT,   eGPIO_MODE_OD,	   eGPIO_OUTPUT_HIGH},	 //PD13  NC		  普通端口 输出 开漏 初始为高
  #endif
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD14	ESAM_CS		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD15	CVLCD		普通端口 输出 开漏 初始为高
  
  //GPIOE引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	IC_TXD		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	IC_RXD		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	/CVIC		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE4	RX485_1		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE5	TX485_1		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE6	DIR485_1	普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE7	LVDIN		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	IC_KEY		普通端口 输出 开漏 初始为高
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
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA0	PLCEVE		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA1	PLCRST		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCSET		普通端口 输出 开漏 初始为高 低电平使能
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCSTA		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PA4	/CV7038		普通端口 输出 推挽 初始为高 采样芯片电源控制 低电平供电
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA5	IR_W		普通端口 输出 开漏 初始为高 外唤醒检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA6	JTAG		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA7	COVER		普通端口 输入 浮空 开上盖检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	/TERMINAL	普通端口 输入 浮空 开尾盖检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA9	EWP3		普通端口 输出 开漏 初始为高 铁电写保护
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	UP_KEY		普通端口 输入 浮空 上翻键检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DOWN_KEY	普通端口 输入 浮空 下翻键检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA12	VIN_LC		普通端口 输出 开漏 初始为高 停电抄表电池检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA13	BAT_LC		普通端口 输出 开漏 初始为高 时钟电池检测
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				无此引脚
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				无此引脚
  
  //GPIOB引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB0	DIR485_2	普通端口 输出 开漏 初始为高 第二路485方向控制 高电平使能接收
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB1	YK			普通端口 输出 开漏 初始为高 报警继电器 高电平报警
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB2	CV3.3V		普通端口 输出 开漏 初始为高 光耦等电源控制 低电平打开电源
  {eGPIO_TYPE_COMMON,	   eGPIO_DIRECTION_OUT, eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB3   /CVBL 	  普通端口 输出 推挽 初始为低 背光控制 高电平点亮背光
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB4	RTC_SCL		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB5	RTC_SDA		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	NC			普通端口 输出 开漏 初始为高 空引脚
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB7	SCLK		功能端口 第二功能 (HT6025K新增) 输出 推挽
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB8	MISO		功能端口 第二功能 (HT6025K新增) 输入 浮空
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB9	MOSI		功能端口 第二功能 (HT6025K新增) 输出 推挽
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB10	FLASH_CS	普通端口 输出 推挽 初始为高 FLASH片选 低电平选中
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB11	NC			普通端口 输出 开漏 初始为高 空引脚
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB12	/RST_IC		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB13	MTMS		普通端口 输出 推挽 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB14	NC			普通端口 输出 开漏 初始为高 空引脚
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB15	MTCK		普通端口 输出 推挽 初始为高
  
  //GPIOC引脚配置
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC0	IR_T		功能端口 第一功能 输出 推挽
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC1	IR_R		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC2	PLCRX		普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC3	PLCTX		普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC4	MOSI		普通端口 输出 推挽 初始为低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC5	MISO		普通端口 输出 推挽 初始为低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC6	SPI_CLK		普通端口 输出 推挽 初始为低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PC7	/CS7038		普通端口 输出 推挽 初始为高 采集芯片CS 低电平选择
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC8	SEC			普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC9	RELAYOFF	普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC10	RELAYON		普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC11	TX485_2		普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC12	RX485_2		普通端口 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	LCD_SCL		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	LCD_SDA		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				无此引脚
  
  //GPIOD引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD0	EWP2		普通端口 输出 推挽 初始为低 第一片EEPROM写保护 高电平使能写保护
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD1	ESDA		普通端口 输出 开漏 初始为低 EEPROM I2C
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD2	ESCL		普通端口 输出 开漏 初始为低
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD3	EWP1		普通端口 输出 推挽 初始为低 第二片EEPROM写保护 高电平使能写保护
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD4	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD5	/CVEPROM	普通端口 输出 推挽 初始为高 存储芯片电源控制 低电平供电
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD6	CEKHALL		普通端口 输出 开漏 初始为高 强磁检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD7	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD8	NC			普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD9	AUX/THZJC	普通端口 输出 开漏 初始为高 继电器检测
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD10	/CVIR		普通端口 输出 推挽 初始为高 红外电源控制 低电平打开电源
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD11	/CVESAM		普通端口 输出 推挽 初始为高 ESAM电源控制 低电平供电
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD12	/LED_T		普通端口 输出 推挽 初始为高 跳闸灯 低电平点亮
   #if(BOARD_TYPE == BOARD_HT_THREE_0132515)
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD13	NC			普通端口 输出 开漏 初始为高
  #else
  {eGPIO_TYPE_COMMON,	   eGPIO_DIRECTION_OUT,   eGPIO_MODE_OD,	   eGPIO_OUTPUT_HIGH},	 //PD13  NC		  普通端口 输出 开漏 初始为高
  #endif
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD14	ESAM_CS		普通端口 输出 开漏 初始为低 ESAM片选 低电平选中
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD15	CVLCD		普通端口 输出 推挽 初始为高 液晶电源控制 高电平供电
  
  //GPIOE引脚配置
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		普通端口 输出 开漏 初始为高 无源蜂鸣器
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	IC_TXD		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	IC_RXD		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	/CVIC		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE4	RX485_1		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE5	TX485_1		普通端口 输出 开漏 初始为高
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE6	DIR485_1	普通端口 输出 开漏 初始为高 第一路485方向 高电平接收
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		功能端口 第一功能 输入 浮空
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	IC_KEY		普通端口 输出 开漏 初始为高
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
static WORD Pulse_Timer = 0;//有功脉冲宽度计时器

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 系统电源检测
//
//参数: 	
//          	Type[in]:	
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
	
	//LVDIN0
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
	
	HT_PMU->VDETCFG = 0x004D;					//上电比较VCC电压3.0V BOR 2.0伏复位
	HT_PMU->VDETPCFG = 0x003A;					//推荐配置，影响低功耗上电检测，不可随意更改
	HT_GPIOE->PTOD  |= 0x0080;					//不开漏
	HT_GPIOE->PTUP  |= 0x0080;					//不上拉
	HT_GPIOE->PTDIR &=~0x0080;					//输入
		
	NVIC_DisableIRQ(PMU_IRQn);					//禁止PMU中断
	HT_PMU->PMUIE   = 0x0000;					//禁止PMU中断
	
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
//备注:   
//-----------------------------------------------
static void InitExtInt(void)
{

}


//-----------------------------------------------
//函数功能: 对CPU的AD进行配置
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:  
//-----------------------------------------------
static void ADC_Config( void )
{
   	//使能VCC、ADC1(时钟电池)、ADC0(停电抄表电池)、温度测量
	if (HT_TBS->TBSCON	!= 0x655d)		{HT_TBS->TBSCON	 = 0x655d;}
	//禁止TBS中断
	if (HT_TBS->TBSIE	!= 0x0000)		{HT_TBS->TBSIE	 = 0x0000;}
	//电池电压测量周期=1s;温度测量周期=1/2s
	if (HT_TBS->TBSPRD	!= 0x0000)		{HT_TBS->TBSPRD	 = 0x0000;}
	//pdf上没有TBSTEST这个寄存器说明，张国办说默认写0x0200
	//TBS_ADC电流=1uA;ADC_ldo电压=2.7V;
	if (HT_TBS->TBSTEST	!= 0x0200)		{HT_TBS->TBSTEST = 0x0200;}
}


//-----------------------------------------------
//函数功能: 用于检查维护cpu的寄存器值
//
//参数: 	无
//                    
//返回值:  	无
//
//备注: 没有调用，需要处理@@@@@@   
//-----------------------------------------------
void Maintain_MCU( void )
{
	//TBS 类
	ADC_Config();
}


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
            //计算公式 VADCIN1 = 0.0258*ADC1DAT+4.7559 单位mV
            if( HT_TBS->ADC0DAT & 0x8000 )//负数
            {
                Voltage = 0;
            }
            else
            {
                Voltage = HT_TBS->ADC0DAT;
				Voltage *= ADCINcoffConst_K;
				Voltage += ADCINOffsetConst_K;
				Voltage /= 22;
				Voltage *= 222;//3/23分压
				Voltage /= 1000000L;
				
				if(abs(BatteryPara.ReadBatteryOffset) < (BYTE)(ReadBatteryStandardVoltage*15/100) )//校准偏置，允许最大值0.5V 依据5%的电阻误差，2个电阻最大误差10%，按照15%进行考虑 600*0.12= 72
				{
					Voltage += BatteryPara.ReadBatteryOffset;
				}
				
				if( Voltage >= 660 )
				{
					Voltage = 660;
				}
				
				if( Voltage < 0 )
				{
					Voltage = 0;
				}
            }           
            break;
        case SYS_CLOCK_VBAT_AD://时钟电池电压 保留二位小数
            //计算公式 VADCIN1 = 0.0258*ADC1DAT+4.7559 单位mV
            if( HT_TBS->ADC1DAT & 0x8000 )//负数
            {
                Voltage = 0;
            }
            else
            {
                Voltage = HT_TBS->ADC1DAT;
				Voltage *= ADCINcoffConst_K;
				Voltage += ADCINOffsetConst_K;
				if(( SelThreeBoard != BOARD_HT_THREE_0130347 )&&( SelThreeBoard != BOARD_HT_THREE_0132515 ))	
				{
					Voltage /= 47;
					Voltage *= 247;//47/247分压
				}
				else
				{
					Voltage /= 10;//Voltage /= 68;
					Voltage *= 57;//68/338分压//Voltage *= 338;//68/338分压	
				}
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
		POWER_FLASH_OPEN;
		POWER_SAMPLE_OPEN;
		POWER_HONGWAI_REC_OPEN;
		POWER_HONGWAI_TXD_OPEN;
		POWER_LCD_OPEN;
		POWER_HALL_OPEN;
		POWER_ESAM_OPEN;
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

	//PC4           MOSI                //普通端口 输出 推挽 初始为低
	//PC5           MISO                //普通端口 输出 推挽 初始为低
	//PC6           SPI_CLK             //普通端口 输出 推挽 初始为低
	//PD14          SPI_CS              //普通端口 输出 推挽 初始为低
	//端口方向
	HT_GPIOC->IOCFG &=  0xff8f;

	HT_GPIOC->PTDIR |=  GPIO_Pin_4|
						GPIO_Pin_5|	
	                    GPIO_Pin_6;
	//推挽
	HT_GPIOC->PTOD |=   GPIO_Pin_4|
						GPIO_Pin_5|
	                    GPIO_Pin_6;
	                    
	//初始为低
	HT_GPIOC->PTCLR |=  GPIO_Pin_4|
						GPIO_Pin_5|
	                    GPIO_Pin_6;

	HT_GPIOD->IOCFG  &= 0xBFFF;	//关闭CS
	HT_GPIOD->PTDIR  |=	GPIO_Pin_14;
	HT_GPIOD->PTOD   |=	GPIO_Pin_14;
	HT_GPIOD->PTCLR  |= GPIO_Pin_14;

	POWER_ESAM_CLOSE;//重新打开电源
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
	
	if( Type == eCOMPONENT_SPI_ESAM )
	{
		//PC4           MOSI            	//普通端口 输出 推挽 初始为低
		//PC5           MISO            	//普通端口 输入 禁止上拉
		//PC6           SPI_CLK         	//普通端口 输出 推挽 初始为低
		//PC7           SPI_CS          	//普通端口 输出 推挽 初始为低
		//端口方向
		HT_GPIOC->IOCFG &= 	0xff8f;
		
		HT_GPIOC->PTDIR &= 	(~GPIO_Pin_5); 
		
		HT_GPIOC->PTDIR |=	GPIO_Pin_4|
							GPIO_Pin_6;
		//推挽
		HT_GPIOC->PTOD |=	GPIO_Pin_4|
							GPIO_Pin_6;
							
		HT_GPIOC->PTUP |=   GPIO_Pin_5;	
		
		//初始为低
		HT_GPIOC->PTCLR |= 	GPIO_Pin_4|
							GPIO_Pin_6;
	}
	else
	{
		//PB9           MOSI            	//普通端口 输出 推挽 初始为低
		//PB8           MISO            	//普通端口 输入 禁止上拉
		//PB7           SPI_CLK         	//普通端口 输出 推挽 初始为低
		
		//端口方向
		HT_GPIOB->IOCFG &= 	0xfc7f;
		
		HT_GPIOB->PTDIR &= 	(~GPIO_Pin_8); 
		
		HT_GPIOB->PTDIR |=	GPIO_Pin_7|
							GPIO_Pin_9;
		//推挽
		HT_GPIOB->PTOD |=	GPIO_Pin_7|
							GPIO_Pin_9;
							
		HT_GPIOB->PTUP |=   GPIO_Pin_8;	
		
		//初始为低
		HT_GPIOB->PTCLR |= 	GPIO_Pin_7|
							GPIO_Pin_9;
	}
	
	if( Type == eCOMPONENT_SPI_ESAM ) //pd14  关闭ESAM CS
	{
		HT_GPIOD->IOCFG  &= 0xbfff;	//关闭CS
		HT_GPIOD->PTDIR  |=	GPIO_Pin_14;
		HT_GPIOD->PTOD   |=	GPIO_Pin_14;
		HT_GPIOD->PTCLR  |= GPIO_Pin_14;
		
		POWER_ESAM_CLOSE;//重新打开电源
		api_Delayms(20);
		POWER_ESAM_OPEN;
		api_Delayms(Time);
		
		HT_GPIOD->IOCFG  &= 0xbfff; //重新配置CS   
		HT_GPIOD->PTDIR  |=	GPIO_Pin_14;
		HT_GPIOD->PTOD   |=	GPIO_Pin_14;
		HT_GPIOD->PTCLR  |= GPIO_Pin_14;
		//初始为高
	    HT_GPIOD->PTSET	|= GPIO_Pin_14;
	      
		HT_GPIOD->PTUP	|= 0x4000;
	}
	else if( Type == eCOMPONENT_SPI_FLASH ) //pb10 关闭flash CS
	{
		HT_GPIOB->IOCFG &= 0xfbff;	//关闭CS
		HT_GPIOB->PTDIR |= GPIO_Pin_10;
		HT_GPIOB->PTOD |= GPIO_Pin_10;
		HT_GPIOB->PTCLR |= GPIO_Pin_10;
		
		POWER_FLASH_CLOSE;//重新打开电源
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		api_Delayms(Time);
		
		HT_GPIOB->IOCFG  &= 0xfbff; //重新配置CS   
		HT_GPIOB->PTDIR  |=	GPIO_Pin_10;
		HT_GPIOB->PTOD   |=	GPIO_Pin_10;
		HT_GPIOB->PTCLR  |= GPIO_Pin_10;
		//初始为高
	    HT_GPIOB->PTSET	|=	GPIO_Pin_10;
	      
		HT_GPIOB->PTUP   |=0x0400;
	}
	else //pc7 关闭计量芯片 CS
	{
		HT_GPIOC->IOCFG  &= 0xff7f;	//关闭CS
		HT_GPIOC->PTDIR |= GPIO_Pin_7;
		HT_GPIOC->PTOD |= GPIO_Pin_7;
		HT_GPIOC->PTCLR |= GPIO_Pin_7;
		
		POWER_SAMPLE_CLOSE;//重新打开电源
		api_Delayms(Time);
		POWER_SAMPLE_OPEN;
		api_Delayms(Time);
		
		HT_GPIOC->IOCFG  &= 0xff7f; //重新配置CS   
		HT_GPIOC->PTDIR |= GPIO_Pin_7;
		HT_GPIOC->PTOD |= GPIO_Pin_7;
		HT_GPIOC->PTCLR |= GPIO_Pin_7;
		//初始为高
	    HT_GPIOC->PTSET	|= GPIO_Pin_7;
		HT_GPIOC->PTUP |=0x0080;
	}

	if( Type == eCOMPONENT_SPI_ESAM )
	{
		//PC4           MOSI            	//功能端口
		//PC5           MISO            	//功能端口 输入 禁止上拉
		//PC6           SPI_CLK         	//功能端口
		//PC7           SPI_CS          	//普通端口 输出 推挽 初始为高//PA11           SPI_CS          	//普通端口 输出 推挽 初始为高
		HT_GPIOC->IOCFG |= 	GPIO_Pin_4|
							GPIO_Pin_5|
							GPIO_Pin_6;   
		//端口方向
		HT_GPIOC->PTDIR &= 	(~GPIO_Pin_5);
		
		HT_GPIOC->PTDIR |= 	GPIO_Pin_4|
							GPIO_Pin_6;
		//推挽
		HT_GPIOC->PTOD |=	GPIO_Pin_4|
							GPIO_Pin_6;
							
		HT_GPIOC->PTUP |=   GPIO_Pin_5;
		
		api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
	}
	else
	{
		//PB9           MOSI            	//普通端口 输出 推挽 初始为低
		//PB8           MISO            	//普通端口 输入 禁止上拉
		//PB7           SPI_CLK         	//普通端口 输出 推挽 初始为低
		
		HT_GPIOB->NEWAFEN = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		
		HT_GPIOB->IOCFG |= 	GPIO_Pin_7|
							GPIO_Pin_8|
							GPIO_Pin_9;
		
		HT_GPIOB->AFCFG=GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		
		//端口方向
		HT_GPIOB->PTDIR &= 	(~GPIO_Pin_8);
		
		HT_GPIOB->PTDIR |= 	GPIO_Pin_7|
							GPIO_Pin_9;
		//推挽
		HT_GPIOB->PTOD |=	GPIO_Pin_7|
							GPIO_Pin_9;
							
		HT_GPIOB->PTUP |=   GPIO_Pin_8;
		
		if( Type == eCOMPONENT_SPI_SAMPLE )
		{
			api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
		}
		else
		{
			api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
		}
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
	
	//端口初始化
	ResetSPIDevice( eCOMPONENT_SPI_ESAM, 100 ); //对esam进行复位
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 5 ); //对计量芯片进行复位
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 ); //对Flash进行复位
    
    InitPort( ePowerOnMode );//正常初始化
	
	PowerCtrl( 0 );
	
	ESAMSPIPowerDown( ePowerOnMode );//关闭ESAM电源，CS，SPI管脚配置为普通
	
	ADC_Config();
	
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
	ADC_Config(); //!!!!特别注意 此处ADC_Config不能删 - yxk
	
	api_WriteFreeEvent(EVENT_SYS_START, HT_PMU->RSTSTA);
	if((HT_PMU->RSTSTA&0x0005)==0x0004)//看门狗复位记录异常事件
	{
		api_WriteSysUNMsg( SYSERR_WDTRST_ERR );
	}
	HT_PMU->RSTSTA = 0;//清除标志
	
	api_PowerUpPara();
	api_PowerUpSave();
	
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

	if( No == CS_SPI_ESAM )
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
	BpsBak = api_GetBaudRate( 0, SciPhNum ); 

    switch(SciPhNum)
    {
        case 0:
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
        case 1:
            USARTx = HT_UART1;
			UARTx_IRQn = UART1_IRQn;
            NVIC_DisableIRQ(UART1_IRQn);                //禁止串口中断
            NVIC_ClearPendingIRQ(UART1_IRQn);           //清除挂起状态
            NVIC_SetPriority(UART1_IRQn, 3);            //设置优先级
            //NVIC_EnableIRQ(UART1_IRQn);                 //使能串口中断
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000020;               //开启串口时钟
           	#if( COMM_TYPE == COMM_NEAR )
            HT_UART1->IRCON   = 0x0000;                   //红外不调制
            HT_UART1->IRDUTY  = 0x0000;                   //50%占空比
			#else
			HT_UART1->IRCON   = 0x0001;                   //红外调制
            HT_UART1->IRDUTY  = 0x0001;                   //25%占空比
			#endif
            DisWr_WPREG();
            break;
		case 2:
			if( (BpsBak&0x0F) == 10 ) //115200波特率关闭滤波功能
			{
				HT_GPIOC->FILT &= (~(1 << 12));               //关闭滤波功能
			}
			else
			{
				HT_GPIOC->FILT |= (1 << 12);
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
		case 3:
			if( (BpsBak&0x0F) == 10 ) //115200波特率关闭滤波功能
			{
				HT_GPIOE->FILT &= (~(1 << 4));                //关闭滤波功能
			}
			else
			{
				HT_GPIOE->FILT |= (1 << 4);
			}
			USARTx = HT_UART3;
			UARTx_IRQn = UART3_IRQn;
            NVIC_DisableIRQ(UART3_IRQn);                //禁止串口中断
            NVIC_ClearPendingIRQ(UART3_IRQn);           //清除挂起状态
            NVIC_SetPriority(UART3_IRQn, 3);            //设置优先级
            //NVIC_EnableIRQ(UART3_IRQn);                 //使能串口中断
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000080;               //开启串口时钟
            DisWr_WPREG();
            break;
		/*
		case 4:
			if( (BpsBak&0x0F) == 10 ) //115200波特率开启滤波功能
			{
				HT_GPIOE->FILT &=(~(1<<2));					//关闭滤波功能
			}
			else
			{
				HT_GPIOE->FILT |=(1<<2);					//关闭滤波功能
			}
			USARTx = HT_UART4;
			UARTx_IRQn = UART4_IRQn;
            NVIC_DisableIRQ(UART4_IRQn);                //禁止串口中断
            NVIC_ClearPendingIRQ(UART4_IRQn);           //清除挂起状态
            NVIC_SetPriority(UART4_IRQn, 3);            //设置优先级
            //NVIC_EnableIRQ(UART4_IRQn);                 //使能串口中断
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
    Bps = BpsBak & 0x0F;
	if( Bps >= (sizeof(TAB_Baud) / sizeof(TAB_Baud[0])) )
	{
		Bps = 6;		//如果数据错误 默认为9600
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
	//Bps=0x40;//强制偶校验（实测奇偶校验都能连上）
	//Bps=0x20;//强制奇校验（实测奇偶校验都能连上）
	//Bps=0;//强制无校验
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
        /*
        case 4:
            ENABLE_HARD_SCI_4_RECE;
            api_Delay100us(1);// 延时100us
            HT_UART4->UARTCON |= 0x000A;
            break;
        */
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
        /*
        case 4:
            HT_UART4->UARTCON &= ~0x000A;
            break;
        */
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
        /*
        case 4:
            ENABLE_HARD_SCI_4_SEND;
            api_Delay100us(1);          // 延时100us
            break;
        */
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
                    case 0:
                        HT_UART0->UARTCON |= 0x0001; //发送使能
                        HT_UART0->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
                        HT_UART0->UARTCON |= 0x0004; //发送中断使能
                        NVIC_EnableIRQ(UART0_IRQn);//开中断
                        break;
                    case 1:
                        HT_UART1->UARTCON |= 0x0001; //发送使能
                        HT_UART1->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
                        HT_UART1->UARTCON |= 0x0004; //发送中断使能
                        NVIC_EnableIRQ(UART1_IRQn);//开中断
                        break;
                    case 2:
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
                    /*
                    case 4:
                        HT_UART4->UARTCON |= 0x0001; //发送使能
                        HT_UART4->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // 写入数据
                        HT_UART4->UARTCON |= 0x0004; //发送中断使能
                        NVIC_EnableIRQ(UART4_IRQn);//开中断
                        break;
                    */
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
	if( Component == eCOMPONENT_SPI_ESAM )
	{
		EnWr_WPREG();
		HT_CMU->CLKCTRL0 |= 0x000004; //使能SPI0时钟
		//0x00bX，实测1.4M左右
		//0x00cX，实测680K左右
		//0x00aX，计算2.8M左右
		//0x009X，计算5.6M左右
		//0x008X，计算11M左右
		switch( Type )
		{
			case eSPI_MODE_0:
				HT_SPI0->SPICON = 0x00a3; //
				break;
			case eSPI_MODE_1:
				HT_SPI0->SPICON = 0x00cb; //7026和7038采用模式1，低速配置时最高1M
				break;
			case eSPI_MODE_2:
				HT_SPI0->SPICON = 0x00a7;
				break;
			case eSPI_MODE_3:
				HT_SPI0->SPICON = 0x00af;
				break;
			default:
				HT_SPI0->SPICON = 0x00af;
				break;
		}

		HT_SPI0->SPISTA = 0x0000; //状态清零
		DisWr_WPREG();
	}
	else
	{
		EnWr_WPREG();
		HT_CMU->CLKCTRL1 |= 0x020000; //使能SPI3时钟
		//0x00bX，实测1.4M左右
		//0x00cX，实测680K左右
		//0x00aX，计算2.8M左右
		//0x009X，计算5.6M左右
		//0x008X，计算11M左右
		switch( Type )
		{
			case eSPI_MODE_0:
				HT_SPI3->SPICON = 0x00a3; //
				break;
			case eSPI_MODE_1:
				HT_SPI3->SPICON = 0x00cb; //7026和7038采用模式1，低速配置时最高1M
				break;
			case eSPI_MODE_2:
				HT_SPI3->SPICON = 0x00a7;
				break;
			case eSPI_MODE_3:
				HT_SPI3->SPICON = 0x00af;
				break;
			default:
				HT_SPI3->SPICON = 0x00af;
				break;
		}

		HT_SPI3->SPISTA = 0x0000; //状态清零
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
	if( Component == eCOMPONENT_SPI_ESAM )
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
//参数:	    Type[in]		多功能端子的输出类型（时钟脉冲、需量周期、时段切换）
//          
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
    
	if( ( Type == eDEMAND_PERIOD_OUTPUT ) || ( Type == eSEGMENT_SWITCH_OUTPUT ) )
    {
        GlobalVariable.g_byMultiFunPortCounter = (80/SYS_TICK_PERIOD_MS);//80ms 由systick定时完成
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


//-----------------------------------------------
//函数功能: 外部中断0服务程序，目前用于脉冲采集
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void EXTI0IRQ_Service( void )
{
	#if( SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH )
	
	if(DeltaTestNo > (MAX_PORT_INT_NUM-1))
	{
		DeltaTestNo = 0;
	}
	WakeUpCountNew = HT_TMR4->TMRCNT;
	DeltaTest[DeltaTestNo] = WakeUpCountNew - WakeUpCountOld;
	DeltaTestNo ++;
	WakeUpCountOld = WakeUpCountNew;
	
	#endif//#if( SEL_CONTINUS_FRAM_WAKEUP == YES )
		
	HT_INT->EXTIF &= ~0x0101;									//清中断标志
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


//目前没有用
void EXTI4IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x1010;									//清中断标志
}


//低功耗按键唤醒(上键)
void EXTI5IRQ_Service( void )
{
	if( HT_INT->EXTIF&0x2000 )
	{
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			if( UP_KEY_PRESSED )
			{
				api_Delay10us( 5 ); //按键消抖 实测约36.7ms
				if( UP_KEY_PRESSED )
				{
					IsExitLowPower = 10;
					LcdKeyDetTimer.UpKey = 10;
					LcdKeyDetTimer.DownKey = 10;
					LcdKeyDetTimer.UpDownKey = 10;
				}
			}
		}
	}
	HT_INT->EXTIF &= ~0x2020;									//清中断标志
}


//低功耗按键唤醒(下键)
void EXTI6IRQ_Service( void )
{
	if( HT_INT->EXTIF&0x4000 )
	{
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			if( DOWN_KEY_PRESSED )
			{
				api_Delay10us( 5 ); //按键消抖 实测约36.7ms
				if( DOWN_KEY_PRESSED )
				{
					IsExitLowPower = 10;
					LcdKeyDetTimer.UpKey = 10;
					LcdKeyDetTimer.DownKey = 10;
					LcdKeyDetTimer.UpDownKey = 10;
				}
			}
		}
	}
	HT_INT->EXTIF &= ~0x4040;									//清中断标志
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
//备注:   
//-----------------------------------------------
void RTCIRQ_Service( void )
{
	#if( RTC_CHIP_TYPE == SOC_CLOCK )
	
	TRealTimer t;
	
	if( HT_RTC->RTCIF & 0x0001 )
	{	
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{

			if( LowPowerSecCount < (IR_WAKEUP_LIMIT_TIME+86400) ) //8天余量 24*8*3600 避免超限后重新累加
		    {
                LowPowerSecCount ++;
		    }
			
			#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )
			ProcContinueFramWakeup();

			#endif//#if( SEL_CONTINUS_FRAM_WAKEUP == YES )
			
			#if( (SEL_EVENT_LOST_ALL_V == YES) || (SEL_EVENT_POWER_ERR == YES) )

			if( g_byDownPowerTime < 100 )
			{
				g_byDownPowerTime++;
			}

			if( (g_byDownPowerTime == START_TEST_LOST_ALLV) || (g_byDownPowerTime == START_TEST_POWER_ERR) )
			{
				//低功耗唤醒期间不能关闭eeprom电源  进入低功耗会自动关闭电源对应 BUG_VERSION -- 4
				if( api_GetSysStatus(eSYS_LOW_POWER_WAKEUP) == FALSE )
				{
					if( IsExitLowPower == 0 )
					{
						#if( SEL_EVENT_POWER_ERR == YES )
						if( g_byDownPowerTime == START_TEST_POWER_ERR )
						{
							IsExitLowPower = 20;
						}
						#endif
						
						#if( SEL_EVENT_LOST_ALL_V == YES )
						if( g_byDownPowerTime == START_TEST_LOST_ALLV )
						{
							IsExitLowPower = 30;
						}
						#endif
					}
				}
				else
				{
					g_byDownPowerTime -= START_TEST_POWER_ERR;
				}
			}

			#endif//#if( (SEL_EVENT_LOST_ALL_V == YES) || (SEL_EVENT_POWER_ERR == YES) )
		}
	}
	
	HT_RTC->RTCIF = 0;
	
	#endif//#if( RTC_CHIP_TYPE == SOC_CLOCK )
}


//目前没有用
void TIMER4IRQ_Service( void )
{
	HT_TMR4->TMRIF = 0;
}


//目前没有用
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


#endif//#if( (BOARD_TYPE == BOARD_HT_THREE_0134566) )

