//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.9.20
//描述		保护区
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

#if( MAX_PHASE == 3 )

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
#if( cMETER_TYPES == METER_ZT )//20规范三相表只有直通表支持零线电流计量
const BYTE SelZeroCurrentConst = YES;//零线电流检测
#else//三相三线电表没有零线电流
const BYTE SelZeroCurrentConst = NO;//零线电流检测
#endif
#if(cRELAY_TYPE == RELAY_NO)//现在只有智能表有辅助电源
const BYTE SelSecPowerConst = YES;
#else
const BYTE SelSecPowerConst = NO;
#endif
const BYTE SelOscTypeConst = cOSCILATOR_TYPE;//选择晶体类型
//---关联赋值语句不允许修改end------

//------以下配置可根据情况修改------
const WORD SoftVersionConst = 0xff4B;   //前两位代表省份，第三位代表年，第四位代表招标或送测月份
const BYTE SelDivAdjust = YES;			//是否采用分段校准方法  必须一直为YES
#if((BOARD_TYPE == BOARD_HT_THREE_0132515)&&(PPRECISION_TYPE == PPRECISION_D))
const BYTE SelMChipVrefgain = NO;		//是否开启计量芯片自身片内温补
#else
const BYTE SelMChipVrefgain = YES;		//是否开启计量芯片自身片内温补
#endif
const BYTE SelVolUnbalanceGain = YES; 	//是否开启电压不平衡补偿  ---供货时NO关闭----送样YES开启
const BYTE ESAMPowerControlConst = YES;	//上电ESAM电源控制时间，---供货时NO关闭 电源开启120s----送样YES开启--电源开启3s
const BYTE SmallCurrentCorrection = YES;//供货必须关闭
const BYTE SelVolChangeGain = YES;//电压改变补偿，供货必须关闭

#if( cMETER_TYPES == METER_ZT )
const BYTE HalfWaveDefCheckTime = 0xFF;				//直流偶次谐波默认检测时间(送样一直开启--0xFF  供货24小时)
#else
const BYTE HalfWaveDefCheckTime = 0x00;				//直流偶次谐波默认检测时间(一直关闭)
#endif

//------配置可根据情况修改end------


//-----------------------------------------------
//			protocol 配置参数
//-----------------------------------------------
const BYTE WATCH_SCI_TIMER = 65;//7 考虑手动组包读后续数据 wlk 2009-7-16
//接受超时时间，单位，毫秒，也就是在500毫秒后若没有接受到新的数据，则清接受缓冲
const WORD MAX_RX_OVER_TIME = 500/10;//定时器是10ms
//接收到发送的延时，毫秒为单位，接收到有效帧后，延时此时间在发送（认为程序处理不耗费时间）
const WORD RECE_TO_SEND_DELAY = 25/10+1;//定时器是10ms

//-----------------------------------------------
//			Sample 采样默认参数
//-----------------------------------------------

//零线电流计量芯片电压输入值
//基本电流 * 猛铜阻值 * 1000(单位mv)
const WORD ZeroSampleCurrConst = 1000;

//取样管脚电压 单位mv
//电压值 单位0.01mv 9个169k 1个33k 1个1k	(100000/1555)*10
#if( cMETER_TYPES == METER_ZT )	
	#if( cCURR_TYPES == CURR_60A )	
		const WORD wMeterBasicCurrentConst = 5000;//基本电流，3位小数
		const DWORD dwMeterMaxCurrentConst = 60000; //最大电流，3位小数
		const WORD SimpleCurrConst = 20;
		const WORD IRegionConst = 500;      //500%Ib
		const WORD wSampleCurGainConst = 1;	//采样芯片电流增益倍数
		const WORD SimpleVolConst = 721;		
		const WORD wSampleVolGainConst = 2;	//采样芯片电压增益倍数
	#elif( cCURR_TYPES == CURR_100A )
		const WORD wMeterBasicCurrentConst = 10000; //基本电流，3位小数
		const DWORD dwMeterMaxCurrentConst = 100000; //最大电流，3位小数
		const WORD SimpleCurrConst = 40;
		const WORD IRegionConst = 500;      //500%Ib
		const WORD wSampleCurGainConst = 1;	//采样芯片电流增益倍数
		const WORD SimpleVolConst = 721;		
		const WORD wSampleVolGainConst = 2;	//采样芯片电压增益倍数
		
	#endif		
		const WORD wStandardVoltageConst = 2200; //基本电压，1位小数
		const WORD wCaliVoltageConst = 2200; //校表电压，1位小数
#elif( cMETER_TYPES == METER_3P3W )	
	#if( cCURR_TYPES == CURR_6A )	
		const WORD wMeterBasicCurrentConst = 1500; //基本电流，3位小数
		const DWORD dwMeterMaxCurrentConst = 6000; //最大电流，3位小数
		#if( ( BOARD_TYPE == BOARD_HT_THREE_0130347 ) && (cRELAY_TYPE == RELAY_NO))	
		const WORD SimpleCurrConst = 94;
		#elif( ( BOARD_TYPE == BOARD_HT_THREE_0132515 ) && (cRELAY_TYPE == RELAY_NO))	
		const WORD SimpleCurrConst = 94;
		#else	
		const WORD SimpleCurrConst = 51;
		#endif
		const WORD IRegionConst = 60;       //60%Ib
		const WORD wSampleCurGainConst = 1;	//采样芯片电流增益倍数
		const WORD SimpleVolConst = 1730;		
		const WORD wSampleVolGainConst = 2;	//采样芯片电压增益倍数
		
	#elif( cCURR_TYPES == CURR_1A )
		const WORD wMeterBasicCurrentConst = 300; //基本电流，3位小数
		const DWORD dwMeterMaxCurrentConst = 1200; //最大电流，3位小数
		#if( ( BOARD_TYPE == BOARD_HT_THREE_0130347 ) && (cRELAY_TYPE == RELAY_NO))	
		const WORD SimpleCurrConst = 40;
		#elif( ( BOARD_TYPE == BOARD_HT_THREE_0132515 ) && (cRELAY_TYPE == RELAY_NO))	
		const WORD SimpleCurrConst = 40;
		#else
		const WORD SimpleCurrConst = 20;
		#endif
		const WORD IRegionConst = 80;       //80%Ib
		const WORD wSampleCurGainConst = 2;	//采样芯片电流增益倍数
		const WORD SimpleVolConst = 1730;		
		const WORD wSampleVolGainConst = 2;	//采样芯片电压增益倍数
	#endif
		const WORD wStandardVoltageConst = 1000; //基本电压，1位小数
		const WORD wCaliVoltageConst = 1000; //校表电压，1位小数
#else//( cMETER_TYPES == METER_3P4W )
	//如果是2018-0719之前的57.7v费控表 需确认是否需要SimpleVolConst改为154,wSampleVolGainConst改为8		!!!
	#if( cCURR_TYPES == CURR_6A )
		const WORD wMeterBasicCurrentConst = 1500; //基本电流，3位小数
		const DWORD dwMeterMaxCurrentConst = 6000; //最大电流，3位小数
		#if( ( BOARD_TYPE == BOARD_HT_THREE_0130347 ) && (cRELAY_TYPE == RELAY_NO))
		const WORD SimpleCurrConst = 94;
	#elif( ( BOARD_TYPE == BOARD_HT_THREE_0132515 ) && (cRELAY_TYPE == RELAY_NO))	
	const WORD SimpleCurrConst = 94;
		#else
		const WORD SimpleCurrConst = 51;
		#endif
		const WORD IRegionConst = 60;		//60%Ib
		const WORD wSampleCurGainConst = 1;	//采样芯片电流增益倍数
	#elif( cCURR_TYPES == CURR_1A )
		const WORD wMeterBasicCurrentConst = 300; //基本电流，3位小数
		const DWORD dwMeterMaxCurrentConst = 1200; //最大电流，3位小数
		#if( ( BOARD_TYPE == BOARD_HT_THREE_0130347 ) && (cRELAY_TYPE == RELAY_NO))
		const WORD SimpleCurrConst = 40;
	#elif( ( BOARD_TYPE == BOARD_HT_THREE_0132515 ) && (cRELAY_TYPE == RELAY_NO))	
	const WORD SimpleCurrConst = 40;
		#else
		const WORD SimpleCurrConst = 20;
		#endif
		const WORD IRegionConst = 80;		//80%Ib
		const WORD wSampleCurGainConst = 2;	//采样芯片电流增益倍数
	#endif
	
	#if(cMETER_VOLTAGE == METER_220V)
		const WORD SimpleVolConst = 721; //721;
		const WORD wSampleVolGainConst = 2; //2;	//采样芯片电压增益倍数
		const WORD wStandardVoltageConst = 2200; //基本电压，1位小数
		const WORD wCaliVoltageConst = 2200; //校表电压，1位小数
	#else
		const WORD SimpleVolConst = 3355; //721;
		const WORD wSampleVolGainConst = 2; //2;	//采样芯片电压增益倍数
		const WORD wStandardVoltageConst = 577; //基本电压，1位小数
		const WORD wCaliVoltageConst = 600; //校表电压，1位小数
	#endif
#endif

const WORD wSampleMinCurGainConst = 8; //采样芯片电流增益倍数

//采样芯片控制类寄存器地址
const BYTE SampleCtrlAddr[15] = 
{
	0x01,	//ModeCfg
	0x02,	//PGACtrl
	0x03,	//EMUCfg
	0x16,	//QPhsCal
	0x1d,	//IStartup
	0x1e,	//HFConst
	0x1f,	//FailVoltage
	0x30,	//EMUIE
	0x31,	//ModuleCfg
	0x32,	//AllGain
	0x33,	//HFDouble
	0x35,	//PinCtrl
	0x36,	//PStart
	0x37,	//IRegion
	0x70,	//EMCfg
};
//采样芯片增益类寄存器地址
const BYTE SampleAdjustAddr[22] = 
{
	0x04,0x05,0x06,	//PGain[3]
	0x07,0x08,0x09,	//QGain[3]
	0x0a,0x0b,0x0c,	//SGain[3]
	0x0d,0x0e,0x0f,	//PhsReg[0][3]
	0x10,0x11,0x12,	//PhsReg[1][3]
	0x17,0x18,0x19,	//UGain[3]
	0x1a,0x1b,0x1c,	//IGain[3]
	0x20,			//NGain
};
//采样芯片偏移类寄存器地址
const BYTE SampleOffsetAddr[19] = 
{
	0x13,0x14,0x15,	//POffset[3]
	0x64,0x65,0x66,	//POffsetL[3]
	0x21,0x22,0x23,	//QOffset[3]
	0x67,0x68,0x69,	//QOffsetL[3]
	0x24,0x25,0x26,	//URMSOffset[3]
	0x27,0x28,0x29,	//IRMSOffset[3]
	0x6A,
};
//采样芯片控制类寄存器默认数据
const WORD SampleCtrlDefData[15] = 
{
	//直通表有零线电流计量,SelZeroCurrentConst == YES
	#if( cMETER_TYPES == METER_ZT )
	0xbd7F,	//ModeCfg
	0x0103,	//PGACtrl
	#else
	0xbd7e,	//ModeCfg
	0x0100,	//PGACtrl
	#endif
	0x7dc4,	//EMUCfg
	0x0000,	//QPhsCal
	0x0160,	//IStartup
	0x0000,	//HFConst
	0x0960,	//FailVoltage
	0x0001,	//EMUIE
	0x3c37,	//ModuleCfg
	0x0000,	//AllGain
	0x0000,	//HFDouble
	0x000f,	//PinCtrl
	0x0000,	//PStart
	0x0000,	//IRegion
	0x0000,	//EMCfg
};

//采样芯片增益类寄存器默认数据
const WORD SampleAdjustDefData[] = 
{
	0x0000,0x0000,0x0000,	//PGain[3]
	0x0000,0x0000,0x0000,	//QGain[3]
	0x0000,0x0000,0x0000,	//SGain[3]
	0x0000,0x0000,0x0000,	//PhsReg[0][3]
	0x0000,0x0000,0x0000,	//PhsReg[1][3]
	0x0000,0x0000,0x0000,	//UGain[3]
	0x0000,0x0000,0x0000,	//IGain[3]
	0x0000,					//NGain
};
//采样芯片偏移类寄存器默认数据
const WORD SampleOffsetDefData[20] = 
{
	0x0000,0x0000,0x0000,	//POffset[3]
	0x0000,0x0000,0x0000,	//POffsetL[3]
	0x0000,0x0000,0x0000,	//QOffset[3]
	0x0000,0x0000,0x0000,	//QOffsetL[3]
	0x0000,0x0000,0x0000,	//URMSOffset[3]
	0x0007,0x0007,0x0007,	//IRMSOffset[3]
	0x0005,0x0000,			//电流矢量和偏置 零线电流偏置
};

//直流偶次谐波默认补偿系数
#if( cCURR_TYPES == CURR_100A )
const WORD SampleHalfWaveDefData[12] =
{
	0xF74C, 0x0036,			//Coe[0] -- 相位补偿值, 电流值(1位小数)
	0xF594, 0x0189,			//Coe[1] -- 相位补偿值, 电流值(1位小数)
	0xF29D, 0x0287,			//Coe[2] -- 相位补偿值, 电流值(1位小数)
	0x018D, 0x0036,			//Gain[0]-- 增益补偿值, 电流值(1位小数)
	0x0252, 0x0188,			//Gain[1]-- 增益补偿值, 电流值(1位小数)
	0x040E, 0x0286,			//Gain[2]-- 增益补偿值, 电流值(1位小数)
};
#else//60A
const WORD SampleHalfWaveDefData[12] =
{
	0xF5F4, 0x0020,			//Coe[0] -- 相位补偿值, 电流值(1位小数)
	0xF407, 0x00E8,			//Coe[1] -- 相位补偿值, 电流值(1位小数)
	0xF2B5, 0x0164,			//Coe[2] -- 相位补偿值, 电流值(1位小数)
	0x0000, 0x0000,			//Gain[0]-- 增益补偿值, 电流值(1位小数)
	0x0000, 0x0000,			//Gain[1]-- 增益补偿值, 电流值(1位小数)
	0x0000, 0x0000,			//Gain[2]-- 增益补偿值, 电流值(1位小数)
};
#endif
//阻尼震荡波补偿默认系数
const WORD CosGainSwtichData = 0xFF8E;

//电压改变补偿默认系数
const WORD SampleVolChangeData = 0x0001;

//芯片温度补偿曲线默认系数
const WORD SampleTCcoffDefData[3] =
{
	0xFF00,	//0xFF11,	//TCcoffA-0x6D
	0x0DB8,	//0x2B53,	//TCcoffB-0x6E
	0xD1DA,	//0xD483,	//TCcoffC-0x6F
};
//与B相有关的测量类寄存器地址
const BYTE PhaseBRegAddr[9] = 
{
	r_PB,
	r_QB,
	r_SB,
	r_UBRMS,
	r_IBRMS,
	r_PFB,
	r_EPB,
	r_EQB,
	r_ESB
};

//直流偶次谐波门限值
#if( cCURR_TYPES == CURR_60A )
const WORD HD2CheckCurrentValueMin = 10000;			//开启直流偶次谐波检测电流最小值(1A)
const WORD HD2CosLimitValue = 9000;					//直流偶次谐波1.0L补偿时相角最小判定值
const WORD HD2CurrentDiffValue = 25000;				//上一次补偿电流值与当前电流值的差值
const BYTE HD2ContentMin = 25;						//半波中二次谐波含量最小值
const BYTE HD2ContentMax = 55;						//半波中二次谐波含量最大值
const BYTE HD2DiffValue = 8;						//当前二次谐波与上一次二次谐波差值
const BYTE FirstDiffValue = 10;						//当前基波与上一次基波差值
const WORD HD2CurrentRangeMin = 10;					//直流偶次谐波补偿电流最小值1A
const WORD HD2CurrentRangeMax = 500;				//直流偶次谐波补偿电流最大值50A
#else//100A
const WORD HD2CheckCurrentValueMin = 20000;			//开启直流偶次谐波检测电流最小值(2A)
const WORD HD2CosLimitValue = 9000;					//直流偶次谐波1.0L补偿时相角最小判定值
const WORD HD2CurrentDiffValue = 25000;				//上一次补偿电流值与当前电流值的差值
const BYTE HD2ContentMin = 25;						//半波中二次谐波含量最小值
const BYTE HD2ContentMax = 55;						//半波中二次谐波含量最大值
const BYTE HD2DiffValue = 8;						//当前二次谐波与上一次二次谐波差值
const BYTE FirstDiffValue = 10;						//当前基波与上一次基波差值
const WORD HD2CurrentRangeMin = 20;					//直流偶次谐波补偿电流最小值2A
const WORD HD2CurrentRangeMax = 1000;				//直流偶次谐波补偿电流最大值100A
#endif

//-----------------------------------------------
//			LCD 显示默认参数
//-----------------------------------------------

const TLCDPara LCDParaDef = 
{ 
	.PowerOnDispTimer = 5 , 		//上电全显时间  unsigned，
	.BackLightPressKeyTimer = 60 , 	//背光点亮时间  long-unsigned(按键时背光点亮时间)
	.BackLightViewTimer = 10 , 		//显示查看背光点亮时间  long-unsigned，
	.LcdSwitchTime = 30 , 			//无电按键屏幕驻留最大时间  long-unsigned， 轮显时间，HEX 单位为秒
	.EnergyFloat = 2 , 				//电能小数点
	.DemandFloat = 4 , 				//功率（需量）小数点
	.Meaning12 = 0,					//显示12字样意义
#if(PREPAY_MODE == PREPAY_LOCAL)//本地表
	.DispItemNum = {19,97},			//循显，键显
#elif(cRELAY_TYPE == RELAY_NO)	//智能表
	.DispItemNum = {20,86},			//循显，键显
#else							//远程费控表
	.DispItemNum = {18,84},			//循显，键显
#endif
	.KeyDisplayTime = 0x003C,		//键显切换到循环显示的时间 显示类属性3参数
	.LoopDisplayTime = 0x0005,		//循环显示时每屏的显示时间 显示类属性3参数
};

//20规范电表默认不点亮背光，小铃铛报警默认与背光报警一致
#if(cRELAY_TYPE != RELAY_NO )
const TWarnDispPara WarnDispParaConst = 
{  	  
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 0 -- 报警继电器
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 1 -- LCD小铃铛报警控制参数
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 2 -- 液晶背光
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 3 -- 液晶ERR显示报警
};
#else
//08 01 21 00 03 00 00 00
const TWarnDispPara WarnDispParaConst = 
{  	  
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 0 -- 报警继电器
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 1 -- LCD小铃铛报警控制参数
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 2 -- 液晶背光
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 3 -- 液晶ERR显示报警
};
#endif


//循显项目默认设置(35个) 0-置默认显示项 1~199采用LcdItemTable里面的列号，200~249采用ExpandLcdItemTable里面的列号，fe-此项不能初始化， 通过规约设置
const BYTE LoopItemDef[35]=                    
{
	1,  //循显第1项	40000200--当前日期
	2,  //循显第2项	40000200--当前时间
#if(PREPAY_MODE == PREPAY_LOCAL)
	3,  //循显第3项	202c0201--当前剩余金额
#endif
	149,//循显4项 00000401--当前组合有功总高精度电量
	150,//循显第5项 00100401--当前正向有功总高精度电量
	151,//循显第6项 00100402--当前正向有功尖高精度电量
	152,//循显第7项 00100403--当前正向有功峰高精度电量
	153,//循显第8项 00100404--当前正向有功平高精度电量
	154,//循显第9项 00100405--当前正向有功谷高精度电量
	10, //循显第10项	10100201--当前正向有功总最大需量
#if(cRELAY_TYPE == RELAY_NO)	
	160,//循显第11项 00300401--当前组合无功1总高精度电量
	161,//循显第12项 00400401--当前组合无功2总高精度电量
#endif
	162,//循显第13项 00500401--当前第一象限无功总高精度电量
	163,//循显第14项 00600401--当前第二象限无功总高精度电量
	164,//循显第15项 00700401--当前第三象限无功总高精度电量
	165,//循显第16项 00800401--当前第四象限无功总高精度电量
	155,//循显第17项 00200401--当前反向有功总高精度电量
	156,//循显第18项 00200402--当前反向有功尖高精度电量
	157,//循显第19项 00200403--当前反向有功峰高精度电量
	158,//循显第20项 00200404--当前反向有功平高精度电量
	159,//循显第21项 00200405--当前反向有功谷高精度电量
};


//键显项目默认设置(110个) 0-置默认显示项 1~199采用LcdItemTable里面的列号，200~249采用ExpandLcdItemTable里面的列号，fe-此项不能初始化， 通过规约设置
const BYTE KeyItemDef[110] =                
{
	1,//键显第1项	40000200--当前日期
	2,//键显第2项	40000200--当前时间
#if(PREPAY_MODE == PREPAY_LOCAL)
	3,//键显第3项	202c0201--当前剩余金额
#endif
	149,//键显4项 00000401--当前组合有功总高精度电量
	150,//键显第5项 00100401--当前正向有功总高精度电量
	151,//键显第6项 00100402--当前正向有功尖高精度电量
	152,//键显第7项 00100403--当前正向有功峰高精度电量
	153,//键显第8项 00100404--当前正向有功平高精度电量
	154,//键显第9项 00100405--当前正向有功谷高精度电量
	10, //键显第10项	10100201--当前正向有功总最大需量
	11, //键显第11项	10100201--当前正向有功总最大需量发生日期
	12, //键显第12项	10100201--当前正向有功总最大需量发生时间
	155,//键显第13项 00200401--当前反向有功总高精度电量
	156,//键显第14项 00200402--当前反向有功尖高精度电量
	157,//键显第15项 00200403--当前反向有功峰高精度电量
	158,//键显第16项 00200404--当前反向有功平高精度电量
	159,//键显第17项 00200405--当前反向有功谷高精度电量
	18, //键显第18项	10200201--当前反向有功总最大需量
	19, //键显第19项	10200201--当前反向有功总最大需量发生日期
	20, //键显第20项	10200201--当前反向有功总最大需量发生时间
#if(cRELAY_TYPE == RELAY_NO)
	160,//循显第21项 00300401--当前组合无功1总高精度电量
	161,//循显第22项 00400401--当前组合无功2总高精度电量
#endif
	162,//键显第23项 00500401--当前第一象限无功总高精度电量
	163,//键显第24项 00600401--当前第二象限无功总高精度电量
	164,//键显第25项 00700401--当前第三象限无功总高精度电量
	165,//键显第26项 00800401--当前第四象限无功总高精度电量
	167,//键显第27项 50050201 00100401--上1月正向有功总高精度电量
	168,//键显第28项 50050201 00100402--上1月正向有功尖高精度电量
	169,//键显第29项 50050201 00100403--上1月正向有功峰高精度电量
	170,//键显第30项 50050201 00100404--上1月正向有功平高精度电量
	171,//键显第31项 50050201 00100405--上1月正向有功谷高精度电量
	32, //键显第32项	50050201 10100201--上1月正向有功总最大需量
	33, //键显第33项	50050201 10100201--上1月正向有功总最大需量发生日期
	34, //键显第34项	50050201 10100201--上1月正向有功总最大需量发生时间
	172,//键显第35项 50050201 00200401--上1月反向有功总高精度电量
	173,//键显第36项 50050201 00200402--上1月反向有功尖高精度电量
	174,//键显第37项 50050201 00200403--上1月反向有功峰高精度电量
	175,//键显第38项 50050201 00200404--上1月反向有功平高精度电量
	176,//键显第39项 50050201 00200405--上1月反向有功谷高精度电量
	40, //键显第40项	50050201 10200201--上1月反向有功总最大需量
	41, //键显第41项	50050201 10200201--上1月反向有功总最大需量发生日期
	42, //键显第42项	50050201 10200201--上1月反向有功总最大需量发生时间
	179,//键显第43项 50050201 00500401--上1月第一象限无功总高精度电量
	180,//键显第44项 50050201 00600401--上1月第二象限无功总高精度电量
	181,//键显第45项 50050201 00700401--上1月第三象限无功总高精度电量
	182,//键显第46项 50050201 00800401--上1月第四象限无功总高精度电量
	47, //键显第47项	40010200--通信地址低8位
	48, //键显第48项	40010200--通信地址高4位
	49, //键显第49项	F2010201--通信波特率
	50, //键显第50项	41090200--有功脉冲常数
	51, //键显第51项	410A0200--无功脉冲常数
	52, //键显第52项	20130200--时钟电池使用时间
	53, //键显第53项	30120201 33020202--最近一次编程日期
	54, //键显第54项	30120201 33020202--最近一次编程时间
	55, //键显第55项	30000D01--总失压次数、对象增加属性12
	56, //键显第56项	30000D02--总失压累计时间、对象增加属性12
	57, //键显第57项	30000D03--最近一次失压起始日期
	58, //键显第58项	30000D03--最近一次失压起始时间
	59, //键显第59项	30000D04--最近一次失压结束日期
	60, //键显第60项	30000D04--最近一次失压结束时间
	61, //键显第61项	30000701 00102201--最近一次A相失压起始时刻正向有功电量
	62, //键显第62项	30000701 00108201--最近一次A相失压结束时刻正向有功电量
	63, //键显第63项	30000701 00202201--最近一次A相失压起始时刻反向有功电量
	64, //键显第64项	30000701 00208201--最近一次A相失压结束时刻反向有功电量
	65, //键显第65项	30000801 00102201--最近一次B相失压起始时刻正向有功电量
	66, //键显第66项	30000801 00108201--最近一次B相失压结束时刻正向有功电量
	67, //键显第67项	30000801 00202201--最近一次B相失压起始时刻反向有功电量
	68, //键显第68项	30000801 00208201--最近一次B相失压结束时刻反向有功电量
	69, //键显第69项	30000901 00102201--最近一次C相失压起始时刻正向有功电量
	70, //键显第70项	30000901 00108201--最近一次C相失压结束时刻正向有功电量
	71, //键显第71项	30000901 00202201--最近一次C相失压起始时刻反向有功电量
	72, //键显第72项	30000901 00208201--最近一次C相失压结束时刻反向有功电量
	73, //键显第73项	20000201--A相电压
	74, //键显第74项	20000202--B相电压
	75, //键显第75项	20000203--C相电压
	76, //键显第76项	20010201--A相电流
	77, //键显第77项	20010202--B相电流
	78, //键显第78项	20010203--C相电流
	79, //键显第79项	20040201--瞬时总有功功率
	80, //键显第80项	20040202--瞬时A相有功功率
	81, //键显第81项	20040203--瞬时B相有功功率
	82, //键显第82项	20040204--瞬时C相有功功率
	83, //键显第83项	200A0201--瞬时总功率因数
	84, //键显第84项	200A0202--瞬时A相功率因数
	85, //键显第85项	200A0203--瞬时B相功率因数
	86, //键显第86项	200A0204--瞬时C相功率因数		
#if(PREPAY_MODE == PREPAY_LOCAL)
	87, //键显第87项	40180201--当前尖费率电价
	88, //键显第88项	40180202--当前峰费率电价
	89, //键显第89项	40180203--当前平费率电价
	90, //键显第90项	40180204--当前谷费率电价
	91, //键显第91项	401A0202--阶梯1电价
	92, //键显第92项	401A0202--阶梯2电价
	93, //键显第93项	401A0202--阶梯3电价
	94, //键显第94项	401A0202--阶梯4电价
	95, //键显第94项	201A0200--当前电价
	96, //键显第96项	401E0201--报警金额1
	97, //键显第97项	401E0202--报警金额2
	98, //键显第98项	401F0201--透支金额
#endif
	99, //键显第99项	41160201--结算日
};
//扩展项，序号从200开始
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
//			Freeze冻结默认参数
//-----------------------------------------------
// 瞬时冻结
// 默认周期无效，3个点
const TFreezePara InstantFreeze[MAX_INSTANT_FREEZE_ATTRIBUTE+1]=
{
	{12,0x0000000c,	12},		//冻结属性个数
	{0, 0x00100400, 3 }, 		//正向有功电能					20  
	{0, 0x00200400, 3 }, 		//反向有功电能					20
	{0, 0x00300400, 3 }, 		//组合无功1电能 				20
	{0, 0x00400400, 3 }, 		//组合无功2电能 			  	20	  
	{0, 0x00500400, 3 }, 		//第一象限无功电能				20 	 
	{0, 0x00600400, 3 }, 		//第二象限无功电能				20 	 
	{0, 0x00700400, 3 }, 		//第三象限无功电能				20 	 
	{0, 0x00800400, 3 }, 		//第四象限无功电能				20 	 
	{0, 0x10100200, 3 }, 		//正向有功最大需量及发生时间	55 			   
	{0, 0x10200200, 3 }, 		//反向有功最大需量及发生时间	55 			   
	{0, 0x20040200, 3 }, 		//有功功率					   	16
	{0, 0x20050200, 3 }, 		//无功功率					   	16
};

// 分钟冻结
// 28800个点   周期和深度均不能设置为0
const TFreezePara MinFreeze[MAX_MINUTE_FREEZE_ATTRIBUTE+1]=
{
	#if(cMETER_TYPES == METER_ZT)
	{ 17,0x00000011,	17},				    //冻结属性个数
	#else
	{ 15,0x0000000F,	15},					//冻结属性个数
	#endif
	
	{ 15, 0x00100401, MAX_FREEZE_MIN_DEPTH }, // 正向有功总电能
	{ 15, 0x00200401, MAX_FREEZE_MIN_DEPTH }, // 反向有功总电能
	{ 15, 0x00300401, MAX_FREEZE_MIN_DEPTH }, // 组合无功1总电能
	{ 15, 0x00400401, MAX_FREEZE_MIN_DEPTH }, // 组合无功2总电能
	{ 15, 0x00500401, MAX_FREEZE_MIN_DEPTH }, // 第一象限无功总电能
	{ 15, 0x00600401, MAX_FREEZE_MIN_DEPTH }, // 第二象限无功总电能
	{ 15, 0x00700401, MAX_FREEZE_MIN_DEPTH }, // 第三象限无功总电能
	{ 15, 0x00800401, MAX_FREEZE_MIN_DEPTH }, // 第四象限无功总电能
	{ 15, 0x20170200, MAX_FREEZE_MIN_DEPTH }, // 有功需量
	{ 15, 0x20180200, MAX_FREEZE_MIN_DEPTH }, // 无功需量
	{ 15, 0x20000200, MAX_FREEZE_MIN_DEPTH }, // 电压
	{ 15, 0x20010200, MAX_FREEZE_MIN_DEPTH }, // 电流
	#if(cMETER_TYPES == METER_ZT)
	{ 15, 0x20010400, MAX_FREEZE_MIN_DEPTH }, // 零线电流
	{ 15, 0x20010600, MAX_FREEZE_MIN_DEPTH }, // 零序电流
	#endif
	{ 15, 0x20040200, MAX_FREEZE_MIN_DEPTH }, // 有功功率
	{ 15, 0x20050200, MAX_FREEZE_MIN_DEPTH }, // 无功功率
	{ 15, 0x200A0200, MAX_FREEZE_MIN_DEPTH }, // 功率因数
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
	{23, 0x00000017,	23},					//冻结属性个数
	#else
	{21, 0x00000015,	21},					//冻结属性个数
	#endif
	{1,  0x00100400, 365 },// 正向有功电能
	{1,  0x00110400, 365 },// A相正向有功电能				
	{1,  0x00120400, 365 },// B相正向有功电能				
	{1,  0x00130400, 365 },// C相正向有功电能				
	{1,  0x00200400, 365 },// 反向有功电能
	{1,  0x00210400, 365 },// A相反向有功电能				
	{1,  0x00220400, 365 },// B相反向有功电能				
	{1,  0x00230400, 365 },// C相反向有功电能		
	{1,  0x00300400, 365 },// 组合无功1电能
	{1,  0x00400400, 365 },// 组合无功1电能
	{1,  0x00500400, 365 },// 第一象限无功电能
	{1,  0x00600400, 365 },// 第二象限无功电能
	{1,  0x00700400, 365 },// 第三象限无功电能
	{1,  0x00800400, 365 },// 第四象限无功电能
	{1,  0x10100200, 365 },// 正向有功最大需量及发生时间
	{1,  0x10200200, 365 },// 反向有功最大需量及发生时间
	{1,  0x20040200, 365 },// 有功功率
	{1,  0x20050200, 365 },// 无功功率
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{1,	 0x202c0200, 365 },// 剩余金额、购电次数
	{1,	 0x202d0200, 365 },// 透支金额
	#endif
	{1,  0x21310201, 365 },// A相电压合格率
	{1,  0x21320201, 365 },// B相电压合格率
	{1,  0x21330201, 365 } // C相电压合格率
};

// 结算日冻结
// 默认周期无效，12个点
const TFreezePara ClosingFreeze[MAX_CLOSING_FREEZE_ATTRIBUTE+1]=
{
	{15, 0x0000000f,15},//冻结属性个数
	{1, 0x00000400, 12 },//组合有功电能
	{1, 0x00100400, 12 },//正向有功电能
	{1, 0x00200400, 12 },//反向有功电能
	{1, 0x00300400, 12 },//组合无功1电能 			  	
	{1, 0x00400400, 12 },//组合无功2电能 			  	
	{1, 0x00500400, 12 },//第一象限无功电能			   	  
	{1, 0x00600400, 12 },//第二象限无功电能				
	{1, 0x00700400, 12 },//第三象限无功电能				
	{1, 0x00800400, 12 },//第四象限无功电能				
	{1, 0x00110400, 12 },//A相正向有功电能
	{1, 0x00120400, 12 },//B相正向有功电能
	{1, 0x00130400, 12 },//C相正向有功电能
	{1, 0x10100200, 12 },//正向有功最大需量及发生时间	55 	   
	{1, 0x10200200, 12 },//反向有功最大需量及发生时间	55
	{1, 0x20310200, 12 },//月度用电量					4	 
};

// 月冻结
// 默认周期1月，12个点
const TFreezePara MonFreeze[MAX_MON_FREEZE_ATTRIBUTE+1]=
{
	{20, 0x00000014,20},	//冻结属性个数
	{1, 0x00000400, 24 },	//组合有功电能					20
	{1, 0x00100400, 24 },	//正向有功电能				   	20
	{1, 0x00110400, 24 },	//A相正向有功电能				20
	{1, 0x00120400, 24 },	//B相正向有功电能				20
	{1, 0x00130400, 24 },	//C相正向有功电能				20
	{1, 0x00200400, 24 },	//反向有功电能				 	20
	{1, 0x00210400, 24 },	//A相反向有功电能			   	20 
	{1, 0x00220400, 24 },	//B相反向有功电能				20
	{1, 0x00230400, 24 },	//C相反向有功电能				20
	{1, 0x00300400, 24 },	//组合无功1电能 			  	20
	{1, 0x00400400, 24 },	//组合无功2电能 			  	20
	{1, 0x00500400, 24 },	//第一象限无功电能			   	20  
	{1, 0x00600400, 24 },	//第二象限无功电能				20
	{1, 0x00700400, 24 },	//第三象限无功电能				20
	{1, 0x00800400, 24 },	//第四象限无功电能				20
	{1, 0x10100200, 24 },	//正向有功最大需量及发生时间	55 	   
	{1, 0x10200200, 24 },	//反向有功最大需量及发生时间	55 
	{1, 0x21310202, 24 },	//--A相电压合格率			   	16
	{1, 0x21320202, 24 },	//--B相电压合格率			   	16
	{1, 0x21330202, 24 } 	//--C相电压合格率			   	16
};

// 时区表切换冻结
// 默认周期无效，2个点
const TFreezePara TZChgFreeze[MAX_TIME_ZONE_FREEZE_ATTRIBUTE+1]=
{
	{12, 0x0000000c, 12},		//冻结属性个数 
	{1,  0x00100400,  2 }, 	   //正向有功电能				20
	{1,  0x00200400,  2 }, 	   //反向有功电能				20
	{1,  0x00300400,  2 }, 	   //组合无功1电能				20
	{1,  0x00400400,  2 }, 	   //组合无功2电能				20
	{1,  0x00500400,  2 }, 	   //第一象限无功电能			20
	{1,  0x00600400,  2 }, 	   //第二象限无功电能			20
	{1,  0x00700400,  2 }, 	   //第三象限无功电能			20
	{1,  0x00800400,  2 }, 	   //第四象限无功电能			20
	{1,  0x10100200,  2 }, 	   //正向有功最大需量及发生时间	55
	{1,  0x10200200,  2 }, 	   //反向有功最大需量及发生时间	55
	{1,  0x20040200,  2 }, 	   //有功功率					16
	{1,  0x20050200,  2 }, 	   //无功功率					16
};

// 日时段表切换冻结
// 默认周期无效，2个点
const TFreezePara DTTChgFreeze[MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE+1]=
{
	{12, 0x0000000c, 12},		//冻结属性个数 
	{1,  0x00100400,  2 },		//正向有功电能					20
	{1,  0x00200400,  2 },		//反向有功电能					20
	{1,  0x00300400,  2 },		//组合无功1电能					20
	{1,  0x00400400,  2 },		//组合无功2电能					20
	{1,  0x00500400,  2 },		//第一象限无功电能				20
	{1,  0x00600400,  2 },		//第二象限无功电能				20
	{1,  0x00700400,  2 },		//第三象限无功电能				20
	{1,  0x00800400,  2 },		//第四象限无功电能				20
	{1,  0x10100200,  2 },		//正向有功最大需量及发生时间	55
	{1,  0x10200200,  2 },		//反向有功最大需量及发生时间	55
	{1,  0x20040200,  2 },		//有功功率						16
	{1,  0x20050200,  2 },		//无功功率						16
};

#if(PREPAY_MODE == PREPAY_LOCAL)
// 费率电价切换冻结
// 默认周期无效，2个点
const TFreezePara TariffRateChgFreeze[MAX_TARIFF_RATE_FREEZE_ATTRIBUTE+1]=
{
	{12, 0x0000000c, 12},		//冻结属性个数 
	{1,  0x00100400,  2 },		//正向有功电能					20
	{1,  0x00200400,  2 },		//反向有功电能					20
	{1,  0x00300400,  2 },		//组合无功1电能					20
	{1,  0x00400400,  2 },		//组合无功2电能					20
	{1,  0x00500400,  2 },		//第一象限无功电能				20
	{1,  0x00600400,  2 },		//第二象限无功电能				20
	{1,  0x00700400,  2 },		//第三象限无功电能				20
	{1,  0x00800400,  2 },		//第四象限无功电能				20
	{1,  0x10100200,  2 },		//正向有功最大需量及发生时间	55
	{1,  0x10200200,  2 },		//反向有功最大需量及发生时间	55
	{1,  0x20040200,  2 },		//有功功率						16
	{1,  0x20050200,  2 },		//无功功率						16
};


// 阶梯切换冻结
// 默认周期无效，2个点
const TFreezePara LadderChgFreeze[MAX_LADDER_FREEZE_ATTRIBUTE+1]=
{
	{12, 0x0000000c, 12},		//冻结属性个数 
	{1,  0x00100400,  2 },		//正向有功电能					20
	{1,  0x00200400,  2 },		//反向有功电能					20
	{1,  0x00300400,  2 },		//组合无功1电能					20
	{1,  0x00400400,  2 },		//组合无功2电能					20
	{1,  0x00500400,  2 },		//第一象限无功电能				20
	{1,  0x00600400,  2 },		//第二象限无功电能				20
	{1,  0x00700400,  2 },		//第三象限无功电能				20
	{1,  0x00800400,  2 },		//第四象限无功电能				20
	{1,  0x10100200,  2 },		//正向有功最大需量及发生时间	55
	{1,  0x10200200,  2 },		//反向有功最大需量及发生时间	55
	{1,  0x20040200,  2 },		//有功功率						16
	{1,  0x20050200,  2 },		//无功功率						16
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

#if( cMETER_TYPES == METER_ZT )		
	const BYTE RateVoltageConst[6] = {"220V"};		//额定电压 ascii码     4
	#if( cCURR_TYPES == CURR_60A )
		const BYTE RateCurrentConst[6] = {"5A"};	//额定电流 ascii码     5
		const BYTE MaxCurrentConst[6] = {"60A"};	//最大电流 ascii码     6
		const BYTE MinCurrentConst[10] = {4,'0','.','2','A'};	//最小电流 ascii码     7
		const BYTE TurnCurrentConst[10] = {4,'0','.','5','A'};	//转折电流 ascii码     8
		const DWORD ActiveConstantConst = 1000;		//有功脉冲常数    	11
	#elif( cCURR_TYPES == CURR_100A )
		const BYTE RateCurrentConst[6] = {"10A"};	//额定电流 ascii码     5
		const BYTE MaxCurrentConst[6] = {"100A"};	//最大电流 ascii码     6
		const BYTE MinCurrentConst[10] = {4,'0','.','4','A'};	//最小电流 ascii码     7
		const BYTE TurnCurrentConst[10] = {2,'1','A'};	//转折电流 ascii码     8
		const DWORD ActiveConstantConst = 500;		//有功脉冲常数         11
	#else
		"配置错误"！
	#endif		
#elif( cMETER_TYPES == METER_3P3W )	
	const BYTE RateVoltageConst[6] = {"100V"};		//额定电压 ascii码     4	
	#if( cCURR_TYPES == CURR_6A )
		const BYTE RateCurrentConst[6] = {"1.5A"};	//额定电流 ascii码     5
		const BYTE MaxCurrentConst[6] = {"6A"};		//最大电流 ascii码     6
		const BYTE MinCurrentConst[10] = {6,'0','.','0','1','5','A'};//最小电流 ascii码  7
		const BYTE TurnCurrentConst[10] = {6,'0','.','0','7','5','A'};//转折电流 ascii码 8
		const DWORD ActiveConstantConst = 20000L;	//有功脉冲常数    	11
	#elif( cCURR_TYPES == CURR_1A )
		const BYTE RateCurrentConst[6] = {"0.3A"};	//额定电流 ascii码     5
		const BYTE MaxCurrentConst[6] = {"1.2A"};	//最大电流 ascii码     6
		const BYTE MinCurrentConst[10] = {6,'0','.','0','0','3','A'};//最小电流 ascii码 7
		const BYTE TurnCurrentConst[10] = {6,'0','.','0','1','5','A'};//转折电流 ascii码8
		const DWORD ActiveConstantConst = 100000L;	//有功脉冲常数     	11
	#else
		"配置错误"！
	#endif		
#else//( cMETER_TYPES == METER_3P4W )		
	#if(cMETER_VOLTAGE==METER_220V)
	const BYTE RateVoltageConst[6] = { "220V" };     //额定电压 ascii码     4
	#else
	const BYTE RateVoltageConst[6] = { "57.7V" };     //额定电压 ascii码     4
	#endif
			
	#if( cCURR_TYPES == CURR_6A )
		const BYTE RateCurrentConst[6] = {"1.5A"};	//额定电流 ascii码     5
		const BYTE MaxCurrentConst[6] = {"6A"};		//最大电流 ascii码     6
		const BYTE MinCurrentConst[10] = {6,'0','.','0','1','5','A'};//最小电流 ascii码  7
		const BYTE TurnCurrentConst[10] = {6,'0','.','0','7','5','A'};//转折电流 ascii码 8
		//57v对应20000,220v对应ActiveConstantConst220V 6400
		#if(cMETER_VOLTAGE==METER_220V)
		const DWORD ActiveConstantConst = 10000L;    //有功脉冲常数    	11
		#else
		const DWORD ActiveConstantConst = 20000L;	//有功脉冲常数         11
		#endif
	#elif( cCURR_TYPES == CURR_1A )
		const BYTE RateCurrentConst[6] = {"0.3A"};	//额定电流 ascii码     5
		const BYTE MaxCurrentConst[6] = {"1.2A"};	//最大电流 ascii码     6
		const BYTE MinCurrentConst[10] = {6,'0','.','0','0','3','A'};//最小电流 ascii码  7
		const BYTE TurnCurrentConst[10] = {6,'0','.','0','1','5','A'};//转折电流 ascii码 8
		//57v对应100000,220v对应30000
		#if(cMETER_VOLTAGE==METER_220V)
		const DWORD ActiveConstantConst = 40000L;    //有功脉冲常数         11
		#else
		const DWORD ActiveConstantConst = 100000L;   //有功脉冲常数         11
		#endif
	#else
		"配置错误"！
	#endif		
#endif

const TEnereyDemandMode EnereyDemandModeConst =
{
	//有功组合方式特征字 组合有功
	.byActiveCalMode = 0x05,
	
	#if(SEL_RACTIVE_ENERGY == YES)
	//正向无功模式控制字
	.PReactiveMode = 0x41,
	//反向无功模式控制字
	.NReactiveMode = 0x14,
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
#if (( BOARD_TYPE != BOARD_HT_THREE_0130347 )&&( BOARD_TYPE != BOARD_HT_THREE_0132515 ))
// 46: 波特率9600; 4A: 115200
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
	.I485 = 0x4A,       	//第一路485
	.ComModule = 0x4A,  	//模块
	.ComModuleDeflt = 0x4A,	//模块恢复缺省值
	.II485 = 0x4A,      	//第二路485（单相不用）
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
const DWORD	Remote645AuthTimeConst = 30;    //远程认证时间
const BYTE  Report645ResetTimeConst = 30;   //645上报延时
//电表运行特征字1是否启用跟随上报 NO：不启用 YES:启用
//电表运行特征字1的其他参数位于lcd显示参数和继电器控制，需配置698使用
const BOOL  Meter645FollowStatusConst = FALSE; 
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
const BYTE QPrecisionConst[4] = {"2"};
#if( cMETER_TYPES != METER_3P3W )
	#if(cRELAY_TYPE == RELAY_NO )
	//电表型号 ascii码     11
	const BYTE MeterModelConst[32] = {"DTZXXX"};
	#else
		#if( PREPAY_MODE == PREPAY_LOCAL )
		//电表型号 ascii码     11
		const BYTE MeterModelConst[32] = {"DTZYXXX"};
		#else
		//电表型号 ascii码     11
		const BYTE MeterModelConst[32] = {"DTZYXXX"};
		#endif
	#endif
#else
	#if(cRELAY_TYPE == RELAY_NO )
	//电表型号 ascii码     11
	const BYTE MeterModelConst[32] = {"DSZXXX"};
	#else
	//电表型号 ascii码     11
	const BYTE MeterModelConst[32] = {"DSZYXXX"};
	#endif
#endif
//698.45协议版本号(数据类型:WORD)    13
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
//软件备案号 ASCII码      19 0062201904170010
const BYTE SoftRecordConst[16] = {'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A'};
//默认当前费率时段
const BYTE DefCurrRatioConst = 3; //平，不能为0

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
#elif(cOSCILATOR_TYPE == OSCILATOR_JG)
//HT602xK+精工：
const unsigned short TAB_DFx_K[10] = 		//HT6025H默认参数
{
	0x0000, 0x0000,
	0x007F, 0xDa4b,
	0x007E, 0xD9ac,
	0x0000, 0x4a2e,
	0x007F, 0xfc90
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
#if(BOARD_TYPE == BOARD_HT_THREE_0134566)	//背光控制老方式+6A小电流电阻为5欧 硬件地址
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_HARD, CHIP_E2_ST_HARD};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_THREE_0131699)	//背光控制新方式+6A小电流电阻为5欧 软件地址
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_HARD, CHIP_E2_ST_HARD};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_THREE_0130347)	//背光控制新方式+小电流电阻加倍+支持115200波特率+新E2
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_SOFT, CHIP_E2_ST_SOFT};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_THREE_0132515)	//背光控制新方式+小电流电阻加倍+支持115200波特率+新E2
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
const WORD	OverIProtectTime_Def = 1440;	//超电流门限保护延时时间 HEX 单位：分钟
const DWORD	RelayProtectI_Def = 300000;		//继电器拉闸电流门限值 HEX 单位：A，换算-4	

#if( cRELAY_TYPE == RELAY_INSIDE )
	const BYTE	RelayCtrlMode_Def = 0;			//继电器输出开关属性 0--脉冲 1--保持
#else
	const BYTE	RelayCtrlMode_Def = 1;			//继电器输出开关属性 0--脉冲 1--保持
#endif

//-----------------------------------------------
//			Event 事件默认参数
//-----------------------------------------------
//失压 0x3000
#if( SEL_EVENT_LOST_V == YES )
const BYTE	EventLostVTimeConst = 60;	//事件延时时间 60s
const BYTE  EventLostVURateConst = 78;	//78%参比电压,失压事件电压触发上限定值范围： 70%～90%参比电压，最小设定值级差 0.1V
const BYTE  EventLostVIRateConst = 5;	//0.5%额定（基本）电流，失压事件电流触发下限定值范围： 0.5%～5%额定（基本）电流，最小设定值级差 0.1mA
const BYTE  EventLostVRecoverLimitVRateConst = 85;//85%参比电压,失压事件电压恢复下限定值范围： 失压事件电压触发上限～90%参比电压，最小设定值级差 0.1V
#endif

//欠压 0x3001
#if( SEL_EVENT_WEAK_V == YES )
const BYTE	EventWeakVTimeConst = 60;   //事件延时时间 60s
const BYTE  EventWeakVURateConst = 78;  //78%参比电压,电压触发上限定值范围： 70%～90%参比电压，最小设定值级差 0.1V
#endif

//过压 0x3002
#if( SEL_EVENT_OVER_V == YES )
const BYTE	EventOverVTimeConst = 60;   //事件延时时间 60s
const BYTE  EventOverVURateConst = 120;	//120%参比电压,过压事件电压触发下限定值范围： 110%～130%参比电压，最小设定值级差 0.1V
#endif

//断相 0x3003
#if( SEL_EVENT_BREAK == YES )
const BYTE	EventBreakTimeConst = 60;   //事件延时时间 60s
const BYTE  EventBreakURateConst = 60;	//电压触发上限60%Un
const BYTE  EventBreakIRateConst = 5;	//电流触发上限0.5%Ib
#endif

//失流 0x3004
#if( SEL_EVENT_LOST_I == YES )
const BYTE	EventLostITimeConst = 60;   //事件延时时间 60s
const BYTE  EventLostIURateConst = 70;	//70%,失流事件电压触发下限定值范围： 60%～90%参比电压，最小设定值级差 0.1V
const BYTE  EventLostIIRateConst = 5;	//0.5%额定（基本）电流,失流事件电流触发上限定值范围： 0.5%～2%额定（基本）电流，最小设定值级差 0.1mA
const BYTE  EventLostILimitLIConst = 5;	//5%额定（基本）电流,失流事件电流恢复下限定值范围： 3%～10%额定（基本）电流，最小设定值级差 0.1mA
#endif

//过流 0x3005 IMax本身保留了3位小数，过流门限值要求4位小数
#if( SEL_EVENT_OVER_I == YES )
const BYTE	EventOverITimeConst = 60;   //事件延时时间 60s
const BYTE  EventOverIIRateConst = 12; //1.2Imax,过流事件电流触发下限定值范围： 0.5～1.5Imax，最小设定值级差 0.1mA
#endif

//断流 0x3006
#if( SEL_EVENT_BREAK_I == YES )
const BYTE	EventBreakITimeConst = 60;  //事件延时时间 60s
const BYTE  EventBreakIURateConst = 60;	//60%参比电压，断流事件电压触发下限定值范围： 60%～85%参比电压，最小设定值级差 0.1V
const BYTE  EventBreakIIRateConst = 5;	//0.5%额定（基本）电流,断流事件电流触发上限定值范围： 0.5%～5%额定（基本）电流，最小设定值级差 0.1mA
#endif

//功率反向 0x3007
#if( SEL_EVENT_BACKPROP == YES )
const BYTE	EventBackpTimeConst = 60;   //事件延时时间 60s
	#if(( cMETER_VOLTAGE != METER_57V ) || ( cCURR_TYPES != CURR_1A ) )
	const BYTE  EventBackpPRateConst = 5;	//0.5%单相基本功率,有功功率反向事件有功功率触发下限定值范围：0.5%～5%单相基本功率，最小设定值级差 0.0001kW
	#else
	const BYTE  EventBackpPRateConst = 6;	//0.5%单相基本功率,57.7V0.3(1.2)A电能表触发门限0（0.00008KW），此事件就不会触发，配置为6进位为1(0.0001kW)
	#endif
#endif

//过载 0x3008
#if( SEL_EVENT_OVERLOAD == YES )
const BYTE	EventOverLoadTimeConst = 60;    //事件延时时间 60s
const BYTE  EventOverLoadPRateConst = 12;	//1.2Imax*100%Un,过载事件有功功率触发下限定值范围： 0.5～1.5Imax 单相基本功率，最小设定值级差 0.0001kW
#endif

//0x3009	电能表正向有功需量超限事件
//0x300a	电能表反向有功需量超限事件
//0x300b	电能表无功需量超限事件
#if( SEL_DEMAND_OVER == YES )
const BYTE	EventDemandOverTimeConst = 60;  //事件延时时间 60s
const BYTE  EventDemandOverRateConst = 12;	//1.2Imax*100%Un,有功需量超限事件需量触发下限定值范围 0.05～99.99kW，最小设定值级差 0.0001kW
#endif

//功率因数超限 0x303B
#if( SEL_EVENT_COS_OVER == YES )
const BYTE	EventCosOverTimeConst = 60; //事件延时时间 60s
const WORD  EventCosOverRateConst = 300;//0.3,功率因数超下限阀值定值范围： 0.2～0.6， 最小设定值级差 0.001
#endif

//全失压 0x300d
#if( SEL_EVENT_LOST_ALL_V == YES )
const BYTE	EventLostAllVTimeConst = 60;    //事件延时时间 60s
#endif

//辅助电源掉电 0x300e
#if( SEL_EVENT_LOST_SECPOWER == YES )
const BYTE	EventLostSecPowerTimeConst = 60;    //事件延时时间 60s
#endif

//电压逆相序 0x300f
#if( SEL_EVENT_V_REVERSAL == YES )
const BYTE	EventVReversalTimeConst = 60;   //事件延时时间 60s
#endif

//电流逆相序 0x3010
#if( SEL_EVENT_I_REVERSAL == YES )
const BYTE	EventIReversalTimeConst = 60;   //事件延时时间 60s
#endif

//掉电 0x3011
#if( SEL_EVENT_LOST_POWER == YES )
const BYTE	EventLostPowerTimeConst = 0;   //事件延时时间 60s
#endif

//电压不平衡 0x301d
#if( SEL_EVENT_V_UNBALANCE == YES )
const BYTE	EventVUnbalanceTimeConst = 60;  //事件延时时间 60s
const WORD  EventVUnbalanceRateConst = 3000;//30%,电压不平衡率限值定值范围： 10%～99%，最小设定值级差 0.01%
#endif

//电流不平衡 0x301e
#if( SEL_EVENT_I_UNBALANCE == YES )
const BYTE	EventIUnbalanceTimeConst = 60;  //事件延时时间 60s
const WORD  EventIUnbalanceRateConst = 3000; //30%,电流不平衡率限值定值范围： 10%～90%，最小设定值级差 0.01%
#endif


//电流严重不平衡 0x302d
#if( SEL_EVENT_I_S_UNBALANCE == YES )
const BYTE	EventISUnbalanceTimeConst = 60; //事件延时时间 60s
const WORD  EventISUnbalanceRateConst = 9000; //90%,电流严重不平衡率限值定值范围： 20%～99%，最小设定值级差 0.01%
#endif
//计量芯片故障 0x302F
#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
const BYTE	EventSampleChipErrTimeConst = 60;   //事件延时时间 60s
#endif

//电压合格率0x4030
#if( SEL_STAT_V_RUN == YES )
const BYTE  EventStatVRateConst = 12;			//电压考核上限	1.2Un
const BYTE  EventStatVRunLimitLVConst = 70;		//电压考核下限	0.7Un
const BYTE  EventStatVRunRecoverHVConst = 107;	//电压合格上限	1.07Un
const BYTE  EventStatVRunRecoverLVConst = 93;	//电压合格下限	0.93Un
#endif
//电能表零线电流异常 0x3040
#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
const BYTE	 EventNeutralCurrentErrTimeConst = 60; //事件延时时间 60s
const DWORD  EventNeutralCurrentErrLimitIConst = 20000; //20%;电流触发下限：最小设定值级差 0.001%
const WORD   EventNeutralCurrentErrRatioConst = 5000; //50%零线电流异常下限范围： 10%～98%，最小设定值级差 0.01%
#endif

//-----------------------------------------------
//			Report 事件默认参数
//-----------------------------------------------
//			跟随上报		主动上报		上报通道
//模块表		禁止			允许			模块通信口
//非模块表		允许			禁止			所有RS485
#if(cRELAY_TYPE == RELAY_NO)
	//默认不允许跟随上报 4300	电气设备 的属性7. 允许跟随上报
	const BYTE	MeterReportFlagConst = TRUE;
	//默认主动上报标识 4300	电气设备 的属性8. 允许主动上报
	const BYTE	MeterActiveReportFlagConst = FALSE;

	//默认跟随上报状态字 上报方式 0：主动上报  1：跟随上报
	const BYTE	MeterReportStatusFlagConst = 1;
	//跟随上报通道;
	const BYTE	FollowReportChannelConst[MAX_COM_CHANNEL_NUM+1] = 
	{
		2,			//第一个字节表示长度
		eRS485_I,//第一路485
		eRS485_II,//第二路485
	};
	//eRS485_I,//第一路485
	//eIR,//红外通道
	//eCR,//模块通道
	//eRS485_II,//第二路485
#else
	//默认不允许跟随上报 4300	电气设备 的属性7. 允许跟随上报
	const BYTE	MeterReportFlagConst = FALSE;
	//默认主动上报标识 4300	电气设备 的属性8. 允许主动上报
	const BYTE	MeterActiveReportFlagConst = TRUE;

	//默认跟随上报状态字 上报方式 0：主动上报  1：跟随上报
	const BYTE	MeterReportStatusFlagConst = 1;
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

#endif


// 通道OAD与eSERIAL_TYPE的顺序一致
const DWORD ChannelOAD[] =
{
	0x010201F2,     //eRS485_I
	0x000202F2,     //eIR
	0x000209F2,     //eCR
	0x020201F2,     //eRS485_II
};
//默认事件上报方式为主动上报的列表
const BYTE	ReportActiveMethodConst[15+1] = 
{
	5,
	eEVENT_LOST_POWER_NO,		//掉电事件
	eEVENT_METERCOVER_NO,		//开表盖事件
	eEVENT_OVER_I_NO,			//过流事件
	eEVENT_MAGNETIC_INT_NO,		//恒定磁场干扰事件
	eEVENT_RTC_ERR_NO,			//时钟故障事件
};

//跟随上报模式字
const BYTE	FollowReportModeConst[4] = 
{   
    0x00,//bit24~bit31
    0x00,//bit16~bit23
    0x00,//bit8~bit15       bit8停电抄表电池欠压、bit9透支状态、bit14跳闸成功、bit15合闸成功
    0x00,//bit0~bit7        bit1ESAM错误、bit3时钟电池电压低、bit5存储器故障
};


//此处顺序要和 eEVENT_INDEX 定义的顺序一致，且数组大小和eNUM_OF_EVENT_PRG数据一致
//不上报（0），事件发生上报（BIT0），事件恢复上报（BIT1），事件发生恢复均上报（BIT0|BIT1）
const TReportModeConst	ReportModeConst[15+1] =
#if((cMETER_VOLTAGE == METER_220V) &&(cRELAY_TYPE != RELAY_NO))
{
	//事件枚举号 上报模式
	5, 5,
	eEVENT_LOST_POWER_NO,	(BIT0|BIT1),		//掉电事件
	eEVENT_METERCOVER_NO,	BIT0,		//开表盖事件
	eEVENT_OVER_I_NO,		BIT0,		//过流事件
	eEVENT_MAGNETIC_INT_NO,	BIT0,		//恒定磁场干扰事件
	eEVENT_RTC_ERR_NO,		BIT0,		//时钟故障事件
};
#else
{
	//事件枚举号 上报模式
	5, 5,
	eEVENT_LOST_POWER_NO,	BIT1,		//掉电事件
	eEVENT_METERCOVER_NO,	BIT0,		//开表盖事件
	eEVENT_OVER_I_NO,		BIT0,		//过流事件
	eEVENT_MAGNETIC_INT_NO,	BIT0,		//恒定磁场干扰事件
	eEVENT_RTC_ERR_NO,		BIT0,		//时钟故障事件
};
#endif

//备用，不要修改
const Fun FunctionConst @"PRG_CONST_B" = api_DelayNop;
const BYTE a_byDefaultData[] @"PRG_CONST_C"= 
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


#endif//#if( MAX_PHASE == 3 )

