#ifndef __MPRUNINFO_H__
#define __MPRUNINFO_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef enum MPDT_TYPE{
	DT_NUL = 0, DT_F1, DT_F2, DT_F3, DT_F4, DT_F5,DT_F6,DT_F7,DT_F8, DT_F9, DT_F10,
	DT_F11, DT_F12, DT_F13, DT_F14, DT_F15,DT_F16,DT_F17,DT_F18, DT_F19,DT_F20,
	DT_F21, DT_F22, DT_F23, DT_F24, DT_F25,DT_F26,DT_F27,DT_F28, DT_F29,DT_F30,
	DT_F31,DT_F32,DT_F33,DT_F34,DT_F35,DT_MAX, DT_END //������MAX��DT_ENDΪ�б�ʹ��
}MPDT_TYPE;

#pragma pack(1)
typedef struct _UData{
	TMPTime	Time;
	DWORD	dwVal;
}UData;

//�������Զ�������
typedef struct _MPDT_F1{
	WORD	AutoDay[3];			//16����
}MPDT_F1;

//����
typedef struct _MPDT_F2{
	DWORD	Num[4];				//�ܼ�A��B��C�������
	DWORD	Time[4];			//�����ۼ�ʱ��
	TMPTime	STime[4];			//���һ�ζ�����ʼʱ��
	TMPTime	ETime[4];			//���һ�ζ������ʱ��
	DWORD	DD[4];				//���෢��ʱ�����й����
	long	AI[4][6];			//���෢��ʱ��ѹ������
	DWORD	EDD[4];				//�������ʱ�����й����
}MPDT_F2;

//ʧѹ
typedef MPDT_F2		MPDT_F3;

//���ʱ�䡢����
typedef struct _MPDT_F4{
	DWORD	dwNum;				//����
	TMPTime Time;				
}MPDT_F4;

//������0
typedef MPDT_F4		MPDT_F5;
//ʱ�α���	
typedef MPDT_F4		MPDT_F6;

//��ع���ʱ��
typedef struct _MPDT_F7{
	DWORD	dwMins;				
}MPDT_F7;

//���һ�δų����ż�¼ 
typedef struct _MPDT_F8{
	DWORD	dwNum;				//����
	TMPTime STime;		
	TMPTime ETime;		
}MPDT_F8;

//���һ�ο��Ǵ���
typedef MPDT_F8		MPDT_F9;

//�����¼ 
typedef MPDT_F4		MPDT_F10;	
//�¼������¼ 
typedef MPDT_F4		MPDT_F11;	

//Уʱ��¼ 
typedef MPDT_F8		MPDT_F12;	

//��������
typedef struct _MPDT_F13{
	DWORD	Num[3];			
}MPDT_F13;

//���峣��
typedef struct _MPDT_F14{
	DWORD	Const[2];			
}MPDT_F14;

//ʱ�β���
typedef struct _MPDT_F15{
	DWORD	Seg[8];			
}MPDT_F15;

//PT��CT����
typedef struct _MPDT_F16{
	WORD	PCT[2];
}MPDT_F16;

//����Ǽ�¼
typedef struct _MPDT_F17{
	DWORD	dwNum;				//����
	TMPTime STime;		
	TMPTime ETime;	
	DWORD	DD[12];				//6������ǰ��6��������
}MPDT_F17;

//��β�Ǽ�¼
typedef MPDT_F17	MPDT_F18;

//���10�ε����¼
typedef struct _MPDT_F19{
	DWORD	dwNum;				//����
	TMPTime STime[10];		
	TMPTime ETime[10];		
}MPDT_F19;

//���һ����բ��¼
typedef MPDT_F4	MPDT_F20;	

//���һ�κ�բ��¼
typedef MPDT_F4	MPDT_F21;	
//��������
typedef MPDT_F13 MPDT_F22;	
//ʧ������
typedef MPDT_F13 MPDT_F23;	
//��Դ�쳣
typedef struct _MPDT_F24{
	DWORD	dwNum;			
}MPDT_F24;
//��ѹ������
typedef MPDT_F24 MPDT_F25;	
//��ѹ��ƽ��
typedef MPDT_F24 MPDT_F26;	
//Ƿѹ����
typedef MPDT_F13 MPDT_F27;	
//��ѹ����
typedef MPDT_F13 MPDT_F28;
//���ɿ��ض�������
typedef MPDT_F24 MPDT_F29;	
//���ܱ�״̬�ּ������ʶ
typedef struct _MPDT_F30{
	WORD	Status[7];			
	WORD	BWStatus[7];			
}MPDT_F30; 	
//���ܱ��¼��ɼ�״̬��
typedef struct _MPDT_F31{
	BYTE	byFramLen;					//֡��
	BYTE	UpStatus[12];				//�����ϱ�״̬��	
	BYTE	EventIncNum[62];			//�¼����ӵĸ���(˳��������ϱ�״̬�ֺ��¼���Ӧ��,���0�����Ƿ��������ϱ�״̬��)
}MPDT_F31; 	

typedef struct _MPDT_F32{				//�¼�����״̬��
	BYTE	EventIncNum[62];			//�¼����ӵĸ���(˳��������ϱ�״̬�ֺ��¼���Ӧ��,���0�����Ƿ��������ϱ�״̬��)
}MPDT_F32; 	

typedef struct _MPDT_F33{				
	DWORD	EventNum[62];				//�¼��ĸ���(˳��������ϱ�״̬�ֺ��¼���Ӧ��,���0�����Ƿ��������ϱ�״̬��)
	BYTE	ValidFlag[8];
}MPDT_F33; 

typedef struct _MPDT_F34{				
	BYTE	byFmLen;					//040015����֡����
	BYTE	FmData[60];
}PACK MPDT_F34; 

typedef struct _MPDT_F35{
	DWORD	EventNum[62];				//�¼��ĸ���(˳��������ϱ�״̬�ֺ��¼���Ӧ��,���0�����Ƿ��������ϱ�״̬��)
	BYTE	ValidFlag[8];
}PACK MPDT_F35; 

#pragma pack()

typedef struct _MPDT_LEN{
	MPDT_TYPE Fn;
	WORD wLen;
}MPDT_LEN;

// BOOL IsEventInfo(DWORD dwDI);
// BOOL GetMPFnSize(MPDT_TYPE ft,DWORD *pdwSize);
// BOOL GetMPFnIndex(MPDT_TYPE ft,DWORD *pdwOffset);
// void WriteMeterRunInfo(MPDT_TYPE ft,WORD wNo,UData Val);
// void InitMPAllRunInfo(WORD wMPNo,HPARA hInfo);
// BOOL GetMPAllRunInfo(WORD wMPNo,HPARA hInfo);
// BOOL WriteMPAllRunInfo(WORD wMPNo,HPARA hInfo,DWORD dwFreshFlag);
// BOOL GetMPRunInfo(WORD wMPNo,MPDT_TYPE ft,HPARA hInfo);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
