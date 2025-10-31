//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.2
//描述		参数管理头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __PARA_API_H
#define __PARA_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	//时钟脉冲输出
	eCLOCK_PULS_OUTPUT=0,
	//需量周期输出
	eDEMAND_PERIOD_OUTPUT,
	//时段切换输出
	eSEGMENT_SWITCH_OUTPUT
	
}TOutputType;

typedef struct TGlobalVariable_t
{
	//多功能端子输出类型
	BYTE g_byMultiFunPortType;
	//需量周期输出计数器
	BYTE g_byMultiFunPortCounter;
	

}TGlobalVariable;


typedef struct TMeterSnPara_t
{
	//通讯地址，BCD [0]-保存高字节
	BYTE CommAddr[6];
	//表号，BCD  [0]-保存高字节
	BYTE MeterNo[6];
	//客户编号   [0]-保存高字节
	BYTE CustomCode[6];
}TMeterSnPara;


#pragma pack(1)
typedef struct TLCDPara_t
{
	//上电全显时间  unsigned，
	BYTE PowerOnDispTimer;//上电背光点亮时间也用这个
	//背光点亮时间  long-unsigned(按键时背光点亮时间)，
	WORD BackLightPressKeyTimer;
	//显示查看背光点亮时间  long-unsigned，
	WORD BackLightViewTimer;
	//无电按键屏幕驻留最大时间  long-unsigned， HEX 单位为秒
	WORD LcdSwitchTime;
	//电能小数点
	BYTE EnergyFloat;
	//功率（需量）小数点
	BYTE DemandFloat;
	//显示12字样意义  0:用于显示当前套、备用套时段 1：用于显示当前套、备用套费率电价
	BYTE Meaning12;
	
	//下标:0--循显项目数 1--键显项目数 
	BYTE DispItemNum[2];
	//键显切换到循环显示的时间 显示类属性3参数
	WORD KeyDisplayTime;
	//循环显示时每屏的显示时间 显示类属性3参数
	WORD LoopDisplayTime;
	
}TLCDPara;
#pragma pack()


#pragma pack(1)
typedef struct TTimeZoneSegPara_t
{
	//年时区数，Hex码
	BYTE TimeZoneNum;
	//日时段表数，Hex码
	BYTE TimeSegTableNum;
	//日时段数，Hex码
	BYTE TimeSegNum;
	//费率数，要小于最大费率数，Hex码，当前用户设置的最大费率数
	BYTE Ratio;
	//公共假日数，Hex码 因为新规约要求2bytes n <= 254
	BYTE HolidayNum;
	
	//周休状态字（1-工作，0-休息）
	//D0	周日
	//D1	周一
	//D2	周二
	//D3	周三
	//D4	周四
	//D5	周五
	//D6	周六
	BYTE WeekStatus;
	//周修日采用的时段表
	BYTE WeekSeg;

}TTimeZoneSegPara;
#pragma pack()


typedef struct TFPara1_t
{
	TMeterSnPara MeterSnPara;

	//为方便规约处理，特意组合的结构
	TLCDPara LCDPara;

	TTimeZoneSegPara TimeZoneSegPara;
	
	WORD wClearMeterType; //0x1111--电表清零(数据初始化) 0x2222--恢复出厂参数 0x4444--事件清零 0x8888--需量清零

	//校验
	DWORD CRC32;
	
}TFPara1;


#pragma pack(1)
typedef struct TIntervalDemandFreezePeriod_t
{
	BYTE Unit;				//期间需量周期单位	分钟
	WORD FreezePeriod;		//期间需量周期
	
}TIntervalDemandFreezePeriod;
#pragma pack()


typedef struct TEnereyDemandMode_t
{
	//有功组合方式特征字 组合有功
	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
	//保留	保留		保留	保留	反向有功	反向有功	正向有功	正向有功
	//									0不减1减	0不加1加	0不减1减	0不加1加
	BYTE byActiveCalMode;
	
	//04000602 04000603 无功组合方式1、2特征字
	//Bit7		Bit6		Bit5		Bit4		Bit3		Bit2		Bit1		Bit0
	//IV象限	IV象限		III象限		III象限		II象限		II象限		I象限		I象限
	//0不减1减	0不加1加	0不减1减	0不加1加	0不减1减	0不加1加	0不减1减	0不加1加
	#if(SEL_RACTIVE_ENERGY == YES)
	BYTE PReactiveMode;//正向无功模式控制字
	BYTE NReactiveMode;//反向无功模式控制字
	#endif
	
	#if( MAX_PHASE != 1 )
	//需量周期
	BYTE DemandPeriod;	
	//需量滑差时间
	BYTE DemandSlip;
	//期间需量冻结周期
	TIntervalDemandFreezePeriod IntervalDemandFreezePeriod;
	#endif

}TEnereyDemandMode;


//通讯控制参数
typedef struct TCommPara_t
{
	//通讯控制参数
	//逻辑第一串行口
	//D3-D0	拨特率控制
	//		0000 -- 300
	//		0001 -- 600
	//		0010 -- 1200
	//		0011 -- 2400
	//		0100 -- 4800
	//		0101 -- 7200
	//		0110 -- 9600
	//		0111 -- 19200
	// 		1000 -- 38400
	// 		1001 -- 57600
	// 		1010 -- 115200 A
	//		1011 -- 230400 B
	//		1100 -- 460800 C
	//		1101 -- 921600 D
	//D4	停止位
	//		0 -- 1位停止位
	//		1 -- 2位停止位
	//D6-D5	校验控制
	//		00 -- 无校验
	//		01 -- 奇校验
	//		10 -- 偶校验
	//D7	硬件控制
	//		0 -- 无硬件控制
	//		1 -- 有硬件控制
	
	BYTE I485;
	
	//逻辑第三串行口（第二串口固定为红外了并且红外的波特率是固定的）
	BYTE ComModule;
	
	//逻辑第四串行口
	BYTE II485;

	//CAN总线波特率特征字
	BYTE CanBaud;
	
}TCommPara;


//第二个区域
typedef struct TFPara2_t
{
	TEnereyDemandMode EnereyDemandMode;
	
	//通讯控制参数
	TCommPara CommPara;
	
	DWORD ActiveConstant;//HEX data
	DWORD ReactiveConstant;//HEX data

	//校验
	DWORD CRC32;

}TFPara2;



typedef struct TMuchPassword_t
{
	//MAX_PASSWORD_LEVEL为5可以兼容国网2,4级密码，南网3级密码，黑龙江0级密码，用下标表示
	BYTE Password645[MAX_645PASSWORD_LEVEL][MAX_645PASSWORD_LENGTH];
	BYTE Password698[MAX_698PASSWORD_LENGTH];
	DWORD CRC32;
}TMuchPassword;


//时段、时区设置，注意扩充时区时段表，可能会引起FlashBuf处理溢出，扩充不能超过FlashBuf的一半。
typedef struct TTimeAreaTable_t
{
	//**************************************************
	//时区起始日期及时段表号
	//每项格式为-----月：日：时段表号（0、1、2）
	BYTE TimeArea[MAX_TIME_AREA][3];
	DWORD CRC32;
	
}TTimeAreaTable;


typedef struct TTimeSegTable_t
{
	//**************************************************
	//时段表描述
	//每项格式为-----时：分：费率号（0、1、2）
	BYTE TimeSeg[MAX_TIME_SEG][3];
	DWORD CRC32;

}TTimeSegTable;


#pragma pack(1)
typedef struct TTimeHoliday_t
{
	TRealTimerDate RealTimerDate;
	BYTE HolidaySegNum;
	
}TTimeHoliday;


typedef struct TTimeHolidayTable_t
{
	TTimeHoliday TimeHoliday[MAX_HOLIDAY];
	DWORD CRC32;
	
}TTimeHolidayTable;
#pragma pack()


typedef struct TTimeTable_t
{
	//时区描述
	TTimeAreaTable TimeAreaTable;
	//时段表描述
	TTimeSegTable TimeSegTable[MAX_TIME_SEG_TABLE];

}TTimeTable;


typedef struct TSwitchTimePara_t
{
	//04000106  YYMMDDhhmm		5   年月日时分  两套时区表切换时间
	//04000107	YYMMDDhhmm		5	年月日时分	 两套日时段切换时间
	//04000108  YYMMDDhhmm		5	两套分时费率切换时间
	//04000109	YYMMDDhhmm 		5	两套梯度切换时间
	//0400010A	YYMMDDhhmm 		5	保留
	//保存相对时间：切换时间与2000年1月1日0时0分之间的时间差，单位：分钟 (用函数api_CalcInTimeRelativeMin()计算值)，
	DWORD dwSwitchTime[5];
	
	DWORD CRC32;

}TSwitchTimePara;





typedef struct TMonSavePara_t
{
	//DL/T645-2007要求每月最多3次结算，且每个结算日都有日时
	//HEX码，若为0xFF无效，对应一月每一天。
	BYTE Day;
	//冻结小时 HEX码，如为FF无效
	BYTE Hour;

}TMonSavePara;

typedef struct TBillingPara_t
{
	//DL/T645-2007要求每月最多3次结算，且每个结算日都有日时
	TMonSavePara MonSavePara[MAX_MON_CLOSING_NUM];
	//校验
	DWORD CRC32;
	
}TBillingPara;
typedef struct TReportModeConst_t
{
  BYTE EventIndex;//需上报事件
  BYTE ReportMode;//上报模式
	
}TReportModeConst;

#if(SEL_EVENT_DI_CHANGE == YES)
#pragma pack(1)
typedef struct TDIPara_t
{
	DWORD 	AntiShakeTime;			//遥信防抖动时间
	BYTE	bAccessState;			//接入状态
	BYTE	bAttributeState;		//属性状态
	DWORD	CRC32;
}TDIPara;
#pragma pack()
#endif

typedef struct TSafeMeterPara_t
{
	//密码
	TMuchPassword MuchPassword;
	
	//第一套时区时段表
	TTimeTable FirstTimeTable;
	//第二套时区时段表
	TTimeTable SecondTimeTable;
	//节假日描述
	TTimeHolidayTable TimeHolidayTable;
	//切换参数
	TSwitchTimePara SwitchTimePara;
	
	TFPara1 FPara1;
	TFPara2 FPara2;
	
	//月结算和年结算参数
	TBillingPara BillingPara;	

	//CAN总线参数
//	TCanBusPara CanBusPara;

	#if(SEL_EVENT_DI_CHANGE == YES)
	TDIPara DIPara;
	#endif

}TParaSafeRom;
////////////////////////////////////////////////////////////////
//以下是连续空间参数
////////////////////////////////////////////////////////////////
//645-2007有关电表型号等方面的参数
typedef struct TMeterTypePara_t
{
    //698.45协议版本号(数据类型:WORD)  13	 - 为避免对齐问题，故放到首位置-jwh
	WORD ProtocolVersion;
	//资产管理编码 ascii码 3
	BYTE PropertyCode[32];
	//额定电压 ascii码     4
	BYTE RateVoltage[6];
	//额定电流 ascii码     5
	BYTE RateCurrent[6];
	//最大电流 ascii码     6
	BYTE MaxCurrent[6];
	//最小电流 ascii码     7
	BYTE MinCurrent[10];
	//转折电流 ascii码     8
	BYTE TurningCurrent[10];
	//有功精度等级 ascii码 9
	BYTE PPrecision[4];
	//无功精度等级 ascii码 10
	BYTE QPrecision[4];
	//电表型号 ascii码     13
	BYTE MeterModel[32];
	//生产日期 date_time_s    14 645 698共有 存HEX数据，读ASCII时转一下 
	BYTE ProduceDate[sizeof(TRealTimer)];
	//电表位置信息 经度 纬度 高度 17
	BYTE MeterPosition[18];
	//厂家软件版本号 ascii码 19
	BYTE SoftWareVersion[4];
	//厂家软件版本日期	20
	BYTE SoftWareDate[6];
	//厂家硬件版本号 ascii码 21
	BYTE HardWareVersion[4];
	//厂家硬件版本日	22
	BYTE HardWareDate[6];
	//厂家编号  ascii码       18
	BYTE FactoryCode[4];
	//软件备案号  ASCII码     23
	BYTE SoftRecord[16];
	
}TMeterTypePara;


typedef struct TContinueMeterPara_t
{
	WORD			ConstParaSum;	//保存在ee的保护区校验和，当完全电表初始化时记录下此数值
	TMeterTypePara MeterTypePara;
	
}TParaConRom;

//4G模块
typedef struct
{
	//GPRS参数 IP地址(4)、端口号(2)、APN(16)、累计和(1)
	BYTE IP_Port_APN[23];
	//GPRS参数 行政区码(2)、终端地址(2)、累计和(1)
	BYTE AreaCode_Ter[5];
	//GPRS参数 模块APN用户名(32)
	BYTE APN_UseName[32];
	//GPRS参数 模块APN登录密码(32)、用户名密码累计和(1)
	BYTE APN_PassWord[33];
}TGprsTypePara;

typedef struct
{
	TGprsTypePara TcpipPara;
	TGprsTypePara MqttPara;
	BYTE Reserved[128];
}TGprsConRom;
////////////////////////////////////////////////////////////
//电表参数类型定义 1~15的值不要改动，以和645中顺序一致，方便调用wlk 2017/2/16 16-20顺序请勿改变 -jwh
////////////////////////////////////////////////////////////
typedef enum
{
	//电表通讯地址          1
	eMeterCommAddr = 1,
	//电表表号               2
	eMeterMeterNo,
	//资产管理编码 ascii码 3
	eMeterPropertyCode,
	//额定电压 ascii码     4
	eMeterRateVoltage,
	//额定电流 ascii码     5
	eMeterRateCurrent,
	//最大电流 ascii码     6
	eMeterMaxCurrent,
	//最小电流 ascii码  7
	eMeterMinCurrent,
	//转折电流 ascii码     8
	eMeterTurningCurrent,
	//有功精度等级 ascii码 9
	eMeterPPrecision,
	//无功精度等级 ascii码 10
	eMeterQPrecision,
	//有功脉冲常数         11
	eMeterActiveConstant,
	//无功脉冲常数         12
	eMeterReactiveConstant,
	//电表型号 ascii码  13
	eMeterMeterModel,
	//生产日期 date_time_s     14
	eMeterProduceDate,
	//698.45协议版本号(数据类型:WORD)   15
	eMeterProtocolVersion,
	//客户编号             16
	eMeterCustomCode,
	//电表位置信息 经度 纬度 高度 17
	eMeterMeterPosition,
	//厂家编号  ascii码      18
	eMeterFactoryCode,
	//厂家软件版本号 ascii码 19
	eMeterSoftWareVersion,
	//厂家软件版本日期				20
	eMeterSoftWareDate,
	//厂家硬件版本号 ascii码 21
	eMeterHardWareVersion,
	//厂家硬件版本日期				22
	eMeterHardWareDate,

	//软件备案号          23
	eMeterSoftRecord,
	
}TMeterTypeEnum;

typedef enum
{
	//IP地址、端口号、APN
	eTCPIP_IP_Port_APN = 0,
	//GPRS参数 行政区码、终端地址
	eTCPIP_AreaCode_Ter,
	//GPRS参数 模块APN用户名
	eTCPIP_APN_UseName,
	//GPRS参数 模块APN登录密码
	eTCPIP_APN_PassWord,
	//IP地址、端口号、APN
	eMQTT_IP_Port_APN,
	//GPRS参数 行政区码、终端地址
	eMQTT_AreaCode_Ter,
	//GPRS参数 模块APN用户名
	eMQTT_APN_UseName,
	//GPRS参数 模块APN登录密码
	eMQTT_APN_PassWord,
}TGprsTypeEnum;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TFPara1* const FPara1;
extern TFPara2* const FPara2;


//System 系统配置参数

//Sample 采样默认参数
extern const WORD Uoffset;		
extern const WORD Defendenergyvoltage;	
extern const WORD Def_Shielding_I1;	
extern const WORD Def_Shielding_I2;	
extern const WORD Def_MaxError;
extern const DWORD	MaxEnergyPulseConst;
extern const WORD SimpleCurrConst;
extern const WORD SimpleVolConst;
extern const WORD IRegionConst;
extern const WORD wSampleVolGainConst;
extern const WORD wSampleCurGainConst;
extern const WORD	wStandardVoltageConst;
extern const WORD	wCaliVoltageConst;
//extern const WORD	wMeterBasicCurrentConst;
extern const WORD wSampleMinCurGainConst; //采样芯片电流增益倍数
extern const WORD IRegionUPConst;
extern const WORD IRegionDownConst;
extern const DWORD dwMeterBasicCurrentConst;
extern const DWORD	dwMeterMaxCurrentConst;
#if( MAX_PHASE == 3 )
#if( SAMPLE_CHIP==CHIP_HT7627 )
extern const WORD ZeroSampleCurrConst;
extern const BYTE SampleCtrlAddr[15];
extern const BYTE SampleAdjustAddr[22];
extern const BYTE SampleOffsetAddr[19];
extern const WORD SampleCtrlDefData[15];
extern const WORD SampleAdjustDefData[22];
extern const WORD SampleOffsetDefData[20];
extern const WORD SampleHalfWaveDefData[12];
extern const WORD SampleTCcoffDefData[3];
extern const BYTE PhaseBRegAddr[9];
extern const WORD HD2CheckCurrentValueMin;
extern const WORD HD2CosLimitValue;
extern const WORD HD2CurrentDiffValue;
extern const BYTE HD2ContentMin;
extern const BYTE HD2ContentMax;
extern const BYTE HD2DiffValue;
extern const BYTE FirstDiffValue;
extern const WORD HD2CurrentRangeMin;
extern const WORD HD2CurrentRangeMax;
extern const BYTE HalfWaveDefCheckTime;

#elif( SAMPLE_CHIP==CHIP_RN8302B )
extern const WORD ZeroSampleCurrConst;
extern const DWORD SampleCtrlAddr[18][2];
extern const DWORD SampleAdjustAddr[33][2];
//extern const BYTE SampleOffsetAddr[19];
extern const DWORD SampleCtrlDefData[18];
extern const DWORD SampleAdjustDefData[33];
extern const DWORD SampleUIPGain[11];
//extern const WORD SampleOffsetDefData[20];
extern const WORD SampleHalfWaveDefData[12];
extern const WORD SampleTCcoffDefData[3];
extern const BYTE PhaseBRegAddr[9];
extern const WORD HD2CheckCurrentValueMin;
extern const WORD HD2CosLimitValue;
extern const WORD HD2CurrentDiffValue;
extern const BYTE HD2ContentMin;
extern const BYTE HD2ContentMax;
extern const BYTE HD2DiffValue;
extern const BYTE FirstDiffValue;
extern const WORD HD2CurrentRangeMin;
extern const WORD HD2CurrentRangeMax;
extern const BYTE HalfWaveDefCheckTime;
#endif

#else
extern const BYTE SampleCtrlAddr[7];
extern const BYTE SampleAdjustAddr[15];
extern const WORD SampleCtrlDefData[8];
extern const WORD SampleCtrlDefData8000[8];
extern const WORD SampleAdjustDefData[15];
#endif
//LCD 显示默认参数
extern const TLCDPara LCDParaDef;
#if( LCD_TYPE == LCD_HT_SINGLE_METER )
extern const BYTE LoopItemDef[15];
extern const BYTE KeyItemDef[60];
#endif
#if( LCD_TYPE == LCD_NO )
extern const BYTE LoopItemDef[35];
extern const BYTE KeyItemDef[110];
#endif
extern const BYTE ExpandLcdItemTable[20][10];
//Protocol 通信默认参数
extern const WORD BlePayload;

//Lpf 负荷曲线默认参数
//Demand 需量默认参数
//Event 事件默认参数
extern const BYTE	EventLostVTimeConst;
extern const BYTE  EventLostVURateConst;
extern const BYTE  EventLostVIRateConst;
extern const BYTE  EventLostVRecoverLimitVRateConst;
extern const BYTE	EventWeakVTimeConst;
extern const BYTE  EventWeakVURateConst;
extern const BYTE	EventOverVTimeConst;
extern const BYTE  EventOverVURateConst;
extern const BYTE	EventBreakTimeConst;
extern const BYTE  EventBreakURateConst;
extern const BYTE  EventBreakIRateConst;
extern const BYTE	EventLostITimeConst;
extern const BYTE  EventLostIURateConst;
extern const BYTE  EventLostIIRateConst;
extern const BYTE  EventLostILimitLIConst;
extern const BYTE	EventOverITimeConst;
extern const BYTE  EventOverIIRateConst;
extern const BYTE	EventBreakITimeConst;
extern const BYTE  EventBreakIURateConst;
extern const BYTE  EventBreakIIRateConst;
extern const BYTE	EventBackpTimeConst;
extern const BYTE  EventBackpPRateConst;
extern const BYTE	EventOverLoadTimeConst;
extern const BYTE  EventOverLoadPRateConst;
extern const BYTE	EventDemandOverTimeConst;
extern const BYTE  EventDemandOverRateConst;
extern const BYTE	EventCosOverTimeConst;
extern const WORD  EventCosOverRateConst;
extern const BYTE	EventLostAllVTimeConst;
extern const BYTE	EventLostSecPowerTimeConst;
extern const BYTE	EventVReversalTimeConst;
extern const BYTE	EventIReversalTimeConst;
extern const BYTE	EventLostPowerTimeConst;
extern const BYTE	EventVUnbalanceTimeConst;
extern const WORD  EventVUnbalanceRateConst;
extern const BYTE	EventIUnbalanceTimeConst;
extern const WORD  EventIUnbalanceRateConst;
extern const BYTE	EventISUnbalanceTimeConst;
extern const BYTE	EventSampleChipErrTimeConst;   
extern const WORD  EventISUnbalanceRateConst;
extern const BYTE  EventStatVRateConst;
extern const BYTE  EventStatVRunLimitLVConst;
extern const BYTE  EventStatVRunRecoverHVConst;
extern const BYTE  EventStatVRunRecoverLVConst;
extern const BYTE  EventNeutralCurrentErrTimeConst;
extern const DWORD EventNeutralCurrentErrLimitIConst; 
extern const WORD  EventNeutralCurrentErrRatioConst;
//罗氏线圈变更事件
#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )
extern const WORD	EventRogowskiADelayConst;
extern const WORD   EventRogowskiBDelayConst;
#endif
//终端抄表失败事件
#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
extern const BYTE	EventRetryNoConst;
extern const BYTE   EventCollectNoConst;
#endif

//PrePay 预付费默认参数
//Freeze 冻结默认参数

//Energy 电能默认参数
//Para 参数默认参数
extern const DWORD ActiveConstantConst;

extern const TEnereyDemandMode EnereyDemandModeConst;
extern const TCommPara CommParaConst;
extern const BYTE MonBillParaConst[];
extern const TTimeZoneSegPara TimeZoneSegParaConst;
// extern const TTimeAreaTable TimeAreaTableConst1;
// extern const TTimeAreaTable TimeAreaTableConst2;
extern const TTimeSegTable TimeSegTableConst1;
extern const TTimeSegTable TimeSegTableConst2;
extern const TTimeSegTable TimeSegTableConst3;
extern const TTimeSegTable TimeSegTableConst4;
extern const BYTE TimeSegTableFlag[];
// extern const TSwitchTimePara SwitchTimeParaConst;
extern const TMuchPassword MuchPasswordConst;
extern const TWarnDispPara WarnDispParaConst;
extern const BYTE RateVoltageConst[6];
extern const BYTE RateCurrentConst[6];
extern const BYTE MaxCurrentConst[6];
extern const BYTE MinCurrentConst[10];
extern const BYTE TurnCurrentConst[10];
extern const BYTE PPrecisionConst[4];
extern const BYTE QPrecisionConst[4];
extern const BYTE MeterModelConst[32];
extern const BYTE MeterPositionConst[18];
extern const WORD ProtocolVersionConst;
extern const BYTE ProtocolVersionConst_645[16];
extern const BYTE ProtocolVersionConst_698[21];
extern const BYTE SoftWareVersionConst[4];
extern const BYTE SoftWareDateConst[6];
extern const BYTE HardWareVersionConst[4];
extern const BYTE HardWareDateConst[6];
extern const BYTE FactoryCodeConst[4];
extern const BYTE SoftRecordConst[16];
extern const BYTE DefCurrRatioConst;
extern const WORD TimeBroadCastTimeMinLimit; 						//广播校时最小响应限制 单位秒
extern const WORD TimeBroadCastTimeMaxLimit;						//广播校时最大响应限制 单位秒
extern const DWORD	IRTimeConst;
extern const BOOL  RelayKeepPowerFlag;
#if( SEL_DLT645_2007 == YES )
extern const DWORD Remote645AuthTimeConst;
#endif
extern const BYTE	ScretKeyNum;   
extern const BYTE	LadderNumConst;
#if( PREPAY_MODE == PREPAY_LOCAL )
extern const BOOL  RelayKeepPowerFlag;
extern const DWORD	TickLimitConst;  
extern const DWORD	RegrateLimitConst;
extern const DWORD	CloseLimitConst;
#endif
//Rtc 时钟默认参数
extern const signed short TAB_Temperature[];
extern const unsigned short TAB_DFx_K[10];
extern const long ADCINcoffConst_K;
extern const long ADCINOffsetConst_K;
extern const long VBatcoffConst_K;
extern const long VBatOffsetConst_K;
extern const long VcccoffConst_K;
extern const long VccOffsetConst_K;
extern const long TPScoffConst_K;
extern const long TPSOffsetConst_K;

extern const WORD WaitOSCRunTime;

//LowPower 低功耗默认参数
//SysWatch 监视默认参数
extern const WORD ReadBatteryStandardVoltage;
extern const WORD ClockBatteryStandardVoltage;

//Relay 继电器默认参数
#if( SEL_DLT645_2007 == YES )
extern const WORD	wRelayWaitOffTime_Def;	
#endif
extern const WORD	OverIProtectTime_Def;	
extern const DWORD	RelayProtectI_Def;	
extern const BYTE	RelayCtrlMode_Def;
extern const BYTE	MeterReportFlagConst;
extern const BYTE	MeterActiveReportFlagConst;
extern const BYTE	MeterReportStatusFlagConst;
//默认事件上报方式为主动上报的列表
extern const BYTE	ReportActiveMethodConst[15+1];
//默认需上报事件及上报时刻列表
extern const TReportModeConst	ReportModeConst[15+1];
extern const BYTE	FollowReportModeConst[4];
extern const BYTE	FollowReportChannelConst[MAX_COM_CHANNEL_NUM+1];
extern const DWORD ChannelOAD[];
#if(SEL_EVENT_DI_CHANGE == YES)
extern const TDIPara DIParaconst;
#endif
extern TGlobalVariable GlobalVariable;
extern const DWORD HT7627SUConst;
extern const DWORD HT7627SIConst;

//4G模块默认参数
extern const BYTE TCPIP_IP_Port_APNConst[23];
extern const BYTE TCPIP_AreaCode_TerConst[5];
extern const BYTE TCPIP_APN_UseNameConst[32];
extern const BYTE TCPIP_APN_PassWordConst[33];

extern const BYTE MQTT_IP_Port_APNConst[23];
extern const BYTE MQTT_AreaCode_TerConst[5];
extern const BYTE MQTT_APN_UseNameConst[32];
extern const BYTE MQTT_APN_PassWordConst[33];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 刷新参数在RAM中的参数
//
//参数: 
//			No[in]					0--FPara1  
//									1--FPara2 
//                    
//返回值:  	TRUE: 操作正确 	FALSE：操作错误
//备注:  读出eeprom中的数据更新当前ram中的参数 
//-----------------------------------------------
BOOL api_FreshParaRamFromEeprom( WORD No );


//-----------------------------------------------
//函数功能: 把参数写入FPara1和FPara2及其他当前ram，并且存储到相应的eeprom中
//
//参数: 
//			No[in]					0：FPara1  1：FPara2
//			Addr[in]				结构内的偏移地址
//			Length[in]				要写入的数据长度
//			Ptr[in]					存放要写入的数据
//                    
//返回值:  	TRUE: 操作正确 	FALSE：操作错误
//备注:   
//-----------------------------------------------
BOOL api_WritePara(WORD No, WORD Addr, WORD Length, BYTE * Ptr);

//-----------------------------------------------
//函数功能: 读写4G模块相关参数
//
//参数: 
//			Operation[in]			操作方式 只能是WRITE或READ
//			Type[in]				需要读的数据类型，Type类型的序号定义不要改，是按645-2007的顺序定义的
//			pBuf[in]				读出数据的缓冲
//                    
//返回值:  	返回操作数据的长度，如果写失败，返回0
//
//备注:  	
//-----------------------------------------------
BYTE api_ProcGprsTypePara( BYTE Operation, TGprsTypeEnum Type, BYTE *pBuf);

//-----------------------------------------------
//函数功能: 读电表的一些相关参数
//
//参数: 
//			Operation[in]			操作方式 只能是WRITE或READ
//			Type[in]				需要读的数据类型，Type类型的序号定义不要改，是按645-2007的顺序定义的
//			pBuf[in]				读出数据的缓冲
//                    
//返回值:  	返回操作数据的长度，如果写失败，返回0
//
//备注:  	
//-----------------------------------------------
BYTE api_ProcMeterTypePara( BYTE Operation, BYTE Type, BYTE *pBuf );

//-----------------------------------------------
//函数功能: 读当前、备用套的时区表、时段表、公共假日表
//
//参数: 	Type[in]				
//						D7:	0--第一套时区时段表 1--第二套时区时段表
//						D6--D0:
//							1 -- TTimeAreaTable
//							2 -- TTimeHolidayTable
//							3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable
//			Addr[in]	在相应结构中的偏移
//			Length[in]	要读出内容的长度
//			pBuf[out]	读出数据的缓冲
//                    
//返回值:  	TRUE:正确	FALSE:错误
//
//备注:  	
//-----------------------------------------------
BOOL api_ReadTimeTable(BYTE Type, WORD Addr, WORD Length, BYTE * Buf);

//-----------------------------------------------
//函数功能: 写入当前、备用套的时区表、时段表、公共假日表
//
//参数: 	Type[in]				
//						D7:	0--第一套时区时段表 1--第二套时区时段表
//						D6--D0:
//							1 -- TTimeAreaTable
//							2 -- TTimeHolidayTable
//							3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable
//			Addr[in]	在相应结构中的偏移
//			Length[in]	要写入内容的长度
//			pBuf[in]	写入数据的缓冲
//                    
//返回值:  	TRUE:正确	FALSE:错误
//
//备注:  	
//-----------------------------------------------
BOOL api_WriteTimeTable(BYTE Type, WORD Addr, WORD Length, BYTE * Buf);

//-----------------------------------------------
//函数功能: 得到当前费率
//
//参数: 	无
//                   
//返回值:   BYTE:返回当前费率
//
//备注:   
//-----------------------------------------------
BYTE api_GetCurRatio( void );

//-----------------------------------------------
//函数功能: 参数大循环任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_ParaTask( void );

//-----------------------------------------------
//函数功能: 参数任务上电初始化
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_PowerUpPara( void );

//-----------------------------------------------
//函数功能: 检测时段表，每分钟调用一次
//
//参数: 
//			Type[in]				TRUE 转存  FALSE 不转存
//                    
//返回值:  	无
//备注:   
//-----------------------------------------------
void api_CheckTimeTable(WORD Type);

//-----------------------------------------------
//函数功能: 电表初始化
//
//参数:		Type[in]	0--完全初始化	1--初始化除了显示、冻结、事件外的参数
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_FactoryInitTask( void );

//-----------------------------------------------
//函数功能: 对脉冲常数进行合法性判断
//
//参数:		无
//						
//返回值:	有功脉冲常数
//		   
//备注:
//-----------------------------------------------
DWORD api_GetActiveConstant( void );
//-----------------------------------------------
//函数功能: 获取电表运行特征字1
//
//参数:		bit--具体第几位
//						
//返回值:	三种功能(bit0~2)分别的置位情况
//		   
//备注:
//		bit2主动上报模式 (0不启用后续标志，1启用后续标志)
//		bit1液晶①②字样意义 (0显示1、2套时段，1显示1、2套费率)
//		bit0外置开关控制方式(0电平，1脉冲）
//
//-----------------------------------------------
DWORD api_GetMeterRunFeature1(BYTE bit);
//---------------------------------------------------------------
//函数功能: 得到切换时间的相对分钟数，并返回是否越界
//
//参数: 	
//			TmpRealTimer[in] --切换时间，格式YYYYMMDDHHmm，HEX				
//			pdwTime[out]--切换时间，相对于2000年1月1日0时0分的相对分钟数
//                    
//返回值:  	TRUE--数据合格，FALSE--数据越界
//
//备注:   
//---------------------------------------------------------------
BOOL api_GetSwitchTimeRelativeMin( DWORD *pdwTime, TRealTimer TmpRealTimer );
//---------------------------------------------------------------
//函数功能: 读写切换时间
//
//参数: 	
//			Operation[in]	操作方式 只能是WRITE或READ
//			Type[in]				
//							0--两套时区表切换时间
// 							1--两套日时段切换时间
// 							2--两套分时费率切换时间
// 							3--两套阶梯切换时间 
// 							4--两套阶梯时区表切换时间(南网)
//										
//			pBuf[in]		读出或者写入数据的缓冲HEX(YYMMDDhhmm) YY为两个字节 带20 比如2017
//                    
//返回值:  	TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL api_ReadAndWriteSwitchTime( BYTE Operation, WORD Type, BYTE *pTmpArray );
//-----------------------------------------------
//函数功能: 将Para1参数从E2恢复
//
//参数:		无
//						
//返回值:	
//		   
//备注:低功耗上电后写时间前调用
//-----------------------------------------------
void api_ReadPara1FromEE(void);


#endif//#ifndef __PARA_API_H
