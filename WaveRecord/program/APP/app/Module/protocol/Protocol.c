/*****************************************************************************/
/*�ļ�����Protocol.c
 *Copyright (c) 2003 ��̨������˼�ٵ������޹�˾������Ʒ������
 *All rights reserved
 *
 *�����ˣ�κ����
 *��  �ڣ�2007-11-20
 *�޸��ˣ�κ����
 *��  �ڣ�2007-11-20
 *��  ����DTSD/DSSD178ȫ����ʽ�๦�ܵ��ܱ�����Լ���򣬴��ļ��е��ø�����Լ�������
 *
 *��  ����
 */
/*****************************************************************************/

#include "AllHead.h"
#include "MCollect.h"
#include "Dlt645_2007.h"
#include "Dlt698_45.h"

//���ڻ��壬0����һ485��1�����⣻2���ڶ�485
TSerial Serial[MAX_COM_CHANNEL_NUM];
WORD FactoryTime;//�򿪳���ģʽʱ��
WORD LowFactoryTime;//�򿪵͵ȼ�����ģʽʱ��

BYTE APDUResponseBuf[MAX_COM_CHANNEL_NUM][MAX_APDU_SIZE+EXTRA_BUF];
ePROTOCO_TYPE LAST_PROTOCO_TYPE;
WORD CROverTime;//�ز���ʱʱ��
BYTE bBleChannel;
DWORD JTOverTime;//��̬��ʱʱ��

extern void CheckParaDaemon(void);

//�°��ʼ������ͨѶָ��
void api_InitSciStatus( BYTE i )
{
	InitPoint(&Serial[i]);

	Serial[i].TXPoint = 0;
	Serial[i].SendLength = 0;

	Serial[i].ReceToSendDelay = 0;

	Serial[i].BroadCastFlag = 0;
}

//i -- Ҫ��ʼ���Ķ˿ں�
void api_InitSubProtocol(WORD i)
{
	if(Serial[i].RxOverCount != 1)
	{
		Serial[i].BeginPoint = 0xff;
		Serial[i].byLastRXDCon = 0x91;//�����ϴ�ͨ�ŵ��Ӧ������룬�����������֡�ж��ã���ʼ��ΪĬ���ϴ�������ȡ
	}

	api_InitSciStatus( i );
    // InitAPDUBufFlag( i );
}
//��ʼ����Լ��дָ��,��ʼ�������ճ�ʱ����Լ����������������ã�
void InitPoint(TSerial * p)
{
	BYTE IntStatus,Ch;

	IntStatus = api_splx( 0 );

	p->ProStepDlt645 = 0;
	p->BeginPosDlt645 = 0;
	p->Dlt645DataLen = 0;

	#if(SEL_DLT698_2016_FUNC == YES)
	InitPoint_Dlt698(p);
	#endif
    

	p->RXWPoint = 0;
	p->RXRPoint = 0;

	p->RxOverCount = 0;

	api_splx( IntStatus );

}

void DoSendProc( BYTE Channel )
{
	SerialMap[Channel].SCIBeginSend(SerialMap[Channel].SCI_Ph_Num);
}

//---------------------------------------------------------------
//��������: �жϽ��ձ����е�ͨ�ŵ�ַ�Ƿ�ȫ���Ǹ���������
//
//����:    bySpecialData[in]:
//			99--�ж��·������Ƿ�㲥��ַ
//			88--�ж��·������Ƿ��Ƕ�����չ��6��88����12��8
//        pBuf[in] - ����ͨ�ŵ�ַ
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
WORD JudgeRecMeterNoOnlyOneData(BYTE bySpecialData,BYTE *pBuf)
{
	BYTE i,j;
	j = TRUE;

	for(i=0; i<6; i++)
	{
		if(pBuf[i] != bySpecialData )
		{
			j = FALSE;
			break;
		}
	}

	return j;
}

void Proc645(WORD i)
{
	WORD ReceNum, n;
	BYTE IntStatus;
	BYTE Result = FALSE;
	WORD ReportResult;
	SDWORD sdProLen;
	
	//��������
	if( Serial[i].SendLength > Serial[i].TXPoint )
	{
		DoSendProc( i );
	}
	else
	{
//		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == TRUE )//�ϵ���������ϱ��ж�
//		{
//			if( i == eCR )//�����ز�ͨ��
//			{
//				//�ز�ͨ�������ݷ��� �޷�֡ �ҵ�ǰδ���ܵ������ݺ󣬽��������ϱ��ж�
//				if( (Serial[eCR].RxOverCount == 0) && (FrameOverTime == 0) )
//				{
//					ReportResult = api_JudgeActiveReport();
//					if( ReportResult == TRUE )//�ϱ��ɹ���ǰ����
//					{
//						return ;
//					}
//				}
//			}
//		}

//		if( i == eBlueTooth )
//		{
//			sdProLen = api_mt_read_ble_ch_data(Serial[i].ProBuf,MAX_PRO_BUF_REAL+EXTRA_BUF,(BYTE*)&bBleChannel);
//			if( (sdProLen > 0) && (sdProLen <= (MAX_PRO_BUF_REAL+EXTRA_BUF)) )
//			{
//				Serial[i].RXWPoint = sdProLen;
//			}
//		}

		//������ɺ��������ա�
		IntStatus = api_splx( 0 );
		ReceNum = Serial[i].RXWPoint;
		api_splx( IntStatus );

        #if (SEL_SEARCH_METER == YES )
		if( (i == SEARCH_METER_CH) 
          && (SKMeter.byAutoCheck != 0) 
          //&& ( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) )
           )
        {   
            SKMeter.dwRxCnt += (MAX_PRO_BUF_REAL + Serial[i].RXWPoint - Serial[i].RXRPoint) % MAX_PRO_BUF_REAL;
            if( ReceNum > 2 )// ���ݲɼ�������ֵ���յ�����2���ֽھ���Ϊ�ǻظ������л�������
            {
                SKMeter.byAutoAck = 1;// ��ʱ�����ǽ����������ĺ��������ݵ����
            }
        }
        #endif

		//Ϊ���Э�鴦���ٶ� ��Ӧ115200ͨѶ������ һ��ѭ�����������ֽ�����
		//for( n = 0; n < 15; n++ )
		{
			while( ReceNum != Serial[i].RXRPoint )
			{
				Serial[i].RxOverCount = (DWORD)MAX_RX_OVER_TIME;
	
				#if( SEL_DLT645_2007 == YES )
				Result = DoReceProc_Dlt645( Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i] );
				#else
				Result = FALSE;
				#endif
				
				#if( SEL_DLT698_2016_FUNC == YES )
				if( Result == FALSE )
				{
					Result = DoReceProc_DLT698( Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i] );
				}
				#endif

                #if( SEL_1376_2_FUNC == YES )
				// if( Result == FALSE )
				// {
				// 	if (i == eCR) //ֻ���ز��ڴ���
				// 	{
				// 		Result = DoReceProc_13762( Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i] );
				// 	}	
				// }
                #endif
	
				//���������������InitPoint()������ִ������
				if( Result == FALSE )
				{
					Serial[i].RXRPoint++;
					if( Serial[i].RXRPoint >= MAX_PRO_BUF_REAL )
					{
						Serial[i].RXRPoint = 0;
					}
				}
				else//���ܵ���������
				{
					if( i == eCR )
					{
						setLedflag(eLedTxRx,eLightAllNone);
						CROverTime = CR_OVER_TIME;//���ܵ��ز����� ˢ�³�ʱʱ��
					}
					#if (SEL_SEARCH_METER == YES )
				    if( (i == SEARCH_METER_CH) 
                      && (SKMeter.byAutoCheck != 0) 
                      &&( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) )
                      )
                    {
                        SKMeter.byAutoAck = 0;
                        SKMeter.byRxOK = 1;
                    }
					#endif
					break;
				}
			}
			//else
			//{
			//	break;
			//}
		}
	}

	//������ն�ʱ
	if( Serial[i].RxOverCount != 0 )
	{
		if( Serial[i].RxOverCount == 1 )
		{
    		api_InitSci( i );
    		Serial[i].RxOverCount = 0;
		}

		//Ϊ��ȷ��ʱ���Ѽ��������Ƶ������ж���
		//Serial[i].RxOverCount --;
	}
}


//--------------------------------------------------
//��������:  ��ѭ�����㺯��
//         
//����  :   ��
//
//����ֵ:    ��   
//         
//��ע����:  ÿ500�������һ��
//--------------------------------------------------
void ProcClrTask(void)
{
	WORD wTmpClearMeterType;

	//wClearMeterType //0x1111--�������(���ݳ�ʼ��) 0x2222--�ָ��������� 0x4444--�¼����� 0x8888--��������
	if( FPara1->wClearMeterType == 0 )
	{
		return;
	}
	
	//���������1У��������˳�����ִ�����㣬��ֹ��EEPROM�ϵ���������
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara1), sizeof(TFPara1), UN_REPAIR_CRC ) == FALSE )
	{	
		return;
	}
	
	wTmpClearMeterType = FPara1->wClearMeterType;

	if( wTmpClearMeterType == eCLEAR_FACTORY )//��ʼ������
	{
		// ClearMeter( );		
		// InitPara( 1 );
		api_FactoryInitTask( );
	}
	
	
	wTmpClearMeterType = 0;
		
	api_WritePara(0, GET_STRUCT_ADDR(TFPara1, wClearMeterType), sizeof(WORD), (BYTE *)&wTmpClearMeterType );
}

void api_ProtocolTask(void)
{
    WORD i;
    BYTE tHour,TmpCommPara0;

    // static BYTE bBleProCount = 0;
	//���Ͳ���
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		//�ڽ��յ�һ֡�󣬷���ǰ��Ҫ��ʱһ��ʱ�䣬�ڴ�ʱ���ڣ�������645����
		if( Serial[i].ReceToSendDelay != 0 )
		{
			if( Serial[i].ReceToSendDelay != 1 )
			{
				//Ϊ��ʱ��׼ȷ�ԣ��ŵ������жϽ��м���
				//�����´���
				//Serial[i].ReceToSendDelay --;
				//����485���Ӷ�ʱ
			}
			else
			{
				Serial[i].ReceToSendDelay = 0;

				Proc645(i);
			}
		}
		else
		{
			Proc645(i);
		}
		
		api_ProSciTimer( i );
	}

	// if(api_GetTaskFlag(eTASK_PROTOCOL_ID, eFLAG_10_MS) == TRUE)
	// {
	// 	bBleProCount++;
	// 	if(bBleProCount == 2)
	// 	{
	// 		bBleProCount = 0;
	// 		ble_20ms_task();
	// 	}
	// 	api_ClrTaskFlag( eTASK_PROTOCOL_ID, eFLAG_10_MS );
	// }
	
	//500��������
	if(api_GetTaskFlag(eTASK_PROTOCOL_ID, eFLAG_500_MS) == TRUE)	
	{
		ProcClrTask();
		// api_ChargingCanTask(0);
	  	api_ClrTaskFlag( eTASK_PROTOCOL_ID, eFLAG_500_MS );
	}
	
	//1�뵽
	if( api_GetTaskFlag(eTASK_PROTOCOL_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PROTOCOL_ID,eFLAG_SECOND);

		// api_IapCountDown();

		//û��5���������´���
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			return;
		}

		// CheckParaDaemon();
		// TimeLinessEventJudge();
		for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
		{
			//���Ӵ���
			if( Serial[i].WatchSciTimer != 0 )
			{
				//���ڳ�ʱ��û�в�������Ҫ��ʼ��
				if( Serial[i].WatchSciTimer == 1 )
				{
				 	api_InitSci( i );
				}
				else
				{
					Serial[i].WatchSciTimer --;
				}
			}
			else
			{
				api_InitSci( i ); 
			}
		}
		
		if( LowFactoryTime != 0 )
		{
			LowFactoryTime --;
			if( LowFactoryTime > (60*30) )//����ֵ�ж�
			{
				LowFactoryTime = 0;
			}

			if( LowFactoryTime == 0 )//��͵ȼ�����ģʽȨ��
			{
				api_ClrSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY );
			}
		}
		
/*		#if(SEL_POWER_LINE_CARRIER_OR_GPRS == YES)
		if(NeedResetModul == 1)
		{
			//ÿ��23��13�����Ҹ�λһ�£��ϵ�30���ھͲ�Ҫ��λ��
			if( (RealTimer.Hour == 23) && (RealTimer.Min == 12) && (g_PowerOnSecTimer == 0) )
			{
				if(g_ResetModulTimer == 0)
				{
					g_ResetModulTimer = 65;
				}
			}
			if(g_ResetModulTimer != 0)
			{
				g_ResetModulTimer--;
				if(g_ResetModulTimer == 0)
				{
					//��λģ��
					RESET_PLC_MODUAL;
					ENABLE_PLC_PROGRAMM;
				}
			}
		}
		#endif*/

        #if (SEL_SEARCH_METER == YES )
        api_SearchMeterSecondTask();
        #endif

        #if (SEL_TOPOLOGY == YES )
        // TopoSecondTask();
        #endif  
	}
	
	//1���ӵ�
	if( api_GetTaskFlag(eTASK_PROTOCOL_ID,eFLAG_MINUTE) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PROTOCOL_ID,eFLAG_MINUTE);
		
		//û��5���������´���
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			return;
		}

		if( CROverTime > CR_OVER_TIME )//����ֵ�ж�
		{
			CROverTime = CR_OVER_TIME;
		}

		if( CROverTime != 0 )//����ʱ
		{
			CROverTime --;
			if (CROverTime==(CR_OVER_TIME-120))//��ʱ��Сʱ
			{
				// api_WriteSysUNMsg(SYSERR_CRTIME_ERR);
				// setLedflag(eLedTxRx,eLightAllOn);
				api_ResetModule4g();
			}
			
			if( CROverTime == 0 )
			{
				TmpCommPara0 = CommParaConst.ComModule;
				api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.ComModule), 1, &TmpCommPara0);
				Serial[eCR].OperationFlag = 1;
			}

		}
	}

		//Сʱ����
	if( api_GetTaskFlag(eTASK_PROTOCOL_ID, eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PROTOCOL_ID, eFLAG_HOUR);

		// api_CheckCanBusID();
        
        #if (SEL_SEARCH_METER == YES )
        CheckCycleSInfo();//���������ѱ����
        #endif
        
        #if (SEL_TOPOLOGY == YES )
        // CheckTopoPara();
        #endif

		// api_CheckMessageTransPara();
		
		api_GetRtcDateTime(DATETIME_hh,&tHour);
		//�жϹ����
		if(tHour == 0)
		{
		    api_ClrRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);//����698��������־
			api_ClrRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);//����02����������־
			api_ClrRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);//����04����������־
			api_ClrRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );//����㲥Уʱһ��ֻ�ܽ���һ�εı�־λ
			api_ClrRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );//������Ĺ㲥У����ʱһ��ֻ�ܼ�¼һ�εı�־λ
            memset( g_645PassWordErrCounter, 0x00, sizeof(g_645PassWordErrCounter) );
//			g_PassWordErrCounter = 0;	
		}	
	}
	
}


BYTE CalRXD_CheckSum(WORD ProtocolType, TSerial * p)
{
	WORD i;
	BYTE bySum = 0;

	WORD ProStep, BeginPos;
	//����645��Լ
	if( ProtocolType == PRO_NO_SPECIAL )
	{
		ProStep = p->ProStepDlt645;
		BeginPos = p->BeginPosDlt645;
	}
	else
	{
		return 0;
	}
    if( ProStep <= 0 )
    {
		return 0;
    }

	for( i=0; i<(ProStep-1); i++ )
	{
		bySum += p->ProBuf[(BeginPos+i)%MAX_PRO_BUF_REAL];
	}
	return bySum;
}

//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ����
void DoReceMoveData(WORD ProtocolType, TSerial * p)
{
	WORD ProStep, BeginPos;
	WORD i;
	BYTE Buf[MAX_PRO_BUF_REAL+30];
	//����645��Լ
	if( ProtocolType == PRO_NO_SPECIAL )
	{
		ProStep = p->ProStepDlt645;
		BeginPos = p->BeginPosDlt645;
	}

	#if(SEL_DLT698_2016_FUNC == YES )
	else if( ProtocolType == PRO_DLT698_45 )
	{
		ProStep = p->ProStepDlt698_45;
		BeginPos = p->BeginPosDlt698_45;
	}
	#endif
	else
	{
		return;
	}

	if( BeginPos == 0 )
	{
		return;
	}
	if( ProStep > MAX_PRO_BUF_REAL )
	{
		return;
	}	

	if( BeginPos <= (MAX_PRO_BUF_REAL-ProStep) )
	{
		for( i=0; i<ProStep; i++ )
		{
			 p->ProBuf[i] = p->ProBuf[BeginPos+i];
		}
	}
	else
	{

		for( i=0; i<ProStep; i++ )
		{
			 Buf[i] = p->ProBuf[(BeginPos+i)%MAX_PRO_BUF_REAL];
		}

		memcpy( (void*)p->ProBuf, (void*)Buf, ProStep );
	}

	//����645��Լ
	if( ProtocolType == PRO_NO_SPECIAL )
	{
		p->BeginPosDlt645 = 0;
	}

	#if(SEL_DLT698_2016_FUNC == YES )
	else if( ProtocolType == PRO_DLT698_45 )
	{
		p->BeginPosDlt698_45 = 0;
	}
	#endif
}



//ȡ�û�����TSerial�е��ĸ�����
//����: FALSE--���Ǵ��ڻ��� ����--���ص��Ǵ���Serial[]�ĵ�ַ
TSerial *GetSerial(BYTE *pBuf)
{
	BYTE i;

	i = 0;

	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( (pBuf >= Serial[i].ProBuf)&&(pBuf < (Serial[i].ProBuf+MAX_PRO_BUF_REAL)) )
		{
		//	return TRUE;
		//}
		//if( pBuf == (Serial[i].ProBuf+12) )
		//{
			return &(Serial[i]);
			//break;
		}
	}
	return FALSE;
}


void api_PowerUpProtocol( void )
{    
    TRealTimer PowerDownTime;
	BYTE TmpCommPara0;
	
    #if( SEL_DLT698_2016_FUNC == YES )
    PowerUpDlt698();
    #endif
    
    #if( SEL_DLT645_2007 == YES )
    PowerUpDlt645();
    #endif
	//��ȡ����ʱ��
	// api_GetPowerDownTime((TRealTimer *)&PowerDownTime);
	//���粻��ram���� ���� ����ʱ���뵱ǰʱ�䲻��ͬһ�� ����������־
	if( (api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR) == TRUE) || (RealTimer.Day != PowerDownTime.Day) )
	{
        api_ClrRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);//����698��������־
        api_ClrRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);//����02����������־
        api_ClrRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);//����04����������־
        api_ClrRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );//����㲥Уʱһ��ֻ�ܽ���һ�εı�־λ
		api_ClrRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );//������Ĺ㲥У����ʱһ��ֻ�ܼ�¼һ�εı�־λ
        memset( g_645PassWordErrCounter, 0x00, sizeof(g_645PassWordErrCounter) );
//        g_PassWordErrCounter = 0;   
	}
	
	TmpCommPara0 = CommParaConst.ComModule;
	if( TmpCommPara0 != FPara2->CommPara.ComModule )//�ز������ʱ仯���ϵ�ָ�Ĭ��ֵ
	{
		if( lib_CheckSafeMemVerify( (BYTE*)FPara2, sizeof(TFPara2), UN_REPAIR_CRC ) == TRUE )//CRCУ�鲻�ԣ����������FPara����ֹ���������ݵ�CRC�ָ�
		{
			api_WritePara( 1, GET_STRUCT_ADDR( TFPara2, CommPara.ComModule ), 1, &TmpCommPara0 );
			Serial[eCR].OperationFlag = 1;
			CROverTime = CR_OVER_TIME; //�ϵ���������ʱ
		}
	}
}

//---------------------------------------------------------------
//��������: ���������־
//
//����: 
//			eCLEAR_METER_TYPE - �����־λ
// 					eCLEAR_METER   = 0x1111,	//�������
//					eCLEAR_FACTORY = 0x2222		//����ʼ��
//					eCLEAR_EVENT   = 0x4444,	//�¼�����
//					eCLEAR_MONEY   = 0x6666,	//Ǯ����ʼ�������Ԥ�ÿ�
//					eCLEAR_PARA    = 0x7777,	//��ʼ������������ʼ�����ᡢ�¼�����ʾ�б�
//					eCLEAR_DEMAND  = 0x8888,	//��������
// 			BYTE DelayTime_500ms - ������ʱʱ�� 500msΪ��λ������Ϊ1��ʾ��ʱ500ms
//                   
//����ֵ:   TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL api_SetClearFlag( eCLEAR_METER_TYPE ClearType, BYTE DelayTime_500ms )
{
	//�������
	if( ClearType > eCLEAR_DEMAND )
	{
		return FALSE;
	}
	
	if( api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, wClearMeterType ), sizeof(WORD), (BYTE *)&ClearType ) == FALSE )
	{
		return FALSE;
	}
//	g_byClrTimer_500ms = DelayTime_500ms;
	
	api_WriteFreeEvent( EVENT_DEMAND_INIT_698, (WORD)ClearType );
	
	return TRUE;
}

//-----------------------------------------------
//��������  :   ���ڳ�ʼ��Э�����
//����  : 	��
//����ֵ: 
//��ע����  :  ��
//-----------------------------------------------
void FactoryInitProtocol( void )
{
    #if( SEL_DLT698_2016_FUNC == YES )
    FactoryInitDLT698();
    #endif//#if( SEL_DLT698_2016_FUNC == YES )
   
    #if( SEL_DLT645_2007 == YES )
    FactoryInitDLT645();
    #endif//#if( SEL_DLT645_2007 == YES )
}

//--------------------------------------------------
//��������:     �����������Buf
//
//����  :     BYTE Ch[in]                     ͨ��ѡ��
//          ePROTOCO_TYPE ProtocolType[in]  Э������
//
//����ֵ:      BYTE* bufָ��
//
//��ע����:  ��
//--------------------------------------------------
BYTE* AllocResponseBuf( BYTE Ch,         ePROTOCO_TYPE ProtocolType )
{
	if( LAST_PROTOCO_TYPE != ProtocolType )
    {
		if( ProtocolType == ePROTOCO_645 )
        {
            // InitDLT698Flag(Ch);
        }
		else if( ProtocolType == ePROTOCO_698 )
        {
            InitDLT645Flag((eSERIAL_TYPE)Ch);
        }

		LAST_PROTOCO_TYPE = ProtocolType;
    } 
    
    return (BYTE*)&APDUResponseBuf[Ch];
}


//--------------------------------------------------
//��������:     4Gģ�鸴λ
//
//����  :     
//
//����ֵ:      
//
//��ע����:  ��
//--------------------------------------------------
void api_ResetModule4g( void )
{
	RESET_PLC_MODUAL;
	// CROverTime = 1440;
}


