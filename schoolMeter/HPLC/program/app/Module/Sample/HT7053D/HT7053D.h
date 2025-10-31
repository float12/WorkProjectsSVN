//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.7.30
//描述		采样芯片HT7017 模块内头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __HT7053D_H
#define __HT7053D_H

#if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define SEL_MEASURE_FREQ	YES
#define SEL_CRYSTALOSCILLATOR_55296M	YES				//外部晶振,一般默认选择6M的晶振,5.5296M的晶振特殊处理
//读取的瞬时值
#define	IARMS			0x06
#define	IBRMS			0x07
#define	URMS			0x08
#define	Freq_U			0x09
#define	PowerPA			0x0a
#define	PowerPB			0x10
#define	SPower			0x0c
#define	EmuStatus		0x19
#define	DeviceID		0x1c

//计量芯片通信校验和寄存器
#define COMCHECKSUM     0x17
#define CHIPID    		0x1B

//校表参数
#define	GPA				0x50
#define	GQA				0x51
#define	GSA				0x52
#define GPB     		0x54
#define	HFConst			0x61
#define	P1OFFSET		0x65
#define	IBGain			0x5b
#define	PStart			0x5f
#define	GPhs1			0x6d
#define	IARMSOS			0x69
#define	IBRMSOS			0x6a
#define P2OFFSET    	0x66
#define GPhs2			0x6e
#define	ITAMP			0x63
#define	P1OFFSETL		0x76
#define QPhsCal			0x58
#define DEC_Shift		0x64

//瞬时量系数默认值
#define METER_Uk				0x1F25		//原为0x1D92，HPLC修改lsc 
#define METER_I1k				0x1325		//原为0x0F6F，HPLC修改lsc 
#define METER_I2k				0x114C
#define METER_Pk				0x9167		 

//粗调（功率校表）
#define BASE_ADJ		0x01
//精调(误差校表)
#define ACCURATE_ADJ	0x02

//电压影响量支持的点数
// 1~2  |  Imin  -  5%   | 1.0  | 90%/110%
// 3~5  |  Itr   -  10%  | 1.0  | 80%/85%/115%
// 6~7  |  Itr   -  10%  | 0.5L | 90%/110%
// 8~9  |  10Itr -  100% | 0.5L | 90%/110%
// 10~11|  Imax  -  Imax | 0.5L | 90%/110%
#define MAX_VOLCORR_OFFSET		5	//电压影响量OFFSET校表点个数 1.0
#define MAX_VOLCORR_RGPHS		6	//电压影响量RGPHS校表点个数 0.5L
#define MAX_VOLCORR_STEP		( MAX_VOLCORR_OFFSET + MAX_VOLCORR_RGPHS ) //电压影响量总点数

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

typedef struct TRemoteValue_t
{
	TFourByteUnion   U;				//电压xxxxxxx.x
	TFourByteUnion   I[2];			//电流xxxxx.xxx lL N
	TFourByteUnion   P;				//有功功率xxxx.xxxx
	TFourByteUnion   S;				//视在功率xxxx.xxxx
	TFourByteUnion   Cos;			//功率因数xxxxx.xxx
	TFourByteUnion   Pav;			//一分钟平均功率xxxx.xxxx
	TFourByteUnion   Freq;			//电网频率
}TRemoteValue;

typedef struct TSampleAdjustReg_t
{
	TTwoByteUnion 	GP1;			//50H L线有功功率校正
	TTwoByteUnion 	GQ1;			//51H L线无功功率校正
	TTwoByteUnion 	GS1;			//52H L线视在功率校正
	TTwoByteUnion 	GP2;			//54H N线有功功率校正
	TTwoByteUnion 	RQPhsCal;		//58H 无功相位校正
	TTwoByteUnion 	RADCCON;		//59H ADC通道增益选择
	TTwoByteUnion 	RPStart;		//5fH 起动功率寄存器
	TTwoByteUnion 	RHFConst;		//61H 输出脉冲频率
	TTwoByteUnion 	RDECShift;		//64H 移采样点相位校正
	TTwoByteUnion 	RP1OFFSET;		//65H L线有功功率偏置校正	
	TTwoByteUnion 	RI1RMSOFFSET;	//69H L线有效值补偿
	TTwoByteUnion 	RI2RMSOFFSET;	//6aH N线有效值补偿
	TTwoByteUnion 	RGPhs1;			//6dH L线相位校正
	TTwoByteUnion 	RGPhs2;			//6eH N线相位校正
	TTwoByteUnion 	RP1OFFSETL;		//76H L线有功功率偏置校正低字节
	DWORD 			CRC32;			//4字节校验
}TSampleAdjustReg;

typedef struct TSampleCtrlReg
{
	TTwoByteUnion 	EMUCFG;			    //40H EMU配置寄存器
	TTwoByteUnion 	FreqCFG;		    //41H 时钟配置寄存器
	TTwoByteUnion 	ModuleEn;		    //42H 模式控制寄存器
	TTwoByteUnion 	ANAEN;		        //43H ADC开关寄存器
	TTwoByteUnion 	IOCFG;		        //45H 输出引脚配置寄存器
	TTwoByteUnion 	ANACON;		        //72H 模拟控制寄存器
	TTwoByteUnion 	MODECFG;		    //75H MODE配置寄存器	
	WORD            FrequencyFlag;		//频率选择寄存器	
	DWORD 			CRC32;			    //4字节校验
}TSampleCtrlReg;


typedef struct TSampleCorr_t
{
	TFourByteUnion	UGain;		//电压系数
	TFourByteUnion	I1Gain;		//火线电流系数
	TFourByteUnion  I2Gain;		//零线电流系数
	TFourByteUnion  P1Gain;		//火线功率系数
	TFourByteUnion  P2Gain;		//零线功率系数 暂时保留 用于零线可计量表
	DWORD 			CRC32;		//4字节校验
}TSampleCorr;

typedef struct TSampleVolCorr_t
{
	TTwoByteUnion 	RP1OFFSET[MAX_VOLCORR_OFFSET];      //65H L线有功功率偏置校正
	TTwoByteUnion 	RP1OFFSETL[MAX_VOLCORR_OFFSET];     //76H L线有功功率偏置校正低字节
	TTwoByteUnion 	RGPhs1[MAX_VOLCORR_RGPHS];          //6dH L线相位校正
	DWORD 			CRC32;				//4字节校验	
}TSampleVolCorr;
#if(SEL_F415 == YES)
typedef struct TSampleUIK_t
{
	float	Uk[3];	//电压系数
	float	Ik[3];	//电流系数
	DWORD	CRC32;
}TSampleUIK;

typedef struct TTimeTem_t
{
	TRealTimer	Time;	//时间
	SWORD		Tem;	//温度
}TTimeTem;
#endif
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern TSampleCorr		SampleCorr;	
extern TSampleAdjustReg	SampleAdjustReg;
extern TSampleVolCorr	SampleVolCorr;
extern TSampleCtrlReg   SampleCtrlReg;
extern TRemoteValue		RemoteValue;



//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
BOOL Link_ReadSampleReg(BYTE addr, BYTE *pBuf);
BOOL Link_WriteSampleReg(BYTE addr, BYTE *pBuf);
void SampleSpecCmdOp(BYTE Cmd);
void SoftResetSample( void );
BYTE GetVolCorrectLevel( BYTE Mode );
//---------------------------------------------------------------
//函数功能: 硬复位计量芯片
//
//参数:   无
//
//返回值: 无
//
//备注: 硬复位只支持硬件支持的情况下使用
//---------------------------------------------------------------
void HardResetSample( void );
//-----------------------------------------------
//函数功能: 写7053D寄存器高速串口数据输出
//
//参数:		无
//返回值: 	无
//		    
//备注: 
//-----------------------------------------------
void Link_OpenSampleUartReg( WORD Len );
#endif//#if(( SAMPLE_CHIP == CHIP_HT7017) || (SAMPLE_CHIP == CHIP_HT7053D) )
#endif//#ifndef __HT7053D_H
