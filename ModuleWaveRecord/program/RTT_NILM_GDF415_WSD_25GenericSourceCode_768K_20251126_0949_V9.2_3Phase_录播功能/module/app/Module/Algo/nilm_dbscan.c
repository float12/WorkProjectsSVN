#include "AllHead.h"
#include "arm_math.h"

short distance_between_two(short *read_data, uint16_t first_point, uint16_t second_point)
{	
	float powtmp1,powtmp2 = 0;
	float subtmp1,subtmp2 = 0;
	float sqrttmp = 0;

	subtmp1=(float)(first_point - second_point);
	subtmp2=(float)(read_data[first_point] - read_data[second_point]);
	arm_power_f32(&subtmp1,1,&powtmp1);
	arm_power_f32(&subtmp2,1,&powtmp2);
	arm_sqrt_f32(powtmp1+powtmp2,&sqrttmp);
	//short temp = (short)sqrt( pow((double)(first_point - second_point), 2) + pow((double)(read_data[first_point] - read_data[second_point]), 2));
	return (short)sqrttmp;
}

void iteration_find_neighbor(short *read_data, uint16_t core_id, uint8_t neighbor_value, uint8_t *noise_flag, uint8_t cluster_id, uint16_t data_len)
{
	uint16_t i = 0;
	short distant = 0;

	for(i = 0; i < data_len; i++)
	{
		if(noise_flag[i] == cluster_id)
		{
			continue;
		}
		distant = distance_between_two(read_data, core_id, i);
		if(distant <= neighbor_value) //如果距离小于半径eps，则在把这个点判断为属于当前核心点所在集群
		{
			noise_flag[i] = cluster_id;
			iteration_find_neighbor(read_data, i, neighbor_value, noise_flag, cluster_id, data_len);
		}
	}

	return;
}
/*****************************
参数：*read_data 输入参数一维数组，数组下标为时间，数组内容为功率数据
	  data_len 数组长度
	  neighbor_value 可视为统一聚类的长度
	  neighbor_min 视为中心点的最小点数
	  *noise_flag 输出结果,0为噪声，1~n为聚类，***注意需调用者开辟空间
******************************/
void dbscan_less_space(short *read_data, uint16_t data_len, uint8_t neighbor_value, uint8_t neighbor_min, uint8_t *noise_flag)
{
	uint16_t i,j;
	short distance = 0;
	uint16_t neighbor_count = 0;
	uint8_t cluster_count = 0;
	
	if(noise_flag == NULL)
	{
		return;
	}
	
	for(i = 0; i < data_len; i++)
	{
		if(noise_flag[i] != 0)
		{
			//已经属于某个聚类，就不对齐再进行分析
			continue;
		}
		neighbor_count = 0;
		//查找相邻点个数
		for(j = 0; j < data_len; j++)
		{
			/*if(i == j)
			{
				continue;
			}*/
			
			distance = distance_between_two(read_data, i, j); //计算两个点之间的欧氏距离
			if(distance <= neighbor_value) //如果距离小于最大邻域半径eps
			{
				neighbor_count++;
			}
		}
		
		if(neighbor_count >= neighbor_min) 	//如果个数大于邻域最少点数
		{
			//相邻点大于参数，作为核心点寻找聚类
			cluster_count++; //核心点的个数
			iteration_find_neighbor(read_data, i, neighbor_value, noise_flag, cluster_count, data_len); //递归出来后，应该是一个类别就聚类完成了
			
		}
		else
		{
			//作为噪点，但可能被后续聚类包含
			//噪点不统计
		}
	}
	adjust_cluster_order(data_len, noise_flag);
	return;
}

//--------------------------------------------------
//功能描述:  聚类标签重排序：确保标签值从 1 开始连续递增（跳过缺失值）。
//         
//参数:      
//         
//返回值:    
//         
//备注:  输入数组：[1, 3, 2, 3, 1],输出数组：[1, 2, 3, 2, 1]（标签按顺序 1→2→3 出现）。
//--------------------------------------------------
void adjust_cluster_order(uint16_t data_len,uint8_t *noise_flag)
{
	uint8_t maxnow=1;

	for (uint16_t i=0;i<data_len;i++)
	{
		if (noise_flag[i]==(maxnow+1))
		{
			maxnow++;
		}
		else if (noise_flag[i]==(maxnow+2))
		{
			for (uint16_t j=0;j<data_len;j++)
			{
				if (noise_flag[j]==maxnow+2)
				{
					noise_flag[j]=maxnow+1;
				}
				else if(noise_flag[j]==maxnow+1)
				{
					noise_flag[j]=maxnow+2;
				}
			}
			maxnow=maxnow+2;
		}
	}
}
/*int main()
{
	int noise_flag[4096] = {0};
	
	dbscan_less_space(read_data, sizeof(read_data)/sizeof(float), 60, 15, noise_flag);

	return 0;

}*/
