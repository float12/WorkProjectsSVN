//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include <stdlib.h>
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define PROVINCE_CODE  	135//福建
#define DERTA_P			30 //有功功率差值
#define DERTA_D			50 //畸变功率差值
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE							N15min   = 0;
WORD							tlength  = 0;  //本次的数据长度，单位s
BYTE							Flag_15min = 0;  //存够15min数据标志,'1'有效
extern float					maxstd;
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
BYTE							AddNum = 0;
static short					Pinit    = 0;  //整段数据的初始有功，作为背景噪声的标准
static short					Pthres   = 50;
static short					Pthres_allclose = 50;  //所有设备关停阈值
static BYTE						dpzrflag        = 0;
#if(MAX_PHASE == 3)
static BYTE 					devidnum[MAX_PHASE]= {1,1,1};						//从开机以来，有多少个设备
#elif(MAX_PHASE == 1)
static BYTE 					devidnum[MAX_PHASE]= {1};							//从开机以来，有多少个设备
#endif
static struct unknownR_typeDef 	unknownR[MAX_PHASE][4]     = {0};	//电热类使用
int								last_P          = 0;
int								last_Pmax  = 0;  // cluster0的最大值可能在上一个冻结的末尾出现
static struct devID_typeDef 	DevidRecord[MAX_PHASE]                   = {0};//设备从开启到现在识别的电器记录
static BYTE						switch_count            = 0;  //记录本冻结周期是否有反复启停类
static BYTE						expectone               = 0;
static short					lastclusteruselastpoint = 0;
static BYTE						totalclusters           = 0;
WORD							cur_len         = 0;
BYTE							type1           = 0;
static BYTE						lastresultis8   = 0; //洗衣机顺延的变量
float							Pusedall_real   = 0;
static volatile BYTE 			frozen_is_ready = 0;//冻结计算完成标志
static BYTE 					frozen_period = 15;//冻结周期
static frozen_typeDef 			frozen[MAX_PHASE]={0};//最近一次冻结   三相冻结
static frozen_typeDef 			frozen_combine={0};//合并冻结
static frozen_typeDef 			frozen_history[4]={0};//4次历史冻结
//输出冻结数据
static frozen_typeDef 			frozen_output={0};
//未知设备特征
static int8_t   				uk_device_praw_output[256] = {0};
static BYTE  					uk_device_index = 0;
static BYTE 					phase_index = 0; //此函数用于记录相别，与unknownR、devidnum、DevidRecord有关系

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
struct lastfeature_typeDef nilm_15min(struct features_typeDef    feats,struct lastfeature_typeDef lastcluster,BYTE Phase);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
/* 
 * 设置冻结周期
 * 参数：
 *      uint8_t period   --  冻结周期（15、30、45、60）
 * 返回值：
 *      0  --  成功
 *      1  --  失败
 */
uint8_t algo_set_frozen_period(uint8_t period)
{
	if (period == 15 || period == 30 || period == 45 || period == 60)
	{
		frozen_period = period;
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("algo_set_frozen_period: %d\r\n", frozen_period);
		}
		#endif
		return 0;
	}
	else
	{
		return 1;
	}
}
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
static void FrozenUpdata15Min(void)
{
	memcpy(&frozen_history[0], &frozen_history[1], sizeof(frozen_typeDef));
	memcpy(&frozen_history[1], &frozen_history[2], sizeof(frozen_typeDef));
	memcpy(&frozen_history[2], &frozen_history[3], sizeof(frozen_typeDef));
	memcpy(&frozen_history[3], &frozen_combine, sizeof(frozen_typeDef));

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("FrozenUpdata15Min\r\n");
	}
	#endif
}
//合并两个相邻时间的冻结，合并后放在frozen2   (从最新时间往前合并！！)
static void merge_2_frozen(frozen_typeDef *frozen1, frozen_typeDef *frozen2)
{
    frozen_typeDef frozen_merge = {0};
    memcpy(&frozen_merge, frozen1, sizeof(frozen_typeDef));
    
    //遍历冻结2
    for(int i=0; i<frozen2->num; i++)
    {
        uint8_t find_same_device_index = 0xFF;
        
        for(int j=0; j<frozen1->num; j++)
        {
            //电器类型相同且 1结束时间FF，2开始时间有FF
            if (frozen2->sindev[i].device == frozen1->sindev[j].device)
            {
                if (frozen1->sindev[i].end == 0xFF && frozen2->sindev[i].start == 0xFF)
                find_same_device_index = j;
                break;
            }
        }
        
        if (find_same_device_index != 0xFF)
        {
            //找到相同的电器
            frozen_merge.sindev[find_same_device_index].pused += frozen2->sindev[i].pused;//电量累加
            frozen_merge.sindev[find_same_device_index].p += frozen2->sindev[i].p;//平均功率  (暂时就直接平均吧，加权不大好算)
            frozen_merge.sindev[find_same_device_index].p /= 2;
            frozen_merge.sindev[find_same_device_index].end = frozen2->sindev[i].end;//启动时间不变，停止时间+15
            if(frozen_merge.sindev[find_same_device_index].end != 0xFF)
            {
                frozen_merge.sindev[find_same_device_index].end += 15;
            }
            frozen_merge.sindev[find_same_device_index].switchtimes += frozen2->sindev[i].switchtimes;//合并启停数相加
        }
        else
        {
            //未找到相同电器
            memcpy(&frozen_merge.sindev[frozen_merge.num], &frozen2->sindev[i], sizeof(sindev_typeDef));
            //启停时间增加15
            if (frozen_merge.sindev[frozen_merge.num].start != 0xFF)
            {
                frozen_merge.sindev[frozen_merge.num].start += 15;
            }
            if (frozen_merge.sindev[frozen_merge.num].end != 0xFF)
            {
                frozen_merge.sindev[frozen_merge.num].end += 15;
            }
            frozen_merge.num++;
        }
    }  
    memcpy(frozen2, &frozen_merge, sizeof(frozen_typeDef));
}
/* 
 * 获取冻结数据 
 * 参数：
 *      uint8_t * dev_num            -- 设备个数
 *      T_ApplFrzData * fro_data     -- 冻结
 */
void get_frozen(uint8_t * dev_num, T_ApplFrzData  fro_data[])
{
	TRealTimer Time;

	//等待冻结计算完成
	while (frozen_is_ready == 0)
	{
		rt_thread_mdelay(100);

		get_sys_time(&Time);
		if(Time.Sec > ((MAX_PHASE)+1)) //单相理论计算最大时间是1s，三相*3，多1秒的冗余
		{
			return;
		}
	}


	//复制最近一条冻结
	memcpy(&frozen_output, &frozen_history[3], sizeof(frozen_typeDef));
	switch(frozen_period)
	{
		case 15://15分钟直接输出
		{
			break;
		}
		case 30:
		{
			//合并2 3
			merge_2_frozen(&frozen_history[2], &frozen_output);
			break;
		}
		case 45:
		{
			//合并2 3
			merge_2_frozen(&frozen_history[2], &frozen_output);
			//合并1 23
			merge_2_frozen(&frozen_history[1], &frozen_output);
			break;
		}
		case 60:
		{
			//合并2 3
			merge_2_frozen(&frozen_history[2], &frozen_output);
			//合并1 23
			merge_2_frozen(&frozen_history[1], &frozen_output);
			//合并0 123
			merge_2_frozen(&frozen_history[0], &frozen_output);
			break;
		}
	}

	//删除未知类
	for(int i=0; i<frozen_output.num; i++)
	{
		uint16_t temp = frozen_output.sindev[i].device & 0xFF00;
		if (temp == 0xB100 || temp == 0xB200  || temp == 0xE100 || temp == 0xE200 || temp == 0xF100 || temp == 0xF200)
		{
			memcpy(&frozen_output.sindev[i], &frozen_output.sindev[i+1], (frozen_output.num-i-1)*sizeof(sindev_typeDef));
			frozen_output.num--;
			i--;
		}
	}

			

	//输出数据
	//识别到的设备个数
	*dev_num = frozen_output.num;
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("get_frozen: * dev_num:%d\r\n", * dev_num);
	}
	#endif

	for(int i=0; i<frozen_output.num; i++)
	{
		fro_data[i].ApplClassID = frozen_output.sindev[i].device;//设备类号
		fro_data[i].ApplFrzCylP = frozen_output.sindev[i].pused;//冻结周期内有功用电量
		fro_data[i].ApplFrzCylAvgP = frozen_output.sindev[i].p;//冻结周期内设备运行平均功率
		fro_data[i].StartTime = frozen_output.sindev[i].start;//启动时间
		fro_data[i].EndTime = frozen_output.sindev[i].end;//停止时间
		fro_data[i].SECount = frozen_output.sindev[i].switchtimes;//合并启停数
		
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("get_frozen: i:%d, device:0x%x, pused:%d, p:%d, start:%d, end:%d, switchtimes:%d\r\n", 
					i, frozen_output.sindev[i].device, frozen_output.sindev[i].pused, frozen_output.sindev[i].p, frozen_output.sindev[i].start, frozen_output.sindev[i].end, frozen_output.sindev[i].switchtimes);
		}
		#endif
	}
	frozen_is_ready = 0;
}

#if(PROTOCOL_VERSION == 25)
/* 
 * 获取未知设备特征个数
 */
uint8_t get_feature_event_num(void)
{
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("get_feature_event_num:1\r\n");
	}
	#endif
	return 1;
}
/* 
 * 获取未知设备特征 
 */
void get_feature_event(uint8_t * buf)
{
	memcpy(buf, uk_device_praw_output, 256);

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("get_feature_event:\r\n");
		for (int i=0; i<256; i++)
		{
			rt_kprintf("%02x ", buf[i]);
		}
		rt_kprintf("\r\n");
	}
	#endif
}
/* 
 * 获取已知设备个数
 */
uint8_t get_device_event_num(void)
{
	uint8_t device_event_num = 1;
	if (frozen_output.num > 10)
	{
		device_event_num = 2;
	}
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{	
		rt_kprintf("get_device_event_num:%d,  device_event_num:%d\r\n", frozen_output.num, device_event_num);
	}
	#endif
	return device_event_num;
}
/* 
 * 获取已知设备
 */
void get_device_event(TDeviceInfoPool * device, uint8_t index)
{
	device->bDevCount = frozen_output.num-10*index;
	if (device->bDevCount > 10)
	{
		device->bDevCount = 10;//设备个数
	}

	rt_kprintf("get_device_event:%d, index:%d\r\n", device->bDevCount, index);

	for (int i=0; i<device->bDevCount; i++)
	{
		device->Message[i].wDevID = frozen_output.sindev[i].device;//设备类别
		device->Message[i].bDevStatus = 0;                          //设备状态不知道是什么
		device->Message[i].dwDevAvgPow = frozen_output.sindev[i].p; //平均功率
		device->Message[i].tDevTime = frozen_output.sindev[i].start*60; //事件发生时间 s
		
		rt_kprintf("wDevID:%d, dwDevAvgPow:%d, tDevTime:%d\r\n", device->Message[i].wDevID, device->Message[i].dwDevAvgPow, device->Message[i].tDevTime);
	}
}
#endif //#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  合并冻结
//         
//参数:      Phase 入参是相位数
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
static void CombineFreeze(void)
{
	BYTE i,j;

	memset(&frozen_combine,0,sizeof(frozen_typeDef));
	memcpy(&frozen_combine.time,&frozen[0].time,sizeof(date_time_s));//A相冻结时间

	for (i=0; i<PhaseNum; i++)
	{
		for (j=0; j<frozen[i].num; j++)
		{
			if (frozen_combine.num<MAXDEVNUM)
			{
				memcpy(&frozen_combine.sindev[frozen_combine.num],&frozen[i].sindev[j],sizeof(sindev_typeDef));
				frozen_combine.num ++;
			}
		}
	}

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		//打印冻结结果
		rt_kprintf("frozen_combine: time:%d-%d-%d %d:%d:%d\n", frozen_combine.time.year, frozen_combine.time.mon, frozen_combine.time.day, frozen_combine.time.hour, frozen_combine.time.min, frozen_combine.time.sec);
		for (i = 0; i < frozen_combine.num; i++)
		{
			rt_kprintf("total:device:%d-P:%d-Pused:%d\r\n", frozen_combine.sindev[i].device, frozen_combine.sindev[i].p, frozen_combine.sindev[i].pused);
		}
	}
	#endif
}


/*****************************************************************************
 函 数 名  : nilm
 功能描述  : 识别主程序，持续收集数据
 输入参数  : features
 输出参数  :
 返 回 值  :
*****************************************************************************/
void nilm(struct lastfeature_typeDef *lastcluster, struct features_typeDef* feats, BYTE Phase)
{
	BYTE k = 0;

	if (Flag_15min == 1)  //当Flag_15min为1时，说明features_tmp存满15min或者数据停止了，开始计算
	{
		frozen_is_ready = 0;
		tlength             = Count_features_002s / 50;
		Count_features_002s = 0;

		//未知设备清0
		memset(uk_device_praw_output, 0, 256);
		uk_device_index = 0;
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{		
			rt_kprintf(" h:%d   m:%d   s:%d\r\n", time_15min.hour, time_15min.min, time_15min.sec);
		}
		#endif
		for(k=0; k<Phase; k++)
		{
			phase_index = k;
			lastcluster[k] = nilm_15min(feats[k], lastcluster[k], k);  //负荷识别
		}
		CombineFreeze();		//合并冻结

		FrozenUpdata15Min(); 		//生成冻结
		frozen_is_ready = 1;

		memset(feats, 0, sizeof(struct features_typeDef)*Phase);  //清除features，等待下一轮
		if (((time_15min.min % FROZEN_CYCLE) == 0) && (time_15min.sec == 0))  //如果当前数据长度为900，说明接下来还有数据，数据段标记+1
		{
			//一定会进入这里 N15min只有第一次为0，后面永远不为0
			if (N15min < 255)
			{
				N15min += 1;
			}
			else
			{
				N15min = 1;
			}
		} 
		else 
		{
			N15min = 0;
		}

		time_15min.min = 199;
		GetTime(&time_15min);

		if (((time_15min.min % FROZEN_CYCLE) == 0) && (time_15min.sec == 0))  //等待整15分钟这秒过去
		{
			rt_thread_mdelay(1000);//设定的1s
		}
		Flag_15min = 0;  //清除标志位
        return ;
	} 
	else
	{
        return ;
	}
}

/*****************************************************************************
 函 数 名  : nilm_15min
 功能描述  : 算法程序，每15min运行一次
 输入参数  : feats
 输出参数  : Devices,P,pused
*****************************************************************************/
struct lastfeature_typeDef nilm_15min(struct features_typeDef    feats,struct lastfeature_typeDef lastcluster,BYTE Phase)
{
	Pthres                           = 50;
	expectone                        = 0;
	lastclusteruselastpoint          = 0;
	maxstd                           = 0;
	uint8_t n_cluster                = 0;  //类数目
	uint8_t device                   = 0;
	Pusedall_real                    = 0;
	AddNum                           = 0;
	struct steady_typeDef   diff     = {0};
	struct cluster_typeDef* clusters = NULL;
	BYTE j = 0,i = 0,k = 0,m = 0;
	#if(USE_EBIDF)
	BYTE bEb = 0;
	WORD wStep = 0;
	#endif

	#if(USE_PRINTF == 1)
	TRealTimer	Time;
	#endif

	if (unknownR[Phase][0].P == 0)
	{
        memset(&unknownR[Phase][0], 0, 4 * sizeof(struct unknownR_typeDef));
	}

	//根据有功功率对15min数据进行聚类
	clusters = cluster(feats.i50, feats.P, feats.Q, feats.Qfft, feats.D, feats.Praw,
						feats.Pmax, &Pusedall_real, &n_cluster);

	//没有类，直接退出
	if (n_cluster == 0)
	{
        frozen[Phase].num = 0;
        memcpy(&frozen[Phase].time, &time_15min, sizeof(date_time_s));
        return lastcluster;
	}
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT 9\r\n");
	}
	#endif
	//标记反复启停类，并且把每一个反复启停类配对
	switch_label(clusters, n_cluster, feats.P);  
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT 10\r\n");
	}
	#endif

	totalclusters                 = n_cluster;
	struct result_typeDef* result = NULL;
	result                        = (struct result_typeDef*)malloc(n_cluster *
											sizeof(struct result_typeDef));  //按类的个数给result分配内存
	if (!result) 
	{
        frozen[Phase].num = 0;
        memcpy(&frozen[Phase].time, &time_15min, sizeof(date_time_s));
		free(clusters);
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("result out of memory\n");
		}
		#endif
		return lastcluster;
	}
	memset(result, 0, n_cluster * sizeof(struct result_typeDef));
	
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("ncluster: %d\r\n", n_cluster);
	}
	#endif

	if (!N15min)  //如果是第一个15min，确定有功功率的基线
	{
		
	} 
	//当前处理的是继承这块的逻辑，现在处理第一个类别,从上一个周期继承而来，先识别电热类，再识别其他类别
	else  //如果不是第一个15min，继承上一个15min，并识别第一类
	{
		//洗衣机很难识别到关，所以要先删除掉洗衣机
		if (inarr_u8(lastcluster.lastresult.device, 8, RESULT_NUM) >=
			0)  //洗衣机不继承 从lastresult里删掉洗衣机
		{
			remove_device(&lastcluster.lastresult, 8, 0);
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("removed xyj");
			}
			#endif
		}
		memcpy(&result[0], &lastcluster.lastresult, sizeof(struct result_typeDef));  //继承上一个15min最后一个非switch类
		#if(USE_EBIDF)
		for(i = 0; i<RESULT_NUM; i++)
		{
			if(lastcluster.lastresult.device[i] == 0)
			{
				break;
			}
			else
			{
				lastcluster.lastresult.Ident[i] = i;
				AddNum ++;
			}
		}
		#endif//#if(USE_EBIDF)
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("POINT11\r\n");
		}
		#endif

		//为了识别电热大类
		for (uint8_t j = 0; j < RESULT_NUM; j++)  //根据当前类的时间重新计算功耗
		{
			if (result[0].device[j]) 
			{
				result[0].Pused[j] = result[0].P[j] * clusters[0].loc;
				if (result[0].device[j] >= 22 && result[0].device[j] <= 25)  //根据运行时间区分热水器等   电热类
				{
                    unknownR[Phase][result[0].device[j] - 22].during += clusters[0].loc;
					recunknownR(result, 1, result[0].device[j] - 22);   //识别热水器 电暖气 电饭煲，判断功率和时间
				}
			} 
			else
				break;
		}
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("POINT12\r\n");
		}
		#endif
		//如果当前数据第一类第一点与上15min的最后一类功率不接近，或者当前为缓变类且功率变化超过400，或没有开变频空调，重新判断
		if ((feats.P[clusters[0].t[0]] - lastcluster.Pmax) < -100 ||
			(feats.P[clusters[0].t[0]] - lastcluster.Pmax) > 100 ||
			((clusters[0].flag & 0x01) && ((feats.P[clusters[0].t[clusters[0].loc]] - lastcluster.Pmax) > 400 ||
				(inarr_u8(result[0].device, 2, RESULT_NUM) < 0)))) 
		{
            if (clusters[0].pair)  //如果是反复启停启类，那它自己与pair比较，用它的pair跟lastcluster进行比较
            {
                diff   = diff_features(clusters[0], clusters[clusters[0].pair], feats, result[0]);
                device = recdevice(diff, 2, result, 0, clusters, feats, n_cluster);  // 反复启停类识别 //误判：脉冲充电法进入此分支
                add_device(result, device, 0, diff.P, diff.Praw, diff.Pused,
                           clusters[0].loc);  // 将识别结果添加至result
                if (clusters[0].pair ==
                    1)  // 如果下一类就是它的pair，那拿下一类跟lastcluster比较，且把增删的设备从0类里也增删
                {
                    memcpy(&result[1], &lastcluster.lastresult,
                           sizeof(struct result_typeDef));  //继承上一个15min最后一个非switch类
                    for (uint8_t j = 0; j < RESULT_NUM; j++)     //根据当前类的时间重新计算功耗
                    {
                        if (result[1].device[j]) {
                            result[1].Pused[j] = result[1].P[j] * clusters[1].loc;
                            if (result[1].device[j] >= 22 &&
                                result[1].device[j] <= 25)  //根据运行时间区分热水器等
                            {
                                recunknownR(result, 2, result[1].device[j] - 22);
                            }
                        } else
                            break;
                    }
                    if (clusters[1].P > lastcluster.P + Pthres &&
                        clusters[1].Prawsteady > lastcluster.Prawsteady + Pthres)  //有设备开启潜力
                    {
                        diff = diff_lastcluster(clusters[1], lastcluster, feats, 1);
                        if ((clusters[1].P > lastcluster.P + 100) ||
                            ((clusters[1].flag & 0x01) && (diff.Q > 200)) ||
                            (diff.Q > 80 &&
                             diff.Pmax / diff.P >
                                 10))  //判断是否有设备开启：1.功率变化大于100 2.缓变类且无功变化大于200
                                       // 3.Pmax/P>2且无功大于80
                        {
                            if (clusters[1].flag & 0x01)  //缓变类
                            {
                                device = recdevice(diff, 3, result, 1, clusters, feats,
                                                   n_cluster);  // 缓慢变化类识别
                            } else {
                                device = recdevice(diff, 1, result, 1, clusters, feats,
                                                   n_cluster);  // 普通类识别
                            }
                            add_device(result, device, 1, diff.P, diff.Praw, diff.Pused,
                                       clusters[1].loc);  //将识别结果添加至result
                        }
                    } else if ((clusters[1].P < (lastcluster.P - 100)) &&
                               (result[1].device[1] ||
                                clusters[1].P <
                                    300))  //判断是否有设备关闭(如果只剩一个设备且剩余功率大于300，则不关)
                    {
                        diff = diff_lastcluster(clusters[1], lastcluster, feats, 0);
                        device =
                            recdevice(diff, 4, result, 1, clusters, feats, n_cluster);  // 关闭设备识别
                        remove_device(result, device, 1);
                        remove_device(result, device, 0);  //从cluster 0 里也删掉
                    }
                }
            } 
			else if (clusters[0].P > lastcluster.P + Pthres &&
                       clusters[0].Prawsteady > lastcluster.Prawsteady + Pthres)  //有设备开启潜力，反复启停的可能的启
            {
                diff = diff_lastcluster(clusters[0], lastcluster, feats, 1);
                if ((clusters[0].P > lastcluster.P + 100) || ((clusters[0].flag & 0x01) && (diff.Q > 200)) ||
                    (diff.Q > 80 && diff.Pmax / diff.P > 2) ||
                    (diff.P > 50 &&
                     diff.Q > diff.P))  //判断是否有设备开启：1.功率变化大于100 2.缓变类且无功变化大于200
                                        // 3.Pmax/P>2且无功大于80 4.无功大于有功
                {
                    if (clusters[0].flag & 0x01)  //缓变类
                    {
                        device =
                            recdevice(diff, 3, result, 0, clusters, feats, n_cluster);  // 缓慢变化类识别
                    } else {
                        device = recdevice(diff, 1, result, 0, clusters, feats, n_cluster);  // 普通类识别
                    }
                    add_device(result, device, 0, diff.P, diff.Praw, diff.Pused,
                               clusters[0].loc);  //将识别结果添加至result
                }
            } 
			else if ((clusters[0].P < (lastcluster.P - 100)) &&
                       (result[0].device[1] ||
                        clusters[0].P < 300))  //判断是否有设备关闭(如果只剩一个设备且剩余功率大于300，则不关)，反复启停的可能的停
            {
                diff   = diff_lastcluster(clusters[0], lastcluster, feats, 0);
                device = recdevice(diff, 4, result, 0, clusters, feats, n_cluster);  // 关闭设备识别
                remove_device(result, device, 0);
            }
        } 
		else if (clusters[0].flag & 0x01)  //如果是缓变类但不满足其它两个条件，更新变频空调参数
        {
            diff = diff_lastcluster(clusters[0], lastcluster, feats, 1);
            update_device(result, diff, clusters[0].loc, 2, 0, 1);  //更新开着的变频空调参数
        } 
		else if (clusters[0].P - Pinit < Pthres_allclose)  //如果当前类功率相较基线小于50W，认为一切设备关停
        {
            while (result[0].device[0]) {
                remove_device(result, result[0].device[0], 0);
            }
        }
        update_recunknownR(result, clusters, 0, n_cluster);
    }
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
    	rt_kprintf("POINT1\r\n");
	}
	#endif
	//i从1开始，这里处理的是本周期聚类的新的类别
    for (i = 1; i < n_cluster; i++) {
        if (i == 1 && clusters[0].pair == 1 &&
            (result[1].device[0] != 0 ||
             (clusters[i].P - Pinit < 50 &&
              !result[1].device[0])))  //如果是cluster0的pair是1，那么cluster1不重新算了，因为之前已经算过了
            continue; //误判：脉冲充电法会进入此分支
        /***********  START继承上一个类（非反复启停的启）的result  **********/
        k = 1;
        if (!clusters[i - 1].pair)  //找上一个不是反复启停启的类
        {
            k = 1;
            if (i >= 3 && (inarr_u8(result[i - 2].device, 22, RESULT_NUM) < 0) &&
                abs(clusters[i].P - clusters[i - 1].P) > abs(clusters[i].P - clusters[i - 2].P) &&
                (abs(clusters[i].P - clusters[i - 2].P) < 50 ||
                 ((inarr_u8(result[i - 2].device, 1, RESULT_NUM) >= 0) &&
                  (abs(clusters[i].P - clusters[i - 2].P) < 150))) &&
                abs(clusters[i].Q - clusters[i - 2].Q) < 50 && (!clusters[i - 2].pair)) {
                k = 2;
            }
        } 
		else if (i > 1 && !clusters[i - 2].pair &&
                   !(i > 2 && clusters[i].P > 1900 && clusters[i - 1].P > 2500 &&
                     inarr_u8(result[i - 2].device, 5, RESULT_NUM) >= 0)) {
            k = 2;
        } 
		else if (i > 2 && !clusters[i - 3].pair &&
                   !(i > 3 && clusters[i].P > 1900 && clusters[i - 1].P > 2300 &&
                     inarr_u8(result[i - 3].device, 5, RESULT_NUM) >= 0)) {
            k = 3;
        } 
		else if (i > 3 && !clusters[i - 4].pair) {
            k = 4;
        } 
		else if (i > 4 && !clusters[i - 5].pair) {
            k = 5;
        }
        if (i > 0 && !clusters[i].pair)  //如果不是反复启停的启，继承上个不是启的类的result
        {
            memcpy(&result[i], &result[i - k], sizeof(struct result_typeDef));
            for (j = 0; j < RESULT_NUM; j++)  //根据当前类的时间重新计算功耗
            {
                if (result[i].device[j]) {
                    result[i].Pused[j] = result[i].P[j] * clusters[i].loc;
                    if (result[i].device[j] >= 22 && result[i].device[j] <= 25)  //根据运行时间区分热水器等
                    {
                        recunknownR(result, n_cluster, result[i].device[j] - 22);
                    }
                } else {
                    break;
                }
            }
        }
        /***********  END继承上一个类的result  **********/

        /***********   START 差分特征值        *********/
        //反复启停类找其配对类差分
        if (clusters[i].P - Pinit < Pthres_allclose)  //如果当前类功率相较基线小于50W，认为一切设备关停
        {
            while (result[i].device[0]) {
                remove_device(result, result[i].device[0], i);
            }
            continue;
        } 
		else if (clusters[i].pair) //反复启停配对的类别，误判：脉冲充电法会进入此分支
		{
            update_recunknownR(result, clusters, i, n_cluster);
            diff = diff_features(clusters[i], clusters[i + clusters[i].pair], feats, result[i]);
        }

        //缓慢变化类及普通类找其前一个非反复启停类差分，这里判断开启的
        else if (clusters[i].P >= clusters[i - k].Psteady + Pthres &&
                 clusters[i].Prawsteady >= clusters[i - k].Prawsteady + Pthres)  //判断是否有设备开启
        {
            update_recunknownR(result, clusters, i, n_cluster);
            diff = diff_features(clusters[i], clusters[i - k], feats, result[i]);//后减去前面
			#if(USE_EBIDF == 0)
            if ((clusters[i].P >= clusters[i - k].Psteady + 2 * Pthres) ||
                ((clusters[i].flag & 0x01) && (diff.Q > 200)) || (diff.Pmax / diff.P > 2) ||
                (diff.P >= Pthres && diff.Q > diff.P))  // 判断是否具有初步准入条件,最后一条针对电动自行自行车
            {
            } 
			else
                continue;
			#endif //#if(USE_EBIDF == 0)
        } 
		//这里是缓慢变化类及普通类的判断关闭的，前减去后面
		else if (
            clusters[i].P <=
            clusters[i - k].Psteady -
                100)  //判断是否有设备关闭（如果前一类中有洗衣机，那么判断有设备关的条件要提高(待加入)）
        {
            if ((inarr_u8(result[i].device, 2, RESULT_NUM) >= 0 &&
                 (clusters[i].P >= clusters[i - k].Psteady - 200)) ||
                (inarr_u8(result[i].device, 1, RESULT_NUM) >= 0 &&
                 (clusters[i].P >= clusters[i - k].Psteady - 150)) ||
                ((!result[i].device[1]) && (clusters[i].P > 300) &&
                 (!(clusters[i].flag & 0x01) || clusters[i].P >= clusters[i - k].Psteady - 500)) ||
                (dpzrflag && !result[i].device[1] && result[i].device[0] == 1 &&
                 clusters[i].P >
                     800))  //不关设备：变频空调开且变化小于200；定频空调开且变化小于150；仅有最后一个设备开且当前P>300
            {
                diff = diff_features(clusters[i - k], clusters[i], feats, result[i]);
                update_device(result, diff, clusters[i].loc, 2, i, 0);  //更新此次判定功率下降的设备
            } 
			else {
                if ((clusters[i].flag & 0x01) && clusters[i].Psteady - feats.P[clusters[i].t[0]] > 400 &&
                    feats.P[clusters[i - 1].t[clusters[i - 1].loc - 1]] - feats.P[clusters[i].t[0]] >
                        1000 &&
                    !(clusters[i - 1].flag & 0x01) &&
                    result[i].device[0])  //如果关闭后有新缓变设备开启，没有分出单纯关闭类
                {
                    diff = diff_features_2slowly(clusters[i - k], clusters[i], feats, 2);  // 69.csv
                    device = recdevice(diff, 4, result, i, clusters, feats, n_cluster);  // 关闭设备识别
                    remove_device(result, device, i);
                    diff   = diff_features_2slowly(clusters[i], clusters[i - k], feats, 1);
                    device = recdevice(diff, 1, result, i, clusters, feats,
                                       n_cluster);  // 缓变类开启前的设备识别，缓变类
                    if (inarr_u8(result[i].device, device, RESULT_NUM) < 0) {
                        add_device(result, device, i, diff.P, diff.Praw, diff.Pused, clusters[i].loc);
                    }
                    continue;
                }
                // if (((inarr_u8(result[i-k].device, 1, RESULT_NUM) >= 0 )||(inarr_u8(result[i-k].device, 2,
                // RESULT_NUM) >= 0 ))&&(clusters[i].flag & 0x04)&&(clusters[i-k].flag & 0x04))
                diff = diff_features_2slowly(clusters[i - k], clusters[i], feats, 3);
                // else
                //     diff = diff_features(clusters[i - k], clusters[i], feats, result[i]);

                if ((inarr_u8(result[i].device, 2, RESULT_NUM) >= 0 && (clusters[i].flag & 0x01) &&
                     diff.Q < 0) ||
                    (inarr_u8(result[i].device, 1, RESULT_NUM) >= 0 && (clusters[i].flag & 0x01) && dpzrflag))
                    device = recdevice(diff, 5, result, i, clusters, feats,
                                       n_cluster);  // 如果有变频空调且当前类为slowly且diff.Q<0 ,缓变的另一个特殊处理
                else {
                    diff.Pstd = clusters[i].Pstd;
                    device = recdevice(diff, 4, result, i, clusters, feats, n_cluster);  // 关闭设备识别，普通类
                }

                remove_device(result, device, i);
            }
            /*float Qtmp=0;//check 电动车
            Qtmp=arr_mean_short(feats.Q,clusters[i].t[0],clusters[i].t[clusters[i].loc-1]);
                        rt_kprintf("Qtmp:%f\r\n",Qtmp);
            if (Qtmp>clusters[i].P*1.1 && clusters[i].P<300 && clusters[i].P>200 &&clusters[i].loc>500&&
            (!result[i].device[0]))
            //如果当前类无功大于有功，且有功小于400，且无设备或只有一个非电动车设备，给弄成自行车
            {
                add_device(result,19,i,clusters[i].P,clusters[i].Praw,clusters[i].P*clusters[i].loc,clusters[i].loc);
            }*/
            if (result[i].device[0] == 2 && result[i].device[1] == 0)  //如果变频空调是现存的唯一设备
            {
                diff.P    = clusters[i].P - Pinit;
                diff.Praw = clusters[i].Praw - Pinit;
                update_device(result, diff, clusters[i].loc, 2, i, 2);  //更新开着的变频空调参数
            }
            update_recunknownR(result, clusters, i, n_cluster);
            continue;
        } 
		else  //变化很小认为无事发生，既没有开，也没有关的处理
        {
            /*float Qtmp=0;//check 电动车
            Qtmp=arr_mean_short(feats.Q,clusters[i].t[0],clusters[i].t[clusters[i].loc]);
            if (Qtmp>clusters[i].P*1.05 && clusters[i].P<400 &&clusters[i].loc>500&&
            (!result[i].device[0]||(!result[i].device[1] &&result[i].device[0]!=19)))
            //如果当前类无功大于有功，且有功小于400，且无设备或只有一个非电动车设备，给弄成自行车
            {
                result[i].device[0]=19;
            }*/
            // rt_kprintf("时段：%d\r\n",N15min);
            // rt_kprintf("无事发生 \r\n");
            if ((result[i].device[0] == 2 || result[i].device[0] == 1 || result[i].device[0] == 8) &&
                result[i].device[1] == 0)  // 如果变频空调是现存的唯一设备，//误判：脉冲充电法会进入此分支
            {
                diff.P    = clusters[i].P - Pinit;
                diff.Praw = clusters[i].Praw - Pinit;
                update_device(result, diff, clusters[i].loc, result[i].device[0], i,
                              2);  // 更新开着的变频空调参数
            }
            update_recunknownR(result, clusters, i, n_cluster);//所有的未知都是电热大类的判断
            continue;
        }
        /***********     END 差分特征值        *********/

        
        
        
        /***********START 根据差分特征值识别设备*******/
        if (clusters[i].pair)  //反复启停类，误判：脉冲充电法会进入此分支
        {
            device = recdevice(diff, 2, result, i, clusters, feats, n_cluster);  // 反复启停类识别
        } 
		//非反复启停
		else 
		{
			//也是一个特殊处理
            if (clusters[i].Psteady - clusters[i - k].Psteady > 2200 &&
                clusters[i].Pstd <
                    400)  //如果单次功率差太大，说明slowly类开启跟前一个类没分开，此处需要多识别一次，并重新计算缓变类特征值
            {
                diff = diff_features_2slowly(
                    clusters[i], clusters[i - k], feats,
                    0);  // 计算缓变类开始开启前的一个设备的特征值（默认前五个点为前一类，即如果设备开启时间差小于5s，会识别错误）
                device =
                    recdevice(diff, 1, result, i, clusters, feats, n_cluster);  // 缓变类开启前的设备识别
                add_device(result, device, i, diff.P, diff.Praw, diff.Pused, clusters[i].loc);
                diff   = diff_features_2slowly(clusters[i], clusters[i - k], feats,
                                             1);  //在一类内部计算缓变类的特征值
                N15min = 0;
            }
			//这里才是处理这正的缓变
            if (clusters[i].flag & 0x01) {
                device = recdevice(diff, 3, result, i, clusters, feats, n_cluster);  // 缓慢变化类识别
                //特殊情况
				if ((device == 4 || device == 22) && diff.P > 1400 && result[i].device[0] == 0 && i == 1) {
                    diff = diff_features_2slowly(
                        clusters[i], clusters[i - k], feats,
                        0);  // 计算缓变类开始开启前的一个设备的特征值（默认前五个点为前一类，即如果设备开启时间差小于5s，会识别错误）
                    device = recdevice(diff, 1, result, i, clusters, feats,
                                       n_cluster);  // 缓变类开启前的设备识别
                    add_device(result, device, i, diff.P, diff.Praw, diff.Pused, clusters[i].loc);
                    diff   = diff_features_2slowly(clusters[i], clusters[i - k], feats,
                                                 1);  // 在一类内部计算缓变类的特征值
                    device = recdevice(diff, 3, result, i, clusters, feats, n_cluster);
                }
				//特殊情况，判断空调用
                if (device == 2 && k == 1 && result[i].device[0] == 2 && result[i].device[1] == 0) {
                    diff.P    = clusters[i].P - Pinit;
                    diff.Praw = clusters[i].Praw - Pinit;
                    update_device(result, diff, clusters[i].loc, 2, i, 2);  //更新开着的变频空调参数
                    continue;
                }
            } 
			else  //变频空调状态更新及普通设备
            {

				#if(USE_EBIDF == 0)
				//处理空调的，上一个已经有了空调，这个延续，处理变频
                if (inarr_u8(result[i].device, 2, RESULT_NUM) >= 0 &&
                    (clusters[i].P <=
                     clusters[i - k].Psteady + 200))  //如果有变频空调开着，那么小于200的变化认为是变频空调
                {
                    update_device(result, diff, clusters[i].loc, 2, i, 1);  //更新开着的变频空调参数
                    continue;
                } 
				//处理空调的，处理定频
				else if 
				(
                    (inarr_u8(result[i].device, 1, RESULT_NUM) >= 0 &&
                     (clusters[i].P <= clusters[i - k].Psteady + 150)) ||
                    (inarr_u8(result[i].device, 1, RESULT_NUM) >= 0 &&
                     (clusters[i].P <= clusters[i - k].Psteady + 300) &&
                     result[i].device[inarr_u8(result[i].device, 1, RESULT_NUM) + 1] ==
                         0))  //如果有定频空调开着，那么小于150的变化认为是定频空调,如果定频空调刚开，那么小于300的变化认为是定频空调
                {
                    update_device(result, diff, clusters[i].loc, 1, i, 1);  //更新开着的定频空调参数
                    continue;
                } 
				
				//处理普通类别
				else 
				#endif //#if(USE_EBIDF == 0)
				{
					//i是最后一个类的时候，保留最好一个类的状态，准备给下一次识别用
					if (i == n_cluster - 1 && (tlength - clusters[i].t[clusters[i].loc - 1] - 1 >= 4) &&
						(tlength - clusters[i].t[clusters[i].loc - 1] - 1 <= 13) &&
						clusters[i].pair == 0)  //最后一类，若实际采样长度-最后一个点长度在[5,14]，且pair=0
					{
						short tmpPmax, tmpPmin, tmppmaxmax;
						short tmpP[15]    = {0};
						short tmppmax[15] = {0};
						uint8_t    tmpcount    = 0;
						for (uint16_t tmpi = clusters[i].t[clusters[i].loc - 1] + 2; tmpi < tlength; tmpi++) 
						{
							tmpP[tmpcount]    = feats.P[tmpi];
							tmppmax[tmpcount] = feats.Pmax[tmpi];
							tmpcount++;
						}
						tmppmax[tmpcount] = feats.Pmax[clusters[i].t[clusters[i].loc - 1] + 1];
						tmpPmax           = arr_max_short(tmpP, tmpcount);
						tmpPmin           = arr_min_short(tmpP, tmpcount);
						tmppmaxmax        = arr_max_short(tmppmax, tmpcount + 1);
						if (tmpPmax - tmpPmin < 10 && ((clusters[i].flag & 0x01) == 0) &&
							(tmpP[0] < clusters[i].P)) 
						{
							lastclusteruselastpoint = 1;
							continue;
						} 
						else if (tmppmaxmax - tmpPmin > 3000 &&
									(tmpPmax - tmpPmin) < (0.35 * (tmpPmax - clusters[i].P))) 
						{
							lastclusteruselastpoint = tmppmaxmax - tmpPmin;
						}
					}
					device = recdevice(diff, 1, result, i, clusters, feats, n_cluster);  // 普通类识别
				}
			}
		}
		
		//定频的特殊处理
		if (inarr_u8(result[i].device, 1, RESULT_NUM) >= 0 && diff.P < 450 && dpzrflag &&
			(device == 21 || device == 8)) 
		{
			update_device(result, diff, clusters[i].loc, 1, i, 1);  //更新开着的定频空调参数
			continue;
		}
		
		//如果接反了，会不会进入，不过对冻结结果不影响
		if (diff.P > 0 || clusters[i].P < 0)  //光伏有可能触发clusters[i].P<0
			add_device(result, device, i, diff.P, diff.Praw, diff.Pused,clusters[i].loc);  //将识别结果添加至result

		/***********END 根据差分特征值识别设备*******/
	}

	//将反复启停设备关闭时的合集赋予反复启停设备的启动状态
	switch_union(clusters, result, n_cluster); //
	del_add_device(clusters, result, n_cluster);

	//识别unKnownR,电热类的识别，这个电热类的识别是穿插的程序始终
	for (m = 0; m < n_cluster; m++) {
		for (uint8_t j = 0; j < RESULT_NUM; j++)  //根据当前类的时间重新计算功耗
		{
			if (result[m].device[j]) {
				if (result[m].device[j] >= 22 && result[m].device[j] <= 25)  //根据运行时间区分热水器等
				{
					recunknownR(result, n_cluster, result[m].device[j] - 22);
				}
			} else
				break;
		}
	}

	//显示结果
	BYTE    dev_output[RESULT_NUM]   = {0};
	SWORD 	p_output[RESULT_NUM]     = {0};
	float 	pused_output[RESULT_NUM] = {0};
	BYTE    dev_start[RESULT_NUM]    = {0};
	BYTE    dev_end[RESULT_NUM]      = {0};
	BYTE    dev_switch[RESULT_NUM]   = {0};
	BYTE    dev_count                = 0;//代表当前有多少个识别的设备

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("n_cluster:%d\n", n_cluster);
	}
	#endif


	for (i = 0; i < n_cluster; i++) 
	{
		char temp[200];
		snprintf(temp, 199,
			"*******cluster:%d ,clusterP:%d,clusterPraw:%d,,clusterQ:%d,pair:%d,flag:0x%02x,Pstd:%f,loc:%d********\r\n",
			i, clusters[i].P, clusters[i].Praw, clusters[i].Q, clusters[i].pair, clusters[i].flag,
			clusters[i].Pstd, clusters[i].loc);
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf(temp);
		}
		#endif

		for (j = 0; j < RESULT_NUM; j++) 
		{
			if (result[i].device[j]) 
			{
				//向屏幕打印
				#if(USE_PRINTF == 1)
				if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
				{
					rt_kprintf("time:%d\r\n", N15min);
					rt_kprintf("device:%d\r\n", result[i].device[j]);
					rt_kprintf("P:%d\r\n", result[i].P[j]);
//                	rt_kprintf("Pused:%f\r\n", result[i].Pused[j] / 3600000);
				}
				#endif
				char temp[100];
				snprintf(temp, 99,"Pused:%f\r\n", result[i].Pused[j] / 3600000);
				#if(USE_PRINTF == 1)
				if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
				{
					rt_kprintf(temp);
				}
				#endif
			}
		}
	}
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT4\r\n");
	}
	#endif
	//统计冻结结果
	float Pusedall_cal = 0;  //统计分解出的电器总消耗电量，与波形总电量比较
	for (i = 0; i < n_cluster; i++) {
		for (j = 0; j < RESULT_NUM; j++) 
		{
			Pusedall_cal += result[i].Pused[j]; //把所有电能累计
			if (dev_count >= RESULT_NUM) 
			{
				break;
			}
			//增加device[j] == 0x13的判断条件，防止电动车在同周期出现多次单独启停，导致的只能识别一个的问题，
			//有两种情况会进入，情况一是电动车开关后间隔一段时间再开关,这种情况认为是两个电动车
			//情况二是一个电动车中间关闭了某个电器，导致电动车被切割两半，这种情况是一个电动车，最后要合并起来
			#if(USE_EBIDF)
			if(wStep != 0)
			{
				wStep = 0;
			}

			if(result[i].device[j] == 0x13)
			{
				for(k=0; k<i; k++)
				{
					for(m=0;m<RESULT_NUM;m++)
					{
						if(result[i].device[j] == result[k].device[m])
						{
							wStep = abs(clusters[k].t[clusters[k].loc -1] - clusters[i].t[0]);
							if(wStep < 5)//判定当前电动车和上一个电动车结束的时间差，如果小于5，则认为是同一个连续的电动车，用于区分情况2
							{
								break;
							}
						}
					}
					if((wStep < 5) && (m != RESULT_NUM))
					{
						break;
					}
				}
			}

			if ((inarr_u8(dev_output, result[i].device[j], RESULT_NUM) < 0) 
			|| ((result[i].device[j] != 0) && (inarr_u8_count(result[i].device, result[i].device[j], j + 1) > inarr_u8_count(dev_output, result[i].device[j],RESULT_NUM)))
			|| ((wStep > 5) && (result[i].device[j] == 0x13) && (inarr_u8_count(result[i].device, result[i].device[j], j + 1) >= inarr_u8_count(dev_output, result[i].device[j],RESULT_NUM))))  //该类没出现过，或者同类设备为同时开启(result中数量大于dev中数量)
			#else
			if ((inarr_u8(dev_output, result[i].device[j], RESULT_NUM) < 0) 
			|| ((result[i].device[j] != 0) && (inarr_u8_count(result[i].device, result[i].device[j], j + 1) > inarr_u8_count(dev_output, result[i].device[j],RESULT_NUM))))  //该类没出现过，或者同类设备为同时开启(result中数量大于dev中数量)
			#endif //#if(USE_EBIDF)
			{
				dev_output[dev_count]   = result[i].device[j];
				pused_output[dev_count] = result[i].Pused[j];
				p_output[dev_count]     = result[i].P[j];
				dev_count += 1;
			} 
			else  //后续可加入如果有两个同种设备时，挑功率功率相近的合并
			{
				pused_output[inarr_u8(dev_output, result[i].device[j], RESULT_NUM)] += result[i].Pused[j];
				p_output[inarr_u8(dev_output, result[i].device[j], RESULT_NUM)] =
					(result[i].P[j] + p_output[inarr_u8(dev_output, result[i].device[j], RESULT_NUM)]) / 2;  //为什么要//2
			}
		}
	}

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT5\r\n");
	}
	#endif

	//启停时间
	startendtime(dev_count, dev_output, n_cluster, clusters, result, dev_start, dev_end, dev_switch);

	uint8_t addedbx = 0;
	// 比较实际波形总电量与计算总电量

	char temp[100];
	snprintf(temp, 99,"******Pusedall_real:%f\n", Pusedall_real / 3600000);
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf(temp);
	}
	#endif

	//洗衣机的特殊处理
	if (dev_count == 1 && dev_output[0] == 8 && arr_max_short(feats.Pmax, tlength) > 1500 &&
		Pusedall_real > 0.02 * 3600000 &&
		totalclusters == 2)  // 若只有洗衣机，Pmax>1500且真实总电量>0.02kwh且总聚类只有两类，把冰箱加进来
	{
		dev_count       = 2;
		dev_output[1]   = 11;
		pused_output[0] = 27000;
		p_output[1]     = 78;
		addedbx         = 1;
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("addedbx\r\n");
		}
		#endif
    }

	//如果计算值大于真实值，对应除以比例
	if (Pusedall_cal > Pusedall_real) {
		for (i = 0; i < dev_count; i++) 
		{
			pused_output[i] = pused_output[i] * Pusedall_real / Pusedall_cal;
			p_output[i]     = (short)(p_output[i] * Pusedall_real / Pusedall_cal);
		}
	}
	//另一个特殊处理
	else if (Pusedall_cal < Pusedall_real * 0.9 && dev_count == 1 &&
				(dev_output[0] == 12 || dev_output[0] == 8 || dev_output[0] == 10 || dev_output[0] == 1 ||
				dev_output[0] == 5 || dev_output[0] == 11 || (switch_count == 2 && dev_output[0] == 6))) 
	{
		pused_output[0] = Pusedall_real * 0.95;
	} 
	//这里是变频空调的处理
	else if ((Pusedall_cal == 0 && Pusedall_real > 0.005 * 3600000 && totalclusters > 3 &&
				dev_output[0] == 0) ||
				(Pusedall_cal < 0.005 * 3600000 && Pusedall_real < 0.005 * 3600000 && dev_output[0] == 2 &&
				dev_count == 1 && totalclusters == 2)) 
	{
		dev_output[0]   = 8;
		pused_output[0] = Pusedall_real * 0.95;
		p_output[0]     = 98;
		dev_count       = 1;
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("added xyj\r\n");
		}
		#endif
	}
	
	if (dev_output[0] == 8 && dev_count == 1)  //洗衣机顺延
	{
		lastresultis8 += 1;
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf(" lastresultis8:%d\r\n", lastresultis8);
		}
		#endif
	} 

	//电量比较小，也判断洗衣机没关闭
	else if ((dev_count == 0 ||
				(dev_count == 1 && dev_output[0] == 2 && pused_output[0] < 0.01 * 3600000)) &&
				Pusedall_real > 0.008 * 3600000 && lastresultis8) 
	{
		dev_output[0]   = 8;
		pused_output[0] = Pusedall_real * 0.95;
		p_output[0]     = 98;
		dev_count       = 1;
		lastresultis8 += 1;
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf(" lastresultis8:%d\r\n", lastresultis8);
			rt_kprintf("added xyj\r\n");
		}
		#endif
    }
	// 01haierrsq+dcl//海尔热水器+电磁炉
	else if (dev_count == 2 && dev_output[0] == 3 && dev_output[1] == 6 &&
				(Pusedall_cal / Pusedall_real) < 0.95 && (Pusedall_cal / Pusedall_real) > 0.7) {
		pused_output[0] = Pusedall_real / Pusedall_cal * pused_output[0] * 0.98;
		pused_output[1] = Pusedall_real / Pusedall_cal * pused_output[1] * 0.98;
	}
	//认为洗衣机最多开一个小时
	if (lastresultis8 >= 5 || dev_output[0] != 8)  //洗衣机一般会顺延一个小时，
		lastresultis8 = 0;

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT6\r\n");
	}
	#endif

	//向pi打印
	for (i = 0; i < dev_count; i++) 
	{
		pused_output[i] = pused_output[i] / 3600000;
		char temp[200];
		snprintf(temp, 199,"dev_output:%d,algo_p_output:%d,algo_pused_output:%f\n", dev_output[i], p_output[i],
					pused_output[i]);
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf(temp);
		}
		#endif
	}
	creat_frozen(&frozen[Phase], dev_count, dev_output, pused_output, p_output, dev_start, dev_end, dev_switch);

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		//打印冻结结果
		for (i = 0; i < frozen[Phase].num; i++)
		{
			rt_kprintf("Phase:%d, device:0x%04x-P:%d-Pused:%d-start:%d\r\n", Phase, frozen[Phase].sindev[i].device, frozen[Phase].sindev[i].p,
					frozen[Phase].sindev[i].pused, frozen[Phase].sindev[i].start);
		}

		get_sys_time(&Time);
		rt_kprintf("time:%d/%d/%d/%d\n",Time.Mon,Time.Day,Time.Hour,Time.Min);
		for (i = 0; i < MAX_PHASE; i++)
		{
			#if(USE_EBIDF)
			for(bEb = 0; bEb < (RESULT_NUM); bEb++)
			{
				rt_kprintf("EBResult.RevTime%02X\n",EBResult[bEb].RevTime[i]);
				rt_kprintf("TimeToClose:%ds\n",EBResult[bEb].TimeToClose[i]);
				rt_kprintf("EBResult.AverPower%02X\n",EBResult[bEb].AverPower[i]);
			}
			#endif
			for(j = 0; j < RESULT_NUM; j++)
			{	if(dev_output[j] != 0)
				{
					rt_kprintf("dev_p%d\n",p_output[j]);
					rt_kprintf("frozen device:%02X",dev_output[j]);
					rt_kprintf("dev_start:%02X",frozen[i].sindev[j].start);
					rt_kprintf("dev_end:%02X",frozen[i].sindev[j].end);
					rt_kprintf("dev_switch: %02X\n",frozen[i].sindev[j].switchtimes);
				}
			}
			rt_kprintf("feats.P[tlength-1]%d\n",feats.P[tlength-1]);
		}
			
	}
	#endif//#if(USE_EBIDF)
	
	//保存未知设备聚类
	for(int i=0; i<n_cluster; i++) //>10 ? 10: n_cluster
	{
		if (uk_device_index == 10)//最多10个
		{
			break;
		}
		memcpy(uk_device_praw_output+24*uk_device_index, &clusters[i].P, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+2, &clusters[i].Q, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+4, &clusters[i].Qfft, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+6, &clusters[i].D, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+8, &clusters[i].Psteady, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+10, &clusters[i].Praw, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+12, &clusters[i].Prawsteady, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+14, &clusters[i].switch_n, 1);
		memcpy(uk_device_praw_output+24*uk_device_index+15, &clusters[i].flag, 1);
		memcpy(uk_device_praw_output+24*uk_device_index+16, &clusters[i].loc, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+18, &clusters[i].pair, 2);
		memcpy(uk_device_praw_output+24*uk_device_index+20, &clusters[i].Pstd, 4);

		uk_device_index ++;
		char temp[200];
		snprintf(temp, 199,"i:%d,P:%x,Q:%x,Qfft:%x,D:%x,Psteady:%x,Praw:%x,Prawsteady:%x,switch_n:%x,flag:%x,loc:%x,pair:%x,Pstd:%x\r\n",
				i, clusters[i].P, clusters[i].Q, clusters[i].Qfft, clusters[i].D, clusters[i].Psteady, clusters[i].Praw, 
				clusters[i].Prawsteady, clusters[i].switch_n, clusters[i].flag, clusters[i].loc, clusters[i].pair, clusters[i].Pstd);
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf(temp);	
		}
		#endif
	}

	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT7\r\n");
	}
	#endif
    //如果本次是记满15min才停的，说明接下来还有下一段数据，将本次最后一个非switch启类的特征存下来传递给下一次
    // if(tlength>=900)
	if (((time_15min.min % FROZEN_CYCLE == 0 && time_15min.sec == 0) || tlength >= FROZEN_CYCLE_s) &&
		n_cluster > 0) 
	{
		uint8_t k = 1;
		//找上一个不是反复启停启的类
		if (!clusters[n_cluster - 1].pair || n_cluster == 1)  //找上一个不是反复启停启的类
		{
			k = 1;
		} else if ((n_cluster > 1 && !clusters[n_cluster - 2].pair) || n_cluster == 2) {
			k = 2;
		} else if (n_cluster > 2 && !clusters[n_cluster - 3].pair) {
			k = 3;
		}
		
		//900s的最后一个s的特征
		if (lastclusteruselastpoint == 1) 
		{
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("lastclusteruselastpoint == 1");
			}
			#endif
			lastcluster.P          = feats.P[tlength - 1];
			lastcluster.Praw       = feats.Praw[tlength - 1];
			lastcluster.Psteady    = lastcluster.P;
			lastcluster.Prawsteady = lastcluster.Praw;
			lastcluster.Q          = feats.Q[tlength - 1];
			lastcluster.Pmax       = (short)lastcluster.P;  //用来存最后一个P
			lastcluster.i50        = feats.i50[tlength - 1];
			lastcluster.i150       = feats.i150[tlength - 1];
			lastcluster.i250       = feats.i250[tlength - 1];
			lastcluster.phi50      = feats.phi50[tlength - 1];
			lastcluster.phi150     = feats.phi150[tlength - 1];
			lastcluster.phi250     = feats.phi250[tlength - 1];
			lastcluster.flag       = 0;
			memcpy(&lastcluster.lastresult, &result[n_cluster - k],
					sizeof(struct result_typeDef));  //将结果也赋值给lastcluster
		}
		//放最后一个类别的特征
		else 
		{
			lastcluster = last_cluster_features(clusters[n_cluster - k], feats, result[n_cluster - k]);
			if (lastclusteruselastpoint > 1) 
			{
				#if(USE_PRINTF == 1)
				if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
				{
					rt_kprintf("nextPmax = %d\r\n", lastclusteruselastpoint);
				}
				#endif
				lastcluster.nextPmax = lastclusteruselastpoint;
			} 
			else if (addedbx == 1) 
			{
				lastcluster.lastresult.device[0] = 11;
				lastcluster.lastresult.P[0]      = 78;
				lastcluster.lastresult.Pused[0]  = 27000;
			}
		}
		last_P    = feats.P[tlength - 1];
		last_Pmax = 0;
		for (uint8_t i = 1; i < 15; i++) {
			last_Pmax = max_short(last_Pmax, feats.Pmax[900 - i]);  // 最大峰值取最后15个的
		}
	}

	AbnormFlag[phase_index] = 0;
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT8\r\n");
	}
	#endif
	free(clusters);
	free(result);
	return lastcluster;
}

/*****************************************************************************
 函 数 名  : diff_features
 功能描述  : 差分两个类的特征值
 输入参数  : c1 c2
 输出参数  : difffeatures
 返 回 值  :
*****************************************************************************/
struct steady_typeDef diff_features(struct cluster_typeDef c1, struct cluster_typeDef c2,
                                    struct features_typeDef feats, struct result_typeDef result)
{
    struct steady_typeDef  diff      = {0};
    struct feature_typeDef c1sum     = {0};
    struct feature_typeDef c2sum     = {0};
    uint8_t                     findmaxpN = 15;
    csum_cal(&feats, c1.t, c1.loc, &c1sum);
    csum_cal(&feats, c2.t, c2.loc, &c2sum);

    if (c1.flag == 2 && c2.t[c2.loc - 1] + 10 < c1.t[0] &&
        c2.t[c2.loc - 1] + 40 > c1.t[0])  //当前类是反复启停类，且与上一类的时间间隔在[10,,40]
    {
        findmaxpN = c1.t[0] - c2.t[c2.loc - 1] + 5;  //最大峰值的间隔需往前找二者时间间隔+5
    }
    for (uint8_t i = 1; i <= findmaxpN; i++) {
        short n;
        n = c1.t[0] - i;
        if (n >= 0) {
            c1sum.Pmax = max_short(
                c1sum.Pmax, feats.Pmax[n]);  //最大峰值向前找15s(abs_short(diff.Q) < 20&& (diff.P>1400 ||
                                                //(inarr_u8(result.device, 2, RESULT_NUM) >= 0 && diff.P >
                                                // 1200))&& diff.Pmax < 3000&& diff.Pstd < 100)
        }
    }
    float Sub_50[2] = {0}, Sub_150[2] = {0}, Sub_250[2] = {0};
    diff.Pslope    = arr_slope_short(feats.P, c1.t, c1.loc);
    diff.Pslopeold = arr_slope_short(feats.P, c2.t, c2.loc);
    diff.Q         = (short)(c1.Q - c2.Q);
    diff.Qfft      = (short)(c1.Qfft - c2.Qfft);
    diff.D         = (short)(c1.D - c2.D);
    diff.Pmax      = (short)(c1sum.Pmax - c2.P);
    diff.Pstd      = c1.Pstd;
    if ((c1.t[0] > c2.t[0]) &&
        (((c1.flag & 0x01) && (c2.flag & 0x01) && (c1.P - c2.P > 1400) && diff.Pslope > 0.3 &&
          diff.Pslopeold > 0.3 && c1sum.Pmax - c1.P < 500) ||
         ((c1.flag & 0x01) && inarr_u8(result.device, 2, RESULT_NUM) >= 0 &&
          ((abs_short(diff.Q) < 20 || abs_short(diff.Q) < (c1.P - c2.P) * 0.02) && (c1.P - c2.P) > 1400 &&
           diff.Pmax < 3000 &&
           (diff.Pstd < 100 ||
            diff.Pstd < (c1.P - c2.P) * 0.1)))))  //仅针对开启，根据类是否是slowly决定P的计算方法
    {
        diff.P     = (short)(feats.P[c1.t[0]] - c2.Psteady);
        diff.Q     = (short)(c1.Q - feats.Q[c2.t[c2.loc - 1]]);
        diff.Qfft  = (short)(c1.Qfft - feats.Qfft[c2.t[c2.loc - 1]]);
        diff.D     = (short)(c1.D - feats.D[c2.t[c2.loc - 1]]);
        c1sum.Pmax = feats.P[c1.t[0]];
        for (uint8_t i = 1; i < 15; i++) {
            short n;
            if (c1.loc > 10) {
                n = c1.t[5] - i;
            } else {
                n = c1.t[0] - i;
            }

            if (n >= 0) {
                c1sum.Pmax = max_short(c1sum.Pmax, feats.Pmax[n]);  //最大峰值向前找15s
            }
        }
        diff.Pmax      = (short)(c1sum.Pmax - c2.Psteady);
        diff.notslowly = 1;
    } else if ((c1.flag & 0x01) && (c2.flag & 0x01)) {
        diff.P = (short)(c1.P - c2.P);
    } else {
        diff.P = (short)(c1.Psteady - c2.Psteady);
    }
    diff.Plast = c2.Psteady;
    diff.Praw  = (short)(c1.Prawsteady - c2.Prawsteady);
    if (((c2.flag & 0x01) && diff.Pslope > 0) && (c2.Prawsteady - c2.Praw > 200) && (!c1.pair) && (!c2.pair))
        diff.Pused = (c1.Praw - c2.Praw) * c1.loc;
    else
        diff.Pused = (c1.Praw - c2.Prawsteady) * c1.loc;
    vector_sub(c1sum.i50 / c1.loc, c2sum.i50 / c2.loc, c1sum.phi50 / c1.loc, c2sum.phi50 / c2.loc, Sub_50);
    vector_sub(c1sum.i150 / c1.loc, c2sum.i150 / c2.loc, c1sum.phi150 / c1.loc, c2sum.phi150 / c2.loc,
               Sub_150);
    vector_sub(c1sum.i250 / c1.loc, c2sum.i250 / c2.loc, c1sum.phi250 / c1.loc, c2sum.phi250 / c2.loc,
               Sub_250);
    diff.hdi3 = Sub_150[0] / Sub_50[0];
    diff.hdi5 = Sub_250[0] / Sub_50[0];

    if (c1.t[0] > c2.t[c2.loc - 1])
        diff.risetime = c1.t[0] - c2.t[c2.loc - 1];
    diff.Ptrans = feats.P[c1.t[0]] - feats.P[c2.t[c2.loc - 1]];
    if (c1.flag & 0x10)
        diff.specialswitch = 1;
    diff.dur = c1.loc;
    return diff;
}
/*****************************************************************************
 函 数 名  : diff_features_2slowly
 功能描述  : 差分两个类的特征值
 输入参数  : type0：当前类前十个数减上个类，type1：当前类后十个数减前十个数
 输出参数  : difffeatures
 返 回 值  :
*****************************************************************************/
struct steady_typeDef diff_features_2slowly(struct cluster_typeDef c1, struct cluster_typeDef c2,
                                            struct features_typeDef feats, uint8_t type)
{
    struct steady_typeDef  diff  = {0};
    struct feature_typeDef c1sum = {0};
    struct feature_typeDef c2sum = {0};
    uint16_t                    N1 = 0, N2 = 0;
    short*                 tempP = NULL;
    uint16_t*                   tempt = NULL;
    if (abs_short(feats.P[c1.t[0]] - c1.Psteady) < 200 &&
        type == 1)  //变化小于200对应的是在这一类前已经开启了另一个设备，但因时间不够没被识别成类
    {
        N1 = c1.loc;  //类从后往前取点数
        if (c1.t[0] > 8)
            N2 = 8;
        else
            N2 = c1.t[0] - 1;
        csum_cal(&feats, c1.t, N1, &c1sum);
        csum_cal(&feats, c1.t, -N2, &c2sum);
        diff.Pused  = (c1sum.Praw / N1 - c2sum.Praw / N2) * N1;
        diff.Pslope = arr_slope_short(feats.P, c1.t, c1.loc);
        diff.Plast  = (short)(c2sum.P / N2);
        for (uint8_t i = 1; i < 8; i++)  //最大峰值向前找8s
        {
            short n;
            n = c1.t[0] - i;
            if (n >= 0) {
                c1sum.Pmax = max_short(c1sum.Pmax, feats.Pmax[n]);
            }
        }
    } else if (abs_short(feats.P[c1.t[0]] - c1.Psteady) < 200 && type == 0)  //当前类前一类时间过短没被分开
    {
        if (c1.t[0] > 8)
            N1 = 7;
        else
            N1 = c1.t[0] - 1;  //类从后往前取点数
        N2 = c2.loc;
        csum_cal(&feats, c1.t, -N1, &c1sum);  //当前类t0=53向前取7个点t0=52--52-7
        csum_cal(&feats, c2.t, N2, &c2sum);
        diff.Plast  = (short)(c2sum.P / N2);
        diff.Pslope = 0;
        diff.Pused =
            (c1sum.Praw / N1 - c2sum.Praw / N2) *
            (N1 +
             c1.loc);  //这里似乎应该*c1.loc+N1，因为前一类没分开的话，大概率会继承到当前类,根据01_美的定频空调挂机_01_制冷和九阳电水壶_01_烧水修改
        for (uint8_t i = 1; i < 48; i++)  // 最大峰值向前找48s
        {
            short n;
            n = c1.t[0] - i;
            if (n >= 0) {
                c1sum.Pmax = max_short(c1sum.Pmax, feats.Pmax[n]);
            }
        }
    } else if (type == 1)  //变化大于200，对应的是在本类中有个新的缓变
    {
        N1 = 10;  //类从后往前取点数
        for (uint16_t i = 0; i < c1.loc; i++) {
            if (feats.P[c1.t[i]] > feats.P[c1.t[0]] + 100)  //比初始点大过100，就算第二类的点了
            {
                N1 = c1.loc - i;  //真正的缓变类的长度
                break;
            }
        }
        N2    = 10;  //类从前往后取点数
        tempP = (short*)malloc(N1 * sizeof(short));
        tempt = (uint16_t*)malloc(N1 * sizeof(short));
        if ((!tempP) || (!tempt)) {
            if (tempP)
                free(tempP);
            if (tempt)
                free(tempt);
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
            	rt_kprintf("tempPt out of memory");
			}
			#endif
            return diff;
        }
        csum_cal(&feats, c1.t + c1.loc - N1, N1, &c1sum);  //只对真正的缓变类求特征
        csum_cal(&feats, c1.t, N2, &c2sum);                //对缓变类前N2个求特征
        for (uint16_t i = c1.loc - N1; i < c1.loc; i++) {
            tempP[i + N1 - c1.loc] = feats.P[c1.t[i]];
            tempt[i + N1 - c1.loc] = i + N1 - c1.loc;
        }

        diff.Pslope = arr_slope_short(tempP, tempt, N1);
        diff.Plast  = (short)(c2sum.P / N2);
        diff.Pused  = (c1sum.Praw / N1 - c2sum.Praw / N2) * N1;
        free(tempP);
        free(tempt);
    } else if (type == 2)  //变化大于200，对应的是在本类中有个新的缓变
    {
        N1 = c1.loc;
        N2 = 5;

        csum_cal(&feats, c1.t, N1, &c1sum);
        csum_cal(&feats, c2.t, N2, &c2sum);  //新的缓变类前五个数

        diff.Ptrans = feats.P[c1.t[c1.loc - 1]] - feats.P[c2.t[0]];
        for (uint8_t i = 1; i < 15; i++)  //最大峰值向前找15s
        {
            short n;
            n = c1.t[0] - i;
            if (n >= 0) {
                c1sum.Pmax = max_short(c1sum.Pmax, feats.Pmax[n]);
            }
        }
        diff.P          = (short)(c1sum.P / N1 - c2sum.P / N2);
        diff.Praw       = (short)(c1sum.Praw / N1 - c2sum.Praw / N2);
        diff.Q          = (short)(c1sum.Q / N1 - c2sum.Q / N2);
        diff.Qfft       = (short)(c1sum.Qfft / N1 - c2sum.Qfft / N2);
        diff.D          = (short)(c1sum.D / N1 - c2sum.D / N2);
        diff.Pmax       = (short)(c1sum.Pmax - c2sum.P / N2);
        float Sub_50[2] = {0}, Sub_150[2] = {0}, Sub_250[2] = {0};
        vector_sub(c1sum.i50 / N1, c2sum.i50 / N2, c1sum.phi50 / N1, c2sum.phi50 / N2, Sub_50);
        vector_sub(c1sum.i150 / N1, c2sum.i150 / N2, c1sum.phi150 / N1, c2sum.phi150 / N2, Sub_150);
        vector_sub(c1sum.i250 / N1, c2sum.i250 / N2, c1sum.phi250 / N1, c2sum.phi250 / N2, Sub_250);
        diff.hdi3 = Sub_150[0] / Sub_50[0];
        diff.hdi5 = Sub_250[0] / Sub_50[0];

        return diff;
    } else if (type == 3)  // 关闭的取前一类后几个点，和当前类前几个点
    {
        N1 = 5;

        N2 = 5;

        type1   = 3;
        cur_len = c1.loc;
        csum_cal(&feats, c1.t, -N1, &c1sum);
        type1 = 0;
        csum_cal(&feats, c2.t, N2, &c2sum);  // 新的缓变类前五个数

        diff.Ptrans = feats.P[c1.t[c1.loc - 1]] - feats.P[c2.t[0]];
        for (uint8_t i = 1; i < 15; i++)  // 最大峰值向前找15s
        {
            short n;
            n = c1.t[0] - i;
            if (n >= 0) {
                c1sum.Pmax = max_short(c1sum.Pmax, feats.Pmax[n]);
            }
        }
        diff.P          = (short)(c1sum.P / N1 - c2sum.P / N2);
        diff.Praw       = (short)(c1sum.Praw / N1 - c2sum.Praw / N2);
        diff.Q          = (short)(c1sum.Q / N1 - c2sum.Q / N2);
        diff.Qfft       = (short)(c1sum.Qfft / N1 - c2sum.Qfft / N2);
        diff.D          = (short)(c1sum.D / N1 - c2sum.D / N2);
        diff.Pmax       = (short)(c1sum.Pmax - c2sum.P / N2);
        float Sub_50[2] = {0}, Sub_150[2] = {0}, Sub_250[2] = {0};
        vector_sub(c1sum.i50 / N1, c2sum.i50 / N2, c1sum.phi50 / N1, c2sum.phi50 / N2, Sub_50);
        vector_sub(c1sum.i150 / N1, c2sum.i150 / N2, c1sum.phi150 / N1, c2sum.phi150 / N2, Sub_150);
        vector_sub(c1sum.i250 / N1, c2sum.i250 / N2, c1sum.phi250 / N1, c2sum.phi250 / N2, Sub_250);
        diff.hdi3 = Sub_150[0] / Sub_50[0];
        diff.hdi5 = Sub_250[0] / Sub_50[0];

        return diff;
    } else {
        N1    = 10;
        N2    = c2.loc - 1;
        tempP = (short*)malloc(N1 * sizeof(short));
        tempt = (uint16_t*)malloc(N1 * sizeof(short));
        if ((!tempP) || (!tempt)) {
            if (tempP)
                free(tempP);
            if (tempt)
                free(tempt);
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
            	rt_kprintf("tempPt out of memory");
			}
			#endif
            return diff;
        }
        csum_cal(&feats, c1.t, N1, &c1sum);
        csum_cal(&feats, c2.t, N2, &c2sum);
        for (uint16_t i = 0; i < N1; i++) {
            tempP[i] = feats.P[c1.t[i]];
            tempt[i] = i;
        }
        diff.Pslope = arr_slope_short(tempP, tempt, 10);
        diff.Plast  = c2.Psteady;
        diff.Pused  = (c1sum.Praw / N1 - c2sum.Praw / N2) * c1.loc;
        free(tempP);
        free(tempt);
        if (c1.t[0] > c2.t[c2.loc - 1])  //计算上升时间、短时瞬时功率变化
            diff.risetime = c1.t[0] - c2.t[c2.loc - 1];
        diff.Ptrans = feats.P[c1.t[0]] - feats.P[c2.t[c2.loc - 1]];
        for (uint8_t i = 1; i < 15; i++)  //最大峰值向前找15s
        {
            short n;
            n = c1.t[0] - i;
            if (n >= 0) {
                c1sum.Pmax = max_short(c1sum.Pmax, feats.Pmax[n]);
            }
        }
    }
    float Sub_50[2] = {0}, Sub_150[2] = {0}, Sub_250[2] = {0};

    diff.P    = (short)(c1sum.P / N1 - c2sum.P / N2);
    diff.Praw = (short)(c1sum.Praw / N1 - c2sum.Praw / N2);
    diff.Q    = (short)(c1sum.Q / N1 - c2sum.Q / N2);
    diff.Qfft = (short)(c1sum.Qfft / N1 - c2sum.Qfft / N2);
    diff.D    = (short)(c1sum.D / N1 - c2sum.D / N2);
    diff.Pmax = (short)(c1sum.Pmax - c2sum.P / N2);
    vector_sub(c1sum.i50 / N1, c2sum.i50 / N2, c1sum.phi50 / N1, c2sum.phi50 / N2, Sub_50);
    vector_sub(c1sum.i150 / N1, c2sum.i150 / N2, c1sum.phi150 / N1, c2sum.phi150 / N2, Sub_150);
    vector_sub(c1sum.i250 / N1, c2sum.i250 / N2, c1sum.phi250 / N1, c2sum.phi250 / N2, Sub_250);
    diff.hdi3 = Sub_150[0] / Sub_50[0];
    diff.hdi5 = Sub_250[0] / Sub_50[0];

    return diff;
}
/*****************************************************************************
函 数 名  : diff_lastcluster
功能描述  : 专门针对与上15min最后一类差分的函数
输入参数  : c1:当前类 c2：上一类
 输出参数  : difffeatures
 返 回 值  :
*****************************************************************************/
struct steady_typeDef diff_lastcluster(struct cluster_typeDef c1, struct lastfeature_typeDef c2,
                                       struct features_typeDef feats, uint8_t onoff)
{
    struct steady_typeDef  diff  = {0};
    struct feature_typeDef c1sum = {0};
    // memset(&diff,0,sizeof(struct steady_typeDef));
    csum_cal(&feats, c1.t, c1.loc, &c1sum);
    int mean_p_10 = 0;

    for (uint8_t i = 1; i < 10; i++) {
        short n;
        n = c1.t[0] - i;
        if (n >= 0) {
            c1sum.Pmax = max_short(c1sum.Pmax, feats.Pmax[n]);  //最大峰值向前找10s
        }
        mean_p_10 += feats.P[i];
    }

    if (mean_p_10 / 10 > 100 && c1sum.Pmax < last_Pmax)  // cluster[0]属于开启状态并且最大值
    {
        c1sum.Pmax = last_Pmax;
    }

    float Sub_50[2] = {0}, Sub_150[2] = {0}, Sub_250[2] = {0};
    if (onoff) {
        if ((c1.flag & 0x01) && (c2.flag & 0x01))  //根据类是否是slowly决定P的计算方法
        {
            diff.P = (short)(c1.P - c2.P);
        } else {
            diff.P = (short)(c1.Psteady - c2.Psteady);
        }
        diff.Praw  = (short)(c1.Prawsteady - c2.Prawsteady);
        diff.Pused = (c1.Praw - c2.Prawsteady) * c1.loc;
        diff.Q     = (short)(c1.Q - c2.Q);
        diff.Qfft  = (short)(c1.Qfft - c2.Qfft);
        diff.D     = (short)(c1.D - c2.D);
        diff.Pmax  = max_short((short)(c1sum.Pmax - c2.P), c2.nextPmax);
        // diff.Praw=c1sum.Praw/c1.loc-c2sum.Praw/c2.loc;
        vector_sub(c1sum.i50 / c1.loc, c2.i50, c1sum.phi50 / c1.loc, c2.phi50, Sub_50);
        vector_sub(c1sum.i150 / c1.loc, c2.i150, c1sum.phi150 / c1.loc, c2.phi150, Sub_150);
        vector_sub(c1sum.i250 / c1.loc, c2.i250, c1sum.phi250 / c1.loc, c2.phi250, Sub_250);
        diff.hdi3     = Sub_150[0] / Sub_50[0];
        diff.hdi5     = Sub_250[0] / Sub_50[0];
        diff.Pslope   = arr_slope_short(feats.P, c1.t, c1.loc);
        diff.Pstd     = c1.Pstd;
        diff.Ptrans   = (short)(feats.P[c1.t[0]] - c2.Psteady);
        diff.risetime = c1.t[0];
        diff.Plast    = (short)c2.Psteady;
    } else {
        diff.P    = (short)(c2.P - c1.Psteady);
        diff.Q    = (short)(c2.Q - c1sum.Q / c1.loc);
        diff.Qfft = (short)(c2.Qfft - c1sum.Qfft / c1.loc);
        diff.D    = (short)(c2.D - c1sum.D / c1.loc);
        // diff.Pmax=(short)(c1sum.Pmax-c2.P);
        diff.Praw = (short)(c2.Prawsteady - c1.Prawsteady);
        diff.Pstd = c1.Pstd;
        vector_sub(c2.i50, c1sum.i50 / c1.loc, c2.phi50, c1sum.phi50 / c1.loc, Sub_50);
        vector_sub(c2.i150, c1sum.i150 / c1.loc, c2.phi150, c1sum.phi150 / c1.loc, Sub_150);
        vector_sub(c2.i250, c1sum.i250 / c1.loc, c2.phi250, c1sum.phi250 / c1.loc, Sub_250);
        diff.hdi3   = Sub_150[0] / Sub_50[0];
        diff.hdi5   = Sub_250[0] / Sub_50[0];
        diff.Pslope = arr_slope_short(feats.P, c1.t, c1.loc);
    }
    diff.Plast = (short)c2.P;
    return diff;
}
/*****************************************************************************
 函 数 名  : last_cluster_features
 功能描述  : 计算15min最后一个类的特征值，留下来给下一个15min
 输入参数  : 最后一类c1，总特征features，最后一类的结果result
 输出参数  : lastfeature_typeDef
 返 回 值  :
*****************************************************************************/
struct lastfeature_typeDef last_cluster_features(struct cluster_typeDef c1, struct features_typeDef feats,
                                                 struct result_typeDef result)
{
    struct feature_typeDef     c1sum       = {0};
    struct lastfeature_typeDef lastcluster = {0};
    for (uint16_t i = 0; i < c1.loc; i++) {
        // c1sum.P += feats.P[c1.t[i]];
        c1sum.Q += feats.Q[c1.t[i]];
        c1sum.Qfft += feats.Qfft[c1.t[i]];
        c1sum.D += feats.D[c1.t[i]];
        c1sum.i50 += feats.i50[c1.t[i] >> 1];
        c1sum.i150 += feats.i150[c1.t[i] >> 1];
        c1sum.i250 += feats.i250[c1.t[i] >> 1];
        c1sum.phi50 += feats.phi50[c1.t[i] >> 1];
        c1sum.phi150 += feats.phi150[c1.t[i] >> 1];
        c1sum.phi250 += feats.phi250[c1.t[i] >> 1];
    }

    lastcluster.P          = c1.P;
    lastcluster.Praw       = c1.Prawsteady;
    lastcluster.Psteady    = c1.Psteady;
    lastcluster.Prawsteady = c1.Prawsteady;
    lastcluster.Q          = (short)(c1sum.Q / c1.loc);
    lastcluster.Qfft       = (short)(c1sum.Qfft / c1.loc);
    lastcluster.D          = (short)(c1sum.D / c1.loc);
    lastcluster.Pmax       = feats.P[c1.t[c1.loc - 1]];  //用来存最后一个P
    lastcluster.i50        = c1sum.i50 / c1.loc;
    lastcluster.i150       = c1sum.i150 / c1.loc;
    lastcluster.i250       = c1sum.i250 / c1.loc;
    lastcluster.phi50      = c1sum.phi50 / c1.loc;
    lastcluster.phi150     = c1sum.phi150 / c1.loc;
    lastcluster.phi250     = c1sum.phi250 / c1.loc;
    lastcluster.flag       = c1.flag;
    memcpy(&lastcluster.lastresult, &result, sizeof(struct result_typeDef));  //将结果也赋值给lastcluster
    return lastcluster;
}

/*****************************************************************************
 函 数 名  : cluster
 功能描述  : 将15min数据依据功率和时间聚类，并将结果整理成包含类数目个结构体的数组。
 输入参数  : P（1s窗口的有功功率900个）
 输出参数  : clusters
 返 回 值  :
*****************************************************************************/
struct cluster_typeDef* cluster(float* i50, short* P, short* Q, short* Qfft, short* D, short* Praw,
                                short* Pmax, float* Pusedall_real, uint8_t* n_cluster)
{
	uint8_t* c = NULL;
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT1 \r\n");
	}
	#endif
	c = (uint8_t*)malloc(FROZEN_CYCLE_s * sizeof(uint8_t));
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT3 \r\n");
	}
	#endif

	if (!c) 
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("c out of memory\n");
		}
		#endif
		return NULL;
	}
	memset(c, 0, FROZEN_CYCLE_s * sizeof(uint8_t));

	c = DBSCAN(P, 48, 12, c, FROZEN_CYCLE_s);   //c：每一个点属于第几类
	if (N15min == 0) //第一次进入算法
	{
		c = check_1st(c, P, Q);  //看第一时段的第一类的功率是否较高，如果是的话，说明开启太快，要在第一类之前人工补一类
	}

	*n_cluster = arr_max_u8(c, tlength);  //获取有效类的个数

	uint8_t unlabeled    = 0;
	uint8_t clusterthre1 = 15;
	uint8_t clusterthre2 = 7;
	uint8_t maxnow       = 0;
//	uint8_t maxnowcount  = 0;

	for (uint16_t i = 0; i < tlength; i++)  //未归类多且一致归为一类
	{
		if (c[i] == 0 && maxnow >= 1)  //处理连续稳定的未归类点
		{
			unlabeled += 1;			//累计未归类个数
			if (unlabeled >= clusterthre2) //如果未归类个>=7
			{
				//检查功率波动是否小于10w
				if (arr_max_short(&P[i - clusterthre2 + 1], clusterthre2) -
						arr_min_short(&P[i - clusterthre2 + 1], clusterthre2) < 10)  //连续的7个噪声点，功率最大值和最小值差
				{
					if (maxnow >= 2 && c[i - unlabeled] == maxnow) 
					{
						if (arr_max_u8(c, i - unlabeled) == maxnow - 1)  //假设11122230000，c[i-unlabeled]=3
						{
							c[i - unlabeled] = 0;
							maxnow -= 1;
						}
					}
					for (uint8_t j = 0; j < clusterthre2; j++) 
					{
						c[i - j] = maxnow + 1;
					}
					*n_cluster += 1;
					unlabeled = 0;
					for (uint16_t j = i + 1; j < tlength; j++) 
					{
						if (c[j] >= maxnow + 1) 
						{
							c[j] = c[j] + 1;  //插入类后面其他类标号（>=插入类标号）都加1
						}
					}
				}
			}
		}
		else //处理归类点和变化大的未归类点
		{
			maxnow = max_u8(maxnow, c[i]);  //更新最大类别标签
			
			//如果未归类数大于15，并且功率变量变换大于5000，满足条件重新聚类
			if (unlabeled > clusterthre1 && 
			arr_max_short(&Pmax[i - unlabeled + 1], unlabeled) - arr_min_short(&Pmax[i - unlabeled + 1], unlabeled) >5000)  //连续未聚类过多，且功率差异很大
			{
				uint8_t* tmpc = NULL;
				tmpc     = (uint8_t*)malloc(unlabeled * sizeof(uint8_t));
				if (!tmpc) 
				{
					#if(USE_PRINTF == 1)
					if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
					{
						rt_kprintf("tmpc out of memory\n");
					}
					#endif
					*n_cluster = 0;
					return NULL;
				}
				memset(tmpc, 0, unlabeled * sizeof(uint8_t));
				tmpc = DBSCAN(&P[i - unlabeled], 100, 10, tmpc, unlabeled);
				if (arr_max_u8(tmpc, unlabeled) == 1) 
				{
					for (uint8_t j = 0; j < unlabeled; j++) 
					{
						c[i - unlabeled + j] = maxnow;  //为什么是max_now不是max_now+1
					}
				}
				free(tmpc);
			}
			unlabeled = 0;  //重置未归类计数
		}
	}

	//场景1：当聚类算法完全失败时,保守地将所有数据点归为1个类,保证系统有基本输出，避免完全失效
	if (*n_cluster == 0) 
	{
		*n_cluster = 1;
		c[0]       = 1;
		for (uint16_t i = 0; i < tlength; i++) 
		{
			c[i] = 1;
		}
	} 
	//情况2：只有一个类且数据量较大 (二次聚类)
	else if (*n_cluster == 1 && tlength > 850)  //二次分类
	{
		memset(c, 0, FROZEN_CYCLE_s * sizeof(uint8_t));
		c          = DBSCAN(Pmax, 50, 12, c, FROZEN_CYCLE_s); //（宽松设置，检测大变化）
		*n_cluster = arr_max_u8(c, tlength);  //获取有效类的个数

		if (*n_cluster == 0) //数据点归为1个类，避免完全失效
		{
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("n_cluster:%d,tlength:%d,P[0]:%d,P[50]:%d\r\n", *n_cluster, tlength, P[0], P[50]);
			}
			#endif
			*n_cluster = 1;
			c[0]       = 1;
			for (uint16_t i = 0; i < tlength; i++) 
			{
				c[i] = 1;
			}
		} 
		else if (*n_cluster == 1 && P[0] == 0 && P[50] != 0) 
		{
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("n_cluster:%d,tlength:%d,P[0]:%d,P[50]:%d\r\n", *n_cluster, tlength, P[0], P[50]);
			}
			#endif
			memset(c, 0, FROZEN_CYCLE_s * sizeof(uint8_t));
			c          = DBSCAN(Pmax, 20, 12, c, FROZEN_CYCLE_s); //（严格设置，检测小变化）
			*n_cluster = arr_max_u8(c, tlength);  //获取有效类的个数
			Pthres     = 10;
			expectone  = 1;
			
			if (*n_cluster == 0) //数据点归为1个类，避免完全失效
			{
				*n_cluster = 1;
				c[0]       = 1;
				for (uint16_t i = 0; i < tlength; i++) 
				{
					c[i] = 1;
				}
			}
		} 
		else 
		{
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("n_cluster:%d,tlength:%d,P[0]:%d,P[50]:%d\r\n", *n_cluster, tlength, P[0], P[50]);
			}
			#endif
		}
	}
	// M0222
	if (arr_abs_sum_short(P, 0, tlength) != 0 &&
		arr_abs_sum_short(P, tlength - 15, tlength) == 0)  //若有设备开启，且结尾有连续15个0
	{
		for (uint16_t i = 0; i < tlength; i++) 
		{
			if (P[i] == 0 && arr_abs_sum_short(P, i, tlength) == 0)  //从第i个点开始补0，找到第一个全零起始点i
			{
				uint8_t  maxnow            = arr_max_u8(c, tlength) + 1;
				uint16_t lastcluster_count = 0;
				for (uint16_t k = 0; k < tlength; k++)  //需要被分割的类的点数
				{
					if (c[k] == c[i]) 
					{
						lastcluster_count += 1;
					}
				}
				if (abs(lastcluster_count - (tlength - i)) > 10) //判断长度差异是否显著（>10）
				{
					for (uint16_t j = i; j < tlength; j++) 
					{
						c[j] = maxnow;  //补的0为新一类
					}
					(*n_cluster) += 1;
				}
				break;
			}
		}
	}
	struct cluster_typeDef* clusters = NULL;
	clusters = (struct cluster_typeDef*)malloc(*n_cluster * sizeof(struct cluster_typeDef));  // 按类的个数给clusters分配内存
	if (!clusters) 
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("clusters out of memory\n");
		}
		#endif

		*n_cluster = 0;
		free(c);
		return clusters;
	}
	memset(clusters, 0, *n_cluster * sizeof(struct cluster_typeDef));
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("clusters size:%d\r\n", *n_cluster * sizeof(struct cluster_typeDef));
	}
	#endif

	//统计时间、特征、电量
	for (uint16_t i = 0; i < tlength; i++)  //将每一类的时间、有功功率存入对应的结构体数组clusters中。   只存时间和点数
	{
		if (c[i] != 0 && (c[i] <= *n_cluster)) 
		{
			clusters[c[i] - 1].t[clusters[c[i] - 1].loc] = i;  //每一类的时间
			// clusters[c[i]-1].P[clusters[c[i]-1].loc]=P[i];//赋值每一类的平均功率
			clusters[c[i] - 1].loc += 1;  //每一类的点数
			unlabeled = 0;
		}
		*Pusedall_real += Praw[i];  //统计本冻结周期波形总电量
	}
	free(c);

	for (uint8_t i = 0; i < *n_cluster; i++) 
	{
		if (i == 0 && P[clusters[0].t[0]] == 0 && P[clusters[0].t[clusters[0].loc - 1]] != 0) 
		{  //第一类 第一点功率0，最后一点功率不为0
			cluster_Pstd(i50, P, Praw, Q, Qfft, D, &clusters[i], 1);  //1-只用非零电流的点计算
		} 
		else 
		{
			cluster_Pstd(i50, P, Praw, Q, Qfft, D, &clusters[i], 0);  //赋值每一类的平均功率、稳定功率缓变标志
		}
	}

	if (*n_cluster == 2 && tlength > 850 && clusters[0].P < 10 && clusters[1].P < 50)  //二次分类
	{
		c = (uint8_t*)malloc(FROZEN_CYCLE_s * sizeof(uint8_t));
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("POINT3 \r\n");
		}
		#endif
		if (!c) 
		{
			free(clusters);
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("c out of memory\n");
			}
			#endif
			*n_cluster = 0;
			return NULL;
		}

		memset(c, 0, FROZEN_CYCLE_s * sizeof(uint8_t));

		c = DBSCAN(Pmax, 50, 12, c, FROZEN_CYCLE_s);
		uint8_t oldn_cluster = *n_cluster;
		*n_cluster      = arr_max_u8(c, tlength);  //获取有效类的个数
		if (*n_cluster == 3) 
		{
			free(clusters);
			clusters = NULL;
			clusters = (struct cluster_typeDef*)malloc(
				*n_cluster * sizeof(struct cluster_typeDef));  //按类的个数给clusters分配内存
			if (!clusters) 
			{
				#if(USE_PRINTF == 1)
				if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
				{
					rt_kprintf("clusters out of memory\n");
				}
				#endif
				*n_cluster = 0;
				free(c);
				return clusters;
			}
			memset(clusters, 0, *n_cluster * sizeof(struct cluster_typeDef));
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("clusters size:%d\r\n", *n_cluster * sizeof(struct cluster_typeDef));
			}
			#endif

			for (uint16_t i = 0; i < tlength; i++)  //将每一类的时间、有功功率存入对应的结构体数组clusters中。
			{
				if (c[i] != 0 && (c[i] <= *n_cluster)) 
				{
					clusters[c[i] - 1].t[clusters[c[i] - 1].loc] = i;  //每一类的时间
					// clusters[c[i]-1].P[clusters[c[i]-1].loc]=P[i];//赋值每一类的平均功率
					clusters[c[i] - 1].loc += 1;  //每一类的点数
					unlabeled = 0;
				}
				*Pusedall_real += Praw[i];  //统计本冻结周期波形总电量
			}
			free(c);
			for (uint8_t i = 0; i < *n_cluster; i++) 
			{
				if (i == 0 && P[clusters[0].t[0]] == 0 && P[clusters[0].t[clusters[0].loc - 1]] != 0) 
				{
					cluster_Pstd(i50, P, Praw, Q, Qfft, D, &clusters[i], 1);
				} 
				else 
				{
					cluster_Pstd(i50, P, Praw, Q, Qfft, D, &clusters[i],0);  //赋值每一类的平均功率、稳定功率缓变标志
				}
			}
		} 
		else 
		{
			*n_cluster = oldn_cluster;
			free(c);
		}
	}
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("POINT2 \r\n");
	}
	#endif
	return clusters;
}



/*****************************************************************************
 函 数 名  : new_rule
 功能描述  : 定制规则
 输入参数  : P/onoff启/停标识/result（仅在关闭识别时有用）
 输出参数  : device
 返 回 值  :
*****************************************************************************/
uint8_t new_rule(struct steady_typeDef diff, struct result_typeDef* result, uint8_t device_id, int i,
            struct cluster_typeDef* clusters, struct features_typeDef feats, uint8_t n_cluster, uint8_t type)
{
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
	{
		rt_kprintf("\n in new rule\n");
		rt_kprintf("diff.P:%d,diff.Pmax:%d,diff.Q:%d,diff.hdi3:%f,diff.hdi5:%f,diff.Pslope:%f", diff.P, diff.Pmax,
				diff.Q, diff.hdi3, diff.hdi5, diff.Pslope);
	}
	#endif


	if (PROVINCE_CODE == 135)  // 135
	{
		if (clusters[i].Q > 4 &&
			((abs(clusters[i].P - 639) < 15 && clusters[i].Q < 20) ||
				(abs(clusters[i].P - 150) < 20 && clusters[i].Q < 20 && abs(diff.Pmax - 166) < 20) ||
				(abs(diff.P - 639) < 15 && diff.Q < 20) ||
				(abs(diff.P - 150) < 20 && diff.Q < 20 && abs(diff.Pmax - 166) < 20))) 
		{
			// rt_kprintf("diff.P:%d,diff.Pmax:%d,diff.Q:%d,diff.hdi3:%f,diff.hdi5:%f,diff.Pslope:%f",diff.P,diff.Pmax,diff.Q,diff.hdi3,diff.hdi5,diff.Pslope);
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("change to dcf\n");
			}
			#endif
			return 14;
		}
        //(diff.Pmax-clusters[i].Pmax)>10
        if (clusters[i].Q > 4 &&
            ((abs(clusters[i].P - 2018) < 20 && clusters[i].Q < 25) ||
             (abs(clusters[i].P - 1117) < 20 && clusters[i].Q < 25) ||
             (abs(clusters[i].P - 640) < 20 && clusters[i].Q > 700) || (abs(clusters[i].P - 150) < 20) ||
             (abs(clusters[i].P - 1568) < 20 && abs(clusters[i].Q - 199) < 25) ||
             (abs(clusters[i].P - 1023) < 20 && abs(clusters[i].Q - 52) < 20) ||
             ((abs(clusters[i].P - 562) < 20 || abs(clusters[i].P - 1462) < 20) &&
              abs(clusters[i].Q - 253) < 20) ||
             (abs(clusters[i].P - 288) < 20 && abs(clusters[i].Q - 46) < 20) ||
             (abs(clusters[i].P - 376) < 20 && abs(clusters[i].Q - 17) < 20) ||
             (abs(clusters[i].P - 193) < 20 && abs(clusters[i].Q - 89) < 20) ||
             (abs(diff.P - 2018) < 20 && diff.Q < 25) || (abs(diff.P - 1117) < 20 && diff.Q < 25) ||
             (abs(diff.P - 1568) < 20 && abs(diff.Q - 199) < 25) ||
             (abs(diff.P - 933) < 20 && abs(diff.Q - 694) < 25) ||
             (abs(diff.P - 1023) < 20 && abs(diff.Q - 52) < 20) ||
             ((abs(diff.P - 562) < 20 || abs(diff.P - 1462) < 20) && abs(diff.Q - 253) < 20) ||
             (abs(diff.P - 288) < 20 && abs(diff.Q - 46) < 20) ||
             (abs(diff.P - 376) < 20 && abs(diff.Q - 17) < 20) ||
             (abs(diff.P - 193) < 20 && abs(diff.Q - 89) < 20))) 
		{
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
            	rt_kprintf("change to dcf\n");
			}
			#endif
            return 14;
        }
        if (clusters[i].Q > 4 &&
            ((abs(clusters[i].P - 2018) < 20 && clusters[i].Q < 25) ||
             (abs(clusters[i].P - 1117) < 20 && clusters[i].Q < 25) ||
             (abs(clusters[i].P - 1165) < 20 && abs(clusters[i].Q - 686) < 25) ||
             (abs(clusters[i].P - 1117) < 20 && abs(clusters[i].Q - 15) < 20) ||
             ((abs(clusters[i].P - 935) < 20) && abs(clusters[i].Q - 698) < 20) ||
             (abs(clusters[i].P - 967) < 20 && abs(clusters[i].Q - 238) < 20) ||
             (abs(clusters[i].P - 30) < 10 && clusters[i].Q > 75) ||
             (abs(clusters[i].P - 375) < 20 && abs(clusters[i].Q - 15) < 20) ||
             (abs(clusters[i].P - 222) < 20 && abs(clusters[i].Q - 127) < 20) ||
             (abs(diff.P - 2018) < 20 && diff.Q < 25) || (abs(diff.P - 1117) < 20 && diff.Q < 25) ||
             (abs(diff.P - 1568) < 20 && abs(diff.Q - 199) < 25) ||
             (abs(diff.P - 933) < 20 && abs(diff.Q - 694) < 25) ||
             (abs(diff.P - 1023) < 20 && abs(diff.Q - 52) < 20) ||
             ((abs(diff.P - 562) < 20 || abs(diff.P - 1462) < 20) && abs(diff.Q - 253) < 20) ||
             (abs(diff.P - 288) < 20 && abs(diff.Q - 46) < 20) ||
             (abs(diff.P - 376) < 20 && abs(diff.Q - 17) < 20) ||
             (abs(diff.P - 193) < 20 && abs(diff.Q - 89) < 20))) 
		{
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("change to dcf\n");
			}
			#endif
            return 14;
        }
    }
    
    return device_id;
    
}


/*****************************************************************************
 函 数 名  : recdevice
 功能描述  : 识别设备种类
 输入参数  : 差分特征值diff，设备类型type：1普通启；2反复启停启；3缓变启；4关闭。
 输出参数  : device
 返 回 值  :
*****************************************************************************/
uint8_t recdevice(struct steady_typeDef diff, uint8_t type, struct result_typeDef* result, int i,
             struct cluster_typeDef* clusters, struct features_typeDef feats, uint8_t n_cluster)
{
	uint8_t device = 0;
	uint8_t rlc    = 0;
	uint8_t kttype = 0;

	if ((type == 0x03) && (diff.notslowly == 0x01)) 
	{
		if(inarr_u8(result[i].device, 2, RESULT_NUM) >= 0x00)
		{
			type = 1;
		}
	}
	switch (type) 
	{
		case 1:  //普通类 判阻和非阻性
			rlc = reclevel1(diff.P, diff.Q, diff.Pmax, diff.Plast, 1);
			if (rlc == 1) 
			{
				if (diff.Pmax - diff.P > 1500 &&
					(diff.hdi3 > 0.05 || (diff.hdi3 > 0.04 && diff.risetime > 2)) && diff.P > 950 &&
					diff.P < 1200)
					device = reclevel2lc(diff.P, diff.Q, diff.Pmax, diff.hdi3, diff.hdi5, diff.Pslope,
											diff.Pstd, 1, result[i], 0);  // 非阻类
				else
					device = reclevel2r(diff.P, (short)diff.Pstd, diff.Pmax, diff.dur, 1, result[i]);  // 电阻类
			} 
			else 
			{
				device = reclevel2lc(diff.P, diff.Q, diff.Pmax, diff.hdi3, diff.hdi5, diff.Pslope, diff.Pstd,
										1, result[i], 0);  // 非阻类
			}
			break;
		case 2:                      //反复启停类
			if (diff.specialswitch)  //假反复启停类
			{
				rlc = reclevel1(diff.P, diff.Q, diff.Pmax, diff.Plast, 1);
				if (rlc == 1)
					device = reclevel2r(diff.P, (short)diff.Pstd, diff.Pmax, diff.dur, 1, result[i]);  // 电阻类
				else
					device = reclevel2lc(diff.P, diff.Q, diff.Pmax, diff.hdi3, diff.hdi5, diff.Pslope,
											diff.Pstd, 1, result[i], 0);  // 非阻类
			} 
			else if ((diff.Q >= 200 || (diff.Q >= 170 && diff.hdi3 > 0.13)) && diff.P > 500) 
			{
				device = 7;  //微波炉
			} 
			else if (diff.P > 1200) 
			{
				device = 12;  //烤箱
			} 
			else if (diff.P > 120 && diff.P < 160 && diff.dur < 30 && diff.Q < 20 && diff.Pstd < 20) 
			{
				device = 13;  //抽油烟机
			} 
			else if (diff.P < 250) 
			{
				device = 8;  // 洗衣机 //误判：脉冲充电法电动车容易判断为这个分支
			}                // Morg：九阳电磁炉，将diff.P>890变为diff.P>850
			else if ((diff.Q > 90 && diff.Q < 140 && diff.P > 850 && diff.P < 1000) ||
						(diff.Q > 80 && diff.Q < 140 && diff.P > 800 && diff.P < 1000 && diff.Pstd > 20 &&
						diff.Ptrans > 800)) 
			{
				device = 6;  //电磁炉
			} 
			else 
			{
				device = 5;  //电饭煲
			}
			break;
		case 3:  //缓变类
			if ((diff.Pstd < 60 &&
					((abs_short(diff.Q) < 20 && diff.Pmax < 3000) || (diff.Plast < 50 && diff.Q < 20))) ||
				(abs_short(diff.Q) < 20 && diff.P > 1400 && diff.Pmax < 3000 && diff.Pstd < 100) ||
				(dpzrflag && (abs_short(diff.Q) < 20))) 
			{
				device = reclevel2r(diff.P, (short)diff.Pstd, diff.Pmax, diff.dur, 1, result[i]);
				#if(USE_PRINTF == 1)
				if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
				{
					rt_kprintf("slowly turned to %d\r\n", device);
				}
				#endif
			} 
			else if ((diff.P < 160 || (diff.P < 250 && diff.Pslope < -0.1) ||
						(diff.P < 250 && diff.Pstd > 75 && diff.Pslope < 0.1)) &&
						diff.P > 80 && diff.Pslope < 0.5) 
			{
				device = 8;  //洗衣机
			} 
			else if (diff.Pmax > 3000) 
			{
				device = 1;  //定频空调
				if (diff.Pstd > 70)
					dpzrflag = 1;
			} 
			else if (diff.P > 1600) 
			{
				device = 6;  //电磁炉
			} 
			else if (diff.Pslope < 0 && diff.P > 600 && diff.P < 1500) 
			{
				if (diff.hdi3 > 0.1 || diff.Q > 100)
					device = 7;  //微波炉
				else
					device = 15;  //电暖器（启动峰值有时会被聚到类里）
			} 
			else if (inarr_u8(result[i].device, 2, RESULT_NUM) >= 0 && diff.hdi3 > 0.2 && diff.P > 1000 &&
						diff.P < 1500)  // 变频空调开着会影响slope，因此加入hdi3+P的判据
			{
				device = 7;  //微波炉
			} 
			else if (diff.risetime <= 5 && diff.Ptrans > 600 && dpzrflag == 1) 
			{
				if (diff.P > 1600)
					device = 6;  //电磁炉
				else
					device = 7;
			} 
			else 
			{
				device = 2;
			}
			#if(USE_PRINTF == 1)
			if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
			{
				rt_kprintf("case3: device:%d,std:%f, P:%d, pslope:%f, dur:%d\r\n", device, diff.Pstd, diff.P,
						diff.Pslope, diff.dur);
			}
			#endif
			break;
		case 5:  //设备关闭且当前为缓变类
			if (dpzrflag)
				kttype = 1;
			else
				kttype = 2;
		case 4:  //设备关闭
			rlc = reclevel1(diff.P, diff.Q, diff.Pmax, diff.Plast, 0);
			if (rlc == 1) 
			{
				device = reclevel2r(diff.P, (short)diff.Pstd, diff.Pmax, diff.dur, 0, result[i]);  // 电阻类
				for (uint8_t j = 0; j < RESULT_NUM; j++)  //关闭设备不在设备列表中
				{
					if (device == result[i].device[j]) 
					{
						break;
					}
					if (j == RESULT_NUM - 1) 
					{
						device = reclevel2lc(diff.P, diff.Q, diff.Pmax, diff.hdi3, diff.hdi5, diff.Pslope,
												diff.Pstd, 0, result[i], kttype);
					}
				}
			} 
			else 
			{
				device = reclevel2lc(diff.P, diff.Q, diff.Pmax, diff.hdi3, diff.hdi5, diff.Pslope, diff.Pstd,
										0, result[i], kttype);
				for (uint8_t j = 0; j < RESULT_NUM; j++) 
				{
					if (device == result[i].device[j]) 
					{
						break;
					}
					if (j == RESULT_NUM - 1) 
					{
						device = reclevel2r(diff.P, (short)diff.Pstd, diff.Pmax, diff.dur, 0, result[i]);
					}
				}
			}
			break;
	}

	#if(USE_EBIDF == 0)
	device = new_rule(diff, result, device, i, clusters, feats, n_cluster, type);//福建特殊处理
	#endif

	return device;
}

/*****************************************************************************
 函 数 名  : reclevel1
 功能描述  : 根据差分特征判断设备为阻类还是非阻类。1.无功在±20以内为阻类，无功<0且前一个有功<20为阻类。
 输入参数  : P，Q
 输出参数  : 1(阻类)/2（非阻类）
 返 回 值  :
*****************************************************************************/
uint8_t reclevel1(short P, short Q, short Pmax, short Plast, uint8_t onoff)
{
	short stemp = 0;
	
	stemp = abs_short(Q);
	if ((((Q < 20) && (Q > -25)) || ((Q < 0) && (Plast < 50)) || ((stemp * 100 < 4 * P)  && (Pmax < 3000) && (onoff == 1))) && ((P > 100) || (Pmax < 1000)))  //通用条件
	// if ((Q < 15 && Q>-25)||(Q<0 && Plast<50)||(abs_short(Q) <(0.04*P)&&(Pmax<3000&&onoff)))
	// //针对电科院五小时奇葩空调的修改条件
	{
		return 1;
	} 
	else if (Q < 25 && Q > -25 && (!onoff) && stemp < (0.03 * P) && (P > 100 || Pmax < 1000)) 
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("new R\r\n");
		}
		#endif
		return 1;
	} 
	else 
	{
		return 2;
	}
}

/*****************************************************************************
 函 数 名  : reclevel2r
 功能描述  : 在阻类中进一步判断设备种类。
 输入参数  : P/onoff启/停标识/result（仅在关闭识别时有用）
 输出参数  : device
 返 回 值  :
*****************************************************************************/
uint8_t reclevel2r(short P, short Pstd, short Pmax, uint16_t dur, uint8_t onoff, struct result_typeDef result)
{
	uint8_t device[13] = {
		3,  4,  5,  14, 15, 8, 13,
		10, 21, 22, 23, 24, 25};  //热水器3，热水壶4，电饭煲5,吹风机14，电暖器15，饮水机16,电热大类22,电视10
	uint8_t score[13] = {0};
	if (!onoff)  //如果是关闭事件，那把现有设备列表中设备对应的分加到高一个等级
	{
		for (uint8_t i = 0; i < 13; i++) {
			for (uint8_t j = 0; j < RESULT_NUM; j++) {
				if (result.device[j] == device[i]) {
					if (score[i] < 100)  //防止两个同种设备开导致该设备关闭分过高
						score[i] += 10;
					if (abs(result.P[j] - P) <
						100)  // 找关闭时给功率相近的设备加分，关闭的功率和继承的开启功率相近
						score[i] += 2;
					else if (abs(result.P[j] - P) < 200)
						score[i] += 1;
				}
			}
		}
	}

	if (onoff && (Pmax - P > 1000) && fabs(P - 747) > 30 && fabs(Pmax - 1770) > 30)
		score[8] += 7;
	else if ((P >= 130 || (P >= 110 && Pstd < 6 && Pmax > P * 1.9)) && P < 250) {
		if (Pmax > P * 1.9 && Pstd < 40)
			score[6] += 7;
		else
			score[5] += 7;
	} else if (P < 100 && expectone == 1)
		score[7] += 7;
	else if (P < 130)
		score[5] += 7;
	else if (P < 500)
		score[8] += 7;
	else if (P < 650 && P > 500)
		score[9] += 7;  // 250~700可能是饮水机或者电热锅
	else if (P > 900 && P < 1150 && dur >= 600)
		score[4] += 7;  // 250~700可能是饮水机或者电热锅
	else if (P < 1000)
		score[2] += 7;  // 250~700可能是饮水机或者电热锅
	else if (P < 1560 && P > 1450 && Pmax < 1650)
		score[3] += 7;  // 700~950之间有可能是电饭煲或者电吹风
	else if (P < 1550 && P > 1450 && Pmax > 1850)
		score[4] += 7;              // 1450~1550之间且有启动峰值有可能是电饭煲或者电吹风
	else if (P > 1600 && P < 1850)  // 1000~1200之间有可能是电饭煲或者电暖器
		score[1] += 7;
	else if (P > 1850)  // 1200~1500之间有可能是吹风机或者电暖器
		score[9] += 7;
	else
		score[9] += 7;

	uint8_t id = 0;  // 找分数最高的作为输出，TODO相同评分似乎没考虑
	for (uint8_t i = 1; i < 13; i++) {
		if (score[i] > score[id]) {
			id = i;
		}
	}
	if (onoff && id == 9)  // 电热大类
	{
		for (uint8_t i = 0; i < 4; i++) {
            if (unknownR[phase_index][i].P == 0) {
				id = 9 + i;
				break;
			}
		}
	}
	if (!onoff) {
		if (score[id] == 10 && id >= 9 && id <= 12 &&
			abs_short(result.P[inarr_u8(result.device, device[id], RESULT_NUM)] - P) / P < 0.3) {
		}

		else if (score[id] <= 10 && score[8] == 0) {
			short mindiff = 10000;
			for (uint8_t j = 0; j < RESULT_NUM; j++) {
				if (abs_short(result.P[j] - P) < mindiff) {
					mindiff = abs_short(result.P[j] - P);
					id      = inarr_u8(device, result.device[j], 13);
				}
			}
		}
	}
	if (id == 255 && !onoff)
		return 55;
	else
		return device[id];
}

/*****************************************************************************
 函 数 名  : reclevel2lc
 功能描述  : 在非阻类中进一步判断设备种类。
 输入参数  : P,Pmax,hdi3/onoff启(1)/停(0)标识/（仅在关闭识别时有用）,
            type0:普通，type1:当前类是slowly，但是关闭事件
 输出参数  : device
 返 回 值  :
*****************************************************************************/
uint8_t reclevel2lc(short P, short Q, short Pmax, float hdi3, float hdi5, float Pslope, float pstd, uint8_t onoff,
               struct result_typeDef result, uint8_t type)
{
	uint8_t device[12] = {
		1,  2,  6, 7,  14, 11, 13,
		18, 21, 8, 19, 50};  //定频空调1，变频空调2，电磁炉（2100）6，微波炉7，电视10，冰箱11，抽油烟机13,吸尘器18，未知21,洗衣机8，吹风机14,大功率充电类（电动车）19
	short score[12]   = {0};
	uint8_t    otherdevice = 0;

	#if(USE_EBIDF)
	BYTE bEb = 0;
	#endif

	if (!onoff)  //如果是关闭事件，那把现有设备列表中设备对应的分加到高一个等级
	{
		for (uint8_t i = 0; i < 12; i++) {
			for (uint8_t j = 0; j < RESULT_NUM; j++) {
				if (result.device[j] == device[i]) {
					if (score[i] < 100)  //防止两个同种设备开导致该设备关闭分过高
						score[i] += 100;
					if (abs(result.P[j] - P) < 100)  //找关闭时给功率相近的设备加分
						score[i] += 20;
					else if (abs(result.P[j] - P) < 200)
						score[i] += 10;
				}
				//根据功率相近程度增加对应设备分数
				if ((result.device[j] != 0x00) && (result.device[j] != type))
					otherdevice = 1;
			}
		}
		if (type &&
			otherdevice)  //如果是slowly类且变频/定频空调不是唯一的现有设备时，排除变频/定频空调的关闭嫌疑
		{
			score[type - 1] = 0;
		}
	}
	// M0222
	// 三次谐波占比
	// if (hdi3 >= 0.2 && hdi3 < 0.4)
	if (hdi3 >= 0.199 && hdi3 < 0.4) {
		score[3] += 35;  //微波炉
	} else if (hdi3 > 0.17 && hdi3 < 0.2 && Pslope < 0) {
		score[3] += 35;  //微波炉
	} else if (hdi3 > 0.6) {
		score[1] += 35;  //变频空调
	} else if (P < 1550 && P > 1450 && abs_short(Q) < 100) {
		score[4] += 50;  //电吹风
	} else if ((P > 1300 && hdi3 < 0.1) || P > 1800) {
		score[2] += 50;  //电磁炉
	}

	//无功与有功比,电动车
	if (Q > P * 1.05 && P < 300) 
	{
		if ((Pmax / P) > 10) 
		{
			score[10] += 65;  //电动车
		} else 
		{
			score[10] += 75;  //电动车
		}
	} 
	else if (Q > P * 0.9) 
	{
		if ((Pmax / P) < 3)
		{
			score[10] += 65;  //电动车
		}

	} 
	else if (Q < P * 0.5) 
	{
		score[10] -= 75;  //电动车
	}

	//最大峰值比
	if ((Pmax / P) > 10 && P < 150) 
	{
		score[5] += 70;  //冰箱
	}
	//有功功率
	if (P > 1000 && P < 1800) 
	{
		score[0] += 25;  //定频空调
		score[3] += 25;  //微波炉
		score[7] += 25;  //吸尘器
		if (!onoff) 
		{
			score[1] += 25;  //变频空调
		}
	}

	else if (P > 250 && P < 620) 
	{
		score[1] += 25;  //变频空调
		score[3] -= 25;  //微波炉
	} 
	else if (P > 100 && P < 250) 
	{
		score[9] += 50;   //洗衣机
		score[10] += 50;  //电动车
	} 
	else if (P <= 100) 
	{
		score[4] += 50;  //电视
		score[5] += 50;  //冰箱
	}

	//最大峰值
	if (Pmax < 3000 && onoff) 
	{
		score[0] -= 70;  //定频空调
		score[7] -= 70;  //吸尘器
	} 
	else if (Pmax > 3000 && onoff) 
	{
		if (hdi5 < 0.1 && P > 1000 && P < 1500 && hdi5 < hdi3)
			score[7] += 70;  //吸尘器
		else
			score[0] += 70;  //定频空调
	} 
	else if (Pmax - P > 1500 && onoff && hdi3 > 0.05 && P > 950 && P < 1200) {
		score[3] += 35;  //微波炉
	}

	//电动车的特殊处理 1：代码为了防止多个周期时，nilm任务没有识别结果，但是EBResult存在，从而匹配不上结果，导致冻结丢失问题
	//如果多个周期后，电动车功率降低，开启一个电器功率和电动车的记录平均功率接近的，是存在可能把非电动车的电器识别为电动车！！！！！！
	#if(USE_EBIDF)
	for(bEb = 0; bEb < (EB_RESULT_NUM); bEb++)
	{
		if(((EBResult[bEb].TimeToClose[phase_index] > 0) 
		&& ((fabs(EBResult[bEb].PMAX_PHASE[phase_index] - P) < EB_MINPDIFF)) 
		&& (EBResult[bEb].IsCompared[phase_index] == 0))
		|| (AbnormFlag[phase_index] == 0xAA)) //iscompared必须是0才能进入判断，一次冻结结果重复进多次
		{
			EBResult[bEb].IsCompared[phase_index] = 1;
			score[10] += 80;  //电动车
			break;
		}
	}
	#endif//#if(USE_EBIDF)

	//找分数最高的作为输出
	uint8_t id = 0;
	for (uint8_t i = 1; i < 12; i++) 
	{
		if (score[i] > score[id]) 
		{
			id = i;
		}
	}
	//如果有重复的，输出未知21。
	if (onoff) 
	{
		for (uint8_t i = id + 1; i < 12; i++) 
		{
			if (score[i] == score[id]) 
			{
				id = 8;
			}
		}
	} 
	else 
	{
		if (score[id] <= 100 && score[8] == 0) 
		{
			short mindiff = 10000;
			for (uint8_t j = 0; j < RESULT_NUM; j++) 
			{
				if (abs_short(result.P[j] - P) < mindiff) 
				{
					mindiff = abs_short(result.P[j] - P);
					id      = inarr_u8(device, result.device[j], 12);
				}
			}
		} else if ((score[id] < 130 && score[8] > 0) || score[id] < 100) {
			id = 8;
		}
	}
	if (id == 1 && inarr_u8(result.device, 1, RESULT_NUM) >= 0 && dpzrflag == 1 && !onoff) 
	{
		if (pstd < 10) 
		{
			id       = 0;  //定频空调
			dpzrflag = 0;
		}
	}
	if (device[id] == 18)
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("XCQ: Pmax= %d,score6:%d,score1:%d,score18:%d,hdi5:%f \r\n", Pmax, score[2], score[0],
					score[7], hdi5);
		}
		#endif
	}
	return device[id];
}

/*****************************************************************************
 函 数 名  : DBSCAN
 功能描述  : DBSCAN聚类。
 输入参数  : t,P
 输出参数  : c
 返 回 值  :
*****************************************************************************/
uint8_t* DBSCAN(short* P, uint8_t eps, uint8_t min_samples, uint8_t* c, uint16_t len)
{
	// uint16_t t[900];
	dbscan_less_space(P, len, eps, min_samples, c);
	return c;
}

/*****************************************************************************
 函 数 名  : check_1st
 功能描述  : 1.看第一时段的第一类的功率是否较高，如果是的话，说明开启太快，要在第一类之前人工补一类
             2.如果无功功率高且有功std大于40，也补一类
 输入参数  : c,P
 输出参数  : c
 返 回 值  :
*****************************************************************************/
uint8_t* check_1st(uint8_t* c, short* P, short* Q)
{
	float* Ptmp = NULL;
	Ptmp        = (float*)malloc(tlength * sizeof(float));  //存储P值数组，计算std
	if (!Ptmp) 
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("check_1st out of memory");
		}
		#endif
		return c;
	}
	memset(Ptmp, 0, tlength * sizeof(float));
	uint16_t   fstclusterlen = 0;  //第一类结束的长度，用于计算功率std
	float std           = 0;  //第一类计算功率std

	int ptmp = 0, qtmp = 0;  //累计P Q值，用于计算平均有功无功功率
	uint16_t ttmp = 0, tstart = 0;

	if (c[0] == 0)  //电器开得太快了的情况
	{
		for (uint16_t i = 0; i < tlength; i++) 
		{
			if (c[i] == 1 && tstart == 0) 
			{
				tstart = i;
			}
			if (c[i] == 1) 
			{
				ptmp += P[i];
				ttmp += 1;
			}
		}
		std = 0;
	} 
	else  //第一类是洗衣机之类启动时没形成新类的情况
	{
		for (uint16_t i = 0; i < tlength; i++) 
		{
			if (c[i] == 1 && tstart == 0 && (P[i] - P[0]) > 50) //寻找功率突增点（>50W）作为潜在起始位置
			{
				tstart = i;
			}
			if (c[i] == 1) 
			{
				ptmp += P[i];
				ttmp += 1;
				qtmp += Q[i];
				Ptmp[i]       = P[i];
				fstclusterlen = i;
			}
		}
		arm_std_f32(Ptmp, fstclusterlen, &std);  //计算功率std
	}
	free(Ptmp);
	//条件1：平均有功功率比初始值高100W以上
	//条件2：有功波动大（std>40）且平均无功高200Var以上
	if (((ptmp / ttmp - P[0]) >= 100) 
	|| ((std > 40) && ((qtmp / ttmp - Q[0]) >= 200)))  //平均功率减功率初始值大于100W 或 无功功率>200且有功std大于40
	{
		for (uint16_t i = 0; i < tstart; i++) 
		{   
			//手动加一类
			if (P[i] - P[0] < 50)
			{
				c[i] = 1;
			}
		}
		for (uint16_t i = tstart; i < tlength; i++) 
		{    
			//其他类后移
			if (c[i] != 0) 
			{
				c[i] = c[i] + 1;
			}
		}
	}
	return c;
}
/*****************************************************************************
 函 数 名  : switch_label
 功能描述  : 找到多次启停类。
 输入参数  : clusters，n(类的数目）u16 tnew[50]={0};//待加入新switch启类的t的值
 输出参数  :
 返 回 值  :
*****************************************************************************/
void switch_label(struct cluster_typeDef* clusters, uint8_t n, short* P)
{
	uint8_t  switch_n;  //记录一个类中t中断多少次
	uint8_t* switch_c = NULL;
	switch_c     = (uint8_t*)malloc(n * sizeof(uint8_t));  //记录switch潜力类的编号

	uint8_t tnew_len = 0;  //待加入新switch停类的t的长度
	uint8_t inew     = 0;  //记录新加入的switch启的switch组内编号
	if (!switch_c) 
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("switch_c out of memory");
		}
		#endif
		return;
	}
	memset(switch_c, 0, n * sizeof(uint8_t));

	switch_count = 0;  //记录switch潜力类的个数
	for (uint8_t i = 0; i < n; i++) 
	{
		switch_n = 0;
		for (uint16_t j = 1; j < clusters[i].loc; j++) 
		{
			//如果想判断电动车开启不是反复启停类,在这里再增加条件，电动车判断之间点的差值大于60直接break， free(switch_c);,认为3到60内开，可能会判断为反复启停，可以结合畸变功率滤除，60根据实测适当调整小
			//脉冲式充电特殊处理：就判断异常标志，证明整个周期都是异常的。就直接break，不管几个类，free
			if(AbnormFlag[phase_index] == 0xAA)
			{
				free(switch_c);
				break;
			}
			if (clusters[i].t[j] - clusters[i].t[j - 1] >= 3) {  //时间中断3秒
				switch_n += 1;
			}
		}
		if (switch_n >= 1)  //如果一个类中，t中断次数≥1次，则标记为switch的潜力类
		{
			clusters[i].flag       = clusters[i].flag | 0x02;  // flag的倒数第二位，反复启停位
			switch_c[switch_count] = i;
			switch_count += 1;
		}
	}
	for (uint8_t i = 0; i < switch_count; i++)  //在switch潜力类中，分出启/停状态。
	{
		if (i < switch_count - 1 &&
			clusters[switch_c[i]].t[clusters[switch_c[i]].loc - 1] >
				clusters[switch_c[i + 1]].t[0] + 30)  //如果两个switch类的时间重合度高，则配对   两个相邻的类时间重合30以上
		{
			if (clusters[switch_c[i]].P >
				clusters[switch_c[i + 1]].P)  //如果后一个类功率低，说明其配对为后一类，根据类标号配置pair    当前类功率高，启，pair记录当前类可做差分的停
			{
				clusters[switch_c[i]].pair = switch_c[i + 1] - switch_c[i];
			}
		}
		if (i > 0 && !clusters[switch_c[i - 1]].pair &&
			clusters[switch_c[i]].t[0] <
				clusters[switch_c[i - 1]].t[clusters[switch_c[i - 1]].loc - 1] - 30) 
		{
			if (clusters[switch_c[i]].P >
				clusters[switch_c[i - 1]].P)  // 如果前一个类功率低，说明其配对为前一类，根据类标号配置pair
			{
				clusters[switch_c[i]].pair = switch_c[i - 1] - switch_c[i];
			}
		} 
		else if (i > 1 && clusters[switch_c[i - 1]].pair &&
					(clusters[switch_c[i]].t[0] <
					clusters[switch_c[i - 2]].t[clusters[switch_c[i - 2]].loc - 1] - 30) &&
					(!clusters[switch_c[i]].pair))  //如果前一类是反复启停‘启’，那再往前找一类
		{
			if (clusters[switch_c[i]].P > clusters[switch_c[i - 2]].P) {
				clusters[switch_c[i]].pair = switch_c[i - 2] - switch_c[i];
			}
		}
		if (!clusters[switch_c[i]]
					.pair)  //找出因base变化较大没聚成类而没被判为switch的类，但不将其base点加入周围的base类里
		{
			if (switch_count > 1 && tnew_len == 0 && clusters[switch_c[i]].loc < 60 &&
				((switch_c[i] == 0) ||
					(clusters[switch_c[i]].t[0] > clusters[switch_c[i] - 1].t[clusters[switch_c[i] - 1].loc])) &&
				((switch_c[i] == n - 1) || (clusters[switch_c[i]].loc < clusters[switch_c[i] + 1].t[0]))) 
			{
				uint16_t t = clusters[switch_c[i]].t[0];
				inew  = i;
				for (uint8_t j = 0; j < clusters[switch_c[i]].loc; j++)
				{
					if ((clusters[switch_c[i]].t[j] > t + 1) && (clusters[switch_c[i]].P > P[t + 1])) 
					{
						t++;
						while (t < clusters[switch_c[i]].t[j]) 
						{
							tnew_len++;
							t++;
						}
					} 
					else if (clusters[switch_c[i]].t[j] == (t + 1))
					{
						t++;
					}
				}
			}
		}
	}
	if (tnew_len) 
	{
		for (uint8_t k = 1; k < 3; k++) 
		{
			if (inew > 0 && (inew - k) > 0 && !clusters[switch_c[inew - k]].pair &&
				clusters[switch_c[inew - k]].P < clusters[switch_c[inew]].P) 
			{
				clusters[switch_c[inew]].pair = switch_c[inew - k] - switch_c[inew];
				break;
			}
			else if (!clusters[switch_c[inew + k]].pair &&
						clusters[switch_c[inew + k]].P < clusters[switch_c[inew]].P) 
			{
				clusters[switch_c[inew]].pair = switch_c[inew + k] - switch_c[inew];
				break;
			}
		}
	}
	free(switch_c);
}


/*****************************************************************************
 函 数 名  : add_device
 功能描述  : 将识别出的设备加入输出结果中
 输入参数  : result[],设备名字device,类序号i,设备功率P,设备持续时间
 输出参数  : result[i]
 返 回 值  :
*****************************************************************************/
void add_device(struct result_typeDef* result, uint8_t device, uint8_t i, short P, short Praw, float Pused, uint16_t t)
{
	if (device == 244)
		return;
	uint8_t j = 0;
	if (device >= 22 && device <= 25) 
	{
		unknownR[phase_index][device - 22].P      = P;
		unknownR[phase_index][device - 22].during = t;
	}
	while (j < RESULT_NUM) 
	{
		if (result[i].device[j] == 0) 
		{
			break;
		}
		/*if (device==result[i].device[j])//如果设备列表中已经有该设备
		{
			result[i].P[j]+=P;
			result[i].Pused[j]+=P*t;
		}*/
		j++;
	}
	if (j == RESULT_NUM) 
	{
		#if(USE_PRINTF == 1)
		if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
		{
			rt_kprintf("result超过 %d 个\r\n", RESULT_NUM);
		}
		#endif
        return;
    }
    result[i].device[j] = device;
    result[i].P[j]      = Praw;
    result[i].Pused[j]  = Pused;
	result[i].Ident[j]  = AddNum;
	AddNum += 1;
}

/*****************************************************************************
 函 数 名  : remove_device
 功能描述  : 将识别出的设备从输出结果中删除。如果有两个同名设备，找功率接近的那个（未加）
 输入参数  : result[i],第i类的输出，包含设备名字device,设备功率P,设备持续时间
 输出参数  : result[i]
 返 回 值  :
*****************************************************************************/
void remove_device(struct result_typeDef* result, uint8_t device, uint8_t i)
{
    uint8_t j = 0;
    while (j < RESULT_NUM) {
        if (result[i].device[j] == device) {
            break;
        }
        j++;
    }
    while (j < RESULT_NUM - 1) {
        result[i].device[j] = result[i].device[j + 1];
        result[i].P[j]      = result[i].P[j + 1];
        result[i].Pused[j]  = result[i].Pused[j + 1];
		result[i].Ident[j]	= result[i].Ident[j+1];
		result[i].Pdiff[j]	= result[i].Pdiff[j+1];
		result[i].Ddiff[j]	= result[i].Ddiff[j+1];
        j++;
    }
    result[i].device[RESULT_NUM - 1] = 0;
    result[i].P[RESULT_NUM - 1]      = 0;
    result[i].Pused[RESULT_NUM - 1]  = 0;
	result[i].Ident[RESULT_NUM-1]    = 0;
	result[i].Pdiff[RESULT_NUM-1]	= 0;
	result[i].Ddiff[RESULT_NUM-1]	= 0;
    if (device >= 22 && device <= 25) {
        unknownR[phase_index][device - 22].P      = 0;
        unknownR[phase_index][device - 22].during = 0;
    }
    if (device == 1) {
        dpzrflag = 0;
    }
}
/*****************************************************************************
 函 数 名  : update_device
 功能描述  : 更新已识别设备的功率、功耗
输入参数  : result[i],第i类的输出，diff：包含参数增量，device：待合并的设备,onoff:启/停标识
 输出参数  : result[i]
 返 回 值  :
*****************************************************************************/
void update_device(struct result_typeDef* result, struct steady_typeDef diff, uint16_t t, uint8_t device, uint8_t i,uint8_t onoff)
{
	if (onoff == 1) 
	{
		uint8_t j = 0;
		while (j < RESULT_NUM) 
		{
			if (result[i].device[j] == device) 
			{
				break;
			}
			j++;
		}
		if (diff.P > 0) 
		{
			result[i].P[j] += diff.P;
			result[i].Pused[j] += diff.P * t;
		}
	} 
	else if (onoff == 0) 
	{
		uint8_t dev_position = 0;
		if (!result[i].device[1])  //如果只有一个设备开着，更新其参数
		{
			if (result[i].P[0] > diff.P && result[i].Pused[0] > (diff.P * t)) 
			{
				result[i].P[0] -= diff.P;
				result[i].Pused[0] -= diff.P * t;
			}
		} 
		else if (inarr_u8(result[i].device, 2, RESULT_NUM) >= 0) 
		{
			if (result[i].P[dev_position] > diff.P && result[i].Pused[dev_position] > (diff.P * t)) 
			{
				dev_position = inarr_u8(result[i].device, 2, RESULT_NUM);
				result[i].P[dev_position] -= diff.P;
				result[i].Pused[dev_position] -= diff.P * t;
			}
		} 
		else if (inarr_u8(result[i].device, 1, RESULT_NUM) >= 0) 
		{
			if (result[i].P[dev_position] > diff.P && result[i].Pused[dev_position] > (diff.P * t)) 
			{
				dev_position = inarr_u8(result[i].device, 1, RESULT_NUM);
				result[i].P[dev_position] -= diff.P;
				result[i].Pused[dev_position] -= diff.P * t;
			}
		}
	} 
	else if (onoff == 2) 
	{
		result[i].P[0]     = diff.P;
		result[i].Pused[0] = diff.Praw * t;
	}
}
/*****************************************************************************
 函 数 名  : switch_union
 功能描述  :
将启动设备关闭时的合集赋予启动设备的启动状态。有配对的频繁启停设备，若停里有一个设备，则启里也要有 输入参数  :
所有类clusters，结果result，类数目n， 输出参数  : result[i] 返 回 值  :
*****************************************************************************/
void switch_union(struct cluster_typeDef* clusters, struct result_typeDef* result, uint8_t n)
{
	for (int8_t i = n - 1; i >= 0; i--)  //这里改成倒序的，如110
	{
		if (clusters[i].pair) 
		{
			int P_sum = 0;
			for (uint8_t j = 0; j < RESULT_NUM; j++) 
			{
				P_sum = P_sum + result[i].P[j];
			}
			for (uint8_t j = 0; j < RESULT_NUM; j++) 
			{
				if ((inarr_u8(result[i].device, result[i + clusters[i].pair].device[j], RESULT_NUM) < 0) &&
					(abs(clusters[i].Praw - P_sum - clusters[i + clusters[i].pair].Praw) < 100 ||
						(result[i + clusters[i].pair].device[j] == 2 &&
						(abs(clusters[i].Praw - P_sum - clusters[i + clusters[i].pair].Praw) <
						200))))  // pair停里的result在启动的result不存在
				{
					#if(USE_PRINTF == 1)
					if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
					{
						rt_kprintf("org_csv_test\n");
					}
					#endif
					if (result[i + clusters[i].pair].device[j] >= 22 &&
						result[i + clusters[i].pair].device[j] <= 25)  // 根据运行时间区分热水器等
					{
						add_device(
							result, result[i + clusters[i].pair].device[j], i,
							result[i + clusters[i].pair].P[j], result[i + clusters[i].pair].P[j],
							result[i + clusters[i].pair].P[j] * clusters[i].loc,
                            unknownR[phase_index][result[i + clusters[i].pair].device[j] - 22].during + clusters[i].loc);
					} 
					else 
					{
						add_device(result, result[i + clusters[i].pair].device[j], i,
									result[i + clusters[i].pair].P[j], result[i + clusters[i].pair].P[j],
									result[i + clusters[i].pair].P[j] * clusters[i].loc, clusters[i].loc);//loc就是当前类别中包含的点数，也是包含的s数
					}
				}
			}
		}
	}
}
/*****************************************************************************
 函 数 名  : del_add_device
 功能描述  : 有时结果中的电量会超过这段聚类的总电量，需要删除掉与超出电量相近的设备
 输入参数  : 所有类clusters，结果result，类数目n，
 输出参数  : result[i]
 返 回 值  :
*****************************************************************************/
void del_add_device(struct cluster_typeDef* clusters, struct result_typeDef* result, uint8_t n)
{
    for (uint8_t i = 0; i < n; i++) {
        uint8_t flag_del_dev = 1;
        uint8_t count        = 0;

        while (flag_del_dev && count < 20) {
            count++;
            int P_sum = 0;
            for (uint8_t j = 0; j < RESULT_NUM; j++) {
                P_sum = P_sum + result[i].P[j];
            }

            if ((P_sum - clusters[i].Praw > 250) && (P_sum - clusters[i].Psteady > 250))
            // if ((P_sum - clusters[i].Praw > 250)&&(P_sum - clusters[i].Psteady > 250))
            // //（结果中的功率值之和-当前总功率）过大
            {
                //找到与超出功率最相近的设备。并删除
                short mindiff = 10000;
                uint8_t    device  = 0;
                short p;
                for (uint8_t j = 0; j < RESULT_NUM; j++) {
                    if (abs_short(result[i].P[j] - (P_sum - clusters[i].Praw)) < mindiff &&
                        result[i].device[j] != 0) {
                        mindiff = abs_short(result[i].P[j] - (P_sum - clusters[i].Praw));
                        device  = result[i].device[j];
                        p       = result[i].P[j];
                    }
                }

                if (abs(p - (P_sum - clusters[i].Praw)) < 300)  //(待删除设备功率-超出功率)<300才删除
                {
					#if(USE_PRINTF == 1)
					if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
					{
						rt_kprintf("\norg_csv:result-%d remove device-%d P:%d,diff:%d\n", i, device, p,
							(P_sum - clusters[i].Praw));
					}
					#endif
					remove_device(result, device, i);
				} 
				else 
				{
					#if(USE_PRINTF == 1)
					if(((UsePrintfFlag >> 3) & 0x0001) == 0x0001)
					{
						rt_kprintf("\norg_csv:pover%d remove device-%d P:%d,diff:%d\n", i, device, p,
								(P_sum - clusters[i].Praw));
					}
					#endif
					break;
				}

			} 
			else
				flag_del_dev = 0;
		}
	}
}
/*****************************************************************************
 函 数 名  : csumcal
 功能描述  : 计算差分特征时需要的累加值
 输入参数  : 全部特征值，时间数组，时间数组长度（为负则向前按s取值）
 输出参数  : result[i]
 返 回 值  :
*****************************************************************************/
void csum_cal(struct features_typeDef* feats, uint16_t* t, short length, struct feature_typeDef* csum)
{
    uint16_t temp = 0;
    for (uint16_t i = 0; i < abs_short(length); i++) {
        if (length > 0)
            temp = t[i];
        else
            temp =
                t[0] - i -
                2;  //换成-2了,过渡区的值可能即不属于第一类，也不属于第二类，要去掉（如51,52,53,2100,2400，2401,2402）
        if (type1 == 3)
            temp = t[0] + cur_len - i - 2;
        csum->P += feats->P[temp];
        csum->Q += feats->Q[temp];
        csum->Qfft += feats->Qfft[temp];
        csum->D += feats->D[temp];
        if (i < 20) {
            csum->Pmax = max_short(csum->Pmax, feats->Pmax[temp]);
        }
        csum->Praw += feats->Praw[temp];
        csum->i50 += feats->i50[temp >> 1];
        csum->i150 += feats->i150[temp >> 1];
        csum->i250 += feats->i250[temp >> 1];
        csum->phi50 += feats->phi50[temp >> 1];
        csum->phi150 += feats->phi150[temp >> 1];
        csum->phi250 += feats->phi250[temp >> 1];
    }
}

/*****************************************************************************
 函 数 名  : recunknownR
 功能描述  : 看未知R类是否达到识别的条件了
 输入参数  : result 输出结果,当前结果序号N,本次要判别的UnknownR的序号n
 输出参数  : result[i]
 返 回 值  :
*****************************************************************************/
void recunknownR(struct result_typeDef* result, uint8_t N, uint8_t n)
{
    uint8_t device = 0;
    if (unknownR[phase_index][n].P > 1600 && unknownR[phase_index][n].during > 520)  // 高功率时间够长，认为是热水器
    {
        device             = 3;
        unknownR[phase_index][n].P      = 0;
        unknownR[phase_index][n].during = 0;
    } else if (unknownR[phase_index][n].P < 1201 && unknownR[phase_index][n].P > 1200 &&
               unknownR[phase_index][n].during > 2000)  //中高功率时间够长，认为是电暖器
    {
        device             = 15;
        unknownR[phase_index][n].P      = 0;
        unknownR[phase_index][n].during = 0;
    } else if (unknownR[phase_index][n].P < 950 &&
               unknownR[phase_index][n].P > 700)  //中功率且附近有电饭煲，且二者功率接近，认为是电饭煲
    {
        for (uint8_t i = 0; i < N; i++) {
            if (inarr_u8(result[i].device, 5, RESULT_NUM) >= 0) {
                device             = 5;
                unknownR[phase_index][n].P      = 0;
                unknownR[phase_index][n].during = 0;
                break;
            }
        }
    }
    if (device) {
        for (uint8_t i = 0; i < N; i++) {
            for (uint8_t j = 0; j < RESULT_NUM; j++) {
                if (result[i].device[j] == n + 22) {
                    result[i].device[j] = device;
                    break;
                }
            }
        }
    }
}

/*****************************************************************************
 函 数 名  : update_recunknownR
 功能描述  : 更新unknownR时长，并rec
 输入参数  : result 输出结果,clusters,当前结果i,总clusters数n_cluster
 输出参数  : result[i]
 返 回 值  :
*****************************************************************************/
void update_recunknownR(struct result_typeDef* result, struct cluster_typeDef* clusters, uint8_t i, uint8_t n_cluster)
{
    for (uint8_t j = 0; j < RESULT_NUM; j++)  //根据当前类的时间重新计算功耗
    {
        if (result[i].device[j]) {
            if (result[i].device[j] >= 22 && result[i].device[j] <= 25)  //根据运行时间区分热水器等
            {
                unknownR[phase_index][result[i].device[j] - 22].during += clusters[i].loc;
                recunknownR(result, n_cluster, result[i].device[j] - 22);
            }
        } else {
            break;
        }
    }
}
/*****************************************************************************
 函 数 名  : startendtime
 功能描述  : 记录设备启停事件，create frozen的时候用
 输入参数  : dev_count,dev_output,clusters,start[],end[]
 输出参数  : start end
 返 回 值  :
*****************************************************************************/
void startendtime(uint8_t dev_count, uint8_t* dev_output, uint8_t n_cluster, struct cluster_typeDef* clusters,
                  struct result_typeDef* result, uint8_t* dev_start, uint8_t* dev_end, uint8_t* dev_switch)
{
	float Pdiff[RESULT_NUM], Ddiff[RESULT_NUM];
	float Pend = 0, Dend = 0;
	BYTE i = 0 ,j = 0,k = 0,m = 0;
	SWORD startdiff = 0;
	BYTE Startahead = 0;//代表前多少个设备是被上一个周期继承来的，不用匹配开始时间
	BYTE Endtmp = 0;
	BYTE IsMatch[RESULT_NUM];//i匹配到了哪个j
	BYTE open[254] = {0};
	BYTE DevID[RESULT_NUM]={0};
	float Pd[256],Dd[256];//用来存储标志为x的电器的开启后有功和畸变功率

	memset((BYTE*)&Pdiff[0],0,sizeof(Pdiff));
	memset((BYTE*)&Ddiff[0],0,sizeof(Ddiff));
	memset((BYTE*)&Pd[0],0,sizeof(Pd));
	memset((BYTE*)&Dd[0],0,sizeof(Dd));
	memset((BYTE*)&IsMatch[0],0xFF,sizeof(IsMatch));

	//用来标记该cluster是否已正确匹配到某设备的关闭的标志，清零
	for(k=0; k<n_cluster; k++) 
	{
		clusters[k].flag &= 0xF7;//0b11110111，第四位置0
	}
	//给每一个设备启动时刻匹配顺序
	for(j=0; j<(n_cluster-1); j++)
	{
		for(i=0; i<RESULT_NUM; i++)
		{
			if(result[j].device[i] == 0)
			{
				if(result[j+1].device[i] != 0)
				{
					open[j] = m;
					m++;
				}
				break;
			}
		}
	}
	//用来统计result[0]里面的非零结果个数
	for(k=0; k<RESULT_NUM; k++) 
	{
		if((result[0].device[k] != 0))
		{
			Startahead++;
		}
		else
		{
			break;
		}
	}

	for ( i = 0; i < dev_count; i++) 
	{
		for ( j = 0; j < (n_cluster - 1); j++) 
		{	
			//rt_kprintf("i %d,j %d,n1 %d,n2 %d\n",inarr_u8_count(result[j].device, dev_output[i], RESULT_NUM),inarr_u8_count(result[j + 1].device, dev_output[i], RESULT_NUM));
			startdiff = inarr_u8_count(result[j + 1].device, dev_output[i], RESULT_NUM) - inarr_u8_count(result[j].device, dev_output[i], RESULT_NUM);
			//记录继承来的电器参数
			if(i < Startahead)
			{
				Pdiff[i] = result[0].Pdiff[i];
				Ddiff[i] = result[0].Ddiff[i];
				DevID[i] = i;
			}
			//开启的处理
			if (startdiff > 0) 
			{
				//继承来的电器不处理开启，开启时间是0xFF
				if(i < Startahead)
				{
					continue;
				}
				//j<i可以不判断，否则影响电动车的开启时间,会被刷成最小值
				if((open[j]+Startahead) < i)//当前j开启的电器时刻要在i的后面
				{
					continue;
				}

				if(dev_output[i] == 0x13)//电动车的话
				{	
					if (dev_start[i] == 0) 
					{
						dev_start[i] = 1 + (uint8_t)((clusters[j + 1].t[0] + 900 - tlength) / 60);
						//启动这里需要加1，合并启停数就是启动+1，关闭+1
						dev_switch[i] += 1;
						//用来匹配结束功率
						Pdiff[i] = (clusters[j + 1].P - clusters[j].P); //后减前
						Ddiff[i] = (clusters[j + 1].D - clusters[j].D);
						//如果该电器被继承到下一个周期，用来在下一个周期匹配结束功率
						for(k=1; k<RESULT_NUM; k++)
						{
							if(result[j+1].device[k] == 0)
							{
								break;
							}
						}
						DevID[i] = result[j+1].Ident[k-1];//存下来当前设备i的标志序号
						Pd[DevID[i]] = Pdiff[i];
						Dd[DevID[i]] = Ddiff[i];

					} 
					else 
					{
						//多个电动车情况下，如果所有电动车的时间最小值和已知相等，那么合并启停数没必要再加一，电动车不是多状态，只会启动一次。
						if((dev_start[i] == min_u8(dev_start[i], 1 + (uint8_t)((clusters[j + 1].t[0] + 900 - tlength) / 60))))
						{
							dev_start[i] = min_u8(dev_start[i], 1 + (uint8_t)((clusters[j + 1].t[0] + 900 - tlength) / 60));
						}

					}

				}
				else//其他电器不变,因为其他电器有可能是多状态，但是电动车开启就一个状态
				{					
					if (dev_start[i] == 0) 
					{
						//用来匹配EBResult和dev_output中的电动车
						dev_start[i] = 1 + (uint8_t)((clusters[j + 1].t[0] + 900 - tlength) / 60);
						//启动这里需要加1，合并启停数就是启动+1，关闭+1
						dev_switch[i] += 1;
					} 
					else 
					{
						dev_start[i] =min_u8(dev_start[i], 1 + (uint8_t)((clusters[j + 1].t[0] + 900 - tlength) / 60));
					}
				}
			} 
			//停止的处理
			else if (startdiff < 0) 
			{
				if(dev_output[i] == 0x13)//电动车
				{
					if((clusters[j].flag & 0x08) == 0x08) //判断当前j是否被匹配过
					{
						continue; 
					}
					
					//计算当前j和j-1的功率差
					Pend = (clusters[j].P - clusters[j+1].P); //前减后
					Dend = (clusters[j].D - clusters[j+1].D);

					//1、单一匹配，如果匹配到，给i的结束时间赋值，匹配到就停止
					//功率满足范围 或者 总功率小于EB_CLOSE_MINDEFFw任务所有电器都已经关闭
					if((fabs(Pend - Pdiff[i]) < DERTA_P) && (fabs(Dend - Ddiff[i]) < DERTA_D) && (clusters[j+1].P >= NILM_POWER_MIN))
					{
						Endtmp = 1 + (uint8_t)((clusters[j].t[clusters[j].loc - 1] + 900 - tlength) / 60);
						if(dev_end[i] <= Endtmp)
						{
							dev_end[i] = Endtmp;
							IsMatch[i] = j;
						}
					}	
					//2、如果匹配到多个之和,暂不考虑，需要暴力搜索或动态规划出非空子集！！！！！！

					//3、总功率小于50的时候
					if(clusters[j+1].P < NILM_POWER_MIN)
					{
						if((Pdiff[i]==0) &&(Ddiff[i] == 0))
						{
							continue;
						}
						if(dev_end[i] == 0) //前面的状态j都没有匹配到结束
						{
							dev_end[i]= 1 + (uint8_t)((clusters[j].t[clusters[j].loc - 1] + 900 - tlength) / 60);
							IsMatch[i] = 0xFE;
							break;
						}
					}

					
				}
				else//其他电器不变
				{		
					dev_end[i] = max_u8(dev_end[i], 1 + (uint8_t)((clusters[j].t[clusters[j].loc - 1] + 900 - tlength) / 60));
					dev_switch[i] += 1;	
				}
			}

			//把需要继承的设备的设备开启畸变和功率继承
			if(j == (n_cluster-2))
			{
				for(k=0; k<RESULT_NUM; k++)
				{
					if(result[j+1].device[k] == 0)
					{
						break;
					}
					if(result[j+1].Ident[k] == DevID[i]) 
					{
						result[j+1].Pdiff[k] = Pd[result[j+1].Ident[k]];
						result[j+1].Ddiff[k] = Dd[result[j+1].Ident[k]];
						break;
					}
				}
			}
		}
	
		//统一处理关闭的启停数
		if(dev_output[i] == 0x13)
		{	
			if(IsMatch[i] == 0xFE) //如果是FE代表小于50w
			{
				dev_switch[i] += 1;
			}
			else if(IsMatch[i] == 0xFF)//说明当前设备i没匹配上结束时间
			{	
				dev_end[i] = 0;
			}
			else//功率匹配关闭
			{
				clusters[IsMatch[i]].flag |= 0x08;
				dev_switch[i] += 1;
			}
		}
	}

	//用来标记该cluster是否已正确匹配到某设备的关闭的标志，清零
	for(k=0; k<n_cluster; k++) 
	{
		clusters[k].flag &= 0xF7;//0b11110111，第四位置0
	}

	//电动车的特殊处理 2 ：特殊处理1加强电动车识别，然后nilm计算启停时间，最后nilm的启停时间和EBResult比较
	#if(USE_EBIDF)
	//处理开启
	//刷新倒计时
	for(j = 0; j<dev_count; j++)//j用来表示当前周期识别出的设备的序号
	{
		for(m=0; m<RESULT_NUM; m++)//m用来表示最后一个类中已有设备的序号
		{
			//处理延续，如果最后一个状态的设备都是0，那就是没有设备需要延续
			if(result[n_cluster-1].device[m] == 0x00)
			{
				break;
			}
			for(i = 0; i < (EB_RESULT_NUM); i++)
			{
				if( result[n_cluster-1].Ident[m] == DevID[j])
				{
					if((((EBResult[i].TimeToClose[phase_index] > 0) && (EBResult[i].TimeToClose[phase_index] < EB_FLAG_COUNT))
					&&(fabs(EBResult[i].PMAX_PHASE[phase_index] - result[n_cluster-1].Pdiff[m]) < EB_MINPDIFF)
					&&(fabs(EBResult[i].DMAX_PHASE[phase_index] - result[n_cluster-1].Ddiff[m]) < EB_MINDDIFF)
					&&((abs(EBResult[i].RevTime[phase_index] - dev_start[j]) == 0) || (dev_start[j] == 0x00)))
					|| (AbnormFlag[phase_index] == 0xAA)) //第一个条件防止重复进入
					{
						EBResult[i].TimeToClose[phase_index] = EB_FLAG_COUNT;
						break;
					}
				}
			}
		}
	}
	//结果修正
	for(j=0; j<dev_count; j++)
	{
		if(dev_output[j] != 0x13)
		{	
			for(i = 0; i < EB_RESULT_NUM; i++)
			{
				if(((EBResult[i].TimeToClose[phase_index] >= 0) //必须大于等于零防止20到80之间产生了冻结，时间还没赋值
				&& (fabs(Pdiff[j]-EBResult[i].PMAX_PHASE[phase_index]) < EB_MINPDIFF)
				&& (fabs(Ddiff[j]-EBResult[i].DMAX_PHASE[phase_index]) < EB_MINDDIFF)
				&& ((abs(dev_start[j]-EBResult[i].RevTime[phase_index]) == 0) || (dev_start[j] == 0x00)))
				|| (AbnormFlag[phase_index] == 0xAA)) //第一个条件判断大于0即可
				{
					dev_output[j] = 0x13;
					EBResult[i].TimeToClose[phase_index] = EB_FLAG_COUNT;
					for(m=0; m<RESULT_NUM; m++)
					{
						if(result[n_cluster-1].device[m] == 0x00)
						{
							break;
						}
						//修正后，最后一个类别也需要修正，才能继承下一个周期
						if(DevID[j] == result[n_cluster-1].Ident[m])
						{
							result[n_cluster-1].device[m] = 0x13;
						}
					}
				}
			}
		}
	}

	//处理结束
	//匹配到后清零某一项
	for(j=0; j<dev_count; j++)
	{
		if(dev_end[j] != 0x00)
		{	
			for(i = 0; i<EB_RESULT_NUM; i++)
			{
				if(((EBResult[i].TimeToClose[phase_index] > 0)
				&& (fabs(Pdiff[j]-EBResult[i].PMAX_PHASE[phase_index]) < EB_MINPDIFF)
				&& (fabs(Ddiff[j]-EBResult[i].DMAX_PHASE[phase_index]) < EB_MINDDIFF)
				&& ((abs(dev_start[j]-EBResult[i].RevTime[phase_index])== 0) || (dev_start[j] == 0x00)))
				|| (AbnormFlag[phase_index] == 0xAA))
				{
					api_EBResDeleteIndex(phase_index,i);
					break;
				}
			}
		}
	}
	//功率小于50w，清全部
	for(i = 0; i < (EB_RESULT_NUM); i++)
	{
		if((EBResult[i].TimeToClose[phase_index] > 0) 
		&& ((features[phase_index].P[tlength-1] < NILM_POWER_MIN) && (tlength!=0)))//当前功率小于50w，意味着电器关闭
		{
			api_EBResDeleteIndex(phase_index,EB_RESULT_NUM); //小于50可，清除当前相的全部数据
			break;
		}
	}


	//清除电动车特殊处理1的IsCompared标志
	for(i = 0; i < (EB_RESULT_NUM); i++)
	{
		EBResult[i].IsCompared[phase_index] = 0;
	}

	#endif//#if(USE_EBIDF)
}

/*****************************************************************************
 函 数 名  : creat_frozen
 功能描述  : 看未知R类是否达到识别的条件了
 输入参数  : result 输出结果,当前结果序号N,本次要判别的UnknownR的序号n
 输出参数  : result[i]
 返 回 值  :
*****************************************************************************/
void creat_frozen(frozen_typeDef *frozentmp,uint8_t dev_count, uint8_t* dev_output, float* pused_output, short* p_output, uint8_t* dev_start,
                  uint8_t* dev_end, uint8_t* dev_switch)
{

    memset(frozentmp, 0, sizeof(frozen_typeDef));
    uint8_t newid[20] = {
        0,  1, 2,  3,  4, 5,  8,  9,  34, 35, 37,
        33, 6, 36, 10, 7, 12, 39, 11, 49};  //将旧ID（excel版）映射到新ID（word版）,word版没有的饮水机定为12，电动车（大功率充电）定为49，洗碗机39
    uint8_t i = 0;

    while (i < dev_count) {
        frozentmp->sindev[i].pused = (uint32_t)(10000 * pused_output[i]);
        frozentmp->sindev[i].p     = abs_short(p_output[i]) * 10;
//        if (dev_output[i] >= 22 && dev_output[i] <= 25) {
            frozentmp->sindev[i].start       = dev_start[i];
            frozentmp->sindev[i].end         = dev_end[i];
            frozentmp->sindev[i].switchtimes = dev_switch[i];
            if (frozentmp->sindev[i].start == 0)
                frozentmp->sindev[i].start = 0xFF;
            if (frozentmp->sindev[i].end == 0)
                frozentmp->sindev[i].end = 0xFF;
            if (frozentmp->sindev[i].end != 0xFF && frozentmp->sindev[i].switchtimes == 0)
                frozentmp->sindev[i].switchtimes = 1;


        if (dev_output[i] >= 22 && dev_output[i] <= 25)
            frozentmp->sindev[i].device = 0xA100;  //+dev_output[i]-22;
        else if (dev_output[i] == 21)
            frozentmp->sindev[i].device = 0xE100;
        else if (dev_output[i] < 20) {
            frozentmp->sindev[i].device = newid[dev_output[i]] * 256;
            //同类设备开启多个，给不同代码
            /*if (inarr_u8(dev_output,dev_output[i],i)>=0)
            {
              frozentmp->sindev[i].device=newid[dev_output[i]]*256+1;
            }*/
			//电动车需要判断开启时间是否够长,如果有电动车专用算法，那么屏蔽该分支
			#if(USE_EBIDF == 0)
			if (dev_output[i] == 19) 
			{
				if ((pused_output[i] * 3600000 / p_output[i]) < 800 && p_output[i] < 30000)  //用功耗除以功率计算时间
				{
					frozen[phase_index].sindev[i].device = 0xB100;
				}
			}
			#endif
        }
        //给设备编号
        for (uint8_t j = 0; j < devidnum[phase_index]; j++) {
            if ((frozentmp->sindev[i].device >> 8) == DevidRecord[phase_index].appid[j])//已有电器编号
            {
                if (abs_short(p_output[i] - DevidRecord[phase_index].devp[j]) < 20) {
                    frozentmp->sindev[i].device = frozentmp->sindev[i].device + j + phase_index;
                    break;
                }
            }
            if (j == devidnum[phase_index] - 1) //新电器编号
            {
                frozentmp->sindev[i].device   = frozentmp->sindev[i].device + j + phase_index*30;//每相预留30个编号
                DevidRecord[phase_index].appid[devidnum[phase_index] - 1] = frozentmp->sindev[i].device >> 8;
                DevidRecord[phase_index].devp[devidnum[phase_index] - 1]  = p_output[i];
                if (devidnum[phase_index] >= KNOWNNUM - 1) {
                    devidnum[phase_index] = 1;
                } else {
                    devidnum[phase_index]++;
                }
                break;
            }
        }

        i += 1;
    }
    frozentmp->num = dev_count;
    // GetTime(&frozentmp->time);


    memcpy(&frozentmp->time, &time_15min, sizeof(date_time_s));
}



