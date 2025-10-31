#ifndef __CONFIG_MEASURING_SWITCH_H
#define __CONFIG_MEASURING_SWITCH_H
//------------------------------------------------------------------------------------------
//	��    �ƣ� config_Dlt645_2007.h
//	��    ��:  ���� DL/T645-2007 �ĵ��������ͷ�ļ���
//	��    ��:  κ����
//	����ʱ��:  2009-7-15
//	����ʱ��:
//	��    ע:	���ļ���ֻ���к궨�壬�����нṹ
//	�޸ļ�¼:
//------------------------------------------------------------------------------------------
//��Ҫ���¼���¼����


#if( MAX_PHASE == 3 )
	//�Ƿ�ѡ���¹�Լ DL/T645-2007
	#define SEL_DLT645_2007 					YES
	//�Ƿ�ѡ���¹�Լ DL/T698.45-2016
	//�Ƿ�ѡ��Modbus��Լ
	#define SEL_PRO_MODBUS						NO
	//�Ƿ�֧��г������
	#define SEL_MEASURE_HARMONIC				NO
	//�Ƿ�֧��698Э�飨�ز�����
	#define  SEL_DLT698_2016_FUNC				YES
	//�Ƿ��¼��DLT645Ҫ������ڵ��ܣ�ֻΪDL/T645-2007����
	#define SEL_APPARENT_ENERGY					YES
	//�Ƿ�֧���޹�����
	#define SEL_RACTIVE_ENERGY					YES
	//�Ƿ�֧�ַ������ DL/T645_1997 Ҳ���򿪹�
	#define SEL_SEPARATE_ENERGY					YES
	//�Ƿ�֧�ַ�����ʵ���
	#define SEL_SEPARATE_RATIO_ENERGY			NO
	//�Ƿ�ѡ������޹�����   ���ѡ�񲻼����޹����� ��Ҳ�������������޹�����
	//���ѡ������޹����������������޹�����Ҳ����
	#define SEL_REACTIVE_DEMAND					YES
	//�Ƿ�ѡ�������������  ��SEL_APPARENT_ENERGY�޹أ����ڹ���ʲôʱ��֧��
	#define	SEL_APPARENT_DEMAND					NO
	//�Ƿ�֧�ֶ����������������
	#define	SEL_FREEZE_PERIOD_DEMAND			NO
	//�Ƿ�֧�ְ�ʱ���㹦��
	#define SEL_AHOUR_FUNC						NO

	//�洢�Ƿ�֧��Ӳ������
	#define SEL_MEMORY_PROTECT					NO
	//�Ƿ�֧��������ɶ�ʱ���������壨��������岻����CPU�������޷�ѡ��YES��
	#define SEL_TIMER_PLUSE_FUNC				NO

	//----------------------------------------------------
	//���ݿ����ֵ����
	//----------------------------------------------------
	//ѡ�����ݿ��е���С��λ�������ѡ��С����λ��̫�����ݿ⽫���������洢
	#define SYS_DBASE_ENERGY_DOT		0xff//���ݿⰴ�������洢
	//����¼����
	#define MAX_MONTH					13
	#define MAX_MONTH_FOR_BACKUP		3	//�������ݱ�������
	//��������
	#define MAX_RATIO					5
	//���GPRS��DL/T645_2007 Ҫ��10������
	#define MAX_645PASSWORD_LEVEL		5//645���뼶��
	//���볤��
	#define MAX_645PASSWORD_LENGTH		4//645���볤��
	//���볤��
	#define MAX_698PASSWORD_LENGTH		8////698���볤��(���볤��+ ����)
	//����������
	#define MAX_PASSWORD_ERR_TIMES		0x03
	//���ʱ����
	#define MAX_TIME_AREA				14
	//���ʱ�α���
	#define MAX_TIME_SEG_TABLE			8
	//ÿ��ʱ�α����ʱ����
	#define MAX_TIME_SEG				14
	//��󹫹�������
	#define MAX_HOLIDAY					20//����20��Ҫ�����뻺��Ĵ�С
	//�ȸ������й���Ҫ����8���ֽڱ����쳣�����ֽ�
	#define MAX_WARN_ERR_NUM			8
	//DL/T645-2007Ҫ��ÿ�����3�ν��㣬��ÿ�������ն�����ʱ
	#define MAX_MON_CLOSING_NUM			3 
	
	#define MAX_SAFE_MODE_PARA_NUM		60	//���ܳ���62��

	//----------------------------------------------------
	// �¼���¼ѡ��
	//--------------------------------------
	// �Ƿ�ѡ�������ϱ�
	#define SEL_AUTO_WARN					NO
	// ������Դʧ��
	#define SEL_EVENT_LOST_SECPOWER			NO//���඼�򿪣�����Ҫ�˹�����cSEL_SECPOWER����
	// ʧѹ��¼
	#define SEL_EVENT_LOST_V				NO
	// Ƿѹ��¼
	#define SEL_EVENT_WEAK_V				NO
	// ��ѹ��¼
	#define SEL_EVENT_OVER_V				NO
	// �����¼
	#define SEL_EVENT_BREAK					NO
	// ��ѹ�������¼
	#define SEL_EVENT_V_REVERSAL			NO
	// �����������¼
	#define SEL_EVENT_I_REVERSAL			NO
	// ��ѹ��ƽ���¼
	#define SEL_EVENT_V_UNBALANCE			NO
	// ������ƽ���¼
	#define SEL_EVENT_I_UNBALANCE			NO
	// ʧ����¼
	#define SEL_EVENT_LOST_I				NO
	// ������¼
	#define SEL_EVENT_OVER_I				NO
	// ������¼
	#define SEL_EVENT_BREAK_I				NO
	// ���� ���ʷ���
	#define SEL_EVENT_BACKPROP				NO
	// ���ؼ�¼
	#define SEL_EVENT_OVERLOAD				NO
	// �����¼
	#define SEL_EVENT_LOST_POWER			NO
	// ����Ǽ�¼
	#define SEL_EVENT_METERCOVER     		NO
	// ����ť�м�¼
	#define SEL_EVENT_BUTTONCOVER			NO
	// ���๦���������޼�¼
	#define SEL_EVENT_COS_OVER				NO
	// �������ز�ƽ���¼
	#define SEL_EVENT_I_S_UNBALANCE			NO
	// ���������ߵ�����ƽ���¼
	#define SEL_EVENT_SP_I_UNBALANCE		NO
	// �㶨�ų����ż�¼
	#define SEL_EVENT_MAGNETIC_INT			NO
	// ���ɿ����󶯼�¼
	#define SEL_EVENT_RELAY_ERR				NO
	// ��Դ�쳣��¼
	#define SEL_EVENT_POWER_ERR				NO
	//ȫʧѹ��¼
	#define SEL_EVENT_LOST_ALL_V			NO
	//ʱ�ӹ���
	#define SEL_EVENT_RTC_ERR				NO
	//����оƬ����
	#define SEL_EVENT_SAMPLECHIP_ERR		NO
	//�����Ե��¼�
	#define SEL_EVENT_STEAL_ELECTRICITY		YES	
	//�õ��쳣���ʶ��
	#define SEL_EVENT_ELECTRICITY_ERR		YES	
	//------------------------------------------------------------------------------------
	// ��̼�¼ѡ��  ��12��
	//--------------------------------------
	// ��������¼
	#define SEL_PRG_INFO_CLEAR_METER			NO
	// ��������¼
	#define SEL_PRG_INFO_CLEAR_MD				NO
	// �¼������¼
	#define SEL_PRG_INFO_CLEAR_EVENT			NO
	// Уʱ��¼
	#define SEL_PRG_INFO_ADJUST_TIME			NO
	// ʱ�α��̼�¼
	#define SEL_PRG_INFO_TIME_TABLE				NO
	// ʱ�����̼�¼
	#define SEL_PRG_INFO_TIME_AREA				NO
	// �����ձ�̼�¼
	#define SEL_PRG_INFO_WEEK					NO
	// �ڼ��ձ�̼�¼
	#define SEL_PRG_INFO_HOLIDAY				NO
	// �й���Ϸ�ʽ��̼�¼
	#define SEL_PRG_INFO_P_COMBO				NO
	// �޹���Ϸ�ʽ��̼�¼
	#define SEL_PRG_INFO_Q_COMBO				NO
	// �����ձ�̼�¼
	#define SEL_PRG_INFO_CLOSING_DAY			NO
	// ��Կ���¼�¼
	#define SEL_PRG_UPDATE_KEY					NO	
	// 645��̼�¼����¼���10�����ݱ�ʶ��
	#define SEL_PRG_RECORD645					NO

	//------------------------------------------------------------------------------------------------------------
	// ��չ�¼�����ѡ��  ��6��
	//------------------------------------------------------------------------------------------------------------
	// �����¼
	#define SEL_EVENTEXT_BUYELEC				NO
	// �Ƿ��忨��¼
	#define SEL_EVENTEXT_ILLEGALCARD			NO

	//------------------------------------------------------------------------------------
	// ͳ�Ƽ�¼ѡ��  ��2��
	//--------------------------------------
	// ��ѹ�ϸ��ʼ�¼
	#define SEL_STAT_V_RUN						YES
	// �������޼�¼
	#define SEL_DEMAND_OVER						NO
	
#endif//#if( MAX_PHASE == 3 )

#endif//#ifndef __CONFIG_MEASURING_SWITCH_H