//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.8.15
//描述		

//修改记录	first_prog_flag首次编程标志需要在这边处理吗!!!!!!
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"



//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------																						


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
__no_init   BYTE			MacErrCounter;			//MAC错误次数
BYTE						PrePayPowerOnTimer;		//上电计时，从30减减
__no_init TPrePayPara  		PrePayPara;				//基本参数
TSafeAuPara         		SafeAuPara;             //TSafeAuPara

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明 
//-----------------------------------------------
static void VerifyPrePayPara( void );


//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 读预付费参数
//
//参数:		
//          ePrePayPara Type[in]	读写数据 详见枚举定义
//
//			Buf[out]	输出缓冲
//返回值:	    数据长度   长度为0 错误
//		   
//备注:
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
		case eKeyNum://密钥条数
			Buf[0] = PrePayPara.ScretKeyNum;
			Len = 1;
			break;
		case eCTCoe://CT变比
			memcpy(Buf, (BYTE*)&PrePayPara.CTCoe, 4);
			Len = 4;
			break;
		case ePTCoe://PT变比
			memcpy(Buf, (BYTE*)&PrePayPara.PTCoe, 4);
			Len = 4;
			break;
		case eIRTime://红外认证时间
			memcpy(Buf, (BYTE*)&PrePayPara.IRTime, 4);
			Len = 4;
			break;
        case eRemoteTime: //身份认证时间
            memcpy( Buf, (BYTE *)&PrePayPara.Remote645AuthTime, 2 );
            Len = 2;
            break;
			
        #if( PREPAY_MODE == PREPAY_LOCAL)
        case eLadderNum://阶梯数	
            Buf[0] = PrePayPara.LadderNum;
			Len = 1;
            break;
            
		case eTickLimit://透支金额门限
			memcpy(Buf, (BYTE*)&LimitMoney.TickLimit, 4);
			Len = 4;
			break;

        case eRegrateLimit://囤积金额门限
            memcpy(Buf, (BYTE*)&LimitMoney.RegrateLimit, 4);
            Len = 4;
            break;
            
        case eCloseLimit://合闸允许金额门限 
            memcpy(Buf, (BYTE*)&LimitMoney.CloseLimit, 4);
            Len = 4;
            break;
            
        case eAlarm_Limit1://报警金额1
            memcpy(Buf, (BYTE*)&LimitMoney.Alarm_Limit1, 4);
            Len = 4;
            break;
            
        case eAlarm_Limit2://报警金额2     
            memcpy(Buf, (BYTE*)&LimitMoney.Alarm_Limit2, 4);
            Len = 4;
            break;
        case eCurRateTable://当前备用套费率
        case eBackupRateTable:
            if( Type == eCurRateTable )//当前套
            {
                memcpy(Buf, (BYTE*)&CurRateTable, (sizeof(TRatePrice)-sizeof(DWORD)));
                Len = (sizeof(TRatePrice)-sizeof(DWORD));
            }
            else//备用套
            {
                MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.BackupRate );
                MyLength = sizeof(TRatePrice);
                api_VReadSafeMem( MyAddr, sizeof(TRatePrice),(BYTE*)&RatePrice );
                memcpy(Buf, (BYTE*)&RatePrice, (sizeof(TRatePrice)-sizeof(DWORD)));
                Len = (sizeof(TRatePrice)-sizeof(DWORD));
            }
            break;
            
        case eCurLadderTable://当前备用套阶梯
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

        case eRemainMoney://剩余金额（固定为2位小数，6位小数的只在模块内使用，不调用此函数）
        	Money.Remain.ll = CurMoneyBag.Money.Remain.ll / 10000;           
            memcpy( Buf, Money.Remain.b, 4 );
            Len = 4;
            break;
        case eBuyTimes://与购电次数
            memcpy( Money.Remain.b, CurMoneyBag.Money.Remain.b, sizeof(TMoney));
            memcpy( Buf, (BYTE*)&Money.BuyTimes, sizeof(Money.BuyTimes) );
            Len = sizeof(Money.BuyTimes);
            break;
        case eCurPice://当前电价
            memcpy( Buf, CurPrice.Current.b, 4 );
            Len = 4;
            break;
        case eCurRatePrice://当前费率电价
            memcpy( Buf, CurPrice.Current_F.b, 4 );
            Len = 4;
            break;
        case eCurLadderPrice://当前阶梯电价
            memcpy( Buf, CurPrice.Current_L.b, 4 );
            Len = 4;
            break;
        case eLocalAccountFlag: //本地开户状态
            Buf[0] = CardInfo.LocalFlag;
            Len = 1;
            break;
        case eRemoteAccountFlag: //远程开户状态
            Buf[0] = CardInfo.RemoteFlag;
            Len = 1;
            break;
        case eIllegalCardNum://异常插卡次数
            memcpy( Buf, CardInfo.IllegalCardNum.b, 4 );
            Len = 4;
            break;
        case eLadderUseEnergy://当前阶梯结算用电量
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
//函数功能: 写预付费参数
//
//参数:		ePrePayPara Type[in]	读写数据 详见枚举定义
//
//			Buf[in]	   输入缓冲 均为HEX格式
//返回值:	    TRUE/FALSE
//
//备注:    此函数更改费率阶梯电价可自动重新判断电价
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
		case eKeyNum://密钥条数
			PrePayPara.ScretKeyNum = Buf[0];
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
			break; 
		case eCTCoe://CT变比
			memcpy( (BYTE*)&PrePayPara.CTCoe, Buf, 4);
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
			break;
			
		case ePTCoe://PT变比
			memcpy( (BYTE*)&PrePayPara.PTCoe, Buf, 4);
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
			break;
			
		case eIRTime://红外认证时间
			memcpy( (BYTE*)&PrePayPara.IRTime, Buf, 4);
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
			break;
        case eRemoteTime: //身份认证时间
            memcpy( (BYTE *)&PrePayPara.Remote645AuthTime, Buf, 2 );
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
            MyLength = sizeof(TPrePayPara);
            pBuf = (BYTE *)&PrePayPara;
            break;
			
#if( PREPAY_MODE == PREPAY_LOCAL)
		case eLadderNum://阶梯数
			PrePayPara.LadderNum = Buf[0];
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara );
			MyLength = sizeof(TPrePayPara);
			pBuf = (BYTE*)&PrePayPara;
		    break;
        case eAlarm_Limit1://报警金额1
            memcpy( (BYTE*)&LimitMoney.Alarm_Limit1, Buf, 4);
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
            break;
            
        case eAlarm_Limit2://报警金额2     
            memcpy( (BYTE*)&LimitMoney.Alarm_Limit2, Buf, 4);
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
            break;

		case eTickLimit://透支金额门限
			memcpy( (BYTE*)&LimitMoney.TickLimit, Buf, 4);
			MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
			break;

        case eRegrateLimit://囤积金额门限
            memcpy( (BYTE*)&LimitMoney.RegrateLimit, Buf, 4);
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
            break;
            
        case eCloseLimit://合闸允许金额门限 
            memcpy( (BYTE*)&LimitMoney.CloseLimit, Buf, 4);
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney );
			MyLength = sizeof(TLimitMoney);
			pBuf = (BYTE*)&LimitMoney;
            break;
        case eCurRateTable://当前、备用套费率
        case eBackupRateTable:
            if( Type == eCurRateTable )//当前套费率
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
            
        case eCurLadderTable://当前、备用套阶梯
        case eBackupLadderTable:
            if( Type == eCurLadderTable )//国网直接更新RAM..南网当前套有两张 不能直接更新RAM中电价 需要判阶梯时区表
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

        case eLocalAccountFlag: //本地开户状态
            CardInfo.LocalFlag = Buf[0];  
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo );
            MyLength = sizeof(TCardInfo);
            pBuf = (BYTE*)&CardInfo;
            break;

        case eRemoteAccountFlag: //远程开户状态
            CardInfo.RemoteFlag = Buf[0];  
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo );
            MyLength = sizeof(TCardInfo);
            pBuf = (BYTE*)&CardInfo;
            break;

        case eSerialNo://卡片序列号
            memcpy( CardInfo.SerialNo, Buf, 8 );  
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo );
            MyLength = sizeof(TCardInfo);
            pBuf = (BYTE*)&CardInfo;
            break;

        case eIllegalCardNum://非法插卡记录
            memcpy( CardInfo.IllegalCardNum.b, Buf, 4 );  
            MyAddr = GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo );
            MyLength = sizeof(TCardInfo);
            pBuf = (BYTE*)&CardInfo;
            break;
            
        // case eRemoteTime://身份认证时间
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
	    if( TmpType == 0 )//如果写epprom失败重新将数据读出来，避免更改了ram没有改epprom
	    {
            api_VReadSafeMem( MyAddr, MyLength, pBuf);
	    }
	}

	#if( PREPAY_MODE == PREPAY_LOCAL)
	if( Type == eCurLadderTable )
	{
		api_SetUpdatePriceFlag( eChangeCurLadderPrice );//写阶梯电价 重新刷新阶梯电价
	}
	else if( Type == eCurRateTable )
	{
		api_SetUpdatePriceFlag( eChangeCurRatePrice );//写阶梯电价 重新刷新阶梯电价
	}
	#endif
    //更新完本地参数 刷新继电器状态
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
//函数功能: 预付费任务上电初始化
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_PowerUpPrePay(void)
{
    TRealTimer		PowerDownTime;
    DWORD dwAddr;
							
	PrePayPowerOnTimer = 30;
	
	VerifyPrePayPara();  //校验基本参数
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	PowerUpMoney(ePowerOnMode);          //校验金额参数
	ProcLadderModeSwitch();	 //阶梯模式判断
	#endif
	
	api_GetPowerDownTime((TRealTimer *)&PowerDownTime);
	//掉电不清ram出错 或者 掉电时间与当前时间不是同一天 清MAC错误标志
	if( (api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR) == TRUE) || (RealTimer.Day != PowerDownTime.Day) )
	{
		MacErrCounter = 0;
		api_ClrRunHardFlag(eRUN_HARD_MAC_ERR_FLAG);//清零密码错误标志
	}
}

//-----------------------------------------------
//函数功能: 预付费任务低功耗唤醒任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_LowPowerWakeUpPrePay(void)
{
	VerifyPrePayPara();  //校验基本参数
	#if( PREPAY_MODE == PREPAY_LOCAL )
	PowerUpMoney(ePowerWakeUpMode);          //校验金额参数
	#endif
}


//-----------------------------------------------
//函数功能: 预付费掉电处理
//
//参数:		无 
//                 
//返回值: 	无
//		   
//备注:   
//-----------------------------------------------
void api_PowerDownPrePay(void)
{
    #if( PREPAY_MODE == PREPAY_LOCAL )
    PowerDownMoney();
    #endif
}


//-----------------------------------------------
//函数功能: 预付费大循环任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_PrePayTask(void)
{
	TRealTimer t;
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	if( api_GetTaskFlag(eTASK_PREPAY_ID, eFLAG_10_MS) == TRUE )//每10ms执行一次
	{
		api_ClrTaskFlag(eTASK_PREPAY_ID, eFLAG_10_MS);
		Card_Proc();
	}
    #endif   

	//秒任务
	if( api_GetTaskFlag(eTASK_PREPAY_ID, eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PREPAY_ID, eFLAG_SECOND);
		
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
		
		if(PrePayPowerOnTimer)
		{
			PrePayPowerOnTimer--;
		}
		
		#if( PREPAY_MODE == PREPAY_LOCAL )
		MoneySecTask();//金额秒任务
		#endif
		
		//上电第3秒刷新密钥状态
		if(PrePayPowerOnTimer == 27)
		{
			#if( MAX_PHASE != 1 )
			api_RecordPowerUpVoltage();	//记录上电电压
			#endif
			#if( SEL_ESSAM == YES )
			api_FlashKeyStatus();		//刷新密钥状态
			PowerUpESAMMeterNo( );		//刷新表号
			api_FlashIdentAuthFlag();	//刷新身份认证权限标志
			#endif
		}
		
		if( t.Sec == (eTASK_PREPAY_ID*3+3) )
		{
			VerifyPrePayPara(); //校验基本参数
			#if( PREPAY_MODE == PREPAY_LOCAL )
			MoneyMinuteTask();//金额分钟任务
			#endif

		}
	}
	//分钟任务
	//if( api_GetTaskFlag( eTASK_PREPAY_ID, eFLAG_MINUTE ) == TRUE )
	{

	}
	//小时任务
	if( api_GetTaskFlag(eTASK_PREPAY_ID, eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PREPAY_ID, eFLAG_HOUR);
		#if( SEL_ESSAM == YES )
		api_FlashKeyStatus();
		api_FlashIdentAuthFlag();	//刷新身份认证权限标志
		#endif
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss,(BYTE*)&t);
		
		//判断过零点
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
//函数功能: 基本参数校验 
//
//参数: 	无
//                   
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void VerifyPrePayPara( void )
{	
	//PT、CT、红外认证时间
	if( PrePayPara.CRC32 != lib_CheckCRC32( (BYTE*)&PrePayPara.ScretKeyNum, sizeof(TPrePayPara) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.PrePayPara ), sizeof(TPrePayPara),(BYTE*)&PrePayPara.ScretKeyNum ) != TRUE )
		{
			//采用默认值
		}
	}
}

//-----------------------------------------------
//函数功能: 厂内电表初始化任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_FactoryInitPrePay(void)
{
	MacErrCounter = 0;
	
    PrePayPara.IRTime = IRTimeConst;		 //红外认证时间30分钟
    PrePayPara.LadderNum = LadderNumConst;
    PrePayPara.ScretKeyNum = ScretKeyNum;
	#if( PREPAY_MODE != PREPAY_LOCAL )
	PrePayPara.CTCoe = 1;			//无小数 
	PrePayPara.PTCoe = 1;			//无小数
	#endif
	PrePayPara.Remote645AuthTime = Remote645AuthTimeConst;	//身份认证时效30分钟
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(PrePaySafeRom.PrePayPara),sizeof(TPrePayPara),(BYTE*)&PrePayPara);

    #if( PREPAY_MODE == PREPAY_LOCAL )
    LimitMoney.CloseLimit.d = CloseLimitConst;
    LimitMoney.RegrateLimit.d = RegrateLimitConst;
    LimitMoney.TickLimit.d = TickLimitConst;
    api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(PrePaySafeRom.LimitMoney),sizeof(TLimitMoney),(BYTE*)&LimitMoney);

	#endif
	
	#if( SEL_ESSAM == YES )
	api_FlashKeyStatus();
	api_FlashIdentAuthFlag();	//刷新身份认证权限标志
	#endif
}



//-----------------------------------------------
//函数功能: 预付费清零，供电表清零 ClearMeter 调用
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_ClrPrePay(void)
{	
	MacErrCounter = 0;
	#if( PREPAY_MODE == PREPAY_LOCAL )
    api_ClrCurLadderUseEnergy();
    ClrCardInfo();
    //不能调用，否则参数预置卡、钱包初始化中购电后在此清零了累计购电金额导致为0 wlk
    //api_ClrContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD) );	
    api_SetUpdatePriceFlag( ePriceChangeEnergy );
	#endif
}




