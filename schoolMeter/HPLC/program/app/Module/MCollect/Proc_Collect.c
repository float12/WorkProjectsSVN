//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	lq 
//��������	
//����		
//�޸ļ�¼  ����23�����⿪�أ�ɾ���뱾��ͨ�Ų��֣�ֻ������������GDF415����
//----------------------------------------------------------------------
#include "AllHead.h"
#include "MCollect.h"
#include "Dlt698_45.h"

#if(SEL_F415 == YES)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define CJFALG_RETRY_COUN				4				//CJFALG ���Կ��ƿ�֧�ָ���
#define F415_POWER_RESET 				YES 			//�Ƿ�����F415��Դ�������ܣ����ú���������쳣�����󣬻ᵼ�·��������F415ʱ�򣬳��ֵ��������������¼���ϳ��������
#define F415_REC_FAILED_TIMES			20				//����ʧ�ܴ���
#define COLLECTION_RECOVERY_FOR_415		30				//415�������²ɼ���ͣ����Ҫ�Ļָ�ʱ��30s
#define CALIBRATION_CYCLE_FOR_415		6				//6025��ʱ��415Уʱ��ʱ�����ڣ���Сʱ��
#define COUNTUPPERLIMIT					60000			//��λ6025��415ͨ�Ų��Եļ�������
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
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
//				ȫ��ʹ�õı���������
//-----------------------------------------------
DWORD m_CJFlag[2] = {0,0};   								//����ɼ���ʶ
TElectIdentify ElectIdentify[IDF_LOOP_NUM][IDF_RES_NUM];	//������F415���ɱ�ʶ���
extern TTimeTem	TF415TimeTem;
BOOL IsUpdateF415 = FALSE; 									//����415�ı�־
WORD Collection_Recovery_Time;								//�ɼ��ָ�ʱ�䶨ʱ��
BYTE Address_645[6];										//415�ش��ĵ�ַ��ͬ6025ͨ�ŵ�ַһ��
AMessage_T MessageData;										//415���ص�ʶ����
//TSendRecĿǰ������ֻ�Ǽ����·uart
TCollectSendRec TSendRec[2] = 
{
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
};
BYTE ControlResetStatus;									//�Ƿ�����415��λ��0��Ĭ������1��������415��λ	

TTriggerViolationSign Harmonic;
TTriggerViolationSign Svm;
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
TJLTxBuf JLTxBuf[2]; 										//0����Ӧ485����1����Ӧ����)
static BYTE m_CJInterv = 15; 								//�ɼ����
static TMCJCtl  tCJCtl = {0};

static TCJFlagRetryCtrl s_CJFlagRetryCtrl;					//CJFlag����
//-----------------------------------------------
//				�ڲ���������
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
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: �������˿�д����
//
//����: 	��
//			
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void JLWrite(BYTE destCh,BYTE *hBuf,DWORD dwLen,BYTE byGyType,BYTE sourceCh)
{
	BYTE chNo = (destCh == ePT_UART1_F415) ? 0 : 1;
	
	if(JLTxBuf[chNo].byValid != JLS_IDLE)
	{
		if(JLTxBuf[chNo].byValid == JLS_COMPLETE)
		{//1��ʱ��͸���ɹ�����δ�ó�JLS_IDLE
			JLTxBuf[chNo].byValid = JLS_IDLE;
			JLTxBuf[chNo].byBaudSelect = 0;
		}
		else
		{//2��Ķ˿��ڼ���
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
	
	//��4��fe����֡
	memset(JLTxBuf[chNo].TransBuf,0xFE,4);
	memcpy(&JLTxBuf[chNo].TransBuf[4],hBuf,dwLen);
}

//-----------------------------------------------
//��������: ��������ж˿��յ��ı��ģ��Ƿ���Ҫת������
//
//����: 	sourchCh-Դ�˿ڣ�pDestCh-Ŀ��˿�
//			
//����ֵ:  	BOOL
//
//��ע:   
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
		
		// ��GDF415�����ݣ���ַĬ�Ͼ��Ǳ��ַ���жϲ�ͬ�����ݱ�ʶ����ת��
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
				if ((pFm->byCtrl == 0x11)		//������
				||(pFm->byCtrl == 0x14)			// д����
				||(pFm->byCtrl == 0x1C)			// ����
				||(pFm->byCtrl == 0x1D))		// ����
				{
					if((dwDI == 0x04000F02) && (pFm->byCtrl == 0x14))//������F415���õ�ѹ����ϵ��
					{
						return FALSE;
					}
					if(pFm->byCtrl == 0x1D)
					{
						tPassWord.b[0] = pBuf[14] - 0x33;
						tPassWord.b[1] = pBuf[15] - 0x33;
						tPassWord.b[2] = pBuf[16] - 0x33;
						tPassWord.b[3] = pBuf[17] - 0x33;

						//���������������������ж������Ƿ�Ϸ������������������ο�CheckPassword()����
						if((dwDI == 0x0f0f0f04)||(dwDI == 0x0f0f0f06))
						{
							//��EEPROM��ȡ�������� ���EEPROM���ݴ��� �������� ��ֹEEPROM�������δ��ʼ���޷��򿪳���ģʽ
							if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)Password.Password645) != TRUE )
							{
								return FALSE;
							}
							//�ж��Ƿ�Ϊ02������&&���ڹ�Լ�������У԰��ڶ�·485������
							if((gPr645[sourceCh].eProtocolType == ePROTOCOL_DLT645_FACTORY)&&(tPassWord.b[0] == MINGWEN_H_PASSWORD_LEVEL) )
							{
								//����Ա�
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
								if((dwDI == 0x0f0f0f04)||(dwDI == 0x0f0f0f05)||(dwDI == 0x0f0f0f06))//415����-----��������,�м�֡���ͣ����������������
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
					pBuf[wLen-2] = lib_CheckSum(&pBuf[0], wLen-2); // ���¼���֡У��

					return TRUE; // ��Ҫ͸��������
				}
				return FALSE;
			}
		}
		else
		{
			//��ַ��һ��ʱ��������ѱ��б��У�ֱ��ת��
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
//��������: ���˿��Ƿ��ڼ���״̬
//
//����: 	��
//			
//����ֵ:  	BOOL
//
//��ע:   
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
		{//1�ϴ�͸��ʧ��
            
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
	case JLS_READY: //��Ӧ����
		JLTxBuf[chNo].byValid      = JLS_IDLE;
		return FALSE;
	case JLS_COMPLETE:  
		JLTxBuf[chNo].byValid      = JLS_IDLE;
		return FALSE;
	case JLS_WORK:  
		break;
	}

	//������������
	memcpy( (Serial[Ch].ProBuf),JLTxBuf[chNo].TransBuf,JLTxBuf[chNo].wWptr);


	Serial[Ch].SendLength = (JLTxBuf[chNo].wWptr);
    Serial[Ch].TXPoint    = 0;
	
	Serial[Ch].RXWPoint = 0;
	Serial[Ch].RXRPoint = 0;
	//ִ����ʱ
	//Serial[Ch].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;
	SerialMap[Ch].SCIDisableRcv(SerialMap[Ch].SCI_Ph_Num);
	SerialMap[Ch].SCIEnableSend(SerialMap[Ch].SCI_Ph_Num);
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��F415�Ķ����� ��֡
//
//����:	
//		pAddr[in]: Ҫ���͵ĵ�ַ
//����ֵ:
//	��
//
//��ע:
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
//��������: ��F415��д���� ��֡
//
//����:	
//		pAddr[in]: Ҫ���͵ĵ�ַ
//����ֵ:
//	��
//
//��ע:
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
//��������:  ���õ��Ĳɼ���־λ (tCJCtl.m_CjFlag)
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
//��������:  �����־
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void ClearSFlag_m(BYTE Ch,BYTE FLagNo)
{
	m_CJFlag[Ch] &= ~(1<<FLagNo);
}
//--------------------------------------------------
//��������:  �ñ�־
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void SetCJFlag_m(BYTE Ch,BYTE Flag)
{
	m_CJFlag[Ch] |= (1<<Flag);
}
//--------------------------------------------------
//��������:  ��ȡ��־
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BYTE  GetCJFlag_m(BYTE Ch, BYTE Flag )
{
	return m_CJFlag[Ch] & (1<<Flag);
}
//-----------------------------------------------
//��������: ��ȡʱ����¶�
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_GetTimeTemp(void)
{
	TTimeTem TF415Temp;
	TRealTimer t;

	// TF415Temp.Tem = api_GetTemp();
	// ��ʱ��ģ���¶�ֵ����
	TF415Temp.Tem = 15;
	if(TF415TimeTem.Tem != TF415Temp.Tem)
	{
		memcpy((BYTE*)&TF415TimeTem.Tem,(BYTE*)&TF415Temp.Tem,sizeof(SWORD));
	}
	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&TF415Temp.Time); // ��ȡ��ǰʱ��
	memcpy((BYTE*)&TF415TimeTem.Time,(BYTE*)&TF415Temp.Time,sizeof(TRealTimer));
}
//-----------------------------------------------
//��������: �˿������ɼ�
//
//����: 	ͨ����
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL Data_Txd(BYTE SerialNo)
{
	TSchedMeterInfo metInfo;
	BYTE Addr[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	DWORD dwID=0;
	BYTE byCtrl,pData[50],dataLen;
	TRealTimer t;

	if ( SerialNo == ePT_UART1_F415 )//��F415ͨ��
	{
		//�������ɱ�ʶ�����3�볭��һ��
		if(GetCJFlag_m(SerialNo-1,MSF_IDENTIFY_PARA))
		{
			ClearSFlag_m(SerialNo-1,MSF_IDENTIFY_PARA);
			dwID =  REQ_RES; //DI_INFOs����� ���ɱ�ʶ��� ���ݱ�ʶ
			Tx_F415_Read(SerialNo,&Addr[0], dwID);
			TSendRec[SerialNo-1].Sec_Send_Success++;
			return TRUE;
		}

		// //��Ƶ����������ţ�����������������ò�����Ӧ

		// ���õ�ѹ����ϵ�����¶ȡ�ʱ�䣬1��������һ��
		// Ϊ�˼��ٶ�415ʱ��ĸ��ţ�415�ڴ˴�ֻ���յ�ѹ����ϵ�����������¶���ʱ�� lsc
		if(GetCJFlag_m(SerialNo-1,MSF_VOL_CUR_PARA))
		{
			ClearSFlag_m(SerialNo-1,MSF_VOL_CUR_PARA);
			byCtrl = 0x14;
			dwID =  REQ_UIK; //6025���ݣ�д��F415��ѹ����ϵ�������ݱ�ʶ
			memcpy((BYTE*)&pData[0],(BYTE*)&TF415UIK,sizeof(TSampleUIK)-sizeof(DWORD));
			api_GetTimeTemp();//��ȡ�¶Ⱥ�ʱ��
			memcpy((BYTE*)&pData[sizeof(TSampleUIK)-sizeof(DWORD)],(BYTE*)&TF415TimeTem,sizeof(TTimeTem));//��ʱ����¶����ø�F415
			dataLen = sizeof(TSampleUIK)+sizeof(TTimeTem)-sizeof(DWORD);
			Tx_F415_Write(SerialNo,&Addr[0], byCtrl,dwID,(BYTE*)&pData[0],dataLen);
			TSendRec[SerialNo-1].Min_Send_Success++;
			return TRUE;
		}
		// 415�㷨���ԣ��޸�Ϊ6Сʱ����һ�Σ�Ŀ����������Уʱģ��Ļ���һ�£�
		if(GetCJFlag_m(SerialNo-1,MSF_TIME_PARA))
		{
			ClearSFlag_m(SerialNo-1,MSF_TIME_PARA);
			byCtrl = 0x14;
			dwID = REQ_TIME;
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t); // ��ȡ��ǰʱ��
			dataLen = sizeof(TRealTimer);
			Tx_F415_Write(SerialNo,&Addr[0], byCtrl,dwID,(BYTE*)&t,dataLen);
			TSendRec[SerialNo-1].Min_Send_Success++;
			return TRUE;
		}
	}
	
	return FALSE;
}
//--------------------------------------------------
//��������:  ����6025���յĵ�ѹ����ϵ��
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  RX_UIK(BYTE Ch, BYTE* pBuf )
{
	TSendRec[Ch].Min_Rec_Success++;
}
//--------------------------------------------------
//��������:  ��Լ��ȡ��һ����·�ĸ��ɱ�ʶ���
//         
//����:      ���룺i	���ֵ�һ����·
//
//����ֵ:    
//         
//��ע:  
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
//��������:  ����6025���յĸ��ɱ�ʶ���
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
//��������: ��Լ���մ���
//
//����: 	��
//			
//����ֵ:  	BOOL
//
//��ע:   
//-----------------------------------------------
BOOL GyRxMonitor(BYTE Ch,BYTE GyType)
{
	TSerial *p = &Serial[Ch];
	T645FmRx *pRxFm=(T645FmRx *)p->ProBuf;
	BOOL bRc = TRUE;
	BYTE byI,*pBuf;
	
	if (GyType == PRO_NO_SPECIAL)
	{
		if (pRxFm->dwDI == 0x3712120E) //04DFDFDB  ��幽���
			return FALSE;
		
		if (pRxFm->dwDI == 0x37333435) //���Եڶ�·485 ��幻ظ�
			return FALSE;

		if(pRxFm->byCtrl == 0x91)//�����ɹ�����
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
		else if (pRxFm->byCtrl == 0xD1)//����ʧ�ܴ���
		{
			TSendRec[Ch-1].Sec_Rec_Failure++;
		}
		else if(pRxFm->byCtrl == 0x94)//���óɹ�����
		{
			RX_UIK(Ch-1, p->ProBuf );
		}
		else if (pRxFm->byCtrl == 0xD4)//����ʧ�ܴ���
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
	if(Ch >= MAX_COM_CHANNEL_NUM)
		return FALSE;
	
    //�ײ�û������
	if(Serial[Ch].TXPoint < Serial[Ch].SendLength)
		return FALSE;
	
	if(Serial[Ch].SendToSendDelay != 0)
		return FALSE;

	return TRUE; 
}
//-----------------------------------------------
//��������: ��һ��·�����㷨����ж�ʱ����ƺ���
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
static void CheckTripFlagValid(void)
{
//	//�����Ҫ����жϣ�Ŀǰ����һ��·����
//	if (MalignantLoadPara.AlgorithmCombineSwitch[RELAY1] == 0x00)			//00��ʾ��ϡ�&&��
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
	TRealTimer t;

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t); // ��ȡ��ǰʱ��
	Ch = ePT_UART1_F415;
	pollCount++;
	
	if( api_GetTaskFlag(eTASK_COLLECT_ID, eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_COLLECT_ID, eFLAG_SECOND);
		// SecLoopCJFlagRetry();			//���Կ���
		if (IsUpdateF415 == TRUE)
		{
			if (Collection_Recovery_Time > 0)
			{
				Collection_Recovery_Time--;	
			}
			else if (Collection_Recovery_Time == 0)
			{
				//ʱ�䵽����Ϊ����������ɣ��ָ��ɼ�����
				IsUpdateF415 = FALSE;
			}
			else
			{
				return ;
			}				
		}
		else
		{
			if (t.Sec == 5)	//ÿ���ӵ�5����һ��
			{
				SetCJFlag_m(0,MSF_VOL_CUR_PARA);
			}
			if ((t.Sec % 3) == 0) //3����һ��
			{
				SetCJFlag_m(0,MSF_IDENTIFY_PARA);
			}
		}
		//����жϵ�һ��·�����㷨����ж�ԭ���־����Ч��
		CheckTripFlagValid();
	}

	//��������
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

	//Сʱ����
	if( api_GetTaskFlag(eTASK_COLLECT_ID, eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_COLLECT_ID, eFLAG_HOUR);
	}

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
//	//2���ѱ�
//	if (Ch == ePT_4852)
//	{
//		// if(api_ProSearchMeter(Ch))
//		// {
//		// 	Serial[Ch].SendToSendDelay = MAX_SEND_DELAY;
//		// 	return;
//		// }
//	}

	//3���ɼ�����ı��ļ���
	if((Ch == ePT_UART1_F415) || (Ch == ePT_UART2_F415))
	{
		if(IsUpdateF415 == FALSE)
		{
			if((TSendRec[Ch-1].Sec_Send_Success - TSendRec[Ch-1].Sec_Rec_Success) > F415_REC_FAILED_TIMES)
			{
				memset(&TSendRec[Ch-1].Sec_Send_Success,0,sizeof(TCollectSendRec));	//����
				//��¼�쳣�¼�
				api_WriteSysUNMsg( GD32F415_PROTOCOL_ERR );
				// ����F415��Դ
				if(ControlResetStatus == 0)
				{
					POWER_F415_CV_CLOSE;
					api_Delayms(1000);
					POWER_F415_CV_OPEN;
				}
			}
		}
		//�ɼ����Լ���������
		if(TSendRec[Ch-1].Sec_Send_Success > COUNTUPPERLIMIT)
		{
			memset(&TSendRec[Ch-1].Sec_Send_Success,0x00,sizeof(TCollectSendRec));
		}
	}

	//3���ɼ�
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
//��������: ���ڴ���һ�η���
//
//����:	Serial[in]: ��Ҫ���͵Ĵ��ں�
//		Buf[in]: ��Ҫ���͵�����
//		Size[in]: ���ݳߴ�
//����ֵ:��
//
//��ע:
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
//��������: �ϵ�ɼ���ʼ��
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void api_PowerUpCollect(void)
{
	// api_InitI2C();
	// PowerUpUnitParaInit();

	//�����������Լ�������
	//m_CJFlag |= (1U << MSF_OVERI_PARA_I);
	//m_CJFlag |= (1U << MSF_OVERI_PARA_II);
	//m_CJFlag |= (1U << MSF_OVERI_PARA_III);
	//m_CJFlag |= (1U << MSF_OVERI_PARA_N);
	//m_CJFlag |= (1U << MSF_CTRL_WORD);

	// SetCJFlagRetry(MSF_UNIT_PARA, 3, 20);
	// m_CJFlag |= (1U << MSF_IDENTIFY_PARA);
	//�ϵ�ͬ��415��ѹ����ϵ��
	SetCJFlag_m(0,MSF_VOL_CUR_PARA);
	//�ϵ�ͬ��415ʱ��
	SetCJFlag_m(0,MSF_TIME_PARA);
	//�ϵ��ʶ������ݳ�ʼ��
	memset((BYTE*)&MessageData.ucTime,0xFF,sizeof(MessageData));
}

//--------------------------------------------------
//��������: �õ����ƿ��е� FLAG����
//
//����:	Flagp[in]: ����λ��
//
//����ֵ: λ��
//
//��ע:
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
//��������: �õ����ƿ��е� ��Ч��λ��
//
//����:
//
//����ֵ: ��Чλ��
//
//��ע:
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
//��������: ������Ա�־
//
//����: Flag[in]: ��־
//
//����ֵ:
//
//��ע:
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
//��������: ���� ���ͱ�־
//
//����:	Flag[in] : ��־
//		RetryNump[in]: ���Դ���
//		Sec[in]: ���ʱ��
//����ֵ:
//
//��ע:
//--------------------------------------------------
void SetCJFlagRetry(BYTE Ch, BYTE Flag, BYTE RetryNum, BYTE Sec)
{
	BYTE Index;

	//1�������Ƿ��п��п��ƿ飬���������͵�Ȩ����ֻ�ǲ�����
	m_CJFlag[Ch] |= (1U << Flag);

	//2���������������־�������ظ�����
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
//��������: ���ƿ���ѭ������
//
//����:	��
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
static void SecLoopCJFlagRetry(BYTE Ch)
{
	for (BYTE i = 0; i < CJFALG_RETRY_COUN; i++)
	{
		if(s_CJFlagRetryCtrl.Valid[i/8] & (1U << (i%8) ))
		{
			//1����ʱ����Ϊ0����ݼ�
			if(s_CJFlagRetryCtrl.Timer[i] != 0)
			{
				s_CJFlagRetryCtrl.Timer[i]--;
			}
			//2����ʱ��Ϊ0���Ҵ���Ϊ0����������
			else if(s_CJFlagRetryCtrl.RetryNum[i] == 0)
			{
				s_CJFlagRetryCtrl.Valid[i / 8] &= ~(1U << (i % 8));
			}
			else //3����ʱ��Ϊ0��������Ϊ0��������
			{
				s_CJFlagRetryCtrl.RetryNum[i]--;
				s_CJFlagRetryCtrl.Timer[i] = s_CJFlagRetryCtrl.TimerConst[i];

				m_CJFlag[Ch] |= (1U << s_CJFlagRetryCtrl.Flag[i]);
			}
		}	
	}
}
//--------------------------------------------------
//��������: 6025_to_415�ɼ�����״̬�Ķ�ȡ
//
//����:			0:�ɼ�״̬��ȡ
//				1:�ɼ�״̬����
//
//����ֵ:		BYTE
//
//��ע:415����ʱ���ɼ�����رգ�415ִ������ʱ���ɼ������
//			IsUpdateF415		�ɼ�����״̬
//				FALSE  			  1������)
//				TRUE  			  0���ر�)
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
//��������: 6025_to_415�ɼ�����״̬������
//
//����:		TYPE : 0���������ɼ�״̬�ر�----415������������ִ��
//			TYPE : 1���������ɼ�״̬����----415��������ܾ�ִ��
//
//����ֵ:		BYTE
//
//��ע:			
//һ��ֻ����ɼ���������Ȩ�ޣ��ɼ��ر�Ȩ������ʱ��ȡ����Collection_Recovery_For_415
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