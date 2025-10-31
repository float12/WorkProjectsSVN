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
#define MAX_SYS_UN_MSG		128
//�¼���¼

//ģ���ϵ�
#define SYSUN_POWER_ON          0x8A01
//ģ�����
#define SYSUN_POWER_DOWN        0x8A02
//mqtt�������ر�
#define SYSUN_MQTT_DISCON       0x8A03
//tcp �������ر�
#define SYSUN_TCP_CLOSE         0x8A04    //����pn �������رս�Ƶ�� �������¼�쳣�¼�
//tcp ����������ʧ��
#define SYSUN_TCP_CONERR        0x8A05  
//�����쳣 
#define SYSUN_NETWORK_ABNORMAL  0x8A06
//789��Ӧ��

//��ظ�λ��¼.............................................
//WDG ���Ź����¸�λ
#define WDG_RESET				0x8AFC
//tcp����
#define TCP_DISCONNECT_RESET	0x8AFD
//����ʧ�ܵ��¸�λ				
#define DATACALL_RESET			0x8AFE
//ģ�����
#define POWER_DOWN				0x8AFF
//tcp δ�ͷ�socket��Դ
#define TCP_NOFREE_SOCKET		0x8A2A
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
#endif //#ifndef __SYSEVENT_H
