//------------------------------------------------------------------------------------------
//	��    �ƣ� main.c
//	��    ��:  ϵͳ��ں���
//	��    ��:  liuzhijun
//	����ʱ��:  2002.6
//	����ʱ��:
//	��    ע:
//	�޸ļ�¼:
//------------------------------------------------------------------------------------------

#include "AllHead.h"
#include "para_API.h"
#include "SEGGER_RTT.h"
extern void root_init(void);

//������ϵͳ��ȷ��
void CheckSysLimit( void )
{
	#if( MAX_TIME_SEG_TABLE > 10 )//���鲻Ҫ����10
	for(;;);
	#endif
	
	#if( MAX_RATIO > 12 )//���鲻Ҫ����12
	for(;;);
	#endif
	
	#if( MAX_HOLIDAY > 20 )//�������մ���21����ڴ�
	for(;;);
	#endif
	
	#if( eTASK_ID_NUM_T > 16 )
	for(;;);
	#endif
	
	//���¼����궨�帺ֵ���������嶼��BYTE����
	#if( cMETER_TYPES > 255 )
	for(;;);
	#endif
	
	#if( cCURR_TYPES > 255 )
	for(;;);
	#endif
	
	#if( cSPECIAL_CLIENTS > 255 )
	for(;;);
	#endif
	
	#if( SLAVE_MEM_CHIP != CHIP_NO )
	#if ( MASTER_MEMORY_CHIP != SLAVE_MEM_CHIP )
		"���ô�����Ƭeeprom������һ����"
	#endif
	#endif
	
	#if ( MAX_DEMAND_DOT < DEFAULT_DEMAND_DOT )
	����DEMAND_DOT
	#endif

	if(eTASK_ID_NUM_T > MAX_TASK_ID_NUM)
	{
		for(;;);
	}
	
	if(eSYS_STATUS_NUM_T > MAX_SYS_STATUS_NUM)
	{
		for(;;);
	}
	
	if(eRUN_HARD_NUM_T > MAX_RUN_HARD_FLAG_NUM)
	{
		for(;;);
	}
	
	if(ePRO_HARD_NUM_T > MAX_PRO_HARD_FLAG_NUM)
	{
		for(;;);
	}

	#if( MAX_PHASE != 1 )
	if( SelDivAdjust == NO )
	{
		//���������رշֶ�У׼
		for(;;);
	}
	#endif

	// ����ƬFlash����Ϊ�����ռ䴦��
	if( ((DWORD)2*sizeof(TSafeMem)+sizeof(TConMem)) > ((DWORD)SINGLE_CHIP_SIZE*2) )
	{
		for(;;);
	}

	#if(THIRD_MEM_CHIP != CHIP_NO)
	//	�¼�+���������
//	if( FLASH_FREEZE_END_ADDR > THIRD_MEM_SPACE )
//	{
//		for(;;);
//	}
	#endif

	//ֻ��ֱͨ�������ߵ�����������
	if(SelZeroCurrentConst == YES)
	{
		if(MeterTypesConst != METER_ZT)
		{
			for(;;); 
		}
	}

	#if( MAX_PHASE == 3)
	//��ʡ�������ܱ�ֱ��ż��г�����ʱ��
	if((SoftVersionConst >> 8) != 0xFF )//��ֱͨ�� ������ֱ��ż��г��
	{
		if(HalfWaveDefCheckTime == 0xFF)
		{
			for(;;); 
		}

	}

	if(MeterTypesConst != METER_ZT)
	{
		if(HalfWaveDefCheckTime != 0 )
		{
			for(;;); 
		}
	}
	#endif
	
	#if( MAX_PHASE == 3)

	if( ( (SoftVersionConst&0xff00) != 0xff00)
	|| (ESAMPowerControlConst ==NO) )//��ʡ�汾�� ����״̬ �������������л�
	{
		if( SmallCurrentCorrection == YES )
		{
			for(;;); 
		}
	}
	
	#endif
        
}


void CheckImportantPara( void )
{
	//��ֹ���峣��̫�󣬶����ܴ洢�ֲ�������洢�����
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == TRUE )
	{
		if( api_GetEnergyConst() == 1 )
		{
			if( FPara2->ActiveConstant > (DWORD)2*ActiveConstantConst )
			{
//				api_SetError(ERR_PULSCONST_TOO_BIG);//¼�����õ���
			}
		}
	}
}

#define TEST_PORT PortD
#define TEST_PIN Pin03


extern void toggleTestPin();

void initTestPin()
{
	stc_port_init_t stcPortInit;
	/* configuration structure initialization */
	MEM_ZERO_STRUCT(stcPortInit);

	stcPortInit.enPinMode = Pin_Mode_Out;
	stcPortInit.enExInt = Enable;
	stcPortInit.enPullUp = Enable;
	stcPortInit.enPinDrv = Pin_Drv_H;
	stcPortInit.enInvert = Disable;
	stcPortInit.enLatch = Disable;
	stcPortInit.enPinOType = Pin_OType_Cmos;
	stcPortInit.enPinSubFunc = Disable;
	PORT_Init(TEST_PORT, TEST_PIN, &stcPortInit);
	PORT_ResetPortData(TEST_PORT, TEST_PIN);
	PORT_SetPortData(TEST_PORT, TEST_PIN);
	PORT_ResetPortData(TEST_PORT, TEST_PIN);
}
void toggleTestPin()
{
	PORT_Toggle(TEST_PORT, TEST_PIN);
}


void TransmitDataInAsciiFormat(DWORD Data)
{
  BYTE AsciiString[60] = {0};
  WORD StringLength = 0;
  BYTE tempNum1 = 0;
  WORD tempNum2 = 0;
  DWORD Num = 1000000;
  WORD i = 0;
  
  while(Num > 1)
  {
    tempNum1 = Data/Num;//�������λ
    Data = Data%Num;//����ʣ�µ�����
    Num /= 10;//
    
    AsciiString[StringLength] = '0' + tempNum1;
    StringLength ++;
  }
  
  AsciiString[StringLength] = '0' + Data;
  StringLength ++;
  AsciiString[StringLength] = ' ';
  StringLength ++;//��ֵת�����
  
  for(i = 0; i < StringLength; i ++)
  {
    if(AsciiString[i] != '0')
    {
      tempNum2 = i;
      break;//�ҵ���һ������0������
    }
  }
  for(i = tempNum2; i < StringLength; i ++)
  {
  		USART_SendData(M4_USART3, AsciiString[i]);	//д������
    api_Delayms(3);
  }
}

void api_USART_SEND(DWORD dwData)
{
	#if(SELECT_4851_SEND_DATA == 1)
	USART_FuncCmd(M4_USART3, UsartTx, Enable);							//����ʹ��
	USART_FuncCmd(M4_USART3, UsartTxEmptyInt, Disable);					//�����ж�ʹ��
	USART_SendData(M4_USART3, dwData);
	api_Delay100us(2);
	USART_FuncCmd(M4_USART3, UsartRx, Enable);
	USART_FuncCmd(M4_USART3, UsartRxInt, Enable);
	NVIC_EnableIRQ(Int012_IRQn);	
	#endif
	// api_Delay100us(2);
	// ENABLE_HARD_SCI_3_RECE;
}


//--------------------------------------------------
//��������:  //�ض���fputc���� 
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
int fputc(int ch, FILE *f)
{  
	#if( SEL_DEBUG_VERSION == YES )
	SEGGER_RTT_PutChar(0, ch);
	#endif
	return ch;
}
//��ѭ��
void MainTask(void)
{
	#if(SELECT_4851_SEND_DATA == 1)
		ENABLE_HARD_SCI_3_SEND;	
		api_Delay100us(2);
	#endif
	for(;;)
	{
		// toggleTestPin();
		CLEAR_WATCH_DOG;
		//api_CheckAllocBuf();
		//�ж�ϵͳ״̬
		//��ϵͳ���ڵ͹���״̬����ִ�����³���
		if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )	
		{
			api_WAVE_Task();
			
			// ϵͳ���Ӵ�����ͣ��״̬��ҲҪִ�С�
			api_SysWatchTask();
//			FunctionConst(SYS_WATCH_TASK);
			
			if( api_CheckSysVol( eOnRunDetectPowerMode ) == FALSE )
			{			
				api_EnterLowPower( eFromOnRunEnterDownMode );
			}	
			//�ɼ�����
//			FunctionConst(SAMPLE_TASK);

			api_SampleTask();
			//��������
			#if( MAX_PHASE == 3 )
			// api_DemandTask();
			#endif
			
			//���ܴ���
			// api_EnergyTask();

			if( api_CheckSysVol( eOnRunDetectPowerMode ) == FALSE )
			{
				api_EnterLowPower( eFromOnRunEnterDownMode );
			}

			// api_MidLayerTask();

			// api_EventTask();
			
			#if(SEL_EVENT_DI_CHANGE == YES)
			// api_DIProcTask();
			#endif
			
			// RelayTask();
			
			FunctionConst(PRAYPAY_TASK);
			
			// api_PrePayTask();
			api_ParaTask();
			//����֮ǰ���ӵ����ж�
			if( api_CheckSysVol( eOnRunDetectPowerMode ) == FALSE )
			{
				api_EnterLowPower( eFromOnRunEnterDownMode );
			}
			//���ᴦ��-������api_ParaTask()֮��,�Ҳ����жϵ��硣��para�������л���־����磬�ᶪһ��Լ������
			// api_FreezeTask();
			//Э�鴦��

			FunctionConst(PROTOCOL_TASK);
			
			//�ڴ�ѭ���������Ҫ�������͹����ж�
			if( api_CheckSysVol( eOnRunDetectPowerMode ) == FALSE )
			{
				api_EnterLowPower( eFromOnRunEnterDownMode );
			}
		}
		else
		{
			if( api_CheckSysVol( eWakeupDetectPowerMode ) == TRUE )//�͹����ϵ�
			{
				Reset_CPU();
			}
			
			api_LowPowerTask();
		}
		
		api_ProtocolTask();


		// if (api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE)
		// {
		// 	 api_MCollectTask();
		// }
		
		//LCD����
		FunctionConst(LCD_RUN_TASK);
		// api_LcdTask();
		FunctionConst(RTC_RUN_TASK);
		api_RtcTask();//RTC�������Э������֮�󣬱��ⶳ���־������Э�����RTC���¶���ʱ�䲻��

	}
}



#if( SEL_DEBUG_VERSION == YES )
DWORD hchtest[40];
void TestCode()
{
	WORD i,m,n;
	
	CLEAR_WATCH_DOG;
	hchtest[0] = sizeof(TSafeMem);
	hchtest[1] = sizeof(TConMem);
	hchtest[2] = sizeof(TFreezeConRom);
//	hchtest[3] = sizeof(TEventConRom);
    //hchtest[5] = sizeof(TFreezeAttRom);
    hchtest[6] = sizeof(TFreezeEeData);
//    hchtest[7] = FLASH_FREEZE_BASE;
//    hchtest[8] = FLASH_FREEZE_BASE+ 
	//hchtest[4] = sizeof(TFreezeAtt);
	//hchtest[5] = GET_CONTINUE_ADDR( FreezeAtt );
	//hchtest[6] = sizeof(PortDescript8[0]);
    //hchtest[7] = strlen(PortDescript);
    #if(THIRD_MEM_CHIP != CHIP_NO)
//    hchtest[7] = FLASH_FREEZE_END_ADDR;
    #endif
	//hchtest[8] = GET_CONTINUE_ADDR( FreezeAtt.FreezeTimeZone );
//	hchtest[9] =  sizeof(gEventPara.byLimit);
    
	//hchtest[10] =  GET_STRUCT_MEM_LEN( TDemandOverRom, EventData[0] );
	
    //hchtest[11] = GET_STRUCT_MEM_LEN( TDemandOverRom, EventData[0] );
	//memset( hchtest, 0XFF, 20 );
	//	hchtest[5] = lib_CheckCRC32( (BYTE*)hchtest, 20 );

	// ��������led
	LIGHT_RUN_TOGGLE;
	PORT_Toggle(PortB, Pin04);
	PORT_Toggle(PortD, Pin07);
	
	// ����eeprom1��2
	memset((BYTE *)hchtest, 0x01, 10);
	memset( (BYTE *)hchtest+12, 0x02, 10 );
	WriteEEPRom1( 20000, 10, (BYTE *)hchtest );
	WriteEEPRom2( 20000, 10, (BYTE *)hchtest+12 );
	memset( (BYTE *)hchtest+24, 0x00, 24 );
	ReadEEPRom1( 20000, 10, (BYTE *)hchtest+24 );
	ReadEEPRom2( 20000, 10, (BYTE *)hchtest+36 );
	//���Բ���оƬ
    
//  	Link_ReadSampleReg( 0, (BYTE *)&hchtest[0] );
//  	
//  	//POWER_ESAM_CLOSE;
//  	
//  	//����esam
//  	POWER_ESAM_OPEN;
//	api_GetEsamInfo( 0x02, (BYTE *)&hchtest[0] );
//  	
  	//����eeprom1
// 	memset( hchtest, 0x01, 10 );
//  	WriteEEPRom1( 20000, 10, (BYTE *)hchtest );
// 	memset( (BYTE *)hchtest+12, 0x00, 10 );
// 	ReadEEPRom1( 20000, 10, (BYTE *)hchtest+12 );
//
// 	memset( hchtest, 0x02, 10 );
//  	WriteEEPRomM( 20000, 10, (BYTE *)hchtest );
// 	memset( (BYTE *)hchtest+12, 0x00, 10 );
// 	ReadEEPRomM( 20000, 10, (BYTE *)hchtest+12 );
//        
//    
//  	
//  	//����eeprom2
//  	memset( hchtest, 0x02, 10 );
//  	WriteEEPRom2( 20000, 10, (BYTE *)hchtest );
//  	memset( (BYTE *)hchtest+12, 0x00, 10 );
//  	ReadEEPRom2( 20000, 10, (BYTE *)hchtest+12 );
////  	
////  	//����flash
////  	#if( BOARD_TYPE == BOARD_HT_THREE_0131037 )
	memset( hchtest, 0xaa, 10 );
	api_WriteMemRecordData(3L*4*1024, 10, (BYTE *)hchtest);
 	memset( hchtest, 0x55, 10 );
 	api_ReadMemRecordData(3L*4*1024, 10, (BYTE *)hchtest);
 	api_ReadMemRecordData(0, 100, (BYTE *)hchtest);
////  	#endif
//
//	testFlash();
   	
  	//api_ClrDemand(eClearAllDemand);
  	
  	//api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
  	//api_FactoryInitTask();
  	
  	//ʱ���������
  	/*
  	EnWr_WPREG();//�رռĴ���д����
  	HT_GPIOE->IOCFG |= GPIO_Pin_3;
  	HT_GPIOC->AFCFG &= ~GPIO_Pin_3;
  	HT_GPIOE->PTDIR |= GPIO_Pin_3;                                                                                                                                                   
  	HT_GPIOE->PTOD |= GPIO_Pin_3;
  	HT_CMU->CLKOUTSEL = 0x02;
  	HT_CMU->CLKOUTDIV = 7;
  	HT_CMU->CLKCTRL0 |= 0x1000;
  	
  	for(;;)
  	{
  		CLEAR_WATCH_DOG;
  	}  	*/
} 
#endif



  void main(void)
{
	DISABLE_CPU_INT;		// �ر��ж�

	//������ϵͳ��ȷ��
	CheckSysLimit();
	
	//!!!!!!�ڴ��ʼ��һ��Ҫ������ǰ�棬��Ϊ�����˶Ѳ���!!!!!!
	//api_InitAllocBuf();
	
	api_InitCPU();
    #if( SEL_DEBUG_VERSION == YES )    
	SEGGER_RTT_Init();
	printf("\r\n power on ");
    #endif
	InitBoard();
	
	#if( SEL_DEBUG_VERSION == YES )
//	TestCode();
	#endif
	
	CheckImportantPara();
	
//	root_init();

	// ble_protocol_init();

  	//�����ж�  �ڽ���ѭ��֮ǰ�Ŵ��ж�
	ENABLE_CPU_INT;
		
    //���������
	MainTask();
}


