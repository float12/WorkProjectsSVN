//------------------------------------------------------------------------------------------------------------
// 单相表零火线电流不平衡记录函数 
//------------------------------------------------------------------------------------------------------------

#include "AllHead.h"
#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
static const DWORD NeutralCurrentErrOad[] =
	#if(MAX_PHASE == 1)
	{
		0x02,				// 个数
		0x20012201, // 事件发生时刻A相电流
		0x20012400, // 事件发生时刻零线电流
	};
	#else
	{
		0x02,				// 个数
		0x20012400, // 事件发生时刻零线电流
		0x20012600, // 事件发生时刻零序电流（三相电流矢量和）
	};
	#endif
#endif

#if( SEL_EVENT_SP_I_UNBALANCE == YES )
// 判断单相表零火线电流不平衡 
BYTE GetSPIUnbalanceStatus(void)
{
	/*
	WORD w;
	BYTE b, BigChn;
	DWORD dI[2];
  DWORD d;

	dI[0] = RamDBase.RemoteData.I[0];
	dI[1] = GetZeroCurr();
	
	//-----------------------
	// 得到两个通道差值，同时找到电流较大通道 
	if( dI[0] > dI[1] )
	{
		d = dI[0] - dI[1];
		BigChn = 0;
	}
	else
	{
		d = dI[1] - dI[0];
		BigChn = 1;
	}
	
	//-----------------------
	// 判断电流是否超过死区值 
	DealThresholdPara(0, 0xda, 2, (BYTE *)&w);
	
	// 最大电流小于启动门限 
	if( dI[BigChn] <= w )
	{
		// 则不进行不平衡判断 

		// 清状态字 
		api_ClrSysStatus(SYS_STATUS_SPI_UN_B);
		return 0;
	}
	
	//-----------------------
	// 取比值 
	d = (d*100) / dI[BigChn];
	
	//-----------------------
	#if( SEL_PARA_SYS_THRESHOLD == YES )
	// 读取不平衡门限值	
	DealThresholdPara(0, 0xDD, 1, (BYTE *)&b);
	#else
	// 如不可设,则默认为50%
	b = 50;
	#endif
	
	// 是否超过门限 	
	if(d > b)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	*/
	return 0;
}

// 单相表零火线电流不平衡开始 
void DealSPIUnbalanceBegin(void)
{
	/*
	BYTE i, Point;
	WORD TempI[2];
	DWORD Addr;
	
	//-------------------------------------
	// 取得指针
	Point = DealEventPoint(0, EVENT_SP_I_UNBALANCE_NO, 0, 0);
	
	// 此事件记录存储结构的开始地址
	Addr = GetEventDataExtAddr(EVENT_SP_I_UNBALANCE_NO, 0, Point);

	// 记录发生时刻电流值 
	for(i=0; i<2; i++)
	{
		// 电流i 
		TempI[i] = (WORD)(CS5464_I[i]/10);
		
		// 转换BCD码 
		TempI[i] = lib_WBinToBCD(TempI[i]);
	}
	
	// 写入
	api_WriteToContinueEEPRom(Addr, sizeof(TempI), (BYTE*)&(TempI[0]));
	*/
}

// 单相表零火线电流不平衡结束 
void DealSPIUnbalanceEnd(void)
{
	// 没有附属数据，不需要处理
}

// 读取单相表电流不平衡记录  
// No -- 0：总次数、累计时间 
//     1-A：上n次 
// pBfu --- 存储读出的数据 
// 返回 --- 实际读出的数据长度 
BYTE ReadSPIUnbalance(BYTE No, BYTE BufLen, BYTE *pBuf)
{
	/*
	BYTE Len, Point;
	DWORD Addr;
	
	if(No > EVENT_MAX_TIMES)
	{
		return 0xff;
	}
	
	if(No == 0)
	{
		return ReadEventCommon(EVENT_SP_I_UNBALANCE_NO, 0, 0, 1, 0, BufLen, pBuf);
	}
	else
	{
		// 取得指针
		Point = DealEventPoint(2, EVENT_SP_I_UNBALANCE_NO, 0, No);
		
		// 读标志位
		if(DealEventFlag(0, EVENT_SP_I_UNBALANCE_NO, 0, Point) == FALSE)
		{
			// 数据长度 
			Len = 12 + sizeof(TSPIUnbalanceData);
		
			// 零数据
			memset(pBuf, 0, Len);
		}
		else
		{
			// 读取时间
			Len = ReadEventCommon(EVENT_SP_I_UNBALANCE_NO, 1, 0, 1, No, 12, pBuf);
			
			// 保存的数据块地址
			Addr = GetEventDataExtAddr(EVENT_SP_I_UNBALANCE_NO, 0, Point);
			
			// 读取电流值
			Len += api_ReadFromContinueEEPRom(Addr, sizeof(TSPIUnbalanceData), (pBuf + Len));	
		}	
	
		// 保护
		ASSERT( Len <= BufLen, 1 );
		
		return Len;
	}
	*/
	return 0;
}

#endif

#if(SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )

//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void FactoryInitNeutralCurrentErrPara( void )
{	
	api_WriteEventAttribute( 0x3040, 0xFF, (BYTE *)&NeutralCurrentErrOad[0], sizeof(NeutralCurrentErrOad)/sizeof(DWORD) );// 零线电流异常
	
}
//-----------------------------------------------
//函数功能: 判断是否零线电流不平衡
//
//参数:  无
//                    
//返回值:	掉电状态		FALSE：正常
//					TRUE：零线电流不平衡状态
//
//备注: 
//-----------------------------------------------
BYTE GetNeutralCurrentErrStatus(BYTE Phase)
{
	DWORD dwRemoteI_A,dwRemoteI_N,dwRemoteI_Max,dwRemoteI_Dis,dwLimitI;
	WORD wRatioI;//读取的触发比例

	//拥有零线电流计量功能
	if(SelZeroCurrentConst == NO )
	{
		return FALSE;
	}
	//零线电流异常不平衡率触发限值
	DealEventParaByEventOrder( READ, eEVENT_NEUTRAL_CURRENT_ERR_NO, eTYPE_WORD, (BYTE *)&wRatioI );
	if(wRatioI == 0)//不平衡率触发限值为0表示不启用
	{
		return FALSE;
	}
	//零线电流异常电流触发下限
	DealEventParaByEventOrder( READ, eEVENT_NEUTRAL_CURRENT_ERR_NO, eTYPE_DWORD, (BYTE *)&dwLimitI );
	
	#if(MAX_PHASE == 1)
	//读A相电流
	api_GetRemoteData(PHASE_A+REMOTE_I, DATA_HEX, 4, 4, (BYTE *)&dwRemoteI_A);
	#else
	//读零序电流
	api_GetRemoteData(PHASE_N2+REMOTE_I,DATA_HEX, 4, 4, (BYTE *)&dwRemoteI_A);
	#endif
	//读N相电流
	api_GetRemoteData(PHASE_N+REMOTE_I, DATA_HEX, 4, 4, (BYTE *)&dwRemoteI_N);
	if(dwRemoteI_A > dwRemoteI_N)//读的数据格式为DATA_HEX，确保了电流为正值     
	{
		dwRemoteI_Max = dwRemoteI_A;
		dwRemoteI_Dis =	dwRemoteI_A - dwRemoteI_N;
	}
	else
	{
		dwRemoteI_Max = dwRemoteI_N;
		dwRemoteI_Dis =	dwRemoteI_N - dwRemoteI_A;
	}
	if(dwRemoteI_Max <= dwLimitI)//零线电流和火线电流中任一电流大于电流触发下限
	{
		return FALSE;
	}

	//计算零线电流与火线电流的比值，
    if((WORD)((QWORD)dwRemoteI_Dis * 10000 / dwRemoteI_Max ) > wRatioI) 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#endif
