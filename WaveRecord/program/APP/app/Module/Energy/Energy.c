//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	刘骞
//创建日期	2016.8.5
//描述		电能模块源文件
//修改记录	
//---------------------------------------------------------------------- 

#include "AllHead.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define SECS_OF_ONE_HOUR	3600	// 1小时有多少秒（3600s）


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct TEnergySwapFlag_t
{
	WORD	wFlag;
	DWORD	CRC32;
}TEnergySwapFlag;

typedef enum
{
	eSWAP_ENERGY_ALL = 0,		// 总电能转存
	eSWAP_ENERGY_RATIO,			// 费率电能转存
	eSWAP_ENERGY_SEP,			// 分相电能转存
	eCLEAR_ENERGY_REMAIN		// 清脉冲尾数
}eSWAP_TYPE;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static BYTE	SwapEnergyFlag;							// 电能转存flag
static __no_init TEnergyRam EnergyRam;

static __no_init TEnergySwapFlag EnergySwapFlag[2];	// 总，费率电能转存成功标志
static __no_init TEnergyRom	EnergyRomBackup;        // eeprom电能在ram中的备份
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static BOOL EnergyCheck( BYTE Type );
static BOOL ReadCombReActiveEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *Energy );
static BOOL ReadCombActiveEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *Energy );
static BOOL SwapEnergySub( BYTE Type );

//-----------------------------------------------
//				函数定义
//-----------------------------------------------



//-----------------------------------------------
//函数功能: 清零电能基本单元
//
//参数: 	pDBase[in] 电能基本单元指针
// 			                  
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void ClearTDBase( TEnergyDBase *pDBase )
{
	memset( (void*)pDBase, 0, sizeof(TEnergyDBase) );
	lib_CheckSafeMemVerify( (BYTE *)(pDBase), sizeof(TEnergyDBase), REPAIR_CRC );
}


//-----------------------------------------------
//函数功能: 获取对应类型电能
//
//参数:
//		Type[in]
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
// 		p[in] 电能指针
//返回值:	返回对应类型电能
//
//备注:
//-----------------------------------------------
static DWORD GetDbaseValue(WORD Type, TEnergyDBase * p)
{
	WORD tType;
	DWORD Temp = 0;

	if( lib_CheckSafeMemVerify( (BYTE *)p, sizeof(TEnergyDBase), UN_REPAIR_CRC ) == FALSE )
	{
		return 0;
	}
	
	tType = Type&0x00ff;

	switch( tType )
	{
		case P_ACTIVE:
			Temp = p->PActive;
			break;

		case N_ACTIVE:
			Temp = p->NActive;
			break;

		#if(SEL_RACTIVE_ENERGY == YES)
		case RACTIVE1:
			Temp = p->RActive[0];
			break;

		case RACTIVE2:
			Temp = p->RActive[1];
			break;

		case RACTIVE3:
			Temp = p->RActive[2];
			break;

		case RACTIVE4:
			Temp = p->RActive[3];
			break;
		#endif

		#if( SEL_APPARENT_ENERGY == YES )// 视在（正向、反向）
		case APPARENT_P:
			Temp = p->Apparent[0];
			break;

		case APPARENT_N:
			Temp = p->Apparent[1];
			break;
		#endif
		
		default:
			Temp = p->PActive;
			break;

	}

	return Temp;
}


//-----------------------------------------------
//函数功能: 获取电能保存最小单位
//
//参数: 
// 			无
//                    
//返回值:	返回值N--代表ram及eeprom中保存的最小步长代表的电能脉冲个数
//
//备注: 若返回值为12，则代表电能存储最小补偿为12个脉冲  
//-----------------------------------------------
DWORD api_GetEnergyConst( void )
{
		
	#if( SYS_DBASE_ENERGY_DOT == 0xff ) //数据库电能按脉冲数存储
	
    return 1;
    
    #else
    
    DWORD dwEnergyConst,dwActiveConstant;//放上面有警告
    
		
	dwActiveConstant = api_GetActiveConstant();

	#if( SYS_DBASE_ENERGY_DOT == 1 )
	dwEnergyConst = (DWORD)(dwActiveConstant/10);					// 电能保留1位小数
	#elif( SYS_DBASE_ENERGY_DOT == 2 )
	dwEnergyConst = (DWORD)(dwActiveConstant/100);					// 电能保留2位小数
	#elif( SYS_DBASE_ENERGY_DOT == 3 )
	dwEnergyConst = (DWORD)(dwActiveConstant/1000);					// 电能保留3位小数
	#elif( SYS_DBASE_ENERGY_DOT == 4 )
	if( MeterTypesConst == METER_ZT )
	{
		//"直通表不能选择4位小数！！！"
		for(;;);
	}
	dwEnergyConst = (DWORD)(dwActiveConstant/10000);					// 电能保留4位小数
	#else
	dwEnergyConst = (DWORD)(dwActiveConstant/100);					// 电能保留2位小数
	#endif
	
	if( dwEnergyConst == 0 )
	{
		dwEnergyConst = 1;
	}

	return dwEnergyConst;
    
    #endif
}

//-----------------------------------------------
//函数功能: 写入电能
//
//参数:
//	Type[in]:
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	Value[in]:	脉冲个数
//
//返回值:	无
//
//备注:   计量调用
//-----------------------------------------------
void api_WritePulseEnergy( WORD Type, BYTE Value )
{
	WORD Phase,EnergyType;
	TEnergyDBase *pDBase;

	Phase = (Type>>12)&0x0007;
	EnergyType = Type&0x00FF;

	if( Phase >= (1+NUM_OF_SEPARATE_ENERGY) )
	{
		return;
	}
	
	if( Value == 0 )
	{
		return;
	}

	pDBase = &EnergyRam.Energy[Phase];
	//检查RAM电能
	if( lib_CheckSafeMemVerify( (BYTE *)pDBase, sizeof(TEnergyDBase), UN_REPAIR_CRC ) == FALSE )
	{
		ASSERT( 0, 1 );
		// 清除
		ClearTDBase(pDBase);
	}

	switch(EnergyType)
	{
		case P_ACTIVE:
			pDBase->PActive += Value;
			break;

		case N_ACTIVE:
			pDBase->NActive += Value;
			break;

		#if(SEL_RACTIVE_ENERGY == YES)
		case RACTIVE1:
		case RACTIVE2:
		case RACTIVE3:
		case RACTIVE4:
			pDBase->RActive[EnergyType-RACTIVE1] += Value;
			break;
		#endif

		#if( SEL_APPARENT_ENERGY == YES )
		// DLT645-2007要求的视在电能
		case APPARENT_P:
			pDBase->Apparent[0] += Value;
			break;

		case APPARENT_N:
			pDBase->Apparent[1] += Value;
			break;
		#endif

		default:
			break;
	}

	//计算校验
	lib_CheckSafeMemVerify( (BYTE *)pDBase, sizeof(TEnergyDBase), REPAIR_CRC );
}

//-----------------------------------------------
//函数功能: 读EEPROM中的电能
//
//参数:
//Type:		
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//
//Ratio		费率
//			1~MAX_RATIO		 其它个费率
//			0				 各费率总计
//
//Value		指向电能的指针
//
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:  结算日数据属于冻结数据，故此函数只支持当前电能
//-----------------------------------------------
static BOOL ReadEnergyRom(WORD Type, WORD Ratio, BYTE * Value)
{
	TEnergyDBase DbaseEnergy;
	WORD wAddr1,Phase;
	DWORD	dwTemp;// 12个月的指针，备份的指针，次数
	BYTE EnergyRamFlag;
	
	Phase = (Type >> 12)&0x0007;

	if( Ratio > MAX_RATIO )
	{
		return FALSE;
	}
	if( Phase >= (1+NUM_OF_SEPARATE_ENERGY) )
	{
		return FALSE;
	}
	
	//先判断RAM中保存底数的电能是否正确
	if( Ratio == 0 )
	{
		memcpy((BYTE *)&DbaseEnergy, (BYTE *)&EnergyRomBackup.Energy[Phase], sizeof(TEnergyDBase) );
	}
	else
	{
		memcpy((BYTE *)&DbaseEnergy, (BYTE *)&EnergyRomBackup.RatioEnergy[Ratio-1][Phase], sizeof(TEnergyDBase) );
	}
	
	if( lib_CheckSafeMemVerify( (BYTE *)&DbaseEnergy, sizeof(TEnergyDBase), UN_REPAIR_CRC ) == TRUE )
	{
		EnergyRamFlag = 0x55;
	}
	else
	{
		EnergyRamFlag = 0; 
	}

	// 若ram中保存的底数电能不正确，再读取EEPROM中的数据
	if( EnergyRamFlag != 0x55 )
	{
		if( Ratio == 0 )
		{
			// 总
			wAddr1 = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[Phase] );	
		}
		else
		{
			// 各费率
			wAddr1 = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.RatioEnergy[Ratio-1][Phase] );	
		}
	
		if( api_VReadSafeMem( wAddr1, sizeof(TEnergyDBase), (BYTE *)&DbaseEnergy ) != TRUE )
		{
			return FALSE;
		}
		
		//若ram中数据不对，从ee恢复后同步当前ram低度
		if( Ratio == 0 )
		{
			memcpy( (BYTE *)&EnergyRomBackup.Energy[Phase], (BYTE *)&DbaseEnergy, sizeof(TEnergyDBase) );
		}
		else
		{
			memcpy( (BYTE *)&EnergyRomBackup.RatioEnergy[Ratio - 1][Phase], (BYTE *)&DbaseEnergy, sizeof(TEnergyDBase) );
		}
	}

	dwTemp = GetDbaseValue( Type, &DbaseEnergy );
	memcpy( Value, (BYTE *)&dwTemp, sizeof(DWORD) );

	return TRUE;	
}

//-----------------------------------------------
//函数功能: 十六进制方式读电能
//
//参数:
//  Type:
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	Ratio[in]:
//			费率
//			1~MAX_RATIO		 各费率
//			0				 各费率总计
//	Dot[in]: 小数点位数
//			0: --	无小数点
//			1~N: --	1~N个小数点 支持6位小数的读取（电能尾数）
//	Energy[out]:指向电能的指针
//
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:	返回EEP+RAM中的脉冲电能，不包括组合有功，组合无功
//-----------------------------------------------
static BOOL ReadEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *Energy )
{	
	BYTE DotNum;
	WORD Phase;
	DWORD dwTemp;
	SQWORD tll;

	// 读EEPROM中电能
	if( ReadEnergyRom(Type, Ratio, (BYTE *)&dwTemp) == FALSE )
	{
		return FALSE;
	}
	
	Phase = (Type >> 12)&0x0007;

	tll = dwTemp;
	dwTemp = api_GetEnergyConst();
	tll *= dwTemp;//还原为脉冲数
	// RAM电能
	if( (Ratio==0) || (Ratio==api_GetCurRatio()) )
	{
		dwTemp = GetDbaseValue(Type, &(EnergyRam.Energy[Phase]));
		tll += dwTemp;
	}

	memcpy( (void*)Energy, (void*)&tll, sizeof(tll) );	
	
	DotNum = Dot;
	if( Dot != 6 )//电能尾数，小数为6位 因此支持6位电能的读取
	{
		if( Dot > 4 )
		{
			if( (Type&0x8000) == 0x8000 ) //高精度电能
			{
				DotNum = 4;
			}
			else
			{
				DotNum = 2;			// 默认2位小数
			}
		}
	}

	*Energy *= (long)lib_MyPow10(DotNum);
	*Energy /= (long)api_GetActiveConstant();

	return TRUE;
}
//-----------------------------------------------
//函数功能: 获取当前电能数据
//
//参数:
//	Type[in]:
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//	Ratio[in]:
//			费率
//			1~MAX_RATIO		 各费率
//			0				 各费率总计
//
//	Dot[in]: 小数点位数
//			0: --	无小数点
//			1~N: --	1~N个小数点 支持6位小数的读取（电能尾数）
//
//	Energy[out]:指向电能的指针
//			Hex补码表示，最高位为符号位
//			高精度电能返回 8字节，非高精度电能返回4字节
//
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_GetCurrEnergyData( WORD Type, BYTE DataType, WORD Ratio, BYTE Dot, BYTE * Energy)
{
	DWORD temp;
	WORD EnergyType;
	SQWORD tll;
	BYTE MinusFlag;
	BOOL Result;

	EnergyType = (Type&0xff);
	MinusFlag = 0;

	if( EnergyType == COMB_ACTIVE )
	{
		Result = ReadCombActiveEnergyHex( Type, Ratio, Dot, &tll );
	}
	#if(SEL_RACTIVE_ENERGY == YES)
	else if( (EnergyType == P_RACTIVE)||(EnergyType == N_RACTIVE) )
	{
		Result = ReadCombReActiveEnergyHex( Type, Ratio, Dot, &tll ); 
	}
	#endif
	else
	{
		Result = ReadEnergyHex(Type, Ratio, Dot, &tll );
	}

	if( Result == FALSE )
	{
		return FALSE;
	}

	if( Dot == 6 )
	{
		if( 	(EnergyType == COMB_ACTIVE)
		#if(SEL_RACTIVE_ENERGY == YES)
			||	(EnergyType == P_RACTIVE)
			||	(EnergyType == N_RACTIVE) 
		#endif
			)
		{
			if( tll < 0 )
			{
				tll *= -1;
				MinusFlag = 1;
			}

			Energy[0] = (tll%100);
			if( MinusFlag == 1 )
			{
				Energy[0] *= -1;
			}
			
		}
		else
		{
			Energy[0] = (tll%100);
		}

		return TRUE;
	}
	// 高精度电能为8字节有符号数，存于tll，不进行处理
	// 非高精度电能为4字节有符号数，多于4字节要进行取模处理
	if( (Type&0x8000) == 0X0000 )
	{
		if( 	(EnergyType == COMB_ACTIVE)
			#if(SEL_RACTIVE_ENERGY == YES)
			||	(EnergyType == P_RACTIVE)
			||	(EnergyType == N_RACTIVE) 
			#endif
			)
		{
			if( tll < 0 )
			{
				tll *= -1;
				MinusFlag = 1;
			}	
			
			temp = tll;

			if( DataType == DATA_BCD )
			{
			    tll %= (DWORD)(80000000);
				temp = lib_DWBinToBCD(tll);
				
				if( MinusFlag == 1 )
				{
					temp |= 0x80000000;
				}
			}
			else if( DataType == DATA_HEXCOMP )
			{
				if( MinusFlag == 1 )
				{
					temp =  ~temp+1;
				}
			}
			else if( DataType == DATA_HEX )
			{
				
			}
		}
		else
		{
			temp = tll;
			if( DataType == DATA_BCD )
			{
                tll %= (DWORD)(100000000);// 正值
				temp = lib_DWBinToBCD(tll);
			}	
		}

		memcpy( Energy, &temp, sizeof(DWORD) );
		
	}
	else
	{
		if( DataType == DATA_BCD )
		{
			if( 	(EnergyType == COMB_ACTIVE)
			#if(SEL_RACTIVE_ENERGY == YES)
			||	(EnergyType == P_RACTIVE)
			||	(EnergyType == N_RACTIVE) 
			#endif
			)
			{
				if( tll < 0 )
				{
					tll *= -1;
					MinusFlag = 1;
				}
				tll %= (QWORD)(8000000000);
			}
			tll = lib_QWBinToBCD(tll);
			
			if( MinusFlag == 1 )
			{
				tll |= (0x8000000000);
			}
			memcpy( Energy, &tll, 5 );//sizeof(tll) );
		}
		else if( DataType == DATA_HEX )
		{
			if( tll < 0 )
			{
				tll *= -1;
			}
			//temp = tll;
			memcpy( Energy, &tll, sizeof(tll) );
		}
		else if( DataType == DATA_HEXCOMP )
		{
			//temp = tll;
			memcpy( Energy, &tll, sizeof(tll) );
		}
		
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 读组合有功电能(不是脉冲数)
//
//参数:
//  Type:
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	Ratio[in]:
//			费率
//			1~MAX_RATIO		 各费率
//			0				 各费率总计
//	Dot[in]: 小数点位数
//			0: --	无小数点
//			1~N: --	1~N个小数点
//	Energy[out]:指向电能的指针
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:Dot需要考虑默认值0xFF的情况
//-----------------------------------------------
static BOOL ReadCombActiveEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *pEnergy )
{
	SQWORD llEnergy, llTemp;
	TTwoByteUnion tEnergyType;
	BYTE Flag,DotBak;

	//byActiveCalMode:有功组合方式特征字 组合有功
	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
	//保留	保留		保留	保留	反向有功	反向有功	正向有功	正向有功
	//									0不减1减	0不加1加	0不减1减	0不加1加

	Flag = 0;
	DotBak = Dot;
	
	if( Dot != 6 )//6位小数不处理 支持1、2、3、4
	{
		DotBak = 4;
		Flag = 0x55;
	}
	
	tEnergyType.w = Type;//顺序不能调整
	
	llEnergy = 0;
	if( FPara2->EnereyDemandMode.byActiveCalMode & 0x0c )
	{
		llTemp = 0;
		tEnergyType.b[0] = N_ACTIVE;
		//电能组合前 Dot按照实际位数处理
		if( ReadEnergyHex( tEnergyType.w, Ratio, DotBak, &llTemp ) == FALSE )
		{			
			return FALSE;
		}
		
		if( FPara2->EnereyDemandMode.byActiveCalMode & 0x08 )
		{
			llEnergy -= llTemp;
		}
		if( FPara2->EnereyDemandMode.byActiveCalMode & 0x04 )
		{
			llEnergy += llTemp;
		}
	}
	if( FPara2->EnereyDemandMode.byActiveCalMode & 0x03 )
	{
		llTemp = 0;
		tEnergyType.b[0] = P_ACTIVE;
		if( ReadEnergyHex( tEnergyType.w, Ratio, DotBak, &llTemp ) == FALSE )
		{
			return FALSE;
		}
		
		if( FPara2->EnereyDemandMode.byActiveCalMode & 0x02 )
		{
			llEnergy -= llTemp;
		}
		if( FPara2->EnereyDemandMode.byActiveCalMode & 0x01 )
		{
			llEnergy += llTemp;
		}
	}
	//电能位数处理
	if( Flag == 0x55 )
	{
		if( Dot != 4 )//4位小数无需处理 6位小数前面判断了
		{
			//默认值处理
			if(Dot == 0xFF)
			{
				if( (Type&0x8000) == 0X0000 )
				{
					Dot = 2;
				}
				else
				{
					Dot = 4;
				}
			}
			if( Dot > 4 )
			{
				Dot = 2;
			}
			//数据按4位读取，这里把多余的小数点数处理掉
			llEnergy = (llEnergy/(long)lib_MyPow10(DotBak-Dot));
		}
	}

	*pEnergy = llEnergy;

	return TRUE;
}

//-----------------------------------------------
//函数功能: 读组合无功电能(不是脉冲个数)
//
//参数:
//  Type:
//			D15--	高精度电能 				8000
//			D14-D12：PHASE_ALL -- 总电能 	0000
//					PHASE_A -- A相电能  	1000
//					PHASE_B -- B相电能  	2000
//					PHASE_C -- C相电能  	3000
// 					PHASE_N -- N线电能		4000
//			D11-D8：暂时用不到
//					常规电能	--		0000
//					基波电能	--		0100
//					谐波电能	--		0200
//			D7-D0：	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	Ratio[in]:
//			费率
//			1~MAX_RATIO		 各费率
//			0				 各费率总计
//	Dot[in]: 小数点位数
//			0: --	无小数点
//			1~N: --	1~N个小数点
//	Energy[out]:指向电能的指针
//
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:Dot需要考虑默认值0xFF的情况
//-----------------------------------------------
#if(SEL_RACTIVE_ENERGY == YES)
static BOOL ReadCombReActiveEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *pEnergy )
{
	SQWORD llEnergy,llTemp;
	TTwoByteUnion tEnergyType;
	BYTE i,Mode,Flag,DotBak;

	llEnergy = 0;
	Flag = 0;

	DotBak = Dot;

	if( Dot != 6 )//6位小数不处理 支持1、2、3、4
	{
		DotBak = 4;
		Flag = 0x55;
	}

	tEnergyType.w = Type;//顺序不能调整
	
	for( i = 0; i < 4; i++ )
	{
		//判断是无功1还是无功2
		if( (Type&0x00ff) == P_RACTIVE )
		{
			Mode = (FPara2->EnereyDemandMode.PReactiveMode>>(2*i))&0x03;
		}
		else
		{
			Mode = (FPara2->EnereyDemandMode.NReactiveMode>>(2*i))&0x03;
		}
		
		tEnergyType.b[0] = RACTIVE1 + i;
		if( ReadEnergyHex( tEnergyType.w, Ratio, DotBak, &llTemp ) == FALSE )
		{
			return FALSE;
		}

		//值是1加 2减 0和3不要
		if( Mode == 0x01 )
		{
			llEnergy += llTemp;
		}
		else if( Mode == 0x02 )
		{
			llEnergy -= llTemp;
		}
	}

	//电能位数处理
	if( Flag == 0x55 )
	{
		if( Dot != 4 )//4位小数无需处理 6位小数前面判断了
		{
			//默认值处理
			if(Dot == 0xFF)
			{
				if( (Type&0x8000) == 0X0000 )
				{
					Dot = 2;
				}
				else
				{
					Dot = 4;
				}
			}
			if( Dot > 4 )
			{
				Dot = 2;
			}
			
			llEnergy = (llEnergy/(long)lib_MyPow10(DotBak-Dot));
		}
	}

	*pEnergy = llEnergy;

	return TRUE;
}
#endif

//-----------------------------------------------
//函数功能: 清当前电能
//
//参数: 
//	无
//                    
//返回值:
//  无
//
//备注:   
//-----------------------------------------------
void api_ClrEnergy( void )
{
	BYTE i, j;
	WORD wAddr;
	TDEnergyAccuBase	EnergyAccu;

	CLEAR_WATCH_DOG;

	//清RAM 总+A,B,C
	for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
	{
		ClearTDBase( &(EnergyRam.Energy[i]) );		
	}

	api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
	//清ram中保存的电能低度	
	memset((BYTE *)&EnergyRomBackup.Energy[0], 0x00, sizeof(TEnergyDBase) );
	EnergyRomBackup.Energy[0].CRC32 = lib_CheckCRC32((BYTE *)&EnergyRomBackup.Energy[0], sizeof(TEnergyDBase)-sizeof(DWORD));
	
	for( i = 1; i < (1 + NUM_OF_SEPARATE_ENERGY); i++ )
	{
		memcpy((BYTE *)&EnergyRomBackup.Energy[i], (BYTE *)&EnergyRomBackup.Energy[0], sizeof(TEnergyDBase) );
	}
	for( i = 0; i < MAX_RATIO; i++ )
	{
		for( j = 0; j < (1+NUM_OF_SEPARATE_ENERGY_RATIO); j++ )
		{
			memcpy((BYTE *)&EnergyRomBackup.RatioEnergy[i][j], (BYTE *)&EnergyRomBackup.Energy[0], sizeof(TEnergyDBase) );
		}
	}
	
	//清当前EEP总，A，B，C
	for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
	{
		wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[i] );
		api_ClrSafeMem( wAddr, sizeof(TEnergyDBase) );	
	}

	//清当前EEP费率电能
	for( i = 0; i < MAX_RATIO; i++ )
	{
		for( j = 0; j < (1+NUM_OF_SEPARATE_ENERGY_RATIO); j++ )
		{
			wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.RatioEnergy[i][j] );
			api_ClrSafeMem( wAddr, sizeof(TEnergyDBase) );
		}
	}
	
	//清总,A,B,C余数
	for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
	{
		wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Remain.RemainEnergy[i] );
		api_ClrSafeMem( wAddr, sizeof(TEnergyDBase) );		
	}

	// 清月累计电能
	EnergyAccu.PActive = 0;
	EnergyAccu.NActive = 0;

	for( i = 0; i < 2; i++ )
	{
		wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[i] );
		api_VWriteSafeMem( wAddr, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu );
	}
	
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
}

//-----------------------------------------------
//函数功能: 电能监视
//
//参数: 
//	pDW[in]:	
//			指向被检查电能的指针
//	Type[in]:	
//		0 	检查电能数据，如果大于某个值则进行转存（每分钟检查）
//	 	1 	检查RAM电能数据，如果大于某个值则清零（初始化时检查）
//		2	检查EEP电能余数，如果大于某个值则清零（初始化时检查）
//
//返回值:	无
//
//备注: 
//如果内存中的电能大于额定电压，最大电流，功率因数等于1时走4小时（单相是12个小时）的电能
//则需要进行转存 直通表按220v 500安考虑 
//-----------------------------------------------
static BOOL WatchEnergy( DWORD *pDW, BYTE Type )
{
	DWORD dwEnergy,dwEnergyConst,dwActiveConstant;
	BYTE i,Status;

	dwActiveConstant = api_GetActiveConstant();
	
	dwEnergyConst = api_GetEnergyConst();

	if( Type == 0 )
	{
		if( MeterTypesConst == METER_ZT )
		{
			dwEnergy = (DWORD)1320*dwActiveConstant;//三相：220V * 500A /1000KW/W * 4h * 3Phase * 200ConstantConst = 264000 
		}
		else
		{
			dwEnergy = (DWORD)26*dwActiveConstant;
		}
	}
	else if( Type == 1 )
	{
		if( MeterTypesConst == METER_ZT )
		{
			dwEnergy = (DWORD)(1320*2)*dwActiveConstant;
		}
		else
		{
			dwEnergy = (DWORD)(26*2)*dwActiveConstant;
		}
	}
	else
	{
		dwEnergy = dwEnergyConst + dwEnergyConst/2;
	}

	Status = TRUE;
     
    for(i=0; i<( (sizeof(TEnergyDBase)-sizeof(DWORD))/sizeof(DWORD) ); i++)
	{
		if( pDW[i] > dwEnergy )
		{
			Status = FALSE;
			break;
		}
	}
	
	return Status;
}

//-----------------------------------------------
//函数功能: 上电电能处理 
//
//参数:  
//			无
//
//返回值:	无
//
//备注: 校验和取值范围对的情况，补转存/转存
// 		在此转存是为了防止未到转存时间点就复位的情况
//-----------------------------------------------
void api_PowerUpEnergy( void )
{
	BOOL Result;
	BYTE i,j;
	WORD Addr;

	SwapEnergyFlag = 0;
	
	Result = EnergyCheck( 1 );
	
	if( Result != FALSE )
	{
		if( (lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[0], sizeof(TEnergySwapFlag), UN_REPAIR_CRC ) != FALSE )&&
			(lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[1], sizeof(TEnergySwapFlag), UN_REPAIR_CRC ) != FALSE ))
		{
			if( (EnergySwapFlag[0].wFlag == 0xA5A5)&&(EnergySwapFlag[1].wFlag == 0x5A5A) )
			{
				// 总未成功
				SwapEnergySub( eSWAP_ENERGY_ALL );
				SwapEnergySub( eCLEAR_ENERGY_REMAIN );
				EnergySwapFlag[0].wFlag = 0x5A5A;
				lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[0], sizeof(TEnergySwapFlag), REPAIR_CRC );
			}
			else if( (EnergySwapFlag[1].wFlag == 0xA5A5)&&(EnergySwapFlag[0].wFlag == 0x5A5A) )
			{
				// 费率未成功
				SwapEnergySub( eSWAP_ENERGY_RATIO );
				SwapEnergySub( eCLEAR_ENERGY_REMAIN );
				EnergySwapFlag[1].wFlag = 0x5A5A;
				lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[1], sizeof(TEnergySwapFlag), REPAIR_CRC );
			}
			else//如果两个都存成功或者两个都不成功了 也要再存一次 防止电表复位一直没有转存
			{
				api_SwapEnergy();
			}
		}
		else
		{
			api_SwapEnergy();
		}
	}
	
	//电能低度首先信任ee中的数据
	//因此上电的时候将所有电能的校验都判断一下，只要有一个错误就认为错误，所有数据都从ee中恢复
	for( i = 0; i < sizeof(TEnergyRom) / sizeof(TEnergyDBase); i++ )
	{
		if( lib_CheckSafeMemVerify( (BYTE *)&EnergyRomBackup + i * sizeof(TEnergyDBase), sizeof(TEnergyDBase), UN_REPAIR_CRC ) != TRUE )
		{
			break;
		}
	}

	Result = TRUE;
	
	if( i < sizeof(TEnergyRom) / sizeof(TEnergyDBase) )
	{
		for( j = 0; j < sizeof(TEnergyRom) / sizeof(TEnergyDBase); j++ )
		{
			Addr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[0] ) + j * sizeof(TEnergyDBase);
			if( api_VReadSafeMem( Addr, sizeof(TEnergyDBase), (BYTE *)&EnergyRomBackup + j * sizeof(TEnergyDBase) ) != TRUE )
			{
				Result = FALSE;
			}
		}
	}

}

//-----------------------------------------------
//函数功能: 掉电电能处理 
//
//参数:  无
//
//返回值:	无
//
//备注:
//-----------------------------------------------
void api_PowerDownEnergy( void )
{
	api_SwapEnergy();
}

//-----------------------------------------------
//函数功能: 电能处理任务 
//
//参数:  无
//
//返回值:	无
//
//备注:	大循环调用
//-----------------------------------------------
void api_EnergyTask( void )
{
	BYTE i;
	TRealTimer t;

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );//获取当前时间
	
	// 秒变化
	if( api_GetTaskFlag(eTASK_ENERGY_ID,eFLAG_SECOND) == TRUE )
	{
	    api_ClrTaskFlag(eTASK_ENERGY_ID,eFLAG_SECOND);
	    FunctionConst(ENERGY_TASK1);
		
		//分钟任务
		if( t.Sec == (eTASK_ENERGY_ID*3+3+0) )
		{			
			for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
			{
				if( WatchEnergy( (DWORD *)&EnergyRam.Energy[i], 0 ) == FALSE )//只要有一个满足转存条件，就转存
				{
					api_SwapEnergy();//校验在这可以不判断，因为SwapEnergy会判断
					api_WriteSysUNMsg(WATCH_ENERGY_SWAP_ERR);
					break;
				}
			}
		}
	}

	//处理转存业务，第一位为小时转存
	if( t.Min == 0 )
	{
	    FunctionConst(ENERGY_TASK2);
		if( SwapEnergyFlag == 0 )
		{
			if( t.Sec > 28 )
			{
				api_SwapEnergy();
				#if( PREPAY_MODE == PREPAY_LOCAL )
				api_SwapMoney(); 
				#endif
				SwapEnergyFlag = 1;
			}
		}
	}
	else
	{
		SwapEnergyFlag = 0;
	}
}

//-----------------------------------------------
//函数功能: 检查RAM和EEP中电能 
//
//参数:  Type:  0: 检查不对直接清0
// 				1：检查不对，从eeprom中恢复尾数电能
//
//返回值:	TRUE: 总和分相校验，取值范围都对
// 			FALSE: 校验错/取值范围错
//
//备注:
//-----------------------------------------------
static BOOL EnergyCheck( BYTE Type )
{
	WORD wAddr;
	BYTE i,flag;

	flag = 0;
	for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
	{
		if( lib_CheckSafeMemVerify( (BYTE *)&EnergyRam.Energy[i], sizeof(TEnergyDBase), UN_REPAIR_CRC ) == FALSE )
		{
			if( Type == 0 )
			{
				ClearTDBase(&EnergyRam.Energy[i]);
                // api_WriteSysUNMsg( ENERGY_CHECK_RAM_ERR );//无抄表电池暂不移出此if 
			}
			else
			{
				// 从EEP恢复电能尾数
				wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Remain.RemainEnergy[i] );
				if( api_VReadSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&EnergyRam.Energy[i] ) == FALSE )
				{
					ClearTDBase(&EnergyRam.Energy[i]);
				}
				//检查EEP电能尾数
				if( WatchEnergy( (DWORD *)&(EnergyRam.Energy[i]), 2 ) == FALSE )
				{
					ClearTDBase(&EnergyRam.Energy[i]);
				}
			}
		}
		else
		{
			//检查RAM电能
			if( WatchEnergy( (DWORD *)&(EnergyRam.Energy[i]), 1 ) == FALSE )
			{
				ClearTDBase(&EnergyRam.Energy[i]);
				api_WriteSysUNMsg(WATCH_ENERGY_OVERRIDE_ERR);
			}
			else
			{
				flag++;	// 电能的校验和取值范围是正确的
			}
		}
	}

	if( flag == (1+NUM_OF_SEPARATE_ENERGY) )	
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
//函数功能: 电能转存子函数
//
//参数:  Type[in]:  eSWAP_TYPE
//
//返回值:	TRUE: 转存正确
// 			FALSE: 转存错误
//
//备注:	
//-----------------------------------------------
static BOOL SwapEnergySub( BYTE Type )
{
	TEnergyDBase	RamEnergy[1+NUM_OF_SEPARATE_ENERGY],RomEnergy;
	DWORD	dwEnergyConst;
	DWORD	*pTemp;
	DWORD	*pRom;
	DWORD	*pRam;
	WORD	wAddr;
	BYTE 	i,j;
	BYTE 	CurrRatio;

	dwEnergyConst = api_GetEnergyConst();
	CurrRatio = api_GetCurRatio();

	// 将脉冲数转化为电能
	memcpy( (BYTE *)&RamEnergy[0], (BYTE *)&EnergyRam.Energy[0], (sizeof(TEnergyDBase)*(1+NUM_OF_SEPARATE_ENERGY)) );
	for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
	{
		pTemp = (DWORD *)&RamEnergy[i];
		for( j = 0; j < ( (sizeof(TEnergyDBase)-sizeof(DWORD))/sizeof(DWORD) ); j++ )
		{
			pTemp[j] /= dwEnergyConst;
		}
	}

	if( Type == eSWAP_ENERGY_ALL )
	{
		pTemp = (DWORD *)&RamEnergy[0];
		pRom = (DWORD *)&RomEnergy;
		
		//判断总增量电能是否都为0，若为0，不叠加，直接返回TRUE
		for( j = 0; j < ((sizeof(TEnergyDBase) - sizeof(DWORD)) / sizeof(DWORD)); j++ )
		{
			if( pTemp[j] != 0 )
			{
				break;
			}
		}
		if( j == ((sizeof(TEnergyDBase) - sizeof(DWORD)) / sizeof(DWORD)) )
		{
			return TRUE;
		}

		wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[0] );
		if( api_VReadSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&RomEnergy ) == FALSE )
		{
			return FALSE;
		}
		
		for( j = 0; j < ( (sizeof(TEnergyDBase)-sizeof(DWORD))/sizeof(DWORD) ); j++ )
		{
			pRom[j] += pTemp[j];
		}

		api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
		api_VWriteSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&RomEnergy );
		api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
		
		//同步RAM中保存电能低度
		memcpy( (BYTE *)&EnergyRomBackup.Energy[0], (BYTE *)&RomEnergy, sizeof(TEnergyDBase) );
	}
	else if( Type == eSWAP_ENERGY_SEP )
	{
	    #if( NUM_OF_SEPARATE_ENERGY != 0 )
		for( i = 1; i <= NUM_OF_SEPARATE_ENERGY; i++ )
		{
			pTemp = (DWORD *)&RamEnergy[i];
			pRom = (DWORD *)&RomEnergy;

			//判断分相增量电能是否都为0，若为0，不叠加
			for( j = 0; j < ((sizeof(TEnergyDBase) - sizeof(DWORD)) / sizeof(DWORD)); j++ )
			{
				if( pTemp[j] != 0 )
				{
					break;
				}
			}
			if( j == ((sizeof(TEnergyDBase) - sizeof(DWORD)) / sizeof(DWORD)) )
			{
				continue;
			}

			wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[i] );
			if( api_VReadSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&RomEnergy ) == FALSE )
			{
				return FALSE;
			}

			
			for( j = 0; j < ( (sizeof(TEnergyDBase)-sizeof(DWORD))/sizeof(DWORD) ); j++ )
			{
				pRom[j] += pTemp[j];
			}
			
			api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
			api_VWriteSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&RomEnergy );
			api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
			
			//同步RAM中保存电能低度
			memcpy( (BYTE *)&EnergyRomBackup.Energy[i], (BYTE *)&RomEnergy, sizeof(TEnergyDBase) );
		}
		#endif
	}
	else if( Type == eSWAP_ENERGY_RATIO )
	{
		for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY_RATIO); i++ )
		{
			//一定要判断费率 如果费率不对会把别的电能给冲掉
			if( (CurrRatio == 0)||(CurrRatio > MAX_RATIO) )
			{
				CurrRatio = DefCurrRatioConst;//缺省认为是平时段
			}
			
			pTemp = (DWORD *)&RamEnergy[i];
			pRom = (DWORD *)&RomEnergy;

			//判断费率增量电能是否都为0，若为0，不叠加，
			for( j = 0; j < ((sizeof(TEnergyDBase) - sizeof(DWORD)) / sizeof(DWORD)); j++ )
			{
				if( pTemp[j] != 0 )
				{
					break;
				}
			}
			if( j == ((sizeof(TEnergyDBase) - sizeof(DWORD)) / sizeof(DWORD)) )
			{
				continue;
			}
			
			wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.RatioEnergy[CurrRatio - 1][i] );
			if( api_VReadSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&RomEnergy ) == FALSE )
			{
				return FALSE;
			}

			for( j = 0; j < ( (sizeof(TEnergyDBase)-sizeof(DWORD))/sizeof(DWORD) ); j++ )
			{
				pRom[j] += pTemp[j];
			}

			api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
			api_VWriteSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&RomEnergy );
			api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
			
			//同步RAM中保存电能低度
			memcpy( (BYTE *)&EnergyRomBackup.RatioEnergy[CurrRatio-1][i], (BYTE *)&RomEnergy, sizeof(TEnergyDBase) );
		}
	}
	else
	{
		// 更新RAM尾数，此处不能判断pTemp为0返回
		for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
		{
			pRam = (DWORD *)&EnergyRam.Energy[i];
			pTemp = (DWORD *)&RamEnergy[i];
			for( j = 0; j < ( (sizeof(TEnergyDBase)-sizeof(DWORD))/sizeof(DWORD) ); j++ )
			{
				pRam[j] -= pTemp[j]*dwEnergyConst;
			}
			lib_CheckSafeMemVerify( (BYTE *)&EnergyRam.Energy[i], sizeof(TEnergyDBase), REPAIR_CRC );	//计算CRC校验

			// 尾数存EEP
			wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Remain.RemainEnergy[i] );

			api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
			api_VWriteSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&EnergyRam.Energy[i] );
			api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
		}
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 电能转存 
//
//参数:  无
//
//返回值:	无
//
//备注:	将RAM电能转存到EEP
//-----------------------------------------------
void api_SwapEnergy( void )
{
	// 检查RAM电能
	EnergyCheck( 0 );
	
	// 置未成功标志
	EnergySwapFlag[0].wFlag= 0xA5A5;
	lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[0], sizeof(TEnergySwapFlag), REPAIR_CRC );
	EnergySwapFlag[1].wFlag= 0xA5A5;
	lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[1], sizeof(TEnergySwapFlag), REPAIR_CRC );

	// 转存总电能
	if( SwapEnergySub( eSWAP_ENERGY_ALL ) != FALSE )
	{
		EnergySwapFlag[0].wFlag= 0x5A5A;
		lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[0], sizeof(TEnergySwapFlag), REPAIR_CRC );
	}
	
	// 转存费率电能
	if( SwapEnergySub( eSWAP_ENERGY_RATIO ) != FALSE )
	{
		EnergySwapFlag[1].wFlag= 0x5A5A;
		lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[1], sizeof(TEnergySwapFlag), REPAIR_CRC );
	}

	// 转存分相电能
	SwapEnergySub( eSWAP_ENERGY_SEP );

	if( (EnergySwapFlag[0].wFlag == 0xA5A5)&&(EnergySwapFlag[1].wFlag == 0xA5A5) )
	{
		return;	// 等待下次转存
	}
	else if( (EnergySwapFlag[0].wFlag == 0x5A5A)&&(EnergySwapFlag[1].wFlag == 0x5A5A) )
	{
		SwapEnergySub(eCLEAR_ENERGY_REMAIN);
	}
	else if( EnergySwapFlag[0].wFlag == 0xA5A5 )
	{
		SwapEnergySub( eSWAP_ENERGY_ALL );
		SwapEnergySub( eCLEAR_ENERGY_REMAIN );
		EnergySwapFlag[0].wFlag = 0x5A5A;
		lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[0], sizeof(TEnergySwapFlag), REPAIR_CRC );
	}
	else
	{
		SwapEnergySub( eSWAP_ENERGY_RATIO );
		SwapEnergySub( eCLEAR_ENERGY_REMAIN );
		EnergySwapFlag[1].wFlag = 0x5A5A;
		lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[1], sizeof(TEnergySwapFlag), REPAIR_CRC );
	}
}

//-----------------------------------------------
//函数功能: 月结算电能，用于月累计电能计算
//
//参数:  	无
//	
//返回值:	无
//
//备注:保存脉冲个数	
//-----------------------------------------------
void api_ClosingMonEnergy( void )
{
	WORD				wAddr1,wAddr2;
	TDEnergyAccuBase	EnergyAccu;

	//将上1转存到上2
	wAddr1 = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[0] );
	if( api_VReadSafeMem( wAddr1, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu ) == FALSE )
	{
		return;
	}

	wAddr2 = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[1] );
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
	api_VWriteSafeMem( wAddr2, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu );
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);

	//将当前的转存到上1		（eeprom+尾数）8个F按10万脉冲常数可走42949kWh电，够用了
	//读电能方式，不能读脉冲个数。否则会产生组合与正反向电能差0.01情况
	api_GetCurrEnergyData( PHASE_ALL+P_ACTIVE, DATA_HEX, 0, 2, (BYTE*)&EnergyAccu.PActive );
	api_GetCurrEnergyData( PHASE_ALL+N_ACTIVE, DATA_HEX, 0, 2, (BYTE*)&EnergyAccu.NActive );
	EnergyAccu.CRC32 = lib_CheckCRC32( (BYTE *)&EnergyAccu, (sizeof(TDEnergyAccuBase)-sizeof(DWORD)) );

	api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
	api_VWriteSafeMem( wAddr1, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu );
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
}

//-----------------------------------------------
//函数功能: 获取累计用电量（月结算）
//
//参数:  
//	No[in]:
//			0 当前月
//			1 上月
//
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
// 
//	Dot[in]: 小数点位数 最多支持2位小数
//			0: --	无小数点
//			1~N: --	1~N个小数点
//
//	pEnergy[in]:
//			指向电能的指针
// 			Hex补码表示，最高位为符号位 %0x80000000
//
//返回值:	TRUE   正确
//			FALSE  错误
//
//备注:	
//-----------------------------------------------
BOOL api_GetAccuEnergyData( BYTE No, BYTE DataType, BYTE Dot, BYTE *pEnergy )
{
	TDEnergyAccuBase EnergyAccu;
	DWORD	CurPActive,CurNActive,Energy;
	DWORD	dwTmp,dwAddr;

	//判断Dot是否正确
	if( Dot > 2 )
	{
		Dot = 2;
	}
	
	if( No == 0 ) // 当前月度
	{
		//读电能方式，不能读脉冲个数。否则会产生组合与正反向电能差0.01情况
		//读取数据按照最大位数读取 必须先组合 再取小数位 否则数据计算错误
		api_GetCurrEnergyData( PHASE_ALL+P_ACTIVE, DATA_HEX, 0, 2, (BYTE*)&CurPActive );
		api_GetCurrEnergyData( PHASE_ALL+N_ACTIVE, DATA_HEX, 0, 2, (BYTE*)&CurNActive );

		dwAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[0] );
		if( api_VReadSafeMem( dwAddr, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu ) == FALSE )
		{
			return FALSE;
		}
	}
	else if( No == 1 )// 上1月度
	{
		dwAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[0] );
		if( api_VReadSafeMem( dwAddr, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu ) == FALSE )
		{
			return FALSE;
		}

		CurPActive = EnergyAccu.PActive;
		CurNActive = EnergyAccu.NActive;
		
		dwAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[1] );
		if( api_VReadSafeMem( dwAddr, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu ) == FALSE )
		{
			return FALSE;
		}			
	}
	else
	{
		return FALSE;
	}
	

	CurPActive -= EnergyAccu.PActive;
	CurNActive -= EnergyAccu.NActive;

	//byActiveCalMode:有功组合方式特征字 组合有功
	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
	//保留	保留		保留	保留	反向有功	反向有功	正向有功	正向有功
	//									0不减1减	0不加1加	0不减1减	0不加1加

	Energy = 0;
	if( FPara2->EnereyDemandMode.byActiveCalMode & 0x0c )
	{
		//有功组合方式特征字 4112 的 bit1 和 bit3 标识为“1 减” 时其代表的电量不参与运算。
		//if( FPara2->EnereyDemandMode.byActiveCalMode & 0x08 )
		//{
		//	Energy -= CurNActive;
		//}
		if( FPara2->EnereyDemandMode.byActiveCalMode & 0x04 )
		{
			Energy += CurNActive;
		}
	}
	if( FPara2->EnereyDemandMode.byActiveCalMode & 0x03 )
	{
		//if( FPara2->EnereyDemandMode.byActiveCalMode & 0x02 )
		//{
		//	Energy -= CurPActive;
		//}
		if( FPara2->EnereyDemandMode.byActiveCalMode & 0x01 )
		{
			Energy += CurPActive;
		}
	}

	//舍掉符号位 正常不会有符号位
	dwTmp = Energy % 0x80000000;
	//保留小数位数
	dwTmp = dwTmp / lib_MyPow10( 2 - Dot );
	
	if( DataType == DATA_BCD )
	{
		dwTmp = Energy % 80000000L; 
		dwTmp = lib_DWBinToBCD( dwTmp );
	}
	
	memcpy( pEnergy, (void*)&dwTmp, sizeof(DWORD) );

	return TRUE;
}

#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//函数功能: 读累计用电量BCD-645规约（结算周期）
//
//参数:
//	No[in]:
//			0 当前周期
//			1 上一周期
//
//	DataType[in]	DATA_BCD/DATA_HEX(原码格式)/DATA_HEXCOMP(补码格式)
//
//	Dot[in]: 小数点位数 最多支持2位小数
//			0: --	无小数点
//			1~N: --	1~N个小数点
//
//	pEnergy[in]:
//			指向电能的指针
// 			返回hex
//
//返回值:	TRUE   正确
//		  FALSE  错误
//
//备注:
//-----------------------------------------------
BOOL api_GetAccuPeriEnergy( BYTE No, BYTE DataType, BYTE Dot, BYTE *pEnergy )
{
	DWORD CurPActive, CurNActive, LastPActive, LastNActive, Energy, dwTmp;
	DWORD length;
	TFourByteUnion DataId, EnergData;
	DataId.l = 0;
	EnergData.l = 0;
	
	return FALSE;
// 	//判断Dot是否正确
// 	if( Dot > 2 )
// 	{
// 		Dot = 2;
// 	}

// 	if( No == 0 ) // 当前结算周期
// 	{
// 		//读电能方式，不能读脉冲个数。否则会产生组合与正反向电能差0.01情况
// 		api_GetCurrEnergyData( PHASE_ALL + P_ACTIVE, DATA_HEX, 0, 2, (BYTE *)&CurPActive );
// 		api_GetCurrEnergyData( PHASE_ALL + N_ACTIVE, DATA_HEX, 0, 2, (BYTE *)&CurNActive );

// 		DataId.l = 0x00010001;
// 		length = api_ReadClosingRecord645( DataId.b, (BYTE *)EnergData.b );
// 		if( (length == 0) || (length == 0x8000) )
// 		{
// 			return FALSE;
// 		}
// 		LastPActive = lib_DWBCDToBin( EnergData.d );

// 		DataId.l = 0x00020001;
// 		length = api_ReadClosingRecord645( DataId.b, (BYTE *)EnergData.b );
// 		if( (length == 0) || (length == 0x8000) )
// 		{
// 			return FALSE;
// 		}
// 		LastNActive = lib_DWBCDToBin( EnergData.d );
// 	}
// 	else if( No == 1 ) // 上1结算周期
// 	{
// 		DataId.l = 0x00010001;
// 		length = api_ReadClosingRecord645( DataId.b, (BYTE *)EnergData.b );
// 		if( (length == 0) || (length == 0x8000) )
// 		{
// 			return FALSE;
// 		}
// 		CurPActive = lib_DWBCDToBin( EnergData.d );

// 		DataId.l = 0x00020001;
// 		length = api_ReadClosingRecord645( DataId.b, (BYTE *)EnergData.b );
// 		if( (length == 0) || (length == 0x8000) )
// 		{
// 			return FALSE;
// 		}
// 		CurNActive = lib_DWBCDToBin( EnergData.d );

// 		DataId.l = 0x00010002;
// 		length = api_ReadClosingRecord645( DataId.b, (BYTE *)EnergData.b );
// 		if( (length == 0) || (length == 0x8000) )
// 		{
// 			return FALSE;
// 		}
// 		LastPActive = lib_DWBCDToBin( EnergData.d );

// 		DataId.l = 0x00020002;
// 		length = api_ReadClosingRecord645( DataId.b, (BYTE *)EnergData.b );
// 		if( (length == 0) || (length == 0x8000) )
// 		{
// 			return FALSE;
// 		}
// 		LastNActive = lib_DWBCDToBin( EnergData.d );
// 	}
// 	else
// 	{
// 		return FALSE;
// 	}

// 	CurPActive -= LastPActive;
// 	CurNActive -= LastNActive;

// 	//byActiveCalMode:有功组合方式特征字 组合有功
// 	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
// 	//保留	保留		保留	保留	反向有功	反向有功	正向有功	正向有功
// 	//									0不减1减	0不加1加	0不减1减	0不加1加

// 	Energy = 0;
// 	if( FPara2->EnereyDemandMode.byActiveCalMode&0x0c )
// 	{
// 		//有功组合方式特征字 4112 的 bit1 和 bit3 标识为“1 减” 时其代表的电量不参与运算。
// //		if( FPara2->EnereyDemandMode.byActiveCalMode & 0x08 )
// //		{
// //			Energy -= CurNActive;
// //		}
// 		if( FPara2->EnereyDemandMode.byActiveCalMode&0x04 )
// 		{
// 			Energy += CurNActive;
// 		}
// 	}
// 	if( FPara2->EnereyDemandMode.byActiveCalMode&0x03 )
// 	{
// //		if( FPara2->EnereyDemandMode.byActiveCalMode & 0x02 )
// //		{
// //			Energy -= CurPActive;
// //		}
// 		if( FPara2->EnereyDemandMode.byActiveCalMode&0x01 )
// 		{
// 			Energy += CurPActive;
// 		}
// 	}

// 	//舍掉符号位 正常不会有符号位
// 	dwTmp = Energy % 80000000; //组合有功电量不应该超过79999999 而且也不会出现负值
// 	//保留小数位数
// 	dwTmp = dwTmp / lib_MyPow10( 2 - Dot );

// 	if( DataType == DATA_BCD )
// 	{
// 		dwTmp = lib_DWBinToBCD( dwTmp );
// 	}
// 	memcpy( pEnergy, (void *)&dwTmp, 4 );

	// return TRUE;
}
#endif//#if( SEL_DLT645_2007 == YES )

//-----------------------------------------------
//函数功能: 脉冲尾数转换为电能
//
//参数:
//		SwitchEnergyType[in]: 转化高精度电能   	转化普通电能
//
//		Num[in]: 费率个数+1
//
//		BufLen[in]: 输入bufg长度
//
//		Buf[in]:指向电能的指针 返回hex
//
//返回值:
//
//备注:
//-----------------------------------------------
WORD api_EnergyDBaseToEnergy( eSwitchEnergyType SwitchEnergyType, BYTE Num, BYTE BufLen, BYTE* Buf )
{
	BYTE i,DotNum,EnergyLen,Len,Flag;
	QWORD Energy[MAX_RATIO+4];
	DWORD DEnergy;

	if( Num > (MAX_RATIO+1) )//极限值判断
	{
		Num = (MAX_RATIO+1);

	}
	
	Len = 0;
	memset( &Energy, 0x00, sizeof( Energy ) );
	
	if( SwitchEnergyType == SwitchNormalEnergy )//转化普通电能
	{
		DotNum = 2;
		EnergyLen = 4;
	}
	else
	{
		DotNum = 4; 
		EnergyLen = 8;
	}	

	for( i = 0; i < Num; i++ )//计算对应电能
	{
		Flag = Buf[5*i];
		memcpy( &Energy[i], Buf+5*i+1, 4 );
		Energy[i] *= (long)lib_MyPow10(DotNum);
		Energy[i] /= (long)api_GetActiveConstant();
		//冻结事件中电能脉冲，符号字节中只有0x00和0x55，若出现FF则代表着存储空间未赋值，
		//这种情况下按照脉冲去计算电能，会得到一个错误值，因此将其赋值为0。
		if(Flag == 0xff)
		{
			memset( &Energy[i], 0x00, sizeof(QWORD) );
		}
		else if( Flag == 0x55 )//数据为负号
		{
			Energy[i] *= -1;
		}
		Len += EnergyLen;
	}
	
	if( Len <= BufLen )//理论长度肯定满足
	{
		if( SwitchEnergyType == SwitchNormalEnergy )//转化普通电能
		{
			for( i = 0; i < Num; i++ )
			{
				DEnergy = Energy[i];
				memcpy( Buf+sizeof(DEnergy)*i, &DEnergy, sizeof(DEnergy) );
			}

			return  (sizeof(DEnergy)*Num);
		}
		else
		{
			for( i = 0; i < Num; i++ )
			{
				memcpy( Buf+sizeof(QWORD)*i, &Energy[i], sizeof(QWORD) );
			}
			return  (sizeof(QWORD)*Num);
		}


	}

	return 0x8000;

}

//-----------------------------------------------
//函数功能: 电能转换为脉冲数
//
//参数:
//
//		Num[in]: 费率个数+1
//
//		BufLen[in]: 输入buf长度
//
//		Buf[in]:指向电能的指针 返回hex
//
//返回值:
//
//备注:
//-----------------------------------------------
WORD api_EnergyToEnergyDBase( BYTE Num, BYTE BufLen, BYTE* Buf )
{
	BYTE i,EnergyBuf[MAX_RATIO*8+30];
	SQWORD Energy;

	if( Num > (MAX_RATIO+1) )//极限值判断
	{
		Num = (MAX_RATIO+1);

	}
	
	memset( EnergyBuf, 0x00, sizeof(EnergyBuf) );
	Energy = 0;
	
	for( i = 0; i < Num; i++ )//计算对应电能
	{
		memcpy( &Energy, Buf+8*i, 8 );
		
		if( Energy < 0 )//认为高精度电能，最高位不会走到，如果最高位有值就是组合电能为负
		{
			EnergyBuf[5*i] = 0x55;	//脉冲尾数符号位为负
			Energy *= -1;		//数据转化为正值保存
		}
		
		Energy *= (long)api_GetActiveConstant();	
		Energy /= (long)lib_MyPow10(4);	
		memcpy( &EnergyBuf[5*i+1], &Energy, 4 );
	}
	
	if( (Num*5) <= BufLen )//理论长度肯定满足
	{
		memcpy( Buf, EnergyBuf, Num*5 );
		return (Num*5);
	}
	
	return 0x8000;

}

//-----------------------------------------------
//函数功能:	获取电能地址
//
//参数:	pBuf[in/out]
//
//返回值:	4字节电能增量地址+4字节电能备份地址+4字节EE存储地址
//
//备注:	方便测试组测试
//-----------------------------------------------
BYTE api_GetFactoryEnergyPara( BYTE* pBuf )
{
	BYTE Offset;
	DWORD td;
	
	Offset = 0;
	
	//4字节电能增量Ram地址
	td = (DWORD)&EnergyRam;
	lib_ExchangeData( pBuf + Offset, (BYTE *)&td, 4 );
	Offset += 4;

	//4字节电能备份Ram地址
	td = (DWORD)&EnergyRomBackup;
	lib_ExchangeData( pBuf + Offset, (BYTE *)&td, 4 );
	Offset += 4;

	//4字节EE存储地址
	td = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[0] );
	lib_ExchangeData( pBuf + Offset, (BYTE *)&td, 4 );
	Offset += 4;

	return Offset;
}

