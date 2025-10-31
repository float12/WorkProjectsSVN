//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.30
//描述		CPU驱动程序 //这个文件要只留公共的部分，尽量不要用宏判断
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

#if ( CPU_TYPE == CPU_HT6025 )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define C_SysTickLoad   (((22020096>>SPD_MCU) / (1000/SYS_TICK_PERIOD_MS)) - 1)

//6015的HRC是14M(6025的是11.01M) 所以低功耗唤醒的系统时钟为7M 不是5.5M SysTick的速度会加快
#define C_SingleLowPowerSysTickLoad   (((22020096>>SINGLE_LOW_POWER_SPD_MCU) / (1000/SYS_TICK_PERIOD_MS)) - 1)
#define HT_HFCFG (*((volatile unsigned int*)(0x4000C0F0)))

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//Flash读保护是否开启
// #if( SEL_FLASH_PROTECT == FLASH_PROTECT_OPEN )
// __root const DWORD FlashProtect@".password"=0xF7FF00A2;
// #else
// __root const DWORD FlashProtect@".password"=0xF7FFFFA2;mIN
// #warning: "警告:调试时可关闭读保护,供货时要开启读保护";
// #endif
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
DWORD	ReceiveProgStatus;
WORD 	ProgFlashPW;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

static void InitWatchDog(void);

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 毫秒延时函数
//
//参数: 	ms[in]		需要延时的毫秒数
//                    
//返回值:  	无
//
//备注:    K版:
//			22M主频实测延时1.007ms 11M主频实测延时1.018ms
//-----------------------------------------------
void api_Delayms(WORD ms)
{
	WORD i, j;
	for(i=0; i<ms; i++)
	{
		#if(SPD_MCU == SPD_22000K)
		for(j=0; j<(3160/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#elif(SPD_MCU == SPD_11000K) 
		for(j=0; j<(3190/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#endif	//#if(SPD_MCU == SPD_22000K)
	}
}


//-----------------------------------------------
//函数功能: 100微秒延时函数 - 中等优化
//
//参数: 	us[in]		需要延时的100微秒数
//                    
//返回值:  	无
//
//备注:    K版:
//			22M主频时 实测约延时104.25us-1次 102.45-10次   循环值大于256和小于256延时值相差很大，从汇编看也不一样
//			11M主频时 实测约延时102.65us-1次 100.45-10次
//-----------------------------------------------
void api_Delay100us(WORD us)
{
	WORD i, j;
	
	for(i=0; i<us; i++)
	{
		#if(SPD_MCU == SPD_22000K)
		for(j=0; j<(320/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#elif(SPD_MCU == SPD_11000K)
		for(j=0; j<(312/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#endif	//#if(SPD_MCU == SPD_22000K)
	}
}


//-----------------------------------------------
//函数功能: 10微秒延时函数 - 中等优化
//
//参数: 	us[in]		需要延时的10微秒数
//                    
//返回值:  	无
//
//备注:     K版:
//			22M主频下实测约12.3us 
//			和延时时间有关 5-10.99 10-10.84 50-10.7 100-10.69 2-11.5 4-11.08 6-10.94
//			11M主频下实测约12.4us
//			和延时时间有关 5-10.4 10-10.15 50-9.95 100-9.93 2-11.15 4-10.5 6-10.3
//-----------------------------------------------
void api_Delay10us(WORD us)
{
	WORD i, j;
	
	for(i=0; i<us; i++)
	{
		#if(SPD_MCU == SPD_22000K)
		for(j=0; j<(32/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#elif(SPD_MCU == SPD_11000K)
		for(j=0; j<(28/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#endif	//#if(SPD_MCU == SPD_22000K)
	}
}


//-----------------------------------------------
//函数功能: 通过NOP进行延时函数
//
//参数: 	Step[in]		需要延时的NOP个数
//                    
//返回值:  	无
//
//备注:   !!!!!!该函数另有一个函数指针调用方式，至少要保留一个地方采用函数名调用方式 !!!!!!
//-----------------------------------------------
#pragma optimize=size low//中级优化的情况下，会把a_byDefaultData优化掉
void api_DelayNop(BYTE Step)
{
	BYTE i;
	
	i = a_byDefaultData[0];
	
	for(i=0; i<Step; i++)
	{
   		__NOP();
   	}
}

//---------------------------------------------------------------
//函数功能: 加载HRC补偿系数--用于上电延时
//
//参数: 无
//                   
//返回值:  无
//
//备注:   
//---------------------------------------------------------------
static void LoadHRCInfo( void )
{
	BYTE adj, u80, u81;
	DWORD u320,u321; 
	
	EnWr_WPREG();
	//HRC需要从infoflash取出补偿值 可保证在11m左右 误差不超过1.2% 否则输出不稳定
	u320 = *(DWORD *)0x40140;
	u321 = *(DWORD *)0x40144;
	u321 = ~u321;
	u80  = *(BYTE *)0x40140;
	u81  = *(BYTE *)0x40141;
	u81  = ~u81;

	if(u320 == u321)       //11M
	{
		adj = u320;
	}
	else if(u80 == u81)    //14M 最初的6015的调校值为14M
	{
		adj = u80;
	}
	else                    //8M 如果数据错误 8M附近最准确 因此固定为8M
	{
		adj = 0x3D;
	}
	
	HT_CMU->CLKCTRL0 |= 0x000020;//使能HRC
	HT_CMU->PREFETCH = 0x0000;	 //将预取指令置为零 
	HT_CMU->HRCADJ = adj;        //输出11MHz

	DisWr_WPREG();	
}
//---------------------------------------------------------------
//函数功能: 等OSC起振延时
//
//参数: Val
//                   
//返回值: 无 
//
//备注: 
//	冷复位后，运行HRC11M，经过层层分频后，主频在 11M/4 左右。默认寄存器配置
//仿真情况下入参：
//  10   10.010ms、	20	20.01ms   50  50.000ms  100  100.011ms
//实际情况下入参：
//  10   10.009ms、	20	20.008ms   50  50.008ms  100  100.008ms
//---------------------------------------------------------------
void WaitOSCRunDelayms( WORD Val )
{
	WORD i, j;
	
	for(i=0; i<Val; i++)
	{
		for(j=0; j<395; j++)
		{
			asm("nop");
		}
	}	
}

//---------------------------------------------------------------
//函数功能: 冷复位等OSC起振
//
//参数: 无
//
//返回值:  无
//
//备注:
//---------------------------------------------------------------
static void ResetWaitOSCRun( void )
{
	WORD i;
	//冷复位 上电延时。POR(上电复位)或者LBOR(低电压检测复位)复位
	if((HT_PMU->RSTSTA&0x0003)||
	   (NoInitDataFlag.CRC32 != lib_CheckCRC32( (BYTE *)&NoInitDataFlag, sizeof(NoInitDataFlag) - sizeof(DWORD) )))
	{
		LoadHRCInfo();//补偿HRC
		//上电默认时钟为HRC11M，HRC_DIV 2分频，SYSCLK_DIV 2分频、FCLK = 2.5M
		//仿真下1.96秒	非仿真1.97秒
		for(i = 0; i < WaitOSCRunTime; i++)
		{
			CLEAR_WATCH_DOG;
			//eSleepDetectPowerMode 函数内部没有延时处理，防止误判，第一次不进低功耗
			//循环30次再进低功耗，解决源上电速度慢电表误判直接进入低功耗引发看门狗复位，导致上电速度慢的问题
			if((api_CheckSysVol( eSleepDetectPowerMode ) == FALSE) && ( i >= 40 ))
			{
				api_EnterLowPower( eFromOnInitEnterDownMode );
			}
			WaitOSCRunDelayms( 20 );
		}
	}
}

//-----------------------------------------------
//函数功能: CPU系统时钟初始化函数，模块内函数
//
//参数: 	Type[in]		ePowerOnMode:正常上电  ePowerDownMode:低功耗
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_MCU_SysClockInit( ePOWER_MODE Type )
{
	BYTE adj, u80, u81;
	DWORD u320,u321; 
	
	
    EnWr_WPREG();
	
	//CLKCTRL0、CLKCTRL1由16位扩到24位
    HT_CMU->CLKCTRL0 &= ~0x000380;//关闭所有的时钟停振检测
    if( Type == ePowerOnMode )
    {
	    HT_CMU->CLKCTRL0 |= 0x000010;//使能PLL
		#if( SPD_MCU == SPD_22000K )
	    HT_CMU->PREFETCH = 0x0001;	//矩泉说22Mhz时也加，起滤波作用
		#endif
		HT_CMU->SYSCLKDIV = SPD_MCU;//设置Fcpu=Fsys/2
	    //while (HT_CMU->CLKSTA & 0x0010);//等待Fpll正常    
	    if( HT_CMU->SYSCLKCFG & 0x0002 )//当前时钟为fpll或fhrc
	    {
	    	api_Delayms( 3 );
	    }
	    else
	    {
	    	api_DelayNop( 99 );
	    }
	    
	    HT_CMU->SYSCLKCFG = 0x0083;//设置Fsys为Fpll（设置系统时钟时，需将bit7置1）
	    api_DelayNop(5);
		HT_CMU->SYSCLKCFG = 0x0003;//再次设置Fsys为Fpll（这一步基本没有用）
	}
	else
	{
		//HRC需要从infoflash取出补偿值 可保证在11m左右 误差不超过1.2% 否则输出不稳定
		u320 = *(DWORD *)0x40140;
		u321 = *(DWORD *)0x40144;
		u321 = ~u321;
		u80  = *(BYTE *)0x40140;
		u81  = *(BYTE *)0x40141;
		u81  = ~u81;

		if(u320 == u321)       //11M
		{
			adj = u320;
		}
		else if(u80 == u81)    //14M 最初的6015的调校值为14M
		{
			adj = u80;
		}
		else                    //8M 如果数据错误 8M附近最准确 因此固定为8M
		{
			adj = 0x3D;
		}
		
		HT_CMU->CLKCTRL0 |= 0x000020;//使能HRC
	 	HT_CMU->PREFETCH = 0x0000;//低功耗下将预取指令置为零 避免影响红外唤醒
		HT_CMU->HRCADJ = adj;                //输出11MHz
		HT_CMU->HRCDIV = 0x0001;//设置为2分频，缺省就是这个
		HT_CMU->SYSCLKDIV = 0;//设置Fcpu=Fsys/2  设置为不分频
	    //while (HT_CMU->CLKSTA & 0x0010);//等待Fpll正常    
	    if( HT_CMU->SYSCLKCFG & 0x0002 )//当前时钟为fpll或fhrc
	    {
	    	api_Delayms( 3 );
	    }
	    else
	    {
	    	api_DelayNop( 99 );
	    }
	    
	    HT_CMU->SYSCLKCFG = 0x0082;//设置Fsys为HRC（设置系统时钟时，需将bit7置1）
	    api_DelayNop(5);
		HT_CMU->SYSCLKCFG = 0x0002;//再次设置Fsys为HRC（这一步基本没有用）
	}
	
	//HT_PMU->PMUCON=0x0;//关闭各种掉电中断2016.6.27
    HT_PMU->PMUCON=0x0004;//开启LVDIN0低电压检测
    HT_PMU->PMUIF=0;
    
    DisWr_WPREG();
}

//-----------------------------------------------
//函数功能:     systick定时器初始化函数
//
//参数: 	 	Type[in]		ePowerOnMode:正常上电   ePowerDownMode:低功耗
//                    
//返回值:  	无
//
//备注:       ePowerDownMode模式 仅用于单相
//-----------------------------------------------
void api_InitSysTickTimer( ePOWER_MODE Type)
{
    SysTick->CTRL = 0x00000000;    
    
    if( Type == ePowerOnMode )
    {
        SysTick->LOAD = C_SysTickLoad;
    }
    else
    {
        SysTick->LOAD = C_SingleLowPowerSysTickLoad;
    }
    
    SysTick->VAL  = 0x00000000;
    NVIC_SetPriority(SysTick_IRQn, 3);
    SysTick->CTRL = 0x00000007;
}

void InitTimer4( void )
{
	EnWr_WPREG();
	
	HT_CMU->CLKCTRL1 |= CMU_CLKCTRL1_TMR4EN;
	HT_TMR4->TMRDIV = 0;
	HT_TMR4->TMRPRD = 0xFFFF;
	HT_TMR4->TMRIE = 0;
	HT_TMR4->TMRCON = 0x0107;//选择flf时钟（32768），周期定时模式
	
	DisWr_WPREG();
}

#if( PREPAY_MODE == PREPAY_LOCAL )
//-----------------------------------------------
//函数功能: 定时器0配置
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void InitTimer0( void )
{
	EnWr_WPREG();
	
	HT_CMU->CLKCTRL1 |= CMU_CLKCTRL1_TMR0EN;
	HT_TMR0->TMRDIV = 0;
	HT_TMR0->TMRCMP = 0x0ABE;//占空比为50%
	HT_TMR0->TMRPRD = 0x157C;//频率为4K
	HT_TMR0->TMRIE = 0;
	HT_TMR0->TMRCON = 0x030B;//选择Fpll时钟（22M），pwm模式,向上计数
	
	DisWr_WPREG();
}
#endif

//-----------------------------------------------
//函数功能: 获取波特率参数
//
//参数: 	Type[in]		0:初始化时用波特率 1：平时读波特率
//          SciPhNum[in]	物理串口号
//
//返回值:  	波特率参数值
//
//备注:   
//-----------------------------------------------
BYTE api_GetBaudRate(BYTE Type, BYTE SciPhNum)
{
    BYTE i;
    BYTE BpsBak, Status;
    Status = FALSE;

    if(Type == 0)
    {
        //判断FPara2的校验
        if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == TRUE )
        {
            Status = TRUE;
        }
    }
    else
    {
        Status = TRUE;
    }

	for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
    	if(SerialMap[i].SCI_Ph_Num == SciPhNum)
        {
        	if(i == eRS485_I)//485
            {
            	if( Status == TRUE )
                {
                 	BpsBak = FPara2->CommPara.I485;
                }
                else
                {
                  	BpsBak = CommParaConst.I485;//115200 1停止位 偶校验
                }
            }
            else if(i == ePT_UART1_F415)//415
            {
				BpsBak = 0x4A;//6025与415之间波特率115200
            }
            else if(i == eCR)  //载波
            {
            	if( Status == TRUE )
                {
                  	BpsBak = FPara2->CommPara.ComModule;
                }
                else
                {
                  	BpsBak = CommParaConst.ComModule;//9600bps
                }
           	}
            else if(i == eBlueTooth)	//蓝牙!!!!!!同国网导轨表一致
            {
            	if( Status == TRUE )
                {
                   	BpsBak = FPara2->CommPara.II485;
                }
                else
                {
                  	BpsBak = CommParaConst.II485;//2400bps
                }
            }

            break;
        }
    }

    return BpsBak;
}


//-----------------------------------------------
//函数功能: 处理更改波特率后的串口初始化
//
//参数: 	Channel[in]		逻辑串口号 0：第一485  1：红外  2：模块
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void DealSciFlag( BYTE Channel )
{
    //有改波特操作，电表应答完再改波特率
    if( Serial[Channel].OperationFlag & BIT0 )
    {
    	Serial[Channel].OperationFlag &= ~BIT0;
    	SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
    }
}


//-----------------------------------------------
//函数功能: 串口初始化
//
//参数: 
//			Channel[in]		逻辑串口号 0：第一485  1：红外  2：模块
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_InitSci( BYTE Channel )
{
	SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
}


//-----------------------------------------------
//函数功能: 禁止指定串口接收
//
//参数: 
//			p[in]		需要禁止串口的指针
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_DisableRece(TSerial * p)
{
	BYTE i;
	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( p == &Serial[i] )
		{
			SerialMap[i].SCIDisableRcv(SerialMap[i].SCI_Ph_Num);
		}
	}
}


//-----------------------------------------------
//函数功能: 允许指定接口接收
//
//参数: 
//			p[in]		需要允许串口的指针
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_EnableRece(TSerial * p)
{
	BYTE i;
	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( p == &Serial[i] )
		{
			SerialMap[i].SCIEnableRcv(SerialMap[i].SCI_Ph_Num);
		}
	}
}


//-----------------------------------------------
//函数功能: 广播命令无应答的情况下串口恢复接收
//
//参数: 
//			Channel[in]		逻辑串口号 0：第一485  1：红外  2：模块
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ProSciTimer( BYTE Channel )
{
	BYTE IntStatus;

    if(Serial[Channel].BroadCastFlag==0xff)
	{
		DealSciFlag( Channel );
		SerialMap[Channel].SCIEnableRcv(SerialMap[Channel].SCI_Ph_Num);

		IntStatus = api_splx(0);
		api_InitSciStatus( Channel );
		api_splx(IntStatus);
	}
}


//-----------------------------------------------
//函数功能: CPU初始化
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_InitCPU( void )
{
	api_Delay100us(10);
	FunctionConst(INIT_BASE_TIMER);
	//冷复位等待 OSC起振
	ResetWaitOSCRun();
	//初始化系统时钟
	api_MCU_SysClockInit( ePowerOnMode );
	//初始化开关
	InitWatchDog();
	CLEAR_WATCH_DOG;
	//系统定时器初始化（Tick定时器）
	api_InitSysTickTimer( ePowerOnMode );
	//初始化定时器4，用于查运行时间，时钟为32768
	InitTimer4();
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//初始化定时器0，用于pwm驱动的蜂鸣器
	InitTimer0();
	#endif
}


//-----------------------------------------------
//函数功能: 看门狗初始化，HT6025的看门狗默认是开启的
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void InitWatchDog(void)
{
    //HT_WDT->WDTCFG  = 0x00;//配置为看门狗溢出复位
    HT_WDT->WDTSET = 0xAAfF;//FeedWDT per 4s
}


//-----------------------------------------------
//函数功能: RTC的中断使能
//
//参数: 	Type[in]		0:秒 分 时中断开启  1:秒中断开启
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_MCU_RTCInt( void )
{
    EnWr_WPREG();
    
    HT_RTC->RTCCON = 0x0000;

    HT_RTC->RTCIE = 0x0001;//RTC 秒 中断使能位

    NVIC_EnableIRQ(RTC_IRQn);
    
    DisWr_WPREG();
}

//-----------------------------------------------
//函数功能: RTC的中断使能
//
//参数: 	Type[in]:中断周期sec秒
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_MCU_RTCIntSec( BYTE Sec )
{
	if( Sec == 0 )
	{
		Sec = 1;
	}
	
    EnWr_WPREG();
	NVIC_DisableIRQ(RTC_IRQn);		//禁止RTC中断
	HT_RTC->RTCCON &= (~0x0020);    //禁止RTC定时器1
	HT_RTC->RTCTMR1= Sec-1;			//RTC1定时周期SEC秒
	HT_RTC->RTCCON = 0x0020;        //使能RTC定时器1
	HT_RTC->RTCIE = 0x0020;			//使能RTC定时器1中断
	HT_RTC->RTCIF  = 0x0000;		//清中断标志
	NVIC_EnableIRQ(RTC_IRQn);		//使能RTC中断
	DisWr_WPREG();	
}

//-----------------------------------------------
//函数功能: 关闭和恢复中断
//
//参数: 
//			Data[in]		0:关闭中断  其他（关闭前记录的数据）：恢复中断
//                    
//返回值:  	关闭前的中断状态
//
//备注:   
//-----------------------------------------------
BYTE api_splx(BYTE Data)
{
    BYTE SRValue;
    SRValue = __get_PRIMASK();

    if((Data & 0x01) == 0x00)
    {
        DISABLE_CPU_INT;
    }
    else
    {
        ENABLE_CPU_INT;
    }

    return (~SRValue) & 0x01;
}

#if( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//函数功能: 复位卡片
//
//参数: 	无
//                   
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ResetCard( void )
{
	EnWr_WPREG();
	#if( MAX_PHASE == 1 )
	//开启CLKOUT
	if (!(HT_CMU->CLKCTRL0 & 0x001000)
	|| (HT_CMU->CLKOUTSEL != 0x0003)
	|| (HT_CMU->CLKOUTDIV != 0x0002))
	{
		HT_CMU->CLKCTRL0 |= 0x001000;				//使能CLKOUT
		HT_CMU->CLKOUTSEL = 0x0003;					//Fsys
		HT_CMU->CLKOUTDIV = 0x0002;					//Fsys/(2*(2+1))=3.670016MHz
	}
	OPEN_CARD_IO;									//配置卡IO引脚--功能端口 推挽输出
	OPEN_CARD_CLK;									//配置卡CLK引脚--功能端口 推挽输出
	#else

	//避免短时间重复复位ESAM
	if( ESAM_POWER_IS_CLOSE )//esam电源未打开
	{
		api_ResetEsamSpi(); 
	}

	POWER_CARD_OPEN;	//从打开电源到晶体起振稳定 大约需要600us时间
	#endif//#if( MAX_PHASE == 3 )
	
	RESET_CARD;
	DisWr_WPREG();
}

//---------------------------------------------------------------
//函数功能: 串口数据是否发送结束
//
//参数:   
//		SciPhNum -- 串口号	
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL api_UartIsSendEnd( BYTE SciPhNum )
{
	WORD Count;
	HT_UART_TypeDef *USARTx;
	
	Count = 1500; //按1200波特率算 发送两个字节的时间 大约15000us。
	
	switch( SciPhNum )
	{
		case 0:
			USARTx = HT_UART0;
			break;
		case 1:
			USARTx = HT_UART1;
			break;
		case 2:
			USARTx = HT_UART2;
			break;
		case 3:
			USARTx = HT_UART3;
			break;
		case 4:
			USARTx = HT_UART4;
			break;
		default:
			return FALSE;
	}
	
	while( (!(USARTx->UARTSTA & UART_UARTSTA_TXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	
	if( Count == 0 )
	{
		return FALSE;
	}
	
	return TRUE;
}
//-----------------------------------------------
//函数功能: 开启卡接收功能
//
//参数: 	无
//
//返回值:   无
//
//备注:
//-----------------------------------------------
void api_OpenRxCard(void)
{
	WORD Count;
	Count = 200;
	
	EnWr_WPREG();
	HT_CMU->CLKCTRL1 |= 0x000100;               //开启串口时钟
	DisWr_WPREG();
	
	#if( MAX_PHASE == 1 )
	NVIC_DisableIRQ(UART4_IRQn);				//禁止串口中断
	HT_UART4->MODESEL	 = 0x0001;				//7816功能
	HT_UART4->ISO7816STA = 0x0000;				//清除中断标志位
	HT_UART4->ISO7816CON = 0x01C0;				//偶校验,重收重发,重复2次,响应1位,禁止中断
	HT_UART4->SREL		 = 0x045B;				//9866bps
	OPEN_CARD_IO;								//配置卡IO引脚--功能端口 推挽输出
	OPEN_CARD_CLK;								//配置卡CLK引脚--功能端口 推挽输出
	#else
	NVIC_DisableIRQ(UART4_IRQn);				//禁止串口中断
	HT_UART4->MODESEL  = 0x0000;				//UART功能（非7016功能）
	HT_UART4->UARTSTA  = 0x0000;				//清除中断标志位	
	HT_UART4->UARTCON  = 0x0150;				//偶校验 2位停止位
	HT_UART4->SREL     = 0x03E7;				//非标准波特率 波特率为11010
	HT_UART4->UARTCON &= (~0x0001);             //发送禁止
	HT_UART4->UARTCON |= 0x0002;				//接受使能	
	#endif//#if( MAX_PHASE == 1 )
}

//-----------------------------------------------
//函数功能: 开启卡发送功能
//
//参数: 	head[in] - 发送首字节
//                   
//返回值:   无
//
//备注:   
//-----------------------------------------------
void api_OpenTxCard( BYTE head )
{
	EnWr_WPREG();
	HT_CMU->CLKCTRL1 |= 0x000100;               //开启串口时钟
	DisWr_WPREG();

	#if( MAX_PHASE == 1 )
	NVIC_DisableIRQ(UART4_IRQn);				//禁止串口中断
	HT_UART4->MODESEL	 = 0x0001;				//7816功能
	HT_UART4->ISO7816STA = 0x0000;				//清除中断标志位
	HT_UART4->ISO7816CON = 0x01C0;				//偶校验,重收重发,重复2次,响应1位,禁止中断
	HT_UART4->SREL		 = 0x045B;				//9866bps
	HT_UART4->SBUF		 = head;
	OPEN_CARD_IO;								//配置卡IO引脚--功能端口 推挽输出
	OPEN_CARD_CLK;								//配置卡CLK引脚--功能端口 推挽输出
	#else
	NVIC_DisableIRQ(UART4_IRQn);				//禁止串口中断
	HT_UART4->MODESEL  = 0x0000;				//UART功能（非7016功能）
	HT_UART4->UARTSTA  = 0x0000;				//清除中断标志位	
	HT_UART4->UARTCON  = 0x0150;				//偶校验 2位停止位
	HT_UART4->SREL     = 0x03E7;				//非标准波特率 波特率为11010
	HT_UART4->UARTCON |= 0x0001; 				//发送使能
	HT_UART4->UARTCON &= (~0x0002);				//接受禁止
	HT_UART4->SBUF = head;
	#endif//#if( MAX_PHASE == 1 )
}

//-----------------------------------------------
//函数功能: 关闭卡接口
//
//参数: 	head[in] - 发送首字节
//                   
//返回值:   无
//
//备注:   
//-----------------------------------------------
void api_CloseCard(void)
{
	NVIC_DisableIRQ(UART4_IRQn);				//禁止串口中断
	#if( MAX_PHASE == 1 )
	HT_UART4->ISO7816CON = 0x0000;				//禁止串口功能

	EnWr_WPREG();
	HT_CMU->CLKCTRL0 &= (~0x001000);				//停止时钟
	DisWr_WPREG();
	
	CLOSE_CARD_IO;								//配置卡IO引脚--普通端口 开漏输出
	CLOSE_CARD_CLK;								//配置卡CLK引脚--普通端口 开漏输出
	
	#else
	POWER_CARD_CLOSE;								
	#endif//#if( MAX_PHASE == 1 )
}
//-----------------------------------------------
//函数功能: 非中断方式7816读卡一个字节
//
//参数: 	接收一个字节的缓存
//                   
//返回值:   BOOL 读取结果是否成功，成功则改变Data中的值为读取的数据。如果不成功，则Data中数据置为ff
//
//备注:   18.12.30，增加接收字节是否成功判断，防止多个字节一直等待接收，导致看门狗复位
//-----------------------------------------------

BOOL api_ReceiveCardByte( BYTE *Data )
{
	WORD Count;
	
	Count = 10000;//防止误判失败，增加为10000，IAR采用中度优化，如果不用6000用户卡返写时间不够，用户卡返写需要33ms
	
	#if( MAX_PHASE == 1 )
	//while( !(HT_UART4->ISO7816STA & UART_UARTSTA_RXIF) )
	while( (!(HT_UART4->ISO7816STA & UART_UARTSTA_RXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	HT_UART4->ISO7816STA &= ~UART_UARTSTA_RXIF;
	if(Count == 0)
	{
		ASSERT(FALSE,0);
		*Data = 0xff;
		return FALSE;//长时间接受不到卡发送的数据，则置为FALSE。
	}
	*Data = HT_UART4->SBUF;
	return TRUE;

	#else
	while( (!(HT_UART4->UARTSTA & UART_UARTSTA_RXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	HT_UART4->UARTSTA &= ~UART_UARTSTA_RXIF;
	if(Count == 0)
	{
		ASSERT(FALSE,0);
		*Data = 0xff;
		return FALSE;//长时间接受不到卡发送的数据，则置为FALSE。
	}
	*Data = HT_UART4->SBUF;
	return TRUE;
	#endif//#if( MAX_PHASE == 1 )
}



//-----------------------------------------------
//函数功能: 非中断方式7816读卡一个字节
//
//参数: 	写入字节
//                   
//返回值:   TRUE/FALSE
//
//备注:   
//-----------------------------------------------
BOOL api_SendCardByte( BYTE Data )
{
	WORD Count;
	
	Count = 2000;
	
	#if( MAX_PHASE == 1 )
	while( (!(HT_UART4->ISO7816STA & UART_UARTSTA_TXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	if( Count == 0 )
	{
		return FALSE;
	}
	
	HT_UART4->ISO7816STA &= ~UART_UARTSTA_TXIF;
	HT_UART4->SBUF = Data;
	
	#else
	while( (!(HT_UART4->UARTSTA & UART_UARTSTA_TXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	if( Count == 0 )
	{
		return FALSE;
	}
	
	HT_UART4->UARTSTA &= ~UART_UARTSTA_TXIF;
	HT_UART4->SBUF = Data;
	#endif//#if( MAX_PHASE == 1 )
	
	return TRUE;
}


#endif//if( PREPAY_MODE == PREPAY_LOCAL )
//-----------------------------------------------
//函数功能: 计算cpu Flash的累加和，采用word相加模式
//
//参数: 	Type[in]0--程序区间Sum和	1--保护区参数Sum和	ff--全部flash区间Sum和
//                    
//返回值:  	计算的word累加和
//
//备注:95ms   
//-----------------------------------------------
WORD api_CheckCpuFlashSum(BYTE Type)
{
    DWORD Sum;
	DWORD StartZone,EndZone;
    WORD *pData;
    
    CLEAR_WATCH_DOG;
	
	#if( CPU_TYPE != CPU_HT6025 )
	换成其他cpu需要重新配置起始截止地址
	#endif
	
	if( Type == 0 )
	{
		StartZone = 0;
		EndZone = 0x3f000;
	}
	else if(  Type == 1 )
	{
		StartZone = 0x3f000;		
		EndZone = 0x40000;

	}
	else
	{
		StartZone = 0;		
		EndZone = 0x40000;
	}
	
	Sum = 0;
 
	for( pData = (WORD *)StartZone; pData < (WORD *)EndZone; pData++ )
	{
		Sum += *pData;
	}
    
    CLEAR_WATCH_DOG;
    
    return (WORD)Sum;
}


//-----------------------------------------------
//函数功能: 读取cpu内ram及片上flash数据
//
//参数: 	Type[in]	0--Ram 	1--flash
//          Addr[in]	地址 
//			Len[in]		读取长度    
//			Buf[out]	输出缓冲     
//
//返回值:  	读取数据的长度，如果为0，表示读取失败
//
//备注:   
//-----------------------------------------------
BYTE api_ReadCpuRamAndInFlash(BYTE Type, DWORD Addr, BYTE Len, BYTE *Buf)
{
	if( Type > 1 )
	{
		return 0;
	}
	
	if( (Addr>=0x00040400) && (Addr<0x20000000) )
	{
		return 0;
	}
	if( (Addr>=0x20008000) && (Addr<0x40000000) )
	{
		return 0;
	}
	if( Addr >= 0x40030000 )
	{
		return 0;
	}
	
	if( ((Addr+Len)>=0x00040400) && ((Addr+Len)<0x20000000) )
	{
		return 0;
	}
	if( ((Addr+Len)>=0x20008000) && ((Addr+Len)<0x40000000) )
	{
		return 0;
	}
	if( (Addr+Len) >= 0x40030000 )
	{
		return 0;
	}
	
	memcpy(Buf,(BYTE*)Addr,Len);
	
	return Len;
}


//-----------------------------------------------
//函数功能: 升级程序flash
//
//参数: 	无     
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void UpdateProg( void ) 
{
	BYTE i,ProgMessageNum,ProgData[PROGRAM_FLASH_SECTOR];//一次升级1024个字节
	BYTE IntStatus;
	WORD j;
	DWORD ProgAddr;
	TUpdateProgMessage TmpUpdateProgMessage;
	TUpdateProgHead TmpUpdateProgHead;
	BYTE *pData;
	
	FunctionConst(UPDATEPROG_TASK);
	//获取数据
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TmpUpdateProgHead), (BYTE*)&TmpUpdateProgHead ) != TRUE )
	{
		return;
	}
	ProgAddr = TmpUpdateProgHead.ProgAddr;
	ProgMessageNum = TmpUpdateProgHead.ProgMessageNum;
	
	if( ProgMessageNum > (PROGRAM_FLASH_SECTOR/(sizeof(TmpUpdateProgMessage.ProgData))) )
	{
		return;
	}
	
//@	if( (ProgAddr%PROGRAM_FLASH_SECTOR) != 0 )
//@	{
//@		return;
//@	}
	
	memcpy( (void*)ProgData, (BYTE *)ProgAddr, sizeof(ProgData) );
	
	for(i=0; i<ProgMessageNum; i++)
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[i]), sizeof(TmpUpdateProgMessage), (BYTE*)&TmpUpdateProgMessage ) != TRUE )
		{
			return;
		}
		
		if( TmpUpdateProgMessage.ProgFrameLen )//新方式帧长度不为0
		{	
			memcpy(&ProgData[TmpUpdateProgMessage.ProgOffset], &TmpUpdateProgMessage.ProgData[0], TmpUpdateProgMessage.ProgFrameLen);
		}
		else
		{
			memcpy(&ProgData[sizeof(TmpUpdateProgMessage.ProgData)*i], &TmpUpdateProgMessage.ProgData[0], sizeof(TmpUpdateProgMessage.ProgData));
		}
	} 

	//更新数据
	for(i=0; i<2; i++)
	{
		IntStatus = api_splx(0);
		
		if( HT_Flash_PageErase( ProgAddr ) == FALSE )
		{
			api_splx(IntStatus);
			return;
		}
		
		if( HT_Flash_ByteWrite( ProgData, ProgAddr, PROGRAM_FLASH_SECTOR ) == FALSE )
		{
			api_splx(IntStatus);
			return;
		}
		
		api_splx(IntStatus);
		
		pData = (BYTE *)ProgAddr;
		for(j=0; j<PROGRAM_FLASH_SECTOR; j++)
		{
			if( ProgData[j] != *(pData+j) )
			{
				break;
			}
		}
		
		//数据写成功 跳出循环
		if( j == PROGRAM_FLASH_SECTOR )
		{
			break;;
		}
	}
	
	//数据写失败
	if( (i == 2) && (j != PROGRAM_FLASH_SECTOR) )
	{
		return;
	}
	
	//把eeprom中的报文头改为无效
	TmpUpdateProgHead.ProgAddr = 0x3fc01;
  	TmpUpdateProgHead.ProgMessageNum = (PROGRAM_FLASH_SECTOR/(sizeof(TmpUpdateProgMessage)-sizeof(DWORD)))+1;
  	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( UpdateProgSafeRom.UpdateProgHead ), sizeof(TmpUpdateProgHead), (BYTE *)&TmpUpdateProgHead );
}


//-----------------------------------------------
//函数功能: 规约写升级程序信息头
//
//参数: 	pBuf[in] 输入缓冲     
//
//返回值:  	无
//
//备注: 规约见《电表软件平台扩展规约》附录C  
//-----------------------------------------------
void api_WriteUpdateProgHead(BYTE *pBuf)
{
	TUpdateProgHead TmpUpdateProgHead;

	memcpy((BYTE*)&TmpUpdateProgHead.ProgAddr,pBuf,4);
	TmpUpdateProgHead.ProgMessageNum = pBuf[4];
	
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE*)&TmpUpdateProgHead );	
	
	//清状态位，一次程序升级不允许掉电，升级过程中掉电需要重新升级
	ReceiveProgStatus = 0;
}


//-----------------------------------------------
//函数功能: 规约写升级程序数据
//
//参数: 	pBuf[in] 输入缓冲     
//
//返回值:  	TRUE/FALSE
//
//备注: 规约见《电表软件平台扩展规约》附录C  
//-----------------------------------------------
BOOL api_WriteUpdateProgMessage(BYTE *pBuf)
{
	TTwoByteUnion tw;
	TUpdateProgMessage TmpUpdateProgMessage;
	
	tw.w = 0;
	
	memcpy(tw.b,pBuf,2);	//帧序号
	if( tw.w >= (PROGRAM_FLASH_SECTOR /(sizeof(TmpUpdateProgMessage.ProgData))) )
	{
		return FALSE;
	}
	
	//旧升级方式的帧长度、偏移地址置为 0
	TmpUpdateProgMessage.ProgFrameLen = 0; //帧长度
	TmpUpdateProgMessage.ProgOffset = 0; //偏移地址
	
	//128字节数据，不倒序
	memcpy(TmpUpdateProgMessage.ProgData,pBuf+2,sizeof(TmpUpdateProgMessage.ProgData));
	
	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32( TmpUpdateProgMessage.ProgData, sizeof(TmpUpdateProgMessage)-sizeof(DWORD) );
	
	//不能对单帧重写，只能重头开始重写
	if( (ReceiveProgStatus&(0x01L<<tw.w)) == 0 )
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE*)&TmpUpdateProgMessage );
		ReceiveProgStatus |= (0x01L<<tw.w);
	}
	
	return TRUE;
}
//-----------------------------------------------
//函数功能: 规约写升级程序数据
//
//参数: 	pBuf[in] 输入缓冲     
//
//返回值:  	TRUE/FALSE
//
//备注: 规约见《电表软件平台扩展规约》附录C  
//-----------------------------------------------

BOOL api_WriteUpdateProgMessageNew(BYTE *pBuf)
{
	TTwoByteUnion tw;
	TTwoByteUnion len;
	TUpdateProgMessage TmpUpdateProgMessage;
	
	tw.w = 0;
	len.w = 0;
	
	memcpy(tw.b,pBuf,2);	//帧序号
	if( tw.w >= (PROGRAM_FLASH_SECTOR/(sizeof(TmpUpdateProgMessage.ProgData))) )
	{
		return FALSE;
	}
	
	memcpy(len.b,pBuf+2,2);	//帧长度
	if( (len.w > (sizeof(TmpUpdateProgMessage.ProgData))) || (len.w == 0) )
	{
		return FALSE;
	}
	TmpUpdateProgMessage.ProgFrameLen = len.w; //帧长度
	
	memcpy((BYTE *)&TmpUpdateProgMessage.ProgOffset,pBuf+4,2); //偏移地址
	if( (TmpUpdateProgMessage.ProgFrameLen + TmpUpdateProgMessage.ProgOffset) > PROGRAM_FLASH_SECTOR )
	{
		return FALSE;
	}

	memset(TmpUpdateProgMessage.ProgData,0,sizeof(TmpUpdateProgMessage.ProgData));
	memcpy(TmpUpdateProgMessage.ProgData,pBuf+6,len.w);
	
	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32( (BYTE *)&TmpUpdateProgMessage, sizeof(TmpUpdateProgMessage)-sizeof(DWORD) );
	
	//不能对单帧重写，只能重头开始重写
	if( (ReceiveProgStatus&(0x01L<<tw.w)) == 0 )
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE*)&TmpUpdateProgMessage );
		ReceiveProgStatus |= (0x01L<<tw.w);
	}
	
	return TRUE;
}


//-----------------------------------------------
//函数功能: 启动程序升级
//
//参数: 	pBuf[in] 输入缓冲     
//
//返回值:  	此函数最后会执行数据转存及软复位
//
//备注: 规约见《电表软件平台扩展规约》附录C  
//-----------------------------------------------
void api_StartUpdateProgMessage(BYTE *pBuf)
{
	BOOL Result;
	BYTE i,Num;
	TFourByteUnion tdw;	
	TUpdateProgHead TmpUpdateProgHead;
	
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE*)&TmpUpdateProgHead );	
	if(Result != TRUE)
	{
		return;
	}
	
	memcpy(tdw.b,pBuf,4);
	if(tdw.d != TmpUpdateProgHead.ProgAddr)
	{
		return;
	}
	
	if(TmpUpdateProgHead.ProgMessageNum != pBuf[4])
	{
		return;
	}
	
	Num = 0;
	for(i=0;i<TmpUpdateProgHead.ProgMessageNum;i++)
	{
		if( ReceiveProgStatus & (0x01L<<i) )
		{
			Num++;
		}
	}
	if(Num != TmpUpdateProgHead.ProgMessageNum)
	{
		return;
	}
	
	//做一下掉电保存 !!!!!!
	api_PowerDownEnergy();
	
	ProgFlashPW = 0xa55a;
	
	UpdateProg();
	
	ProgFlashPW = 0;
	ReceiveProgStatus = 0;
	
	//RESET
	//Reset_CPU();
}

//---------------------------------------------------------------
//函数功能: HT6025K新增写保护寄存器维护
//
//参数: 	无
//
//返回值:  	无
//
//备注:
//---------------------------------------------------------------
void api_FreshNewRegister( void )
{
	EnWr_WPREG();                           //关闭写保护

	//HT6025K新增写保护寄存器
	if(HT_CMU->LFDETCFG != 0x8020)
	{
		HT_CMU->CLKCTRL0 |= 0x020000;       //关闭CMU新增写保护
		HT_CMU->LFDETCFG = 0x8020;          //生效位为1，配置有效，配置值为默认值
		HT_CMU->CLKCTRL0 &= ~0x020000;      //打开CMU新增写保护
	}
	if(HT_CMU->MULTFUNCFG != 0x8000)
	{
		HT_CMU->CLKCTRL0 |= 0x020000;       //关闭CMU新增写保护
		HT_CMU->MULTFUNCFG = 0x8000;        //生效位为1，配置有效，配置值为默认值
		HT_CMU->CLKCTRL0 &= ~0x020000;      //打开CMU新增写保护
	}
	if(HT_CMU->LFCLKCFG != 0x8000)
	{
		HT_CMU->CLKCTRL0 |= 0x020000;       //关闭CMU新增写保护
		HT_CMU->LFCLKCFG = 0x8000;          //生效位为1，配置有效，配置值为默认值
		HT_CMU->CLKCTRL0 &= ~0x020000;      //打开CMU新增写保护
	}

	//必须检查此寄存器 否则芯片进入测试模式会导致CLKOUT无输出 影响ESAM通讯
	if((HT_HFCFG&0x80) != 0)
	{
		HT_HFCFG = 0x00; //初始化为 0
	}

	DisWr_WPREG();                          //打开写保护
}
//-----------------------------------------------
//函数功能: 检查MCU使用的时钟频率
//
//参数: 无
//          
//		     
//
//返回值: 1：32.786K 0: 5.5M与其他
//
//备注:   
//-----------------------------------------------
BYTE  api_CheckMCUCLKIsFlf( void )
{
	//如果低功耗下时钟不是Flf 强制切换一下时钟 防止时钟没有被切换回来
	if( (HT_CMU->SYSCLKCFG&0x0007) != 0x0001 )//32768
	{
		return FALSE;
	}
	else 
	{
		return TRUE;
	}

}



/////////////////////////////////////////////////////////////////////
//以下是中断向量程序
/////////////////////////////////////////////////////////////////////

//复位中断向量 代码用start.s中的
//void Reset_Handler(void)
//{
//}

//代码用start.s中的 如果有中断则复位
//void NMI_Handler(void)
//{
//}

//代码用start.s中的 如果有中断则复位
//void HardFault_Handler(void)
//{
//	__NOP();
//}

//代码用start.s中的 如果有中断则复位
//void SVC_Handler(void)
//{
//}

//代码用start.s中的 如果有中断则复位
//void PendSV_Handler(void)
//{
//}

//10ms中断一次
void SysTick_Handler(void)
{
	asm("nop");
	SysTickIRQ_Service();
}

//代码用start.s中的 如果有中断则复位
void PMU_IRQHandler(void)
{
	PMUIRQ_Service();
}

//代码用start.s中的 如果有中断则复位
//void AES_IRQHandler(void)
//{
//}


void EXTI0_IRQHandler(void)
{
	EXTI0IRQ_Service();
}


void EXTI1_IRQHandler(void)
{
	EXTI1IRQ_Service();
}


void EXTI2_IRQHandler(void)
{
	EXTI2IRQ_Service();
}


void EXTI3_IRQHandler(void)
{
	EXTI3IRQ_Service();
}


void EXTI4_IRQHandler(void)
{
	EXTI4IRQ_Service();
}


void EXTI5_IRQHandler(void)
{
	EXTI5IRQ_Service();
}


void EXTI6_IRQHandler(void)
{
	EXTI6IRQ_Service();
}


void UART0_IRQHandler(void)
{
	UART0IRQ_Service();
}


void UART1_IRQHandler(void)
{
	UART1IRQ_Service();
}


void UART2_IRQHandler(void)
{
	UART2IRQ_Service();
}


void UART3_IRQHandler(void)
{
	UART3IRQ_Service();
}


void UART4_IRQHandler(void)
{
	UART4IRQ_Service();
}


void UART5_IRQHandler(void)
{
	UART5IRQ_Service();
}


void TIMER_0_IRQHandler(void)
{
	TIMER0IRQ_Service();
}


void TIMER_1_IRQHandler(void)
{
	TIMER1IRQ_Service();
}


void TIMER_2_IRQHandler(void)
{
	TIMER2IRQ_Service();
}


void TIMER_3_IRQHandler(void)
{
	TIMER3IRQ_Service();
}

//代码用start.s中的 如果有中断则复位
//void TBS_IRQHandler(void)
//{
//}


void RTC_IRQHandler(void)
{
	RTCIRQ_Service();
}

//代码用start.s中的 如果有中断则复位
//void I2C_IRQHandler(void)
//{
//}

//代码用start.s中的 如果有中断则复位
//void SPI0_IRQHandler(void)
//{
//}

//代码用start.s中的 如果有中断则复位
//void SPI1_IRQHandler(void)
//{
//}

//代码用start.s中的 如果有中断则复位
//void SelfTestFreq_IRQHandler(void)
//{
//}


void TIMER_4_IRQHandler(void)
{
	TIMER4IRQ_Service();
}


void TIMER_5_IRQHandler(void)
{
	TIMER5IRQ_Service();
}


void UART6_IRQHandler(void)
{
	UART6IRQ_Service();
}


void EXTI7_IRQHandler(void)
{
	EXTI7IRQ_Service();
}


void EXTI8_IRQHandler(void)
{
	EXTI8IRQ_Service();
}


void EXTI9_IRQHandler(void)
{
	EXTI9IRQ_Service();
}

//代码用start.s中的 如果有中断则复位
//void DMA_IRQHandler(void)
//{
//}


//////////////////////////////////////////////////////////////////////////
// 串口中断服务子程序,改服务程序要求中断不允许嵌套
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------
//函数功能: 串口接收中断函数
//
//参数: 	SCI_Ph_Num[in]	物理串口号
//          Data[in]		接收到的数据
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
__IO void USARTx_Rcv_IRQHandler(BYTE SCI_Ph_Num, BYTE Data)
{
    BYTE i;

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
        {
            if(Serial[ i ].RXWPoint < MAX_PRO_BUF)
            {
	            Serial[ i ].ProBuf[Serial[ i ].RXWPoint++] = Data;
            }

            if(Serial[ i ].RXWPoint >= MAX_PRO_BUF)
            {
				//如果接受到数据等于大于最大值 但是协议里还一个字节都没处理 强制调用一次 解决发送512字节数据电表有概率不回复的问题
				if(Serial[i].RXRPoint == 0)
				{
					Proc645( i );
				}
                Serial[ i ].RXWPoint = 0;
            }
			Serial[i].SendToSendDelay = MAX_TX_COMMIDLETIME;
            break;
        }
    }
}


//-----------------------------------------------
//函数功能: 串口发送中断函数
//
//参数: 	SCI_Ph_Num[in]	物理串口号
//          USARTx[in]		对应的串口指针
//
//返回值:  	无
//
//备注:   
//-----------------------------------------------
__IO void USARTx_Send_IRQHandler(BYTE SCI_Ph_Num, HT_UART_TypeDef* USARTx)
{
    BYTE i, j;
    //BYTE IntStatus;

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
        {
            if(Serial[ i ].TXPoint < Serial[ i ].SendLength)
            {
                USARTx->SBUF = Serial[ i ].ProBuf[Serial[ i ].TXPoint++];
            }
            else//发送完毕，切换到接收状态
            {
                // 关闭中断
                USARTx->UARTCON &= ~0x0004; //禁止发送中断
				
                //如果有波特率改动 发完数据更新波特率 必须每个都遍历到 存在用485_1设置485_2的情况
				for( j = 0; j < MAX_COM_CHANNEL_NUM; j++ )
				{
					DealSciFlag( j );
				}
				//立转接收状态
                SerialMap[i].SCIEnableRcv(SerialMap[i].SCI_Ph_Num);
                //IntStatus = api_splx(0);
                api_InitSciStatus(i);
                //api_splx(IntStatus);
				Serial[i].SendToSendDelay = MAX_RX_COMMIDLETIME;
            }
            break;
        }
    }
}

#endif//#if ( CPU_TYPE == CPU_HT6025 )
