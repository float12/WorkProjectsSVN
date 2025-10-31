///////////////////////////////////////////////////////////////
//	�� �� �� :usros.h
//	�ļ����� :����ϵͳ�����ӿ�
//	��    �� : jiangjy
//	����ʱ�� : 2015��7��21��
//	��Ŀ���� ��
//	����ϵͳ : 
//	��    ע :
//	��ʷ��¼�� : 
///////////////////////////////////////////////////////////////
#ifndef _USROS_H_
#define _USROS_H_
#ifdef __cplusplus
 #if __cplusplus
	extern "C" {
 #endif
#endif

#if(MOS_MSVC)
#define DATA_PATH		"C:/DF6203FS"
#define ZK_PATH			"./vcLib/VLinux"
#else
#define DATA_PATH		"/DF6203FS"
#define ZK_PATH			"/usr/lib"
#endif

// BOOL OS_GetTaskName(char *pszTaskName);		
//void  SM_P(DWORD dwPVID);
//void SM_V(DWORD dwPVID);
// BOOL OS_SetTimer(DWORD events,DWORD dwMSTimes);
// void OS_KillTimer(DWORD events);

//�������ȼ�����
#ifdef _MSC_VER
#define MTPRI_WATCHDOG	-13         //WATCHDOG���ȼ�
#define MTPRI_ROOT		-12         //ROOT���ȼ�
#define MTPRI_HIGH		-10         //�����ȼ�
#define MTPRI_NORMAL	0			//�����ȼ�
#define MTPRI_LOW		13          //�����ȼ�
#define MTPRI_SYSTEM	MTPRI_HIGH  //Ĭ��ϵͳ�������ȼ�
#define MTPRI_USER		MTPRI_NORMAL//Ĭ���û��������ȼ�
#elif(MOS_UCOSII)//uCOSIIϵͳ
#define MTPRI_WATCHDOG	0			//WATCHDOG���ȼ�
#define MTPRI_ROOT		1			//ROOT���ȼ�
#define MTPRI_HIGH		2			//�����ȼ�
#define MTPRI_NORMAL	5			//�����ȼ�
#define MTPRI_LOW		20          //�����ȼ�
#define MTPRI_SYSTEM	MTPRI_HIGH  //Ĭ��ϵͳ�������ȼ�
#define MTPRI_USER		MTPRI_NORMAL//Ĭ���û��������ȼ�
#elif(MOS_LINUX)
#define MTPRI_WATCHDOG	90			//WATCHDOG���ȼ�
#define MTPRI_ROOT		85			//WATCHDOG���ȼ�
#define MTPRI_HIGH		80			//�����ȼ�
#define MTPRI_NORMAL	50			//�����ȼ�
#define MTPRI_LOW		20          //�����ȼ�
#define MTPRI_SYSTEM	MTPRI_HIGH  //Ĭ��ϵͳ�������ȼ�
#define MTPRI_USER		MTPRI_NORMAL//Ĭ���û��������ȼ�
#endif

///////////////////////////////////////////////////////////////
//	���󷵻���
///////////////////////////////////////////////////////////////
#define TASK_NO_ERR				0	//û�д���
#define TASK_ERR_CREATE			1	
#define TASK_ERR_INVALIDENTRY	2	//��ں�������
#define TASK_ERR_NOPCB			3	//û���㹻�Ŀ��ƿ�
//��ͨ����(��װ����Ҫ�ȴ��ں�ƽ̨��ʼ����ſ����е�����)��ϵͳ����(��װ�������е�����)		
typedef enum{TASK_GEN,TASK_SYS}TASK_ATTR;
// typedef struct _TTKI{
// 	char *iszName;
// 	FV_H ifTaskEntry;
// 	DWORD iPri;
// 	DWORD idwStackSize;
// 	HPARA iArg;
// 	DWORD dwMaxTimeSlice;			//�����ʱ��Ƭ��ms��,0�����ֹ
// 	TASK_ATTR  Attr;				//��������
// }TTKI;
// INT InstallTask(TTKI *pTKI);
// void OS_Run(void);
typedef enum{MCMD_ONTIMEOUT=1}COMMAND_TASK;
// void OS_OnCommand(COMMAND_TASK CMDID,HPARA fFun);
//���õ�ǰ����Ĵ���ֵ
// BOOL OS_SetReg(BYTE byRegNo, DWORD dwRegVal);
// BOOL OS_GetReg(BYTE byRegNo, DWORD *pdwRegVal);
// void OS_Sleep(DWORD dwMs);
//ϵͳ���к�����
// DWORD OS_GetRunTime(void);

// typedef enum{
// 	HK_RESET,
// 	HK_HALF_SEC
// }HOOK_TYPE;


//TRACE��Ϣ���
// INT DF_sprintf(char *buf, char *format, ...);
#ifdef __cplusplus
 #if __cplusplus
	}
 #endif
#endif
#endif
