//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.26
//描述		矩泉cpu三相表低功耗接口文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "LowPower_MeasuringSwitch.h"
#include "PowerErr.h"
#include "LostAllV.h"

#if( (CPU_TYPE == CPU_HC32F4A0) && (MAX_PHASE == 3) ) //暂时用cpu定义，可以考虑用单三相定义
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
	
	if( IsExitLowPower == 10 )
	{
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

}
#endif//#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
#endif//#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )




//-----------------------------------------------
//函数功能: 将所有端口配置成低功耗模式（除了JTAG端口外），所有端口功能失效（包括spi、eeprom端口）
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void InitLowPowerPort( void )
{

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
    ProcLowPowerMeterCoverRecord( 0x00 );
    #endif
    
    //开尾盖检测
    #if( SEL_EVENT_BUTTONCOVER == YES )
    ProcLowPowerButtonCoverRecord( 0x00 );
    #endif
    
    //电源异常检测
    #if( SEL_EVENT_POWER_ERR == YES )
    PowerErrCheck( Type );
    #endif
    
    //全失压检测
    #if( SEL_EVENT_LOST_ALL_V == YES )
	AllLostVCheck( Type );
	#endif
    
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
    POWER_EEPROM_CLOSE;
	POWER_FLASH_CLOSE;
	POWER_HALL_CLOSE;
    POWER_HONGWAI_REC_CLOSE;
    POWER_HONGWAI_TXD_CLOSE;
    POWER_CV485_CLOSE;
    

    PULSE_LED_OFF;
    WARN_ALARM_LED_OFF;
    RELAY_LED_OFF;
    BEEP_OFF;

	LIGHT_RUN_STOP;
	LIGHT_A_STOP;
    LIGHT_B_STOP;
    LIGHT_C_STOP;
    LIGHT_N_STOP;
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
static void HTMCU_GoToLowSpeed(void)
{

}

static void LowPowerWakeup( void )
{
	if( IsExitLowPower == 10 )
	{
		DISABLE_CPU_INT;		// 关闭中断
		
		//!!!!!!内存初始化一定要放在最前面，因为调用了堆操作!!!!!!
		api_InitAllocBuf();
		api_MCU_SysClockInit( ePowerOnMode );//三相表要求低功耗唤醒的情况下，红外要能通讯
		InitPort( ePowerDownMode );
		
		memset( Serial[eIR].ProBuf, 0x00, MAX_PRO_BUF );
		
		CloseAllPowerExceptESAM();
		POWER_EEPROM_OPEN;
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
		api_SetSysStatus( eSYS_LOW_POWER_WAKEUP );
		RESET_STACK_POINTER;
		//允许中断  在进主循环之前才打开中断
		ENABLE_CPU_INT;
		
		MainTask();
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
void api_EnterLowPower(eENTER_DOWN_MODE Type)
{
	BYTE	PowerDownCompleteFlag = 0;
	
	DISABLE_CPU_INT;
    CLEAR_WATCH_DOG;
	
	//避免按键唤醒后会清掉
	if( Type == eFromOnRunEnterDownMode )
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
    api_PowerDownLcd();//清屏
    
    CloseAllPowerExceptESAM();//关闭所有外围器件的供电
    ESAMSPIPowerDown( ePowerDownMode );
    POWER_EEPROM_OPEN;//一定要打开eeprom电源，因为需要向eeprom写数据

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
        
        //记录事件
	 	api_WriteFreeEvent(EVENT_ENTER_LOW_POWER, 0);    
 	}
 	
 	api_PowerDownSample();
 	
 	api_PowerDownFlag();

 	CLEAR_WATCH_DOG;//清看门狗
 	
    //从正常上电到掉电完成动作，置此标志
	if( Type == eFromOnRunEnterDownMode )
	{
		PowerDownCompleteFlag = 0xaa;
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
 	//关闭没有用到的模块(WDT,TimerB,A/D,物理串口SCI,SPI，DCO,Comparator_A,lcd,所有I/O口)
 	InitLowPowerPort();//各引脚进行低功耗处理
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
	
	Reset_CPU();// 直接复位，没有低功耗
	
	//#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	//重新配置红外
	//PowerDownOpenIRReceive(0);	
	//#endif
    
 	//CLEAR_WATCH_DOG;
	//HTMCU_GoToLowSpeed();//切换到低频32.768时钟
    
	//#if( SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH )
    //ClearIRWakeUpData();
    //#endif
    
    //HT_GPIOD->PTDIR &= ~GPIO_Pin_13;//上翻按键
    //HT_GPIOD->PTDIR &= ~GPIO_Pin_14;//下翻按键
    //HT_GPIOA->PTDIR &= ~GPIO_Pin_4; //上盖检测
    //HT_GPIOD->PTDIR &= ~GPIO_Pin_15;//尾盖检测
    //api_MCU_RTCInt( );//开启RTC秒中断，中断后会唤醒单片机的hold状态
	//ENABLE_CPU_INT;
	
	//FunctionConst(LOWPOWER_TASK);

	//while(1)//低功耗循环
	//{
    //    Hold_Sleep();//进入hold模式

    //    //这里等待秒中断唤醒后，往下执行
    //    CLEAR_WATCH_DOG;//清看门狗
        
    //    if( api_CheckSysVol( eSleepDetectPowerMode ) == TRUE )
    //    {
    //    	Reset_CPU();
    //    }
        
    //    LowPowerWakeup();
        
    //    LowPowerCheck( Type );
		
	//	#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	//	ProcLowPowerIrComm();
	//	#endif
	//}
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




#endif//#if( (CPU_TYPE == CPU_HC32F4A0) && (MAX_PHASE == 3) ) //暂时用cpu定义，可以考虑用单三相定义

