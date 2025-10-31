//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.26
//����		��Ȫcpu�����͹��Ľӿ��ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "LowPower_MeasuringSwitch.h"
#include "PowerErr.h"
#include "LostAllV.h"

#if( (CPU_TYPE == CPU_HC32F4A0) && (MAX_PHASE == 3) ) //��ʱ��cpu���壬���Կ����õ����ඨ��
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
	if( LowPowerSecCount < IR_WAKEUP_LIMIT_TIME )
	{
		if( WakeUpTimer == 0 )
		{
			if( (LowPowerSecCount % 3) == 0 )
			{
				PowerDownOpenIRReceive( 1 );
				api_Delay100us( 2 );
				PowerDownCloseIRReceive();
			}
		}
	}
}
#endif

void ProcContinueFramWakeup( void )
{
	BYTE FramWakeupNo;
	WORD tRXWPoint;
	
	//ˢ���µ�ǰʱ��
	if( LowPowerSecCount > IR_WAKEUP_LIMIT_TIME )
	{
		//�����Ѿ�����7�죬��������
		#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
		DeltaTestNo = 0;
		#elif(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
		PowerDownCloseIRReceive();
		#endif
		return;
	}
	
	if( IsExitLowPower == 10 )
	{
		return;
	}

	#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	tRXWPoint = Serial[eIR].RXWPoint;
	//����������յ�15���ֽ��Ƿ��з���Ҫ������ݣ�û�к��ⷢ�͵�ʱ��򿪽������յ����룬ָ��仯������Ҫ��ǰ��
	for( FramWakeupNo = 0; FramWakeupNo < 15; FramWakeupNo++ )
	{
		//ǰ3���ֽڿ��Բ���
		if( (tRXWPoint - 3) > MAX_PRO_BUF )
		{
			return;
		}
		if( Serial[eIR].ProBuf[tRXWPoint - 1] == 0x04 )
		{
			if( Serial[eIR].ProBuf[tRXWPoint - 2] == 0x11 )
			{
				if( Serial[eIR].ProBuf[tRXWPoint - 3] == 0x68 )
				{
					IsExitLowPower = 10;
					break; 
				}
			}			
		}
		tRXWPoint--;
	}
	#elif(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
	if( DeltaTestNo < 15 )
	{
		DeltaTestNo = 0;	
		return;
	}
	
	for(FramWakeupNo=15; FramWakeupNo<(MAX_PORT_INT_NUM-8); FramWakeupNo++)
	{
		//68 11 04 �����룺68 45 50 68 54 82������5���������ڲ�ͬ�ƻ��ڷ����ֽڼ���ʱ��һ�£����Բ�ѯ68 68 82������������
		//5438��ʱ�Ķ�ʱ����16384��ʵ�ʶ�Ӧ5λ 3λ 4λ 5λ 4λ 6λ 1200BPS
		//if( (DeltaTest[FramWakeupNo]<141) && (DeltaTest[FramWakeupNo]>131) ) ����6025���˲����ã���һ��������68���ܵ��ֽڼ���ʱ���� �ʲ������ж�-���ĺ�
		{
			//for(FramWakeupNoj = FramWakeupNo+1; FramWakeupNoj<FramWakeupNo+5; FramWakeupNoj++)
			{
				if((DeltaTest[FramWakeupNo]<141) && (DeltaTest[FramWakeupNo]>131))
				{
					//for(FramWakeupNok = FramWakeupNoj+1; FramWakeupNok<FramWakeupNoj+3; FramWakeupNok++)
					{
						if((DeltaTest[FramWakeupNo+2]<169) && (DeltaTest[FramWakeupNo+2]>159))
						{							
							//�ú��⻽�ѱ�־
							//RamDBase.FlagByte.ExtSysStatus1 |= 0x2000;
							IsExitLowPower = 10;
							DeltaTestNo = 0;
							return;
						}
					}
				}
			}
		}
	}
	
	DeltaTestNo = 0;
	 
	#endif	
}

#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
void ClearIRWakeUpData()
{

}
#endif//#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
#endif//#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )




//-----------------------------------------------
//��������: �����ж˿����óɵ͹���ģʽ������JTAG�˿��⣩�����ж˿ڹ���ʧЧ������spi��eeprom�˿ڣ�
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void InitLowPowerPort( void )
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
    POWER_SAMPLE_CLOSE;
    POWER_LCD_CLOSE;
    POWER_EEPROM_CLOSE;
	POWER_FLASH_CLOSE;
	POWER_HALL_CLOSE;
    POWER_HONGWAI_REC_CLOSE;
    POWER_HONGWAI_TXD_CLOSE;
    POWER_CV485_CLOSE;
    

    PULSE_LED_OFF;
    WARN_ALARM_LED_OFF;
    RELAY_LED_OFF;
    BEEP_OFF;

	LIGHT_RUN_STOP;
	LIGHT_A_STOP;
    LIGHT_B_STOP;
    LIGHT_C_STOP;
    LIGHT_N_STOP;
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
		api_InitAllocBuf();
		api_MCU_SysClockInit( ePowerOnMode );//�����Ҫ��͹��Ļ��ѵ�����£�����Ҫ��ͨѶ
		InitPort( ePowerDownMode );
		
		memset( Serial[eIR].ProBuf, 0x00, MAX_PRO_BUF );
		
		CloseAllPowerExceptESAM();
		POWER_EEPROM_OPEN;
		POWER_SAMPLE_OPEN;
		POWER_HONGWAI_REC_OPEN;
		POWER_HONGWAI_TXD_OPEN;
		POWER_LCD_OPEN;

		ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 );
		SerialMap[eIR].SCIInit(SerialMap[eIR].SCI_Ph_Num);//����˿ڳ�ʼ��
		api_FreshParaRamFromEeprom(0);
		api_FreshParaRamFromEeprom(1);
		api_PowerUpLcd( ePowerDownMode );
		api_CheckTimeTable(FALSE);
		api_SetSysStatus( eSYS_LOW_POWER_WAKEUP );
		RESET_STACK_POINTER;
		//�����ж�  �ڽ���ѭ��֮ǰ�Ŵ��ж�
		ENABLE_CPU_INT;
		
		MainTask();
	}
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
void api_EnterLowPower(eENTER_DOWN_MODE Type)
{
	BYTE	PowerDownCompleteFlag = 0;
	
	DISABLE_CPU_INT;
    CLEAR_WATCH_DOG;
	
	//���ⰴ�����Ѻ�����
	if( Type == eFromOnRunEnterDownMode )
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
    api_PowerDownLcd();//����
    
    CloseAllPowerExceptESAM();//�ر�������Χ�����Ĺ���
    ESAMSPIPowerDown( ePowerDownMode );
    POWER_EEPROM_OPEN;//һ��Ҫ��eeprom��Դ����Ϊ��Ҫ��eepromд����

    //ֻ������������ʱ��ת�棬��������⻽�Ѻ����͹����ǲ�����ת��ġ�
 	if( Type == eFromOnRunEnterDownMode )
 	{      
        if( g_PowerOnSecTimer == 0 )
        {
        	api_PowerDownEnergy();         // �������ת�洦��
        	api_PowerDownPrePay();
        }
        
        api_PowerDownFreeze();
        api_PowerDownEvent();           // �����¼�����
        
        //��¼�¼�
	 	api_WriteFreeEvent(EVENT_ENTER_LOW_POWER, 0);    
 	}
 	
 	api_PowerDownSample();
 	
 	api_PowerDownFlag();

 	CLEAR_WATCH_DOG;//�忴�Ź�
 	
    //�������ϵ絽������ɶ������ô˱�־
	if( Type == eFromOnRunEnterDownMode )
	{
		PowerDownCompleteFlag = 0xaa;
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
 	//�ر�û���õ���ģ��(WDT,TimerB,A/D,������SCI,SPI��DCO,Comparator_A,lcd,����I/O��)
 	InitLowPowerPort();//�����Ž��е͹��Ĵ���
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
	
	Reset_CPU();// ֱ�Ӹ�λ��û�е͹���
	
	//#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	//�������ú���
	//PowerDownOpenIRReceive(0);	
	//#endif
    
 	//CLEAR_WATCH_DOG;
	//HTMCU_GoToLowSpeed();//�л�����Ƶ32.768ʱ��
    
	//#if( SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH )
    //ClearIRWakeUpData();
    //#endif
    
    //HT_GPIOD->PTDIR &= ~GPIO_Pin_13;//�Ϸ�����
    //HT_GPIOD->PTDIR &= ~GPIO_Pin_14;//�·�����
    //HT_GPIOA->PTDIR &= ~GPIO_Pin_4; //�ϸǼ��
    //HT_GPIOD->PTDIR &= ~GPIO_Pin_15;//β�Ǽ��
    //api_MCU_RTCInt( );//����RTC���жϣ��жϺ�ỽ�ѵ�Ƭ����hold״̬
	//ENABLE_CPU_INT;
	
	//FunctionConst(LOWPOWER_TASK);

	//while(1)//�͹���ѭ��
	//{
    //    Hold_Sleep();//����holdģʽ

    //    //����ȴ����жϻ��Ѻ�����ִ��
    //    CLEAR_WATCH_DOG;//�忴�Ź�
        
    //    if( api_CheckSysVol( eSleepDetectPowerMode ) == TRUE )
    //    {
    //    	Reset_CPU();
    //    }
        
    //    LowPowerWakeup();
        
    //    LowPowerCheck( Type );
		
	//	#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	//	ProcLowPowerIrComm();
	//	#endif
	//}
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




#endif//#if( (CPU_TYPE == CPU_HC32F4A0) && (MAX_PHASE == 3) ) //��ʱ��cpu���壬���Կ����õ����ඨ��

