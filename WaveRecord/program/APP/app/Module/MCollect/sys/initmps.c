
#include "appcfg.h"
#include "cgy.h"
extern THisBuf hisDataBuf;

TMetCfg_Base gMetCfg[MAX_MP_NUM];
// ///////////////////////////////////////////////////////////////
// //	�� �� �� : IsMeter
// //	�������� : �жϲ������Ƿ��ǵ��
// //	������� : ���������ж˿��в����Ƿ��иñ�������Ϊ�Ǳ�
// //	��    ע : 
// //	��    �� : jiangjy
// //	ʱ    �� : 2016��5��27��
// //	�� �� ֵ : BOOL
// //	����˵�� : WORD wMPNo
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
	
	//1����ʼ��������ɲ�����
	for(wI=1;wI<MAX_MP_NUM;wI++)	
	{//������Ų���
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
			{//��ʼ���˿ڲ�������Ϣ
				byPortNo = gMetCfg[wI].byPortNo;
				pFlag	= pGetPortMPFlag(byPortNo); 
				if(NULL != pFlag)
					pFlag[wMPNo>>3]		|= (1<<(wMPNo&0x07));
			}	
		}		
	}	
}

///////////////////////////////////////////////////////////////
//	�� �� �� : InitMPs
//	�������� : ��ʼ�����в����㣨������ɡ����塢���������ĳ�ʼ���ݱ�ʶ
///////////////////////////////////////////////////////////////
void InitMPs(void)
{
	TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
	

	//1����ʼ��������ɲ�����
	InitAllFrzCjFlag();
}

WORD GetJCMPNo(void)
{
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();

	return pMPFlagsCtrl->wJCMPNo;
}



