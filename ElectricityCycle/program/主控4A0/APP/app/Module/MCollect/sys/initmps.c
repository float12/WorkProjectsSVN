
#include "appcfg.h"
#include "cgy.h"
extern THisBuf hisDataBuf;

TMetCfg_Base gMetCfg[MAX_MP_NUM];
// ///////////////////////////////////////////////////////////////
// //	函 数 名 : IsMeter
// //	函数功能 : 判断测量点是否是电表
// //	处理过程 : 从所有下行端口中查找是否有该表，有则认为是表
// //	备    注 : 
// //	作    者 : jiangjy
// //	时    间 : 2016年5月27日
// //	返 回 值 : BOOL
// //	参数说明 : WORD wMPNo
// ///////////////////////////////////////////////////////////////
// BOOL IsMeter(WORD wMPNo)
// {
// BYTE *pFlag;
// BYTE byI;

// 	if((wMPNo == GetJCMPNo())
//         ||(wMPNo == 0)
// 		||(wMPNo >= MAX_MP_NUM))
// 		return FALSE;
// 	for(byI=0;byI<MAX_DWNPORT_NUM;byI++)
// 	{
// 		pFlag	= _pGetPortMPFlag(byI);
// 		if(pFlag[wMPNo>>3] & (1<<(wMPNo&0x07)))
// 			return TRUE;
// 	}
// 	return FALSE;
// }

void InitAllFrzCjFlag(void)
{
	TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
	TMetCfg_Base MetCfg;
	BYTE *pFlag;
	WORD wI,wMPNo;
	BYTE byPortNo;
	BOOL bRc;
	
	//1、初始化电表、交采测量点
	for(wI=1;wI<MAX_MP_NUM;wI++)	
	{//根据序号查找
		bRc		= sys_InitMPCfg(wI,&gMetCfg[wI]);
		wMPNo	= gMetCfg[wI].wMPNo;
		
		if(bRc && (wMPNo != 0 )&&(wMPNo < MAX_MP_NUM))
		{		
			pMPFlagsCtrl->Flag_Valid[wMPNo>>3]	|= 1<<(wMPNo&0x07);
			
			if(IsJCMP(&gMetCfg[wI]))
			{
				if(pMPFlagsCtrl->wJCMPNo >= MAX_MP_NUM)
				{
					pMPFlagsCtrl->wJCMPNo	= wMPNo;
				}			
			}	
			else 
			{//初始化端口测量点信息
				byPortNo = gMetCfg[wI].byPortNo;
				pFlag	= pGetPortMPFlag(byPortNo); 
				if(NULL != pFlag)
					pFlag[wMPNo>>3]		|= (1<<(wMPNo&0x07));
			}	
		}		
	}	
}

///////////////////////////////////////////////////////////////
//	函 数 名 : InitMPs
//	函数功能 : 初始化所有测量点（电表、交采、脉冲、运算量）的初始数据标识
///////////////////////////////////////////////////////////////
void InitMPs(void)
{
	TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
	

	//1、初始化电表、交采测量点
	InitAllFrzCjFlag();
}

WORD GetJCMPNo(void)
{
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();

	return pMPFlagsCtrl->wJCMPNo;
}



