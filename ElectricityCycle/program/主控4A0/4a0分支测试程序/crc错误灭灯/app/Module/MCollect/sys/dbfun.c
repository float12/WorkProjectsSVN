///////////////////////////////////////////////////////////////
//	�� �� �� :dbfun.c
//	�ļ����� :ͨ�ù���
//	��    �� : jiangjy
//	����ʱ�� : 2015��9��2��
//	��Ŀ���� ��DF6203
//	����ϵͳ : 
//	��    ע :
//	��ʷ��¼�� : 
///////////////////////////////////////////////////////////////
#include "appcfg.h"
#include "cgy.h"
#include "stdlib.h"
#include "AllHead.h"
// extern void MeterTimeCheck(WORD wMPNo,long nTime);
extern void ClearCat(void);
// extern void InitDase698Ex(void);
extern void SaveEnergy2EEProm(void);
extern void InitNewFrzBlock(BYTE byType,DWORD dwTime);

THisBuf hisDataBuf;
TSysParaCfg *gpSysPara;

// BOOL MPTime2Time(TMPTime *pMPTime,TTime *pTime)
// {
// 	pTime->wYear= (WORD)(pMPTime->Buf[5]+2000);
// 	pTime->Mon	= pMPTime->Buf[4];
// 	pTime->Day	= pMPTime->Buf[3];
// 	pTime->Hour	= pMPTime->Buf[2];
// 	pTime->Min	= pMPTime->Buf[1];
// 	pTime->Sec	= pMPTime->Buf[0];
// 	return TRUE;
// }

// void InitDase(void)
// {
// 	InitDase698Ex();
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : InitData
//	�������� : ��ʼ��������
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2016��2��22��
//	�� �� ֵ : void
//	����˵�� : void
///////////////////////////////////////////////////////////////
// void InitData(void)
// {
// TFileClean FC;
// TData_6203 *pDF6203=pGetDF6203();
// // BYTE byOldACDCFlag=pDF6203->byACDCFlag;
// BYTE byI;
// DWORD dwTmp=pDF6203->dwLastRunTime;

// 	// gVar.byInitOK	= 0;

// 	//���FALSH����!!!!!!!!!����������������0x20000Ϊ�澯��С�����������
// 	FC.iszName	= FILE_NVRAM;//��һ���ļ�
// 	FC.idwCleanLen = (MAX_DATA_FLASH-NVRAM_OFFSET);
// 	FC.bClearBak= FALSE;
// 	FC.idwOffset= 0;
// 	FileClean(&FC);
// 	ClearCat();
// 	memset(pDF6203,0,sizeof(TData_6203));	
// 	// pDF6203->byACDCFlag		= byOldACDCFlag;
// 	pDF6203->dwLastRunTime	= dwTmp;
// 	//��ʷ���ݺ�Сʱ���᲻����	
// 	memset(pDF6203->HourFrzFlag,0xFF,sizeof(pDF6203->HourFrzFlag));
// 	memset(pDF6203->DFrzFlag,0xFF,sizeof(pDF6203->DFrzFlag));
// 	for(byI=0;byI<MAX_DWNPORT_NUM;byI++)
// 	{
// 		pDF6203->HourFrzFlag[byI]	&= ~BIT0;
// 		pDF6203->DFrzFlag[byI][0]	&= ~BIT0;
// 	}

// 	//��λ��ʶ
// 	pDF6203->dwImagicB	= IMAGIC_START;
// 	pDF6203->dwImagicE	= IMAGIC_END;
// }

// DWORD GetLastRunMins(void)
// {//������һ������ʱ��
// TData_6203 *pDF6203=pGetDF6203();

// 	return pDF6203->dwLastRunTime;
// }


// void DB_SaveData2(DWORD dwMins)
// {
// TData_6203 *pDF6203=pGetDF6203();
// DWORD dwTime,dwTmp;
// TTime Time;
	
// 	dwTime	= (dwMins/1440)*1440;
// 	//2���ն������ݴ���
// 	//ʱ����ǰ������,����ǰֵ����һ��
// 	if(pDF6203->dwLastDSTM > dwTime)
// 	{
// 		pDF6203->dwLastDSTM = dwTime; 		
// 		return;
// 	}	
// 	dwTmp = (dwTime - pDF6203->dwLastDSTM);
// 	if(dwTmp < 1440)
// 		return;
// 	//ÿ�춨�ڴ�������NVRAM����,��������û�����ͳ�����ݵĹ����쳣
// 	SaveNvRam_DF6203();

// 	pDF6203->dwLastDSTM	= dwTime;
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : DB_SaveData
//	�������� : ���ݶ��ڴ����ӿ�
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2016��2��23��
//	�� �� ֵ : void
//	����˵�� : DWORD dwMins
///////////////////////////////////////////////////////////////
// void DB_SaveData(DWORD dwMins)
// {
// TData_6203 *pDF6203=pGetDF6203();

// //	if(gVar.byInitOK)	//�ⲿ�������ж�
// 	{//�ն��Ѵ����ϵ�״̬		

// 		DB_SaveData2(dwMins);
// 		pDF6203->dwLastRunTime	= dwMins;
// 	}	
// }




