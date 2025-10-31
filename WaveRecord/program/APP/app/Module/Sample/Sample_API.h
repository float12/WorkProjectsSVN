//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.7.30
//描述		采样芯片 Api头文件
//修改记录	
//用到的全局的 脉冲常数
//----------------------------------------------------------------------
#ifndef __SAMPLE_API_H
#define __SAMPLE_API_H

#if( SAMPLE_CHIP == CHIP_HT7017 )
#include ".\HT7017\HT7017.h"
#endif

#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
#include ".\HT7038\HT7038.h"
#endif
#if( SAMPLE_CHIP == CHIP_HT7627 )
#include ".\HT7627S\HT7627.h"
#endif
#if( SAMPLE_CHIP == CHIP_RN8302B )
#include ".\RN8302B\RN8302B.h"
#endif
//-----------------------------------------------
//				宏定义
//-----------------------------------------------						

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//采样安全空间
//注意：操作ee中的此结构数据需要先打开eSYS_STATUS_EN_WRITE_SAMPLEPARA标志。
typedef struct TSamplePara_t
{
	#if( SAMPLE_CHIP == CHIP_HT7017 )          
	TSampleAdjustReg    SampleAdjustReg;
	TSampleCtrlReg      SampleCtrlReg;
	TSampleCorr		    SampleCorr;
	TSampleVolCorr	    SampleVolCorr;
	BYTE			    Reserve[72];		//预留空间 size0f(TSampleSafeRom) = 188
	#endif//#if( SAMPLE_CHIP == CHIP_HT7017 )
	
	#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
	TSampleOffsetPara 			SampleOffsetPara;
	TSampleCtrlPara 			SampleCtrlPara;
	TSampleAdjustPara 			SampleAdjustPara;
	TSampleAdjustParaDiv		SampleAdjustParaDiv;
	TSampleManualModifyPara		SampleManualModifyPara;
	TSampleHalfWavePara			SampleHalfWavePara;
	TSampleHalfWaveTimePara		SampleHalfWaveTimePara;
	BYTE						Reserve[148];	//预留空间
	#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
	#if( SAMPLE_CHIP==CHIP_HT7627 )
	THT7627SAdjustPara			HT7627SAdjustPara;
	BYTE						Reserve[120];	//预留空间
	#endif
	
	#if (SAMPLE_CHIP == CHIP_RN8302B)
	TSampleAdjustReg SampleAdjustReg;
	TSampleCtrlReg SampleCtrlReg;
	TSampleCorr SampleCorr;
	TEDTDataPara EDTDataPara;
	TSampleChipPara SampleChipPara;
    TSampleUIK	SampleUIKSafeRom;
	BYTE Reserve[80]; // 预留空间
	#endif				  // #if( SAMPLE_CHIP == CHIP_RN2026 )
}TSampleSafeRom;


#if( SEL_AHOUR_FUNC == YES)
// 安时值基本结构体
typedef struct TAHour_t
{
	QWORD		AHour[MAX_PHASE+1]; //单位0.01As
	DWORD		Rsv;                //需要保留，这个是8对齐,要保证CRC32放在最后4个字节
	DWORD		CRC32;
}TAHour;

#endif



//-----------------------------------------------
//				变量声明
//-----------------------------------------------


//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 新增脉冲个数，脉冲中断调用
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注: 单相采用脉冲中断，三相采用读寄存器（此处为空函数）  
//-----------------------------------------------
void api_AddSamplePulse(void);


//-----------------------------------------------
//函数功能: 初始化采样芯片
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
BOOL api_InitSampleChip( void );

#if( MAX_PHASE != 1 )//三相表有此功能
//-----------------------------------------------
//函数功能: 	记录上电电压
//
//参数:		无 
//                 
//返回值:	 	无
//		   
//备注:   
//-----------------------------------------------
void api_RecordPowerUpVoltage(void);
#endif
//-----------------------------------------------
//函数功能: 上电初始化计量芯片
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_PowerUpSample(void);


//-----------------------------------------------
//函数功能: 掉电处理计量芯片
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_PowerDownSample(void);

//-----------------------------------------------
//函数功能: 获取SAG状态
//
//参数:		无 
//                 
//返回值: 	TRUE/FALSE
//		   
//备注:   
//-----------------------------------------------
BYTE api_GetSAGStatus(void);

//-----------------------------------------------
//函数功能: 采样芯片大循环任务
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_SampleTask(void);


//-----------------------------------------------
//函数功能: 获取采样芯片远动数据
//
//参数:	
//	Type[in]:	
//		D15-D12 0:总				PHASE_ALL
//				1:A相				PHASE_A
//				2:B相				PHASE_B	
//				3:C相				PHASE_C	
//				4:N线				PHASE_N				
//		D07-D00										单位	字节长度	小数点位数
//				0:电压				REMOTE_U		V			4			3					
//				1:电流				REMOTE_I		A 			4			4
//				2:电压相角			REMOTE_PHASEU	度			2			1	
//				3:电压电流相角		REMOTE_PHASEI	度			2			1							
//				4:有功功率			REMOTE_P		kW			4			6
//				5:无功功率			REMOTE_Q		kVAR		4			6
//				6:视在功率			REMOTE_S		kVA			4			6
//				7:一分钟平均有功功率REMOTE_P_AVE	kW			4			6						
//				8:一分钟平均无功功率REMOTE_Q_AVE	kVAR		4			6	
//				9:一分钟平均视在功率REMOTE_S_AVE	kVA			4			6	
//				A:功率因数			REMOTE_COS					4			4
//				B:电压波形失真率					%			2			2 							
//				C:电流波形失真率					%			2			2			
//				D:电压谐波含有量					%			2			2			
//				E:电流谐波含有量					%			2			2		
//				F:电网频率			REMOTE_HZ		HZ			2			2
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//	Dot[in]			小数位数 若为ff则为默认小数位数
//	Len[in]			数据长度
//  Buf[out] 		输出数据       
//返回值: 	TRUE:正确返回值		FALSE：异常
//		   
//备注:  
//-----------------------------------------------
BOOL api_GetRemoteData(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE *Buf);


//-----------------------------------------------
//函数功能: 获取采样芯片工况
//
//参数:		Type[in]	
//				0x00--电压不平衡率 
//				0x01--电流不平衡率
//				0x02--负载率
//				0x10--电压状态
//				0x11--电流状态
//				0x20--起动潜动状态
//				0x3X--当前相无功象限 0x30~0x33 分别代表总/A/B/C相无功所处象限
//返回值: 	0xffff--不支持此数据
//				0x00--电压不平衡率	数据类型：long-unsigned，单位：%，换算：-2 
//              0x01--电流不平衡率	数据类型：long-unsigned，单位：%，换算：-2
//				0x02--负载率		数据类型：long-unsigned，单位：%，换算：-2
//				0x10--电压状态		Bit15 1--电压相序状态，bit0~2	1--A/B/C相电压小于0.6Un
//				0x11--电流状态		Bit15 1--电流相序状态，bit0~2	1--A/B/C相电流小于5%Ib
//				0x20--起动潜动状态	1--潜动		0--起动
//				0x3X--当前相无功象限 返回1~4，代表一象限~四象限
//备注:   
//-----------------------------------------------
WORD api_GetSampleStatus(BYTE Type);

//---------------------------------------------------------------
//函数功能: 更新采样参数化 用于设置电表电压规格时调用
//
//参数: 	无
//                   
//返回值:   无
//
//备注:   
//---------------------------------------------------------------
void api_InitSamplePara( void );

//-----------------------------------------------
//函数功能: 校表操作
//
//参数:		AdjType[in] 	0xFF：	校表初始化
//							0x00：	零漂
//							0x01:	大电流增益 1.0(L)
//							0x02:	小电流增益 1.0(L)
//							0x51:	大电流相角 0.5L(L)
//							0x52:	小电流相角 0.5L(L)
//							0x81:	大电流增益 N线 1.0(N)
//							0xD1:	大电流相角 N线 0.5L(N)
//			Buf[in]			高字节在后，低字节在前	AA HF常数 AA A相瞬时量 AA B相瞬时量 AA C相瞬时量 AA
//							AA HFConstL HFConstH
//							AA UA1 UA2 UA3 UA4 IA1 IA2 IA3 IA4 PA1 PA2 PA3 PA4
//							AA UB1 UB2 UB3 UB4 IB1 IB2 IB3 IB4 PB1 PB2 PB3 PB4
//							AA UC1 UC2 UC3 UC4 IC1 IC2 IC3 IC4 PC1 PC2 PC3 PC4 AA 	
//          
//返回值: 	TRUE/FALSE
//		   
//备注:校表规约调用     
//-----------------------------------------------
WORD api_ProcSampleAdjust(BYTE AdjType, BYTE *Buf);


//-----------------------------------------------
//函数功能: 通讯操作校表系数
//
//参数:		Operation[in]	READ/WRITE
//          Addr[in]		0xFFFF			校表初始化
//							0x0000~0x8FFF	采样芯片寄存器
//							0x9000~0x9FFF	瞬时量系数 
//							0xA000~0xAFFF	电压影响量系数
//							0xB000~0xBFFF	自热补偿系数
//			Len[in]			返回数据长度
//			Buf[in/out]		数据域，高在后，低在前
//
//返回值: 	TRUE/FALSE
//		   
//备注: 7038，0000~3FFF为校正寄存器	4000~8fff为计量寄存器   
//-----------------------------------------------
BOOL api_ProcSampleCorr(BYTE Operation, WORD Addr, BYTE Len, BYTE *Buf);
#if(SAMPLE_CHIP != CHIP_HT7627)
//-----------------------------------------------
//函数功能: 初始化计量与精度无关参数
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void api_FactoryInitSample( void );
#endif
#if( SEL_STAT_V_RUN == YES )
// 清除当前数据
//Type 	BIT0：日数据		BIT1：月数据
void api_ClearVRunTimeNow( BYTE Type );
#endif

#if( SEL_AHOUR_FUNC == YES)
//-----------------------------------------------
//函数功能: 清安时值
//
//参数:  无
//
//返回值:	无
//
//备注:
//-----------------------------------------------
void api_ClrAHour( void );

//-----------------------------------------------
//函数功能: 读安时值
//
//参数:  Type[in]: ePHASE_TYPE
//		DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
// 		pBuf[out]: 指向返回数据的指针
//
//返回值:	无
//
//备注:   TRUE   正确
//		  FALSE  错误
//-----------------------------------------------
BOOL api_GetAHourData( BYTE Type, BYTE DataType, BYTE *pBuf );

#endif//#if( SEL_AHOUR_FUNC == YES)
//-----------------------------------------------
//函数功能: 获取计量回路数
//
//参数:	无	
//
//返回值: 回路数	
//备注:   
//-----------------------------------------------
eMeasurementMode api_GetMeasureMentMode(void);
//-----------------------------------------------
//函数功能: 设置计量回路数
//
//参数:	无	
//
//返回值: 成功/失败	
//备注:   
//-----------------------------------------------
BYTE api_SetSampleChip(BYTE bCount);
//-----------------------------------------------
//函数功能: 获取采样芯片校表数据效验和
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注:
//-----------------------------------------------
void GetSampleChipCheckSum(void);

//--------------------------------------------------
//功能描述:  获取温度
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
SWORD  api_GetTemp(void);
#endif//#ifndef __SAMPLE_API_H
