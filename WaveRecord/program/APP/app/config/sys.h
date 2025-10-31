//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.27
//����		���������û�����
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __SYS_H
#define __SYS_H

//�Ƿ�ѳ���ѡ��ΪDEBUG�汾��DEBUG�汾֧��ASSERT������֧��ASSERT
#define SEL_DEBUG_VERSION			YES

//�Ƿ��Flash������������ʱ���������Կɹر�
#define SEL_FLASH_PROTECT   FLASH_PROTECT_OPEN
//����FLASH������   FLASH_PROTECT_OPEN
//�ر�FLASH������   FLASH_PROTECT_COLSE

//���Ͷ���  ͨ����ӡ�ư��ϵ����ڻ�ͼ��ѡ����Ӧ�İ���
#define BOARD_TYPE			BOARD_HC_SEPARATETYPE
//�����
//BOARD_HT_SINGLE_78201602		11602//20�淶 ����SPI 9600������-Ĭ����E2����E2���ı�������
//BOARD_HT_SINGLE_78202201		12201//20�淶 ����SPI 115200������ ̼Ĥ����-Ĭ����E2����E2���ı�������
//BOARD_HT_SINGLE_78202202		12202//20�淶 ����SPI 115200������ �߷���-Ĭ����E2����E2���ı�������
//�����
//#define BOARD_HT_THREE_0134566		34566//20�淶�����+��������Ϸ�ʽ+6AС��������Ϊ5ŷ-Ĭ����E2����E2���ı�������
//#define BOARD_HT_THREE_0131699		31699//20�淶�����+��������·�ʽ+6AС��������Ϊ5ŷ-Ĭ����E2����E2���ı�������
//#define BOARD_HT_THREE_0130347		30347//20�淶�����+��������·�ʽ+С��������ӱ�+֧��115200������-Ĭ����E2����E2���ı�������



//ѡ�������
#define cMETER_TYPES		METER_ZT
//METER_ZT		ֱͨ������Ϊ220V��
//METER_3P3W	3��3�߱�����Ϊ100V��
//METER_3P4W	3��4�߱�57V��220V��

#define cMETER_VOLTAGE		METER_220V
//METER_57V		57V��ѹ
//METER_100V	100V��ѹ
//METER_220V	220v��ѹ
//METER_380V	380v��ѹ

//��������ѡ��
#if (HARDWARE_TYPE == SINGLE_PHASE_BOARD)
#define cCURR_TYPES			CURR_20A
#elif (HARDWARE_TYPE == THREE_PHASE_BOARD)
#define cCURR_TYPES			CURR_500A
#endif
//CURR_1A		0.3(1.2)A��
//CURR_6A		1.5(6)A��
//CURR_60A		5(60)A
//CURR_100A		10(100)


//Ԥ����ģʽ(���ܱ�ѡ��PREPAY_REMOTE)
#define PREPAY_MODE			PREPAY_NO
//PREPAY_NO					û��Ԥ���ѹ��ܣ��ݲ� ��ѡ��
//PREPAY_LOCAL				���Ԥ���ѣ����أ�
//PREPAY_REMOTE				��վԤ����(Զ��)


//�̵�������
#define cRELAY_TYPE			RELAY_NO
//RELAY_NO					�޼̵���
//RELAY_INSIDE				���ü̵���
//RELAY_OUTSIDE				���ü̵���

//�й��ȼ�ѡ�񣨲�Ӱ�����ȶԣ�ֻ�ı䱣�����ڵĵ��ֻ��������
#define PPRECISION_TYPE		PPRECISION_B
//PPRECISION_A	 		    A����-2��          
//PPRECISION_B		        B����-1��
//PPRECISION_C	        	C����-0.5S
//PPRECISION_D			    D����-0.2S

//�������㷽ʽ �������������
#define SEL_DEMAND_2022		YES
//NO--���ù���������
//YEW-���õ���������-�·�ʽ-22-1����ʼ�л�

//�����û�ѡ��
#define cSPECIAL_CLIENTS		SPECIAL_GUO_WANG
//#define SPECIAL_GUO_WANG				0xff

//��������� �����ó�������Ӳ��ȷ�ϴ���Ŀ��
#define cOSCILATOR_TYPE		OSCILATOR_JG
//OSCILATOR_JG			0x01	//��������
//OSCILATOR_XTC	        0x02	//�����Ǿ���
//OSCILATOR_XTC_3215	0x03	//�����Ǿ���_3215(С��װ����)

#endif//��Ӧ�ļ���ǰ���#ifndef __SYS_H



