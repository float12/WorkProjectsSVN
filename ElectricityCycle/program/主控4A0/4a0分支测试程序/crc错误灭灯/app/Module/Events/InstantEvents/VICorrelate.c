//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.10.27
//����		���ѹ��������ص��¼���¼
//�޸ļ�¼	
//---------------------------------------------------------------------- 
#include "AllHead.h"
#include "VICorrelate.h"

#if((SEL_EVENT_LOST_V==YES)||(SEL_EVENT_WEAK_V==YES)||(SEL_EVENT_OVER_V==YES)||(SEL_EVENT_BREAK==YES)||(SEL_EVENT_LOST_I==YES)||(SEL_EVENT_OVER_I==YES)||(SEL_EVENT_BREAK_I==YES))
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------



//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static const DWORD VICorrelateOad[] =
{
#if( MAX_PHASE == 1)
	0x08,				// ����
	0x00102201,	// ����ʱ�������й��ܵ���
	0x00202201, // ����ʱ�̷����й��ܵ���
	0x20002200, // ����ʱ�̵�ѹ
	0x20012200, // ����ʱ�̵���
	0x20042200, // ����ʱ���й�����
	0x200A2200, // ����ʱ�̹�������
	0x00108201,	// ����ʱ�������й��ܵ���
	0x00208201  // ����ʱ�̷����й��ܵ���	
#else
//	0x26,				// ����
//	0x00102201,	// ����ʱ�������й��ܵ���
//	0x00202201, // ����ʱ�̷����й��ܵ���
//	0x00302201,	// ����ʱ������޹�1�ܵ���
//	0x00402201, // ����ʱ������޹�2�ܵ���
//          
//	0x00112201,	// ����ʱ��A�������й�����
//	0x00212201, // ����ʱ��A�෴���й�����
//	0x00312201,	// ����ʱ��A������޹�1����
//	0x00412201, // ����ʱ��A������޹�2����
//          
//	0x00122201,	// ����ʱ��B�������й�����
//	0x00222201, // ����ʱ��B�෴���й�����
//	0x00322201,	// ����ʱ��B������޹�1����
//	0x00422201, // ����ʱ��B������޹�2����
//          
//	0x00132201,	// ����ʱ��C�������й�����
//	0x00232201, // ����ʱ��C�෴���й�����
//	0x00332201,	// ����ʱ��C������޹�1����
//	0x00432201, // ����ʱ��C������޹�2����
//          
//	0x20002200, // ����ʱ�̵�ѹ
//	0x20012200, // ����ʱ�̵���
//	0x20042200, // ����ʱ���й�����
//	0x20052200, // ����ʱ���޹�����
//	0x200A2200, // ����ʱ�̹�������
//	
//	0x20296200,	// �¼������ڼ䰲ʱֵ
//          
//	0x00108201,	// ����ʱ�������й��ܵ���
//	0x00208201, // ����ʱ�̷����й��ܵ���	
//	0x00308201,	// ����ʱ������޹�1�ܵ���
//	0x00408201, // ����ʱ������޹�2�ܵ���	
//          
//	0x00118201,	// ����ʱ��A�������й�����
//	0x00218201, // ����ʱ��A�෴���й�����
//	0x00318201,	// ����ʱ��A������޹�1����
//	0x00418201, // ����ʱ��A������޹�2����
//          
//	0x00128201,	// ����ʱ��B�������й�����
//	0x00228201, // ����ʱ��B�෴���й�����
//	0x00328201,	// ����ʱ��B������޹�1����
//	0x00428201, // ����ʱ��B������޹�2����
//          
//	0x00138201,	// ����ʱ��C�������й�����
//	0x00238201, // ����ʱ��C�෴���й�����
//	0x00338201,	// ����ʱ��C������޹�1����
//	0x00438201, // ����ʱ��C������޹�2����

	0x04,				// ����
	0x00102201,	// ����ʱ�������й��ܵ���
	0x00202201, // ����ʱ�̷����й��ܵ���
	0x00108201,	// ����ʱ�������й��ܵ���
	0x00208201  // ����ʱ�̷����й��ܵ���	

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
void FactoryInitVICorrelatePara( void )
{	
    DWORD OAD_Temp;

	#if( SEL_EVENT_OVER_I == YES )
	api_WriteEventAttribute( 0x3005, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// ����

//    OAD_Temp = 0x00622920,     //�¼������ڼ䰲ʱֵ
//    api_DeleteEventAttribute(0x3005,(BYTE *)&OAD_Temp);
	#endif
	
	#if( SEL_EVENT_BREAK_I == YES )
	api_WriteEventAttribute( 0x3006, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// ����

//	OAD_Temp = 0x00622920,     //�¼������ڼ䰲ʱֵ
//    api_DeleteEventAttribute(0x3006,(BYTE *)&OAD_Temp);
	#endif
	
	#if( SEL_EVENT_LOST_I == YES )
	api_WriteEventAttribute( 0x3004, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// ʧ��

//    OAD_Temp = 0x00622920,     //�¼������ڼ䰲ʱֵ
//    api_DeleteEventAttribute(0x3004,(BYTE *)&OAD_Temp);
	#endif
	
	#if( SEL_EVENT_BREAK == YES )
	api_WriteEventAttribute( 0x3003, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// ����
	#endif
	
	#if( SEL_EVENT_OVER_V == YES )
	api_WriteEventAttribute( 0x3002, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// ��ѹ
	#endif
	
	#if( SEL_EVENT_WEAK_V == YES )
	api_WriteEventAttribute( 0x3001, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// Ƿѹ
	#endif
	
	#if( SEL_EVENT_LOST_V == YES )
	api_WriteEventAttribute( 0x3000, 0xFF, (BYTE *)&VICorrelateOad[0], sizeof(VICorrelateOad)/sizeof(DWORD) );// ʧѹ
	#endif
}

// ʧѹ����
#if( SEL_EVENT_LOST_V == YES )
// ʧѹ��¼��״̬���
BYTE GetLostVStatus(BYTE Phase)
{
	WORD TypeI,TypeV;
	WORD w_RemoteV,w_Limit_V,w_Limit_V_UP;
	DWORD dw_RemoteI,dw_Limit_I;

	static BYTE Status[3] = {0,0,0};

	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}

	if( (Phase == 0) || (Phase > 3) )
	{
		//ֻ��A��B��C����
		return 0;//Status
	}
	#if( SEL_EVENT_LOST_ALL_V == YES )
    //�������ȫʧѹ״̬ ��������ʧѹ״̬
	if (SubEventInfoTab[eSUB_EVENT_LOST_ALL_V].pGetEventStatus(SubEventInfoTab[eSUB_EVENT_LOST_ALL_V].Phase) == 1 )
	{
        return 0;
	}
    #endif
	Phase --;
	// ʧѹ�¼���ѹ��������
	DealEventParaByEventOrder( READ, eEVENT_LOST_V_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
	//�¹淶����ȷ���ʧѹ�¼���ѹ��������Ϊ0�����¼�������
	if(w_Limit_V == 0)
	{
		Status[Phase] = 0;//
		return Status[Phase];//Status;
	}

	// ʧѹ�¼�������������
	DealEventParaByEventOrder( READ, eEVENT_LOST_V_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I );
	// ʧѹ�¼���ѹ�ָ�����
	DealEventParaByEventOrder( READ, eEVENT_LOST_V_NO, eTYPE_SPEC1, (BYTE *)&w_Limit_V_UP );

	if( Phase == 0 )
	{
		TypeI = PHASE_A+REMOTE_I;
		TypeV = PHASE_A+REMOTE_U;
	}
	else if( Phase == 1 )
	{
		TypeI = PHASE_B+REMOTE_I;
		TypeV = PHASE_B+REMOTE_U;	
	}
	else
	{
		TypeI = PHASE_C+REMOTE_I;
		TypeV = PHASE_C+REMOTE_U;
	}
	api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);
	api_GetRemoteData(TypeV, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);

	// �������ڵ�����������
	if( dw_RemoteI > dw_Limit_I )
	{
		if( (w_RemoteV < w_Limit_V)  
		|| ( api_DealEventStatus( eGET_EVENT_STATUS, (eSUB_EVENT_LOSTV_A+Phase) )	&& (w_RemoteV <=w_Limit_V_UP) ))
		{
			// ʧѹ
			Status[Phase] = 1;
		}
		else
		{
			Status[Phase] = 0;//��ѹ���ڵ�ѹ�ָ����ޣ���ʱ�����ǵ���
		}
	}
	else
	{
		Status[Phase] = 0;//
	}
	
	return Status[Phase];
}

#endif//#if( SEL_EVENT_LOST_V == YES )

//Ƿѹ����
#if(SEL_EVENT_WEAK_V==YES)
// Ƿѹ��¼��״̬���
BYTE GetWeakVStatus(BYTE Phase)
{
	BYTE Status;
	WORD Type;
	WORD w_RemoteV,w_Limit_V;

	Status = 0;

	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}

	if( (Phase == 0) || (Phase > 3) )
	{
		//ֻ��A��B��C����
		return Status;
	}
	Phase --;
	// Ƿѹ�¼���ѹ��������
	DealEventParaByEventOrder( READ, eEVENT_WEAK_V_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
	//�¹淶����ȷ���Ƿѹ�¼���ѹ��������Ϊ0�����¼�������
	if(w_Limit_V == 0)
	{
		return Status;
	}

	if( Phase == 0 )
	{
		Type = PHASE_A+REMOTE_U;
	}
	else if( Phase == 1 )
	{
		Type = PHASE_B+REMOTE_U;	
	}
	else
	{
		Type = PHASE_C+REMOTE_U;
	}
	api_GetRemoteData(Type, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);

	// �����ѹС�ڵ�ѹ��������
	if( w_RemoteV < w_Limit_V )
	{
		// Ƿѹ
		Status = 1;
	}
	return Status;
}

#endif//#if(SEL_EVENT_WEAK_V==YES)

//��ѹ����
#if(SEL_EVENT_OVER_V==YES)
// ��ѹ��¼��״̬���
BYTE GetOverVStatus(BYTE Phase)
{
	BYTE Status;
	WORD Type;
	WORD w_RemoteV,w_Limit_V;

	Status = 0;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	if( (Phase == 0) || (Phase > 3) )
	{
		//ֻ��A��B��C����
		return Status;
	}
	Phase --;
	// ��ѹ�¼���ѹ��������
	DealEventParaByEventOrder( READ, eEVENT_OVER_V_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
	//�¹淶����ȷ�����ѹ�¼���ѹ��������Ϊ0�����¼�������
	if(w_Limit_V == 0)
	{
		return Status;
	}

	if( Phase == 0 )
	{
		Type = PHASE_A+REMOTE_U;
	}
	else if( Phase == 1 )
	{
		Type = PHASE_B+REMOTE_U;	
	}
	else
	{
		Type = PHASE_C+REMOTE_U;
	}
	api_GetRemoteData(Type, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);

	// �����ѹ���ڵ�ѹ��������
	if( w_RemoteV > w_Limit_V )
	{
		// ��ѹ
		Status = 1;
	}
	return Status;
}

#endif//#if(SEL_EVENT_OVER_V==YES)

//���ദ��
#if(SEL_EVENT_BREAK==YES)
// �����¼��״̬���
BYTE GetBreakStatus(BYTE Phase)
{
	BYTE Status;
	WORD TypeU,TypeI;
	WORD w_RemoteV,w_Limit_V;
	DWORD dw_RemoteI,dw_Limit_I;

	Status = 0;

	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}

	if( (Phase == 0) || (Phase > 3) )
	{
		//ֻ��A��B��C����
		return Status;
	}
    //������ڵ���״̬����ѹС��60%�����������
	if (SubEventInfoTab[eSUB_EVENT_LOST_POWER].pGetEventStatus(SubEventInfoTab[eSUB_EVENT_LOST_POWER].Phase) == 1 )
	{
        return Status;
	}
	
	Phase --;
	// �����¼���ѹ��������
	DealEventParaByEventOrder( READ, eEVENT_BREAK_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
	//�¹淶����ȷ��������¼���ѹ��������Ϊ0�����¼�������
	if(w_Limit_V == 0)
	{
		return Status;
	}

	// �����¼�������������
	DealEventParaByEventOrder( READ, eEVENT_BREAK_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I );
	if( Phase == 0 )
	{
		TypeU = PHASE_A+REMOTE_U;
		TypeI = PHASE_RA+REMOTE_I;
	}
	else if( Phase == 1 )
	{
		TypeU = PHASE_B+REMOTE_U;
		TypeI = PHASE_RB+REMOTE_I;
	}
	else
	{
		TypeU = PHASE_C+REMOTE_U;
		TypeI = PHASE_RC+REMOTE_I;
	}
	api_GetRemoteData(TypeU, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);
	api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);

	// �����ѹС�ڵ�ѹ��������
	if( w_RemoteV < w_Limit_V )
	{
		if( dw_RemoteI < dw_Limit_I )
		{
			// ����
			Status = 1;
		}
	}
	return Status;
}

#endif//#if(SEL_EVENT_BREAK==YES)

//ʧ������
#if(SEL_EVENT_LOST_I==YES)
// ʧ����¼��״̬���
BYTE GetLostIStatus(BYTE Phase)
{
	BYTE i,Status;
	WORD TypeU,TypeI;
	WORD w_RemoteV,w_Limit_V;
	DWORD dw_RemoteI,dw_Limit_I,dw_Limit_I_Up;

	Status = 0;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	if( (Phase == 0) || (Phase > 3) )
	{
		//ֻ��A��B��C����
		return Status;
	}
	Phase --;
	// ʧ���¼�������������
	DealEventParaByEventOrder( READ, eEVENT_LOST_I_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I_Up );
	//�¹淶����ȷ���ʧ���¼�������������Ϊ0�����¼�������
	if(dw_Limit_I_Up == 0)
	{
		return Status;
	}
	
	// ʧ���¼�������������
	DealEventParaByEventOrder( READ, eEVENT_LOST_I_NO, eTYPE_SPEC1, (BYTE *)&dw_Limit_I );

	for( i = 0; i < MAX_PHASE; i++ )
	{
		if( i == 0 )
		{
			TypeI = PHASE_RA+REMOTE_I;
		}
		else if( i == 1 )
		{
			if( MeterTypesConst == METER_3P3W )
			{
				continue;
			}	
			TypeI = PHASE_RB+REMOTE_I;
		}
		else
		{
			TypeI = PHASE_RC+REMOTE_I;
		}
		api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);

		// ����������ڵ�����������
		if( dw_RemoteI > dw_Limit_I )
		{
			Status = 1;
			break;
		}
	}
	
	// ��������һ����ڵ�����������
	if( Status != 0 )
	{
		Status = 0;
		// ʧ���¼���ѹ��������
		DealEventParaByEventOrder( READ, eEVENT_LOST_I_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );
		if( Phase == 0 )
		{
			TypeU = PHASE_A+REMOTE_U;
			TypeI = PHASE_RA+REMOTE_I;
		}
		else if( Phase == 1 )
		{
			if( MeterTypesConst == METER_3P3W )
			{
				return 0;
			}
			TypeU = PHASE_B+REMOTE_U;
			TypeI = PHASE_RB+REMOTE_I;
		}
		else
		{
			TypeU = PHASE_C+REMOTE_U;
			TypeI = PHASE_RC+REMOTE_I;
		}
		api_GetRemoteData(TypeU, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);
		api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);

		// �����ѹ���ڵ�ѹ��������
		if( w_RemoteV > w_Limit_V )
		{
			//�������С�ڵ�����������
			if( dw_RemoteI < dw_Limit_I_Up )
			{
				// ʧ��
				return 1;
			}
		}	
	}
	else// �����������������ڵ�����������
	{
		Status = 0;
	}	

	return Status;
}

#endif//#if(SEL_EVENT_LOST_I==YES)

//��������
#if(SEL_EVENT_BREAK_I==YES)
// ������¼��״̬���
BYTE GetBreakIStatus(BYTE Phase)
{
	BYTE Status;
	WORD TypeU,TypeI;
	WORD w_RemoteV,w_Limit_V;
	DWORD dw_RemoteI,dw_Limit_I;

	Status = 0;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	if( (Phase == 0) || (Phase > 3) )
	{
		//ֻ��A��B��C����
		return Status;
	}
	Phase --;
	// �����¼�������������
	DealEventParaByEventOrder( READ, eEVENT_BREAK_I_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I );
	//�¹淶����ȷ��������¼�������������Ϊ0�����¼�������
	if(dw_Limit_I == 0)
	{
		return Status;
	}

	// �����¼���ѹ��������
	DealEventParaByEventOrder( READ, eEVENT_BREAK_I_NO, eTYPE_WORD, (BYTE *)&w_Limit_V );

	if( Phase == 0 )
	{
		TypeU = PHASE_A+REMOTE_U;
		TypeI = PHASE_RA+REMOTE_I;
	}
	else if( Phase == 1 )
	{
		TypeU = PHASE_B+REMOTE_U;
		TypeI = PHASE_RB+REMOTE_I;
	}
	else
	{
		TypeU = PHASE_C+REMOTE_U;
		TypeI = PHASE_RC+REMOTE_I;
	}
	api_GetRemoteData(TypeI, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);
	api_GetRemoteData(TypeU, DATA_HEX, 1, 2, (BYTE *)&w_RemoteV);

	// �����ѹ���ڵ�ѹ��������
	if( w_RemoteV > w_Limit_V )
	{
		if( dw_RemoteI < dw_Limit_I )
		{
			// ����
			Status = 1;
		}
	}
	return Status;
}

#endif//#if(SEL_EVENT_BREAK_I==YES)

#if( SEL_EVENT_OVER_I == YES )
// ������¼��״̬���
BYTE GetOverIStatus(BYTE Phase)
{
	BYTE Status;
	WORD Type;
	DWORD dw_RemoteI,dw_Limit_I;

	Status = 0;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	if( (Phase == 0) || (Phase > 3) )
	{
		//ֻ��A��B��C����
		return Status;
	}
	Phase --;
	// �����¼�������������
	DealEventParaByEventOrder( READ, eEVENT_OVER_I_NO, eTYPE_DWORD, (BYTE *)&dw_Limit_I );
	//�¹淶����ȷ��������¼�������������Ϊ0�����¼�������
	if(dw_Limit_I == 0)
	{
		return Status;
	}

	if( Phase == 0 )
	{
		Type = PHASE_A+REMOTE_I;
	}
	else if( Phase == 1 )
	{
		Type = PHASE_B+REMOTE_I;	
	}
	else
	{
		Type = PHASE_C+REMOTE_I;
	}
	api_GetRemoteData(Type, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteI);

	// �������ڵ�����������
	if( dw_RemoteI > dw_Limit_I )
	{
		// ����
		Status = 1;
	}
	return Status;
}


#endif//#if( SEL_EVENT_OVER_I == YES )

#endif//#if((SEL_EVENT_LOST_V==YES)||(SEL_EVENT_WEAK_V==YES)||(SEL_EVENT_OVER_V==YES)||(SEL_EVENT_BREAK==YES)||(SEL_EVENT_LOST_I==YES)||(SEL_EVENT_OVER_I==YES)||(SEL_EVENT_BREAK_I==YES))

