//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人    	尹西科
//创建日期		2017年9月20日09:16:13
//描述			645读取变量
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "dlt645_2007.h"

#if( SEL_DLT645_2007 == YES )

//用于通讯读取计量类瞬时量
#define PRO_PHASE_T				(0x01)		//瞬时量总
#define PRO_PHASE_A				(0x02)		//瞬时量A
#define PRO_PHASE_B				(0x04)		//瞬时量B
#define PRO_PHASE_C				(0x08)		//瞬时量C
#define PRO_PHASE_N				(0x10)		//瞬时量N
#define PRO_PHASE_ABC			(PRO_PHASE_A+PRO_PHASE_B+PRO_PHASE_C)
#define PRO_PHASE_TABC			(PRO_PHASE_T+PRO_PHASE_A+PRO_PHASE_B+PRO_PHASE_C)

static const TMeteringVaribleTab ConstMeteringVaribleTab[] =
{
	//电压
	{ 0x0101, REMOTE_U, PRO_PHASE_A, 2, 1 },
	#if( MAX_PHASE == 3 )
	{ 0x0102, REMOTE_U, PRO_PHASE_B, 2, 1 },
	{ 0x0103, REMOTE_U, PRO_PHASE_C, 2, 1 },
	#endif//#if(MAX_PHASE == 3)
	{ 0x01FF, REMOTE_U, PRO_PHASE_ABC, 2, 1 },

	//电流
	{ 0x0201, REMOTE_I, PRO_PHASE_A, 3, 3 },
	#if( MAX_PHASE == 3 )
	{ 0x0202, REMOTE_I, PRO_PHASE_B, 3, 3 },
	{ 0x0203, REMOTE_I, PRO_PHASE_C, 3, 3 },
	#endif//#if(MAX_PHASE == 3)
	{ 0x02FF, REMOTE_I, PRO_PHASE_ABC, 3, 3 },

	//有功功率
	{ 0x0300, REMOTE_P, PRO_PHASE_T, 3, 4},
	{ 0x0301, REMOTE_P, PRO_PHASE_A, 3, 4},
	#if( MAX_PHASE == 3 )
	{ 0x0302, REMOTE_P, PRO_PHASE_B, 3, 4},
	{ 0x0303, REMOTE_P, PRO_PHASE_C, 3, 4},
	#endif//#if(MAX_PHASE == 3)
	{ 0x03FF, REMOTE_P, PRO_PHASE_TABC, 3, 4},

	//无功功率
	{ 0x0400, REMOTE_Q, PRO_PHASE_T, 3, 4},
	{ 0x0401, REMOTE_Q, PRO_PHASE_A, 3, 4},
	#if( MAX_PHASE == 3 )
	{ 0x0402, REMOTE_Q, PRO_PHASE_B, 3, 4},
	{ 0x0403, REMOTE_Q, PRO_PHASE_C, 3, 4},
	#endif//#if(MAX_PHASE == 3)
	{ 0x04FF, REMOTE_Q, PRO_PHASE_TABC, 3, 4},

	//视在功率
	{ 0x0500, REMOTE_S, PRO_PHASE_T, 3, 4},
	{ 0x0501, REMOTE_S, PRO_PHASE_A, 3, 4},
	#if( MAX_PHASE == 3 )
	{ 0x0502, REMOTE_S, PRO_PHASE_B, 3, 4},
	{ 0x0503, REMOTE_S, PRO_PHASE_C, 3, 4},
	#endif//#if(MAX_PHASE == 3)
	{ 0x05FF, REMOTE_S, PRO_PHASE_TABC, 3, 4},

	//功率因数
	{ 0x0600, REMOTE_COS, PRO_PHASE_T, 2, 3},
	{ 0x0601, REMOTE_COS, PRO_PHASE_A, 2, 3},
	#if( MAX_PHASE == 3 )
	{ 0x0602, REMOTE_COS, PRO_PHASE_B, 2, 3},
	{ 0x0603, REMOTE_COS, PRO_PHASE_C, 2, 3},
	#endif//#if(MAX_PHASE == 3)
	{ 0x06FF, REMOTE_COS, PRO_PHASE_TABC, 2, 3},

	//相角
	{ 0x0701, REMOTE_PHASE, PRO_PHASE_A, 2, 1},
	#if( MAX_PHASE == 3 )
	{ 0x0702, REMOTE_PHASE, PRO_PHASE_B, 2, 1},
	{ 0x0703, REMOTE_PHASE, PRO_PHASE_C, 2, 1},
	#endif//#if(MAX_PHASE == 3)
	{ 0x07FF, REMOTE_PHASE, PRO_PHASE_ABC, 2, 1},
};


//---------------------------------------------------------------
//函数功能: 读取变量
//
//参数: 
//			Dot[in]		   小数位数       
//          *DI[in]		   数据标识                       
//          OutBufLen[in]  读取的长度       
//          OutBuf[out]	   返回数据                   
//返回值: 
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//
//备注:   
//---------------------------------------------------------------
WORD Get645RequestVariable( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE i, Num, tDot;
	BYTE Buf[16];						//变量类最多四个字节 ABC相 内存扩大一倍
	WORD wResult, Lenth, ReadType[4];	//瞬时量读取 最多四个变量
	TFourByteUnion DataID,Time;
	TFourByteUnion FourTemp;			//用于4字节数据的特殊处理
	TMeteringVaribleTab  MVaribleTab;
	BYTE Sel_HEX2BCD; 					//是否需要HEX转BCD
	BYTE Sel_EXData;					//是否需要倒序
	eTYPE_COVER_DATA CoverMode;			//HEX到BCD数据转换方式
	
	//默认BCD转换 无需倒序
	Sel_HEX2BCD = NO;
	Sel_EXData = NO;
	CoverMode = eBYTE_MODE;
	FourTemp.d = 0;
	
	Lenth = 0;

	//复制数据标识
	memcpy( DataID.b, DI, 4 );
	wResult = 0;
	
	MVaribleTab.ID = DataID.b[1] + (DataID.b[2] << 8);
	
//读取计量类数据 数据标识 02010100 ~ 0208FF00
//========================================================================================//	
	for( i = 0; i < (sizeof(ConstMeteringVaribleTab) / sizeof(TMeteringVaribleTab)); i++ )
	{
		if( MVaribleTab.ID == ConstMeteringVaribleTab[i].ID )
		{
			memcpy( (void *)&MVaribleTab.ID, (void *)&ConstMeteringVaribleTab[i].ID, sizeof(TMeteringVaribleTab) );
			break;
		}
	}	
    
    if( Dot != 0xFF)
	{
       tDot = Dot;
	}
	else
	{
       tDot = MVaribleTab.Dot;
	}
	
	if( i != (sizeof(ConstMeteringVaribleTab) / sizeof(TMeteringVaribleTab)) )
	{	
		Num = 0;
		for( i = 0; i < 4; i++ )
		{
			if( MVaribleTab.Phase & (0x0001 << i) )
			{
				ReadType[Num++] = MVaribleTab.Type + (i << 12);
			}
		}
		
		FourTemp.b[0] = 0;
		for( i = 0; i < Num; i++ )
		{
			if( api_GetRemoteData( ReadType[i], DATA_BCD, tDot, MVaribleTab.Lenth, Buf + MVaribleTab.Lenth * i ) == TRUE )
			{
				FourTemp.b[0]++;	//借用该变量存储抄读的数据个数
			}
		}
		if( FourTemp.b[0] == 0 )	//如果一个数据都没有 返回错误
		{
			return DLT645_ERR_ID_02;
		}
		
		if( MVaribleTab.Lenth*Num < OutBufLen )
		{
			//单相数据块和三相格式相同 但是没有B相和C相 需要填充FF
			memset( OutBuf, 0xFF, MVaribleTab.Lenth * Num );
			memcpy( OutBuf, Buf, MVaribleTab.Lenth*FourTemp.b[0] );
			FourTemp.b[0] = 0;
			
			return (MVaribleTab.Lenth * Num);
		}
		
		return DLT645_ERR_ID_02;
	}
//========================================================================================//	
    
	if( (DI[2] == 0x80)&&(DI[1] == 0x00) )
	{
		switch( DI[0] )
		{
			case 0x01:	//零线电流
				#if( MAX_PHASE == 1 )
				if( api_GetRemoteData( PHASE_N + REMOTE_I, DATA_BCD, 3, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				#else
				if( MeterTypesConst == METER_3P3W)//三相表按零序电流(电流矢量和)返回
				{
					Lenth = 3;
					memset( Buf, 0x00, Lenth );
				}
				else if( MeterTypesConst == METER_ZT )//直通表按照零线电流-非直通表按照零序电流
				{
					if( api_GetRemoteData( PHASE_N + REMOTE_I, DATA_BCD, 3, 3, Buf ) == TRUE )
					{
						Lenth = 3;
					}
				}
				else
				{
					if( api_GetRemoteData( PHASE_N2 + REMOTE_I, DATA_BCD, 3, 3, Buf ) == TRUE )
					{
						Lenth = 3;
					}
				}
				#endif
				break;
			case 0x02:	//电网频率
				if( api_GetRemoteData( REMOTE_HZ, DATA_BCD, 2, 2, Buf ) == TRUE )
				{
					Lenth = 2;
				}
				break;
			case 0x03:	//一分钟有功总平均功率
				if( api_GetRemoteData( REMOTE_P_AVE, DATA_BCD, 4, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
			
			#if( MAX_PHASE == 3 )
			case 0x04:	//当前有功需量
			case 0x05:	//当前无功需量
			case 0x06:	//当前视在需量
				api_GetCurrDemandData((DI[0] - 0x03), DATA_BCD, 4, 3, Buf );
				
				Lenth = 3;
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;				
				break;
			#endif//#if( MAX_PHASE == 3 )
			case 0x07:	//表内温度
				wResult = api_GetBatAndTempValue( DATA_BCD, SYS_TEMPERATURE_AD);
				memcpy( Buf, (void*)&wResult, 2 );
				Lenth = 2;
				break;
			case 0x08:	//时钟电池电压(内部)
				wResult = api_GetBatAndTempValue( DATA_BCD, SYS_CLOCK_VBAT_AD);
				memcpy( Buf, (void*)&wResult, 2 );
				Lenth = 2;
				break;
			case 0x09:	//停电抄表电池电压 (外部)
				wResult = api_GetBatAndTempValue ( DATA_BCD, SYS_READ_VBAT_AD);
				memcpy( Buf, (void*)&wResult, 2 );
				Lenth = 2;
				break;
			case 0x0A:	//内部电池工作时间
				if( api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( BatteryTime ), sizeof(DWORD), Buf ) == TRUE )
				{
					Lenth = 4;
					Sel_HEX2BCD = YES;
					CoverMode = eCOMB_MODE;
				}
				break;				
			#if(PREPAY_MODE == PREPAY_LOCAL)	
			case 0x0B://当前阶梯电价	XXXX.XXXX	4	元/kWh
				api_ReadPrePayPara(eCurLadderPrice, Buf);
				Sel_HEX2BCD = YES;
				Sel_EXData = NO;
				CoverMode = eCOMB_MODE;
				Lenth = 4;
				break;
			#endif				
			case 0x0C:	//零线功率因数---南网
				if( api_GetRemoteData( PHASE_N + REMOTE_COS, DATA_BCD, 3, 2, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
			case 0x0D:	//零线有功功率---南网 
				if( api_GetRemoteData( PHASE_N + REMOTE_P, DATA_BCD, 4, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
				break;
			case 0x0E:	//火线电流---南网
				if( api_GetRemoteData( PHASE_A + REMOTE_I, DATA_BCD, 3, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
			case 0x0F:  //火线有功功率---南网
				if( api_GetRemoteData( PHASE_A + REMOTE_P, DATA_BCD, 4, 3, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;
			case 0x10:	//火线功率因数---南网
				if( api_GetRemoteData( PHASE_A + REMOTE_COS, DATA_BCD, 3, 2, Buf ) == TRUE )
				{
					Lenth = 3;
				}
				break;				
			#if(PREPAY_MODE == PREPAY_LOCAL)	
        	case 0x20: //当前电价
				api_ReadPrePayPara(eCurPice, Buf);
				Sel_HEX2BCD = YES;
				Sel_EXData = NO;
				CoverMode = eCOMB_MODE;
				Lenth = 4;
				break;
        	case 0x21: //当前费率电价
				api_ReadPrePayPara(eCurRatePrice, Buf);
				Sel_HEX2BCD = YES;
				Sel_EXData = NO;
				CoverMode = eCOMB_MODE;
				Lenth = 4;
				break;
			#endif			
//			case 0x22:	//身份认证时效剩余时间
//				if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE )
//				{
//					memcpy( Time.b, (BYTE *)&APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime, 4 );
//					if( (Time.d%60) != 0 )
//					{
//						Time.d = (Time.d /60)+1;
//					}
//					else
//					{
//						Time.d = (Time.d /60);
//					}
//				}
//				else
//				{
//					Time.d = 0;
//				}
//
//			    memcpy( Buf, Time.b, 2 );//强制转化为2个字节
//				Lenth = 2;
//				Sel_HEX2BCD = YES;
//				CoverMode = eCOMB_MODE;
//				break;
//			case 0x23:	//红外认证时效剩余时间
//			    memcpy( Time.b, (BYTE*)&APPConnect.IRAuthTime, 4); 
//			    if( (Time.d%60) != 0 )
//			    {
//			        Time.d = (Time.d /60)+1;
//			    }
//			    else
//			    {
//                    Time.d = (Time.d /60);
//			    }
//
//			    Buf[0] = Time.b[0];//强制转化为1字节
//			    Lenth = 1;
//				Sel_HEX2BCD = YES;
//				break;
			default:
				break;
		}
	}
	
	//数据长度超长
	if( (Lenth == 0x00) || (Lenth > OutBufLen) )
	{
		return DLT645_ERR_ID_02;
	}
	
	//统一处理HEX转BCD
	if( Sel_HEX2BCD == YES )
	{
		if( ProBinToBCD( CoverMode, Lenth, Buf ) == FALSE )
		{
			return DLT645_ERR_ID_02;
		}
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

#endif//#if( (SEL_DLT645_2007 == YES) || (SEL_DLT645_2007_BASIC == YES) )

