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
#define CPU_TYPE		CPU_HC32F460
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
//							IIC管脚定义
//////////////////////////////////////////////////////////////////////////


    
//EEPROM 的地址
#define MEM_24LC25601_DEVADDR			0xa0
#define MEM_24LC25602_DEVADDR			0xa0



//////////////////////////////////////////////////////////////////////////
//SPI CS配置
// FLASH片选管脚
#define FLASH_CS_ENABLE					PORT_ResetPortData(PortH,Pin02)
#define FLASH_CS_DISABLE			    PORT_SetPortData(PortH,Pin02)
// 计量芯片片选管脚 
#define SAMPLE_CS_ENABLE			    PORT_ResetPortData(PortB,Pin08);
#define SAMPLE_CS_DISABLE			    PORT_SetPortData(PortB,Pin08);
// ESAM芯片片选管脚 
#define ESAM_CS_ENABLE			    	PORT_ResetPortData(PortE,Pin06);
#define ESAM_CS_DISABLE			    	PORT_SetPortData(PortE,Pin06);

// 运行灯开关
#define LIGHT_RUN_OPEN					PORT_SetPortData(PortB,Pin05)
#define LIGHT_RUN_STOP					PORT_ResetPortData(PortB,Pin05)



//报警指示灯        
#define WARN_ALARM_LED_ON		        PORT_ResetPortData(PortB,Pin04)
#define WARN_ALARM_LED_OFF		        PORT_SetPortData(PortB,Pin04)


//////////////////////////////////////////////////////////////////////////
//							输入相关管脚操作
//////////////////////////////////////////////////////////////////////////
//获取按键的状态 按下是高电平
#define UP_KEY_PRESSED          0//((HT_GPIOD->PTDAT & (1<<14)))//上翻按键
#define DOWN_KEY_PRESSED        0//((HT_GPIOD->PTDAT & (1<<13)))
#define PROG_KEY_PRESSED        0//编程按键
// 强磁场检测 低电平表示检测到
#define CHECK_MAGNETIC_STATUS   0//(!(HT_GPIOD->PTDAT & (1<<6)))
#define MAGNETIC_IS_OCCUR       CHECK_MAGNETIC_STATUS
// 开上盖盖输入 高电平检测到开盖
#define UP_COVER_IS_OPEN        0//(!(HT_GPIOA->PTDAT & (1<<4)))
#define UP_COVER_IS_CLOSE       0//((HT_GPIOA->PTDAT & (1<<4)))
#define ENABLE_UP_COVER_INT     ;
#define DIS_UP_COVER_INT        ;
// 开尾盖输入 高电平检测到开盖
#define END_COVER_IS_OPEN       0//(!(HT_GPIOD->PTDAT & (1<<15)))
#define END_COVER_IS_CLOSE      0//((HT_GPIOD->PTDAT & (1<<15)))
#define ENABLE_END_COVER_INT    ;
#define DIS_END_COVER_INT       ;

///////////////////////////////////////////////////////////////////////////
// 485串口方向控制定义
//////////////////////////////////////////////////////////////////////////
#define ENABLE_HARD_SCI_0_RECE		    
#define ENABLE_HARD_SCI_0_SEND		    
#define ENABLE_HARD_SCI_1_RECE		    PORT_ResetPortData(PortB, Pin01)
#define ENABLE_HARD_SCI_1_SEND		    PORT_SetPortData(PortB, Pin01)
#define ENABLE_HARD_SCI_2_RECE		    PORT_ResetPortData(PortB, Pin10)
#define ENABLE_HARD_SCI_2_SEND		    PORT_SetPortData(PortB, Pin10)
#define ENABLE_HARD_SCI_3_RECE		    //HT_GPIOE->PTCLR=(1<<3)
#define ENABLE_HARD_SCI_3_SEND		    //HT_GPIOE->PTSET=(1<<3)
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
// 采样芯片复位控制 低电平复位
#define RESET_SAMPLE_OPEN				PORT_ResetPortData(PortA,Pin01)
#define RESET_SAMPLE_CLOSE				PORT_SetPortData(PortA,Pin01)

//eeprom电源控制 低电平打开电源
#define POWER_FLASH_OPEN				PORT_ResetPortData(PortB,Pin07)
#define POWER_FLASH_CLOSE				PORT_SetPortData(PortB,Pin07)
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


