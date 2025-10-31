//---------------------------------------------
// �����������޼�¼
//---------------------------------------------

#include "AllHead.h"

#if( SEL_EVENT_COS_OVER == YES )

static const DWORD CosOverOad[] =
{
#if( MAX_PHASE == 1)
	0x04,				// ����
	0x00102201,    //�¼�����ʱ�������й��ܵ���
	0x00202201,    //�¼�����ʱ�̷����й��ܵ���
	0x00108201,    //�¼������������й��ܵ���
	0x00208201,    //�¼����������й��ܵ���
#else
	0x08,				// ����
	0x00102201,    //�¼�����ʱ�������й��ܵ���
	0x00202201,    //�¼�����ʱ�̷����й��ܵ���
	0x00302201,    //�¼�����ʱ������޹�1�ܵ���
	0x00402201,    //�¼�����ʱ������޹�2�ܵ���
	0x00108201,    //�¼������������й��ܵ���
	0x00208201,    //�¼����������й��ܵ���
	0x00308201,    //�¼�����������޹�1�ܵ���
	0x00408201     //�¼�����������޹�2�ܵ���
#endif
};

//------------------------------------------------------------------------------------------------------
// �����¼�����ר�ò��֣��й����ʷ���
//------------------------------------------------------------------------------------------------------
void FactoryInitCosOverPara( void )
{
	api_WriteEventAttribute( 0x303B, 0xFF, (BYTE *)&CosOverOad[0], sizeof(CosOverOad)/sizeof(DWORD) );
}

// �жϹ������������Ƿ���,����20�淶��������������£����жϷ��๦�����������ޣ����������ж�
BYTE GetCosoverStatus(BYTE Phase)
{
	BYTE Status;
	WORD Type;
	WORD w_LimitCos,w_RemoteCos;
	
	if(api_GetMeasureMentMode() == eOnePhase)
	{
		if( (Phase == 2) || (Phase == 3) )
		{
			return 0;
		}		
	}
	
	Status = 0;
	if(Phase>ePHASE_C)
	{
		return Status;
	}
	// �������������޷�ֵ
	DealEventParaByEventOrder( READ, eEVENT_COS_OVER_NO, eTYPE_WORD, (BYTE *)&w_LimitCos );
	//�¹淶����ȷ����������������޷�ֵΪ0�����¼�������
	if( w_LimitCos == 0 )
	{
		return 0;
	}
	//�淶Ҫ�����������ޣ������������ 5%�������������
    if( Phase == ePHASE_ALL )//������ܹ���������
    {
        if( MeterTypesConst == METER_3P3W )
        {
    		if( (api_GetSampleStatus(0x11)&0x0005) == 0x05 )
    		{
    			return 0;
    		}
        }
        else
        {
    		if( (api_GetSampleStatus(0x11)&0x0007) == 0x07 )
    		{
    			return 0;
    		}
        }

    }
	else 
    {
        if( ( (api_GetSampleStatus(0x11)&0x0007) & (0x0001<<(Phase-1)) ) != 0 )
		{
			return 0;
		}
    }
	// �����������������ж�
	if( Phase == ePHASE_ALL )
	{
		Type = PHASE_ALL+REMOTE_COS;
	}
	else if( Phase == ePHASE_A )
	{
		Type = PHASE_A+REMOTE_COS;	
	}
	else if( Phase == ePHASE_B )
	{
		Type = PHASE_B+REMOTE_COS;	
	}
	else// if( Phase == ePHASE_C )
	{
		Type = PHASE_C+REMOTE_COS;
	}
	api_GetRemoteData(Type, DATA_HEX, 3, 2, (BYTE *)&w_RemoteCos);
	if( w_RemoteCos < w_LimitCos)
	{
		Status = 1;
	}

	return Status;
}


#endif
