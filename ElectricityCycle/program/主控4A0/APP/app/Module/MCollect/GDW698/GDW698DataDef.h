
#ifndef GDW698_DATA_TYPE_DEF_H_
#define GDW698_DATA_TYPE_DEF_H_
#if(MD_BASEUSER	== MD_GDW698)
#include <stdlib.h>
#define INVALID_LEN		0xFFFF //表示需要特殊计算

//任务类型 不要修改
#define TT_DATA_CJ	1	
#define TT_EVENT_CJ	2	
#define TT_TRANS_CJ	3	
#define TT_REPORT	4

typedef enum TRANS_DIR{ TD12, TD21 } TRANS_DIR; //转换方向: TD12:1形参->2形参; TD21:2->1
	
typedef enum OBJ_DATA_TYPE{ 
	dt_null = 0,
	dt_array,
	dt_structure,
	dt_boolean,
	dt_bit_string,
	dt_double_long,
	dt_double_long_unsigned,
	dt_octet_str = 9,
	dt_visible_string,
	dt_UTF8_string = 12,
	dt_bcd,
	dt_Integer = 15,
	dt_long,
	dt_unsigned,
	dt_long_unsigned,
	dt_long64  = 20,
	dt_long64_unsigned,
	dt_enum,
	dt_float32,
	dt_float64,
	dt_date_time,
	dt_date,
	dt_time,
	dt_datetime_s = 28,

	dt_OI       = 80,
	dt_OAD      = 81,	
	dt_ROAD     = 82,
	dt_OMD      = 83,

	dt_TI = 84,
	dt_TSA = 85,
	dt_MAC  = 86,
	dt_Rand  = 87,
	dt_Region = 88,
	dt_Scaler_Unit = 89,

	dt_RSD = 90,
	dt_CSD = 91,
	dt_MS  = 92,
	dt_ESAM_SID = 93,
	dt_SID_MAC = 94,
	dt_COMDCB  = 95,
	dt_RCSD = 96,

	dt_PIID = 128,				//PIID数据类型
	dt_PIID_ACD,
	dt_FN,
	dt_FN_PIID,
	dt_DAR,
	dt_SPECIFIC = 0xFF,
}OBJ_DATA_TYPE;

//APDU定义
typedef enum APDU_TYPE{
	ATE_NULL = 0,
	LINK_Request = 0x01,		//预连接响应	
	CONNECT_Request,			//建立应用连接请求	
 	RELEASE_Request,			//断开应用连接请求

	GET_Request=0x5,			//读取请求
	SET_Request,				//设置请求	
	ACTION_Request,				//操作请求
	REPORT_Response,			//上报应答
	PROXY_Request,				//代理请求
 	Security_Request=0x10,		//安全认证请求

    LINK_Response = 0x81,		//预连接请求
	CONNECT_Response,			//建立应用连接响应
 	RELEASE_Response,			//断开应用连接响应
  	RELEASE_Notification,		//断开应用连接通知

    GET_Response=133,			//读取响应
    SET_Response,				//设置响应
    ACTION_Response,			//操作响应
    REPORT_Notification,		//上报通知
    PROXY_Response,				//代理响应
    Security_Response=144,		//安全认证响应

	ATE_DATA_FN = 200,
	ATE_UNIT_DATA, ATE_ADDR, ATE_REP,ATE_TP, ATE_TAIL,ATE_TAIL_DOWN//方便程序编程增加，但不是真正的AFN TYPE
} APDU_TYPE;

typedef	WORD	OI;				// 对象标识

// 数据访问结果枚举
typedef enum Data_Access_Result{
	dar_success						= 0,			// 成功
	dar_hardware_fault					= 1,			// 硬件失效
	dar_temporary_failure				= 2,			// 暂时失效
	dar_read_write_denied				= 3,			// 决绝读写
	dar_object_undefined				= 4,			// 对象未定义
	dar_object_class_inconsistent		= 5,			// 对象接口类不符合
	dar_object_unavailable				= 6,			// 对象不存在
	dar_type_unmatched					= 7,			// 类型不匹配
	dar_scope_of_access_violated		= 8,			// 越界
	dar_data_block_unavailable			= 9,			// 数据块不可用
	dar_long_get_aborted				= 10,			// 分帧传输已取消
	dar_no_long_get_in_progress			= 11,			// 不处于分帧传输状态
	dar_long_set_aborted				= 12,			// 块写取消
	dar_no_long_set_in_progress			= 13,			// 不处于块写状态
	dar_invalid_blockno					= 14,			// 数据块序号无效
	dar_password_failure				= 15,			// 密码错/未授权
	dar_speed_noaccess					= 16,			// 通信速率不能更改
	dar_timezone_over					= 17,			// 年时区数超
	dar_timeperiod_over					= 18,			// 日时段数超
	dar_feerate_over					= 19,			// 费率数超
	dar_security_unmatched				= 20,			// 安全认证不匹配
	dar_recharge_repeat					= 21,			// 重复充值
	dar_esam_failure					= 22,			// ESAM验证失败
	dar_security_failure				= 23,			// 安全认证失败
	dar_customeno_inconsistent			= 24,			// 客户编号不匹配
	dar_recharge_num_error				= 25,			// 充值次数错误
	dar_recharge_value_over				= 26,			// 购电超囤积
	dar_address_exception				= 27,			// 地址异常
	dar_symmetric_decryption_failure	= 28,			// 对称解密错误
	dar_asymmetric_decryption_failure	= 29,			// 非对称解密错误
	dar_signature_failure				= 30,			// 签名错误
	dar_meter_suspend					= 31,			// 电能表挂起
	dar_timeflag_invalid				= 32,			// 时间标签无效
	dar_request_timeout					= 33,			// 请求超时
	dar_esam_p1p2_error					= 34,			// ESAM的P1P2不正确
	dar_esam_lc_error					= 35,			// ESAM的LC错误
	dar_compara_failure					= 36,			// 比对失败
	dar_other_reason					= 255			// 其他
}DAR;

typedef	BYTE	octet_string;

#pragma pack(1)
// 日期
typedef struct tag_Date{
	WORD	year;		// 年
	BYTE	month;		// 月
	BYTE	mday;		// 日
	BYTE	wday;		// 周
}Date;

// 时间
typedef struct tag_Time{
	BYTE	hour;		// 时
	BYTE	minute;		// 分
	BYTE	second;		// 秒
}Time;

typedef struct tag_DATETIME{
	WORD	year;		// 年
	BYTE	month;		// 月
	BYTE	mday;		// 日
	BYTE	wday;		// 周
	BYTE	hour;		// 时
	BYTE	minute;		// 分
	BYTE	second;		// 秒
	WORD	msec;		// 毫秒
}DATETIME;

typedef struct tag_DATETIME_S{
	WORD	year;		// 年
	BYTE	month;		// 月
	BYTE	day;		// 日
	BYTE	hour;		// 时
	BYTE	minute;		// 分
	BYTE	second;		// 秒
}DATETIME_S;

// 时间间隔
typedef struct tag_Time_Interval{
	BYTE    unit;		// 间隔单位(enum{0=秒，1=分，2=时，3=日，4=月，5=年})
	WORD    value;		// 间隔值
}TI;

// 时间戳信息
typedef struct tag_TimeTag{
	DATETIME_S	tmSend;
	TI			tmDelay;
}TimeTag;

typedef enum tag_AddrFlag{
	AF_Single    = 0,
	AF_Adapter   = 1,
	AF_Group     = 2,
	AF_Broadcast = 3
}TAddrFlag;

// 对象属性描述符
typedef union Object_Attribute_Descriptor{
	DWORD			value;				// OAD值
	struct
	{
		OI			nObjID;				// 对象标识
		struct
		{
			BYTE	attID  : 5;			// 属性ID
			BYTE	attPro : 3;			// 属性标识（用于事件）
		};
		BYTE		nIndex;				// 属性数据索引
	};
}OAD;

typedef union Object_Method_Descriptor{
	DWORD		value;		// OMD值
	struct	{		
		OI		nObjID;		// 对象标识
		BYTE	nModID;		// 方法ID
		BYTE	nMode;		// 操作模式
	};
}OMD;

#define OBJ_MAX_OADS		(32)		// 记录列选择最大个数（根据实际扩展）

typedef struct Cognate_Object_Attribute_Descriptor{
	BYTE	nNum;						// 关联对象属性个数
	OAD		oad[OBJ_MAX_OADS];			// 关联对象属性列表
}PACK COAD, OADS;

// 记录型对象属性描述符
typedef struct Record_Object_Attribute_Descriptor{
	OAD		oadMain;		// 主OAD
	COAD	oadCols;		// 关联对象属性
}ROAD;

typedef struct Column_Selection_Descriptor{
	BYTE		choice;		// 类型标识
	union
	{
		OAD		oad;		// 单OAD
		ROAD	road;		// 记录型OAD
	};
}CSD;

typedef struct tag_ComDCB{
	BYTE	baud;
	BYTE	parity;
	BYTE	databits;
	BYTE	stopbits;
	BYTE	flowctrl;
}ComDCB;

typedef struct tag_Variant{
	BYTE				type;
	union	{
		char			cVal;
		BOOL			bVal;
		BYTE			byVal;
		BYTE            enumVal;
		DATETIME		dtVal;
		Date			dVal;
		Time			tVal;
		DATETIME_S		tmVal;
		long			nVal;
		WORD			wVal;
		short			sVal;
		DWORD			dwVal;
		DLONG			llVal;
		UDLONG			ullVal;
		float			fVal;
		double			dbVal;
		OI				oi;
		TI				ti;
		TSA				tsa;
		OAD				oad;
		OMD				omd;
		CSD				csd;
		ComDCB			dcb;
		DAR				darr;
	};
}Variant;

typedef struct tag_Selector0{
	BYTE	nullv;
}Selector0;

typedef struct tag_Selector1{
	OAD		oad;
	Variant	value;
}Selector1;

typedef struct tag_Selector2{
	OAD		oad;
	Variant	from;
	Variant	to;
	Variant span;
}Selector2;

#define MAX_SELECTOR2_NUM	(4)

typedef struct tag_Selector3{
	WORD		nNum;
	Selector2	item[MAX_SELECTOR2_NUM];
}Selector3;

// 电表集合
#define OBJ_MAX_MSGROUP		(32)	// 电表集合数
#define OBJ_MAX_MSNO		(192)	// 电表用户配置序号数(暂订II型集中器最大测量点数)

// 一组用户类型
typedef struct Meter_Type{
	WORD size;
	BYTE type[OBJ_MAX_MSGROUP];
}MSTYPE;

// 一组用户地址
typedef struct Meter_Addr{
	WORD size;
	TSA  add[OBJ_MAX_MSGROUP];
}MSADDR;

// 一组用户配置序号
typedef struct Meter_No{
	WORD size;
	WORD idx[OBJ_MAX_MSNO];
}MSNO;

// 用户类型区间
typedef struct Meter_Region_Type{
	BYTE	nType;
	BYTE	start;
	BYTE	end;
}MRTYPE;

// 用户地址区间
typedef struct Meter_Region_TSA{
	BYTE	nType;
	TSA		start;
	TSA		end;
}MRTSA;

// 用户配置序号区间
typedef struct Meter_Region_Index{
	BYTE	nType;
	WORD	start;
	WORD	end;
}MRIDX;

// 一组用户类型区间
typedef struct Meter_Set_Region_Type{
	WORD		size;
	MRTYPE		region[OBJ_MAX_MSGROUP];
}MSRT;

// 一组用户地址区间
typedef struct Meter_Set_Region_TSA{
	WORD		size;
	MRTSA		region[OBJ_MAX_MSGROUP];
}MSRA;

// 一组用户配置序号区间
typedef struct Meter_Set_Region_Index{
	WORD	size;
	MRIDX	region[OBJ_MAX_MSGROUP];
}MSRI;

typedef enum tag_MS_CHOICE{
	MS_NULL			= 0,
	MS_ALL			= 1,
	MS_TYPE			= 2,
	MS_TSA			= 3,
	MS_INDEX		= 4,
	MS_REGION_TYPE	= 5,
	MS_REGION_TSA	= 6,
	MS_REGION_IDX	= 7
}MS_CHOICE;

typedef struct Meter_Set{
	BYTE		choice;		// choice = 0（无电能表）
							// choice = 1（所有电能表）
	union
	{
		WORD	size;
		MSTYPE		mst;		// choice = 2（一组用户类型）
		MSADDR		msa;		// choice = 3（一组用户地址）
		MSNO		msi;		// choice = 4（一组用户配置序号）
		MSRT	msrt;			// choice = 5（一组用户类型区间）
		MSRA	msra;			// choice = 6（一组用户地址区间）
		MSRI	msri;			// choice = 7（一组用户序号区间）
	};
}MS;

typedef struct tag_Selector4{
	DATETIME_S	time;
	MS			ms;
}Selector4;

typedef struct tag_Selector5{
	DATETIME_S	time;
	MS			ms;
}Selector5;

typedef struct tag_Selector6{
	DATETIME_S	start;
	DATETIME_S	end;
	TI			ti;
	MS			ms;
}Selector6;

typedef struct tag_Selector7{
	DATETIME_S	start;
	DATETIME_S	end;
	TI			ti;
	MS			ms;
}Selector7;

typedef struct tag_Selector8{
	DATETIME_S	start;
	DATETIME_S	end;
	TI			ti;
	MS			ms;
}Selector8;

typedef struct tag_Selector9{
	BYTE	nLast;
}Selector9;

typedef struct tag_Selector10{
	BYTE	nTimes;
	MS		ms;
}Selector10;

typedef struct tag_RSD{
	BYTE			choice;
	union{
		Selector0	sel0;
		Selector1	sel1;
		Selector2	sel2;
		Selector3	sel3;
		Selector4	sel4;
		Selector5	sel5;
		Selector6	sel6;
		Selector7	sel7;
		Selector8	sel8;
		Selector9	sel9;
		Selector10	sel10;
	};
}RSD;	//sizeof(RSD) = 1639

//终端地址
typedef struct _TOad40010200{
	TSA		addr;
}TOad40010200;

//组地址
typedef struct _TOad40050200{
	BYTE	grp_num;
	TSA		addr_grp[8];
}TOad40050200;

//终端电压逆相序配置参数
typedef struct _TOad300F0600{
	BYTE	delay;
}TOad300F0600;

//电能表时钟超差配置参数
typedef struct _TOad31050600{
	WORD	threshold;			//门限值
	BYTE	task_no;			//关联任务号
}TOad31050600;

//终端停/上电事件配置参数 1
typedef struct _TOad31060601{
	BYTE byFlagCJ;				//停电采集标志 D0:1有效,D0:0无效，D1:0 指定测量点 D1:1随机选择
	BYTE byCJGap;				//采集间隔  单位:小时
	BYTE byCJTmLmt;				//停电事件抄读时间限值  单位: min
	BYTE byTSANum;				//地址个数	
	TSA	 mp[6];					//地址列表
}TOad31060601;

//终端停/上电事件配置参数 2
typedef struct _TOad31060602{
	WORD wTDTMMinGap;			//停电时间最小有效间隔  单位：分钟
    WORD wTDTMMaxGap;			//最大有效间隔  单位：分钟
    WORD wTMBELmt;				//起止时间偏差限值 单位：分钟
    WORD wTmMinLmt;				//时间区段限值   单位：分钟
	WORD VOnLmt_1;
	WORD VOffLmt_1;				//单位: V 1位小数点
}TOad31060602;

typedef struct _TOad31060600{
	TOad31060601 oad1;
	TOad31060602 oad2;
}TOad31060600;
//电能表示度下降配置参数
typedef struct _TOad310B0600{
	BYTE	task_no;			//关联任务号
}TOad310B0600;

//电能量超差事件配置参数
typedef struct _TOad310C0600{
	DWORD	threshold;			//门限值
	BYTE	task_no;			//关联任务号
}TOad310C0600;
//电能量飞走事件配置参数
typedef TOad310C0600 TOad310D0600;
//电能量停走事件配置参数
typedef struct _TOad310E0600{
	TI		ti;			//门限值
	BYTE	task_no;			//关联任务号
}TOad310E0600;
//终端抄表失败事件配置参数
typedef struct _TOad310F0600{
	BYTE	byTryNum;			//重试轮次
	BYTE	task_no;			//关联任务号
}TOad310F0600;
//月通信流量超限事件配置参数
typedef struct _TOad31100600{
	DWORD	dwGPRSFlow;			//通信流量门限
}TOad31100600;
//电能表数据变更监控记录事件配置参数
typedef TOad310B0600 TOad311C0600;

#define MAX_EVENT_PARA3  6
typedef struct _TOadEventClass7{
	BYTE	byOADNum;			//终端状态量变位事件
	OAD		oadEvent[MAX_EVENT_PARA3];
}TOadEventClass7;

typedef TOadEventClass7 TOad31010300; //终端版本变更事件

typedef TOadEventClass7 TOad31040300; //终端状态量变位事件

typedef TOadEventClass7 TOad31090300; //消息认证错误记录

typedef TOadEventClass7 TOad310B0300; //电能表示度下降

typedef TOad310B0300 TOad310C0300; //电能量超差

typedef TOad310B0300 TOad310D0300; //电能表飞走

typedef TOad310B0300 TOad310E0300; //电能表停走

typedef TOadEventClass7 TOad310F0300; //抄表失败

typedef TOadEventClass7 TOad31100300; //终端与主站通信流量超门限

typedef TOadEventClass7 TOad31140300; //对时事件记录

typedef TOadEventClass7 TOad31190300; //终端电流回路异常事件

typedef TOadEventClass7 TOad32000300; //功控跳闸记录

typedef TOadEventClass7 TOad32010300; //电控跳闸记录

typedef TOadEventClass7 TOad32020300; //购电参数设置记录

#define MAX_EVENT_PARA24 40

typedef struct _TOadEventClass24{
	BYTE	byOADNum;			//终端状态量变位事件
	OAD		oadEvent[MAX_EVENT_PARA24];
}TOadEventClass24;

typedef TOadEventClass24 TOad30000200; //电能表失压事件

typedef TOadEventClass24 TOad30010200; //电能表欠压事件

typedef TOadEventClass24 TOad30020200; //电能表过压事件

typedef TOadEventClass24 TOad30030200; //电能表断相事件

typedef TOadEventClass24 TOad30040200; //电能表失流事件

typedef TOadEventClass24 TOad30050200; //电能表过流事件

typedef TOadEventClass24 TOad30060200; //电能表断流事件

//校时模式
typedef struct _TOad40000300{	
	BYTE byMode;				//0-主站授时，1-终端精确校时
}TOad40000300; 

//精准校时参数
typedef struct _TOad40000400{
	BYTE byHeartNum;			//最近心跳时间总个数   
	BYTE byMaxDelNum;			//最大值剔除个数       
	BYTE byMinDelNum;			//最小值剔除个数       
	BYTE byDelayTime;			//通讯延时阈值          
	BYTE byValidNum;			//最少有效个数         
}TOad40000400; 
//地理位置
typedef struct _position{
	BYTE byFangWei;				//方位
	BYTE byDu;					//度  
	BYTE byFen;					//分 
	BYTE bySec;					//秒
}position;

typedef struct _TOad40040200{
	position jing;				//经度  
	position wei;				//纬度   
	DWORD high;					//高度 cm
}TOad40040200;

//LCD参数
typedef struct _TOad40070200{
	BYTE byLightTime;			//上电全显时间
	WORD wBlackTime;			//背光点亮时间
	WORD wShowTime;				//显示查看背光点亮时间
	WORD wKeyTime;				//无电按键屏幕驻留最大时间
	BYTE byEnergyNum;			//显示电能小数位数
	BYTE byPowerNum;			//显示功率(最大需量)
	BYTE byLcdType;				//液晶①②字样意义
}TOad40070200;

//电压合格率参数
typedef struct _TOad40300200{
	WORD wVolLimitUp;			//电压考核上限 
	WORD wVolLimitDown;			//电压考核下限 
	WORD wVolUp;				//电压合格上限 
	WORD wVolDown;				//电压合格下限 
}TOad40300200;

typedef struct _TOad42020200{
	BOOL bValid;				//级联标志            bool，
	DWORD dwOAD;				//级联通信端口号      OAD，
	WORD wTimeOut;				//总等待超时（10ms）  long-unsigned，
	WORD wCharTimeOut;			//字节超时（10ms）    long-unsigned，		
	BYTE byRetryNum;			//重发次数 	
	BYTE interval_cjMins;		//巡测周期（min）     unsigned，
	BYTE byJLPortNum;			//级联（被）端口数    unsigned，
	BYTE bysaNum;	
	TSA	sa[4];					//级联（被）终端地址  array TSA
}TOad42020200;

//终端广播校时
typedef struct _TOad42040200{
	Time  tTime;
	BOOL  bUse;		  			//是否启用
}TOad42040200;		
typedef struct _TOad42040300{
	char maxErrTime;			//时钟误差阈值
	Time  tTime;	
	BOOL  bUse;					//是否启用
}TOad42040300;		

//采集器升级控制参数
/*
typedef struct _TOad42050300{
	BYTE byBroadNum;			//允许一次升级广播轮次数      
    BYTE byDays;				//允许一次升级点对点补发天数  
}TOad42050300;
*/
//无线公网通信接口通信配置
typedef struct _TOad45000200{
	BYTE byWorkMode;			//工作模式  enum{混合模式（0），客户机模式（1），服务器模式（2）}，
	BYTE byVMode;				//在线方式  enum{永久在线（0），被动激活（1）}，
	BYTE byTCPUDPMode;			//连接方式  enum{TCP（0），UDP（1）}，
	BYTE byLinkMode;			//连接应用方式enum{主备模式（0），多连接模式（1）}，
	BYTE byListenPortNum;
	WORD wListenPort[3];    	//侦听端口列表array long-unsigned，
	char szAPN[33];				//visible-string，
	char szUser[33];			//用户名    visible-string，
	char szPass[33];			//密码      visible-string，
	BYTE byProxyServerIPLen;
	DWORD dwProxyServerIP;		//代理服务器地址  octet-string，
	WORD wProxyProt; 	    	//代理端口  long-unsigned，
	BYTE byRetryTime;	    	//超时时间（秒），超时时间及bit1~bit0：重发次数
	WORD wSecs_Tick;	        //心跳周期(秒)  long-unsigned
}TOad45000200;
//无线公网通信接口主站通信参数表
typedef struct _TStaionConfig{
	BYTE byIPServerLen;
	DWORD IPServer;		    	//IP 地址  octet-string，
	WORD  wPort;	        	//端口  octet-string，
}TStaionConfig;

typedef struct _TOad45000300{
	BYTE byNum;
	TStaionConfig stationConfig[2];
}TOad45000300;
//无线公网通信接口短信通信参数
typedef struct _TOad45000400{
	char szSMS[17];	 			//短信中心  visible-string，
	BYTE byPhoneNum;
	char szPhone[3][17];		//主站号码   visible-string，
	BYTE byAlmPhoneNum;
	char szAlmPhone[3][17];		//短信通知目的码      visible-string，
}TOad45000400;	
//无线公网通信接口版本信息
typedef struct _TOad45000500{
	char  productCode[4];		//厂商代码       visible-string(SIZE (4))，
	char  softVer[4];			//软件版本号     visible-string(SIZE (4))，
	char  softDate[6];			//软件版本日期   visible-string(SIZE (6))，
	char  hardVer[4];			//硬件版本号     visible-string(SIZE (4))，
	char  hardDate[6];			//硬件版本日期   visible-string(SIZE (6))，
	char  productorInfo[8];		//厂家扩展信息   visible-string(SIZE (8))
}TOad45000500;

//以太网通信通信配置
typedef struct _TOad45100200{
	BYTE byWorkMode;			//工作模式  enum{混合模式（0），客户机模式（1），服务器模式（2）}，
	BYTE byTCPUDPMode;			//连接方式  enum{TCP（0），UDP（1）}，
	BYTE byLinkMode;			//连接应用方式enum{主备模式（0），多连接模式（1）}，
	BYTE byListenPortNum;
	WORD wListenPort[3];    	//侦听端口列表array long-unsigned，
	BYTE byProxyServerIPLen;
	DWORD dwProxyServerIP;		//代理服务器地址  octet-string，
	WORD wProxyProt;			//代理端口  long-unsigned，
	BYTE byRetryTime;	    	//超时时间（秒），超时时间及bit1~bit0：重发次数
	WORD wSecs_Tick;			//心跳周期(秒)  long-unsigned
}TOad45100200;

//以太网通信主站通信参数表
typedef struct _TOad45100300{
	BYTE byNum;
	TStaionConfig stationConfig[2];
}TOad45100300;
//以太网通信终端IP
typedef struct _TOad45100400{
	BYTE byIPMode;				//IP配置方式enum{DHCP（0）,静态（1），PPPoE（2）}，
	BYTE byNetLocalIPLen;
	DWORD dwNetLocalIP;			//IP地址       octet-string，
	BYTE byNetMaskLen;
	DWORD dwNetMask;			//子网掩码     octet-string，
	BYTE byNetGateLen;
	DWORD dwNetGate;			//网关地址     octet-string，
	char PPoEUser[33];			//PPPoE用户名  octet-string,
	char PPoEPass[33];			//PPPoE密码    octet-string
}TOad45100400;

//以太网通信MAC地址
typedef struct _TOad45100500{
	char strMac[7];
}TOad45100500;

//安全模式选择
typedef struct _TOadF1010200{
	BYTE bySafeUser;
}TOadF1010200;

typedef struct  _TSafeMode{
	 OI oi;						//对象标识   OI，
	 WORD wMode;				//安全模式   long-unsigned
}TSafeMode;
#define SAFE_MODE_NUM 30
//显式安全模式参数
typedef struct _TOadF1010300{
	BYTE byModeNum;
	TSafeMode safeMode[SAFE_MODE_NUM];
}TOadF1010300;

typedef struct _TOadF2030400{
	BYTE byFlag;				//接入标志		
	BYTE byAttrib;				//属性标志
}TOadF2030400;

typedef struct _TOadF2010200_2{
	BYTE byPortNo;	
	char attrStr[16];			//属性描述符
	BYTE attrStr_len;			//属性长度
	ComDCB com_dcb;				//端口参数
	BYTE type_fun;				//功能类型
}TOadF2010200_2;

typedef struct _TOadF2010200{
	BYTE byPortNum;	
	TOadF2010200_2 rs485[3];
}TOadF2010200;

typedef struct _TOadF2050200{
	BYTE relay_type[2];			//继电器类型
}TOadF2050200;

typedef struct _TOadF2060400_2{
	Time begin;					//开始时间
	Time end;					//结束时间
}TOadF2060400_2;

typedef struct _TOadF2060400{
	BYTE alm_tm_num;			//告警时段
	TOadF2060400_2 alm_tm[24]; 
}TOadF2060400;

typedef struct _TChinaInfo{
	DWORD	dwTime;
	BYTE	byType;
	BYTE	byBNo;
	BYTE	bRead;
	BYTE	byValid;
	char	Info[201];
}TChinaInfo;

#define OBJ_PWD_LEN			(8)				// 密码域长度
#define OBJ_MAX_ANNEX_LEN	(10)			// 最大附加信息域长度
#define OBJ_MAX_ANNEX_NUM	(3)				// 最大附加信息域数量
typedef struct tag_Meter_Basic{
	TSA			tsa;						// 表计地址
	BYTE		baud;						// 波特率
	BYTE		protocol;					// 规约类型
	OAD			port;						// 端口
	char		password[OBJ_PWD_LEN];		// 通信密码
	BYTE		pwdLen;						// 密码长度
	BYTE		rateNum;					// 费率个数
	BYTE		userType;					// 用户类型
	BYTE		conType;					// 接线方式
	WORD		stVolt;						// 额定电压
	WORD		stI;						// 额定电流
}Meter_Basic;

typedef struct tag_Meter_Extended{
	TSA		         add;			    // 采集器地址
	BYTE			 astLen;
	BYTE			 assetNo[32];		// 资产号
	WORD	         PT;				// PT
	WORD	         CT;				// CT
}Meter_Extended;

typedef struct tag_AnnexObject{
	OAD 		oad;
	BYTE		nLen;
	BYTE		value[OBJ_MAX_ANNEX_LEN];
}AnnexObject;

typedef struct tag_Meter_Annex{
	BYTE			nNum;
	AnnexObject		annexObj[OBJ_MAX_ANNEX_NUM];
}Meter_Annex;

typedef struct TOad60010200{	//sizeof(TOad60010200) = 149
	WORD			nIndex;		// 配置序号 2
	Meter_Basic		basic;		// 基本信息 32
	Meter_Extended	extend;		// 扩展信息 23
	Meter_Annex		annex;		// 附属信息 63 14个
	BYTE MeterNo[8];			// 电表表号(固定8字节,ESAM加密)
}TOad60010200;

typedef struct tag_OBJM_METER_129{
	WORD		nIndex;
	Meter_Basic	basic;
}OBJM_METER_129;

typedef struct tag_OBJM_METER_130{
	WORD			nIndex;
	Meter_Extended	ext;
	Meter_Annex		annex;
}OBJM_METER_130;

typedef struct tag_OBJM_METER_133{
	TSA		add;
	OAD		port;
}OBJM_METER_133;


typedef struct _TOad60020800{
	BOOL bDaySearchUse;			//是否启用每天周期搜表   
	BOOL bUpdateAD;				//自动更新采集档案       
	BOOL bsearchEvent;			//是否产生搜表相关事件   
	BYTE byResult;				//清空搜表结果选项       
}TOad60020800;

typedef struct _TOad60020900{
	Time tStartTime;	 		//开始时间
	WORD wSearchMin;	 		//搜表时长      
}TOad60020900;

typedef struct TaskSegTime{
	BYTE byBeginHour;			//时
	BYTE byBeginMin;			//分
	BYTE byEndHour;				//时
	BYTE byEndMin;				//分
}TaskSegTime;

//任务配置单元
//注意：调整结构时重分考虑TTask698ParaCfg结构中有关内存
typedef struct _TOad60130200{
	WORD	validflag;			//0x5aa5为任务有效
	BYTE	byTaskID;			//任务ID    unsigned
	TI		intelTI;			//执行频率  TI，
	BYTE	byTaskType;			//方案类型  enum 普通采集方案1,事件采集方案2
								//透明方案 3 上报方案 4 脚本方案5 实时监控方案6
	BYTE	byShemeNo;			//方案编号 unsigned，
	DWORD	dwBeginTime;		//开始时间    ObjDateTime_s ，
	DWORD	dwEndTime;	    	//结束时间    ObjDateTime_s ，
	TI		dealyTI;			//延时        TI ，
	BYTE	byPrior;			//执行优先级  enum{首要（1），必要（2），需要（3），可能（4）}，
	BYTE	byStuts;			//状态        enum{正常（1），停用（2）}，
	WORD    wBeginId;			//任务开始前脚本id   long-unsigned，
	WORD    wEndId;		    	//任务完成后脚本id   long-unsigned，
	BYTE	byRunType;			//任务运行时段类型
	BYTE	bySegNum;			//任务运行时段数
	TaskSegTime segTime[MAX_TASK_TIMEREG_NUM];	
}TOad60130200;	

typedef union _UCjInfo{
	BYTE	_N;
	TI		tii;
	struct {
		TI tii;
		WORD lastNums;
	}retryMeter;
}UCjInfo;
//普通采集方案
typedef struct _TOad60150200{	//sizeof(TOad60150200) = 1652
	WORD	valid_flag;			//0x5aA5
	BYTE	byShemeNo;			//方案编号 unsigned，
	WORD	wSaveDeep;			//存储深度
	BYTE    byCJType;			//0 当前数据 1采集上n次数据 2按冻结时标采集 3按时标间隔采集
	UCjInfo CJContent;			//采集内容
	BYTE	byColNum;			//采集列数目
	BYTE	csdBuf[512];		//保存设置
	MS		objMS;				//电能表集合
	BYTE	bySaveFlag;			//存储时标选择 未定义0，任务开始时间1，相对当日0点0分2，
								//相对上日23点59分3，相对上日0点0分4，相对当月1日0点0分 5
	DWORD   dwValidTime;		//任务有效时间
}TOad60150200;	
	
//事件采集方案	
typedef struct _TOad60170200{	//sizeof(TOad60170200) = 1634
	WORD	valid_flag;			//0x5aA5
	BYTE	byEventNo;			//方案编号unsigned，		
	BYTE    byCJType;			//采集类型
	BOOL    bReport;			//上报标识 
	WORD	wSaveDeep;			//存储深度 
	//.ROAD	objRoad[20];		//采集的事件数据
	BYTE	byCJNum;
	BYTE	EventROADS[500];	//事件 ROAD	
	MS		objMS;				//电能表集合
}TOad60170200;	

//上报方案
typedef struct _TOad601D0200{	//sizeof(TOad601D0200) = 512
	WORD	valid_flag;			//0x5aA5
	BYTE	byReportNo;	 	 	//方案编号
	BYTE	port_num;			//上报通道个数
	OAD		oadReport[3];      	//上报通道
	TI		reportTI;        	//上报响应超时时间
	BYTE    byReportNum;	 	//最大上报次数
	BYTE	byReportType;	 	//上报内容
	BYTE	Report[500];		//上报内容  0 对象属性数据 1 上报记录型对象属性
}TOad601D0200;	
	
//透明方案
typedef struct _TTransFrame{
	BYTE    byFrameNo;  //报文序号
	WORD    wFrameLen;  //报文长度
	WORD    wFmOffset;  //报文在缓冲区的起始偏移
}TTransFrame;

typedef struct _TTransContent{
	BYTE    use_flag;    
	WORD    contentNo;   //方案内容序号
    TSA     tsa;         //通信地址
	WORD    bId;         //开始前脚本id
	WORD    eId;         //完成后脚本id
	BOOL    bRptAndWait; //控制标志-上报透明方案结果并等待后续报文
	WORD    waitTime;    //控制标志-等待后续报文超时时间
    BYTE    cmp_flag;     //控制标志-结果比对标识
	BYTE    cmp_Byte;    //控制标志-结果比对-特征字节
	WORD    cmp_startpos;//控制标志-结果比对-截取开始
	WORD    cmp_lenth;   //控制标志-结果比对-截取长度
    BYTE    byFrameNum;  //报文个数
	TTransFrame tranFrames[6]; //方案报文
	BYTE    FmBuffer[200];     //报文内容缓冲区
}TTransContent;

typedef struct _TOad60190200{
	WORD	valid_flag;			//0x5aA5
	BYTE	byTransNo;			//方案编号unsigned，		
	BYTE	byContentNum;       //方案内容集个数
	TTransContent Contents[5];  //方案内容集（最大支持5个）
	WORD    wSaveDeep;          //存储深度
}TOad60190200;

typedef struct _TSysCtrlPara{
	//23：30-24：00	23：00-23：30	22：30-23：00	22：00-22：30	第12字节
	//每半小时以两位编码表示4种控制状态：取值0…3依次表示不控制、控制1、控制2、保留。
	//控制状态标识的应用规则：连续时间单元具有相同控制状态标识表示同一控制时段；连续时间单元具有不同控制状态标识，表示相邻的两个时段，控制1与控制2用以区分具备2个不同的定值的连续时段，当控制状态标识发生变化时，表示前一控制时段结束，后一控制时段开始，对于不连续的控制时段可以用控制1或控制2表示。
	BYTE	PKTM[12];			//功控时段
	long	nPower_Safe;		//保安定值
	BYTE	Mins_Lun[2];		//最多2轮
}TSysCtrlPara;
//总加组参数
typedef struct _TOad81090200{
	BYTE	valid_flag;			//方案有效标识
	char	pkcoft;				//功控系数
	BYTE	tm_flag[3];			//时段号
	long	fixval[3][8];		//时段定值
}TOad81090200;

//厂休
typedef struct _TOad810A0200{
	TTime   stXDTime;
	WORD	wMins_XD;			//限电时间
	BYTE	byFlag_FRXD;		//厂休限电标志
	long	nFixVal;			//厂休定值
}TOad810A0200;

typedef struct _TOad810B0200{
	TTime	tBTSTime;
	TTime	tBTETime;	
	long	nFixVal;			//报停定值
}TOad810B0200;


typedef struct _TGRPCtrlPara{
	BYTE	byTmPKSchNo;		//时段功控方案号
	BYTE	byFlag_TMPK;		//时段功控投入标识
	BYTE	byFlag_PK;			//功控投入标志
	BYTE	byFlag_EK;			//电控投入标志
	
	long	nPower_PDPKFix;		//下浮控定值
	BYTE	byMin_PDPKSlide;	//下浮控滑差时间
	BYTE	byCoft_PDPK;		//下浮控浮动系数
	BYTE	byMin_PDDelay;		//下浮控后延时
	BYTE	byCoftDir_PDPK;		//下浮控系数符号
	WORD	wMins_PDXD;			//临时限电时间
	BYTE	Min_PDAlm[4];		//下浮控轮次告警时间,华北

	DWORD	dwPDOnTime;			//下浮控投入的起始时间
	long	nPower_PDFrz;		//下浮控控后冻结值
	BYTE	byEKType;			//0-电量控，1-电费控
	BYTE byHCTime;				//功率控制的功率计算滑差时间（范围：1~60）BIN min 1
	BYTE byPKLCFlag;			//功控轮次标志位 BS8 1
	BYTE byDKLCFlag;			//电控轮次标志位 BS8 1
	BYTE unit_valid;			//单元有效标识	TOad81090200。。。
}PACK TGRPCtrlPara;

typedef struct _GB698_GRP_CFG{
	TGRPCtrlPara  tGRPPara;
	TOad81090200  para8109;		//时段功控参数
	TOad810A0200  para810A;		//厂休控参数	
	TOad810B0200  para810B;		//报停控参数
//	TOad810C0200  para810C;		//购电控参数
//	TOad810D0200  para810D;		//月电量控参数
}PACK GB698_GRP_CFG;

typedef struct _TEsamSafeMode{
	OI oii;
	WORD wSafeMode;
}PACK TEsamSafeMode;

#define ESAM_SAFE_NUM	20//不能超过127，否则类的读写会有问题
typedef struct _TsysCfgOther{	//总共300个字节 
	WORD wFlag_Ctrl;
	DWORD dwMins_BD;			//此标志最低字节用于II型集中器实时转日冻结参数 0x5A-实时转日冻结 其他-电表冻结
	DWORD dwSecs_BDStart;
	BYTE  byBDTmNum;			//保电时段个数
	WORD  BDTm[24];				//高字节开始、低字节结束

	DWORD dwLcdPass;
	BYTE  link_authentication;	//连接认证模式(公共连接   (0)，  普通密码   (1)，  对称加密   (2)，  数字签名   (3))

	DWORD dwDisplayCtrl;		//巡显配置	
	BYTE  byDisplayInterval;	//巡显间隔
	BYTE byNetType;				//0x55 有线宽带，其他-以太网	

	BYTE  CFHourFlag[3];		//催费小时标识
	char  Info[139];			//催费信息
	BYTE	szSIMCard[17];
	BYTE	szPassWord[32];		//连接密码
	BYTE	byUpPortNum;		//上报通道个数
	DWORD	UpPortOAD[3];		//上报通道	
	WORD max_no_data_mins;		//允许与主站最大无通信时长（分钟），0表示不自动保电。
	WORD max_power_delay_mins;	//上电自动保电时长（分钟），0表示上电不自动保电。
	BYTE safeMode_ctrl;			//BIT0-是否启用安全模式,BIT1-是否启用SAL安全应用数据链路层参数
	BYTE valid_safemode[(ESAM_SAFE_NUM+7)/8];	
	TEsamSafeMode safeMode[ESAM_SAFE_NUM];
	BYTE EventFlag[88];			//BIT0~BIT1-上报标识，BIT2-有效标识
	TSysCtrlPara sysCtrl;
	DWORD threshold_Relay;
	WORD mins_OverI;
	BYTE byGNetMode; //G网选择(BIT1~0, 0:自由选择 1:2G 2:3G 3:4G )BIT2: 1 保活机制打开,0 关闭, BIT3:鉴权类型
}PACK TsysCfgOther;

typedef struct _TPkTM{
	BYTE	byTmNum;			//时段个数
	WORD	Mins_TMEnd[19];		//时段(相对于0点的结束时间)
	BYTE	Type_TM[19];		//0-不控制 1-控制 2-控制2
	TTime	TimeStart;			//时段执行开始时间
	TTime	TimeEnd;			//时段执行结束时间
	BYTE	byFreq;				//执行频率
	DWORD	dwFlag;				//标志omm
}TPkTM;

typedef struct _TPulseCfg{
	WORD wConst;
	BYTE byPulseNo;				//脉冲输入端口号
	BYTE attrib;
}TPulseCfg;

typedef struct _TOad24010200{	//脉冲测量点配置
	//.BYTE Addr[17];		
	TSA sa;
	BYTE byPulseNum;	
	WORD wPT;
	WORD wCT;
	TPulseCfg Cfg[4];
}TOad24010200;

typedef struct _TFrz698Cfg{
	WORD	interval;			//冻结周期
	DWORD	oadd;				//关联oadd
	WORD	wSaveDeep;			//存贮深度
}TFrz698Cfg;

#define MAX_FRZ_CFG_NUM	50 //60 		
typedef struct _TOad50020300{	//分钟冻结配置	
	BYTE byNum;
	TFrz698Cfg FrzCfg[MAX_FRZ_CFG_NUM];	
}TOad50020300;

typedef TOad50020300 TOad50030300;
typedef TOad50020300 TOad50040300;
typedef TOad50030300 TOad50060300;

typedef struct _TOad41030200{		//资产管理编码	
	BYTE byNum;
	BYTE asetNo[32];	
}TOad41030200;

#pragma pack()

#endif //#if(MD_OBJECT==YES)
#endif //_NXCJ_DATA_TYPE_DEF_H_

