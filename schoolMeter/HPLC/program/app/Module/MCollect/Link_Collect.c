//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	    
//创建日期	
//描述		
//修改记录	
//----------------------------------------------------------------------

#include "AllHead.h"
#include "MCollect.h"

//-----------------------------------------------
//      本文件使用的宏定义
//-----------------------------------------------


#define NODATA				0x40
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//      全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//      本文件使用的变量，常量
//-----------------------------------------------
//透传给F415的698标识
const DWORD OAD_SW[] =  { 
	0x49160200,
	0x49160300,
	0x49160400,
	0x49160500
};

const DWORD OAD_LOCK[] =  { 
	0x43100B00,
	0x43107F00
};

//透传给F415的645标识
const T_DIINFO DI_INFOs[] = 
{ 
	{0, 0x04000F02},	//读取电压电流系数 F415的系数
	{0, 0x04000F03},	//设/读通信地址
	{0, 0x04000F04},	//读F415版本校验
	{0, 0x04000F06},	//读F415的数据状态标志
	{0, 0x04000F07},	//写给F415的时间、温度
	{0, 0x04000F08},	//读F415的SPI恢复失败标志
	{0, 0x04000F09},	//读F415的整体总校验
	{0, 0x04000F10},	//自研算法识别第二回路结果
	{0, 0x04000F11},	//自研算法识别第二回路是空调的概率
	{0, 0x04000F12},	//自研算法识别第二回路是非空调的概率
	{0, 0x04000F13},	//第一回路自研算法识别结果
	{0, 0x04000F14},	//自研算法上一次SVM恶性负载记录
	{0, 0x04000F15},	//自研算法上二次SVM恶性负载记录
	{0, 0x04000F16},	//自研算法上三次SVM恶性负载记录
	{0, 0x04000F17},	//自研算法上四次SVM恶性负载记录
	{0, 0x04000F18},	//自研算法上五次SVM恶性负载记录
	{0, 0x04000F19},	//自研算法SVM指定历史数据为白名单
	{0, 0x02000F01},	//读F415第一回路电压
	{0, 0x02000F02},	//读F415第一回路电流
	{0, 0x02000F03},	//读F415第一回路有功
	{0, 0x02000F04},	//读F415第二回路电压
	{0, 0x02000F05},	//读F415第二回路电流
	{0, 0x02000F06},	//读F415第二回路有功
	{0, 0xDBDF09DB},	//读取415CPU_ID
	{0, 0xDBDF0ADB},	//读取415算法license状态
	{0, 0xDBDF3001},	//设置写威思顿license
	{0, 0xDBDF3003},	//设置写第三方license
	{0, 0x0F0F0F04},	//主站升级数据发送请求
	{0, 0x0F0F0F05},	//主站下发数据帧
	{0, 0x0F0F0F06},	//主站发送完成帧
};

//透传锁具数据标识
const T_DIINFO DI_LOCKs[] = { 	
	{0, 0x02830001},
	{0, 0x04500002},
	{0, 0x06030100},	
};


const BYTE ExtCmdDI[4] = {0x0E,0x12,0x12,0x37};		
const BYTE SubCmdVolTimeDI[4] ={0x34,0x53,0x12,0x0E}; 		

extern TJLTxBuf JLTxBuf[2];
// static BYTE gyBuffer[256];
// BYTE IapSwDataBuffer[1000];
tSwIapTrans SwitchIapTrans;
//-----------------------------------------------
//      内部函数声明
//-----------------------------------------------
extern WORD fcs16( BYTE *cp, WORD len );

//-----------------------------------------------
//      函数定义
//-----------------------------------------------
WORD MW(BYTE Hi,BYTE Lo)
{
	return (WORD)((Hi<<8)+Lo);
}
DWORD MDW(BYTE HH,BYTE HL,BYTE LH,BYTE LL)
{
	return (DWORD)((HH<<24) | (HL<<16) | (LH<<8) | LL);
}
//-----------------------------------------------
//函数功能: 判断是否需要转发给开关本体
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   698.45
//-----------------------------------------------
BOOL IsGyTransOAD(DWORD oadd)
{
//	BYTE i;
//	
//	for (i = 0; i < ARRAY_SZ(OAD_SW); i++)
//	{
//		if (oadd == OAD_SW[i])
//			return TRUE;
//	}
	return FALSE; 
}

//-----------------------------------------------
//函数功能: 判断是否需要转发给锁具
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   698.45
//-----------------------------------------------
BOOL IsLOCKOAD(DWORD oadd)
{
	BYTE i;
	
	for (i = 0; i < ARRAY_SZ(OAD_LOCK); i++)
	{
		if (oadd == OAD_LOCK[i])
			return TRUE;
	}
	return FALSE; 
}

//-----------------------------------------------
//函数功能: 判断是否需要转发给开关本体
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   645-2007
//-----------------------------------------------
BOOL IsGyTransDI(DWORD DI)
{
	BYTE i;
	
    for (i = 0; i < ARRAY_SZ(DI_INFOs); i++)
	{
		if (DI_INFOs[i].byFlag == 0)
		{
			if (DI == DI_INFOs[i].dwDI)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
//-----------------------------------------------
//函数功能: 判断是否需要转发给锁具
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   645-2007
//-----------------------------------------------
BOOL IsLOCKDI(DWORD DI)
{
	BYTE i;
	
    for (i = 0; i < ARRAY_SZ(DI_LOCKs); i++)
	{
		if (DI_LOCKs[i].byFlag == 0)
		{
			if (DI == DI_LOCKs[i].dwDI)
				return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------
//函数功能: 
//
//参数:			 
//			
//
//返回值: 	TRUE/FALSE
//
//备注:     无    
//-----------------------------------------------
BOOL objSmockRead( TFrame698_FULL_BASE *pAfnBase, BYTE *Buf )
{
	BYTE byLen = 0;
	
	pAfnBase->pFrame = (OBJ_EXCEPT_TAIL*)Buf; 
	pAfnBase->pTail  = (OBJ_TAIL*) (Buf + ((pAfnBase->pFrame->wFmLen))+sizeof(BYTE)-2 ); 
	if(pAfnBase->pFrame->byStart != 0x68 )
		return FALSE;		
	byLen = (BYTE)( (pAfnBase->pFrame->byAddrF & 0x0F)+1 );
	
	pAfnBase->pDataAPDU = (BYTE *)( &pAfnBase->pFrame->_byAddrStart+(byLen+3) );
	
	pAfnBase->pObjTsa = (TSA *)(&pAfnBase->pFrame->byAddrF);
	return TRUE;
}

WORD objGetAPDUSize(TFrame698_FULL_BASE *pAfnBase)
{//获得APDU大小
	return (WORD)((DWORD)(pAfnBase->pTail)-(DWORD)(pAfnBase->pDataAPDU));
}

WORD gdw698buf_OAD(BYTE *pBuf,DWORD *pdword,TRANS_DIR td) 
{
	BYTE *pData = (BYTE*)pdword;
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

WORD gdw698buf_WORD(BYTE *pBuf,WORD *pword,TRANS_DIR td) 
{
	BYTE *pData = (BYTE*)pword;
	switch(td)
	{
	case TD12:	
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
				psa->AF = pBuf[tmp++];
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

WORD gdw698Buf_Num(BYTE *pBuf,WORD *pw,TRANS_DIR td)
{
	WORD tmp=1,wNum;
	BYTE *pwTmp = (BYTE*)pw;
	switch(td)
	{
	case TD12:
		pwTmp[0] = 0;
		pwTmp[1] = 0;
		if(pBuf[0] & 0x80)
		{
			wNum	= (WORD)(pBuf[0] & 0x7f);	
			if(wNum == 1)
			{
				pwTmp[1]  = 0;
				pwTmp[0]  = pBuf[1];
				tmp	++;
			}
			else if(wNum == 2)
			{
				pwTmp[1] = pBuf[1];
				pwTmp[0] = pBuf[2];
				tmp	+= 2;
			}
			else
			{
				pwTmp[0] = 0;
				pwTmp[1] = 0;
			}	
		}
		else
		{
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

WORD MakeFrameHead(TSA *sa,BYTE *pBuf,BYTE byClientAddr)
{
	BYTE *pHead;
	WORD wLen=0;
	
	pHead	= pBuf;
	pHead[wLen++]= 0x68;
	wLen	+= 2;//跳过长度域
	wLen	++;	//控制域
	//地址域
	wLen	+= gdw698bufLink_TSA(&pHead[wLen],sa,TD21);
	pHead[wLen++]=byClientAddr;//get_client_addr();	//客户地址区
	//跳过帧头校验
	wLen	+= 2;
	return wLen;
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

WORD _TxFm698Read(TSA *sa,TGetPara *pPara,BYTE *pBuf)
{
	WORD wLen,wAPDULen=0,wOffset=0,wI;
	BYTE *pAPDU;
	
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
	if(pPara->bUseSafeMode)
	{//明文+随机数
		BYTE Head[10];
		Head[wOffset++] = Security_Request; 
		Head[wOffset++] = 0; 
		wOffset	+= gdw698Buf_Num(&Head[wOffset],&wAPDULen,TD21);
		//调整APDU插入安全报文
		for(wI=0;wI<wAPDULen;wI++)
			pAPDU[wAPDULen-1-wI+wOffset]	= pAPDU[wAPDULen-1-wI];
		memcpy(pAPDU,Head,wOffset);
		wAPDULen	+= wOffset;
		pAPDU[wAPDULen++]	= 1;//随机数
		pAPDU[wAPDULen++]	= 16;//随机数长度
		memcpy(&pAPDU[wAPDULen],pPara->Rand,16);
		wAPDULen	+= 16;
	}
	wLen	= Add698FrameTail((BYTE)(0x40 | 3),(WORD)(wAPDULen+wLen-1),pBuf);
	return wLen;
}

WORD Tx_Fm64597_Read(BYTE *pAddr,WORD wCode,BYTE *pOutBuf)
{
T645FmTx97 *pTxFm=(T645FmTx97 *)pOutBuf;
BYTE *pBuf=(BYTE *)(pTxFm+1);
BYTE byHDI,byLDI;
	
	pTxFm->byS68	= 0x68;
	memcpy(pTxFm->MAddr,pAddr,6);
	pTxFm->byE68	= 0x68;
	pTxFm->byCtrl	= 0x01;
	pTxFm->byDataLen= 0x02;
	byHDI           = HIBYTE(wCode) + 0x33;
	byLDI           = LOBYTE(wCode) + 0x33;
	pTxFm->wDI		 = MW(byHDI,byLDI);
	
	pBuf[0]			= lib_CheckSum(&pTxFm->byS68,12);	 
	pBuf[1]			= 0x16;
	return 0x0E;
}

WORD Tx_Fm698_Read(BYTE *pAddr,DWORD dwOad,BYTE *pOutBuf)
{
	TSA sa;
	TGetPara para={0};
	BYTE TxBuf[50];
	
	memset(&sa,0,sizeof(TSA));
	memcpy(sa.addr,pAddr,6);
	sa.len = 5;
	if (dwOad == 0x40010200) //搜表
	{
		sa.type = 1; //通配地址
		para.bUseSafeMode = 0;
	}
	else //抄表
	{
		if(dwOad == REQ_ENG698)
		{
			sa.type = 0; //单地址
			para.bUseSafeMode = 0; 		
		}
		else
		{
			sa.type = 0; //单地址
			para.bUseSafeMode = 1;
		}
	}
	para.byType	= 1;
	para.pTxBuff= TxBuf;
	para.wBufLen	+= gdw698buf_OAD(&para.pTxBuff[para.wBufLen],&dwOad,TD21);				
	return _TxFm698Read(&sa,&para,pOutBuf);
}

WORD Tx_Fm64507_Read(BYTE *pAddr,DWORD dwCode,BYTE *pOutBuf)
{
T645FmRx *pTxFm=(T645FmRx *)pOutBuf;
BYTE *pBuf=(BYTE *)(pTxFm+1);
BYTE byHHDI,byHLDI,byLHDI,byLLDI; 

	pTxFm->byS68	= 0x68;
#if 1
	memcpy(pTxFm->MAddr,pAddr,6);
#else
	memset(pTxFm->MAddr,0xAA,6); //调试用
#endif
	pTxFm->byE68	= 0x68;
	pTxFm->byCtrl	= 0x11;
	pTxFm->byDataLen= 0x04;
	byHHDI           = HHBYTE(dwCode) + 0x33;
	byHLDI           = HLBYTE(dwCode) + 0x33;
    byLHDI           = LHBYTE(dwCode) + 0x33;
	byLLDI           = LLBYTE(dwCode) + 0x33;
	pTxFm->dwDI		 = MDW(byHHDI,byHLDI,byLHDI,byLLDI);
	
	pBuf[0]			= lib_CheckSum(&pTxFm->byS68,14);	 
	pBuf[1]			= 0x16;
	return 0x10;
}

WORD Tx_Fm64507_Set(BYTE *pAddr,BYTE byCtrl,DWORD dwCode,BYTE *pData,BYTE dataLen,BYTE *pOutBuf)
{
T645FmRxEx *pTxFm=(T645FmRxEx *)pOutBuf;
BYTE i,*pBuf=(BYTE *)(pTxFm+1); 
WORD wLen = 0;
	
    for (i = 0; i < dataLen; i++)
	{
		pData[i] += 0x33;
	}
	pTxFm->byS68	= 0x68;
#if 1
	memcpy(pTxFm->MAddr,pAddr,6);
#else
	memset(pTxFm->MAddr,0xAA,6); //调试用
#endif
	pTxFm->byE68	= 0x68;
	pTxFm->byCtrl	= byCtrl;
	if(dwCode != 0)
	{
		pTxFm->byDataLen = (BYTE)(12+dataLen);
		pBuf[wLen++]     = LLBYTE(dwCode) + 0x33;
		pBuf[wLen++]     = LHBYTE(dwCode) + 0x33;
		pBuf[wLen++]     = HLBYTE(dwCode) + 0x33;
		pBuf[wLen++]     = HHBYTE(dwCode) + 0x33;
	}
	else
	{
		pTxFm->byDataLen = (BYTE)(8+dataLen);
	}
    pBuf[wLen++]	= 0x35; //密码
    pBuf[wLen++]	= 0x33;
	pBuf[wLen++]	= 0x33;
	pBuf[wLen++]	= 0x33;
	pBuf[wLen++]	= 0xAB; //操作者代码
	pBuf[wLen++]	= 0x89;
	pBuf[wLen++]	= 0x67;
	pBuf[wLen++]	= 0x45;
	memcpy(&pBuf[wLen],pData,dataLen);
	wLen += dataLen;
	pBuf[wLen]	    = lib_CheckSum(&pTxFm->byS68,wLen+sizeof(T645FmRxEx));
	wLen ++;
	pBuf[wLen++]	= 0x16;
	return (wLen+sizeof(T645FmRxEx));
}

// WORD MakeRecord(DWORD MainOad,DWORD SubOad,BYTE byLastN,BYTE *pInBuf,BYTE *pOutBuf)
// {
// WORD wLen = 0;
// BOOL bRc = FALSE;
// TTime tTime;
// DATETIME_S DateTime;
// BYTE byI,byPhase=0;
// BYTE byDevCause=0;
// BYTE byErrFlag=0;
// BYTE byErrCause=0;
// BYTE byDlqChange=0;
// BYTE byCurrentDir=0;
// BYTE aStatus[2]={0};
// WORD wTemperature=0;
// WORD szVol[3]={0,0,0};
// DWORD szCur[3]={0,0,0}; 
// BYTE ActionCause=0,ActionWay=0;
	
// 	switch(MainOad)
// 	{
// 	case 0x35030200:
// 		byDevCause = pInBuf[0];
// 		byPhase = pInBuf[1];
// 		tTime.Sec = lib_BBCDToBin(pInBuf[2]);						
// 		tTime.Min = lib_BBCDToBin(pInBuf[3]);
// 		tTime.Hour = lib_BBCDToBin(pInBuf[4]);
// 		tTime.Day = lib_BBCDToBin(pInBuf[5]);
// 		tTime.Mon = lib_BBCDToBin(pInBuf[6]);
// 		tTime.wYear = (WORD)(lib_BBCDToBin(pInBuf[7])+2000);
// 		szVol[0] = lib_WBCDToBin( MW(pInBuf[9],pInBuf[8]) );
// 		szVol[1] = lib_WBCDToBin( MW(pInBuf[11],pInBuf[10]) );
// 		szVol[2] = lib_WBCDToBin( MW(pInBuf[13],pInBuf[12]) );
//         szCur[0] = lib_DWBCDToBin( MDW(0,pInBuf[16]&0x7F,pInBuf[15],pInBuf[14]) ) * 100;
// 		szCur[1] = lib_DWBCDToBin( MDW(0,pInBuf[19]&0x7F,pInBuf[18],pInBuf[17]) ) * 100;
// 		szCur[2] = lib_DWBCDToBin( MDW(0,pInBuf[22]&0x7F,pInBuf[21],pInBuf[20]) ) * 100;
// 		byCurrentDir = pInBuf[23];
// 		break;
// 	case 0x35040200:
// 		byDlqChange = pInBuf[1];
// 		byPhase = pInBuf[2];
// 		tTime.Sec = lib_BBCDToBin(pInBuf[3]);						
// 		tTime.Min = lib_BBCDToBin(pInBuf[4]);
// 		tTime.Hour = lib_BBCDToBin(pInBuf[5]);
// 		tTime.Day = lib_BBCDToBin(pInBuf[6]);
// 		tTime.Mon = lib_BBCDToBin(pInBuf[7]);
// 		tTime.wYear = (WORD)(lib_BBCDToBin(pInBuf[8])+2000);
//         szVol[0] = lib_WBCDToBin( MW(pInBuf[10],pInBuf[9]) );
// 		szVol[1] = lib_WBCDToBin( MW(pInBuf[12],pInBuf[11]) );
// 		szVol[2] = lib_WBCDToBin( MW(pInBuf[14],pInBuf[13]) );
//         szCur[0] = lib_DWBCDToBin( MDW(0,pInBuf[17]&0x7F,pInBuf[16],pInBuf[15]) ) * 100;
// 		szCur[1] = lib_DWBCDToBin( MDW(0,pInBuf[20]&0x7F,pInBuf[19],pInBuf[18]) ) * 100;
// 		szCur[2] = lib_DWBCDToBin( MDW(0,pInBuf[23]&0x7F,pInBuf[22],pInBuf[21]) ) * 100;
// 		byCurrentDir = pInBuf[24];
// 		break;
// 	case 0x35050200:
//         tTime.Sec = lib_BBCDToBin(pInBuf[0]);						
// 		tTime.Min = lib_BBCDToBin(pInBuf[1]);
// 		tTime.Hour = lib_BBCDToBin(pInBuf[2]);
// 		tTime.Day = lib_BBCDToBin(pInBuf[3]);
// 		tTime.Mon = lib_BBCDToBin(pInBuf[4]);
// 		tTime.wYear = (WORD)(lib_BBCDToBin(pInBuf[5])+2000);
//         ActionCause = pInBuf[6];
// 		ActionWay = pInBuf[7];
// 		break;
// 	case 0x35060200:
// 		byErrFlag = (pInBuf[0] & BIT0);
// 		byErrCause = pInBuf[1];
// 		byPhase = pInBuf[2];
// 		tTime.Sec = lib_BBCDToBin(pInBuf[3]);						
// 		tTime.Min = lib_BBCDToBin(pInBuf[4]);
// 		tTime.Hour = lib_BBCDToBin(pInBuf[5]);
// 		tTime.Day = lib_BBCDToBin(pInBuf[6]);
// 		tTime.Mon = lib_BBCDToBin(pInBuf[7]);
// 		tTime.wYear = (WORD)(lib_BBCDToBin(pInBuf[8])+2000);
// 		szVol[0] = lib_WBCDToBin( MW(pInBuf[10],pInBuf[9]) );
// 		szVol[1] = lib_WBCDToBin( MW(pInBuf[12],pInBuf[11]) );
// 		szVol[2] = lib_WBCDToBin( MW(pInBuf[14],pInBuf[13]) );
//         szCur[0] = lib_DWBCDToBin( MDW(0,pInBuf[17]&0x7F,pInBuf[16],pInBuf[15]) ) * 100;
// 		szCur[1] = lib_DWBCDToBin( MDW(0,pInBuf[20]&0x7F,pInBuf[19],pInBuf[18]) ) * 100;
// 		szCur[2] = lib_DWBCDToBin( MDW(0,pInBuf[23]&0x7F,pInBuf[22],pInBuf[21]) ) * 100;
// 		byCurrentDir = pInBuf[24];
// 		break;
// 	case 0x30110200:
// 		{
// 			BYTE byOffset = (SubOad==0x201E0200)?0:6;
// 			tTime.Sec = lib_BBCDToBin(pInBuf[0+byOffset]);						
// 			tTime.Min = lib_BBCDToBin(pInBuf[1+byOffset]);
// 			tTime.Hour = lib_BBCDToBin(pInBuf[2+byOffset]);
// 			tTime.Day = lib_BBCDToBin(pInBuf[3+byOffset]);
// 			tTime.Mon = lib_BBCDToBin(pInBuf[4+byOffset]);
// 			tTime.wYear = (WORD)(lib_BBCDToBin(pInBuf[5+byOffset])+2000);
// 		}
// 		break;
// 	case 0x350A0200:
//         tTime.Sec = lib_BBCDToBin(pInBuf[3]);						
// 		tTime.Min = lib_BBCDToBin(pInBuf[4]);
// 		tTime.Hour = lib_BBCDToBin(pInBuf[5]);
// 		tTime.Day = lib_BBCDToBin(pInBuf[6]);
// 		tTime.Mon = lib_BBCDToBin(pInBuf[7]);
// 		tTime.wYear = (WORD)(lib_BBCDToBin(pInBuf[8])+2000);
// 		wTemperature = MW(pInBuf[10],pInBuf[9]);
// 		wTemperature = lib_WBCDToBin(wTemperature);
// 		break;
// 	}
// 	switch(SubOad)
// 	{
// 	case 0x20220200: //事件记录序号
// 		{
// 			DWORD dwNo = ReadEventCount(MainOad);
// 			if (dwNo >= byLastN)
// 			{
// 				dwNo -= byLastN;
// 			}
// 			pOutBuf[wLen++] = 0x06;
// 			pOutBuf[wLen++] = HHBYTE(dwNo);
// 			pOutBuf[wLen++] = HLBYTE(dwNo);
// 			pOutBuf[wLen++] = LHBYTE(dwNo);
// 			pOutBuf[wLen++] = LLBYTE(dwNo);
// 		}
// 		break;
// 	case 0x201E0200: //事件发生时间
// 		pOutBuf[wLen++] = 0x1C;
// 		DATETIME_S_Time(&DateTime,&tTime,TD21);
// 		wLen += gdw698buf_DATETIME_S(&pOutBuf[wLen],&DateTime,TD21);
// 		break;
// 	case 0x20200200: //事件结束时间
// 		pOutBuf[wLen++] = 0x1C;
// 		DATETIME_S_Time(&DateTime,&tTime,TD21);
// 		wLen += gdw698buf_DATETIME_S(&pOutBuf[wLen],&DateTime,TD21);
// 		break;
// 	case 0x20240200: //事件发生源
// 		pOutBuf[wLen++] = 0x16;
// 		pOutBuf[wLen++] = byErrFlag;
//         break;
// 	case 0x26002200: //发生时刻温度
//         pOutBuf[wLen++] = 0x10;
// 	    pOutBuf[wLen++] = HIBYTE(wTemperature);
// 		pOutBuf[wLen++] = LOBYTE(wTemperature);
// 		break;
// 	case 0x26032200: //设备运行状态字
// 		pOutBuf[wLen++] = 0x04;
// 		pOutBuf[wLen++] = 0x10;
// 		api_GetEquipmStatesWord(eStatus,aStatus);
// 		lib_InverseData(aStatus,2);
// 		lib_ExchangeBit(&pOutBuf[wLen],&aStatus[0],2);
// 		wLen += 2;
// 		break;
//     case 0x26062200: //设备故障原因
// 		pOutBuf[wLen++] = 0x11;
// 		pOutBuf[wLen++] = byDevCause;
// 		break;
// 	case 0x26070200: //设备告警原因
// 	case 0x26072200:
// 		pOutBuf[wLen++] = 0x11;
// 		pOutBuf[wLen++] = byErrCause;
// 		break;
// 	case 0x26082200: //断路器变位原因
// 		pOutBuf[wLen++] = 0x11;
// 		pOutBuf[wLen++] = byDlqChange;
// 		break;
// 	case 0x26092200: //相别
// 		pOutBuf[wLen++] = 0x04;
// 		pOutBuf[wLen++] = 0x08;
// 		lib_ExchangeBit(&pOutBuf[wLen],&byPhase,1);
// 		wLen ++;
// 		break;
// 	case 0x20002200: //发生时刻电压
// 		pOutBuf[wLen++] = 0x01;
// 		pOutBuf[wLen++] = 0x03;
// 		for (byI=0; byI<3; byI++) 
// 		{
// 			pOutBuf[wLen++] = 0x12;
// 			pOutBuf[wLen++] = HIBYTE(szVol[byI]);
// 			pOutBuf[wLen++] = LOBYTE(szVol[byI]);
// 		}
// 		break;
// 	case 0x20012200: //发生时刻电流
// 		pOutBuf[wLen++] = 0x01;
// 		pOutBuf[wLen++] = 0x03;
// 		for (byI=0; byI<3; byI++) 
// 		{
// 			pOutBuf[wLen++] = 0x05;
// 			pOutBuf[wLen++] = HHBYTE(szCur[byI]);
// 			pOutBuf[wLen++] = HLBYTE(szCur[byI]);
// 			pOutBuf[wLen++] = LHBYTE(szCur[byI]);
// 			pOutBuf[wLen++] = LLBYTE(szCur[byI]);
// 		}
// 		break;
// 	case 0x20012A00: //发生时刻电流方向
// 		pOutBuf[wLen++] = 0x04;
// 		pOutBuf[wLen++] = 0x08;
// 		lib_ExchangeBit(&pOutBuf[wLen],&byCurrentDir,1);
// 		wLen ++;
// 		break;
// 	case 0x33420206: //保护投退-动作原因
//         pOutBuf[wLen++] = 0x11;
// 		pOutBuf[wLen++] = ActionCause;
// 		break;
// 	case 0x33420209: //保护投退-投退方式
// 		pOutBuf[wLen++] = 0x16;
// 		pOutBuf[wLen++] = ActionWay;
// 		break;
// 	default:
// 		pOutBuf[wLen++] = 0;
// 		break;
// 	}
// 	return wLen;
// }
//-----------------------------------------------
//函数功能: 698与645帧格式重组
//
//参数:			 
//			
//
//返回值: 	帧长度
//
//备注:     无    
//-----------------------------------------------
/*WORD GyData_698_645(BYTE Ch,BYTE *pData,TRANS_DIR TD)
{
TFrame698_FULL_BASE afnBase;
T698Head *pHead_698 = NULL;
T645FmRxEx *pHead_645 = NULL;
T698HCS *pHCS = NULL;
T698FCS *pFCS = NULL;
TTransPara *pPara = NULL;
TSA Sa;
TTime Time;
DATETIME_S DateTime;
BYTE DataEx[128];
BYTE *pBuf = NULL;
BYTE *pDataAPDU = NULL;
BYTE byI,byLenEx = 0;
WORD wLen = 0,wTmp,wOffset,wOffset2;
DWORD dwOAD,dwDI,dwTmp;
BOOL bRc;
	
    if(Ch == ePT_4852)
	{
		pPara = &JLTxBuf[0].transPara;
	}
	else
	{
		pPara = &JLTxBuf[1].transPara;

	}
	if (TD12 == TD) //698 --> 645
	{
		if ( objSmockRead(&afnBase,pData) )
		{
			if (afnBase.pObjTsa->len == 5)
			{
				pPara->byCtrl = afnBase.pFrame->byCtrl;
				pPara->byCA = afnBase.pObjTsa->addr[6];
				pDataAPDU = afnBase.pDataAPDU;
                if (pDataAPDU[0] == Security_Request) //安全请求
				{
					pPara->bySecurity = 1;
					if (pDataAPDU[2] == 0x82)
					{
						pDataAPDU += 5;
					}
					else if(pDataAPDU[2] == 0x81)
					{
						pDataAPDU += 4;
					}
					else
					{
						pDataAPDU += 3;
					}
				}
				else
				{
					pPara->bySecurity = 0;
				}
				pPara->byAPDU = *pDataAPDU;
				pDataAPDU ++;
				
				pPara->byType = *pDataAPDU;
				pDataAPDU ++;
				
				pPara->byPIID = *pDataAPDU;
				pDataAPDU ++;
				
				if (pPara->byAPDU == GET_Request)
				{					
					if (pPara->byType == 0x01)
					{															//跳过PIID
						wTmp = gdw698buf_OAD(pDataAPDU,&dwOAD,TD12);
						pDataAPDU += wTmp;
						if (OAD2DI(&dwOAD,&dwDI,TD12))
						{
							pPara->dwDI = dwDI;				
							wLen = Tx_Fm64507_Read(afnBase.pObjTsa->addr,dwDI,gyBuffer);
						}
					}
					else if (pPara->byType == 0x03)
					{
						wTmp = gdw698buf_OAD(pDataAPDU,&dwOAD,TD12);
						pDataAPDU += wTmp;
						if (*pDataAPDU == 0x09)							//方法9
						{
							pDataAPDU ++;
							
							pPara->byRec = *pDataAPDU;					//上N次记录
							pDataAPDU ++;
							
							pPara->byNum = *pDataAPDU;
							pDataAPDU ++;
							if (pPara->byNum <= ARRAY_SZ(pPara->dwOAD))
							{
								for (byI=0;byI<pPara->byNum;byI++)
								{
									pDataAPDU ++;						//OAD
									wTmp = gdw698buf_OAD(pDataAPDU,&pPara->dwOAD[byI],TD12);
									pDataAPDU += wTmp;
								}	
								if (OAD2DI(&dwOAD,&dwDI,TD12))
								{
									pPara->dwDI = dwDI;	
									dwDI |= pPara->byRec;	
									wLen = Tx_Fm64507_Read(afnBase.pObjTsa->addr,dwDI,gyBuffer);
								}
							}							
						}
					}
				}
				else if (pPara->byAPDU == SET_Request)
				{
					if (pPara->byType == 0x01)
					{
						wTmp = gdw698buf_OAD(pDataAPDU,&dwOAD,TD12);
						pDataAPDU += wTmp;
						if (OAD2DI(&dwOAD,&dwDI,TD12))
						{
							pPara->dwDI = dwDI;
							
							if ((dwDI == 0x04001301) || (dwDI == 0x04001302) || (dwDI == 0x04001303))
							{
								if (*pDataAPDU == 0x02)
								{
									pDataAPDU ++;
									if (*pDataAPDU == 0x02)
									{
										pDataAPDU ++;
										if (*pDataAPDU == 0x06)
										{
											pDataAPDU ++;
											dwTmp = MDW(pDataAPDU[0],pDataAPDU[1],pDataAPDU[2],pDataAPDU[3]);
											dwTmp /= 10;
											dwTmp = lib_DWBinToBCD(dwTmp);
											DataEx[byLenEx ++] = LLBYTE(dwTmp);
											DataEx[byLenEx ++] = LHBYTE(dwTmp);
										}
									}
								}
							}
							else if (dwDI == 0x040010FF)
							{
								if (*pDataAPDU == 0x04)
								{
									pDataAPDU ++;
									if (*pDataAPDU == 0x40)
									{
										pDataAPDU ++;
										lib_ExchangeBit(&DataEx[0],&pDataAPDU[0],1);
										lib_ExchangeBit(&DataEx[1],&pDataAPDU[1],1);
										lib_ExchangeBit(&DataEx[2],&pDataAPDU[2],1);
										lib_ExchangeBit(&DataEx[3],&pDataAPDU[3],1);
										lib_ExchangeBit(&DataEx[4],&pDataAPDU[4],1);
										lib_ExchangeBit(&DataEx[5],&pDataAPDU[5],1);
										lib_ExchangeBit(&DataEx[6],&pDataAPDU[6],1);
										lib_ExchangeBit(&tCycleSInfoRom.byCtrl,&pDataAPDU[7],1);
										api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom);
										byLenEx = 7;
									}
								}
							}
							else if ((dwDI == 0x04001409) || (dwDI == 0x04001408))
							{
                                if (*pDataAPDU == 0x02)
								{
									pDataAPDU += 6;
									if (pDataAPDU[0] == 0x05)
									{
										dwTmp = MDW(pDataAPDU[1],pDataAPDU[2],pDataAPDU[3],pDataAPDU[4]);
										dwTmp = lib_DWBinToBCD(dwTmp);
										DataEx[byLenEx++] = LLBYTE(dwTmp);
                                        DataEx[byLenEx++] = LHBYTE(dwTmp);
										DataEx[byLenEx++] = HLBYTE(dwTmp);
										DataEx[byLenEx++] = HHBYTE(dwTmp);
										pDataAPDU += 5;
									}
									if (pDataAPDU[0] == 0x06)
									{
										dwTmp = MDW(pDataAPDU[1],pDataAPDU[2],pDataAPDU[3],pDataAPDU[4]);
										dwTmp = lib_DWBinToBCD(dwTmp);
										DataEx[byLenEx++] = LLBYTE(dwTmp);
                                        DataEx[byLenEx++] = LHBYTE(dwTmp);
										DataEx[byLenEx++] = HLBYTE(dwTmp);
										DataEx[byLenEx++] = HHBYTE(dwTmp);
									}
								}
							}
							else if (dwDI == 0x04001401)
							{
								if (*pDataAPDU == 0x02)
								{
									pDataAPDU += 6;
									if (pDataAPDU[0] == 0x05)
									{
										dwTmp = MDW(pDataAPDU[1],pDataAPDU[2],pDataAPDU[3],pDataAPDU[4]);
										dwTmp /= 100;
										dwTmp = lib_DWBinToBCD(dwTmp);
										DataEx[byLenEx++] = LLBYTE(dwTmp);
                                        DataEx[byLenEx++] = LHBYTE(dwTmp);
										DataEx[byLenEx++] = HLBYTE(dwTmp);
									}
								}
							}
							else if(dwDI == 0x040016FF)
							{
								if (*pDataAPDU == 0x02)
								{
									if ((pDataAPDU[2] == 0x01) && (pDataAPDU[3] == 0x03))
									{
										pDataAPDU += 4;
										for (byI = 0; byI < 3; byI++)
										{
											if (pDataAPDU[2] == 0x10)
											{
												dwTmp = MDW(0,0,pDataAPDU[3],pDataAPDU[4]);
												dwTmp = lib_DWBinToBCD(dwTmp);
												DataEx[byLenEx++] = LLBYTE(dwTmp);
												DataEx[byLenEx++] = LHBYTE(dwTmp);
											}
											if (pDataAPDU[8] == 0x10)
											{
												dwTmp = MDW(0,0,pDataAPDU[9],pDataAPDU[10]);
												dwTmp = lib_DWBinToBCD(dwTmp);
												DataEx[byLenEx++] = LLBYTE(dwTmp);
												DataEx[byLenEx++] = LHBYTE(dwTmp);
											}
											pDataAPDU += 14; //跳过一组温度阈值
										}
									}
								}
							}
							wLen = Tx_Fm64507_Set(afnBase.pObjTsa->addr,0x14,dwDI,DataEx,byLenEx,gyBuffer);
						}						
					}
				}
				else if (pPara->byAPDU == ACTION_Request)
				{
					if (pPara->byType == 0x01)
					{
						wTmp = gdw698buf_OAD(pDataAPDU,&dwOAD,TD12);
						pDataAPDU += wTmp;
						if (OAD2DI(&dwOAD,&dwDI,TD12))
						{
							pPara->dwDI = dwDI;
							if (dwDI == 0x06010101)
							{
								pDataAPDU += 2;
								if ((pDataAPDU[0] == 0x02) && (pDataAPDU[1] == 0x04))
								{
									if(pDataAPDU[7] == 0x11)
									{
										DataEx[0] = lib_BBinToBCD(pDataAPDU[8]);
									}
									else
									{
										DataEx[0] = 0;
									}
								}
								else
								{
									DataEx[0] = 0;
								}
								DataEx[1] = 0x02; //单位-分
								wLen = Tx_Fm64507_Set(afnBase.pObjTsa->addr,0x1C,dwDI,DataEx,2,gyBuffer);
							}
							else if (dwDI == 0x06010102)
							{
								wLen = Tx_Fm64507_Set(afnBase.pObjTsa->addr,0x1C,dwDI,DataEx,0,gyBuffer);
							}
							else
							{
								wLen = Tx_Fm64507_Set(afnBase.pObjTsa->addr,0x1B,0,(BYTE*)&dwDI,4,gyBuffer);
							}
						}						
					}
				}
			}
		}
	}
	else //645 --> 698
	{
		pHead_645 = (T645FmRxEx *)pData;
		pBuf = (BYTE *)(pHead_645+1);
		for (byI=0;byI<pHead_645->byDataLen;byI++)
			pBuf[byI] -= 0x33;
		dwDI = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
		pBuf += sizeof(DWORD);
		pHead_645->byDataLen -= sizeof(DWORD);
		memcpy(DataEx,pBuf,pHead_645->byDataLen>sizeof(DataEx)?sizeof(DataEx):pHead_645->byDataLen);
		Sa.AF = 0x05;

		Sa.addr[0] = pHead_645->MAddr[0];
		Sa.addr[1] = pHead_645->MAddr[1];
		Sa.addr[2] = pHead_645->MAddr[2];
		Sa.addr[3] = pHead_645->MAddr[3];
		Sa.addr[4] = pHead_645->MAddr[4];
		Sa.addr[5] = pHead_645->MAddr[5];
		
		pHead_698 = (T698Head *)gyBuffer;
		pHead_698->byS68 = 0x68;
		pHead_698->byCtrl = (BYTE)(pPara->byCtrl|0x80);				//控制域
		wLen = sizeof(T698Head);
		wLen += gdw698bufLink_TSA(&gyBuffer[wLen],&Sa,TD21);
		gyBuffer[wLen++] = pPara->byCA;								//客户机地址
		wOffset = wLen;
		pHCS = (T698HCS *)&gyBuffer[wLen];
		wLen += sizeof(T698HCS);
		if (pPara->bySecurity) //安全请求
		{
             gyBuffer[wLen++] = Security_Response;
			 gyBuffer[wLen++] = 0x00;
			 wOffset2 = wLen;
			 gyBuffer[wLen++] = 0x00; //长度放在最后填充
		}
		gyBuffer[wLen++] = (BYTE)(pPara->byAPDU|0x80);
		gyBuffer[wLen++] = pPara->byType;
		gyBuffer[wLen++] = pPara->byPIID;								//PIID				
		
		if (pPara->byAPDU == GET_Request)
		{	
			bRc = OAD2DI(&dwOAD,&dwDI,TD21);
			if (pPara->byType == 0x03)
				bRc = OAD2DI(&dwOAD,&pPara->dwDI,TD21);
			if (bRc)
			{
				wLen += gdw698buf_OAD(&gyBuffer[wLen],&dwOAD,TD21);
				if (dwOAD == 0x26030200)
				{
					gyBuffer[wLen++] = 0x01;	
					gyBuffer[wLen++] = 0x04;
					gyBuffer[wLen++] = 0x10;
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[0],1);
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[1],1);
				}
				else if (dwOAD == 0x26000200)
				{
					gyBuffer[wLen++] = 0x01;	
					gyBuffer[wLen++] = 0x01;
					gyBuffer[wLen++] = 0x07;
					for (byI=0;byI<7;byI++)
					{
						gyBuffer[wLen++] = 0x10;
						wTmp = MW(DataEx[byI*2+1],DataEx[byI*2+0]);
						gyBuffer[wLen++] = HIBYTE(wTmp);
						gyBuffer[wLen++] = LOBYTE(wTmp);
					}
				}
				else if (dwOAD == 0x26110200)
				{
					gyBuffer[wLen++] = 0x01;	
					gyBuffer[wLen++] = 0x02;
					gyBuffer[wLen++] = 0x0A;
					for (byI=0;byI<10;byI++)
					{
						gyBuffer[wLen++] = 0x12;
						wTmp = MW(DataEx[byI*2+1],DataEx[byI*2+0]);
						gyBuffer[wLen++] = HIBYTE(wTmp);
						gyBuffer[wLen++] = LOBYTE(wTmp);
					}
				}
				else if ((dwOAD == 0x35030200) || (dwOAD == 0x35040200) || (dwOAD == 0x35050200) || (dwOAD == 0x35060200) || (dwOAD == 0x30110200) || (dwOAD == 0x350A0200))
				{
					gyBuffer[wLen++] = pPara->byNum;
					for (byI=0;byI<pPara->byNum;byI++)
					{
						gyBuffer[wLen++] = 0x00;							//OAD
						wLen += gdw698buf_OAD(&gyBuffer[wLen],&pPara->dwOAD[byI],TD21);
					}
					if ( pHead_645->byDataLen && ( ! lib_IsAllAssignNum(DataEx,0x00,pHead_645->byDataLen) ) )
					{
						gyBuffer[wLen++] = 0x01;								//有数据
						gyBuffer[wLen++] = 0x01;								//1条记录
						
						for (byI=0;byI<pPara->byNum;byI++)
						{
							wLen += MakeRecord(dwOAD,pPara->dwOAD[byI],pPara->byRec,DataEx,&gyBuffer[wLen]);
						}	
					}
					else
					{
						gyBuffer[wLen++] = 0x01;
                        gyBuffer[wLen++] = 0x00;
					}
				}
				else if ((dwOAD == 0x35030E00) || (dwOAD == 0x35040E00) || (dwOAD == 0x35050E00) || (dwOAD == 0x35060E00) || (dwOAD == 0x30110E00))
				{
					gyBuffer[wLen++] = 0x01;	
					gyBuffer[wLen++] = 0x11;
					gyBuffer[wLen++] = lib_BBCDToBin(DataEx[0]);
				}
				else if (dwOAD == 0x40000200)
				{
					gyBuffer[wLen++] = 0x01;
					gyBuffer[wLen++] = 0x1C;
					Time.Sec = lib_BBCDToBin(DataEx[0]);
					Time.Min = lib_BBCDToBin(DataEx[1]);
					Time.Hour = lib_BBCDToBin(DataEx[2]);
					Time.Day = lib_BBCDToBin(DataEx[4]);
					Time.Mon = lib_BBCDToBin(DataEx[5]);
					Time.wYear = (WORD)(lib_BBCDToBin(DataEx[6])+2000);
					DATETIME_S_Time(&DateTime,&Time,TD21);
					wLen += gdw698buf_DATETIME_S(&gyBuffer[wLen],&DateTime,TD21);
				}
				else if (dwOAD == 0x40010200)
				{
					gyBuffer[wLen++] = 0x01;
					gyBuffer[wLen++] = 0x09;
					gyBuffer[wLen++] = 0x06;
					gyBuffer[wLen++] = DataEx[5];
					gyBuffer[wLen++] = DataEx[4];
					gyBuffer[wLen++] = DataEx[3];
					gyBuffer[wLen++] = DataEx[2];
					gyBuffer[wLen++] = DataEx[1];
					gyBuffer[wLen++] = DataEx[0];
				}
				else if ((dwOAD == 0x49050200) || (dwOAD == 0x49040200) || (dwOAD == 0x49200200))
				{
					gyBuffer[wLen++] = 0x01;
					gyBuffer[wLen++] = 0x02;
					gyBuffer[wLen++] = 0x02;
					gyBuffer[wLen++] = 0x06;
					dwTmp = MDW(0,0,DataEx[1],DataEx[0]);
					dwTmp = lib_DWBCDToBin(dwTmp);
					dwTmp *= 10;
					gyBuffer[wLen++] = HHBYTE(dwTmp);
					gyBuffer[wLen++] = HLBYTE(dwTmp);
					gyBuffer[wLen++] = LHBYTE(dwTmp);
					gyBuffer[wLen++] = LLBYTE(dwTmp);
					
					gyBuffer[wLen++] = 0x12;
					gyBuffer[wLen++] = 0x00;
					gyBuffer[wLen++] = 0x00;					
				}
				else if (dwOAD == 0x491A0200)
				{
					gyBuffer[wLen++] = 0x01;
					gyBuffer[wLen++] = 0x04;
					gyBuffer[wLen++] = 0x40;
					
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[0],1);
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[1],1);
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[2],1);
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[3],1);
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[4],1);
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[5],1);
					lib_ExchangeBit(&gyBuffer[wLen++],&DataEx[6],1);
					lib_ExchangeBit(&gyBuffer[wLen++],&tCycleSInfoRom.byCtrl,1);
				}
				else if ((dwOAD == 0x49160200) || (dwOAD == 0x49160300) || (dwOAD == 0x49160400))
				{
					gyBuffer[wLen++] = 0x01;
                    gyBuffer[wLen++] = 0x02;
					gyBuffer[wLen++] = 0x04;
					gyBuffer[wLen++] = 0x03; //bool
					gyBuffer[wLen++] = 0x00;
                    gyBuffer[wLen++] = 0x03; //bool
					gyBuffer[wLen++] = 0x00;
					gyBuffer[wLen++] = 0x05; //double-long
                    if(LHBYTE(dwOAD) == 0x04)
					{//Attribute 4
						dwTmp = MDW(0,DataEx[2],DataEx[1],DataEx[0]);
						dwTmp = lib_DWBCDToBin(dwTmp);
						dwTmp *= 100;
					}
					else
					{
						dwTmp = MDW(DataEx[3],DataEx[2],DataEx[1],DataEx[0]);
						dwTmp = lib_DWBCDToBin(dwTmp);
					}
					gyBuffer[wLen++] = HHBYTE(dwTmp);
					gyBuffer[wLen++] = HLBYTE(dwTmp);
					gyBuffer[wLen++] = LHBYTE(dwTmp);
					gyBuffer[wLen++] = LLBYTE(dwTmp); 
					gyBuffer[wLen++] = 0x06; //double-long-unsigned
                    if(LHBYTE(dwOAD) == 0x03)
					{//Attribute 3
						dwTmp = MDW(DataEx[7],DataEx[6],DataEx[5],DataEx[4]);
						dwTmp = lib_DWBCDToBin(dwTmp);
					}
					else
					{
						dwTmp = 0;
					}
					gyBuffer[wLen++] = HHBYTE(dwTmp);
					gyBuffer[wLen++] = HLBYTE(dwTmp);
					gyBuffer[wLen++] = LHBYTE(dwTmp);
					gyBuffer[wLen++] = LLBYTE(dwTmp);
				}
				else if (dwOAD == 0x350A0600)
				{
					gyBuffer[wLen++] = 0x01;
                    gyBuffer[wLen++] = 0x02; //struct
					gyBuffer[wLen++] = 0x02;
					gyBuffer[wLen++] = 0x01; //array
					gyBuffer[wLen++] = 0x03;
					for (byI = 0; byI < 3; byI++)
					{
						gyBuffer[wLen++] = 0x02; 
						gyBuffer[wLen++] = 0x04;
						gyBuffer[wLen++] = 0x10; //long
						dwTmp = MDW(0,0,DataEx[byI*4+1],DataEx[byI*4]);
						dwTmp = lib_DWBCDToBin(dwTmp);
						gyBuffer[wLen++] = LHBYTE(dwTmp);
						gyBuffer[wLen++] = LLBYTE(dwTmp);
						gyBuffer[wLen++] = 0x10; //long
						gyBuffer[wLen++] = 0x00; 
						gyBuffer[wLen++] = 0x00; 
						gyBuffer[wLen++] = 0x10; //long
						dwTmp = MDW(0,0,DataEx[byI*4+3],DataEx[byI*4+2]);
						dwTmp = lib_DWBCDToBin(dwTmp);
						gyBuffer[wLen++] = LHBYTE(dwTmp);
						gyBuffer[wLen++] = LLBYTE(dwTmp);
						gyBuffer[wLen++] = 0x10; //long
						gyBuffer[wLen++] = 0x00; 
						gyBuffer[wLen++] = 0x00;
					}
                    gyBuffer[wLen++] = 0x00;
				}
			}
		}
		else if (pPara->byAPDU == SET_Request)
		{
			if (OAD2DI(&dwOAD,&pPara->dwDI,TD21))
				wLen += gdw698buf_OAD(&gyBuffer[wLen],&dwOAD,TD21);
			
			if (pHead_645->byCtrl == 0x94)
				gyBuffer[wLen++] = 0x00;								//设置成功
			else
				gyBuffer[wLen++] = 0x01;								//设置失败
		}
		else if (pPara->byAPDU == ACTION_Request)
		{
			if (OAD2DI(&dwOAD,&pPara->dwDI,TD21))
				wLen += gdw698buf_OAD(&gyBuffer[wLen],&dwOAD,TD21);
			
			if ((pHead_645->byCtrl == 0x9B) || (pHead_645->byCtrl == 0x9C))
				gyBuffer[wLen++] = 0x00;								//操作成功
			else
				gyBuffer[wLen++] = 0x01;								//操作失败
			
			gyBuffer[wLen++] = 0x00;									//返回数据
		}
		gyBuffer[wLen++] = 0x00;										//跟随上报信息域
		gyBuffer[wLen++] = 0x00;										//时间标签

		if (pPara->bySecurity)
		{
			gyBuffer[wOffset2] = (BYTE)(wLen-wOffset2-1);
			gyBuffer[wLen++] = 0x01;
			gyBuffer[wLen++] = 0x00; //MAC验证
			gyBuffer[wLen++] = 0x04;
			gyBuffer[wLen++] = 0xDF;
			gyBuffer[wLen++] = 0x00;
			gyBuffer[wLen++] = 0x00;
			gyBuffer[wLen++] = 0x01;
			pPara->bySecurity = 0;
		}

		pFCS = (T698FCS *)&gyBuffer[wLen];
		wLen += sizeof(T698FCS);
		
		pHead_698->Len[0] = LOBYTE(wLen-sizeof(BYTE));
		pHead_698->Len[1] = HIBYTE(wLen-sizeof(BYTE));
		
		wTmp = fcs16(&gyBuffer[1],(WORD)(wOffset-1)); 
		pHCS->byLCS = HIBYTE(wTmp);
		pHCS->byHCS = LOBYTE(wTmp);
		
		wTmp = fcs16(&gyBuffer[1],(WORD)(wLen-sizeof(WORD)-1));
		pFCS->byLCS = HIBYTE(wTmp);
		pFCS->byHCS = LOBYTE(wTmp);
		gyBuffer[wLen++] = 0x16;			
	}
	memcpy(pData,gyBuffer,wLen);
	
	return wLen;
}*/

// 与本体通信组帧
void api_TxSwitchCmd(eF460SpiCommCmd CmdTppe, BYTE SerialNo, BYTE *pAddr)
{
	TSerial *p = &Serial[SerialNo];
	BYTE *pBuf = p->ProBuf;
	BYTE bOffset,i;
	TFourByteUnion  Temp;
	
	pBuf[0] = 0xfe;
	pBuf[1] = 0xfe;
	pBuf[2] = 0xfe;
	pBuf[3] = 0xfe;
	
	pBuf[4] = 0x68;
	memset(&pBuf[5], 0x99, 6); //5,6,7,8,9,10
	pBuf[11] = 0x68;
	pBuf[12] = 0x14;          // cmd 0x11
	pBuf[13] = 0x27;          // len
	
	memcpy(&pBuf[14], ExtCmdDI, 4);    // 14,15,16,17
	
	pBuf[18] = 0x35;
	pBuf[19] = 0x33;
	pBuf[20] = 0x33;
	pBuf[21] = 0x33;
	pBuf[22] = 0xAB;
	pBuf[23] = 0x89;
	pBuf[24] = 0x67;
	pBuf[25] = 0x45;
	
	if( CmdTppe == eVolTime )
	{
		memcpy( &pBuf[26],SubCmdVolTimeDI,4);// 26,27,28,29
	}
	
	pBuf[30] = 0x33;
	pBuf[31] = 0x33;
	pBuf[32] = 0x33;
	pBuf[33] = 0x33;
	
	bOffset = 0;
	
	if( CmdTppe == eVolTime )
	{
		for( i = 0; i< 3; i++ )
		{
			Temp.d = 0;
			api_GetRemoteData(PHASE_A + 0x1000 * i+REMOTE_U, DATA_HEX, 3, sizeof(DWORD), Temp.b);
			memcpy(&pBuf[34 + bOffset], Temp.b, 4);
			bOffset = bOffset+4;
		}
		pBuf[34 + bOffset] = RealTimer.wYear%2000;          // YY
		bOffset = bOffset+1;
		memcpy( &pBuf[34 + bOffset], (BYTE *)&RealTimer.Mon, 5);// Mon day hour min sec
		bOffset = bOffset+5;
//		pBuf[34 + bOffset] = (api_GetReportMode() & 0x3f);
//		if(api_SetOrReadReportFalg(1,0) == 0)
		{
			pBuf[34 + bOffset] |= 0x80;
		}
		bOffset++;
		
		for( i=0; i < bOffset; i++ )
		{
			pBuf[34+i] += 0x33; 
		}
	}
	
	pBuf[34 + bOffset] = lib_CheckSum(&pBuf[4], 34 + bOffset-4); // cs
	bOffset = bOffset+1;
	pBuf[34 + bOffset] = 0x16;
	bOffset = bOffset + 1;
	
	p->SendLength = 34 + bOffset;
	p->TXPoint = 0;
	
	// 将接收指针清零，用于判断是否收到数据
	p->RXWPoint = 0;
	p->RXRPoint = 0;
	//执行延时
	// p->ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;  直接发送
	SerialMap[SerialNo].SCIDisableRcv(SerialMap[SerialNo].SCI_Ph_Num);
	SerialMap[SerialNo].SCIEnableSend(SerialMap[SerialNo].SCI_Ph_Num);
}

// //-----------------------------------------------
// //函数功能: 从外部flash获取升级数据，存放在IapSwDataBuffer
// //
// //参数:		无
// //
// //返回值: 	无
// //
// //备注:为了提高升级效率，一包数据192*4=768=256*3 可以写RN2026 3个page
// // IapDataBuffer   192*4+2字节校验
// //-----------------------------------------------
// void GetSwIapData(void)
// {
// 	DWORD dwAddr;
// 	WORD wOffset, wTemp, wFrameNo;
// 	BYTE i;

// 	wOffset = 0;
// 	wFrameNo = SwitchIapTrans.IapFrameNo;
// 	for (i = 0; i < 4; i++)
// 	{
// 		if (wFrameNo < tIap.wAllFrameNo)
// 		{
// 			dwAddr = IAP_CODE_ADDR;
// 			dwAddr += (wFrameNo / FRAMENUM_OF_ONE_SECTOR) * SECTOR_SIZE + (wFrameNo % FRAMENUM_OF_ONE_SECTOR) * LEN_OF_ONE_FRAME;
// 			api_ReadMemRecordData(dwAddr, LEN_OF_ONE_FRAME, &IapSwDataBuffer[wOffset]);
// 		}
// 		else
// 		{
// 			// 不足4个192，补齐0xff 再发送，但是校验是按照192*FrameNo来计算
// 			memset(&IapSwDataBuffer[wOffset], 0xFF, LEN_OF_ONE_FRAME);
// 		}

// 		wOffset += LEN_OF_ONE_FRAME;
// 		wFrameNo++;
// 	}
// 	wTemp = CalCRC16(IapSwDataBuffer, DATA_LEN_OF_ONE_FRAME);

// 	memcpy((BYTE *)&IapSwDataBuffer[DATA_LEN_OF_ONE_FRAME], (BYTE *)&wTemp, sizeof(WORD));
// }

// //-----------------------------------------------
// //函数功能: // 与本体通信组帧
// //
// //参数:		无
// //
// //返回值: 	无
// //
// //备注:
// //-----------------------------------------------
// void api_TxSwitchIap(BYTE Type)
// {
// 	BYTE SerialNo = 2;
// 	TSerial *p = &Serial[SerialNo];
// 	BYTE *pBuf = p->ProBuf;
// 	BYTE bOffset, i;
// 	TFourByteUnion Temp;
//     WORD wData;

// 	if ((Type == 0) || (Type>3))
// 	{
// 		return;
// 	}

// 	pBuf[0] = 0xfe;
// 	pBuf[1] = 0xfe;
// 	pBuf[2] = 0xfe;
// 	pBuf[3] = 0xfe;

// 	pBuf[4] = 0x68;
// 	pBuf[5] = 0x17;
// 	pBuf[6] = Type;
// 	pBuf[7] = 0x00;

// 	if( Type == 1 )
// 	{
// 		memcpy((BYTE *)&pBuf[8], (BYTE *)&tIap.dwFileSize, 4);
// 		memcpy((BYTE *)&pBuf[12], (BYTE *)&tIap.wAllFrameNo, 2);
// 		memcpy((BYTE *)&pBuf[14], (BYTE *)&tIap.wCrc16, 2);
// 		memset((BYTE *)&pBuf[16], 0xff, IAP_SEND_DATA_LEN - 14);
// 	}
// 	else if( Type == 2 )
// 	{
// 		memcpy((BYTE *)&pBuf[8], (BYTE *)&SwitchIapTrans.IapFrameNo, 2);				   // 帧序号
// 		memcpy((BYTE *)&pBuf[10], (BYTE *)&IapSwDataBuffer[0], DATA_LEN_OF_ONE_FRAME + 2); // 数据192*4+2字节校验
// 	}
// 	else if (Type == 3)
// 	{
// 		pBuf[8] = 0x55; // 升级指令
// 		memset((BYTE *)&pBuf[9], 0xff, IAP_SEND_DATA_LEN - 7);
// 	}

// 	wData = fcs16(pBuf + 4, DATA_LEN_OF_ONE_FRAME + 8);
// 	pBuf[10 + DATA_LEN_OF_ONE_FRAME + 2] = (wData & 0x00FF);
// 	pBuf[10 + DATA_LEN_OF_ONE_FRAME + 3] = ((wData >> 8) & 0x00FF);

// 	p->SendLength = IAP_SEND_DATA_LEN + 4;
// 	p->TXPoint = 0;

// 	// 将接收指针清零，用于判断是否收到数据
// 	p->RXWPoint = 0;
// 	p->RXRPoint = 0;
// 	//执行延时
// 	// p->ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;  直接发送
// 	SerialMap[SerialNo].SCIDisableRcv(SerialMap[SerialNo].SCI_Ph_Num);
// 	SerialMap[SerialNo].SCIEnableSend(SerialMap[SerialNo].SCI_Ph_Num);
// }