
#ifndef DBASE_H_
#define DBASE_H_

/* 本系统用到的无功算法,计算出来的无功 大约是有功的 Q_Ratio 倍 */
#if(SAMPLE_NUM==128)
#define Q_Ratio 10.190008123548713f
#elif(SAMPLE_NUM==256)
#define Q_Ratio 20.373878167204523f
#else
#define Q_Ratio (SAMPLE_NUM/128*10.190008123548713f)
#endif

#define DEFAULT_U_FACTOR 0.0001f//2.82533248f//(282533248/100000000.0f)//10.0f//
#define DEFAULT_I_FACTOR 0.0001f//0.02990633f//(2990633/100000000.0f)//10.0f//
#define DEFAULT_A_FACTOR 0.0f
// #define DEFAULT_P_FACTOR (DEFAULT_U_FACTOR*DEFAULT_I_FACTOR/10000.0f)  //8位小数转4位小数,除以10的四次方
#define DEFAULT_P_FACTOR (DEFAULT_U_FACTOR*DEFAULT_I_FACTOR)
#define DEFAULT_Q_FACTOR (DEFAULT_P_FACTOR*Q_Ratio)

/*4位小数*/
#define DEFAULT_U       (2200000)
#define DEFAULT_I       (10000)



/* 瞬时量数据 */
typedef enum{
    PQ_U_RMS=0,             // 电压有效值
    PQ_U_ANG,               // 电压相角
    PQ_U_DC,                // 电压直流分量
    PQ_U_SEQ_POS,           // 正序电压
    PQ_U_SEQ_NEG,           // 负序电压
    PQ_U_SEQ_ZERO,          // 零序电压
    PQ_U_DEV,               // 电压偏差
    PQ_U_UNBALANCE_NEG,     // 负序电压不平衡
    PQ_U_UNBALANCE_ZERO,    // 零序电压不平衡
    PQ_U_FREQ,              // 频率
    PQ_U_FREQ_DEV,          // 频率偏差
    PQ_U_FLUC,              // 波动
    PQ_U_FLUCF,             // 波动频率
    PQ_U_PST,               // 短闪
    PQ_U_PLT,               // 长闪
    PQ_I_RMS,               // 电流有效值
    PQ_I_ANG,               // 电流相角
    PQ_I_DC,                // 电流直流分量
    PQ_I_SEQ_POS,           // 正序电流
    PQ_I_SEQ_NEG,           // 负序电流
    PQ_I_SEQ_ZERO,          // 零序电流
    PQ_I_UNBALANCE_NEG,     // 负序电流不平衡
    PQ_I_UNBALANCE_ZERO,    // 零序电流不平衡
    PQ_PF,                  // 功率因数
    PQ_P,                   // 有功功率
    PQ_Q,                   // 无功功率
    PQ_S,                   // 视在功率
    PQ_FUND_P,              // 基波有功功率
    PQ_FUND_Q,              // 基波无功功率
    PQ_HARM_POS_P_TOTAL,    // 2-50次总谐波正向有功功率
    PQ_HARM_REV_P_TOTAL,    // 2-50次总谐波反向有功功率
    PQ_HARM_POS_Q_TOTAL,    // 2-50次总谐波正向无功功率
    PQ_HARM_REV_Q_TOTAL,    // 2-50次总谐波反向无功功率
    PQ_HARM_P,              // 2-50次谐波有功功率
    PQ_HARM_Q,              // 2-50次谐波无功功率
    PQ_IHARM_P,             // 2-50次间谐波有功功率
    PQ_IHARM_Q,             // 2-50次间谐波无功功率
    PQ_EXTRERMS,            // 暂升暂降中断用的周波有效值

	PQ_U_FLUC2,              // 波动
    PQ_U_FLUCF2,             // 波动频率

}PQ_MMXU_TYPE;

/* 统计数据 */
// 顺序请勿改变，需与 eSysStatusFlag 保持一致
typedef enum{
    STATS_HARM_VOL=0,
    STATS_HARM_CUR,
    STATS_IHARM_VOL,
    STATS_IHARM_CUR,
    STATS_PST,
    STATS_PLT,
    STATS_U_DEV,         // 电压偏差日统计
    STATS_HZ_DEV,        // 频率偏差日统计
    STATS_U_UNBALANCE,   // 电压不平衡日统计
    STATS_I_UNBALANCE,   // 电流不平衡日统计
    STATS_U_FLUC,        // 电压波动日统计
    STATS_U_FLUC_F,      // 电压波动日统计
    STATS_MAX_NUM
}STATS_TYPE;


typedef struct{
    DWORD sec;
    DWORD msec;
}DSPTIME;

/* 时间戳数据 */
typedef enum{
    TIME_3S = 0,    /* 3秒数据时间戳 */
    TIME_10S,       /* 10秒数据时间戳 */
    TIME_10M,       /* 10分数据时间戳 */
    TIME_2H         /* 2小时数据时间戳 */
}TIIME_STAMP;

#pragma pack(1)
//定义实时时钟结构 698协议中 date_time_s 就是这种结构 要求年是word，但长度为7个字节
typedef struct TRealTimer_t
{
	//年
	WORD wYear;
	//月
	BYTE Mon;
	//日
	BYTE Day;
	//时
	BYTE Hour;
	//分
	BYTE Min;
	//秒
	BYTE Sec;

}TRealTimer;
#pragma pack()

/* 系统参数 */
typedef enum{
    PARA_3P3W_VOL = 0,  /* 电压1是否3相3 */
    /*--------------以下连续-------------*/
    PARA_RATE_VOL,   /* 额定电压1 4位小数*/
    /*额定电流*/
    PARA_RATE_CUR   /* 额定电流1 4位小数*/
}PARA_TYPE;


//暂态开始 到 暂态结束的 最小时间.若不达到此时间,则不记录
typedef enum
{
    Evt_Min_0ms     =   0,              //0ms时 意味着不开启 过滤                                        --(DSP默认)
    Evt_Min_5ms     =   1,              //5ms时 过滤掉包括5ms在内的暂升暂降事件
    Evt_Min_7_5ms   =   2,              //7.5ms时 过滤掉包括7.5ms在内的暂升暂降事件
    Evt_Min_10ms    =   3,              //10ms时 过滤掉包括10ms在内的暂升暂降事件
    Evt_Min_20ms    =   4,              //20ms时 过滤掉包括20ms在内的暂升暂降事件
    Evt_Min_40ms    =   5,              //40ms时 过滤掉包括40ms在内的暂升暂降事件
    Evt_Min_enum_MAX
}EVT_START_MIN_DUR;

//暂态结束 到 下一次暂态开始的 最小时间.若不达到此时间,则认为是一次暂态
typedef enum
{
    Evt_Wait_100ms       =   0,
    Evt_Wait_150ms       =   1,
    Evt_Wait_200ms       =   2,
    Evt_Wait_250ms       =   3,
    Evt_Wait_400ms       =   4,
    Evt_Wait_500ms       =   5,
    Evt_Wait_enum_MAX
}EVT_END_MIN_DUR;


typedef struct{
    DWORD   fluc[PH_NUM];                   //4位小数  电压波动
    DWORD   fluc_f[PH_NUM];                 //0位小数  电压波动频率
    DWORD   pst[PH_NUM];                    //4位小数  短闪变  10min
    DWORD   plt[PH_NUM];                    //4位小数  长闪变  2h
    DWORD   hz;                             //4位小数  频率     10s
    SDWORD  vol_dev[PH_NUM];                //2位小数  电压偏差 xx.xx%
    SDWORD  hz_dev;                         //4位小数  频率偏差 Hz
    DWORD   ExtreRms[PH_NUM][MaxRms+1];     //4位小数  暂升暂降中断用的周波有效值

	DWORD   fluc2[PH_NUM];                   //4位小数  电压波动
    DWORD   fluc_f2[PH_NUM];                 //0位小数  电压波动频率
}VOL_UNION;

typedef struct{
    // 功率
    INT     P[PH_NUM+1];//  4位小数    有功功率w   ABC+总  3s
    INT     Q[PH_NUM+1];//  4位小数    无功功率var ABC+总  3s
    INT     S[PH_NUM+1];//  4位小数    视在功率VA  ABC+总  3s
    INT     PF[PH_NUM+1]; //  功率因数   ABC
}CUR_UNION;

typedef union{
    VOL_UNION v;
    CUR_UNION c;
}VI_UNION;

typedef struct{
    DWORD   pos;            // 正序 4位小数
    DWORD   neg;            // 负序 4位小数
    DWORD   zero;           // 零序 4位小数
}VI_SEQ;

typedef struct{
    DWORD   neg;  // 负序不平衡 xx.xx%
    DWORD   zero; // 零序不平衡 xx.xx%
}VI_UNBALANCE;

#define     HAMONIC_ANYSIS_NUM  50      // 谐波分析次数 256点每周波，10周波2560点混合基
#define     STATS_HARM_NUM      24      // 统计谐波次数 2-25 次
#define     STATS_IHARM_NUM     21      // 统计间谐波次数 0.5-20.5 次
//采样率为256*50, 理论上应该能分析256/4/50Hz=95次谐波
typedef struct{
    DWORD           vi_mag[PH_NUM];                                 // 4位小数 电压电流
    VI_UNION        vi_union;
    INT             vi_dc[PH_NUM];                                  // 4位小数 直流
    DWORD           vi_harm_mag[PH_NUM][HAMONIC_ANYSIS_NUM];        // 4位小数 基波+谐波幅值
    DWORD           vi_intharm_mag[PH_NUM][HAMONIC_ANYSIS_NUM];     // 4位小数 基波+谐波幅值
    WORD            vi_harm_ang[PH_NUM][HAMONIC_ANYSIS_NUM];        // 4位小数 基波+谐波相位弧度
    WORD            vi_intharm_ang[PH_NUM][HAMONIC_ANYSIS_NUM];     // 4位小数 基波+谐波相位弧度
    DWORD           vi_THD[PH_NUM];                                 // 2位小数(xxxx=xx.xx%) 总谐波畸变率
    DWORD           vi_harm_content[PH_NUM][HAMONIC_ANYSIS_NUM];    // 2位小数(xxxx=xx.xx%) 谐波含量
    DWORD           vi_intharm_content[PH_NUM][HAMONIC_ANYSIS_NUM]; // 2位小数(xxxx=xx.xx%) 间谐波含量
    VI_SEQ          vi_seq;                                         // 序分量
    VI_UNBALANCE    vi_unbalance;                                   // 不平衡
}DSP_RT_ONE_50;

/* 时间戳 */
typedef struct{
    DSPTIME tm_3s;      /* 3秒数据时间戳 */
    DSPTIME tm_10s; /* 10秒数据时间戳 */
    DSPTIME tm_10m; /* 10分钟数据时间戳 */
    DSPTIME tm_2h;      /* 2小时数据时间戳 */
}TIME_STAMP;

// 基波功率
typedef struct
{
    FLOAT P[PH_NUM+1];     // 基波有功功率
    FLOAT Q[PH_NUM+1];     // 基波无功功率
}FUNDPOWER;

// 谐波总功率
typedef struct
{
    FLOAT posP[PH_NUM+1];     // 谐波正向有功总功率
    FLOAT revP[PH_NUM+1];     // 谐波反向有功总功率
    FLOAT posQ[PH_NUM+1];     // 谐波正向无功总功率
    FLOAT revQ[PH_NUM+1];     // 谐波反向无功总功率
}HARMPOWER_TOTAL;

// 各次谐波功率
typedef struct
{
    FLOAT P[PH_NUM+1][HAMONIC_ANYSIS_NUM];     // 1-50次谐波有功功率
    FLOAT Q[PH_NUM+1][HAMONIC_ANYSIS_NUM];     // 1-50次谐波无功功率
}HARMPOWER;

// 谐波量
typedef struct
{
    FUNDPOWER       fundpower;              // 基波功率
    HARMPOWER_TOTAL harmpower_total;        // 谐波总功率
    HARMPOWER       harmpower;              // 谐波功率
    HARMPOWER       iharmpower;             // 间谐波功率
    DWORD           harmactpower_content[PH_NUM][HAMONIC_ANYSIS_NUM];       // 2位小数(xxxx=xx.xx%) 谐波功率含量
    DWORD           iharmactpower_content[PH_NUM][HAMONIC_ANYSIS_NUM];      // 2位小数(xxxx=xx.xx%) 间谐波功率含量

}HARM_DATA;

// 瞬时量
typedef struct
{
    TIME_STAMP      time;
    DSP_RT_ONE_50   vol;
    DSP_RT_ONE_50   cur;
    HARM_DATA       harm_data;
}PQ_DATA;

// 统计结构体
// typedef struct
// {
//     DWORD max;
//     DWORD min;
//     DWORD avg;
//     DWORD p95;
//     DWORD rms;
// }STATSDATA;

typedef union{
    DWORD   d;
    SDWORD  l;
}TYPE_UNION;

typedef struct
{
    TYPE_UNION  max_data;
    TRealTimer  max_time;
    TYPE_UNION  min_data;
    TRealTimer  min_time;
}STATSDATA_STRUCT;

// 统计量
typedef struct
{
    DWORD               p95_harm_vol[PH_NUM][STATS_HARM_NUM];
    DWORD               p95_harm_cur[PH_NUM][STATS_HARM_NUM];
    DWORD               p95_iharm_vol[PH_NUM][STATS_IHARM_NUM];
    DWORD               p95_iharm_cur[PH_NUM][STATS_IHARM_NUM];
    STATSDATA_STRUCT    pst[PH_NUM];
    STATSDATA_STRUCT    plt[PH_NUM];
    STATSDATA_STRUCT    vol_dev[PH_NUM];
    STATSDATA_STRUCT    hz_dev;
    STATSDATA_STRUCT    vol_unbalance;
    STATSDATA_STRUCT    cur_unbalance;
    STATSDATA_STRUCT    fluc[PH_NUM];
    STATSDATA_STRUCT    fluc_f[PH_NUM];

}PQ_STATSDATA;

typedef enum
{
    PQ_Correct_U    = 0 ,
    PQ_Correct_I        ,
    // PQ_Correct_I_3s     ,
    PQ_Correct_P        ,
    PQ_Correct_Q        ,
    PQ_Correct_U_double ,
    PQ_Correct_I_double ,
    PQ_Correct_A
}CORRECT_TYPE;

typedef struct
{
    FLOAT U_factor[PH_NUM];// 大校表系数
    FLOAT I_factor[PH_NUM];// 校表系数
    FLOAT A_factor[PH_NUM];// 校表系数 -相角修正-弧度
    FLOAT P_factor[PH_NUM];// 校表系数 有功功率
    FLOAT Q_factor[PH_NUM];// 校表系数 无功功率
    
    DWORD dwCRCfact;
    
}BIG_ADJUST_PARA;

typedef enum
{
    Factor_Big_Vol      = 0 ,
    Factor_Big_Cur      = 1 ,
    Factor_Big_Ang      = 2 ,
    Factor_P            = 3 ,
    Factor_Q            = 4 ,
    Factor_Clr          = 5 ,
	
		EVol								= 6,
		ECur								= 7,
		Is3P3W							= 8
}ADJUST_TYPE;

typedef enum
{  
    U = 0,                  // 电压
    U_SEQ_POS,              // 正序电压
    U_SEQ_NEG,              // 负序电压
    U_SEQ_ZERO,             // 零序电压
    U_DEV,                  // 电压偏差
    I,                      // 电流（2位小数）
    I_HIGHPREC,             // 高精度电流（4位小数）
    I_SEQ_POS,              // 正序电流
    I_SEQ_NEG,              // 负序电流
    I_SEQ_ZERO,             // 零序电流
    PF,                     // 功率因数
    U_THD,                  // 电压波形失真度(畸变率)
    I_THD,                  // 电流波形失真度(畸变率)
    HZ,                     // 频率
    HZ_DEV,                 // 频率偏差
    U_UNBALANCE_NEG,        // 负序电压不平衡
    U_UNBALANCE_ZERO,       // 零序电压不平衡
    I_UNBALANCE_NEG,        // 负序电流不平衡
    I_UNBALANCE_ZERO,       // 零序电流不平衡

    UChange,                // 电压波动      电压波动4
	UChangeFre,             // 电压波动      电压频次4
    UFlicker,               // 电压闪变      短时4+
	UFlickerLong,           // 电压闪变      长时4

    MODULE_STATUS,          // 模块状态字
    HarmonicU,              // 谐波电压          4字节  象限+次数
    HarmonicI,              // 谐波电流          4字节    象限+次数
    HarmonicP,              // 谐波功率          4字节  象限+次数
    HarmonicPALL,           // 谐波总功率        8字节
    IHarmonicU,             // 间谐波电压        4字节  象限+次数
    IHarmonicI,             // 间谐波电流        4字节  象限+次数
    IHarmonicP,             // 间谐波功率        4字节  象限+次数
    HarmonicUDayCount,      // 谐波电压日统计    20个字节 象限+次数
    HarmonicIDayCount,      // 谐波电流日统计    20个字节 象限+次数
    IHarmonicUDayCount,     // 间谐波电压日统计  20个字节 象限+次数
    IHarmonicIDayCount,     // 间谐波电流日统计  20个字节 象限+次数
    PSTDayCount,            // 本日短时闪变统计
    PLTDayCount,            // 本日长时闪变统计
    U_DEV_DayCount,         // 电压偏差日统计
    HZ_DEV_DayCount,        // 频率偏差日统计
    U_SEQ_DayCount,         // 电压不平衡日统计
    I_SEQ_DayCount,         // 电流不平衡日统计
    UChange_DayCount,       // 电压波动日统计
    UChangeFre_DayCount,       // 电压波动日统计

	HarmUContent,//200E0200—A相谐波
	HarmIContent,//200E0200—A相谐波电流含有率
	IHarmUContent,//20490200—A相间谐波电压含有率
	IHarmIContent,//204A0200—A相间谐波电流含有率

	ExtremStat,	
	ProbabilityStat,

	HGStat_A,
	HGStat_B,
	HGStat_C,
	FATStat,
	FATStatPara,

	BodongPara,
	ExtremStat3,	
	ProbabilityStat3,


	MD_LIST,
	UChange3,                // 电压波动      电压波动4
	NILM_P,

}eModuleType;//请求类型

typedef struct
{
	DWORD	dwDayJZOAD;					//	关联对象属性描
	BYTE	byDayJZPeriod;
	DWORD   dwDayJZTi;
}JZ_PARA;

typedef struct
{
	//日极值统计
	//	一个关联对象属性∷=structure
	//{
	//		关联对象属性描述符  OAD，
	//		统计周期  unsigned，
	//		统计间隔  TI
	//}


	DWORD	dwDayJZOAD;					//	关联对象属性描
	BYTE	byDayJZPeriod;
	DWORD   dwDayJZTi;
	
	//日概率值统计
	//一个关联对象属性∷=structure
	//{
	//	关联对象属性描述符  OAD，
	//		统计周期  unsigned，
	//		统计间隔  TI，
	//		置信水平  unsigned（单位：%，换算：0），
	//		统计类型  enum{最大值（0），最小值（1），中间值（2），自定义（255）}，
	//		自定义类别 visible-string，
	//		自定义公式 octet-string,
	//		自定义参数 octet-string
	//}
	DWORD	dwDayTJOAD;					//	关联对象属性描
	BYTE	byDayTJPeriod;
	DWORD   dwDayTJTi;
	BYTE	byDayTJConLevel;
	BYTE	byDayTJType;
	
	//电压波动
	//属性4（波动阈值）∷=long-unsigned，单位：%，换算：-2
	
	DWORD	dwBdThreshold;
	
	
}PQ_JZ_RATE;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    BOOL Uis3P3W;// 电压是否3相3线 低半字节=0表示3相4; 低半字节=1
    DWORD vol_std;// 额定电压 -4位小数
    DWORD cur_std;// 额定电流 -4位小数
    BYTE EvtCtrl;         //暂态过滤
    WORD evt_threshold[TRA_EVT_MAX][EVT_PARA_MAX];// 暂降开始门限 暂降结束门限 暂升开始门限 暂升结束门限 中断开始门限 中断结束门限，百分比
    BIG_ADJUST_PARA big_adjust_para;

}SYS_PARA_1808;
#pragma pack(pop)

/* 瞬时量数据区 */
extern PQ_DATA * GetPqDataAddr(void);
extern PQ_STATSDATA * GetPqStatsDataAddr(void);
extern void SetTimeStamp(BYTE type, DSPTIME *val);
extern void SetPqData(BYTE type, void *val);
extern DWORD GetFreq(void);

/*系统参数数据区 */
extern SYS_PARA_1808 *GetSysParaAddr(void);
extern DWORD Get_Sys_Para(BYTE type);
extern void Set_Adjust_Para(BYTE type ,BYTE phase,void *factor);
extern void GetDig_Factor(void *buf);
extern int ICorrect(BYTE ph,BYTE type, float val);
extern void Correct_UI_double(BYTE ph, BYTE type, double *data);
extern int CorrectUIPQ(BYTE ph, BYTE type, void *data);
extern void CorrectAngle(INT *p,INT *q);
extern void SysParaInit(void);
extern void Save_Para(void);
extern void RefreshTemFactor(void);
extern void memclr_f(void *ptr, int len);
extern DWORD GetFastFreq(void);        //来自于task_freq.c
extern WORD api_GetModuleData( BYTE ModuleType, BYTE Phase, BYTE Times, BYTE *pBuf );

#endif /* DBASE_H_ */
