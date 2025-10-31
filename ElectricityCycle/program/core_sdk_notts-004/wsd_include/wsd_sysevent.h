//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з��� 
//������	
//��������	
//����		SYSEVENTͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __SYSEVENT_H
#define __SYSEVENT_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define MAX_SYS_UN_MSG		256
//�¼���¼

//ģ���ϵ�
#define SYSUN_POWER_ON			0x8A01
//ģ�����
#define SYSUN_POWER_DOWN		0x8A02
//mqtt�������ر�
#define SYSUN_MQTT_DISCON		0x8A03
//tcp �������ر�
#define SYSUN_TCP_CLOSE			0x8A04    //����pn �������رս�Ƶ�� �������¼�쳣�¼�
//tcp ����������ʧ��
#define SYSUN_TCP_CONERR		0x8A05  
//�����쳣 
#define SYSUN_NETWORK_ABNORMAL  0x8A06
//���ղ��δ��ڶ��б仯
#define QUEUE_WAVE				0x8A07
//���ݲ�����
#define DATA_LOST				0x8A08
//д�ļ�����д���ʱ��
#define WRITE_TF_MS				0x8A09
//�������ݽ�����
#define WAVE_QUEUE_FULL			0x8A10
//�������ݶ��� ʣ��ռ䷢���仯
#define WAVE_QUEUE_SPACE_CHANGE	0x8A11
//���շ���֡�ֽ�
#define UART_RECV_NO_815		0x8A13
//���մ���4096�ֽ�
#define UART_RECV_EXCEED_4096	0x8A14
//tcp���Է��Ͳ�������ʧ��
#define TCP_RETRY_SEND_DATA_ERR	0x8A15
//�źŲ�
#define BAD_SIGNAL				0x8A16
//�������ݼ��tcp�Ͽ�
#define SEND_DATA_DISCONNECT	0x8A17
//tcp ���ж����ݼ��tcp�Ͽ�
#define READ_DATA_DISCONNECT	0x8A18
//��������ʧ��
#define RECON_SEND_DATA_ERR		0x8A19
//tcp δ�ͷ�socket��Դ
#define TCP_NOFREE_SOCKET		0x8A2A
//mqtt δ�ͷ�socket��Դ
#define MQTT_NOFREE_SOCKET		0x8A2B
//ģ�����
#define POWER_DOWN				0x8AFF

//
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

//�¼���¼��ʽ
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

//�����ռ�ṹ����
typedef struct TConMem_t
{		
	//�쳣��Ϣ
	TSysAbrEventConRom 	SysAbrEventConRom;
}TConMem;
//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------
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
//����ֵ:  	�������ݳ���
//
//��ע:   
//-----------------------------------------------
WORD api_ReadSysUNMsg(WORD Num,BYTE *Buf);
//--------------------------------------------------
//��������:  �ϵ紴�����ʱ���¼�ļ�
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_PowerOnCreatSyseventTable( void );
BOOL  api_DeleteSysEventFile( void );
#endif //#ifndef __SYSEVENT_H
