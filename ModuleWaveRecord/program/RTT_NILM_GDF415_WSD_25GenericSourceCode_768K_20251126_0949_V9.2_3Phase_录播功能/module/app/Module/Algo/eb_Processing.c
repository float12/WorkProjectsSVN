//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "arm_math.h"
#include "math.h"
#if(USE_EBIDF)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define EB_WAITSEC					60 				//持续60s判断电动车未关闭
#define EB_CLOSE_MINDEFF			10				//P,D的减少与电器开启判断的P,D增加的偏差
#define ABEB_MEANDIFF_P 			50				//连续两个1分钟数据的平均功率之差
#define ABEB_FREQ 					(50.0f) 		//监测电动车充电波形的频率
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)
typedef struct TPeaks_t
{
	float Value;
	BYTE  Index;
}TPeaks;
#pragma pack()
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
struct features1min_djc_typeDef features_djc_latest1min[MAX_PHASE] = {0};
TEBRES EBResult[EB_RESULT_NUM]; //电动车识别结果
TABNORMEBRES AbnormEBResult;//异常电动车识别结果
BYTE EBCurrentIndex[MAX_PHASE]; //电动车当前存储位置
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
BYTE SpecEBFlag = 0; //特殊形状电动车的标志
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  电动车结果初始化
//         
//参数:      Phase == 0/1/2,分别初始化单一相
//			 phase == 其他，初始化所有相
//          Num 是第几个，Num是最大值时候清除全部
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  EBResultInit(BYTE Phase,BYTE Num)
{
	BYTE i = 0;
	
	if(Phase < MAX_PHASE) //单相处理0，三相处理0,1,2
	{
		if(Num < EB_RESULT_NUM)
		{
			memset((BYTE*)&EBResult[Num].RevTime[Phase],0xFE,sizeof(BYTE));
			memset((BYTE*)&EBResult[Num].RevSec[Phase],0,sizeof(DWORD));
			memset((BYTE*)&EBResult[Num].PMAX_PHASE[Phase],0,sizeof(float));
			memset((BYTE*)&EBResult[Num].DMAX_PHASE[Phase],0,sizeof(float));
			memset((BYTE*)&EBResult[Num].BeforPower[Phase],0,sizeof(float));
			memset((BYTE*)&EBResult[Num].AverPower[Phase],0,sizeof(float));
			memset((BYTE*)&EBResult[Num].AverCount[Phase],0,sizeof(BYTE));
			memset((BYTE*)&EBResult[Num].TimeToClose[Phase],0,sizeof(WORD));
			memset((BYTE*)&EBResult[Num].EBResultTimes[Phase],0,sizeof(BYTE));
			memset((BYTE*)&EBResult[Num].IsCompared[Phase],0,sizeof(BYTE));
			memset((BYTE*)&EBResult[Num].EBWaitforReport[Phase],0,sizeof(DWORD));
			memset((BYTE*)&EBResult[Num].EBWaitforReportP[Phase],0,sizeof(float));
			memset((BYTE*)&EBResult[Num].EBWaitforReportD[Phase],0,sizeof(float));
		}
		else //清除当前相全部
		{
			for(i = 0; i < EB_RESULT_NUM; i++)
			{
				memset((BYTE*)&EBResult[i].RevTime[Phase],0xFE,sizeof(BYTE));
				memset((BYTE*)&EBResult[i].RevSec[Phase],0,sizeof(DWORD));
				memset((BYTE*)&EBResult[i].PMAX_PHASE[Phase],0,sizeof(float));
				memset((BYTE*)&EBResult[i].DMAX_PHASE[Phase],0,sizeof(float));
				memset((BYTE*)&EBResult[i].BeforPower[Phase],0,sizeof(float));
				memset((BYTE*)&EBResult[i].AverPower[Phase],0,sizeof(float));
				memset((BYTE*)&EBResult[i].AverCount[Phase],0,sizeof(BYTE));
				memset((BYTE*)&EBResult[i].TimeToClose[Phase],0,sizeof(WORD));
				memset((BYTE*)&EBResult[i].EBResultTimes[Phase],0,sizeof(BYTE));
				memset((BYTE*)&EBResult[i].IsCompared[Phase],0,sizeof(BYTE));
				memset((BYTE*)&EBResult[i].EBWaitforReport[Phase],0,sizeof(DWORD));
				memset((BYTE*)&EBResult[i].EBWaitforReportP[Phase],0,sizeof(float));
				memset((BYTE*)&EBResult[i].EBWaitforReportD[Phase],0,sizeof(float));
			}
		}
	}
	else //清除全部相
	{
		memset((BYTE*)&EBResult[0].RevTime[0],0,sizeof(TEBRES)*EB_RESULT_NUM);
		for(i = 0; i < EB_RESULT_NUM; i++)
		{
			memset((BYTE*)&EBResult[i].RevTime[0],0xFE,sizeof(BYTE)*MAX_PHASE);
		}
	}
}
//--------------------------------------------------
//功能描述:  电动车标志存储处理
//         
//参数:      Tpye = 0是添加，等于1是删除
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_EBResAddIndex(BYTE Phase)
{
	EBCurrentIndex[Phase] += 1;
	if (EBCurrentIndex[Phase] > (EB_RESULT_NUM-1))
	{
		EBCurrentIndex[Phase] -= 1;
		memcpy(&EBResult[0].RevTime[Phase],&EBResult[1].RevTime[Phase],EBCurrentIndex[Phase]*sizeof(TEBRES));
		EBResultInit(Phase,EB_RESULT_NUM-1); //删除最后一个
	}
}

//--------------------------------------------------
//功能描述:  电动车标志删除处理
//         
//参数:      Phase == 0/1/2,分别初始化单一相
//			 phase == 其他，初始化所有相
//          Num 是第几个，Num是最大值时候清除全部
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_EBResDeleteIndex( BYTE Phase,BYTE Num )
{
    BYTE i = 0,j = 0;
    
	if(Phase < MAX_PHASE) //单相处理0，三相处理0,1,2
	{
		if(Num < (EB_RESULT_NUM -1))
		{
			if (EBCurrentIndex[Phase] > 0)
			{
				memcpy(&EBResult[Num].RevTime[Phase],&EBResult[Num+1].RevTime[Phase],(EB_RESULT_NUM - Num - 1)*sizeof(TEBRES));
				EBResultInit(Phase,(EB_RESULT_NUM-1));
				EBCurrentIndex[Phase] -=1;
			}
		}
		else if(Num == (EB_RESULT_NUM -1))
		{
			if (EBCurrentIndex[Phase] > 0)
			{
				EBResultInit(Phase,(EB_RESULT_NUM-1));
				EBCurrentIndex[Phase] -=1;
			}
		}
		else//清除当前相
		{
			EBCurrentIndex[Phase] = 0;
			EBResultInit(Phase,EB_RESULT_NUM);
		}
	}
	else//清除全部相
	{
		memset(&EBCurrentIndex[0],0,sizeof(EBCurrentIndex));
		EBResultInit(MAX_PHASE,EB_RESULT_NUM);
	}

	for(i = 0; i< MAX_PHASE; i++)
	{
		for (j=0; j < EB_RESULT_NUM; j++)
		{
			rt_kprintf("Rev:%d,Sec:%d\n",EBResult[j].RevTime[i],EBResult[j].RevSec[i]);
		}
	}
}
//--------------------------------------------------
//功能描述:  电动车确认函数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE EBConfirm(BYTE tmpi, struct features1min_djc_typeDef *latest1min,BYTE Phase)
{
	TRealTimer tTime;
	float sump3s_old = 0,sump3s_new = 0,sumd3s_old = 0,sumd3s_new = 0;
	float sump19s_old = 0,sump19s_new = 0, sumd19s_old = 0, sumd19s_new=0;
	float sump19s = 0;//用于计算19s的平均功率

	get_sys_time(&tTime);

	for (rt_uint8_t i=36; i<39; i++)
	{
		sump3s_old = sump3s_old + latest1min->P[i];
		sumd3s_old = sumd3s_old + latest1min->D[i];
	}
	for (rt_uint8_t i=41; i<44; i++)
	{
		sump3s_new = sump3s_new + latest1min->P[i];
		sumd3s_new = sumd3s_new + latest1min->D[i];
	}
	for (rt_uint8_t i=20; i<39; i++)
	{
		sump19s_old = sump19s_old + latest1min->P[i];
		sumd19s_old = sumd19s_old + latest1min->D[i];
	}
	for (rt_uint8_t i=41; i<60; i++)
	{
		sump19s_new = sump19s_new + latest1min->P[i];
		sumd19s_new = sumd19s_new + latest1min->D[i];
		sump19s = sump19s + (latest1min->P[i] - sump3s_old/3);
	}
	//rt_kprintf("s:%f,%f,%f,%f,%f,%f\n",sump3s_new,sump3s_old,sump19s_new,sump19s_old,sumd19s_new,sumd19s_old);
	
	//电动车开启前后功率平均值是否大于等于50
	//电动车开启后19s,畸变功率大于等于电动车开启前后功率的一半
	if((((sump3s_new-sump3s_old)/3 >= 50) && (((sump19s_new-sump19s_old)*0.5) <= (sumd19s_new-sumd19s_old))) 
	|| (SpecEBFlag == 0xBB))
	{
		if(SpecEBFlag == 0xBB)
		{
			SpecEBFlag = 0;
		}
		rt_kprintf("EB confirmed\r\n");
		EBResult[tmpi].RevTime[Phase] = 1+ ((tTime.Min * EB_WAITSEC + tTime.Sec - EB_DETECT_TIME)/EB_WAITSEC)%15;
		EBResult[tmpi].RevSec[Phase] =  api_CalcInTimeRelativeSec(&tTime);
		EBResult[tmpi].PMAX_PHASE[Phase] = (sump3s_new-sump3s_old)/3;
		EBResult[tmpi].DMAX_PHASE[Phase] = (sumd3s_new-sumd3s_old)/3;
		EBResult[tmpi].BeforPower[Phase] = sump3s_old/3;
		EBResult[tmpi].AverPower[Phase] = sump19s; //用于前19s功率总和统计
		EBResult[tmpi].AverCount[Phase] = EB_DETECT_TIME -1;//记录前19s的时间
		EBResult[tmpi].EBWaitforReport[Phase] = 1;
		EBResult[tmpi].EBWaitforReportD[Phase] = features_djc_latest1min[Phase].D[59];
		EBResult[tmpi].EBWaitforReportP[Phase] = features_djc_latest1min[Phase].P[59];

		AbnormEBResult.StartFlag[Phase] = 0xAA;

		return 1;
	}
	else
	{
		rt_kprintf("EB canceled\r\n");
		return 0;
	}
}

//--------------------------------------------------
//功能描述:  对 sdata[N] 做 0/1 量化并估算平均周期/频率
//         
//参数:      sdata    : 浮点数组
//         	 N        : 数组长度
//返回值:    pFreq    : 平均频率（Hz）
//
//备注:     1 成功； 0 上升沿不足 2 个，无法估算
//--------------------------------------------------
BOOL CalcPeriodFreq(float *sdata, float *pFreq)
{
	float Vmin = sdata[0], Vmax = sdata[0], Vth = 0;
	BYTE PrevBin = 0,CurrBin = 0;
	WORD EdgeCnt = 0;
	BYTE Edges[FEATRUE_SIZE_MINUTE] = {0};
	BYTE i = 0;
	float AvgT = 0.0f, Freq = 0.0f,SumPeriods = 0.0f;
	const float Fs =1000.0f;

	// 1. 找 Vmin/Vmax
	for (i = 1; i < FEATRUE_SIZE_MINUTE; i++) 
	{
		if (sdata[i] < Vmin)
		{
 			Vmin = sdata[i];
		}

		if (sdata[i] > Vmax)
		{
			Vmax = sdata[i];
		} 
	}

	Vth = (Vmin + Vmax) / 2.0f;

	// 2. 0/1 量化 + 上升沿检测
	if(sdata[0] > Vth)
	{
		PrevBin = 1;
	}

	for (i = 1; i < FEATRUE_SIZE_MINUTE; i++) 
	{
		if(sdata[i] > Vth)
		{
			CurrBin = 1;
		}
		else
		{
			CurrBin = 0;
		}
		if (CurrBin > PrevBin) // 0->1 上升沿
		{
			Edges[EdgeCnt] = i;
			EdgeCnt++;
		}
		PrevBin = CurrBin;
	}

	if (EdgeCnt < 2) //上升沿数目小于2 直接返回
	{
		*pFreq  = 0;
		return FALSE;
	}

	// 3. 计算平均周期（点数 → 秒）
	for (i = 1; i < EdgeCnt; i++) 
	{
		SumPeriods += (float)(Edges[i] - Edges[i - 1]);
	}

	AvgT = (float)((SumPeriods / (float)(EdgeCnt - 1)) / Fs); // 秒
	Freq = 1.0f / AvgT; // Hz

	*pFreq = Freq;
	return 1;
}

//--------------------------------------------------
//功能描述:  电动车结果上报、确认是否是电动车
//         
//参数:      
//         
//返回值:    
//         
//备注:  此函数1s一个周期
//--------------------------------------------------
void EBCalculate(void)
{
	BOOL BlRES = FALSE;
	BYTE i = 0,j = 0,k = 0,StartID = 0;
	float CurAverP = 0;
	TRealTimer tTime;
	DWORD dwRevSec = 0;
	float Freq = 0.0f;
	BYTE ConfRes = 0;

	for(i = 0; i< MAX_PHASE; i++)
	{
		for(j = 0; j < (EB_RESULT_NUM); j++) //用for循环是为了怕1分钟有多个结果
		{
			if(EBResult[j].EBWaitforReport[i])
			{
				EBResult[j].EBWaitforReport[i]++;
				if (EBResult[j].EBWaitforReport[i] > EB_WAITSEC)//判是否大于60s，决定上报，如果条件成立，意味着电动车是再1分20s之前加入的电动车
				{
					EBResult[j].TimeToClose[i] = EB_FLAG_COUNT;
					if(EBResult[j].AverCount[i] != 0)
					{
						EBResult[j].AverPower[i] = EBResult[j].AverPower[i]/EBResult[j].AverCount[i];
						EBResult[j].AverCount[i] = 0;
					}
					EBResult[j].EBWaitforReport[i] = 0;
				}
				else
				{
					// rt_kprintf("features%d,%f,%f,%f\n",j,features_djc_latest1min[i].P[59],EBResult[j].BeforPower[i],(features_djc_latest1min[i].P[59] - EBResult[j].BeforPower[i]));
					if(((features_djc_latest1min[i].P[59] - EBResult[j].BeforPower[i]) < ((EBResult[j].PMAX_PHASE[i]) + EB_MINPDIFF)) &&
					((features_djc_latest1min[i].P[59] - EBResult[j].BeforPower[i]) > ((EBResult[j].PMAX_PHASE[i]) - EB_MINPDIFF))) //功率变换差要满足起始功率激增差±30，否则如果60s内有电器开或者电器关，超过这个范围则会影响到功率取平均
					{
						EBResult[j].AverPower[i] += (features_djc_latest1min[i].P[59] - EBResult[j].BeforPower[i]);
						EBResult[j].AverCount[i]++;
					}
				}

				//畸变功率最新值比记录值小50以上，无法确认关闭的是哪一个，只能把当前相全部清除
				if(((features_djc_latest1min[i].D[59] + 50) < EBResult[j].EBWaitforReportD[i])) 
				{
					//1、倒序单一匹配，删除最新的，如果删除导致后续冻结结果出错，可以解释是多个电动车混叠情况下，最新加入的电动车不一定正确识别。
					for(k=EB_RESULT_NUM;k>0;k--)
					{
						//P,D的减少与电器开启判断的P,D增加的偏差小于EB_CLOSE_MINDEFF的记录，进行清除
						if((fabs(fabs(features_djc_latest1min[i].D[59] - EBResult[j].EBWaitforReportD[i]) - EBResult[k-1].DMAX_PHASE[i]) < EB_CLOSE_MINDEFF)
						&& (fabs(fabs(features_djc_latest1min[i].P[59] - EBResult[j].EBWaitforReportP[i]) - EBResult[k-1].PMAX_PHASE[i]) < EB_CLOSE_MINDEFF)
						&& (AbnormEBResult.StartFlag[i] != 0xAA))
						{
							//清除记录,清除当前相的某一条记录,复测，看谁清除！！！！！！
							api_EBResDeleteIndex(i,k-1);
							rt_kprintf("D is Lower\n");
							break;
						}
					}
					
					//2、如果单一没有匹配成功，那么只能通过动态规划或者暴力搜索方法去求得差值对应的非空子集之和
					//考虑复杂度较高，暂不处理,后续优化！！！！！！

					//3、如果匹配完后没有匹配成功，全部清除。经过反复考虑，还是不要清除比较好。
				}
			}
		}
	}

	for(i = 0; i< MAX_PHASE; i++)
	{
		for (j=0; j < EB_RESULT_NUM; j++)
		{
			if (EBResult[j].EBResultTimes[i])
			{
				EBResult[j].EBResultTimes[i] = EBResult[j].EBResultTimes[i] + 1;
				if (EBResult[j].EBResultTimes[i]>=(EB_DETECT_TIME+1))
				{
					rt_kprintf("%d,%d Phase eb_confirming\n",i,j);
					ConfRes = EBConfirm(j,&features_djc_latest1min[i],i);
					EBResult[j].EBResultTimes[i] = 0;
					if(ConfRes == 0)
					{
						api_EBResDeleteIndex(i,j);
						rt_kprintf("Cancel Del Id%d\n",j);
						AbnormEBResult.RemaTime[i] += AbnormEBResult.EndCount[i];
						AbnormEBResult.EndCount[i] = 0;//相当于重新计数，不论AbnormEBResult.StartFlag是什么。
					}
				}
			}
		}
		if(AbnormEBResult.StartFlag[i] == 0xAA) //开启异常电动车检测,不管是第几个j产生了AA，就用一个标志来统计频率，后续确定是异常，就把1分钟之前所有EBResult清除
		{
			AbnormEBResult.EndCount[i] ++; //累计时间，直到60s
			if(AbnormEBResult.EndCount[i] == (FEATRUE_SIZE_MINUTE - EB_DETECT_TIME)) //前面20s已经已经有数据，再记录40s即可
			{
				AbnormEBResult.StartFlag[i] = 0;
				get_sys_time(&tTime);
				dwRevSec = api_CalcInTimeRelativeSec(&tTime);
				AbnormEBResult.EndCount[i] = 0;
				BlRES = CalcPeriodFreq(&features_djc_latest1min[i].P[0],&Freq);
				AbnormEBResult.Freq[i] = Freq;
				if(BlRES == TRUE) //频率计算出来，可能是异常频率
				{
					if(AbnormEBResult.Freq[i] > ABEB_FREQ) //频率大于50，判断是异常脉冲式充电法电动车
					{
						AbnormFlag[i] = 0xAA;
						rt_kprintf("Freq is Abnormal\n");
						//情况1：如果前一次是正常，本次异常，那么保留第一次异常其余删除
						if(AbnormEBResult.LastAbnormFlag[i] == 0)
						{
							AbnormEBResult.LastAbnormFlag[i] = 0xEE;
							CurAverP = arr_mean(&features_djc_latest1min[i].P[0],0,FEATRUE_SIZE_MINUTE);
							rt_kprintf("Diff Power1 %f\n",(CurAverP - AbnormEBResult.LastAverP[i]));
							//寻找第一个电动车标志
							for(k=0; k<EB_RESULT_NUM; k++)
							{
								if((EBResult[k].RevSec[i] >= (dwRevSec - FEATRUE_SIZE_MINUTE - AbnormEBResult.RemaTime[i]))
								&& (EBResult[k].RevSec[i] <= (dwRevSec)))
								{
									StartID = k;
									rt_kprintf("Save Id%d\n",StartID);
									break;
								}
							}
							//仅保留一分钟内的第一个电动车标志,其他清除
							for(k=(StartID+1); k<EB_RESULT_NUM; k++)
							{
								if((EBResult[k].RevSec[i] >= (dwRevSec - FEATRUE_SIZE_MINUTE - AbnormEBResult.RemaTime[i]))
								&& (EBResult[k].RevSec[i] <= (dwRevSec)))
								{
									rt_kprintf("Frist Del Id%d,%d,%d,%d\n",k,EBResult[k].RevSec[i], AbnormEBResult.RemaTime[i],dwRevSec);
									api_EBResDeleteIndex(i,k);
									if(k>0)
									{
										k--; //因为删除了1个，所以k-1，重新遍历
									}
								}
							}
							AbnormEBResult.RemaTime[i] = 0;
							//更新平均功率
							AbnormEBResult.LastAverP[i] = CurAverP;
							rt_kprintf("Del Some Abnormal\n");
						}

						//情况2：如果上一次异常，本次也是异常，那么全部删除。除非是有功率变化，则保留时间内第一个，其余删除
						else if(AbnormEBResult.LastAbnormFlag[i] == 0xEE)
						{
							AbnormEBResult.LastAbnormFlag[i] = 0xEE;
							CurAverP = arr_mean(&features_djc_latest1min[i].P[0],0,FEATRUE_SIZE_MINUTE);
							rt_kprintf("Diff Power2 %f\n",(CurAverP - AbnormEBResult.LastAverP[i]));
							if((CurAverP - AbnormEBResult.LastAverP[i]) > ABEB_MEANDIFF_P) //如果新平均功率大于记录的上一个平均功率50w以上
							{
								//寻找第一个电动车标志
								for(k=0; k<EB_RESULT_NUM; k++)
								{
									if((EBResult[k].RevSec[i] >= (dwRevSec - FEATRUE_SIZE_MINUTE - AbnormEBResult.RemaTime[i]))
									&& (EBResult[k].RevSec[i] <= (dwRevSec)))
									{
										StartID = k;
										rt_kprintf("Large Power Save Id%d\n",StartID);
										break;
									}
								}
								//仅保留一分钟内的第一个电动车标志,其他清除
								for(k=(StartID+1); k<EB_RESULT_NUM; k++)
								{
									rt_kprintf("k:%d,StartID%d\n",k,StartID);
									if((EBResult[k].RevSec[i] >= (dwRevSec - FEATRUE_SIZE_MINUTE - AbnormEBResult.RemaTime[i]))
									&& (EBResult[k].RevSec[i] <= (dwRevSec)))
									{
										rt_kprintf("Secend Del Id%d,%d,%d,%d\n",k,EBResult[k].RevSec[i], AbnormEBResult.RemaTime[i],dwRevSec);
										api_EBResDeleteIndex(i,k);
										if(k>0)
										{
											k--; //因为删除了1个，所以k-1，重新遍历
										}
									}
								}
								AbnormEBResult.RemaTime[i] = 0;
								rt_kprintf("Del Some Abnormal\n");
							}
							else
							{
								//清除一分钟内的所有电动车标志
								for(k=0; k<EB_RESULT_NUM; k++)
								{
									rt_kprintf("k:%d,StartID%d\n",k,StartID);
									if((EBResult[k].RevSec[i] >= (dwRevSec - FEATRUE_SIZE_MINUTE - AbnormEBResult.RemaTime[i]))
									&& (EBResult[k].RevSec[i] <= (dwRevSec)))
									{
										//清除记录，这种情况需要清除率先倒计时结束的数据
										rt_kprintf("Thrid Del Id%d,%d,%d,%d\n",k,EBResult[k].RevSec[i], AbnormEBResult.RemaTime[i],dwRevSec);
										api_EBResDeleteIndex(i,k);
										if(k>0)
										{
											k--; //因为删除了1个，所以k-1，重新遍历
										}
									}
								}
								AbnormEBResult.RemaTime[i] = 0;
								rt_kprintf("Del ALL Abnormal\n");
							}
							
							//更新平均功率
							AbnormEBResult.LastAverP[i] = CurAverP;
						}
					}
					else
					{
						//情况3：如果上一次异常，本次正常，不做处理
						//情况4：如果上一次正常，本次正常，不做处理
						AbnormEBResult.LastAbnormFlag[i] = 0;
					}
				}
				else
				{
					//情况3：如果上一次异常，本次正常，不做处理
					//情况4：如果上一次正常，本次正常，不做处理
					AbnormEBResult.LastAbnormFlag[i] = 0;
				}
			}
		}
	}

	//标志清零倒计时处理
	for(i = 0; i < MAX_PHASE; i++)
	{
		for(j = 0; j < (EB_RESULT_NUM); j++) //用for循环是为了怕1分钟有多个结果
		{
			if(EBResult[j].TimeToClose[i] > 0)
			{
				EBResult[j].TimeToClose[i]--;

				if(EBResult[j].TimeToClose[i] == 0)
				{
					//清除记录，这种情况需要清除率先倒计时结束的数据
					api_EBResDeleteIndex(i,j);
					rt_kprintf("TimeToClose is Zero\n");
				}
			}
		}
	}
}

//--------------------------------------------------
//功能描述:   一维数组寻找局部极值
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL FindPeaks(float *Wave)
{
	BYTE i = 0;
	TPeaks maxval1_p; //第一极大值，索引
	TPeaks maxval2_p;//第二极大值,索引
	TPeaks minval1_p;//第一极小值,索引
	float AreaSum = 0;//计算面积

	for(i = 0; i< G_SAMPLE_NUM; i++)
	{
		AreaSum += fabs(Wave[i]);
	}

	//前半个周波都是正数
	for (i = 1; i < (G_SAMPLE_NUM/2 -1); i++) //算第一极值
	{
		if ((Wave[i] > Wave[i - 1]) && (Wave[i] > Wave[i + 1])) 
		{
			maxval1_p.Value = Wave[i];
			maxval1_p.Index = i;
		}
	}

	for (i = 1; i < (G_SAMPLE_NUM/2 -1); i++) //算第二极值
	{
		if (((Wave[i] > Wave[i - 1]) && (Wave[i] > Wave[i + 1])) && (i != maxval1_p.Index))
		{
			maxval2_p.Value = Wave[i];
			maxval2_p.Index = i;
		}
	}

	if(maxval1_p.Index < maxval2_p.Index) //第2极大比第1极大大
	{
		minval1_p.Value = maxval1_p.Value; //初值
		for (i = maxval1_p.Index; i < (maxval2_p.Index); i++) 
		{
			if((Wave[i] < minval1_p.Value))
			{
				minval1_p.Value = Wave[i];
				minval1_p.Index = i;
			}
		}
	}
	else if (maxval1_p.Index > maxval2_p.Index)//第1极大比第2极大大
	{
		minval1_p.Value = maxval2_p.Value;
		for (i = maxval2_p.Index; i < (maxval1_p.Index); i++) 
		{
			if((Wave[i] < minval1_p.Value))
			{
				minval1_p.Value = Wave[i];
				minval1_p.Index = i;
			}
		}
	}

	//如果前半个周波没有这种条件，考虑前半个周波是负数
	if((minval1_p.Index + maxval1_p.Index + maxval2_p.Index) < 1)
	{
		memset(&maxval1_p.Value,0,sizeof(maxval1_p));
		memset(&maxval2_p.Value,0,sizeof(maxval2_p));
		memset(&minval1_p.Value,0,sizeof(minval1_p));

		//前半个周波都是负数,考虑处理后半波
		for (i = G_SAMPLE_NUM/2; i < (G_SAMPLE_NUM -1); i++) 
		{
			if ((Wave[i] > Wave[i - 1]) && (Wave[i] > Wave[i + 1])) 
			{
				maxval1_p.Value = Wave[i];
				maxval1_p.Index = i;
			}
		}

		for (i = G_SAMPLE_NUM/2; i < (G_SAMPLE_NUM -1); i++)
		{
			if (((Wave[i] > Wave[i - 1]) && (Wave[i] > Wave[i + 1])) && (fabs(Wave[i] - maxval1_p.Value) > 0.01))
			{
				maxval2_p.Value = Wave[i];
				maxval2_p.Index = i;
			}
		}

		if(maxval1_p.Index < maxval2_p.Index)
		{
			minval1_p.Value = maxval1_p.Value;
			for (i = maxval2_p.Index; i < (G_SAMPLE_NUM -1); i++) 
			{
				if((Wave[i] < minval1_p.Value))
				{
					minval1_p.Value = Wave[i];
					minval1_p.Index = i;
				}
			}
		}
		else if (maxval1_p.Index > maxval2_p.Index)
		{
			minval1_p.Value = maxval2_p.Value;
			for (i = maxval2_p.Index; i < (G_SAMPLE_NUM -1); i++) 
			{
				if((Wave[i] < minval1_p.Value))
				{
					minval1_p.Value = Wave[i];
					minval1_p.Index = i;
				}
			}
		}
	}

	if(((maxval1_p.Index < minval1_p.Index) && (minval1_p.Index < maxval2_p.Index)) 
	|| ((maxval1_p.Index > minval1_p.Index) && (minval1_p.Index > maxval2_p.Index)))//两峰夹一谷
	{
		// rt_kprintf("%f,%f,%f,%d,%d,%d,%f\n",minval1_p.Value,maxval1_p.Value,maxval2_p.Value,minval1_p.Index,maxval1_p.Index,maxval2_p.Index,AreaSum);
		if((fabs(maxval1_p.Index - maxval2_p.Index) >= 4) && ((AreaSum > 20) && (AreaSum < 30))) //两个峰值距离大于10,面积大于20小于30
		{
			return YES;
		}

	}

	return NO;
}

//--------------------------------------------------
//功能描述:  计算电流差、计算特征、判断电动车
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void CurrentSub(float* u12old,float* i12old,float* u10new,float* i10new,BYTE ph)
{

	float *u_sub =NULL;
	u_sub=u10new;//存电压
	// float *i_sub = rt_malloc(4 * INTER_CYCLE*10);//存电流
	float fIsub[INTER_CYCLE*10]; //栈空间需要增加5K
	rt_uint8_t cross0=0;
	float rmsinew = 0;//电流有效值做差
	float rmsiold = 0;
	BOOL PeakRes = 0; //极值点个数
	BYTE count5 = 0,count6 = 0;
	float OutData[EB_RESULT_NUM] = {0};

	for (rt_uint8_t tmp = 0; tmp < EB_RESULT_NUM; tmp++)
	{
		//结果大于0，小于等于EB_DETECT_TIME时才会进入，意味着电动车20s内仅仅可以识别一个电动车
		//如果电动车识别到一次后，20s内无需执行后续计算，直到20s时间结束
		if ((EBResult[tmp].EBResultTimes[ph] <= EB_DETECT_TIME) && (EBResult[tmp].EBResultTimes[ph]>0))
		{
			return;
		}
	}

	memset((BYTE*)&fIsub[0],0,sizeof(fIsub));
	arm_rms_f32(i10new,INTER_CYCLE*10,&rmsinew);
	arm_rms_f32(i12old+INTER_CYCLE,INTER_CYCLE*10,&rmsiold);

	for (int i=0; i<INTER_CYCLE+5; i++)
	{
		if (u10new[i]<=0 && u10new[i+1]>0)  // 找到电压从负到正的过零点
		{
			cross0=1;
			int ks[9]= {0,-1,1,-2,2,-3,3,-4,4};
			int k=0;
			for (rt_uint8_t m=0; m<9; m++)
			{
				k=ks[m];
				if (u12old[i+INTER_CYCLE+k]<=0 && u12old[i+INTER_CYCLE+1+k]>0)
				{
					// 对齐历史电流数据并计算差值
					for (rt_uint16_t j =0; j<INTER_CYCLE*10; j++)
					{
						fIsub[j]=i10new[j]-i12old[j+INTER_CYCLE+k];
					}
					break;
				}

				if(m==8) // 如果都没对齐，就用默认偏移
				{
					//rt_kprintf("k:%d\n",k);
					//rt_kprintf("k not enough\n");
					for (rt_uint16_t j =0; j<INTER_CYCLE*10; j++)
					{
						fIsub[j]=i10new[j]-i12old[j+INTER_CYCLE];
					}
					break;
				}
			}
			break;
		}
	}
	if (cross0==0)
	{
		rt_kprintf("cross0 is 0\n");
		// rt_free(i_sub);
		memset((BYTE*)&fIsub[0],0,sizeof(fIsub));
		return;
	}

	float count3=0; //描述小耳朵形状
	float maxtmp1,maxtmp2,mintmp1,mintmp2=0;
	float maxtmp1s[10] = {0};
	for(rt_uint8_t j=0; j<10; j++)//周波数
	{
		maxtmp1 = 0;
		maxtmp2 = 0;
		mintmp1 = 0;
		mintmp2 = 0;
		for (rt_uint16_t m =0; m<INTER_CYCLE; m++)//周波点数
		{
			if (u_sub[j*INTER_CYCLE+m]>250)
			{
				maxtmp1=max_float(maxtmp1,fIsub[j*INTER_CYCLE+m]);
				mintmp1=min_float(mintmp1,fIsub[j*INTER_CYCLE+m]);
			}
			else if (u_sub[j*INTER_CYCLE+m]<-250)
			{
				maxtmp2=max_float(maxtmp2,fIsub[j*INTER_CYCLE+m]);
				mintmp2=min_float(mintmp2,fIsub[j*INTER_CYCLE+m]);
			}
		}
		maxtmp1s[j] = maxtmp1;
		//rt_kprintf("m0:%f,%f,%f,%f\n",maxtmp1,maxtmp2,mintmp1,mintmp2);
		if (maxtmp1>1.5f &&maxtmp2<(maxtmp1*0.25f) && mintmp1>(mintmp2*0.2f)&& mintmp2<-1.5f)//上下耳朵的峰值要大，耳根和耳尖距离要大
		{
			// rt_kprintf("m1:%f,%f,%f,%f\n",maxtmp1,maxtmp2,mintmp1,mintmp2);
			count3 += 1.0f;
		}
		if (maxtmp1>1.5f &&maxtmp2<(maxtmp1*0.1f) && mintmp1>(mintmp2*0.1f)&& mintmp2<-1.5f)
		{
			// rt_kprintf("m2:%f,%f,%f,%f\n",maxtmp1,maxtmp2,mintmp1,mintmp2);
			count3 += 0.5f;
		}
		//后加代码,解决虽然是小耳朵形状，但是达不到阈值的情况，最大值是1.4到1.5之间的
		if (((maxtmp1<1.5f) && (maxtmp1>1.4f)) && (maxtmp2<(maxtmp1*0.2f)) && (mintmp1>(mintmp2*0.2f)) && ((mintmp2>-1.5f) && (mintmp2<-1.4f)))//上下耳朵的峰值要大，耳根和耳尖距离要大
		{
			// rt_kprintf("m1:%f,%f,%f,%f\n",maxtmp1,maxtmp2,mintmp1,mintmp2);
			count3 += 0.75f;
		}
		//后加代码,解决虽然是小耳朵形状，但是达不到阈值的情况，最大值是0.75以上
		if (((maxtmp1>0.75f)) &&(maxtmp2<(maxtmp1*0.5f)) &&( mintmp1>(mintmp2*0.5f))&& ((mintmp2<-0.75f)))
		{
			// rt_kprintf("m3:%f,%f,%f,%f\n",maxtmp1,maxtmp2,mintmp1,mintmp2);
			count3 += 0.25f;
		}
		// rt_kprintf("m0:%f,%f,%f,%f\n",maxtmp1,maxtmp2,mintmp1,mintmp2);
	}
	
	//后加代码,处理类似方波的电动车波形
	for(rt_uint8_t j=0; j<10; j++)//周波数
	{
		PeakRes = FindPeaks(&fIsub[j*INTER_CYCLE]);
		// rt_kprintf("PN%d\n",PeakRes);
		if(PeakRes == YES)
		{
			count5 ++;
		}
	}

	//差分判断，作为补充条件,防止小耳朵判不出来
	CalDiff(&features_djc_latest1min[ph].P[50],5,EB_RESULT_NUM,&OutData[0]);
	CalDiff(&OutData[0],5,EB_RESULT_NUM-1,&OutData[0]);
	maxtmp1=arr_max_float(&features_djc_latest1min[ph].P[50],10);
	
	if(((OutData[5]>0.3) && (OutData[5]<(-0.5)) && (OutData[5]>0.3))
	&& ((maxtmp1 - features_djc_latest1min[ph].P[50]) > 8))
	{
		count6++;
	}
	// rt_kprintf("Caldif:");
	// for(m = 0; m<EB_RESULT_NUM-2;m++)
	// {
		// rt_kprintf(" %f",OutData[m]);
	// }
	// rt_kprintf("\n");


	rt_uint8_t count4 = 0;//电流逐渐减小的计数器
	for (rt_uint8_t j = 1; j < 10; j++)
	{
		if ((maxtmp1s[j-1] - maxtmp1s[j])>0.4f)//电流上2个周波-上1周波的峰值大于0.4
		{
			count4 ++;
		}
	}

	float maxtmp1s_max,maxtmp1s_min = 0;
	uint32_t maxtmp1s_max_index,maxtmp1s_min_index = 0;
	// arm_max_f32(maxtmp1s,10,&maxtmp1s_max,NULL);
    // arm_min_f32(maxtmp1s,10,&maxtmp1s_min,NULL);
	//-------------第四个传入参数需要进行调整lsc-----
	arm_max_f32(maxtmp1s,10,&maxtmp1s_max,&maxtmp1s_max_index);
	arm_min_f32(maxtmp1s,10,&maxtmp1s_min,&maxtmp1s_min_index);
	if ((maxtmp1s_max - maxtmp1s_min)>3)//10个周波里，最大峰值与最小峰值差大于3
	{
		count4 ++;
	}

	rt_uint16_t count1 = 0;//电压在-250到250的点数
	float isum = 0;
	for (rt_uint16_t j=0; j<10*INTER_CYCLE; j++)
	{
		if (u_sub[j]>-250 && u_sub[j]<250)//计算vi轨迹中有较长时间的电流平稳状态，
		{
			isum += fIsub[j];
			count1 += 1;
		}
	}
	float imean = 0;
	rt_uint16_t count2 = 0;//电压在-250到250时，电流波动较小的总点数
	imean = isum/count1;//平稳电流的均值
	float tmpthres = 0;
	tmpthres = 0.08f*max_float(maxtmp1,7);//稳定的阈值
	for (rt_uint16_t j = 0; j < 10*INTER_CYCLE; j++)
	{
		if (u_sub[j]>-250 && u_sub[j]<250 && (fIsub[j]-imean)<tmpthres)//电流波动小于阈值
		{
			count2+=1;
		}
	}
	
	//综合判断:
	//1、平稳电流占空比：count2/count1>0.9
	//2、是否满足上下两个小耳朵的形状：count3>9
	//3、存在设备开启：rmsinew-rmsiold>0.3
	//4、电流不是逐步减小的：count4<4
	//5、两峰夹一谷的波形个数：count5>8
	// rt_kprintf("t:%d,c1:%d,c2:%d,c3:%f,c4:%d,c5:%d,c6:%d,rnew:%f,rold:%f\n",Count_features_002s,count1,count2,count3,count4,count5,count6,rmsinew,rmsiold);
	if (((count2>(count1*0.9f)) && (count3>=9) && (count4<4) && ((rmsinew-rmsiold)>0.3f))
	|| ((count2>(count1*0.5f)) &&(count5>=4) && (count4<4) && ((rmsinew-rmsiold)>0.25f))
	|| ((count2>(count1*0.5f)) &&(count6>=1) && (count4<4) && ((rmsinew-rmsiold)>0.25f)))
	{
		EBResult[EBCurrentIndex[ph]].EBResultTimes[ph] = 1;
		rt_kprintf("EB:%d\r\n",EBResult[EBCurrentIndex[ph]].EBResultTimes[ph]);
		api_EBResAddIndex(ph);

		//新增
		if((count3 < 9) && (count5 >= 4))
		{
			SpecEBFlag = 0xBB;
		}
	}
	// rt_free(i_sub);
	memset((BYTE*)&fIsub[0],0,sizeof(fIsub));
	return;
}

#endif //#if(USE_EBIDF)