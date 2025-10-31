//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人    尹西科
//创建日期  2017年9月6日09:23:07
//描述      645规约获取数据
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "dlt645_2007.h"

#define MAX_PARA_BUF_LENTH         (MAX_TIME_SEG*3*MAX_TIME_SEG_TABLE+30)//时段表源数据长度为最大数据长度

#if( SEL_DLT645_2007 == YES )

//声明默认生产日期
extern const BYTE ProduceDate[7];
//---------------------------------------------------------------
//函数功能: 读时间类
//
//参数:
//		DI0[IN] -- 数据标识DI0
// 		Buf -- 输出缓存
//
//返回值: 
//      Lenth -- 读取数据长度
//
//备注:
//---------------------------------------------------------------
static WORD ReadParaData0001XX( BYTE DI0, BYTE *Buf )
{
	BYTE i;
	WORD Lenth,Result;
	WORD wTemp,OI;
	WORD Sel_HEX2BCD; //是否需要HEX转BCD
	eTYPE_COVER_DATA CoverMode;	//数据转换方式
	TTwoByteUnion Year;
	TFourByteUnion dwTime;
	BYTE pBuf[sizeof(TSwitchTimePara)+10];
	
	Sel_HEX2BCD = YES;
	CoverMode= eBYTE_MODE;
	Lenth = 0;
	
	switch( DI0 )
	{
		case 0x01:	//日期及星期
		case 0x02:	//时间
		case 0x0C:	//日期、星期(其中0代表星期天)及时间 
			if( DI0 == 0x01 )
			{
				wTemp = DATETIME_YYMMDDWW;
			}
			else if( DI0 == 0x02 )
			{
				wTemp = DATETIME_hhmmss;
			}
			else
			{
				wTemp = DATETIME_YYMMDDWWhhmmss;
			}
			
			Lenth = api_GetRtcDateTime(wTemp, Buf );
			if( Lenth != 0 )
			{
				CoverMode = eBYTE_MODE;
			}
			break;
		#if( MAX_PHASE == 3 )	
		case 0x03:	//最大需量周期
			Buf[0] = FPara2->EnereyDemandMode.DemandPeriod;
			Lenth = 1;
			CoverMode = eBYTE_MODE;
			break;
		case 0x04:	//滑差时间
			Buf[0] = FPara2->EnereyDemandMode.DemandSlip;
			Lenth = 1;
			CoverMode = eBYTE_MODE;
			break;
		#endif//#if( MAX_PHASE == 3 )	
		case 0x05:	//校表脉冲宽度
			Buf[0] = 0x00;
			Buf[1] = 0x80;
			Sel_HEX2BCD = NO;
			Lenth = 2;
			break; 
		case 0x06:	//两套时区表切换时间
		case 0x07:	//两套日时段切换时间
		#if( PREPAY_MODE == PREPAY_LOCAL )
		case 0x08:	//两套分时费率切换时间
		case 0x09:	//两套阶梯切换时间
		#endif   
            Sel_HEX2BCD = NO;
            
            Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara),sizeof(TSwitchTimePara), pBuf);
            if( Result == FALSE )
            {
                return 0;
            }
            
            memcpy((void *)&dwTime.b,pBuf+(DI0-6)*4,4);
            if(dwTime.d == 0)
            {
                memset(Buf,0x00,5);
            }
			else if(dwTime.d == 0x99999999)
			{
				memset(Buf,0x99,5);
			}
            else
            {
                Result = api_ToAbsTime( dwTime.d, (TRealTimer *)pBuf );
                if(Result == FALSE)
                {
                    return FALSE;
                }
                api_TimeFormat698To645( (TRealTimer *)pBuf, Buf );
            }
            
            Lenth = 5;
			break;
			
        default: 
            Lenth = 0;
	    break;
	}
	
	//统一处理HEX转BCD
	if( Sel_HEX2BCD == YES )
	{
		//不判是否正确 即使错误也要返回错误数据
		ProBinToBCD( CoverMode, Lenth, Buf );
	}
	
    return Lenth;
}

//-----------------------------------------------
//--------------------------------------------------
//功能描述:  读取结算日
//参数	:
//			pBuf[out]
//返回值:	TRUE	FALSE
//注:返回时间数据为HEX
//--------------------------------------------------
WORD ReadSettlementDate645(BYTE DI0,BYTE *pBuf)
{		
	WORD Result;
    TBillingPara BillingParaBuf;

	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE*)&BillingParaBuf);
	if( Result == FALSE )
	{
		return FALSE;
	}
	else
	{
		if((DI0<=(MAX_MON_CLOSING_NUM))&&(DI0 > 0))
		{
			*(pBuf++) = BillingParaBuf.MonSavePara[DI0-1].Day;
			*(pBuf++) = BillingParaBuf.MonSavePara[DI0-1].Hour;
		}
		else
		{
			return DLT645_ERR_ID_02;
		}
	
		return TRUE;
	}
}
#if(PREPAY_MODE == PREPAY_LOCAL)
//---------------------------------------------------------------
//函数功能: 读两套费率电价、两套阶梯电价
//
//参数:
//		DI2[IN] -- 数据标识DI2
//		DI1[IN] -- 数据标识DI1
//		DI0[IN] -- 数据标识DI0
// 		Buf -- 输出缓存
//
//返回值: 
//      Lenth -- 读取数据长度
//
//备注:
//---------------------------------------------------------------
static WORD ReadPriceParaData645( BYTE DI2,BYTE DI1,BYTE DI0, BYTE *Buf )
{
	BYTE i;
	WORD Lenth;
	TRatePrice				RatePrice;		//费率电价表
    TLadderPrice			LadderPrice;	//阶梯电价表
    TRatePrice   			RatePriceBCD;	
	TLadderPrice			LadderPriceBCD;
    Lenth = 0;
    
	if( DI2 == 0x05 )//费率电价
	{
		if( DI1 == 0x01 )
		{
			api_ReadPrePayPara(eCurRateTable, (BYTE*)RatePrice.Price);
		}
		else
		{
			api_ReadPrePayPara(eBackupRateTable, (BYTE*)RatePrice.Price);
		}

		for(i=0;i<(FPara1->TimeZoneSegPara.Ratio);i++)
		{
			RatePriceBCD.Price[i] = lib_DWBinToBCD(RatePrice.Price[i]);
		}
							
		if( DI0 == 0xff )
		{
			memcpy( Buf, (BYTE*)RatePriceBCD.Price, 4*(FPara1->TimeZoneSegPara.Ratio) );
			Lenth += (4*(FPara1->TimeZoneSegPara.Ratio));
		}
		else
		{	
			if( DI0 > (FPara1->TimeZoneSegPara.Ratio) )
			{
				return Lenth;
			}
			memcpy( Buf, (BYTE*)&RatePriceBCD.Price[DI0-1], 4 );
			Lenth += 4;
		}
	}
	else if( DI2 == 0x06 )//阶梯电价
	{
		if( DI1 == 0x04)
		{
			api_ReadPrePayPara(eCurLadderTable, (BYTE*)LadderPrice.Ladder_Dl );
		}
		else
		{
			api_ReadPrePayPara(eBackupLadderTable, (BYTE*)LadderPrice.Ladder_Dl );
		}

		//将电价、阶梯值和年结算日转化为BCD格式
		for(i=0;i<(MAX_LADDER);i++)
		{
			LadderPriceBCD.Ladder_Dl[i] = lib_DWBinToBCD(LadderPrice.Ladder_Dl[i]);
		}
		for(i=0;i<(MAX_LADDER+1);i++)
		{
			LadderPriceBCD.Price[i] = lib_DWBinToBCD(LadderPrice.Price[i]);
		}

		lib_MultipleHEXToBBCD((BYTE*)&LadderPrice.YearBill[0][0],(BYTE*)&LadderPriceBCD.YearBill[0][0],(3*MAX_YEAR_BILL));
		//年结算日倒序
		for( i = 0; i < MAX_YEAR_BILL; i++ )
		{
			lib_InverseData( (BYTE*)&LadderPriceBCD.YearBill[i][0], 3 );
		}
		
		memcpy( Buf, (BYTE*)LadderPriceBCD.Ladder_Dl, (sizeof(TLadderPrice)-sizeof(DWORD)) );    
		Lenth += (sizeof(TLadderPrice)-sizeof(DWORD));
	}

	return Lenth;
}
#endif


//---------------------------------------------------------------
//函数功能: 软件备案号由ASCII转换为BCD
//
//参数:
//		InBuf[in] - 输入备案号 ASCII格式
//		OutBuf[out] - 输出备案号 BCD格式
//返回值:
//
//备注:
//---------------------------------------------------------------
void SwitchSoftRecordToBCD( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i;
	
	memset( OutBuf, 0xAA, 8 ); //默认值设置为0xAA
	for( i = 0; i < 8; i++ )
	{
		//低半字节
		if( InBuf[2 * i] <= '9' )
		{
			OutBuf[i] = ((InBuf[2 * i] - '0') << 4);
		}
		else if( InBuf[2 * i] == 'A' )
		{
			OutBuf[i] = 0xA0; 
		}
		
		//高半字节
		if( InBuf[2 * i + 1] <= '9' )
		{
			OutBuf[i] |= (InBuf[2 * i + 1] - '0');
		}
		else if( InBuf[2 * i + 1] == 'A' )
		{
			OutBuf[i] |= 0x0A;
		}
	}
}

//---------------------------------------------------------------
//函数功能: 生产日期由HEX转换为ASCII
//
//参数:
//		InBuf[in] - 输入日期 HEX格式
//		OutBuf[out] - 输出日期 ASCII格式
//返回值:
//
//备注:
//---------------------------------------------------------------
void SwitchProduceDataToAscii( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i;
	TFourByteUnion FourTemp;
	BYTE Ascii[] = "0123456789";
	memcpy( (void *)FourTemp.b, InBuf, 4 );
	
	//转换年
	FourTemp.w[0] = lib_WBinToBCD( FourTemp.w[0] ); //年
	for( i = 0; i < 2; i++ )
	{
		OutBuf[2 * i] = Ascii[FourTemp.b[1 - i] >> 4];
		OutBuf[2 * i + 1] = Ascii[FourTemp.b[1 - i]&0x0F];
	}
	OutBuf[4] = '/';

	//转换月
	FourTemp.b[2] = lib_BBinToBCD( FourTemp.b[2] ); //月
	OutBuf[5] = Ascii[FourTemp.b[2] >> 4];
	OutBuf[6] = Ascii[FourTemp.b[2]&0x0F];
	OutBuf[7] = '/'; 
	
	//转换日
	FourTemp.b[3] = lib_BBinToBCD( FourTemp.b[3] ); //日
	OutBuf[8] = Ascii[FourTemp.b[3] >> 4];
	OutBuf[9] = Ascii[FourTemp.b[3]&0x0F];
}
//---------------------------------------------------------------
//函数功能: 读出参变量数据
//
//参数:
//                   PortType[in] - 通道
//		    ID[in]      - 传入数据标识
//		    OutBufLen   - 传入缓存长度
//			OutBuf[out] - 传出数据
//
//返回值:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//
//备注:
//---------------------------------------------------------------
WORD GetPro645RequestMeterPara( eSERIAL_TYPE PortType,BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Buf[MAX_PARA_BUF_LENTH];
	BYTE TaggedByteBuf[1];
	BYTE i,Status;
	BYTE DataBuf[20];
	WORD wID1_2, Lenth;
	TFourByteUnion DataID;
	TFourByteUnion FourTemp;	//用于4字节数据的特殊处理
	BYTE Type;
	BYTE Sel_HEX2BCD; 			//是否需要HEX转BCD
	BYTE Sel_EXData;			//是否需要倒序
	eTYPE_COVER_DATA CoverMode;	//HEX到BCD数据转换方式

	BYTE ReceiveErrorNum;			//局部变量，6025抄读接受415失败次数
	
	//默认BCD转换 无需倒序
	Sel_HEX2BCD = YES;
	Sel_EXData = YES;
	CoverMode = eBYTE_MODE;
	FourTemp.d = 0;
	Lenth = 0;
	Status = 0;
	Type = 0;
	memset( Buf, 0x00, sizeof(Buf) );
	memset( DataBuf, 0x00, sizeof(DataBuf) );
	
	if( OutBufLen == 0 )
	{
		return DLT645_ERR_ID_02;
	}

	memcpy((BYTE *)DataID.b, DI, 4);
	memcpy( (BYTE*)&wID1_2, DI+1, 2 );
	
	switch( wID1_2 )
	{
		case 0x0001:
			Lenth = ReadParaData0001XX( DI[0], Buf );
			Sel_HEX2BCD = NO;
			break;
		case 0x0002:
			if (DI[0] == 0x01)  //年时区数
			{
				Lenth = 1;
				Buf[0] = FPara1->TimeZoneSegPara.TimeZoneNum;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x02)  //日时段表数
			{
				Lenth = 1;
				Buf[0] = FPara1->TimeZoneSegPara.TimeSegTableNum;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x03)  //日时段数
			{
				Lenth = 1;
				Buf[0] = FPara1->TimeZoneSegPara.TimeSegNum;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x04)  //费率数
			{
				Lenth = 1;
				Buf[0] = FPara1->TimeZoneSegPara.Ratio;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x05)  //公共假日数
			{
				Lenth = 2;
				Buf[0] = 0x00;
				Buf[1] = FPara1->TimeZoneSegPara.HolidayNum;
			}
			else if (DI[0] == 0x08)  //密钥总条数
			{
				Lenth = 1;
				Buf[0] = ScretKeyNum;
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;
			}
			else
			{
				Lenth = 0;
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;
			}
			break;
		case 0x0003:
			if (DI[0] == 0x01)  //自动循环显示屏数
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.DispItemNum[0];
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x02)	//每屏显示时间
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.LoopDisplayTime;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x03)	//显示电能小数位数
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.EnergyFloat;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x04)  //显示功率（最大需量）小数位数
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.DemandFloat;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x05)  //按键循环显示屏数
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.DispItemNum[1];
				Sel_EXData = NO;
			}
			else if( DI[0] <= 0x07 )  //电流互感器变比 电压互感器变比
			{
				Lenth = api_ReadPrePayPara( (ePrePayParaType) (DI[0]-0x05), FourTemp.b );

				if( Lenth == 4 )
				{
					Lenth = 3;
					FourTemp.d = lib_DWBinToBCD( FourTemp.d );
					memcpy( Buf, FourTemp.b, Lenth );
					Sel_HEX2BCD = NO;
					Sel_EXData = NO;
				}
			}
			else if (DI[0] == 0x08)  //上电全显时间 
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.PowerOnDispTimer;
				Sel_EXData = NO;
			}
			else
			{
				Lenth = 0;
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;
			}
			break;
		case 0x0004:
			//TMeterTypeEnum枚举中，6之后加入2个698参数，导致645参数与DI[0]不对应
			Type = (DI[0] > 6) ? (DI[0] + 2) : DI[0];
			Lenth = api_ProcMeterTypePara( READ, Type, Buf );	
			//脉冲常数在电表中以16进制存储 需要转BCD 无需倒序
			#if( MAX_PHASE == 1 )	
			if( DI[0] == 0x09 ) //电表有功常数
			#else
			if( (DI[0] == 0x09)||(DI[0] == 0x0A) ) //电表无功常数
			#endif	
			{
				Lenth = 3;
				memcpy( (void *)FourTemp.b, Buf, 4 );
				FourTemp.d = lib_DWBinToBCD( FourTemp.d );
				memcpy( Buf, FourTemp.b, Lenth );
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;
			}
			else if( DI[0] == 0x0B )	//电表型号
			{
				Lenth = 10;			//645：电表型号10字节	698:32字节
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0] == 0x0C )	//生产日期
			{
				if( api_CheckClock((TRealTimer *)Buf)!=TRUE )
				{
               		memcpy(Buf,ProduceDate,7);
            	} 
				Lenth = 10;
				
				//HEX转换为ASCII
				SwitchProduceDataToAscii( Buf, DataBuf );
				memcpy( Buf, DataBuf, 10 );
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0] == 0x0D ) //645协议版本号
			{
				Lenth = 16; 
				memcpy( Buf, ProtocolVersionConst_645, 16 );
				Sel_HEX2BCD = NO; 
			}
			else if( DI[0] == 0x0F )  //设备地理位置 电能表位置信息
            {
				memset( Buf, 0x00, 11 ); //645地理位置固定为0
				
				Lenth = 11;
				Sel_HEX2BCD = NO;
                Sel_EXData = NO;
            }
			else
			{
				Sel_HEX2BCD = NO;
			}
			break;
		case 0x0005:	//电表运行状态字 密钥状态字
			if( DI[0] <= 0x07 )
			{
				Lenth = api_GetMeterStatus( ePROTOCO_645, DI[0], 2, Buf );
				if( Lenth != 2 )
				{
					Lenth = 0;
				}
			}
			else if( DI[0] == 0x08 )
			{
				if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == TRUE)
				{	
					FourTemp.d = 0;						//公钥返回0
					memcpy( Buf, FourTemp.b, 4 );
				}
				else
				{
					FourTemp.d = 0xFFFF0F00;    		//私钥返回定值 0x000FFFFF
					memcpy( Buf, FourTemp.b, 4 );
				}
				Lenth = 4;
			}
			else if( DI[0] == 0xFF )  //返回长度 Lenth 为18字节 电表运行状态字共14字节，密钥状态字4字节
			{
				Lenth = api_GetMeterStatus( ePROTOCO_645, 0x00, 14, Buf );
				if( Lenth == 14 )
				{
					if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == TRUE)
					{	
						FourTemp.d = 0;
						memcpy( Buf+14, FourTemp.b, 4 );
					}
					else
					{
						FourTemp.d = 0xFFFF0F00;
						memcpy( Buf+14, FourTemp.b, 4 );
					}
					Lenth += 4;
				}
				else
                {  
					Lenth = 0;
				}
              
				if( Lenth == 18 )
				{
                    for( i=0; i<7; i++ )
                    {
                        lib_InverseData( (Buf+(i*2)), 2 );
                    } 
                    lib_InverseData( Buf+14, 4 );
                }
                else
                {  
					Lenth = 0;
				}
				
				Sel_EXData = NO;
			}
			Sel_HEX2BCD = NO;
			break;
		case 0x0006:
			if( DI[0] == 0x01 )	//有功组合方式特征字
			{
				Buf[0]=FPara2->EnereyDemandMode.byActiveCalMode;
				Lenth = 1;
			}
			#if(SEL_RACTIVE_ENERGY == YES)
			else if( DI[0] == 0x02 )	//无功组合方式1特征字
			{
				Buf[0]=FPara2->EnereyDemandMode.PReactiveMode;
				Lenth = 1;
			}
			else if( DI[0] == 0x03 )	//无功组合方式2特征字
			{
				Buf[0]=FPara2->EnereyDemandMode.NReactiveMode;
				Lenth = 1;
			}
			#endif//#if(SEL_RACTIVE_ENERGY == YES)
			Sel_HEX2BCD = NO;
			break;
		case 0x0007:
			if( DI[0] == 0x01 )
			{
				Buf[0] = 0x04;  //红外固定波特率1200
				Lenth = 1;
				Sel_HEX2BCD = NO;
			}
			else if( (DI[0] >= 0x03) && (DI[0] <= 0x05) )
			{
				if( DI[0] == 0x03 )
				{
					Buf[0] = FPara2->CommPara.I485&0x0F;
				}
				else if( DI[0] == 0x04 )
				{
					Buf[0] = FPara2->CommPara.II485&0x0F;
				}
				else
				{
					Buf[0] = FPara2->CommPara.ComModule&0x0F;
				}

				if( Buf[0] < 0x05 )
				{
					Buf[0] = 0x01 << Buf[0];
				}
				else if( Buf[0] <= 0x07 ) //698有7200波特率 645没有 所以Buf[0]要减一
				{
					Buf[0] = 0x01 << (Buf[0] - 1);
				}
				Lenth = 1;
				Sel_HEX2BCD = NO;
			}
			break; 
		case 0x0008:
			if( (DI[0] > 0x00) && (DI[0] <= 0x02) )   //周休日特征字 周休日釆用的日时段表号
			{
				Buf[0] = *(&FPara1->TimeZoneSegPara.WeekStatus + (DI[0] - 0x01));
				Lenth = 1;
				Sel_HEX2BCD = NO;
			}
			break;
		case 0x000a:    //负荷曲线间隔时间
        	Lenth = api_ReadFreezeLpf645IntervalTime( DI, Buf );
        	Sel_HEX2BCD = NO;
        	Sel_EXData = NO;
        	break;	
		case 0x000B: //每月第1结算日 每月第2结算日 每月第3结算日
			if( DI[0]<=0x03 )
			{
				if( ReadSettlementDate645( DI[0], Buf) == TRUE )
				{
					Lenth = 2;
					Sel_EXData = YES;
				}
			}
			break;
		case 0x0011:
			if( DI[0] == 0x01 )	//电表运行特征字1 
			{
                Status = 0;

			    if( api_GetRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS ) == TRUE )
			    {
                    Status |= BIT2;
			    }

			    if( (FPara1->LCDPara.Meaning12) == 1 )
			    {
                    Status |= BIT1;
			    }
			    
                ReadRelayPara( 3, Buf);
			    if( Buf[0] == 0 )
			    {
                    Status |= BIT0;
			    }
			    
				Buf[0]= Status;
				Lenth = 1;
				Sel_EXData = NO;
				Sel_HEX2BCD = NO;
				
			}
			else if( DI[0] == 0x04 )//主动上报模式字 
			{
				Lenth = api_ReadReportStatusMode( Buf );
				
				Sel_EXData = NO;
				Sel_HEX2BCD = NO;
			}
			break;
		case 0x0014:    
			switch( DI[0] )
			{
				case 0x01:  //跳闸延时时间 拉闸延时时间
					if( ReadRelayPara( 2,  Buf ) == TRUE )
					{
						Lenth = 2;
						Sel_HEX2BCD = YES;
						CoverMode = eCOMB_MODE;
						Sel_EXData = NO;
					}
					break;
				case 0x02:  //继电器拉闸控制电流门限值
					if( ReadRelayPara( 0,  FourTemp.b ) == TRUE )
					{
						Lenth = 3;
						FourTemp.d = lib_DWBinToBCD( FourTemp.d / 10 ); //写数据时乘过10
						memcpy( Buf, FourTemp.b, Lenth );
						Sel_HEX2BCD = NO;
						Sel_EXData = NO; //
					}
					break;
				case 0x03:  //超拉闸控制电流门限保护延时时间
					if( ReadRelayPara( 1, Buf ) == TRUE )
					{
						Lenth = 2;
						Sel_EXData = NO;
						CoverMode = eCOMB_MODE;
					}
					break;
				case 0x04:  //红外认证时效
					api_ReadPrePayPara( eIRTime, Buf ); //强制转化为1字节
					Lenth = 1;
					Sel_EXData = NO;
					Sel_HEX2BCD = YES;
					break;
				case 0x05:  //主动上报状态字自动复位延时时间
					Buf[0] = api_GetReportResetTime();
					Lenth = 1;
					Sel_EXData = NO;
					Sel_HEX2BCD = YES;
					break;
				default:
					break;
			}
			break;
        case 0x0015:    //主动上报状态字
            if( DI[0] == 0x01 ) //读主动上报状态字
            {   
                Lenth = api_ReadReportStatusByte( PortType, eREAD_STATUS_PROC, Buf );
                Sel_EXData = NO;
                Sel_HEX2BCD = NO;
            }
            break;		
		case 0x0100:	//第一套时区表数据 第一套日时段表数据
		case 0x0200:	//第二套时区表数据 第二套日时段表数据
			if( DI[0] == 0x00 )	
			{
				Lenth  = FPara1->TimeZoneSegPara.TimeZoneNum * 3;
				FourTemp.b[0] = 0x01;
			}
			else if( DI[0] <= 0x08 )
			{
				Lenth = FPara1->TimeZoneSegPara.TimeSegNum * 3;
				FourTemp.b[0] = DI[0] + 0x02;
			}
			if(DI[2] == 0x02)   
            {
				FourTemp.b[0] |= 0x80;
            }

			if( api_ReadTimeTable( FourTemp.b[0], 0, Lenth, Buf ) == FALSE )
			{
				Lenth = 0;
			}
			//分项倒序
			for( i = 0; i < Lenth / 3; i++ )
			{
				lib_InverseData( &Buf[3*i], 3 );
			}
			Sel_EXData = NO;
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )	//Sel_HEX2BCD\Sel_EXData都选NO，在函数内部处理数据格式，外部的处理有问题，处理不了这些数据
		case 0x0501:
		case 0x0502:
			Lenth = ReadPriceParaData645(0x05,DI[1],DI[0],Buf);
			Sel_HEX2BCD = NO;
			Sel_EXData = NO;
			break;
		case 0x0604:
		case 0x0605:
			Lenth = ReadPriceParaData645(0x06,DI[1],DI[0],Buf);
			Sel_HEX2BCD = NO;
            Sel_EXData = NO;
			break;
		#endif
		case 0x8000:
			switch( DI[0] )
			{
				case 0x01:	//厂家软件版本号
					Status = eMeterSoftWareVersion;
                    break;
				case 0x02:  //厂家硬件版本号
					Status = eMeterHardWareVersion;
                    break;
				case 0x03:  //厂家编号
					Status = eMeterFactoryCode;
					break;
				case 0x04:	//软件备案号
					Status = eMeterSoftRecord;
					break;
				default:
					Status = 0xff;
					break;
			}
			if( Status != 0xff )
			{
				if(api_ProcMeterTypePara( READ, Status, DataBuf ) != FALSE)
				{
					if(DI[0] == 0x04)
					{
						SwitchSoftRecordToBCD( DataBuf, Buf ); 
	                    Lenth = 8;
					}
					else
					{
						memcpy( Buf, DataBuf, 4 );
						Lenth = 32;
					}


				}
				else
				{
					Lenth = 0;
				}
			}
			else
			{
				Lenth = 0;
			}
			Sel_EXData = YES;
			Sel_HEX2BCD = NO;
			break;
		#if (SEL_F415 == YES)
		case 0x000F:
			if( DI[0]==0x00 )		//读取电压电流系数
			{
				memcpy(Buf,(BYTE*)&TF415UIK,sizeof(TSampleUIK)-sizeof(DWORD) );
				Lenth = sizeof(TSampleUIK)-sizeof(DWORD);
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0]==0x01 )	//抄读第一回路负荷辨识结果
			{
				api_ReadIdentResult(0, Buf);

				Lenth = sizeof(TElectIdentify)*IDF_RES_NUM;
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0]==0x05 )	//抄读第二回路负荷辨识结果
			{
				api_ReadIdentResult(1, Buf);

				Lenth = sizeof(TElectIdentify)*IDF_RES_NUM;
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0]==0xB0 )	//读取6025_to_415采集任务的开关状态
			{
				Buf[0] = api_GetCollectionTaskStatus415();
				Lenth = 1;
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0]==0xB1 )	//读取415返回的识别结果
			{
				
				memcpy(Buf,(BYTE*)&MessageData.ucTime,sizeof(AMessage_T) );
				Lenth = sizeof(AMessage_T);
				
				//需要单独处理一下年，倒序+年位置
				ProBinToBCD( eCOMB_MODE, sizeof(WORD), Buf );
				ProBinToBCD( CoverMode, sizeof(TRealTimer)-sizeof(WORD), Buf+2 );
				lib_InverseData(Buf,sizeof(WORD));
				lib_ExchangeData(Buf,Buf,sizeof(TRealTimer));
				Sel_HEX2BCD = NO;
				// Sel_EXData = YES;
				Sel_EXData = NO;
			}
			else if( DI[0]==0xB2 )	//读取抄读415失败次数
			{
				//415的第一路uart为与6025通信的主uart。
				ReceiveErrorNum =  TSendRec[0].Sec_Send_Success - TSendRec[0].Sec_Rec_Success;
				memcpy(Buf,(BYTE*)&ReceiveErrorNum,sizeof(ReceiveErrorNum) );
				Lenth = sizeof(ReceiveErrorNum);
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			break;
		#endif//#if (SEL_F415 == YES)
		default:
			break;
	}
	//数据长度超长
	if( (Lenth == 0x00) || (Lenth > OutBufLen) )
	{
		return DLT645_ERR_ID_02;
	}
	
	//统一处理HEX转BCD
	if( Sel_HEX2BCD == YES )
	{
		//不判返回值 即使转换错误 也要返回
		ProBinToBCD( CoverMode, Lenth, Buf );
	}
	
	//统一处理倒序
	if( Sel_EXData == YES )
	{
		lib_ExchangeData( OutBuf, Buf, Lenth );
	}
	else
	{
		memcpy( OutBuf, Buf, Lenth );
	}
	
	return Lenth;
}


#endif//#if( SEL_DLT645_2007 == YES )
