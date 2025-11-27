//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include <rtthread.h>
#include "rtdef.h"
#include "TaskFun.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
#if(PROTOCOL_VERSION == 25)
/* 消息队列控制块 */
rt_mq_t mq_UnkDevFeature = RT_NULL;
rt_mq_t mq_DeviceInfo = RT_NULL;
rt_mq_t mq_WaveData = RT_NULL;
#endif

BYTE freeze_len;												//算法提供识别结果个数，freeze_len最大30
TRealTimer g_Time;
BYTE AbnormFlag[MAX_PHASE];					//异常标志
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------

//--------------------------------------------------
//功能描述:  波形回放前，先初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_VariableInit( void )
{
	//nilm任务初始化
	NILMParaInit();

	//电动车任务初始化
	#if(USE_EBIDF)
	EBParaInit();
	#endif

}
//--------------------------------------------------
//功能描述:  通信初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void ThreadCommInit( void )
{
	#if(USE_NILM)
	#if(PROTOCOL_VERSION == 25)
	CreateMqtoEvent(); //负荷辨识任务创建消息队列，25规范
	#endif

	proc_init();		//负荷辨识任务通信初始化
	#endif //#if(USE_NILM == 1)

	#if(USE_EBIDF)
	EBProcInit();		//电动车任务通信初始化
	#endif

	#if(USE_RECORDWAVE)
	//创建波形消息队列
	mq_WaveData = rt_mq_create("mq_WaveData",			//队列名称
		sizeof(TWaveData),		//消息大小
		WAVE_QUEUE_SIZE, 					//消息个数
		RT_IPC_FLAG_FIFO);				//fifo
	if(mq_WaveData == RT_NULL)
	{
		rt_kprintf("mq_WaveData create failed\n");
		api_WriteSysUNMsg(SYSERR_CRTEVENTMSG_FALSE);
	}	
	#endif

}

#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  设置冻结周期
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE set_freeze_period(WORD period)
{
	//算法需要根据入参period获知当前冻结周期
    algo_set_frozen_period(period);//返回0成功 1失败
	//getperiod(period); //算法商自行判断入参合法性，以及获取周期后的处理逻辑
	return 1;
}
#endif //#if(PROTOCOL_VERSION == 25)

//--------------------------------------------------
//功能描述:  //算法提供算法鉴权状态函数
//         
//参数:      byStatus = 0 代表未授权，byStatus = 1代表授权
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE get_license_status( void ) 
{
	BYTE byStatus = 0;
	//如果算法有安全考虑，请提供算法鉴权状态获取函数

	return byStatus;
}
//--------------------------------------------------
//功能描述:  负荷辨识算法处理模块
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void NILMAlgorithm ( void* para )
{
	proc_thread_entry();	//正常负荷识别
}

#if(USE_EBIDF)
//--------------------------------------------------
//功能描述:  电动车算法处理模块
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void EBAlgorithm ( void* para )
{
	EBThreadEntry();		//电动车负荷识别
}
#endif

#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  添加309A或者309B事件信息 算法提供真实数据，负责逻辑实现
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_AdditionInfo( WORD OI )
{
	BYTE i = 0;
	BYTE bMaxSC = 0;									//最大发送消息个数
	rt_err_t Result = RT_EOK;
	BYTE msg_FeatureTest[FEATRUE_MSG_SIZE];				//存放未知设备事件消息的消息池
	TDeviceInfoPool msg_DevInofTest;					//存放特定设备事件消息的消息池，消息池包含MSG_NUMBER个消息框，每个消息框包含index，类别，功率

	//初始化
	memset((BYTE*)&msg_FeatureTest[0],0xFF,FEATRUE_MSG_SIZE);
	memset((BYTE*)&msg_DevInofTest.bDevCount,0xFF,sizeof(TDeviceInfoPool));

	if(OI == 0x309A)
	{
		if(mq_UnkDevFeature != RT_NULL)
		{
			// *** bMaxSC = save_feature_record();  //需要算法提供接口，获取最大发送消息个数,最大支持一次性传递3条消息
            bMaxSC = get_feature_event_num();

			//发送消息
			for( i = 0; i < bMaxSC; i++)
			{
				// *** get_feature_record_by_idx(msg_FeatureTest, i);  //需要算法提供接口，获取消息内容
                
                get_feature_event(msg_FeatureTest);
                
				Result = api_QueueSend(mq_UnkDevFeature, (BYTE*)&msg_FeatureTest[0], FEATRUE_MSG_SIZE,0);//发送普通消息
				if(Result != RT_EOK)
				{
					rt_kprintf("send msg error!\n");
				}
			}

		}
		else
		{
			;
		}
	}
	else if(OI == 0x309B)
	{
		if(mq_DeviceInfo != RT_NULL)
		{
			// *** bMaxSC = get_pool_len();   //需要算法提供接口，获取最大发送消息个数
            bMaxSC = get_device_event_num();
			//发送消息
			for( i = 0; i < bMaxSC; i++)
			{
				memset((BYTE*)&msg_DevInofTest.bDevCount,0xFF,sizeof(TDeviceInfoPool));
				// ***get_app_events_by_idx(&msg_DevInofTest, i); //需要算法提供接口，获取消息内容
                
                get_device_event(&msg_DevInofTest, i);

				Result = api_QueueSend(mq_DeviceInfo, (BYTE*)&msg_DevInofTest.bDevCount,sizeof(TDeviceInfoPool),0);//发送普通消息
				if(Result != RT_EOK)
				{
					rt_kprintf("send msg error!\n");
				}
			}
		}
		else
		{
			;
		}
	}
	else
	{
		;
	}
}
#endif //#if(PROTOCOL_VERSION == 25)

//--------------------------------------------------
//功能描述:  获取数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  这个函数会根据设置的冻结周期调取一次，提供所需数据,25规范支持设置，24规范默认15分钟。
//--------------------------------------------------
void api_SaveFreezeData( void )
{
	BYTE freeze_len = 0;
	memset(tApplFrzData, 0xFF, sizeof(T_ApplFrzData)*NILM_EQUIPMENT_MAX_NO);

	get_frozen(&freeze_len, tApplFrzData);
}

//--------------------------------------------------
//功能描述:  /*获取电压电流数据*/
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE api_GetData(void)
{
	void *smp_p = NULL;
	BYTE byRes = 0;

	if(api_GetSmpPoints(&smp_p))
	{
		get_voltage(smp_p,&V_data[0][0]);		//计算电压
		get_current(smp_p,&I_data[0][0]);		//计算电流
		SaveVIP();								//不可屏蔽
		byRes = 1;

		//存储录播数据
		#if(USE_RECORDWAVE)
		api_QueueSend(mq_WaveData, smp_p, sizeof(WaveDataTmp[0]),0);
		//实现一个消息队列传递功能，把smp_p起始到便宜128点的所有电压电流原始周波传递到存储任务
		#endif
	}
	else
	{
		byRes = 0;
	}
	
	return byRes;
}

//--------------------------------------------------
//功能描述:  获取周波数据任务
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  GetDataTask( void* para )
{
	BYTE byRes = 0;
	TRealTimer ttime;

	for(;;)
	{
		byRes = api_GetData();

		if (byRes == 1)
		{
			if(InitComplete) //握手完成后再拷贝数据和释放信号，否则提前释放，没有接收信号容易出问题
			{
				//计算负荷辨识的特征
				#if(USE_NILM)
				AlgoThreadEntry(G_SAMPLE_NUM, MAX_PHASE, V_data, I_data);
				#endif

				//计算电动车识别的特征
				#if(USE_EBIDF)
				EBDataCopy(&V_data[0][0], &I_data[0][0]);
				#endif
			}
		}

		else
		{
			get_sys_time(&ttime);
			if(memcmp((BYTE*)&g_Time,(BYTE*)&ttime,sizeof(TRealTimer)) != 0)
			{
				memcpy((BYTE*)&g_Time,(BYTE*)&ttime,sizeof(TRealTimer));
				g_dwSoftWatDog++;
			}
		}

		rt_thread_mdelay(1);
	}
}

//--------------------------------------------------
//功能描述:  存储冻结的任务
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  FreezeEventTask( void* para )
{
	for(;;)
	{
		if(SaveFreeze == 0xAABB) //根据设置的冻结周期产生
		{
			api_SaveFreezeData();//算法获取数据
			#if(PROTOCOL_VERSION == 25)
			api_AdditionInfo(0x309B);
			#endif
			SaveFreeze = 0x0000;
			GetFreeze = 0xAABB;
		}
		
		#if(PROTOCOL_VERSION == 25)
		if(SaveEvent == 0xAABB) //15分钟产生一次
		{
			api_AdditionInfo(0x309A); //算法发送消息队列
			SaveEvent = 0x0000;
		}
		#endif
		rt_thread_mdelay(1);
	}
}


//--------------------------------------------------
//功能描述:  数据处理接口
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void vCreateTasktoGetData(void)
{
	int xReturned;
	int xHandle = 0;

	/* Create the task, storing the handle. */
	xReturned = xTaskCreate(
					GetDataTask,       /* Function that implements the task. */
					"getdata",          /* Text name for the task. */
					1024*40,      /* Stack size in words, not bytes. */
					NULL,    /* Parameter passed into the task. */
					12,/* Priority at which the task is created. */
					&xHandle );      /* Used to pass out the created task's handle. */


	if( xReturned == 0 )
	{

	}
}

//--------------------------------------------------
//功能描述:  负荷辨识算法处理接口
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void vCreateTasktoCalcNILM(void)
{
    int xReturned;
    int xHandle = 0;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    NILMAlgorithm,  /* Function that implements the task. */
                    "nilmalg",      /* Text name for the task. */
                    1024*200,      	/* Stack size in words, not bytes. */
                    NULL,    		/* Parameter passed into the task. */
                    15,				/* Priority at which the task is created. */
                    &xHandle );      /* Used to pass out the created task's handle. */

    if( xReturned == 0 )
    {

    }
}

#if(USE_EBIDF)
//--------------------------------------------------
//功能描述:  电动车算法处理接口
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void vCreateTasktoCalcEB(void)
{
    int xReturned;
    int xHandle = 0;
    
    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    EBAlgorithm,       /* Function that implements the task. */
                    "ebalg",          /* Text name for the task. */
                    1024*10,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    17,/* Priority at which the task is created. */
                    &xHandle );      /* Used to pass out the created task's handle. */

    if( xReturned == 0 )
    {

    }
}
#endif //#if(USE_EBIDF)
//--------------------------------------------------
//功能描述:  录波
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void vCreateTasktoSaveWaveData(void)
{
	int xReturned;
	int xHandle = 0;

	/* Create the task, storing the handle. */
	xReturned = xTaskCreate(
					RecordWaveDataTask,/* Function that implements the task. */
					"RecordWaveData",       /* Text name for the task. */
					1024*5,      	/* Stack size in words, not bytes. */
					NULL,    		/* Parameter passed into the task. */
					22,				/* Priority at which the task is created. */
					&xHandle );      /* Used to pass out the created task's handle. */

	if( xReturned == 0 )
	{

	}
}
//--------------------------------------------------
//功能描述:  计算冻结结果接口
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  vCreateTasktoSaveFreezeEvent( void )
{
	int xReturned;
	int xHandle = 0;

	/* Create the task, storing the handle. */
	xReturned = xTaskCreate(
					FreezeEventTask,/* Function that implements the task. */
					"FrzEvt",       /* Text name for the task. */
					1024*10,      	/* Stack size in words, not bytes. */
					NULL,    		/* Parameter passed into the task. */
					20,				/* Priority at which the task is created. */
					&xHandle );      /* Used to pass out the created task's handle. */

	if( xReturned == 0 )
	{

	}
}
#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  创建消息队列和信号量
//         
//参数:      无
//         
//返回值:    无
//         
//备注:  
//--------------------------------------------------
void CreateMqtoEvent ( void )
{
	//创建未知设备事件的特征消息队列
	mq_UnkDevFeature = rt_mq_create("mq_UnkDevFeature",		//队列名称
								FEATRUE_MSG_SIZE,			//消息大小
								MSG_NUMBER, 				//消息个数
								RT_IPC_FLAG_PRIO);			//根据优先级分配
	if(mq_UnkDevFeature == RT_NULL)
	{
		rt_kprintf("mq_UnkDevFeature create failed\n");
		api_WriteSysUNMsg(SYSERR_CRTFEARMSG_FALSE);

	}

	//创建指定设备事件的设备信息消息队列
	mq_DeviceInfo = rt_mq_create("mq_DeviceInfo",			//队列名称
							sizeof(TDeviceInfoPool),		//消息大小
							MSG_NUMBER, 					//消息个数
							RT_IPC_FLAG_PRIO);				//根据优先级分配
	if(mq_DeviceInfo == RT_NULL)
	{
		rt_kprintf("mq_DeviceInfo create failed\n");
		api_WriteSysUNMsg(SYSERR_CRTEVENTMSG_FALSE);
	}	
}
#endif //#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  测试用，填充数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_TestComplementData( void )
{
	BYTE i;
	static BYTE num = 1;
	WORD Arr[] = {0x0100,0x0200,0x0301,0x0402,0x0500,0x0605,0x0700,0x0804,0x0900,0x0A01,0xA101,0xB100,0x0C03,0xA204,0xB10};
	TRealTimer stime;
	DWORD seed = 0;
	DWORD a = 1103515245;	// 可以选择其他值以提高随机性
	DWORD c = 12345;		// 可以选择其他值以提高随机性
	DWORD m = 16;			// 由于需要生成0到15的随机数，所以模数设为16

	api_ReadMeterTime(&stime);
	seed = stime.Sec;
	seed = ((a * seed + c) % m)%16;

	memset(tApplFrzData, 0xFF, sizeof(T_ApplFrzData)*NILM_EQUIPMENT_MAX_NO);
// 712021000705
//  68 4F 00 C3 26 03 11 11 11 11 11 11 A0 4B 1B 85 03 00 50 02 02 00 03 00 20 23 02 00 00 20 21 02 00 00 48 00 04 00 01 01 06 00 00 00 01 1C 07 E7 03 06 0E 0F 00 01 01 02 06 12 01 00 06 00 00 03 E8 05 00 00 27 10 11 00 11 00 11 00 00 00 11 82 16 
	for ( i = 0; i < num; i++)
	{
		tApplFrzData[i].ApplClassID = Arr[i];
		tApplFrzData[i].ApplFrzCylAvgP = 1000+ stime.Day * (i+stime.Sec) * stime.Hour + stime.Min;
		tApplFrzData[i].ApplFrzCylP = 100 + stime.Day * (i + 1 + stime.Sec) * stime.Hour + stime.Min;
		tApplFrzData[i].StartTime = seed;
		tApplFrzData[i].EndTime = 15-seed;
		tApplFrzData[i].SECount = seed/2;
	}
	num++;
	if(num > 30)
	{
		num = 1;
	}
}

//--------------------------------------------------
//功能描述:  创建负荷辨识算法任务
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_CreateNILMTask(void)
{
	//任务通信初始化
	ThreadCommInit();

	//创建数据获取任务
	vCreateTasktoGetData();

	#if(USE_NILM)
	//创建负荷辨识算法处理的任务
	vCreateTasktoCalcNILM();
	#endif

	#if(USE_EBIDF)
	//创建电动车处理任务
	vCreateTasktoCalcEB();
	#endif

	//计算冻结和发送消息队列任务
	vCreateTasktoSaveFreezeEvent();

	
	#if(USE_RECORDWAVE)
	vCreateTasktoSaveWaveData();
	//封装一个写数据函数，任务优先级定为22,任务空间考虑三相大小，用MAX_PHASE控制，接收到消息队列后就开始存储，理论每个周波存储一次
	//掉电检测功能也可以放在此任务，任务最后用rt_thread_mdelay(1)来切换
	#endif
}
