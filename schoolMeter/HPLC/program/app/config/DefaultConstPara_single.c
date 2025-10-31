//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.9.20
//描述		保护区
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

#if( MAX_PHASE == 1 )
//-----------------------------------------------
//			System 系统配置参数
//-----------------------------------------------
//---以下关联赋值语句不允许修改------
const BYTE MeterTypesConst = cMETER_TYPES;
const BYTE MeterCurrentTypesConst = cCURR_TYPES;
const BYTE RelayTypeConst = cRELAY_TYPE;
const BYTE SpecialClientsConst = cSPECIAL_CLIENTS;
const WORD SelThreeBoard = BOARD_TYPE;      //CPU板,对单相表没意义，主要是考虑三相模块和485表的程序兼容
const BYTE MeterVoltageConst = cMETER_VOLTAGE;  //电表电压等级
const BYTE SelZeroCurrentConst = YES;//零线电流检测
const BYTE SelSecPowerConst = NO;//单相表没有辅助电源
const BYTE SelOscTypeConst = cOSCILATOR_TYPE;//选择晶体类型
//---关联赋值语句不允许修改end------

//------以下配置可根据情况修改------
const WORD SoftVersionConst = 0xff47;   	//前两位代表省份，第三位代表年，第四位代表招标或送测月份
const BYTE SelVolCorrectConst = NO;		//电压影响量开关 ---供货时NO关闭----送样YES开启---
const BYTE SelDivAdjust =  NO;				//三相表参数 单相表没有实际意义const BYTE SelMChipVrefgain = NO;		    //三相表参数 单相表没有实际意义
const BYTE SelMChipVrefgain = NO;		    //三相表参数 单相表没有实际意义const BYTE PowerUpSyncMoneyBagConst = NO;	//上电是否需要同步钱包，高分需要选NO，为了降低功耗。其他情况选择YES
const BYTE ZeroCTPositive = NO;				//单相零线电流互感器正反接开关 此参数固定为NO, 不能被更改！！！除非硬件改变---YES: 正接---NO: 反接---
const BYTE ESAMPowerControlConst = YES;		//上电ESAM电源控制时间 ---供货时NO关闭 电源开启120s----送样YES开启--电源开启3s
//------配置可根据情况修改end------


//-----------------------------------------------
//			protocol 配置参数
//-----------------------------------------------
const BYTE WATCH_SCI_TIMER = 65;//7 考虑手动组包读后续数据 wlk 2009-7-16
//接受超时时间，单位，毫秒，也就是在500毫秒后若没有接受到新的数据，则清接受缓冲
const WORD MAX_RX_OVER_TIME = 500/10;//定时器是10ms
//接收到发送的延时，毫秒为单位，接收到有效帧后，延时此时间在发送（认为程序处理不耗费时间）
const WORD RECE_TO_SEND_DELAY = 25 / 10 + 1; //定时器是10ms

//-----------------------------------------------
//			Sample 采样默认参数
//-----------------------------------------------
#if( cCURR_TYPES == CURR_60A )
const WORD wMeterBasicCurrentConst = 5000;//基本电流，3位小数
const DWORD dwMeterMaxCurrentConst = 60000; //最大电流，3位小数
#else
const WORD wMeterBasicCurrentConst = 10000; //基本电流，3位小数
const DWORD dwMeterMaxCurrentConst = 100000; //最大电流，3位小数
#endif
const WORD wStandardVoltageConst = 2200; //基本电压，1位小数
const WORD wCaliVoltageConst = 2200; //校表电压，1位小数
//默认电压系数偏移量
const WORD Uoffset = 0x0000;		
//如果为125000表示125v关闭计量,如果为0则禁止关闭计量(建议设置成0)
const WORD Defendenergyvoltage = 0;	
//允许火线清零最大值15mA
const WORD Def_Shielding_I1 = 150;	
//允许零线清零最大值20mA
const WORD Def_Shielding_I2 = 200;
//允许校表最大误差值	
const WORD Def_MaxError = 30;
//脉冲数累加非法判断	
const DWORD	MaxEnergyPulseConst = 100;//单相用，三相可以不判

const BYTE SampleCtrlAddr[7] = 
{
	0x40,	//EMUCFG
	0x41,	//FreqCFG
	0x42,	//ModuleEn
	0x43,	//ANAEN	
	0x45,	//IOCFG
	0x72,	//ANACON
	0x75,	//MODECFG
};
const BYTE SampleAdjustAddr[15] = 
{
	0x50,	//GPA
	0x51,	//GQA
	0x52,	//GSA
	0x54,	//GPB
	0x58,	//QPhsCal
	0x59,	//ADCCON
	0x5f,	//PStart
	0x61,	//HFConst
	0x64,	//DEC_Shift
	0x65,	//P1OFFSET
	0x69,	//IARMSOS
	0x6a,	//IBRMSOS
	0x6d,	//GPHs1
	0x6e,	//GPHs2
	0x76,	//P1OFFSETL
};
const WORD SampleCtrlDefData[8] = 
{	
	0x0000,	//EMUCFG
	0x0088,	//FreqCFG
	0x007e,	//ModuleEn
	0x0007,	//ANAEN	
	0x0000,	//IOCFG
	0x0031,	//ANACON
	0x0000,	//MODECFG
	0x0000,	//频率选择标志
};

const WORD SampleCtrlDefData8000[8] = 
{	
	0x0000,	//EMUCFG
	0x0000,	//FreqCFG
	0x007e,	//ModuleEn
	0x0187,	//ANAEN		Bit8-Bit7 Vref基准二阶调整寄存器（常温高温段）需根据不同继电器调整不同值 仅8000:1芯片
	0x0000,	//IOCFG
	0x0231, //ANACON	Bit10-Bit8 Vref基准一阶调整寄存器 需根据不同继电器调整不同值 仅8000:1芯片
	0x0000,	//MODECFG
	0x5555,	//频率选择标志
};

//5000:1的HT7017校表寄存器0x58(无功相位校正)和0x64(移采样点相位校正)参数不写入计量芯片
const WORD SampleAdjustDefData[15] = 
{
	0x0000,	//GPA
	0x0000,	//GQA
	0x0000,	//GSA
	0x0172,	//GPB 零线互感器
	#if( cRELAY_TYPE == RELAY_INSIDE )
	0xFF7C, //QPhsCal 无功相位校正(频率影响量)--内置
	#else
	0xFF7C, //QPhsCal 无功相位校正(频率影响量)
	#endif
	#if( cCURR_TYPES == CURR_60A )
	0x100C, //ADCCON 火线电流24倍增益
	#else
	0x000C, //ADCCON 火线电流16倍增益
	#endif
	0x0040,	//PStart
	#if(SEL_CRYSTALOSCILLATOR_55296M == YES)
	0x0093,	//HFConst
	#else
	0x00A0,	//HFConst
	#endif
	#if( cRELAY_TYPE == RELAY_INSIDE )
	0x001B, //DEC_Shift 移采样点相位校正(频率影响量)--内置
	#else
	0x0010, //DEC_Shift 移采样点相位校正(频率影响量)
	#endif
	0x0000,	//P1OFFSET
	0x0010, //IARMSOS	火线零漂默认校正值 防止电表在未校零漂的情况下出现零漂电流影响继电器的判断，从而影响车间的调试
	0x0006, //IBRMSOS	零线零漂默认校正值
	0x0000,	//GPHs1 火线相角
	0xEA1E,	//GPHs2 零线相角 零线互感器
	0x0000, //P1OFFSETL
};
//-----------------------------------------------
//			LCD 显示默认参数
//-----------------------------------------------

const TLCDPara LCDParaDef = 
{ 
	.PowerOnDispTimer = 5 , 			//上电全显时间  unsigned，
	.BackLightPressKeyTimer = 60 , 	//背光点亮时间  long-unsigned(按键时背光点亮时间)
	.BackLightViewTimer = 10 , 		//显示查看背光点亮时间  long-unsigned，
	.LcdSwitchTime = 30, 			//无电按键屏幕驻留最大时间  long-unsigned， 轮显时间，HEX 单位为秒
	.EnergyFloat = 2 , 				//电能小数点
	.DemandFloat = 4 , 				//功率（需量）小数点
	.Meaning12 = 0,					//显示12字样意义
#if(PREPAY_MODE == PREPAY_LOCAL)//本地表
	.DispItemNum = {7,27},			//循显，键显
#else							//远程费控表
	.DispItemNum = {5,23},			//循显，键显
#endif	
	.KeyDisplayTime = 0x003C,		//键显切换到循环显示的时间 显示类属性3参数
	.LoopDisplayTime = 0x0005,		//循环显示时每屏的显示时间 显示类属性3参数
};
//20规范背光不点亮，小铃铛与背光同步	
const TWarnDispPara WarnDispParaConst = 
{  	  
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 0 -- 报警继电器
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 1 -- LCD小铃铛报警控制参数
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 2 -- 液晶背光
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 3 -- 液晶ERR显示报警
};

//循显项目默认设置(15个) 0-置默认显示项 1~199采用LcdItemTable里面的列号，200~234采用ExpandLcdItemTable里面的列号，fe-此项不能初始化， 通过规约设置
const BYTE LoopItemDef[15]=                    
{
#if(PREPAY_MODE == PREPAY_LOCAL)
	1,//循显第1项	当前剩余金额
#endif
	62,//循显第2项	当前组合有功总高精度电量
	63,//循显第3项	当前组合有功尖高精度电量
	64,//循显第4项	当前组合有功峰高精度电量
	65,//循显第5项	当前组合有功平高精度电量
	66,//循显第6项	当前组合有功谷高精度电量	
#if(PREPAY_MODE == PREPAY_LOCAL)
	17,//循显第7项		当前电价
#endif
};

//键显项目默认设置(60个) 0-置默认显示项 1~199采用LcdItemTable里面的列号，200~234采用ExpandLcdItemTable里面的列号，fe-此项不能初始化， 通过规约设置
const BYTE KeyItemDef[60] =                
{
#if(PREPAY_MODE == PREPAY_LOCAL)
	1,	//键显第1项 		剩余金额
#endif
	62,//键显第2项	当前组合有功总高精度电量
	63,//键显第3项	当前组合有功尖高精度电量
	64,//键显第4项	当前组合有功峰高精度电量
	65,//键显第5项	当前组合有功平高精度电量
	66,//键显第6项	当前组合有功谷高精度电量	
	77,//键显第7项 上1月组合有功总高精度电量
	78,//键显第8项	上1月组合有功尖高精度电量
	79,//键显第9项	上1月组合有功峰高精度电量
	80,//键显第10项	上1月组合有功平高精度电量
	81,//键显第11项	上1月组合有功谷高精度电量
	92,//键显第12项	上2月组合有功总高精度电量
	93,//键显第13项	上2月组合有功尖高精度电量
	94,//键显第14项	上2月组合有功峰高精度电量
	95,//键显第15项	上2月组合有功平高精度电量
	96,//键显第16项	上2月组合有功谷高精度电量
#if(PREPAY_MODE == PREPAY_LOCAL)
	17,//键显第17项	当前电价
	18,//键显第18项	用户户号低8位
	19,//键显第19项	用户户号高4位
#endif
	20,//键显第20项	通信地址低8位		40010200
	21,//键显第21项	通信地址高4位
	22,//键显第22项	当前日期			40000200
	23,//键显第23项	当前时间
	24,//键显第24项	瞬时电压			20000201
	25,//键显第25项	火线电流			20010201
	26,//键显第26项	瞬时功率			20040201
	27,//键显第27项	功率因数			200A0201
};
//扩展项，从200开始
const BYTE ExpandLcdItemTable[20][10] =
{
	{0X00, 0X00, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//扩展第1项	00000201--当前组合有功总电量 备用
};

//-----------------------------------------------
//			Protocol 通信默认参数
//-----------------------------------------------

//-----------------------------------------------
//			Lpf 负荷曲线默认参数
//-----------------------------------------------

//-----------------------------------------------
//			Demand 需量默认参数
//-----------------------------------------------

//-----------------------------------------------
//			PrePay 预付费默认参数
//-----------------------------------------------

//-----------------------------------------------
//			Freeze 冻结默认参数
//-----------------------------------------------
// 瞬时冻结
// 默认周期无效，3个点
const TFreezePara InstantFreeze[MAX_INSTANT_FREEZE_ATTRIBUTE+1]=
{
	{3, 0x00000003,	3 },		//冻结属性个数
	{0, 0x00100400, 3 }, 		//正向有功电能					20  
	{0, 0x00200400, 3 }, 		//反向有功电能					20
	{0, 0x20040200, 3 }, 		//有功功率					   	16	
};

// 分钟冻结
// 288个点   周期和深度均不能设置为0
const TFreezePara MinFreeze[MAX_MINUTE_FREEZE_ATTRIBUTE+1]=
{
	{8,  0x00000008, 8					 }, //冻结属性个数
	{15, 0x00100401, MAX_FREEZE_MIN_DEPTH },// 正向有功总电能
	{15, 0x00200401, MAX_FREEZE_MIN_DEPTH },// 反向有功总电能
	{15, 0x20000200, MAX_FREEZE_MIN_DEPTH },// 电压
	{15, 0x20010200, MAX_FREEZE_MIN_DEPTH },// 电流
	{15, 0x20010400, MAX_FREEZE_MIN_DEPTH },// 零线电流
	{15, 0x20040200, MAX_FREEZE_MIN_DEPTH },// 有功功率
	{15, 0x200A0200, MAX_FREEZE_MIN_DEPTH },// 功率因数
	{15, 0x48002400, FREEZE_MIN_DEPTH1 }, 	// 电器设备冻结数据单元
};

// 小时冻结
// 默认周期1小时，254个点
const TFreezePara HourFreeze[MAX_HOUR_FREEZE_ATTRIBUTE+1]=
{
	{2, 0x00000002,	2},					//冻结属性个数
	{1, 0x00100401, 254 },
	{1, 0x00200401, 254 }
};

// 日冻结
// 默认周期1天，62个点
const TFreezePara DayFreeze[MAX_DAY_FREEZE_ATTRIBUTE+1]=
{
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{5, 0x00000005,	5},					//冻结属性个数
	#else
	{3, 0x00000003,	3},					//冻结属性个数
	#endif
	{1, 0x00100400,  365 },// 正向有功电能
	{1, 0x00200400,  365 },// 反向有功电能
	{1, 0x20040200,  365 }, // 有功功率
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{1,	 0x202c0200, 365 },// 剩余金额、购电次数
	{1,	 0x202d0200, 365 },// 透支金额
	#endif
};

// 结算日冻结
// 默认周期无效，12个点
const TFreezePara ClosingFreeze[MAX_CLOSING_FREEZE_ATTRIBUTE+1]=
{	
	{4, 0x00000004,	4},		//冻结属性个数
	{1, 0x00000400, 12 }, //组合有功电能
	{1, 0x00100400, 12 }, //正向有功电能
	{1, 0x00200400, 12 }, //反向有功电能
	{1, 0x20310200, 12 }, //月度用电量 
};

// 月冻结
// 默认周期1月，12个点
const TFreezePara MonFreeze[MAX_MON_FREEZE_ATTRIBUTE+1]=
{
	{3, 0x00000003,	3},		//冻结属性个数
	{1, 0x00000400, 24 },	//组合有功电能					20
	{1, 0x00100400, 24 },	//正向有功电能				   	20
	{1, 0x00200400, 24 },	//反向有功电能				 	20
};

// 时区表切换冻结
// 默认周期无效，2个点
const TFreezePara TZChgFreeze[MAX_TIME_ZONE_FREEZE_ATTRIBUTE+1]=
{
	{3,  0x00000003, 3},		//冻结属性个数
	{1,  0x00100400, 2 },
	{1,  0x00200400, 2 },
	{1,  0x20040200, 2 },
};

// 日时段表切换冻结
// 默认周期无效，2个点
const TFreezePara DTTChgFreeze[MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE+1]=
{
	{3,  0x00000003, 3},		//冻结属性个数
	{1,  0x00100400, 2 },
	{1,  0x00200400, 2 },
	{1,  0x20040200, 2 }
};
#if(PREPAY_MODE == PREPAY_LOCAL)
// 费率电价切换冻结
// 默认周期无效，2个点
const TFreezePara TariffRateChgFreeze[MAX_TARIFF_RATE_FREEZE_ATTRIBUTE+1]=
{
	{3,  0x00000003, 3},		//冻结属性个数
	{1,  0x00100400, 2 },
	{1,  0x00200400, 2 },
	{1,  0x20040200, 2 }
};


// 阶梯切换冻结
// 默认周期无效，2个点
const TFreezePara LadderChgFreeze[MAX_LADDER_FREEZE_ATTRIBUTE+1]=
{
	{3,  0x00000003, 3},		//冻结属性个数
	{1,  0x00100400, 2 },
	{1,  0x00200400, 2 },
	{1,  0x20040200, 2 }
};
// 阶梯结算冻结
// 默认周期无效，12个点
const TFreezePara LadderClosingFreeze[MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE+1]=
{
	{1, 0x00000001,	1},//冻结属性个数
	{1, 0x20320200, 4 },//阶梯结算用电量				4	 
};
#endif
//-----------------------------------------------
//			Energy 电能默认参数
//-----------------------------------------------
//
//-----------------------------------------------
//			Para 参数默认参数
//-----------------------------------------------
//有功脉冲常数         9
#if( cCURR_TYPES == CURR_60A )
	const DWORD ActiveConstantConst = 2000;
#elif( cCURR_TYPES == CURR_100A )
	const DWORD ActiveConstantConst = 1000;
#else
	配置错误
#endif

const TEnereyDemandMode EnereyDemandModeConst =
{
	//有功组合方式特征字 组合有功
	.byActiveCalMode = 0x05,
	
	#if(SEL_RACTIVE_ENERGY == YES)
	//正向无功模式控制字
	.PReactiveMode = 0x05,
	//反向无功模式控制字
	.NReactiveMode = 0x50,
	#endif
	
	#if( MAX_PHASE != 1 )
	//需量周期
	.DemandPeriod = 15,	
	//需量滑差时间
	.DemandSlip = 1,
	//期间需量冻结周期
	.IntervalDemandFreezePeriod.FreezePeriod = 15,
	.IntervalDemandFreezePeriod.Unit = 1,
	#endif
};

//通信波特率
#if ( BOARD_TYPE == BOARD_HT_SINGLE_78201602 )
// 46:波特率9600; 4A: 115200
const TCommPara CommParaConst = 
{
	.I485 = 0x46,       	//第一路485
	.ComModule = 0x46,  	//模块
	.ComModuleDeflt = 0x46,	//模块恢复缺省值
	.II485 = 0x46,      	//第二路485（单相不用）
};
#else
//波特率115200
const TCommPara CommParaConst =
{
	.I485 = 0x4A,       	//第一路485----115200
	.ComModule = 0x4A,  	//模块-415----115200
	.ComModuleDeflt = 0x46,	//载波	  ----9600
	.II485 = 0x03,      	//蓝牙通信 ----2400
}; 
#endif

//结算日
const BYTE MonBillParaConst[] =
{
	01,00,
	99,99,
	99,99,
};

const TTimeZoneSegPara TimeZoneSegParaConst = 
{
	.TimeZoneNum = 1,		//年时区数，Hex码
	.TimeSegTableNum = 2,	//日时段表数，Hex码
	.TimeSegNum = 8,		//日时段数，Hex码
	.Ratio = 4,				//费率数，要小于最大费率数，Hex码，当前用户设置的最大费率数
	.HolidayNum = 0,		//公共假日数，Hex码 因为新规约要求2bytes n <= 254
	.WeekStatus = 0x7f,		//周休状态字（1-工作，0-休息）
	.WeekSeg = 0x01,		//周修日采用的时段表
};

const TTimeAreaTable TimeAreaTableConst1 =//当前套 
{
	//时段表号	日   月
	1,  1,  1,		//第1时区表
	1,  1,  1,		//第2时区表
	1,  1,  1,		//第3时区表
	1,  1,  1,		//第4时区表
	1,  1,  1,		//第5时区表
	1,  1,  1,		//第6时区表
	1,  1,  1,		//第7时区表
	1,  1,  1,		//第8时区表
	1,  1,  1,		//第9时区表
	1,  1,  1,		//第10时区表
	1,  1,  1,		//第11时区表
	1,  1,  1,		//第12时区表
	1,  1,  1,		//第13时区表
	1,  1,  1,		//第14时区表
};

const TTimeAreaTable TimeAreaTableConst2 =//备用套 
{
	//时段表号	日   月
	1,  1,  1,		//第1时区表
	1,  1,  1,		//第2时区表
	1,  1,  1,		//第3时区表
	1,  1,  1,		//第4时区表
	1,  1,  1,		//第5时区表
	1,  1,  1,		//第6时区表
	1,  1,  1,		//第7时区表
	1,  1,  1,		//第8时区表
	1,  1,  1,		//第9时区表
	1,  1,  1,		//第10时区表
	1,  1,  1,		//第11时区表
	1,  1,  1,		//第12时区表
	1,  1,  1,		//第13时区表
	1,  1,  1,		//第14时区表
};

const TTimeSegTable TimeSegTableConst1 = //第一张时段表
{
	//费率号   分   时
	0,  0,  1,		//第1时段
	3,  0,  2,		//第2时段
	6,  0,  3,		//第3时段
	9,  0,  4,		//第4时段
	12, 0,  1,		//第5时段
	15, 0,  2,		//第6时段
	18, 0,  3,		//第7时段
	21, 0,  4,		//第8时段
	21, 0,  4,		//第9时段
	21, 0,  4,		//第10时段
	21, 0,  4,		//第11时段
	21, 0,  4,		//第12时段
	21, 0,  4,		//第13时段
	21, 0,  4,		//第14时段
};

const TTimeSegTable TimeSegTableConst2 = //第二张时段表
{
	//费率号   分   时
	0,  0,  1,		//第1时段
	3,  0,  2,		//第2时段
	6,  0,  3,		//第3时段
	9,  0,  4,		//第4时段
	12, 0,  1,		//第5时段
	15, 0,  2,		//第6时段
	18, 0,  3,		//第7时段
	21, 0,  4,		//第8时段
	21, 0,  4,		//第9时段
	21, 0,  4,		//第10时段
	21, 0,  4,		//第11时段
	21, 0,  4,		//第12时段
	21, 0,  4,		//第13时段
	21, 0,  4,		//第14时段
};
const TTimeSegTable TimeSegTableConst3 = //第一张时段表
{
	//费率号   分   时
	0,  0,  1,		//第1时段
	3,  0,  2,		//第2时段
	6,  0,  3,		//第3时段
	9,  0,  4,		//第4时段
	12, 0,  1,		//第5时段
	15, 0,  2,		//第6时段
	18, 0,  3,		//第7时段
	21, 0,  4,		//第8时段
	21, 0,  4,		//第9时段
	21, 0,  4,		//第10时段
	21, 0,  4,		//第11时段
	21, 0,  4,		//第12时段
	21, 0,  4,		//第13时段
	21, 0,  4,		//第14时段
};

const TTimeSegTable TimeSegTableConst4 = //第二张时段表
{
	//费率号   分   时
	0,  0,  1,		//第1时段
	3,  0,  2,		//第2时段
	6,  0,  3,		//第3时段
	9,  0,  4,		//第4时段
	12, 0,  1,		//第5时段
	15, 0,  2,		//第6时段
	18, 0,  3,		//第7时段
	21, 0,  4,		//第8时段
	21, 0,  4,		//第9时段
	21, 0,  4,		//第10时段
	21, 0,  4,		//第11时段
	21, 0,  4,		//第12时段
	21, 0,  4,		//第13时段
	21, 0,  4,		//第14时段
};

const BYTE TimeSegTableFlag[] = 
{
    //时段表选择
	1, //第一套第1时段表选择默认表
	1, //第一套第2时段表选择默认表
	1, //第一套第3时段表选择默认表
	1, //第一套第4时段表选择默认表
	1, //第一套第5时段表选择默认表
	1, //第一套第6时段表选择默认表
	1, //第一套第7时段表选择默认表
	1, //第一套第8时段表选择默认表
	1, //第二套第1时段表选择默认表
	1, //第二套第2时段表选择默认表
	1, //第二套第3时段表选择默认表
	1, //第二套第4时段表选择默认表
	1, //第二套第5时段表选择默认表
	1, //第二套第6时段表选择默认表
	1, //第二套第7时段表选择默认表
	1, //第二套第8时段表选择默认表
};

const TSwitchTimePara SwitchTimeParaConst = 
{
	0x00L,
	0x00L,
	0x00L,
	0x00L,
	0x00L,
};

const TMuchPassword MuchPasswordConst =
{
	.Password645[0] = {0x00,0x00,0x00,0x00 },
	.Password645[1] = {0x00,0x00,0x00,0x00 },
	.Password645[2] = {0x00,0x00,0x00,0x00 },					//管理员02级密码出厂初始设置为000000
	.Password645[3] = {0x00,0x00,0x00,0x00 },
	.Password645[4] = {0x11, 0x11, 0x11, 0x11 },				//操作员04级密码出厂初始设置为111111
	.Password698 = { 0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},	//698密码初始化为全0 asics码
};

const WORD TimeBroadCastTimeMinLimit = 60; 						//广播校时最小响应限制 单位秒
const WORD TimeBroadCastTimeMaxLimit = 300;						//广播校时最大响应限制 单位秒

const DWORD	IRTimeConst = 30;               //红外认证时间
const BYTE	ScretKeyNum = 35;               //密钥条数
const BYTE	LadderNumConst = 0;             //阶梯数
#if(PREPAY_MODE == PREPAY_LOCAL)
const DWORD	TickLimitConst = 1000;          //透支金额门限
const DWORD	RegrateLimitConst = 99999999;   //囤积金额门限
const DWORD	CloseLimitConst = 200;          //合闸允许金额门限
#endif

//保电标志 TRUE:保电       FALSE:不保电
const BOOL  RelayKeepPowerFlag = FALSE;

#if( SEL_DLT645_2007 == YES )
const DWORD	Remote645AuthTimeConst = 30;
const BYTE  Report645ResetTimeConst = 30;   //645上报延时
//电表运行特征字1是否启用跟随上报 NO：不启用 YES:启用
//电表运行特征字1的其他参数位于lcd显示参数和继电器控制，需配置698使用
const BOOL  Meter645FollowStatusConst = FALSE;    
#endif
#if( cCURR_TYPES == CURR_60A )
	//额定电压 ascii码     4
	const BYTE RateVoltageConst[6] = {"220V"};
	//额定电流 ascii码     5
	const BYTE RateCurrentConst[6] = {"5A"};
	//最大电流 ascii码     6
	const BYTE MaxCurrentConst[6] = {"60A"};
	const BYTE MinCurrentConst[10] = {5,'0','.','2','5','A'};//最小电流 ascii码 
	const BYTE TurnCurrentConst[10] = {4,'0','.','5','A'};//转折电流 ascii码 
#elif( cCURR_TYPES == CURR_100A )
	//额定电压 ascii码     4
	const BYTE RateVoltageConst[6] = {"220V"};
	//额定电流 ascii码     5
	const BYTE RateCurrentConst[6] = {"10A"};
	//最大电流 ascii码     6
	const BYTE MaxCurrentConst[6] = {"100A"};
	const BYTE MinCurrentConst[10] = {4,'0','.','5','A'};//最小电流 ascii码 
	const BYTE TurnCurrentConst[10] = {2,'1','A'};//转折电流 ascii码 
#else
	"配置错误"！
#endif
	
//有功精度等级 ascii码 7
#if(PPRECISION_TYPE == PPRECISION_A )
const BYTE PPrecisionConst[4] = {"A"};
#elif(PPRECISION_TYPE == PPRECISION_C )
const BYTE PPrecisionConst[4] = {"C"};
#elif(PPRECISION_TYPE == PPRECISION_D )
const BYTE PPrecisionConst[4] = {"D"};
#else
const BYTE PPrecisionConst[4] = {"B"};
#endif

//无功精度等级 ascii码 8
const BYTE QPrecisionConst[4] = {""};
//电表型号 ascii码     11
const BYTE MeterModelConst[32] = {"DDZYXXX"};
//698.45协议版本号(数据类型:WORD)   13
const WORD ProtocolVersionConst = 0x0016;
//645协议版本号(数据类型:ASCII)   
const BYTE ProtocolVersionConst_645[16] = {"DL/T645-2007-14"};
//698协议版本号(数据类型:ASCII)   
const BYTE ProtocolVersionConst_698[21] = {20,'D','L','/','T','6','9','8','.','4','5','-','2','0','1','7','-','2','0','2','0'};//格式为len+数据
//厂家软件版本号 ascii码 16
const BYTE SoftWareVersionConst[4] = {"XXXX"};//{"V1.0"};
//厂家软件版本日期
const BYTE SoftWareDateConst[6] = {"XXXXXX"};//{"210504"};
//厂家硬件版本号 ascii码 17
const BYTE HardWareVersionConst[4] = {"XXXX"};//{"V1.0"};
//厂家硬件版本日期
const BYTE HardWareDateConst[6] = {"XXXXXX"};//{"210504"};
//厂家编号  ascii码       18
const BYTE FactoryCodeConst[4] = {"XXXX"};
//软件备案号 ASCII码      19
const BYTE SoftRecordConst[16] = {'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A'};
//默认当前费率时段
const BYTE DefCurrRatioConst = 3;//平
//-----------------------------------------------
//			Rtc 时钟默认参数
//-----------------------------------------------
#if( cOSCILATOR_TYPE == OSCILATOR_XTC )
//HT602xK+西铁城：
const unsigned short TAB_DFx_K[10] =
{
	0x0000, 0x0000,
	0x007F, 0xD9B3,
	0x007E, 0xCDDF,
	0x0000, 0x44D4,
	0x007F, 0xF220
};
#elif(cOSCILATOR_TYPE == OSCILATOR_XTC_3215)
//HT602xK+西铁城3215封装：
const unsigned short TAB_DFx_K[10] =
{
	0x0000, 0x0000,
	0x007F, 0xD61E,	//-10722
	0x007E, 0xDCF2,	//-74510
	0x0000, 0x4BE5,	//19429
	0x0000, 0x0437	//1079
};
#elif(cOSCILATOR_TYPE == OSCILATOR_JG)
//HT602xK+精工：
const unsigned short TAB_DFx_K[10] =
{
	0x0000, 0x0000,
	0x007F, 0xDa4b,
	0x007E, 0xD9ac,
	0x0000, 0x4a2e,
	0x007F, 0xfc90
}; 

#elif(cOSCILATOR_TYPE == OSCILATOR_JG_3215)
//HT602xK+精工3215：
const unsigned short TAB_DFx_K[10] =
{
	0x0000, 0x0000,
	0x007F, 0xE173,//-7821
	0x007E, 0xFA1B,//-67045
	0x0000, 0x601E,//24606
	0x0000, 0x2563//9571
}; 
//晶体系数选择禁止使用#else
#endif

const signed short TAB_Temperature[] =
{
	22495,	4638,			//-50C,0C
	//-720,	-7862,			//+15C,+35C
	-720,	-10000,			//+15C,+35C
	-13220,	-31077,			//+50C,+100C
};

//-----------------------------------------------
//			板级参数
//-----------------------------------------------
//CHIP_E2_ST_HARD: ST E2硬件地址
//CHIP_E2_ST_SOFT: ST E2软件地址
//SelEEDoubleErrReportConst：YES两片EE都故障才上报，NO一片EE故障就上报
#if(BOARD_TYPE == BOARD_HT_SINGLE_78201602)	//9600的表硬件地址
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_HARD, NO}; 
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_78202201)	//115200的表为软件地址
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_SOFT, NO};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_78202202)	//高防护的表为软件地址
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_SOFT, NO};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_78202303)	//115200 碳膜按键 双E2 128
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_FM_SOFT, CHIP_E2_BL_HARD};
	const BYTE SelEEDoubleErrReportConst = YES;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_78202401)	//115200 碳膜按键 双E2 128
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_FM_SOFT, CHIP_E2_BL_HARD};
	const BYTE SelEEDoubleErrReportConst = YES;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_20250819)	//单相费控智能电能表（电动自行车充电自动识别）初版本
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_SOFT, CHIP_E2_ST_SOFT};
	const BYTE SelEEDoubleErrReportConst = NO;
#endif
//-----------------------------------------------
//			cpu默认参数
//-----------------------------------------------
//HT5025K默认参数
const long ADCINcoffConst_K = 2547;
const long ADCINOffsetConst_K = 39170;
const long VBatcoffConst_K = 17695;
const long VBatOffsetConst_K = -249270;
const long VcccoffConst_K = 17734;
const long VccOffsetConst_K = -1176300;
const long TPScoffConst_K = -282;
const long TPSOffsetConst_K = 1298520;

//等待OSC起振延时
const WORD WaitOSCRunTime = 97;
//-----------------------------------------------
//			LowPower 低功耗默认参数
//-----------------------------------------------

//-----------------------------------------------
//			SysWatch 监视默认参数
//-----------------------------------------------
const WORD ReadBatteryStandardVoltage = 640;
const WORD ClockBatteryStandardVoltage = 367;

//-----------------------------------------------
//			Relay 继电器默认参数
//-----------------------------------------------
#if( SEL_DLT645_2007 == YES )
const WORD	wRelayWaitOffTime_Def = 0;		//跳闸延时时间（分钟）
#endif

#if( cCURR_TYPES == CURR_60A )
const WORD	OverIProtectTime_Def = 1440;	//超电流门限保护延时时间 HEX 单位：分钟
const DWORD	RelayProtectI_Def = 300000;		//继电器拉闸电流门限值 HEX 单位：A，换算-4	
#else
const WORD	OverIProtectTime_Def = 0;    //超电流门限保护延时时间 HEX 单位：分钟
const DWORD	RelayProtectI_Def = 0;     	 //继电器拉闸电流门限值 HEX 单位：A，换算-4
#endif//#if( cCURR_TYPES == CURR_60A )

#if( cRELAY_TYPE == RELAY_INSIDE )
	const BYTE	RelayCtrlMode_Def = 0;			//继电器输出开关属性 0--脉冲 1--保持
#else
	const BYTE	RelayCtrlMode_Def = 1;			//继电器输出开关属性 0--脉冲 1--保持
#endif

//-----------------------------------------------
//			Event 事件默认参数
//-----------------------------------------------

//过流 0x3005 IMax本身保留了3位小数，过流门限值要求4位小数
#if( SEL_EVENT_OVER_I == YES )
const BYTE	EventOverITimeConst = 60;   //事件延时时间 60s
const BYTE  EventOverIIRateConst = 12; //1.2Imax,过流事件电流触发下限定值范围： 0.5～1.5Imax，最小设定值级差 0.0001A
#endif


//掉电 0x3011
#if( SEL_EVENT_LOST_POWER == YES )
const BYTE	EventLostPowerTimeConst = 0; //事件延时时间 60s
#endif

//计量芯片故障 0x302F
#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
const BYTE	EventSampleChipErrTimeConst = 60;   //事件延时时间 60s
#endif
//电能表零线电流异常 0x3040
#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
const BYTE	 EventNeutralCurrentErrTimeConst = 60; //事件延时时间 60s
const DWORD  EventNeutralCurrentErrLimitIConst = 20000; //20%;电流触发下限：最小设定值级差 0.001%
const WORD   EventNeutralCurrentErrRatioConst = 5000; //50%,零线电流异常下限范围： 10%～98%，最小设定值级差 0.01%
#endif
//-----------------------------------------------
//			Report 事件默认参数
//-----------------------------------------------

//默认不允许跟随上报 4300	电气设备 的属性7. 允许跟随上报
const BYTE	MeterReportFlagConst = FALSE;
//默认主动上报标识 4300	电气设备 的属性8. 允许主动上报
const BYTE	MeterActiveReportFlagConst = TRUE;

//默认跟随上报状态字 上报方式 0：主动上报  1：跟随上报
const BYTE	MeterReportStatusFlagConst = 1;
//默认事件上报方式为主动上报的列表
const BYTE	ReportActiveMethodConst[15+1] = 
{
	4,
	eEVENT_LOST_POWER_NO,		//掉电事件
	eEVENT_METERCOVER_NO,		//开表盖事件
	eEVENT_OVER_I_NO,			//过流事件
	eEVENT_RTC_ERR_NO,			//时钟故障
};
//跟随上报模式字
const BYTE	FollowReportModeConst[4] = 
{   
    0x00,//bit24~bit31
    0x00,//bit16~bit23
    0x00,//bit8~bit15       bit8停电抄表电池欠压、bit9透支状态、bit14跳闸成功、bit15合闸成功
    0x00,//bit0~bit7        bit1ESAM错误、bit3时钟电池电压低、bit5时钟故障
};
//跟随上报通道;
const BYTE	FollowReportChannelConst[MAX_COM_CHANNEL_NUM+1] = 
{
	1,			//第一个字节表示长度
	eCR,//模块通道
};
//eRS485_I,//第一路485
//eIR,//红外通道
//eCR,//模块通道
//eRS485_II,//第二路485
//待定!!!!!!
// 通道OAD与eSERIAL_TYPE的顺序一致
const DWORD ChannelOAD[] =
{
	0x010201F2,     //eRS485_I
	0x000202F2,     //eIR
	0x000209F2,     //eCR
	0x020201F2,     //eRS485_II
};

//此处顺序要和 eEVENT_INDEX 定义的顺序一致，且数组大小和eNUM_OF_EVENT_PRG数据一致
//不上报（0），事件发生上报（BIT0），事件恢复上报（BIT1），事件发生恢复均上报（BIT0|BIT1）
const TReportModeConst	ReportModeConst[15+1] =
{
	//事件枚举号 上报模式
	4, 4,
	eEVENT_LOST_POWER_NO,	(BIT0|BIT1),		//掉电事件
	eEVENT_METERCOVER_NO,	BIT0,		//开表盖事件
	eEVENT_OVER_I_NO,		BIT0,		//过流事件
	eEVENT_RTC_ERR_NO,		BIT0,		//时钟故障
	
};

//备用，不要修改
const Fun FunctionConst @"PRG_CONST_B" = api_DelayNop;
const BYTE a_byDefaultData[]@"PRG_CONST_C"= 
{
	0X40, 0X00, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //键显第1项	40000200--当前日期
	0X40, 0X00, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //键显第2项	40000200--当前时间
	0X00, 0X00, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第3项	00000201--当前组合有功总电量
	0X00, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第4项	00100201--当前正向有功总电量
	0X00, 0X10, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第5项	00100202--当前正向有功尖电量
	0X00, 0X10, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第6项	00100203--当前正向有功峰电量
	0X00, 0X10, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第7项	00100204--当前正向有功平电量
	0X00, 0X10, 0X02, 0X05, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第8项	00100205--当前正向有功谷电量
	0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //键显第9项	10100201--当前正向有功总最大需量
	0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //键显第10项	10100201--当前正向有功总最大需量发生日期
	0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X03, //键显第11项	10100201--当前正向有功总最大需量发生时间
	0X00, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第12项	00200201--当前反向有功总电量
	0X00, 0X20, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第13项	00200202--当前反向有功尖电量
	0X00, 0X20, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第14项	00200203--当前反向有功峰电量
	0X00, 0X20, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第15项	00200204--当前反向有功平电量
	0X00, 0X20, 0X02, 0X05, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第16项	00200205--当前反向有功谷电量
	0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //键显第17项	10200201--当前反向有功总最大需量
	0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //键显第18项	10200201--当前反向有功总最大需量发生日期
	0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X03, //键显第19项	10200201--当前反向有功总最大需量发生时间
	0X00, 0X30, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第20项	00300201--当前组合无功1总电量
	0X00, 0X40, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第21项	00400201--当前组合无功2总电量
	0X00, 0X50, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第22项	00500201--当前第1象限无功总电量
	0X00, 0X60, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第23项	00600201--当前第2象限无功总电量
	0X00, 0X70, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第24项	00700201--当前第3象限无功总电量
	0X00, 0X80, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第25项	00800201--当前第4象限无功总电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X01, 0x02, 0X00, //键显第26项	50050201 00100201--上1月正向有功总电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X02, 0x02, 0X00, //键显第27项	50050201 00100202--上1月正向有功尖电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X03, 0x02, 0X00, //键显第28项	50050201 00100203--上1月正向有功峰电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X04, 0x02, 0X00, //键显第29项	50050201 00100204--上1月正向有功平电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X05, 0x02, 0X00, //键显第30项	50050201 00100205--上1月正向有功谷电量
	0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X01, //键显第31项	50050201 10100201--上1月正向有功总最大需量
	0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X02, //键显第32项	50050201 10100201--上1月正向有功总最大需量发生日期
	0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X03, //键显第33项	50050201 10100201--上1月正向有功总最大需量发生时间
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X01, 0x02, 0X00, //键显第34项	50050201 00200201--上1月反向有功总电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X02, 0x02, 0X00, //键显第35项	50050201 00200202--上1月反向有功尖电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X03, 0x02, 0X00, //键显第36项	50050201 00200203--上1月反向有功峰电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X04, 0x02, 0X00, //键显第37项	50050201 00200204--上1月反向有功平电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X05, 0x02, 0X00, //键显第38项	50050201 00200205--上1月反向有功谷电量
	0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X01, //键显第39项	50050201 10200201--上1月反向有功总最大需量
	0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X02, //键显第40项	50050201 10200201--上1月反向有功总最大需量发生日期
	0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X03, //键显第41项	50050201 10200201--上1月反向有功总最大需量发生时间
	0X50, 0X05, 0X02, 0X01, 0X00, 0X50, 0X02, 0X01, 0x02, 0X00, //键显第42项	50050201 00500201--上1月第1象限无功总电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X60, 0X02, 0X01, 0x02, 0X00, //键显第43项	50050201 00600201--上1月第2象限无功总电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X70, 0X02, 0X01, 0x02, 0X00, //键显第44项	50050201 00700201--上1月第3象限无功总电量
	0X50, 0X05, 0X02, 0X01, 0X00, 0X80, 0X02, 0X01, 0x02, 0X00, //键显第45项	50050201 00800201--上1月第4象限无功总电量
	0X40, 0X01, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //键显第46项	40010200--通信地址低8位
	0X40, 0X01, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //键显第47项	40010200--通信地址高4位
	0XF2, 0X01, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //键显第48项	F2010201--通信波特率
	0X41, 0X09, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第49项	41090200--有功脉冲常数
	0X41, 0X0A, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第50项	410A0200--无功脉冲常数
	0X20, 0X13, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第51项	20130200--时钟电池使用时间
	0X30, 0X12, 0X02, 0X01, 0X20, 0X1e, 0X02, 0X00, 0x02, 0X01, //键显第52项	30120201 33020202--最近一次编程日期
	0X30, 0X12, 0X02, 0X01, 0X20, 0X1e, 0X02, 0X00, 0x02, 0X02, //键显第53项	30120201 33020202--最近一次编程时间
	0X30, 0X00, 0X0D, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第54项	30000D01--总失压次数、对象增加属性12
	0X30, 0X00, 0X0D, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第55项	30000D02--总失压累计时间、对象增加属性12
	0X30, 0X00, 0X0D, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //键显第56项	30000D03--最近一次失压起始日期
	0X30, 0X00, 0X0D, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //键显第57项	30000D03--最近一次失压起始时间
	0X30, 0X00, 0X0D, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //键显第58项	30000D04--最近一次失压结束日期
	0X30, 0X00, 0X0D, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //键显第59项	30000D04--最近一次失压结束时间
	0X30, 0X00, 0X07, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00, //键显第60项	30000701 00102201--最近一次A相失压起始时刻正向有功电量
	0X30, 0X00, 0X07, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00, //键显第61项	30000701 00108201--最近一次A相失压结束时刻正向有功电量
	0X30, 0X00, 0X07, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00, //键显第62项	30000701 00202201--最近一次A相失压起始时刻反向有功电量
	0X30, 0X00, 0X07, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00, //键显第63项	30000701 00208201--最近一次A相失压结束时刻反向有功电量
	0X30, 0X00, 0X08, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00, //键显第64项	30000801 00102201--最近一次B相失压起始时刻正向有功电量
	0X30, 0X00, 0X08, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00, //键显第65项	30000801 00108201--最近一次B相失压结束时刻正向有功电量
	0X30, 0X00, 0X08, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00, //键显第66项	30000801 00202201--最近一次B相失压起始时刻反向有功电量
	0X30, 0X00, 0X08, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00, //键显第67项	30000801 00208201--最近一次B相失压结束时刻反向有功电量
	0X30, 0X00, 0X09, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00, //键显第68项	30000901 00102201--最近一次C相失压起始时刻正向有功电量
	0X30, 0X00, 0X09, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00, //键显第69项	30000901 00108201--最近一次C相失压结束时刻正向有功电量
	0X30, 0X00, 0X09, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00, //键显第70项	30000901 00202201--最近一次C相失压起始时刻反向有功电量、
	0X30, 0X00, 0X09, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00, //键显第71项	30000901 00208201--最近一次C相失压结束时刻反向有功电量
	0X20, 0X00, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第72项	20000201--A相电压
	0X20, 0X00, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第73项	20000202--B相电压
	0X20, 0X00, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第74项	20000203--C相电压
	0X20, 0X01, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第75项	20010201--A相电流
	0X20, 0X01, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第76项	20010202--B相电流
	0X20, 0X01, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第77项	20010203--C相电流
	0X20, 0X04, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第78项	20040201--瞬时总有功功率
	0X20, 0X04, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第79项	20040202--瞬时A相有功功率
	0X20, 0X04, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第80项	20040203--瞬时B相有功功率
	0X20, 0X04, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第81项	20040204--瞬时C相有功功率
	0X20, 0X0A, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第82项	200A0201--瞬时总功率因数
	0X20, 0X0A, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第83项	200A0202--瞬时A相功率因数
	0X20, 0X0A, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第84项	200A0203--瞬时B相功率因数
	0X20, 0X0A, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第85项	200A0204--瞬时C相功率因数	
	0X41, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //键显第86项	41160201--结算日
};                                                        	
#endif//#if( MAX_PHASE == 1 )