#ifndef GDW698_H__
#define GDW698_H__
#include "appcfg.h"

#if(MD_BASEUSER	== MD_GDW698)
#define MIN_RXFM_SIZE   10		//��С����֡��
#include "GDW698DataDef.h"
#include "GDW698Funcs.h"
#include "cgy.h"

#define MAX_SAPDU_SIZE		4096//�����������APDU����	
#define	MAX_HEAD_TAIL_LEN	37	//֡ͷ֡β����󳤶ȣ���10�ֽ�APDUͷ��Ϣ��
#define MAX_OBJ_DATA_LEN	50	//����APDU�����ݵ���󳤶�
#define MAX_DWN_485FRAME_LEN	512	//�������֡����(͸��)	
#define MAX_DWN_ZBFRAME_LEN		256	//�������֡����(͸��)	
	
#define MAX_NUM_NEST	10
#define MAX_GDW698_MEMSIZE	16*1024	//698�����ڴ�	
#define OBJ_START_FLAG	0x68	//��ʼ�ַ�
#define CTRL_DIR	0x80		//���䷽��λ 0-����;1-����
#define CTRL_SC		0x08		//�����ʶλ
#define PRM			0x40		//������־λ 0-���Է�����;1-���Կͻ���
#define PRM_BIT		14

#define ACD			0x40		//Ҫ�����λ
#define ADDR_BITS	0x0F		//��ַ����

#define CTRL_SP		0x020		//��֡��ʶλ
#define CTRLCODE	0x0F		//������

//������
#define CC_PRM1_NO_ANSWER	0	//0 ���ͨM�޻ش� �û�����								
#define CC_PRM1_SEND		1	//1 ��·����
#define CC_PRM1_DATA		3	//3 �û�����

//���ݵ�Ԫ
#define OBJ_NO_DATA		0x00	//������
#define OBJ_END_FLAG	0x16	//�����ַ�

//��ȫ�����еļ��ܷ�ʽ
// typedef enum _OBJ_SECURITY_TYPE{
// 	Plain_Text = 0, //����
// 		Plain_Text_RN = 1, //����+�����
// 		Clipher_Text = 2, //����
// 		Clipher_Text_MAC = 3, //����+SID_MAC
// 		Plain_Text_MAC = 4, //����+SID_MAC  ����ȫ�����ͬ�� ����+�������
// }OBJ_SECURITY_TYPE;


// typedef struct _TRxAPDU{
// 	HPARA	hMem;				//�ڴ�ָ��
// 	WORD	wMemSize;			//�ڴ��С
// 	WORD	wRPtr;				//��ǰ����ָ��
// 	WORD	wOffset;			//�����ݵ����ݸ���ƫ��
// 	WORD	wOldReq;
// 	BYTE	byRxCtrl;			//���տ�����
// 	WORD	wApduWptr;
// 	DWORD	dwRxTime;			//����ʱ��
// 	BYTE	byPIID;
// 	BYTE	byRxAddrType;		//��ַ����
// 	BYTE    byCtrl;				//BIT4-����ʱ���ǩ��BIT0-ʱ���ǩ�Ƿ���Ч��BIT1-�Ƿ�Ϊ��֡����,BIT2--��֡��Ч,BIT3-1�к���֡
// 	TimeTag time_tag;
// 	BYTE	byPrivate;			//��ȫ����
// 	BYTE	abyRound[32];
// 	WORD	tx_seq_no;
// }TRxAPDU;

#pragma pack(1)
typedef struct _TClass_get{
	BYTE class_no;				//���
	FW_DWHWH func_get;	
	FW_DHWHWHH func_get_rec;	
}TClass_get;

// typedef struct _TClass_set{
// 	BYTE class_no;				//���
// 	FB_DHH func_set;
// }TClass_set;

// typedef struct _TClass_action{
// 	BYTE class_no;				//���
// 	FB_DHHH func_action;
// }TClass_action;

typedef struct OBJ_TAIL{
	WORD wCheckCS;				//У���
	BYTE byEnd;					//������
}OBJ_TAIL;

//����֡β��֡����
typedef struct OBJ_EXCEPT_TAIL{
	BYTE  byStart;				//�����ַ�0X68
	WORD wFmLen;				//֡��
	BYTE  byCtrl;				//������C
	BYTE  byAddrF;
	BYTE  _byAddrStart;
}OBJ_EXCEPT_TAIL;

typedef struct _TGDW698TaskHead{
	BYTE task_no;				//�����
	WORD type_len;				//�������ͳ��ȣ�BIT15-1����Դ�ڵ����0�ն˶���,BIT14-(1_698��0-645) ��
	DWORD start_time;			//��������ʱ��
	DWORD save_time;			//�������ʱ��
	DWORD success_time;			//����ɹ�ʱ��
}TGDW698TaskHead;
#pragma pack()

#define	MAX_FRAME_HEAD_LEN		(28) //���֡ͷ����

typedef struct _TFrame698_FULL_BASE{
	OBJ_EXCEPT_TAIL *pFrame;
	BYTE *pDataAPDU;
	TSA  *pObjTsa;
	OBJ_TAIL *pTail;
}TFrame698_FULL_BASE;

//���ݿǻ���
// typedef struct OBJ_DATA_SHELL_BASE{
// 	TFrame698_FULL_BASE *_pFullFrame; //���shell������frameָ��
// 	BYTE* _pDataFirst;			//����ͷ
// 	void* _pDataEnd;			//����β
// 	void* _pCursor;				//��ǰָ��
// }OBJ_DATA_SHELL_BASE;

typedef struct _TTask698RunInfo{
	BYTE  byStatus;				//(0-δִ�У�1-ִ���У�2-��ִ��)
	WORD total_num;				//�ܸ��� 	
	WORD wCj_No;				//�ɼ�����	
	WORD success_num;			//�ɼ��ɹ�����
	WORD tx_packet_num;			//�ѷ��ͱ�������
	WORD rx_packet_num;			//�ѽ��ձ�������
	BYTE byRetryNum[MAX_DWNPORT_NUM]; //
	DWORD dwTaskTime;			//����ʱ��	
	DWORD startTask_time[MAX_DWNPORT_NUM];			//������ʼʱ��
	DWORD success_time;			//����ɹ�ʱ��
    BYTE success_mps[(MAX_MP_NUM+7)/8];    //�ɼ��ɹ��Ĳ�����
}TTask698RunInfo;
// BYTE get_client_addr(void);
// BYTE get_logic_addr(void);
// void MakeServerFrame(BYTE byCtrl,BYTE *pAPDU,WORD wAPDULen);
//������Լ����
#include "GDW698Base.h"
//#include "APDU/Get.h"
//#include "APDU/Set.h"
//#include "APDU/Action.h"
//#include "APDU/Report.h"
//#include "APDU/Proxy.h"
//#include "APDU/Security.h"
//#include "APDU/Link.h"
//#include "APDU/Connect.h"
//#include "APDU/Release.h"
//#include "GDW698Alm.h"
#include "GDW698_PARA.h"
//#include "DBase/fun/dbFrz_698.h"
#include "Class/Class_698.h"
#endif
#endif