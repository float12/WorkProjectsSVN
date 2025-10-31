//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.15
//����		

//�޸ļ�¼	first_prog_flag�״α�̱�־��Ҫ����ߴ�����!!!!!!
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"



//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------																						


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
__no_init   BYTE			MacErrCounter;			//MAC�������
BYTE						PrePayPowerOnTimer;		//�ϵ��ʱ����30����
__no_init TPrePayPara  		PrePayPara;				//��������
TSafeAuPara         		SafeAuPara;             //TSafeAuPara

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ��������� 
//-----------------------------------------------
static void VerifyPrePayPara( void );


//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��Ԥ���Ѳ���
//
//����:		
//          ePrePayPara Type[in]	��д���� ���ö�ٶ���
//
//			Buf[out]	�������
//����ֵ:	    ���ݳ���   ����Ϊ0 ����
//		   
//��ע:
//-----------------------------------------------
WORD api_ReadPrePayPara( ePrePayParaType Type, BYTE *Buf)
{
    BYTE i;
	WORD Len;
	WORD MyAddr, MyLength;
    #if( PREPAY_MODE == PREPAY_LOCAL)
	TMoney Money;
	TRatePrice RatePrice;
	
	TLadderPrice LadderPrice;
    TFourByteUnion UseEnergy;
    #endif
    
	Len = 0;
    
    switch( Type )
	{
		case eKeyNum://��Կ����
			Buf[0] = PrePayPara.ScretKeyNum;
			Len = 1;
			break;
		case eCTCoe://CT���
			memcpy(Buf, (BYTE*)&PrePayPara.CTCoe, 4);
			Len = 4;
			break;
		case ePTCoe://PT���
			memcpy(Buf, (BYTE*)&PrePayPara.PTCoe, 4);
			Len = 4;
			break;
		case eIRTime://������֤ʱ��
			memcpy(Buf, (BYTE*)&PrePayPara.IRTime, 4);
			Len = 4;
			break;
        case eRemoteTime: //�����֤ʱ��
            memcpy( Buf, (BYTE *)&PrePayPara.Remote645AuthTime, 2 );
            Len = 2;
            break;
			
        #if( PREPAY_MODE == PREPAY_LOCAL)
        case eLadderNum://������	
            Buf[0] = PrePayPara.LadderNum;
			Len = 1;
            break;
            
		case eTickLimit://͸֧�������
			memcpy(Buf, (BYTE*)&LimitMoney.TickLimit, 4);
			Len = 4;
			break;

        case eRegrateLimit://�ڻ��������
            memcpy(Buf, (BYTE*)&LimitMoney.RegrateLimit, 4);
            Len = 4;
            break;
            
        case eCloseLimit://��բ���������� 
            memcpy(Buf, (BYTE*)&LimitMoney.CloseLimit, 4);
            Len = 4;
            break;
            
        case eAlarm_Limit1://�������1
            memcpy(Buf, (BYTE*)&LimitMoney.Alarm_Limit1, 4);
            Len = 4;
            break;
            
        case eAlarm_Limit2://�������2     
            memcpy(Buf, (BYTE*)&LimitMoney.Alarm_Limit2, 4);
            Len = 4;
            break;
        case eCurRateTable://��ǰ�����׷���
        case eBackupRateTable:
            if( Type == eCurRateTable )//��ǰ��
            {
                memcpy(Buf, (BYTE*)&CurRateTable, (sizeof(TRatePrice)-sizeof(DWORD)));
                Len = (sizeof(TRatePrice)-sizeof(DWORD));
            }
            else//������
            {
                MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.BackupRate );
                MyLength = sizeof(TRatePrice);
                api_VReadSafeMem( MyAddr, sizeof(TRatePrice),(BYTE*)&RatePrice );
                memcpy(Buf, (BYTE*)&RatePrice, (sizeof(TRatePrice)-sizeof(DWORD)));
                Len = (sizeof(TRatePrice)-sizeof(DWORD));
            }
            break;
            
        case eCurLadderTable://��ǰ�����׽���
        case eBackupLadderTable:
            if( Type == eCurLadderTable )
            {
                memcpy(Buf, (BYTE*)&CurLadderTable, (sizeof(TLadderPrice)-sizeof(DWORD)));
                Len = (sizeof(TLadderPrice)-sizeof(DWORD));
            }
            else
            {
            	MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.BackupLadder );
            	MyLength = sizeof(TLadderPrice);
            	api_VReadSafeMem( MyAddr, sizeof(TLadderPrice),(BYTE*)&LadderPrice );
            	memcpy(Buf, (BYTE*)&LadderPrice, (sizeof(TLadderPrice)-sizeof(DWORD)));
            	Len = (sizeof(TLadderPrice)-sizeof(DWORD));
            }
            break; 

        case eRemainMoney://ʣ����̶�Ϊ2λС����6λС����ֻ��ģ����ʹ�ã������ô˺�����
        	Money.Remain.ll = CurMoneyBag.Money.Remain.ll / 10000;           
            memcpy( Buf, Money.Remain.b, 4 );
            Len = 4;
            break;
        case eBuyTimes://�빺�����
            memcpy( Money.Remain.b, CurMoneyBag.Money.Remain.b, sizeof(TMoney));
            memcpy( Buf, (BYTE*)&Money.BuyTimes, sizeof(Money.BuyTimes) );
            Len = sizeof(Money.BuyTimes);
            break;
        case eCurPice://��ǰ���
            memcpy( Buf, CurPrice.Current.b, 4 );
            Len = 4;
            break;
        case eCurRatePrice://��ǰ���ʵ��
            memcpy( Buf, CurPrice.Current_F.b, 4 );
            Len = 4;
            break;
        case eCurLadderPrice://��ǰ���ݵ��
            memcpy( Buf, CurPrice.Current_L.b, 4 );
            Len = 4;
            break;
        case eLocalAccountFlag: //���ؿ���״̬
            Buf[0] = CardInfo.LocalFlag;
            Len = 1;
            break;
        case eRemoteAccountFlag: //Զ�̿���״̬
            Buf[0] = CardInfo.RemoteFlag;
            Len = 1;
            break;
        case eIllegalCardNum://�쳣�忨����
            memcpy( Buf, CardInfo.IllegalCardNum.b, 4 );
            Len = 4;
            break;
        case eLadderUseEnergy://��ǰ���ݽ����õ���
            UseEnergy.d = GetCurLadderUserEnergy();
            memcpy( Buf, UseEnergy.b, 4 );
            Len = 4;
            break;
        #endif

		default:
		    Len = 0;
			break;
	}
	
	return Len;
}

//-----------------------------------------------
//��������: дԤ���Ѳ���
//
//����:		ePrePayPara Type[in]	��д���� ���ö�ٶ���
//
//			Buf[in]	   ���뻺�� ��ΪHEX��ʽ
//����ֵ:	    TRUE/FALSE
//
//��ע:    �˺������ķ��ʽ��ݵ�ۿ��Զ������жϵ��
//-----------------------------------------------
BOOL api_WritePrePayPara( ePrePayParaType Type, BYTE *Buf)
{
    BYTE i;
	WORD Result;
	WORD MyAddr, MyLength;
	BYTE TmpType;
    BYTE *pBuf;
	
    TmpType = (BYTE)Type&0x80;
	
	switch(Type)
	{
		case eKeyNum://��Կ����
			PrePayPara.ScretKeyNum = Buf[0];
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
			break; 
		case eCTCoe://CT���
			memcpy( (BYTE*)&PrePayPara.CTCoe, Buf, 4);
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
			break;
			
		case ePTCoe://PT���
			memcpy( (BYTE*)&PrePayPara.PTCoe, Buf, 4);
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
			break;
			
		case eIRTime://������֤ʱ��
			memcpy( (BYTE*)&PrePayPara.IRTime, Buf, 4);
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
			break;
        case eRemoteTime: //�����֤ʱ��
            memcpy( (BYTE *)&PrePayPara.Remote645AuthTime, Buf, 2 );
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
            MyLength = sizeof(TPrePayPara);
            pBuf = (BYTE *)&PrePayPara;
            break;
			
#if( PREPAY_MODE == PREPAY_LOCAL)
		case eLadderNum://������
			PrePayPara.LadderNum = Buf[0];
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
		    break;
        case eAlarm_Limit1://�������1
            memcpy( (BYTE*)&LimitMoney.Alarm_Limit1, Buf, 4);
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
            break;
            
        case eAlarm_Limit2://�������2     
            memcpy( (BYTE*)&LimitMoney.Alarm_Limit2, Buf, 4);
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
            break;

		case eTickLimit://͸֧�������
			memcpy( (BYTE*)&LimitMoney.TickLimit, Buf, 4);
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
			break;

        case eRegrateLimit://�ڻ��������
            memcpy( (BYTE*)&LimitMoney.RegrateLimit, Buf, 4);
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
            break;
            
        case eCloseLimit://��բ���������� 
            memcpy( (BYTE*)&LimitMoney.CloseLimit, Buf, 4);
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
            break;
        case eCurRateTable://��ǰ�������׷���
        case eBackupRateTable:
            if( Type == eCurRateTable )//��ǰ�׷���
            {
                 memcpy( (BYTE*)&CurRateTable, Buf, sizeof(TRatePrice)-sizeof(DWORD));
                 MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentRate );
                 MyLength = sizeof(TRatePrice);
                 pBuf = (BYTE*)&CurRateTable;
           
            }
            else
            {
                MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.BackupRate );
                MyLength = sizeof(TRatePrice);
                pBuf = Buf;
            }
            
            break;
            
        case eCurLadderTable://��ǰ�������׽���
        case eBackupLadderTable:
            if( Type == eCurLadderTable )//����ֱ�Ӹ���RAM..������ǰ�������� ����ֱ�Ӹ���RAM�е�� ��Ҫ�н���ʱ����
            {
				memcpy( (BYTE*)&CurLadderTable, Buf, sizeof(TLadderPrice)-sizeof(DWORD));
				MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentLadder );
                MyLength = sizeof(TLadderPrice);
                pBuf = (BYTE*)&CurLadderTable;
                
                
            }
            else
            {
                MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.BackupLadder );
                MyLength = sizeof(TLadderPrice);
                pBuf = Buf;
            }
        
            break;

        case eLocalAccountFlag: //���ؿ���״̬
            CardInfo.LocalFlag = Buf[0];  
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo );
            MyLength = sizeof(TCardInfo);
            pBuf = (BYTE*)&CardInfo;
            break;

        case eRemoteAccountFlag: //Զ�̿���״̬
            CardInfo.RemoteFlag = Buf[0];  
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo );
            MyLength = sizeof(TCardInfo);
            pBuf = (BYTE*)&CardInfo;
            break;

        case eSerialNo://��Ƭ���к�
            memcpy( CardInfo.SerialNo, Buf, 8 );  
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo );
            MyLength = sizeof(TCardInfo);
            pBuf = (BYTE*)&CardInfo;
            break;

        case eIllegalCardNum://�Ƿ��忨��¼
            memcpy( CardInfo.IllegalCardNum.b, Buf, 4 );  
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo );
            MyLength = sizeof(TCardInfo);
            pBuf = (BYTE*)&CardInfo;
            break;
            
        // case eRemoteTime://�����֤ʱ��
        //     memcpy( (BYTE*)&PrePayPara.RemoteTime, Buf, 4);
        //	MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
		//	MyLength = sizeof(TPrePayPara);
		//	pBuf = (BYTE*)&PrePayPara;
		//	break;         
 #endif
		default:
		    return FALSE;
			break;
	}
	
	Result = api_VWriteSafeMem( MyAddr, MyLength, pBuf);
	if(Result != TRUE)
	{
	    if( TmpType == 0 )//���дeppromʧ�����½����ݶ����������������ramû�и�epprom
	    {
            api_VReadSafeMem( MyAddr, MyLength, pBuf);
	    }
	}

	#if( PREPAY_MODE == PREPAY_LOCAL)
	if( Type == eCurLadderTable )
	{
		api_SetUpdatePriceFlag( eChangeCurLadderPrice );//д���ݵ�� ����ˢ�½��ݵ��
	}
	else if( Type == eCurRateTable )
	{
		api_SetUpdatePriceFlag( eChangeCurRatePrice );//д���ݵ�� ����ˢ�½��ݵ��
	}
	#endif
    //�����걾�ز��� ˢ�¼̵���״̬
	#if( PREPAY_MODE == PREPAY_LOCAL)
    if( (Type == eAlarm_Limit1) || (Type == eAlarm_Limit2) || (Type == eTickLimit) )
    {
        JudgeLocalStatus( eSetParaMode, Type );
    }
	#endif
	
	return TRUE;
}

BOOL PowerUpESAMMeterNo( void )
{
	BYTE Buf[20];
	WORD Result;

	Result = TRUE;
	
	Result = api_ReadEsamData(0x0001, 0x0007, 0x0006, Buf);
	if( Result == FALSE )
	{
		return FALSE;
	}

	if( memcmp( FPara1->MeterSnPara.MeterNo, Buf, 6) != 0 )
	{
		Result = api_ProcMeterTypePara( WRITE, eMeterMeterNo, Buf);
		api_WriteFreeEvent( EVENT_RESTORE_INFO_FROM_ESAM, 0 );
	}

	return Result;
	
}
//-----------------------------------------------
//��������: Ԥ���������ϵ��ʼ��
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_PowerUpPrePay(void)
{
    TRealTimer		PowerDownTime;
    DWORD dwAddr;
							
	PrePayPowerOnTimer = 30;
	
	VerifyPrePayPara();  //У���������
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	PowerUpMoney(ePowerOnMode);          //У�������
	ProcLadderModeSwitch();	 //����ģʽ�ж�
	#endif
	
	api_GetPowerDownTime((TRealTimer *)&PowerDownTime);
	//���粻��ram���� ���� ����ʱ���뵱ǰʱ�䲻��ͬһ�� ��MAC�����־
	if( (api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR) == TRUE) || (RealTimer.Day != PowerDownTime.Day) )
	{
		MacErrCounter = 0;
		api_ClrRunHardFlag(eRUN_HARD_MAC_ERR_FLAG);//������������־
	}
}

//-----------------------------------------------
//��������: Ԥ��������͹��Ļ�������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_LowPowerWakeUpPrePay(void)
{
	VerifyPrePayPara();  //У���������
	#if( PREPAY_MODE == PREPAY_LOCAL )
	PowerUpMoney(ePowerWakeUpMode);          //У�������
	#endif
}


//-----------------------------------------------
//��������: Ԥ���ѵ��紦��
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_PowerDownPrePay(void)
{
    #if( PREPAY_MODE == PREPAY_LOCAL )
    PowerDownMoney();
    #endif
}


//-----------------------------------------------
//��������: Ԥ���Ѵ�ѭ������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_PrePayTask(void)
{
	TRealTimer t;
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	if( api_GetTaskFlag(eTASK_PREPAY_ID, eFLAG_10_MS) == TRUE )//ÿ10msִ��һ��
	{
		api_ClrTaskFlag(eTASK_PREPAY_ID, eFLAG_10_MS);
		Card_Proc();
	}
    #endif   

	//������
	if( api_GetTaskFlag(eTASK_PREPAY_ID, eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PREPAY_ID, eFLAG_SECOND);
		
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
		
		if(PrePayPowerOnTimer)
		{
			PrePayPowerOnTimer--;
		}
		
		#if( PREPAY_MODE == PREPAY_LOCAL )
		MoneySecTask();//���������
		#endif
		
		//�ϵ��3��ˢ����Կ״̬
		if(PrePayPowerOnTimer == 27)
		{
			#if( MAX_PHASE != 1 )
			api_RecordPowerUpVoltage();	//��¼�ϵ��ѹ
			#endif
			#if( SEL_ESSAM == YES )
			api_FlashKeyStatus();		//ˢ����Կ״̬
			PowerUpESAMMeterNo( );		//ˢ�±��
			api_FlashIdentAuthFlag();	//ˢ�������֤Ȩ�ޱ�־
			#endif
		}
		
		if( t.Sec == (eTASK_PREPAY_ID*3+3) )
		{
			VerifyPrePayPara(); //У���������
			#if( PREPAY_MODE == PREPAY_LOCAL )
			MoneyMinuteTask();//����������
			#endif

		}
	}
	//��������
	//if( api_GetTaskFlag( eTASK_PREPAY_ID, eFLAG_MINUTE ) == TRUE )
	{

	}
	//Сʱ����
	if( api_GetTaskFlag(eTASK_PREPAY_ID, eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PREPAY_ID, eFLAG_HOUR);
		#if( SEL_ESSAM == YES )
		api_FlashKeyStatus();
		api_FlashIdentAuthFlag();	//ˢ�������֤Ȩ�ޱ�־
		#endif
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
		
		//�жϹ����
		if(t.Hour == 0)
		{
			api_ClrRunHardFlag(eRUN_HARD_MAC_ERR_FLAG);
			MacErrCounter = 0;
			SysFREEMsgCounter = 0;
			SysUNMsgCounter = 0;
		}	
	}
}


//-----------------------------------------------
//��������: ��������У�� 
//
//����: 	��
//                   
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void VerifyPrePayPara( void )
{	
	//PT��CT��������֤ʱ��
	if( PrePayPara.CRC32 != lib_CheckCRC32( (BYTE*)&PrePayPara.ScretKeyNum, sizeof(TPrePayPara) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara ), sizeof(TPrePayPara),(BYTE*)&PrePayPara.ScretKeyNum ) != TRUE )
		{
			//����Ĭ��ֵ
		}
	}
}

//-----------------------------------------------
//��������: ���ڵ���ʼ������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_FactoryInitPrePay(void)
{
	MacErrCounter = 0;
	
    PrePayPara.IRTime = IRTimeConst;		 //������֤ʱ��30����
    PrePayPara.LadderNum = LadderNumConst;
    PrePayPara.ScretKeyNum = ScretKeyNum;
	#if( PREPAY_MODE != PREPAY_LOCAL )
	PrePayPara.CTCoe = 1;			//��С�� 
	PrePayPara.PTCoe = 1;			//��С��
	#endif
	PrePayPara.Remote645AuthTime = Remote645AuthTimeConst;	//�����֤ʱЧ30����
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(PrePaySafeRom.PrePayPara),sizeof(TPrePayPara),(BYTE*)&PrePayPara);

    #if( PREPAY_MODE == PREPAY_LOCAL )
    LimitMoney.CloseLimit.d = CloseLimitConst;
    LimitMoney.RegrateLimit.d = RegrateLimitConst;
    LimitMoney.TickLimit.d = TickLimitConst;
    api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(PrePaySafeRom.LimitMoney),sizeof(TLimitMoney),(BYTE*)&LimitMoney);

	#endif
	
	#if( SEL_ESSAM == YES )
	api_FlashKeyStatus();
	api_FlashIdentAuthFlag();	//ˢ�������֤Ȩ�ޱ�־
	#endif
}



//-----------------------------------------------
//��������: Ԥ�������㣬��������� ClearMeter ����
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_ClrPrePay(void)
{	
	MacErrCounter = 0;
	#if( PREPAY_MODE == PREPAY_LOCAL )
    api_ClrCurLadderUseEnergy();
    ClrCardInfo();
    //���ܵ��ã��������Ԥ�ÿ���Ǯ����ʼ���й�����ڴ��������ۼƹ������Ϊ0 wlk
    //api_ClrContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD) );	
    api_SetUpdatePriceFlag( ePriceChangeEnergy );
	#endif
}




