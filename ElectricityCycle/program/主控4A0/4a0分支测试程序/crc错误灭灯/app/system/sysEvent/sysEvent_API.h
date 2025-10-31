//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.9
//����		�����¼��쳣�¼�Apiͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __SYSEVENT_API_H
#define __SYSEVENT_API_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
//����ʱ���¼����
#define MAX_FREE_EVENT      128
//����¼�������쳣�¼�
#define MAX_SYS_FREE_MSG	256
//����¼��ϵͳ�쳣��Ϣ
#define MAX_SYS_UN_MSG		64



// �����¼���¼
//ϵͳ�����¼�
#define EVENT_SYS_START					1
//ϵͳ���磬����͹���
#define EVENT_ENTER_LOW_POWER			2
//ϵͳ�ϵ��ѹ�¼�
#define EVENT_SYS_START_VOLTAGE			3

//ϵͳ�����˳�ʼ������
//����ܲ����ɹ�
//#define EVENT_CLR_ENERGY_SUCCEED		4
//����ܲ���ʧ��
//#define EVENT_CLR_ENERGY_FALSE		5
//���������Ĳ���
//#define EVENT_CLR_DEMAND				6
//���޸�����Ĳ���
#define EVENT_CHG_PSW					7
//���޸�ѭ�Բ���
//#define EVENT_SET_LOOP_DISPLAY		8
//���޸ļ��Բ���
//#define EVENT_SET_KEY_DISPLAY			9
//�о��������Ĳ���
#define EVENT_PRECISION_CORRECT			10
//�������С��ѹ
//#define EVENT_CLR_MAXMIN_VOL			11
//�������С��������
//#define EVENT_CLR_MAXMIN_COS			12
//���ù�������������
//#define EVENT_COS_LEVEL				13
//�幦�������ϸ���
//#define EVENT_CLR_COS_TIME			14
//���ѹ��¼
//#define EVENT_CLR_OVER_VOL			15
//�������ƽ����
//#define EVENT_CLR_CURRENT_UN			16
//д��ʱ������ʱ�α���������������ʱ����������������
//#define EVENT_TIMETABLE_1				17
//д��ʱ������
//#define EVENT_TIMETABLE_2				18
//д��ʱ�α�����
//#define EVENT_TIMETABLE_3				19
//д��������ʱ�α��
//#define EVENT_WEEKEND_NO				20
//д�빫�����ա�������ʱ�α��
//#define EVENT_HOLIDAY_STATUS			21
//д���������ã������������ơ�LCD�������Զ������ա����ϴ����յȡ�
//#define EVENT_OTHER_SET				22
//д������״̬��
//#define EVENT_WEEKEND_STATUS			23
//д����
//#define EVENT_METER_NO_ONLY			24
//д���š��û��š��豸��
//#define EVENT_METER_NO				25
//д��ѹ�ϸ����
//#define EVENT_WRITE_VOL_LIMIT			26
//д���޹����Ʒ�ʽ����ѹ���ޡ���ѹ��㡢������ƽ��
//#define EVENT_WRITE_OTHER_CONTROL		27
//������ֱ����ѹƫ��ϵ���Ĳ���
#define EVENT_ADJUST_DC_VOL_OFF			28
//������ֱ������ƫ��ϵ���Ĳ���
#define EVENT_ADJUST_DC_CUR_OFF			29
//������������ѹƫ��ϵ���Ĳ���
#define EVENT_ADJUST_AC_VOL_OFF			30
//��������������ƫ��ϵ���Ĳ���
#define EVENT_ADJUST_AC_CUR_OFF			31
//��������ѹ����Ĳ���
#define EVENT_ADJUST_VOL_GAIN			32
//��������������Ĳ���
#define EVENT_ADJUST_CUR_GAIN			33
//�е�ѹ΢���Ĳ���
#define EVENT_ADJUST_VOL_SMALL			34
//��������ǵĲ���
#define EVENT_ADJUST_ANGLE				35
//����ʱ��
//#define EVENT_CLEAR_BATTERY_TIME		36
//���ѹ�ϸ���
//#define EVENT_CLR_VOL_TIME			37
//������¼
//#define EVENT_CLR_BREAK				38
//��ʧѹ��¼
//#define EVENT_CLR_LOST_V				39
//ϵͳ��ʼ����ʼ
#define EVENT_SYS_INIT_BEGIN			40

//��Լ����¼���¼
//��Լ�б�׼��̣����ϱ�׼��Լ��
//#define EVENT_PROTOCOL_STD_WRITE		41
//��Լ�б�̸��Ĳ�����
//#define EVENT_PROTOCOL_CHG_BPS		42
//��Լ�б���޸�����
#define EVENT_PROTOCOL_CHG_PSW			43
//��Լ�б�������
//#define EVENT_PROTOCOL_CLR_ENG		44
//��Լ�б��������¼
//#define EVENT_PROTOCOL_CLR_BREAK		45
//��Լ�б������ѭ��
//#define EVENT_PROTOCOL_SET_LOOP_DIS	46
//��Լ�б�����ü���
//#define EVENT_PROTOCOL_SET_KEY_DIS	47
//��Լ�б�̾�������
#define EVENT_PROTOCOL_PRECISION		48
//��Լ�б������ϵͳ����λ
#define EVENT_PROTOCOL_SET_SYS_BIT		49
//��ʧ����¼
//#define EVENT_CLR_LOST_I				50
//д�㲥ʱ��ɹ�
#define EVENT_BROADCAST_WRITE_TIME		51
//дʱ��оƬ
#define EVENT_WRITE_HARD_TIME			52//0x34
//���λ
#define EVENT_COMMAND_RESET				53
//�쳣��λ�����ϵ縴λ��
#define EVENT_UNNORMAL_RESET			54
//����ͨ�ŵ�ַ 2010-2-24
#define EVENT_SET_COMM_ADDR				55
//��Զ�̱�������
#define EVENT_SET_WARN_RELAY			56
//�н��Զ�̱�������
#define EVENT_CLR_WARN_RELAY			57
//�б������
#define EVENT_SET_SAFETY				58
//�н���������
#define EVENT_CLR_SAFETY				59
//MAC���󳬣�����
#define EVENT_MAC_ERR_LOCK				60
//�ϵ��esam�лָ����,��Կ״̬
#define EVENT_RESTORE_INFO_FROM_ESAM	61
//�򿪳���ģʽ
#define EVENT_OPEN_FAC_MODE				62
//ʵ�����÷�����С��ʱ�η�����
#define EVENT_TIME_TABLE_NUM_ERR		63
//��������������
#define EVENT_PROGRAMKEY_LOCK			64

//698�в������ò���
#define EVENT_PROGRAM_698				65//0x41
//698�з�������
#define EVENT_METHOD_698				66//0x42
//698�����ݳ�ʼ������
#define EVENT_DATA_INIT_698				67//0x43
//698�лָ�������������
#define EVENT_PARA_INIT_698				68//0x44
//698�����¼�����
#define EVENT_EVENT_INIT_698			69//0x45
//698������������
#define EVENT_DEMAND_INIT_698			70//0x46
//��У�����
#define EVENT_ADJUST_METER				71//0x47
//�����üĴ�����У��ϵ������
#define EVENT_WRITE_REG					72//0x48
//���ڹ�Լ��չд����
#define EVENT_FACTORY_WRITE				73//0x49
//Զ�̵̼�������ʧ��
#define EVENT_RELAY_ERR					74//0x4A

//ϵͳ�쳣��Ϣ��¼
//���λΪ 00(0)		ΪSetError�洢���쳣�¼�  ���Ϊ�����ж���Ĵ���ţ�����ʾһ��
//���λΪ 01(4)		����
//���λΪ 10(8)		��ͨ���쳣�¼�
//���λΪ 11(c)		Ϊ���Դ洢���쳣�¼�  ���Ϊ�������ڵ��к�

//���޸�ʱ��ʱ�α�Ĳ���
#define SYSUN_FIX_TIME_TABLE			0x8001
//�д�EEPROM1�лָ��������Ĳ���
#define SYSUN_PARA_READ_EEPROM1			0x8002
//�д�EEPROM2�лָ��������Ĳ���
#define SYSUN_PARA_READ_EEPROM2			0x8003
//ʹ���˵��ܱ���������һ�ζ����ܲ��ɹ���
#define SYSUN_USE_ENERGY_BACK			0x8004
//ת��д��ʱ����鵽RAM���ƻ�
#define SYSUN_SWAP_BUF_ERR				0x8005
//ת��ʱд���һ����������
#define SYSUN_SWAP_PART1_1				0x8006
//ת��ʱд���һ����������
#define SYSUN_SWAP_PART1_2				0x8007
//ת�����ǰ��鵽RAM���ݿⱻ�ƻ�
#define SYSUN_SWAP_E_RAMDBASE_ERR		0x8008
//ת��ǰ��鵽EEPROM���ݿ��𻵣���ת��
#define SYSUN_SWAP_EEPROM_ERR			0x8009
//ת��ʱ��鵽�ݴ�RAM���ƻ������ԣ�
#define SYSUN_SWAP_MID_RAM_ERR			0x800a
//ת��ǰ��鵽5460����
#define SYSUN_SWAP_5460_ERR				0x800b
//д��EEPROMǰ��⵽�����߼���������
#define SYSUN_EEPROM_PROTECT_11			0x800c
//д��EEPROMǰ��⵽�ؼ������߼���������
#define SYSUN_EEPROM_PROTECT_33			0x800d
//д��EEPROMʱ��������
#define SYSUN_EEPROM_REPET_WRITE		0x800e
//д��FLASHʱ��������
#define SYSUN_FALSE_REPET_ERR			0x800f
//����ʱ��EEPROM����������
#define SYSUN_READ_REMANDER_ERR			0x8010
//����ϵ��ʱ����ѹ΢��������Χ
#define SYSUN_ADJUST_VOL				0x8011
//ת������ǰ��鵽RAM���ݿⱻ�ƻ�
#define SYSUN_SWAP_D_RAMDBASE_ERR		0x8012
//������ȷ�Լ��ʱ����⵽FSTA2
#define SYSUN_CHECK_ENERGY_FSTA2		0x8013
//�޸Ĺ�5460����
#define MODIFIED_5460_PARA				0x8014
//�ֶ�����������ͽǶ�
//#define MANUAL_MODIFIED_PARA			0x8015
//�޸�����һƬ��5460����
#define REPAIRED_FIRST_5460_PARA		0x8016
//�޸����ڶ�Ƭ��5460����
#define REPAIRED_SECOND_5460_PARA		0x8017
//7022�ڲ�����У��͸ı���
#define CHECKSUM_CHANGE_7022			0x8018
//д���ݳ��������ռ��ַ
#define OVERWRITE_CONTINUE_SPACE		0x8019
//�ڴ����ָ��ͱ��ݵĲ�һ��
#define ALLOCBUF_POINT_ERROR			0x801a
//��ʼ������оƬʧ�ܹ�
#define INIT_SAMPLE_CHIP_FAIL			0x801b
//���ζ�7026E����ʱ���ֶ����������
#define EVENT_7026E_PULS_ERR			0x8021
//��Լ����յ�У�����ֻ�������м�¼�Թ��պ������Ų�
#define EVENT_CALIBRATE					0x8022
//У׼��������
#define SAMPLE_CALI_PARA_ERR			0x8023
//���ܼ�鷢��RAMУ�����
#define ENERGY_CHECK_RAM_ERR			0x8024
//��鵽�ڴ���ܳ���������
#define WATCH_ENERGY_OVERRIDE_ERR		0x8025
//���ڷ�����ת�����
#define WATCH_ENERGY_SWAP_ERR			0x8026
//ESAM��һ�θ�λʧ��
#define ESAM_FIRST_RESET_ERR			0x8027
//д��EEPROMǰ��⵽����߼���������
#define SYSUN_EEPROM_PROTECT_44			0x8028
//��鵽�ڴ���У�����
#define  WATCH_MONEY_RAM_ERR			0x8029
//��������ESAM�ָ��¼�
#define SYSUN_FIX_MONEY					0x802A
//�����ϵ��޸�RTCʱ��
#define SYSUN_POWERON_FIX_TIME			0x802B
//�����л�ʱ��CRCУ����������
#define SWITCH_TIME_CRC_ERR				0x802C
//��������CRCУ�����
#define GET_FREEZE_RECORD_ERR			0x802D
//sysytick ������
#define SYSERR_SYSTICK_ERR				0x802E
//���Ź���λ
#define SYSERR_WDTRST_ERR				0x802F
//����EEͨ��FLASH�ָ�
#define SYSUN_WRITE_ENERGY_EE			0x8030
//����EEͨ��FLASH�ָ�
#define SYSUN_WRITE_FREEZE_EE			0x8031
//tf�������������������ݶ�ʧ
#define SYSUN_TF_BUF_FULL				0x8032
//�ղ�������crc����
#define SYSUN_WAVE_DATA1_CRC_ERR			0x8033
#define SYSUN_WAVE_DATA2_CRC_ERR			0x8034
#define SYSUN_WAVE_DATA3_CRC_ERR			0x8035
#define SYSUN_WAVE_DATA4_CRC_ERR			0x8036

//д����оƬ�Ĵ���8100+�Ĵ�����ַ(һ���ֽ�)1
#define MANUAL_MODIFIED_PARA			0x8100
//flashд�����
#define SYSUN_FALSE_WRITE_ERR			0x2000 //bit0-bit13��¼������
//ESAM��ȡ�����ʧ��
#define ESAM_GET_RN_ERR				    0x8800
//ESAM��֤MACʧ��
#define ESAM_VERIFY_MAC_ERR				0x8801
//���ɷ���δ֪���ܱ��¼������쳣
#define FIND_UNKNOWN_METER_EVENT_ERR    0x8900

//hardfault����NMI�����жϲ���Ӧ
#define SYSERR_SCB_INFO					0x4000 //bit0-bit11��¼�쳣��Ϣ
//CR��ʱ
#define SYSERR_CRTIME_ERR				0x8CFF
//assert�쳣�¼� ��Ϊ0xc000+�к�
#define ASSERT_EVENT					0xc000


//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//�����¼���¼��ʽ
typedef struct TFreeEvent_t
{
	//�¼�����
	WORD EventType;
	//���Ӽ�¼
	WORD SubEvent;
	//����ʱ�䣬Ҫ��¼���꣺�£��գ�ʱ���֣��루5��4��3��2��1��0��
	BYTE EventTime[6];

}TFreeEvent;

//�¼���¼�������¼���¼
typedef struct TEventSave_t
{
	WORD FreeEventPtr;
	//����
	TFreeEvent FreeEvent[MAX_FREE_EVENT];
}TSysFreeEventConRom;

//�쳣�¼���¼��ʽ
typedef struct TSysUNMsg_t
{
	//�¼�����
	WORD EventType;
	//����ʱ�䣬Ҫ��¼���꣺�£��գ�ʱ���֣��루5��4��3��2��1��0��
	BYTE EventTime[6];

}TSysUNMsg;

typedef struct TSaveSysMsg_t
{
	WORD SysUnMsgPoint;
	TSysUNMsg SysUNMsg[MAX_SYS_UN_MSG];
}TSysAbrEventConRom;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern WORD		SysFREEMsgCounter;		//һ�������¼���¼��¼������
extern BYTE		SysUNMsgCounter;		//һ���쳣�¼���¼��¼������
//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��¼�����¼�
//
//����: 
//			EventType[in]		�����¼�����
//        	EventSub[in]		��������
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_WriteFreeEvent(WORD EventType, WORD EventSub);


//-----------------------------------------------
//��������: �������¼�
//
//����: 
//			Num[in]		��Num�������¼���¼
//          Buf[out]	�������	          
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
WORD api_ReadFreeEvent(WORD Num,BYTE *Buf);

//-----------------------------------------------
//��������: ��¼�쳣�¼�
//
//����: 
//			Msg[in]		�쳣�¼�����
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_WriteSysUNMsg(WORD Msg);


//-----------------------------------------------
//��������: ���쳣�¼�
//
//����: 
//			Num[in]		��Num���쳣�¼���¼
//          Buf[out]	�������	          
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
WORD api_ReadSysUNMsg(WORD Num,BYTE *Buf);


#endif//#ifndef __SYSEVENT_API_H

