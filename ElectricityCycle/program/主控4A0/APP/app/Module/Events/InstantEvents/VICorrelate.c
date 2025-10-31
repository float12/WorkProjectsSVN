//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.10.27
//描述		与电压、电流相关的事件记录
//修改记录	
//---------------------------------------------------------------------- 
#include "AllHead.h"
#include "VICorrelate.h"

#if((SEL_EVENT_LOST_V==YES)||(SEL_EVENT_WEAK_V==YES)||(SEL_EVENT_OVER_V==YES)||(SEL_EVENT_BREAK==YES)||(SEL_EVENT_LOST_I==YES)||(SEL_EVENT_OVER_I==YES)||(SEL_EVENT_BREAK_I==YES))
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------



//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static const DWORD VICorrelateOad[] =
{
#if( MAX_PHASE == 1)
	0x08,				// 个数
	0x00102201,	// 发生时刻正向有功总电能
	0x00202201, // 发生时刻反向有功总电能
	0x20002200, // 发生时刻电压
	0x20012200, // 发生时刻电流
	0x20042200, // 发生时刻有功功率
	0x200A2200, // 发生时刻功率因数
	0x00108201,	// 结束时刻正向有功总电能
	0x00208201  // 结束时刻反向有功总电能	
#else
//	0x26,				// 个数
//	0x00102201,	// 发生时刻正向有功总电能
//	0x00202201, // 发生时刻反向有功总电能
//	0x00302201,	// 发生时刻组合无功1总电能
//	0x00402201, // 发生时刻组合无功2总电能
//          
//	0x00112201,	// 发生时刻A相正向有功电能
//	0x00212201, // 发生时刻A相反向有功电能
//	0x00312201,	// 发生时刻A相组合无功1电能
//	0x00412201, // 发生时刻A相组合无功2电能
//          
//	0x00122201,	// 发生时刻B相正向有功电能
//	0x00222201, // 发生时刻B相反向有功电能
//	0x00322201,	// 发生时刻B相组合无功1电能
//	0x00422201, // 发生时刻B相组合无功2电能
//          
//	0x00132201,	// 发生时刻C相正向有功电能
//	0x00232201, // 发生时刻C相反向有功电能
//	0x00332201,	// 发生时刻C相组合无功1电能
//	0x00432201, // 发生时刻C相组合无功2电能
//          
//	0x20002200, // 发生时刻电压
//	0x20012200, // 发生时刻电流
//	0x20042200, // 发生时刻有功功率
//	0x20052200, // 发生时刻无功功率
//	0x200A2200, // 发生时刻功率因数
//	
//	0x20296200,	// 事件发生期间安时值
//          
//	0x00108201,	// 结束时刻正向有功总电能
//	0x00208201, // 结束时刻反向有功总电能	
//	0x00308201,	// 结束时刻组合无功1总电能
//	0x00408201, // 结束时刻组合无功2总电能	
//          
//	0x00118201,	// 结束时刻A相正向有功电能
//	0x00218201, // 结束时刻A相反向有功电能
//	0x00318201,	// 结束时刻A相组合无功1电能
//	0x00418201, // 结束时刻A相组合无功2电能
//          
//	0x00128201,	// 结束时刻B相正向有功电能
//	0x00228201, // 结束时刻B相反向有功电能
//	0x00328201,	// 结束时刻B相组合无功1电能
//	0x00428201, // 结束时刻B相组合无功2电能
//          
//	0x00138201,	// 结束时刻C相正向有功电能
//	0x00238201, // 结束时刻C相反向有功电能
//	0x00338201,	// 结束时刻C相组合无功1电能
//	0x00438201, // 结束时刻C相组合无功2电能

	0x04,				// 个数
	0x00102201,	// 发生时刻正向有功总电能
	0x00202201, // 发生时刻反向有功总电能
	0x00108201,	// 结束时刻正向有功总电能
	0x00208201  // 结束时刻反向有功总电能	

#endif
};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------


//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void FactoryInitVICorrelatePara( void )
{	
    DWORD OAD_Temp;

	#if( SEL_EVENT_OVER_I == YES )
	api_WriteEventAttribute( 0x3005, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// 过流

//    OAD_Temp = 0x00622920,     //事件发生期间安时值
//    api_DeleteEventAttribute(0x3005,(BYTE *)&OAD_Temp);
	#endif
	
	#if( SEL_EVENT_BREAK_I == YES )
	api_WriteEventAttribute( 0x3006, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// 断流

//	OAD_Temp = 0x00622920,     //事件发生期间安时值
//    api_DeleteEventAttribute(0x3006,(BYTE *)&OAD_Temp);
	#endif
	
	#if( SEL_EVENT_LOST_I == YES )
	api_WriteEventAttribute( 0x3004, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// 失流

//    OAD_Temp = 0x00622920,     //事件发生期间安时值
//    api_DeleteEventAttribute(0x3004,(BYTE *)&OAD_Temp);
	#endif
	
	#if( SEL_EVENT_BREAK == YES )
	api_WriteEventAttribute( 0x3003, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// 断相
	#endif
	
	#if( SEL_EVENT_OVER_V == YES )
	api_WriteEventAttribute( 0x3002, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// 过压
	#endif
	
	#if( SEL_EVENT_WEAK_V == YES )
	api_WriteEventAttribute( 0x3001, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// 欠压
	#endif
	
	#if( SEL_EVENT_LOST_V == YES )
	api_WriteEventAttribute( 0x3000, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// 失压
	#endif
}

// 失压处理
#if( SEL_EVENT_LOST_V == YES )
// 失压记录的状态检测
BYTE GetLostVStatus(BYTE Phase)
{
	WORD TypeI,TypeV;
	WORD w_RemoteV,w_Limit_V,w_Limit_V_UP;
	DWORD dw_RemoteI,dw_Limit_I;

	static BYTE Status[3] = {0,0,0};

	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}

	if( (Phase == 0) || (Phase > 3) )
	{
		//只有A、B、C分相
		return 0;//Status
	}
	#if( SEL_EVENT_LOST_ALL_V == YES )
    //如果处于全失压状态 结束分相失压状态
	if (SubEventInfoTab[eSUB_EVENT_LOST_ALL_V].pGetEventStatus(SubEventInfoTab[eSUB_EVENT_LOST_ALL_V].Phase) == 1 )
	{
        return 0;
	}
    #endif
	Phase --;
	// 失压事件电压触发上限
	DealEventParaByEventOrder( READ, eEVENT_LOST_V_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
	//新规范中明确如果失压事件电压触发上限为0，该事件不启用
	if(w_Limit_V == 0)
	{
		Status[Phase] = 0;//
		return Status[Phase];//Status;
	}

	// 失压事件电流触发下限
	DealEventParaByEventOrder( READ, eEVENT_LOST_V_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I );
	// 失压事件电压恢复下限
	DealEventParaByEventOrder( READ, eEVENT_LOST_V_NO, eTYPE_SPEC1, (BYTE *)&w_Limit_V_UP );

	if( Phase == 0 )
	{
		TypeI = PHASE_A+REMOTE_I;
		TypeV = PHASE_A+REMOTE_U;
	}
	else if( Phase == 1 )
	{
		TypeI = PHASE_B+REMOTE_I;
		TypeV = PHASE_B+REMOTE_U;	
	}
	else
	{
		TypeI = PHASE_C+REMOTE_I;
		TypeV = PHASE_C+REMOTE_U;
	}
	api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);
	api_GetRemoteData(TypeV, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);

	// 电流大于电流触发下限
	if( dw_RemoteI > dw_Limit_I )
	{
		if( (w_RemoteV < w_Limit_V)  
		|| ( api_DealEventStatus( eGET_EVENT_STATUS, (eSUB_EVENT_LOSTV_A+Phase) )	&& (w_RemoteV <=w_Limit_V_UP) ))
		{
			// 失压
			Status[Phase] = 1;
		}
		else
		{
			Status[Phase] = 0;//电压大于电压恢复下限，此时不考虑电流
		}
	}
	else
	{
		Status[Phase] = 0;//
	}
	
	return Status[Phase];
}

#endif//#if( SEL_EVENT_LOST_V == YES )

//欠压处理
#if(SEL_EVENT_WEAK_V==YES)
// 欠压记录的状态检测
BYTE GetWeakVStatus(BYTE Phase)
{
	BYTE Status;
	WORD Type;
	WORD w_RemoteV,w_Limit_V;

	Status = 0;

	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}

	if( (Phase == 0) || (Phase > 3) )
	{
		//只有A、B、C分相
		return Status;
	}
	Phase --;
	// 欠压事件电压触发上限
	DealEventParaByEventOrder( READ, eEVENT_WEAK_V_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
	//新规范中明确如果欠压事件电压触发上限为0，该事件不启用
	if(w_Limit_V == 0)
	{
		return Status;
	}

	if( Phase == 0 )
	{
		Type = PHASE_A+REMOTE_U;
	}
	else if( Phase == 1 )
	{
		Type = PHASE_B+REMOTE_U;	
	}
	else
	{
		Type = PHASE_C+REMOTE_U;
	}
	api_GetRemoteData(Type, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);

	// 此相电压小于电压触发上限
	if( w_RemoteV < w_Limit_V )
	{
		// 欠压
		Status = 1;
	}
	return Status;
}

#endif//#if(SEL_EVENT_WEAK_V==YES)

//过压处理
#if(SEL_EVENT_OVER_V==YES)
// 过压记录的状态检测
BYTE GetOverVStatus(BYTE Phase)
{
	BYTE Status;
	WORD Type;
	WORD w_RemoteV,w_Limit_V;

	Status = 0;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	if( (Phase == 0) || (Phase > 3) )
	{
		//只有A、B、C分相
		return Status;
	}
	Phase --;
	// 过压事件电压触发下限
	DealEventParaByEventOrder( READ, eEVENT_OVER_V_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
	//新规范中明确如果过压事件电压触发下限为0，该事件不启用
	if(w_Limit_V == 0)
	{
		return Status;
	}

	if( Phase == 0 )
	{
		Type = PHASE_A+REMOTE_U;
	}
	else if( Phase == 1 )
	{
		Type = PHASE_B+REMOTE_U;	
	}
	else
	{
		Type = PHASE_C+REMOTE_U;
	}
	api_GetRemoteData(Type, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);

	// 此相电压大于电压触发下限
	if( w_RemoteV > w_Limit_V )
	{
		// 过压
		Status = 1;
	}
	return Status;
}

#endif//#if(SEL_EVENT_OVER_V==YES)

//断相处理
#if(SEL_EVENT_BREAK==YES)
// 断相记录的状态检测
BYTE GetBreakStatus(BYTE Phase)
{
	BYTE Status;
	WORD TypeU,TypeI;
	WORD w_RemoteV,w_Limit_V;
	DWORD dw_RemoteI,dw_Limit_I;

	Status = 0;

	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}

	if( (Phase == 0) || (Phase > 3) )
	{
		//只有A、B、C分相
		return Status;
	}
    //如果处于掉电状态（电压小于60%）则结束断相
	if (SubEventInfoTab[eSUB_EVENT_LOST_POWER].pGetEventStatus(SubEventInfoTab[eSUB_EVENT_LOST_POWER].Phase) == 1 )
	{
        return Status;
	}
	
	Phase --;
	// 断相事件电压触发上限
	DealEventParaByEventOrder( READ, eEVENT_BREAK_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
	//新规范中明确如果断相事件电压触发上限为0，该事件不启用
	if(w_Limit_V == 0)
	{
		return Status;
	}

	// 断相事件电流触发上限
	DealEventParaByEventOrder( READ, eEVENT_BREAK_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I );
	if( Phase == 0 )
	{
		TypeU = PHASE_A+REMOTE_U;
		TypeI = PHASE_RA+REMOTE_I;
	}
	else if( Phase == 1 )
	{
		TypeU = PHASE_B+REMOTE_U;
		TypeI = PHASE_RB+REMOTE_I;
	}
	else
	{
		TypeU = PHASE_C+REMOTE_U;
		TypeI = PHASE_RC+REMOTE_I;
	}
	api_GetRemoteData(TypeU, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);
	api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);

	// 此相电压小于电压触发上限
	if( w_RemoteV < w_Limit_V )
	{
		if( dw_RemoteI < dw_Limit_I )
		{
			// 断相
			Status = 1;
		}
	}
	return Status;
}

#endif//#if(SEL_EVENT_BREAK==YES)

//失流处理
#if(SEL_EVENT_LOST_I==YES)
// 失流记录的状态检测
BYTE GetLostIStatus(BYTE Phase)
{
	BYTE i,Status;
	WORD TypeU,TypeI;
	WORD w_RemoteV,w_Limit_V;
	DWORD dw_RemoteI,dw_Limit_I,dw_Limit_I_Up;

	Status = 0;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	if( (Phase == 0) || (Phase > 3) )
	{
		//只有A、B、C分相
		return Status;
	}
	Phase --;
	// 失流事件电流触发上限
	DealEventParaByEventOrder( READ, eEVENT_LOST_I_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I_Up );
	//新规范中明确如果失流事件电流触发上限为0，该事件不启用
	if(dw_Limit_I_Up == 0)
	{
		return Status;
	}
	
	// 失流事件电流触发下限
	DealEventParaByEventOrder( READ, eEVENT_LOST_I_NO, eTYPE_SPEC1, (BYTE *)&dw_Limit_I );

	for( i = 0; i < MAX_PHASE; i++ )
	{
		if( i == 0 )
		{
			TypeI = PHASE_RA+REMOTE_I;
		}
		else if( i == 1 )
		{
			if( MeterTypesConst == METER_3P3W )
			{
				continue;
			}	
			TypeI = PHASE_RB+REMOTE_I;
		}
		else
		{
			TypeI = PHASE_RC+REMOTE_I;
		}
		api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);

		// 此相电流大于电流触发下限
		if( dw_RemoteI > dw_Limit_I )
		{
			Status = 1;
			break;
		}
	}
	
	// 若至少有一相大于电流触发下限
	if( Status != 0 )
	{
		Status = 0;
		// 失流事件电压触发下限
		DealEventParaByEventOrder( READ, eEVENT_LOST_I_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
		if( Phase == 0 )
		{
			TypeU = PHASE_A+REMOTE_U;
			TypeI = PHASE_RA+REMOTE_I;
		}
		else if( Phase == 1 )
		{
			if( MeterTypesConst == METER_3P3W )
			{
				return 0;
			}
			TypeU = PHASE_B+REMOTE_U;
			TypeI = PHASE_RB+REMOTE_I;
		}
		else
		{
			TypeU = PHASE_C+REMOTE_U;
			TypeI = PHASE_RC+REMOTE_I;
		}
		api_GetRemoteData(TypeU, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);
		api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);

		// 此相电压大于电压触发下限
		if( w_RemoteV > w_Limit_V )
		{
			//此相电流小于电流触发上限
			if( dw_RemoteI < dw_Limit_I_Up )
			{
				// 失流
				return 1;
			}
		}	
	}
	else// 如果三相电流都不大于电流触发下限
	{
		Status = 0;
	}	

	return Status;
}

#endif//#if(SEL_EVENT_LOST_I==YES)

//断流处理
#if(SEL_EVENT_BREAK_I==YES)
// 断流记录的状态检测
BYTE GetBreakIStatus(BYTE Phase)
{
	BYTE Status;
	WORD TypeU,TypeI;
	WORD w_RemoteV,w_Limit_V;
	DWORD dw_RemoteI,dw_Limit_I;

	Status = 0;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	if( (Phase == 0) || (Phase > 3) )
	{
		//只有A、B、C分相
		return Status;
	}
	Phase --;
	// 断流事件电流触发上限
	DealEventParaByEventOrder( READ, eEVENT_BREAK_I_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I );
	//新规范中明确如果断流事件电流触发上限为0，该事件不启用
	if(dw_Limit_I == 0)
	{
		return Status;
	}

	// 断流事件电压触发下限
	DealEventParaByEventOrder( READ, eEVENT_BREAK_I_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );

	if( Phase == 0 )
	{
		TypeU = PHASE_A+REMOTE_U;
		TypeI = PHASE_RA+REMOTE_I;
	}
	else if( Phase == 1 )
	{
		TypeU = PHASE_B+REMOTE_U;
		TypeI = PHASE_RB+REMOTE_I;
	}
	else
	{
		TypeU = PHASE_C+REMOTE_U;
		TypeI = PHASE_RC+REMOTE_I;
	}
	api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);
	api_GetRemoteData(TypeU, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);

	// 此相电压大于电压触发下限
	if( w_RemoteV > w_Limit_V )
	{
		if( dw_RemoteI < dw_Limit_I )
		{
			// 断流
			Status = 1;
		}
	}
	return Status;
}

#endif//#if(SEL_EVENT_BREAK_I==YES)

#if( SEL_EVENT_OVER_I == YES )
// 过流记录的状态检测
BYTE GetOverIStatus(BYTE Phase)
{
	BYTE Status;
	WORD Type;
	DWORD dw_RemoteI,dw_Limit_I;

	Status = 0;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	if( (Phase == 0) || (Phase > 3) )
	{
		//只有A、B、C分相
		return Status;
	}
	Phase --;
	// 过流事件电流触发下限
	DealEventParaByEventOrder( READ, eEVENT_OVER_I_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I );
	//新规范中明确如果过流事件电流触发下限为0，该事件不启用
	if(dw_Limit_I == 0)
	{
		return Status;
	}

	if( Phase == 0 )
	{
		Type = PHASE_A+REMOTE_I;
	}
	else if( Phase == 1 )
	{
		Type = PHASE_B+REMOTE_I;	
	}
	else
	{
		Type = PHASE_C+REMOTE_I;
	}
	api_GetRemoteData(Type, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);

	// 电流大于电流触发下限
	if( dw_RemoteI > dw_Limit_I )
	{
		// 过流
		Status = 1;
	}
	return Status;
}


#endif//#if( SEL_EVENT_OVER_I == YES )

#endif//#if((SEL_EVENT_LOST_V==YES)||(SEL_EVENT_WEAK_V==YES)||(SEL_EVENT_OVER_V==YES)||(SEL_EVENT_BREAK==YES)||(SEL_EVENT_LOST_I==YES)||(SEL_EVENT_OVER_I==YES)||(SEL_EVENT_BREAK_I==YES))

