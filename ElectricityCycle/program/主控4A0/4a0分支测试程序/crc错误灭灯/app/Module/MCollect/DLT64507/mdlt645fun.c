#include "appcfg.h"
#include "CommFunc_API.h"
#include "AllHead.h"
#include "MCollect.h"
#if(MD_MDLT645_07 || MD_MDLT645_97)
#include "mdlt645.h"
///////////////////////////////////////////////////////////////
//	�� �� �� : Tx_WriteFm
//	�������� : д������
//	������� : 
//	��    ע : Ŀǰֻ֧�ֹ㲥Уʱ
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��5��
//	�� �� ֵ : void
//	����˵�� : BOOL bDlt645_2007,
//				BYTE byCtrl,
//				BYTE *pAddr,
//				BYTE *pPara,
//				TTx *pTx
///////////////////////////////////////////////////////////////
// void Tx_WriteFm(BOOL bDlt645_2007,BYTE byCtrl,DWORD dwDI,BYTE *pData,BYTE byDataLen,BYTE *pAddr,TTx *pTx)
// {
// TMDlt645Fm *pTxFm=(TMDlt645Fm *)&pTx->pBuf[4];
// BYTE *pBuf=(BYTE *)(pTxFm+1);
// BYTE byI=0,byJ;

// 	pTx->wRptr		= pTx->wWptr = 0;
// 	memset(pTx->pBuf,0xFE,4);
// 	pTxFm->byS68	= 0x68;
// 	memcpy(pTxFm->Addr,pAddr,6);
// 	pTxFm->byE68	= 0x68;
// 	pTxFm->byCtrl	= byCtrl;
// 	if(byCtrl == CTRL_W_BROADCAST)
// 	{//Ŀǰֻ֧�ֹ㲥Уʱ
// 		pTxFm->byDataLen= byDataLen;
// 		for(byI=0;byI<byDataLen;byI++)
// 		{
// 			pBuf[byI]	= (BYTE)(pData[byI]+0x33);
// 		}
// 		pBuf[byDataLen] 	= CheckSum(&pTxFm->byS68,(byDataLen+10));	
// 		pBuf[byDataLen+1]	= 0x16;
// 		pTx->wWptr	= (byDataLen+10+6);	
// 	}
// 	else
// 	{
// 		pTxFm->byDataLen= (BYTE)(byDataLen+4);
// 		pBuf[byI++]	= (BYTE)(LLBYTE(dwDI)+0x33);
// 		pBuf[byI++]	= (BYTE)(LHBYTE(dwDI)+0x33);
// 		pBuf[byI++]	= (BYTE)(HLBYTE(dwDI)+0x33);
// 		pBuf[byI++]	= (BYTE)(HHBYTE(dwDI)+0x33);
// 		for(byJ=0;byJ<byDataLen;byJ++)
// 		{
// 			pBuf[byI++]	= (BYTE)(pData[byJ]+0x33);
// 		}
// 		pBuf[byI] = CheckSum(&pTxFm->byS68,(byI+10));	
// 		byI++;
// 		pBuf[byI++]	= 0x16;
// 		pTx->wWptr	= (byI+14);	
// 	}
// }

void _Tx_Fm645Read(BOOL bDlt645_2007,DWORD dwDI,BYTE *pData,BYTE byDataLen,BYTE *pAddr,TTx *pTx)
{
TMDlt645Fm *pTxFm=(TMDlt645Fm *)&pTx->pBuf[4];
BYTE *pBuf=(BYTE *)(pTxFm+1);
BYTE byI;

	pTx->wRptr		= pTx->wWptr = 0;
	if(pTx->wSize < (WORD)(0x14+byDataLen))
		return;
	memset(pTx->pBuf,0xFE,4);
	pTxFm->byS68	= 0x68;
	memcpy(pTxFm->Addr,pAddr,6);
	pTxFm->byE68	= 0x68;
	if(bDlt645_2007)
	{
		pTxFm->byCtrl	= 0x11;
		pTxFm->byDataLen= (BYTE)(0x04+byDataLen);
		pBuf[0]			= (BYTE)(LLBYTE(dwDI)+0x33);	 
		pBuf[1]			= (BYTE)(LHBYTE(dwDI)+0x33);
		pBuf[2]			= (BYTE)(HLBYTE(dwDI)+0x33);	 
		pBuf[3]			= (BYTE)(HHBYTE(dwDI)+0x33);
		if(byDataLen)
		{
			for(byI=0;byI<byDataLen;byI++)
				pBuf[4+byI]	= (BYTE)(pData[byI]+0x33);
		}	
		pBuf[4+byDataLen]= CheckSum(&pTxFm->byS68,14+byDataLen);	 
		pBuf[5+byDataLen]= 0x16;
		pTx->wWptr		= (WORD)(0x14+byDataLen);
	}
	else
	{
		pTxFm->byCtrl	= 0x01;
		pTxFm->byDataLen= 0x02;
		pBuf[0]			= (BYTE)(LLBYTE(dwDI)+0x33);	 
		pBuf[1]			= (BYTE)(LHBYTE(dwDI)+0x33);
		if(byDataLen)
		{
			for(byI=0;byI<byDataLen;byI++)
				pBuf[2+byI]	= (BYTE)(pData[byI]+0x33);
		}	
		pBuf[2+byDataLen]= CheckSum(&pTxFm->byS68,12+byDataLen);	 
		pBuf[3+byDataLen]= 0x16;
		pTx->wWptr		= (WORD)(0x12+byDataLen);
	}	
}

///////////////////////////////////////////////////////////////
//	�� �� �� : Tx_Fm645Read
//	�������� : ͨ�ö�645����
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��5��
//	�� �� ֵ : void
//	����˵�� : BOOL bDlt645_2007,
//				DWORD dwDI,
//				BYTE *pAddr,
//				TTx *pTx
///////////////////////////////////////////////////////////////
// void Tx_Fm645Read(BOOL bDlt645_2007,DWORD dwDI,BYTE *pAddr,TTx *pTx)
// {
// 	_Tx_Fm645Read(bDlt645_2007,dwDI,NULL,0,pAddr,pTx);
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : Search645Frame
//	�������� : ��׼645��Լ������Ч֡
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��5��
//	�� �� ֵ : DWORD
//	����˵�� : HPARA hBuf,
//				DWORD dwLen
//				BYTE *pAddr,
//				BOOL bDirect TRUE-���ͱ��ģ�FALSE-���ձ���
///////////////////////////////////////////////////////////////
DWORD Search645Frame(HPARA hBuf,DWORD dwLen,BYTE *pAddr,BOOL bDirect)
{
TMDlt645Fm *pRxFm=(TMDlt645Fm *)hBuf;
BYTE *pBuf=(BYTE *)(pRxFm+1);
DWORD dwFmLen,dwRc;
BYTE byI;

	if(dwLen<MIN_RX645FM_SIZE)
		return FM_LESS;
	dwRc = SearchHead2(hBuf,dwLen,0,0x68,7,0x68);
	if(dwRc)
		return FM_ERR | dwRc;
	//����֡�����Ƿ�ϸ�
	dwFmLen = pRxFm->byDataLen + LEN_645FM_HEAD + LEN_645FM_TAIL;
	if(dwFmLen > MAX_RX645FM_SIZE)
		return FM_ERR | 1;
	if(dwFmLen > dwLen)
		return FM_LESS;
	//�жϴ��ͷ���
	if(bDirect)
	{//����
		if(pRxFm->byCtrl&0x80)
			return FM_ERR | 1;
	}
	else if((pRxFm->byCtrl&0x80) == 0)
		return FM_ERR | 1;
	//��������ַ��Ƿ����
	if(pBuf[pRxFm->byDataLen+1] != 0x16 )
		return FM_ERR | 1;
	//����У���Ƿ�ϸ�
	if(pBuf[pRxFm->byDataLen] != CheckSum(hBuf,pRxFm->byDataLen + LEN_645FM_HEAD))
		return FM_ERR | 1;
	//�жϵ�ַ�Է�
	if(NULL != pAddr)
	{
		for(byI=0;byI<6;byI++)
		{
			if((pAddr[byI] != 0xAA)&&(pRxFm->Addr[byI] != pAddr[byI]))
				break;		
		}
		if(byI < 6)	
		{		
			return FM_ERR | dwFmLen;
		}
	}
	return FM_OK | dwFmLen;
}

///////////////////////////////////////////////////////////////
//	�� �� �� : MDlt645_RxMonitor
//	�������� : 645��Լ���մ���
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��9��6��
//	�� �� ֵ : BYTE
//	����˵�� : BOOL bIsDlt6452007,
//				BYTE *pBuf,
//				BYTE *pRead,
//				DWORD *pdwRxDI,
//				BYTE *pbyErr
///////////////////////////////////////////////////////////////
BYTE MDlt645_RxMonitor(BOOL bIsDlt6452007,BYTE *pBuf,DWORD *pdwRxDI,BYTE *pbyErr)
{
TMDlt645Fm *pRxFm=(TMDlt645Fm *)pBuf;
BYTE *pRxData=(BYTE *)&pRxFm[1];
BYTE *pRead;
BYTE byI,byLen;
	
	if((!bIsDlt6452007)&&((pRxFm->byCtrl&0x0f)!=1))
	{
		*pbyErr = 1;
		return 0;
	}
	//�������������
	if(pRxFm->byCtrl & NODATA)
	{
		*pbyErr	= 1;
		return 0;
	}
	*pbyErr	= 0;
    
	if(pRxFm->byDataLen)
	{//�ָ���������		
		if(bIsDlt6452007)
		{
			*pdwRxDI	= MDW((BYTE)(pRxData[3]-0x33),(BYTE)(pRxData[2]-0x33),(BYTE)(pRxData[1]-0x33),(BYTE)(pRxData[0]-0x33));
			pRead	= &pRxData[4];
			byLen	= (BYTE)(pRxFm->byDataLen-4);

            #if (SEL_SEARCH_METER == YES )
            if ( (SKMeter.byAutoCheck & BIT1) && (SKMeter.byGyType == ePROTOCOL_645_07) )
            {
                if( (GetPortNo() == SEARCH_METER_CH) &&
                    ( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) ) 
                )
                {
                    if( (pRxFm->byCtrl == 0x91) && (pRxFm->byDataLen == SEARCH_METER_654_07_DATA_LEN ) && (*pdwRxDI == SEARCH_METER_654_07_OAD) )
                    {
                        api_Addr2MeterNo(pRxFm->Addr, ePROTOCOL_645_07, SKMeter.byBaud, SEARCH_METER_CH_OAD,TRUE);
                        return 0;	
                    }
                }
            }
            #endif
		}
		else
		{
			*pdwRxDI= MW((BYTE)(pRxData[1]-0x33),(BYTE)(pRxData[0]-0x33));
			pRead	= &pRxData[2];
			byLen	= (BYTE)(pRxFm->byDataLen-2);
		}

		if(byLen >= MAX_RX645FM_SIZE)
		{
			*pbyErr	= 2;
			return 0;
		}
		for(byI=0;byI<byLen;byI++)
		{
			pRead[byI]	-= 0x33;
		}
	}
	else 
    {
        byLen	= 0;//���óɹ�Ӧ�� ��ʱ����բ����.....
    }
	return byLen;
}

///////////////////////////////////////////////////////////////
//	�� �� �� : Sunitem
//	�������� : ������߶����
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��23��
//	�� �� ֵ : BOOL
//	����˵�� :	DWORD dwDI,		���ݱ�ʶ	
//				WORD wFlag,		�����־
//				BYTE byMaxNum,	�������
//				BYTE byPlace,	�������ֽ���
///////////////////////////////////////////////////////////////
BOOL Sunitem(BOOL bIsDlt6452007,DWORD dwDI,DWORD dwFlagNo,BYTE byMaxNum,BYTE byPlace,BYTE byStarNum,DWORD *pdwDI)
{
BYTE byI;
	
	if((byPlace>4)||((!bIsDlt6452007)&&(byStarNum+byMaxNum-1>0x0F)))
		return FALSE;
	for(byI=0;byI<byMaxNum;byI++)
	{ 
		if(CheckClearFlag(MSF_CENT_0+byI))
		{
			if((byI+1) != byMaxNum)
				SetFlag(dwFlagNo);
			break;
		}
	}
	if(byI >= byMaxNum)
	{
		*pdwDI = dwDI;
	}
	else
	{
		if(bIsDlt6452007)
		{
			dwDI	&= ~(0xFF<<byPlace*8); 
			*pdwDI  = (dwDI|((byI+byStarNum)<<byPlace*8));
		}
		else
		{
			dwDI	&= ~(0x0F<<byPlace*4); 
			*pdwDI  = (dwDI|((byI+byStarNum)<<byPlace*4));
		}
	}
	return TRUE;
}

// BYTE MaterEvent(DWORD dwFlagNo,BYTE byMaxNum)
// {
// BYTE byI;
	
// 	for(byI=0;byI<byMaxNum;byI++)
// 	{ 
// 		if(CheckClearFlag(MSF_CENT_0+byI))
// 		{
// 			if((byI+1) != byMaxNum)
// 				SetFlag(dwFlagNo);
// 			break;
// 		}
// 	}
// 	return byI;
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : SunitemFlag
//	�������� : ����/�����־
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��23��
//	�� �� ֵ : void
//	����˵�� : void
///////////////////////////////////////////////////////////////
void SunitemFlag(DWORD dwFlagNo,BOOL bIsSetFlag)
{
	if(bIsSetFlag)
		SetFlag(dwFlagNo);
	else 
		ClearFlag(dwFlagNo);
}

///////////////////////////////////////////////////////////////
//	�� �� �� : SetSunitemFlag
//	�������� : ���ñ�־���������ݿ鲻����ʱ���ɼ�����
//	������� : byFlagNum�������������������������������ĸ���
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��23��
//	�� �� ֵ : void
//	����˵�� : void
///////////////////////////////////////////////////////////////
void SetSunitemFlag(BOOL bIs07,DWORD dwDI,BOOL bIsTx)
{
BYTE byI,byFlagNum = 0;
BOOL bSetFlag = bIsTx;
BOOL bNoDI = FALSE;

	if(bIs07)
	{//07
		switch(dwDI) 
		{//���ݿ鷢��ʱ���÷����־�����ݿ��н���ʱ����ֱ���
		case 0x0001ff00: SunitemFlag(MSF_AP,	bSetFlag);byFlagNum = 1;break;//����
		case 0x0002ff00: SunitemFlag(MSF_AR,	bSetFlag);byFlagNum = 1;break;//����
		case 0x0003ff00: SunitemFlag(MSF_RP,	bSetFlag);byFlagNum = 1;break;//����
		case 0x0004ff00: SunitemFlag(MSF_RR,	bSetFlag);byFlagNum = 1;break;//����
		case 0x0201ff00: SunitemFlag(MSF_VT,	bSetFlag);byFlagNum = 3;break;//��ѹ
		case 0x0202ff00: SunitemFlag(MSF_CT,	bSetFlag);byFlagNum = 3;break;//����
		case 0x0203ff00: SunitemFlag(MSF_PA,	bSetFlag);byFlagNum = 4;break;//�й�����
		case 0x0204ff00: SunitemFlag(MSF_PR,	bSetFlag);byFlagNum = 4;break;//�޹�����
		case 0x0205ff00: SunitemFlag(MSF_PAP,	bSetFlag);byFlagNum = 4;break;//���ڹ���
		case 0x0206ff00: SunitemFlag(MSF_PE,	bSetFlag);byFlagNum = 4;break;//��������
		case 0x10000001: SunitemFlag(MSF_SYINFALL,bSetFlag);byFlagNum = 2;break;//ȫʧѹ��������ʱ�ɼ�����֮ǰ�ı�ʶ��		
		default:bNoDI=TRUE;	break;
		}
		if(bIsTx)
		{//����ʱ����
			switch(dwDI) 
			{
			case 0x00150000: SunitemFlag(MSF_A_ARP,		bSetFlag);byFlagNum = 3;break;//ʵʱA�������
			case 0x00290000: SunitemFlag(MSF_B_ARP,		bSetFlag);byFlagNum = 3;break;
			case 0x003D0000: SunitemFlag(MSF_C_ARP,		bSetFlag);byFlagNum = 3;break;
			case 0x04000B01: SunitemFlag(MSF_AUTODAY,	bSetFlag);byFlagNum = 2;break;//�Զ�������
			case 0x00150001: SunitemFlag(MSF_MON_A,		bSetFlag);byFlagNum = 3;break;//ʵʱA�������
			case 0x00290001: SunitemFlag(MSF_MON_B,		bSetFlag);byFlagNum = 3;break;
			case 0x003D0001: SunitemFlag(MSF_MON_C,		bSetFlag);byFlagNum = 3;break;			
			}			
		}
		else if(bNoDI)
		{//���¼�����Ϊ���յ�����ʱ���ñ�־
			bSetFlag = TRUE;
			switch(dwDI)
			{
			case 0x13010001: SunitemFlag(MSF_DXINFOA,	bSetFlag);byFlagNum = 2;ClearFlag(MSF_DXALL);break;//A�����
			case 0x13020001: SunitemFlag(MSF_DXINFOB,	bSetFlag);byFlagNum = 2;break;//B�����
			case 0x13030001: SunitemFlag(MSF_DXINFOC,	bSetFlag);byFlagNum = 2;break;//C�����
			case 0x03040000: SunitemFlag(MSF_DXALL,		bSetFlag);byFlagNum = 3;break;//����2�α���ǰ
			case 0x10010001: SunitemFlag(MSF_DXINFOA,	bSetFlag);byFlagNum = 2;break;//A��ʧѹ
			case 0x10020001: SunitemFlag(MSF_DXINFOB,	bSetFlag);byFlagNum = 2;break;//B��ʧѹ
			case 0x10030001: SunitemFlag(MSF_DXINFOC,	bSetFlag);byFlagNum = 2;break;//C��ʧѹ
			case 0x03300100: SunitemFlag(MSF_CLR_NUM,	bSetFlag);byFlagNum = 1;break;//��������ܴ���
			case 0x03300200: SunitemFlag(MSF_XLCLRNUM,	bSetFlag);byFlagNum = 1;break;//����������������
			case 0x03300300: SunitemFlag(MSF_CLRETNUM,	bSetFlag);byFlagNum = 1;break;//����¼��������
			case 0x03300400: SunitemFlag(MSF_CORTTMNUM,	bSetFlag);byFlagNum = 1;break;//Уʱ����
			case 0x04000409: SunitemFlag(MSF_CONSTANT,	bSetFlag);byFlagNum = 1;break;//�й������޹�����
			case 0x03300000: SunitemFlag(MSF_PROGNUM,	bSetFlag);byFlagNum = 1;break;//��̴���
			case 0x03300D00: SunitemFlag(MSF_OPEN_WG,	bSetFlag);byFlagNum = 1;break;//���Ǵ���
			case 0x03300E00: SunitemFlag(MSF_OPEN_DNG,	bSetFlag);byFlagNum = 1;break;//����ť�Ǵ���
			case 0x03300500: SunitemFlag(RSF_TMPRONUM,	bSetFlag);byFlagNum = 1;break;//ʱ�α���ܴ���
			case 0x03350000: SunitemFlag(MSF_LASTMFREC,	bSetFlag);byFlagNum = 1;break;//�ų����Ŵ���
			case 0x03110000: SunitemFlag(MSF_LOSTPW_NUM,bSetFlag);byFlagNum = 10;break;//�������//10
			case 0x04000306: SunitemFlag(MSF_HGQBB_CPT,	bSetFlag);byFlagNum = 1;break;//CTPT
			case 0x1D000101: SunitemFlag(RSF_TRIPTM,	bSetFlag);byFlagNum = 1;break;//���һ��Զ�̿�����/��բʱ��
			}
		}	
	}
	else
	{//97
		switch(dwDI) 
		{//����ʱ���ã�����ʱȡ��
		case 0x901f: SunitemFlag(MSF_AP,			bSetFlag);byFlagNum =1;break;
		case 0x902f: SunitemFlag(MSF_AR,			bSetFlag);byFlagNum =1;break;
		case 0x911f: SunitemFlag(MSF_RP,			bSetFlag);byFlagNum =1;break;
		case 0x912f: SunitemFlag(MSF_RR,			bSetFlag);byFlagNum =1;break;
		default: bNoDI = TRUE;
		}
		if(bIsTx)
		{//����ʱ����
			switch(dwDI)
			{
			case 0xb611: SunitemFlag(MSF_VT,		bSetFlag);byFlagNum =2;break;
			case 0xb621: SunitemFlag(MSF_CT,		bSetFlag);byFlagNum =2;break;
			case 0xb650: SunitemFlag(MSF_PE,		bSetFlag);byFlagNum =3;break;
			case 0xb630: SunitemFlag(MSF_PA,		bSetFlag);byFlagNum =3;break;
			case 0xb640: SunitemFlag(MSF_PR,		bSetFlag);byFlagNum =3;break;
			case 0xb660: SunitemFlag(MSF_PAP,		bSetFlag);byFlagNum =3;break;
			case 0xb69f: SunitemFlag(MSF_VIANGLE,	bSetFlag);byFlagNum =1;break;
			case 0xa01f: SunitemFlag(MSF_APXL,		bSetFlag);byFlagNum =1;break;
			case 0xa02f: SunitemFlag(MSF_ARXL,		bSetFlag);byFlagNum =1;break;
			case 0xa11f: SunitemFlag(MSF_RPXL,		bSetFlag);byFlagNum =1;break;
			case 0xa12f: SunitemFlag(MSF_RRXL,		bSetFlag);byFlagNum =1;break;
			case 0xa41f: SunitemFlag(MSF_MONXLAP,	bSetFlag);byFlagNum =1;break;
			case 0xa42f: SunitemFlag(MSF_MONXLAR,	bSetFlag);byFlagNum =1;break;
			case 0xa51f: SunitemFlag(MSF_MONXLRP,	bSetFlag);byFlagNum =1;break;
			case 0xa52f: SunitemFlag(MSF_MONXLRR,	bSetFlag);byFlagNum =1;break;
			}
		}
		else if(bNoDI)
		{//����ʱ����
			bSetFlag = TRUE;
			switch(dwDI)
			{			
			case 0xb310: SunitemFlag(MSF_DXALL,		bSetFlag);byFlagNum =3;break;
			case 0xb311: SunitemFlag(MSF_DXINFOA,	bSetFlag);byFlagNum =3;break;
			case 0xb312: SunitemFlag(MSF_DXINFOB,	bSetFlag);byFlagNum =3;break;
			case 0xb313: SunitemFlag(MSF_DXINFOC,	bSetFlag);byFlagNum =3;break;
			case 0xB213: SunitemFlag(MSF_XLCLRNUM,	bSetFlag);byFlagNum =1;break;
			case 0xc030: SunitemFlag(MSF_CONSTANT,	bSetFlag);byFlagNum =1;break;
			case 0xC331: SunitemFlag(RSF_TIMESEG,	bSetFlag);byFlagNum =7;break;
			case 0xB212: SunitemFlag(MSF_PROGNUM,	bSetFlag);byFlagNum =1;break;
			}
		}
	}
	for(byI=0;byI<byFlagNum;byI++)
	{
		SunitemFlag(MSF_CENT_0+byI,bSetFlag);
	}
}

//////////////////////////////////////////////////////////////
/*               645_2007��645_1997ͨ�ò���                 */
//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//	�� �� �� : Rx_Date
//	�������� : ���� 
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��19��
//	�� �� ֵ : void
//	����˵�� :
///////////////////////////////////////////////////////////////
void Rx_Date(BYTE *pBuf)
{
BYTE Buf[3],byI;	

	for(byI=0;byI<3;byI++)
	{
		Buf[byI]	= Bcd2Bin(pBuf[byI+1]);
	}
	WriteTime(0,Buf);
}

///////////////////////////////////////////////////////////////
//	�� �� �� : Rx_Time
//	�������� : ʱ�� 
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��19��
//	�� �� ֵ : void
//	����˵�� : 
///////////////////////////////////////////////////////////////
void Rx_Time(BYTE *pBuf)
{
BYTE Buf[3],byI;	

	for(byI=0;byI<3;byI++)
	{
		Buf[byI]	= Bcd2Bin(pBuf[byI]);
	}
	WriteTime(1,Buf);
}

///////////////////////////////////////////////////////////////
//	�� �� �� : Rx_DD
//	�������� : ˲ʱ������ʾֵ 
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��19��
//	�� �� ֵ : void
//	����˵�� : WORD wNo
///////////////////////////////////////////////////////////////
void Rx_DD(BYTE *pBuf,BYTE byLen,WORD wNo)
{
BYTE byI;
DWORD dwDD;
#if(MD_BASEUSER	== MD_GDW698)
long CI;
#endif
	
	for(byI=0;(byI<(byLen/4))&&(byI<5);byI++)
	{
		//��BCDת��Ϊ������
		dwDD = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);

		if ((wNo==10) || (wNo==15) || (wNo==42) || (wNo==43) || (wNo==46) || (wNo==47) || (wNo==50) || (wNo==51) || (wNo==52))
		{
			if (pBuf[3]&BIT7)
				dwDD = MDW((BYTE)(pBuf[3]&0x7F),pBuf[2],pBuf[1],pBuf[0]);
		}

		//�ж��Ƿ��Ǻϸ��BCD�룬��Ч��д���
		if(IsBCD(dwDD))
		{
			dwDD = DWBcd2Bin(dwDD);
#if(MD_BASEUSER	== MD_GDW698)
			if ((wNo==10) || (wNo==15) || (wNo==42) || (wNo==43) || (wNo==46) || (wNo==47) || (wNo==50) || (wNo==51) || (wNo==52))//����й�������޹�1 ����޹�2���λΪ����λ
			{
				if (pBuf[3]&BIT7)
				{
					dwDD = (DWORD)(0-dwDD);
					CI = (long)dwDD;
					CI *= 10;
					dwDD = (DWORD)CI;
				}
				else
					dwDD *= 10;
			}
			else
				dwDD *= 10;
#else
			dwDD *= 10;
#endif

			//������
			WriteDD((DWORD)(byI+wNo),dwDD);
		}		
		//ָ����һ���ݿ�
		pBuf += 4;
	}

}

/**************************************************************
 *	�� �� �� : Rx_DD2Frz
 *	�������� : ���յ�ʵʱ���ݺ����ת���ն���    
 *	������� : 
 *	��    ע : 
 *	��    �� : lsg
 *	ʱ    �� : 2018��11��20�� 
 *	�� �� ֵ : void
 *	����˵�� : BYTE *pBuf,BYTE byLen,WORD wNo
**************************************************************/
void Rx_DD2Frz(BYTE *pBuf,BYTE byLen,WORD wNo)
{
	BYTE byI;
	DWORD dwDD;
	
	for(byI=0;(byI<(byLen/4))&&(byI<5);byI++)
	{
		//��BCDת��Ϊ������
		dwDD = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
		//�ж��Ƿ��Ǻϸ��BCD�룬��Ч��д���
		if(IsBCD(dwDD))
		{
			dwDD = DWBcd2Bin(dwDD)*10;
			//������
			WriteDayDD(byI+wNo,dwDD);
		}		
		//ָ����һ���ݿ�
		pBuf += 4;
	}
}

/**************************************************************
 *	�� �� �� : Rx_DD_Ex
 *	�������� : �������з���ʵʱ�������������������ж���δ洢    
 *	������� : 
 *	��    ע : 
 *	��    �� : lsg
 *	ʱ    �� : 2018��11��20�� 
 *	�� �� ֵ : void
 *	����˵�� : BYTE *pBuf,BYTE byLen,WORD wNo
**************************************************************/
void Rx_DD_Ex(BYTE *pBuf,BYTE byLen,WORD wNo)
{
	TPort *pPort=(TPort *)GethPort();
	if(pPort->GyRunInfo.byReqType==REQ_DFRZ)
		Rx_DD2Frz(pBuf,byLen,wNo);
	else
		Rx_DD(pBuf,byLen,wNo);
}

///////////////////////////////////////////////////////////////
//	�� �� �� : Rx_AutoDay
//	�������� : ������ 
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��19��
//	�� �� ֵ : void
//	����˵�� : BYTE byNo
///////////////////////////////////////////////////////////////
void Rx_AutoDay(BYTE *pBuf,BYTE byLen,BYTE byNo)
{
UData Data;
	
	//��BCDת��Ϊ������
	Data.dwVal = MW(Bcd2Bin(pBuf[1]),Bcd2Bin(pBuf[0]));
//	WriteMeterRunInfo(DT_F1,byNo,Data);
}

///////////////////////////////////////////////////////////////
//	�� �� �� : Rx_MonDD
//	�������� : �µ����� 
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��19��
//	�� �� ֵ : void
//	����˵�� : BYTE byStartNo
///////////////////////////////////////////////////////////////
void Rx_MonDD(BYTE *pBuf,BYTE byLen,BYTE byStartNo)
{
BYTE byNo;
DWORD dwDD;
#if(MD_BASEUSER	== MD_GDW698)
long CI;
#endif

	for(byNo=0;(byNo<(byLen/4))&&(byNo<5);byNo++)
	{
		//��BCDת��Ϊ������
		dwDD = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
		if ((byStartNo==10) || (byStartNo==15))
		{
			if (pBuf[3]&BIT7)
				dwDD = MDW((BYTE)(pBuf[3]&0x7F),pBuf[2],pBuf[1],pBuf[0]);
		}
		if(IsBCD(dwDD))
		{
			dwDD = DWBcd2Bin(dwDD);
#if(MD_BASEUSER	== MD_GDW698)
			if ((byStartNo==10) || (byStartNo==15))//����޹�1 ����޹�2���λΪ����λ
			{
				if (pBuf[3]&BIT7)
				{
					dwDD = (DWORD)(0-dwDD);
					CI = (long)dwDD;
					CI *= 10;
					dwDD = (DWORD)CI;
				}
				else
					dwDD *= 10;
			}
			else
				dwDD *= 10;
#else
			dwDD *= 10;
#endif
			//������
			WriteMonDD((BYTE)(byStartNo+byNo),dwDD);
		}
		//ָ����һ���ݿ�
		pBuf += 4;
	}
}
///////////////////////////////////////////////////////////////
//	�� �� �� : Rx_BatTime
//	�������� : ��ع���ʱ�� 
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��19��
//	�� �� ֵ : void
//	����˵�� : BYTE byPhase
///////////////////////////////////////////////////////////////
// void Rx_BatTime(BYTE *pBuf,BYTE byLen)
// {
// UData Data;
		
// 	if(byLen == 3)
// 		Data.dwVal	= MDW(0,pBuf[2],pBuf[1],pBuf[0]);
// 	else
// 		Data.dwVal	= MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
// 	if(IsBCD(Data.dwVal))
// 	{
// 		Data.dwVal	= DWBcd2Bin(Data.dwVal);
// //		WriteMeterRunInfo(DT_F7,0,Data);
// 	}	
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : Rx_Constant
//	�������� : �й����޹����� 
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��19��
//	�� �� ֵ : void
//	����˵�� : BYTE byPhase
///////////////////////////////////////////////////////////////
// void Rx_Constant(BYTE *pBuf,BYTE byLen,BYTE byNo)
// {
// BYTE byI;
// UData Data;
	
// 	for(byI=0;((byI<2)&&(byI<(byLen/3)));byI++)
// 	{
// 		//��BCDת��Ϊ������
// 		Data.dwVal = MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 		if(IsBCD(Data.dwVal))
// 		{			
// 			Data.dwVal = DWBcd2Bin(Data.dwVal);
// //			WriteMeterRunInfo(DT_F14,byNo,Data);
// 		}
// 		byNo++;
// 		pBuf	+= 3;
// 	}
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : Rx_TimeSeg
//	�������� : ��һ��ʱ�������� 
//	������� : 
//	��    ע : 
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��19��
//	�� �� ֵ : void
//	����˵�� : BYTE byNo
///////////////////////////////////////////////////////////////
// void Rx_TimeSeg(BYTE *pBuf,BYTE byLen,BYTE byNo)
// {
// BYTE byI;
// UData Data;

// 	for(byI=0;((byI<8) && (byI<(byLen/3)));byI++)
// 	{
// 		Data.dwVal = MDW(0,pBuf[2],pBuf[1],pBuf[0]);
// 		if(IsBCD(Data.dwVal))
// 		{
// 			Data.dwVal = DWBcd2Bin(Data.dwVal);
// //			WriteMeterRunInfo(DT_F15,byNo,Data);
// 		}
// 		pBuf	+= 3;
// 		byNo++;
// 	}
// }

#endif
