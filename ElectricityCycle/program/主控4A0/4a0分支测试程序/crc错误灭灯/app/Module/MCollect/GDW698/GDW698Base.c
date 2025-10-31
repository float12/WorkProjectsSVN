
#include "GDW698.h"
#if(MD_BASEUSER	== MD_GDW698)
FRAME_RESULT objSmockRead( TFrame698_FULL_BASE *pAfnBase, BYTE *Buf )
{
BYTE byLen = 0;

	pAfnBase->pFrame = (OBJ_EXCEPT_TAIL*)Buf; 
	pAfnBase->pTail  = (OBJ_TAIL*) (Buf + ((pAfnBase->pFrame->wFmLen))+sizeof(BYTE)-2 ); 
	if(pAfnBase->pFrame->byStart != OBJ_START_FLAG )
		return FR_NOT_START_FLAG;		
	byLen = (BYTE)( (pAfnBase->pFrame->byAddrF & ADDR_BITS)+1 );
//..	if((pAfnBase->pFrame->byCtrl&BIT5) == 0 ) //不分帧
		pAfnBase->pDataAPDU = (BYTE *)( &pAfnBase->pFrame->_byAddrStart+(byLen+3) );
//..	else //分帧
//..		pAfnBase->pDataAPDU = (BYTE *)( &pAfnBase->pFrame->_byAddrStart+(byLen+3) +2 );
	pAfnBase->pObjTsa = (TSA *)(&pAfnBase->pFrame->byAddrF);
	return FR_OK;
}

WORD objGetAPDUSize(TFrame698_FULL_BASE *pAfnBase)
{//获得APDU大小
	return (WORD)((DWORD)(pAfnBase->pTail)-(DWORD)(pAfnBase->pDataAPDU));
}

// WORD GetClientAPDUDatalen(BYTE *pDataAPDU,WORD wAPDUSize)
// {//获得客户应用协议单元数据长度
// WORD wRc=0;
// BYTE choice;

// 	choice	= pDataAPDU[wRc++];
// 	switch(choice)
// 	{
// //	case CONNECT_Request: wRc	+= get_Apdu_CONNECT_request_len(&pDataAPDU[wRc]);break;	//建立应用连接请求
// //	case RELEASE_Request: wRc	+= get_Apdu_RELEASE_request_len(&pDataAPDU[wRc]);break;  //断开应用连接请求
// 	case GET_Request	: wRc	+= get_Apdu_GET_request_len(&pDataAPDU[wRc]);	break;  //读取请求
// //	case SET_Request	: wRc	+= get_Apdu_Set_request_len(&pDataAPDU[wRc]);	break;  //设置请求
// 	case ACTION_Request	: wRc	+= get_Apdu_ACTION_request_len(&pDataAPDU[wRc]);	break;  //操作请求
// //	case REPORT_Response: wRc	+= get_Apdu_REPORT_response_len(&pDataAPDU[wRc]);	break;  //上报应答
// //	case PROXY_Request	: wRc	+= get_Apdu_PROXY_request_len(&pDataAPDU[wRc]);	break;  //代理请求
// 	}
// 	return wRc;	
// }

// BOOL TpIsOk(TimeTag *pTimeTag)
// {//时间标签是否有效
// long nTimeDiff = 0;
// DWORD dwIntelTime=0;
// TTime tTime,Time;
	
// 	DTBinToShortTime(&pTimeTag->tmSend,&tTime,TD12);
// 	if(TimeIsValid(&tTime))
// 	{
// 		GetTime(&Time);
// 		nTimeDiff = abs(Time2Sec(&Time)-Time2Sec(&tTime));
// 		switch(pTimeTag->tmDelay.unit)
// 		{
// 		case 0: dwIntelTime = pTimeTag->tmDelay.value;			break;	//秒
// 		case 1: dwIntelTime = pTimeTag->tmDelay.value*60;		break;	//分
// 		case 2: dwIntelTime = pTimeTag->tmDelay.value*60*60;	break; //时
// 		case 3: dwIntelTime = pTimeTag->tmDelay.value*1440*60;	break;	//日
// 		case 4: dwIntelTime	= pTimeTag->tmDelay.value*1440*30*60;break;
// 		case 5: dwIntelTime	= pTimeTag->tmDelay.value*365*1440*60;break;
// 		}		
// 		if( ((DWORD)nTimeDiff <= dwIntelTime) 
// 			||(dwIntelTime == 0))
// 			return TRUE;		
// 	}
// 	return FALSE;
// }

WORD objGetFrameLength( TFrame698_FULL_BASE *pAfnBase )
{//获取帧长
	CHECKPTR( pAfnBase, 0 );
	return ( (pAfnBase->pFrame->wFmLen) );
}

WORD objGetFrameSize( TFrame698_FULL_BASE *pAfnBase )
{//获取整帧总的字节数
	CHECKPTR( pAfnBase, 0 );
	return objGetFrameLength( pAfnBase ) + sizeof(BYTE) + sizeof(BYTE);//起始和结束字符
}

DWORD gdw698_SearchOneFrame(BYTE byCtrl_Dir,WORD wMaxFmSize,HPARA hBuf,DWORD dwLen)
{//搜索有效帧
TFrame698_FULL_BASE afnBase;
DWORD dwRc,dwFrameSize; //帧总字节数
WORD wCheck,wAddLen;	//校验和
BYTE *pAddCS;

	if(dwLen < MIN_RXFM_SIZE)
		return FM_LESS;
	//1、检验起始标识
	dwRc	= SearchHead1(hBuf,dwLen,0,OBJ_START_FLAG);
	if(dwRc)
		return FM_ERR | dwRc;
	//2、检验长度
	objSmockRead( &afnBase, (BYTE*)hBuf );
	wAddLen = (WORD)( (afnBase.pFrame->byAddrF & ADDR_BITS)+1 );
	if((DWORD)(wAddLen+8) > dwLen)
		return FM_LESS;
	wCheck = fcs16( (BYTE*)(&afnBase.pFrame->wFmLen), (WORD)(wAddLen+5) ) ;
	pAddCS = ( &afnBase.pFrame->_byAddrStart+(wAddLen+1) );
	if(wCheck != MW(pAddCS[1],pAddCS[0]) )
		return FM_ERR | 1;
	dwFrameSize = objGetFrameSize( &afnBase );
	if( dwFrameSize >= wMaxFmSize)
		return FM_ERR | 1;
	if (dwFrameSize > dwLen)
		return FM_LESS;
	//3、判断尾
	if (afnBase.pTail->byEnd != OBJ_END_FLAG) 
		return FM_ERR | 1;
	if( dwFrameSize < wAddLen+sizeof(OBJ_EXCEPT_TAIL)+sizeof(OBJ_TAIL) )
	{//最小应用报文长度
		return FM_ERR | 1;
	}
	wCheck = fcs16((BYTE*)(&afnBase.pFrame->wFmLen), (WORD)(objGetFrameLength(&afnBase)-2) );	//判断校验和
	if( wCheck != afnBase.pTail->wCheckCS)
		return FM_ERR | 1;
	if(afnBase.pTail->byEnd	!= 0x16)
		return FM_ERR | dwFrameSize;
	//判断传送方向错误
	if ((afnBase.pFrame->byCtrl & CTRL_DIR) != byCtrl_Dir)	
		return FM_ERR | dwFrameSize;
	return FM_OK | dwFrameSize;
}

DWORD gdw698_SearchOneFrame2(BYTE byCtrl_Dir,WORD wMaxFmSize,HPARA hBuf,DWORD dwLen)
{
BYTE *pBuf=(BYTE*)hBuf;
DWORD dwRptr=0,dwRc;	

	while(1)
	{
		dwRc	= gdw698_SearchOneFrame(byCtrl_Dir,wMaxFmSize,&pBuf[dwRptr],dwLen-dwRptr);
		if((dwRc & FM_SHIELD) == FM_LESS)
		{
			return dwRc;
		}
		else if((dwRc & FM_SHIELD) == FM_OK)
		{
			return (dwRc&FM_SHIELD) +dwRptr;
		}
		dwRptr	+= LOWORD(dwRc);
	}
}

WORD MakeFrameHead(TSA *sa,BYTE *pBuf,BYTE byClientAddr)
{
TTx *pTx;
BYTE *pHead;
WORD wLen=0;

	if(NULL == pBuf)
	{
		pTx	= GetTx();
		pHead	= pTx->pBuf;
	}
	else
		pHead	= pBuf;
	pHead[wLen++]= OBJ_START_FLAG;
	wLen	+= 2;//跳过长度域
	wLen	++;	//控制域
	//地址域
	wLen	+= gdw698bufLink_TSA(&pHead[wLen],sa,TD21);
	pHead[wLen++]=byClientAddr;//get_client_addr();	//客户地址区
	//跳过帧头校验
	wLen	+= 2;
	return wLen;
}

void MakeFrameTail(BYTE byCtrl,WORD wFrameLen)
{
TTx *pTx=GetTx();
BYTE *pBuf=pTx->pBuf;
WORD wLen=0,wTmp;

	pTx->wRptr	= pTx->wWptr	= 0;
	pBuf[1]	= LOBYTE(wFrameLen+2);	
	pBuf[2]	= HIBYTE(wFrameLen+2);
	pBuf[3]	= byCtrl;
	//包头校验
	wLen	= (pBuf[4]&ADDR_BITS)+1+2;
	//固定头4字节
	wLen	+= 4;
	wTmp	= fcs16(&pBuf[1], (WORD)(((pBuf[4]&ADDR_BITS)+1)+5));	
	pBuf[wLen++]	= LOBYTE(wTmp);
	pBuf[wLen++]	= HIBYTE(wTmp);	
	//整包校验
	wTmp	= fcs16(&pBuf[1],wFrameLen);
	//加入68长度1字节
	wFrameLen++;
	pBuf[wFrameLen++]	= LOBYTE(wTmp);
	pBuf[wFrameLen++]	= HIBYTE(wTmp);	
	pBuf[wFrameLen++]	= 0x16;	
	pTx->wWptr		= wFrameLen;
}

WORD Add698FrameTail(BYTE byCtrl,WORD wFrameLen,BYTE *pBuf)
{
WORD wLen=0,wTmp;

	pBuf[1]	= LOBYTE(wFrameLen+2);	
	pBuf[2]	= HIBYTE(wFrameLen+2);
	pBuf[3]	= byCtrl;
	//包头校验
	wLen	= (pBuf[4]&ADDR_BITS)+1+2;
	//固定头4字节
	wLen	+= 4;
	wTmp	= fcs16(&pBuf[1], (WORD)(((pBuf[4]&ADDR_BITS)+1)+5));	
	pBuf[wLen++]	= LOBYTE(wTmp);
	pBuf[wLen++]	= HIBYTE(wTmp);	
	//整包校验
	wTmp	= fcs16(&pBuf[1],wFrameLen);
	//加入68长度1字节
	wFrameLen++;
	pBuf[wFrameLen++]	= LOBYTE(wTmp);
	pBuf[wFrameLen++]	= HIBYTE(wTmp);	
	pBuf[wFrameLen++]	= 0x16;	
	return wFrameLen;
}

// void MakeFrame(BYTE byCtrl,TSA *sa,BYTE *pAPDU,WORD wAPDULen)
// {
// TTx *pTx=GetTx();
// BYTE *pBuf=pTx->pBuf;
// WORD wLen=0,i;
// BYTE byCA=0;

// 	pTx->wRptr	= pTx->wWptr	= 0;
// 	if((byCtrl&CTRL_DIR)&&((byCtrl&PRM)==0))
// 		byCA = 0;
// 	else 
// 		byCA = get_client_addr();
// 	wLen	= MakeFrameHead(sa,pBuf,byCA);
// 	if(NULL != pAPDU)
// 	{
// 		for(i=0;i<wAPDULen;i++)
// 			pBuf[wLen+i]	= pAPDU[i];	
// 	}
// 	pTx->wWptr	= Add698FrameTail(byCtrl,(WORD)(wAPDULen+wLen-1),pBuf);
// }

// WORD GetTxMaxApduSize(void)
// {//最大可发送的APDU大小
// TTx *pTx=GetTx();
	
// 	return (WORD)(pTx->wSize-MAX_HEAD_TAIL_LEN);
// }

void DATETIME_Time(DATETIME *pdatatime,TTime *pTime,TRANS_DIR td)
{
	switch(td)
	{
	case TD12:
		pTime->wYear	= pdatatime->year;
		pTime->Mon		= pdatatime->month;
		pTime->Day		= pdatatime->mday;
		pTime->Hour		= pdatatime->hour;
		pTime->Min		= pdatatime->minute;
		pTime->Sec		= pdatatime->second;
		pTime->wMSec	= pdatatime->msec;		
		break;
	case TD21:
		pdatatime->year	= pTime->wYear;
		pdatatime->month= pTime->Mon;
		pdatatime->mday	= pTime->Day;
		pdatatime->hour	= pTime->Hour;
		pdatatime->minute= pTime->Min;
		pdatatime->second= pTime->Sec;
		pdatatime->msec	= pTime->wMSec;		
		pdatatime->wday	= GetWeek(pTime);
		break;
	}
}

void DATETIME_S_Time(DATETIME_S *pdatatime,TTime *pTime,TRANS_DIR td)
{
	switch(td)
	{
	case TD12:
		pTime->wYear	= pdatatime->year;
		pTime->Mon		= pdatatime->month;
		pTime->Day		= pdatatime->day;
		pTime->Hour		= pdatatime->hour;
		pTime->Min		= pdatatime->minute;
		pTime->Sec		= pdatatime->second;
		pTime->wMSec	= 0;		
		break;
	case TD21:
		pdatatime->year	= pTime->wYear;
		pdatatime->month	= pTime->Mon;
		pdatatime->day	= pTime->Day;
		pdatatime->hour	= pTime->Hour;
		pdatatime->minute= pTime->Min;
		pdatatime->second= pTime->Sec;
		break;
	}
}

///////////////////////////////////////////////////////////////
//	函 数 名 : gdw698buf_TSA
//	函数功能 : 规约缓冲区和地址转换
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年6月20日
//	返 回 值 : WORD
//	参数说明 : BYTE *pBuf,
//				TSA *psa,
//				TRANS_DIR td
//				BOOL bAddHead
///////////////////////////////////////////////////////////////
WORD gdw698buf_TSA(BYTE *pBuf,TSA *psa,TRANS_DIR td,BOOL bAddHead) 
{
WORD tmp=0,i,num=0;

	switch(td)
	{
	case TD12:
		memset(psa,0,sizeof(TSA));
		if(bAddHead)
			num	= pBuf[tmp++];
		else	
			num	= (BYTE)(pBuf[tmp++]&ADDR_BITS)+1;
		if(num)	
		{
			if(bAddHead)
			{
				psa->AF	= pBuf[tmp++];
				num = num-1;
				if(num >= 1)
				{
					for(i=0;i<num;i++)
						psa->addr[num-i-1]	= pBuf[tmp++];
				}
			}
			else
			{
				psa->AF = pBuf[tmp-1];
				if(num >= 1)
				{
					for(i=0;i<num;i++)
						psa->addr[num-i-1]	= pBuf[tmp++];
				}
			}			
		}
		break;
	case TD21:
		if(bAddHead)
		{
			pBuf[tmp++]	= ((psa->AF&ADDR_BITS)+1)+1;
			pBuf[tmp++]	= psa->AF;
			for(i=0;i<((psa->AF&ADDR_BITS)+1);i++)
				pBuf[tmp++]	= psa->addr[(psa->AF&ADDR_BITS)-i];	
		}
		else
		{
			pBuf[tmp++]	= psa->AF;
			for(i=0;i<((psa->AF&ADDR_BITS)+1);i++)
				pBuf[tmp++]	= psa->addr[(psa->AF&ADDR_BITS)-i];	
		}			
		break;
	}	
	return tmp;
}

WORD gdw698bufLink_TSA(BYTE *pBuf,TSA *psa,TRANS_DIR td) 
{//.链路层需要反转
WORD tmp=0,i,num=0;

	switch(td)
	{
	case TD12:
		memset(psa,0,sizeof(TSA));
		num	= (BYTE)(pBuf[tmp++]&ADDR_BITS)+1;
		if(num)	
		{
			psa->AF = pBuf[tmp-1];
			if(num >= 1)
			{
				for(i=0;i<num;i++)
					psa->addr[i]	= pBuf[tmp++];
			}			
		}
		break;
	case TD21:
		pBuf[tmp++]	= psa->AF;
		for(i=0;i<((psa->AF&ADDR_BITS)+1);i++)
			pBuf[tmp++]	= psa->addr[i];	
		break;
	}	
	return tmp;
}

WORD gdw698bufLink_TSA_Oct_Str(BYTE *pBuf,TSA *psa,TRANS_DIR td)
{//按16进制字符串转换链路地址格式
WORD tmp=0,i;
BYTE num=0;

	switch(td)
	{
	case TD12:
		memset(psa,0,sizeof(TSA));
		num	= pBuf[tmp++];	//转换成Oct_Str个数
		if(num > 16)
			num	= 16;
		if(num)	
		{
			num--;
			psa->AF = (BYTE)num;			
			if(num >= 1)
			{
				for(i=0;i<num;i++)
					psa->addr[i]	= pBuf[tmp++];
			}			
		}
		break;
	case TD21:
		num = (BYTE)((psa->AF&ADDR_BITS)+1);
		if (num > 16)
			num = 16;
		pBuf[tmp++]	= num;		//Oct_Str个数转换成AF
		for(i=0;i<num;i++)
			pBuf[tmp++]	= psa->addr[num-i-1];	
		break;
	}	
	return tmp;
} 

WORD gdw698buf_WORD(BYTE *pBuf,WORD *pword,TRANS_DIR td) 
{
	BYTE *pData = (BYTE*)pword;
	switch(td)
	{
	case TD12:
	//.memset(pword,0,sizeof(WORD));
	//.	*pword	= MW(pBuf[0],pBuf[1]);	
		pData[1]	= pBuf[0];
		pData[0]	= pBuf[1];	
		break;
	case TD21:
		pBuf[0]	= pData[1];
		pBuf[1]	= pData[0];		
		break;
	}	
	return 2;
}

WORD gdw698buf_DWORD(BYTE *pBuf,DWORD *pdword,TRANS_DIR td) 
{
	BYTE *pData = (BYTE*)pdword;
	switch(td)
	{
	case TD12:
		//.memset(pdword,0,sizeof(DWORD));
		//*pdword	= MDW(pBuf[0],pBuf[1],pBuf[2],pBuf[3]);	
		pData[3]	= pBuf[0];
		pData[2]	= pBuf[1];		
		pData[1]	= pBuf[2];
		pData[0]	= pBuf[3];		
		break;
	case TD21:
		pBuf[0]	= pData[3];
		pBuf[1]	= pData[2];		
		pBuf[2]	= pData[1];
		pBuf[3]	= pData[0];		
		break;
	}	
	return 4;
}

WORD gdw698buf_float32(BYTE *pBuf,float *pfloat,TRANS_DIR td) 
{
	BYTE *pData = (BYTE*)pfloat;

	switch(td)
	{
	case TD12:
		pData[3]	= pBuf[0];
		pData[2]	= pBuf[1];		
		pData[1]	= pBuf[2];
		pData[0]	= pBuf[3];	
		break;
	case TD21:
		pBuf[0]	= pData[3];
		pBuf[1]	= pData[2];		
		pBuf[2]	= pData[1];
		pBuf[3]	= pData[0];		
		break;
	}	
	return 4;
}

WORD gdw698buf_OAD(BYTE *pBuf,DWORD *pdword,TRANS_DIR td) 
{
	return gdw698buf_DWORD(pBuf,pdword,td);
}

WORD gdw698buf_OMD(BYTE *pBuf,OMD *pomd,TRANS_DIR td) 
{
	return gdw698buf_DWORD(pBuf,&pomd->value,td);
}

WORD gdw698buf_DATE(BYTE *pBuf,Date *pdate,TRANS_DIR td) 
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		memset(pdate,0,sizeof(Date));
		tmp	= gdw698buf_WORD(pBuf,&pdate->year,TD12);
		pdate->month	= pBuf[tmp++];		// 月
		pdate->mday		= pBuf[tmp++];		// 日
		pdate->wday		= pBuf[tmp++];		// 星期
		break;
	case TD21:
		tmp	= gdw698buf_WORD(pBuf,&pdate->year,TD21);
		pBuf[tmp++]	= pdate->month;		// 月
		pBuf[tmp++]	= pdate->mday;		// 日
		pBuf[tmp++]	= pdate->wday;		// 星期		
		break;
	}	
	return tmp;
}

BOOL TimeHMSIsValid(Time *pTime)
{
	if((pTime->hour > 23)
		||(pTime->minute > 59)
		||(pTime->second > 59))
		return FALSE;
	return TRUE;
}

WORD gdw698buf_TIME(BYTE *pBuf,Time *ptime,TRANS_DIR td) 
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		memset(ptime,0,sizeof(Time));
		ptime->hour		= pBuf[tmp++];	// 月
		ptime->minute	= pBuf[tmp++];	// 日
		ptime->second	= pBuf[tmp++];	// 星期
		break;
	case TD21:
		pBuf[tmp++]	= ptime->hour;		// 月
		pBuf[tmp++]	= ptime->minute;	// 日
		pBuf[tmp++]	= ptime->second;	// 星期
		break;
	}	
	return tmp;
}

WORD gdw698buf_DATETIME(BYTE *pBuf,DATETIME *pdatatime,TRANS_DIR td) 
{
WORD tmp=0;
TTime tm;

	switch(td)
	{
	case TD12:
		memset(pdatatime,0,sizeof(DATETIME));
		tmp	= gdw698buf_WORD(pBuf,&pdatatime->year,TD12);
		pdatatime->month	= pBuf[tmp++];		// 月
		pdatatime->mday		= pBuf[tmp++];		// 日
		pdatatime->wday		= pBuf[tmp++];		// 星期
		pdatatime->hour		= pBuf[tmp++];		// 时
		pdatatime->minute	= pBuf[tmp++];		// 分
		pdatatime->second	= pBuf[tmp++];		// 秒
		tmp	+= gdw698buf_WORD(&pBuf[tmp],&pdatatime->msec,TD12);	
		break;
	case TD21:
		tmp	= gdw698buf_WORD(pBuf,&pdatatime->year,TD21);
		DATETIME_Time(pdatatime,&tm,TD12);
		pBuf[tmp++]	= pdatatime->month;			// 月
		pBuf[tmp++]	= pdatatime->mday;			// 日
		pBuf[tmp++]	= GetWeek(&tm);				// 星期
		pBuf[tmp++]	= pdatatime->hour;			// 时
		pBuf[tmp++]	= pdatatime->minute;		// 分
		pBuf[tmp++]	= pdatatime->second;		// 秒
		tmp	+= gdw698buf_WORD(&pBuf[tmp],&pdatatime->msec,TD21);	
		break;
	}	
	return tmp;
}

WORD gdw698buf_DATETIME_S(BYTE *pBuf,DATETIME_S *pdatatime,TRANS_DIR td) 
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		memset(pdatatime,0,sizeof(DATETIME_S));
		tmp	= gdw698buf_WORD(pBuf,&pdatatime->year,TD12);
		pdatatime->month	= pBuf[tmp++];		// 月
		pdatatime->day		= pBuf[tmp++];		// 日
		pdatatime->hour		= pBuf[tmp++];		// 时
		pdatatime->minute	= pBuf[tmp++];		// 分
		pdatatime->second	= pBuf[tmp++];		// 秒
		break;
	case TD21:
		tmp	= gdw698buf_WORD(pBuf,&pdatatime->year,TD21);
		pBuf[tmp++]	= pdatatime->month;			// 月
		pBuf[tmp++]	= pdatatime->day;			// 日
		pBuf[tmp++]	= pdatatime->hour;			// 时
		pBuf[tmp++]	= pdatatime->minute;		// 分
		pBuf[tmp++]	= pdatatime->second;		// 秒
		break;
	}	
	return tmp;
}

WORD gdw698buf_TI(BYTE *pBuf,TI *pti,TRANS_DIR td) 
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		memset(pti,0,sizeof(TI));
		pti->unit	= pBuf[tmp++];		// 间隔单位(enum{0=秒，1=分，2=时，3=日，4=月，5=年})
		tmp	+= gdw698buf_WORD(&pBuf[tmp],&pti->value,TD12);
		break;
	case TD21:
		pBuf[tmp++]	= pti->unit;		// 间隔单位(enum{0=秒，1=分，2=时，3=日，4=月，5=年})
		tmp	+= gdw698buf_WORD(&pBuf[tmp],&pti->value,TD21);				
		break;
	}	
	return tmp;
}

WORD gdw698buf_OI(BYTE *pBuf,OI *poi,TRANS_DIR td) 
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		memset(poi,0,sizeof(OI));
		tmp	+= gdw698buf_WORD(&pBuf[tmp],poi,TD12);
		break;
	case TD21:		
		tmp	+= gdw698buf_WORD(&pBuf[tmp],poi,TD21);				
		break;
	}	
	return tmp;
}

WORD gdw698buf_TimeTag(BYTE *pBuf,TimeTag *ptimetag,TRANS_DIR td) 
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		memset(ptimetag,0,sizeof(TimeTag));
		tmp	= gdw698buf_DATETIME_S(pBuf,&ptimetag->tmSend,TD12);
		tmp	+= gdw698buf_TI(&pBuf[tmp],&ptimetag->tmDelay,TD12);
		break;
	case TD21:
		tmp	= gdw698buf_DATETIME_S(pBuf,&ptimetag->tmSend,TD21);
		tmp	+= gdw698buf_TI(&pBuf[tmp],&ptimetag->tmDelay,TD21);
		break;
	}	
	return tmp;
}

WORD gdw698buf_DAR(BYTE *pBuf,DAR *pdar,TRANS_DIR td) 
{
WORD tmp=1;

	switch(td)
	{
	case TD12:
		memset(pdar,0,sizeof(DAR));
		*pdar	= (DAR)pBuf[0];
		break;
	case TD21:
		pBuf[0]	= (BYTE)(*pdar);
		break;
	}	
	return tmp;
}

WORD gdw698buf_ROAD(BYTE *pBuf,ROAD *proad,TRANS_DIR td)
{
WORD tmp=0,wNum,wI,tmp2=0;

	switch(td)
	{
	case TD12:
		memset(proad,0,sizeof(ROAD));
		tmp	+= gdw698buf_DWORD(pBuf,&proad->oadMain.value,TD12);
		tmp	+= gdw698Buf_Num(&pBuf[tmp],&wNum,TD12);
		tmp2	= tmp;
		for(wI=0;(wI<wNum)&&(wI<OBJ_MAX_OADS);wI++)
		{
			tmp2	+= gdw698buf_OAD(&pBuf[tmp2],&proad->oadCols.oad[wI].value,TD12);
			proad->oadCols.nNum++;
		}		
		tmp	+= wNum*sizeof(DWORD);
		break;
	case TD21:
		tmp	+= gdw698buf_DWORD(pBuf,&proad->oadMain.value,TD21);
		if(proad->oadCols.nNum > OBJ_MAX_OADS)
			wNum	= OBJ_MAX_OADS;
		else
			wNum	= proad->oadCols.nNum;
		tmp	+= gdw698Buf_Num(&pBuf[tmp],&wNum,TD21);		
		for(wI=0;(wI<wNum);wI++)
		{
			tmp	+= gdw698buf_OAD(&pBuf[tmp],&proad->oadCols.oad[wI].value,TD21);			
		}				
		break;
	}	
	return tmp;
}

WORD gdw698buf_CSD(BYTE *pBuf,CSD *pcsd,TRANS_DIR td)
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		pcsd->choice	= pBuf[tmp++];
		if(pcsd->choice == 0)
			tmp	+= gdw698buf_DWORD(&pBuf[tmp],&pcsd->oad.value,TD12);
		else if(pcsd->choice == 1)
			tmp	+= gdw698buf_ROAD(&pBuf[tmp],&pcsd->road,TD12);
		break;
	case TD21:
		pBuf[tmp++]	= pcsd->choice;
		if(pcsd->choice == 0)
			tmp	+= gdw698buf_DWORD(&pBuf[tmp],&pcsd->oad.value,TD21);
		else if(pcsd->choice == 1)
			tmp	+= gdw698buf_ROAD(&pBuf[tmp],&pcsd->road,TD21);
		break;
	}
	return tmp;
}

WORD gdw698buf_Bit8Str(BYTE *pBuf,BYTE *pbyFlag,TRANS_DIR td)
{
WORD wI;
BYTE byFlag=0;

	switch(td)
	{
	case TD12:		
		for(wI=0;wI<8;wI++)
		{
			if(pBuf[0]&(1<<(7-wI)))
			{
				byFlag	|= (1<<wI);
			}
		}	
		*pbyFlag	= byFlag;
		break;
	case TD21:
		byFlag	= 0;
		for(wI=0;wI<8;wI++)
		{
			if((*pbyFlag)&(1<<(7-wI)))
			{
				byFlag	|= (1<<wI);
			}
		}		
		pBuf[0]	= byFlag;
		break;
	}
	return 1;
}

WORD gdw698buf_ComDCB(BYTE *pBuf,ComDCB *pcomdcb,TRANS_DIR td)
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		pcomdcb->baud		= pBuf[tmp++];
		pcomdcb->parity		= pBuf[tmp++];
		pcomdcb->databits	= pBuf[tmp++];
		pcomdcb->stopbits	= pBuf[tmp++];
		pcomdcb->flowctrl	= pBuf[tmp++];
		break;
	case TD21:
		pBuf[tmp++]	= pcomdcb->baud;
		pBuf[tmp++]	= pcomdcb->parity;
		pBuf[tmp++]	= pcomdcb->databits;
		pBuf[tmp++]	= pcomdcb->stopbits;
		pBuf[tmp++]	= pcomdcb->flowctrl;
		break;
	}
	return tmp;
}

WORD gdw698Buf_MS(BYTE *pBuf,MS *pms,TRANS_DIR td)
{
WORD tmp=0,i,num,tmp2;

	switch(td)
	{
	case TD12:
		memset(pms,0,sizeof(MS));
		pms->choice	= pBuf[tmp++];
		switch(pms->choice)
		{
		case 2: //一组用户类型  	[2] 	SEQUENCE OF unsigned			
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&num,TD12);
			if(num > OBJ_MAX_MSGROUP)
				pms->mst.size	= OBJ_MAX_MSGROUP;
			else
				pms->mst.size	= num;
			for(i=0;i<num;i++)
			{
				if(i < pms->mst.size)
					tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&pms->mst.type[i],TD12);
				else
					tmp	+= objGetDataLen(dt_unsigned,&pBuf[tmp],MAX_NUM_NEST);
			}
			break;
		case 3://一组用户地址
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&num,TD12);
			if(num > OBJ_MAX_MSGROUP)
				pms->msa.size	= OBJ_MAX_MSGROUP;
			else
				pms->msa.size	= num;
			for(i=0;i<num;i++)
			{
				if(i < pms->msa.size)
					tmp	+= gdw698buf_TSA(&pBuf[tmp],&pms->msa.add[i],TD12,TRUE);
				else
					tmp	+= objGetDataLen(dt_TSA,&pBuf[tmp],MAX_NUM_NEST);
			}
			break;
		case 4://一组配置序号			
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&num,TD12);
			if(num > OBJ_MAX_MSNO)
				pms->msi.size	= OBJ_MAX_MSNO;
			else
				pms->msi.size	= num;
			for(i=0;i<num;i++)
			{
				if(i < pms->msi.size)
					tmp	+= gdw698Buf_long_unsigned(&pBuf[tmp],&pms->msi.idx[i],TD12);
				else
					tmp	+= objGetDataLen(dt_long_unsigned,&pBuf[tmp],MAX_NUM_NEST);
			}
			break;
		case 5://一组用户类型区间
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&num,TD12);
			if(num > OBJ_MAX_MSGROUP)
				pms->msrt.size	= OBJ_MAX_MSGROUP;
			else
				pms->msrt.size	= num;
			for(i=0;i<num;i++)
			{
				if(i < pms->msrt.size)
				{
					tmp2	= tmp;
					pms->msrt.region[i].nType	= pBuf[tmp2++];
					tmp2++;
					tmp2	+= gdw698Buf_unsigned(&pBuf[tmp2],&pms->msrt.region[i].start,TD12);
					tmp2++;
					tmp2	+= gdw698Buf_unsigned(&pBuf[tmp2],&pms->msrt.region[i].end,TD12);	
				}
				tmp	+= objGetDataLen(dt_Region,&pBuf[tmp],MAX_NUM_NEST);
			}
			break;
		case 6://一组用户地址空间			
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&num,TD12);
			if(num > OBJ_MAX_MSGROUP)
				pms->msra.size	= OBJ_MAX_MSGROUP;
			else
				pms->msra.size	= num;
			for(i=0;i<num;i++)
			{
				if(i < pms->msra.size)
				{
					tmp2	= tmp;
					pms->msra.region[i].nType	= pBuf[tmp2++];
					tmp2++;
					tmp2	+= gdw698buf_TSA(&pBuf[tmp2],&pms->msra.region[i].start,TD12,TRUE);
					tmp2++;
					tmp2	+= gdw698buf_TSA(&pBuf[tmp2],&pms->msra.region[i].end,TD12,TRUE);	
				}				
				tmp	+= objGetDataLen(dt_Region,&pBuf[tmp],MAX_NUM_NEST);
			}
			break;
		case 7://一组用户序号空间			
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&num,TD12);
			if(num > OBJ_MAX_MSGROUP)
				pms->msri.size	= OBJ_MAX_MSGROUP;
			else
				pms->msri.size	= num;
			for(i=0;i<num;i++)
			{
				if(i < pms->msri.size)
				{
					tmp2	= tmp;
					pms->msri.region[i].nType	= pBuf[tmp2++];
					tmp2++;
					tmp2	+= gdw698Buf_long_unsigned(&pBuf[tmp2],&pms->msri.region[i].start,TD12);
					tmp2++;
					tmp2	+= gdw698Buf_long_unsigned(&pBuf[tmp2],&pms->msri.region[i].end,TD12);	
				}				
				tmp	+= objGetDataLen(dt_Region,&pBuf[tmp],MAX_NUM_NEST);
			}
			break;
		}
		break;
	case TD21:
		pBuf[tmp++]	= pms->choice;
		switch(pms->choice)
		{
		case 2: //一组用户类型  	[2] 	SEQUENCE OF unsigned			
			if(pms->mst.size > OBJ_MAX_MSGROUP)
				pms->mst.size	= OBJ_MAX_MSGROUP;
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&pms->mst.size,TD21);
			for(i=0;i<pms->mst.size;i++)
				tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&pms->mst.type[i],TD21);				
			break;
		case 3://一组用户地址			
			if(pms->msa.size > OBJ_MAX_MSGROUP)
				pms->msa.size	= OBJ_MAX_MSGROUP;
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&pms->msa.size,TD21);			
			for(i=0;i<pms->msa.size;i++)
			{				
				tmp	+= gdw698buf_TSA(&pBuf[tmp],&pms->msa.add[i],TD21,TRUE);			
			}
			break;
		case 4://一组配置序号			
			if(pms->msi.size > OBJ_MAX_MSNO)
				pms->msi.size	= OBJ_MAX_MSNO;
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&pms->msi.size,TD21);
			for(i=0;i<pms->msi.size;i++)
				tmp	+= gdw698Buf_long_unsigned(&pBuf[tmp],&pms->msi.idx[i],TD21);				
			break;
		case 5://一组用户类型区间			
			if(pms->msrt.size > OBJ_MAX_MSGROUP)
				pms->msrt.size	= OBJ_MAX_MSGROUP;
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&pms->msrt.size,TD21);			
			for(i=0;i<pms->msrt.size;i++)
			{
				pBuf[tmp++]	= pms->msrt.region[i].nType;
				pBuf[tmp++]	= dt_unsigned;
				tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&pms->msrt.region[i].start,TD21);
				pBuf[tmp++]	= dt_unsigned;
				tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&pms->msrt.region[i].end,TD21);					
			}
			break;
		case 6://一组用户地址空间			
			if(pms->msra.size > OBJ_MAX_MSGROUP)
				pms->msra.size	= OBJ_MAX_MSGROUP;
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&pms->msra.size,TD21);	
			for(i=0;i<pms->msra.size;i++)
			{
				pBuf[tmp++]	= pms->msra.region[i].nType;
				pBuf[tmp++]	= dt_TSA;
				tmp	+= gdw698buf_TSA(&pBuf[tmp],&pms->msra.region[i].start,TD21,TRUE);
				pBuf[tmp++]	= dt_TSA;
				tmp	+= gdw698buf_TSA(&pBuf[tmp],&pms->msra.region[i].end,TD21,TRUE);	
			}
			break;
		case 7://一组用户序号空间			
			if(pms->msri.size > OBJ_MAX_MSGROUP)
				pms->msri.size	= OBJ_MAX_MSGROUP;
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&pms->msri.size,TD21);			
			for(i=0;i<pms->msri.size;i++)
			{
				pBuf[tmp++]	= pms->msri.region[i].nType;
				pBuf[tmp++]	= dt_long_unsigned;
				tmp	+= gdw698Buf_long_unsigned(&pBuf[tmp],&pms->msri.region[i].start,TD21);
				pBuf[tmp++]	= dt_long_unsigned;
				tmp	+= gdw698Buf_long_unsigned(&pBuf[tmp],&pms->msri.region[i].end,TD21);					
			}
			break;
		}
		break;
	}
	return tmp;
}

WORD gdw698Buf_Variant(BYTE *pBuf,Variant *pvar,TRANS_DIR td)
{
WORD tmp=0;

	switch(td)
	{
	case TD12:
		pvar->type	= pBuf[tmp++];
		switch(pvar->type)
		{
		case dt_Integer	:
			tmp	+= gdw698Buf_Integer(&pBuf[tmp],&pvar->cVal,TD12);
			break;
		case dt_unsigned:
			tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&pvar->byVal,TD12);
			break;
		case dt_boolean	:
			tmp	+= gdw698Buf_boolean(&pBuf[tmp],&pvar->bVal,TD12);
			break;
		case dt_enum:
			tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&pvar->enumVal,TD12);
			break;
		case dt_date_time:
			tmp	+= gdw698buf_DATETIME(&pBuf[tmp],&pvar->dtVal,TD12);
			break;
		case dt_date	:
			tmp	+= gdw698buf_DATE(&pBuf[tmp],&pvar->dVal,TD12);
			break;
		case dt_time	:	
			tmp	+= gdw698buf_TIME(&pBuf[tmp],&pvar->tVal,TD12);
			break;
		case dt_datetime_s:	
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&pvar->tmVal,TD12);
			break;
		case dt_double_long:
			tmp	+= gdw698Buf_double_long(&pBuf[tmp],&pvar->nVal,TD12);	
			break;
		case dt_long_unsigned:	
			tmp	+= gdw698Buf_long_unsigned(&pBuf[tmp],&pvar->wVal,TD12);
			break;
		case dt_long	:	
			tmp	+= gdw698Buf_long(&pBuf[tmp],&pvar->sVal,TD12);
			break;
		case dt_double_long_unsigned:		
			tmp	+= gdw698Buf_double_long_unsigned(&pBuf[tmp],&pvar->dwVal,TD12);
			break;
		case dt_long64:		
			tmp	+= gdw698Buf_long64(&pBuf[tmp],&pvar->llVal,TD12);
			break;			
		case dt_long64_unsigned:		
			tmp	+= gdw698Buf_long64_unsigned(&pBuf[tmp],&pvar->ullVal,TD12);
			break;			
		case dt_float32:
			tmp	+= gdw698buf_float32(&pBuf[tmp],&pvar->fVal,TD12);
			break;
		case dt_float64:		
			tmp	+= gdw698buf_float64(&pBuf[tmp],&pvar->dbVal,TD12);
			break;
		case dt_OI:	
			tmp	+= gdw698buf_OI(&pBuf[tmp],&pvar->oi,TD12);
			break;
		case dt_TI:	
			tmp	+= gdw698buf_TI(&pBuf[tmp],&pvar->ti,TD12);
			break;
		case dt_TSA:
			tmp	+= gdw698buf_TSA(&pBuf[tmp],&pvar->tsa,TD12,TRUE);
			break;
		case dt_OAD:
			tmp	+= gdw698buf_OAD(&pBuf[tmp],&pvar->oad.value,TD12);	
			break;
		case dt_OMD:
			tmp	+= gdw698buf_OMD(&pBuf[tmp],&pvar->omd,TD12);	
			break;
		case dt_CSD:	
			tmp	+= gdw698buf_CSD(&pBuf[tmp],&pvar->csd,TD12);	
			break;
		case dt_COMDCB:	
			tmp	+= gdw698buf_ComDCB(&pBuf[tmp],&pvar->dcb,TD12);	
			break;
		case dt_DAR:
			tmp	+= gdw698buf_DAR(&pBuf[tmp],&pvar->darr,TD12);
			break;
		default:
			tmp	+= objGetDataLen((OBJ_DATA_TYPE)pvar->type,&pBuf[tmp],MAX_NUM_NEST);
			break;
		}
		break;
	case TD21:
		pBuf[tmp++]	= pvar->type;
		switch(pvar->type)
		{
		case dt_Integer	:
			tmp	+= gdw698Buf_Integer(&pBuf[tmp],&pvar->cVal,TD21);
			break;
		case dt_unsigned:
			tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&pvar->byVal,TD21);
			break;
		case dt_boolean	:
			tmp	+= gdw698Buf_boolean(&pBuf[tmp],&pvar->bVal,TD21);
			break;
		case dt_enum:
			tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&pvar->enumVal,TD12);
			break;
		case dt_date_time:
			tmp	+= gdw698buf_DATETIME(&pBuf[tmp],&pvar->dtVal,TD21);
			break;
		case dt_date	:
			tmp	+= gdw698buf_DATE(&pBuf[tmp],&pvar->dVal,TD21);
			break;
		case dt_time	:	
			tmp	+= gdw698buf_TIME(&pBuf[tmp],&pvar->tVal,TD21);
			break;
		case dt_datetime_s:	
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&pvar->tmVal,TD21);
			break;
		case dt_double_long:
			tmp	+= gdw698Buf_double_long(&pBuf[tmp],&pvar->nVal,TD21);	
			break;
		case dt_long_unsigned:	
			tmp	+= gdw698Buf_long_unsigned(&pBuf[tmp],&pvar->wVal,TD21);
			break;
		case dt_long	:	
			tmp	+= gdw698Buf_long(&pBuf[tmp],&pvar->sVal,TD21);
			break;
		case dt_double_long_unsigned:		
			tmp	+= gdw698Buf_double_long_unsigned(&pBuf[tmp],&pvar->dwVal,TD21);
			break;
		case dt_long64:		
			tmp	+= gdw698Buf_long64(&pBuf[tmp],&pvar->llVal,TD21);
			break;			
		case dt_long64_unsigned:		
			tmp	+= gdw698Buf_long64_unsigned(&pBuf[tmp],&pvar->ullVal,TD21);
			break;			
		case dt_float32:
			tmp	+= gdw698buf_float32(&pBuf[tmp],&pvar->fVal,TD21);
			break;
		case dt_float64:		
			tmp	+= gdw698buf_float64(&pBuf[tmp],&pvar->dbVal,TD21);
			break;
		case dt_OI:	
			tmp	+= gdw698buf_OI(&pBuf[tmp],&pvar->oi,TD21);
			break;
		case dt_TI:	
			tmp	+= gdw698buf_TI(&pBuf[tmp],&pvar->ti,TD21);
			break;
		case dt_TSA:
			tmp	+= gdw698buf_TSA(&pBuf[tmp],&pvar->tsa,TD21,TRUE);
			break;
		case dt_OAD:
			tmp	+= gdw698buf_OAD(&pBuf[tmp],&pvar->oad.value,TD21);	
			break;
		case dt_OMD:
			tmp	+= gdw698buf_OMD(&pBuf[tmp],&pvar->omd,TD21);	
			break;
		case dt_CSD:	
			tmp	+= gdw698buf_CSD(&pBuf[tmp],&pvar->csd,TD21);	
			break;
		case dt_COMDCB:	
			tmp	+= gdw698buf_ComDCB(&pBuf[tmp],&pvar->dcb,TD21);	
			break;
		case dt_DAR:
			tmp	+= gdw698buf_DAR(&pBuf[tmp],&pvar->darr,TD21);
			break;
		}
		break;
	}
	return tmp;
}

WORD gdw698buf_RSD(BYTE *pBuf,RSD *prsd,TRANS_DIR td)
{
WORD tmp=0,i;

	switch(td)
	{
	case TD12:
		prsd->choice	= pBuf[tmp++];
		switch(prsd->choice)
		{
		default:
		case 0:			
			break;
		case 1:
			tmp	+= gdw698buf_OAD(&pBuf[tmp],&prsd->sel1.oad.value,TD12);
			tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel1.value,TD12);
			break;
		case 2:
			tmp	+= gdw698buf_OAD(&pBuf[tmp],&prsd->sel2.oad.value,TD12);
			tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel2.from,TD12);
			tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel2.to,TD12);
			tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel2.span,TD12);
			break;
		case 3:
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&prsd->sel3.nNum,TD12);
			for(i=0;i<prsd->sel3.nNum;i++)
			{
				if(i<MAX_SELECTOR2_NUM)
				{
					tmp	+= gdw698buf_OAD(&pBuf[tmp],&prsd->sel3.item[i].oad.value,TD12);
					tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel3.item[i].from,TD12);
					tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel3.item[i].to,TD12);
					tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel3.item[i].span,TD12);
				}
				else
				{
					tmp	+= objGetDataLen(dt_OAD,&pBuf[tmp],MAX_NUM_NEST);
					tmp	+= objGetDataLen((OBJ_DATA_TYPE)pBuf[tmp],&pBuf[tmp+1],MAX_NUM_NEST)+1;
					tmp	+= objGetDataLen((OBJ_DATA_TYPE)pBuf[tmp],&pBuf[tmp+1],MAX_NUM_NEST)+1;
					tmp	+= objGetDataLen((OBJ_DATA_TYPE)pBuf[tmp],&pBuf[tmp+1],MAX_NUM_NEST)+1;
				}		
			}
			if(prsd->sel3.nNum > MAX_SELECTOR2_NUM)
				prsd->sel3.nNum = MAX_SELECTOR2_NUM;
			break;
		case 4:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel4.time,TD12);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel4.ms,TD12);
			break;
		case 5:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel5.time,TD12);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel5.ms,TD12);
			break;
		case 6:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel6.start,TD12);
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel6.end,TD12);
			tmp	+= gdw698buf_TI(&pBuf[tmp],&prsd->sel6.ti,TD12);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel6.ms,TD12);
			break;
		case 7:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel7.start,TD12);
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel7.end,TD12);
			tmp	+= gdw698buf_TI(&pBuf[tmp],&prsd->sel7.ti,TD12);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel7.ms,TD12);
			break;
		case 8:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel8.start,TD12);
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel8.end,TD12);
			tmp	+= gdw698buf_TI(&pBuf[tmp],&prsd->sel8.ti,TD12);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel8.ms,TD12);
			break;
		case 9:
			tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&prsd->sel9.nLast,TD12);
			break;
		case 10:
			tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&prsd->sel10.nTimes,TD12);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel10.ms,TD12);
			break;
		}
		break;
	case TD21:
		pBuf[tmp++]	= prsd->choice;
		switch(prsd->choice)
		{
		default:
		case 0:			
			break;
		case 1:
			tmp	+= gdw698buf_OAD(&pBuf[tmp],&prsd->sel1.oad.value,TD21);
			tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel1.value,TD21);
			break;
		case 2:
			tmp	+= gdw698buf_OAD(&pBuf[tmp],&prsd->sel2.oad.value,TD21);
			tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel2.from,TD21);
			tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel2.to,TD21);
			tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel2.span,TD21);
			break;
		case 3:
			tmp	+= gdw698Buf_Num(&pBuf[tmp],&prsd->sel3.nNum,TD21);
			for(i=0;i<prsd->sel3.nNum;i++)
			{
				tmp	+= gdw698buf_OAD(&pBuf[tmp],&prsd->sel3.item[i].oad.value,TD21);
				tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel3.item[i].from,TD21);
				tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel3.item[i].to,TD21);
				tmp	+= gdw698Buf_Variant(&pBuf[tmp],&prsd->sel3.item[i].span,TD21);
			}
			break;
		case 4:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel4.time,TD21);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel4.ms,TD21);
			break;
		case 5:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel5.time,TD21);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel5.ms,TD21);
			break;
		case 6:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel6.start,TD21);
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel6.end,TD21);
			tmp	+= gdw698buf_TI(&pBuf[tmp],&prsd->sel6.ti,TD21);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel6.ms,TD21);
			break;
		case 7:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel7.start,TD21);
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel7.end,TD21);
			tmp	+= gdw698buf_TI(&pBuf[tmp],&prsd->sel7.ti,TD21);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel7.ms,TD21);
			break;
		case 8:
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel8.start,TD21);
			tmp	+= gdw698buf_DATETIME_S(&pBuf[tmp],&prsd->sel8.end,TD21);
			tmp	+= gdw698buf_TI(&pBuf[tmp],&prsd->sel8.ti,TD21);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel8.ms,TD21);
			break;
		case 9:
			tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&prsd->sel9.nLast,TD21);
			break;
		case 10:
			tmp	+= gdw698Buf_unsigned(&pBuf[tmp],&prsd->sel10.nTimes,TD21);
			tmp	+= gdw698Buf_MS(&pBuf[tmp],&prsd->sel10.ms,TD21);
			break;
		}
		break;
	}
	return tmp;
}

WORD gdw698Buf_Num(BYTE *pBuf,WORD *pw,TRANS_DIR td)
{
WORD tmp=1,wNum;
BYTE *pwTmp = (BYTE*)pw;
	switch(td)
	{
	case TD12:
//		*pw	= 0;
		pwTmp[0] = 0;
		pwTmp[1] = 0;
		if(pBuf[0] & 0x80)
		{
			wNum	= (WORD)(pBuf[0] & 0x7f);	
			if(wNum == 1)
			{
			//..	*pw	= pBuf[1];
				pwTmp[1]  = 0;
				pwTmp[0]  = pBuf[1];
				tmp	++;
			}
			else if(wNum == 2)
			{
			//..	*pw	= MW(pBuf[1],pBuf[2]);
				pwTmp[1] = pBuf[1];
				pwTmp[0] = pBuf[2];
				tmp	+= 2;
			}
			else
			{
			//	*pw = 0;
				pwTmp[0] = 0;
				pwTmp[1] = 0;
			//.	tmp	= 0xFFFF;
			}	
		}
		else
		{
		//	*pw	= pBuf[0];
			pwTmp[1] = 0;
			pwTmp[0] = pBuf[0];
		}
		break;
	case TD21:

		if((((pwTmp[1]<<8) | pwTmp[0]) < 0x80))
			pBuf[0] = pwTmp[0];
		else if(((pwTmp[1]<<8) | pwTmp[0]) < 0x100)
		{
			pBuf[0] = 0x81;
			pBuf[1] = pwTmp[0];
			tmp++;
		}
		else
		{
			pBuf[0] = 0x82;
			pBuf[1] = pwTmp[1];
			pBuf[2] = pwTmp[0];
			tmp	+= 2;
		}
		break;
	}	
	return tmp;
}

WORD gdw698Buf_boolean(BYTE *pbuf,BOOL *pbool,TRANS_DIR td)
{
  BYTE *pData = (BYTE*)pbool;
  switch(td)
  {
  case TD12:  
    memset(pData,0,sizeof(BOOL));
    pData[0] = pbuf[0]; 
    break;
  case TD21: 
    pbuf[0] = pData[0]; 
    break;
  }
  return 1;
}

WORD gdw698Buf_double_long(BYTE *pbuf,long *pvalue,TRANS_DIR td)
{
//	DWORD dwTmp;
	BYTE *pData = (BYTE*)pvalue;
	switch(td)
	{
	case TD12:
	//	memset(pvalue,0,sizeof(long));
	//	*pvalue	= (long)MDW(pbuf[0],pbuf[1],pbuf[2],pbuf[3]);
		pData[3]	= pbuf[0];
		pData[2]	= pbuf[1];		
		pData[1]	= pbuf[2];
		pData[0]	= pbuf[3];
		break;
	case TD21:
	/*	dwTmp	= (DWORD)(pvalue);
		pbuf[0]	= HHBYTE(dwTmp);
 		pbuf[1]	= HLBYTE(dwTmp);		
 		pbuf[2]	= LHBYTE(dwTmp);
 		pbuf[3]	= LLBYTE(dwTmp);*/
		pbuf[0]	= pData[3];
		pbuf[1]	= pData[2];		
		pbuf[2]	= pData[1];
		pbuf[3]	= pData[0];	
		break;
	}	
	return 4;
}

WORD gdw698Buf_double_long_unsigned(BYTE *pbuf,DWORD *pvalue,TRANS_DIR td)
{
	return gdw698buf_DWORD(pbuf,pvalue,td);
}

WORD gdw698Buf_Integer(BYTE *pbuf,char *pvalue,TRANS_DIR td)
{
	switch(td)
	{
	case TD12:	*pvalue	= pbuf[0];	break;
	case TD21:	pbuf[0]	= (BYTE)(*pvalue);	break;
	}
	return 1;
}

WORD gdw698Buf_long(BYTE *pbuf,short *pvalue,TRANS_DIR td)
{
	return gdw698buf_WORD(pbuf,(WORD *)pvalue,td);	
}

WORD gdw698Buf_unsigned(BYTE *pbuf,BYTE *pvalue,TRANS_DIR td)
{
	return gdw698Buf_Integer(pbuf,(char*)pvalue,td);	
}

WORD gdw698Buf_long_unsigned(BYTE *pbuf,WORD *pvalue,TRANS_DIR td)
{
	return gdw698buf_WORD(pbuf,pvalue,td);	
}

WORD gdw698Buf_long64(BYTE *pbuf,DLONG *pvalue,TRANS_DIR td)
{
BYTE *pData = (BYTE*)pvalue;
	switch(td)
	{
	case TD12:	
// 		*pvalue	= MDW(pbuf[0],pbuf[1],pbuf[2],pbuf[3])*0x100000000;	
// 		*pvalue	|= MDW(pbuf[4],pbuf[5],pbuf[6],pbuf[7]);
		pData[7]	= pbuf[0];
		pData[6]	= pbuf[1];		
		pData[5]	= pbuf[2];
		pData[4]	= pbuf[3];
		pData[3]	= pbuf[4];
		pData[2]	= pbuf[5];		
		pData[1]	= pbuf[6];
		pData[0]	= pbuf[7];
		break;
	case TD21:	
// 		pbuf[0]	= (BYTE)((*pvalue)>>56);
// 		pbuf[1]	= (BYTE)((*pvalue)>>48);
// 		pbuf[2]	= (BYTE)((*pvalue)>>40);
// 		pbuf[3]	= (BYTE)((*pvalue)>>32);
// 		pbuf[4]	= (BYTE)((*pvalue)>>24);
// 		pbuf[5]	= (BYTE)((*pvalue)>>16);
// 		pbuf[6]	= (BYTE)((*pvalue)>>8);
// 		pbuf[7]	= (BYTE)(*pvalue);
		pbuf[0]	= pData[7];
		pbuf[1]	= pData[6];		
		pbuf[2]	= pData[5];
		pbuf[3]	= pData[4];	
		pbuf[4]	= pData[3];
		pbuf[5]	= pData[2];		
		pbuf[6]	= pData[1];
		pbuf[7]	= pData[0];	
		break;
	}
	return 8;
}

WORD gdw698buf_float64(BYTE *pbuf,double *pdouble,TRANS_DIR td)
{
//	DLONG *pvalue=(DLONG *)pdouble;
BYTE *pData = (BYTE*)pdouble;	
	switch(td)
	{
	case TD12:	
// 		*pvalue	= MDW(pbuf[0],pbuf[1],pbuf[2],pbuf[3])*0x100000000;	
// 		*pvalue	|= MDW(pbuf[4],pbuf[5],pbuf[6],pbuf[7]);
		pData[7]	= pbuf[0];
		pData[6]	= pbuf[1];		
		pData[5]	= pbuf[2];
		pData[4]	= pbuf[3];
		pData[3]	= pbuf[4];
		pData[2]	= pbuf[5];		
		pData[1]	= pbuf[6];
		pData[0]	= pbuf[7];		
		break;
	case TD21:	
// 		pbuf[0]	= (BYTE)((*pvalue)>>56);
// 		pbuf[1]	= (BYTE)((*pvalue)>>48);
// 		pbuf[2]	= (BYTE)((*pvalue)>>40);
// 		pbuf[3]	= (BYTE)((*pvalue)>>32);
// 		pbuf[4]	= (BYTE)((*pvalue)>>24);
// 		pbuf[5]	= (BYTE)((*pvalue)>>16);
// 		pbuf[6]	= (BYTE)((*pvalue)>>8);
// 		pbuf[7]	= (BYTE)(*pvalue);
		pbuf[0]	= pData[7];
		pbuf[1]	= pData[6];		
		pbuf[2]	= pData[5];
		pbuf[3]	= pData[4];	
		pbuf[4]	= pData[3];
		pbuf[5]	= pData[2];		
		pbuf[6]	= pData[1];
		pbuf[7]	= pData[0];	
		break;
	}
	return 8;
}

WORD gdw698Buf_long64_unsigned(BYTE *pbuf,UDLONG *pvalue,TRANS_DIR td)
{
	BYTE *pData = (BYTE*)pvalue;
	switch(td)
	{
	case TD12:	
// 		*pvalue	= MDW(pbuf[0],pbuf[1],pbuf[2],pbuf[3])*0x100000000;	
// 		*pvalue	|= MDW(pbuf[4],pbuf[5],pbuf[6],pbuf[7]);
		pData[7]	= pbuf[0];
		pData[6]	= pbuf[1];		
		pData[5]	= pbuf[2];
		pData[4]	= pbuf[3];
		pData[3]	= pbuf[4];
		pData[2]	= pbuf[5];		
		pData[1]	= pbuf[6];
		pData[0]	= pbuf[7];
		break;
	case TD21:	
// 		pbuf[0]	= (BYTE)((*pvalue)>>56);
// 		pbuf[1]	= (BYTE)((*pvalue)>>48);
// 		pbuf[2]	= (BYTE)((*pvalue)>>40);
// 		pbuf[3]	= (BYTE)((*pvalue)>>32);
// 		pbuf[4]	= (BYTE)((*pvalue)>>24);
// 		pbuf[5]	= (BYTE)((*pvalue)>>16);
// 		pbuf[6]	= (BYTE)((*pvalue)>>8);
// 		pbuf[7]	= (BYTE)(*pvalue);
		pbuf[0]	= pData[7];
		pbuf[1]	= pData[6];		
		pbuf[2]	= pData[5];
		pbuf[3]	= pData[4];	
		pbuf[4]	= pData[3];
		pbuf[5]	= pData[2];		
		pbuf[6]	= pData[1];
		pbuf[7]	= pData[0];
		break;
	}
	return 8;
}

// int set_data_obj(Variant *val, BYTE *pBuf )
// {
// int nSize = 0;

// 	pBuf[nSize++]	= val->type;
// 	switch( val->type )
// 	{
// 	case dt_boolean:
// 		nSize	+= gdw698Buf_boolean(&pBuf[nSize],&val->bVal,TD21);		
// 		break;
// 	case dt_double_long:		
// 		nSize	+= gdw698Buf_double_long(&pBuf[nSize],&val->nVal,TD21);		
// 		break;
// 	case dt_double_long_unsigned:
// 		nSize	+= gdw698Buf_double_long_unsigned(&pBuf[nSize],&val->dwVal,TD21);			
// 		break;
// 	case dt_Integer:
// 		nSize	+= gdw698Buf_Integer(&pBuf[nSize],&val->cVal,TD21);						
// 		break;
// 	case dt_long:
// 		nSize	+= gdw698Buf_long(&pBuf[nSize],&val->sVal,TD21);			
// 		break;
// 	case dt_unsigned:
// 		nSize	+= gdw698Buf_unsigned(&pBuf[nSize],&val->byVal,TD21);					
// 		break;
// 	case dt_long_unsigned:
// 		nSize	+= gdw698Buf_long_unsigned(&pBuf[nSize],&val->wVal,TD21);							
// 		break;
// 	case dt_long64:
// 		nSize	+= gdw698Buf_long64(&pBuf[nSize],&val->llVal,TD21);
// 		break;
// 	case dt_long64_unsigned:
// 		nSize	+= gdw698Buf_long64_unsigned(&pBuf[nSize],&val->ullVal,TD21);
// 		break;
// 	case dt_date_time:
// 		nSize	+= gdw698buf_DATETIME(&pBuf[nSize],&val->dtVal,TD21);
// 		break;
// 	case dt_datetime_s:
// 		nSize	+= gdw698buf_DATETIME_S(&pBuf[nSize],&val->tmVal,TD21);
// 		break;
// 	case dt_OI:
// 		nSize	+= gdw698buf_WORD(&pBuf[nSize],&val->oi,TD21);
// 		break;
// 	case dt_OAD:
// 		nSize	+= gdw698buf_DWORD(&pBuf[nSize],&val->oad.value,TD21);		
// 		break;
// 	case dt_TI:
// 		nSize	+= gdw698buf_TI(&pBuf[nSize],&val->ti,TD21);
// 		break;
// 	case dt_TSA:
// 		nSize	+= gdw698buf_TSA(&pBuf[nSize],&val->tsa,TD21,TRUE);
// 		break;
// 	case dt_CSD:
// 		nSize	+= gdw698buf_CSD(&pBuf[nSize],&val->csd,TD21);
// 		break;
// 	default:
// 		return -1;
// 	}	
// 	return nSize;
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : get_oad_data_of_rcsd
//	函数功能 : 从一个CSD数据块中找出指定的OAD数据
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年7月11日
//	返 回 值 : WORD
//	参数说明 : DWORD oad,
//				RCSD *rcsdd,
//				BYTE *pData,
//				BYTE *pOutData
///////////////////////////////////////////////////////////////
WORD get_oad_data_of_rcsd(DWORD oad,BYTE *rcsdd,BYTE *pData,BYTE *pOutData)
{
WORD wRc=0,i,tmp=0,wCSDNum;
CSD csdd;

	wCSDNum	= get_csd_num_of_RCSD(rcsdd);
	for(i=0;i<wCSDNum;i++)
	{
		get_no_csd_of_RCSD(rcsdd,i,&csdd);
		if(csdd.choice == 0)
		{
			if(csdd.oad.value == oad)
			{
				tmp	= objGetDataLen((OBJ_DATA_TYPE)pData[wRc],&pData[wRc+1],MAX_NUM_NEST)+1;
				memcpy(pOutData,&pData[wRc],tmp);
				return tmp;
			}			
		}
		wRc	+= objGetDataLen((OBJ_DATA_TYPE)pData[wRc],&pData[wRc+1],MAX_NUM_NEST)+1;
	}
	if(tmp == 0)
	{
// 		Variant varr;
// 		varr.type	= dt_DAR;
// 		varr.darr	= dar_data_block_unavailable;
// 		tmp	+= set_data_obj(&varr,&pOutData[tmp]);
		pOutData[tmp] = 0;
		tmp++;
	}
	return tmp;
}

BOOL GetMP698Addr(TSA *sa)
{
TPort *pPort=(TPort *)GethPort();

	memcpy(sa,&pPort->GyRunInfo.sa,sizeof(TSA));
	return TRUE;
}

// void str_IP(char *pstr,BYTE bystrLen,DWORD *pdwIP,TRANS_DIR td)
// {
// DWORD IPAddress=0;
// BYTE  j,offset=0;

// 	switch(td)
// 	{
// 	case TD12:
// 		for(j=0;(j<bystrLen)&&(j<4);j++)
// 		{
// 			IPAddress	<<= 8;	
// 			IPAddress += (BYTE)pstr[offset+j];
// 		}
// 		*pdwIP	= IPAddress;
// 		break;
// 	case TD21:
// 		if(bystrLen > 4)
// 			bystrLen	= 4;
// 		IPAddress	= *pdwIP;
// 		for(j=0;j<bystrLen;j++)
// 		{
// 			pstr[offset++]		= (BYTE)(IPAddress>>((bystrLen-1-j)*8));	
// 		}
// 		break;
// 	}
// }

// DWORD vstr2IP(char *pstr)
// {
// WORD i,wIPTmp,woffset=0;
// DWORD IPAddress=0,rc;

// 	for( i=0;i<4;i++)
// 	{
// 		IPAddress	<<= 8;	
// 		wIPTmp = ASCII2INT(&pstr[woffset],3);
// 	    if(min(strlen(pstr),16)<woffset)
// 			break;
// 		rc	= SearchHead1(&pstr[woffset],min(strlen(pstr),16)-woffset,0,'.');
// 		if(rc > 3)
// 			break;
// 		woffset	+= (WORD)(rc+1);
// 		if(wIPTmp>=0x100)
// 			break;
// 		else
// 			IPAddress |= wIPTmp;
// 	}
// 	return IPAddress;
// }

// BYTE UpPort2No(TPort *pPort)
// {//根据端口匹配到上报任务检查时间的序号
// BYTE no=0;

// 	if(HHBYTE(pPort->dwID) == MPT_376_3)
// 		no	= 0;
// 	else if(HHBYTE(pPort->dwID) == MPT_ETHERNET)
// 		no	= 1;
// 	else 
// 		no	= 2;
// 	if(no >= MAX_DWNPORT_NUM)
// 		no	= 0;
// 	return no;
// }
#endif