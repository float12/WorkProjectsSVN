//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.20
//����		������
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

#if( MAX_PHASE == 1 )
//-----------------------------------------------
//			System ϵͳ���ò���
//-----------------------------------------------
//---���¹�����ֵ��䲻�����޸�------
const BYTE MeterTypesConst = cMETER_TYPES;
const BYTE MeterCurrentTypesConst = cCURR_TYPES;
const BYTE RelayTypeConst = cRELAY_TYPE;
const BYTE SpecialClientsConst = cSPECIAL_CLIENTS;
const WORD SelThreeBoard = BOARD_TYPE;      //CPU��,�Ե����û���壬��Ҫ�ǿ�������ģ���485��ĳ������
const BYTE MeterVoltageConst = cMETER_VOLTAGE;  //����ѹ�ȼ�
const BYTE SelZeroCurrentConst = YES;//���ߵ������
const BYTE SelSecPowerConst = NO;//�����û�и�����Դ
const BYTE SelOscTypeConst = cOSCILATOR_TYPE;//ѡ��������
//---������ֵ��䲻�����޸�end------

//------�������ÿɸ�������޸�------
const WORD SoftVersionConst = 0xff47;   	//ǰ��λ����ʡ�ݣ�����λ�����꣬����λ�����б���Ͳ��·�
const BYTE SelVolCorrectConst = NO;		//��ѹӰ�������� ---����ʱNO�ر�----����YES����---
const BYTE SelDivAdjust =  NO;				//�������� �����û��ʵ������const BYTE SelMChipVrefgain = NO;		    //�������� �����û��ʵ������
const BYTE SelMChipVrefgain = NO;		    //�������� �����û��ʵ������const BYTE PowerUpSyncMoneyBagConst = NO;	//�ϵ��Ƿ���Ҫͬ��Ǯ�����߷���ҪѡNO��Ϊ�˽��͹��ġ��������ѡ��YES
const BYTE ZeroCTPositive = NO;				//�������ߵ��������������ӿ��� �˲����̶�ΪNO, ���ܱ����ģ���������Ӳ���ı�---YES: ����---NO: ����---
const BYTE ESAMPowerControlConst = YES;		//�ϵ�ESAM��Դ����ʱ�� ---����ʱNO�ر� ��Դ����120s----����YES����--��Դ����3s
//------���ÿɸ�������޸�end------


//-----------------------------------------------
//			protocol ���ò���
//-----------------------------------------------
const BYTE WATCH_SCI_TIMER = 65;//7 �����ֶ�������������� wlk 2009-7-16
//���ܳ�ʱʱ�䣬��λ�����룬Ҳ������500�������û�н��ܵ��µ����ݣ�������ܻ���
const WORD MAX_RX_OVER_TIME = 500/10;//��ʱ����10ms
//���յ����͵���ʱ������Ϊ��λ�����յ���Ч֡����ʱ��ʱ���ڷ��ͣ���Ϊ�������ķ�ʱ�䣩
const WORD RECE_TO_SEND_DELAY = 25 / 10 + 1; //��ʱ����10ms

//-----------------------------------------------
//			Sample ����Ĭ�ϲ���
//-----------------------------------------------
#if( cCURR_TYPES == CURR_60A )
const WORD wMeterBasicCurrentConst = 5000;//����������3λС��
const DWORD dwMeterMaxCurrentConst = 60000; //��������3λС��
#else
const WORD wMeterBasicCurrentConst = 10000; //����������3λС��
const DWORD dwMeterMaxCurrentConst = 100000; //��������3λС��
#endif
const WORD wStandardVoltageConst = 2200; //������ѹ��1λС��
const WORD wCaliVoltageConst = 2200; //У���ѹ��1λС��
//Ĭ�ϵ�ѹϵ��ƫ����
const WORD Uoffset = 0x0000;		
//���Ϊ125000��ʾ125v�رռ���,���Ϊ0���ֹ�رռ���(�������ó�0)
const WORD Defendenergyvoltage = 0;	
//��������������ֵ15mA
const WORD Def_Shielding_I1 = 150;	
//���������������ֵ20mA
const WORD Def_Shielding_I2 = 200;
//����У��������ֵ	
const WORD Def_MaxError = 30;
//�������ۼӷǷ��ж�	
const DWORD	MaxEnergyPulseConst = 100;//�����ã�������Բ���

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
	0x0000,	//Ƶ��ѡ���־
};

const WORD SampleCtrlDefData8000[8] = 
{	
	0x0000,	//EMUCFG
	0x0000,	//FreqCFG
	0x007e,	//ModuleEn
	0x0187,	//ANAEN		Bit8-Bit7 Vref��׼���׵����Ĵ��������¸��¶Σ�����ݲ�ͬ�̵���������ֵͬ ��8000:1оƬ
	0x0000,	//IOCFG
	0x0231, //ANACON	Bit10-Bit8 Vref��׼һ�׵����Ĵ��� ����ݲ�ͬ�̵���������ֵͬ ��8000:1оƬ
	0x0000,	//MODECFG
	0x5555,	//Ƶ��ѡ���־
};

//5000:1��HT7017У��Ĵ���0x58(�޹���λУ��)��0x64(�Ʋ�������λУ��)������д�����оƬ
const WORD SampleAdjustDefData[15] = 
{
	0x0000,	//GPA
	0x0000,	//GQA
	0x0000,	//GSA
	0x0172,	//GPB ���߻�����
	#if( cRELAY_TYPE == RELAY_INSIDE )
	0xFF7C, //QPhsCal �޹���λУ��(Ƶ��Ӱ����)--����
	#else
	0xFF7C, //QPhsCal �޹���λУ��(Ƶ��Ӱ����)
	#endif
	#if( cCURR_TYPES == CURR_60A )
	0x100C, //ADCCON ���ߵ���24������
	#else
	0x000C, //ADCCON ���ߵ���16������
	#endif
	0x0040,	//PStart
	#if(SEL_CRYSTALOSCILLATOR_55296M == YES)
	0x0093,	//HFConst
	#else
	0x00A0,	//HFConst
	#endif
	#if( cRELAY_TYPE == RELAY_INSIDE )
	0x001B, //DEC_Shift �Ʋ�������λУ��(Ƶ��Ӱ����)--����
	#else
	0x0010, //DEC_Shift �Ʋ�������λУ��(Ƶ��Ӱ����)
	#endif
	0x0000,	//P1OFFSET
	0x0010, //IARMSOS	������ƯĬ��У��ֵ ��ֹ�����δУ��Ư������³�����Ư����Ӱ��̵������жϣ��Ӷ�Ӱ�쳵��ĵ���
	0x0006, //IBRMSOS	������ƯĬ��У��ֵ
	0x0000,	//GPHs1 �������
	0xEA1E,	//GPHs2 ������� ���߻�����
	0x0000, //P1OFFSETL
};
//-----------------------------------------------
//			LCD ��ʾĬ�ϲ���
//-----------------------------------------------

const TLCDPara LCDParaDef = 
{ 
	.PowerOnDispTimer = 5 , 			//�ϵ�ȫ��ʱ��  unsigned��
	.BackLightPressKeyTimer = 60 , 	//�������ʱ��  long-unsigned(����ʱ�������ʱ��)
	.BackLightViewTimer = 10 , 		//��ʾ�鿴�������ʱ��  long-unsigned��
	.LcdSwitchTime = 30, 			//�޵簴����Ļפ�����ʱ��  long-unsigned�� ����ʱ�䣬HEX ��λΪ��
	.EnergyFloat = 2 , 				//����С����
	.DemandFloat = 4 , 				//���ʣ�������С����
	.Meaning12 = 0,					//��ʾ12��������
#if(PREPAY_MODE == PREPAY_LOCAL)//���ر�
	.DispItemNum = {7,27},			//ѭ�ԣ�����
#else							//Զ�̷ѿر�
	.DispItemNum = {5,23},			//ѭ�ԣ�����
#endif	
	.KeyDisplayTime = 0x003C,		//�����л���ѭ����ʾ��ʱ�� ��ʾ������3����
	.LoopDisplayTime = 0x0005,		//ѭ����ʾʱÿ������ʾʱ�� ��ʾ������3����
};
//20�淶���ⲻ������С�����뱳��ͬ��	
const TWarnDispPara WarnDispParaConst = 
{  	  
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 0 -- �����̵���
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 1 -- LCDС�����������Ʋ���
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 2 -- Һ������
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 3 -- Һ��ERR��ʾ����
};

//ѭ����ĿĬ������(15��) 0-��Ĭ����ʾ�� 1~199����LcdItemTable������кţ�200~234����ExpandLcdItemTable������кţ�fe-����ܳ�ʼ���� ͨ����Լ����
const BYTE LoopItemDef[15]=                    
{
#if(PREPAY_MODE == PREPAY_LOCAL)
	1,//ѭ�Ե�1��	��ǰʣ����
#endif
	62,//ѭ�Ե�2��	��ǰ����й��ܸ߾��ȵ���
	63,//ѭ�Ե�3��	��ǰ����й���߾��ȵ���
	64,//ѭ�Ե�4��	��ǰ����й���߾��ȵ���
	65,//ѭ�Ե�5��	��ǰ����й�ƽ�߾��ȵ���
	66,//ѭ�Ե�6��	��ǰ����й��ȸ߾��ȵ���	
#if(PREPAY_MODE == PREPAY_LOCAL)
	17,//ѭ�Ե�7��		��ǰ���
#endif
};

//������ĿĬ������(60��) 0-��Ĭ����ʾ�� 1~199����LcdItemTable������кţ�200~234����ExpandLcdItemTable������кţ�fe-����ܳ�ʼ���� ͨ����Լ����
const BYTE KeyItemDef[60] =                
{
#if(PREPAY_MODE == PREPAY_LOCAL)
	1,	//���Ե�1�� 		ʣ����
#endif
	62,//���Ե�2��	��ǰ����й��ܸ߾��ȵ���
	63,//���Ե�3��	��ǰ����й���߾��ȵ���
	64,//���Ե�4��	��ǰ����й���߾��ȵ���
	65,//���Ե�5��	��ǰ����й�ƽ�߾��ȵ���
	66,//���Ե�6��	��ǰ����й��ȸ߾��ȵ���	
	77,//���Ե�7�� ��1������й��ܸ߾��ȵ���
	78,//���Ե�8��	��1������й���߾��ȵ���
	79,//���Ե�9��	��1������й���߾��ȵ���
	80,//���Ե�10��	��1������й�ƽ�߾��ȵ���
	81,//���Ե�11��	��1������й��ȸ߾��ȵ���
	92,//���Ե�12��	��2������й��ܸ߾��ȵ���
	93,//���Ե�13��	��2������й���߾��ȵ���
	94,//���Ե�14��	��2������й���߾��ȵ���
	95,//���Ե�15��	��2������й�ƽ�߾��ȵ���
	96,//���Ե�16��	��2������й��ȸ߾��ȵ���
#if(PREPAY_MODE == PREPAY_LOCAL)
	17,//���Ե�17��	��ǰ���
	18,//���Ե�18��	�û����ŵ�8λ
	19,//���Ե�19��	�û����Ÿ�4λ
#endif
	20,//���Ե�20��	ͨ�ŵ�ַ��8λ		40010200
	21,//���Ե�21��	ͨ�ŵ�ַ��4λ
	22,//���Ե�22��	��ǰ����			40000200
	23,//���Ե�23��	��ǰʱ��
	24,//���Ե�24��	˲ʱ��ѹ			20000201
	25,//���Ե�25��	���ߵ���			20010201
	26,//���Ե�26��	˲ʱ����			20040201
	27,//���Ե�27��	��������			200A0201
};
//��չ���200��ʼ
const BYTE ExpandLcdItemTable[20][10] =
{
	{0X00, 0X00, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//��չ��1��	00000201--��ǰ����й��ܵ��� ����
};

//-----------------------------------------------
//			Protocol ͨ��Ĭ�ϲ���
//-----------------------------------------------

//-----------------------------------------------
//			Lpf ��������Ĭ�ϲ���
//-----------------------------------------------

//-----------------------------------------------
//			Demand ����Ĭ�ϲ���
//-----------------------------------------------

//-----------------------------------------------
//			PrePay Ԥ����Ĭ�ϲ���
//-----------------------------------------------

//-----------------------------------------------
//			Freeze ����Ĭ�ϲ���
//-----------------------------------------------
// ˲ʱ����
// Ĭ��������Ч��3����
const TFreezePara InstantFreeze[MAX_INSTANT_FREEZE_ATTRIBUTE+1]=
{
	{3, 0x00000003,	3 },		//�������Ը���
	{0, 0x00100400, 3 }, 		//�����й�����					20  
	{0, 0x00200400, 3 }, 		//�����й�����					20
	{0, 0x20040200, 3 }, 		//�й�����					   	16	
};

// ���Ӷ���
// 288����   ���ں���Ⱦ���������Ϊ0
const TFreezePara MinFreeze[MAX_MINUTE_FREEZE_ATTRIBUTE+1]=
{
	{8,  0x00000008, 8					 }, //�������Ը���
	{15, 0x00100401, MAX_FREEZE_MIN_DEPTH },// �����й��ܵ���
	{15, 0x00200401, MAX_FREEZE_MIN_DEPTH },// �����й��ܵ���
	{15, 0x20000200, MAX_FREEZE_MIN_DEPTH },// ��ѹ
	{15, 0x20010200, MAX_FREEZE_MIN_DEPTH },// ����
	{15, 0x20010400, MAX_FREEZE_MIN_DEPTH },// ���ߵ���
	{15, 0x20040200, MAX_FREEZE_MIN_DEPTH },// �й�����
	{15, 0x200A0200, MAX_FREEZE_MIN_DEPTH },// ��������
	{15, 0x48002400, FREEZE_MIN_DEPTH1 }, 	// �����豸�������ݵ�Ԫ
};

// Сʱ����
// Ĭ������1Сʱ��254����
const TFreezePara HourFreeze[MAX_HOUR_FREEZE_ATTRIBUTE+1]=
{
	{2, 0x00000002,	2},					//�������Ը���
	{1, 0x00100401, 254 },
	{1, 0x00200401, 254 }
};

// �ն���
// Ĭ������1�죬62����
const TFreezePara DayFreeze[MAX_DAY_FREEZE_ATTRIBUTE+1]=
{
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{5, 0x00000005,	5},					//�������Ը���
	#else
	{3, 0x00000003,	3},					//�������Ը���
	#endif
	{1, 0x00100400,  365 },// �����й�����
	{1, 0x00200400,  365 },// �����й�����
	{1, 0x20040200,  365 }, // �й�����
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{1,	 0x202c0200, 365 },// ʣ����������
	{1,	 0x202d0200, 365 },// ͸֧���
	#endif
};

// �����ն���
// Ĭ��������Ч��12����
const TFreezePara ClosingFreeze[MAX_CLOSING_FREEZE_ATTRIBUTE+1]=
{	
	{4, 0x00000004,	4},		//�������Ը���
	{1, 0x00000400, 12 }, //����й�����
	{1, 0x00100400, 12 }, //�����й�����
	{1, 0x00200400, 12 }, //�����й�����
	{1, 0x20310200, 12 }, //�¶��õ��� 
};

// �¶���
// Ĭ������1�£�12����
const TFreezePara MonFreeze[MAX_MON_FREEZE_ATTRIBUTE+1]=
{
	{3, 0x00000003,	3},		//�������Ը���
	{1, 0x00000400, 24 },	//����й�����					20
	{1, 0x00100400, 24 },	//�����й�����				   	20
	{1, 0x00200400, 24 },	//�����й�����				 	20
};

// ʱ�����л�����
// Ĭ��������Ч��2����
const TFreezePara TZChgFreeze[MAX_TIME_ZONE_FREEZE_ATTRIBUTE+1]=
{
	{3,  0x00000003, 3},		//�������Ը���
	{1,  0x00100400, 2 },
	{1,  0x00200400, 2 },
	{1,  0x20040200, 2 },
};

// ��ʱ�α��л�����
// Ĭ��������Ч��2����
const TFreezePara DTTChgFreeze[MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE+1]=
{
	{3,  0x00000003, 3},		//�������Ը���
	{1,  0x00100400, 2 },
	{1,  0x00200400, 2 },
	{1,  0x20040200, 2 }
};
#if(PREPAY_MODE == PREPAY_LOCAL)
// ���ʵ���л�����
// Ĭ��������Ч��2����
const TFreezePara TariffRateChgFreeze[MAX_TARIFF_RATE_FREEZE_ATTRIBUTE+1]=
{
	{3,  0x00000003, 3},		//�������Ը���
	{1,  0x00100400, 2 },
	{1,  0x00200400, 2 },
	{1,  0x20040200, 2 }
};


// �����л�����
// Ĭ��������Ч��2����
const TFreezePara LadderChgFreeze[MAX_LADDER_FREEZE_ATTRIBUTE+1]=
{
	{3,  0x00000003, 3},		//�������Ը���
	{1,  0x00100400, 2 },
	{1,  0x00200400, 2 },
	{1,  0x20040200, 2 }
};
// ���ݽ��㶳��
// Ĭ��������Ч��12����
const TFreezePara LadderClosingFreeze[MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE+1]=
{
	{1, 0x00000001,	1},//�������Ը���
	{1, 0x20320200, 4 },//���ݽ����õ���				4	 
};
#endif
//-----------------------------------------------
//			Energy ����Ĭ�ϲ���
//-----------------------------------------------
//
//-----------------------------------------------
//			Para ����Ĭ�ϲ���
//-----------------------------------------------
//�й����峣��         9
#if( cCURR_TYPES == CURR_60A )
	const DWORD ActiveConstantConst = 2000;
#elif( cCURR_TYPES == CURR_100A )
	const DWORD ActiveConstantConst = 1000;
#else
	���ô���
#endif

const TEnereyDemandMode EnereyDemandModeConst =
{
	//�й���Ϸ�ʽ������ ����й�
	.byActiveCalMode = 0x05,
	
	#if(SEL_RACTIVE_ENERGY == YES)
	//�����޹�ģʽ������
	.PReactiveMode = 0x05,
	//�����޹�ģʽ������
	.NReactiveMode = 0x50,
	#endif
	
	#if( MAX_PHASE != 1 )
	//��������
	.DemandPeriod = 15,	
	//��������ʱ��
	.DemandSlip = 1,
	//�ڼ�������������
	.IntervalDemandFreezePeriod.FreezePeriod = 15,
	.IntervalDemandFreezePeriod.Unit = 1,
	#endif
};

//ͨ�Ų�����
#if ( BOARD_TYPE == BOARD_HT_SINGLE_78201602 )
// 46:������9600; 4A: 115200
const TCommPara CommParaConst = 
{
	.I485 = 0x46,       	//��һ·485
	.ComModule = 0x46,  	//ģ��
	.ComModuleDeflt = 0x46,	//ģ��ָ�ȱʡֵ
	.II485 = 0x46,      	//�ڶ�·485�����಻�ã�
};
#else
//������115200
const TCommPara CommParaConst =
{
	.I485 = 0x4A,       	//��һ·485----115200
	.ComModule = 0x4A,  	//ģ��-415----115200
	.ComModuleDeflt = 0x46,	//�ز�	  ----9600
	.II485 = 0x03,      	//����ͨ�� ----2400
}; 
#endif

//������
const BYTE MonBillParaConst[] =
{
	01,00,
	99,99,
	99,99,
};

const TTimeZoneSegPara TimeZoneSegParaConst = 
{
	.TimeZoneNum = 1,		//��ʱ������Hex��
	.TimeSegTableNum = 2,	//��ʱ�α�����Hex��
	.TimeSegNum = 8,		//��ʱ������Hex��
	.Ratio = 4,				//��������ҪС������������Hex�룬��ǰ�û����õ���������
	.HolidayNum = 0,		//������������Hex�� ��Ϊ�¹�ԼҪ��2bytes n <= 254
	.WeekStatus = 0x7f,		//����״̬�֣�1-������0-��Ϣ��
	.WeekSeg = 0x01,		//�����ղ��õ�ʱ�α�
};

const TTimeAreaTable TimeAreaTableConst1 =//��ǰ�� 
{
	//ʱ�α��	��   ��
	1,  1,  1,		//��1ʱ����
	1,  1,  1,		//��2ʱ����
	1,  1,  1,		//��3ʱ����
	1,  1,  1,		//��4ʱ����
	1,  1,  1,		//��5ʱ����
	1,  1,  1,		//��6ʱ����
	1,  1,  1,		//��7ʱ����
	1,  1,  1,		//��8ʱ����
	1,  1,  1,		//��9ʱ����
	1,  1,  1,		//��10ʱ����
	1,  1,  1,		//��11ʱ����
	1,  1,  1,		//��12ʱ����
	1,  1,  1,		//��13ʱ����
	1,  1,  1,		//��14ʱ����
};

const TTimeAreaTable TimeAreaTableConst2 =//������ 
{
	//ʱ�α��	��   ��
	1,  1,  1,		//��1ʱ����
	1,  1,  1,		//��2ʱ����
	1,  1,  1,		//��3ʱ����
	1,  1,  1,		//��4ʱ����
	1,  1,  1,		//��5ʱ����
	1,  1,  1,		//��6ʱ����
	1,  1,  1,		//��7ʱ����
	1,  1,  1,		//��8ʱ����
	1,  1,  1,		//��9ʱ����
	1,  1,  1,		//��10ʱ����
	1,  1,  1,		//��11ʱ����
	1,  1,  1,		//��12ʱ����
	1,  1,  1,		//��13ʱ����
	1,  1,  1,		//��14ʱ����
};

const TTimeSegTable TimeSegTableConst1 = //��һ��ʱ�α�
{
	//���ʺ�   ��   ʱ
	0,  0,  1,		//��1ʱ��
	3,  0,  2,		//��2ʱ��
	6,  0,  3,		//��3ʱ��
	9,  0,  4,		//��4ʱ��
	12, 0,  1,		//��5ʱ��
	15, 0,  2,		//��6ʱ��
	18, 0,  3,		//��7ʱ��
	21, 0,  4,		//��8ʱ��
	21, 0,  4,		//��9ʱ��
	21, 0,  4,		//��10ʱ��
	21, 0,  4,		//��11ʱ��
	21, 0,  4,		//��12ʱ��
	21, 0,  4,		//��13ʱ��
	21, 0,  4,		//��14ʱ��
};

const TTimeSegTable TimeSegTableConst2 = //�ڶ���ʱ�α�
{
	//���ʺ�   ��   ʱ
	0,  0,  1,		//��1ʱ��
	3,  0,  2,		//��2ʱ��
	6,  0,  3,		//��3ʱ��
	9,  0,  4,		//��4ʱ��
	12, 0,  1,		//��5ʱ��
	15, 0,  2,		//��6ʱ��
	18, 0,  3,		//��7ʱ��
	21, 0,  4,		//��8ʱ��
	21, 0,  4,		//��9ʱ��
	21, 0,  4,		//��10ʱ��
	21, 0,  4,		//��11ʱ��
	21, 0,  4,		//��12ʱ��
	21, 0,  4,		//��13ʱ��
	21, 0,  4,		//��14ʱ��
};
const TTimeSegTable TimeSegTableConst3 = //��һ��ʱ�α�
{
	//���ʺ�   ��   ʱ
	0,  0,  1,		//��1ʱ��
	3,  0,  2,		//��2ʱ��
	6,  0,  3,		//��3ʱ��
	9,  0,  4,		//��4ʱ��
	12, 0,  1,		//��5ʱ��
	15, 0,  2,		//��6ʱ��
	18, 0,  3,		//��7ʱ��
	21, 0,  4,		//��8ʱ��
	21, 0,  4,		//��9ʱ��
	21, 0,  4,		//��10ʱ��
	21, 0,  4,		//��11ʱ��
	21, 0,  4,		//��12ʱ��
	21, 0,  4,		//��13ʱ��
	21, 0,  4,		//��14ʱ��
};

const TTimeSegTable TimeSegTableConst4 = //�ڶ���ʱ�α�
{
	//���ʺ�   ��   ʱ
	0,  0,  1,		//��1ʱ��
	3,  0,  2,		//��2ʱ��
	6,  0,  3,		//��3ʱ��
	9,  0,  4,		//��4ʱ��
	12, 0,  1,		//��5ʱ��
	15, 0,  2,		//��6ʱ��
	18, 0,  3,		//��7ʱ��
	21, 0,  4,		//��8ʱ��
	21, 0,  4,		//��9ʱ��
	21, 0,  4,		//��10ʱ��
	21, 0,  4,		//��11ʱ��
	21, 0,  4,		//��12ʱ��
	21, 0,  4,		//��13ʱ��
	21, 0,  4,		//��14ʱ��
};

const BYTE TimeSegTableFlag[] = 
{
    //ʱ�α�ѡ��
	1, //��һ�׵�1ʱ�α�ѡ��Ĭ�ϱ�
	1, //��һ�׵�2ʱ�α�ѡ��Ĭ�ϱ�
	1, //��һ�׵�3ʱ�α�ѡ��Ĭ�ϱ�
	1, //��һ�׵�4ʱ�α�ѡ��Ĭ�ϱ�
	1, //��һ�׵�5ʱ�α�ѡ��Ĭ�ϱ�
	1, //��һ�׵�6ʱ�α�ѡ��Ĭ�ϱ�
	1, //��һ�׵�7ʱ�α�ѡ��Ĭ�ϱ�
	1, //��һ�׵�8ʱ�α�ѡ��Ĭ�ϱ�
	1, //�ڶ��׵�1ʱ�α�ѡ��Ĭ�ϱ�
	1, //�ڶ��׵�2ʱ�α�ѡ��Ĭ�ϱ�
	1, //�ڶ��׵�3ʱ�α�ѡ��Ĭ�ϱ�
	1, //�ڶ��׵�4ʱ�α�ѡ��Ĭ�ϱ�
	1, //�ڶ��׵�5ʱ�α�ѡ��Ĭ�ϱ�
	1, //�ڶ��׵�6ʱ�α�ѡ��Ĭ�ϱ�
	1, //�ڶ��׵�7ʱ�α�ѡ��Ĭ�ϱ�
	1, //�ڶ��׵�8ʱ�α�ѡ��Ĭ�ϱ�
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
	.Password645[2] = {0x00,0x00,0x00,0x00 },					//����Ա02�����������ʼ����Ϊ000000
	.Password645[3] = {0x00,0x00,0x00,0x00 },
	.Password645[4] = {0x11, 0x11, 0x11, 0x11 },				//����Ա04�����������ʼ����Ϊ111111
	.Password698 = { 0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},	//698�����ʼ��Ϊȫ0 asics��
};

const WORD TimeBroadCastTimeMinLimit = 60; 						//�㲥Уʱ��С��Ӧ���� ��λ��
const WORD TimeBroadCastTimeMaxLimit = 300;						//�㲥Уʱ�����Ӧ���� ��λ��

const DWORD	IRTimeConst = 30;               //������֤ʱ��
const BYTE	ScretKeyNum = 35;               //��Կ����
const BYTE	LadderNumConst = 0;             //������
#if(PREPAY_MODE == PREPAY_LOCAL)
const DWORD	TickLimitConst = 1000;          //͸֧�������
const DWORD	RegrateLimitConst = 99999999;   //�ڻ��������
const DWORD	CloseLimitConst = 200;          //��բ����������
#endif

//�����־ TRUE:����       FALSE:������
const BOOL  RelayKeepPowerFlag = FALSE;

#if( SEL_DLT645_2007 == YES )
const DWORD	Remote645AuthTimeConst = 30;
const BYTE  Report645ResetTimeConst = 30;   //645�ϱ���ʱ
//�������������1�Ƿ����ø����ϱ� NO�������� YES:����
//�������������1����������λ��lcd��ʾ�����ͼ̵������ƣ�������698ʹ��
const BOOL  Meter645FollowStatusConst = FALSE;    
#endif
#if( cCURR_TYPES == CURR_60A )
	//���ѹ ascii��     4
	const BYTE RateVoltageConst[6] = {"220V"};
	//����� ascii��     5
	const BYTE RateCurrentConst[6] = {"5A"};
	//������ ascii��     6
	const BYTE MaxCurrentConst[6] = {"60A"};
	const BYTE MinCurrentConst[10] = {5,'0','.','2','5','A'};//��С���� ascii�� 
	const BYTE TurnCurrentConst[10] = {4,'0','.','5','A'};//ת�۵��� ascii�� 
#elif( cCURR_TYPES == CURR_100A )
	//���ѹ ascii��     4
	const BYTE RateVoltageConst[6] = {"220V"};
	//����� ascii��     5
	const BYTE RateCurrentConst[6] = {"10A"};
	//������ ascii��     6
	const BYTE MaxCurrentConst[6] = {"100A"};
	const BYTE MinCurrentConst[10] = {4,'0','.','5','A'};//��С���� ascii�� 
	const BYTE TurnCurrentConst[10] = {2,'1','A'};//ת�۵��� ascii�� 
#else
	"���ô���"��
#endif
	
//�й����ȵȼ� ascii�� 7
#if(PPRECISION_TYPE == PPRECISION_A )
const BYTE PPrecisionConst[4] = {"A"};
#elif(PPRECISION_TYPE == PPRECISION_C )
const BYTE PPrecisionConst[4] = {"C"};
#elif(PPRECISION_TYPE == PPRECISION_D )
const BYTE PPrecisionConst[4] = {"D"};
#else
const BYTE PPrecisionConst[4] = {"B"};
#endif

//�޹����ȵȼ� ascii�� 8
const BYTE QPrecisionConst[4] = {""};
//����ͺ� ascii��     11
const BYTE MeterModelConst[32] = {"DDZYXXX"};
//698.45Э��汾��(��������:WORD)   13
const WORD ProtocolVersionConst = 0x0016;
//645Э��汾��(��������:ASCII)   
const BYTE ProtocolVersionConst_645[16] = {"DL/T645-2007-14"};
//698Э��汾��(��������:ASCII)   
const BYTE ProtocolVersionConst_698[21] = {20,'D','L','/','T','6','9','8','.','4','5','-','2','0','1','7','-','2','0','2','0'};//��ʽΪlen+����
//��������汾�� ascii�� 16
const BYTE SoftWareVersionConst[4] = {"XXXX"};//{"V1.0"};
//��������汾����
const BYTE SoftWareDateConst[6] = {"XXXXXX"};//{"210504"};
//����Ӳ���汾�� ascii�� 17
const BYTE HardWareVersionConst[4] = {"XXXX"};//{"V1.0"};
//����Ӳ���汾����
const BYTE HardWareDateConst[6] = {"XXXXXX"};//{"210504"};
//���ұ��  ascii��       18
const BYTE FactoryCodeConst[4] = {"XXXX"};
//��������� ASCII��      19
const BYTE SoftRecordConst[16] = {'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A'};
//Ĭ�ϵ�ǰ����ʱ��
const BYTE DefCurrRatioConst = 3;//ƽ
//-----------------------------------------------
//			Rtc ʱ��Ĭ�ϲ���
//-----------------------------------------------
#if( cOSCILATOR_TYPE == OSCILATOR_XTC )
//HT602xK+�����ǣ�
const unsigned short TAB_DFx_K[10] =
{
	0x0000, 0x0000,
	0x007F, 0xD9B3,
	0x007E, 0xCDDF,
	0x0000, 0x44D4,
	0x007F, 0xF220
};
#elif(cOSCILATOR_TYPE == OSCILATOR_XTC_3215)
//HT602xK+������3215��װ��
const unsigned short TAB_DFx_K[10] =
{
	0x0000, 0x0000,
	0x007F, 0xD61E,	//-10722
	0x007E, 0xDCF2,	//-74510
	0x0000, 0x4BE5,	//19429
	0x0000, 0x0437	//1079
};
#elif(cOSCILATOR_TYPE == OSCILATOR_JG)
//HT602xK+������
const unsigned short TAB_DFx_K[10] =
{
	0x0000, 0x0000,
	0x007F, 0xDa4b,
	0x007E, 0xD9ac,
	0x0000, 0x4a2e,
	0x007F, 0xfc90
}; 

#elif(cOSCILATOR_TYPE == OSCILATOR_JG_3215)
//HT602xK+����3215��
const unsigned short TAB_DFx_K[10] =
{
	0x0000, 0x0000,
	0x007F, 0xE173,//-7821
	0x007E, 0xFA1B,//-67045
	0x0000, 0x601E,//24606
	0x0000, 0x2563//9571
}; 
//����ϵ��ѡ���ֹʹ��#else
#endif

const signed short TAB_Temperature[] =
{
	22495,	4638,			//-50C,0C
	//-720,	-7862,			//+15C,+35C
	-720,	-10000,			//+15C,+35C
	-13220,	-31077,			//+50C,+100C
};

//-----------------------------------------------
//			�弶����
//-----------------------------------------------
//CHIP_E2_ST_HARD: ST E2Ӳ����ַ
//CHIP_E2_ST_SOFT: ST E2�����ַ
//SelEEDoubleErrReportConst��YES��ƬEE�����ϲ��ϱ���NOһƬEE���Ͼ��ϱ�
#if(BOARD_TYPE == BOARD_HT_SINGLE_78201602)	//9600�ı�Ӳ����ַ
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_HARD, NO}; 
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_78202201)	//115200�ı�Ϊ�����ַ
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_SOFT, NO};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_78202202)	//�߷����ı�Ϊ�����ַ
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_SOFT, NO};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_78202303)	//115200 ̼Ĥ���� ˫E2 128
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_FM_SOFT, CHIP_E2_BL_HARD};
	const BYTE SelEEDoubleErrReportConst = YES;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_78202401)	//115200 ̼Ĥ���� ˫E2 128
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_FM_SOFT, CHIP_E2_BL_HARD};
	const BYTE SelEEDoubleErrReportConst = YES;
#elif(BOARD_TYPE == BOARD_HT_SINGLE_20250819)	//����ѿ����ܵ��ܱ��綯���г�����Զ�ʶ�𣩳��汾
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_SOFT, CHIP_E2_ST_SOFT};
	const BYTE SelEEDoubleErrReportConst = NO;
#endif
//-----------------------------------------------
//			cpuĬ�ϲ���
//-----------------------------------------------
//HT5025KĬ�ϲ���
const long ADCINcoffConst_K = 2547;
const long ADCINOffsetConst_K = 39170;
const long VBatcoffConst_K = 17695;
const long VBatOffsetConst_K = -249270;
const long VcccoffConst_K = 17734;
const long VccOffsetConst_K = -1176300;
const long TPScoffConst_K = -282;
const long TPSOffsetConst_K = 1298520;

//�ȴ�OSC������ʱ
const WORD WaitOSCRunTime = 97;
//-----------------------------------------------
//			LowPower �͹���Ĭ�ϲ���
//-----------------------------------------------

//-----------------------------------------------
//			SysWatch ����Ĭ�ϲ���
//-----------------------------------------------
const WORD ReadBatteryStandardVoltage = 640;
const WORD ClockBatteryStandardVoltage = 367;

//-----------------------------------------------
//			Relay �̵���Ĭ�ϲ���
//-----------------------------------------------
#if( SEL_DLT645_2007 == YES )
const WORD	wRelayWaitOffTime_Def = 0;		//��բ��ʱʱ�䣨���ӣ�
#endif

#if( cCURR_TYPES == CURR_60A )
const WORD	OverIProtectTime_Def = 1440;	//���������ޱ�����ʱʱ�� HEX ��λ������
const DWORD	RelayProtectI_Def = 300000;		//�̵�����բ��������ֵ HEX ��λ��A������-4	
#else
const WORD	OverIProtectTime_Def = 0;    //���������ޱ�����ʱʱ�� HEX ��λ������
const DWORD	RelayProtectI_Def = 0;     	 //�̵�����բ��������ֵ HEX ��λ��A������-4
#endif//#if( cCURR_TYPES == CURR_60A )

#if( cRELAY_TYPE == RELAY_INSIDE )
	const BYTE	RelayCtrlMode_Def = 0;			//�̵�������������� 0--���� 1--����
#else
	const BYTE	RelayCtrlMode_Def = 1;			//�̵�������������� 0--���� 1--����
#endif

//-----------------------------------------------
//			Event �¼�Ĭ�ϲ���
//-----------------------------------------------

//���� 0x3005 IMax��������3λС������������ֵҪ��4λС��
#if( SEL_EVENT_OVER_I == YES )
const BYTE	EventOverITimeConst = 60;   //�¼���ʱʱ�� 60s
const BYTE  EventOverIIRateConst = 12; //1.2Imax,�����¼������������޶�ֵ��Χ�� 0.5��1.5Imax����С�趨ֵ���� 0.0001A
#endif


//���� 0x3011
#if( SEL_EVENT_LOST_POWER == YES )
const BYTE	EventLostPowerTimeConst = 0; //�¼���ʱʱ�� 60s
#endif

//����оƬ���� 0x302F
#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
const BYTE	EventSampleChipErrTimeConst = 60;   //�¼���ʱʱ�� 60s
#endif
//���ܱ����ߵ����쳣 0x3040
#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
const BYTE	 EventNeutralCurrentErrTimeConst = 60; //�¼���ʱʱ�� 60s
const DWORD  EventNeutralCurrentErrLimitIConst = 20000; //20%;�����������ޣ���С�趨ֵ���� 0.001%
const WORD   EventNeutralCurrentErrRatioConst = 5000; //50%,���ߵ����쳣���޷�Χ�� 10%��98%����С�趨ֵ���� 0.01%
#endif
//-----------------------------------------------
//			Report �¼�Ĭ�ϲ���
//-----------------------------------------------

//Ĭ�ϲ���������ϱ� 4300	�����豸 ������7. ��������ϱ�
const BYTE	MeterReportFlagConst = FALSE;
//Ĭ�������ϱ���ʶ 4300	�����豸 ������8. ���������ϱ�
const BYTE	MeterActiveReportFlagConst = TRUE;

//Ĭ�ϸ����ϱ�״̬�� �ϱ���ʽ 0�������ϱ�  1�������ϱ�
const BYTE	MeterReportStatusFlagConst = 1;
//Ĭ���¼��ϱ���ʽΪ�����ϱ����б�
const BYTE	ReportActiveMethodConst[15+1] = 
{
	4,
	eEVENT_LOST_POWER_NO,		//�����¼�
	eEVENT_METERCOVER_NO,		//������¼�
	eEVENT_OVER_I_NO,			//�����¼�
	eEVENT_RTC_ERR_NO,			//ʱ�ӹ���
};
//�����ϱ�ģʽ��
const BYTE	FollowReportModeConst[4] = 
{   
    0x00,//bit24~bit31
    0x00,//bit16~bit23
    0x00,//bit8~bit15       bit8ͣ�糭����Ƿѹ��bit9͸֧״̬��bit14��բ�ɹ���bit15��բ�ɹ�
    0x00,//bit0~bit7        bit1ESAM����bit3ʱ�ӵ�ص�ѹ�͡�bit5ʱ�ӹ���
};
//�����ϱ�ͨ��;
const BYTE	FollowReportChannelConst[MAX_COM_CHANNEL_NUM+1] = 
{
	1,			//��һ���ֽڱ�ʾ����
	eCR,//ģ��ͨ��
};
//eRS485_I,//��һ·485
//eIR,//����ͨ��
//eCR,//ģ��ͨ��
//eRS485_II,//�ڶ�·485
//����!!!!!!
// ͨ��OAD��eSERIAL_TYPE��˳��һ��
const DWORD ChannelOAD[] =
{
	0x010201F2,     //eRS485_I
	0x000202F2,     //eIR
	0x000209F2,     //eCR
	0x020201F2,     //eRS485_II
};

//�˴�˳��Ҫ�� eEVENT_INDEX �����˳��һ�£��������С��eNUM_OF_EVENT_PRG����һ��
//���ϱ���0�����¼������ϱ���BIT0�����¼��ָ��ϱ���BIT1�����¼������ָ����ϱ���BIT0|BIT1��
const TReportModeConst	ReportModeConst[15+1] =
{
	//�¼�ö�ٺ� �ϱ�ģʽ
	4, 4,
	eEVENT_LOST_POWER_NO,	(BIT0|BIT1),		//�����¼�
	eEVENT_METERCOVER_NO,	BIT0,		//������¼�
	eEVENT_OVER_I_NO,		BIT0,		//�����¼�
	eEVENT_RTC_ERR_NO,		BIT0,		//ʱ�ӹ���
	
};

//���ã���Ҫ�޸�
const Fun FunctionConst @"PRG_CONST_B" = api_DelayNop;
const BYTE a_byDefaultData[]@"PRG_CONST_C"= 
{
	0X40, 0X00, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //���Ե�1��	40000200--��ǰ����
	0X40, 0X00, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //���Ե�2��	40000200--��ǰʱ��
	0X00, 0X00, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�3��	00000201--��ǰ����й��ܵ���
	0X00, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�4��	00100201--��ǰ�����й��ܵ���
	0X00, 0X10, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�5��	00100202--��ǰ�����й������
	0X00, 0X10, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�6��	00100203--��ǰ�����й������
	0X00, 0X10, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�7��	00100204--��ǰ�����й�ƽ����
	0X00, 0X10, 0X02, 0X05, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�8��	00100205--��ǰ�����й��ȵ���
	0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //���Ե�9��	10100201--��ǰ�����й����������
	0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //���Ե�10��	10100201--��ǰ�����й������������������
	0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X03, //���Ե�11��	10100201--��ǰ�����й��������������ʱ��
	0X00, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�12��	00200201--��ǰ�����й��ܵ���
	0X00, 0X20, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�13��	00200202--��ǰ�����й������
	0X00, 0X20, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�14��	00200203--��ǰ�����й������
	0X00, 0X20, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�15��	00200204--��ǰ�����й�ƽ����
	0X00, 0X20, 0X02, 0X05, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�16��	00200205--��ǰ�����й��ȵ���
	0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //���Ե�17��	10200201--��ǰ�����й����������
	0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //���Ե�18��	10200201--��ǰ�����й������������������
	0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X03, //���Ե�19��	10200201--��ǰ�����й��������������ʱ��
	0X00, 0X30, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�20��	00300201--��ǰ����޹�1�ܵ���
	0X00, 0X40, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�21��	00400201--��ǰ����޹�2�ܵ���
	0X00, 0X50, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�22��	00500201--��ǰ��1�����޹��ܵ���
	0X00, 0X60, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�23��	00600201--��ǰ��2�����޹��ܵ���
	0X00, 0X70, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�24��	00700201--��ǰ��3�����޹��ܵ���
	0X00, 0X80, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�25��	00800201--��ǰ��4�����޹��ܵ���
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X01, 0x02, 0X00, //���Ե�26��	50050201 00100201--��1�������й��ܵ���
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X02, 0x02, 0X00, //���Ե�27��	50050201 00100202--��1�������й������
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X03, 0x02, 0X00, //���Ե�28��	50050201 00100203--��1�������й������
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X04, 0x02, 0X00, //���Ե�29��	50050201 00100204--��1�������й�ƽ����
	0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X05, 0x02, 0X00, //���Ե�30��	50050201 00100205--��1�������й��ȵ���
	0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X01, //���Ե�31��	50050201 10100201--��1�������й����������
	0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X02, //���Ե�32��	50050201 10100201--��1�������й������������������
	0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X03, //���Ե�33��	50050201 10100201--��1�������й��������������ʱ��
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X01, 0x02, 0X00, //���Ե�34��	50050201 00200201--��1�·����й��ܵ���
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X02, 0x02, 0X00, //���Ե�35��	50050201 00200202--��1�·����й������
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X03, 0x02, 0X00, //���Ե�36��	50050201 00200203--��1�·����й������
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X04, 0x02, 0X00, //���Ե�37��	50050201 00200204--��1�·����й�ƽ����
	0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X05, 0x02, 0X00, //���Ե�38��	50050201 00200205--��1�·����й��ȵ���
	0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X01, //���Ե�39��	50050201 10200201--��1�·����й����������
	0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X02, //���Ե�40��	50050201 10200201--��1�·����й������������������
	0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X03, //���Ե�41��	50050201 10200201--��1�·����й��������������ʱ��
	0X50, 0X05, 0X02, 0X01, 0X00, 0X50, 0X02, 0X01, 0x02, 0X00, //���Ե�42��	50050201 00500201--��1�µ�1�����޹��ܵ���
	0X50, 0X05, 0X02, 0X01, 0X00, 0X60, 0X02, 0X01, 0x02, 0X00, //���Ե�43��	50050201 00600201--��1�µ�2�����޹��ܵ���
	0X50, 0X05, 0X02, 0X01, 0X00, 0X70, 0X02, 0X01, 0x02, 0X00, //���Ե�44��	50050201 00700201--��1�µ�3�����޹��ܵ���
	0X50, 0X05, 0X02, 0X01, 0X00, 0X80, 0X02, 0X01, 0x02, 0X00, //���Ե�45��	50050201 00800201--��1�µ�4�����޹��ܵ���
	0X40, 0X01, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //���Ե�46��	40010200--ͨ�ŵ�ַ��8λ
	0X40, 0X01, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //���Ե�47��	40010200--ͨ�ŵ�ַ��4λ
	0XF2, 0X01, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //���Ե�48��	F2010201--ͨ�Ų�����
	0X41, 0X09, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�49��	41090200--�й����峣��
	0X41, 0X0A, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�50��	410A0200--�޹����峣��
	0X20, 0X13, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�51��	20130200--ʱ�ӵ��ʹ��ʱ��
	0X30, 0X12, 0X02, 0X01, 0X20, 0X1e, 0X02, 0X00, 0x02, 0X01, //���Ե�52��	30120201 33020202--���һ�α������
	0X30, 0X12, 0X02, 0X01, 0X20, 0X1e, 0X02, 0X00, 0x02, 0X02, //���Ե�53��	30120201 33020202--���һ�α��ʱ��
	0X30, 0X00, 0X0D, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�54��	30000D01--��ʧѹ������������������12
	0X30, 0X00, 0X0D, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�55��	30000D02--��ʧѹ�ۼ�ʱ�䡢������������12
	0X30, 0X00, 0X0D, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //���Ե�56��	30000D03--���һ��ʧѹ��ʼ����
	0X30, 0X00, 0X0D, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //���Ե�57��	30000D03--���һ��ʧѹ��ʼʱ��
	0X30, 0X00, 0X0D, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01, //���Ե�58��	30000D04--���һ��ʧѹ��������
	0X30, 0X00, 0X0D, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02, //���Ե�59��	30000D04--���һ��ʧѹ����ʱ��
	0X30, 0X00, 0X07, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00, //���Ե�60��	30000701 00102201--���һ��A��ʧѹ��ʼʱ�������й�����
	0X30, 0X00, 0X07, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00, //���Ե�61��	30000701 00108201--���һ��A��ʧѹ����ʱ�������й�����
	0X30, 0X00, 0X07, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00, //���Ե�62��	30000701 00202201--���һ��A��ʧѹ��ʼʱ�̷����й�����
	0X30, 0X00, 0X07, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00, //���Ե�63��	30000701 00208201--���һ��A��ʧѹ����ʱ�̷����й�����
	0X30, 0X00, 0X08, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00, //���Ե�64��	30000801 00102201--���һ��B��ʧѹ��ʼʱ�������й�����
	0X30, 0X00, 0X08, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00, //���Ե�65��	30000801 00108201--���һ��B��ʧѹ����ʱ�������й�����
	0X30, 0X00, 0X08, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00, //���Ե�66��	30000801 00202201--���һ��B��ʧѹ��ʼʱ�̷����й�����
	0X30, 0X00, 0X08, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00, //���Ե�67��	30000801 00208201--���һ��B��ʧѹ����ʱ�̷����й�����
	0X30, 0X00, 0X09, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00, //���Ե�68��	30000901 00102201--���һ��C��ʧѹ��ʼʱ�������й�����
	0X30, 0X00, 0X09, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00, //���Ե�69��	30000901 00108201--���һ��C��ʧѹ����ʱ�������й�����
	0X30, 0X00, 0X09, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00, //���Ե�70��	30000901 00202201--���һ��C��ʧѹ��ʼʱ�̷����й�������
	0X30, 0X00, 0X09, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00, //���Ե�71��	30000901 00208201--���һ��C��ʧѹ����ʱ�̷����й�����
	0X20, 0X00, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�72��	20000201--A���ѹ
	0X20, 0X00, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�73��	20000202--B���ѹ
	0X20, 0X00, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�74��	20000203--C���ѹ
	0X20, 0X01, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�75��	20010201--A�����
	0X20, 0X01, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�76��	20010202--B�����
	0X20, 0X01, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�77��	20010203--C�����
	0X20, 0X04, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�78��	20040201--˲ʱ���й�����
	0X20, 0X04, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�79��	20040202--˲ʱA���й�����
	0X20, 0X04, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�80��	20040203--˲ʱB���й�����
	0X20, 0X04, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�81��	20040204--˲ʱC���й�����
	0X20, 0X0A, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�82��	200A0201--˲ʱ�ܹ�������
	0X20, 0X0A, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�83��	200A0202--˲ʱA�๦������
	0X20, 0X0A, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�84��	200A0203--˲ʱB�๦������
	0X20, 0X0A, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�85��	200A0204--˲ʱC�๦������	
	0X41, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00, //���Ե�86��	41160201--������
};                                                        	
#endif//#if( MAX_PHASE == 1 )