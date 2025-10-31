//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.11
//����		�洢����С���塢��ȫ�ռ�������ռ����
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __SAVEMEM_API_H
#define __SAVEMEM_API_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
// ȡ�ṹ��ַ
#define GET_STRUCT_ADDR( StructName, ItemName )		(WORD)&( ((StructName *)(0))->ItemName )
#define GET_STRUCT_MEM_LEN( StructName, ItemName )		sizeof( ((StructName *)(0))->ItemName )

#if ( MASTER_MEMORY_CHIP == CHIP_24LC128 )
	#define MASTER_MEM_SPACE		((DWORD)16*1024)
#elif ( MASTER_MEMORY_CHIP == CHIP_24LC256 )
	#define MASTER_MEM_SPACE		((DWORD)32*1024)
#elif ( MASTER_MEMORY_CHIP == CHIP_24LC512 )
	#define MASTER_MEM_SPACE		((DWORD)64*1024)
#endif//#if ( MASTER_MEMORY_CHIP == CHIP_24LC256 )

#if  ( THIRD_MEM_CHIP == CHIP_GD25Q64C )
	#define THIRD_MEM_SPACE			((DWORD)8192*1024)
#elif ( THIRD_MEM_CHIP == CHIP_GD25Q32C )
	#define THIRD_MEM_SPACE			((DWORD)4096*1024)
#elif ( THIRD_MEM_CHIP == CHIP_GD25Q16C )
	#define THIRD_MEM_SPACE			((DWORD)2048*1024)
#elif ( THIRD_MEM_CHIP == CHIP_NO )
	#define THIRD_MEM_SPACE			(0)
#endif//#if  ( THIRD_MEM_CHIP == CHIP_GD25Q64C )

//��Ƭ�洢оƬ��С ���ο���һƬ ����ڶ�Ƭû�У���ѵ�һƬ�԰����
#if( SLAVE_MEM_CHIP == CHIP_NO )
	#define SINGLE_CHIP_SIZE		((DWORD)(MASTER_MEM_SPACE/2))
#else
	#define SINGLE_CHIP_SIZE		MASTER_MEM_SPACE
#endif//#if( SLAVE_MEM_CHIP == CHIP_NO )

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//��ȫ�ռ����
typedef struct TSafeMem_t
{
	#if( CPU_TYPE == CPU_HT6025 )
    TRtcSafeRom 		RtcSafeRom;
    #endif
	
	//����ģ���ڰ�ȫ�ռ�Ŀռ�
	//ע�⣺����ee�еĴ˽ṹ������Ҫ�ȴ�eSYS_STATUS_EN_WRITE_SAMPLEPARA��־��
	TSampleSafeRom		SampleSafeRom;
	//����ģ���ڰ�ȫ�ռ�Ŀռ�
	TParaSafeRom 		ParaSafeRom;
	//Һ��ģ���ڰ�ȫ�ռ�Ŀռ�
	TLcdSafeRom 		LcdSafeRom;
	//�洢���粻��ʧ���б�־�Ľṹ
	TRunHardFlagSafeRom RunHardFlagSafeRom;
	// �洢���粻��ʧ��Լ���ñ�־�Ľṹ
	TProHardFlagSafeRom ProHardFlagSafeRom;
	//����ģ���ڰ�ȫ�ռ�Ŀռ�
	TEnergySafeRom		EnergySafeRom;
	
	#if( MAX_PHASE == 3 )
	//����ģ���ڰ�ȫ�ռ�Ŀռ�
	TDemandSafeRom 		DemandSafeRom;
	#endif
	
	TRelaySafeRom 		RelaySafeRom;
	
	TPrePaySafeRom 		PrePaySafeRom;
	
	TProSafeRom  		ProSafeRom;
	
	TUpdateProgSafeRom 	UpdateProgSafeRom;
	
	TEventSafeRom		EventSafeRom;
	
	TFreezeSafeRom		FreezeSafeRom;
	
	TReportSafeRom		ReportSafeRom;
	
	TSysWatchSafeRom 	SysWatchSafeRom;
}TSafeMem;

#define SAFE_SPACE_START_ADDR					( 0 )
#define GET_SAFE_SPACE_ADDR( Addr_Name )		( (WORD)&(((TSafeMem *)SAFE_SPACE_START_ADDR)->Addr_Name) )


//��һƬ25640��ĩ��
#define LAST_ADDR_256401			( sizeof(TSafeMem) )
//�ڶ�Ƭ25640��ĩ��
#define LAST_ADDR_256402			( sizeof(TSafeMem) )


//�����������ռ����
//�����һƬ�����ռ俪ʼ��ַ
#define SINGLE1_CONTINUE_ADDR		(DWORD)LAST_ADDR_256401
//�����һƬ�����ռ��С
#define SINGLE1_CONTINUE_SIZE		(DWORD)((DWORD)SINGLE_CHIP_SIZE-LAST_ADDR_256401)

//����ڶ�Ƭ�����ռ俪ʼ��ַ
#define SINGLE2_CONTINUE_ADDR		(DWORD)LAST_ADDR_256402
//����ڶ�Ƭ�����ռ��С
#define SINGLE2_CONTINUE_SIZE		(DWORD)((DWORD)SINGLE_CHIP_SIZE-LAST_ADDR_256402)

#define CONTINUE_TOTAL_SIZE			(DWORD)((DWORD)SINGLE1_CONTINUE_SIZE+(DWORD)SINGLE2_CONTINUE_SIZE)


//�����ռ�ṹ����
typedef struct TConMem_t
{		
    //���������ռ�Ĳ��� ��У��ķ�ʽ
    TParaConRom 		ParaConRom;
    //��ʾ������
    TLcdConRom 			LcdConRom;
    //�쳣��Ϣ
    TSysAbrEventConRom 	SysAbrEventConRom;
    //�����¼�
    TSysFreeEventConRom SysFreeEventConRom;
    //�̵��������ռ�
    TRelayConRom		RelayConRom;
    
    //�͹��������ռ�
    TLowPowerConRom		LowPowerConRom;

    BYTE FactoryRcvArea[64];

    //��ع���ʱ�䣬��λ����
    DWORD BatteryTime;
    
    //Ԥ���������ռ�
    TPrePayConMem		PrePayConMem;
    #if( SEL_AHOUR_FUNC == YES)
    TAHour				AHourConRom;
    #endif	
    
    TEventConRom		EventConRom;

	TFreezeConRom		FreezeConRom;

    TReportConRom		ReportConRom; 
	
}TConMem;

#define CONTINUE_SPACE_START_ADDR		0
#define GET_CONTINUE_ADDR( Addr_Name )		( (DWORD)&(((TConMem *)CONTINUE_SPACE_START_ADDR)->Addr_Name) )


//FLASH���ݿռ�ṹ����
typedef struct TFlashMem_t
{
	TFreezeDataInfo		MinDataInfo[MAX_FREEZE_PLANNUM];	// ���Ӷ��ᣬÿ��������һ��RecordNo

	TEnergyRom 			Energy;	// ���ܣ������������
}TFlashMem;


#if( SLE_THIRD_MEM_BACKUP == YES )
	#if(MAX_PHASE == 1)
	#define FLASH_MEM_SECTOR_NUM		(55)
	#else
	#define FLASH_MEM_SECTOR_NUM		(155)
	#endif
	#define FLASH_MEM_NUM_PER_SECTOR	(SECTOR_SIZE/sizeof(TFlashMem))
	#define FLASH_MEM_DEPTH				(FLASH_MEM_NUM_PER_SECTOR*(FLASH_MEM_SECTOR_NUM-5))
#else
	#define FLASH_MEM_SECTOR_NUM		(0)	// FLASH���ݿռ�������
	#define FLASH_MEM_DEPTH				(0)	// FLASH���ݿռ����
#endif

#define FLASH_MEM_DATA_LEN				(sizeof(TFlashMem))					// FLASH���ݿռ�1�����ݵĳ���
#define FLASH_MEM_BASE_ADDR				(FLASH_FREEZE_END_ADDR)				// FLASH���ݿռ���ʼ��ַ
#define FLASH_MEM_SIZE					(FLASH_MEM_SECTOR_NUM*SECTOR_SIZE)	// FLASH���ݿռ��С
#define FLASH_MEM_END_ADDR				(FLASH_MEM_BASE_ADDR+FLASH_MEM_SIZE)// FLASH���ݿռ������ַ

// FLASH���ݿռ�ָ
#define FLASH_MEM_RECORDNO_LEN				(4)												// ����
#define FLASH_MEM_RECORDNO_SECTOR_NUM		(1)												// ��������
#define FLASH_MEM_RECORDNO_SIZE				(FLASH_MEM_RECORDNO_SECTOR_NUM*SECTOR_SIZE)		// ��С
#define FLASH_MEM_RECORDNO_ADDR 			(FLASH_MEM_END_ADDR-FLASH_MEM_RECORDNO_SIZE)	// ��ַ
#define FLASH_MEM_RECORDNO_DEPTH			(FLASH_MEM_RECORDNO_SIZE/FLASH_MEM_RECORDNO_LEN)// ���

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ����ȫ�ռ�,Ҫ�󳤶ȱ����ǽṹ�ĳ��ȣ�����У�����ڣ�
//			���Ұ�ȫ�ռ�ĵڶ���һ����ȡ���洢��У��̶���Ϊ��CRC32
//
//����: 
//			Addr[in]				��ʼ��ַ
//			Length[in]				���ݳ��ȣ������ṹ�ĳ��ȣ�����У��
//			pBuf[in/out]			��Ŷ��������ݣ�����У��
//                    
//����ֵ:  	TRUE:		��һ�ݶ�����Ϊ�Ƕ�
//			FALSE��		���ݶ�����
//��ע:   
//-----------------------------------------------
WORD api_VReadSafeMem( WORD Addr, WORD Length, BYTE * pBuf );

//-----------------------------------------------
//��������: д��ȫ�ռ�,Ҫ�󳤶ȱ����ǽṹ�ĳ��ȣ�����У�����ڣ�
//			���Ұ�ȫ�ռ�ĵڶ�����ȡ���洢,У��̶���Ϊ��CRC32�����У�鲻�ԣ��ú������Զ����
//
//����: 
//			Addr[in]				��ʼ��ַ
//			Length[in]				���ݳ��ȣ������ṹ�ĳ��ȣ�����У��
//			pBuf[in/out]			��Ŷ��������ݣ�����У��
//                    
//����ֵ:  	TRUE:��ȷд������������	FALSE:��һ�ݻ���������û����ȷд��
//
//��ע:   
//-----------------------------------------------
WORD api_VWriteSafeMem( WORD Addr, WORD Length, BYTE * pBuf );

//-----------------------------------------------
//��������: �尲ȫ�ռ�,Ҫ�󳤶ȱ����ǽṹ�ĳ��ȣ�����У�����ڣ�
//			���Ұ�ȫ�ռ�ĵڶ�����ȡ���洢,У��̶���Ϊ��CRC32���ú������Զ����У��
//
//����: 
//			Addr[in]				��ʼ��ַ
//			Length[in]				���ݳ��ȣ������ṹ�ĳ��ȣ�����У��
//                    
//����ֵ:  	TRUE:��ȷд������������	FALSE:��һ�ݻ���������û����ȷд��
//
//��ע:   
//-----------------------------------------------
WORD api_ClrSafeMem( WORD Addr, WORD Length );

//-----------------------------------------------
//��������: д�����ռ亯��
//
//����: 
//			Addr[in]		Ҫд����ʼ��ַ
//			Length[in]		Ҫд�ĳ���
//			pBuf[in/out]	д����ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_WriteToContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf);

//-----------------------------------------------
//��������: �������ռ�����ݵĺ���
//
//����: 
//			Addr[in]		Ҫ������ʼ��ַ
//			Length[in]		Ҫ���ĳ���
//			pBuf[in/out]	������ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_ReadFromContinueEEPRom(DWORD Addr, WORD Length, BYTE * pBuf);

//-----------------------------------------------
//��������: �Ѵ�Ƭ�������ռ�д��0����ҪΪ�¼�����Ȳ���ʹ�á����ж��Ƿ�����ɹ�
//
//����: 
//			Addr[in]		Ҫ�������ʼ��ַ
//			Length[in]		Ҫ����ĳ���
//                    
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void api_ClrContinueEEPRom(DWORD Addr, WORD Length);


//-----------------------------------------------
//��������: �������ռ�����ݼ�����,Ҫ�󳤶ȱ����ǽṹ�ĳ��ȣ�����У�����ڣ�
//			�ڶ���ȡ���洢��У��̶���Ϊ��CRC32
//
//����: 
//			Addr1[in]				��һ���������ռ����ʼ��ַ
//			Addr2[in]				�ڶ����������ռ����ʼ��ַ,��Ϊ0���򲻶��ڶ���
//			Length[in]				���ݳ��ȣ������ṹ�ĳ��ȣ�����У��
//			pBuf[in/out]			��Ŷ��������ݣ�����У��
//                    
//����ֵ:  	TRUE:		��һ�ݶ�����Ϊ�Ƕ�
//			FALSE��		���ݶ�����
//��ע:   
//-----------------------------------------------
WORD api_VReadContinueMem( DWORD Addr1, DWORD Addr2, BYTE Length, BYTE * pBuf );

//-----------------------------------------------
//��������: �������¼�������
//
//����: 
//			Addr[in]		Ҫ������ʼ��ַ
//			Length[in]		Ҫ���ĳ���
//			pBuf[in/out]	������ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:����flash�Ķ�flash����û��flash�Ķ�eeprom������   
//-----------------------------------------------
BOOL api_ReadMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf);

//-----------------------------------------------
//��������: д�����¼�������,����ַУ��
//
//����: 
//			Addr[in]		Ҫд����ʼ��ַ
//			Length[in]		Ҫд�ĳ���
//			pBuf[in/out]	д����ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:�˺���ֻдFlash�ж����¼�������
//-----------------------------------------------
BOOL api_WriteMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf);

//-----------------------------------------------
//��������: �����д洢оƬָ����ַ���ݣ�Ϊ���ڹ��ܵ���д�ĺ���
//
//����: 
//			No[in]			ָ������һƬEEPROM
//							0x01����һƬEEPROM
//							0x02���ڶ�ƬEEPROM
//							0x11����һƬFlash
//							0x21: ��һƬ����
//							0x31: �������ռ�
//			Addr[in]		Ҫ������ʼ��ַ��оƬ��ƫ�Ƶ�ַ��
//			Length[in]		Ҫ�����ݵĳ���
//			pBuf[out]		Ҫ�������ݵĻ���
//                    
//����ֵ:  	�������ݵĳ��ȣ����Ϊ0����ʾʧ��
//
//��ע: ����ģ�龡����Ҫ�ô˺���  
//-----------------------------------------------
WORD api_ReadSaveMem( BYTE No, DWORD Addr, WORD Length, BYTE *pBuf );

//-----------------------------------------------
//��������: ����洢ģ����ϵ��ʼ������
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_PowerUpSave( void );

void api_FactoryInitEEPRomAddr( void );
BOOL api_CheckEEPRomAddr(BYTE Type);
BOOL api_GetEEPRomAddr( BYTE Type, BYTE* Buf );

#endif//#ifndef __SAVEMEM_API_H


