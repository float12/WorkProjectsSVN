//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.30
//����		���������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------

#include "AllHead.h"


#if( MAX_PHASE == 3 )//ֻ�������Ŵ���������
#if( SEL_DEMAND_2022 == YES )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
//�������ڶԻ���ʱ��������
#define MAX_DEMANDPERIOD_MUM		60

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

typedef struct TDemandRam_t
{
	//���������
	TDemandDBase TotalDemand;
	
	//RAM�е�ǰ���ʵ��������
	TDemandDBase RatioDemand;
	
	//�����������������
	#if(SEL_FREEZE_PERIOD_DEMAND == YES)
	TDemandDBase FreezePeriodDemand;
	#endif
	
	//��ǰ���� �����й� �����й� 1~4�����޹�
	DWORD CurrentDemand[eMaxDemand6Type];
	
}TDemandRam;

//��ǰ��������ṹ��
typedef struct TDemandDir_t
{
	BOOL DemandBackFlag;//�Ƿ���(eDemandPForward = 0,//���ʷ���Ϊ����ö��ֵ��ҪΪ0,eDemandPReverse = 1,//���ʷ���Ϊ��) 
	BYTE ChangeCnt;//����ı��������ÿ��+1
}TDemandDir;
//��ǰ��������ṹ��
typedef struct TDemandCountData_t
{
	//���뵱ǰ���������洢���--ÿ�����ۼ�,�������գ����������
	DWORD DemandMinCnt[2];
	//��ǰ���ӵ��������ݣ���������
	WORD CurMinPulse[eMaxDemand6Type];
	//�����й��������й����������޹����������60�����ڵ���������
	WORD MinPulse[MAX_DEMANDPERIOD_MUM][eMaxDemand6Type];
}TDemandCountData;


// �������ڲ���
static BYTE PeriodReal;//�������ڷ�����//��ע��****lpk
// ����(����)�ķ�����
static WORD StepLength;

//����������ͣ��־
eDemandPauseFlag DemandPauseFlag;
//��������������
TDemandCountData DemandCountData;

TDemandDir DemandDir[2];//��ǰ��������[0]�й�������[1]�޹�����
//�ڴ��е�����
static TDemandRam DemandRam;

// �����õ�ǰ���ʣ��жϷ����л���
static BYTE RecordTariff;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static BOOL DbReadDemandBin(WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand);
static BOOL DbReadPNRActiveDemand( WORD Type, WORD Ratio, BYTE Dot, BYTE * pDemand );
static void CountDemand(BYTE tar);
static void CalPeriod( void );
static void ClearDemandCountData(void);

//-----------------------------------------------
//				��������
//-----------------------------------------------

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
	BYTE inDot;
	inDot = Dot;
	
	if( inDot > MAX_DEMAND_DOT )
	{
		inDot = DEFAULT_DEMAND_DOT;
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
		if( Dot == 6 )
		{
			TmpDemandData.DemandValue *= (long)lib_MyPow10(Dot-MAX_DEMAND_DOT);
		}
		else
		{
			TmpDemandData.DemandValue /= (long)lib_MyPow10(MAX_DEMAND_DOT-inDot);
		}
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
//��������: �������޹��������
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
	BYTE inDot;
	inDot = Dot;

	if(inDot > MAX_DEMAND_DOT)//�������С��λ������MAX_DEMAND_DOT,��������DEFAULT_DEMAND_DOTλС������
	{
		inDot = DEFAULT_DEMAND_DOT;
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
			if( Dot == 6 )
			{
				TmpDemandData2.DemandValue *= (long)lib_MyPow10(Dot-MAX_DEMAND_DOT);
			}
			else
			{
				TmpDemandData2.DemandValue /= (long)lib_MyPow10(MAX_DEMAND_DOT-inDot);
			}
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
//��������: ���µ�ǰ����״̬���������ʷ������������
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void UptCurDemandStatus( void )
{
	BYTE i, PDir[2];
	//���ú��෴���־
	if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ))
	{
		PDir[0] = eDemandPReverse;
	}
	else
	{
		PDir[0] = eDemandPForward;
	}
	//���ú��෴���־
	if(api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q ))
	{
		PDir[1] = eDemandPReverse;
	}
	else
	{
		PDir[1] = eDemandPForward;
	}

	for( i = 0 ; i < 2;i ++)
	{
		//�ж��й��������޹������ķ���仯,�����������仯ʱ�����м�ʱ
		if( DemandDir[i].DemandBackFlag != PDir[i])
		{
			//�������޷���ı����60������ʱ�����������л�
			if( DemandDir[i].ChangeCnt < 60 )
			{
				DemandDir[i].ChangeCnt ++;
			}
			else
			{
				DemandDir[i].DemandBackFlag = PDir[i];
				DemandDir[i].ChangeCnt = 0;
			}
		}
		else
		{
			DemandDir[i].ChangeCnt = 0;
		}
	}
}
void DemandSecTask( void )
{
	UptCurDemandStatus();
}

//-----------------------------------------------
//��������: ��������ķ�������ÿ����ִ��1��
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void DemandMinTask( void )
{
	BYTE i = 0;

	BYTE Cnt = 0;//�������ķ�

	if( DemandPauseFlag == eDemandPause )
	{
		//�ϵ硢���㡢�������ڱ��������ʱ����ʱ��DemandPauseFlag��λ��������һ������������
		DemandPauseFlag = eDemandRun;
		//������������ýṹ��
		memset( (BYTE*)&DemandCountData, 0x00, sizeof(DemandCountData) );
		return;
	}
	else
	{
		Cnt = (DemandCountData.DemandMinCnt[0]%MAX_DEMANDPERIOD_MUM);
		//����ͬ��
		memcpy((BYTE*)&DemandCountData.MinPulse[Cnt], (BYTE*)&DemandCountData.CurMinPulse, sizeof(DemandCountData.CurMinPulse));//�����
		memset((BYTE*)&DemandCountData.CurMinPulse, 0x00, sizeof(DemandCountData.CurMinPulse));

		for( i = 0; i < 2; i++)
		{
			if(DemandCountData.DemandMinCnt[i] != 0xffffffff)
			{
				DemandCountData.DemandMinCnt[i]++;
			}
			else
			{
				DemandCountData.DemandMinCnt[i] = MAX_DEMANDPERIOD_MUM+(DemandCountData.DemandMinCnt[i]%MAX_DEMANDPERIOD_MUM)+1;
			}
		}
	}
	//����ͬ���������ڲ���������ʱ��
	CalPeriod();

	#if(SEL_FREEZE_PERIOD_DEMAND == YES)
	DealFreezePeriodDemand();
	#endif
	//��¼������
	if( ( DemandCountData.DemandMinCnt[0] >= PeriodReal )
	&& ( (DemandCountData.DemandMinCnt[0] % StepLength) == 0))
	{
		CountDemand(0);//���㣬i��Ӧ�ܻ�ǰ��������

		api_MultiFunPortCtrl( eDEMAND_PERIOD_OUTPUT );

	}
	//��ǰ��������
	if( ( DemandCountData.DemandMinCnt[0] >= PeriodReal )
	&& ( DemandCountData.DemandMinCnt[1] >= PeriodReal )
	&& ( DemandCountData.DemandMinCnt[1] % StepLength == 0) )
	{
		CountDemand(1);//���㣬i��Ӧ�ܻ�ǰ��������

	}
}

//-----------------------------------------------
//��������: ���������������
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_DemandTask( void )
{
	if( api_GetTaskFlag( eTASK_DEMAND_ID, eFLAG_SECOND ) == TRUE )
	{
		DemandSecTask();//����������
		api_ClrTaskFlag(eTASK_DEMAND_ID,eFLAG_SECOND);//���������־
	}
	
	if( api_GetTaskFlag( eTASK_DEMAND_ID, eFLAG_MINUTE ) == TRUE )
	{
		DemandMinTask();//������������
		api_ClrTaskFlag(eTASK_DEMAND_ID,eFLAG_MINUTE);//����ӱ�־
	}
	
	//���ʷ����仯
	if( RecordTariff != api_GetCurRatio() )//����ʵĽṹ��
	{
		DemandCountData.DemandMinCnt[1] = 0;//��������������
		memset( &DemandRam.RatioDemand, 0x00, sizeof(DemandRam.RatioDemand) );
		DemandRam.RatioDemand.CRC32  = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
        //��¼����
		RecordTariff = api_GetCurRatio();
	}

	
}


//-----------------------------------------------
//��������: ���ܻ�ַ��������������
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
	BYTE inDot;
	inDot = Dot;
	
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
	
	if( inDot > MAX_DEMAND_DOT)
	{
		inDot = DEFAULT_DEMAND_DOT;
	}

	pDemandData = (BYTE *)&TmpDemandDBase;
	pDemandData += GetTDemandDbaseOffset( Type );
	
	if( inDot == DEFAULT_DEMAND_DOT )//С��λ��Ϊ4λ���н�λ
	{   
	    //�����ش򿪣������һλ��λ����������
    	if( api_GetProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) == TRUE )
    	{
    	    if( (((TDemandData *)pDemandData)->DemandValue) != 0 )
    	    {
    		    ((TDemandData *)pDemandData)->DemandValue += (5*(MAX_DEMAND_DOT-DEFAULT_DEMAND_DOT));
    	    }
    	}
	}

	DemandValue = (((TDemandData *)pDemandData)->DemandValue);
	
	if(  DemandValue != 0xffffffff )
	{
		if( Dot == 6 ) //����С��λ��Ϊ6λʱ����������10��������С��λ������6λ����4λ����
		{
			((TDemandData *)pDemandData)->DemandValue *= (long)lib_MyPow10(Dot-MAX_DEMAND_DOT);
		}
		else
		{
			((TDemandData *)pDemandData)->DemandValue /= (long)lib_MyPow10(MAX_DEMAND_DOT-inDot);
		}
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
		if(DemandRam.RatioDemand.CRC32 != lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD)))
		{
			memset( (void *)&DemandRam.RatioDemand, 0x00, sizeof(TDemandDBase) );
			DemandRam.RatioDemand.CRC32 = lib_CheckCRC32((BYTE *)&(DemandRam.RatioDemand), sizeof(TDemandDBase)-sizeof(DWORD));
		}
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
		if(DemandRam.TotalDemand.CRC32 != lib_CheckCRC32((BYTE *)&(DemandRam.TotalDemand), sizeof(TDemandDBase)-sizeof(DWORD)))
		{
			memset( (void *)&DemandRam.TotalDemand, 0x00, sizeof(TDemandDBase) );
			DemandRam.TotalDemand.CRC32 = lib_CheckCRC32((BYTE *)&(DemandRam.TotalDemand), sizeof(TDemandDBase)-sizeof(DWORD));
		}
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
		DbWriteFreezePeriodDemand( Type, pDemand );//@@�����ڼ�������δ�������޸�
		#endif
		
		//д�뵱ǰ������˲ʱ����
		switch( Type )
		{
		case P_ACTIVE:
			DemandRam.CurrentDemand[ePActiveDemand] = pDemand->DemandValue;
			break;
		case N_ACTIVE:
			DemandRam.CurrentDemand[eNActiveDemand] = pDemand->DemandValue;
			break;
		#if( SEL_REACTIVE_DEMAND == YES )
		case RACTIVE1:
			DemandRam.CurrentDemand[eReactive1Demand] = pDemand->DemandValue;
			break;
		case RACTIVE2:
			DemandRam.CurrentDemand[eReactive2Demand] = pDemand->DemandValue;
			break;
		case RACTIVE3:
			DemandRam.CurrentDemand[eReactive3Demand] = pDemand->DemandValue;
            break;
		case RACTIVE4:
			DemandRam.CurrentDemand[eReactive4Demand] = pDemand->DemandValue;
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
//����:		tar[in]		�������� 0:�ܵ�  1:�����ʵ�
//                    
//����ֵ: 	��
//
//��ע: ��������6λС����EEPROM  
//-----------------------------------------------
static void CountDemand(BYTE tar)
{
	BYTE i = 0;
	BYTE j = 0;
	TDemandData	TmpDemandData;
	WORD Type = 0,tRatio = 0;
	DWORD dwTmpDemand = 0;
	WORD wTmpValidTime = 0;
	DWORD dwActiveConstant = 0;

	memset( &TmpDemandData, 0, sizeof(TmpDemandData) );
	//�쳣����
	if(DemandCountData.DemandMinCnt[0] < PeriodReal)
	{
		DemandCountData.DemandMinCnt[0] = PeriodReal;
	}
	 
	for(i=0; i<eMaxDemand6Type; i++)
	{
		dwTmpDemand = 0;
		for( j = 0; j < PeriodReal; j++ )
		{
		
			dwTmpDemand += (DWORD)(DemandCountData.MinPulse[(DemandCountData.DemandMinCnt[0]-j-1)%MAX_DEMANDPERIOD_MUM][i]);//���������ڵ�������������
		}
		wTmpValidTime = (WORD)PeriodReal * 60;//���������ڳ��𶯹���ʱ�䣬��λ��

		dwActiveConstant = api_GetActiveConstant();//���峣��

		//����=(����������/���峣��)/�������ڣ���λKw������-MAX_DEMAND_DOT
		//lib_MyPow10(MAX_DEMAND_DOT)������(-MAX_DEMAND_DOT)
		//3600,Kwh����ΪKws
		if( (wTmpValidTime != 0) && (dwActiveConstant != 0) )
		{
			TmpDemandData.DemandValue = (DWORD)( ( ((QWORD)dwTmpDemand * (lib_MyPow10(MAX_DEMAND_DOT)) * 3600) / dwActiveConstant ) / wTmpValidTime );
		}
		else
		{
			TmpDemandData.DemandValue = 0;
		}

		api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );

		//��������
		Type = (i<2) ? (i+1) : (i+3);
		tRatio = (tar == 0) ? 0 : RecordTariff;
		DbWriteDemand(Type, tRatio, &TmpDemandData );
	
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
	if( Flag == FALSE )
	{
		//RAM�еĲ��Բ��ñ�����Ĭ�ϲ���
		PeriodReal = EnereyDemandModeConst.DemandPeriod;
		StepLength = EnereyDemandModeConst.DemandSlip;
	}
	else
	{
		//Ϊ�˱��������뻬��ʱ�䲻�������������
		//��������15���ӻ���7���ӣ���������14�֣�21�ֳ�����
		PeriodReal = (Period/Step)*Step;
		StepLength = Step;
	}
}

//-----------------------------------------------
//��������: ��RAM�е���������������
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   ���ϵ缰��Լ������ʼ���Լ�Уʱ��ʱ��������ʱ����
//-----------------------------------------------
static void ClearDemandCountData(void)
{
	BYTE i = 0;
	WORD EnergyType = 0;
	
	//�й��������޹������ķ���Ĭ��Ϊ��
	for( i = 0; i < (sizeof(DemandDir)/sizeof(TDemandDir)); i++ )
	{
		DemandDir[i].DemandBackFlag = eDemandPForward;//���ʷ���Ϊ��
		DemandDir[i].ChangeCnt = 0;
	}

	//����60��������
	memset( (BYTE*)&DemandCountData, 0x00, sizeof(DemandCountData) );
}
//-----------------------------------------------
//��������: ����ģ���ʼ��
//
//����:		InDemandPauseFlag[in]����ͣ����������־
//                    
//����ֵ:     ��
//
//��ע:   
//-----------------------------------------------
void api_InitDemand( void )
{
	// ȷ���������ڡ�����
	CalPeriod();
	
	ClearDemandCountData();

	//���ڴ��е�ǰ��������͵�ǰ����
	api_ClrDemand( eClearRamDemand, eDemandPause);

	// ��¼����
	RecordTariff = api_GetCurRatio();
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
	api_InitDemand();
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
	BYTE DataSign = 0x00;
	BYTE DemandIndex = 0;//Ҫ��ȡ�ĵ�ǰ�����������е�λ��
	BYTE TmpQuadrant = 0;
	BYTE BackFlag = 0;//�����־
	BYTE inDot;//�����־
	inDot = Dot;
	tl.l = 0;

	if( inDot > MAX_DEMAND_DOT )
	{
		inDot = DEFAULT_DEMAND_DOT;
	}
	
	if( (Len != 3)&&(Len != 4) ) 
	{
		return;//���ȷǷ�ʱ����дpDemand
	}

	if( (Type<1) || (Type>2) ) 
	{
		memset( (BYTE*)pDemand, 0, Len );
        return;
	}

	DataSign = 0;
	//�ж��й������Ƿ���
	if( DemandDir[0].DemandBackFlag == eDemandPReverse )
	{
		DataSign |= 0x08;
		BackFlag = 1;
		DemandIndex = eNActiveDemand;
	}
	else
	{
		BackFlag = 0;
		DemandIndex = ePActiveDemand;
	}

	if( Type == 2 )
	{
		//�ж��޹������Ƿ���
		if( DemandDir[1].DemandBackFlag == eDemandPReverse )
		{
			DataSign |= 0x80;
			BackFlag = 1;
		}
		else
		{
			BackFlag = 0;
		}

		switch( DataSign )
		{
		case 0x00://һ����
			DemandIndex = eReactive1Demand;
			break;
		case 0x08://������
			DemandIndex = eReactive2Demand;
			break;
		case 0x88://������
			DemandIndex = eReactive3Demand;
			break;
		case 0x80://������
			DemandIndex = eReactive4Demand;
			break;
		default:
			DemandIndex = eReactive1Demand;
			break;
		}
	}
	
	//DemandRam.CurrentDemand������Ϊ�޷�����
	tl.l = (long)DemandRam.CurrentDemand[DemandIndex];
	if(Dot == 6) //����С��λ��Ϊ6λʱ����������10��������С��λ������6λ����4λ����
	{
		tl.l *= (long)lib_MyPow10(Dot-MAX_DEMAND_DOT);
	}
	else
	{
		tl.l /= (long)lib_MyPow10(MAX_DEMAND_DOT-inDot);
	}

	if( tl.l == 0 )//�����ֵΪ0����0���أ�������-0
	{
		BackFlag = 0;
	}
	
	if( BackFlag )
	{
		tl.l *= (-1);
	}

	if( DataType == DATA_BCD )
	{
		if( BackFlag )
		{
			tl.l *= -1;
		}
		tl.d = lib_DWBinToBCD( tl.d );
		
		//�������λ�÷���
		if( BackFlag )
		{
			tl.b[Len - 1] |= 0x80;
		}
	}
	else if( DataType == DATA_HEX )
	{
		if( BackFlag )
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
//��������: ��RAM�л����6�ֵ�ǰ�����������������й��������й���1~4���ޣ���֧�ַ���������
//
//����: 
//	Type[in]:	�������ͣ����������й��������й���1~4�����޹�
//				
//	DataType[in]:	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)      
//	Dot[in]:	С����λ��
//	Len[in]:    ��ȡ��������
//	pDemand[out]:�������
//
//����ֵ:	��

//��ע: DATA_BCD��ʽ���λλ����λ
//-----------------------------------------------
void api_Get6TypeCurrDemandData(eDemand6Type Type, BYTE DataType, BYTE Dot, BYTE Len, BYTE * pDemand)
{
	DWORD dwDemand = 0;

	if( (Dot > MAX_DEMAND_DOT)||(Dot < 3) )
	{
		Dot = 4;
	}
	
	if( (Len != 3)&&(Len != 4) )
	{
		return;//���ȷǷ�ʱ����дpDemand
	}

	if( Type >= eMaxDemand6Type ) 
	{
		memset( (BYTE*)pDemand, 0, Len );
        return;
	}
	
	//DemandRam.CurrentDemand������Ϊ�޷�����
	dwDemand = (DWORD)DemandRam.CurrentDemand[Type];
	dwDemand /= (DWORD)lib_MyPow10(MAX_DEMAND_DOT-Dot);

	if( DataType == DATA_BCD )
	{
		dwDemand = lib_DWBinToBCD( dwDemand );
	}
	
	memcpy( (BYTE*)pDemand, &dwDemand, Len );
}

//-----------------------------------------------
//��������: ��������
//
//����: 	eType[in]��		Ҫ������������
//      eClearRamDemand	�嵱ǰ���������ڴ��е�ǰ�������
//	    eClearAllDemand	��eeprom�е�ǰ�������		
//
//			InDemandPauseFlag[in]: ��ͣ����������־
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_ClrDemand( eClearDemandType eType, eDemandPauseFlag InDemandPauseFlag )
{
	BYTE i = 0;
	TDemandDBase TmpDemandDBase;
	memset( &TmpDemandDBase, 0, sizeof(TmpDemandDBase) );
	
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
		ClearDemandCountData();
		RecordTariff = api_GetCurRatio();
	}

	DemandPauseFlag = InDemandPauseFlag;
}
//-----------------------------------------------
//��������: ����6������ͨ����Ӧ�ĵ�������������ʱ������
//
//����:		Type[in]��	ePActiveDemand --�����й�
//						eNActiveDemand --�����й�
//						eReactive1Demand --1�����޹�
//						eReactive2Demand --2�����޹�
//						eReactive3Demand --3�����޹�
//						eReactive4Demand --4�����޹�
//			PulseNum[in]:		��������
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_UpdateDemandEnergyPulse( eDemand6Type Type, WORD PulseNum )
{
	if(Type>=eMaxDemand6Type)//����ֻ��6ͨ��
	{
		return;
	}
	//��������������1000���峣���£�60A��1min�ĵ�������Ϊ660��20000���峣����6A1min��������Ϊ1154
	if(PulseNum > 5000)//�ؿڱ�1000000,9A-1minΪ3.3��
	{
		return;
	}
	//�������������ܷ���
	if( (DemandCountData.CurMinPulse[Type] +PulseNum) < 60000)
	{
		DemandCountData.CurMinPulse[Type] += PulseNum;
	}
}
#endif//#if( MAX_PHASE == 3 )
#endif
