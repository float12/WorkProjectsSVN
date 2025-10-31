
#ifndef __FLAGS_H__
#define __FLAGS_H__


//ע�⣺��׼��ʶ�Ŵ�0x20��ʼ����Լ�������ʹ��(0~0x1F��Ϊ������ʹ��)
typedef enum{
	MSF_CENT_0=0,				//0~10Ϊ��ʱ�������־����������ռ��
	//һ��ʵʱ
	MSF_DATE=0x20,				//�������
	MSF_TIME,					//���ʱ��
	MSF_APR,					//����й����ܣ������ʣ�
	MSF_AP,						//�����й����ܣ������ʣ�
	MSF_AR,						//�����й����ܣ������ʣ�
	MSF_RP,						//�����޹����ܣ������ʣ�
	MSF_RR,						//�����޹����ܣ������ʣ�
	MSF_R1,						//һ�����޹����ܣ������ʣ�
	MSF_R2,						//�������޹����ܣ������ʣ�
	MSF_R3,						//�������޹����ܣ������ʣ�
	MSF_R4,						//�������޹����ܣ������ʣ�
	MSF_A_ARP,					//A�������		
	MSF_B_ARP,					//B�������		
	MSF_C_ARP,					//C�������		
	MSF_LEFTDD,					//ʣ����
	MSF_PAYNUM,					//�������
	MSF_PAYSUM,					//�ۼƹ�����
	MSF_OVERDRAFT,              //͸֧���
	//1.2ʵʱң��ֵ
	MSF_VT,      				//��ѹ˲ʱ��
	MSF_CT,     				//����˲ʱ��
	MSF_PA,						//�й�����˲ʱ��
	MSF_PR,						//�޹�����˲ʱ��
	MSF_PAP,					//���ڹ���
	MSF_PE,						//��������˲ʱ��
	MSF_I0,						//�������	
	MSF_VIANGLE,				//��λ��
	MSF_FREQ,					//Ƶ��
	MSF_STSMET,					//���״̬��
	MSF_STSMET1,				//���״̬��1
	MSF_STSREST,				//������״̬��
	//1.3����	
	MSF_APXL,    				//�����й����������Լ�����ʱ��
	MSF_ARXL,    				//�����й����������Լ�����ʱ��
	MSF_RPXL,    				//�����޹����������Լ�����ʱ��
	MSF_RRXL,    				//�����޹����������Լ�����ʱ��

	//1.3����
	MSF_BATTIME,				//��ع���ʱ��
	MSF_CONSTANT,				//�й����޹�����
	RSF_TRIPTM,					//���һ��Զ�̿�����/��բʱ��
	RSF_TIMESEG,				//��Xʱ�α����
	//�¼�
	MSF_OPEN_WG,				//β��						
	MSF_OPEN_DNG,				//��ť��
	MSF_XLCLRNUM,				//�����������
	MSF_PROGNUM,				//��̴���
	MSF_CLR_NUM,				//�������	
	MSF_CLRETNUM,				//�¼��������	
	MSF_CORTTMNUM,				//Уʱ����
	RSF_TMPRONUM,				//ʱ�α��
	MSF_DXINFOA,				//A��������645 2�α���
	MSF_DXINFOB,				//B�����
	MSF_DXINFOC,				//C�����
	MSF_DXALL,					//2�α���ǰ 
	MSF_STSMET0,
	MSF_SYINFALL,				//ȫʧѹ
	MSF_SYINFOA,     			//A��ʧѹ��Ϣ
	MSF_SYINFOB,      			//B��ʧѹ��Ϣ
	MSF_SYINFOC,				//C��ʧѹ��Ϣ
	MSF_TMPROTIME,   			//���һ��ʱ�α���ʱ��
	MSF_LASTMFREC,   			//���һ�δų����Ƽ�¼
	MSF_DLCOUNTA,    			//A������ܴ���
	MSF_DLCOUNTB,    			//B������ܴ���
	MSF_DLCOUNTC,    			//C������ܴ���
	MSF_QSY_TIME,    			//ȫʧѹ�ܴ��� �ۼ�ʱ��
	MSF_LOSTPW_NUM,				//�������
	MSF_LOSTPWTIME,				//����ʱ��

	MSF_UPSTSMET,				//��������ϱ�״̬��
	MSF_EVENTCJ,
	MSF_EVENTCJ_EX,
	
	//������	
	MSF_CURE1,
	MSF_CURE2,
	MSF_CURE3,
	MSF_CURE4,
	MSF_CURE5,
	MSF_CURE6,
	MSF_CURE,
	//�����ն���
	MSF_DAY_TM,  	
	MSF_DAY_AP,		
	MSF_DAY_AR,		
	MSF_DAY_RP,		
	MSF_DAY_RR,		
	MSF_DAY_R1,  	
	MSF_DAY_R2,  	
	MSF_DAY_R3,  	
	MSF_DAY_R4,  	
	MSF_DAYXLAP, 	
	MSF_DAYXLAR, 	
	MSF_DAYXLRP, 	
	MSF_DAYXLRR, 
	MSF_DAYLEFTPAY,  //..add by zsm 20190528
	MSF_DAYP07,
	//�ġ��¶���
	MSF_MON_APR,	
	MSF_MON_AP,		
	MSF_MON_AR,		
	MSF_MON_RP,		
	MSF_MON_RR,		
	MSF_MON_R1,  	
	MSF_MON_R2,  	
	MSF_MON_R3,  	
	MSF_MON_R4,
	MSF_MON_A,					//A�������		
	MSF_MON_B,					//B�������		
	MSF_MON_C,					//C�������
	MSF_MONXLAP,				//����������
	MSF_MONXLAR,				//�·�������
	MSF_MONXLRP,				//����������
	MSF_MONXLRR,				//�·�������	
	//�塢����
	MSF_AUTODAY,
	MSF_TMPRO_NUM,   			//ʱ�α��̴���
	MSF_HGQBB_CPT,   			//������ѹ���������
	MSF_CALC_DATA,					//��Ƽ�������

	MAX_FLAGNO					//�˿�����ʶ����
}MeterMsf;		

DWORD GetFlag(DWORD dwFlagNo);
void  SetFlag(DWORD dwFlagNo);
void  ClearFlag(DWORD dwFlagNo);
void  ClearAllFlag(void);
BOOL  CheckClearFlag(DWORD dwFlagNo);
#endif
