
#include "appcfg.h"
#include "cgy.h"

extern BOOL GetMPAddrEx_698(WORD wMPNo,TSA *sa);

///////////////////////////////////////////////////////////////
//	�� �� �� : GetMPNum
//	�������� : ���ָ���˿��²��������
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
{//��ȡ�����������б������
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
//	�� �� �� : No2MP
//	�������� : ������Ż�ò������
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��15��
//	�� �� ֵ : BOOL
//	����˵�� : WORD wNo,
//				WORD *pwMPNo
///////////////////////////////////////////////////////////////
BOOL No2MP(WORD wNo,WORD *pwMPNo)
{
	*pwMPNo	= wNo;
	return (wNo != 0);
}

BOOL MP2No(WORD wMPNo,WORD *pwSeq)
{//���ݲ�����ŵõ���Ӧ�����

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
{//���ݵ�ַ��ò������
TMPFlagsCtrl *pMPFlags=pGetMPFlag();
WORD wI,wMPReqNo;
BYTE byGyPortNo=0xFF; 
	
	for(wI=1;wI<MAX_MP_NUM;wI++)
	{
		if((pMPFlags->Flag_Valid[wI>>3] & (1<<(wI&0x07))) == 0)
			continue;
		//��������Ч
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
{//�������Ƿ���Ч
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
	
	if((wMPNo == 0)||( wMPNo>= MAX_MP_NUM))
		return FALSE;
	return ((pMPFlagsCtrl->Flag_Valid[wMPNo>>3] & (1<<(wMPNo&0x07))) != 0);
}

///////////////////////////////////////////////////////////////
//	�� �� �� : GetMPAddr
//	�������� : ���ָ����Ų������ַ

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
