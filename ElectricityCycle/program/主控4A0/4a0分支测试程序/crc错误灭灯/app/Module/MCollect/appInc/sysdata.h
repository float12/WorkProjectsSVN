#ifndef __SYSDATA_H__
#define __SYSDATA_H__
#ifdef __cplusplus
 #if __cplusplus
	extern "C" {
 #endif
#endif

	
//������EEPROM�е�ϵͳ�����ļ�
//!!!!!!!!!!!!!!!!��������������ã�����Ӱ�������ܵ�ƫ�ƣ���Ҫʱʹ�ýṹ�б�����Bak�ֶ�	
//�޸�Ӱ��������EEPROM����������		
#pragma pack(1)
//��������˫���ݣ�����У��Բſ���
// typedef struct _TERomSysCfg{
// 	// BYTE byUserType;			//�û�����
// //	BYTE GyNo[20][2];			//�ⲿ��Լ��
// 	// BYTE byDays_BC;				//�ն��Ჹ������
// 	// BYTE byDFrz_MaxCJHour;		//�ն�����ಹ��Сʱ��	
// 	// BYTE byJCType;				//0x33-��������,0x55-�޽���,0x34-��������
// 	// BYTE bySourceType;			//1-����ᡢ����-���û�ж���ʱ�����ն˶���	
// 	// BYTE byCurvSource;			//0x55-����ᡢ����-�����ն˶���	
// 	// BYTE byMFRZSource;			//0x55-�����ݶ�����Դ�ڵ���ᣬ��������Դ�ڵ����1	
// 	// BYTE byHaveESAM;			//��Ӳ��ESAM
// //����
// 	// BYTE byRealFromMeter;		//ʵʱ����ֱ��ȡ�Ե�� 0x55
// 	//��Ҫ�ŵ�04Fx������
// 	WORD wInitParaValid;		//0x5AA5 ��ʼ��������������Ч		
// 	// WORD wDefaultJCMPNo;		//ȱʡ���ɲ������
// 	// BYTE bySysCtrlFlag;         //BIT0 -(1)���ж�376.1��Լ��CON��ʶλ,BIT1-1 (698�ɱ�MAC��֤��Ч)
// 	DWORD dwCheck;
// } TERomSysCfg;

#pragma pack()

//��ṹƫ��
//#define offsetof(structTest,e) (size_t)&(((structTest*)0)->e)		

// typedef struct _TVar{
// 	//�ڴ���ʱ״̬	
// 	DWORD	dwStartTime;			//ϵͳ����ʱ��
// 	// DWORD   dwLastOnTime;			//���һ���ϵ�ʱ��
// 	// DWORD   dwLastStopTime;			//�ϴ�ͣ��ʱ��
// 	// BYTE	bySavePara;				//�Ƿ�������������
// 	// BYTE	byInitOK;				//�Ƿ��ʼ������
// 	// BYTE	byStart_reset;			//ϵͳ�Ƿ�Ϊ��λ����
// 	TTime	GyTime;
// 	// BYTE	bResetSystem;				//..
// 	//EROMӳ��(����������Ϣ)
// 	// TERomSysCfg Cfg;
// }TVar;

// #define GetUserType()	(gVar.Cfg.byUserType)
// #define GetDataSource()	(gVar.Cfg.bySourceType)
// #define GetJCType()		(gVar.Cfg.byJCType)
// #define GetCurveSource() (gVar.Cfg.byCurvSource)


// extern TVar gVar;
void SaveNvRam_DF6203(void);
void ParaSaveDaemon(BOOL bFlag,TTime *pTime);
BOOL IsDlt698_45(WORD wDrvID);
BYTE PortID2No(DWORD dwPortID);
//�ɼ���Ϣ


 #ifdef __cplusplus
  #if __cplusplus
}
  #endif
 #endif
#endif
