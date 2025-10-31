///////////////////////////////////////////////////////////////
//	文 件 名 :MGDW698.c
//	文件功能 :698客户端(整包下发(发送缓冲区小于对方的最大接收缓冲区)和分帧下发)
//	作    者 : 蒋剑跃
//	创建时间 : 2017年6月27日
//	项目名称 ：DF62XX
//	操作系统 : 
//	备    注 : 采集一个测量点的所有任务数据,任务的最小判断时间为1分钟,当前未实现链路层分帧
//	历史记录： : 
///////////////////////////////////////////////////////////////
#include "../GDW698.h"
#include "MGDW698.h"
#include "AllHead.h"
#include "Dlt698_45.h"
#include "MCollect.h"
#if(MD_BASEUSER	== MD_GDW698)
extern HPARA GetMGyApp(void);
// extern void write_oad_data(DWORD oadd,BOOL bErr,BYTE *pData);
// extern void write_oad_dataEx(DWORD oadd,BYTE byClom,BOOL bErr,BYTE *pData);
extern void fresh_task_status(TPort *pPort,BYTE byNo,WORD wNo,BOOL bOK);


static BOOL _SwitchTo698Addr(TSA *ssa)
{
BYTE i,byAdrLen,byAddrType=(BYTE)((ssa->AF&0xC0)>>6);
TSA mp_sa;
BOOL bRc=FALSE;
	
	switch(byAddrType)
	{
	case AF_Broadcast	: 
	case AF_Group		: 
	case AF_Adapter		: break;
	default				: //单地址		
		GetMP698Addr(&mp_sa);
		byAdrLen	= (BYTE)((mp_sa.AF&0x0F)+1);
		if(byAdrLen == 6)
		{
			for(i=0;i<6;i++)
			{
				if(mp_sa.addr[i] != 0x88)
					break;
			}
			if(i >= 6)
				return TRUE;
		}
		for(i=0;i<byAdrLen;i++)
		{
			if(mp_sa.addr[i] != 0xAA)
				break;
		}
		if(i >= byAdrLen)
			return TRUE;		
		if(comp_TSA_addr(ssa,&mp_sa) == 0)		
			bRc	= TRUE;
		break;
	}
	return bRc;
}

WORD get_OAD_data_len(BYTE *pAPDU)
{
WORD wRealLen=0;
BYTE choice;
	
	wRealLen+= objGetDataLen(dt_OAD,&pAPDU[wRealLen],MAX_NUM_NEST);
	choice	= pAPDU[wRealLen++];
	if(choice == 0)
		wRealLen+= objGetDataLen(dt_DAR,&pAPDU[wRealLen],MAX_NUM_NEST);
	else if(choice == 1)
		wRealLen+= objGetDataLen((OBJ_DATA_TYPE)pAPDU[wRealLen],&pAPDU[wRealLen+1],MAX_NUM_NEST)+1;
	else
		wRealLen= 0xFFFF;	
	return wRealLen;
}

WORD get_OADs_data_len(BYTE *pAPDU)
{
WORD num,wRealLen=0,i,tmp;

	wRealLen	+= gdw698Buf_Num(&pAPDU[wRealLen],&num,TD12);
	for(i=0;i<num;i++)
	{
		tmp	= get_OAD_data_len(&pAPDU[wRealLen]);
		if(tmp == 0xFFFF)
			return 0xFFFF;	
		else
			wRealLen	+= tmp;
	}
	return wRealLen;
}

WORD get_REC_data_len(BYTE *pAPDU)
{
WORD wRealLen=0,i,num,j,wCSDNum;
BYTE choice;

	wRealLen+= objGetDataLen(dt_OAD,&pAPDU[wRealLen],MAX_NUM_NEST);
	wCSDNum	= get_csd_num_of_RCSD(&pAPDU[wRealLen]);
	wRealLen+= objGetDataLen(dt_RCSD,&pAPDU[wRealLen],MAX_NUM_NEST);
	choice	= pAPDU[wRealLen++];
	if(choice == 0)
		wRealLen+= objGetDataLen(dt_DAR,&pAPDU[wRealLen],MAX_NUM_NEST);
	else if(choice == 1)
	{
		wRealLen	+= gdw698Buf_Num(&pAPDU[wRealLen],&num,TD12);
		if(num>100) num =100;
		for(i=0;i<num;i++)
		{
			for(j=0;j<wCSDNum;j++)
				wRealLen+= objGetDataLen((OBJ_DATA_TYPE)pAPDU[wRealLen],&pAPDU[wRealLen+1],MAX_NUM_NEST)+1;				
		}	
	}
	else
		wRealLen= 0xFFFF;	
	return wRealLen;
}

WORD get_RECS_data_len(BYTE *pAPDU)
{
WORD wRealLen=0,i,num,tmp;

	wRealLen	+= gdw698Buf_Num(&pAPDU[wRealLen],&num,TD12);
	for(i=0;i<num;i++)
	{
		tmp	= get_REC_data_len(&pAPDU[wRealLen]);
		if(tmp == 0xFFFF)
		{
			wRealLen= 0xFFFF;	
			break;
		}
		else
			wRealLen	+= tmp;
	}
	return wRealLen;
}

BOOL Get_GetReponse_APDUlen(BYTE *pAPDU,WORD wLen)
{
BYTE choice;
DWORD dwRealLen=0;

	choice=pAPDU[dwRealLen++];
	dwRealLen++;
	switch(choice)
	{
	case 1:	dwRealLen	+= get_OAD_data_len(&pAPDU[dwRealLen]);	break;
	case 2:	dwRealLen	+= get_OADs_data_len(&pAPDU[dwRealLen]);break;	
	case 3: dwRealLen	+= get_REC_data_len(&pAPDU[dwRealLen]);	break;
	case 4: dwRealLen	+= get_RECS_data_len(&pAPDU[dwRealLen]);break;
	case 5:	//
		dwRealLen	+= objGetDataLen(dt_boolean,&pAPDU[dwRealLen],MAX_NUM_NEST);
		dwRealLen	+= objGetDataLen(dt_long_unsigned,&pAPDU[dwRealLen],MAX_NUM_NEST);
		choice	= pAPDU[dwRealLen++];
		if(choice == 0)
			dwRealLen	+= objGetDataLen(dt_DAR,&pAPDU[dwRealLen],MAX_NUM_NEST);
		else if(choice == 1)
		{
			dwRealLen	+= get_OADs_data_len(&pAPDU[dwRealLen]);					
		}
		else if(choice == 2)
		{
			dwRealLen	+= get_RECS_data_len(&pAPDU[dwRealLen]);			
		}
		else
			dwRealLen	+= 0xFFFF;	
		break;
	default: 	
		dwRealLen	+= 0xFFFF;
		break;
	}
	if(dwRealLen < wLen)
	{//跟随上报域		
		choice	= pAPDU[dwRealLen++];
		switch(choice)
		{
		case 1:	dwRealLen	+= get_OADs_data_len(&pAPDU[dwRealLen]);	break;
		case 2:	dwRealLen	+= get_RECS_data_len(&pAPDU[dwRealLen]);	break;
		case 0:	break;
		default:	dwRealLen	+= 0xFFFF;		break;
		}			
	}
	if(dwRealLen < wLen)
	{//时间域
		choice	= pAPDU[dwRealLen++];
		if(choice)
		{
			dwRealLen	+= objGetDataLen(dt_datetime_s,&pAPDU[dwRealLen],MAX_NUM_NEST);
			dwRealLen	+= objGetDataLen(dt_TI,&pAPDU[dwRealLen],MAX_NUM_NEST);
		}		
	}
	return TRUE;//(dwRealLen == wLen);
}

BOOL Get_SecurityReponse_APDUlen(BYTE *pAPDU,WORD wLen)
{
DWORD dwRealLen=0;
WORD wNum;
BYTE choice;

	choice=pAPDU[dwRealLen++];
	switch(choice)
	{
	case 0:	
	case 1:	
		dwRealLen	+= gdw698Buf_Num(&pAPDU[dwRealLen],&wNum,TD12);
		dwRealLen	+= wNum;		
		break;
	case 2:	
		dwRealLen	+= objGetDataLen(dt_DAR,&pAPDU[dwRealLen],MAX_NUM_NEST);
		break;
	default: 	
		dwRealLen	+= 0xFFFF;
		break;
	}	
	if(pAPDU[dwRealLen++])
	{
		dwRealLen++;
		dwRealLen	+= objGetDataLen(dt_MAC,&pAPDU[dwRealLen],MAX_NUM_NEST);
	}	
	return (dwRealLen == wLen);
}

BOOL GetReponseAPDUDatalen(BYTE *pAPDU,WORD wLen)
{
BOOL bRc=FALSE;
	
	if(pAPDU[0] == GET_Response)
		bRc	= Get_GetReponse_APDUlen(&pAPDU[1],(WORD)(wLen-1));
	else if(pAPDU[0] == Security_Response)
		bRc	= Get_SecurityReponse_APDUlen(&pAPDU[1],(WORD)(wLen-1));
	else if(pAPDU[0] == ACTION_Response)
		bRc = TRUE;
	return bRc;
}

static DWORD gdw698M_SearchOneFrame(HPARA hPort,HPARA hBuf,DWORD dwLen)
{//搜索有效帧
TPort *pPort=(TPort*)hPort;
TFrame698_FULL_BASE afnBase;
DWORD dwRc;
TSA sa;

	dwRc	= gdw698_SearchOneFrame(CTRL_DIR,pPort->Rx.wSize,hBuf,dwLen);
	if((dwRc&FM_SHIELD) == FM_OK)
	{
		objSmockRead( &afnBase,(BYTE*)hBuf );
		gdw698bufLink_TSA(&afnBase.pFrame->byAddrF,&sa,TD12);
		if(!_SwitchTo698Addr(&sa))
			dwRc	= (FM_ERR | LOWORD(dwRc));
		else
		{
			WORD wI,wAPDUSize;
			BYTE *pDataAPDU;
			wAPDUSize	= objGetAPDUSize(&afnBase);	
			pDataAPDU	= afnBase.pDataAPDU;
			if(afnBase.pFrame->byCtrl & CTRL_SC)
			{//扰码格式
				for(wI=0;wI<wAPDUSize;wI++)
					pDataAPDU[wI]	= (BYTE)(pDataAPDU[wI]-0x33);
			}	
			if(!GetReponseAPDUDatalen(pDataAPDU,wAPDUSize))
				dwRc	= (FM_ERR | LOWORD(dwRc));
		}
	}
	return dwRc;
}

WORD _TxFm698Read(TSA *sa,TGetPara *pPara,BYTE *pBuf)
{
	WORD wLen = 0,wAPDULen=0,wOffset=0,wI = 0;
	BYTE Head[10] = {0},randNumLen = 0;
	BYTE *pAPDU = NULL;

	if((sa == NULL)
	||(pPara == NULL)
	||(pBuf == NULL))
	{
		wLen = 0;
		return wLen;
	}

	wLen	= MakeFrameHead(sa,pBuf,0);
	pAPDU	= &pBuf[wLen];
	pAPDU[wAPDULen++]	= GET_Request;
	pAPDU[wAPDULen++]	= pPara->byType;
	pAPDU[wAPDULen++]	= 0; //PIID
	switch(pPara->byType)
	{
	case 2://请求多个OAD
	case 4://请求多个REC
		wAPDULen	+= gdw698Buf_Num(&pAPDU[wAPDULen],&pPara->wNum,TD21);
		break;
	}
	memcpy(&pAPDU[wAPDULen],pPara->pTxBuff,pPara->wBufLen);
	wAPDULen	+= pPara->wBufLen;
	pAPDU[wAPDULen++]	= 0; //无时间标签
	if((pPara->bUseSafeMode) &&(pPara->Rand!= NULL))
	{
		randNumLen = pPara->Rand[0];
		if(( randNumLen == 16)||(randNumLen == 0))
		{
			//明文+随机数
			Head[wOffset++] = Security_Request; 
			Head[wOffset++] = 0; 
			wOffset	+= gdw698Buf_Num(&Head[wOffset],&wAPDULen,TD21);
			//调整APDU插入安全报文
			for(wI=0;wI<wAPDULen;wI++)
				pAPDU[wAPDULen-1-wI+wOffset]	= pAPDU[wAPDULen-1-wI];
			memcpy(pAPDU,Head,wOffset);
			wAPDULen	+= wOffset;
			pAPDU[wAPDULen++]	= 1;//随机数
			//随机数无效
			if(randNumLen == 0)
			{
				randNumLen	= 16;
				memset(&pPara->Rand[1],0x00,randNumLen);
			}
			pAPDU[wAPDULen++]	= randNumLen;//随机数长度

			memcpy(&pAPDU[wAPDULen],&pPara->Rand[1],randNumLen);
			wAPDULen	+= randNumLen;
		}
	}
	wLen	= Add698FrameTail((BYTE)(PRM | 3),(WORD)(wAPDULen+wLen-1),pBuf);
	return wLen;
}

WORD Tx698ActionFrame(BYTE *pCllAddr,BYTE byAPDU,BYTE byChoice,BYTE byPIID,BYTE *pData,BYTE *pBuf)
{
WORD wSize;
WORD wLen;
WORD wAPDULen=0;
DWORD dwOAD;
BYTE *pAPDU;
TSA sa;
OMD Method;
BYTE *pAdd = NULL;
	
	sa.log_addr = 1;
	sa.type = 0;
	sa.len = 5;
	memcpy(sa.addr,pCllAddr,6);
	pAdd = pData;
	wLen	= MakeFrameHead(&sa,pBuf,0);
	pAPDU	= &pBuf[wLen];
	pAPDU[wAPDULen++]	= byAPDU;
	pAPDU[wAPDULen++]	= byChoice;
	pAPDU[wAPDULen++]	= byPIID;
	if (byAPDU == ACTION_Request)
	{
		switch(byChoice)
		{
		case 1:
			Method.value = 0x60437F00;
			wAPDULen += gdw698buf_OMD(&pAPDU[wAPDULen],&Method,TD21);

			if (pAdd)
			{
				pAPDU[wAPDULen++] = dt_unsigned;
				wSize = gdw698Buf_unsigned(&pAPDU[wAPDULen],pAdd,TD21);
				wAPDULen += wSize;
				pAdd += wSize;
			}
			
			break;
		default:
			break;
		}
	}
	else if (byAPDU == REPORT_Response)
	{
		switch(byChoice)
		{
		case 0x02:
			dwOAD = 0x60120300;
			pAPDU[wAPDULen++] = 0x01;
			wSize = gdw698buf_OAD(&pAPDU[wAPDULen],&dwOAD,TD21);
			wAPDULen += wSize;
			break;
		default:
			break;
		}
	}

	pAPDU[wAPDULen++] = 0;													//无时间标签

	wLen = Add698FrameTail((BYTE)(PRM | 3),(WORD)(wAPDULen+wLen-1),pBuf);
	return wLen;
}

// static BOOL _general_Txmonitor(TOad60130200 *pTaskInfo)
// {
// TMGDW698 *p=(TMGDW698 *)GetMGyApp();
// TPort *pPort=(TPort *)GethPort();
// TOad60150200 *pschNo=(TOad60150200 *)pPort->GyRunInfo.psch698;
// TTx *pTx=GetTx();
// TSA	mp_sa;
// WORD wI,j,num=0,wMaxRxLen=get_max_m698rx_apdu_len(pPort),wOADMaxRxLen,wReqRxLen=0,wOldTmp2;
// BOOL bRc=FALSE;
// BYTE err[10]={dt_null};		
// TGetPara para={0};
// BYTE *TxBuf;
// CSD csdd;
// BYTE bbuf[1024];
// 	TxBuf	= bbuf;
// 	if(NULL == TxBuf)
// 		return FALSE;
// 	para.pTxBuff	= TxBuf;
// 	para.bUseSafeMode= IsUseSafeMode(pPort->GyRunInfo.wMPSeqNo);
// 	para.Rand = p->Rand;

// 	GetMP698Addr(&mp_sa);	
// 	p->byCJTypeFlag = 0;
// 	switch(pschNo->byCJType)
// 	{
// 	case 0: //采集当前数据		
// 		{
// 			DWORD dwOAD;
// 			num=0;
// 			//1、对以前有未应答的先填充异常数据
// 			for(j=0;(j<pschNo->byColNum)&&(pPort->GyRunInfo.cj_698.CSDRptr < pPort->GyRunInfo.cj_698.CSDWptr);j++)
// 			{
// 				get_no_csd_of_CSDS(pschNo->csdBuf,j,&csdd);
// 				if(csdd.choice == 0)
// 				{
// 					if(num >= pPort->GyRunInfo.cj_698.CSDRptr)
// 					{
// 						write_oad_data(csdd.oad.value,TRUE,err);
// 						pPort->GyRunInfo.cj_698.CSDRptr++;						
// 					}
// 					num++;					
// 				}	
// 			}								
// 			pPort->GyRunInfo.cj_698.CSDRptr	= pPort->GyRunInfo.cj_698.CSDWptr;
// 			//发送数据
// 			num	= 0;
// 			for(wI=0;wI<pschNo->byColNum;wI++)
// 			{
// 				get_no_csd_of_CSDS(pschNo->csdBuf,wI,&csdd);
// 				if(csdd.choice == 0)
// 				{
// 					if(num >= pPort->GyRunInfo.cj_698.CSDRptr)
// 					{
// 						dwOAD	= csdd.oad.value;	
// 						wOADMaxRxLen	= get_max_OAD_reponse_len(dwOAD);
// 						wOADMaxRxLen	+= 4;//OAD长度
// 						if((wOADMaxRxLen+wReqRxLen) > wMaxRxLen)
// 							break;
// 						wReqRxLen	+= wOADMaxRxLen;
// 						para.wBufLen+= gdw698buf_OAD(&para.pTxBuff[para.wBufLen],&dwOAD,TD21);
// 						para.wNum++;						
// 						pPort->GyRunInfo.cj_698.CSDWptr++;						
// 					}
// 					num++;					
// 				}	
// 			}			
// 			if(para.wNum)
// 			{
// 				bRc	= TRUE;
// 				pTx->wRptr	= pTx->wWptr	= 0;
// 				para.byType	= 2;
// 				pTx->wWptr	= _TxFm698Read(&mp_sa,&para,pTx->pBuf);
// 			}			
// 		}
// 		break;
// 	case 1: //采集上N次
// 	case 2: //按冻结时标采集
// 	case 3: //按时标间隔采集
// 		{			
// 			DWORD oadd,k,i;
// //..			BYTE byCJStep=1;
// 			memset(&p->rsdd,0,sizeof(RSD));
// 			num=0;
// 			for(j=0;(j<pschNo->byColNum)&&(pschNo->byCJType != 3);j++)
// 			{
// 				if(pPort->GyRunInfo.cj_698.CSDRptr >= pPort->GyRunInfo.cj_698.CSDWptr)
// 					break;
// 				get_no_csd_of_CSDS(pschNo->csdBuf,j,&csdd);
// 				if(csdd.choice == 0)
// 					continue;
// 				for(k=0;k<csdd.road.oadCols.nNum;k++)
// 				{
// 					if(pPort->GyRunInfo.cj_698.CSDRptr >= pPort->GyRunInfo.cj_698.CSDWptr)
// 						break;
// 					if(num >= pPort->GyRunInfo.cj_698.CSDRptr)
// 					{
// 						write_oad_data(csdd.road.oadCols.oad[k].value,TRUE,err);			
// 						pPort->GyRunInfo.cj_698.CSDRptr++;
// 					}	
// 					num++;							
// 				}					
// 			}
// 			if( pschNo->byCJType != 3 )
// 				pPort->GyRunInfo.cj_698.CSDRptr	= pPort->GyRunInfo.cj_698.CSDWptr;
// 			pPort->GyRunInfo.cj_698.byTxRowNum	= 1;
// 			if(pschNo->byCJType == 1)
// 			{//采集上N次
// 				TTime tTime;
// 				bRc	= TRUE;
// 				p->rsdd.choice		= 9;
// 				if(pschNo->CJContent._N)
// 					p->rsdd.sel9.nLast	= pschNo->CJContent._N;			
// 				else
// 				{
// 					p->rsdd.sel9.nLast	= 1;	
// 					GetTime(&tTime);
// 					// pPort->GyRunInfo.cj_698.dwFrzTimeMark = Time2Min(&tTime);
// 				}
// 			}
// 			else if(pschNo->byCJType == 2)
// 			{//按冻结时标采集(日冻结)
// 				WORD wCurveIntel=15;
// 				DWORD tmp;
// 				TTime tTime;
// 				switch(pTaskInfo->intelTI.unit)
// 				{
// 				case 0: wCurveIntel = pTaskInfo->intelTI.value/60;	break;	//秒
// 				case 1: wCurveIntel = pTaskInfo->intelTI.value;		break;	//分
// 				case 2: wCurveIntel = pTaskInfo->intelTI.value*60;	break;//时
// 				case 3: wCurveIntel = pTaskInfo->intelTI.value*1440;break;//日
// 				case 4: wCurveIntel = 0xFFFF;	break;
// 				}
// 				tmp	= pPort->GyRunInfo.dwTaskTime;			
// 				get_no_csd_of_CSDS(pschNo->csdBuf,0,&csdd);
// 				if(csdd.road.oadMain.value == 0x50060200)
// 				{
// 					Min2Time(tmp,&tTime);
// 					tTime.Day	= 1;	
// 					tTime.Hour = 0; tTime.Min=0; tTime.Sec=0;
// 				}
// 				else if(csdd.road.oadMain.value == 0x50050200)
// 				{
// 					Min2Time(tmp,&tTime);
// 				//	tTime.Hour = 0; 
// 					tTime.Min=0; 
// 					tTime.Sec=0;
// 				}
// 				else if(csdd.road.oadMain.value == 0x50040200)
// 				{
// 					Min2Time(tmp,&tTime);
// 					tTime.Hour = 0; tTime.Min=0; tTime.Sec=0;
// 				}
// 				else
// 				{
// 					if(wCurveIntel==0) 
// 						wCurveIntel=15;
// 					tmp  = (tmp/wCurveIntel)*wCurveIntel;//??????????????????防止按冻结时间采集，但开始时间不为整点！！
// 					//.tmp	-= wCurveIntel;
// 					Min2Time(tmp,&tTime);
// 				}
// 				p->rsdd.choice		= 1;
// 				p->rsdd.sel1.oad.value	= 0x20210200;	//冻结时间
// 				p->rsdd.sel1.value.type= dt_datetime_s;
// 				DATETIME_S_Time(&p->rsdd.sel1.value.tmVal,&tTime,TD21);
// 				// pPort->GyRunInfo.cj_698.dwFrzTimeMark = Time2Min(&tTime);
// 				if(wCurveIntel)
// 					bRc	= TRUE;
// 			}
// 			else
// 			{//按时标间隔采集				
// 				TTime tTime;
// 				DWORD dwStart,dwEnd;
// 				num	= 0;
// 				//1、对发送一包无应答的填充无效数据
// 				for(j=0;j<pschNo->byColNum;j++)
// 				{
// 					if(pPort->GyRunInfo.cj_698.CSDRptr >= pPort->GyRunInfo.cj_698.CSDWptr)
// 						break;
// 					get_no_csd_of_CSDS(pschNo->csdBuf,j,&csdd);
// 					if(csdd.choice == 0)
// 						continue;
// 					for(k=0;k<csdd.road.oadCols.nNum;k++)
// 					{
// 						if(pPort->GyRunInfo.cj_698.CSDRptr >= pPort->GyRunInfo.cj_698.CSDWptr)
// 							break;
// 						if(num >= pPort->GyRunInfo.cj_698.CSDRptr)
// 						{
// 							for(i=0;i<pPort->GyRunInfo.CurvCjCfg.byCJStep;i++)
// 								write_oad_dataEx(csdd.road.oadCols.oad[k].value,(BYTE)i,TRUE,err);											
// 							pPort->GyRunInfo.cj_698.CSDRptr++;
// 						}
// 						num++;						
// 					}					
// 				}
// 				pPort->GyRunInfo.cj_698.CSDRptr	= pPort->GyRunInfo.cj_698.CSDWptr;	
// 				pPort->GyRunInfo.cj_698.byTxRowNum = pPort->GyRunInfo.CurvCjCfg.byCJStep;
// 				if(pPort->GyRunInfo.CurvCjCfg.wNo < pPort->GyRunInfo.CurvCjCfg.wNum)
// 				{
// 					dwStart	= pPort->GyRunInfo.CurvCjCfg.dwStart+pPort->GyRunInfo.CurvCjCfg.wNo*pPort->GyRunInfo.CurvCjCfg.wInterval;
// 					Min2Time(dwStart,&tTime);
// 					p->rsdd.sel2.from.type	= dt_datetime_s;
// 					DATETIME_S_Time(&p->rsdd.sel2.from.tmVal,&tTime,TD21);
// 					dwEnd	= dwStart+pPort->GyRunInfo.CurvCjCfg.byCJStep*pPort->GyRunInfo.CurvCjCfg.wInterval;
// 					Min2Time(dwEnd,&tTime);
// 					p->rsdd.sel2.to.type	= dt_datetime_s;
// 					DATETIME_S_Time(&p->rsdd.sel2.to.tmVal,&tTime,TD21);				
// 					p->rsdd.choice = 2;
// 					p->rsdd.sel2.oad.value	= 0x20210200;	//冻结时标				
// 					p->rsdd.sel2.span.type	= dt_TI;
// 					p->rsdd.sel2.span.ti.unit	= pschNo->CJContent.tii.unit;
// 					p->rsdd.sel2.span.ti.value	= pschNo->CJContent.tii.value;						
// 					bRc	= TRUE;
// 				}
// 			}			
// 			num	= 0;			
// 			if(bRc)
// 			{	
// 				BOOL bReal2Frz = FALSE;
				
// 				if((GetUserType() == MAR_SHAN_DONG) && (GetCurveSource() != 0x55))
// 				{
// 					for (i=0;i<pschNo->byColNum;i++)
// 					{
// 						get_no_csd_of_CSDS(pschNo->csdBuf,i,&csdd);
// 						if (csdd.choice == 0)
// 							continue;
// 						if (csdd.road.oadMain.value == 0x50020200)
// 						{
// 							if(pschNo->byCJType == 3)
// 							{
// 								if(pPort->GyRunInfo.CurvCjCfg.wNum == 1)//只处理一个点
// 									bReal2Frz = TRUE;	
// 							}
// 							else
// 								bReal2Frz = TRUE;
							
// 							if(bReal2Frz)
// 							{
// 								if(is_need_cj_curv(&csdd))
// 									bReal2Frz = FALSE;
// 								else
// 								{
// 									p->byCJTypeFlag = pschNo->byCJType;
// 									p->dwLastTaskTime = pPort->GyRunInfo.dwTaskTime;
// 								}
// 							}
// 						}
// 					}	
// 				}
// 				if(bReal2Frz)
// 				{//曲线用实时转存
// 					WORD wNum,wOldTmp,wVaild;
// 					BYTE byColNum;
// 					for(wI=0;wI<pschNo->byColNum;wI++)
// 					{
// 						get_no_csd_of_CSDS(pschNo->csdBuf,wI,&csdd);
// 						if(csdd.choice == 0)
// 							continue;
// 						wOldTmp	= para.wBufLen;
// 						wVaild	= 0;
// 						wNum	= csdd.road.oadCols.nNum;
// 						byColNum	= 0;
// 						for(j=0;j<wNum;j++)
// 						{	
// 							if(num >= pPort->GyRunInfo.cj_698.CSDRptr)
// 							{	
// 								oadd	= csdd.road.oadCols.oad[j].value;
// 								//分帧判断
// 								wOADMaxRxLen	= get_max_OAD_reponse_len(oadd);
// 								if(pschNo->byCJType == 3)//对曲线每次几个时间点 
// 									wOADMaxRxLen	*= pPort->GyRunInfo.CurvCjCfg.byCJStep;
// 								wOADMaxRxLen	+= 4;//(OAD长度)
// 								if(((wOADMaxRxLen+wReqRxLen) > wMaxRxLen)||(byColNum >= 8))
// 									break;	
// 								byColNum++;
// 								wReqRxLen	+= wOADMaxRxLen;							
// 								para.wBufLen	+= gdw698buf_OAD(&TxBuf[para.wBufLen],&oadd,TD21);
// 								pPort->GyRunInfo.cj_698.CSDWptr	++;
// 								para.wNum++;
// 								wVaild	= 1;
// 							}
// 							num++;					
// 						}									
// 						if((j < num)&&para.wNum)
// 							break;
// 					}
// 					if(para.wNum)
// 					{
// 						pTx->wRptr	= pTx->wWptr	= 0;
// 						para.byType	= 2;
// 						pTx->wWptr	= _TxFm698Read(&mp_sa,&para,pTx->pBuf);
// 					}
// 					else
// 					{
// 						p->byCJTypeFlag = 0;
// 						bRc	= FALSE;
// 					}
// 				}
// 				else
// 				{
// 					WORD wNum,wOldTmp,wVaild;
// 					BYTE byColNum;
// 					for(wI=0;wI<pschNo->byColNum;wI++)
// 					{
// 						get_no_csd_of_CSDS(pschNo->csdBuf,wI,&csdd);
// 						if(csdd.choice == 0)
// 							continue;
// 						wOldTmp	= para.wBufLen;
// 						wVaild	= 0;
// 						oadd	= csdd.road.oadMain.value;					
// 						para.wBufLen	+= gdw698buf_OAD(&TxBuf[para.wBufLen],&oadd,TD21);
// 						para.wBufLen	+= gdw698buf_RSD(&TxBuf[para.wBufLen],&p->rsdd,TD21);
// 						wNum	= csdd.road.oadCols.nNum;
// 						wOldTmp2= para.wBufLen;		
// 						para.wBufLen++;
// 						byColNum	= 0;
// 						for(j=0;j<wNum;j++)
// 						{	
// 							if(num >= pPort->GyRunInfo.cj_698.CSDRptr)
// 							{	
// 								oadd	= csdd.road.oadCols.oad[j].value;
// 								if (csdd.road.oadMain.value == 0x50020200)
// 								{//单相电压曲线配置20000201 部分698表回复数据为NULL
// 									if ((LHBYTE(oadd) == 2)&&(LLBYTE(oadd)==1))
// 									{
// 										WORD oi = HIWORD(oadd);
// 										if ((oi== 0x2000)||(oi== 0x2001)||(oi == 0x2004)||(oi == 0x2005)||(oi == 0x200A))
// 											oadd--;//索引变0
// 									}
// 								}
// 								//分帧判断
// 								wOADMaxRxLen	= get_max_OAD_reponse_len(oadd);
// 								if(pschNo->byCJType == 3)//对曲线每次几个时间点 
// 									wOADMaxRxLen	*= pPort->GyRunInfo.CurvCjCfg.byCJStep;
// 								wOADMaxRxLen	+= 4;//(OAD长度)
// 								if(((wOADMaxRxLen+wReqRxLen) > wMaxRxLen)||(byColNum >= 8))
// 									break;	
// 								byColNum++;
// 								wReqRxLen	+= wOADMaxRxLen;
// 								TxBuf[para.wBufLen++]	= 0;
// 								para.wBufLen	+= gdw698buf_OAD(&TxBuf[para.wBufLen],&oadd,TD21);
// 								pPort->GyRunInfo.cj_698.CSDWptr	++;
// 								wVaild	= 1;
// 							}
// 							num++;					
// 						}					
// 						if(wVaild)
// 						{
// 							TxBuf[wOldTmp2]	= byColNum;
// 							para.wNum++;											
// 						}	
// 						else//此条ROAD已经发送结束 (回退RSD)
// 							para.wBufLen	= wOldTmp;					
// 						if((j < num)&&para.wNum)
// 							break;
// 					}
// 					if(para.wNum)
// 					{					
// 						pTx->wRptr	= pTx->wWptr	= 0;
// 						if(para.wNum > 1)
// 							para.byType	= 4;							
// 						else
// 							para.byType	= 3;	
// 						pTx->wWptr	= _TxFm698Read(&mp_sa,&para,pTx->pBuf);
// 					}
// 					else	bRc	= FALSE;				
// 				}			
// 			}
// 		}
// 		break;
// 	case 4: //补抄(后续具体用户实现)
// 		{
// 			WORD wCurveIntel=15;
// 			DWORD tmp,oadd;
// 			TTime tTime;
// 			//if(pPort->GyRunInfo.CurvCjCfg.wNo < pPort->GyRunInfo.CurvCjCfg.wNum)
// 			//	return FALSE;
// 			pPort->GyRunInfo.cj_698.CSDRptr	= pPort->GyRunInfo.cj_698.CSDWptr;	
// 			pPort->GyRunInfo.cj_698.byTxRowNum = (BYTE)pPort->GyRunInfo.ReBuCJCfg.wReCJStep;
// 			switch(pTaskInfo->intelTI.unit)
// 			{
// 			case 0: wCurveIntel = pTaskInfo->intelTI.value/60;	break;	//秒
// 			case 1: wCurveIntel = pTaskInfo->intelTI.value;		break;	//分
// 			case 2: wCurveIntel = pTaskInfo->intelTI.value*60;	break;//时
// 			case 3: wCurveIntel = pTaskInfo->intelTI.value*1440;break;//日
// 			case 4: wCurveIntel = 0xFFFF;	break;
// 			}
// 			tmp	= pPort->GyRunInfo.ReBuCJCfg.dwStart+pPort->GyRunInfo.ReBuCJCfg.wNo*pPort->GyRunInfo.ReBuCJCfg.wInterval;			
// 			get_no_csd_of_CSDS(pschNo->csdBuf,0,&csdd);
// 			if(csdd.road.oadMain.value == 0x50060200)
// 			{
// 				Min2Time(tmp,&tTime);
// 				tTime.Day	= 1;	
// 				tTime.Hour = 0; tTime.Min=0; tTime.Sec=0;
// 					p->rsdd.choice		= 1;
// 				p->rsdd.sel1.oad.value	= 0x20210200;	//冻结时间
// 				p->rsdd.sel1.value.type= dt_datetime_s;
// 				DATETIME_S_Time(&p->rsdd.sel1.value.tmVal,&tTime,TD21);
// 				if(wCurveIntel)
// 					bRc	= TRUE;
// 			}	
// 			else if(csdd.road.oadMain.value == 0x50040200)
// 			{
// 				Min2Time(tmp,&tTime);
// 				tTime.Hour = 0; tTime.Min=0; tTime.Sec=0;
// 				p->rsdd.choice		= 1;
// 				p->rsdd.sel1.oad.value	= 0x20210200;	//冻结时间
// 				p->rsdd.sel1.value.type= dt_datetime_s;
// 				DATETIME_S_Time(&p->rsdd.sel1.value.tmVal,&tTime,TD21);
// 				if(wCurveIntel)
// 				bRc	= TRUE;
// 			}
// 			else
// 			{
// 				DWORD dwStart,dwEnd;
// 				if(wCurveIntel==0) 
// 					wCurveIntel=15;
// 				tmp  = (tmp/wCurveIntel)*wCurveIntel;
// 				Min2Time(tmp,&tTime);
// 				dwStart	= tmp;
// 				Min2Time(dwStart,&tTime);
// 				p->rsdd.sel2.from.type	= dt_datetime_s;
// 				DATETIME_S_Time(&p->rsdd.sel2.from.tmVal,&tTime,TD21);
// 				dwEnd	= dwStart+pPort->GyRunInfo.ReBuCJCfg.wReCJStep*pPort->GyRunInfo.ReBuCJCfg.wInterval;
// 				Min2Time(dwEnd,&tTime);
// 				p->rsdd.sel2.to.type	= dt_datetime_s;
// 				DATETIME_S_Time(&p->rsdd.sel2.to.tmVal,&tTime,TD21);				
// 				p->rsdd.choice = 2;
// 				p->rsdd.sel2.oad.value	= 0x20210200;	//冻结时标				
// 				p->rsdd.sel2.span.type	= dt_TI;
// 				p->rsdd.sel2.span.ti.unit	= pschNo->CJContent.retryMeter.tii.unit;
// 				p->rsdd.sel2.span.ti.value	= pschNo->CJContent.retryMeter.tii.value;						
// 				bRc	= TRUE;
// 			}		
// 			num	= 0;			
// 			if(bRc)
// 			{		
// 				WORD wNum,wOldTmp,wVaild;
// 				BYTE byColNum;
// 				for(wI=0;wI<pschNo->byColNum;wI++)
// 				{
// 					get_no_csd_of_CSDS(pschNo->csdBuf,wI,&csdd);
// 					if(csdd.choice == 0)
// 						continue;
// 					wOldTmp	= para.wBufLen;
// 					wVaild	= 0;
// 					oadd	= csdd.road.oadMain.value;					
// 					para.wBufLen	+= gdw698buf_OAD(&TxBuf[para.wBufLen],&oadd,TD21);
// 					para.wBufLen	+= gdw698buf_RSD(&TxBuf[para.wBufLen],&p->rsdd,TD21);
// 					wNum	= csdd.road.oadCols.nNum;
// 					wOldTmp2= para.wBufLen;		
// 					para.wBufLen++;
// 					byColNum	= 0;
// 					for(j=0;j<wNum;j++)
// 					{	
// 						if(num >= pPort->GyRunInfo.cj_698.CSDRptr)
// 						{	
// 							oadd	= csdd.road.oadCols.oad[j].value;
// 							//分帧判断
// 							wOADMaxRxLen	= get_max_OAD_reponse_len(oadd);
// 							if(pschNo->byCJType == 4)//对曲线每次几个时间点 
// 								wOADMaxRxLen	*= pPort->GyRunInfo.ReBuCJCfg.wReCJStep;
// 							wOADMaxRxLen	+= 4;//(OAD长度)
// 							if(((wOADMaxRxLen+wReqRxLen) > wMaxRxLen)||(byColNum >= 8))
// 								break;	
// 							byColNum++;
// 							wReqRxLen	+= wOADMaxRxLen;
// 							TxBuf[para.wBufLen++]	= 0;							
// 							para.wBufLen	+= gdw698buf_OAD(&TxBuf[para.wBufLen],&oadd,TD21);
// 							pPort->GyRunInfo.cj_698.CSDWptr	++;
// 							wVaild	= 1;
// 						}
// 						num++;					
// 					}					
// 					if(wVaild)
// 					{
// 						TxBuf[wOldTmp2]	= byColNum;
// 						para.wNum++;											
// 					}	
// 					else//此条ROAD已经发送结束 (回退RSD)
// 						para.wBufLen	= wOldTmp;					
// 					if((j < num)&&para.wNum)
// 						break;
// 				}
// 				if(para.wNum)
// 				{					
// 					pTx->wRptr	= pTx->wWptr	= 0;
// 					if(para.wNum > 1)
// 						para.byType	= 4;							
// 					else
// 						para.byType	= 3;	
// 					pTx->wWptr	= _TxFm698Read(&mp_sa,&para,pTx->pBuf);
// 					//.pPort->GyRunInfo.ReBuCJCfg.wNo++;
// 				}
// 				else	bRc	= FALSE;				
// 			}	
// 		}		
// 		break;
// 	}
// 	return bRc;
// }

static BOOL _event_Txmonitor(TOad60130200 *pTaskInfo)
{

BOOL bRc=FALSE;

	return bRc;
}

static BOOL _transparent_Txmonitor(TOad60130200 *pTaskInfo)
{
TMGDW698 *p=(TMGDW698 *)GetMGyApp();
TPort *pPort=(TPort *)GethPort();
TOad60190200 *pschNo=(TOad60190200 *)pPort->GyRunInfo.psch698;
TTx *pTx=GetTx();
TSA	mp_sa;
WORD i,j,num=0;
WORD wFmLen = 0;
WORD wFmOffset = 0;
BYTE upFlag = 0;
	
	GetMP698Addr(&mp_sa);
	
	pTx->wRptr	= pTx->wWptr	= 0;
	for (i = 0; i < pschNo->byContentNum; i++)
	{
		if(comp_TSA_addr(&pschNo->Contents[i].tsa,&mp_sa) != 0) /* 地址不一致 */
			continue;
		for (j = 0; j < pschNo->Contents[i].byFrameNum; j++)
		{
			if(num >= pPort->GyRunInfo.cj_698.wTransRptr)
			{
				upFlag	= (BYTE)pschNo->Contents[i].bRptAndWait;
				pPort->GyRunInfo.cj_698.wTransRptr++;
				pPort->GyRunInfo.cj_698.byTransNo	= pschNo->byTransNo;
				// pPort->GyRunInfo.cj_698.dwTransInfo = MDW(0,upFlag,(BYTE)i,pschNo->Contents[i].tranFrames[j].byFrameNo);
				wFmLen	= pschNo->Contents[i].tranFrames[j].wFrameLen;
				wFmOffset = pschNo->Contents[i].tranFrames[j].wFmOffset;
				if(((WORD)(wFmOffset+wFmLen) <= sizeof(pschNo->Contents[i].FmBuffer)) && wFmLen)
				{
					memcpy(pTx->pBuf,&pschNo->Contents[i].FmBuffer[wFmOffset],wFmLen);
					pTx->wWptr	= wFmLen;
					//Trace("trans tx %d",num);
					return TRUE;
				}
			}
			num++;
		}
	}
	return FALSE;
}

// BOOL gdw698M_TxMonitor(TPort *pPort)
// {
// 	TMGDW698 *p=(TMGDW698 *)GetMGyApp();
// 	BYTE task_no=get_698_task_no();
// 	TOad60130200 *ptaskCfg=pGet698NoTaskCfg(task_no);
// 	BOOL bRc=FALSE;
// 	TSA mp_sa;
// 	BYTE TxBuf[20];
// 	TGetPara para={0};
// 	TTx *pTx=GetTx();
	
// 	para.Rand = p->Rand;

// 	if(NULL == ptaskCfg)
// 		return FALSE;
// 	if(pPort->GyRunInfo.wMPSeqNo==0xFFFF)
// 	{
// 		return FALSE;
// 	}
// 	if(!SafeModeIsInit(pPort->GyRunInfo.wMPSeqNo))
// 	{
// 		switch(pPort->GyRunInfo.byGyStep&0x7F)
// 		{
// 		case 0://读安全模式
// 			if((pPort->GyRunInfo.byGyStep & BIT7) == 0)
// 			{
// 				DWORD dwOAD=0xF1010200;
// 				GetMP698Addr(&mp_sa);
// 				para.byType	= 1;
// 				para.pTxBuff= TxBuf;
// 				para.wBufLen	+= gdw698buf_OAD(&para.pTxBuff[para.wBufLen],&dwOAD,TD21);
// 				pTx->wRptr	= pTx->wWptr	= 0;				
// 				pTx->wWptr	= _TxFm698Read(&mp_sa,&para,pTx->pBuf);
// 				pPort->GyRunInfo.byGyStep	|= BIT7;
// 				return TRUE;
// 			}
// 			break;
// 		case 1://读电表表号
// 			if((pPort->GyRunInfo.byGyStep & BIT7) == 0)
// 			{
// 				DWORD dwOAD=0x40020200;
// 				GetMP698Addr(&mp_sa);
// 				para.byType	= 1;
// 				para.pTxBuff= TxBuf;
// 				para.wBufLen	+= gdw698buf_OAD(&para.pTxBuff[para.wBufLen],&dwOAD,TD21);
// 				pTx->wRptr	= pTx->wWptr	= 0;				
// 				pTx->wWptr	= _TxFm698Read(&mp_sa,&para,pTx->pBuf);
// 				pPort->GyRunInfo.byGyStep	|= BIT7;
// 				return TRUE;
// 			}
// 			break;
// 		}		
// 		return FALSE;		
// 	}
// 	if(((DWORD)(pPort->dwRunCount-p->dwRandFreshTime) >= 3600000)
// 		||(p->dwRandFreshTime == 0))
// 	{//每小时更新一次随机数
// 	#if(!MOS_MSVC)
// //		TESAM_GetRandom(p->Rand);
// 	#endif
// 		p->dwRandFreshTime	= pPort->dwRunCount;
// 	}
// 	if(p->byReqNext)	
// 	{//请求后续帧		
// 		pTx->wRptr	= 0;
// 		p->byReqNext= 0;
// 		GetMP698Addr(&mp_sa);
// 		para.byType	= 5;
// 		para.pTxBuff= TxBuf;
// 		para.bUseSafeMode	= IsUseSafeMode(pPort->GyRunInfo.wMPSeqNo);
// 		para.wBufLen= gdw698Buf_long_unsigned(para.pTxBuff,&p->wRxOKSeq,TD21);
// 		pTx->wWptr	= _TxFm698Read(&mp_sa,&para,pTx->pBuf);
//         return TRUE;
// 	}	
// 	//1、获的采集方案表
// 	switch(ptaskCfg->byTaskType)
// 	{
// 	case TT_DATA_CJ	: bRc	= _general_Txmonitor(ptaskCfg);		break; //普通采集方案
// 	case TT_EVENT_CJ: bRc	= _event_Txmonitor(ptaskCfg);		break; //事件采集方案
// 	case TT_TRANS_CJ: bRc	= _transparent_Txmonitor(ptaskCfg);	break; //透明方案
// 	}
// 	if((pTx->wRptr < pTx->wWptr)&&bRc)
// 		p->byTxTaskType	= ptaskCfg->byTaskType;	
// 	else
// 	{
// 		p->byTxTaskType	= 0;
// 		pTx->wRptr = pTx->wWptr	= 0;
// 	}	
// 	return bRc;
// }

// WORD rx_OAD_Reponse(BYTE *pData)
// {
// TMGDW698 *p=(TMGDW698 *)GetMGyApp();
// TPort *pPort=(TPort *)GethPort();
// DWORD oadd;
// WORD wLen=0,wNum,wRc=get_OAD_data_len(pData);
// BYTE choice;
// BOOL bInitSafeMode=FALSE;

// 	if(pPort->GyRunInfo.byGyStep)
// 		bInitSafeMode	= TRUE;
// 	wLen	+= gdw698buf_OAD(&pData[wLen],&oadd,TD12);
// 	choice	= pData[wLen++];
// 	if(choice == 0)
// 	{
// 		BYTE err[10];
// 		err[0]	= dt_null;	
// 		if(!bInitSafeMode)
// 			write_oad_data(oadd,TRUE,err);
// 		else
// 		{//安全模式应答错误的情况
// 			pPort->GyRunInfo.byGyStep	&= 0x7F;
// 			if(oadd == 0xF1010200)
// 			{
// 				pPort->GyRunInfo.byGyStep++;
// 				set_mp_safemode(pPort->GyRunInfo.wMPSeqNo,FALSE);
// 			}	
// 			else if(oadd == 0x40020200)
// 			{
// 				TData_6203 *pSys=pGetDF6203();
// 				pPort->GyRunInfo.byGyStep	= 0;	
// 				pSys->safmodeInit[pPort->GyRunInfo.wMPSeqNo>>3]	|= 1<<(pPort->GyRunInfo.wMPSeqNo&0x07);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if(bInitSafeMode)
// 		{//			
// 			pPort->GyRunInfo.byGyStep	&= 0x7F;
// 			if(oadd == 0xF1010200)
// 			{//安全模式是否启用
// 				if(pData[wLen+1])
// 					set_mp_safemode(pPort->GyRunInfo.wMPSeqNo,TRUE);
// 				else
// 					set_mp_safemode(pPort->GyRunInfo.wMPSeqNo,FALSE);				
// 				pPort->GyRunInfo.byGyStep++;
// 			}
// 			else if(oadd == 0x40020200)
// 			{
// 				BYTE MeterNo[8]={0};
// 				if(pData[wLen++] != dt_octet_str)
// 					return wRc;
// 				wLen	+= gdw698Buf_Num(&pData[wLen],&wNum,TD12);
// 				if(wNum > 8)
// 					wNum	= 8;
// 				memcpy(&MeterNo[8-wNum],&pData[wLen],wNum);
// 				write_mp_meterno(pPort->GyRunInfo.wMPSeqNo,MeterNo);
// 				pPort->GyRunInfo.byGyStep	= 0;		
// 			}		
// 			return wRc;
// 		}
// 		if(p->byTxTaskType == TT_DATA_CJ)
// 		{			
// 			write_oad_data(oadd,FALSE,&pData[wLen]);
// 			// pPort->GyRunInfo.cj_698.wOKDataNum++;
// 			pPort->GyRunInfo.byReqDataValid = 1;
// 		}	
// 	}
// 	// pPort->GyRunInfo.cj_698.wRxDataNum++;	
// 	pPort->GyRunInfo.cj_698.CSDRptr++;
// 	return wRc;
// }

// WORD rx_OADS_Reponse(BYTE *pData)
// {
// WORD num,i;
// WORD wLen=0;

// 	wLen	+= gdw698Buf_Num(&pData[wLen],&num,TD12);
// 	for(i=0;i<num;i++)
// 	{
// 		wLen	+= rx_OAD_Reponse(&pData[wLen]);		
// 	}
// 	return wLen;
// }

void _WriteMeterEvent(DWORD oadd,TSA *psa,BYTE *pData,WORD wDataLen,BOOL bReprot)
{

}		

void Rx_Event_Reponse(BYTE *pData,BYTE byType)
{

}


void set_trans_oad(DWORD oadd,DWORD *p_trans_flag)
{
WORD oi;
			
	CHECKPTR0(p_trans_flag);

	if ((LHBYTE(oadd) == 2)&&(LLBYTE(oadd)==1))
	{
		oi = HIWORD(oadd);
		if ((oi == 0x2000)||(oi == 0x2001)||(oi == 0x2004)||(oi == 0x2005)||(oi == 0x200A))
		{
			(*p_trans_flag) |= 1<<(oi-0x2000);//bit0 bit1 bit4 bit5 bit10;
		}
	}
}

void get_trans_oad(DWORD *poad,DWORD trans_flag)
{
DWORD oadd;
WORD oi;

	CHECKPTR0(poad);

	oadd = *poad;
	if ((LHBYTE(oadd) == 2)&&(LLBYTE(oadd)==0))
	{
		oi = HIWORD(oadd);
		if ((oi == 0x2000)||(oi == 0x2001)||(oi == 0x2004)||(oi == 0x2005)||(oi == 0x200A))
		{
			if(trans_flag&(1<<(oi-0x2000)))//bit0 bit1 bit4 bit5 bit10;
			{
				oadd++;//索引1
				*poad = oadd;
			}
		}
	}
}

/*************************************************************************
*Function: ResultNormal_2_RecodRow
*Description: APDU域重组（8502-->8503）
*Others: 
*Input:  BYTE *pSource,
*Output: BYTE *pDest
*Return: WORD
*************************************************************************/
WORD ResultNormal_2_RecodRow(BYTE *pSource,BYTE *pDest)
{
TMGDW698 *p=(TMGDW698 *)GetMGyApp();
DWORD oadd = 0x50020200;
WORD wLen_source = 0,wLen_dest = 0;
WORD wOffset,wDataLen = 0;
BYTE i,byNum,choice;
	
	byNum = pSource[3]; //SEQUENCE OF A-ResultNormal
	wLen_source += 4;
    if(byNum == 0)
	{
		return 0;
	}
	pDest[wLen_dest++] = 0x85;
	pDest[wLen_dest++] = 0x03;
	pDest[wLen_dest++] = 0; //piid
    wLen_dest	+= gdw698buf_OAD(&pDest[wLen_dest],&oadd,TD21);
	pDest[wLen_dest++] = byNum;
	wOffset		= (WORD)(wLen_dest + byNum*5);
	pDest[wOffset++] = 0x01;
	pDest[wOffset++] = 0x01; //SEQUENCE OF A-RecordRow
	for(i = 0; i<byNum; i++)
	{
		wLen_source	+= gdw698buf_OAD(&pSource[wLen_source],&oadd,TD12);
		choice	= pSource[wLen_source++];
		if(choice == 0)
			wDataLen = 1;
		else
			wDataLen	= objGetDataLen((OBJ_DATA_TYPE)pSource[wLen_source],&pSource[wLen_source+1],MAX_NUM_NEST)+1;
		
		pDest[wLen_dest++] = 0;
		wLen_dest	+= gdw698buf_OAD(&pDest[wLen_dest],&oadd,TD21);
		
		if(oadd == 0x20210200)
		{
			if(p->byCJTypeFlag == 2) 
			{
				pDest[wOffset++] = dt_datetime_s;
				gdw698buf_DATETIME_S(&pDest[wOffset],&p->rsdd.sel1.value.tmVal,TD21);
				wOffset += sizeof(DATETIME_S);
			}
			else if(p->byCJTypeFlag == 3)
			{
				pDest[wOffset++] = dt_datetime_s;
				gdw698buf_DATETIME_S(&pDest[wOffset],&p->rsdd.sel2.from.tmVal,TD21);
				wOffset += sizeof(DATETIME_S);
			}
			else
			{
				DATETIME_S dts;
				TTime tTime;
#if 0
                pDest[wOffset++] = dt_null;
#else
				Min2Time(p->dwLastTaskTime,&tTime);
				DATETIME_S_Time(&dts,&tTime,TD21);

				pDest[wOffset++] = dt_datetime_s;
				gdw698buf_DATETIME_S(&pDest[wOffset],&dts,TD21);
				wOffset += sizeof(DATETIME_S);
#endif
			}	
		}
		else
		{
			if(choice == 0)
				pDest[wOffset++] = dt_null;
			else
			{
				memcpy(&pDest[wOffset],&pSource[wLen_source],wDataLen);
				wOffset += wDataLen;
			}
		}
		wLen_source += wDataLen;
	}
    return wOffset;
}
// WORD rx_REC_Reponse(BYTE *pData)
// {
// TMGDW698 *p=(TMGDW698 *)GetMGyApp();
// TPort *pPort=(TPort *)GethPort();
// TOad60150200 *pschNo=(TOad60150200 *)pPort->GyRunInfo.psch698;
// DWORD oadd;
// WORD wLen=0,i,num,wCSDNum,wRc=get_REC_data_len(pData);
// BYTE *pRCSD;
// BYTE choice,j;
// CSD csdd;
// DWORD trans_flag = 0;

// 	wLen	+= gdw698buf_OAD(&pData[wLen],&oadd,TD12);		
// 	pRCSD	= &pData[wLen];
// 	wLen	+= objGetDataLen(dt_RCSD,&pData[wLen],MAX_NUM_NEST);	
// 	wCSDNum	= get_csd_num_of_RCSD(pRCSD);
// 	choice	= pData[wLen++];
// 	if(p->byTxTaskType == TT_EVENT_CJ)
// 	{//事件		
// 		Rx_Event_Reponse(pData,1);
// 		return wRc;
// 	}

// //	if(GetUserType() == MAR_SHAN_DONG)
// 	{//山东单相电压曲线配置20000201 部分698表回复数据为NULL
// 		if ((pschNo != NULL) && /*(GetCurveSource() == 0x55)*/(p->byCJTypeFlag == 0))
// 		{
// 			for (i=0;i<pschNo->byColNum;i++)
// 			{
// 				get_no_csd_of_CSDS(pschNo->csdBuf,i,&csdd);
// 				if (csdd.choice == 0)
// 					continue;
// 				if (csdd.road.oadMain.value == 0x50020200)
// 				{
// 					for(j=0;j<csdd.road.oadCols.nNum;j++)
// 						set_trans_oad(csdd.road.oadCols.oad[j].value,&trans_flag);
// 				}
// 			}
// 		}
// 	}

// 	if(choice == 0)
// 	{//没有数据
// 		BYTE err[10];
// 		err[0]	= dt_null;		
// 		for(j=0;j<pPort->GyRunInfo.cj_698.byTxRowNum;j++)
// 		{
// 			for(i=0;i<wCSDNum;i++)
// 			{
// 				get_no_csd_of_RCSD(pRCSD,i,&csdd);
// 				if (trans_flag)
// 					get_trans_oad(&csdd.oad.value,trans_flag);
// 				write_oad_dataEx(csdd.oad.value,j,TRUE,&err[0]);				
// 			}
// 		}
// 		pPort->GyRunInfo.cj_698.CSDRptr	+= wCSDNum;
// 		if(pPort->GyRunInfo.byReqDataValid != 1)
// 			pPort->GyRunInfo.byReqDataValid = 0;
// 	}
// 	else
// 	{//有数据
// 		wLen	+= gdw698Buf_Num(&pData[wLen],&num,TD12);
// 		if( num || pPort->GyRunInfo.byReqDataValid )
// 			pPort->GyRunInfo.byReqDataValid = 1;
// 		else
// 			pPort->GyRunInfo.byReqDataValid = 0;
// 		if(num==0)
// 		{
// 			BYTE err[10];
// 			err[0]	= dt_null;		
// 			for(j=0;j<pPort->GyRunInfo.cj_698.byTxRowNum;j++)
// 			{
// 				for(i=0;i<wCSDNum;i++)
// 				{
// 					get_no_csd_of_RCSD(pRCSD,i,&csdd);
// 					if (trans_flag)
// 						get_trans_oad(&csdd.oad.value,trans_flag);
// 					write_oad_dataEx(csdd.oad.value,j,TRUE,&err[0]);				
// 				}
// 			}
// 		}
// 		for(j=0;j<pPort->GyRunInfo.cj_698.byTxRowNum;j++)
// 		{
// 			if(j  < num)
// 			{
// 				for(i=0;i<wCSDNum;i++)
// 				{
// 					get_no_csd_of_RCSD(pRCSD,i,&csdd);
// 					if (trans_flag)
// 						get_trans_oad(&csdd.oad.value,trans_flag);
// 					write_oad_dataEx(csdd.oad.value,j,FALSE,&pData[wLen]);				
// 					wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],MAX_NUM_NEST)+1;									
// 				}
// 			}
// 			else if(num)
// 			{
// 				BYTE err[10];
// 				err[0]	= dt_null;	
// 				for(i=0;i<wCSDNum;i++)
// 				{
// 					get_no_csd_of_RCSD(pRCSD,i,&csdd);
// 					if (trans_flag)
// 						get_trans_oad(&csdd.oad.value,trans_flag);
// 					write_oad_dataEx(csdd.oad.value,j,TRUE,&err[0]);				
// 				}
// 			}
// 		}			
// 		pPort->GyRunInfo.cj_698.CSDRptr	+= wCSDNum;
// 		// pPort->GyRunInfo.cj_698.wOKDataNum++;
// 	}
// 	// pPort->GyRunInfo.cj_698.wRxDataNum++;	
// 	return wRc;
// }

// WORD rx_RECS_Reponse(BYTE *pData)
// {
// WORD num,i;
// WORD wLen=0;

// 	wLen	+= gdw698Buf_Num(&pData[wLen],&num,TD12);
// 	for(i=0;i<num;i++)
// 	{
// 		wLen	+= rx_REC_Reponse(&pData[wLen]);		
// 	}
// 	return wLen;
// }

// void rs_Get(BYTE  *pDataAPDU)
// {
// TMGDW698 *p=(TMGDW698 *)GetMGyApp();
// WORD wOffset=2;
	
// 	p->byReqNext	= 0;	
// 	switch(pDataAPDU[0])
// 	{
// 	case 1: wOffset	+= rx_OAD_Reponse(&pDataAPDU[2]);	break;	//获得响应		
// 	case 2: wOffset	+= rx_OADS_Reponse(&pDataAPDU[2]);	break;	//获得若干对象的响应
// 	case 3:	wOffset	+= rx_REC_Reponse(&pDataAPDU[2]);	break;
// 	case 4:	wOffset	+= rx_RECS_Reponse(&pDataAPDU[2]);	break;
// 	case 5: //分帧响应一个数据块
// 		if(pDataAPDU[2] == 0)
// 		{//还有后续帧
// 			p->byReqNext	= 1;
// 		}
// 		wOffset	= 3;
// 		//获得当前正确接收的分帧序号
// 		wOffset	+= gdw698Buf_long_unsigned(&pDataAPDU[wOffset],&p->wRxOKSeq,TD12);
// 		switch(pDataAPDU[wOffset])
// 		{
// 		case 0:	wOffset	+= 2;	break;	//DAR
// 		case 1:	//sequence of A ResutNormal
// 			wOffset	+= rx_OADS_Reponse(&pDataAPDU[wOffset+1])+1;
// 			break;
// 		case 2:	//sequence of A ResutRecord
// 			wOffset	+= rx_RECS_Reponse(&pDataAPDU[wOffset+1])+1;
// 			break;
// 		}	
// 		break;
// 	}
// 	//跟随上报域
// 	if(pDataAPDU[wOffset++] == 2)
// 	{
// 	//.	Rx_Event_Reponse(&pDataAPDU[wOffset],0);
// 	}
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : gdw698M_RxMonitor
//	函数功能 : 接收处理
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年6月13日
//	返 回 值 : void
//	参数说明 : TPort *pPort
///////////////////////////////////////////////////////////////
void gdw698M_RxMonitor(TPort *pPort)
{
	TTwoByteUnion frameLen = {0};
	TFrame698_FULL_BASE afnBase = {0};
	TMGDW698 *p=(TMGDW698 *)GetMGyApp();
	BYTE task_no=get_698_task_no();
	// TOad60130200 *ptaskCfg=pGet698NoTaskCfg(task_no);
	BYTE *pDataAPDU = NULL;
	#if (SEL_SEARCH_METER == YES )
	BOOL bRc = TRUE;
	BYTE byTmpData[7];
	DWORD oadd;
	TSA sa;
	#endif
	
	if ( FR_OK != objSmockRead( &afnBase, (BYTE*)GetRxFm() ) )
	{//格式错误
		return;
	}

	pDataAPDU = afnBase.pDataAPDU;	
	p->wAPDUSize = objGetAPDUSize(&afnBase);
	
	#if (SEL_SEARCH_METER == YES )
    if ( (SKMeter.byAutoCheck & BIT0) && (SKMeter.byGyType == ePROTOCOL_698) )
    {
        if((pDataAPDU[0] != 0x85)||(pDataAPDU[1] != 1))
            bRc = FALSE;
        gdw698buf_OAD(&pDataAPDU[3],&oadd,TD12);
        if(oadd != SEARCH_METER_698_OAD)
            bRc = FALSE;
        if(pDataAPDU[7] != 1)
            bRc = FALSE;
        if((pDataAPDU[8] != 9)||(pDataAPDU[9] != 6))
            bRc = FALSE;
        if (bRc)
        {
                if( (GetPortNo() == SEARCH_METER_CH) &&
                    ( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) ) 
                )
                {
                    memcpy(&byTmpData[0],&pDataAPDU[9],sizeof(byTmpData));
                    byTmpData[0]--;
                    gdw698buf_TSA(&byTmpData[0],&sa,TD12,FALSE);
                    api_Addr2MeterNo(sa.addr, ePROTOCOL_698, SKMeter.byBaud, SEARCH_METER_CH_OAD,TRUE);
                    return;
                }
        }
    }
	#endif

	// if(pDataAPDU[0] == Security_Response)
	// {//数据解密
	// 	if(!api_analysisClearTextMAC((BYTE*)GetRxFm(),p->Rand,&frameLen))
	// 	{
	// 		return;
	// 	}
	// 	//解密完后再次解析
	// 	if ( FR_OK != objSmockRead( &afnBase, (BYTE*)GetRxFm() ) )
	// 	{//格式错误
	// 		return;
	// 	}
	// 	pDataAPDU	= afnBase.pDataAPDU;	
	// 	p->wAPDUSize = objGetAPDUSize(&afnBase);
	// }

	// if(pDataAPDU[0] == GET_Response)
	// {
	// 	if((GetUserType() == MAR_SHAN_DONG) && (GetCurveSource() != 0x55) && p->byCJTypeFlag)
	// 	{
	// 		if(pDataAPDU[1] == 0x02)
	// 		{
	// 			memset(p->DataBuffer,0,sizeof(p->DataBuffer));
	// 			p->wAPDUSize = ResultNormal_2_RecodRow(pDataAPDU,p->DataBuffer);
	// 			if(p->wAPDUSize == 0)
	// 			{
	// 				p->byCJTypeFlag = 0;
	// 				return;
	// 			}
	// 			pDataAPDU	= p->DataBuffer;
	// 		}
	// 	}
	// 	rs_Get(&pDataAPDU[1]);
	// 	p->byCJTypeFlag	= 0;
	// }
}

#endif