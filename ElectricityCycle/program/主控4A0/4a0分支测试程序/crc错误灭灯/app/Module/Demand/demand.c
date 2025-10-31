//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.30
//����		���������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------

#include "AllHead.h"

#if( MAX_PHASE == 3 )//ֻ�������Ŵ���������
#if( SEL_DEMAND_2022 == NO )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#if( SEL_REACTIVE_DEMAND == YES )
	// �й����������޹�
	#define DEMAND_ITEM			2
#else
	// �й�
	#define DEMAND_ITEM			1
#endif

#if( SEL_APPARENT_DEMAND == YES )
	#undef DEMAND_ITEM
	// �й����������޹�������
	#define DEMAND_ITEM		3
#endif

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TDemandRam_t
{
	//������
	TDemandDBase TotalDemand;
	
	//��ǰ��������
	TDemandDBase RatioDemand;
	
	//�����������������
	#if(SEL_FREEZE_PERIOD_DEMAND == YES)
	TDemandDBase FreezePeriodDemand;
	#endif
	
	//��ǰ���� �й� �޹� ��������
	long CurrentDemand[DEMAND_ITEM];
	
}TDemandRam;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//�ڴ��е�����
static TDemandRam DemandRam;

// ����������
static WORD SecondLength;

// �������ڲ���
static BYTE DemandMultiple;

// ��ǰ����
static BYTE RecordTariff;

// ���ܷ����־λ
//d0 ����
//d1 ����
//d2 ����
//d3 ����
//d4 �й����� 0������1������
//d5 �޹����� 0������1������
static BYTE RecordFlag[2];//0:��ʷ��־ 1����ǰ��־

// �������
static WORD CalPoint[DEMAND_ITEM][2];

//�й����������޹������ڵ��ܵļ������ʵķֲ������ۻ���Ԫ
static long StepDemand[DEMAND_ITEM][2][15];

//�������ۻ�����
// ��һ�㣺�й����������޹�������
// �ڶ��㣺�ܵļ�������
static long CumulatePower[DEMAND_ITEM][2];

// �����������ڲ�����
// ��һ�㣺�й����������޹�������
// �ڶ��㣺�ܵļ�������
static BYTE MinuteStep[DEMAND_ITEM][2];

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static BOOL DbReadDemandBin(WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand);
static BOOL DbReadPNRActiveDemand( WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand );
static void CountDemand(BYTE erg,BYTE tar);
static void CalPeriod( void );
//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ��¼�����ϴ��й����޹����ܷ��򼰷���
//
//����: 	Type[in]	0:��¼��ʷ��־  1����¼��ǰ��־
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
static void BackupEnergyFlag( BYTE Type )
{
	ASSERT( Type < 2, 0 );
	
	// ��¼�й����ܷ��� 0��1��
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE )
	{
		RecordFlag[Type] |= (0x10);
	}
	else
	{
		RecordFlag[Type] &= ~(0x10);
	}

	// ��¼�޹����ܷ���
	#if( SEL_REACTIVE_DEMAND == YES )
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q ) == TRUE )
	{
		RecordFlag[Type] |= 0x20;
	}
	else
	{
		RecordFlag[Type] &= ~(0x20);
	}
	#endif

	// ��¼��ǰ����
	if( Type == 0 )
	{
		RecordTariff = api_GetCurRatio();
	}
}


//-----------------------------------------------
//��������: ��ȡ�������������ڶ�Ӧ����������Ԫ��ƫ�Ƶ�ַ
//
//����: 
//	Type[in]:	P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//              APPARENT_P	10		// ��������
//				APPARENT_N	11		// ��������
//����ֵ: 	ƫ�Ƶ�ַ
//
//��ע:   
//-----------------------------------------------
static WORD GetTDemandDbaseOffset(BYTE Type)
{
 	WORD Addr = 0;

 	switch(Type)
 	{
 		case P_ACTIVE:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Active[0] );
 			break;
 		case N_ACTIVE:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Active[1] );
 			break;

 		#if( SEL_REACTIVE_DEMAND == YES )
 		case RACTIVE1:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, RActive[0] );
 			break;
 		case RACTIVE2:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, RActive[1] );
 			break;
 		case RACTIVE3:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, RActive[2] );
 			break;
 		case RACTIVE4:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, RActive[3] );
 			break;
 		#endif

 		#if( SEL_APPARENT_DEMAND == YES )
 		case APPARENT_P:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Apparent[0] );
 			break;
 		case APPARENT_N:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Apparent[1] );
 			break;
 		#endif

 		default:
 			Addr = GET_STRUCT_ADDR( TDemandDBase, Active[0] );
 			ASSERT( 0!=0, 0 );
 			break;
 	}

 	return Addr;
}


#if(SEL_FREEZE_PERIOD_DEMAND == YES)
//-----------------------------------------------
//��������: �ж��ڼ�������������ʱ���Ƿ񵽣�ʱ�䵽����������
//
//����: 	��
//
//����ֵ: 	��
//
//��ע:   ÿ���ӵ���һ��
//-----------------------------------------------
static void DealFreezePeriodDemand( void )
{
	//��λ�����Ϊ����
	if( FPara2->EnereyDemandMode.IntervalDemandFreezePeriod.Unit != 1 )
	{
		return;
	}
	
	//�������ڲ�����1��
	if( FPara2->EnereyDemandMode.IntervalDemandFreezePeriod.FreezePeriod > 1440 )
	{
		return;
	}
	
	//�������ڲ���Ϊ0
	if( FPara2->EnereyDemandMode.IntervalDemandFreezePeriod.FreezePeriod == 0 )
	{
		return;
	}
	
	//�ж��ڼ�������������ʱ���Ƿ񵽣�ʱ�䵽�õ�ǰ����
	if( (g_RelativeMin%FPara2->EnereyDemandMode.IntervalDemandFreezePeriod.FreezePeriod) == 0 )
	{
		memcpy( (void *)&DemandRam.FreezePeriodDemand, (void *)&DemandRam.TotalDemand, sizeof(DemandRam.FreezePeriodDemand) );
	}
}


//-----------------------------------------------
//��������: �����ڼ�����
//
//����: 	
//	Type[in]:	P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//              APPARENT_P	10		// ��������
//				APPARENT_N	11		// ��������
//	pDemand[in]	������������
//
//����ֵ: 	��
//
//��ע:   ÿ�θ���������ͬ���ж��Ƿ���Ҫ�����ڼ�����
//-----------------------------------------------
static void DbWriteFreezePeriodDemand( BYTE Type, TDemandData *pDemand )
{
	BYTE *pDemandData=0;
	
	pDemandData = (BYTE *)&DemandRam.FreezePeriodDemand;
	pDemandData += GetTDemandDbaseOffset( Type );
	if( pDemand->DemandValue > ((TDemandData *)pDemandData)->DemandValue )
	{
		memcpy( pDemandData, pDemand, sizeof(TDemandData) );
		DemandRam.FreezePeriodDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.FreezePeriodDemand), sizeof(TDemandDBase)-sizeof(DWORD));
	}
}


//-----------------------------------------------
//��������: ���ڼ����������������ʱ��
//
//����: 
//	Type[in]:	P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//              APPARENT_P	10		// ��������
//				APPARENT_N	11		// ��������
//	Dot[in]:	С����λ��
//	pDemand[out]:�������
//
//����ֵ:	TRUE/FALSE

//��ע: 
//-----------------------------------------------
static BOOL DbReadFreezePeriodDemandHex(WORD Type, BYTE Dot, BYTE * pDemand)
{
	BYTE *pDemandData=0;
	TDemandData TmpDemandData;
	
	if( Dot == 0xff )
	{
		Dot = 4;
	}
	ASSERT( (Dot<=6)&&(Dot>=3), 0 );//��kw 4 5 6���ǣ�eeprom��w ��λС��
	if( Dot > 6 )
	{
		Dot = 4;
	}
	
	#if( SEL_REACTIVE_DEMAND == YES )
	if( (Type == P_RACTIVE) || (Type == N_RACTIVE) )
	{
		return DbReadPNRActiveDemand( Type|0x8000, 0, Dot, pDemand );
	}
	#endif

	pDemandData = (BYTE *)&DemandRam.FreezePeriodDemand;
	pDemandData += GetTDemandDbaseOffset( Type );
	memcpy( (BYTE *)&TmpDemandData, (BYTE*)pDemandData, sizeof(TmpDemandData) );
	if( TmpDemandData.DemandValue != 0xffffffff )
	{
		TmpDemandData.DemandValue /= (long)lib_MyPow10(6-Dot);
	}

	memcpy( (void*)pDemand, (void*)&TmpDemandData, sizeof(TmpDemandData) );

	return TRUE;
}
#endif//#if(SEL_FREEZE_PERIOD_DEMAND == YES)


#if( SEL_REACTIVE_DEMAND == YES )
//-----------------------------------------------
//��������: �жϸ��������Ƿ������޹�ģʽ����
//
//����: 
//	Mode[in]:		�޹����ģʽ��
//	Quadrant[in]:	ָ������
//
//����ֵ:	TRUE--�������������޹�ģʽ����
//			FALSE--�������޲������޹�ģʽ����
//��ע: 
//-----------------------------------------------
static BOOL QuadrantIsInMode( BYTE Mode, BYTE Quadrant )
{
	if( ((Mode>>(2*Quadrant))&0x03) == 0 )
	{
		return FALSE;
	}
	
	return TRUE;
}


//-----------------------------------------------
//��������: ��ȡ��������
//
//����: 
//	erg[in]:	�й�/�޹�/���� ����
//	tar[in]:	��/����
//
//����ֵ:	��������	����/����/1��/2��/3��/4��/������/������
//			
//��ע: 
//-----------------------------------------------
BYTE GetDemandType(BYTE erg,BYTE tar)
{
	BYTE i,Type;
	long lTmpDemand;
	
	//���������Ĳ���ȷ����ǰ�������ʲô�����Ӷ�������Ӧ������
	if(erg == 0)// �й�
	{
		if(RecordFlag[0]&0x10)
		{
			Type = N_ACTIVE;
		}
		else
		{
			Type = P_ACTIVE;
		}
	}
	#if( SEL_REACTIVE_DEMAND == YES )
	else if(erg == 1)// �������޹�
	{
		switch(RecordFlag[0]&0x30)
		{
		case 0x00://һ����
			Type = RACTIVE1;
			break;
		case 0x10://������
			Type = RACTIVE2;
			break;
		case 0x20://������
			Type = RACTIVE4;
			break;
		default://������
			Type = RACTIVE3;
		}
	}
	#endif
	#if( SEL_APPARENT_DEMAND == YES )
	else// ����
	{
		if(RecordFlag[0]&0x10)
		{
			Type = APPARENT_N;
		}
		else
		{
			Type = APPARENT_P;
		}
	}
	#endif
		
	
	if( api_GetProHardFlag( ePRO_HARD_DEMAND_CONTINUE_MEASURE ) == TRUE )
	{
		lTmpDemand = 0;
		for(i=0; i<DemandMultiple; i++)
		{
			lTmpDemand += StepDemand[erg][tar][i];
		}
			
		if( erg == 0 )
		{
			if( lTmpDemand < 0 )
			{
				Type = N_ACTIVE;
			}
			else
			{
				Type = P_ACTIVE;
			}
		}
		#if( SEL_APPARENT_DEMAND == YES )
		else if( erg == 2 )
		{
			if( lTmpDemand < 0 )
			{
				Type = APPARENT_N;
			}
			else
			{
				Type = APPARENT_P;
			}
		}
		#endif
	}
	
	return Type;
}


//-----------------------------------------------
//��������: �������޹�����
//
//����: 
//	Type[in]:		�޹����ģʽ��
//						bit7Ϊ1��ʾ���������������� 0--��ʾ����ͨ����
//						P_ACTIVE	1
//						N_ACTIVE	2
//						P_RACTIVE	3
//						N_RACTIVE	4
//						RACTIVE1	5
//						RACTIVE2	6
//						RACTIVE3	7
//						RACTIVE4	8
//		              	APPARENT_P	10		// ��������
//						APPARENT_N	11		// ��������
//	Ratio[in]:		����
//	Dot[in]:		С��λ��
//	pDemand[out]:	��������
//
//����ֵ:	TRUE/FALSE
//��ע: ���ٴ洢�����޹����ݣ�ֱ�Ӵ��������л�ȡ
//-----------------------------------------------
static BOOL DbReadPNRActiveDemand( WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand )
{
	BYTE i,ReactiveMode,byPNRactiveDemandDir,DemandType;
    long DemandValue;
	TDemandData TmpDemandData1,TmpDemandData2;

	if(Dot > 6)//�������С��λ������6,��������4λС������
	{
		Dot = 4;
	}
	DemandType = (Type&0x7f);
	if( DemandType == P_RACTIVE )
	{
		if( FPara2->EnereyDemandMode.PReactiveMode == 0x00 )
		{
			ReactiveMode = 0x01;//ģʽ��Ϊ0��������޹�1�����һ���޵�����
		}
		else
		{
			ReactiveMode = FPara2->EnereyDemandMode.PReactiveMode;
		}
	}
	else if( DemandType == N_RACTIVE )
	{
		if( FPara2->EnereyDemandMode.NReactiveMode == 0x00 )
		{
			ReactiveMode = 0x10;//ģʽ��Ϊ0��������޹�2����������޵�����
		}
		else
		{
			ReactiveMode = FPara2->EnereyDemandMode.NReactiveMode;
		}
	}
	else
	{
        return FALSE;
	}

	//����޹����������ȸ�0����ʾ����
	byPNRactiveDemandDir = 0;//0-���� 1-���� ��ʱ��λ���ú���
	memset( (void*)&TmpDemandData1, 0x00, sizeof(TDemandData) );//������
	for(i=0; i<4; i++)
	{
		//����޹�1����������޹�2����������޹��ķ�ʽ�ж�
		if( QuadrantIsInMode( ReactiveMode, i ) == FALSE )
		{
			continue;
		}

		#if( ( SEL_REACTIVE_DEMAND == YES )&&( SEL_FREEZE_PERIOD_DEMAND == YES ) ) 
		if(Type & 0x8000)
		{
			memcpy( (BYTE *)&TmpDemandData2, (BYTE *)&DemandRam.FreezePeriodDemand.RActive[i], sizeof(TDemandData));
			TmpDemandData2.DemandValue /= (long)lib_MyPow10(6-Dot);
		}
		else
		#endif
		{
			DbReadDemandBin( RACTIVE1+i, Ratio, Dot, (BYTE *)&TmpDemandData2 );
		}
		
		if( TmpDemandData2.DemandValue > TmpDemandData1.DemandValue )
		{
			TmpDemandData1 = TmpDemandData2;

			//����޹���������
			if( i <= 1 )//I���� II����
			{
				byPNRactiveDemandDir = 0;//0-���� 1-���� ��ʱ��λ���ú���
			}
			else//III���� IV����
			{
				byPNRactiveDemandDir = 1;//0-���� 1-���� ��ʱ��λ���ú���
			}
		}
	}

	if( TmpDemandData1.DemandValue == 0xffffffff )
	{
		memset( (void*)&TmpDemandData1, 0xff, sizeof(TDemandData) );
	}
	else
	{
		DemandValue = TmpDemandData1.DemandValue;
		if( byPNRactiveDemandDir == 1 )
		{
			DemandValue *= (-1);
		}
		TmpDemandData1.DemandValue = DemandValue;
	}
	
	memcpy( (BYTE*)pDemand, (BYTE*)&TmpDemandData1, sizeof(TDemandData) );

	return TRUE;
}

#endif//#if( SEL_REACTIVE_DEMAND == YES )


//-----------------------------------------------
//��������:�������ʷ��ͱ仯ʱ�������� 
//
//����: 	i[in]	��������	0--�й����� 1--�޹�����	2--��������
//          j[in]	����		0--��������	1--��������          
//����ֵ: 	��
//
//��ע:���Ӧ��������Ϊ��ʱ���嵱ǰ����
//-----------------------------------------------
static void ChangeCount(BYTE i,BYTE j)
{
	BYTE k,Type;
	DWORD dwAddr;
	TDemandData * p1;

	//���������������
	CalPoint[i][j] = 0;
	//������������
	MinuteStep[i][j] = 0;
	//���������ۼ�����
	CumulatePower[i][j] = 0;
	//������ƽ����������
	for(k=0; k<15; k++)
	{
		StepDemand[i][j][k] = 0;
	}

	//��Ϊ�ܣ����Ӧ�ĵ�ǰ����
	if( j == 0 )
	{
		memset( (void *)&DemandRam.CurrentDemand[i], 0x00, sizeof(DWORD) );
	}
	else
	{
		Type = GetDemandType(i,j);
		dwAddr = GetTDemandDbaseOffset(Type);
		p1 = (TDemandData *)((DWORD)dwAddr + (DWORD)&DemandRam.RatioDemand);
		memset( (void*)p1, 0x00, sizeof(TDemandData) );
		DemandRam.RatioDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
	}
}


//-----------------------------------------------
//��������: ����δ�仯��������
//
//����: 	i[in]	��������	0--�й����� 1--�޹�����	2--��������
//          j[in]	����		0--��������	1--�������� 
//                    
//����ֵ: 	��
//
//��ע:��������ã����жϹ�һ������ʱ��ʱ��ˢ�¶�Ӧ������ƽ�����ʲ����㵱ǰ����   
//-----------------------------------------------
static void NoChangeCount(BYTE i, BYTE j)
{
	CalPoint[i][j]++;
	if( CalPoint[i][j] == SecondLength )
	{
		StepDemand[i][j][MinuteStep[i][j] % DemandMultiple] = ((CumulatePower[i][j]/SecondLength));

		MinuteStep[i][j]++;

		//�����ϵ��MinuteStepΪ0�����Բ���һ�����ڵ�����²����������
		if( MinuteStep[i][j] < DemandMultiple )
		{
		}
		else
		{
			//ִ�й�һ���������ں󣬰�MinuteStep��ʼ��Ϊ15�������Ժ�ÿ������ʱ�䶼�����һ������
			if( MinuteStep[i][j] >= (DemandMultiple*2) )
			{
				MinuteStep[i][j] = DemandMultiple;
			}

			CountDemand(i,j);//����

			if( (i==0) && (j==0) )
			{
				api_MultiFunPortCtrl( eDEMAND_PERIOD_OUTPUT );
			}
		}

		CumulatePower[i][j] = 0;//��Ĵ���
		CalPoint[i][j] = 0;//�����������
	}
}

//-----------------------------------------------
//��������: �����������������ÿ��ִ��һ��
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_DemandTask( void )
{
	BYTE i,j;
	long lTmp[3];
	
	//�ϵ����ʱ5���ٿ�ʼ��������
	if( g_PowerOnSecTimer >= 25 )
	{
		return;
	}
	
	if( api_GetTaskFlag(eTASK_DEMAND_ID,eFLAG_SECOND) == TRUE )//����һ����
	{
		api_ClrTaskFlag(eTASK_DEMAND_ID,eFLAG_SECOND);//�����־
	}
	else
	{
		return;
	}
	
	#if(SEL_FREEZE_PERIOD_DEMAND == YES)
	if( api_GetTaskFlag( eTASK_DEMAND_ID, eFLAG_MINUTE ) == TRUE )
	{
		//����ӱ�־
		api_ClrTaskFlag(eTASK_DEMAND_ID,eFLAG_MINUTE);
		//����ͬ���������ڲ���������ʱ��
		CalPeriod();
		DealFreezePeriodDemand();
	}
	#endif
	
	BackupEnergyFlag( 1 );
	
	//��������Դͳһ����5λС�����棬6λС������220v��60A��3�࣬3λС������λw��60��ĺͻ����
	api_GetRemoteData( PHASE_ALL+REMOTE_P, DATA_HEX, 5, 4, (BYTE *)&lTmp[0] );
	api_GetRemoteData( PHASE_ALL+REMOTE_Q, DATA_HEX, 5, 4, (BYTE *)&lTmp[1] );
	api_GetRemoteData( PHASE_ALL+REMOTE_S, DATA_HEX,   5, 4, (BYTE *)&lTmp[2] );
	if( api_GetProHardFlag( ePRO_HARD_DEMAND_CONTINUE_MEASURE ) == TRUE )
	{
		RecordFlag[0] &= ~0x10;//���й������ٸı�
		if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE )
		{
			RecordFlag[0] |= 0x10;
			lTmp[0] *= -1;
			lTmp[2] *= -1;
		}
		else
		{
			//�޲���
		}
	}
	else
	{
		//�޲���
	}
	
	// ������ܷ���û��
	if( (RecordFlag[0]&0x30)==(RecordFlag[1]&0x30) )
	{
		// �������û��
		if( RecordTariff == api_GetCurRatio() )
		{
			for(i=0; i<DEMAND_ITEM; i++)
			{
				for(j=0; j<2; j++)
				{
					CumulatePower[i][j] += lTmp[i];
					
					NoChangeCount(i,j);
				}
			}
		}
		else// ������ʱ���
		{
			for(i=0; i<DEMAND_ITEM; i++)
			{
				for(j=0; j<2; j++)
				{	
					//�Ը������Ƚ�����ǰ������������
					if( j==1 )//�����ʵ�
					{
						ChangeCount(i,j);
					}
					else//���ܵ���˵����ʲ��ñ�
					{						
						CumulatePower[i][j] += lTmp[i];
						
						NoChangeCount(i,j);
					}
				}
			}
		}
	}
	else// ������ܷ������
	{
		// ����й�û��  �޹�����
		if( (RecordFlag[0]&0x10)==(RecordFlag[1]&0x10) )
		{
			// �������û��
			if( RecordTariff == api_GetCurRatio() )
			{
				for(i=0; i<DEMAND_ITEM; i++)
				{
					for(j=0; j<2; j++)
					{
						if(i == 1)// �޹��������޵�
						{
							// �Ƚ�����ǰ��������
							ChangeCount(i,j);
						}
						else// �й������ڷ��򲻱�
						{
							CumulatePower[i][j] += lTmp[i];
							
							NoChangeCount(i,j);
						}
					}
				}
			}
			else// ������ʱ���
			{
				for(i=0; i<DEMAND_ITEM; i++)
				{
					for(j=0; j<2; j++)
					{
						// ���й��ܵĵ�������
						if( (i==0)&&(j==0) )
						{
							CumulatePower[i][j] += lTmp[i];
							
							NoChangeCount(i,j);
						}
						// ���ڵ�������
						#if( SEL_APPARENT_DEMAND == YES )
						else if( (i==2)&&(j==0) )
						{
							CumulatePower[i][j] += lTmp[i];
							
							NoChangeCount(i,j);
						}
						#endif
						else
						{
							//����Ļ����й��������޹������ڸ����ʵĽ����Ƚ��������
							ChangeCount(i,j);
						}
					}
				}
			}
		}
		else
		{
			//û���������޹��ˣ��������޹����������л�ȡ �����й��仯�ˣ����ޱض��仯
			for(i=0; i<DEMAND_ITEM; i++)
			{
				for(j=0; j<2; j++)
				{
					ChangeCount(i,j);
				}
			}
		}
	}

	//��¼��־
	BackupEnergyFlag( 0 );
}


//-----------------------------------------------
//��������: ����ǰ��ַ��������������
//
//����: 
//	Type[in]:		P_ACTIVE	1
//					N_ACTIVE	2
//					P_RACTIVE	3
//					N_RACTIVE	4
//					RACTIVE1	5
//					RACTIVE2	6
//					RACTIVE3	7
//					RACTIVE4	8
//		            APPARENT_P	10		// ��������
//					APPARENT_N	11		// ��������
//	Ratio[in]:		����
//	Dot[in]:		С��λ��
//	pDemand[out]:	��������
//
//����ֵ:	TRUE/FALSE
//��ע:��ǰ���������������eeprom�� 
//-----------------------------------------------
static BOOL DbReadDemandBin(WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand)
{
	DWORD dwAddr,DemandValue;
	TDemandDBase TmpDemandDBase;
	BYTE *pDemandData=0;
	
	ASSERT( Ratio <= MAX_RATIO, 0 );

	if( Ratio == 0 )
	{
		//������
		dwAddr = GET_SAFE_SPACE_ADDR( DemandSafeRom.TotalDemand );
	}
	else
	{
		//��Ӧ��������
		dwAddr = GET_SAFE_SPACE_ADDR( DemandSafeRom.RatioDemand[Ratio-1] );
	}
	
	api_VReadSafeMem( dwAddr, sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
	
	if( Dot == 0xff )//����Ĭ�ϰ�����λС������
	{
		Dot = 4;
	}
	if(Dot > 6)//����������С��λ������6����Ĭ��С��λ������
	{
		Dot = 4;
	}

	pDemandData = (BYTE *)&TmpDemandDBase;
	pDemandData += GetTDemandDbaseOffset( Type );
	
	if( Dot == 4 )//С��λ��Ϊ4λ���н�λ
	{   
	    //�����ش򿪣������һλ��λ
    	if( api_GetProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) == TRUE )
    	{
    	    if( (((TDemandData *)pDemandData)->DemandValue) != 0 )
    	    {
    		    ((TDemandData *)pDemandData)->DemandValue += 50;
    	    }
    	}
	}

	DemandValue = (((TDemandData *)pDemandData)->DemandValue);
	
	if(  DemandValue != 0xffffffff )
	{
		((TDemandData *)pDemandData)->DemandValue /= (long)lib_MyPow10(6-Dot);
	}
	
	memcpy( (void*)pDemand, (void*)pDemandData, sizeof(TDemandData) );

	return TRUE;
}


//-----------------------------------------------
//��������: ˢ�±���ĵ�ǰ�������
//
//����: 
//	Type[in]:		P_ACTIVE	1
//					N_ACTIVE	2
//					P_RACTIVE	3
//					N_RACTIVE	4
//					RACTIVE1	5
//					RACTIVE2	6
//					RACTIVE3	7
//					RACTIVE4	8
//		            APPARENT_P	10		// ��������
//					APPARENT_N	11		// ��������
//	Ratio[in]:		����
//	pDemand[in]:	��������
//
//����ֵ:	��
//��ע:����ǰ��������ee�������������������ee����Ķ�Ӧ�������
//		��Ϊ�ܣ���ͬ�������ڼ���������ǰ�������
//-----------------------------------------------
static void DbWriteDemand(WORD Type, WORD Ratio, TDemandData *pDemand)
{
	DWORD dwAddr;
	TDemandData TmpDemandData;
	TDemandDBase TmpDemandDBase;
	TDemandData * p1;

	//�ж��Ƿ�Ϊ��ǰ����
	if( Ratio==api_GetCurRatio() )
	{
		dwAddr = GetTDemandDbaseOffset(Type);
		p1 = (TDemandData *)((DWORD)dwAddr + (DWORD)&DemandRam.RatioDemand);

		//��ǰ���ʴ���ֱ�Ӹ���RAM�е�������
		if( (pDemand->DemandValue) > (p1->DemandValue) )
		{
			memcpy( (void*)p1, (void*)pDemand, sizeof(TDemandData) );
			//����У��
			DemandRam.RatioDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
			//ֱ��дeeprom������ͣ��Ͳ�����д��
			if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(DemandSafeRom.RatioDemand[Ratio-1]), sizeof(TmpDemandDBase), (BYTE *)&TmpDemandDBase ) != FALSE )
			{
				memcpy( (BYTE *)&TmpDemandData, (BYTE*)(dwAddr+(BYTE*)&TmpDemandDBase), sizeof(TmpDemandData) );
				if( p1->DemandValue > TmpDemandData.DemandValue )
				{
					memcpy( (BYTE*)(dwAddr+(BYTE*)&TmpDemandDBase), (BYTE*)p1, sizeof(TmpDemandData) );
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(DemandSafeRom.RatioDemand[Ratio-1]), sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
				}
			}
		}
	}
	//������
	else if( Ratio == 0 )
	{
		dwAddr = GetTDemandDbaseOffset(Type);
		p1 = (TDemandData *)((DWORD)dwAddr + (DWORD)&DemandRam.TotalDemand);

		//��ǰ����������ֱ�Ӹ���RAM�е��������
		if( (pDemand->DemandValue) > (p1->DemandValue) )
		{
			memcpy( (void*)p1, (void*)pDemand, sizeof(TDemandData) );
			//����У��
			DemandRam.TotalDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.TotalDemand), sizeof(TDemandDBase)-sizeof(DWORD));
			//ֱ��дeeprom������ͣ��Ͳ�����д��
			if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(DemandSafeRom.TotalDemand), sizeof(TmpDemandDBase), (BYTE *)&TmpDemandDBase ) != FALSE )
			{
				memcpy( (BYTE *)&TmpDemandData, (BYTE*)(dwAddr+(BYTE*)&TmpDemandDBase), sizeof(TmpDemandData) );
				if( p1->DemandValue > TmpDemandData.DemandValue )
				{
					memcpy( (BYTE*)(dwAddr+(BYTE*)&TmpDemandDBase), (BYTE*)p1, sizeof(TmpDemandData) );
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(DemandSafeRom.TotalDemand), sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
				}
			}
		}
		
		#if(SEL_FREEZE_PERIOD_DEMAND == YES)
		DbWriteFreezePeriodDemand( Type, pDemand );
		#endif
		
		//д�뵱ǰ������˲ʱ����
		switch( Type )
		{
		case P_ACTIVE:
			DemandRam.CurrentDemand[0] = pDemand->DemandValue;
			break;
		case N_ACTIVE:
			DemandRam.CurrentDemand[0] = ~pDemand->DemandValue;//����
			DemandRam.CurrentDemand[0] += 1;
			break;
		#if( SEL_REACTIVE_DEMAND == YES )
		case RACTIVE1:
		case RACTIVE2:
			DemandRam.CurrentDemand[1] = pDemand->DemandValue;
			break;
		case RACTIVE3:
		case RACTIVE4:
			DemandRam.CurrentDemand[1] = ~pDemand->DemandValue;//����
			DemandRam.CurrentDemand[1] += 1;
			break;
		#endif
		#if( SEL_APPARENT_DEMAND == YES )
		case APPARENT_P:
			DemandRam.CurrentDemand[2] = pDemand->DemandValue;
			break;
		case APPARENT_N:
			DemandRam.CurrentDemand[2] = ~pDemand->DemandValue;//����
			DemandRam.CurrentDemand[2] += 1;
			break;
		#endif
		default:
			break;
		}
	}
}


//-----------------------------------------------
//��������: ��������ֵ
//
//����: 	erg[in]		�������� 0:�й�  1:�������޹�  2:����
//			tar[in]		�������� 0:�ܵ�  1:�����ʵ�
//                    
//����ֵ: 	��
//
//��ע: ��������6λС����EEPROM  
//-----------------------------------------------
static void CountDemand(BYTE erg,BYTE tar)
{
	BYTE i,Type;
	long lTmpDemand;
	TDemandData	TmpDemandData;

	Type = GetDemandType(erg,tar); 

	lTmpDemand = 0;
	for(i=0; i<DemandMultiple; i++)
	{
		lTmpDemand += StepDemand[erg][tar][i];
	}
	
	if( api_GetProHardFlag( ePRO_HARD_DEMAND_CONTINUE_MEASURE ) == TRUE )
	{
		if( (erg==0) || (erg==2) )
		{
			if( lTmpDemand < 0 )
			{
				lTmpDemand *= -1;
			}
		}
	}

	TmpDemandData.DemandValue = lTmpDemand;

	//eeprom�е�������Ϊ�洢6λС���㣬��λkw
	TmpDemandData.DemandValue *= 10;
	TmpDemandData.DemandValue /= (long)DemandMultiple;
	api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );
	
	DbWriteDemand(Type, RecordTariff*tar, &TmpDemandData );
	if( tar != 0 )
	{
		//�ѷ�����������������дһ�Σ�ȷ�����������ڷ�������
		DbWriteDemand(Type, 0, &TmpDemandData );
	}
}


//-----------------------------------------------
//��������: �ϵ��ȷ���������ڡ�����
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   �ϵ�����������ͨѶ���ò�������øú���
//-----------------------------------------------
static void CalPeriod( void )
{
	BYTE    Flag;
	BYTE    Step, Period;

	Step = FPara2->EnereyDemandMode.DemandSlip;
	Period = FPara2->EnereyDemandMode.DemandPeriod;

	Flag = TRUE;

	//�������60����
	if( (Step == 0) || (Step > 60) )
	{
		Flag = FALSE;
	}

	//���ڲ�����60����
	if( (Period == 0) || (Period > 60) )
	{
		Flag = FALSE;
	}

	if( Period < Step )
	{
		Flag = FALSE;
	}

	//Ҫ�����ںͻ���Ȳ�����15
	if( (Period/Step) > 15 )
	{
		Flag = FALSE;
	}

	if( Flag == FALSE )
	{
		DemandMultiple = 15;
		SecondLength = 60;//60��
	}
	else
	{
		DemandMultiple = Period/Step;
		SecondLength = Step*60;
	}

	
}

//-----------------------------------------------
//��������: ��RAM�е���������
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   ���ϵ缰��Լ������ʼ������ʱ����
//-----------------------------------------------
static void ClearDemandRamData(void)
{
	BYTE i,j;
	
	memset(StepDemand, 0, sizeof(StepDemand));

	for(i=0;i<DEMAND_ITEM;i++)
	{
		for(j=0;j<2;j++)
		{
			CumulatePower[i][j] = 0;
			CalPoint[i][j] = 0;
			MinuteStep[i][j] = 0;
		}
	}
}
//-----------------------------------------------
//��������: ����ģ���ʼ��
//
//����: 	   ��
//                    
//����ֵ:     ��
//
//��ע:   
//-----------------------------------------------
void api_InitDemand( void )
{
	// ȷ���������ڡ�����
	CalPeriod();
	
	ClearDemandRamData();

	//���ڴ��е�ǰ��������͵�ǰ����
	api_ClrDemand( eClearRamDemand);

	// ��¼�����־
	BackupEnergyFlag( 0 );
}
//-----------------------------------------------
//��������: ����ģ���ϵ��ʼ��
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_PowerUpDemand( void )
{		
	// ȷ���������ڡ�����
	CalPeriod();
	
	ClearDemandRamData();

	//���ڴ��е�ǰ��������͵�ǰ����
	api_ClrDemand( eClearRamDemand );

	// ��¼�����־
	BackupEnergyFlag( 0 );
}

//-----------------------------------------------
//��������: �����������������ʱ��
//
//����: 
//	Type[in]:	Bit15		1--���������������	0--�Ƕ��������������
//				P_ACTIVE	1
//				N_ACTIVE	2
//				P_RACTIVE	3
//				N_RACTIVE	4
//				RACTIVE1	5
//				RACTIVE2	6
//				RACTIVE3	7
//				RACTIVE4	8
//              APPARENT_P	10		// ��������
//				APPARENT_N	11		// ��������
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//  Ratio[in]:	����
//				1-MAX_RATIO	 ����������
//				0				 �������ܼ�             
//	Dot[in]:	С����λ��
//	Buf[out]:	�������
//
//����ֵ:	TRUE/FALSE

//��ע: 
//-----------------------------------------------
BOOL api_GetDemand(WORD Type, BYTE DataType, WORD Ratio, BYTE Dot, BYTE * Buf)
{		
	BOOL ReturnStatus;
	TDemandData DemandData;
	
	if(Type & 0x8000)
	{
		#if(SEL_FREEZE_PERIOD_DEMAND == YES)
		ReturnStatus = DbReadFreezePeriodDemandHex(Type&(~0x8000), Dot, (BYTE*)&DemandData);
		#else
		return FALSE;
		#endif
	}	
	else if( (Type == P_RACTIVE) || (Type == N_RACTIVE) )
	{
	    #if( SEL_REACTIVE_DEMAND == YES )
		ReturnStatus = DbReadPNRActiveDemand( Type, Ratio, Dot, (BYTE*)&DemandData );
        #else
        return FALSE;
        #endif
	}
	else
	{
		ReturnStatus = DbReadDemandBin( Type, Ratio, Dot, (BYTE*)&DemandData );
	}
	
	if( DataType == DATA_BCD )
	{
		if( DemandData.DemandValue < 0 )
		{
			DemandData.DemandValue *= -1;
		}
		DemandData.DemandValue = lib_DWBinToBCD( DemandData.DemandValue );
		
		DemandData.DemandTime.wYear = lib_WBinToBCD( DemandData.DemandTime.wYear );
		DemandData.DemandTime.Mon = lib_BBinToBCD( DemandData.DemandTime.Mon );
		DemandData.DemandTime.Day = lib_BBinToBCD( DemandData.DemandTime.Day );
		DemandData.DemandTime.Hour = lib_BBinToBCD( DemandData.DemandTime.Hour );
		DemandData.DemandTime.Min = lib_BBinToBCD( DemandData.DemandTime.Min );
		DemandData.DemandTime.Sec = lib_BBinToBCD( DemandData.DemandTime.Sec );
	}
	else if( DataType == DATA_HEX )
	{
		if( DemandData.DemandValue < 0 )
		{
			DemandData.DemandValue *= -1;
		}
	}
	else if( DataType == DATA_HEXCOMP )
	{
	}
	
	memcpy( Buf, (BYTE*)&DemandData, sizeof(TDemandData) );
	
	return ReturnStatus;
}


//-----------------------------------------------
//��������: ����ǰ����������֧�ַ���������
//
//����: 
//	Type[in]:	1  	�й�
//				2	�޹�
//				3	���� 
//	DataType[in]:	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)      
//	Dot[in]:	С����λ��
//	Len[in]:    ��ȡ��������
//	pDemand[out]:�������
//
//����ֵ:	��

//��ע: DATA_BCD��ʽ���λλ����λ
//-----------------------------------------------
void api_GetCurrDemandData(WORD Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE * pDemand)
{
	TFourByteUnion tl;
	BYTE BackFlag;
	BYTE Len1;
	if(Dot > 6)
	{
		Dot = 4;
	}
	Len1 = Len;
	if( (Len != 3)&&(Len != 4) ) 
	{
		Len1 = 4;
	}	
	
	tl.l = (long)DemandRam.CurrentDemand[Type-1];
	
	tl.l /= (long)lib_MyPow10(6-Dot);
	
	if( DataType == DATA_BCD )
	{
		BackFlag = 0;
		if( tl.l < 0 )
		{
			tl.l *= -1;
			BackFlag = 1;
		}
		tl.d = lib_DWBinToBCD( tl.d );
		
		//�������λ�÷���
		if( BackFlag == 1 )
		{
			tl.b[Len1 - 1] |= 0x80;
		}
	}
	else if( DataType == DATA_HEX )
	{
		if( tl.l < 0 )
		{
			tl.l *= -1;
		}
	}
	else if( DataType == DATA_HEXCOMP )
	{
		
	}
	
	memcpy( (BYTE*)pDemand, tl.b, Len );
}
//-----------------------------------------------
//��������: ��������
//
//����: 	eType[in]��		Ҫ������������
//      eClearRamDemand	�嵱ǰ���������ڴ��е�ǰ�������
//	    eClearAllDemand	��eeprom�е�ǰ�������		
//
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_ClrDemand( eClearDemandType eType )
{
	BYTE i;
	TDemandDBase TmpDemandDBase;
	
	if( (eType == eClearRamDemand) || (eType == eClearAllDemand) )//���RAM���������Լ��������
	{
		memset( (void *)&DemandRam.CurrentDemand, 0x00, sizeof(DemandRam.CurrentDemand) );
		memset( (void *)&DemandRam.TotalDemand, 0x00, sizeof(TDemandDBase) );
		DemandRam.TotalDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.TotalDemand), sizeof(TDemandDBase)-sizeof(DWORD));
		memset( (void *)&DemandRam.RatioDemand, 0x00, sizeof(TDemandDBase) );
		DemandRam.RatioDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
		#if(SEL_FREEZE_PERIOD_DEMAND == YES)
		memset( (void *)&DemandRam.FreezePeriodDemand, 0x00, sizeof(TDemandDBase) );
		DemandRam.FreezePeriodDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.FreezePeriodDemand), sizeof(TDemandDBase)-sizeof(DWORD));
		#endif
	}
	
	if( eType == eClearAllDemand )//���e2�е�����
	{
		memset( (void *)&TmpDemandDBase, 0x00, sizeof(TDemandDBase) );
		TmpDemandDBase.CRC32  = lib_CheckCRC32((BYTE *)&(TmpDemandDBase), sizeof(TDemandDBase)-sizeof(DWORD));
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( DemandSafeRom.TotalDemand ), sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
		
		for(i=0; i<MAX_RATIO; i++)
		{
			memset( (void *)&TmpDemandDBase, 0x00, sizeof(TDemandDBase) );
			TmpDemandDBase.CRC32  = lib_CheckCRC32((BYTE *)&(TmpDemandDBase), sizeof(TDemandDBase)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( DemandSafeRom.RatioDemand[i] ), sizeof(TDemandDBase), (BYTE *)&TmpDemandDBase );
			CLEAR_WATCH_DOG;
		}

		// ȷ���������ڡ�����
		CalPeriod();
		//�����������
		ClearDemandRamData();
		// ��¼�����־
		BackupEnergyFlag( 0 );
	}
}


#endif//#if( MAX_PHASE == 3 )
#endif