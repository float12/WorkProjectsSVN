#ifndef __CONFIG_3PHASE_METER_H
#define __CONFIG_3PHASE_METER_H
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
	#define  SEL_DLT698_2016_FUNC            	YES
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
	#if( SEL_DEMAND_2022 == NO )
	//�Ƿ�ѡ�������������  ��SEL_APPARENT_ENERGY�޹أ����ڹ���ʲôʱ��֧��
	#define	SEL_APPARENT_DEMAND					YES
	#else//������������֧����������
	#define	SEL_APPARENT_DEMAND					NO
	#endif
	//�Ƿ�֧�ֶ����������������
	#define	SEL_FREEZE_PERIOD_DEMAND			YES
	//�Ƿ�֧�ְ�ʱ���㹦��
	#define SEL_AHOUR_FUNC						YES

	//�洢�Ƿ�֧��Ӳ������
	#define SEL_MEMORY_PROTECT					YES

	//----------------------------------------------------
	//���ݿ����ֵ����
	//----------------------------------------------------
	//ѡ�����ݿ��е���С��λ�������ѡ��С����λ��̫�����ݿ⽫���������洢
	#define SYS_DBASE_ENERGY_DOT		0xff//���ݿⰴ�������洢
	//����¼����
	#define MAX_MONTH					13
	#define MAX_MONTH_FOR_BACKUP		3	// �������ݱ�������
	//��������
	#define MAX_RATIO					12
	#if(PREPAY_MODE == PREPAY_LOCAL)	
	//��������
	#define MAX_LADDER					6
	//ESAM����������
	#define MAX_ESAM_LADDER				6
	//������������
	#define MAX_YEAR_BILL				4
    //ESAM��������������
	#define MAX_ESAM_YEAR_BILL			4
	#endif
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
	#define SEL_AUTO_WARN					YES
	// ������Դʧ��
	#define SEL_EVENT_LOST_SECPOWER			YES//���඼�򿪣�����Ҫ�˹�����cSEL_SECPOWER����
	// ʧѹ��¼
	#define SEL_EVENT_LOST_V				YES
	// Ƿѹ��¼
	#define SEL_EVENT_WEAK_V				YES
	// ��ѹ��¼
	#define SEL_EVENT_OVER_V				YES
	// �����¼
	#define SEL_EVENT_BREAK					YES
	// ��ѹ�������¼
	#define SEL_EVENT_V_REVERSAL			YES
	// �����������¼
	#define SEL_EVENT_I_REVERSAL			YES
	// ��ѹ��ƽ���¼
	#define SEL_EVENT_V_UNBALANCE			YES
	// ������ƽ���¼
	#define SEL_EVENT_I_UNBALANCE			YES
	// ʧ����¼
	#define SEL_EVENT_LOST_I				YES
	// ������¼
	#define SEL_EVENT_OVER_I				YES
	// ������¼
	#define SEL_EVENT_BREAK_I				YES
	// ���� ���ʷ���
	#define SEL_EVENT_BACKPROP				YES
	// ���ؼ�¼
	#define SEL_EVENT_OVERLOAD				YES
	// �����¼
	#define SEL_EVENT_LOST_POWER			YES
	// ����Ǽ�¼
	#define SEL_EVENT_METERCOVER     		YES
	// ����ť�м�¼
	#define SEL_EVENT_BUTTONCOVER			YES
	// ���๦���������޼�¼
	#define SEL_EVENT_COS_OVER				YES
	// �������ز�ƽ���¼
	#define SEL_EVENT_I_S_UNBALANCE			YES
	// ���������ߵ�����ƽ���¼
	#define SEL_EVENT_SP_I_UNBALANCE		NO
	// �㶨�ų����ż�¼
	#define SEL_EVENT_MAGNETIC_INT			YES
	// ���ɿ����󶯼�¼
	#define SEL_EVENT_RELAY_ERR				YES
	// ��Դ�쳣��¼
	#define SEL_EVENT_POWER_ERR				YES
	//ȫʧѹ��¼
	#define SEL_EVENT_LOST_ALL_V			YES
	//ʱ�ӹ���
	#define SEL_EVENT_RTC_ERR				YES
	//����оƬ����
	#define SEL_EVENT_SAMPLECHIP_ERR		YES
	//���ܱ����ߵ����쳣
	#define SEL_EVENT_NEUTRAL_CURRENT_ERR	YES
	//------------------------------------------------------------------------------------
	// ��̼�¼ѡ��  ��12��
	//--------------------------------------
	// ��������¼
	#define SEL_PRG_INFO_CLEAR_METER			YES
	// ��������¼
	#define SEL_PRG_INFO_CLEAR_MD				YES
	// �¼������¼
	#define SEL_PRG_INFO_CLEAR_EVENT			YES
	// Уʱ��¼
	#define SEL_PRG_INFO_ADJUST_TIME			YES
	// ʱ�α��̼�¼
	#define SEL_PRG_INFO_TIME_TABLE				YES
	// ʱ�����̼�¼
	#define SEL_PRG_INFO_TIME_AREA				YES
	// �����ձ�̼�¼
	#define SEL_PRG_INFO_WEEK					YES
	// �ڼ��ձ�̼�¼
	#define SEL_PRG_INFO_HOLIDAY				YES
	// �й���Ϸ�ʽ��̼�¼
	#define SEL_PRG_INFO_P_COMBO				YES
	// �޹���Ϸ�ʽ��̼�¼
	#define SEL_PRG_INFO_Q_COMBO				YES
	// �����ձ�̼�¼
	#define SEL_PRG_INFO_CLOSING_DAY			YES
	// ��Կ���¼�¼
	#define SEL_PRG_UPDATE_KEY					YES	
	// 645��̼�¼����¼���10�����ݱ�ʶ��
	#define SEL_PRG_RECORD645					YES
		// �㲥Уʱ��¼
	#define SEL_PRG_INFO_BROADJUST_TIME			YES
	//------------------------------------------------------------------------------------------------------------
	// ��չ�¼�����ѡ��  ��6��
	//------------------------------------------------------------------------------------------------------------
	// �����¼
	#if(PREPAY_MODE == PREPAY_LOCAL)
	#define SEL_EVENTEXT_BUYELEC				YES
	// �Ƿ��忨��¼
	#define SEL_EVENTEXT_ILLEGALCARD			YES
	#else
	#define SEL_EVENTEXT_BUYELEC				NO
	// �Ƿ��忨��¼
	#define SEL_EVENTEXT_ILLEGALCARD			NO
	#endif
	//------------------------------------------------------------------------------------
	// ͳ�Ƽ�¼ѡ��  ��2��
	//--------------------------------------
	// ��ѹ�ϸ��ʼ�¼
	#define SEL_STAT_V_RUN						YES
	// �������޼�¼
	#define SEL_DEMAND_OVER						YES
	
#endif//#if( MAX_PHASE == 3 )

#endif//#ifndef __CONFIG_3PHASE_METER_H
