
#include "appcfg.h"
#include "cgy.h"

static TMPFlagsCtrl gMPFlagsCtrl = {0};

TMPFlagsCtrl *pGetMPFlag(void)
{//所有测量点的标识
	return &gMPFlagsCtrl;
}

void InitMPFlags(void)
{//初始化测量点标识
TMPFlagsCtrl *p=pGetMPFlag();

	memset(p,0,sizeof(TMPFlagsCtrl));

	p->wJCMPNo	= 0xFFFF;
}

BYTE *_pGetPortMPFlag(BYTE byNo)
{//获得端口下测量点有效标识
TMPFlagsCtrl *p=pGetMPFlag();

	return &p->PortMPFlag[byNo][0];
}

///////////////////////////////////////////////////////////////
//	函 数 名 : *pGetPortMPFlag
//	函数功能 : 获得指定端口下测量点有效标识
//	参数说明 : BYTE byPortNo -实际的物理端口
///////////////////////////////////////////////////////////////
BYTE *pGetPortMPFlag(BYTE byPortNo)
{//获得端口下测量点有效标识
BYTE byNo;
	
	byNo	= PortID2No(MDW(MPT_SERIAL,byPortNo,0,0));

	if(byNo < MAX_DWNPORT_NUM)
		return _pGetPortMPFlag(byNo);
	else
		return NULL;
}

