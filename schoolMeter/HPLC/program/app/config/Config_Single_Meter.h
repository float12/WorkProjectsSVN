#ifndef __CONFIG_SINGLE_METER_H
#define __CONFIG_SINGLE_METER_H
//------------------------------------------------------------------------------------------
//	名    称： config_Dlt645_2007.h
//	功    能:  采用 DL/T645-2007 的电表功能配置头文件，
//	作    者:  魏灵坤
//	创建时间:  2009-7-15
//	更新时间:
//	备    注:	此文件中只能有宏定义，不能有结构
//	修改记录:
//------------------------------------------------------------------------------------------
//主要是事件记录部分


#if( MAX_PHASE == 1 )
	//是否选择新规约 DL/T645-2007
	#define SEL_DLT645_2007 					YES                                           	
	//是否选择新规约 DL/T698.45-2016
	#define  SEL_DLT698_2016_FUNC            	YES
	//是否记录新DLT645要求的视在电能，只为DL/T645-2007所用
	#define SEL_APPARENT_ENERGY					NO  
	//是否支持无功电能
	#define SEL_RACTIVE_ENERGY					NO                            
	//是否支持分相电能 DL/T645_1997 也曾打开过
	#define SEL_SEPARATE_ENERGY					NO
	//是否支持分相费率电能
	#define SEL_SEPARATE_RATIO_ENERGY			NO
	//是否选择计算无功需量   如果选择不计算无功需量 则也不计算四象限无功需量
	//如果选择计算无功需量，则四象限无功需量也计算
	#define SEL_REACTIVE_DEMAND					NO
	//是否选择计算视在需量  与SEL_APPARENT_ENERGY无关，视在功率什么时候都支持
	#define	SEL_APPARENT_DEMAND					NO
	//是否支持冻结周期内最大需量
	#define	SEL_FREEZE_PERIOD_DEMAND			NO
	//是否支持安时计算功能
	#define SEL_AHOUR_FUNC						NO
	//存储是否支持硬件保护
	#define SEL_MEMORY_PROTECT					NO

	//是否启用F415芯片
	#define SEL_F415							YES
	
	//是否启用ESSAM
	#define SEL_ESSAM							NO

	//是否启用7053D芯片采样波形输出
	#define SEL_WAVE_OUTPUT						YES

	//----------------------------------------------------
	//数据库最大值定义
	//----------------------------------------------------
	//选择数据库中电能小数位数，如果选择小数点位数太大，数据库将按脉冲数存储
	#define SYS_DBASE_ENERGY_DOT		0xff//数据库按脉冲数存储
	//最大记录月数
	#define MAX_MONTH					13
	#define MAX_MONTH_FOR_BACKUP		3	// 结算数据备份月数
	//最大费率数
	#define MAX_RATIO					12
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//最大阶梯数
	#define MAX_LADDER					6
	//ESAM中最大阶梯数
	#define MAX_ESAM_LADDER				6
	//最大年结算日数
	#define MAX_YEAR_BILL				4
	//ESAM中最大年结算日数
	#define MAX_ESAM_YEAR_BILL			4
	#endif
	//天津GPRS、DL/T645_2007 要求10级密码
	#define MAX_645PASSWORD_LEVEL		5//645密码级别
	//密码长度
	#define MAX_645PASSWORD_LENGTH		4//645密码长度
	//密码长度
	#define MAX_698PASSWORD_LENGTH		8//698密码长度(密码长度+ 密码)
	//密码错误次数
	#define MAX_PASSWORD_ERR_TIMES		0x03
	//最大时区数
	#define MAX_TIME_AREA				14
	//最大时段表数
	#define MAX_TIME_SEG_TABLE			8//
	//每个时段表最大时段数
	#define MAX_TIME_SEG				14
	//最大公共假日数
	#define MAX_HOLIDAY					20//超过20需要改申请缓冲的大小
	//先根据现有国网要求定义8个字节报警异常控制字节
	#define MAX_WARN_ERR_NUM			8
	//DL/T645-2007要求每月最多3次结算，且每个结算日都有日时
	#define MAX_MON_CLOSING_NUM			3

	#define	SAMPLE_CHANNALS				2 	//直流和纹波两路
	
	#define MAX_SAFE_MODE_PARA_NUM		60	//不能超过62个

	//----------------------------------------------------
	// 事件记录选择
	//--------------------------------------
	// 是否选择主动上报
	#define SEL_AUTO_WARN					NO
	// 辅助电源失电
	#define SEL_EVENT_LOST_SECPOWER			NO
	// 失压记录
	#define SEL_EVENT_LOST_V				NO
	// 欠压记录
	#define SEL_EVENT_WEAK_V				NO
	// 过压记录
	#define SEL_EVENT_OVER_V				NO
	// 断相记录
	#define SEL_EVENT_BREAK					NO
	// 电压逆相序记录
	#define SEL_EVENT_V_REVERSAL			NO
	// 电流逆相序记录
	#define SEL_EVENT_I_REVERSAL			NO
	// 电压不平衡记录
	#define SEL_EVENT_V_UNBALANCE			NO
	// 电流不平衡记录
	#define SEL_EVENT_I_UNBALANCE			NO
	// 失流记录
	#define SEL_EVENT_LOST_I				NO
	// 过流记录
	#define SEL_EVENT_OVER_I				YES
	// 断流记录
	#define SEL_EVENT_BREAK_I				NO
	// 分相 功率反向
	#define SEL_EVENT_BACKPROP				NO
	// 过载记录
	#define SEL_EVENT_OVERLOAD				NO
	// 掉电记录
	#define SEL_EVENT_LOST_POWER			YES
	// 开表盖记录
	#define SEL_EVENT_METERCOVER     		YES
	// 开端钮盒记录
	#define SEL_EVENT_BUTTONCOVER			NO
	// 分相功率因数超限记录
	#define SEL_EVENT_COS_OVER				NO
	// 电流严重不平衡记录
	#define SEL_EVENT_I_S_UNBALANCE			NO
	// 单相表零火线电流不平衡记录
	#define SEL_EVENT_SP_I_UNBALANCE		NO
	// 恒定磁场干扰记录
	#define SEL_EVENT_MAGNETIC_INT			NO
	// 负荷开关误动记录
	#define SEL_EVENT_RELAY_ERR				YES
	// 电源异常记录
	#define SEL_EVENT_POWER_ERR				YES
	//全失压记录
	#define SEL_EVENT_LOST_ALL_V			NO
	//时钟故障
	#define SEL_EVENT_RTC_ERR				YES
	//计量芯片故障
	#define SEL_EVENT_SAMPLECHIP_ERR		YES
	//电能表零线电流异常
	#define SEL_EVENT_NEUTRAL_CURRENT_ERR	YES
	//------------------------------------------------------------------------------------
	// 编程记录选择  共12项
	//--------------------------------------
	// 电表清零记录
	#define SEL_PRG_INFO_CLEAR_METER			YES
	// 清需量记录
	#define SEL_PRG_INFO_CLEAR_MD				NO
	// 事件清零记录
	#define SEL_PRG_INFO_CLEAR_EVENT			YES
	// 校时记录
	#define SEL_PRG_INFO_ADJUST_TIME			YES
	// 时段表编程记录
	#define SEL_PRG_INFO_TIME_TABLE				YES
	// 时区表编程记录
	#define SEL_PRG_INFO_TIME_AREA				YES
	// 周休日编程记录
	#define SEL_PRG_INFO_WEEK					YES
	// 节假日编程记录
	#define SEL_PRG_INFO_HOLIDAY				YES
	// 有功组合方式编程记录
	#define SEL_PRG_INFO_P_COMBO				YES
	// 无功组合方式编程记录
	#define SEL_PRG_INFO_Q_COMBO				NO
	// 结算日编程记录
	#define SEL_PRG_INFO_CLOSING_DAY			YES
	// 密钥更新记录
	#define SEL_PRG_UPDATE_KEY					YES
	// 645编程记录（记录最近10次数据标识）
	#define SEL_PRG_RECORD645					YES
	// 广播校时记录
	#define SEL_PRG_INFO_BROADJUST_TIME			YES
	//------------------------------------------------------------------------------------------------------------
	// 扩展事件类型选择  共6项
	//------------------------------------------------------------------------------------------------------------
	#if(PREPAY_MODE == PREPAY_LOCAL)
	// 购电记录
	#define SEL_EVENTEXT_BUYELEC				YES
	// 非法插卡记录
	#define SEL_EVENTEXT_ILLEGALCARD			YES
	#else
	// 购电记录
	#define SEL_EVENTEXT_BUYELEC				NO
	// 非法插卡记录
	#define SEL_EVENTEXT_ILLEGALCARD			NO
	#endif
	//------------------------------------------------------------------------------------
	// 统计记录选择  共2项
	//--------------------------------------
	// 电压合格率记录
	#define SEL_STAT_V_RUN						NO
	// 需量超限记录
	#define SEL_DEMAND_OVER						NO

#endif//#if( MAX_PHASE == 1 )

#endif//#ifndef __CONFIG_SINGLE_METER_H


