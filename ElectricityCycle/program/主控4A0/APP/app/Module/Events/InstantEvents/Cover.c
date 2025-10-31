//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.10.8
//����		����ǣ�����ť��ģ��Դ�ļ�
//�޸ļ�¼	
//---------------------------------------------------------------------- 

#include "AllHead.h"
#include "cover.h"
#if((SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES))
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
#if( SEL_EVENT_METERCOVER == YES )
static __no_init BYTE MeterCoverOldStatus;					// �����״̬
static __no_init TLowPowerMeterCoverData LowPowerMeterCoverData;		// �͹�������
#endif

#if( SEL_EVENT_BUTTONCOVER == YES )
static __no_init BYTE ButtonCoverOldStatus;					// ����ť��״̬
static __no_init TLowPowerButtonCoverData LowPowerButtonCoverData;	// �͹�������
#endif

// ����� 0x301b ����ť�� 0x301c
static const DWORD CoverOad[] =
{
#if( MAX_PHASE == 1)
	0x04,		// ����
	0x00102201,//����ʱ�������й��ܵ���
	0x00202201,//����ʱ�̷����й��ܵ���
	0x00108201,//����ʱ�������й��ܵ���
	0x00208201// ����ʱ�������й��ܵ���
#else
	0x0c,		// ����
	0x00102201,//����ʱ�������й��ܵ���
	0x00202201,//����ʱ�̷����й��ܵ���
	0x00502201,//����ʱ�̵�һ�����޹��ܵ���
	0x00602201,//����ʱ�̵ڶ������޹��ܵ���
	0x00702201,//����ʱ�̵��������޹��ܵ���
	0x00802201,//����ʱ�̵��������޹��ܵ���
	0x00108201,//����ʱ�������й��ܵ���
	0x00208201,//����ʱ�������й��ܵ���
	0x00508201,//����ʱ�̵�һ�����޹��ܵ���
	0x00608201,//����ʱ�̵ڶ������޹��ܵ���
	0x00708201,//����ʱ�̵��������޹��ܵ���
	0x00808201,//����ʱ�̵��������޹��ܵ���
#endif
};
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
#if( SEL_EVENT_METERCOVER == YES )
void MeterCoverPowerOn( void )
{
	BYTE Buf[((SUB_EVENT_INDEX_MAX_NUM+7)/8)+20],i;
	WORD Len;
	DWORD dwAddr;

	Len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] );
	api_ReadFromContinueEEPRom( dwAddr, Len, Buf );
	// ����ǰ�¼��Ѿ���ʼ,���ѿ����
	if( ( Buf[eSUB_EVENT_METER_COVER/8] & (0x01<<(eSUB_EVENT_METER_COVER%8)) ) != 0 )
	{
		//�����״̬
		for(i=0;i<5;i++)	
		{
			if(!( UP_COVER_IS_OPEN ))
			{
				break;
			}
			api_Delayms(10);
		}
		
		if(i>=5)	// �ϵ�������Ǵ�״̬��������
		{
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
			return;
		}
		else		// �ϵ�����ǹر�״̬������ʱ�䲹����
		{
			SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_PWRDOWN );
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);
		}
	}



	// TRealTimer RealTimerBak;

	// if(LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	// {
	// 	//�͹����ڼ�δ���������¼�
	// 	//�����ǰ״̬Ϊ���ǣ����֣�
	// 	if(MeterCoverOldStatus == 1)
	// 	{
	// 		EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
	// 		api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
	// 	}
	// 	return;
	// }

	// //��ʼ����͹����ڼ俪������
	// //�ȿ��Ƿ���Ҫ��¼�����¼���ʼ
	// //�ڼ����ٷ�������Ǳ�λ
	// memcpy(&RealTimerBak, &RealTimer, sizeof(TRealTimer));
	// if( api_CheckClock(&LowPowerMeterCoverData.MeterCoverOpenTime) == TRUE )
	// {
	// 	//��¼��ʼ
	// 	RealTimer = LowPowerMeterCoverData.MeterCoverOpenTime;
	// 	SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_START, eEVENT_ENDTIME_CURRENT );
	// 	EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
	// 	api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
	// }
	// if( api_CheckClock(&LowPowerMeterCoverData.MeterCoverCloseTime) == TRUE )
	// {
	// 	//��¼����
	// 	RealTimer = LowPowerMeterCoverData.MeterCoverCloseTime;
	// 	SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
	// 	EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
	// 	api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
	// }
	// memcpy(&RealTimer, &RealTimerBak, sizeof(TRealTimer));//��Ϊǰ���ʱ�串���ˣ�����Ҫˢ��һ��
}

//-----------------------------------------------
//��������  :    �͹��Ŀ���Ǽ��
//����:  
//           BYTE Mode[in] 0���͹���    0x55������
//
//����ֵ:     
//��ע����  :   
//-----------------------------------------------
void ProcLowPowerMeterCoverRecord( BYTE Mode )
{
	BYTE Status;

	if( UP_COVER_IS_OPEN )
	{
		Status = 1;
		if( Status != MeterCoverOldStatus )
		{
			if( Mode == 0x55 )//��������½�������
			{
				api_Delayms(50);//ʵ����ʱʱ��Ϊ50.22ms
			}
			else
			{
				api_Delay100us( 1 );//ʵ����ʱʱ��Ϊ48.28ms
			}

			if( UP_COVER_IS_OPEN )
			{
				Status = 1;
			}
			else
			{
				Status = 0;
			}
		}
	}
	else
	{
		Status = 0;
	}

	if( Status != MeterCoverOldStatus )
	{
		LowPowerMeterCoverData.CheckMeterCoverStatus = 0x12345678;
		if( Status == 0 )//��״̬��0�����ǽ���
		{
			if( LowPowerMeterCoverData.MeterCoverCloseTime.Day != 0xff )
			{
				//֮ǰ�Ѿ�������
				MeterCoverOldStatus = Status;
				return;
			}
			//��ʱ��¼���ǽ�����RAMֵ
			LowPowerMeterCoverData.MeterCoverStatus[1] = Status;
			
			if( Mode == 0x55 )//���������,����Ҫ��E2��Դ
			{
				api_ReadMeterTime( 0 );
			}
			else
			{
				POWER_FLASH_OPEN;
				api_ReadMeterTime( 0 );
				// POWER_FLASH_CLOSE;
			}

			LowPowerMeterCoverData.MeterCoverCloseTime = RealTimer;
			MeterCoverOldStatus = Status;
			return;
		}
		else//��״̬����0�����ǿ�ʼ
		{
			if( ( LowPowerMeterCoverData.MeterCoverCloseTime.Day != 0xff ) || ( LowPowerMeterCoverData.MeterCoverOpenTime.Day != 0xff ) )
			{
				//֮ǰ�Ѿ����������ߵ͹����ڼ䷢�������ǽ���
				MeterCoverOldStatus = Status;
				return;
			}
			//��ʱ��¼���ǿ�ʼ��RAMֵ
			LowPowerMeterCoverData.MeterCoverStatus[0] = Status;
			
			if( Mode == 0x55 )//���������,����Ҫ��E2��Դ
			{
				api_ReadMeterTime( 0 );
			}
			else
			{
				POWER_FLASH_OPEN;
				api_ReadMeterTime( 0 );
				// POWER_FLASH_CLOSE;
			}

			LowPowerMeterCoverData.MeterCoverOpenTime = RealTimer;
		}
	}

	MeterCoverOldStatus = Status;
}

//-----------------------------------------------
//��������: �жϿ�����Ƿ���
//
//����:  ��
//                    
//����ֵ:	��
//
//��ע: 
//-----------------------------------------------
BYTE GetMeterCoverStatus(BYTE Phase)
{
	BYTE i;
	//���Ǽ��
	if( UP_COVER_IS_OPEN )
	{
		if((EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] & (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8))) == 0)
		{
			for(i=0;i<5;i++)	
			{
				if(!( UP_COVER_IS_OPEN ))
				{
					return 0;
				}
				api_Delayms(10);
			}
		}
		return 1;
	}

	return 0;

}

void MeterCoverPowerOff( void )
{
	MeterCoverOldStatus = api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_METER_COVER );

	memset(&LowPowerMeterCoverData,0xff,sizeof(TLowPowerMeterCoverData));
}




#endif//#if( SEL_EVENT_METERCOVER == YES )


#if( SEL_EVENT_BUTTONCOVER == YES )
//�ϵ�ʱ����͹����ڼ俪�Ǽ�¼
void ButtonCoverPowerOn( void )
{
	TRealTimer RealTimerBak;

	if(LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	{
		//�͹����ڼ�δ��������ť���¼�
		//�����ǰ״̬Ϊ���ǣ����֣�
		if(ButtonCoverOldStatus == 1)
		{
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER );
		}
		return;
	}
	//��ʼ����͹����ڼ俪��ť������
	//�ȿ��Ƿ���Ҫ��¼�����¼���ʼ
	//�ڼ����ٷ�������Ǳ�λ
	memcpy(&RealTimerBak, &RealTimer, sizeof(TRealTimer));
	if( api_CheckClock(&LowPowerButtonCoverData.ButtonCoverOpenTime) == TRUE )
	{
		//��¼��ʼ
		RealTimer = LowPowerButtonCoverData.ButtonCoverOpenTime;
		SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_START, eEVENT_ENDTIME_CURRENT );
		EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
		api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);
	}
	if( api_CheckClock(&LowPowerButtonCoverData.ButtonCoverCloseTime) == TRUE )
	{
		//��¼����
		RealTimer = LowPowerButtonCoverData.ButtonCoverCloseTime;
		SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
		EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
		api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
	}
	memcpy(&RealTimer, &RealTimerBak, sizeof(TRealTimer));//��Ϊǰ���ʱ�串���ˣ�����Ҫˢ��һ��
}

//-----------------------------------------------
//��������  :    �͹��Ŀ���Ǽ��
//����:  
//           BYTE Mode[in] 0���͹���    0x55������
//
//����ֵ:     
//��ע����  :   
//-----------------------------------------------
void ProcLowPowerButtonCoverRecord( BYTE Mode )
{	
	BYTE Status;

	if( END_COVER_IS_OPEN )
	{
		Status = 1;
		if( Status != ButtonCoverOldStatus )
		{
			if( Mode == 0x55 )//��������½�������
			{
				api_Delayms(50);//ʵ����ʱʱ��Ϊ50.22ms
			}
			else
			{
				api_Delay100us( 1 );//ʵ����ʱʱ��Ϊ48.28ms
			}

			if( END_COVER_IS_OPEN )
			{
				Status = 1;
			}
			else
			{
				Status = 0;
			}
		}
	}
	else
	{
		Status = 0;
	}

	if( Status != ButtonCoverOldStatus )
	{
		LowPowerButtonCoverData.CheckButtonCoverStatus = 0x12345678;
		if( Status == 0 )//��״̬��0������ť�ǽ���
		{
			if( LowPowerButtonCoverData.ButtonCoverCloseTime.Day != 0xff )
			{
				ButtonCoverOldStatus = Status;
				//֮ǰ�Ѿ�������
				return;
			}
			//��ʱ��¼����ť�ǽ�����RAMֵ
			LowPowerButtonCoverData.ButtonCoverStatus[1] = Status;
			
			if( Mode == 0x55 )//���������,����Ҫ��E2��Դ
			{
				api_ReadMeterTime( 0 );

			}
			else
			{
				POWER_FLASH_OPEN;
				api_ReadMeterTime( 0 );
				POWER_FLASH_CLOSE;
			}

			LowPowerButtonCoverData.ButtonCoverCloseTime = RealTimer;
			ButtonCoverOldStatus = Status;
			return;
		}
		else//��״̬����0������ť�ǿ�ʼ
		{
			if( ( LowPowerButtonCoverData.ButtonCoverOpenTime.Day != 0xff ) || ( LowPowerButtonCoverData.ButtonCoverCloseTime.Day != 0xff ) )
			{
				//֮ǰ�Ѿ����������ߵ͹����ڼ䷢��������ť�ǽ���
				ButtonCoverOldStatus = Status;
				return;
			}
			LowPowerButtonCoverData.ButtonCoverStatus[0] = Status;
			
			if( Mode == 0x55 )//���������,����Ҫ��E2��Դ
			{
				api_ReadMeterTime( 0 );
			}
			else
			{
				POWER_FLASH_OPEN;
				api_ReadMeterTime( 0 );
				POWER_FLASH_CLOSE;
			}

			LowPowerButtonCoverData.ButtonCoverOpenTime = RealTimer;
		}
	}
	ButtonCoverOldStatus = Status;
}

// �жϿ���ť���Ƿ���
BYTE GetButtonCoverStatus(BYTE Phase)
{
	//����ť�Ǽ��
	if( END_COVER_IS_OPEN )
	{
		return 1;
	}

	return 0;
}

void ButtonCoverPowerOff( void )
{
	ButtonCoverOldStatus = api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER );

	memset(&LowPowerButtonCoverData,0xff,sizeof(TLowPowerButtonCoverData));
}

#endif//#if( SEL_EVENT_BUTTONCOVER == YES )

//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void FactoryInitCoverPara( void )
{
	#if( SEL_EVENT_METERCOVER == YES )
	api_WriteEventAttribute( 0x301b, 0xFF, (BYTE *)&CoverOad[0], sizeof(CoverOad)/sizeof(DWORD) );			// �����
	#endif

	#if( SEL_EVENT_BUTTONCOVER == YES )
	api_WriteEventAttribute( 0x301c, 0xFF, (BYTE *)&CoverOad[0], sizeof(CoverOad)/sizeof(DWORD) );			// ����ť��
	#endif
}

#endif//#if((SEL_EVENT_METERCOVER==YES) || (SEL_EVENT_BUTTONCOVER==YES))
