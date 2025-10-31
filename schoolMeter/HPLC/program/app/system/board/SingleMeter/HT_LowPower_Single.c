//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.26
//描述		矩泉cpu单相表低功耗接口文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT_LowPower_Single.h"

#if( (CPU_TYPE == CPU_HT6025) && (MAX_PHASE == 1) ) //暂时用cpu定义，可以考虑用单三相定义
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//唤醒计时器
WORD WakeUpTimer;
WORD MaxWakeupTimer;

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 在唤醒状态下的低功耗任务，定时进入低功耗状态
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_LowPowerTask(void)
{
	//1秒钟到
	if( api_GetTaskFlag(eTASK_LOWPOWER_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_LOWPOWER_ID,eFLAG_SECOND);
		
		#if( SEL_EVENT_METERCOVER == YES )
		ProcLowPowerMeterCoverRecord();//开表盖检测
		#endif
		#if( SEL_EVENT_BUTTONCOVER == YES )
		ProcLowPowerButtonCoverRecord();//开端钮盖检测
		#endif

		WakeUpTimer ++;

		if( (WakeUpTimer >= MaxWakeupTimer) || (WakeUpTimer >= 500) )//500秒强制进入低功耗
		{
			WakeUpTimer = 0;

			//是低功耗状态，进行低功耗状态处理
			api_EnterLowPower(eFromWakeupEnterDownMode);
		}
	}
}

//---------------------------------------------------------------
//函数功能: 低功耗按键配置
//
//参数: 	ePOWER_MODE Type - 模式(上电/低功耗/低功耗唤醒)
//
//返回值:
//
//备注:
//---------------------------------------------------------------
void LowPowerKeyConfig( ePOWER_MODE Type )
{
	#if ( (BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303)\
		|| (BOARD_TYPE == BOARD_HT_SINGLE_78202401) )
	if( Type == ePowerDownMode )
	{
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE = 0;
		HT_INT->EXTIE |= 0x0040;		//使能下降沿中断
		HT_INT->PINFLT |= 0x0040;		//使能数字滤波
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI6_IRQn );
		
		//使能开盖唤醒
		#if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
		HT_GPIOA->IOCFG |= GPIO_Pin_10;
		HT_GPIOA->AFCFG &= ~(GPIO_Pin_10);
		HT_GPIOA->PTDIR &= ~GPIO_Pin_10;
		HT_GPIOA->PTUP	|= GPIO_Pin_10;
		NVIC_DisableIRQ( EXTI5_IRQn );
		HT_INT->EXTIE |= 0x2020; //使能双边沿触发
		HT_INT->PINFLT |= 0x0020; //数字滤波
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI5_IRQn );
		#else
		HT_GPIOA->IOCFG |= GPIO_Pin_6;
		HT_GPIOA->AFCFG &= ~(GPIO_Pin_6);
		HT_GPIOA->PTDIR &= ~GPIO_Pin_6;
		HT_GPIOA->PTUP	|= GPIO_Pin_6;
		NVIC_DisableIRQ( EXTI1_IRQn );
		HT_INT->EXTIE |= 0x0202; //使能上升沿中断
		HT_INT->PINFLT |= 0x0002; //数字滤波
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI1_IRQn );
		#endif
	}
	else
	{
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE &= ~0x0040;		//关闭下降沿中断
		HT_INT->PINFLT &= ~0x0040;		//关闭数字滤波
		HT_INT->EXTIF = 0;
		
		#if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
		NVIC_DisableIRQ( EXTI5_IRQn );
		HT_INT->EXTIE &= ~0x2020; //禁止中断
		HT_INT->PINFLT &= ~0x0020; //数字滤波
		HT_INT->EXTIF = 0;
		#else
		NVIC_DisableIRQ( EXTI1_IRQn );
		HT_INT->EXTIE &= ~0x0202; //使能上升沿中断
		HT_INT->PINFLT &= ~0x0002; //数字滤波
		HT_INT->EXTIF = 0;
		#endif
	}
	#else
	if( Type == ePowerDownMode )
	{
		NVIC_DisableIRQ( EXTI5_IRQn );
		HT_INT->EXTIE = 0;
		HT_INT->EXTIE |= 0x0020;		//使能下降沿中断
		HT_INT->PINFLT |= 0x0020;		//使能数字滤波
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI5_IRQn );

		#if (BOARD_TYPE == BOARD_HT_SINGLE_78202202)
		HT_GPIOA->IOCFG |= GPIO_Pin_11;
		HT_GPIOA->AFCFG &= ~(GPIO_Pin_11);
		HT_GPIOA->PTDIR &= ~GPIO_Pin_11;
		HT_GPIOA->PTUP	|= GPIO_Pin_11;
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE |= 0x4040; //使能双边沿触发
		HT_INT->PINFLT |= 0x0040; //数字滤波
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI6_IRQn );
		#endif
	}
	else
	{
		NVIC_DisableIRQ( EXTI5_IRQn );
		HT_INT->EXTIE &= ~0x0020;		//关闭下降沿中断
		HT_INT->PINFLT &= ~0x0020;		//关闭数字滤波
		HT_INT->EXTIF = 0;

		#if (BOARD_TYPE == BOARD_HT_SINGLE_78202202)
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE &= 0x4040; //使能双边沿触发
		HT_INT->PINFLT &= 0x0040; //数字滤波
		HT_INT->EXTIF = 0;
		#endif
	}
	#endif//#if(BOARD_TYPE==BOARD_HT_SINGLE_78201602)
}

//-----------------------------------------------
//函数功能: 关闭单片机内部各个功能模块，及中断
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void HTMCU_DisableSelfModule(void)
{
	int i;  
  
    EnWr_WPREG();
    HT_UART0->UARTCON=0;
    HT_UART1->UARTCON=0;
    HT_UART2->UARTCON=0;
    HT_UART3->UARTCON=0;
    HT_UART4->UARTCON=0;
    HT_UART5->UARTCON=0;

    HT_SPI0->SPICON=0;
    HT_TBS->TBSCON=0x6541;
    // HT_TBS->TBSPRD=0x0004; //温度测量周期8秒
	HT_TBS->TBSPRD = 0x0007; //温度测量周期=64s
    
    HT_TMR0->TMRCON=0;
    HT_TMR1->TMRCON=0;
    HT_TMR2->TMRCON=0;
    
    //HT_LCD->LCDCON=0;
    HT_PMU->PMUCON=0;
    HT_INT->EXTIE=0;
    
    //HT_CMU->CLKCTRL1=0;//关闭UART0-5,TMR0-3,SPI1
    //HT_CMU->CLKCTRL0&=~0x000d;//关闭SPI0、IIC等
    
    DisWr_WPREG();

    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    NVIC_ClearPendingIRQ(EXTI3_IRQn);
    NVIC_ClearPendingIRQ(EXTI4_IRQn);
    NVIC_ClearPendingIRQ(EXTI5_IRQn);
    NVIC_ClearPendingIRQ(EXTI6_IRQn);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_ClearPendingIRQ(PMU_IRQn);

	//Systick为内核中断 不能通过屏蔽中断方式关闭 只能关闭此模块
	CLOSE_SYSTICK;
	
    for(i=0;i<31;i++)
    {
        NVIC_DisableIRQ( (IRQn_Type)i );//2016.8.1//全部关中断
    }
}

//-----------------------------------------------
//函数功能: 检查低功耗时钟
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void CheckLowPowerCLK( void )
{
	//如果低功耗下时钟不是Flf 强制切换一下时钟 防止时钟没有被切换回来
	if( (HT_CMU->SYSCLKCFG&0x0007) != 0x0001 )
	{
		HTMCU_GoToLowSpeed();
	}
}

//-----------------------------------------------
//函数功能: 低功耗时执行的开盖、电源异常等检测
//
//参数: 	Type[in]：eENTER_DOWN_MODE
//					eFromOnRunEnterDownMode
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void LowPowerCheck(BYTE Type)
{
    //EnableLowPowerPort();//开启低功耗唤醒下所需的必要端口，比如开盖检测
    
    #if( SEL_EVENT_METERCOVER == YES )
    ProcLowPowerMeterCoverRecord();//开上盖检测
    #endif
    
    #if( SEL_EVENT_BUTTONCOVER == YES )
    ProcLowPowerButtonCoverRecord();//开尾盖检测
    #endif
    
    #if( SEL_EVENT_POWER_ERR == YES )
    PowerErrCheck( Type );//电源异常检测 
    #endif
	
	//检查低功耗时钟
	CheckLowPowerCLK();

    //DisableLowPowerPort();//关闭刚才开启的端口
}


//-----------------------------------------------
//函数功能: 关闭所有外围器件的电源-不包括ESAM电源-ESAM电源单独控制
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void CloseAllPowerExceptESAM(void)
{
    CLOSE_BACKLIGHT;
    POWER_SAMPLE_CLOSE;
    POWER_LCD_CLOSE;
    POWER_FLASH_CLOSE;
    POWER_HALL_CLOSE;
    POWER_HONGWAI_REC_CLOSE;
    POWER_HONGWAI_TXD_CLOSE;
    POWER_CV485_CLOSE;

    PULSE_LED_OFF;
    WARN_ALARM_LED_OFF;
    RELAY_LED_OFF;
}

//-----------------------------------------------
//函数功能: 关闭所有外围器件的电源-不包括ESAM电源和FLASH电源
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void CloseAllPowerExceptESAM_FLASH(void)
{
    CLOSE_BACKLIGHT;
    POWER_SAMPLE_CLOSE;
    POWER_LCD_CLOSE;
	
    POWER_HALL_CLOSE;
    POWER_HONGWAI_REC_CLOSE;
    POWER_HONGWAI_TXD_CLOSE;
    POWER_CV485_CLOSE;

    PULSE_LED_OFF;
    WARN_ALARM_LED_OFF;
    RELAY_LED_OFF;
}


//-----------------------------------------------
//函数功能: 单片机切换到低频32.768KHz下运行
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void HTMCU_GoToLowSpeed(void)
{
    EnWr_WPREG();
    HT_CMU->SYSCLKCFG=0x0081;//选择Flf为系统时钟Fsys,此时Flf为Fosc，即：外部低频OSC晶振时钟(32.768KHz)
    HT_CMU->CLKCTRL1=0;
    HT_CMU->PREFETCH = 0x0000;//低功耗下将预取指令置为零 避免影响红外唤醒
    HT_CMU->CLKCTRL0=0x002c40;//这里有先后顺序，不能搞反
    HT_CMU->SYSCLKDIV=0x0;
    DisWr_WPREG();
}

static void LowPowerWakeup( void )
{
	if( IsExitLowPower == 10 )
	{
		DISABLE_CPU_INT;		// 关闭中断
		
		//!!!!!!内存初始化一定要放在最前面，因为调用了堆操作!!!!!!
		//api_InitAllocBuf();
		api_MCU_SysClockInit( ePowerDownMode );
		api_InitSysTickTimer( ePowerDownMode );
		InitPort( ePowerWakeUpMode );
		
		LowPowerKeyConfig( ePowerWakeUpMode );//低功耗唤醒后按键配置

		CloseAllPowerExceptESAM();//关闭所有外围器件的供电
		#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303))
		api_AdcConfig(ePowerWakeUpMode);
		#endif
        ESAMSPIPowerDown( ePowerDownMode );//关闭esam电源
		POWER_LCD_OPEN;
		
		api_FreshParaRamFromEeprom(0);
		api_FreshParaRamFromEeprom(1);
		api_PowerUpLcd( ePowerDownMode );
		api_CheckTimeTable(FALSE);
		api_LowPowerWakeUpPrePay();
		RESET_STACK_POINTER;
		
		//允许中断  在进主循环之前才打开中断
		ENABLE_CPU_INT;
		
		MainTask();
	}
}

//-----------------------------------------------
//函数功能: 单相表低功耗pmu处理
//
//参数: 	    无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void LowPowerPmuRtcInt(void)
{
	//PMU
	HT_GPIOE->IOCFG |= 0x0080;		//复用
	HT_GPIOE->AFCFG &=~0x0080;		//功能1
	EnWr_WPREG();
	NVIC_DisableIRQ(PMU_IRQn);		//禁止RTC中断
	HT_PMU->PMUCON	= 0x0007;
	HT_PMU->VDETCFG = 0x006D;		//上电比较VCC电压4.6V BOR 2.0伏复位
	HT_PMU->PMUIE	= 0x0004;		//使能PMU中断 只使能LVDIN
	HT_PMU->PMUIF	= 0;			//清中断标志		
	NVIC_ClearPendingIRQ(PMU_IRQn);	
	NVIC_EnableIRQ(PMU_IRQn);		//使能PMU中断
	DisWr_WPREG();		
}

//-----------------------------------------------
//函数功能: 上报载波通道的管脚配置
//
//参数: 	0x00:上报前
//		0x55:上报后
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------

void api_ReportCRWakeUpConfig(BYTE Type)
{
	#if( BOARD_TYPE == BOARD_HT_SINGLE_78202202 )
	if(Type == 0x00)
	{
		api_MCU_SysClockInit( ePowerOnMode );//三相表要求低功耗唤醒的情况下，红外要能通讯
		//PC12	PLCRX		功能端口 第一功能 输入 浮空
		//PC11	PLCTX		功能端口 第一功能 输出 开漏
		HT_GPIOC->IOCFG |= ((1<<11)|(1<<12));
		HT_GPIOC->AFCFG &= ~((1<<11)|(1<<12));
		
		HT_GPIOC->PTDIR &= ~(1<<12);
		HT_GPIOC->PTDIR |= (1<<11);
		
		HT_GPIOC->PTOD &= ~(1<<11);
		HT_GPIOC->PTUP |= (1<<12);
		ENABLE_CPU_INT;
		
		SerialMap[eCR].SCIInit(SerialMap[eCR].SCI_Ph_Num);//载波端口初始化
		POWER_REPORT_CR_OPEN;
	}
	else
	{
		DISABLE_CPU_INT;
		POWER_REPORT_CR_CLOSE;
		//PC12	PLCRX		配置为低功耗引脚
		//PC11	PLCTX		配置为低功耗引脚
		HT_GPIOC->IOCFG &= ~((1<<11)|(1<<12));
		HT_GPIOC->AFCFG &= ~((1<<11)|(1<<12));
		
		HT_GPIOC->PTDIR |= ((1<<11)|(1<<12));
		HT_GPIOC->PTSET |= ((1<<11)|(1<<12));
		HT_GPIOC->PTOD &= ~((1<<11)|(1<<12));
		HT_GPIOC->PTUP |= ((1<<11)|(1<<12));
		SerialMap[eCR].SCIDisableRcv(SerialMap[eCR].SCI_Ph_Num);//载波关闭接收中断
		NVIC_ClearPendingIRQ( (IRQn_Type)(9+SerialMap[eCR].SCI_Ph_Num) );//全部关中断
        NVIC_DisableIRQ( (IRQn_Type)(9+SerialMap[eCR].SCI_Ph_Num) );//全部关中断
		ENABLE_CPU_INT;
		HTMCU_GoToLowSpeed();//切换到低频32.768时钟
		ENABLE_CPU_INT;
	}
	#else
	if(Type == 0x00)
	{
		api_MCU_SysClockInit( ePowerOnMode );//三相表要求低功耗唤醒的情况下，红外要能通讯
		api_MCU_RTCIntSec(1);

		//PC2	PLCRX		功能端口 第一功能 输入 浮空
		//PC3	PLCTX		功能端口 第一功能 输出 开漏
		HT_GPIOC->IOCFG |= ((1<<2)|(1<<3));
		HT_GPIOC->AFCFG &= ~((1<<2)|(1<<3));
		
		HT_GPIOC->PTDIR &= ~(1<<2);
		HT_GPIOC->PTDIR |= (1<<3);
		
		HT_GPIOC->PTOD &= ~(1<<3);
		HT_GPIOC->PTUP |= (1<<2);
		ENABLE_CPU_INT;
		
		SerialMap[eCR].SCIInit(SerialMap[eCR].SCI_Ph_Num);//载波端口初始化
		POWER_REPORT_CR_OPEN;
	}
	else
	{
		DISABLE_CPU_INT;
		POWER_REPORT_CR_CLOSE;
		//PC2	PLCRX		配置为低功耗引脚
		//PC3	PLCTX		配置为低功耗引脚
		HT_GPIOC->IOCFG &= ~((1<<2)|(1<<3));
		HT_GPIOC->AFCFG &= ~((1<<2)|(1<<3));
		
		HT_GPIOC->PTDIR |= ((1<<2)|(1<<3));
		HT_GPIOC->PTSET |= ((1<<2)|(1<<3));
		HT_GPIOC->PTOD &= ~((1<<2)|(1<<3));
		HT_GPIOC->PTUP |= ((1<<2)|(1<<3));
		SerialMap[eCR].SCIDisableRcv(SerialMap[eCR].SCI_Ph_Num);//载波关闭接收中断
		NVIC_ClearPendingIRQ( (IRQn_Type)(9+SerialMap[eCR].SCI_Ph_Num) );//全部关中断
        NVIC_DisableIRQ( (IRQn_Type)(9+SerialMap[eCR].SCI_Ph_Num) );//全部关中断
		ENABLE_CPU_INT;
		HTMCU_GoToLowSpeed();//切换到低频32.768时钟
		ENABLE_CPU_INT;
	}
	#endif//#if( BOARD_TYPE == BOARD_HT_SINGLE_78202202 )
}

//-----------------------------------------------
//函数功能: 进入低功耗及相应模块处理
//
//参数: 	Type[in]
//					eFromOnInitEnterDownMode	从上电初始化进入低功耗
//					eFromOnRunEnterDownMode		上电运行后进入低功耗
//					eFromWakeupEnterDownMode	低功耗唤醒后再进入低功耗
//					eFromDetectEnterDownMode	低功耗进行电源异常或全失压检测后再进入低功耗
//返回值:  	无
//
//备注:   
//-----------------------------------------------
extern void SwapAHour(void);
void api_EnterLowPower(eENTER_DOWN_MODE Type)
{
    BYTE	PowerDownCompleteFlag = 0;
	static DWORD adc_count=0;

    DISABLE_CPU_INT;
    CLEAR_WATCH_DOG;
	
	//避免按键唤醒后会清掉
	if( Type == eFromOnRunEnterDownMode )
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
    api_PowerDownLcd();//清屏
	
	#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202303) || (BOARD_TYPE == BOARD_HT_SINGLE_78202401))
	CloseAllPowerExceptESAM_FLASH();//关闭所有外围器件的供电除flash/ESAM
	#else
	CloseAllPowerExceptESAM();//关闭所有外围器件的供电
	#endif
    ESAMSPIPowerDown( ePowerDownMode );//关闭esam电源

    //只有在正常掉电时才转存，按键或红外唤醒后进入低功耗是不进行转存的。
 	if( Type == eFromOnRunEnterDownMode )
 	{      
        if( g_PowerOnSecTimer == 0 )
        {
        	api_PowerDownEnergy();     // 掉电电能转存处理
        	api_PowerDownPrePay();
        }

        api_PowerDownFreeze();
    	api_PowerDownEvent();       // 掉电事件处理

		 #if( SEL_AHOUR_FUNC == YES )
		SwapAHour();
		#endif
 	}
 	//是否为上电后进入低功耗
 	if((Type == eFromOnRunEnterDownMode) || (Type == eFromOnInitEnterDownMode))
 	{
		// api_LowPowerCheckMeterCoverStatus();
		api_WriteFreeEvent(EVENT_ENTER_LOW_POWER, Type);//记录自由事件
		#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303))
		TimerLcdTmp = 5;
		#endif
 	}
 	api_PowerDownSample();
 	api_InitNoInitFlag();

 	CLEAR_WATCH_DOG;//清看门狗
 	
    //从正常上电到掉电完成动作，置此标志
	if( Type == eFromOnRunEnterDownMode )
	{
		PowerDownCompleteFlag = 0xaa;
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}

 	//关闭没有用到的模块(WDT,TimerB,A/D,物理串口SCI,SPI，DCO,Comparator_A,lcd,所有I/O口)
 	InitPort( ePowerDownMode );//各引脚进行低功耗处理
 	LOWPOWER_INVALID_RELAY_CTRL;
 	
    HTMCU_DisableSelfModule();//关闭mcu各个模块
    
    CloseAllPowerExceptESAM();//关闭所有外围器件的供电
    ESAMSPIPowerDown( ePowerDownMode );//关闭esam电源
 	CLEAR_WATCH_DOG;
 	IsExitLowPower = 0;
	
	//!!!!!!屏蔽，lsc
	// #if (BOARD_TYPE == BOARD_HT_SINGLE_78202202)
	// api_MCU_RTCInt();
	// #else
	// LowPowerPmuRtcInt();
	// api_MCU_RTCIntSec( 2 );
	// #endif
	// LowPowerKeyConfig( ePowerDownMode );//低功耗下按键配置
	// HTMCU_GoToLowSpeed();//切换到低频32.768时钟
	
    ENABLE_CPU_INT;
    
	FunctionConst(LOWPOWER_TASK);

//    while(1)//低功耗循环
//	{
//		
//		Hold_Sleep();//进入hold模式
//		//这里等待秒中断唤醒后，往下执行
//		CLEAR_WATCH_DOG;//清看门狗
//		ProcLowPowerMeterReport();//检查是否上报
//		if( api_CheckSysVol( eSleepDetectPowerMode ) == TRUE )
//		{
//			Reset_CPU();
//		}
//
//		LowPowerWakeup();
//
//		LowPowerCheck(Type);
//	}
	Reset_CPU();
}


//-----------------------------------------------
//函数功能: 获取掉电是否完成标志
//
//参数: 	无

//返回值:  	正常完成掉电返回TRUE,未完成掉电返回FALSE
//
//备注:   
//-----------------------------------------------
BOOL api_GetPowerDownCompleteFlag( void )
{
	BYTE	PowerDownCompleteFlag;
	
    api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	
	if( PowerDownCompleteFlag == 0xaa )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


#endif//#if( (CPU_TYPE == CPU_HT6025) && (MAX_PHASE == 1) ) //暂时用cpu定义，可以考虑用单三相定义

