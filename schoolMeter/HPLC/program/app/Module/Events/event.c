//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.9.28
//描述		事件模块源文件
//修改记录.读取计量序号从0开始，表内存储第一条序号为1	
//---------------------------------------------------------------------- 

#include "AllHead.h"

// 若有事件记录
// #if( MAX_EVENT_NUM != 0 )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#if(PREPAY_MODE == PREPAY_LOCAL)
#define EVENT_COLUMN_OAD_MAX_NUM			9					// 固定列OAD最多个数,异常插卡是9个固定列
#else
#define EVENT_COLUMN_OAD_MAX_NUM			7					// 固定列OAD最多个数
#endif
#define	EVENT_Q_COMBO_OI					0x3023				// 无功组合方式编程OI
#define	EVENT_PACTIVE_DEMANDOVER_OI			0x3009				// 正向有功需量超限
#define	EVENT_NACTIVE_DEMANDOVER_OI			0x300A				// 反向有功需量超限
#define	EVENT_REACTIVE_DEMANDOVER_OI		0x300B				// 无功需量超限
#define EVENT_AHOUR_OI						0X2029				// 安时值
#define EVENT_I_UNBALANCE_OI				0X301E				// 电流不平衡
#define EVENT_I_S_UNBALANCE_OI				0X302D				// 电流严重不平衡
#define EVENT_ABR_CARD_OI					0x3027				//电能表异常插卡事件
// 相对规约反序
#define STARTTIME_OAD					0x00021E20			// 事件开始时间对应OAD
#define ENDTIME_OAD						0x00022020			// 事件结束时间对应OAD
#define RECORDNO_OAD					0x00022220			// 事件记录序号对应OAD
#define GEN_SOURCE_OAD					0x00022420			// 事件发生源对应OAD
#define NOTIFICATION_OAD				0x00020033			// 事件上报状态对应OAD
#define	MAXDEMAND_OAD					0X06020833			// 超限期间需量最大值OAD
#define	MAXDEMAND_TIME_OAD				0X07020833			// 超限期间需量最大值发生时间OAD
#define PRG_OBJ_LIST_OAD				0X06020233			// 编程对象列表
#define CLR_EVENT_LIST_OAD				0X06020c33			// 事件清零列表
#define AHOUR_OAD						0X00022920			// 安时值OAD
#define V_UNBALANCE_OAD					0X00022620			// 电压不平衡OAD
#define I_UNBALANCE_OAD					0X00022720			// 电流不平衡OAD
#define PRG_SEGTABLE_OAD				0X06021433			// 时段表编程OAD
#define PRG_SEGTABLE_VALUE				0X07021433			// 时段表编程前时段表数据
#define PRG_HOLIDAY_OAD					0X06021533			// 节假日编程OAD
#define PRG_HOLIDAY_VALUE				0X07021533			// 节假日编程前节假日数据
#define CARD_SN_OAD						0X06021033			//异常插卡事件记录单元0x3310--卡序列号	octet-string，
#define CARD_ERR_INFO_OAD				0X07021033			//异常插卡事件记录单元0x3310--插卡错误信息字	unsigned，
#define CARD_ORDER_HEAD_OAD				0X08021033			//异常插卡事件记录单元0x3310--插卡操作命令头	octet-string，
#define CARD_ERR_RES_OAD				0X09021033			//异常插卡事件记录单元0x3310--插卡错误响应状态  long-unsigned，
#define RETURN_MONEY_OAD				0X06021133			//退费事件记录单元0x3311--退费金额      double-long-unsigned（单位：元，换算：-2），

#if( SEL_DLT645_2007 == YES )
static const WORD SignEventOI_Table[] = 
{
	0x2001,//电流
	0x2004,//有功功率
	0x2005,//无功功率
	0x2006,//视在功率
	0x200A,//功率因数
	0x0000,//组合有电能
	0x0030,//总组合无功1电能
	0x0031,//A相组合无功1电能
	0x0032,//B相组合无功1电能
	0x0033,//C相组合无功1电能
	0x0040,//总组合无功2电能
	0x0041,//A相组合无功2电能
	0x0042,//B相组合无功2电能
	0x0043,//C相组合无功2电能
};
#endif

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct TEventData_t
{
	TDLT698RecordPara 		*pDLT698RecordPara;
	TEventOADCommonData		*pEventOADCommonData;
	TEventAttInfo			*pEventAttInfo;
	TEventAddrLen			*pEventAddrLen;
	TEventSectorInfo 		*pEventSectorInfo;
	BYTE					*pBuf;
	DWORD					dwOad;
	WORD					Len;
	WORD					off;
	BYTE					Tag;
	BYTE					status;
	BYTE					AllorSep;
	BYTE					Last;
}TEventData;

typedef struct TEventOadTable_t
{
	WORD	OI;
	WORD	Number;
	DWORD	OadList[EVENT_COLUMN_OAD_MAX_NUM];
}TEventOadTab;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
TEventPara	gEventPara;	// 事件门限及延时参数
TEventStatus EventStatus;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//调试用数组，查看每种事件的序号，调试版使用，正式版程序没有引用此数组，会被优化掉。
BYTE eEventTypehch[]=
{
	#if ( SEL_EVENT_LOST_V == YES )
	eEVENT_LOST_V_NO,						// 失压 0
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_WEAK_V == YES )
	eEVENT_WEAK_V_NO,						// 欠压 1
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_OVER_V == YES )
	eEVENT_OVER_V_NO,						// 过压 2
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_BREAK == YES )
	eEVENT_BREAK_NO,						// 断相 3
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_LOST_I == YES )
	eEVENT_LOST_I_NO,						// 失流 4
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_OVER_I == YES )
	eEVENT_OVER_I_NO,						// 过流 5
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_BREAK_I == YES )
	eEVENT_BREAK_I_NO,						// 断流 6
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_BACKPROP == YES )
	eEVENT_BACKPROP_NO,						// 功率反向 7
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_OVERLOAD == YES )
	eEVENT_OVERLOAD_NO,						// 过载 8
	#else
	0xff,
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	eEVENT_PA_DEMAND_OVER_NO,				//正向有功超限 9
	eEVENT_NA_DEMAND_OVER_NO,				//反向有功超限 10
	eEVENT_RE_DEMAND_OVER_NO,				//无功超限     11
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	#if( SEL_EVENT_COS_OVER == YES )
	eEVENT_COS_OVER_NO,						// 功率因数超限 12
	#else
	0xff,
	#endif
		
	#if( SEL_EVENT_LOST_ALL_V == YES )
	eEVENT_LOST_ALL_V_NO,					// 全失压 13
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	eEVENT_LOST_SECPOWER_NO,				//  辅助电源失电 14
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_V_REVERSAL == YES )
	eEVENT_V_REVERSAL_NO,					// 电压逆相序 15
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_I_REVERSAL == YES )
	eEVENT_I_REVERSAL_NO,					// 电流逆相序 16
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_LOST_POWER == YES )
	eEVENT_LOST_POWER_NO,					// 掉电 17
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )
	eEVENT_METERCOVER_NO,					// 开表盖 18
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_BUTTONCOVER == YES )
	eEVENT_BUTTONCOVER_NO,					// 开端钮盒 19
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_V_UNBALANCE == YES )
	eEVENT_V_UNBALANCE_NO,					// 电压不平衡 20
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	eEVENT_I_UNBALANCE_NO,					// 电流不平衡 21
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	eEVENT_MAGNETIC_INT_NO,					// 恒定磁场干扰 22
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_RELAY_ERR == YES )
	eEVENT_RELAY_ERR_NO,					// 负荷开关误动 23
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	eEVENT_POWER_ERR_NO,					// 电源异常 24
	#else
	0xff,
	#endif	
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	eEVENT_I_S_UNBALANCE_NO,				// 电流严重不平衡 25
	#else
	0xff,
	#endif

	#if(SEL_EVENT_RTC_ERR == YES)
	eEVENT_RTC_ERR_NO,						//时钟故障    26
	#else
	0xff,
	#endif
	
	#if(SEL_EVENT_SAMPLECHIP_ERR == YES)
	eEVENT_SAMPLECHIP_ERR_NO,				// 计量芯片故障  27
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	eEVENT_NEUTRAL_CURRENT_ERR_NO,                    // 电能表零线电流异常
	#else
	0xff,
	#endif
	
	eNUM_OF_EVENT,							// 工况类的事件数量 28

	// vvvvvvvvvvv编程类记录vvvvvvvvvvvvvv			
	ePRG_RECORD_NO,				// 电能表编程事件 28

	#if( SEL_PRG_INFO_CLEAR_METER == YES )
	ePRG_CLEAR_METER_NO,						// 清零记录 29
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )
	ePRG_CLEAR_MD_NO,							// 清需量记录 30
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
	ePRG_CLEAR_EVENT_NO,						// 清事件记录 31
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )
	ePRG_ADJUST_TIME_NO,						// 校时记录 32
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )
	ePRG_TIME_TABLE_NO,							// 时段表编程记录 33
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_TIME_AREA == YES )
	ePRG_TIME_AREA_NO,							// 时区表编程记录 34
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_WEEK == YES )
	ePRG_WEEKEND_NO,							// 周休日编程记录 35
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )
	ePRG_CLOSING_DAY_NO,						// 结算日编程记录 36
	#else
	0xff,
	#endif
	
	ePRG_OPEN_RELAY_NO,							// 拉闸 37
	ePRG_CLOSE_RELAY_NO,						// 合闸 38
	
	#if( SEL_PRG_INFO_HOLIDAY == YES )
	ePRG_HOLIDAY_NO,							// 节假日编程记录 39
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_P_COMBO == YES )
	ePRG_P_COMBO_NO,							// 有功组合方式编程记录 40
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )
	ePRG_Q_COMBO_NO,							// 无功组合方式编程记录 41
	#else
	0xff,
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	ePRG_TARIFF_TABLE_NO,						//电能表费率参数表编程事件
	ePRG_LADDER_TABLE_NO,						// 电能表阶梯表编程事件
	#else
	0xff,
	0xff,
	#endif
		
	#if( SEL_PRG_UPDATE_KEY == YES )
	ePRG_UPDATE_KEY_NO,							// 密钥更新记录 42
	#else
	0xff,
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	ePRG_ABR_CARD_NO,							//电能表异常插卡事件
	ePRG_BUY_MONEY_NO,							//电能表购电记录
	ePRG_RETURN_MONEY_NO,						//电能表退费记录
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )
	ePRG_BROADJUST_TIME_NO,						// 广播校时记录 43
	#else
	0xff,
	#endif

	
	eNUM_OF_EVENT_PRG,							// 事件和编程类总数 44

	// vvvvvvvvvvv之后的事件不存储记录vvvvvvvvvvvvvv
	#if( SEL_STAT_V_RUN == YES )
	eSTATISTIC_V_PASS_RATE_NO,					//44
	#else
	0xff,
	#endif

	eNUM_OF_EVENT_ALL,							// 所有的事件总数，包括不存储记录的 44
};

BYTE eSubEventTypehch[]=
{	//失压
	#if ( SEL_EVENT_LOST_V == YES )//0
	eSUB_EVENT_LOSTV_A,
	eSUB_EVENT_LOSTV_B,
	eSUB_EVENT_LOSTV_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
		
	//欠压
	#if( SEL_EVENT_WEAK_V == YES )//3
	eSUB_EVENT_WEAK_V_A,
	eSUB_EVENT_WEAK_V_B,
	eSUB_EVENT_WEAK_V_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//过压
	#if( SEL_EVENT_OVER_V == YES )//6
	eSUB_EVENT_OVER_V_A,
	eSUB_EVENT_OVER_V_B,
	eSUB_EVENT_OVER_V_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//断相
	#if( SEL_EVENT_BREAK == YES )//9
	eSUB_EVENT_BREAK_A,
	eSUB_EVENT_BREAK_B,
	eSUB_EVENT_BREAK_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//失流
	#if( SEL_EVENT_LOST_I == YES )//12
	eSUB_EVENT_LOSTI_A,
	eSUB_EVENT_LOSTI_B,
	eSUB_EVENT_LOSTI_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//过流
	#if( SEL_EVENT_OVER_I == YES )//15
		#if( MAX_PHASE == 3 )
		eSUB_EVENT_OVER_I_A,
		eSUB_EVENT_OVER_I_B,
		eSUB_EVENT_OVER_I_C,
		#else
		eSUB_EVENT_OVER_I_A,		//单相 0
		#endif
	#else
		#if( MAX_PHASE == 3 )
		0xff,
		0xff,
		0xff,
		#else
		0xff,
		#endif
	#endif
	
	//断流
	#if( SEL_EVENT_BREAK_I == YES )//18
	eSUB_EVENT_BREAK_I_A,
	eSUB_EVENT_BREAK_I_B,
	eSUB_EVENT_BREAK_I_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
		
	//功率反向
	#if( SEL_EVENT_BACKPROP == YES )//21
	eSUB_EVENT_POW_BACK_ALL,
	eSUB_EVENT_POW_BACK_A,
	eSUB_EVENT_POW_BACK_B,
	eSUB_EVENT_POW_BACK_C,
	#else
	0xff,
	0xff,
	0xff,
	0xff,
	#endif
	
	//过载
	#if( SEL_EVENT_OVERLOAD == YES )//25
	eSUB_EVENT_OVERLOAD_A,
	eSUB_EVENT_OVERLOAD_B,
	eSUB_EVENT_OVERLOAD_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	#if(SEL_DEMAND_OVER == YES)//28
	//正向有功需量超限
	eSUB_EVENT_PA_DEMAND_OVER,
	//反向有功需量超限
	eSUB_EVENT_NA_DEMAND_OVER,
	//一象限有功需量超限
	eSUB_EVENT_QI_DEMAND_OVER,
	//二象限有功需量超限
	eSUB_EVENT_QII_DEMAND_OVER,
	//三象限有功需量超限
	eSUB_EVENT_QIII_DEMAND_OVER,
	//四象限有功需量超限
	eSUB_EVENT_QIV_DEMAND_OVER,
	#else
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	#endif
	
	//功率因数超限
	#if( SEL_EVENT_COS_OVER == YES )//34
	eSUB_EVENT_COS_OVER_ALL,
	eSUB_EVENT_COS_OVER_A,
	eSUB_EVENT_COS_OVER_B,
	eSUB_EVENT_COS_OVER_C,
	#else
	0xff,
	0xff,
	0xff,
	0xff,
	#endif	
	
	//全失压
	#if( SEL_EVENT_LOST_ALL_V == YES )//35
	eSUB_EVENT_LOST_ALL_V,
	#else
	0xff,
	#endif
	
	//辅助电源失电事件
	#if( SEL_EVENT_LOST_SECPOWER == YES )//36
	eSUB_EVENT_LOST_SECPOWER,
	#else
	0xff,
	#endif
	
	//电压逆相序
	#if( SEL_EVENT_V_REVERSAL == YES )//37
	eSUB_EVENT_V_REVERSAL,
	#else
	0xff,
	#endif
	
	//电流逆相序
	#if( SEL_EVENT_I_REVERSAL == YES )//38
	eSUB_EVENT_I_REVERSAL,
	#else
	0xff,
	#endif

	//掉电事件
	#if(SEL_EVENT_LOST_POWER == YES)//39
	eSUB_EVENT_LOST_POWER,			//单相 1
	#else
	0xff,
	#endif
	
	//开表盖事件
	#if(SEL_EVENT_METERCOVER == YES)//40
	eSUB_EVENT_METER_COVER,			//单相 2
	#else
	0xff,
	#endif
	
	//开端钮盖事件
	#if(SEL_EVENT_BUTTONCOVER == YES)//41
	eSUB_EVENT_BUTTON_COVER,
	#else
	0xff,
	#endif
		
	//电压不平衡
	#if(SEL_EVENT_V_UNBALANCE == YES)//42
	eSUB_EVENT_V_UNBALANCE,
	#else
	0xff,
	#endif
	
	//电流不平衡
	#if(SEL_EVENT_I_UNBALANCE == YES)//43
	eSUB_EVENT_I_UNBALANCE,
	#else
	0xff,
	#endif
	
	//恒定磁场干扰
	#if(SEL_EVENT_MAGNETIC_INT == YES)//44
	eSUB_EVENT_MAGNETIC_INT,
	#else
	0xff,
	#endif
	
	//负荷开关误动作
	#if(SEL_EVENT_RELAY_ERR == YES)	//45
	eSUB_EVENT_RELAY_ERR,			//单相 3
	#else
	0xff,
	#endif
	
	//电源异常
	#if(SEL_EVENT_POWER_ERR == YES)	//46
	eSUB_EVENT_POWER_ERR,			//单相 4
	#else
	0xff,
	#endif
	
	//电流严重不平衡
	#if(SEL_EVENT_I_S_UNBALANCE == YES)//47
	eSUB_EVENT_S_I_UNBALANCE,
	#else
	0xff,
	#endif
	
	//时钟故障
	#if(SEL_EVENT_RTC_ERR == YES)	//48
	eSUB_EVENT_RTC_ERR,				//单相 5
	#else
	0xff,
	#endif
	
	// 计量芯片故障
	#if(SEL_EVENT_SAMPLECHIP_ERR == YES)//49
	eSUB_EVENT_SAMPLECHIP_ERR,		//单相 6
	#else
	0xff,
	#endif
	// 电能表零线电流异常
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	eSUB_NEUTRAL_CURRENT_ERR_NO,				
	#else
	0xff,
	#endif
	
	//电能表编程事件//50
	eSUB_EVENT_PRG_RECORD,			//单相 7
	
	//电能表清零事件
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//51		
	eSUB_EVENT_PRG_CLEAR_METER,		//单相 8	
	#else
	0xff,	
	#endif
	
	//电能表需量清零事件	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )//52		
	eSUB_EVENT_PRG_CLEAR_MD,		
	#else
	0xff,		
	#endif
											
	//电能表事件清零事件	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//53	
	eSUB_EVENT_PRG_CLEAR_EVENT,			//单相 9
	#else
	0xff,		
	#endif
	
	//电能表校时事件		
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//54	
	eSUB_EVENT_PRG_ADJUST_TIME,			//单相 10	
	#else
	0xff,	
	#endif
	
	//电能表时段表编程事件	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )//55	
	eSUB_EVENT_PRG_TIME_TABLE,			//单相 11	
	#else
	0xff,		
	#endif
	
	//电能表时区表编程事件	
	#if( SEL_PRG_INFO_TIME_AREA == YES )//56	
	eSUB_EVENT_PRG_TIME_AREA,			//单相 12	
	#else
	0xff,		
	#endif
	
	//电能表周休日编程事件	
	#if( SEL_PRG_INFO_WEEK == YES )//57			
	eSUB_EVENT_PRG_WEEKEND,				//单相 13	
	#else
	0xff,	
	#endif
	
	//电能表结算日编程事件	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//58	
	eSUB_EVENT_PRG_CLOSING_DAY,			//单相 14	
	#else
	0xff,		
	#endif
	
	//电能表跳闸合闸事件		
	eSUB_EVENT_PRG_OPEN_RELAY,		//单相 15			
	eSUB_EVENT_PRG_CLOSE_RELAY,		//单相 16	
	
	//节假日编程记录		
	#if( SEL_PRG_INFO_HOLIDAY == YES )//61		
	eSUB_EVENT_PRG_HOLIDAY,			//单相 17	
	#else
	0xff,	
	#endif
	
	//有功组合方式编程记录	
	#if( SEL_PRG_INFO_P_COMBO == YES )//62		
	eSUB_EVENT_ePRG_P_COMBO,		//单相 18	
	#else
	0xff,			
	#endif
	
	//无功组合方式编程记录	
	#if( SEL_PRG_INFO_Q_COMBO == YES )//63		
	eSUB_EVENT_PRG_Q_COMBO,		
	#else
	0xff,		
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	eSUB_TARIFF_TABLE_NO,						//电能表费率参数表编程事件
	eSUB_LADDER_TABLE_NO,						// 电能表阶梯表编程事件
	#else
	0xff,
	0xff,
	#endif
	
	//电能表密钥更新事件	
	#if( SEL_PRG_UPDATE_KEY == YES )//64		
	eSUB_EVENT_PRG_UPDATE_KEY,		//单相 19
	#else
	0xff,
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	eSUB_ABR_CARD_NO,							//电能表异常插卡事件
	eSUB_BUY_MONEY_NO,							//电能表购电记录
	eSUB_RETURN_MONEY_NO,						//电能表退费记录
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//电能表广播校时事件	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )//54	
	eSUB_EVENT_BROADJUST_TIME,			//单相 10	
	#else
	0xff,	
	#endif
	
	eSUB_EVENT_STATUS_TOTAL				//最大值65 单相20

};

// 事件信息表，用于查找事件状态函数  此表格应与eSUB_EVENT_INDEX里面定义的枚举顺序保持一致
const TSubEventInfoTab	SubEventInfoTab[] = 
{
	#if ( SEL_EVENT_LOST_V == YES )
	//失压事件
	{eSUB_EVENT_LOSTV_A, 			eEVENT_LOST_V_NO, 			ePHASE_A, 	GetLostVStatus },
	{eSUB_EVENT_LOSTV_B, 			eEVENT_LOST_V_NO, 			ePHASE_B, 	GetLostVStatus },
	{eSUB_EVENT_LOSTV_C, 			eEVENT_LOST_V_NO, 			ePHASE_C, 	GetLostVStatus },
	#endif

	#if( SEL_EVENT_WEAK_V == YES )
	//欠压事件
	{eSUB_EVENT_WEAK_V_A, 			eEVENT_WEAK_V_NO, 			ePHASE_A, 	GetWeakVStatus },	
	{eSUB_EVENT_WEAK_V_B, 			eEVENT_WEAK_V_NO, 			ePHASE_B, 	GetWeakVStatus }, 
	{eSUB_EVENT_WEAK_V_C, 			eEVENT_WEAK_V_NO, 			ePHASE_C, 	GetWeakVStatus }, 
	#endif

	#if( SEL_EVENT_OVER_V == YES )
	//过压事件
	{eSUB_EVENT_OVER_V_A, 			eEVENT_OVER_V_NO, 			ePHASE_A, 	GetOverVStatus }, 
	{eSUB_EVENT_OVER_V_B, 			eEVENT_OVER_V_NO, 			ePHASE_B, 	GetOverVStatus }, 
	{eSUB_EVENT_OVER_V_C, 			eEVENT_OVER_V_NO, 			ePHASE_C, 	GetOverVStatus }, 
	#endif

	#if( SEL_EVENT_BREAK == YES )
	//断相事件
	{eSUB_EVENT_BREAK_A, 			eEVENT_BREAK_NO, 			ePHASE_A, 	GetBreakStatus }, 
	{eSUB_EVENT_BREAK_B, 			eEVENT_BREAK_NO, 			ePHASE_B, 	GetBreakStatus }, 
	{eSUB_EVENT_BREAK_C, 			eEVENT_BREAK_NO, 			ePHASE_C, 	GetBreakStatus }, 
	#endif
	
	#if( SEL_EVENT_LOST_I == YES )
	//失流事件
	{eSUB_EVENT_LOSTI_A, 			eEVENT_LOST_I_NO, 			ePHASE_A, 	GetLostIStatus }, 
	{eSUB_EVENT_LOSTI_B, 			eEVENT_LOST_I_NO, 			ePHASE_B, 	GetLostIStatus }, 
	{eSUB_EVENT_LOSTI_C, 			eEVENT_LOST_I_NO, 			ePHASE_C, 	GetLostIStatus }, 
	#endif

	#if( SEL_EVENT_OVER_I == YES )
	//过流事件
	#if( MAX_PHASE == 3 )
	{eSUB_EVENT_OVER_I_A, 			eEVENT_OVER_I_NO, 			ePHASE_A, 	GetOverIStatus },
	{eSUB_EVENT_OVER_I_B, 			eEVENT_OVER_I_NO, 			ePHASE_B, 	GetOverIStatus },	
	{eSUB_EVENT_OVER_I_C, 			eEVENT_OVER_I_NO, 			ePHASE_C, 	GetOverIStatus },	
	#else
	{eSUB_EVENT_OVER_I_A, 			eEVENT_OVER_I_NO, 			ePHASE_A, 	GetOverIStatus },
	#endif
	#endif

	#if( SEL_EVENT_BREAK_I == YES )
	//断流事件
	{eSUB_EVENT_BREAK_I_A, 			eEVENT_BREAK_I_NO, 			ePHASE_A, 	GetBreakIStatus },
	{eSUB_EVENT_BREAK_I_B, 			eEVENT_BREAK_I_NO, 			ePHASE_B, 	GetBreakIStatus },	
	{eSUB_EVENT_BREAK_I_C, 			eEVENT_BREAK_I_NO, 			ePHASE_C, 	GetBreakIStatus },	
	#endif
	
	#if( SEL_EVENT_BACKPROP == YES )//不要区分三相三和三相四，三相三的也要在表格里面有总ABC,规约都的时候是按照这个查找的
	//功率反向
	{eSUB_EVENT_POW_BACK_ALL, 		eEVENT_BACKPROP_NO, 		ePHASE_ALL, GetBackpropStatus },	
	{eSUB_EVENT_POW_BACK_A, 		eEVENT_BACKPROP_NO, 		ePHASE_A, 	GetBackpropStatus },
	{eSUB_EVENT_POW_BACK_B, 		eEVENT_BACKPROP_NO, 		ePHASE_B, 	GetBackpropStatus },	
	{eSUB_EVENT_POW_BACK_C, 		eEVENT_BACKPROP_NO, 		ePHASE_C, 	GetBackpropStatus },	
	#endif
	
	#if( SEL_EVENT_OVERLOAD == YES )
	//过载事件
	{eSUB_EVENT_OVERLOAD_A, 		eEVENT_OVERLOAD_NO, 		ePHASE_A, 	GetOverloadStatus },
	{eSUB_EVENT_OVERLOAD_B, 		eEVENT_OVERLOAD_NO, 		ePHASE_B, 	GetOverloadStatus },	
	{eSUB_EVENT_OVERLOAD_C, 		eEVENT_OVERLOAD_NO, 		ePHASE_C, 	GetOverloadStatus },	
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	//需量超限事件
	{eSUB_EVENT_PA_DEMAND_OVER,		eEVENT_PA_DEMAND_OVER_NO, 	ePHASE_ALL, GetPADemandOverStatus },	
	{eSUB_EVENT_NA_DEMAND_OVER, 	eEVENT_NA_DEMAND_OVER_NO, 	ePHASE_ALL, GetNADemandOverStatus },	
	{eSUB_EVENT_QI_DEMAND_OVER, 	eEVENT_RE_DEMAND_OVER_NO, 	ePHASE_ALL, GetQDemandOverStatus },	
	{eSUB_EVENT_QII_DEMAND_OVER, 	eEVENT_RE_DEMAND_OVER_NO, 	ePHASE_A, 	GetQDemandOverStatus },	
	{eSUB_EVENT_QIII_DEMAND_OVER,	eEVENT_RE_DEMAND_OVER_NO, 	ePHASE_B, 	GetQDemandOverStatus },	
	{eSUB_EVENT_QIV_DEMAND_OVER, 	eEVENT_RE_DEMAND_OVER_NO,	ePHASE_C, 	GetQDemandOverStatus },	
    #endif
    
    #if( SEL_EVENT_COS_OVER == YES )
	//总功率因数超限事件
	{eSUB_EVENT_COS_OVER_ALL, 		eEVENT_COS_OVER_NO, 		ePHASE_ALL, GetCosoverStatus },	
	{eSUB_EVENT_COS_OVER_A, 		eEVENT_COS_OVER_NO, 		ePHASE_A, 	GetCosoverStatus },	
	{eSUB_EVENT_COS_OVER_B, 		eEVENT_COS_OVER_NO, 		ePHASE_B,	GetCosoverStatus },	
	{eSUB_EVENT_COS_OVER_C, 		eEVENT_COS_OVER_NO, 		ePHASE_C,	GetCosoverStatus },	
	#endif

	#if( SEL_EVENT_LOST_ALL_V == YES )
	//全失压事件
	{eSUB_EVENT_LOST_ALL_V, 		eEVENT_LOST_ALL_V_NO, 		ePHASE_ALL, GetLostAllVStatus }, 
	#endif

	#if( SEL_EVENT_LOST_SECPOWER == YES )
	//辅助电源失电事件
	{eSUB_EVENT_LOST_SECPOWER, 		eEVENT_LOST_SECPOWER_NO, 	ePHASE_ALL, GetLostSecPowerStatus }, 
	#endif

	#if( SEL_EVENT_V_REVERSAL == YES )
	//电压逆相序事件
	{eSUB_EVENT_V_REVERSAL, 		eEVENT_V_REVERSAL_NO, 		ePHASE_ALL, GetVReversalStatus }, 
	#endif

	#if( SEL_EVENT_I_REVERSAL == YES )
	//电流逆相序事件
	{eSUB_EVENT_I_REVERSAL, 		eEVENT_I_REVERSAL_NO, 		ePHASE_ALL, GetIReversalStatus }, 
	#endif
	
	#if( SEL_EVENT_LOST_POWER == YES )
	//掉电事件
	{eSUB_EVENT_LOST_POWER, 		eEVENT_LOST_POWER_NO, 		ePHASE_ALL, GetLostPowerStatus },	
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )
	//开表盖事件
	{eSUB_EVENT_METER_COVER, 		eEVENT_METERCOVER_NO, 		ePHASE_ALL, GetMeterCoverStatus },	
	#endif
	
	#if( SEL_EVENT_BUTTONCOVER == YES )
	//开端钮盒事件
	{eSUB_EVENT_BUTTON_COVER, 		eEVENT_BUTTONCOVER_NO, 		ePHASE_ALL, GetButtonCoverStatus },	
	#endif

	#if( SEL_EVENT_V_UNBALANCE == YES )
	//电压不平衡事件
	{eSUB_EVENT_V_UNBALANCE, 		eEVENT_V_UNBALANCE_NO, 		ePHASE_ALL, GetVUnBalanceStatus }, 
	#endif

	#if( SEL_EVENT_I_UNBALANCE == YES )
	//电流不平衡事件
	{eSUB_EVENT_I_UNBALANCE, 		eEVENT_I_UNBALANCE_NO, 		ePHASE_ALL, GetIUnBalanceStatus }, 
	#endif
	
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	//恒定磁场干扰
	{eSUB_EVENT_MAGNETIC_INT, 		eEVENT_MAGNETIC_INT_NO, 	ePHASE_ALL, GetMagneticStatus },	
	#endif
	
	#if( SEL_EVENT_RELAY_ERR == YES )
	//负荷开关误动作
	{eSUB_EVENT_RELAY_ERR, 			eEVENT_RELAY_ERR_NO, 		ePHASE_ALL, GetRelayErrStatus },	
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	//电源异常事件
	{eSUB_EVENT_POWER_ERR, 			eEVENT_POWER_ERR_NO, 		ePHASE_ALL, GetPowerErrStatus },	
    #endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	//电流严重不平衡记录
	{eSUB_EVENT_S_I_UNBALANCE, 		eEVENT_I_S_UNBALANCE_NO, 	ePHASE_ALL, GetISUnBalanceStatus },	
	#endif
	
	#if( SEL_EVENT_RTC_ERR == YES )
    //时钟故障
    {eSUB_EVENT_RTC_ERR, 			eEVENT_RTC_ERR_NO, 			ePHASE_ALL, GetRtcErrStatus },	
	#endif

	#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
	// 计量芯片故障
	{eSUB_EVENT_SAMPLECHIP_ERR, 	eEVENT_SAMPLECHIP_ERR_NO, 	ePHASE_ALL,	GetSampleChipErrStatus },	
	#endif
	
	//电能表零线电流异常
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )//			
	{eSUB_NEUTRAL_CURRENT_ERR_NO,eEVENT_NEUTRAL_CURRENT_ERR_NO,ePHASE_ALL, GetNeutralCurrentErrStatus },	
	#endif
	
	//电能表编程事件
	{eSUB_EVENT_PRG_RECORD, 		ePRG_RECORD_NO,		 		ePHASE_ALL, GetPrgStatus },	
	
	//电能表清零事件
	#if( SEL_PRG_INFO_CLEAR_METER == YES )	
	{eSUB_EVENT_PRG_CLEAR_METER, 	ePRG_CLEAR_METER_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//电能表需量清零事件	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )//42					
	{eSUB_EVENT_PRG_CLEAR_MD, 		ePRG_CLEAR_MD_NO, 			ePHASE_ALL, GetPrgStatus },	
	#endif
											
	//电能表事件清零事件	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//43			
	{eSUB_EVENT_PRG_CLEAR_EVENT, 	ePRG_CLEAR_EVENT_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//电能表校时事件		
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//44			
	{eSUB_EVENT_PRG_ADJUST_TIME, 	ePRG_ADJUST_TIME_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//电能表时段表编程事件	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )//45			
	{eSUB_EVENT_PRG_TIME_TABLE, 	ePRG_TIME_TABLE_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//电能表时区表编程事件	
	#if( SEL_PRG_INFO_TIME_AREA == YES )//46			
	{eSUB_EVENT_PRG_TIME_AREA, 		ePRG_TIME_AREA_NO,	 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//电能表周休日编程事件	
	#if( SEL_PRG_INFO_WEEK == YES )//47					
	{eSUB_EVENT_PRG_WEEKEND,	 	ePRG_WEEKEND_NO, 			ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//电能表结算日编程事件	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//48				
	{eSUB_EVENT_PRG_CLOSING_DAY, 	ePRG_CLOSING_DAY_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//电能表跳闸合闸事件			
	{eSUB_EVENT_PRG_OPEN_RELAY, 	ePRG_OPEN_RELAY_NO, 		ePHASE_ALL, GetPrgStatus }, 
	{eSUB_EVENT_PRG_CLOSE_RELAY, 	ePRG_CLOSE_RELAY_NO, 		ePHASE_ALL, GetPrgStatus }, 			
	
	//节假日编程记录		
	#if( SEL_PRG_INFO_HOLIDAY == YES )//55			
	{eSUB_EVENT_PRG_HOLIDAY,	 	ePRG_HOLIDAY_NO, 			ePHASE_ALL, GetPrgStatus },			
	#endif
	
	//有功组合方式编程记录	
	#if( SEL_PRG_INFO_P_COMBO == YES )//56		
	{eSUB_EVENT_ePRG_P_COMBO, 		ePRG_P_COMBO_NO, 			ePHASE_ALL, GetPrgStatus },				
	#endif
	
	//无功组合方式编程记录	
	#if( SEL_PRG_INFO_Q_COMBO == YES )//57		
	{eSUB_EVENT_PRG_Q_COMBO,	 	ePRG_Q_COMBO_NO,	 		ePHASE_ALL, GetPrgStatus },
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	//电能表费率参数表编程事件
	{eSUB_TARIFF_TABLE_NO,			ePRG_TARIFF_TABLE_NO,		ePHASE_ALL,	GetPrgStatus},
	// 电能表阶梯表编程事件
	{eSUB_LADDER_TABLE_NO,			ePRG_LADDER_TABLE_NO,		ePHASE_ALL,	GetPrgStatus},
	#endif
	
	//电能表密钥更新事件	
	#if( SEL_PRG_UPDATE_KEY == YES )//58			
	{eSUB_EVENT_PRG_UPDATE_KEY,		ePRG_UPDATE_KEY_NO,			ePHASE_ALL, GetPrgStatus },
	#endif 
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//电能表异常插卡事件
	{eSUB_ABR_CARD_NO,				ePRG_ABR_CARD_NO,			ePHASE_ALL,	GetPrgStatus},
	//电能表购电记录
	{eSUB_BUY_MONEY_NO,				ePRG_BUY_MONEY_NO,			ePHASE_ALL,	GetPrgStatus},
	//电能表退费记录
	{eSUB_RETURN_MONEY_NO,			ePRG_RETURN_MONEY_NO,		ePHASE_ALL,	GetPrgStatus},
	#endif
	
	//电能表广播校时事件		
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )//			
	{eSUB_EVENT_BROADJUST_TIME, 	ePRG_BROADJUST_TIME_NO, 	ePHASE_ALL, GetPrgStatus },	
	#endif
};

//此表格应与eEVENT_INDEX里面定义枚举的顺序保持一致
const TEventInfoTab EventInfoTab[] =
{
	#if( SEL_EVENT_LOST_V == YES )		//失压
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 电压上限	电压下限	电流下限	判断时间 
	{0x3000,eEVENT_LOST_V_NO,		MAX_PHASE,			NO,		0,				4,	eTYPE_WORD,	eTYPE_SPEC1,eTYPE_DWORD,eTYPE_BYTE,},
	#endif
	
	#if( SEL_EVENT_WEAK_V == YES )		//欠压
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 电压上限	判断时间		 
	{0x3001,eEVENT_WEAK_V_NO,		MAX_PHASE,			NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_OVER_V == YES )		//过压
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 电压下限	判断时间 
	{0x3002,eEVENT_OVER_V_NO,		MAX_PHASE,			NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_BREAK == YES )		//断相
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 电压上限	电流上限	判断时间	 
	{0x3003,eEVENT_BREAK_NO,		MAX_PHASE,			NO,		0,		    	3,	eTYPE_WORD,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_LOST_I == YES )		//失流
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 电压下限	电流上限	电流下限	判断时间 
	{0x3004,eEVENT_LOST_I_NO,		MAX_PHASE,			NO, 	0,		    	4,	eTYPE_WORD,	eTYPE_DWORD,eTYPE_SPEC1,eTYPE_BYTE,},
	#endif
		
	#if( SEL_EVENT_OVER_I == YES )		//过流
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 电流下限	判断时间 
	{0x3005,eEVENT_OVER_I_NO,		MAX_PHASE,			NO,		0,		    	2,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	
	#if( SEL_EVENT_BREAK_I == YES )		//断流
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 电压下限	电流上限	判断时间 
	{0x3006,eEVENT_BREAK_I_NO,		MAX_PHASE,			NO,		0,		    	3,	eTYPE_WORD,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,},
	#endif
	
	
	#if( SEL_EVENT_BACKPROP == YES )	//功率反向
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 功率下限	判断时间 
	{0x3007,eEVENT_BACKPROP_NO,		EVENT_PHASE_FOUR,	NO,		0,		    	2,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	
	#if( SEL_EVENT_OVERLOAD == YES )	//过载
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 功率下限	判断时间 
	{0x3008,eEVENT_OVERLOAD_NO,		MAX_PHASE,			NO,		0,		    	2,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	//0x3009	电能表正向有功需量超限事件
	//0x300a	电能表反向有功需量超限事件
	//0x300b	电能表无功需量超限事件
	#if( SEL_DEMAND_OVER == YES )
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 
	{0x3009,eEVENT_PA_DEMAND_OVER_NO,	1,					NO,	sizeof(TDemandData),2,	eTYPE_DWORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL},
	{0x300a,eEVENT_NA_DEMAND_OVER_NO,	1,					NO,	sizeof(TDemandData),2,	eTYPE_DWORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL},
	{0x300b,eEVENT_RE_DEMAND_OVER_NO,	EVENT_PHASE_FOUR,	NO,	sizeof(TDemandData),2,	eTYPE_DWORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL},
	#endif
	
	#if( SEL_EVENT_COS_OVER == YES )	//功率因数超限
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 下限阀值	判断时间 
	{0x303B,eEVENT_COS_OVER_NO,			EVENT_PHASE_FOUR,	NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_LOST_ALL_V == YES )	//全失压
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数  
	{0x300d,eEVENT_LOST_ALL_V_NO,		1,					YES,	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )//辅助电源失电
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 判断时间 
	{0x300e,eEVENT_LOST_SECPOWER_NO,	1, 					YES,	0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_V_REVERSAL == YES )	//电压逆相序
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 判断时间 
	{0x300f,eEVENT_V_REVERSAL_NO,		1,					NO,		0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_I_REVERSAL == YES )	//电流逆相序
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 判断时间 
	{0x3010,eEVENT_I_REVERSAL_NO,		1,					NO,		0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_LOST_POWER == YES )	//掉电
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 判断时间 
	{0x3011,eEVENT_LOST_POWER_NO,		1, 					YES,	0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )	//开表盖
	//OI	事件枚举号					获取地址长度			最大相		掉电特殊处理 扩展长度		参数个数 电压		电压下限	电流下限	判断时间 
	{0x301b,eEVENT_METERCOVER_NO,		1,					YES,	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_BUTTONCOVER == YES )	//开端钮盖事件		
	//OI	事件枚举号					获取地址长度			最大相		掉电特殊处理 扩展长度		参数个数 电压		电压下限	电流下限	判断时间 
	{0x301c,eEVENT_BUTTONCOVER_NO,		1,					YES,	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_V_UNBALANCE == YES )	//电压不平衡
	//OI	事件枚举号					获取地址长度			最大相		掉电特殊处理 扩展长度		参数个数 限值		判断时间 
	{0x301d,eEVENT_V_UNBALANCE_NO,		1,					NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )	//电流不平衡
	//OI	事件枚举号					获取地址长度			最大相		掉电特殊处理 扩展长度		参数个数 限值		判断时间 
	{0x301e,eEVENT_I_UNBALANCE_NO,		1,					NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
										//电能表编程事件
	#if( SEL_EVENT_MAGNETIC_INT == YES )//恒定磁场干扰
	//OI	事件枚举号					获取地址长度			最大相		掉电特殊处理 扩展长度		参数个数 判断时间 
	{0x302a,eEVENT_MAGNETIC_INT_NO,		1,					NO,		0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_RELAY_ERR == YES )	//负荷开关误动作
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 判断时间 
	{0x302b,eEVENT_RELAY_ERR_NO,		1,					NO, 	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )	//电源异常事件
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 判断时间 
	{0x302c,eEVENT_POWER_ERR_NO,		1,					NO, 	0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )//电流严重不平衡
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数 限值		判断时间 
	{0x302d,eEVENT_I_S_UNBALANCE_NO,	1,					NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_RTC_ERR == YES )		//时钟故障
	//OI	事件枚举号					最大相		掉电特殊处理 扩展长度		参数个数  
	{0x302e,eEVENT_RTC_ERR_NO,			1,					YES,	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL},
	#endif
	
	#if( SEL_EVENT_SAMPLECHIP_ERR == YES )// 计量芯片故障
	//OI	事件枚举号					获取地址长度			最大相		掉电特殊处理 扩展长度		参数个数  
	{0x302f,eEVENT_SAMPLECHIP_ERR_NO,	1,					NO,		0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL},
	#endif

	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )//电能表零线电流异常事件		
	{0x3040,eEVENT_NEUTRAL_CURRENT_ERR_NO,		1,			NO,		0,				3,	eTYPE_DWORD,eTYPE_WORD,eTYPE_BYTE,	eTYPE_NULL,},
	#endif
	
	{0x3012,ePRG_RECORD_NO, 			1,				NO,sizeof(TPrgProgramSubRom),0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//电能表清零事件		
	{0x3013,ePRG_CLEAR_METER_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )	//电能表需量清零事件	
	{0x3014,ePRG_CLEAR_MD_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
												
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//电能表事件清零事件	
	{0x3015,ePRG_CLEAR_EVENT_NO,		1,					NO,		4,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//电能表校时事件		
	{0x3016,ePRG_ADJUST_TIME_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )//电能表时段表编程事件								时段表+OAD
	{0x3017,ePRG_TIME_TABLE_NO,			1,					NO,		42+4,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_TIME_AREA == YES )//电能表时区表编程事件	
	{0x3018,ePRG_TIME_AREA_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_WEEK == YES )		//电能表周休日编程事件	
	{0x3019,ePRG_WEEKEND_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},							
	#endif
	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//电能表结算日编程事件	
	{0x301a,ePRG_CLOSING_DAY_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	{0x301f,ePRG_OPEN_RELAY_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{0x3020,ePRG_CLOSE_RELAY_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	
	#if( SEL_PRG_INFO_HOLIDAY == YES )	//节假日编程记录									//date+时段表号+OAD
	{0x3021,ePRG_HOLIDAY_NO,			1,					NO,		6+4,			0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_P_COMBO == YES )	//有功组合方式编程记录	
	{0x3022,ePRG_P_COMBO_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )	//无功组合方式编程记录	
	{0x3023, ePRG_Q_COMBO_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	//电能表费率参数表编程事件
	{0x3024,ePRG_TARIFF_TABLE_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	// 电能表阶梯表编程事件
	{0x3025,ePRG_LADDER_TABLE_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	#endif
	
	#if( SEL_PRG_UPDATE_KEY == YES )	//电能表密钥更新事件	
	{0x3026,ePRG_UPDATE_KEY_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//电能表异常插卡事件 18=8+1+7+2 其中7：命令头，命令头保存时保存 CH,Cla,Ins,P1,P2,P31,P32,其中CH:0x01--ESAM 0x00--CARD,esam命令头为CH后的6字节，卡为CH后的5字节
	{0x3027,ePRG_ABR_CARD_NO,			1,					NO,		18,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	//电能表购电记录
	{0x3028,ePRG_BUY_MONEY_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	//电能表退费记录
	{0x3029,ePRG_RETURN_MONEY_NO,		1,					NO,		4,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	#endif
	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )//电能表广播校时事件		
	{0x303C,ePRG_BROADJUST_TIME_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_STAT_V_RUN == YES )		//电压合格率
	//OI	事件枚举号					获取地址长度			最大相		掉电特殊处理 扩展长度		参数个数 考核上限	考核下限	合格上限	合格下限 
	{0x4030,eSTATISTIC_V_PASS_RATE_NO,	1,					NO,		0,		    	4,	eTYPE_WORD,	eTYPE_SPEC1,eTYPE_SPEC2,eTYPE_SPEC3,},
	#endif
};

// 所有固定列的OAD
const DWORD FixedColumnOadTab[] =
{
	RECORDNO_OAD,		// 序号
	STARTTIME_OAD,		// 开始时间
	ENDTIME_OAD,		// 结束时间
	GEN_SOURCE_OAD,		// 发生源
	NOTIFICATION_OAD,	// 上报状态
	MAXDEMAND_OAD,		// 超限期间最大需量
	MAXDEMAND_TIME_OAD,	// 超限期间最大需量发生时间
	PRG_OBJ_LIST_OAD,	// 编程对象列表
	CLR_EVENT_LIST_OAD	// 事件清零列表
};

// OAD 固定列列表 如果事件记录单元为标准事件单元可不加到此数组中，特殊事件记录单元必须加过来
static const TEventOadTab	EventOadTab[] = 
{
	{0xFFFF,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 7
	{0x3000,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3001,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3002,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3003,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3004,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3005,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3006,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3007,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3008,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x303B,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3009,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,MAXDEMAND_OAD,MAXDEMAND_TIME_OAD },	// 正向有功需量超限 class id 7
	{0x300a,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,MAXDEMAND_OAD,MAXDEMAND_TIME_OAD },	// 反向有功需量超限 class id 7
	{0x300b,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,MAXDEMAND_OAD,MAXDEMAND_TIME_OAD },	// 无功需量超限 class id 24
	{0x3012,6,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,PRG_OBJ_LIST_OAD },					// 编程记录 class id 7
	{0x3015,6,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,CLR_EVENT_LIST_OAD },					// 事件清零 class id 7
	{0x3017,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,PRG_SEGTABLE_OAD,PRG_SEGTABLE_VALUE }, //电能表时段表编程事件3017
	{0x3021,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,PRG_HOLIDAY_OAD,PRG_HOLIDAY_VALUE},	//电能表节假日编程事件3021	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	{0x3027,9,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,CARD_SN_OAD,CARD_ERR_INFO_OAD,CARD_ORDER_HEAD_OAD,CARD_ERR_RES_OAD},//电能表异常插卡事件0x3027
	{0x3029,6,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,RETURN_MONEY_OAD},	//电能表退费记录3029	
	#endif
};

static BYTE 	SubEventTimer[SUB_EVENT_INDEX_MAX_NUM];	// 定时计数器
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 根据事件类型及相位确定分相事件枚举号
//
//参数: 
//			EventIndex[in]:	事件枚举号                    
//			Phase[in]:		相位                    
//返回值:	分相事件枚举号	
//备注:
//-----------------------------------------------
BYTE GetSubEventIndex(TEventAddrLen *pEventAddrLen)
{
	BYTE i;
	
	for( i=0; i<(sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab)); i++ )
	{
		if( 	(SubEventInfoTab[i].EventIndex==pEventAddrLen->EventIndex)
			&& 	(SubEventInfoTab[i].Phase==pEventAddrLen->Phase) )
		{
			break;
		}
	}

	if( i == (sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab)) )//如果没有搜索到 置默认值
	{
        i = 0;
        ASSERT( FALSE, 0 );
	}
	
	return SubEventInfoTab[i].SubEventIndex;
}

//-----------------------------------------------
//函数功能: 获取OI对应的EventIndex
//
//参数: 
//	OI[in]:	事件OI
//                    
//	pEventOrder[out]:	指向EventOrder的指针
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//			
//备注:
//-----------------------------------------------
BOOL GetEventIndex( WORD OI, BYTE *pEventIndex )
{
	BYTE i;

	for( i = 0; i < (sizeof(EventInfoTab)/sizeof(TEventInfoTab)); i++ )
	{
		if( OI == EventInfoTab[i].OI )
		{
			*pEventIndex = EventInfoTab[i].EventIndex;
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------
//函数功能: 获取inEventIndex对应的OI
//参数: 
//			inEventIndex[in]:	事件枚举号              
//返回值:	OI		
//备注:
//-----------------------------------------------
WORD GetEventOI( BYTE inEventIndex )
{
	BYTE i;

	for( i = 0; i < (sizeof(EventInfoTab)/sizeof(TEventInfoTab)); i++ )
	{
		if( inEventIndex == EventInfoTab[i].EventIndex )
		{
			return EventInfoTab[i].OI;
		}
	}

	return EventInfoTab[0].OI;
}

//-----------------------------------------------
//函数功能: 获取事件记录的基地址信息
//
//参数: 		BYTE i[in]
//			pTEventAddrLen[in/out]
//			EventIndex[in]		   --输入事件枚举号
//			Phase  [in] 		   --输入相，编程类均为0
//			dwAttInfoEeAddr[out]   --输出对应类编程记录的关联对象属性
//			dwDataInfoEeAddr[out]  --输出eeprom中的info数据地址
//			dwRecordAddr[out]	   --输出对应类编程记录的固定列数据地址
//返回值:  	TRUE
//
//备注:       以下每个函数为每类事件记录的获取地址函数
//-----------------------------------------------
BOOL GetEventRecordInfo(BYTE i, TEventAddrLen *pTEventAddrLen )
{
	
	if( pTEventAddrLen->EventIndex > eNUM_OF_EVENT_PRG )//极限值检测
	{
		return FALSE;
	}
	
	if( pTEventAddrLen->SubEventIndex > eSUB_EVENT_STATUS_TOTAL )
	{
		return FALSE;
	}
	
	if( pTEventAddrLen->Phase > ePHASE_C )
	{
		return FALSE;
	}

	if( i > sizeof(EventInfoTab)/sizeof(TEventInfoTab) )
	{
		return FALSE;
	}

	pTEventAddrLen->dwAttInfoEeAddr = GET_CONTINUE_ADDR( EventConRom.EventAttInfo[pTEventAddrLen->EventIndex] );
	pTEventAddrLen->dwDataInfoEeAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.EventDataInfo[pTEventAddrLen->SubEventIndex] );
	pTEventAddrLen->dwRecordAddr = FLASH_EVENT_BASE+(DWORD)pTEventAddrLen->SubEventIndex*EVENT_RECORD_LEN;

	return TRUE;
}

//-----------------------------------------------
//函数功能: 获取Type对应的属性地址，数据信息地址，数据地址，公共数据地址，最大数据长度
//
//参数: 
//	pTEventAddrLen:	TEventAddrLen类型的指针                    
// 		dwAttInfoEeAddr[out]:		OAD信息地址
// 		dwDataInfoEeAddr[out]:		数据信息地址
// 		dwRecordAddr[out]:			公共数据地址
// 		wDataLen[out]:				最大数据长度
// 		EventIndex[in]:				eEVENT_INDEX
// 		Phase[in]:					相位0总,1A,2B,3C
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//			
//备注:
//-----------------------------------------------
BOOL GetEventInfo( TEventAddrLen *pEventAddrLen )
{
	BYTE i;

	for( i = 0; i < sizeof(EventInfoTab)/sizeof(TEventInfoTab); i++ )
	{
		if( EventInfoTab[i].EventIndex == pEventAddrLen->EventIndex )
		{
			pEventAddrLen->SubEventIndex = GetSubEventIndex(pEventAddrLen);
			return GetEventRecordInfo( i, pEventAddrLen );
		}
	}

	return FALSE;
}
//-----------------------------------------------
//函数功能: 获取事件数据存储的偏移地址相关信息
//
//参数: 
// 			Len[in]:事件的整条记录的长度
// 			RecordDepth[in]:	此OAD的深度
// 			inRecordNo[in]:	此OAD数据的RecordNo
// 			pSectorInfo[out]:指向SectorInfo的指针
//返回值:
//	无
//
//备注: 
//-----------------------------------------------
 BOOL GetEventSectorInfo( WORD Len, WORD RecordDepth, DWORD inRecordNo, TEventSectorInfo *pEventSectorInfo )
{
	WORD twSectorNo;			// 此OAD占用的扇区个数	
	WORD twOadNoPerSector;		// 一个扇区能存放的OAD的条数
	WORD twSectorOff;			// 扇区的偏移
	WORD twInSectorOff;			// 扇区内的偏移
	
	
	if( (Len<4) || (RecordDepth==0) || ( Len > EVENT_VALID_OADLEN) )
	{

		return FALSE;
	}
	else
	{
		// 一个扇区存的OAD的个数
		twOadNoPerSector = SECTOR_SIZE/Len;
		if( RecordDepth < twOadNoPerSector )
		{
			twOadNoPerSector = RecordDepth;
		}	
		// 此OAD占扇区个数
		if( (RecordDepth%twOadNoPerSector) == 0 )
		{
			twSectorNo = (RecordDepth/twOadNoPerSector)+1;
		}
		else
		{
			twSectorNo = ((RecordDepth/twOadNoPerSector)+1)+1;	// 进位加1
		}
		// 扇区的偏移
		twSectorOff = inRecordNo/twOadNoPerSector;
		twSectorOff %= twSectorNo;
		// 扇区内的偏移
		twInSectorOff = inRecordNo%twOadNoPerSector;

		pEventSectorInfo->dwPointer = inRecordNo % RecordDepth;
		pEventSectorInfo->dwSaveSpace = SECTOR_SIZE*twSectorNo;
		pEventSectorInfo->dwSectorAddr = (DWORD)twSectorOff*SECTOR_SIZE+twInSectorOff*Len;	
		
		return TRUE;
	}
}

//-----------------------------------------------
//函数功能: 清零数据
//
//参数: 
// inEventIndex[in]:	eEVENT_INDEX    
//                
//返回值:无
//			
//备注: 清数据对应的Info
//-----------------------------------------------
static void ClearEventData( BYTE inEventIndex )
{	
	BYTE i,tSubEventIndex;
	DWORD dwAddr1;
	BYTE Buf[sizeof(TEventDataInfo)];

	memset(Buf,0x00,sizeof(TEventDataInfo));

	for( i = 0; i < sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab); i++ )
	{
		if( inEventIndex == SubEventInfoTab[i].EventIndex )
		{
			dwAddr1 = GET_SAFE_SPACE_ADDR( EventSafeRom.EventDataInfo[i] );
			api_VWriteSafeMem( dwAddr1, sizeof(TEventDataInfo), Buf );
			// 事件既然找到了就都在这里处理，不必要再搞一次循环
			if( inEventIndex == ePRG_RECORD_NO )
			{
				// 清EE临时保存编程记录里的OAD指针信息
				dwAddr1 = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );			
				api_ClrContinueEEPRom( dwAddr1, sizeof(TPrgProgramSubRom) );
				// 清零后重新记录编程记录
				ClearPrgRecordRam();
			}
			else if(inEventIndex < eNUM_OF_EVENT)// 清相关事件的掉电数据// 需要清除掉电标志中的RAM数据
			{
				// 当前状态
				tSubEventIndex = SubEventInfoTab[i].SubEventIndex;
				EventStatus.CurrentStatus[tSubEventIndex/8] &= (BYTE)~(0x01<<(tSubEventIndex%8));

				// Timer
				SubEventTimer[tSubEventIndex] = 0;

				// 延时后的状态
				api_DealEventStatus(eCLEAR_EVENT_STATUS, tSubEventIndex);
				
				//此处不能添加break
			}
			
			#if( SEL_PRG_INFO_Q_COMBO == YES )// 无功组合方式编程记录
			if(inEventIndex == ePRG_Q_COMBO_NO )
			{
				api_ClrContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.dwPrgQComBo2Num ),(sizeof(DWORD)+(sizeof(TRealTimer)*2)) );
			}	
			#endif			
		}
	}

}

//-----------------------------------------------
//函数功能: 读事件关联对象属性
//参数:
//		OI[in]:	事件OI
//  	No[in]:	第几个OAD，0代表全部OAD
//  	Len[in]:规约层传来数据长度，超过此长度返回FALSE
//  	pBuf[out]:返回数据的指针 按照冻结周期，OAD，存储深度顺序返回，每个DWORD，总共3*4字节
//
//返回值:
//		值为0x8000 ：代表出现错误（ OI不支持，读取Epprom错误等问题）
//		值为0x0000 ：代表buf长度不够
//		其他:		正常数据长度( 在个数为0时，添加1字节的0 返回长度为1)
//备注: 此处不要使用EventAddrLen.SubEventIndex,有些事件获取的可能不对!!!!!!
//-----------------------------------------------
WORD api_ReadEventAttribute( WORD OI, BYTE No, WORD Len, BYTE *pBuf )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	WORD wLenTemp;
	BYTE i;
	
	if( GetEventIndex( OI, &i ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.EventIndex = i;
	EventAddrLen.Phase = 0;
	//由于Phase可能不为0，此处不要使用EventAddrLen.SubEventIndex,有些事件获取的可能不对!!!!!!
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	if( No == 0 )
	{
		wLenTemp = 0;
		for( i = 0; i < EventAttInfo.NumofOad; i++ )
		{
			wLenTemp += sizeof(DWORD);
			if( wLenTemp > Len )
			{
				return FALSE;
			}
			memcpy( pBuf+wLenTemp-sizeof(DWORD), (BYTE *)&EventAttInfo.Oad[i], sizeof(DWORD) );
		}

		if( wLenTemp == 0 )
		{
			pBuf[0] = 0;
			return 1;
		}
		else
		{
			return wLenTemp;			
		}
	}
	else
	{
		if( No > EventAttInfo.NumofOad )
		{
			pBuf[0] = 0;
			return 1;
		}
		if( sizeof(DWORD) > Len )
		{
			return FALSE;
		}
		memcpy( pBuf, (BYTE *)&EventAttInfo.Oad[No-1], sizeof(DWORD) );

		return sizeof(DWORD);
	}
}

//-----------------------------------------------
//函数功能: 添加事件关联对象属性
//
//参数:
//		OI[in]: 	事件OI
//		pOad[in]:	对象属性设置时的OAD
//
//返回值:	TRUE/FALSE
//备注:   添加一个OAD，不删除数据，超出范围返回错误
//此处不要使用EventAddrLen.SubEventIndex,有些事件获取的可能不对!!!!!!
//-----------------------------------------------
BOOL api_AddEventAttribute( WORD OI, BYTE *pOad )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOad;
	WORD Len,RecordDepth;
	BYTE i,tEventIndex;
	TEventSectorInfo tEventSectorInfo;
	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = 0;
	//由于Phase可能不为0，此处不要使用EventAddrLen.SubEventIndex,有些事件获取的可能不对!!!!!!
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	if( (EventAttInfo.NumofOad+1) > MAX_EVENT_OAD_NUM )
	{
		return FALSE;	
	}
	memcpy( (BYTE *)&EventAttInfo.Oad[EventAttInfo.NumofOad], pOad, sizeof(DWORD) );

    for( i=0; i<EventAttInfo.NumofOad; i++ )//不允许设置相同的OAD
    {
        if( EventAttInfo.Oad[EventAttInfo.NumofOad] == EventAttInfo.Oad[i] )
        {
            return FALSE;
        }
    }

	dwOad = EventAttInfo.Oad[EventAttInfo.NumofOad];
	dwOad &= ~0x00E00000;
	// 不允许关联全部时段表,不允许关联全部节假日编程
	if( (dwOad==0x00021640) || (dwOad==0x00021740)  || (dwOad==0x00021140) )
	{
		return FALSE;
	}
	else
	{
		Len = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 ); 
		if( Len < 0xff )
		{
			EventAttInfo.OadLen[EventAttInfo.NumofOad] = Len;
		}
		else
		{
			return FALSE;
		}
		EventAttInfo.OadValidDataLen += EventAttInfo.OadLen[EventAttInfo.NumofOad];
	}
	EventAttInfo.NumofOad += 1;
	
	EventAttInfo.AllDataLen = (EventAttInfo.OadValidDataLen+sizeof(TEventOADCommonData)+EventInfoTab[EventAddrLen.EventIndex].ExtDataLen);
	//防越界
	if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
	{
		return FALSE;
	}
	//掉电和广播校时事件的深度为100，其他事件为10
	if( (tEventIndex == eEVENT_LOST_POWER_NO ) 
	|| (tEventIndex == ePRG_BROADJUST_TIME_NO ))
	{
		RecordDepth = EVENT_LOSTPOWERRECORD_NUM;
	}
	else
	{
		RecordDepth = MAX_EVENTRECORD_NUMBER;
	}
	//获取本事件根据关联对象属性表所用存储空间
	if(GetEventSectorInfo(EventAttInfo.AllDataLen, RecordDepth, RecordDepth, &tEventSectorInfo) != FALSE)
	{
		//本事件的所需的存储空间大于每个事件分配扇区，返回false，不允许设置
		if( tEventSectorInfo.dwSaveSpace > EVENT_RECORD_LEN )
		{
			return FALSE;
		}
		else
		{
			EventAttInfo.MaxRecordNo = RecordDepth;//赋值最大记录数
		}
	}
	else
	{
		return FALSE;
	}
	api_WriteToContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	// 数据清零
	ClearEventData( tEventIndex );
	//清对应的新增事件列表中的上报事件
	ClrReportAllChannelIndex( tEventIndex );
	return TRUE;
}

//-----------------------------------------------
//函数功能: 删除事件关联对象属性
//
//参数:
//		OI[in]:		事件OI
//		pOad[in]:	要删除的OAD
//返回值:	TRUE/FALSE
//备注:   删掉一个OAD，根据EVENT_DEL_DATA_FUNC配置看是否删除数据
//此处不要使用EventAddrLen.SubEventIndex,有些事件获取的可能不对!!!!!!
//-----------------------------------------------
BOOL api_DeleteEventAttribute( WORD OI, BYTE *pOad )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	BYTE i,j,tEventIndex;
	DWORD dwOad;
	WORD RecordDepth;
	TEventSectorInfo tEventSectorInfo;
	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}

	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = 0;
	//由于Phase可能不为0，此处不要使用EventAddrLen.SubEventIndex,有些事件获取的可能不对!!!!!!
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD) );
	for( i = 0; i < EventAttInfo.NumofOad; i++ )
	{
		if( dwOad == EventAttInfo.Oad[i] )
		{
			//删除OAD同时删除保存数据		
			for( j = i; j < (EventAttInfo.NumofOad-1); j++ )
			{
				EventAttInfo.Oad[j] = EventAttInfo.Oad[j+1];
				EventAttInfo.OadLen[j] = EventAttInfo.OadLen[j+1];
			}
			EventAttInfo.Oad[EventAttInfo.NumofOad-1] = 0;
			EventAttInfo.OadLen[EventAttInfo.NumofOad-1] = 0;			
			break;
		}
	}

	if( i == EventAttInfo.NumofOad )
	{
		return FALSE;
	}
		
	EventAttInfo.NumofOad -= 1;
	EventAttInfo.OadValidDataLen = 0;

	for( i = 0; i < EventAttInfo.NumofOad; i++ )
	{
		dwOad = EventAttInfo.Oad[i];
		EventAttInfo.OadValidDataLen += api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
	}
	
	EventAttInfo.AllDataLen = (EventAttInfo.OadValidDataLen+sizeof(TEventOADCommonData)+EventInfoTab[EventAddrLen.EventIndex].ExtDataLen);

	//防越界
	if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
	{
		return FALSE;
	}
	
	//掉电和广播校时事件的深度为100，其他事件为10
	if( (tEventIndex == eEVENT_LOST_POWER_NO ) 
	|| (tEventIndex == ePRG_BROADJUST_TIME_NO ))
	{
		RecordDepth = EVENT_LOSTPOWERRECORD_NUM;
	}
	else
	{
		RecordDepth = MAX_EVENTRECORD_NUMBER;
	}
	//获取本事件根据关联对象属性表所用存储空间
	if(GetEventSectorInfo(EventAttInfo.AllDataLen, RecordDepth, RecordDepth, &tEventSectorInfo) != FALSE)
	{
		//本事件的所需的存储空间大于每个事件分配扇区，返回false，不允许设置
		if( tEventSectorInfo.dwSaveSpace > EVENT_RECORD_LEN )
		{
			return FALSE;
		}
		else
		{
			EventAttInfo.MaxRecordNo = RecordDepth;//赋值最大记录数
		}
	}
	else
	{
		return FALSE;
	}
	
	api_WriteToContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	// 数据清零
	ClearEventData( tEventIndex );
	//清对应的新增事件列表中的上报事件
	ClrReportAllChannelIndex( tEventIndex );
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 设置事件关联对象属性
//
//参数:
//		OI[in]:	事件OI
//  	OadType[in]:
//				0 ~ 代表所有的OAD
//				N ~ 属性列表里的第N个OAD  设置第N个OAD时，认为第N个OAD之前是有值的
//				0xFF 初始化
//  	pOad[in]:  指向OAD Buffer的指针  2字节冻结周期+4字节OAD+2字节存储深度
//  	OadNum[in]:	OAD的个数
//返回值:	TRUE   正确
//			FALSE  错误
//备注:	所有的重新计，重新分配EEP空间  只支持设置已有的OAD,相当于修改指定OAD
//此处不要使用EventAddrLen.SubEventIndex,有些事件获取的可能不对!!!!!!
//-----------------------------------------------
BOOL api_WriteEventAttribute( WORD OI, BYTE OadType, BYTE *pOad, WORD OadNum )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOad;
	WORD Len,RecordDepth;
	BYTE i,j,n,tEventIndex;
	TEventSectorInfo tEventSectorInfo;
	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = 0;
	//由于Phase可能不为0，此处不要使用EventAddrLen.SubEventIndex,有些事件获取的可能不对!!!!!!
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	if( OadNum>MAX_EVENT_OAD_NUM )//OadNum =0情况下 勿轻易使用pOad -jwh
	{
		return FALSE;
	}

	if( OadType > MAX_EVENT_OAD_NUM )
	{
		if( OadType != 0xFF )
		{
			return FALSE;
		}
	}

	//api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);//读取事件关联信息
    
	if( (OadType==0) || (OadType==0xFF) )
	{
        memset( (BYTE *)&EventAttInfo, 0x00, sizeof(TEventAttInfo) );//将事件关联信息清零
    	
		for( i = 0; i < OadNum; i++ )
		{
			if( i == 0 )
			{
				if( OadType == 0xFF )
				{
					//第一个数代表oad个数
					memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD));
					if( dwOad == 0 )
					{
						break;	
					}					
					continue;
				}
			}
			
			if( OadType == 0xFF )
			{
				j = i-1;
				lib_ExchangeData( (BYTE *)&EventAttInfo.Oad[j], pOad+i*sizeof(DWORD), sizeof(DWORD));
			}
			else
			{
				j = i;
				memcpy( (BYTE *)&EventAttInfo.Oad[j], pOad+i*sizeof(DWORD), sizeof(DWORD));
			}
			
			for( n=0; n<j; n++ )//不允许设置相同的OAD
			{
				if( EventAttInfo.Oad[j] == EventAttInfo.Oad[n] )
				{
					return FALSE;
				}
			}

			dwOad = EventAttInfo.Oad[j];
			dwOad &= ~0x00E00000;
			
			// 不允许关联全部时段表,不允许关联全部节假日编程
			if( (dwOad==0x00021640) || (dwOad==0x00021740)  || (dwOad==0x00021140) )
			{
				return FALSE;
			}
			else
			{
				Len = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
			}

			if( Len > 0xff )
			{
				ASSERT(0!=0,1);
				return FALSE;
			}
			
			EventAttInfo.OadLen[j] = Len;
						
			EventAttInfo.OadValidDataLen += EventAttInfo.OadLen[j];
			EventAttInfo.NumofOad++;
		}
		
		EventAttInfo.AllDataLen = (EventAttInfo.OadValidDataLen+sizeof(TEventOADCommonData)+EventInfoTab[EventAddrLen.EventIndex].ExtDataLen);

		//防越界
		if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
		|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
		{
			return FALSE;
		}
	
		//掉电和广播校时事件的深度为100，其他事件为10
		if( (tEventIndex == eEVENT_LOST_POWER_NO ) 
		|| (tEventIndex == ePRG_BROADJUST_TIME_NO ))
		{
			RecordDepth = EVENT_LOSTPOWERRECORD_NUM;
		}
		else
		{
			RecordDepth = MAX_EVENTRECORD_NUMBER;
		}
		//获取本事件根据关联对象属性表所用存储空间
		if(GetEventSectorInfo(EventAttInfo.AllDataLen, RecordDepth, RecordDepth, &tEventSectorInfo) != FALSE)
		{
			//本事件的所需的存储空间大于每个事件分配扇区，返回false，不允许设置
			if( tEventSectorInfo.dwSaveSpace > EVENT_RECORD_LEN )
			{
				return FALSE;
			}
			else
			{
				EventAttInfo.MaxRecordNo = RecordDepth;//赋值最大记录数
			}
		}
		else
		{
			return FALSE;
		}
		
		api_WriteToContinueEEPRom( EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo );
	}
	else
	{
		return FALSE;
	}

	// 数据清零
	ClearEventData( tEventIndex );
	//清对应的新增事件列表中的上报事件
	ClrReportAllChannelIndex( tEventIndex );
	
	return TRUE;
}


//-----------------------------------------------
//函数功能: 获取掉电时间
//
//参数: 
//	PowerDownTime[out]:	掉电时间 
//  
//返回值:				无  
//备注:
//-----------------------------------------------
void api_GetPowerDownTime(TRealTimer *pPowerDownTime)
{
	DWORD		dwAddr;
	TLostPowerTime LostPowerTime;

	dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.LostPowerTime );
	api_VReadSafeMem( dwAddr, sizeof(TLostPowerTime) , LostPowerTime.Time );	
	memcpy( (BYTE*)pPowerDownTime, LostPowerTime.Time, sizeof(TRealTimer));
}

//-----------------------------------------------
//函数功能: 写事件数据区,带地址校验
//
//参数: 
//			Addr[in]		要写的起始地址
//			Length[in]		要写的长度
//			BaseAddr[in]	冻结或事件分配的基地址
//			MemSpace[in]	冻结或事件所分配的空间
//			pBuf[in/out]	写操作时的缓冲
//                    
//返回值:  	TRUE--操作成功 FALSE--操作失败
//
//备注:此函数只写Flash事件数据区
//-----------------------------------------------
BOOL WriteEventRecordData(DWORD Addr, WORD Length, DWORD BaseAddr, DWORD MemSpace, BYTE * pBuf)
{
	if((Addr >= BaseAddr) && ((Addr+Length) <= (BaseAddr+MemSpace)))
	{
		return api_WriteMemRecordData( Addr, Length, pBuf );
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
//函数功能: 保存工况类事件记录（失压失流等）
//
//参数: 
//	inEventIndex[in]:	eEVENT_INDEX 
//	Phase[in]:			ePHASE_TYPE  
//	BeforeAfter[in]:	事件发生前/发生后 eEVENT_TIME_TYPE 
//	EndTimeType[in]:	eEVENT_ENDTIME_CURRENT  当前时间
// 						eEVENT_ENDTIME_PWRDOWN  掉电时间
//  
//返回值:				TRUE/FALSE  
//备注:
//-----------------------------------------------
BOOL SaveInstantEventRecord( BYTE inEventIndex, BYTE Phase, BYTE BeforeAfter, BYTE EndTimeType )
{
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;	
	BYTE i,Temp,Sec,OADIndex,OADAttribute;
	WORD Len;
	BYTE TmpBuf[EVENT_VALID_OADLEN+30];
	TRealTimer	Time;
	WORD Offset,OI;
	DWORD dwOad,dwTemp,dwTemp1,dwAddr,dwMin,dwBaseAddr;	
	BYTE Buf[16],Buf1[16];	//大于TDemandData长度,不小于16
	TEventSectorInfo EventSectorInfo;
	
	EventAddrLen.EventIndex = inEventIndex;
	EventAddrLen.Phase = Phase;
	
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	if(api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo) == FALSE)
	{
		return FALSE;
	}

	if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.OadValidDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
	{
		return FALSE;
	}

	Offset = 0;
	//基地址等于此OAD的基地址;
	dwBaseAddr =  EventAddrLen.dwRecordAddr;
	if( BeforeAfter == EVENT_START )
	{
		//获取本条事件的偏移地址
		GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,EventDataInfo.RecordNo, &EventSectorInfo);
		
		memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) );

		EventDataInfo.RecordNo++;// 一直增加
		api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		
		EventOADCommonData.EventNo = EventDataInfo.RecordNo;
		if( EndTimeType == eEVENT_ENDTIME_CURRENT )
		{
			// 以当前时间作为时间发生时间
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.BeginTime );			
		}
		else
		{
		    api_GetPowerDownTime( (TRealTimer*)&EventOADCommonData.EventTime.BeginTime );
			// 以掉电时间作为时间发生时间
			#if( SEL_EVENT_LOST_ALL_V == YES )
			if( inEventIndex == eEVENT_LOST_ALL_V_NO )//全失压事件
			{
			    //掉电全失压开始时间，默认时间+1分钟，使用相对分钟数进行处理，避免分钟超限问题
			    dwMin = api_CalcInTimeRelativeMin( (TRealTimer*)&EventOADCommonData.EventTime.BeginTime );
			    dwMin += 1;
			    Sec = EventOADCommonData.EventTime.BeginTime.Sec;
			    api_ToAbsTime(dwMin, (TRealTimer*)&EventOADCommonData.EventTime.BeginTime);
			    EventOADCommonData.EventTime.BeginTime.Sec = Sec;
			}	
			#endif
		}
		// 必须先存时间,记录序号,上报状态，再存数据，否则在指针为0时会清整个扇区
		dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
		if(WriteEventRecordData( dwAddr, sizeof(TEventOADCommonData), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData ) == FALSE)
		{
		}
		// 存开始OAD对应的数据
		if( EventAttInfo.NumofOad != 0 )
		{
			//先全置FF，这样结束数据存为FF，事件结束时再保存结束数据
			memset( TmpBuf, 0xff, EventAttInfo.OadValidDataLen );

			for( i = 0; i < EventAttInfo.NumofOad; i++ )
			{
				Temp = (BYTE)((EventAttInfo.Oad[i]&0x00E00000)>>21);
				dwOad = EventAttInfo.Oad[i];
				dwOad &= ~0x00E00000;

				//OAD属性特征bit5~7:1：事件发生前 2：事件发生后 3：事件结束前 4：事件结束后
				if( (Temp != 3) && (Temp != 4) )//如果属性特征不为3,4，默认按照1,2,进行处理！！！！！！
				{
					#if( SEL_EVENT_LOST_ALL_V == YES )
					if( inEventIndex == eEVENT_LOST_ALL_V_NO )
					{
						OI = (WORD)(dwOad&0xFFFF);
						lib_InverseData((BYTE *)&OI, sizeof(WORD));

						if(OI == 0x2001)
						{
							OADAttribute = ((dwOad >> 16) & 0x1f);
							OADIndex = (dwOad >> 24);
							if(OADAttribute == 2)
							{
								if(OADIndex == 0)
								{
									memcpy( TmpBuf+Offset, (BYTE *)&LostAllVData[0].Current[0], EventAttInfo.OadLen[i] );	
								}
								else if(OADIndex < 4)
								{
									memcpy( TmpBuf+Offset, (BYTE *)&LostAllVData[0].Current[OADIndex - 1], EventAttInfo.OadLen[i] );
								}
								else
								{
								}
							}
							else if(OADAttribute == 4)
							{
								memcpy( TmpBuf+Offset, (BYTE *)&LostAllVData[0].Current[3], EventAttInfo.OadLen[i] );	
							}
							else
							{
								api_GetProOadData( eGetRecordData,eData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, TmpBuf+Offset );// 其他
							}
							
						}
						else if( (OI>=0x2000) && (OI<=0x200A) )// 除电流外的其他瞬时量
						{
							memset( TmpBuf+Offset, 0x00, EventAttInfo.OadLen[i] );		
						}
						else
						{
							api_GetProOadData( eGetRecordData,eData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, TmpBuf+Offset );		// 其他
						}
					}
					else
					#endif
					{
						api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, TmpBuf+Offset );	
					}
				}
				else
				{
                    #if( MAX_PHASE != 1 )
					OI = (WORD)(dwOad&0xFFFF);
					lib_InverseData((BYTE *)&OI, sizeof(WORD));
					if( OI == EVENT_AHOUR_OI )
					{
						if( ((inEventIndex-eEVENT_LOST_V_NO)<4) && ((Phase-ePHASE_A)<3) )//(Phase-ePHASE_A)会出现0-1的情况，出现此情况不会进行处理，无异常
						{	
							// 事件发生时存安时值的初始值
							api_GetProOadData( eGetRecordData,eData, 0xff, (BYTE *)&dwOad, NULL, sizeof(Buf), Buf );		
							api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.AHour[inEventIndex-eEVENT_LOST_V_NO][Phase-ePHASE_A][0] ),
													sizeof(DWORD)*4, Buf );	
						}
					}
                    #endif
				}
				Offset += EventAttInfo.OadLen[i];
			}
			
			dwAddr = EventAddrLen.dwRecordAddr+sizeof(TEventOADCommonData)+EventSectorInfo.dwSectorAddr;
			if(WriteEventRecordData( dwAddr, EventAttInfo.OadValidDataLen, dwBaseAddr, EVENT_RECORD_LEN, TmpBuf ) == FALSE)// 存开始OAD对应的数据
			{
			
			}
		}	
		
		SetReportIndex( EventAddrLen.SubEventIndex, EVENT_START );					
	}
	else//if( BeforeAfter == EVENT_START )
	{						
		if(EventDataInfo.RecordNo == 0)
		{
			return FALSE;
		}
		
		//获取本条事件的偏移地址
		GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,(EventDataInfo.RecordNo-1), &EventSectorInfo);

		// 存结束OAD对应的数据
		if( EventAttInfo.NumofOad != 0 )
		{

			memset( TmpBuf, 0xff, EventAttInfo.OadValidDataLen );
			
			dwAddr = EventAddrLen.dwRecordAddr+sizeof(TEventOADCommonData)+EventSectorInfo.dwSectorAddr;
			api_ReadMemRecordData( dwAddr, EventAttInfo.OadValidDataLen, TmpBuf );

			for( i = 0; i < EventAttInfo.NumofOad; i++ )
			{
				Temp = (BYTE)((EventAttInfo.Oad[i]&0x00E00000)>>21);
				dwOad = EventAttInfo.Oad[i];
				dwOad &= ~0x00E00000; 

				if( (Temp==3) || (Temp==4) )
				{                    
					if( (dwOad&0x000fffff) == AHOUR_OAD )
					{
						#if( MAX_PHASE != 1 )
						// 得到当前安时值
						api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, sizeof(Buf), Buf );
						if( ((inEventIndex-eEVENT_LOST_V_NO)<4) && ((Phase-ePHASE_A)<3) )
						{
							api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.AHour[inEventIndex-eEVENT_LOST_V_NO][Phase-ePHASE_A][0] ),
								sizeof(DWORD)*4, Buf1 );
						}
						else
						{
							memset( Buf1, 0x00, sizeof(DWORD)*4);
						}
						

						for( Temp=0; Temp<(EventAttInfo.OadLen[i]/sizeof(DWORD)); Temp++ )
						{							
                            memcpy( (BYTE *)&dwTemp1, Buf+Temp*sizeof(DWORD), sizeof(DWORD) );                //当前安时值
							memcpy( (BYTE *)&dwTemp, Buf1+Temp*sizeof(DWORD), sizeof(DWORD) );				//当前安时值
							
							if( dwTemp1 >= dwTemp )
							{
								dwTemp1 = dwTemp1-dwTemp;	
							}
							else
							{
								dwTemp1 = 0;
							}

							memcpy( TmpBuf+Offset+Temp*sizeof(DWORD), (BYTE *)&dwTemp1, sizeof(DWORD) );	// 存期间安时值
						}
						#endif
					}                    
					#if( SEL_EVENT_V_UNBALANCE == YES )
					else if( dwOad == V_UNBALANCE_OAD )
					{
						if( EndTimeType == eEVENT_ENDTIME_CURRENT )// 当前时间
						{
							// 读取RAM里的电压不平衡率
							OI = GetUnBalanceMaxRate(0x00);
						}
						else
						{
							// 读取EEP里的电压不平衡率
							OI = GetUnBalanceMaxRate(0x80);
						}
						memcpy( TmpBuf+Offset, (BYTE *)&OI, sizeof(WORD) );
					}
					#endif
					#if( (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
					else if( dwOad == I_UNBALANCE_OAD )
					{
						// 电流严重不平衡
						if( EventAddrLen.EventIndex == eEVENT_I_S_UNBALANCE_NO )	
						{
							if( EndTimeType == eEVENT_ENDTIME_CURRENT )// 当前时间
							{
								// 读取RAM里的电流不平衡率
								OI = GetUnBalanceMaxRate(0x02);
							}
							else
							{
								// 读取EEP里的电流不平衡率
								OI = GetUnBalanceMaxRate(0x82);
							}
						}
						else// 电流不平衡
						{
							if( EndTimeType == eEVENT_ENDTIME_CURRENT )// 当前时间
							{
								// 读取RAM里的电流严重不平衡率
								OI = GetUnBalanceMaxRate(0x01);
							}
							else
							{
								// 读取EEP里的电流严重不平衡率
								OI = GetUnBalanceMaxRate(0x81);
							}
						}
						memcpy( TmpBuf+Offset, (BYTE *)&OI, sizeof(WORD) );
					}
					#endif
					else
					{
						api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, TmpBuf+Offset );	
					}					
				}
				Offset += EventAttInfo.OadLen[i];
			}

			if(WriteEventRecordData( dwAddr, EventAttInfo.OadValidDataLen, dwBaseAddr, EVENT_RECORD_LEN, TmpBuf ) == FALSE)
			{
			
			}
		}
		
		if( EndTimeType == eEVENT_ENDTIME_CURRENT )
		{
			// 当前时间
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.EndTime );									
		}
		else
		{
			// 掉电时间
			api_GetPowerDownTime( (TRealTimer*)&EventOADCommonData.EventTime.EndTime );
			#if( SEL_EVENT_LOST_ALL_V == YES )
			if( inEventIndex == eEVENT_LOST_ALL_V_NO )//全失压事件
			{
			    //掉电全失压开始时间，默认时间+1分钟，使用相对分钟数进行处理，避免分钟超限问题
			    dwMin = api_CalcInTimeRelativeMin( (TRealTimer*)&EventOADCommonData.EventTime.EndTime );
			    dwMin += 1;
			    Sec = EventOADCommonData.EventTime.EndTime.Sec;
			    api_ToAbsTime(dwMin, (TRealTimer*)&EventOADCommonData.EventTime.EndTime);
			    EventOADCommonData.EventTime.EndTime.Sec = Sec;
			}	
			#endif
		}

		// 存结束时间
		dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
		dwAddr += GET_STRUCT_ADDR( TEventOADCommonData, EventTime.EndTime );
		if(WriteEventRecordData( dwAddr, sizeof(TRealTimer), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData.EventTime.EndTime ) == FALSE)
		{

		}
		dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
		dwAddr += GET_STRUCT_ADDR( TEventOADCommonData, EventTime.BeginTime );
		api_ReadMemRecordData( dwAddr, sizeof(TRealTimer) , (BYTE *)&Time );

		// 计算累计时间
		// 结束时间正确
		if( api_CheckClock((TRealTimer *)&EventOADCommonData.EventTime.EndTime) == TRUE )	
		{
			// 开始时间正确
			if( api_CheckClock(&Time) == TRUE )					
			{
				// 结束时间对应相对秒数
				dwTemp = api_CalcInTimeRelativeSec( (TRealTimer *)&EventOADCommonData.EventTime.EndTime );			
				// 发生时刻对应相对秒数
				dwAddr = api_CalcInTimeRelativeSec( &Time );							

				if( dwTemp >= dwAddr )
				{
					dwTemp = dwTemp - dwAddr;
				}
				else
				{
					dwTemp = 0;
				}
				EventDataInfo.AccTime += dwTemp;
				api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);	
			}
		}

		#if( SEL_DEMAND_OVER == YES )
		if( 	(inEventIndex==eEVENT_PA_DEMAND_OVER_NO) 
			|| 	(inEventIndex==eEVENT_NA_DEMAND_OVER_NO) 
			|| 	(inEventIndex==eEVENT_RE_DEMAND_OVER_NO) )
		{
			// 最大需量+发生时间
			if( inEventIndex == eEVENT_PA_DEMAND_OVER_NO )
			{
				dwAddr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue);
			}
			else if( inEventIndex == eEVENT_NA_DEMAND_OVER_NO )
			{
				dwAddr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue);
			}
			else// if( inEventIndex == eEVENT_RE_DEMAND_OVER_NO )
			{
				dwAddr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[Phase]);
			}

			// 读出最大需量+发生时间
			api_ReadFromContinueEEPRom( dwAddr, sizeof(TDemandData) , Buf );

			dwAddr = EventAddrLen.dwRecordAddr+sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;
			//不能按照GET_STRUCT_ADDR(TDemandOverRom,Data)方式，存的Data不一定在此空间
			//dwAddr += GET_STRUCT_ADDR(TDemandOverRom,Data);
			dwAddr += EventAttInfo.OadValidDataLen;
			if(WriteEventRecordData( dwAddr, sizeof(TDemandData), dwBaseAddr, EVENT_RECORD_LEN, Buf ) == FALSE)
			{

			}
		}
		#endif
		
		SetReportIndex( EventAddrLen.SubEventIndex, EVENT_END );	
	}
	api_LostPowerReportFraming( EventAddrLen.SubEventIndex ,EventDataInfo.RecordNo );
	return TRUE;
}

//-----------------------------------------------
//函数功能: 根据OAD读数据
//
//参数: 
//		pDLT698RecordPara[in]：指向结构体TDLT698RecordPara的指针              
//		pEventOADCommonData[in]：指向结构体TEventOADCommonData的指针                
//		pEventAttInfo[in]：指向结构体TEventAttInfo的指针                
//		pEventAddrLen[in]：指向结构体TEventAddrLen的指针                
//		dwPointer[in]：地址偏移指针                
//		pBuf[out]：返回数据的指针，有的传数据，没有的补0                
//		dwOad[in]：OAD                
//		Len[in]：输入的buf长度 用来判断buf长度是否够用                
//		Tag[in]：0 不加Tag  1 加Tag                
//		off[in]：已经有的偏移                
//		status[in]：事件结束标识                
//		AllorSep[in]：1 指定OAD 0 所有OAD
// 		Last[in]: 上N次
//返回值:	0xFFFF: 缓冲满或者错误 其他值：返回数据长度             
//备注: 
//-----------------------------------------------
static WORD GetDataByOad( TEventData *pData )
{
	DWORD dwTemp,dwTemp1,dwTemp2,dwOad,dwTemp3,dwTemp4;
	WORD wLen,wLen1,wLenTemp,w,Result;
	BYTE Temp,i,OadIndex,tmpBuf[EVENT_ALONE_OADLEN+30];	
	BYTE *pTemp;
	BYTE Buf[30],Buf1[30];
	TPrgProgramSubRom	OadList;

	if( pData->dwOad == STARTTIME_OAD )
	{
		//若发生时间都为ff，则置为null
		if( lib_IsAllAssignNum( (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, 0xff, sizeof(TRealTimer) ) == TRUE )
		{
			if( (pData->off+1) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			wLen = 1;
			pData->pBuf[pData->off] = 0;
		}
		else
		{
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			
			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetNormalData, eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, 0xFF, pData->pBuf+pData->off );	
			}
			else
			{
				memcpy( pData->pBuf+pData->off, (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, wLen );		
			}
		}				
	}
	else if( pData->dwOad == ENDTIME_OAD )
	{
		//若结束时间都为ff，则置为null eNUM_OF_EVENT
		//校时记录，记录结束时间
		if( (( lib_IsAllAssignNum((BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, 0xff, sizeof(TRealTimer) ) == TRUE )
			||	( pData->pEventAddrLen->EventIndex>=ePRG_RECORD_NO ))
			#if( SEL_PRG_INFO_ADJUST_TIME == YES )
			&&( pData->pEventAddrLen->EventIndex!=ePRG_ADJUST_TIME_NO)
			#endif
			#if( SEL_PRG_INFO_BROADJUST_TIME == YES )
			&&( pData->pEventAddrLen->EventIndex!=ePRG_BROADJUST_TIME_NO)
			#endif
		 )
		{
			if( (pData->off+1) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			wLen = 1;
			pData->pBuf[pData->off] = 0;
		}
		else
		{
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			
			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetNormalData, eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, 0xFF, pData->pBuf+pData->off );	
			}
			else
			{
				memcpy( pData->pBuf+pData->off, (BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, wLen );		
			}
		}
	}
	else if( pData->dwOad == RECORDNO_OAD )
	{
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// 已满
		}

		if( pData->pEventOADCommonData->EventNo != 0 )
		{
			dwTemp = pData->pEventOADCommonData->EventNo - 1;
		}
		else
		{
			dwTemp = 0;
		}
		
		if( pData->Tag == eTagData )
		{
			api_GetProOadData( eGetNormalData, eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&dwTemp, 0xFF, pData->pBuf+pData->off );			
		}
		else
		{
			memcpy( pData->pBuf+pData->off, (BYTE *)&dwTemp, wLen );
		}
	}
	else if( pData->dwOad == GEN_SOURCE_OAD )
	{
		// 工况类事件无事件发生源
		if( ((pData->pDLT698RecordPara->OI >= 0x3000) && (pData->pDLT698RecordPara->OI<= 0x3008)) 
		|| (pData->pDLT698RecordPara->OI == 0x303B) )//接口类24  class_id=24
		{
			if( pData->AllorSep == 0 )
			{
				wLen = 0;
			}
			else
			{
				wLen = 1;
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// 已满
				}

				pData->pBuf[pData->off] = 0;	// 只传一个0，代表NULL
			}
		}
		else
		{
			//电表事件发生源只有3023	电能表无功组合方式编程事件 有效 
			//事件发生源∷=enum{无功组合方式1特征字（0），无功组合方式2特征字（1）}
			#if( SEL_PRG_INFO_Q_COMBO == YES )
			if( pData->pEventAddrLen->EventIndex == ePRG_Q_COMBO_NO )
			{
				wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// 已满
				}
				
				dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.QcombSource[pData->pEventSectorInfo->dwPointer] );
				api_ReadFromContinueEEPRom( dwTemp, 1, (BYTE *)&Temp );
				
				if( Temp > 1 )
				{
					Temp = 0;
				}
				if( pData->Tag == eTagData )
				{
					api_GetProOadData( eGetRecordData, eAddTag,0xff,(BYTE *)&pData->dwOad, (BYTE *)&Temp, 2, (pData->pBuf+pData->off) );
				}
				else
				{
					pData->pBuf[pData->off] = Temp;
				}
			}
			else
			#endif
			{
				wLen = 1;
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// 已满
				}

				pData->pBuf[pData->off] = 0;	// 只传一个0，代表NULL
			}
		}
	}
	else if( pData->dwOad == NOTIFICATION_OAD )
	{
		//补遗8要求电表可不支持通道上报，传NULL
		pData->pBuf[pData->off] = 0;
		wLen = 1;
	}
	#if( SEL_DEMAND_OVER == YES )
	else if( (pData->dwOad==MAXDEMAND_OAD) || (pData->dwOad==MAXDEMAND_TIME_OAD) )
	{
		//超限期间需量最大值/发生时间
		Temp = 0;
		if( pData->pDLT698RecordPara->OI == EVENT_PACTIVE_DEMANDOVER_OI )
		{
			Temp = 1;
			dwTemp1 = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue);
		}
		else if( pData->pDLT698RecordPara->OI == EVENT_NACTIVE_DEMANDOVER_OI )
		{
			Temp = 1;
			dwTemp1 = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue);
		}
		else if( pData->pDLT698RecordPara->OI == EVENT_REACTIVE_DEMANDOVER_OI )
		{
			Temp = 1;
			dwTemp1 = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[pData->pDLT698RecordPara->Phase]);
		}
		else
		{
			Temp = 0;
		}

		if( Temp == 1 )
		{
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}

			if( (pData->Last==1) && (pData->status==1) )
			{
				// 上1次并且未结束
				api_ReadFromContinueEEPRom( dwTemp1, sizeof(TDemandData), Buf );
			}
			else
			{
				dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData) + pData->pEventSectorInfo->dwSectorAddr;
				dwTemp += pData->pEventAttInfo->OadValidDataLen;
				api_ReadMemRecordData( dwTemp, sizeof(TDemandData), Buf );							
			}

			if( pData->Tag == eTagData )
			{
				if( pData->dwOad == MAXDEMAND_OAD )
				{
					api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&pData->dwOad, Buf, 0xFF, pData->pBuf+pData->off );
				}
				else
				{
					api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&pData->dwOad, Buf+sizeof(DWORD), 0xFF, pData->pBuf+pData->off );
				}
			}
			else
			{
				if( pData->dwOad == MAXDEMAND_OAD )
				{
					memcpy( pData->pBuf+pData->off, Buf, wLen );
				}
				else
				{
					memcpy( pData->pBuf+pData->off, Buf+sizeof(DWORD), wLen );
				}
			}		
		}
		else
		{
			wLen = 1;
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			pData->pBuf[pData->off] = 0;
		}
	}
	#endif
	else if( pData->dwOad == PRG_OBJ_LIST_OAD )
	{			
		//读上一条 且 编程记录还没有结束 时 
		if( (pData->Last==1) && (pData->status==1) )
		{
			//从ee暂存的数据中读取OAD列表  需要验证在按照时间读取时是否正确！！！！！！
			dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );
			api_ReadFromContinueEEPRom(dwTemp, sizeof(OadList), (BYTE *)&OadList);			
		}
		else
		{
			//从保存的数据区读取OAD列表
			dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr+ pData->pEventAttInfo->OadValidDataLen;
			api_ReadMemRecordData( dwTemp, sizeof(OadList), (BYTE *)&OadList );
		}
		
		if( OadList.SaveOadNum > 10 )
		{
			return 0xffff;
		}			
		
		pTemp = pData->pBuf+pData->off;
		if( pData->Tag == eTagData )
		{			
			wLen = OadList.SaveOadNum*5+2;
			pTemp[0] = 1;	//array
			pTemp[1] = OadList.SaveOadNum;
			for(i=0; i<OadList.SaveOadNum; i++)
			{
				pTemp[2+i*5] = OadList.Type[i];
				memcpy(pTemp+2+i*5+1, (BYTE*)&OadList.Oad[0]+i*sizeof(DWORD), sizeof(DWORD) );
			}
		}
		else
		{
			wLen = OadList.SaveOadNum*4;
			memcpy( pTemp, OadList.Oad, OadList.SaveOadNum*sizeof(DWORD) );
		}			
	}
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
	else if( pData->dwOad == CLR_EVENT_LIST_OAD )
	{
		//事件清零列表  array OMD
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 1 );

		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// 已满
		}
		
		wLenTemp = api_GetProOADLen( eGetNormalData, eData, (BYTE *)&pData->dwOad, 1 );// 4*N
		
		dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
		dwTemp +=pData->pEventAttInfo->OadValidDataLen;
		api_ReadMemRecordData( dwTemp, sizeof(DWORD), tmpBuf );
		
		if( pData->Tag == eTagData )
		{
			if( wLenTemp > (sizeof(tmpBuf) -1))//防止数组越界
			{
				return 0xFFFF;// 已满
			}
			memmove( (tmpBuf+1), tmpBuf, wLenTemp );
			*tmpBuf = 1;	// 第一个字节为array的个数,固定1个字节
			api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&pData->dwOad, tmpBuf, 0xFF, pData->pBuf+pData->off );
		}
		else
		{
			memcpy(pData->pBuf+pData->off,tmpBuf,wLen);
		}
	}
	#endif
	#if( (SEL_PRG_INFO_HOLIDAY == YES)  || (SEL_PRG_INFO_TIME_TABLE == YES))// 节假日编程//时段表编程
	else if( (pData->dwOad==PRG_HOLIDAY_OAD)  || (pData->dwOad==PRG_HOLIDAY_VALUE) ||(pData->dwOad==PRG_SEGTABLE_OAD)  || (pData->dwOad==PRG_SEGTABLE_VALUE))
	{
		if( (pData->dwOad==PRG_SEGTABLE_OAD) || (pData->dwOad==PRG_SEGTABLE_VALUE) )
		{
			dwOad = 0x01021640;
		}
		else
		{
			dwOad = 0x01021140;
		}
		Temp = api_GetProOADLen( eGetNormalData, eData, (BYTE *)&dwOad, 0 );	

		// 扩展数据一次性读出需要的缓冲长度
		wLen = Temp+sizeof(DWORD);
		
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// 已满
		}

	    // 读拓展数据
	    dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
		dwTemp +=pData->pEventAttInfo->OadValidDataLen;
		api_ReadMemRecordData(dwTemp, wLen, tmpBuf);
	
	    // 获取编程OAD
		memcpy((BYTE*)&dwTemp,&tmpBuf[wLen-sizeof(DWORD)],sizeof(DWORD));
		// 如果是获取OAD的话
		if( (pData->dwOad==PRG_SEGTABLE_OAD) || (pData->dwOad==PRG_HOLIDAY_OAD) )
		{
			wLen=((pData->Tag)?(1+sizeof(DWORD)):(sizeof(DWORD)));
		}
		else
		{
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwTemp, 0 );
			Temp=0;
		}
		// 检查缓冲是否够
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// 已满
		}
		// 设置数据
		if( pData->Tag == eTagData )
		{
			if( (pData->dwOad==PRG_SEGTABLE_OAD) || (pData->dwOad==PRG_HOLIDAY_OAD))// OAD加tag的话直接处理
			{
				tmpBuf[Temp-1]=0x51;// OAD
				memcpy(pData->pBuf+pData->off,&tmpBuf[Temp-1],wLen);
			}
			else
			{
				api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwTemp, tmpBuf, 0xFF, pData->pBuf+pData->off );// 使用修改存储的OAD对数据进行加tag
			}
		}
		else
		{
			memcpy(pData->pBuf+pData->off,&tmpBuf[Temp],wLen);
		}
	}
	#endif//#if( (SEL_PRG_INFO_HOLIDAY == YES)  || (SEL_PRG_INFO_TIME_TABLE == YES))// 节假日编程//时段表编程
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//{0x3027,9,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,CARD_SN_OAD,CARD_ERR_INFO_OAD,CARD_ORDER_HEAD_OAD,CARD_ERR_RES_OAD},//电能表异常插卡事件0x3027
	//{0x3029,6,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,RETURN_MONEY_OAD},	//电能表退费记录3029	
	else if( (pData->dwOad==CARD_SN_OAD) 
			 ||(pData->dwOad==CARD_ERR_INFO_OAD) 
			 ||(pData->dwOad==CARD_ORDER_HEAD_OAD) 
			 ||(pData->dwOad==CARD_ERR_RES_OAD)
			 ||(pData->dwOad==RETURN_MONEY_OAD) )
	{
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 1 );

		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// 已满
		}
		
		wLen1 = api_GetProOADLen( eGetNormalData, eData, (BYTE *)&pData->dwOad, 1 );// 4*N
		
		dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
		dwTemp +=pData->pEventAttInfo->OadValidDataLen;
		if( (pData->dwOad & 0xffff ) == 0x1033 )//如果是异常插卡，则把异常插卡保存的卡序列号、错误信息字、命令头、错误响应状态共18字节全读出来
		{
			wLen1 = 18;
		}		
		
		api_ReadMemRecordData( dwTemp, wLen1, tmpBuf );
         		
		if(pData->dwOad==CARD_ORDER_HEAD_OAD)
        {
        	Temp = 10;
        	if( tmpBuf[9] == 0x01 )
        	{
        		wLen = 8;
        		wLen1 = 6;//数据+TAG  --对于 Octet_string_698 ，会加2字节,例如如果串是6字节,则数据+TAG后是8字节，因为有 Tag Len or Type Len
        	}
        	else
        	{
        		wLen = 7;
        		wLen1 = 5;
        	}

        	if( pData->Tag == eTagData )
        	{
        		pData->pBuf[pData->off] = 9;//9 -- Octet_string_698; 如果用 Octet_string_698 编译不过
        		pData->pBuf[pData->off+1] = wLen1;
        		memcpy(pData->pBuf+pData->off+2,tmpBuf+Temp,wLen1);
        	}
        	else
        	{
        		memcpy(pData->pBuf+pData->off,tmpBuf+Temp,wLen1);
        	}		
        }
        else
        {
        	 Temp = 0;//暂用于保存读出缓冲pAllocBuf中各个数据的偏移,先默认为0
	        //电能表异常插卡事件 18=8+1+7+2 其中7：命令头，命令头保存时保存 CH,Cla,Ins,P1,P2,P31,P32,其中CH:0x01--ESAM 0x00--CARD,esam命令头为CH后的6字节，卡为CH后的5字节
	        if( pData->dwOad== CARD_ERR_INFO_OAD )
	        {
	        	Temp = 8;
	        } 
	        else if( pData->dwOad==CARD_ERR_RES_OAD )
	        {
	        	Temp = 16;
	        }	
        
			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&pData->dwOad, tmpBuf+Temp, 0xFF, pData->pBuf+pData->off );
			}
			else
			{
				memcpy(pData->pBuf+pData->off,tmpBuf+Temp,wLen1);
			}
		}

	}	
	#endif
	else
	{
		dwTemp1 = 0;
		OadIndex = 0;
		for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
		{
			if( pData->pEventAttInfo->Oad[i] == pData->dwOad )
			{
				break;
			}
			dwTemp1 += pData->pEventAttInfo->OadLen[i];
		}

		// 没找到指定OAD
		if( i == pData->pEventAttInfo->NumofOad )
		{
			dwOad = pData->dwOad;
			OadIndex = (BYTE)(dwOad>>24);

			if( OadIndex != 0 )
			{
				dwTemp1 = 0;
				dwOad &= ~0xFF000000;
				// 索引置为0再找一遍
				for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
				{
					if( pData->pEventAttInfo->Oad[i] == dwOad )
					{
						Temp = 2;
						break;
					}
					dwTemp1 += pData->pEventAttInfo->OadLen[i];
				}
			}
		}

		// 找到OAD
		if( i != pData->pEventAttInfo->NumofOad )
		{
			dwOad = pData->dwOad;
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}

			if( pData->pEventAttInfo->OadLen[i] > EVENT_ALONE_OADLEN )
			{
				return 0xFFFF;
			}

			w = pData->pEventAttInfo->OadLen[i];

			if( pData->pEventAddrLen->EventIndex <= ePRG_RECORD_NO )
			{
				Temp = (BYTE)((pData->dwOad&0x00E00000)>>21);
				
				if( (pData->Last==1) && (pData->status==1) && ((Temp==3)||(Temp==4)) )
				{
					#if( SEL_AHOUR_FUNC == YES )
					if( (pData->dwOad&0x000fffff) == AHOUR_OAD )//期间安时值特殊处理
					{
						if( ((pData->pEventAddrLen->EventIndex-eEVENT_LOST_V_NO)<4) && ((pData->pDLT698RecordPara->Phase-ePHASE_A)<3) )
						{
							// 得到当前安时值
							api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, sizeof(Buf), Buf );
							if( OadIndex != 0 )//如果元素索引不为零对数据进行一致性处理，保证换算数据正确
							{
								for( Temp = 0; Temp < 3; Temp++ )
								{
									memmove( (BYTE*)&Buf[4+Temp*sizeof(DWORD)], Buf, 4 );
								}
							}
							api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.AHour[pData->pEventAddrLen->EventIndex-eEVENT_LOST_V_NO][pData->pDLT698RecordPara->Phase-ePHASE_A][0] ),
														sizeof(DWORD)*4, Buf1 );

							for( Temp=0; Temp<(pData->pEventAttInfo->OadLen[i]/sizeof(DWORD)); Temp++ )
							{							
	                            memcpy( (BYTE *)&dwTemp3, Buf+Temp*sizeof(DWORD), sizeof(DWORD) );//当前安时值
								memcpy( (BYTE *)&dwTemp4, Buf1+Temp*sizeof(DWORD), sizeof(DWORD) );//当前安时值
								
								if( dwTemp3 >= dwTemp4 )
								{
									dwTemp3 = dwTemp3-dwTemp4;	
								}
								else
								{
									dwTemp3 = 0;
								}

								memcpy( tmpBuf+Temp*sizeof(DWORD), (BYTE *)&dwTemp3, sizeof(DWORD) );//计算安时值
							}

						}
						else//不支持期间冻结需量的，返回null
						{
							pData->pBuf[pData->off] = 0;//null
							return 1;
						}
					}
					else
					#endif
					{
						pData->pBuf[pData->off] = 0;//null
						return 1;
					}

				}
				else
				{
					dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
					dwTemp += dwTemp1;
					api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], tmpBuf);
				}
			}
			else
			{
				dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
				dwTemp += dwTemp1;
				api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], tmpBuf);
			}

			if( OadIndex != 0 )
			{
				pTemp = tmpBuf;
				dwTemp = dwOad;

				for( i = 1; i < OadIndex; i++ ) // pData->OadIndex 从1开始
				{
					dwTemp2 = i;
					dwOad = (dwTemp&0X00FFFFFF);//先去掉元素索引
					dwOad |= (dwTemp2<<24);//置元素索引
					wLen1 = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );//对冻结数据进行偏移使用eGetRecordData
					if( wLen1 == 0x8000 )
					{
						return 0x8000;
					}
					pTemp += wLen1;
				}

				dwOad = ((dwOad&0x00ffffff) | (OadIndex<<24));//置元素索引
			}
			else
			{
				pTemp = tmpBuf;
			}

			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, pTemp, 0xFF, pData->pBuf+pData->off );
			}
			else
			{
				//电能使用冻结长度传入，电能进行转化后 会将wLen转化为实际长度
				wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );
				//前面判断长度了 可以直接相减
				Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, pTemp,pData->Len-pData->off, pData->pBuf+pData->off );
				if( (Result == 0x8000) || (Result == 0) )
				{
					return Result;
				}
			}

		}
		else
		{
			wLen = 1;
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}
			pData->pBuf[pData->off] = 0;
		}
	}

	return wLen;
}

//-----------------------------------------------
//函数功能: 根据OAD读数据
//
//参数: 
//		pDLT698RecordPara[in]：		指向结构体TDLT698RecordPara的指针              
//		pEventOADCommonData[in]：	指向结构体TEventOADCommonData的指针                
//		pEventAttInfo[in]：			指向结构体TEventAttInfo的指针                
//		pEventAddrLen[in]：			指向结构体TEventAddrLen的指针                
//		dwPointer[in]：				地址偏移指针                
//		pBuf[out]：					返回数据的指针，有的传数据，没有的补0                
//		dwOad[in]：					OAD                
//		Len[in]：					输入的buf长度 用来判断buf长度是否够用                
//		Tag[in]：					0 不加Tag  1 加Tag                
//		off[in]：					已经有的偏移                
//		status[in]：				事件结束标识                
//		AllorSep[in]：				1 指定OAD 0 所有OAD
//返回值:							bit15位 置1 错误 置0正确； 
//									bit0~bit14 代表返回的数据长度             
//备注: 
//-----------------------------------------------
static WORD ReadEventRecordSub( TEventData *pData )
{
	DWORD dwOad,dwTemp,dwTemp1,dwTemp2;
	WORD wLen,Result;
	BYTE i,j,Number,Temp,Buf[30],Buf1[30],TmpBuf[EVENT_VALID_OADLEN+30];
	BYTE *pTemp;
	
	// 找OI对应固定列的OAD
	for( i = 0; i < (sizeof(EventOadTab)/sizeof(TEventOadTab)); i++ )
	{
		if( pData->pDLT698RecordPara->OI == EventOadTab[i].OI )
		{
			break;
		}
	}

	if( i < (sizeof(EventOadTab)/sizeof(TEventOadTab)) )
	{
		pTemp = (BYTE *)&EventOadTab[i].OadList[0];	// 找到
		Number = EventOadTab[i].Number;
	}
	else
	{
		// 默认Class id 7 工况类事件，操作类编程
		pTemp = (BYTE *)&EventOadTab[0].OadList[0];	
		Number = EventOadTab[0].Number;
	}

	pData->off = 0;

	if( pData->pDLT698RecordPara->OADNum != 0 )
	{
		pData->AllorSep = 1;
		// 指定OAD
		for( i = 0; i < pData->pDLT698RecordPara->OADNum; i++ )
		{		    
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
			pData->dwOad = dwOad;

			//判断此OAD是否为固定列OAD
			for( j = 0; j < sizeof(FixedColumnOadTab)/sizeof(DWORD); j++ )
			{
				if( FixedColumnOadTab[j] == dwOad )
				{
					break;
				}
			}

			if( j < sizeof(FixedColumnOadTab)/sizeof(DWORD) )
			{
				//判断此OAD是否为此事件记录支持的固定列OAD
				for( j = 0; j < Number; j++ )
				{
					memcpy( (BYTE *)&dwTemp, pTemp+sizeof(DWORD)*j, sizeof(DWORD) );
					if( dwTemp == dwOad )
					{
						break;
					}
				}

				if( j < Number )
				{
					// 是此OI对应的固定列
					wLen = GetDataByOad( pData );

					if( wLen == 0xFFFF )
					{
						return 0; 	// 缓冲满
					}
				}
				else
				{
					wLen = 1;
					if( (pData->off + wLen) > pData->Len )
					{
						return 0; 	// 缓冲满	
					}

					*(pData->pBuf+pData->off) = 0;// NULL 不支持的固定列
				}
			}
			else
			{
				// 找OAD列表
				wLen = GetDataByOad( pData );

				if( wLen == 0xFFFF )
				{
					return 0; 	// 缓冲满
				}
			}
			pData->off += wLen;
		}
	}
	else
	{
		// 固定列
		for( i = 0; i < Number; i++ )
		{
			memcpy( (BYTE *)&dwOad, pTemp+sizeof(DWORD)*i, sizeof(DWORD) );
			pData->dwOad = dwOad;
			
			wLen = GetDataByOad( pData );

			if( wLen == 0xFFFF )
			{
				return 0; 	// 缓冲满
			}

			pData->off += wLen;
		}

		// 一次将数据全部读出，传进来的数据地址已经考虑两个扇区问题
		dwTemp = pData->pEventAddrLen->dwRecordAddr + sizeof(TEventOADCommonData);
		dwTemp += pData->pEventSectorInfo->dwSectorAddr;
		
		// 关联对象属性列表
		if( pData->pEventAttInfo->NumofOad == 0 )
		{
			return pData->off;
		}

		if( (pData->pEventAttInfo->AllDataLen > EVENT_VALID_OADLEN) || (pData->pEventAttInfo->OadValidDataLen > EVENT_VALID_OADLEN) )
		{
			return 0x8000;
		}

		memset( TmpBuf, 0x00, pData->pEventAttInfo->OadValidDataLen );//此语句后面不能再调用GetDataByOad，原因是可能会导致TmpBuf数据被冲
		api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadValidDataLen, TmpBuf);
 
		dwTemp = 0;
		for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
		{		    
			dwOad = pData->pEventAttInfo->Oad[i];
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );// 虽然未结束时填null只需要一个字节，这里不修改此函数，唯一的影响是提前分帧
			if( (pData->off+wLen) > pData->Len )
			{
				return 0;// 已满
			}
			
			if( pData->pEventAddrLen->EventIndex <= ePRG_RECORD_NO )
			{
				Temp = (BYTE)((pData->pEventAttInfo->Oad[i]&0x00E00000)>>21);
				// 上1次 && 事件还未结束 && 结束时刻OAD   返回0
				if( (pData->Last==1) && (pData->status==1) && ((Temp==3)||(Temp==4)) )
				{
					#if( SEL_AHOUR_FUNC == YES )
					if( (dwOad&0x000fffff) == AHOUR_OAD )//期间安时值特殊处理
					{
						if( ((pData->pEventAddrLen->EventIndex-eEVENT_LOST_V_NO)<4) && ((pData->pDLT698RecordPara->Phase-ePHASE_A)<3) )
						{
							// 得到当前安时值
							api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, sizeof(Buf), Buf );
							api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.AHour[pData->pEventAddrLen->EventIndex-eEVENT_LOST_V_NO][pData->pDLT698RecordPara->Phase-ePHASE_A][0] ),
														sizeof(DWORD)*4, Buf1 );

							for( Temp=0; Temp<(pData->pEventAttInfo->OadLen[i]/sizeof(DWORD)); Temp++ )
							{							
	                            memcpy( (BYTE *)&dwTemp1, Buf+Temp*sizeof(DWORD), sizeof(DWORD) );//当前安时值
								memcpy( (BYTE *)&dwTemp2, Buf1+Temp*sizeof(DWORD), sizeof(DWORD) );//当前安时值
								
								if( dwTemp1 >= dwTemp2 )
								{
									dwTemp1 = dwTemp1-dwTemp2;	
								}
								else
								{
									dwTemp1 = 0;
								}
								//将从flash中读取的数据替换掉
								memcpy( TmpBuf+(dwTemp+Temp*sizeof(DWORD)), (BYTE *)&dwTemp1, sizeof(DWORD) );//计算安时值
							}

						}
						else//不支持期间冻结需量的，返回null
						{
							// 事件还没有结束,结束数据置null
							dwTemp+= pData->pEventAttInfo->OadLen[i];
							*(pData->pBuf+pData->off)=0;//NULL
							pData->off+=1;
							continue;
						}
					}
					else
					#endif
					{
						// 事件还没有结束,结束数据置null
						dwTemp+= pData->pEventAttInfo->OadLen[i];
						*(pData->pBuf+pData->off)=0;//NULL
						pData->off+=1;
						continue;
					}
				}
			}

			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, TmpBuf+dwTemp, 0xFF, pData->pBuf+pData->off );
			}
			else
			{
				//电能使用冻结长度传入，电能进行转化后 会将wLen转化为实际长度
				wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );
				//前面判断长度了 可以直接相减
				Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, pTemp,pData->Len -pData->off, pData->pBuf+pData->off );
				if( (Result == 0x8000) || (Result == 0) )
				{
					return Result;
				}
			}
			
			dwTemp += pData->pEventAttInfo->OadLen[i];// EEP数据偏移
			pData->off += wLen;
		}
	}

	return pData->off;
}

//-----------------------------------------------
//函数功能: 以时间方式读工况类事件记录
//
//参数: 
//		Tag[in]:				0 不加Tag 1 加Tag                
//		pDLT698RecordPara[in]: 	读取冻结的参数结构体                
//  	Len[in]：				输入的buf长度 用来判断buf长度是否够用                
//  	pBuf[out]: 				返回数据的指针，有的传数据，没有的补0
//返回值:						bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//								bit0~bit14 代表返回的冻结数据长度
//备注: 在已存的时间里搜索，只搜索时间相等的记录
//-----------------------------------------------
static WORD ReadEventRecordByTime( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventOADCommonData EventOADCommonDataBak;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	DWORD dwTemp,dwPointer,dwPointerBak;
	WORD twCurrentRecordNo;
	BYTE i,status;
	TRealTimer	Time;
	TRealTimer	EventTime,EventTimeBak;//以后优化为指针更好zym
	TEventSectorInfo EventSectorInfo;
	
    dwPointerBak = 0;
    dwPointer = 0;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//根据当前记录号为0，且当前状态为事件没发生
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}
	else if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
	{
		twCurrentRecordNo = EventAttInfo.MaxRecordNo;
	}
	else
	{
		twCurrentRecordNo = EventDataInfo.RecordNo;	
	}
	
	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	

	//事件要读取的时标单位为秒（相对秒数）
	api_ToAbsTime( pDLT698RecordPara->TimeOrLast/60, (TRealTimer *)&Time );
	Time.Sec = pDLT698RecordPara->TimeOrLast%60;
	// 在已有的记录里搜索
	for( i = 1; i <= twCurrentRecordNo; i++ )
	{
		dwPointer = EventDataInfo.RecordNo - i;

		//计算此条记录偏移地址
		GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, dwPointer, &EventSectorInfo);
		
		dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;//地址为基地址加偏移地址
		
		api_ReadMemRecordData(dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
		
		if( pDLT698RecordPara->eTimeOrLastFlag == eEVENT_START_TIME_FLAG )
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.BeginTime, sizeof(TRealTimer) );
		}
		else
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.EndTime, sizeof(TRealTimer) );
		}
		
		//第一次查找，先判断要读的时间与最近一次的大小关系，若比最近一次记录的时间大，直接返回错误
		if( i == 1 )
		{
			if( memcmp( &Time, (BYTE *)&EventTime, sizeof(TRealTimer) ) > 0 )
			{
				return 0x8000;// 没找到相应时间
			}
			//第一次查找，初始化EventTimeBak为EventTime
			memcpy( (BYTE *)&EventTimeBak, (BYTE *)&EventTime, sizeof(TRealTimer) );
		}
		
		if( memcmp( &Time, (BYTE *)&EventTime, sizeof(TRealTimer) ) == 0 )
		{
			//若读出的时间与查找的时间相等，直接中断查询
			break;
		}
		else if( (memcmp( &Time, (BYTE *)&EventTime, sizeof(TRealTimer) ) > 0)
			&& (memcmp( &Time, (BYTE *)&EventTimeBak, sizeof(TRealTimer) ) < 0) )
		{
			//若查找的时间大于此次读出的时间，小于上次读出的时间，则中断查询
			i -= 1;//i肯定不为1
			memcpy( (BYTE *)&EventOADCommonData, (BYTE *)&EventOADCommonDataBak, sizeof(TEventOADCommonData) );		
			dwPointer = dwPointerBak;
			break;
		}

		memcpy( (BYTE *)&EventTimeBak, (BYTE *)&EventTime, sizeof(TRealTimer) );
		memcpy( (BYTE *)&EventOADCommonDataBak, (BYTE *)&EventOADCommonData, sizeof(TEventOADCommonData) );	
		dwPointerBak = 	dwPointer;
	}

	if( i > twCurrentRecordNo )
	{
		//查找完后，若要读的时间小于最后一次的时间，则返回此次的数据
		if( memcmp( &Time, (BYTE *)&EventTime, sizeof(TRealTimer) ) < 0 )
		{
			i = twCurrentRecordNo;
		}
		else
		{
			return 0x8000;// 没找到相应时间
		}
	}
	
	//计算此条记录偏移地址
	GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, dwPointer, &EventSectorInfo);

	Data.Last = i;
	Data.pEventOADCommonData = &EventOADCommonData;

	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		i = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventStatus( eGET_EVENT_STATUS, i );	
	}
	else if( EventAddrLen.EventIndex == ePRG_RECORD_NO )
	{
		status = GetPrgRecord();
	}
	
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pEventAttInfo = &EventAttInfo;
	Data.pEventAddrLen = &EventAddrLen;
	Data.pEventSectorInfo = &EventSectorInfo;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.Tag = Tag;
	Data.status = status;
	Data.AllorSep = 0;

	return ReadEventRecordSub(&Data);
}

static WORD ReadEventRecordByTimeInterval( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	TEventSectorInfo	EventSectorInfo;
	DWORD dwTemp,dwPointer,SearchStartTime,SearchEndTime,EventTimeSec;
	WORD twCurrentRecordNo;
	BYTE i, status, Offset;
	TRealTimer	SaveStartTime,SaveEndTime;
	TRealTimer	EventTime;//以后优化为指针更好zym
	WORD wLen = 0;	//读一次事件记录得到的数据长度，若读取的事件未结束，则长度短
	WORD ReturnLen = 0;	//返回数据总长度
	
	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//根据当前记录号为0，且当前状态为事件没发生
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}
	else if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
	{
		twCurrentRecordNo = EventAttInfo.MaxRecordNo;
	}
	else
	{
		twCurrentRecordNo = EventDataInfo.RecordNo;	
	}
	
	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	
	
	//要查找的起始时间大于等于查找的截止时间，返回错误
	if( pDLT698RecordPara->TimeIntervalPara.StartTime >= pDLT698RecordPara->TimeIntervalPara.EndTime )
	{
		return 0x8000;
	}
	
	//事件要读取的时标单位为秒（相对秒数）	
	memcpy( (BYTE*)&SearchStartTime, (BYTE*)&pDLT698RecordPara->TimeIntervalPara.StartTime, sizeof(DWORD) );

	memcpy( (BYTE*)&SearchEndTime, (BYTE*)&pDLT698RecordPara->TimeIntervalPara.EndTime, sizeof(DWORD) );
	
    Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum = 0;

	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		
        status = api_DealEventStatus( eGET_EVENT_STATUS, GetSubEventIndex((TEventAddrLen*)&EventAddrLen) );	
    }
	else if( EventAddrLen.EventIndex==ePRG_RECORD_NO )
	{
		status = GetPrgRecord();
	}
	
	// 在已有的记录里搜索
    for( i = twCurrentRecordNo; i >= 1; i-- )
	{
		//按结束时间读，若最近一次事件未结束，则跳过本次循环
		if( (i == 1) && status && (pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_END_TIME_FLAG)) )
		{
			continue;
		}

		//计算此条记录偏移地址
		GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, (EventDataInfo.RecordNo-i), &EventSectorInfo);
	
		dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
		api_ReadMemRecordData(dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
		
        memset( (BYTE*)&EventTime, 0x00, sizeof(EventTime) );
        
		if( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_START_TIME_FLAG) )
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.BeginTime, sizeof(TRealTimer) );			
		}
		else
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.EndTime, sizeof(TRealTimer) );
		}
		EventTimeSec = api_CalcInTimeRelativeSec((TRealTimer *)&EventTime);//计算时间的相对秒数
		//判断要读的时间区间（左闭右开）与最近一次的大小关系，若超出时间区间，则直接返回错误		
	    if(( EventTimeSec < SearchStartTime) || (EventTimeSec >= SearchEndTime) )
		{
			continue;// 没找到相应时间
		}

		Data.pEventAttInfo = &EventAttInfo;
		Data.pEventAddrLen = &EventAddrLen;
		Data.pEventSectorInfo = &EventSectorInfo;
		Data.Tag = Tag;
		Data.AllorSep = 0;	
		Data.Last = i;
		Data.pEventOADCommonData = &EventOADCommonData;

		if( i == 1)	//最近一次事件需要判断事件是否结束
		{
			Data.status = status;
		}
		else
		{
			Data.status = 0;	//非最近一次事件默认已结束
		}
			
		wLen = ReadEventRecordSub(&Data);
		if( wLen == 0x8000 )
	    {
            return 0x8000;
	    }

	    if( wLen == 0 )
	    {
            return ReturnLen;
	    }
	    
	    if( (ReturnLen+wLen) > Len )
	    {
            return ReturnLen;
	    }
	    ReturnLen += wLen;
	    
		Data.pBuf += wLen;

		if( Data.Len < wLen )
		{
            return ReturnLen;
		}
		Data.Len -= wLen;
		
		Data.pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum++;
		
	}

	if( ReturnLen == 0 )
	{
		return 0x8000;// 没找到相应时间
	}
	
	return ReturnLen;
}

//-----------------------------------------------
//函数功能: 读事件记录
//
//参数: 
//		Tag[in]:				0 不加Tag 1 加Tag                
//		pDLT698RecordPara[in]: 	读取冻结的参数结构体                
//  	Len[in]：				输入的buf长度 用来判断buf长度是否够用                
//  	pBuf[out]: 				返回数据的指针，有的传数据，没有的补0	
//返回值:			bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//					bit0~bit14 代表返回的冻结数据长度
//备注: 只支持上Last条记录
//-----------------------------------------------
WORD ReadEventRecordByNo( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	TEventSectorInfo	EventSectorInfo;
	DWORD dwTemp;
	WORD twCurrentRecordNo;
	BYTE i,status;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//根据当前记录号为0，且当前状态为事件没发生
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}
	else if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
	{
		twCurrentRecordNo = EventAttInfo.MaxRecordNo;
	}
	else
	{
		twCurrentRecordNo = EventDataInfo.RecordNo;		
	}
	
	//清此通道新增事件列表中的指定事件
	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	
	
	if( (pDLT698RecordPara->TimeOrLast > twCurrentRecordNo) || (pDLT698RecordPara->TimeOrLast==0) )// 包括无数据的情况, 上0次返回无数据
	{
		return 0x8000;//无数据
	}
	
	EventDataInfo.RecordNo -= pDLT698RecordPara->TimeOrLast;	
	//计算此条记录偏移地址
	GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, EventDataInfo.RecordNo, &EventSectorInfo);
	
	dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
	
	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		// 工况类事件
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		i = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventStatus( eGET_EVENT_STATUS, i );	
	}
	else if( EventAddrLen.EventIndex == ePRG_RECORD_NO )
	{
		//编程类
		status = GetPrgRecord();
	}
	
	api_ReadMemRecordData( dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
	
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pEventOADCommonData = &EventOADCommonData;
	Data.pEventAttInfo = &EventAttInfo;
	Data.pEventAddrLen = &EventAddrLen;
	Data.pEventSectorInfo = &EventSectorInfo;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.Tag = Tag;
	Data.status = status;
	Data.AllorSep = 0;
	Data.Last = (BYTE)pDLT698RecordPara->TimeOrLast;

	return ReadEventRecordSub(&Data);
}

//-----------------------------------------------
//函数功能: 读事件记录
//
//参数: 
//	Tag[in]:				0 不加Tag 1 加Tag                
//	pDLT698RecordPara[in]: 	读取冻结的参数结构体                
//  Len[in]：				输入的buf长度 用来判断buf长度是否够用                
//  pBuf[out]: 				返回数据的指针，有的传数据，没有的补0	
//返回值:			bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//					bit0~bit14 代表返回的冻结数据长度
//备注:
//-----------------------------------------------
WORD api_ReadEventRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	BYTE tEventIndex;

	if( GetEventIndex( pDLT698RecordPara->OI, &tEventIndex ) == FALSE )
	{
		return 0x8000;
	}

	if( pDLT698RecordPara->eTimeOrLastFlag == eNUM_FLAG )
	{
		// 读工况类，读上N次
		return ReadEventRecordByNo( Tag, pDLT698RecordPara, Len, pBuf );		
	}
	else if( 	(pDLT698RecordPara->eTimeOrLastFlag == eEVENT_START_TIME_FLAG) 
			|| 	(pDLT698RecordPara->eTimeOrLastFlag == eEVENT_END_TIME_FLAG) )
	{
		// 读工况类，以开始时间，结束时间读
		return ReadEventRecordByTime( Tag, pDLT698RecordPara, Len, pBuf );		
	}
	else if( 	( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_START_TIME_FLAG) )
			||	( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_END_TIME_FLAG) ) )
	{
		return ReadEventRecordByTimeInterval( Tag, pDLT698RecordPara, Len, pBuf );	
	}

	return 0x8000;
}

//-----------------------------------------------
//函数功能: 读取当前记录数，最大记录数
//
//参数:  
//  	OI[in]: 	OI   
//  	Phase[in]: 	相位0总,1A,2B,3C            
//  	Type[in]: 	参考eEVENT_RECORD_NO_TYPE                
//  	pBuf[out]：	返回数据的缓冲                    
//返回值:	TRUE/FALSE
//
//备注: 
//-----------------------------------------------
BOOL api_ReadEventRecordNo( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf )
{
	TEventAttInfo	EventAttInfo;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	WORD			twCurrentRecordNo;
	BYTE			tEventIndex;

	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}

	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	if( Type == eEVENT_CURRENT_RECORD_NO )
	{
		// 当前记录数
		api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
		if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
		{
			twCurrentRecordNo = EventAttInfo.MaxRecordNo;
		}
		else
		{
			twCurrentRecordNo = EventDataInfo.RecordNo;
		}
		memcpy( pBuf, (BYTE *)&twCurrentRecordNo, sizeof(WORD) );		
	}
	else if( Type == eEVENT_MAX_RECORD_NO )
	{
		// 最大记录数
		api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
		memcpy( pBuf, (BYTE *)&EventAttInfo.MaxRecordNo, sizeof(WORD) );					
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

#if ( SEL_EVENT_LOST_V == YES )
//-----------------------------------------------
//函数功能: 读取失压属性13
//
//参数:  
//  Index[in]:	0 所有 
// 				1 事件发生总次数 
// 				2 事件总累计时间    
// 				3 最近一次失压发生时间
// 				4 最近一次失压结束时间   
//  pBuf[out]：	返回数据的缓冲
//                    
//返回值:		TRUE/FALSE
//
//备注:国网2013版标准宣贯材料中：
//（3）“最近1次失压发生时刻”：A、B、C 相中最近发生的那次失压发生时刻。
//（4）“最近1次失压结束时刻”：A、B、C 相中最近发生的那次失压结束时刻。
//-----------------------------------------------
BOOL api_ReadEventLostVAtt( BYTE Index, BYTE *pBuf )
{
	TEventOADCommonData	EventOADCommonData;
	TEventDataInfo		EventDataInfo;
	TRealTimer			Time[2];// 0,1最近一次发生结束时间
	DWORD				dwAddr,dwNum,dwTime,dwPointer,dwTemp;
	DWORD				dwStartTime;
	BYTE				i,off,status;
	TDLT698RecordPara 	DLT698RecordPara;
	DWORD 				dwOAD[2];
	WORD 				wLen;
	if( Index > 4 )
	{
		return FALSE;
	}

	dwNum = 0;
	dwTime = 0;
	dwStartTime = 0;
	status = 0;
	
	memset( (BYTE*)&Time[0], 0x00, sizeof(TRealTimer) );
	memset( (BYTE*)&Time[1], 0x00, sizeof(TRealTimer) );
	
	for( i = 0; i < MAX_PHASE; i++ )
	{
		dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.EventDataInfo[eSUB_EVENT_LOSTV_A+i] );
		api_VReadSafeMem( dwAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo );

		if( EventDataInfo.RecordNo != 0 )
		{
			dwOAD[0] = 0x00021E20;//201E	8	事件发生时间
			dwOAD[1] = 0x00022020;//2020	8	事件结束时间
			
			DLT698RecordPara.OI = 0x3000;                   //OI数据
			DLT698RecordPara.Phase = (ePHASE_TYPE)(i+1);

			DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOAD指针
			DLT698RecordPara.OADNum = 2;     	//OAD个数
			DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //选择次数
			DLT698RecordPara.TimeOrLast = 1;           //绝对时间数 上1次

			memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) ); 
			
			//调用函数 读取记录 返回数据 不加TAG
			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, sizeof(TEventTime), (BYTE *)&EventOADCommonData.EventTime );
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				memset( (BYTE*)&EventOADCommonData.EventTime, 0x00, sizeof(TEventTime) );
			}
			
			if( lib_IsAllAssignNum( (BYTE *)&EventOADCommonData.EventTime.BeginTime, 0xff, sizeof(TRealTimer) ) == TRUE )
			{
				memset( (BYTE*)&EventOADCommonData.EventTime.BeginTime, 0x00, sizeof(TRealTimer) );
			}
			
			if( lib_IsAllAssignNum( (BYTE *)&EventOADCommonData.EventTime.EndTime, 0xff, sizeof(TRealTimer) ) == TRUE )
			{
				memset( (BYTE*)&EventOADCommonData.EventTime.EndTime, 0x00, sizeof(TRealTimer) );
			}
			
			status = api_DealEventStatus( eGET_EVENT_STATUS, (eSUB_EVENT_LOSTV_A+i) );
			// 还未结束
			if( status == 1 )
			{
				// 计算累计时间
				dwPointer = api_CalcInTimeRelativeSec( &RealTimer );							// 当前时间对应相对秒数
				dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );	// 发生时刻对应相对秒数
				
				if( (dwPointer!=0xffffffff) && (dwTemp!=0xffffffff) )
				{
					if( dwPointer >= dwTemp )
					{
						dwTime += (dwPointer-dwTemp);
					}
				}
			}
			
			dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );
			if( dwTemp > dwStartTime )
			{
				dwStartTime = dwTemp;
				memcpy(&Time[0], (BYTE*)&EventOADCommonData.EventTime.BeginTime, sizeof(TRealTimer) );
				if( status == 1 )
				{
					memset(&Time[1],0x00,sizeof(TRealTimer));
				}
				else
				{	
					memcpy(&Time[1], (BYTE*)&EventOADCommonData.EventTime.EndTime, sizeof(TRealTimer) );
				}
			}
		}

		dwNum += EventDataInfo.RecordNo;	// 总次数
		dwTime += EventDataInfo.AccTime;	// 总累计时间		
	}
		
	off = 0;
	
	if( Index == 0 )
	{
		memcpy( &pBuf[off], (BYTE *)&dwNum, sizeof(DWORD) );
		off += sizeof(DWORD);
		memcpy( &pBuf[off], (BYTE *)&dwTime, sizeof(DWORD) );
		off += sizeof(DWORD);
		memcpy( &pBuf[off], (BYTE *)&Time[0], sizeof(TRealTimer) );			
		off += sizeof(TRealTimer);
		memcpy( &pBuf[off], (BYTE *)&Time[1], sizeof(TRealTimer) );					
		off += sizeof(TRealTimer);
	}
	else if( Index == 1 )
	{
		memcpy( &pBuf[off], (BYTE *)&dwNum, sizeof(DWORD) );
		off += sizeof(DWORD);
	}
	else if( Index == 2 )
	{
		memcpy( &pBuf[off], (BYTE *)&dwTime, sizeof(DWORD) );
		off += sizeof(DWORD);
	}
	else if( Index == 3 )
	{
		memcpy( &pBuf[off], (BYTE *)&Time[0], sizeof(TRealTimer) );			
		off += sizeof(TRealTimer);
	}
	else
	{
		memcpy( &pBuf[off], (BYTE *)&Time[1], sizeof(TRealTimer) );					
		off += sizeof(TRealTimer);
	}
	
	return TRUE;
}
#endif

#if( SEL_PRG_INFO_Q_COMBO == YES )
//-----------------------------------------------
//函数功能: 读取当前值记录表(无功组合方式1特征字、无功组合方式2特征字编程次数)
//
//参数:  
//  byType[in]: 	0--无功组合方式1特征字, 1--无功组合方式2特征字
//  pBuf[out]：	返回数据的缓冲
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注: 事件类的属性7  事件发生源+发生次数+累计时间
//-----------------------------------------------
BOOL api_ReadCurrentRecordTable_Q_Combo( BYTE byType,BYTE *pBuf )
{	
	DWORD dwTemp, dwTemp2;
	
	dwTemp = 0;
	if( api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.dwPrgQComBo2Num ),sizeof(DWORD), (BYTE *)&dwTemp ) == FALSE )
	{
		return FALSE;
	}	
	if( byType == 0 )
	{
		if( api_ReadCurrentRecordTable( 0x3023, 0, 1, pBuf ) == 0 )
		{
			return FALSE;
		}	
		memcpy( (void*)&dwTemp2, pBuf+1, sizeof(DWORD) );
		if( dwTemp2 >= dwTemp )
		{
			dwTemp = dwTemp2 - dwTemp;
		}
		else
		{
			dwTemp = 0;
		}	
	}
	
	memset( (void *)pBuf, 0x00, 9 );
	pBuf[0] = byType;
	memcpy( (void *)(pBuf+1), (void *)&dwTemp, 4 );	
	return TRUE;
}
#endif//#if( SEL_PRG_INFO_Q_COMBO == YES )
//-----------------------------------------------
//函数功能: 读取当前值记录表
//
//参数:  
//  OI[in]: 	OI      
//  Phase[in]: 	相位0总,1A,2B,3C 
//	Type[in]:	0--读最近1次发生时间、结束时间(Class 7有发生源)  其它--读发生次数+累计时间(Class 7有发生源)
//  pBuf[out]：	返回数据的缓冲
//                    
//返回值:	0--读取失败，其它--返回读取长度
//
//备注: 读事件类Class 7最近1次发生时间、结束时间时，无功组合模式字编程记录不在此处处理，发生源固定为NULL，返回如下：
//			00 00 00--发生源为NULL，发生时间为NULL，结束时间为NULL
//			00 YY YY MM DD hh mm ss 00--发生源为NULL，发生时间有值，结束时间为NULL 
//			00 YY YY MM DD hh mm ss YY YY MM DD hh mm ss--发生源为NULL，发生时间有值，结束时间有值
//		读事件类Class 24最近1次发生时间、结束时间时，返回如下:
//			00 00--发生时间为NULL，结束时间为NULL
//			YY YY MM DD hh mm ss 00--发生时间有值，结束时间为NULL 
//			YY YY MM DD hh mm ss YY YY MM DD hh mm ss--发生时间有值，结束时间有值
//		读事件类Class 7发生次数+累计时间时，返回如下：
//			00 00 00 00 00 00 00 00 00--发生源为NULL，发生次数为4字节00，累计时间为4字节00
//			00 N0 N1 N2 N3 00 00 00 00--发生源为NULL，发生次数为4字节数据，累计时间为4字节00
//			00 N0 N1 N2 N3 T0 T1 T2 T3--发生源为NULL，发生次数为4字节数据，累计时间为4字节数据
//		读事件类Class 24发生次数+累计时间时，返回如下：
//			00 00 00 00 00 00 00 00--发生次数为4字节00，累计时间为4字节00
//			N0 N1 N2 N3 00 00 00 00--发生次数为4字节数据，累计时间为4字节00
//			N0 N1 N2 N3 T0 T1 T2 T3--发生次数为4字节数据，累计时间为4字节数据
//-----------------------------------------------
WORD api_ReadCurrentRecordTable( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf )
{
	WORD wLen;
	TDLT698RecordPara DLT698RecordPara;      
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo	EventAttInfo;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOAD[3];
	DWORD	dwTemp,dwTemp1;
	//WORD	twSavePointer;
	BYTE	i,j,status;
	BYTE	*pTemp;
	
	if( GetEventIndex( OI, &i ) == FALSE )//判断事件OI是否支持，不支持返回false
	{
		return 0;
	}
	
	// 只支持分相ABC，读总，填零返回TRUE
	if( Phase == ePHASE_ALL )
	{
		if( EventInfoTab[i].NumofPhase == 3 )//如果对应事件不支持总 返回NULL
		{
			pBuf[0] = 0;
			return 1;
		}
	}

	EventAddrLen.EventIndex = i;
	EventAddrLen.Phase = Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		
	pTemp = pBuf;

	// 事件发生源
	if( ((OI>=0x3000)&&(OI<=0x3008)) || (OI==0x300b) || (OI==0x303b) ) 	//class_id=24
	{
		// 无
		wLen = 0;
	} 
	else												//class_id=7
	{
		pTemp[0] = 0;// 发生源(1),SEL_PRG_INFO_Q_COMBO 无功组合模式字的发生源在 api_ReadCurrentRecordTable_Q_Combo中赋值，此处不用赋值
		wLen = 1;
	}
	pTemp += wLen;

	if( Type != 0 )//Type[in]:	0--读最近1次发生时间、结束时间(Class 7有发生源)  其它--读发生次数+累计时间(Class 7有发生源)
	{	
		// 事件发生次数
		memcpy( pTemp, (BYTE *)&EventDataInfo.RecordNo, sizeof(DWORD) );//发生次数
		pTemp += sizeof(DWORD);
	}
	
	dwOAD[0] = 0x00021E20;//201E	8	事件发生时间 
	dwOAD[1] = 0x00022020;//2020	8	事件结束时间
	
	DLT698RecordPara.OI = OI;                   //OI数据
	DLT698RecordPara.Phase = (ePHASE_TYPE)Phase;
	
	DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOAD指针
	DLT698RecordPara.OADNum = 2;     	//OAD个数
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //选择次数
	DLT698RecordPara.TimeOrLast = 1;           //绝对时间数 上1次

	// 累计时间
	if( (EventAddrLen.EventIndex<eNUM_OF_EVENT_PRG) && (EventAddrLen.EventIndex>=ePRG_RECORD_NO) )
	{
		if( Type == 0 )
		{
			//调用函数 读取记录 返回数据 不加TAG
			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), pTemp );

			//如果BeginTime中为0，则拷到pTemp也是00 00，两个00表示发生时间、结束时间都为NULL
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				pTemp[0] = 0x00;
				pTemp[1] = 0x00;  
				return (pTemp+2-pBuf);
			}
			else//如果BeginTime中不为0，则拷到pTemp也7字节BeginTime 00，00表示结束时间为NULL
			{				
				if(( EventAddrLen.EventIndex == ePRG_ADJUST_TIME_NO )//校时事件返回记录结束时间
				||( EventAddrLen.EventIndex == ePRG_BROADJUST_TIME_NO ))//广播校时事件返回记录结束时间
				{
					if( wLen < 14 )
					{
						pTemp[7] = 0x00;
						return (pTemp+8-pBuf);//如果BeginTime中不为0，则拷到pTemp也7字节BeginTime 00，00表示结束时间为NULL
					}
					else
					{
						return (pTemp+14-pBuf);;
					}
				}
				pTemp[7] = 0x00;
				return (pTemp+8-pBuf);
			}		 
		}
		else
		{		
			memset( pTemp, 0x00, sizeof(DWORD) );//操作类编程无累计时间
			return (pTemp+4-pBuf);
		}
	}
	else
	{
		j = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );	
		
		if( Type == 0 )
		{
			//调用函数 读取记录 返回数据 不加TAG
			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), pTemp );

			//如果BeginTime中为0，则拷到pTemp也是00 00，两个00表示发生时间、结束时间都为NULL
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				pTemp[0] = 0x00;
				pTemp[1] = 0x00;  
				return (pTemp+2-pBuf);
			}
			else
			{
				if( api_DealEventStatus( eGET_EVENT_STATUS, j ) == 1 )//如果事件正在发生
				{
	
					dwOAD[0] = 0x00022020;//2020	8	事件结束时间
					DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOAD指针
					DLT698RecordPara.OADNum = 1;     	//OAD个数
					DLT698RecordPara.TimeOrLast = 2;           //绝对时间数 上2次
					//调用函数 读取记录 返回数据 不加TAG
					wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, 7, (pTemp+7) );	
					if( (wLen & 0x8000)||(wLen == 1) )
					{
						pTemp[7] = 0x00;
						return (pTemp+8-pBuf);//如果BeginTime中不为0，则拷到pTemp也7字节BeginTime 00，00表示结束时间为NULL
					}
					else
					{
						return (pTemp+14-pBuf);
					}
				}
				else
				{
					return (pTemp+14-pBuf);
				}
			}		
		}
		else
		{
			status = api_DealEventStatus( eGET_EVENT_STATUS, j );		

			// 发生还未结束
			if( status == 1 )
			{			
				//调用函数 读取记录 返回数据 不加TAG
    			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), (BYTE *)&EventOADCommonData.EventTime.BeginTime );
				
				if( (wLen & 0x8000) == 0x0000 )
				{	
					// 计算累计时间
					dwTemp1 = api_CalcInTimeRelativeSec( &RealTimer );							// 当前时间对应相对秒数
					dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );	// 发生时刻对应相对秒数
					
					if( (dwTemp1!=0xffffffff) && (dwTemp!=0xffffffff) )
					{
						if( dwTemp1 >= dwTemp )
						{
							EventDataInfo.AccTime += (dwTemp1-dwTemp);
						}
						else
						{
							//EventDataInfo.AccTime = 0;
						}
					}
					else
					{
						//EventDataInfo.AccTime = 0; // 错误返回0
					}
				}
			}
	
			memcpy( pTemp, (BYTE *)&EventDataInfo.AccTime, sizeof(DWORD) );
			return (pTemp+4-pBuf);
		}
	}
}
#if( SEL_DLT645_2007 == YES )
BYTE SearchSignEventOI( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(SignEventOI_Table)/sizeof(SignEventOI_Table[0]));
	if( Num >= 0x80 )		//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == SignEventOI_Table[i] )
		{
			return 0X55;
		}
	}
	
	return 0x00;
}

//-----------------------------------------------
//函数功能: 读取645当前值记录表
//
//参数:  
//  OI[in]: 	OI      
//  Phase[in]: 	相位0总,1A,2B,3C 
//	Type[in]:	1 事件发生总次数 
// 				2 事件总累计时间    
// 				3 最近一次失压发生时间(仅10000101:最近1次失压发生时刻)
// 				4 最近一次失压结束时间(仅10000201:最近1次失压结束时刻)   
//  pBuf[out]：	返回数据的缓冲
//                    
//返回值:	0x8000--读取失败，其它--返回读取长度
//
//备注: 
//-----------------------------------------------
WORD api_ReadCurrentRecordTable645( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf )
{
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventSectorInfo	EventSectorInfo;
	DWORD	dwTemp,dwTemp1;
	//WORD	twSavePointer;
	BYTE	i,j,status,Buf[32+20];

    if( (Type > 4) || (Type == 0) )//不允许Type == 0;
    {
        return 0x8000;
    }
    
	if( GetEventIndex( OI, &i ) == FALSE )//判断事件OI是否支持，不支持返回false
	{
		return 0x8000;
	}
	
    
	if( (OI == 0x3000) && (Phase == ePHASE_ALL ) )//总失压事件单独处理
	{
	    #if ( SEL_EVENT_LOST_V == YES )
        api_ReadEventLostVAtt( Type, Buf);
        if( (Type == 1) || (Type == 2) )//发生次数与累计时间
        {
            memcpy( (BYTE*)&dwTemp1, Buf, 4 ); 
            if( Type == 2 )//累计时间将秒转化为分钟
            {
                dwTemp1 = (dwTemp1/60);
            }

            dwTemp = lib_DWBinToBCD(dwTemp1);
            memcpy( pBuf, (BYTE *)&dwTemp, 3 );
            return 3;         
        }
        else//最近一次发生、结束时间
        {
            api_TimeFormat698To645((TRealTimer * )Buf, pBuf );
            lib_InverseData( pBuf, 6 );
            return 6; 
        }
        #else
        return 0x8000;
        #endif
       
	}
	else
	{
	    // 只支持分相ABC，读总，填零返回TRUE
    	if( Phase == ePHASE_ALL )
    	{
    		if( EventInfoTab[i].NumofPhase == 3 )//如果对应事件不支持总 返回NULL
    		{
    			return 0x8000;
    		}
    	}
    	
    	EventAddrLen.EventIndex = i;
    	EventAddrLen.Phase = Phase;
    	if( GetEventInfo( &EventAddrLen ) == FALSE )
    	{
    		return 0;
    	}

    	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
    	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
    		
    	if( EventDataInfo.RecordNo == 0 )
    	{
			if( Type < 3 )//读发生次数、累计时间    
    		{		
    		    memset( pBuf, 0x00, 3 );
    		    return 3;
    		}
    		else
    		{ 
    			memset( pBuf, 0x00, 6 );
    			return 6;
    		}
    	}
    	
		//计算此条记录偏移地址
		GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, (EventDataInfo.RecordNo-1), &EventSectorInfo);
	
        
        if( Type == 1 )//事件发生次数
        { 
            dwTemp = lib_DWBinToBCD(EventDataInfo.RecordNo);
            memcpy( pBuf, (BYTE *)&dwTemp, 3 );

            return 3;         
        }
        else if( Type == 2 )//累计时间
        {
            // 累计时间
            if( (EventAddrLen.EventIndex<eNUM_OF_EVENT_PRG) && (EventAddrLen.EventIndex>=ePRG_RECORD_NO) )
            {
                memset( pBuf, 0xff, 3 );//操作类编程无累计时间
            }
            else
            {
                j = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );  

                status = api_DealEventStatus( eGET_EVENT_STATUS, j );       

                // 发生还未结束
                if( status == 1 )
                {           
                    EventAddrLen.dwRecordAddr += EventSectorInfo.dwSectorAddr;
                    api_ReadMemRecordData( EventAddrLen.dwRecordAddr, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );

                    // 计算累计时间
                    dwTemp1 = api_CalcInTimeRelativeSec( &RealTimer );                          // 当前时间对应相对秒数
                    dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );  // 发生时刻对应相对秒数
                    
                    if( (dwTemp1!=0xffffffff) && (dwTemp!=0xffffffff) )
                    {
                        if( dwTemp1 >= dwTemp )
                        {
                            EventDataInfo.AccTime += (dwTemp1-dwTemp);
                        }
                        else
                        {
                            EventDataInfo.AccTime = 0;
                        }
                    }
                    else
                    {
                        EventDataInfo.AccTime = 0; // 错误返回0
                    }
                }

                EventDataInfo.AccTime = (EventDataInfo.AccTime/60);//累计时间将秒转化为分钟
                dwTemp = lib_DWBinToBCD(EventDataInfo.AccTime);
                memcpy( pBuf, (BYTE *)&dwTemp, 3 );

                return 3;
            }
        }
     	else
    	{
            return 0x8000;
    	}
	}
	
    return 0x8000;
}

//-----------------------------------------------
//函数功能: 根据OAD读数据
//
//参数: 
//		pDLT698RecordPara[in]：指向结构体TDLT698RecordPara的指针              
//		pEventOADCommonData[in]：指向结构体TEventOADCommonData的指针                
//		pEventAttInfo[in]：指向结构体TEventAttInfo的指针                
//		pEventAddrLen[in]：指向结构体TEventAddrLen的指针                
//		dwPointer[in]：地址偏移指针                
//		pBuf[out]：返回数据的指针，有的传数据，没有的补0                
//		dwOad[in]：OAD                
//		Len[in]：输入的buf长度 用来判断buf长度是否够用                
//		Tag[in]：0 不加Tag  1 加Tag                
//		off[in]：已经有的偏移                
//		status[in]：事件结束标识                
//		AllorSep[in]：1 指定OAD 0 所有OAD
// 		Last[in]: 上N次
//返回值:	0xFFFF: 缓冲满 0x8000错误 其他值：返回数据长度             
//备注: 
//-----------------------------------------------
static WORD GetDataByOad645( BYTE NoDataFlag, TEventData *pData )
{
	DWORD dwTemp,dwTemp1,dwTemp2,dwOad,dwOad1;
	WORD wLen,wLen1,OI,w;
	BYTE Temp,i,OadIndex,SignFlag,Flag,tmpBuf[EVENT_ALONE_OADLEN+30];	
	BYTE *pTemp;
	BYTE Buf[20];
	BYTE RelayErrBuf[10];
	TFourByteUnion DWValue,DWValue1;
	TTwoByteUnion  WValue;
	
	Flag = 0;
	memset(Buf, 0x00, sizeof(Buf));
    
	if( pData->dwOad == STARTTIME_OAD )
	{
		wLen = 6;
		if( (pData->off+6) > pData->Len )
		{
			return 0xFFFF;// 已满
		}

		//若发生时间都为ff，则置为null
		if( (lib_IsAllAssignNum((BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, 0xff, sizeof(TRealTimer) ) == TRUE)
		    ||( NoDataFlag == 0x55 ) )
		{   
			memset( pData->pBuf+pData->off, 0x00, 6 );
		}
		else
		{
			api_TimeFormat698To645((TRealTimer * )&pData->pEventOADCommonData->EventTime.BeginTime, Buf );
			lib_ExchangeData( pData->pBuf+pData->off, Buf, 6);
		}				
	}
	else if( pData->dwOad == ENDTIME_OAD )
	{
		wLen = 6;
		if( (pData->off+6) > pData->Len )
		{
			return 0xFFFF;// 已满
		}

		//若结束时间都为ff，则置为null eNUM_OF_EVENT
		//校时记录，记录结束时间
		if( ((( lib_IsAllAssignNum((BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, 0xff, sizeof(TRealTimer) ) == TRUE )
			||	( pData->pEventAddrLen->EventIndex>=ePRG_RECORD_NO )) &&( pData->pEventAddrLen->EventIndex!=ePRG_ADJUST_TIME_NO) )
			||( NoDataFlag == 0x55 ) )
		{
			memset( pData->pBuf+pData->off, 0x00, 6 );
		}
		else
		{
			api_TimeFormat698To645((TRealTimer * )&pData->pEventOADCommonData->EventTime.EndTime, Buf );
			lib_ExchangeData( pData->pBuf+pData->off, Buf, 6);
		}
	}
	else if( pData->dwOad == 0x55555555 )//操作者代码
	{
		wLen = 4;
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// 已满
		}
		if( NoDataFlag == 0x55 )
		{
			memset( pData->pBuf+pData->off, 0x00, 4 );
		}
		else
		{
			memset( pData->pBuf+pData->off, 0xff, 4 );
		}

		wLen = 4;
	}	
	else
	{
		dwTemp1 = 0;
		OadIndex = 0;
		dwOad = pData->dwOad;
		OadIndex = (BYTE)(dwOad>>24);
		lib_ExchangeData( (BYTE*)&OI, (BYTE*)&dwOad, 2);//取OI

		for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
		{
			if( pData->pEventAttInfo->Oad[i] == pData->dwOad )
			{
				break;
			}
			dwTemp1 += pData->pEventAttInfo->OadLen[i];
		}

		// 没找到指定OAD
		if( i == pData->pEventAttInfo->NumofOad )
		{
			if( OadIndex != 0 )
			{
				dwTemp1 = 0;
				dwOad &= ~0xFF000000;
				// 索引置为0再找一遍
				for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
				{
					if( pData->pEventAttInfo->Oad[i] == dwOad )
					{
						wLen = api_GetProOADLen( eGetRecordData, 0, (BYTE *)&pData->dwOad, 0 );
						if( wLen != 0x8000 )
						{
							Temp = 2;
							break;
						}
					}
					dwTemp1 += pData->pEventAttInfo->OadLen[i];
				}
			}
		}

		// 找到OAD
		if( i != pData->pEventAttInfo->NumofOad )
		{
			dwOad = pData->dwOad;
			wLen = api_GetProOADLen( eGetNormalData, 0, (BYTE *)&dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}

			if( pData->pEventAttInfo->OadLen[i] > EVENT_ALONE_OADLEN )
			{
				return 0xFFFF;
			}

			w = pData->pEventAttInfo->OadLen[i];

			if( pData->pEventAddrLen->EventIndex <= ePRG_RECORD_NO )
			{
				Temp = (BYTE)((pData->dwOad&0x00E00000)>>21);
				
				if( (pData->Last==1) && (pData->status==1) && ((Temp==3)||(Temp==4)) )
				{
					memset( tmpBuf, 0x00, pData->pEventAttInfo->OadLen[i] );
				}
				else
				{
				    if( NoDataFlag == 0x55 )
				    {
                        memset( tmpBuf, 0x00, pData->pEventAttInfo->OadLen[i] );
					}
					else
					{
						dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
						dwTemp += dwTemp1;
    					api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], tmpBuf);
					}
				}
			}
			else
			{
				if( NoDataFlag == 0x55 )
				{
					//未产生事件
                    memset( tmpBuf, 0x00, pData->pEventAttInfo->OadLen[i] );
				}
				else
				{
					dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
					dwTemp += dwTemp1;
					//根据地址长度读FLASH
                    api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], tmpBuf);
				}
			}

			if( OadIndex != 0 )
			{
				pTemp = tmpBuf;
				dwTemp = dwOad;

				for( i = 1; i < OadIndex; i++ ) // pData->OadIndex 从1开始
				{
					dwTemp2 = i;
					dwOad = (dwTemp&0X00FFFFFF);//先去掉元素索引
					dwOad |= (dwTemp2<<24);//置元素索引
					wLen1 = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );
					if( wLen1 == 0x8000 )
					{
						return 0x8000;
					}
					pTemp += wLen1;
				}
			}
			else
			{
				pTemp = tmpBuf;
			}
			
			if( OI < 0x1000 )
			{
				api_EnergyDBaseToEnergy( SwitchNormalEnergy, w/5, w, pTemp );//转化为电能 w为数据长度 每个电能占5个字节
			}

			SignFlag = SearchSignEventOI( OI );
			dwOad1 = (pData->dwOad&0x0000ffff);
			//电流、有功功率、无功功率、视在功率、特殊处理
			if( (OI == 0x2001) || (OI == 0x2004)
			|| (OI == 0x2005) || (OI == 0x2006) )
			{
				wLen = 3;
				if((OI == 0x2001)&&(pData->pDLT698RecordPara->OI == 0X300D))
				{	
					DWValue.l = 0;
					for( i = 0; i < 3; i++ )
					{
						memcpy( DWValue1.b, pTemp+4*i, 4 );
						DWValue.l += DWValue1.l;
					}
					DWValue.l = DWValue.l /3;
				}
				else
				{
					memcpy( DWValue.b, pTemp, 4 );
				}
				

				if( DWValue.l < 0 )
				{
					DWValue.l *= -1;
					Flag = 1;
				}
				else
				{
				    Flag = 0;
				}

				DWValue.d = lib_DWBinToBCD(DWValue.d);

				if( Flag == 1 )
				{
					DWValue.b[2] |= 0x80;
				}

				memcpy( pData->pBuf+pData->off, DWValue.b, 3 );
		       
		    }
			else if( (OI == 0x2026) || (OI == 0x2027) )//电压不平衡率、电流不平衡率
			{
			  	wLen = 3;
				DWValue.l = 0;
				memcpy( DWValue.b, pTemp, 2 );
				DWValue.d = lib_DWBinToBCD(DWValue.d);
				memcpy( pData->pBuf+pData->off, DWValue.b, wLen );
			}
			else if( OI == 0x4000 )//校时OAD
			{
				wLen = 6;
				api_TimeFormat698To645((TRealTimer * )pTemp, Buf );
				lib_ExchangeData( pData->pBuf+pData->off, Buf, 6);  
			}
			else if( OI == 0x4116 ) //结算日
			{
				wLen = 2;
				WValue.b[0] = lib_BBinToBCD( pTemp[1] );
				WValue.b[1] = lib_BBinToBCD( pTemp[0] ); 
				memcpy( pData->pBuf + pData->off, WValue.b, 2 ); 
			}
			else
			{
				if( wLen == 1 )//暂因为1个字节的数据都是无符号的
				{
					DWValue.b[0] = lib_BBinToBCD( pTemp[0] );
					memcpy( pData->pBuf+pData->off, DWValue.b, 1 );
				}
				else if( wLen == 2 )
				{
					memcpy( WValue.b, pTemp, 2 );
					if( SignFlag == 0x55 )
					{
						if( WValue.sw < 0 )
						{
							WValue.sw *= -1;
							Flag = 1;
						}
						else
						{
							Flag = 0;
						}
					}
					WValue.w = lib_WBinToBCD( WValue.w );
					if( SignFlag == 0x55 )
					{
						if( Flag == 1 )
						{
							 WValue.b[1] |= 0x80;
						}
					}
					memcpy( pData->pBuf+pData->off, WValue.b, 2 );
				}
				else if( wLen == 4 )
				{
					memcpy( DWValue.b, pTemp, 4 );
					if( SignFlag == 0x55 )
					{
						if( DWValue.l < 0 )
						{
							DWValue.l *= -1;
							Flag = 1;
						}
						else
						{
							Flag = 0;
						}
					}
					DWValue.d = lib_DWBinToBCD( DWValue.d );

					if( SignFlag == 0x55 )
					{
						if( Flag == 1 )
						{
						    DWValue.b[3] |= 0x80;
						}
					}     

					memcpy( pData->pBuf+pData->off, DWValue.b, 4 );
				}
				else if( wLen == 11 )//需量
				{
					memcpy( DWValue.b, pTemp, 4 );
					DWValue.d = lib_DWBinToBCD( DWValue.d );
					//写入需量，长度3
					memcpy( pData->pBuf+pData->off, DWValue.b, 3 );
					//向临时数组存入转换后的BCD时间
					api_TimeFormat698To645( (TRealTimer *)(pTemp + 4), Buf );
					//写入时间，需量时间年减去2000，没有秒，长度5
					lib_ExchangeData( pData->pBuf+pData->off + 3, Buf, 5 );
					//645需量一条长度8
					wLen = 8;
				}
				else
				{
					return 0x8000;
				}
			}

		}
		else
		{
			if( pData->pDLT698RecordPara->OADNum == 1 )//读取指定OAD，不支持的数据项，返回数据错误
			{
				return 0x8000;
			}

			dwOad = pData->dwOad;
			wLen = api_GetProOADLen( eGetNormalData, 0, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )//不支持的数据项
			{
				if( OI < 0x1000 )//单相不支持的电能，默认填充4字节0xff
				{
					wLen = 4;
				}
				else if( (OI >= 0x1000) && (OI <= 0x10A3) )	//需量类
				{
					//读分相需量，程序不支持，所以这里加上补FF的长度
					wLen = 8;	//3长度需量，5长度时间
				}
				else if( (OI >= 0x2000) &&(OI <= 0x2027) ) //单相不支持的变量 默认为2个字节
				{
					wLen = 2; 
				}
			}

			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// 已满
			}

			//电流、有功功率、无功功率、视在功率、电压不平衡率、电流不平衡率特殊处理
			if( (OI == 0x2001) || (OI == 0x2004)
			|| (OI == 0x2005) || (OI == 0x2006)
			|| (OI == 0x2026) || (OI == 0x2027) )
			{
				wLen = 3;
				memset( pData->pBuf+pData->off, 0xff, 3 );
			}
			else if( OI == 0x4000 )//校时OAD
			{
				wLen = 6;
				memset( pData->pBuf+pData->off, 0xff, 6 ); 
			}
			else if( OI == 0x400F )
			{
				//0x400F 只是在698中找到一个一字节的OAD来替代负荷开关误动作中的继电器状态标识，
				//实际上可以使用与698不冲突的OI来替代即可 ，这里只是因为重庆使用了这个OAD所以继续延用
				//因为在645中负荷开关误动作事件记录这个状态采用1字节数据，而698为8字节的继电器输出属性
				//因此需要进行转换
				//其中得1字节得负荷开关误动作得继电器状态
				//如果修改此项，注意修改645事件记录中得负荷开关误动作事件抄读
				wLen = 1;
				dwOad = 0x012205F2;
				dwTemp1 = 0;
				for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
				{
					if( pData->pEventAttInfo->Oad[i] == dwOad )
					{
						break;
					}
					dwTemp1 += pData->pEventAttInfo->OadLen[i];
				}
				if( i == pData->pEventAttInfo->NumofOad )
				{
					memset(pData->pBuf+pData->off,0xff,1);
				}
				else
				{
				  	memset(RelayErrBuf,0xff,10);
					dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
					dwTemp += dwTemp1;
					api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], RelayErrBuf);
					
					if( NoDataFlag == 0x55 )
					{
						memset(pData->pBuf+pData->off,0x00,1);
					}
					else
					{
						//继电器输出描述符是ASCII 可能是变长的 因此需要取长度 不能写死 
						wLen1 = api_GetProOADLen( eGetNormalData, 0, (BYTE *)&dwOad, 0 );
						if( (wLen1 > (sizeof(RelayErrBuf)+3)) || (wLen1 < 3) )
						{
							return 0x8000; 
						}
						memcpy( pData->pBuf + pData->off, RelayErrBuf + wLen1 - 3, 1 );
					}
				}
			}
			else if( (OI >= 0x1000) && (OI <= 0x10A3) ) //需量类
			{
				memset( pData->pBuf+pData->off, 0xff, 8 );
			}
			else
			{
				if( wLen == 1 )
				{
					memset( pData->pBuf+pData->off, 0xff, 1 );
				}
				else if( wLen == 2 )
				{
					memset( pData->pBuf+pData->off, 0xff, 2 );
				}
				else if( wLen == 4 )
				{
					memset( pData->pBuf+pData->off, 0xff, 4 );
				}
				else if( wLen == 11 )
				{
					memset( pData->pBuf+pData->off, 0xff, 11 );
				}
				else
				{
					return 0x8000;
				}

			}
		}
	}

	return wLen;
}

//-----------------------------------------------
//函数功能: 根据OAD读数据
//
//参数: 
//		pDLT698RecordPara[in]：		指向结构体TDLT698RecordPara的指针              
//		pEventOADCommonData[in]：	指向结构体TEventOADCommonData的指针                
//		pEventAttInfo[in]：			指向结构体TEventAttInfo的指针                
//		pEventAddrLen[in]：			指向结构体TEventAddrLen的指针                
//		dwPointer[in]：				地址偏移指针                
//		pBuf[out]：					返回数据的指针，有的传数据，没有的补0                
//		dwOad[in]：					OAD                
//		Len[in]：					输入的buf长度 用来判断buf长度是否够用                
//		Tag[in]：					0 不加Tag  1 加Tag                
//		off[in]：					已经有的偏移                
//		status[in]：				事件结束标识                
//		AllorSep[in]：				1 指定OAD 0 所有OAD
//返回值:							bit15位 置1 错误 置0正确； 
//									bit0~bit14 代表返回的数据长度             
//备注: 
//-----------------------------------------------
static WORD ReadEvent645RecordSub( BYTE NoDataFlag, TEventData *pData )
{
	DWORD dwOad,dwTemp;
	WORD wLen;
	BYTE i,j,Number,Temp;
	BYTE *pTemp,*pAllocBuf;
	
	// 找OI对应固定列的OAD
	for( i = 0; i < (sizeof(EventOadTab)/sizeof(TEventOadTab)); i++ )
	{
		if( pData->pDLT698RecordPara->OI == EventOadTab[i].OI )
		{
			break;
		}
	}

	if( i < (sizeof(EventOadTab)/sizeof(TEventOadTab)) )
	{
		pTemp = (BYTE *)&EventOadTab[i].OadList[0];	// 找到
		Number = EventOadTab[i].Number;
	}
	else
	{
		// 默认Class id 7 工况类事件，操作类编程
		pTemp = (BYTE *)&EventOadTab[0].OadList[0];	
		Number = EventOadTab[0].Number;
	}

	pData->off = 0;
	
	if( pData->pDLT698RecordPara->OADNum == 0 )
	{
		return 0x8000;
	}

	pData->AllorSep = 1;
	// 指定OAD
	for( i = 0; i < pData->pDLT698RecordPara->OADNum; i++ )
	{		    
		memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
		pData->dwOad = dwOad;
		
		// 是此OI对应的固定列
		wLen = GetDataByOad645( NoDataFlag, pData );

		if( wLen == 0xFFFF )
		{
			return 0; 	// 缓冲满
		}
		else if( wLen == 0x8000 )
		{
			return 0x8000; 	// 缓冲满
		}		

		pData->off += wLen;
	}	

	return pData->off;
}

//-----------------------------------------------
//函数功能: 读事件记录
//
//参数: 
//		Tag[in]:				0 不加Tag 1 加Tag                             
//  	Len[in]：				输入的buf长度 用来判断buf长度是否够用                
//  	pBuf[out]: 				返回数据的指针，有的传数据，没有的补0	
//返回值:			bit15位置1 代表冻结无此类型 置0代表数据能正确返回； 
//					bit0~bit14 代表返回的冻结数据长度
//备注: 只支持上Last条记录
//-----------------------------------------------
WORD api_ReadEvent645RecordByNo( TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	TEventSectorInfo	EventSectorInfo;
	DWORD dwTemp;
	WORD twCurrentRecordNo;
	BYTE i,status,NoDataFlag;
	
    NoDataFlag = 0;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
    if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
	{
		twCurrentRecordNo = EventAttInfo.MaxRecordNo;
	}
	else
	{
		twCurrentRecordNo = EventDataInfo.RecordNo;		
	}	
	
	if( pDLT698RecordPara->TimeOrLast==0 )// 包括无数据的情况, 上0次返回无数据
	{
		return 0x8000;//无数据
	}

    //根据当前记录号为0，且当前状态为事件没发生
	if( (EventDataInfo.RecordNo == 0) || (pDLT698RecordPara->TimeOrLast > twCurrentRecordNo) )
	{
		NoDataFlag = 0x55;
	}
	
	EventDataInfo.RecordNo -= pDLT698RecordPara->TimeOrLast;	
	
	//计算此条记录偏移地址
	GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, EventDataInfo.RecordNo, &EventSectorInfo);
	
	dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
	
	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		// 工况类事件
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		i = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventStatus( eGET_EVENT_STATUS, i );	
	}
	else if( EventAddrLen.EventIndex == ePRG_RECORD_NO )
	{
		//编程类
		status = GetPrgRecord();
	}
	
	api_ReadMemRecordData( dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
	
	if( NoDataFlag == 0x55 )
	{
		memset( &EventOADCommonData.EventTime, 0xFF, sizeof(EventOADCommonData.EventTime) );
	}

	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pEventOADCommonData = &EventOADCommonData;
	Data.pEventAttInfo = &EventAttInfo;
	//传入考虑两个扇区后的地址
	Data.pEventAddrLen = &EventAddrLen;
	Data.pEventSectorInfo = &EventSectorInfo;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.Tag = 0;
	Data.status = status;
	Data.AllorSep = 0;
	Data.Last = (BYTE)pDLT698RecordPara->TimeOrLast;

	return ReadEvent645RecordSub( NoDataFlag,&Data );
}

#endif
//-----------------------------------------------
//函数功能: 初始化所有相关RAM
//
//参数: 无
//	
//返回值: 无
//
//备注: 上电，总清零调用
//-----------------------------------------------
static void InitEventAllRam( void )
{
	// 清事件序号计数器
	ClearPrgRecordRam();
	// 清定时计数器
	memset(SubEventTimer, 0x00, sizeof(SubEventTimer));

	memset((BYTE *)&EventStatus, 0x00, sizeof(EventStatus));
}

//-----------------------------------------------
//函数功能: 清分相事件记录数据
//
//参数: 
//  OI[in]:	OI
//	
//返回值:  	TRUE   正确
//			FALSE  错误
//  
// 备注:
//-----------------------------------------------
BOOL api_ClrSepEvent( WORD OI )
{
	BYTE i,Type;

	if( GetEventIndex( OI, &Type ) == FALSE )
	{
		return FALSE;
	}
	//清事件记录
	ClearEventData( Type );
	//清对应的新增事件列表中的上报事件
	ClrReportAllChannelIndex( Type );
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 清所有事件记录数据
//
//参数: 
//  Type[in]:	
//				eFACTORY_INIT = 0,				清所有事件，包含电表清零记录，事件清零记录
//				eEXCEPT_CLEAR_METER,			清所有事件，不包含电表清零记录
//				eEXCEPT_CLEAR_METER_CLEAR_EVENT	清所有事件，不包含电表清零记录，事件清零记录
//返回值:	无
//  
// 备注:
//-----------------------------------------------
BOOL api_ClrAllEvent( BYTE Type )
{
	TEventAddrLen	EventAddrLen;
	BYTE i;

	for( i = 0; i < eNUM_OF_EVENT_ALL; i++ )
	{
		if( Type == eEXCEPT_CLEAR_METER )
		{
			#if( SEL_PRG_INFO_CLEAR_METER == YES )
			if( i == ePRG_CLEAR_METER_NO )
			{
				continue;	// 事件清零不清电表清零记录
			}
			#endif
		}
		else if( Type == eEXCEPT_CLEAR_METER_CLEAR_EVENT )
		{
			#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
			if( i == ePRG_CLEAR_EVENT_NO )
			{
				continue;	// 不清事件清零记录
			}
			#endif
			#if( SEL_PRG_INFO_CLEAR_METER == YES )
			if( i == ePRG_CLEAR_METER_NO )
			{
				continue;	// 不清电表清零记录
			}
			#endif
		}
		else if( Type == eFACTORY_INIT )
		{
			// do nothing
		}
		
		EventAddrLen.EventIndex = i;
		EventAddrLen.Phase = 0;
		if( GetEventInfo( &EventAddrLen ) == FALSE )
		{
			continue;
		}
		ClearEventData( i );
	}

	// 清相关事件的掉电数据
	api_ClrContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] ), GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag ) );

	ClearReport( Type );
	
	api_ClrRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW );
	api_ClrRunHardFlag( eRUN_HARD_READ_BAT_LOW );
	
	//清除645编程记录，没有放在枚举列表中，单独处理
	#if(SEL_PRG_RECORD645 == YES)
	api_ClrSafeMem( GET_SAFE_SPACE_ADDR( EventSafeRom.ProRecord645 ), sizeof(TEventDataInfo) );
	#endif

	return TRUE;
}

//当电能表记录全失压事件的同时，不应再重复记录分相失压事件。
/*
BYTE CheckSpecialEvent(BYTE EventOrder)
{
	//如果现在没有处于全失压或者掉电状态下，则电表正常运行，不需要特殊判断
	#if( SEL_EVENT_LOST_ALL_V == YES )
	if(GetLostAllVStatus(ePHASE_ALL) == 1)
	{
		//如果现在处于全失压状态，则分相失压检测时返回FALSE
		if( EventOrder == eEVENT_LOST_V_NO )
		{
			return FALSE;
		}
	}
	#endif

	#if( SEL_EVENT_LOST_POWER == YES )
	if(GetLostPowerStatus(ePHASE_ALL) == 1)
	{
		#if( SEL_EVENT_BREAK == YES )
		//如果现在处于掉电状态，则分相断相检测时返回FALSE
		if( EventOrder == eEVENT_BREAK_NO )
		{
			return FALSE;
		}
		#endif
	}
	#endif

	return TRUE;

}
*/

// 得到事件延时（事件序号从1开始）
BYTE GetEventDelayTime(BYTE EventOrder)
{
	BYTE DelayTime;

	#if( SEL_EVENT_METERCOVER == YES )
	if(EventOrder == eEVENT_METERCOVER_NO)
	{
		// 表盖取1秒延时
		DelayTime = 1;
	}
	#endif
	#if( SEL_EVENT_BUTTONCOVER == YES )
	else if(EventOrder == eEVENT_BUTTONCOVER_NO)
	{
		// 端盖取1秒延时
		DelayTime = 1;	
	}
	#endif
	#if( SEL_EVENT_RELAY_ERR == YES )
	else if(EventOrder == eEVENT_RELAY_ERR_NO)
	{
		// 继电器异常取1秒延时
		DelayTime = 1;
	}
	#endif
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	else if(EventOrder == eEVENT_MAGNETIC_INT_NO)
	{
		// 强磁取5秒延时
		DelayTime = 5;
	}
	#endif
	#if( SEL_EVENT_POWER_ERR == YES )
	else if(EventOrder == eEVENT_POWER_ERR_NO)
	{
		// 电源异常取1秒延时，实际无作用，事件处理都是在停上电时进行记录
		DelayTime = 1;
	}
	#endif
	#if( SEL_EVENT_LOST_POWER == YES )
	else if(EventOrder == eEVENT_LOST_POWER_NO)
	{
		// 掉电事件取1秒
		DelayTime = 1;
	}
	#endif
	#if( SEL_EVENT_RTC_ERR == YES )
	else if(EventOrder == eEVENT_RTC_ERR_NO)
	{
		// 时钟故障取1秒
		DelayTime = 1;
	}
	#endif
	#if( (SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES) || (SEL_EVENT_RELAY_ERR==YES) || (SEL_EVENT_MAGNETIC_INT==YES) || (SEL_EVENT_POWER_ERR==YES) || ( SEL_EVENT_LOST_POWER == YES ) ||( SEL_EVENT_RTC_ERR == YES ) )
	else
	#endif
	{
		// 填充定时器，启动事件记录延时处理，认为事件开始延时时间与结束延时时间可以不一致
		DealEventParaByEventOrder( READ, EventOrder, eTYPE_BYTE, (BYTE *)&DelayTime );
	}

	return DelayTime;
}

//-----------------------------------------------
//函数功能: 设置事件实时状态
//
//参数: 
// 			inSubEventIndex[in]		eSUB_EVENT_INDEX
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void SetEventSysStatus(BYTE inSubEventIndex)
{
	BYTE i, j;
	
	i = inSubEventIndex / 8;
	j = inSubEventIndex % 8;

	EventStatus.DelayedStatus[i] |= (0x01<<j);
}


//-----------------------------------------------
//函数功能: 清除事件状态
//
//参数: 
// 			inSubEventIndex[in]		eSUB_EVENT_INDEX
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void ClrEventSysStatus(BYTE inSubEventIndex)
{
	BYTE i, j;
	
	i = inSubEventIndex / 8;
	j = inSubEventIndex % 8;

	EventStatus.DelayedStatus[i] &= ~(0x01<<j);
}


//-----------------------------------------------
//函数功能: 获取事件状态
//
//参数: 
// 			inSubEventIndex[in]		eSUB_EVENT_INDEX
//                    
//返回值:  	TRUE:事件状态置位了 FALSE:事件状态未置位
//
//备注:   
//-----------------------------------------------
static BOOL GetEventSysStatus(BYTE inSubEventIndex)
{
	BYTE i, j;

	i = inSubEventIndex / 8;
	j = inSubEventIndex % 8;

	if( (EventStatus.DelayedStatus[i] & (0x01<<j)) != 0 )
	{
		// 状态有效
		return TRUE;
	}

	// 状态无效
	return FALSE;
}

//-----------------------------------------------
//函数功能: 处理事件状态
//
//参数: 
//			Type[in]：			eCLEAR_EVENT_STATUS							
//								eSET_EVENT_STATUS
//								eGET_EVENT_STATUS                    
//			inSubEventIndex[in]	eSUB_EVENT_INDEX
//                    
//返回值: Type为eGET_EVENT_STATUS时，1为事件发生还未结束，0为事件未发生 	
//				其他时  固定返回0
//备注:   
//-----------------------------------------------
BYTE api_DealEventStatus(BYTE Type, BYTE inSubEventIndex )
{	
	BYTE Status = 0;

	if( Type == eGET_EVENT_STATUS )
	{
		if( GetEventSysStatus(inSubEventIndex) == TRUE )
		{
			Status = 1;
		}
	}
	else if( Type == eSET_EVENT_STATUS )
	{
		SetEventSysStatus(inSubEventIndex);
	}
	else if( Type == eCLEAR_EVENT_STATUS )
	{
		ClrEventSysStatus(inSubEventIndex);
	}
	else
	{
		ASSERT(0, 0);	
	}

	return Status;
}

//-----------------------------------------------
//函数功能: 事件定时处理，每秒调用一次
//
//参数: 无
//                    
//返回值: 无
//			
//备注:
//-----------------------------------------------
static void EventSecTask( void )
{
	BYTE i,Status,Temp;

	#if( SEL_DEMAND_OVER == YES )
	DealDemandOverMaxDemand();
	#endif

	#if( (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
	FreshUnBalanceRate();
	#endif
	//-------------------------------
	// 处理所有事件记录
	// 事件序号从1开始
	for( i = 0; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		FunctionConst(EVENT_RUN_TASK);

		//三相三线表不保存B相事件但保存无功需量超限的第三象限事件
		if( (SubEventInfoTab[i].Phase == ePHASE_B)&&(MeterTypesConst == METER_3P3W) )
		{
			#if(SEL_DEMAND_OVER == YES)//28
			if( i != eSUB_EVENT_QIII_DEMAND_OVER )
			#endif
			{
			    continue;
			}

		}
		#if(SEL_EVENT_COS_OVER == YES)//28
		//三相三线表不保存ABC相功率因数超限事件
		if( (MeterTypesConst == METER_3P3W)
		&&( (i == eSUB_EVENT_COS_OVER_A)||(i == eSUB_EVENT_COS_OVER_B)||(i == eSUB_EVENT_COS_OVER_C)) )
		{
			continue;   
		}
		#endif
		// 是否处于活动状态
		if( SubEventTimer[i] != 0 )
		{
			// 时间递减
			SubEventTimer[i]--;

			// 是否时间到
			if( SubEventTimer[i] == 0 )
			{
				// 清看门狗
				CLEAR_WATCH_DOG;
				
				// 判断记录的状态
				if( EventStatus.CurrentStatus[i/8]&(0x01<<(i%8) ) )
				{
					api_DealEventStatus( eSET_EVENT_STATUS, i );
					SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_START, eEVENT_ENDTIME_CURRENT );
				}
				else
				{
					api_DealEventStatus( eCLEAR_EVENT_STATUS, i );
					SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_CURRENT );	
				}
			}
		}
	}
	
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		// 得到状态 非0即1
		Status = SubEventInfoTab[i].pGetEventStatus( SubEventInfoTab[i].Phase ); 
	
		// 若没有变化则不处理
		Temp = (BYTE)( EventStatus.CurrentStatus[i/8] & (0x01<<(i%8)) );
		Temp = (Temp>>(i%8));
		
		if( Status == Temp )
		{
			continue;
		}
	
		// 若延时正在继续
		if( SubEventTimer[i] != 0 )
		{
			// 停止计时，因为在延时过程中发生了状态变化
			// 延时状态取消，事件停止处理
			SubEventTimer[i] = 0;
		}
		else
		{
			SubEventTimer[i] = GetEventDelayTime(SubEventInfoTab[i].EventIndex);
		}
	
		if( Status == 0 )
		{
			EventStatus.CurrentStatus[i/8] &= (BYTE)~(0x01<<(i%8));
		}
		else
		{
			EventStatus.CurrentStatus[i/8] |= (BYTE)(0x01<<(i%8));
		}
	}
	
	ReportSecTask();
}

//-----------------------------------------------
//函数功能: 判断事件状态
//
//参数: 无
//                    
//返回值: 无
//			
//备注: 大循环调用
//-----------------------------------------------
void api_EventTask(void)
{
	// 秒变化
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_SECOND);
		
		EventSecTask();	
	}
	
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_MINUTE) == TRUE )
	{
        api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_MINUTE);
        #if( SEL_DLT645_2007 == YES )
        api_Report645MinTask( );
        #endif
	}
	
	// 小时变化
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_HOUR);
		//上报小时任务
		ReportHourTask( );
	}
	
	
}

// 上电事件处理
// 掉电、全失压处理
void api_PowerUpEvent(void)
{
	BYTE i,Buf[((SUB_EVENT_INDEX_MAX_NUM+7)/8)+20];
	WORD Len;
	DWORD dwAddr,PowerOffTime,dwOad;
	TRealTimer t1;

	CLEAR_WATCH_DOG;

	//计算掉电时间
	api_GetPowerDownTime(&t1);
	PowerOffTime = api_CalcIntervalMinute( (TRealTimer *)&RealTimer, (TRealTimer *)&t1 ); 
	
	//写入电池工作时间
	api_ReadFromContinueEEPRom(	GET_CONTINUE_ADDR( BatteryTime ), sizeof(DWORD), (BYTE *)&dwAddr);
	dwAddr += PowerOffTime;//用dwAddr 暂存 电池工作时间
	api_WriteToContinueEEPRom(	GET_CONTINUE_ADDR( BatteryTime ), sizeof(DWORD), (BYTE *)&dwAddr);
	
	InitEventAllRam();

	// 初始化事件参数	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara );
	api_ReadFromContinueEEPRom( dwAddr, sizeof(TEventPara), (BYTE * )&gEventPara );	
	
	PowerUpReport();
	
	#if( SEL_EVENT_LOST_ALL_V == YES )
	PowerUpDealLostAllV();
	#endif//#if( SEL_EVENT_LOST_ALL_V == YES )

	#if( SEL_EVENT_LOST_POWER == YES )
	// 掉电事件的上电处理
	LostPowerPowerOn();
	#endif

	#if( SEL_EVENT_LOST_SECPOWER == YES )
	if( SelSecPowerConst == YES )
	// 置掉辅助电源失电状态
	{    
	    if( PowerDownLostSecPowerFlag == 0x5aa55aa5 )//掉电前处于辅助电源有电状态
	    {
            SaveInstantEventRecord( SubEventInfoTab[eSUB_EVENT_LOST_SECPOWER].EventIndex, SubEventInfoTab[eSUB_EVENT_LOST_SECPOWER].Phase, EVENT_START, eEVENT_ENDTIME_PWRDOWN );//记录开始
            PowerDownLostSecPowerFlag = 0x12341234;
	    }
		EventStatus.CurrentStatus[eSUB_EVENT_LOST_SECPOWER/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_SECPOWER%8));
		api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_SECPOWER );
	}
	#endif

	#if( SEL_EVENT_METERCOVER == YES )
	// 读取低功耗下得表盖状态位
	MeterCoverPowerOn();
	#endif

	#if( SEL_EVENT_BUTTONCOVER == YES )
	// 读取低功耗下得端钮盖状态位
	ButtonCoverPowerOn();
	#endif
	
	#if( SEL_EVENT_RTC_ERR == YES )
	//时钟故障事件上电处理
	RtcErrPowerOn();
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	PowerUpDealPowerErr();
	#endif

	// 掉电前未结束事件，掉电上电后结束事件
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.ProgramPwrDwn );
	api_ReadFromContinueEEPRom( dwAddr, 1, (BYTE *)&i );

	if( i == 0xA5 )
	{
		dwOad = 0xFFFFFFFF;
		PrgFlag = ePROGRAM_STARTED;
		api_SaveProgramRecord( EVENT_END, 0x51, (BYTE *)&dwOad );
		i = 0x5A;
		api_WriteToContinueEEPRom( dwAddr, 1, (BYTE *)&i );
	}
	
	Len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] );
	api_ReadFromContinueEEPRom( dwAddr, Len, Buf );

	for( i = 0; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		//若掉电时的事件没有结束，则上电时结束
		if( Buf[i/8] & (0x01<<(i%8)) )
		{
			if(EventInfoTab[ SubEventInfoTab[i].EventIndex ].SpecialProc == NO)
			{
                #if( SEL_EVENT_RELAY_ERR == YES  )
				if(( api_DealRelayErrFlag(READ,0x0000) == 0xA000 )||( api_DealRelayErrFlag(READ,0x0000)== 0x0A00 ))
				{
					api_DealRelayErrFlag(WRITE,0x00AA);
				}
                #endif
				SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_PWRDOWN );
			}
		}
	}
	
	//处理完毕后清除暂存的事件掉电标志
	api_ClrContinueEEPRom( dwAddr, Len );
	
	#if(SEL_PRG_RECORD645 == YES)
	PrgFlag645 = ePROGRAM_NOT_YET_START;
	ProRecord645DiNum = 0;
	#endif
	
	#if(SEL_DEBUG_VERSION == YES)
	if( (eEventTypehch[1]==0xff) || (eSubEventTypehch[2]==0xff) )
	{
		CLEAR_WATCH_DOG;
	}
	#endif
}

// 掉电处理
// 掉电时，所有事件记录（除全失压、掉电外）都应该结束
// 1、若有全失压正在发生，则掉电时不再记录全失压开始
// 2、全失压与掉电为两个事件，全失压发生时不记录掉电事件
void api_PowerDownEvent(void)
{
	BYTE i,len;
	DWORD dwAddr;
	TLostPowerTime LostPowerTime;
	
	// 记录掉电时间
	memcpy( LostPowerTime.Time, (BYTE *)&RealTimer, sizeof(TRealTimer) );
	dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.LostPowerTime );
	api_VWriteSafeMem( dwAddr, sizeof(TLostPowerTime) , LostPowerTime.Time );

	// 全失压
	#if(SEL_EVENT_LOST_ALL_V == YES)//认为掉电都支持	
	PowerDownDealLostAllV();
	LostPowerPowerDown();
	#else	
	LostPowerPowerDown();		
	#endif//#if(SEL_EVENT_LOST_ALL_V == YES)
	
	PowerDownReport();

	#if( SEL_EVENT_METERCOVER == YES )
	MeterCoverPowerOff();
	#endif
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )
    LostSecPowerPowerOff();
    #endif

	#if( SEL_EVENT_BUTTONCOVER == YES )
	ButtonCoverPowerOff();
	#endif

	#if( (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
	UnBalanceRatePowerOff();
	#endif

	#if( SEL_STAT_V_RUN == YES )
	// 处理电压合格率转存
	SwapVRunTime();
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	wPowerErrFlag = 0x00000000;
	#endif

	// 保存当前的事件状态
	len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] );
	api_WriteToContinueEEPRom( dwAddr, len, (BYTE *)&EventStatus.DelayedStatus[0] );

	i = GetPrgRecord();
	if( i == ePROGRAM_STARTED )													// 编程记录还未结束
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.ProgramPwrDwn );
		i = 0xA5;	
		api_WriteToContinueEEPRom( dwAddr, 1, (BYTE *)&i );
	}
	
}

//-----------------------------------------------
//函数功能: 读写事件参数
//
//参数: 
//			Operation[in]:			WRITE/READ
//			inEventIndex[in]:		eEVENT_INDEX		
//			ParaType[in]:			eEVENT_PARA_TYPE
//			pBuf[out/in]:			读写数据的缓冲
//                    
//返回值:  	数据长度  0：代表错误 非0：正确
//
//备注:  	
//-----------------------------------------------
WORD DealEventParaByEventOrder( BYTE Operation, BYTE inEventIndex, BYTE ParaType, BYTE *pBuf )
{
	DWORD dwAddr;
	WORD wLen;
	BYTE *pTemp;

	wLen = 0;

	if( ParaType == eTYPE_BYTE )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.byLimit[inEventIndex] );	
		pTemp = (BYTE *)&gEventPara.byLimit[inEventIndex];
		wLen = 1;
	}
	else if( ParaType == eTYPE_WORD )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.wLimit[inEventIndex] );	
		pTemp = (BYTE *)&gEventPara.wLimit[inEventIndex];
		wLen = 2;
	}
	else if( ParaType == eTYPE_DWORD )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.dwLimit[inEventIndex] );	
		pTemp = (BYTE *)&gEventPara.dwLimit[inEventIndex];
		wLen = 4;
	}
	//vvvvvvvvvvvv特殊数据vvvvvvvvvvvvvvvvv
	else if( ParaType == eTYPE_SPEC1 )
	{
		#if( SEL_STAT_V_RUN == YES )	//电压合格率
		if( inEventIndex == eSTATISTIC_V_PASS_RATE_NO )
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.VRunLimitLV );	
			pTemp = (BYTE *)&gEventPara.VRunLimitLV;
			wLen = 2;
		}
		#endif

		#if( SEL_EVENT_LOST_V == YES )	//失压
		if( inEventIndex == eEVENT_LOST_V_NO )	
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.LostVRecoverLimitV );
			pTemp = (BYTE *)&gEventPara.LostVRecoverLimitV;
			wLen = 2;
		}
		#endif
		
		#if( SEL_EVENT_LOST_I == YES )	// 失流
		if( inEventIndex == eEVENT_LOST_I_NO )	
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.LostILimitLI );	
			pTemp = (BYTE *)&gEventPara.LostILimitLI;
			wLen = 4;
		}
		#endif

	}
	else if( ParaType == eTYPE_SPEC2 )
	{
		#if( SEL_STAT_V_RUN == YES )	//电压合格率
		if( inEventIndex == eSTATISTIC_V_PASS_RATE_NO )
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.VRunRecoverHV );	
			pTemp = (BYTE *)&gEventPara.VRunRecoverHV;
			wLen = 2;
		}
		#endif

	}
	else if( ParaType == eTYPE_SPEC3 )
	{
		#if( SEL_STAT_V_RUN == YES )	//电压合格率
		if( inEventIndex == eSTATISTIC_V_PASS_RATE_NO )
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.VRunRecoverLV );	
			pTemp = (BYTE *)&gEventPara.VRunRecoverLV;
			wLen = 2;
		}
		#endif
	}
	// 保证非法参数是返回
	if(wLen==0)
	{
		return 0;
	}

	if( Operation == WRITE )
	{
		api_WriteToContinueEEPRom( dwAddr, wLen, pBuf );
		memcpy( pTemp, pBuf, wLen );	// 更新RAM
	}
	else
	{
		memcpy( pBuf, pTemp, wLen );
	}

	return wLen;
}

//-----------------------------------------------
//函数功能: 读事件参数
//
//参数: 
//			OI[in]:					OI
//			No[in]:					0: 全部参数
//									N: 第N个参数
//			pLen[out]:				读出数据的长度
//			pBuf[out]:				读数据的缓冲
//                    
//返回值:  	TRUE   正确
//			FALSE  错误
//
//备注:  	读取ram  698规约调用
//-----------------------------------------------
BOOL api_ReadEventPara( WORD OI, BYTE No, BYTE *pLen, BYTE *pBuf )
{
	BYTE	i,Len,tEventIndex,AllLen;
	BYTE	*pTemp;
	const 	TEventInfoTab	*pParaTable;

	Len = 0;
	AllLen = 0;
	pTemp = pBuf;

	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	
	pParaTable = &EventInfoTab[tEventIndex];

	if( No == 0 )
	{
		for( i = 0; i < pParaTable->ParaNum; i++ )
		{
			Len = DealEventParaByEventOrder( READ, tEventIndex, pParaTable->ParaType[i], pTemp );

			if( Len == 0 )
			{
				return FALSE;	
			}
			pTemp += Len;
			AllLen += Len;
		}
	}
	else
	{
		if(pParaTable->ParaNum<No)
		{
		 	return FALSE;
		}
		
		Len = DealEventParaByEventOrder( READ, tEventIndex, pParaTable->ParaType[No-1], pTemp );
		
		if( Len == 0 )
		{
			return FALSE;
		}
		AllLen = Len;
	}

	*pLen = AllLen;
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 写事件参数
//
//参数: 
//			OI[in]:					OI
//			No[in]:					0: 全部参数
//									N: 第N个参数
//			pBuf[in]:				写数据的缓冲
//                    
//返回值:  	TRUE   正确
//			FALSE  错误
//
//备注:  	写EEP+RAM  698规约调用
//-----------------------------------------------
BOOL api_WriteEventPara( WORD OI, BYTE No, BYTE *pBuf )
{
	BYTE	i,Len,tEventIndex;
	BYTE	*pTemp;
	const 	TEventInfoTab	*pParaTable;

	Len = 0;
	pTemp = pBuf;

	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	
	pParaTable = &EventInfoTab[tEventIndex];

	if( No == 0 )
	{
		for( i = 0; i < pParaTable->ParaNum; i++ )
		{
			Len = DealEventParaByEventOrder( WRITE, tEventIndex, pParaTable->ParaType[i], pTemp );

			if( Len == 0 )
			{
				return FALSE;	
			}
			pTemp += Len;
		}
	}
	else
	{
		if(pParaTable->ParaNum<No)
		{
		 	return FALSE;
		}
		
		if( DealEventParaByEventOrder( WRITE, tEventIndex, pParaTable->ParaType[No-1], pTemp ) == 0 )
		{
			return FALSE;	
		}
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 初始化各事件记录的门限值
//
//参数: 无
//                   
//返回值:  无
//
//备注:   
//-----------------------------------------------
void FactoryInitParaEventLimit( void )
{
	DWORD dwTmp;

	memset( (BYTE *)&gEventPara, 0x00, sizeof(gEventPara) );
	
	//失压 0x3000
	#if( SEL_EVENT_LOST_V == YES )
	gEventPara.byLimit[eEVENT_LOST_V_NO] = EventLostVTimeConst;
	gEventPara.wLimit[eEVENT_LOST_V_NO] = (DWORD)wStandardVoltageConst * EventLostVURateConst / 100;//0.78Un
	gEventPara.dwLimit[eEVENT_LOST_V_NO] = (DWORD)wMeterBasicCurrentConst * EventLostVIRateConst / 100;//0.05Ib
	gEventPara.LostVRecoverLimitV = (DWORD)wStandardVoltageConst * EventLostVRecoverLimitVRateConst / 100;//0.85Un
	#endif

	//欠压 0x3001
	#if( SEL_EVENT_WEAK_V == YES )
	gEventPara.byLimit[eEVENT_WEAK_V_NO] = EventWeakVTimeConst;
	gEventPara.wLimit[eEVENT_WEAK_V_NO] = (DWORD)wStandardVoltageConst * EventWeakVURateConst / 100; //0.78Un
	#endif

	//过压 0x3002
	#if( SEL_EVENT_OVER_V == YES )
	gEventPara.byLimit[eEVENT_OVER_V_NO] = EventOverVTimeConst;
	gEventPara.wLimit[eEVENT_OVER_V_NO] = (DWORD)wStandardVoltageConst * EventOverVURateConst / 100;//1.2Un
	#endif

	//断相 0x3003
	#if( SEL_EVENT_BREAK == YES )
	gEventPara.byLimit[eEVENT_BREAK_NO] = EventBreakTimeConst;
	gEventPara.wLimit[eEVENT_BREAK_NO] = (DWORD)wStandardVoltageConst * EventBreakURateConst / 100; //0.6Un
	gEventPara.dwLimit[eEVENT_BREAK_NO] = (DWORD)wMeterBasicCurrentConst * EventBreakIRateConst / 100; //0.005Ib
	#endif

	//失流 0x3004
	#if( SEL_EVENT_LOST_I == YES )
	gEventPara.byLimit[eEVENT_LOST_I_NO] = EventLostITimeConst;
	gEventPara.wLimit[eEVENT_LOST_I_NO] = (DWORD)wStandardVoltageConst * EventLostIURateConst / 100;	//0.7Un
	gEventPara.dwLimit[eEVENT_LOST_I_NO] = (DWORD)wMeterBasicCurrentConst * EventLostIIRateConst / 100;	//0.005Ib
	gEventPara.LostILimitLI = (DWORD)wMeterBasicCurrentConst * EventLostILimitLIConst / 10; //5%额定电流 四位小数
	#endif

	//过流 0x3005 IMax本身保留了3位小数，过流门限值要求4位小数
	#if( SEL_EVENT_OVER_I == YES )
	gEventPara.byLimit[eEVENT_OVER_I_NO] = EventOverITimeConst;
	gEventPara.dwLimit[eEVENT_OVER_I_NO] = (DWORD)dwMeterMaxCurrentConst * EventOverIIRateConst; //1.2Imax
	#endif

	//断流 0x3006
	#if( SEL_EVENT_BREAK_I == YES )
	gEventPara.byLimit[eEVENT_BREAK_I_NO] = EventBreakITimeConst;
	gEventPara.wLimit[eEVENT_BREAK_I_NO] = (DWORD)wStandardVoltageConst * EventBreakIURateConst / 100;    //0.6Un
	gEventPara.dwLimit[eEVENT_BREAK_I_NO] = (DWORD)wMeterBasicCurrentConst * EventBreakIIRateConst / 100; //0.005Ib
	#endif

	//功率反向 0x3007
	#if( SEL_EVENT_BACKPROP == YES )
	gEventPara.byLimit[eEVENT_BACKPROP_NO] = EventBackpTimeConst;
	gEventPara.dwLimit[eEVENT_BACKPROP_NO] = (DWORD)wStandardVoltageConst * wMeterBasicCurrentConst * EventBackpPRateConst / 1000000L; //0.5%基本功率
	#endif

	//过载 0x3008
	#if( SEL_EVENT_OVERLOAD == YES )
	gEventPara.byLimit[eEVENT_OVERLOAD_NO] = EventOverLoadTimeConst;
	gEventPara.dwLimit[eEVENT_OVERLOAD_NO] = (DWORD)wStandardVoltageConst * dwMeterMaxCurrentConst * EventOverLoadPRateConst / 10000L; //1.2最大功率
	#endif

	//0x3009	电能表正向有功需量超限事件
	//0x300a	电能表反向有功需量超限事件
	//0x300b	电能表无功需量超限事件
	#if( SEL_DEMAND_OVER == YES )
	gEventPara.byLimit[eEVENT_PA_DEMAND_OVER_NO] = EventDemandOverTimeConst;
	gEventPara.byLimit[eEVENT_NA_DEMAND_OVER_NO] = EventDemandOverTimeConst;
	gEventPara.byLimit[eEVENT_RE_DEMAND_OVER_NO] = EventDemandOverTimeConst;
	dwTmp = (DWORD)(wStandardVoltageConst * dwMeterMaxCurrentConst * EventDemandOverRateConst) / 1000; //单相Un*1.2Imax 1位小数，单位w，此处放大10倍，后面除以10
	if( MeterTypesConst == METER_3P3W )
	{
		dwTmp = dwTmp * 1732 / 1000 / 10;
	}
	else
	{
		dwTmp = dwTmp * 3 / 10;
	}
	gEventPara.dwLimit[eEVENT_PA_DEMAND_OVER_NO] = dwTmp;
	gEventPara.dwLimit[eEVENT_NA_DEMAND_OVER_NO] = dwTmp;
	gEventPara.dwLimit[eEVENT_RE_DEMAND_OVER_NO] = dwTmp;
	#endif

	//功率因数超限 0x303B
	#if( SEL_EVENT_COS_OVER == YES )
	gEventPara.byLimit[eEVENT_COS_OVER_NO] = EventCosOverTimeConst;//60秒
	gEventPara.wLimit[eEVENT_COS_OVER_NO] = EventCosOverRateConst;//0.3功率因数
	#endif

	//全失压 0x300d
	#if( SEL_EVENT_LOST_ALL_V == YES )
	gEventPara.byLimit[eEVENT_LOST_ALL_V_NO] = EventLostAllVTimeConst;
	#endif
	
	//辅助电源掉电 0x300e
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	gEventPara.byLimit[eEVENT_LOST_SECPOWER_NO] = EventLostSecPowerTimeConst;
	#endif

	//电压逆相序 0x300f
	#if( SEL_EVENT_V_REVERSAL == YES )
	gEventPara.byLimit[eEVENT_V_REVERSAL_NO] = EventVReversalTimeConst;
	#endif
	
	//电流逆相序 0x3010
	#if( SEL_EVENT_I_REVERSAL == YES )
	gEventPara.byLimit[eEVENT_I_REVERSAL_NO] = EventIReversalTimeConst;
	#endif

	//掉电 0x3011
	#if( SEL_EVENT_LOST_POWER == YES )
	gEventPara.byLimit[eEVENT_LOST_POWER_NO] = EventLostPowerTimeConst;
	#endif
	
	
	//电压不平衡 0x301d
	#if( SEL_EVENT_V_UNBALANCE == YES )
	gEventPara.byLimit[eEVENT_V_UNBALANCE_NO] = EventVUnbalanceTimeConst;
	gEventPara.wLimit[eEVENT_V_UNBALANCE_NO] = EventVUnbalanceRateConst;
	#endif

	//电流不平衡 0x301e
	#if( SEL_EVENT_I_UNBALANCE == YES )
	gEventPara.byLimit[eEVENT_I_UNBALANCE_NO] = EventIUnbalanceTimeConst;
	gEventPara.wLimit[eEVENT_I_UNBALANCE_NO] = EventIUnbalanceRateConst;
	#endif

	//电流严重不平衡 0x302d
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	gEventPara.byLimit[eEVENT_I_S_UNBALANCE_NO] = EventISUnbalanceTimeConst;
	gEventPara.wLimit[eEVENT_I_S_UNBALANCE_NO] = EventISUnbalanceRateConst;
	#endif

	//计量芯片故障
	#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
	gEventPara.byLimit[eEVENT_SAMPLECHIP_ERR_NO] = EventSampleChipErrTimeConst;
	#endif

	//零线电流异常
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	gEventPara.byLimit[eEVENT_NEUTRAL_CURRENT_ERR_NO] = EventNeutralCurrentErrTimeConst;
	gEventPara.dwLimit[eEVENT_NEUTRAL_CURRENT_ERR_NO] = wMeterBasicCurrentConst * EventNeutralCurrentErrLimitIConst / 10000;//20%,电流触发下限  double-long（单位：A，换算：-4），
	gEventPara.wLimit[eEVENT_NEUTRAL_CURRENT_ERR_NO] = EventNeutralCurrentErrRatioConst;
	
	#endif

	//0x4030
	#if( SEL_STAT_V_RUN == YES )//电压合格率
	gEventPara.wLimit[eSTATISTIC_V_PASS_RATE_NO] = wStandardVoltageConst * EventStatVRateConst/10;//1.2Un
	gEventPara.VRunLimitLV = (DWORD)wStandardVoltageConst * EventStatVRunLimitLVConst / 100; //0.7Un
	gEventPara.VRunRecoverHV = (DWORD)wStandardVoltageConst * EventStatVRunRecoverHVConst / 100; //1.07Un
	gEventPara.VRunRecoverLV = (DWORD)wStandardVoltageConst * EventStatVRunRecoverLVConst / 100; //0.93Un
	#endif

	//把参数写入到eeprom 一定要放在最后!!!!!!
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&gEventPara );

}

//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_FactoryInitEvent( void )
{
	//电压合格率参数
	#if( SEL_STAT_V_RUN == YES )
	FactoryInitStatisticsVPara();
	#endif
	
	#if((SEL_EVENT_LOST_V==YES)||(SEL_EVENT_WEAK_V==YES)||(SEL_EVENT_OVER_V==YES)||(SEL_EVENT_BREAK==YES)||(SEL_EVENT_LOST_I==YES)||(SEL_EVENT_OVER_I==YES)||(SEL_EVENT_BREAK_I==YES))
	FactoryInitVICorrelatePara();
	#endif
	
	#if( SEL_EVENT_BACKPROP == YES )
	FactoryInitBackPopPara();
	#endif

	#if( SEL_EVENT_OVERLOAD == YES )
	FactoryInitOverLoadPara();
	#endif

	#if( SEL_EVENT_COS_OVER == YES )
	FactoryInitCosOverPara();
	#endif

	#if( SEL_EVENT_MAGNETIC_INT == YES )
	FactoryInitMagneticPara();
	#endif

	#if( SEL_EVENT_RELAY_ERR == YES )
	FactoryInitRelayErrPara();
	#endif
	
	#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
	FactoryInitReversalUnBalancePara();
	#endif

	// 掉电和辅助电源失电参数
	#if( (SEL_EVENT_LOST_SECPOWER == YES) || (SEL_EVENT_LOST_POWER == YES) )
	FactoryInitLostPowerPara();
	#endif
	
	#if( SEL_EVENT_LOST_ALL_V == YES )
	FactoryInitLostAllVPara();
	#endif
	
	#if((SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES))
	FactoryInitCoverPara();
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	FactoryInitDemandOverPara();
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	FactoryInitPowerErrPara();
	#endif

	#if( SEL_EVENT_RTC_ERR == YES )
	api_FactoryInitRtcErrPara();
	#endif

	#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
	FactoryInitSampleChipErrPara();
	#endif

	//零线电流异常
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	FactoryInitNeutralCurrentErrPara();
	#endif
	
	FactoryInitPrgRecordPara();

	// 主动上报标识@@
	FactoryInitReport();
	
	FactoryInitParaEventLimit();
}

// #endif // #if( MAX_EVENT_NUM != 0 )
