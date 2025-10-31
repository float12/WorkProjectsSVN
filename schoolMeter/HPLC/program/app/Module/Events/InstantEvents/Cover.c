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
static __no_init DWORD MeterCoverOldStatus;					// �����״̬
static __no_init TLowPowerMeterCoverData LowPowerMeterCoverData;		// �͹�������
#endif

#if( SEL_EVENT_BUTTONCOVER == YES )
static __no_init DWORD ButtonCoverOldStatus;					// ����ť��״̬
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
	TRealTimer RealTimerBak;
	DWORD dwAddr;
	//��E2�ָ�
	BYTE CoverStatus=0,Buf[10],CloseTimeFlag=0,OpenTimeFlag=0;
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_METER_COVER/8] );
	if(api_ReadFromContinueEEPRom( dwAddr, sizeof(BYTE), Buf ) == FALSE)
	{
		CoverStatus = 0;
		EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
		api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
	}
	else
	{
		if(Buf[0] & (0x01<<(eSUB_EVENT_METER_COVER%8)))
		{
			CoverStatus = 1;
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
		}
		else
		{
			CoverStatus = 0;
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
		}
	}
	//RAM���ݲ��� �ٴ�EE�ָ�
	if( (LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	 && (LowPowerMeterCoverData.CheckMeterCoverStatus != 0xffffffff) )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
		api_ReadFromContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);
	}
	//����ɾ-��ֹƵ�����µ���RAM��Ϊʱ��Ϸ������¼
	if( LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	{ 
		//������ݲ���ȫFF ��������һ��EE ��ֹ���ĳ���EEλ�øı䵼�¿��Ǽ�¼����
		memset( &LowPowerMeterCoverData, 0xff, sizeof(TLowPowerMeterCoverData) );
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
		api_WriteToContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);
		return;
	}
	if( api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverOpenTime) == TRUE )
	{
		OpenTimeFlag = 1;
	}

	if(api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverCloseTime) == TRUE )
	{
		CloseTimeFlag = 1;

	}
	
	//��ʼ����͹����ڼ俪������
	memcpy(&RealTimerBak, &RealTimer, sizeof(TRealTimer));//д�¼���¼����RealTimer������ɾ
	if( CoverStatus == 1 )//����
	{
		if( CloseTimeFlag == 1 )//�ϸ�ʱ��Ϸ�
		{
			//��¼����
			RealTimer = LowPowerMeterCoverData.MeterCoverCloseTime;
			SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
		}
	}
	else//�ϸ�
	{
		if(OpenTimeFlag == 1 )//����ʱ��Ϸ�
		{
			//��¼��ʼ
			RealTimer = LowPowerMeterCoverData.MeterCoverOpenTime;
			SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_START, eEVENT_ENDTIME_CURRENT );
			EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_METER_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_METER_COVER);
			if( CloseTimeFlag == 1 )//�ϸ�ʱ��Ϸ�
			{
				//��¼����
				RealTimer = LowPowerMeterCoverData.MeterCoverCloseTime;
				SaveInstantEventRecord( eEVENT_METERCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
				EventStatus.CurrentStatus[eSUB_EVENT_METER_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_METER_COVER%8));
				api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_METER_COVER);	
			}
		}
	}
	memcpy(&RealTimer, &RealTimerBak, sizeof(TRealTimer));//����ɾ����Ϊǰ���ʱ�串���ˣ�����Ҫˢ��һ��
	
	//��RAM��EE������ΪȫFF
	memset(&LowPowerMeterCoverData,0xff,sizeof(TLowPowerMeterCoverData));
	dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
	api_WriteToContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);
}

//-----------------------------------------------
//��������  : ����͹���ǰ���п������ݼ��
//����:
//         
//
//����ֵ:     
//��ע����:ֻ���ڽ���͹���ǰ���� �͹����²��ܵ���
//-----------------------------------------------
void api_LowPowerCheckMeterCoverStatus( void )
{
	BYTE Buf[8];
	DWORD dwAddr;
		
	//RAM�����ݴ����EE�ָ�
	if( ( MeterCoverOldStatus > 1 )
	|| ( (LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	   &&(LowPowerMeterCoverData.CheckMeterCoverStatus != 0xffffffff)))
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_METER_COVER/8] );
		if(api_ReadFromContinueEEPRom( dwAddr, sizeof(BYTE), Buf ) == FALSE)
		{
			MeterCoverOldStatus = 0;	
		}
		else
		{
			if(Buf[0] & (0x01<<(eSUB_EVENT_METER_COVER%8)))
			{
				MeterCoverOldStatus = 1;
			}
			else
			{
				MeterCoverOldStatus = 0;
			}
		}
		
		//��EE�ָ������ڼ俪�Ǽ�¼
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
		api_ReadFromContinueEEPRom(	dwAddr, sizeof(LowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);

		//���EE���ݻ��ǲ��� ��ΪȫFF
		if( (LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
		 && (LowPowerMeterCoverData.CheckMeterCoverStatus != 0xffffffff) )
		{
			memset(&LowPowerMeterCoverData, 0xff, sizeof(LowPowerMeterCoverData));
		}
	}
}
//-----------------------------------------------
//��������  :    �͹��Ŀ���Ǽ��
//����:
//           BYTE Mode[in] 0���͹���    0x55������
//
//����ֵ:
//��ע����  :
//-----------------------------------------------
void ProcLowPowerMeterCoverRecord(  )
{
	BYTE Status, Mode;
	DWORD dwAddr;
	//RAM�����ݺϷ����ж�
	if( ( MeterCoverOldStatus > 1 )
	|| ( (LowPowerMeterCoverData.CheckMeterCoverStatus != 0x12345678)
	   &&(LowPowerMeterCoverData.CheckMeterCoverStatus != 0xffffffff)))
	{
		//Ĭ����Ϊ�ϸ�״̬
		MeterCoverOldStatus = 0;
		memset(&LowPowerMeterCoverData, 0xff, sizeof(LowPowerMeterCoverData));
	}
	if( api_CheckMCUCLKIsFlf() == TRUE )//��⵱ǰƵ���Ƿ�Ϊ32768
	{
		Mode = 0;
	}
	else
	{
		Mode = 0x55;
	}
	
	//���ϸǼ��
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

	//���ϸǼ�¼
	if( Status != MeterCoverOldStatus )
	{
		MeterCoverOldStatus = Status;//��ʼ�Ͱ�״̬ˢ��һ�� ��ֹ���濪EE��¼���ϸǵ��¸�λ
		LowPowerMeterCoverData.CheckMeterCoverStatus = 0x12345678;
		if( Status == 0 )//��״̬��0�����ǽ���
		{	
			//�Ѿ���¼�ϸ� �����ظ���¼
			if( api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverCloseTime) == TRUE )
			{
				//֮ǰ�Ѿ����������ߵ͹����ڼ䷢�����ϸǽ���
				return;
			}
			//��ʱ��¼���ǽ�����RAMֵ
			LowPowerMeterCoverData.MeterCoverStatus[1] = Status;
			
			if( Mode == 0x55 )//���������,����Ҫ��E2��Դ
			{
				api_ReadMeterTimeLowPower();
			}
			else
			{
				api_ReadMeterTimeLowPower();
			}

			LowPowerMeterCoverData.MeterCoverCloseTime = RealTimer;
		}
		else//��״̬����0�����ǿ�ʼ
		{
			if( ( api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverCloseTime) == TRUE )
			|| ( api_CheckClockYear(&LowPowerMeterCoverData.MeterCoverOpenTime) == TRUE ))
			{
				//֮ǰ�Ѿ����������ߵ͹����ڼ䷢�������ǽ���
				return;
			}
			//��ʱ��¼���ǿ�ʼ��RAMֵ
			LowPowerMeterCoverData.MeterCoverStatus[0] = Status;
			
			if( Mode == 0x55 )//���������,����Ҫ��E2��Դ
			{
				api_ReadMeterTimeLowPower();
			}
			else
			{
				api_ReadMeterTimeLowPower();
			}

			LowPowerMeterCoverData.MeterCoverOpenTime = RealTimer;
		}
		
		//����д��EE
		//һ��Ҫд��RAM֮���ٿ�����ƵHRC ��ֹ���ĸ�CPU��λ���¿���δ��¼
		if( Mode != 0x55 )
		{
			api_MCU_SysClockInit( ePowerDownMode );//�л���5.5Mʱ��
		}
		POWER_EEPROM_OPEN;
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerMeterCoverData );
		api_WriteToContinueEEPRom(	dwAddr, sizeof(LowPowerMeterCoverData), (BYTE *)&LowPowerMeterCoverData);
		EEPROM_INIT_LOW_POWER;
		POWER_EEPROM_CLOSE;	
		
		if( Mode != 0x55 )
		{
			HTMCU_GoToLowSpeed(); //�л�����Ƶ32.768ʱ��
		}
	}
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
//�ϵ�ʱ����͹����ڼ俪��ť�Ǽ�¼
void ButtonCoverPowerOn( void )
{
	TRealTimer RealTimerBak;
	DWORD dwAddr;
	//��E2�ָ�
	BYTE CoverStatus=0,Buf[10],CloseTimeFlag=0,OpenTimeFlag=0;
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_BUTTON_COVER/8] );
	if(api_ReadFromContinueEEPRom( dwAddr, sizeof(BYTE), Buf ) == FALSE)
	{
		CoverStatus = 0;
		EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
		api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
	}
	else
	{
		if(Buf[0] & (0x01<<(eSUB_EVENT_BUTTON_COVER%8)))
		{
			CoverStatus = 1;
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);
		}
		else
		{
			CoverStatus = 0;
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
		}
	}
	//RAM���ݲ��� �ٴ�EE�ָ�
	if( (LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	 && (LowPowerButtonCoverData.CheckButtonCoverStatus != 0xffffffff) )
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
		api_ReadFromContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerButtonCoverData);
	}
	//����ɾ-��ֹƵ�����µ���RAM��Ϊʱ��Ϸ������¼
	if( LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	{ 
		memset( &LowPowerButtonCoverData, 0xff, sizeof(TLowPowerMeterCoverData) );
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
		api_WriteToContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerButtonCoverData);
	}
	if( api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverOpenTime) == TRUE )
	{
		OpenTimeFlag = 1;
	}

	if(api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverCloseTime) == TRUE )
	{
		CloseTimeFlag = 1;

	}
	
	//��ʼ����͹����ڼ俪������
	memcpy(&RealTimerBak, &RealTimer, sizeof(TRealTimer));//д�¼���¼����RealTimer������ɾ
	if( CoverStatus == 1 )//����
	{
		if( CloseTimeFlag == 1 )//�ϸ�ʱ��Ϸ�
		{
			//��¼����
			RealTimer = LowPowerButtonCoverData.ButtonCoverCloseTime;
			SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
		}
	}
	else//�ϸ�
	{
		if(OpenTimeFlag == 1 )//����ʱ��Ϸ�
		{
			//��¼��ʼ
			RealTimer = LowPowerButtonCoverData.ButtonCoverOpenTime;
			SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_START, eEVENT_ENDTIME_CURRENT );
			EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] |= (BYTE)(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
			api_DealEventStatus(eSET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);
			if( CloseTimeFlag == 1 )//�ϸ�ʱ��Ϸ�
			{
				//��¼����
				RealTimer = LowPowerButtonCoverData.ButtonCoverCloseTime;
				SaveInstantEventRecord( eEVENT_BUTTONCOVER_NO, 0, EVENT_END, eEVENT_ENDTIME_CURRENT );
				EventStatus.CurrentStatus[eSUB_EVENT_BUTTON_COVER/8] &= (BYTE)~(0x01<<(eSUB_EVENT_BUTTON_COVER%8));
				api_DealEventStatus(eCLEAR_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER);	
			}
		}
	}
	memcpy(&RealTimer, &RealTimerBak, sizeof(TRealTimer));//����ɾ����Ϊǰ���ʱ�串���ˣ�����Ҫˢ��һ��
	
	memset(&LowPowerButtonCoverData,0xff,sizeof(TLowPowerButtonCoverData));
	dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
	api_WriteToContinueEEPRom(	dwAddr, sizeof(TLowPowerMeterCoverData), (BYTE *)&LowPowerButtonCoverData);
}
//-----------------------------------------------
//��������  : ����͹���ǰ���п������ݼ��
//����:
//         
//
//����ֵ:     
//��ע����:ֻ���ڽ���͹���ǰ���� �͹����²��ܵ���
//-----------------------------------------------
void api_LowPowerCheckButtonCoverStatus( void )
{
	BYTE Buf[8];
	DWORD dwAddr;
		
	//RAM�����ݴ����EE�ָ�
	if( ( ButtonCoverOldStatus > 1 )
	|| ( (LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	   &&(LowPowerButtonCoverData.CheckButtonCoverStatus != 0xffffffff)))
	{
		dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[eSUB_EVENT_BUTTON_COVER/8] );
		if(api_ReadFromContinueEEPRom( dwAddr, sizeof(BYTE), Buf ) == FALSE)
		{
			ButtonCoverOldStatus = 0;	
		}
		else
		{
			if(Buf[0] & (0x01<<(eSUB_EVENT_BUTTON_COVER%8)))
			{
				ButtonCoverOldStatus = 1;
			}
			else
			{
				ButtonCoverOldStatus = 0;
			}
		}
		
		//��EE�ָ������ڼ俪�Ǽ�¼
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
		api_ReadFromContinueEEPRom(	dwAddr, sizeof(LowPowerButtonCoverData), (BYTE *)&LowPowerButtonCoverData);

		//���EE���ݻ��ǲ��� ��ΪȫFF
		if( (LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
		 && (LowPowerButtonCoverData.CheckButtonCoverStatus != 0xffffffff) )
		{
			memset(&LowPowerButtonCoverData, 0xff, sizeof(LowPowerButtonCoverData));
		}
	}
}
//-----------------------------------------------
//��������  :    �͹��Ŀ���ť�Ǽ��
//����:  
//           BYTE Mode[in] 0���͹���    0x55������
//
//����ֵ:     
//��ע����  :   
//-----------------------------------------------
void ProcLowPowerButtonCoverRecord(  )
{	
	BYTE Status,Mode;
	DWORD dwAddr;
	
	//RAM�����ݴ����EE�ָ�
	if( ( ButtonCoverOldStatus > 1 )
	|| ( (LowPowerButtonCoverData.CheckButtonCoverStatus != 0x12345678)
	   &&(LowPowerButtonCoverData.CheckButtonCoverStatus != 0xffffffff)))
	{
		ButtonCoverOldStatus = 0;	
		memset(&LowPowerButtonCoverData, 0xff, sizeof(LowPowerButtonCoverData));
	}
	if( api_CheckMCUCLKIsFlf() == TRUE )//��⵱ǰƵ���Ƿ�Ϊ32768
	{
		Mode = 0;
	}
	else
	{
		Mode = 0x55;
	}
	//���ϸǼ��
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
		ButtonCoverOldStatus = Status;//��ʼ�Ͱ�״̬ˢ��һ�� ��ֹ���濪EE��¼���ϸǵ��¸�λ
		LowPowerButtonCoverData.CheckButtonCoverStatus = 0x12345678;
		if( Status == 0 )//��״̬��0������ť�ǽ���
		{
			if( api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverCloseTime) == TRUE )
			{
				//֮ǰ�Ѿ�������
				return;
			}
			//��ʱ��¼����ť�ǽ�����RAMֵ
			LowPowerButtonCoverData.ButtonCoverStatus[1] = Status;
			
			if( Mode == 0x55 )//���������,����Ҫ��E2��Դ
			{
				api_ReadMeterTimeLowPower();
			}
			else
			{
				api_ReadMeterTimeLowPower();
			}

			LowPowerButtonCoverData.ButtonCoverCloseTime = RealTimer;
		}
		else//��״̬����0������ť�ǿ�ʼ
		{
			if( ( api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverOpenTime) == TRUE )
			|| ( api_CheckClockYear(&LowPowerButtonCoverData.ButtonCoverCloseTime) == TRUE ))
			{
				//֮ǰ�Ѿ����������ߵ͹����ڼ䷢��������ť�ǽ���
				return;
			}
			LowPowerButtonCoverData.ButtonCoverStatus[0] = Status;
			
			if( Mode == 0x55 )//���������,����Ҫ��E2��Դ
			{
				api_ReadMeterTimeLowPower();
			}
			else
			{
				api_ReadMeterTimeLowPower();
			}

			LowPowerButtonCoverData.ButtonCoverOpenTime = RealTimer;
		}
		//����д��EE
		//һ��Ҫд��RAM֮���ٿ�����ƵHRC ��ֹ���ĸ�CPU��λ���¿���δ��¼
		if( Mode != 0x55 )
		{
			api_MCU_SysClockInit( ePowerDownMode );//�л���5.5Mʱ��
		}
		POWER_EEPROM_OPEN;
		dwAddr = GET_CONTINUE_ADDR( EventConRom.InstantEventRecord.LowPowerButtonCoverData );
		api_WriteToContinueEEPRom(	dwAddr, sizeof(LowPowerButtonCoverData), (BYTE *)&LowPowerButtonCoverData);
		EEPROM_INIT_LOW_POWER;
		POWER_EEPROM_CLOSE;	
		
		if( Mode != 0x55 )
		{
			HTMCU_GoToLowSpeed(); //�л�����Ƶ32.768ʱ��
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
