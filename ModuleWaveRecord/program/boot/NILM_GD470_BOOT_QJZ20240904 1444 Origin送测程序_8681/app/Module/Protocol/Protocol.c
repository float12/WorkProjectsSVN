#include "AllHead.h"
#include "Dlt698_45.h"


//接受超时时间，单位，毫秒，也就是在500毫秒后若没有接受到新的数据，则清接受缓冲
const WORD MAX_RX_OVER_TIME = 500/1;//定时器是1ms
//接收到发送的延时，毫秒为单位，接收到有效帧后，延时此时间在发送（认为程序处理不耗费时间）
const WORD RECE_TO_SEND_DELAY = 25/10+1;//定时器是10ms

//串口缓冲，0－第一485；1－红外；2－第二485
TSerial Serial[MAX_COM_CHANNEL_NUM];
WORD FactoryTime;//打开厂内模式时间
WORD LowFactoryTime;//打开低等级厂内模式时间

BYTE APDUResponseBuf[MAX_COM_CHANNEL_NUM][MAX_APDU_SIZE+EXTRA_BUF];
ePROTOCO_TYPE LAST_PROTOCO_TYPE;

//新板初始化串口通讯指针
void api_InitSciStatus( BYTE i )
{
	InitPoint(&Serial[i]);

	Serial[i].TXPoint = 0;
	Serial[i].SendLength = 0;

	Serial[i].ReceToSendDelay = 0;

	Serial[i].BroadCastFlag = 0;

    Serial[i].SendingFlag = 0;
}

//i -- 要初始化的端口号
void api_InitSubProtocol(WORD i)
{
	if(Serial[i].RxOverCount != 1)
	{
		Serial[i].BeginPoint = 0xff;
		Serial[i].byLastRXDCon = 0x91;//保存上次通信电表应答控制码，供后面读后续帧判断用，初始化为默认上次正常读取
	}

	api_InitSciStatus( i );//完成协议初始化工作
    InitAPDUBufFlag( i );       //TODO:  为什么注释 需要打开吗？
}
//初始化规约读写指针,初始化、接收超时、规约处理完三种情况调用，
void InitPoint(TSerial * p)
{

	p->ProStepDlt645 = 0;
	p->BeginPosDlt645 = 0;
	p->Dlt645DataLen = 0;

	InitPoint_Dlt698(p);

	p->RXWPoint = 0;
	p->RXRPoint = 0;

	p->RxOverCount = 0;


}


void DoSendProc( BYTE Channel )
{
    SerialMap[Channel].SCIBeginSend(Channel);
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
	WORD ReceNum;
	BYTE Result = FALSE;// ComResult;

	//正常处理
	if((Serial[i].SendingFlag == 0) && (Serial[i].SendLength > Serial[i].TXPoint))
	{
		DoSendProc( i );
	}
	else
	{
		// if( Serial[0].RxOverCount == 0)//载波通道无数据发送 无分帧 且当前未接受到新数据后，进行主动上报判断
		// {
		// 	ComResult = api_ManageCOM();
		// 	if( ComResult == TRUE )//上报成功提前返回
		// 	{
		// 		return ;
		// 	}
		// }
		
		{
			//发送完成后才允许接
			ReceNum = Serial[i].RXWPoint;

			if( ReceNum != Serial[i].RXRPoint )
			{
				Serial[i].RxOverCount = (DWORD)MAX_RX_OVER_TIME;

                #if( SEL_DLT645_2007 == YES )
                Result = DoReceProc_Dlt645( Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i] );
                #else
                Result = FALSE;
                #endif

				// if( Result == FALSE )
				// {
				// 	Result = DoReceProc_DLT698( Serial[i].ProBuf[Serial[i].RXRPoint], &Serial[i] );
				// }
				

				//如果报文已收完且InitPoint()，则不再执行下面
				if( Result == FALSE )
				{
					Serial[i].RXRPoint++;
					if( Serial[i].RXRPoint >= MAX_PRO_BUF )
					{
						Serial[i].RXRPoint = 0;
					}
				}
			}
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


void api_ProtocolTask(void)
{
    WORD i;

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
		
		//api_ProSciTimer( i ); //广播校时使用
	}

	//500毫秒任务
	if(api_GetTaskFlag(eTASK_PROTOCOL_ID, eFLAG_500_MS) == TRUE)
	{
		//ProcClrTask();  //698
	  	api_ClrTaskFlag( eTASK_PROTOCOL_ID, eFLAG_500_MS );
	}
	
	//1秒到
	if( api_GetTaskFlag(eTASK_PROTOCOL_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PROTOCOL_ID,eFLAG_SECOND);
		
		if((ManageComInfo.DelayTime != 0)&&(ManageComInfo.DelayTime != 0xFFFF))
		{
			ManageComInfo.DelayTime--;
		}
		
		if((ManageComInfo.EventReportOverTime != 0)&&(ManageComInfo.EventReportOverTime != 0xFFFF))
		{
			ManageComInfo.EventReportOverTime--;
		}

	    //TimeLinessEventJudge( );  //698
	    
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
		bySum += p->ProBuf[(BeginPos+i)%MAX_PRO_BUF];
	}
	return bySum;
}

//把数据从报文开始位置 向 以p->ProBuf[0]开始处移
void DoReceMoveData(WORD ProtocolType, TSerial * p)
{
	WORD ProStep, BeginPos;
	WORD i;
	BYTE * pAllocBuf;
	//国标645规约
	if( ProtocolType == PRO_NO_SPECIAL )
	{
		ProStep = p->ProStepDlt645;
		BeginPos = p->BeginPosDlt645;
	}

	else if( ProtocolType == PRO_DLT698_45 )
	{
		ProStep = p->ProStepDlt698_45;
		BeginPos = p->BeginPosDlt698_45;
	}
	else
	{
		return;
	}

	if( BeginPos == 0 )
	{
		return;
	}
	if( ProStep >= MAX_PRO_BUF )
	{
		return;
	}	

	if( BeginPos <= (MAX_PRO_BUF-ProStep) )
	{
		for( i=0; i<ProStep; i++ )
		{
			 p->ProBuf[i] = p->ProBuf[BeginPos+i];
		}
	}
	else
	{
		pAllocBuf = malloc( ProStep );// 申请缓冲

		for( i=0; i<ProStep; i++ )
		{
			 pAllocBuf[i] = p->ProBuf[(BeginPos+i)%MAX_PRO_BUF];
		}

		memcpy( (void*)p->ProBuf, (void*)pAllocBuf, ProStep );

		// 释放缓冲
		free(pAllocBuf);
	}
}



//取得缓冲是TSerial中的哪个串口
//返回: FALSE--不是串口缓冲 其它--返回的是串口Serial[]的地址
TSerial *GetSerial(BYTE *pBuf)
{
	BYTE i;

	i = 0;

	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( (pBuf >= Serial[i].ProBuf)&&(pBuf < (Serial[i].ProBuf+MAX_PRO_BUF)) )
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

   // PowerUpDlt698();
    
}

//-----------------------------------------------
//功能描述  :   厂内初始化协议参数
//参数  : 	无
//返回值: 
//备注内容  :  无
//-----------------------------------------------
void FactoryInitProtocol( void )
{

   // FactoryInitDLT698();
   
}

//--------------------------------------------------
//功能描述:     申请数据填充Buf
//         
//参数  :     BYTE Ch[in]                     通道选择
//          ePROTOCO_TYPE PROTOCO_TYPE[in]  协议类型
//
//返回值:      BYTE* buf指针
//         
//备注内容:  无
//--------------------------------------------------
BYTE* AllocResponseBuf(BYTE Ch,         ePROTOCO_TYPE PROTOCO_TYPE )
{
    if( LAST_PROTOCO_TYPE != PROTOCO_TYPE )
    {

        if( PROTOCO_TYPE ==ePROTOCO_698 )
        {
           //InitDLT645Flag((eSERIAL_TYPE)Ch);
        }

       LAST_PROTOCO_TYPE = PROTOCO_TYPE;
    } 
    
    return (BYTE*)&APDUResponseBuf[Ch];
}





