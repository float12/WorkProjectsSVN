#include "appcfg.h"

// TData_6203 *gpData_6203;
extern void CheckClearFrzBlock(BYTE byType,DWORD dwNewTime);
extern void CheckClearCurvBlock(DWORD dwNewTime);
extern void InitGDW698Cfg(void);

// #define DF6203_SIZE ( ((sizeof(TData_6203)+SIZE_PAGE-1)/SIZE_PAGE)*SIZE_PAGE )

// BYTE df_DataBuffer[DF6203_SIZE];

// static void _InitSysBaseCfg(void)
// {
	// TVar *pVar=&gVar;

//	memset(&pVar->Cfg,0,sizeof(TERomSysCfg));
//	
//	pVar->Cfg.byUserType	= MAR_STANDARD;	//�û�����
	
// #if(!MOS_MSVC)		
	// pVar->Cfg.byHaveESAM	= 0x55;		//��ESAM
// #endif
//	pVar->Cfg.byCurvSource	= 0x55;
	// pVar->Cfg.wDefaultJCMPNo	= 1;
	//������������
	// pVar->Cfg.byJCType		= 0x34;		
	
	//ȷ��645-97��07��Լ�������
	// 	pVar->Cfg.GyNo[17][0]	= 10;			//645-1997
	// 	pVar->Cfg.GyNo[17][1]	= 1;			//��վ��Լ��645-1997
	// 	pVar->Cfg.GyNo[18][0]	= 15;			//645_2007
	// 	pVar->Cfg.GyNo[18][1]	= 2;			//��վ��Լ��645_2007
	// 	pVar->Cfg.GyNo[19][0]	= 16;			//698.45
	// 	pVar->Cfg.GyNo[19][1]	= 3;			//��վ��Լ��698.45

// }

// TData_6203 *pGetDF6203(void)
// {
// 	return gpData_6203;
// }

static void _HooK_Reset(void)
{
	//������ʷ�������е����ݣ����¶��ᡢʵʱ����ʷ���߶��ᣩ
	// FrzBuf2File(REQ_DFRZ);
	// FrzBuf2File(REQ_MFRZ);
	// FrzBuf2File(REQ_CURV);
	// FrzBuf2File(REQ_SETTLEDAY);
	//��������
	ParaSaveDaemon(FALSE,NULL);
	// gpData_6203->dwImagicB = IMAGIC_START;
	// gpData_6203->dwImagicE = IMAGIC_END;
	// //����NVRAM����
	// SaveNvRam_DF6203();	
}

//todo �����λʱ�����
void api_PowerDownSave(void)
{
	_HooK_Reset();
}

// void SaveNvRam_DF6203(void)
// {
// TData_6203 *pDF6203=pGetDF6203();
// TFileWrite FW;

// 	pDF6203->dwImagicB = IMAGIC_START;
// 	pDF6203->dwImagicE = IMAGIC_END;
// 	//4������NVRAM����
// 	//�����Ӧ�ĵ����ݣ�NANDFLASH���Ȳ���д��ͳһ����������ֱ��д��������Ч�ʣ�
// #if MOS_UCOSII
// 	{
// 		TFileClean FC;
// 		FC.iszName	= FILE_NVRAM;
// 		FC.bClearBak= FALSE;
// 		//�������
// 		FC.idwCleanLen = (((sizeof(TData_6203)+SIZE_FLASH_BLOCK-1)/SIZE_FLASH_BLOCK)*SIZE_FLASH_BLOCK);
// 		FC.idwOffset= 0;
// 		FileClean(&FC);
// 	} 	
// #endif
// 	//����NVRAM���ݵ�FLASH��
// 	FW.iszName	= FILE_NVRAM;
// 	FW.idwOffset= 0;	
// 	//�ײ�Ķ�д��ҳ�ķ�ʽ����
// 	FW.idwBufLen= (((sizeof(TData_6203)+SIZE_PAGE-1)/SIZE_PAGE)*SIZE_PAGE);
// 	FW.ipvBuf	= pDF6203;
// 	FileWrite(&FW);
// }

// void InitDF6203(void)
// {//������λ����(��λʱ�������Ӧ�ĺ���)	
// DWORD dwFlashSaveSize=((sizeof(TData_6203)+SIZE_PAGE-1)/SIZE_PAGE)*SIZE_PAGE;
// BYTE byBootType;

// 	//gpData_6203		= NvAlloc(dwFlashSaveSize);//����������ҳ
//     gpData_6203 = (TData_6203*)&df_DataBuffer[0];
// 	if((gpData_6203->dwImagicB != IMAGIC_START)
// 		||(gpData_6203->dwImagicE != IMAGIC_END))
// 	{//�����˵�����Ϊ
// 		TFileRead FR;		
// 		FR.iszName	= FILE_NVRAM;
// 		FR.idwOffset= 0;
// 		FR.ipvBuf	= gpData_6203;
// 		FR.idwBufLen= dwFlashSaveSize;
// 		FileRead(&FR);	
// 	#if(!MOS_MSVC)	
// 		if((gpData_6203->dwImagicB != IMAGIC_START)
// 			||(gpData_6203->dwImagicE != IMAGIC_END))
// 	#endif
// 		{	
// 			memset(gpData_6203,0,sizeof(TData_6203));
// 		}	
// 	}
// 	else
// 	// gVar.byStart_reset	= 1;	//��ǰ����Ϊ�ն˸�λ(NVRAMû�б仯)
// 	gpData_6203->dwImagicB	= IMAGIC_START;
// 	gpData_6203->dwImagicE	= IMAGIC_END;

// 	gpData_6203->dwLastRunTime	= gVar.dwStartTime/60;
// 	Min2Time(gpData_6203->dwLastRunTime,&gVar.GyTime);

// 	//�Գ���ͣ�˺��������������Ӧ�����ݿ�
// 	//1����ʼ�����������
// 	CheckClearFrzBlock(REQ_DFRZ,gpData_6203->dwLastRunTime);
// 	CheckClearFrzBlock(REQ_MFRZ,gpData_6203->dwLastRunTime);
// 	//2�����������߿�
// 	CheckClearCurvBlock(gpData_6203->dwLastRunTime);
// }

void InitSysCfg(void)
{
//	InitDF6203();

	// _InitSysBaseCfg();

	InitGDW698Cfg();
}

///////////////////////////////////////////////////////////////
//	�� �� �� : PortID2No
//	�������� : ���ݶ˿�ID���ʵ�ʵ��ڴ��еĶ˿����
//	������� : 
//	��    ע : (ֻ�����г����)
//	�� �� ֵ : BYTE
//	����˵�� : DWORD dwPortID
///////////////////////////////////////////////////////////////
BYTE PortID2No(DWORD dwPortID)
{//���ݶ˿�ID���ʵ�ʵ��ڴ��еĶ˿����
	if(HHBYTE(dwPortID) == MPT_SERIAL)
	{
		BYTE port_no=(BYTE)(HLBYTE(dwPortID)-1);
		
		return port_no;
	}
	return 0;
}

