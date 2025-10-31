//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.10.26
//描述		操作类编程记录，编程记录模块源文件
//修改记录	
//---------------------------------------------------------------------- 

#include "AllHead.h"
#include "PrgRecord.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//电能表编程事件 0x3012
static const DWORD ProgramOadConst[] =
{
	0,					// 个数
};

#if( SEL_PRG_INFO_CLEAR_METER == YES )		//电能表清零事件 0x3013
static const DWORD ClearEnergyOadConst[] =
{
	#if( MAX_PHASE == 1)
	0x02,				// 个数
	0x00102201,
	0x00202201
	#else
	0x0C,				// 个数
	0x00102201,        //电表清零前正向有功总电能
	0x00202201,        //电表清零前反向有功总电能
	0x00502201,        //电表清零前第一象限无功总电能
	0x00602201,        //电表清零前第二象限无功总电能
	0x00702201,        //电表清零前第三象限无功总电能
	0x00802201,        //电表清零前第四象限无功总电能
	0x00112201,        //电表清零前A相正向有功电能
	0x00212201,        //电表清零前A相反向有功电能
//	0x00512201,        //电表清零前A相第一象限无功电能
//	0x00612201,        //电表清零前A相第二象限无功电能
//	0x00712201,        //电表清零前A相第三象限无功电能
//	0x00812201,        //电表清零前A相第四象限无功电能
	0x00122201,        //电表清零前B相正向有功电能
	0x00222201,        //电表清零前B相反向有功电能
//	0x00522201,        //电表清零前B相第一象限无功电能
//	0x00622201,        //电表清零前B相第二象限无功电能
//	0x00722201,        //电表清零前B相第三象限无功电能
//	0x00822201,        //电表清零前B相第四象限无功电能
	0x00132201,        //电表清零前C相正向有功电能
	0x00232201,        //电表清零前C相反向有功电能
//	0x00532201,        //电表清零前C相第一象限无功电能
//	0x00632201,        //电表清零前C相第二象限无功电能
//	0x00732201,        //电表清零前C相第三象限无功电能
//	0x00832201,        //电表清零前C相第四象限无功电能
	#endif
};
#endif

#if( SEL_PRG_INFO_CLEAR_MD == YES )			//电能表需量清零事件 0x3014
static const DWORD ClearMDOadConst[] =
{
	6,					//个数
	0x10102201,			//需量清零前正向有功总最大需量及发生时间
	0x10202201,			//需量清零前反向有功总最大需量及发生时间
	0x10502201,			//需量清零前第一象限无功总最大需量及发生时间
	0x10602201,			//需量清零前第二象限无功总最大需量及发生时间
	0x10702201,			//需量清零前第三象限无功总最大需量及发生时间
	0x10802201,			//需量清零前第四象限无功总最大需量及发生时间
};
#endif

#if( SEL_PRG_INFO_CLEAR_EVENT == YES )		//电能表事件清零事件 0x3015
static const DWORD ClearEventOadConst[] =
{
	0,					// 个数
};
#endif

#if( SEL_PRG_INFO_ADJUST_TIME == YES )		//电能表校时事件	0x3016
static const DWORD AdjustTimeOadConst[] =
{
	0x02,				// 个数
	0x00102201,		//校时前正向有功总电能
	0x00202201,		//校时前反向有功总电能
};
#endif

#if( SEL_PRG_INFO_TIME_TABLE == YES )		//电能表时段表编程事件 0x3017
static const DWORD TimeTableOadConst[] =
{
	0x00,				// 个数
	//0x40162200,
	//0x40172200
};
#endif

#if( SEL_PRG_INFO_TIME_AREA == YES )		//电能表时区表编程事件 0x3018
static const DWORD TimeAreaOadConst[] =
{
	0x01,				// 个数
	0x40152200
};
#endif

#if( SEL_PRG_INFO_WEEK == YES )				//电能表周休日编程事件 0x3019
static const DWORD WeekOadConst[] =
{
	0x02,				// 个数
	0x40132200,			//编程前周休日釆用的日时段表号
};
#endif

#if( SEL_PRG_INFO_CLOSING_DAY == YES )		//电能表结算日编程事件 0x301a
static const DWORD ClosingDayOadConst[] =
{
	0x01,				// 个数
	0x41162200
};
#endif

 
//电能表跳闸事件 电能表合闸事件 拉闸 0x301F 合闸 0x3020
static const DWORD OpenCloseRelayOadConst[] =
{
    #if( MAX_PHASE == 1)
	0x02,		   // 个数
	0x00102201,    //事件发生时刻正向有功总电能
	0x00202201,    //事件发生时刻反向有功总电能
	#else
	0x06,		   // 个数
	0x00102201,    //事件发生时刻正向有功总电能
	0x00202201,    //事件发生时刻反向有功总电能
	0x00502201,    //事件发生时刻第一象限无功总电能
	0x00602201,    //事件发生时刻第二象限无功总电能
	0x00702201,    //事件发生时刻第三象限无功总电能
	0x00802201,    //事件发生时刻第四象限无功总电能
	#endif
};

#if( SEL_PRG_INFO_HOLIDAY == YES )			// 节假日编程记录 0x3021
static const DWORD HolidayOadConst[] =
{
	0,					// 个数
};
#endif

#if( SEL_PRG_INFO_P_COMBO == YES )			// 有功组合方式编程记录 0x3022
static const DWORD PCombOadConst[] =
{
	1,					// 个数
	0x41122200,			//有功组合模式字
};
#endif

#if( SEL_PRG_INFO_Q_COMBO == YES )			// 无功组合方式编程记录 0x3023
static const DWORD QCombOadConst[] =
{
	2,					// 个数
	0x41132200,			//无功组合模式字1
	0x41142200,			//无功组合模式字2
};
#endif

#if( PREPAY_MODE == PREPAY_LOCAL )
//0x3024	电能表费率参数表编程事件
static const DWORD TariffRateOadConst[] =
{
	2,					// 个数
	0x40182200,			//40182200——编程前当前套费率电价 
	0x40192200,			//40192200——编程前备用套费率电价 
};

//0x3025	电能表阶梯表编程事件
static const DWORD LadderOadConst[] =
{
	2,					// 个数
	0x401A2200,			//401A2200——编程前当前套阶梯电价 
	0x401B2200,			//401B2200——编程前备用套阶梯电价 
};
#endif//#if( PREPAY_MODE == PREPAY_LOCAL )

#if( SEL_PRG_UPDATE_KEY == YES )			//电能表密钥更新事件 0x3026
static const DWORD UpdateKeyOadConst[] =
{
	1,					// 个数
	0xf1002400,			//对称密钥版本
};
#endif

#if( PREPAY_MODE == PREPAY_LOCAL )
//0x3027	电能表异常插卡事件
static const DWORD AbnormalCardOadConst[] =
{
	4,			   		// 个数
	0x202C2202,	   		//购电前总购电次数
	0x202C2201,	   		//购电前剩余金额
	0x00102201,    		//事件发生时刻正向有功总电能
	0x00202201,    		//事件发生时刻反向有功总电能
};
//0x3028	电能表购电记录
static const DWORD BuyEnergyOadConst[] =
{
	6,					// 个数
	0x202C8202,			//202C8202——购电后购电次数 
	0x202C2201,			//202C2201——购电前剩余金额 
	0x202D2200,			//202D2200——购电前透支金额 
	0x202E2200,			//202E2200——购电前累计购电金额 
	0x202C8201,			//202C8201——购电后剩余金额
	0x202E8200,			//202E8200——购电后累计购电金额
};
//0x3029	电能表退费记录
static const DWORD ReturnMoneyOadConst[] =
{
	3,					// 个数
	0x202C2202,	   		//退费前总购电次数
	0x202C2201,	   		//退费前剩余金额
	0x202C8201,			//退费后剩余金额
};
#endif//#if( PREPAY_MODE == PREPAY_LOCAL )
#if( SEL_PRG_INFO_BROADJUST_TIME == YES )		//电能表广播校时事件	0x303B
static const DWORD BroAdjustTimeOadConst[] =
{
	0x02,				// 个数
	0x00102201,		//校时前正向有功总电能
	0x00202201,		//校时前反向有功总电能 
};
#endif

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
BYTE PrgFlag;
#if(SEL_PRG_RECORD645 == YES)
BYTE PrgFlag645; 
BYTE ProRecord645DiNum; //645最近一条编程记录（还未结束）里面记录了几个数据标识
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 获取编程记录状态
//
//参数: 	Phase[in]             
//返回值:  	TRUE/FALSE
//
//备注: 
//-----------------------------------------------
BYTE GetPrgStatus(BYTE Phase)
{
	if(Phase < 5)
	{
		return FALSE;
	}
	
	return TRUE;
}



//-----------------------------------------------
//函数功能: 暂存编程记录之前数据项的OAD及数据
//
//参数:  	Type[in]: 0--OAD	1--OMD
//			dwData[in]: 要设置的参数OAD 高字节在前，低字节在后
//                  
//返回值: TRUE/FALSE
//
//备注:规约调用编程记录时，在设置参数之前要先调用此函数。 
//-----------------------------------------------
BOOL api_WritePreProgramData( BYTE Type,DWORD dwData )
{
	WORD wLen,wLen1;
	DWORD dwTemp;
	BYTE Buf[MAX_PRG_CACHE_LEN+30];
	
	if(Type == 0)
	{
		wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE*)&dwData, 0 );
		
		if( (wLen+sizeof(DWORD)) > GET_STRUCT_MEM_LEN( TPreProgramData, Data ) )
		{
			return FALSE;
		}
		
		wLen1 = wLen+sizeof(DWORD);
		
		dwTemp = (dwData|0x00200000);
		memcpy( Buf, (BYTE*)&dwTemp, sizeof(DWORD) );
			
		dwTemp &= (dwData&(~0x00E00000));	
		api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwTemp, NULL, wLen, Buf+sizeof(DWORD) );			
	
		dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[0] );	
		api_WriteToContinueEEPRom( dwTemp, wLen+sizeof(DWORD), Buf );
	
	}
	else
	{
		dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[0] );	
		api_WriteToContinueEEPRom( dwTemp, sizeof(DWORD), (BYTE*)&dwData );
	}
	
	return TRUE;
}


//-----------------------------------------------
//函数功能: 清编程记录之前暂存的数据项OAD及数据
//
//参数:  Len[in]: 数据长度
//                
//返回值: 无
//
//备注: 
//-----------------------------------------------
static void ClrPreProgramData( BYTE Len )
{	
	DWORD dwAddr;
	
	if( Len > GET_STRUCT_MEM_LEN( TPreProgramData, Data ) )
	{
		return;
	}
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[0] );
	
	api_ClrContinueEEPRom( dwAddr, Len );	
}


//-----------------------------------------------
//函数功能: 写无功组合模式字编程事件发生源
//
//参数:  OI[in]: 0x4113 无功组合模式字1
//               0x4114 无功组合模式字2
//		Pointer[in]:     
//返回值: 无
//
//备注: 
//-----------------------------------------------
#if( SEL_PRG_INFO_Q_COMBO == YES )
static void WriteQcombSource(WORD OI, WORD Pointer)	
{
	DWORD dwTemp;
	BYTE Type;
	
	if(OI == 0x4113)
	{
		Type = 0;
	}
	else if(OI == 0x4114)
	{
		Type = 1;
	}
	else
	{
		return;
	}		

	//写当前无功组合模式字编程事件发生源
	dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.QcombSource[Pointer] );
	api_WriteToContinueEEPRom( dwTemp, 1, (BYTE *)&Type );	
}
//-----------------------------------------------
//函数功能: 无功组合方式2特征字编程次数加1
//
//参数:  
//                    
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注: 事件类的属性7  事件发生源+发生次数+累计时间
//-----------------------------------------------
static void AddProgram_Q_Combo2_Num( void )
{	
	DWORD dwTemp;
	
	dwTemp = 0;
	if( api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.dwPrgQComBo2Num ),sizeof(DWORD), (BYTE *)&dwTemp ) == FALSE )
	{
		return ;
	}
	dwTemp++;
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.dwPrgQComBo2Num ),sizeof(DWORD), (BYTE *)&dwTemp );
}	
#endif

//-----------------------------------------------
//函数功能: 清当前编程记录状态，置为未开始状态
//
//参数:  	无
//		  
//返回值: 	无
//
//备注: 
//-----------------------------------------------
void ClearPrgRecordRam( void )
{
	PrgFlag = ePROGRAM_NOT_YET_START;
}

//-----------------------------------------------
//函数功能: 获取当前编程记录状态
//
//参数:  	无
//		  
//返回值: 	0	ePROGRAM_NOT_YET_START	编程还未开始
//			1	ePROGRAM_STARTED		编程已经开始
//
//备注: 
//-----------------------------------------------
BYTE GetPrgRecord( void )
{
	return PrgFlag;
}

//-----------------------------------------------
//函数功能: 保存编程事件记录
//
//参数: 
//	Type[in]:	EVENT_START:	编程开始，设置数据时置开始
//				EVENT_END:		编程结束，断开应用连接或者掉电时置结束
//  DIType[in]:	0x51--OAD	0X53--OMD
//	pOad[in]:	指向OAD的指针
// 
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注: 只存第一个OAD的时间为开始时间
//		后续其他的OAD只保存OAD，不再存OAD对应数据
//		结束本次事件记录时，pOad无效
//-----------------------------------------------
BOOL api_SaveProgramRecord( BYTE Type, BYTE DIType, BYTE *pOad )
{	
	BYTE 				i;
	WORD				wOffSet;
	DWORD 				dwOad,dwOadTemp,dwAddr,dwBaseAddr;
	TPrgProgramSubRom	OadList;
	TEventAttInfo		EventAttInfo;
	TEventOADCommonData	EventOADCommonData;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventSectorInfo	EventSectorInfo;
	BYTE Buf[EVENT_VALID_OADLEN+30];//申请不定长数据
	
	//先读取编程记录的指针信息 Info
	EventAddrLen.EventIndex = ePRG_RECORD_NO;
	EventAddrLen.Phase = 0;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}
	
	if(api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo)== FALSE )
	{
		return FALSE;
	}	
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	if( EventAttInfo.AllDataLen > EVENT_VALID_OADLEN )
	{
		return FALSE;
	}
	//賦值基地址
	dwBaseAddr = EventAddrLen.dwRecordAddr;
	if( Type == EVENT_START )//0: 编程开始
	{
		if( (DIType!=0x51) && (DIType!=0x53) )
		{
			return FALSE;
		}
		
		// 编程还未开始
		if( PrgFlag	== ePROGRAM_NOT_YET_START )
		{
			// 第一个OAD，要保存时间，OAD对应相关数据
			PrgFlag = ePROGRAM_STARTED;

			memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD) );

			//获取本条事件的偏移地址
			GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,EventDataInfo.RecordNo, &EventSectorInfo);
			
			EventDataInfo.RecordNo++;
			api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);	
			//------存固定列数据 FixData 需要先写固定数据 否则导致清零不执行!!!
			memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) );
			//事件记录序号
			EventOADCommonData.EventNo = EventDataInfo.RecordNo;
			//事件发生时间
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.BeginTime );
			//保存数据
			dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
			if(WriteEventRecordData( dwAddr, sizeof(TEventOADCommonData), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData ) == FALSE)
			{
				
			}
			//------存关联属性对象表中的数据 Data
			if( EventAttInfo.NumofOad != 0 )
			{
				dwAddr += sizeof(TEventOADCommonData);
				// 存OAD对应数据
				wOffSet = 0;
				
				if( EventAttInfo.OadValidDataLen > EVENT_VALID_OADLEN )
				{
					return FALSE;
				}
								
				memset(Buf,0xff,EventAttInfo.OadValidDataLen);
				for( i = 0; i < EventAttInfo.NumofOad; i++ )
				{
					if(EventAttInfo.Oad[i] != 0xffffffff)
					{
						dwOadTemp = EventAttInfo.Oad[i];
						dwOadTemp &= ~0x00E00000;				
						api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOadTemp, NULL, 0xFF, Buf+wOffSet );
					}
					wOffSet += EventAttInfo.OadLen[i];
				}
				if(WriteEventRecordData( dwAddr, EventAttInfo.OadValidDataLen, dwBaseAddr, EVENT_RECORD_LEN, Buf ) == FALSE)
				{
				
				}
			}

			//处理上报
			SetReportIndex( EventAddrLen.SubEventIndex, EVENT_START );	

			//-------存编程对象列表 
			//刷新暂存ee的最近10个编程OAD 及OAD OMD标识
			memset( (BYTE*)&OadList, 0x00, sizeof(OadList) );
			dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );
			OadList.SaveOadNum = 1;	
			memcpy((BYTE *)&OadList.Oad[0], pOad, sizeof(DWORD));
			OadList.Type[0] = DIType;			
			api_WriteToContinueEEPRom(dwAddr, sizeof(OadList), (BYTE*)&OadList);			
		}
		else// 编程已经开始
		{
			if(EventDataInfo.RecordNo == 0)
			{
				return FALSE;
			}							
			
			//累加暂存ee的编程OAD个数
			dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );
			api_ReadFromContinueEEPRom(dwAddr, sizeof(OadList), (BYTE *)&OadList);
			if( OadList.SaveOadNum < MAX_PROGRAM_OAD_NUMBER )
			{
				OadList.SaveOadNum++;
			}			
			
			//刷新暂存ee的最近10个编程OAD
			memmove((BYTE *)&OadList.Oad+sizeof(DWORD), (BYTE *)&OadList.Oad, (MAX_PROGRAM_OAD_NUMBER-1)*sizeof(DWORD) );// 拷贝sizeof(tEeOad)-sizeof(DWORD)个，多了就溢出了
			memcpy((BYTE *)&OadList.Oad, pOad, sizeof(DWORD));
			
			//刷新暂存ee的最近10个编程OAD OMD标识
			memmove((BYTE *)&OadList.Type+1, (BYTE *)&OadList.Type, MAX_PROGRAM_OAD_NUMBER-1 );// 拷贝sizeof(tEeType)-1个，多了就溢出了
			OadList.Type[0] = DIType;
			api_WriteToContinueEEPRom(dwAddr, sizeof(OadList), (BYTE *)&OadList);
		}
	}
	else if( Type == EVENT_END )//1：编程结束
	{
		if( PrgFlag == ePROGRAM_STARTED )
		{
			if(EventDataInfo.RecordNo == 0)
			{
				return FALSE;
			}
			//获取本条事件的偏移地址
			GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,(EventDataInfo.RecordNo-1), &EventSectorInfo);
			
			// 置编程结束标志并写结束时间
			PrgFlag	= ePROGRAM_NOT_YET_START;

			// 存结束时间
			dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
			//api_ReadMemRecordData( dwAddr, sizeof(EventOADCommonData), (BYTE *)&EventOADCommonData );
			dwAddr += GET_STRUCT_ADDR( TEventOADCommonData, EventTime.EndTime );

			if( pOad == NULL )
			{
				api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.EndTime );			// 当前时间
			}
			else
			{
				api_GetPowerDownTime( (TRealTimer*)&EventOADCommonData.EventTime.EndTime );								// 掉电时间
			}
			if(WriteEventRecordData( dwAddr, sizeof(TRealTimer), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData.EventTime.EndTime ) == FALSE)
			{
			
			}
			
			//读出暂存在ee中的编程OAD个数，再保存到对应的编程记录中。
			dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );
			api_ReadFromContinueEEPRom(dwAddr, sizeof(OadList), (BYTE *)&OadList);
			dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;
			dwAddr += EventAttInfo.OadValidDataLen;
			if(WriteEventRecordData(dwAddr, sizeof(OadList), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&OadList) == FALSE)
			{
			
			}
			
			//处理上报
			SetReportIndex( EventAddrLen.SubEventIndex, EVENT_END );
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 保存操作类每个事件的编程记录（清电能，时区时段表编程等）
//
//参数:
//	inEventIndex[in]:	编程类型枚举号
//
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_SavePrgOperationRecord( eEVENT_INDEX inEventIndex )
{
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventSectorInfo	EventSectorInfo;
	DWORD 				dwPreOad,dwAddr,dwBaseAddr;
	TFourByteUnion 		tOAD;
	WORD 				twSavePointer,OffSet,wLen,wLen1;
	BYTE 				i;
	BYTE 				Buf[EVENT_VALID_OADLEN+30];
	BYTE 				PreBuf[EVENT_ALONE_OADLEN+30];
	
	EventAddrLen.EventIndex = inEventIndex;
	EventAddrLen.Phase = 0;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	if(api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo) == FALSE )
	{
		return FALSE;
	}

	if( EventAttInfo.AllDataLen > EVENT_VALID_OADLEN )
	{
		return FALSE;
	}
	
	if( EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM )
	{
		return FALSE;
	}
	//賦值基地址
	dwBaseAddr	= EventAddrLen.dwRecordAddr;
	//获取本条事件的偏移地址
	GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,EventDataInfo.RecordNo, &EventSectorInfo);

	twSavePointer = EventDataInfo.RecordNo%EventAttInfo.MaxRecordNo;
	EventDataInfo.RecordNo++;// 一直增加
	
	//读出编程之前暂存的OAD及对应的数据或OMD
	dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[0] );
	api_ReadFromContinueEEPRom( dwAddr, 4, (BYTE*)&dwPreOad );

	wLen = 0;
	wLen1 = 0;
	//电表清零、需量清零不需要读设置之前的数据
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//电表清零记录
	if(EventAddrLen.EventIndex == ePRG_CLEAR_METER_NO)
	{
		//置为无效OAD
		dwPreOad = 0xffffffff;
		wLen = 1;
	}
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )//清需量记录
	if(EventAddrLen.EventIndex == ePRG_CLEAR_MD_NO)
	{
		//置为无效OAD
		dwPreOad = 0xffffffff;
		wLen = 1;
	}
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//事件清零记录
	if(EventAddrLen.EventIndex == ePRG_CLEAR_EVENT_NO)
	{
		//dwPreOad中存放的是OMD
		wLen = 1;
	}
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)		
	if(EventAddrLen.EventIndex == ePRG_BUY_MONEY_NO)//电能表购电记录
	{
		//置为无效OAD
		dwPreOad = 0xffffffff;
		wLen = 16;//按规范多保存购电前的购电次数，避免用户扩展不满足
		//202C2201——购电前剩余金额 
		//202D2200——购电前透支金额 
		//202E2200——购电前累计购电金额 	
	}		
	if(EventAddrLen.EventIndex == ePRG_ABR_CARD_NO)//电能表异常插卡事件
	{
		//置为无效OAD
		dwPreOad = 0xffffffff;
		wLen = 18;//命令头保存时保存 CH,Cla,Ins,P1,P2,P31,P32,其中CH:0x01--ESAM 0x00--CARD,esam命令头为CH后的6字节，卡为CH后的5字节
	}
	if(EventAddrLen.EventIndex == ePRG_RETURN_MONEY_NO)//电能表退费记录
	{
		//dwPreOad中存放的是退费金额
		wLen = 4;
	}
	#endif
	
	if( RelayTypeConst != RELAY_NO ) //跳合闸记录
	{
		if( (EventAddrLen.EventIndex == ePRG_OPEN_RELAY_NO)||(EventAddrLen.EventIndex == ePRG_CLOSE_RELAY_NO) )
		{
			//dwPreOad中存放的是OMD
			wLen = 1;
		}
	}
	
	if(wLen == 0)
	{
		wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwPreOad, 0 );	
		if( (wLen+sizeof(DWORD)) > GET_STRUCT_MEM_LEN(TPreProgramData,Data) )
		{
			return FALSE;
		}
	}
	
	api_ReadFromContinueEEPRom( dwAddr+sizeof(DWORD), wLen, PreBuf );
	// 写数据
	// 需要先写EventOADCommonData否则会导致flash清零不执行
	//存事件记录序号，事件发生时间,编程类没有事件结束时间,上报状态
	memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) );

	EventOADCommonData.EventNo = EventDataInfo.RecordNo;
	if(( inEventIndex == ePRG_ADJUST_TIME_NO ) || (inEventIndex == ePRG_BROADJUST_TIME_NO))
	{
		memcpy((BYTE *)&EventOADCommonData.EventTime.BeginTime, PreBuf, sizeof(TRealTimer));
	}
	else
	{
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.BeginTime ); 
	}

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.EndTime );//记录结束时间
			
	dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
	if(WriteEventRecordData( dwAddr, sizeof(TEventOADCommonData), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData ) == FALSE)
	{
		
	
	}	
	if( EventAttInfo.NumofOad != 0 )
	{

		memset(Buf,0xff,EventAttInfo.OadValidDataLen);

		// 填满数据
		OffSet = 0;
		for( i = 0; i < EventAttInfo.NumofOad; i++ )
		{
			tOAD.d = EventAttInfo.Oad[i];

			if( tOAD.d == 0xfffffff )
			{
			}
			#if(PREPAY_MODE == PREPAY_LOCAL)
			else if( (EventAddrLen.EventIndex == ePRG_RETURN_MONEY_NO)&&(dwPreOad == 0X06021033) )//电能表退费记录
			{
				memcpy(Buf+OffSet, (BYTE *)&dwPreOad, EventAttInfo.OadLen[i]);
			}
            else if ((EventAddrLen.EventIndex == ePRG_RETURN_MONEY_NO)&&(tOAD.d == 0x01222C20))
           	{
				memcpy(Buf+OffSet, PreBuf, 4);
           	}  
			//202C2201——购电前剩余金额 tOAD.d=0x02822C20 tOAD.w[0]=0x2c20 tOAD.b[2]=0x22
			//202D2200——购电前透支金额 
			//202E2200——购电前累计购电金额 	
			else if( (EventAddrLen.EventIndex == ePRG_BUY_MONEY_NO)&&((tOAD.w[0] == 0x2C20)||(tOAD.w[0] == 0x2D20)||(tOAD.w[0] == 0x2E20))
				&&((tOAD.b[2] == 0x22)||(tOAD.b[2] == 0x42)) )//电能表购电记录 购电前的剩余金额、透支金额、累计购电金额
			{
				if(tOAD.w[0] == 0x2C20)
				{
					if(tOAD.b[3] == 0x02)//购电前购电次数，//按规范多保存购电前的购电次数，避免用户扩展不满足
					{
						wLen1 = 12;
					}
					else
					{
						wLen1 = 0; //购电前剩余金额
					}
				}
				else if(tOAD.w[0] == 0x2D20)
				{
					wLen1 = 4;
				}
				else//	||(tOAD.w[0] == 0x2E20)
				{
					wLen1 = 8;
				}	
				memcpy(Buf+OffSet, PreBuf+wLen1, 4);
			}
			#endif
			else if( tOAD.d == dwPreOad )//编程前暂存的OAD及对应数据
			{
				#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
				if(EventAddrLen.EventIndex == ePRG_CLEAR_EVENT_NO)
				{
					//事件清零要保存OMD
					memcpy(Buf+OffSet, (void*)&dwPreOad, sizeof(dwPreOad));
				}
				else
				#endif
				{
					memcpy(Buf+OffSet, PreBuf, wLen);
				}
			}
			else
			{
				tOAD.d &= ~0x00E00000;
				api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&tOAD.d, NULL, 0xFF, Buf+OffSet );			
			}

			OffSet += EventAttInfo.OadLen[i];
		}
		
		// 写记录数据
		dwAddr+=sizeof(TEventOADCommonData);// 从开始位置直接偏移固定数据的长度就是数据区
		if(WriteEventRecordData( dwAddr, EventAttInfo.OadValidDataLen, dwBaseAddr, EVENT_RECORD_LEN, Buf ) == FALSE)
		{
		
		}
	}
	// 时段表编程的话保存编程时段表
	#if(SEL_PRG_INFO_TIME_TABLE==YES)
	if(EventAddrLen.EventIndex==ePRG_TIME_TABLE_NO)
	{
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;
		dwAddr += EventAttInfo.OadValidDataLen;// 偏移
		tOAD.d = dwPreOad & (~0x00E00000);//时段表，倒序
		memcpy(&PreBuf[wLen],(BYTE *)&tOAD.d,sizeof(DWORD));
		if(WriteEventRecordData( dwAddr, wLen+sizeof(DWORD), dwBaseAddr, EVENT_RECORD_LEN, PreBuf) == FALSE)
		{
		
		}
	}
	#endif
	// 节假日编程，保存编程前节假日
	#if(SEL_PRG_INFO_HOLIDAY==YES)
	if(EventAddrLen.EventIndex==ePRG_HOLIDAY_NO)
	{
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;
		dwAddr += EventAttInfo.OadValidDataLen;// 偏移
		tOAD.d = dwPreOad & (~0x00E00000);//节假日，倒序
		memcpy(&PreBuf[wLen],(BYTE *)&tOAD.d,sizeof(DWORD));
		if(WriteEventRecordData( dwAddr, wLen+sizeof(DWORD), dwBaseAddr, EVENT_RECORD_LEN, PreBuf) == FALSE)
		{
		
		}
	}
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)			
	if(EventAddrLen.EventIndex == ePRG_ABR_CARD_NO)//电能表异常插卡事件
	{
		//wLen = 18;//命令头保存时保存 CH,Cla,Ins,P1,P2,P31,P32,其中CH:0x01--ESAM 0x00--CARD,esam命令头为CH后的6字节，卡为CH后的5字节
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;// 不确定dwAddr上次的位置，需要重新定位
		dwAddr += EventAttInfo.OadValidDataLen;// 偏移
		if(WriteEventRecordData( dwAddr, wLen, dwBaseAddr, EVENT_RECORD_LEN, PreBuf ) == FALSE)
		{
		}
	}
	//ePRG_BUY_MONEY_NO,							//电能表购电记录					
	if(EventAddrLen.EventIndex == ePRG_RETURN_MONEY_NO)//电能表退费记录
	{
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;// 不确定dwAddr上次的位置，需要重新定位
		dwAddr += EventAttInfo.OadValidDataLen;// 偏移
		if(WriteEventRecordData( dwAddr, wLen, dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&dwPreOad )== FALSE)//dwPreOad中存放的是退费金额
		{
		}
	}
	#endif
	
	//保存 事件清零列表  array OMD ，此处我们只支持一个OMD
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
	if(EventAddrLen.EventIndex == ePRG_CLEAR_EVENT_NO)
	{
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;// 不确定dwAddr上次的位置，需要重新定位
		dwAddr += EventAttInfo.OadValidDataLen;// 偏移
		if(WriteEventRecordData( dwAddr, sizeof(DWORD), dwBaseAddr, EVENT_RECORD_LEN, (BYTE*)&dwPreOad ) == FALSE)
		{
		
		}
	}
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )
	if(EventAddrLen.EventIndex == ePRG_Q_COMBO_NO)
	{
		memcpy( (BYTE*)&OffSet, (BYTE*)&dwPreOad, sizeof(WORD));
		lib_InverseData( (BYTE*)&OffSet, sizeof(WORD) );
		WriteQcombSource(OffSet, twSavePointer);
		if( OffSet == 0x4114 ) //4114	8	无功组合方式2特征字
		{
			AddProgram_Q_Combo2_Num();
			api_WriteToContinueEEPRom(GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PrgQCombo_CurrentEventStatus[1] ),sizeof(TRealTimer),(BYTE *)&(EventOADCommonData.EventTime.BeginTime) );
		}
		else
		{
			api_WriteToContinueEEPRom(GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PrgQCombo_CurrentEventStatus[0] ),sizeof(TRealTimer),(BYTE *)&(EventOADCommonData.EventTime.BeginTime) );
		}		
	}
	#endif
	
	api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	//清编程记录之前暂存的数据项OAD及数据
	ClrPreProgramData(wLen+1);// 是否有必要，是否有必要清除这么长

	//清零记录和清事件记录不再此置位，有单独的地方进行置位
	if( (inEventIndex != ePRG_CLEAR_METER_NO) && (inEventIndex != ePRG_CLEAR_EVENT_NO) )
	{
		SetReportIndex( EventAddrLen.SubEventIndex, EVENT_START+EVENT_END );
	}
	
	return TRUE;
}

#if(SEL_PRG_RECORD645 == YES)
//电能表编程事件645
static BOOL GetPrg645RecordInfo( TEventAddrLen *pTEventAddrLen )
{
	//pTEventAddrLen->dwAttInfoEeAddr = GET_CONTINUE_ADDR( EventConRom.EventAttInfo[pTEventAddrLen->EventIndex] );
	pTEventAddrLen->dwDataInfoEeAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.ProRecord645 );
	pTEventAddrLen->dwRecordAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PrgProgramRom645[0].Time );
//	pTEventAddrLen->wDataLen = sizeof(TPrgProgramRom645) * MAX_EVENTRECORD_NUMBER;//@@@@@李丕凯

	return TRUE;
}

//-----------------------------------------------
//函数功能: 保存645规约编程事件记录
//
//参数:
//	Type[in]:	EVENT_START:	编程开始，设置数据时置开始
//				EVENT_END:		编程结束，断开应用连接或者掉电时置结束
//	Op[in]:		操作者代码 倒序
//	pDI[in]:	指向DI的指针 倒序
//
//  返回值:	TRUE   正确
//			FALSE  错误
//
//备注:设置数据时Type为EVENT_START，在认证结束的调用Type为EVENT_END
//-----------------------------------------------
BOOL api_SaveProgramRecord645( BYTE Type, BYTE *Op, BYTE *pDI )
{
	WORD				twSavePointer;
	DWORD 				dwAddr;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TRealTimer			t;
	TPrgProgramRom645	tPrgProgramRom645;
	TFourByteUnion dwDI;
	
	memcpy(dwDI.b,pDI,4);
		
	//现在只通过OI将复位主动上报状态字清除
	if( dwDI.d == 0x04001503 )
	{
		return FALSE;
	}

	memset( tPrgProgramRom645.Time, 0xff, sizeof(TPrgProgramRom645) );

	//先读取编程记录的指针信息 Info	
	GetPrg645RecordInfo( &EventAddrLen );
	//读取RecordNo
	if(api_VReadSafeMem( EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo ) == FALSE )
	{
		return FALSE;
	}
	
	if( Type == EVENT_START ) //0: 编程开始
	{
		// 编程还未开始
		if( PrgFlag645 == ePROGRAM_NOT_YET_START )
		{
			// 第一个OAD，要保存时间，OAD对应相关数据
			PrgFlag645 = ePROGRAM_STARTED;
			
			twSavePointer = EventDataInfo.RecordNo % MAX_EVENTRECORD_NUMBER;
			dwAddr = EventAddrLen.dwRecordAddr + sizeof(TPrgProgramRom645) * twSavePointer; 
	
			EventDataInfo.RecordNo++;
			api_VWriteSafeMem( EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo );
	
			//事件发生时间
			api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
			api_TimeFormat698To645( &t, tPrgProgramRom645.Time );
			lib_InverseData( tPrgProgramRom645.Time, 6 );
			//操作者代码
			memcpy( tPrgProgramRom645.Op, Op, 4 );
			//保存数据
			memcpy( tPrgProgramRom645.DI, pDI, 4 );
						
			api_WriteToContinueEEPRom( dwAddr, sizeof(TPrgProgramRom645), tPrgProgramRom645.Time );

			//处理上报			
			SetReportIndex( eSUB_EVENT_PRG_RECORD, EVENT_START );	
			
			ProRecord645DiNum = 1;
		}
		else // 编程已经开始
		{
			if( EventDataInfo.RecordNo == 0 )
			{
				return FALSE;
			}
			
			twSavePointer = (EventDataInfo.RecordNo-1) % MAX_EVENTRECORD_NUMBER;
			dwAddr = EventAddrLen.dwRecordAddr + sizeof(TPrgProgramRom645) * twSavePointer;
			
			api_ReadFromContinueEEPRom( dwAddr + 10, sizeof(TPrgProgramRom645) - 10, tPrgProgramRom645.DI ); 
			
			if( ProRecord645DiNum <= 10 )
			{
				memmove( tPrgProgramRom645.DI+4, tPrgProgramRom645.DI, (MAX_EVENTRECORD_NUMBER - 1) * 4 );
				memcpy( tPrgProgramRom645.DI, pDI, 4 );
			}
			else
			{
				return FALSE;
			}
					
			//发生时间及操作者代码不再重复写
			api_WriteToContinueEEPRom( dwAddr+10, sizeof(TPrgProgramRom645)-10, tPrgProgramRom645.DI ); 
		}	
	}
	else
	{
		// 编程已开始
		if( PrgFlag645 == ePROGRAM_STARTED )
		{
			SetReportIndex( eSUB_EVENT_PRG_RECORD, EVENT_END );
		}
		PrgFlag645 = ePROGRAM_NOT_YET_START;
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 645规约编程事件记录（03300000~0330000a）
//
//参数:
//	DI[in]:		读取数据的数据标识
//				EVENT_END:		编程结束，断开应用连接或者掉电时置结束
//	pOutBuf[out]:	返回的数据
//
//  返回值:	数据长度 8000代表错误
//
//备注:支持编程次数及最近10次编程记录的读取
//-----------------------------------------------
WORD api_ReadProgramRecord645( BYTE *DI, BYTE *pOutBuf )
{
	TFourByteUnion 		tdw;
	WORD				twSavePointer;
	DWORD 				dwAddr;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	
	memcpy( tdw.b, DI, 4 );
	if( (tdw.d > 0x0330000a)||(tdw.d < 0x03300000) )
	{
		return 0x8000;
	}
	
	//先读取编程记录的指针信息 Info
	GetPrg645RecordInfo( &EventAddrLen );
	//读取RecordNo
	api_VReadSafeMem( EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo );
	
	if( tdw.d == 0x03300000 )
	{
		dwAddr = lib_DWBinToBCD( EventDataInfo.RecordNo );
		memcpy( pOutBuf, (BYTE *)&dwAddr, 3 );
		
		return 3;
	}
	else
	{
		if( tdw.b[0] > EventDataInfo.RecordNo )
		{
			memset( pOutBuf, 0x00, sizeof(TPrgProgramRom645) );
		}
		else
		{
			twSavePointer = (EventDataInfo.RecordNo - tdw.b[0])% MAX_EVENTRECORD_NUMBER;
			dwAddr = EventAddrLen.dwRecordAddr + sizeof(TPrgProgramRom645) * twSavePointer;
			
			api_ReadFromContinueEEPRom( dwAddr, sizeof(TPrgProgramRom645), pOutBuf );						
		}
	}
	
	return sizeof(TPrgProgramRom645); 
}
#endif//#if(SEL_PRG_RECORD645 == YES)

//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void FactoryInitPrgRecordPara( void )
{
	//电能表编程事件 0x3012
	api_WriteEventAttribute( 0x3012, 0xFF, (BYTE *)&ProgramOadConst[0], 		sizeof(ProgramOadConst)/sizeof(DWORD) );
	
	#if( SEL_PRG_INFO_CLEAR_METER == YES )		//电能表清零事件 0x3013
	api_WriteEventAttribute( 0x3013, 0xFF, (BYTE *)&ClearEnergyOadConst[0], 	sizeof(ClearEnergyOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )			//电能表需量清零事件 0x3014
	api_WriteEventAttribute( 0x3014, 0xFF, (BYTE *)&ClearMDOadConst[0], 		sizeof(ClearMDOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )		//电能表事件清零事件 0x3015
	api_WriteEventAttribute( 0x3015, 0xFF, (BYTE *)&ClearEventOadConst[0], 	sizeof(ClearEventOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )		//电能表校时事件	0x3016
	api_WriteEventAttribute( 0x3016, 0xFF, (BYTE *)&AdjustTimeOadConst[0], 	sizeof(AdjustTimeOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )		//电能表时段表编程事件 0x3017
	api_WriteEventAttribute( 0x3017, 0xFF, (BYTE *)&TimeTableOadConst[0], 	sizeof(TimeTableOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_INFO_TIME_AREA == YES )		//电能表时区表编程事件 0x3018
	api_WriteEventAttribute( 0x3018, 0xFF, (BYTE *)&TimeAreaOadConst[0], 		sizeof(TimeAreaOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_WEEK == YES )				//电能表周休日编程事件 0x3019
	api_WriteEventAttribute( 0x3019, 0xFF, (BYTE *)&WeekOadConst[0], 			sizeof(WeekOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )		//电能表结算日编程事件 0x301a
	api_WriteEventAttribute( 0x301a, 0xFF, (BYTE *)&ClosingDayOadConst[0], 	sizeof(ClosingDayOadConst)/sizeof(DWORD) );
	#endif
	
	//电能表跳闸事件 电能表合闸事件 拉闸 0x301F 合闸 0x3020
	api_WriteEventAttribute( 0x301F, 0xFF, (BYTE *)&OpenCloseRelayOadConst[0], sizeof(OpenCloseRelayOadConst)/sizeof(DWORD) );
	api_WriteEventAttribute( 0x3020, 0xFF, (BYTE *)&OpenCloseRelayOadConst[0], sizeof(OpenCloseRelayOadConst)/sizeof(DWORD) );
	
	#if( SEL_PRG_INFO_HOLIDAY == YES )			// 节假日编程记录 0x3021	
	api_WriteEventAttribute( 0x3021, 0xFF, (BYTE *)&HolidayOadConst[0], 		sizeof(HolidayOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_INFO_P_COMBO == YES )			// 有功组合方式编程记录 0x3022	
	api_WriteEventAttribute( 0x3022, 0xFF, (BYTE *)&PCombOadConst[0], 		sizeof(PCombOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )			// 无功组合方式编程记录 0x3023	
	api_WriteEventAttribute( 0x3023, 0xFF, (BYTE *)&QCombOadConst[0], 		sizeof(QCombOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_UPDATE_KEY == YES )			//电能表密钥更新事件 0x3026	
	api_WriteEventAttribute( 0x3026, 0xFF, (BYTE *)&UpdateKeyOadConst[0], 	sizeof(UpdateKeyOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//0x3024	电能表费率参数表编程事件
	api_WriteEventAttribute( 0x3024, 0xFF, (BYTE *)&TariffRateOadConst[0], 	sizeof(TariffRateOadConst)/sizeof(DWORD) );
	//0x3025	电能表阶梯表编程事件
	api_WriteEventAttribute( 0x3025, 0xFF, (BYTE *)&LadderOadConst[0], 	sizeof(LadderOadConst)/sizeof(DWORD) );
	//0x3027	电能表异常插卡事件
	api_WriteEventAttribute( 0x3027, 0xFF, (BYTE *)&AbnormalCardOadConst[0], 	sizeof(AbnormalCardOadConst)/sizeof(DWORD) );
	//0x3028	电能表购电记录
	api_WriteEventAttribute( 0x3028, 0xFF, (BYTE *)&BuyEnergyOadConst[0], 	sizeof(BuyEnergyOadConst)/sizeof(DWORD) );
	//0x3029	电能表退费记录
	api_WriteEventAttribute( 0x3029, 0xFF, (BYTE *)&ReturnMoneyOadConst[0], 	sizeof(ReturnMoneyOadConst)/sizeof(DWORD) );
	#endif//#if( PREPAY_MODE == PREPAY_LOCAL )
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )		//电能表广播校时事件	0x303B
	api_WriteEventAttribute( 0x303C, 0xFF, (BYTE *)&BroAdjustTimeOadConst[0], 	sizeof(BroAdjustTimeOadConst)/sizeof(DWORD) );	
	#endif
}

