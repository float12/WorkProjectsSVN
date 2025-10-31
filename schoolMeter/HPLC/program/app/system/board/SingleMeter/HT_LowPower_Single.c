//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.26
//����		��Ȫcpu�����͹��Ľӿ��ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT_LowPower_Single.h"

#if( (CPU_TYPE == CPU_HT6025) && (MAX_PHASE == 1) ) //��ʱ��cpu���壬���Կ����õ����ඨ��
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

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
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
	#if ( (BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303)\
		|| (BOARD_TYPE == BOARD_HT_SINGLE_78202401) )
	if( Type == ePowerDownMode )
	{
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE = 0;
		HT_INT->EXTIE |= 0x0040;		//ʹ���½����ж�
		HT_INT->PINFLT |= 0x0040;		//ʹ�������˲�
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI6_IRQn );
		
		//ʹ�ܿ��ǻ���
		#if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
		HT_GPIOA->IOCFG |= GPIO_Pin_10;
		HT_GPIOA->AFCFG &= ~(GPIO_Pin_10);
		HT_GPIOA->PTDIR &= ~GPIO_Pin_10;
		HT_GPIOA->PTUP	|= GPIO_Pin_10;
		NVIC_DisableIRQ( EXTI5_IRQn );
		HT_INT->EXTIE |= 0x2020; //ʹ��˫���ش���
		HT_INT->PINFLT |= 0x0020; //�����˲�
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI5_IRQn );
		#else
		HT_GPIOA->IOCFG |= GPIO_Pin_6;
		HT_GPIOA->AFCFG &= ~(GPIO_Pin_6);
		HT_GPIOA->PTDIR &= ~GPIO_Pin_6;
		HT_GPIOA->PTUP	|= GPIO_Pin_6;
		NVIC_DisableIRQ( EXTI1_IRQn );
		HT_INT->EXTIE |= 0x0202; //ʹ���������ж�
		HT_INT->PINFLT |= 0x0002; //�����˲�
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI1_IRQn );
		#endif
	}
	else
	{
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE &= ~0x0040;		//�ر��½����ж�
		HT_INT->PINFLT &= ~0x0040;		//�ر������˲�
		HT_INT->EXTIF = 0;
		
		#if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
		NVIC_DisableIRQ( EXTI5_IRQn );
		HT_INT->EXTIE &= ~0x2020; //��ֹ�ж�
		HT_INT->PINFLT &= ~0x0020; //�����˲�
		HT_INT->EXTIF = 0;
		#else
		NVIC_DisableIRQ( EXTI1_IRQn );
		HT_INT->EXTIE &= ~0x0202; //ʹ���������ж�
		HT_INT->PINFLT &= ~0x0002; //�����˲�
		HT_INT->EXTIF = 0;
		#endif
	}
	#else
	if( Type == ePowerDownMode )
	{
		NVIC_DisableIRQ( EXTI5_IRQn );
		HT_INT->EXTIE = 0;
		HT_INT->EXTIE |= 0x0020;		//ʹ���½����ж�
		HT_INT->PINFLT |= 0x0020;		//ʹ�������˲�
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI5_IRQn );

		#if (BOARD_TYPE == BOARD_HT_SINGLE_78202202)
		HT_GPIOA->IOCFG |= GPIO_Pin_11;
		HT_GPIOA->AFCFG &= ~(GPIO_Pin_11);
		HT_GPIOA->PTDIR &= ~GPIO_Pin_11;
		HT_GPIOA->PTUP	|= GPIO_Pin_11;
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE |= 0x4040; //ʹ��˫���ش���
		HT_INT->PINFLT |= 0x0040; //�����˲�
		HT_INT->EXTIF = 0;
		NVIC_EnableIRQ( EXTI6_IRQn );
		#endif
	}
	else
	{
		NVIC_DisableIRQ( EXTI5_IRQn );
		HT_INT->EXTIE &= ~0x0020;		//�ر��½����ж�
		HT_INT->PINFLT &= ~0x0020;		//�ر������˲�
		HT_INT->EXTIF = 0;

		#if (BOARD_TYPE == BOARD_HT_SINGLE_78202202)
		NVIC_DisableIRQ( EXTI6_IRQn );
		HT_INT->EXTIE &= 0x4040; //ʹ��˫���ش���
		HT_INT->PINFLT &= 0x0040; //�����˲�
		HT_INT->EXTIF = 0;
		#endif
	}
	#endif//#if(BOARD_TYPE==BOARD_HT_SINGLE_78201602)
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
    HT_TBS->TBSCON=0x6541;
    // HT_TBS->TBSPRD=0x0004; //�¶Ȳ�������8��
	HT_TBS->TBSPRD = 0x0007; //�¶Ȳ�������=64s
    
    HT_TMR0->TMRCON=0;
    HT_TMR1->TMRCON=0;
    HT_TMR2->TMRCON=0;
    
    //HT_LCD->LCDCON=0;
    HT_PMU->PMUCON=0;
    HT_INT->EXTIE=0;
    
    //HT_CMU->CLKCTRL1=0;//�ر�UART0-5,TMR0-3,SPI1
    //HT_CMU->CLKCTRL0&=~0x000d;//�ر�SPI0��IIC��
    
    DisWr_WPREG();

    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    NVIC_ClearPendingIRQ(EXTI3_IRQn);
    NVIC_ClearPendingIRQ(EXTI4_IRQn);
    NVIC_ClearPendingIRQ(EXTI5_IRQn);
    NVIC_ClearPendingIRQ(EXTI6_IRQn);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_ClearPendingIRQ(PMU_IRQn);

	//SystickΪ�ں��ж� ����ͨ�������жϷ�ʽ�ر� ֻ�ܹرմ�ģ��
	CLOSE_SYSTICK;
	
    for(i=0;i<31;i++)
    {
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
	//����͹�����ʱ�Ӳ���Flf ǿ���л�һ��ʱ�� ��ֹʱ��û�б��л�����
	if( (HT_CMU->SYSCLKCFG&0x0007) != 0x0001 )
	{
		HTMCU_GoToLowSpeed();
	}
}

//-----------------------------------------------
//��������: �͹���ʱִ�еĿ��ǡ���Դ�쳣�ȼ��
//
//����: 	Type[in]��eENTER_DOWN_MODE
//					eFromOnRunEnterDownMode
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void LowPowerCheck(BYTE Type)
{
    //EnableLowPowerPort();//�����͹��Ļ���������ı�Ҫ�˿ڣ����翪�Ǽ��
    
    #if( SEL_EVENT_METERCOVER == YES )
    ProcLowPowerMeterCoverRecord();//���ϸǼ��
    #endif
    
    #if( SEL_EVENT_BUTTONCOVER == YES )
    ProcLowPowerButtonCoverRecord();//��β�Ǽ��
    #endif
    
    #if( SEL_EVENT_POWER_ERR == YES )
    PowerErrCheck( Type );//��Դ�쳣��� 
    #endif
	
	//���͹���ʱ��
	CheckLowPowerCLK();

    //DisableLowPowerPort();//�رողſ����Ķ˿�
}


//-----------------------------------------------
//��������: �ر�������Χ�����ĵ�Դ-������ESAM��Դ-ESAM��Դ��������
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
}

//-----------------------------------------------
//��������: �ر�������Χ�����ĵ�Դ-������ESAM��Դ��FLASH��Դ
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void CloseAllPowerExceptESAM_FLASH(void)
{
    CLOSE_BACKLIGHT;
    POWER_SAMPLE_CLOSE;
    POWER_LCD_CLOSE;
	
    POWER_HALL_CLOSE;
    POWER_HONGWAI_REC_CLOSE;
    POWER_HONGWAI_TXD_CLOSE;
    POWER_CV485_CLOSE;

    PULSE_LED_OFF;
    WARN_ALARM_LED_OFF;
    RELAY_LED_OFF;
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
		api_MCU_SysClockInit( ePowerDownMode );
		api_InitSysTickTimer( ePowerDownMode );
		InitPort( ePowerWakeUpMode );
		
		LowPowerKeyConfig( ePowerWakeUpMode );//�͹��Ļ��Ѻ󰴼�����

		CloseAllPowerExceptESAM();//�ر�������Χ�����Ĺ���
		#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303))
		api_AdcConfig(ePowerWakeUpMode);
		#endif
        ESAMSPIPowerDown( ePowerDownMode );//�ر�esam��Դ
		POWER_LCD_OPEN;
		
		api_FreshParaRamFromEeprom(0);
		api_FreshParaRamFromEeprom(1);
		api_PowerUpLcd( ePowerDownMode );
		api_CheckTimeTable(FALSE);
		api_LowPowerWakeUpPrePay();
		RESET_STACK_POINTER;
		
		//�����ж�  �ڽ���ѭ��֮ǰ�Ŵ��ж�
		ENABLE_CPU_INT;
		
		MainTask();
	}
}

//-----------------------------------------------
//��������: �����͹���pmu����
//
//����: 	    ��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void LowPowerPmuRtcInt(void)
{
	//PMU
	HT_GPIOE->IOCFG |= 0x0080;		//����
	HT_GPIOE->AFCFG &=~0x0080;		//����1
	EnWr_WPREG();
	NVIC_DisableIRQ(PMU_IRQn);		//��ֹRTC�ж�
	HT_PMU->PMUCON	= 0x0007;
	HT_PMU->VDETCFG = 0x006D;		//�ϵ�Ƚ�VCC��ѹ4.6V BOR 2.0����λ
	HT_PMU->PMUIE	= 0x0004;		//ʹ��PMU�ж� ֻʹ��LVDIN
	HT_PMU->PMUIF	= 0;			//���жϱ�־		
	NVIC_ClearPendingIRQ(PMU_IRQn);	
	NVIC_EnableIRQ(PMU_IRQn);		//ʹ��PMU�ж�
	DisWr_WPREG();		
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
	#if( BOARD_TYPE == BOARD_HT_SINGLE_78202202 )
	if(Type == 0x00)
	{
		api_MCU_SysClockInit( ePowerOnMode );//�����Ҫ��͹��Ļ��ѵ�����£�����Ҫ��ͨѶ
		//PC12	PLCRX		���ܶ˿� ��һ���� ���� ����
		//PC11	PLCTX		���ܶ˿� ��һ���� ��� ��©
		HT_GPIOC->IOCFG |= ((1<<11)|(1<<12));
		HT_GPIOC->AFCFG &= ~((1<<11)|(1<<12));
		
		HT_GPIOC->PTDIR &= ~(1<<12);
		HT_GPIOC->PTDIR |= (1<<11);
		
		HT_GPIOC->PTOD &= ~(1<<11);
		HT_GPIOC->PTUP |= (1<<12);
		ENABLE_CPU_INT;
		
		SerialMap[eCR].SCIInit(SerialMap[eCR].SCI_Ph_Num);//�ز��˿ڳ�ʼ��
		POWER_REPORT_CR_OPEN;
	}
	else
	{
		DISABLE_CPU_INT;
		POWER_REPORT_CR_CLOSE;
		//PC12	PLCRX		����Ϊ�͹�������
		//PC11	PLCTX		����Ϊ�͹�������
		HT_GPIOC->IOCFG &= ~((1<<11)|(1<<12));
		HT_GPIOC->AFCFG &= ~((1<<11)|(1<<12));
		
		HT_GPIOC->PTDIR |= ((1<<11)|(1<<12));
		HT_GPIOC->PTSET |= ((1<<11)|(1<<12));
		HT_GPIOC->PTOD &= ~((1<<11)|(1<<12));
		HT_GPIOC->PTUP |= ((1<<11)|(1<<12));
		SerialMap[eCR].SCIDisableRcv(SerialMap[eCR].SCI_Ph_Num);//�ز��رս����ж�
		NVIC_ClearPendingIRQ( (IRQn_Type)(9+SerialMap[eCR].SCI_Ph_Num) );//ȫ�����ж�
        NVIC_DisableIRQ( (IRQn_Type)(9+SerialMap[eCR].SCI_Ph_Num) );//ȫ�����ж�
		ENABLE_CPU_INT;
		HTMCU_GoToLowSpeed();//�л�����Ƶ32.768ʱ��
		ENABLE_CPU_INT;
	}
	#else
	if(Type == 0x00)
	{
		api_MCU_SysClockInit( ePowerOnMode );//�����Ҫ��͹��Ļ��ѵ�����£�����Ҫ��ͨѶ
		api_MCU_RTCIntSec(1);

		//PC2	PLCRX		���ܶ˿� ��һ���� ���� ����
		//PC3	PLCTX		���ܶ˿� ��һ���� ��� ��©
		HT_GPIOC->IOCFG |= ((1<<2)|(1<<3));
		HT_GPIOC->AFCFG &= ~((1<<2)|(1<<3));
		
		HT_GPIOC->PTDIR &= ~(1<<2);
		HT_GPIOC->PTDIR |= (1<<3);
		
		HT_GPIOC->PTOD &= ~(1<<3);
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
	#endif//#if( BOARD_TYPE == BOARD_HT_SINGLE_78202202 )
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
	static DWORD adc_count=0;

    DISABLE_CPU_INT;
    CLEAR_WATCH_DOG;
	
	//���ⰴ�����Ѻ�����
	if( Type == eFromOnRunEnterDownMode )
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LowPowerConRom.DetectFlag),1,&PowerDownCompleteFlag);
	}
	
    api_PowerDownLcd();//����
	
	#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202303) || (BOARD_TYPE == BOARD_HT_SINGLE_78202401))
	CloseAllPowerExceptESAM_FLASH();//�ر�������Χ�����Ĺ����flash/ESAM
	#else
	CloseAllPowerExceptESAM();//�ر�������Χ�����Ĺ���
	#endif
    ESAMSPIPowerDown( ePowerDownMode );//�ر�esam��Դ

    //ֻ������������ʱ��ת�棬��������⻽�Ѻ����͹����ǲ�����ת��ġ�
 	if( Type == eFromOnRunEnterDownMode )
 	{      
        if( g_PowerOnSecTimer == 0 )
        {
        	api_PowerDownEnergy();     // �������ת�洦��
        	api_PowerDownPrePay();
        }

        api_PowerDownFreeze();
    	api_PowerDownEvent();       // �����¼�����

		 #if( SEL_AHOUR_FUNC == YES )
		SwapAHour();
		#endif
 	}
 	//�Ƿ�Ϊ�ϵ�����͹���
 	if((Type == eFromOnRunEnterDownMode) || (Type == eFromOnInitEnterDownMode))
 	{
		// api_LowPowerCheckMeterCoverStatus();
		api_WriteFreeEvent(EVENT_ENTER_LOW_POWER, Type);//��¼�����¼�
		#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303))
		TimerLcdTmp = 5;
		#endif
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
    ESAMSPIPowerDown( ePowerDownMode );//�ر�esam��Դ
 	CLEAR_WATCH_DOG;
 	IsExitLowPower = 0;
	
	//!!!!!!���Σ�lsc
	// #if (BOARD_TYPE == BOARD_HT_SINGLE_78202202)
	// api_MCU_RTCInt();
	// #else
	// LowPowerPmuRtcInt();
	// api_MCU_RTCIntSec( 2 );
	// #endif
	// LowPowerKeyConfig( ePowerDownMode );//�͹����°�������
	// HTMCU_GoToLowSpeed();//�л�����Ƶ32.768ʱ��
	
    ENABLE_CPU_INT;
    
	FunctionConst(LOWPOWER_TASK);

//    while(1)//�͹���ѭ��
//	{
//		
//		Hold_Sleep();//����holdģʽ
//		//����ȴ����жϻ��Ѻ�����ִ��
//		CLEAR_WATCH_DOG;//�忴�Ź�
//		ProcLowPowerMeterReport();//����Ƿ��ϱ�
//		if( api_CheckSysVol( eSleepDetectPowerMode ) == TRUE )
//		{
//			Reset_CPU();
//		}
//
//		LowPowerWakeup();
//
//		LowPowerCheck(Type);
//	}
	Reset_CPU();
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


#endif//#if( (CPU_TYPE == CPU_HT6025) && (MAX_PHASE == 1) ) //��ʱ��cpu���壬���Կ����õ����ඨ��

