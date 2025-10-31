//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		支持698.45单相表的印制板头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __BOARD_HC_MEASURINGSWITCH_H
#define __BOARD_HC_MEASURINGSWITCH_H

#if (BOARD_TYPE == BOARD_HC_MEASURINGSWITCH)

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//定义CPU类型
#define CPU_TYPE		CPU_HC32F4A0
//#define CPU_HT6025              	6

#define MASTER_MEMORY_CHIP			CHIP_24LC512
//CHIP_24LC256
//CHIP_24LC512
//CHIP_AT45DB161					支持45Db161芯片//只有第三片支持这种芯片
//CHIP_AT45DB321					支持45Db321芯片//只有第三片支持这种芯片
//CHIP_NO							没有焊存储芯片  只有第三片可以选没有

//第二片存储芯片(D4)
#define SLAVE_MEM_CHIP				CHIP_NO

//第三片存储器类型  只用于连续空间的数据存储
#define THIRD_MEM_CHIP				CHIP_GD25Q64C



//-----------------------------------------------------------
//关联判断
//-----------------------------------------------------------
	
//三相表没有零火线不平衡事件
#undef		SEL_EVENT_SP_I_UNBALANCE
#define		SEL_EVENT_SP_I_UNBALANCE			NO

//////////////////////////////////////////////////////////////////////////
//							XTAL管脚定义
//////////////////////////////////////////////////////////////////////////
/* BSP XTAL Configure definition */
#define BSP_XTAL_PORT                   (GPIO_PORT_H)
#define BSP_XTAL_IN_PIN                 (GPIO_PIN_00)
#define BSP_XTAL_OUT_PIN                (GPIO_PIN_01)


//////////////////////////////////////////////////////////////////////////
//							IIC管脚定义
//////////////////////////////////////////////////////////////////////////


    
//EEPROM 的地址
#define MEM_24LC25601_DEVADDR			0xa0
#define MEM_24LC25602_DEVADDR			0xae

//EEPROM的IIC定义
#define EEPROM_I2C_SDA_PORT_IN			CM_GPIO->PWPR = 0xA501u;\
										CM_GPIO->PCRB9 = 0x0400;\
										CM_GPIO->PWPR = 0xA500u	//解锁PORT; 初始化，默认为浮空输入，内部上拉无效;  锁定PORT
#define EEPROM_I2C_SDA_PORT_DATA_IN		GPIO_ReadInputPins(PortB,Pin09)
#define EEPROM_SCL_L					GPIO_ResetPins(PortB,Pin08)
#define EEPROM_SCL_H					GPIO_SetPins(PortB,Pin08)
#define EEPROM_SDA_L					CM_GPIO->PWPR = 0xA501u;\
										CM_GPIO->PCRB9 = 0x0406;\
										CM_GPIO->PWPR = 0xA500u//解锁PORT   开漏,数字输出有效,输出许可,输出低  锁定PORT
#define EEPROM_SDA_H					EEPROM_I2C_SDA_PORT_IN

//////////////////////////////////////////////////////////////////////////
//SPI CS配置
// FLASH片选管脚
#define FLASH_CS_ENABLE					GPIO_ResetPins(PortC,Pin10)
#define FLASH_CS_DISABLE			    GPIO_SetPins(PortC,Pin10)

// 计量芯片片选管脚 
#define SAMPLE_CS_ENABLE			    //GPIO_ResetPins(PortC,Pin05)//HT_GPIOA->PTCLR |= (1<<5)
#define SAMPLE_CS_DISABLE			    //GPIO_SetPins(PortC,Pin05)//HT_GPIOA->PTSET |= (1<<5)

// ESAM芯片片选管脚 
#define ESAM_CS_ENABLE			    	;
#define ESAM_CS_DISABLE			    	;

// 运行灯开关
#define LIGHT_RUN_OPEN					GPIO_SetPins(PortD,Pin07)
#define LIGHT_RUN_STOP					GPIO_ResetPins(PortD,Pin07)

// 报警灯开关
#define WARN_ALARM_LED_ON				GPIO_SetPins(PortD,Pin06)
#define WARN_ALARM_LED_OFF				GPIO_ResetPins(PortD,Pin06)

// 通讯灯开关
#define LIGHT_485RX_OPEN				GPIO_SetPins(PortD, Pin03)	
#define LIGHT_485RX_CLOSE				GPIO_ResetPins(PortD, Pin03) 
        
#define LIGHT_485TX_OPEN		        GPIO_SetPins(PortD,Pin02)
#define LIGHT_485TX_CLOSE		        GPIO_ResetPins(PortD,Pin02)

#define LIGHT_CANRX_OPEN				GPIO_SetPins(PortD, Pin05)	
#define LIGHT_CANRX_CLOSE				GPIO_ResetPins(PortD, Pin05) 
        
#define LIGHT_CANTX_OPEN		        GPIO_SetPins(PortD,Pin04)
#define LIGHT_CANTX_CLOSE		        GPIO_ResetPins(PortD,Pin04)

#define LIGHT_BLERX_OPEN				GPIO_SetPins(PortD, Pin01)	
#define LIGHT_BLERX_CLOSE				GPIO_ResetPins(PortD, Pin01) 
        
#define LIGHT_BLETX_OPEN		        GPIO_SetPins(PortD, Pin00)
#define LIGHT_BLETX_CLOSE		        GPIO_ResetPins(PortD, Pin00)

//计量模块                     
#define EMU_UARTSTA						;
#define EMU_UARTBUF						;

// 载波模块控制管脚定义
#define ENABLE_PLC_PROGRAMM         	GPIO_ResetPins(PortC, Pin13);
#define DISABLE_PLC_PROGRAMM        	GPIO_SetPins(PortC,Pin13);

//载波发生状态检测 高电平表示载波发生
#define CHECK_CARRRWAVE_SENDING     	GPIO_ReadInputPins(PortE,Pin06)
#define RESET_PLC_MODUAL            	GPIO_ResetPins(PortE, Pin05);\
										api_Delayms( 250 );\
										GPIO_SetPins(PortE,Pin05)

//模块事件通知 高电平表示有事件发生 低电平表示没有事件发生
#define CANCEL_EVENTOUT			    	GPIO_ResetPins(PortE, Pin04);
#define NOTICE_EVENTOUT			    	GPIO_SetPins(PortE, Pin04);

//////////////////////////////////////////////////////////////////////////
//							输入相关管脚操作
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 电源控制定义
//////////////////////////////////////////////////////////////////////////
//flash电源控制
#define POWER_FLASH_OPEN				GPIO_ResetPins(PortA,Pin15)
#define POWER_FLASH_CLOSE				GPIO_SetPins(PortA,Pin15)
										
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//指示灯控制枚举
typedef enum
{
	eLCD_Rx,
	eLCD_Tx,
	eLCD_V,
	eLCD_REV,
} ElCDCONTROL;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------


//-----------------------------------------------
// 				函数声明
//-----------------------------------------------


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
void InitPort(ePOWER_MODE Type);

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
//
//参数: 	Type[in]			获取AD数据的类型
//				SYS_POWER_AD		    		0//系统电压 保留二位小数 单位v
//				SYS_CLOCK_VBAT_AD		    	1//时钟电池电压 保留二位小数 单位v
//				SYS_READ_VBAT_AD	    		2//抄表电池的电压 保留二位小数 单位v
//				SYS_TEMPERATURE_AD		    	3//温度AD采样 保留一位小数
//                    
//返回值:  	AD转换的结果，只有温度支持负值，其他不支持负值。温度的负值采用补码表示
//
//备注:   
//-----------------------------------------------
short GetADValue(BYTE Type);

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
//函数功能:     查询端口对应串口
//
//参数:       Type[in] ：类型 eSERIAL_TYPE
//          
//
//返回值:   查表查得的串口号
//         
//备注:   返回< MAX_COM_CHANNEL_NUM 正确 ；等于MAX_COM_CHANNEL_NUM错误
//-----------------------------------------------
BYTE api_GetSerialIndex( BYTE Type );


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
//函数功能: 通信灯控制
//
//参数:		eType[in]:哪个灯
//			bStatus[in]: 0 - 关，1 - 开
//
//返回值: 	无
//
//备注:   借用别的灯进行通信指示
//-----------------------------------------------
void api_ProLightControl(ElCDCONTROL eType, BYTE bStatus);

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

#endif //#if ( (BOARD_TYPE == BOARD_HC_MEASURINGSWITCH) )

#endif//#ifndef BOARD_HC_MEASURINGSWITCH_H


