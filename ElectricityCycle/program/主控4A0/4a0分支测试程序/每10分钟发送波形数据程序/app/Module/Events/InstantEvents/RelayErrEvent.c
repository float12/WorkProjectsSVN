//---------------------------------------------
// 开盖处理记录
//---------------------------------------------

#include "AllHead.h"

static const DWORD RelayErrOad[] =
{
#if( MAX_PHASE == 1)
	0x05,
	0xF2052201,    //负荷开关误动作时状态
	0x00102201,    //负荷开关误动作前正向有功总电能
	0x00202201,    //负荷开关误动作前反向有功总电能
	0x00108201,    //负荷开关误动作后正向有功总电能
	0x00208201     //负荷开关误动作后反向有功总电能
#else
	0x05,
	0xF2052201,    //负荷开关误动作时状态
	0x00102201,    //负荷开关误动作前正向有功总电能
	0x00202201,    //负荷开关误动作前反向有功总电能
	0x00108201,    //负荷开关误动作后正向有功总电能
	0x00208201     //负荷开关误动作后反向有功总电能

#endif
};

#if( SEL_EVENT_RELAY_ERR == YES )
void FactoryInitRelayErrPara( void )
{	
	api_WriteEventAttribute( 0x302B, 0xFF, (BYTE *)&RelayErrOad[0], sizeof(RelayErrOad)/sizeof(DWORD) );	
}
#endif
// 负荷开关误动处理公共部分
#if( SEL_EVENT_RELAY_ERR == YES )

//------------------------------------------------------------------------------------------------------
// 判断负荷开关误动是否发生
BYTE GetRelayErrStatus(BYTE Phase)
{
	//由继电器模块提供开关误动的状态
	// 继电器故障状态 //控制回路错误
	if( api_GetSysStatus(eSYS_STATUS_RELAY_ERR) == TRUE )
	{
		//本次操作继电器后已经记录了一次完整的记录了
		if(api_DealRelayErrFlag(READ,0x0000)& 0x00AA)
		{
			return 0;
		}
		return 1;
	}
	
	api_DealRelayErrFlag(WRITE,0x00AA);//内部有保护，未开始不会写入结束；只有开始的才可以写入结束			
	return 0;
}



#endif

