//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.15
//描述		管理系统标志、硬件标志、错误标志
//修改记录	
//----------------------------------------------------------------------
#ifndef __FLAG_API_H
#define __FLAG_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//任务名定义
//不是要求在0秒执行的分钟任务，要分配在不同的秒，每个模块的窗口为（ID*3+3~ID*3+3+2）秒
typedef enum
{//							序号	秒分配范围
 	eTASK_SAMPLE_ID = 0,	//0		(3~5)s
	eTASK_LCD_ID,			//1		(6~8)s
	eTASK_PROTOCOL_ID,		//2		(9~11)s
	eTASK_LOADPROFILE_ID,	//3		(12~14)s
	eTASK_DEMAND_ID,		//4		(15~17)s
	eTASK_EVENTS_ID,		//5		(18~20)s
	eTASK_PREPAY_ID,		//6		(21~23)s
	eTASK_FREEZE_ID,		//7		(24~26)s
	eTASK_ENERGY_ID,		//8		(27~29)s
	eTASK_PARA_ID,			//9		(30~32)s
	eTASK_RTC_ID,			//10	(33~35)s
	eTASK_LOWPOWER_ID,		//11	(36~38)s
	eTASK_SYSWATCH_ID,		//12	(39~41)s
	eTASK_RELAY_ID,			//13	(42~44)s
	eTASK_MID_ID,			//14 (45~48)	
	
	////!!!!!!这个必须放最后，不要挪动,此序号最大为16（目前只分配了一个WORD），不要再超过次数!!!!!!
	eTASK_ID_NUM_T
	
}TTaskIDNum;

#define		MAX_TASK_ID_NUM		16		//不要改，取决于TaskTimeFlag的类型


//每个任务标志定义
typedef enum
{
	//用于高速标志
	eFLAG_10_MS = 0,
	//500ms标志
	eFLAG_500_MS,
	eFLAG_SECOND,
	eFLAG_MINUTE,
	eFLAG_HOUR,
	eFLAG_TIME_CHANGE,			//时间改变标志
	eFLAG_SWITCH_JUDGE,			//设置切换时间标志
	
	eFLAG_TIMER_T,////!!!!!!这个必须放最后，不要挪动!!!!!!

}eTaskTimeFlag;

	

typedef enum
{
	//系统是否上电
	eSYS_STATUS_POWER_ON = 0,
	// 有功功率反向（总）
	eSYS_STATUS_OPPOSITE_P,//总 A B C一定要按顺序相连
	// A相有功功率反向（由采样芯片判断）
	eSYS_STATUS_OPPOSITE_P_A,
	// B相有功功率反向（由采样芯片判断）
	eSYS_STATUS_OPPOSITE_P_B,
	// C相有功功率反向（由采样芯片判断）
	eSYS_STATUS_OPPOSITE_P_C,
	
	// 无功功率反向（总）
	eSYS_STATUS_OPPOSITE_Q,//总 A B C一定要按顺序相连
	// A相无功功率反向（由采样芯片判断）
	eSYS_STATUS_OPPOSITE_Q_A,
	// B相无功功率反向（由采样芯片判断）
	eSYS_STATUS_OPPOSITE_Q_B,
	// C相无功功率反向（由采样芯片判断）
	eSYS_STATUS_OPPOSITE_Q_C,

	// 电流逆相序
	eSYS_STATUS_REVERSE_PHASE_I,
	
	// 控制回路错误 继电器动作故障
	eSYS_STATUS_RELAY_ERR,
	
	//698主站编程允许
	eSYS_STATUS_ID_698MASTER_AUTH,
	//698终端编程允许
	eSYS_STATUS_ID_698TERMINAL_AUTH,
	//698一般密码编程允许
	eSYS_STATUS_ID_698GENERAL_AUTH,
	//645编程允许
	eSYS_STATUS_ID_645AUTH,
	// 红外编程允许
	eSYS_IR_ALLOW_PRG,
	// 终端的身份认证
	eSYS_TERMINAL_ALLOW_PRG,
	// 透支状态（启动透支操作后，进入透支状态）
	eSYS_STATUS_PRE_USE_MONEY,
	//电能写使能开关
	eSYS_STATUS_EN_WRITE_ENERGY,
	//金额写使能开关
	eSYS_STATUS_EN_WRITE_MONEY,
	//校表系数写使能开关
	eSYS_STATUS_EN_WRITE_SAMPLEPARA,
	//低功耗唤醒标志
	eSYS_LOW_POWER_WAKEUP,	
	// 厂内模式字,可执行相关内部操作,（1为厂内可操作）,（0为出厂不可操作）
	eSYS_STATUS_INSIDE_FACTORY,
	// 低级厂内模式字,可执行低等级操作,（1为厂内可操作）,（0为出厂不可操作）
	eSYS_STATUS_LOW_INSIDE_FACTORY,
	//上电不清零变量数据出错标志
	eSYS_STATUS_NO_INIT_DATA_ERR,
	//计量芯片AD开启标志
	eSYS_STATUS_AD_START,
	#if( SEL_TOPOLOGY == YES )
	//正在拓扑发送标志
	eSYS_STATUS_TOPOING,
	#endif
	// 运行在低功耗模式
	eSYS_STATUS_RUN_LOW_POWER,

	//!!!!!!这个必须放最后，不要挪动!!!!!!
	eSYS_STATUS_NUM_T,
	
}eSysStatusFlag;

#define		MAX_SYS_STATUS_NUM		64		//必须为8的倍数


typedef enum
{
	// 公钥状态（否则为私钥）
	eRUN_HARD_COMMON_KEY = 0,
	// 切换标志，预留6个（要连续），TRUE代切换时间有效，FALSE代表切换时间无效
	//时区表切换标志
	eRUN_HARD_SWITCH_FLAG1,
	//时段表切换标志
	eRUN_HARD_SWITCH_FLAG2,
	//费率价格参数表切换标志
	eRUN_HARD_SWITCH_FLAG3,
	//阶梯参数表切换标志
	eRUN_HARD_SWITCH_FLAG4,
	//阶梯时区表切换标志
	eRUN_HARD_SWITCH_FLAG5,
	//保留
	eRUN_HARD_SWITCH_FLAG6,
	//MAC错误标记
	eRUN_HARD_MAC_ERR_FLAG,
	// ESAM故障
	eRUN_HARD_ESAM_ERR,
	//时钟电池电压低
	eRUN_HARD_CLOCK_BAT_LOW,
	//抄表电池电压低
	eRUN_HARD_READ_BAT_LOW,
	//密码闭锁状态：0：未闭锁	1：闭锁
	eRUN_HARD_PASSWORD_ERR_STATUS,//698密码闭锁标志
	eRUN_HARD_H_PASSWORD_ERR_STATUS,//02级密码闭锁标志
	eRUN_HARD_L_PASSWORD_ERR_STATUS,//04级密码闭锁标志
	//广播校时标志
	eRUN_HARD_ALREADY_BROADCAST_FLAG,
	//明文广播校时故障标志
	eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG,
	//时钟故障标志_RTC故障
	eRUN_HARD_ERR_RTC_FLAG,
	//时钟故障标志_明文广播校时超窗口期
	eRUN_HARD_BROADCAST_ERR_FLAG,
	#if( SEL_DLT645_2007 == YES )
	eRUN_HARD_645_FOLLOW_STATUS,//645启用后续帧标志       YES:启用   NO:不启用 电表运行特征字1对应的其他2个地方分别为lcd显示和继电器控制
	#endif
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//当前阶梯模式 FALSE：否 TRUE：是
	eRUN_HARD_YEAR_LADDER_MODE,	//年模式
	eRUN_HARD_MON_LADDER_MODE,	//月模式
	//透支标志
	eRUN_HARD_TICK_FLAG,
	#endif
	//身份认证启用使能 ==TRUE; 身份认证启用，必须建立应用连接才可以通过明文+RN抄读数据
	eRUN_HARD_EN_IDENTAUTH,
	//是否允许掉电上报上报标志，=TRUE：上电超过1小时，
	eRUN_HARD_POWERDOWN_REPORT_FLAG,

	eRUN_HARD_NUM_T,
		
}eRunHardFlag;//个数小于 MAX_RUN_HARD_FLAG_NUM

#define		MAX_RUN_HARD_FLAG_NUM		64		//必须为8的倍数

//出厂检时，这几个标志都要检查！！！！！！电表初始化时不清这些标志
typedef enum
{
	//需量是否采用连续计量方式，YES方向改变时不清需量，继续计算
	ePRO_HARD_DEMAND_CONTINUE_MEASURE,
	//自热是否补偿相角
	ePRO_HARD_PHASE_HEAT,
	//要读取的需量最低位是否要进位,YES代表不进位
	ePRO_HARD_DEMAND_DATA_CARRY,
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//所有阶梯结算日无效情况，且阶梯电价非零情况下，阶梯电价如何扣费：0--不扣费，1-按月阶梯扣费并在每月第1结算日转存阶梯用电量
	ePRO_HARD_LADDER_SAVE_PARA_INVALID_DEAL,
	#endif
	ePRO_HARD_NUM_T,
		
}eProHardFlag;

#define		MAX_PRO_HARD_FLAG_NUM		32		//必须为8的倍数


//!!!!!!为保证和以前的错误提示一致，不要更改错误号!!!!!!
//系统异常信息记录,若发生错误需要记录异常事件 
//最高位为 00(0)		为SetError存储的异常事件  序号为程序中定义的错误号，和显示一致
//最高位为 01(4)		保留
//最高位为 10(8)		普通的异常事件
//最高位为 11(c)		为断言存储的异常事件  序号为断言所在的行号
//这样定义后就不会有序号0了
#define ERR_EEPROM_CHECKSUN					1
//EEPROM写入错误（重试次数到），不能反映芯片局部损坏，只有整个损坏才会此报警。
#define ERR_WRITE_EEPROM1					4
#define ERR_WRITE_EEPROM2					5
#define ERR_WRITE_EEPROM3					6
//第一个参数区错误
#define ERR_FPARA1							17
//第二个参数区错误
#define ERR_FPARA2							18
//第三个参数区错误 告警和错误控制字等
#define ERR_FPARA3							19
//时区时段表错误
#define ERR_TIME_TABLE						20
//5460参数错误
#define ERR_PARA_CS5460						21
//启动时检测EEPROM中的电能校验错误。
#define ERR_CHECK_ENERGY					22
//月转存错误
#define ERR_SWAP_MONTH						23
//时钟芯片损坏
//#define ERR_RTC_READ						24
//5460自检错误，三个错误必须相邻。不能改变以下三个代码
#define ERR_CHECK_5460_1					25
#define ERR_CHECK_5460_2					26
#define ERR_CHECK_5460_3					27
#define ERR_SWAP_MONTH1						28
#define ERR_SWAP_MONTH2						29
// 修改掉电不丢失标志错误
#define ERR_SET_RUN_HARD_FLAG				33
#define ERR_SET_PRO_HARD_FLAG				34
// 读出掉电不丢失标志错误
#define ERR_GET_HARD_FLAG					35
#define ERR_GET_YFF_ENERGY					41
#define ERR_YFF_DEC_ERROR					42
#define ERR_GET_RATIO_PRICE					43		// 得到各个费率价格错误
#define ERR_DO_REMAIN_ENERGY				44		// 操作剩余电费等参数错误
#define ERR_DO_GET_BUY_NUM					45		// 操作剩余电费等参数错误
#define ERR_GET_REMAIN_ENERGY				46		// 读出剩余电量错误
#define ERR_GET_PRICE						47
#define ERR_CPU_TYPE						48
#define ERR_WRITE_SAMPLE_PARA				49		// 写入校表参数错误
#define ERR_PULSCONST_TOO_BIG				50		// 脉冲常数太大
#define ERR_PRICE_DATA						51		// 电价参数错误
#define ERR_CYCLESINFO_DATA					52		// 周期搜表参数错误
#define ERR_TOPO_DATA					    53		// 拓扑参数错误
#define ERR_MESSAGE_TRANS_DATA				54		// 报文透传参数错误
//错误标识码。最大64个错误标识
#define MAX_ERROR							64

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

// 存储掉电不丢失运行标志的结构
typedef struct TRunHardFlag_t
{
	
	BYTE Flag[ MAX_RUN_HARD_FLAG_NUM / 8];
	DWORD CRC32;
	
}TRunHardFlagSafeRom;

// 存储掉电不丢失规约设置标志的结构
typedef struct TProHardFlag_t
{
	
	BYTE Flag[ MAX_PRO_HARD_FLAG_NUM / 8];
	DWORD CRC32;
	
}TProHardFlagSafeRom;

typedef struct TFlagBytes_t
{
	//任务标志，暂定最大16个任务，即每个时间标志占2个字节
	WORD TaskTimeFlag[eFLAG_TIMER_T];
	// 系统状态
	BYTE SysStatus[MAX_SYS_STATUS_NUM / 8];
	// 存储掉电不丢失运行标志的结构
	TRunHardFlagSafeRom RunHardFlagSafeRom;
	// 存储掉电不丢失规约设置标志的结构
	TProHardFlagSafeRom ProHardFlagSafeRom;
	//错误标志
	BYTE ErrByte[(MAX_ERROR/8)];
	
}TFlagBytes;

typedef struct TNoInitDataFlag_t
{
	DWORD	Flag;
	DWORD	CRC32;
}TNoInitDataFlag;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern __no_init TNoInitDataFlag NoInitDataFlag;

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 设置系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetSysStatus(BYTE StatusNo);

//-----------------------------------------------
//函数功能: 清除系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrSysStatus(BYTE StatusNo);

//-----------------------------------------------
//函数功能: 获取系统状态
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	TRUE:系统状态置位了 FALSE:系统状态未置位
//
//备注:   
//-----------------------------------------------
BOOL api_GetSysStatus(BYTE StatusNo);

//-----------------------------------------------
//函数功能: 设置系统运行固化标志
//
//参数: 
//			FlagNo[in]		系统固化标志号
//                    
//返回值:  	TRUE:设置成功	FALSE:设置失败
//
//备注:   
//-----------------------------------------------
BOOL api_SetRunHardFlag(BYTE FlagNo);

//-----------------------------------------------
//函数功能: 清除系统运行固化标志
//
//参数: 
//			FlagNo[in]		系统固化标志号
//                    
//返回值:  	TRUE:清除成功	FALSE:清除失败
//
//备注:   
//-----------------------------------------------
BOOL api_ClrRunHardFlag(BYTE FlagNo);

//-----------------------------------------------
//函数功能: 获取系统运行固化标志
//
//参数: 
//			FlagNo[in]		系统固化标志号
//                    
//返回值:  	TRUE:相应固化标志置位了	FALSE:相应固化标志未置位或数据不对
//
//备注:   
//-----------------------------------------------
BOOL api_GetRunHardFlag(BYTE FlagNo);

//-----------------------------------------------
//函数功能: 复位系统运行固化标志
//
//参数: 	无
//                    
//返回值:  	TRUE:初始化成功	FALSE:初始化失败
//
//备注:   
//-----------------------------------------------
BOOL api_FactoryInitRunHardFlag(void);

//-----------------------------------------------
//函数功能: 设置系统规约设置固化标志
//
//参数: 
//			FlagNo[in]		规约设置系统固化标志号
//                    
//返回值:  	TRUE:设置成功	FALSE:设置失败
//
//备注:   
//-----------------------------------------------
BOOL api_SetProHardFlag(BYTE FlagNo);

//-----------------------------------------------
//函数功能: 清除系统规约设置固化标志
//
//参数: 
//			FlagNo[in]		系统规约设置固化标志号
//                    
//返回值:  	TRUE:清除成功	FALSE:清除失败
//
//备注:   
//-----------------------------------------------
BOOL api_ClrProHardFlag(BYTE FlagNo);

//-----------------------------------------------
//函数功能: 获取系统规约设置固化标志
//
//参数: 
//			FlagNo[in]		系统规约设置固化标志号
//                    
//返回值:  	TRUE:相应固化标志置位了	FALSE:相应固化标志未置位或数据不对
//
//备注:   
//-----------------------------------------------
BOOL api_GetProHardFlag(BYTE FlagNo);

//-----------------------------------------------
//函数功能: 复位系统规约设置固化标志
//
//参数: 	无
//                    
//返回值:  	TRUE:初始化成功	FALSE:初始化失败
//
//备注:   
//-----------------------------------------------
void api_FactoryInitProHardFlag(void);

//-----------------------------------------------
//函数功能: 设置指定的错误
//
//参数: 
//			Err[in]		指定的错误标志号
//			BIT15		0  错误信息要写入EEPROM
//           			1  错误信息不写入EEPROM
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetError(WORD Err);

//-----------------------------------------------
//函数功能: 检查当前是否存在指定的错误
//
//参数: 
//			Err[in]		指定的错误标志号
//                    
//返回值:  	TRUE:存在指定的错误  	FALSE:不存在指定的错误
//
//备注:   
//-----------------------------------------------
BOOL api_CheckError(WORD Err);

//-----------------------------------------------
//函数功能: 清除指定的错误标志号
//
//参数: 
//			Err[in]		指定的错误标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrError(WORD Err);

//-----------------------------------------------
//函数功能: 给所有任务置同一个标志
//
//参数: 
//			FlagID[in]		标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetAllTaskFlag( BYTE FlagID );

//-----------------------------------------------
//函数功能: 置任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_SetTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//函数功能: 获取任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	TRUE:相应标志置位了  FALSE:相应标志未置位
//
//备注:   
//-----------------------------------------------
BOOL api_GetTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//函数功能: 清除任务标志
//
//参数: 
//			TaskID[in]		任务号
//			FlagID[in]		标志号
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_ClrTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//函数功能: 标志任务上电处理
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_PowerUpFlag( void );

//-----------------------------------------------
//函数功能: 初始化_noinit_标志
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_InitNoInitFlag( void );

#endif//#ifndef __FLAG_API_H
