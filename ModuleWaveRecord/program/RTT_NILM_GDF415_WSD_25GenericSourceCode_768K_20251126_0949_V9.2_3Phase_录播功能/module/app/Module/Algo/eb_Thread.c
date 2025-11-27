
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
#if(USE_EBIDF)
//消息大小
#define DATA_MSG_SIZE			(128)
//消息个数
#define MSG_NUMBER				(10)
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//信号量
struct rt_semaphore FlagEbSemaphore;
rt_sem_t FlagEBSem = &FlagEbSemaphore;
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
float u12old[MAX_PHASE][INTER_CYCLE * 12]; //INTER_CYCLE  周波点数
float i12old[MAX_PHASE][INTER_CYCLE * 12];
float u10new[MAX_PHASE][INTER_CYCLE * 10];
float i10new[MAX_PHASE][INTER_CYCLE * 10];
float u10mid[MAX_PHASE][INTER_CYCLE * 10];
float i10mid[MAX_PHASE][INTER_CYCLE * 10];

WORD dbg_LastPoint = 0;//记录上一次处理位置
int EbCount_features_002s=0;
float latestU[MAX_PHASE][CYCLES_POINT_10*INTER_CYCLE]; 		//缓存10周波UA
float latestI[MAX_PHASE][CYCLES_POINT_10*INTER_CYCLE]; 		//缓存10周波IA
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//功能描述:  任务通信初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void EBProcInit(void)
{
	//电动车任务初始化
	EBParaInit();

	//初始化信号量FlagEBSem,10周波传递一次
	rt_sem_init(FlagEBSem, "FlagEBSem", 0, RT_IPC_FLAG_FIFO);
}

//--------------------------------------------------
//功能描述:  NILM任务函数
//         
//参数:      
//         
//返回值:    
//         
//备注:  需要4K空间
//--------------------------------------------------
void EBThreadEntry(void)
{
	SWORD delay_1s = 10;
	BYTE ph = 0;

	while(1)
	{
		if(InitComplete) //握手完成后再识别，确保时间是正确的
		{
			if(rt_sem_take(FlagEBSem, RT_WAITING_NO) == RT_EOK)  //10个周波发一次这个信号量,200ms来一次
			{
				for(ph=0; ph<MAX_PHASE; ph++)
				{
					memcpy((BYTE*)&u10new[ph][0],(BYTE*)&latestU[ph][0],10*INTER_CYCLE*sizeof(float));
					memcpy((BYTE*)&i10new[ph][0],(BYTE*)&latestI[ph][0],10*INTER_CYCLE*sizeof(float));

					CurrentSub((float*)&u12old[ph][0],(float*)&i12old[ph][0],(float*)&u10new[ph][0],(float*)&i10new[ph][0],ph);

					//电压拷贝
					memcpy((BYTE*)&u12old[ph][0],(BYTE*)&u12old[ph][INTER_CYCLE*10],INTER_CYCLE*sizeof(float));
					memcpy((BYTE*)&u12old[ph][INTER_CYCLE*11],(BYTE*)&u10new[ph][0],INTER_CYCLE*sizeof(float));
					memcpy((BYTE*)&u12old[ph][INTER_CYCLE],(BYTE*)&u10mid[ph][0],INTER_CYCLE*10*sizeof(float));
					//电流拷贝
					memcpy((BYTE*)&i12old[ph][0],(BYTE*)&i12old[ph][INTER_CYCLE*10],INTER_CYCLE*sizeof(float));
					memcpy((BYTE*)&i12old[ph][INTER_CYCLE*11],(BYTE*)&i10new[ph][0],INTER_CYCLE*sizeof(float));
					memcpy((BYTE*)&i12old[ph][INTER_CYCLE],(BYTE*)&i10mid[ph][0],INTER_CYCLE*10*sizeof(float));
					//mid拷贝
					memcpy((BYTE*)&u10mid[ph][0],(BYTE*)&u10new[ph][0],INTER_CYCLE*10*sizeof(float));
					memcpy((BYTE*)&i10mid[ph][0],(BYTE*)&i10new[ph][0],INTER_CYCLE*10*sizeof(float));
				}
			}
					
			if(((UsePrintfFlag >> 5) & 0x0001) == 0x0001) //开启了周波输入功能
			{
				if(dbg_LastPoint == 0) //复位从0开始
				{
					dbg_LastPoint = Count_features_002s; //记录上一次处理位置
				}
				if(Count_features_002s == 0) //跨圈后清零
				{
					dbg_LastPoint = 0;
				}
				if((Count_features_002s - dbg_LastPoint) >= 50)
				{
					EBCalculate(); //1s到,如果进来相当于处理了5个10周波数据
					dbg_LastPoint = Count_features_002s; //记录上一次处理位置
				}
			}
			else
			{
				delay_1s -=1;
				if(delay_1s <=0)
				{
					EBCalculate();//1s到,如果进来相当于处理了5个10周波数据
					delay_1s = 10;
				}
			}
		}

		if(((UsePrintfFlag >> 5) & 0x0001) == 0x0001) //开启了周波输入功能
		{
			rt_thread_mdelay(1); //1ms延时
		}
		else
		{
			rt_thread_mdelay(100); //100ms延时
		}
	}

}
//--------------------------------------------------
//功能描述:  计算差分
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  CalDiff(float *Input,BYTE Step,BYTE Len, float *Out)
{
	BYTE i = 0;

	for(i=0; i<(Len-1); i++)
	{
		Out[i] = (Input[i+1] - Input[i]) / Step;
	}
}
//--------------------------------------------------
//功能描述:  数据拷贝
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void EBDataCopy(float* U_data, float* I_data)
{	
	BYTE i = 0;

	for(i=0; i<MAX_PHASE; i++)
	{
		memcpy((BYTE*)&latestU[i][(EbCount_features_002s%10)*INTER_CYCLE],(BYTE*)&U_data[INTER_CYCLE*i],INTER_CYCLE*sizeof(float));
		memcpy((BYTE*)&latestI[i][(EbCount_features_002s%10)*INTER_CYCLE],(BYTE*)&I_data[INTER_CYCLE*i],INTER_CYCLE*sizeof(float));
	}

	if((EbCount_features_002s+1)%10 == 0)
	{
		rt_sem_release(FlagEBSem);
	}

	if((EbCount_features_002s+1) == WAVE_LEN)
	{
		for(i=0; i<MAX_PHASE; i++)
		{
			memmove(&features_djc_latest1min[i].P, &features_djc_latest1min[i].P[1], 59*sizeof(float));
			memmove(&features_djc_latest1min[i].D, &features_djc_latest1min[i].D[1], 59*sizeof(float));
			features_djc_latest1min[i].P[59] = feature[i].P; 	//目前是单相程序，因此特征是取的一个相，后续nilm算法是三相后，这里需要改成三相特征。！！！！！！
			features_djc_latest1min[i].D[59] = feature[i].D;	//目前是单相程序，因此特征是取的一个相，后续nilm算法是三相后，这里需要改成三相特征。！！！！！！
		}
	}


	EbCount_features_002s += 1;  //每存入一个周波，计数加1

	if(EbCount_features_002s >= WAVE_LEN)
	{
		EbCount_features_002s = 0;
	}
}

//--------------------------------------------------
//功能描述:  电动车算法参数初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  EBParaInit( void )
{
	api_EBResDeleteIndex(MAX_PHASE,EB_RESULT_NUM);
	EbCount_features_002s = 0;
	memset(&EBCurrentIndex[0],0,sizeof(EBCurrentIndex));
	memset(&features_djc_latest1min[0],0,sizeof(features_djc_latest1min));
}

#endif //#if(USE_EBIDF)