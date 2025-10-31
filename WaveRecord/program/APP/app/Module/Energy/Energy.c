//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.8.5
//����		����ģ��Դ�ļ�
//�޸ļ�¼	
//---------------------------------------------------------------------- 

#include "AllHead.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define SECS_OF_ONE_HOUR	3600	// 1Сʱ�ж����루3600s��


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TEnergySwapFlag_t
{
	WORD	wFlag;
	DWORD	CRC32;
}TEnergySwapFlag;

typedef enum
{
	eSWAP_ENERGY_ALL = 0,		// �ܵ���ת��
	eSWAP_ENERGY_RATIO,			// ���ʵ���ת��
	eSWAP_ENERGY_SEP,			// �������ת��
	eCLEAR_ENERGY_REMAIN		// ������β��
}eSWAP_TYPE;
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static BYTE	SwapEnergyFlag;							// ����ת��flag
static __no_init TEnergyRam EnergyRam;

static __no_init TEnergySwapFlag EnergySwapFlag[2];	// �ܣ����ʵ���ת��ɹ���־
static __no_init TEnergyRom	EnergyRomBackup;        // eeprom������ram�еı���
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static BOOL EnergyCheck( BYTE Type );
static BOOL ReadCombReActiveEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *Energy );
static BOOL ReadCombActiveEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *Energy );
static BOOL SwapEnergySub( BYTE Type );

//-----------------------------------------------
//				��������
//-----------------------------------------------



//-----------------------------------------------
//��������: ������ܻ�����Ԫ
//
//����: 	pDBase[in] ���ܻ�����Ԫָ��
// 			                  
//����ֵ:	��
//
//��ע:   
//-----------------------------------------------
static void ClearTDBase( TEnergyDBase *pDBase )
{
	memset( (void*)pDBase, 0, sizeof(TEnergyDBase) );
	lib_CheckSafeMemVerify( (BYTE *)(pDBase), sizeof(TEnergyDBase), REPAIR_CRC );
}


//-----------------------------------------------
//��������: ��ȡ��Ӧ���͵���
//
//����:
//		Type[in]
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
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
// 		p[in] ����ָ��
//����ֵ:	���ض�Ӧ���͵���
//
//��ע:
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

		#if( SEL_APPARENT_ENERGY == YES )// ���ڣ����򡢷���
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
//��������: ��ȡ���ܱ�����С��λ
//
//����: 
// 			��
//                    
//����ֵ:	����ֵN--����ram��eeprom�б������С��������ĵ����������
//
//��ע: ������ֵΪ12���������ܴ洢��С����Ϊ12������  
//-----------------------------------------------
DWORD api_GetEnergyConst( void )
{
		
	#if( SYS_DBASE_ENERGY_DOT == 0xff ) //���ݿ���ܰ��������洢
	
    return 1;
    
    #else
    
    DWORD dwEnergyConst,dwActiveConstant;//�������о���
    
		
	dwActiveConstant = api_GetActiveConstant();

	#if( SYS_DBASE_ENERGY_DOT == 1 )
	dwEnergyConst = (DWORD)(dwActiveConstant/10);					// ���ܱ���1λС��
	#elif( SYS_DBASE_ENERGY_DOT == 2 )
	dwEnergyConst = (DWORD)(dwActiveConstant/100);					// ���ܱ���2λС��
	#elif( SYS_DBASE_ENERGY_DOT == 3 )
	dwEnergyConst = (DWORD)(dwActiveConstant/1000);					// ���ܱ���3λС��
	#elif( SYS_DBASE_ENERGY_DOT == 4 )
	if( MeterTypesConst == METER_ZT )
	{
		//"ֱͨ����ѡ��4λС��������"
		for(;;);
	}
	dwEnergyConst = (DWORD)(dwActiveConstant/10000);					// ���ܱ���4λС��
	#else
	dwEnergyConst = (DWORD)(dwActiveConstant/100);					// ���ܱ���2λС��
	#endif
	
	if( dwEnergyConst == 0 )
	{
		dwEnergyConst = 1;
	}

	return dwEnergyConst;
    
    #endif
}

//-----------------------------------------------
//��������: д�����
//
//����:
//	Type[in]:
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
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
//	Value[in]:	�������
//
//����ֵ:	��
//
//��ע:   ��������
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
	//���RAM����
	if( lib_CheckSafeMemVerify( (BYTE *)pDBase, sizeof(TEnergyDBase), UN_REPAIR_CRC ) == FALSE )
	{
		ASSERT( 0, 1 );
		// ���
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
		// DLT645-2007Ҫ������ڵ���
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

	//����У��
	lib_CheckSafeMemVerify( (BYTE *)pDBase, sizeof(TEnergyDBase), REPAIR_CRC );
}

//-----------------------------------------------
//��������: ��EEPROM�еĵ���
//
//����:
//Type:		
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
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
//Ratio		����
//			1~MAX_RATIO		 ����������
//			0				 �������ܼ�
//
//Value		ָ����ܵ�ָ��
//
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:  �������������ڶ������ݣ��ʴ˺���ֻ֧�ֵ�ǰ����
//-----------------------------------------------
static BOOL ReadEnergyRom(WORD Type, WORD Ratio, BYTE * Value)
{
	TEnergyDBase DbaseEnergy;
	WORD wAddr1,Phase;
	DWORD	dwTemp;// 12���µ�ָ�룬���ݵ�ָ�룬����
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
	
	//���ж�RAM�б�������ĵ����Ƿ���ȷ
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

	// ��ram�б���ĵ������ܲ���ȷ���ٶ�ȡEEPROM�е�����
	if( EnergyRamFlag != 0x55 )
	{
		if( Ratio == 0 )
		{
			// ��
			wAddr1 = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[Phase] );	
		}
		else
		{
			// ������
			wAddr1 = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.RatioEnergy[Ratio-1][Phase] );	
		}
	
		if( api_VReadSafeMem( wAddr1, sizeof(TEnergyDBase), (BYTE *)&DbaseEnergy ) != TRUE )
		{
			return FALSE;
		}
		
		//��ram�����ݲ��ԣ���ee�ָ���ͬ����ǰram�Ͷ�
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
//��������: ʮ�����Ʒ�ʽ������
//
//����:
//  Type:
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
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
//			����
//			1~MAX_RATIO		 ������
//			0				 �������ܼ�
//	Dot[in]: С����λ��
//			0: --	��С����
//			1~N: --	1~N��С���� ֧��6λС���Ķ�ȡ������β����
//	Energy[out]:ָ����ܵ�ָ��
//
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:	����EEP+RAM�е�������ܣ�����������й�������޹�
//-----------------------------------------------
static BOOL ReadEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *Energy )
{	
	BYTE DotNum;
	WORD Phase;
	DWORD dwTemp;
	SQWORD tll;

	// ��EEPROM�е���
	if( ReadEnergyRom(Type, Ratio, (BYTE *)&dwTemp) == FALSE )
	{
		return FALSE;
	}
	
	Phase = (Type >> 12)&0x0007;

	tll = dwTemp;
	dwTemp = api_GetEnergyConst();
	tll *= dwTemp;//��ԭΪ������
	// RAM����
	if( (Ratio==0) || (Ratio==api_GetCurRatio()) )
	{
		dwTemp = GetDbaseValue(Type, &(EnergyRam.Energy[Phase]));
		tll += dwTemp;
	}

	memcpy( (void*)Energy, (void*)&tll, sizeof(tll) );	
	
	DotNum = Dot;
	if( Dot != 6 )//����β����С��Ϊ6λ ���֧��6λ���ܵĶ�ȡ
	{
		if( Dot > 4 )
		{
			if( (Type&0x8000) == 0x8000 ) //�߾��ȵ���
			{
				DotNum = 4;
			}
			else
			{
				DotNum = 2;			// Ĭ��2λС��
			}
		}
	}

	*Energy *= (long)lib_MyPow10(DotNum);
	*Energy /= (long)api_GetActiveConstant();

	return TRUE;
}
//-----------------------------------------------
//��������: ��ȡ��ǰ��������
//
//����:
//	Type[in]:
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
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
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//	Ratio[in]:
//			����
//			1~MAX_RATIO		 ������
//			0				 �������ܼ�
//
//	Dot[in]: С����λ��
//			0: --	��С����
//			1~N: --	1~N��С���� ֧��6λС���Ķ�ȡ������β����
//
//	Energy[out]:ָ����ܵ�ָ��
//			Hex�����ʾ�����λΪ����λ
//			�߾��ȵ��ܷ��� 8�ֽڣ��Ǹ߾��ȵ��ܷ���4�ֽ�
//
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:
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
	// �߾��ȵ���Ϊ8�ֽ��з�����������tll�������д���
	// �Ǹ߾��ȵ���Ϊ4�ֽ��з�����������4�ֽ�Ҫ����ȡģ����
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
                tll %= (DWORD)(100000000);// ��ֵ
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
//��������: ������й�����(����������)
//
//����:
//  Type:
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
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
//			����
//			1~MAX_RATIO		 ������
//			0				 �������ܼ�
//	Dot[in]: С����λ��
//			0: --	��С����
//			1~N: --	1~N��С����
//	Energy[out]:ָ����ܵ�ָ��
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:Dot��Ҫ����Ĭ��ֵ0xFF�����
//-----------------------------------------------
static BOOL ReadCombActiveEnergyHex( WORD Type, WORD Ratio, BYTE Dot, SQWORD *pEnergy )
{
	SQWORD llEnergy, llTemp;
	TTwoByteUnion tEnergyType;
	BYTE Flag,DotBak;

	//byActiveCalMode:�й���Ϸ�ʽ������ ����й�
	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
	//����	����		����	����	�����й�	�����й�	�����й�	�����й�
	//									0����1��	0����1��	0����1��	0����1��

	Flag = 0;
	DotBak = Dot;
	
	if( Dot != 6 )//6λС�������� ֧��1��2��3��4
	{
		DotBak = 4;
		Flag = 0x55;
	}
	
	tEnergyType.w = Type;//˳���ܵ���
	
	llEnergy = 0;
	if( FPara2->EnereyDemandMode.byActiveCalMode & 0x0c )
	{
		llTemp = 0;
		tEnergyType.b[0] = N_ACTIVE;
		//�������ǰ Dot����ʵ��λ������
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
	//����λ������
	if( Flag == 0x55 )
	{
		if( Dot != 4 )//4λС�����账�� 6λС��ǰ���ж���
		{
			//Ĭ��ֵ����
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
			//���ݰ�4λ��ȡ������Ѷ����С�����������
			llEnergy = (llEnergy/(long)lib_MyPow10(DotBak-Dot));
		}
	}

	*pEnergy = llEnergy;

	return TRUE;
}

//-----------------------------------------------
//��������: ������޹�����(�����������)
//
//����:
//  Type:
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
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
//			����
//			1~MAX_RATIO		 ������
//			0				 �������ܼ�
//	Dot[in]: С����λ��
//			0: --	��С����
//			1~N: --	1~N��С����
//	Energy[out]:ָ����ܵ�ָ��
//
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:Dot��Ҫ����Ĭ��ֵ0xFF�����
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

	if( Dot != 6 )//6λС�������� ֧��1��2��3��4
	{
		DotBak = 4;
		Flag = 0x55;
	}

	tEnergyType.w = Type;//˳���ܵ���
	
	for( i = 0; i < 4; i++ )
	{
		//�ж����޹�1�����޹�2
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

		//ֵ��1�� 2�� 0��3��Ҫ
		if( Mode == 0x01 )
		{
			llEnergy += llTemp;
		}
		else if( Mode == 0x02 )
		{
			llEnergy -= llTemp;
		}
	}

	//����λ������
	if( Flag == 0x55 )
	{
		if( Dot != 4 )//4λС�����账�� 6λС��ǰ���ж���
		{
			//Ĭ��ֵ����
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
//��������: �嵱ǰ����
//
//����: 
//	��
//                    
//����ֵ:
//  ��
//
//��ע:   
//-----------------------------------------------
void api_ClrEnergy( void )
{
	BYTE i, j;
	WORD wAddr;
	TDEnergyAccuBase	EnergyAccu;

	CLEAR_WATCH_DOG;

	//��RAM ��+A,B,C
	for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
	{
		ClearTDBase( &(EnergyRam.Energy[i]) );		
	}

	api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
	//��ram�б���ĵ��ܵͶ�	
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
	
	//�嵱ǰEEP�ܣ�A��B��C
	for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
	{
		wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[i] );
		api_ClrSafeMem( wAddr, sizeof(TEnergyDBase) );	
	}

	//�嵱ǰEEP���ʵ���
	for( i = 0; i < MAX_RATIO; i++ )
	{
		for( j = 0; j < (1+NUM_OF_SEPARATE_ENERGY_RATIO); j++ )
		{
			wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.RatioEnergy[i][j] );
			api_ClrSafeMem( wAddr, sizeof(TEnergyDBase) );
		}
	}
	
	//����,A,B,C����
	for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
	{
		wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Remain.RemainEnergy[i] );
		api_ClrSafeMem( wAddr, sizeof(TEnergyDBase) );		
	}

	// �����ۼƵ���
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
//��������: ���ܼ���
//
//����: 
//	pDW[in]:	
//			ָ�򱻼����ܵ�ָ��
//	Type[in]:	
//		0 	���������ݣ��������ĳ��ֵ�����ת�棨ÿ���Ӽ�飩
//	 	1 	���RAM�������ݣ��������ĳ��ֵ�����㣨��ʼ��ʱ��飩
//		2	���EEP�����������������ĳ��ֵ�����㣨��ʼ��ʱ��飩
//
//����ֵ:	��
//
//��ע: 
//����ڴ��еĵ��ܴ��ڶ��ѹ����������������������1ʱ��4Сʱ��������12��Сʱ���ĵ���
//����Ҫ����ת�� ֱͨ��220v 500������ 
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
			dwEnergy = (DWORD)1320*dwActiveConstant;//���ࣺ220V * 500A /1000KW/W * 4h * 3Phase * 200ConstantConst = 264000 
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
//��������: �ϵ���ܴ��� 
//
//����:  
//			��
//
//����ֵ:	��
//
//��ע: У���ȡֵ��Χ�Ե��������ת��/ת��
// 		�ڴ�ת����Ϊ�˷�ֹδ��ת��ʱ���͸�λ�����
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
				// ��δ�ɹ�
				SwapEnergySub( eSWAP_ENERGY_ALL );
				SwapEnergySub( eCLEAR_ENERGY_REMAIN );
				EnergySwapFlag[0].wFlag = 0x5A5A;
				lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[0], sizeof(TEnergySwapFlag), REPAIR_CRC );
			}
			else if( (EnergySwapFlag[1].wFlag == 0xA5A5)&&(EnergySwapFlag[0].wFlag == 0x5A5A) )
			{
				// ����δ�ɹ�
				SwapEnergySub( eSWAP_ENERGY_RATIO );
				SwapEnergySub( eCLEAR_ENERGY_REMAIN );
				EnergySwapFlag[1].wFlag = 0x5A5A;
				lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[1], sizeof(TEnergySwapFlag), REPAIR_CRC );
			}
			else//�����������ɹ��������������ɹ��� ҲҪ�ٴ�һ�� ��ֹ���λһֱû��ת��
			{
				api_SwapEnergy();
			}
		}
		else
		{
			api_SwapEnergy();
		}
	}
	
	//���ܵͶ���������ee�е�����
	//����ϵ��ʱ�����е��ܵ�У�鶼�ж�һ�£�ֻҪ��һ���������Ϊ�����������ݶ���ee�лָ�
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
//��������: ������ܴ��� 
//
//����:  ��
//
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
void api_PowerDownEnergy( void )
{
	api_SwapEnergy();
}

//-----------------------------------------------
//��������: ���ܴ������� 
//
//����:  ��
//
//����ֵ:	��
//
//��ע:	��ѭ������
//-----------------------------------------------
void api_EnergyTask( void )
{
	BYTE i;
	TRealTimer t;

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );//��ȡ��ǰʱ��
	
	// ��仯
	if( api_GetTaskFlag(eTASK_ENERGY_ID,eFLAG_SECOND) == TRUE )
	{
	    api_ClrTaskFlag(eTASK_ENERGY_ID,eFLAG_SECOND);
	    FunctionConst(ENERGY_TASK1);
		
		//��������
		if( t.Sec == (eTASK_ENERGY_ID*3+3+0) )
		{			
			for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
			{
				if( WatchEnergy( (DWORD *)&EnergyRam.Energy[i], 0 ) == FALSE )//ֻҪ��һ������ת����������ת��
				{
					api_SwapEnergy();//У��������Բ��жϣ���ΪSwapEnergy���ж�
					api_WriteSysUNMsg(WATCH_ENERGY_SWAP_ERR);
					break;
				}
			}
		}
	}

	//����ת��ҵ�񣬵�һλΪСʱת��
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
//��������: ���RAM��EEP�е��� 
//
//����:  Type:  0: ��鲻��ֱ����0
// 				1����鲻�ԣ���eeprom�лָ�β������
//
//����ֵ:	TRUE: �ܺͷ���У�飬ȡֵ��Χ����
// 			FALSE: У���/ȡֵ��Χ��
//
//��ע:
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
                // api_WriteSysUNMsg( ENERGY_CHECK_RAM_ERR );//�޳������ݲ��Ƴ���if 
			}
			else
			{
				// ��EEP�ָ�����β��
				wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Remain.RemainEnergy[i] );
				if( api_VReadSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&EnergyRam.Energy[i] ) == FALSE )
				{
					ClearTDBase(&EnergyRam.Energy[i]);
				}
				//���EEP����β��
				if( WatchEnergy( (DWORD *)&(EnergyRam.Energy[i]), 2 ) == FALSE )
				{
					ClearTDBase(&EnergyRam.Energy[i]);
				}
			}
		}
		else
		{
			//���RAM����
			if( WatchEnergy( (DWORD *)&(EnergyRam.Energy[i]), 1 ) == FALSE )
			{
				ClearTDBase(&EnergyRam.Energy[i]);
				api_WriteSysUNMsg(WATCH_ENERGY_OVERRIDE_ERR);
			}
			else
			{
				flag++;	// ���ܵ�У���ȡֵ��Χ����ȷ��
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
//��������: ����ת���Ӻ���
//
//����:  Type[in]:  eSWAP_TYPE
//
//����ֵ:	TRUE: ת����ȷ
// 			FALSE: ת�����
//
//��ע:	
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

	// ��������ת��Ϊ����
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
		
		//�ж������������Ƿ�Ϊ0����Ϊ0�������ӣ�ֱ�ӷ���TRUE
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
		
		//ͬ��RAM�б�����ܵͶ�
		memcpy( (BYTE *)&EnergyRomBackup.Energy[0], (BYTE *)&RomEnergy, sizeof(TEnergyDBase) );
	}
	else if( Type == eSWAP_ENERGY_SEP )
	{
	    #if( NUM_OF_SEPARATE_ENERGY != 0 )
		for( i = 1; i <= NUM_OF_SEPARATE_ENERGY; i++ )
		{
			pTemp = (DWORD *)&RamEnergy[i];
			pRom = (DWORD *)&RomEnergy;

			//�жϷ������������Ƿ�Ϊ0����Ϊ0��������
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
			
			//ͬ��RAM�б�����ܵͶ�
			memcpy( (BYTE *)&EnergyRomBackup.Energy[i], (BYTE *)&RomEnergy, sizeof(TEnergyDBase) );
		}
		#endif
	}
	else if( Type == eSWAP_ENERGY_RATIO )
	{
		for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY_RATIO); i++ )
		{
			//һ��Ҫ�жϷ��� ������ʲ��Ի�ѱ�ĵ��ܸ����
			if( (CurrRatio == 0)||(CurrRatio > MAX_RATIO) )
			{
				CurrRatio = DefCurrRatioConst;//ȱʡ��Ϊ��ƽʱ��
			}
			
			pTemp = (DWORD *)&RamEnergy[i];
			pRom = (DWORD *)&RomEnergy;

			//�жϷ������������Ƿ�Ϊ0����Ϊ0�������ӣ�
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
			
			//ͬ��RAM�б�����ܵͶ�
			memcpy( (BYTE *)&EnergyRomBackup.RatioEnergy[CurrRatio-1][i], (BYTE *)&RomEnergy, sizeof(TEnergyDBase) );
		}
	}
	else
	{
		// ����RAMβ�����˴������ж�pTempΪ0����
		for( i = 0; i < (1+NUM_OF_SEPARATE_ENERGY); i++ )
		{
			pRam = (DWORD *)&EnergyRam.Energy[i];
			pTemp = (DWORD *)&RamEnergy[i];
			for( j = 0; j < ( (sizeof(TEnergyDBase)-sizeof(DWORD))/sizeof(DWORD) ); j++ )
			{
				pRam[j] -= pTemp[j]*dwEnergyConst;
			}
			lib_CheckSafeMemVerify( (BYTE *)&EnergyRam.Energy[i], sizeof(TEnergyDBase), REPAIR_CRC );	//����CRCУ��

			// β����EEP
			wAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.Remain.RemainEnergy[i] );

			api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
			api_VWriteSafeMem( wAddr, sizeof(TEnergyDBase), (BYTE *)&EnergyRam.Energy[i] );
			api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
		}
	}

	return TRUE;
}

//-----------------------------------------------
//��������: ����ת�� 
//
//����:  ��
//
//����ֵ:	��
//
//��ע:	��RAM����ת�浽EEP
//-----------------------------------------------
void api_SwapEnergy( void )
{
	// ���RAM����
	EnergyCheck( 0 );
	
	// ��δ�ɹ���־
	EnergySwapFlag[0].wFlag= 0xA5A5;
	lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[0], sizeof(TEnergySwapFlag), REPAIR_CRC );
	EnergySwapFlag[1].wFlag= 0xA5A5;
	lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[1], sizeof(TEnergySwapFlag), REPAIR_CRC );

	// ת���ܵ���
	if( SwapEnergySub( eSWAP_ENERGY_ALL ) != FALSE )
	{
		EnergySwapFlag[0].wFlag= 0x5A5A;
		lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[0], sizeof(TEnergySwapFlag), REPAIR_CRC );
	}
	
	// ת����ʵ���
	if( SwapEnergySub( eSWAP_ENERGY_RATIO ) != FALSE )
	{
		EnergySwapFlag[1].wFlag= 0x5A5A;
		lib_CheckSafeMemVerify( (BYTE *)&EnergySwapFlag[1], sizeof(TEnergySwapFlag), REPAIR_CRC );
	}

	// ת��������
	SwapEnergySub( eSWAP_ENERGY_SEP );

	if( (EnergySwapFlag[0].wFlag == 0xA5A5)&&(EnergySwapFlag[1].wFlag == 0xA5A5) )
	{
		return;	// �ȴ��´�ת��
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
//��������: �½�����ܣ��������ۼƵ��ܼ���
//
//����:  	��
//	
//����ֵ:	��
//
//��ע:�����������	
//-----------------------------------------------
void api_ClosingMonEnergy( void )
{
	WORD				wAddr1,wAddr2;
	TDEnergyAccuBase	EnergyAccu;

	//����1ת�浽��2
	wAddr1 = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[0] );
	if( api_VReadSafeMem( wAddr1, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu ) == FALSE )
	{
		return;
	}

	wAddr2 = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[1] );
	api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
	api_VWriteSafeMem( wAddr2, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu );
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);

	//����ǰ��ת�浽��1		��eeprom+β����8��F��10�����峣������42949kWh�磬������
	//�����ܷ�ʽ�����ܶ�������������������������������ܲ�0.01���
	api_GetCurrEnergyData( PHASE_ALL+P_ACTIVE, DATA_HEX, 0, 2, (BYTE*)&EnergyAccu.PActive );
	api_GetCurrEnergyData( PHASE_ALL+N_ACTIVE, DATA_HEX, 0, 2, (BYTE*)&EnergyAccu.NActive );
	EnergyAccu.CRC32 = lib_CheckCRC32( (BYTE *)&EnergyAccu, (sizeof(TDEnergyAccuBase)-sizeof(DWORD)) );

	api_SetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
	api_VWriteSafeMem( wAddr1, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu );
	api_ClrSysStatus(eSYS_STATUS_EN_WRITE_ENERGY);
}

//-----------------------------------------------
//��������: ��ȡ�ۼ��õ������½��㣩
//
//����:  
//	No[in]:
//			0 ��ǰ��
//			1 ����
//
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
// 
//	Dot[in]: С����λ�� ���֧��2λС��
//			0: --	��С����
//			1~N: --	1~N��С����
//
//	pEnergy[in]:
//			ָ����ܵ�ָ��
// 			Hex�����ʾ�����λΪ����λ %0x80000000
//
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:	
//-----------------------------------------------
BOOL api_GetAccuEnergyData( BYTE No, BYTE DataType, BYTE Dot, BYTE *pEnergy )
{
	TDEnergyAccuBase EnergyAccu;
	DWORD	CurPActive,CurNActive,Energy;
	DWORD	dwTmp,dwAddr;

	//�ж�Dot�Ƿ���ȷ
	if( Dot > 2 )
	{
		Dot = 2;
	}
	
	if( No == 0 ) // ��ǰ�¶�
	{
		//�����ܷ�ʽ�����ܶ�������������������������������ܲ�0.01���
		//��ȡ���ݰ������λ����ȡ ��������� ��ȡС��λ �������ݼ������
		api_GetCurrEnergyData( PHASE_ALL+P_ACTIVE, DATA_HEX, 0, 2, (BYTE*)&CurPActive );
		api_GetCurrEnergyData( PHASE_ALL+N_ACTIVE, DATA_HEX, 0, 2, (BYTE*)&CurNActive );

		dwAddr = GET_SAFE_SPACE_ADDR( EnergySafeRom.EnergyAccMon[0] );
		if( api_VReadSafeMem( dwAddr, sizeof(TDEnergyAccuBase), (BYTE *)&EnergyAccu ) == FALSE )
		{
			return FALSE;
		}
	}
	else if( No == 1 )// ��1�¶�
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

	//byActiveCalMode:�й���Ϸ�ʽ������ ����й�
	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
	//����	����		����	����	�����й�	�����й�	�����й�	�����й�
	//									0����1��	0����1��	0����1��	0����1��

	Energy = 0;
	if( FPara2->EnereyDemandMode.byActiveCalMode & 0x0c )
	{
		//�й���Ϸ�ʽ������ 4112 �� bit1 �� bit3 ��ʶΪ��1 ���� ʱ�����ĵ������������㡣
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

	//�������λ ���������з���λ
	dwTmp = Energy % 0x80000000;
	//����С��λ��
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
//��������: ���ۼ��õ���BCD-645��Լ���������ڣ�
//
//����:
//	No[in]:
//			0 ��ǰ����
//			1 ��һ����
//
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//
//	Dot[in]: С����λ�� ���֧��2λС��
//			0: --	��С����
//			1~N: --	1~N��С����
//
//	pEnergy[in]:
//			ָ����ܵ�ָ��
// 			����hex
//
//����ֵ:	TRUE   ��ȷ
//		  FALSE  ����
//
//��ע:
//-----------------------------------------------
BOOL api_GetAccuPeriEnergy( BYTE No, BYTE DataType, BYTE Dot, BYTE *pEnergy )
{
	DWORD CurPActive, CurNActive, LastPActive, LastNActive, Energy, dwTmp;
	DWORD length;
	TFourByteUnion DataId, EnergData;
	DataId.l = 0;
	EnergData.l = 0;
	
	return FALSE;
// 	//�ж�Dot�Ƿ���ȷ
// 	if( Dot > 2 )
// 	{
// 		Dot = 2;
// 	}

// 	if( No == 0 ) // ��ǰ��������
// 	{
// 		//�����ܷ�ʽ�����ܶ�������������������������������ܲ�0.01���
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
// 	else if( No == 1 ) // ��1��������
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

// 	//byActiveCalMode:�й���Ϸ�ʽ������ ����й�
// 	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
// 	//����	����		����	����	�����й�	�����й�	�����й�	�����й�
// 	//									0����1��	0����1��	0����1��	0����1��

// 	Energy = 0;
// 	if( FPara2->EnereyDemandMode.byActiveCalMode&0x0c )
// 	{
// 		//�й���Ϸ�ʽ������ 4112 �� bit1 �� bit3 ��ʶΪ��1 ���� ʱ�����ĵ������������㡣
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

// 	//�������λ ���������з���λ
// 	dwTmp = Energy % 80000000; //����й�������Ӧ�ó���79999999 ����Ҳ������ָ�ֵ
// 	//����С��λ��
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
//��������: ����β��ת��Ϊ����
//
//����:
//		SwitchEnergyType[in]: ת���߾��ȵ���   	ת����ͨ����
//
//		Num[in]: ���ʸ���+1
//
//		BufLen[in]: ����bufg����
//
//		Buf[in]:ָ����ܵ�ָ�� ����hex
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
WORD api_EnergyDBaseToEnergy( eSwitchEnergyType SwitchEnergyType, BYTE Num, BYTE BufLen, BYTE* Buf )
{
	BYTE i,DotNum,EnergyLen,Len,Flag;
	QWORD Energy[MAX_RATIO+4];
	DWORD DEnergy;

	if( Num > (MAX_RATIO+1) )//����ֵ�ж�
	{
		Num = (MAX_RATIO+1);

	}
	
	Len = 0;
	memset( &Energy, 0x00, sizeof( Energy ) );
	
	if( SwitchEnergyType == SwitchNormalEnergy )//ת����ͨ����
	{
		DotNum = 2;
		EnergyLen = 4;
	}
	else
	{
		DotNum = 4; 
		EnergyLen = 8;
	}	

	for( i = 0; i < Num; i++ )//�����Ӧ����
	{
		Flag = Buf[5*i];
		memcpy( &Energy[i], Buf+5*i+1, 4 );
		Energy[i] *= (long)lib_MyPow10(DotNum);
		Energy[i] /= (long)api_GetActiveConstant();
		//�����¼��е������壬�����ֽ���ֻ��0x00��0x55��������FF������Ŵ洢�ռ�δ��ֵ��
		//��������°�������ȥ������ܣ���õ�һ������ֵ����˽��丳ֵΪ0��
		if(Flag == 0xff)
		{
			memset( &Energy[i], 0x00, sizeof(QWORD) );
		}
		else if( Flag == 0x55 )//����Ϊ����
		{
			Energy[i] *= -1;
		}
		Len += EnergyLen;
	}
	
	if( Len <= BufLen )//���۳��ȿ϶�����
	{
		if( SwitchEnergyType == SwitchNormalEnergy )//ת����ͨ����
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
//��������: ����ת��Ϊ������
//
//����:
//
//		Num[in]: ���ʸ���+1
//
//		BufLen[in]: ����buf����
//
//		Buf[in]:ָ����ܵ�ָ�� ����hex
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
WORD api_EnergyToEnergyDBase( BYTE Num, BYTE BufLen, BYTE* Buf )
{
	BYTE i,EnergyBuf[MAX_RATIO*8+30];
	SQWORD Energy;

	if( Num > (MAX_RATIO+1) )//����ֵ�ж�
	{
		Num = (MAX_RATIO+1);

	}
	
	memset( EnergyBuf, 0x00, sizeof(EnergyBuf) );
	Energy = 0;
	
	for( i = 0; i < Num; i++ )//�����Ӧ����
	{
		memcpy( &Energy, Buf+8*i, 8 );
		
		if( Energy < 0 )//��Ϊ�߾��ȵ��ܣ����λ�����ߵ���������λ��ֵ������ϵ���Ϊ��
		{
			EnergyBuf[5*i] = 0x55;	//����β������λΪ��
			Energy *= -1;		//����ת��Ϊ��ֵ����
		}
		
		Energy *= (long)api_GetActiveConstant();	
		Energy /= (long)lib_MyPow10(4);	
		memcpy( &EnergyBuf[5*i+1], &Energy, 4 );
	}
	
	if( (Num*5) <= BufLen )//���۳��ȿ϶�����
	{
		memcpy( Buf, EnergyBuf, Num*5 );
		return (Num*5);
	}
	
	return 0x8000;

}

//-----------------------------------------------
//��������:	��ȡ���ܵ�ַ
//
//����:	pBuf[in/out]
//
//����ֵ:	4�ֽڵ���������ַ+4�ֽڵ��ܱ��ݵ�ַ+4�ֽ�EE�洢��ַ
//
//��ע:	������������
//-----------------------------------------------
BYTE api_GetFactoryEnergyPara( BYTE* pBuf )
{
	BYTE Offset;
	DWORD td;
	
	Offset = 0;
	
	//4�ֽڵ�������Ram��ַ
	td = (DWORD)&EnergyRam;
	lib_ExchangeData( pBuf + Offset, (BYTE *)&td, 4 );
	Offset += 4;

	//4�ֽڵ��ܱ���Ram��ַ
	td = (DWORD)&EnergyRomBackup;
	lib_ExchangeData( pBuf + Offset, (BYTE *)&td, 4 );
	Offset += 4;

	//4�ֽ�EE�洢��ַ
	td = GET_SAFE_SPACE_ADDR( EnergySafeRom.Energy.Energy[0] );
	lib_ExchangeData( pBuf + Offset, (BYTE *)&td, 4 );
	Offset += 4;

	return Offset;
}

