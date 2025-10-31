//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.26
//描述		矩泉cpu三相表低功耗接口文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT_LowPower_Three.h"
#include "PowerErr.h"
#include "LostAllV.h"

#if( (CPU_TYPE == CPU_HT6025) && (MAX_PHASE == 3) ) //暂时用cpu定义，可以考虑用单三相定义
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
__no_init BYTE g_byDownPowerTime;

#if( SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH )
WORD WakeUpCountOld;
WORD WakeUpCountNew;
BYTE DeltaTestNo;
WORD DeltaTest[MAX_PORT_INT_NUM];
#endif
WORD ReadVBATLowFlag;
//
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )

#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
//-----------------------------------------------
//函数功能: 进入低功耗重新配置红外接收功能
//
//参数: 	Type[in] 0--由大循环进入低功耗	1--低功耗下循环调用
//返回值:  	无
//
//备注:光敏管在有倾角的时候接收不到信号，改为不用光敏管的方式
//-----------------------------------------------
static void PowerDownOpenIRReceive( BYTE Type )
{
	if( LowPowerSecCount < IR_WAKEUP_LIMIT_TIME )
	{
		//功能端口
		HT_GPIOC->IOCFG |= (GPIO_Pin_0|GPIO_Pin_1);

		//复用功能2
		HT_GPIOC->AFCFG = 0;
		//端口方向
		HT_GPIOC->PTDIR &= 0xfffc;

		//初始为高
		HT_GPIOC->PTSET = 0x0003;

		//打开接收电源
		POWER_HONGWAI_REC_OPEN;
		if( Type == 0 )
		{
			SerialMap[eIR].SCIInit( SerialMap[eIR].SCI_Ph_Num ); //红外端口初始化
		}
		//按照32768时钟源配置1200红外通信波特率
		HT_UART1->SREL    = 0x0d;
	}
}

//-----------------------------------------------
//函数功能: 进入低功耗重新配置红外接收功能
//
//参数: 	无
//返回值:  	无
//
//备注:光敏管在有倾角的时候接收不到信号，改为不用光敏管的方式
//-----------------------------------------------
static void PowerDownCloseIRReceive( void )
{
	//IO端口
	HT_GPIOC->IOCFG &= 0xfffc;

	//复用功能2
	HT_GPIOC->AFCFG = 0;
	//端口方向
	HT_GPIOC->PTDIR |= 0x0003;

	//初始为高
	HT_GPIOC->PTSET = 0x0003;

	//关闭接收电源
	POWER_HONGWAI_REC_CLOSE;
}

//-----------------------------------------------
//函数功能: 底功耗下操作红外
//
//参数: 	无
//返回值:  	无
//
//备注:前7天每3秒打开100ms红外接收，打开时功耗为1mA左右，平均增加功耗
//-----------------------------------------------
static void ProcLowPowerIrComm(void)
{
	if( LowPowerSecCount < IR_WAKEUP_LIMIT_TIME )
	{
		if( WakeUpTimer == 0 )
		{
			if( (LowPowerSecCount % 3) == 0 )
			{
				PowerDownOpenIRReceive( 1 );
				api_Delay100us( 2 );
				PowerDownCloseIRReceive();
			}
		}
	}
}
#endif

void ProcContinueFramWakeup( void )
{
	BYTE FramWakeupNo;
	WORD tRXWPoint;
	
	if( IsExitLowPower == 10 )
	{
		return;
	}
	
	//刷新下当前时间
	if( LowPowerSecCount > IR_WAKEUP_LIMIT_TIME )
	{
		//掉电已经超过7天，不允许唤醒
		#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
		DeltaTestNo = 0;
		#elif(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
		PowerDownCloseIRReceive();
		#endif
		return;
	}
	
	#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	tRXWPoint = Serial[eIR].RXWPoint;
	//查找最近接收的15个字节是否有符合要求的数据，没有红外发送的时候打开接收能收到乱码，指针变化，所以要往前找
	for( FramWakeupNo = 0; FramWakeupNo < 15; FramWakeupNo++ )
	{
		//前3个字节可以不管
		if( (tRXWPoint - 3) > MAX_PRO_BUF )
		{
			return;
		}
		if( Serial[eIR].ProBuf[tRXWPoint - 1] == 0x04 )
		{
			if( Serial[eIR].ProBuf[tRXWPoint - 2] == 0x11 )
			{
				if( Serial[eIR].ProBuf[tRXWPoint - 3] == 0x68 )
				{
					IsExitLowPower = 10;
					break; 
				}
			}			
		}
		tRXWPoint--;
	}
	#elif(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
	if( DeltaTestNo < 15 )
	{
		DeltaTestNo = 0;	
		return;
	}
	
	for(FramWakeupNo=15; FramWakeupNo<(MAX_PORT_INT_NUM-8); FramWakeupNo++)
	{
		//68 11 04 特征码：68 45 50 68 54 82，允许5个的误差，由于不同掌机在发送字节间延时不一致，所以查询68 68 82这三个特征码
		//5438当时的定时器是16384，实际对应5位 3位 4位 5位 4位 6位 1200BPS
		//if( (DeltaTest[FramWakeupNo]<141) && (DeltaTest[FramWakeupNo]>131) ) 由于6025的滤波作用，第一个特征码68会受到字节间延时干扰 故不进行判断-姜文浩
		{
			//for(FramWakeupNoj = FramWakeupNo+1; FramWakeupNoj<FramWakeupNo+5; FramWakeupNoj++)
			{
				if((DeltaTest[FramWakeupNo]<141) && (DeltaTest[FramWakeupNo]>131))
				{
					//for(FramWakeupNok = FramWakeupNoj+1; FramWakeupNok<FramWakeupNoj+3; FramWakeupNok++)
					{
						if((DeltaTest[FramWakeupNo+2]<169) && (DeltaTest[FramWakeupNo+2]>159))
						{							
							//置红外唤醒标志
							//RamDBase.FlagByte.ExtSysStatus1 |= 0x2000;
							IsExitLowPower = 10;
							DeltaTestNo = 0;
							return;
						}
					}
				}
			}
		}
	}
	
	DeltaTestNo = 0;
	 
	#endif	
}

#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
void ClearIRWakeUpData()
{
	// 清除红外唤醒计数器
	WakeUpCountOld = 0;
	WakeUpCountNew = 0;
	DeltaTestNo = 0;
	memset((BYTE *)&DeltaTest,0,sizeof(DeltaTest));
	
	EnWr_WPREG();
	
	HT_INT->EXTIE |= 0x0100;//上升沿触发
	HT_INT->EXTIF = 0;
	HT_INT->PINFLT |= 0x0001;
	NVIC_EnableIRQ( EXTI0_IRQn );
	
	//功能端口
  	HT_GPIOA->IOCFG |= GPIO_Pin_5;
	//复用功能2
    HT_GPIOA->AFCFG |= 0;
	//端口方向
    HT_GPIOA->PTDIR &= ~GPIO_Pin_5;
	//推挽输出
    HT_GPIOA->PTOD |= 0;
	//初始为高
    HT_GPIOA->PTSET |= 0;
    //初始为低
    HT_GPIOA->PTCLR |= 0;
 	HT_GPIOA->PTUP |= 0;
	
	HT_CMU->CLKCTRL1 |= CMU_CLKCTRL1_TMR4EN;
	HT_TMR4->TMRDIV = 0;
	HT_TMR4->TMRPRD = 0xFFFF;
	HT_TMR4->TMRIE = 0;
	HT_TMR4->TMRCON = 0x0107;//选择flf时钟（32768），周期定时模式
	
	DisWr_WPREG();
}
#endif//#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
#endif//#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )

//-----------------------------------------------
//函数功能: 检查低功耗下开盖检测时间
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void CheckLowPowerCoverCheckTime( void )
{
	if( SelThreeBoard == BOARD_HT_THREE_0132515 )
	{
		if( LowPowerSecCount > COVER_CHECK_LIMIT_TIME ) //开盖检测到达60小时后关闭电源
		{
			if(POWER_COVER_INT)
			{
				POWER_COVER_CLOSE;
			}
		}
	}
}

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
	if( Type == ePowerDownMode )
	{
		NVIC_DisableIRQ( EXTI5_IRQn );
		NVIC_DisableIRQ( EXTI6_IRQn );
		#if(SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_PHOTO_SWITCH)
		HT_INT->EXTIE = 0;
		#endif
		HT_INT->EXTIE |= 0x6000;			//使能上升沿中断
		HT_INT->PINFLT |= 0x0060;			//使能数字滤波
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI5_IRQn );
		NVIC_EnableIRQ( EXTI6_IRQn );
	}
	else
	{
		NVIC_DisableIRQ( EXTI5_IRQn );
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE &= ~0x6000;			//关闭上升沿中断
		HT_INT->PINFLT &= ~0x0060;			//关闭数字滤波
		HT_INT->EXTIF = 0;
	}
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
	HT_SPI3->SPICON=0;
    HT_TBS->TBSCON=0x6541;
    HT_TBS->TBSPRD	= 0x0004;
    
    HT_TMR0->TMRCON=0;
    HT_TMR1->TMRCON=0;
    HT_TMR2->TMRCON=0;
    
    //HT_LCD->LCDCON=0;
    HT_PMU->PMUCON=0;
    HT_INT->EXTIE=0;
    //HT_CMU->CLKCTRL1=0;//关闭UART0-5,TMR0-3,SPI1
    //HT_CMU->CLKCTRL0&=~0x000f;//关闭SPI0、IIC、LCD等
    
    DisWr_WPREG();

	//Systick为内核中断 不能通过屏蔽中断方式关闭 只能关闭此模块
	CLOSE_SYSTICK;
	
    for(i=0;i<31;i++)
    {
    	NVIC_ClearPendingIRQ( (IRQn_Type)i );//2016.8.1//全部关中断
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
	//检测电源异常与检测全失压时会切时钟到5.5M
	if(( IsExitLowPower != 30 ) && ( IsExitLowPower != 20))
	{
		//如果低功耗下时钟不是Flf 强制切换一下时钟 防止时钟没有被切换回来
		if( api_CheckMCUCLKIsFlf() == FALSE )
		{
			HTMCU_GoToLowSpeed();
		}
	}
}


//-----------------------------------------------
//函数功能: 低功耗时执行的开盖、电源异常等检测
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void LowPowerCheck( BYTE Type )
{
    // EnableLowPowerPort();//开启低功耗唤醒下所需的必要端口，比如开盖检测
    
    //开上盖检测
    #if( SEL_EVENT_METERCOVER == YES )
    ProcLowPowerMeterCoverRecord();
    #endif
    
    //开尾盖检测
    #if( SEL_EVENT_BUTTONCOVER == YES )
    ProcLowPowerButtonCoverRecord();
    #endif
    
    //电源异常检测
    #if( SEL_EVENT_POWER_ERR == YES )
	if(ReadVBATLowFlag != 0xAAAA)//停电抄表电池电压低不开计量芯片
	{
		PowerErrCheck( Type );
	}
    #endif
    
    //全失压检测
    #if( SEL_EVENT_LOST_ALL_V == YES )
	if(ReadVBATLowFlag != 0xAAAA)//停电抄表电池电压低不开计量芯片
	{
	 	AllLostVCheck( Type );
	}
	#endif
	
	CheckLowPowerCoverCheckTime();
    //检查低功耗时钟
	CheckLowPowerCLK();
    //DisableLowPowerPort();//关闭刚才开启的端口
}


//-----------------------------------------------
//函数功能: 关闭所有外围器件的电源
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
    #if( PREPAY_MODE == PREPAY_LOCAL )
    BEEP_OFF;
    #endif
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
	#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	HT_CMU->CLKCTRL1 |= 0x000020; 
	#endif
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
		api_MCU_SysClockInit( ePowerOnMode );//三相表要求低功耗唤醒的情况下，红外要能通讯
		api_InitSysTickTimer( ePowerOnMode );
		InitPort( ePowerWakeUpMode );
		CheckLowPowerCoverCheckTime();//检查时钟电池供开盖检测电源时间
		LowPowerKeyConfig( ePowerWakeUpMode );//低功耗唤醒后按键配置
		
		memset( Serial[eIR].ProBuf, 0x00, MAX_PRO_BUF );
		
		CloseAllPowerExceptESAM();
		POWER_FLASH_OPEN;
		POWER_SAMPLE_OPEN;
		POWER_HONGWAI_REC_OPEN;
		POWER_HONGWAI_TXD_OPEN;
		POWER_LCD_OPEN;

		ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 );
		SerialMap[eIR].SCIInit(SerialMap[eIR].SCI_Ph_Num);//红外端口初始化
		api_FreshParaRamFromEeprom(0);
		api_FreshParaRamFromEeprom(1);
		api_PowerUpLcd( ePowerDownMode );
		api_CheckTimeTable(FALSE);
		api_LowPowerWakeUpPrePay();
		api_SetSysStatus( eSYS_LOW_POWER_WAKEUP );
		RESET_STACK_POINTER;
		//允许中断  在进主循环之前才打开中断
		ENABLE_CPU_INT;
		
		MainTask();
	}
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
	if(Type == 0x00)
	{
		api_MCU_SysClockInit( ePowerOnMode );//三相表要求低功耗唤醒的情况下，红外要能通讯
		//PC2	PLCRX		功能端口 第一功能 输入 浮空
		//PC3	PLCTX		功能端口 第一功能 输出 推挽
		HT_GPIOC->IOCFG |= ((1<<2)|(1<<3));
		HT_GPIOC->AFCFG &= ~((1<<2)|(1<<3));
		
		HT_GPIOC->PTDIR &= ~(1<<2);
		HT_GPIOC->PTDIR |= (1<<3);
		
		HT_GPIOC->PTOD |= (1<<3);
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
	
	DISABLE_CPU_INT;
    CLEAR_WATCH_DOG;
	POWER_COVER_OPEN;
	//避免按键唤醒后会清掉
	if( Type == eFromOnRunEnterDownMode )
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
    api_PowerDownLcd();//清屏
    if((Type == eFromOnRunEnterDownMode) || (Type == eFromOnInitEnterDownMode))
 	{
    	ReadVBATLowFlag = 0x5555;
    }
    CloseAllPowerExceptESAM();//关闭所有外围器件的供电
    ESAMSPIPowerDown( ePowerDownMode );
    POWER_FLASH_OPEN;//一定要打开eeprom电源，因为需要向eeprom写数据

    //只有在正常掉电时才转存，按键或红外唤醒后进入低功耗是不进行转存的。
 	if( Type == eFromOnRunEnterDownMode )
 	{    
        if( g_PowerOnSecTimer == 0 )
        {
        	api_PowerDownEnergy();         // 掉电电能转存处理
        	api_PowerDownPrePay();
        }
        
        api_PowerDownFreeze();
        api_PowerDownEvent();           // 掉电事件处理

		#if( SEL_AHOUR_FUNC == YES )
		SwapAHour();
		#endif 
		if( api_GetRunHardFlag( eRUN_HARD_READ_BAT_LOW ) == TRUE )	//BIT3	停电抄表电池( 0 正常, 1欠压)		
		{
			if( SelThreeBoard == BOARD_HT_THREE_0132515 )
			{
				ReadVBATLowFlag = 0xAAAA;
			}
		}
 	}
 	//是否为上电后进入低功耗
 	if((Type == eFromOnRunEnterDownMode) || (Type == eFromOnInitEnterDownMode))
 	{
 		api_LowPowerCheckMeterCoverStatus();
 		api_LowPowerCheckButtonCoverStatus();
 		api_WriteFreeEvent(EVENT_ENTER_LOW_POWER, Type);//记录自由事件
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
    ESAMSPIPowerDown( ePowerDownMode );

	IsExitLowPower = 0;
	api_ClrSysStatus( eSYS_LOW_POWER_WAKEUP );
	
	if( Type == eFromOnRunEnterDownMode )
	{
		#if( SEL_EVENT_POWER_ERR == YES )
		wPowerErrFlag = 0;
		#endif
		
		g_byDownPowerTime = 0;
	}
	
	#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	//重新配置红外
	PowerDownOpenIRReceive(0);	
	#endif
    
 	CLEAR_WATCH_DOG;
	HTMCU_GoToLowSpeed();//切换到低频32.768时钟
    
	#if( SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH )
    ClearIRWakeUpData();
    #endif
    
    LowPowerKeyConfig( ePowerDownMode );//低功耗下按键配置
    api_MCU_RTCInt( );//开启RTC秒中断，中断后会唤醒单片机的hold状态
	ENABLE_CPU_INT;
	
	FunctionConst(LOWPOWER_TASK);

	while(1)//低功耗循环
	{
		
		Hold_Sleep();//进入hold模式
		//这里等待秒中断唤醒后，往下执行
		CLEAR_WATCH_DOG;//清看门狗
		ProcLowPowerMeterReport();
		if( api_CheckSysVol( eSleepDetectPowerMode ) == TRUE )
		{
			Reset_CPU();
		}

		LowPowerWakeup();

		LowPowerCheck( Type );
		
		#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
		ProcLowPowerIrComm();
		#endif
	}
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




#endif//#if( (CPU_TYPE == CPU_HT6025) && (MAX_PHASE == 3) ) //暂时用cpu定义，可以考虑用单三相定义

