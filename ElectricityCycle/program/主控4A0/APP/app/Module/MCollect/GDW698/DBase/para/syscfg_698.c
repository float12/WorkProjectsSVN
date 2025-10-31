
#include "appcfg.h"
#if(MD_BASEUSER	== MD_GDW698)
#include "../../GDW698.h"
#include "cgy.h"

extern void _freshPara(WORD flag);

// void _GetProDefaultSerCfg(BYTE byDrvNo,TSerCfg *pSerCfg)
// {//���ͨ��Э������Ĵ���������Ϣ
// 	memset(pSerCfg,0,sizeof(TSerCfg));
// 	switch(byDrvNo)
// 	{
// 	case 1: //645_1997
// 		pSerCfg->byCharSize	= 8;
// 		pSerCfg->byFlags	= 0x20;
// 		pSerCfg->dwBaud		= 1200;
// 		break;
// 	default:
// 	case 2://645_2007
// 		pSerCfg->byCharSize	= 8;
// 		pSerCfg->byFlags	= 0x20;
// 		pSerCfg->dwBaud		= 2400;
// 		break;
// 	case 3: //698.45
// 		pSerCfg->byCharSize	= 8;
// 		pSerCfg->byFlags	= 0x20;
// 		pSerCfg->dwBaud		= 9600;
// 		break;
// 	}
// }

//todo 
void No2Baud(BYTE *pbyBaudNo,DWORD *pdwBaud,BOOL bDirect)
{
	if(bDirect)
	{
		switch(*pbyBaudNo)
		{
		case 0:	*pdwBaud	= 300;		break;
		case 1: *pdwBaud	= 600;		break;
		case 2: *pdwBaud	= 1200;		break;
		default:
		case 3: *pdwBaud	= 2400;		break;
		case 4: *pdwBaud	= 4800;		break;
		case 5: *pdwBaud	= 7200;		break;
		case 6: *pdwBaud	= 9600;		break;
		case 7: *pdwBaud	= 19200;	break;
		case 8: *pdwBaud	= 38400;	break;
		case 9: *pdwBaud	= 57600;	break;
		case 10:*pdwBaud	= 115200;	break; 
		//can	
		case 13:*pdwBaud	= 10000;	break; 	
		case 14:*pdwBaud	= 25000;	break; 	
		case 15:*pdwBaud	= 50000;	break; 	
		case 16:*pdwBaud	= 125000;	break; 	
		}	
	}
	else
	{
		switch(*pdwBaud)
		{
		case 300:	*pbyBaudNo	= 0;	break;
		case 600: *pbyBaudNo	= 1;	break;
		case 1200: *pbyBaudNo	= 2;	break;
		default:
		case 2400: *pbyBaudNo	= 3;	break;
		case 4800: *pbyBaudNo	= 4;	break;
		case 7200: *pbyBaudNo	= 5;	break;
		case 9600: *pbyBaudNo	= 6;	break;
		case 19200: *pbyBaudNo	= 7;	break;
		case 38400: *pbyBaudNo	= 8;	break;
		case 57600: *pbyBaudNo	= 9;	break;
		case 115200:*pbyBaudNo	= 10;	break; 	
		//can
		case 10000:*pbyBaudNo	= 13;	break; 	
		case 25000:*pbyBaudNo	= 14;	break; 	
		case 50000:*pbyBaudNo	= 15;	break; 	
		case 125000:*pbyBaudNo	= 16;	break; 	
		}	
	}
}

BOOL IsJCMP(TMetCfg_Base *pCfg)
{
	if(pCfg->byPortNo == 0xFF)
		return TRUE;
	return FALSE;
}

/**************************************************************
 *	�� �� �� : sys_SetStatus
 *	�������� : ϵͳ״̬Ͷ��
 *	������� : 
 *	��    ע : 
 *	��    �� : jjy
 *	ʱ    �� : 2007��12��4��
 *	�� �� ֵ : void
 *	����˵�� : DWORD dwNo,
//				0--����Ͷ��
//				1--�߷Ѹ澯Ͷ��
//				2--�����ն�����վͨ��
//				3--�ն��޳�Ͷ��
//				4--�����ն������ϱ�
//				5--�ն�Ͷ������
//				6--�Զ����磨������,ֻ�������
//				7--��ȫģʽ����
//				8--�����ն˸����ϱ�
//				9--�����ն˲����ն���
//				DWORD dwArg0
**************************************************************/
// void sys_SetStatus(DWORD dwNo,DWORD dwArg0)
// {
// TsysCfgOther *pSys=pGetSysCfgOther();

// 	if(dwNo == 1)
// 	{
// 		DWORD *pPara=(DWORD*)dwArg0;		
// 		BYTE *pstr;
// 		pSys->CFHourFlag[0]	= LLBYTE(pPara[0]);
// 		pSys->CFHourFlag[1]	= LHBYTE(pPara[0]);
// 		pSys->CFHourFlag[2]	= HLBYTE(pPara[0]);
// 		pstr	= (BYTE*)pPara[1];
// 		if(pstr[0] > (sizeof(pSys->Info)-1))
// 			pstr[0]	= (BYTE)(sizeof(pSys->Info)-1);
// 		memcpy(&pSys->Info[0],&pstr[0],pstr[0]+1);
// 	}
// 	pSys->wFlag_Ctrl	|= (1<<dwNo);
// 	_freshPara(FLAG_SYS);		
// }

// void sys_ClrStatus(DWORD dwNo,BYTE byFlag)
// {
// TsysCfgOther *pSys;

// 	pSys = pGetSysCfgOther();
// 	pSys->wFlag_Ctrl	&= ~(1<<dwNo);		
// 	_freshPara(FLAG_SYS);
// }

// void ComDcb2SerCfg(ComDCB *commdcb,TSerCfg *pserCfg)
// {
// 	pserCfg->byCharSize	= commdcb->databits;
// 	if(commdcb->parity == 0)
// 		pserCfg->byFlags	= 0;
// 	else if(commdcb->parity == 1)
// 		pserCfg->byFlags	= 0x10;
// 	else
// 		pserCfg->byFlags	= 0x20;
// 	No2Baud(&commdcb->baud,&pserCfg->dwBaud,TRUE);
// }

BOOL OAD2PortNo(DWORD *pdwOAD,BYTE *pbyPortNo,BOOL bDirect)
{	
BYTE *pOAD=(BYTE *)(pdwOAD);
DWORD oad= MDW(pOAD[3],pOAD[2],pOAD[1],pOAD[0]);
	
	if(bDirect)
	{
		switch(oad)
		{		
		case 0xF2010201: *pbyPortNo	= 1; break;	//485��1	
			
		case 0xF2090200: 	
		case 0xF2090201: *pbyPortNo	= 2; break; //�ز���
			
        // case 0xF20B0200:
		// case 0xF20B0201: *pbyPortNo = 3;  break; //����

		case 0xF2020200:
		case 0xF2020201: *pbyPortNo = 3;  break; //����
			
		case 0xF2210200: 			
		case 0xF2210201: *pbyPortNo	= 4; break; //CAN
		default: return FALSE;
		}
	}
	else
	{
		switch(*pbyPortNo)
		{		
		case 1:  *pdwOAD	= 0xF2010201; break;	
		case 2:  *pdwOAD	= 0xF2090201; break;		
		// case 3:  *pdwOAD	= 0xF20B0201; break;			
		case 3:  *pdwOAD	= 0xF2020201; break;
		case 4:  *pdwOAD	= 0xF2210201; break;	
		default: return FALSE;
		}
	}	
	return TRUE;
}
#endif

