//----------------------------------------------------------------------
//Copyright (C) 2016-2023 ��̨������˼�ٵ����ɷ����޹�˾��ѹ̨����Ʒ��
//������	   
//��������	2023.05.29
//����		����оƬRN8302B�ڲ�ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __RN8302B_H
#define __RN8302B_H
#if( SAMPLE_CHIP == CHIP_RN8302B )
//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define SEL_VOL_ANGLE_FUNC		YES
#define SEL_MEASURE_FREQ		YES

//-----------------------��ȡ��˲ʱֵ-----------------------
//����
#define	IA_EMU			0x000B		//IA��Чֵ�Ĵ���
#define	IB_EMU			0x000C		//IB��Чֵ�Ĵ���
#define	IC_EMU			0x000D		//IC��Чֵ�Ĵ���
#define	IN_EMU			0x000E		//IN��Чֵ�Ĵ���
#define	IN2_EMU			0x0010		//IN2��Чֵ�Ĵ���(����)

//��ѹ
#define	UA_EMU			0x0007		//UA��Чֵ�Ĵ���
#define	UB_EMU			0x0008		//UB��Чֵ�Ĵ���
#define	UC_EMU			0x0009		//UC��Чֵ�Ĵ���
#define	UT_EMU			0x000A		//��ѹʸ������Чֵ�Ĵ���

//�й�����
#define	PA_EMU			0x0014		//A·�й�����
#define	PB_EMU			0x0015		//B·�й�����
#define	PC_EMU			0x0016		//C·�й�����
#define	PT_EMU			0x0017		//�����й�����
//�޹�����
#define	QA_EMU			0x0018		//A·�޹�����
#define	QB_EMU			0x0019		//B·�޹�����
#define	QC_EMU			0x001A		//C·�޹�����
#define	QT_EMU			0x001B		//�����޹�����
//���ڹ���
#define	SA_EMU			0x001C		//A·���ڹ�����
#define	SB_EMU			0x001D		//B·���ڹ�����
#define	SC_EMU			0x001E		//C·���ڹ�����
#define	STA_EMU			0x001F		//RMS�������ڹ���
#define	STV_EMU			0x004C		//PQS�������ڹ���
//��������
#define	PfA_EMU			0x0020		//A�๦������
#define	PfB_EMU			0x0021		//B�๦������
#define	PfC_EMU			0x0022		//C�๦������
#define	PfTA_EMU		0x0023		//RMS���๦������
#define	PfTV_EMU		0x004D		//PQS���๦������
//�й�
#define	EPA_EMU			0x0030		//A·�й������Ĵ���
#define	EPB_EMU			0x0031		//B·�й������Ĵ���
#define	EPC_EMU			0x0032		//C·�й������Ĵ���
#define	EPT_EMU			0x0033		//�����й������Ĵ���
//�����й�
#define	PEPA_EMU		0x0034		//A·�����й������Ĵ���
#define	PEPB_EMU		0x0035		//B·�����й������Ĵ���
#define	PEPC_EMU		0x0036		//C·�����й������Ĵ���
#define	PEPT_EMU		0x0037		//���������й������Ĵ���
//�����й�
#define	NEPA_EMU		0x0038			//A·�����й������Ĵ���
#define	NEPB_EMU		0x0039			//B·�����й������Ĵ���
#define	NEPC_EMU		0x003A			//C·�����й������Ĵ���
#define	NEPT_EMU		0x003B			//���෴���й������Ĵ���
//�޹�
#define	EQA_EMU			0x003C			//A·�޹������Ĵ���
#define	EQB_EMU			0x003D			//B·�޹������Ĵ���
#define	EQC_EMU			0x003E			//C·�޹������Ĵ���
#define	EQT_EMU			0x003F			//�����޹������Ĵ���
//�����޹�
#define	PEQA_EMU		0x0040			//A·�����޹������Ĵ���
#define	PEQB_EMU		0x0041			//B·�����޹������Ĵ���
#define	PEQC_EMU		0x0042			//C·�����޹������Ĵ���
#define	PEQT_EMU		0x0043			//���������޹������Ĵ���
//�����޹�
#define	NEQA_EMU		0x0044			//A·�����޹������Ĵ���
#define	NEQB_EMU		0x0045			//B·�����޹������Ĵ���
#define	NEQC_EMU		0x0046			//C·�����޹������Ĵ���
#define	NEQT_EMU		0x0047			//���෴���޹������Ĵ���
//����
#define	ESA_EMU			0x0048			//A·���������Ĵ���
#define	ESB_EMU			0x0049			//B·���������Ĵ���
#define	ESC_EMU			0x004A			//C·���������Ĵ���
#define	ESTA_EMU		0x004B			//RMS�������������Ĵ���
#define	ESTV_EMU		0x004F			//PQS�������������Ĵ���

//����
#define	Freq_U_EMU		0x0057			//��ѹ��Ƶ�ʼĴ���
#define	Status_EMU		0x0167			//Ǳ��״̬��־�Ĵ���
#define	PQSign_EMU		0x0166			//���ʷ���Ĵ���

//-----------------------У�����-----------------------
//100%Ib��1.0L
#define	GPA_EMU				0x0128		//A���й���������Ĵ���
#define	GPB_EMU				0x0129		//B���й���������Ĵ���
#define	GPC_EMU				0x012A		//C���й���������Ĵ���
#define	GQA_EMU				0x012B		//A���޹���������Ĵ���
#define	GQB_EMU				0x012C		//B���޹���������Ĵ���
#define	GQC_EMU				0x012D		//C���޹���������Ĵ���
#define	GSA_EMU				0x012E		//A�����ڹ�������Ĵ���
#define	GSB_EMU				0x012F		//B�����ڹ�������Ĵ���
#define	GSC_EMU				0x0130		//C�����ڹ�������Ĵ���
#define GSUA_EMU			0x0113 		//UAͨ������Ĵ���
#define GSUB_EMU			0x0114 		//UBͨ������Ĵ���
#define GSUC_EMU			0x0115 		//UCͨ������Ĵ���
#define GSIA_EMU			0x0116 		//IAͨ������Ĵ���
#define GSIB_EMU			0x0117 		//IBͨ������Ĵ���
#define GSIC_EMU			0x0118 		//ICͨ������Ĵ���
#define GSIN_EMU			0x0119 		//INͨ������Ĵ���
//100%Ib,0.5L
#define	PAPHSL_EMU			0x0131	//A�й�������λУ����
#define	PBPHSL_EMU			0x0132	//B�й�������λУ����
#define	PCPHSL_EMU			0x0133	//C�й�������λУ����
#define	QAPHSL_EMU			0x0134	//A�޹�������λУ����
#define	QBPHSL_EMU			0x0135	//B�޹�������λУ����
#define	QCPHSL_EMU			0x0136	//C�޹�������λУ����
#define	PHSIA_EMU			0x010F		//IAͨ����λУ���Ĵ���				0x808080
#define	PHSIB_EMU			0x0110		//IBͨ����λУ���Ĵ���				0x808080
#define	PHSIC_EMU			0x0111		//ICͨ����λУ���Ĵ���				0x808080
#define	PHSIN_EMU			0x0112		//INͨ����λУ���Ĵ���				0x80
//5%Ib,0.5L
//����
#define	HFConst_EMU			0X0100	//��Ƶ���峣���Ĵ���
#define	PStart_EMU			0x0102		//�й�����������ֵ�Ĵ��� 
#define	QStart_EMU			0x0103		//�޹�����������ֵ�Ĵ���
#define	ZXOT_EMU			0x0105		//��ѹ������ֵ�Ĵ���
#define	ZXOTU_EMU			0x0190		//��ѹ������ֵ�Ĵ���
#define	LostV_EMU			0x0104		//ʧѹ��ֵ�Ĵ���
//-----------------------�����������ַ-----------------------
#define	CFCFG_EMU			0x0160		//CF�������üĴ��� 
#define	EMUCFG_EMU			0x0161		//������Ԫ���üĴ��� 	
#define	EMUCFG3_EMU			0x016D		//������Ԫ���üĴ���3 	
#define	EMUCON_EMU			0x0162		//�������ƼĴ���
#define	EMMIF_EMU			0x0165		//EMU�жϱ�־�Ĵ���
#define	IRegionS_EMU		0x0168		//��������״̬�Ĵ���
#define	PHASES_EMU			0x0169		//���ѹ����״̬�Ĵ���
#define ZXOTCFG_EMU			0x0192	 	//����������ü���־�Ĵ���
#define	AUOTDC_EN_EMU		0x0191		//ֱ��OFFSET�Զ�У��ʹ�ܼĴ��� 
#define ADCCTL_SYS			0x0183		//ADC���ƼĴ���
#define	MODSEL_EMU			0x0186		//��������/��������ģʽѡ��Ĵ���

#define SYSCFG_EMU			0x0178		//ϵͳ���üĴ��� 
#define HSDCCTL_EMU			0x01C9		//HSDC�ӿ����üĴ���
#define EMMIE2_EMU			0x016B		//EMU�ж�2ʹ�ܼĴ���
#define WSAVECON_EMU		0x0163		//��������д������ƼĴ���
#define DMA_WAVECON_EMU		0x0193		//ͬ������ͨ�����üĴ���	
#define DMA_WAVECON3_EMU	0x01E5		//ͬ������ͨ�����üĴ���3
#define DMA_WAVECON2_EMU	0x01D1		//ͬ������ͨ�����üĴ���

#define	CKSUM_EMU1			0x016A		//У��ͼĴ��� 3�ֽ�
#define	CKSUM_EMU2			0x018B		//У��ͼĴ��� 2�ֽ�
#define	CKSUM_EMU3			0x01AE		//У��ͼĴ��� 4�ֽ�
#define	CKSUM_EMU4			0x01EF		//У��ͼĴ��� 4�ֽ�
#define	WMSW_EMU			0x0181		//ģʽ�л��Ĵ�����ַ
#define SOFTRST_EMU			0x0182		//�����λ�Ĵ���
#define	VERRID_EMU			0x01F1		//8302B�汾

#define SPEL_WREN_EMU 		0x01A1 /*" RN830x ����дʹ�ܼĴ�����ַ "*/
#define BGR_HT_EMU 			0x01F3 /*" ����ģʽдʹ�ܼĴ��� "*/
#define OTPTRIM_STA_EMU 	0x01F2 /*" ����ģʽ״ָ̬ʾ�Ĵ��� "*/
#define OTPDBG_EMU 			0x01F0 /*" ����ģʽ���ƼĴ��� "*/
#define	TR1M_EMU			0x01F1		//TR1M�²�ϵ��
#define ROS_CFG				0x01ED //������Ȧ���üĴ���
#define ROS_DCATTC			0x01EB //������Ȧ����ֱ��˥��ϵ���Ĵ���
#define SYSCFG				0x0178 //ϵͳ���üĴ���
#define SYSSR				0x018A //ϵͳ״̬�Ĵ���
#define ECT_WREN 			0x00d0 //����²����üĴ���дʹ��
#define SAR_CTL0 			0x00D1 //SAR ���ƼĴ���0
#define SAR_CTL1 			0x00D2 //SAR ���ƼĴ���1
#define SAR_EN	 			0x00F8 //SAR ADC ʹ�ܼĴ���
/******************************δʹ�õļĴ���***************************************/
//-----------------------ֻ���Ĵ���-----------------------
#define	UAWAV_EMU			0x0000		//UA���β����Ĵ���
#define	UBWAV_EMU			0x0001		//UB���β����Ĵ���
#define	UCWAV_EMU			0x0002		//UC���β����Ĵ���
#define	IAWAV_EMU			0x0003		//IA���β����Ĵ���		
#define	IBWAV_EMU			0x0004		//IB���β����Ĵ���
#define	ICWAV_EMU			0x0005		//IC���β����Ĵ���
#define	INWAV_EMU			0x0006		//IN���β����Ĵ���
#define	PAFCnt_EMU			0x0024		//A�й���������Ĵ���
#define	PBFCnt_EMU			0x0025		//B�й���������Ĵ���
#define	PCFCnt_EMU			0x0026		//C�й���������Ĵ���
#define	PTFCnt_EMU			0x0027		//�����й���������Ĵ���
#define	QAFCnt_EMU			0x0028		//A�޹���������Ĵ���
#define	QBFCnt_EMU			0x0029		//B�޹���������Ĵ���
#define	QCFCnt_EMU			0x002A		//C�޹���������Ĵ���
#define	QTFCnt_EMU			0x002B		//�����޹���������Ĵ���
#define	SAFCnt_EMU			0x002C		//A������������Ĵ���
#define	SBFCnt_EMU			0x002D		//B������������Ĵ���
#define	SCFCnt_EMU			0x002E		//C������������Ĵ���
#define	STFACnt_EMU			0x002F		//����������������Ĵ���
#define	STFVCnt_EMU			0x004E		//����������������Ĵ���
#define YUA_EMU				0x0050		//����ͨ��UA������ǼĴ���
#define YUB_EMU				0x0051		//����ͨ��UB������ǼĴ���
#define YUC_EMU				0x0052		//����ͨ��UC������ǼĴ���
#define YIA_EMU				0x0053		//����ͨ��IA������ǼĴ���
#define YIB_EMU				0x0054		//����ͨ��IB������ǼĴ���
#define YIC_EMU				0x0055		//����ͨ��IC������ǼĴ���
#define YIN_EMU				0x0056		//����ͨ��IN������ǼĴ���
//���������@@@@@
#define	UA_HW_EMU			0x00A0		//UA�벨��Чֵ�Ĵ���
#define	UB_HW_EMU			0x00A1		//UB�벨��Чֵ�Ĵ���
#define	UC_HW_EMU			0x00A2		//UC�벨��Чֵ�Ĵ���
#define	IA_HW_EMU			0x00A3		//IA�벨��Чֵ�Ĵ���
#define	IB_HW_EMU			0x00A4		//IB�벨��Чֵ�Ĵ���
#define	IC_HW_EMU			0x00A5		//IC�벨��Чֵ�Ĵ���
#define	UAPEAF_EMU			0x00A6		//UA��ѹ�벨��ֵ�Ĵ���
#define	UBPEAF_EMU			0x00A7		//UB��ѹ�벨��ֵ�Ĵ���
#define	UCPEAF_EMU			0x00A8		//UC��ѹ�벨��ֵ�Ĵ���

//-----------------------У��Ĵ���-----------------------
//ͨ��
#define	PHSUA_EMU			0x010C		//UAͨ����λУ���Ĵ���				0x80
#define	PHSUB_EMU			0x010D		//UBͨ����λУ���Ĵ���				0x80
#define	PHSUC_EMU			0x010E		//UCͨ����λУ���Ĵ���				0x80

#define DCOS_UA_EMU			0x011A 		//UAͨ��ֱ��OFFSETУ���Ĵ���
#define DCOS_UB_EMU			0x011B 		//UBͨ��ֱ��OFFSETУ���Ĵ���
#define DCOS_UC_EMU			0x011C 		//UCͨ��ֱ��OFFSETУ���Ĵ���

#define DCOS_IA_EMU			0x011D 		//IAͨ��ֱ��OFFSETУ���Ĵ���
#define DCOS_IB_EMU			0x011E 		//IBͨ��ֱ��OFFSETУ���Ĵ���
#define DCOS_IC_EMU			0x011F 		//ICͨ��ֱ��OFFSETУ���Ĵ���
#define DCOS_IN_EMU			0x0120 		//INͨ��ֱ��OFFSETУ���Ĵ���

#define UA_OS_EMU			0x0121 		//UA��ЧֵOFFSETУ���Ĵ���
#define UB_OS_EMU			0x0122 		//UB��ЧֵOFFSETУ���Ĵ���
#define UC_OS_EMU			0x0123 		//UC��ЧֵOFFSETУ���Ĵ���

#define	IAOS_EMU			0x0124		//IA��ЧֵOFFSETУ���Ĵ���
#define	IBOS_EMU			0x0125		//IB��ЧֵOFFSETУ���Ĵ���
#define	ICOS_EMU			0x0126		//IC��ЧֵOFFSETУ���Ĵ���
#define	INOS_EMU			0x0127		//IN��ЧֵOFFSETУ���Ĵ���

#define FUA_OS_EMU			0x013D 	//UA������ЧֵOFFSETУ���Ĵ���
#define FUB_OS_EMU			0x013E 	//UB������ЧֵOFFSETУ���Ĵ���
#define FUC_OS_EMU			0x013F 	//UC������ЧֵOFFSETУ���Ĵ���

#define	FIAOS_EMU			0x0140		//IA������ЧֵOFFSETУ���Ĵ���
#define	FIBOS_EMU			0x0141		//IB������ЧֵOFFSETУ���Ĵ���
#define	FICOS_EMU			0x0142		//IC������ЧֵOFFSETУ���Ĵ���

//����
#define	GFPA_EMU			0x0143		//A������й���������Ĵ���
#define	GFPB_EMU			0x0144		//B������й���������Ĵ���
#define	GFPC_EMU			0x0145		//C������й���������Ĵ���

#define	GFQA_EMU			0x0146		//A������޹���������Ĵ���
#define	GFQB_EMU			0x0147		//B������޹���������Ĵ���
#define	GFQC_EMU			0x0148		//C������޹���������Ĵ���

#define	GFSA_EMU			0x0149		//A��������ڹ�������Ĵ���
#define	GFSB_EMU			0x014A		//B��������ڹ�������Ĵ���
#define	GFSC_EMU			0x014B		//C��������ڹ�������Ĵ���

#define	FPAPHSL_EMU			0x014C	//A�л�����������λУ����
#define	FPBPHSL_EMU			0x014D	//B�л�����������λУ����
#define	FPCPHSL_EMU			0x014E	//C�л�����������λУ����
#define	FQAPHSL_EMU			0x014F	//A�޻�����������λУ����
#define	FQBPHSL_EMU			0x0150	//B�޻�����������λУ����
#define	FQCPHSL_EMU			0x0151	//C�޻�����������λУ����

#define	PAOS_EMU			0x0152		//A�й�����OFFSETУ���Ĵ���
#define	PBOS_EMU			0x0153		//B�й�����OFFSETУ���Ĵ���
#define	PCOS_EMU			0x0154		//C�й�����OFFSETУ���Ĵ���

#define	QAOS_EMU			0x0155		//A�޹�����OFFSETУ���Ĵ���
#define	QBOS_EMU			0x0156		//B�޹�����OFFSETУ���Ĵ���
#define	QCOS_EMU			0x0157		//C�޹�����OFFSETУ���Ĵ���

/*
#define	D2FP00_EMU			(DWORD)&EMU->PA_PHSM_D2FP00	//�Զ��幦�ʼĴ���0				0x00
#define	D2FP01_EMU			(DWORD)&EMU->PA_PHSH_D2FP01	//�Զ��幦�ʼĴ���1				0x00
#define	D2FP02_EMU			(DWORD)&EMU->PB_PHSM_D2FP02	//�Զ��幦�ʼĴ���2				0x00
#define	D2FP03_EMU			(DWORD)&EMU->PB_PHSH_D2FP03	//�Զ��幦�ʼĴ���3				0x00
#define	D2FP04_EMU			(DWORD)&EMU->PC_PHSM_D2FP04	//�Զ��幦�ʼĴ���4				0x00
#define	D2FP05_EMU			(DWORD)&EMU->PC_PHSH_D2FP05	//�Զ��幦�ʼĴ���5				0x00
#define	D2FP06_EMU			(DWORD)&EMU->QA_PHSM_D2FP06	//�Զ��幦�ʼĴ���6				0x00
#define	D2FP07_EMU			(DWORD)&EMU->QA_PHSH_D2FP07	//�Զ��幦�ʼĴ���7				0x00
#define	D2FP08_EMU			(DWORD)&EMU->QB_PHSM_D2FP08	//�Զ��幦�ʼĴ���8				0x00
#define	D2FP09_EMU			(DWORD)&EMU->QB_PHSH_D2FP09	//�Զ��幦�ʼĴ���9				0x00
#define	D2FP10_EMU			(DWORD)&EMU->QC_PHSM_D2FP10	//�Զ��幦�ʼĴ���10				0x00
#define	D2FP11_EMU			(DWORD)&EMU->QC_PHSH_D2FP11	//�Զ��幦�ʼĴ���11				0x00

#define	PA_PHSM_EMU			(DWORD)&EMU->PA_PHSM_D2FP00	//A�й�������λУ������			0X00
#define	PA_PHSH_EMU			(DWORD)&EMU->PA_PHSH_D2FP01	//A�й�������λУ���ĸ�			0X00
#define	PB_PHSM_EMU			(DWORD)&EMU->PA_PHSM_D2FP02	//B�й�������λУ������			0X00
#define	PB_PHSH_EMU			(DWORD)&EMU->PA_PHSH_D2FP03	//B�й�������λУ���ĸ�			0X00
#define	PC_PHSM_EMU			(DWORD)&EMU->PA_PHSM_D2FP04	//C�й�������λУ������			0X00
#define	PC_PHSH_EMU			(DWORD)&EMU->PA_PHSH_D2FP05	//C�й�������λУ���ĸ�			0X00
#define	QA_PHSM_EMU			(DWORD)&EMU->QA_PHSM_D2FP06	//A�޹�������λУ������			0X00
#define	QA_PHSH_EMU			(DWORD)&EMU->QA_PHSH_D2FP07	//A�޹�������λУ���ĸ�			0X00
#define	QB_PHSM_EMU			(DWORD)&EMU->QA_PHSM_D2FP08	//B�޹�������λУ������			0X00
#define	QB_PHSH_EMU			(DWORD)&EMU->QA_PHSH_D2FP09	//B�޹�������λУ���ĸ�			0X00
#define	QC_PHSM_EMU			(DWORD)&EMU->QA_PHSM_D2FP10	//C�޹�������λУ������			0X00
#define	QC_PHSH_EMU			(DWORD)&EMU->QA_PHSH_D2FP11	//C�޹�������λУ���ĸ�			0X00
*/
//PRTH1L�Ĵ���Ϊ0ʱ,�������ֶ���λУ��
#define	PRTH1L_EMU			0x0106		//��λ�����������üĴ���				0X00												
#define	PRTH1H_EMU			0x0107		//��λ�����������üĴ���				0X00
#define	PRTH2L_EMU			0x0108		//��λ�����������üĴ���				0X00
#define	PRTH2H_EMU			0x0109		//��λ�����������üĴ���				0X00
#define	IRegion3L_EMU		0x010A	//������ֵ3����
#define	IRegion3H_EMU		0x010B	//������ֵ3����
//ֱ��OFFSETУ���Ĵ����ڸ�֮ͨ�󣬵�AUTODC_EN.BIT7 =0,���۲���ͨ·��ͨ�Ƿ�رգ�����Ӱ��EMUͨ����Чֵ�����ʡ������Ȳ�����
#define	OILV_EMU			0x015A		//������ֵ�Ĵ���					0X00
#define	OVLVL_EMU			0x0159		//��ѹ��ֵ�Ĵ���					0X00
#define	OI2_LVL_EMU			0x0198		//������ֵ��ֵ�Ĵ���				0x00
#define	OI2_CNTTH_EMU		0x0199		//����ʱ����ֵ�Ĵ���				0X00
#define	OV2_LVL_EMU			0x019C		//��ѹ��ֵ��ֵ�Ĵ���				0X00
#define	OV2_CNTTH_EMU		0x019D		//��ѹʱ����ֵ�Ĵ���				0X00

//-----------------------������Ĵ���-----------------------
#define	SAGCFG_EMU			0x0158		//��ѹ�ݽ���ֵ�Ĵ���					0X00
#define	EMMIE_EMU			0x0164		//EMU�жϼĴ���					0X00
#define	D2FCFG_EMU			0x0195		//�Զ����������Ĵ���				0X00
#define	CFG2_EMU			0x019A	//������Ԫ���üĴ���2				0X04
#define	HFConst2_EMU		0x0101		//��Ƶ���峣���Ĵ���2			0x1000
#define	HFConst3_EMU		0x0196		//��Ƶ���峣���Ĵ���3			0x00

/*********************************************************************************/
//-----------------------DMA��ؼĴ���-----------------------
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
//��ѹӰ����֧�ֵĵ���
#define MAX_VOLCORR_STEP		10
#define P_CREEP					0x01		//�й�����Ǳ��
#define Q_CREEP					0x10		//�й�����Ǳ��
#define NUM_FFT 				128			// ˲ʱ���ݵ���


#define ADCCTL_SYS_REMAP        0x1000      //��ӦHT6029����Ķ��壬ӳ�䵽�Ĵ���ADCCTL_SYS��

//����Լ��Ĵ�����������չ�� //@liang
#define EDT_CMD_TYPE_MASK       0xFF00
#define EDT_CMD_CHANNEL_MASK    0x00F0
#define EDT_CMD_PHASE_MASK      0x000F

#define EDT_STATUS              0xF000      //EDT״̬��ѯ��0У׼��/1У׼���
#define EDT_CALI                0xF100      //EDTУ׼����            
#define EDT_PARAM               0xF200      //EDT������׼��0xF200��ʼ����24�����ȷ�ֵ����λ���ȵ�����ABC�����ѹ123��ABC��
#define EDT_CONTROL             0xF300      //EDT�������0�ر�/1����
#define EDT_ERR_AMP             0xF400      //��ֵ���ٷֱ�
#define EDT_ERR_PHA             0xF500      //��λ���ٷֱ�
#define EDT_VERSION             0xF600      //ʵ���Ǽ���о�İ汾��������Ϊ����о��EDT�󶨣�
#define HSDC_OPEN_TIME			(30 - 2)	//�ϵ���뿪��HSDC���δ��䣨����ʶ�����ݣ���ʹ��bySamplePowerOnTimer�жϣ�

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	RN830xV20 = 0x20,/*"V2 ��"*/
	RN830xV30 = 0x30,/*"V30 ��"*/
	RN830xV31 = 0x31,/*"V31 ��"*/
	RN830xV32 = 0x32,/*"V32 ��"*/
	RN830xVerr = 0xff/*"����汾"*/
}eRN830xVer_TypeDef;

//˲ʱ��
typedef struct TRemoteValue_t
{
	TFourByteUnion   U[3];				//��ѹxxxxx.xxx A B C
	TFourByteUnion   I[5];				//����xxxx.xxxx A B C N N2
	TFourByteUnion   P[4];				//�й�����xx.xxxxxx T A B C
	TFourByteUnion   Q[4];				//�޹�����xx.xxxxxx
	TFourByteUnion   S[4];				//���ڹ���xx.xxxxxx
	TFourByteUnion   Cos[4];			//��������xxxxx.xxx
	
	#if( SEL_VOL_ANGLE_FUNC == YES )
	TFourByteUnion 	YUAngle[3];			//�����ʽ	
	TFourByteUnion 	YIAngle[3];			//�����ʽ
	#endif
	
	TFourByteUnion	Pav[3][4];			//һ����ƽ������xxxx.xxxx
	TFourByteUnion	Freq;				//����Ƶ��
	
	TTwoByteUnion	Uunbalance;			//��ѹ��ƽ����
	TTwoByteUnion	Iunbalance;			//������ƽ����	
}TRemoteValue;

//У׼ϵ��
typedef struct TSampleAdjustReg_t
{
	TFourByteUnion 	GP[3];			//�й���������У��
	TFourByteUnion 	QP[3];			//�޹���������У��
	TFourByteUnion 	SP[3];			//���ڹ�������У��
	TFourByteUnion 	GSU[3];			//��ѹͨ������У��
	TFourByteUnion 	GSI[3];			//����ͨ������У��
	TFourByteUnion 	PHSP[3];		//�й�������λУ��
	TFourByteUnion 	PHSI[3];		//����ͨ����λУ��
	TFourByteUnion 	POS[3];			//�й�����ƫ��У��	
	TFourByteUnion 	IOS[3];			//�й�����ƫ��У��
	
	DWORD 			CRC32;			//4�ֽ�У��
}TSampleAdjustReg;

//���ƼĴ���
typedef struct TSampleCtrlReg
{
	TFourByteUnion 	CFCFG;			//CF�������üĴ���
	TFourByteUnion 	EMMIE;			//�����жϼĴ���
	TFourByteUnion 	EMUCFG;			//EMU���üĴ���
	TFourByteUnion 	EMUCON;			//EMU���ƼĴ���
	TFourByteUnion	ZXOTCFG;		//����������üĴ���
	TFourByteUnion 	AUOTDC;			//ֱ��OFFSET�Զ�У��ʹ�ܼĴ���
	TFourByteUnion  ADCCTL;			//ADC ������ƼĴ���
	TFourByteUnion 	HFCONST;		//�������Ƶ��
	TFourByteUnion 	PSTART;			//�𶯹��ʼĴ���	
	TFourByteUnion 	MODSEL;			//�������ߺ���������ģʽѡ��Ĵ���
	TFourByteUnion	PRTH1L;			//��λ�����������üĴ���
	TFourByteUnion	PRTH1H;			//��λ�����������üĴ���
	TFourByteUnion	PRTH2L;			//��λ�����������üĴ���
	TFourByteUnion	PRTH2H;			//��λ�����������üĴ���
	TFourByteUnion	SAGCFG;			//����������	
	TFourByteUnion 	EMUCFG3;		//EMU���üĴ���
	TFourByteUnion	ZXOT;			//����������ֵ�Ĵ���
	TFourByteUnion	ZXOTU;			//��ѹ������ֵ�Ĵ���
	DWORD 			CRC32;			//4�ֽ�У��
}TSampleCtrlReg;

//����ϵ��
typedef struct TSampleCorr_t
{
	TFourByteUnion	UGain[3];		//��ѹϵ�� A B C
	TFourByteUnion	IGain[4];		//����ϵ�� A B C N
	TFourByteUnion  PGain[4];		//����ϵ�� T A B C

	DWORD 			CRC32;			//4�ֽ�У��
}TSampleCorr;

//����ϵ�� �ֶβ���ϵ��
typedef struct TSampleAdjustParaDiv_t
{
	WORD PGainDiv[3][10];			//��չ9������ϵ��

	WORD PhsRegDiv[3][10];			//��չ9�νǲ�ϵ��

	DWORD CRC32;
}TSampleAdjustParaDiv;

//�ֶ�����ϵ��
typedef struct TSampleManualModifyPara_t
{
	signed char ManualModifyForWard[9];		//�����й����������������������沿��
	signed char ManualModifyForRA[9];  		//�����й����������������������沿��
	signed char ManualModifyQGain[3][9];	//�ֶ������޹������洢����ֻ���޹������������
	WORD 		VolUnbalanceGain[3];		//��ѹ��ƽ�ⲹ��ϵ��
	
	DWORD CRC32;
}TSampleManualModifyPara;

//�ֶ�У���ͻ�����������
typedef struct TCurrenttValue_t
{
	DWORD Upper;
	DWORD Lower;
}TCurrenttValue;



//����Ĭ������ʹ�С���ϵ��
typedef struct TOriginalPara_t
{
	WORD AngCoe[2][MAX_PHASE];					//ԭʼ��λϵ��
	WORD PGainCoe[MAX_PHASE];					//ԭʼ�й�����ϵ��

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

//EDT��׼�����ṹ��
typedef struct EDTDataPara_t
{
    DWORD   AMP_STD[4][MAX_PHASE];			//��Чֵ
    DWORD   PHS_STD[4][MAX_PHASE];			//��λ
    
	DWORD   CRC32;
}TEDTDataPara;

typedef enum
{
	eVOLTAGE = 0X00,	//��ѹ
	eCURRENT = 0X55,	//����
	ePOWER	= 0XAA,		//����
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
	eCORR = 0,	//����ϵ��
	eADJUST,	//У׼����
	eCRTL,		//���Ʋ���
	eVOLCORR,	//��ѹӰ����(����)
    eEDT,       //EDT��׼����
    eOTHER,		//����
	eALL,		//����	
}eSampleType;

//ƽ������
typedef struct TPAverage_t
{
	BYTE  Cnt;			//����ֵ
	BYTE  ClearFlag;	//�����־
	float SumValue;		//�ۼ�ֵ
}TPAverage;


#pragma pack(1)
//��ѹ�����Сֵ
typedef struct TVolMaxOrMin_t
{
	WORD 		VolValue;
	BYTE		Time[6];    //��¼����ʱ��
}TVolMaxOrMin;

//�������ֵ
typedef struct TCurrMax_t
{
	BYTE				Time[6];    //��¼����ʱ��
	DWORD 				CurrValue[MAX_PHASE];
}TCurrMax;

//������Сֵ
typedef struct TCurrMin_t
{
	DWORD		 		CurrValue;
	BYTE				Time[6];    //��¼����ʱ��
	BYTE 				CurrMinus;	
}TCurrMin;

//��ֵ�洢��
typedef struct TExtremumMax_t
{
	TVolMaxOrMin VolMax[MAX_PHASE];		//��ѹ���ֵ
	TCurrMax CurrMax[MAX_PHASE];		//�������ֵ
}TExtremumMax;

typedef struct TExtremumMin_t
{
	TVolMaxOrMin VolMin[MAX_PHASE];		//��ѹ��Сֵ
	TCurrMin CurrMin[MAX_PHASE];		//������Сֵ
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
//				��������
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
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��RN8302�������������
//
//����:		Cmd[in]	 
//					CH=0xa6,д����ͳһ������
//					CH=0x00,д��ֹ          
//����ֵ: 	��
//		   
//��ע: 
//-----------------------------------------------
void SampleSpecCmdOp(WORD Addr,eRnAddrCmd Cmd, BYTE chipNo);
//-----------------------------------------------
//��������: ��RN2026�Ĵ���
//
//����:		Addr[in],	��ַ	 
//			pBuf[out],	�������ݣ�4�ֽڣ����ں󣬵���ǰ����4�ֽڹ̶���Ϊ0��
//			Len    		���ȣ���ȡ��
//
//����ֵ: 	TRUE/FALSE
//
//��ע:     ��    
//-----------------------------------------------
BOOL Link_ReadSampleReg( WORD Addr, BYTE *pBuf ,BYTE bLen, BYTE chipNo);

//-----------------------------------------------
//��������: дRN2025�Ĵ���
//
//����:		Addr[in]	��ַ	 
//			pBuf[in]	д������ 4�ֽ� ���ں󣬵���ǰ
//����ֵ: 	
//		    TRUE/FALSE
//��ע: 
//-----------------------------------------------
BOOL WriteSampleReg(WORD addr, BYTE *pBuf ,BYTE bLen, BYTE chipNo);

//---------------------------------------------------------------
//��������: ��λ����оƬ
//
//����:   ��
//
//����ֵ: ��
//
//��ע: ��ʱ���õ�������΢�⺯��
//---------------------------------------------------------------
void SoftResetSample( void );

//---------------------------------------------------------------
//��������: �õ���ѹӰ���������ȼ�
//
//����: 	Mode: 
//			 0x00-��У���� ���ص�ǰ��ѹ�ȼ�
//			 0x01-�������� �����ж����� ��ֹ��ѹ����
//                   
//����ֵ:   0���޵�ѹ���� ������220V
//         ���---����---��������--������ѹ�ȼ�
//		   1~4 | 5%   |   1.0   | 80%/90%/110%/115%
//         5~6 | 10%  |   0.5L  | 90%/110%
// 		   7~8 | 100% |   0.5L  | 90%/110%
// 		   9~10| Imax |   0.5L  | 90%/110%
//
//��ע:   
//---------------------------------------------------------------
BYTE GetVolCorrectLevel( BYTE Mode );

//void api_DMAControl(eState Type);

//-----------------------------------------------
//��������: ��ʼ��У���׼Դ�ĵ�ѹ������
//
//����:		��	 
//			
//����ֵ: 	��
//		    
//��ע: 
//-----------------------------------------------
void InitSampleCaliUI( void );

//-----------------------------------------------
//��������: �����Ĵ�������ΪĬ����ֵ
//
//����:		eSampleType Type
//			
//����ֵ: 	��
//		    
//��ע:
//-----------------------------------------------
void UseSampleDefaultPara( eSampleType Type, BYTE chipNo );
//-----------------------------------------------
//��������: Ч�����оƬ�ļĴ���
//
//����:		��
//
//����ֵ: 	��
//
//��ע:  
//-----------------------------------------------
BOOL CheckSampleChipReg(BYTE chipNo);
//--------------------------------------------------
//��������:  ���ü���оƬ����ģʽ
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_SetSampleRunPara( eIntegratorMode inmode , eMeasurementMode memode, BYTE chipNo );

//-----------------------------------------------
//��������: У��ϵ�����浽EEPROM������ˢ�±�־
//
//����:		��
//          
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void WriteEEpromSamplePara( eSampleType Type, BYTE chipNo );


//-----------------------------------------------
//��������: дĳ��У��ϵ����EEPROM
//
//����:		WriteEepromFlag[in]	bit0 SampleCtrlAddr 0--дram��дee	1--дramдee
//								bit1 SampleAdjustAddr 0--дram��дee	1--дramдee
//			Addr[in]	��ַ	 
//			Buf[in]		д�����ݣ����ֽڣ����ں󣬵���ǰ
//����ֵ: 	
//		    TRUE/FALSE
//��ע: 
//-----------------------------------------------
BOOL WriteSampleParaToEE(BYTE WriteEepromFlag,DWORD Addr, BYTE *Buf, BYTE chipNo);

#endif//#if( SAMPLE_CHIP == CHIP_RN8302B )
#endif//#ifndef CHIP_RN2026EMU
