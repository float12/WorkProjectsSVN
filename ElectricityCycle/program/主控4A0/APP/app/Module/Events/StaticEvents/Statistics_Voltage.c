//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.10.20
//描述		电压合格率处理
//备注：未完全与事件event.c分离；待后续与event.c分离；融入到sample模块中
//----------------------------------------------------------------------

#include "AllHead.h"

// 电压合格率处理
#if( SEL_STAT_V_RUN == YES )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
__no_init DWORD ClrVRunDayTimer;	// 清日电压合格率定时计数器
__no_init DWORD ClrVRunMonTimer;	// 清月电压合格率定时计数器

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//总、A、B、C
static TStatVRunData DayStatVRunData[4];//电压日合格率统计
static TStatVRunData MonStatVRunData[4];//电压月合格率统计

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//计算合格率百分数
//Type			0: 日	1：月
//Phase:		0：总  	1：A	2：B	3：C
static void CalcVolTime( BYTE Type, BYTE Phase )
{
	DWORD tdw;
	TStatVRunData *p;
	
	if( Phase > 3 )
	{
		return;
	}
	
	if( Type == 0 )
	{
		p = &DayStatVRunData[Phase];
	}
	else
	{
		p = &MonStatVRunData[Phase];
	}
	
	if( p->dwMonitorTime == 0 )
	{
		p->wPassRate = 10000;//100%
		p->wOverRate = 0;
	}
	else
	{
		tdw = p->dwOverTime + p->dwBelowTime;
		p->wOverRate = tdw * 10000 / p->dwMonitorTime;
		if( p->wOverRate > 10000 )
		{
			p->wOverRate = 10000;
		}
		p->wPassRate = 10000 - p->wOverRate;
	}
}


//上电读出eeprom中存储的数据
void PowerUpVRunTime( void )
{
	DWORD dwAddr;
	if( (ClrVRunDayTimer <= 3) && (ClrVRunDayTimer > 0) )//清日电压合格率
	{
		api_ClearVRunTimeNow( BIT0 );
	}
	
	if( (ClrVRunMonTimer <= 3) && (ClrVRunMonTimer > 0) )//清月电压合格率
	{
		api_ClearVRunTimeNow( BIT1 );
	}

	ClrVRunDayTimer = 0;
	ClrVRunMonTimer = 0;
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.StatVRunDataRom.DayStatVRunData[0] );
	api_ReadFromContinueEEPRom( dwAddr, sizeof(DayStatVRunData), (BYTE *)&DayStatVRunData[0] );
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.StatVRunDataRom.MonStatVRunData[0] );
	api_ReadFromContinueEEPRom( dwAddr, sizeof(MonStatVRunData), (BYTE *)&MonStatVRunData[0] );
}


// 判断电压运行状态
// 返回状态 wStatus
// D0 - 1  A相超合格下限
// D1 - 1  B相超合格下限
// D2 - 1  C相超合格下限
// D3 - 0 暂不用，传0，原来 D3 - 1  总相超下限
// D4 - 1  A相超合格上限
// D5 - 1  B相超合格上限
// D6 - 1  C相超合格上限
// D7 - 0 暂不用，传0，原来 D7 - 1  总相超上限
// D8 - 1  A在考核范围以内
// D9 - 1  B在考核范围以内
// D10 - 1  C在考核范围以内
static WORD GetVolRunStatus(void)
{
	BYTE i;
	WORD wStatus,VolValue;

	wStatus = 0;
	for(i=0; i<3; i++)
	{
		if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
		{
			continue;
		}

		// 保留1位小数
		api_GetRemoteData( PHASE_A+0x1000*i+REMOTE_U, DATA_HEX, 1, sizeof(WORD), (BYTE*)&VolValue );

		if( ( VolValue >= gEventPara.wLimit[eSTATISTIC_V_PASS_RATE_NO] )||( VolValue <= gEventPara.VRunLimitLV ) )
		{
			continue;//如果本相已超考核范围，则不进行后面的超上限、超下限判断，进行下一相判断
		}

		// D8 - 1  A在考核范围以内
		// D9 - 1  B在考核范围以内
		// D10 - 1  C在考核范围以内
		wStatus |= ( 0x100 << i );

		// 电压合格率超上限
		if( VolValue > gEventPara.VRunRecoverHV )
		{
			wStatus |= (0x10<<i);
		}

		// 电压合格率超下限
		if( VolValue < gEventPara.VRunRecoverLV )
		{
			wStatus |= (0x01<<i);
		}
	}

	return wStatus;
}
//--------------------------------------------------
//功能描述:  计算日电压合格率
//         
//参数:     无
//         
//返回值:    无 
//         
//备注内容:  无
//--------------------------------------------------
void DayStatVRunTime(void)
{
	WORD i,wStatus;
	
	wStatus = GetVolRunStatus();
	// 三相超上限时间、超下限时间
	for(i=0; i<MAX_PHASE; i++)
	{
		if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
		{
			continue;
		}
		
		if( (DayStatVRunData[i+1].dwMonitorTime > 2880)//电压监测时间、超下限时间、超上限时间不能超过2880
		|| (DayStatVRunData[i+1].dwBelowTime > 2880)
		|| (DayStatVRunData[i+1].dwOverTime > 2880) )
		{
			memset( (BYTE*)&DayStatVRunData[i+1], 0x00, sizeof(TStatVRunData) );
		}
		else if( (DayStatVRunData[i+1].dwMonitorTime >= 1440)//电压监测时间、超下限时间、超上限时间不能超过1440
			  ||(DayStatVRunData[i+1].dwBelowTime >= 1440)
			  ||(DayStatVRunData[i+1].dwOverTime >= 1440) )
		{
			continue;//不进行累计
		}
		else//正常累计
		{

		}

		// D8 - 1  A在考核范围以内
		// D9 - 1  B在考核范围以内
		// D10 - 1  C在考核范围以内
		if( wStatus & ( 0x100 << i ) )
		{
			// 电压监测时间
			DayStatVRunData[i + 1].dwMonitorTime++;
		}

		if(wStatus & (0x01 << i))
		{
			// 电压超下限时间
			DayStatVRunData[i + 1].dwBelowTime++;
		}
		if(wStatus & (0x01 << (i + 4)))
		{
			// 电压超上限时间
			DayStatVRunData[i + 1].dwOverTime++;
		}
	}
	
	//计算分相合格率百分数
	for(i=0; i<MAX_PHASE; i++)
	{
		CalcVolTime( 0, 1+i );
	}
	
	//总电压合格率统计
	if( 	((MeterTypesConst==METER_3P3W) && ((wStatus&0x500)==0x500)) 
		|| 	((MeterTypesConst!=METER_3P3W) && ((wStatus&0x700)==0x700)) )
	{
		// 电压监测时间 有一相在考核范围以内，总的监测时间就++ 以前这样做 2010-12-14
		// 电压监测时间(合相)要三相电压均在电压考核上、下限内才统计时间++ 福建要求 2010-12-14
		DayStatVRunData[0].dwMonitorTime++;

		// 总超下限时间
		// D0 - 1  A相超下限
		// D1 - 1  B相超下限
		// D2 - 1  C相超下限
		// D3 - 0 暂不用，传0
		if(wStatus & 0x07)
		{
			DayStatVRunData[0].dwBelowTime++;
		}
		// 总超上限时间
		// D4 - 1  A相超上限
		// D5 - 1  B相超上限
		// D6 - 1  C相超上限
		// D7 - 0 暂不用，传0
		if(wStatus & 0x70)
		{
			DayStatVRunData[0].dwOverTime++;
		}

		//计算总电压合格率百分数
		CalcVolTime( 0, 0 );
	}

}

//--------------------------------------------------
//功能描述:  计算日电压合格率
//         
//参数:     无
//         
//返回值:    无 
//         
//备注内容:  无
//--------------------------------------------------
void MonStatVRunTime(void)
{
	WORD i,wStatus;
	
	wStatus = GetVolRunStatus();
	
	// 三相超上限时间、超下限时间
	for(i=0; i<MAX_PHASE; i++)
	{
		if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
		{
			continue;
		}
		
		if( (MonStatVRunData[i+1].dwMonitorTime > (2880*31))//电压监测时间、超下限时间、超上限时间不能超过2880
		|| (MonStatVRunData[i+1].dwBelowTime > (2880*31))
		|| (MonStatVRunData[i+1].dwOverTime > (2880*31)) )
		{
			memset( (BYTE*)&MonStatVRunData[i+1], 0x00, sizeof(TStatVRunData) );
		}
		else if( (MonStatVRunData[i+1].dwMonitorTime >= (1440*31))//电压监测时间、超下限时间、超上限时间不能超过1440
			  ||(MonStatVRunData[i+1].dwBelowTime >= (1440*31))
			  ||(MonStatVRunData[i+1].dwOverTime >= (1440*31)) )
		{
			continue;//不进行累计
		}
		else//正常累计
		{

		}
		 
		// D8 - 1  A在考核范围以内
		// D9 - 1  B在考核范围以内
		// D10 - 1	C在考核范围以内
		if( wStatus & ( 0x100 << i ) )
		{
			// 电压监测时间
			MonStatVRunData[i + 1].dwMonitorTime++;
		}

		if(wStatus & (0x01 << i))
		{
			// 电压超下限时间
			MonStatVRunData[i + 1].dwBelowTime++;
		}
		if(wStatus & (0x01 << (i + 4)))
		{
			// 电压超上限时间
			MonStatVRunData[i + 1].dwOverTime++;
		}
	}
	
	//计算分相合格率百分数
	for(i=0; i<MAX_PHASE; i++)
	{
		CalcVolTime( 1, 1+i );
	}
	
	//总电压合格率统计
	if( 	((MeterTypesConst==METER_3P3W) && ((wStatus&0x500)==0x500)) 
		||	((MeterTypesConst!=METER_3P3W) && ((wStatus&0x700)==0x700)) )
	{		
		// 电压监测时间 有一相在考核范围以内，总的监测时间就++ 以前这样做 2010-12-14
		// 电压监测时间(合相)要三相电压均在电压考核上、下限内才统计时间++ 福建要求 2010-12-14
		MonStatVRunData[0].dwMonitorTime++;

		// 总超下限时间
		// D0 - 1  A相超下限
		// D1 - 1  B相超下限
		// D2 - 1  C相超下限
		// D3 - 0 暂不用，传0
		if(wStatus & 0x07)
		{
			MonStatVRunData[0].dwBelowTime++;
		}
		// 总超上限时间
		// D4 - 1  A相超上限
		// D5 - 1  B相超上限
		// D6 - 1  C相超上限
		// D7 - 0 暂不用，传0
		if(wStatus & 0x70)
		{
			MonStatVRunData[0].dwOverTime++;
		}

		//计算总电压合格率百分数
		CalcVolTime( 1, 0 );
	}

}

// 统计电压合格率时间 分钟调用
void StatVRunTime(void)
{
	DayStatVRunTime( );
	MonStatVRunTime( );
}


// 读电压合格率数据
// Phase -- 要读取的相 0-总、1-A、2-B、3-C
// No ----- BIT0: 日数据  BIT1：月数据
// pBfu --- 存储读出的数据
// 返回 --- 实际读出的数据长度, 如果缓冲不够，返回0
BYTE ReadVRunTime(BYTE Phase, BYTE No, BYTE *pBuf)
{
	BYTE Length;

	Length = 0;
	
	if( (No&BIT0) != 0 )
	{
		memcpy( pBuf, &DayStatVRunData[Phase], sizeof(TStatVRunData) );
		Length += sizeof(TStatVRunData);
	}
	
	if( (No&BIT1) != 0 )
	{
		memcpy( pBuf+Length, &MonStatVRunData[Phase], sizeof(TStatVRunData) );
		Length += sizeof(TStatVRunData);
	}
	
	return Length;
}


// 清除当前数据
//Type 	BIT0：日数据		BIT1：月数据
void api_ClearVRunTimeNow( BYTE Type )
{
	WORD i;
	
	if( (Type&BIT0) != 0 )
	{
		memset( (void*)&DayStatVRunData[0], 0x00, sizeof(DayStatVRunData) );
		//计算分相合格率百分数
		for(i=0; i<(MAX_PHASE+1); i++)
		{
			CalcVolTime( 0, i );
		}
	}
	
	if( (Type&BIT1) != 0 )
	{
		memset( (void*)&MonStatVRunData[0], 0x00, sizeof(DayStatVRunData) );
		//计算分相合格率百分数
		for(i=0; i<(MAX_PHASE+1); i++)
		{
			CalcVolTime( 1, i );
		}
	}

	SwapVRunTime();//都存一下
}


// 电压合格率数据转存
void SwapVRunTime(void)
{
	DWORD dwAddr;
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.StatVRunDataRom.DayStatVRunData[0] );
	api_WriteToContinueEEPRom( dwAddr, sizeof(DayStatVRunData), (BYTE *)&DayStatVRunData[0] );
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.StatVRunDataRom.MonStatVRunData[0] );
	api_WriteToContinueEEPRom( dwAddr, sizeof(MonStatVRunData), (BYTE *)&MonStatVRunData[0] );
}


void FactoryInitStatisticsVPara( void )
{
	//没有OAD方面的初始化
}

#endif//#if( SEL_STAT_V_RUN == YES )

