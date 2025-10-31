//---------------------------------------------
// 功率因数超限记录
//---------------------------------------------

#include "AllHead.h"

#if( SEL_EVENT_COS_OVER == YES )

static const DWORD CosOverOad[] =
{
#if( MAX_PHASE == 1)
	0x04,				// 个数
	0x00102201,    //事件发生时刻正向有功总电能
	0x00202201,    //事件发生时刻反向有功总电能
	0x00108201,    //事件结束后正向有功总电能
	0x00208201,    //事件结束后反向有功总电能
#else
	0x08,				// 个数
	0x00102201,    //事件发生时刻正向有功总电能
	0x00202201,    //事件发生时刻反向有功总电能
	0x00302201,    //事件发生时刻组合无功1总电能
	0x00402201,    //事件发生时刻组合无功2总电能
	0x00108201,    //事件结束后正向有功总电能
	0x00208201,    //事件结束后反向有功总电能
	0x00308201,    //事件结束后组合无功1总电能
	0x00408201     //事件结束后组合无功2总电能
#endif
};

//------------------------------------------------------------------------------------------------------
// 各个事件处理专用部分－有功功率反向
//------------------------------------------------------------------------------------------------------
void FactoryInitCosOverPara( void )
{
	api_WriteEventAttribute( 0x303B, 0xFF, (BYTE *)&CosOverOad[0], sizeof(CosOverOad)/sizeof(DWORD) );
}

// 判断功率因数超限是否发生,国网20规范中三相三线情况下，不判断分相功率因数超下限，三相四线判断
BYTE GetCosoverStatus(BYTE Phase)
{
	BYTE Status;
	WORD Type;
	WORD w_LimitCos,w_RemoteCos;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	Status = 0;
	if(Phase>ePHASE_C)
	{
		return Status;
	}
	// 功率因数超下限阀值
	DealEventParaByEventOrder( READ, eEVENT_COS_OVER_NO, eTYPE_WORD, (BYTE *)&w_LimitCos );
	//新规范中明确如果功率因数超下限阀值为0，该事件不启用
	if( w_LimitCos == 0 )
	{
		return 0;
	}
	//规范要求功率因数超限，该相电流大于 5%额定（基本）电流
    if( Phase == ePHASE_ALL )//如果是总功率因数、
    {
        if( MeterTypesConst == METER_3P3W )
        {
    		if( (api_GetSampleStatus(0x11)&0x0005) == 0x05 )
    		{
    			return 0;
    		}
        }
        else
        {
    		if( (api_GetSampleStatus(0x11)&0x0007) == 0x07 )
    		{
    			return 0;
    		}
        }

    }
	else 
    {
        if( ( (api_GetSampleStatus(0x11)&0x0007) & (0x0001<<(Phase-1)) ) != 0 )
		{
			return 0;
		}
    }
	// 读出功率因数进行判断
	if( Phase == ePHASE_ALL )
	{
		Type = PHASE_ALL+REMOTE_COS;
	}
	else if( Phase == ePHASE_A )
	{
		Type = PHASE_A+REMOTE_COS;	
	}
	else if( Phase == ePHASE_B )
	{
		Type = PHASE_B+REMOTE_COS;	
	}
	else// if( Phase == ePHASE_C )
	{
		Type = PHASE_C+REMOTE_COS;
	}
	api_GetRemoteData(Type, DATA_HEX, 3, 2, (BYTE *)&w_RemoteCos);
	if( w_RemoteCos < w_LimitCos)
	{
		Status = 1;
	}

	return Status;
}


#endif
