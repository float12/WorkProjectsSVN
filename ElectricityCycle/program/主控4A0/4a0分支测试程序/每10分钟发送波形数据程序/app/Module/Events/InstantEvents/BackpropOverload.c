//---------------------------------------------
// 潮流反向处理记录  过载处理记录
//---------------------------------------------
// 在新的DLT645中，潮流反向、过载记录的格式、附属数据基本一致
// 仅仅是判断起始条件不一致，所以放在一起集中处理
//---------------------------------------------

#include "AllHead.h"

#if( (SEL_EVENT_BACKPROP == YES)||(SEL_EVENT_OVERLOAD == YES) )
static const DWORD BackPopOverLoadOad[] =
{
#if( MAX_PHASE == 1)
	0x04,          // 个数:4
	0x00102201,    //事件发生时刻正向有功总电能
	0x00202201,    //事件发生时刻反向有功总电能
	0x00108201,    //事件结束后正向有功总电能
	0x00208201,    //事件结束后反向有功总电能
#else
//	0x20,          // 个数:32
//	0x00102201,    //事件发生时刻正向有功总电能
//	0x00202201,    //事件发生时刻反向有功总电能
//	0x00302201,    //事件发生时刻组合无功1总电能
//	0x00402201,    //事件发生时刻组合无功2总电能
//	0x00112201,    //事件发生时刻A相正向有功电能
//	0x00212201,    //事件发生时刻A相反向有功电能
//	0x00312201,    //事件发生时刻A相组合无功1电能
//	0x00412201,    //事件发生时刻A相组合无功2电能
//	0x00122201,    //事件发生时刻B相正向有功电能
//	0x00222201,    //事件发生时刻B相反向有功电能
//	0x00322201,    //事件发生时刻B相组合无功1电能
//	0x00422201,    //事件发生时刻B相组合无功2电能
//	0x00132201,    //事件发生时刻C相正向有功电能
//	0x00232201,    //事件发生时刻C相反向有功电能
//	0x00332201,    //事件发生时刻C相组合无功1电能
//	0x00432201,    //事件发生时刻C相组合无功2电能
//	0x00108201,    //事件结束后正向有功总电能
//	0x00208201,    //事件结束后反向有功总电能
//	0x00308201,    //事件结束后组合无功1总电能
//	0x00408201,    //事件结束后组合无功2总电能
//	0x00118201,    //事件结束后A相正向有功电能
//	0x00218201,    //事件结束后A相反向有功电能
//	0x00318201,    //事件结束后A相组合无功1电能
//	0x00418201,    //事件结束后A相组合无功2电能
//	0x00128201,    //事件结束后B相正向有功电能
//	0x00228201,    //事件结束后B相反向有功电能
//	0x00328201,    //事件结束后B相组合无功1电能
//	0x00428201,    //事件结束后B相组合无功2电能
//	0x00138201,    //事件结束后C相正向有功电能
//	0x00238201,    //事件结束后C相反向有功电能
//	0x00338201,    //事件结束后C相组合无功1电能
//	0x00438201     //事件结束后C相组合无功2电能
	0x04,          // 个数:4
	0x00102201,    //事件发生时刻正向有功总电能
	0x00202201,    //事件发生时刻反向有功总电能
	0x00108201,    //事件结束后正向有功总电能
	0x00208201,    //事件结束后反向有功总电能
#endif
};

//------------------------------------------------------------------------------------------------------
// 各个事件处理专用部分－有功功率反向
//------------------------------------------------------------------------------------------------------
// 有功功率反向处理
#if( SEL_EVENT_BACKPROP == YES )
void FactoryInitBackPopPara( void )
{
	api_WriteEventAttribute( 0x3007, 0xFF, (BYTE *)&BackPopOverLoadOad[0], sizeof(BackPopOverLoadOad)/sizeof(DWORD) );
}

// 判断有功功率反向是否发生
BYTE GetBackpropStatus(BYTE Phase)
{
	// BYTE Status;
	// WORD TypeP;
	// DWORD dw_LimitP,dw_RemoteP;

	// Status = 0;
	
	// if(api_GetMeasureMentMode() == eOnePhase)
	// {
	// 	if( (Phase == 2) || (Phase == 3) )
	// 	{
	// 		return 0;
	// 	}		
	// }

	// if( Phase > 3 )
	// {
	// 	//只有总及A、B、C分相
	// 	return Status;
	// }
	// // 有功功率反向事件有功功率触发下限
	// DealEventParaByEventOrder( READ, eEVENT_BACKPROP_NO, eTYPE_DWORD, (BYTE *)&dw_LimitP );
	// //新规范中明确如果有功功率反向事件有功功率触发下限为0，该事件不启用
	// if( dw_LimitP == 0 )
	// {
	// 	return Status;
	// }
	
	// if( Phase == ePHASE_ALL )
	// {
	// 	TypeP = PHASE_ALL+REMOTE_P;
	// }
	// else if( MeterTypesConst == METER_3P3W )//三相三线系统下不判断分相功率反向，三相三线只判断总的功率反向
	// {
	// 	return 0;
	// }
	// else if( Phase == ePHASE_A )
	// {
	// 	TypeP = PHASE_A+REMOTE_P;
	// }
	// else if( Phase == ePHASE_B )
	// {
	// 	TypeP = PHASE_B+REMOTE_P;
	// }
	// else// if( Phase == ePHASE_C )
	// {
	// 	TypeP = PHASE_C+REMOTE_P;
	// }

	// api_GetRemoteData(TypeP, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteP);  
	// if( dw_RemoteP > dw_LimitP )// 功率大于有功功率触发下限
	// {
	// 	// 功率反向
	// 	if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P+Phase) == TRUE )
	// 	{
	// 		Status = 1;
	// 	}
	// }

	// return Status;
}

#endif

#if( SEL_EVENT_OVERLOAD == YES )
//------------------------------------------------------------------------------------------------------
// 各个事件处理专用部分－过载
//------------------------------------------------------------------------------------------------------
void FactoryInitOverLoadPara( void )
{
	api_WriteEventAttribute( 0x3008, 0xFF, (BYTE *)&BackPopOverLoadOad[0], sizeof(BackPopOverLoadOad)/sizeof(DWORD) );
}

// 过载处理
// 判断过载是否发生（多处调用，因此从事件定义中提出）
BYTE GetOverloadStatus(BYTE Phase)
{
	BYTE Status;
	WORD TypeP;
	DWORD dw_LimitP,dw_RemoteP;

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
	// 过载事件有功功率触发下限
	DealEventParaByEventOrder( READ, eEVENT_OVERLOAD_NO, eTYPE_DWORD, (BYTE *)&dw_LimitP );// 
	//新规范中明确如果过载事件有功功率触发下限为0，该事件不启用
	if(dw_LimitP == 0)
	{
		return Status;
	}

	if( Phase == 0 )
	{
		TypeP = PHASE_A+REMOTE_P;
	}
	else if( Phase == 1 )
	{
		TypeP = PHASE_B+REMOTE_P;
	}
	else
	{
		TypeP = PHASE_C+REMOTE_P;
	}
	api_GetRemoteData(TypeP, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteP);

	// 功率大于功率触发下限
	if( dw_RemoteP > dw_LimitP )
	{
		// 过载
		Status = 1;
	}

	return Status;

}

#endif

#endif //#if( (SEL_EVENT_BACKPROP == YES)||(SEL_EVENT_OVERLOAD == YES )

