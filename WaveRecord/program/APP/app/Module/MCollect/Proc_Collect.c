//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "flats.h"
#include "./GDW698/GDW698DataDef.h"
#include "./GDW698/GDW698.h"
#include "./GDW698/Client/MGDW698.h"
#include "MCollect.h"
#include "Mid.h"
#include "Dlt698_45.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
TJLTxBuf JLTxBuf = {0}; //ֻ����һ���˿��ڼ���͸��״̬
TFPara2 tfpara2Ram;//���ݳ�ʼ������״̬
// TFPara2* const	tfpara2bak = &tfpara2Ram;
extern TMetCfg_Base gMetCfg[];

static BYTE CjPorts[MAX_DWNPORT_NUM] = {eRS485_I, eCR}; //����ʵ�������д

static TMaster _s_MasterArray[MAX_DWNPORT_NUM];
static TGyDbase _s_DbaseArray[MAX_DWNPORT_NUM];
static TMGDW698 _s_M698Array[MAX_DWNPORT_NUM];
static BYTE _s_SchBuf[MAX_DWNPORT_NUM][sizeof(TOad60150200)+30];
BYTE PLock[MAX_DWNPORT_NUM];
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
//static BOOL DataCjDaemon(BYTE Ch);
static BOOL DealProxyFrame(BYTE *pBuf, TJLTxBuf *pJL);
void ResetComRate(BYTE Ch, BYTE type);
//-----------------------------------------------
//				��������
//-----------------------------------------------
// extern void gdw698_RxMonitor(TPort *pPort);
extern BOOL gdw698M_TxMonitor(TPort *pPort);
extern void gdw698M_RxMonitor(TPort *pPort);
extern BOOL MDlt645_2007_TxMonitor(HPARA hPort);
extern void MDlt645_2007_RxMonitor(HPARA hPort);
extern void fresh_task_runinfo(BYTE byNo,BYTE byType);
extern void CheckClearCurvBlock(DWORD dwNewTime);
extern void InitNewCurvBlock(DWORD dwTime);
extern void InitNewFrzBlock(BYTE byType,DWORD dwTime);
void No2Baud(BYTE *pbyBaudNo,DWORD *pdwBaud,BOOL bDirect);
//-----------------------------------------------
//��������  : JLTxBuf�ṹ���ʼ��
//
//����  :    pJL��JLTxBuf�ṹ��ָ��
//
//����ֵ:    ��
//��ע����  : 
//-----------------------------------------------
void JLTxBufInit(TJLTxBuf *pJL)
{
    memset(pJL,0,sizeof(TJLTxBuf));
}

//-----------------------------------------------
//��������: ��ȡ����ַ
//
//����: 	��
//			
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void GetMeterAddr(BYTE *pAddr)
{
	if (NULL != pAddr)
	{
		memcpy(pAddr, (BYTE *)&FPara1->MeterSnPara.CommAddr[0],6);
	}
}

void JLWriteRx(BYTE Ch,BYTE *hBuf,DWORD dwLen)
{
	TTwoByteUnion frameLen = {0};
    BYTE byMPNo;
    
	if ( (Ch >= MAX_COM_CHANNEL_NUM)
	||(hBuf == NULL)
	||(JLTxBuf.byValid != JLS_RX) 
	||(JLTxBuf.byDestNo != Ch))
	{
		JLTxBuf.wRxWptr = 0;
		return;
	}
	
	//͸��
	if(JLTxBuf.ModeSwitch == 1)
	{
		if( dwLen > TRANS_BUF_LEN )
		{
			JLTxBuf.byDar = dar_scope_of_access_violated;
			JLTxBuf.wRxWptr = 0;
			return;
		}
		memcpy(JLTxBuf.TransRxBuf,hBuf,dwLen);
		JLTxBuf.wRxWptr = dwLen;
		return;
	}

	// //������1�ʹ�����2
    // if( (JLTxBuf.proxy_choice == 1) || (JLTxBuf.proxy_choice == 2))
    // {     
	// 	//��֧�ַ�֡����
	// 	if(hBuf[3]&BIT5)
	// 	{
	// 		JLTxBuf.byDar = dar_long_get_aborted;
	// 		JLTxBuf.wRxWptr = 0;
	// 		return;
	// 	}
    //     //���ݻ���ͨ�ŵ�ַ��ȡ������
    //     byMPNo = FindMeterMPNo(&hBuf[5]);
    //     //���������̶�Ϊ����+RN��ʽ��ȡ
    //     if( (JLTxBuf.proxy_choice == 2) || IsUseSafeMode(byMPNo) )
    //     {
    //         if(api_analysisClearTextMAC(hBuf,s_ReadMeterMAC,&frameLen) != TRUE)
    //         {
    //             JLTxBuf.byDar = dar_esam_failure;
    //             JLTxBuf.wRxWptr = 0;
    //             return;
    //         }
    //         //������ʼ�ַ��ͽ����ַ�����
    //         dwLen = frameLen.w + (DLT_START_BYTE+DLT_END_BYTE);
    //     }
	// }

	// if( (JLTxBuf.proxy_choice == 1) 
    //   ||(JLTxBuf.proxy_choice == 2)
    //   ||(JLTxBuf.proxy_choice == 7)
    // )
	// {   
	// 	if( dwLen > PROXY_BUF_LEN )
	// 	{
	// 		JLTxBuf.byDar = dar_scope_of_access_violated;
	// 		JLTxBuf.wRxWptr = 0;
	// 		return;
	// 	}
	// 	memcpy(JLTxBuf.TransRxBuf,hBuf,dwLen);
	// 	JLTxBuf.wRxWptr = dwLen;
	// 	return;
	// }
	// else
	// {
	// 	JLTxBuf.byDar = dar_other_reason;
	//     JLTxBuf.wRxWptr = 0;
	// }
}

//-----------------------------------------------
//��������: �ɼ���ģʽ�������˿�д����
//
//����: 	��
//			
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void CollectorWriteTx(BYTE sourceCh,BYTE *hBuf,BYTE *pMeterAddr,DWORD dwLen)
{
	BYTE i;
	if(JLTxBuf.byValid != JLS_IDLE)
	{
		return;
	}
	JLTxBufInit(&JLTxBuf);
	memcpy(JLTxBuf.TransTxBuf,hBuf,dwLen);
	JLTxBuf.wTxWptr = dwLen;
	JLTxBuf.bySourceNo   = sourceCh;
	JLTxBuf.byValid      = JLS_TX;
	JLTxBuf.wRxWptr	     = 0;
	JLTxBuf.ModeSwitch   = 1;//�ɼ���ģʽ
	
	//�鵵�� Ѱ��wMPNo
	i = FindMeterMPNo(pMeterAddr);

	if(i != 0 )
	{
		JLTxBuf.byDestNo  =  gMetCfg[i].byPortNo-1;
		JLTxBuf.Dcb.baud  = gMetCfg[i].byBaud; 
	}
	else if( (MessageTransParaRam.AllowUnknownAddrTrans == TRUE) &&(sourceCh != eRS485_I) )
	{
		JLTxBuf.byDestNo =  eRS485_I;
		JLTxBuf.Dcb.baud = MessageTransParaRam.Baud; 
	}
	else
	{
		JLTxBufInit(&JLTxBuf);
		return;
	}

	JLTxBuf.wTimeOut = MessageTransParaRam.Timeout;
	if(JLTxBuf.wTimeOut > 100)
	{
		JLTxBuf.wTimeOut = 100;
	}
	else if(JLTxBuf.wTimeOut < 5)
	{
		JLTxBuf.wTimeOut = 5;
	}
	else
	{
		
	}
}
//-----------------------------------------------
//��������: ���˿��Ƿ��ڼ���״̬
//
//����: 	��
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL IsJLing(BYTE Ch)
{
	if (JLTxBuf.byValid == JLS_IDLE)
		return FALSE;
//	if (Ch == JLTxBuf.bySourceNo)
//		return TRUE;
	if (Ch == JLTxBuf.byDestNo)
		return TRUE;
	return FALSE;
}

BOOL IsJLAddr(BYTE *pMeterAddr)
{
	if (JLTxBuf.byValid == JLS_IDLE)
		return FALSE;
    if (memcmp(JLTxBuf.Addr, pMeterAddr,6) != 0)
		return FALSE;
	return TRUE;
}
//-----------------------------------------------
//��������: ����Ƿ�Ϊ����ͨ�ű���
//
//����: 	
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL IsUpCommuFrame(BYTE byGy, BYTE *pBuf)
{
	if(PRO_NO_SPECIAL == byGy)
	{
		T645FmRx *pFm = (T645FmRx *)&pBuf[0];
		
		if (pFm->byCtrl & 0x80)
		{
			return TRUE;
		}
	}
	else if(PRO_DLT698_45 == byGy)
	{
		TFrame698_FULL_BASE afnBase;
		
		objSmockRead( &afnBase, pBuf );
		
        if ((afnBase.pFrame->byCtrl & 0xC0) == 0xC0)
		{
			return TRUE;
		}
	}
	return FALSE;
}


void api_ExtAgent(TMidParaData *MidParaData)
{
	DWORD dwDestPort;
	BYTE *pData;
	
	if(JLTxBuf.byValid != JLS_IDLE)
	{
		return;
	}
	
	memcpy(JLTxBuf.Addr,FPara1->MeterSnPara.CommAddr,6);
	JLTxBuf.PIID = MidParaData->pAPDU[2];
	JLTxBuf.ClientAddr = MidParaData->ClientAddress;
	
	pData = (BYTE*)&MidParaData->pAPDU[3];
	dwDestPort = MDW(pData[0],pData[1],pData[2],pData[3]);
	if(dwDestPort == 0xF2010201)
		JLTxBuf.byDestNo = eRS485_I;
	else if(dwDestPort == 0xF2090201)
		JLTxBuf.byDestNo = eCR;
//	else if(dwDestPort == 0xF20B0201)
//		JLTxBuf.byDestNo = eBlueTooth;
	// else if(dwDestPort == 0xF2210201)
	// 	JLTxBuf.byDestNo = eCAN;
	else return;
	pData += 4;
	JLTxBuf.Dcb.baud = pData[0];
	JLTxBuf.Dcb.parity = pData[1];
	JLTxBuf.Dcb.databits = pData[2];
	JLTxBuf.Dcb.stopbits = pData[3];
	JLTxBuf.Dcb.flowctrl = pData[4];
	pData += 5;
	JLTxBuf.wTimeOut = MW(pData[0],pData[1]);
	pData += 4;
	pData += gdw698Buf_Num(pData,&JLTxBuf.wTxWptr,TD12);
	if(JLTxBuf.wTxWptr > (sizeof(JLTxBuf.TransTxBuf)-4))
		JLTxBuf.wTxWptr = (sizeof(JLTxBuf.TransTxBuf)-4);
	//��4��fe����֡
	memset(JLTxBuf.TransTxBuf,0xFE,4);
	memcpy(&JLTxBuf.TransTxBuf[4],pData,JLTxBuf.wTxWptr);
	JLTxBuf.wTxWptr += 4;
	
	//��ʱʱ�����
	if (JLTxBuf.wTimeOut == 0)
		JLTxBuf.wTimeOut = 5;
	if (JLTxBuf.byDestNo == eCR)
	{
        if (JLTxBuf.wTimeOut > 30)
			JLTxBuf.wTimeOut = 30;
	}
	else
	{
		if (JLTxBuf.wTimeOut > 10)
			JLTxBuf.wTimeOut = 10;
	}
	JLTxBuf.bySourceNo   = eEXP_PRO;
	JLTxBuf.byValid      = JLS_TX;
	JLTxBuf.wRxWptr	     = 0;	
}
//-----------------------------------------------
//��������: �ж��Ƿ�Ϊ645����
//
//����: 	��
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL Search645Frame2(BYTE *pInBuf,WORD wLen)
{
	T645FmRxEx *p645Fm;
	WORD wI;
	
	for (wI = 0; wI < wLen; wI++)
	{
		if (pInBuf[wI] == 0x68)
			break;
	}
	if (wI >= wLen)
		return FALSE;
	p645Fm = (T645FmRxEx*)&pInBuf[wI];
	
	if (p645Fm->byE68 != 0x68)
		return FALSE;
    if (pInBuf[wI+p645Fm->byDataLen+11] != 0x16)
		return FALSE;
	if (pInBuf[wI+p645Fm->byDataLen+10] != lib_CheckSum(&pInBuf[wI],p645Fm->byDataLen + 10))
		return FALSE;
	
	return TRUE;
}

BOOL Search698Frame(BYTE *pInBuf,WORD wLen)
{
	TFrame698_FULL_BASE afnBase;
	WORD wCheck;	
	WORD wI;
	
	for (wI = 0; wI < wLen; wI++)
	{
		if (pInBuf[wI] == 0x68)
			break;
	}
	if (wI >= wLen)
		return FALSE;
	
	objSmockRead( &afnBase, (BYTE*)&pInBuf[wI] );
	
	if (afnBase.pFrame->wFmLen > wLen)
		return FALSE;
	wCheck = fcs16((BYTE*)(&afnBase.pFrame->wFmLen), (WORD)(afnBase.pFrame->wFmLen-2) );	//�ж�У���
	if( wCheck != afnBase.pTail->wCheckCS)
		return FALSE;
	if(afnBase.pTail->byEnd	!= 0x16)
		return FALSE;
	
	return TRUE;
}

WORD AddProTransmit(BYTE *pInBuf,WORD wLen)
{
	WORD wHeadLen,wAPDULen=0;
	BYTE pAPDU[512];
	DWORD dwPortOAD=0xF2090201;

	pAPDU[wAPDULen++]	= PROXY_Response;
	pAPDU[wAPDULen++]	= 0x07;
	pAPDU[wAPDULen++]	= JLTxBuf.PIID;
	
	if(JLTxBuf.byDestNo == eRS485_I)
		dwPortOAD = 0xF2010201;
	else if(JLTxBuf.byDestNo == eCR)
		dwPortOAD = 0xF2090201;
//	else if(JLTxBuf.byDestNo == eBlueTooth)
//		dwPortOAD = 0xF20B0201;
	// else if(JLTxBuf.byDestNo == eCAN)
	// 	dwPortOAD = 0xF2210201;
	wAPDULen += gdw698buf_OAD(&pAPDU[wAPDULen],&dwPortOAD,TD21);
	if (wLen == 0) //������
	{
		pAPDU[wAPDULen++]	= 0;
		pAPDU[wAPDULen++]	= 33; //DAR
	}
	else
	{
		pAPDU[wAPDULen++]	= 1;
		wAPDULen += gdw698Buf_Num(&pAPDU[wAPDULen],&wLen,TD21);
		memcpy(&pAPDU[wAPDULen],pInBuf,wLen);
		wAPDULen += wLen;
	}
	pAPDU[wAPDULen++]	= 0; //follow
	pAPDU[wAPDULen++]	= 0; //��ʱ���ǩ	

	api_ExtAgencyPro(pAPDU,wAPDULen,0);
	
	return (WORD)wAPDULen;	
}

//-----------------------------------------------
//��������: �Ӵ���͸���ظ����Ŀ�
//
//����: 	��
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
WORD AddProxyShell(TJLTxBuf *pJL,BYTE *pOutBuf)
{
	TFrame698_FULL_BASE afnBase = {0};
	TSA sa  ={0};
	DWORD dwPortOAD=0xF2090201;
	WORD wHeadLen = 0,wAPDULen = 0,wTmpLen= 0;
	BYTE *pAPDU = NULL, *pData = NULL,MessageBuf[MAX_PRO_BUF*2];
	BYTE i = 0,j = 0,tmpDar = 0;

	if((pJL == NULL)
	||(pOutBuf == NULL))
    {
        return FALSE;
    }
	
    memcpy(sa.addr,JLTxBuf.Addr,6);
	sa.AF = 5;
	wHeadLen= MakeFrameHead(&sa,MessageBuf,JLTxBuf.ClientAddr);
	pAPDU	= &MessageBuf[wHeadLen];
	
	if (pJL->proxy_choice == 1)
	{
		pAPDU[wAPDULen++]	= PROXY_Response;
		pAPDU[wAPDULen++]	= 0x01;
		pAPDU[wAPDULen++]	= JLTxBuf.PIID;
		pAPDU[wAPDULen++]	= JLTxBuf.proxy_01_listNum; //SEQUENCE OF
		
		for (i = 0; i < JLTxBuf.proxy_01_listNum; i++)
		{
			if (JLTxBuf.proxy_01_listNum == 1)
			{
				objSmockRead( &afnBase, pJL->TransRxBuf );
				tmpDar = pJL->byDar;
			}
			else
			{
				if(i == 0)
				{
					objSmockRead( &afnBase, pJL->cacheTransRxBuf );
					tmpDar = pJL->byCacheDar;
				}
				else
				{
					objSmockRead( &afnBase, pJL->TransRxBuf );
					tmpDar = pJL->byDar;
				}
			}
				
			if(afnBase.pDataAPDU[0] == 0xee)//�쳣��Ӧ
			{
				tmpDar = dar_other_reason;
			}
			
			if(tmpDar)
			{
				if((JLTxBuf.proxy_01_listNum > 1) && (i == 0))
				{
					objSmockRead( &afnBase, &pJL->cacheTransTxBuf[4] );
				}
				else
				{
					objSmockRead( &afnBase, &pJL->TransTxBuf[4] );
				}

				wAPDULen += gdw698buf_TSA(&pAPDU[wAPDULen],afnBase.pObjTsa,TD21,TRUE);
                if(pJL->proxy_01_oadNum < ArraySize(pJL->proxy_oads))
                {
                    pAPDU[wAPDULen++]	= pJL->proxy_01_oadNum; //SEQUENCE OF
                }
                else
                {
                    pAPDU[wAPDULen++]	= ArraySize(pJL->proxy_oads); //SEQUENCE OF
                }
				for (j = 0; (j < pJL->proxy_01_oadNum)&&(j < ArraySize(pJL->proxy_oads)); j++)
				{
					wAPDULen += gdw698buf_OAD(&pAPDU[wAPDULen],&pJL->proxy_oads[j],TD21);
					pAPDU[wAPDULen++] = 0;
					pAPDU[wAPDULen++] = tmpDar;
				}
			}
			else
			{
				wAPDULen += gdw698buf_TSA(&pAPDU[wAPDULen],afnBase.pObjTsa,TD21,TRUE);
				pAPDU[wAPDULen++]	= pJL->proxy_01_oadNum; //SEQUENCE OF
				if(afnBase.pDataAPDU[1] == 0x01) 
				{
					wTmpLen = getAResultNomalLen(1,&afnBase.pDataAPDU[3]);
					memcpy(&pAPDU[wAPDULen],&afnBase.pDataAPDU[3],wTmpLen);
				}
				else
				{
					wTmpLen = getAResultNomalLen(afnBase.pDataAPDU[3],&afnBase.pDataAPDU[4]);
					memcpy(&pAPDU[wAPDULen],&afnBase.pDataAPDU[4],wTmpLen);
				}
				wAPDULen += wTmpLen;
			}  
		}	
	}
	else if(pJL->proxy_choice == 2)
	{
		pAPDU[wAPDULen++]	= PROXY_Response;
		pAPDU[wAPDULen++]	= 0x02;
		pAPDU[wAPDULen++]	= JLTxBuf.PIID;
		
		if(pJL->byDar)
		{
			objSmockRead( &afnBase, &pJL->TransTxBuf[4] );
			wAPDULen += gdw698buf_TSA(&pAPDU[wAPDULen],afnBase.pObjTsa,TD21,TRUE);
			
			pData = (BYTE*)&afnBase.pDataAPDU[0];
			if(pData[0] == GET_Request)
				pData += 3;
			else
				pData += 6;
			
			wTmpLen = 4;
			memcpy(&pAPDU[wAPDULen],&pData[0],wTmpLen);
			wAPDULen += wTmpLen;
			pData += wTmpLen;
			
			wTmpLen	= objGetDataLen(dt_RSD,&pData[0],MAX_NUM_NEST);
			pData += wTmpLen;
			
			wTmpLen	= objGetDataLen(dt_RCSD,&pData[0],MAX_NUM_NEST);
			
            memcpy(&pAPDU[wAPDULen],&pData[0],wTmpLen);
			wAPDULen += wTmpLen;
			
			pAPDU[wAPDULen++]	= 0;
			pAPDU[wAPDULen++]	= pJL->byDar; //DAR
		}
		else
		{
			objSmockRead( &afnBase, pJL->TransRxBuf );
			wAPDULen += gdw698buf_TSA(&pAPDU[wAPDULen],afnBase.pObjTsa,TD21,TRUE);
			if(afnBase.pDataAPDU[0] == GET_Response)
			{
				wTmpLen = getAResultRecordLen(&afnBase.pDataAPDU[3]);
				memcpy(&pAPDU[wAPDULen],&afnBase.pDataAPDU[3],wTmpLen);
			}
			else if(afnBase.pDataAPDU[0] == 0xee)//�쳣��Ӧ
			{
				memcpy(&pAPDU[wAPDULen],JLTxBuf.proxy_oads,4);  //oad
                pAPDU[wAPDULen+4] = 0;  //RCSD	Ĭ��Ϊ0
                pAPDU[wAPDULen+5] = 0;  //DAR
                pAPDU[wAPDULen+6] = dar_other_reason;//DAR����
                wTmpLen = 7;
			}
			else
			{
				wTmpLen = getAResultRecordLen(&afnBase.pDataAPDU[6]);
				memcpy(&pAPDU[wAPDULen],&afnBase.pDataAPDU[6],wTmpLen);
			}	
			wAPDULen += wTmpLen;
		}
	}
	else
	{
		pAPDU[wAPDULen++]	= PROXY_Response;
		pAPDU[wAPDULen++]	= 0x07;
		pAPDU[wAPDULen++]	= JLTxBuf.PIID;
		
		if(JLTxBuf.byDestNo == eRS485_I)
			dwPortOAD = 0xF2010201;
		else if(JLTxBuf.byDestNo == eCR)
			dwPortOAD = 0xF2090201;
//		else if(JLTxBuf.byDestNo == eBlueTooth)
//			dwPortOAD = 0xF20B0201;
		// else if(JLTxBuf.byDestNo == eCAN)
		// 	dwPortOAD = 0xF2210201;
		wAPDULen += gdw698buf_OAD(&pAPDU[wAPDULen],&dwPortOAD,TD21);
		if (pJL->byDar) 
		{
			pAPDU[wAPDULen++]	= 0;
			pAPDU[wAPDULen++]	= pJL->byDar; //DAR
		}
		else
		{
			pAPDU[wAPDULen++]	= 1;
			wAPDULen	+= gdw698Buf_Num(&pAPDU[wAPDULen],&pJL->wRxWptr,TD21);
			memcpy(&pAPDU[wAPDULen],pJL->TransRxBuf,pJL->wRxWptr);
			wAPDULen	+= pJL->wRxWptr;
		}
	}
	pAPDU[wAPDULen++]	= 0; //follow

    if(APPData[JLTxBuf.bySourceNo].TimeTagFlag != eNO_TimeTag)
    {
	  pAPDU[wAPDULen++] = 1;
      memcpy(&pAPDU[wAPDULen],g_TimeTagBuf,10);
      wAPDULen += 10;
    }
    else
    {
        pAPDU[wAPDULen++]	= 0; //��ʱ���ǩ
    }
    
	if(JLTxBuf.SafeMode > 1)
	{
		//pAPDU[2] = wAPDULen - 3;
		wAPDULen = TremainProcSecurityResponseData(JLTxBuf.SafeMode,pAPDU,JLTxBuf.ClientAddr,wAPDULen);
	}
	Add698FrameTail(0xC3,(WORD)(wAPDULen+wHeadLen-1),MessageBuf);
	
    if((wAPDULen+wHeadLen+3)>MAX_PRO_BUF)
    {
        return FALSE;
    }
    else
    {
        memcpy(pOutBuf,MessageBuf,(wAPDULen+wHeadLen+3));
        return (WORD)(wAPDULen+wHeadLen+3);
    }
}

WORD Add13762Shell(BYTE *pInBuf,WORD wLen,BYTE *pOutBuf)
{
	BYTE DefBuf[29] = {0x68,0x2F,0x00,0x43,
		0x04,0x00,0x28,0x00,0x00,0x1A,
		0x16,0x96,0x00,0x80,0x12,0x22, //Դ��ַ
		0x00,0x00,0x00,0x00,0x00,0x00, //Ŀ�ĵ�ַ
		0x13,0x01,0x00,
		0x02, //ͨ��Э������
		0x00,
		0x00,
		0x10, //���ĳ���
	};
    BYTE source_addr_oft = 10;
	BYTE dest_addr_oft = 16;
	BYTE protocol_oft = 25;
	BYTE lenth_oft = 28;
	WORD wI,wDataLen;
	BYTE *pMeterNo = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);
	
	if((pInBuf == NULL)
	||(pOutBuf == NULL))
    {
        return FALSE;
    }
	for (wI = 0; wI < wLen; wI++)
	{
		if (pInBuf[wI] == 0x68)
			break;
	}
	if( (wI >= wLen) || (wLen > PROTOCOL_3762_MAX_LEN) )
    {
		return FALSE;
    }

	memcpy(pOutBuf,DefBuf,sizeof(DefBuf));
	if (Search645Frame2(pInBuf,wLen))
	{
        pOutBuf[protocol_oft] = 0x02;
		memcpy(&pOutBuf[dest_addr_oft],&pInBuf[wI+1],6);
	}
	else if(Search698Frame(pInBuf,wLen))
	{
		pOutBuf[protocol_oft] = 0x03;
		memcpy(&pOutBuf[dest_addr_oft],&pInBuf[wI+5],6);
	}
	else
	{
		pOutBuf[protocol_oft] = 0x00;
	}
	memcpy(&pOutBuf[source_addr_oft],pMeterNo,6);
	lib_InverseData(&pOutBuf[source_addr_oft],6);
	pOutBuf[lenth_oft] = (BYTE)wLen;
	memcpy(&pOutBuf[lenth_oft+1],pInBuf,wLen);
    wDataLen = (WORD)(29+wLen+2);
    
    if (wDataLen > PROTOCOL_3762_MAX_LEN)
    {
		return FALSE;
    }
    pOutBuf[1] = (BYTE)wDataLen;
	pOutBuf[2] = (BYTE)(wDataLen>>8);
    pOutBuf[29+wLen] = lib_CheckSum(&pOutBuf[3],29+wLen-3);
	pOutBuf[29+wLen+1] = 0x16;
	return wDataLen;
}
//-----------------------------------------------
//��������: ����Ƿ��д��ز���ά���ڷ����ı���,�������ȷ��ͳ�ȥ
//
//����: 	��
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL CheckTxJL(BYTE Ch)
{
	DWORD Baud = 9600;
	
	if (JLTxBuf.byValid == JLS_IDLE)
		return FALSE;
	
	if (JLTxBuf.byValid == JLS_TX)
	{
		if ( (Ch == JLTxBuf.byDestNo) || (JLTxBuf.byDestNo>=eEXP_PRO))
		{
			//���ж��Ƿ��д���
			if(JLTxBuf.byDar)
			{
				JLTxBuf.byValid = JLS_RX;
				JLTxBuf.dwRxStartTime = GetTicks();
				return FALSE;
			}
			//���ò�����
			switch(JLTxBuf.Dcb.baud)
			{
				case 2: Baud = 1200; break;
				case 3: Baud = 2400; break;
				case 4: Baud = 4800; break;
				case 6: Baud = 9600; break;
				case 7: Baud = 19200; break;
				case 8: Baud = 38400; break;
				case 9: Baud = 57600; break;
				case 10:Baud = 115200; break;
				case 13:Baud = 10000; break;
				case 14:Baud = 25000; break;
				case 15:Baud = 50000; break;
				case 16:Baud = 125000; break;
				default: Baud = 9600; break;
			}
			JudgeBaudChange(Ch, Baud);
			
			// if(Ch == eCR) //����ʹ��1376.2Э�鷢��
			// {
			// 	memset(Serial[Ch].ProBuf,0,sizeof(Serial[Ch].ProBuf));
			// 	Serial[Ch].SendLength = Add13762Shell(JLTxBuf.TransTxBuf,JLTxBuf.wTxWptr,Serial[Ch].ProBuf);
			// 	Serial[Ch].TXPoint = 0;
			// 	api_CommuTx(Ch);
			// }
//			else if(Ch == eBlueTooth)  //todo
//			{
//				//��������
//				Serial[Ch].SendLength = 0;
//				api_Tx_Pro_BlueTooth(JLTxBuf.TransTxBuf,JLTxBuf.wTxWptr);
//			}
			// else
			{
				memcpy( (Serial[Ch].ProBuf),JLTxBuf.TransTxBuf,JLTxBuf.wTxWptr);
				Serial[Ch].SendLength = (JLTxBuf.wTxWptr);
				Serial[Ch].TXPoint = 0;
				api_CommuTx(Ch);
			}

			JLTxBuf.byValid = JLS_RX;
			JLTxBuf.dwRxStartTime = GetTicks();
            return TRUE;
		}
	}
	
	if (JLTxBuf.byValid == JLS_RX)
	{
		if (Ch == JLTxBuf.bySourceNo)
		{
			if ((DWORD)(GetTicks() - JLTxBuf.dwRxStartTime) > ( ((DWORD)JLTxBuf.wTimeOut)*1000))
				JLTxBuf.byDar = dar_request_timeout;
			
			if (JLTxBuf.wRxWptr ||  JLTxBuf.byDar)//���յ����ݻ��ߴ������ 
			{
				if (JLTxBuf.proxy_choice == 0x01)
				{
					if (JLTxBuf.proxy_listnum_para > 1)
					{
						JLTxBuf.proxy_listnum_para--;
						//�����ݴ浽cache
						JLTxBuf.wCacheRxWptr = JLTxBuf.wRxWptr;
						memcpy(JLTxBuf.cacheTransRxBuf,JLTxBuf.TransRxBuf,sizeof(JLTxBuf.cacheTransRxBuf));
						JLTxBuf.byValid = JLS_TX;
						JLTxBuf.wRxWptr = 0;
						memset(JLTxBuf.TransRxBuf,0,sizeof(JLTxBuf.TransRxBuf));
						//���ʹ�cacheȡ��
                        lib_MemSwap(JLTxBuf.TransTxBuf,JLTxBuf.cacheTransTxBuf,sizeof(JLTxBuf.cacheTransTxBuf));
						lib_MemSwap((BYTE*)&JLTxBuf.wTxWptr, (BYTE*)&JLTxBuf.wCacheTxWptr, sizeof(JLTxBuf.wTxWptr));
                        lib_MemSwap((BYTE*)&JLTxBuf.byDar, (BYTE*)&JLTxBuf.byCacheDar, sizeof(JLTxBuf.byDar));
						return FALSE;
					}
				}
				JLTxBuf.byValid = JLS_IDLE;
				ResetComRate(Ch,1);
				if(Ch != eEXP_PRO)
				{
					memset(Serial[Ch].ProBuf,0,sizeof(Serial[Ch].ProBuf));
					if(JLTxBuf.ModeSwitch)//�ɼ���ģʽ
					{
						if(JLTxBuf.wRxWptr == 0)
                        {
							JLTxBuf.byValid = JLS_IDLE;
							JLTxBuf.ModeSwitch = 0;
							return FALSE;
                        }
						memcpy(Serial[Ch].ProBuf,JLTxBuf.TransRxBuf,JLTxBuf.wRxWptr);
                        Serial[Ch].SendLength = JLTxBuf.wRxWptr;
                        Serial[Ch].TXPoint = 0;
					}
					else
					{
						Serial[Ch].SendLength = AddProxyShell(&JLTxBuf,Serial[Ch].ProBuf);
					}
					Serial[Ch].TXPoint = 0;
//					if(Ch == eBlueTooth)  //todo
//					{
//						//��������
//						api_mt_write_ble_ch_data(Serial[Ch].ProBuf,Serial[Ch].SendLength,bBleChannel);	
//						Serial[Ch].SendLength = 0;
//					}
//					else
					{
						api_CommuTx(Ch);
					}
					
				}
				else
				{
					AddProTransmit(JLTxBuf.TransRxBuf,JLTxBuf.wRxWptr);
				}
				
				JLTxBuf.byValid = JLS_IDLE;
				JLTxBuf.ModeSwitch = 0;
                return TRUE;
			}
		}
	}
	
	return FALSE;
}
//todo ������can�ĳ�ʱʱ�������
DWORD GetPortDelay(BYTE Ch)
{
	if (Ch == eCR)
		return 30000; /* �ز��˿�Ĭ��30�� */
	else
		return 3000; /* �����˿�Ĭ��3�� */
}
//--------------------------------------------------
//��������:  
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  CRCMemComRate( void )
{
	// if(lib_CheckSafeMemVerify((BYTE *)tfpara2bak, sizeof(TFPara2), 0))
	// {
	// 	return TRUE;
	// }
	// else if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2), sizeof(TFPara2), (BYTE *)tfpara2bak))
	// {
	// 	memcpy(tfpara2bak,FPara2, sizeof(TFPara2));
	// }
	return FALSE;
}
//--------------------------------------------------
// ��������:  �ڲɼ��ж�λ����ʱ�ָ�ͨ�ſڲ�����
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void ResetComRate(BYTE Ch, BYTE type)
{
	BYTE *pdata1 = NULL, *pdata2 = NULL;

	if((PLock[Ch] == 1) || ((JLTxBuf.byValid == JLS_RX) && (Ch == JLTxBuf.byDestNo)))
	{
		return;
	}
	if(CRCMemComRate() != TRUE)
	{
		return;
	}

	switch(Ch)
	{
	// case eRS485_I:
	// 	pdata1 = &tfpara2bak->CommPara.I485;
	// 	pdata2 = &FPara2->CommPara.I485;
	// 	break;
	// case eCR:
	// 	pdata1 = &tfpara2bak->CommPara.ComModule;
	// 	pdata2 = &FPara2->CommPara.ComModule;
	// 	break;
	// case eCAN:
	// 	pdata1 = &tfpara2bak->CommPara.CanBaud;
	// 	pdata2 = &FPara2->CommPara.CanBaud;
	// 	break;

	default:
		return;
		break;
	}

	if(*pdata1 != *pdata2)
	{
		*pdata2 = *pdata1;
		Serial[Ch].OperationFlag = 1;   // �����ʸı�
		lib_CheckSafeMemVerify((BYTE *)FPara2, sizeof(TFPara2), 1);
		if(type)
		{
			DealSciFlag(Ch);
		}
	}
}
//-----------------------------------------------
//��������: ������ʱ����
//
//����:		�� 
//                 
//����ֵ: 	BOOL
//		   
//��ע:   ����TRUE��������Խ��з���������
//-----------------------------------------------
BOOL MsgCtrl(BYTE Ch)
{
	TPort *pPort = pGetPort(Ch);
    DWORD dwDelayMax = GetPortDelay(Ch);
	
	if(Ch >= MAX_DWNPORT_NUM)
		return FALSE;
	
    //�ײ�û������
	if(Serial[Ch].TXPoint < Serial[Ch].SendLength)
		return FALSE;
	
	if(Serial[Ch].SendToSendDelay != 0)  
		return FALSE;
	
	if(pPort->Rx.dwBusy)
	{
		if((DWORD)(GetTicks() -  pPort->Rx.dwBusy) < dwDelayMax)
			return FALSE;
		// else //��ʱ�ݲ�����������
		// {
		// 	//����ʱ �ָ�485��CANͨ������ �ָ�ʱ��EEP�������Ż� todo
		// 	api_FreshParaRamFromEeprom(1);
		// 	Serial[Ch].OperationFlag = 1; // �����ʸı�
		// 	DealSciFlag(Ch);
		// }
	}
	ResetComRate(Ch,1);

	return TRUE; 
}

//-----------------------------------------------
//��������: �ɼ���ѭ������
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_MCollectTask(void)
{	
	static BYTE pollCount = 0;
	BYTE Ch;
    
	if (pollCount >= MAX_DWNPORT_NUM)
	{
		pollCount = 0;
	}
	Ch = CjPorts[pollCount++];
	
    if(!MsgCtrl(Ch))
	{
		return;
	}
	//1�����ȴ���͸��
    if(CheckTxJL(Ch))
	{
		Serial[Ch].SendToSendDelay = MAX_SEND_DELAY;
		return;
	}	

    #if (SEL_SEARCH_METER == YES )
	//2���ѱ�
	if ( (Ch == SEARCH_METER_CH) &&
		( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) )
		)
	{
		if(api_ProSearchMeter(Ch))
		{
			Serial[Ch].SendToSendDelay = MAX_SEND_DELAY;
			return;
		}
	}
    #endif
    
	//3���ɼ�
//	if (DataCjDaemon(Ch))
//	{
//		Serial[Ch].SendToSendDelay = MAX_SEND_DELAY;
// 		return;
// 	}
}

// static BOOL _SwitchToNext(TPort *pPort,WORD *pwNo)
// {//�л�����һ����Ч��ţ��Է��ز��ڲ��Զ�Ӧ�Ĳ����ʸ�λ�˿�
// 	TMaster *pMaster=(TMaster *)pPort->hApp;
// 	WORD wI,wMPNo;
// 	DWORD dwBaud;
//     BYTE byNo;

// 	wI	= (WORD)((*pwNo)+1);
// 	for(;wI<MAX_MP_NUM;wI++)
// 	{
// 		if(!No2MP(wI,&wMPNo))
// 			continue;
// 		//�жϸò������Ƿ��ڱ��˿���
// 		if(pMaster->pMPFlag[wMPNo>>3] & (1<<(wMPNo&0x07)))
// 		{
// 			TMetCfg_Base Cfg;
// 			*pwNo	= wI;
			
// 			if(sys_GetMPCfg(wI,&Cfg))
// 			{							
// 				if(!addrIsInTask(&Cfg.meter_addr,pPort,pMaster->byTaskNo))
// 					continue;
// 				//todo �޸Ķ�Ӧ���ڲ�����
// 				No2Baud(&Cfg.byBaud,&dwBaud,TRUE);
// 				byNo = PortID2No(pPort->dwID);
// 				JudgeBaudChange(byNo, dwBaud);
// 				PLock[byNo]=1;
// 				break;
// 			}			
// 		}
// 	}
// 	if(wI >= MAX_MP_NUM)
// 	{
// 		*pwNo	= 0xFFFF;
// 	}	
// 	if(pPort->GyRunInfo.pApp)
// 	{//�干��Ӧ�ÿռ�
// 		memset(pPort->GyRunInfo.pApp,0,pMaster->wAppSize);
// 	}
// 	return (*pwNo != 0xFFFF);
// }

static BOOL _CheckSetCJFlag(TPort *pPort)
{//������òɼ���ʶ
	TMaster *pMaster=(TMaster *)pPort->hApp;
	
	//�ж������һ���Ƿ���
	if(pMaster->byCJing)
		return FALSE;
	
	pPort->wMetNum	= GetMPNum(HLBYTE(pPort->dwID));
	
	if(pPort->wMetNum == 0)
		return FALSE;
	
	//��ͨ����ɼ�
	pMaster->wSetCJFlag	|= (1<<REQ_REAL);	
	pMaster->wSetCJFlag	|= (1<<REQ_DFRZ);
	pMaster->wSetCJFlag	|= (1<<REQ_MFRZ);
	pMaster->wSetCJFlag	|= (1<<REQ_CURV);
    pMaster->wSetCJFlag |= (1<<REQ_SETTLEDAY);
	return TRUE;
}

/**************************************************************
 *	�� �� �� : CheckParaDaemon
 *	�������� : ���洢�Ͳɼ�����
 *	������� : 
 *	��    ע : �뼶����
 *	��    �� : 
 *	ʱ    �� : 2023��6��30��
 *	�� �� ֵ : void
 *	����˵�� : 
**************************************************************/
void CheckParaDaemon(void)
{
	BYTE i;
	TTime Time;
	DWORD dwTime;
	TPort *pPort;
	TMaster *pMaster;

	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )
	{//���粻ִ��

		GetTime(&Time);	
		// dwTime	= Time2Min(&Time);	
		//�������ڸ��¼��
		ParaSaveDaemon(TRUE,&Time);	
		
		// for( i=0; i<MAX_DWNPORT_NUM; i++ )
		// {
		// 	pPort = pGetPort(i);
		// 	if((NULL == pPort)||(pPort->byOpen == 0))
		// 		continue;
			
		// 	pMaster	= (TMaster *)pPort->hApp;
		// 	if(dwTime == pMaster->dwOldCheckTime)
		// 		continue;
		// 	if(pMaster->byCJing == 0)
		// 	{
		// 		pMaster->dwOldCheckTime	= dwTime;
		// 	}
		// 	//����ʵʱ���ݲɼ���ʶ(ÿ�����ж�һ��)
		// 	_CheckSetCJFlag(pPort);		
		// }
		
		// if((Time.Hour != gVar.GyTime.Hour) 
		// 	||(Time.Day != gVar.GyTime.Day)
		// 	||(Time.Mon != gVar.GyTime.Mon)
		// 	||(Time.wYear != gVar.GyTime.wYear))
		// {//Сʱ�仯
		// 	DWORD dwTime2 = Time2Min(&gVar.GyTime);
			
		// 	if(dwTime < dwTime2)
		// 	{//���ض�ʱ�ˣ�����Ҫ��Ҫ�����ǰ���ݿ��ʱ��
		// 		CheckClearCurvBlock(dwTime);
		// 	}
		// 	else
		// 		InitNewCurvBlock(dwTime);
		// }
		
		// if((Time.Day != gVar.GyTime.Day)
		// 	||(Time.Mon != gVar.GyTime.Mon)
		// 	||(Time.wYear != gVar.GyTime.wYear))
		// {//�շ����仯			
		// 	InitNewFrzBlock(REQ_DFRZ,dwTime);
		// }
		
		// if((Time.Mon != gVar.GyTime.Mon)
		// 	||(Time.wYear !=gVar.GyTime.wYear))
		// {//�·����仯
		// 	InitNewFrzBlock(REQ_MFRZ,dwTime);
		// }	

		// if(	(Time.Min != gVar.GyTime.Min) 
		// 	||(Time.Hour != gVar.GyTime.Hour) 
		// 	||(Time.Day != gVar.GyTime.Day)
		// 	||(Time.Mon != gVar.GyTime.Mon)
		// 	||(Time.wYear != gVar.GyTime.wYear))
		// {//���ӱ仯
		// 	DB_SaveData(dwTime);	
		// }
		//���¹�Լ�˿�ʱ��
		// memcpy(&gVar.GyTime,&Time,sizeof(TTime));
	}
	else//��������ִ�д洢
	{
		ParaSaveDaemon(FALSE,NULL);	
	}
}

BYTE get_698_task_no(void)
{
	TPort *pPort=(TPort *)GethPort();
	TMaster *pMaster=(TMaster *)pPort->hApp;
	
	return pMaster->byTaskNo;
}

static void _SwitchToCJStatus(TPort *pPort, BYTE byNewStatus)
{
	pPort->GyRunInfo.byCJStatus = byNewStatus;
}


BOOL IsDlt698_45(WORD wNo)
{
	TMetCfg_Base Cfg;
	
	if(sys_GetMPCfg(wNo,&Cfg))
	{
        if(Cfg.byProtocolNo == 3) 
		{
			return TRUE;
		}
	}
	return FALSE;
}

// static BOOL _Dlt698_Txmonitor(BYTE Ch)
// {
// 	TPort *pPort = pGetPort(Ch);
// 	BOOL bRc = FALSE;

//     SetRunPort(Ch);
	
// 	bRc = gdw698M_TxMonitor(pPort);

// 	return bRc;
// }

void Dlt698_RxMonitor(BYTE Ch)
{	
	TPort *pPort = pGetPort(Ch);

	SetRunPort(Ch);
	
	gdw698M_RxMonitor(pPort);
}

// static BOOL _Dlt645_Txmonitor(BYTE Ch)
// {
// 	TPort *pPort = pGetPort(Ch);
// 	BOOL bRc = FALSE;

//     SetRunPort(Ch);

// 	bRc = MDlt645_2007_TxMonitor(pPort);

// 	return bRc;
// }

void Dlt645_RxMonitor(BYTE Ch)
{
	TPort *pPort = pGetPort(Ch);

	SetRunPort(Ch);
	
	MDlt645_2007_RxMonitor(pPort);
}

BOOL MGyApp_Init(BYTE Ch)
{
	TPort *pPort = pGetPort(Ch);
	TMaster *pMaster;
	// DWORD dwTime = GetLastRunMins();
	
	memset(&pPort->GyRunInfo,0,sizeof(TGyRunInfo));
	
	pPort->GyRunInfo.hGyDB = (HPARA)&_s_DbaseArray[Ch];//MemAlloc(sizeof(TGyDbase));
	
	pMaster	= &_s_MasterArray[Ch];//MemAlloc(sizeof(TMaster));			
	pMaster->byCJing	= 0;			
	pPort->hApp	= pMaster;
	pPort->byMode = MGYM_MASTER;
	
	//�˿��²������ʶ
	pMaster->pMPFlag= pGetPortMPFlag(HLBYTE(pPort->dwID));			
	
	//���������Լ��������ռ�
	pMaster->wAppSize	= sizeof(TMGDW698); //GetMGyAppSize();
	
	pPort->GyRunInfo.pApp	= (BYTE*)&_s_M698Array[Ch]; //MemAlloc(pMaster->wAppSize);
				
	pPort->GyRunInfo.psch698	= &_s_SchBuf[Ch][0]; //MemAlloc(get_max_sch_size());
	
	// pMaster->dwOldCheckTime	= dwTime;
	
	_CheckSetCJFlag(pPort);	
	
	return TRUE;
}

// BOOL DataCjDaemon(BYTE Ch) 
// {//����ʽ����
//     TPort *pPort = pGetPort(Ch);
//     TMaster *pMaster=(TMaster *)pPort->hApp;
//     TGyDbase *pGyDB;
//     WORD wMPNo,wI;
//     BOOL bNeedTx=FALSE;
//     verifyMacInfo_t verifyMacInfo = {0};
	
// 	switch(pPort->GyRunInfo.byCJStatus)
// 	{
// 	default: //��ȷ��״̬
// 		_SwitchToCJStatus(pPort,STS_CHK_CJ_FLAG);		
// 		return FALSE;
		
// 	case STS_CHK_CJ_FLAG: //���ɼ�״̬			
// 		for(wI=0;wI<MAX_REQ_TYPE;wI++)
// 		{
// 			if(pMaster->wSetCJFlag & (1<<wI))
// 				break;
// 		}		
// 		if(wI >= MAX_REQ_TYPE)
// 		{//�������������ѳ���,���³���״̬						
// 			pMaster->byCJing	= 0;
// 			pPort->GyRunInfo.byReqType= 0;			
// 			return FALSE;
// 		}				
// 		//����ʽ(�ӵ�1���ʼ����)
// 		pMaster->byTaskNo		= 0xFF;
// 		pPort->GyRunInfo.wMPSeqNo= 0xFFFF;			
// 		pMaster->dwCheckTime	= pMaster->dwOldCheckTime;
		
// 		memset(&pPort->GyRunInfo.CurvCjCfg,0,sizeof(TCurvCjCfg));
// 		memset(&pPort->GyRunInfo.ReBuCJCfg,0,sizeof(TReCjCfg));
		
// 		pMaster->byReqType	= (BYTE)wI;				
// 		_SwitchToCJStatus(pPort,STS_SEL_CJ_TASK);		
		
// 	case STS_SEL_CJ_TASK:
		
// 		if(!SwitchToNextTask(pPort,pMaster->dwCheckTime,&pMaster->byTaskNo))
// 		{//�����͵����񳭶��������е���һ�������񳭶�	
// 			// TData_6203 *pDF6203=pGetDF6203();
// 			pMaster->wSetCJFlag = 0;	
// 			pMaster->byCJing	= 0;	
// 			pMaster->byTaskNo	= 0xFF;
// 			// pDF6203->TaskCheckTime[PortID2No(pPort->dwID)]	= pMaster->dwCheckTime;
// 			_SwitchToCJStatus(pPort,STS_CHK_CJ_FLAG);
// 			break;
// 		}	
		
// 		_SwitchToCJStatus(pPort,STS_SEL_MP);
		
// 	case STS_SEL_MP: //�л�����һ����Ч�Ĳ�����			
// 		pMaster->byCJing		= 1;
		
// 		//ָ���һ����Ч������
// 		pPort->GyRunInfo.byReqType= pMaster->byReqType;	
// 		{//698ר��(����������ָ��)
// 			pPort->GyRunInfo.byGyStep= 0;	
// 			pPort->GyRunInfo.CurvCjCfg.byCJInc   = 0;
// 			pPort->GyRunInfo.ReBuCJCfg.byReCJInc = 0;
// 			memset(&pPort->GyRunInfo.cj_698,0,sizeof(TCj698Ctrl));
// 		}		
// 		if(!_SwitchToNext(pPort,&pPort->GyRunInfo.wMPSeqNo))
// 		{//���һ���						
// 			pPort->GyRunInfo.wMPSeqNo	= 0xFFFF;	
// 			_SwitchToCJStatus(pPort,STS_SEL_CJ_TASK);	
// 			break;
// 		}			
// 		GetMPAddr(pPort->GyRunInfo.wMPSeqNo,(BYTE*)&pPort->GyRunInfo.sa);
// 		memset(pPort->GyRunInfo.Addr,0,sizeof(pPort->GyRunInfo.Addr));	
// 		memcpy(pPort->GyRunInfo.Addr,pPort->GyRunInfo.sa.addr,min((pPort->GyRunInfo.sa.AF&0x0f)+1,6));	
		
// 		{
// 			DWORD dwTmp;
// 			if(pPort->GyRunInfo.CurvCjCfg.byInit == 0x5A)//����	
// 				dwTmp	= pPort->GyRunInfo.CurvCjCfg.dwStart+pPort->GyRunInfo.CurvCjCfg.wInterval*pPort->GyRunInfo.CurvCjCfg.wNo;
// 			else
// 				dwTmp	= pPort->GyRunInfo.dwTaskTime;
			
// 			if(pPort->GyRunInfo.ReBuCJCfg.byInit == 0x5A)
// 			{
// 				BOOL bRec = FALSE;
// 				TTime getCJtime;
// 				WORD wDataLen =0;
// 				BYTE byData[50];
// 				dwTmp	= pPort->GyRunInfo.ReBuCJCfg.dwStart+pPort->GyRunInfo.ReBuCJCfg.wInterval*pPort->GyRunInfo.ReBuCJCfg.wNo;
// 				dwTmp	= get_dmfrztask_save_time(dwTmp,(TOad60150200*)pPort->GyRunInfo.psch698);
// 				dwTmp	= (dwTmp+1)/1440*1440;
// 				Min2Time(dwTmp,&getCJtime);
				
// 				if(pPort->GyRunInfo.ReBuCJCfg.wInterval==1440)
// 					bRec = get_dfrz_task_data(&pPort->GyRunInfo.sa,&getCJtime,0x00100200,byData,&wDataLen);
// 				else
// 				{
// 					bRec = get_curv_task_data(&pPort->GyRunInfo.sa,&getCJtime,0x00100201,byData,&wDataLen);
// 					if(bRec==FALSE)
// 						bRec = get_curv_task_data(&pPort->GyRunInfo.sa,&getCJtime,0x20000201,byData,&wDataLen);
// 				}				
// 				if(bRec)
// 				{
// 					_SwitchToCJStatus(pPort,STS_MP_CJ_OVER);
// 					break;
// 				}
// 			}
// 			else
// 			{
// 				if(mp_sch_cj_is_OK(pMaster->byTaskNo,pPort->GyRunInfo.psch698,&pPort->GyRunInfo.sa,dwTmp))
// 				{
// 					_SwitchToCJStatus(pPort,STS_MP_CJ_OVER);
// 					break;
// 				}
// 			}
//         }	
// 		_SwitchToCJStatus(pPort,STS_INIT_MP_CJ);
		
// 	case STS_INIT_MP_CJ: //���ݲɼ�
// 		// pPort->GyRunInfo.dwCJSecs	= Time2Sec(&pMaster->Time);//���һ�γ���ʱ��
// 		pPort->GyRunInfo.byReqDataValid= 0;			
// 		pPort->GyRunInfo.wTxCount	= 0;
// 		pPort->GyRunInfo.wOldTxCount= 0;
// 		pPort->GyRunInfo.wRxCount	= 0;			
// 		pPort->GyRunInfo.byTxIDFlag = 0xFF;
// 		pPort->GyRunInfo.byTxCurvSource = 0;		
// 		ClearAllFlag();		
// 		_SwitchToCJStatus(pPort,STS_SET_MP_DATA_FALG);	
		
// 	case STS_SET_MP_DATA_FALG: //��ʼ����ɼ�	
// 		if(No2MP(pPort->GyRunInfo.wMPSeqNo,&wMPNo))
// 		{
// 			SetRunPort(Ch);

// 			pGyDB=(TGyDbase*)pPort->GyRunInfo.hGyDB;
// 			if(NULL != pGyDB) 
// 			{
// 				memset(pGyDB,0x00,sizeof(TGyDbase));	
// 			}
			
// 			if(!SetMPCJFlag(wMPNo,pPort->GyRunInfo.byReqType,pPort->GyRunInfo.Flag))
// 			{
// 				_SwitchToCJStatus(pPort,STS_CHK_MP_CJ_FLAG);
// 				// pPort->GyRunInfo.byReqOK	= 0;			
// 				return FALSE;
// 			}
			
// 			_SwitchToCJStatus(pPort,STS_DO_MP_CJ);
// 			// if(NULL != pGyDB)
// 			// 	pGyDB->dwFreshBufFlag	= 0;
// 			// pPort->GyRunInfo.byReqOK	= 0;			
// 			return FALSE;			
// 		}
// 		else
// 		{//ָ���ĵ������
// 			_SwitchToCJStatus(pPort,STS_MP_CJ_OVER);			
// 		}
// 		break;
		
// 	case STS_DO_MP_CJ: //���������ɼ�(����)
        
// 		SetRunPort(Ch);

// 		if(pPort->GyRunInfo.wTxCount != pPort->GyRunInfo.wOldTxCount)
// 		{
// 			fresh_task_runinfo(pMaster->byTaskNo,0);
// 			pPort->GyRunInfo.wOldTxCount	= pPort->GyRunInfo.wTxCount;
// 		}
		
// 		if (IsDlt698_45(pPort->GyRunInfo.wMPSeqNo))
// 		{
//             //��ȡ�����
//             verifyMacInfo.rand = ((TMGDW698 *)pPort->GyRunInfo.pApp)->Rand;
//             api_processMacRN(GENERATE_RN,&verifyMacInfo);

// 			if (_Dlt698_Txmonitor(Ch))
// 			{
// 				bNeedTx = TRUE;
// 			}
// 		}
// 		else
// 		{
// 			if (_Dlt645_Txmonitor(Ch))
// 			{
// 				bNeedTx = TRUE;
// 			}
// 		}
		
// 		if (bNeedTx)
// 		{
// 			if (pPort->Tx.wWptr)
// 			{
// 				if(Ch == eCR) //����ʹ��1376.2Э�鷢��
// 				{
// 					memset(Serial[Ch].ProBuf,0,sizeof(Serial[Ch].ProBuf));
// 					Serial[Ch].SendLength = Add13762Shell(pPort->Tx.pBuf,pPort->Tx.wWptr,Serial[Ch].ProBuf);
// 				}				
// 				else
// 				{
// 					memcpy(Serial[Ch].ProBuf,pPort->Tx.pBuf,pPort->Tx.wWptr);
// 					Serial[Ch].SendLength = pPort->Tx.wWptr;
// 					if(Ch == eBlueTooth)
// 					{
// 						//��������
// 						api_Tx_Pro_BlueTooth(Serial[Ch].ProBuf,Serial[Ch].SendLength);
// 						Serial[Ch].SendLength = 0;
// 					}
// 				}
// 				Serial[Ch].TXPoint    = 0;
				
// 				api_CommuTx(Ch);

// 				pPort->Tx.wWptr = 0;
// 				pPort->Rx.dwBusy = GetTicks();
// 				pPort->GyRunInfo.wTxCount++;
// 			}
// 			return TRUE;
// 		}
// 		//һ����һ���������ٻ�����
// 		No2MP(pPort->GyRunInfo.wMPSeqNo,&wMPNo);
		
// 		MPRxDeal(pPort,wMPNo);
		
// 		_SwitchToCJStatus(pPort,STS_CHK_MP_CJ_FLAG);
		
// 	case STS_CHK_MP_CJ_FLAG: //�Ƿ��к������������ٻ�
// 		if((pPort->GyRunInfo.wTxCount-pPort->GyRunInfo.wRxCount) >= MAXCOUNT_NOACK)
// 		{//ͨ�Ź���
// 			_SwitchToCJStatus(pPort,STS_MP_CJ_OVER);				
// 			pPort->GyRunInfo.byReqType	= pMaster->byReqType;			
// 		}
// 		else
// 		{//�жϺ�����������Ҫ��û��			
// 			for(wI=(pPort->GyRunInfo.byReqType+1);wI<MAX_REQ_TYPE;wI++)
// 			{
// 				if(pMaster->wSetCJFlag & (1<<wI))
// 					break;
// 			}
// 			if(wI < MAX_REQ_TYPE)
// 			{//ָ����һ����������				
				
// 				if(IsDlt698_45(pPort->GyRunInfo.wMPSeqNo))
// 				{
// 					_SwitchToCJStatus(pPort,STS_MP_CJ_OVER);
// 				}
// 				else
// 				{
// 					pPort->GyRunInfo.byReqType	= (BYTE)wI;
// 					_SwitchToCJStatus(pPort,STS_INIT_MP_CJ);
// 				}				
// 			}
// 			else
// 			{//һ���ǰ�����ѳ�������,�����л�����һ������
// 				_SwitchToCJStatus(pPort,STS_MP_CJ_OVER);									
// 				pPort->GyRunInfo.byReqType	= pMaster->byReqType;
// 			}	
// 		}		
// 		break;
// 	case STS_MP_CJ_OVER: //���βɼ��ѽ���	
		
// 		_SwitchToCJStatus(pPort,STS_SEL_MP);//�е���һ���				
// 		PLock[Ch]=0;
// 		break;
// 	}
// 	return FALSE;
// }

//-----------------------------------------------
//��������: ��Լ���մ������г���
//
//����: 	��
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL GyRxMonitor(BYTE Ch,BYTE GyType)
{
	TPort *pPort = pGetPort(Ch);
	DWORD dwDlay = GetPortDelay(Ch);
	
	pPort->Rx.wRptr = 0;
	pPort->Rx.wWptr = min(pPort->Rx.wSize,Serial[Ch].RXWPoint);
	memcpy(pPort->Rx.pBuf,Serial[Ch].ProBuf,pPort->Rx.wWptr);
	
	pPort->Rx.dwBusy = (DWORD)(GetTicks() - dwDlay);
	pPort->Rx.dwBusy += 20;
	pPort->GyRunInfo.wRxCount	= pPort->GyRunInfo.wTxCount;
	if (GyType == PRO_NO_SPECIAL)
	{
		Dlt645_RxMonitor(Ch);		
	}
	else if (GyType == PRO_DLT698_45)
	{
		Dlt698_RxMonitor(Ch);
	}

	// //����ʱ �ָ�485��CANͨ������ �ָ�ʱ��EEP�������Ż� todo
	// if((Ch == eRS485_I) || (Ch == eCAN))
	// {
	// 	api_FreshParaRamFromEeprom(1);
	// 	Serial[Ch].OperationFlag = 1; // �����ʸı�
	// 	DealSciFlag(Ch);
	// }
	//ResetComRate(Ch,1);
	return TRUE;
}

//--------------------------------------------------
//��������:    698��������ȡ���ɸ������������ɸ���������
//         
//����:      BYTE Ch[in]		ͨ��ѡ��
// 
//	        pJL[in/out]			TJLTxBufָ��
//
//����ֵ:   ���ش�����
//         
//��ע����:  ��
//--------------------------------------------------
// BOOL ProxyGetRequestList( BYTE Ch ,TJLTxBuf *pJL)
// {
// 	verifyMacInfo_t verifyMacInfo = {0};
// 	TGetPara para = {0};
// 	TSA sa = {0};
// 	TMetCfg_Base Cfg={0};
// 	DWORD dwOAD = 0;
// 	BYTE byMPNo = 0,TxBuf[120] = {0};
// 	BYTE *pData = NULL,i = 0,j = 0;
	
// 	if((Ch >= MAX_COM_CHANNEL_NUM)
// 	||(pJL == NULL))
//     {
//         return FALSE;
//     }

// 	memcpy(pJL->Addr,LinkData[Ch].ServerAddr,6);
// 	pJL->PIID = LinkData[Ch].pAPP[2];
// 	pJL->ClientAddr = LinkData[Ch].ClientAddress;

// 	pData = (BYTE*)&LinkData[Ch].pAPP[3];
// 	pJL->wTimeOut = MW(pData[0],pData[1]);
// 	pData += 2;
// 	pJL->proxy_01_listNum = min(pData[0],2); //SEQUENCE OF //���֧��2ֻ��
// 	pJL->proxy_listnum_para = pJL->proxy_01_listNum;
// 	pData += 1; 

//     //���ݻ���ͨ�ŵ�ַ��ȡ������
//     gdw698buf_TSA(pData,&sa,TD12,TRUE);
//     byMPNo = FindMeterMPNo(sa.addr);

//     if(IsUseSafeMode(byMPNo))
//     {
//         para.bUseSafeMode = eSECURITY_RN;
//         //��ȡ�����
//         verifyMacInfo.rand = s_ReadMeterMAC;
//         api_processMacRN(GENERATE_RN,&verifyMacInfo);
//         para.Rand = s_ReadMeterMAC;
//     }
//     else
//     {
//         para.bUseSafeMode = eNO_SECURITY;
//     }

// 	for (i = 0; i < pJL->proxy_01_listNum; i++) 
// 	{
// 		pData += gdw698buf_TSA(pData,&sa,TD12,TRUE);
// 		byMPNo = FindMeterMPNo(sa.addr);
// 		if (byMPNo == 0)
// 		{
// 			if (i == 0)
// 				pJL->byDar = dar_address_exception;
// 			else
// 				pJL->byCacheDar = dar_address_exception;
// 		}
// 		else
// 		{
// 			sys_GetMPCfg(byMPNo,&Cfg);
// 			if (Cfg.byProtocolNo != 3) //��ʱֻ����698��Լ���
// 			{
// 				if (i == 0)
// 					pJL->byDar = dar_other_reason;
// 				else
// 					pJL->byCacheDar = dar_object_unavailable;
// 			}
// 			pJL->byDestNo = (BYTE)(Cfg.byPortNo-1);
// 			if (pJL->byDestNo >= MAX_VPORT_NUM)
// 			{
// 				if (i == 0)
// 					pJL->byDar = dar_other_reason;
// 				else
// 					pJL->byCacheDar = dar_object_unavailable;
// 			}
// 		}	
// 		JudgeCanID(byMPNo);
// 		pJL->Dcb.baud = Cfg.byBaud;
// 		pJL->Dcb.databits = 8;
// 		pJL->Dcb.parity = 2;
// 		pJL->Dcb.stopbits = 1;
// 		pData += 2;
// 		pJL->proxy_01_oadNum = pData[0]; //SEQUENCE OF��ֻ������oad����С��0x7F�����
// 		pData += 1;
		
// 		para.byType = (pJL->proxy_01_oadNum==1)?1:2;
// 		para.wNum = pJL->proxy_01_oadNum;
// 		para.pTxBuff = TxBuf;
// 		para.wBufLen = 0;

// 		for (j = 0; j<pJL->proxy_01_oadNum; j++)
// 		{
// 			dwOAD = MDW(pData[0],pData[1],pData[2],pData[3]); //OAD
// 			para.wBufLen += gdw698buf_OAD(&para.pTxBuff[para.wBufLen],&dwOAD,TD21);
// 			pData += 4;
            
// 			if(j < ArraySize(pJL->proxy_oads))
// 				pJL->proxy_oads[j] = dwOAD;
            
//             if(para.wBufLen>(sizeof(TxBuf) - 4))
//             {
//                 pJL->proxy_01_oadNum = j+1;
//                 para.wNum = pJL->proxy_01_oadNum;
//                 break;
//             }
// 		}
// 		if (i == 0)
// 		{
// 			memset(pJL->TransTxBuf,0xFE,4);
// 			pJL->wTxWptr = _TxFm698Read(&sa,&para,&pJL->TransTxBuf[4]);
// 			pJL->wTxWptr += 4;
// 		}
// 		else
// 		{
// 			memset(pJL->cacheTransTxBuf,0xFE,4);
// 			pJL->wCacheTxWptr = _TxFm698Read(&sa,&para,&pJL->cacheTransTxBuf[4]);
// 			pJL->wCacheTxWptr += 4;
// 		}
// 	}
// 	pJL->proxy_choice = 1;

// 	return TRUE;
// }

//--------------------------------------------------
//��������:    698��������ȡһ����������һ����¼�Ͷ�������
//         
//����:      BYTE Ch[in]		ͨ��ѡ��
// 
//	        pJL[in/out]			TJLTxBufָ��
//
//����ֵ:   ���ش�����
//         
//��ע����:  ��
//--------------------------------------------------
// BOOL ProxyGetRequestRecord( BYTE Ch ,TJLTxBuf *pJL)
// {
// 	verifyMacInfo_t verifyMacInfo = {0};
// 	TGetPara para = {0};
// 	TSA sa = {0};
// 	TMetCfg_Base Cfg={0};
// 	BYTE byMPNo = 0,TxBuf[120] = {0};
// 	BYTE *pData = NULL;
// 	WORD wTmpLen = 0;

// 	if((Ch >= MAX_COM_CHANNEL_NUM)
// 	||(pJL == NULL))
//     {
//         return FALSE;
//     }

// 	memcpy(pJL->Addr,LinkData[Ch].ServerAddr,6);
// 	pJL->PIID = LinkData[Ch].pAPP[2];
// 	pJL->ClientAddr = LinkData[Ch].ClientAddress;
//     //���������̶�Ϊ����+RN��ʽ��ȡ
//     verifyMacInfo.rand = s_ReadMeterMAC;
//     api_processMacRN(GENERATE_RN,&verifyMacInfo);
//     para.Rand = s_ReadMeterMAC;
//     para.bUseSafeMode = eSECURITY_RN;
    
// 	pData = (BYTE*)&LinkData[Ch].pAPP[3];
// 	pJL->wTimeOut = MW(pData[0],pData[1]);
// 	pData += 2;
// 	pData += gdw698buf_TSA(pData,&sa,TD12,TRUE);
// 	byMPNo = FindMeterMPNo(sa.addr);
// 	if (byMPNo == 0)
// 		pJL->byDar = dar_address_exception;
// 	else
// 	{
// 		sys_GetMPCfg(byMPNo,&Cfg);
// 		if (Cfg.byProtocolNo != 3) //��ʱֻ����698��Լ���
// 			pJL->byDar = dar_other_reason;
// 		pJL->byDestNo = (BYTE)(Cfg.byPortNo-1);
// 		if (pJL->byDestNo >= MAX_VPORT_NUM)
// 			pJL->byDar = dar_object_unavailable;
// 	}
	
// 	pJL->Dcb.baud = Cfg.byBaud;
// 	pJL->Dcb.databits = 8;
// 	pJL->Dcb.parity = 2;
// 	pJL->Dcb.stopbits = 1;

// 	para.byType = 3;
// 	para.wNum = 1;
// 	para.pTxBuff = TxBuf;
// 	para.wBufLen = 0;

//     //��¼OAD���յ�������Ӧʱʹ��
//     memcpy(pJL->proxy_oads,pData,4);
// 	wTmpLen = 4;
// 	wTmpLen	+= objGetDataLen(dt_RSD,&pData[wTmpLen],MAX_NUM_NEST);
// 	wTmpLen	+= objGetDataLen(dt_RCSD,&pData[wTmpLen],MAX_NUM_NEST);
// 	para.wBufLen = wTmpLen;

//     if(para.wBufLen>sizeof(TxBuf))
//     {
//         return FALSE;
//     }
// 	memcpy(para.pTxBuff,pData,para.wBufLen);
	
// 	memset(pJL->TransTxBuf,0xFE,4);
// 	pJL->wTxWptr = _TxFm698Read(&sa,&para,&pJL->TransTxBuf[4]);
// 	pJL->wTxWptr += 4;
    
	
// 	pJL->proxy_choice = 2;

// 	return TRUE;
// }

//--------------------------------------------------
//��������:    698�������͸��ת������
//         
//����:      BYTE Ch[in]		ͨ��ѡ��
// 
//	        pJL[in/out]			TJLTxBufָ��
//        
//����ֵ:    ���ش�����
//         
//��ע����:  ��
//--------------------------------------------------
// BOOL ProxyTransCommandRequest( BYTE Ch ,TJLTxBuf *pJL)
// {
// 	DWORD dwDestPort = 0;
// 	BYTE *pData = NULL;
	
// 	if((Ch >= MAX_COM_CHANNEL_NUM)
// 	||(pJL == NULL))
//     {
//         return FALSE;
//     }

// 	memcpy(pJL->Addr,LinkData[Ch].ServerAddr,6);
// 	pJL->PIID = LinkData[Ch].pAPP[2];
// 	pJL->ClientAddr = LinkData[Ch].ClientAddress;

// 	pData = (BYTE*)&LinkData[Ch].pAPP[3];
// 	dwDestPort = MDW(pData[0],pData[1],pData[2],pData[3]);
// 	if(dwDestPort == 0xF2010201)
// 		JLTxBuf.byDestNo = eRS485_I;
// 	else if(dwDestPort == 0xF2090201)
// 		JLTxBuf.byDestNo = eCR;
// 	else if(dwDestPort == 0xF20B0201)
// 		JLTxBuf.byDestNo = eBlueTooth;
// 	else if(dwDestPort == 0xF2210201)
// 		JLTxBuf.byDestNo = eCAN;
// 	else 
//         return FALSE;
// 	pData += 4;
// 	pJL->Dcb.baud = pData[0];
// 	pJL->Dcb.parity = pData[1];
// 	pJL->Dcb.databits = pData[2];
// 	pJL->Dcb.stopbits = pData[3];
// 	pJL->Dcb.flowctrl = pData[4];
// 	pData += 5;
// 	pJL->wTimeOut = MW(pData[0],pData[1]);
// 	pData += 4;
// 	pData += gdw698Buf_Num(pData,&pJL->wTxWptr,TD12);
    
// 	if(pJL->wTxWptr > (sizeof(pJL->TransTxBuf)-4))
//     {
//         pJL->wTxWptr = 0;
//         return FALSE;
//     }
// 	//��4��fe����֡
// 	memset(pJL->TransTxBuf,0xFE,4);
// 	memcpy(&pJL->TransTxBuf[4],pData,pJL->wTxWptr);
// 	pJL->wTxWptr += 4;
// 	pJL->proxy_choice = 7;

// 	return TRUE;
// }

//-----------------------------------------------
//��������:	����(����+MAC)����
//	
//����:		pFrameBuf[in/out]	����֡����ʼ�ַ��ĵ�ַ
//			rand[in]			���������+�����
//			frameLen[out]		�������֡����(��������ʼ�ַ��ͽ����ַ�)
//����ֵ:	TRUE FALSE
//	
//��ע:   
//-----------------------------------------------
// BOOL api_analysisClearTextMAC(BYTE *pFrameBuf,BYTE *rand,TTwoByteUnion *frameLen)
// {
// 	verifyMacInfo_t verifyMacInfo = {0};
// 	TTwoByteUnion apduLen = {0},macLen = {0};
// 	BYTE *pApdu = NULL;
// 	BYTE apduLenOffest = 0,macLenOffest = 0,addrLen = 0,frameHeadLen = 0;
// 	BYTE result = FALSE;

// 	if( (pFrameBuf == NULL) 
// 	|| (rand == NULL)
// 	|| (frameLen == NULL) )
// 	{
// 		return FALSE;
// 	}
// 	//6�ֽ�ͨ�ŵ�ַ
// 	addrLen = (pFrameBuf[4]&0x0F) + 1;
// 	if(addrLen != sizeof(verifyMacInfo.mailAddr))
// 	{
// 		return FALSE;
// 	}
//     //frameHeadLen������֡��ʼ�ַ�
// 	frameHeadLen = DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+addrLen+DLT_CSLEN;
// 	pApdu= &pFrameBuf[DLT_START_BYTE+frameHeadLen];
	
// 	if( pApdu[0] == SECURITY_RESPONSE )
// 	{
// 		if( pApdu[1] == 0x00 )//���ķ�ʽ
// 		{
// 			apduLenOffest = Deal_698DataLenth( &pApdu[2], apduLen.b, ePROTOCOL_TYPE);//��ȡ���ݳ���
// 			apduLen.w += apduLenOffest+2;
// 			if( pApdu[apduLen.w] != 0x00 )//OPTIONAL ѡ��
// 			{
// 				if( pApdu[apduLen.w+1] == 0x00 )//ѡ��MAC
// 				{
// 					apduLenOffest = Deal_698DataLenth( &pApdu[2], apduLen.b, ePROTOCOL_TYPE);//��ȡAPDU���Ⱥͱ�ʾAPDU���ȵ����ݳ���
// 					if(apduLen.w > MAX_APDU_SIZE)
// 					{
// 						return FALSE;
// 					}
// 					//��������
// 					memcpy( ProtocolBuf, &pApdu[2+apduLenOffest],apduLen.w );
					
// 					macLenOffest = Deal_698DataLenth( &pApdu[2+apduLenOffest+apduLen.w+2], macLen.b, ePROTOCOL_TYPE);//��ȡMAC���Ⱥͱ�ʾMAC���ȵ����ݳ���
// 					if(macLen.w > MAX_RN_SIZE) 
// 					{
// 						return FALSE;
// 					}
// 					//����MAC
// 					memcpy( ProtocolBuf+apduLen.w, &pApdu[2+apduLenOffest+apduLen.w+2+macLenOffest],macLen.w );

// 					memcpy(verifyMacInfo.mailAddr,&pFrameBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+1],addrLen);        //ͨ�ŵ�ַ
// 					verifyMacInfo.apduMacBufLen = apduLen.w+macLen.w;
// 					verifyMacInfo.apduMacBuf = ProtocolBuf;
// 					verifyMacInfo.rand = rand;

// #if SECURED_TRANS_RN_VERIFY_MAC_EN
//                     result = api_processMacRN(VERIFY_MAC,&verifyMacInfo);
// #else
//                     result = TRUE;
// #endif
// 					if(result == TRUE)
// 					{
//                         memcpy( pApdu, ProtocolBuf, apduLen.w);
// 						//�޸�֡���ȣ�֡ͷ��֡βУ��
//                         //frameHeadLen + apduLen.w������֡�ĵ�һ���ֽں����һ���ֽ��Լ�֡βУ��
// 						Add698FrameTail(pFrameBuf[3],frameHeadLen + apduLen.w,pFrameBuf);
//                         frameLen->b[0] = pFrameBuf[1];
//                         frameLen->b[1] = pFrameBuf[2]; 
// 						return TRUE;
// 					}
// 					else
// 					{
// 						return FALSE;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return FALSE;
// }
