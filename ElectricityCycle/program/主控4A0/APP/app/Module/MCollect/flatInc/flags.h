
#ifndef __FLAGS_H__
#define __FLAGS_H__


//注意：标准标识号从0x20开始，规约任务可以使用(0~0x1F作为特殊标记使用)
typedef enum{
	MSF_CENT_0=0,				//0~10为临时抄表缓冲标志，其他不得占用
	//一、实时
	MSF_DATE=0x20,				//电表日期
	MSF_TIME,					//电表时间
	MSF_APR,					//组合有功电能（带费率）
	MSF_AP,						//正向有功电能（带费率）
	MSF_AR,						//反向有功电能（带费率）
	MSF_RP,						//正向无功电能（带费率）
	MSF_RR,						//反向无功电能（带费率）
	MSF_R1,						//一象限无功电能（带费率）
	MSF_R2,						//二象限无功电能（带费率）
	MSF_R3,						//三象限无功电能（带费率）
	MSF_R4,						//四象限无功电能（带费率）
	MSF_A_ARP,					//A相电能量		
	MSF_B_ARP,					//B相电能量		
	MSF_C_ARP,					//C相点能量		
	MSF_LEFTDD,					//剩余电度
	MSF_PAYNUM,					//购电次数
	MSF_PAYSUM,					//累计购电金额
	MSF_OVERDRAFT,              //透支金额
	//1.2实时遥测值
	MSF_VT,      				//电压瞬时量
	MSF_CT,     				//电流瞬时量
	MSF_PA,						//有功功率瞬时量
	MSF_PR,						//无功功率瞬时量
	MSF_PAP,					//视在功率
	MSF_PE,						//功率因素瞬时量
	MSF_I0,						//零序电流	
	MSF_VIANGLE,				//相位角
	MSF_FREQ,					//频率
	MSF_STSMET,					//电表状态字
	MSF_STSMET1,				//电表状态字1
	MSF_STSREST,				//周休日状态字
	//1.3需量	
	MSF_APXL,    				//正向有功电能需量以及发生时间
	MSF_ARXL,    				//反向有功电能需量以及发生时间
	MSF_RPXL,    				//正向无功电能需量以及发生时间
	MSF_RRXL,    				//反向无功电能需量以及发生时间

	//1.3其他
	MSF_BATTIME,				//电池工作时间
	MSF_CONSTANT,				//有功、无功常数
	RSF_TRIPTM,					//最近一次远程控制跳/合闸时间
	RSF_TIMESEG,				//第X时段表参数
	//事件
	MSF_OPEN_WG,				//尾盖						
	MSF_OPEN_DNG,				//端钮盖
	MSF_XLCLRNUM,				//需量清零次数
	MSF_PROGNUM,				//编程次数
	MSF_CLR_NUM,				//清零次数	
	MSF_CLRETNUM,				//事件清零次数	
	MSF_CORTTMNUM,				//校时次数
	RSF_TMPRONUM,				//时段编程
	MSF_DXINFOA,				//A相断相次数645 2次备案
	MSF_DXINFOB,				//B相断相
	MSF_DXINFOC,				//C相断相
	MSF_DXALL,					//2次备案前 
	MSF_STSMET0,
	MSF_SYINFALL,				//全失压
	MSF_SYINFOA,     			//A相失压信息
	MSF_SYINFOB,      			//B相失压信息
	MSF_SYINFOC,				//C相失压信息
	MSF_TMPROTIME,   			//最近一次时段表编程时间
	MSF_LASTMFREC,   			//最近一次磁场干绕记录
	MSF_DLCOUNTA,    			//A相断流总次数
	MSF_DLCOUNTB,    			//B相断流总次数
	MSF_DLCOUNTC,    			//C相断流总次数
	MSF_QSY_TIME,    			//全失压总次数 累计时间
	MSF_LOSTPW_NUM,				//掉电次数
	MSF_LOSTPWTIME,				//掉电时间

	MSF_UPSTSMET,				//电表主动上报状态字
	MSF_EVENTCJ,
	MSF_EVENTCJ_EX,
	
	//二曲线	
	MSF_CURE1,
	MSF_CURE2,
	MSF_CURE3,
	MSF_CURE4,
	MSF_CURE5,
	MSF_CURE6,
	MSF_CURE,
	//三、日冻结
	MSF_DAY_TM,  	
	MSF_DAY_AP,		
	MSF_DAY_AR,		
	MSF_DAY_RP,		
	MSF_DAY_RR,		
	MSF_DAY_R1,  	
	MSF_DAY_R2,  	
	MSF_DAY_R3,  	
	MSF_DAY_R4,  	
	MSF_DAYXLAP, 	
	MSF_DAYXLAR, 	
	MSF_DAYXLRP, 	
	MSF_DAYXLRR, 
	MSF_DAYLEFTPAY,  //..add by zsm 20190528
	MSF_DAYP07,
	//四、月冻结
	MSF_MON_APR,	
	MSF_MON_AP,		
	MSF_MON_AR,		
	MSF_MON_RP,		
	MSF_MON_RR,		
	MSF_MON_R1,  	
	MSF_MON_R2,  	
	MSF_MON_R3,  	
	MSF_MON_R4,
	MSF_MON_A,					//A相电能量		
	MSF_MON_B,					//B相电能量		
	MSF_MON_C,					//C相点能量
	MSF_MONXLAP,				//月正有需量
	MSF_MONXLAR,				//月反有需量
	MSF_MONXLRP,				//月正无需量
	MSF_MONXLRR,				//月反无需量	
	//五、参数
	MSF_AUTODAY,
	MSF_TMPRO_NUM,   			//时段表编程次数
	MSF_HGQBB_CPT,   			//电流电压互感器变比
	MSF_CALC_DATA,					//表计计量数据

	MAX_FLAGNO					//端口最大标识个数
}MeterMsf;		

DWORD GetFlag(DWORD dwFlagNo);
void  SetFlag(DWORD dwFlagNo);
void  ClearFlag(DWORD dwFlagNo);
void  ClearAllFlag(void);
BOOL  CheckClearFlag(DWORD dwFlagNo);
#endif
