//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	lq 
//创建日期	
//描述		
//修改记录  基于23年量测开关，删除与本体通信部分，只保留主动抄读GDF415部分
//----------------------------------------------------------------------
#include "AllHead.h"
#include "MCollect.h"
#include "Dlt698_45.h"

#if(SEL_F415 == YES)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define CJFALG_RETRY_COUN				4				//CJFALG 重试控制块支持个数
#define F415_POWER_RESET 				YES 			//是否启用F415电源重启功能，启用后如果出现异常重启后，会导致仿真或下载F415时候，出现掉仿真现象或者烧录不上程序的问题
#define F415_REC_FAILED_TIMES			20				//接收失败次数
#define COLLECTION_RECOVERY_FOR_415		30				//415升级导致采集暂停，需要的恢复时间30s
#define CALIBRATION_CYCLE_FOR_415		6				//6025定时给415校时的时间周期，按小时记
#define COUNTUPPERLIMIT					60000			//复位6025与415通信测试的计数上限
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
enum {JLS_IDLE=0,JLS_READY,JLS_WORK,JLS_COMPLETE}Jlstatus;

typedef struct TCJFlagRetryCtrl_t
{
	BYTE Flag[CJFALG_RETRY_COUN];
	BYTE TimerConst[CJFALG_RETRY_COUN];
	BYTE Timer[CJFALG_RETRY_COUN];
	BYTE RetryNum[CJFALG_RETRY_COUN];
	BYTE Valid[CJFALG_RETRY_COUN / 8 + 1];
} TCJFlagRetryCtrl;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
DWORD m_CJFlag[2] = {0,0};   								//本体采集标识
TElectIdentify ElectIdentify[IDF_LOOP_NUM][IDF_RES_NUM];	//抄读的F415负荷辨识结果
extern TTimeTem	TF415TimeTem;
BOOL IsUpdateF415 = FALSE; 									//升级415的标志
WORD Collection_Recovery_Time;								//采集恢复时间定时器
BYTE Address_645[6];										//415回传的地址域，同6025通信地址一致
AMessage_T MessageData;										//415返回的识别结果
//TSendRec目前的作用只是监测两路uart
TCollectSendRec TSendRec[2] = 
{
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
};
BYTE ControlResetStatus;									//是否允许415复位，0：默认允许；1：不允许415复位	

TTriggerViolationSign Harmonic;
TTriggerViolationSign Svm;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
TJLTxBuf JLTxBuf[2]; 										//0（对应485），1（对应本体)
static BYTE m_CJInterv = 15; 								//采集间隔
static TMCJCtl  tCJCtl = {0};

static TCJFlagRetryCtrl s_CJFlagRetryCtrl;					//CJFlag重试
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
extern BOOL IsMeterValid(BYTE byNo);
extern void JudgeBaudChange(BYTE SerialNo, DWORD dwBaud);
extern WORD fcs16( BYTE *cp, WORD len );


static void ReadProtectUnitVariat(BYTE Cmd);
static void SerialTriggerOneSend(BYTE Serial, BYTE *Buf, WORD Size);
static void ProtectedUnitOptRelay(BYTE Cmd, BYTE Src, BYTE Cause);
static void ReadProtectUnitPara(BYTE Cmd);
static void SecLoopCJFlagRetry(BYTE Ch);
void SetCJFlagRetry(BYTE Ch,BYTE Flag, BYTE RetryNum, BYTE Sec);
void ClrCJFlagRetry(BYTE Flag);
static void WriteProtectUnitParaSync(void);
extern WORD Tx_Fm64507_Read(BYTE *pAddr,DWORD dwCode,BYTE *pOutBuf);
extern WORD Tx_Fm64507_Set(BYTE *pAddr,BYTE byCtrl,DWORD dwCode,BYTE *pData,BYTE dataLen,BYTE *pOutBuf);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 往级联端口写数据
//
//参数: 	无
//			
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void JLWrite(BYTE destCh,BYTE *hBuf,DWORD dwLen,BYTE byGyType,BYTE sourceCh)
{
	BYTE chNo = (destCh == ePT_UART1_F415) ? 0 : 1;
	
	if(JLTxBuf[chNo].byValid != JLS_IDLE)
	{
		if(JLTxBuf[chNo].byValid == JLS_COMPLETE)
		{//1此时已透传成功，还未置成JLS_IDLE
			JLTxBuf[chNo].byValid = JLS_IDLE;
			JLTxBuf[chNo].byBaudSelect = 0;
		}
		else
		{//2别的端口在级联
			return;
		}
	}
	JLTxBuf[chNo].byValid      = JLS_READY;
	JLTxBuf[chNo].byBaudSelect = 0;
	JLTxBuf[chNo].bySourceNo   = sourceCh;
	JLTxBuf[chNo].byPortFlag   = (1<<destCh);
	JLTxBuf[chNo].wRptr	       = 0;
	JLTxBuf[chNo].byGyType     = byGyType;		
	JLTxBuf[chNo].wWptr        = (WORD)(dwLen+4);
	
	//加4个fe唤醒帧
	memset(JLTxBuf[chNo].TransBuf,0xFE,4);
	memcpy(&JLTxBuf[chNo].TransBuf[4],hBuf,dwLen);
}

//-----------------------------------------------
//函数功能: 检查是上行端口收到的报文，是否需要转发处理
//
//参数: 	sourchCh-源端口，pDestCh-目标端口
//			
//返回值:  	BOOL
//
//备注:   
//-----------------------------------------------
BOOL IsNeedTransmit(BYTE sourceCh,BYTE byGy, BYTE *pBuf,BYTE *pDestCh)
{
	BYTE *pMeterNo = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);
	BYTE ComAddr[6];
	WORD wLen = 0;
	DWORD dwDI;
	TFourByteUnion tPassWord;
	TTwoByteUnion Temp16;
	BYTE Flag =0;
	WORD wResult;
	BYTE i;
	TMuchPassword Password;

	memcpy( &ComAddr[0], pMeterNo, 6);
	lib_InverseData(&ComAddr[0], 6);
	pMeterNo = &ComAddr[0];
	
	if(PRO_NO_SPECIAL == byGy)
	{
		T645FmRx *pFm = (T645FmRx *)&pBuf[0];
		
		// 给GDF415的数据，地址默认就是表地址，判断不同的数据标识进行转发
		if( (memcmp(pFm->MAddr,pMeterNo,6) == 0) ||
			lib_IsAllAssignNum(pFm->MAddr,0xAA,6) ||
			lib_IsAllAssignNum(pFm->MAddr,0x88,6) ||
			lib_IsAllAssignNum(pFm->MAddr,0x99,6) )
		{
			dwDI = MDW( HHBYTE(pFm->dwDI)-0x33, HLBYTE(pFm->dwDI)-0x33, LHBYTE(pFm->dwDI)-0x33, LLBYTE(pFm->dwDI)-0x33 );

			if ( IsGyTransDI(dwDI) )
			{
				*pDestCh = ePT_UART1_F415;
				Flag = 1;
			}

			if( Flag != 0 )
			{
				if ((pFm->byCtrl == 0x11)		//读参数
				||(pFm->byCtrl == 0x14)			// 写参数
				||(pFm->byCtrl == 0x1C)			// 控制
				||(pFm->byCtrl == 0x1D))		// 升级
				{
					if((dwDI == 0x04000F02) && (pFm->byCtrl == 0x14))//不允许F415设置电压电流系数
					{
						return FALSE;
					}
					if(pFm->byCtrl == 0x1D)
					{
						tPassWord.b[0] = pBuf[14] - 0x33;
						tPassWord.b[1] = pBuf[15] - 0x33;
						tPassWord.b[2] = pBuf[16] - 0x33;
						tPassWord.b[3] = pBuf[17] - 0x33;

						//针对于升级的启动与结束判断密码是否合法————————参考CheckPassword()函数
						if((dwDI == 0x0f0f0f04)||(dwDI == 0x0f0f0f06))
						{
							//从EEPROM读取密码数据 如果EEPROM数据错误 不判密码 防止EEPROM错误或者未初始化无法打开厂内模式
							if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)Password.Password645) != TRUE )
							{
								return FALSE;
							}
							//判断是否为02级密码&&厂内规约，针对于校园表第二路485的升级
							if((gPr645[sourceCh].eProtocolType == ePROTOCOL_DLT645_FACTORY)&&(tPassWord.b[0] == MINGWEN_H_PASSWORD_LEVEL) )
							{
								//密码对比
								for( i=0; i<3; i++ )
								{
									if(tPassWord.b[i+1] != Password.Password645[gPr645[sourceCh].bLevel][i] )
									{
										break;
									}
								}
								if (i !=3)
								{
									return FALSE;
								}
								if((dwDI == 0x0f0f0f04)||(dwDI == 0x0f0f0f05)||(dwDI == 0x0f0f0f06))//415升级-----升级请求,中间帧发送，最后启动升级请求
								{
									api_SetCollectionTaskStatus415(0);
								}
							}
							else
							{
								return FALSE;
							}
						}
					}

					memcpy((BYTE *)&Address_645[0],pFm->MAddr,6);
					memset(pFm->MAddr,0x00,6);
					wLen = (Serial[sourceCh].RXWPoint + MAX_PRO_BUF - Serial[sourceCh].BeginPosDlt645) % MAX_PRO_BUF;
					pBuf[wLen-2] = lib_CheckSum(&pBuf[0], wLen-2); // 重新计算帧校验

					return TRUE; // 需要透传给本体
				}
				return FALSE;
			}
		}
		else
		{
			//地址不一致时，如果在搜表列表中，直接转发
			// if ((sourceCh == ePT_HPLC) || (sourceCh == ePT_4851) || (sourceCh == ePT_BLUETOOTH))
			// {				
			// 	if ( IsExistMeter(pFm->MAddr, NULL) )
			// 	{
			// 		*pDestCh = ePT_4852;
			// 		return TRUE;
			// 	}
			// }
		}
	}
	else if(PRO_DLT698_45 == byGy)
	{

	}
	return FALSE;
}

//-----------------------------------------------
//函数功能: 检查端口是否处于级联状态
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   
//-----------------------------------------------
BOOL IsJLing(BYTE Ch, BYTE *pGyType,BYTE *pSourceCh)
{
	BYTE chNo = (Ch == ePT_UART1_F415) ? 0 : 1;
	
	if(JLTxBuf[chNo].byValid != JLS_WORK)
		return FALSE;
	if(JLTxBuf[chNo].byPortFlag & (1<<Ch))
		return FALSE;
	JLTxBuf[chNo].byValid = JLS_COMPLETE;
	JLTxBuf[chNo].wWptr = 0;
	JLTxBuf[chNo].wRptr = 0;
	*pGyType   = JLTxBuf[chNo].byGyType;
	*pSourceCh = JLTxBuf[chNo].bySourceNo;
	return TRUE;
}


//-----------------------------------------------
//函数功能: 检查是否有从载波，维护口发来的报文,有则优先发送出去
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   
//-----------------------------------------------
BOOL CheckTxJL(BYTE Ch)
{
	BYTE chNo = (Ch == ePT_UART1_F415) ? 0 : 1;
	chNo = 0;
	
    if(JLTxBuf[chNo].wWptr >= MAX_PRO_BUF)
	{
		JLTxBuf[chNo].byValid = JLS_IDLE;
		return FALSE;
	}
	if(JLTxBuf[chNo].byPortFlag == 0)
	{
        if(JLTxBuf[chNo].byValid == JLS_WORK)
		{//1上次透抄失败
            
		}
		JLTxBuf[chNo].byValid = JLS_IDLE;
	}
    else 
	{
		if(JLTxBuf[chNo].byPortFlag & (1<<Ch))
		{
			JLTxBuf[chNo].byPortFlag &= ~(1<<Ch);
			if(JLTxBuf[chNo].byValid != JLS_COMPLETE)
				JLTxBuf[chNo].byValid = JLS_WORK;
		}
	}	
	switch(JLTxBuf[chNo].byValid)
	{
	case JLS_IDLE: 
		JLTxBuf[chNo].byBaudSelect = 0;
		return FALSE;
	case JLS_READY: //不应到此
		JLTxBuf[chNo].byValid      = JLS_IDLE;
		return FALSE;
	case JLS_COMPLETE:  
		JLTxBuf[chNo].byValid      = JLS_IDLE;
		return FALSE;
	case JLS_WORK:  
		break;
	}

	//拷贝串口数据
	memcpy( (Serial[Ch].ProBuf),JLTxBuf[chNo].TransBuf,JLTxBuf[chNo].wWptr);


	Serial[Ch].SendLength = (JLTxBuf[chNo].wWptr);
    Serial[Ch].TXPoint    = 0;
	
	Serial[Ch].RXWPoint = 0;
	Serial[Ch].RXRPoint = 0;
	//执行延时
	//Serial[Ch].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;
	SerialMap[Ch].SCIDisableRcv(SerialMap[Ch].SCI_Ph_Num);
	SerialMap[Ch].SCIEnableSend(SerialMap[Ch].SCI_Ph_Num);
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 给F415的读报文 组帧
//
//参数:	
//		pAddr[in]: 要发送的地址
//返回值:
//	无
//
//备注:
//-----------------------------------------------
void Tx_F415_Read(BYTE SerialNo,BYTE *pAddr, DWORD dwID)
{
	TSerial *p = &Serial[SerialNo];
	
	p->TXPoint = 0;

	p->SendLength = Tx_Fm64507_Read(pAddr,dwID,&p->ProBuf[4]);

	memset(p->ProBuf,0xFE,4);
	p->SendLength += 4;
	
	api_CommuTx( SerialNo );
}

//-----------------------------------------------
//函数功能: 给F415的写报文 组帧
//
//参数:	
//		pAddr[in]: 要发送的地址
//返回值:
//	无
//
//备注:
//-----------------------------------------------
void Tx_F415_Write(BYTE SerialNo,BYTE *pAddr,BYTE byCtrl, DWORD dwID,BYTE *pData,BYTE dataLen)
{
	TSerial *p = &Serial[SerialNo];
	
	p->TXPoint = 0;

	p->SendLength = Tx_Fm64507_Set(pAddr,byCtrl,dwID,pData,dataLen,&p->ProBuf[4]);

	memset(p->ProBuf,0xFE,4);
	p->SendLength += 4;
	
	api_CommuTx( SerialNo );
}
//--------------------------------------------------
//功能描述:  设置电表的采集标志位 (tCJCtl.m_CjFlag)
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void SetCJFlag_Min(void)
{
	DWORD dwTime;
	BYTE i;
	
    if(SKMeter.byAutoCheck)
	{
		return;
	}
    if (m_CJInterv == 0)
		m_CJInterv = 15;
    dwTime = RealTimer.Day*1440 + RealTimer.Hour*24 + RealTimer.Min;
	dwTime = (dwTime/m_CJInterv)*m_CJInterv;
	if (dwTime != tCJCtl.m_OldCjTime)
	{
		for (i = 0; i<SEARCH_METER_MAX_NUM; i++)
		{
			if ( IsMeterValid(i) )
			{
				tCJCtl.m_CjFlag[i] |= (1<<MSF_PA);
				tCJCtl.m_CjFlag[i] |= (1<<MSF_AP);
			}
		}
		tCJCtl.m_OldCjTime = dwTime;
	}
	// Event_Daemon();
}

void ClearSFlag(BYTE FLagNo)
{
    if (tCJCtl.m_CjNo < SEARCH_METER_MAX_NUM)
	{
		tCJCtl.m_CjFlag[tCJCtl.m_CjNo] &= ~(1<<FLagNo);
		tCJCtl.m_RetryNum[tCJCtl.m_CjNo] = 0;
	}
}
//--------------------------------------------------
//功能描述:  清除标志
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void ClearSFlag_m(BYTE Ch,BYTE FLagNo)
{
	m_CJFlag[Ch] &= ~(1<<FLagNo);
}
//--------------------------------------------------
//功能描述:  置标志
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void SetCJFlag_m(BYTE Ch,BYTE Flag)
{
	m_CJFlag[Ch] |= (1<<Flag);
}
//--------------------------------------------------
//功能描述:  获取标志
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE  GetCJFlag_m(BYTE Ch, BYTE Flag )
{
	return m_CJFlag[Ch] & (1<<Flag);
}
//-----------------------------------------------
//函数功能: 获取时间和温度
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_GetTimeTemp(void)
{
	TTimeTem TF415Temp;
	TRealTimer t;

	// TF415Temp.Tem = api_GetTemp();
	// 暂时用模拟温度值代替
	TF415Temp.Tem = 15;
	if(TF415TimeTem.Tem != TF415Temp.Tem)
	{
		memcpy((BYTE*)&TF415TimeTem.Tem,(BYTE*)&TF415Temp.Tem,sizeof(SWORD));
	}
	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&TF415Temp.Time); // 获取当前时间
	memcpy((BYTE*)&TF415TimeTem.Time,(BYTE*)&TF415Temp.Time,sizeof(TRealTimer));
}
//-----------------------------------------------
//函数功能: 端口主动采集
//
//参数: 	通道号
//			
//返回值:  	BOOL
//
//备注:   
//-----------------------------------------------
BOOL Data_Txd(BYTE SerialNo)
{
	TSchedMeterInfo metInfo;
	BYTE Addr[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	DWORD dwID=0;
	BYTE byCtrl,pData[50],dataLen;
	TRealTimer t;

	if ( SerialNo == ePT_UART1_F415 )//与F415通信
	{
		//抄读负荷辨识结果，3秒抄读一次
		if(GetCJFlag_m(SerialNo-1,MSF_IDENTIFY_PARA))
		{
			ClearSFlag_m(SerialNo-1,MSF_IDENTIFY_PARA);
			dwID =  REQ_RES; //DI_INFOs里面的 负荷辨识结果 数据标识
			Tx_F415_Read(SerialNo,&Addr[0], dwID);
			TSendRec[SerialNo-1].Sec_Send_Success++;
			return TRUE;
		}

		// //高频率任务往后放，否则其他抄读任务得不到响应

		// 设置电压电流系数、温度、时间，1分钟设置一次
		// 为了减少对415时间的干扰，415在此处只接收电压电流系数，不处理温度与时间 lsc
		if(GetCJFlag_m(SerialNo-1,MSF_VOL_CUR_PARA))
		{
			ClearSFlag_m(SerialNo-1,MSF_VOL_CUR_PARA);
			byCtrl = 0x14;
			dwID =  REQ_UIK; //6025传递（写）F415电压电流系数的数据标识
			memcpy((BYTE*)&pData[0],(BYTE*)&TF415UIK,sizeof(TSampleUIK)-sizeof(DWORD));
			api_GetTimeTemp();//获取温度和时间
			memcpy((BYTE*)&pData[sizeof(TSampleUIK)-sizeof(DWORD)],(BYTE*)&TF415TimeTem,sizeof(TTimeTem));//将时间和温度设置给F415
			dataLen = sizeof(TSampleUIK)+sizeof(TTimeTem)-sizeof(DWORD);
			Tx_F415_Write(SerialNo,&Addr[0], byCtrl,dwID,(BYTE*)&pData[0],dataLen);
			TSendRec[SerialNo-1].Min_Send_Success++;
			return TRUE;
		}
		// 415算法调试，修改为6小时设置一次，目的与物联表校时模组的机制一致；
		if(GetCJFlag_m(SerialNo-1,MSF_TIME_PARA))
		{
			ClearSFlag_m(SerialNo-1,MSF_TIME_PARA);
			byCtrl = 0x14;
			dwID = REQ_TIME;
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t); // 获取当前时间
			dataLen = sizeof(TRealTimer);
			Tx_F415_Write(SerialNo,&Addr[0], byCtrl,dwID,(BYTE*)&t,dataLen);
			TSendRec[SerialNo-1].Min_Send_Success++;
			return TRUE;
		}
	}
	
	return FALSE;
}
//--------------------------------------------------
//功能描述:  处理6025接收的电压电流系数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  RX_UIK(BYTE Ch, BYTE* pBuf )
{
	TSendRec[Ch].Min_Rec_Success++;
}
//--------------------------------------------------
//功能描述:  规约读取第一二回路的负荷辨识结果
//         
//参数:      输入：i	区分第一二回路
//
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_ReadIdentResult(BYTE i, BYTE* Buf )
{
	BYTE j;

	for(j=0;j<IDF_RES_NUM;j++)
	{
		Buf[j*sizeof(TElectIdentify)] = ElectIdentify[i][j].ucMessageID;
		Buf[j*sizeof(TElectIdentify)+1] = ElectIdentify[i][j].ucCode;
		Buf[j*sizeof(TElectIdentify)+2] = ElectIdentify[i][j].ucdevNum;
		lib_ExchangeData((BYTE*)&Buf[j*sizeof(TElectIdentify)+3],(BYTE*)&ElectIdentify[i][j].ucPower,sizeof(SWORD));
	}
}
//--------------------------------------------------
//功能描述:  处理6025接收的负荷辨识结果
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL RX_RES(BYTE Ch, BYTE* pBuf , BYTE* judge)
{
	memset((BYTE *)&ElectIdentify, 0x00, sizeof(ElectIdentify) );
	TSendRec[Ch].Sec_Rec_Success++;
	if(judge[0] < sizeof(DWORD))
	{
        return FALSE;
	}
	if(memcmp((BYTE*)&MessageData.ucTime,pBuf,sizeof(MessageData)) != 0)
	{
		memcpy((BYTE*)&MessageData.ucTime,pBuf,sizeof(MessageData));
	}
	
	return TRUE;
}
//-----------------------------------------------
//函数功能: 规约接收处理
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   
//-----------------------------------------------
BOOL GyRxMonitor(BYTE Ch,BYTE GyType)
{
	TSerial *p = &Serial[Ch];
	T645FmRx *pRxFm=(T645FmRx *)p->ProBuf;
	BOOL bRc = TRUE;
	BYTE byI,*pBuf;
	
	if (GyType == PRO_NO_SPECIAL)
	{
		if (pRxFm->dwDI == 0x3712120E) //04DFDFDB  刘骞解析
			return FALSE;
		
		if (pRxFm->dwDI == 0x37333435) //测试第二路485 刘骞回复
			return FALSE;

		if(pRxFm->byCtrl == 0x91)//抄读成功处理
		{
			pBuf = (BYTE*)&pRxFm->byDataLen;
			pBuf ++;
			for (byI =0; byI<pRxFm->byDataLen; byI++)
			{
				pBuf[byI] -= 0x33;  
			}
			switch(pRxFm->dwDI)
			{
				case REQ_RES:
					RX_RES(Ch-1,p->ProBuf+14, p->ProBuf+9);
				break;
			}
		}
		else if (pRxFm->byCtrl == 0xD1)//抄读失败处理
		{
			TSendRec[Ch-1].Sec_Rec_Failure++;
		}
		else if(pRxFm->byCtrl == 0x94)//设置成功处理
		{
			RX_UIK(Ch-1, p->ProBuf );
		}
		else if (pRxFm->byCtrl == 0xD4)//设置失败处理
		{
			TSendRec[Ch-1].Min_Rec_Failure++;
		}
	}
	else if (GyType == PRO_DLT698_45)
	{
		
	}
	return TRUE;
}
//-----------------------------------------------
//函数功能: 发送延时控制
//
//参数:		无 
//                 
//返回值: 	BOOL
//		   
//备注:   返回TRUE，代表可以进行发送数据了
//-----------------------------------------------
BOOL MsgCtrl(BYTE Ch)
{
	if(Ch >= MAX_COM_CHANNEL_NUM)
		return FALSE;
	
    //底层没发送完
	if(Serial[Ch].TXPoint < Serial[Ch].SendLength)
		return FALSE;
	
	if(Serial[Ch].SendToSendDelay != 0)
		return FALSE;

	return TRUE; 
}
//-----------------------------------------------
//函数功能: 第一回路自研算法结合判断时间控制函数
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
static void CheckTripFlagValid(void)
{
//	//如果需要结合判断，目前仅第一回路有用
//	if (MalignantLoadPara.AlgorithmCombineSwitch[RELAY1] == 0x00)			//00表示结合“&&”
//	{
//		if (Harmonic.ReasonFlag == 0xA5)
//		{
//			Harmonic.DurationTime++;
//			if (Harmonic.DurationTime >= 5)
//			{
//				Harmonic.DurationTime = 0;
//				Harmonic.ReasonFlag = 0x5A;
//			}
//		}
//
//		if (Svm.ReasonFlag == 0xA5)
//		{
//			Svm.DurationTime++;
//			if (Svm.DurationTime >= 5)
//			{
//				Svm.DurationTime = 0;
//				Svm.ReasonFlag = 0x5A;
//			}
//		}
//	}
}
//-----------------------------------------------
//函数功能: 采集大循环任务
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_MCollectTask(void)
{	
	static BYTE pollCount = 0;
	BYTE Ch;
	TRealTimer t;

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t); // 获取当前时间
	Ch = ePT_UART1_F415;
	pollCount++;
	
	if( api_GetTaskFlag(eTASK_COLLECT_ID, eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_COLLECT_ID, eFLAG_SECOND);
		// SecLoopCJFlagRetry();			//重试控制
		if (IsUpdateF415 == TRUE)
		{
			if (Collection_Recovery_Time > 0)
			{
				Collection_Recovery_Time--;	
			}
			else if (Collection_Recovery_Time == 0)
			{
				//时间到，认为升级任务完成，恢复采集任务
				IsUpdateF415 = FALSE;
			}
			else
			{
				return ;
			}				
		}
		else
		{
			if (t.Sec == 5)	//每分钟第5秒置一次
			{
				SetCJFlag_m(0,MSF_VOL_CUR_PARA);
			}
			if ((t.Sec % 3) == 0) //3秒置一次
			{
				SetCJFlag_m(0,MSF_IDENTIFY_PARA);
			}
		}
		//检查判断第一回路自研算法结合判断原因标志的有效性
		CheckTripFlagValid();
	}

	//分钟任务
	if( api_GetTaskFlag(eTASK_COLLECT_ID, eFLAG_MINUTE) == TRUE )
	{
		api_ClrTaskFlag(eTASK_COLLECT_ID, eFLAG_MINUTE);
		if((t.Hour%CALIBRATION_CYCLE_FOR_415) == 0)
		{
			if ((t.Min==0)&&(t.Sec==0))
			{
				SetCJFlag_m(0,MSF_TIME_PARA);
			}
		}
	}

	//小时任务
	if( api_GetTaskFlag(eTASK_COLLECT_ID, eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_COLLECT_ID, eFLAG_HOUR);
	}

	if(!MsgCtrl(Ch))
	{
		return;
	}
	//1、优先处理透传
    if(CheckTxJL(Ch))
	{
		Serial[Ch].SendToSendDelay = MAX_SEND_DELAY;
		return;
	}	
//	//2、搜表
//	if (Ch == ePT_4852)
//	{
//		// if(api_ProSearchMeter(Ch))
//		// {
//		// 	Serial[Ch].SendToSendDelay = MAX_SEND_DELAY;
//		// 	return;
//		// }
//	}

	//3、采集任务的报文监视
	if((Ch == ePT_UART1_F415) || (Ch == ePT_UART2_F415))
	{
		if(IsUpdateF415 == FALSE)
		{
			if((TSendRec[Ch-1].Sec_Send_Success - TSendRec[Ch-1].Sec_Rec_Success) > F415_REC_FAILED_TIMES)
			{
				memset(&TSendRec[Ch-1].Sec_Send_Success,0,sizeof(TCollectSendRec));	//清零
				//记录异常事件
				api_WriteSysUNMsg( GD32F415_PROTOCOL_ERR );
				// 重启F415电源
				if(ControlResetStatus == 0)
				{
					POWER_F415_CV_CLOSE;
					api_Delayms(1000);
					POWER_F415_CV_OPEN;
				}
			}
		}
		//采集测试计数的清零
		if(TSendRec[Ch-1].Sec_Send_Success > COUNTUPPERLIMIT)
		{
			memset(&TSendRec[Ch-1].Sec_Send_Success,0x00,sizeof(TCollectSendRec));
		}
	}

	//3、采集
	if(IsUpdateF415 == FALSE)
	{
		if (Data_Txd(Ch))
		{
			Serial[Ch].SendToSendDelay = MAX_SEND_DELAY;
			return;
		}
	}
}

//--------------------------------------------------
//功能描述: 串口触发一次发送
//
//参数:	Serial[in]: 需要发送的串口号
//		Buf[in]: 需要发送的数据
//		Size[in]: 数据尺寸
//返回值:无
//
//备注:
//--------------------------------------------------
static void SerialTriggerOneSend(BYTE SerialNo, BYTE *Buf, WORD Size)
{
	TSerial *p = &Serial[SerialNo];

	if((Size > (MAX_PRO_BUF - 10)) || ( Size == 0 ))
	{
		return;
	}

	memset(p->TxProBuf, 0xFE, 4);
	memcpy(p->TxProBuf + 4, Buf, Size);
	p->TXPoint = 0;
	p->SendLength = 4 + Size;
	
	api_CommuTx( SerialNo );
}

//--------------------------------------------------
//功能描述: 上电采集初始化
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void api_PowerUpCollect(void)
{
	// api_InitI2C();
	// PowerUpUnitParaInit();

	//读过流参数以及控制字
	//m_CJFlag |= (1U << MSF_OVERI_PARA_I);
	//m_CJFlag |= (1U << MSF_OVERI_PARA_II);
	//m_CJFlag |= (1U << MSF_OVERI_PARA_III);
	//m_CJFlag |= (1U << MSF_OVERI_PARA_N);
	//m_CJFlag |= (1U << MSF_CTRL_WORD);

	// SetCJFlagRetry(MSF_UNIT_PARA, 3, 20);
	// m_CJFlag |= (1U << MSF_IDENTIFY_PARA);
	//上电同步415电压电流系数
	SetCJFlag_m(0,MSF_VOL_CUR_PARA);
	//上电同步415时间
	SetCJFlag_m(0,MSF_TIME_PARA);
	//上电辨识结果内容初始化
	memset((BYTE*)&MessageData.ucTime,0xFF,sizeof(MessageData));
}

//--------------------------------------------------
//功能描述: 得到控制块中的 FLAG索引
//
//参数:	Flagp[in]: 索引位置
//
//返回值: 位置
//
//备注:
//--------------------------------------------------
static BYTE GetCJFlagIndex(BYTE Flag)
{
	BYTE i;

	for (i = 0; i < CJFALG_RETRY_COUN; i++)
	{
		if(s_CJFlagRetryCtrl.Valid[i/8] & (1U << (i%8) ))
		{
			if(s_CJFlagRetryCtrl.Flag[i] == Flag)
			{
				return i;
			}
		}
	}

	return 0xFF;
}
//--------------------------------------------------
//功能描述: 得到控制块中的 无效的位置
//
//参数:
//
//返回值: 无效位置
//
//备注:
//--------------------------------------------------
static BYTE GetCJFlagInvaildIndex(void)
{
	for (BYTE i = 0; i < CJFALG_RETRY_COUN; i++)
	{
		if(!(s_CJFlagRetryCtrl.Valid[i/8] & (1U << (i%8) )))
		{
			return i;
		}
	}

	return 0xFF;
}

//--------------------------------------------------
//功能描述: 清掉重试标志
//
//参数: Flag[in]: 标志
//
//返回值:
//
//备注:
//--------------------------------------------------
void ClrCJFlagRetry(BYTE Flag)
{
	BYTE Index = GetCJFlagIndex(Flag);

	if( Index != 0xFF)
	{
		s_CJFlagRetryCtrl.Valid[Index / 8] &= ~(1U << (Index % 8));
	}
}
//--------------------------------------------------
//功能描述: 设置 发送标志
//
//参数:	Flag[in] : 标志
//		RetryNump[in]: 重试次数
//		Sec[in]: 间隔时间
//返回值:
//
//备注:
//--------------------------------------------------
void SetCJFlagRetry(BYTE Ch, BYTE Flag, BYTE RetryNum, BYTE Sec)
{
	BYTE Index;

	//1、无论是否有空闲控制块，都保留发送的权利，只是不重试
	m_CJFlag[Ch] |= (1U << Flag);

	//2、程序中有这个标志，则不再重复增加
	if( GetCJFlagIndex(Flag) == 0xFF)
	{
		Index = GetCJFlagInvaildIndex();
		if(Index != 0xFF)
		{
			s_CJFlagRetryCtrl.Flag[Index] = Flag;
			s_CJFlagRetryCtrl.RetryNum[Index] = RetryNum;
			s_CJFlagRetryCtrl.Timer[Index] = Sec;
			s_CJFlagRetryCtrl.TimerConst[Index] = Sec;
			s_CJFlagRetryCtrl.Valid[Index / 8] |= (1U << (Index % 8));
		}
	}
}

//--------------------------------------------------
//功能描述: 控制块秒循环任务
//
//参数:	无
//
//返回值:
//
//备注:
//--------------------------------------------------
static void SecLoopCJFlagRetry(BYTE Ch)
{
	for (BYTE i = 0; i < CJFALG_RETRY_COUN; i++)
	{
		if(s_CJFlagRetryCtrl.Valid[i/8] & (1U << (i%8) ))
		{
			//1、定时器不为0，则递减
			if(s_CJFlagRetryCtrl.Timer[i] != 0)
			{
				s_CJFlagRetryCtrl.Timer[i]--;
			}
			//2、定时器为0，且次数为0，则不再重试
			else if(s_CJFlagRetryCtrl.RetryNum[i] == 0)
			{
				s_CJFlagRetryCtrl.Valid[i / 8] &= ~(1U << (i % 8));
			}
			else //3、定时器为0，次数不为0，则重试
			{
				s_CJFlagRetryCtrl.RetryNum[i]--;
				s_CJFlagRetryCtrl.Timer[i] = s_CJFlagRetryCtrl.TimerConst[i];

				m_CJFlag[Ch] |= (1U << s_CJFlagRetryCtrl.Flag[i]);
			}
		}	
	}
}
//--------------------------------------------------
//功能描述: 6025_to_415采集任务状态的读取
//
//参数:			0:采集状态读取
//				1:采集状态设置
//
//返回值:		BYTE
//
//备注:415升级时，采集任务关闭；415执行升级时，采集任务打开
//			IsUpdateF415		采集任务状态
//				FALSE  			  1（开启)
//				TRUE  			  0（关闭)
//--------------------------------------------------
BYTE api_GetCollectionTaskStatus415(void)
{
	BYTE Status;

	if (IsUpdateF415 == FALSE)
	{
		Status = 1;
	}
	else
	{
		Status = 0;
	}
	
	return Status;
}
//--------------------------------------------------
//功能描述: 6025_to_415采集任务状态的设置
//
//参数:		TYPE : 0————采集状态关闭----415升级任务允许执行
//			TYPE : 1————采集状态开启----415升级任务拒绝执行
//
//返回值:		BYTE
//
//备注:			
//一般只给予采集任务开启的权限，采集关闭权限作用时长取决于Collection_Recovery_For_415
//--------------------------------------------------
void api_SetCollectionTaskStatus415(BYTE TYPE)
{
	switch (TYPE)
	{
		case 0:
			IsUpdateF415 = TRUE;
			Collection_Recovery_Time = COLLECTION_RECOVERY_FOR_415;
			break;
		case 1:
			IsUpdateF415 = FALSE;
			break;
		default:
			break;
	}
}
#endif//#if(SEL_F415 == YES)