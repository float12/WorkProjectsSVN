//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.8.17
//描述		

//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"

#if( PREPAY_MODE == PREPAY_LOCAL )

const DWORD DefRatePrice[MAX_RATIO] = {5000, 5000, 5000, 5000};
const DWORD DefLadderDl[MAX_LADDER] = {2880, 4800, 4800, 4800, 4800, 4800};
const DWORD DefLadderPrice[MAX_LADDER+1] = {0, 500, 3000, 3000, 3000, 3000, 3000};
const BYTE  DefCstYearBill[MAX_YEAR_BILL][3] = {1,1,0};//1月1日0时

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

typedef enum
{
	eNoLadderMode = 0,			//无阶梯模式
	eLadderMonMode = 0x55,     //月阶梯模式
	eLadderYearMode = 0xaa,    //年阶梯模式
}eLadderMode; 

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------																						



//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------


//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
__no_init TCurMoneyBag  CurMoneyBag @ (0x20000000+2*sizeof(TEnergyRam)); //钱包文件、已扣费脉冲数 固定地址 跟在EnergyRam后 但是留了一倍余量 
__no_init TRatePrice	CurRateTable;		//当前费率电价表
__no_init TLadderPrice	CurLadderTable;		//当前阶梯电价表
__no_init TCurPrice		CurPrice;			//当前电价 用于扣费
__no_init TLimitMoney	LimitMoney;         //金额门限参数
TCardInfo               CardInfo; //卡信息（开户状态、卡序列号）
eChangeCurPriceFlag  	UpdatePriceFlag;    	//电价刷新判断标志

TLocalStatus			LocalStatus;		//本地状态

TLadderPrice			PowerdownLadderTable;	//掉电前阶梯电价表

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
extern BYTE				PrePayPowerOnTimer;     //上电计时，从30减减
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void CheckCurMoneyBag( ePOWER_MODE Type );    //剩余金额参数校验
//
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 保存剩余金额数据到ee
//
//参数: 	无
//
//返回值:	无
//
//备注:
//-----------------------------------------------
static BOOL SaveCurMoneyBag(void)
{
	BYTE Result;
	
	Result = FALSE;

	if( CurMoneyBag.CRC32 == lib_CheckCRC32( (BYTE*)&CurMoneyBag , sizeof(TCurMoneyBag)-sizeof(DWORD) ) )//存储剩余金额
	{
		api_SetSysStatus(eSYS_STATUS_EN_WRITE_MONEY);
		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.CurMoneyBag ), sizeof(TCurMoneyBag), (BYTE *)&CurMoneyBag );
		api_ClrSysStatus(eSYS_STATUS_EN_WRITE_MONEY);
	}
	
	return Result;
}

//--------------------------------------------------
//功能描述:  esam金额扣款函数
//
//参数:      BYTE* InBuf[in]//4字节金额  输入buf可与输出buf为同一指针，但要注意输出buf的长度
//
//返回值:	OutBuf	2字节长度+4字节扣款金额+4字节交易金额
//
//备注内容:
//--------------------------------------------------
static BOOL ReduceEsamMoney( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE TmpBuf[15];
	BOOL Result;
	WORD ReturnLen;

	api_ProcPrePayCommhead( TmpBuf, 0x80, 0x46, 0x00, 0x00, 0x00, 0x04 );
	lib_ExchangeData( TmpBuf + 6, InBuf, 4 ); //取4字节扣款金额
	Result = Link_ESAM_TxRx( 6, TmpBuf, 4, TmpBuf + 6,  OutBuf );
	if( Result == FALSE )
	{
		return FALSE;
	}

	lib_ExchangeData( (BYTE *)&ReturnLen, OutBuf, 2 );
	memmove( OutBuf, OutBuf + 2, ReturnLen );

	return TRUE;
}

//-----------------------------------------------
//函数功能: RAM与ESAM进行钱包同步
//
//参数:		无
//
//返回值:	无
//
//备注:当ram与esam中购电次数相等且ram金额大于esam时，将esam扣减差值。反之将esam中数值覆盖ram中数值
//-----------------------------------------------
static WORD SyncMoneyBagToEsam( void )
{
	BYTE MoneyBagBuf[20];
	TFourByteUnion ESAMMoney,ESAMBuyTimes,RamMoney;

	if( ReadEsamMoneyBag( MoneyBagBuf ) != TRUE ) //读钱包剩余金额(1)和购电次数(3)
	{
		return FALSE;
	}
	
	lib_ExchangeData((BYTE *)&ESAMMoney, MoneyBagBuf, 4);
	lib_ExchangeData((BYTE *)&ESAMBuyTimes, MoneyBagBuf+4, 4);
	
	if( CurMoneyBag.Money.Remain.ll > 0 )
	{
        RamMoney.d = (DWORD)(CurMoneyBag.Money.Remain.ll/10000);
	}
	else
	{
        RamMoney.d = 0;
	}
	
	//购电次数相等同时剩余ram<钱包,以ram为准
	if( (ESAMBuyTimes.d == CurMoneyBag.Money.BuyTimes)&&(ESAMMoney.d > RamMoney.d) )
	{		
		ESAMMoney.d -= RamMoney.d;		//钱包 - 剩余
		
		ReduceEsamMoney( ESAMMoney.b, MoneyBagBuf );
		lib_ExchangeData( ESAMMoney.b, MoneyBagBuf, 4 );
		
		if( ESAMMoney.d != RamMoney.d )//判断是否扣减成功
		{
			return FALSE;
		}
	}
	else if( (ESAMBuyTimes.d != CurMoneyBag.Money.BuyTimes)||(ESAMMoney.d < RamMoney.d) )//次数不相等或esam金额小于ram金额时以esam为准
	{		
		CurMoneyBag.Money.Remain.ll = (long long)ESAMMoney.d*10000;
		CurMoneyBag.Money.BuyTimes = ESAMBuyTimes.d;	
		CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE*)&CurMoneyBag, (sizeof(TCurMoneyBag) -sizeof(DWORD)));
		
		SaveCurMoneyBag();//保存剩余金额数据到ee
		
		api_WriteSysUNMsg( SYSUN_FIX_MONEY );	
	}
		
	return TRUE;	
}

//-----------------------------------------------
//函数功能: 计量钱包同步
//
//参数:     无
//                   
//返回值:   无 
//
//备注:   
//-----------------------------------------------
void MeteringUpdateMoneybag(void)
{
	if( SyncMoneyBagToEsam() != TRUE )
	{
		ASSERT( 0 != 0, 0 ),
		api_ResetEsamSpi();
		SyncMoneyBagToEsam();  //再次钱包处理
	}
}

//-----------------------------------------------
//函数功能: 更新ram中的阶梯电量/电价/年结算日/CRC
//
//参数: 	Type[in]:ePowerOnMode/ePowerDownMode
//
//返回值:  	无
//
//备注:   程序中对阶梯值进行了排序处理
//-----------------------------------------------
static void CheckLadderPriceRamData( ePOWER_MODE Type )
{
	BYTE i, k, Flag, Crc_Flag;
	DWORD Change_Tmp;
	
	if( CurLadderTable.CRC32 != lib_CheckCRC32((BYTE*)&CurLadderTable,sizeof(TLadderPrice)-sizeof(DWORD)) )//内存校验错误 或者发生阶梯切换时 从EEPROM恢复电价
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentLadder ), sizeof(TLadderPrice), (BYTE *)&CurLadderTable ) != TRUE )//读出当前阶梯表
		{
			//for debug @@@
			memcpy( (BYTE *)&CurLadderTable.Ladder_Dl,(BYTE *)&DefLadderDl,sizeof(DefLadderDl) );
			memcpy( (BYTE *)&CurLadderTable.Price, (BYTE *)&DefLadderPrice, sizeof(DefLadderPrice) );
			memcpy( CurLadderTable.YearBill, DefCstYearBill, sizeof(DefCstYearBill) );
		}
		
		if( Type == ePowerOnMode )
		{
			ProcLadderModeSwitch();
		}
	}
	else
	{
		return;
	}
	
	Crc_Flag = 0;
	for( i=MAX_LADDER-1; i>0; i-- )//对阶梯值进行升序排列
	{
		Flag = 0;
		for(k=0;k<i;k++)
		{
			if( ((CurLadderTable.Ladder_Dl[k] > CurLadderTable.Ladder_Dl[k+1]) && (CurLadderTable.Ladder_Dl[k+1] != 0)) ||     //为0则不交换
                ((CurLadderTable.Ladder_Dl[k+1] > CurLadderTable.Ladder_Dl[k]) && (CurLadderTable.Ladder_Dl[k] == 0))        ) //把没有设置的阶梯值放到后面
			{
				Change_Tmp = CurLadderTable.Ladder_Dl[k];
				CurLadderTable.Ladder_Dl[k] = CurLadderTable.Ladder_Dl[k+1];
				CurLadderTable.Ladder_Dl[k+1] = Change_Tmp;
				
				Flag = 1;
				Crc_Flag = 0x55; //如果位置调整 重新计算CRC
			}
		}
		if( Flag == 0 )
		{
			break;			// 无交换
		}
	}
    //把没有设置的阶梯值赋值为最后一个设置的阶梯值
    for( k=1; k<MAX_LADDER; k++ )
    {
        if( CurLadderTable.Ladder_Dl[k] == 0 )
        {
            CurLadderTable.Ladder_Dl[k] = CurLadderTable.Ladder_Dl[k-1];
            Crc_Flag = 0x55; //如果位置调整 重新计算CRC
        }
    }
    
	if( Crc_Flag == 0x55 ) //如果位置有过调整 重新计算CRC
	{
		CurLadderTable.CRC32 = lib_CheckCRC32((BYTE*)&CurLadderTable, sizeof(TLadderPrice)-sizeof(DWORD));
	}
}

//-----------------------------------------------
//函数功能: 更新RAM中的费率电价
//
//参数: 	无
//                   
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void CheckRatePriceRamData( void )
{
	if( CurRateTable.CRC32 != lib_CheckCRC32( (BYTE *)&CurRateTable, sizeof(TRatePrice) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentRate ), sizeof(TRatePrice), (BYTE *)&CurRateTable ) != TRUE )//读出当前阶梯表
		{
			memcpy( (BYTE *)CurRateTable.Price, (BYTE *)&DefRatePrice, sizeof(DefRatePrice) );//for debug @@@
		}
	}
}

//-----------------------------------------------
//函数功能: 保存累计购电金额
//
//参数:
//			lValue[in],正数：购电金额，负数：退费金额
//返回值:		无
//
//备注:
//-----------------------------------------------
void SaveTotalBuyMoney( long lValue )
{
	DWORD dwData;

	api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD), (BYTE *)&dwData );
	dwData = dwData + lValue;
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD), (BYTE *)&dwData );
}


//-----------------------------------------------
//函数功能: 保存购电前剩余金额透支金额等数据
//
//参数:  		
//
//				Type[in]
//返回值:		无
//		   
//备注:
//-----------------------------------------------
static void SaveBuyMoneyPreData(eUpdataMoneyMode Type)
{
	//用于保存202C2201——购电前剩余金额  
	//202D2200——购电前透支金额 
	//202E2200——购电前累计购电金额 
	//按规范多保存购电前的购电次数，避免用户扩展不满足
	DWORD dwPreData[4];

	memset( (BYTE *)&dwPreData, 0X00, sizeof(dwPreData) );

	if( Type != eInitMoneyMode )//预置金额，用于参数预置卡、钱包初始化调用
	{	
		if( CurMoneyBag.Money.Remain.ll >= 0 )
		{
			dwPreData[0] = CurMoneyBag.Money.Remain.ll/10000;//WORD dwPreRemain;			//202C2201——购电前剩余金额 
		}
		else
		{	
			dwPreData[1] = 	(-1)*CurMoneyBag.Money.Remain.ll/10000;//DWORD dwPreTickMoney;		//202D2200——购电前透支金额 
		}
		api_ReadTotalBuyMoney( (BYTE *)&dwPreData[2]);	//DWORD dwPreTotalBuyMoney;	//202E2200——购电前累计购电金额 
		dwPreData[3] = CurMoneyBag.Money.BuyTimes;//按规范多保存购电前的购电次数，避免用户扩展不满足
	}
	
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[4] ), sizeof(dwPreData), (BYTE *)&dwPreData[0]);
}
//-----------------------------------------------
//函数功能: 保存退费前剩余金额
//
//参数:  		
//
//				Type[in]
//返回值:		无
//		   
//备注:
//-----------------------------------------------
static void SaveReturnMoneyPreData(void)
{ 

	DWORD dwPreData;

	dwPreData= 0;

	if( CurMoneyBag.Money.Remain.ll >= 0 )
	{
		dwPreData = CurMoneyBag.Money.Remain.ll/10000;//WORD dwPreRemain;			//202C2201——购电前剩余金额 
	}

	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[4] ), sizeof(dwPreData), (BYTE *)&dwPreData);
}

//-----------------------------------------------
//函数功能:     充值或更新剩余金额
//
//参数: 	    
//				Type[in] 			
// 					eReturnMoneyMode  - 退费操作
// 					eChargeMoneyMode  - 充值金额
// 					eInitMoneyMode     - 预置金额                
// 		        ReduceValue[in] 	扣除金额                 
//返回值:    无
//
//备注:   
//-----------------------------------------------
BOOL api_UpdataRemainMoney( eUpdataMoneyMode Type, DWORD ReduceValue, DWORD BuyTimes )
{
	DWORD	dw;

	CheckCurMoneyBag( ePowerOnMode );   //剩余金额参数校验

	if( Type > eInitMoneyMode )
	{
		return FALSE;
	}

    if( Type == eInitMoneyMode )//预置金额，钱包初始化
    {
		//保存购电前剩余金额透支金额等数据
		SaveBuyMoneyPreData(eInitMoneyMode);
		//置当前钱包文件
        CurMoneyBag.Money.Remain.ll = (long long)ReduceValue*10000;
        CurMoneyBag.Money.BuyTimes = 0;
        CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE*)&CurMoneyBag, sizeof(TCurMoneyBag)-sizeof(DWORD) );
		//置累计购电金额（保存于eeprom）
		dw = CurMoneyBag.Money.Remain.ll / 10000;
		api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD), (BYTE *)&dw ); 

		api_InitRelay();
    }
	else //充值或退费
    {
		if( Type == eReturnMoneyMode )//退费
		{
			SaveReturnMoneyPreData();
			CurMoneyBag.Money.Remain.ll -= (long long)ReduceValue * 10000;

			api_WritePreProgramData( 1, ReduceValue ); //保存退费金额
			api_SavePrgOperationRecord( ePRG_RETURN_MONEY_NO ); //电能表退费记录

			SaveTotalBuyMoney( (long)(-1 * ReduceValue) ); //保存累计购电金额
		}
		else//充值
		{
			SaveBuyMoneyPreData( eChargeMoneyMode ); //保存购电前剩余金额透支金额等数据
			CurMoneyBag.Money.Remain.ll += (long long)ReduceValue * 10000;
			CurMoneyBag.Money.BuyTimes = BuyTimes;
			SaveTotalBuyMoney( ReduceValue ); //保存的是购电后的累计金额
			api_SavePrgOperationRecord( ePRG_BUY_MONEY_NO ); //购电记录有购电前数据、购电后数据，能不能参考逆相序做，但购电前后数据排名比较乱
		}

		CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE *)&CurMoneyBag, sizeof(TCurMoneyBag) - sizeof(DWORD) );
		MeteringUpdateMoneybag(); 
    }
	
	if( SaveCurMoneyBag() == FALSE ) //保存剩余金额
	{
		return FALSE;
	}

	JudgeLocalStatus( Type, 0xFF ); //本地状态刷新
	
    return TRUE;

}
//-----------------------------------------------
//函数功能: 判断阶梯结算日检查是否是月阶梯，要求传入的必须是阶梯第1结算日
//
//参数:  	无
//
//返回值:	eLadderMode:	eNoLadderMode/eLadderMonMode/eLadderYearMode
//
//备注:   	小时变化调用
//-----------------------------------------------
static eLadderMode JudgeLadderMode( void )
{
	BYTE i;

	//结算日 0--月 1--日  2--时
	for( i = 0; i < MAX_YEAR_BILL; i++ )
	{
		if( (CurLadderTable.YearBill[i][1] <= 28)&&(CurLadderTable.YearBill[i][1] > 0) //日
			&&(CurLadderTable.YearBill[i][2] <= 23) ) //时
		{
			if( (CurLadderTable.YearBill[i][0] <= 12)&&(CurLadderTable.YearBill[i][0] > 0) ) //月
			{
				return eLadderYearMode;//年模式
			}
			else if( i == 0 ) //如果第一阶梯结算日 月无效 说明是月阶梯模式 只判日时即可
			{
				return eLadderMonMode;//月模式
			}
		}
	}

	return eNoLadderMode; //既不是年阶梯、又不是月阶梯
}

//---------------------------------------------------------------
//函数功能: 获得当前阶梯用电量
//
//参数: 	无
//
//返回值:	当前阶梯用电量
//
//备注:   根据组合模式字返回
//---------------------------------------------------------------
DWORD GetCurLadderUserEnergy( void )
{
	DWORD dwUserEnergy;
	
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == FALSE )
	{
		//由EEProm恢复参数
		if( api_FreshParaRamFromEeprom( 1 ) != TRUE )
		{
			api_SetError( ERR_FPARA2 );
		}
	}

	dwUserEnergy = 0;
	if( (FPara2->EnereyDemandMode.byActiveCalMode&0x03) == 0x01 )
	{
		dwUserEnergy += CurMoneyBag.UserEnergy.ForwardEnergy;
	}

	if( (FPara2->EnereyDemandMode.byActiveCalMode&0x0C) == 0x04 )
	{
		dwUserEnergy += CurMoneyBag.UserEnergy.BackwardEnergy;
	}
	
	return dwUserEnergy;
}

//-----------------------------------------------
//函数功能: 电价判断
//
//参数:     ePOWER_MODE Type
//                   
//返回值:   无
//
//备注:   
//年月模式切换
//阶梯模式切换 年阶梯 月阶梯
//需要进行年月阶梯模式切换的情况
//1：卡，交互终端通过用户卡进行设置当前套阶梯表时要判断
//2：费控模式进行切换时要判断
//3：起用备用套阶梯时区表时，需要判断采用当前第几套阶梯表，再判断年月阶梯切换
//4：起用备用套阶梯表时，需要判断年月阶梯切换
//5：阶梯时区表内部进行切换时，找到当前阶梯采用哪套表，再判断年月阶梯切换
//6：设置当前套阶梯时区表时，需要找到当前阶梯采用哪套表，再判断年月阶梯切换
//7：设置备用套阶梯时区表，备用套阶梯表时，根据大任务里面的判断是否进行年月阶梯切换
//8: 年结算日发生变化或者年结算日进行切换
//-----------------------------------------------
void JudgePrice( ePOWER_MODE Type )    //费率阶梯电价判断
{
	BYTE i,t;
	BYTE CurrentRateNum;	//当前费率
	DWORD TmpPTCT;
	DWORD dwUserEnergy;
		
	CheckLadderPriceRamData( Type ); //阶梯数据校验
		
	CheckRatePriceRamData(); //费率电价表校验
	
	t = 6;//6个阶梯电量
	for (i=0; i<5; i++)
	{
		if ( CurLadderTable.Ladder_Dl[5-i] == CurLadderTable.Ladder_Dl[4-i] ) 
		{
			t--;//相同阶梯数减1
		}
		else 
		{
			break;
		}
	}
	
	if( (t>0) && (CurLadderTable.Ladder_Dl[t-1]==0) ) 
	{
		t--;//最后阶梯电量为0,则阶梯数减1
	}
	
	dwUserEnergy = GetCurLadderUserEnergy();

	if( t == 0 )	//判断阶梯电量是否为全0
	{
		CurPrice.Current_Ladder = 0xff;//无阶梯
		CurPrice.Current_L.d = 0;//当前阶梯电价默认为0
		CurPrice.Current_Lx.d = 0;
	}
	else
	{
		for(i=0; i<t; i++)
		{
			if( dwUserEnergy <= CurLadderTable.Ladder_Dl[i] )
			{
				break;//找到
			}
		}//i的范围0~6
		
		CurPrice.Current_Ladder = i;//当前阶梯
		CurPrice.Current_L.d = CurLadderTable.Price[i];//阶梯电价
	}
		
	CurrentRateNum = api_GetCurRatio();//计算费率电价
	if( (CurrentRateNum==0)||(CurrentRateNum > MAX_RATIO) ) //当前费率电价不对，默认使用平电价
	{
		CurrentRateNum = DefCurrRatioConst;
	}
	
	CurPrice.Current_F.d = CurRateTable.Price[CurrentRateNum-1];

	CurPrice.Current.d = (CurPrice.Current_F.d+CurPrice.Current_L.d);//计算当前电价=阶梯电价+费率电价
	//计算变比	
	TmpPTCT = PrePayPara.PTCoe*PrePayPara.CTCoe;//变比hex
	if ( (TmpPTCT==0) || (TmpPTCT>=1000000) ) //三相是否可以!!!!!!
	{
		TmpPTCT = 1;
	}
	
	CurPrice.Current_Lx.d = CurPrice.Current_L.d*TmpPTCT;//变比*当前阶梯电价(hex)	
	CurPrice.Current_Fx.d = CurPrice.Current_F.d*TmpPTCT;//变比*当前费率电价(hex)

    UpdatePriceFlag = eNoUpdatePrice;
}

//-----------------------------------------------
//函数功能: 阶梯结算日检查电能，用于保存阶梯结算时电能底码，用于计算阶梯用电量
//
//参数:  	无
//	
//返回值:	TRUE--当前月、日 、时正好处于阶梯结算日 FALSE---当前月、日 、时不处于阶梯结算日
//
//备注:   	小时变化调用
//-----------------------------------------------
BOOL api_CheckLadderClosing( void )
{
	BYTE i,Mon,Day,Hour;
	TBillingPara Para;
	eLadderMode LadderMode;

	api_GetRtcDateTime( DATETIME_MM, &Mon );
	api_GetRtcDateTime( DATETIME_DD, &Day );
	api_GetRtcDateTime( DATETIME_hh, &Hour );

	if( CurLadderTable.CRC32 != lib_CheckCRC32((BYTE*)&CurLadderTable,sizeof(TLadderPrice)-sizeof(DWORD)) )//内存校验错误 或者发生阶梯切换时 从EEPROM恢复电价
	{
		//电表软件可靠性规范：当阶梯参数异常不能得到阶梯值时，形成事件记录并上报，等待重设，在数据恢复正常前，本地费控智能电能表不应扣减阶梯电价
		JudgePrice( ePowerOnMode );
	}
	
	LadderMode = JudgeLadderMode();

	//如果第一阶梯结算日月无效,且日、时有效 说明是月阶梯模式 只判日时即可
	if( LadderMode == eLadderMonMode )
	{
		if( (Day == CurLadderTable.YearBill[0][1])&&(Hour == CurLadderTable.YearBill[0][2]) )
		{
			return TRUE;
		}
	}
	else if( LadderMode == eLadderYearMode ) //年模式
	{
		for( i = 0; i < MAX_YEAR_BILL; i++ )
		{
			if( (Mon == CurLadderTable.YearBill[i][0])
				&&(Day == CurLadderTable.YearBill[i][1])
				&&(Hour == CurLadderTable.YearBill[i][2]) )
			{
				return TRUE;
			}
		}
	}
    else    //无模式
    {
        
    }
	
    return FALSE;
}

//-----------------------------------------------
//函数功能:     设置电价刷新标志位
//
//参数: 	    PriceFlag[in]
//			    eNoUpdatePrice		        电价未发生变化
//			    ePriceChangeEnergy,			电价未发生变化 但需要刷新当前电价
//			    eChangeCurRatePrice,	    改变了当前费率电价
//			    eChangeCurLadderPrice,		改变了当前阶梯电价
//
//返回值:   无
//
//备注:
//-----------------------------------------------
void api_SetUpdatePriceFlag( eChangeCurPriceFlag  PriceFlag )
{
	//改变RAM中电价CRC 让其从EEPROM恢复
	switch( PriceFlag )
	{
		case eChangeCurRatePrice:
			CurRateTable.CRC32 += 1;
			break;
		case eChangeCurLadderPrice:
			CurLadderTable.CRC32 += 1;
			break;
		case ePriceChangeEnergy:
			CurRateTable.CRC32 += 1;
			CurLadderTable.CRC32 += 1;
			break;
		default:
			break;
	}
	//该变量放于内存中，及时未更新掉电后者复位也无所谓，上电会重新判断电价
	UpdatePriceFlag = PriceFlag;    
}
//-----------------------------------------------
//函数功能: 	剩余金额电量参数校验
//
//参数: 		ePOWER_MODE Type
//
//返回值:  	无
//
//备注:
//-----------------------------------------------
static void CheckCurMoneyBag( ePOWER_MODE Type )    //剩余金额电量参数校验
{
	WORD Result;
	TFourByteUnion td;
	BYTE MoneyBagBuf[20];
	
	td.d = lib_CheckCRC32( (BYTE *)&CurMoneyBag, sizeof(TCurMoneyBag) - sizeof(DWORD) );

	//如果RAM和EEPROM中的数据都不对 则以ESAM中的为准 如果有赊欠 则无法恢复
	if( CurMoneyBag.CRC32 != td.d )
	{
		api_WriteSysUNMsg( WATCH_MONEY_RAM_ERR );
		
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.CurMoneyBag ), sizeof(TCurMoneyBag), (BYTE*)&CurMoneyBag ) != TRUE )
		{
			if( Type != ePowerWakeUpMode )//低功耗唤醒不从esam恢复
			{
				Result = ReadEsamMoneyBag( MoneyBagBuf ); //读钱包剩余金额(1)和购电次数(3)
				if(Result != TRUE) 
				{
					return;
				}
				
				memcpy( td.b, MoneyBagBuf, 4 );//钱包剩余金额转换成ram剩余金额 次数顺序是否需要倒序 后期需要验证@@@@
				CurMoneyBag.Money.Remain.ll = (long long)td.d*10000;
				memcpy( td.b, MoneyBagBuf+4, 4 );//购电次数以esam为准
				CurMoneyBag.Money.BuyTimes = td.d;		
				CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE*)&CurMoneyBag, sizeof(TCurMoneyBag)-sizeof(DWORD) );
				SaveCurMoneyBag();//保存剩余金额数据到ee
				
				api_WriteSysUNMsg( SYSUN_FIX_MONEY );
			}
		}
	}
}

//-----------------------------------------------
//函数功能: 钱包参数校验 
//
//参数: 	无
//                   
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void VerifyMoneyPara( void )    //赊欠金额,囤积金额等参数校验
{	
	//本地二类参数校验 -- 囤积金额、透支金额门限、合闸允许金额门限
	if( LimitMoney.CRC32 != lib_CheckCRC32( LimitMoney.TickLimit.b, sizeof(TLimitMoney) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney ), sizeof(TLimitMoney),LimitMoney.TickLimit.b ) != TRUE )
		{
			//采用默认值---不给用户报警
			LimitMoney.Alarm_Limit1.d = 0;
			LimitMoney.Alarm_Limit2.d = 0; 
			LimitMoney.CloseLimit.d = CloseLimitConst;
			LimitMoney.RegrateLimit.d = RegrateLimitConst;
			LimitMoney.TickLimit.d = TickLimitConst;
		}
	}
	
	//本地状态校验
	if( LocalStatus.CRC32 != lib_CheckCRC32( (BYTE*)&LocalStatus.CurLocalStatus, sizeof(TLocalStatus) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.LocalStatus ), sizeof(TLocalStatus), (BYTE*)&LocalStatus.CurLocalStatus ) != TRUE )
		{
			LocalStatus.CurLocalStatus = eNoAlarm;
			LocalStatus.LastLocalStatus = eNoAlarm;
		}
	}

	if( CardInfo.CRC32 != lib_CheckCRC32( (BYTE*)&CardInfo.LocalFlag, sizeof(TCardInfo) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo ), sizeof(TCardInfo), (BYTE*)&CardInfo.LocalFlag ) != TRUE )
		{
		//是否采用默认值
		}
	}
}

//---------------------------------------------------------------
//函数功能: 清当前用电量
//
//参数:   无
//
//返回值: 无
//
//备注:  用于年结算或者月结算时清零当前用电量
//---------------------------------------------------------------
void api_ClrCurLadderUseEnergy( void )
{
	//校验已扣费电能
	CheckCurMoneyBag( ePowerOnMode );

	CurMoneyBag.UserEnergy.ForwardEnergy = 0;
	CurMoneyBag.UserEnergy.BackwardEnergy = 0;

	api_SetSysStatus( eSYS_STATUS_EN_WRITE_MONEY );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.CurMoneyBag ), sizeof(TCurMoneyBag), (BYTE *)&CurMoneyBag );
	api_ClrSysStatus( eSYS_STATUS_EN_WRITE_MONEY );

	//刷新电价
	//api_SetUpdatePriceFlag( ePriceChangeEnergy );
	JudgePrice( ePowerOnMode );
}
//-----------------------------------------------
//函数功能: 判断电表是否处于正常状态
//
//参数:    无
//
//返回值:  TRUE/FALSE
//
//备注:
//-----------------------------------------------
static BOOL JudgeNoAlarm( void )
{
	DWORD Alarm1, Alarm2; 
	long long RemainMoney;
	
	Alarm1 = LimitMoney.Alarm_Limit1.d;
	Alarm2 = LimitMoney.Alarm_Limit2.d;
	
	//囤积金额及ESAM保证不会超过DWORD数制范围
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	//状态判断	
	if( RemainMoney < 0 )
	{
		return FALSE;
	}
	if( Alarm1 > 0 )
	{
		//Alarm1>0
		if( RemainMoney > Alarm1 )
		{
			return TRUE;
		}
	}
	else
	{
		if( Alarm2 > 0 )	//Alarm1=0,Alarm2>0
		{
			if( RemainMoney > Alarm2 )
			{
				return TRUE;
			}
		}
		else if( RemainMoney > 0 )	//Alarm1=Alarm2=0
		{
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------
//函数功能: 判断电表是否处于报警金额1状态
//
//参数: 	无
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//-----------------------------------------------
static BOOL JudgeAlarm1( void )
{
	DWORD Alarm1, Alarm2; 
	long long RemainMoney;
	
	Alarm1 = LimitMoney.Alarm_Limit1.d;
	Alarm2 = LimitMoney.Alarm_Limit2.d;
	
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	if( RemainMoney < 0 )
	{
		return FALSE;
	}
	if( Alarm1 > 0 )
	{
		if( (Alarm2 > 0)&&(Alarm1 > Alarm2) )
		{
			if( (RemainMoney > Alarm2)&&(RemainMoney <= Alarm1) )
			{
				return TRUE;
			}
		}
		else if( (RemainMoney <= Alarm1)&&(Alarm2 == 0) ) //是否需要RemainMoney 大于0？？？？？？
		{
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------
//函数功能: 判断电表是否处于报警金额2状态
//
//参数: 	无
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//-----------------------------------------------
static BOOL JudgeAlarm2( void )
{
	DWORD Alarm2; 
	long long RemainMoney;
	
	Alarm2 = LimitMoney.Alarm_Limit2.d;
	
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	if( RemainMoney < 0 )
	{
		return FALSE;
	}
	if( Alarm2 > 0 )
	{
		if( (RemainMoney <= Alarm2)&&(RemainMoney > 0) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------
//函数功能: 判断电表是否处于过零状态
//
//参数: 	无
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//-----------------------------------------------
static BOOL JudgeOvrZero( void )
{
	DWORD TickLimit;
	long long RemainMoney;
	
	TickLimit = LimitMoney.TickLimit.d;
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	
	if( TickLimit > 0 )
	{
        if( (RemainMoney <= 0)&&((RemainMoney *(-1)) < TickLimit) )
        {
            return TRUE;
        }    
	}
	return FALSE;
}

//-----------------------------------------------
//函数功能: 判断电表是否处于透支状态
//
//参数: 	无
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//-----------------------------------------------
static BOOL JudgeOvrTick( void )//后续补充！！！！！！
{
	DWORD TickLimit;
	long long RemainMoney;
	
	TickLimit = LimitMoney.TickLimit.d;
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	
	if( TickLimit > 0 )
	{
		if( (RemainMoney < 0)&&((RemainMoney *(-1))>= TickLimit) )
		{
			return TRUE;
		}
	}
	else
	{
		if( RemainMoney <= 0)
		{
			return TRUE;
		} 
	}
	return FALSE;
}

//-----------------------------------------------
//函数功能: 本地状态判断
//
//参数:    Type[in]
//           eReturnMoneyMode  -- 扣费或者退费操作
//           eChargeMoneyMode  -- 充值操作
//			 eSetParaMode	   -- 更改参数操作
//			 eInitMoneyMode     -- 预置操作
//         ParaType[in]        -- 如果是更改参数 传入参数类型 非更改参数操作传入FF
//			 eTickLimit		   -- 透支金额门限
// 			 eAlarm_Limit1	   -- 报警金额1
// 			 eAlarm_Limit2	   -- 报警金额2
// 			 
// 			         
//返回值:  本地状态
//
//备注:   
//-----------------------------------------------
static eLOCAL_STATUS UpdateLocalStatus( eUpdataMoneyMode Type, BYTE ParaType )
{
	QWORD CloseLimit;
			
	if( Type > eInitMoneyMode)
	{
		ASSERT(FALSE,0); 
		return LocalStatus.CurLocalStatus; 
	}
	if( LocalStatus.CurLocalStatus == eOvrTick )
	{
		CloseLimit = (QWORD)LimitMoney.CloseLimit.d; 

		//充值操作 更改报警金额1和报警金额2 过透支后必须大于合闸允许金额才允许合闸
		if( (Type == eChargeMoneyMode)							//充值操作
		 ||((Type == eSetParaMode)&&((ParaType == eAlarm_Limit1) || (ParaType == eAlarm_Limit2))) )	//更改报警金额1和报警金额2
		{
			if( (CurMoneyBag.Money.Remain.ll / 10000) <= (long long)CloseLimit )//无符号与有符号运算，会统一为无符号
			{
				return eOvrTick;
			}			
		}
		//扣费还是透支
		else if( Type == eReturnMoneyMode )
		{
			return eOvrTick;
		}
		//预置和更改透支金额门限正常刷新
		//继续往下走
		
		//如果更改参数不是透支金额 走到这里说明错误 不刷新状态
		if( (Type == eSetParaMode)&&(ParaType != eTickLimit) )
		{
			ASSERT(FALSE,0); 
			return LocalStatus.CurLocalStatus;
		}
	}
	
	if( JudgeOvrTick() == TRUE )
	{
		return eOvrTick;
	}

	if( JudgeOvrZero() == TRUE )
	{
		return eOvrZero;
	}
	
	if( JudgeAlarm2() == TRUE )
	{
		return eAlarm2;
	}
	
	if( JudgeAlarm1() == TRUE )
	{
		return eAlarm1;
	}
	
	if( JudgeNoAlarm()==TRUE )
	{
		return eNoAlarm;
	}
	
	ASSERT(FALSE,0); 
	return LocalStatus.CurLocalStatus; //如果所有状态都没找到 返回之前的状态
}

//-----------------------------------------------
//函数功能: 本地状态判断
//
//参数:    Type[in]
//           eReturnMoneyMode  -- 扣费或者退费操作
//			 eRChargeMoneyMode -- 充值操作
// 			 eSetParaMode      -- 更改参数操作
//			 eInitMoneyMode     -- 预置操作
//        ParaType[in]         -- 如果是更改参数 传入参数类型 非更改参数操作传入FF
//			 eTickLimit		   -- 透支金额门限
// 			 eAlarm_Limit1	   -- 报警金额1
// 			 eAlarm_Limit2	   -- 报警金额2         
//返回值:  本地状态
//
//备注:   
//-----------------------------------------------
void JudgeLocalStatus( eUpdataMoneyMode Type, BYTE ParaType )
{
	eLOCAL_STATUS LocalStatusTmp;
    
	if( Type > eInitMoneyMode)
	{
		ASSERT(FALSE,1); 
		return;
	}
	
	LocalStatusTmp = UpdateLocalStatus( Type, ParaType );
	
	//透支状态判断
	api_ClrSysStatus( eSYS_STATUS_PRE_USE_MONEY );
	if( LocalStatusTmp >= eOvrZero )
	{
		if( CurMoneyBag.Money.Remain.ll < 0 )
		{
			api_SetSysStatus( eSYS_STATUS_PRE_USE_MONEY );
			if( api_GetRunHardFlag(eRUN_HARD_TICK_FLAG) == FALSE )
			{
				api_SetRunHardFlag(eRUN_HARD_TICK_FLAG);
				api_SetFollowReportStatus(eSTATUS_TickFlag);
			}	
		}
		else
		{
			api_ClrRunHardFlag(eRUN_HARD_TICK_FLAG);
		}
	}
	else
	{
		api_ClrRunHardFlag(eRUN_HARD_TICK_FLAG);
	}
	
	//状态发生变化 或者充值预置操作
	if( (LocalStatusTmp != LocalStatus.CurLocalStatus)||(Type != eReturnMoneyMode) )
	{
		LocalStatus.LastLocalStatus = LocalStatus.CurLocalStatus;
		LocalStatus.CurLocalStatus = LocalStatusTmp;
		
		//状态发生变化后就保存 应该不会有问题
		if( api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.LocalStatus ), sizeof(TLocalStatus), (BYTE *)&LocalStatus.CurLocalStatus ) == TRUE )
		{
			api_Set_LocalRelayCmd( Type, LocalStatus.CurLocalStatus );
		}
		else//如果保存EEPROM错误 恢复之前的状态
		{
			LocalStatus.CurLocalStatus = LocalStatus.LastLocalStatus;
		}
	}
}

//-----------------------------------------------
//函数功能:  	上电金额处理
//
//参数: 	    ePOWER_MODE Type
//                   
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void PowerUpMoney( ePOWER_MODE Type )
{
    VerifyMoneyPara();  //重新校验金额参数
    CheckCurMoneyBag( Type );//重新校验剩余金额与已扣费电能
    JudgePrice( Type );
    
    if( Type != ePowerWakeUpMode )//唤醒状态下不刷新本地状态
    {
		JudgeLocalStatus( eReturnMoneyMode, 0xFF ); //上电刷新一次本地状态
    }
}
//-----------------------------------------------
//函数功能: 掉电金额处理
//
//参数: 	    无
//                   
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void PowerDownMoney( void )
{
	SaveCurMoneyBag();
}

//-----------------------------------------------
//函数功能:     金额分钟任务
//
//参数: 	    无
//                   
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void MoneyMinuteTask( void )
{
    //一分钟校验一次本地参数
    VerifyMoneyPara();
    CheckCurMoneyBag( ePowerOnMode );
}
//-----------------------------------------------
//函数功能:     金额秒任务
//
//参数: 	    无
//                   
//返回值:  	无
//
//备注:   	
//-----------------------------------------------
void MoneySecTask( void )
{
    //刷新当前电价
    if( UpdatePriceFlag != eNoUpdatePrice )
    {
        JudgePrice( ePowerOnMode );
    }

	//程序中有个限制 上电超过30秒掉电才保存电能 因此上电同步钱包也必须是30秒 不然会出现扣费和电能无法对应的情况
	//不能判断 PrePayPowerOnTimer == 0 因为30秒后PrePayPowerOnTimer就一致是0了 会导致钱包一秒同步一次
	if( PrePayPowerOnTimer == 1 )//上电30s后同步钱包
    {
		MeteringUpdateMoneybag();//同步钱包
    }
}

//-----------------------------------------------
//函数功能: 电费扣减
//
//参数: 	EnergyDirection[in]		电能反向
//			inTotalReduceEnergyNum[in]	新累加总电能 单位0.01kwh
// 			inRatioReduceEnergyNum[in]	新累加当前费率电能 单位0.01kwh
//返回值:   无
//
//备注:
//-----------------------------------------------
void api_ReduceRemainMoney( BYTE EnergyDirection, BYTE inTotalReduceEnergyNum, BYTE inRatioReduceEnergyNum )
{
	BYTE  	i;
	BYTE	tReduceTotalEnergyNum,tReduceRatioEnergyNum;
	DWORD	*pdw;

	if( (inTotalReduceEnergyNum == 0)&&(inRatioReduceEnergyNum == 0) )
	{
		return;
	}
	
	if( EnergyDirection == N_ACTIVE )
	{
		//若为反向电能扣减，组合模式字里面反向电能不为加，则不处理，直接返回
		if( (FPara2->EnereyDemandMode.byActiveCalMode&0x0C) != 0x04 )
		{
			return;
		}
	}
	else
	{
		//若为正向电能扣减，组合模式字里面正向电能不为加，则不处理，直接返回
		if( (FPara2->EnereyDemandMode.byActiveCalMode&0x03) != 0x01 )
		{
			return;
		}
	}

	pdw = 0;
	
	//校验已扣费电能
	CheckCurMoneyBag( ePowerOnMode );
	
	if( EnergyDirection == N_ACTIVE )
	{
		pdw = &CurMoneyBag.UserEnergy.BackwardEnergy; 
	}
	else
	{
		pdw = &CurMoneyBag.UserEnergy.ForwardEnergy; 
	}
	
	tReduceTotalEnergyNum = inTotalReduceEnergyNum;
	tReduceRatioEnergyNum = inRatioReduceEnergyNum;

	//阶梯扣费，无阶梯模式下不计费！
	if( JudgeLadderMode() != eNoLadderMode )
	{
		//电表软件可靠性规范：当阶梯参数异常不能得到阶梯值时，形成事件记录并上报，等待重设，在数据恢复正常前，本地费控智能电能表不应扣减阶梯电价。
		for( i = 0; i < tReduceTotalEnergyNum; i++ )
		{
			//用电量先加一 再进行电价判断 否则会有0.01度判错阶梯
			//判断电价 阶梯扣费每扣费一次应该判断一次电价 因为涉及阶梯值判断的问题
			(*pdw)++;

			JudgePrice( ePowerOnMode );
			CurMoneyBag.Money.Remain.ll -= CurPrice.Current_Lx.d; //剩余电量减去单位电价
		}
	}
	
	//判断电价 费率扣费电价判断一次即可
	JudgePrice( ePowerOnMode );
	//费率扣减处理 最大值判断处理 避免扣减很多次 20次
	for( i = 0; i < tReduceRatioEnergyNum; i++ )
	{
		CurMoneyBag.Money.Remain.ll -= CurPrice.Current_Fx.d; //剩余电量减去单位电价
	}

	//只有扣减才应该判断本地状态
	JudgeLocalStatus( eReturnMoneyMode, 0xFF );
	//重新计算CRC
	CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE *)&CurMoneyBag, sizeof(TCurMoneyBag) - sizeof(DWORD) );
}

//以下都是跟年阶梯、年月阶梯切换有关的函数
static WORD JudgeSubYearBillPara(BYTE *YearBill_Para)//判断年结算日是否有效
{
	if((YearBill_Para[2] > 23))//时
	{
		return FALSE;
	}
	if(( YearBill_Para[1] > 28 ) || (YearBill_Para[1] == 0 ) ) //日
	{
		return FALSE;
	}
	if( (YearBill_Para[0] == 0) || (YearBill_Para[0] > 12) )//月
	{
		return FALSE;
	}

	return TRUE;	
}

static BYTE ExchangeYearBillPara(BYTE byOffset, BYTE *pBuf )
{
    BYTE Buf[3];
	
	if(   (pBuf[0] > pBuf[byOffset*3])//如果月大于
		||((pBuf[0] == pBuf[byOffset*3])&&(pBuf[1] > pBuf[byOffset*3+1]))//如果月等于，且日大于
		||((pBuf[0] == pBuf[byOffset*3])&&(pBuf[1] == pBuf[byOffset*3+1])&&(pBuf[2] > pBuf[byOffset*3+2]))//如果月、日等于，且时大于
	)
	{
		memcpy( (void *)Buf, pBuf, 3 );
		memcpy( (void *)&(pBuf[0]), (void *)&(pBuf[byOffset*3]), 3 );
		memcpy( (void *)&(pBuf[byOffset*3]), (void *)Buf, 3 );
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------
//函数功能: 重新排序当前内存中年结算日
//
//参数: 	Len固定为年结算日长度MAX_YEAR_BILL*3 pBuf 年结算日传入buf
//                   
//返回值:   TRUE  发生了改变  FALSE 没发生改变
//
//备注:     
//			
//-----------------------------------------------
static BYTE SortYearBillPara( BYTE Len, BYTE *pBuf)
{
	BYTE i, j;
	BYTE Result;
	
	Result = FALSE;
	
	if( Len != MAX_YEAR_BILL*3 )
	{
		return Result;
	}
	
	//判断每个年结算日的合法性，若不合法，则置为全99
	for( i=0; i<MAX_YEAR_BILL; i++ )
	{
		if( JudgeSubYearBillPara( (BYTE *)&(pBuf[i * 3]) ) == FALSE )
		{
			memset( (void *)&(pBuf[i*3]), 99, 3 );
			Result = TRUE;
		}	
	}
	
	//将所有年结算日按照从小到大的顺序排序		
	for( i=0; i<MAX_YEAR_BILL; i++ )
	{
		for( j=(i+1); j<MAX_YEAR_BILL; j++ )
		{
			if( ExchangeYearBillPara( (j - i), (BYTE *)&(pBuf[i * 3]) ) )
			{
				Result = TRUE;
			}
		}
	}
	
	return Result;
}

//-----------------------------------------------
//函数功能: 初始化阶梯表、阶梯模式
//
//参数: 	无
//                   
//返回值:   无
//
//备注:     包括上电初始化和预置卡设置当前套阶梯两种情况
//			本函数还要放在上电初始化PrepayPara的函数中，如果不放其中，则需要加入写ee和ram的函数
//-----------------------------------------------
void InitJudgeBillMode(void)
{
    eLadderMode LadderMode;
    
	//重新获得当前阶梯表
	api_SetUpdatePriceFlag( eChangeCurLadderPrice );
	CheckLadderPriceRamData( ePowerOnMode );
	
    LadderMode = JudgeLadderMode();
	if( LadderMode == eLadderYearMode )
	{
		//年模式
		api_SetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
		api_ClrRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
	}
	else if( LadderMode == eLadderMonMode )
	{
		//月模式
		api_SetRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
		api_ClrRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
	}
	else
	{
		//无模式
		api_ClrRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
		api_ClrRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
	}
}
//-----------------------------------------------
//函数功能: 上电或阶梯表发生切换重新初始化阶梯表、阶梯模式
//
//参数: 	无
//                   
//返回值:   
//			月结算到年结算 //转存月用电量 //全清年结算日用电量
//			年结算日改变 	 //转存年用电量	
//			年结算到月结算 //转存年用电量	//转存月用电量			
//
//备注:		阶梯模式切换 年阶梯 月阶梯
//			需要进行年月阶梯模式切换的情况
//			1：卡，交互终端通过用户卡进行设置当前套阶梯表时要判断
//			2：费控模式进行切换时要判断
//			3：起用备用套阶梯时区表时，需要判断采用当前第几套阶梯表，再判断年月阶梯切换
//			4：起用备用套阶梯表时，需要判断年月阶梯切换
//			5：阶梯时区表内部进行切换时，找到当前阶梯采用哪套表，再判断年月阶梯切换
//			6：设置当前套阶梯时区表时，需要找到当前阶梯采用哪套表，再判断年月阶梯切换
//			7：设置备用套阶梯时区表，备用套阶梯表时，根据大任务里面的判断是否进行年月阶梯切换
//			8: 年结算日发生变化或者年结算日进行切换
//-----------------------------------------------
void ProcLadderModeSwitch(void)//结算方式变换判断及处理
{
	TBackupYearBill  BackupYearBillBuf,CurYearBillBuf;
	eLadderMode LadderMode; 
	
	LadderMode = JudgeLadderMode();
	
	if( LadderMode == eLadderYearMode )
	{
		//月结算到年结算
		if( api_GetRunHardFlag(eRUN_HARD_MON_LADDER_MODE) == TRUE )
		{			
			api_SetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
			api_ClrRunHardFlag( eRUN_HARD_MON_LADDER_MODE );

			//将当前阶梯结算日保存在备份中，在下次更改阶梯结算日时读出进行比较是否有变化
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );
		}
		else if( 	(api_GetRunHardFlag(eRUN_HARD_YEAR_LADDER_MODE) == FALSE) 
				 && (api_GetRunHardFlag(eRUN_HARD_MON_LADDER_MODE) == FALSE) )//无模式到年结算
		{
			api_SetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
			
			//清当前阶梯结算日备份
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );
		}
		else //年结算日改变
		{
			//读出保存的上次年结算日，与当前的进行比较，若不一致需要转结算
		 	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&BackupYearBillBuf.YearBill ); 		
			//年结算日的判断必须排序 存在数据一致 但是顺序不同的问题
			SortYearBillPara( sizeof(TBackupYearBill) - sizeof(DWORD), (BYTE *)&BackupYearBillBuf.YearBill );

			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			SortYearBillPara( sizeof(TBackupYearBill) - sizeof(DWORD), (BYTE *)&CurYearBillBuf.YearBill );

			//若当前套阶梯结算日与备用套阶梯结算日相同，则直接返回
			if( memcmp( (BYTE *)&BackupYearBillBuf, (BYTE *)&CurYearBillBuf.YearBill, sizeof(TBackupYearBill) - sizeof(DWORD) ) == 0 )
			{
				return;
			}
			
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );					
		}
	}
	else if( LadderMode == eLadderMonMode )
	{
		//年结算到月结算
		if( api_GetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE ) == TRUE )
		{	
			//将当前阶梯结算日保存在备份中，在下次更改阶梯结算日时读出进行比较是否有变化
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );

			//置月结算模式
			api_SetRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
			api_ClrRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
		}
		else if( 	(api_GetRunHardFlag(eRUN_HARD_YEAR_LADDER_MODE) == FALSE) 
				 && (api_GetRunHardFlag(eRUN_HARD_MON_LADDER_MODE) == FALSE) )//无模式到年结算
		{
			api_SetRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
			
			//清当前阶梯结算日备份
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );
		}
		else //月结算日改变
		{
			//读出保存的上次年结算日，与当前的进行比较，若不一致需要转结算
		 	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&BackupYearBillBuf.YearBill ); 
			
			//月结算日只判断第一个结算日即可
			if( memcmp( (BYTE *)&BackupYearBillBuf, (BYTE *)&CurLadderTable.YearBill, sizeof(CurLadderTable.YearBill[0]) ) == 0 )
			{
				return;
			}
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );
		}
	}
	else
	{
		if( (api_GetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE ) == FALSE)
			&&(api_GetRunHardFlag( eRUN_HARD_MON_LADDER_MODE ) == FALSE) )
		{
			return;
		}

		//置无结算模式
		api_ClrRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
		api_ClrRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
		
		//清当前阶梯结算日备份
		memset((BYTE *)&CurYearBillBuf.YearBill,0xFF,sizeof(TBackupYearBill)-sizeof(DWORD));
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );	
	}

	api_SetFreezeFlag( eFREEZE_LADDER_CLOSING, 0 );
}

//-----------------------------------------------
//函数功能: 转存金额（包括与ESAM钱包同步）
//
//参数: 	无
//                   
//返回值:  	无
//
//备注: 
//-----------------------------------------------
void api_SwapMoney( void )
{
    CheckCurMoneyBag( ePowerOnMode );//校验剩余金额与已扣费电能
    SaveCurMoneyBag();//保存剩余金额
    MeteringUpdateMoneybag();//同步钱包
}

//-----------------------------------------------
//函数功能: 获取当前的阶梯结算日表
//
//参数: 		LadderBillingPara[out]:阶梯结算日表
//
//返回值:		阶梯模式
//			eNoLadderMode	--无阶梯模式
//			eLadderMonMode	--月阶梯模式
//			eLadderYearMode	--年阶梯模式
//
//备注:
//-----------------------------------------------
BYTE api_GetCurLadderYearBillInfo(TLadderBillingPara* LadderBillingPara)
{
	BYTE i,j;
	eLadderMode LadderMode; 
		
	CheckLadderPriceRamData( ePowerOnMode );
	LadderMode = JudgeLadderMode();
	memset( LadderBillingPara, 0xff, sizeof(TLadderBillingPara) );
	
	//如果第一阶梯结算日月无效,且日、时有效 说明是月阶梯模式 只判日时即可
	if( LadderMode == eLadderMonMode )
	{
		//阶梯结算日合法
		if( ( CurLadderTable.YearBill[0][1] >= 1 ) && ( CurLadderTable.YearBill[0][1] <= 28)
		 &&( CurLadderTable.YearBill[0][2] >= 0 ) && ( CurLadderTable.YearBill[0][2] <= 23))
		{
			LadderBillingPara->LadderSavePara[0].Day = CurLadderTable.YearBill[0][1] ;
			LadderBillingPara->LadderSavePara[0].Hour = CurLadderTable.YearBill[0][2] ;
		}
	}
	else if( LadderMode == eLadderYearMode ) //年模式
	{
		j = 0;
		for( i = 0; i < MAX_YEAR_BILL; i++ )
		{
			//阶梯结算日合法
			if( ( CurLadderTable.YearBill[i][0] >= 1 ) && ( CurLadderTable.YearBill[i][0] <= 12)
			 &&( CurLadderTable.YearBill[i][1] >= 1 ) && ( CurLadderTable.YearBill[i][1] <= 28)
			 &&( CurLadderTable.YearBill[i][2] >= 0 ) && ( CurLadderTable.YearBill[i][2] <= 23))
			{
				memcpy( (BYTE *)&LadderBillingPara->LadderSavePara[j], (BYTE *)&CurLadderTable.YearBill[i][0], sizeof(TLadderSavePara) );
				j++;
			}
		}
	}
    else    //无模式
    {
        
    }
	//返回阶梯结算模式
    return LadderMode;  
}
//-----------------------------------------------
//函数功能: 从E2读取当前阶梯表到掉电前阶梯表
//
//参数: 	
//
//返回值:	

//
//备注:只给补冻結用，在初始化参数前读取；防止掉电过阶梯结算将E2中当前阶梯表覆盖
//-----------------------------------------------
void api_ReadPowerLadderTable(void)
{
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentLadder ), sizeof(TLadderPrice), (BYTE *)&PowerdownLadderTable );
}
//-----------------------------------------------
//函数功能: 获取掉电前的阶梯结算日表
//
//参数: 		LadderBillingPara[out]:阶梯结算日表
//
//返回值:		阶梯模式
//			eNoLadderMode	--无阶梯模式
//			eLadderMonMode	--月阶梯模式
//			eLadderYearMode	--年阶梯模式
//
//备注:
//-----------------------------------------------
BYTE api_GetPowerDownYearBill(TLadderBillingPara* LadderBillingPara)
{
	BYTE i,j;
	eLadderMode LadderMode = eNoLadderMode;
	//内存校验错误 ,从EEPROM读取当前阶梯表，
	if( PowerdownLadderTable.CRC32 != lib_CheckCRC32((BYTE*)&PowerdownLadderTable,sizeof(TLadderPrice)-sizeof(DWORD)) )
	{
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentLadder ), sizeof(TLadderPrice), (BYTE *)&PowerdownLadderTable );
	}
	memset( LadderBillingPara, 0xff, sizeof(TLadderBillingPara) );
	j = 0;
	//结算日 0--月 1--日  2--时
	for( i = 0; i < MAX_YEAR_BILL; i++ )
	{
		if( (PowerdownLadderTable.YearBill[i][1] <= 28)&&(PowerdownLadderTable.YearBill[i][1] > 0) //日
			&&(PowerdownLadderTable.YearBill[i][2] <= 23) ) //时
		{
			if( (PowerdownLadderTable.YearBill[i][0] <= 12)&&(PowerdownLadderTable.YearBill[i][0] > 0) ) //月
			{
				LadderMode = eLadderYearMode;//年模式
				memcpy( (BYTE *)&LadderBillingPara->LadderSavePara[j], (BYTE *)&PowerdownLadderTable.YearBill[i][0], sizeof(TLadderSavePara) );
				j++;
			}
			else if( i == 0 ) //如果第一阶梯结算日 月无效 说明是月阶梯模式 只判日时即可
			{
				LadderMode = eLadderMonMode;//月模式
				LadderBillingPara->LadderSavePara[0].Day = PowerdownLadderTable.YearBill[0][1] ;
				LadderBillingPara->LadderSavePara[0].Hour = PowerdownLadderTable.YearBill[0][2] ;
				break;
			}
		}
	}

	//返回阶梯结算模式
    return LadderMode;  
}

//-----------------------------------------------
//函数功能: 得到当前本地状态
//
//参数: 	无
//
//返回值:  	返回本地状态
//
//备注:   	用于继电器本地状态查询
//-----------------------------------------------
eLOCAL_STATUS api_GetLocalStatus( void )
{
	return LocalStatus.CurLocalStatus;
}

//-----------------------------------------------
//函数功能: 读累计购电金额
//
//参数:  		Buf[out] 输出累计购电金额
//
//返回值:		无
//
//备注:
//-----------------------------------------------
void api_ReadTotalBuyMoney( BYTE *Buf )
{
	api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD), Buf );
	//lib_InverseData(Buf, 4 );因数据读出后在协议层还要调用 Exchange ，此处不用高字节在Buf[0]
}

//---------------------------------------------------------------
//函数功能: 获取当前处于第几阶梯、当前是否有备用套阶梯表、当前是否有备用套费率
//
//参数: 	Type
//            eInfoCurLadder - 获取当前所处阶梯
//            eInfoBackupRateFalg - 是否有备用套费率
//            eInfoBackupLadderFlag - 是否有备用套阶梯
//
//返回值:  	0 	返回所处于的阶梯  0xff不显示阶梯符号
//			1  返回0xff不显示 true代表有 FALSE 代表无
//          2  返回0xff不显示 true代表有 FALSE 代表无
//
//
//备注:   0xff表示远程模式不显示所有阶梯、费率相关的符号
//---------------------------------------------------------------
BYTE api_GetLocalInfo( eLocalInfo Type )
{
	BYTE ReturnData;

	switch( Type )
	{
		case eInfoCurLadder:    	//获取当前所处阶梯
			if( CurPrice.Current_Ladder != 0xff )
			{
				ReturnData = ( CurPrice.Current_Ladder + 1 );
			}
			else
			{
				ReturnData = 0xff;
			}
			break;
		case eInfoBackupRateFalg:   // 是否有备用套费率
			if( api_GetRunHardFlag( eRUN_HARD_SWITCH_FLAG3 ) == TRUE )
			{
				ReturnData = TRUE;
			}
			else
			{
				ReturnData = FALSE;
			}
			break;
		case eInfoBackupLadderFlag: //是否有备用套阶梯
			if( CurPrice.Current_Ladder != 0xff ) //只有当前阶梯电价有效时才显示
			{
				if( api_GetRunHardFlag( eRUN_HARD_SWITCH_FLAG4 ) == TRUE )
				{
					ReturnData =  TRUE;
				}
				else
				{
					ReturnData = FALSE;
				}
			}
			else
			{
				ReturnData = 0xff;
			}
			break;
	}
		
	return ReturnData;
}

#endif//#if( PREPAY_MODE == PREPAY_LOCAL )
