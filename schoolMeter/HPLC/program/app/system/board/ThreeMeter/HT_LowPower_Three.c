//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.26
//����		��Ȫcpu�����͹��Ľӿ��ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT_LowPower_Three.h"
#include "PowerErr.h"
#include "LostAllV.h"

#if( (CPU_TYPE == CPU_HT6025) && (MAX_PHASE == 3) ) //��ʱ��cpu���壬���Կ����õ����ඨ��
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
WORD ReadVBATLowFlag;
//
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
	if( LowPowerSecCount < IR_WAKEUP_LIMIT_TIME )
	{
		//���ܶ˿�
		HT_GPIOC->IOCFG |= (GPIO_Pin_0|GPIO_Pin_1);

		//���ù���2
		HT_GPIOC->AFCFG = 0;
		//�˿ڷ���
		HT_GPIOC->PTDIR &= 0xfffc;

		//��ʼΪ��
		HT_GPIOC->PTSET = 0x0003;

		//�򿪽��յ�Դ
		POWER_HONGWAI_REC_OPEN;
		if( Type == 0 )
		{
			SerialMap[eIR].SCIInit( SerialMap[eIR].SCI_Ph_Num ); //����˿ڳ�ʼ��
		}
		//����32768ʱ��Դ����1200����ͨ�Ų�����
		HT_UART1->SREL    = 0x0d;
	}
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
	//IO�˿�
	HT_GPIOC->IOCFG &= 0xfffc;

	//���ù���2
	HT_GPIOC->AFCFG = 0;
	//�˿ڷ���
	HT_GPIOC->PTDIR |= 0x0003;

	//��ʼΪ��
	HT_GPIOC->PTSET = 0x0003;

	//�رս��յ�Դ
	POWER_HONGWAI_REC_CLOSE;
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
	
	if( IsExitLowPower == 10 )
	{
		return;
	}
	
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
	// ������⻽�Ѽ�����
	WakeUpCountOld = 0;
	WakeUpCountNew = 0;
	DeltaTestNo = 0;
	memset((BYTE *)&DeltaTest,0,sizeof(DeltaTest));
	
	EnWr_WPREG();
	
	HT_INT->EXTIE |= 0x0100;//�����ش���
	HT_INT->EXTIF = 0;
	HT_INT->PINFLT |= 0x0001;
	NVIC_EnableIRQ( EXTI0_IRQn );
	
	//���ܶ˿�
  	HT_GPIOA->IOCFG |= GPIO_Pin_5;
	//���ù���2
    HT_GPIOA->AFCFG |= 0;
	//�˿ڷ���
    HT_GPIOA->PTDIR &= ~GPIO_Pin_5;
	//�������
    HT_GPIOA->PTOD |= 0;
	//��ʼΪ��
    HT_GPIOA->PTSET |= 0;
    //��ʼΪ��
    HT_GPIOA->PTCLR |= 0;
 	HT_GPIOA->PTUP |= 0;
	
	HT_CMU->CLKCTRL1 |= CMU_CLKCTRL1_TMR4EN;
	HT_TMR4->TMRDIV = 0;
	HT_TMR4->TMRPRD = 0xFFFF;
	HT_TMR4->TMRIE = 0;
	HT_TMR4->TMRCON = 0x0107;//ѡ��flfʱ�ӣ�32768�������ڶ�ʱģʽ
	
	DisWr_WPREG();
}
#endif//#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH)
#endif//#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )

//-----------------------------------------------
//��������: ���͹����¿��Ǽ��ʱ��
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void CheckLowPowerCoverCheckTime( void )
{
	if( SelThreeBoard == BOARD_HT_THREE_0132515 )
	{
		if( LowPowerSecCount > COVER_CHECK_LIMIT_TIME ) //���Ǽ�⵽��60Сʱ��رյ�Դ
		{
			if(POWER_COVER_INT)
			{
				POWER_COVER_CLOSE;
			}
		}
	}
}

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
		ProcLowPowerMeterCoverRecord();//����Ǽ��
		#endif
		#if( SEL_EVENT_BUTTONCOVER == YES )
		ProcLowPowerButtonCoverRecord();//����ť�Ǽ��
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
	if( Type == ePowerDownMode )
	{
		NVIC_DisableIRQ( EXTI5_IRQn );
		NVIC_DisableIRQ( EXTI6_IRQn );
		#if(SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_PHOTO_SWITCH)
		HT_INT->EXTIE = 0;
		#endif
		HT_INT->EXTIE |= 0x6000;			//ʹ���������ж�
		HT_INT->PINFLT |= 0x0060;			//ʹ�������˲�
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI5_IRQn );
		NVIC_EnableIRQ( EXTI6_IRQn );
	}
	else
	{
		NVIC_DisableIRQ( EXTI5_IRQn );
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE &= ~0x6000;			//�ر��������ж�
		HT_INT->PINFLT &= ~0x0060;			//�ر������˲�
		HT_INT->EXTIF = 0;
	}
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
	int i;  
  
    EnWr_WPREG();
    HT_UART0->UARTCON=0;
    HT_UART1->UARTCON=0;
    HT_UART2->UARTCON=0;
    HT_UART3->UARTCON=0;
    HT_UART4->UARTCON=0;
    HT_UART5->UARTCON=0;

    HT_SPI0->SPICON=0;
	HT_SPI3->SPICON=0;
    HT_TBS->TBSCON=0x6541;
    HT_TBS->TBSPRD	= 0x0004;
    
    HT_TMR0->TMRCON=0;
    HT_TMR1->TMRCON=0;
    HT_TMR2->TMRCON=0;
    
    //HT_LCD->LCDCON=0;
    HT_PMU->PMUCON=0;
    HT_INT->EXTIE=0;
    //HT_CMU->CLKCTRL1=0;//�ر�UART0-5,TMR0-3,SPI1
    //HT_CMU->CLKCTRL0&=~0x000f;//�ر�SPI0��IIC��LCD��
    
    DisWr_WPREG();

	//SystickΪ�ں��ж� ����ͨ�������жϷ�ʽ�ر� ֻ�ܹرմ�ģ��
	CLOSE_SYSTICK;
	
    for(i=0;i<31;i++)
    {
    	NVIC_ClearPendingIRQ( (IRQn_Type)i );//2016.8.1//ȫ�����ж�
        NVIC_DisableIRQ( (IRQn_Type)i );//2016.8.1//ȫ�����ж�
    }
}

//-----------------------------------------------
//��������: ���͹���ʱ��
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void CheckLowPowerCLK( void )
{
	//����Դ�쳣����ȫʧѹʱ����ʱ�ӵ�5.5M
	if(( IsExitLowPower != 30 ) && ( IsExitLowPower != 20))
	{
		//����͹�����ʱ�Ӳ���Flf ǿ���л�һ��ʱ�� ��ֹʱ��û�б��л�����
		if( api_CheckMCUCLKIsFlf() == FALSE )
		{
			HTMCU_GoToLowSpeed();
		}
	}
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
    ProcLowPowerMeterCoverRecord();
    #endif
    
    //��β�Ǽ��
    #if( SEL_EVENT_BUTTONCOVER == YES )
    ProcLowPowerButtonCoverRecord();
    #endif
    
    //��Դ�쳣���
    #if( SEL_EVENT_POWER_ERR == YES )
	if(ReadVBATLowFlag != 0xAAAA)//ͣ�糭���ص�ѹ�Ͳ�������оƬ
	{
		PowerErrCheck( Type );
	}
    #endif
    
    //ȫʧѹ���
    #if( SEL_EVENT_LOST_ALL_V == YES )
	if(ReadVBATLowFlag != 0xAAAA)//ͣ�糭���ص�ѹ�Ͳ�������оƬ
	{
	 	AllLostVCheck( Type );
	}
	#endif
	
	CheckLowPowerCoverCheckTime();
    //���͹���ʱ��
	CheckLowPowerCLK();
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
    POWER_FLASH_CLOSE;
    POWER_HALL_CLOSE;
    POWER_HONGWAI_REC_CLOSE;
    POWER_HONGWAI_TXD_CLOSE;
    POWER_CV485_CLOSE;
    

    PULSE_LED_OFF;
    WARN_ALARM_LED_OFF;
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
void HTMCU_GoToLowSpeed(void)
{
    EnWr_WPREG();
    HT_CMU->SYSCLKCFG=0x0081;//ѡ��FlfΪϵͳʱ��Fsys,��ʱFlfΪFosc�������ⲿ��ƵOSC����ʱ��(32.768KHz)
    HT_CMU->CLKCTRL1=0;
	#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
	HT_CMU->CLKCTRL1 |= 0x000020; 
	#endif
    HT_CMU->PREFETCH = 0x0000;//�͹����½�Ԥȡָ����Ϊ�� ����Ӱ����⻽��
    HT_CMU->CLKCTRL0=0x002c40;//�������Ⱥ�˳�򣬲��ܸ㷴
    HT_CMU->SYSCLKDIV=0x0;
    DisWr_WPREG();
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
		CheckLowPowerCoverCheckTime();//���ʱ�ӵ�ع����Ǽ���Դʱ��
		LowPowerKeyConfig( ePowerWakeUpMode );//�͹��Ļ��Ѻ󰴼�����
		
		memset( Serial[eIR].ProBuf, 0x00, MAX_PRO_BUF );
		
		CloseAllPowerExceptESAM();
		POWER_FLASH_OPEN;
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
	if(Type == 0x00)
	{
		api_MCU_SysClockInit( ePowerOnMode );//�����Ҫ��͹��Ļ��ѵ�����£�����Ҫ��ͨѶ
		//PC2	PLCRX		���ܶ˿� ��һ���� ���� ����
		//PC3	PLCTX		���ܶ˿� ��һ���� ��� ����
		HT_GPIOC->IOCFG |= ((1<<2)|(1<<3));
		HT_GPIOC->AFCFG &= ~((1<<2)|(1<<3));
		
		HT_GPIOC->PTDIR &= ~(1<<2);
		HT_GPIOC->PTDIR |= (1<<3);
		
		HT_GPIOC->PTOD |= (1<<3);
		HT_GPIOC->PTUP |= (1<<2);
		ENABLE_CPU_INT;
		
		SerialMap[eCR].SCIInit(SerialMap[eCR].SCI_Ph_Num);//�ز��˿ڳ�ʼ��
		POWER_REPORT_CR_OPEN;
	}
	else
	{
		DISABLE_CPU_INT;
		POWER_REPORT_CR_CLOSE;
		//PC2	PLCRX		����Ϊ�͹�������
		//PC3	PLCTX		����Ϊ�͹�������
		HT_GPIOC->IOCFG &= ~((1<<2)|(1<<3));
		HT_GPIOC->AFCFG &= ~((1<<2)|(1<<3));
		
		HT_GPIOC->PTDIR |= ((1<<2)|(1<<3));
		HT_GPIOC->PTSET |= ((1<<2)|(1<<3));
		HT_GPIOC->PTOD &= ~((1<<2)|(1<<3));
		HT_GPIOC->PTUP |= ((1<<2)|(1<<3));
		SerialMap[eCR].SCIDisableRcv(SerialMap[eCR].SCI_Ph_Num);//�ز��رս����ж�
		NVIC_ClearPendingIRQ( (IRQn_Type)(9+SerialMap[eCR].SCI_Ph_Num) );//ȫ�����ж�
        NVIC_DisableIRQ( (IRQn_Type)(9+SerialMap[eCR].SCI_Ph_Num) );//ȫ�����ж�
		ENABLE_CPU_INT;
		HTMCU_GoToLowSpeed();//�л�����Ƶ32.768ʱ��
		ENABLE_CPU_INT;
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
extern void SwapAHour(void);
void api_EnterLowPower(eENTER_DOWN_MODE Type)
{
	BYTE	PowerDownCompleteFlag = 0;
	
	DISABLE_CPU_INT;
    CLEAR_WATCH_DOG;
	POWER_COVER_OPEN;
	//���ⰴ�����Ѻ�����
	if( Type == eFromOnRunEnterDownMode )
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
    api_PowerDownLcd();//����
    if((Type == eFromOnRunEnterDownMode) || (Type == eFromOnInitEnterDownMode))
 	{
    	ReadVBATLowFlag = 0x5555;
    }
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
        
        api_PowerDownFreeze();
        api_PowerDownEvent();           // �����¼�����

		#if( SEL_AHOUR_FUNC == YES )
		SwapAHour();
		#endif 
		if( api_GetRunHardFlag( eRUN_HARD_READ_BAT_LOW ) == TRUE )	//BIT3	ͣ�糭����( 0 ����, 1Ƿѹ)		
		{
			if( SelThreeBoard == BOARD_HT_THREE_0132515 )
			{
				ReadVBATLowFlag = 0xAAAA;
			}
		}
 	}
 	//�Ƿ�Ϊ�ϵ�����͹���
 	if((Type == eFromOnRunEnterDownMode) || (Type == eFromOnInitEnterDownMode))
 	{
 		api_LowPowerCheckMeterCoverStatus();
 		api_LowPowerCheckButtonCoverStatus();
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
    
    LowPowerKeyConfig( ePowerDownMode );//�͹����°�������
    api_MCU_RTCInt( );//����RTC���жϣ��жϺ�ỽ�ѵ�Ƭ����hold״̬
	ENABLE_CPU_INT;
	
	FunctionConst(LOWPOWER_TASK);

	while(1)//�͹���ѭ��
	{
		
		Hold_Sleep();//����holdģʽ
		//����ȴ����жϻ��Ѻ�����ִ��
		CLEAR_WATCH_DOG;//�忴�Ź�
		ProcLowPowerMeterReport();
		if( api_CheckSysVol( eSleepDetectPowerMode ) == TRUE )
		{
			Reset_CPU();
		}

		LowPowerWakeup();

		LowPowerCheck( Type );
		
		#if(SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_UART_COMM)
		ProcLowPowerIrComm();
		#endif
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




#endif//#if( (CPU_TYPE == CPU_HT6025) && (MAX_PHASE == 3) ) //��ʱ��cpu���壬���Կ����õ����ඨ��

