//---------------------------------------------
// ������Դʧ�硢�����¼���¼
//---------------------------------------------

#include "AllHead.h"
#include "lostpower.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//���ڰѸ�����Դʧ���¼��������ϵ���д���
//�����0x5aa55aa5 ��ʾ����ǰ�Ѿ������˵����¼�
//�����0x12341234 ���������Ϊ���ֵ����ֹ�쳣��λ���µ����¼��ؼ�
__no_init DWORD PowerDownLostSecPowerFlag;
static __no_init DWORD PowerDownLostPowerFlag;

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
#if( SEL_EVENT_LOST_POWER == YES )

// ���� 0x3011  �Ĺ����������Ա� oad������������
static const DWORD LostPowerDefaultOadConst[] =
{
	2,//��������ĸ���
	0x00102201,	// �¼�����ʱ�������й�������
	0x00202201,	// �¼�����ʱ�̷����й�������
};
#endif

#if( SEL_EVENT_LOST_SECPOWER == YES )//20121221ma
// ������Դ���� 0x300e �Ĺ����������Ա� oad������������
static const DWORD LostSecPowerDefaultOadConst[] =
{
	0//��������ĸ���
	
};
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
// ������Դʧ���¼�
#if( SEL_EVENT_LOST_SECPOWER == YES )//20121221ma
// �жϸ�����Դ�Ƿ�ʧ��
BYTE GetLostSecPowerStatus(BYTE Phase)
{
	if(Phase>ePHASE_C)
	{
		return 0;
	}
	
	if( SelSecPowerConst == YES )
	{
		// ������Դʧ��
		if(SECPOWER_IS_DOWN)
		{
			return 1;
		}
	}

	return 0;
}

void LostSecPowerPowerOff( void )
{
    PowerDownLostSecPowerFlag = 0;
    if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_SECPOWER ) == 0 )   //������Դδʧ��
    {
        PowerDownLostSecPowerFlag = 0x5aa55aa5;
    }
}

#endif//#if( SEL_EVENT_LOST_SECPOWER == YES )//20121221ma


//---------------------------------------------
// �����¼�
#if( SEL_EVENT_LOST_POWER == YES )

//-----------------------------------------------
//��������: �����¼����ϵ紦��
//
//����:  ��
//                    
//����ֵ:	��
//
//��ע:   
//-----------------------------------------------

void LostPowerPowerOn( void )
{  
	DWORD dwAddr;
	BYTE bLostflag;
	// �����¼�����
 	if( PowerDownLostPowerFlag == eOtherFlag )//�쳣��λ��������
	{
	}
	else//���۵���Ƿ��е磬�ϵ綼��Ϊ������
	{
		EventStatus.CurrentStatus[eSUB_EVENT_LOST_POWER/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_POWER%8));
		api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_POWER );
        if( PowerDownLostPowerFlag == eLostPowerFlag )//�������ǰ ����ڵ���״̬ ��������״̬
        {

        }
        else if(PowerDownLostPowerFlag == eNormalFlag)//����״̬
        {
			SaveInstantEventRecord( eEVENT_LOST_POWER_NO, ePHASE_ALL, EVENT_START, eEVENT_ENDTIME_PWRDOWN );//��¼�¼���ʼ   
        }
		else
		{
			dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_LOST_POWER/8] );
			api_ReadFromContinueEEPRom( dwAddr, 1, &bLostflag );
			if (bLostflag&(0x01<<(eSUB_EVENT_LOST_POWER%8)))//�������ǰ ����ڵ���״̬ ��������״̬
			{
				/* code */
			}
			else//����״̬
			{
				SaveInstantEventRecord( eEVENT_LOST_POWER_NO, ePHASE_ALL, EVENT_START, eEVENT_ENDTIME_PWRDOWN );//��¼�¼���ʼ   
			}
			
			
		}
		
	}
	
	PowerDownLostPowerFlag = eOtherFlag;
	
}

void LostPowerPowerDown( void )
{
	DWORD dwAddr;
	BYTE PowerDownFlag;
	
	if(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 1)
	{
	    PowerDownLostPowerFlag = eLostPowerFlag;
	}
    else
    {
        PowerDownLostPowerFlag = eNormalFlag;
    }
}


//-----------------------------------------------
//��������: �ж��Ƿ����
//
//����:  ��
//                    
//����ֵ:	����״̬ 0��δ����  1������
//
//��ע: 
//-----------------------------------------------
BYTE GetLostPowerStatus( BYTE Phase )
{
	BYTE Status, i;
	WORD wLimitU,wRemoteU;
	DWORD dwRemoteI;
	Status = 0;

	// ��ϵͳ�е�
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == TRUE )
	{
#if( MAX_PHASE == 1 )
		// �ٽ��ѹ
		wLimitU = wStandardVoltageConst * 6 /10;// 60%Un @@
		api_GetRemoteData(PHASE_A+REMOTE_U, DATA_HEX, 1, sizeof(WORD), (BYTE *)&wRemoteU);// 2�ֽ� 1λС��

		if( wRemoteU < wLimitU )
		{
			Status = 1;
		}
		return Status;
#else
		// �ٽ��ѹ����Ҫ��60%Un
		wLimitU = wStandardVoltageConst * 6 /10;// 60%Un @@

		for( i = 0; i < MAX_PHASE; i++ )
		{
			if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
			{
				Status |= 0x02;
				continue;
			}

			api_GetRemoteData(PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, sizeof(WORD), (BYTE *)&wRemoteU);// 2�ֽ� 1λС��

			if( wRemoteU < wLimitU )
			{
				// ����ʧѹ
				Status |= (0x01<<i);
			}
		}

		// ��������ѹ�������ٽ��ѹ�����ж�Ϊ����
		if(Status == 0x07)
		{
			return 1;
		}
		// �е�
		return 0;
#endif//#if( MAX_PHASE == 1 )
	}

	return 1;
}

#endif//#if( SEL_EVENT_LOST_POWER == YES )

//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void FactoryInitLostPowerPara( void )
{
	#if( SEL_EVENT_LOST_POWER == YES )
	api_WriteEventAttribute( 0x3011, 0xff, (BYTE *)&LostPowerDefaultOadConst[0], sizeof(LostPowerDefaultOadConst)/sizeof(DWORD) );// ����
	#endif
	
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	api_WriteEventAttribute( 0x300e, 0xff, (BYTE *)&LostSecPowerDefaultOadConst[0], sizeof(LostSecPowerDefaultOadConst)/sizeof(DWORD) );// ������Դ����
	#endif
}


