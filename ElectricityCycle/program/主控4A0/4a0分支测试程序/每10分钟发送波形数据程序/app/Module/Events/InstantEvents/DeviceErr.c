//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.11.14
//描述		外围器件异常源文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

#if( (SEL_EVENT_RTC_ERR == YES)||(SEL_EVENT_SAMPLECHIP_ERR == YES) )

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
#if( SEL_EVENT_RTC_ERR == YES )
static const DWORD RtcErrDefaultOadConst[] =
{
	0x04,//关联对象的个数
	0x00102201, 	   //事件发生时刻正向有功总电能
	0x00202201, 	   //事件发生时刻反向有功总电能
	0x00108201, 	   //事件结束后正向有功总电能
	0x00208201, 	   //事件结束后反向有功总电能
};
#endif

#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
static const DWORD SampleChipErrDefaultOadConst[] =
{
	0x02,//关联对象的个数
	0x00102201, 	   //事件发生时刻正向有功总电能
	0x00202201, 	   //事件发生时刻反向有功总电能
};
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 获取当前时钟故障状态
//
//参数:  
//  Phase[in]: 无效          
//   
//返回值:	1   有故障
//			0	无故障
//
//备注: 
//-----------------------------------------------
#if( SEL_EVENT_RTC_ERR == YES )
BYTE GetRtcErrStatus(BYTE Phase)
{
	//时钟芯片损坏或者上电时间错误
	if((api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )
	|| (api_GetRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG) == TRUE ))	
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
//-----------------------------------------------
//函数功能: 时钟故障事件的上电处理
//
//参数:  无
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
void RtcErrPowerOn( void )
{
	BYTE TmpOADBuf[10];
	WORD Len;
	TRealTimer TmpRealTimer,TmpRealTimer2;
	TDLT698RecordPara TmpDLT698RecordPara;
	
	memset( (BYTE *)&TmpRealTimer2, 0, sizeof(TRealTimer) );
	memset( (BYTE *)&TmpRealTimer, 0xff, sizeof(TRealTimer) );	 
	TmpOADBuf[0] = 0x20;
	TmpOADBuf[1] = 0x20;
	TmpOADBuf[2] = 0x02;
	TmpOADBuf[3] = 0x00;
	TmpDLT698RecordPara.pOAD = TmpOADBuf;//pOAD指针
	TmpDLT698RecordPara.OADNum = 1; 	//OAD个数
	TmpDLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;//选择次数
	TmpDLT698RecordPara.TimeOrLast = 0x01;	//上一次
	TmpDLT698RecordPara.Phase = ePHASE_ALL;
	TmpDLT698RecordPara.OI = 0x302e;					
	Len = api_ReadProRecordData( 0, &TmpDLT698RecordPara, sizeof(TRealTimer), (BYTE*)&TmpRealTimer );
	if( (memcmp( (BYTE *)&TmpRealTimer, (BYTE *)&TmpRealTimer2, sizeof(TRealTimer) ) == 0)
	||(Len == 1) )//时钟故障事件最近一条记录结束时间为null
	{
	
		//置位时钟故障事件标志
		EventStatus.CurrentStatus[eSUB_EVENT_RTC_ERR/8] |= (BYTE)(0x01<<(eSUB_EVENT_RTC_ERR%8));
		api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_RTC_ERR );
	}

}


//-----------------------------------------------
//函数功能: 初始化时钟故障事件参数
//
//参数:  无
//   
//返回值:	无
//
//备注: 
//-----------------------------------------------
void api_FactoryInitRtcErrPara( void )
{
	api_WriteEventAttribute( 0x302e, 0xff, (BYTE *)&RtcErrDefaultOadConst[0], sizeof(RtcErrDefaultOadConst)/sizeof(DWORD) );
}
#endif

//-----------------------------------------------
//函数功能: 获取当前计量芯片故障状态
//
//参数:  
//  Phase[in]: 无效          
//   
//返回值:	1   有故障
//			0	无故障
//
//备注: 
//-----------------------------------------------
#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
BYTE GetSampleChipErrStatus(BYTE Phase)
{
	if( api_CheckError(ERR_CHECK_5460_1) == TRUE )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//-----------------------------------------------
//函数功能: 初始化计量芯片故障事件参数
//
//参数:  无
//   
//返回值:	无
//
//备注: 
//-----------------------------------------------
void FactoryInitSampleChipErrPara( void )
{
	api_WriteEventAttribute( 0x302f, 0xff, (BYTE *)&SampleChipErrDefaultOadConst[0], sizeof(SampleChipErrDefaultOadConst)/sizeof(DWORD) );
}
#endif

#endif

