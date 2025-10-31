
#include "appcfg.h"
#if(MD_BASEUSER	== MD_GDW698)
#include "../../GDW698.h"
#include "cgy.h"

extern void _freshPara(WORD flag);

// void _GetProDefaultSerCfg(BYTE byDrvNo,TSerCfg *pSerCfg)
// {//获得通信协议基本的串口配置信息
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
 *	函 数 名 : sys_SetStatus
 *	函数功能 : 系统状态投入
 *	处理过程 : 
 *	备    注 : 
 *	作    者 : jjy
 *	时    间 : 2007年12月4日
 *	返 回 值 : void
 *	参数说明 : DWORD dwNo,
//				0--保电投入
//				1--催费告警投入
//				2--允许终端与主站通话
//				3--终端剔除投入
//				4--允许终端主动上报
//				5--终端投入运行
//				6--自动保电（无设置,只有清除）
//				7--安全模式启用
//				8--允许终端跟随上报
//				9--允许终端补报日冻结
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
		case 0xF2010201: *pbyPortNo	= 1; break;	//485口1	
			
		case 0xF2090200: 	
		case 0xF2090201: *pbyPortNo	= 2; break; //载波口
			
        // case 0xF20B0200:
		// case 0xF20B0201: *pbyPortNo = 3;  break; //蓝牙

		case 0xF2020200:
		case 0xF2020201: *pbyPortNo = 3;  break; //红外
			
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

