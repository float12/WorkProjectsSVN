//---------------------------------------------
// �����������¼  ���ش����¼
//---------------------------------------------
// ���µ�DLT645�У��������򡢹��ؼ�¼�ĸ�ʽ���������ݻ���һ��
// �������ж���ʼ������һ�£����Է���һ���д���
//---------------------------------------------

#include "AllHead.h"

#if( (SEL_EVENT_BACKPROP == YES)||(SEL_EVENT_OVERLOAD == YES) )
static const DWORD BackPopOverLoadOad[] =
{
#if( MAX_PHASE == 1)
	0x04,          // ����:4
	0x00102201,    //�¼�����ʱ�������й��ܵ���
	0x00202201,    //�¼�����ʱ�̷����й��ܵ���
	0x00108201,    //�¼������������й��ܵ���
	0x00208201,    //�¼����������й��ܵ���
#else
//	0x20,          // ����:32
//	0x00102201,    //�¼�����ʱ�������й��ܵ���
//	0x00202201,    //�¼�����ʱ�̷����й��ܵ���
//	0x00302201,    //�¼�����ʱ������޹�1�ܵ���
//	0x00402201,    //�¼�����ʱ������޹�2�ܵ���
//	0x00112201,    //�¼�����ʱ��A�������й�����
//	0x00212201,    //�¼�����ʱ��A�෴���й�����
//	0x00312201,    //�¼�����ʱ��A������޹�1����
//	0x00412201,    //�¼�����ʱ��A������޹�2����
//	0x00122201,    //�¼�����ʱ��B�������й�����
//	0x00222201,    //�¼�����ʱ��B�෴���й�����
//	0x00322201,    //�¼�����ʱ��B������޹�1����
//	0x00422201,    //�¼�����ʱ��B������޹�2����
//	0x00132201,    //�¼�����ʱ��C�������й�����
//	0x00232201,    //�¼�����ʱ��C�෴���й�����
//	0x00332201,    //�¼�����ʱ��C������޹�1����
//	0x00432201,    //�¼�����ʱ��C������޹�2����
//	0x00108201,    //�¼������������й��ܵ���
//	0x00208201,    //�¼����������й��ܵ���
//	0x00308201,    //�¼�����������޹�1�ܵ���
//	0x00408201,    //�¼�����������޹�2�ܵ���
//	0x00118201,    //�¼�������A�������й�����
//	0x00218201,    //�¼�������A�෴���й�����
//	0x00318201,    //�¼�������A������޹�1����
//	0x00418201,    //�¼�������A������޹�2����
//	0x00128201,    //�¼�������B�������й�����
//	0x00228201,    //�¼�������B�෴���й�����
//	0x00328201,    //�¼�������B������޹�1����
//	0x00428201,    //�¼�������B������޹�2����
//	0x00138201,    //�¼�������C�������й�����
//	0x00238201,    //�¼�������C�෴���й�����
//	0x00338201,    //�¼�������C������޹�1����
//	0x00438201     //�¼�������C������޹�2����
	0x04,          // ����:4
	0x00102201,    //�¼�����ʱ�������й��ܵ���
	0x00202201,    //�¼�����ʱ�̷����й��ܵ���
	0x00108201,    //�¼������������й��ܵ���
	0x00208201,    //�¼����������й��ܵ���
#endif
};

//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣��й����ʷ���
//------------------------------------------------------------------------------------------------------
// �й����ʷ�����
#if( SEL_EVENT_BACKPROP == YES )
void FactoryInitBackPopPara( void )
{
	api_WriteEventAttribute( 0x3007, 0xFF, (BYTE *)&BackPopOverLoadOad[0], sizeof(BackPopOverLoadOad)/sizeof(DWORD) );
}

// �ж��й����ʷ����Ƿ���
BYTE GetBackpropStatus(BYTE Phase)
{
	// BYTE Status;
	// WORD TypeP;
	// DWORD dw_LimitP,dw_RemoteP;

	// Status = 0;
	
	// if(api_GetMeasureMentMode() == eOnePhase)
	// {
	// 	if( (Phase == 2) || (Phase == 3) )
	// 	{
	// 		return 0;
	// 	}		
	// }

	// if( Phase > 3 )
	// {
	// 	//ֻ���ܼ�A��B��C����
	// 	return Status;
	// }
	// // �й����ʷ����¼��й����ʴ�������
	// DealEventParaByEventOrder( READ, eEVENT_BACKPROP_NO, eTYPE_DWORD, (BYTE *)&dw_LimitP );
	// //�¹淶����ȷ����й����ʷ����¼��й����ʴ�������Ϊ0�����¼�������
	// if( dw_LimitP == 0 )
	// {
	// 	return Status;
	// }
	
	// if( Phase == ePHASE_ALL )
	// {
	// 	TypeP = PHASE_ALL+REMOTE_P;
	// }
	// else if( MeterTypesConst == METER_3P3W )//��������ϵͳ�²��жϷ��๦�ʷ�����������ֻ�ж��ܵĹ��ʷ���
	// {
	// 	return 0;
	// }
	// else if( Phase == ePHASE_A )
	// {
	// 	TypeP = PHASE_A+REMOTE_P;
	// }
	// else if( Phase == ePHASE_B )
	// {
	// 	TypeP = PHASE_B+REMOTE_P;
	// }
	// else// if( Phase == ePHASE_C )
	// {
	// 	TypeP = PHASE_C+REMOTE_P;
	// }

	// api_GetRemoteData(TypeP, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteP);  
	// if( dw_RemoteP > dw_LimitP )// ���ʴ����й����ʴ�������
	// {
	// 	// ���ʷ���
	// 	if( api_GetSysStatus(eSYS_STATUS_OPPOSITE_P+Phase) == TRUE )
	// 	{
	// 		Status = 1;
	// 	}
	// }

	// return Status;
}

#endif

#if( SEL_EVENT_OVERLOAD == YES )
//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣�����
//------------------------------------------------------------------------------------------------------
void FactoryInitOverLoadPara( void )
{
	api_WriteEventAttribute( 0x3008, 0xFF, (BYTE *)&BackPopOverLoadOad[0], sizeof(BackPopOverLoadOad)/sizeof(DWORD) );
}

// ���ش���
// �жϹ����Ƿ������ദ���ã���˴��¼������������
BYTE GetOverloadStatus(BYTE Phase)
{
	BYTE Status;
	WORD TypeP;
	DWORD dw_LimitP,dw_RemoteP;

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
	// �����¼��й����ʴ�������
	DealEventParaByEventOrder( READ, eEVENT_OVERLOAD_NO, eTYPE_DWORD, (BYTE *)&dw_LimitP );// 
	//�¹淶����ȷ��������¼��й����ʴ�������Ϊ0�����¼�������
	if(dw_LimitP == 0)
	{
		return Status;
	}

	if( Phase == 0 )
	{
		TypeP = PHASE_A+REMOTE_P;
	}
	else if( Phase == 1 )
	{
		TypeP = PHASE_B+REMOTE_P;
	}
	else
	{
		TypeP = PHASE_C+REMOTE_P;
	}
	api_GetRemoteData(TypeP, DATA_HEX, 4, 4, (BYTE *)&dw_RemoteP);

	// ���ʴ��ڹ��ʴ�������
	if( dw_RemoteP > dw_LimitP )
	{
		// ����
		Status = 1;
	}

	return Status;

}

#endif

#endif //#if( (SEL_EVENT_BACKPROP == YES)||(SEL_EVENT_OVERLOAD == YES )

