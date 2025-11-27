//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define MULTI900_50 			(FROZEN_CYCLE_s * 50)  // FROZEN_CYCLE_s*50
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
struct rt_semaphore proc_flag_15min_semaphore;                         	// 15min计算信号量
rt_sem_t            proc_flag_15min_sem = &proc_flag_15min_semaphore;  	// 15min计算信号量

date_time_s 		time_15min;               							//时间
int         		Count_features_002s = 0;  							//存15min数据计数，每COUNT=0.02s
BYTE 				PhaseNum = 1;										//算法使用的相位总数
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
//功能描述:  获取时间
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void GetTime(date_time_s *time)
{
	TRealTimer sys_time;
	
	if (get_sys_time(&sys_time))
	{
		memcpy(time, &sys_time, sizeof(date_time_s));
	}
}

//--------------------------------------------------
//功能描述:  特征算法线程入口函数 
//         
//参数:		uint16_t WAVELEN  -- 周波点数（128、256）
// 			uint8_t phase     -- 单相 1，三相 3
//			float * U_data    -- 电压
//			float * I_data    -- 电流
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void AlgoThreadEntry(uint16_t WAVELEN, uint8_t phase, float U_data[MAX_PHASE][G_SAMPLE_NUM], float  I_data[MAX_PHASE][G_SAMPLE_NUM])
{
	/************* START 单相 从buffer中读取数据并进行特征值计算**************/
	float U[128] = {0};
	float I[128] = {0};
	BYTE k = 0;

	if ((phase == 1) || (phase == 3))
	{
		PhaseNum = phase;
		for(k=0; k<phase; k++)//3相
		{
			if (WAVELEN == 128) 
			{
				rt_memcpy(&U,U_data[k],sizeof(float)*WAVELEN);
				rt_memcpy(&I,I_data[k],sizeof(float)*WAVELEN);
			} 
			else if (WAVELEN == 256) 
			{
				for (int i = 0; i < 256; i++) 
				{
					switch (i % 2) 
					{
					case 0:
						U[i/2] = U_data[k][i]; 
						I[i/2] = I_data[k][i]; 
						break;
					}
				}
				WAVELEN = 128;
			} 
			else 
			{
				rt_kprintf("unsupported wave length\r\n");
				WAVELEN = 128;
			}

			if (Count_features_002s % 50 == 0)  //计算特征值并等待集齐50个    1s的第一个周波
			{
				if (k==0)
				{
					g_dwSoftWatDog ++ ;//1s自增，用来给外部判断算法运行状态
				} 
				feature[k] = cal_features(U, I, WAVELEN);
			} 
			else 
			{
				feature[k] = sum_features(feature[k], cal_features(U, I, WAVELEN));   //1秒的后49周波  累加到feature，1秒特征平均值
			}

		}//3相 
		if (((Count_features_002s + 1) % 50) == 0)  // 0.02s特征值集齐50个时求平均并存入1s窗口的features结构体     1秒的最后一个周波
		{
			if (Flag_15min == 0)  //如果15分钟计算没在运行，正常存储数据，否则把一秒数据先收进featurestmp。
			{
				for(k=0; k<phase; k++)//3相
				{
					if (ftmpCount)  //如果featurebuffer中有数据，先读该数据
					{
						getfromtmp(&features[k], &featurestmp[k]); //此函数不能完全用拷贝代替
					}
					collect_features(Count_features_002s, &features[k], &feature[k]);    //1秒的特征放在900数组里  Count_features_002s一直在增加，直接在正确的位置放新的数据
				}
				ftmpCount = 0;
			} 
			else //此分支代表正在进行15分钟计算
			{
				if (ftmpCount == 0) //先清除数据
				{
					memset(&featurestmp, 0, sizeof(struct featurestmp_typeDef)*PhaseNum);//清除3相数据
				}
				if (((Count_features_002s + 1) / 50 - 1) < TMPLEN)  //如果计算负荷辨识结果的过程中有卡顿，依然需要缓存数据，最大1分钟
				{
					for(k=0; k<phase; k++)//3相
					{
						collect_featurestmp(Count_features_002s, &featurestmp[k], &feature[k]); //当前分钟
					}
					ftmpCount += 1;
				}
				if (ftmpCount > TMPLEN - 1)
				{
					rt_kprintf("15min算得太慢啦\r\n");
					Count_features_002s = 0;
					Flag_15min          = 0;
				}
			}
		}
			
		Count_features_002s += 1;  //每存入一个周波，计数加1
		// rt_kprintf("tick:%d\r\n",systick);

		if (Flag_15min == 0) 
		{
			GetTime(&time_15min );
		}
		if (Count_features_002s >= MULTI900_50)
		{
			Count_features_002s -= 50;  //如果特征值够900了但冻结时间没到，那么特征值计数-1等着冻结时间
		}

		if(((UsePrintfFlag >> 5) & 0x0001) == 0x0001) //开启了周波输入功能
		{
			//回放时有两种方式，一种是回放点数到达MULTI900_50，另一种是时间到了。
			if (((time_15min.min % FROZEN_CYCLE) == 0) && (time_15min.sec == 0) && (Count_features_002s > 100) && (Flag_15min == 0))  //整15分钟，有数据时开启计算
			{
				Flag_15min = 1;
				rt_sem_release(proc_flag_15min_sem);
				// Count_features_002s=0;//重置features计数
			}
		}
		else
		{
			if (((time_15min.min % FROZEN_CYCLE) == 0) && (time_15min.sec == 0) && (Count_features_002s != 0) && (Flag_15min == 0))  //整15分钟，有数据时开启计算
			{
				Flag_15min = 1;
				rt_sem_release(proc_flag_15min_sem);
				// Count_features_002s=0;//重置features计数
			}
		}
	}
	else 
	{
		rt_kprintf("不是单相、三相\n");
		return;
	}
}

//--------------------------------------------------
//功能描述:  算法线程初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void proc_init(void)
{
	//NILM任务初始化
	NILMParaInit();

	/* 初始化信号量proc_flag_15min_semaphore */
	rt_sem_init(proc_flag_15min_sem, "proc_flag_15min_sem", 0, RT_IPC_FLAG_FIFO);
}

//--------------------------------------------------
//功能描述:  15min负荷识别算法线程入口函数 
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void proc_thread_entry(void)
{
	struct lastfeature_typeDef lastcluster[MAX_PHASE]={0}; 
	while (1)
	{
		rt_sem_take(proc_flag_15min_sem, RT_WAITING_FOREVER);	//等到达15min信号
		rt_kprintf("start 15min processing!\r\n");
		rt_kprintf("%d,%d\r\n",RealTimer.Min,RealTimer.Sec);
		nilm(lastcluster,features, PhaseNum);
		rt_kprintf("%d,%d\r\n",RealTimer.Min,RealTimer.Sec);
    }
}

//--------------------------------------------------
//功能描述:  负荷辨识任务的参数初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  NILMParaInit( void )
{
	N15min = 0;
	tlength = 0;
	FeatureInit();
	Count_features_002s = 0;
	api_ClearSampBuf();
}


