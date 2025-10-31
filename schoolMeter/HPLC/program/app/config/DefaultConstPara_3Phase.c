//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.20
//����		������
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

#if( MAX_PHASE == 3 )

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
#if( cMETER_TYPES == METER_ZT )//20�淶�����ֻ��ֱͨ��֧�����ߵ�������
const BYTE SelZeroCurrentConst = YES;//���ߵ������
#else//�������ߵ��û�����ߵ���
const BYTE SelZeroCurrentConst = NO;//���ߵ������
#endif
#if(cRELAY_TYPE == RELAY_NO)//����ֻ�����ܱ��и�����Դ
const BYTE SelSecPowerConst = YES;
#else
const BYTE SelSecPowerConst = NO;
#endif
const BYTE SelOscTypeConst = cOSCILATOR_TYPE;//ѡ��������
//---������ֵ��䲻�����޸�end------

//------�������ÿɸ�������޸�------
const WORD SoftVersionConst = 0xff4B;   //ǰ��λ����ʡ�ݣ�����λ�����꣬����λ�����б���Ͳ��·�
const BYTE SelDivAdjust = YES;			//�Ƿ���÷ֶ�У׼����  ����һֱΪYES
#if((BOARD_TYPE == BOARD_HT_THREE_0132515)&&(PPRECISION_TYPE == PPRECISION_D))
const BYTE SelMChipVrefgain = NO;		//�Ƿ�������оƬ����Ƭ���²�
#else
const BYTE SelMChipVrefgain = YES;		//�Ƿ�������оƬ����Ƭ���²�
#endif
const BYTE SelVolUnbalanceGain = YES; 	//�Ƿ�����ѹ��ƽ�ⲹ��  ---����ʱNO�ر�----����YES����
const BYTE ESAMPowerControlConst = YES;	//�ϵ�ESAM��Դ����ʱ�䣬---����ʱNO�ر� ��Դ����120s----����YES����--��Դ����3s
const BYTE SmallCurrentCorrection = YES;//��������ر�
const BYTE SelVolChangeGain = YES;//��ѹ�ı䲹������������ر�

#if( cMETER_TYPES == METER_ZT )
const BYTE HalfWaveDefCheckTime = 0xFF;				//ֱ��ż��г��Ĭ�ϼ��ʱ��(����һֱ����--0xFF  ����24Сʱ)
#else
const BYTE HalfWaveDefCheckTime = 0x00;				//ֱ��ż��г��Ĭ�ϼ��ʱ��(һֱ�ر�)
#endif

//------���ÿɸ�������޸�end------


//-----------------------------------------------
//			protocol ���ò���
//-----------------------------------------------
const BYTE WATCH_SCI_TIMER = 65;//7 �����ֶ�������������� wlk 2009-7-16
//���ܳ�ʱʱ�䣬��λ�����룬Ҳ������500�������û�н��ܵ��µ����ݣ�������ܻ���
const WORD MAX_RX_OVER_TIME = 500/10;//��ʱ����10ms
//���յ����͵���ʱ������Ϊ��λ�����յ���Ч֡����ʱ��ʱ���ڷ��ͣ���Ϊ�������ķ�ʱ�䣩
const WORD RECE_TO_SEND_DELAY = 25/10+1;//��ʱ����10ms

//-----------------------------------------------
//			Sample ����Ĭ�ϲ���
//-----------------------------------------------

//���ߵ�������оƬ��ѹ����ֵ
//�������� * ��ͭ��ֵ * 1000(��λmv)
const WORD ZeroSampleCurrConst = 1000;

//ȡ���ܽŵ�ѹ ��λmv
//��ѹֵ ��λ0.01mv 9��169k 1��33k 1��1k	(100000/1555)*10
#if( cMETER_TYPES == METER_ZT )	
	#if( cCURR_TYPES == CURR_60A )	
		const WORD wMeterBasicCurrentConst = 5000;//����������3λС��
		const DWORD dwMeterMaxCurrentConst = 60000; //��������3λС��
		const WORD SimpleCurrConst = 20;
		const WORD IRegionConst = 500;      //500%Ib
		const WORD wSampleCurGainConst = 1;	//����оƬ�������汶��
		const WORD SimpleVolConst = 721;		
		const WORD wSampleVolGainConst = 2;	//����оƬ��ѹ���汶��
	#elif( cCURR_TYPES == CURR_100A )
		const WORD wMeterBasicCurrentConst = 10000; //����������3λС��
		const DWORD dwMeterMaxCurrentConst = 100000; //��������3λС��
		const WORD SimpleCurrConst = 40;
		const WORD IRegionConst = 500;      //500%Ib
		const WORD wSampleCurGainConst = 1;	//����оƬ�������汶��
		const WORD SimpleVolConst = 721;		
		const WORD wSampleVolGainConst = 2;	//����оƬ��ѹ���汶��
		
	#endif		
		const WORD wStandardVoltageConst = 2200; //������ѹ��1λС��
		const WORD wCaliVoltageConst = 2200; //У���ѹ��1λС��
#elif( cMETER_TYPES == METER_3P3W )	
	#if( cCURR_TYPES == CURR_6A )	
		const WORD wMeterBasicCurrentConst = 1500; //����������3λС��
		const DWORD dwMeterMaxCurrentConst = 6000; //��������3λС��
		#if( ( BOARD_TYPE == BOARD_HT_THREE_0130347 ) && (cRELAY_TYPE == RELAY_NO))	
		const WORD SimpleCurrConst = 94;
		#elif( ( BOARD_TYPE == BOARD_HT_THREE_0132515 ) && (cRELAY_TYPE == RELAY_NO))	
		const WORD SimpleCurrConst = 94;
		#else	
		const WORD SimpleCurrConst = 51;
		#endif
		const WORD IRegionConst = 60;       //60%Ib
		const WORD wSampleCurGainConst = 1;	//����оƬ�������汶��
		const WORD SimpleVolConst = 1730;		
		const WORD wSampleVolGainConst = 2;	//����оƬ��ѹ���汶��
		
	#elif( cCURR_TYPES == CURR_1A )
		const WORD wMeterBasicCurrentConst = 300; //����������3λС��
		const DWORD dwMeterMaxCurrentConst = 1200; //��������3λС��
		#if( ( BOARD_TYPE == BOARD_HT_THREE_0130347 ) && (cRELAY_TYPE == RELAY_NO))	
		const WORD SimpleCurrConst = 40;
		#elif( ( BOARD_TYPE == BOARD_HT_THREE_0132515 ) && (cRELAY_TYPE == RELAY_NO))	
		const WORD SimpleCurrConst = 40;
		#else
		const WORD SimpleCurrConst = 20;
		#endif
		const WORD IRegionConst = 80;       //80%Ib
		const WORD wSampleCurGainConst = 2;	//����оƬ�������汶��
		const WORD SimpleVolConst = 1730;		
		const WORD wSampleVolGainConst = 2;	//����оƬ��ѹ���汶��
	#endif
		const WORD wStandardVoltageConst = 1000; //������ѹ��1λС��
		const WORD wCaliVoltageConst = 1000; //У���ѹ��1λС��
#else//( cMETER_TYPES == METER_3P4W )
	//�����2018-0719֮ǰ��57.7v�ѿر� ��ȷ���Ƿ���ҪSimpleVolConst��Ϊ154,wSampleVolGainConst��Ϊ8		!!!
	#if( cCURR_TYPES == CURR_6A )
		const WORD wMeterBasicCurrentConst = 1500; //����������3λС��
		const DWORD dwMeterMaxCurrentConst = 6000; //��������3λС��
		#if( ( BOARD_TYPE == BOARD_HT_THREE_0130347 ) && (cRELAY_TYPE == RELAY_NO))
		const WORD SimpleCurrConst = 94;
	#elif( ( BOARD_TYPE == BOARD_HT_THREE_0132515 ) && (cRELAY_TYPE == RELAY_NO))	
	const WORD SimpleCurrConst = 94;
		#else
		const WORD SimpleCurrConst = 51;
		#endif
		const WORD IRegionConst = 60;		//60%Ib
		const WORD wSampleCurGainConst = 1;	//����оƬ�������汶��
	#elif( cCURR_TYPES == CURR_1A )
		const WORD wMeterBasicCurrentConst = 300; //����������3λС��
		const DWORD dwMeterMaxCurrentConst = 1200; //��������3λС��
		#if( ( BOARD_TYPE == BOARD_HT_THREE_0130347 ) && (cRELAY_TYPE == RELAY_NO))
		const WORD SimpleCurrConst = 40;
	#elif( ( BOARD_TYPE == BOARD_HT_THREE_0132515 ) && (cRELAY_TYPE == RELAY_NO))	
	const WORD SimpleCurrConst = 40;
		#else
		const WORD SimpleCurrConst = 20;
		#endif
		const WORD IRegionConst = 80;		//80%Ib
		const WORD wSampleCurGainConst = 2;	//����оƬ�������汶��
	#endif
	
	#if(cMETER_VOLTAGE == METER_220V)
		const WORD SimpleVolConst = 721; //721;
		const WORD wSampleVolGainConst = 2; //2;	//����оƬ��ѹ���汶��
		const WORD wStandardVoltageConst = 2200; //������ѹ��1λС��
		const WORD wCaliVoltageConst = 2200; //У���ѹ��1λС��
	#else
		const WORD SimpleVolConst = 3355; //721;
		const WORD wSampleVolGainConst = 2; //2;	//����оƬ��ѹ���汶��
		const WORD wStandardVoltageConst = 577; //������ѹ��1λС��
		const WORD wCaliVoltageConst = 600; //У���ѹ��1λС��
	#endif
#endif

const WORD wSampleMinCurGainConst = 8; //����оƬ�������汶��

//����оƬ������Ĵ�����ַ
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
//����оƬ������Ĵ�����ַ
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
//����оƬƫ����Ĵ�����ַ
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
//����оƬ������Ĵ���Ĭ������
const WORD SampleCtrlDefData[15] = 
{
	//ֱͨ�������ߵ�������,SelZeroCurrentConst == YES
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

//����оƬ������Ĵ���Ĭ������
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
//����оƬƫ����Ĵ���Ĭ������
const WORD SampleOffsetDefData[20] = 
{
	0x0000,0x0000,0x0000,	//POffset[3]
	0x0000,0x0000,0x0000,	//POffsetL[3]
	0x0000,0x0000,0x0000,	//QOffset[3]
	0x0000,0x0000,0x0000,	//QOffsetL[3]
	0x0000,0x0000,0x0000,	//URMSOffset[3]
	0x0007,0x0007,0x0007,	//IRMSOffset[3]
	0x0005,0x0000,			//����ʸ����ƫ�� ���ߵ���ƫ��
};

//ֱ��ż��г��Ĭ�ϲ���ϵ��
#if( cCURR_TYPES == CURR_100A )
const WORD SampleHalfWaveDefData[12] =
{
	0xF74C, 0x0036,			//Coe[0] -- ��λ����ֵ, ����ֵ(1λС��)
	0xF594, 0x0189,			//Coe[1] -- ��λ����ֵ, ����ֵ(1λС��)
	0xF29D, 0x0287,			//Coe[2] -- ��λ����ֵ, ����ֵ(1λС��)
	0x018D, 0x0036,			//Gain[0]-- ���油��ֵ, ����ֵ(1λС��)
	0x0252, 0x0188,			//Gain[1]-- ���油��ֵ, ����ֵ(1λС��)
	0x040E, 0x0286,			//Gain[2]-- ���油��ֵ, ����ֵ(1λС��)
};
#else//60A
const WORD SampleHalfWaveDefData[12] =
{
	0xF5F4, 0x0020,			//Coe[0] -- ��λ����ֵ, ����ֵ(1λС��)
	0xF407, 0x00E8,			//Coe[1] -- ��λ����ֵ, ����ֵ(1λС��)
	0xF2B5, 0x0164,			//Coe[2] -- ��λ����ֵ, ����ֵ(1λС��)
	0x0000, 0x0000,			//Gain[0]-- ���油��ֵ, ����ֵ(1λС��)
	0x0000, 0x0000,			//Gain[1]-- ���油��ֵ, ����ֵ(1λС��)
	0x0000, 0x0000,			//Gain[2]-- ���油��ֵ, ����ֵ(1λС��)
};
#endif
//�����𵴲�����Ĭ��ϵ��
const WORD CosGainSwtichData = 0xFF8E;

//��ѹ�ı䲹��Ĭ��ϵ��
const WORD SampleVolChangeData = 0x0001;

//оƬ�¶Ȳ�������Ĭ��ϵ��
const WORD SampleTCcoffDefData[3] =
{
	0xFF00,	//0xFF11,	//TCcoffA-0x6D
	0x0DB8,	//0x2B53,	//TCcoffB-0x6E
	0xD1DA,	//0xD483,	//TCcoffC-0x6F
};
//��B���йصĲ�����Ĵ�����ַ
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

//ֱ��ż��г������ֵ
#if( cCURR_TYPES == CURR_60A )
const WORD HD2CheckCurrentValueMin = 10000;			//����ֱ��ż��г����������Сֵ(1A)
const WORD HD2CosLimitValue = 9000;					//ֱ��ż��г��1.0L����ʱ�����С�ж�ֵ
const WORD HD2CurrentDiffValue = 25000;				//��һ�β�������ֵ�뵱ǰ����ֵ�Ĳ�ֵ
const BYTE HD2ContentMin = 25;						//�벨�ж���г��������Сֵ
const BYTE HD2ContentMax = 55;						//�벨�ж���г���������ֵ
const BYTE HD2DiffValue = 8;						//��ǰ����г������һ�ζ���г����ֵ
const BYTE FirstDiffValue = 10;						//��ǰ��������һ�λ�����ֵ
const WORD HD2CurrentRangeMin = 10;					//ֱ��ż��г������������Сֵ1A
const WORD HD2CurrentRangeMax = 500;				//ֱ��ż��г�������������ֵ50A
#else//100A
const WORD HD2CheckCurrentValueMin = 20000;			//����ֱ��ż��г����������Сֵ(2A)
const WORD HD2CosLimitValue = 9000;					//ֱ��ż��г��1.0L����ʱ�����С�ж�ֵ
const WORD HD2CurrentDiffValue = 25000;				//��һ�β�������ֵ�뵱ǰ����ֵ�Ĳ�ֵ
const BYTE HD2ContentMin = 25;						//�벨�ж���г��������Сֵ
const BYTE HD2ContentMax = 55;						//�벨�ж���г���������ֵ
const BYTE HD2DiffValue = 8;						//��ǰ����г������һ�ζ���г����ֵ
const BYTE FirstDiffValue = 10;						//��ǰ��������һ�λ�����ֵ
const WORD HD2CurrentRangeMin = 20;					//ֱ��ż��г������������Сֵ2A
const WORD HD2CurrentRangeMax = 1000;				//ֱ��ż��г�������������ֵ100A
#endif

//-----------------------------------------------
//			LCD ��ʾĬ�ϲ���
//-----------------------------------------------

const TLCDPara LCDParaDef = 
{ 
	.PowerOnDispTimer = 5 , 		//�ϵ�ȫ��ʱ��  unsigned��
	.BackLightPressKeyTimer = 60 , 	//�������ʱ��  long-unsigned(����ʱ�������ʱ��)
	.BackLightViewTimer = 10 , 		//��ʾ�鿴�������ʱ��  long-unsigned��
	.LcdSwitchTime = 30 , 			//�޵簴����Ļפ�����ʱ��  long-unsigned�� ����ʱ�䣬HEX ��λΪ��
	.EnergyFloat = 2 , 				//����С����
	.DemandFloat = 4 , 				//���ʣ�������С����
	.Meaning12 = 0,					//��ʾ12��������
#if(PREPAY_MODE == PREPAY_LOCAL)//���ر�
	.DispItemNum = {19,97},			//ѭ�ԣ�����
#elif(cRELAY_TYPE == RELAY_NO)	//���ܱ�
	.DispItemNum = {20,86},			//ѭ�ԣ�����
#else							//Զ�̷ѿر�
	.DispItemNum = {18,84},			//ѭ�ԣ�����
#endif
	.KeyDisplayTime = 0x003C,		//�����л���ѭ����ʾ��ʱ�� ��ʾ������3����
	.LoopDisplayTime = 0x0005,		//ѭ����ʾʱÿ������ʾʱ�� ��ʾ������3����
};

//20�淶���Ĭ�ϲ��������⣬С��������Ĭ���뱳�ⱨ��һ��
#if(cRELAY_TYPE != RELAY_NO )
const TWarnDispPara WarnDispParaConst = 
{  	  
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 0 -- �����̵���
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 1 -- LCDС�����������Ʋ���
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 2 -- Һ������
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 3 -- Һ��ERR��ʾ����
};
#else
//08 01 21 00 03 00 00 00
const TWarnDispPara WarnDispParaConst = 
{  	  
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 0 -- �����̵���
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 1 -- LCDС�����������Ʋ���
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 2 -- Һ������
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 3 -- Һ��ERR��ʾ����
};
#endif


//ѭ����ĿĬ������(35��) 0-��Ĭ����ʾ�� 1~199����LcdItemTable������кţ�200~249����ExpandLcdItemTable������кţ�fe-����ܳ�ʼ���� ͨ����Լ����
const BYTE LoopItemDef[35]=                    
{
	1,  //ѭ�Ե�1��	40000200--��ǰ����
	2,  //ѭ�Ե�2��	40000200--��ǰʱ��
#if(PREPAY_MODE == PREPAY_LOCAL)
	3,  //ѭ�Ե�3��	202c0201--��ǰʣ����
#endif
	149,//ѭ��4�� 00000401--��ǰ����й��ܸ߾��ȵ���
	150,//ѭ�Ե�5�� 00100401--��ǰ�����й��ܸ߾��ȵ���
	151,//ѭ�Ե�6�� 00100402--��ǰ�����й���߾��ȵ���
	152,//ѭ�Ե�7�� 00100403--��ǰ�����й���߾��ȵ���
	153,//ѭ�Ե�8�� 00100404--��ǰ�����й�ƽ�߾��ȵ���
	154,//ѭ�Ե�9�� 00100405--��ǰ�����й��ȸ߾��ȵ���
	10, //ѭ�Ե�10��	10100201--��ǰ�����й����������
#if(cRELAY_TYPE == RELAY_NO)	
	160,//ѭ�Ե�11�� 00300401--��ǰ����޹�1�ܸ߾��ȵ���
	161,//ѭ�Ե�12�� 00400401--��ǰ����޹�2�ܸ߾��ȵ���
#endif
	162,//ѭ�Ե�13�� 00500401--��ǰ��һ�����޹��ܸ߾��ȵ���
	163,//ѭ�Ե�14�� 00600401--��ǰ�ڶ������޹��ܸ߾��ȵ���
	164,//ѭ�Ե�15�� 00700401--��ǰ���������޹��ܸ߾��ȵ���
	165,//ѭ�Ե�16�� 00800401--��ǰ���������޹��ܸ߾��ȵ���
	155,//ѭ�Ե�17�� 00200401--��ǰ�����й��ܸ߾��ȵ���
	156,//ѭ�Ե�18�� 00200402--��ǰ�����й���߾��ȵ���
	157,//ѭ�Ե�19�� 00200403--��ǰ�����й���߾��ȵ���
	158,//ѭ�Ե�20�� 00200404--��ǰ�����й�ƽ�߾��ȵ���
	159,//ѭ�Ե�21�� 00200405--��ǰ�����й��ȸ߾��ȵ���
};


//������ĿĬ������(110��) 0-��Ĭ����ʾ�� 1~199����LcdItemTable������кţ�200~249����ExpandLcdItemTable������кţ�fe-����ܳ�ʼ���� ͨ����Լ����
const BYTE KeyItemDef[110] =                
{
	1,//���Ե�1��	40000200--��ǰ����
	2,//���Ե�2��	40000200--��ǰʱ��
#if(PREPAY_MODE == PREPAY_LOCAL)
	3,//���Ե�3��	202c0201--��ǰʣ����
#endif
	149,//����4�� 00000401--��ǰ����й��ܸ߾��ȵ���
	150,//���Ե�5�� 00100401--��ǰ�����й��ܸ߾��ȵ���
	151,//���Ե�6�� 00100402--��ǰ�����й���߾��ȵ���
	152,//���Ե�7�� 00100403--��ǰ�����й���߾��ȵ���
	153,//���Ե�8�� 00100404--��ǰ�����й�ƽ�߾��ȵ���
	154,//���Ե�9�� 00100405--��ǰ�����й��ȸ߾��ȵ���
	10, //���Ե�10��	10100201--��ǰ�����й����������
	11, //���Ե�11��	10100201--��ǰ�����й������������������
	12, //���Ե�12��	10100201--��ǰ�����й��������������ʱ��
	155,//���Ե�13�� 00200401--��ǰ�����й��ܸ߾��ȵ���
	156,//���Ե�14�� 00200402--��ǰ�����й���߾��ȵ���
	157,//���Ե�15�� 00200403--��ǰ�����й���߾��ȵ���
	158,//���Ե�16�� 00200404--��ǰ�����й�ƽ�߾��ȵ���
	159,//���Ե�17�� 00200405--��ǰ�����й��ȸ߾��ȵ���
	18, //���Ե�18��	10200201--��ǰ�����й����������
	19, //���Ե�19��	10200201--��ǰ�����й������������������
	20, //���Ե�20��	10200201--��ǰ�����й��������������ʱ��
#if(cRELAY_TYPE == RELAY_NO)
	160,//ѭ�Ե�21�� 00300401--��ǰ����޹�1�ܸ߾��ȵ���
	161,//ѭ�Ե�22�� 00400401--��ǰ����޹�2�ܸ߾��ȵ���
#endif
	162,//���Ե�23�� 00500401--��ǰ��һ�����޹��ܸ߾��ȵ���
	163,//���Ե�24�� 00600401--��ǰ�ڶ������޹��ܸ߾��ȵ���
	164,//���Ե�25�� 00700401--��ǰ���������޹��ܸ߾��ȵ���
	165,//���Ե�26�� 00800401--��ǰ���������޹��ܸ߾��ȵ���
	167,//���Ե�27�� 50050201 00100401--��1�������й��ܸ߾��ȵ���
	168,//���Ե�28�� 50050201 00100402--��1�������й���߾��ȵ���
	169,//���Ե�29�� 50050201 00100403--��1�������й���߾��ȵ���
	170,//���Ե�30�� 50050201 00100404--��1�������й�ƽ�߾��ȵ���
	171,//���Ե�31�� 50050201 00100405--��1�������й��ȸ߾��ȵ���
	32, //���Ե�32��	50050201 10100201--��1�������й����������
	33, //���Ե�33��	50050201 10100201--��1�������й������������������
	34, //���Ե�34��	50050201 10100201--��1�������й��������������ʱ��
	172,//���Ե�35�� 50050201 00200401--��1�·����й��ܸ߾��ȵ���
	173,//���Ե�36�� 50050201 00200402--��1�·����й���߾��ȵ���
	174,//���Ե�37�� 50050201 00200403--��1�·����й���߾��ȵ���
	175,//���Ե�38�� 50050201 00200404--��1�·����й�ƽ�߾��ȵ���
	176,//���Ե�39�� 50050201 00200405--��1�·����й��ȸ߾��ȵ���
	40, //���Ե�40��	50050201 10200201--��1�·����й����������
	41, //���Ե�41��	50050201 10200201--��1�·����й������������������
	42, //���Ե�42��	50050201 10200201--��1�·����й��������������ʱ��
	179,//���Ե�43�� 50050201 00500401--��1�µ�һ�����޹��ܸ߾��ȵ���
	180,//���Ե�44�� 50050201 00600401--��1�µڶ������޹��ܸ߾��ȵ���
	181,//���Ե�45�� 50050201 00700401--��1�µ��������޹��ܸ߾��ȵ���
	182,//���Ե�46�� 50050201 00800401--��1�µ��������޹��ܸ߾��ȵ���
	47, //���Ե�47��	40010200--ͨ�ŵ�ַ��8λ
	48, //���Ե�48��	40010200--ͨ�ŵ�ַ��4λ
	49, //���Ե�49��	F2010201--ͨ�Ų�����
	50, //���Ե�50��	41090200--�й����峣��
	51, //���Ե�51��	410A0200--�޹����峣��
	52, //���Ե�52��	20130200--ʱ�ӵ��ʹ��ʱ��
	53, //���Ե�53��	30120201 33020202--���һ�α������
	54, //���Ե�54��	30120201 33020202--���һ�α��ʱ��
	55, //���Ե�55��	30000D01--��ʧѹ������������������12
	56, //���Ե�56��	30000D02--��ʧѹ�ۼ�ʱ�䡢������������12
	57, //���Ե�57��	30000D03--���һ��ʧѹ��ʼ����
	58, //���Ե�58��	30000D03--���һ��ʧѹ��ʼʱ��
	59, //���Ե�59��	30000D04--���һ��ʧѹ��������
	60, //���Ե�60��	30000D04--���һ��ʧѹ����ʱ��
	61, //���Ե�61��	30000701 00102201--���һ��A��ʧѹ��ʼʱ�������й�����
	62, //���Ե�62��	30000701 00108201--���һ��A��ʧѹ����ʱ�������й�����
	63, //���Ե�63��	30000701 00202201--���һ��A��ʧѹ��ʼʱ�̷����й�����
	64, //���Ե�64��	30000701 00208201--���һ��A��ʧѹ����ʱ�̷����й�����
	65, //���Ե�65��	30000801 00102201--���һ��B��ʧѹ��ʼʱ�������й�����
	66, //���Ե�66��	30000801 00108201--���һ��B��ʧѹ����ʱ�������й�����
	67, //���Ե�67��	30000801 00202201--���һ��B��ʧѹ��ʼʱ�̷����й�����
	68, //���Ե�68��	30000801 00208201--���һ��B��ʧѹ����ʱ�̷����й�����
	69, //���Ե�69��	30000901 00102201--���һ��C��ʧѹ��ʼʱ�������й�����
	70, //���Ե�70��	30000901 00108201--���һ��C��ʧѹ����ʱ�������й�����
	71, //���Ե�71��	30000901 00202201--���һ��C��ʧѹ��ʼʱ�̷����й�����
	72, //���Ե�72��	30000901 00208201--���һ��C��ʧѹ����ʱ�̷����й�����
	73, //���Ե�73��	20000201--A���ѹ
	74, //���Ե�74��	20000202--B���ѹ
	75, //���Ե�75��	20000203--C���ѹ
	76, //���Ե�76��	20010201--A�����
	77, //���Ե�77��	20010202--B�����
	78, //���Ե�78��	20010203--C�����
	79, //���Ե�79��	20040201--˲ʱ���й�����
	80, //���Ե�80��	20040202--˲ʱA���й�����
	81, //���Ե�81��	20040203--˲ʱB���й�����
	82, //���Ե�82��	20040204--˲ʱC���й�����
	83, //���Ե�83��	200A0201--˲ʱ�ܹ�������
	84, //���Ե�84��	200A0202--˲ʱA�๦������
	85, //���Ե�85��	200A0203--˲ʱB�๦������
	86, //���Ե�86��	200A0204--˲ʱC�๦������		
#if(PREPAY_MODE == PREPAY_LOCAL)
	87, //���Ե�87��	40180201--��ǰ����ʵ��
	88, //���Ե�88��	40180202--��ǰ����ʵ��
	89, //���Ե�89��	40180203--��ǰƽ���ʵ��
	90, //���Ե�90��	40180204--��ǰ�ȷ��ʵ��
	91, //���Ե�91��	401A0202--����1���
	92, //���Ե�92��	401A0202--����2���
	93, //���Ե�93��	401A0202--����3���
	94, //���Ե�94��	401A0202--����4���
	95, //���Ե�94��	201A0200--��ǰ���
	96, //���Ե�96��	401E0201--�������1
	97, //���Ե�97��	401E0202--�������2
	98, //���Ե�98��	401F0201--͸֧���
#endif
	99, //���Ե�99��	41160201--������
};
//��չ���Ŵ�200��ʼ
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
//			Freeze����Ĭ�ϲ���
//-----------------------------------------------
// ˲ʱ����
// Ĭ��������Ч��3����
const TFreezePara InstantFreeze[MAX_INSTANT_FREEZE_ATTRIBUTE+1]=
{
	{12,0x0000000c,	12},		//�������Ը���
	{0, 0x00100400, 3 }, 		//�����й�����					20  
	{0, 0x00200400, 3 }, 		//�����й�����					20
	{0, 0x00300400, 3 }, 		//����޹�1���� 				20
	{0, 0x00400400, 3 }, 		//����޹�2���� 			  	20	  
	{0, 0x00500400, 3 }, 		//��һ�����޹�����				20 	 
	{0, 0x00600400, 3 }, 		//�ڶ������޹�����				20 	 
	{0, 0x00700400, 3 }, 		//���������޹�����				20 	 
	{0, 0x00800400, 3 }, 		//���������޹�����				20 	 
	{0, 0x10100200, 3 }, 		//�����й��������������ʱ��	55 			   
	{0, 0x10200200, 3 }, 		//�����й��������������ʱ��	55 			   
	{0, 0x20040200, 3 }, 		//�й�����					   	16
	{0, 0x20050200, 3 }, 		//�޹�����					   	16
};

// ���Ӷ���
// 28800����   ���ں���Ⱦ���������Ϊ0
const TFreezePara MinFreeze[MAX_MINUTE_FREEZE_ATTRIBUTE+1]=
{
	#if(cMETER_TYPES == METER_ZT)
	{ 17,0x00000011,	17},				    //�������Ը���
	#else
	{ 15,0x0000000F,	15},					//�������Ը���
	#endif
	
	{ 15, 0x00100401, MAX_FREEZE_MIN_DEPTH }, // �����й��ܵ���
	{ 15, 0x00200401, MAX_FREEZE_MIN_DEPTH }, // �����й��ܵ���
	{ 15, 0x00300401, MAX_FREEZE_MIN_DEPTH }, // ����޹�1�ܵ���
	{ 15, 0x00400401, MAX_FREEZE_MIN_DEPTH }, // ����޹�2�ܵ���
	{ 15, 0x00500401, MAX_FREEZE_MIN_DEPTH }, // ��һ�����޹��ܵ���
	{ 15, 0x00600401, MAX_FREEZE_MIN_DEPTH }, // �ڶ������޹��ܵ���
	{ 15, 0x00700401, MAX_FREEZE_MIN_DEPTH }, // ���������޹��ܵ���
	{ 15, 0x00800401, MAX_FREEZE_MIN_DEPTH }, // ���������޹��ܵ���
	{ 15, 0x20170200, MAX_FREEZE_MIN_DEPTH }, // �й�����
	{ 15, 0x20180200, MAX_FREEZE_MIN_DEPTH }, // �޹�����
	{ 15, 0x20000200, MAX_FREEZE_MIN_DEPTH }, // ��ѹ
	{ 15, 0x20010200, MAX_FREEZE_MIN_DEPTH }, // ����
	#if(cMETER_TYPES == METER_ZT)
	{ 15, 0x20010400, MAX_FREEZE_MIN_DEPTH }, // ���ߵ���
	{ 15, 0x20010600, MAX_FREEZE_MIN_DEPTH }, // �������
	#endif
	{ 15, 0x20040200, MAX_FREEZE_MIN_DEPTH }, // �й�����
	{ 15, 0x20050200, MAX_FREEZE_MIN_DEPTH }, // �޹�����
	{ 15, 0x200A0200, MAX_FREEZE_MIN_DEPTH }, // ��������
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
	{23, 0x00000017,	23},					//�������Ը���
	#else
	{21, 0x00000015,	21},					//�������Ը���
	#endif
	{1,  0x00100400, 365 },// �����й�����
	{1,  0x00110400, 365 },// A�������й�����				
	{1,  0x00120400, 365 },// B�������й�����				
	{1,  0x00130400, 365 },// C�������й�����				
	{1,  0x00200400, 365 },// �����й�����
	{1,  0x00210400, 365 },// A�෴���й�����				
	{1,  0x00220400, 365 },// B�෴���й�����				
	{1,  0x00230400, 365 },// C�෴���й�����		
	{1,  0x00300400, 365 },// ����޹�1����
	{1,  0x00400400, 365 },// ����޹�1����
	{1,  0x00500400, 365 },// ��һ�����޹�����
	{1,  0x00600400, 365 },// �ڶ������޹�����
	{1,  0x00700400, 365 },// ���������޹�����
	{1,  0x00800400, 365 },// ���������޹�����
	{1,  0x10100200, 365 },// �����й��������������ʱ��
	{1,  0x10200200, 365 },// �����й��������������ʱ��
	{1,  0x20040200, 365 },// �й�����
	{1,  0x20050200, 365 },// �޹�����
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{1,	 0x202c0200, 365 },// ʣ����������
	{1,	 0x202d0200, 365 },// ͸֧���
	#endif
	{1,  0x21310201, 365 },// A���ѹ�ϸ���
	{1,  0x21320201, 365 },// B���ѹ�ϸ���
	{1,  0x21330201, 365 } // C���ѹ�ϸ���
};

// �����ն���
// Ĭ��������Ч��12����
const TFreezePara ClosingFreeze[MAX_CLOSING_FREEZE_ATTRIBUTE+1]=
{
	{15, 0x0000000f,15},//�������Ը���
	{1, 0x00000400, 12 },//����й�����
	{1, 0x00100400, 12 },//�����й�����
	{1, 0x00200400, 12 },//�����й�����
	{1, 0x00300400, 12 },//����޹�1���� 			  	
	{1, 0x00400400, 12 },//����޹�2���� 			  	
	{1, 0x00500400, 12 },//��һ�����޹�����			   	  
	{1, 0x00600400, 12 },//�ڶ������޹�����				
	{1, 0x00700400, 12 },//���������޹�����				
	{1, 0x00800400, 12 },//���������޹�����				
	{1, 0x00110400, 12 },//A�������й�����
	{1, 0x00120400, 12 },//B�������й�����
	{1, 0x00130400, 12 },//C�������й�����
	{1, 0x10100200, 12 },//�����й��������������ʱ��	55 	   
	{1, 0x10200200, 12 },//�����й��������������ʱ��	55
	{1, 0x20310200, 12 },//�¶��õ���					4	 
};

// �¶���
// Ĭ������1�£�12����
const TFreezePara MonFreeze[MAX_MON_FREEZE_ATTRIBUTE+1]=
{
	{20, 0x00000014,20},	//�������Ը���
	{1, 0x00000400, 24 },	//����й�����					20
	{1, 0x00100400, 24 },	//�����й�����				   	20
	{1, 0x00110400, 24 },	//A�������й�����				20
	{1, 0x00120400, 24 },	//B�������й�����				20
	{1, 0x00130400, 24 },	//C�������й�����				20
	{1, 0x00200400, 24 },	//�����й�����				 	20
	{1, 0x00210400, 24 },	//A�෴���й�����			   	20 
	{1, 0x00220400, 24 },	//B�෴���й�����				20
	{1, 0x00230400, 24 },	//C�෴���й�����				20
	{1, 0x00300400, 24 },	//����޹�1���� 			  	20
	{1, 0x00400400, 24 },	//����޹�2���� 			  	20
	{1, 0x00500400, 24 },	//��һ�����޹�����			   	20  
	{1, 0x00600400, 24 },	//�ڶ������޹�����				20
	{1, 0x00700400, 24 },	//���������޹�����				20
	{1, 0x00800400, 24 },	//���������޹�����				20
	{1, 0x10100200, 24 },	//�����й��������������ʱ��	55 	   
	{1, 0x10200200, 24 },	//�����й��������������ʱ��	55 
	{1, 0x21310202, 24 },	//--A���ѹ�ϸ���			   	16
	{1, 0x21320202, 24 },	//--B���ѹ�ϸ���			   	16
	{1, 0x21330202, 24 } 	//--C���ѹ�ϸ���			   	16
};

// ʱ�����л�����
// Ĭ��������Ч��2����
const TFreezePara TZChgFreeze[MAX_TIME_ZONE_FREEZE_ATTRIBUTE+1]=
{
	{12, 0x0000000c, 12},		//�������Ը��� 
	{1,  0x00100400,  2 }, 	   //�����й�����				20
	{1,  0x00200400,  2 }, 	   //�����й�����				20
	{1,  0x00300400,  2 }, 	   //����޹�1����				20
	{1,  0x00400400,  2 }, 	   //����޹�2����				20
	{1,  0x00500400,  2 }, 	   //��һ�����޹�����			20
	{1,  0x00600400,  2 }, 	   //�ڶ������޹�����			20
	{1,  0x00700400,  2 }, 	   //���������޹�����			20
	{1,  0x00800400,  2 }, 	   //���������޹�����			20
	{1,  0x10100200,  2 }, 	   //�����й��������������ʱ��	55
	{1,  0x10200200,  2 }, 	   //�����й��������������ʱ��	55
	{1,  0x20040200,  2 }, 	   //�й�����					16
	{1,  0x20050200,  2 }, 	   //�޹�����					16
};

// ��ʱ�α��л�����
// Ĭ��������Ч��2����
const TFreezePara DTTChgFreeze[MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE+1]=
{
	{12, 0x0000000c, 12},		//�������Ը��� 
	{1,  0x00100400,  2 },		//�����й�����					20
	{1,  0x00200400,  2 },		//�����й�����					20
	{1,  0x00300400,  2 },		//����޹�1����					20
	{1,  0x00400400,  2 },		//����޹�2����					20
	{1,  0x00500400,  2 },		//��һ�����޹�����				20
	{1,  0x00600400,  2 },		//�ڶ������޹�����				20
	{1,  0x00700400,  2 },		//���������޹�����				20
	{1,  0x00800400,  2 },		//���������޹�����				20
	{1,  0x10100200,  2 },		//�����й��������������ʱ��	55
	{1,  0x10200200,  2 },		//�����й��������������ʱ��	55
	{1,  0x20040200,  2 },		//�й�����						16
	{1,  0x20050200,  2 },		//�޹�����						16
};

#if(PREPAY_MODE == PREPAY_LOCAL)
// ���ʵ���л�����
// Ĭ��������Ч��2����
const TFreezePara TariffRateChgFreeze[MAX_TARIFF_RATE_FREEZE_ATTRIBUTE+1]=
{
	{12, 0x0000000c, 12},		//�������Ը��� 
	{1,  0x00100400,  2 },		//�����й�����					20
	{1,  0x00200400,  2 },		//�����й�����					20
	{1,  0x00300400,  2 },		//����޹�1����					20
	{1,  0x00400400,  2 },		//����޹�2����					20
	{1,  0x00500400,  2 },		//��һ�����޹�����				20
	{1,  0x00600400,  2 },		//�ڶ������޹�����				20
	{1,  0x00700400,  2 },		//���������޹�����				20
	{1,  0x00800400,  2 },		//���������޹�����				20
	{1,  0x10100200,  2 },		//�����й��������������ʱ��	55
	{1,  0x10200200,  2 },		//�����й��������������ʱ��	55
	{1,  0x20040200,  2 },		//�й�����						16
	{1,  0x20050200,  2 },		//�޹�����						16
};


// �����л�����
// Ĭ��������Ч��2����
const TFreezePara LadderChgFreeze[MAX_LADDER_FREEZE_ATTRIBUTE+1]=
{
	{12, 0x0000000c, 12},		//�������Ը��� 
	{1,  0x00100400,  2 },		//�����й�����					20
	{1,  0x00200400,  2 },		//�����й�����					20
	{1,  0x00300400,  2 },		//����޹�1����					20
	{1,  0x00400400,  2 },		//����޹�2����					20
	{1,  0x00500400,  2 },		//��һ�����޹�����				20
	{1,  0x00600400,  2 },		//�ڶ������޹�����				20
	{1,  0x00700400,  2 },		//���������޹�����				20
	{1,  0x00800400,  2 },		//���������޹�����				20
	{1,  0x10100200,  2 },		//�����й��������������ʱ��	55
	{1,  0x10200200,  2 },		//�����й��������������ʱ��	55
	{1,  0x20040200,  2 },		//�й�����						16
	{1,  0x20050200,  2 },		//�޹�����						16
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

#if( cMETER_TYPES == METER_ZT )		
	const BYTE RateVoltageConst[6] = {"220V"};		//���ѹ ascii��     4
	#if( cCURR_TYPES == CURR_60A )
		const BYTE RateCurrentConst[6] = {"5A"};	//����� ascii��     5
		const BYTE MaxCurrentConst[6] = {"60A"};	//������ ascii��     6
		const BYTE MinCurrentConst[10] = {4,'0','.','2','A'};	//��С���� ascii��     7
		const BYTE TurnCurrentConst[10] = {4,'0','.','5','A'};	//ת�۵��� ascii��     8
		const DWORD ActiveConstantConst = 1000;		//�й����峣��    	11
	#elif( cCURR_TYPES == CURR_100A )
		const BYTE RateCurrentConst[6] = {"10A"};	//����� ascii��     5
		const BYTE MaxCurrentConst[6] = {"100A"};	//������ ascii��     6
		const BYTE MinCurrentConst[10] = {4,'0','.','4','A'};	//��С���� ascii��     7
		const BYTE TurnCurrentConst[10] = {2,'1','A'};	//ת�۵��� ascii��     8
		const DWORD ActiveConstantConst = 500;		//�й����峣��         11
	#else
		"���ô���"��
	#endif		
#elif( cMETER_TYPES == METER_3P3W )	
	const BYTE RateVoltageConst[6] = {"100V"};		//���ѹ ascii��     4	
	#if( cCURR_TYPES == CURR_6A )
		const BYTE RateCurrentConst[6] = {"1.5A"};	//����� ascii��     5
		const BYTE MaxCurrentConst[6] = {"6A"};		//������ ascii��     6
		const BYTE MinCurrentConst[10] = {6,'0','.','0','1','5','A'};//��С���� ascii��  7
		const BYTE TurnCurrentConst[10] = {6,'0','.','0','7','5','A'};//ת�۵��� ascii�� 8
		const DWORD ActiveConstantConst = 20000L;	//�й����峣��    	11
	#elif( cCURR_TYPES == CURR_1A )
		const BYTE RateCurrentConst[6] = {"0.3A"};	//����� ascii��     5
		const BYTE MaxCurrentConst[6] = {"1.2A"};	//������ ascii��     6
		const BYTE MinCurrentConst[10] = {6,'0','.','0','0','3','A'};//��С���� ascii�� 7
		const BYTE TurnCurrentConst[10] = {6,'0','.','0','1','5','A'};//ת�۵��� ascii��8
		const DWORD ActiveConstantConst = 100000L;	//�й����峣��     	11
	#else
		"���ô���"��
	#endif		
#else//( cMETER_TYPES == METER_3P4W )		
	#if(cMETER_VOLTAGE==METER_220V)
	const BYTE RateVoltageConst[6] = { "220V" };     //���ѹ ascii��     4
	#else
	const BYTE RateVoltageConst[6] = { "57.7V" };     //���ѹ ascii��     4
	#endif
			
	#if( cCURR_TYPES == CURR_6A )
		const BYTE RateCurrentConst[6] = {"1.5A"};	//����� ascii��     5
		const BYTE MaxCurrentConst[6] = {"6A"};		//������ ascii��     6
		const BYTE MinCurrentConst[10] = {6,'0','.','0','1','5','A'};//��С���� ascii��  7
		const BYTE TurnCurrentConst[10] = {6,'0','.','0','7','5','A'};//ת�۵��� ascii�� 8
		//57v��Ӧ20000,220v��ӦActiveConstantConst220V 6400
		#if(cMETER_VOLTAGE==METER_220V)
		const DWORD ActiveConstantConst = 10000L;    //�й����峣��    	11
		#else
		const DWORD ActiveConstantConst = 20000L;	//�й����峣��         11
		#endif
	#elif( cCURR_TYPES == CURR_1A )
		const BYTE RateCurrentConst[6] = {"0.3A"};	//����� ascii��     5
		const BYTE MaxCurrentConst[6] = {"1.2A"};	//������ ascii��     6
		const BYTE MinCurrentConst[10] = {6,'0','.','0','0','3','A'};//��С���� ascii��  7
		const BYTE TurnCurrentConst[10] = {6,'0','.','0','1','5','A'};//ת�۵��� ascii�� 8
		//57v��Ӧ100000,220v��Ӧ30000
		#if(cMETER_VOLTAGE==METER_220V)
		const DWORD ActiveConstantConst = 40000L;    //�й����峣��         11
		#else
		const DWORD ActiveConstantConst = 100000L;   //�й����峣��         11
		#endif
	#else
		"���ô���"��
	#endif		
#endif

const TEnereyDemandMode EnereyDemandModeConst =
{
	//�й���Ϸ�ʽ������ ����й�
	.byActiveCalMode = 0x05,
	
	#if(SEL_RACTIVE_ENERGY == YES)
	//�����޹�ģʽ������
	.PReactiveMode = 0x41,
	//�����޹�ģʽ������
	.NReactiveMode = 0x14,
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
#if (( BOARD_TYPE != BOARD_HT_THREE_0130347 )&&( BOARD_TYPE != BOARD_HT_THREE_0132515 ))
// 46: ������9600; 4A: 115200
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
	.I485 = 0x4A,       	//��һ·485
	.ComModule = 0x4A,  	//ģ��
	.ComModuleDeflt = 0x4A,	//ģ��ָ�ȱʡֵ
	.II485 = 0x4A,      	//�ڶ�·485�����಻�ã�
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
const DWORD	Remote645AuthTimeConst = 30;    //Զ����֤ʱ��
const BYTE  Report645ResetTimeConst = 30;   //645�ϱ���ʱ
//�������������1�Ƿ����ø����ϱ� NO�������� YES:����
//�������������1����������λ��lcd��ʾ�����ͼ̵������ƣ�������698ʹ��
const BOOL  Meter645FollowStatusConst = FALSE; 
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
const BYTE QPrecisionConst[4] = {"2"};
#if( cMETER_TYPES != METER_3P3W )
	#if(cRELAY_TYPE == RELAY_NO )
	//����ͺ� ascii��     11
	const BYTE MeterModelConst[32] = {"DTZXXX"};
	#else
		#if( PREPAY_MODE == PREPAY_LOCAL )
		//����ͺ� ascii��     11
		const BYTE MeterModelConst[32] = {"DTZYXXX"};
		#else
		//����ͺ� ascii��     11
		const BYTE MeterModelConst[32] = {"DTZYXXX"};
		#endif
	#endif
#else
	#if(cRELAY_TYPE == RELAY_NO )
	//����ͺ� ascii��     11
	const BYTE MeterModelConst[32] = {"DSZXXX"};
	#else
	//����ͺ� ascii��     11
	const BYTE MeterModelConst[32] = {"DSZYXXX"};
	#endif
#endif
//698.45Э��汾��(��������:WORD)    13
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
//��������� ASCII��      19 0062201904170010
const BYTE SoftRecordConst[16] = {'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A'};
//Ĭ�ϵ�ǰ����ʱ��
const BYTE DefCurrRatioConst = 3; //ƽ������Ϊ0

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
#elif(cOSCILATOR_TYPE == OSCILATOR_JG)
//HT602xK+������
const unsigned short TAB_DFx_K[10] = 		//HT6025HĬ�ϲ���
{
	0x0000, 0x0000,
	0x007F, 0xDa4b,
	0x007E, 0xD9ac,
	0x0000, 0x4a2e,
	0x007F, 0xfc90
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
#if(BOARD_TYPE == BOARD_HT_THREE_0134566)	//��������Ϸ�ʽ+6AС��������Ϊ5ŷ Ӳ����ַ
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_HARD, CHIP_E2_ST_HARD};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_THREE_0131699)	//��������·�ʽ+6AС��������Ϊ5ŷ �����ַ
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_HARD, CHIP_E2_ST_HARD};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_THREE_0130347)	//��������·�ʽ+С��������ӱ�+֧��115200������+��E2
	const BYTE SelEESoftAddrConst[2] = {CHIP_E2_ST_SOFT, CHIP_E2_ST_SOFT};
	const BYTE SelEEDoubleErrReportConst = NO;
#elif(BOARD_TYPE == BOARD_HT_THREE_0132515)	//��������·�ʽ+С��������ӱ�+֧��115200������+��E2
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
const WORD	OverIProtectTime_Def = 1440;	//���������ޱ�����ʱʱ�� HEX ��λ������
const DWORD	RelayProtectI_Def = 300000;		//�̵�����բ��������ֵ HEX ��λ��A������-4	

#if( cRELAY_TYPE == RELAY_INSIDE )
	const BYTE	RelayCtrlMode_Def = 0;			//�̵�������������� 0--���� 1--����
#else
	const BYTE	RelayCtrlMode_Def = 1;			//�̵�������������� 0--���� 1--����
#endif

//-----------------------------------------------
//			Event �¼�Ĭ�ϲ���
//-----------------------------------------------
//ʧѹ 0x3000
#if( SEL_EVENT_LOST_V == YES )
const BYTE	EventLostVTimeConst = 60;	//�¼���ʱʱ�� 60s
const BYTE  EventLostVURateConst = 78;	//78%�αȵ�ѹ,ʧѹ�¼���ѹ�������޶�ֵ��Χ�� 70%��90%�αȵ�ѹ����С�趨ֵ���� 0.1V
const BYTE  EventLostVIRateConst = 5;	//0.5%���������������ʧѹ�¼������������޶�ֵ��Χ�� 0.5%��5%�����������������С�趨ֵ���� 0.1mA
const BYTE  EventLostVRecoverLimitVRateConst = 85;//85%�αȵ�ѹ,ʧѹ�¼���ѹ�ָ����޶�ֵ��Χ�� ʧѹ�¼���ѹ�������ޡ�90%�αȵ�ѹ����С�趨ֵ���� 0.1V
#endif

//Ƿѹ 0x3001
#if( SEL_EVENT_WEAK_V == YES )
const BYTE	EventWeakVTimeConst = 60;   //�¼���ʱʱ�� 60s
const BYTE  EventWeakVURateConst = 78;  //78%�αȵ�ѹ,��ѹ�������޶�ֵ��Χ�� 70%��90%�αȵ�ѹ����С�趨ֵ���� 0.1V
#endif

//��ѹ 0x3002
#if( SEL_EVENT_OVER_V == YES )
const BYTE	EventOverVTimeConst = 60;   //�¼���ʱʱ�� 60s
const BYTE  EventOverVURateConst = 120;	//120%�αȵ�ѹ,��ѹ�¼���ѹ�������޶�ֵ��Χ�� 110%��130%�αȵ�ѹ����С�趨ֵ���� 0.1V
#endif

//���� 0x3003
#if( SEL_EVENT_BREAK == YES )
const BYTE	EventBreakTimeConst = 60;   //�¼���ʱʱ�� 60s
const BYTE  EventBreakURateConst = 60;	//��ѹ��������60%Un
const BYTE  EventBreakIRateConst = 5;	//������������0.5%Ib
#endif

//ʧ�� 0x3004
#if( SEL_EVENT_LOST_I == YES )
const BYTE	EventLostITimeConst = 60;   //�¼���ʱʱ�� 60s
const BYTE  EventLostIURateConst = 70;	//70%,ʧ���¼���ѹ�������޶�ֵ��Χ�� 60%��90%�αȵ�ѹ����С�趨ֵ���� 0.1V
const BYTE  EventLostIIRateConst = 5;	//0.5%�������������,ʧ���¼������������޶�ֵ��Χ�� 0.5%��2%�����������������С�趨ֵ���� 0.1mA
const BYTE  EventLostILimitLIConst = 5;	//5%�������������,ʧ���¼������ָ����޶�ֵ��Χ�� 3%��10%�����������������С�趨ֵ���� 0.1mA
#endif

//���� 0x3005 IMax��������3λС������������ֵҪ��4λС��
#if( SEL_EVENT_OVER_I == YES )
const BYTE	EventOverITimeConst = 60;   //�¼���ʱʱ�� 60s
const BYTE  EventOverIIRateConst = 12; //1.2Imax,�����¼������������޶�ֵ��Χ�� 0.5��1.5Imax����С�趨ֵ���� 0.1mA
#endif

//���� 0x3006
#if( SEL_EVENT_BREAK_I == YES )
const BYTE	EventBreakITimeConst = 60;  //�¼���ʱʱ�� 60s
const BYTE  EventBreakIURateConst = 60;	//60%�αȵ�ѹ�������¼���ѹ�������޶�ֵ��Χ�� 60%��85%�αȵ�ѹ����С�趨ֵ���� 0.1V
const BYTE  EventBreakIIRateConst = 5;	//0.5%�������������,�����¼������������޶�ֵ��Χ�� 0.5%��5%�����������������С�趨ֵ���� 0.1mA
#endif

//���ʷ��� 0x3007
#if( SEL_EVENT_BACKPROP == YES )
const BYTE	EventBackpTimeConst = 60;   //�¼���ʱʱ�� 60s
	#if(( cMETER_VOLTAGE != METER_57V ) || ( cCURR_TYPES != CURR_1A ) )
	const BYTE  EventBackpPRateConst = 5;	//0.5%�����������,�й����ʷ����¼��й����ʴ������޶�ֵ��Χ��0.5%��5%����������ʣ���С�趨ֵ���� 0.0001kW
	#else
	const BYTE  EventBackpPRateConst = 6;	//0.5%�����������,57.7V0.3(1.2)A���ܱ�������0��0.00008KW�������¼��Ͳ��ᴥ��������Ϊ6��λΪ1(0.0001kW)
	#endif
#endif

//���� 0x3008
#if( SEL_EVENT_OVERLOAD == YES )
const BYTE	EventOverLoadTimeConst = 60;    //�¼���ʱʱ�� 60s
const BYTE  EventOverLoadPRateConst = 12;	//1.2Imax*100%Un,�����¼��й����ʴ������޶�ֵ��Χ�� 0.5��1.5Imax ����������ʣ���С�趨ֵ���� 0.0001kW
#endif

//0x3009	���ܱ������й����������¼�
//0x300a	���ܱ����й����������¼�
//0x300b	���ܱ��޹����������¼�
#if( SEL_DEMAND_OVER == YES )
const BYTE	EventDemandOverTimeConst = 60;  //�¼���ʱʱ�� 60s
const BYTE  EventDemandOverRateConst = 12;	//1.2Imax*100%Un,�й����������¼������������޶�ֵ��Χ 0.05��99.99kW����С�趨ֵ���� 0.0001kW
#endif

//������������ 0x303B
#if( SEL_EVENT_COS_OVER == YES )
const BYTE	EventCosOverTimeConst = 60; //�¼���ʱʱ�� 60s
const WORD  EventCosOverRateConst = 300;//0.3,�������������޷�ֵ��ֵ��Χ�� 0.2��0.6�� ��С�趨ֵ���� 0.001
#endif

//ȫʧѹ 0x300d
#if( SEL_EVENT_LOST_ALL_V == YES )
const BYTE	EventLostAllVTimeConst = 60;    //�¼���ʱʱ�� 60s
#endif

//������Դ���� 0x300e
#if( SEL_EVENT_LOST_SECPOWER == YES )
const BYTE	EventLostSecPowerTimeConst = 60;    //�¼���ʱʱ�� 60s
#endif

//��ѹ������ 0x300f
#if( SEL_EVENT_V_REVERSAL == YES )
const BYTE	EventVReversalTimeConst = 60;   //�¼���ʱʱ�� 60s
#endif

//���������� 0x3010
#if( SEL_EVENT_I_REVERSAL == YES )
const BYTE	EventIReversalTimeConst = 60;   //�¼���ʱʱ�� 60s
#endif

//���� 0x3011
#if( SEL_EVENT_LOST_POWER == YES )
const BYTE	EventLostPowerTimeConst = 0;   //�¼���ʱʱ�� 60s
#endif

//��ѹ��ƽ�� 0x301d
#if( SEL_EVENT_V_UNBALANCE == YES )
const BYTE	EventVUnbalanceTimeConst = 60;  //�¼���ʱʱ�� 60s
const WORD  EventVUnbalanceRateConst = 3000;//30%,��ѹ��ƽ������ֵ��ֵ��Χ�� 10%��99%����С�趨ֵ���� 0.01%
#endif

//������ƽ�� 0x301e
#if( SEL_EVENT_I_UNBALANCE == YES )
const BYTE	EventIUnbalanceTimeConst = 60;  //�¼���ʱʱ�� 60s
const WORD  EventIUnbalanceRateConst = 3000; //30%,������ƽ������ֵ��ֵ��Χ�� 10%��90%����С�趨ֵ���� 0.01%
#endif


//�������ز�ƽ�� 0x302d
#if( SEL_EVENT_I_S_UNBALANCE == YES )
const BYTE	EventISUnbalanceTimeConst = 60; //�¼���ʱʱ�� 60s
const WORD  EventISUnbalanceRateConst = 9000; //90%,�������ز�ƽ������ֵ��ֵ��Χ�� 20%��99%����С�趨ֵ���� 0.01%
#endif
//����оƬ���� 0x302F
#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
const BYTE	EventSampleChipErrTimeConst = 60;   //�¼���ʱʱ�� 60s
#endif

//��ѹ�ϸ���0x4030
#if( SEL_STAT_V_RUN == YES )
const BYTE  EventStatVRateConst = 12;			//��ѹ��������	1.2Un
const BYTE  EventStatVRunLimitLVConst = 70;		//��ѹ��������	0.7Un
const BYTE  EventStatVRunRecoverHVConst = 107;	//��ѹ�ϸ�����	1.07Un
const BYTE  EventStatVRunRecoverLVConst = 93;	//��ѹ�ϸ�����	0.93Un
#endif
//���ܱ����ߵ����쳣 0x3040
#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
const BYTE	 EventNeutralCurrentErrTimeConst = 60; //�¼���ʱʱ�� 60s
const DWORD  EventNeutralCurrentErrLimitIConst = 20000; //20%;�����������ޣ���С�趨ֵ���� 0.001%
const WORD   EventNeutralCurrentErrRatioConst = 5000; //50%���ߵ����쳣���޷�Χ�� 10%��98%����С�趨ֵ���� 0.01%
#endif

//-----------------------------------------------
//			Report �¼�Ĭ�ϲ���
//-----------------------------------------------
//			�����ϱ�		�����ϱ�		�ϱ�ͨ��
//ģ���		��ֹ			����			ģ��ͨ�ſ�
//��ģ���		����			��ֹ			����RS485
#if(cRELAY_TYPE == RELAY_NO)
	//Ĭ�ϲ���������ϱ� 4300	�����豸 ������7. ��������ϱ�
	const BYTE	MeterReportFlagConst = TRUE;
	//Ĭ�������ϱ���ʶ 4300	�����豸 ������8. ���������ϱ�
	const BYTE	MeterActiveReportFlagConst = FALSE;

	//Ĭ�ϸ����ϱ�״̬�� �ϱ���ʽ 0�������ϱ�  1�������ϱ�
	const BYTE	MeterReportStatusFlagConst = 1;
	//�����ϱ�ͨ��;
	const BYTE	FollowReportChannelConst[MAX_COM_CHANNEL_NUM+1] = 
	{
		2,			//��һ���ֽڱ�ʾ����
		eRS485_I,//��һ·485
		eRS485_II,//�ڶ�·485
	};
	//eRS485_I,//��һ·485
	//eIR,//����ͨ��
	//eCR,//ģ��ͨ��
	//eRS485_II,//�ڶ�·485
#else
	//Ĭ�ϲ���������ϱ� 4300	�����豸 ������7. ��������ϱ�
	const BYTE	MeterReportFlagConst = FALSE;
	//Ĭ�������ϱ���ʶ 4300	�����豸 ������8. ���������ϱ�
	const BYTE	MeterActiveReportFlagConst = TRUE;

	//Ĭ�ϸ����ϱ�״̬�� �ϱ���ʽ 0�������ϱ�  1�������ϱ�
	const BYTE	MeterReportStatusFlagConst = 1;
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

#endif


// ͨ��OAD��eSERIAL_TYPE��˳��һ��
const DWORD ChannelOAD[] =
{
	0x010201F2,     //eRS485_I
	0x000202F2,     //eIR
	0x000209F2,     //eCR
	0x020201F2,     //eRS485_II
};
//Ĭ���¼��ϱ���ʽΪ�����ϱ����б�
const BYTE	ReportActiveMethodConst[15+1] = 
{
	5,
	eEVENT_LOST_POWER_NO,		//�����¼�
	eEVENT_METERCOVER_NO,		//������¼�
	eEVENT_OVER_I_NO,			//�����¼�
	eEVENT_MAGNETIC_INT_NO,		//�㶨�ų������¼�
	eEVENT_RTC_ERR_NO,			//ʱ�ӹ����¼�
};

//�����ϱ�ģʽ��
const BYTE	FollowReportModeConst[4] = 
{   
    0x00,//bit24~bit31
    0x00,//bit16~bit23
    0x00,//bit8~bit15       bit8ͣ�糭����Ƿѹ��bit9͸֧״̬��bit14��բ�ɹ���bit15��բ�ɹ�
    0x00,//bit0~bit7        bit1ESAM����bit3ʱ�ӵ�ص�ѹ�͡�bit5�洢������
};


//�˴�˳��Ҫ�� eEVENT_INDEX �����˳��һ�£��������С��eNUM_OF_EVENT_PRG����һ��
//���ϱ���0�����¼������ϱ���BIT0�����¼��ָ��ϱ���BIT1�����¼������ָ����ϱ���BIT0|BIT1��
const TReportModeConst	ReportModeConst[15+1] =
#if((cMETER_VOLTAGE == METER_220V) &&(cRELAY_TYPE != RELAY_NO))
{
	//�¼�ö�ٺ� �ϱ�ģʽ
	5, 5,
	eEVENT_LOST_POWER_NO,	(BIT0|BIT1),		//�����¼�
	eEVENT_METERCOVER_NO,	BIT0,		//������¼�
	eEVENT_OVER_I_NO,		BIT0,		//�����¼�
	eEVENT_MAGNETIC_INT_NO,	BIT0,		//�㶨�ų������¼�
	eEVENT_RTC_ERR_NO,		BIT0,		//ʱ�ӹ����¼�
};
#else
{
	//�¼�ö�ٺ� �ϱ�ģʽ
	5, 5,
	eEVENT_LOST_POWER_NO,	BIT1,		//�����¼�
	eEVENT_METERCOVER_NO,	BIT0,		//������¼�
	eEVENT_OVER_I_NO,		BIT0,		//�����¼�
	eEVENT_MAGNETIC_INT_NO,	BIT0,		//�㶨�ų������¼�
	eEVENT_RTC_ERR_NO,		BIT0,		//ʱ�ӹ����¼�
};
#endif

//���ã���Ҫ�޸�
const Fun FunctionConst @"PRG_CONST_B" = api_DelayNop;
const BYTE a_byDefaultData[] @"PRG_CONST_C"= 
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


#endif//#if( MAX_PHASE == 3 )

