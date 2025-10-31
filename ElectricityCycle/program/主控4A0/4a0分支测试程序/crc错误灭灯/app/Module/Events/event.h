//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.10.8
//����		�¼�ģ���ڲ�ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __EVENT_H
#define __EVENT_H
//--------------------------------------

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define FLASH_EVENT_BASE			0x00000000L
#define EVENT_RECORD_LEN			(SECTOR_SIZE*3)	//һ���¼���Ԥ���洢�����С
#define EVENT_PHASE_FOUR			4	// ���������޹�������������������Ϊ4��
#define MAX_EVENTRECORD_NUMBER		10	// �¼���¼����̼�¼���֧��10�μ�¼
#define MAX_PROGRAM_OAD_NUMBER		10	// һ�α�̼�¼���֧��10��OAD
#define EVENT_MAX_PARA_NUM			4	// һ���¼����4������
#define EVENT_LOSTPOWERRECORD_NUM	100	// �����¼����֧��10�μ�¼
#define EVENT_ROGOWSKI_CHANGE_NUM	20	// ��������Ȧ����¼����֧��20�μ�¼
#define EVENT_ROGOWSKI_ERR_NUM		20	// ��������Ȧ�����¼����֧��20�μ�¼
#define EVENT_FIND_UNKNOWN_METER_NUM 16
#define EVENT_TOPU_NUM				 250    // �����¼�����
#define EVENT_TOPU_SIGNAL_NUM        1000   // �����źŸ���

#define	EVENT_START					BIT0//�¼�����
#define	EVENT_END					BIT1//�¼�����

// ����OAD��Ӧ�������ܳ��ȵ����ֵ
#define EVENT_VALID_OADLEN			2048				
// ����OAD��Ӧ�����ݳ��ȵ����ֵ
#define EVENT_ALONE_OADLEN			254

//�����¼�����ռ�õĿռ�
#if( SEL_TOPOLOGY == YES )
#define EVENT_RECORD_TOPO_LEN               (SECTOR_SIZE*20)
#define EVENT_RECORD_TOPO_SIGNAL_LEN        (SECTOR_SIZE*20)
#define EVENT_RECORD_EXT_LEN                (EVENT_RECORD_TOPO_LEN + EVENT_RECORD_TOPO_SIGNAL_LEN)//���������¼��Ĵ��
#else
#define EVENT_RECORD_EXT_LEN        (0)//���������¼��Ĵ��
#endif


//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
// �¼���������
typedef enum
{
	eFACTORY_INIT = 0,					// �������¼���������������¼���¼������¼
	eEXCEPT_CLEAR_METER,				// �������¼�����������������¼
	eEXCEPT_CLEAR_METER_CLEAR_EVENT		// �������¼�����������������¼���¼������¼
}eEVENT_CLEAR_TYPE;

// �¼���¼������
typedef enum
{
	eEVENT_CURRENT_RECORD_NO = 0,	// ��ǰ��¼��
	eEVENT_MAX_RECORD_NO			// ����¼��
}eEVENT_RECORD_NO_TYPE;

// ��ã����ã������¼�״̬ö������
typedef enum
{
	eCLEAR_EVENT_STATUS = 0,
	eSET_EVENT_STATUS,
	eGET_EVENT_STATUS,
	eCLEAR_EVENT_STATUS_CURR = eCLEAR_EVENT_STATUS + 0x80,
	eSET_EVENT_STATUS_CURR   = eSET_EVENT_STATUS + 0x80,
	eGET_EVENT_STATUS_CURR   = eGET_EVENT_STATUS + 0x80,
}eDEAL_EVENT_STATUS;	


typedef enum
{
	eEVENT_ENDTIME_CURRENT = 0, // ��ǰʱ��
	eEVENT_ENDTIME_PWRDOWN		// ����ʱ��
}eEVENT_ENDTIME_TYPE;

// ��̼�¼״̬
typedef enum
{
	ePROGRAM_NOT_YET_START = 0,	// ��̻�δ��ʼ
	ePROGRAM_STARTED			// ����Ѿ���ʼ
}ePROGRAM_STATUS;

// �¼����޲�������
typedef enum
{
	eTYPE_BYTE = 0,	// BYTE���Ͳ�������ʱ
	eTYPE_WORD,		// WORD���Ͳ���
	eTYPE_DWORD,	// DWORD���Ͳ���
	eTYPE_SPEC1,	// �������Ͳ���1
	eTYPE_SPEC2,	// �������Ͳ���2
	eTYPE_SPEC3,	// �������Ͳ���3
	eTYPE_NULL,		// ������Ч
}eEVENT_PARA_TYPE;

//�˶���Ӧ��SubEventInfoTab����˳��һ�£���eEVENT_INDEX˳��һ��
typedef enum
{	//ʧѹ
	#if ( SEL_EVENT_LOST_V == YES )//0
	eSUB_EVENT_LOSTV_A = 0,
	eSUB_EVENT_LOSTV_B,
	eSUB_EVENT_LOSTV_C,
	#endif
		
	//Ƿѹ
	#if( SEL_EVENT_WEAK_V == YES )//3
	eSUB_EVENT_WEAK_V_A,
	eSUB_EVENT_WEAK_V_B,
	eSUB_EVENT_WEAK_V_C,
	#endif
	
	//��ѹ
	#if( SEL_EVENT_OVER_V == YES )//6
	eSUB_EVENT_OVER_V_A,
	eSUB_EVENT_OVER_V_B,
	eSUB_EVENT_OVER_V_C,
	#endif
	
	//����
	#if( SEL_EVENT_BREAK == YES )//9
	eSUB_EVENT_BREAK_A,
	eSUB_EVENT_BREAK_B,
	eSUB_EVENT_BREAK_C,
	#endif
	
	//ʧ��
	#if( SEL_EVENT_LOST_I == YES )//12
	eSUB_EVENT_LOSTI_A,
	eSUB_EVENT_LOSTI_B,
	eSUB_EVENT_LOSTI_C,
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
	#endif
	
	//����
	#if( SEL_EVENT_BREAK_I == YES )//18
	eSUB_EVENT_BREAK_I_A,
	eSUB_EVENT_BREAK_I_B,
	eSUB_EVENT_BREAK_I_C,
	#endif
		
	//���ʷ���
	#if( SEL_EVENT_BACKPROP == YES )//21
	eSUB_EVENT_POW_BACK_ALL,
	eSUB_EVENT_POW_BACK_A,
	eSUB_EVENT_POW_BACK_B,
	eSUB_EVENT_POW_BACK_C,
	#endif
	
	//����
	#if( SEL_EVENT_OVERLOAD == YES )//25
	eSUB_EVENT_OVERLOAD_A,
	eSUB_EVENT_OVERLOAD_B,
	eSUB_EVENT_OVERLOAD_C,
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
	#endif
	
	//������������
	#if( SEL_EVENT_COS_OVER == YES )//34
	eSUB_EVENT_COS_OVER_ALL,
	eSUB_EVENT_COS_OVER_A,
	eSUB_EVENT_COS_OVER_B,
	eSUB_EVENT_COS_OVER_C,
	#endif	
	
	//ȫʧѹ
	#if( SEL_EVENT_LOST_ALL_V == YES )//38
	eSUB_EVENT_LOST_ALL_V,
	#endif
	
	//������Դʧ���¼�
	#if( SEL_EVENT_LOST_SECPOWER == YES )//39
	eSUB_EVENT_LOST_SECPOWER,
	#endif
	
	//��ѹ������
	#if( SEL_EVENT_V_REVERSAL == YES )//40
	eSUB_EVENT_V_REVERSAL,
	#endif
	
	//����������
	#if( SEL_EVENT_I_REVERSAL == YES )//41
	eSUB_EVENT_I_REVERSAL,
	#endif

	//�����¼�
	#if(SEL_EVENT_LOST_POWER == YES)//42
	eSUB_EVENT_LOST_POWER,			//���� 1
	#endif
	
	//������¼�
	#if(SEL_EVENT_METERCOVER == YES)//43
	eSUB_EVENT_METER_COVER,			//���� 2
	#endif
	
	//����ť���¼�
	#if(SEL_EVENT_BUTTONCOVER == YES)//44
	eSUB_EVENT_BUTTON_COVER,
	#endif
		
	//��ѹ��ƽ��
	#if(SEL_EVENT_V_UNBALANCE == YES)//45
	eSUB_EVENT_V_UNBALANCE,
	#endif
	
	//������ƽ��
	#if(SEL_EVENT_I_UNBALANCE == YES)//46
	eSUB_EVENT_I_UNBALANCE,
	#endif
	
	//�㶨�ų�����
	#if(SEL_EVENT_MAGNETIC_INT == YES)//47
	eSUB_EVENT_MAGNETIC_INT,
	#endif
	
	//���ɿ�������
	#if(SEL_EVENT_RELAY_ERR == YES)	//48
	eSUB_EVENT_RELAY_ERR,			//���� 3
	#endif
	
	//��Դ�쳣
	#if(SEL_EVENT_POWER_ERR == YES)	//49
	eSUB_EVENT_POWER_ERR,			//���� 4
	#endif
	
	//�������ز�ƽ��
	#if(SEL_EVENT_I_S_UNBALANCE == YES)//50
	eSUB_EVENT_S_I_UNBALANCE,		
	#endif
	
	//ʱ�ӹ���
	#if(SEL_EVENT_RTC_ERR == YES)	//51
	eSUB_EVENT_RTC_ERR,				//���� 5
	#endif
	
	// ����оƬ����
	#if(SEL_EVENT_SAMPLECHIP_ERR == YES)//52
	eSUB_EVENT_SAMPLECHIP_ERR,		//���� 6
	#endif

	// ���ܱ����ߵ����쳣
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )//53
	eSUB_NEUTRAL_CURRENT_ERR_NO,//���� 7 		
	#endif

	//�ն�״̬����λ
	#if( SEL_EVENT_DI_CHANGE == YES )//54
	eSUB_EVENT_DI_CHANGE,//���� 8
	#endif
	
	eSUB_EVENT_CHARGING_WARNING,//�澯
	eSUB_EVENT_CHARGING_ERR,//����
	eSUB_EVENT_CHARGING_COM_EXCTION,//ͨ���쳣

	// ������Ȧ����¼�
	#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )
	eSUB_ROGOWSKI_CHANGE,              
	#endif

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    eSUB_FIND_UNKNOWN_METER,            //����δ֪���ܱ��¼�
    #endif

	//�ն˳���ʧ���¼�
	#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
	eSUB_ROGOWSKI_COMM_FAIL,              
	#endif

	//�����¼�
	#if( SEL_TOPOLOGY == YES )
	eSUB_EVENT_TOPU,  
    eSUB_EVENT_TOPU_SIGNAL, 
	#endif

	#if( SEL_PRG_MODULE == YES )
    eSUB_EVENT_MODULE_0,
	eSUB_EVENT_MODULE_1,
	eSUB_EVENT_MODULE_2,
	eSUB_EVENT_MODULE_3,
	eSUB_EVENT_MODULE_4,
	eSUB_EVENT_MODULE_5,
	eSUB_EVENT_MODULE_6,
	eSUB_EVENT_MODULE_7,
	eSUB_EVENT_MODULE_8,
	eSUB_EVENT_MODULE_9,
	#endif
	//���ܱ����¼�//54
	eSUB_EVENT_PRG_RECORD,			//���� 8
	
	//���ܱ������¼�
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//55		
	eSUB_EVENT_PRG_CLEAR_METER,		//���� 9		
	#endif
	
	//���ܱ����������¼�	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )//56		
	eSUB_EVENT_PRG_CLEAR_MD,				
	#endif
											
	//���ܱ��¼������¼�	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//57	
	eSUB_EVENT_PRG_CLEAR_EVENT,			//���� 10		
	#endif
	
	//���ܱ�Уʱ�¼�		
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//58	
	eSUB_EVENT_PRG_ADJUST_TIME,			//���� 11		
	#endif
	
	//���ܱ�ʱ�α����¼�	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )//59	
	eSUB_EVENT_PRG_TIME_TABLE,			//���� 12			
	#endif
	
	//���ܱ�ʱ�������¼�	
	#if( SEL_PRG_INFO_TIME_AREA == YES )//60	
	eSUB_EVENT_PRG_TIME_AREA,			//���� 13			
	#endif
	
	//���ܱ������ձ���¼�	
	#if( SEL_PRG_INFO_WEEK == YES )//61		
	eSUB_EVENT_PRG_WEEKEND,				//���� 14	
	#endif
	
	//���ܱ�����ձ���¼�	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//62	
	eSUB_EVENT_PRG_CLOSING_DAY,			//���� 15			
	#endif
	
	//���ܱ���բ��բ�¼�			
	eSUB_EVENT_PRG_OPEN_RELAY,		//63���� 16			
	eSUB_EVENT_PRG_CLOSE_RELAY,		//64���� 17		
	
	//�ڼ��ձ�̼�¼		
	#if( SEL_PRG_INFO_HOLIDAY == YES )//65		
	eSUB_EVENT_PRG_HOLIDAY,			//���� 18		
	#endif
	
	//�й���Ϸ�ʽ��̼�¼	
	#if( SEL_PRG_INFO_P_COMBO == YES )//66		
	eSUB_EVENT_ePRG_P_COMBO,		//���� 19				
	#endif
	
	//�޹���Ϸ�ʽ��̼�¼	
	#if( SEL_PRG_INFO_Q_COMBO == YES )//67		
	eSUB_EVENT_PRG_Q_COMBO,			
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	eSUB_TARIFF_TABLE_NO,       //68���� 20       //���ܱ���ʲ��������¼�
	eSUB_LADDER_TABLE_NO,       //69���� 21       // ���ܱ���ݱ����¼�
	#endif

	//���ܱ���Կ�����¼�
	#if( SEL_PRG_UPDATE_KEY == YES )//70
	eSUB_EVENT_PRG_UPDATE_KEY,  //���� 22    
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	eSUB_ABR_CARD_NO,			//71���� 23        //���ܱ��쳣�忨�¼�
	eSUB_BUY_MONEY_NO,			//72���� 24        //���ܱ����¼
	eSUB_RETURN_MONEY_NO,		//73���� 25        //���ܱ��˷Ѽ�¼
	#endif
	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )
	eSUB_EVENT_BROADJUST_TIME,//74���� 26		//�㲥Уʱ�¼�
	#endif
	
	eSUB_EVENT_STATUS_TOTAL             //���ֵ72 ����27

}eSUB_EVENT_INDEX;

//�˶���Ӧ��EventInfoTab����˳��һ�£���eSUB_EVENT_INDEX˳��һ��
typedef enum
{
	// vvvvvvvvvvv�������¼vvvvvvvvvvvvvv
	#if ( SEL_EVENT_LOST_V == YES )
	eEVENT_LOST_V_NO,						// ʧѹ*3
	#endif
	
	#if( SEL_EVENT_WEAK_V == YES )
	eEVENT_WEAK_V_NO,						// Ƿѹ*3
	#endif
	
	#if( SEL_EVENT_OVER_V == YES )
	eEVENT_OVER_V_NO,						// ��ѹ*3
	#endif
	
	#if( SEL_EVENT_BREAK == YES )
	eEVENT_BREAK_NO,						// ����*3
	#endif	
	
	#if( SEL_EVENT_LOST_I == YES )
	eEVENT_LOST_I_NO,						// ʧ��*3
	#endif
	
	#if( SEL_EVENT_OVER_I == YES )
	eEVENT_OVER_I_NO,						// ����*3
	#endif
	
	#if( SEL_EVENT_BREAK_I == YES )
	eEVENT_BREAK_I_NO,						// ����*3
	#endif
	
	#if( SEL_EVENT_BACKPROP == YES )
	eEVENT_BACKPROP_NO,						// ���ʷ���*4
	#endif
	
	#if( SEL_EVENT_OVERLOAD == YES )
	eEVENT_OVERLOAD_NO,						// ����*3
	#endif
	
	#if( SEL_DEMAND_OVER == YES )
	eEVENT_PA_DEMAND_OVER_NO,				//�����й�����*1
	eEVENT_NA_DEMAND_OVER_NO,				//�����й�����*1
	eEVENT_RE_DEMAND_OVER_NO,				//�޹�����*4
	#endif
	
	#if( SEL_EVENT_COS_OVER == YES )
	eEVENT_COS_OVER_NO,						// ��������������*1
	#endif
	
	#if( SEL_EVENT_LOST_ALL_V == YES )
	eEVENT_LOST_ALL_V_NO,					// ȫʧѹ*1
	#endif	
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	eEVENT_LOST_SECPOWER_NO,				//  ������Դʧ��*1
	#endif	
	
	#if( SEL_EVENT_V_REVERSAL == YES )
	eEVENT_V_REVERSAL_NO,					// ��ѹ������*1
	#endif
	
	#if( SEL_EVENT_I_REVERSAL == YES )
	eEVENT_I_REVERSAL_NO,					// ����������*1
	#endif
	
	#if( SEL_EVENT_LOST_POWER == YES )
	eEVENT_LOST_POWER_NO,					// ����*1
	#endif
	
	#if( SEL_EVENT_METERCOVER == YES )
	eEVENT_METERCOVER_NO,					// �����*1
	#endif
	
	#if( SEL_EVENT_BUTTONCOVER == YES )
	eEVENT_BUTTONCOVER_NO,					// ����ť��*1
	#endif	
	
	#if( SEL_EVENT_V_UNBALANCE == YES )
	eEVENT_V_UNBALANCE_NO,					// ��ѹ��ƽ��*1
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	eEVENT_I_UNBALANCE_NO,					// ������ƽ��*1
	#endif			
	
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	eEVENT_MAGNETIC_INT_NO,					// �㶨�ų�����*1
	#endif
	
	#if( SEL_EVENT_RELAY_ERR == YES )
	eEVENT_RELAY_ERR_NO,					// ���ɿ�����*1
	#endif
	
	#if( SEL_EVENT_POWER_ERR == YES )
	eEVENT_POWER_ERR_NO,					// ��Դ�쳣*1
	#endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	eEVENT_I_S_UNBALANCE_NO,				// �������ز�ƽ��*1
	#endif

	#if(SEL_EVENT_RTC_ERR == YES)
	eEVENT_RTC_ERR_NO,						//ʱ�ӹ���*1
	#endif
	
	#if(SEL_EVENT_SAMPLECHIP_ERR == YES)
	eEVENT_SAMPLECHIP_ERR_NO,				// ����оƬ����*1
	#endif
	
	#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
	eEVENT_NEUTRAL_CURRENT_ERR_NO,              // ���ܱ����ߵ����쳣
	#endif
	
	#if( SEL_EVENT_DI_CHANGE == YES )
	eEVENT_DI_CHANGE_NO,						//�ն�״̬����λ�¼�
	#endif
	
	eEVENT_CHARGING_WARNING_NO,//�澯
	eEVENT_CHARGING_ERR_NO,//����
	eEVENT_CHARGING_COM_EXCTION_NO,//ͨ���쳣
	
	#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )
	eEVENT_ROGOWSKI_CHANGE_NO,              // ������Ȧ����¼�
	#endif

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    eEVENT_FIND_UNKNOWN_METER_NO,           //����δ֪���ܱ��¼�
    #endif
	//�ն˳���ʧ���¼�
	#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
	eSUB_ROGOWSKI_COMM_FAIL_NO,              
	#endif

	#if( SEL_TOPOLOGY == YES )
	eEVENT_TOPU_NO,   
    eEVENT_TOPU_SIGNAL_NO,          
	#endif

	#if( SEL_PRG_MODULE == YES )
    eSUB_EVENT_MODULE_0_NO,
	eSUB_EVENT_MODULE_1_NO,
	eSUB_EVENT_MODULE_2_NO,
	eSUB_EVENT_MODULE_3_NO,
	eSUB_EVENT_MODULE_4_NO,
	eSUB_EVENT_MODULE_5_NO,
	eSUB_EVENT_MODULE_6_NO,
	eSUB_EVENT_MODULE_7_NO,
	eSUB_EVENT_MODULE_8_NO,
	eSUB_EVENT_MODULE_9_NO,
	#endif
	eNUM_OF_EVENT,							// ��������¼�����28

	// vvvvvvvvvvv������¼vvvvvvvvvvvvvv			
	ePRG_RECORD_NO = eNUM_OF_EVENT,				// ���ܱ����¼�

	#if( SEL_PRG_INFO_CLEAR_METER == YES )
	ePRG_CLEAR_METER_NO,						// �����¼
	#endif	
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )
	ePRG_CLEAR_MD_NO,							// ��������¼
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
	ePRG_CLEAR_EVENT_NO,						// ���¼���¼
	#endif
	
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )
	ePRG_ADJUST_TIME_NO,						// Уʱ��¼
	#endif
	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )
	ePRG_TIME_TABLE_NO,							// ʱ�α��̼�¼
	#endif
	
	#if( SEL_PRG_INFO_TIME_AREA == YES )
	ePRG_TIME_AREA_NO,							// ʱ�����̼�¼
	#endif
	
	#if( SEL_PRG_INFO_WEEK == YES )
	ePRG_WEEKEND_NO,							// �����ձ�̼�¼
	#endif
	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )
	ePRG_CLOSING_DAY_NO,						// �����ձ�̼�¼
	#endif
	
	ePRG_OPEN_RELAY_NO,							// ��բ
	ePRG_CLOSE_RELAY_NO,						// ��բ
	
	#if( SEL_PRG_INFO_HOLIDAY == YES )
	ePRG_HOLIDAY_NO,							// �ڼ��ձ�̼�¼
	#endif
	
	#if( SEL_PRG_INFO_P_COMBO == YES )
	ePRG_P_COMBO_NO,							// �й���Ϸ�ʽ��̼�¼
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )
	ePRG_Q_COMBO_NO,							// �޹���Ϸ�ʽ��̼�¼
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	ePRG_TARIFF_TABLE_NO,                       //���ܱ���ʲ��������¼�
	ePRG_LADDER_TABLE_NO,                       // ���ܱ���ݱ����¼�
	#endif

	#if( SEL_PRG_UPDATE_KEY == YES )
	ePRG_UPDATE_KEY_NO,                         // ��Կ���¼�¼
	#endif

	#if(PREPAY_MODE == PREPAY_LOCAL)
	ePRG_ABR_CARD_NO,                           //���ܱ��쳣�忨�¼�
	ePRG_BUY_MONEY_NO,                          //���ܱ����¼
	ePRG_RETURN_MONEY_NO,                       //���ܱ��˷Ѽ�¼
	#endif
	
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )
	ePRG_BROADJUST_TIME_NO,						// �㲥Уʱ��¼
	#endif
	
	eNUM_OF_EVENT_PRG,                          // �¼��ͱ��������42

	// vvvvvvvvvvv֮����¼����洢��¼vvvvvvvvvvvvvv
	#if( SEL_STAT_V_RUN == YES )
	eSTATISTIC_V_PASS_RATE_NO = eNUM_OF_EVENT_PRG,
	#endif

	eNUM_OF_EVENT_ALL,							// ���е��¼��������������洢��¼��44
	
}eEVENT_INDEX;

#define MAX_EVENT_NUM 			eNUM_OF_EVENT

#define SUB_EVENT_INDEX_MAX_NUM	eSUB_EVENT_STATUS_TOTAL		// �¼�״̬λ����
typedef struct TEventStatus_t
{
	BYTE CurrentStatus[(SUB_EVENT_INDEX_MAX_NUM+7)/8];	// �¼�ʵʱ״̬
	BYTE DelayedStatus[(SUB_EVENT_INDEX_MAX_NUM+7)/8]; // ������ʱ���״̬
}TEventStatus;

#pragma pack(1)
// ʱ������
typedef struct TEventTime_t
{
	// ����ʱ��
	TRealTimer BeginTime;
	// ����ʱ��
	TRealTimer EndTime;

}TEventTime;

typedef struct TLostPowerTime_t
{
	BYTE	Time[7];		// ����ʱ��
	DWORD	CRC32;
}TLostPowerTime;

#pragma pack()

typedef struct TEventOADCommonData_t
{
	DWORD			EventNo;					//�¼���¼���@@
	TEventTime		EventTime;					//�¼������ͽ���ʱ��
}TEventOADCommonData;

typedef struct TEventDataInfo_t
{
	DWORD	RecordNo;						//�¼���¼���(��������)
	DWORD	AccTime;						//�¼��ۼ�ʱ��(ֻ�й������¼���Ч)
	DWORD	CRC32;
}TEventDataInfo;


typedef struct TEventAddrLen_t
{
	DWORD	dwAttInfoEeAddr;
	DWORD	dwDataInfoEeAddr;
	DWORD	dwRecordAddr;					//�¼����ݴ洢����ַ�������洢�ռ�
	BYTE	EventIndex;
	BYTE	Phase;
	BYTE	SubEventIndex;
}TEventAddrLen;
// ÿ�μ���ƫ�Ƶ�ַ��
typedef struct TEventSectorInfo_t
{		
	DWORD		dwPointer;		// ��ָ�룬��ǰNO����ȵ�ȡ��
	DWORD		dwSectorAddr;			// Ѱ�ҵ��ַ	
	DWORD		dwSaveSpace;	// ���¼���Ҫ�õ���������
}TEventSectorInfo;

typedef struct TSubEventInfoTab_t
{
	eSUB_EVENT_INDEX	SubEventIndex;
	eEVENT_INDEX		EventIndex;	
	ePHASE_TYPE			Phase;
	BYTE 	(*pGetEventStatus)( BYTE Phase );	// ��ȡ�¼�״̬
}TSubEventInfoTab;

typedef struct TEventInfoTab_t
{
	WORD	OI;
	BYTE 	EventIndex;							//Event����
	BYTE 	NumofPhase;							//��λ��  ���������޹���������Ϊ4��
	BYTE	SpecialProc;						//�������⴦��
	WORD	ExtDataLen;							//��չ���ݳ��� ���¼������¼��OMD
	BYTE 	ParaNum;							//�����ĸ���
	BYTE 	ParaType[EVENT_MAX_PARA_NUM];		//����������
}TEventInfoTab;

// vvvvvvvvvvvvvvvvvv��������������ݴ洢�ռ�vvvvvvvvvvvvvvvv
// Ĭ����OAD���¼����õ����ݽṹ��
typedef struct TEventNormal_t
{
	// #if( MAX_PHASE == 1)
	DWORD	Energy[4]; 	// ����������ʱ�������й��ܵ���
	BYTE	Reserved[10];		// Ԥ��
	// #else
	// #endif
}TEventNormal;
// ^^^^^^^^^^^^^^^^^^^��������������ݴ洢�ռ�^^^^^^^^^^^^^^^^^^
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern TEventStatus EventStatus;
extern const TSubEventInfoTab	SubEventInfoTab[];
extern const DWORD FixedColumnOadTab[];
//-----------------------------------------------
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ��д�¼�����
//
//����: 
//			Operation[in]:			WRITE/READ
//			inEventIndex[in]:		eEVENT_INDEX		
//			ParaType[in]:			eEVENT_PARA_TYPE
//			pBuf[out/in]:			��д���ݵĻ���
//                    
//����ֵ:  	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:  	�¼��ж�ʱ����(��)
//-----------------------------------------------
WORD DealEventParaByEventOrder( BYTE Operation, BYTE inEventIndex, BYTE ParaType, BYTE *pBuf );
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
BOOL WriteEventRecordData(DWORD Addr, WORD Length, DWORD BaseAddr, DWORD MemSpace, BYTE * pBuf);


//-----------------------------------------------
//��������: ���湤�����¼���¼��ʧѹʧ���ȣ�
//
//����: 
//	inEventIndex[in]:	eEVENT_INDEX 
//	Phase[in]:			ePHASE_TYPE  
//	BeforeAfter[in]:	�¼�����ǰ/������ eEVENT_TIME_TYPE
//  					����ʱ,����+1,ƫ��++,����ʱָ��--  
//	EndTimeType[in]:	eEVENT_ENDTIME_CURRENT  ��ǰʱ��
// 						eEVENT_ENDTIME_PWRDOWN  ����ʱ��
//  
//����ֵ:				TRUE/FALSE  
//��ע:
//-----------------------------------------------
BOOL SaveInstantEventRecord( BYTE inEventIndex, BYTE Phase, BYTE BeforeAfter, BYTE EndTimeType );
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
BOOL GetEventInfo( TEventAddrLen *pEventAddrLen );
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
BOOL GetEventSectorInfo( WORD Len, WORD RecordDepth, DWORD inRecordNo, TEventSectorInfo *pEventSectorInfo );

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
BOOL GetEventIndex( WORD OI, BYTE *pEventIndex );

//-----------------------------------------------
//��������: ��ȡinEventIndex��Ӧ��OI
//����: 
//			inEventIndex[in]:	�¼�ö�ٺ�              
//����ֵ:	OI		
//��ע:
//-----------------------------------------------
WORD GetEventOI( BYTE inEventIndex );

//-----------------------------------------------
//��������: �����¼����ͼ���λȷ�������¼�ö�ٺ�
//
//����: 
//			EventIndex[in]:	�¼�ö�ٺ�                    
//			Phase[in]:		��λ                    
//����ֵ:	�����¼�ö�ٺ�	
//��ע:
//-----------------------------------------------
BYTE GetSubEventIndex(TEventAddrLen *pEventAddrLen);
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
WORD ReadEventRecordByNo( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );
//-----------------------------------------------
//��������: ���������ϱ���֡
//
//����:	 
//         
//����ֵ:   
//			
//��ע: 
//-----------------------------------------------
void LostPowerStartReport( void );


#endif//��Ӧ�ļ���ǰ���#define __EVENT_H
