//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.27
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
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

static const BYTE T_MeterStatus[]		= { Array_698, 0x07, Bit_string_698,         16  };		//有符号需量数据标识
static const BYTE T_AH[]				= { Array_698, 0x04, Double_long_unsigned_698, 4       };		//有符号需量数据标识
static const BYTE T_RunTime[]			= //电压合格率
{ 
	Structure_698, 				0x05, 
	Double_long_unsigned_698,	4, 
	Long_unsigned_698,			2, 
	Long_unsigned_698,			2, 
	Double_long_unsigned_698,	4, 
	Double_long_unsigned_698,	4
};	

static const BYTE T_EventNowData[]		= //事件当前值
{ 
	Structure_698, 				0x02, 
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4
};	

static const BYTE T_MoneyBag[]      = //钱包文件
{ 
    Structure_698,              0x02, 
    Double_long_unsigned_698,   4, 
    Double_long_unsigned_698,   4
};  

static const BYTE T_NI[]				= { Double_long_698, 	4 	};						//零线电流数据标识
#if(MAX_PHASE == 1)
static const BYTE T_U[]					= { Array_698, 0x01, Long_unsigned_698,	2	};		//电压数据标识
static const BYTE T_I[]					= { Array_698, 0x01, Double_long_698, 	4 	};		//电流数据标识
static const BYTE T_P[]					= { Array_698, 0x02, Double_long_698, 	4 	};		//功率数据标识
static const BYTE T_COS[]				= { Array_698, 0x02, Long_698, 			2 	};		//功率数据标识
#else
static const BYTE T_U[]					= { Array_698, 0x03, Long_unsigned_698,	2 	};		//电压数据标识
static const BYTE T_I[]					= { Array_698, 0x03, Double_long_698, 	4 	};		//电流数据标识
static const BYTE T_P[]					= { Array_698, 0x04, Double_long_698, 	4 	};		//功率数据标识
static const BYTE T_COS[]				= { Array_698, 0x04, Long_698, 			2 	};		//功率数据标识
static const BYTE T_PHASE[]				= { Array_698, 0x03, Long_unsigned_698,	2 	};		//电流-电压电流相角数据标识
#endif
static const WORD T_RemotePara[] = 
{	
	PHASE_A,	//电压
	PHASE_A,	//电流
	PHASE_A,	//电压相角
	PHASE_A,	//电压电流相角
	PHASE_ALL,	//有功功率
	PHASE_ALL,	//无功功率
	PHASE_ALL,	//视在功率
	PHASE_ALL,	//一分钟平均有功功率
	PHASE_ALL,	//一分钟平均无功功率
	PHASE_ALL,	//一分钟平均视在功率
	PHASE_ALL,	//功率因数
	PHASE_ALL,	//电压波形失真度
	PHASE_ALL,	//电流波形失真度
	PHASE_ALL,	//电压谐波含有量
	PHASE_ALL,	//电流谐波含有量
	PHASE_ALL,	//电网频率
};
//增加数据类型请查看源数据和加TAG是否支持
static const TCommonObj VariableObj[] =
{
	{  0x2000,  &SU_1V,   	T_U				},//电压
	{  0x2001,  &SU_3A,   	T_I				},//电流
#if( MAX_PHASE != 1 )	
	{  0x2002,  &SU_10,  	T_PHASE			},//电压相角
	{  0x2003,  &SU_10,  	T_PHASE			},//电压电流相角
	{  0x2005,  &SU_1VAR,  	T_P		 		},//无功功率
	{  0x2008,  &SU_1VAR,  	T_P		 		},//一分钟平均无功功率
	{  0x2009,  &SU_1VA,  	T_P		 		},//一分钟平均视在功率	
	{  0x2017,  &SU_4KW, 	T_DoubleLong	},//当前有功需量
	{  0x2018,  &SU_4KVAR, 	T_DoubleLong	},//当前无功需量
	#if( SEL_DEMAND_2022 == NO )
	{  0x2019,  &SU_4KVA, 	T_DoubleLong	},//当前视在需量
	#endif
	{  0x2026,  &SU_2PCT, 	T_UNLong 		},//电压不平衡率
	{  0x2027,  &SU_2PCT, 	T_UNLong		},//电流不平衡率
	{  0x2028,  &SU_2PCT, 	T_UNLong 		},//负载率
	{  0x2029,  &SU_2AH, 	T_AH 			},//安时值
#endif
	{  0x2004,  &SU_1W,  	T_P				},//有功功率
	{  0x2006,  &SU_1VA,  	T_P 			},//视在功率
	{  0x2007,  &SU_1W,  	T_P 			},//一分钟平均有功功率
	{  0x200A,  &SU_3,		T_COS			},//功率因数
	{  0x200F,  &SU_2HZ,  	T_UNLong		},//电网频率
	{  0x2010,  &SU_10C, 	T_Long			},//表内温度
	{  0x2011,  &SU_2v, 	T_UNLong		},//时钟电池电压
	{  0x2012,  &SU_2v, 	T_UNLong		},//停电抄表电池电压
	{  0x2013,  &SU_0MIN, 	T_UNDoubleLong	},//时钟电池工作时间
	{  0x2014,  &SU_00, 	T_MeterStatus 	},//电表运行状态字
	{  0x2015,  &SU_00, 	T_BitString32 	},//电表运行状态字
	{  0x2031,  &SU_2kwh, 	T_UNDoubleLong	},//月度用电量
	{  0x2040,  &SU_00, 	T_BitString16	},//控制命令执行状态字
	{  0x2041,  &SU_00, 	T_BitString16 	},//控制命令错误状态字
    #if( PREPAY_MODE == PREPAY_LOCAL )
    {  0x201a,  &SU_4Yuan, 	T_UNDoubleLong 	},//当前电价
    {  0x201b,  &SU_4Yuan, 	T_UNDoubleLong 	},//当前费率电价
    {  0x201c,  &SU_4Yuan, 	T_UNDoubleLong 	},//当前阶梯电价
    {  0x202c,  &SU_2Yuan, 	T_MoneyBag 	    },//钱包文件
    {  0x202d,  &SU_2Yuan, 	T_UNDoubleLong 	},//透支金额
    {  0x202e,  &SU_2Yuan,  T_UNDoubleLong  },//累计购电金额
    {  0x2032,  &SU_2kwh,   T_UNDoubleLong  },//阶梯结算用电量
	
    #endif
	{  0x2700,  &SU_1V,  	T_UNLong		},
	{  0x2701,  &SU_3A,  	T_DoubleLong	},
	{  0x2702,  &SU_1V,  	T_UNLong		},
	{  0x2704,  &SU_10C,  	T_UNLong 		},
	{  0x2705,  &SU_00,  	T_UNLong 		},
	{  0x2706,  &SU_10C, 	T_Long			},
	{  0x2707,  &SU_10C,  	T_Long	 		},
	{  0x2708,  &SU_1V, 	T_UNLong		},
	{  0x2709,  &SU_00,  	T_UNLong 		},
    #if( SEL_STAT_V_RUN == YES )	
	{  0x2131,  &SU_00, 	T_RunTime 		},//当月A相电压合格率
	{  0x2132,  &SU_00, 	T_RunTime 		},//当月B相电压合格率
	{  0x2133,  &SU_00, 	T_RunTime 		},//当月C相电压合格率
    #endif	
//加TAG专用
	{  0x201e,  &SU_00, 	T_DateTimS		},//事件发生时间
	{  0x2020,  &SU_00, 	T_DateTimS		},//事件结束时间
	{  0x2021,  &SU_00, 	T_DateTimS		},//数据冻结时间
	{  0x2022,  &SU_00, 	T_UNDoubleLong	},//事件记录序号
	{  0x2023,  &SU_00, 	T_UNDoubleLong	},//冻结记录序号
	{  0x2024,  &SU_00, 	T_Enum			},//事件发生源
	{  0x2025,  &SU_00, 	T_EventNowData	},//事件当前值
	{  0x2051,  &SU_00, 	T_EventNowData	},
};								
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  根据OI查找对应变量索引
//         
//参数:      OI[in]:	OI
//         
//返回值:    变量数组索引号
//         
//备注:  
//--------------------------------------------------
static BYTE SearchVariableOAD( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(VariableObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == VariableObj[i].OI )
		{
			return i;
		}
	}

	if( i == Num )//未找到
	{
		return 0x80;
	}
	return 0x80;
}

//--------------------------------------------------
//功能描述:  获取变量数据
//         
//参数:      
//			Dot[in]:		获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//			*pOAD[in]:		OAD         
//			VariableIndex[in]:VariableObj中的索引         
//			*OutBuf[in]:	输出变量
//         
//返回值:   返回变量数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注: 不带tag
//--------------------------------------------------
static WORD GetVariableData( BYTE Ch, BYTE Dot, BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex,Num,i;
	WORD VariableType,Result,Len,IType;
	BYTE *pBufBak = InBuf;
	#if( PREPAY_MODE != PREPAY_NO )
	TFourByteUnion Money;
	#endif
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Len =0;
	switch( ClassAttribute )
	{
		case 2://总及费率最大需量数组
			switch( OI.w )
			{
				case 0x2000://电压
				case 0x2001://电流
				case 0x2002://电压相角
				case 0x2003://电压电流相角
				case 0x2004://有功功率
				case 0x2005://无功功率
				case 0x2006://视在功率
				case 0x2007://一分钟平均有功功率
				case 0x2008://一分钟平均无功功率
				case 0x2009://一分钟平均视在功率
				case 0x200A://功率因数
					if( ClassIndex > VariableObj[VariableIndex].Type2[1])
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
    			    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
					i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
					Num = Num+i;
					for( ; i<Num ; i++ )
					{
						VariableType = (T_RemotePara[(OI.w-0x2000)]+i*0x1000+(OI.w-0x2000));					
						Result = api_GetRemoteData( VariableType, DATA_HEXCOMP, Dot, Len, InBuf );
						if( Result == FALSE )
						{
							return 0x8000;
						}
						InBuf += Len;
					}
					break;
				case 0x200F://电网频率
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
    			    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = api_GetRemoteData( 0x0f, DATA_HEXCOMP, Dot, Len, InBuf );
					if( Result == FALSE )
					{
						return 0x8000;
					}
					InBuf += Len;
					break;

				case 0x2010://表内温度
				case 0x2011://时钟电池电压
				//case 0x2012://停电抄表电池电压
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					
					if( OI.w == 0x2010 )
					{
						VariableType = SYS_TEMPERATURE_AD;
					}
					else if( OI.w == 0x2011 )
					{
						VariableType = SYS_CLOCK_VBAT_AD;
					}
					else
					{
						VariableType = SYS_READ_VBAT_AD;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
                    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = api_GetBatAndTempValue( DATA_HEX, VariableType);
					if(Result == 0x8000 )
					{
						return 0x8000;
					}
					memcpy( InBuf,(BYTE*)&Result,Len);
					InBuf += Len;
					break;

				case 0x2013://时钟电池工作时间 
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
					Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( BatteryTime ), sizeof(DWORD), InBuf);
					if(Result == FALSE )
					{
						return 0x8000;
					}
					InBuf += Len;
					break;

				case 0x2014://电表运行状态字
					Result = api_GetMeterStatus( ePROTOCO_698, ClassIndex, 100, InBuf);
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					InBuf += Result;		
					break;

                case 0x2015://电表跟随上报字
                    if( ClassIndex != 0 )
					{
						return 0x8000;
					}
                    Result = api_ReadFollowReportStatus_Mode( Ch, 0, InBuf );
                    if( Result == 0x8000 )
                    {
                        return 0x8000;
                    }
                    InBuf += Result;		
                break;	
				
				#if( MAX_PHASE == 3 )//只有三相表才打开需量功能
				case 0x2017://当前有功需量
				case 0x2018://当前无功需量
				case 0x2019://当前视在需量
				
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					api_GetCurrDemandData( (OI.w-0x2016), DATA_HEXCOMP, Dot, 4, InBuf );
					InBuf += Len;
					break;
				#endif

				case 0x2026://电压不平衡率
				case 0x2027://电流不平衡率
				case 0x2028://负载率
				    if( ClassIndex != 0 )
				    {
                        return 0x8000;
				    }
				    
					Result = api_GetSampleStatus( OI.w-0x2026, 0 );
					if(Result == 0xffff)
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					memcpy( InBuf, (BYTE*)&Result,Len );
					InBuf += Len;
					break;

				#if( SEL_AHOUR_FUNC == YES)
				case 0x2029://安时值				
					if( ClassIndex > 4 )
					{
						return 0x8000;
					}

					Num = ((ClassIndex == 0) ? 4 : 1);
					i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
					Num = Num+i;
					
					for( ; i<Num; i++ )
					{
						Result = api_GetAHourData( i, DATA_HEXCOMP, InBuf );
						if(Result == FALSE)
						{
							return 0x8000;
						}
						InBuf += 4;
					}
					break;
				#endif
				#if( PREPAY_MODE == PREPAY_LOCAL )
				case 0x201a://当前电价
				    if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eCurPice, InBuf );
                    InBuf += Len;
				    break;

				case 0x201b://当前费率电价
					if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eCurRatePrice, InBuf );
                    InBuf += Len;
				    break;

				case 0x201c://当前阶梯电价
					if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eCurLadderPrice, InBuf );
                    InBuf += Len;
				    break;
                case 0x202c://当前钱包文件
                    if( ClassIndex > 2 )
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eRemainMoney, (BYTE*)Money.b );
					api_ReadPrePayPara( eBuyTimes, InBuf + 4 );
					if( Money.l < 0 )
					{
						memset( &Money, 0x00, sizeof(Money) );
					}
					memcpy( InBuf, (BYTE*)Money.b, 4 );

                    if( ClassIndex != 0 )
                    {
                        memmove( InBuf, InBuf+(ClassIndex-1)*4, 4 );
                        InBuf += 4;
                    }
                    else
                    {
                        InBuf += 8;
                    }
                    break;

                case 0x202d://透支金额
                    if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eRemainMoney, (BYTE*)Money.b );
                    if( Money.l > 0 )//无透支金额
                    {
                        memset( InBuf, 0x00, 4 );
                    }
                    else
                    {
                        Money.l = ~Money.l;
                        Money.l += 1;
                        memcpy( InBuf, (BYTE*)Money.b, 4 );
                    }

                    InBuf += 4;
                    break;
				case 0x202e://累计购电金额
					if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadTotalBuyMoney( InBuf );

                    InBuf += 4;
                    break;
                case 0x2032://阶梯结算用电量
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}

					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = api_ReadPrePayPara( eLadderUseEnergy, InBuf );
					if( Result == 0 )
					{
						return 0x8000;
					}
					InBuf += Len;
                    break;

				#endif
				case 0x2031://月度用电量
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}

					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = api_GetAccuEnergyData( 0, DATA_HEXCOMP, 2, InBuf );
					if( Result == FALSE )
					{
						return 0x8000;
					}
					InBuf += Len;
					break;
				case 0x2040://控制命令执行状态字
				case 0x2041://控制命令错误状态字
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = GetRelayReturnStatus((OI.w-0x2040));
					lib_InverseData((BYTE *)&Result, 2);//由于后面是取word的低字节因此先进行倒序
					memcpy( InBuf, (BYTE*)&Result,Len );
					InBuf += Len;
					break;
				#if( SEL_STAT_V_RUN == YES )
				case 0x2131://当月A相电压合格率
				case 0x2132://当月B相电压合格率
				case 0x2133://当月C相电压合格率				
					if( ClassIndex > 2 )
					{
						return 0x8000;
					}
					VariableType = ((ClassIndex==0) ? (BIT0+BIT1) : ClassIndex);
					Len = ReadVRunTime( (OI.w-0x2131+1), VariableType, InBuf );
					if(Len == 0 )
					{
						Len = 0x8000;
						break;
					}
					InBuf += Len;
					break;
				#endif
				case 0x2051:
					InBuf[Len++]=0;
					break;
				// case 0x2700:
				// case 0x2701:
				// case 0x2702:
				// case 0x2704:
				// case 0x2705:
				// case 0x2706:
				// case 0x2707:
				// case 0x2708:
				// case 0x2709:
				// 	Len = Charging698(eChargingGet,InBuf,OI.w,ClassAttribute,api_GetChargingPileCanID(LinkData[Ch].ServerAddr),ClassIndex);
				// 	InBuf += Len;
				// 	break;
				default:
					return 0x8000;
			}
			break;
		case 3://换算及单位
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			InBuf[0] = VariableObj[VariableIndex].Scaler_Unit3->Scaler;
			InBuf[1] = VariableObj[VariableIndex].Scaler_Unit3->Unit;
			InBuf += 2;
			break;

		case 4://零线电流和电表跟随上报状态字
		case 6://零序电流
			// if ((OI.w >= 0x2700) && (OI.w <= 0x2709))
			// {
			// 	Len = Charging698(eChargingGet,InBuf,OI.w,ClassAttribute,api_GetChargingPileCanID(LinkData[Ch].ServerAddr),ClassIndex);
			// 	InBuf += Len;
			// 	break;
			// }else 
			if( (OI.w != 0x2001) && (OI.w != 0x2015) )
			{
				return 0x8000;
			}
			
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( OI.w == 0x2001 )
			{
				if( ClassAttribute == 4 )//零线电流
				{
					return 0x8000;
				}
				else
				{
					IType = PHASE_N2+REMOTE_I;
				}
				
                Len = GetTypeLen( eData, T_NI );
                if( Len == 0x8000 )
                {
                    return 0x8000;
                }               
                Result = api_GetRemoteData( IType , DATA_HEXCOMP, Dot, Len, InBuf );
                if( Result == FALSE )
                {
                    return 0x8000;
                }
                
                InBuf += Len;
			}
			else if( ClassAttribute == 4 )//2015只支持属性4
			{
                Result = api_ReadFollowReportStatus_Mode( Ch, 1, InBuf );
                if( Result == 0x8000 )
                {
                    return 0x8000;
                }
                InBuf += Result;		
			}
			else
			{
				return 0x8000;
			}

			break;
		case 5://电表跟随上报字上报方式
			if( OI.w == 0x2015 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Result = api_ReadEventReportMode( eReportMethod, OI.w, InBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				InBuf += 1;	
			}
			// else if ((OI.w >= 0x2700) && (OI.w <= 0x2709))
			// {
			// 	Len = Charging698(eChargingGet,InBuf,OI.w,ClassAttribute,api_GetChargingPileCanID(LinkData[Ch].ServerAddr),ClassIndex);
			// 	InBuf += Len;
			// }
			else
			{
				return 0x8000;
			}
            break;	
		// case 7:
		// 	if ((OI.w >= 0x2700) && (OI.w <= 0x2709))
		// 	{
		// 		Len = Charging698(eChargingGet,InBuf,OI.w,ClassAttribute,api_GetChargingPileCanID(LinkData[Ch].ServerAddr),ClassIndex);
		// 		InBuf += Len;
		// 	}
		// 	else
		// 	{
		// 		return 0x8000;
		// 	}
		// 	break;
		default:
			return 0x8000;
	}

	return  (WORD)(InBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  对变量数据进行加TAG
//         
//参数:      *pOAD[in]：		OAD         
//           VariableIndex[in]:	VariableObj中的索引         
//           *InBuf[in]：		变量要添加tag的数据         
//           OutBufLen[in]：	OutBuf长度       
//           *OutBuf[out]：		输出数据
//         
//返回值:    返回添加tag后数据长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注:  	无
//--------------------------------------------------
static WORD GetVariableAddTag(BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,Num,i;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth,Lenth1;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	Lenth = 0x8000;
	Num = ((ClassIndex == 0) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	
	switch( ClassAttribute )
	{
		case 2://属性2 array struct类型
			if( 	( (OI.w>=0x2000) && (OI.w<=0x200A) ) 
				|| 	( OI.w == 0x2029 ) 		//安时值
				|| 	( OI.w == 0x2014 ) 		//电表运行状态字
				|| 	( OI.w == 0x2025 )  	//事件当前值
                #if( PREPAY_MODE == PREPAY_LOCAL )
				||  ( OI.w == 0x202c )//钱包文件
				#endif
                  )
			{
				if( ClassIndex > VariableObj[VariableIndex].Type2[1])
				{
					return 0x8000;
				}
				
				Tag = VariableObj[VariableIndex].Type2;
				Tag += GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0	;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;		
			}	
			else if( OI.w <= 0x2041 )//普通数据类型
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag) ;
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;		
			}
			else if ((OI.w <= 0x2709)&&(OI.w >= 0x2700))
			{
				Lenth=Charging698AddTag(ClassAttribute,OI.w,InBuf,OutBuf,ClassIndex);
				return Lenth;
			}
			else if (OI.w==0x2051)
			{
				Lenth=0;
				OutBuf[Lenth++]=0x01;
				OutBuf[Lenth++]=0x03;
				for ( i = 0; i < 3; i++)
				{
					OutBuf[Lenth++]=0x02;
					OutBuf[Lenth++]=0x02;
					OutBuf[Lenth++]=0x09;
					OutBuf[Lenth++]=0x00;
					OutBuf[Lenth++]=0x04;
					OutBuf[Lenth++]=0x10;
					OutBuf[Lenth++]=0x00;
					OutBuf[Lenth++]=0x00;
				}
				return Lenth;
			}
			else//电压合格率 特殊处理
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				Tag = VariableObj[VariableIndex].Type2;
				
				Lenth = ((ClassIndex == 0) ? ((GetTypeLen( eTagData, Tag)*2)+2) : (GetTypeLen( eTagData, Tag)));
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Num = ((ClassIndex == 0) ? 2 : 1);
				i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
				Num = Num+i;
				if( ClassIndex == 0 )
				{
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 02;
				}
				
				for( ; i < Num; i++ )
				{
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					
					Lenth1 = GetTypeLen( eData, Tag);
					if( Lenth1 == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					OutBuf += Result;
					InBuf += Lenth1;

				}
				
				return Lenth;		
			}
						
			break;

		case 3://换算及单位
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( OutBufLen < 3 )//如果缓冲大小不够，则不进行操作
			{
				return 0;
			}
			
			Result = GetRequestAddTag( T_ScalerUnit, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			return 3;
			break;

		case 4://零线电流和电表跟随上报状态字
		case 6://零序电流
			if ((OI.w <= 0x2709)&&(OI.w >= 0x2701))
			{
				Lenth=Charging698AddTag(ClassAttribute,OI.w,InBuf,OutBuf,ClassIndex);
				return Lenth;
			}
			else if( (OI.w != 0x2001) && (OI.w != 0x2015) )
			{
				return 0x8000;
			}
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( OI.w == 0x2001 )
			{
                Tag = T_NI;
			}
			else if( ClassAttribute == 4)//电表跟随上报状态字 属性4类型同属性2
			{
               Tag = VariableObj[VariableIndex].Type2;
			}
			else
			{
				return 0x8000;
			}
			
			Lenth = GetTypeLen( eTagData, Tag) ;
            if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			
			return Lenth;		
			break;
		case 5://电表跟随上报字上报方式
			if( OI.w == 0x2015 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Tag = T_Enum;
				Lenth = GetTypeLen( eTagData, Tag) ;
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;	
			}else if ((OI.w <= 0x2709)&&(OI.w >= 0x2701))
			{
				Lenth=Charging698AddTag(ClassAttribute,OI.w,InBuf,OutBuf,ClassIndex);
				return Lenth;
			}
			break;
		case 7:
			if ((OI.w <= 0x2709)&&(OI.w >= 0x2701))
			{
				Lenth=Charging698AddTag(ClassAttribute,OI.w,InBuf,OutBuf,ClassIndex);
				return Lenth;
			}
			else
			{
				return 0x8000;
			}
			
		default:
			return 0x8000;
	}
    return 0x8000;
}

//--------------------------------------------------
//功能描述:  读取变量
//         
//参数:      DataType[in]：	eData/eTagData/eAddTag        
//           Dot[in] :		小数点        
//           *pOAD[in]：	OAD         
//           *InBuf[in]：	需要添加标签的数据         
//           OutBufLen[in]：留给OutBuf的数据长度         
//           *OutBuf[in]：	返回的电能数据
//         
//返回值:    返回变量长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  
//--------------------------------------------------
WORD GetRequestVariable( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE VariableIndex;
	BYTE Buf[32+30];//电压合格率源数据长度为32
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	Lenth = 0;
	
	VariableIndex = SearchVariableOAD( OI.w );
	if( VariableIndex == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType == eData) || (DataType == eTagData) )
	{
		Lenth = GetVariableData( Ch, Dot, pOAD, VariableIndex, Buf );
		if( Lenth == 0x8000 )
		{
			return Lenth;
		}
		if( DataType == eData )
		{
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			memcpy( OutBuf, Buf, Lenth );
		}
		else
		{
			Lenth = GetVariableAddTag(pOAD, VariableIndex, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		if( InBuf == NULL )
		{
			return 0x8000;
		}
		
		Lenth = GetVariableAddTag(pOAD, VariableIndex, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}
//--------------------------------------------------
//功能描述:  读取拓展模块变量
//         
//参数:      	DataType[in]：	eData/eTagData/eAddTag
//         	Dot[in]:		小数点(暂不用)        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:    返回变量长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  
//--------------------------------------------------
WORD GetModuleRequestVariable(BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
    BYTE len=0;
	BYTE GetCmd[] = {0x05, 0x01, 0x00};
	BYTE APDUbufs[200];

	if (DataType==eAddTag)
	{
		return 0x8000;
	}
    memcpy(APDUbufs,GetCmd,sizeof(GetCmd));
	memcpy((APDUbufs+3), pOAD, 4);
    APDUbufs[7]=0;
	len=api_RequestModule(APDUbufs,8,OutBufLen);//050100+OAD+时间标签00
	
	memcpy(OutBuf,APDUbufs+8,len);

	return len;

}

//--------------------------------------------------
//功能描述:  根据TAG获取变量长度
//         
//参数:      
//			DataType[in]：	eData/eTagData
//			*pOAD[in]：		OAD
//         
//返回值:   返回OAD对应数据的长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestVariableLen( BYTE DataType, BYTE* pOAD)
{
	BYTE ClassAttribute,VariableIndex,ClassIndex;
	TTwoByteUnion OI,Len;	
	const BYTE *Type;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0…bit4编码表示对象属性编号；
	ClassIndex = pOAD[3];
	VariableIndex = SearchVariableOAD( OI.w );
	if( VariableIndex == 0x80 )
	{
		return 0x8000;
	}
	
	Type = VariableObj[VariableIndex].Type2;
	
	switch( ClassAttribute )
	{
		case 2:
			if( (OI.w>=0x2131) && (OI.w<=0x2133) )//电压合格率
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				Len.w = GetTypeLen( DataType, Type );
				if( Len.w == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( DataType == eData )
				{
					Len.w = ((ClassIndex==0) ? (Len.w*2) : Len.w);
				}
				else
				{
					Len.w = ((ClassIndex==0) ? (Len.w*2+2) : Len.w);
				}				
			}
			else
			{
				if( 	((OI.w>=0x2000) && (OI.w<=0x200A)) 
					|| 	( OI.w == 0x2029 ) 
					|| 	( OI.w == 0x2014 ) 
					||	( OI.w == 0x2025 ) 
					#if( PREPAY_MODE == PREPAY_LOCAL )
					||  ( OI.w == 0x202c )//钱包文件
					#endif
					)
				{
					if( ClassIndex > Type[1] )
					{
						return 0x8000;
					}
					
					Type +=GetTagOffsetAddr( ClassIndex, Type );
				}
				else
				{
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
				}
				
				Len.w = GetTypeLen(DataType, Type);
			}				
			break;

		case 3://换算及单位
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( DataType == eData )//源数据长度
			{
				Len.w = 2;
			}
			else
			{
				Len.w = 3;
			}
			break;
		
		case 4://零线电流和电表运行状态字
		case 6://零序电流
			if( (OI.w != 0x2001) && (OI.w != 0x2015))
			{
				return 0x8000;
			}
			
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( OI.w == 0x2001 )
			{
                Len.w = GetTypeLen( DataType, T_NI);    
			}
			else if( ClassAttribute == 4 )//电表运行状态字
			{
               Len.w = GetTypeLen( DataType, Type);     
			}
			else
			{
				return 0x8000;
			}
				
			break;
		case 5://跟随上报状态字 上报方式	
			if( OI.w == 0x2015 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Type = T_Enum;
				Len.w = GetTypeLen( DataType, Type) ;
				if( Len.w == 0x8000 )
                {
                    return 0x8000;
                }
			}
			
			break;
		default:		
			return 0x8000;
	}

	return Len.w;
}

#endif//#if ( SEL_DLT698_2016_FUNC == YES)

