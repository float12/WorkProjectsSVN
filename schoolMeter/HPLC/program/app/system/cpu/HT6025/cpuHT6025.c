//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.30
//����		CPU�������� //����ļ�Ҫֻ�������Ĳ��֣�������Ҫ�ú��ж�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

#if ( CPU_TYPE == CPU_HT6025 )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define C_SysTickLoad   (((22020096>>SPD_MCU) / (1000/SYS_TICK_PERIOD_MS)) - 1)

//6015��HRC��14M(6025����11.01M) ���Ե͹��Ļ��ѵ�ϵͳʱ��Ϊ7M ����5.5M SysTick���ٶȻ�ӿ�
#define C_SingleLowPowerSysTickLoad   (((22020096>>SINGLE_LOW_POWER_SPD_MCU) / (1000/SYS_TICK_PERIOD_MS)) - 1)
#define HT_HFCFG (*((volatile unsigned int*)(0x4000C0F0)))

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//Flash�������Ƿ���
// #if( SEL_FLASH_PROTECT == FLASH_PROTECT_OPEN )
// __root const DWORD FlashProtect@".password"=0xF7FF00A2;
// #else
// __root const DWORD FlashProtect@".password"=0xF7FFFFA2;mIN
// #warning: "����:����ʱ�ɹرն�����,����ʱҪ����������";
// #endif
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
DWORD	ReceiveProgStatus;
WORD 	ProgFlashPW;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

static void InitWatchDog(void);

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ������ʱ����
//
//����: 	ms[in]		��Ҫ��ʱ�ĺ�����
//                    
//����ֵ:  	��
//
//��ע:    K��:
//			22M��Ƶʵ����ʱ1.007ms 11M��Ƶʵ����ʱ1.018ms
//-----------------------------------------------
void api_Delayms(WORD ms)
{
	WORD i, j;
	for(i=0; i<ms; i++)
	{
		#if(SPD_MCU == SPD_22000K)
		for(j=0; j<(3160/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#elif(SPD_MCU == SPD_11000K) 
		for(j=0; j<(3190/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#endif	//#if(SPD_MCU == SPD_22000K)
	}
}


//-----------------------------------------------
//��������: 100΢����ʱ���� - �е��Ż�
//
//����: 	us[in]		��Ҫ��ʱ��100΢����
//                    
//����ֵ:  	��
//
//��ע:    K��:
//			22M��Ƶʱ ʵ��Լ��ʱ104.25us-1�� 102.45-10��   ѭ��ֵ����256��С��256��ʱֵ���ܴ󣬴ӻ�࿴Ҳ��һ��
//			11M��Ƶʱ ʵ��Լ��ʱ102.65us-1�� 100.45-10��
//-----------------------------------------------
void api_Delay100us(WORD us)
{
	WORD i, j;
	
	for(i=0; i<us; i++)
	{
		#if(SPD_MCU == SPD_22000K)
		for(j=0; j<(320/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#elif(SPD_MCU == SPD_11000K)
		for(j=0; j<(312/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#endif	//#if(SPD_MCU == SPD_22000K)
	}
}


//-----------------------------------------------
//��������: 10΢����ʱ���� - �е��Ż�
//
//����: 	us[in]		��Ҫ��ʱ��10΢����
//                    
//����ֵ:  	��
//
//��ע:     K��:
//			22M��Ƶ��ʵ��Լ12.3us 
//			����ʱʱ���й� 5-10.99 10-10.84 50-10.7 100-10.69 2-11.5 4-11.08 6-10.94
//			11M��Ƶ��ʵ��Լ12.4us
//			����ʱʱ���й� 5-10.4 10-10.15 50-9.95 100-9.93 2-11.15 4-10.5 6-10.3
//-----------------------------------------------
void api_Delay10us(WORD us)
{
	WORD i, j;
	
	for(i=0; i<us; i++)
	{
		#if(SPD_MCU == SPD_22000K)
		for(j=0; j<(32/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#elif(SPD_MCU == SPD_11000K)
		for(j=0; j<(28/(SPD_MCU+1)); j++)
		{
			asm("nop");
		}
		#endif	//#if(SPD_MCU == SPD_22000K)
	}
}


//-----------------------------------------------
//��������: ͨ��NOP������ʱ����
//
//����: 	Step[in]		��Ҫ��ʱ��NOP����
//                    
//����ֵ:  	��
//
//��ע:   !!!!!!�ú�������һ������ָ����÷�ʽ������Ҫ����һ���ط����ú��������÷�ʽ !!!!!!
//-----------------------------------------------
#pragma optimize=size low//�м��Ż�������£����a_byDefaultData�Ż���
void api_DelayNop(BYTE Step)
{
	BYTE i;
	
	i = a_byDefaultData[0];
	
	for(i=0; i<Step; i++)
	{
   		__NOP();
   	}
}

//---------------------------------------------------------------
//��������: ����HRC����ϵ��--�����ϵ���ʱ
//
//����: ��
//                   
//����ֵ:  ��
//
//��ע:   
//---------------------------------------------------------------
static void LoadHRCInfo( void )
{
	BYTE adj, u80, u81;
	DWORD u320,u321; 
	
	EnWr_WPREG();
	//HRC��Ҫ��infoflashȡ������ֵ �ɱ�֤��11m���� ������1.2% ����������ȶ�
	u320 = *(DWORD *)0x40140;
	u321 = *(DWORD *)0x40144;
	u321 = ~u321;
	u80  = *(BYTE *)0x40140;
	u81  = *(BYTE *)0x40141;
	u81  = ~u81;

	if(u320 == u321)       //11M
	{
		adj = u320;
	}
	else if(u80 == u81)    //14M �����6015�ĵ�УֵΪ14M
	{
		adj = u80;
	}
	else                    //8M ������ݴ��� 8M������׼ȷ ��˹̶�Ϊ8M
	{
		adj = 0x3D;
	}
	
	HT_CMU->CLKCTRL0 |= 0x000020;//ʹ��HRC
	HT_CMU->PREFETCH = 0x0000;	 //��Ԥȡָ����Ϊ�� 
	HT_CMU->HRCADJ = adj;        //���11MHz

	DisWr_WPREG();	
}
//---------------------------------------------------------------
//��������: ��OSC������ʱ
//
//����: Val
//                   
//����ֵ: �� 
//
//��ע: 
//	�临λ������HRC11M����������Ƶ����Ƶ�� 11M/4 ���ҡ�Ĭ�ϼĴ�������
//�����������Σ�
//  10   10.010ms��	20	20.01ms   50  50.000ms  100  100.011ms
//ʵ���������Σ�
//  10   10.009ms��	20	20.008ms   50  50.008ms  100  100.008ms
//---------------------------------------------------------------
void WaitOSCRunDelayms( WORD Val )
{
	WORD i, j;
	
	for(i=0; i<Val; i++)
	{
		for(j=0; j<395; j++)
		{
			asm("nop");
		}
	}	
}

//---------------------------------------------------------------
//��������: �临λ��OSC����
//
//����: ��
//
//����ֵ:  ��
//
//��ע:
//---------------------------------------------------------------
static void ResetWaitOSCRun( void )
{
	WORD i;
	//�临λ �ϵ���ʱ��POR(�ϵ縴λ)����LBOR(�͵�ѹ��⸴λ)��λ
	if((HT_PMU->RSTSTA&0x0003)||
	   (NoInitDataFlag.CRC32 != lib_CheckCRC32( (BYTE *)&NoInitDataFlag, sizeof(NoInitDataFlag) - sizeof(DWORD) )))
	{
		LoadHRCInfo();//����HRC
		//�ϵ�Ĭ��ʱ��ΪHRC11M��HRC_DIV 2��Ƶ��SYSCLK_DIV 2��Ƶ��FCLK = 2.5M
		//������1.96��	�Ƿ���1.97��
		for(i = 0; i < WaitOSCRunTime; i++)
		{
			CLEAR_WATCH_DOG;
			//eSleepDetectPowerMode �����ڲ�û����ʱ������ֹ���У���һ�β����͹���
			//ѭ��30���ٽ��͹��ģ����Դ�ϵ��ٶ����������ֱ�ӽ���͹����������Ź���λ�������ϵ��ٶ���������
			if((api_CheckSysVol( eSleepDetectPowerMode ) == FALSE) && ( i >= 40 ))
			{
				api_EnterLowPower( eFromOnInitEnterDownMode );
			}
			WaitOSCRunDelayms( 20 );
		}
	}
}

//-----------------------------------------------
//��������: CPUϵͳʱ�ӳ�ʼ��������ģ���ں���
//
//����: 	Type[in]		ePowerOnMode:�����ϵ�  ePowerDownMode:�͹���
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void api_MCU_SysClockInit( ePOWER_MODE Type )
{
	BYTE adj, u80, u81;
	DWORD u320,u321; 
	
	
    EnWr_WPREG();
	
	//CLKCTRL0��CLKCTRL1��16λ����24λ
    HT_CMU->CLKCTRL0 &= ~0x000380;//�ر����е�ʱ��ͣ����
    if( Type == ePowerOnMode )
    {
	    HT_CMU->CLKCTRL0 |= 0x000010;//ʹ��PLL
		#if( SPD_MCU == SPD_22000K )
	    HT_CMU->PREFETCH = 0x0001;	//��Ȫ˵22MhzʱҲ�ӣ����˲�����
		#endif
		HT_CMU->SYSCLKDIV = SPD_MCU;//����Fcpu=Fsys/2
	    //while (HT_CMU->CLKSTA & 0x0010);//�ȴ�Fpll����    
	    if( HT_CMU->SYSCLKCFG & 0x0002 )//��ǰʱ��Ϊfpll��fhrc
	    {
	    	api_Delayms( 3 );
	    }
	    else
	    {
	    	api_DelayNop( 99 );
	    }
	    
	    HT_CMU->SYSCLKCFG = 0x0083;//����FsysΪFpll������ϵͳʱ��ʱ���轫bit7��1��
	    api_DelayNop(5);
		HT_CMU->SYSCLKCFG = 0x0003;//�ٴ�����FsysΪFpll����һ������û���ã�
	}
	else
	{
		//HRC��Ҫ��infoflashȡ������ֵ �ɱ�֤��11m���� ������1.2% ����������ȶ�
		u320 = *(DWORD *)0x40140;
		u321 = *(DWORD *)0x40144;
		u321 = ~u321;
		u80  = *(BYTE *)0x40140;
		u81  = *(BYTE *)0x40141;
		u81  = ~u81;

		if(u320 == u321)       //11M
		{
			adj = u320;
		}
		else if(u80 == u81)    //14M �����6015�ĵ�УֵΪ14M
		{
			adj = u80;
		}
		else                    //8M ������ݴ��� 8M������׼ȷ ��˹̶�Ϊ8M
		{
			adj = 0x3D;
		}
		
		HT_CMU->CLKCTRL0 |= 0x000020;//ʹ��HRC
	 	HT_CMU->PREFETCH = 0x0000;//�͹����½�Ԥȡָ����Ϊ�� ����Ӱ����⻽��
		HT_CMU->HRCADJ = adj;                //���11MHz
		HT_CMU->HRCDIV = 0x0001;//����Ϊ2��Ƶ��ȱʡ�������
		HT_CMU->SYSCLKDIV = 0;//����Fcpu=Fsys/2  ����Ϊ����Ƶ
	    //while (HT_CMU->CLKSTA & 0x0010);//�ȴ�Fpll����    
	    if( HT_CMU->SYSCLKCFG & 0x0002 )//��ǰʱ��Ϊfpll��fhrc
	    {
	    	api_Delayms( 3 );
	    }
	    else
	    {
	    	api_DelayNop( 99 );
	    }
	    
	    HT_CMU->SYSCLKCFG = 0x0082;//����FsysΪHRC������ϵͳʱ��ʱ���轫bit7��1��
	    api_DelayNop(5);
		HT_CMU->SYSCLKCFG = 0x0002;//�ٴ�����FsysΪHRC����һ������û���ã�
	}
	
	//HT_PMU->PMUCON=0x0;//�رո��ֵ����ж�2016.6.27
    HT_PMU->PMUCON=0x0004;//����LVDIN0�͵�ѹ���
    HT_PMU->PMUIF=0;
    
    DisWr_WPREG();
}

//-----------------------------------------------
//��������:     systick��ʱ����ʼ������
//
//����: 	 	Type[in]		ePowerOnMode:�����ϵ�   ePowerDownMode:�͹���
//                    
//����ֵ:  	��
//
//��ע:       ePowerDownModeģʽ �����ڵ���
//-----------------------------------------------
void api_InitSysTickTimer( ePOWER_MODE Type)
{
    SysTick->CTRL = 0x00000000;    
    
    if( Type == ePowerOnMode )
    {
        SysTick->LOAD = C_SysTickLoad;
    }
    else
    {
        SysTick->LOAD = C_SingleLowPowerSysTickLoad;
    }
    
    SysTick->VAL  = 0x00000000;
    NVIC_SetPriority(SysTick_IRQn, 3);
    SysTick->CTRL = 0x00000007;
}

void InitTimer4( void )
{
	EnWr_WPREG();
	
	HT_CMU->CLKCTRL1 |= CMU_CLKCTRL1_TMR4EN;
	HT_TMR4->TMRDIV = 0;
	HT_TMR4->TMRPRD = 0xFFFF;
	HT_TMR4->TMRIE = 0;
	HT_TMR4->TMRCON = 0x0107;//ѡ��flfʱ�ӣ�32768�������ڶ�ʱģʽ
	
	DisWr_WPREG();
}

#if( PREPAY_MODE == PREPAY_LOCAL )
//-----------------------------------------------
//��������: ��ʱ��0����
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void InitTimer0( void )
{
	EnWr_WPREG();
	
	HT_CMU->CLKCTRL1 |= CMU_CLKCTRL1_TMR0EN;
	HT_TMR0->TMRDIV = 0;
	HT_TMR0->TMRCMP = 0x0ABE;//ռ�ձ�Ϊ50%
	HT_TMR0->TMRPRD = 0x157C;//Ƶ��Ϊ4K
	HT_TMR0->TMRIE = 0;
	HT_TMR0->TMRCON = 0x030B;//ѡ��Fpllʱ�ӣ�22M����pwmģʽ,���ϼ���
	
	DisWr_WPREG();
}
#endif

//-----------------------------------------------
//��������: ��ȡ�����ʲ���
//
//����: 	Type[in]		0:��ʼ��ʱ�ò����� 1��ƽʱ��������
//          SciPhNum[in]	�����ں�
//
//����ֵ:  	�����ʲ���ֵ
//
//��ע:   
//-----------------------------------------------
BYTE api_GetBaudRate(BYTE Type, BYTE SciPhNum)
{
    BYTE i;
    BYTE BpsBak, Status;
    Status = FALSE;

    if(Type == 0)
    {
        //�ж�FPara2��У��
        if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == TRUE )
        {
            Status = TRUE;
        }
    }
    else
    {
        Status = TRUE;
    }

	for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
    	if(SerialMap[i].SCI_Ph_Num == SciPhNum)
        {
        	if(i == eRS485_I)//485
            {
            	if( Status == TRUE )
                {
                 	BpsBak = FPara2->CommPara.I485;
                }
                else
                {
                  	BpsBak = CommParaConst.I485;//115200 1ֹͣλ żУ��
                }
            }
            else if(i == ePT_UART1_F415)//415
            {
				BpsBak = 0x4A;//6025��415֮�䲨����115200
            }
            else if(i == eCR)  //�ز�
            {
            	if( Status == TRUE )
                {
                  	BpsBak = FPara2->CommPara.ComModule;
                }
                else
                {
                  	BpsBak = CommParaConst.ComModule;//9600bps
                }
           	}
            else if(i == eBlueTooth)	//����!!!!!!ͬ���������һ��
            {
            	if( Status == TRUE )
                {
                   	BpsBak = FPara2->CommPara.II485;
                }
                else
                {
                  	BpsBak = CommParaConst.II485;//2400bps
                }
            }

            break;
        }
    }

    return BpsBak;
}


//-----------------------------------------------
//��������: ������Ĳ����ʺ�Ĵ��ڳ�ʼ��
//
//����: 	Channel[in]		�߼����ں� 0����һ485  1������  2��ģ��
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void DealSciFlag( BYTE Channel )
{
    //�иĲ��ز��������Ӧ�����ٸĲ�����
    if( Serial[Channel].OperationFlag & BIT0 )
    {
    	Serial[Channel].OperationFlag &= ~BIT0;
    	SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
    }
}


//-----------------------------------------------
//��������: ���ڳ�ʼ��
//
//����: 
//			Channel[in]		�߼����ں� 0����һ485  1������  2��ģ��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_InitSci( BYTE Channel )
{
	SerialMap[Channel].SCIInit(SerialMap[Channel].SCI_Ph_Num);
}


//-----------------------------------------------
//��������: ��ָֹ�����ڽ���
//
//����: 
//			p[in]		��Ҫ��ֹ���ڵ�ָ��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_DisableRece(TSerial * p)
{
	BYTE i;
	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( p == &Serial[i] )
		{
			SerialMap[i].SCIDisableRcv(SerialMap[i].SCI_Ph_Num);
		}
	}
}


//-----------------------------------------------
//��������: ����ָ���ӿڽ���
//
//����: 
//			p[in]		��Ҫ�����ڵ�ָ��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_EnableRece(TSerial * p)
{
	BYTE i;
	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( p == &Serial[i] )
		{
			SerialMap[i].SCIEnableRcv(SerialMap[i].SCI_Ph_Num);
		}
	}
}


//-----------------------------------------------
//��������: �㲥������Ӧ�������´��ڻָ�����
//
//����: 
//			Channel[in]		�߼����ں� 0����һ485  1������  2��ģ��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ProSciTimer( BYTE Channel )
{
	BYTE IntStatus;

    if(Serial[Channel].BroadCastFlag==0xff)
	{
		DealSciFlag( Channel );
		SerialMap[Channel].SCIEnableRcv(SerialMap[Channel].SCI_Ph_Num);

		IntStatus = api_splx(0);
		api_InitSciStatus( Channel );
		api_splx(IntStatus);
	}
}


//-----------------------------------------------
//��������: CPU��ʼ��
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_InitCPU( void )
{
	api_Delay100us(10);
	FunctionConst(INIT_BASE_TIMER);
	//�临λ�ȴ� OSC����
	ResetWaitOSCRun();
	//��ʼ��ϵͳʱ��
	api_MCU_SysClockInit( ePowerOnMode );
	//��ʼ������
	InitWatchDog();
	CLEAR_WATCH_DOG;
	//ϵͳ��ʱ����ʼ����Tick��ʱ����
	api_InitSysTickTimer( ePowerOnMode );
	//��ʼ����ʱ��4�����ڲ�����ʱ�䣬ʱ��Ϊ32768
	InitTimer4();
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//��ʼ����ʱ��0������pwm�����ķ�����
	InitTimer0();
	#endif
}


//-----------------------------------------------
//��������: ���Ź���ʼ����HT6025�Ŀ��Ź�Ĭ���ǿ�����
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void InitWatchDog(void)
{
    //HT_WDT->WDTCFG  = 0x00;//����Ϊ���Ź������λ
    HT_WDT->WDTSET = 0xAAfF;//FeedWDT per 4s
}


//-----------------------------------------------
//��������: RTC���ж�ʹ��
//
//����: 	Type[in]		0:�� �� ʱ�жϿ���  1:���жϿ���
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_MCU_RTCInt( void )
{
    EnWr_WPREG();
    
    HT_RTC->RTCCON = 0x0000;

    HT_RTC->RTCIE = 0x0001;//RTC �� �ж�ʹ��λ

    NVIC_EnableIRQ(RTC_IRQn);
    
    DisWr_WPREG();
}

//-----------------------------------------------
//��������: RTC���ж�ʹ��
//
//����: 	Type[in]:�ж�����sec��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_MCU_RTCIntSec( BYTE Sec )
{
	if( Sec == 0 )
	{
		Sec = 1;
	}
	
    EnWr_WPREG();
	NVIC_DisableIRQ(RTC_IRQn);		//��ֹRTC�ж�
	HT_RTC->RTCCON &= (~0x0020);    //��ֹRTC��ʱ��1
	HT_RTC->RTCTMR1= Sec-1;			//RTC1��ʱ����SEC��
	HT_RTC->RTCCON = 0x0020;        //ʹ��RTC��ʱ��1
	HT_RTC->RTCIE = 0x0020;			//ʹ��RTC��ʱ��1�ж�
	HT_RTC->RTCIF  = 0x0000;		//���жϱ�־
	NVIC_EnableIRQ(RTC_IRQn);		//ʹ��RTC�ж�
	DisWr_WPREG();	
}

//-----------------------------------------------
//��������: �رպͻָ��ж�
//
//����: 
//			Data[in]		0:�ر��ж�  �������ر�ǰ��¼�����ݣ����ָ��ж�
//                    
//����ֵ:  	�ر�ǰ���ж�״̬
//
//��ע:   
//-----------------------------------------------
BYTE api_splx(BYTE Data)
{
    BYTE SRValue;
    SRValue = __get_PRIMASK();

    if((Data & 0x01) == 0x00)
    {
        DISABLE_CPU_INT;
    }
    else
    {
        ENABLE_CPU_INT;
    }

    return (~SRValue) & 0x01;
}

#if( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//��������: ��λ��Ƭ
//
//����: 	��
//                   
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ResetCard( void )
{
	EnWr_WPREG();
	#if( MAX_PHASE == 1 )
	//����CLKOUT
	if (!(HT_CMU->CLKCTRL0 & 0x001000)
	|| (HT_CMU->CLKOUTSEL != 0x0003)
	|| (HT_CMU->CLKOUTDIV != 0x0002))
	{
		HT_CMU->CLKCTRL0 |= 0x001000;				//ʹ��CLKOUT
		HT_CMU->CLKOUTSEL = 0x0003;					//Fsys
		HT_CMU->CLKOUTDIV = 0x0002;					//Fsys/(2*(2+1))=3.670016MHz
	}
	OPEN_CARD_IO;									//���ÿ�IO����--���ܶ˿� �������
	OPEN_CARD_CLK;									//���ÿ�CLK����--���ܶ˿� �������
	#else

	//�����ʱ���ظ���λESAM
	if( ESAM_POWER_IS_CLOSE )//esam��Դδ��
	{
		api_ResetEsamSpi(); 
	}

	POWER_CARD_OPEN;	//�Ӵ򿪵�Դ�����������ȶ� ��Լ��Ҫ600usʱ��
	#endif//#if( MAX_PHASE == 3 )
	
	RESET_CARD;
	DisWr_WPREG();
}

//---------------------------------------------------------------
//��������: ���������Ƿ��ͽ���
//
//����:   
//		SciPhNum -- ���ں�	
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL api_UartIsSendEnd( BYTE SciPhNum )
{
	WORD Count;
	HT_UART_TypeDef *USARTx;
	
	Count = 1500; //��1200�������� ���������ֽڵ�ʱ�� ��Լ15000us��
	
	switch( SciPhNum )
	{
		case 0:
			USARTx = HT_UART0;
			break;
		case 1:
			USARTx = HT_UART1;
			break;
		case 2:
			USARTx = HT_UART2;
			break;
		case 3:
			USARTx = HT_UART3;
			break;
		case 4:
			USARTx = HT_UART4;
			break;
		default:
			return FALSE;
	}
	
	while( (!(USARTx->UARTSTA & UART_UARTSTA_TXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	
	if( Count == 0 )
	{
		return FALSE;
	}
	
	return TRUE;
}
//-----------------------------------------------
//��������: ���������չ���
//
//����: 	��
//
//����ֵ:   ��
//
//��ע:
//-----------------------------------------------
void api_OpenRxCard(void)
{
	WORD Count;
	Count = 200;
	
	EnWr_WPREG();
	HT_CMU->CLKCTRL1 |= 0x000100;               //��������ʱ��
	DisWr_WPREG();
	
	#if( MAX_PHASE == 1 )
	NVIC_DisableIRQ(UART4_IRQn);				//��ֹ�����ж�
	HT_UART4->MODESEL	 = 0x0001;				//7816����
	HT_UART4->ISO7816STA = 0x0000;				//����жϱ�־λ
	HT_UART4->ISO7816CON = 0x01C0;				//żУ��,�����ط�,�ظ�2��,��Ӧ1λ,��ֹ�ж�
	HT_UART4->SREL		 = 0x045B;				//9866bps
	OPEN_CARD_IO;								//���ÿ�IO����--���ܶ˿� �������
	OPEN_CARD_CLK;								//���ÿ�CLK����--���ܶ˿� �������
	#else
	NVIC_DisableIRQ(UART4_IRQn);				//��ֹ�����ж�
	HT_UART4->MODESEL  = 0x0000;				//UART���ܣ���7016���ܣ�
	HT_UART4->UARTSTA  = 0x0000;				//����жϱ�־λ	
	HT_UART4->UARTCON  = 0x0150;				//żУ�� 2λֹͣλ
	HT_UART4->SREL     = 0x03E7;				//�Ǳ�׼������ ������Ϊ11010
	HT_UART4->UARTCON &= (~0x0001);             //���ͽ�ֹ
	HT_UART4->UARTCON |= 0x0002;				//����ʹ��	
	#endif//#if( MAX_PHASE == 1 )
}

//-----------------------------------------------
//��������: ���������͹���
//
//����: 	head[in] - �������ֽ�
//                   
//����ֵ:   ��
//
//��ע:   
//-----------------------------------------------
void api_OpenTxCard( BYTE head )
{
	EnWr_WPREG();
	HT_CMU->CLKCTRL1 |= 0x000100;               //��������ʱ��
	DisWr_WPREG();

	#if( MAX_PHASE == 1 )
	NVIC_DisableIRQ(UART4_IRQn);				//��ֹ�����ж�
	HT_UART4->MODESEL	 = 0x0001;				//7816����
	HT_UART4->ISO7816STA = 0x0000;				//����жϱ�־λ
	HT_UART4->ISO7816CON = 0x01C0;				//żУ��,�����ط�,�ظ�2��,��Ӧ1λ,��ֹ�ж�
	HT_UART4->SREL		 = 0x045B;				//9866bps
	HT_UART4->SBUF		 = head;
	OPEN_CARD_IO;								//���ÿ�IO����--���ܶ˿� �������
	OPEN_CARD_CLK;								//���ÿ�CLK����--���ܶ˿� �������
	#else
	NVIC_DisableIRQ(UART4_IRQn);				//��ֹ�����ж�
	HT_UART4->MODESEL  = 0x0000;				//UART���ܣ���7016���ܣ�
	HT_UART4->UARTSTA  = 0x0000;				//����жϱ�־λ	
	HT_UART4->UARTCON  = 0x0150;				//żУ�� 2λֹͣλ
	HT_UART4->SREL     = 0x03E7;				//�Ǳ�׼������ ������Ϊ11010
	HT_UART4->UARTCON |= 0x0001; 				//����ʹ��
	HT_UART4->UARTCON &= (~0x0002);				//���ܽ�ֹ
	HT_UART4->SBUF = head;
	#endif//#if( MAX_PHASE == 1 )
}

//-----------------------------------------------
//��������: �رտ��ӿ�
//
//����: 	head[in] - �������ֽ�
//                   
//����ֵ:   ��
//
//��ע:   
//-----------------------------------------------
void api_CloseCard(void)
{
	NVIC_DisableIRQ(UART4_IRQn);				//��ֹ�����ж�
	#if( MAX_PHASE == 1 )
	HT_UART4->ISO7816CON = 0x0000;				//��ֹ���ڹ���

	EnWr_WPREG();
	HT_CMU->CLKCTRL0 &= (~0x001000);				//ֹͣʱ��
	DisWr_WPREG();
	
	CLOSE_CARD_IO;								//���ÿ�IO����--��ͨ�˿� ��©���
	CLOSE_CARD_CLK;								//���ÿ�CLK����--��ͨ�˿� ��©���
	
	#else
	POWER_CARD_CLOSE;								
	#endif//#if( MAX_PHASE == 1 )
}
//-----------------------------------------------
//��������: ���жϷ�ʽ7816����һ���ֽ�
//
//����: 	����һ���ֽڵĻ���
//                   
//����ֵ:   BOOL ��ȡ����Ƿ�ɹ����ɹ���ı�Data�е�ֵΪ��ȡ�����ݡ�������ɹ�����Data��������Ϊff
//
//��ע:   18.12.30�����ӽ����ֽ��Ƿ�ɹ��жϣ���ֹ����ֽ�һֱ�ȴ����գ����¿��Ź���λ
//-----------------------------------------------

BOOL api_ReceiveCardByte( BYTE *Data )
{
	WORD Count;
	
	Count = 10000;//��ֹ����ʧ�ܣ�����Ϊ10000��IAR�����ж��Ż����������6000�û�����дʱ�䲻�����û�����д��Ҫ33ms
	
	#if( MAX_PHASE == 1 )
	//while( !(HT_UART4->ISO7816STA & UART_UARTSTA_RXIF) )
	while( (!(HT_UART4->ISO7816STA & UART_UARTSTA_RXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	HT_UART4->ISO7816STA &= ~UART_UARTSTA_RXIF;
	if(Count == 0)
	{
		ASSERT(FALSE,0);
		*Data = 0xff;
		return FALSE;//��ʱ����ܲ��������͵����ݣ�����ΪFALSE��
	}
	*Data = HT_UART4->SBUF;
	return TRUE;

	#else
	while( (!(HT_UART4->UARTSTA & UART_UARTSTA_RXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	HT_UART4->UARTSTA &= ~UART_UARTSTA_RXIF;
	if(Count == 0)
	{
		ASSERT(FALSE,0);
		*Data = 0xff;
		return FALSE;//��ʱ����ܲ��������͵����ݣ�����ΪFALSE��
	}
	*Data = HT_UART4->SBUF;
	return TRUE;
	#endif//#if( MAX_PHASE == 1 )
}



//-----------------------------------------------
//��������: ���жϷ�ʽ7816����һ���ֽ�
//
//����: 	д���ֽ�
//                   
//����ֵ:   TRUE/FALSE
//
//��ע:   
//-----------------------------------------------
BOOL api_SendCardByte( BYTE Data )
{
	WORD Count;
	
	Count = 2000;
	
	#if( MAX_PHASE == 1 )
	while( (!(HT_UART4->ISO7816STA & UART_UARTSTA_TXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	if( Count == 0 )
	{
		return FALSE;
	}
	
	HT_UART4->ISO7816STA &= ~UART_UARTSTA_TXIF;
	HT_UART4->SBUF = Data;
	
	#else
	while( (!(HT_UART4->UARTSTA & UART_UARTSTA_TXIF))&&(Count > 0) )
	{
		api_Delay10us( 1 );
		Count--;
	}
	if( Count == 0 )
	{
		return FALSE;
	}
	
	HT_UART4->UARTSTA &= ~UART_UARTSTA_TXIF;
	HT_UART4->SBUF = Data;
	#endif//#if( MAX_PHASE == 1 )
	
	return TRUE;
}


#endif//if( PREPAY_MODE == PREPAY_LOCAL )
//-----------------------------------------------
//��������: ����cpu Flash���ۼӺͣ�����word���ģʽ
//
//����: 	Type[in]0--��������Sum��	1--����������Sum��	ff--ȫ��flash����Sum��
//                    
//����ֵ:  	�����word�ۼӺ�
//
//��ע:95ms   
//-----------------------------------------------
WORD api_CheckCpuFlashSum(BYTE Type)
{
    DWORD Sum;
	DWORD StartZone,EndZone;
    WORD *pData;
    
    CLEAR_WATCH_DOG;
	
	#if( CPU_TYPE != CPU_HT6025 )
	��������cpu��Ҫ����������ʼ��ֹ��ַ
	#endif
	
	if( Type == 0 )
	{
		StartZone = 0;
		EndZone = 0x3f000;
	}
	else if(  Type == 1 )
	{
		StartZone = 0x3f000;		
		EndZone = 0x40000;

	}
	else
	{
		StartZone = 0;		
		EndZone = 0x40000;
	}
	
	Sum = 0;
 
	for( pData = (WORD *)StartZone; pData < (WORD *)EndZone; pData++ )
	{
		Sum += *pData;
	}
    
    CLEAR_WATCH_DOG;
    
    return (WORD)Sum;
}


//-----------------------------------------------
//��������: ��ȡcpu��ram��Ƭ��flash����
//
//����: 	Type[in]	0--Ram 	1--flash
//          Addr[in]	��ַ 
//			Len[in]		��ȡ����    
//			Buf[out]	�������     
//
//����ֵ:  	��ȡ���ݵĳ��ȣ����Ϊ0����ʾ��ȡʧ��
//
//��ע:   
//-----------------------------------------------
BYTE api_ReadCpuRamAndInFlash(BYTE Type, DWORD Addr, BYTE Len, BYTE *Buf)
{
	if( Type > 1 )
	{
		return 0;
	}
	
	if( (Addr>=0x00040400) && (Addr<0x20000000) )
	{
		return 0;
	}
	if( (Addr>=0x20008000) && (Addr<0x40000000) )
	{
		return 0;
	}
	if( Addr >= 0x40030000 )
	{
		return 0;
	}
	
	if( ((Addr+Len)>=0x00040400) && ((Addr+Len)<0x20000000) )
	{
		return 0;
	}
	if( ((Addr+Len)>=0x20008000) && ((Addr+Len)<0x40000000) )
	{
		return 0;
	}
	if( (Addr+Len) >= 0x40030000 )
	{
		return 0;
	}
	
	memcpy(Buf,(BYTE*)Addr,Len);
	
	return Len;
}


//-----------------------------------------------
//��������: ��������flash
//
//����: 	��     
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void UpdateProg( void ) 
{
	BYTE i,ProgMessageNum,ProgData[PROGRAM_FLASH_SECTOR];//һ������1024���ֽ�
	BYTE IntStatus;
	WORD j;
	DWORD ProgAddr;
	TUpdateProgMessage TmpUpdateProgMessage;
	TUpdateProgHead TmpUpdateProgHead;
	BYTE *pData;
	
	FunctionConst(UPDATEPROG_TASK);
	//��ȡ����
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TmpUpdateProgHead), (BYTE*)&TmpUpdateProgHead ) != TRUE )
	{
		return;
	}
	ProgAddr = TmpUpdateProgHead.ProgAddr;
	ProgMessageNum = TmpUpdateProgHead.ProgMessageNum;
	
	if( ProgMessageNum > (PROGRAM_FLASH_SECTOR/(sizeof(TmpUpdateProgMessage.ProgData))) )
	{
		return;
	}
	
//@	if( (ProgAddr%PROGRAM_FLASH_SECTOR) != 0 )
//@	{
//@		return;
//@	}
	
	memcpy( (void*)ProgData, (BYTE *)ProgAddr, sizeof(ProgData) );
	
	for(i=0; i<ProgMessageNum; i++)
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[i]), sizeof(TmpUpdateProgMessage), (BYTE*)&TmpUpdateProgMessage ) != TRUE )
		{
			return;
		}
		
		if( TmpUpdateProgMessage.ProgFrameLen )//�·�ʽ֡���Ȳ�Ϊ0
		{	
			memcpy(&ProgData[TmpUpdateProgMessage.ProgOffset], &TmpUpdateProgMessage.ProgData[0], TmpUpdateProgMessage.ProgFrameLen);
		}
		else
		{
			memcpy(&ProgData[sizeof(TmpUpdateProgMessage.ProgData)*i], &TmpUpdateProgMessage.ProgData[0], sizeof(TmpUpdateProgMessage.ProgData));
		}
	} 

	//��������
	for(i=0; i<2; i++)
	{
		IntStatus = api_splx(0);
		
		if( HT_Flash_PageErase( ProgAddr ) == FALSE )
		{
			api_splx(IntStatus);
			return;
		}
		
		if( HT_Flash_ByteWrite( ProgData, ProgAddr, PROGRAM_FLASH_SECTOR ) == FALSE )
		{
			api_splx(IntStatus);
			return;
		}
		
		api_splx(IntStatus);
		
		pData = (BYTE *)ProgAddr;
		for(j=0; j<PROGRAM_FLASH_SECTOR; j++)
		{
			if( ProgData[j] != *(pData+j) )
			{
				break;
			}
		}
		
		//����д�ɹ� ����ѭ��
		if( j == PROGRAM_FLASH_SECTOR )
		{
			break;;
		}
	}
	
	//����дʧ��
	if( (i == 2) && (j != PROGRAM_FLASH_SECTOR) )
	{
		return;
	}
	
	//��eeprom�еı���ͷ��Ϊ��Ч
	TmpUpdateProgHead.ProgAddr = 0x3fc01;
  	TmpUpdateProgHead.ProgMessageNum = (PROGRAM_FLASH_SECTOR/(sizeof(TmpUpdateProgMessage)-sizeof(DWORD)))+1;
  	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( UpdateProgSafeRom.UpdateProgHead ), sizeof(TmpUpdateProgHead), (BYTE *)&TmpUpdateProgHead );
}


//-----------------------------------------------
//��������: ��Լд����������Ϣͷ
//
//����: 	pBuf[in] ���뻺��     
//
//����ֵ:  	��
//
//��ע: ��Լ����������ƽ̨��չ��Լ����¼C  
//-----------------------------------------------
void api_WriteUpdateProgHead(BYTE *pBuf)
{
	TUpdateProgHead TmpUpdateProgHead;

	memcpy((BYTE*)&TmpUpdateProgHead.ProgAddr,pBuf,4);
	TmpUpdateProgHead.ProgMessageNum = pBuf[4];
	
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE*)&TmpUpdateProgHead );	
	
	//��״̬λ��һ�γ���������������磬���������е�����Ҫ��������
	ReceiveProgStatus = 0;
}


//-----------------------------------------------
//��������: ��Լд������������
//
//����: 	pBuf[in] ���뻺��     
//
//����ֵ:  	TRUE/FALSE
//
//��ע: ��Լ����������ƽ̨��չ��Լ����¼C  
//-----------------------------------------------
BOOL api_WriteUpdateProgMessage(BYTE *pBuf)
{
	TTwoByteUnion tw;
	TUpdateProgMessage TmpUpdateProgMessage;
	
	tw.w = 0;
	
	memcpy(tw.b,pBuf,2);	//֡���
	if( tw.w >= (PROGRAM_FLASH_SECTOR /(sizeof(TmpUpdateProgMessage.ProgData))) )
	{
		return FALSE;
	}
	
	//��������ʽ��֡���ȡ�ƫ�Ƶ�ַ��Ϊ 0
	TmpUpdateProgMessage.ProgFrameLen = 0; //֡����
	TmpUpdateProgMessage.ProgOffset = 0; //ƫ�Ƶ�ַ
	
	//128�ֽ����ݣ�������
	memcpy(TmpUpdateProgMessage.ProgData,pBuf+2,sizeof(TmpUpdateProgMessage.ProgData));
	
	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32( TmpUpdateProgMessage.ProgData, sizeof(TmpUpdateProgMessage)-sizeof(DWORD) );
	
	//���ܶԵ�֡��д��ֻ����ͷ��ʼ��д
	if( (ReceiveProgStatus&(0x01L<<tw.w)) == 0 )
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE*)&TmpUpdateProgMessage );
		ReceiveProgStatus |= (0x01L<<tw.w);
	}
	
	return TRUE;
}
//-----------------------------------------------
//��������: ��Լд������������
//
//����: 	pBuf[in] ���뻺��     
//
//����ֵ:  	TRUE/FALSE
//
//��ע: ��Լ����������ƽ̨��չ��Լ����¼C  
//-----------------------------------------------

BOOL api_WriteUpdateProgMessageNew(BYTE *pBuf)
{
	TTwoByteUnion tw;
	TTwoByteUnion len;
	TUpdateProgMessage TmpUpdateProgMessage;
	
	tw.w = 0;
	len.w = 0;
	
	memcpy(tw.b,pBuf,2);	//֡���
	if( tw.w >= (PROGRAM_FLASH_SECTOR/(sizeof(TmpUpdateProgMessage.ProgData))) )
	{
		return FALSE;
	}
	
	memcpy(len.b,pBuf+2,2);	//֡����
	if( (len.w > (sizeof(TmpUpdateProgMessage.ProgData))) || (len.w == 0) )
	{
		return FALSE;
	}
	TmpUpdateProgMessage.ProgFrameLen = len.w; //֡����
	
	memcpy((BYTE *)&TmpUpdateProgMessage.ProgOffset,pBuf+4,2); //ƫ�Ƶ�ַ
	if( (TmpUpdateProgMessage.ProgFrameLen + TmpUpdateProgMessage.ProgOffset) > PROGRAM_FLASH_SECTOR )
	{
		return FALSE;
	}

	memset(TmpUpdateProgMessage.ProgData,0,sizeof(TmpUpdateProgMessage.ProgData));
	memcpy(TmpUpdateProgMessage.ProgData,pBuf+6,len.w);
	
	TmpUpdateProgMessage.CRC32 = lib_CheckCRC32( (BYTE *)&TmpUpdateProgMessage, sizeof(TmpUpdateProgMessage)-sizeof(DWORD) );
	
	//���ܶԵ�֡��д��ֻ����ͷ��ʼ��д
	if( (ReceiveProgStatus&(0x01L<<tw.w)) == 0 )
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgMessage[tw.w]), sizeof(TmpUpdateProgMessage), (BYTE*)&TmpUpdateProgMessage );
		ReceiveProgStatus |= (0x01L<<tw.w);
	}
	
	return TRUE;
}


//-----------------------------------------------
//��������: ������������
//
//����: 	pBuf[in] ���뻺��     
//
//����ֵ:  	�˺�������ִ������ת�漰��λ
//
//��ע: ��Լ����������ƽ̨��չ��Լ����¼C  
//-----------------------------------------------
void api_StartUpdateProgMessage(BYTE *pBuf)
{
	BOOL Result;
	BYTE i,Num;
	TFourByteUnion tdw;	
	TUpdateProgHead TmpUpdateProgHead;
	
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(UpdateProgSafeRom.UpdateProgHead), sizeof(TUpdateProgHead), (BYTE*)&TmpUpdateProgHead );	
	if(Result != TRUE)
	{
		return;
	}
	
	memcpy(tdw.b,pBuf,4);
	if(tdw.d != TmpUpdateProgHead.ProgAddr)
	{
		return;
	}
	
	if(TmpUpdateProgHead.ProgMessageNum != pBuf[4])
	{
		return;
	}
	
	Num = 0;
	for(i=0;i<TmpUpdateProgHead.ProgMessageNum;i++)
	{
		if( ReceiveProgStatus & (0x01L<<i) )
		{
			Num++;
		}
	}
	if(Num != TmpUpdateProgHead.ProgMessageNum)
	{
		return;
	}
	
	//��һ�µ��籣�� !!!!!!
	api_PowerDownEnergy();
	
	ProgFlashPW = 0xa55a;
	
	UpdateProg();
	
	ProgFlashPW = 0;
	ReceiveProgStatus = 0;
	
	//RESET
	//Reset_CPU();
}

//---------------------------------------------------------------
//��������: HT6025K����д�����Ĵ���ά��
//
//����: 	��
//
//����ֵ:  	��
//
//��ע:
//---------------------------------------------------------------
void api_FreshNewRegister( void )
{
	EnWr_WPREG();                           //�ر�д����

	//HT6025K����д�����Ĵ���
	if(HT_CMU->LFDETCFG != 0x8020)
	{
		HT_CMU->CLKCTRL0 |= 0x020000;       //�ر�CMU����д����
		HT_CMU->LFDETCFG = 0x8020;          //��ЧλΪ1��������Ч������ֵΪĬ��ֵ
		HT_CMU->CLKCTRL0 &= ~0x020000;      //��CMU����д����
	}
	if(HT_CMU->MULTFUNCFG != 0x8000)
	{
		HT_CMU->CLKCTRL0 |= 0x020000;       //�ر�CMU����д����
		HT_CMU->MULTFUNCFG = 0x8000;        //��ЧλΪ1��������Ч������ֵΪĬ��ֵ
		HT_CMU->CLKCTRL0 &= ~0x020000;      //��CMU����д����
	}
	if(HT_CMU->LFCLKCFG != 0x8000)
	{
		HT_CMU->CLKCTRL0 |= 0x020000;       //�ر�CMU����д����
		HT_CMU->LFCLKCFG = 0x8000;          //��ЧλΪ1��������Ч������ֵΪĬ��ֵ
		HT_CMU->CLKCTRL0 &= ~0x020000;      //��CMU����д����
	}

	//������˼Ĵ��� ����оƬ�������ģʽ�ᵼ��CLKOUT����� Ӱ��ESAMͨѶ
	if((HT_HFCFG&0x80) != 0)
	{
		HT_HFCFG = 0x00; //��ʼ��Ϊ 0
	}

	DisWr_WPREG();                          //��д����
}
//-----------------------------------------------
//��������: ���MCUʹ�õ�ʱ��Ƶ��
//
//����: ��
//          
//		     
//
//����ֵ: 1��32.786K 0: 5.5M������
//
//��ע:   
//-----------------------------------------------
BYTE  api_CheckMCUCLKIsFlf( void )
{
	//����͹�����ʱ�Ӳ���Flf ǿ���л�һ��ʱ�� ��ֹʱ��û�б��л�����
	if( (HT_CMU->SYSCLKCFG&0x0007) != 0x0001 )//32768
	{
		return FALSE;
	}
	else 
	{
		return TRUE;
	}

}



/////////////////////////////////////////////////////////////////////
//�������ж���������
/////////////////////////////////////////////////////////////////////

//��λ�ж����� ������start.s�е�
//void Reset_Handler(void)
//{
//}

//������start.s�е� ������ж���λ
//void NMI_Handler(void)
//{
//}

//������start.s�е� ������ж���λ
//void HardFault_Handler(void)
//{
//	__NOP();
//}

//������start.s�е� ������ж���λ
//void SVC_Handler(void)
//{
//}

//������start.s�е� ������ж���λ
//void PendSV_Handler(void)
//{
//}

//10ms�ж�һ��
void SysTick_Handler(void)
{
	asm("nop");
	SysTickIRQ_Service();
}

//������start.s�е� ������ж���λ
void PMU_IRQHandler(void)
{
	PMUIRQ_Service();
}

//������start.s�е� ������ж���λ
//void AES_IRQHandler(void)
//{
//}


void EXTI0_IRQHandler(void)
{
	EXTI0IRQ_Service();
}


void EXTI1_IRQHandler(void)
{
	EXTI1IRQ_Service();
}


void EXTI2_IRQHandler(void)
{
	EXTI2IRQ_Service();
}


void EXTI3_IRQHandler(void)
{
	EXTI3IRQ_Service();
}


void EXTI4_IRQHandler(void)
{
	EXTI4IRQ_Service();
}


void EXTI5_IRQHandler(void)
{
	EXTI5IRQ_Service();
}


void EXTI6_IRQHandler(void)
{
	EXTI6IRQ_Service();
}


void UART0_IRQHandler(void)
{
	UART0IRQ_Service();
}


void UART1_IRQHandler(void)
{
	UART1IRQ_Service();
}


void UART2_IRQHandler(void)
{
	UART2IRQ_Service();
}


void UART3_IRQHandler(void)
{
	UART3IRQ_Service();
}


void UART4_IRQHandler(void)
{
	UART4IRQ_Service();
}


void UART5_IRQHandler(void)
{
	UART5IRQ_Service();
}


void TIMER_0_IRQHandler(void)
{
	TIMER0IRQ_Service();
}


void TIMER_1_IRQHandler(void)
{
	TIMER1IRQ_Service();
}


void TIMER_2_IRQHandler(void)
{
	TIMER2IRQ_Service();
}


void TIMER_3_IRQHandler(void)
{
	TIMER3IRQ_Service();
}

//������start.s�е� ������ж���λ
//void TBS_IRQHandler(void)
//{
//}


void RTC_IRQHandler(void)
{
	RTCIRQ_Service();
}

//������start.s�е� ������ж���λ
//void I2C_IRQHandler(void)
//{
//}

//������start.s�е� ������ж���λ
//void SPI0_IRQHandler(void)
//{
//}

//������start.s�е� ������ж���λ
//void SPI1_IRQHandler(void)
//{
//}

//������start.s�е� ������ж���λ
//void SelfTestFreq_IRQHandler(void)
//{
//}


void TIMER_4_IRQHandler(void)
{
	TIMER4IRQ_Service();
}


void TIMER_5_IRQHandler(void)
{
	TIMER5IRQ_Service();
}


void UART6_IRQHandler(void)
{
	UART6IRQ_Service();
}


void EXTI7_IRQHandler(void)
{
	EXTI7IRQ_Service();
}


void EXTI8_IRQHandler(void)
{
	EXTI8IRQ_Service();
}


void EXTI9_IRQHandler(void)
{
	EXTI9IRQ_Service();
}

//������start.s�е� ������ж���λ
//void DMA_IRQHandler(void)
//{
//}


//////////////////////////////////////////////////////////////////////////
// �����жϷ����ӳ���,�ķ������Ҫ���жϲ�����Ƕ��
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------
//��������: ���ڽ����жϺ���
//
//����: 	SCI_Ph_Num[in]	�����ں�
//          Data[in]		���յ�������
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
__IO void USARTx_Rcv_IRQHandler(BYTE SCI_Ph_Num, BYTE Data)
{
    BYTE i;

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
        {
            if(Serial[ i ].RXWPoint < MAX_PRO_BUF)
            {
	            Serial[ i ].ProBuf[Serial[ i ].RXWPoint++] = Data;
            }

            if(Serial[ i ].RXWPoint >= MAX_PRO_BUF)
            {
				//������ܵ����ݵ��ڴ������ֵ ����Э���ﻹһ���ֽڶ�û���� ǿ�Ƶ���һ�� �������512�ֽ����ݵ���и��ʲ��ظ�������
				if(Serial[i].RXRPoint == 0)
				{
					Proc645( i );
				}
                Serial[ i ].RXWPoint = 0;
            }
			Serial[i].SendToSendDelay = MAX_TX_COMMIDLETIME;
            break;
        }
    }
}


//-----------------------------------------------
//��������: ���ڷ����жϺ���
//
//����: 	SCI_Ph_Num[in]	�����ں�
//          USARTx[in]		��Ӧ�Ĵ���ָ��
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
__IO void USARTx_Send_IRQHandler(BYTE SCI_Ph_Num, HT_UART_TypeDef* USARTx)
{
    BYTE i, j;
    //BYTE IntStatus;

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
        {
            if(Serial[ i ].TXPoint < Serial[ i ].SendLength)
            {
                USARTx->SBUF = Serial[ i ].ProBuf[Serial[ i ].TXPoint++];
            }
            else//������ϣ��л�������״̬
            {
                // �ر��ж�
                USARTx->UARTCON &= ~0x0004; //��ֹ�����ж�
				
                //����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
				for( j = 0; j < MAX_COM_CHANNEL_NUM; j++ )
				{
					DealSciFlag( j );
				}
				//��ת����״̬
                SerialMap[i].SCIEnableRcv(SerialMap[i].SCI_Ph_Num);
                //IntStatus = api_splx(0);
                api_InitSciStatus(i);
                //api_splx(IntStatus);
				Serial[i].SendToSendDelay = MAX_RX_COMMIDLETIME;
            }
            break;
        }
    }
}

#endif//#if ( CPU_TYPE == CPU_HT6025 )
