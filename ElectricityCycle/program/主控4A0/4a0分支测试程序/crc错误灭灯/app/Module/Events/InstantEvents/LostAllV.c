//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.10.26
//����		ȫʧѹ�¼������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "LostAllV.h"
#include "F4A0_LowPower_Three.h"

#if( SEL_EVENT_LOST_ALL_V == YES )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//������ȫʧѹ״̬��־
static __no_init DWORD PowerDown_LostALLVFlag;//0x12341234��ʼ״̬ 0x5AA55AA5ȫʧѹ 
__no_init TLostAllVData LostAllVData[MAX_LOST_ALL_V];

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
// 0x300d ȫʧѹ��ʼ���Ĺ����������Ա�oad������������
static const DWORD LostAllVDefaultOadConst[] =
{
	1,//��������ĸ���
	0x20012200,	// ����ʱ�̵���

};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void TestLostAllV( void );

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������:ͬ������ֵ
//
//����: 
//	Type[in]:	0��1��2��3����A\B\C\N���� 
//	Value[in]:	����ֵ 4λС�� 
 
//����ֵ:				��
//��ע:ÿ�β���ʱͬ����ֵ�����ڴ���ȫʧѹ��
//-----------------------------------------------
void api_SetLostAllVCurrent( BYTE Type , long Value)
{
	if( Type > 3 )
	{
		return;
	}
	
	LostAllVData[0].Current[Type] = Value / 10;
}


void AllLostVCheck( BYTE Type )
{
	if( IsExitLowPower == 30 )
	{
		//ֻ�н���������Ž���ȫʧѹ���
		if( ((Type==eFromOnRunEnterDownMode)||(Type==eFromWakeupEnterDownMode)||(Type==eFromDetectEnterDownMode)) 
			&& (g_byDownPowerTime<(START_TEST_LOST_ALLV+10)) )
		{
			switch( g_byDownPowerTime )
			{
			case START_TEST_LOST_ALLV:
				api_MCU_SysClockInit( ePowerDownMode );//�л���5.5Mʱ��
				TestLostAllV();
				break;
			case (START_TEST_LOST_ALLV+1):
			case (START_TEST_LOST_ALLV+2):
				TestLostAllV();
				break;
			default:
				RESET_STACK_POINTER;
				api_EnterLowPower( eFromDetectEnterDownMode );
			}
		}
		else
		{
			IsExitLowPower = 0;
		}
	}
}
//�ϵ�ȫʧѹ������
//���ܣ����ݵ���ǰȫʧѹ״̬��͹��ļ��״̬����ȫʧѹ�¼�����Ӧ�Ĵ���
//		���͹���ǰ״̬				�͹��ļ��״̬					�ϵ紦��
//		����						δ���					������
//		����						����ȫʧѹ				������
//		����						ȫʧѹ					��¼ȫʧѹ��ʼ����λȫʧѹ��־
//		ȫʧѹ						δ���					��λȫʧѹ��־
//		ȫʧѹ						��ȫʧѹ				��λȫʧѹ��־
//		ȫʧѹ						����ȫʧѹ				��¼ȫʧѹ����

void PowerUpDealLostAllV( void )
{
	BYTE TmpOADBuf[10];
	DWORD LostALLV_LostPowerFlag,dwSec,dwSec1,dwSec2;//�ϵ��־��ʼ��Ϊ����״̬
	WORD Len;
	TRealTimer TmpRealTimer,TmpRealTimer2,TmpPowerDownRealTimer;
	TDLT698RecordPara TmpDLT698RecordPara;
	
    if( PowerDown_LostALLVFlag == eOtherFlag )//�쳣��λʱ��δ��⵽����
    {
        PowerDown_LostALLVFlag = eNormalFlag;
    }
    else
    {
    	api_GetPowerDownTime( (TRealTimer *)&TmpPowerDownRealTimer );
    	dwSec = api_CalcInTimeRelativeSec( (TRealTimer *)&RealTimer );
    	dwSec1 = api_CalcInTimeRelativeSec( (TRealTimer *)&TmpPowerDownRealTimer );
    	if( dwSec > dwSec1 )
    	{
			dwSec2 = (dwSec-dwSec1);
    	}
    	else
    	{
			dwSec2 = 0;
    	}
    	
        if( dwSec2 >= 60 )
        {
            if( PowerDown_LostALLVFlag == eNormalFlag )//������糬��1���ӣ��������־Ϊ��������λ��⵽����
            {
                PowerDown_LostALLVFlag = eLostPowerFlag;
            }
        }
    }
    
    LostALLV_LostPowerFlag = eNormalFlag;//�ϵ��־��ʼ��Ϊ����״̬
    memset( (BYTE *)&TmpRealTimer2, 0, sizeof(TRealTimer) );//Ϊɶ��ȫ0@@@@��ا��
    memset( (BYTE *)&TmpRealTimer, 0xff, sizeof(TRealTimer) );   
    TmpOADBuf[0] = 0x20;
    TmpOADBuf[1] = 0x20;
    TmpOADBuf[2] = 0x02;
    TmpOADBuf[3] = 0x00;
    TmpDLT698RecordPara.pOAD = TmpOADBuf;//pOADָ��
    TmpDLT698RecordPara.OADNum = 1;		//OAD����
    TmpDLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;//ѡ�����
    TmpDLT698RecordPara.TimeOrLast = 0x01;	//��һ��
    TmpDLT698RecordPara.Phase = ePHASE_ALL;
    TmpDLT698RecordPara.OI = 0x300d;					
	Len = api_ReadProRecordData( 0, &TmpDLT698RecordPara, sizeof(TRealTimer), (BYTE*)&TmpRealTimer );
	if( (memcmp( (BYTE *)&TmpRealTimer, (BYTE *)&TmpRealTimer2, sizeof(TRealTimer) ) == 0)
	||(Len == 1) )//ȫʧѹ�¼����һ����¼����ʱ��Ϊnull
	{
        LostALLV_LostPowerFlag = eLostALLVFlag;
	}

    if( LostALLV_LostPowerFlag == eLostALLVFlag )//�������״̬Ϊδ���״̬
    {
        if( PowerDown_LostALLVFlag == eNormalFlag )//�������״̬Ϊδ��⵽
        {
            //��λȫʧѹ��־
            EventStatus.CurrentStatus[eSUB_EVENT_LOST_ALL_V/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_ALL_V%8));
    	    api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V );
        }
        else if( PowerDown_LostALLVFlag == eLostALLVFlag )//�������״̬Ϊȫʧѹ
        {
            //��λȫʧѹ��־
            EventStatus.CurrentStatus[eSUB_EVENT_LOST_ALL_V/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_ALL_V%8));
    	    api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V );
        }
        else//����������������״̬Ϊ���紦��
        {
            //��¼ȫʧѹ����
		    SaveInstantEventRecord( eEVENT_LOST_ALL_V_NO, 0, EVENT_END, eEVENT_ENDTIME_PWRDOWN );
        }
    }
    else//����ǰ״̬Ϊ����״̬
    {
        if( PowerDown_LostALLVFlag == eNormalFlag )//�������״̬Ϊδ���״̬
        {
            //null ������
        }
        else if( PowerDown_LostALLVFlag == eLostALLVFlag )//�������״̬Ϊȫʧѹ
        {
            //��¼ȫʧѹ��ʼ ��λȫʧѹ��־
		    SaveInstantEventRecord( eEVENT_LOST_ALL_V_NO, 0, EVENT_START, eEVENT_ENDTIME_PWRDOWN );
            EventStatus.CurrentStatus[eSUB_EVENT_LOST_ALL_V/8] |= (BYTE)(0x01<<(eSUB_EVENT_LOST_ALL_V%8));
		    api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V );
        }
        else//����������������״̬Ϊ���紦��
        {
        
        }
    }
    
    memset( (void *)&LostAllVData, 0, sizeof(LostAllVData) );
    PowerDown_LostALLVFlag = eOtherFlag;//��λ��ʼ״̬
}


void PowerDownDealLostAllV( void )
{
	// �۲��ѭ���еļ���У��Ƿ���ȫʧѹ����
	PowerDown_LostALLVFlag = eNormalFlag;//��ʼ״̬
	memset( (void *)&LostAllVData, 0, sizeof(LostAllVData) );
}


static void TestLostAllV( void )
{
//	BYTE i;
//	DWORD tdw;
//	float tf;
//
//	if( g_byDownPowerTime == START_TEST_LOST_ALLV )
//	{
//        InitPort(ePowerWakeUpMode);
//		POWER_SAMPLE_OPEN;
//		POWER_FLASH_OPEN;
//		//POWER_ESAM_OPEN;
//		api_Delayms(10);
//		CLEAR_WATCH_DOG;
//		api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
//		api_Delayms(10);
//
//		if( api_InitSampleChip(0) == FALSE )
//		{
//		    PowerDown_LostALLVFlag = eLostPowerFlag;
//		    g_byDownPowerTime = (START_TEST_LOST_ALLV+3);//����ȫʧѹ���
//			RESET_STACK_POINTER;
//			api_EnterLowPower( eFromDetectEnterDownMode );
//		}
//	}
//
//	if( g_byDownPowerTime==(START_TEST_LOST_ALLV+2) )
//	{
//        for(i=0; i<3; i++)
//    	{
//    		LostAllVData[0].Current[i] = 0;
//    		if( (MeterTypesConst==METER_3P3W) && (i == 1) )
//    		{
//    			continue;
//    		}
//
//    		// ��������
//    		tdw = HT7627S_ReadMeasureData( All_RMS_Ia + i*4 );
//    		if( tdw == 0x00ffffff )//����ֵ�쳣�� �������ж�
//    		{
//               memset( (void *)&LostAllVData, 0, sizeof(LostAllVData) );
//               break;
//    		}
//    		
//    		tf = (float)tdw*10000/TWO_POW13;
//    		tf /= ((float)(6 * CURRENT_VALUE) / 5);
//    		LostAllVData[0].Current[i] = (DWORD)((tf * wMeterBasicCurrentConst) / 10000 + 0.5); //������λС��
//    	}
//    	
//    	//������оƬ�������ʸ����
//    	tdw = HT7627S_ReadMeasureData( All_RMS_In );
//	    if( tdw == 0x00ffffff )//����ֵ�쳣�� �������ж�
//		{
//           memset( (void *)&LostAllVData, 0, sizeof(LostAllVData) );
//		}
//		else
//		{
//            tf = (float)tdw*10000/TWO_POW12;
//            tf /= ((float)(6 * CURRENT_VALUE) / 5);
//            LostAllVData[0].Current[3] = (DWORD)(tf * ((float)wMeterBasicCurrentConst / 10000) + 0.5); //������λС��
//		}
//    	
//    	for(i=0; i<3; i++)
//    	{
//    		if( LostAllVData[0].Current[i] > (DWORD)wMeterBasicCurrentConst * 5 / 100 ) //������3λС�� 5%Ib
//    		{
//    			break;
//    		}
//    	}
//
//    	if( i < 3 )// ˵����ȫʧѹ
//    	{
//    		PowerDown_LostALLVFlag = eLostALLVFlag;
//    	}
//    	else
//    	{
//    		PowerDown_LostALLVFlag = eLostPowerFlag;
//    	}
//
//		RESET_STACK_POINTER;
//		api_EnterLowPower( eFromDetectEnterDownMode );
//	}
}


// ȫʧѹ����
// ȫʧѹ��¼��״̬���
BYTE GetLostAllVStatus(BYTE Phase)
{
	BYTE Status, i;
	WORD wRemoteU;
	DWORD dwRemoteI;

	Status = 0;

	// �ж������ѹ
	for(i=0; i<3; i++)
	{
		if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
		{
			Status |= 0x02;
			continue;
		}

		// �����ѹ�����ٽ��ѹ
        api_GetRemoteData( PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, sizeof(WORD), (BYTE *)&wRemoteU);
		if( wRemoteU < wStandardVoltageConst * 6 / 10 ) //0.6Un 1λС��
		{
			// ����ʧѹ
			Status |= (0x01<<i);
		}
	}

	// ��������ѹ�������ٽ��ѹ�����жϵ���
	if(Status == 0x07)
	{
		Status = 0;

		// �ж��������
		for(i=0; i<3; i++)
		{
			if( (MeterTypesConst == METER_3P3W)&&( i == 1 ) )
			{
				continue;
			}

			api_GetRemoteData( PHASE_A+REMOTE_I+0x1000*i, DATA_HEX, 4, sizeof(DWORD), (BYTE*)&dwRemoteI);
			if( dwRemoteI > (DWORD)dwMeterBasicCurrentConst * 50 / 100 ) // 5%Ib 4λС��
			{
				Status |= (0x01<<i);
			}
		}

		// �����������һ�����ʧѹ��������
		if( Status != 0 )
		{
			// ��Ϊȫʧѹ����
			 Status = 1;
		}
	}
	else
	{
		Status = 0;
	}

	return Status;
}


//��ʼ��ȫʧѹ�����������Ա�
void FactoryInitLostAllVPara( void )
{
	api_WriteEventAttribute( 0x300d, 0xff, (BYTE *)&LostAllVDefaultOadConst[0], sizeof(LostAllVDefaultOadConst)/sizeof(DWORD) );	// ȫʧѹ
}

#endif//#if( SEL_EVENT_LOST_ALL_V == YES )
