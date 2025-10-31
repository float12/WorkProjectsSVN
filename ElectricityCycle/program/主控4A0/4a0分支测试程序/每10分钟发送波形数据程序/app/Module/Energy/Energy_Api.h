//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.8.5
//描述		电能模块Api头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __ENERGY_API_H
#define __ENERGY_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

typedef enum
{
	SwitchNormalEnergy = 0,	//脉冲数转化为普通电能
	SwitchHighEnergy,	 	//脉冲数转化为高精度电能
}eSwitchEnergyType;


//电能结构
typedef struct TEnergyDBase_t
{
	//正向有功
	DWORD PActive;

	//反向有功
	DWORD NActive;
	
	#if(SEL_RACTIVE_ENERGY == YES)
	//四象限无功
	DWORD RActive[4];
	#endif

	#if( SEL_APPARENT_ENERGY == YES )
	// 视在（正向、反向）
	DWORD Apparent[2];
	#endif

	//校验
	DWORD CRC32;
}TEnergyDBase;

typedef struct TEnergyRam_t
{
	TEnergyDBase Energy[1+NUM_OF_SEPARATE_ENERGY];							// 总,A,B,C
}TEnergyRam;

// EEPROM中电能
// 当前存放在安全空间
// 上1~12存放在连续空间
typedef struct TEnergyRom_t
{
	TEnergyDBase Energy[1+NUM_OF_SEPARATE_ENERGY];							// 总,A,B,C

	TEnergyDBase RatioEnergy[MAX_RATIO][1+NUM_OF_SEPARATE_ENERGY_RATIO];	// 总,A,B,C的费率电能  MAX_RATIO为5
}TEnergyRom;

// 电能余数
typedef struct TEnergyRomRemain_t
{
	TEnergyDBase RemainEnergy[1+NUM_OF_SEPARATE_ENERGY];						// 总,A,B,C
}TEnergyRomRemain;

//累计电能存储结构
typedef struct TDEnergyAccuBase_t
{
	DWORD PActive;	//正向有功

	DWORD NActive;	//反向有功
	
	DWORD CRC32;	//校验
}TDEnergyAccuBase;

// 安全空间电能结构定义
typedef struct TEnergySafe_t
{
	TEnergyRom 			Energy;	// 电能，保存脉冲个数

	TEnergyRomRemain	Remain;	// 尾数，保存脉冲个数

	TDEnergyAccuBase	EnergyAccMon[2];	// 月，保存0.01度电能
}TEnergySafeRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------


//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 清电能
//
//参数: 
//	无
//                    
//返回值:
//  无
//
//备注:   
//-----------------------------------------------
void api_ClrEnergy( void );

//-----------------------------------------------
//函数功能: 写入电能
//
//参数:
//	Type[in]:
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	Value[in]:	脉冲个数
//
//返回值:	无
//
//备注:   计量调用
//-----------------------------------------------
void api_WritePulseEnergy( WORD Type, BYTE Value );

//-----------------------------------------------
//函数功能: 获取当前电能数据
//
//参数:
//	Type[in]:
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//	Ratio[in]:
//			费率
//			1~MAX_RATIO		 各费率
//			0				 各费率总计
//
//	Dot[in]: 小数点位数
//			0: --	无小数点
//			1~N: --	1~N个小数点
//
//	Energy[out]:指向电能的指针
//			Hex补码表示，最高位为符号位
//			高精度电能返回 8字节，非高精度电能返回4字节
//
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_GetCurrEnergyData(WORD Type, BYTE DataType, WORD Ratio, BYTE Dot, BYTE * Energy);

//-----------------------------------------------
//函数功能: 获取累计用电量（月结算）
//
//参数:  
//	No[in]:
//			0 当前月
//			1 上月
//
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//
//	Dot[in]: 小数点位数
//			0: --	无小数点
//			1~N: --	1~N个小数点
//
//	pEnergy[in]:
//			指向电能的指针
//			Hex补码表示，最高位为符号位 %0x80000000
//
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:	
//-----------------------------------------------
BOOL api_GetAccuEnergyData( BYTE No, BYTE DataType, BYTE Dot, BYTE *pEnergy );
#if(PREPAY_MODE == PREPAY_LOCAL)
//-----------------------------------------------
//函数功能: 阶梯结算电能，用于保存阶梯结算时电能底码，用于计算阶梯用电量
//
//参数:  	无
//	
//返回值:	无
//
//备注:保存脉冲个数	
//-----------------------------------------------
void api_LadderClosingEnergy( void );
//-----------------------------------------------
//函数功能: 获取阶梯用用电量（阶梯扣费用，阶梯结算时清零）
//
//参数:  
//	No[in]:
//			0 当前月
//			1 上月
//
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
// 
//
//	pEnergy[OUT]:
//			指向电能的指针
// 			Hex补码表示，最高位为符号位 %0x80000000
//
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:	
//-----------------------------------------------
BOOL api_GetLadderAccuEnergyData(  BYTE DataType, BYTE *pEnergy );
#endif
//-----------------------------------------------
//函数功能: 读累计用电量BCD-645规约（结算周期）
//
//参数:
//	No[in]:
//			0 当前周期
//			1 上一周期
//
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//
//	Dot[in]: 小数点位数 最多支持2位小数
//			0: --	无小数点
//			1~N: --	1~N个小数点
//
//	pEnergy[in]:
//			指向电能的指针
// 			返回hex
//
//返回值:	TRUE   正确
//		  FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_GetAccuPeriEnergy( BYTE No, BYTE DataType, BYTE Dot, BYTE *pEnergy );

//-----------------------------------------------
//函数功能: 掉电电能处理 
//
//参数:  无
//
//返回值:	无
//
//备注:
//-----------------------------------------------
void api_PowerDownEnergy( void );

//-----------------------------------------------
//函数功能: 上电电能处理 
//
//参数:  
//			无
//
//返回值:	无
//
//备注:
//-----------------------------------------------
void api_PowerUpEnergy( void );

//-----------------------------------------------
//函数功能: 电能处理任务 
//
//参数:  无
//
//返回值:	无
//
//备注:	大循环调用
//-----------------------------------------------
void api_EnergyTask(void);

//-----------------------------------------------
//函数功能: 电能转存 
//
//参数:  无
//
//返回值:	无
//
//备注:	
//-----------------------------------------------
void api_SwapEnergy(void);

//-----------------------------------------------
//函数功能: 月结算电能，用于月累计电能计算
//
//参数:  	无
//	
//返回值:	无
//
//备注:	
//-----------------------------------------------
void api_ClosingMonEnergy( void );

DWORD api_GetEnergyConst( void );
//-----------------------------------------------
//函数功能: 电能转换为脉冲数
//
//参数:
//
//		Num[in]: 费率个数+1
//
//		BufLen[in]: 输入buf长度
//
//		Buf[in]:指向电能的指针 返回hex
//
//返回值:
//
//备注:
//-----------------------------------------------
WORD api_EnergyToEnergyDBase( BYTE Num, BYTE BufLen, BYTE* Buf );

//-----------------------------------------------
//函数功能: 脉冲尾数转换为电能
//
//参数:
//		SwitchEnergyType[in]: 转化高精度电能   	转化普通电能
//
//		Num[in]: 费率个数+1
//
//		BufLen[in]: 输入bufg长度
//
//		Buf[in]:指向电能的指针 返回hex
//
//返回值:
//
//备注:
//-----------------------------------------------
WORD api_EnergyDBaseToEnergy( eSwitchEnergyType SwitchEnergyType, BYTE Num, BYTE BufLen, BYTE* Buf );

//-----------------------------------------------
//函数功能:	获取电能地址
//
//参数:	pBuf[in/out]
//
//返回值:	4字节电能增量地址+4字节电能备份地址+4字节EE存储地址
//
//备注:	方便测试组测试
//-----------------------------------------------
BYTE api_GetFactoryEnergyPara(BYTE *pBuf);

#endif//__ENERGY_API_H
