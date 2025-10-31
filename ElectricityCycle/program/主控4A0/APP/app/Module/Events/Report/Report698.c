//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	魏灵坤 张玉猛
//创建日期	2016.10.26
//描述		698规约上报
//修改记录	
//---------------------------------------------------------------------- 

#include "AllHead.h"

#if ( SEL_DLT698_2016_FUNC == YES)

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define LostPowerEndTimeThreshold	3540//掉电事件上报，事件结束时间阈值
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern const TSubEventInfoTab	SubEventInfoTab[];
extern const TEventInfoTab EventInfoTab[];
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
__no_init TReportMode		ReportMode;		//事件上报标识 bit:事件有效标识

__no_init TReportData 		ReportData[MAX_COM_CHANNEL_NUM];//不保存红外通道
__no_init TReportData 		ReportDataBak[MAX_COM_CHANNEL_NUM];//不包括红外通道备份，新增事件列表只载波通道有用
__no_init TReportTimes 		ReportTimes;//主动上报次数，掉电存E2

TReportTimes 		LostPowerReportTimes;//掉电主动上报次数，上电清零，不存E2

WORD ReportOverTime;//主动上报延时
WORD ReportPowerDownTime;//掉电事件主动上报延时

WORD EventLostPowerEndTime; //掉电事件结束时间，累加（单位秒）

BYTE LostPowerReportBuf[MAX_PRO_BUF+30];
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 	上报次数加1
//
//参数: 		无
//        	
//返回值:		无
//			
//备注:
//-----------------------------------------------
static void ReportTimesReduce1( eActiveReportMode ActiveReportMode  )
{
	if( ActiveReportMode == eActiveReportPowerDown )         
	{
		if( LostPowerReportTimes.Times != 0 )
		{
			if( LostPowerReportTimes.Times > 3 )//极限值判断
			{
				LostPowerReportTimes.Times = 0;
			}
			else if( LostPowerReportTimes.Times != 0 )
			{
				LostPowerReportTimes.Times --; 
			}
			LostPowerReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD));

			//次数为0的时候清空buf
			if( LostPowerReportTimes.Times == 0 )
			{
				memset(LostPowerReportBuf,0x00,sizeof(LostPowerReportBuf));
			}
		}
	}
	else
	{
		if( ReportTimes.Times != 0 )
		{
			if( ReportTimes.Times > 3 )//极限值判断
			{
				ReportTimes.Times = 0;
			}
			else if( ReportTimes.Times != 0 )
			{
				ReportTimes.Times --; 
			}
			ReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
		}
	}

	

}

//-----------------------------------------------
//函数功能: 	清零上报次数
//
//参数: 		Mode[in] :	0x00:清掉电上报次数
//						0x01:清主动上报次数
//						0xFF:清上报次数
//        	
//返回值:		无
//			
//备注: 
//-----------------------------------------------
static void ClearReportTimes(BYTE Mode )
{
	if( Mode == eActiveReportPowerDown )
	{
		memset( &LostPowerReportTimes, 0x00, sizeof(LostPowerReportTimes) );
		LostPowerReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
		memset(LostPowerReportBuf,0x00,sizeof(LostPowerReportBuf));
	}
	else if( Mode == eActiveReportNormal )
	{
		memset( &ReportTimes, 0x00, sizeof(ReportTimes) );
		ReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
	}
	else
	{
		memset( &ReportTimes, 0x00, sizeof(ReportTimes) );
		ReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
		memset( &LostPowerReportTimes, 0x00, sizeof(LostPowerReportTimes) );
		LostPowerReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
		memset(LostPowerReportBuf,0x00,sizeof(LostPowerReportBuf));
	}
	
}

//-----------------------------------------------
//函数功能: 	重置上报次数 3次
//
//参数: 		Type 0
//        	
//返回值:		无
//			
//备注:
//-----------------------------------------------
static void ResetReportTimes( eActiveReportMode ActiveReportMode )
{
	if( ActiveReportMode == eActiveReportPowerDown)
	{	
		ReportPowerDownTime = 0;
		LostPowerReportTimes.Times = 3; 
		LostPowerReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
	}
	else
	{
		ReportOverTime = 0;//初始化上报超时时间
		ReportTimes.Times = 3; 
		ReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
	}

	
}

//-----------------------------------------------
//函数功能: 	主动上报时间减减
//
//参数: 		无
//        	
//返回值:		无
//			
//备注:主动上报时间处理，放在systick中断中，越减短越好
//-----------------------------------------------
void ReportTimeMinus( )
{
	 if( ReportOverTime > 0 )//主动上报延时
	{
		ReportOverTime --;
	}

	if( ReportPowerDownTime > 0 )//掉电主动上报延时
	{
		ReportPowerDownTime --;
	}

}
//-----------------------------------------------
//函数功能: 	清零上报ram数据包括上报次数-上报数据备份
//
//参数: 		BYTE Ch[in] 通道 0xff 全通道 
//返回值:		无
//			
//备注:
//-----------------------------------------------
static void ClearReportDataRam( BYTE Ch )
{
	BYTE i;
	
	//清零上报数据
	for( i=0 ; i < MAX_COM_CHANNEL_NUM; i++ )
	{
		if( (Ch == 0xff) || (i == Ch) )
		{
			//清除对应的上报数据
			memset((BYTE *)&(ReportData[i]), 0x00, sizeof(TReportData));
			memset((BYTE *)&(ReportData[i].Index), 0xFF, sizeof(ReportData[0].Index));
			ReportData[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[i],sizeof(TReportData)-sizeof(DWORD));		
			//清楚对应的跟随上报状态字备份
			memset((BYTE *)&(ReportDataBak[i]), 0x00, sizeof(TReportData));
			//清零上报数据备份
			memset((BYTE *)&(ReportDataBak[i].Index), 0xFF, sizeof(ReportDataBak[0].Index));
			ReportDataBak[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD));
		}

	}
}


//-----------------------------------------------
//函数功能: 	上报ram数据校验
//
//参数: 		BYTE Ch[in] 校验指定通道的数据
//        	
//返回值:		无
//			
//备注:
//-----------------------------------------------
static void ReportDataCheck( BYTE Ch )
{
	BYTE i;
	
	//清零上报数据
	for( i=0 ; i < MAX_COM_CHANNEL_NUM; i++ )
	{
		if( (Ch == 0xff) || (i == Ch) )
		{
			//上报数据crc错误 数据清零 不从e2恢复 如果e2恢复有可能会导致重复上报
			if( ReportData[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportData[i],sizeof(TReportData)-sizeof(DWORD) ))
			{
				ClearReportDataRam( i );
			}
			
			//备份上报数据错误 备份数据清零
			if( ReportDataBak[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD) ))
			{
				//清零上报数据备份
				memset((BYTE *)&(ReportDataBak[i]), 0x00, sizeof(TReportData));
				memset((BYTE *)&(ReportDataBak[i].Index), 0xFF, sizeof(ReportDataBak[0].Index));
				ReportDataBak[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD));
			}

			if( i == eCR)//载波通道进行上报次数检测
			{
				//上报次数错误 清零上报次数
				if( ReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD) ))
				{
					ClearReportTimes(eActiveReportNormal);
				}
				//上报次数错误 清零上报次数
				if( LostPowerReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD) ))
				{
					ClearReportTimes(eActiveReportPowerDown);
				}

			}
		}
	}
}

//-----------------------------------------------
//函数功能: 	上报参数校验
//
//参数: 		BYTE Ch[in] 校验指定通道的数据
//        	
//返回值:		无
//			
//备注:
//-----------------------------------------------
static void ReportParaCheck( void )
{
	BYTE i,j,k;
	WORD Result;
	
	//模式字校验
	if( ReportMode.CRC32 != lib_CheckCRC32((BYTE*)&ReportMode,sizeof(ReportMode)-sizeof(DWORD)) )
	{
		Result = api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode);
		if( Result == FALSE )
		{
			ReportMode.bMeterReportFlag = MeterReportFlagConst;
			ReportMode.bMeterActiveReportFlag = MeterActiveReportFlagConst;
			//跟随上报状态字 上报方式
			ReportMode.byReportStatusMethod = MeterReportStatusFlagConst;
			//初始化为默认跟随上报
			memset( &ReportMode.byReportMethod, 0xff, sizeof(ReportMode.byReportMethod) );
			//事件主动上报方式
			for( i = 0; i< ReportActiveMethodConst[0]; i++ )
			{
				j = (ReportActiveMethodConst[i+1]%eNUM_OF_EVENT_PRG) / 8;
				k = ReportActiveMethodConst[i+1] % 8;
				ReportMode.byReportMethod[j] &= ~(0x01<<k);
			}

			memcpy( (void *)&(ReportMode.byFollowReportMode[0]), FollowReportModeConst, GET_STRUCT_MEM_LEN(TReportMode, byFollowReportMode) );//20150400 电能表跟随上报模式字
			memset( (void *)&(ReportMode.ReportChannelOAD[0]), 0x00, (sizeof(DWORD)*MAX_COM_CHANNEL_NUM) );

			//写上报通道
			for(i=0;i<FollowReportChannelConst[0];i++)
			{
				ReportMode.ReportChannelOAD[i]=ChannelOAD[FollowReportChannelConst[i+1]];
			}
			memset( &ReportMode.byReportMode, 0x00, sizeof(ReportMode.byReportMode) );
			//事件上报时刻
			for( i = 0; i< ReportModeConst[0].EventIndex; i++ )
			{
				ReportMode.byReportMode[ReportModeConst[i+1].EventIndex] = ReportModeConst[i+1].ReportMode;
			}
			ReportMode.CRC32 = lib_CheckCRC32((BYTE*)&ReportMode,sizeof(ReportMode)-sizeof(DWORD));		
		}
	}

}

//-----------------------------------------------
//函数功能: 置位跟随上报状态字对位的状态位
//
//参数: 
//			StatusNo[in]		系统状态号
//                    
//返回值:  	无
//
//备注: 只有在该状态位由0变1时才调用本函数  
//-----------------------------------------------
void api_SetFollowReportStatus(BYTE StatusNo)
{
	BYTE i, j,Ch;
	
	ASSERT( StatusNo < eMAX_FOLLOW_REPORT_STATUS_NUM, 0 );
	
	i = (StatusNo%eMAX_FOLLOW_REPORT_STATUS_NUM) / 8;
	j = StatusNo % 8;
	
	if( ReportMode.byFollowReportMode[i] & (0x01<<j) )
	{
		ReportDataCheck( 0xff );
		
		for( Ch=0; Ch < MAX_COM_CHANNEL_NUM; Ch++ )
		{
//			if( Ch == eIR )
//			{
//				continue;
//			}
			
			ReportData[Ch].Status[i] |= (0x01<<j);
			ReportData[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[Ch],sizeof(TReportData)-sizeof(DWORD));
		}
		
		if( ReportMode.byReportStatusMethod == 0 )//上报状态字上报方式为主动上报
		{
			//重置上报次数 重新进行上报
			ResetReportTimes( eActiveReportNormal );
		}
	}
}

//-----------------------------------------------
//函数功能: 设置新增事件列表
//
//参数: 
//		inSubEventIndex[in]:	时间索引号
//  	BeforeAfter[in]:		EVENT_START	BIT0	
//  	                		EVENT_END	BIT1
//返回值:	无
//			
//备注:3320的属性2，不是发生结束都具备的输入 EVENT_START+EVENT_END
//-----------------------------------------------
void SetReportIndex( BYTE inSubEventIndex, BYTE BeforeAfter )
{
	BYTE i,j,n,m,k,tEventIndex;
	BYTE Index[SUB_EVENT_INDEX_MAX_NUM+10];

	
	if( inSubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
	{
		return;
	}

    if( ( inSubEventIndex == eSUB_EVENT_PRG_CLEAR_METER )//公钥下电表清零不进行上报
    && ( api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == TRUE ) )
    {
		return;
    }
    
    tEventIndex = SubEventInfoTab[inSubEventIndex].EventIndex;

	//是否为需上报OI
	if( (ReportMode.byReportMode[tEventIndex]&BeforeAfter) == 0x00 )
	{
		return;
	}
	
	ReportDataCheck( 0xff );
	
	m = ( tEventIndex % eNUM_OF_EVENT_PRG ) / 8;
	k = ( tEventIndex % 8 );

	//添加新增事件列表不要判断跟随上报标志及事件上报标识				
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		n = 1;
		
	    if( i == eIR )//红外通道不进行置位，直接跳过
	    {
           continue;
	    }
	    if((inSubEventIndex == eSUB_EVENT_LOST_POWER) //直通表的掉电事件事件开始主动上报，不添加3320，
	    && (BeforeAfter == EVENT_START) && ( MeterVoltageConst == METER_220V )
	    &&( (ReportMode.byReportMethod[m] & (0x01<<k)) == 0 ))
	    {
			continue;
	    }

		if(	  (inSubEventIndex == eSUB_EVENT_LOST_POWER)&&(BeforeAfter == EVENT_END)
			&&( (ReportMode.byReportMethod[m] & (0x01<<k)) == 0 )
			&&( api_GetRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG) == FALSE) )
		{
			continue;
		}

	    memcpy( Index, ReportData[i].Index, SUB_EVENT_INDEX_MAX_NUM );//备份数据
		memset( ReportData[i].Index, 0xff, sizeof(ReportData[0].Index) );//初始化重新进行排序
		ReportData[i].Index[0] = inSubEventIndex;
		
		for(j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++)
		{

			if((Index[j] != 0xff) && (Index[j] != inSubEventIndex ) )//向前进行依次排列
			{
				if( n < SUB_EVENT_INDEX_MAX_NUM )//防止数据超限 理论不会出现
				{
					ReportData[i].Index[n] = Index[j];
					n++;
				}
			}
			
		}
		
	    ReportData[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[i],sizeof(TReportData)-sizeof(DWORD));		

	    #if( SEL_DLT645_2007 == YES )
        api_AddReportTimes( i,0x00, inSubEventIndex );
        #endif
	}
	
	if( (ReportMode.byReportMethod[m] & (0x01<<k)) == 0 )//为主动上报
	{
		if( inSubEventIndex == eSUB_EVENT_LOST_POWER )
		{
			if( BeforeAfter & EVENT_END )//掉电结束判标志
			{
				
				//掉电发生上报，用的是恢复之前的时间，用掉电不丢失标志，
				if(api_GetRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG) == TRUE)
				{
					api_ClrRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG);//结束清标志
					ResetReportTimes( eActiveReportPowerDown);	//重置掉电上报次数进行上报

				}
				//掉电事件结束时，就要清掉电结束时间
				EventLostPowerEndTime =0;
			}
			else if( BeforeAfter & EVENT_START )//掉电发生判时间
			{
				if( MeterVoltageConst == METER_220V )//只有220V电表进行上报
				{
					ClearReportTimes( eActiveReportPowerDown);	//重置掉电上报次数进行上报
				
					//判断是否大于1小时上报
					//掉电结束上报判时间，是因为要防止退出低功耗后，上电时发生上报的问题
					if(EventLostPowerEndTime > LostPowerEndTimeThreshold)
					{
						ResetReportTimes( eActiveReportPowerDown);	//重置掉电上报次数进行上报
						
						ReportPowerDownTime = 501;//掉电不进入低功耗的上报增加5秒延时,5010ms
					}
				}    
			}
		}
		else
		{
			ResetReportTimes( eActiveReportNormal );	//重置正常上报次数进行上报、
		}
	}
}

//-----------------------------------------------
//函数功能: 清指定通道新增事件列表中的指定事件
//
//参数: 
//		Ch[in]:				通道号
//  	inSubEventIndex[in]:事件索引号	
//  	                	
//返回值:	无
//			
//备注:单通道
//-----------------------------------------------
void ClrReportIndexChannel( BYTE Ch, BYTE inSubEventIndex )
{
	BYTE j,n;
	BYTE Index[SUB_EVENT_INDEX_MAX_NUM+10];
	
	if( inSubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
	{
		return;
	}
	
	if( Ch >= MAX_COM_CHANNEL_NUM )
	{
		return;
	}
	
	ReportDataCheck( Ch );
		
	//进行重新排序
	memcpy( Index, ReportData[Ch].Index, SUB_EVENT_INDEX_MAX_NUM );//备份数据
	memset( ReportData[Ch].Index, 0xff, sizeof(ReportData[0].Index) );//初始化重新进行排序
	n = 0;
	
	for(j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++)
	{
	
		if((Index[j] != 0xff) && (Index[j] != inSubEventIndex ) )//向前进行依次排列
		{
			ReportData[Ch].Index[n] = Index[j];
			n++;
		}
	}
	
	//清除645的上报次数
	#if( SEL_DLT645_2007 == YES )
	ClearReport645Times( Ch, 0x00, inSubEventIndex );
	#endif

	ReportData[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[Ch],sizeof(TReportData)-sizeof(DWORD));		
}
//-----------------------------------------------
//函数功能: 清所有通道新增事件列表中的指定事件
//
//参数: 
//  	inSubEventIndex[in]:事件索引号	
//  	                	
//返回值:	无
//			
//备注:所有通道
//-----------------------------------------------
void ClrReportAllChannelIndex(  BYTE inEventIndex )
{
	BYTE i,Ch;
	BYTE tSubEventIndex;

	if(inEventIndex >= eNUM_OF_EVENT_PRG)//此处限制应为事件与编程类事件总数；
	{
		return;
	}
	for( Ch = 0 ;Ch < MAX_COM_CHANNEL_NUM ; Ch++ )
	{
//		if( Ch == eIR )
//		{
//			continue;
//		}
		//查找EventIndex对应的SubEventIndex
		for( i = 0 ; i < SUB_EVENT_INDEX_MAX_NUM ; i++ )
		{
			if( inEventIndex == SubEventInfoTab[i].EventIndex )
			{
				tSubEventIndex = SubEventInfoTab[i].SubEventIndex;
				//清当前通道的指定新增事件列表中的指定事件。
				ClrReportIndexChannel( Ch, tSubEventIndex );
			}

		}
	}		
}

//-----------------------------------------------
//函数功能: 清上报数据
//
//参数: 	无
//  	                	
//返回值:	无
//			
//备注:清所有通道的新增事件列表、指针
//-----------------------------------------------
void ClearReport( BYTE Type )
{
	ClearReportDataRam( 0xff );
	ClearReportTimes( 0xff );//清零上报次数
	
	//清除645的上报次数
	#if( SEL_DLT645_2007 == YES )
	ClearReport645Times( 0, 0x55, 0 ); 
	#endif

    if( Type == eEXCEPT_CLEAR_METER )//不清电表清零 产生新的电表主动上报
    {
        #if( SEL_PRG_INFO_CLEAR_METER == YES ) //电能表清零事件
        SetReportIndex( eSUB_EVENT_PRG_CLEAR_METER, EVENT_START+EVENT_END );
        #endif
    }
    else if( Type == eEXCEPT_CLEAR_METER_CLEAR_EVENT )
    {
        #if( SEL_PRG_INFO_CLEAR_EVENT == YES )//电能表事件清零事件
        SetReportIndex( eSUB_EVENT_PRG_CLEAR_EVENT, EVENT_START+EVENT_END );
        #endif
    }
}

//-----------------------------------------------
//函数功能: 根据事件索引号计算对应OAD
//
//参数: 	
//		Index[in]:	事件索引号
//		Buf[out]：	计算出的单个OAD 
//           	
//返回值:	无
//			
//备注:
//-----------------------------------------------
static void CalReportOad(BYTE Index, BYTE *Buf)
{
	BYTE Phase;
	TFourByteUnion tdw;
	
	if(Index >= SUB_EVENT_INDEX_MAX_NUM)
	{
		return;
	}
	
	Phase = SubEventInfoTab[Index].Phase;
	tdw.w[1] = EventInfoTab[ SubEventInfoTab[Index].EventIndex].OI;
	FindThisEventReallyOI(&tdw.w[1]);
	if( (tdw.w[1]>0x3008) && (tdw.w[1]!=0x300b) && (tdw.w[1]!=0x303B) )
	{
		//属性7
		tdw.b[1] = 0x02;
	}
	else
	{
		//属性24
		tdw.b[1] = Phase+0x06;
	}
	
	tdw.b[0] = 0x00;
	
	lib_ExchangeData(Buf,tdw.b,sizeof(DWORD));
}

//-----------------------------------------------
//函数功能: 获取跟随上报通道标志
//
//参数: 	   Ch[in]:		通道号
//           	
//返回值:	   TRUE-此通道支持上报	FALSE--此通道不支持上报
//			
//备注:
//-----------------------------------------------
BOOL api_GetReportChannelStatus( BYTE Ch )
{
	BYTE i;
	
	//判断通道Ch是否属于4300电气设备的属性10. 上报通道array OAD
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		//0x000209F2,	//eCR 
		if( ((ReportMode.ReportChannelOAD[i]<<8) == 0x0209f200) &&( (ChannelOAD[Ch]<<8) == 0x0209f200) )
		{
			break;
		}	
		
		if( ReportMode.ReportChannelOAD[i] == ChannelOAD[Ch] )
		{
			break;
		}
	}

	if( i >= MAX_COM_CHANNEL_NUM )
	{
		return FALSE;		
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 备份上报列表
//
//参数: 	
//           	
//返回值:	
//			
//备注:
//-----------------------------------------------
void api_BackupReportIndex( void )
{
	BYTE i,j,k,Offset,n;
	eEVENT_INDEX EventIndex;

	n = 0;
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		
		if( ReportData[eCR].Index[i] != 0xff )
		{
			EventIndex = SubEventInfoTab[ReportData[eCR].Index[i]].EventIndex;
			j = ( EventIndex % eNUM_OF_EVENT_PRG ) / 8;
			k = ( EventIndex % 8 );
			
			if( (ReportMode.byReportMethod[j] & (0x01<<k)) == 0 )//为主动上报
			{
				ReportDataBak[eCR].Index[n] = ReportData[eCR].Index[i];
				n++;
			}	

		}
		else//SetReportIndex 中已经保证了无效0xff仅出现在结尾 ,所以这里一旦发现0xff就应该退出循环
		{
			break;
		}
	}
	//计算crc
	ReportDataBak[eCR].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[eCR],sizeof(TReportData)-sizeof(DWORD));

}


//-----------------------------------------------
//函数功能: 获取跟随上报OAD
//
//参数: 	
//		Type[in]:	0--跟随上报调用	1--EVENTOUT使用
//		Ch[in]:		通道号
//		Buf[out]：	计算出的单个OAD 
//           	
//返回值:	TRUE--有跟随上报	FALSE--无跟随上报
//			
//备注:
//-----------------------------------------------
BOOL GetFirstReportOad(BYTE Type, BYTE Ch, BYTE *Buf)
{
	BYTE i,j,k;
	eEVENT_INDEX EVENT_INDEX;
	
	ReportDataCheck(Ch);//上报数据校验
	ReportParaCheck();//上报参数校验
	
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		if( ReportData[Ch].Index[i] != 0xff )
		{
			EVENT_INDEX = SubEventInfoTab[ReportData[Ch].Index[i]].EventIndex;
			j = ( EVENT_INDEX % eNUM_OF_EVENT_PRG ) / 8;
			k = ( EVENT_INDEX % 8 );

			if( ReportMode.byReportMethod[j] & (0x01<<k) )//置1跟随上报
			{
				if(Type == 0)
				{
					CalReportOad(ReportData[Ch].Index[i], Buf);
				}
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
//-----------------------------------------------
//函数功能: 读取跟随上报状态字、模式字
//
//参数:		BYTE Ch[in]  通道选择
//			BYTE Mode[in]：	   模式 0： 代表读取电表状态字           1：代表读电表模式字 2:返回数据判断跟随上报标识-用于跟随上报
//          BYTE *pBuf[in]：    输入buf      
//         
//返回值:    返回变量长度 0x0000:数据长度不够  0x8000: 出现错误
//			
//备注: 
//-----------------------------------------------
WORD api_ReadFollowReportStatus_Mode( BYTE Ch, BYTE Mode,BYTE *pBuf )
{
	if( Ch == 0x55 )//为主动上报准备 内部读取载波通道
	{
		Ch = eCR;
	}
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return 0x8000;
    }
    
	if( Mode == 0 )
	{
		ReportDataCheck( Ch );
		memcpy(pBuf, (BYTE *)&(ReportData[Ch].Status), sizeof(ReportData[Ch].Status));
		//备份上报数据
		memcpy( ReportDataBak[Ch].Status, ReportData[Ch].Status, sizeof(ReportData[Ch].Status) );
		ReportDataBak[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[Ch],sizeof(TReportData)-sizeof(DWORD));
        return 4;
	}
	else if( Mode == 1 )
	{
		memcpy(pBuf, (BYTE *)&(ReportMode.byFollowReportMode[0]), 4 );//20150400 电能表跟随上报模式字
        return 4;
	}
	else
	{
        return 0x8000;
	}
}

//-----------------------------------------------
//函数功能: 获取新增事件列表OAD
//
//参数: 	
//		GetReportListType[in]	0:全部		1：主动上报
//		Ch[in]:					通道号
//		Len[in]:				输入数据长度
//		Buf[out]：				保存数据的缓冲
//           	
//返回值:	返回数据长度 0x8000--输入Buf长度不足
//			
//备注:新增事件列表 属性2
//-----------------------------------------------
WORD GetReportOadList( eGetReportListType GetReportListType, BYTE Ch, WORD Len, BYTE *Buf)
{
	BYTE i,j,k;
	BYTE Offset;
	eEVENT_INDEX EventIndex;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return 0x8000;
    }
    
	ReportDataCheck(Ch);//上报数据校验
	ReportParaCheck();//上报参数校验
	
	Offset = 0;
	
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		if( (Offset+sizeof(DWORD)) > Len )
		{
			return 0x8000;
		}
		
		if( ReportData[Ch].Index[i] != 0xff )
		{
			if( GetReportListType == eGetReportActiveList )
			{
				EventIndex = SubEventInfoTab[ReportData[Ch].Index[i]].EventIndex;
				j = ( EventIndex % eNUM_OF_EVENT_PRG ) / 8;
				k = ( EventIndex % 8 );
				
				if( (ReportMode.byReportMethod[j] & (0x01<<k)) == 0 )//为主动上报
				{
					CalReportOad(ReportData[Ch].Index[i], Buf+Offset);
					Offset += sizeof(DWORD);
				}	
			}
			else
			{
				CalReportOad(ReportData[Ch].Index[i], Buf+Offset);
				Offset += sizeof(DWORD);
			}
		}
		else//  SetReportIndex 中已经保证了无效0xff仅出现在结尾 ,所以这里一旦发现0xff就应该退出循环
		{
			break;
		}
	}
	
	return Offset;
}

//-----------------------------------------------
//函数功能: 获取需上报事件对象列表 OI
//
//参数: 	
//		Len[in]:	输入数据长度
//		Buf[out]：	array OI 
//           	
//返回值:	返回数据长度 0x8000--输入Buf长度不足
//			
//备注:新增事件列表3320 属性3 需上报事件对象列表
//-----------------------------------------------
WORD GetReportOIList(WORD Len, BYTE *Buf)
{
	BYTE i;
	WORD Offset;
	TTwoByteUnion tw;
	
	Offset = 0;
	
	for(i=0; i<eNUM_OF_EVENT_PRG; i++)
	{
		if(ReportMode.byReportMode[i] & 0x03)
		{
			if( (Offset+sizeof(DWORD)) > Len )
			{
				return 0x8000;
			}
			
			tw.w = EventInfoTab[i].OI;
			FindThisEventReallyOI(&tw.w);
			lib_ExchangeData(Buf+Offset, tw.b, sizeof(WORD));
			Offset += sizeof(WORD);
		}
	}
	
	return Offset;
}

//-----------------------------------------------
//函数功能: 设置上报通道
//
//参数: 	
//		byIndex[in]:	通道索引，对应ReportChannelOAD[0]数组下标，0--所有上报通道 1--第1个上报通道
//		Num[in]:		设置几个上报通道,如果byIndex不为0，则只能设置1个上报通道
//		pBuf[in]:		array OAD
//       	
//返回值:	TRUE/FALSE
//			
//备注:4300电气设备的属性10 上报通道array OAD
//-----------------------------------------------
BOOL SetReportChannel( BYTE byIndex, BYTE Num, BYTE *pBuf )
{
	BYTE i, Flag, bySN;
	
	if( Num > MAX_COM_CHANNEL_NUM )
	{
		return FALSE;
	}
		
	Flag = 0;
	bySN = 0;
	if( byIndex != 0 )
	{
		bySN = byIndex-1;	
		Num = 1;
	}	
	if( memcmp( (void *)pBuf, (void*)&(ReportMode.ReportChannelOAD[bySN]), (Num*4) ) == 0 )//数据相同
	{
		if( byIndex == 0 )//如果是一次性设置所有的几个上报通道
		{
			if( Num != MAX_COM_CHANNEL_NUM )
			{
				for( i=Num;i<MAX_COM_CHANNEL_NUM; i++ )
				{
					if( ReportMode.ReportChannelOAD[i] != 0 )
					{
						ReportMode.ReportChannelOAD[i] = 0;//多余的上报通道OAD置0
						Flag = 1;
					}	
				} 
			}
		}	
	}
	else
	{
		if( byIndex == 0 )
		{
			//4300	电气设备 的属性10	上报通道	array OAD	
			memset( (void *)&(ReportMode.ReportChannelOAD[0]), 0x00, (sizeof(DWORD)*MAX_COM_CHANNEL_NUM) );		
		}
		Flag = 1;
		memcpy( (void *)&(ReportMode.ReportChannelOAD[bySN]), (void *)pBuf, (Num*4) );
	}		
	if( Flag != 0 )
	{
		return api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode);
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读取上报通道
//
//参数: 		
//			pBuf[in]:		array OAD
//       	
//返回值:	数据长度
//			
//备注:4300电气设备的属性10 上报通道array OAD
//-----------------------------------------------
WORD ReadReportChannel( BYTE *pBuf )
{
	BYTE i;
	
	for( i=0;i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( ReportMode.ReportChannelOAD[i] == 0 )
		{
			break;					
		}	
	}
	if( i == 0 )		 
	{
		return FALSE;
	}
	memcpy( (void *)pBuf, (BYTE * )&(ReportMode.ReportChannelOAD[0]),(sizeof(DWORD)*i) );

	return (sizeof(DWORD)*i);
}

//-----------------------------------------------
//函数功能: 设置允许上报标志
//
//参数: 
//  		bMeterReportFlag[in]:	TRUE/FALSE
//			Type					eFollowReport/eActiveReport
//返回值:	TRUE/FALSE
//			
//备注:4300电气设备 属性7.8 允许跟随上报,允许主动上报
//-----------------------------------------------
BOOL SetReportFlag( BYTE bMeterReportFlag,eReportType Type)
{
	if( Type == eFollowReport )
	{
		if( bMeterReportFlag == ReportMode.bMeterReportFlag )
		{
			return TRUE;
		}	
				
		ReportMode.bMeterReportFlag = bMeterReportFlag;
		
		if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode) == FALSE )
		{
			return FALSE;
		}	
		
		return TRUE;
	}
	else if( Type == eActiveReport )
	{
		if( bMeterReportFlag == ReportMode.bMeterActiveReportFlag )
		{
			return TRUE;
		}	
				
		ReportMode.bMeterActiveReportFlag = bMeterReportFlag;
		
		if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode) == FALSE )
		{
			return FALSE;
		}	
		
		return TRUE;		
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
//函数功能: 读取允许上报标志
//
//参数: 	Type		eFollowReport/eActiveReport/eFollowStatusReportMethod
//
//返回值:	TRUE--允许上报	FALSE--不允许上报
//		eFollowStatusReportMethod   0：主动上报  	1：跟随上报	
//备注:4300电气设备 属性7.8 允许跟随上报,允许主动上报
//-----------------------------------------------
BOOL ReadReportFlag( eReportType Type )
{
	BYTE Result = FALSE;
	if( Type == eFollowReport )
	{
		Result = ReportMode.bMeterReportFlag;
	}
	else if( Type == eActiveReport )
	{
		Result = ReportMode.bMeterActiveReportFlag;
	}
	else if( Type == eFollowStatusReportMethod)
	{
		Result = ReportMode.byReportStatusMethod;
	}
	
	return Result;	
}
//-----------------------------------------------
//函数功能: 掉电事件上报组帧
//
//参数:	inSubEventIndex[in]: eSUB_EVENT_LOST_POWER（掉电事件）
//							其他(其他事件返回)
//
//		RecordNo[in]: 		记录序号，写到buf中，掉电发生上报用
//							
//返回值:  
//			
//备注: 
//-----------------------------------------------
void api_LostPowerReportFraming( BYTE inSubEventIndex ,DWORD RecordNo )
{
	WORD Result,i,Num;
	BYTE Buf[MAX_PRO_BUF+30],OADBuf[200];
	TTwoByteUnion Len;
	TDLT698RecordPara DLT698RecordPara;
	
	//非掉电事件退出
	if(inSubEventIndex != eSUB_EVENT_LOST_POWER )
	{
		return ;
	}
	if( MeterVoltageConst != METER_220V )//掉电主动上报只有直通表
	{
		return ;
	}
	//判断是否已经置位
	memset(LostPowerReportBuf,0x00,sizeof(LostPowerReportBuf));
	
	ReportParaCheck( );
	
	if( ReportMode.bMeterActiveReportFlag == FALSE )//主动上报总开关未开启不上报
	{
		return ;
	}
	
	if( api_GetReportChannelStatus(eCR) == FALSE )//上报通道有载波通道
	{
		return ;
	}

	//掉电事件的上报方式为主动上报
	if((ReportMode.byReportMethod[eEVENT_LOST_POWER_NO/8]&(BYTE)(0x01<<(eEVENT_LOST_POWER_NO%8))) != 0x00)
	{
		return ;
	}

	memset( Buf, 0x00, sizeof( Buf ) );//清零比较buf

	Buf[0] = 0x88;//136-上报
	Buf[1] = 0x02;
	Buf[2] = 0x00;
	Buf[3] = 0x01;
	Buf[4] = 0x30;
	Buf[5] = 0x11;
	Buf[6] = 0x02;
	Buf[7] = 0x01;

	//读取关联对象属性表
	Len.w = api_ReadEventAttribute( 0x3011,0,sizeof(OADBuf), OADBuf);
	if( (Len.w & 0x8000) ||(Len.w < 4))//返回错误或者长度小于1个oad长度
	{
		Num =0;
	}
	else
	{
		Num = Len.w/4;
	}

	if( Num > MAX_EVENT_OAD_NUM )//进行极限值的判断
	{
		return ;
	}

	Buf[8] = Num+5;
	Len.w =9;
	
	for( i = 0; i < 5; i++ )//添加固定列
	{
		Buf[Len.w] = 0x00;
		memcpy( &Buf[Len.w+1] ,&FixedColumnOadTab[i], 4 );
		Len.w += 5;
	}

	for( i=0; i < Num; i++ )//添加关联对象属性
	{
		Buf[Len.w] = 0x00;
		memcpy( &Buf[Len.w+1] ,&OADBuf[4*i], 4 );
		Len.w += 5;
	}

	Buf[Len.w] = 0x01;//data
	Buf[Len.w+1] = 0x01;//1条记录
	Len.w += 2;

	DLT698RecordPara.OI = 0x3011;                   //掉电OI
	DLT698RecordPara.Ch = 0x55;                        //不清上报 固定通道传输
	DLT698RecordPara.pOAD = OADBuf;                 //pOAD指针
	DLT698RecordPara.OADNum = 0;               		//OAD个数
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //选择次数
	DLT698RecordPara.TimeOrLast = 1;           		//取上1次掉电记录
	DLT698RecordPara.Phase = ePHASE_ALL;

	Result = ReadEventRecordByNo( 1, &DLT698RecordPara, MAX_PRO_BUF-Len.w, Buf+Len.w );
	if( (Result == 0x8000) || (Result == 0) )
	{
		return;
	}
	Len.w += Result;

	Buf[Len.w] = 0x00;//跟随上报
	Buf[Len.w+1] = 0x00;//时间标签
	Len.w += 2;
	if( Len.w > 450 )//极限值判断67*5+10+2+14
	{
		return ;
	}
	memcpy(LostPowerReportBuf,(BYTE*)&RecordNo,0x04);//将长度保存到前两位
	memcpy(LostPowerReportBuf+4,(BYTE*)&Len,0x02);//将长度保存到前两位
	memcpy(LostPowerReportBuf+6,(BYTE*)&Buf,Len.w);//将长度保存到前两位
	lib_CheckSafeMemVerify( LostPowerReportBuf, sizeof(LostPowerReportBuf),0x01);//计算校验

}

//-----------------------------------------------
//函数功能: 掉电主动上报组帧
//
//参数:	 
//         
//返回值:   
//			
//备注: 
//	上报次数		事件未发生	事件已发生
//	上报次数=0		不上报			组帧上报
//	上报次数!=0		继续上报		清上报次数不上报
//-----------------------------------------------
void LostPowerStartReport( void )
{
	TFourByteUnion OAD;
	WORD Result,i,Num;
	BYTE Temp,Buf[MAX_PRO_BUF+30],OADBuf[200];
	TTwoByteUnion Len;
	TDLT698RecordPara DLT698RecordPara;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOad,RecordNo;
	TRealTimer LostPowerTime;
	
	ReportParaCheck( );
	if( MeterVoltageConst != METER_220V )//掉电主动上报只有直通表
	{
		return ;
	}
	
	if( ReportMode.bMeterActiveReportFlag == FALSE )//主动上报总开关未开启不上报
	{
		return ;
	}
	
	if( api_GetReportChannelStatus(eCR) == FALSE )//上报通道有载波通道
	{
		return ;
	}
	
	if((ReportMode.byReportMode[eEVENT_LOST_POWER_NO] & 0x01) != 0x01)//掉电事件发生上报
	{
		return ;
	}
	
	if((ReportMode.byReportMethod[eEVENT_LOST_POWER_NO/8]&(BYTE)(0x01<<(eEVENT_LOST_POWER_NO%8))) != 0x00)//掉电事件的上报方式为主动上报
	{
		return ;
	}
	
	//上报已经发生且次数为零，代表已经上报完成，不需要上报，退出即可
	if( ( api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 1 )
	&& ( LostPowerReportTimes.Times == 0 ) )
	{
		return ;
	}

	//上报没有发生且次数不为零，代表已经事件结束上报未完成，清上报，退出即可
	if( ( api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 0 )
	&& ( LostPowerReportTimes.Times != 0 ) )
	{
		ClearReportTimes( eActiveReportPowerDown );//置上报次数
		return ;
	}
	
	//判断是否大于1小时上报
	if(EventLostPowerEndTime < LostPowerEndTimeThreshold)
	{
		ClearReportTimes( eActiveReportPowerDown );//置上报次数
		return ;
	}
	//事件状态位为未发生，进行组帧上报
	if( ( api_DealEventStatus( eGET_EVENT_STATUS , eSUB_EVENT_LOST_POWER ) == 0 )
	|| (lib_CheckSafeMemVerify( LostPowerReportBuf, sizeof(LostPowerReportBuf),0x00) != TRUE ) )
	{
		memset( Buf, 0x00, sizeof( Buf ) );//清零比较buf
		memset( LostPowerReportBuf , 0x00 , sizeof( LostPowerReportBuf ) );
		
		Buf[0] = 0x88;//136-上报
		Buf[1] = 0x02;
		Buf[2] = 0x00;
		Buf[3] = 0x01;
		Buf[4] = 0x30;
		Buf[5] = 0x11;
		Buf[6] = 0x02;
		Buf[7] = 0x01;

		//读取关联对象属性表
		Len.w = api_ReadEventAttribute( 0x3011 , 0 , sizeof( OADBuf ), OADBuf );
		if( ( Len.w & 0x8000 ) ||( Len.w < 4))//返回错误或者长度小于1个oad长度
		{
			Num =0;
		}
		else
		{
			Num = Len.w / 4;
		}
		
		if( (Num + 4) > MAX_EVENT_OAD_NUM )//进行极限值的判断
		{
			ClearReportTimes( eActiveReportPowerDown);
			return;
		}
		
		Buf[8] = Num + 5;
		Len.w = 9;

		for( i = 0; i < 5; i++ )//添加固定列
		{
			Buf[Len.w] = 0x00;
			memcpy( &Buf[Len.w+1] ,&FixedColumnOadTab[i], 4 );
			Len.w += 5;
		}
		
		for( i=0; i < Num; i++ )//添加关联对象属性
		{
			Buf[Len.w] = 0x00;
			memcpy( &Buf[Len.w+1] ,&OADBuf[4*i], 4 );
			Len.w += 5;
		}
		Buf[Len.w] = 0x01;//data
		Buf[Len.w+1] = 0x01;//1条记录
		Len.w += 2;
		EventAddrLen.EventIndex = eEVENT_LOST_POWER_NO ;
		EventAddrLen.Phase = ePHASE_ALL;
		if( GetEventInfo( &EventAddrLen ) == FALSE )
		{
			ClearReportTimes( eActiveReportPowerDown);
			return;
		}
		
		// 当前记录数
		api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		RecordNo = EventDataInfo.RecordNo;
		//事件已经发生的序号--
		if( api_DealEventStatus( eGET_EVENT_STATUS , eSUB_EVENT_LOST_POWER ) == 1 )
		{
			RecordNo--;
		}
		
		Buf[Len.w] = 0x06;//Double_long_unsigned
		lib_ExchangeData((BYTE*)&Buf[Len.w+1] ,(BYTE*)&RecordNo, 4 );

		//添加时间
		Len.w += 5;
		Buf[Len.w] = 0x1C;//Date_Time_S
		// 以E2中存的进入掉电时间作为事件发生时间，与上电后保存一致
		api_GetPowerDownTime( (TRealTimer*)&LostPowerTime );
		memcpy( Buf+Len.w+1 ,&LostPowerTime, 7 );
		lib_ExchangeData((BYTE*)&Buf[Len.w+1] ,(BYTE*)&Buf[Len.w+1], 2 );
		Len.w += 8;
		Buf[Len.w] = 0x00;
		Buf[Len.w+1] = 0x00;
		Buf[Len.w+2] = 0x00;
		Len.w += 3;
		//添加事件关联对象属性数据
		for( i = 0; i < Num; i++ )
		{
			memcpy( (BYTE*)&dwOad ,&OADBuf[4*i], 4 );
			Temp = (BYTE)((dwOad&0x00E00000)>>21);
			dwOad &= ~0x00E00000;

			//OAD属性特征bit5~7:1：事件发生前 2：事件发生后 3：事件结束前 4：事件结束后
			if( (Temp != 3) && (Temp != 4) )//如果属性特征不为3,4，默认按照1,2,进行处理！！！！！！
			{
				Result = api_GetProOadData( eGetNormalData, eTagData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, Buf+Len.w );
				if( (Result == 0x8000) || (Result == 0) )
				{
					ClearReportTimes( eActiveReportPowerDown);
					return;
				}
				Len.w += Result;
			}
			else
			{
				Buf[Len.w] = 0x00;
				Len.w += 1;
			}	
		}
		Buf[Len.w] = 0x00;//跟随上报
		Buf[Len.w+1] = 0x00;//时间标签
		Len.w += 2;
		if( Len.w > 450 )//极限值判断67*5+10+2+14
		{
			ClearReportTimes( eActiveReportPowerDown);
			return;
		}
		
	
		memcpy(LostPowerReportBuf,(BYTE*)&RecordNo,0x04);//将RecordNo保存到前两位
		memcpy(LostPowerReportBuf+4,(BYTE*)&Len,0x02);//将长度保存到前两位
		memcpy(LostPowerReportBuf+6,(BYTE*)&Buf,Len.w);//将buf保存
		lib_CheckSafeMemVerify( LostPowerReportBuf, sizeof(LostPowerReportBuf),0x01);//计算校验
		ResetReportTimes( eActiveReportPowerDown);//置上报次数
	}
	else
	{
		EventAddrLen.EventIndex = eEVENT_LOST_POWER_NO ;
		EventAddrLen.Phase = ePHASE_ALL;
		if( GetEventInfo( &EventAddrLen ) == FALSE )
		{
			ClearReportTimes( eActiveReportPowerDown);
			return;
		}
	
		// 当前记录数
		api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		//掉电前判断buf对不对
		memcpy((BYTE*)&RecordNo,(BYTE*)&LostPowerReportBuf,0x04);//将记录序号读出判断一下
		//读出的序号与缓存内的buf不同重新读flash组帧
		if(RecordNo != EventDataInfo.RecordNo)
		{
			ClearReportTimes( eActiveReportPowerDown);//置上报次数
			return;
		}
		
		
		//上报次数大于3，重置上报为3次
		if(LostPowerReportTimes.Times > 3)
		{
			ResetReportTimes( eActiveReportPowerDown);//置上报次数
		}
		
	}
}
//-----------------------------------------------
//函数功能: 掉电主动上报发送
//
//参数:	 
//         
//返回值:   
//			
//备注: 低功耗大循环调用
//-----------------------------------------------
void ProcLowPowerMeterReport( void)
{
	WORD i;
	TTwoByteUnion Len;
	if( MeterVoltageConst != METER_220V )//掉电主动上报只有直通表
	{
		return ;
	}
	if(LostPowerReportTimes.Times == 0)
	{
		return ;
	}
	//上报次数错误 清零上报次数
	if( LostPowerReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD) ))
	{
		ClearReportTimes( eActiveReportPowerDown );
		return ;
	}
	if((LostPowerReportTimes.Times > 0)&&(LostPowerReportTimes.Times <= 3))
	{
		
		api_ReportCRWakeUpConfig(0x00);
		if(lib_CheckSafeMemVerify( LostPowerReportBuf, sizeof(LostPowerReportBuf),0x00)== TRUE)
		{
			memcpy((BYTE*)&Len,LostPowerReportBuf+4 ,2);
			api_ActiveReportResponse( LostPowerReportBuf+6, Len.w , 0x55);
			SerialMap[eCR].SCIBeginSend(SerialMap[eCR].SCI_Ph_Num);
			CLEAR_WATCH_DOG;//先清看门狗再发
			for(i=0;i<500;i++)//堵在这里发送
			{
				if( Serial[eCR].TXPoint >= Serial[eCR].SendLength )
				{
					break;
				}
				api_Delayms(2);

			}
			api_Delayms(5);//2400的波特率下一个字节理论法送时间为4.1ms,此处延时5ms
			ReportTimesReduce1(eActiveReportPowerDown);
			if(LostPowerReportTimes.Times == 0)//上报完成清Buf
			{
				api_EnterLowPower( eFromDetectEnterDownMode );//重新调用进入低功耗函数
			}
		}
		else
		{
			ClearReportTimes( eActiveReportPowerDown );
		}
		api_ReportCRWakeUpConfig(0x55);
	}
	else if(LostPowerReportTimes.Times > 3)
	{
		ClearReportTimes( eActiveReportPowerDown );
	}
	
}

//-----------------------------------------------
//函数功能: 设置事件上报标识
//
//参数: 	
//			eReportClass ReportClass
//			OI[in]:				事件OI
//			byReportMode[in]:	eReportMode 	事件上报标识{不上报(0),事件发生上报(1),事件恢复上报(2),事件发生恢复均上报(3)}
//								eReportMethod 	0:主动上报 1：跟随上报
//
//返回值:	TRUE/FALSE
//			
//备注:接口类7的属性8，接口类24的属性11
//-----------------------------------------------
BOOL api_SetEventReportMode( eReportClass ReportClass, WORD OI, BYTE byReportMode )
{
	BYTE i,j,k;

	if( ReportClass > eReportMethod )
	{
		return FALSE;
	}
	
	if( OI == 0x2015 )//跟随上报方式
	{
		if( ReportClass != eReportMethod )
		{
			return FALSE;
		}
		
		if( byReportMode > 1 )
		{
			return FALSE;
		}
		
		if( byReportMode == ReportMode.byReportStatusMethod )//设置值与现值一样
		{
			return TRUE;
		}
		
		ReportMode.byReportStatusMethod = byReportMode;
	}
	else
	{
		if( GetEventIndex( OI, &i ) == FALSE )
		{
			return FALSE;
		}
		
		if( ReportClass == eReportMode )//上报模式
		{
			if( byReportMode > 3 )
			{
				return FALSE;
			}
			
			if( byReportMode == ReportMode.byReportMode[i] )//如果要设置值和RAM中一样，实际也和EEPROM中一样
			{
				return TRUE;	 
			}

			ReportMode.byReportMode[i] = byReportMode;
		}
		else//上报方法
		{
			if( byReportMode > 1 )
			{
				return FALSE;
			}
			
			j = (i%eNUM_OF_EVENT_PRG) / 8;
			k = i % 8;

			if( byReportMode == 1 )
			{
				if( ReportMode.byReportMethod[j] & (0x01<<k) )//这位已经置1，避免频繁写EEPROM
				{
					return TRUE;
				}	
				
				ReportMode.byReportMethod[j] |= (0x01<<k);

			}
			else
			{
				if( (ReportMode.byReportMethod[j] & (0x01<<k)) == 0 )//这位已经清零，避免频繁写EEPROM
				{
					return TRUE;
				}	
				ReportMode.byReportMethod[j] &= ~(0x01<<k);
			}	
		}
	}
		
	return api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode);
	
}

//-----------------------------------------------
//函数功能: 读取事件上报标识
//
//参数: 	
//			eReportClass ReportClass
//			OI[in]:				事件OI
//			byReportMode[in]:	eReportMode 	事件上报标识{不上报(0),事件发生上报(1),事件恢复上报(2),事件发生恢复均上报(3)}
//								eReportMethod 	0:主动上报 1：跟随上报
//
//返回值:	TRUE/FALSE
//			
//备注:接口类7的属性8，接口类24的属性11
//-----------------------------------------------
BOOL api_ReadEventReportMode( eReportClass ReportClass, WORD OI, BYTE *pBuf )
{
	BYTE i,j,k;

	if( ReportClass > eReportMethod )
	{
		return FALSE;
	}
	
	if( OI == 0x2015 )//跟随上报
	{
		if( ReportClass != eReportMethod )
		{
			return FALSE;
		}
		
		pBuf[0] = ReportMode.byReportStatusMethod;
	}
	else
	{
		if( GetEventIndex( OI, &i ) == FALSE )
		{
			return FALSE;
		}

		if( ReportClass == eReportMode )//上报模式
		{
			pBuf[0] = ReportMode.byReportMode[i];
		}
		else
		{
			j = (i%eNUM_OF_EVENT_PRG) / 8;
			k = i % 8;
			
			if( ReportMode.byReportMethod[j] & (0x01<<k) )
			{
				pBuf[0] = 1;
			}
			else
			{
				pBuf[0] = 0;
			}
			
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 主动上报判断
//
//参数:	   无
//         
//返回值:     TRUE：产生主动上报         FLASE:无新增上报
//			
//备注: 
//-----------------------------------------------
WORD api_JudgeActiveReport( void )
{
	TReportData tmpReportData;
	TFourByteUnion OAD;
	WORD StatusResult,IndexResult,Result,i;
	BYTE Buf[MAX_PRO_BUF+30],OADBuf[200];
	TTwoByteUnion Len;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	DWORD RecordNo;

	StatusResult = 0;
	IndexResult = 0;
	
	if( ReportMode.bMeterActiveReportFlag == FALSE )//主动上报总开关未开启不上报
	{
		return FALSE;
	}
	
	if( api_GetReportChannelStatus(eCR) == FALSE )
	{
		return FALSE;
	}
	
	if( ((ReportTimes.Times != 0) && (ReportOverTime == 0))
	|| ((LostPowerReportTimes.Times != 0) && (ReportPowerDownTime == 0)) )//有上报需要上报
	{
		
		ReportDataCheck( eCR );
		ReportParaCheck( );

		if( ReportMode.byReportStatusMethod == 0 )//上报状态字上报方式为主动上报
		{
			//上报状态字比较
			memset( (BYTE*)&tmpReportData, 0x00, sizeof( tmpReportData ) );//清零比较buf
			StatusResult = memcmp( ReportData[eCR].Status, tmpReportData.Status, sizeof(ReportData[0].Status));
		}

		//上报事件列表比较
		Result = GetReportOadList( eGetReportActiveList, eCR, MAX_PRO_BUF, Buf );
		if( (Result != 0) && (Result != 0x8000) )
		{
			IndexResult = 1;//有主动上报
		}

		if((LostPowerReportTimes.Times != 0) && (ReportPowerDownTime == 0))//上报掉电记录
		{
			//掉电事件结束检3320中有没有
			if(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 0)
			{
				for( i = 0; i < (Result/4); i++ )
				{
					lib_ExchangeData( OAD.b, &Buf[4*i], 4 );
					if( (OAD.d&0xffff0000) == 0x30110000 )
					{
						break;
					}
				}
				if( i == (Result/4) )//上报列表中无掉电事件，清零上报次数
				{
					ClearReportTimes( eActiveReportPowerDown);
					return FALSE;
				}
			}

			EventAddrLen.EventIndex = eEVENT_LOST_POWER_NO ;
			EventAddrLen.Phase = ePHASE_ALL;
			if( GetEventInfo( &EventAddrLen ) == FALSE )
			{
				return FALSE;
			}
			
			// 当前记录数
			api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
			memcpy((BYTE*)&RecordNo,LostPowerReportBuf,0x04);//取出buf中存储的RecordNo
			
			if( ( lib_CheckSafeMemVerify( LostPowerReportBuf , sizeof( LostPowerReportBuf ), 0x00 )== TRUE )
			&& (RecordNo == EventDataInfo.RecordNo) )
			{
				memcpy((BYTE*)&Len,LostPowerReportBuf+4,0x02);//将长度保存到前两位
				if( Len.w > sizeof(Buf))//防止数组越界
				{
					return FALSE;
				}
				memcpy((BYTE*)&Buf,LostPowerReportBuf+6,Len.w);//将数据搬到buf中
			}
			else
			{
				ClearReportTimes( eActiveReportPowerDown);
				return FALSE;
			}
			//掉电事件发生上报延时为1S
			if(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 1)
			{
				ReportPowerDownTime = 101;//启动上报延时+发送时间 1010ms
				api_ActiveReportResponse( Buf, Len.w  , 0x55);
			}
			else
			{
				ReportPowerDownTime = 1010;//启动上报延时+发送时间 10100ms
				api_ActiveReportResponse( Buf, Len.w  , 0x00);
			}
			

			ReportTimesReduce1( eActiveReportPowerDown );//上报次数减1
			

		}
		else
		{
			if((StatusResult != 0) ||(IndexResult != 0) )//有跟随上报 进行数据组帧
			{
			
				memset( Buf, 0x00, sizeof( Buf ) );//清零比较buf
				
				Buf[0] = 136;
				Buf[1] = 0x01;
				Buf[2] = 0x00;
				if((StatusResult != 0) && (IndexResult != 0) )//A-ResultNormal个数
				{
					Buf[3] = 0x02;
				}
				else
				{
					Buf[3] = 0x01;
				}
				
				Len.w =4;
	
				if( IndexResult != 0 )
				{
					Buf[Len.w] = 0x33;//跟随上报列表OAD
					Buf[Len.w+1] = 0x20;
					Buf[Len.w+2] = 0x02;
					Buf[Len.w+3] = 0x00;
					Buf[Len.w+4] = 0x01;
					Len.w += 5;
					Result = AddActiveReportList( MAX_PRO_BUF-Len.w, Buf+Len.w );
					if( Result == 0x8000 )
					{
						ReportTimesReduce1( eActiveReportNormal );//出现错误 上报次数减1 避免错误一直存在一直判断的情况
						return FALSE;
					}
					Len.w += Result;
				}
	
				if( StatusResult != 0 )
				{
					Buf[Len.w] = 0x20;	//跟随上报状态字OAD
					Buf[Len.w+1] = 0x15;
					Buf[Len.w+2] = 0x02;
					Buf[Len.w+3] = 0x00;
					Buf[Len.w+4] = 0x01;
					Len.w +=5;
					Result = api_GetProOadData( eGetNormalData, eTagData, 0XFF, (BYTE*)&Buf[Len.w-5], NULL, sizeof(Buf)-Len.w, Buf+Len.w);
					if( Result == 0x8000 )
					{
						ReportTimesReduce1( eActiveReportNormal );//出现错误 上报次数减1 避免错误一直存在一直判断的情况
						return FALSE;
					}
					
					Len.w += Result;
				}
				Buf[Len.w] = 0x00;//跟随上报
				Buf[Len.w+1] = 0x00;//时间标签
				Len.w += 2;
				if( Len.w > 450 )//极限值判断67*5+10+2+14
				{
					ReportTimesReduce1( eActiveReportNormal );//出现错误 上报次数减1 避免错误一直存在一直判断的情况
					return FALSE;
				}
				
				ReportOverTime = 1010;//启动上报延时+发送时间 10100ms
				api_ActiveReportResponse( Buf, Len.w  , 0x00);
	
				ReportTimesReduce1( eActiveReportNormal );//上报次数减1
			}
			else
			{
				ClearReportTimes(0xff);
				return FALSE;
			}
		}
		return TRUE;
	}

	return FALSE;
}
//-----------------------------------------------
//函数功能: 确认主动上报
//
//参数:	   BYTE Type[in] 0x00: 确认新增事件列表 0x55：确认跟随上报状态字 0xAA：确认掉电事件
//         
//返回值:     TRUE：产生主动上报         FLASE:无新增上报
//			
//备注: 
//-----------------------------------------------
void api_ResponseActiveReport( BYTE Type )
{
	BYTE i,j,n,Index[SUB_EVENT_INDEX_MAX_NUM+10];

	//进行数据校验
	ReportDataCheck( eCR );
	
	if( Type == 0 )
	{
		for( i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++ )
		{
			if( ReportDataBak[eCR].Index[i] != 0xff)
			{
				n = 0;
				memcpy( Index, ReportData[eCR].Index, sizeof(ReportData[eCR].Index) );
				memset( ReportData[eCR].Index, 0xff, sizeof(ReportData[eCR].Index));//清空对应buf
				for( j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++ )
				{
					if( Index[j] == 0xff )//查询到0xff退出
					{
						break;
					}
					
					if( Index[j] != ReportDataBak[eCR].Index[i] )
					{

						ReportData[eCR].Index[n] = Index[j];
						n++;
					}
				}
			}
			else
			{
				break;
			}

		}

		//清零上报数据备份
		memset((BYTE *)&(ReportDataBak[eCR].Index), 0xFF, sizeof(ReportDataBak[0].Index));
		ReportDataBak[eCR].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[eCR],sizeof(TReportData)-sizeof(DWORD));
	}
	else if( Type == 0xAA )//确认掉电
	{
		n = 0;
		memcpy( Index, ReportData[eCR].Index, sizeof(ReportData[eCR].Index) );
		memset( ReportData[eCR].Index, 0xff, sizeof(ReportData[eCR].Index));//清空对应buf
		for( j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++ )
		{
			if( Index[j] == 0xff )//查询到0xff退出
			{
				break;
			}
			
			if( Index[j] != eSUB_EVENT_LOST_POWER )
			{
				ReportData[eCR].Index[n] = Index[j];
				n++;
			}
		}
		ClearReportTimes( eActiveReportPowerDown);
		
	}
	else if( Type == 0x55 )
	{
		for( i=0; i<4; i++ )
		{
			//如果该位已上报，且命令要清这位，则清零该位
			ReportData[eCR].Status[i] &= ~ReportDataBak[eCR].Status[i];
		}

		//清楚对应的跟随上报状态字备份
		memset((BYTE *)&(ReportDataBak[eCR].Status), 0x00, sizeof(ReportDataBak[eCR].Status));
		ReportDataBak[eCR].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[eCR],sizeof(TReportData)-sizeof(DWORD));
	}
	else
	{}
	
	ReportData[eCR].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[eCR],sizeof(TReportData)-sizeof(DWORD));		

}

//-----------------------------------------------
//函数功能: 设置跟随上报模式字
//
//参数:    
//			pBuf[in]：	保存数据的缓冲
//返回值:	TRUE/FALSE  
//			
//备注: 
//-----------------------------------------------
WORD api_SetFollowReportMode( BYTE *pBuf )
{
	memcpy( (BYTE*)&(ReportMode.byFollowReportMode[0]), pBuf, 4);

	if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode) == FALSE )
	{
		return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------
//函数功能: 上报秒任务
//
//参数: 	无
//
//返回值:	无
//			
//备注:
//-----------------------------------------------
void ReportSecTask(void)
{
	BYTE Status;
	BYTE *pByte;

	//掉电结束时间
	if(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 0)
	{
		if(EventLostPowerEndTime <= LostPowerEndTimeThreshold)
		{
			EventLostPowerEndTime++;
		}
		else
		{
			if(api_GetRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG) != TRUE)
			{
				api_SetRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG);
			}	
		}
	}
	
	//载波通道不支持上报或跟随上报未开启               不进行eventout管脚的置位
	if( (api_GetReportChannelStatus( eCR ) == FALSE) || (ReadReportFlag(eFollowReport) == FALSE))
	{
		CANCEL_EVENTOUT;
		return ;
	}

	//载波通道若有跟随上报，则置EVENTOUT为高阻态
	Status = GetFirstReportOad(1, eCR, NULL);

	//没有事件上报时判断有无跟随上报状态字
	if( Status == FALSE )
	{
		//跟随上报状态字上报方式为跟随上报
		if(ReportMode.byReportStatusMethod == 1)
		{
			if( (ReportData[eCR].Status[0] ==0x00) && (ReportData[eCR].Status[1] ==0x00) && (ReportData[eCR].Status[2] ==0x00) && (ReportData[eCR].Status[3] ==0x00) )
			{
			}
			else
			{
				Status = TRUE;
			}
		}
	}	
	
	if(Status == TRUE)
	{
		NOTICE_EVENTOUT;
	}
	else
	{
		CANCEL_EVENTOUT;
	}
}

//-----------------------------------------------
//函数功能: 上报小时任务
//
//参数: 	无
//
//返回值:	无
//			
//备注:
//-----------------------------------------------
void ReportHourTask(void)
{
	//上报参数校验
	ReportParaCheck();
	//上报数据校验
	ReportDataCheck( 0xff );

	//上报次数定时校验
	if( ReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD)))
	{
		ClearReportTimes(eActiveReportNormal);
	}
	//上报次数定时校验
	if( LostPowerReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD)))
	{
		ClearReportTimes(eActiveReportPowerDown);
	}
}

//-----------------------------------------------
//函数功能: 上报上电任务
//
//参数: 	无
//
//返回值:	无
//			
//备注:
//-----------------------------------------------		
void PowerUpReport(void)
{
    BYTE i,j,k;

    //模式字校验
	ReportParaCheck();

	ClearReportDataRam( eIR );//清空红外上报数据
	
	for( i=0; i < MAX_COM_CHANNEL_NUM; i++ )
	{
		if( i== eIR )
		{
			continue;
		}

		//上报数据校验
		if( ReportData[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportData[i],sizeof(TReportData)-sizeof(DWORD) ))
		{
			//读取失败清零
			if( api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(ReportConRom.ReportData[i]), sizeof(ReportData[0]), (BYTE *)&ReportData[i] ) == FALSE)
			{
				ClearReportDataRam( i );
			}
		}
		
		//备份上报数据错误 备份数据清零
		if( ReportDataBak[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD) ))
		{
			memset((BYTE *)&(ReportDataBak[i]), 0x00, sizeof(TReportData));
			memset((BYTE *)&(ReportDataBak[i].Index), 0xFF, sizeof(ReportDataBak[0].Index));
			ReportDataBak[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD));
		}
	}

	//上报次数错误 清零上报次数
	if( ReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD) ))
	{	
		//读取失败清零
		if( api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportTimes), sizeof(ReportTimes), (BYTE *)&ReportTimes ) == FALSE )
		{
			ClearReportTimes(eActiveReportNormal);
		}
	}
	//上电清掉电上报buf
	ClearReportTimes(eActiveReportPowerDown);

    #if( SEL_DLT645_2007 == YES )
    api_PowerUpReport645();
    #endif
}   

//-----------------------------------------------
//函数功能: 上报掉电任务
//
//参数: 	无
//
//返回值:	无
//			
//备注:
//-----------------------------------------------	
void PowerDownReport(void)
{
    BYTE i;

    //上报数据校验
	ReportDataCheck(0xff);
	//保存上报次数、上报数据、备份数据
	for( i=0; i < MAX_COM_CHANNEL_NUM; i++ )
	{
//		if( i== eIR )
//		{
//			continue;
//		}

 		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(ReportConRom.ReportData[i]), sizeof(ReportData[0]), (BYTE *)&ReportData[i] );
	}

	api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportTimes), sizeof(ReportTimes), (BYTE *)&ReportTimes );
	//置位掉电事件发生上报，组帧掉电上报内容
	LostPowerStartReport();
    #if( SEL_DLT645_2007 == YES )
    api_PowerDownReport645();
    #endif
}


//-----------------------------------------------
//函数功能: 上报电表初始化
//
//参数: 	无	
//           	
//返回值:	无
//			
//备注:
//-----------------------------------------------
void FactoryInitReport(void)
{
	BYTE i,j,k;

	memset( &ReportMode, 0x00, sizeof(ReportMode) );
	
	//默认不允许跟随上报 4300	电气设备 的属性7. 允许跟随上报
	ReportMode.bMeterReportFlag = MeterReportFlagConst;
	ReportMode.bMeterActiveReportFlag = MeterActiveReportFlagConst;
	//跟随上报状态字 上报方式
	ReportMode.byReportStatusMethod = MeterReportStatusFlagConst;
	
	//初始化为默认跟随上报
	memset( &ReportMode.byReportMethod, 0xff, sizeof(ReportMode.byReportMethod) );
	//事件主动上报方式
	for( i = 0; i< ReportActiveMethodConst[0]; i++ )
	{
		j = (ReportActiveMethodConst[i+1]%eNUM_OF_EVENT_PRG) / 8;
		k = ReportActiveMethodConst[i+1] % 8;
		ReportMode.byReportMethod[j] &= ~(0x01<<k);;
	}
	
    memcpy( (void *)&(ReportMode.byFollowReportMode[0]), FollowReportModeConst, GET_STRUCT_MEM_LEN(TReportMode, byFollowReportMode) );//20150400 电能表跟随上报模式字

	//4300	电气设备 的属性10	上报通道	array OAD
	memset( (void *)&(ReportMode.ReportChannelOAD[0]), 0x00, (sizeof(DWORD)*MAX_COM_CHANNEL_NUM) );
	//写上报通道
	for(i=0;i<FollowReportChannelConst[0];i++)
	{
		ReportMode.ReportChannelOAD[i]=ChannelOAD[FollowReportChannelConst[i+1]];
	}
		
	memset( &ReportMode.byReportMode, 0x00, sizeof(ReportMode.byReportMode) );
	//事件上报时刻
	for( i = 0; i< ReportModeConst[0].EventIndex; i++ )
	{
		ReportMode.byReportMode[ReportModeConst[i+1].EventIndex] = ReportModeConst[i+1].ReportMode;
	}
	api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode);

    #if( SEL_DLT645_2007 == YES )
    api_FactoryInitReport645( );
    #endif
    
	ClearReportDataRam( 0xff );
	//清零上报次数
	ClearReportTimes( 0xff );
	
	//初始化上报次数
	api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportTimes), sizeof(ReportTimes), (BYTE *)&ReportTimes );
   	//初始化上报数据
	for( i=0 ; i < MAX_COM_CHANNEL_NUM; i++ )
	{
//		if( i== eIR )
//		{
//			continue;
//		}
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(ReportConRom.ReportData[i]), sizeof(TReportData), (BYTE *)&ReportData[i] );
	}

}


#endif//#if ( SEL_DLT698_2016_FUNC == YES)
