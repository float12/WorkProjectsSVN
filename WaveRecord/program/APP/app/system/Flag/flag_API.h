//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.15
//����		����ϵͳ��־��Ӳ����־�������־
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __FLAG_API_H
#define __FLAG_API_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
//����������
//����Ҫ����0��ִ�еķ�������Ҫ�����ڲ�ͬ���룬ÿ��ģ��Ĵ���Ϊ��ID*3+3~ID*3+3+2����
typedef enum
{//							���	����䷶Χ
 	eTASK_SAMPLE_ID = 0,	//0		(3~5)s
	eTASK_LCD_ID,			//1		(6~8)s
	eTASK_PROTOCOL_ID,		//2		(9~11)s
	eTASK_LOADPROFILE_ID,	//3		(12~14)s
	eTASK_DEMAND_ID,		//4		(15~17)s
	eTASK_EVENTS_ID,		//5		(18~20)s
	eTASK_PREPAY_ID,		//6		(21~23)s
	eTASK_FREEZE_ID,		//7		(24~26)s
	eTASK_ENERGY_ID,		//8		(27~29)s
	eTASK_PARA_ID,			//9		(30~32)s
	eTASK_RTC_ID,			//10	(33~35)s
	eTASK_LOWPOWER_ID,		//11	(36~38)s
	eTASK_SYSWATCH_ID,		//12	(39~41)s
	eTASK_RELAY_ID,			//13	(42~44)s
	eTASK_MID_ID,			//14 (45~48)	
	
	////!!!!!!����������󣬲�ҪŲ��,��������Ϊ16��Ŀǰֻ������һ��WORD������Ҫ�ٳ�������!!!!!!
	eTASK_ID_NUM_T
	
}TTaskIDNum;

#define		MAX_TASK_ID_NUM		16		//��Ҫ�ģ�ȡ����TaskTimeFlag������


//ÿ�������־����
typedef enum
{
	//���ڸ��ٱ�־
	eFLAG_10_MS = 0,
	//500ms��־
	eFLAG_500_MS,
	eFLAG_SECOND,
	eFLAG_MINUTE,
	eFLAG_HOUR,
	eFLAG_TIME_CHANGE,			//ʱ��ı��־
	eFLAG_SWITCH_JUDGE,			//�����л�ʱ���־
	
	eFLAG_TIMER_T,////!!!!!!����������󣬲�ҪŲ��!!!!!!

}eTaskTimeFlag;

	

typedef enum
{
	//ϵͳ�Ƿ��ϵ�
	eSYS_STATUS_POWER_ON = 0,
	// �й����ʷ����ܣ�
	eSYS_STATUS_OPPOSITE_P,//�� A B Cһ��Ҫ��˳������
	// A���й����ʷ����ɲ���оƬ�жϣ�
	eSYS_STATUS_OPPOSITE_P_A,
	// B���й����ʷ����ɲ���оƬ�жϣ�
	eSYS_STATUS_OPPOSITE_P_B,
	// C���й����ʷ����ɲ���оƬ�жϣ�
	eSYS_STATUS_OPPOSITE_P_C,
	
	// �޹����ʷ����ܣ�
	eSYS_STATUS_OPPOSITE_Q,//�� A B Cһ��Ҫ��˳������
	// A���޹����ʷ����ɲ���оƬ�жϣ�
	eSYS_STATUS_OPPOSITE_Q_A,
	// B���޹����ʷ����ɲ���оƬ�жϣ�
	eSYS_STATUS_OPPOSITE_Q_B,
	// C���޹����ʷ����ɲ���оƬ�жϣ�
	eSYS_STATUS_OPPOSITE_Q_C,

	// ����������
	eSYS_STATUS_REVERSE_PHASE_I,
	
	// ���ƻ�·���� �̵�����������
	eSYS_STATUS_RELAY_ERR,
	
	//698��վ�������
	eSYS_STATUS_ID_698MASTER_AUTH,
	//698�ն˱������
	eSYS_STATUS_ID_698TERMINAL_AUTH,
	//698һ������������
	eSYS_STATUS_ID_698GENERAL_AUTH,
	//645�������
	eSYS_STATUS_ID_645AUTH,
	// ����������
	eSYS_IR_ALLOW_PRG,
	// �ն˵������֤
	eSYS_TERMINAL_ALLOW_PRG,
	// ͸֧״̬������͸֧�����󣬽���͸֧״̬��
	eSYS_STATUS_PRE_USE_MONEY,
	//����дʹ�ܿ���
	eSYS_STATUS_EN_WRITE_ENERGY,
	//���дʹ�ܿ���
	eSYS_STATUS_EN_WRITE_MONEY,
	//У��ϵ��дʹ�ܿ���
	eSYS_STATUS_EN_WRITE_SAMPLEPARA,
	//�͹��Ļ��ѱ�־
	eSYS_LOW_POWER_WAKEUP,	
	// ����ģʽ��,��ִ������ڲ�����,��1Ϊ���ڿɲ�����,��0Ϊ�������ɲ�����
	eSYS_STATUS_INSIDE_FACTORY,
	// �ͼ�����ģʽ��,��ִ�е͵ȼ�����,��1Ϊ���ڿɲ�����,��0Ϊ�������ɲ�����
	eSYS_STATUS_LOW_INSIDE_FACTORY,
	//�ϵ粻����������ݳ����־
	eSYS_STATUS_NO_INIT_DATA_ERR,
	//����оƬAD������־
	eSYS_STATUS_AD_START,
	#if( SEL_TOPOLOGY == YES )
	//�������˷��ͱ�־
	eSYS_STATUS_TOPOING,
	#endif
	// �����ڵ͹���ģʽ
	eSYS_STATUS_RUN_LOW_POWER,

	//!!!!!!����������󣬲�ҪŲ��!!!!!!
	eSYS_STATUS_NUM_T,
	
}eSysStatusFlag;

#define		MAX_SYS_STATUS_NUM		64		//����Ϊ8�ı���


typedef enum
{
	// ��Կ״̬������Ϊ˽Կ��
	eRUN_HARD_COMMON_KEY = 0,
	// �л���־��Ԥ��6����Ҫ��������TRUE���л�ʱ����Ч��FALSE�����л�ʱ����Ч
	//ʱ�����л���־
	eRUN_HARD_SWITCH_FLAG1,
	//ʱ�α��л���־
	eRUN_HARD_SWITCH_FLAG2,
	//���ʼ۸�������л���־
	eRUN_HARD_SWITCH_FLAG3,
	//���ݲ������л���־
	eRUN_HARD_SWITCH_FLAG4,
	//����ʱ�����л���־
	eRUN_HARD_SWITCH_FLAG5,
	//����
	eRUN_HARD_SWITCH_FLAG6,
	//MAC������
	eRUN_HARD_MAC_ERR_FLAG,
	// ESAM����
	eRUN_HARD_ESAM_ERR,
	//ʱ�ӵ�ص�ѹ��
	eRUN_HARD_CLOCK_BAT_LOW,
	//�����ص�ѹ��
	eRUN_HARD_READ_BAT_LOW,
	//�������״̬��0��δ����	1������
	eRUN_HARD_PASSWORD_ERR_STATUS,//698���������־
	eRUN_HARD_H_PASSWORD_ERR_STATUS,//02�����������־
	eRUN_HARD_L_PASSWORD_ERR_STATUS,//04�����������־
	//�㲥Уʱ��־
	eRUN_HARD_ALREADY_BROADCAST_FLAG,
	//���Ĺ㲥Уʱ���ϱ�־
	eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG,
	//ʱ�ӹ��ϱ�־_RTC����
	eRUN_HARD_ERR_RTC_FLAG,
	//ʱ�ӹ��ϱ�־_���Ĺ㲥Уʱ��������
	eRUN_HARD_BROADCAST_ERR_FLAG,
	#if( SEL_DLT645_2007 == YES )
	eRUN_HARD_645_FOLLOW_STATUS,//645���ú���֡��־       YES:����   NO:������ �������������1��Ӧ������2���ط��ֱ�Ϊlcd��ʾ�ͼ̵�������
	#endif
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//��ǰ����ģʽ FALSE���� TRUE����
	eRUN_HARD_YEAR_LADDER_MODE,	//��ģʽ
	eRUN_HARD_MON_LADDER_MODE,	//��ģʽ
	//͸֧��־
	eRUN_HARD_TICK_FLAG,
	#endif
	//�����֤����ʹ�� ==TRUE; �����֤���ã����뽨��Ӧ�����Ӳſ���ͨ������+RN��������
	eRUN_HARD_EN_IDENTAUTH,
	//�Ƿ���������ϱ��ϱ���־��=TRUE���ϵ糬��1Сʱ��
	eRUN_HARD_POWERDOWN_REPORT_FLAG,

	eRUN_HARD_NUM_T,
		
}eRunHardFlag;//����С�� MAX_RUN_HARD_FLAG_NUM

#define		MAX_RUN_HARD_FLAG_NUM		64		//����Ϊ8�ı���

//������ʱ���⼸����־��Ҫ��飡��������������ʼ��ʱ������Щ��־
typedef enum
{
	//�����Ƿ��������������ʽ��YES����ı�ʱ������������������
	ePRO_HARD_DEMAND_CONTINUE_MEASURE,
	//�����Ƿ񲹳����
	ePRO_HARD_PHASE_HEAT,
	//Ҫ��ȡ���������λ�Ƿ�Ҫ��λ,YES������λ
	ePRO_HARD_DEMAND_DATA_CARRY,
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//���н��ݽ�������Ч������ҽ��ݵ�۷�������£����ݵ����ο۷ѣ�0--���۷ѣ�1-���½��ݿ۷Ѳ���ÿ�µ�1������ת������õ���
	ePRO_HARD_LADDER_SAVE_PARA_INVALID_DEAL,
	#endif
	ePRO_HARD_NUM_T,
		
}eProHardFlag;

#define		MAX_PRO_HARD_FLAG_NUM		32		//����Ϊ8�ı���


//!!!!!!Ϊ��֤����ǰ�Ĵ�����ʾһ�£���Ҫ���Ĵ����!!!!!!
//ϵͳ�쳣��Ϣ��¼,������������Ҫ��¼�쳣�¼� 
//���λΪ 00(0)		ΪSetError�洢���쳣�¼�  ���Ϊ�����ж���Ĵ���ţ�����ʾһ��
//���λΪ 01(4)		����
//���λΪ 10(8)		��ͨ���쳣�¼�
//���λΪ 11(c)		Ϊ���Դ洢���쳣�¼�  ���Ϊ�������ڵ��к�
//���������Ͳ��������0��
#define ERR_EEPROM_CHECKSUN					1
//EEPROMд��������Դ������������ܷ�ӳоƬ�ֲ��𻵣�ֻ�������𻵲Ż�˱�����
#define ERR_WRITE_EEPROM1					4
#define ERR_WRITE_EEPROM2					5
#define ERR_WRITE_EEPROM3					6
//��һ������������
#define ERR_FPARA1							17
//�ڶ�������������
#define ERR_FPARA2							18
//���������������� �澯�ʹ�������ֵ�
#define ERR_FPARA3							19
//ʱ��ʱ�α����
#define ERR_TIME_TABLE						20
//5460��������
#define ERR_PARA_CS5460						21
//����ʱ���EEPROM�еĵ���У�����
#define ERR_CHECK_ENERGY					22
//��ת�����
#define ERR_SWAP_MONTH						23
//ʱ��оƬ��
//#define ERR_RTC_READ						24
//5460�Լ������������������ڡ����ܸı�������������
#define ERR_CHECK_5460_1					25
#define ERR_CHECK_5460_2					26
#define ERR_CHECK_5460_3					27
#define ERR_SWAP_MONTH1						28
#define ERR_SWAP_MONTH2						29
// �޸ĵ��粻��ʧ��־����
#define ERR_SET_RUN_HARD_FLAG				33
#define ERR_SET_PRO_HARD_FLAG				34
// �������粻��ʧ��־����
#define ERR_GET_HARD_FLAG					35
#define ERR_GET_YFF_ENERGY					41
#define ERR_YFF_DEC_ERROR					42
#define ERR_GET_RATIO_PRICE					43		// �õ��������ʼ۸����
#define ERR_DO_REMAIN_ENERGY				44		// ����ʣ���ѵȲ�������
#define ERR_DO_GET_BUY_NUM					45		// ����ʣ���ѵȲ�������
#define ERR_GET_REMAIN_ENERGY				46		// ����ʣ���������
#define ERR_GET_PRICE						47
#define ERR_CPU_TYPE						48
#define ERR_WRITE_SAMPLE_PARA				49		// д��У���������
#define ERR_PULSCONST_TOO_BIG				50		// ���峣��̫��
#define ERR_PRICE_DATA						51		// ��۲�������
#define ERR_CYCLESINFO_DATA					52		// �����ѱ��������
#define ERR_TOPO_DATA					    53		// ���˲�������
#define ERR_MESSAGE_TRANS_DATA				54		// ����͸����������
//�����ʶ�롣���64�������ʶ
#define MAX_ERROR							64

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

// �洢���粻��ʧ���б�־�Ľṹ
typedef struct TRunHardFlag_t
{
	
	BYTE Flag[ MAX_RUN_HARD_FLAG_NUM / 8];
	DWORD CRC32;
	
}TRunHardFlagSafeRom;

// �洢���粻��ʧ��Լ���ñ�־�Ľṹ
typedef struct TProHardFlag_t
{
	
	BYTE Flag[ MAX_PRO_HARD_FLAG_NUM / 8];
	DWORD CRC32;
	
}TProHardFlagSafeRom;

typedef struct TFlagBytes_t
{
	//�����־���ݶ����16�����񣬼�ÿ��ʱ���־ռ2���ֽ�
	WORD TaskTimeFlag[eFLAG_TIMER_T];
	// ϵͳ״̬
	BYTE SysStatus[MAX_SYS_STATUS_NUM / 8];
	// �洢���粻��ʧ���б�־�Ľṹ
	TRunHardFlagSafeRom RunHardFlagSafeRom;
	// �洢���粻��ʧ��Լ���ñ�־�Ľṹ
	TProHardFlagSafeRom ProHardFlagSafeRom;
	//�����־
	BYTE ErrByte[(MAX_ERROR/8)];
	
}TFlagBytes;

typedef struct TNoInitDataFlag_t
{
	DWORD	Flag;
	DWORD	CRC32;
}TNoInitDataFlag;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern __no_init TNoInitDataFlag NoInitDataFlag;

//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ����ϵͳ״̬
//
//����: 
//			StatusNo[in]		ϵͳ״̬��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SetSysStatus(BYTE StatusNo);

//-----------------------------------------------
//��������: ���ϵͳ״̬
//
//����: 
//			StatusNo[in]		ϵͳ״̬��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ClrSysStatus(BYTE StatusNo);

//-----------------------------------------------
//��������: ��ȡϵͳ״̬
//
//����: 
//			StatusNo[in]		ϵͳ״̬��
//                    
//����ֵ:  	TRUE:ϵͳ״̬��λ�� FALSE:ϵͳ״̬δ��λ
//
//��ע:   
//-----------------------------------------------
BOOL api_GetSysStatus(BYTE StatusNo);

//-----------------------------------------------
//��������: ����ϵͳ���й̻���־
//
//����: 
//			FlagNo[in]		ϵͳ�̻���־��
//                    
//����ֵ:  	TRUE:���óɹ�	FALSE:����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_SetRunHardFlag(BYTE FlagNo);

//-----------------------------------------------
//��������: ���ϵͳ���й̻���־
//
//����: 
//			FlagNo[in]		ϵͳ�̻���־��
//                    
//����ֵ:  	TRUE:����ɹ�	FALSE:���ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_ClrRunHardFlag(BYTE FlagNo);

//-----------------------------------------------
//��������: ��ȡϵͳ���й̻���־
//
//����: 
//			FlagNo[in]		ϵͳ�̻���־��
//                    
//����ֵ:  	TRUE:��Ӧ�̻���־��λ��	FALSE:��Ӧ�̻���־δ��λ�����ݲ���
//
//��ע:   
//-----------------------------------------------
BOOL api_GetRunHardFlag(BYTE FlagNo);

//-----------------------------------------------
//��������: ��λϵͳ���й̻���־
//
//����: 	��
//                    
//����ֵ:  	TRUE:��ʼ���ɹ�	FALSE:��ʼ��ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_FactoryInitRunHardFlag(void);

//-----------------------------------------------
//��������: ����ϵͳ��Լ���ù̻���־
//
//����: 
//			FlagNo[in]		��Լ����ϵͳ�̻���־��
//                    
//����ֵ:  	TRUE:���óɹ�	FALSE:����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_SetProHardFlag(BYTE FlagNo);

//-----------------------------------------------
//��������: ���ϵͳ��Լ���ù̻���־
//
//����: 
//			FlagNo[in]		ϵͳ��Լ���ù̻���־��
//                    
//����ֵ:  	TRUE:����ɹ�	FALSE:���ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_ClrProHardFlag(BYTE FlagNo);

//-----------------------------------------------
//��������: ��ȡϵͳ��Լ���ù̻���־
//
//����: 
//			FlagNo[in]		ϵͳ��Լ���ù̻���־��
//                    
//����ֵ:  	TRUE:��Ӧ�̻���־��λ��	FALSE:��Ӧ�̻���־δ��λ�����ݲ���
//
//��ע:   
//-----------------------------------------------
BOOL api_GetProHardFlag(BYTE FlagNo);

//-----------------------------------------------
//��������: ��λϵͳ��Լ���ù̻���־
//
//����: 	��
//                    
//����ֵ:  	TRUE:��ʼ���ɹ�	FALSE:��ʼ��ʧ��
//
//��ע:   
//-----------------------------------------------
void api_FactoryInitProHardFlag(void);

//-----------------------------------------------
//��������: ����ָ���Ĵ���
//
//����: 
//			Err[in]		ָ���Ĵ����־��
//			BIT15		0  ������ϢҪд��EEPROM
//           			1  ������Ϣ��д��EEPROM
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SetError(WORD Err);

//-----------------------------------------------
//��������: ��鵱ǰ�Ƿ����ָ���Ĵ���
//
//����: 
//			Err[in]		ָ���Ĵ����־��
//                    
//����ֵ:  	TRUE:����ָ���Ĵ���  	FALSE:������ָ���Ĵ���
//
//��ע:   
//-----------------------------------------------
BOOL api_CheckError(WORD Err);

//-----------------------------------------------
//��������: ���ָ���Ĵ����־��
//
//����: 
//			Err[in]		ָ���Ĵ����־��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ClrError(WORD Err);

//-----------------------------------------------
//��������: ������������ͬһ����־
//
//����: 
//			FlagID[in]		��־��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SetAllTaskFlag( BYTE FlagID );

//-----------------------------------------------
//��������: �������־
//
//����: 
//			TaskID[in]		�����
//			FlagID[in]		��־��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SetTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//��������: ��ȡ�����־
//
//����: 
//			TaskID[in]		�����
//			FlagID[in]		��־��
//                    
//����ֵ:  	TRUE:��Ӧ��־��λ��  FALSE:��Ӧ��־δ��λ
//
//��ע:   
//-----------------------------------------------
BOOL api_GetTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//��������: ��������־
//
//����: 
//			TaskID[in]		�����
//			FlagID[in]		��־��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ClrTaskFlag( BYTE TaskID, BYTE FlagID );

//-----------------------------------------------
//��������: ��־�����ϵ紦��
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_PowerUpFlag( void );

//-----------------------------------------------
//��������: ��ʼ��_noinit_��־
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_InitNoInitFlag( void );

#endif//#ifndef __FLAG_API_H
