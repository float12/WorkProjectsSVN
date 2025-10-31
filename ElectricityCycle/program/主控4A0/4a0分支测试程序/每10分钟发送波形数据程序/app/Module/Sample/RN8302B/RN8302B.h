//----------------------------------------------------------------------
//Copyright (C) 2016-2023 烟台东方威思顿电气股份有限公司低压台区产品部
//创建人	   
//创建日期	2023.05.29
//描述		采样芯片RN8302B内部头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __RN8302B_H
#define __RN8302B_H
#if( SAMPLE_CHIP == CHIP_RN8302B )
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define SEL_VOL_ANGLE_FUNC		YES
#define SEL_MEASURE_FREQ		YES

//-----------------------读取的瞬时值-----------------------
//电流
#define	IA_EMU			0x000B		//IA有效值寄存器
#define	IB_EMU			0x000C		//IB有效值寄存器
#define	IC_EMU			0x000D		//IC有效值寄存器
#define	IN_EMU			0x000E		//IN有效值寄存器
#define	IN2_EMU			0x0010		//IN2有效值寄存器(零序)

//电压
#define	UA_EMU			0x0007		//UA有效值寄存器
#define	UB_EMU			0x0008		//UB有效值寄存器
#define	UC_EMU			0x0009		//UC有效值寄存器
#define	UT_EMU			0x000A		//电压矢量和有效值寄存器

//有功功率
#define	PA_EMU			0x0014		//A路有功功率
#define	PB_EMU			0x0015		//B路有功功率
#define	PC_EMU			0x0016		//C路有功功率
#define	PT_EMU			0x0017		//合相有功功率
//无功功率
#define	QA_EMU			0x0018		//A路无功功率
#define	QB_EMU			0x0019		//B路无功功率
#define	QC_EMU			0x001A		//C路无功功率
#define	QT_EMU			0x001B		//合相无功功率
//视在功率
#define	SA_EMU			0x001C		//A路视在功功率
#define	SB_EMU			0x001D		//B路视在功功率
#define	SC_EMU			0x001E		//C路视在功功率
#define	STA_EMU			0x001F		//RMS合相视在功率
#define	STV_EMU			0x004C		//PQS合相视在功率
//功率因数
#define	PfA_EMU			0x0020		//A相功率因数
#define	PfB_EMU			0x0021		//B相功率因数
#define	PfC_EMU			0x0022		//C相功率因数
#define	PfTA_EMU		0x0023		//RMS合相功率因数
#define	PfTV_EMU		0x004D		//PQS合相功率因数
//有功
#define	EPA_EMU			0x0030		//A路有功能量寄存器
#define	EPB_EMU			0x0031		//B路有功能量寄存器
#define	EPC_EMU			0x0032		//C路有功能量寄存器
#define	EPT_EMU			0x0033		//合相有功能量寄存器
//正向有功
#define	PEPA_EMU		0x0034		//A路正向有功能量寄存器
#define	PEPB_EMU		0x0035		//B路正向有功能量寄存器
#define	PEPC_EMU		0x0036		//C路正向有功能量寄存器
#define	PEPT_EMU		0x0037		//合相正向有功能量寄存器
//反向有功
#define	NEPA_EMU		0x0038			//A路反向有功能量寄存器
#define	NEPB_EMU		0x0039			//B路反向有功能量寄存器
#define	NEPC_EMU		0x003A			//C路反向有功能量寄存器
#define	NEPT_EMU		0x003B			//合相反向有功能量寄存器
//无功
#define	EQA_EMU			0x003C			//A路无功能量寄存器
#define	EQB_EMU			0x003D			//B路无功能量寄存器
#define	EQC_EMU			0x003E			//C路无功能量寄存器
#define	EQT_EMU			0x003F			//合相无功能量寄存器
//正向无功
#define	PEQA_EMU		0x0040			//A路正向无功能量寄存器
#define	PEQB_EMU		0x0041			//B路正向无功能量寄存器
#define	PEQC_EMU		0x0042			//C路正向无功能量寄存器
#define	PEQT_EMU		0x0043			//合相正向无功能量寄存器
//反向无功
#define	NEQA_EMU		0x0044			//A路反向无功能量寄存器
#define	NEQB_EMU		0x0045			//B路反向无功能量寄存器
#define	NEQC_EMU		0x0046			//C路反向无功能量寄存器
#define	NEQT_EMU		0x0047			//合相反向无功能量寄存器
//视在
#define	ESA_EMU			0x0048			//A路视在能量寄存器
#define	ESB_EMU			0x0049			//B路视在能量寄存器
#define	ESC_EMU			0x004A			//C路视在能量寄存器
#define	ESTA_EMU		0x004B			//RMS合相视在能量寄存器
#define	ESTV_EMU		0x004F			//PQS合相视在能量寄存器

//其他
#define	Freq_U_EMU		0x0057			//电压线频率寄存器
#define	Status_EMU		0x0167			//潜动状态标志寄存器
#define	PQSign_EMU		0x0166			//功率方向寄存器

//-----------------------校表参数-----------------------
//100%Ib，1.0L
#define	GPA_EMU				0x0128		//A相有功功率增益寄存器
#define	GPB_EMU				0x0129		//B相有功功率增益寄存器
#define	GPC_EMU				0x012A		//C相有功功率增益寄存器
#define	GQA_EMU				0x012B		//A相无功功率增益寄存器
#define	GQB_EMU				0x012C		//B相无功功率增益寄存器
#define	GQC_EMU				0x012D		//C相无功功率增益寄存器
#define	GSA_EMU				0x012E		//A相视在功率增益寄存器
#define	GSB_EMU				0x012F		//B相视在功率增益寄存器
#define	GSC_EMU				0x0130		//C相视在功率增益寄存器
#define GSUA_EMU			0x0113 		//UA通道增益寄存器
#define GSUB_EMU			0x0114 		//UB通道增益寄存器
#define GSUC_EMU			0x0115 		//UC通道增益寄存器
#define GSIA_EMU			0x0116 		//IA通道增益寄存器
#define GSIB_EMU			0x0117 		//IB通道增益寄存器
#define GSIC_EMU			0x0118 		//IC通道增益寄存器
#define GSIN_EMU			0x0119 		//IN通道增益寄存器
//100%Ib,0.5L
#define	PAPHSL_EMU			0x0131	//A有功功率相位校正寄
#define	PBPHSL_EMU			0x0132	//B有功功率相位校正寄
#define	PCPHSL_EMU			0x0133	//C有功功率相位校正寄
#define	QAPHSL_EMU			0x0134	//A无功功率相位校正寄
#define	QBPHSL_EMU			0x0135	//B无功功率相位校正寄
#define	QCPHSL_EMU			0x0136	//C无功功率相位校正寄
#define	PHSIA_EMU			0x010F		//IA通道相位校正寄存器				0x808080
#define	PHSIB_EMU			0x0110		//IB通道相位校正寄存器				0x808080
#define	PHSIC_EMU			0x0111		//IC通道相位校正寄存器				0x808080
#define	PHSIN_EMU			0x0112		//IN通道相位校正寄存器				0x80
//5%Ib,0.5L
//其他
#define	HFConst_EMU			0X0100	//高频脉冲常数寄存器
#define	PStart_EMU			0x0102		//有功功率启动阈值寄存器 
#define	QStart_EMU			0x0103		//无功功率启动阈值寄存器
#define	ZXOT_EMU			0x0105		//电压过零阈值寄存器
#define	ZXOTU_EMU			0x0190		//电压过零阈值寄存器
#define	LostV_EMU			0x0104		//失压阈值寄存器
//-----------------------控制类参数地址-----------------------
#define	CFCFG_EMU			0x0160		//CF脉冲配置寄存器 
#define	EMUCFG_EMU			0x0161		//计量单元配置寄存器 	
#define	EMUCFG3_EMU			0x016D		//计量单元配置寄存器3 	
#define	EMUCON_EMU			0x0162		//计量控制寄存器
#define	EMMIF_EMU			0x0165		//EMU中断标志寄存器
#define	IRegionS_EMU		0x0168		//电流分区状态寄存器
#define	PHASES_EMU			0x0169		//相电压电流状态寄存器
#define ZXOTCFG_EMU			0x0192	 	//过零计算配置及标志寄存器
#define	AUOTDC_EN_EMU		0x0191		//直流OFFSET自动校正使能寄存器 
#define ADCCTL_SYS			0x0183		//ADC控制寄存器
#define	MODSEL_EMU			0x0186		//三相四线/三相三线模式选择寄存器

#define SYSCFG_EMU			0x0178		//系统配置寄存器 
#define HSDCCTL_EMU			0x01C9		//HSDC接口配置寄存器
#define EMMIE2_EMU			0x016B		//EMU中断2使能寄存器
#define WSAVECON_EMU		0x0163		//采样数据写缓存控制寄存器
#define DMA_WAVECON_EMU		0x0193		//同步采样通道配置寄存器	
#define DMA_WAVECON3_EMU	0x01E5		//同步采样通道配置寄存器3
#define DMA_WAVECON2_EMU	0x01D1		//同步采样通道配置寄存器

#define	CKSUM_EMU1			0x016A		//校验和寄存器 3字节
#define	CKSUM_EMU2			0x018B		//校验和寄存器 2字节
#define	CKSUM_EMU3			0x01AE		//校验和寄存器 4字节
#define	CKSUM_EMU4			0x01EF		//校验和寄存器 4字节
#define	WMSW_EMU			0x0181		//模式切换寄存器地址
#define SOFTRST_EMU			0x0182		//软件复位寄存器
#define	VERRID_EMU			0x01F1		//8302B版本

#define SPEL_WREN_EMU 		0x01A1 /*" RN830x 特殊写使能寄存器地址 "*/
#define BGR_HT_EMU 			0x01F3 /*" 调试模式写使能寄存器 "*/
#define OTPTRIM_STA_EMU 	0x01F2 /*" 调试模式状态指示寄存器 "*/
#define OTPDBG_EMU 			0x01F0 /*" 调试模式控制寄存器 "*/
#define	TR1M_EMU			0x01F1		//TR1M温补系数
#define ROS_CFG				0x01ED //罗氏线圈配置寄存器
#define ROS_DCATTC			0x01EB //罗氏线圈积分直流衰减系数寄存器
#define SYSCFG				0x0178 //系统配置寄存器
#define SYSSR				0x018A //系统状态寄存器
#define ECT_WREN 			0x00d0 //误差温补配置寄存器写使能
#define SAR_CTL0 			0x00D1 //SAR 控制寄存器0
#define SAR_CTL1 			0x00D2 //SAR 控制寄存器1
#define SAR_EN	 			0x00F8 //SAR ADC 使能寄存器
/******************************未使用的寄存器***************************************/
//-----------------------只读寄存器-----------------------
#define	UAWAV_EMU			0x0000		//UA波形采样寄存器
#define	UBWAV_EMU			0x0001		//UB波形采样寄存器
#define	UCWAV_EMU			0x0002		//UC波形采样寄存器
#define	IAWAV_EMU			0x0003		//IA波形采样寄存器		
#define	IBWAV_EMU			0x0004		//IB波形采样寄存器
#define	ICWAV_EMU			0x0005		//IC波形采样寄存器
#define	INWAV_EMU			0x0006		//IN波形采样寄存器
#define	PAFCnt_EMU			0x0024		//A有功脉冲计数寄存器
#define	PBFCnt_EMU			0x0025		//B有功脉冲计数寄存器
#define	PCFCnt_EMU			0x0026		//C有功脉冲计数寄存器
#define	PTFCnt_EMU			0x0027		//合相有功脉冲计数寄存器
#define	QAFCnt_EMU			0x0028		//A无功脉冲计数寄存器
#define	QBFCnt_EMU			0x0029		//B无功脉冲计数寄存器
#define	QCFCnt_EMU			0x002A		//C无功脉冲计数寄存器
#define	QTFCnt_EMU			0x002B		//合相无功脉冲计数寄存器
#define	SAFCnt_EMU			0x002C		//A视在脉冲计数寄存器
#define	SBFCnt_EMU			0x002D		//B视在脉冲计数寄存器
#define	SCFCnt_EMU			0x002E		//C视在脉冲计数寄存器
#define	STFACnt_EMU			0x002F		//合相视在脉冲计数寄存器
#define	STFVCnt_EMU			0x004E		//合相视在脉冲计数寄存器
#define YUA_EMU				0x0050		//采样通道UA基波相角寄存器
#define YUB_EMU				0x0051		//采样通道UB基波相角寄存器
#define YUC_EMU				0x0052		//采样通道UC基波相角寄存器
#define YIA_EMU				0x0053		//采样通道IA基波相角寄存器
#define YIB_EMU				0x0054		//采样通道IB基波相角寄存器
#define YIC_EMU				0x0055		//采样通道IC基波相角寄存器
#define YIN_EMU				0x0056		//采样通道IN基波相角寄存器
//基波待添加@@@@@
#define	UA_HW_EMU			0x00A0		//UA半波有效值寄存器
#define	UB_HW_EMU			0x00A1		//UB半波有效值寄存器
#define	UC_HW_EMU			0x00A2		//UC半波有效值寄存器
#define	IA_HW_EMU			0x00A3		//IA半波有效值寄存器
#define	IB_HW_EMU			0x00A4		//IB半波有效值寄存器
#define	IC_HW_EMU			0x00A5		//IC半波有效值寄存器
#define	UAPEAF_EMU			0x00A6		//UA电压半波峰值寄存器
#define	UBPEAF_EMU			0x00A7		//UB电压半波峰值寄存器
#define	UCPEAF_EMU			0x00A8		//UC电压半波峰值寄存器

//-----------------------校表寄存器-----------------------
//通道
#define	PHSUA_EMU			0x010C		//UA通道相位校正寄存器				0x80
#define	PHSUB_EMU			0x010D		//UB通道相位校正寄存器				0x80
#define	PHSUC_EMU			0x010E		//UC通道相位校正寄存器				0x80

#define DCOS_UA_EMU			0x011A 		//UA通道直流OFFSET校正寄存器
#define DCOS_UB_EMU			0x011B 		//UB通道直流OFFSET校正寄存器
#define DCOS_UC_EMU			0x011C 		//UC通道直流OFFSET校正寄存器

#define DCOS_IA_EMU			0x011D 		//IA通道直流OFFSET校正寄存器
#define DCOS_IB_EMU			0x011E 		//IB通道直流OFFSET校正寄存器
#define DCOS_IC_EMU			0x011F 		//IC通道直流OFFSET校正寄存器
#define DCOS_IN_EMU			0x0120 		//IN通道直流OFFSET校正寄存器

#define UA_OS_EMU			0x0121 		//UA有效值OFFSET校正寄存器
#define UB_OS_EMU			0x0122 		//UB有效值OFFSET校正寄存器
#define UC_OS_EMU			0x0123 		//UC有效值OFFSET校正寄存器

#define	IAOS_EMU			0x0124		//IA有效值OFFSET校正寄存器
#define	IBOS_EMU			0x0125		//IB有效值OFFSET校正寄存器
#define	ICOS_EMU			0x0126		//IC有效值OFFSET校正寄存器
#define	INOS_EMU			0x0127		//IN有效值OFFSET校正寄存器

#define FUA_OS_EMU			0x013D 	//UA基波有效值OFFSET校正寄存器
#define FUB_OS_EMU			0x013E 	//UB基波有效值OFFSET校正寄存器
#define FUC_OS_EMU			0x013F 	//UC基波有效值OFFSET校正寄存器

#define	FIAOS_EMU			0x0140		//IA基波有效值OFFSET校正寄存器
#define	FIBOS_EMU			0x0141		//IB基波有效值OFFSET校正寄存器
#define	FICOS_EMU			0x0142		//IC基波有效值OFFSET校正寄存器

//功率
#define	GFPA_EMU			0x0143		//A相基波有功功率增益寄存器
#define	GFPB_EMU			0x0144		//B相基波有功功率增益寄存器
#define	GFPC_EMU			0x0145		//C相基波有功功率增益寄存器

#define	GFQA_EMU			0x0146		//A相基波无功功率增益寄存器
#define	GFQB_EMU			0x0147		//B相基波无功功率增益寄存器
#define	GFQC_EMU			0x0148		//C相基波无功功率增益寄存器

#define	GFSA_EMU			0x0149		//A相基波视在功率增益寄存器
#define	GFSB_EMU			0x014A		//B相基波视在功率增益寄存器
#define	GFSC_EMU			0x014B		//C相基波视在功率增益寄存器

#define	FPAPHSL_EMU			0x014C	//A有基波功功率相位校正寄
#define	FPBPHSL_EMU			0x014D	//B有基波功功率相位校正寄
#define	FPCPHSL_EMU			0x014E	//C有基波功功率相位校正寄
#define	FQAPHSL_EMU			0x014F	//A无基波功功率相位校正寄
#define	FQBPHSL_EMU			0x0150	//B无基波功功率相位校正寄
#define	FQCPHSL_EMU			0x0151	//C无基波功功率相位校正寄

#define	PAOS_EMU			0x0152		//A有功功率OFFSET校正寄存器
#define	PBOS_EMU			0x0153		//B有功功率OFFSET校正寄存器
#define	PCOS_EMU			0x0154		//C有功功率OFFSET校正寄存器

#define	QAOS_EMU			0x0155		//A无功功率OFFSET校正寄存器
#define	QBOS_EMU			0x0156		//B无功功率OFFSET校正寄存器
#define	QCOS_EMU			0x0157		//C无功功率OFFSET校正寄存器

/*
#define	D2FP00_EMU			(DWORD)&EMU->PA_PHSM_D2FP00	//自定义功率寄存器0				0x00
#define	D2FP01_EMU			(DWORD)&EMU->PA_PHSH_D2FP01	//自定义功率寄存器1				0x00
#define	D2FP02_EMU			(DWORD)&EMU->PB_PHSM_D2FP02	//自定义功率寄存器2				0x00
#define	D2FP03_EMU			(DWORD)&EMU->PB_PHSH_D2FP03	//自定义功率寄存器3				0x00
#define	D2FP04_EMU			(DWORD)&EMU->PC_PHSM_D2FP04	//自定义功率寄存器4				0x00
#define	D2FP05_EMU			(DWORD)&EMU->PC_PHSH_D2FP05	//自定义功率寄存器5				0x00
#define	D2FP06_EMU			(DWORD)&EMU->QA_PHSM_D2FP06	//自定义功率寄存器6				0x00
#define	D2FP07_EMU			(DWORD)&EMU->QA_PHSH_D2FP07	//自定义功率寄存器7				0x00
#define	D2FP08_EMU			(DWORD)&EMU->QB_PHSM_D2FP08	//自定义功率寄存器8				0x00
#define	D2FP09_EMU			(DWORD)&EMU->QB_PHSH_D2FP09	//自定义功率寄存器9				0x00
#define	D2FP10_EMU			(DWORD)&EMU->QC_PHSM_D2FP10	//自定义功率寄存器10				0x00
#define	D2FP11_EMU			(DWORD)&EMU->QC_PHSH_D2FP11	//自定义功率寄存器11				0x00

#define	PA_PHSM_EMU			(DWORD)&EMU->PA_PHSM_D2FP00	//A有功功率相位校正寄中			0X00
#define	PA_PHSH_EMU			(DWORD)&EMU->PA_PHSH_D2FP01	//A有功功率相位校正寄高			0X00
#define	PB_PHSM_EMU			(DWORD)&EMU->PA_PHSM_D2FP02	//B有功功率相位校正寄中			0X00
#define	PB_PHSH_EMU			(DWORD)&EMU->PA_PHSH_D2FP03	//B有功功率相位校正寄高			0X00
#define	PC_PHSM_EMU			(DWORD)&EMU->PA_PHSM_D2FP04	//C有功功率相位校正寄中			0X00
#define	PC_PHSH_EMU			(DWORD)&EMU->PA_PHSH_D2FP05	//C有功功率相位校正寄高			0X00
#define	QA_PHSM_EMU			(DWORD)&EMU->QA_PHSM_D2FP06	//A无功功率相位校正寄中			0X00
#define	QA_PHSH_EMU			(DWORD)&EMU->QA_PHSH_D2FP07	//A无功功率相位校正寄高			0X00
#define	QB_PHSM_EMU			(DWORD)&EMU->QA_PHSM_D2FP08	//B无功功率相位校正寄中			0X00
#define	QB_PHSH_EMU			(DWORD)&EMU->QA_PHSH_D2FP09	//B无功功率相位校正寄高			0X00
#define	QC_PHSM_EMU			(DWORD)&EMU->QA_PHSM_D2FP10	//C无功功率相位校正寄中			0X00
#define	QC_PHSH_EMU			(DWORD)&EMU->QA_PHSH_D2FP11	//C无功功率相位校正寄高			0X00
*/
//PRTH1L寄存器为0时,不启动分段相位校正
#define	PRTH1L_EMU			0x0106		//相位补偿区域设置寄存器				0X00												
#define	PRTH1H_EMU			0x0107		//相位补偿区域设置寄存器				0X00
#define	PRTH2L_EMU			0x0108		//相位补偿区域设置寄存器				0X00
#define	PRTH2H_EMU			0x0109		//相位补偿区域设置寄存器				0X00
#define	IRegion3L_EMU		0x010A	//电流阈值3下限
#define	IRegion3H_EMU		0x010B	//电流阈值3上限
//直流OFFSET校正寄存器在高通之后，当AUTODC_EN.BIT7 =0,无论采样通路高通是否关闭，均会影响EMU通道有效值、功率、能量等参数。
#define	OILV_EMU			0x015A		//过流阈值寄存器					0X00
#define	OVLVL_EMU			0x0159		//过压阈值寄存器					0X00
#define	OI2_LVL_EMU			0x0198		//过流幅值阈值寄存器				0x00
#define	OI2_CNTTH_EMU		0x0199		//过流时间阈值寄存器				0X00
#define	OV2_LVL_EMU			0x019C		//过压幅值阈值寄存器				0X00
#define	OV2_CNTTH_EMU		0x019D		//过压时间阈值寄存器				0X00

//-----------------------控制类寄存器-----------------------
#define	SAGCFG_EMU			0x0158		//电压暂降阈值寄存器					0X00
#define	EMMIE_EMU			0x0164		//EMU中断寄存器					0X00
#define	D2FCFG_EMU			0x0195		//自定义电能脉冲寄存器				0X00
#define	CFG2_EMU			0x019A	//计量单元配置寄存器2				0X04
#define	HFConst2_EMU		0x0101		//高频脉冲常数寄存器2			0x1000
#define	HFConst3_EMU		0x0196		//高频脉冲常数寄存器3			0x00

/*********************************************************************************/
//-----------------------DMA相关寄存器-----------------------
/*
#define	BUF_CTL_EMU			((DWORD )&EMU->BUF_CTRL 	)
#define	BUF_BASERAD_EMU 	((DWORD )&EMU->BUF_BASEADDR )
#define	BUF_DEPTH_EMU 		((DWORD )&EMU->BUF_DEPTH  	)
#define	WAVE_CHSEL_EMU	 	((DWORD )&EMU->WAVE_CHSEL 	)
#define	WAVE_ERRADDR_EMU 	((DWORD )&EMU->WAVE_ERRADDR )
#define	WAVECON_EMU 		((DWORD )&EMU->WAVECON		)
#define	EMMIE2_EMU 			((DWORD )&EMU->EMMIE2		)
#define	EMMIF2_EMU			((DWORD )&EMU->EMMIF2		)
*/
//电压影响量支持的点数
#define MAX_VOLCORR_STEP		10
#define P_CREEP					0x01		//有功功率潜动
#define Q_CREEP					0x10		//有功功率潜动
#define NUM_FFT 				128			// 瞬时数据点数


#define ADCCTL_SYS_REMAP        0x1000      //对应HT6029里面的定义，映射到寄存器ADCCTL_SYS上

//误差自监测寄存器（厂内扩展） //@liang
#define EDT_CMD_TYPE_MASK       0xFF00
#define EDT_CMD_CHANNEL_MASK    0x00F0
#define EDT_CMD_PHASE_MASK      0x000F

#define EDT_STATUS              0xF000      //EDT状态查询，0校准中/1校准完成
#define EDT_CALI                0xF100      //EDT校准命令            
#define EDT_PARAM               0xF200      //EDT参数基准（0xF200开始，共24个，先幅值后相位，先电流的ABC，后电压123的ABC）
#define EDT_CONTROL             0xF300      //EDT控制命令，0关闭/1开启
#define EDT_ERR_AMP             0xF400      //幅值误差百分比
#define EDT_ERR_PHA             0xF500      //相位误差百分比
#define EDT_VERSION             0xF600      //实际是计量芯的版本（这里认为计量芯和EDT绑定）
#define HSDC_OPEN_TIME			(30 - 2)	//上电后几秒开启HSDC波形传输（拓扑识别数据），使用bySamplePowerOnTimer判断，

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	RN830xV20 = 0x20,/*"V2 版"*/
	RN830xV30 = 0x30,/*"V30 版"*/
	RN830xV31 = 0x31,/*"V31 版"*/
	RN830xV32 = 0x32,/*"V32 版"*/
	RN830xVerr = 0xff/*"错误版本"*/
}eRN830xVer_TypeDef;

//瞬时量
typedef struct TRemoteValue_t
{
	TFourByteUnion   U[3];				//电压xxxxx.xxx A B C
	TFourByteUnion   I[5];				//电流xxxx.xxxx A B C N N2
	TFourByteUnion   P[4];				//有功功率xx.xxxxxx T A B C
	TFourByteUnion   Q[4];				//无功功率xx.xxxxxx
	TFourByteUnion   S[4];				//视在功率xx.xxxxxx
	TFourByteUnion   Cos[4];			//功率因数xxxxx.xxx
	
	#if( SEL_VOL_ANGLE_FUNC == YES )
	TFourByteUnion 	YUAngle[3];			//补码格式	
	TFourByteUnion 	YIAngle[3];			//补码格式
	#endif
	
	TFourByteUnion	Pav[3][4];			//一分钟平均功率xxxx.xxxx
	TFourByteUnion	Freq;				//电网频率
	
	TTwoByteUnion	Uunbalance;			//电压不平衡率
	TTwoByteUnion	Iunbalance;			//电流不平衡率	
}TRemoteValue;

//校准系数
typedef struct TSampleAdjustReg_t
{
	TFourByteUnion 	GP[3];			//有功功率增益校正
	TFourByteUnion 	QP[3];			//无功功率增益校正
	TFourByteUnion 	SP[3];			//视在功率增益校正
	TFourByteUnion 	GSU[3];			//电压通道增益校正
	TFourByteUnion 	GSI[3];			//电流通道增益校正
	TFourByteUnion 	PHSP[3];		//有功功率相位校正
	TFourByteUnion 	PHSI[3];		//电流通道相位校正
	TFourByteUnion 	POS[3];			//有功功率偏置校正	
	TFourByteUnion 	IOS[3];			//有功功率偏置校正
	
	DWORD 			CRC32;			//4字节校验
}TSampleAdjustReg;

//控制寄存器
typedef struct TSampleCtrlReg
{
	TFourByteUnion 	CFCFG;			//CF引脚配置寄存器
	TFourByteUnion 	EMMIE;			//计量中断寄存器
	TFourByteUnion 	EMUCFG;			//EMU配置寄存器
	TFourByteUnion 	EMUCON;			//EMU控制寄存器
	TFourByteUnion	ZXOTCFG;		//过零计算配置寄存器
	TFourByteUnion 	AUOTDC;			//直流OFFSET自动校正使能寄存器
	TFourByteUnion  ADCCTL;			//ADC 增益控制寄存器
	TFourByteUnion 	HFCONST;		//输出脉冲频率
	TFourByteUnion 	PSTART;			//起动功率寄存器	
	TFourByteUnion 	MODSEL;			//三相四线和三相三线模式选择寄存器
	TFourByteUnion	PRTH1L;			//相位补偿区域设置寄存器
	TFourByteUnion	PRTH1H;			//相位补偿区域设置寄存器
	TFourByteUnion	PRTH2L;			//相位补偿区域设置寄存器
	TFourByteUnion	PRTH2H;			//相位补偿区域设置寄存器
	TFourByteUnion	SAGCFG;			//掉电检测门限	
	TFourByteUnion 	EMUCFG3;		//EMU配置寄存器
	TFourByteUnion	ZXOT;			//电流过零阈值寄存器
	TFourByteUnion	ZXOTU;			//电压过零阈值寄存器
	DWORD 			CRC32;			//4字节校验
}TSampleCtrlReg;

//比例系数
typedef struct TSampleCorr_t
{
	TFourByteUnion	UGain[3];		//电压系数 A B C
	TFourByteUnion	IGain[4];		//电流系数 A B C N
	TFourByteUnion  PGain[4];		//功率系数 T A B C

	DWORD 			CRC32;			//4字节校验
}TSampleCorr;

//精调系数 分段补偿系数
typedef struct TSampleAdjustParaDiv_t
{
	WORD PGainDiv[3][10];			//扩展9段增益系数

	WORD PhsRegDiv[3][10];			//扩展9段角差系数

	DWORD CRC32;
}TSampleAdjustParaDiv;

//手动修正系数
typedef struct TSampleManualModifyPara_t
{
	signed char ManualModifyForWard[9];		//正向有功精度修正，仅仅修正增益部分
	signed char ManualModifyForRA[9];  		//反向有功精度修正，仅仅修正增益部分
	signed char ManualModifyQGain[3][9];	//手动修正无功参数存储区，只对无功增益进行修正
	WORD 		VolUnbalanceGain[3];		//电压不平衡补偿系数
	
	DWORD CRC32;
}TSampleManualModifyPara;

//分段校正滞回区间上下线
typedef struct TCurrenttValue_t
{
	DWORD Upper;
	DWORD Lower;
}TCurrenttValue;



//精调默认增益和大小相角系数
typedef struct TOriginalPara_t
{
	WORD AngCoe[2][MAX_PHASE];					//原始相位系数
	WORD PGainCoe[MAX_PHASE];					//原始有功增益系数

	DWORD CRC32;
}TOriginalPara;

#pragma pack(1)
typedef struct TSampleChipPara_t
{
	BYTE bCount;
	BYTE bRev[7];
	DWORD CRC32;
}TSampleChipPara;
#pragma pack()

//EDT基准参数结构体
typedef struct EDTDataPara_t
{
    DWORD   AMP_STD[4][MAX_PHASE];			//有效值
    DWORD   PHS_STD[4][MAX_PHASE];			//相位
    
	DWORD   CRC32;
}TEDTDataPara;

typedef enum
{
	eVOLTAGE = 0X00,	//电压
	eCURRENT = 0X55,	//电流
	ePOWER	= 0XAA,		//功率
}eUIPType;
typedef enum
{
	eUnuseIntegrator,
	eUseIntegrator,
	eNoneIntegrator,
}eIntegratorMode;
typedef enum
{
	eThreePhase,
	eOnePhase,
	eNonePhase,
}eMeasurementMode;
typedef enum
{
	eCORR = 0,	//比例系数
	eADJUST,	//校准参数
	eCRTL,		//控制参数
	eVOLCORR,	//电压影响量(单相)
    eEDT,       //EDT基准参数
    eOTHER,		//其他
	eALL,		//所有	
}eSampleType;

//平均功率
typedef struct TPAverage_t
{
	BYTE  Cnt;			//计数值
	BYTE  ClearFlag;	//清除标志
	float SumValue;		//累加值
}TPAverage;


#pragma pack(1)
//电压最大最小值
typedef struct TVolMaxOrMin_t
{
	WORD 		VolValue;
	BYTE		Time[6];    //记录发生时间
}TVolMaxOrMin;

//电流最大值
typedef struct TCurrMax_t
{
	BYTE				Time[6];    //记录发生时间
	DWORD 				CurrValue[MAX_PHASE];
}TCurrMax;

//电流最小值
typedef struct TCurrMin_t
{
	DWORD		 		CurrValue;
	BYTE				Time[6];    //记录发生时间
	BYTE 				CurrMinus;	
}TCurrMin;

//极值存储块
typedef struct TExtremumMax_t
{
	TVolMaxOrMin VolMax[MAX_PHASE];		//电压最大值
	TCurrMax CurrMax[MAX_PHASE];		//电流最大值
}TExtremumMax;

typedef struct TExtremumMin_t
{
	TVolMaxOrMin VolMin[MAX_PHASE];		//电压最小值
	TCurrMin CurrMin[MAX_PHASE];		//电流最小值
}TExtremumMin;

typedef struct TContinueXtalPara
{
	DWORD XtalType;
	DWORD CRC32;
}TXtalConRom;
typedef enum 
{
	CmdWriteClose=0x00,
	CmdWriteOpen=0xa6
}eRnAddrCmd;
#pragma pack()
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE SampleNo;
extern TSampleCorr		SampleCorr;
extern TSampleAdjustReg	SampleAdjustReg[SAMPLE_CHIP_NUM];
extern TSampleCtrlReg	SampleCtrlReg;
extern TEDTDataPara     EDTDataPara;
extern DWORD			dwSampleSFlag[SAMPLE_CHIP_NUM];
extern DWORD			EmuAdcRegBuf[2][MAX_PHASE*2*NUM_FFT];
extern TCurrenttValue	CurrentValue[9];
extern TRemoteValue 	RemoteValue;
extern TRemoteValue 	RemoteValueArr[SAMPLE_CHIP_NUM];
extern BYTE 	        bySamplePowerOnTimer;
extern DWORD			dwSmallStandardCurrent;
extern TXtalConRom		XtalPara;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 对RN8302的特殊命令操作
//
//参数:		Cmd[in]	 
//					CH=0xa6,写允许。统一函数。
//					CH=0x00,写禁止          
//返回值: 	无
//		   
//备注: 
//-----------------------------------------------
void SampleSpecCmdOp(WORD Addr,eRnAddrCmd Cmd, BYTE chipNo);
//-----------------------------------------------
//函数功能: 读RN2026寄存器
//
//参数:		Addr[in],	地址	 
//			pBuf[out],	读出数据，4字节（高在后，低在前，第4字节固定清为0）
//			Len    		长度（读取）
//
//返回值: 	TRUE/FALSE
//
//备注:     无    
//-----------------------------------------------
BOOL Link_ReadSampleReg( WORD Addr, BYTE *pBuf ,BYTE bLen, BYTE chipNo);

//-----------------------------------------------
//函数功能: 写RN2025寄存器
//
//参数:		Addr[in]	地址	 
//			pBuf[in]	写入数据 4字节 高在后，低在前
//返回值: 	
//		    TRUE/FALSE
//备注: 
//-----------------------------------------------
BOOL WriteSampleReg(WORD addr, BYTE *pBuf ,BYTE bLen, BYTE chipNo);

//---------------------------------------------------------------
//函数功能: 软复位计量芯片
//
//参数:   无
//
//返回值: 无
//
//备注: 暂时调用的是锐能微库函数
//---------------------------------------------------------------
void SoftResetSample( void );

//---------------------------------------------------------------
//函数功能: 得到电压影响量补偿等级
//
//参数: 	Mode: 
//			 0x00-调校调用 返回当前电压等级
//			 0x01-补偿调用 连续判断两次 防止电压波动
//                   
//返回值:   0：无电压补偿 或者在220V
//         序号---电流---功率因数--——电压等级
//		   1~4 | 5%   |   1.0   | 80%/90%/110%/115%
//         5~6 | 10%  |   0.5L  | 90%/110%
// 		   7~8 | 100% |   0.5L  | 90%/110%
// 		   9~10| Imax |   0.5L  | 90%/110%
//
//备注:   
//---------------------------------------------------------------
BYTE GetVolCorrectLevel( BYTE Mode );

//void api_DMAControl(eState Type);

//-----------------------------------------------
//函数功能: 初始化校表标准源的电压、电流
//
//参数:		无	 
//			
//返回值: 	无
//		    
//备注: 
//-----------------------------------------------
void InitSampleCaliUI( void );

//-----------------------------------------------
//函数功能: 采样寄存器设置为默认数值
//
//参数:		eSampleType Type
//			
//返回值: 	无
//		    
//备注:
//-----------------------------------------------
void UseSampleDefaultPara( eSampleType Type, BYTE chipNo );
//-----------------------------------------------
//函数功能: 效验采样芯片的寄存器
//
//参数:		无
//
//返回值: 	无
//
//备注:  
//-----------------------------------------------
BOOL CheckSampleChipReg(BYTE chipNo);
//--------------------------------------------------
//功能描述:  设置计量芯片工作模式
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_SetSampleRunPara( eIntegratorMode inmode , eMeasurementMode memode, BYTE chipNo );

//-----------------------------------------------
//函数功能: 校表系数保存到EEPROM，并置刷新标志
//
//参数:		无
//          
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void WriteEEpromSamplePara( eSampleType Type, BYTE chipNo );


//-----------------------------------------------
//函数功能: 写某个校正系数到EEPROM
//
//参数:		WriteEepromFlag[in]	bit0 SampleCtrlAddr 0--写ram不写ee	1--写ram写ee
//								bit1 SampleAdjustAddr 0--写ram不写ee	1--写ram写ee
//			Addr[in]	地址	 
//			Buf[in]		写入数据，两字节，高在后，低在前
//返回值: 	
//		    TRUE/FALSE
//备注: 
//-----------------------------------------------
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,DWORD Addr, BYTE *Buf, BYTE chipNo);

#endif//#if( SAMPLE_CHIP == CHIP_RN8302B )
#endif//#ifndef CHIP_RN2026EMU
