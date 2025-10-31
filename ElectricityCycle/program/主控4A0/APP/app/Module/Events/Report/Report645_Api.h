#ifndef __REPORT645_API_H
#define __REPORT645_API_H

#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define RPT_MODE_LEN			8	//主动上报模式字长度，8字节
#define RPT_STATUS_LEN			12	//主动上报状态字长度，12字节
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//主动上报状态字读取方式枚举
typedef enum
{
	eREAD_STATUS_PROC = 0,	//规约读取
	eREAD_STATUS_OTHER		//非规约读取
}eREAD_STATUS_TYPE;

//主动上报状态字索引号
typedef enum
{
	//负荷开关误动或拒动--0
	eSUB_STATUS_RELAY_ERR = 0,					

	//ESAM错误--1
	eSUB_STATUS_ESAM_ERR = 1,					
	
	//内卡初始化错误（与存储器故障或损坏含义相同）--2
	eSUB_STATUS_ROM_ERR = 2,					
	
	//时钟电池电压低--3
	eSUB_STATUS_CLOCK_BAT_LOW = 3,
	
	//内部程序错误--4
	eSUB_STATUS_CODE_ERR = 4,

	//存储器故障或损坏--5
	eSUB_STATUS_MEM_ERR = 5,					

	//保留--6
	eSUB_STATUS_BIT6 = 6,
	
	//时钟故障--7
	eSUB_STATUS_RTC_ERR = 7,
	
	//停电抄表电池欠压--8
	eSUB_STATUS_READ_BAT_LOW = 8,			

	//透支状态--9
	eSUB_STATUS_OVER_DRAFT = 9,			
	
	//开表盖--10
	eSUB_STATUS_METER_COVER = 10,
	
	//开端钮盖--11
	eSUB_STATUS_BUTTON_COVER = 11,
	
	//恒定磁场干扰--12
	eSUB_STATUS_MAGNETIC_INT = 12,

	//电源异常--13
	eSUB_STATUS_POWER_ERR = 13,

	//跳闸成功--14
	eSUB_STATUS_PRG_OPEN_RELAY = 14,	

	//合闸成功--15
	eSUB_STATUS_PRG_CLOSE_RELAY = 15,	

	//A相失压--16
	eSUB_STATUS_LOSTV_A = 16,

	//A相欠压--17
	eSUB_STATUS_WEAK_V_A = 17,

	//A相过压--18
	eSUB_STATUS_OVER_V_A = 18,

	//A相失流--19
	eSUB_STATUS_LOSTI_A = 19,

	//A相过流--20
	eSUB_STATUS_OVER_I_A = 20,

	//A相过载--21
    eSUB_STATUS_OVERLOAD_A = 21,

	//A相功率反向--22
	eSUB_STATUS_POW_BACK_A = 22,
	
	//A相断相--23
	eSUB_STATUS_BREAK_A = 23,

	//A相断流--24
	eSUB_STATUS_BREAK_I_A = 24,

	//保留--25
	eSUB_STATUS_BIT25 = 25,

	//保留--26
	eSUB_EVENT_BIT26 = 26,

	//保留--27
	eSUB_STATUS_BIT27 = 27,
	
	//保留--28
	eSUB_STATUS_BIT28 = 28,
	
	//保留--29
	eSUB_STATUS_BIT29 = 29,
	
	//保留--30
	eSUB_STATUS_BIT30 = 30,
	
	//保留--31
	eSUB_STATUS_BIT31 = 31,
	
	//B相失压--32
	eSUB_STATUS_LOSTV_B = 32,
	
	//B相欠压--33
	eSUB_STATUS_WEAK_V_B = 33,

	//B相过压--34
	eSUB_STATUS_OVER_V_B = 34,
	
	//B相失流--35
	eSUB_STATUS_LOSTI_B = 35,

	//B相过流--36
	eSUB_STATUS_OVER_I_B = 36,

	//B相过载--37
	eSUB_STATUS_OVERLOAD_B = 37,

	//B相功率反向--38
	eSUB_STATUS_POW_BACK_B = 38,

	//B相断相--39
	eSUB_STATUS_BREAK_B = 39,

	//B相断流--40
	eSUB_STATUS_BREAK_I_B = 40,

	//保留--41
	eSUB_STATUS_BIT41 = 41,

	//保留--42
	eSUB_STATUS_BIT42 = 42,

	//保留--43
	eSUB_STATUS_BIT43 = 43,

	//保留--44
	eSUB_STATUS_BIT44 = 44,
	
	//保留--45
	eSUB_STATUS_BIT45 = 45,

	//保留--46
	eSUB_STATUS_BIT46 = 46,

	//保留--47
	eSUB_STATUS_BIT47 = 47,

	//C相失压--48
	eSUB_STATUS_LOSTV_C = 48,
	
	//C相欠压--49
	eSUB_STATUS_WEAK_V_C = 49,

	//C相过压--50
	eSUB_STATUS_OVER_V_C = 50,

	//C相失流--51
	eSUB_STATUS_LOSTI_C = 51,

	//C相过流--52
	eSUB_STATUS_OVER_I_C = 52,

	//C相过载--53
	eSUB_STATUS_OVERLOAD_C = 53,

	//C相功率反向--54
	eSUB_STATUS_POW_BACK_C = 54,

	//C相断相--55
	eSUB_STATUS_BREAK_C = 55,
	
	//C相断流--56
	eSUB_STATUS_BREAK_I_C = 56,

	//保留--57
	eSUB_STATUS_BIT57 = 57,

	//保留--58
	eSUB_STATUS_BIT58 = 58,

	//保留--59
	eSUB_STATUS_BIT59 = 59,

	//保留--60
	eSUB_STATUS_BIT60 = 60,

	//保留--61
	eSUB_STATUS_BIT61 = 61,

	//保留--62
	eSUB_STATUS_BIT62 = 62,

	//保留--63
	eSUB_STATUS_BIT63 = 63,

	//电压逆相序-64
	eSUB_STATUS_V_REVERSAL = 64,

	//电流逆相序--65
	eSUB_STATUS_I_REVERSAL = 65,

	//电压不平衡--66
	eSUB_STATUS_V_UNBALANCE = 66,

	//电流不平衡--67
	eSUB_STATUS_I_UNBALANCE = 67,

	//辅助电源失电--68
	eSUB_STATUS_LOST_SECPOWER = 68,

	//掉电--69
	eSUB_STATUS_LOST_POWER = 69,		

	//需量超限--70
	eSUB_STATUS_DEMAND_OVER = 70,

	//总功率因数超限--71
	eSUB_STATUS_COS_OVER_ALL = 71,		

	//电流严重不平衡--72
	eSUB_STATUS_S_I_UNBALANCE = 72,

	//潮流反向--73
	eSUB_STATUS_BACKPROPALL = 73,

	//全失压--74
	eSUB_STATUS_LOST_ALL_V = 74,

	//保留--75
	eSUB_STATUS_BIT75 = 75,

	//保留--76
	eSUB_STATUS_BIT76 = 76,
#if( SEL_TOPOLOGY == YES )
    //特征电流信号识别结果事件--77
	eSUB_STATUS_TOPO_IDENTI_RESULT= 77,
#else
	//保留--77
	eSUB_STATUS_BIT77 = 77,
#endif

	//保留--78
	eSUB_STATUS_BIT78 = 78,

	//保留--79
	eSUB_STATUS_BIT79 = 79,

	//编程--80
	eSUB_STATUS_PRG_RECORD = 80,

	//电表清零--81	
	eSUB_STATUS_PRG_CLEAR_METER = 81,			

	//需量清零--82
	eSUB_STATUS_PRG_CLEAR_MD = 82,				

	//事件清零--83
	eSUB_STATUS_PRG_CLEAR_EVENT = 83,		

	//校时--84
	eSUB_STATUS_PRG_ADJUST_TIME = 84,			

	//时段表编程--85
	eSUB_STATUS_PRG_TIME_TABLE = 85,		

	//时区表编程--86
	eSUB_STATUS_PRG_TIME_AREA = 86,		

	//周休日编程--87
	eSUB_STATUS_PRG_WEEKEND = 87,			

	//节假日编程--88
	eSUB_STATUS_PRG_HOLIDAY = 88,		

	//有功组合方式编程--89
	eSUB_STATUS_PRG_P_COMBO = 89,			

	//无功组合方式1编程--90
	eSUB_STATUS_PRG_Q_COMBO = 90,					

	//无功组合方式2编程--91
	eSUB_STATUS_PRG_Q2_COMBO = 91,				

	//结算日编程--92
	eSUB_STATUS_PRG_CLOSING_DAY = 92,				

	//费率参数编程--93
	eSUB_STATUS_PRG_RATE = 93,		  

	//阶梯表编程--94
	eSUB_STATUS_PRG_STEP = 94,           

	//密钥更新--95
	eSUB_STATUS_PRG_UPDATE_KEY = 95,	

	eREPORT_STATUS_TOTAL,
	//占位
	eSUB_STATUS_NULL	//事件记录有些项在状态字中没有，所以用eSUB_STATUS_NULL在SubEventInfoTab表格中占位
}eREPORT_STATUS_INDEX;

//主动上报模式字索引号
typedef enum
{
	//负荷开关误动或拒动--0
	eSUB_MODE_RELAY_ERR = 0,					

	//ESAM错误--1
	eSUB_MODE_ESAM_ERR = 1,					
	
	//内卡初始化错误（与存储器故障或损坏含义相同）--2
	eSUB_MODE_ROM_ERR = 2,					
	
	//时钟电池电压低--3
	eSUB_MODE_CLOCK_BAT_LOW = 3,
	
	//内部程序错误--4
	eSUB_MODE_CODE_ERR = 4,

	//存储器故障或损坏--5
	eSUB_MODE_MEM_ERR = 5,					

	//保留--6
	eSUB_MODE_BIT6 = 6,
	
	//时钟故障--7
	eSUB_MODE_RTC_ERR = 7,
	
	//停电抄表电池欠压--8
	eSUB_MODE_READ_BAT_LOW = 8,			

	//透支状态--9
	eSUB_MODE_OVER_DRAFT = 9,			
	
	//开表盖--10
	eSUB_MODE_METER_COVER = 10,
	
	//开端钮盖--11
	eSUB_MODE_BUTTON_COVER = 11,
	
	//恒定磁场干扰--12
	eSUB_MODE_MAGNETIC_INT = 12,

	//电源异常--13
	eSUB_MODE_POWER_ERR = 13,

	//跳闸成功--14
	eSUB_MODE_PRG_OPEN_RELAY = 14,	

	//合闸成功--15
	eSUB_MODE_PRG_CLOSE_RELAY = 15,	

	//失压--16
	eSUB_MODE_LOSTV = 16,

	//欠压--17
	eSUB_MODE_WEAK_V = 17,

	//过压--18
	eSUB_MODE_OVER_V = 18,

	//失流--19
	eSUB_MODE_LOSTI = 19,

	//过流--20
	eSUB_MODE_OVER_I = 20,

	//过载--21
	eSUB_MODE_OVERLOAD = 21,

	//功率反向--22
	eSUB_MODE_POW_BACK = 22,
	
	//A相断相--23
	eSUB_MODE_BREAK = 23,

	//A相断流--24
	eSUB_MODE_BREAK_I = 24,

	//保留--25
	eSUB_MODE_BIT25 = 25,

	//保留--26
	eSUB_MODE_BIT26 = 26,

	//保留--27
	eSUB_MODE_BIT27 = 27,
	
	//保留--28
	eSUB_MODE_BIT28 = 28,
	
	//保留--29
	eSUB_MODE_BIT29 = 29,
	
	//保留--30
	eSUB_MODE_BIT30 = 30,
	
	//保留--31
	eSUB_MODE_BIT31 = 31,

	//电压逆相序-32
	eSUB_MODE_V_REVERSAL = 32,

	//电流逆相序--33
	eSUB_MODE_I_REVERSAL = 33,

	//电压不平衡--34
	eSUB_MODE_V_UNBALANCE = 34,

	//电流不平衡--35
	eSUB_MODE_I_UNBALANCE = 35,

	//辅助电源失电--36
	eSUB_MODE_LOST_SECPOWER = 36,

	//掉电--37
	eSUB_MODE_LOST_POWER = 37,		

	//需量超限--38
	eSUB_MODE_DEMAND_OVER = 38,

	//总功率因数超限--39
	eSUB_MODE_COS_OVER_ALL = 39,		

	//电流严重不平衡--40
	eSUB_MODE_S_I_UNBALANCE = 40,

	//潮流反向--41
	eSUB_MODE_BACKPROPALL = 41,

	//全失压--42
	eSUB_MODE_LOST_ALL_V = 42,

	//保留--43
	eSUB_MODE_BIT43 = 43,

	//保留--44
	eSUB_MODE_BIT44 = 44,
    
#if( SEL_TOPOLOGY == YES )
    //特征电流信号识别结果事件--45
    eSUB_MODE_TOPO_IDENTI_RESULT = 45,
#else
	//保留--45
	eSUB_MODE_BIT45 = 45,
#endif

	//保留--46
	eSUB_MODE_BIT46 = 46,

	//保留--47
	eSUB_MODE_BIT47 = 47,

	//编程--48
	eSUB_MODE_PRG_RECORD = 48,

	//电表清零--49
	eSUB_MODE_PRG_CLEAR_METER = 49,			

	//需量清零--50
	eSUB_MODE_PRG_CLEAR_MD = 50,				

	//事件清零--51
	eSUB_MODE_PRG_CLEAR_EVENT = 51,		

	//校时--52
	eSUB_MODE_PRG_ADJUST_TIME = 52,			

	//时段表编程--53
	eSUB_MODE_PRG_TIME_TABLE = 53,		

	//时区表编程--54
	eSUB_MODE_PRG_TIME_AREA = 54,		

	//周休日编程--55
	eSUB_MODE_PRG_WEEKEND = 55,			

	//节假日编程--56
	eSUB_MODE_PRG_HOLIDAY = 56,		

	//有功组合方式编程--57
	eSUB_MODE_PRG_P_COMBO = 57,			

	//无功组合方式1编程--58
	eSUB_MODE_PRG_Q_COMBO = 58,					

	//无功组合方式2编程--59
	eSUB_MODE_PRG_Q2_COMBO = 59,				

	//结算日编程--60
	eSUB_MODE_PRG_CLOSING_DAY = 60,				

	//费率参数编程--61
	eSUB_MODE_PRG_RATE = 61,		  

	//阶梯表编程--62
	eSUB_MODE_PRG_STEP = 62,           

	//密钥更新--63
	eSUB_MODE_PRG_UPDATE_KEY = 63,	

	eREPORT_MODE_TOTAL,
}eREPORT_MODE_INDEX;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------


//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

#pragma pack(1)	//本文件内所有结构体均1字节对齐

typedef struct TReport645Table_t
{
	BYTE    Event_SubIndex;	//事件sub枚举
	BYTE	Status_Index;	//主动上报状态字枚举
	BYTE	Event_Index;	//事件枚举
	BYTE	Mode_Index;	    //主动上报模式字枚举
}TReport645Table;

//主动上报结构体
typedef struct TReport645_t
{
    BYTE BakFlag[MAX_COM_CHANNEL_NUM];                           //复位标志 0x00无上报 0x55上报已读走
	BYTE StatusBak[MAX_COM_CHANNEL_NUM][RPT_STATUS_LEN];		 //主动上报状态字备份
    BYTE ResetTime[MAX_COM_CHANNEL_NUM];                        //复位主动上报延时（实时）
	DWORD CRC32;
}TReport645;

typedef struct TReport645ResetTime_t
{
    BYTE  Time;                                        //复位主动上报延时（固定）
	DWORD CRC32;
}TReport645ResetTime;


typedef struct TReportTimes645_t
{
   BYTE Times[MAX_COM_CHANNEL_NUM][SUB_EVENT_INDEX_MAX_NUM];    //主动上报状态字新增次数
   DWORD CRC32;
}TReportTimes645;

#pragma pack()	
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 清上报次数
//
//参数:      BYTE Mode[in]        0x00:清指定事件
//                              0x55:全清
//         BYTE SubEventIndex   增加次数的事件索引
//
//返回值:		Result		- TRUE：启用 		FALSE：不启用
//
//备注:
//---------------------------------------------------------------
WORD  ClearReport645Times( BYTE Ch, BYTE Mode, BYTE SubEventIndex );

//-----------------------------------------------
//函数功能: 增加上报次数
//
//参数:      BYTE Ch[in]          通道选择
//
//         BYTE Mode[in]        0x00:次数累计加1
//                              0x55:次数重置为1
//
//         BYTE SubEventIndex   增加次数的事件索引
//
//返回值:		Result		- TRUE：启用 		FALSE：不启用
//
//备注:
//---------------------------------------------------------------
WORD  api_AddReportTimes( BYTE Ch, BYTE Mode, BYTE SubEventIndex);

//-----------------------------------------------
//函数功能:      设置主动上报复位时间
//
//参数:        BYTE ResetTime[in]
//
//返回值:		
//
//备注:
//---------------------------------------------------------------
WORD  api_SetReportResetTime( BYTE ResetTime );

//-----------------------------------------------
//函数功能:      读取主动上报复位时间
//
//参数:        BYTE Time[Out]
//
//返回值:		
//
//备注:
//---------------------------------------------------------------
BYTE  api_GetReportResetTime( void );

//-----------------------------------------------
//函数功能: 主动上报状态字读取
//
//参数:		BYTE Ch[in]  - 通道选择
//          Type[in]	 - 读取方式 eREAD_STATUS_PROC--规约读取	         	eREAD_STATUS_OTHER--非规约读取
//			pBuf[out]	 - 主动上报状态字缓存
//返回值:		ReturnLen	 - 数据长度		0：错误
//
//备注:
//---------------------------------------------------------------
WORD api_ReadReportStatusByte( BYTE Ch, eREAD_STATUS_TYPE Type, BYTE *pBuf	);

//-----------------------------------------------
//函数功能:     主动上报状态字读取
//
//参数:	    BYTE *pBuf[in]  输入buf
//
//备注:
//---------------------------------------------------------------
WORD api_ReadReportStatusMode( BYTE *pBuf	);


//-----------------------------------------------
//函数功能: 获取主动上报状态字是否启用后续帧
//
//参数:      BYTE Ch[in] 通道
//			
//
//返回值:	
//
//备注:		
//---------------------------------------------------------------
WORD api_GetReportReqFlag(          BYTE Ch );

//-----------------------------------------------
//函数功能: 主动上报状态字复位
//
//参数:		Ch[in]      - 通道选择
//          pBuf[in]	- 复位缓存
//
//返回值:		Result	- TRUE：正确		FALSE：错误
//
//备注:
//---------------------------------------------------------------
WORD api_ResetReportStatusByte(          BYTE Ch, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 结束低功耗时从EEPROM恢复主动上报状态字、新增次数、标志位、复位倒计时、故障标志位
//
//参数:		无
//			
//返回值:		无
//
//备注:
//---------------------------------------------------------------
void api_PowerUpReport645( void );

//-----------------------------------------------
//函数功能: 掉电保存故障标志
//
//参数:		Type[in]	-
//			Index[in]	-
//返回值:		无
//
//备注:
//---------------------------------------------------------------
void api_PowerDownReport645( void );

//-----------------------------------------------
//函数功能:     初始化主动上报所有数据与参数
//
//参数:		无
//			
//返回值:		无
//
//备注:
//---------------------------------------------------------------
void api_FactoryInitReport645( void );

//-----------------------------------------------
//函数功能: 主动上报分钟任务
//
//参数:		（无）
//
//返回值:		（无）
//
//备注:		状态字全为0时，EVENTOUT输出高阻态，否则拉低
//---------------------------------------------------------------
void api_Report645MinTask( void );


#endif  //#if( SEL_DLT645_2007 == YES )
#endif	//#ifndef __REPORT645_API_H
