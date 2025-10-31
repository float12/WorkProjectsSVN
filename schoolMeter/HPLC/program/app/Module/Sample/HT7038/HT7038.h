//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.10.8
//描述		采样芯片HT7038 模块内头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __HT7038_H
#define __HT7038_H

#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define SEL_VOL_ANGLE_FUNC	YES
#define SEL_MEASURE_FREQ	YES

//零线电流通道增益选择
#define CUR_ZERO_CURRENT_GAIN	16		//只能选1,2,8,16
//电压值 单位0.01mv 9个169k 1个33k 1个1k
#define	VOL_VALUE		( ((DWORD)(wCaliVoltageConst/10)*1*SimpleVolConst/10) * wSampleVolGainConst )
//电流值 
#define	CURRENT_VALUE	((WORD)(wSampleCurGainConst*SimpleCurrConst))
#define	CURRENT_MIN_VALUE	((WORD)(wSampleMinCurGainConst*SimpleCurrConst))

//零线电流计量芯片ADC采样值
#define ZERO_CURRENT_VALUE	((float)CUR_ZERO_CURRENT_GAIN * (float)ZeroSampleCurrConst / 1000.0)
#define	UN_IB_EC		((float)(wCaliVoltageConst/10)*((float)wMeterBasicCurrentConst/1000)*(api_GetActiveConstant()))
//做成2、3、4、6、12能够整除 所以除以12再乘以12
//HFConst＝INT[2.592*10^10*G*G*Vu*Vi/(EC*Un*Ib)] G=1.163 系数为2.592 * 1.163 * 1.163 = 3.505858848
//系数本是35058.58848，但VOL_VALUE扩了100倍，此系数除以100
#define	HFCONST_DEF		( (DWORD)(((float)350.5858848*VOL_VALUE*CURRENT_VALUE/UN_IB_EC+6)/12)*12 )//保证可以被12整除

#define	HFCONST_MIN_DEF		( (DWORD)(wSampleMinCurGainConst*HFCONST_DEF/wSampleCurGainConst))//保证可以被12整除(小电流增益倍数不一致时使用，特殊处理)


#define TWO_POW24		(DWORD)0x01000000
#define TWO_POW23		(DWORD)0x00800000
#define TWO_POW22		(DWORD)0x00400000
#define TWO_POW21		(DWORD)0x00200000
#define TWO_POW20		(DWORD)0x00100000
#define TWO_POW19		(DWORD)0x00080000
#define TWO_POW18		(DWORD)0x00040000
#define TWO_POW17		(DWORD)0x00020000
#define TWO_POW16		(DWORD)0x00010000
#define TWO_POW15		(DWORD)0x00008000
#define TWO_POW14		(DWORD)0x00004000
#define TWO_POW13		(DWORD)0x00002000
#define TWO_POW12		(DWORD)0x00001000
#define TWO_POW11		(DWORD)0x00000800
#define TWO_POW10		(DWORD)0x00000400
#define TWO_POW09		(DWORD)0x00000200
#define TWO_POW08		(DWORD)0x00000100
#define TWO_POW07		(DWORD)0x00000080
#define TWO_POW06		(DWORD)0x00000040
#define TWO_POW05		(DWORD)0x00000020
#define TWO_POW04		(DWORD)0x00000010
#define TWO_POW03		(DWORD)0x00000008
#define TWO_POW02		(DWORD)0x00000004
#define TWO_POW01		(DWORD)0x00000002
#define TWO_POW00		(DWORD)0x00000001


#define r_DeviceID	0x00
#define r_PA		0x01//有功功率
#define r_PB		0x02
#define r_PC		0x03
#define r_PT		0x04
#define r_QA		0x05//无功功率
#define r_QB		0x06
#define r_QC		0x07
#define r_QT		0x08
#define r_SA		0x09//视在功率
#define r_SB		0x0a
#define r_SC		0x0b
#define r_ST		0x0c

#define r_UARMS		0x0d//电压有效值
#define r_UBRMS		0x0e
#define r_UCRMS		0x0f
#define r_IARMS		0x10//电流有效值
#define r_IBRMS		0x11
#define r_ICRMS		0x12
#define r_ITRMS		0x13

#define r_PFA		0x14//功率因数
#define r_PFB		0x15
#define r_PFC		0x16
#define r_PFT		0x17
#define r_PGA		0x18//电流和电压夹角
#define r_PGB		0x19
#define r_PGC		0x1a

#define r_INTFlag	0x1b//中断标志，读后自动清零

#define r_Freq		0x1c//频率

#define r_EFlag		0x1d//电能寄存器的工作状态，读后清零
#define r_EPA		0x1e//A相有功电能（可配置为读后清零）
#define r_EPB		0x1f
#define r_EPC		0x20
#define r_EPT		0x21
#define r_EQA		0x22
#define r_EQB		0x23
#define r_EQC		0x24
#define r_EQT		0x25

#define r_YUAUB		0x26//A相电压和B相电压的夹角
#define r_YUAUC		0x27
#define r_YUBUC		0x28

#define r_I0RMS		0x29//零线电流有效值

#define r_TPSD		0x2a//温度

#define r_UTRMS		0x2b//三相电压矢量和

#define r_SFlag		0x2c//断相、相序等标志状态

#define r_BckReg	0x2d//通讯备份寄存器

#define r_ComChkSum	0x2e//通讯校验和寄存器

#define r_Sample_IA	0x2f//采样值寄存器
#define r_Sample_IB	0x30
#define r_Sample_IC	0x31
#define r_Sample_UA	0x32
#define r_Sample_UB	0x33
#define r_Sample_UC	0x34

#define r_ESA		0x35//视在电能
#define r_ESB		0x36
#define r_ESC		0x37
#define r_EST		0x38
#define r_FstCntA	0x39//快速脉冲计数
#define r_FstCntB	0x3a
#define r_FstCntC	0x3b
#define r_FstCntT	0x3c

#define r_LineIaRrms	0x4b//基波/谐波电流有效值
#define r_LineIbRrms	0x4c
#define r_LineIcRrms	0x4d


#define r_PFlag		0x3d//功率方向 0正1负

#define r_ChkSum	0x3e//校表数据校验和

#define r_Sample_I0	0x3f//零线电流采样数据

#define r_ChkSum1	0x5e//新版扩展校表数据校验和

//HT7034(7036)新加寄存器
#define w_WaveBuffTx	0x7E
#define w_WaveData		0x7F

//0x00 保留
#define w_ModeCfg	0x01
#define w_PGACtrl	0x02
#define w_EMUCfg	0x03

#define w_PGainA	0x04
#define w_PGainB	0x05
#define w_PGainC	0x06
#define w_QGainA	0x07
#define w_QGainB	0x08
#define w_QGainC	0x09
#define w_SGainA	0x0a
#define w_SGainB	0x0b
#define w_SGainC	0x0c

#define w_PhSregApq0	0x0d
#define w_PhSregBpq0	0x0e
#define w_PhSregCpq0	0x0f
#define w_PhSregApq1	0x10
#define w_PhSregBpq1	0x11
#define w_PhSregCpq1	0x12
#define w_POffsetA		0x13
#define w_POffsetB		0x14
#define w_POffsetC		0x15

#define w_QPhsCal		0x16

#define w_UGainA		0x17
#define w_UGainB		0x18
#define w_UGainC		0x19
#define w_IGainA		0x1a
#define w_IGainB		0x1b
#define w_IGainC		0x1c

#define w_IStartup		0x1d

#define w_HFConst		0x1e

#define w_FailVoltage	0x1f

#define w_GainADC7		0x20

#define w_QOffsetA		0x21
#define w_QOffsetB		0x22
#define w_QOffsetC		0x23
#define w_UARMSOffset	0x24
#define w_UBRMSOffset	0x25
#define w_UCRMSOffset	0x26
#define w_IARMSOffset	0x27
#define w_IBRMSOffset	0x28
#define w_ICRMSOffset	0x29
#define w_UOffsetA		0x2a
#define w_UOffsetB		0x2b
#define w_UOffsetC		0x2c
#define w_IOffsetA		0x2d
#define w_IOffsetB		0x2e
#define w_IOffsetC		0x2f

#define w_EMUIE			0x30
#define w_ModuleCfg		0x31
#define w_AllGain		0x32
#define w_HFDouble		0x33
//0x34 空
#define w_PinCtrl		0x35
#define w_PStart		0x36

#define w_IRegion		0x37

#define w_POffsetAL		0x64
#define w_POffsetBL		0x65
#define w_POffsetCL		0x66
#define w_QOffsetAL		0x67
#define w_QOffsetBL		0x68
#define w_QOffsetCL		0x69

#define w_ItRmsOffset	0x6A
//Toffset 校正寄存器(校表参数0x6B) 
#define w_Toffset		0x6B

//Vrefgain 的补偿曲线系数TCcoffA ，TCcoffB，TCcoffC
#define w_TCcoffA		0x6D
#define w_TCcoffB		0x6E
#define w_TCcoffC		0x6F

//新增算法控制寄存器
#define w_EMCfg			0x70
//0x38 保留
//0x39 保留


//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//芯片控制参数
typedef struct TSampleCtrlPara_t
{
	WORD ModeCfg;

	WORD PGACtrl;

	WORD EMUCfg;

	WORD QPhsCal;

	WORD IStartup;

	WORD HFConst;

	WORD FailVoltage;

	//WORD GainADC7;

	WORD EMUIE;

	WORD ModuleCfg;

	WORD AllGain;

	WORD HFDouble;

	WORD PinCtrl;

	WORD PStart;

	WORD IRegion;
	
	WORD EMCfg;

	DWORD CRC32;//4字节校验

}TSampleCtrlPara;


typedef struct TSampleAdjustPara_t
{
	WORD PGain[3];//有功功率增益系数

	WORD QGain[3];

	WORD SGain[3];

	WORD PhsReg[2][3];//角度修正系数

	WORD UGain[3];//电压增益系数

	WORD IGain[3];//电流增益系数
	
	WORD NGain;	//零线电流增益系数

	DWORD CRC32;//4字节校验

}TSampleAdjustPara;

typedef struct TSampleOffsetPara_t
{
	WORD POffset[3];//有功功率偏移

	WORD POffsetL[3];//有功功率偏移
	
	WORD QOffset[3];
	
	WORD QOffsetL[3];

	WORD URMSOffset[3];

	WORD IRMSOffset[3];

	WORD ItRmsOffset;//电流矢量和偏置
	
	WORD I0RMSOffset;//零线电流偏置
	
	DWORD CRC32;//4字节校验

}TSampleOffsetPara;

//精调系数 分段补偿系数
typedef struct TSampleAdjustParaDiv_t
{
	WORD PGainDiv[3][10];					//扩展9段增益系数

	WORD PhsRegDiv[3][10];					//扩展9段角差系数

	DWORD CRC32;
}TSampleAdjustParaDiv;

//手动修正系数
typedef struct TSampleManualModifyPara_t
{
	BYTE		ManualModifyChipToffset;	//计量芯片中TOffset温补数值，即在25°下测量寄存器0x2A读出数值
	signed char ManualModifyForWard[9];		//正向有功精度修正，仅仅修正增益部分
	signed char ManualModifyForRA[9];  		//反向有功精度修正，仅仅修正增益部分
	signed char ManualModifyQGain[3][9];	//手动修正无功参数存储区，只对无功增益进行修正
	WORD 		VolUnbalanceGain[3];		//电压不平衡补偿系数
	//WORD 		VolChangeGain[3];			//电压改变补偿系数
	DWORD CRC32;
}TSampleManualModifyPara;

//直流偶次谐波系数
typedef struct TSampleHalfWavePara_t
{
	WORD		Coe[3][2];					//0.5L直流偶次谐波 [][0]校准值 [][1]电流值
	WORD		Gain[3][2];					//1.0L直流偶次谐波 [][0]校准值 [][1]电流值
	DWORD		CRC32;
}TSampleHalfWavePara;

//直流偶次谐波检测时间
typedef struct TSampleHalfWaveTimePara_t
{
	BYTE		Hours;						//小时
	DWORD		CRC32;
}TSampleHalfWaveTimePara;

//远动数据
typedef struct TRemoteValue_t
{
	TFourByteUnion U[3];				//无符号原码格式
	TFourByteUnion I[5];				//补码格式	单位A 保留3位小数 A B C N1 N2 
	TFourByteUnion P[4];				//0代表总值//补码格式	
	TFourByteUnion Q[4];				//补码格式		
	TFourByteUnion S[4];				//视在功率//无符号原码格式
	TFourByteUnion Cos[4];				//补码格式	
	
	#if( SEL_VOL_ANGLE_FUNC == YES )
	TFourByteUnion YUaUbUcAngle[3];		//补码格式	
	TFourByteUnion YUIAngle[3];			//补码格式
	#endif
	
	TFourByteUnion Freq;				//无符号原码格式
	TFourByteUnion PQSAverage[3][4];	//一分钟平均功率PQS
	
	TTwoByteUnion Uunbalance;			//电压不平衡率
	TTwoByteUnion Iunbalance;			//电流不平衡率	
	
	TFourByteUnion RI[3];				//补码格式	事件判定使用的电流 
	//温度
	short Temperature;	
	
}TRemoteValue;

//分段校正滞回区间上下线
typedef struct TCurrenttValue_t
{
	DWORD Upper;
	DWORD Lower;
}TCurrenttValue;

//精调默认增益和大小相角系数
typedef struct TOriginalPara_t
{
	WORD AngCoe[2][MAX_PHASE];						//原始相位系数
	WORD PGainCoe[MAX_PHASE];						//原始有功增益系数
	
	DWORD CRC32;

}TOriginalPara;

//直流偶次谐波补偿
typedef enum
{
	eAD_HALFWAVE_NO = 0,	//不是半波
	eAD_HALFWAVE_YES,		//是半波
}eGetHalfWaveStatus;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

extern TRemoteValue 			RemoteValue;				//远动数据
extern TSampleCtrlPara			SampleCtrlPara;
extern TSampleAdjustPara		SampleAdjustPara;
extern TSampleOffsetPara		SampleOffsetPara;
extern TSampleAdjustParaDiv 	SampleAdjustParaDiv;		//分段补偿数据
extern TSampleManualModifyPara	SampleManualModifyPara;		//手动修正系数
extern TSampleHalfWavePara		SampleHalfWavePara;			//直流偶次谐波系数
extern TSampleHalfWaveTimePara	SampleHalfWaveTimePara;		//直流偶次谐波要求检测时间(小时)
extern TOriginalPara			OriginalPara;	
extern TCurrenttValue			CurrentValue[9];
extern BYTE 	bySamplePowerOnTimer;
extern DWORD	AdjustDivCurrent[10];		//分段校正各点基准值
extern DWORD	dwBigStandardCurrent;		//大相角校正对应电流
extern WORD		wSmallStandardCurrent;		//小相角校正对应电流
extern BYTE 	g_ucNowSampleDivisionNo[MAX_PHASE];
extern BOOL 	UnBalanceGainFlag[3];			//电压不平衡补偿标志 TRUE-已补偿 FALSE-未补偿
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//读计量数据
DWORD ReadMeasureData( BYTE Addr );
//读校表数据
DWORD ReadCalibrateData( BYTE Addr );
BOOL WriteSampleReg( BYTE Addr, DWORD Data );
//获取采样芯片的寄存器效验和
void GetSampleChipCheckSum(void);
void UseSampleDefaultPara(WORD Type);
BOOL WriteSampleChipOffsetPara(void);
BOOL WriteSampleChipCtrlPara(void);
BOOL WriteSampleChipAdjustPara(void);
void InitSampleCaliUI( void );
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,BYTE Addr, BYTE *Buf);
BOOL Link_ReadSampleReg(BYTE Addr, BYTE *pBuf);
//7038校表寄存器写保护开关
void SampleSpecCmdOp(BYTE Cmd);
BYTE GetRowSampleSegmentNo( BYTE ucPhase );
BYTE GetRowSampleSegmentAdjustNo( BYTE ucPhase );
BOOL JudgeUnBalance( BYTE Phase );
void FreshCurrent( void );


#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )


#endif//#ifndef __HT7038_H

