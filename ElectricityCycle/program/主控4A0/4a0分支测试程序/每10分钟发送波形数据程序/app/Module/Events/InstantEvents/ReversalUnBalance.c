//---------------------------------------------
// ���������¼  ��ƽ�⴦���¼
//---------------------------------------------
// ���µ�DLT645�У���ѹ�������������¼�͵�ѹ��������ƽ���¼�ĸ�ʽ���������ݻ���һ��
// ��ͬ���ж���ʼ������һ�£���ƽ���¼�ж���һ�����ƽ����
// ���Է���һ���д���
//---------------------------------------------

#include "AllHead.h"

// �����򡢲�ƽ�⴦��������
#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
static const DWORD ReversalUnBalanceOad[] =
{
#if( MAX_PHASE == 1)
	0x06,			   //����:6��	��ѹ��ƽ���ʣ�������ƽ���ʣ�������	�����¼�������������飬���ݲ�ͬ���¼�����ɾ��OAD
	0x00102201, 	   //�¼�����ʱ�������й��ܵ���
	0x00202201, 	   //�¼�����ʱ�̷����й��ܵ���
	0x20266200, 	   //�¼������ڼ��ѹ��ƽ����
	0x20276200,       //�¼������ڼ������ƽ����
	0x00108201, 	   //�¼������������й��ܵ���
	0x00208201, 	   //�¼����������й��ܵ���
#else
	0x22,			   //����:34��	��ѹ��ƽ���ʣ�������ƽ���ʣ�������	�����¼�������������飬���ݲ�ͬ���¼�����ɾ��OAD
	0x00102201, 	   //�¼�����ʱ�������й��ܵ���
	0x00202201, 	   //�¼�����ʱ�̷����й��ܵ���
	0x00302201, 	   //�¼�����ʱ������޹�1�ܵ���
	0x00402201, 	   //�¼�����ʱ������޹�2�ܵ���
	0x00112201, 	   //�¼�����ʱ��A�������й�����
	0x00212201, 	   //�¼�����ʱ��A�෴���й�����
	0x00312201, 	   //�¼�����ʱ��A������޹�1����
	0x00412201, 	   //�¼�����ʱ��A������޹�2����
	0x00122201, 	   //�¼�����ʱ��B�������й�����
	0x00222201, 	   //�¼�����ʱ��B�෴���й�����
	0x00322201, 	   //�¼�����ʱ��B������޹�1����
	0x00422201, 	   //�¼�����ʱ��B������޹�2����
	0x00132201, 	   //�¼�����ʱ��C�������й�����
	0x00232201, 	   //�¼�����ʱ��C�෴���й�����
	0x00332201, 	   //�¼�����ʱ��C������޹�1����
	0x00432201, 	   //�¼�����ʱ��C������޹�2����
	0x20266200, 	   //�¼������ڼ��ѹ��ƽ����
	0x20276200,       //�¼������ڼ������ƽ����
	0x00108201, 	   //�¼������������й��ܵ���
	0x00208201, 	   //�¼����������й��ܵ���
	0x00308201, 	   //�¼�����������޹�1�ܵ���
	0x00408201, 	   //�¼�����������޹�2�ܵ���
	0x00118201, 	   //�¼�������A�������й�����
	0x00218201, 	   //�¼�������A�෴���й�����
	0x00318201, 	   //�¼�������A������޹�1����
	0x00418201, 	   //�¼�������A������޹�2����
	0x00128201, 	   //�¼�������B�������й�����
	0x00228201, 	   //�¼�������B�෴���й�����
	0x00328201, 	   //�¼�������B������޹�1����
	0x00428201, 	   //�¼�������B������޹�2����
	0x00138201, 	   //�¼�������C�������й�����
	0x00238201, 	   //�¼�������C�෴���й�����
	0x00338201, 	   //�¼�������C������޹�1����
	0x00438201, 	   //�¼�������C������޹�2����
#endif
};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void FactoryInitReversalUnBalancePara( void )
{
	DWORD OAD_Temp = 0x00;
	
	#if( SEL_EVENT_V_REVERSAL == YES )
	api_WriteEventAttribute( 0x300f, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// ����

	OAD_Temp = 0x00622620, 	   //�¼������ڼ��ѹ��ƽ����
	api_DeleteEventAttribute(0x300f,(BYTE *)&OAD_Temp);
	OAD_Temp = 0x00622720,        //�¼������ڼ������ƽ����
	api_DeleteEventAttribute(0x300f,(BYTE *)&OAD_Temp);

	#endif
	
	#if( SEL_EVENT_I_REVERSAL == YES )
	api_WriteEventAttribute( 0x3010, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// ����

	OAD_Temp = 0x00622620, 	   //�¼������ڼ��ѹ��ƽ����
	api_DeleteEventAttribute(0x3010,(BYTE *)&OAD_Temp);
	OAD_Temp = 0x00622720,        //�¼������ڼ������ƽ����
	api_DeleteEventAttribute(0x3010,(BYTE *)&OAD_Temp);

	#endif
	
	#if( SEL_EVENT_V_UNBALANCE == YES )
	api_WriteEventAttribute( 0x301d, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// ʧ��

	OAD_Temp = 0x00622720,        //�¼������ڼ������ƽ����
	api_DeleteEventAttribute(0x301d,(BYTE *)&OAD_Temp);

	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	api_WriteEventAttribute( 0x301e, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// ����

	OAD_Temp = 0x00622620, 	   //�¼������ڼ��ѹ��ƽ����
	api_DeleteEventAttribute(0x301e,(BYTE *)&OAD_Temp);

	#endif
	
	#if( SEL_EVENT_OVER_V == YES )
	api_WriteEventAttribute( 0x302d, 0xFF, (BYTE *)&ReversalUnBalanceOad[0], sizeof(ReversalUnBalanceOad)/sizeof(DWORD) );					// ��ѹ

	OAD_Temp = 0x00622620, 	   //�¼������ڼ��ѹ��ƽ����
	api_DeleteEventAttribute(0x302d,(BYTE *)&OAD_Temp);

	#endif
}
//RAM���ݴ�����ƽ����
WORD RamUnBalanceRate[3];
//-----------------------------------------------
//��������: ���ص�ѹ��������ƽ���¼������ڼ����ƽ����
//
//����: Type[in]	:	D7 -- 0 -- RAM�еĲ�ƽ���ʣ�1 -- EEPROM�еĲ�ƽ����
//								0 -- ��ѹ��ƽ����
//								1 -- ������ƽ����
//								2 -- �������ز�ƽ����
//                    
//����ֵ:  	��Ӧ�Ĳ�ƽ����
//
//��ע:
//-----------------------------------------------
WORD GetUnBalanceMaxRate(BYTE Type)
{
	DWORD Addr = 0;
	WORD TempUnBalanceRate;
	if(Type & 0x80)
	{
		#if( SEL_EVENT_V_UNBALANCE == YES )
		if(Type == 0x80)
		{
			Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomVUnBalanceRate);
		}
		#endif
		#if( SEL_EVENT_I_UNBALANCE == YES )
		if(Type == 0x81)
		{
			Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomIUnBalanceRate);
		}
		#endif
		#if( SEL_EVENT_I_S_UNBALANCE == YES )
		if(Type == 0x82)
		{
			Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomISUnBalanceRate);
		}
		#endif
		api_ReadFromContinueEEPRom( Addr, sizeof(WORD), (BYTE *)&TempUnBalanceRate );
	}
	else
	{
		TempUnBalanceRate = RamUnBalanceRate[Type&0x0f];
	}
	return TempUnBalanceRate;
}
//-----------------------------------------------
//��������: ˢ�µ�ѹ��������ƽ���¼������ڼ�����ƽ����
//
//����: 			��
//                    
//����ֵ:  	��
//
//��ע:	�¼��������е���
//-----------------------------------------------
void FreshUnBalanceRate(void)
{
	WORD TempRate;
	#if( SEL_EVENT_V_UNBALANCE == YES )
	if( (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_V_UNBALANCE )) == FALSE )
	{
		RamUnBalanceRate[0] = 0;
	}
	else
	{
		TempRate = api_GetSampleStatus(0);
		if( TempRate > RamUnBalanceRate[0] )
		{
			RamUnBalanceRate[0] = TempRate;
		}
	}
	#endif

	#if( SEL_EVENT_I_UNBALANCE == YES )
	if( (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_I_UNBALANCE )) == FALSE )
	{
		RamUnBalanceRate[1] = 0;
	}
	else
	{
		TempRate = api_GetSampleStatus(1);
		if( TempRate > RamUnBalanceRate[1] )
		{
			RamUnBalanceRate[1] = TempRate;
		}
	}
	#endif

	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	if( (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_S_I_UNBALANCE )) == FALSE )
	{
		RamUnBalanceRate[2] = 0;
	}
	else
	{
		TempRate = api_GetSampleStatus(1);
		if( TempRate > RamUnBalanceRate[2] )
		{
			RamUnBalanceRate[2] = TempRate;
		}
	}
	#endif
}
//-----------------------------------------------
//��������: ����洢��ѹ��������ƽ���¼������ڼ�����ƽ����
//
//����: 			��
//                    
//����ֵ:  	��
//
//��ע:	����ʱ����
//-----------------------------------------------
void UnBalanceRatePowerOff(void)
{
	DWORD Addr = 0;
	
	#if( SEL_EVENT_V_UNBALANCE == YES )
	Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomVUnBalanceRate);
	api_WriteToContinueEEPRom( Addr, sizeof(WORD), (BYTE *)&RamUnBalanceRate[0] );
	#endif
	
	#if( SEL_EVENT_I_UNBALANCE == YES )
	Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomIUnBalanceRate);
	api_WriteToContinueEEPRom( Addr, sizeof(WORD), (BYTE *)&RamUnBalanceRate[1] );
	#endif
	
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	Addr = GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.RomISUnBalanceRate);
	api_WriteToContinueEEPRom( Addr, sizeof(WORD), (BYTE *)&RamUnBalanceRate[2] );
	#endif
}

//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣���ѹ������
//------------------------------------------------------------------------------------------------------
// ��ѹ��������
#if( SEL_EVENT_V_REVERSAL == YES )
// �жϵ�ѹ�������Ƿ���
BYTE GetVReversalStatus(BYTE Phase)
{
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		return 0;		
	}
	
	//����ܹ淶��Լ������ĳ���ѹ����60%UNʱ�����ٽ��е�ѹ��������ж�
	if( (api_GetSampleStatus(0x10)&0x8007) == 0x8000 )
	{
		return 1;
	}

	return 0;
}



#endif

//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣�����������
//------------------------------------------------------------------------------------------------------
// ������������
#if( SEL_EVENT_I_REVERSAL == YES )
// �жϵ����������Ƿ���
BYTE GetIReversalStatus(BYTE Phase)
{
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		return 0;		
	}
	//����ܹ淶��Լ���������ѹ�����ڵ��ܱ���ٽ��ѹ�� ������������� 5%����������������ж�
	if( (api_GetSampleStatus(0x10)&0x0007) != 0 )
	{
		return 0;
	}
	
	if( (api_GetSampleStatus(0x11)&0x8007) == 0x8000 )
	{
		return 1;
	}

	return 0;
}

#endif


//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣���ѹ��ƽ��
//------------------------------------------------------------------------------------------------------
// ��ѹ��ƽ�⴦��
#if( SEL_EVENT_V_UNBALANCE == YES )
// �жϵ�ѹ��ƽ���Ƿ���
BYTE GetVUnBalanceStatus(BYTE Phase)
{
	BYTE Status;
	WORD dw_LimitUnBalance_V;
	Status = 0;
	dw_LimitUnBalance_V = 0;

	//�����淶Ҫ��������һ���ѹ�����ٽ��ѹ
    if( MeterTypesConst == METER_3P3W )
    {
		if( (api_GetSampleStatus(0x10)&0x0005) == 0x05 )
		{
			return Status;
		}
    }
    else
    {
		if( (api_GetSampleStatus(0x10)&0x0007) == 0x07 )
		{
			return Status;
		}
    }
    
	// ��ѹ��ƽ������ֵ
	DealEventParaByEventOrder( READ, eEVENT_V_UNBALANCE_NO, eTYPE_WORD, (BYTE *)&dw_LimitUnBalance_V );
	//�¹淶����ȷ�����ѹ��ƽ������ֵΪ0�����¼�������
	if(dw_LimitUnBalance_V == 0)
	{
		return Status;
	}

	if(api_GetSampleStatus(0) > dw_LimitUnBalance_V)
	{
		Status = 1;
	}

	return Status;
}
#endif


//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣�������ƽ��
//------------------------------------------------------------------------------------------------------
// ������ƽ�⴦��
#if( SEL_EVENT_I_UNBALANCE == YES )
// �жϵ�����ƽ���Ƿ���
BYTE GetIUnBalanceStatus(BYTE Phase)
{
	BYTE Status;
	WORD dw_LimitUnBalance_I;
	Status = 0;
	dw_LimitUnBalance_I = 0;

	//�����淶Ҫ��������һ���������5%Ib
	if( MeterTypesConst == METER_3P3W )
    {
		if( (api_GetSampleStatus(0x11)&0x0005) == 0x05 )
		{
			return Status;
		}
    }
    else
    {
		if( (api_GetSampleStatus(0x11)&0x0007) == 0x07 )
		{
			return Status;
		}
    }
    
	// ��ѹ��ƽ������ֵ
	DealEventParaByEventOrder( READ, eEVENT_I_UNBALANCE_NO, eTYPE_WORD, (BYTE *)&dw_LimitUnBalance_I );
	//�¹淶����ȷ�����ѹ��ƽ������ֵΪ0�����¼�������
	if(dw_LimitUnBalance_I == 0)
	{
		return Status;
	}

	if(api_GetSampleStatus(1) > dw_LimitUnBalance_I)
	{
		Status = 1;
	}

	return Status;
}

#endif

//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣��������ز�ƽ���¼
//------------------------------------------------------------------------------------------------------
// �������ز�ƽ���¼
#if( SEL_EVENT_I_S_UNBALANCE == YES )
// �жϵ������ز�ƽ���¼�Ƿ���
BYTE GetISUnBalanceStatus(BYTE Phase)
{
	BYTE Status;
	WORD dw_LimitUnBalance_I;
	Status = 0;
	dw_LimitUnBalance_I = 0;

	//�����淶Ҫ��������һ���������5%Ib
	if( MeterTypesConst == METER_3P3W )
    {
		if( (api_GetSampleStatus(0x11)&0x0005) == 0x05 )
		{
			return Status;
		}
    }
    else
    {
		if( (api_GetSampleStatus(0x11)&0x0007) == 0x07 )
		{
			return Status;
		}
    }

	// ������ƽ������ֵ
	DealEventParaByEventOrder( READ, eEVENT_I_S_UNBALANCE_NO, eTYPE_WORD, (BYTE *)&dw_LimitUnBalance_I );
	//�¹淶����ȷ���������ƽ������ֵΪ0�����¼�������
	if(dw_LimitUnBalance_I == 0)
	{
		return Status;
	}

	if(api_GetSampleStatus(1) > dw_LimitUnBalance_I)
	{
		Status = 1;
	}

	return Status;
}
#endif
#endif//#if( (SEL_EVENT_V_REVERSAL==YES) || (SEL_EVENT_I_REVERSAL==YES) || (SEL_EVENT_V_UNBALANCE==YES) || (SEL_EVENT_I_UNBALANCE==YES) || (SEL_EVENT_I_S_UNBALANCE==YES) )
