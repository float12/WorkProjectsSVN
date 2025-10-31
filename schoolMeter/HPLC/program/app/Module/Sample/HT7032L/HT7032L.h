//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.10.8
//描述		采样芯片HT7038 模块内头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __HT7032L_H
#define __HT7032L_H

#if(SAMPLE_CHIP==CHIP_HT7032L)
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define SEL_VOL_ANGLE_FUNC	YES
#define SEL_MEASURE_FREQ	YES

//零线电流通道增益选择
#define CUR_ZERO_CURRENT_GAIN	16		//只能选1,2,4,8,16,24,2,4,24
//电压值 单位0.01mv 9个169k 1个33k 1个1k
#define	VOL_VALUE		( ((DWORD)(wCaliVoltageConst/10)*1*SimpleVolConst/10) * wSampleVolGainConst )
//电流值 
#define	CURRENT_VALUE	((WORD)(wSampleCurGainConst*SimpleCurrConst))

//零线电流计量芯片ADC采样值
#define ZERO_CURRENT_VALUE	((float)CUR_ZERO_CURRENT_GAIN * (float)ZeroSampleCurrConst / 1000.0)
#define	UN_IB_EC		((float)(wCaliVoltageConst/10)*((float)wMeterBasicCurrentConst/1000)*(api_GetActiveConstant()))
//做成2、3、4、6、12能够整除 所以除以12再乘以12
//HFConst＝INT[2.592*10^10*G*G*Vu*Vi/(EC*Un*Ib)] G=1.163 系数为2.592 * 1.163 * 1.163 = 3.505858848
//HFConst＝2.592*Vu*Vi*10^10*1.163*1.163*/(EC*Un*Ib)
//HFConst＝9.02*10^11*Vu*Vi/(EC*Un*IB)
//系数本是35058.58848，但VOL_VALUE扩了100倍，此系数除以100
#define	HFCONST_DEF		( (DWORD)(((float)902*VOL_VALUE*CURRENT_VALUE/UN_IB_EC+6)/12)*12 )//保证可以被12整除

#define TWO_POW32		(DWORD)0x100000000
#define TWO_POW31		(DWORD)0x80000000
#define TWO_POW30		(DWORD)0x40000000
#define TWO_POW29		(DWORD)0x20000000
#define TWO_POW28		(DWORD)0x10000000
#define TWO_POW27		(DWORD)0x08000000
#define TWO_POW26		(DWORD)0x04000000
#define TWO_POW25		(DWORD)0x02000000
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
#define r_PTP		0x04
#define r_PTN		0x05
#define r_QA		0x06//无功功率
#define r_QB		0x07
#define r_QC		0x08
#define r_QTP		0x09
#define r_QTN		0x0A
#define r_SA		0x0B//视在功率
#define r_SB		0x0C
#define r_SC		0x0D
#define r_ST		0x0E

#define r_RMSUA		0x0F//电压有效值
#define r_RMSUB		0x10
#define r_RMSUC		0x11
#define r_RMSIA		0x12//电流有效值
#define r_RMSIB		0x13
#define r_RMSIC		0x14
#define r_RMSIN		0x15//零线电流有效值
#define r_RMSUT		0x16//电压矢量和有效值
#define r_RMSIT		0x17//电流矢量和有效值
#define r_RMSUAB	0x08//线电压有效值
#define r_RMSUCB	0x19
#define r_RMSUAC	0x1A
#define r_FreqUA	0x1B//频率
#define r_FreqUB	0x1C
#define r_FreqUC	0x1D
#define r_FreqU		0x1E

#define r_PFA		0x1F//功率因数
#define r_PFB		0x20
#define r_PFC		0x21
#define r_PFT		0x22
#define r_PAUA		0x23//A 相电压角度
#define r_PAUB		0x24
#define r_PAUC		0x25
#define r_PAIA		0x26
#define r_PAIB		0x27
#define r_PAIC		0x28
#define r_PAIN		0x29

#define r_EPPA		0x2A//A相全波正向有功电能（可配置为读后清零）
#define r_EPPB		0x2B
#define r_EPPC		0x2C
#define r_EPPT		0x2D
#define r_EPQA		0x2E
#define r_EPQB		0x2F
#define r_EPQC		0x30
#define r_EPQT		0x31
#define r_ESA		0x32//视在电能
#define r_ESB		0x33
#define r_ESC		0x34
#define r_EST		0x35

#define r_ENPA		0x36//A相全波反向有功电能
#define r_ENPB		0x37
#define r_ENPC		0x38
#define r_ENPT		0x39
#define r_ENQA		0x3A
#define r_ENQB		0x3B
#define r_ENQC		0x3C
#define r_ENQT		0x3D
#define r_ENSA		0x3E//视在电能(反向电能)
#define r_ENSB		0x3F
#define r_ENSC		0x40
#define r_ENST		0x41

#define r_FPA		0x42//基波有功功率
#define r_FPB		0x43
#define r_FPC		0x44
#define r_FPTP		0x45
#define r_FPTN		0x46
#define r_FQA		0x47
#define r_FQB		0x48
#define r_FQC		0x49
#define r_FQTP		0x4A
#define r_FQTN		0x4B
#define r_FSA		0x4C
#define r_FSB		0x4D
#define r_FSC		0x4E
#define r_FST		0x4F
#define r_FRMSUA	0x50//基波电压有效值
#define r_FRMSUB	0x51
#define r_FRMSUC	0x52
#define r_FRMSIA	0x53//基波电流有效值
#define r_FRMSIB	0x54
#define r_FRMSIC	0x55
#define r_FRMSIN	0x56//基波零线电流有效值
#define r_FPFA		0x57//功率因数
#define r_FPFB		0x58
#define r_FPFC		0x59
#define r_FPFT		0x5A

#define r_FEPPA		0x5B//A相基波正向有功电能（可配置为读后清零）
#define r_FEPPB		0x5C
#define r_FEPPC		0x5D
#define r_FEPPT		0x5E
#define r_FENPA		0x5F//A相基波反向有功电能（可配置为读后清零）
#define r_FENPB		0x60
#define r_FENPC		0x61
#define r_FENPT		0x62

#define r_SPLUA		0x80//A 相电压通道 ADC 采样数据
#define r_SPLUB		0x81
#define r_SPLUC		0x82
#define r_SPLIA		0x83//A 相电流通道 ADC 采样数据
#define r_SPLIB		0x84
#define r_SPLIC		0x85
#define r_SPLIN		0x86

#define r_TPSD		0xD1//温度

#define r_BckReg	0xD2//通讯备份寄存器

#define r_ComChkSum	0xD3//通讯校验和寄存器

#define r_ChkSum0	0xD4//通讯校验和寄存器
#define r_ChkSum1	0xD5//通讯校验和寄存器
#define r_ChkSum2	0xD6//通讯校验和寄存器

#define r_Noload		0x100//潜动状态寄存器
#define r_REV_Power		0x101//功率方向寄存器
#define r_REV_Energy 	0x102//电能方向寄存器（各电能通路的反向标志）
#define r_P90_Power 	0x103//当前前有功/无功的功率因数太小
#define r_SignalStatus 	0x104//信号状态寄存器（角度基准、相位分段区间、错序标志、失压标志）
#define r_PSIIStatus	0x105//Peak / Sag / Int / Iov 这四种事件的状态 Status
#define r_RST_STA		0x106//复位状态标志

#define	r_Buffer0 			0x180		//Buffer0 缓存数据寄存器，内部自增，重复读取获取内部数据.
#define	r_UnreadNum0 		0x181		//Buffer0 未读数据的数量寄存器
#define	r_Buffer1 			0x182		//Buffer1 缓存数据寄存器，内部自增，重复读取获取内部数据
#define	r_UnreadNum1 		0x183		//Buffer1 未读数据的数量寄存器
#define	r_Buffer2 			0x184		//Buffer2 缓存数据寄存器，内部自增，重复读取获取内部数据
#define	r_UnreadNum2 		0x185		//Buffer2 未读数据的数量寄存器
#define	r_Buffer3 			0x186		//Buffer3 缓存数据寄存器，内部自增，重复读取获取内部数据
#define	r_UnreadNum3 		0x187		//Buffer3 未读数据的数量寄存器
#define	r_Buffer4 			0x188		//Buffer4 缓存数据寄存器，内部自增，重复读取获取内部数据
#define	r_UnreadNum4 		0x189		//Buffer4 未读数据的数量寄存器
#define	r_ComBuffer 		0x18A		//ComBuffer 缓存数据寄存器，内部自增，重复读取获取内部数据
#define	r_ComB_UnreadNum	0x18B		//ComBuffer 未读数据的数量寄存器


//0x00 保留
#define w_GPA	0x300	//A 相全波有功功率增益
#define w_GPB	0x301
#define w_GPC	0x302
#define w_GQA	0x303
#define w_GQB	0x304
#define w_GQC	0x305
#define w_GSA	0x306
#define w_GSB	0x307
#define w_GSC	0x308
#define w_GphsA0	0x309	//A 相全波相位校正 0
#define w_GphsA1	0x30A
#define w_GphsA2	0x30B
#define w_GphsB0	0x30C
#define w_GphsB1	0x30D
#define w_GphsB2	0x30E
#define w_GphsC0	0x30F
#define w_GphsC1	0x310
#define w_GphsC2	0x311

#define w_POffsetPA		0x313//A 相全波有功功率偏置校正
#define w_POffsetPB		0x314
#define w_POffsetPC		0x315
#define w_POffsetQA		0x316
#define w_POffsetQB		0x317
#define w_POffsetQC		0x318
#define w_RmsOffsetUA	0x319//Ua 通道全波电压有效值偏置校正
#define w_RmsOffsetUB	0x31A
#define w_RmsOffsetUC	0x31B
#define w_RmsOffsetIA	0x31C
#define w_RmsOffsetIB	0x31D
#define w_RmsOffsetIC	0x31E
#define w_RmsOffsetIN	0x31F
#define w_RmsOffsetUT	0x320//合相电压有效值偏置校正
#define w_RmsOffsetIT	0x321//合相电流有效值偏置校正

#define w_FGPA		0x330	//A 相基波有功功率增益
#define w_FGPB		0x331
#define w_FGPC		0x332
#define w_FGQA		0x333
#define w_FGQB		0x334
#define w_FGQC		0x335
#define w_FGSA		0x336
#define w_FGSB		0x337
#define w_FGSC		0x338
#define w_FGphsA0	0x339	//A 相基波相位校正 0 
#define w_FGphsA1	0x33A
#define w_FGphsA2	0x33B
#define w_FGphsB0	0x33C
#define w_FGphsB1	0x33D
#define w_FGphsB2	0x33E
#define w_FGphsC0	0x33F
#define w_FGphsC1	0x340
#define w_FGphsC2	0x341

#define w_FPOffsetPA	0x343	//A 相基波有功功率偏置校正
#define w_FPOffsetPB	0x344
#define w_FPOffsetPC	0x345
//#define w_FPOffsetQA	0x346
//#define w_FPOffsetQB	0x347
//#define w_FPOffsetQC	0x348
#define w_FRmsOffsetUA	0x349
#define w_FRmsOffsetUB	0x34A
#define w_FRmsOffsetUC	0x34B
#define w_FRmsOffsetIA	0x34C
#define w_FRmsOffsetIB	0x34D
#define w_FRmsOffsetIC	0x34E
#define w_FRmsOffsetIN	0x34F

#define w_HGPA			0x350	//A 相谐波有功功率增益
#define w_HGPB			0x351
#define w_HGPC			0x352
#define w_HPOffsetPA	0x353//A 相谐波有功功率偏置校正
#define w_HPOffsetPB	0x354
#define w_HPOffsetPC	0x355
#define w_HRmsOffsetUA	0x356
#define w_HRmsOffsetUB	0x357
#define w_HRmsOffsetUC	0x358
#define w_HRmsOffsetIA	0x359
#define w_HRmsOffsetIB	0x35A
#define w_HRmsOffsetIC	0x35B
#define w_HRmsOffsetIN	0x35C

#define w_ADCOffsetUA	0x35D
#define w_ADCOffsetUB	0x35E
#define w_ADCOffsetUC	0x35F
#define w_ADCOffsetIA	0x360
#define w_ADCOffsetIB	0x361
#define w_ADCOffsetIC	0x362
#define w_ADCOffsetIN	0x363

#define w_TGainU		0x364	//U 采样通道的温度增益
#define w_TGainI		0x365	//I 采样通道的温度增益
#define w_TGainIN		0x366	//In 采样通道的温度增益

#define w_GainUA		0x367	//Ua 采样通道增益
#define w_GainUB		0x368
#define w_GainUC		0x369
#define w_GainIA		0x36a
#define w_GainIB		0x36b
#define w_GainIC		0x36c
#define w_GainIN		0x36d
#define w_DigtalGain	0x36e	//数字增益
#define w_DecShiftUaH	0x36f	//Ua 通道移动 Onebit 相位校正大拍
#define w_DecShiftUbH	0x370
#define w_DecShiftUcH	0x371
#define w_DecShiftUaL	0x372
#define w_DecShiftUbL	0x373
#define w_DecShiftUcL	0x374
#define w_DecShiftIaH	0x375	//Ua 通道移动 Onebit 相位校正大拍
#define w_DecShiftIbH	0x376
#define w_DecShiftIcH	0x377
#define w_DecShiftInH	0x378
#define w_DecShiftIaL	0x379
#define w_DecShiftIbL	0x37a
#define w_DecShiftIcL	0x37b
#define w_DecShiftInL	0x37c

#define w_PConstPa		0x37d
#define w_PConstPb		0x37e
#define w_PConstPc		0x37f
#define w_PConstPt		0x380
#define w_PConstQa		0x381
#define w_PConstQb		0x382
#define w_PConstQc		0x383
#define w_PConstQt		0x384
#define w_PConstSa		0x385
#define w_PConstSb		0x386
#define w_PConstSc		0x387
#define w_PConstSt		0x388
#define w_PConstFPa		0x389
#define w_PConstFPb		0x38a
#define w_PConstFPc		0x38b
#define w_PConstFPt		0x38c
#define w_PConstHPa		0x38d
#define w_PConstHPb		0x38e
#define w_PConstHPc		0x38f
#define w_PConstHPt		0x390

#define w_CFCntPosPa	0x3A0//A 相全波正向有功快速脉冲计数
#define w_CFCntPosPb	0x3A1
#define w_CFCntPosPc	0x3A2
#define w_CFCntPosPt	0x3A3
#define w_CFCntPosQa	0x3A4
#define w_CFCntPosQb	0x3A5
#define w_CFCntPosQc	0x3A6
#define w_CFCntPosQt	0x3A7
#define w_CFCntPosSa	0x3A8
#define w_CFCntPosSb	0x3A9
#define w_CFCntPosSc	0x3AA
#define w_CFCntPosSt	0x3AB
#define w_CFCntNegPa	0x3AC//A 相全波反向有功快速脉冲计数
#define w_CFCntNegPb	0x3AD
#define w_CFCntNegPc	0x3AE
#define w_CFCntNegPt	0x3AF
#define w_CFCntNegQa	0x3B0
#define w_CFCntNegQb	0x3B1
#define w_CFCntNegQc	0x3B2
#define w_CFCntNegQt	0x3B3
#define w_CFCntNegSa	0x3B4
#define w_CFCntNegSb	0x3B5
#define w_CFCntNegSc	0x3B6
#define w_CFCntNegSt	0x3B7

#define w_FCFCntPosPa	0x3B8//A 相全波正向有功快速脉冲计数
#define w_FCFCntPosPb	0x3B9
#define w_FCFCntPosPc	0x3BA
#define w_FCFCntPosPt	0x3BB
#define w_FCFCntPosQa	0x3BC
#define w_FCFCntPosQb	0x3BD
#define w_FCFCntPosQc	0x3BE
#define w_FCFCntPosQt	0x3BF
#define w_FCFCntNegPa	0x3C0//A 相全波反向有功快速脉冲计数
#define w_FCFCntNegPb	0x3C1
#define w_FCFCntNegPc	0x3C2
#define w_FCFCntNegPt	0x3C3
#define w_FCFCntNegQa	0x3C4
#define w_FCFCntNegQb	0x3C5
#define w_FCFCntNegQc	0x3C6
#define w_FCFCntNegQt	0x3C7

#define w_UconstA		0x3D0//A相常数电压通道(替代 All_RMS_Ux 去计算各相视在功率并累加电能(只有全波))
#define w_UconstB		0x3D1//B相常数电压通道
#define w_UconstC		0x3D2//C相常数电压通道

#define w_ZXLostCFG		0x3D3//过零相关事件配置
#define w_PStart		0x3D4//有功起动功率设置寄存器
#define w_QStart		0x3D5//无功起动功率设置寄存器
#define w_HarStart		0x3D6//谐波起动功率设置寄存器

#define w_AVGEnergy		0x3D7//瞬时功率经过该平均模块后得到平均功率
#define w_AVGEnergyREG	0x3D8
#define w_AVGRMS		0x3D9
#define w_AVGRMST		0x3DA

#define w_HFConstAF		0x3DB//
#define w_HFConstHar	0x3DC

#define w_AFDIV			0x3DD//全波/基波电能脉冲分辨率
#define w_HarDIV		0x3DE//谐波电能脉冲分辨率
#define w_ChannelSel	0x3DF//采样通道互换与反向寄存器
#define w_PGACON		0x3E0//7 路采样通道的模拟 PGA 控制
#define w_ADCCFG		0x3E1//系统时钟,源头等 ADC 配置
#define w_ADCCON		0x3E2//ADC 开关，事件的功能开关等
#define w_EMUCFG		0x3E3//计量相关配置选择
#define w_FilterCon		0x3E4//Rosi与常数电压寄存器
#define w_FilterCFG		0x3E5//常规滤波器系数配置
#define w_StartCFG		0x3E6//起动潜动配置
#define w_MULCFG		0x3E7//基波谐波功能配置寄存器
#define w_GMULFund		0x3E8//基波 MULGain，"全-基"计算谐波时，用于补偿基波滤波器的衰减
#define w_ANACFG1		0x3E9//模拟配置
#define w_ANACFG2		0x3EA//模拟配置
#define w_ANACFG3		0x3EB//模拟配置
#define w_ADCTMLOAD		0x3EC//ADC 数据外灌模式的配置

#define w_TPSCTL		0x3F5//TPS 功能配置
#define w_TPSCal		0x3F6//TPS 校正寄存器
#define w_FailVoltage	0x3F7//失压阈值，和 All_RMSU 的 Bit[23：8]对齐
#define w_CFxRun		0x3F8//脉冲输出开关，电能通路的能量累加独立使能
#define w_EnergyCon		0x3F9//能量控制，计算电能的功率选择
#define w_PowerSrcCFG	0x3FA//功率源配置，选择用于电能累加的功率
#define w_PowerModeCFG	0x3FB//功率选择模式寄存器，电能累计模块与功率分拣四种模式
#define w_CFCFG			0x3FC//脉冲输出寄存器
#define w_PowerPN_THO	0x3FD//分相正反相阈值寄存器

#define w_UZCLVL		0x40B//电压通道过零阈值
#define w_IZCLVL		0x40C//电流通道过零阈值

#define w_TUCCOFFA		0x40d//自动温补系数 U
#define w_TUCCOFFB		0x40e//自动温补系数 U
#define w_TUCCOFFC		0x40f//自动温补系数 U
#define w_TICCOFFA		0x410//自动温补系数 I
#define w_TICCOFFB		0x411//自动温补系数 I
#define w_TICCOFFC		0x412//自动温补系数 I
#define w_TInCCOFFA		0x413//自动温补系数 I
#define w_TInCCOFFB		0x414//自动温补系数 I
#define w_TInCCOFFC		0x415//自动温补系数 I

#define w_IRegion0		0x416	//相位分段校正电流阈值0
#define w_IRegion1		0x417	//相位分段校正电流阈值1
#define w_IRegion0Hys	0x418	//相位分段校正电流阈值0滞回区间
#define w_IRegion1Hys	0x419	//相位分段校正电流阈值1滞回区间
#define w_FlickerCfg	0x41a	//Flicker 配置寄存器

#define w_DECReset		0x421	//DEC 滤波器清零寄存器

#define w_EMUWPREG		0x422	//写保护寄存器
#define w_SRSTREG		0x423	//软件复位寄存器

#define w_SPICFG		0x424	//SPI 配置寄存器
#define w_HSDCCFG		0x425	//HSDC 配置寄存器
#define w_FrameCFG		0x426	//HSDC 组帧寄存器

#define w_EMUIE1		0x480	//EMU 中断使能 1
#define w_EMUIE2		0x481	//EMU 中断使能 2
#define w_EMUIE3		0x482	//EMU 中断使能 3
#define w_EMUIF1		0x483	//EMU 中断标志 1
#define w_EMUIF2		0x484	//EMU 中断标志 2
#define w_EMUIF3		0x485	//EMU 中断标志 3
#define	w_EMUIF_EnergyPOVIF		0x486	//EMU溢出中断(R/W)							x
#define	w_EMUIF_EnergyNOVIF		0x487	//EMU溢出中断(R/W)							x
#define w_PLLCTRL		0x500	//PLL 配置寄存器

#define	w_PLL_Gain				0x501	//用户需要保持默认(推荐)值
#define	w_PLL_ACC				0x503	//用户需要保持默认(推荐)值
#define	w_SFO_EST				0x504	//用户需要保持默认(推荐)值
#define	w_LOCK_THO				0x505	//用户需要保持默认(推荐)值
#define	w_AMP_THO				0x506	//用户需要保持默认(推荐)值
#define	w_TED_RSA				0x507	//用户需要保持默认(推荐)值
#define	w_LTV_CFG				0x540	//LVT模块配置寄存器
#define	w_SFO_Value				0x541	//SFO数据
#define	w_DFT_CFG				0x543	//Ux,Ix的右移，用户需要保持默认(推荐)值
#define	w_HpRMS_Ctl_U			0x545	//半波有效值的过零点校正
#define	w_HpRMS_Ctl_I			0x546	//半波有效值的过零点校正
#define	w_DataChnCFG			0x580	//数据通道选择寄存器
#define	w_Buffer0CFG			0x581	//Buffer0配置寄存器
#define	w_Buffer0_AddrLen		0x582	//Buffer0起始地址和长度设置寄存器
#define	w_UnRLVL_NullNum0		0x583	//Buffer0未读数据阈值与缓存预留长度设置寄存器
#define	w_Buffer1CFG			0x584	//Buffer1配置寄存器
#define	w_Buffer1_AddrLen		0x585	//Buffer1起始地址和长度设置寄存器
#define	w_UnRLVL_NullNum1		0x586	//Buffer1未读数据阈值与缓存预留长度设置寄存器
#define	w_Buffer2CFG			0x587	//Buffer2配置寄存器
#define	w_Buffer2_AddrLen		0x588	//Buffer2起始地址和长度设置寄存器
#define	w_UnRLVL_NullNum2		0x589	//Buffer2未读数据阈值与缓存预留长度设置寄存器
#define	w_Buffer3CFG			0x58A	//Buffer3配置寄存器
#define	w_Buffer3_AddrLen		0x58B	//Buffer3起始地址和长度设置寄存器
#define	w_UnRLVL_NullNum3		0x58C	//Buffer3未读数据阈值与缓存预留长度设置寄存器
#define	w_Buffer4CFG			0x58D	//Buffer4配置寄存器
#define	w_Buffer4_AddrLen		0x58E	//Buffer4起始地址和长度设置寄存器
#define	w_UnRLVL_NullNum4		0x58F	//Buffer4未读数据阈值与缓存预留长度设置寄存器
#define	w_ComBuff_CFG			0x590	//通用Buffer配置寄存器
#define	w_ComBuffer_AddrLen		0x591	//通用Buffer起始地址和长度设置寄存器
#define	w_UnRLVL_NullNumCom		0x592	//通用Buffer未读数据阈值与缓存预留长度设置寄存器



#define SampleCtrlRegNum  25


//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//芯片控制参数
typedef struct TSampleCtrlPara_t
{
	DWORD CtrlPara[SampleCtrlRegNum];
	DWORD CRC32;//4字节校验

}TSampleCtrlPara;


typedef struct TSampleAdjustPara_t
{
	DWORD PGain[3];//有功功率增益系数

	DWORD QGain[3];

	DWORD SGain[3];

	DWORD PhsReg[3][3];//角度修正系数

	DWORD UGain[3];//电压增益系数

	DWORD IGain[3];//电流增益系数
	
	DWORD NGain;	//零线电流增益系数

	DWORD CRC32;//4字节校验

}TSampleAdjustPara;

typedef struct TSampleOffsetPara_t
{
	DWORD Offset[15];//偏移
	
	DWORD CRC32;//4字节校验

}TSampleOffsetPara;

//精调系数 分段补偿系数
typedef struct TSampleAdjustParaDiv_t
{
	DWORD PGainDiv[3][10];					//扩展9段增益系数

	DWORD PhsRegDiv[3][10];					//扩展9段角差系数
	
	DWORD CRC32;
}TSampleAdjustParaDiv;

//手动修正系数
typedef struct TSampleManualModifyPara_t
{
	BYTE		ManualModifyChipToffset;	//计量芯片中TOffset温补数值，即在25°下测量寄存器0x2A读出数值
	signed char ManualModifyForWard[9];		//正向有功精度修正，仅仅修正增益部分
	signed char ManualModifyForRA[9];  		//反向有功精度修正，仅仅修正增益部分
	signed char ManualModifyQGain[3][9];	//手动修正无功参数存储区，只对无功增益进行修正
	DWORD 		VolUnbalanceGain[3];		//电压不平衡补偿系数
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
	TFourByteUnion P[5];				//0代表总值//补码格式	
	TFourByteUnion Q[5];				//补码格式		
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
	DWORD AngCoe[3][MAX_PHASE];						//原始相位系数
	DWORD PGainCoe[MAX_PHASE];						//原始有功增益系数
	
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
extern TOriginalPara			OriginalPara;	
extern TCurrenttValue			CurrentValue[9];
extern BYTE 	bySamplePowerOnTimer;
extern DWORD	AdjustDivCurrent[10];		//分段校正各点基准值
extern DWORD		dwBigStandardCurrent;		//大相角校正对应电流
extern DWORD		dwSmallStandardCurrent;		//小相角校正对应电流
extern BYTE 	g_ucNowSampleDivisionNo[MAX_PHASE];
extern BOOL 	UnBalanceGainFlag[3];			//电压不平衡补偿标志 TRUE-已补偿 FALSE-未补偿
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//读计量数据
DWORD ReadSampleData( WORD Addr );
////读校表数据
//DWORD ReadCalibrateData( WORD Addr );
BOOL WriteSampleReg( WORD Addr, DWORD Data );
BOOL Link_ReadSampleRegCont(WORD Addr, WORD Num, DWORD *pBuf);
BOOL ReadSampleBuffCont(WORD Addr, WORD Num, SWORD *pBuf);


//获取采样芯片的寄存器效验和
void GetSampleChipCheckSum(void);
void UseSampleDefaultPara(WORD Type);
BOOL WriteSampleChipOffsetPara(void);
BOOL WriteSampleChipCtrlPara(void);
BOOL WriteSampleChipAdjustPara(void);
void InitSampleCaliUI( void );
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,WORD Addr, BYTE *Buf);
//7038校表寄存器写保护开关
void SampleSpecCmdOp(BYTE Cmd);
BYTE GetRowSampleSegmentNo( BYTE ucPhase );
BYTE GetRowSampleSegmentAdjustNo( BYTE ucPhase );
BOOL JudgeUnBalance( BYTE Phase );
void FreshCurrent( void );


#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )


#endif//#ifndef __HT7032_H

