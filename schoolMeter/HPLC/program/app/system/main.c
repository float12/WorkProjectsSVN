//------------------------------------------------------------------------------------------
//	名    称： main.c
//	功    能:  系统入口函数
//	作    者:  liuzhijun
//	创建时间:  2002.6
//	更新时间:
//	备    注:
//	修改记录:
//------------------------------------------------------------------------------------------

#include "AllHead.h"

//检查编译系统正确性
void CheckSysLimit( void )
{
	#if( MAX_TIME_SEG_TABLE > 10 )//建议不要大于10
	for(;;);
	#endif
	
	#if( MAX_RATIO > 12 )//建议不要大于12
	for(;;);
	#endif
	
	#if( MAX_HOLIDAY > 20 )//公共假日大于21会冲内存
	for(;;);
	#endif
	
	#if( eTASK_ID_NUM_T > 16 )
	for(;;);
	#endif
	
	//以下几个宏定义负值给变量定义都是BYTE类型
	#if( cMETER_TYPES > 255 )
	for(;;);
	#endif
	
	#if( cCURR_TYPES > 255 )
	for(;;);
	#endif
	
	#if( cSPECIAL_CLIENTS > 255 )
	for(;;);
	#endif
	
	#if( SLAVE_MEM_CHIP != CHIP_NO )
	#if ( MASTER_MEMORY_CHIP != SLAVE_MEM_CHIP )
		"配置错误，两片eeprom必须是一样的"
	#endif
	#endif
	
	#ifndef SLE_THIRD_MEM_BACKUP
		#error "宏未定义"
	#endif
		  
	#if ( MAX_DEMAND_DOT < DEFAULT_DEMAND_DOT )
	错误DEMAND_DOT
	#endif

	if(eTASK_ID_NUM_T > MAX_TASK_ID_NUM)
	{
		for(;;);
	}
	
	if(eSYS_STATUS_NUM_T > MAX_SYS_STATUS_NUM)
	{
		for(;;);
	}
	
	if(eRUN_HARD_NUM_T > MAX_RUN_HARD_FLAG_NUM)
	{
		for(;;);
	}
	
	if(ePRO_HARD_NUM_T > MAX_PRO_HARD_FLAG_NUM)
	{
		for(;;);
	}

	#if( MAX_PHASE != 1 )
	if( SelDivAdjust == NO )
	{
		//三相表不允许关闭分段校准
		for(;;);
	}
	#endif

	// 第三片Flash不作为连续空间处理
	if( ((DWORD)2*sizeof(TSafeMem)+sizeof(TConMem)) > ((DWORD)SINGLE_CHIP_SIZE*2) )
	{
		for(;;);
	}

	#if(THIRD_MEM_CHIP != CHIP_NO)
	//	事件+冻结的数据
	if( FLASH_MEM_END_ADDR > THIRD_MEM_SPACE )
	{
		for(;;);
	}
	#endif

	//只有直通表有零线电流计量功能
	if(SelZeroCurrentConst == YES)
	{
		if(MeterTypesConst != METER_ZT)
		{
			for(;;); 
		}
	}

	#if( MAX_PHASE == 3)
	//网省供货电能表直流偶次谐波检测时间
	if((SoftVersionConst >> 8) != 0xFF )//非直通表 不开启直流偶次谐波
	{
		if(HalfWaveDefCheckTime == 0xFF)
		{
			for(;;); 
		}

	}

	if(MeterTypesConst != METER_ZT)
	{
		if(HalfWaveDefCheckTime != 0 )
		{
			for(;;); 
		}
	}
	#endif
	
	#if( MAX_PHASE == 3)

	if( ( (SoftVersionConst&0xff00) != 0xff00)
	|| (ESAMPowerControlConst ==NO) )//网省版本号 供货状态 不允许开启增益切换
	{
		if( SmallCurrentCorrection == YES )
		{
			for(;;); 
		}
		if( SelVolChangeGain == YES )//电压改变补偿，供货必须关闭
		{
			for(;;); 
		}
	}
	
	#endif
        
}


void CheckImportantPara( void )
{
	//防止脉冲常数太大，而电能存储又采用脉冲存储的情况
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == TRUE )
	{
		if( api_GetEnergyConst() == 1 )
		{
			if( FPara2->ActiveConstant > (DWORD)2*ActiveConstantConst )
			{
				api_SetError(ERR_PULSCONST_TOO_BIG);
			}
		}
	}
}

//大循环
void MainTask(void)
{
	for(;;)
	{
		CLEAR_WATCH_DOG;
		//api_CheckAllocBuf();
		//判断系统状态
		//若系统处于低功耗状态，则不执行以下程序
		if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )	
		{
			//系统监视处理，在停电状态下也要执行。
			api_SysWatchTask();
			FunctionConst(SYS_WATCH_TASK);
			
			if( api_CheckSysVol( eOnRunDetectPowerMode ) == FALSE )
			{			
				api_EnterLowPower( eFromOnRunEnterDownMode );
			}	
			//采集处理
			FunctionConst(SAMPLE_TASK);
			api_SampleTask();
			
			//需量计算
			#if( MAX_PHASE == 3 )
			api_DemandTask();
			#endif
			
			//电能处理
			api_EnergyTask();

			if( api_CheckSysVol( eOnRunDetectPowerMode ) == FALSE )
			{
				api_EnterLowPower( eFromOnRunEnterDownMode );
			}

			api_EventTask();
			
			RelayTask();
			
			FunctionConst(PRAYPAY_TASK);
			
			// api_PrePayTask(); //预付费处理
			
			api_ParaTask();
			//冻结之前增加掉电判断
			if( api_CheckSysVol( eOnRunDetectPowerMode ) == FALSE )
			{
				api_EnterLowPower( eFromOnRunEnterDownMode );
			}
			//冻结处理-必须在api_ParaTask()之后,且不能判断掉电。在para中置了切换标志后掉电，会丢一次约定冻结
			api_FreezeTask();
			//协议处理

			FunctionConst(PROTOCOL_TASK);
			
			//在大循环的最后面要加入进入低功耗判断
			if( api_CheckSysVol( eOnRunDetectPowerMode ) == FALSE )
			{
				api_EnterLowPower( eFromOnRunEnterDownMode );
			}
		}
		else
		{
			// if( api_CheckSysVol( eWakeupDetectPowerMode ) == TRUE )//低功耗上电
			// {
			// 	Reset_CPU();
			// }
			
			// api_LowPowerTask();
		}
                
		api_ProtocolTask();

		
		#if(SEL_F415 == YES)
		api_MCollectTask();//透传、搜表、采集
		#endif
		//LCD处理
		FunctionConst(LCD_RUN_TASK);
		api_LcdTask();
		FunctionConst(RTC_RUN_TASK);
		api_RtcTask();//RTC任务放在协议任务之后，避免冻结标志产生后协议更改RTC导致冻结时间不对

	}
}



#if( SEL_DEBUG_VERSION == YES )
DWORD hchtest[40];
void TestCode()
{
	WORD i,m,n;

	
	CLEAR_WATCH_DOG;
	hchtest[0] = sizeof(TSafeMem);
	hchtest[1] = sizeof(TConMem);
	hchtest[2] = sizeof(TFreezeConRom);
	hchtest[3] = sizeof(TEventConRom);
    //hchtest[5] = sizeof(TFreezeAttRom);
    hchtest[6] = sizeof(TFreezeEeData);
//    hchtest[7] = FLASH_FREEZE_BASE;
//    hchtest[8] = FLASH_FREEZE_BASE+ 
	//hchtest[4] = sizeof(TFreezeAtt);
	//hchtest[5] = GET_CONTINUE_ADDR( FreezeAtt );
	//hchtest[6] = sizeof(PortDescript8[0]);
    //hchtest[7] = strlen(PortDescript);
    #if(THIRD_MEM_CHIP != CHIP_NO)
    hchtest[7] = FLASH_FREEZE_END_ADDR;
    #endif
	//hchtest[8] = GET_CONTINUE_ADDR( FreezeAtt.FreezeTimeZone );
	hchtest[9] =  sizeof(gEventPara.byLimit);
    
	//hchtest[10] =  GET_STRUCT_MEM_LEN( TDemandOverRom, EventData[0] );
	
    //hchtest[11] = GET_STRUCT_MEM_LEN( TDemandOverRom, EventData[0] );
	//memset( hchtest, 0XFF, 20 );
//	hchtest[5] = lib_CheckCRC32( (BYTE*)hchtest, 20 );
	
	// //测试采样芯片
 	// Link_ReadSampleReg( 0, (BYTE *)&hchtest[0] );
 	
 	//POWER_ESAM_CLOSE;
//  	
//  	//测试esam
//  	POWER_ESAM_OPEN;
//	api_GetEsamInfo( 0x02, (BYTE *)&hchtest[0] );
//  	
 	//测试eeprom1
 	memset( hchtest, 0x01, 10 );
 	WriteEEPRom1( 20000, 10, (BYTE *)hchtest );
 	memset( (BYTE *)hchtest+12, 0x00, 10 );
 	ReadEEPRom1( 20000, 10, (BYTE *)hchtest+12 );
//  	
//  	//测试eeprom2
 	memset( hchtest, 0x02, 10 );
 	WriteEEPRom2( 20000, 10, (BYTE *)hchtest );
 	memset( (BYTE *)hchtest+12, 0x00, 10 );
 	ReadEEPRom2( 20000, 10, (BYTE *)hchtest+12 );
//  	
 	//测试flash
 	#if( BOARD_TYPE == BOARD_HT_SINGLE_20250819 )
 	memset( hchtest, 0xaa, 10 );
 	api_WriteMemRecordData(3L*4*1024, 10, (BYTE *)hchtest);
 	memset( hchtest, 0x55, 10 );
 	api_ReadMemRecordData(3L*4*1024, 10, (BYTE *)hchtest);
 	api_ReadMemRecordData(0, 100, (BYTE *)hchtest);
 	#endif
   	
  	// api_ClrDemand(eClearAllDemand);
  	
  	api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
  	api_FactoryInitTask();
  	
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
} 
#endif
//--------------------------------------------------
//功能描述:  主程序入口
//         
//参数:      
//         
//返回值:    
//         
//备注:  单相费控智能电能表（电动自行车充电自动识别）初版本-2
//--------------------------------------------------
void main(void)
{
	DISABLE_CPU_INT;		// 关闭中断
	
	//检查编译系统正确性
	CheckSysLimit();
	
	//!!!!!!内存初始化一定要放在最前面，因为调用了堆操作!!!!!!
	//api_InitAllocBuf();
	
	api_InitCPU();
	
	InitBoard();
	
	#if( SEL_DEBUG_VERSION == YES )
	TestCode();
	#endif
	
	CheckImportantPara();
	
  	//允许中断  在进主循环之前才打开中断
	ENABLE_CPU_INT;
	
    //主任务入口
	MainTask();
}


