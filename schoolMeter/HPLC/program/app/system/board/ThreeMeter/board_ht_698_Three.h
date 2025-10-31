//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		支持698.45单相表的印制板头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __BOARD_HT_698_THREE_H
#define __BOARD_HT_698_THREE_H

#if( ( BOARD_TYPE == BOARD_HT_THREE_0134566 ) || (BOARD_TYPE == BOARD_HT_THREE_0131699)\
 || (BOARD_TYPE == BOARD_HT_THREE_0130347)  || (BOARD_TYPE == BOARD_HT_THREE_0132515))


//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//系统支持的相数
//三相表(无论三相三线还是三相四线或直通都用3)  单相表都用1
#define MAX_PHASE		3

//定义CPU类型
#define CPU_TYPE		CPU_HT6025
//#define CPU_HT6025              	6

//数据源
#define SAMPLE_CHIP			CHIP_HT7038
//#define CHIP_HT7017			6
//#define CHIP_HT7038			7
//#define CHIP_HT7026			8

//时钟芯片类型
#define RTC_CHIP_TYPE		SOC_CLOCK
//SOC_CLOCK					SOC集成时钟
//OUT_CLOCK					外部独立时钟

//时钟芯片
#define RTC_CHIP			RTC_HT_INCLOCK
//RTC_CHIP_8025T			支持带温度补偿的8025
//RTC_HT_INCLOCK			矩泉内置RTC

//液晶类型选择
#define LCD_TYPE			LCD_GW3PHASE_09
//LCD_GW3PHASE_09			09年8月国网三相表液晶
//LCD_HT_SINGLE_METER		矩泉单相表国网液晶

//液晶驱动芯片地址线宽度
#define LCD_DRIVE_CHIP			LCD_CHIP_BU97950
//#define LCD_CHIP_1623					7
//#define LCD_CHIP_1622					6
//#define LCD_CHIP_BU97950				5
//#define LCD_CHIP_BU9792               4
//#define LCD_CHIP_PCF8576              3
//#define LCD_CHIP_BU9794               2
//#define LCD_CHIP_BU9799               1
//#define LCD_CHIP_NO                   0

//通讯类型选择，（近红外、远红外）
#define COMM_TYPE				COMM_FAR
//COMM_NEAR		近红外
//COMM_FAR		远红外

#define MASTER_MEMORY_CHIP			CHIP_24LC256
//CHIP_24LC256
//CHIP_24LC512
//CHIP_NO							没有焊存储芯片  只有第三片可以选没有

//第二片存储芯片(D4)
#define SLAVE_MEM_CHIP				CHIP_24LC256

//第三片存储器类型  只用于连续空间的数据存储
#define THIRD_MEM_CHIP				CHIP_GD25Q64C //内部无RAM兆晶Flash

//第三片存储器是否开启数据备份功能
#define SLE_THIRD_MEM_BACKUP		NO

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

//选择几路串口 只能选择4(硬件串口和模拟串口的总数)
#define MAX_COM_CHANNEL_NUM			4

//IIC是模拟的还是硬件的
#define SEL_24LC256_COMMUNICATION_MOD	IIC_SIM
//IIC_SIM
//IIC_HARD

//是否选择任意报文进行低功耗唤醒
#define SEL_CONTINUS_FRAM_WAKEUP		IR_WAKEUP_PHOTO_SWITCH

//-----------------------------------------------------------
//关联判断
//-----------------------------------------------------------
	

//////////////////////////////////////////////////////////////////////////
//							IIC管脚定义
//////////////////////////////////////////////////////////////////////////
//EEPROM的IIC定义
#define EEPROM_I2C_SDA_PORT_IN			HT_GPIOD->PTDIR &=~(1<<1)
#define EEPROM_I2C_SDA_PORT_DATA_IN		(HT_GPIOD->PTDAT & (1<<1))
#define EEPROM_SCL_L					HT_GPIOD->PTCLR |= (1<<2)				
#define EEPROM_SCL_H					HT_GPIOD->PTSET |= (1<<2);\
										HT_GPIOD->PTDIR |= (1<<2)
#define EEPROM_SDA_L					HT_GPIOD->PTCLR |= (1<<1);\
										HT_GPIOD->PTDIR |= (1<<1)
#define EEPROM_SDA_H					EEPROM_I2C_SDA_PORT_IN
                      
// EEPROM 写入保护管脚 有管脚连接，但软件一直置为写保护无效
#define ENABLE_WRITE_PROTECT1		    ;

#define ENABLE_WRITE_PROTECT1_FOR_ADDR	HT_GPIOD->PTDIR |= (1<<0);\
										HT_GPIOD->PTOD |= (1<<0);\
										HT_GPIOD->PTSET |= (1<<0)

#define DISABLE_WRITE_PROTECT1		    HT_GPIOD->PTDIR |= (1<<0);\
										HT_GPIOD->PTOD |= (1<<0);\
										HT_GPIOD->PTCLR |= (1<<0)
#define ENABLE_WRITE_PROTECT2		    ;
#define ENABLE_WRITE_PROTECT2_FOR_ADDR	HT_GPIOD->PTDIR |= (1<<3);\
										HT_GPIOD->PTOD |= (1<<3);\
										HT_GPIOD->PTSET |= (1<<3)

#define DISABLE_WRITE_PROTECT2		    HT_GPIOD->PTDIR |= (1<<3);\
										HT_GPIOD->PTOD |= (1<<3);\
										HT_GPIOD->PTCLR |= (1<<3)

//低功耗三相表开盖会操作EE，需要将管脚置为高阻态，防止功耗异常
#define EEPROM_INIT_LOW_POWER				HT_GPIOD->PTDIR |= ((1<<0)|(1<<1)|(1<<2)|(1<<3));\
											HT_GPIOD->PTOD &= ~((1<<0)|(1<<1)|(1<<2)|(1<<3));\
											HT_GPIOD->PTSET |= ((1<<0)|(1<<1)|(1<<2)|(1<<3))
											
//EEPROM 的地址
#define MEM_24LC25601_DEVADDR		    0xa0
#define MEM_24LC25602_DEVADDR		    0xae
#define MEM_24LC25601_CONFIG_DEVADDR	0xb0
#define MEM_24LC25602_CONFIG_DEVADDR	0xbe


//////////////////////////////////////////////////////////////////////////
// 液晶驱动芯片IIC定义
#define LCD_I2C_SDA_PORT_IN             HT_GPIOC->PTDIR &= ~(1<<14)
#define LCD_I2C_SDA_PORT_DATA_IN        (HT_GPIOC->PTDAT & (1<<14))
#define LCD_SCL_LOW                     HT_GPIOC->PTCLR |= (1<<13)
#define LCD_SCL_HIGH                    HT_GPIOC->PTSET |= (1<<13);\
										HT_GPIOC->PTDIR |= (1<<13)
#define LCD_SDA_LOW                     HT_GPIOC->PTCLR |= (1<<14);\
										HT_GPIOC->PTDIR |= (1<<14)
#define LCD_SDA_HIGH                    LCD_I2C_SDA_PORT_IN

/////////////////////////////////////////////////////////////////////////
//时钟芯片的IIC定义
#define CLOCK_I2C_SDA_PORT_IN             	HT_GPIOB->PTDIR &= ~(1<<5)
#define CLOCK_I2C_SDA_PORT_DATA_IN        	(HT_GPIOB->PTDAT & (1<<5))
#define CLOCK_SCL_L                     	HT_GPIOB->PTCLR |= (1<<4)
#define CLOCK_SCL_H                    		HT_GPIOB->PTSET |= (1<<4);\
											HT_GPIOB->PTDIR |= (1<<4)
#define CLOCK_SDA_L                     	HT_GPIOB->PTCLR |= (1<<5);\
											HT_GPIOB->PTDIR |= (1<<5)
#define CLOCK_SDA_H                    		CLOCK_I2C_SDA_PORT_IN

//////////////////////////////////////////////////////////////////////////
//SPI CS配置
// FLASH片选管脚    
#define FLASH_CS_ENABLE				    HT_GPIOB->PTCLR |= (1<<10)
#define FLASH_CS_DISABLE			    HT_GPIOB->PTSET |= (1<<10)
// 计量芯片片选管脚 
#define SAMPLE_CS_ENABLE			    HT_GPIOC->PTCLR |= (1<<7)
#define SAMPLE_CS_DISABLE			    HT_GPIOC->PTSET |= (1<<7)
// ESAM芯片片选管脚 
#define ESAM_CS_ENABLE			    	HT_GPIOD->PTCLR |= (1<<14)
#define ESAM_CS_DISABLE			    	HT_GPIOD->PTSET |= (1<<14)

//计量模块                     
#define EMU_UARTSTA						;
#define EMU_UARTBUF						;

// 载波模块控制管脚定义
#define ENABLE_PLC_PROGRAMM         	HT_GPIOA->PTDIR |= (1<<2);\
										HT_GPIOA->PTCLR |= (1<<2)
#define DISABLE_PLC_PROGRAMM        	HT_GPIOA->PTDIR |= (1<<2);\
										HT_GPIOA->PTSET |= (1<<2)

//载波发生状态检测 高电平表示载波发生
#define CHECK_CARRRWAVE_SENDING     	(HT_GPIOA->PTDAT & (1<<3))
#define RESET_PLC_MODUAL            	HT_GPIOA->PTDIR |= (1<<1);\
										HT_GPIOA->PTCLR |= (1<<1);\
										api_Delayms( 250 );\
										HT_GPIOA->PTSET |= (1<<1)

//模块事件通知 高电平表示有事件发生 低电平表示没有事件发生
#define CANCEL_EVENTOUT			    HT_GPIOA->PTDIR |= (1<<0);\
									HT_GPIOA->PTCLR |= (1<<0)
#define NOTICE_EVENTOUT			    HT_GPIOA->PTDIR |= (1<<0);\
									HT_GPIOA->PTSET |= (1<<0)

//多功能端子输出低
#define MULTI_FUN_PORT_OUT_L			HT_GPIOC->IOCFG &= ~(1<<8);\
										HT_GPIOC->PTDIR |= (1<<8);\
										HT_GPIOC->PTCLR |= (1<<8)
										
//多功能端子输出高
#define MULTI_FUN_PORT_OUT_H			HT_GPIOC->IOCFG &= ~(1<<8);\
										HT_GPIOC->PTDIR |= (1<<8);\
										HT_GPIOC->PTSET |= (1<<8)
										
//切换到时钟脉冲输出
#define SWITCH_TO_CLOCK_PULS_OUTPUT		HT_GPIOC->IOCFG |= (1<<8)

//点亮背光     
#define	OPEN_BACKLIGHT			        if( SelThreeBoard == BOARD_HT_THREE_0134566 )\
										{\
											HT_GPIOB->PTDIR |= (1<<3);\
											HT_GPIOB->PTCLR |= (1<<3);\
										}\
										else\
										{\
											HT_GPIOB->PTDIR |= (1<<3);\
											HT_GPIOB->PTSET |= (1<<3);\
										}
	
#define	CLOSE_BACKLIGHT			       	if( SelThreeBoard == BOARD_HT_THREE_0134566 )\
										{\
											HT_GPIOB->PTDIR |= (1<<3);\
											HT_GPIOB->PTSET |= (1<<3);\
										}\
										else\
										{\
											HT_GPIOB->PTDIR |= (1<<3);\
											HT_GPIOB->PTCLR |= (1<<3);\
										}	
//跳闸指示灯     
#define RELAY_LED_ON			       	 HT_GPIOD->PTCLR |= (1<<12)                                          
#define RELAY_LED_OFF			         HT_GPIOD->PTSET |= (1<<12)
//计量芯片脉冲输入
#define EMU_ACTIVE						;
//有功脉冲输出指示灯
#define PULSE_LED_ON			        ;
#define PULSE_LED_OFF			        ;
//报警指示灯        
#define WARN_ALARM_LED_ON		        ;//HT_GPIOD->PTCLR |= (1<<7)
#define WARN_ALARM_LED_OFF		        ;//HT_GPIOD->PTSET |= (1<<7)
//蜂鸣器
#if(PREPAY_MODE == PREPAY_LOCAL)
//普通端口 开漏输出高
#define BEEP_OFF						HT_GPIOE->IOCFG &= ~(1<<0);\
										HT_GPIOE->PTDIR |= (1<<0);\
										HT_GPIOE->PTOD &= ~(1<<0);\
										HT_GPIOE->PTSET |= (1<<0);
//第一复用功能(PWM输出)
#define BEEP_ON							HT_GPIOE->IOCFG |= (1<<0);\
										HT_GPIOE->AFCFG &= ~(1<<0);\
										HT_GPIOE->PTDIR |= (1<<0);\
										HT_GPIOE->PTOD |= (1<<0);
#endif

//辅助电源是否掉电检测
#define SECPOWER_IS_DOWN	        	(HT_GPIOD->PTDAT & (1<<9))

//报警继电器动作
#define WARN_RELAY_OPEN_4566					HT_GPIOB->PTSET |= (1<<1);\
												HT_GPIOB->PTDIR |= (1<<1);
//报警继电器取消
#define WARN_RELAY_CLOSE_4566					HT_GPIOB->PTCLR |= (1<<1);\
												HT_GPIOB->PTDIR |= (1<<1);

//合闸 低电平有效但不能同时为低--实验在248v，三相加电时30ms可驱动继电器
#define CLOSE_POWER_RELAY_PLUS_INSIDE_4566		HT_GPIOC->PTSET|=(1<<9);HT_GPIOC->PTCLR|=(1<<10);\
												HT_GPIOC->PTDIR|=(1<<9);HT_GPIOC->PTDIR|=(1<<10);\
												api_Delayms(120);\
												HT_GPIOC->PTSET|=(1<<9);HT_GPIOC->PTSET|=(1<<10);
//跳闸 低电平有效但不能同时为低
#define OPEN_POWER_RELAY_PLUS_INSIDE_4566		HT_GPIOC->PTSET|=(1<<10);HT_GPIOC->PTCLR|=(1<<9);\
												HT_GPIOC->PTDIR|=(1<<9);HT_GPIOC->PTDIR|=(1<<10);\
												api_Delayms(120);\
												HT_GPIOC->PTSET|=(1<<9);HT_GPIOC->PTSET|=(1<<10);
//外置继电器合闸
#define CLOSE_POWER_RELAY_LEVEL_OUTSIDE_4566 	HT_GPIOC->PTCLR|=(1<<10);\
												HT_GPIOC->PTDIR|=(1<<10);
//外置继电器跳闸
#define OPEN_POWER_RELAY_LEVEL_OUTSIDE_4566		HT_GPIOC->PTSET|=(1<<10);\
												HT_GPIOC->PTDIR|=(1<<10);
//外置继电器跳闸(脉冲方式)
#define OPEN_POWER_RELAY_PLUS_OUTSIDE_4566		HT_GPIOC->PTCLR|=(1<<10);\
												HT_GPIOC->PTDIR|=(1<<10);\
												api_Delayms(400);\
												HT_GPIOC->PTSET|=(1<<9);\
												HT_GPIOC->PTSET|=(1<<10);  
//用于防止低功耗初始化管脚时突然上电导致继电器跳闸
#define LOWPOWER_INVALID_RELAY_CTRL_4566		HT_GPIOC->PTOD |= 0x0600;\
												HT_GPIOC->PTCLR |= 0x0600;\
												HT_GPIOC->PTDIR |= 0x0600 

//报警继电器动作
#define WARN_RELAY_OPEN				    		WARN_RELAY_OPEN_4566

//报警继电器取消
#define WARN_RELAY_CLOSE						WARN_RELAY_CLOSE_4566
//合闸 高电平有效但不能同时为高
#define CLOSE_POWER_RELAY_PLUS_INSIDE			CLOSE_POWER_RELAY_PLUS_INSIDE_4566

//跳闸 高电平有效但不能同时为高
#define OPEN_POWER_RELAY_PLUS_INSIDE			OPEN_POWER_RELAY_PLUS_INSIDE_4566

//外置继电器合闸
#define CLOSE_POWER_RELAY_LEVEL_OUTSIDE			CLOSE_POWER_RELAY_LEVEL_OUTSIDE_4566

//外置继电器跳闸
#define OPEN_POWER_RELAY_LEVEL_OUTSIDE			OPEN_POWER_RELAY_LEVEL_OUTSIDE_4566

//外置继电器跳闸(脉冲方式)
#define OPEN_POWER_RELAY_PLUS_OUTSIDE			OPEN_POWER_RELAY_PLUS_OUTSIDE_4566

//用于防止低功耗初始化管脚时突然上电导致继电器跳闸
#define LOWPOWER_INVALID_RELAY_CTRL				LOWPOWER_INVALID_RELAY_CTRL_4566


//内置继电器状态检测
#define RELAY_STATUS_OPEN	        (!(HT_GPIOD->PTDAT & (1<<9)))


//////////////////////////////////////////////////////////////////////////
//							输入相关管脚操作
//////////////////////////////////////////////////////////////////////////
//获取按键的状态 按下是高电平
#define UP_KEY_PRESSED          ((HT_GPIOA->PTDAT & (1<<10)))//上翻按键
#define DOWN_KEY_PRESSED        ((HT_GPIOA->PTDAT & (1<<11)))
#define PROG_KEY_PRESSED        0//编程按键
// 强磁场检测 低电平表示检测到
#define CHECK_MAGNETIC_STATUS   (!(HT_GPIOD->PTDAT & (1<<6)))
#define MAGNETIC_IS_OCCUR       CHECK_MAGNETIC_STATUS
// 开上盖盖输入 高电平检测到开盖
#define UP_COVER_IS_OPEN        (!(HT_GPIOA->PTDAT & (1<<7)))
#define UP_COVER_IS_CLOSE       ((HT_GPIOA->PTDAT & (1<<7)))
#define ENABLE_UP_COVER_INT     ;
#define DIS_UP_COVER_INT        ;
// 开尾盖输入 高电平检测到开盖
#define END_COVER_IS_OPEN       (!(HT_GPIOA->PTDAT & (1<<8)))
#define END_COVER_IS_CLOSE      ((HT_GPIOA->PTDAT & (1<<8)))
#define ENABLE_END_COVER_INT    ;
#define DIS_END_COVER_INT       ;

#if(PREPAY_MODE == PREPAY_LOCAL)
#define CARD_IN_SLOT       		(!(HT_GPIOE->PTDAT &0x0100))//卡在卡槽中，触发了IC_Key
#endif  

///////////////////////////////////////////////////////////////////////////
// 485串口方向控制定义
//////////////////////////////////////////////////////////////////////////
#define ENABLE_HARD_SCI_0_RECE		    ;
#define ENABLE_HARD_SCI_0_SEND		    ;
#define ENABLE_HARD_SCI_1_RECE		    ;
#define ENABLE_HARD_SCI_1_SEND		    ;
#define ENABLE_HARD_SCI_2_RECE		    HT_GPIOB->PTSET=(1<<0)
#define ENABLE_HARD_SCI_2_SEND		    HT_GPIOB->PTCLR=(1<<0)
#define ENABLE_HARD_SCI_3_RECE		    HT_GPIOE->PTSET=(1<<6)
#define ENABLE_HARD_SCI_3_SEND		    HT_GPIOE->PTCLR=(1<<6)
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
// 采样芯片电源控制 低电平打开电源
#define POWER_SAMPLE_OPEN				HT_GPIOA->PTDIR |= (1<<4);\
										HT_GPIOA->PTOD |= (1<<4);\
										HT_GPIOA->PTCLR |= (1<<4)
#define POWER_SAMPLE_CLOSE				HT_GPIOA->PTDIR |= (1<<4);\
										HT_GPIOA->PTOD |= (1<<4);\
										HT_GPIOA->PTSET |= (1<<4)	

//eeprom电源控制 低电平打开电源
#define POWER_FLASH_OPEN				HT_GPIOD->PTDIR |= (1<<5);\
										HT_GPIOD->PTOD |= (1<<5);\
										HT_GPIOD->PTCLR |= (1<<5)	
#define POWER_FLASH_CLOSE				HT_GPIOD->PTDIR |= (1<<5);\
										HT_GPIOD->PTOD |= (1<<5);\
										HT_GPIOD->PTSET |= (1<<5)
										
#define POWER_EEPROM_OPEN				if( SelThreeBoard != BOARD_HT_THREE_0132515 )\
										{\
											POWER_FLASH_OPEN;\
										}
#define POWER_EEPROM_CLOSE				if( SelThreeBoard != BOARD_HT_THREE_0132515 )\
										{\
											POWER_FLASH_CLOSE;\
										}

//红外接收电源控制 低电平打开电源
#define POWER_HONGWAI_REC_OPEN			HT_GPIOD->PTDIR |= (1<<10);\
										HT_GPIOD->PTOD |= (1<<10);\
										HT_GPIOD->PTCLR |= (1<<10)	
#define POWER_HONGWAI_REC_CLOSE			HT_GPIOD->PTDIR |= (1<<10);\
										HT_GPIOD->PTOD |= (1<<10);\
										HT_GPIOD->PTSET |= (1<<10)		

//红外发送电源控制 低电平打开电源
#define POWER_HONGWAI_TXD_OPEN			;//HT_GPIOE->PTDIR |= (1<<8);\
										//HT_GPIOE->PTOD |= (1<<8);\
										//HT_GPIOE->PTCLR |= (1<<8)	
#define POWER_HONGWAI_TXD_CLOSE			;//HT_GPIOE->PTDIR |= (1<<8);\
										//HT_GPIOE->PTOD |= (1<<8);\
										//HT_GPIOE->PTSET |= (1<<8)		

//lcd电源控制 高电平打开电源
#define POWER_LCD_OPEN				    HT_GPIOD->PTDIR |= (1<<15);\
										HT_GPIOD->PTOD |= (1<<15);\
										HT_GPIOD->PTSET |= (1<<15)	
#define POWER_LCD_CLOSE				    HT_GPIOD->PTDIR |= (1<<15);\
										HT_GPIOD->PTOD |= (1<<15);\
										HT_GPIOD->PTCLR |= (1<<15)	
// ESAM电源控制 低电平打开电源
#define POWER_ESAM_OPEN				    HT_GPIOD->PTDIR |= (1<<11);\
										HT_GPIOD->PTOD |= (1<<11);\
										HT_GPIOD->PTCLR |= (1<<11)
#define POWER_ESAM_CLOSE			    HT_GPIOD->PTDIR |= (1<<11);\
										HT_GPIOD->PTOD |= (1<<11);\
										HT_GPIOD->PTSET |= (1<<11)

#define ESAM_POWER_IS_CLOSE             (HT_GPIOD->PTDAT & (1<<11))

//霍尔传感器电源 低电平打开电源
#define POWER_HALL_OPEN				    ;//HT_GPIOD->PTDIR |= (1<<8);\
										//HT_GPIOD->PTOD |= (1<<8);\
										//HT_GPIOD->PTCLR |= (1<<8)
#define POWER_HALL_CLOSE	            ;//HT_GPIOD->PTDIR |= (1<<8);\
										//HT_GPIOD->PTOD |= (1<<8);\
										//HT_GPIOD->PTSET |= (1<<8)
										
#define POWER_CV485_OPEN				HT_GPIOB->PTDIR |= (1<<2);\
										HT_GPIOB->PTOD |= (1<<2);\
										HT_GPIOB->PTCLR |= (1<<2)
#define POWER_CV485_CLOSE				HT_GPIOB->PTDIR |= (1<<2);\
										HT_GPIOB->PTOD |= (1<<2);\
										HT_GPIOB->PTSET |= (1<<2)
										
#if( PREPAY_MODE == PREPAY_LOCAL )
//拉低复位引脚5ms，复位CPU卡，三相表目前的硬件是用这个引脚
#define RESET_CARD						HT_GPIOB->PTDIR |= (1<<12);\
										HT_GPIOB->PTOD |= (1<<12);\
										HT_GPIOB->PTCLR |= (1<<12);\
										api_Delayms(5);\
										HT_GPIOB->PTDIR |= (1<<12);\
										HT_GPIOB->PTOD |= (1<<12);\
										HT_GPIOB->PTSET |= (1<<12)
										
//CARD电源控制 低电平打开电源 只有ESAM电源打开后 CARD电源才有效
#define POWER_CARD_OPEN					HT_GPIOE->PTDIR |= (1<<3);\
										HT_GPIOE->PTOD |= (1<<3);\
										HT_GPIOE->PTCLR |= (1<<3)  										

#define POWER_CARD_CLOSE				HT_GPIOE->PTDIR |= (1<<3);\
										HT_GPIOE->PTOD |= (1<<3);\
										HT_GPIOE->PTSET |= (1<<3) 
										
#endif//if( PREPAY_MODE == PREPAY_LOCAL )

//低功耗上报通道电源控制
#define	POWER_REPORT_CR_OPEN			if( SelThreeBoard != BOARD_HT_THREE_0132515 )\
										{\
											HT_GPIOD->PTDIR |= (1<<13);\
											HT_GPIOD->PTOD |= (1<<13);\
											HT_GPIOD->PTSET |= (1<<13);\
										}
										
#define	POWER_REPORT_CR_CLOSE			if( SelThreeBoard != BOARD_HT_THREE_0132515 )\
										{\
											HT_GPIOD->PTDIR |= (1<<13);\
											HT_GPIOD->PTOD &= ~(1<<13);\
											HT_GPIOD->PTUP |= (1<<13);\
										}
							
//开盖检测电源 高电平打开电源
#define POWER_COVER_OPEN				if( SelThreeBoard == BOARD_HT_THREE_0132515 )\
										{\
											HT_GPIOD->PTDIR |= (1<<13);\
											HT_GPIOD->PTOD |= (1<<13); \
											HT_GPIOD->PTSET = (1<<13);\
										}  										

#define POWER_COVER_CLOSE				if( SelThreeBoard == BOARD_HT_THREE_0132515 )\
										{\
											HT_GPIOD->PTDIR |= (1<<13);\
											HT_GPIOD->PTOD |= (1<<13);\
											HT_GPIOD->PTCLR =  (1<<13);\
										}
#define POWER_COVER_INT					(HT_GPIOD->PTDAT & (1<<13))
						

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
extern DWORD LowPowerSecCount;     //低功耗累计秒数

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
//功能描述:  与单相函数对应，因三相SPI复用，无法关闭esam电源，空函数
//         
//参数  : 无
//
//返回值:    
//         
//备注内容:  空函数 无作用
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
//函数功能: 关闭显示
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void ShutOff_Lcd(void);

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

void HTMCU_GoToLowSpeed(void);
#endif//#if( (BOARD_TYPE == BOARD_HT_THREE_0134566) )

#endif//#ifndef __BOARD_HT_698_THREE_H


