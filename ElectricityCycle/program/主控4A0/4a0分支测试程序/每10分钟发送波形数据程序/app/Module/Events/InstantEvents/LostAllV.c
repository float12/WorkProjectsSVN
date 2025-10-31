//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.26
//描述		全失压事件处理文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "LostAllV.h"
#include "F4A0_LowPower_Three.h"

#if( SEL_EVENT_LOST_ALL_V == YES )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//掉电检测全失压状态标志
static __no_init DWORD PowerDown_LostALLVFlag;//0x12341234初始状态 0x5AA55AA5全失压 
__no_init TLostAllVData LostAllVData[MAX_LOST_ALL_V];

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
// 0x300d 全失压初始化的关联对象属性表，oad采用正序输入
static const DWORD LostAllVDefaultOadConst[] =
{
	1,//关联对象的个数
	0x20012200,	// 发生时刻电流

};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void TestLostAllV( void );

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能:同步电流值
//
//参数: 
//	Type[in]:	0、1、2、3代表A\B\C\N电流 
//	Value[in]:	电流值 4位小数 
 
//返回值:				无
//备注:每次采样时同步此值，供在处理全失压用
//-----------------------------------------------
void api_SetLostAllVCurrent( BYTE Type , long Value)
{
	if( Type > 3 )
	{
		return;
	}
	
	LostAllVData[0].Current[Type] = Value / 10;
}


void AllLostVCheck( BYTE Type )
{
	if( IsExitLowPower == 30 )
	{
		//只有交流电掉电后才进行全失压检测
		if( ((Type==eFromOnRunEnterDownMode)||(Type==eFromWakeupEnterDownMode)||(Type==eFromDetectEnterDownMode)) 
			&& (g_byDownPowerTime<(START_TEST_LOST_ALLV+10)) )
		{
			switch( g_byDownPowerTime )
			{
			case START_TEST_LOST_ALLV:
				api_MCU_SysClockInit( ePowerDownMode );//切换到5.5M时钟
				TestLostAllV();
				break;
			case (START_TEST_LOST_ALLV+1):
			case (START_TEST_LOST_ALLV+2):
				TestLostAllV();
				break;
			default:
				RESET_STACK_POINTER;
				api_EnterLowPower( eFromDetectEnterDownMode );
			}
		}
		else
		{
			IsExitLowPower = 0;
		}
	}
}
//上电全失压处理函数
//功能：根据掉电前全失压状态与低功耗检测状态，对全失压事件做相应的处理
//		进低功耗前状态				低功耗检测状态					上电处理
//		正常						未检测					不处理
//		正常						不是全失压				不处理
//		正常						全失压					记录全失压开始，置位全失压标志
//		全失压						未检测					置位全失压标志
//		全失压						是全失压				置位全失压标志
//		全失压						不是全失压				记录全失压结束

void PowerUpDealLostAllV( void )
{
	BYTE TmpOADBuf[10];
	DWORD LostALLV_LostPowerFlag,dwSec,dwSec1,dwSec2;//上电标志初始化为正常状态
	WORD Len;
	TRealTimer TmpRealTimer,TmpRealTimer2,TmpPowerDownRealTimer;
	TDLT698RecordPara TmpDLT698RecordPara;
	
    if( PowerDown_LostALLVFlag == eOtherFlag )//异常复位时按未检测到处理
    {
        PowerDown_LostALLVFlag = eNormalFlag;
    }
    else
    {
    	api_GetPowerDownTime( (TRealTimer *)&TmpPowerDownRealTimer );
    	dwSec = api_CalcInTimeRelativeSec( (TRealTimer *)&RealTimer );
    	dwSec1 = api_CalcInTimeRelativeSec( (TRealTimer *)&TmpPowerDownRealTimer );
    	if( dwSec > dwSec1 )
    	{
			dwSec2 = (dwSec-dwSec1);
    	}
    	else
    	{
			dwSec2 = 0;
    	}
    	
        if( dwSec2 >= 60 )
        {
            if( PowerDown_LostALLVFlag == eNormalFlag )//如果掉电超过1分钟，掉电检测标志为正常、置位检测到掉电
            {
                PowerDown_LostALLVFlag = eLostPowerFlag;
            }
        }
    }
    
    LostALLV_LostPowerFlag = eNormalFlag;//上电标志初始化为正常状态
    memset( (BYTE *)&TmpRealTimer2, 0, sizeof(TRealTimer) );//为啥是全0@@@@李丕凯
    memset( (BYTE *)&TmpRealTimer, 0xff, sizeof(TRealTimer) );   
    TmpOADBuf[0] = 0x20;
    TmpOADBuf[1] = 0x20;
    TmpOADBuf[2] = 0x02;
    TmpOADBuf[3] = 0x00;
    TmpDLT698RecordPara.pOAD = TmpOADBuf;//pOAD指针
    TmpDLT698RecordPara.OADNum = 1;		//OAD个数
    TmpDLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;//选择次数
    TmpDLT698RecordPara.TimeOrLast = 0x01;	//上一次
    TmpDLT698RecordPara.Phase = ePHASE_ALL;
    TmpDLT698RecordPara.OI = 0x300d;					
	Len = api_ReadProRecordData( 0, &TmpDLT698RecordPara, sizeof(TRealTimer), (BYTE*)&TmpRealTimer );
	if( (memcmp( (BYTE *)&TmpRealTimer, (BYTE *)&TmpRealTimer2, sizeof(TRealTimer) ) == 0)
	||(Len == 1) )//全失压事件最近一条记录结束时间为null
	{
        LostALLV_LostPowerFlag = eLostALLVFlag;
	}

    if( LostALLV_LostPowerFlag == eLostALLVFlag )//掉电检测的状态为未检测状态
    {
        if( PowerDown_LostALLVFlag == eNormalFlag )//掉电检测的状态为未检测到
        {
            //置位全失压标志
            EventStatus.CurrentStatus[eSUB_EVENT_LOST_ALL_V/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_ALL_V%8));
    	    api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V );
        }
        else if( PowerDown_LostALLVFlag == eLostALLVFlag )//掉电检测的状态为全失压
        {
            //置位全失压标志
            EventStatus.CurrentStatus[eSUB_EVENT_LOST_ALL_V/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_ALL_V%8));
    	    api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V );
        }
        else//其他情况按掉电检测的状态为掉电处理
        {
            //记录全失压结束
		    SaveInstantEventRecord( eEVENT_LOST_ALL_V_NO, 0, EVENT_END, eEVENT_ENDTIME_PWRDOWN );
        }
    }
    else//掉电前状态为正常状态
    {
        if( PowerDown_LostALLVFlag == eNormalFlag )//掉电检测的状态为未检测状态
        {
            //null 不处理
        }
        else if( PowerDown_LostALLVFlag == eLostALLVFlag )//掉电检测的状态为全失压
        {
            //记录全失压开始 置位全失压标志
		    SaveInstantEventRecord( eEVENT_LOST_ALL_V_NO, 0, EVENT_START, eEVENT_ENDTIME_PWRDOWN );
            EventStatus.CurrentStatus[eSUB_EVENT_LOST_ALL_V/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_ALL_V%8));
		    api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V );
        }
        else//其他情况按掉电检测的状态为掉电处理
        {
        
        }
    }
    
    memset( (void *)&LostAllVData, 0, sizeof(LostAllVData) );
    PowerDown_LostALLVFlag = eOtherFlag;//置位初始状态
}


void PowerDownDealLostAllV( void )
{
	// 观察大循环中的检测中，是否有全失压发生
	PowerDown_LostALLVFlag = eNormalFlag;//初始状态
	memset( (void *)&LostAllVData, 0, sizeof(LostAllVData) );
}


static void TestLostAllV( void )
{
//	BYTE i;
//	DWORD tdw;
//	float tf;
//
//	if( g_byDownPowerTime == START_TEST_LOST_ALLV )
//	{
//        InitPort(ePowerWakeUpMode);
//		POWER_SAMPLE_OPEN;
//		POWER_FLASH_OPEN;
//		//POWER_ESAM_OPEN;
//		api_Delayms(10);
//		CLEAR_WATCH_DOG;
//		api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
//		api_Delayms(10);
//
//		if( api_InitSampleChip(0) == FALSE )
//		{
//		    PowerDown_LostALLVFlag = eLostPowerFlag;
//		    g_byDownPowerTime = (START_TEST_LOST_ALLV+3);//跳过全失压检测
//			RESET_STACK_POINTER;
//			api_EnterLowPower( eFromDetectEnterDownMode );
//		}
//	}
//
//	if( g_byDownPowerTime==(START_TEST_LOST_ALLV+2) )
//	{
//        for(i=0; i<3; i++)
//    	{
//    		LostAllVData[0].Current[i] = 0;
//    		if( (MeterTypesConst==METER_3P3W) && (i == 1) )
//    		{
//    			continue;
//    		}
//
//    		// 读出电流
//    		tdw = HT7627S_ReadMeasureData( All_RMS_Ia + i*4 );
//    		if( tdw == 0x00ffffff )//读的值异常后 跳出不判断
//    		{
//               memset( (void *)&LostAllVData, 0, sizeof(LostAllVData) );
//               break;
//    		}
//    		
//    		tf = (float)tdw*10000/TWO_POW13;
//    		tf /= ((float)(6 * CURRENT_VALUE) / 5);
//    		LostAllVData[0].Current[i] = (DWORD)((tf * wMeterBasicCurrentConst) / 10000 + 0.5); //保留三位小数
//    	}
//    	
//    	//读计量芯片三相电流矢量和
//    	tdw = HT7627S_ReadMeasureData( All_RMS_In );
//	    if( tdw == 0x00ffffff )//读的值异常后 跳出不判断
//		{
//           memset( (void *)&LostAllVData, 0, sizeof(LostAllVData) );
//		}
//		else
//		{
//            tf = (float)tdw*10000/TWO_POW12;
//            tf /= ((float)(6 * CURRENT_VALUE) / 5);
//            LostAllVData[0].Current[3] = (DWORD)(tf * ((float)wMeterBasicCurrentConst / 10000) + 0.5); //保留三位小数
//		}
//    	
//    	for(i=0; i<3; i++)
//    	{
//    		if( LostAllVData[0].Current[i] > (DWORD)wMeterBasicCurrentConst * 5 / 100 ) //保留了3位小数 5%Ib
//    		{
//    			break;
//    		}
//    	}
//
//    	if( i < 3 )// 说明是全失压
//    	{
//    		PowerDown_LostALLVFlag = eLostALLVFlag;
//    	}
//    	else
//    	{
//    		PowerDown_LostALLVFlag = eLostPowerFlag;
//    	}
//
//		RESET_STACK_POINTER;
//		api_EnterLowPower( eFromDetectEnterDownMode );
//	}
}


// 全失压处理
// 全失压记录的状态检测
BYTE GetLostAllVStatus(BYTE Phase)
{
	BYTE Status, i;
	WORD wRemoteU;
	DWORD dwRemoteI;

	Status = 0;

	// 判断三相电压
	for(i=0; i<3; i++)
	{
		if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
		{
			Status |= 0x02;
			continue;
		}

		// 此相电压低于临界电压
        api_GetRemoteData( PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, sizeof(WORD), (BYTE *)&wRemoteU);
		if( wRemoteU < wStandardVoltageConst * 6 / 10 ) //0.6Un 1位小数
		{
			// 此相失压
			Status |= (0x01<<i);
		}
	}

	// 如果三相电压都低于临界电压，则判断电流
	if(Status == 0x07)
	{
		Status = 0;

		// 判断三相电流
		for(i=0; i<3; i++)
		{
			if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
			{
				continue;
			}

			api_GetRemoteData( PHASE_A+REMOTE_I+0x1000*i, DATA_HEX, 4, sizeof(DWORD), (BYTE*)&dwRemoteI);
			if( dwRemoteI > (DWORD)dwMeterBasicCurrentConst * 50 / 100 ) // 5%Ib 4位小数
			{
				Status |= (0x01<<i);
			}
		}

		// 若三相电流有一相大于失压电流门限
		if( Status != 0 )
		{
			// 认为全失压发生
			 Status = 1;
		}
	}
	else
	{
		Status = 0;
	}

	return Status;
}


//初始化全失压关联对象属性表
void FactoryInitLostAllVPara( void )
{
	api_WriteEventAttribute( 0x300d, 0xff, (BYTE *)&LostAllVDefaultOadConst[0], sizeof(LostAllVDefaultOadConst)/sizeof(DWORD) );	// 全失压
}

#endif//#if( SEL_EVENT_LOST_ALL_V == YES )
