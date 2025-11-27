//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.27
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
//#include "defaultPara.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
	
#define MT_SIG_PH		1
#define MT_MLT_PH		2
#define MT_MLT_TM		3
#define MT_EXT_STA		4
#define MT_PRO_STA		5

#define USE_HARN_FREEZE_25              0
static const BYTE T_EventNowData[]		= //事件当前值
{ 
	Structure_698, 				0x02, 
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4
};	

static const BYTE T_U[]	= { Array_698, 50, Double_long_unsigned_698, 4  };//电压数据标识
static const BYTE T_I[] = { Array_698, 50, Double_long_698, 4  };//电流数据标识
static const BYTE T_P[]	= { Array_698, 50, Double_long_698, 4  };//功率数据标识

static const BYTE T_ModuleValue[]	= //电能质量模块电压,电流
{ 	
	Array_698,					MAX_PHASE,
	Double_long_unsigned_698,	4,
};

static const BYTE T_NILM_ModuleValue[]	= //负荷辨识模块电压
{ 	
	Array_698,					MAX_PHASE,
	Long_unsigned_698,			2,
};


static const BYTE T_ModuleValue_I[]	= //电能质量模块电压,电流
{ 	
	Array_698,					MAX_PHASE,
	Double_long_698,			4,
};
static const BYTE T_NILM_ModuleValue_I[]	= //负荷辨识模块电流
{ 	
	Array_698,					MAX_PHASE,
	Double_long_698,			4,
};

static const BYTE T_NILM_ModuleValue_P[]	= //NILM,有功功率
{ 	
	Array_698,					MAX_PHASE+1,
	Double_long_698,			4,
};
static const BYTE T_ModuleValueCOS[]   = //电能质量模块功率因数
{
    Array_698,                  MAX_PHASE,
    Long_698,                   2,
};
static const BYTE T_ModuleValueCOS4[]   = //电能质量模块功率因数
{
    Array_698,                  MAX_PHASE+1,
	Long_698,                   2,
};

static const BYTE T_ModuleValueChange[]	= //电能质量模块电压波动
{ 	
	Array_698,					MAX_PHASE,
	Structure_698, 	 			2, 
	Double_long_unsigned_698,	4, 
	Double_long_unsigned_698,	4, 
};
static const BYTE T_ModuleValueChange2[]	= //电能质量模块电压波动
{ 	
	Array_698,					MAX_PHASE,
	Structure_698, 	 			2, 
	Double_long_unsigned_698,	4, 
	Long_unsigned_698,			2, 
};

static const BYTE T_VI_COUNT[]	= //电压电流统计
{ 
	Array_698, 				50,

	Double_long_unsigned_698, 	4, 
	//wxy 20210813.19:15
/*	Structure_698, 			0x05, 
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
*/
};

static const BYTE T_VI_COUNT2[]	= //电压电流统计
{ 
	Array_698, 				50,

	Long_698, 	2, 

};

static const BYTE T_VChange_COUNT[]	= //电压电流统计
{ 
	Structure_698, 			0x05, 
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
	Double_long_unsigned_698, 	4,
};


static const BYTE T_VChangeLong_COUNT[]	= //电压电流统计
{ 
	Structure_698, 			0x05, 
	Long_698, 	4, 
	Long_698, 	4,
	Long_698, 	4,
	Long_698, 	4,
	Long_698, 	4,
};


static const BYTE T_P_ALL[]	= 
{ 
	Structure_698, 				2, 
	Double_long_unsigned_698, 	4,  
	Double_long_unsigned_698, 	4,  
};//功率数据标识

static const BYTE T_EXTM_STAT[]	= 
{ 
#if(MAX_PHASE == 3)
	Array_698,				 12,
#else
	Array_698,					5,
#endif

	Structure_698, 	 			5, 
	OAD_698,                                4,      //Double_long_unsigned_698, 	4,  // OAD = 20000700
	Double_long_unsigned_698, 	4, 
	DateTime_S_698,				7,
	Double_long_unsigned_698, 	4, 
	DateTime_S_698,				7,

};//
static const BYTE T_EXTM_STAT3[]	= 
{ 
#if(MAX_PHASE == 3)
	Array_698,				 12,
#else
	Array_698,					5,
#endif
		
	Structure_698, 	 			3, 
	OAD_698,                                4,      //Double_long_unsigned_698, 	4,  // OAD = 20000700
	Unsigned_698,				2,
	TI_698,						3,	
		
};//

static const BYTE T_PROB_STAT[]	= 
{ 
	Array_698,					1,
		
	Structure_698, 	 			3, 
		
	OAD_698,                                4,      //Double_long_unsigned_698, 	4,  // OAD = 20000700
		
	Array_698,					24,
	Double_long_unsigned_698, 	4, 
		
	INVOID_698,				1,

};//

static const BYTE T_PROB_STAT3[]	= 
{ 
	Array_698,					1,
		
	Structure_698, 	 			5, 
		
	OAD_698,                                4,      //Double_long_unsigned_698, 	4,  // OAD = 20000700
	Unsigned_698,				2,
	TI_698,						3,	
	Unsigned_698,				2,
	Enum_698,					1,			
};//

static const BYTE T_HG_STA[]	= 
{ 
	Structure_698,				2,
		
	Structure_698, 	 			5, 
		
	Double_long_unsigned_698, 	4,  // OAD = 20000700
		
	Long_unsigned_698,			2,
	Long_unsigned_698,			2,

	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4, 

	Structure_698, 	 			5, 
	
	Double_long_unsigned_698, 	4,  // OAD = 20000700
	
	Long_unsigned_698,			2,
	Long_unsigned_698,			2,
	
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4, 

};


static const BYTE T_FAT_STA[]	= 
{ 
        Array_698,		        1,

	Structure_698, 	 	        2, 
        OAD_698,                        4,      //Double_long_unsigned_698, 	4, 
        
	Array_698,			3,

	Structure_698, 	 		2, 
		
	Double_long_unsigned_698, 	4,  // OAD = 20000700
		
	Double_long_unsigned_698, 	4, 
			
};

static const BYTE T_FAT_STA_PARA[]	= 
{ 
	Array_698,					1,
		
	Structure_698, 	 			4, 
		
	OAD_698,                                4,      //Double_long_unsigned_698, 	4,  // OAD = 20000700

	Array_698,					3,

	Region_698,					7,

	Unsigned_698,				1,

	TI_698,						3,
};
const BYTE T_44000200AppLink2[] =
{
	Array_698,			1,           //对象列表∷=array 一个可访问对象
		Structure_698,	    2,			//一个可访问对象∷=structure {	对象标识  OI，	访问权限  structure}
		
		Long_unsigned_698,  2,			//对象标识  OI
		Structure_698,	    2,          //访问权限  structure
		
		Array_698,			1,			//属性访问权限  array 一个属性访问权限，
		Array_698,			1,			//方法访问权限  array 一个方法访问权限
		
		Structure_698,	    2,			//一个属性访问权限∷=structure
		Unsigned_698,		1,			//属性ID  			unsigned
		Unsigned_698,		1,			//属性访问权限类别  	enum
		
		Structure_698,	    2,			//一个方法访问权限∷=structure
		Unsigned_698,		1,			//方法ID  		unsigned，
		Unsigned_698,		1,			//方法访问权限	bool
};

static const BYTE T_BODONG_PARA[]	= 
{ 
	Long_unsigned_698,  2,			//对象标识  OI
};
/*

static const BYTE T_PROB_STAT[]	= 
{ 
	Array_698,					1,
		
	Structure_698, 	 			2, 

	Double_long_unsigned_698, 	4,  // OAD = 20000700

	Array_698,					24,
	Structure_698, 	 			2, 

	Double_long_unsigned_698, 	4, 

	DateTime_S_698,				7,
		
};//
*/
//增加数据类型请查看源数据和加TAG是否支持
static const TCommonObj VariableObj[] =
{

	{  0x2000,  &SU_1V,   	T_NILM_ModuleValue,		MT_MLT_PH,  	2,		U,           0	},					//电压,分相数值数组 zh20240430
	{  0x2000,  &SU_4V,   	T_UNDoubleLong,		MT_SIG_PH,  	6,		U_SEQ_ZERO,      0	},					//电压,零序电压
	{  0x2000,  &SU_1V,   	T_ModuleValueCOS,	MT_MLT_PH,  	7,		U_DEV,           0	},					//电压,分相偏差数值数组
	{  0x2000,  &SU_4V,   	T_UNDoubleLong,		MT_SIG_PH,  	9,		U_SEQ_POS,       0	},					//电压,正序电压
	{  0x2000,  &SU_4V,   	T_UNDoubleLong,		MT_SIG_PH,  	10,		U_SEQ_NEG,       0	},					//电压,负序电压
	
	{  0x2001,  &SU_3A,     T_NILM_ModuleValue_I,   MT_MLT_PH,      2,	        I,              0	},					//电流,分相数值数组 zh20240430
	{  0x2001,  &SU_4A,     T_ModuleValue_I,        MT_MLT_PH,      5,	        I_HIGHPREC,     0       },					//属性5（高精度电流）∷=double-long
	{  0x2001,  &SU_4A,     T_DoubleLong,           MT_SIG_PH,      6,	        I_SEQ_ZERO,     0	},					//属性6（零序电流）∷=double-long， 单位：A，换算：-3
	{  0x2001,  &SU_4A,     T_ScalerUnit,           MT_MLT_PH,      7,	        I,              0	},					//属性7（高精度电流换算及单位）∷=Scaler_Unit，单位：A，换算：-4
	{  0x2001,  &SU_4A,     T_DoubleLong,           MT_SIG_PH,      9,	        I_SEQ_POS,      0	},					//属性9（正序电流）∷=double-long， 单位：A，换算：-3
	{  0x2001,  &SU_4A,     T_DoubleLong,           MT_SIG_PH,      10,	        I_SEQ_NEG,      0	},					//属性10（负序电流）∷=double-long， 单位：A，换算：-3

	{  0x200A,  &SU_3,      T_ModuleValueCOS4,      MT_MLT_PH,      2,	        PF,             0   	},					//功率因数
	{  0x200F,  &SU_4V,   	T_UNDoubleLong,		MT_SIG_PH,	2,		HZ,             0	},					//频率
	{  0x200F,  &SU_4V,   	T_DoubleLong,	   	MT_SIG_PH,	4,		HZ_DEV,         0	},					//频率,偏差数值

	//{  0x2026,  &SU_2PCT, 	T_UNLong,			MT_MLT_PH,		2,		ImbalanceU},					//电压不平衡, 属性2（不平衡率）∷=long-unsigned，单位：%，换算：-2
	{  0x2026,  &SU_2PCT, 	T_UNLong,		MT_MLT_PH,	4,		U_UNBALANCE_NEG,      0},					//电压不平衡, 属性4（负序不平衡度）∷=long-unsigned，单位：%，换算：-2
	{  0x2026,  &SU_2PCT, 	T_UNLong,		MT_MLT_PH,	5,		U_UNBALANCE_ZERO,     0},					//电压不平衡, 属性5（零序不平衡度）∷=long-unsigned，单位：%，换算：-2

	//{  0x2027,  &SU_2PCT, 	T_Long,				MT_MLT_PH,		2,		ImbalanceI},					//电流不平衡, 属性4（负序不平衡度）∷=long-unsigned，单位：%，换算：-2
	{  0x2027,  &SU_2PCT, 	T_UNLong,		MT_MLT_PH,	4,		I_UNBALANCE_NEG,      0},					//电流不平衡, 属性4（负序不平衡度）∷=long-unsigned，单位：%，换算：-2
	{  0x2027,  &SU_2PCT, 	T_UNLong,		MT_MLT_PH,	5,		I_UNBALANCE_ZERO,     0},					//电流不平衡, 属性5（零序不平衡度）∷=long-unsigned，单位：%，换算：-2
	
	{  0x2035,  &SU_4V,   	T_ModuleValueChange,	MT_MLT_PH,	2,		UChange	,             0},					//电压波动   老的标准
	{  0x2035,  &SU_4V,   	T_ModuleValueChange2,	MT_MLT_PH,	3,		UChange3,             0},					//电压波动   新的标准
	
	{  0x2036,  &SU_4V,   	T_ModuleValueChange,	MT_MLT_PH,	2,		UChangeFre,           0	},					//湖南老版本 闪变
	
	{  0x2037,  &SU_4V,   	T_ModuleValue,		MT_MLT_PH,	2,		UFlicker,             0	},				//电压闪变      短时
	{  0x2038,  &SU_4V,   	T_ModuleValue,		MT_MLT_PH,	2,		UFlickerLong,         0	},				//电压闪变      长时


	{  0x2046,  &SU_4V,   	T_U,			MT_MLT_TM,		2,		HarmonicU,      0	},					//谐波电压
	{  0x2046,  &SU_4V,   	T_U,			MT_MLT_TM,		3,		HarmonicU,      0	},					//谐波
	{  0x2046,  &SU_4V,   	T_U,			MT_MLT_TM,		4,		HarmonicU,      0	},					//谐波

	{  0x2047,  &SU_4A,   	T_I,			MT_MLT_TM,		2,		HarmonicI,      0	},					//谐波电流
	{  0x2047,  &SU_4A,   	T_I,			MT_MLT_TM,		3,		HarmonicI,      0	},					//谐波
	{  0x2047,  &SU_4A,   	T_I,			MT_MLT_TM,		4,		HarmonicI,      0	},					//谐波

	
	{  0x2048,  &SU_4W,   	T_P,			MT_MLT_TM,		2,		HarmonicP,      0	},					//谐波功率
	{  0x2048,  &SU_4W,   	T_P,			MT_MLT_TM,		3,		HarmonicP,      0	},					//谐波
	{  0x2048,  &SU_4W,   	T_P,			MT_MLT_TM,		4,		HarmonicP,      0	},					//谐波
	{  0x2048,  &SU_4W,   	T_P_ALL,		MT_SIG_PH,		7,		HarmonicPALL,      0},					//谐波功率

	{  0x2060,  &SU_4A,   	T_U,			MT_MLT_TM,		2,		IHarmonicU,      0	},					//间谐波电压
	{  0x2060,  &SU_4A,   	T_U,			MT_MLT_TM,		3,		IHarmonicU,      0	},					//间谐波电压
	{  0x2060,  &SU_4A,   	T_U,			MT_MLT_TM,		4,		IHarmonicU,      0	},					//间谐波电压

	{  0x2061,  &SU_4V,   	T_I,			MT_MLT_TM,		2,		IHarmonicI,      0	},					//间谐波电流
	{  0x2061,  &SU_4V,   	T_I,			MT_MLT_TM,		3,		IHarmonicI,      0	},					//间谐波电流
	{  0x2061,  &SU_4V,   	T_I,			MT_MLT_TM,		4,		IHarmonicI,      0	},					//间谐波电流

	{  0x2062,  &SU_4A,   	T_P,			MT_MLT_TM,		2,		IHarmonicP,      0	},					//间谐波功率
	{  0x2062,  &SU_4A,   	T_P,			MT_MLT_TM,		3,		IHarmonicP,      0	},					//间谐波功率
	{  0x2062,  &SU_4A,   	T_P,			MT_MLT_TM,		4,		IHarmonicP,      0	},					//间谐波功率

	{  0x2063,  &SU_4V,   	T_VI_COUNT,		MT_MLT_TM,		2,		HarmonicUDayCount,      0	},			//本日谐波电压统计
	{  0x2063,  &SU_4V,   	T_VI_COUNT,		MT_MLT_TM,		3,		HarmonicUDayCount,      0	},			//本日谐波电压统计
	{  0x2063,  &SU_4V,   	T_VI_COUNT,		MT_MLT_TM,		4,		HarmonicUDayCount,      0	},			//本日谐波电压统计

	{  0x2064,  &SU_4A,   	T_VI_COUNT,		MT_MLT_TM,		2,		HarmonicIDayCount,      0	},			//本日谐波电流统计
	{  0x2064,  &SU_4A,   	T_VI_COUNT,		MT_MLT_TM,		3,		HarmonicIDayCount,      0	},			//本日谐波
	{  0x2064,  &SU_4A,   	T_VI_COUNT,		MT_MLT_TM,		4,		HarmonicIDayCount,      0	},			//本日谐波

	{  0x2065,  &SU_4V,   	T_VI_COUNT,		MT_MLT_TM,		2,		IHarmonicUDayCount,      0	},			//本日间谐波电压统计
	{  0x2065,  &SU_4V,   	T_VI_COUNT,		MT_MLT_TM,		3,		IHarmonicUDayCount,      0	},			//本日间谐波电压统计
	{  0x2065,  &SU_4V,   	T_VI_COUNT,		MT_MLT_TM,		4,		IHarmonicUDayCount,      0	},			//本日间谐波电压统计

	{  0x2066,  &SU_4A,   	T_VI_COUNT,		MT_MLT_TM,		2,		IHarmonicIDayCount,      0	},			//本日间谐波电流统计
	{  0x2066,  &SU_4A,   	T_VI_COUNT,		MT_MLT_TM,		3,		IHarmonicIDayCount,      0	},			//本日间谐波电流统计
	{  0x2066,  &SU_4A,   	T_VI_COUNT,		MT_MLT_TM,		4,		IHarmonicIDayCount,      0	},			//本日间谐波电流统计

	{  0x2067,  &SU_4A,   	T_VChange_COUNT,	MT_MLT_TM,		2,		PSTDayCount,      0	},			//本日闪变统计
	
	//wxy add
	{  0x2042,  &SU_4V,   	T_BitString32,		MT_SIG_PH,		4,		MODULE_STATUS,      0		},//20420400—电能质量模组状态字

	{  0x200D,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		2,		HarmUContent,      2		},//200D0200—A相谐波电压含有率
	{  0x200D,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		3,		HarmUContent,      2		},//200D0200—A相谐波
	{  0x200D,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		4,		HarmUContent,      2		},//200D0200—A相谐波

//        {  0x200D,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		2,		HarmUContent,      4		},//200D0200—A相谐波电压含有率//zh
//	{  0x200D,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		3,		HarmUContent,      4		},//200D0200—A相谐波//zh
//	{  0x200D,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		4,		HarmUContent,      4		},//200D0200—A相谐波//zh
        
        
	{  0x200E,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		2,		HarmIContent,      2		},//200E0200—A相谐波电流含有率
	{  0x200E,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		3,		HarmIContent,      2		},//200E0200—A相谐波
	{  0x200E,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		4,		HarmIContent,      2		},//200E0200—A相谐波

//        {  0x200E,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		2,		HarmIContent,      4		},//200E0200—A相谐波电流含有率//zh
//	{  0x200E,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		3,		HarmIContent,      4		},//200E0200—A相谐波//zh
//	{  0x200E,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		4,		HarmIContent,      4		},//200E0200—A相谐波//zh
        
        
	{  0x2049,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		2,		IHarmUContent,      2		},//20490200—A相间谐波电压含有率 
	{  0x2049,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		3,		IHarmUContent,      2		},//20490200—A相间谐波电压含有率 
	{  0x2049,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		4,		IHarmUContent,      2		},//20490200—A相间谐波电压含有率 

//        {  0x2049,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		2,		IHarmUContent,      4		},//20490200—A相间谐波电压含有率 //zh
//	{  0x2049,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		3,		IHarmUContent,      4		},//20490200—A相间谐波电压含有率 //zh
//	{  0x2049,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		4,		IHarmUContent,      4		},//20490200—A相间谐波电压含有率 //zh

        
	{  0x204A,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		2,		IHarmIContent,      2		},//204A0200—A相间谐波电流含有率
	{  0x204A,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		3,		IHarmIContent,      2		},//204A0200—A相间谐波电流含有率 
	{  0x204A,  &SU_4V,   	T_VI_COUNT2,		MT_MLT_TM,		4,		IHarmIContent,      2		},//204A0200—A相间谐波电流含有率 

//        {  0x204A,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		2,		IHarmIContent,      4		},//204A0200—A相间谐波电流含有率//zh
//	{  0x204A,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		3,		IHarmIContent,      4		},//204A0200—A相间谐波电流含有率 //zh
//	{  0x204A,  &SU_4V,   	T_VI_COUNT4,		MT_MLT_TM,		4,		IHarmIContent,      4		},//204A0200—A相间谐波电流含有率 //zh

        
        
	{  0x2068,  &SU_4V,   	T_VChangeLong_COUNT,	MT_MLT_TM,		2,		U_DEV_DayCount,      0		},			//当日电压偏差统计
	{  0x2069,  &SU_4V,   	T_VChangeLong_COUNT,	MT_MLT_TM,		2,		HZ_DEV_DayCount,      0		},			//当日频率偏差统计
	{  0x206A,  &SU_4V,   	T_VChangeLong_COUNT,	MT_MLT_TM,		2,		U_SEQ_DayCount,      0		},			//当日三相电压不平衡度统计
	{  0x206B,  &SU_4V,   	T_VChangeLong_COUNT,	MT_MLT_TM,		2,		I_SEQ_DayCount,      0		},			//当日三相电流不平衡度统计
	{  0x206C,  &SU_4V,   	T_VChangeLong_COUNT,	MT_MLT_TM,		2,		UChange_DayCount,      0	},			//当日电压波动统计

	
//	{  0x200B,  &SU_4V,   	T_ModuleValueCOS_JBL,	MT_MLT_PH,		2,		U_THD,      2			},//——电压谐波总畸变率,电压波形失真度//zh
//        {  0x200C,  &SU_4V,   	T_ModuleValueCOS_JBL,	MT_MLT_PH,		2,		I_THD,      2			},//——电流谐波总畸变率,电流波形失真度//zh
 
//        {  0x200B,  &SU_4V,   	T_ModuleValueCOS_JBL4,	MT_MLT_PH,		2,		U_THD,      4			},//——电压谐波总畸变率,电压波形失真度//zh
//        {  0x200C,  &SU_4V,   	T_ModuleValueCOS_JBL4,	MT_MLT_PH,		2,		I_THD,      4			},//zh
	
	{  0x2122,  &SU_4V,   	T_EXTM_STAT,		MT_EXT_STA,		2,		ExtremStat,      0			},
	{  0x2127,  &SU_4V,   	T_PROB_STAT,		MT_PRO_STA,		2,		ProbabilityStat,      0		},

	{  0x2131,  &SU_4V,   	T_HG_STA,		MT_SIG_PH,		2,		HGStat_A,      0			}, //A 相电压合格率
	{  0x2132,  &SU_4V,   	T_HG_STA,		MT_SIG_PH,		2,		HGStat_B,      0			}, //B 相电压合格率
	{  0x2133,  &SU_4V,   	T_HG_STA,		MT_SIG_PH,		2,		HGStat_C,      0			}, //C 相电压合格率

	{  0x2103,  &SU_4V,   	T_FAT_STA,		MT_MLT_PH,		2,		FATStat,      0			}, //功率因数区间
	{  0x2103,  &SU_4V,   	T_FAT_STA_PARA,		MT_MLT_PH,		3,		FATStatPara,      0		}, //功率因数区间参数

	{  0x2035,  &SU_4V,   	T_BODONG_PARA,		MT_MLT_PH,		4,		BodongPara,      0		}, //波动阈值参数
	{  0x2122,  &SU_4V,   	T_EXTM_STAT3,		MT_EXT_STA,		3,		ExtremStat3,      0		},
	{  0x2127,  &SU_4V,   	T_PROB_STAT3,		MT_PRO_STA,		3,		ProbabilityStat3,      0	},


//加TAG专用
	{  0x201e,  &SU_00, 	T_DateTimS,		MT_MLT_PH,		2,		0xFF,      0},//事件发生时间
	{  0x2020,  &SU_00, 	T_DateTimS,		MT_MLT_PH,		2,		0xFF,      0},//事件结束时间
	{  0x2021,  &SU_00, 	T_DateTimS,		MT_MLT_PH,		2,		0xFF,      0},//数据冻结时间
	{  0x2022,  &SU_00, 	T_UNDoubleLong,		MT_MLT_PH,		2,		0xFF,      0},//事件记录序号
	{  0x2023,  &SU_00, 	T_UNDoubleLong,		MT_MLT_PH,		2,		0xFF,      0},//冻结记录序号
	{  0x2024,  &SU_00, 	T_Enum,			MT_MLT_PH,		2,		0xFF,      0},//事件发生源
	{  0x2025,  &SU_00, 	T_EventNowData,		MT_MLT_PH,		2,		0xFF,      0},//事件当前值

	{  0x2004,  &SU_1W,     T_NILM_ModuleValue_P,	MT_MLT_PH,      2,	    NILM_P,0},//有功功率
};								
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
extern WORD api_GetExtremeStatDataLen(BYTE Index);
extern WORD api_GetExtremeStatData_AddTag( BYTE Index, BYTE* InBuf, BYTE *OutBuf );
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

BOOL IsSpecialVariableOAD(WORD OI, BYTE byVip)
{
//zh
//	if(OI == 0x200B || OI == 0x200C)
//	{
//		if(gDefaultPara.byDistortionFactorPoint != 2 && gDefaultPara.byDistortionFactorPoint!=4)
//			return TRUE;
//
//		if(gDefaultPara.byDistortionFactorPoint == byVip)
//			return TRUE;
//	}
//	else
//	{
//		if(gDefaultPara.byContentPoint != 2 && gDefaultPara.byContentPoint!=4)
//			return TRUE;
//
//		if(gDefaultPara.byContentPoint == byVip)
//			return TRUE;
//	}
	return FALSE;

}




BYTE *GetSecondVariableObj(WORD wOad)
{
//zh
//	int i;
//	int Num = (sizeof(VariableObj2)/sizeof(TCommonObj));
//
//	for(i=0; i<Num; i++)
//	{
//		if(VariableObj2[i].OI == wOad)
//		{
//                      if(VariableObj2[i].byVip == 0 || IsSpecialVariableOAD(VariableObj2[i].OI, VariableObj2[i].byVip) )
//                          return (BYTE*)VariableObj2[i].Type2;
//		}
//	}
	return NULL;
}


//--------------------------------------------------
//功能描述:  根据OI查找对应变量索引
//         
//参数:      OI[in]:	OI
//         
//返回值:    变量数组索引号
//         
//备注:  
//--------------------------------------------------
static BYTE SearchVariableOAD( WORD OI, BYTE byProperty)
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(VariableObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		//20220426 wxy 
		//if( OI == VariableObj[i].OI && ((byProperty == VariableObj[i].Property) || ((byProperty <= 4) && (VariableObj[i].wMode != MT_SIG_PH) ) ))
		if( OI == VariableObj[i].OI && ((byProperty == VariableObj[i].Property) || ((byProperty <= 2) && (VariableObj[i].wMode != MT_SIG_PH) ) ))
		{
                      if(VariableObj[i].byVip == 0 || IsSpecialVariableOAD(OI, VariableObj[i].byVip) )
                          return i;
		}
	}

	if( i == Num )//未找到
	{
		return 0x80;
	}
	return 0x80;
}

//--------------------------------------------------
//功能描述:  获取变量数据
//         
//参数:      
//			Dot[in]:		获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//			*pOAD[in]:		OAD         
//			VariableIndex[in]:VariableObj中的索引         
//			*OutBuf[in]:	输出变量
//         
//返回值:   返回变量数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注: 不带tag
//--------------------------------------------------
WORD api_GetModuleDataEx(BYTE VariableIndex, BYTE ClassAttribute, BYTE ClassIndex, BYTE *InBuf )
{
    WORD Len;
//    BYTE bPhase=0;
    SDWORD n = 0, i, Num;
    BYTE *Tag;
    BYTE *pBufBak = InBuf;

    
	Tag = (BYTE*)&VariableObj[VariableIndex].Type2[0];

	Len = 0;

    Num = Tag[n+1];
    
	// Phase = ( (ClassIndex == 0) ? 0 : (ClassIndex));

    Num = ((ClassIndex == 0) ? Num : 1);
    i = ( (ClassIndex == 0) ? 0 : (ClassIndex));
    Num = Num + i;

    for(; i<Num; i++)
    {
        Len = api_GetModuleData( VariableObj[VariableIndex].wDataType, i, i, InBuf );
        
        if( Len == 0x8000 )
        {
            return 0x8000;
        }
        InBuf += Len;
    }

	return (WORD)(InBuf-pBufBak);
} 


///////////////////////////////////////////////////////////////
//	函 数 名 : GetVariableData
//	函数功能 : 简化版，测试中
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年7月2日
//	返 回 值 : WORD
//	参数说明 :  BYTE Ch,
//				 BYTE Dot,
//				 BYTE *pOAD,
//				 BYTE VariableIndex,
//				 BYTE *InBuf 
///////////////////////////////////////////////////////////////
static WORD GetVariableData( BYTE Ch, BYTE Dot, BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf )
{
	TTwoByteUnion OI;
	// TFourByteUnion TempData;
	BYTE ClassAttribute,ClassIndex,Num,i,Phase;//,Type,SpecialFlag;
	WORD Len;//VariableType,Result
	BYTE *pBufBak = InBuf;
	// const BYTE *Tag;

	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Len =0;

	if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
	{
	//	return 0x8000;
	}
				
	Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
	if( Len == 0x8000 )
	{
	//wxy	return 0x8000;
	}
	//return api_GetModuleDataEx(VariableIndex, ClassAttribute, ClassIndex, InBuf );
	
	if(VariableObj[VariableIndex].wMode == MT_MLT_PH)
	{
	/*	Phase = 0;
		i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
		Num = ((ClassIndex==0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
		Num = Num+i;
		i = 0;*/
		Phase = (ClassAttribute-2);
		Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
		i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
		Num = Num+i;

	}
	else if(VariableObj[VariableIndex].wMode == MT_MLT_TM)
	{	
		Phase = (ClassAttribute-2);
		Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
		i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
		Num = Num+i;
	}
	else if(VariableObj[VariableIndex].wMode == MT_EXT_STA)//wxy add 20210809
	{	
		Phase = ( (ClassIndex ==0) ? 0 : (ClassIndex));
		Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
		i = ( (ClassIndex ==0) ? 1 : (ClassIndex));
		Num = Num+i;
	}
	else if(VariableObj[VariableIndex].wMode == MT_PRO_STA)//wxy add 20210809
	{	
		Phase = ( (ClassIndex ==0) ? 0 : (ClassIndex));
		Num = (VariableObj[VariableIndex].Type2[1]);
		i = 0;
		Num = Num+i;
	}
	else
	{
		Phase = 0;
		i = 0;
		Num = 1;
	}
	for( ; i<Num; i++ )//i=0
	{
		if(VariableObj[VariableIndex].wMode == MT_MLT_TM)
		{
			Len = api_GetModuleData( VariableObj[VariableIndex].wDataType, Phase, i, InBuf );
		}
		else if(VariableObj[VariableIndex].wMode == MT_EXT_STA)
		{
			Len = api_GetModuleData( VariableObj[VariableIndex].wDataType, Phase, i, InBuf );
		}
		else if(VariableObj[VariableIndex].wMode == MT_PRO_STA)
		{
			Len = api_GetModuleData( VariableObj[VariableIndex].wDataType, Phase, i, InBuf );
		}
		else
		{
			Len = api_GetModuleData(VariableObj[VariableIndex].wDataType, i, ClassIndex, InBuf );
		}
		
        if( Len == 0x8000 )
        {
             return 0x8000;
        }
		InBuf += Len;
	}

	return  (WORD)(InBuf-pBufBak);
}
/////////////////////////////////////////////////////////////////
////	函 数 名 : GetVariableData2
////	函数功能 : 暂时封闭不用，用上面的简化版
////	处理过程 : 
////	备    注 : 
////	作    者 : 智能电表组
////	时    间 : 2021年7月2日
////	返 回 值 : WORD
////	参数说明 :  BYTE Ch,
////				 BYTE Dot,
////				 BYTE *pOAD,
////				 BYTE VariableIndex,
////				 BYTE *InBuf 
/////////////////////////////////////////////////////////////////
//static WORD GetVariableData2( BYTE Ch, BYTE Dot, BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf )
//{
//	TTwoByteUnion OI;
//	TFourByteUnion TempData;
//	BYTE ClassAttribute,ClassIndex,Num,i,Phase,Type;//,SpecialFlag;
//	WORD VariableType,Len;//,Result
//	BYTE *pBufBak = InBuf;
//	const BYTE *Tag;
//	
//	lib_ExchangeData(OI.b,pOAD,2);
//	ClassAttribute = (pOAD[2]&0x0f);
//	ClassIndex = pOAD[3];
//	Len =0;
//	// SpecialFlag = 0;
//	
//	switch( ClassAttribute )
//	{
//		case 2://A相扩展次数
//			if( OI.w == 0x2000 )//电压
//			{
//				if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
//				{
//					return 0x8000;
//				}
//				
//				Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
//				if( Len == 0x8000 )
//				{
//					return 0x8000;
//				}
//				
//				i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
//				Num = ((ClassIndex==0) ? 3 : 1);
//				Num = Num+i;
//				
//				for( i=0; i<Num; i++ )
//				{
//					Len = api_GetModuleData( U, i, 0, InBuf );
//                    if( Len == 0x8000 )
//                    {
//                        return 0x8000;
//                    }
//					InBuf += Len;
//				}
//				break;
//			}
//            else if( OI.w == 0x2001 )//电流
//			{
//				if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
//				{
//					return 0x8000;
//				}
//				
//				Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
//				if( Len == 0x8000 )
//				{
//					return 0x8000;
//				}
//				
//				i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
//				Num = ((ClassIndex==0) ? 3 : 1);
//				Num = Num+i;
//				
//				for( i=0; i<Num; i++ )
//				{
//					Len = api_GetModuleData( I, i, 0, InBuf );
//                    if( Len == 0x8000 )
//                    {
//                        return 0x8000;
//                    }
//					InBuf += Len;
//				}
//				break;
//			}
//			else if( OI.w == 0x200F )//频率
//			{
//				if( ClassIndex != 0 )
//				{
//					return 0x8000;
//				}
//
//				Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[0] );
//				if( Len == 0x8000 )
//				{
//					return 0x8000;
//				}
//
//				//调用接口函数
//				Len = api_GetModuleData( HZ, 0, 0, InBuf );
//                if( Len == 0x8000 )
//                {
//                    return 0x8000;
//                }
//				InBuf += Len;
//				break;
//			}
//			else if( (OI.w == 0x2035) || (OI.w == 0x2036) )//电压波动\电压闪变
//			{
//				if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
//				{
//					return 0x8000;
//				}
//				
//				if( OI.w == 0x2035 )
//				{
//					Type = UChange;
//				}
//				else if(OI.w == 0x2036)
//				{
//					Type = UFlicker;
//				}
//				
//				Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
//				if( Len == 0x8000 )
//				{
//					return 0x8000;
//				}
//				
//				i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
//				Num = ((ClassIndex==0) ? 3 : 1);
//				Num = Num+i;
//				
//				for( i=0; i<Num; i++ )
//				{
//					
//					//---调用接口函数 读取电压波动和电压波动频率
//					Len = api_GetModuleData( Type, i, 0, InBuf );
//                    if( Len == 0x8000 )
//                    {
//                        return 0x8000;
//                    }
//					InBuf += Len;
//				}
//				break;
//			}
//			else if( OI.w == 0x2067 )
//			{
//				if( ClassIndex > 3 )
//				{
//					return 0x8000;
//				}
//				
//				Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
//				if( Len == 0x8000 )
//				{
//					return 0x8000;
//				}
//				
//				i = ((ClassIndex==0) ? 0 : (ClassIndex-1));
//				Num = ((ClassIndex==0) ? 3 : 1);
//				Num = Num+i;
//				
//				for( i=0; i<Num; i++ )
//				{
//					
//					//---调用接口函数 读取闪变
//					Len = api_GetModuleData( PSTDayCount, i, 0, InBuf );
//                    if( Len == 0x8000 )
//                    {
//                        return 0x8000;
//                    }
//					InBuf += Len;
//				}
//				break;
//			}
//			else//进行下面判断，不能break
//			{
//
//			}
//
//		case 3://B相扩展次数
//		case 4://C相扩展次数
//			switch( OI.w )
//			{
//				case 0x2046://谐波电压
//				case 0x2047://谐波电流
//				case 0x2048://谐波功率
//				case 0x2060://间谐波电压
//				case 0x2061://间谐波电流
//				case 0x2062://间谐波功率
//					if( ClassIndex > VariableObj[VariableIndex].Type2[1])
//					{
//						return 0x8000;
//					}
//					
//					Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
//    			    if( Len == 0x8000 )
//                    {
//                        return 0x8000;
//                    }
//                    
//                    if( OI.w < 0x2060 )
//                    {
//						VariableType = HarmonicU+(OI.w-0x2046);
//                    }
//                    else
//                    {
//						VariableType = IHarmonicU+(OI.w-0x2060);
//                    }
//                    
//                    Phase = (ClassAttribute-2);
//					Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
//					i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
//					Num = Num+i;
//					for( ; i<Num ; i++ )
//					{
//						Len = api_GetModuleData( VariableType, Phase, i, InBuf );
//                        if( Len == 0x8000 )
//                        {
//                            return 0x8000;
//                        }
//						InBuf += Len;
//					}
//					break;
//				
//				case 0x2063://本日谐波电压统计
//				case 0x2064://本日谐波电流统计
//				case 0x2065://本日间谐波电压统计
//				case 0x2066://本日间谐波电流统计
//					if( ClassIndex > VariableObj[VariableIndex].Type2[1])
//					{
//						return 0x8000;
//					}
//					Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
//    			    if( Len == 0x8000 )
//                    {
//                        return 0x8000;
//                    }
//                    
//					Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
//					i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
//					Num = Num+i;
//					
//					VariableType = HarmonicUDayCount+(OI.w-0x2063);
//					Phase = (ClassAttribute-2);
//					
//					for( ; i<Num ; i++ )
//					{					
//						//调用接口函数
//						Len = api_GetModuleData( VariableType, Phase, i, InBuf );
//						if( Len == 0x8000 )
//                        {
//                            return 0x8000;
//                        }
//						InBuf += Len;
//					}
//					break;
//				default:
//					return 0x8000;
//			}
//			break;
//		case 5:
//			if( OI.w == 0x2000 )
//			{
//				Len = api_GetModuleData( U, 0, 0, TempData.b );
//				TempData.l = (2200000 - TempData.l)*100/TempData.l;
//				memcpy(InBuf, TempData.b, 2);
//				InBuf += 2;
//			}
//			else if( OI.w == 0x200F )
//			{
//				Len = api_GetModuleData( HZ, 0, 0, TempData.b );
//				TempData.l = TempData.d - 500000;
//				memcpy(InBuf, TempData.b, 4);
//				InBuf += 4;
//			}
//			break;
//		case 6://换算及单位
//			if( ClassIndex != 0 )
//			{
//				return 0x8000;
//			}
//			InBuf[0] = VariableObj[VariableIndex].Scaler_Unit3->Scaler;
//			InBuf[1] = VariableObj[VariableIndex].Scaler_Unit3->Unit;
//			InBuf += 2;
//			break;
//
//		case 7://谐波总有功功率
//			if( OI.w != 0x2048 )
//			{
//				return 0x8000;
//			}
//			
//			if( ClassIndex > 2 )
//			{
//				return 0x8000;
//			}
//			Tag = T_P_ALL;
//			Len = GetTypeLen( eData, Tag+2 );
//			if( Len == 0x8000 )
//			{
//				return 0x8000;
//			}
//
//			Len = api_GetModuleData( HarmonicPALL, 0, 0, InBuf );
//            if( Len == 0x8000 )
//            {
//                return 0x8000;
//            }
//			InBuf += Len;
//
//			break;
//		
//		default:
//			return 0x8000;
//	}
//
//	return  (WORD)(InBuf-pBufBak);
//}

//--------------------------------------------------
//功能描述:  对变量数据进行加TAG
//         
//参数:      *pOAD[in]：		OAD         
//           VariableIndex[in]:	VariableObj中的索引         
//           *InBuf[in]：		变量要添加tag的数据         
//           OutBufLen[in]：	OutBuf长度       
//           *OutBuf[out]：		输出数据
//         
//返回值:    返回添加tag后数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注:  	无
//--------------------------------------------------
static WORD GetVariableAddTag(BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf, BYTE byType)
{
	BYTE ClassAttribute,ClassIndex,i;//,Num
	BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth,Lenth1;
	
//        BYTE* byOldTag;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0x8000;

	switch( ClassAttribute )
	{
		case 2://A相扩展次数
			if(( OI.w == 0x200F )||( OI.w == 0x21a0 )||( OI.w == 0x21a1 ))
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}	
				break;
			}
			else if( OI.w == 0x2067 )
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}

				Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				Lenth1 = GetTypeLen( eData, Tag );

				if( ClassIndex == 0 )
				{
					OutBuf[0] = Array_698;
					OutBuf[1] = 0x03;
					OutBuf +=2;
					for( i=0; i<3; i++ )
					{
						OutBuf[0] = Structure_698;
						OutBuf[1] = 0x02;
						OutBuf +=2;
						if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
						{
							return 0;
						}
						
						Result = GetRequestAddTag( Tag, InBuf, OutBuf );
						if( Result == 0x8000 )
						{
							return 0x8000;
						}
						
						InBuf += Lenth1;
						OutBuf += Lenth;
						Result = GetRequestAddTag( Tag, InBuf, OutBuf );
						if( Result == 0x8000 )
						{
							return 0x8000;
						}
						InBuf += Lenth1;
						OutBuf += Lenth;
					}

					Lenth = 2+(Lenth*2+2)*3;
				}
				else
				{
					OutBuf[0] = Structure_698;
					OutBuf[1] = 0x02;
					OutBuf +=2;
					if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
					{
						return 0;
					}
					
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					
					InBuf += Lenth1;
					OutBuf += Lenth;
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					Lenth = Lenth*2+2;
				}	
				break;
			}
			else if(( OI.w == 0x2000 ) || ( OI.w == 0x2001 ) || ( OI.w == 0x2004 ) )//电压,电流，功率zh20240430 加tag
			{
				if(OI.w == 0x2004)
				{
					if( ClassIndex > (MAX_PHASE+1) )
					{
						return 0x8000;
					}
				}
				else
				{
					if( ClassIndex > MAX_PHASE )
					{
						return 0x8000;
					}
				}

				if(ClassIndex == 0)
				{
					Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				}
				else
				{
					Tag = (BYTE*)&VariableObj[VariableIndex].Type2[2];
				}
				Lenth = GetTypeLen( eTagData, Tag );
				if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );


				if( Result == 0x8000 )
				{
					return 0x8000;
				}	
				break;
			}

		case 3://B相扩展次数
		case 4://C相扩展次数
		case 5:
			if( ClassIndex > VariableObj[VariableIndex].Type2[1] )
			{
				//return 0x8000;
			}
			if(ClassIndex == 0)
				Tag = (BYTE*)VariableObj[VariableIndex].Type2;
			else
				Tag = (BYTE*)&VariableObj[VariableIndex].Type2[2];
                        
#if(USE_HARN_FREEZE_25)  
                        if(byType == 0x55 )
			{
				if(ClassIndex == 0xFF)
				{
					Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				}
                                
                                {
                                      byOldTag = GetSecondVariableObj(OI.w);
                                      if(byOldTag != NULL && (ClassIndex == 0) )
                                              Tag = byOldTag;
                                }
                                
				/*if(OI.w == 0x200D || OI.w == 0x200E || OI.w == 0x2047 || OI.w == 0x2046)
				{
					if(ClassIndex == 0)
						Tag[1] = 25;
				}
				else if(OI.w == 0x2049 || OI.w == 0x204A || OI.w == 0x2060 || OI.w == 0x2061)
				{
					if(ClassIndex == 0)
						Tag[1] = 21;
				}*/
				
			}
#endif                        
			if(VariableObj[VariableIndex].wMode != MT_PRO_STA)
			{
					if(VariableObj[VariableIndex].wDataType == HGStat_A
						|| VariableObj[VariableIndex].wDataType == HGStat_B
					  || VariableObj[VariableIndex].wDataType == HGStat_C)
					{
							if(ClassIndex == 2) ClassIndex=1;
					}
				if(ClassIndex == 0)
					Tag += GetTagOffsetAddr( ClassIndex, Tag);
			}
			
			Lenth = GetTypeLenEx( eTagData, Tag,1);
		//	Lenth = GetTypeLen( eTagData, Tag);
			if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			Result = GetRequestAddTagEx( Tag, 1, InBuf, OutBuf );
			//Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			Lenth = Result;
			break;

		case 5555:
			if(( OI.w == 0x2000 ) || ( OI.w == 0x2001 ) || ( OI.w == 0x2026 ) || ( OI.w == 0x2027 ))
			{
				
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			else
			{
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( OI.w == 0x2000 )
			{
				Tag = (BYTE*)T_Long;
			}
			else if( OI.w == 0x200F )
			{
				Tag = (BYTE*)T_DoubleLong;
			}
			else
			{
				return 0x8000;
			}
			
			Result = GetRequestAddTag( T_ScalerUnit, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			Lenth = 3;
			}
			break;

		case 6://换算及单位
			if(( OI.w == 0x2000 ) || ( OI.w == 0x2001 ) || ( OI.w == 0x2004))//zh
			{

				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
				{
					return 0;
				}
					
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
			}
			else
			{
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( OutBufLen < 3 )//如果缓冲大小不够，则不进行操作
			{
				return 0;
			}
			
			Result = GetRequestAddTag( T_ScalerUnit, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			Lenth = 3;
			}
			break;

		case 7://谐波功率
			Tag = (BYTE*)T_P_ALL;
			if(( OI.w == 0x2000 )||( OI.w == 0x2001 ) ||( OI.w == 0x2048 ))//wxy
			{
				
				if( ClassIndex != 0 )
				{
					Tag =(BYTE*) &VariableObj[VariableIndex].Type2[2];
				}
				else
					Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				break;
			}
			if( OI.w != 0x2048 )
			{
				return 0x8000;
			}
			
			if( ClassIndex > Tag[1])
			{
				return 0x8000;
			}
			
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
			if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
        	if( OutBufLen < Lenth )
			{
				return 0	;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			
			break;
		case 9://
			if(( OI.w == 0x2000 )||( OI.w == 0x2001 ))
			{
				
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				break;
			}
			break;
		case 10://
			if(( OI.w == 0x2000 )||( OI.w == 0x2001 ))
			{
				
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = (BYTE*)VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag );
				if( OutBufLen < Lenth )//如果缓冲大小不够，则不进行操作
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				break;
			}
			break;
		default:
			return 0x8000;
	}

	return Lenth;	
}

//--------------------------------------------------
//功能描述:  读取变量
//         
//参数:      DataType[in]：	eData/eTagData/eAddTag        
//           Dot[in] :		小数点        
//           *pOAD[in]：	OAD         
//           *InBuf[in]：	需要添加标签的数据         
//           OutBufLen[in]：留给OutBuf的数据长度         
//           *OutBuf[in]：	返回的电能数据
//         
//返回值:    返回变量长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  
//--------------------------------------------------
WORD GetRequestVariable( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE VariableIndex;
	BYTE Buf[3000];//电压合格率源数据长度为32
	WORD Lenth;
	BYTE ClassAttribute;
	BYTE ClassIndex;
//const BYTE *Tag;

//int nLen=0;
	ClassAttribute = (pOAD[2]&0x0f);
	lib_ExchangeData(OI.b,pOAD,2);
	Lenth = 0;
	
	ClassIndex = (pOAD[3]&0x0f);
	VariableIndex = SearchVariableOAD( OI.w, ClassAttribute);
	if( VariableIndex == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType == eData) || (DataType == eTagData) )
	{
		Lenth = GetVariableData( Ch, Dot, pOAD, VariableIndex, Buf );
		if( Lenth == 0x8000 )
		{
			return Lenth;
		}
		if( DataType == eData )
		{
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			memcpy( OutBuf, Buf, Lenth );
		}
		else
		{
			if(OI.w == 0x2122 && ClassAttribute==2)
			{

				Lenth = api_GetExtremeStatData_AddTag(ClassIndex, Buf, OutBuf);

				return Lenth;
			}
			Lenth = GetVariableAddTag(pOAD, VariableIndex, Buf, OutBufLen, OutBuf, Ch);
		}
	}
	else if( DataType == eAddTag )
	{
		if(OI.w == 0x2122 && ClassAttribute==2)
		{

			Lenth = api_GetExtremeStatData_AddTag(ClassIndex, InBuf, OutBuf);
			
			return Lenth;
		}
		Lenth = GetVariableAddTag(pOAD, VariableIndex, InBuf, OutBufLen, OutBuf, Ch);
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}

//--------------------------------------------------
//功能描述:  根据TAG获取变量长度
//         
//参数:      
//			DataType[in]：	eData/eTagData
//			*pOAD[in]：		OAD
//         
//返回值:   返回OAD对应数据的长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestVariableLen( BYTE DataType, BYTE* pOAD, BYTE Number)
{
	BYTE ClassAttribute,VariableIndex,ClassIndex;
	TTwoByteUnion OI,Len;	
	BYTE *Type;
//	BYTE* byOldTag;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	VariableIndex = SearchVariableOAD( OI.w, ClassAttribute);
	if( VariableIndex == 0x80 )
	{
		return 0x8000;
	}
	
	Type = (BYTE*)VariableObj[VariableIndex].Type2;
	
	switch( ClassAttribute )
	{
		case 9:
		case 10:
		case 5://wxy add
		case 2://A相扩展次数
			if( OI.w == 0x200F )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Len.w = GetTypeLen( eTagData, Type );
				break;
			}
			else if( OI.w == 0x2067 )
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}
				
				Len.w = GetTypeLen( eTagData, Type );

				if( ClassIndex == 0 )
				{

					Len.w = 2+(Len.w*2+2)*3;
				}
				else
				{
					Len.w = Len.w*2+2;
				}	
				break;
			}
			else if(( OI.w == 0x2000 ) || ( OI.w == 0x2001 ) || ( OI.w == 0x2004 ) )//电压,电流，功率zh20240430 算len
			{
				if(OI.w == 0x2004)
				{
					if( ClassIndex > (MAX_PHASE+1) )
					{
						return 0x8000;
					}
				}
				else
				{
					if( ClassIndex > MAX_PHASE )
					{
						return 0x8000;
					}
				}
				if(ClassIndex == 0)
				{
					Type = (BYTE*)VariableObj[VariableIndex].Type2;
				}
				else
				{
					Type = (BYTE*)&VariableObj[VariableIndex].Type2[2];
				}
				Len.w = GetTypeLen( eTagData, Type );
				break;
			}
		case 3://B相扩展次数
		case 4://C相扩展次数
			if( ClassIndex > Type[1] )
			{
			//	return 0x8000;
			}

			if(VariableObj[VariableIndex].wMode != MT_PRO_STA)
				Type +=GetTagOffsetAddr( ClassIndex, Type );
			
 #if(USE_HARN_FREEZE_25)                        
			if(Number == 0x55)
			{
				if(ClassIndex == 0xFF)
				{
					Type = (BYTE*)VariableObj[VariableIndex].Type2;
					//byOldTag = Type[1];
				}

                                byOldTag = GetSecondVariableObj(OI.w);
				if(byOldTag != NULL && (ClassIndex == 0) )
                                    Type = byOldTag;
                                
				/*if(OI.w == 0x200D || OI.w == 0x200E || OI.w == 0x2047 || OI.w == 0x2046)
				{
					if(ClassIndex == 0)
						Type[1] = 25;
				}
				else if(OI.w == 0x2049 || OI.w == 0x204A || OI.w == 0x2060 || OI.w == 0x2061)
				{
					if(ClassIndex == 0)
						Type[1] = 21;
				}*/
				
			}
#endif
			if( OI.w == 0x2122)
				Len.w = api_GetExtremeStatDataLen(ClassIndex);
			else
				Len.w = GetTypeLenEx( DataType, Type,1); 
			//wxy 20220816 Len.w = GetTypeLenEx( eTagData, Type,1);
		//20220818	Len.w = GetTypeLenEx( DataType, Type,1); 
			//Len.w = GetTypeLen2(DataType, Type);
			//Len.w = GetTypeLen( eTagData, Type);
			break;

		case 6://换算及单位
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( DataType == eData )//源数据长度
			{
				Len.w = 2;
			}
			else
			{
				Len.w = 3;
			}
			//break;
		
		case 7://谐波总功率
			//Type = T_P_ALL;
			
			if( OI.w != 0x2048 )
			{
				//return 0x8000;
			}
			
			if( ClassIndex > Type[1] )
			{
				return 0x8000;
			}

			Type +=GetTagOffsetAddr( ClassIndex, Type );
			Len.w = GetTypeLen(DataType, Type);

			break;
		
		default:		
			return 0x8000;
	}

	return Len.w;
}

