//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.10.8
//描述		事件模块内部头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __EVENT_H
#define __EVENT_H
//--------------------------------------

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define FLASH_EVENT_BASE			0x00000000L
#define EVENT_RECORD_LEN			(SECTOR_SIZE*3)	//一个事件的预留存储区域大小
#define EVENT_PHASE_FOUR			4	// 潮流反向，无功需量，功率因数超限为4相
#define MAX_EVENTRECORD_NUMBER		10	// 事件记录，编程记录最多支持10次记录
#define MAX_PROGRAM_OAD_NUMBER		10	// 一次编程记录最多支持10个OAD
#define EVENT_MAX_PARA_NUM			4	// 一个事件最多4个参数
#define EVENT_LOSTPOWERRECORD_NUM	100	// 掉电事件最多支持10次记录
#define EVENT_ROGOWSKI_CHANGE_NUM	20	// 互感器线圈变更事件最多支持20次记录
#define EVENT_ROGOWSKI_ERR_NUM		20	// 互感器线圈错误事件最多支持20次记录
#define EVENT_FIND_UNKNOWN_METER_NUM 16
#define EVENT_TOPU_NUM				 250    // 拓扑事件个数
#define EVENT_TOPU_SIGNAL_NUM        1000   // 拓扑信号个数

#define	EVENT_START					BIT0//事件发生
#define	EVENT_END					BIT1//事件结束

// 所有OAD对应的数据总长度的最大值
#define EVENT_VALID_OADLEN			2048				
// 单独OAD对应的数据长度的最大值
#define EVENT_ALONE_OADLEN			254

//拓扑事件数据占用的空间
#if( SEL_TOPOLOGY == YES )
#define EVENT_RECORD_TOPO_LEN               (SECTOR_SIZE*20)
#define EVENT_RECORD_TOPO_SIGNAL_LEN        (SECTOR_SIZE*20)
#define EVENT_RECORD_EXT_LEN                (EVENT_RECORD_TOPO_LEN + EVENT_RECORD_TOPO_SIGNAL_LEN)//用于特殊事件的存放
#else
#define EVENT_RECORD_EXT_LEN        (0)//用于特殊事件的存放
#endif


//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
// 事件清零类型
typedef enum
{
	eFACTORY_INIT = 0,					// 清所有事件，包含电表清零记录，事件清零记录
	eEXCEPT_CLEAR_METER,				// 清所有事件，不包含电表清零记录
	eEXCEPT_CLEAR_METER_CLEAR_EVENT		// 清所有事件，不包含电表清零记录，事件清零记录
}eEVENT_CLEAR_TYPE;

// 事件记录数类型
typedef enum
{
	eEVENT_CURRENT_RECORD_NO = 0,	// 当前记录数
	eEVENT_MAX_RECORD_NO			// 最大记录数
}eEVENT_RECORD_NO_TYPE;

// 获得，设置，清零事件状态枚举类型
typedef enum
{
	eCLEAR_EVENT_STATUS = 0,
	eSET_EVENT_STATUS,
	eGET_EVENT_STATUS,
	eCLEAR_EVENT_STATUS_CURR = eCLEAR_EVENT_STATUS + 0x80,
	eSET_EVENT_STATUS_CURR   = eSET_EVENT_STATUS + 0x80,
	eGET_EVENT_STATUS_CURR   = eGET_EVENT_STATUS + 0x80,
}eDEAL_EVENT_STATUS;	


typedef enum
{
	eEVENT_ENDTIME_CURRENT = 0, // 当前时间
	eEVENT_ENDTIME_PWRDOWN		// 掉电时间
}eEVENT_ENDTIME_TYPE;

// 编程记录状态
typedef enum
{
	ePROGRAM_NOT_YET_START = 0,	// 编程还未开始
	ePROGRAM_STARTED			// 编程已经开始
}ePROGRAM_STATUS;

// 事件门限参数类型
typedef enum
{
	eTYPE_BYTE = 0,	// BYTE类型参数，延时
	eTYPE_WORD,		// WORD类型参数
	eTYPE_DWORD,	// DWORD类型参数
	eTYPE_SPEC1,	// 特殊类型参数1
	eTYPE_SPEC2,	// 特殊类型参数2
	eTYPE_SPEC3,	// 特殊类型参数3
	eTYPE_NULL,		// 参数无效
}eEVENT_PARA_TYPE;

//此定义应与SubEventInfoTab定义顺序一致，与eEVENT_INDEX顺序一致
typedef enum
{	//失压
	#if ( SEL_EVENT_LOST_V == YES )//0
	eSUB_EVENT_LOSTV_A = 0,
	eSUB_EVENT_LOSTV_B,
	eSUB_EVENT_LOSTV_C,
	#endif
		
	//欠压
	#if( SEL_EVENT_WEAK_V == YES )//3
	eSUB_EVENT_WEAK_V_A,
	eSUB_EVENT_WEAK_V_B,
	eSUB_EVENT_WEAK_V_C,
	#endif
	
	//过压
	#if( SEL_EVENT_OVER_V == YES )//6
	eSUB_EVENT_OVER_V_A,
	eSUB_EVENT_OVER_V_B,
	eSUB_EVENT_OVER_V_C,
	#endif
	
	//断相
	#if( SEL_EVENT_BREAK == YES )//9
	eSUB_EVENT_BREAK_A,
	eSUB_EVENT_BREAK_B,
	eSUB_EVENT_BREAK_C,
	#endif
	
	//失流
	#if( SEL_EVENT_LOST_I == YES )//12
	eSUB_EVENT_LOSTI_A,
	eSUB_EVENT_LOSTI_B,
	eSUB_EVENT_LOSTI_C,
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
	#endif
	
	//断流
	#if( SEL_EVENT_BREAK_I == YES )//18
	eSUB_EVENT_BREAK_I_A,
	eSUB_EVENT_BREAK_I_B,
	eSUB_EVENT_BREAK_I_C,
	#endif
		
	//功率反向
	#if( SEL_EVENT_BACKPROP == YES )//21
	eSUB_EVENT_POW_BACK_ALL,
	eSUB_EVENT_POW_BACK_A,
	eSUB_EVENT_POW_BACK_B,
	eSUB_EVENT_POW_BACK_C,
	#endif
	
	//过载
	#if( SEL_EVENT_OVERLOAD == YES )//25
	eSUB_EVENT_OVERLOAD_A,
	eSUB_EVENT_OVERLOAD_B,
	eSUB_EVENT_OVERLOAD_C,
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
	#endif
	
	//功率因数超限
	#if( SEL_EVENT_COS_OVER == YES )//34
	eSUB_EVENT_COS_OVER_ALL,
	eSUB_EVENT_COS_OVER_A,
	eSUB_EVENT_COS_OVER_B,
	eSUB_EVENT_COS_OVER_C,
	#endif	
	
	//全失压
	#if( SEL_EVENT_LOST_ALL_V == YES )//38
	eSUB_EVENT_LOST_ALL_V,
	#endif
	
	//辅助电源失电事件
	#if( SEL_EVENT_LOST_SECPOWER == YES )//39
	eSUB_EVENT_LOST_SECPOWER,
	#endif
	
	//电压逆相序
	#if( SEL_EVENT_V_REVERSAL == YES )//40
	eSUB_EVENT_V_REVERSAL,
	#endif
	
	//电流逆相序
	#if( SEL_EVENT_I_REVERSAL == YES )//41
	eSUB_EVENT_I_REVERSAL,
	#endif

	//掉电事件
	#if(SEL_EVENT_LOST_POWER == YES)//42
	eSUB_EVENT_LOST_POWER,			//单相 1
	#endif
	
	//开表盖事件
	#if(SEL_EVENT_METERCOVER == YES)//43
	eSUB_EVENT_METER_COVER,			//单相 2
	#endif
	
	//开端钮盖事件
	#if(SEL_EVENT_BUTTONCOVER == YES)//44
	eSUB_EVENT_BUTTON_COVER,
	#endif
		
	//电压不平衡
	#if(SEL_EVENT_V_UNBALANCE == YES)//45
	eSUB_EVENT_V_UNBALANCE,
	#endif
	
	//电流不平衡
	#if(SEL_EVENT_I_UNBALANCE == YES)//46
	eSUB_EVENT_I_UNBALANCE,
	#endif
	
	//恒定磁场干扰
	#if(SEL_EVENT_MAGNETIC_INT == YES)//47
	eSUB_EVENT_MAGNETIC_INT,
	#endif
	
	//负荷开关误动作
	#if(SEL_EVENT_RELAY_ERR == YES)	//48
	eSUB_EVENT_RELAY_ERR,			//单相 3
	#endif
	
	//电源异常
	#if(SEL_EVENT_POWER_ERR == YES)	//49
	eSUB_EVENT_POWER_ERR,			//单相 4
	#endif
	
	//电流严重不平衡
	#if(SEL_EVENT_I_S_UNBALANCE == YES)//50
	eSUB_EVENT_S_I_UNBALANCE,		
	#endif
	
	//时钟故障
	#if(SEL_EVENT_RTC_ERR == YES)	//51
	eSUB_EVENT_RTC_ERR,				//单相 5
	#endif
	
	// 计量芯片故障
	#if(SEL_EVENT_SAMPLECHIP_ERR == YES)//52
	eSUB_EVENT_SAMPLECHIP_ERR,		//单相 6
	#endif

	// 电能表零线电流异常
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )//53
	eSUB_NEUTRAL_CURRENT_ERR_NO,//单相 7 		
	#endif

	//终端状态量变位
	#if( SEL_EVENT_DI_CHANGE == YES )//54
	eSUB_EVENT_DI_CHANGE,//单相 8
	#endif
	
	eSUB_EVENT_CHARGING_WARNING,//告警
	eSUB_EVENT_CHARGING_ERR,//故障
	eSUB_EVENT_CHARGING_COM_EXCTION,//通信异常

	// 罗氏线圈变更事件
	#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )
	eSUB_ROGOWSKI_CHANGE,              
	#endif

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    eSUB_FIND_UNKNOWN_METER,            //发现未知电能表事件
    #endif

	//终端抄表失败事件
	#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
	eSUB_ROGOWSKI_COMM_FAIL,              
	#endif

	//拓扑事件
	#if( SEL_TOPOLOGY == YES )
	eSUB_EVENT_TOPU,  
    eSUB_EVENT_TOPU_SIGNAL, 
	#endif

	#if( SEL_PRG_MODULE == YES )
    eSUB_EVENT_MODULE_0,
	eSUB_EVENT_MODULE_1,
	eSUB_EVENT_MODULE_2,
	eSUB_EVENT_MODULE_3,
	eSUB_EVENT_MODULE_4,
	eSUB_EVENT_MODULE_5,
	eSUB_EVENT_MODULE_6,
	eSUB_EVENT_MODULE_7,
	eSUB_EVENT_MODULE_8,
	eSUB_EVENT_MODULE_9,
	#endif
	//电能表编程事件//54
	eSUB_EVENT_PRG_RECORD,			//单相 8
	
	//电能表清零事件
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//55		
	eSUB_EVENT_PRG_CLEAR_METER,		//单相 9		
	#endif
	
	//电能表需量清零事件	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )//56		
	eSUB_EVENT_PRG_CLEAR_MD,				
	#endif
											
	//电能表事件清零事件	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//57	
	eSUB_EVENT_PRG_CLEAR_EVENT,			//单相 10		
	#endif
	
	//电能表校时事件		
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//58	
	eSUB_EVENT_PRG_ADJUST_TIME,			//单相 11		
	#endif
	
	//电能表时段表编程事件	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )//59	
	eSUB_EVENT_PRG_TIME_TABLE,			//单相 12			
	#endif
	
	//电能表时区表编程事件	
	#if( SEL_PRG_INFO_TIME_AREA == YES )//60	
	eSUB_EVENT_PRG_TIME_AREA,			//单相 13			
	#endif
	
	//电能表周休日编程事件	
	#if( SEL_PRG_INFO_WEEK == YES )//61		
	eSUB_EVENT_PRG_WEEKEND,				//单相 14	
	#endif
	
	//电能表结算日编程事件	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//62	
	eSUB_EVENT_PRG_CLOSING_DAY,			//单相 15			
	#endif
	
	//电能表跳闸合闸事件			
	eSUB_EVENT_PRG_OPEN_RELAY,		//63单相 16			
	eSUB_EVENT_PRG_CLOSE_RELAY,		//64单相 17		
	
	//节假日编程记录		
	#if( SEL_PRG_INFO_HOLIDAY == YES )//65		
	eSUB_EVENT_PRG_HOLIDAY,			//单相 18		
	#endif
	
	//有功组合方式编程记录	
	#if( SEL_PRG_INFO_P_COMBO == YES )//66		
	eSUB_EVENT_ePRG_P_COMBO,		//单相 19				
	#endif
	
	//无功组合方式编程记录	
	#if( SEL_PRG_INFO_Q_COMBO == YES )//67		
	eSUB_EVENT_PRG_Q_COMBO,			
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	eSUB_TARIFF_TABLE_NO,       //68单相 20       //电能表费率参数表编程事件
	eSUB_LADDER_TABLE_NO,       //69单相 21       // 电能表阶梯表编程事件
	#endif

	//电能表密钥更新事件
	#if( SEL_PRG_UPDATE_KEY == YES )//70
	eSUB_EVENT_PRG_UPDATE_KEY,  //单相 22    
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	eSUB_ABR_CARD_NO,			//71单相 23        //电能表异常插卡事件
	eSUB_BUY_MONEY_NO,			//72单相 24        //电能表购电记录
	eSUB_RETURN_MONEY_NO,		//73单相 25        //电能表退费记录
	#endif
	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )
	eSUB_EVENT_BROADJUST_TIME,//74单相 26		//广播校时事件
	#endif
	
	eSUB_EVENT_STATUS_TOTAL             //最大值72 单相27

}eSUB_EVENT_INDEX;

//此定义应与EventInfoTab定义顺序一致，与eSUB_EVENT_INDEX顺序一致
typedef enum
{
	// vvvvvvvvvvv工况类记录vvvvvvvvvvvvvv
	#if ( SEL_EVENT_LOST_V == YES )
	eEVENT_LOST_V_NO,						// 失压*3
	#endif
	
	#if( SEL_EVENT_WEAK_V == YES )
	eEVENT_WEAK_V_NO,						// 欠压*3
	#endif
	
	#if( SEL_EVENT_OVER_V == YES )
	eEVENT_OVER_V_NO,						// 过压*3
	#endif
	
	#if( SEL_EVENT_BREAK == YES )
	eEVENT_BREAK_NO,						// 断相*3
	#endif	
	
	#if( SEL_EVENT_LOST_I == YES )
	eEVENT_LOST_I_NO,						// 失流*3
	#endif
	
	#if( SEL_EVENT_OVER_I == YES )
	eEVENT_OVER_I_NO,						// 过流*3
	#endif
	
	#if( SEL_EVENT_BREAK_I == YES )
	eEVENT_BREAK_I_NO,						// 断流*3
	#endif
	
	#if( SEL_EVENT_BACKPROP == YES )
	eEVENT_BACKPROP_NO,						// 功率反向*4
	#endif
	
	#if( SEL_EVENT_OVERLOAD == YES )
	eEVENT_OVERLOAD_NO,						// 过载*3
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	eEVENT_PA_DEMAND_OVER_NO,				//正向有功超限*1
	eEVENT_NA_DEMAND_OVER_NO,				//反向有功超限*1
	eEVENT_RE_DEMAND_OVER_NO,				//无功超限*4
	#endif
	
	#if( SEL_EVENT_COS_OVER == YES )
	eEVENT_COS_OVER_NO,						// 功率因数超下限*1
	#endif
	
	#if( SEL_EVENT_LOST_ALL_V == YES )
	eEVENT_LOST_ALL_V_NO,					// 全失压*1
	#endif	
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	eEVENT_LOST_SECPOWER_NO,				//  辅助电源失电*1
	#endif	
	
	#if( SEL_EVENT_V_REVERSAL == YES )
	eEVENT_V_REVERSAL_NO,					// 电压逆相序*1
	#endif
	
	#if( SEL_EVENT_I_REVERSAL == YES )
	eEVENT_I_REVERSAL_NO,					// 电流逆相序*1
	#endif
	
	#if( SEL_EVENT_LOST_POWER == YES )
	eEVENT_LOST_POWER_NO,					// 掉电*1
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )
	eEVENT_METERCOVER_NO,					// 开表盖*1
	#endif
	
	#if( SEL_EVENT_BUTTONCOVER == YES )
	eEVENT_BUTTONCOVER_NO,					// 开端钮盒*1
	#endif	
	
	#if( SEL_EVENT_V_UNBALANCE == YES )
	eEVENT_V_UNBALANCE_NO,					// 电压不平衡*1
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	eEVENT_I_UNBALANCE_NO,					// 电流不平衡*1
	#endif			
	
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	eEVENT_MAGNETIC_INT_NO,					// 恒定磁场干扰*1
	#endif
	
	#if( SEL_EVENT_RELAY_ERR == YES )
	eEVENT_RELAY_ERR_NO,					// 负荷开关误动*1
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	eEVENT_POWER_ERR_NO,					// 电源异常*1
	#endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	eEVENT_I_S_UNBALANCE_NO,				// 电流严重不平衡*1
	#endif

	#if(SEL_EVENT_RTC_ERR == YES)
	eEVENT_RTC_ERR_NO,						//时钟故障*1
	#endif
	
	#if(SEL_EVENT_SAMPLECHIP_ERR == YES)
	eEVENT_SAMPLECHIP_ERR_NO,				// 计量芯片故障*1
	#endif
	
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	eEVENT_NEUTRAL_CURRENT_ERR_NO,              // 电能表零线电流异常
	#endif
	
	#if( SEL_EVENT_DI_CHANGE == YES )
	eEVENT_DI_CHANGE_NO,						//终端状态量变位事件
	#endif
	
	eEVENT_CHARGING_WARNING_NO,//告警
	eEVENT_CHARGING_ERR_NO,//故障
	eEVENT_CHARGING_COM_EXCTION_NO,//通信异常
	
	#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )
	eEVENT_ROGOWSKI_CHANGE_NO,              // 罗氏线圈变更事件
	#endif

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    eEVENT_FIND_UNKNOWN_METER_NO,           //发现未知电能表事件
    #endif
	//终端抄表失败事件
	#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
	eSUB_ROGOWSKI_COMM_FAIL_NO,              
	#endif

	#if( SEL_TOPOLOGY == YES )
	eEVENT_TOPU_NO,   
    eEVENT_TOPU_SIGNAL_NO,          
	#endif

	#if( SEL_PRG_MODULE == YES )
    eSUB_EVENT_MODULE_0_NO,
	eSUB_EVENT_MODULE_1_NO,
	eSUB_EVENT_MODULE_2_NO,
	eSUB_EVENT_MODULE_3_NO,
	eSUB_EVENT_MODULE_4_NO,
	eSUB_EVENT_MODULE_5_NO,
	eSUB_EVENT_MODULE_6_NO,
	eSUB_EVENT_MODULE_7_NO,
	eSUB_EVENT_MODULE_8_NO,
	eSUB_EVENT_MODULE_9_NO,
	#endif
	eNUM_OF_EVENT,							// 工况类的事件数量28

	// vvvvvvvvvvv编程类记录vvvvvvvvvvvvvv			
	ePRG_RECORD_NO = eNUM_OF_EVENT,				// 电能表编程事件

	#if( SEL_PRG_INFO_CLEAR_METER == YES )
	ePRG_CLEAR_METER_NO,						// 清零记录
	#endif	
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )
	ePRG_CLEAR_MD_NO,							// 清需量记录
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
	ePRG_CLEAR_EVENT_NO,						// 清事件记录
	#endif
	
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )
	ePRG_ADJUST_TIME_NO,						// 校时记录
	#endif
	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )
	ePRG_TIME_TABLE_NO,							// 时段表编程记录
	#endif
	
	#if( SEL_PRG_INFO_TIME_AREA == YES )
	ePRG_TIME_AREA_NO,							// 时区表编程记录
	#endif
	
	#if( SEL_PRG_INFO_WEEK == YES )
	ePRG_WEEKEND_NO,							// 周休日编程记录
	#endif
	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )
	ePRG_CLOSING_DAY_NO,						// 结算日编程记录
	#endif
	
	ePRG_OPEN_RELAY_NO,							// 拉闸
	ePRG_CLOSE_RELAY_NO,						// 合闸
	
	#if( SEL_PRG_INFO_HOLIDAY == YES )
	ePRG_HOLIDAY_NO,							// 节假日编程记录
	#endif
	
	#if( SEL_PRG_INFO_P_COMBO == YES )
	ePRG_P_COMBO_NO,							// 有功组合方式编程记录
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )
	ePRG_Q_COMBO_NO,							// 无功组合方式编程记录
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	ePRG_TARIFF_TABLE_NO,                       //电能表费率参数表编程事件
	ePRG_LADDER_TABLE_NO,                       // 电能表阶梯表编程事件
	#endif

	#if( SEL_PRG_UPDATE_KEY == YES )
	ePRG_UPDATE_KEY_NO,                         // 密钥更新记录
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	ePRG_ABR_CARD_NO,                           //电能表异常插卡事件
	ePRG_BUY_MONEY_NO,                          //电能表购电记录
	ePRG_RETURN_MONEY_NO,                       //电能表退费记录
	#endif
	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )
	ePRG_BROADJUST_TIME_NO,						// 广播校时记录
	#endif
	
	eNUM_OF_EVENT_PRG,                          // 事件和编程类总数42

	// vvvvvvvvvvv之后的事件不存储记录vvvvvvvvvvvvvv
	#if( SEL_STAT_V_RUN == YES )
	eSTATISTIC_V_PASS_RATE_NO = eNUM_OF_EVENT_PRG,
	#endif

	eNUM_OF_EVENT_ALL,							// 所有的事件总数，包括不存储记录的44
	
}eEVENT_INDEX;

#define MAX_EVENT_NUM 			eNUM_OF_EVENT

#define SUB_EVENT_INDEX_MAX_NUM	eSUB_EVENT_STATUS_TOTAL		// 事件状态位数量
typedef struct TEventStatus_t
{
	BYTE CurrentStatus[(SUB_EVENT_INDEX_MAX_NUM+7)/8];	// 事件实时状态
	BYTE DelayedStatus[(SUB_EVENT_INDEX_MAX_NUM+7)/8]; // 经过延时后的状态
}TEventStatus;

#pragma pack(1)
// 时间数据
typedef struct TEventTime_t
{
	// 发生时刻
	TRealTimer BeginTime;
	// 结束时刻
	TRealTimer EndTime;

}TEventTime;

typedef struct TLostPowerTime_t
{
	BYTE	Time[7];		// 掉电时间
	DWORD	CRC32;
}TLostPowerTime;

#pragma pack()

typedef struct TEventOADCommonData_t
{
	DWORD			EventNo;					//事件记录序号@@
	TEventTime		EventTime;					//事件发生和结束时间
}TEventOADCommonData;

typedef struct TEventDataInfo_t
{
	DWORD	RecordNo;						//事件记录序号(单调递增)
	DWORD	AccTime;						//事件累计时间(只有工况类事件有效)
	DWORD	CRC32;
}TEventDataInfo;


typedef struct TEventAddrLen_t
{
	DWORD	dwAttInfoEeAddr;
	DWORD	dwDataInfoEeAddr;
	DWORD	dwRecordAddr;					//事件数据存储基地址，第三存储空间
	BYTE	EventIndex;
	BYTE	Phase;
	BYTE	SubEventIndex;
}TEventAddrLen;
// 每次计算偏移地址用
typedef struct TEventSectorInfo_t
{		
	DWORD		dwPointer;		// 点指针，当前NO对深度的取余
	DWORD		dwSectorAddr;			// 寻找点地址	
	DWORD		dwSaveSpace;	// 此事件需要用的扇区个数
}TEventSectorInfo;

typedef struct TSubEventInfoTab_t
{
	eSUB_EVENT_INDEX	SubEventIndex;
	eEVENT_INDEX		EventIndex;	
	ePHASE_TYPE			Phase;
	BYTE 	(*pGetEventStatus)( BYTE Phase );	// 获取事件状态
}TSubEventInfoTab;

typedef struct TEventInfoTab_t
{
	WORD	OI;
	BYTE 	EventIndex;							//Event类型
	BYTE 	NumofPhase;							//相位数  潮流反向，无功需量超限为4相
	BYTE	SpecialProc;						//掉电特殊处理
	WORD	ExtDataLen;							//扩展数据长度 如事件清零记录的OMD
	BYTE 	ParaNum;							//参数的个数
	BYTE 	ParaType[EVENT_MAX_PARA_NUM];		//参数的类型
}TEventInfoTab;

// vvvvvvvvvvvvvvvvvv用来分配最大数据存储空间vvvvvvvvvvvvvvvv
// 默认无OAD的事件采用的数据结构体
typedef struct TEventNormal_t
{
	// #if( MAX_PHASE == 1)
	DWORD	Energy[4]; 	// 发生，结束时刻正反有功总电能
	BYTE	Reserved[10];		// 预留
	// #else
	// #endif
}TEventNormal;
// ^^^^^^^^^^^^^^^^^^^用来分配最大数据存储空间^^^^^^^^^^^^^^^^^^
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TEventStatus EventStatus;
extern const TSubEventInfoTab	SubEventInfoTab[];
extern const DWORD FixedColumnOadTab[];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 读写事件参数
//
//参数: 
//			Operation[in]:			WRITE/READ
//			inEventIndex[in]:		eEVENT_INDEX		
//			ParaType[in]:			eEVENT_PARA_TYPE
//			pBuf[out/in]:			读写数据的缓冲
//                    
//返回值:  	TRUE   正确
//			FALSE  错误
//
//备注:  	事件判断时调用(读)
//-----------------------------------------------
WORD DealEventParaByEventOrder( BYTE Operation, BYTE inEventIndex, BYTE ParaType, BYTE *pBuf );
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
BOOL WriteEventRecordData(DWORD Addr, WORD Length, DWORD BaseAddr, DWORD MemSpace, BYTE * pBuf);


//-----------------------------------------------
//函数功能: 保存工况类事件记录（失压失流等）
//
//参数: 
//	inEventIndex[in]:	eEVENT_INDEX 
//	Phase[in]:			ePHASE_TYPE  
//	BeforeAfter[in]:	事件发生前/发生后 eEVENT_TIME_TYPE
//  					发生时,次数+1,偏移++,结束时指针--  
//	EndTimeType[in]:	eEVENT_ENDTIME_CURRENT  当前时间
// 						eEVENT_ENDTIME_PWRDOWN  掉电时间
//  
//返回值:				TRUE/FALSE  
//备注:
//-----------------------------------------------
BOOL SaveInstantEventRecord( BYTE inEventIndex, BYTE Phase, BYTE BeforeAfter, BYTE EndTimeType );
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
BOOL GetEventInfo( TEventAddrLen *pEventAddrLen );
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
BOOL GetEventSectorInfo( WORD Len, WORD RecordDepth, DWORD inRecordNo, TEventSectorInfo *pEventSectorInfo );

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
BOOL GetEventIndex( WORD OI, BYTE *pEventIndex );

//-----------------------------------------------
//函数功能: 获取inEventIndex对应的OI
//参数: 
//			inEventIndex[in]:	事件枚举号              
//返回值:	OI		
//备注:
//-----------------------------------------------
WORD GetEventOI( BYTE inEventIndex );

//-----------------------------------------------
//函数功能: 根据事件类型及相位确定分相事件枚举号
//
//参数: 
//			EventIndex[in]:	事件枚举号                    
//			Phase[in]:		相位                    
//返回值:	分相事件枚举号	
//备注:
//-----------------------------------------------
BYTE GetSubEventIndex(TEventAddrLen *pEventAddrLen);
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
WORD ReadEventRecordByNo( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );
//-----------------------------------------------
//函数功能: 掉电主动上报组帧
//
//参数:	 
//         
//返回值:   
//			
//备注: 
//-----------------------------------------------
void LostPowerStartReport( void );


#endif//对应文件最前面的#define __EVENT_H
