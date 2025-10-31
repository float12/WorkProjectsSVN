/*****************************************************************************/
/*文件名：Protocol.c
 *Copyright (c) 2003 烟台东方威思顿电气有限公司计量产品开发组
 *All rights reserved
 *
 *创建人：魏灵坤
 *日  期：2007-11-20
 *修改人：魏灵坤
 *日  期：2007-11-20
 *描  述：DTSD/DSSD178全电子式多功能电能表国标规约程序，此文件中调用各个规约处理程序
 *
 *版  本：
 */
/*****************************************************************************/

#include "AllHead.h"
#include "MCollect.h"
#include "Dlt645_2007.h"
#include "Dlt698_45.h"

//串口缓冲，0－第一485；1－红外；2－第二485
TSerial Serial[MAX_COM_CHANNEL_NUM];
WORD FactoryTime;//打开厂内模式时间
WORD LowFactoryTime;//打开低等级厂内模式时间

BYTE APDUResponseBuf[MAX_COM_CHANNEL_NUM][MAX_APDU_SIZE+EXTRA_BUF];
ePROTOCO_TYPE LAST_PROTOCO_TYPE;
WORD CROverTime;//载波超时时间
BYTE bBleChannel;
DWORD JTOverTime;//静态超时时间

extern void CheckParaDaemon(void);

//新板初始化串口通讯指针
void api_InitSciStatus( BYTE i )
{
	InitPoint(&Serial[i]);

	Serial[i].TXPoint = 0;
	Serial[i].SendLength = 0;

	Serial[i].ReceToSendDelay = 0;

	Serial[i].BroadCastFlag = 0;
}

//i -- 要初始化的端口号
void api_InitSubProtocol(WORD i)
{
	if(Serial[i].RxOverCount != 1)
	{
		Serial[i].BeginPoint = 0xff;
		Serial[i].byLastRXDCon = 0x91;//保存上次通信电表应答控制码，供后面读后续帧判断用，初始化为默认上次正常读取
	}

	api_InitSciStatus( i );
    // InitAPDUBufFlag( i );
}
//初始化规约读写指针,初始化、接收超时、规约处理完三种情况调用，
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
//函数功能: 判断接收报文中的通信地址是否全部是给定的数据
//
//参数:    bySpecialData[in]:
//			99--判断下发命令是否广播地址
//			88--判断下发命令是否是东方扩展的6个88，即12个8
//        pBuf[in] - 传入通信地址
//                   
//返回值:  TRUE/FALSE
//
//备注:   
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
	
	//正常处理
	if( Serial[i].SendLength > Serial[i].TXPoint )
	{
		DoSendProc( i );
	}
	else
	{
//		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == TRUE )//上电进行主动上报判断
//		{
//			if( i == eCR )//处理载波通道
//			{
//				//载波通道无数据发送 无分帧 且当前未接受到新数据后，进行主动上报判断
//				if( (Serial[eCR].RxOverCount == 0) && (FrameOverTime == 0) )
//				{
//					ReportResult = api_JudgeActiveReport();
//					if( ReportResult == TRUE )//上报成功提前返回
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

		//发送完成后才允许接收。
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
            if( ReceNum > 2 )// 根据采集器经验值，收到超过2个字节就认为是回复报文有混叠的情况
            {
                SKMeter.byAutoAck = 1;// 暂时不考虑解析完整报文后，又来数据的情况
            }
        }
        #endif

		//为提高协议处理速度 适应115200通讯波特率 一个循环处理所有字节数据
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
				// 	if (i == eCR) //只在载波口处理
				// 	{
				// 		Result = DoReceProc_13762( Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i] );
				// 	}	
				// }
                #endif
	
				//如果报文已收完且InitPoint()，则不再执行下面
				if( Result == FALSE )
				{
					Serial[i].RXRPoint++;
					if( Serial[i].RXRPoint >= MAX_PRO_BUF_REAL )
					{
						Serial[i].RXRPoint = 0;
					}
				}
				else//接受到完整报文
				{
					if( i == eCR )
					{
						setLedflag(eLedTxRx,eLightAllNone);
						CROverTime = CR_OVER_TIME;//接受到载波数据 刷新超时时间
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

	//处理接收定时
	if( Serial[i].RxOverCount != 0 )
	{
		if( Serial[i].RxOverCount == 1 )
		{
    		api_InitSci( i );
    		Serial[i].RxOverCount = 0;
		}

		//为精确定时，把减减操作移到毫秒中断了
		//Serial[i].RxOverCount --;
	}
}


//--------------------------------------------------
//功能描述:  主循环清零函数
//         
//参数  :   无
//
//返回值:    无   
//         
//备注内容:  每500毫秒调用一次
//--------------------------------------------------
void ProcClrTask(void)
{
	WORD wTmpClearMeterType;

	//wClearMeterType //0x1111--电表清零(数据初始化) 0x2222--恢复出厂参数 0x4444--事件清零 0x8888--需量清零
	if( FPara1->wClearMeterType == 0 )
	{
		return;
	}
	
	//如果参数区1校验错误，则退出，不执行清零，防止新EEPROM上电总在清零
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara1), sizeof(TFPara1), UN_REPAIR_CRC ) == FALSE )
	{	
		return;
	}
	
	wTmpClearMeterType = FPara1->wClearMeterType;

	if( wTmpClearMeterType == eCLEAR_FACTORY )//初始化参数
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
	//发送部分
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		//在接收到一帧后，发送前，要延时一段时间，在此时间内，不允许645工作
		if( Serial[i].ReceToSendDelay != 0 )
		{
			if( Serial[i].ReceToSendDelay != 1 )
			{
				//为延时的准确性，放到毫秒中断进行减了
				//不向下处理
				//Serial[i].ReceToSendDelay --;
				//继续485监视定时
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
	
	//500毫秒任务
	if(api_GetTaskFlag(eTASK_PROTOCOL_ID, eFLAG_500_MS) == TRUE)	
	{
		ProcClrTask();
		// api_ChargingCanTask(0);
	  	api_ClrTaskFlag( eTASK_PROTOCOL_ID, eFLAG_500_MS );
	}
	
	//1秒到
	if( api_GetTaskFlag(eTASK_PROTOCOL_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PROTOCOL_ID,eFLAG_SECOND);

		// api_IapCountDown();

		//没有5伏，不向下处理
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			return;
		}

		// CheckParaDaemon();
		// TimeLinessEventJudge();
		for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
		{
			//监视串口
			if( Serial[i].WatchSciTimer != 0 )
			{
				//串口长时间没有操作，需要初始化
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
			if( LowFactoryTime > (60*30) )//极限值判断
			{
				LowFactoryTime = 0;
			}

			if( LowFactoryTime == 0 )//清低等级厂内模式权限
			{
				api_ClrSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY );
			}
		}
		
/*		#if(SEL_POWER_LINE_CARRIER_OR_GPRS == YES)
		if(NeedResetModul == 1)
		{
			//每天23点13分左右复位一下，上电30秒内就不要复位了
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
					//复位模块
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
	
	//1分钟到
	if( api_GetTaskFlag(eTASK_PROTOCOL_ID,eFLAG_MINUTE) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PROTOCOL_ID,eFLAG_MINUTE);
		
		//没有5伏，不向下处理
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			return;
		}

		if( CROverTime > CR_OVER_TIME )//极限值判断
		{
			CROverTime = CR_OVER_TIME;
		}

		if( CROverTime != 0 )//倒计时
		{
			CROverTime --;
			if (CROverTime==(CR_OVER_TIME-120))//超时两小时
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

		//小时任务
	if( api_GetTaskFlag(eTASK_PROTOCOL_ID, eFLAG_HOUR) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PROTOCOL_ID, eFLAG_HOUR);

		// api_CheckCanBusID();
        
        #if (SEL_SEARCH_METER == YES )
        CheckCycleSInfo();//监视周期搜表参数
        #endif
        
        #if (SEL_TOPOLOGY == YES )
        // CheckTopoPara();
        #endif

		// api_CheckMessageTransPara();
		
		api_GetRtcDateTime(DATETIME_hh,&tHour);
		//判断过零点
		if(tHour == 0)
		{
		    api_ClrRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);//清零698密码错误标志
			api_ClrRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);//清零02级密码错误标志
			api_ClrRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);//清零04级密码错误标志
			api_ClrRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );//清除广播校时一天只能进行一次的标志位
			api_ClrRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );//清除明文广播校故障时一天只能记录一次的标志位
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
	//国标645规约
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

//把数据从报文开始位置 向 以p->ProBuf[0]开始处移
void DoReceMoveData(WORD ProtocolType, TSerial * p)
{
	WORD ProStep, BeginPos;
	WORD i;
	BYTE Buf[MAX_PRO_BUF_REAL+30];
	//国标645规约
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

	//国标645规约
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



//取得缓冲是TSerial中的哪个串口
//返回: FALSE--不是串口缓冲 其它--返回的是串口Serial[]的地址
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
	//获取掉电时间
	// api_GetPowerDownTime((TRealTimer *)&PowerDownTime);
	//掉电不清ram出错 或者 掉电时间与当前时间不是同一天 清密码错误标志
	if( (api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR) == TRUE) || (RealTimer.Day != PowerDownTime.Day) )
	{
        api_ClrRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);//清零698密码错误标志
        api_ClrRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);//清零02级密码错误标志
        api_ClrRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);//清零04级密码错误标志
        api_ClrRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );//清除广播校时一天只能进行一次的标志位
		api_ClrRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );//清除明文广播校故障时一天只能记录一次的标志位
        memset( g_645PassWordErrCounter, 0x00, sizeof(g_645PassWordErrCounter) );
//        g_PassWordErrCounter = 0;   
	}
	
	TmpCommPara0 = CommParaConst.ComModule;
	if( TmpCommPara0 != FPara2->CommPara.ComModule )//载波波特率变化后上电恢复默认值
	{
		if( lib_CheckSafeMemVerify( (BYTE*)FPara2, sizeof(TFPara2), UN_REPAIR_CRC ) == TRUE )//CRC校验不对，不允许操作FPara，防止将错误数据的CRC恢复
		{
			api_WritePara( 1, GET_STRUCT_ADDR( TFPara2, CommPara.ComModule ), 1, &TmpCommPara0 );
			Serial[eCR].OperationFlag = 1;
			CROverTime = CR_OVER_TIME; //上电启动倒计时
		}
	}
}

//---------------------------------------------------------------
//函数功能: 设置清零标志
//
//参数: 
//			eCLEAR_METER_TYPE - 清零标志位
// 					eCLEAR_METER   = 0x1111,	//电表清零
//					eCLEAR_FACTORY = 0x2222		//电表初始化
//					eCLEAR_EVENT   = 0x4444,	//事件清零
//					eCLEAR_MONEY   = 0x6666,	//钱包初始化或参数预置卡
//					eCLEAR_PARA    = 0x7777,	//初始化参数（不初始化冻结、事件、显示列表）
//					eCLEAR_DEMAND  = 0x8888,	//需量清零
// 			BYTE DelayTime_500ms - 清零延时时间 500ms为单位，设置为1表示延时500ms
//                   
//返回值:   TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL api_SetClearFlag( eCLEAR_METER_TYPE ClearType, BYTE DelayTime_500ms )
{
	//参数检查
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
//功能描述  :   厂内初始化协议参数
//参数  : 	无
//返回值: 
//备注内容  :  无
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
//功能描述:     申请数据填充Buf
//
//参数  :     BYTE Ch[in]                     通道选择
//          ePROTOCO_TYPE ProtocolType[in]  协议类型
//
//返回值:      BYTE* buf指针
//
//备注内容:  无
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
//功能描述:     4G模块复位
//
//参数  :     
//
//返回值:      
//
//备注内容:  无
//--------------------------------------------------
void api_ResetModule4g( void )
{
	RESET_PLC_MODUAL;
	// CROverTime = 1440;
}


