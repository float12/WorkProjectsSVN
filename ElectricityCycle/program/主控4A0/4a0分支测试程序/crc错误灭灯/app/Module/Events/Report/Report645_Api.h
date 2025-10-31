#ifndef __REPORT645_API_H
#define __REPORT645_API_H

#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define RPT_MODE_LEN			8	//�����ϱ�ģʽ�ֳ��ȣ�8�ֽ�
#define RPT_STATUS_LEN			12	//�����ϱ�״̬�ֳ��ȣ�12�ֽ�
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//�����ϱ�״̬�ֶ�ȡ��ʽö��
typedef enum
{
	eREAD_STATUS_PROC = 0,	//��Լ��ȡ
	eREAD_STATUS_OTHER		//�ǹ�Լ��ȡ
}eREAD_STATUS_TYPE;

//�����ϱ�״̬��������
typedef enum
{
	//���ɿ����󶯻�ܶ�--0
	eSUB_STATUS_RELAY_ERR = 0,					

	//ESAM����--1
	eSUB_STATUS_ESAM_ERR = 1,					
	
	//�ڿ���ʼ��������洢�����ϻ��𻵺�����ͬ��--2
	eSUB_STATUS_ROM_ERR = 2,					
	
	//ʱ�ӵ�ص�ѹ��--3
	eSUB_STATUS_CLOCK_BAT_LOW = 3,
	
	//�ڲ��������--4
	eSUB_STATUS_CODE_ERR = 4,

	//�洢�����ϻ���--5
	eSUB_STATUS_MEM_ERR = 5,					

	//����--6
	eSUB_STATUS_BIT6 = 6,
	
	//ʱ�ӹ���--7
	eSUB_STATUS_RTC_ERR = 7,
	
	//ͣ�糭����Ƿѹ--8
	eSUB_STATUS_READ_BAT_LOW = 8,			

	//͸֧״̬--9
	eSUB_STATUS_OVER_DRAFT = 9,			
	
	//�����--10
	eSUB_STATUS_METER_COVER = 10,
	
	//����ť��--11
	eSUB_STATUS_BUTTON_COVER = 11,
	
	//�㶨�ų�����--12
	eSUB_STATUS_MAGNETIC_INT = 12,

	//��Դ�쳣--13
	eSUB_STATUS_POWER_ERR = 13,

	//��բ�ɹ�--14
	eSUB_STATUS_PRG_OPEN_RELAY = 14,	

	//��բ�ɹ�--15
	eSUB_STATUS_PRG_CLOSE_RELAY = 15,	

	//A��ʧѹ--16
	eSUB_STATUS_LOSTV_A = 16,

	//A��Ƿѹ--17
	eSUB_STATUS_WEAK_V_A = 17,

	//A���ѹ--18
	eSUB_STATUS_OVER_V_A = 18,

	//A��ʧ��--19
	eSUB_STATUS_LOSTI_A = 19,

	//A�����--20
	eSUB_STATUS_OVER_I_A = 20,

	//A�����--21
    eSUB_STATUS_OVERLOAD_A = 21,

	//A�๦�ʷ���--22
	eSUB_STATUS_POW_BACK_A = 22,
	
	//A�����--23
	eSUB_STATUS_BREAK_A = 23,

	//A�����--24
	eSUB_STATUS_BREAK_I_A = 24,

	//����--25
	eSUB_STATUS_BIT25 = 25,

	//����--26
	eSUB_EVENT_BIT26 = 26,

	//����--27
	eSUB_STATUS_BIT27 = 27,
	
	//����--28
	eSUB_STATUS_BIT28 = 28,
	
	//����--29
	eSUB_STATUS_BIT29 = 29,
	
	//����--30
	eSUB_STATUS_BIT30 = 30,
	
	//����--31
	eSUB_STATUS_BIT31 = 31,
	
	//B��ʧѹ--32
	eSUB_STATUS_LOSTV_B = 32,
	
	//B��Ƿѹ--33
	eSUB_STATUS_WEAK_V_B = 33,

	//B���ѹ--34
	eSUB_STATUS_OVER_V_B = 34,
	
	//B��ʧ��--35
	eSUB_STATUS_LOSTI_B = 35,

	//B�����--36
	eSUB_STATUS_OVER_I_B = 36,

	//B�����--37
	eSUB_STATUS_OVERLOAD_B = 37,

	//B�๦�ʷ���--38
	eSUB_STATUS_POW_BACK_B = 38,

	//B�����--39
	eSUB_STATUS_BREAK_B = 39,

	//B�����--40
	eSUB_STATUS_BREAK_I_B = 40,

	//����--41
	eSUB_STATUS_BIT41 = 41,

	//����--42
	eSUB_STATUS_BIT42 = 42,

	//����--43
	eSUB_STATUS_BIT43 = 43,

	//����--44
	eSUB_STATUS_BIT44 = 44,
	
	//����--45
	eSUB_STATUS_BIT45 = 45,

	//����--46
	eSUB_STATUS_BIT46 = 46,

	//����--47
	eSUB_STATUS_BIT47 = 47,

	//C��ʧѹ--48
	eSUB_STATUS_LOSTV_C = 48,
	
	//C��Ƿѹ--49
	eSUB_STATUS_WEAK_V_C = 49,

	//C���ѹ--50
	eSUB_STATUS_OVER_V_C = 50,

	//C��ʧ��--51
	eSUB_STATUS_LOSTI_C = 51,

	//C�����--52
	eSUB_STATUS_OVER_I_C = 52,

	//C�����--53
	eSUB_STATUS_OVERLOAD_C = 53,

	//C�๦�ʷ���--54
	eSUB_STATUS_POW_BACK_C = 54,

	//C�����--55
	eSUB_STATUS_BREAK_C = 55,
	
	//C�����--56
	eSUB_STATUS_BREAK_I_C = 56,

	//����--57
	eSUB_STATUS_BIT57 = 57,

	//����--58
	eSUB_STATUS_BIT58 = 58,

	//����--59
	eSUB_STATUS_BIT59 = 59,

	//����--60
	eSUB_STATUS_BIT60 = 60,

	//����--61
	eSUB_STATUS_BIT61 = 61,

	//����--62
	eSUB_STATUS_BIT62 = 62,

	//����--63
	eSUB_STATUS_BIT63 = 63,

	//��ѹ������-64
	eSUB_STATUS_V_REVERSAL = 64,

	//����������--65
	eSUB_STATUS_I_REVERSAL = 65,

	//��ѹ��ƽ��--66
	eSUB_STATUS_V_UNBALANCE = 66,

	//������ƽ��--67
	eSUB_STATUS_I_UNBALANCE = 67,

	//������Դʧ��--68
	eSUB_STATUS_LOST_SECPOWER = 68,

	//����--69
	eSUB_STATUS_LOST_POWER = 69,		

	//��������--70
	eSUB_STATUS_DEMAND_OVER = 70,

	//�ܹ�����������--71
	eSUB_STATUS_COS_OVER_ALL = 71,		

	//�������ز�ƽ��--72
	eSUB_STATUS_S_I_UNBALANCE = 72,

	//��������--73
	eSUB_STATUS_BACKPROPALL = 73,

	//ȫʧѹ--74
	eSUB_STATUS_LOST_ALL_V = 74,

	//����--75
	eSUB_STATUS_BIT75 = 75,

	//����--76
	eSUB_STATUS_BIT76 = 76,
#if( SEL_TOPOLOGY == YES )
    //���������ź�ʶ�����¼�--77
	eSUB_STATUS_TOPO_IDENTI_RESULT= 77,
#else
	//����--77
	eSUB_STATUS_BIT77 = 77,
#endif

	//����--78
	eSUB_STATUS_BIT78 = 78,

	//����--79
	eSUB_STATUS_BIT79 = 79,

	//���--80
	eSUB_STATUS_PRG_RECORD = 80,

	//�������--81	
	eSUB_STATUS_PRG_CLEAR_METER = 81,			

	//��������--82
	eSUB_STATUS_PRG_CLEAR_MD = 82,				

	//�¼�����--83
	eSUB_STATUS_PRG_CLEAR_EVENT = 83,		

	//Уʱ--84
	eSUB_STATUS_PRG_ADJUST_TIME = 84,			

	//ʱ�α���--85
	eSUB_STATUS_PRG_TIME_TABLE = 85,		

	//ʱ������--86
	eSUB_STATUS_PRG_TIME_AREA = 86,		

	//�����ձ��--87
	eSUB_STATUS_PRG_WEEKEND = 87,			

	//�ڼ��ձ��--88
	eSUB_STATUS_PRG_HOLIDAY = 88,		

	//�й���Ϸ�ʽ���--89
	eSUB_STATUS_PRG_P_COMBO = 89,			

	//�޹���Ϸ�ʽ1���--90
	eSUB_STATUS_PRG_Q_COMBO = 90,					

	//�޹���Ϸ�ʽ2���--91
	eSUB_STATUS_PRG_Q2_COMBO = 91,				

	//�����ձ��--92
	eSUB_STATUS_PRG_CLOSING_DAY = 92,				

	//���ʲ������--93
	eSUB_STATUS_PRG_RATE = 93,		  

	//���ݱ���--94
	eSUB_STATUS_PRG_STEP = 94,           

	//��Կ����--95
	eSUB_STATUS_PRG_UPDATE_KEY = 95,	

	eREPORT_STATUS_TOTAL,
	//ռλ
	eSUB_STATUS_NULL	//�¼���¼��Щ����״̬����û�У�������eSUB_STATUS_NULL��SubEventInfoTab�����ռλ
}eREPORT_STATUS_INDEX;

//�����ϱ�ģʽ��������
typedef enum
{
	//���ɿ����󶯻�ܶ�--0
	eSUB_MODE_RELAY_ERR = 0,					

	//ESAM����--1
	eSUB_MODE_ESAM_ERR = 1,					
	
	//�ڿ���ʼ��������洢�����ϻ��𻵺�����ͬ��--2
	eSUB_MODE_ROM_ERR = 2,					
	
	//ʱ�ӵ�ص�ѹ��--3
	eSUB_MODE_CLOCK_BAT_LOW = 3,
	
	//�ڲ��������--4
	eSUB_MODE_CODE_ERR = 4,

	//�洢�����ϻ���--5
	eSUB_MODE_MEM_ERR = 5,					

	//����--6
	eSUB_MODE_BIT6 = 6,
	
	//ʱ�ӹ���--7
	eSUB_MODE_RTC_ERR = 7,
	
	//ͣ�糭����Ƿѹ--8
	eSUB_MODE_READ_BAT_LOW = 8,			

	//͸֧״̬--9
	eSUB_MODE_OVER_DRAFT = 9,			
	
	//�����--10
	eSUB_MODE_METER_COVER = 10,
	
	//����ť��--11
	eSUB_MODE_BUTTON_COVER = 11,
	
	//�㶨�ų�����--12
	eSUB_MODE_MAGNETIC_INT = 12,

	//��Դ�쳣--13
	eSUB_MODE_POWER_ERR = 13,

	//��բ�ɹ�--14
	eSUB_MODE_PRG_OPEN_RELAY = 14,	

	//��բ�ɹ�--15
	eSUB_MODE_PRG_CLOSE_RELAY = 15,	

	//ʧѹ--16
	eSUB_MODE_LOSTV = 16,

	//Ƿѹ--17
	eSUB_MODE_WEAK_V = 17,

	//��ѹ--18
	eSUB_MODE_OVER_V = 18,

	//ʧ��--19
	eSUB_MODE_LOSTI = 19,

	//����--20
	eSUB_MODE_OVER_I = 20,

	//����--21
	eSUB_MODE_OVERLOAD = 21,

	//���ʷ���--22
	eSUB_MODE_POW_BACK = 22,
	
	//A�����--23
	eSUB_MODE_BREAK = 23,

	//A�����--24
	eSUB_MODE_BREAK_I = 24,

	//����--25
	eSUB_MODE_BIT25 = 25,

	//����--26
	eSUB_MODE_BIT26 = 26,

	//����--27
	eSUB_MODE_BIT27 = 27,
	
	//����--28
	eSUB_MODE_BIT28 = 28,
	
	//����--29
	eSUB_MODE_BIT29 = 29,
	
	//����--30
	eSUB_MODE_BIT30 = 30,
	
	//����--31
	eSUB_MODE_BIT31 = 31,

	//��ѹ������-32
	eSUB_MODE_V_REVERSAL = 32,

	//����������--33
	eSUB_MODE_I_REVERSAL = 33,

	//��ѹ��ƽ��--34
	eSUB_MODE_V_UNBALANCE = 34,

	//������ƽ��--35
	eSUB_MODE_I_UNBALANCE = 35,

	//������Դʧ��--36
	eSUB_MODE_LOST_SECPOWER = 36,

	//����--37
	eSUB_MODE_LOST_POWER = 37,		

	//��������--38
	eSUB_MODE_DEMAND_OVER = 38,

	//�ܹ�����������--39
	eSUB_MODE_COS_OVER_ALL = 39,		

	//�������ز�ƽ��--40
	eSUB_MODE_S_I_UNBALANCE = 40,

	//��������--41
	eSUB_MODE_BACKPROPALL = 41,

	//ȫʧѹ--42
	eSUB_MODE_LOST_ALL_V = 42,

	//����--43
	eSUB_MODE_BIT43 = 43,

	//����--44
	eSUB_MODE_BIT44 = 44,
    
#if( SEL_TOPOLOGY == YES )
    //���������ź�ʶ�����¼�--45
    eSUB_MODE_TOPO_IDENTI_RESULT = 45,
#else
	//����--45
	eSUB_MODE_BIT45 = 45,
#endif

	//����--46
	eSUB_MODE_BIT46 = 46,

	//����--47
	eSUB_MODE_BIT47 = 47,

	//���--48
	eSUB_MODE_PRG_RECORD = 48,

	//�������--49
	eSUB_MODE_PRG_CLEAR_METER = 49,			

	//��������--50
	eSUB_MODE_PRG_CLEAR_MD = 50,				

	//�¼�����--51
	eSUB_MODE_PRG_CLEAR_EVENT = 51,		

	//Уʱ--52
	eSUB_MODE_PRG_ADJUST_TIME = 52,			

	//ʱ�α���--53
	eSUB_MODE_PRG_TIME_TABLE = 53,		

	//ʱ������--54
	eSUB_MODE_PRG_TIME_AREA = 54,		

	//�����ձ��--55
	eSUB_MODE_PRG_WEEKEND = 55,			

	//�ڼ��ձ��--56
	eSUB_MODE_PRG_HOLIDAY = 56,		

	//�й���Ϸ�ʽ���--57
	eSUB_MODE_PRG_P_COMBO = 57,			

	//�޹���Ϸ�ʽ1���--58
	eSUB_MODE_PRG_Q_COMBO = 58,					

	//�޹���Ϸ�ʽ2���--59
	eSUB_MODE_PRG_Q2_COMBO = 59,				

	//�����ձ��--60
	eSUB_MODE_PRG_CLOSING_DAY = 60,				

	//���ʲ������--61
	eSUB_MODE_PRG_RATE = 61,		  

	//���ݱ���--62
	eSUB_MODE_PRG_STEP = 62,           

	//��Կ����--63
	eSUB_MODE_PRG_UPDATE_KEY = 63,	

	eREPORT_MODE_TOTAL,
}eREPORT_MODE_INDEX;

//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

#pragma pack(1)	//���ļ������нṹ���1�ֽڶ���

typedef struct TReport645Table_t
{
	BYTE    Event_SubIndex;	//�¼�subö��
	BYTE	Status_Index;	//�����ϱ�״̬��ö��
	BYTE	Event_Index;	//�¼�ö��
	BYTE	Mode_Index;	    //�����ϱ�ģʽ��ö��
}TReport645Table;

//�����ϱ��ṹ��
typedef struct TReport645_t
{
    BYTE BakFlag[MAX_COM_CHANNEL_NUM];                           //��λ��־ 0x00���ϱ� 0x55�ϱ��Ѷ���
	BYTE StatusBak[MAX_COM_CHANNEL_NUM][RPT_STATUS_LEN];		 //�����ϱ�״̬�ֱ���
    BYTE ResetTime[MAX_COM_CHANNEL_NUM];                        //��λ�����ϱ���ʱ��ʵʱ��
	DWORD CRC32;
}TReport645;

typedef struct TReport645ResetTime_t
{
    BYTE  Time;                                        //��λ�����ϱ���ʱ���̶���
	DWORD CRC32;
}TReport645ResetTime;


typedef struct TReportTimes645_t
{
   BYTE Times[MAX_COM_CHANNEL_NUM][SUB_EVENT_INDEX_MAX_NUM];    //�����ϱ�״̬����������
   DWORD CRC32;
}TReportTimes645;

#pragma pack()	
//-----------------------------------------------
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ���ϱ�����
//
//����:      BYTE Mode[in]        0x00:��ָ���¼�
//                              0x55:ȫ��
//         BYTE SubEventIndex   ���Ӵ������¼�����
//
//����ֵ:		Result		- TRUE������ 		FALSE��������
//
//��ע:
//---------------------------------------------------------------
WORD  ClearReport645Times( BYTE Ch, BYTE Mode, BYTE SubEventIndex );

//-----------------------------------------------
//��������: �����ϱ�����
//
//����:      BYTE Ch[in]          ͨ��ѡ��
//
//         BYTE Mode[in]        0x00:�����ۼƼ�1
//                              0x55:��������Ϊ1
//
//         BYTE SubEventIndex   ���Ӵ������¼�����
//
//����ֵ:		Result		- TRUE������ 		FALSE��������
//
//��ע:
//---------------------------------------------------------------
WORD  api_AddReportTimes( BYTE Ch, BYTE Mode, BYTE SubEventIndex);

//-----------------------------------------------
//��������:      ���������ϱ���λʱ��
//
//����:        BYTE ResetTime[in]
//
//����ֵ:		
//
//��ע:
//---------------------------------------------------------------
WORD  api_SetReportResetTime( BYTE ResetTime );

//-----------------------------------------------
//��������:      ��ȡ�����ϱ���λʱ��
//
//����:        BYTE Time[Out]
//
//����ֵ:		
//
//��ע:
//---------------------------------------------------------------
BYTE  api_GetReportResetTime( void );

//-----------------------------------------------
//��������: �����ϱ�״̬�ֶ�ȡ
//
//����:		BYTE Ch[in]  - ͨ��ѡ��
//          Type[in]	 - ��ȡ��ʽ eREAD_STATUS_PROC--��Լ��ȡ	         	eREAD_STATUS_OTHER--�ǹ�Լ��ȡ
//			pBuf[out]	 - �����ϱ�״̬�ֻ���
//����ֵ:		ReturnLen	 - ���ݳ���		0������
//
//��ע:
//---------------------------------------------------------------
WORD api_ReadReportStatusByte( BYTE Ch, eREAD_STATUS_TYPE Type, BYTE *pBuf	);

//-----------------------------------------------
//��������:     �����ϱ�״̬�ֶ�ȡ
//
//����:	    BYTE *pBuf[in]  ����buf
//
//��ע:
//---------------------------------------------------------------
WORD api_ReadReportStatusMode( BYTE *pBuf	);


//-----------------------------------------------
//��������: ��ȡ�����ϱ�״̬���Ƿ����ú���֡
//
//����:      BYTE Ch[in] ͨ��
//			
//
//����ֵ:	
//
//��ע:		
//---------------------------------------------------------------
WORD api_GetReportReqFlag(          BYTE Ch );

//-----------------------------------------------
//��������: �����ϱ�״̬�ָ�λ
//
//����:		Ch[in]      - ͨ��ѡ��
//          pBuf[in]	- ��λ����
//
//����ֵ:		Result	- TRUE����ȷ		FALSE������
//
//��ע:
//---------------------------------------------------------------
WORD api_ResetReportStatusByte(          BYTE Ch, BYTE *pBuf );

//-----------------------------------------------
//��������: �����͹���ʱ��EEPROM�ָ������ϱ�״̬�֡�������������־λ����λ����ʱ�����ϱ�־λ
//
//����:		��
//			
//����ֵ:		��
//
//��ע:
//---------------------------------------------------------------
void api_PowerUpReport645( void );

//-----------------------------------------------
//��������: ���籣����ϱ�־
//
//����:		Type[in]	-
//			Index[in]	-
//����ֵ:		��
//
//��ע:
//---------------------------------------------------------------
void api_PowerDownReport645( void );

//-----------------------------------------------
//��������:     ��ʼ�������ϱ��������������
//
//����:		��
//			
//����ֵ:		��
//
//��ע:
//---------------------------------------------------------------
void api_FactoryInitReport645( void );

//-----------------------------------------------
//��������: �����ϱ���������
//
//����:		���ޣ�
//
//����ֵ:		���ޣ�
//
//��ע:		״̬��ȫΪ0ʱ��EVENTOUT�������̬����������
//---------------------------------------------------------------
void api_Report645MinTask( void );


#endif  //#if( SEL_DLT645_2007 == YES )
#endif	//#ifndef __REPORT645_API_H
