//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		支持698.45单相表的印制板头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __BOARD_115200_SINGLE_H
#define __BOARD_115200_SINGLE_H

#if ( (BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303) \
  || (BOARD_TYPE == BOARD_HT_SINGLE_78202401)  || (BOARD_TYPE == BOARD_HT_SINGLE_20250819))

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAX_PHASE		1

//定义CPU类型
#define CPU_TYPE		CPU_HT6025
//#define CPU_HT6025              	6

//数据源
#define SAMPLE_CHIP			CHIP_HT7053D
// #define CHIP_HT_7053D		9
//时钟芯片类型
#define RTC_CHIP_TYPE		SOC_CLOCK
//SOC_CLOCK					SOC集成时钟
//OUT_CLOCK					外部独立时钟

//时钟芯片
#define RTC_CHIP			RTC_HT_INCLOCK
//RTC_CHIP_8025T			支持带温度补偿的8025
//RTC_HT_INCLOCK			矩泉内置RTC

//液晶类型选择
#define LCD_TYPE			LCD_HT_SINGLE_METER
//LCD_GW3PHASE_09			09年8月国网三相表液晶
//LCD_HT_SINGLE_METER		矩泉单相表国网液晶

//液晶驱动芯片地址线宽度
#define LCD_DRIVE_CHIP			LCD_CHIP_NO
//#define LCD_CHIP_1623					7
//#define LCD_CHIP_1622					6
//#define LCD_CHIP_BU97950				5
//#define LCD_CHIP_BU9792               4
//#define LCD_CHIP_PCF8576              3
//#define LCD_CHIP_BU9794               2
//#define LCD_CHIP_BU9799               1
//#define LCD_CHIP_NO                   0

//通讯类型选择，（近红外、远红外）
#define COMM_TYPE			COMM_NOAR
//COMM_NEAR		近红外
//COMM_FAR		远红外
//COMM_NOAR		无红外

#if(BOARD_TYPE == BOARD_HT_SINGLE_78202201) 
	#define MASTER_MEMORY_CHIP			CHIP_24LC256
	//CHIP_24LC256
	//CHIP_24LC512
	//CHIP_NO							没有焊存储芯片  只有第三片可以选没有

	//第二片存储芯片(D4)
	#define SLAVE_MEM_CHIP				CHIP_NO
#elif(BOARD_TYPE == BOARD_HT_SINGLE_20250819) 
	#define MASTER_MEMORY_CHIP			CHIP_24LC512
	//CHIP_24LC256
	//CHIP_24LC512
	//CHIP_NO							没有焊存储芯片  只有第三片可以选没有

	//第二片存储芯片(D4)
	#define SLAVE_MEM_CHIP				CHIP_NO
#else
	#define MASTER_MEMORY_CHIP			CHIP_24LC128
	//CHIP_24LC256
	//CHIP_24LC512
	//CHIP_NO							没有焊存储芯片  只有第三片可以选没有

	//第二片存储芯片(D4)
	#define SLAVE_MEM_CHIP				CHIP_24LC128
#endif

//第三片存储器类型  只用于连续空间的数据存储
#define THIRD_MEM_CHIP				CHIP_GD25Q32C//带E和不带E的驱动是不同的2016.5.30//单相带E

//第三片存储器是否开启数据备份功能
#if((BOARD_TYPE == BOARD_HT_SINGLE_78202303) || (BOARD_TYPE == BOARD_HT_SINGLE_78202401)) 
#define SLE_THIRD_MEM_BACKUP		YES
#else
#define SLE_THIRD_MEM_BACKUP		NO
#endif
//--------------------------------------------

//预付费标准
#define PREPAY_STANDARD		PREPAY_GUOWANG_698
//PREPAY_GUOWANG_698	 	国网698规约
//PREPAY_GUOWANG_13			国网13规范
//PREPAY_NANWANG_15			南网15规范

//加密方式
#define ENCRYPT_MODE		ENCRYPT_ESAM
//ENCRYPT_ESAM				硬加密
//ENCRYPT_SOFTWARE			软加密

//卡片类型
#define CARD_COM_TYPE		CARD_NO
//CARD_NO
//CARD_CONTACT
//CARD_RADIO

//ESAM通信模式
#define ESAM_COM_TYPE		TYPE_SPI
//TYPE_SPI
//TYPE_7816

//选择几路串口 只能选择3(硬件串口和模拟串口的总数)
#define MAX_COM_CHANNEL_NUM			4

//IIC是模拟的还是硬件的
#define SEL_24LC256_COMMUNICATION_MOD	IIC_SIM
//IIC_SIM
//IIC_HARD

//是否选择任意报文进行低功耗唤醒
#define SEL_CONTINUS_FRAM_WAKEUP		IR_WAKEUP_NO_FUNC
//////////////////////////////////////////////////////////////////////////
//							IIC管脚定义
//////////////////////////////////////////////////////////////////////////
// IIC1
//EEPROM的IIC定义
#define EEPROM_I2C_SDA_PORT_IN			HT_GPIOC->PTDIR &=~(1<<14)
#define EEPROM_I2C_SDA_PORT_DATA_IN		(HT_GPIOC->PTDAT & (1<<14))
#define EEPROM_SCL_L					HT_GPIOC->PTCLR |= (1<<13)				
#define EEPROM_SCL_H					HT_GPIOC->PTSET |= (1<<13);\
										HT_GPIOC->PTDIR |= (1<<13)
#define EEPROM_SDA_L					HT_GPIOC->PTCLR |= (1<<14);\
										HT_GPIOC->PTDIR |= (1<<14)
#define EEPROM_SDA_H					EEPROM_I2C_SDA_PORT_IN
                      
// EEPROM 写入保护管脚 硬件就没有连接，芯片直接接地了。
#define ENABLE_WRITE_PROTECT1		    ;
#define ENABLE_WRITE_PROTECT2		    ;
#define DISABLE_WRITE_PROTECT1		    ;
#define DISABLE_WRITE_PROTECT2		    ;
#define ENABLE_WRITE_PROTECT1_FOR_ADDR	;
#define ENABLE_WRITE_PROTECT2_FOR_ADDR	;

#define POWER_EEPROM_OPEN				;
#define POWER_EEPROM_CLOSE				;
#define EEPROM_INIT_LOW_POWER			;

//EEPROM 的地址
#define MEM_24LC25601_DEVADDR		    0xa0
#define MEM_24LC25602_DEVADDR		    0xae
#define MEM_24LC25601_CONFIG_DEVADDR	0xb0
#define MEM_24LC25602_CONFIG_DEVADDR	0xbe


//////////////////////////////////////////////////////////////////////////
// 液晶驱动芯片IIC定义
#define LCD_I2C_SDA_PORT_IN             ;
#define LCD_I2C_SDA_PORT_DATA_IN        ;
#define LCD_SCL_LOW                     ;
#define LCD_SCL_HIGH                    ;
#define LCD_SDA_LOW                     ;
#define LCD_SDA_HIGH                    ;

//////////////////////////////////////////////////////////////////////////
//SPI CS配置
// FLASH片选管脚    
#define FLASH_CS_ENABLE				    HT_GPIOC->PTCLR |= (1<<7)
#define FLASH_CS_DISABLE			    HT_GPIOC->PTSET |= (1<<7)
// 计量芯片片选管脚 
#define SAMPLE_CS_ENABLE			    HT_GPIOD->PTCLR |= (1<<14);
#define SAMPLE_CS_DISABLE			    HT_GPIOD->PTSET |= (1<<14);
// ESAM芯片片选管脚 
#define ESAM_CS_ENABLE			    	HT_GPIOB->PTCLR |= (1<<6)
#define ESAM_CS_DISABLE			    	HT_GPIOB->PTSET |= (1<<6)

//计量模块                     
#define EMU_UARTSTA						(HT_UART3->UARTSTA)
#define EMU_UARTBUF						(HT_UART3->SBUF)

// 载波模块控制管脚定义
#define ENABLE_PLC_PROGRAMM         	;//HT_GPIOA->PTDIR |= (1<<2);\
										//HT_GPIOA->PTCLR |= (1<<2)
#define DISABLE_PLC_PROGRAMM        	;//HT_GPIOA->PTDIR |= (1<<2);\
										//HT_GPIOA->PTSET |= (1<<2)

//载波发生状态检测 高电平表示载波发生  	
#define RESET_PLC_MODUAL            	HT_GPIOA->PTDIR |= (1<<3);\
										HT_GPIOA->PTCLR |= (1<<3);\
										api_Delayms( 250 );\
										HT_GPIOA->PTSET |= (1<<3)

#if( PREPAY_MODE == PREPAY_LOCAL )

//拉低复位引脚5ms，复位CPU卡
#define RESET_CARD						HT_GPIOE->PTDIR |= (1<<6);\
										HT_GPIOE->PTOD |= (1<<6);\
										HT_GPIOE->PTCLR |= (1<<6);\
										api_Delayms(5);\
										HT_GPIOE->PTDIR |= (1<<6);\
										HT_GPIOE->PTOD |= (1<<6);\
										HT_GPIOE->PTSET |= (1<<6)

//拉低复位引脚--普通端口 推挽输出低
#define CLOSE_CARD_RESET				HT_GPIOE->PTDIR |= (1<<6);\
										HT_GPIOE->PTOD |= (1<<6);\
										HT_GPIOE->PTCLR |= (1<<6)

//开启卡IO引脚配置--第二功能 推挽输出
#define OPEN_CARD_IO					HT_GPIOE->IOCFG |= (1<<2);\
										HT_GPIOE->AFCFG |= (1<<2);\
										HT_GPIOE->PTDIR |= (1<<2);\
										HT_GPIOE->PTOD |= (1<<2)
//关闭卡IO引脚配置--普通端口 推挽输出低
#define CLOSE_CARD_IO					HT_GPIOE->IOCFG &= ~(1<<2);\
										HT_GPIOE->AFCFG &= ~(1<<2);\
										HT_GPIOE->PTDIR |= (1<<2);\
										HT_GPIOE->PTOD |= (1<<2);\
										HT_GPIOE->PTCLR |= (1<<2)
//开启卡时钟引脚配置--第一功能 推挽输出
#define OPEN_CARD_CLK					HT_GPIOE->IOCFG |= (1<<3);\
										HT_GPIOE->AFCFG &= ~(1<<3);\
										HT_GPIOE->PTDIR |= (1<<3);\
										HT_GPIOE->PTOD |= (1<<3)
//关闭卡时钟引脚配置--普通端口 推挽输出低
#define CLOSE_CARD_CLK					HT_GPIOE->IOCFG &= ~(1<<3);\
										HT_GPIOE->AFCFG &= ~(1<<3);\
										HT_GPIOE->PTDIR |= (1<<3);\
										HT_GPIOE->PTOD |= (1<<3);\
										HT_GPIOE->PTCLR |= (1<<3)

#endif//if( PREPAY_MODE == PREPAY_LOCAL )//----
//模块事件通知 高电平表示有事件发生 低电平表示没有事件发生
#define CANCEL_EVENTOUT			    HT_GPIOA->PTDIR |= (1<<2);\
									HT_GPIOA->PTCLR |= (1<<2)
#define NOTICE_EVENTOUT			    HT_GPIOA->PTDIR |= (1<<2);\
									HT_GPIOA->PTSET |= (1<<2)

//多功能端子输出低
#define MULTI_FUN_PORT_OUT_L			HT_GPIOE->IOCFG &= ~(1<<1);\
										HT_GPIOE->PTDIR |= (1<<1);\
										HT_GPIOE->PTCLR |= (1<<1)
										
//多功能端子输出高
#define MULTI_FUN_PORT_OUT_H			HT_GPIOE->IOCFG &= ~(1<<1);\
										HT_GPIOE->PTDIR |= (1<<1);\
										HT_GPIOE->PTSET |= (1<<1)
										
//切换到时钟脉冲输出
#define SWITCH_TO_CLOCK_PULS_OUTPUT		HT_GPIOE->IOCFG |= (1<<1)

// 点亮背光     
#define	OPEN_BACKLIGHT			        HT_GPIOA->PTCLR |= (1<<7)
#define	CLOSE_BACKLIGHT			        HT_GPIOA->PTSET |= (1<<7)		
//跳闸指示灯     
#define RELAY_LED_ON			        HT_GPIOE->PTCLR |= (1<<3)	                                           
#define RELAY_LED_OFF			        HT_GPIOE->PTSET |= (1<<3)
//计量芯片有功脉冲输入
#define EMU_ACTIVE						(HT_GPIOA->PTDAT & (1<<10))
//计量芯片无功脉冲输入
#define EMU_QCTIVE						(HT_GPIOA->PTDAT & (1<<9))
//有功脉冲输出指示灯
#define PULSE_LED_ON			        HT_GPIOE->PTCLR |= (1<<2)	                                           
#define PULSE_LED_OFF			        HT_GPIOE->PTSET |= (1<<2)
//报警指示灯        
#define WARN_ALARM_LED_ON		        ;
#define WARN_ALARM_LED_OFF		        ;
#if( PREPAY_MODE == PREPAY_LOCAL )
//蜂鸣器
//第一复用功能 推挽输出(PWM输出)
#define BEEP_ON							HT_GPIOE->IOCFG |= (1<<0);\
										HT_GPIOE->AFCFG &= ~(1<<0);\
										HT_GPIOE->PTDIR |= (1<<0);\
										HT_GPIOE->PTOD |= (1<<0);
//普通端口 开漏输出低
#define BEEP_OFF						HT_GPIOE->IOCFG &= ~(1<<0);\
										HT_GPIOE->PTDIR |= (1<<0);\
										HT_GPIOE->PTOD &= ~(1<<0);\
										HT_GPIOE->PTCLR |= (1<<0);
#endif

//辅助电源是否掉电检测
#define SECPOWER_IS_DOWN	        	0

//报警继电器动作
#define WARN_RELAY_OPEN					;
//报警继电器取消
#define WARN_RELAY_CLOSE				;

#define CLOSE_POWER_RELAY_PLUS_INSIDE	HT_GPIOC->PTCLR|=(1<<9);HT_GPIOC->PTSET|=(1<<10);\
										HT_GPIOC->PTDIR|=(1<<9);HT_GPIOC->PTDIR|=(1<<10);\
                                        api_Delayms(120);\
                                        HT_GPIOC->PTCLR|=(1<<9);HT_GPIOC->PTCLR|=(1<<10);
                                            
#define OPEN_POWER_RELAY_PLUS_INSIDE	HT_GPIOC->PTCLR|=(1<<10);HT_GPIOC->PTSET|=(1<<9);\
										HT_GPIOC->PTDIR|=(1<<9);HT_GPIOC->PTDIR|=(1<<10);\
                                        api_Delayms(120);\
                                        HT_GPIOC->PTCLR|=(1<<9);HT_GPIOC->PTCLR|=(1<<10);
//外置继电器合闸
#define CLOSE_POWER_RELAY_LEVEL_OUTSIDE HT_GPIOC->PTCLR|=(1<<9);\
										HT_GPIOC->PTDIR|=(1<<9);
//外置继电器跳闸
#define OPEN_POWER_RELAY_LEVEL_OUTSIDE	HT_GPIOC->PTSET|=(1<<9);\
										HT_GPIOC->PTDIR|=(1<<9);
//外置继电器跳闸(脉冲方式)
#define OPEN_POWER_RELAY_PLUS_OUTSIDE	;

//内置继电器状态检测
#define RELAY_STATUS_OPEN	        (HT_GPIOD->PTDAT & (1<<15))
//检测配置：P15使能输入，浮空. 
#define RELAY_CHECK_STATUS 			;//HT_GPIOD->PTDIR |= (1 << 14);	HT_GPIOD->PTOD |= (1 << 14);	HT_GPIOD->PTSET = (1 << 14);\
									//HT_GPIOD->PTDIR &= ~(1 << 15);	HT_GPIOD->PTUP |= (1 << 15);
//低功耗配置：开漏输出高
#define RELAY_LOWPOWER_STATUS		;//HT_GPIOD->PTDIR |= (1 << 15);	HT_GPIOD->PTOD &= ~(1 << 15); 	HT_GPIOD->PTSET = (1 << 15);\
									//HT_GPIOD->PTDIR |= (1 << 14);	HT_GPIOD->PTOD &= ~(1 << 14); 	HT_GPIOD->PTSET = (1 << 14);

//用于防止低功耗初始化管脚时突然上电导致继电器跳闸
#define LOWPOWER_INVALID_RELAY_CTRL	HT_GPIOC->PTOD |= 0x0600;\
									HT_GPIOC->PTCLR |= 0x0600;\
									HT_GPIOC->PTDIR |= 0x0600
									
									
//////////////////////////////////////////////////////////////////////////
//							输入相关管脚操作
//////////////////////////////////////////////////////////////////////////
//获取按键的状态 按下是低电平
#define UP_KEY_PRESSED          0//上翻按键
#define DOWN_KEY_PRESSED        (!(HT_GPIOA->PTDAT & (1<<11)))//(HT_GPIOA->PTDAT & (1<<11))//
#define PROG_KEY_PRESSED        0//编程按键
// 强磁场检测
#define CHECK_MAGNETIC_STATUS   ;
#define MAGNETIC_IS_OCCUR       0

// 开上盖盖输入
#if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
#define UP_COVER_IS_OPEN      	((HT_GPIOA->PTDAT & (1<<10)))
#define UP_COVER_IS_CLOSE       (!(HT_GPIOA->PTDAT & (1<<10)))
#else
#define UP_COVER_IS_OPEN        (!(HT_GPIOA->PTDAT & (1<<6)))
#define UP_COVER_IS_CLOSE       ((HT_GPIOA->PTDAT & (1<<6)))
#endif

#define ENABLE_UP_COVER_INT     ;
#define DIS_UP_COVER_INT        ;
// 开尾盖输入
#define END_COVER_IS_OPEN       0
#define END_COVER_IS_CLOSE      0
#define ENABLE_END_COVER_INT    ;
#define DIS_END_COVER_INT       ;
                                      
#if(PREPAY_MODE == PREPAY_LOCAL)
#define CARD_IN_SLOT       		(!(HT_GPIOA->PTDAT &(1<<10)))//卡在卡槽中，触发了IC_Key
#endif                                      

///////////////////////////////////////////////////////////////////////////
// 485串口方向控制定义
//////////////////////////////////////////////////////////////////////////
#define ENABLE_HARD_SCI_0_RECE		    ;
#define ENABLE_HARD_SCI_0_SEND		    ;
#define ENABLE_HARD_SCI_1_RECE		    ;
#define ENABLE_HARD_SCI_1_SEND		    ;
#define ENABLE_HARD_SCI_2_RECE		    HT_GPIOE->PTSET|=(1<<8)
#define ENABLE_HARD_SCI_2_SEND		    HT_GPIOE->PTCLR|=(1<<8)
#define ENABLE_HARD_SCI_3_RECE		    ;
#define ENABLE_HARD_SCI_3_SEND		    ;
#define ENABLE_HARD_SCI_4_RECE		    ;
#define ENABLE_HARD_SCI_4_SEND		    ;
#define ENABLE_HARD_SCI_5_RECE		    ;
#define ENABLE_HARD_SCI_5_SEND		    ;
#define ENABLE_HARD_SCI_6_RECE		    ;
#define ENABLE_HARD_SCI_6_SEND		    ;
#define ENABLE_HARD_SCI_7_RECE		    ;
#define ENABLE_HARD_SCI_7_SEND		    ;
#define ENABLE_HARD_SCI_8_RECE		    ;
#define ENABLE_HARD_SCI_8_SEND		    ;

//////////////////////////////////////////////////////////////////////////
// 电源控制定义
//////////////////////////////////////////////////////////////////////////
// 采样芯片电源控制 
#define POWER_SAMPLE_OPEN				HT_GPIOE->PTDIR |= (1<<0);\
										HT_GPIOE->PTOD |= (1<<0);\
										HT_GPIOE->PTCLR |= (1<<0)
#define POWER_SAMPLE_CLOSE				HT_GPIOE->PTDIR |= (1<<0);\
										HT_GPIOE->PTOD |= (1<<0);\
										HT_GPIOE->PTSET |= (1<<0)	

//红外接收电源控制 低电平打开电源
#define POWER_HONGWAI_REC_OPEN			;
#define POWER_HONGWAI_REC_CLOSE			;

//红外发送电源控制 低电平打开电源
#define POWER_HONGWAI_TXD_OPEN			;
#define POWER_HONGWAI_TXD_CLOSE			;

//lcd电源控制
#define POWER_LCD_OPEN				    ;
#define POWER_LCD_CLOSE				    ;

// ESAM电源控制 
#define POWER_ESAM_OPEN				    HT_GPIOB->PTDIR |= (1<<10);\
										HT_GPIOB->PTOD |= (1<<10);\
										HT_GPIOB->PTCLR |= (1<<10)
#define POWER_ESAM_CLOSE			    HT_GPIOB->PTDIR |= (1<<10);\
										HT_GPIOB->PTOD |= (1<<10);\
										HT_GPIOB->PTSET |= (1<<10)
										
#define ESAM_POWER_IS_CLOSE             (HT_GPIOB->PTDAT & (1<<10))

//Flash电源控制
#define POWER_FLASH_OPEN				HT_GPIOA->PTDIR |= (1<<5);\
										HT_GPIOA->PTOD |= (1<<5);\
										HT_GPIOA->PTCLR |= (1<<5)
#define POWER_FLASH_CLOSE				HT_GPIOA->PTDIR |= (1<<5);\
										HT_GPIOA->PTOD |= (1<<5);\
										HT_GPIOA->PTSET |= (1<<5)

//霍尔传感器电源
#define POWER_HALL_OPEN				    ;
#define POWER_HALL_CLOSE	            ;

#define POWER_CV485_OPEN				;
#define POWER_CV485_CLOSE				;

#define POWER_F415_CV_OPEN 				HT_GPIOC->PTDIR |= (1<<8);\
										HT_GPIOC->PTOD |= (1<<8);\
										HT_GPIOC->PTSET |= (1<<8)
#define POWER_F415_CV_CLOSE				HT_GPIOC->PTDIR |= (1<<8);\
										HT_GPIOC->PTOD |= (1<<8);\
										HT_GPIOC->PTCLR |= (1<<8)

#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202303) || (BOARD_TYPE == BOARD_HT_SINGLE_78202401))
//低功耗上报通道电源控制
#define POWER_REPORT_CR_OPEN			POWER_FLASH_OPEN
#define POWER_REPORT_CR_CLOSE			POWER_FLASH_CLOSE
#else
//低功耗上报通道电源控制
#define POWER_REPORT_CR_OPEN			;
#define POWER_REPORT_CR_CLOSE			;
#endif
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
// 串口驱动结构定义
typedef struct
{
	BYTE SerialType;//串口类型 0: 485_I 1:IR 2:CR 3:485_II
	BYTE SCI_Ph_Num;								// 物理串口号
	BYTE (*SCIInit)( BYTE SCI_Ph_Num );				// 初始化串口
	BYTE (*SCIEnableRcv)( BYTE SCI_Ph_Num );		// 允许接收  
	BYTE (*SCIDisableRcv)( BYTE SCI_Ph_Num );		// 禁止接收接收
	BYTE (*SCIEnableSend)( BYTE SCI_Ph_Num );		// 允许发送
	BYTE (*SCIBeginSend)( BYTE SCI_Ph_Num );		// 开始发送
}TypeDef_Pulic_SCI;

//GPIO类型配置
typedef enum
{
	eGPIO_TYPE_COMMON = 0,			//普通IO
	eGPIO_TYPE_AF_FIRST,			//第一复用功能
	eGPIO_TYPE_AF_SECOND,			//第二复用功能
	eGPIO_TYPE_NONE					//无
}eGPIO_TYPE;

//GPIO方向配置
typedef enum
{
	eGPIO_DIRECTION_IN = 0,			//输入
	eGPIO_DIRECTION_OUT,			//输出
	eGPIO_DIRECTION_NONE			//无
}eGPIO_DIRECTION;

//GPIO模式配置
typedef enum
{
	eGPIO_MODE_PP = 0,				//推挽
	eGPIO_MODE_OD,					//开漏
	eGPIO_MODE_IPU,					//上拉
	eGPIO_MODE_IPD,					//下拉
	eGPIO_MODE_FLOATING,			//浮空
	eGPIO_MODE_NONE					//无
}eGPIO_MODE;

//GPIO输出配置
typedef enum
{
	eGPIO_OUTPUT_HIGH = 0,			//高
	eGPIO_OUTPUT_LOW,				//低
	eGPIO_OUTPUT_HI,				//高阻态
	eGPIO_OUTPUT_NONE				//无
}eGPIO_OUTPUT;

//GPIO初始化结构定义
typedef struct
{
	eGPIO_TYPE eGPIO_TYPE;				//GPIO的类型(参数取自 GPIO_TYPE 中定义的值)
	eGPIO_DIRECTION eGPIO_DIRECTION;	//GPIO的方向(参数取自 GPIO_DIRECTION 中定义的值)
	eGPIO_MODE eGPIO_MODE;				//GPIO的模式(参数取自 GPIO_MODE 中的定义值)
	eGPIO_OUTPUT eGPIO_OUTPUT;			//GPIO的输出(参数取自 GPIO_OUTPUT 中的定义值)
}GPIO_InitTypeDef;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE IsExitLowPower;//模块内变量
extern const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM];
extern WORD WakeUpTimer;
extern WORD MaxWakeupTimer;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

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
void InitPort( ePOWER_MODE Type );

//--------------------------------------------------
//功能描述:  关闭esam电源
//         
//参数  : 无
//
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void ESAMSPIPowerDown( ePOWER_MODE PowerMode );

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
BOOL ResetSPIDevice( eCOMPONENT_TYPE Type, BYTE Time );


//-----------------------------------------------
//函数功能: 初始化液晶驱动芯片
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void InitLCDDriver(void);

//-----------------------------------------------
//函数功能: 对CPU的AD进行配置
//
//参数: 	Type[in]:ePowerOnMode/ePowerWakeUpMode
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_AdcConfig( ePOWER_MODE Type );

//-----------------------------------------------
//函数功能: 用于检查维护cpu的寄存器值
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void Maintain_MCU( void );

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
short GetADValue( BYTE Mode, BYTE Type);


//-----------------------------------------------
//函数功能: 采样芯片URAT读取采样芯片一个字节
//
//参数:		无 
//                    
//返回值: 
//		   返回读取的数据，超时返回0
//备注:   
//-----------------------------------------------
BYTE Drv_SampleReadByte(void);

//-----------------------------------------------
//函数功能: 采样芯片URAT写入采样芯片一个字节
//
//参数:		Data[in] 要写到采样芯片的数据 
//                    
//返回值: 无
//		   
//备注:   
//-----------------------------------------------
void Drv_SampleWriteByte(BYTE Data);

//-----------------------------------------------
//函数功能: 对外围设备的电源控制
//
//参数:		Type[in] 0:上电时对外部电源的控制  1:其他
//                    
//返回值: 无
//		   
//备注:   
//-----------------------------------------------
void PowerCtrl( BYTE Type );

//-----------------------------------------------
//函数功能: 板件初始化函数
//
//参数:		无
//                    
//返回值: 无
//		   
//备注:   
//-----------------------------------------------
void InitBoard( void );

//-----------------------------------------------
//函数功能: SPI片选驱动函数
//
//参数:		No[in]		需要选择的芯片
//          Do[in]		TRUE：使能片选   FALSE：禁止片选
//
//返回值: 无
//		   
//备注:   
//-----------------------------------------------
void DoSPICS(WORD No, WORD Do);

//-----------------------------------------------
//函数功能: 开启计量模块的串口、初始化串口、发送首字节
//
//参数:		head[in]		要发送的第一个字节
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void Open_EMUUART(BYTE Head);

//-----------------------------------------------
//函数功能: 关闭计量模块的串口
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void Close_EMUUART(void);


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
BYTE api_MultiFunPortSet( BYTE Type );


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
void api_MultiFunPortCtrl( BYTE Type );


//-----------------------------------------------
//函数功能: systick中断服务程序，10ms中断一次
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void SysTickIRQ_Service(void);


//-----------------------------------------------
//函数功能: PMU中断处理函数
//
//参数:		无
//
//返回值: 	无
//
//备注:
//-----------------------------------------------
void PMUIRQ_Service( void );


//-----------------------------------------------
//函数功能: 外部中断0服务程序，目前用于脉冲采集
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void EXTI0IRQ_Service( void );

//目前没有用
void EXTI1IRQ_Service( void );

//目前没有用
void EXTI2IRQ_Service( void );

//目前没有用
void EXTI3IRQ_Service( void );

//目前没有用
void EXTI4IRQ_Service( void );

//目前没有用
void EXTI5IRQ_Service( void );

//目前没有用
void EXTI6IRQ_Service( void );

//-----------------------------------------------
//函数功能: 物理串口0中断服务程序，具体对应哪个逻辑串口，参看SerialMap配置
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void UART0IRQ_Service( void );

//-----------------------------------------------
//函数功能: 物理串口1中断服务程序，具体对应哪个逻辑串口，参看SerialMap配置
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void UART1IRQ_Service( void );

//-----------------------------------------------
//函数功能: 物理串口2中断服务程序，具体对应哪个逻辑串口，参看SerialMap配置
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void UART2IRQ_Service( void );

//目前没有用
void UART3IRQ_Service( void );

//目前没有用
void UART4IRQ_Service( void );

//目前没有用
void UART5IRQ_Service( void );

//目前没有用
void TIMER0IRQ_Service( void );

//目前没有用
void TIMER1IRQ_Service( void );

//目前没有用
void TIMER2IRQ_Service( void );

//目前没有用
void TIMER3IRQ_Service( void );

//-----------------------------------------------
//函数功能: 时钟中断服务程序，用于产生秒、分、时任务标志
//
//参数:		无
//
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void RTCIRQ_Service( void );

//目前没有用
void TIMER4IRQ_Service( void );

//目前没有用
void TIMER5IRQ_Service( void );

//目前没有用
void UART6IRQ_Service( void );

//目前没有用
void EXTI7IRQ_Service( void );

//目前没有用
void EXTI8IRQ_Service( void );

//目前没有用
void EXTI9IRQ_Service( void );

#endif//#if ( (BOARD_TYPE == BOARD_HT_SINGLE_78201662) || (BOARD_TYPE == BOARD_HT_SINGLE_78201602) )
//#endif
#endif//#ifndef __BOARD_115200_SINGLE_H
