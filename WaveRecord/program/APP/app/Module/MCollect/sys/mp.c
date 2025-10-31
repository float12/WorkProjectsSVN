
#include "appcfg.h"
#include "cgy.h"

extern BOOL GetMPAddrEx_698(WORD wMPNo,TSA *sa);

///////////////////////////////////////////////////////////////
//	函 数 名 : GetMPNum
//	函数功能 : 获得指定端口下测量点个数
///////////////////////////////////////////////////////////////
WORD GetMPNum(BYTE byPortNo)
{
BYTE *pFlag=pGetPortMPFlag(byPortNo);
WORD wMPNum=0,wI;

	if(NULL == pFlag)
		return 0;
	for(wI=1;wI<MAX_MP_NUM;wI++)
	{
		if(pFlag[wI>>3] & (1<<(wI&0x07)))
			wMPNum++;
	}	
	return wMPNum;
}

WORD GetAllMeterSum(void)
{//获取集中器内所有表的数量
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
WORD wNum=0,wNo;
	
	for(wNo=1;wNo<MAX_MP_NUM;wNo++)
	{
		if(pMPFlagsCtrl->Flag_Valid[wNo>>3] & (1<<(wNo&0x07)))
			wNum++;
	}
	return wNum;		
}

///////////////////////////////////////////////////////////////
//	函 数 名 : No2MP
//	函数功能 : 根据序号获得测量点号
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年9月15日
//	返 回 值 : BOOL
//	参数说明 : WORD wNo,
//				WORD *pwMPNo
///////////////////////////////////////////////////////////////
BOOL No2MP(WORD wNo,WORD *pwMPNo)
{
	*pwMPNo	= wNo;
	return (wNo != 0);
}

BOOL MP2No(WORD wMPNo,WORD *pwSeq)
{//根据测量点号得到相应的序号

	*pwSeq	= wMPNo;
	return TRUE;
}

int comp_TSA_addr(TSA *s1,TSA *s2)
{
BYTE i;

	if(s1->len > s2->len)
		return 1;
	else if(s1->len < s2->len)
		return -1;
	for(i=0;i<(s1->len+1);i++)
	{
		if(s1->addr[i] > s2->addr[i])
			return 1;
		else if(s1->addr[i] < s2->addr[i])
			return -1;
	}
	return 0;
}

BOOL Addr2MP(BYTE byPortNo,HPARA hAddr,BOOL bDlt645,WORD *pwMPNo)
{//根据地址获得测量点号
TMPFlagsCtrl *pMPFlags=pGetMPFlag();
WORD wI,wMPReqNo;
BYTE byGyPortNo=0xFF; 
	
	for(wI=1;wI<MAX_MP_NUM;wI++)
	{
		if((pMPFlags->Flag_Valid[wI>>3] & (1<<(wI&0x07))) == 0)
			continue;
		//测量点有效
		if(MP2No(wI,&wMPReqNo))
		{
			TMetCfg_Base Cfg;
			if(sys_GetMPCfg(wMPReqNo,&Cfg))
			{
				if(bDlt645)
				{
					if(memcmp(hAddr,Cfg.meter_addr.addr,6) == 0)
					{
						if((Cfg.byPortNo == byGyPortNo)||(byPortNo == 0))
							*pwMPNo	= Cfg.wMPNo;
						return TRUE;
					}	
				}
				else if((Cfg.byPortNo == byGyPortNo)||(byPortNo == 0))
				{
					if(comp_TSA_addr(hAddr,&Cfg.meter_addr) == 0)
					{
						*pwMPNo	= Cfg.wMPNo;
						return TRUE;
					}	
				}	
			}
		}			
	}		
	
	return FALSE;
}

BOOL MP_IsValid(WORD wMPNo)
{//测量点是否有效
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
	
	if((wMPNo == 0)||( wMPNo>= MAX_MP_NUM))
		return FALSE;
	return ((pMPFlagsCtrl->Flag_Valid[wMPNo>>3] & (1<<(wMPNo&0x07))) != 0);
}

///////////////////////////////////////////////////////////////
//	函 数 名 : GetMPAddr
//	函数功能 : 获得指定序号测量点地址

///////////////////////////////////////////////////////////////
BOOL GetMPAddr(WORD wMPSeqNo,BYTE *pAddr)
{
TMetCfg_Base Cfg;
BOOL bRc=FALSE;
// #if(MD_BASEUSER	== MD_GDW698)
// 	if(wMPSeqNo >= MAX_MP_NUM)
// 		bRc	= GetMPAddrEx_698(wMPSeqNo,(TSA*)pAddr);
// 	else
// #endif
	if(sys_GetMPCfg(wMPSeqNo,&Cfg))
	{
		bRc	= TRUE;		
		memcpy(pAddr,&Cfg.meter_addr,sizeof(TSA));		
	}
	return bRc;
}
