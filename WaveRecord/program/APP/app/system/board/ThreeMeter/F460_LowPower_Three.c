//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.26
//����		��Ȫcpu�����͹��Ľӿ��ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "F460_LowPower_Three.h"
//#include "PowerErr.h"
//#include "LostAllV.h"

#if( (CPU_TYPE == CPU_HC32F460) && (MAX_PHASE == 3) ) //��ʱ��cpu���壬���Կ����õ����ඨ��
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//���Ѽ�ʱ��
WORD WakeUpTimer;
WORD MaxWakeupTimer;
__no_init BYTE g_byDownPowerTime;

#if( SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH )
WORD WakeUpCountOld;
WORD WakeUpCountNew;
BYTE DeltaTestNo;
WORD DeltaTest[MAX_PORT_INT_NUM];
#endif

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
extern void IrPWMEnable(BOOL enNewSta);
//-----------------------------------------------
//				��������
//-----------------------------------------------
#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )

#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
//-----------------------------------------------
//��������: ����͹����������ú�����չ���
//
//����: 	Type[in] 0--�ɴ�ѭ������͹���	1--�͹�����ѭ������
//����ֵ:  	��
//
//��ע:������������ǵ�ʱ����ղ����źţ���Ϊ���ù����ܵķ�ʽ
//-----------------------------------------------
static void PowerDownOpenIRReceive( BYTE Type )
{

}

//-----------------------------------------------
//��������: ����͹����������ú�����չ���
//
//����: 	��
//����ֵ:  	��
//
//��ע:������������ǵ�ʱ����ղ����źţ���Ϊ���ù����ܵķ�ʽ
//-----------------------------------------------
static void PowerDownCloseIRReceive( void )
{

}

//-----------------------------------------------
//��������: �׹����²�������
//
//����: 	��
//����ֵ:  	��
//
//��ע:ǰ7��ÿ3���100ms������գ���ʱ����Ϊ1mA���ң�ƽ�����ӹ���
//-----------------------------------------------
static void ProcLowPowerIrComm(void)
{

}
#endif

void ProcContinueFramWakeup( void )
{

}

#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
void ClearIRWakeUpData()
{

}
#endif//#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
#endif//#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )


//-----------------------------------------------
//��������: �ڻ���״̬�µĵ͹������񣬶�ʱ����͹���״̬
//
//����: 	��
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_LowPowerTask(void)
{
	//1���ӵ�
	if( api_GetTaskFlag(eTASK_LOWPOWER_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_LOWPOWER_ID,eFLAG_SECOND);

		#if( SEL_EVENT_METERCOVER == YES )
		ProcLowPowerMeterCoverRecord( 0x55 );//����Ǽ��
		#endif
		#if( SEL_EVENT_BUTTONCOVER == YES )
		ProcLowPowerButtonCoverRecord( 0x55 );//����ť�Ǽ��
		#endif

		WakeUpTimer ++;

		if( (WakeUpTimer >= MaxWakeupTimer) || (WakeUpTimer >= 500) )//500��ǿ�ƽ���͹���
		{
			WakeUpTimer = 0;

			//�ǵ͹���״̬�����е͹���״̬����
			api_EnterLowPower(eFromWakeupEnterDownMode);
		}
	}
}

//---------------------------------------------------------------
//��������: �͹��İ�������
//
//����: 	ePOWER_MODE Type - ģʽ(�ϵ�/�͹���/�͹��Ļ���)
//
//����ֵ:
//
//��ע:
//---------------------------------------------------------------
void LowPowerKeyConfig( ePOWER_MODE Type )
{

}

//-----------------------------------------------
//��������: �رյ�Ƭ���ڲ���������ģ�飬���ж�
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void HTMCU_DisableSelfModule(void)
{

}


//-----------------------------------------------
//��������: �͹���ʱִ�еĿ��ǡ���Դ�쳣�ȼ��
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void LowPowerCheck( BYTE Type )
{
    // EnableLowPowerPort();//�����͹��Ļ���������ı�Ҫ�˿ڣ����翪�Ǽ��
    
    //���ϸǼ��
    #if( SEL_EVENT_METERCOVER == YES )
    ProcLowPowerMeterCoverRecord( 0x00 );
    #endif
    
    //��β�Ǽ��
    #if( SEL_EVENT_BUTTONCOVER == YES )
    ProcLowPowerButtonCoverRecord( 0x00 );
    #endif
    
    //��Դ�쳣���
    #if( SEL_EVENT_POWER_ERR == YES )
    PowerErrCheck( Type );
    #endif
    
    //ȫʧѹ���
    #if( SEL_EVENT_LOST_ALL_V == YES )
	AllLostVCheck( Type );
	#endif
    
    //DisableLowPowerPort();//�رողſ����Ķ˿�
}


//-----------------------------------------------
//��������: �ر�������Χ�����ĵ�Դ
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void CloseAllPowerExceptESAM(void)
{
    CLOSE_BACKLIGHT;
    // POWER_SAMPLE_CLOSE;	// �Ų��ǵļ���оƬ��FLASH����SPI��ֻ�رյ�Դ�޷�ʹ����ֹͣ����
    POWER_LCD_CLOSE;
    // POWER_FLASH_CLOSE;
    POWER_HALL_CLOSE;
    POWER_HONGWAI_REC_CLOSE;
    POWER_HONGWAI_TXD_CLOSE;
    POWER_CV485_CLOSE;
    // LIGHT_RUN_STOP;
    LIGHT_U_STOP(0);
    LIGHT_U_STOP(1);
    LIGHT_U_STOP(2);
    PULSE_LED_OFF;
    WARN_ALARM_LED_OFF;
	// TX_LED_OFF;
    RELAY_LED_OFF;
    #if( PREPAY_MODE == PREPAY_LOCAL )
    BEEP_OFF;
    #endif
}


//-----------------------------------------------
//��������: ��Ƭ���л�����Ƶ32.768KHz������
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void HTMCU_GoToLowSpeed(void)
{

}

static void LowPowerWakeup( void )
{
	if( IsExitLowPower == 10 )
	{
		DISABLE_CPU_INT;		// �ر��ж�
		
		//!!!!!!�ڴ��ʼ��һ��Ҫ������ǰ�棬��Ϊ�����˶Ѳ���!!!!!!
		//api_InitAllocBuf();
		api_MCU_SysClockInit( ePowerOnMode );//�����Ҫ��͹��Ļ��ѵ�����£�����Ҫ��ͨѶ
		api_InitSysTickTimer( ePowerOnMode );
		InitPort( ePowerWakeUpMode );
		
		LowPowerKeyConfig( ePowerWakeUpMode );//�͹��Ļ��Ѻ󰴼�����
		
//		memset( Serial[eIR].ProBuf, 0x00, MAX_PRO_BUF_REAL );
		
		CloseAllPowerExceptESAM();
		POWER_FLASH_OPEN;
		POWER_SAMPLE_OPEN;
		POWER_HONGWAI_REC_OPEN;
		POWER_HONGWAI_TXD_OPEN;
		POWER_LCD_OPEN;

		ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 );
//		SerialMap[eIR].SCIInit(SerialMap[eIR].SCI_Ph_Num);//����˿ڳ�ʼ��
		api_FreshParaRamFromEeprom(0);
		api_FreshParaRamFromEeprom(1);
		api_PowerUpLcd( ePowerDownMode );
		api_CheckTimeTable(FALSE);
		api_LowPowerWakeUpPrePay();
		api_SetSysStatus( eSYS_LOW_POWER_WAKEUP );
		RESET_STACK_POINTER;
		//�����ж�  �ڽ���ѭ��֮ǰ�Ŵ��ж�
		ENABLE_CPU_INT;
		
		MainTask();
	}
}
//-----------------------------------------------
//��������: �ϱ��ز�ͨ���Ĺܽ�����
//
//����: 	0x00:�ϱ�ǰ
//		0x55:�ϱ���
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ReportCRWakeUpConfig(BYTE Type)
{

}

//-----------------------------------------------
//��������: ����͹��ļ���Ӧģ�鴦��
//
//����: 	Type[in]
//					eFromOnInitEnterDownMode	���ϵ��ʼ������͹���
//					eFromOnRunEnterDownMode		�ϵ����к����͹���
//					eFromWakeupEnterDownMode	�͹��Ļ��Ѻ��ٽ���͹���
//					eFromDetectEnterDownMode	�͹��Ľ��е�Դ�쳣��ȫʧѹ�����ٽ���͹���
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
extern void SwapAHour(void);
void api_EnterLowPower(eENTER_DOWN_MODE Type)
{
	BYTE	PowerDownCompleteFlag = 0;
	
	// DISABLE_CPU_INT;
    CLEAR_WATCH_DOG;

	//���ⰴ�����Ѻ�����
	if( Type == eFromOnRunEnterDownMode )
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
	api_PowerDownWaveRecord();

    api_PowerDownLcd();//����
    
    CloseAllPowerExceptESAM();//�ر�������Χ�����Ĺ���
    ESAMSPIPowerDown( ePowerDownMode );
    POWER_FLASH_OPEN;//һ��Ҫ��eeprom��Դ����Ϊ��Ҫ��eepromд����

    //ֻ������������ʱ��ת�棬��������⻽�Ѻ����͹����ǲ�����ת��ġ�
 	if( Type == eFromOnRunEnterDownMode )
 	{      
        if( g_PowerOnSecTimer == 0 )
        {
        	api_PowerDownEnergy();         // �������ת�洦��
        	api_PowerDownPrePay();
        }
        
        // api_PowerDownFreeze();
        // api_PowerDownEvent();           // �����¼�����

        #if (SEL_SEARCH_METER == YES )	
        api_PowerDownSchClctMeter();
        #endif
        
        #if (SEL_TOPOLOGY == YES )
        TopoPowerDown();
        #endif

		#if( SEL_AHOUR_FUNC == YES )
		SwapAHour();
		#endif   
		//�ɼ���ص���洢������Ե���ת��ʱ�� 
		if( g_PowerOnSecTimer == 0 )
        {
        	// api_PowerDownSave();
        }
 	}
 	//�Ƿ�Ϊ�ϵ�����͹���
 	if((Type == eFromOnRunEnterDownMode) || (Type == eFromOnInitEnterDownMode))
 	{
 		api_WriteFreeEvent(EVENT_ENTER_LOW_POWER, Type);//��¼�����¼�
 	}
	
 	api_PowerDownSample();
 	
 	api_InitNoInitFlag();

 	CLEAR_WATCH_DOG;//�忴�Ź�
 	
    //�������ϵ絽������ɶ������ô˱�־
	if( Type == eFromOnRunEnterDownMode )
	{
		PowerDownCompleteFlag = 0xaa;
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
 	//�ر�û���õ���ģ��(WDT,TimerB,A/D,������SCI,SPI��DCO,Comparator_A,lcd,����I/O��)
 	InitPort( ePowerDownMode );//�����Ž��е͹��Ĵ���
 	LOWPOWER_INVALID_RELAY_CTRL;
 	
    HTMCU_DisableSelfModule();//�ر�mcu����ģ��
	
    CloseAllPowerExceptESAM();//�ر�������Χ�����Ĺ���
    ESAMSPIPowerDown( ePowerDownMode );

	IsExitLowPower = 0;
	api_ClrSysStatus( eSYS_LOW_POWER_WAKEUP );
	
	if( Type == eFromOnRunEnterDownMode )
	{
		#if( SEL_EVENT_POWER_ERR == YES )
		wPowerErrFlag = 0;
		#endif
		
		g_byDownPowerTime = 0;
	}
	
	#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	//�������ú���
	PowerDownOpenIRReceive(0);	
	#endif
    
 	CLEAR_WATCH_DOG;
	HTMCU_GoToLowSpeed();//�л�����Ƶ32.768ʱ��
    
	#if( SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH )
    ClearIRWakeUpData();
    #endif
	
//    LowPowerKeyConfig( ePowerDownMode );//�͹����°�������
//    api_MCU_RTCInt( );//����RTC���жϣ��жϺ�ỽ�ѵ�Ƭ����hold״̬
//	ENABLE_CPU_INT;
	
//	FunctionConst(LOWPOWER_TASK);

	api_SetSysStatus(eSYS_STATUS_RUN_LOW_POWER);

	while(1)//�͹���ѭ��
	{
		CLEAR_WATCH_DOG;//�忴�Ź�
		if( api_CheckSysVol( eSleepDetectPowerMode ) == TRUE )
		{
			Reset_CPU();
		}
//		
//		Hold_Sleep();//����holdģʽ
//		//����ȴ����жϻ��Ѻ�����ִ��
//		CLEAR_WATCH_DOG;//�忴�Ź�
//		ProcLowPowerMeterReport();
//		if( api_CheckSysVol( eSleepDetectPowerMode ) == TRUE )
//		{
//			Reset_CPU();
//		}
//
//		LowPowerWakeup();
//
//		LowPowerCheck( Type );

		api_ProtocolTask();
		//LCD����
		FunctionConst(LCD_RUN_TASK);
		api_LcdTask();
		FunctionConst(RTC_RUN_TASK);
		api_RtcTask();//RTC�������Э������֮�󣬱��ⶳ���־������Э�����RTC���¶���ʱ�䲻��
//		
//		#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
//		ProcLowPowerIrComm();
//		#endif


	}
}

//-----------------------------------------------
//��������: ��ȡ�����Ƿ���ɱ�־
//
//����: 	��

//����ֵ:  	������ɵ��緵��TRUE,δ��ɵ��緵��FALSE
//
//��ע:   
//-----------------------------------------------
BOOL api_GetPowerDownCompleteFlag( void )
{
    BYTE	PowerDownCompleteFlag;
	
    api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	
	if( PowerDownCompleteFlag == 0xaa )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}




#endif//#if( (CPU_TYPE == CPU_HC32F460) && (MAX_PHASE == 3) ) //��ʱ��cpu���壬���Կ����õ����ඨ��

