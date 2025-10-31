
#include "appcfg.h"
#include "cgy.h"

static TMPFlagsCtrl gMPFlagsCtrl = {0};

TMPFlagsCtrl *pGetMPFlag(void)
{//���в�����ı�ʶ
	return &gMPFlagsCtrl;
}

void InitMPFlags(void)
{//��ʼ���������ʶ
TMPFlagsCtrl *p=pGetMPFlag();

	memset(p,0,sizeof(TMPFlagsCtrl));

	p->wJCMPNo	= 0xFFFF;
}

BYTE *_pGetPortMPFlag(BYTE byNo)
{//��ö˿��²�������Ч��ʶ
TMPFlagsCtrl *p=pGetMPFlag();

	return &p->PortMPFlag[byNo][0];
}

///////////////////////////////////////////////////////////////
//	�� �� �� : *pGetPortMPFlag
//	�������� : ���ָ���˿��²�������Ч��ʶ
//	����˵�� : BYTE byPortNo -ʵ�ʵ�����˿�
///////////////////////////////////////////////////////////////
BYTE *pGetPortMPFlag(BYTE byPortNo)
{//��ö˿��²�������Ч��ʶ
BYTE byNo;
	
	byNo	= PortID2No(MDW(MPT_SERIAL,byPortNo,0,0));

	if(byNo < MAX_DWNPORT_NUM)
		return _pGetPortMPFlag(byNo);
	else
		return NULL;
}

