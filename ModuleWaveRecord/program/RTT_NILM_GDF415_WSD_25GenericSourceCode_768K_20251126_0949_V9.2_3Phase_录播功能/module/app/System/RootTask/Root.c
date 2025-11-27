//------------------------------------------------------------------------------------------
//	名    称： main.c
//	功    能:  系统入口函数
//	作    者:  wangjunsheng
//	创建时间:  2020.10.28
//	更新时间:
//	备    注:
//	修改记录:
//------------------------------------------------------------------------------------------
#include "AllHead.h"
#include "main_task.h"
//#include "LED.h"
#include "sm_task.h"
#include "fal.h"
#include "TaskFun.h"
#include "LED.h"
#include "task_update.h"
#include <rtthread.h>

#define E2PROM_SAFECON_LEN  ((sizeof(TSafeMem))+(sizeof(TConMem))+HALFRMN_MEM_SPACE)

volatile DWORD BufferLen=0;
extern TRealTimer RealTimer;//此处作为timer的主定义  声明为全局变量

extern int ChannelManagerInit(void);
extern void InitUartPara(void);
extern void delay_ms(uint32_t us);
void CheckSysLimit( void )
{
	int nSafeSize, nConSize, nSize;

	nSafeSize = sizeof(TSafeMem);

	nConSize = sizeof(TConMem);

	nSize = nSafeSize + nConSize + HALFRMN_MEM_SPACE;
  
	if(nSize >= CONTINUE_MEM_MAX)
	{
        for(;;);
	}

}

/**
 * @brief 检查UID是否正确，防盗版
 * 
 */
void CheckCodeSafety(void)
{
#if(!WIN32)

//WORD wData;
int nData;
	
	api_CheckCpuFlashSum(0);
	
	nData = memcmp((void *)(FLASH_ADDR_BOOT_VERSION+6), (void *)(*(DWORD *)(FLASH_ADDR_BOOT_VERSION+2)), 12);
	
	//if(memcmp((void *)(FLASH_ADDR_BOOT_VERSION+6), (void *)(*(DWORD *)(FLASH_ADDR_BOOT_VERSION+2)), 12) != 0)
     if(nData !=0 )
    {
        api_DeInitTIM2();
        InitLED();
        api_SetUpLED(LED_RED, 1);
        api_SetUpLED(LED_GREEN, 0);
        for(;;)
        {
            CLEAR_WATCH_DOG;
            //api_delayms
            delay_ms(500);
            api_SetUpLED(LED_RED, 2);
            api_SetUpLED(LED_GREEN, 2);
        }
    }
#endif
}

typedef struct tNoInitPara_t
{
    DWORD               dwMagic;
    int                 nInitComplete;
    TRealTimer           Time;
    DWORD               dwCrc;
    
}tNoInitPara;

__no_init tNoInitPara NoInitPara;

void initResetPara()
{

     if(NoInitPara.dwMagic != 0xA5B6C7D8)
    {
        NoInitPara.dwMagic = 0xA5B6C7D8;
        NoInitPara.nInitComplete = 0;
        get_sys_time(&NoInitPara.Time);
 
    }
    else
    {
        InitComplete =  NoInitPara.nInitComplete;
        if(NoInitPara.Time.Sec < 58)
          NoInitPara.Time.Sec += 2;
        
        api_WriteMeterTime(&NoInitPara.Time);
    }
}

void FreshResetPara()
{
      if(NoInitPara.dwMagic == 0xA5B6C7D8)
      {
           NoInitPara.nInitComplete = InitComplete;
           get_sys_time(&NoInitPara.Time);
      }
}
//--------------------------------------------------
//功能描述:  Root初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_InitRoot(void)
{
	#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	#if(USE_PRINTF==1)
	ChannelManagerInit();//默认jlinkrtt通道
	#endif
	#endif

	#if( SEL_DEBUG_VERSION == YES )
	DWORD hchtest[40];
	TestCode();
	#endif

	/* 检查编译系统正确性 */
	CheckSysLimit();

    /* 开启全局中断 */
#if(!WIN32)
    //__enable_irq();     // 需要提前开启，否则HAL_Delay()进入死循环
#endif

    /* 初始化CPU */
    api_InitCPU();

    /* 程序安全检测 */
#if(USE_CODE_SAFEMODE)
  // CheckCodeSafety();
#endif

	/* 初始化板件 */
	InitBoard();

	/*初始化flash 虚拟层*/
	fal_init();

	/* 初始化应用 */
	InitApp();

	/* 初始化安全库*/
	smTask_main();
			
	/* 初始化文件状态*/
	InitUpFileStatus();//wxy
				
	/* 初始化串口参数*/
	InitUartPara();
  
#if(USER_TEST_MODE)
    initResetPara();
#endif
}

#if(WIN32)
void mainTask(void* para)
#else
void RootTask(void* para)
#endif
{
	for(;;)
	{
		CLEAR_WATCH_DOG;

		if(InitComplete != 0)
		{
			//时间刷新后再进行冻结事件处理
			api_EventTask();

			api_FreezeTask();
		}

		api_ProtocolTask();
		
		api_RtcTask();

		api_JlinkTask();

		api_WatchTask();

		rt_thread_mdelay(1);
	}
}

#if( USE_DEBUG == YES )
DWORD hchtest[40];
void TestCode()
{
	WORD i,m,n;

	
	CLEAR_WATCH_DOG;
//	hchtest[0] = sizeof(TSafeMem);
//	hchtest[1] = sizeof(TConMem);
	// hchtest[2] = sizeof(TFreezeConRom);
//	hchtest[3] = sizeof(TEventConRom);
    //hchtest[5] = sizeof(TFreezeAttRom);
    // hchtest[6] = sizeof(TFreezeEeData);
//    hchtest[7] = FLASH_FREEZE_BASE;
//    hchtest[8] = FLASH_FREEZE_BASE+ 
	//hchtest[4] = sizeof(TFreezeAtt);
	//hchtest[5] = GET_CONTINUE_ADDR( FreezeAtt );
	//hchtest[6] = sizeof(PortDescript8[0]);
    //hchtest[7] = strlen(PortDescript);
    #if(THIRD_MEM_CHIP != CHIP_NO)
    // hchtest[7] = FLASH_FREEZE_END_ADDR;
    #endif
	//hchtest[8] = GET_CONTINUE_ADDR( FreezeAtt.FreezeTimeZone );
	// hchtest[9] =  sizeof(gEventPara.byLimit);
    
	//hchtest[10] =  GET_STRUCT_MEM_LEN( TDemandOverRom, EventData[0] );
	
    //hchtest[11] = GET_STRUCT_MEM_LEN( TDemandOverRom, EventData[0] );
	//memset( hchtest, 0XFF, 20 );
//	hchtest[5] = lib_CheckCRC32( (BYTE*)hchtest, 20 );
	
	//测试采样芯片
//  	Link_ReadSampleReg( 0, (BYTE *)&hchtest[0] );
//  	
//  	//POWER_ESAM_CLOSE;
//  	
//  	//测试esam
//  	POWER_ESAM_OPEN;
//	api_GetEsamInfo( 0x02, (BYTE *)&hchtest[0] );
  	
  	//测试eeprom1
 	 memset( hchtest, 0x01, 10 );
 	 WriteEEPRom1( 20000, 10, (BYTE *)hchtest );
 	 memset( (BYTE *)hchtest+12, 0x00, 10 );
 	 ReadEEPRom1( 20000, 10, (BYTE *)hchtest+12 );
  	
  	//测试eeprom2
 	 memset( hchtest, 0x02, 10 );
 	 WriteEEPRom2( 20000, 10, (BYTE *)hchtest );
 	 memset( (BYTE *)hchtest+12, 0x00, 10 );
 	 ReadEEPRom2( 20000, 10, (BYTE *)hchtest+12 );
  	
//  	//测试flash
  	#if( BOARD_TYPE == BOARD_ST_PQM )
  	memset( hchtest, 0xaa, 10 );
  	api_WriteMemRecordData(3L*4*1024, 10, (BYTE *)hchtest);
  	 memset( hchtest, 0x55, 10 );
  	api_ReadMemRecordData(3L*4*1024, 10, (BYTE *)hchtest);
  	api_ReadMemRecordData(0, 100, (BYTE *)hchtest);
  	#endif
   	
  	//api_ClrDemand(eClearAllDemand);
  	
  	// api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
  	// api_FactoryInitTask();
  	
  	//时钟输出测试
  	/*
  	EnWr_WPREG();//关闭寄存器写保护
  	HT_GPIOE->IOCFG |= GPIO_Pin_3;
  	HT_GPIOC->AFCFG &= ~GPIO_Pin_3;
  	HT_GPIOE->PTDIR |= GPIO_Pin_3;                                                                                                                                                   
  	HT_GPIOE->PTOD |= GPIO_Pin_3;
  	HT_CMU->CLKOUTSEL = 0x02;
  	HT_CMU->CLKOUTDIV = 7;
  	HT_CMU->CLKCTRL0 |= 0x1000;
  	
  	for(;;)
  	{
  		CLEAR_WATCH_DOG;
  	}  	*/
//	api_MeterClearEventStart();//wxy 2021.12.1
//	api_FactoryInitEvent();
//	api_FactoryInitFreeze();
//	api_MeterClearEventEnd();
} 
#endif

//--------------------------------------------------
//功能描述:  创建root任务
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_CreateRootTask(void)
{
	int tid;

	#if USE_DEBUG
	TestCode();
	ReadSm4Para(1);
	api_MeterClearEventStart();//wxy 2021.12.1
	api_FactoryInitEvent();
	api_FactoryInitFreeze();
	api_MeterClearEventEnd();
	#endif

	//创建电表主任务
    xTaskCreate(RootTask,          // 直接传递函数指针（去掉 void* 参数）
                "Root",
                1024*20,          // 栈深度（根据系统确认单位是字还是字节）
                NULL,              // 任务参数
                10,                // 优先级
                &tid               // 任务句柄指针（现在类型是 int*）
                );
}


