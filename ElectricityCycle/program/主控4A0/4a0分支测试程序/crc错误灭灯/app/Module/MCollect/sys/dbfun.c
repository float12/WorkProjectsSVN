///////////////////////////////////////////////////////////////
//	文 件 名 :dbfun.c
//	文件功能 :通用功能
//	作    者 : jiangjy
//	创建时间 : 2015年9月2日
//	项目名称 ：DF6203
//	操作系统 : 
//	备    注 :
//	历史记录： : 
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
//	函 数 名 : InitData
//	函数功能 : 初始化数据区
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2016年2月22日
//	返 回 值 : void
//	参数说明 : void
///////////////////////////////////////////////////////////////
// void InitData(void)
// {
// TFileClean FC;
// TData_6203 *pDF6203=pGetDF6203();
// // BYTE byOldACDCFlag=pDF6203->byACDCFlag;
// BYTE byI;
// DWORD dwTmp=pDF6203->dwLastRunTime;

// 	// gVar.byInitOK	= 0;

// 	//清除FALSH数据!!!!!!!!!（擦除所有数据区0x20000为告警大小，单独清除）
// 	FC.iszName	= FILE_NVRAM;//第一个文件
// 	FC.idwCleanLen = (MAX_DATA_FLASH-NVRAM_OFFSET);
// 	FC.bClearBak= FALSE;
// 	FC.idwOffset= 0;
// 	FileClean(&FC);
// 	ClearCat();
// 	memset(pDF6203,0,sizeof(TData_6203));	
// 	// pDF6203->byACDCFlag		= byOldACDCFlag;
// 	pDF6203->dwLastRunTime	= dwTmp;
// 	//历史数据和小时冻结不补抄	
// 	memset(pDF6203->HourFrzFlag,0xFF,sizeof(pDF6203->HourFrzFlag));
// 	memset(pDF6203->DFrzFlag,0xFF,sizeof(pDF6203->DFrzFlag));
// 	for(byI=0;byI<MAX_DWNPORT_NUM;byI++)
// 	{
// 		pDF6203->HourFrzFlag[byI]	&= ~BIT0;
// 		pDF6203->DFrzFlag[byI][0]	&= ~BIT0;
// 	}

// 	//置位标识
// 	pDF6203->dwImagicB	= IMAGIC_START;
// 	pDF6203->dwImagicE	= IMAGIC_END;
// }

// DWORD GetLastRunMins(void)
// {//获得最近一次运行时间
// TData_6203 *pDF6203=pGetDF6203();

// 	return pDF6203->dwLastRunTime;
// }


// void DB_SaveData2(DWORD dwMins)
// {
// TData_6203 *pDF6203=pGetDF6203();
// DWORD dwTime,dwTmp;
// TTime Time;
	
// 	dwTime	= (dwMins/1440)*1440;
// 	//2、日冻结数据存贮
// 	//时间向前修正了,将当前值保存一下
// 	if(pDF6203->dwLastDSTM > dwTime)
// 	{
// 		pDF6203->dwLastDSTM = dwTime; 		
// 		return;
// 	}	
// 	dwTmp = (dwTime - pDF6203->dwLastDSTM);
// 	if(dwTmp < 1440)
// 		return;
// 	//每天定期存贮整个NVRAM数据,避免因电池没电产生统计数据的过度异常
// 	SaveNvRam_DF6203();

// 	pDF6203->dwLastDSTM	= dwTime;
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : DB_SaveData
//	函数功能 : 数据定期存贮接口
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2016年2月23日
//	返 回 值 : void
//	参数说明 : DWORD dwMins
///////////////////////////////////////////////////////////////
// void DB_SaveData(DWORD dwMins)
// {
// TData_6203 *pDF6203=pGetDF6203();

// //	if(gVar.byInitOK)	//外部调用已判断
// 	{//终端已处于上电状态		

// 		DB_SaveData2(dwMins);
// 		pDF6203->dwLastRunTime	= dwMins;
// 	}	
// }




