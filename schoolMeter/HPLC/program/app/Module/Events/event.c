//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.9.28
//����		�¼�ģ��Դ�ļ�
//�޸ļ�¼.��ȡ������Ŵ�0��ʼ�����ڴ洢��һ�����Ϊ1	
//---------------------------------------------------------------------- 

#include "AllHead.h"

// �����¼���¼
// #if( MAX_EVENT_NUM != 0 )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#if(PREPAY_MODE == PREPAY_LOCAL)
#define EVENT_COLUMN_OAD_MAX_NUM			9					// �̶���OAD������,�쳣�忨��9���̶���
#else
#define EVENT_COLUMN_OAD_MAX_NUM			7					// �̶���OAD������
#endif
#define	EVENT_Q_COMBO_OI					0x3023				// �޹���Ϸ�ʽ���OI
#define	EVENT_PACTIVE_DEMANDOVER_OI			0x3009				// �����й���������
#define	EVENT_NACTIVE_DEMANDOVER_OI			0x300A				// �����й���������
#define	EVENT_REACTIVE_DEMANDOVER_OI		0x300B				// �޹���������
#define EVENT_AHOUR_OI						0X2029				// ��ʱֵ
#define EVENT_I_UNBALANCE_OI				0X301E				// ������ƽ��
#define EVENT_I_S_UNBALANCE_OI				0X302D				// �������ز�ƽ��
#define EVENT_ABR_CARD_OI					0x3027				//���ܱ��쳣�忨�¼�
// ��Թ�Լ����
#define STARTTIME_OAD					0x00021E20			// �¼���ʼʱ���ӦOAD
#define ENDTIME_OAD						0x00022020			// �¼�����ʱ���ӦOAD
#define RECORDNO_OAD					0x00022220			// �¼���¼��Ŷ�ӦOAD
#define GEN_SOURCE_OAD					0x00022420			// �¼�����Դ��ӦOAD
#define NOTIFICATION_OAD				0x00020033			// �¼��ϱ�״̬��ӦOAD
#define	MAXDEMAND_OAD					0X06020833			// �����ڼ��������ֵOAD
#define	MAXDEMAND_TIME_OAD				0X07020833			// �����ڼ��������ֵ����ʱ��OAD
#define PRG_OBJ_LIST_OAD				0X06020233			// ��̶����б�
#define CLR_EVENT_LIST_OAD				0X06020c33			// �¼������б�
#define AHOUR_OAD						0X00022920			// ��ʱֵOAD
#define V_UNBALANCE_OAD					0X00022620			// ��ѹ��ƽ��OAD
#define I_UNBALANCE_OAD					0X00022720			// ������ƽ��OAD
#define PRG_SEGTABLE_OAD				0X06021433			// ʱ�α���OAD
#define PRG_SEGTABLE_VALUE				0X07021433			// ʱ�α���ǰʱ�α�����
#define PRG_HOLIDAY_OAD					0X06021533			// �ڼ��ձ��OAD
#define PRG_HOLIDAY_VALUE				0X07021533			// �ڼ��ձ��ǰ�ڼ�������
#define CARD_SN_OAD						0X06021033			//�쳣�忨�¼���¼��Ԫ0x3310--�����к�	octet-string��
#define CARD_ERR_INFO_OAD				0X07021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨������Ϣ��	unsigned��
#define CARD_ORDER_HEAD_OAD				0X08021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨��������ͷ	octet-string��
#define CARD_ERR_RES_OAD				0X09021033			//�쳣�忨�¼���¼��Ԫ0x3310--�忨������Ӧ״̬  long-unsigned��
#define RETURN_MONEY_OAD				0X06021133			//�˷��¼���¼��Ԫ0x3311--�˷ѽ��      double-long-unsigned����λ��Ԫ�����㣺-2����

#if( SEL_DLT645_2007 == YES )
static const WORD SignEventOI_Table[] = 
{
	0x2001,//����
	0x2004,//�й�����
	0x2005,//�޹�����
	0x2006,//���ڹ���
	0x200A,//��������
	0x0000,//����е���
	0x0030,//������޹�1����
	0x0031,//A������޹�1����
	0x0032,//B������޹�1����
	0x0033,//C������޹�1����
	0x0040,//������޹�2����
	0x0041,//A������޹�2����
	0x0042,//B������޹�2����
	0x0043,//C������޹�2����
};
#endif

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TEventData_t
{
	TDLT698RecordPara 		*pDLT698RecordPara;
	TEventOADCommonData		*pEventOADCommonData;
	TEventAttInfo			*pEventAttInfo;
	TEventAddrLen			*pEventAddrLen;
	TEventSectorInfo 		*pEventSectorInfo;
	BYTE					*pBuf;
	DWORD					dwOad;
	WORD					Len;
	WORD					off;
	BYTE					Tag;
	BYTE					status;
	BYTE					AllorSep;
	BYTE					Last;
}TEventData;

typedef struct TEventOadTable_t
{
	WORD	OI;
	WORD	Number;
	DWORD	OadList[EVENT_COLUMN_OAD_MAX_NUM];
}TEventOadTab;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
TEventPara	gEventPara;	// �¼����޼���ʱ����
TEventStatus EventStatus;
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//���������飬�鿴ÿ���¼�����ţ����԰�ʹ�ã���ʽ�����û�����ô����飬�ᱻ�Ż�����
BYTE eEventTypehch[]=
{
	#if ( SEL_EVENT_LOST_V == YES )
	eEVENT_LOST_V_NO,						// ʧѹ 0
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_WEAK_V == YES )
	eEVENT_WEAK_V_NO,						// Ƿѹ 1
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_OVER_V == YES )
	eEVENT_OVER_V_NO,						// ��ѹ 2
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_BREAK == YES )
	eEVENT_BREAK_NO,						// ���� 3
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_LOST_I == YES )
	eEVENT_LOST_I_NO,						// ʧ�� 4
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_OVER_I == YES )
	eEVENT_OVER_I_NO,						// ���� 5
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_BREAK_I == YES )
	eEVENT_BREAK_I_NO,						// ���� 6
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_BACKPROP == YES )
	eEVENT_BACKPROP_NO,						// ���ʷ��� 7
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_OVERLOAD == YES )
	eEVENT_OVERLOAD_NO,						// ���� 8
	#else
	0xff,
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	eEVENT_PA_DEMAND_OVER_NO,				//�����й����� 9
	eEVENT_NA_DEMAND_OVER_NO,				//�����й����� 10
	eEVENT_RE_DEMAND_OVER_NO,				//�޹�����     11
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	#if( SEL_EVENT_COS_OVER == YES )
	eEVENT_COS_OVER_NO,						// ������������ 12
	#else
	0xff,
	#endif
		
	#if( SEL_EVENT_LOST_ALL_V == YES )
	eEVENT_LOST_ALL_V_NO,					// ȫʧѹ 13
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	eEVENT_LOST_SECPOWER_NO,				//  ������Դʧ�� 14
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_V_REVERSAL == YES )
	eEVENT_V_REVERSAL_NO,					// ��ѹ������ 15
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_I_REVERSAL == YES )
	eEVENT_I_REVERSAL_NO,					// ���������� 16
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_LOST_POWER == YES )
	eEVENT_LOST_POWER_NO,					// ���� 17
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )
	eEVENT_METERCOVER_NO,					// ����� 18
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_BUTTONCOVER == YES )
	eEVENT_BUTTONCOVER_NO,					// ����ť�� 19
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_V_UNBALANCE == YES )
	eEVENT_V_UNBALANCE_NO,					// ��ѹ��ƽ�� 20
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	eEVENT_I_UNBALANCE_NO,					// ������ƽ�� 21
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	eEVENT_MAGNETIC_INT_NO,					// �㶨�ų����� 22
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_RELAY_ERR == YES )
	eEVENT_RELAY_ERR_NO,					// ���ɿ����� 23
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	eEVENT_POWER_ERR_NO,					// ��Դ�쳣 24
	#else
	0xff,
	#endif	
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	eEVENT_I_S_UNBALANCE_NO,				// �������ز�ƽ�� 25
	#else
	0xff,
	#endif

	#if(SEL_EVENT_RTC_ERR == YES)
	eEVENT_RTC_ERR_NO,						//ʱ�ӹ���    26
	#else
	0xff,
	#endif
	
	#if(SEL_EVENT_SAMPLECHIP_ERR == YES)
	eEVENT_SAMPLECHIP_ERR_NO,				// ����оƬ����  27
	#else
	0xff,
	#endif
	
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	eEVENT_NEUTRAL_CURRENT_ERR_NO,                    // ���ܱ����ߵ����쳣
	#else
	0xff,
	#endif
	
	eNUM_OF_EVENT,							// ��������¼����� 28

	// vvvvvvvvvvv������¼vvvvvvvvvvvvvv			
	ePRG_RECORD_NO,				// ���ܱ����¼� 28

	#if( SEL_PRG_INFO_CLEAR_METER == YES )
	ePRG_CLEAR_METER_NO,						// �����¼ 29
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )
	ePRG_CLEAR_MD_NO,							// ��������¼ 30
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
	ePRG_CLEAR_EVENT_NO,						// ���¼���¼ 31
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )
	ePRG_ADJUST_TIME_NO,						// Уʱ��¼ 32
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )
	ePRG_TIME_TABLE_NO,							// ʱ�α��̼�¼ 33
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_TIME_AREA == YES )
	ePRG_TIME_AREA_NO,							// ʱ�����̼�¼ 34
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_WEEK == YES )
	ePRG_WEEKEND_NO,							// �����ձ�̼�¼ 35
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )
	ePRG_CLOSING_DAY_NO,						// �����ձ�̼�¼ 36
	#else
	0xff,
	#endif
	
	ePRG_OPEN_RELAY_NO,							// ��բ 37
	ePRG_CLOSE_RELAY_NO,						// ��բ 38
	
	#if( SEL_PRG_INFO_HOLIDAY == YES )
	ePRG_HOLIDAY_NO,							// �ڼ��ձ�̼�¼ 39
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_P_COMBO == YES )
	ePRG_P_COMBO_NO,							// �й���Ϸ�ʽ��̼�¼ 40
	#else
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )
	ePRG_Q_COMBO_NO,							// �޹���Ϸ�ʽ��̼�¼ 41
	#else
	0xff,
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	ePRG_TARIFF_TABLE_NO,						//���ܱ���ʲ��������¼�
	ePRG_LADDER_TABLE_NO,						// ���ܱ���ݱ����¼�
	#else
	0xff,
	0xff,
	#endif
		
	#if( SEL_PRG_UPDATE_KEY == YES )
	ePRG_UPDATE_KEY_NO,							// ��Կ���¼�¼ 42
	#else
	0xff,
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	ePRG_ABR_CARD_NO,							//���ܱ��쳣�忨�¼�
	ePRG_BUY_MONEY_NO,							//���ܱ����¼
	ePRG_RETURN_MONEY_NO,						//���ܱ��˷Ѽ�¼
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )
	ePRG_BROADJUST_TIME_NO,						// �㲥Уʱ��¼ 43
	#else
	0xff,
	#endif

	
	eNUM_OF_EVENT_PRG,							// �¼��ͱ�������� 44

	// vvvvvvvvvvv֮����¼����洢��¼vvvvvvvvvvvvvv
	#if( SEL_STAT_V_RUN == YES )
	eSTATISTIC_V_PASS_RATE_NO,					//44
	#else
	0xff,
	#endif

	eNUM_OF_EVENT_ALL,							// ���е��¼��������������洢��¼�� 44
};

BYTE eSubEventTypehch[]=
{	//ʧѹ
	#if ( SEL_EVENT_LOST_V == YES )//0
	eSUB_EVENT_LOSTV_A,
	eSUB_EVENT_LOSTV_B,
	eSUB_EVENT_LOSTV_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
		
	//Ƿѹ
	#if( SEL_EVENT_WEAK_V == YES )//3
	eSUB_EVENT_WEAK_V_A,
	eSUB_EVENT_WEAK_V_B,
	eSUB_EVENT_WEAK_V_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//��ѹ
	#if( SEL_EVENT_OVER_V == YES )//6
	eSUB_EVENT_OVER_V_A,
	eSUB_EVENT_OVER_V_B,
	eSUB_EVENT_OVER_V_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//����
	#if( SEL_EVENT_BREAK == YES )//9
	eSUB_EVENT_BREAK_A,
	eSUB_EVENT_BREAK_B,
	eSUB_EVENT_BREAK_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//ʧ��
	#if( SEL_EVENT_LOST_I == YES )//12
	eSUB_EVENT_LOSTI_A,
	eSUB_EVENT_LOSTI_B,
	eSUB_EVENT_LOSTI_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//����
	#if( SEL_EVENT_OVER_I == YES )//15
		#if( MAX_PHASE == 3 )
		eSUB_EVENT_OVER_I_A,
		eSUB_EVENT_OVER_I_B,
		eSUB_EVENT_OVER_I_C,
		#else
		eSUB_EVENT_OVER_I_A,		//���� 0
		#endif
	#else
		#if( MAX_PHASE == 3 )
		0xff,
		0xff,
		0xff,
		#else
		0xff,
		#endif
	#endif
	
	//����
	#if( SEL_EVENT_BREAK_I == YES )//18
	eSUB_EVENT_BREAK_I_A,
	eSUB_EVENT_BREAK_I_B,
	eSUB_EVENT_BREAK_I_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
		
	//���ʷ���
	#if( SEL_EVENT_BACKPROP == YES )//21
	eSUB_EVENT_POW_BACK_ALL,
	eSUB_EVENT_POW_BACK_A,
	eSUB_EVENT_POW_BACK_B,
	eSUB_EVENT_POW_BACK_C,
	#else
	0xff,
	0xff,
	0xff,
	0xff,
	#endif
	
	//����
	#if( SEL_EVENT_OVERLOAD == YES )//25
	eSUB_EVENT_OVERLOAD_A,
	eSUB_EVENT_OVERLOAD_B,
	eSUB_EVENT_OVERLOAD_C,
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	#if(SEL_DEMAND_OVER == YES)//28
	//�����й���������
	eSUB_EVENT_PA_DEMAND_OVER,
	//�����й���������
	eSUB_EVENT_NA_DEMAND_OVER,
	//һ�����й���������
	eSUB_EVENT_QI_DEMAND_OVER,
	//�������й���������
	eSUB_EVENT_QII_DEMAND_OVER,
	//�������й���������
	eSUB_EVENT_QIII_DEMAND_OVER,
	//�������й���������
	eSUB_EVENT_QIV_DEMAND_OVER,
	#else
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	#endif
	
	//������������
	#if( SEL_EVENT_COS_OVER == YES )//34
	eSUB_EVENT_COS_OVER_ALL,
	eSUB_EVENT_COS_OVER_A,
	eSUB_EVENT_COS_OVER_B,
	eSUB_EVENT_COS_OVER_C,
	#else
	0xff,
	0xff,
	0xff,
	0xff,
	#endif	
	
	//ȫʧѹ
	#if( SEL_EVENT_LOST_ALL_V == YES )//35
	eSUB_EVENT_LOST_ALL_V,
	#else
	0xff,
	#endif
	
	//������Դʧ���¼�
	#if( SEL_EVENT_LOST_SECPOWER == YES )//36
	eSUB_EVENT_LOST_SECPOWER,
	#else
	0xff,
	#endif
	
	//��ѹ������
	#if( SEL_EVENT_V_REVERSAL == YES )//37
	eSUB_EVENT_V_REVERSAL,
	#else
	0xff,
	#endif
	
	//����������
	#if( SEL_EVENT_I_REVERSAL == YES )//38
	eSUB_EVENT_I_REVERSAL,
	#else
	0xff,
	#endif

	//�����¼�
	#if(SEL_EVENT_LOST_POWER == YES)//39
	eSUB_EVENT_LOST_POWER,			//���� 1
	#else
	0xff,
	#endif
	
	//������¼�
	#if(SEL_EVENT_METERCOVER == YES)//40
	eSUB_EVENT_METER_COVER,			//���� 2
	#else
	0xff,
	#endif
	
	//����ť���¼�
	#if(SEL_EVENT_BUTTONCOVER == YES)//41
	eSUB_EVENT_BUTTON_COVER,
	#else
	0xff,
	#endif
		
	//��ѹ��ƽ��
	#if(SEL_EVENT_V_UNBALANCE == YES)//42
	eSUB_EVENT_V_UNBALANCE,
	#else
	0xff,
	#endif
	
	//������ƽ��
	#if(SEL_EVENT_I_UNBALANCE == YES)//43
	eSUB_EVENT_I_UNBALANCE,
	#else
	0xff,
	#endif
	
	//�㶨�ų�����
	#if(SEL_EVENT_MAGNETIC_INT == YES)//44
	eSUB_EVENT_MAGNETIC_INT,
	#else
	0xff,
	#endif
	
	//���ɿ�������
	#if(SEL_EVENT_RELAY_ERR == YES)	//45
	eSUB_EVENT_RELAY_ERR,			//���� 3
	#else
	0xff,
	#endif
	
	//��Դ�쳣
	#if(SEL_EVENT_POWER_ERR == YES)	//46
	eSUB_EVENT_POWER_ERR,			//���� 4
	#else
	0xff,
	#endif
	
	//�������ز�ƽ��
	#if(SEL_EVENT_I_S_UNBALANCE == YES)//47
	eSUB_EVENT_S_I_UNBALANCE,
	#else
	0xff,
	#endif
	
	//ʱ�ӹ���
	#if(SEL_EVENT_RTC_ERR == YES)	//48
	eSUB_EVENT_RTC_ERR,				//���� 5
	#else
	0xff,
	#endif
	
	// ����оƬ����
	#if(SEL_EVENT_SAMPLECHIP_ERR == YES)//49
	eSUB_EVENT_SAMPLECHIP_ERR,		//���� 6
	#else
	0xff,
	#endif
	// ���ܱ����ߵ����쳣
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	eSUB_NEUTRAL_CURRENT_ERR_NO,				
	#else
	0xff,
	#endif
	
	//���ܱ����¼�//50
	eSUB_EVENT_PRG_RECORD,			//���� 7
	
	//���ܱ������¼�
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//51		
	eSUB_EVENT_PRG_CLEAR_METER,		//���� 8	
	#else
	0xff,	
	#endif
	
	//���ܱ����������¼�	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )//52		
	eSUB_EVENT_PRG_CLEAR_MD,		
	#else
	0xff,		
	#endif
											
	//���ܱ��¼������¼�	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//53	
	eSUB_EVENT_PRG_CLEAR_EVENT,			//���� 9
	#else
	0xff,		
	#endif
	
	//���ܱ�Уʱ�¼�		
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//54	
	eSUB_EVENT_PRG_ADJUST_TIME,			//���� 10	
	#else
	0xff,	
	#endif
	
	//���ܱ�ʱ�α����¼�	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )//55	
	eSUB_EVENT_PRG_TIME_TABLE,			//���� 11	
	#else
	0xff,		
	#endif
	
	//���ܱ�ʱ�������¼�	
	#if( SEL_PRG_INFO_TIME_AREA == YES )//56	
	eSUB_EVENT_PRG_TIME_AREA,			//���� 12	
	#else
	0xff,		
	#endif
	
	//���ܱ������ձ���¼�	
	#if( SEL_PRG_INFO_WEEK == YES )//57			
	eSUB_EVENT_PRG_WEEKEND,				//���� 13	
	#else
	0xff,	
	#endif
	
	//���ܱ�����ձ���¼�	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//58	
	eSUB_EVENT_PRG_CLOSING_DAY,			//���� 14	
	#else
	0xff,		
	#endif
	
	//���ܱ���բ��բ�¼�		
	eSUB_EVENT_PRG_OPEN_RELAY,		//���� 15			
	eSUB_EVENT_PRG_CLOSE_RELAY,		//���� 16	
	
	//�ڼ��ձ�̼�¼		
	#if( SEL_PRG_INFO_HOLIDAY == YES )//61		
	eSUB_EVENT_PRG_HOLIDAY,			//���� 17	
	#else
	0xff,	
	#endif
	
	//�й���Ϸ�ʽ��̼�¼	
	#if( SEL_PRG_INFO_P_COMBO == YES )//62		
	eSUB_EVENT_ePRG_P_COMBO,		//���� 18	
	#else
	0xff,			
	#endif
	
	//�޹���Ϸ�ʽ��̼�¼	
	#if( SEL_PRG_INFO_Q_COMBO == YES )//63		
	eSUB_EVENT_PRG_Q_COMBO,		
	#else
	0xff,		
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	eSUB_TARIFF_TABLE_NO,						//���ܱ���ʲ��������¼�
	eSUB_LADDER_TABLE_NO,						// ���ܱ���ݱ����¼�
	#else
	0xff,
	0xff,
	#endif
	
	//���ܱ���Կ�����¼�	
	#if( SEL_PRG_UPDATE_KEY == YES )//64		
	eSUB_EVENT_PRG_UPDATE_KEY,		//���� 19
	#else
	0xff,
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	eSUB_ABR_CARD_NO,							//���ܱ��쳣�忨�¼�
	eSUB_BUY_MONEY_NO,							//���ܱ����¼
	eSUB_RETURN_MONEY_NO,						//���ܱ��˷Ѽ�¼
	#else
	0xff,
	0xff,
	0xff,
	#endif
	
	//���ܱ�㲥Уʱ�¼�	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )//54	
	eSUB_EVENT_BROADJUST_TIME,			//���� 10	
	#else
	0xff,	
	#endif
	
	eSUB_EVENT_STATUS_TOTAL				//���ֵ65 ����20

};

// �¼���Ϣ�����ڲ����¼�״̬����  �˱��Ӧ��eSUB_EVENT_INDEX���涨���ö��˳�򱣳�һ��
const TSubEventInfoTab	SubEventInfoTab[] = 
{
	#if ( SEL_EVENT_LOST_V == YES )
	//ʧѹ�¼�
	{eSUB_EVENT_LOSTV_A, 			eEVENT_LOST_V_NO, 			ePHASE_A, 	GetLostVStatus },
	{eSUB_EVENT_LOSTV_B, 			eEVENT_LOST_V_NO, 			ePHASE_B, 	GetLostVStatus },
	{eSUB_EVENT_LOSTV_C, 			eEVENT_LOST_V_NO, 			ePHASE_C, 	GetLostVStatus },
	#endif

	#if( SEL_EVENT_WEAK_V == YES )
	//Ƿѹ�¼�
	{eSUB_EVENT_WEAK_V_A, 			eEVENT_WEAK_V_NO, 			ePHASE_A, 	GetWeakVStatus },	
	{eSUB_EVENT_WEAK_V_B, 			eEVENT_WEAK_V_NO, 			ePHASE_B, 	GetWeakVStatus }, 
	{eSUB_EVENT_WEAK_V_C, 			eEVENT_WEAK_V_NO, 			ePHASE_C, 	GetWeakVStatus }, 
	#endif

	#if( SEL_EVENT_OVER_V == YES )
	//��ѹ�¼�
	{eSUB_EVENT_OVER_V_A, 			eEVENT_OVER_V_NO, 			ePHASE_A, 	GetOverVStatus }, 
	{eSUB_EVENT_OVER_V_B, 			eEVENT_OVER_V_NO, 			ePHASE_B, 	GetOverVStatus }, 
	{eSUB_EVENT_OVER_V_C, 			eEVENT_OVER_V_NO, 			ePHASE_C, 	GetOverVStatus }, 
	#endif

	#if( SEL_EVENT_BREAK == YES )
	//�����¼�
	{eSUB_EVENT_BREAK_A, 			eEVENT_BREAK_NO, 			ePHASE_A, 	GetBreakStatus }, 
	{eSUB_EVENT_BREAK_B, 			eEVENT_BREAK_NO, 			ePHASE_B, 	GetBreakStatus }, 
	{eSUB_EVENT_BREAK_C, 			eEVENT_BREAK_NO, 			ePHASE_C, 	GetBreakStatus }, 
	#endif
	
	#if( SEL_EVENT_LOST_I == YES )
	//ʧ���¼�
	{eSUB_EVENT_LOSTI_A, 			eEVENT_LOST_I_NO, 			ePHASE_A, 	GetLostIStatus }, 
	{eSUB_EVENT_LOSTI_B, 			eEVENT_LOST_I_NO, 			ePHASE_B, 	GetLostIStatus }, 
	{eSUB_EVENT_LOSTI_C, 			eEVENT_LOST_I_NO, 			ePHASE_C, 	GetLostIStatus }, 
	#endif

	#if( SEL_EVENT_OVER_I == YES )
	//�����¼�
	#if( MAX_PHASE == 3 )
	{eSUB_EVENT_OVER_I_A, 			eEVENT_OVER_I_NO, 			ePHASE_A, 	GetOverIStatus },
	{eSUB_EVENT_OVER_I_B, 			eEVENT_OVER_I_NO, 			ePHASE_B, 	GetOverIStatus },	
	{eSUB_EVENT_OVER_I_C, 			eEVENT_OVER_I_NO, 			ePHASE_C, 	GetOverIStatus },	
	#else
	{eSUB_EVENT_OVER_I_A, 			eEVENT_OVER_I_NO, 			ePHASE_A, 	GetOverIStatus },
	#endif
	#endif

	#if( SEL_EVENT_BREAK_I == YES )
	//�����¼�
	{eSUB_EVENT_BREAK_I_A, 			eEVENT_BREAK_I_NO, 			ePHASE_A, 	GetBreakIStatus },
	{eSUB_EVENT_BREAK_I_B, 			eEVENT_BREAK_I_NO, 			ePHASE_B, 	GetBreakIStatus },	
	{eSUB_EVENT_BREAK_I_C, 			eEVENT_BREAK_I_NO, 			ePHASE_C, 	GetBreakIStatus },	
	#endif
	
	#if( SEL_EVENT_BACKPROP == YES )//��Ҫ�����������������ģ���������ҲҪ�ڱ����������ABC,��Լ����ʱ���ǰ���������ҵ�
	//���ʷ���
	{eSUB_EVENT_POW_BACK_ALL, 		eEVENT_BACKPROP_NO, 		ePHASE_ALL, GetBackpropStatus },	
	{eSUB_EVENT_POW_BACK_A, 		eEVENT_BACKPROP_NO, 		ePHASE_A, 	GetBackpropStatus },
	{eSUB_EVENT_POW_BACK_B, 		eEVENT_BACKPROP_NO, 		ePHASE_B, 	GetBackpropStatus },	
	{eSUB_EVENT_POW_BACK_C, 		eEVENT_BACKPROP_NO, 		ePHASE_C, 	GetBackpropStatus },	
	#endif
	
	#if( SEL_EVENT_OVERLOAD == YES )
	//�����¼�
	{eSUB_EVENT_OVERLOAD_A, 		eEVENT_OVERLOAD_NO, 		ePHASE_A, 	GetOverloadStatus },
	{eSUB_EVENT_OVERLOAD_B, 		eEVENT_OVERLOAD_NO, 		ePHASE_B, 	GetOverloadStatus },	
	{eSUB_EVENT_OVERLOAD_C, 		eEVENT_OVERLOAD_NO, 		ePHASE_C, 	GetOverloadStatus },	
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	//���������¼�
	{eSUB_EVENT_PA_DEMAND_OVER,		eEVENT_PA_DEMAND_OVER_NO, 	ePHASE_ALL, GetPADemandOverStatus },	
	{eSUB_EVENT_NA_DEMAND_OVER, 	eEVENT_NA_DEMAND_OVER_NO, 	ePHASE_ALL, GetNADemandOverStatus },	
	{eSUB_EVENT_QI_DEMAND_OVER, 	eEVENT_RE_DEMAND_OVER_NO, 	ePHASE_ALL, GetQDemandOverStatus },	
	{eSUB_EVENT_QII_DEMAND_OVER, 	eEVENT_RE_DEMAND_OVER_NO, 	ePHASE_A, 	GetQDemandOverStatus },	
	{eSUB_EVENT_QIII_DEMAND_OVER,	eEVENT_RE_DEMAND_OVER_NO, 	ePHASE_B, 	GetQDemandOverStatus },	
	{eSUB_EVENT_QIV_DEMAND_OVER, 	eEVENT_RE_DEMAND_OVER_NO,	ePHASE_C, 	GetQDemandOverStatus },	
    #endif
    
    #if( SEL_EVENT_COS_OVER == YES )
	//�ܹ������������¼�
	{eSUB_EVENT_COS_OVER_ALL, 		eEVENT_COS_OVER_NO, 		ePHASE_ALL, GetCosoverStatus },	
	{eSUB_EVENT_COS_OVER_A, 		eEVENT_COS_OVER_NO, 		ePHASE_A, 	GetCosoverStatus },	
	{eSUB_EVENT_COS_OVER_B, 		eEVENT_COS_OVER_NO, 		ePHASE_B,	GetCosoverStatus },	
	{eSUB_EVENT_COS_OVER_C, 		eEVENT_COS_OVER_NO, 		ePHASE_C,	GetCosoverStatus },	
	#endif

	#if( SEL_EVENT_LOST_ALL_V == YES )
	//ȫʧѹ�¼�
	{eSUB_EVENT_LOST_ALL_V, 		eEVENT_LOST_ALL_V_NO, 		ePHASE_ALL, GetLostAllVStatus }, 
	#endif

	#if( SEL_EVENT_LOST_SECPOWER == YES )
	//������Դʧ���¼�
	{eSUB_EVENT_LOST_SECPOWER, 		eEVENT_LOST_SECPOWER_NO, 	ePHASE_ALL, GetLostSecPowerStatus }, 
	#endif

	#if( SEL_EVENT_V_REVERSAL == YES )
	//��ѹ�������¼�
	{eSUB_EVENT_V_REVERSAL, 		eEVENT_V_REVERSAL_NO, 		ePHASE_ALL, GetVReversalStatus }, 
	#endif

	#if( SEL_EVENT_I_REVERSAL == YES )
	//�����������¼�
	{eSUB_EVENT_I_REVERSAL, 		eEVENT_I_REVERSAL_NO, 		ePHASE_ALL, GetIReversalStatus }, 
	#endif
	
	#if( SEL_EVENT_LOST_POWER == YES )
	//�����¼�
	{eSUB_EVENT_LOST_POWER, 		eEVENT_LOST_POWER_NO, 		ePHASE_ALL, GetLostPowerStatus },	
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )
	//������¼�
	{eSUB_EVENT_METER_COVER, 		eEVENT_METERCOVER_NO, 		ePHASE_ALL, GetMeterCoverStatus },	
	#endif
	
	#if( SEL_EVENT_BUTTONCOVER == YES )
	//����ť���¼�
	{eSUB_EVENT_BUTTON_COVER, 		eEVENT_BUTTONCOVER_NO, 		ePHASE_ALL, GetButtonCoverStatus },	
	#endif

	#if( SEL_EVENT_V_UNBALANCE == YES )
	//��ѹ��ƽ���¼�
	{eSUB_EVENT_V_UNBALANCE, 		eEVENT_V_UNBALANCE_NO, 		ePHASE_ALL, GetVUnBalanceStatus }, 
	#endif

	#if( SEL_EVENT_I_UNBALANCE == YES )
	//������ƽ���¼�
	{eSUB_EVENT_I_UNBALANCE, 		eEVENT_I_UNBALANCE_NO, 		ePHASE_ALL, GetIUnBalanceStatus }, 
	#endif
	
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	//�㶨�ų�����
	{eSUB_EVENT_MAGNETIC_INT, 		eEVENT_MAGNETIC_INT_NO, 	ePHASE_ALL, GetMagneticStatus },	
	#endif
	
	#if( SEL_EVENT_RELAY_ERR == YES )
	//���ɿ�������
	{eSUB_EVENT_RELAY_ERR, 			eEVENT_RELAY_ERR_NO, 		ePHASE_ALL, GetRelayErrStatus },	
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	//��Դ�쳣�¼�
	{eSUB_EVENT_POWER_ERR, 			eEVENT_POWER_ERR_NO, 		ePHASE_ALL, GetPowerErrStatus },	
    #endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	//�������ز�ƽ���¼
	{eSUB_EVENT_S_I_UNBALANCE, 		eEVENT_I_S_UNBALANCE_NO, 	ePHASE_ALL, GetISUnBalanceStatus },	
	#endif
	
	#if( SEL_EVENT_RTC_ERR == YES )
    //ʱ�ӹ���
    {eSUB_EVENT_RTC_ERR, 			eEVENT_RTC_ERR_NO, 			ePHASE_ALL, GetRtcErrStatus },	
	#endif

	#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
	// ����оƬ����
	{eSUB_EVENT_SAMPLECHIP_ERR, 	eEVENT_SAMPLECHIP_ERR_NO, 	ePHASE_ALL,	GetSampleChipErrStatus },	
	#endif
	
	//���ܱ����ߵ����쳣
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )//			
	{eSUB_NEUTRAL_CURRENT_ERR_NO,eEVENT_NEUTRAL_CURRENT_ERR_NO,ePHASE_ALL, GetNeutralCurrentErrStatus },	
	#endif
	
	//���ܱ����¼�
	{eSUB_EVENT_PRG_RECORD, 		ePRG_RECORD_NO,		 		ePHASE_ALL, GetPrgStatus },	
	
	//���ܱ������¼�
	#if( SEL_PRG_INFO_CLEAR_METER == YES )	
	{eSUB_EVENT_PRG_CLEAR_METER, 	ePRG_CLEAR_METER_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//���ܱ����������¼�	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )//42					
	{eSUB_EVENT_PRG_CLEAR_MD, 		ePRG_CLEAR_MD_NO, 			ePHASE_ALL, GetPrgStatus },	
	#endif
											
	//���ܱ��¼������¼�	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//43			
	{eSUB_EVENT_PRG_CLEAR_EVENT, 	ePRG_CLEAR_EVENT_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//���ܱ�Уʱ�¼�		
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//44			
	{eSUB_EVENT_PRG_ADJUST_TIME, 	ePRG_ADJUST_TIME_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//���ܱ�ʱ�α����¼�	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )//45			
	{eSUB_EVENT_PRG_TIME_TABLE, 	ePRG_TIME_TABLE_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//���ܱ�ʱ�������¼�	
	#if( SEL_PRG_INFO_TIME_AREA == YES )//46			
	{eSUB_EVENT_PRG_TIME_AREA, 		ePRG_TIME_AREA_NO,	 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//���ܱ������ձ���¼�	
	#if( SEL_PRG_INFO_WEEK == YES )//47					
	{eSUB_EVENT_PRG_WEEKEND,	 	ePRG_WEEKEND_NO, 			ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//���ܱ�����ձ���¼�	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//48				
	{eSUB_EVENT_PRG_CLOSING_DAY, 	ePRG_CLOSING_DAY_NO, 		ePHASE_ALL, GetPrgStatus },	
	#endif
	
	//���ܱ���բ��բ�¼�			
	{eSUB_EVENT_PRG_OPEN_RELAY, 	ePRG_OPEN_RELAY_NO, 		ePHASE_ALL, GetPrgStatus }, 
	{eSUB_EVENT_PRG_CLOSE_RELAY, 	ePRG_CLOSE_RELAY_NO, 		ePHASE_ALL, GetPrgStatus }, 			
	
	//�ڼ��ձ�̼�¼		
	#if( SEL_PRG_INFO_HOLIDAY == YES )//55			
	{eSUB_EVENT_PRG_HOLIDAY,	 	ePRG_HOLIDAY_NO, 			ePHASE_ALL, GetPrgStatus },			
	#endif
	
	//�й���Ϸ�ʽ��̼�¼	
	#if( SEL_PRG_INFO_P_COMBO == YES )//56		
	{eSUB_EVENT_ePRG_P_COMBO, 		ePRG_P_COMBO_NO, 			ePHASE_ALL, GetPrgStatus },				
	#endif
	
	//�޹���Ϸ�ʽ��̼�¼	
	#if( SEL_PRG_INFO_Q_COMBO == YES )//57		
	{eSUB_EVENT_PRG_Q_COMBO,	 	ePRG_Q_COMBO_NO,	 		ePHASE_ALL, GetPrgStatus },
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	//���ܱ���ʲ��������¼�
	{eSUB_TARIFF_TABLE_NO,			ePRG_TARIFF_TABLE_NO,		ePHASE_ALL,	GetPrgStatus},
	// ���ܱ���ݱ����¼�
	{eSUB_LADDER_TABLE_NO,			ePRG_LADDER_TABLE_NO,		ePHASE_ALL,	GetPrgStatus},
	#endif
	
	//���ܱ���Կ�����¼�	
	#if( SEL_PRG_UPDATE_KEY == YES )//58			
	{eSUB_EVENT_PRG_UPDATE_KEY,		ePRG_UPDATE_KEY_NO,			ePHASE_ALL, GetPrgStatus },
	#endif 
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//���ܱ��쳣�忨�¼�
	{eSUB_ABR_CARD_NO,				ePRG_ABR_CARD_NO,			ePHASE_ALL,	GetPrgStatus},
	//���ܱ����¼
	{eSUB_BUY_MONEY_NO,				ePRG_BUY_MONEY_NO,			ePHASE_ALL,	GetPrgStatus},
	//���ܱ��˷Ѽ�¼
	{eSUB_RETURN_MONEY_NO,			ePRG_RETURN_MONEY_NO,		ePHASE_ALL,	GetPrgStatus},
	#endif
	
	//���ܱ�㲥Уʱ�¼�		
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )//			
	{eSUB_EVENT_BROADJUST_TIME, 	ePRG_BROADJUST_TIME_NO, 	ePHASE_ALL, GetPrgStatus },	
	#endif
};

//�˱��Ӧ��eEVENT_INDEX���涨��ö�ٵ�˳�򱣳�һ��
const TEventInfoTab EventInfoTab[] =
{
	#if( SEL_EVENT_LOST_V == YES )		//ʧѹ
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��ѹ����	��ѹ����	��������	�ж�ʱ�� 
	{0x3000,eEVENT_LOST_V_NO,		MAX_PHASE,			NO,		0,				4,	eTYPE_WORD,	eTYPE_SPEC1,eTYPE_DWORD,eTYPE_BYTE,},
	#endif
	
	#if( SEL_EVENT_WEAK_V == YES )		//Ƿѹ
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��ѹ����	�ж�ʱ��		 
	{0x3001,eEVENT_WEAK_V_NO,		MAX_PHASE,			NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_OVER_V == YES )		//��ѹ
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��ѹ����	�ж�ʱ�� 
	{0x3002,eEVENT_OVER_V_NO,		MAX_PHASE,			NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_BREAK == YES )		//����
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��ѹ����	��������	�ж�ʱ��	 
	{0x3003,eEVENT_BREAK_NO,		MAX_PHASE,			NO,		0,		    	3,	eTYPE_WORD,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_LOST_I == YES )		//ʧ��
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��ѹ����	��������	��������	�ж�ʱ�� 
	{0x3004,eEVENT_LOST_I_NO,		MAX_PHASE,			NO, 	0,		    	4,	eTYPE_WORD,	eTYPE_DWORD,eTYPE_SPEC1,eTYPE_BYTE,},
	#endif
		
	#if( SEL_EVENT_OVER_I == YES )		//����
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��������	�ж�ʱ�� 
	{0x3005,eEVENT_OVER_I_NO,		MAX_PHASE,			NO,		0,		    	2,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	
	#if( SEL_EVENT_BREAK_I == YES )		//����
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��ѹ����	��������	�ж�ʱ�� 
	{0x3006,eEVENT_BREAK_I_NO,		MAX_PHASE,			NO,		0,		    	3,	eTYPE_WORD,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,},
	#endif
	
	
	#if( SEL_EVENT_BACKPROP == YES )	//���ʷ���
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��������	�ж�ʱ�� 
	{0x3007,eEVENT_BACKPROP_NO,		EVENT_PHASE_FOUR,	NO,		0,		    	2,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	
	#if( SEL_EVENT_OVERLOAD == YES )	//����
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��������	�ж�ʱ�� 
	{0x3008,eEVENT_OVERLOAD_NO,		MAX_PHASE,			NO,		0,		    	2,	eTYPE_DWORD,eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	//0x3009	���ܱ������й����������¼�
	//0x300a	���ܱ����й����������¼�
	//0x300b	���ܱ��޹����������¼�
	#if( SEL_DEMAND_OVER == YES )
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� 
	{0x3009,eEVENT_PA_DEMAND_OVER_NO,	1,					NO,	sizeof(TDemandData),2,	eTYPE_DWORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL},
	{0x300a,eEVENT_NA_DEMAND_OVER_NO,	1,					NO,	sizeof(TDemandData),2,	eTYPE_DWORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL},
	{0x300b,eEVENT_RE_DEMAND_OVER_NO,	EVENT_PHASE_FOUR,	NO,	sizeof(TDemandData),2,	eTYPE_DWORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL},
	#endif
	
	#if( SEL_EVENT_COS_OVER == YES )	//������������
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ���޷�ֵ	�ж�ʱ�� 
	{0x303B,eEVENT_COS_OVER_NO,			EVENT_PHASE_FOUR,	NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_LOST_ALL_V == YES )	//ȫʧѹ
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		��������  
	{0x300d,eEVENT_LOST_ALL_V_NO,		1,					YES,	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )//������Դʧ��
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� �ж�ʱ�� 
	{0x300e,eEVENT_LOST_SECPOWER_NO,	1, 					YES,	0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_V_REVERSAL == YES )	//��ѹ������
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� �ж�ʱ�� 
	{0x300f,eEVENT_V_REVERSAL_NO,		1,					NO,		0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_I_REVERSAL == YES )	//����������
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� �ж�ʱ�� 
	{0x3010,eEVENT_I_REVERSAL_NO,		1,					NO,		0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_LOST_POWER == YES )	//����
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� �ж�ʱ�� 
	{0x3011,eEVENT_LOST_POWER_NO,		1, 					YES,	0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )	//�����
	//OI	�¼�ö�ٺ�					��ȡ��ַ����			�����		�������⴦�� ��չ����		�������� ��ѹ		��ѹ����	��������	�ж�ʱ�� 
	{0x301b,eEVENT_METERCOVER_NO,		1,					YES,	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_BUTTONCOVER == YES )	//����ť���¼�		
	//OI	�¼�ö�ٺ�					��ȡ��ַ����			�����		�������⴦�� ��չ����		�������� ��ѹ		��ѹ����	��������	�ж�ʱ�� 
	{0x301c,eEVENT_BUTTONCOVER_NO,		1,					YES,	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_V_UNBALANCE == YES )	//��ѹ��ƽ��
	//OI	�¼�ö�ٺ�					��ȡ��ַ����			�����		�������⴦�� ��չ����		�������� ��ֵ		�ж�ʱ�� 
	{0x301d,eEVENT_V_UNBALANCE_NO,		1,					NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )	//������ƽ��
	//OI	�¼�ö�ٺ�					��ȡ��ַ����			�����		�������⴦�� ��չ����		�������� ��ֵ		�ж�ʱ�� 
	{0x301e,eEVENT_I_UNBALANCE_NO,		1,					NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
										//���ܱ����¼�
	#if( SEL_EVENT_MAGNETIC_INT == YES )//�㶨�ų�����
	//OI	�¼�ö�ٺ�					��ȡ��ַ����			�����		�������⴦�� ��չ����		�������� �ж�ʱ�� 
	{0x302a,eEVENT_MAGNETIC_INT_NO,		1,					NO,		0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_RELAY_ERR == YES )	//���ɿ�������
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� �ж�ʱ�� 
	{0x302b,eEVENT_RELAY_ERR_NO,		1,					NO, 	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )	//��Դ�쳣�¼�
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� �ж�ʱ�� 
	{0x302c,eEVENT_POWER_ERR_NO,		1,					NO, 	0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )//�������ز�ƽ��
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		�������� ��ֵ		�ж�ʱ�� 
	{0x302d,eEVENT_I_S_UNBALANCE_NO,	1,					NO,		0,		    	2,	eTYPE_WORD,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_EVENT_RTC_ERR == YES )		//ʱ�ӹ���
	//OI	�¼�ö�ٺ�					�����		�������⴦�� ��չ����		��������  
	{0x302e,eEVENT_RTC_ERR_NO,			1,					YES,	0,		    	0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL},
	#endif
	
	#if( SEL_EVENT_SAMPLECHIP_ERR == YES )// ����оƬ����
	//OI	�¼�ö�ٺ�					��ȡ��ַ����			�����		�������⴦�� ��չ����		��������  
	{0x302f,eEVENT_SAMPLECHIP_ERR_NO,	1,					NO,		0,		    	1,	eTYPE_BYTE,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL},
	#endif

	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )//���ܱ����ߵ����쳣�¼�		
	{0x3040,eEVENT_NEUTRAL_CURRENT_ERR_NO,		1,			NO,		0,				3,	eTYPE_DWORD,eTYPE_WORD,eTYPE_BYTE,	eTYPE_NULL,},
	#endif
	
	{0x3012,ePRG_RECORD_NO, 			1,				NO,sizeof(TPrgProgramSubRom),0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//���ܱ������¼�		
	{0x3013,ePRG_CLEAR_METER_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )	//���ܱ����������¼�	
	{0x3014,ePRG_CLEAR_MD_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
												
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//���ܱ��¼������¼�	
	{0x3015,ePRG_CLEAR_EVENT_NO,		1,					NO,		4,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//���ܱ�Уʱ�¼�		
	{0x3016,ePRG_ADJUST_TIME_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )//���ܱ�ʱ�α����¼�								ʱ�α�+OAD
	{0x3017,ePRG_TIME_TABLE_NO,			1,					NO,		42+4,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_TIME_AREA == YES )//���ܱ�ʱ�������¼�	
	{0x3018,ePRG_TIME_AREA_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_WEEK == YES )		//���ܱ������ձ���¼�	
	{0x3019,ePRG_WEEKEND_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},							
	#endif
	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//���ܱ�����ձ���¼�	
	{0x301a,ePRG_CLOSING_DAY_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	{0x301f,ePRG_OPEN_RELAY_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	{0x3020,ePRG_CLOSE_RELAY_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	
	#if( SEL_PRG_INFO_HOLIDAY == YES )	//�ڼ��ձ�̼�¼									//date+ʱ�α��+OAD
	{0x3021,ePRG_HOLIDAY_NO,			1,					NO,		6+4,			0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_P_COMBO == YES )	//�й���Ϸ�ʽ��̼�¼	
	{0x3022,ePRG_P_COMBO_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )	//�޹���Ϸ�ʽ��̼�¼	
	{0x3023, ePRG_Q_COMBO_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	//���ܱ���ʲ��������¼�
	{0x3024,ePRG_TARIFF_TABLE_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	// ���ܱ���ݱ����¼�
	{0x3025,ePRG_LADDER_TABLE_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	#endif
	
	#if( SEL_PRG_UPDATE_KEY == YES )	//���ܱ���Կ�����¼�	
	{0x3026,ePRG_UPDATE_KEY_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//���ܱ��쳣�忨�¼� 18=8+1+7+2 ����7������ͷ������ͷ����ʱ���� CH,Cla,Ins,P1,P2,P31,P32,����CH:0x01--ESAM 0x00--CARD,esam����ͷΪCH���6�ֽڣ���ΪCH���5�ֽ�
	{0x3027,ePRG_ABR_CARD_NO,			1,					NO,		18,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	//���ܱ����¼
	{0x3028,ePRG_BUY_MONEY_NO,			1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	//���ܱ��˷Ѽ�¼
	{0x3029,ePRG_RETURN_MONEY_NO,		1,					NO,		4,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL, },
	#endif
	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )//���ܱ�㲥Уʱ�¼�		
	{0x303C,ePRG_BROADJUST_TIME_NO,		1,					NO,		0,				0,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,	eTYPE_NULL,},
	#endif
	
	#if( SEL_STAT_V_RUN == YES )		//��ѹ�ϸ���
	//OI	�¼�ö�ٺ�					��ȡ��ַ����			�����		�������⴦�� ��չ����		�������� ��������	��������	�ϸ�����	�ϸ����� 
	{0x4030,eSTATISTIC_V_PASS_RATE_NO,	1,					NO,		0,		    	4,	eTYPE_WORD,	eTYPE_SPEC1,eTYPE_SPEC2,eTYPE_SPEC3,},
	#endif
};

// ���й̶��е�OAD
const DWORD FixedColumnOadTab[] =
{
	RECORDNO_OAD,		// ���
	STARTTIME_OAD,		// ��ʼʱ��
	ENDTIME_OAD,		// ����ʱ��
	GEN_SOURCE_OAD,		// ����Դ
	NOTIFICATION_OAD,	// �ϱ�״̬
	MAXDEMAND_OAD,		// �����ڼ��������
	MAXDEMAND_TIME_OAD,	// �����ڼ������������ʱ��
	PRG_OBJ_LIST_OAD,	// ��̶����б�
	CLR_EVENT_LIST_OAD	// �¼������б�
};

// OAD �̶����б� ����¼���¼��ԪΪ��׼�¼���Ԫ�ɲ��ӵ��������У������¼���¼��Ԫ����ӹ���
static const TEventOadTab	EventOadTab[] = 
{
	{0xFFFF,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 7
	{0x3000,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3001,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3002,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3003,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3004,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3005,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3006,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3007,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3008,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x303B,5,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD },										// class id 24
	{0x3009,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,MAXDEMAND_OAD,MAXDEMAND_TIME_OAD },	// �����й��������� class id 7
	{0x300a,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,MAXDEMAND_OAD,MAXDEMAND_TIME_OAD },	// �����й��������� class id 7
	{0x300b,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,MAXDEMAND_OAD,MAXDEMAND_TIME_OAD },	// �޹��������� class id 24
	{0x3012,6,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,PRG_OBJ_LIST_OAD },					// ��̼�¼ class id 7
	{0x3015,6,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,CLR_EVENT_LIST_OAD },					// �¼����� class id 7
	{0x3017,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,PRG_SEGTABLE_OAD,PRG_SEGTABLE_VALUE }, //���ܱ�ʱ�α����¼�3017
	{0x3021,7,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,PRG_HOLIDAY_OAD,PRG_HOLIDAY_VALUE},	//���ܱ�ڼ��ձ���¼�3021	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	{0x3027,9,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,CARD_SN_OAD,CARD_ERR_INFO_OAD,CARD_ORDER_HEAD_OAD,CARD_ERR_RES_OAD},//���ܱ��쳣�忨�¼�0x3027
	{0x3029,6,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,RETURN_MONEY_OAD},	//���ܱ��˷Ѽ�¼3029	
	#endif
};

static BYTE 	SubEventTimer[SUB_EVENT_INDEX_MAX_NUM];	// ��ʱ������
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: �����¼����ͼ���λȷ�������¼�ö�ٺ�
//
//����: 
//			EventIndex[in]:	�¼�ö�ٺ�                    
//			Phase[in]:		��λ                    
//����ֵ:	�����¼�ö�ٺ�	
//��ע:
//-----------------------------------------------
BYTE GetSubEventIndex(TEventAddrLen *pEventAddrLen)
{
	BYTE i;
	
	for( i=0; i<(sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab)); i++ )
	{
		if( 	(SubEventInfoTab[i].EventIndex==pEventAddrLen->EventIndex)
			&& 	(SubEventInfoTab[i].Phase==pEventAddrLen->Phase) )
		{
			break;
		}
	}

	if( i == (sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab)) )//���û�������� ��Ĭ��ֵ
	{
        i = 0;
        ASSERT( FALSE, 0 );
	}
	
	return SubEventInfoTab[i].SubEventIndex;
}

//-----------------------------------------------
//��������: ��ȡOI��Ӧ��EventIndex
//
//����: 
//	OI[in]:	�¼�OI
//                    
//	pEventOrder[out]:	ָ��EventOrder��ָ��
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//			
//��ע:
//-----------------------------------------------
BOOL GetEventIndex( WORD OI, BYTE *pEventIndex )
{
	BYTE i;

	for( i = 0; i < (sizeof(EventInfoTab)/sizeof(TEventInfoTab)); i++ )
	{
		if( OI == EventInfoTab[i].OI )
		{
			*pEventIndex = EventInfoTab[i].EventIndex;
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------
//��������: ��ȡinEventIndex��Ӧ��OI
//����: 
//			inEventIndex[in]:	�¼�ö�ٺ�              
//����ֵ:	OI		
//��ע:
//-----------------------------------------------
WORD GetEventOI( BYTE inEventIndex )
{
	BYTE i;

	for( i = 0; i < (sizeof(EventInfoTab)/sizeof(TEventInfoTab)); i++ )
	{
		if( inEventIndex == EventInfoTab[i].EventIndex )
		{
			return EventInfoTab[i].OI;
		}
	}

	return EventInfoTab[0].OI;
}

//-----------------------------------------------
//��������: ��ȡ�¼���¼�Ļ���ַ��Ϣ
//
//����: 		BYTE i[in]
//			pTEventAddrLen[in/out]
//			EventIndex[in]		   --�����¼�ö�ٺ�
//			Phase  [in] 		   --�����࣬������Ϊ0
//			dwAttInfoEeAddr[out]   --�����Ӧ���̼�¼�Ĺ�����������
//			dwDataInfoEeAddr[out]  --���eeprom�е�info���ݵ�ַ
//			dwRecordAddr[out]	   --�����Ӧ���̼�¼�Ĺ̶������ݵ�ַ
//����ֵ:  	TRUE
//
//��ע:       ����ÿ������Ϊÿ���¼���¼�Ļ�ȡ��ַ����
//-----------------------------------------------
BOOL GetEventRecordInfo(BYTE i, TEventAddrLen *pTEventAddrLen )
{
	
	if( pTEventAddrLen->EventIndex > eNUM_OF_EVENT_PRG )//����ֵ���
	{
		return FALSE;
	}
	
	if( pTEventAddrLen->SubEventIndex > eSUB_EVENT_STATUS_TOTAL )
	{
		return FALSE;
	}
	
	if( pTEventAddrLen->Phase > ePHASE_C )
	{
		return FALSE;
	}

	if( i > sizeof(EventInfoTab)/sizeof(TEventInfoTab) )
	{
		return FALSE;
	}

	pTEventAddrLen->dwAttInfoEeAddr = GET_CONTINUE_ADDR( EventConRom.EventAttInfo[pTEventAddrLen->EventIndex] );
	pTEventAddrLen->dwDataInfoEeAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.EventDataInfo[pTEventAddrLen->SubEventIndex] );
	pTEventAddrLen->dwRecordAddr = FLASH_EVENT_BASE+(DWORD)pTEventAddrLen->SubEventIndex*EVENT_RECORD_LEN;

	return TRUE;
}

//-----------------------------------------------
//��������: ��ȡType��Ӧ�����Ե�ַ��������Ϣ��ַ�����ݵ�ַ���������ݵ�ַ��������ݳ���
//
//����: 
//	pTEventAddrLen:	TEventAddrLen���͵�ָ��                    
// 		dwAttInfoEeAddr[out]:		OAD��Ϣ��ַ
// 		dwDataInfoEeAddr[out]:		������Ϣ��ַ
// 		dwRecordAddr[out]:			�������ݵ�ַ
// 		wDataLen[out]:				������ݳ���
// 		EventIndex[in]:				eEVENT_INDEX
// 		Phase[in]:					��λ0��,1A,2B,3C
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//			
//��ע:
//-----------------------------------------------
BOOL GetEventInfo( TEventAddrLen *pEventAddrLen )
{
	BYTE i;

	for( i = 0; i < sizeof(EventInfoTab)/sizeof(TEventInfoTab); i++ )
	{
		if( EventInfoTab[i].EventIndex == pEventAddrLen->EventIndex )
		{
			pEventAddrLen->SubEventIndex = GetSubEventIndex(pEventAddrLen);
			return GetEventRecordInfo( i, pEventAddrLen );
		}
	}

	return FALSE;
}
//-----------------------------------------------
//��������: ��ȡ�¼����ݴ洢��ƫ�Ƶ�ַ�����Ϣ
//
//����: 
// 			Len[in]:�¼���������¼�ĳ���
// 			RecordDepth[in]:	��OAD�����
// 			inRecordNo[in]:	��OAD���ݵ�RecordNo
// 			pSectorInfo[out]:ָ��SectorInfo��ָ��
//����ֵ:
//	��
//
//��ע: 
//-----------------------------------------------
 BOOL GetEventSectorInfo( WORD Len, WORD RecordDepth, DWORD inRecordNo, TEventSectorInfo *pEventSectorInfo )
{
	WORD twSectorNo;			// ��OADռ�õ���������	
	WORD twOadNoPerSector;		// һ�������ܴ�ŵ�OAD������
	WORD twSectorOff;			// ������ƫ��
	WORD twInSectorOff;			// �����ڵ�ƫ��
	
	
	if( (Len<4) || (RecordDepth==0) || ( Len > EVENT_VALID_OADLEN) )
	{

		return FALSE;
	}
	else
	{
		// һ���������OAD�ĸ���
		twOadNoPerSector = SECTOR_SIZE/Len;
		if( RecordDepth < twOadNoPerSector )
		{
			twOadNoPerSector = RecordDepth;
		}	
		// ��OADռ��������
		if( (RecordDepth%twOadNoPerSector) == 0 )
		{
			twSectorNo = (RecordDepth/twOadNoPerSector)+1;
		}
		else
		{
			twSectorNo = ((RecordDepth/twOadNoPerSector)+1)+1;	// ��λ��1
		}
		// ������ƫ��
		twSectorOff = inRecordNo/twOadNoPerSector;
		twSectorOff %= twSectorNo;
		// �����ڵ�ƫ��
		twInSectorOff = inRecordNo%twOadNoPerSector;

		pEventSectorInfo->dwPointer = inRecordNo % RecordDepth;
		pEventSectorInfo->dwSaveSpace = SECTOR_SIZE*twSectorNo;
		pEventSectorInfo->dwSectorAddr = (DWORD)twSectorOff*SECTOR_SIZE+twInSectorOff*Len;	
		
		return TRUE;
	}
}

//-----------------------------------------------
//��������: ��������
//
//����: 
// inEventIndex[in]:	eEVENT_INDEX    
//                
//����ֵ:��
//			
//��ע: �����ݶ�Ӧ��Info
//-----------------------------------------------
static void ClearEventData( BYTE inEventIndex )
{	
	BYTE i,tSubEventIndex;
	DWORD dwAddr1;
	BYTE Buf[sizeof(TEventDataInfo)];

	memset(Buf,0x00,sizeof(TEventDataInfo));

	for( i = 0; i < sizeof(SubEventInfoTab)/sizeof(TSubEventInfoTab); i++ )
	{
		if( inEventIndex == SubEventInfoTab[i].EventIndex )
		{
			dwAddr1 = GET_SAFE_SPACE_ADDR( EventSafeRom.EventDataInfo[i] );
			api_VWriteSafeMem( dwAddr1, sizeof(TEventDataInfo), Buf );
			// �¼���Ȼ�ҵ��˾Ͷ������ﴦ������Ҫ�ٸ�һ��ѭ��
			if( inEventIndex == ePRG_RECORD_NO )
			{
				// ��EE��ʱ�����̼�¼���OADָ����Ϣ
				dwAddr1 = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );			
				api_ClrContinueEEPRom( dwAddr1, sizeof(TPrgProgramSubRom) );
				// ��������¼�¼��̼�¼
				ClearPrgRecordRam();
			}
			else if(inEventIndex < eNUM_OF_EVENT)// ������¼��ĵ�������// ��Ҫ��������־�е�RAM����
			{
				// ��ǰ״̬
				tSubEventIndex = SubEventInfoTab[i].SubEventIndex;
				EventStatus.CurrentStatus[tSubEventIndex/8] &= (BYTE)~(0x01<<(tSubEventIndex%8));

				// Timer
				SubEventTimer[tSubEventIndex] = 0;

				// ��ʱ���״̬
				api_DealEventStatus(eCLEAR_EVENT_STATUS, tSubEventIndex);
				
				//�˴��������break
			}
			
			#if( SEL_PRG_INFO_Q_COMBO == YES )// �޹���Ϸ�ʽ��̼�¼
			if(inEventIndex == ePRG_Q_COMBO_NO )
			{
				api_ClrContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.dwPrgQComBo2Num ),(sizeof(DWORD)+(sizeof(TRealTimer)*2)) );
			}	
			#endif			
		}
	}

}

//-----------------------------------------------
//��������: ���¼�������������
//����:
//		OI[in]:	�¼�OI
//  	No[in]:	�ڼ���OAD��0����ȫ��OAD
//  	Len[in]:��Լ�㴫�����ݳ��ȣ������˳��ȷ���FALSE
//  	pBuf[out]:�������ݵ�ָ�� ���ն������ڣ�OAD���洢���˳�򷵻أ�ÿ��DWORD���ܹ�3*4�ֽ�
//
//����ֵ:
//		ֵΪ0x8000 ��������ִ��� OI��֧�֣���ȡEpprom��������⣩
//		ֵΪ0x0000 ������buf���Ȳ���
//		����:		�������ݳ���( �ڸ���Ϊ0ʱ�����1�ֽڵ�0 ���س���Ϊ1)
//��ע: �˴���Ҫʹ��EventAddrLen.SubEventIndex,��Щ�¼���ȡ�Ŀ��ܲ���!!!!!!
//-----------------------------------------------
WORD api_ReadEventAttribute( WORD OI, BYTE No, WORD Len, BYTE *pBuf )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	WORD wLenTemp;
	BYTE i;
	
	if( GetEventIndex( OI, &i ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.EventIndex = i;
	EventAddrLen.Phase = 0;
	//����Phase���ܲ�Ϊ0���˴���Ҫʹ��EventAddrLen.SubEventIndex,��Щ�¼���ȡ�Ŀ��ܲ���!!!!!!
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	if( No == 0 )
	{
		wLenTemp = 0;
		for( i = 0; i < EventAttInfo.NumofOad; i++ )
		{
			wLenTemp += sizeof(DWORD);
			if( wLenTemp > Len )
			{
				return FALSE;
			}
			memcpy( pBuf+wLenTemp-sizeof(DWORD), (BYTE *)&EventAttInfo.Oad[i], sizeof(DWORD) );
		}

		if( wLenTemp == 0 )
		{
			pBuf[0] = 0;
			return 1;
		}
		else
		{
			return wLenTemp;			
		}
	}
	else
	{
		if( No > EventAttInfo.NumofOad )
		{
			pBuf[0] = 0;
			return 1;
		}
		if( sizeof(DWORD) > Len )
		{
			return FALSE;
		}
		memcpy( pBuf, (BYTE *)&EventAttInfo.Oad[No-1], sizeof(DWORD) );

		return sizeof(DWORD);
	}
}

//-----------------------------------------------
//��������: ����¼�������������
//
//����:
//		OI[in]: 	�¼�OI
//		pOad[in]:	������������ʱ��OAD
//
//����ֵ:	TRUE/FALSE
//��ע:   ���һ��OAD����ɾ�����ݣ�������Χ���ش���
//�˴���Ҫʹ��EventAddrLen.SubEventIndex,��Щ�¼���ȡ�Ŀ��ܲ���!!!!!!
//-----------------------------------------------
BOOL api_AddEventAttribute( WORD OI, BYTE *pOad )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOad;
	WORD Len,RecordDepth;
	BYTE i,tEventIndex;
	TEventSectorInfo tEventSectorInfo;
	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = 0;
	//����Phase���ܲ�Ϊ0���˴���Ҫʹ��EventAddrLen.SubEventIndex,��Щ�¼���ȡ�Ŀ��ܲ���!!!!!!
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	if( (EventAttInfo.NumofOad+1) > MAX_EVENT_OAD_NUM )
	{
		return FALSE;	
	}
	memcpy( (BYTE *)&EventAttInfo.Oad[EventAttInfo.NumofOad], pOad, sizeof(DWORD) );

    for( i=0; i<EventAttInfo.NumofOad; i++ )//������������ͬ��OAD
    {
        if( EventAttInfo.Oad[EventAttInfo.NumofOad] == EventAttInfo.Oad[i] )
        {
            return FALSE;
        }
    }

	dwOad = EventAttInfo.Oad[EventAttInfo.NumofOad];
	dwOad &= ~0x00E00000;
	// ���������ȫ��ʱ�α�,���������ȫ���ڼ��ձ��
	if( (dwOad==0x00021640) || (dwOad==0x00021740)  || (dwOad==0x00021140) )
	{
		return FALSE;
	}
	else
	{
		Len = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 ); 
		if( Len < 0xff )
		{
			EventAttInfo.OadLen[EventAttInfo.NumofOad] = Len;
		}
		else
		{
			return FALSE;
		}
		EventAttInfo.OadValidDataLen += EventAttInfo.OadLen[EventAttInfo.NumofOad];
	}
	EventAttInfo.NumofOad += 1;
	
	EventAttInfo.AllDataLen = (EventAttInfo.OadValidDataLen+sizeof(TEventOADCommonData)+EventInfoTab[EventAddrLen.EventIndex].ExtDataLen);
	//��Խ��
	if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
	{
		return FALSE;
	}
	//����͹㲥Уʱ�¼������Ϊ100�������¼�Ϊ10
	if( (tEventIndex == eEVENT_LOST_POWER_NO ) 
	|| (tEventIndex == ePRG_BROADJUST_TIME_NO ))
	{
		RecordDepth = EVENT_LOSTPOWERRECORD_NUM;
	}
	else
	{
		RecordDepth = MAX_EVENTRECORD_NUMBER;
	}
	//��ȡ���¼����ݹ����������Ա����ô洢�ռ�
	if(GetEventSectorInfo(EventAttInfo.AllDataLen, RecordDepth, RecordDepth, &tEventSectorInfo) != FALSE)
	{
		//���¼�������Ĵ洢�ռ����ÿ���¼���������������false������������
		if( tEventSectorInfo.dwSaveSpace > EVENT_RECORD_LEN )
		{
			return FALSE;
		}
		else
		{
			EventAttInfo.MaxRecordNo = RecordDepth;//��ֵ����¼��
		}
	}
	else
	{
		return FALSE;
	}
	api_WriteToContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	// ��������
	ClearEventData( tEventIndex );
	//���Ӧ�������¼��б��е��ϱ��¼�
	ClrReportAllChannelIndex( tEventIndex );
	return TRUE;
}

//-----------------------------------------------
//��������: ɾ���¼�������������
//
//����:
//		OI[in]:		�¼�OI
//		pOad[in]:	Ҫɾ����OAD
//����ֵ:	TRUE/FALSE
//��ע:   ɾ��һ��OAD������EVENT_DEL_DATA_FUNC���ÿ��Ƿ�ɾ������
//�˴���Ҫʹ��EventAddrLen.SubEventIndex,��Щ�¼���ȡ�Ŀ��ܲ���!!!!!!
//-----------------------------------------------
BOOL api_DeleteEventAttribute( WORD OI, BYTE *pOad )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	BYTE i,j,tEventIndex;
	DWORD dwOad;
	WORD RecordDepth;
	TEventSectorInfo tEventSectorInfo;
	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}

	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = 0;
	//����Phase���ܲ�Ϊ0���˴���Ҫʹ��EventAddrLen.SubEventIndex,��Щ�¼���ȡ�Ŀ��ܲ���!!!!!!
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD) );
	for( i = 0; i < EventAttInfo.NumofOad; i++ )
	{
		if( dwOad == EventAttInfo.Oad[i] )
		{
			//ɾ��OADͬʱɾ����������		
			for( j = i; j < (EventAttInfo.NumofOad-1); j++ )
			{
				EventAttInfo.Oad[j] = EventAttInfo.Oad[j+1];
				EventAttInfo.OadLen[j] = EventAttInfo.OadLen[j+1];
			}
			EventAttInfo.Oad[EventAttInfo.NumofOad-1] = 0;
			EventAttInfo.OadLen[EventAttInfo.NumofOad-1] = 0;			
			break;
		}
	}

	if( i == EventAttInfo.NumofOad )
	{
		return FALSE;
	}
		
	EventAttInfo.NumofOad -= 1;
	EventAttInfo.OadValidDataLen = 0;

	for( i = 0; i < EventAttInfo.NumofOad; i++ )
	{
		dwOad = EventAttInfo.Oad[i];
		EventAttInfo.OadValidDataLen += api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
	}
	
	EventAttInfo.AllDataLen = (EventAttInfo.OadValidDataLen+sizeof(TEventOADCommonData)+EventInfoTab[EventAddrLen.EventIndex].ExtDataLen);

	//��Խ��
	if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
	{
		return FALSE;
	}
	
	//����͹㲥Уʱ�¼������Ϊ100�������¼�Ϊ10
	if( (tEventIndex == eEVENT_LOST_POWER_NO ) 
	|| (tEventIndex == ePRG_BROADJUST_TIME_NO ))
	{
		RecordDepth = EVENT_LOSTPOWERRECORD_NUM;
	}
	else
	{
		RecordDepth = MAX_EVENTRECORD_NUMBER;
	}
	//��ȡ���¼����ݹ����������Ա����ô洢�ռ�
	if(GetEventSectorInfo(EventAttInfo.AllDataLen, RecordDepth, RecordDepth, &tEventSectorInfo) != FALSE)
	{
		//���¼�������Ĵ洢�ռ����ÿ���¼���������������false������������
		if( tEventSectorInfo.dwSaveSpace > EVENT_RECORD_LEN )
		{
			return FALSE;
		}
		else
		{
			EventAttInfo.MaxRecordNo = RecordDepth;//��ֵ����¼��
		}
	}
	else
	{
		return FALSE;
	}
	
	api_WriteToContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);

	// ��������
	ClearEventData( tEventIndex );
	//���Ӧ�������¼��б��е��ϱ��¼�
	ClrReportAllChannelIndex( tEventIndex );
	
	return TRUE;
}

//-----------------------------------------------
//��������: �����¼�������������
//
//����:
//		OI[in]:	�¼�OI
//  	OadType[in]:
//				0 ~ �������е�OAD
//				N ~ �����б���ĵ�N��OAD  ���õ�N��OADʱ����Ϊ��N��OAD֮ǰ����ֵ��
//				0xFF ��ʼ��
//  	pOad[in]:  ָ��OAD Buffer��ָ��  2�ֽڶ�������+4�ֽ�OAD+2�ֽڴ洢���
//  	OadNum[in]:	OAD�ĸ���
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//��ע:	���е����¼ƣ����·���EEP�ռ�  ֻ֧���������е�OAD,�൱���޸�ָ��OAD
//�˴���Ҫʹ��EventAddrLen.SubEventIndex,��Щ�¼���ȡ�Ŀ��ܲ���!!!!!!
//-----------------------------------------------
BOOL api_WriteEventAttribute( WORD OI, BYTE OadType, BYTE *pOad, WORD OadNum )
{
	TEventAttInfo	EventAttInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOad;
	WORD Len,RecordDepth;
	BYTE i,j,n,tEventIndex;
	TEventSectorInfo tEventSectorInfo;
	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = 0;
	//����Phase���ܲ�Ϊ0���˴���Ҫʹ��EventAddrLen.SubEventIndex,��Щ�¼���ȡ�Ŀ��ܲ���!!!!!!
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	if( OadNum>MAX_EVENT_OAD_NUM )//OadNum =0����� ������ʹ��pOad -jwh
	{
		return FALSE;
	}

	if( OadType > MAX_EVENT_OAD_NUM )
	{
		if( OadType != 0xFF )
		{
			return FALSE;
		}
	}

	//api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);//��ȡ�¼�������Ϣ
    
	if( (OadType==0) || (OadType==0xFF) )
	{
        memset( (BYTE *)&EventAttInfo, 0x00, sizeof(TEventAttInfo) );//���¼�������Ϣ����
    	
		for( i = 0; i < OadNum; i++ )
		{
			if( i == 0 )
			{
				if( OadType == 0xFF )
				{
					//��һ��������oad����
					memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD));
					if( dwOad == 0 )
					{
						break;	
					}					
					continue;
				}
			}
			
			if( OadType == 0xFF )
			{
				j = i-1;
				lib_ExchangeData( (BYTE *)&EventAttInfo.Oad[j], pOad+i*sizeof(DWORD), sizeof(DWORD));
			}
			else
			{
				j = i;
				memcpy( (BYTE *)&EventAttInfo.Oad[j], pOad+i*sizeof(DWORD), sizeof(DWORD));
			}
			
			for( n=0; n<j; n++ )//������������ͬ��OAD
			{
				if( EventAttInfo.Oad[j] == EventAttInfo.Oad[n] )
				{
					return FALSE;
				}
			}

			dwOad = EventAttInfo.Oad[j];
			dwOad &= ~0x00E00000;
			
			// ���������ȫ��ʱ�α�,���������ȫ���ڼ��ձ��
			if( (dwOad==0x00021640) || (dwOad==0x00021740)  || (dwOad==0x00021140) )
			{
				return FALSE;
			}
			else
			{
				Len = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
			}

			if( Len > 0xff )
			{
				ASSERT(0!=0,1);
				return FALSE;
			}
			
			EventAttInfo.OadLen[j] = Len;
						
			EventAttInfo.OadValidDataLen += EventAttInfo.OadLen[j];
			EventAttInfo.NumofOad++;
		}
		
		EventAttInfo.AllDataLen = (EventAttInfo.OadValidDataLen+sizeof(TEventOADCommonData)+EventInfoTab[EventAddrLen.EventIndex].ExtDataLen);

		//��Խ��
		if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
		|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
		{
			return FALSE;
		}
	
		//����͹㲥Уʱ�¼������Ϊ100�������¼�Ϊ10
		if( (tEventIndex == eEVENT_LOST_POWER_NO ) 
		|| (tEventIndex == ePRG_BROADJUST_TIME_NO ))
		{
			RecordDepth = EVENT_LOSTPOWERRECORD_NUM;
		}
		else
		{
			RecordDepth = MAX_EVENTRECORD_NUMBER;
		}
		//��ȡ���¼����ݹ����������Ա����ô洢�ռ�
		if(GetEventSectorInfo(EventAttInfo.AllDataLen, RecordDepth, RecordDepth, &tEventSectorInfo) != FALSE)
		{
			//���¼�������Ĵ洢�ռ����ÿ���¼���������������false������������
			if( tEventSectorInfo.dwSaveSpace > EVENT_RECORD_LEN )
			{
				return FALSE;
			}
			else
			{
				EventAttInfo.MaxRecordNo = RecordDepth;//��ֵ����¼��
			}
		}
		else
		{
			return FALSE;
		}
		
		api_WriteToContinueEEPRom( EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo );
	}
	else
	{
		return FALSE;
	}

	// ��������
	ClearEventData( tEventIndex );
	//���Ӧ�������¼��б��е��ϱ��¼�
	ClrReportAllChannelIndex( tEventIndex );
	
	return TRUE;
}


//-----------------------------------------------
//��������: ��ȡ����ʱ��
//
//����: 
//	PowerDownTime[out]:	����ʱ�� 
//  
//����ֵ:				��  
//��ע:
//-----------------------------------------------
void api_GetPowerDownTime(TRealTimer *pPowerDownTime)
{
	DWORD		dwAddr;
	TLostPowerTime LostPowerTime;

	dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.LostPowerTime );
	api_VReadSafeMem( dwAddr, sizeof(TLostPowerTime) , LostPowerTime.Time );	
	memcpy( (BYTE*)pPowerDownTime, LostPowerTime.Time, sizeof(TRealTimer));
}

//-----------------------------------------------
//��������: д�¼�������,����ַУ��
//
//����: 
//			Addr[in]		Ҫд����ʼ��ַ
//			Length[in]		Ҫд�ĳ���
//			BaseAddr[in]	������¼�����Ļ���ַ
//			MemSpace[in]	������¼�������Ŀռ�
//			pBuf[in/out]	д����ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:�˺���ֻдFlash�¼�������
//-----------------------------------------------
BOOL WriteEventRecordData(DWORD Addr, WORD Length, DWORD BaseAddr, DWORD MemSpace, BYTE * pBuf)
{
	if((Addr >= BaseAddr) && ((Addr+Length) <= (BaseAddr+MemSpace)))
	{
		return api_WriteMemRecordData( Addr, Length, pBuf );
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
//��������: ���湤�����¼���¼��ʧѹʧ���ȣ�
//
//����: 
//	inEventIndex[in]:	eEVENT_INDEX 
//	Phase[in]:			ePHASE_TYPE  
//	BeforeAfter[in]:	�¼�����ǰ/������ eEVENT_TIME_TYPE 
//	EndTimeType[in]:	eEVENT_ENDTIME_CURRENT  ��ǰʱ��
// 						eEVENT_ENDTIME_PWRDOWN  ����ʱ��
//  
//����ֵ:				TRUE/FALSE  
//��ע:
//-----------------------------------------------
BOOL SaveInstantEventRecord( BYTE inEventIndex, BYTE Phase, BYTE BeforeAfter, BYTE EndTimeType )
{
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;	
	BYTE i,Temp,Sec,OADIndex,OADAttribute;
	WORD Len;
	BYTE TmpBuf[EVENT_VALID_OADLEN+30];
	TRealTimer	Time;
	WORD Offset,OI;
	DWORD dwOad,dwTemp,dwTemp1,dwAddr,dwMin,dwBaseAddr;	
	BYTE Buf[16],Buf1[16];	//����TDemandData����,��С��16
	TEventSectorInfo EventSectorInfo;
	
	EventAddrLen.EventIndex = inEventIndex;
	EventAddrLen.Phase = Phase;
	
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	if(api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo) == FALSE)
	{
		return FALSE;
	}

	if( (EventAttInfo.AllDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.OadValidDataLen > EVENT_VALID_OADLEN) 
	|| (EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM) )
	{
		return FALSE;
	}

	Offset = 0;
	//����ַ���ڴ�OAD�Ļ���ַ;
	dwBaseAddr =  EventAddrLen.dwRecordAddr;
	if( BeforeAfter == EVENT_START )
	{
		//��ȡ�����¼���ƫ�Ƶ�ַ
		GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,EventDataInfo.RecordNo, &EventSectorInfo);
		
		memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) );

		EventDataInfo.RecordNo++;// һֱ����
		api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		
		EventOADCommonData.EventNo = EventDataInfo.RecordNo;
		if( EndTimeType == eEVENT_ENDTIME_CURRENT )
		{
			// �Ե�ǰʱ����Ϊʱ�䷢��ʱ��
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.BeginTime );			
		}
		else
		{
		    api_GetPowerDownTime( (TRealTimer*)&EventOADCommonData.EventTime.BeginTime );
			// �Ե���ʱ����Ϊʱ�䷢��ʱ��
			#if( SEL_EVENT_LOST_ALL_V == YES )
			if( inEventIndex == eEVENT_LOST_ALL_V_NO )//ȫʧѹ�¼�
			{
			    //����ȫʧѹ��ʼʱ�䣬Ĭ��ʱ��+1���ӣ�ʹ����Է��������д���������ӳ�������
			    dwMin = api_CalcInTimeRelativeMin( (TRealTimer*)&EventOADCommonData.EventTime.BeginTime );
			    dwMin += 1;
			    Sec = EventOADCommonData.EventTime.BeginTime.Sec;
			    api_ToAbsTime(dwMin, (TRealTimer*)&EventOADCommonData.EventTime.BeginTime);
			    EventOADCommonData.EventTime.BeginTime.Sec = Sec;
			}	
			#endif
		}
		// �����ȴ�ʱ��,��¼���,�ϱ�״̬���ٴ����ݣ�������ָ��Ϊ0ʱ������������
		dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
		if(WriteEventRecordData( dwAddr, sizeof(TEventOADCommonData), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData ) == FALSE)
		{
		}
		// �濪ʼOAD��Ӧ������
		if( EventAttInfo.NumofOad != 0 )
		{
			//��ȫ��FF�������������ݴ�ΪFF���¼�����ʱ�ٱ����������
			memset( TmpBuf, 0xff, EventAttInfo.OadValidDataLen );

			for( i = 0; i < EventAttInfo.NumofOad; i++ )
			{
				Temp = (BYTE)((EventAttInfo.Oad[i]&0x00E00000)>>21);
				dwOad = EventAttInfo.Oad[i];
				dwOad &= ~0x00E00000;

				//OAD��������bit5~7:1���¼�����ǰ 2���¼������� 3���¼�����ǰ 4���¼�������
				if( (Temp != 3) && (Temp != 4) )//�������������Ϊ3,4��Ĭ�ϰ���1,2,���д�������������
				{
					#if( SEL_EVENT_LOST_ALL_V == YES )
					if( inEventIndex == eEVENT_LOST_ALL_V_NO )
					{
						OI = (WORD)(dwOad&0xFFFF);
						lib_InverseData((BYTE *)&OI, sizeof(WORD));

						if(OI == 0x2001)
						{
							OADAttribute = ((dwOad >> 16) & 0x1f);
							OADIndex = (dwOad >> 24);
							if(OADAttribute == 2)
							{
								if(OADIndex == 0)
								{
									memcpy( TmpBuf+Offset, (BYTE *)&LostAllVData[0].Current[0], EventAttInfo.OadLen[i] );	
								}
								else if(OADIndex < 4)
								{
									memcpy( TmpBuf+Offset, (BYTE *)&LostAllVData[0].Current[OADIndex - 1], EventAttInfo.OadLen[i] );
								}
								else
								{
								}
							}
							else if(OADAttribute == 4)
							{
								memcpy( TmpBuf+Offset, (BYTE *)&LostAllVData[0].Current[3], EventAttInfo.OadLen[i] );	
							}
							else
							{
								api_GetProOadData( eGetRecordData,eData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, TmpBuf+Offset );// ����
							}
							
						}
						else if( (OI>=0x2000) && (OI<=0x200A) )// �������������˲ʱ��
						{
							memset( TmpBuf+Offset, 0x00, EventAttInfo.OadLen[i] );		
						}
						else
						{
							api_GetProOadData( eGetRecordData,eData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, TmpBuf+Offset );		// ����
						}
					}
					else
					#endif
					{
						api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, TmpBuf+Offset );	
					}
				}
				else
				{
                    #if( MAX_PHASE != 1 )
					OI = (WORD)(dwOad&0xFFFF);
					lib_InverseData((BYTE *)&OI, sizeof(WORD));
					if( OI == EVENT_AHOUR_OI )
					{
						if( ((inEventIndex-eEVENT_LOST_V_NO)<4) && ((Phase-ePHASE_A)<3) )//(Phase-ePHASE_A)�����0-1����������ִ����������д������쳣
						{	
							// �¼�����ʱ�氲ʱֵ�ĳ�ʼֵ
							api_GetProOadData( eGetRecordData,eData, 0xff, (BYTE *)&dwOad, NULL, sizeof(Buf), Buf );		
							api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.AHour[inEventIndex-eEVENT_LOST_V_NO][Phase-ePHASE_A][0] ),
													sizeof(DWORD)*4, Buf );	
						}
					}
                    #endif
				}
				Offset += EventAttInfo.OadLen[i];
			}
			
			dwAddr = EventAddrLen.dwRecordAddr+sizeof(TEventOADCommonData)+EventSectorInfo.dwSectorAddr;
			if(WriteEventRecordData( dwAddr, EventAttInfo.OadValidDataLen, dwBaseAddr, EVENT_RECORD_LEN, TmpBuf ) == FALSE)// �濪ʼOAD��Ӧ������
			{
			
			}
		}	
		
		SetReportIndex( EventAddrLen.SubEventIndex, EVENT_START );					
	}
	else//if( BeforeAfter == EVENT_START )
	{						
		if(EventDataInfo.RecordNo == 0)
		{
			return FALSE;
		}
		
		//��ȡ�����¼���ƫ�Ƶ�ַ
		GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,(EventDataInfo.RecordNo-1), &EventSectorInfo);

		// �����OAD��Ӧ������
		if( EventAttInfo.NumofOad != 0 )
		{

			memset( TmpBuf, 0xff, EventAttInfo.OadValidDataLen );
			
			dwAddr = EventAddrLen.dwRecordAddr+sizeof(TEventOADCommonData)+EventSectorInfo.dwSectorAddr;
			api_ReadMemRecordData( dwAddr, EventAttInfo.OadValidDataLen, TmpBuf );

			for( i = 0; i < EventAttInfo.NumofOad; i++ )
			{
				Temp = (BYTE)((EventAttInfo.Oad[i]&0x00E00000)>>21);
				dwOad = EventAttInfo.Oad[i];
				dwOad &= ~0x00E00000; 

				if( (Temp==3) || (Temp==4) )
				{                    
					if( (dwOad&0x000fffff) == AHOUR_OAD )
					{
						#if( MAX_PHASE != 1 )
						// �õ���ǰ��ʱֵ
						api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, sizeof(Buf), Buf );
						if( ((inEventIndex-eEVENT_LOST_V_NO)<4) && ((Phase-ePHASE_A)<3) )
						{
							api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.AHour[inEventIndex-eEVENT_LOST_V_NO][Phase-ePHASE_A][0] ),
								sizeof(DWORD)*4, Buf1 );
						}
						else
						{
							memset( Buf1, 0x00, sizeof(DWORD)*4);
						}
						

						for( Temp=0; Temp<(EventAttInfo.OadLen[i]/sizeof(DWORD)); Temp++ )
						{							
                            memcpy( (BYTE *)&dwTemp1, Buf+Temp*sizeof(DWORD), sizeof(DWORD) );                //��ǰ��ʱֵ
							memcpy( (BYTE *)&dwTemp, Buf1+Temp*sizeof(DWORD), sizeof(DWORD) );				//��ǰ��ʱֵ
							
							if( dwTemp1 >= dwTemp )
							{
								dwTemp1 = dwTemp1-dwTemp;	
							}
							else
							{
								dwTemp1 = 0;
							}

							memcpy( TmpBuf+Offset+Temp*sizeof(DWORD), (BYTE *)&dwTemp1, sizeof(DWORD) );	// ���ڼ䰲ʱֵ
						}
						#endif
					}                    
					#if( SEL_EVENT_V_UNBALANCE == YES )
					else if( dwOad == V_UNBALANCE_OAD )
					{
						if( EndTimeType == eEVENT_ENDTIME_CURRENT )// ��ǰʱ��
						{
							// ��ȡRAM��ĵ�ѹ��ƽ����
							OI = GetUnBalanceMaxRate(0x00);
						}
						else
						{
							// ��ȡEEP��ĵ�ѹ��ƽ����
							OI = GetUnBalanceMaxRate(0x80);
						}
						memcpy( TmpBuf+Offset, (BYTE *)&OI, sizeof(WORD) );
					}
					#endif
					#if( (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
					else if( dwOad == I_UNBALANCE_OAD )
					{
						// �������ز�ƽ��
						if( EventAddrLen.EventIndex == eEVENT_I_S_UNBALANCE_NO )	
						{
							if( EndTimeType == eEVENT_ENDTIME_CURRENT )// ��ǰʱ��
							{
								// ��ȡRAM��ĵ�����ƽ����
								OI = GetUnBalanceMaxRate(0x02);
							}
							else
							{
								// ��ȡEEP��ĵ�����ƽ����
								OI = GetUnBalanceMaxRate(0x82);
							}
						}
						else// ������ƽ��
						{
							if( EndTimeType == eEVENT_ENDTIME_CURRENT )// ��ǰʱ��
							{
								// ��ȡRAM��ĵ������ز�ƽ����
								OI = GetUnBalanceMaxRate(0x01);
							}
							else
							{
								// ��ȡEEP��ĵ������ز�ƽ����
								OI = GetUnBalanceMaxRate(0x81);
							}
						}
						memcpy( TmpBuf+Offset, (BYTE *)&OI, sizeof(WORD) );
					}
					#endif
					else
					{
						api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, TmpBuf+Offset );	
					}					
				}
				Offset += EventAttInfo.OadLen[i];
			}

			if(WriteEventRecordData( dwAddr, EventAttInfo.OadValidDataLen, dwBaseAddr, EVENT_RECORD_LEN, TmpBuf ) == FALSE)
			{
			
			}
		}
		
		if( EndTimeType == eEVENT_ENDTIME_CURRENT )
		{
			// ��ǰʱ��
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.EndTime );									
		}
		else
		{
			// ����ʱ��
			api_GetPowerDownTime( (TRealTimer*)&EventOADCommonData.EventTime.EndTime );
			#if( SEL_EVENT_LOST_ALL_V == YES )
			if( inEventIndex == eEVENT_LOST_ALL_V_NO )//ȫʧѹ�¼�
			{
			    //����ȫʧѹ��ʼʱ�䣬Ĭ��ʱ��+1���ӣ�ʹ����Է��������д���������ӳ�������
			    dwMin = api_CalcInTimeRelativeMin( (TRealTimer*)&EventOADCommonData.EventTime.EndTime );
			    dwMin += 1;
			    Sec = EventOADCommonData.EventTime.EndTime.Sec;
			    api_ToAbsTime(dwMin, (TRealTimer*)&EventOADCommonData.EventTime.EndTime);
			    EventOADCommonData.EventTime.EndTime.Sec = Sec;
			}	
			#endif
		}

		// �����ʱ��
		dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
		dwAddr += GET_STRUCT_ADDR( TEventOADCommonData, EventTime.EndTime );
		if(WriteEventRecordData( dwAddr, sizeof(TRealTimer), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData.EventTime.EndTime ) == FALSE)
		{

		}
		dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
		dwAddr += GET_STRUCT_ADDR( TEventOADCommonData, EventTime.BeginTime );
		api_ReadMemRecordData( dwAddr, sizeof(TRealTimer) , (BYTE *)&Time );

		// �����ۼ�ʱ��
		// ����ʱ����ȷ
		if( api_CheckClock((TRealTimer *)&EventOADCommonData.EventTime.EndTime) == TRUE )	
		{
			// ��ʼʱ����ȷ
			if( api_CheckClock(&Time) == TRUE )					
			{
				// ����ʱ���Ӧ�������
				dwTemp = api_CalcInTimeRelativeSec( (TRealTimer *)&EventOADCommonData.EventTime.EndTime );			
				// ����ʱ�̶�Ӧ�������
				dwAddr = api_CalcInTimeRelativeSec( &Time );							

				if( dwTemp >= dwAddr )
				{
					dwTemp = dwTemp - dwAddr;
				}
				else
				{
					dwTemp = 0;
				}
				EventDataInfo.AccTime += dwTemp;
				api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);	
			}
		}

		#if( SEL_DEMAND_OVER == YES )
		if( 	(inEventIndex==eEVENT_PA_DEMAND_OVER_NO) 
			|| 	(inEventIndex==eEVENT_NA_DEMAND_OVER_NO) 
			|| 	(inEventIndex==eEVENT_RE_DEMAND_OVER_NO) )
		{
			// �������+����ʱ��
			if( inEventIndex == eEVENT_PA_DEMAND_OVER_NO )
			{
				dwAddr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue);
			}
			else if( inEventIndex == eEVENT_NA_DEMAND_OVER_NO )
			{
				dwAddr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue);
			}
			else// if( inEventIndex == eEVENT_RE_DEMAND_OVER_NO )
			{
				dwAddr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[Phase]);
			}

			// �����������+����ʱ��
			api_ReadFromContinueEEPRom( dwAddr, sizeof(TDemandData) , Buf );

			dwAddr = EventAddrLen.dwRecordAddr+sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;
			//���ܰ���GET_STRUCT_ADDR(TDemandOverRom,Data)��ʽ�����Data��һ���ڴ˿ռ�
			//dwAddr += GET_STRUCT_ADDR(TDemandOverRom,Data);
			dwAddr += EventAttInfo.OadValidDataLen;
			if(WriteEventRecordData( dwAddr, sizeof(TDemandData), dwBaseAddr, EVENT_RECORD_LEN, Buf ) == FALSE)
			{

			}
		}
		#endif
		
		SetReportIndex( EventAddrLen.SubEventIndex, EVENT_END );	
	}
	api_LostPowerReportFraming( EventAddrLen.SubEventIndex ,EventDataInfo.RecordNo );
	return TRUE;
}

//-----------------------------------------------
//��������: ����OAD������
//
//����: 
//		pDLT698RecordPara[in]��ָ��ṹ��TDLT698RecordPara��ָ��              
//		pEventOADCommonData[in]��ָ��ṹ��TEventOADCommonData��ָ��                
//		pEventAttInfo[in]��ָ��ṹ��TEventAttInfo��ָ��                
//		pEventAddrLen[in]��ָ��ṹ��TEventAddrLen��ָ��                
//		dwPointer[in]����ַƫ��ָ��                
//		pBuf[out]���������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0                
//		dwOad[in]��OAD                
//		Len[in]�������buf���� �����ж�buf�����Ƿ���                
//		Tag[in]��0 ����Tag  1 ��Tag                
//		off[in]���Ѿ��е�ƫ��                
//		status[in]���¼�������ʶ                
//		AllorSep[in]��1 ָ��OAD 0 ����OAD
// 		Last[in]: ��N��
//����ֵ:	0xFFFF: ���������ߴ��� ����ֵ���������ݳ���             
//��ע: 
//-----------------------------------------------
static WORD GetDataByOad( TEventData *pData )
{
	DWORD dwTemp,dwTemp1,dwTemp2,dwOad,dwTemp3,dwTemp4;
	WORD wLen,wLen1,wLenTemp,w,Result;
	BYTE Temp,i,OadIndex,tmpBuf[EVENT_ALONE_OADLEN+30];	
	BYTE *pTemp;
	BYTE Buf[30],Buf1[30];
	TPrgProgramSubRom	OadList;

	if( pData->dwOad == STARTTIME_OAD )
	{
		//������ʱ�䶼Ϊff������Ϊnull
		if( lib_IsAllAssignNum( (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, 0xff, sizeof(TRealTimer) ) == TRUE )
		{
			if( (pData->off+1) > pData->Len )
			{
				return 0xFFFF;// ����
			}
			wLen = 1;
			pData->pBuf[pData->off] = 0;
		}
		else
		{
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// ����
			}
			
			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetNormalData, eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, 0xFF, pData->pBuf+pData->off );	
			}
			else
			{
				memcpy( pData->pBuf+pData->off, (BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, wLen );		
			}
		}				
	}
	else if( pData->dwOad == ENDTIME_OAD )
	{
		//������ʱ�䶼Ϊff������Ϊnull eNUM_OF_EVENT
		//Уʱ��¼����¼����ʱ��
		if( (( lib_IsAllAssignNum((BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, 0xff, sizeof(TRealTimer) ) == TRUE )
			||	( pData->pEventAddrLen->EventIndex>=ePRG_RECORD_NO ))
			#if( SEL_PRG_INFO_ADJUST_TIME == YES )
			&&( pData->pEventAddrLen->EventIndex!=ePRG_ADJUST_TIME_NO)
			#endif
			#if( SEL_PRG_INFO_BROADJUST_TIME == YES )
			&&( pData->pEventAddrLen->EventIndex!=ePRG_BROADJUST_TIME_NO)
			#endif
		 )
		{
			if( (pData->off+1) > pData->Len )
			{
				return 0xFFFF;// ����
			}
			wLen = 1;
			pData->pBuf[pData->off] = 0;
		}
		else
		{
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// ����
			}
			
			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetNormalData, eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, 0xFF, pData->pBuf+pData->off );	
			}
			else
			{
				memcpy( pData->pBuf+pData->off, (BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, wLen );		
			}
		}
	}
	else if( pData->dwOad == RECORDNO_OAD )
	{
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// ����
		}

		if( pData->pEventOADCommonData->EventNo != 0 )
		{
			dwTemp = pData->pEventOADCommonData->EventNo - 1;
		}
		else
		{
			dwTemp = 0;
		}
		
		if( pData->Tag == eTagData )
		{
			api_GetProOadData( eGetNormalData, eAddTag, 0xff, (BYTE *)&pData->dwOad, (BYTE *)&dwTemp, 0xFF, pData->pBuf+pData->off );			
		}
		else
		{
			memcpy( pData->pBuf+pData->off, (BYTE *)&dwTemp, wLen );
		}
	}
	else if( pData->dwOad == GEN_SOURCE_OAD )
	{
		// �������¼����¼�����Դ
		if( ((pData->pDLT698RecordPara->OI >= 0x3000) && (pData->pDLT698RecordPara->OI<= 0x3008)) 
		|| (pData->pDLT698RecordPara->OI == 0x303B) )//�ӿ���24  class_id=24
		{
			if( pData->AllorSep == 0 )
			{
				wLen = 0;
			}
			else
			{
				wLen = 1;
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// ����
				}

				pData->pBuf[pData->off] = 0;	// ֻ��һ��0������NULL
			}
		}
		else
		{
			//����¼�����Դֻ��3023	���ܱ��޹���Ϸ�ʽ����¼� ��Ч 
			//�¼�����Դ��=enum{�޹���Ϸ�ʽ1�����֣�0�����޹���Ϸ�ʽ2�����֣�1��}
			#if( SEL_PRG_INFO_Q_COMBO == YES )
			if( pData->pEventAddrLen->EventIndex == ePRG_Q_COMBO_NO )
			{
				wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// ����
				}
				
				dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.QcombSource[pData->pEventSectorInfo->dwPointer] );
				api_ReadFromContinueEEPRom( dwTemp, 1, (BYTE *)&Temp );
				
				if( Temp > 1 )
				{
					Temp = 0;
				}
				if( pData->Tag == eTagData )
				{
					api_GetProOadData( eGetRecordData, eAddTag,0xff,(BYTE *)&pData->dwOad, (BYTE *)&Temp, 2, (pData->pBuf+pData->off) );
				}
				else
				{
					pData->pBuf[pData->off] = Temp;
				}
			}
			else
			#endif
			{
				wLen = 1;
				if( (pData->off+wLen) > pData->Len )
				{
					return 0xFFFF;// ����
				}

				pData->pBuf[pData->off] = 0;	// ֻ��һ��0������NULL
			}
		}
	}
	else if( pData->dwOad == NOTIFICATION_OAD )
	{
		//����8Ҫ����ɲ�֧��ͨ���ϱ�����NULL
		pData->pBuf[pData->off] = 0;
		wLen = 1;
	}
	#if( SEL_DEMAND_OVER == YES )
	else if( (pData->dwOad==MAXDEMAND_OAD) || (pData->dwOad==MAXDEMAND_TIME_OAD) )
	{
		//�����ڼ��������ֵ/����ʱ��
		Temp = 0;
		if( pData->pDLT698RecordPara->OI == EVENT_PACTIVE_DEMANDOVER_OI )
		{
			Temp = 1;
			dwTemp1 = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue);
		}
		else if( pData->pDLT698RecordPara->OI == EVENT_NACTIVE_DEMANDOVER_OI )
		{
			Temp = 1;
			dwTemp1 = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue);
		}
		else if( pData->pDLT698RecordPara->OI == EVENT_REACTIVE_DEMANDOVER_OI )
		{
			Temp = 1;
			dwTemp1 = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[pData->pDLT698RecordPara->Phase]);
		}
		else
		{
			Temp = 0;
		}

		if( Temp == 1 )
		{
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// ����
			}

			if( (pData->Last==1) && (pData->status==1) )
			{
				// ��1�β���δ����
				api_ReadFromContinueEEPRom( dwTemp1, sizeof(TDemandData), Buf );
			}
			else
			{
				dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData) + pData->pEventSectorInfo->dwSectorAddr;
				dwTemp += pData->pEventAttInfo->OadValidDataLen;
				api_ReadMemRecordData( dwTemp, sizeof(TDemandData), Buf );							
			}

			if( pData->Tag == eTagData )
			{
				if( pData->dwOad == MAXDEMAND_OAD )
				{
					api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&pData->dwOad, Buf, 0xFF, pData->pBuf+pData->off );
				}
				else
				{
					api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&pData->dwOad, Buf+sizeof(DWORD), 0xFF, pData->pBuf+pData->off );
				}
			}
			else
			{
				if( pData->dwOad == MAXDEMAND_OAD )
				{
					memcpy( pData->pBuf+pData->off, Buf, wLen );
				}
				else
				{
					memcpy( pData->pBuf+pData->off, Buf+sizeof(DWORD), wLen );
				}
			}		
		}
		else
		{
			wLen = 1;
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// ����
			}
			pData->pBuf[pData->off] = 0;
		}
	}
	#endif
	else if( pData->dwOad == PRG_OBJ_LIST_OAD )
	{			
		//����һ�� �� ��̼�¼��û�н��� ʱ 
		if( (pData->Last==1) && (pData->status==1) )
		{
			//��ee�ݴ�������ж�ȡOAD�б�  ��Ҫ��֤�ڰ���ʱ���ȡʱ�Ƿ���ȷ������������
			dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );
			api_ReadFromContinueEEPRom(dwTemp, sizeof(OadList), (BYTE *)&OadList);			
		}
		else
		{
			//�ӱ������������ȡOAD�б�
			dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr+ pData->pEventAttInfo->OadValidDataLen;
			api_ReadMemRecordData( dwTemp, sizeof(OadList), (BYTE *)&OadList );
		}
		
		if( OadList.SaveOadNum > 10 )
		{
			return 0xffff;
		}			
		
		pTemp = pData->pBuf+pData->off;
		if( pData->Tag == eTagData )
		{			
			wLen = OadList.SaveOadNum*5+2;
			pTemp[0] = 1;	//array
			pTemp[1] = OadList.SaveOadNum;
			for(i=0; i<OadList.SaveOadNum; i++)
			{
				pTemp[2+i*5] = OadList.Type[i];
				memcpy(pTemp+2+i*5+1, (BYTE*)&OadList.Oad[0]+i*sizeof(DWORD), sizeof(DWORD) );
			}
		}
		else
		{
			wLen = OadList.SaveOadNum*4;
			memcpy( pTemp, OadList.Oad, OadList.SaveOadNum*sizeof(DWORD) );
		}			
	}
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
	else if( pData->dwOad == CLR_EVENT_LIST_OAD )
	{
		//�¼������б�  array OMD
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 1 );

		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// ����
		}
		
		wLenTemp = api_GetProOADLen( eGetNormalData, eData, (BYTE *)&pData->dwOad, 1 );// 4*N
		
		dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
		dwTemp +=pData->pEventAttInfo->OadValidDataLen;
		api_ReadMemRecordData( dwTemp, sizeof(DWORD), tmpBuf );
		
		if( pData->Tag == eTagData )
		{
			if( wLenTemp > (sizeof(tmpBuf) -1))//��ֹ����Խ��
			{
				return 0xFFFF;// ����
			}
			memmove( (tmpBuf+1), tmpBuf, wLenTemp );
			*tmpBuf = 1;	// ��һ���ֽ�Ϊarray�ĸ���,�̶�1���ֽ�
			api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&pData->dwOad, tmpBuf, 0xFF, pData->pBuf+pData->off );
		}
		else
		{
			memcpy(pData->pBuf+pData->off,tmpBuf,wLen);
		}
	}
	#endif
	#if( (SEL_PRG_INFO_HOLIDAY == YES)  || (SEL_PRG_INFO_TIME_TABLE == YES))// �ڼ��ձ��//ʱ�α���
	else if( (pData->dwOad==PRG_HOLIDAY_OAD)  || (pData->dwOad==PRG_HOLIDAY_VALUE) ||(pData->dwOad==PRG_SEGTABLE_OAD)  || (pData->dwOad==PRG_SEGTABLE_VALUE))
	{
		if( (pData->dwOad==PRG_SEGTABLE_OAD) || (pData->dwOad==PRG_SEGTABLE_VALUE) )
		{
			dwOad = 0x01021640;
		}
		else
		{
			dwOad = 0x01021140;
		}
		Temp = api_GetProOADLen( eGetNormalData, eData, (BYTE *)&dwOad, 0 );	

		// ��չ����һ���Զ�����Ҫ�Ļ��峤��
		wLen = Temp+sizeof(DWORD);
		
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// ����
		}

	    // ����չ����
	    dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
		dwTemp +=pData->pEventAttInfo->OadValidDataLen;
		api_ReadMemRecordData(dwTemp, wLen, tmpBuf);
	
	    // ��ȡ���OAD
		memcpy((BYTE*)&dwTemp,&tmpBuf[wLen-sizeof(DWORD)],sizeof(DWORD));
		// ����ǻ�ȡOAD�Ļ�
		if( (pData->dwOad==PRG_SEGTABLE_OAD) || (pData->dwOad==PRG_HOLIDAY_OAD) )
		{
			wLen=((pData->Tag)?(1+sizeof(DWORD)):(sizeof(DWORD)));
		}
		else
		{
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwTemp, 0 );
			Temp=0;
		}
		// ��黺���Ƿ�
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// ����
		}
		// ��������
		if( pData->Tag == eTagData )
		{
			if( (pData->dwOad==PRG_SEGTABLE_OAD) || (pData->dwOad==PRG_HOLIDAY_OAD))// OAD��tag�Ļ�ֱ�Ӵ���
			{
				tmpBuf[Temp-1]=0x51;// OAD
				memcpy(pData->pBuf+pData->off,&tmpBuf[Temp-1],wLen);
			}
			else
			{
				api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwTemp, tmpBuf, 0xFF, pData->pBuf+pData->off );// ʹ���޸Ĵ洢��OAD�����ݽ��м�tag
			}
		}
		else
		{
			memcpy(pData->pBuf+pData->off,&tmpBuf[Temp],wLen);
		}
	}
	#endif//#if( (SEL_PRG_INFO_HOLIDAY == YES)  || (SEL_PRG_INFO_TIME_TABLE == YES))// �ڼ��ձ��//ʱ�α���
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//{0x3027,9,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,CARD_SN_OAD,CARD_ERR_INFO_OAD,CARD_ORDER_HEAD_OAD,CARD_ERR_RES_OAD},//���ܱ��쳣�忨�¼�0x3027
	//{0x3029,6,RECORDNO_OAD,STARTTIME_OAD,ENDTIME_OAD,GEN_SOURCE_OAD,NOTIFICATION_OAD,RETURN_MONEY_OAD},	//���ܱ��˷Ѽ�¼3029	
	else if( (pData->dwOad==CARD_SN_OAD) 
			 ||(pData->dwOad==CARD_ERR_INFO_OAD) 
			 ||(pData->dwOad==CARD_ORDER_HEAD_OAD) 
			 ||(pData->dwOad==CARD_ERR_RES_OAD)
			 ||(pData->dwOad==RETURN_MONEY_OAD) )
	{
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&pData->dwOad, 1 );

		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// ����
		}
		
		wLen1 = api_GetProOADLen( eGetNormalData, eData, (BYTE *)&pData->dwOad, 1 );// 4*N
		
		dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
		dwTemp +=pData->pEventAttInfo->OadValidDataLen;
		if( (pData->dwOad & 0xffff ) == 0x1033 )//������쳣�忨������쳣�忨����Ŀ����кš�������Ϣ�֡�����ͷ��������Ӧ״̬��18�ֽ�ȫ������
		{
			wLen1 = 18;
		}		
		
		api_ReadMemRecordData( dwTemp, wLen1, tmpBuf );
         		
		if(pData->dwOad==CARD_ORDER_HEAD_OAD)
        {
        	Temp = 10;
        	if( tmpBuf[9] == 0x01 )
        	{
        		wLen = 8;
        		wLen1 = 6;//����+TAG  --���� Octet_string_698 �����2�ֽ�,�����������6�ֽ�,������+TAG����8�ֽڣ���Ϊ�� Tag Len or Type Len
        	}
        	else
        	{
        		wLen = 7;
        		wLen1 = 5;
        	}

        	if( pData->Tag == eTagData )
        	{
        		pData->pBuf[pData->off] = 9;//9 -- Octet_string_698; ����� Octet_string_698 ���벻��
        		pData->pBuf[pData->off+1] = wLen1;
        		memcpy(pData->pBuf+pData->off+2,tmpBuf+Temp,wLen1);
        	}
        	else
        	{
        		memcpy(pData->pBuf+pData->off,tmpBuf+Temp,wLen1);
        	}		
        }
        else
        {
        	 Temp = 0;//�����ڱ����������pAllocBuf�и������ݵ�ƫ��,��Ĭ��Ϊ0
	        //���ܱ��쳣�忨�¼� 18=8+1+7+2 ����7������ͷ������ͷ����ʱ���� CH,Cla,Ins,P1,P2,P31,P32,����CH:0x01--ESAM 0x00--CARD,esam����ͷΪCH���6�ֽڣ���ΪCH���5�ֽ�
	        if( pData->dwOad== CARD_ERR_INFO_OAD )
	        {
	        	Temp = 8;
	        } 
	        else if( pData->dwOad==CARD_ERR_RES_OAD )
	        {
	        	Temp = 16;
	        }	
        
			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&pData->dwOad, tmpBuf+Temp, 0xFF, pData->pBuf+pData->off );
			}
			else
			{
				memcpy(pData->pBuf+pData->off,tmpBuf+Temp,wLen1);
			}
		}

	}	
	#endif
	else
	{
		dwTemp1 = 0;
		OadIndex = 0;
		for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
		{
			if( pData->pEventAttInfo->Oad[i] == pData->dwOad )
			{
				break;
			}
			dwTemp1 += pData->pEventAttInfo->OadLen[i];
		}

		// û�ҵ�ָ��OAD
		if( i == pData->pEventAttInfo->NumofOad )
		{
			dwOad = pData->dwOad;
			OadIndex = (BYTE)(dwOad>>24);

			if( OadIndex != 0 )
			{
				dwTemp1 = 0;
				dwOad &= ~0xFF000000;
				// ������Ϊ0����һ��
				for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
				{
					if( pData->pEventAttInfo->Oad[i] == dwOad )
					{
						Temp = 2;
						break;
					}
					dwTemp1 += pData->pEventAttInfo->OadLen[i];
				}
			}
		}

		// �ҵ�OAD
		if( i != pData->pEventAttInfo->NumofOad )
		{
			dwOad = pData->dwOad;
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// ����
			}

			if( pData->pEventAttInfo->OadLen[i] > EVENT_ALONE_OADLEN )
			{
				return 0xFFFF;
			}

			w = pData->pEventAttInfo->OadLen[i];

			if( pData->pEventAddrLen->EventIndex <= ePRG_RECORD_NO )
			{
				Temp = (BYTE)((pData->dwOad&0x00E00000)>>21);
				
				if( (pData->Last==1) && (pData->status==1) && ((Temp==3)||(Temp==4)) )
				{
					#if( SEL_AHOUR_FUNC == YES )
					if( (pData->dwOad&0x000fffff) == AHOUR_OAD )//�ڼ䰲ʱֵ���⴦��
					{
						if( ((pData->pEventAddrLen->EventIndex-eEVENT_LOST_V_NO)<4) && ((pData->pDLT698RecordPara->Phase-ePHASE_A)<3) )
						{
							// �õ���ǰ��ʱֵ
							api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, sizeof(Buf), Buf );
							if( OadIndex != 0 )//���Ԫ��������Ϊ������ݽ���һ���Դ�����֤����������ȷ
							{
								for( Temp = 0; Temp < 3; Temp++ )
								{
									memmove( (BYTE*)&Buf[4+Temp*sizeof(DWORD)], Buf, 4 );
								}
							}
							api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.AHour[pData->pEventAddrLen->EventIndex-eEVENT_LOST_V_NO][pData->pDLT698RecordPara->Phase-ePHASE_A][0] ),
														sizeof(DWORD)*4, Buf1 );

							for( Temp=0; Temp<(pData->pEventAttInfo->OadLen[i]/sizeof(DWORD)); Temp++ )
							{							
	                            memcpy( (BYTE *)&dwTemp3, Buf+Temp*sizeof(DWORD), sizeof(DWORD) );//��ǰ��ʱֵ
								memcpy( (BYTE *)&dwTemp4, Buf1+Temp*sizeof(DWORD), sizeof(DWORD) );//��ǰ��ʱֵ
								
								if( dwTemp3 >= dwTemp4 )
								{
									dwTemp3 = dwTemp3-dwTemp4;	
								}
								else
								{
									dwTemp3 = 0;
								}

								memcpy( tmpBuf+Temp*sizeof(DWORD), (BYTE *)&dwTemp3, sizeof(DWORD) );//���㰲ʱֵ
							}

						}
						else//��֧���ڼ䶳�������ģ�����null
						{
							pData->pBuf[pData->off] = 0;//null
							return 1;
						}
					}
					else
					#endif
					{
						pData->pBuf[pData->off] = 0;//null
						return 1;
					}

				}
				else
				{
					dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
					dwTemp += dwTemp1;
					api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], tmpBuf);
				}
			}
			else
			{
				dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
				dwTemp += dwTemp1;
				api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], tmpBuf);
			}

			if( OadIndex != 0 )
			{
				pTemp = tmpBuf;
				dwTemp = dwOad;

				for( i = 1; i < OadIndex; i++ ) // pData->OadIndex ��1��ʼ
				{
					dwTemp2 = i;
					dwOad = (dwTemp&0X00FFFFFF);//��ȥ��Ԫ������
					dwOad |= (dwTemp2<<24);//��Ԫ������
					wLen1 = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );//�Զ������ݽ���ƫ��ʹ��eGetRecordData
					if( wLen1 == 0x8000 )
					{
						return 0x8000;
					}
					pTemp += wLen1;
				}

				dwOad = ((dwOad&0x00ffffff) | (OadIndex<<24));//��Ԫ������
			}
			else
			{
				pTemp = tmpBuf;
			}

			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, pTemp, 0xFF, pData->pBuf+pData->off );
			}
			else
			{
				//����ʹ�ö��᳤�ȴ��룬���ܽ���ת���� �ὫwLenת��Ϊʵ�ʳ���
				wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );
				//ǰ���жϳ����� ����ֱ�����
				Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, pTemp,pData->Len-pData->off, pData->pBuf+pData->off );
				if( (Result == 0x8000) || (Result == 0) )
				{
					return Result;
				}
			}

		}
		else
		{
			wLen = 1;
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// ����
			}
			pData->pBuf[pData->off] = 0;
		}
	}

	return wLen;
}

//-----------------------------------------------
//��������: ����OAD������
//
//����: 
//		pDLT698RecordPara[in]��		ָ��ṹ��TDLT698RecordPara��ָ��              
//		pEventOADCommonData[in]��	ָ��ṹ��TEventOADCommonData��ָ��                
//		pEventAttInfo[in]��			ָ��ṹ��TEventAttInfo��ָ��                
//		pEventAddrLen[in]��			ָ��ṹ��TEventAddrLen��ָ��                
//		dwPointer[in]��				��ַƫ��ָ��                
//		pBuf[out]��					�������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0                
//		dwOad[in]��					OAD                
//		Len[in]��					�����buf���� �����ж�buf�����Ƿ���                
//		Tag[in]��					0 ����Tag  1 ��Tag                
//		off[in]��					�Ѿ��е�ƫ��                
//		status[in]��				�¼�������ʶ                
//		AllorSep[in]��				1 ָ��OAD 0 ����OAD
//����ֵ:							bit15λ ��1 ���� ��0��ȷ�� 
//									bit0~bit14 �����ص����ݳ���             
//��ע: 
//-----------------------------------------------
static WORD ReadEventRecordSub( TEventData *pData )
{
	DWORD dwOad,dwTemp,dwTemp1,dwTemp2;
	WORD wLen,Result;
	BYTE i,j,Number,Temp,Buf[30],Buf1[30],TmpBuf[EVENT_VALID_OADLEN+30];
	BYTE *pTemp;
	
	// ��OI��Ӧ�̶��е�OAD
	for( i = 0; i < (sizeof(EventOadTab)/sizeof(TEventOadTab)); i++ )
	{
		if( pData->pDLT698RecordPara->OI == EventOadTab[i].OI )
		{
			break;
		}
	}

	if( i < (sizeof(EventOadTab)/sizeof(TEventOadTab)) )
	{
		pTemp = (BYTE *)&EventOadTab[i].OadList[0];	// �ҵ�
		Number = EventOadTab[i].Number;
	}
	else
	{
		// Ĭ��Class id 7 �������¼�����������
		pTemp = (BYTE *)&EventOadTab[0].OadList[0];	
		Number = EventOadTab[0].Number;
	}

	pData->off = 0;

	if( pData->pDLT698RecordPara->OADNum != 0 )
	{
		pData->AllorSep = 1;
		// ָ��OAD
		for( i = 0; i < pData->pDLT698RecordPara->OADNum; i++ )
		{		    
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
			pData->dwOad = dwOad;

			//�жϴ�OAD�Ƿ�Ϊ�̶���OAD
			for( j = 0; j < sizeof(FixedColumnOadTab)/sizeof(DWORD); j++ )
			{
				if( FixedColumnOadTab[j] == dwOad )
				{
					break;
				}
			}

			if( j < sizeof(FixedColumnOadTab)/sizeof(DWORD) )
			{
				//�жϴ�OAD�Ƿ�Ϊ���¼���¼֧�ֵĹ̶���OAD
				for( j = 0; j < Number; j++ )
				{
					memcpy( (BYTE *)&dwTemp, pTemp+sizeof(DWORD)*j, sizeof(DWORD) );
					if( dwTemp == dwOad )
					{
						break;
					}
				}

				if( j < Number )
				{
					// �Ǵ�OI��Ӧ�Ĺ̶���
					wLen = GetDataByOad( pData );

					if( wLen == 0xFFFF )
					{
						return 0; 	// ������
					}
				}
				else
				{
					wLen = 1;
					if( (pData->off + wLen) > pData->Len )
					{
						return 0; 	// ������	
					}

					*(pData->pBuf+pData->off) = 0;// NULL ��֧�ֵĹ̶���
				}
			}
			else
			{
				// ��OAD�б�
				wLen = GetDataByOad( pData );

				if( wLen == 0xFFFF )
				{
					return 0; 	// ������
				}
			}
			pData->off += wLen;
		}
	}
	else
	{
		// �̶���
		for( i = 0; i < Number; i++ )
		{
			memcpy( (BYTE *)&dwOad, pTemp+sizeof(DWORD)*i, sizeof(DWORD) );
			pData->dwOad = dwOad;
			
			wLen = GetDataByOad( pData );

			if( wLen == 0xFFFF )
			{
				return 0; 	// ������
			}

			pData->off += wLen;
		}

		// һ�ν�����ȫ�������������������ݵ�ַ�Ѿ�����������������
		dwTemp = pData->pEventAddrLen->dwRecordAddr + sizeof(TEventOADCommonData);
		dwTemp += pData->pEventSectorInfo->dwSectorAddr;
		
		// �������������б�
		if( pData->pEventAttInfo->NumofOad == 0 )
		{
			return pData->off;
		}

		if( (pData->pEventAttInfo->AllDataLen > EVENT_VALID_OADLEN) || (pData->pEventAttInfo->OadValidDataLen > EVENT_VALID_OADLEN) )
		{
			return 0x8000;
		}

		memset( TmpBuf, 0x00, pData->pEventAttInfo->OadValidDataLen );//�������治���ٵ���GetDataByOad��ԭ���ǿ��ܻᵼ��TmpBuf���ݱ���
		api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadValidDataLen, TmpBuf);
 
		dwTemp = 0;
		for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
		{		    
			dwOad = pData->pEventAttInfo->Oad[i];
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );// ��Ȼδ����ʱ��nullֻ��Ҫһ���ֽڣ����ﲻ�޸Ĵ˺�����Ψһ��Ӱ������ǰ��֡
			if( (pData->off+wLen) > pData->Len )
			{
				return 0;// ����
			}
			
			if( pData->pEventAddrLen->EventIndex <= ePRG_RECORD_NO )
			{
				Temp = (BYTE)((pData->pEventAttInfo->Oad[i]&0x00E00000)>>21);
				// ��1�� && �¼���δ���� && ����ʱ��OAD   ����0
				if( (pData->Last==1) && (pData->status==1) && ((Temp==3)||(Temp==4)) )
				{
					#if( SEL_AHOUR_FUNC == YES )
					if( (dwOad&0x000fffff) == AHOUR_OAD )//�ڼ䰲ʱֵ���⴦��
					{
						if( ((pData->pEventAddrLen->EventIndex-eEVENT_LOST_V_NO)<4) && ((pData->pDLT698RecordPara->Phase-ePHASE_A)<3) )
						{
							// �õ���ǰ��ʱֵ
							api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOad, NULL, sizeof(Buf), Buf );
							api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.AHour[pData->pEventAddrLen->EventIndex-eEVENT_LOST_V_NO][pData->pDLT698RecordPara->Phase-ePHASE_A][0] ),
														sizeof(DWORD)*4, Buf1 );

							for( Temp=0; Temp<(pData->pEventAttInfo->OadLen[i]/sizeof(DWORD)); Temp++ )
							{							
	                            memcpy( (BYTE *)&dwTemp1, Buf+Temp*sizeof(DWORD), sizeof(DWORD) );//��ǰ��ʱֵ
								memcpy( (BYTE *)&dwTemp2, Buf1+Temp*sizeof(DWORD), sizeof(DWORD) );//��ǰ��ʱֵ
								
								if( dwTemp1 >= dwTemp2 )
								{
									dwTemp1 = dwTemp1-dwTemp2;	
								}
								else
								{
									dwTemp1 = 0;
								}
								//����flash�ж�ȡ�������滻��
								memcpy( TmpBuf+(dwTemp+Temp*sizeof(DWORD)), (BYTE *)&dwTemp1, sizeof(DWORD) );//���㰲ʱֵ
							}

						}
						else//��֧���ڼ䶳�������ģ�����null
						{
							// �¼���û�н���,����������null
							dwTemp+= pData->pEventAttInfo->OadLen[i];
							*(pData->pBuf+pData->off)=0;//NULL
							pData->off+=1;
							continue;
						}
					}
					else
					#endif
					{
						// �¼���û�н���,����������null
						dwTemp+= pData->pEventAttInfo->OadLen[i];
						*(pData->pBuf+pData->off)=0;//NULL
						pData->off+=1;
						continue;
					}
				}
			}

			if( pData->Tag == eTagData )
			{
				api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, TmpBuf+dwTemp, 0xFF, pData->pBuf+pData->off );
			}
			else
			{
				//����ʹ�ö��᳤�ȴ��룬���ܽ���ת���� �ὫwLenת��Ϊʵ�ʳ���
				wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );
				//ǰ���жϳ����� ����ֱ�����
				Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, pTemp,pData->Len -pData->off, pData->pBuf+pData->off );
				if( (Result == 0x8000) || (Result == 0) )
				{
					return Result;
				}
			}
			
			dwTemp += pData->pEventAttInfo->OadLen[i];// EEP����ƫ��
			pData->off += wLen;
		}
	}

	return pData->off;
}

//-----------------------------------------------
//��������: ��ʱ�䷽ʽ���������¼���¼
//
//����: 
//		Tag[in]:				0 ����Tag 1 ��Tag                
//		pDLT698RecordPara[in]: 	��ȡ����Ĳ����ṹ��                
//  	Len[in]��				�����buf���� �����ж�buf�����Ƿ���                
//  	pBuf[out]: 				�������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0
//����ֵ:						bit15λ��1 �������޴����� ��0������������ȷ���أ� 
//								bit0~bit14 �����صĶ������ݳ���
//��ע: ���Ѵ��ʱ����������ֻ����ʱ����ȵļ�¼
//-----------------------------------------------
static WORD ReadEventRecordByTime( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventOADCommonData EventOADCommonDataBak;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	DWORD dwTemp,dwPointer,dwPointerBak;
	WORD twCurrentRecordNo;
	BYTE i,status;
	TRealTimer	Time;
	TRealTimer	EventTime,EventTimeBak;//�Ժ��Ż�Ϊָ�����zym
	TEventSectorInfo EventSectorInfo;
	
    dwPointerBak = 0;
    dwPointer = 0;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//���ݵ�ǰ��¼��Ϊ0���ҵ�ǰ״̬Ϊ�¼�û����
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}
	else if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
	{
		twCurrentRecordNo = EventAttInfo.MaxRecordNo;
	}
	else
	{
		twCurrentRecordNo = EventDataInfo.RecordNo;	
	}
	
	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	

	//�¼�Ҫ��ȡ��ʱ�굥λΪ�루���������
	api_ToAbsTime( pDLT698RecordPara->TimeOrLast/60, (TRealTimer *)&Time );
	Time.Sec = pDLT698RecordPara->TimeOrLast%60;
	// �����еļ�¼������
	for( i = 1; i <= twCurrentRecordNo; i++ )
	{
		dwPointer = EventDataInfo.RecordNo - i;

		//���������¼ƫ�Ƶ�ַ
		GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, dwPointer, &EventSectorInfo);
		
		dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;//��ַΪ����ַ��ƫ�Ƶ�ַ
		
		api_ReadMemRecordData(dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
		
		if( pDLT698RecordPara->eTimeOrLastFlag == eEVENT_START_TIME_FLAG )
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.BeginTime, sizeof(TRealTimer) );
		}
		else
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.EndTime, sizeof(TRealTimer) );
		}
		
		//��һ�β��ң����ж�Ҫ����ʱ�������һ�εĴ�С��ϵ���������һ�μ�¼��ʱ���ֱ�ӷ��ش���
		if( i == 1 )
		{
			if( memcmp( &Time, (BYTE *)&EventTime, sizeof(TRealTimer) ) > 0 )
			{
				return 0x8000;// û�ҵ���Ӧʱ��
			}
			//��һ�β��ң���ʼ��EventTimeBakΪEventTime
			memcpy( (BYTE *)&EventTimeBak, (BYTE *)&EventTime, sizeof(TRealTimer) );
		}
		
		if( memcmp( &Time, (BYTE *)&EventTime, sizeof(TRealTimer) ) == 0 )
		{
			//��������ʱ������ҵ�ʱ����ȣ�ֱ���жϲ�ѯ
			break;
		}
		else if( (memcmp( &Time, (BYTE *)&EventTime, sizeof(TRealTimer) ) > 0)
			&& (memcmp( &Time, (BYTE *)&EventTimeBak, sizeof(TRealTimer) ) < 0) )
		{
			//�����ҵ�ʱ����ڴ˴ζ�����ʱ�䣬С���ϴζ�����ʱ�䣬���жϲ�ѯ
			i -= 1;//i�϶���Ϊ1
			memcpy( (BYTE *)&EventOADCommonData, (BYTE *)&EventOADCommonDataBak, sizeof(TEventOADCommonData) );		
			dwPointer = dwPointerBak;
			break;
		}

		memcpy( (BYTE *)&EventTimeBak, (BYTE *)&EventTime, sizeof(TRealTimer) );
		memcpy( (BYTE *)&EventOADCommonDataBak, (BYTE *)&EventOADCommonData, sizeof(TEventOADCommonData) );	
		dwPointerBak = 	dwPointer;
	}

	if( i > twCurrentRecordNo )
	{
		//���������Ҫ����ʱ��С�����һ�ε�ʱ�䣬�򷵻ش˴ε�����
		if( memcmp( &Time, (BYTE *)&EventTime, sizeof(TRealTimer) ) < 0 )
		{
			i = twCurrentRecordNo;
		}
		else
		{
			return 0x8000;// û�ҵ���Ӧʱ��
		}
	}
	
	//���������¼ƫ�Ƶ�ַ
	GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, dwPointer, &EventSectorInfo);

	Data.Last = i;
	Data.pEventOADCommonData = &EventOADCommonData;

	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		i = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventStatus( eGET_EVENT_STATUS, i );	
	}
	else if( EventAddrLen.EventIndex == ePRG_RECORD_NO )
	{
		status = GetPrgRecord();
	}
	
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pEventAttInfo = &EventAttInfo;
	Data.pEventAddrLen = &EventAddrLen;
	Data.pEventSectorInfo = &EventSectorInfo;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.Tag = Tag;
	Data.status = status;
	Data.AllorSep = 0;

	return ReadEventRecordSub(&Data);
}

static WORD ReadEventRecordByTimeInterval( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	TEventSectorInfo	EventSectorInfo;
	DWORD dwTemp,dwPointer,SearchStartTime,SearchEndTime,EventTimeSec;
	WORD twCurrentRecordNo;
	BYTE i, status, Offset;
	TRealTimer	SaveStartTime,SaveEndTime;
	TRealTimer	EventTime;//�Ժ��Ż�Ϊָ�����zym
	WORD wLen = 0;	//��һ���¼���¼�õ������ݳ��ȣ�����ȡ���¼�δ�������򳤶ȶ�
	WORD ReturnLen = 0;	//���������ܳ���
	
	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//���ݵ�ǰ��¼��Ϊ0���ҵ�ǰ״̬Ϊ�¼�û����
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}
	else if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
	{
		twCurrentRecordNo = EventAttInfo.MaxRecordNo;
	}
	else
	{
		twCurrentRecordNo = EventDataInfo.RecordNo;	
	}
	
	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	
	
	//Ҫ���ҵ���ʼʱ����ڵ��ڲ��ҵĽ�ֹʱ�䣬���ش���
	if( pDLT698RecordPara->TimeIntervalPara.StartTime >= pDLT698RecordPara->TimeIntervalPara.EndTime )
	{
		return 0x8000;
	}
	
	//�¼�Ҫ��ȡ��ʱ�굥λΪ�루���������	
	memcpy( (BYTE*)&SearchStartTime, (BYTE*)&pDLT698RecordPara->TimeIntervalPara.StartTime, sizeof(DWORD) );

	memcpy( (BYTE*)&SearchEndTime, (BYTE*)&pDLT698RecordPara->TimeIntervalPara.EndTime, sizeof(DWORD) );
	
    Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum = 0;

	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		
        status = api_DealEventStatus( eGET_EVENT_STATUS, GetSubEventIndex((TEventAddrLen*)&EventAddrLen) );	
    }
	else if( EventAddrLen.EventIndex==ePRG_RECORD_NO )
	{
		status = GetPrgRecord();
	}
	
	// �����еļ�¼������
    for( i = twCurrentRecordNo; i >= 1; i-- )
	{
		//������ʱ����������һ���¼�δ����������������ѭ��
		if( (i == 1) && status && (pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_END_TIME_FLAG)) )
		{
			continue;
		}

		//���������¼ƫ�Ƶ�ַ
		GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, (EventDataInfo.RecordNo-i), &EventSectorInfo);
	
		dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
		api_ReadMemRecordData(dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
		
        memset( (BYTE*)&EventTime, 0x00, sizeof(EventTime) );
        
		if( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_START_TIME_FLAG) )
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.BeginTime, sizeof(TRealTimer) );			
		}
		else
		{
			memcpy( (BYTE*)&EventTime, (BYTE*)&EventOADCommonData.EventTime.EndTime, sizeof(TRealTimer) );
		}
		EventTimeSec = api_CalcInTimeRelativeSec((TRealTimer *)&EventTime);//����ʱ����������
		//�ж�Ҫ����ʱ�����䣨����ҿ��������һ�εĴ�С��ϵ��������ʱ�����䣬��ֱ�ӷ��ش���		
	    if(( EventTimeSec < SearchStartTime) || (EventTimeSec >= SearchEndTime) )
		{
			continue;// û�ҵ���Ӧʱ��
		}

		Data.pEventAttInfo = &EventAttInfo;
		Data.pEventAddrLen = &EventAddrLen;
		Data.pEventSectorInfo = &EventSectorInfo;
		Data.Tag = Tag;
		Data.AllorSep = 0;	
		Data.Last = i;
		Data.pEventOADCommonData = &EventOADCommonData;

		if( i == 1)	//���һ���¼���Ҫ�ж��¼��Ƿ����
		{
			Data.status = status;
		}
		else
		{
			Data.status = 0;	//�����һ���¼�Ĭ���ѽ���
		}
			
		wLen = ReadEventRecordSub(&Data);
		if( wLen == 0x8000 )
	    {
            return 0x8000;
	    }

	    if( wLen == 0 )
	    {
            return ReturnLen;
	    }
	    
	    if( (ReturnLen+wLen) > Len )
	    {
            return ReturnLen;
	    }
	    ReturnLen += wLen;
	    
		Data.pBuf += wLen;

		if( Data.Len < wLen )
		{
            return ReturnLen;
		}
		Data.Len -= wLen;
		
		Data.pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum++;
		
	}

	if( ReturnLen == 0 )
	{
		return 0x8000;// û�ҵ���Ӧʱ��
	}
	
	return ReturnLen;
}

//-----------------------------------------------
//��������: ���¼���¼
//
//����: 
//		Tag[in]:				0 ����Tag 1 ��Tag                
//		pDLT698RecordPara[in]: 	��ȡ����Ĳ����ṹ��                
//  	Len[in]��				�����buf���� �����ж�buf�����Ƿ���                
//  	pBuf[out]: 				�������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0	
//����ֵ:			bit15λ��1 �������޴����� ��0������������ȷ���أ� 
//					bit0~bit14 �����صĶ������ݳ���
//��ע: ֻ֧����Last����¼
//-----------------------------------------------
WORD ReadEventRecordByNo( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	TEventSectorInfo	EventSectorInfo;
	DWORD dwTemp;
	WORD twCurrentRecordNo;
	BYTE i,status;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//���ݵ�ǰ��¼��Ϊ0���ҵ�ǰ״̬Ϊ�¼�û����
	if( EventDataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}
	else if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
	{
		twCurrentRecordNo = EventAttInfo.MaxRecordNo;
	}
	else
	{
		twCurrentRecordNo = EventDataInfo.RecordNo;		
	}
	
	//���ͨ�������¼��б��е�ָ���¼�
	ClrReportIndexChannel( pDLT698RecordPara->Ch, EventAddrLen.SubEventIndex );	
	
	if( (pDLT698RecordPara->TimeOrLast > twCurrentRecordNo) || (pDLT698RecordPara->TimeOrLast==0) )// ���������ݵ����, ��0�η���������
	{
		return 0x8000;//������
	}
	
	EventDataInfo.RecordNo -= pDLT698RecordPara->TimeOrLast;	
	//���������¼ƫ�Ƶ�ַ
	GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, EventDataInfo.RecordNo, &EventSectorInfo);
	
	dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
	
	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		// �������¼�
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		i = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventStatus( eGET_EVENT_STATUS, i );	
	}
	else if( EventAddrLen.EventIndex == ePRG_RECORD_NO )
	{
		//�����
		status = GetPrgRecord();
	}
	
	api_ReadMemRecordData( dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
	
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pEventOADCommonData = &EventOADCommonData;
	Data.pEventAttInfo = &EventAttInfo;
	Data.pEventAddrLen = &EventAddrLen;
	Data.pEventSectorInfo = &EventSectorInfo;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.Tag = Tag;
	Data.status = status;
	Data.AllorSep = 0;
	Data.Last = (BYTE)pDLT698RecordPara->TimeOrLast;

	return ReadEventRecordSub(&Data);
}

//-----------------------------------------------
//��������: ���¼���¼
//
//����: 
//	Tag[in]:				0 ����Tag 1 ��Tag                
//	pDLT698RecordPara[in]: 	��ȡ����Ĳ����ṹ��                
//  Len[in]��				�����buf���� �����ж�buf�����Ƿ���                
//  pBuf[out]: 				�������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0	
//����ֵ:			bit15λ��1 �������޴����� ��0������������ȷ���أ� 
//					bit0~bit14 �����صĶ������ݳ���
//��ע:
//-----------------------------------------------
WORD api_ReadEventRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	BYTE tEventIndex;

	if( GetEventIndex( pDLT698RecordPara->OI, &tEventIndex ) == FALSE )
	{
		return 0x8000;
	}

	if( pDLT698RecordPara->eTimeOrLastFlag == eNUM_FLAG )
	{
		// �������࣬����N��
		return ReadEventRecordByNo( Tag, pDLT698RecordPara, Len, pBuf );		
	}
	else if( 	(pDLT698RecordPara->eTimeOrLastFlag == eEVENT_START_TIME_FLAG) 
			|| 	(pDLT698RecordPara->eTimeOrLastFlag == eEVENT_END_TIME_FLAG) )
	{
		// �������࣬�Կ�ʼʱ�䣬����ʱ���
		return ReadEventRecordByTime( Tag, pDLT698RecordPara, Len, pBuf );		
	}
	else if( 	( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_START_TIME_FLAG) )
			||	( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eEVENT_END_TIME_FLAG) ) )
	{
		return ReadEventRecordByTimeInterval( Tag, pDLT698RecordPara, Len, pBuf );	
	}

	return 0x8000;
}

//-----------------------------------------------
//��������: ��ȡ��ǰ��¼��������¼��
//
//����:  
//  	OI[in]: 	OI   
//  	Phase[in]: 	��λ0��,1A,2B,3C            
//  	Type[in]: 	�ο�eEVENT_RECORD_NO_TYPE                
//  	pBuf[out]��	�������ݵĻ���                    
//����ֵ:	TRUE/FALSE
//
//��ע: 
//-----------------------------------------------
BOOL api_ReadEventRecordNo( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf )
{
	TEventAttInfo	EventAttInfo;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	WORD			twCurrentRecordNo;
	BYTE			tEventIndex;

	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}

	EventAddrLen.EventIndex = tEventIndex;
	EventAddrLen.Phase = Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	if( Type == eEVENT_CURRENT_RECORD_NO )
	{
		// ��ǰ��¼��
		api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
		if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
		{
			twCurrentRecordNo = EventAttInfo.MaxRecordNo;
		}
		else
		{
			twCurrentRecordNo = EventDataInfo.RecordNo;
		}
		memcpy( pBuf, (BYTE *)&twCurrentRecordNo, sizeof(WORD) );		
	}
	else if( Type == eEVENT_MAX_RECORD_NO )
	{
		// ����¼��
		api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
		memcpy( pBuf, (BYTE *)&EventAttInfo.MaxRecordNo, sizeof(WORD) );					
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

#if ( SEL_EVENT_LOST_V == YES )
//-----------------------------------------------
//��������: ��ȡʧѹ����13
//
//����:  
//  Index[in]:	0 ���� 
// 				1 �¼������ܴ��� 
// 				2 �¼����ۼ�ʱ��    
// 				3 ���һ��ʧѹ����ʱ��
// 				4 ���һ��ʧѹ����ʱ��   
//  pBuf[out]��	�������ݵĻ���
//                    
//����ֵ:		TRUE/FALSE
//
//��ע:����2013���׼��������У�
//��3�������1��ʧѹ����ʱ�̡���A��B��C ��������������Ǵ�ʧѹ����ʱ�̡�
//��4�������1��ʧѹ����ʱ�̡���A��B��C ��������������Ǵ�ʧѹ����ʱ�̡�
//-----------------------------------------------
BOOL api_ReadEventLostVAtt( BYTE Index, BYTE *pBuf )
{
	TEventOADCommonData	EventOADCommonData;
	TEventDataInfo		EventDataInfo;
	TRealTimer			Time[2];// 0,1���һ�η�������ʱ��
	DWORD				dwAddr,dwNum,dwTime,dwPointer,dwTemp;
	DWORD				dwStartTime;
	BYTE				i,off,status;
	TDLT698RecordPara 	DLT698RecordPara;
	DWORD 				dwOAD[2];
	WORD 				wLen;
	if( Index > 4 )
	{
		return FALSE;
	}

	dwNum = 0;
	dwTime = 0;
	dwStartTime = 0;
	status = 0;
	
	memset( (BYTE*)&Time[0], 0x00, sizeof(TRealTimer) );
	memset( (BYTE*)&Time[1], 0x00, sizeof(TRealTimer) );
	
	for( i = 0; i < MAX_PHASE; i++ )
	{
		dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.EventDataInfo[eSUB_EVENT_LOSTV_A+i] );
		api_VReadSafeMem( dwAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo );

		if( EventDataInfo.RecordNo != 0 )
		{
			dwOAD[0] = 0x00021E20;//201E	8	�¼�����ʱ��
			dwOAD[1] = 0x00022020;//2020	8	�¼�����ʱ��
			
			DLT698RecordPara.OI = 0x3000;                   //OI����
			DLT698RecordPara.Phase = (ePHASE_TYPE)(i+1);

			DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOADָ��
			DLT698RecordPara.OADNum = 2;     	//OAD����
			DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //ѡ�����
			DLT698RecordPara.TimeOrLast = 1;           //����ʱ���� ��1��

			memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) ); 
			
			//���ú��� ��ȡ��¼ �������� ����TAG
			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, sizeof(TEventTime), (BYTE *)&EventOADCommonData.EventTime );
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				memset( (BYTE*)&EventOADCommonData.EventTime, 0x00, sizeof(TEventTime) );
			}
			
			if( lib_IsAllAssignNum( (BYTE *)&EventOADCommonData.EventTime.BeginTime, 0xff, sizeof(TRealTimer) ) == TRUE )
			{
				memset( (BYTE*)&EventOADCommonData.EventTime.BeginTime, 0x00, sizeof(TRealTimer) );
			}
			
			if( lib_IsAllAssignNum( (BYTE *)&EventOADCommonData.EventTime.EndTime, 0xff, sizeof(TRealTimer) ) == TRUE )
			{
				memset( (BYTE*)&EventOADCommonData.EventTime.EndTime, 0x00, sizeof(TRealTimer) );
			}
			
			status = api_DealEventStatus( eGET_EVENT_STATUS, (eSUB_EVENT_LOSTV_A+i) );
			// ��δ����
			if( status == 1 )
			{
				// �����ۼ�ʱ��
				dwPointer = api_CalcInTimeRelativeSec( &RealTimer );							// ��ǰʱ���Ӧ�������
				dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );	// ����ʱ�̶�Ӧ�������
				
				if( (dwPointer!=0xffffffff) && (dwTemp!=0xffffffff) )
				{
					if( dwPointer >= dwTemp )
					{
						dwTime += (dwPointer-dwTemp);
					}
				}
			}
			
			dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );
			if( dwTemp > dwStartTime )
			{
				dwStartTime = dwTemp;
				memcpy(&Time[0], (BYTE*)&EventOADCommonData.EventTime.BeginTime, sizeof(TRealTimer) );
				if( status == 1 )
				{
					memset(&Time[1],0x00,sizeof(TRealTimer));
				}
				else
				{	
					memcpy(&Time[1], (BYTE*)&EventOADCommonData.EventTime.EndTime, sizeof(TRealTimer) );
				}
			}
		}

		dwNum += EventDataInfo.RecordNo;	// �ܴ���
		dwTime += EventDataInfo.AccTime;	// ���ۼ�ʱ��		
	}
		
	off = 0;
	
	if( Index == 0 )
	{
		memcpy( &pBuf[off], (BYTE *)&dwNum, sizeof(DWORD) );
		off += sizeof(DWORD);
		memcpy( &pBuf[off], (BYTE *)&dwTime, sizeof(DWORD) );
		off += sizeof(DWORD);
		memcpy( &pBuf[off], (BYTE *)&Time[0], sizeof(TRealTimer) );			
		off += sizeof(TRealTimer);
		memcpy( &pBuf[off], (BYTE *)&Time[1], sizeof(TRealTimer) );					
		off += sizeof(TRealTimer);
	}
	else if( Index == 1 )
	{
		memcpy( &pBuf[off], (BYTE *)&dwNum, sizeof(DWORD) );
		off += sizeof(DWORD);
	}
	else if( Index == 2 )
	{
		memcpy( &pBuf[off], (BYTE *)&dwTime, sizeof(DWORD) );
		off += sizeof(DWORD);
	}
	else if( Index == 3 )
	{
		memcpy( &pBuf[off], (BYTE *)&Time[0], sizeof(TRealTimer) );			
		off += sizeof(TRealTimer);
	}
	else
	{
		memcpy( &pBuf[off], (BYTE *)&Time[1], sizeof(TRealTimer) );					
		off += sizeof(TRealTimer);
	}
	
	return TRUE;
}
#endif

#if( SEL_PRG_INFO_Q_COMBO == YES )
//-----------------------------------------------
//��������: ��ȡ��ǰֵ��¼��(�޹���Ϸ�ʽ1�����֡��޹���Ϸ�ʽ2�����ֱ�̴���)
//
//����:  
//  byType[in]: 	0--�޹���Ϸ�ʽ1������, 1--�޹���Ϸ�ʽ2������
//  pBuf[out]��	�������ݵĻ���
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע: �¼��������7  �¼�����Դ+��������+�ۼ�ʱ��
//-----------------------------------------------
BOOL api_ReadCurrentRecordTable_Q_Combo( BYTE byType,BYTE *pBuf )
{	
	DWORD dwTemp, dwTemp2;
	
	dwTemp = 0;
	if( api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.dwPrgQComBo2Num ),sizeof(DWORD), (BYTE *)&dwTemp ) == FALSE )
	{
		return FALSE;
	}	
	if( byType == 0 )
	{
		if( api_ReadCurrentRecordTable( 0x3023, 0, 1, pBuf ) == 0 )
		{
			return FALSE;
		}	
		memcpy( (void*)&dwTemp2, pBuf+1, sizeof(DWORD) );
		if( dwTemp2 >= dwTemp )
		{
			dwTemp = dwTemp2 - dwTemp;
		}
		else
		{
			dwTemp = 0;
		}	
	}
	
	memset( (void *)pBuf, 0x00, 9 );
	pBuf[0] = byType;
	memcpy( (void *)(pBuf+1), (void *)&dwTemp, 4 );	
	return TRUE;
}
#endif//#if( SEL_PRG_INFO_Q_COMBO == YES )
//-----------------------------------------------
//��������: ��ȡ��ǰֵ��¼��
//
//����:  
//  OI[in]: 	OI      
//  Phase[in]: 	��λ0��,1A,2B,3C 
//	Type[in]:	0--�����1�η���ʱ�䡢����ʱ��(Class 7�з���Դ)  ����--����������+�ۼ�ʱ��(Class 7�з���Դ)
//  pBuf[out]��	�������ݵĻ���
//                    
//����ֵ:	0--��ȡʧ�ܣ�����--���ض�ȡ����
//
//��ע: ���¼���Class 7���1�η���ʱ�䡢����ʱ��ʱ���޹����ģʽ�ֱ�̼�¼���ڴ˴���������Դ�̶�ΪNULL���������£�
//			00 00 00--����ԴΪNULL������ʱ��ΪNULL������ʱ��ΪNULL
//			00 YY YY MM DD hh mm ss 00--����ԴΪNULL������ʱ����ֵ������ʱ��ΪNULL 
//			00 YY YY MM DD hh mm ss YY YY MM DD hh mm ss--����ԴΪNULL������ʱ����ֵ������ʱ����ֵ
//		���¼���Class 24���1�η���ʱ�䡢����ʱ��ʱ����������:
//			00 00--����ʱ��ΪNULL������ʱ��ΪNULL
//			YY YY MM DD hh mm ss 00--����ʱ����ֵ������ʱ��ΪNULL 
//			YY YY MM DD hh mm ss YY YY MM DD hh mm ss--����ʱ����ֵ������ʱ����ֵ
//		���¼���Class 7��������+�ۼ�ʱ��ʱ���������£�
//			00 00 00 00 00 00 00 00 00--����ԴΪNULL����������Ϊ4�ֽ�00���ۼ�ʱ��Ϊ4�ֽ�00
//			00 N0 N1 N2 N3 00 00 00 00--����ԴΪNULL����������Ϊ4�ֽ����ݣ��ۼ�ʱ��Ϊ4�ֽ�00
//			00 N0 N1 N2 N3 T0 T1 T2 T3--����ԴΪNULL����������Ϊ4�ֽ����ݣ��ۼ�ʱ��Ϊ4�ֽ�����
//		���¼���Class 24��������+�ۼ�ʱ��ʱ���������£�
//			00 00 00 00 00 00 00 00--��������Ϊ4�ֽ�00���ۼ�ʱ��Ϊ4�ֽ�00
//			N0 N1 N2 N3 00 00 00 00--��������Ϊ4�ֽ����ݣ��ۼ�ʱ��Ϊ4�ֽ�00
//			N0 N1 N2 N3 T0 T1 T2 T3--��������Ϊ4�ֽ����ݣ��ۼ�ʱ��Ϊ4�ֽ�����
//-----------------------------------------------
WORD api_ReadCurrentRecordTable( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf )
{
	WORD wLen;
	TDLT698RecordPara DLT698RecordPara;      
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo	EventAttInfo;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOAD[3];
	DWORD	dwTemp,dwTemp1;
	//WORD	twSavePointer;
	BYTE	i,j,status;
	BYTE	*pTemp;
	
	if( GetEventIndex( OI, &i ) == FALSE )//�ж��¼�OI�Ƿ�֧�֣���֧�ַ���false
	{
		return 0;
	}
	
	// ֻ֧�ַ���ABC�����ܣ����㷵��TRUE
	if( Phase == ePHASE_ALL )
	{
		if( EventInfoTab[i].NumofPhase == 3 )//�����Ӧ�¼���֧���� ����NULL
		{
			pBuf[0] = 0;
			return 1;
		}
	}

	EventAddrLen.EventIndex = i;
	EventAddrLen.Phase = Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		
	pTemp = pBuf;

	// �¼�����Դ
	if( ((OI>=0x3000)&&(OI<=0x3008)) || (OI==0x300b) || (OI==0x303b) ) 	//class_id=24
	{
		// ��
		wLen = 0;
	} 
	else												//class_id=7
	{
		pTemp[0] = 0;// ����Դ(1),SEL_PRG_INFO_Q_COMBO �޹����ģʽ�ֵķ���Դ�� api_ReadCurrentRecordTable_Q_Combo�и�ֵ���˴����ø�ֵ
		wLen = 1;
	}
	pTemp += wLen;

	if( Type != 0 )//Type[in]:	0--�����1�η���ʱ�䡢����ʱ��(Class 7�з���Դ)  ����--����������+�ۼ�ʱ��(Class 7�з���Դ)
	{	
		// �¼���������
		memcpy( pTemp, (BYTE *)&EventDataInfo.RecordNo, sizeof(DWORD) );//��������
		pTemp += sizeof(DWORD);
	}
	
	dwOAD[0] = 0x00021E20;//201E	8	�¼�����ʱ�� 
	dwOAD[1] = 0x00022020;//2020	8	�¼�����ʱ��
	
	DLT698RecordPara.OI = OI;                   //OI����
	DLT698RecordPara.Phase = (ePHASE_TYPE)Phase;
	
	DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOADָ��
	DLT698RecordPara.OADNum = 2;     	//OAD����
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //ѡ�����
	DLT698RecordPara.TimeOrLast = 1;           //����ʱ���� ��1��

	// �ۼ�ʱ��
	if( (EventAddrLen.EventIndex<eNUM_OF_EVENT_PRG) && (EventAddrLen.EventIndex>=ePRG_RECORD_NO) )
	{
		if( Type == 0 )
		{
			//���ú��� ��ȡ��¼ �������� ����TAG
			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), pTemp );

			//���BeginTime��Ϊ0���򿽵�pTempҲ��00 00������00��ʾ����ʱ�䡢����ʱ�䶼ΪNULL
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				pTemp[0] = 0x00;
				pTemp[1] = 0x00;  
				return (pTemp+2-pBuf);
			}
			else//���BeginTime�в�Ϊ0���򿽵�pTempҲ7�ֽ�BeginTime 00��00��ʾ����ʱ��ΪNULL
			{				
				if(( EventAddrLen.EventIndex == ePRG_ADJUST_TIME_NO )//Уʱ�¼����ؼ�¼����ʱ��
				||( EventAddrLen.EventIndex == ePRG_BROADJUST_TIME_NO ))//�㲥Уʱ�¼����ؼ�¼����ʱ��
				{
					if( wLen < 14 )
					{
						pTemp[7] = 0x00;
						return (pTemp+8-pBuf);//���BeginTime�в�Ϊ0���򿽵�pTempҲ7�ֽ�BeginTime 00��00��ʾ����ʱ��ΪNULL
					}
					else
					{
						return (pTemp+14-pBuf);;
					}
				}
				pTemp[7] = 0x00;
				return (pTemp+8-pBuf);
			}		 
		}
		else
		{		
			memset( pTemp, 0x00, sizeof(DWORD) );//�����������ۼ�ʱ��
			return (pTemp+4-pBuf);
		}
	}
	else
	{
		j = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );	
		
		if( Type == 0 )
		{
			//���ú��� ��ȡ��¼ �������� ����TAG
			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), pTemp );

			//���BeginTime��Ϊ0���򿽵�pTempҲ��00 00������00��ʾ����ʱ�䡢����ʱ�䶼ΪNULL
			if( (wLen & 0x8000)||(wLen == 2) )
			{	
				pTemp[0] = 0x00;
				pTemp[1] = 0x00;  
				return (pTemp+2-pBuf);
			}
			else
			{
				if( api_DealEventStatus( eGET_EVENT_STATUS, j ) == 1 )//����¼����ڷ���
				{
	
					dwOAD[0] = 0x00022020;//2020	8	�¼�����ʱ��
					DLT698RecordPara.pOAD = (BYTE *)&dwOAD[0];  //pOADָ��
					DLT698RecordPara.OADNum = 1;     	//OAD����
					DLT698RecordPara.TimeOrLast = 2;           //����ʱ���� ��2��
					//���ú��� ��ȡ��¼ �������� ����TAG
					wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, 7, (pTemp+7) );	
					if( (wLen & 0x8000)||(wLen == 1) )
					{
						pTemp[7] = 0x00;
						return (pTemp+8-pBuf);//���BeginTime�в�Ϊ0���򿽵�pTempҲ7�ֽ�BeginTime 00��00��ʾ����ʱ��ΪNULL
					}
					else
					{
						return (pTemp+14-pBuf);
					}
				}
				else
				{
					return (pTemp+14-pBuf);
				}
			}		
		}
		else
		{
			status = api_DealEventStatus( eGET_EVENT_STATUS, j );		

			// ������δ����
			if( status == 1 )
			{			
				//���ú��� ��ȡ��¼ �������� ����TAG
    			wLen = api_ReadProRecordData( 0x00, &DLT698RecordPara, (7+7), (BYTE *)&EventOADCommonData.EventTime.BeginTime );
				
				if( (wLen & 0x8000) == 0x0000 )
				{	
					// �����ۼ�ʱ��
					dwTemp1 = api_CalcInTimeRelativeSec( &RealTimer );							// ��ǰʱ���Ӧ�������
					dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );	// ����ʱ�̶�Ӧ�������
					
					if( (dwTemp1!=0xffffffff) && (dwTemp!=0xffffffff) )
					{
						if( dwTemp1 >= dwTemp )
						{
							EventDataInfo.AccTime += (dwTemp1-dwTemp);
						}
						else
						{
							//EventDataInfo.AccTime = 0;
						}
					}
					else
					{
						//EventDataInfo.AccTime = 0; // ���󷵻�0
					}
				}
			}
	
			memcpy( pTemp, (BYTE *)&EventDataInfo.AccTime, sizeof(DWORD) );
			return (pTemp+4-pBuf);
		}
	}
}
#if( SEL_DLT645_2007 == YES )
BYTE SearchSignEventOI( WORD OI )
{
	BYTE i,Num;//����������ܳ���255

	Num = (sizeof(SignEventOI_Table)/sizeof(SignEventOI_Table[0]));
	if( Num >= 0x80 )		//������ѭ��
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == SignEventOI_Table[i] )
		{
			return 0X55;
		}
	}
	
	return 0x00;
}

//-----------------------------------------------
//��������: ��ȡ645��ǰֵ��¼��
//
//����:  
//  OI[in]: 	OI      
//  Phase[in]: 	��λ0��,1A,2B,3C 
//	Type[in]:	1 �¼������ܴ��� 
// 				2 �¼����ۼ�ʱ��    
// 				3 ���һ��ʧѹ����ʱ��(��10000101:���1��ʧѹ����ʱ��)
// 				4 ���һ��ʧѹ����ʱ��(��10000201:���1��ʧѹ����ʱ��)   
//  pBuf[out]��	�������ݵĻ���
//                    
//����ֵ:	0x8000--��ȡʧ�ܣ�����--���ض�ȡ����
//
//��ע: 
//-----------------------------------------------
WORD api_ReadCurrentRecordTable645( WORD OI, BYTE Phase, BYTE Type, BYTE *pBuf )
{
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventSectorInfo	EventSectorInfo;
	DWORD	dwTemp,dwTemp1;
	//WORD	twSavePointer;
	BYTE	i,j,status,Buf[32+20];

    if( (Type > 4) || (Type == 0) )//������Type == 0;
    {
        return 0x8000;
    }
    
	if( GetEventIndex( OI, &i ) == FALSE )//�ж��¼�OI�Ƿ�֧�֣���֧�ַ���false
	{
		return 0x8000;
	}
	
    
	if( (OI == 0x3000) && (Phase == ePHASE_ALL ) )//��ʧѹ�¼���������
	{
	    #if ( SEL_EVENT_LOST_V == YES )
        api_ReadEventLostVAtt( Type, Buf);
        if( (Type == 1) || (Type == 2) )//�����������ۼ�ʱ��
        {
            memcpy( (BYTE*)&dwTemp1, Buf, 4 ); 
            if( Type == 2 )//�ۼ�ʱ�佫��ת��Ϊ����
            {
                dwTemp1 = (dwTemp1/60);
            }

            dwTemp = lib_DWBinToBCD(dwTemp1);
            memcpy( pBuf, (BYTE *)&dwTemp, 3 );
            return 3;         
        }
        else//���һ�η���������ʱ��
        {
            api_TimeFormat698To645((TRealTimer * )Buf, pBuf );
            lib_InverseData( pBuf, 6 );
            return 6; 
        }
        #else
        return 0x8000;
        #endif
       
	}
	else
	{
	    // ֻ֧�ַ���ABC�����ܣ����㷵��TRUE
    	if( Phase == ePHASE_ALL )
    	{
    		if( EventInfoTab[i].NumofPhase == 3 )//�����Ӧ�¼���֧���� ����NULL
    		{
    			return 0x8000;
    		}
    	}
    	
    	EventAddrLen.EventIndex = i;
    	EventAddrLen.Phase = Phase;
    	if( GetEventInfo( &EventAddrLen ) == FALSE )
    	{
    		return 0;
    	}

    	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
    	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
    		
    	if( EventDataInfo.RecordNo == 0 )
    	{
			if( Type < 3 )//�������������ۼ�ʱ��    
    		{		
    		    memset( pBuf, 0x00, 3 );
    		    return 3;
    		}
    		else
    		{ 
    			memset( pBuf, 0x00, 6 );
    			return 6;
    		}
    	}
    	
		//���������¼ƫ�Ƶ�ַ
		GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, (EventDataInfo.RecordNo-1), &EventSectorInfo);
	
        
        if( Type == 1 )//�¼���������
        { 
            dwTemp = lib_DWBinToBCD(EventDataInfo.RecordNo);
            memcpy( pBuf, (BYTE *)&dwTemp, 3 );

            return 3;         
        }
        else if( Type == 2 )//�ۼ�ʱ��
        {
            // �ۼ�ʱ��
            if( (EventAddrLen.EventIndex<eNUM_OF_EVENT_PRG) && (EventAddrLen.EventIndex>=ePRG_RECORD_NO) )
            {
                memset( pBuf, 0xff, 3 );//�����������ۼ�ʱ��
            }
            else
            {
                j = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );  

                status = api_DealEventStatus( eGET_EVENT_STATUS, j );       

                // ������δ����
                if( status == 1 )
                {           
                    EventAddrLen.dwRecordAddr += EventSectorInfo.dwSectorAddr;
                    api_ReadMemRecordData( EventAddrLen.dwRecordAddr, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );

                    // �����ۼ�ʱ��
                    dwTemp1 = api_CalcInTimeRelativeSec( &RealTimer );                          // ��ǰʱ���Ӧ�������
                    dwTemp = api_CalcInTimeRelativeSec( &EventOADCommonData.EventTime.BeginTime );  // ����ʱ�̶�Ӧ�������
                    
                    if( (dwTemp1!=0xffffffff) && (dwTemp!=0xffffffff) )
                    {
                        if( dwTemp1 >= dwTemp )
                        {
                            EventDataInfo.AccTime += (dwTemp1-dwTemp);
                        }
                        else
                        {
                            EventDataInfo.AccTime = 0;
                        }
                    }
                    else
                    {
                        EventDataInfo.AccTime = 0; // ���󷵻�0
                    }
                }

                EventDataInfo.AccTime = (EventDataInfo.AccTime/60);//�ۼ�ʱ�佫��ת��Ϊ����
                dwTemp = lib_DWBinToBCD(EventDataInfo.AccTime);
                memcpy( pBuf, (BYTE *)&dwTemp, 3 );

                return 3;
            }
        }
     	else
    	{
            return 0x8000;
    	}
	}
	
    return 0x8000;
}

//-----------------------------------------------
//��������: ����OAD������
//
//����: 
//		pDLT698RecordPara[in]��ָ��ṹ��TDLT698RecordPara��ָ��              
//		pEventOADCommonData[in]��ָ��ṹ��TEventOADCommonData��ָ��                
//		pEventAttInfo[in]��ָ��ṹ��TEventAttInfo��ָ��                
//		pEventAddrLen[in]��ָ��ṹ��TEventAddrLen��ָ��                
//		dwPointer[in]����ַƫ��ָ��                
//		pBuf[out]���������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0                
//		dwOad[in]��OAD                
//		Len[in]�������buf���� �����ж�buf�����Ƿ���                
//		Tag[in]��0 ����Tag  1 ��Tag                
//		off[in]���Ѿ��е�ƫ��                
//		status[in]���¼�������ʶ                
//		AllorSep[in]��1 ָ��OAD 0 ����OAD
// 		Last[in]: ��N��
//����ֵ:	0xFFFF: ������ 0x8000���� ����ֵ���������ݳ���             
//��ע: 
//-----------------------------------------------
static WORD GetDataByOad645( BYTE NoDataFlag, TEventData *pData )
{
	DWORD dwTemp,dwTemp1,dwTemp2,dwOad,dwOad1;
	WORD wLen,wLen1,OI,w;
	BYTE Temp,i,OadIndex,SignFlag,Flag,tmpBuf[EVENT_ALONE_OADLEN+30];	
	BYTE *pTemp;
	BYTE Buf[20];
	BYTE RelayErrBuf[10];
	TFourByteUnion DWValue,DWValue1;
	TTwoByteUnion  WValue;
	
	Flag = 0;
	memset(Buf, 0x00, sizeof(Buf));
    
	if( pData->dwOad == STARTTIME_OAD )
	{
		wLen = 6;
		if( (pData->off+6) > pData->Len )
		{
			return 0xFFFF;// ����
		}

		//������ʱ�䶼Ϊff������Ϊnull
		if( (lib_IsAllAssignNum((BYTE *)&pData->pEventOADCommonData->EventTime.BeginTime, 0xff, sizeof(TRealTimer) ) == TRUE)
		    ||( NoDataFlag == 0x55 ) )
		{   
			memset( pData->pBuf+pData->off, 0x00, 6 );
		}
		else
		{
			api_TimeFormat698To645((TRealTimer * )&pData->pEventOADCommonData->EventTime.BeginTime, Buf );
			lib_ExchangeData( pData->pBuf+pData->off, Buf, 6);
		}				
	}
	else if( pData->dwOad == ENDTIME_OAD )
	{
		wLen = 6;
		if( (pData->off+6) > pData->Len )
		{
			return 0xFFFF;// ����
		}

		//������ʱ�䶼Ϊff������Ϊnull eNUM_OF_EVENT
		//Уʱ��¼����¼����ʱ��
		if( ((( lib_IsAllAssignNum((BYTE *)&pData->pEventOADCommonData->EventTime.EndTime, 0xff, sizeof(TRealTimer) ) == TRUE )
			||	( pData->pEventAddrLen->EventIndex>=ePRG_RECORD_NO )) &&( pData->pEventAddrLen->EventIndex!=ePRG_ADJUST_TIME_NO) )
			||( NoDataFlag == 0x55 ) )
		{
			memset( pData->pBuf+pData->off, 0x00, 6 );
		}
		else
		{
			api_TimeFormat698To645((TRealTimer * )&pData->pEventOADCommonData->EventTime.EndTime, Buf );
			lib_ExchangeData( pData->pBuf+pData->off, Buf, 6);
		}
	}
	else if( pData->dwOad == 0x55555555 )//�����ߴ���
	{
		wLen = 4;
		if( (pData->off+wLen) > pData->Len )
		{
			return 0xFFFF;// ����
		}
		if( NoDataFlag == 0x55 )
		{
			memset( pData->pBuf+pData->off, 0x00, 4 );
		}
		else
		{
			memset( pData->pBuf+pData->off, 0xff, 4 );
		}

		wLen = 4;
	}	
	else
	{
		dwTemp1 = 0;
		OadIndex = 0;
		dwOad = pData->dwOad;
		OadIndex = (BYTE)(dwOad>>24);
		lib_ExchangeData( (BYTE*)&OI, (BYTE*)&dwOad, 2);//ȡOI

		for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
		{
			if( pData->pEventAttInfo->Oad[i] == pData->dwOad )
			{
				break;
			}
			dwTemp1 += pData->pEventAttInfo->OadLen[i];
		}

		// û�ҵ�ָ��OAD
		if( i == pData->pEventAttInfo->NumofOad )
		{
			if( OadIndex != 0 )
			{
				dwTemp1 = 0;
				dwOad &= ~0xFF000000;
				// ������Ϊ0����һ��
				for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
				{
					if( pData->pEventAttInfo->Oad[i] == dwOad )
					{
						wLen = api_GetProOADLen( eGetRecordData, 0, (BYTE *)&pData->dwOad, 0 );
						if( wLen != 0x8000 )
						{
							Temp = 2;
							break;
						}
					}
					dwTemp1 += pData->pEventAttInfo->OadLen[i];
				}
			}
		}

		// �ҵ�OAD
		if( i != pData->pEventAttInfo->NumofOad )
		{
			dwOad = pData->dwOad;
			wLen = api_GetProOADLen( eGetNormalData, 0, (BYTE *)&dwOad, 0 );
			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// ����
			}

			if( pData->pEventAttInfo->OadLen[i] > EVENT_ALONE_OADLEN )
			{
				return 0xFFFF;
			}

			w = pData->pEventAttInfo->OadLen[i];

			if( pData->pEventAddrLen->EventIndex <= ePRG_RECORD_NO )
			{
				Temp = (BYTE)((pData->dwOad&0x00E00000)>>21);
				
				if( (pData->Last==1) && (pData->status==1) && ((Temp==3)||(Temp==4)) )
				{
					memset( tmpBuf, 0x00, pData->pEventAttInfo->OadLen[i] );
				}
				else
				{
				    if( NoDataFlag == 0x55 )
				    {
                        memset( tmpBuf, 0x00, pData->pEventAttInfo->OadLen[i] );
					}
					else
					{
						dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
						dwTemp += dwTemp1;
    					api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], tmpBuf);
					}
				}
			}
			else
			{
				if( NoDataFlag == 0x55 )
				{
					//δ�����¼�
                    memset( tmpBuf, 0x00, pData->pEventAttInfo->OadLen[i] );
				}
				else
				{
					dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
					dwTemp += dwTemp1;
					//���ݵ�ַ���ȶ�FLASH
                    api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], tmpBuf);
				}
			}

			if( OadIndex != 0 )
			{
				pTemp = tmpBuf;
				dwTemp = dwOad;

				for( i = 1; i < OadIndex; i++ ) // pData->OadIndex ��1��ʼ
				{
					dwTemp2 = i;
					dwOad = (dwTemp&0X00FFFFFF);//��ȥ��Ԫ������
					dwOad |= (dwTemp2<<24);//��Ԫ������
					wLen1 = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );
					if( wLen1 == 0x8000 )
					{
						return 0x8000;
					}
					pTemp += wLen1;
				}
			}
			else
			{
				pTemp = tmpBuf;
			}
			
			if( OI < 0x1000 )
			{
				api_EnergyDBaseToEnergy( SwitchNormalEnergy, w/5, w, pTemp );//ת��Ϊ���� wΪ���ݳ��� ÿ������ռ5���ֽ�
			}

			SignFlag = SearchSignEventOI( OI );
			dwOad1 = (pData->dwOad&0x0000ffff);
			//�������й����ʡ��޹����ʡ����ڹ��ʡ����⴦��
			if( (OI == 0x2001) || (OI == 0x2004)
			|| (OI == 0x2005) || (OI == 0x2006) )
			{
				wLen = 3;
				if((OI == 0x2001)&&(pData->pDLT698RecordPara->OI == 0X300D))
				{	
					DWValue.l = 0;
					for( i = 0; i < 3; i++ )
					{
						memcpy( DWValue1.b, pTemp+4*i, 4 );
						DWValue.l += DWValue1.l;
					}
					DWValue.l = DWValue.l /3;
				}
				else
				{
					memcpy( DWValue.b, pTemp, 4 );
				}
				

				if( DWValue.l < 0 )
				{
					DWValue.l *= -1;
					Flag = 1;
				}
				else
				{
				    Flag = 0;
				}

				DWValue.d = lib_DWBinToBCD(DWValue.d);

				if( Flag == 1 )
				{
					DWValue.b[2] |= 0x80;
				}

				memcpy( pData->pBuf+pData->off, DWValue.b, 3 );
		       
		    }
			else if( (OI == 0x2026) || (OI == 0x2027) )//��ѹ��ƽ���ʡ�������ƽ����
			{
			  	wLen = 3;
				DWValue.l = 0;
				memcpy( DWValue.b, pTemp, 2 );
				DWValue.d = lib_DWBinToBCD(DWValue.d);
				memcpy( pData->pBuf+pData->off, DWValue.b, wLen );
			}
			else if( OI == 0x4000 )//УʱOAD
			{
				wLen = 6;
				api_TimeFormat698To645((TRealTimer * )pTemp, Buf );
				lib_ExchangeData( pData->pBuf+pData->off, Buf, 6);  
			}
			else if( OI == 0x4116 ) //������
			{
				wLen = 2;
				WValue.b[0] = lib_BBinToBCD( pTemp[1] );
				WValue.b[1] = lib_BBinToBCD( pTemp[0] ); 
				memcpy( pData->pBuf + pData->off, WValue.b, 2 ); 
			}
			else
			{
				if( wLen == 1 )//����Ϊ1���ֽڵ����ݶ����޷��ŵ�
				{
					DWValue.b[0] = lib_BBinToBCD( pTemp[0] );
					memcpy( pData->pBuf+pData->off, DWValue.b, 1 );
				}
				else if( wLen == 2 )
				{
					memcpy( WValue.b, pTemp, 2 );
					if( SignFlag == 0x55 )
					{
						if( WValue.sw < 0 )
						{
							WValue.sw *= -1;
							Flag = 1;
						}
						else
						{
							Flag = 0;
						}
					}
					WValue.w = lib_WBinToBCD( WValue.w );
					if( SignFlag == 0x55 )
					{
						if( Flag == 1 )
						{
							 WValue.b[1] |= 0x80;
						}
					}
					memcpy( pData->pBuf+pData->off, WValue.b, 2 );
				}
				else if( wLen == 4 )
				{
					memcpy( DWValue.b, pTemp, 4 );
					if( SignFlag == 0x55 )
					{
						if( DWValue.l < 0 )
						{
							DWValue.l *= -1;
							Flag = 1;
						}
						else
						{
							Flag = 0;
						}
					}
					DWValue.d = lib_DWBinToBCD( DWValue.d );

					if( SignFlag == 0x55 )
					{
						if( Flag == 1 )
						{
						    DWValue.b[3] |= 0x80;
						}
					}     

					memcpy( pData->pBuf+pData->off, DWValue.b, 4 );
				}
				else if( wLen == 11 )//����
				{
					memcpy( DWValue.b, pTemp, 4 );
					DWValue.d = lib_DWBinToBCD( DWValue.d );
					//д������������3
					memcpy( pData->pBuf+pData->off, DWValue.b, 3 );
					//����ʱ�������ת�����BCDʱ��
					api_TimeFormat698To645( (TRealTimer *)(pTemp + 4), Buf );
					//д��ʱ�䣬����ʱ�����ȥ2000��û���룬����5
					lib_ExchangeData( pData->pBuf+pData->off + 3, Buf, 5 );
					//645����һ������8
					wLen = 8;
				}
				else
				{
					return 0x8000;
				}
			}

		}
		else
		{
			if( pData->pDLT698RecordPara->OADNum == 1 )//��ȡָ��OAD����֧�ֵ�������������ݴ���
			{
				return 0x8000;
			}

			dwOad = pData->dwOad;
			wLen = api_GetProOADLen( eGetNormalData, 0, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )//��֧�ֵ�������
			{
				if( OI < 0x1000 )//���಻֧�ֵĵ��ܣ�Ĭ�����4�ֽ�0xff
				{
					wLen = 4;
				}
				else if( (OI >= 0x1000) && (OI <= 0x10A3) )	//������
				{
					//����������������֧�֣�����������ϲ�FF�ĳ���
					wLen = 8;	//3����������5����ʱ��
				}
				else if( (OI >= 0x2000) &&(OI <= 0x2027) ) //���಻֧�ֵı��� Ĭ��Ϊ2���ֽ�
				{
					wLen = 2; 
				}
			}

			if( (pData->off+wLen) > pData->Len )
			{
				return 0xFFFF;// ����
			}

			//�������й����ʡ��޹����ʡ����ڹ��ʡ���ѹ��ƽ���ʡ�������ƽ�������⴦��
			if( (OI == 0x2001) || (OI == 0x2004)
			|| (OI == 0x2005) || (OI == 0x2006)
			|| (OI == 0x2026) || (OI == 0x2027) )
			{
				wLen = 3;
				memset( pData->pBuf+pData->off, 0xff, 3 );
			}
			else if( OI == 0x4000 )//УʱOAD
			{
				wLen = 6;
				memset( pData->pBuf+pData->off, 0xff, 6 ); 
			}
			else if( OI == 0x400F )
			{
				//0x400F ֻ����698���ҵ�һ��һ�ֽڵ�OAD��������ɿ��������еļ̵���״̬��ʶ��
				//ʵ���Ͽ���ʹ����698����ͻ��OI��������� ������ֻ����Ϊ����ʹ�������OAD���Լ�������
				//��Ϊ��645�и��ɿ��������¼���¼���״̬����1�ֽ����ݣ���698Ϊ8�ֽڵļ̵����������
				//�����Ҫ����ת��
				//���е�1�ֽڵø��ɿ��������ü̵���״̬
				//����޸Ĵ��ע���޸�645�¼���¼�еø��ɿ��������¼�����
				wLen = 1;
				dwOad = 0x012205F2;
				dwTemp1 = 0;
				for( i = 0; i < pData->pEventAttInfo->NumofOad; i++ )
				{
					if( pData->pEventAttInfo->Oad[i] == dwOad )
					{
						break;
					}
					dwTemp1 += pData->pEventAttInfo->OadLen[i];
				}
				if( i == pData->pEventAttInfo->NumofOad )
				{
					memset(pData->pBuf+pData->off,0xff,1);
				}
				else
				{
				  	memset(RelayErrBuf,0xff,10);
					dwTemp = pData->pEventAddrLen->dwRecordAddr+sizeof(TEventOADCommonData)+pData->pEventSectorInfo->dwSectorAddr;
					dwTemp += dwTemp1;
					api_ReadMemRecordData(dwTemp, pData->pEventAttInfo->OadLen[i], RelayErrBuf);
					
					if( NoDataFlag == 0x55 )
					{
						memset(pData->pBuf+pData->off,0x00,1);
					}
					else
					{
						//�̵��������������ASCII �����Ǳ䳤�� �����Ҫȡ���� ����д�� 
						wLen1 = api_GetProOADLen( eGetNormalData, 0, (BYTE *)&dwOad, 0 );
						if( (wLen1 > (sizeof(RelayErrBuf)+3)) || (wLen1 < 3) )
						{
							return 0x8000; 
						}
						memcpy( pData->pBuf + pData->off, RelayErrBuf + wLen1 - 3, 1 );
					}
				}
			}
			else if( (OI >= 0x1000) && (OI <= 0x10A3) ) //������
			{
				memset( pData->pBuf+pData->off, 0xff, 8 );
			}
			else
			{
				if( wLen == 1 )
				{
					memset( pData->pBuf+pData->off, 0xff, 1 );
				}
				else if( wLen == 2 )
				{
					memset( pData->pBuf+pData->off, 0xff, 2 );
				}
				else if( wLen == 4 )
				{
					memset( pData->pBuf+pData->off, 0xff, 4 );
				}
				else if( wLen == 11 )
				{
					memset( pData->pBuf+pData->off, 0xff, 11 );
				}
				else
				{
					return 0x8000;
				}

			}
		}
	}

	return wLen;
}

//-----------------------------------------------
//��������: ����OAD������
//
//����: 
//		pDLT698RecordPara[in]��		ָ��ṹ��TDLT698RecordPara��ָ��              
//		pEventOADCommonData[in]��	ָ��ṹ��TEventOADCommonData��ָ��                
//		pEventAttInfo[in]��			ָ��ṹ��TEventAttInfo��ָ��                
//		pEventAddrLen[in]��			ָ��ṹ��TEventAddrLen��ָ��                
//		dwPointer[in]��				��ַƫ��ָ��                
//		pBuf[out]��					�������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0                
//		dwOad[in]��					OAD                
//		Len[in]��					�����buf���� �����ж�buf�����Ƿ���                
//		Tag[in]��					0 ����Tag  1 ��Tag                
//		off[in]��					�Ѿ��е�ƫ��                
//		status[in]��				�¼�������ʶ                
//		AllorSep[in]��				1 ָ��OAD 0 ����OAD
//����ֵ:							bit15λ ��1 ���� ��0��ȷ�� 
//									bit0~bit14 �����ص����ݳ���             
//��ע: 
//-----------------------------------------------
static WORD ReadEvent645RecordSub( BYTE NoDataFlag, TEventData *pData )
{
	DWORD dwOad,dwTemp;
	WORD wLen;
	BYTE i,j,Number,Temp;
	BYTE *pTemp,*pAllocBuf;
	
	// ��OI��Ӧ�̶��е�OAD
	for( i = 0; i < (sizeof(EventOadTab)/sizeof(TEventOadTab)); i++ )
	{
		if( pData->pDLT698RecordPara->OI == EventOadTab[i].OI )
		{
			break;
		}
	}

	if( i < (sizeof(EventOadTab)/sizeof(TEventOadTab)) )
	{
		pTemp = (BYTE *)&EventOadTab[i].OadList[0];	// �ҵ�
		Number = EventOadTab[i].Number;
	}
	else
	{
		// Ĭ��Class id 7 �������¼�����������
		pTemp = (BYTE *)&EventOadTab[0].OadList[0];	
		Number = EventOadTab[0].Number;
	}

	pData->off = 0;
	
	if( pData->pDLT698RecordPara->OADNum == 0 )
	{
		return 0x8000;
	}

	pData->AllorSep = 1;
	// ָ��OAD
	for( i = 0; i < pData->pDLT698RecordPara->OADNum; i++ )
	{		    
		memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
		pData->dwOad = dwOad;
		
		// �Ǵ�OI��Ӧ�Ĺ̶���
		wLen = GetDataByOad645( NoDataFlag, pData );

		if( wLen == 0xFFFF )
		{
			return 0; 	// ������
		}
		else if( wLen == 0x8000 )
		{
			return 0x8000; 	// ������
		}		

		pData->off += wLen;
	}	

	return pData->off;
}

//-----------------------------------------------
//��������: ���¼���¼
//
//����: 
//		Tag[in]:				0 ����Tag 1 ��Tag                             
//  	Len[in]��				�����buf���� �����ж�buf�����Ƿ���                
//  	pBuf[out]: 				�������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0	
//����ֵ:			bit15λ��1 �������޴����� ��0������������ȷ���أ� 
//					bit0~bit14 �����صĶ������ݳ���
//��ע: ֻ֧����Last����¼
//-----------------------------------------------
WORD api_ReadEvent645RecordByNo( TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventData			Data;
	TEventSectorInfo	EventSectorInfo;
	DWORD dwTemp;
	WORD twCurrentRecordNo;
	BYTE i,status,NoDataFlag;
	
    NoDataFlag = 0;

	if( GetEventIndex( pDLT698RecordPara->OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return 0x8000;
	}

	EventAddrLen.Phase = pDLT698RecordPara->Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
    if( EventDataInfo.RecordNo > EventAttInfo.MaxRecordNo )
	{
		twCurrentRecordNo = EventAttInfo.MaxRecordNo;
	}
	else
	{
		twCurrentRecordNo = EventDataInfo.RecordNo;		
	}	
	
	if( pDLT698RecordPara->TimeOrLast==0 )// ���������ݵ����, ��0�η���������
	{
		return 0x8000;//������
	}

    //���ݵ�ǰ��¼��Ϊ0���ҵ�ǰ״̬Ϊ�¼�û����
	if( (EventDataInfo.RecordNo == 0) || (pDLT698RecordPara->TimeOrLast > twCurrentRecordNo) )
	{
		NoDataFlag = 0x55;
	}
	
	EventDataInfo.RecordNo -= pDLT698RecordPara->TimeOrLast;	
	
	//���������¼ƫ�Ƶ�ַ
	GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, EventDataInfo.RecordNo, &EventSectorInfo);
	
	dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
	
	status = 0;
	if( EventAddrLen.EventIndex < eNUM_OF_EVENT )
	{
		// �������¼�
		EventAddrLen.Phase = pDLT698RecordPara->Phase;
		i = GetSubEventIndex( (TEventAddrLen*)&EventAddrLen );		
		status = api_DealEventStatus( eGET_EVENT_STATUS, i );	
	}
	else if( EventAddrLen.EventIndex == ePRG_RECORD_NO )
	{
		//�����
		status = GetPrgRecord();
	}
	
	api_ReadMemRecordData( dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
	
	if( NoDataFlag == 0x55 )
	{
		memset( &EventOADCommonData.EventTime, 0xFF, sizeof(EventOADCommonData.EventTime) );
	}

	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pEventOADCommonData = &EventOADCommonData;
	Data.pEventAttInfo = &EventAttInfo;
	//���뿼������������ĵ�ַ
	Data.pEventAddrLen = &EventAddrLen;
	Data.pEventSectorInfo = &EventSectorInfo;
	Data.pBuf = pBuf;
	Data.Len = Len;
	Data.Tag = 0;
	Data.status = status;
	Data.AllorSep = 0;
	Data.Last = (BYTE)pDLT698RecordPara->TimeOrLast;

	return ReadEvent645RecordSub( NoDataFlag,&Data );
}

#endif
//-----------------------------------------------
//��������: ��ʼ���������RAM
//
//����: ��
//	
//����ֵ: ��
//
//��ע: �ϵ磬���������
//-----------------------------------------------
static void InitEventAllRam( void )
{
	// ���¼���ż�����
	ClearPrgRecordRam();
	// �嶨ʱ������
	memset(SubEventTimer, 0x00, sizeof(SubEventTimer));

	memset((BYTE *)&EventStatus, 0x00, sizeof(EventStatus));
}

//-----------------------------------------------
//��������: ������¼���¼����
//
//����: 
//  OI[in]:	OI
//	
//����ֵ:  	TRUE   ��ȷ
//			FALSE  ����
//  
// ��ע:
//-----------------------------------------------
BOOL api_ClrSepEvent( WORD OI )
{
	BYTE i,Type;

	if( GetEventIndex( OI, &Type ) == FALSE )
	{
		return FALSE;
	}
	//���¼���¼
	ClearEventData( Type );
	//���Ӧ�������¼��б��е��ϱ��¼�
	ClrReportAllChannelIndex( Type );
	
	return TRUE;
}

//-----------------------------------------------
//��������: �������¼���¼����
//
//����: 
//  Type[in]:	
//				eFACTORY_INIT = 0,				�������¼���������������¼���¼������¼
//				eEXCEPT_CLEAR_METER,			�������¼�����������������¼
//				eEXCEPT_CLEAR_METER_CLEAR_EVENT	�������¼�����������������¼���¼������¼
//����ֵ:	��
//  
// ��ע:
//-----------------------------------------------
BOOL api_ClrAllEvent( BYTE Type )
{
	TEventAddrLen	EventAddrLen;
	BYTE i;

	for( i = 0; i < eNUM_OF_EVENT_ALL; i++ )
	{
		if( Type == eEXCEPT_CLEAR_METER )
		{
			#if( SEL_PRG_INFO_CLEAR_METER == YES )
			if( i == ePRG_CLEAR_METER_NO )
			{
				continue;	// �¼����㲻���������¼
			}
			#endif
		}
		else if( Type == eEXCEPT_CLEAR_METER_CLEAR_EVENT )
		{
			#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
			if( i == ePRG_CLEAR_EVENT_NO )
			{
				continue;	// �����¼������¼
			}
			#endif
			#if( SEL_PRG_INFO_CLEAR_METER == YES )
			if( i == ePRG_CLEAR_METER_NO )
			{
				continue;	// �����������¼
			}
			#endif
		}
		else if( Type == eFACTORY_INIT )
		{
			// do nothing
		}
		
		EventAddrLen.EventIndex = i;
		EventAddrLen.Phase = 0;
		if( GetEventInfo( &EventAddrLen ) == FALSE )
		{
			continue;
		}
		ClearEventData( i );
	}

	// ������¼��ĵ�������
	api_ClrContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] ), GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag ) );

	ClearReport( Type );
	
	api_ClrRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW );
	api_ClrRunHardFlag( eRUN_HARD_READ_BAT_LOW );
	
	//���645��̼�¼��û�з���ö���б��У���������
	#if(SEL_PRG_RECORD645 == YES)
	api_ClrSafeMem( GET_SAFE_SPACE_ADDR( EventSafeRom.ProRecord645 ), sizeof(TEventDataInfo) );
	#endif

	return TRUE;
}

//�����ܱ��¼ȫʧѹ�¼���ͬʱ����Ӧ���ظ���¼����ʧѹ�¼���
/*
BYTE CheckSpecialEvent(BYTE EventOrder)
{
	//�������û�д���ȫʧѹ���ߵ���״̬�£������������У�����Ҫ�����ж�
	#if( SEL_EVENT_LOST_ALL_V == YES )
	if(GetLostAllVStatus(ePHASE_ALL) == 1)
	{
		//������ڴ���ȫʧѹ״̬�������ʧѹ���ʱ����FALSE
		if( EventOrder == eEVENT_LOST_V_NO )
		{
			return FALSE;
		}
	}
	#endif

	#if( SEL_EVENT_LOST_POWER == YES )
	if(GetLostPowerStatus(ePHASE_ALL) == 1)
	{
		#if( SEL_EVENT_BREAK == YES )
		//������ڴ��ڵ���״̬������������ʱ����FALSE
		if( EventOrder == eEVENT_BREAK_NO )
		{
			return FALSE;
		}
		#endif
	}
	#endif

	return TRUE;

}
*/

// �õ��¼���ʱ���¼���Ŵ�1��ʼ��
BYTE GetEventDelayTime(BYTE EventOrder)
{
	BYTE DelayTime;

	#if( SEL_EVENT_METERCOVER == YES )
	if(EventOrder == eEVENT_METERCOVER_NO)
	{
		// ���ȡ1����ʱ
		DelayTime = 1;
	}
	#endif
	#if( SEL_EVENT_BUTTONCOVER == YES )
	else if(EventOrder == eEVENT_BUTTONCOVER_NO)
	{
		// �˸�ȡ1����ʱ
		DelayTime = 1;	
	}
	#endif
	#if( SEL_EVENT_RELAY_ERR == YES )
	else if(EventOrder == eEVENT_RELAY_ERR_NO)
	{
		// �̵����쳣ȡ1����ʱ
		DelayTime = 1;
	}
	#endif
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	else if(EventOrder == eEVENT_MAGNETIC_INT_NO)
	{
		// ǿ��ȡ5����ʱ
		DelayTime = 5;
	}
	#endif
	#if( SEL_EVENT_POWER_ERR == YES )
	else if(EventOrder == eEVENT_POWER_ERR_NO)
	{
		// ��Դ�쳣ȡ1����ʱ��ʵ�������ã��¼���������ͣ�ϵ�ʱ���м�¼
		DelayTime = 1;
	}
	#endif
	#if( SEL_EVENT_LOST_POWER == YES )
	else if(EventOrder == eEVENT_LOST_POWER_NO)
	{
		// �����¼�ȡ1��
		DelayTime = 1;
	}
	#endif
	#if( SEL_EVENT_RTC_ERR == YES )
	else if(EventOrder == eEVENT_RTC_ERR_NO)
	{
		// ʱ�ӹ���ȡ1��
		DelayTime = 1;
	}
	#endif
	#if( (SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES) || (SEL_EVENT_RELAY_ERR==YES) || (SEL_EVENT_MAGNETIC_INT==YES) || (SEL_EVENT_POWER_ERR==YES) || ( SEL_EVENT_LOST_POWER == YES ) ||( SEL_EVENT_RTC_ERR == YES ) )
	else
	#endif
	{
		// ��䶨ʱ���������¼���¼��ʱ������Ϊ�¼���ʼ��ʱʱ���������ʱʱ����Բ�һ��
		DealEventParaByEventOrder( READ, EventOrder, eTYPE_BYTE, (BYTE *)&DelayTime );
	}

	return DelayTime;
}

//-----------------------------------------------
//��������: �����¼�ʵʱ״̬
//
//����: 
// 			inSubEventIndex[in]		eSUB_EVENT_INDEX
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void SetEventSysStatus(BYTE inSubEventIndex)
{
	BYTE i, j;
	
	i = inSubEventIndex / 8;
	j = inSubEventIndex % 8;

	EventStatus.DelayedStatus[i] |= (0x01<<j);
}


//-----------------------------------------------
//��������: ����¼�״̬
//
//����: 
// 			inSubEventIndex[in]		eSUB_EVENT_INDEX
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void ClrEventSysStatus(BYTE inSubEventIndex)
{
	BYTE i, j;
	
	i = inSubEventIndex / 8;
	j = inSubEventIndex % 8;

	EventStatus.DelayedStatus[i] &= ~(0x01<<j);
}


//-----------------------------------------------
//��������: ��ȡ�¼�״̬
//
//����: 
// 			inSubEventIndex[in]		eSUB_EVENT_INDEX
//                    
//����ֵ:  	TRUE:�¼�״̬��λ�� FALSE:�¼�״̬δ��λ
//
//��ע:   
//-----------------------------------------------
static BOOL GetEventSysStatus(BYTE inSubEventIndex)
{
	BYTE i, j;

	i = inSubEventIndex / 8;
	j = inSubEventIndex % 8;

	if( (EventStatus.DelayedStatus[i] & (0x01<<j)) != 0 )
	{
		// ״̬��Ч
		return TRUE;
	}

	// ״̬��Ч
	return FALSE;
}

//-----------------------------------------------
//��������: �����¼�״̬
//
//����: 
//			Type[in]��			eCLEAR_EVENT_STATUS							
//								eSET_EVENT_STATUS
//								eGET_EVENT_STATUS                    
//			inSubEventIndex[in]	eSUB_EVENT_INDEX
//                    
//����ֵ: TypeΪeGET_EVENT_STATUSʱ��1Ϊ�¼�������δ������0Ϊ�¼�δ���� 	
//				����ʱ  �̶�����0
//��ע:   
//-----------------------------------------------
BYTE api_DealEventStatus(BYTE Type, BYTE inSubEventIndex )
{	
	BYTE Status = 0;

	if( Type == eGET_EVENT_STATUS )
	{
		if( GetEventSysStatus(inSubEventIndex) == TRUE )
		{
			Status = 1;
		}
	}
	else if( Type == eSET_EVENT_STATUS )
	{
		SetEventSysStatus(inSubEventIndex);
	}
	else if( Type == eCLEAR_EVENT_STATUS )
	{
		ClrEventSysStatus(inSubEventIndex);
	}
	else
	{
		ASSERT(0, 0);	
	}

	return Status;
}

//-----------------------------------------------
//��������: �¼���ʱ����ÿ�����һ��
//
//����: ��
//                    
//����ֵ: ��
//			
//��ע:
//-----------------------------------------------
static void EventSecTask( void )
{
	BYTE i,Status,Temp;

	#if( SEL_DEMAND_OVER == YES )
	DealDemandOverMaxDemand();
	#endif

	#if( (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
	FreshUnBalanceRate();
	#endif
	//-------------------------------
	// ���������¼���¼
	// �¼���Ŵ�1��ʼ
	for( i = 0; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		FunctionConst(EVENT_RUN_TASK);

		//�������߱�����B���¼��������޹��������޵ĵ��������¼�
		if( (SubEventInfoTab[i].Phase == ePHASE_B)&&(MeterTypesConst == METER_3P3W) )
		{
			#if(SEL_DEMAND_OVER == YES)//28
			if( i != eSUB_EVENT_QIII_DEMAND_OVER )
			#endif
			{
			    continue;
			}

		}
		#if(SEL_EVENT_COS_OVER == YES)//28
		//�������߱�����ABC�๦�����������¼�
		if( (MeterTypesConst == METER_3P3W)
		&&( (i == eSUB_EVENT_COS_OVER_A)||(i == eSUB_EVENT_COS_OVER_B)||(i == eSUB_EVENT_COS_OVER_C)) )
		{
			continue;   
		}
		#endif
		// �Ƿ��ڻ״̬
		if( SubEventTimer[i] != 0 )
		{
			// ʱ��ݼ�
			SubEventTimer[i]--;

			// �Ƿ�ʱ�䵽
			if( SubEventTimer[i] == 0 )
			{
				// �忴�Ź�
				CLEAR_WATCH_DOG;
				
				// �жϼ�¼��״̬
				if( EventStatus.CurrentStatus[i/8]&(0x01<<(i%8) ) )
				{
					api_DealEventStatus( eSET_EVENT_STATUS, i );
					SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_START, eEVENT_ENDTIME_CURRENT );
				}
				else
				{
					api_DealEventStatus( eCLEAR_EVENT_STATUS, i );
					SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_CURRENT );	
				}
			}
		}
	}
	
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		// �õ�״̬ ��0��1
		Status = SubEventInfoTab[i].pGetEventStatus( SubEventInfoTab[i].Phase ); 
	
		// ��û�б仯�򲻴���
		Temp = (BYTE)( EventStatus.CurrentStatus[i/8] & (0x01<<(i%8)) );
		Temp = (Temp>>(i%8));
		
		if( Status == Temp )
		{
			continue;
		}
	
		// ����ʱ���ڼ���
		if( SubEventTimer[i] != 0 )
		{
			// ֹͣ��ʱ����Ϊ����ʱ�����з�����״̬�仯
			// ��ʱ״̬ȡ�����¼�ֹͣ����
			SubEventTimer[i] = 0;
		}
		else
		{
			SubEventTimer[i] = GetEventDelayTime(SubEventInfoTab[i].EventIndex);
		}
	
		if( Status == 0 )
		{
			EventStatus.CurrentStatus[i/8] &= (BYTE)~(0x01<<(i%8));
		}
		else
		{
			EventStatus.CurrentStatus[i/8] |= (BYTE)(0x01<<(i%8));
		}
	}
	
	ReportSecTask();
}

//-----------------------------------------------
//��������: �ж��¼�״̬
//
//����: ��
//                    
//����ֵ: ��
//			
//��ע: ��ѭ������
//-----------------------------------------------
void api_EventTask(void)
{
	// ��仯
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_SECOND);
		
		EventSecTask();	
	}
	
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_MINUTE) == TRUE )
	{
        api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_MINUTE);
        #if( SEL_DLT645_2007 == YES )
        api_Report645MinTask( );
        #endif
	}
	
	// Сʱ�仯
	if( api_GetTaskFlag(eTASK_EVENTS_ID,eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_EVENTS_ID,eFLAG_HOUR);
		//�ϱ�Сʱ����
		ReportHourTask( );
	}
	
	
}

// �ϵ��¼�����
// ���硢ȫʧѹ����
void api_PowerUpEvent(void)
{
	BYTE i,Buf[((SUB_EVENT_INDEX_MAX_NUM+7)/8)+20];
	WORD Len;
	DWORD dwAddr,PowerOffTime,dwOad;
	TRealTimer t1;

	CLEAR_WATCH_DOG;

	//�������ʱ��
	api_GetPowerDownTime(&t1);
	PowerOffTime = api_CalcIntervalMinute( (TRealTimer *)&RealTimer, (TRealTimer *)&t1 ); 
	
	//д���ع���ʱ��
	api_ReadFromContinueEEPRom(	GET_CONTINUE_ADDR( BatteryTime ), sizeof(DWORD), (BYTE *)&dwAddr);
	dwAddr += PowerOffTime;//��dwAddr �ݴ� ��ع���ʱ��
	api_WriteToContinueEEPRom(	GET_CONTINUE_ADDR( BatteryTime ), sizeof(DWORD), (BYTE *)&dwAddr);
	
	InitEventAllRam();

	// ��ʼ���¼�����	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara );
	api_ReadFromContinueEEPRom( dwAddr, sizeof(TEventPara), (BYTE * )&gEventPara );	
	
	PowerUpReport();
	
	#if( SEL_EVENT_LOST_ALL_V == YES )
	PowerUpDealLostAllV();
	#endif//#if( SEL_EVENT_LOST_ALL_V == YES )

	#if( SEL_EVENT_LOST_POWER == YES )
	// �����¼����ϵ紦��
	LostPowerPowerOn();
	#endif

	#if( SEL_EVENT_LOST_SECPOWER == YES )
	if( SelSecPowerConst == YES )
	// �õ�������Դʧ��״̬
	{    
	    if( PowerDownLostSecPowerFlag == 0x5aa55aa5 )//����ǰ���ڸ�����Դ�е�״̬
	    {
            SaveInstantEventRecord( SubEventInfoTab[eSUB_EVENT_LOST_SECPOWER].EventIndex, SubEventInfoTab[eSUB_EVENT_LOST_SECPOWER].Phase, EVENT_START, eEVENT_ENDTIME_PWRDOWN );//��¼��ʼ
            PowerDownLostSecPowerFlag = 0x12341234;
	    }
		EventStatus.CurrentStatus[eSUB_EVENT_LOST_SECPOWER/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_SECPOWER%8));
		api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_SECPOWER );
	}
	#endif

	#if( SEL_EVENT_METERCOVER == YES )
	// ��ȡ�͹����µñ��״̬λ
	MeterCoverPowerOn();
	#endif

	#if( SEL_EVENT_BUTTONCOVER == YES )
	// ��ȡ�͹����µö�ť��״̬λ
	ButtonCoverPowerOn();
	#endif
	
	#if( SEL_EVENT_RTC_ERR == YES )
	//ʱ�ӹ����¼��ϵ紦��
	RtcErrPowerOn();
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	PowerUpDealPowerErr();
	#endif

	// ����ǰδ�����¼��������ϵ������¼�
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.ProgramPwrDwn );
	api_ReadFromContinueEEPRom( dwAddr, 1, (BYTE *)&i );

	if( i == 0xA5 )
	{
		dwOad = 0xFFFFFFFF;
		PrgFlag = ePROGRAM_STARTED;
		api_SaveProgramRecord( EVENT_END, 0x51, (BYTE *)&dwOad );
		i = 0x5A;
		api_WriteToContinueEEPRom( dwAddr, 1, (BYTE *)&i );
	}
	
	Len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] );
	api_ReadFromContinueEEPRom( dwAddr, Len, Buf );

	for( i = 0; i < SUB_EVENT_INDEX_MAX_NUM; i++ )
	{
		//������ʱ���¼�û�н��������ϵ�ʱ����
		if( Buf[i/8] & (0x01<<(i%8)) )
		{
			if(EventInfoTab[ SubEventInfoTab[i].EventIndex ].SpecialProc == NO)
			{
                #if( SEL_EVENT_RELAY_ERR == YES  )
				if(( api_DealRelayErrFlag(READ,0x0000) == 0xA000 )||( api_DealRelayErrFlag(READ,0x0000)== 0x0A00 ))
				{
					api_DealRelayErrFlag(WRITE,0x00AA);
				}
                #endif
				SaveInstantEventRecord( SubEventInfoTab[i].EventIndex, SubEventInfoTab[i].Phase, EVENT_END, eEVENT_ENDTIME_PWRDOWN );
			}
		}
	}
	
	//������Ϻ�����ݴ���¼������־
	api_ClrContinueEEPRom( dwAddr, Len );
	
	#if(SEL_PRG_RECORD645 == YES)
	PrgFlag645 = ePROGRAM_NOT_YET_START;
	ProRecord645DiNum = 0;
	#endif
	
	#if(SEL_DEBUG_VERSION == YES)
	if( (eEventTypehch[1]==0xff) || (eSubEventTypehch[2]==0xff) )
	{
		CLEAR_WATCH_DOG;
	}
	#endif
}

// ���紦��
// ����ʱ�������¼���¼����ȫʧѹ�������⣩��Ӧ�ý���
// 1������ȫʧѹ���ڷ����������ʱ���ټ�¼ȫʧѹ��ʼ
// 2��ȫʧѹ�����Ϊ�����¼���ȫʧѹ����ʱ����¼�����¼�
void api_PowerDownEvent(void)
{
	BYTE i,len;
	DWORD dwAddr;
	TLostPowerTime LostPowerTime;
	
	// ��¼����ʱ��
	memcpy( LostPowerTime.Time, (BYTE *)&RealTimer, sizeof(TRealTimer) );
	dwAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.LostPowerTime );
	api_VWriteSafeMem( dwAddr, sizeof(TLostPowerTime) , LostPowerTime.Time );

	// ȫʧѹ
	#if(SEL_EVENT_LOST_ALL_V == YES)//��Ϊ���綼֧��	
	PowerDownDealLostAllV();
	LostPowerPowerDown();
	#else	
	LostPowerPowerDown();		
	#endif//#if(SEL_EVENT_LOST_ALL_V == YES)
	
	PowerDownReport();

	#if( SEL_EVENT_METERCOVER == YES )
	MeterCoverPowerOff();
	#endif
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )
    LostSecPowerPowerOff();
    #endif

	#if( SEL_EVENT_BUTTONCOVER == YES )
	ButtonCoverPowerOff();
	#endif

	#if( (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
	UnBalanceRatePowerOff();
	#endif

	#if( SEL_STAT_V_RUN == YES )
	// �����ѹ�ϸ���ת��
	SwapVRunTime();
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	wPowerErrFlag = 0x00000000;
	#endif

	// ���浱ǰ���¼�״̬
	len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] );
	api_WriteToContinueEEPRom( dwAddr, len, (BYTE *)&EventStatus.DelayedStatus[0] );

	i = GetPrgRecord();
	if( i == ePROGRAM_STARTED )													// ��̼�¼��δ����
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.ProgramPwrDwn );
		i = 0xA5;	
		api_WriteToContinueEEPRom( dwAddr, 1, (BYTE *)&i );
	}
	
}

//-----------------------------------------------
//��������: ��д�¼�����
//
//����: 
//			Operation[in]:			WRITE/READ
//			inEventIndex[in]:		eEVENT_INDEX		
//			ParaType[in]:			eEVENT_PARA_TYPE
//			pBuf[out/in]:			��д���ݵĻ���
//                    
//����ֵ:  	���ݳ���  0��������� ��0����ȷ
//
//��ע:  	
//-----------------------------------------------
WORD DealEventParaByEventOrder( BYTE Operation, BYTE inEventIndex, BYTE ParaType, BYTE *pBuf )
{
	DWORD dwAddr;
	WORD wLen;
	BYTE *pTemp;

	wLen = 0;

	if( ParaType == eTYPE_BYTE )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.byLimit[inEventIndex] );	
		pTemp = (BYTE *)&gEventPara.byLimit[inEventIndex];
		wLen = 1;
	}
	else if( ParaType == eTYPE_WORD )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.wLimit[inEventIndex] );	
		pTemp = (BYTE *)&gEventPara.wLimit[inEventIndex];
		wLen = 2;
	}
	else if( ParaType == eTYPE_DWORD )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.dwLimit[inEventIndex] );	
		pTemp = (BYTE *)&gEventPara.dwLimit[inEventIndex];
		wLen = 4;
	}
	//vvvvvvvvvvvv��������vvvvvvvvvvvvvvvvv
	else if( ParaType == eTYPE_SPEC1 )
	{
		#if( SEL_STAT_V_RUN == YES )	//��ѹ�ϸ���
		if( inEventIndex == eSTATISTIC_V_PASS_RATE_NO )
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.VRunLimitLV );	
			pTemp = (BYTE *)&gEventPara.VRunLimitLV;
			wLen = 2;
		}
		#endif

		#if( SEL_EVENT_LOST_V == YES )	//ʧѹ
		if( inEventIndex == eEVENT_LOST_V_NO )	
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.LostVRecoverLimitV );
			pTemp = (BYTE *)&gEventPara.LostVRecoverLimitV;
			wLen = 2;
		}
		#endif
		
		#if( SEL_EVENT_LOST_I == YES )	// ʧ��
		if( inEventIndex == eEVENT_LOST_I_NO )	
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.LostILimitLI );	
			pTemp = (BYTE *)&gEventPara.LostILimitLI;
			wLen = 4;
		}
		#endif

	}
	else if( ParaType == eTYPE_SPEC2 )
	{
		#if( SEL_STAT_V_RUN == YES )	//��ѹ�ϸ���
		if( inEventIndex == eSTATISTIC_V_PASS_RATE_NO )
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.VRunRecoverHV );	
			pTemp = (BYTE *)&gEventPara.VRunRecoverHV;
			wLen = 2;
		}
		#endif

	}
	else if( ParaType == eTYPE_SPEC3 )
	{
		#if( SEL_STAT_V_RUN == YES )	//��ѹ�ϸ���
		if( inEventIndex == eSTATISTIC_V_PASS_RATE_NO )
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.EventPara.VRunRecoverLV );	
			pTemp = (BYTE *)&gEventPara.VRunRecoverLV;
			wLen = 2;
		}
		#endif
	}
	// ��֤�Ƿ������Ƿ���
	if(wLen==0)
	{
		return 0;
	}

	if( Operation == WRITE )
	{
		api_WriteToContinueEEPRom( dwAddr, wLen, pBuf );
		memcpy( pTemp, pBuf, wLen );	// ����RAM
	}
	else
	{
		memcpy( pBuf, pTemp, wLen );
	}

	return wLen;
}

//-----------------------------------------------
//��������: ���¼�����
//
//����: 
//			OI[in]:					OI
//			No[in]:					0: ȫ������
//									N: ��N������
//			pLen[out]:				�������ݵĳ���
//			pBuf[out]:				�����ݵĻ���
//                    
//����ֵ:  	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:  	��ȡram  698��Լ����
//-----------------------------------------------
BOOL api_ReadEventPara( WORD OI, BYTE No, BYTE *pLen, BYTE *pBuf )
{
	BYTE	i,Len,tEventIndex,AllLen;
	BYTE	*pTemp;
	const 	TEventInfoTab	*pParaTable;

	Len = 0;
	AllLen = 0;
	pTemp = pBuf;

	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	
	pParaTable = &EventInfoTab[tEventIndex];

	if( No == 0 )
	{
		for( i = 0; i < pParaTable->ParaNum; i++ )
		{
			Len = DealEventParaByEventOrder( READ, tEventIndex, pParaTable->ParaType[i], pTemp );

			if( Len == 0 )
			{
				return FALSE;	
			}
			pTemp += Len;
			AllLen += Len;
		}
	}
	else
	{
		if(pParaTable->ParaNum<No)
		{
		 	return FALSE;
		}
		
		Len = DealEventParaByEventOrder( READ, tEventIndex, pParaTable->ParaType[No-1], pTemp );
		
		if( Len == 0 )
		{
			return FALSE;
		}
		AllLen = Len;
	}

	*pLen = AllLen;
	
	return TRUE;
}

//-----------------------------------------------
//��������: д�¼�����
//
//����: 
//			OI[in]:					OI
//			No[in]:					0: ȫ������
//									N: ��N������
//			pBuf[in]:				д���ݵĻ���
//                    
//����ֵ:  	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:  	дEEP+RAM  698��Լ����
//-----------------------------------------------
BOOL api_WriteEventPara( WORD OI, BYTE No, BYTE *pBuf )
{
	BYTE	i,Len,tEventIndex;
	BYTE	*pTemp;
	const 	TEventInfoTab	*pParaTable;

	Len = 0;
	pTemp = pBuf;

	if( GetEventIndex( OI, &tEventIndex ) == FALSE )
	{
		return FALSE;
	}
	
	pParaTable = &EventInfoTab[tEventIndex];

	if( No == 0 )
	{
		for( i = 0; i < pParaTable->ParaNum; i++ )
		{
			Len = DealEventParaByEventOrder( WRITE, tEventIndex, pParaTable->ParaType[i], pTemp );

			if( Len == 0 )
			{
				return FALSE;	
			}
			pTemp += Len;
		}
	}
	else
	{
		if(pParaTable->ParaNum<No)
		{
		 	return FALSE;
		}
		
		if( DealEventParaByEventOrder( WRITE, tEventIndex, pParaTable->ParaType[No-1], pTemp ) == 0 )
		{
			return FALSE;	
		}
	}

	return TRUE;
}

//-----------------------------------------------
//��������: ��ʼ�����¼���¼������ֵ
//
//����: ��
//                   
//����ֵ:  ��
//
//��ע:   
//-----------------------------------------------
void FactoryInitParaEventLimit( void )
{
	DWORD dwTmp;

	memset( (BYTE *)&gEventPara, 0x00, sizeof(gEventPara) );
	
	//ʧѹ 0x3000
	#if( SEL_EVENT_LOST_V == YES )
	gEventPara.byLimit[eEVENT_LOST_V_NO] = EventLostVTimeConst;
	gEventPara.wLimit[eEVENT_LOST_V_NO] = (DWORD)wStandardVoltageConst * EventLostVURateConst / 100;//0.78Un
	gEventPara.dwLimit[eEVENT_LOST_V_NO] = (DWORD)wMeterBasicCurrentConst * EventLostVIRateConst / 100;//0.05Ib
	gEventPara.LostVRecoverLimitV = (DWORD)wStandardVoltageConst * EventLostVRecoverLimitVRateConst / 100;//0.85Un
	#endif

	//Ƿѹ 0x3001
	#if( SEL_EVENT_WEAK_V == YES )
	gEventPara.byLimit[eEVENT_WEAK_V_NO] = EventWeakVTimeConst;
	gEventPara.wLimit[eEVENT_WEAK_V_NO] = (DWORD)wStandardVoltageConst * EventWeakVURateConst / 100; //0.78Un
	#endif

	//��ѹ 0x3002
	#if( SEL_EVENT_OVER_V == YES )
	gEventPara.byLimit[eEVENT_OVER_V_NO] = EventOverVTimeConst;
	gEventPara.wLimit[eEVENT_OVER_V_NO] = (DWORD)wStandardVoltageConst * EventOverVURateConst / 100;//1.2Un
	#endif

	//���� 0x3003
	#if( SEL_EVENT_BREAK == YES )
	gEventPara.byLimit[eEVENT_BREAK_NO] = EventBreakTimeConst;
	gEventPara.wLimit[eEVENT_BREAK_NO] = (DWORD)wStandardVoltageConst * EventBreakURateConst / 100; //0.6Un
	gEventPara.dwLimit[eEVENT_BREAK_NO] = (DWORD)wMeterBasicCurrentConst * EventBreakIRateConst / 100; //0.005Ib
	#endif

	//ʧ�� 0x3004
	#if( SEL_EVENT_LOST_I == YES )
	gEventPara.byLimit[eEVENT_LOST_I_NO] = EventLostITimeConst;
	gEventPara.wLimit[eEVENT_LOST_I_NO] = (DWORD)wStandardVoltageConst * EventLostIURateConst / 100;	//0.7Un
	gEventPara.dwLimit[eEVENT_LOST_I_NO] = (DWORD)wMeterBasicCurrentConst * EventLostIIRateConst / 100;	//0.005Ib
	gEventPara.LostILimitLI = (DWORD)wMeterBasicCurrentConst * EventLostILimitLIConst / 10; //5%����� ��λС��
	#endif

	//���� 0x3005 IMax��������3λС������������ֵҪ��4λС��
	#if( SEL_EVENT_OVER_I == YES )
	gEventPara.byLimit[eEVENT_OVER_I_NO] = EventOverITimeConst;
	gEventPara.dwLimit[eEVENT_OVER_I_NO] = (DWORD)dwMeterMaxCurrentConst * EventOverIIRateConst; //1.2Imax
	#endif

	//���� 0x3006
	#if( SEL_EVENT_BREAK_I == YES )
	gEventPara.byLimit[eEVENT_BREAK_I_NO] = EventBreakITimeConst;
	gEventPara.wLimit[eEVENT_BREAK_I_NO] = (DWORD)wStandardVoltageConst * EventBreakIURateConst / 100;    //0.6Un
	gEventPara.dwLimit[eEVENT_BREAK_I_NO] = (DWORD)wMeterBasicCurrentConst * EventBreakIIRateConst / 100; //0.005Ib
	#endif

	//���ʷ��� 0x3007
	#if( SEL_EVENT_BACKPROP == YES )
	gEventPara.byLimit[eEVENT_BACKPROP_NO] = EventBackpTimeConst;
	gEventPara.dwLimit[eEVENT_BACKPROP_NO] = (DWORD)wStandardVoltageConst * wMeterBasicCurrentConst * EventBackpPRateConst / 1000000L; //0.5%��������
	#endif

	//���� 0x3008
	#if( SEL_EVENT_OVERLOAD == YES )
	gEventPara.byLimit[eEVENT_OVERLOAD_NO] = EventOverLoadTimeConst;
	gEventPara.dwLimit[eEVENT_OVERLOAD_NO] = (DWORD)wStandardVoltageConst * dwMeterMaxCurrentConst * EventOverLoadPRateConst / 10000L; //1.2�����
	#endif

	//0x3009	���ܱ������й����������¼�
	//0x300a	���ܱ����й����������¼�
	//0x300b	���ܱ��޹����������¼�
	#if( SEL_DEMAND_OVER == YES )
	gEventPara.byLimit[eEVENT_PA_DEMAND_OVER_NO] = EventDemandOverTimeConst;
	gEventPara.byLimit[eEVENT_NA_DEMAND_OVER_NO] = EventDemandOverTimeConst;
	gEventPara.byLimit[eEVENT_RE_DEMAND_OVER_NO] = EventDemandOverTimeConst;
	dwTmp = (DWORD)(wStandardVoltageConst * dwMeterMaxCurrentConst * EventDemandOverRateConst) / 1000; //����Un*1.2Imax 1λС������λw���˴��Ŵ�10�����������10
	if( MeterTypesConst == METER_3P3W )
	{
		dwTmp = dwTmp * 1732 / 1000 / 10;
	}
	else
	{
		dwTmp = dwTmp * 3 / 10;
	}
	gEventPara.dwLimit[eEVENT_PA_DEMAND_OVER_NO] = dwTmp;
	gEventPara.dwLimit[eEVENT_NA_DEMAND_OVER_NO] = dwTmp;
	gEventPara.dwLimit[eEVENT_RE_DEMAND_OVER_NO] = dwTmp;
	#endif

	//������������ 0x303B
	#if( SEL_EVENT_COS_OVER == YES )
	gEventPara.byLimit[eEVENT_COS_OVER_NO] = EventCosOverTimeConst;//60��
	gEventPara.wLimit[eEVENT_COS_OVER_NO] = EventCosOverRateConst;//0.3��������
	#endif

	//ȫʧѹ 0x300d
	#if( SEL_EVENT_LOST_ALL_V == YES )
	gEventPara.byLimit[eEVENT_LOST_ALL_V_NO] = EventLostAllVTimeConst;
	#endif
	
	//������Դ���� 0x300e
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	gEventPara.byLimit[eEVENT_LOST_SECPOWER_NO] = EventLostSecPowerTimeConst;
	#endif

	//��ѹ������ 0x300f
	#if( SEL_EVENT_V_REVERSAL == YES )
	gEventPara.byLimit[eEVENT_V_REVERSAL_NO] = EventVReversalTimeConst;
	#endif
	
	//���������� 0x3010
	#if( SEL_EVENT_I_REVERSAL == YES )
	gEventPara.byLimit[eEVENT_I_REVERSAL_NO] = EventIReversalTimeConst;
	#endif

	//���� 0x3011
	#if( SEL_EVENT_LOST_POWER == YES )
	gEventPara.byLimit[eEVENT_LOST_POWER_NO] = EventLostPowerTimeConst;
	#endif
	
	
	//��ѹ��ƽ�� 0x301d
	#if( SEL_EVENT_V_UNBALANCE == YES )
	gEventPara.byLimit[eEVENT_V_UNBALANCE_NO] = EventVUnbalanceTimeConst;
	gEventPara.wLimit[eEVENT_V_UNBALANCE_NO] = EventVUnbalanceRateConst;
	#endif

	//������ƽ�� 0x301e
	#if( SEL_EVENT_I_UNBALANCE == YES )
	gEventPara.byLimit[eEVENT_I_UNBALANCE_NO] = EventIUnbalanceTimeConst;
	gEventPara.wLimit[eEVENT_I_UNBALANCE_NO] = EventIUnbalanceRateConst;
	#endif

	//�������ز�ƽ�� 0x302d
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	gEventPara.byLimit[eEVENT_I_S_UNBALANCE_NO] = EventISUnbalanceTimeConst;
	gEventPara.wLimit[eEVENT_I_S_UNBALANCE_NO] = EventISUnbalanceRateConst;
	#endif

	//����оƬ����
	#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
	gEventPara.byLimit[eEVENT_SAMPLECHIP_ERR_NO] = EventSampleChipErrTimeConst;
	#endif

	//���ߵ����쳣
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	gEventPara.byLimit[eEVENT_NEUTRAL_CURRENT_ERR_NO] = EventNeutralCurrentErrTimeConst;
	gEventPara.dwLimit[eEVENT_NEUTRAL_CURRENT_ERR_NO] = wMeterBasicCurrentConst * EventNeutralCurrentErrLimitIConst / 10000;//20%,������������  double-long����λ��A�����㣺-4����
	gEventPara.wLimit[eEVENT_NEUTRAL_CURRENT_ERR_NO] = EventNeutralCurrentErrRatioConst;
	
	#endif

	//0x4030
	#if( SEL_STAT_V_RUN == YES )//��ѹ�ϸ���
	gEventPara.wLimit[eSTATISTIC_V_PASS_RATE_NO] = wStandardVoltageConst * EventStatVRateConst/10;//1.2Un
	gEventPara.VRunLimitLV = (DWORD)wStandardVoltageConst * EventStatVRunLimitLVConst / 100; //0.7Un
	gEventPara.VRunRecoverHV = (DWORD)wStandardVoltageConst * EventStatVRunRecoverHVConst / 100; //1.07Un
	gEventPara.VRunRecoverLV = (DWORD)wStandardVoltageConst * EventStatVRunRecoverLVConst / 100; //0.93Un
	#endif

	//�Ѳ���д�뵽eeprom һ��Ҫ�������!!!!!!
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.EventPara ), sizeof(TEventPara), (BYTE *)&gEventPara );

}

//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_FactoryInitEvent( void )
{
	//��ѹ�ϸ��ʲ���
	#if( SEL_STAT_V_RUN == YES )
	FactoryInitStatisticsVPara();
	#endif
	
	#if((SEL_EVENT_LOST_V==YES)||(SEL_EVENT_WEAK_V==YES)||(SEL_EVENT_OVER_V==YES)||(SEL_EVENT_BREAK==YES)||(SEL_EVENT_LOST_I==YES)||(SEL_EVENT_OVER_I==YES)||(SEL_EVENT_BREAK_I==YES))
	FactoryInitVICorrelatePara();
	#endif
	
	#if( SEL_EVENT_BACKPROP == YES )
	FactoryInitBackPopPara();
	#endif

	#if( SEL_EVENT_OVERLOAD == YES )
	FactoryInitOverLoadPara();
	#endif

	#if( SEL_EVENT_COS_OVER == YES )
	FactoryInitCosOverPara();
	#endif

	#if( SEL_EVENT_MAGNETIC_INT == YES )
	FactoryInitMagneticPara();
	#endif

	#if( SEL_EVENT_RELAY_ERR == YES )
	FactoryInitRelayErrPara();
	#endif
	
	#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
	FactoryInitReversalUnBalancePara();
	#endif

	// ����͸�����Դʧ�����
	#if( (SEL_EVENT_LOST_SECPOWER == YES) || (SEL_EVENT_LOST_POWER == YES) )
	FactoryInitLostPowerPara();
	#endif
	
	#if( SEL_EVENT_LOST_ALL_V == YES )
	FactoryInitLostAllVPara();
	#endif
	
	#if((SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES))
	FactoryInitCoverPara();
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	FactoryInitDemandOverPara();
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	FactoryInitPowerErrPara();
	#endif

	#if( SEL_EVENT_RTC_ERR == YES )
	api_FactoryInitRtcErrPara();
	#endif

	#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
	FactoryInitSampleChipErrPara();
	#endif

	//���ߵ����쳣
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	FactoryInitNeutralCurrentErrPara();
	#endif
	
	FactoryInitPrgRecordPara();

	// �����ϱ���ʶ@@
	FactoryInitReport();
	
	FactoryInitParaEventLimit();
}

// #endif // #if( MAX_EVENT_NUM != 0 )
