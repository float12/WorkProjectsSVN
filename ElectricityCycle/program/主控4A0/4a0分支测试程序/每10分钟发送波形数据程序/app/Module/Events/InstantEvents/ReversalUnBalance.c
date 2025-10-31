//---------------------------------------------
// 逆相序处理记录  不平衡处理记录
//---------------------------------------------
// 在新的DLT645中，电压、电流逆相序记录和电压、电流不平衡记录的格式、附属数据基本一致
// 不同：判断起始条件不一致；不平衡记录中多了一项最大不平衡率
// 所以放在一起集中处理
//---------------------------------------------

#include "AllHead.h"

// 逆相序、不平衡处理公共部分
#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
static const DWORD ReversalUnBalanceOad[] =
{
#if( MAX_PHASE == 1)
	0x06,			   //个数:6个	电压不平衡率，电流不平衡率，逆相序	三个事件都是用这个数组，根据不同的事件进行删除OAD
	0x00102201, 	   //事件发生时刻正向有功总电能
	0x00202201, 	   //事件发生时刻反向有功总电能
	0x20266200, 	   //事件发生期间电压不平衡率
	0x20276200,       //事件发生期间电流不平衡率
	0x00108201, 	   //事件结束后正向有功总电能
	0x00208201, 	   //事件结束后反向有功总电能
#else
	0x22,			   //个数:34个	电压不平衡率，电流不平衡率，逆相序	三个事件都是用这个数组，根据不同的事件进行删除OAD
	0x00102201, 	   //事件发生时刻正向有功总电能
	0x00202201, 	   //事件发生时刻反向有功总电能
	0x00302201, 	   //事件发生时刻组合无功1总电能
	0x00402201, 	   //事件发生时刻组合无功2总电能
	0x00112201, 	   //事件发生时刻A相正向有功电能
	0x00212201, 	   //事件发生时刻A相反向有功电能
	0x00312201, 	   //事件发生时刻A相组合无功1电能
	0x00412201, 	   //事件发生时刻A相组合无功2电能
	0x00122201, 	   //事件发生时刻B相正向有功电能
	0x00222201, 	   //事件发生时刻B相反向有功电能
	0x00322201, 	   //事件发生时刻B相组合无功1电能
	0x00422201, 	   //事件发生时刻B相组合无功2电能
	0x00132201, 	   //事件发生时刻C相正向有功电能
	0x00232201, 	   //事件发生时刻C相反向有功电能
	0x00332201, 	   //事件发生时刻C相组合无功1电能
	0x00432201, 	   //事件发生时刻C相组合无功2电能
	0x20266200, 	   //事件发生期间电压不平衡率
	0x20276200,       //事件发生期间电流不平衡率
	0x00108201, 	   //事件结束后正向有功总电能
	0x00208201, 	   //事件结束后反向有功总电能
	0x00308201, 	   //事件结束后组合无功1总电能
	0x00408201, 	   //事件结束后组合无功2总电能
	0x00118201, 	   //事件结束后A相正向有功电能
	0x00218201, 	   //事件结束后A相反向有功电能
	0x00318201, 	   //事件结束后A相组合无功1电能
	0x00418201, 	   //事件结束后A相组合无功2电能
	0x00128201, 	   //事件结束后B相正向有功电能
	0x00228201, 	   //事件结束后B相反向有功电能
	0x00328201, 	   //事件结束后B相组合无功1电能
	0x00428201, 	   //事件结束后B相组合无功2电能
	0x00138201, 	   //事件结束后C相正向有功电能
	0x00238201, 	   //事件结束后C相反向有功电能
	0x00338201, 	   //事件结束后C相组合无功1电能
	0x00438201, 	   //事件结束后C相组合无功2电能
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
void FactoryInitReversalUnBalancePara( void )
{
	DWORD OAD_Temp = 0x00;
	
	#if( SEL_EVENT_V_REVERSAL == YES )
	api_WriteEventAttribute( 0x300f, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// 过流

	OAD_Temp = 0x00622620, 	   //事件发生期间电压不平衡率
	api_DeleteEventAttribute(0x300f,(BYTE *)&OAD_Temp);
	OAD_Temp = 0x00622720,        //事件发生期间电流不平衡率
	api_DeleteEventAttribute(0x300f,(BYTE *)&OAD_Temp);

	#endif
	
	#if( SEL_EVENT_I_REVERSAL == YES )
	api_WriteEventAttribute( 0x3010, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// 断流

	OAD_Temp = 0x00622620, 	   //事件发生期间电压不平衡率
	api_DeleteEventAttribute(0x3010,(BYTE *)&OAD_Temp);
	OAD_Temp = 0x00622720,        //事件发生期间电流不平衡率
	api_DeleteEventAttribute(0x3010,(BYTE *)&OAD_Temp);

	#endif
	
	#if( SEL_EVENT_V_UNBALANCE == YES )
	api_WriteEventAttribute( 0x301d, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// 失流

	OAD_Temp = 0x00622720,        //事件发生期间电流不平衡率
	api_DeleteEventAttribute(0x301d,(BYTE *)&OAD_Temp);

	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	api_WriteEventAttribute( 0x301e, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// 断相

	OAD_Temp = 0x00622620, 	   //事件发生期间电压不平衡率
	api_DeleteEventAttribute(0x301e,(BYTE *)&OAD_Temp);

	#endif
	
	#if( SEL_EVENT_OVER_V == YES )
	api_WriteEventAttribute( 0x302d, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// 过压

	OAD_Temp = 0x00622620, 	   //事件发生期间电压不平衡率
	api_DeleteEventAttribute(0x302d,(BYTE *)&OAD_Temp);

	#endif
}
//RAM中暂存的最大不平衡率
WORD RamUnBalanceRate[3];
//-----------------------------------------------
//函数功能: 返回电压、电流不平衡事件发生期间最大不平衡率
//
//参数: Type[in]	:	D7 -- 0 -- RAM中的不平衡率；1 -- EEPROM中的不平衡率
//								0 -- 电压不平衡率
//								1 -- 电流不平衡率
//								2 -- 电流严重不平衡率
//                    
//返回值:  	对应的不平衡率
//
//备注:
//-----------------------------------------------
WORD GetUnBalanceMaxRate(BYTE Type)
{
	DWORD Addr = 0;
	WORD TempUnBalanceRate;
	if(Type & 0x80)
	{
		#if( SEL_EVENT_V_UNBALANCE == YES )
		if(Type == 0x80)
		{
			Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomVUnBalanceRate);
		}
		#endif
		#if( SEL_EVENT_I_UNBALANCE == YES )
		if(Type == 0x81)
		{
			Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomIUnBalanceRate);
		}
		#endif
		#if( SEL_EVENT_I_S_UNBALANCE == YES )
		if(Type == 0x82)
		{
			Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomISUnBalanceRate);
		}
		#endif
		api_ReadFromContinueEEPRom( Addr, sizeof(WORD), (BYTE *)&TempUnBalanceRate );
	}
	else
	{
		TempUnBalanceRate = RamUnBalanceRate[Type&0x0f];
	}
	return TempUnBalanceRate;
}
//-----------------------------------------------
//函数功能: 刷新电压、电流不平衡事件发生期间的最大不平衡率
//
//参数: 			无
//                    
//返回值:  	无
//
//备注:	事件秒任务中调用
//-----------------------------------------------
void FreshUnBalanceRate(void)
{
	WORD TempRate;
	#if( SEL_EVENT_V_UNBALANCE == YES )
	if( (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_V_UNBALANCE )) == FALSE )
	{
		RamUnBalanceRate[0] = 0;
	}
	else
	{
		TempRate = api_GetSampleStatus(0);
		if( TempRate > RamUnBalanceRate[0] )
		{
			RamUnBalanceRate[0] = TempRate;
		}
	}
	#endif

	#if( SEL_EVENT_I_UNBALANCE == YES )
	if( (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_I_UNBALANCE )) == FALSE )
	{
		RamUnBalanceRate[1] = 0;
	}
	else
	{
		TempRate = api_GetSampleStatus(1);
		if( TempRate > RamUnBalanceRate[1] )
		{
			RamUnBalanceRate[1] = TempRate;
		}
	}
	#endif

	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	if( (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_S_I_UNBALANCE )) == FALSE )
	{
		RamUnBalanceRate[2] = 0;
	}
	else
	{
		TempRate = api_GetSampleStatus(1);
		if( TempRate > RamUnBalanceRate[2] )
		{
			RamUnBalanceRate[2] = TempRate;
		}
	}
	#endif
}
//-----------------------------------------------
//函数功能: 掉电存储电压、电流不平衡事件发生期间的最大不平衡率
//
//参数: 			无
//                    
//返回值:  	无
//
//备注:	掉电时调用
//-----------------------------------------------
void UnBalanceRatePowerOff(void)
{
	DWORD Addr = 0;
	
	#if( SEL_EVENT_V_UNBALANCE == YES )
	Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomVUnBalanceRate);
	api_WriteToContinueEEPRom( Addr, sizeof(WORD), (BYTE *)&RamUnBalanceRate[0] );
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomIUnBalanceRate);
	api_WriteToContinueEEPRom( Addr, sizeof(WORD), (BYTE *)&RamUnBalanceRate[1] );
	#endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomISUnBalanceRate);
	api_WriteToContinueEEPRom( Addr, sizeof(WORD), (BYTE *)&RamUnBalanceRate[2] );
	#endif
}

//------------------------------------------------------------------------------------------------------
// 各个事件处理专用部分－电压逆相序
//------------------------------------------------------------------------------------------------------
// 电压逆相序处理
#if( SEL_EVENT_V_REVERSAL == YES )
// 判断电压逆相序是否发生
BYTE GetVReversalStatus(BYTE Phase)
{
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		return 0;		
	}
	
	//电表功能规范中约定，当某相电压低于60%UN时，不再进行电压逆相序的判断
	if( (api_GetSampleStatus(0x10)&0x8007) == 0x8000 )
	{
		return 1;
	}

	return 0;
}



#endif

//------------------------------------------------------------------------------------------------------
// 各个事件处理专用部分－电流逆相序
//------------------------------------------------------------------------------------------------------
// 电流逆相序处理
#if( SEL_EVENT_I_REVERSAL == YES )
// 判断电流逆相序是否发生
BYTE GetIReversalStatus(BYTE Phase)
{
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		return 0;		
	}
	//电表功能规范中约定，三相电压均大于电能表的临界电压， 三相电流均大于 5%额定（基本）电流才判断
	if( (api_GetSampleStatus(0x10)&0x0007) != 0 )
	{
		return 0;
	}
	
	if( (api_GetSampleStatus(0x11)&0x8007) == 0x8000 )
	{
		return 1;
	}

	return 0;
}

#endif


//------------------------------------------------------------------------------------------------------
// 各个事件处理专用部分－电压不平衡
//------------------------------------------------------------------------------------------------------
// 电压不平衡处理
#if( SEL_EVENT_V_UNBALANCE == YES )
// 判断电压不平衡是否发生
BYTE GetVUnBalanceStatus(BYTE Phase)
{
	BYTE Status;
	WORD dw_LimitUnBalance_V;
	Status = 0;
	dw_LimitUnBalance_V = 0;

	//国网规范要求至少有一相电压大于临界电压
    if( MeterTypesConst == METER_3P3W )
    {
		if( (api_GetSampleStatus(0x10)&0x0005) == 0x05 )
		{
			return Status;
		}
    }
    else
    {
		if( (api_GetSampleStatus(0x10)&0x0007) == 0x07 )
		{
			return Status;
		}
    }
    
	// 电压不平衡率限值
	DealEventParaByEventOrder( READ, eEVENT_V_UNBALANCE_NO, eTYPE_WORD, (BYTE *)&dw_LimitUnBalance_V );
	//新规范中明确如果电压不平衡率限值为0，该事件不启用
	if(dw_LimitUnBalance_V == 0)
	{
		return Status;
	}

	if(api_GetSampleStatus(0) > dw_LimitUnBalance_V)
	{
		Status = 1;
	}

	return Status;
}
#endif


//------------------------------------------------------------------------------------------------------
// 各个事件处理专用部分－电流不平衡
//------------------------------------------------------------------------------------------------------
// 电流不平衡处理
#if( SEL_EVENT_I_UNBALANCE == YES )
// 判断电流不平衡是否发生
BYTE GetIUnBalanceStatus(BYTE Phase)
{
	BYTE Status;
	WORD dw_LimitUnBalance_I;
	Status = 0;
	dw_LimitUnBalance_I = 0;

	//国网规范要求至少有一相电流大于5%Ib
	if( MeterTypesConst == METER_3P3W )
    {
		if( (api_GetSampleStatus(0x11)&0x0005) == 0x05 )
		{
			return Status;
		}
    }
    else
    {
		if( (api_GetSampleStatus(0x11)&0x0007) == 0x07 )
		{
			return Status;
		}
    }
    
	// 电压不平衡率限值
	DealEventParaByEventOrder( READ, eEVENT_I_UNBALANCE_NO, eTYPE_WORD, (BYTE *)&dw_LimitUnBalance_I );
	//新规范中明确如果电压不平衡率限值为0，该事件不启用
	if(dw_LimitUnBalance_I == 0)
	{
		return Status;
	}

	if(api_GetSampleStatus(1) > dw_LimitUnBalance_I)
	{
		Status = 1;
	}

	return Status;
}

#endif

//------------------------------------------------------------------------------------------------------
// 各个事件处理专用部分－电流严重不平衡记录
//------------------------------------------------------------------------------------------------------
// 电流严重不平衡记录
#if( SEL_EVENT_I_S_UNBALANCE == YES )
// 判断电流严重不平衡记录是否发生
BYTE GetISUnBalanceStatus(BYTE Phase)
{
	BYTE Status;
	WORD dw_LimitUnBalance_I;
	Status = 0;
	dw_LimitUnBalance_I = 0;

	//国网规范要求至少有一相电流大于5%Ib
	if( MeterTypesConst == METER_3P3W )
    {
		if( (api_GetSampleStatus(0x11)&0x0005) == 0x05 )
		{
			return Status;
		}
    }
    else
    {
		if( (api_GetSampleStatus(0x11)&0x0007) == 0x07 )
		{
			return Status;
		}
    }

	// 电流不平衡率限值
	DealEventParaByEventOrder( READ, eEVENT_I_S_UNBALANCE_NO, eTYPE_WORD, (BYTE *)&dw_LimitUnBalance_I );
	//新规范中明确如果电流不平衡率限值为0，该事件不启用
	if(dw_LimitUnBalance_I == 0)
	{
		return Status;
	}

	if(api_GetSampleStatus(1) > dw_LimitUnBalance_I)
	{
		Status = 1;
	}

	return Status;
}
#endif
#endif//#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
