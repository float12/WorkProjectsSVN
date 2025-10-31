//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.24
//����		���������¼�����
//�޸ļ�¼	
//----------------------------------------------------------------------

#include "AllHead.h"

// ���������¼�
#if( SEL_DEMAND_OVER == YES )

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
static DWORD dwDemandOverMaxValue[2+4];

static const DWORD DemandOverDefaultOadConst[] =
{
	0//��������ĸ���
};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
#if( SEL_DEMAND_2022 == NO )//--���������жϷ�ʽ
//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: �ж������Ƿ���
//
//����: 
//	DemandOverType[in]:	�������ͣ����¼��������й��������й���һ�����������޹��������޵�ö������
//                    
//����ֵ:  	TRUE:��������  	FALSE:����δ����
//
//��ע:   
//-----------------------------------------------
BOOL GetPADemandOverStatus( BYTE DemandOverType )
{
	DWORD dwThreshold;
	long lTmpCurrentDemand;

	DealEventParaByEventOrder( READ, eEVENT_PA_DEMAND_OVER_NO, eTYPE_DWORD, (BYTE *)&dwThreshold );
	if( dwThreshold == 0 )
	{
		return FALSE;
	}
	
	api_GetCurrDemandData( 1, DATA_HEXCOMP, 4, 4, (BYTE *)&lTmpCurrentDemand );
	if( lTmpCurrentDemand < 0 )
	{
		return FALSE;
	}
			
	if( lTmpCurrentDemand < dwThreshold )
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL GetNADemandOverStatus( BYTE DemandOverType )
{
	DWORD dwThreshold;
	long lTmpCurrentDemand;

	DealEventParaByEventOrder( READ, eEVENT_NA_DEMAND_OVER_NO, eTYPE_DWORD, (BYTE *)&dwThreshold );
	if( dwThreshold == 0 )
	{
		return FALSE;
	}
	
	api_GetCurrDemandData( 1, DATA_HEXCOMP, 4, 4, (BYTE *)&lTmpCurrentDemand );
	if( lTmpCurrentDemand > 0 )
	{
		return FALSE;
	}
	
	lTmpCurrentDemand *= -1;
	if( lTmpCurrentDemand < dwThreshold )
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL GetQDemandOverStatus( BYTE DemandOverType )
{
	DWORD dwThreshold;
	long lTmpCurrentDemand;

	DealEventParaByEventOrder( READ, eEVENT_RE_DEMAND_OVER_NO, eTYPE_DWORD, (BYTE *)&dwThreshold );
	if( dwThreshold == 0 )
	{
		return FALSE;
	}
	
	if( api_GetSampleStatus( 0x30 ) != (DemandOverType+1) )//���޲��ԣ�������ǰ���޵��ж�
	{
		return FALSE;
	}
	
	api_GetCurrDemandData( 2, DATA_HEXCOMP, 4, 4, (BYTE *)&lTmpCurrentDemand );
			
	if( lTmpCurrentDemand < 0 )
	{
		lTmpCurrentDemand *= -1;
	}
	if( lTmpCurrentDemand < dwThreshold )
	{
		return FALSE;
	}
	
	return TRUE;
}

void DealDemandOverMaxDemand( void )
{
	BYTE i;
	long tl;
	TDemandData TmpDemandData;
	
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_PA_DEMAND_OVER ) == TRUE )
	{
		api_GetCurrDemandData( 1, DATA_HEXCOMP, 0xff, 4, (BYTE*)&tl );
		if( tl > 0 )
		{
			if( tl > dwDemandOverMaxValue[0] )
			{
				dwDemandOverMaxValue[0] = tl;
				TmpDemandData.DemandValue = tl;
				api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );
				api_WriteToContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue), sizeof(TmpDemandData), (BYTE *)&TmpDemandData );
			}
		}
	}
	else
	{
		if( dwDemandOverMaxValue[0] != 0 )
		{
			dwDemandOverMaxValue[0] = 0;
			api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue), sizeof(TmpDemandData) );
		}

	}
	
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_NA_DEMAND_OVER ) == TRUE )
	{
		api_GetCurrDemandData( 1, DATA_HEXCOMP, 0xff, 4, (BYTE*)&tl );
		if( tl < 0 )
		{
			tl *= -1;
			if( tl > dwDemandOverMaxValue[1] )
			{
				dwDemandOverMaxValue[1] = tl;
				TmpDemandData.DemandValue = tl;
				api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );
				api_WriteToContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue), sizeof(TmpDemandData), (BYTE *)&TmpDemandData );
			}
		}
	}
	else
	{
		if( dwDemandOverMaxValue[1] != 0 )
		{
			dwDemandOverMaxValue[1] = 0;
			api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue), sizeof(TmpDemandData) );

		}

	}
	
	#if( SEL_REACTIVE_DEMAND == YES )
	for(i=0; i<4; i++)
	{
		if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_QI_DEMAND_OVER+i ) == TRUE )
		{
			api_GetCurrDemandData( 2, DATA_HEXCOMP, 0xff, 4, (BYTE*)&tl );
			if( tl < 0 )
			{
				tl *= -1;
			}
			
			if( tl > dwDemandOverMaxValue[2+i] )
			{
				dwDemandOverMaxValue[2+i] = tl;
				TmpDemandData.DemandValue = tl;
				api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );
				api_WriteToContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[i]), sizeof(TmpDemandData), (BYTE *)&TmpDemandData );
			}
		}
		else
		{
			if( dwDemandOverMaxValue[2+i] != 0 )
			{
				dwDemandOverMaxValue[2+i] = 0;
				api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[i]), sizeof(TmpDemandData) );
			}

		}
	}
	#endif//#if( SEL_REACTIVE_DEMAND == YES )
}
#else
//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: �ж������Ƿ���
//
//����: 
//	DemandOverType[in]:	�������ͣ����¼��������й��������й���һ�����������޹��������޵�ö������
//                    
//����ֵ:  	TRUE:��������  	FALSE:����δ����
//
//��ע:   
//-----------------------------------------------

BOOL GetPADemandOverStatus( BYTE DemandOverType )
{
	DWORD dwThreshold;
	DWORD dwTmpCurrentDemand;

	DealEventParaByEventOrder( READ, eEVENT_PA_DEMAND_OVER_NO, eTYPE_DWORD, (BYTE *)&dwThreshold );
	if( dwThreshold == 0 )
	{
		return FALSE;
	}
	
	api_Get6TypeCurrDemandData( ePActiveDemand, DATA_HEX, 4, 4, (BYTE *)&dwTmpCurrentDemand );
			
	if( dwTmpCurrentDemand <= dwThreshold )
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL GetNADemandOverStatus( BYTE DemandOverType )
{
	DWORD dwThreshold;
	DWORD dwTmpCurrentDemand;

	DealEventParaByEventOrder( READ, eEVENT_NA_DEMAND_OVER_NO, eTYPE_DWORD, (BYTE *)&dwThreshold );
	if( dwThreshold == 0 )
	{
		return FALSE;
	}
	
	api_Get6TypeCurrDemandData( eNActiveDemand, DATA_HEX, 4, 4, (BYTE *)&dwTmpCurrentDemand );
	
	if( dwTmpCurrentDemand <= dwThreshold )
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL GetQDemandOverStatus( BYTE DemandOverType )
{
	DWORD dwThreshold;
	DWORD dwTmpCurrentDemand;
	eDemand6Type Demand6Type;

	DealEventParaByEventOrder( READ, eEVENT_RE_DEMAND_OVER_NO, eTYPE_DWORD, (BYTE *)&dwThreshold );
	if( dwThreshold == 0 )
	{
		return FALSE;
	}

	if( DemandOverType == ePHASE_ALL )
	{
		Demand6Type = eReactive1Demand;
	}
	else if( DemandOverType == ePHASE_A )
	{
		Demand6Type = eReactive2Demand;
	}
	else if( DemandOverType == ePHASE_B )
	{
		Demand6Type = eReactive3Demand;
	}
	else if( DemandOverType == ePHASE_C )
	{
		Demand6Type = eReactive4Demand;
	}
	else
	{
		return FALSE;
	}

	api_Get6TypeCurrDemandData( Demand6Type, DATA_HEX, 4, 4, (BYTE *)&dwTmpCurrentDemand );
	
	if( dwTmpCurrentDemand <= dwThreshold )
	{
		return FALSE;
	}
	
	return TRUE;
}

void DealDemandOverMaxDemand( void )
{
	BYTE i;
	DWORD tl;
	TDemandData TmpDemandData;
	
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_PA_DEMAND_OVER ) == TRUE )
	{
		api_Get6TypeCurrDemandData( ePActiveDemand, DATA_HEX, 4, 4, (BYTE *)&tl );

		if( tl > dwDemandOverMaxValue[0] )
		{
			dwDemandOverMaxValue[0] = tl;
			TmpDemandData.DemandValue = tl;
			api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );
			TmpDemandData.DemandTime.Sec = 0;//����ʱ��Ĭ��Ϊ������
			api_WriteToContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue), sizeof(TmpDemandData), (BYTE *)&TmpDemandData );
		}
	}
	else
	{
		if( dwDemandOverMaxValue[0] != 0 )
		{
			dwDemandOverMaxValue[0] = 0;
			api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue), sizeof(TmpDemandData) );
		}

	}
	
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_NA_DEMAND_OVER ) == TRUE )
	{
		api_Get6TypeCurrDemandData( eNActiveDemand, DATA_HEX, 4, 4, (BYTE *)&tl );

		if( tl > dwDemandOverMaxValue[1] )
		{
			dwDemandOverMaxValue[1] = tl;
			TmpDemandData.DemandValue = tl;
			api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );
			TmpDemandData.DemandTime.Sec = 0;//����ʱ��Ĭ��Ϊ������
			api_WriteToContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue), sizeof(TmpDemandData), (BYTE *)&TmpDemandData );
		}
	}
	else
	{
		if( dwDemandOverMaxValue[1] != 0 )
		{
			dwDemandOverMaxValue[1] = 0;
			api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue), sizeof(TmpDemandData) );

		}

	}
	
	#if( SEL_REACTIVE_DEMAND == YES )
	for(i=0; i<4; i++)
	{
		if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_QI_DEMAND_OVER+i ) == TRUE )
		{
			api_Get6TypeCurrDemandData( ((eDemand6Type)(eReactive1Demand+i)), DATA_HEX, 4, 4, (BYTE *)&tl );
			
			if( tl > dwDemandOverMaxValue[2+i] )
			{
				dwDemandOverMaxValue[2+i] = tl;
				TmpDemandData.DemandValue = tl;
				api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpDemandData.DemandTime );
				TmpDemandData.DemandTime.Sec = 0;//����ʱ��Ĭ��Ϊ������
				api_WriteToContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[i]), sizeof(TmpDemandData), (BYTE *)&TmpDemandData );
			}
		}
		else
		{
			if( dwDemandOverMaxValue[2+i] != 0 )
			{
				dwDemandOverMaxValue[2+i] = 0;
				api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[i]), sizeof(TmpDemandData) );
			}

		}
	}
	#endif//#if( SEL_REACTIVE_DEMAND == YES )
}

#endif

void FactoryInitDemandOverPara( void )
{
	BYTE i;

	api_WriteEventAttribute( 0x3009, 0xff, (BYTE *)&DemandOverDefaultOadConst[0], sizeof(DemandOverDefaultOadConst)/sizeof(DWORD) );
	
	api_WriteEventAttribute( 0x300a, 0xff, (BYTE *)&DemandOverDefaultOadConst[0], sizeof(DemandOverDefaultOadConst)/sizeof(DWORD) );
	
	api_WriteEventAttribute( 0x300b, 0xff, (BYTE *)&DemandOverDefaultOadConst[0], sizeof(DemandOverDefaultOadConst)/sizeof(DWORD) );
	//���E2�е����������¼��ļ�¼ֵ;
	api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.PADemandOverMaxValue), sizeof(TDemandData) );
	api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.NADemandOverMaxValue), sizeof(TDemandData) );
	for(i=0; i<4; i++)
	{
		api_ClrContinueEEPRom( GET_CONTINUE_ADDR(EventConRom.InstantEventRecord.QDemandOverMaxValue[i]), sizeof(TDemandData) );
	}
}

#endif//#if( SEL_DEMAND_OVER == YES )
