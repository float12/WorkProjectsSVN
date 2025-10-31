//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.8
//����		����оƬHT7038 ģ����ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __HT7032L_H
#define __HT7032L_H

#if(SAMPLE_CHIP==CHIP_HT7032L)
//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define SEL_VOL_ANGLE_FUNC	YES
#define SEL_MEASURE_FREQ	YES

//���ߵ���ͨ������ѡ��
#define CUR_ZERO_CURRENT_GAIN	16		//ֻ��ѡ1,2,4,8,16,24,2,4,24
//��ѹֵ ��λ0.01mv 9��169k 1��33k 1��1k
#define	VOL_VALUE		( ((DWORD)(wCaliVoltageConst/10)*1*SimpleVolConst/10) * wSampleVolGainConst )
//����ֵ 
#define	CURRENT_VALUE	((WORD)(wSampleCurGainConst*SimpleCurrConst))

//���ߵ�������оƬADC����ֵ
#define ZERO_CURRENT_VALUE	((float)CUR_ZERO_CURRENT_GAIN * (float)ZeroSampleCurrConst / 1000.0)
#define	UN_IB_EC		((float)(wCaliVoltageConst/10)*((float)wMeterBasicCurrentConst/1000)*(api_GetActiveConstant()))
//����2��3��4��6��12�ܹ����� ���Գ���12�ٳ���12
//HFConst��INT[2.592*10^10*G*G*Vu*Vi/(EC*Un*Ib)] G=1.163 ϵ��Ϊ2.592 * 1.163 * 1.163 = 3.505858848
//HFConst��2.592*Vu*Vi*10^10*1.163*1.163*/(EC*Un*Ib)
//HFConst��9.02*10^11*Vu*Vi/(EC*Un*IB)
//ϵ������35058.58848����VOL_VALUE����100������ϵ������100
#define	HFCONST_DEF		( (DWORD)(((float)902*VOL_VALUE*CURRENT_VALUE/UN_IB_EC+6)/12)*12 )//��֤���Ա�12����

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
#define r_PA		0x01//�й�����
#define r_PB		0x02
#define r_PC		0x03
#define r_PTP		0x04
#define r_PTN		0x05
#define r_QA		0x06//�޹�����
#define r_QB		0x07
#define r_QC		0x08
#define r_QTP		0x09
#define r_QTN		0x0A
#define r_SA		0x0B//���ڹ���
#define r_SB		0x0C
#define r_SC		0x0D
#define r_ST		0x0E

#define r_RMSUA		0x0F//��ѹ��Чֵ
#define r_RMSUB		0x10
#define r_RMSUC		0x11
#define r_RMSIA		0x12//������Чֵ
#define r_RMSIB		0x13
#define r_RMSIC		0x14
#define r_RMSIN		0x15//���ߵ�����Чֵ
#define r_RMSUT		0x16//��ѹʸ������Чֵ
#define r_RMSIT		0x17//����ʸ������Чֵ
#define r_RMSUAB	0x08//�ߵ�ѹ��Чֵ
#define r_RMSUCB	0x19
#define r_RMSUAC	0x1A
#define r_FreqUA	0x1B//Ƶ��
#define r_FreqUB	0x1C
#define r_FreqUC	0x1D
#define r_FreqU		0x1E

#define r_PFA		0x1F//��������
#define r_PFB		0x20
#define r_PFC		0x21
#define r_PFT		0x22
#define r_PAUA		0x23//A ���ѹ�Ƕ�
#define r_PAUB		0x24
#define r_PAUC		0x25
#define r_PAIA		0x26
#define r_PAIB		0x27
#define r_PAIC		0x28
#define r_PAIN		0x29

#define r_EPPA		0x2A//A��ȫ�������й����ܣ�������Ϊ�������㣩
#define r_EPPB		0x2B
#define r_EPPC		0x2C
#define r_EPPT		0x2D
#define r_EPQA		0x2E
#define r_EPQB		0x2F
#define r_EPQC		0x30
#define r_EPQT		0x31
#define r_ESA		0x32//���ڵ���
#define r_ESB		0x33
#define r_ESC		0x34
#define r_EST		0x35

#define r_ENPA		0x36//A��ȫ�������й�����
#define r_ENPB		0x37
#define r_ENPC		0x38
#define r_ENPT		0x39
#define r_ENQA		0x3A
#define r_ENQB		0x3B
#define r_ENQC		0x3C
#define r_ENQT		0x3D
#define r_ENSA		0x3E//���ڵ���(�������)
#define r_ENSB		0x3F
#define r_ENSC		0x40
#define r_ENST		0x41

#define r_FPA		0x42//�����й�����
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
#define r_FRMSUA	0x50//������ѹ��Чֵ
#define r_FRMSUB	0x51
#define r_FRMSUC	0x52
#define r_FRMSIA	0x53//����������Чֵ
#define r_FRMSIB	0x54
#define r_FRMSIC	0x55
#define r_FRMSIN	0x56//�������ߵ�����Чֵ
#define r_FPFA		0x57//��������
#define r_FPFB		0x58
#define r_FPFC		0x59
#define r_FPFT		0x5A

#define r_FEPPA		0x5B//A����������й����ܣ�������Ϊ�������㣩
#define r_FEPPB		0x5C
#define r_FEPPC		0x5D
#define r_FEPPT		0x5E
#define r_FENPA		0x5F//A����������й����ܣ�������Ϊ�������㣩
#define r_FENPB		0x60
#define r_FENPC		0x61
#define r_FENPT		0x62

#define r_SPLUA		0x80//A ���ѹͨ�� ADC ��������
#define r_SPLUB		0x81
#define r_SPLUC		0x82
#define r_SPLIA		0x83//A �����ͨ�� ADC ��������
#define r_SPLIB		0x84
#define r_SPLIC		0x85
#define r_SPLIN		0x86

#define r_TPSD		0xD1//�¶�

#define r_BckReg	0xD2//ͨѶ���ݼĴ���

#define r_ComChkSum	0xD3//ͨѶУ��ͼĴ���

#define r_ChkSum0	0xD4//ͨѶУ��ͼĴ���
#define r_ChkSum1	0xD5//ͨѶУ��ͼĴ���
#define r_ChkSum2	0xD6//ͨѶУ��ͼĴ���

#define r_Noload		0x100//Ǳ��״̬�Ĵ���
#define r_REV_Power		0x101//���ʷ���Ĵ���
#define r_REV_Energy 	0x102//���ܷ���Ĵ�����������ͨ·�ķ����־��
#define r_P90_Power 	0x103//��ǰǰ�й�/�޹��Ĺ�������̫С
#define r_SignalStatus 	0x104//�ź�״̬�Ĵ������ǶȻ�׼����λ�ֶ����䡢�����־��ʧѹ��־��
#define r_PSIIStatus	0x105//Peak / Sag / Int / Iov �������¼���״̬ Status
#define r_RST_STA		0x106//��λ״̬��־

#define	r_Buffer0 			0x180		//Buffer0 �������ݼĴ������ڲ��������ظ���ȡ��ȡ�ڲ�����.
#define	r_UnreadNum0 		0x181		//Buffer0 δ�����ݵ������Ĵ���
#define	r_Buffer1 			0x182		//Buffer1 �������ݼĴ������ڲ��������ظ���ȡ��ȡ�ڲ�����
#define	r_UnreadNum1 		0x183		//Buffer1 δ�����ݵ������Ĵ���
#define	r_Buffer2 			0x184		//Buffer2 �������ݼĴ������ڲ��������ظ���ȡ��ȡ�ڲ�����
#define	r_UnreadNum2 		0x185		//Buffer2 δ�����ݵ������Ĵ���
#define	r_Buffer3 			0x186		//Buffer3 �������ݼĴ������ڲ��������ظ���ȡ��ȡ�ڲ�����
#define	r_UnreadNum3 		0x187		//Buffer3 δ�����ݵ������Ĵ���
#define	r_Buffer4 			0x188		//Buffer4 �������ݼĴ������ڲ��������ظ���ȡ��ȡ�ڲ�����
#define	r_UnreadNum4 		0x189		//Buffer4 δ�����ݵ������Ĵ���
#define	r_ComBuffer 		0x18A		//ComBuffer �������ݼĴ������ڲ��������ظ���ȡ��ȡ�ڲ�����
#define	r_ComB_UnreadNum	0x18B		//ComBuffer δ�����ݵ������Ĵ���


//0x00 ����
#define w_GPA	0x300	//A ��ȫ���й���������
#define w_GPB	0x301
#define w_GPC	0x302
#define w_GQA	0x303
#define w_GQB	0x304
#define w_GQC	0x305
#define w_GSA	0x306
#define w_GSB	0x307
#define w_GSC	0x308
#define w_GphsA0	0x309	//A ��ȫ����λУ�� 0
#define w_GphsA1	0x30A
#define w_GphsA2	0x30B
#define w_GphsB0	0x30C
#define w_GphsB1	0x30D
#define w_GphsB2	0x30E
#define w_GphsC0	0x30F
#define w_GphsC1	0x310
#define w_GphsC2	0x311

#define w_POffsetPA		0x313//A ��ȫ���й�����ƫ��У��
#define w_POffsetPB		0x314
#define w_POffsetPC		0x315
#define w_POffsetQA		0x316
#define w_POffsetQB		0x317
#define w_POffsetQC		0x318
#define w_RmsOffsetUA	0x319//Ua ͨ��ȫ����ѹ��Чֵƫ��У��
#define w_RmsOffsetUB	0x31A
#define w_RmsOffsetUC	0x31B
#define w_RmsOffsetIA	0x31C
#define w_RmsOffsetIB	0x31D
#define w_RmsOffsetIC	0x31E
#define w_RmsOffsetIN	0x31F
#define w_RmsOffsetUT	0x320//�����ѹ��Чֵƫ��У��
#define w_RmsOffsetIT	0x321//���������Чֵƫ��У��

#define w_FGPA		0x330	//A ������й���������
#define w_FGPB		0x331
#define w_FGPC		0x332
#define w_FGQA		0x333
#define w_FGQB		0x334
#define w_FGQC		0x335
#define w_FGSA		0x336
#define w_FGSB		0x337
#define w_FGSC		0x338
#define w_FGphsA0	0x339	//A �������λУ�� 0 
#define w_FGphsA1	0x33A
#define w_FGphsA2	0x33B
#define w_FGphsB0	0x33C
#define w_FGphsB1	0x33D
#define w_FGphsB2	0x33E
#define w_FGphsC0	0x33F
#define w_FGphsC1	0x340
#define w_FGphsC2	0x341

#define w_FPOffsetPA	0x343	//A ������й�����ƫ��У��
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

#define w_HGPA			0x350	//A ��г���й���������
#define w_HGPB			0x351
#define w_HGPC			0x352
#define w_HPOffsetPA	0x353//A ��г���й�����ƫ��У��
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

#define w_TGainU		0x364	//U ����ͨ�����¶�����
#define w_TGainI		0x365	//I ����ͨ�����¶�����
#define w_TGainIN		0x366	//In ����ͨ�����¶�����

#define w_GainUA		0x367	//Ua ����ͨ������
#define w_GainUB		0x368
#define w_GainUC		0x369
#define w_GainIA		0x36a
#define w_GainIB		0x36b
#define w_GainIC		0x36c
#define w_GainIN		0x36d
#define w_DigtalGain	0x36e	//��������
#define w_DecShiftUaH	0x36f	//Ua ͨ���ƶ� Onebit ��λУ������
#define w_DecShiftUbH	0x370
#define w_DecShiftUcH	0x371
#define w_DecShiftUaL	0x372
#define w_DecShiftUbL	0x373
#define w_DecShiftUcL	0x374
#define w_DecShiftIaH	0x375	//Ua ͨ���ƶ� Onebit ��λУ������
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

#define w_CFCntPosPa	0x3A0//A ��ȫ�������й������������
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
#define w_CFCntNegPa	0x3AC//A ��ȫ�������й������������
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

#define w_FCFCntPosPa	0x3B8//A ��ȫ�������й������������
#define w_FCFCntPosPb	0x3B9
#define w_FCFCntPosPc	0x3BA
#define w_FCFCntPosPt	0x3BB
#define w_FCFCntPosQa	0x3BC
#define w_FCFCntPosQb	0x3BD
#define w_FCFCntPosQc	0x3BE
#define w_FCFCntPosQt	0x3BF
#define w_FCFCntNegPa	0x3C0//A ��ȫ�������й������������
#define w_FCFCntNegPb	0x3C1
#define w_FCFCntNegPc	0x3C2
#define w_FCFCntNegPt	0x3C3
#define w_FCFCntNegQa	0x3C4
#define w_FCFCntNegQb	0x3C5
#define w_FCFCntNegQc	0x3C6
#define w_FCFCntNegQt	0x3C7

#define w_UconstA		0x3D0//A�ೣ����ѹͨ��(��� All_RMS_Ux ȥ����������ڹ��ʲ��ۼӵ���(ֻ��ȫ��))
#define w_UconstB		0x3D1//B�ೣ����ѹͨ��
#define w_UconstC		0x3D2//C�ೣ����ѹͨ��

#define w_ZXLostCFG		0x3D3//��������¼�����
#define w_PStart		0x3D4//�й��𶯹������üĴ���
#define w_QStart		0x3D5//�޹��𶯹������üĴ���
#define w_HarStart		0x3D6//г���𶯹������üĴ���

#define w_AVGEnergy		0x3D7//˲ʱ���ʾ�����ƽ��ģ���õ�ƽ������
#define w_AVGEnergyREG	0x3D8
#define w_AVGRMS		0x3D9
#define w_AVGRMST		0x3DA

#define w_HFConstAF		0x3DB//
#define w_HFConstHar	0x3DC

#define w_AFDIV			0x3DD//ȫ��/������������ֱ���
#define w_HarDIV		0x3DE//г����������ֱ���
#define w_ChannelSel	0x3DF//����ͨ�������뷴��Ĵ���
#define w_PGACON		0x3E0//7 ·����ͨ����ģ�� PGA ����
#define w_ADCCFG		0x3E1//ϵͳʱ��,Դͷ�� ADC ����
#define w_ADCCON		0x3E2//ADC ���أ��¼��Ĺ��ܿ��ص�
#define w_EMUCFG		0x3E3//�����������ѡ��
#define w_FilterCon		0x3E4//Rosi�볣����ѹ�Ĵ���
#define w_FilterCFG		0x3E5//�����˲���ϵ������
#define w_StartCFG		0x3E6//��Ǳ������
#define w_MULCFG		0x3E7//����г���������üĴ���
#define w_GMULFund		0x3E8//���� MULGain��"ȫ-��"����г��ʱ�����ڲ��������˲�����˥��
#define w_ANACFG1		0x3E9//ģ������
#define w_ANACFG2		0x3EA//ģ������
#define w_ANACFG3		0x3EB//ģ������
#define w_ADCTMLOAD		0x3EC//ADC �������ģʽ������

#define w_TPSCTL		0x3F5//TPS ��������
#define w_TPSCal		0x3F6//TPS У���Ĵ���
#define w_FailVoltage	0x3F7//ʧѹ��ֵ���� All_RMSU �� Bit[23��8]����
#define w_CFxRun		0x3F8//����������أ�����ͨ·�������ۼӶ���ʹ��
#define w_EnergyCon		0x3F9//�������ƣ�������ܵĹ���ѡ��
#define w_PowerSrcCFG	0x3FA//����Դ���ã�ѡ�����ڵ����ۼӵĹ���
#define w_PowerModeCFG	0x3FB//����ѡ��ģʽ�Ĵ����������ۼ�ģ���빦�ʷּ�����ģʽ
#define w_CFCFG			0x3FC//��������Ĵ���
#define w_PowerPN_THO	0x3FD//������������ֵ�Ĵ���

#define w_UZCLVL		0x40B//��ѹͨ��������ֵ
#define w_IZCLVL		0x40C//����ͨ��������ֵ

#define w_TUCCOFFA		0x40d//�Զ��²�ϵ�� U
#define w_TUCCOFFB		0x40e//�Զ��²�ϵ�� U
#define w_TUCCOFFC		0x40f//�Զ��²�ϵ�� U
#define w_TICCOFFA		0x410//�Զ��²�ϵ�� I
#define w_TICCOFFB		0x411//�Զ��²�ϵ�� I
#define w_TICCOFFC		0x412//�Զ��²�ϵ�� I
#define w_TInCCOFFA		0x413//�Զ��²�ϵ�� I
#define w_TInCCOFFB		0x414//�Զ��²�ϵ�� I
#define w_TInCCOFFC		0x415//�Զ��²�ϵ�� I

#define w_IRegion0		0x416	//��λ�ֶ�У��������ֵ0
#define w_IRegion1		0x417	//��λ�ֶ�У��������ֵ1
#define w_IRegion0Hys	0x418	//��λ�ֶ�У��������ֵ0�ͻ�����
#define w_IRegion1Hys	0x419	//��λ�ֶ�У��������ֵ1�ͻ�����
#define w_FlickerCfg	0x41a	//Flicker ���üĴ���

#define w_DECReset		0x421	//DEC �˲�������Ĵ���

#define w_EMUWPREG		0x422	//д�����Ĵ���
#define w_SRSTREG		0x423	//�����λ�Ĵ���

#define w_SPICFG		0x424	//SPI ���üĴ���
#define w_HSDCCFG		0x425	//HSDC ���üĴ���
#define w_FrameCFG		0x426	//HSDC ��֡�Ĵ���

#define w_EMUIE1		0x480	//EMU �ж�ʹ�� 1
#define w_EMUIE2		0x481	//EMU �ж�ʹ�� 2
#define w_EMUIE3		0x482	//EMU �ж�ʹ�� 3
#define w_EMUIF1		0x483	//EMU �жϱ�־ 1
#define w_EMUIF2		0x484	//EMU �жϱ�־ 2
#define w_EMUIF3		0x485	//EMU �жϱ�־ 3
#define	w_EMUIF_EnergyPOVIF		0x486	//EMU����ж�(R/W)							x
#define	w_EMUIF_EnergyNOVIF		0x487	//EMU����ж�(R/W)							x
#define w_PLLCTRL		0x500	//PLL ���üĴ���

#define	w_PLL_Gain				0x501	//�û���Ҫ����Ĭ��(�Ƽ�)ֵ
#define	w_PLL_ACC				0x503	//�û���Ҫ����Ĭ��(�Ƽ�)ֵ
#define	w_SFO_EST				0x504	//�û���Ҫ����Ĭ��(�Ƽ�)ֵ
#define	w_LOCK_THO				0x505	//�û���Ҫ����Ĭ��(�Ƽ�)ֵ
#define	w_AMP_THO				0x506	//�û���Ҫ����Ĭ��(�Ƽ�)ֵ
#define	w_TED_RSA				0x507	//�û���Ҫ����Ĭ��(�Ƽ�)ֵ
#define	w_LTV_CFG				0x540	//LVTģ�����üĴ���
#define	w_SFO_Value				0x541	//SFO����
#define	w_DFT_CFG				0x543	//Ux,Ix�����ƣ��û���Ҫ����Ĭ��(�Ƽ�)ֵ
#define	w_HpRMS_Ctl_U			0x545	//�벨��Чֵ�Ĺ����У��
#define	w_HpRMS_Ctl_I			0x546	//�벨��Чֵ�Ĺ����У��
#define	w_DataChnCFG			0x580	//����ͨ��ѡ��Ĵ���
#define	w_Buffer0CFG			0x581	//Buffer0���üĴ���
#define	w_Buffer0_AddrLen		0x582	//Buffer0��ʼ��ַ�ͳ������üĴ���
#define	w_UnRLVL_NullNum0		0x583	//Buffer0δ��������ֵ�뻺��Ԥ���������üĴ���
#define	w_Buffer1CFG			0x584	//Buffer1���üĴ���
#define	w_Buffer1_AddrLen		0x585	//Buffer1��ʼ��ַ�ͳ������üĴ���
#define	w_UnRLVL_NullNum1		0x586	//Buffer1δ��������ֵ�뻺��Ԥ���������üĴ���
#define	w_Buffer2CFG			0x587	//Buffer2���üĴ���
#define	w_Buffer2_AddrLen		0x588	//Buffer2��ʼ��ַ�ͳ������üĴ���
#define	w_UnRLVL_NullNum2		0x589	//Buffer2δ��������ֵ�뻺��Ԥ���������üĴ���
#define	w_Buffer3CFG			0x58A	//Buffer3���üĴ���
#define	w_Buffer3_AddrLen		0x58B	//Buffer3��ʼ��ַ�ͳ������üĴ���
#define	w_UnRLVL_NullNum3		0x58C	//Buffer3δ��������ֵ�뻺��Ԥ���������üĴ���
#define	w_Buffer4CFG			0x58D	//Buffer4���üĴ���
#define	w_Buffer4_AddrLen		0x58E	//Buffer4��ʼ��ַ�ͳ������üĴ���
#define	w_UnRLVL_NullNum4		0x58F	//Buffer4δ��������ֵ�뻺��Ԥ���������üĴ���
#define	w_ComBuff_CFG			0x590	//ͨ��Buffer���üĴ���
#define	w_ComBuffer_AddrLen		0x591	//ͨ��Buffer��ʼ��ַ�ͳ������üĴ���
#define	w_UnRLVL_NullNumCom		0x592	//ͨ��Bufferδ��������ֵ�뻺��Ԥ���������üĴ���



#define SampleCtrlRegNum  25


//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//оƬ���Ʋ���
typedef struct TSampleCtrlPara_t
{
	DWORD CtrlPara[SampleCtrlRegNum];
	DWORD CRC32;//4�ֽ�У��

}TSampleCtrlPara;


typedef struct TSampleAdjustPara_t
{
	DWORD PGain[3];//�й���������ϵ��

	DWORD QGain[3];

	DWORD SGain[3];

	DWORD PhsReg[3][3];//�Ƕ�����ϵ��

	DWORD UGain[3];//��ѹ����ϵ��

	DWORD IGain[3];//��������ϵ��
	
	DWORD NGain;	//���ߵ�������ϵ��

	DWORD CRC32;//4�ֽ�У��

}TSampleAdjustPara;

typedef struct TSampleOffsetPara_t
{
	DWORD Offset[15];//ƫ��
	
	DWORD CRC32;//4�ֽ�У��

}TSampleOffsetPara;

//����ϵ�� �ֶβ���ϵ��
typedef struct TSampleAdjustParaDiv_t
{
	DWORD PGainDiv[3][10];					//��չ9������ϵ��

	DWORD PhsRegDiv[3][10];					//��չ9�νǲ�ϵ��
	
	DWORD CRC32;
}TSampleAdjustParaDiv;

//�ֶ�����ϵ��
typedef struct TSampleManualModifyPara_t
{
	BYTE		ManualModifyChipToffset;	//����оƬ��TOffset�²���ֵ������25���²����Ĵ���0x2A������ֵ
	signed char ManualModifyForWard[9];		//�����й����������������������沿��
	signed char ManualModifyForRA[9];  		//�����й����������������������沿��
	signed char ManualModifyQGain[3][9];	//�ֶ������޹������洢����ֻ���޹������������
	DWORD 		VolUnbalanceGain[3];		//��ѹ��ƽ�ⲹ��ϵ��
	DWORD CRC32;
}TSampleManualModifyPara;
//ֱ��ż��г��ϵ��
typedef struct TSampleHalfWavePara_t
{
	WORD		Coe[3][2];					//0.5Lֱ��ż��г�� [][0]У׼ֵ [][1]����ֵ
	WORD		Gain[3][2];					//1.0Lֱ��ż��г�� [][0]У׼ֵ [][1]����ֵ
	DWORD		CRC32;
}TSampleHalfWavePara;

//ֱ��ż��г�����ʱ��
typedef struct TSampleHalfWaveTimePara_t
{
	BYTE		Hours;						//Сʱ
	DWORD		CRC32;
}TSampleHalfWaveTimePara;

//Զ������
typedef struct TRemoteValue_t
{
	TFourByteUnion U[3];				//�޷���ԭ���ʽ
	TFourByteUnion I[5];				//�����ʽ	��λA ����3λС�� A B C N1 N2 
	TFourByteUnion P[5];				//0������ֵ//�����ʽ	
	TFourByteUnion Q[5];				//�����ʽ		
	TFourByteUnion S[4];				//���ڹ���//�޷���ԭ���ʽ
	TFourByteUnion Cos[4];				//�����ʽ	
	
	#if( SEL_VOL_ANGLE_FUNC == YES )
	TFourByteUnion YUaUbUcAngle[3];		//�����ʽ	
	TFourByteUnion YUIAngle[3];			//�����ʽ
	#endif
	
	TFourByteUnion Freq;				//�޷���ԭ���ʽ
	TFourByteUnion PQSAverage[3][4];	//һ����ƽ������PQS
	
	TTwoByteUnion Uunbalance;			//��ѹ��ƽ����
	TTwoByteUnion Iunbalance;			//������ƽ����	

	//�¶�
	short Temperature;	
	
}TRemoteValue;

//�ֶ�У���ͻ�����������
typedef struct TCurrenttValue_t
{
	DWORD Upper;
	DWORD Lower;
}TCurrenttValue;

//����Ĭ������ʹ�С���ϵ��
typedef struct TOriginalPara_t
{
	DWORD AngCoe[3][MAX_PHASE];						//ԭʼ��λϵ��
	DWORD PGainCoe[MAX_PHASE];						//ԭʼ�й�����ϵ��
	
	DWORD CRC32;

}TOriginalPara;

//ֱ��ż��г������
typedef enum
{
	eAD_HALFWAVE_NO = 0,	//���ǰ벨
	eAD_HALFWAVE_YES,		//�ǰ벨
}eGetHalfWaveStatus;

//-----------------------------------------------
//				��������
//-----------------------------------------------

extern TRemoteValue 			RemoteValue;				//Զ������
extern TSampleCtrlPara			SampleCtrlPara;
extern TSampleAdjustPara		SampleAdjustPara;
extern TSampleOffsetPara		SampleOffsetPara;
extern TSampleAdjustParaDiv 	SampleAdjustParaDiv;		//�ֶβ�������
extern TSampleManualModifyPara	SampleManualModifyPara;		//�ֶ�����ϵ��
extern TOriginalPara			OriginalPara;	
extern TCurrenttValue			CurrentValue[9];
extern BYTE 	bySamplePowerOnTimer;
extern DWORD	AdjustDivCurrent[10];		//�ֶ�У�������׼ֵ
extern DWORD		dwBigStandardCurrent;		//�����У����Ӧ����
extern DWORD		dwSmallStandardCurrent;		//С���У����Ӧ����
extern BYTE 	g_ucNowSampleDivisionNo[MAX_PHASE];
extern BOOL 	UnBalanceGainFlag[3];			//��ѹ��ƽ�ⲹ����־ TRUE-�Ѳ��� FALSE-δ����
//-----------------------------------------------
// 				��������
//-----------------------------------------------
//����������
DWORD ReadSampleData( WORD Addr );
////��У������
//DWORD ReadCalibrateData( WORD Addr );
BOOL WriteSampleReg( WORD Addr, DWORD Data );
BOOL Link_ReadSampleRegCont(WORD Addr, WORD Num, DWORD *pBuf);
BOOL ReadSampleBuffCont(WORD Addr, WORD Num, SWORD *pBuf);


//��ȡ����оƬ�ļĴ���Ч���
void GetSampleChipCheckSum(void);
void UseSampleDefaultPara(WORD Type);
BOOL WriteSampleChipOffsetPara(void);
BOOL WriteSampleChipCtrlPara(void);
BOOL WriteSampleChipAdjustPara(void);
void InitSampleCaliUI( void );
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,WORD Addr, BYTE *Buf);
//7038У��Ĵ���д��������
void SampleSpecCmdOp(BYTE Cmd);
BYTE GetRowSampleSegmentNo( BYTE ucPhase );
BYTE GetRowSampleSegmentAdjustNo( BYTE ucPhase );
BOOL JudgeUnBalance( BYTE Phase );
void FreshCurrent( void );


#endif//#if( (SAMPLE_CHIP==CHIP_HT7038) || (SAMPLE_CHIP==CHIP_HT7026) )


#endif//#ifndef __HT7032_H

