//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __PROCESSING_DKY_PH1_H__
#define __PROCESSING_DKY_PH1_H__
#include "math.h" 
#include "stdio.h"
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define KNOWNNUM 					10
#define MAXDEVNUM 					20
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
struct unknownR_typeDef  //未知阻类的记录
{
    short P;
    uint16_t   during;
};
struct devID_typeDef  //用来统计冻结中电器的id
{
	BYTE    appid[KNOWNNUM];
	short devp[KNOWNNUM];
};
/*******************************************************************************
 *  一次冻结的数据 frozen_typeDef
 *
 */
#pragma pack(1)
typedef struct 
{
	uint16_t year;
	uint8_t  mon;
	uint8_t  day;
	uint8_t  hour;
	uint8_t  min;
	uint8_t  sec;
}date_time_s;

/*******************************************************************************
 *  单个设备结果结构体 sindev_typeDef
 *
 */
typedef struct 
{
	uint16_t device;//设备类号
	uint32_t pused;//
	int p;
	uint8_t start;
	uint8_t end;
	uint8_t switchtimes;
}sindev_typeDef;


typedef struct
{
	sindev_typeDef sindev[MAXDEVNUM];
	date_time_s time;
	uint8_t num;//电器数量
	uint32_t counter;//冻结编号
}frozen_typeDef;
#pragma pack()
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern uint8_t                      N15min;
extern WORD                     	tlength;  //本次的数据长度，单位s
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
struct cluster_typeDef *cluster(float *i50, short *P, short *Q, short *Qfft, short *D, short *Praw,
                                short *Pmax, float *Pusedall_real, uint8_t *n_cluster);
uint8_t *                    DBSCAN(short *P, uint8_t eps, uint8_t min_samples, uint8_t *c, uint16_t len);
uint8_t *                    check_1st(uint8_t *c, short *P, short *Q);
void                    switch_label(struct cluster_typeDef *clusters, uint8_t n, short *P);

void nilm(struct lastfeature_typeDef *lastcluster, struct features_typeDef *feats ,BYTE Phase);
struct steady_typeDef      diff_features(struct cluster_typeDef c1, struct cluster_typeDef c2,
                                         struct features_typeDef feats, struct result_typeDef result);
struct steady_typeDef      diff_features_2slowly(struct cluster_typeDef c1, struct cluster_typeDef c2,
                                                 struct features_typeDef feats, uint8_t type);
struct steady_typeDef      diff_lastcluster(struct cluster_typeDef c1, struct lastfeature_typeDef c2,
                                            struct features_typeDef feats, uint8_t onoff);
void csum_cal(struct features_typeDef *feats, uint16_t *t, short length, struct feature_typeDef *csum);
struct lastfeature_typeDef last_cluster_features(struct cluster_typeDef c1, struct features_typeDef feats,
                                                 struct result_typeDef result);

void add_device(struct result_typeDef *result, uint8_t device, uint8_t i, short P, short Praw, float Pused, uint16_t t);
void remove_device(struct result_typeDef *result, uint8_t device, uint8_t i);
void update_device(struct result_typeDef *result, struct steady_typeDef diff, uint16_t t, uint8_t device, uint8_t i,
                   uint8_t onoff);
void recunknownR(struct result_typeDef *result, uint8_t N, uint8_t n);
void update_recunknownR(struct result_typeDef *result, struct cluster_typeDef *clusters, uint8_t i, uint8_t n_cluster);
void switch_union(struct cluster_typeDef *clusters, struct result_typeDef *result, uint8_t n);
void del_add_device(struct cluster_typeDef *clusters, struct result_typeDef *result, uint8_t n);

uint8_t recdevice(struct steady_typeDef diff, uint8_t type, struct result_typeDef *result, int i,
             struct cluster_typeDef *clusters, struct features_typeDef feats, uint8_t n_cluster);
uint8_t reclevel1(short P, short Q, short Pmax, short Plast, uint8_t onoff);
uint8_t reclevel2r(short P, short Pstd, short Pmax, uint16_t dur, uint8_t onoff, struct result_typeDef result);
uint8_t reclevel2lc(short P, short Q, short Pmax, float hdi3, float hdi5, float Pslope, float std, uint8_t onoff,
               struct result_typeDef result, uint8_t type);

void       creat_frozen(frozen_typeDef *frozentmp, uint8_t dev_count, uint8_t *dev_output, float *pused_output, short *p_output, uint8_t *dev_start,
                        uint8_t *dev_end, uint8_t *dev_switch);
void       startendtime(uint8_t dev_count, uint8_t *dev_output, uint8_t n_cluster, struct cluster_typeDef *clusters,
                        struct result_typeDef *result, uint8_t *dev_start, uint8_t *dev_end, uint8_t *dev_switch);

/* 
 * 获取冻结数据 
 * 参数：
 *      uint8_t * dev_num            -- 设备个数
 *      T_ApplFrzData * fro_data     -- 冻结
 */
extern void get_frozen(uint8_t * dev_num, T_ApplFrzData * fro_data);

/* 
 * 设置冻结周期
 * 参数：
 *      uint8_t period   --  冻结周期（15、30、45、60）
 * 返回值：
 *      0  --  成功
 *      1  --  失败
 */
extern uint8_t algo_set_frozen_period(uint8_t period);

#if(PROTOCOL_VERSION == 25)
/* 
 * 获取未知设备特征个数
 */
extern uint8_t get_feature_event_num(void);
/* 
 * 获取未知设备特征 
 */
extern void get_feature_event(uint8_t * buf);
/* 
 * 获取已知设备个数
 */
extern uint8_t get_device_event_num(void);

/* 
* 获取已知设备
*/
extern void get_device_event(TDeviceInfoPool * device, uint8_t index);
#endif //#if(PROTOCOL_VERSION == 25)

#endif //#ifndef __PROCESSING_DKY_PH1_H__
