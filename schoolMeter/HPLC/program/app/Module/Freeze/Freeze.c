//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.8.27
//����		����ģ��Դ�ļ�
//�޸ļ�¼	
//---------------------------------------------------------------------- 

#include "AllHead.h"
#include "Freeze.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define FREEZE_ATTRIBUTE_LEN				(2+4+2)				// �������ڣ�OAD���洢���
#define	FREEZE_RECORD_NO_OAD				0x00022320			// �����¼���OAD
#define	FREEZE_RECORD_TIME_OAD				0x00022120			// ����ʱ��OAD
#define MAX_DEMAND_START_OI					0x1010				// �����й��������
#define MAX_DEMAND_END_OI					0x10A3				// C�෴�������������
#define ZEROCURRRNT_OAD						0x00040120			// ���ߵ���OAD
#define CURRRNTVECTORSUM_OAD				0x00060120			// �������OAD
#define VALID_OAD_INDEX						20					// OAD���������ֵ
#define OI4800_04_SIZE						13					// 48000400 һ���ṹ���Ӧ�ĳ���
#define FREEZEBUFFERTIME					10					// ���Ӷ������ݻ���ʱ��

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
	eGET_FREEZE_DATA,				// ��ȡ����
	eGET_FREEZE_TIME,				// ��ȡʱ��
}eGetType;

typedef enum
{
	eTO_FILL_TYPE,				// Ҫ������ݵ�����
	eNOT_TO_FILL_TYPE,			// ������������
	eUNSUPPORTED_TYPE			// ��֧�ֵ�����
}eFillorNotType;

typedef enum
{
	eALL_OAD_LEN,				// ����OAD�ĳ���
	eSELECTED_OAD_LEN			// ѡ�е�OAD�ĳ���
}eOadLenType;

typedef enum
{
	eFREEZE_DATA_ADDR,				// �������ݵ�ַ
	eFREEZE_DEPTH,					// �������ݶ�Ӧ�����
	eFREEZE_CYCLE,					// ÿ�ֶ����Ӧ������ ����Ϊ��λ
}eLastTimeType;

typedef enum
{
	eFREEZE_RECORDNO,				// ��ȡ�����¼���
	eFREEZE_SAVE_DOT,			// ��ȡ�����¼ʣ�����
}eReadFreezeType;


typedef struct TTBinarySchInfo_t
{
	DWORD		dwBaseAddr;			//in �����Զ�Ӧ���ݵ��׵�ַ
	DWORD		dwDepth;			//in �������
	DWORD		dwSaveDot;			//in ��Ҫ����ʣ��Ķ����¼���������ж��ַ�����ʱ�õ�
	DWORD		dwRecordNo;			//in ���һ����ļ�¼��
	DWORD		Time;				//in Ҫ������ʱ��
	DWORD		*pRecordNo;			//out �����ҵ���RecordNo
	DWORD		*pTime;				//out �����ҵ���ʱ��
	WORD		wLen;				//in �����Ե������ݵĳ���
	BYTE		FreezeIndex;		//in ��������

	BYTE 		MinPlan;			//���Ӷ��᷽��
	DWORD 		MinBakAddr;			//���Ӷ��ᱸ���� x��������ʼ��ַ
}TBinarySchInfo;

// û��EEP��ÿ���õ�ʱ�����
typedef struct TSectorInfo_t
{		
	WORD		wPointer;		// Ѱ�ҵ�ָ��
	DWORD		dwAddr;			// Ѱ�ҵ��ַ		
	DWORD		dwSaveSpace;	// ��OAD�洢�ռ��С�����Ӷ������ã������������ڸ��Ĺ����������Ա�ʱ��Խ��
}TSectorInfo;

typedef struct TReadFreezeRecordInfo_t
{
	BYTE		FreezeIndex;	// ��������
	DWORD 		RecordNo;		// �����¼���к�
	DWORD		dwAddr;			// ��������Flash��ַ		
	WORD		RecordLen;		//�����������ݳ���

	BYTE 		MinPlan;		// ���Ӷ��᷽��(ֻ�з��Ӷ���)
	DWORD 		dwBakAddr;		// ���Ӷ��� ���ݵ�ַ	
}TReadFreezeRecordInfo;

typedef struct TCompleteFreezeTime_t
{
	TRealTimer	CompleteFreezeClosingTime;
	TRealTimer	CompleteFreezeLadderClosingTime;
	DWORD CRC32;
}TCompleteFreezeTime;



T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];
BYTE minFreezeFlag;
BYTE SecondBufferTime;
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];
BYTE minFreezeFlag;
BYTE SecondBufferTime;
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//���ű��˳��Ҫ��eFreezeType���涨���˳��һ��
const TFreezeInfoTab	FreezeInfoTab[] = 
{
	{0x5005,	eFREEZE_CLOSING,			MAX_CLOSING_FREEZE_ATTRIBUTE,		MAX_CLOSING_FREEZE_DATA_LEN,		MAX_CLOSING_FREEZE_E2_DEPTH,},	//�����ն���
	#if( PREPAY_MODE == PREPAY_LOCAL )
	{0x500a,	eFREEZE_TARIFF_RATE_CHG,	MAX_TARIFF_RATE_FREEZE_ATTRIBUTE,	MAX_TARIFF_RATE_FREEZE_DATA_LEN,	0,							},	//���ʵ���л�
	{0x500b,	eFREEZE_LADDER_CHG,			MAX_LADDER_FREEZE_ATTRIBUTE,		MAX_LADDER_FREEZE_DATA_LEN,			0,							},	//�����л� 0X0B
	{0x5011,	eFREEZE_LADDER_CLOSING,		MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE,MAX_LADDER_CLOSING_FREEZE_DATA_LEN,0,							},	//���ݽ���
	#endif
	{0x5008,	eFREEZE_TIME_ZONE_CHG,		MAX_TIME_ZONE_FREEZE_ATTRIBUTE,		MAX_TIME_ZONE_FREEZE_DATA_LEN,		0,							},	//ʱ�����л�
	{0x5009,	eFREEZE_DAY_TIMETABLE_CHG,	MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE,	MAX_DAY_TIMETABLE_FREEZE_DATA_LEN, 	0,							},	//��ʱ�α��л�
	{0x5000,	eFREEZE_INSTANT,			MAX_INSTANT_FREEZE_ATTRIBUTE,		MAX_INSTANT_FREEZE_DATA_LEN,		0,							},	//˲ʱ����	
	{0x5003,	eFREEZE_HOUR,				MAX_HOUR_FREEZE_ATTRIBUTE,			MAX_HOUR_FREEZE_DATA_LEN,			0,							},	//Сʱ����
	{0x5006,	eFREEZE_MON,				MAX_MON_FREEZE_ATTRIBUTE,			MAX_MON_FREEZE_DATA_LEN,			0,							},	//�¶���	
	{0x5004,	eFREEZE_DAY,				MAX_DAY_FREEZE_ATTRIBUTE,			MAX_DAY_FREEZE_DATA_LEN,			MAX_DAY_FREEZE_E2_DEPTH,	},	//�ն���	
	{0x5002,	eFREEZE_MIN,				MAX_MINUTE_FREEZE_ATTRIBUTE,		MAX_MIN_FREEZE_DATA_LEN,			0,							}	//���Ӷ���
};

static	DWORD	FreezeFlag;			// �����־�ϵ���

static __no_init WORD	wInstantFreezeTimer;// ˲ʱ������ʱʱ��(��)
static __no_init WORD	wInstantFreezeTimerBak;// ˲ʱ������ʱʱ��(��) ȡ������ΪЧ�� �ϵ���
__no_init TFreezeDataInfo  MinInfo[MAX_FREEZE_PLANNUM];	// ���Ӷ��������Ϣ
static __no_init TRealTimer	FreezePowerDownTimeBak;	//�ϵ粹����־����ֹ�쳣��λ�ظ�����
static DWORD PowerUpFreezeMin;//�ϵ粹��ʱ����
static __no_init TCompleteFreezeTime	CompleteFreezeTime;	//�ϴβ�����ʱ��
static const BYTE bConstTempBuf[OI4800_04_SIZE]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static WORD BinSearchByTime( TBinarySchInfo *pBinSchInfo );
void SaveFlashMemData(DWORD dwRecordNo, BYTE Type, BYTE* pBuf);
void InitFlashMem(void);
BOOL RepairFreezeUseFlash(void);
BOOL RefreshFreezeUseFlash( DWORD dwRecordNo );

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��ȡ���ݴ洢�������Ϣ
//
//����: 
// 			Len[in]:	�Է��Ӷ���--4�ֽ�ʱ��+OAD�ĳ��ȣ���������--������¼�ĳ��ȣ�����ʱ�꣩
// 			RecordDepth[in]:	��OAD�����
// 			inRecordNo[in]:	��OAD���ݵ�RecordNo
// 			pSectorInfo[out]:ָ��SectorInfo��ָ��
//����ֵ:
//	��
//
//��ע: 
//-----------------------------------------------
static BOOL GetSectorInfo( WORD Len, WORD RecordDepth, DWORD inRecordNo, TSectorInfo *pSectorInfo )
{
	WORD twSectorNo;			// ��OADռ�õ���������	
	WORD twOadNoPerSector;		// һ�������ܴ�ŵ�OAD������
	WORD twSectorOff;			// ������ƫ��
	WORD twInSectorOff;			// �����ڵ�ƫ��
	
	if( (Len<4) || (RecordDepth==0) ||(Len > 2000) )
	{
		return FALSE;
	}
	else
	{

		// һ���������OAD�ĸ���
		twOadNoPerSector = SECTOR_SIZE/Len;
		if( RecordDepth < twOadNoPerSector )
		{
			twOadNoPerSector = RecordDepth;
		}	
		// ��OADռ��������
		if( (RecordDepth%twOadNoPerSector) == 0 )
		{
			twSectorNo = (RecordDepth/twOadNoPerSector)+1;
		}
		else
		{
			twSectorNo = ((RecordDepth/twOadNoPerSector)+1)+1;	// ��λ��1
		}
		// ������ƫ��
		twSectorOff = inRecordNo/twOadNoPerSector;
		twSectorOff %= twSectorNo;
		// �����ڵ�ƫ��
		twInSectorOff = inRecordNo%twOadNoPerSector;

		pSectorInfo->wPointer = twSectorOff*twOadNoPerSector+twInSectorOff;
		pSectorInfo->dwSaveSpace = SECTOR_SIZE*twSectorNo;
		pSectorInfo->dwAddr = (DWORD)twSectorOff*SECTOR_SIZE+twInSectorOff*Len;	

		return TRUE;
	}
}

//-----------------------------------------------
//��������:     ���ö����־
//
//����: 
//			FreezeNo[in]	����ö�ٺ�
//                    
//����ֵ:  	TRUE:���óɹ�	 FALSE:����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL SetFreezeFlag(BYTE FreezeNo )
{
    if( FreezeNo > eFREEZE_MAX_NUM )
    {
        return FALSE;
    }

    FreezeFlag |= (0x0001<<FreezeNo);
    
    return TRUE;
}

//-----------------------------------------------
//��������:     ���㶳���־
//
//����: 
//			FreezeNo[in]	����ö�ٺ�
//                    
//����ֵ:  	TRUE:���óɹ�	 FALSE:����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL ClrFreezeFlag(BYTE FreezeNo )
{
    if( FreezeNo > eFREEZE_MAX_NUM )
    {
        return FALSE;
    }
    
    FreezeFlag &= ~(0x0001<<FreezeNo);
    
    return TRUE;
}



//-----------------------------------------------
//��������: ��ʼ�����Ӷ�����Ϣ
//
//����:   ��
//                    
//����ֵ: ��
//			
//��ע:
//-----------------------------------------------
static void InitMinFreezeInfo( void )
{
	BYTE i;

	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		MinInfo[i].RecordNo = 0;
		MinInfo[i].SaveDot = 0;
		MinInfo[i].CRC32 = lib_CheckCRC32((BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo)-sizeof(DWORD));

		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] );
	}

	#if (SLE_THIRD_MEM_BACKUP == YES)
	//��ʼ�����ᱸ��
	InitFlashMem();
	#endif
}
//-----------------------------------------------
//��������: �����Ӷ���ָ�������
//
//����:   ��
//                    
//����ֵ: ��
//			
//��ע:
//-----------------------------------------------
static BOOL CheckMinFreezeInfo( void )
{
	BYTE i,Result;
	WORD MinUpTimeFlag;
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;

	MinUpTimeFlag=0;
	Result = TRUE;
	
	//���MinInfo��ȷ��
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		// ���RAM��У��
		if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == TRUE )
		{
			continue;
		}
		MinUpTimeFlag |= (0x0001<<i);
	
		// ��EEP�ָ�
		if(api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[i] ), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i] )== TRUE )
		{
			continue;
		}
		
		Result = FALSE;

		memset( (BYTE *)&MinInfo[i], 0x00, sizeof(TFreezeDataInfo) );
		MinInfo[i].CRC32 = lib_CheckCRC32((BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo)-sizeof(DWORD));
	}
	
	if(MinUpTimeFlag != 0)
	{
		//��������ʱ���־
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
		FreezeUpdateTimeFlag.wMinFlag |= MinUpTimeFlag;
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
	}

	return Result;
}
//-----------------------------------------------
//��������: ������Ӷ���ָ�������
//
//����: 	��
//	
//����ֵ:	��
//
//��ע:	 ÿ1Сʱ����һ�Σ����籣��һ��
//-----------------------------------------------
void SaveMinFreezePoint( void )
{

	BYTE i;
	BYTE Buf[sizeof(MinInfo)+sizeof(DWORD)];
	//�����Ӷ���ָ�������
	CheckMinFreezeInfo();
	
	memcpy(Buf,&MinInfo[0],sizeof(MinInfo));
	// дEEP
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[0] ), sizeof(MinInfo)+sizeof(DWORD), Buf );
	
}

//-----------------------------------------------
//��������: ��ȡOI��Ӧ�Ķ���ö������
//
//����: 
//			OI[in]:	����OI
//                    
//����ֵ:	OI��Ӧ��ö������
//			
//��ע:
//-----------------------------------------------
BYTE api_GetFreezeIndex(WORD OI)
{
	BYTE i;
	
	for(i=0;i<sizeof(FreezeInfoTab)/sizeof(TFreezeInfoTab);i++)
	{
		if( OI == FreezeInfoTab[i].OI )
		{
			break;
		}
	}
	
	if( i == sizeof(FreezeInfoTab)/sizeof(TFreezeInfoTab) )
	{
		ASSERT(0,0);
	}
	
	return i;
}


//-----------------------------------------------
//��������: ��ȡType��Ӧ�����Ե�ַ�����ݵ�ַ��������Ը�����������ݳ���
//
//����: 
//			inFreezeIndex[in]:		�������� eFreezeType
//			pFreezeAddrLen[out]:    wAllInfoAddr
//									wDataInfoEeAddr
//									dwAttOadEeAddr
//									dwAttCycleDepthEeAddr	
//									dwDataAddr          
//����ֵ:	TRUE/FALSE
//			
//��ע:
//-----------------------------------------------
BOOL GetFreezeAddrInfo( BYTE inFreezeIndex, TFreezeAddrLen *pFreezeAddrLen )
{
	BYTE i,FreezeIndex;
		 
	FreezeIndex = inFreezeIndex;

	if( FreezeIndex > eFREEZE_MAX_NUM )
	{
		ASSERT(0,0);
	}
	
	if( FreezeIndex >= eFREEZE_MAX_NUM )
	{
		FreezeIndex = eFREEZE_CLOSING;
		pFreezeAddrLen->wAllInfoAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeAllInfoRom[eFREEZE_CLOSING] );
	    pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Closing );
		pFreezeAddrLen->dwAttCycleDepthEeAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttCycleDepthRom.Freeze[FreezeIndex] );
		pFreezeAddrLen->wDataInfoEeAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.DataInfo[FreezeIndex] );
		
	
		pFreezeAddrLen->dwDataAddr = FLASH_FREEZE_BASE+(DWORD)FreezeIndex*FLASH_SECTOR_NUM*SECTOR_SIZE;

		
        return FALSE;
	}

	pFreezeAddrLen->wAllInfoAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeAllInfoRom[FreezeIndex] );
	
	if( FreezeIndex == eFREEZE_INSTANT )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Instant );	
	}
	else if( FreezeIndex == eFREEZE_MIN )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Min );
	}
	else if( FreezeIndex == eFREEZE_HOUR )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Hour );
	}
	else if( FreezeIndex == eFREEZE_DAY )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Day );
	
	}
	else if( FreezeIndex == eFREEZE_MON )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Mon );
	}
	else if( FreezeIndex == eFREEZE_TIME_ZONE_CHG )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.TimeZone );
	}
	else if( FreezeIndex == eFREEZE_DAY_TIMETABLE_CHG )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.DayTimeTab );
	}
	#if( PREPAY_MODE == PREPAY_LOCAL )
	else if( FreezeIndex == eFREEZE_TARIFF_RATE_CHG )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.TariffRateChg );	
	}
	else if( FreezeIndex == eFREEZE_LADDER_CHG )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.LadderChg );
	}
	else if( FreezeIndex == eFREEZE_LADDER_CLOSING )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.LadderClosing );	
	}
	#endif
	else if( FreezeIndex == eFREEZE_CLOSING )
	{
		pFreezeAddrLen->dwAttOadEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttOadRom.Closing );
	
	}
	else
	{
		ASSERT(0,0);
		return FALSE;
	}

	if( FreezeIndex == eFREEZE_MIN )
	{
		pFreezeAddrLen->dwAttCycleDepthEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttCycleDepthRom.Min[0] );
		pFreezeAddrLen->wDataInfoEeAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[0] );

	}
	else
	{
		pFreezeAddrLen->dwAttCycleDepthEeAddr =  GET_SAFE_SPACE_ADDR( FreezeSafeRom.AttCycleDepthRom.Freeze[FreezeIndex] );
		pFreezeAddrLen->wDataInfoEeAddr = GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.DataInfo[FreezeIndex] );

	}
	pFreezeAddrLen->dwDataAddr = FLASH_FREEZE_BASE; 
	for(i=0;i<FreezeIndex;i++)
	{
		pFreezeAddrLen->dwDataAddr += FreezeInfoTab[i].dwDataLen;
	}


	return TRUE;
}
//--------------------------------------------------
//��������:�õ���ȣ����¼����С
//
//����: Len[in]  ÿһ�����ݳ���
//		Space[in/out]  �����Ĵ洢�ռ��С	
//
//����ֵ: ������ȣ� ������Len = 100�� Space = 8192��return 40
//		  �����������Դ洢80�������������40,����������֤�������Ч����Ϊ40
//
//��ע: ����ŵ�flash�У�����ռ��������������֤һ��������������
//		����һ�������е���������Ч����
//--------------------------------------------------
static DWORD GetDepthRecaluSpace(DWORD Len, DWORD *Size)
{
	TSectorInfo	SectorInfo;
	DWORD Depth;
	DWORD NumPerSector;
	DWORD Space = Size[0];

	if(Space < (2*SECTOR_SIZE))
	{
		Space = 2 * SECTOR_SIZE;
	}
	NumPerSector = SECTOR_SIZE / Len;
	Depth = ((Space / SECTOR_SIZE) - 1) * NumPerSector;

	GetSectorInfo(Len, Depth, Depth, &SectorInfo);
	Size[0] = SectorInfo.dwSaveSpace;

	return Depth;
}
//--------------------------------------------------
//��������:���Ӷ��ᱸ����Ϣ����
//
//����:		Addr[in]	���Ӷ����������� ��ʼ��ַ
//			MainMem[in] ���Ӷ������洢���� ���������Լ����
//			BakMem[out]	���Ӷ��ᱸ������ ���������Լ����
//			pFreezeMinInfo[in/out] 
//						DataBakAddr[out]  ���Ӷ��ᱸ����������������ʼ��ַ
//����ֵ:��
//
//��ע: 1�� �������� ��Ч��������ռ�ռ ������Ч�����ռ�ı���ӳ�䵽 ����������
//		2�� DealMinFreezeInfoֻҪ�����ش���DealMinBakFreezeInfo�����д�
//--------------------------------------------------
void DealMinBakFreezeInfo(DWORD Addr, TFreezeAttCycleDepth* MainMem, TFreezeAttCycleDepth* BakMem, TFreezeMinInfo* pFreezeMinInfo)
{
	DWORD PlanSaveSpace[MAX_FREEZE_PLANNUM];		//���洢�� ���������ڴ��С
	DWORD PlanBakSpace, PlanBakDepth, PlanBakOffset, PlanBakRemainSpace;
	DWORD AllPlanSumSpace;		
	BYTE i, ValidPlanNum;
	TSectorInfo	SectorInfo;	
    
	Addr = Addr + MAX_MIN_FREEZE_DATA_LEN;	//���ݿռ����ʼ��ַ

	//�������洢�� ÿ��������ռ�ռ�Ĵ�С���Լ�������Ч�����ܿռ��С
	ValidPlanNum = 0;
    AllPlanSumSpace = 0;
	for(i = 0; i < MAX_FREEZE_PLANNUM; i++)
	{
		if( MainMem[i].Depth == 0 )	//���洢�� i�����ķ���ռ�Ϊ0���÷���û��ʹ��
		{
			continue;
		}
		ValidPlanNum++;	//��Ч����
		GetSectorInfo(pFreezeMinInfo->AllAttDataLen[i], MainMem[i].Depth, MainMem[i].Depth, &SectorInfo);
		PlanSaveSpace[i] = SectorInfo.dwSaveSpace;
		AllPlanSumSpace += PlanSaveSpace[i];	//���洢�� ��Ч�����ڴ��
	}

	//���㱸���� ÿ��������ռ�ռ�Ĵ�С���Լ�������ַ
	memcpy(BakMem, MainMem, sizeof(TFreezeAttCycleDepth) * MAX_FREEZE_PLANNUM);
	PlanBakOffset = 0;
	PlanBakRemainSpace = MAX_MINBAK_FREEZE_DATA_LEN;
	for(i = 0; i < MAX_FREEZE_PLANNUM; i++)
	{
		if( MainMem[i].Depth == 0 )	//���洢�� i�����ķ���ռ�Ϊ0���÷���û��ʹ��
		{
			continue;
		}
		ValidPlanNum--;
		pFreezeMinInfo->DataBakAddr[i] = Addr + PlanBakOffset;

		//�������� ÿ������ռ ���з����ռ�ı����� �ڱ������������ֲ���
		PlanBakSpace = (DWORD)(MAX_MINBAK_FREEZE_DATA_LEN * ((float)PlanSaveSpace[i] / AllPlanSumSpace));
		PlanBakDepth = GetDepthRecaluSpace(pFreezeMinInfo->AllAttDataLen[i], &PlanBakSpace);
		
		//��֤ʣ��� �����ķ�����������������
		if(PlanBakRemainSpace  < (PlanBakSpace + 2*SECTOR_SIZE*ValidPlanNum))
		{
			PlanBakSpace = PlanBakRemainSpace - (2 * SECTOR_SIZE * ValidPlanNum);
			PlanBakDepth = GetDepthRecaluSpace(pFreezeMinInfo->AllAttDataLen[i], &PlanBakSpace);
		}

		//��ַ����ʱ���������Ϊ0��д����flashʱ���ж����Ϊ0ʱ������дflash
		if((pFreezeMinInfo->DataBakAddr[i] + PlanBakSpace) > (FLASH_FREEZE_END_ADDR))
		{
			PlanBakSpace = 0;
			PlanBakDepth = 0;
		}

		BakMem[i].Depth = PlanBakDepth;
		PlanBakOffset += PlanBakSpace;
		PlanBakRemainSpace -= PlanBakSpace;
	}
}
//-----------------------------------------------
//��������: ���Ӷ���洢��Ϣ����
//
//����: 
//			OADNum[in]:				���Ӷ���OAD���� 
//			pAttOad[in]:			�����������Ա�           
//  		pAttCycleDepth[in]: 	�������
//			MinAddr[in]:			�洢��ַ
//			pFreezeAllInfoRom[in]:	����������Ϣ                   
//                    
//����ֵ:		TRUE/FALSE
//
//��ע:   ���ɾ����������
//-----------------------------------------------
WORD DealMinFreezeInfo( BYTE OADNum, TFreezeAttOad*	pAttOad, TFreezeAttCycleDepth* pAttCycleDepth, DWORD MinAddr,TFreezeMinInfo* pFreezeMinInfo )
{
	BYTE i,PlanIndex;
	WORD wLen;
	DWORD AllSaveSpace;
	TSectorInfo	SectorInfo;

	memset( pFreezeMinInfo, 0x00, sizeof(TFreezeMinInfo) );//��շ��Ӷ����ַ��Ϣ
	pFreezeMinInfo->DataAddr[0] = MinAddr;//����0 �̶�Ϊ���Ӷ����ʼ��ַ
	AllSaveSpace = 0;

	for( i = 0; i < OADNum; i++ )
	{
		PlanIndex = CURR_PLAN( pAttOad[i].Oad );//��ȡ������
		wLen = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&pAttOad[i].Oad, 0 );
		if( wLen == 0x8000 )
		{
			return FALSE;
		}

		pAttOad[i].AttDataLen = wLen;
		
		if( pFreezeMinInfo->AllAttDataLen[PlanIndex] == 0 )//��һ�γ�����Ҫ��4-����ʱ��
		{
			pFreezeMinInfo->AllAttDataLen[PlanIndex] = 4;//���·����ܳ���
		}
		
		pAttOad[i].Offset = pFreezeMinInfo->AllAttDataLen[PlanIndex];//����ƫ�ƣ�ƫ�Ʋ�������ǰOAD�ĳ���
		pFreezeMinInfo->AllAttDataLen[PlanIndex] += wLen;//���·����ܳ���

	}
	for(i = 0; i < MAX_FREEZE_PLANNUM; i++)
	{
		if( pFreezeMinInfo->AllAttDataLen[i] != 0 )
		{
			//���Ӷ����������� 4�ֽ�ʱ�� + ���� + 4�ֽ�CRC
			//		   ������ 4�ֽ�ʱ�� + ���� + 4�ֽڼ�¼���
			//��������CRC���ԣ������ݡ����ݼ�¼������ļ�¼��Ų���ȣ����ݲ���
			pFreezeMinInfo->AllAttDataLen[i] += 4;//���·����ܳ���
		}
	}

	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		if( pAttCycleDepth[i].Depth == 0 )//����Ϊ0�����޴˷���
		{
			continue;
		}
		else
		{
			//��ȡ�����Ŀռ�
			if(GetSectorInfo( pFreezeMinInfo->AllAttDataLen[i], pAttCycleDepth[i].Depth, 0, &SectorInfo ) == FALSE)
			{
				return FALSE;
			}
		}
		
		if( i != 0 )//����0ֻ�жϿռ��Ƿ񹻾Ϳ��� ����Ҫ�����ַ
		{
			pFreezeMinInfo->DataAddr[i] = (pFreezeMinInfo->DataAddr[0]+AllSaveSpace);
		}

		AllSaveSpace += SectorInfo.dwSaveSpace;
		
		if( AllSaveSpace > FreezeInfoTab[eFREEZE_MIN].dwDataLen )//�жϵ�ǰ�����ռ��Ƿ���
		{
			return FALSE;
		}	
		
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��Ӷ��������������
//
//����: 
//			OI[in]:		0x5000~0x500B,0x5011 ������Ķ����ʶOI  
//			pOad[in]:	ָ��OAD Buffer��ָ��  2�ֽڶ�������+4�ֽ�OAD+2�ֽڴ洢���                    
//  		OadNum[in]: �������OAD�ĸ���
//                    
//����ֵ:	TRUE/FALSE
//
//��ע:   ���ɾ����������
//-----------------------------------------------
BOOL api_AddFreezeAttribue( WORD OI, BYTE *pOad ,BYTE OadNum )
{	
	BYTE i,j,AttNum,tFreezeIndex,PlanIndex;
	WORD tw,tCycle,tRecordDepth,wLen,Result;
	DWORD dwLen,dwOad;
	TSectorInfo	SectorInfo;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//��֤�ȴ洢��ee�е����ݴ��еط����CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth	MinBakAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TFreezeMinInfo FreezeMinInfo;
	
	memset( (BYTE *)&AttOad[0], 0x00, sizeof(AttOad) );
	memset( (BYTE *)&FreezeMinInfo, 0x00, sizeof(FreezeMinInfo) );

	tFreezeIndex = api_GetFreezeIndex( OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}
	
	// ��ø������Ե�ַ
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return FALSE;
	}

	//������������ĳ��ȼ��������Եĸ���
	if( api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo) != TRUE )
	{
		return FALSE;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return FALSE;
	}

	//�������еĹ������Զ���
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);
					
	//���Ƿ��Ӷ��ᣬҪ���·�OAD�����������ͬ����һ��
	if( tFreezeIndex == eFREEZE_MIN )	
	{
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
		}
	}
	else
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );
	}
	AttNum = FreezeAllInfo.NumofOad;
	
	//��Ҫ��ӵ�����OAD��������ж�
	for(i=0; i<OadNum; i++)
	{	
		//OAD
		memcpy( (BYTE *)&dwOad, (pOad+2+FREEZE_ATTRIBUTE_LEN*i), sizeof(DWORD) );
		//����
		memcpy( (BYTE *)&tCycle, pOad+i*FREEZE_ATTRIBUTE_LEN, 2);
		//���
		memcpy( (BYTE *)&tRecordDepth, pOad+6+i*FREEZE_ATTRIBUTE_LEN, 2);
		//���õĹ����������Ա��в������������Ϊ0
		//�����Զ��᲻��������Ϊ0
		if((tFreezeIndex > eFREEZE_HOUR) && (tCycle == 0x0000) )	
		{
			return FALSE;
		}
		//���õĹ����������Ա��в��������Ϊ0
		if(tRecordDepth == 0x0000)
		{
			return FALSE;
		}
		//ȡ���������Ӷ�������������0
		if(tFreezeIndex == eFREEZE_MIN)
		{
			PlanIndex = CURR_PLAN(dwOad);
		}
		else
		{
			PlanIndex =0;
		}
		//�жϴ˷�����������Ȳ�Ϊ��
		if((AttCycleDepth[PlanIndex].Cycle == 0x0000) && ((AttCycleDepth[PlanIndex].Depth == 0x0000)))
		{
			AttCycleDepth[PlanIndex].Cycle = tCycle;
			AttCycleDepth[PlanIndex].Depth = tRecordDepth;
		}
		else
		{
			//�����ͬ������������Ȳ�ͬ����FALSE
			if( (tCycle!=AttCycleDepth[PlanIndex].Cycle) || (tRecordDepth!=AttCycleDepth[PlanIndex].Depth) )	
			{
				return FALSE;
			}
		}
		//����Ƿ��к�֮ǰ��OAD�ظ���
		if( AttNum != 0 )
		{
			for( j=0; j<AttNum; j++ )
			{
				// ����������ͬ��OAD
				if( AttOad[j].Oad == dwOad )
				{
					break;
				}
			}
			
			if( j != AttNum )
			{
				continue;
			}	
		}			
		
		//�ж��·���OAD�Ƿ���ȷ
		wLen = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return FALSE;
		}
				
		//�����������ݵĳ���		
		AttOad[AttNum].Oad = dwOad;
								
		if( tFreezeIndex != eFREEZE_MIN )//���Ӷ��� �ռ䵥���ж�
		{
			AttOad[AttNum].AttDataLen = wLen;	
			
			if( AttNum == 0 )	
			{
				FreezeAllInfo.AllAttDataLen = 4+wLen+4;	//ʱ��+���ݳ��ȣ���4�ֽ�CRC����У��---��ا��200407
				AttOad[0].Offset = 4;					//ǰ����ʱ��	
			}
			else
			{
				//����һ������������ܳ��ȣ�����ʱ�꣩ ���Ӷ��᲻��
				FreezeAllInfo.AllAttDataLen += wLen;
				
				//������������������������ƫ�Ƶ�ַ
				AttOad[AttNum].Offset = AttOad[AttNum-1].Offset+AttOad[AttNum-1].AttDataLen;
			}
		}		

		AttNum++;
		
		if( AttNum > FreezeInfoTab[tFreezeIndex].MaxAttNum )
		{
			return FALSE;
		}
	}

	if( tFreezeIndex != eFREEZE_MIN )//���Ӷ����ڴ�֮ǰ�Ѿ��жϴ��泤��
	{
		if( FreezeAllInfo.AllAttDataLen > 4096 )//�����������д
		{
			return FALSE;
		}
		//�ն������ݳ�����E2�ռ��С������,���ȼ�4Ϊ��ȥEe�д洢�Ķ������кŵĳ���
		if((tFreezeIndex == eFREEZE_DAY) && (FreezeAllInfo.AllAttDataLen > (MAX_DAY_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
		{
			return FALSE;
		}
		//�����ն������ݳ�����E2�ռ��С������,���ȼ�4Ϊ��ȥEe�д洢�Ķ������кŵĳ���
		if((tFreezeIndex == eFREEZE_CLOSING) && (FreezeAllInfo.AllAttDataLen > (MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
		{
			return FALSE;
		}
		if(GetSectorInfo( FreezeAllInfo.AllAttDataLen, AttCycleDepth[0].Depth, AttCycleDepth[0].Depth, &SectorInfo ) == FALSE )
		{
			return FALSE;  
		}

        if( SectorInfo.dwSaveSpace > FreezeInfoTab[tFreezeIndex].dwDataLen )
        {
            ASSERT(0,1);
            return FALSE;           
        }
	}
	else
	{
		Result = DealMinFreezeInfo( AttNum, (TFreezeAttOad*)&AttOad, (TFreezeAttCycleDepth*)&AttCycleDepth, FreezeAddrLen.dwDataAddr, &FreezeMinInfo );
		if( Result == FALSE )
		{
			return FALSE;
		}
		//������Ӷ��ᱸ������ �洢��Ϣ���������Ĵ洢��ȣ��Լ����ݵ�ַ
		DealMinBakFreezeInfo(FreezeAddrLen.dwDataAddr, AttCycleDepth, MinBakAttCycleDepth, &FreezeMinInfo);
	}
	
	//���µ�OAD���
	if( AttNum > FreezeAllInfo.NumofOad )
	{				
		//�������Զ����OAD
		api_VWriteSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0] );	
		//��������ӵ����Զ���� ��� ����
		if( tFreezeIndex == eFREEZE_MIN )
		{
			for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
			{
				api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
				api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (MAX_FREEZE_PLANNUM + i), sizeof(TFreezeAttCycleDepth), (BYTE *)&MinBakAttCycleDepth[i] );
			}
			//���·��Ӷ�����Ϣ
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo );	
		}
		else if( FreezeAllInfo.NumofOad == 0 )
		{				
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );	
		}
			
		//���¶�������Ը���
		FreezeAllInfo.NumofOad = AttNum;
		//������������ĳ��ȼ��������Եĸ���
		api_VWriteSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
		
		//��ָ��
		if( tFreezeIndex == eFREEZE_MIN )
		{
			InitMinFreezeInfo();
		}
		else
		{
			api_ClrSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo) );			
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ɾ�����������������
//
//����: 
//			OI[in]:		0x5000~0x500B,0x5011 ������Ķ����ʶOI  
//			Type[in]:	eDELETE_ATT_BY_OAD--ɾ��ĳ������ 	eDELETE_ATT_TYPE--ɾ�����ж���  
//			pOad[in]:	Ҫɾ����OAD,���Ա�ʶ��������bit5~bit7������ඳ��Ķ��᷽��
//                    
//����ֵ:	TRUE/FALSE
//
//��ע:   ɾ��һ��OAD�����е����¼ƣ����·���EEP�ռ�
//-----------------------------------------------
BOOL api_DeleteFreezeAttribue( WORD OI, BYTE Type, BYTE *pOad )
{	
	BYTE i,j,tFreezeIndex,PlanIndex;
	WORD wLen,Result;
	DWORD dwOad;
	TSectorInfo	SectorInfo;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//��֤�ȴ洢��ee�е����ݴ��еط����CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth	MinBakAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TFreezeMinInfo FreezeMinInfo;

	memset( (BYTE *)&AttOad[0], 0x00, sizeof(AttOad) );
	memset( (BYTE *)&AttCycleDepth[0], 0x00, sizeof(AttCycleDepth) );
	memset( (BYTE *)&FreezeMinInfo, 0x00, sizeof(FreezeMinInfo) );
	
	tFreezeIndex = api_GetFreezeIndex( OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}

	// ��ø������Ե�ַ
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return FALSE;
	}
	
	//������������ĳ��ȼ��������Եĸ���
	if( api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo) != TRUE )
	{
		return FALSE;
	}
	
	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return FALSE;
	}

	if( Type == eDELETE_ATT_BY_OAD )
	{
		memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD) );

		//�������еĹ������Զ��� Oad
		api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);	
		//�������еĹ������Զ��� �������
		if( tFreezeIndex == eFREEZE_MIN )
		{
			for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
			{
				api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
			}
		}
		else
		{
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );
		}

		for( i=0; i<FreezeAllInfo.NumofOad; i++ )
		{
			// �ж�Ҫɾ���Ķ����ڶ�������Ƿ���ڣ�������ɾ������������Ķ���ǰ��
			if( AttOad[i].Oad == dwOad )
			{
				for( j=i; j<(FreezeAllInfo.NumofOad-1); j++ )
				{
					memcpy( (BYTE *)&AttOad[j], (BYTE *)&AttOad[j+1], sizeof(TFreezeAttOad) );	
				}
				
				memset( (BYTE *)&AttOad[FreezeAllInfo.NumofOad-1], 0x00, sizeof(TFreezeAttOad) );
				break;
			}
		}

		if( i == FreezeAllInfo.NumofOad )
		{
			return FALSE;
		}

		if( FreezeAllInfo.NumofOad == 0 )//�ڽ���FreezeAllInfo.NumofOad--֮ǰ���г����ж�-jwh
		{
            return FALSE;
		}
		
		FreezeAllInfo.NumofOad--;

		//������ÿ��OAD�ĵ�ַ
		for( i=0; i<FreezeAllInfo.NumofOad; i++ )
		{
			// һ�ֶ��᷽�����OAD����Ψһ��
			dwOad = AttOad[i].Oad;
			wLen = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )
			{
				return FALSE;
			}
			
			if( tFreezeIndex != eFREEZE_MIN )//���Ӷ��� ��������
			{
				AttOad[i].AttDataLen = wLen;
				if( i == 0 )
				{
					FreezeAllInfo.AllAttDataLen = 4+wLen+4; //ʱ��+���ݳ��ȣ���4�ֽ�CRC����У��---��ا��200407
					AttOad[i].Offset = 4;					//ǰ����ʱ��
				}
				else
				{						
					FreezeAllInfo.AllAttDataLen += wLen;
					AttOad[i].Offset = AttOad[i-1].Offset+AttOad[i-1].AttDataLen;
				}

			}				
		}
		
		if( tFreezeIndex == eFREEZE_MIN )
		{
			for( i=0; i<MAX_FREEZE_PLANNUM; i++ )
			{
				for( j=0; j<FreezeAllInfo.NumofOad; j++ )
				{
					PlanIndex = CURR_PLAN(AttOad[j].Oad);//ȡ�����������Եķ�����
					if(PlanIndex == i)
					{
						break;
					}
				}
				if(j == FreezeAllInfo.NumofOad)
				{
					memset( (BYTE *)&AttCycleDepth[i], 0x00, sizeof(TFreezeAttCycleDepth));
				}
			}
		}	
		
    	if( tFreezeIndex != eFREEZE_MIN )//���Ӷ����ڴ�֮ǰ�Ѿ��жϴ��泤��
    	{
			if( FreezeAllInfo.AllAttDataLen > 4096 )//�����������д
			{
				return FALSE;
			}
			//�ն������ݳ�����E2�ռ��С������,���ȼ�4Ϊ��ȥEe�д洢�Ķ������кŵĳ���
			if((tFreezeIndex == eFREEZE_DAY) && (FreezeAllInfo.AllAttDataLen > (MAX_DAY_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
			{
				return FALSE;
			}
			//�����ն������ݳ�����E2�ռ��С������,���ȼ�4Ϊ��ȥEe�д洢�Ķ������кŵĳ���
			if((tFreezeIndex == eFREEZE_CLOSING) && (FreezeAllInfo.AllAttDataLen > (MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
			{
				return FALSE;
			}
			if(GetSectorInfo( FreezeAllInfo.AllAttDataLen, AttCycleDepth[0].Depth, AttCycleDepth[0].Depth, &SectorInfo ) == FALSE )
			{
				return FALSE;
			}
			
			if( SectorInfo.dwSaveSpace > FreezeInfoTab[tFreezeIndex].dwDataLen )
			{
				ASSERT(0,1);
				return FALSE;			
			}
    	}
    	else
    	{
			Result = DealMinFreezeInfo( FreezeAllInfo.NumofOad, (TFreezeAttOad*)&AttOad, (TFreezeAttCycleDepth*)&AttCycleDepth, FreezeAddrLen.dwDataAddr, &FreezeMinInfo );
			if( Result == FALSE )
			{
				return FALSE;
			}
			//������Ӷ��ᱸ������ �洢��Ϣ���������Ĵ洢��ȣ��Լ����ݵ�ַ
			DealMinBakFreezeInfo(FreezeAddrLen.dwDataAddr, AttCycleDepth, MinBakAttCycleDepth, &FreezeMinInfo);
    	}
	}
	else if( Type == eDELETE_ATT_ALL )
	{		
		FreezeAllInfo.NumofOad = 0;
		FreezeAllInfo.AllAttDataLen = 0;
	}
	else
	{
		return FALSE;
	}
	
	//д��������������Ա�
	api_VWriteSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);
	
	api_VWriteSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
			
	if( tFreezeIndex == eFREEZE_MIN )
	{
		//���������������
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (MAX_FREEZE_PLANNUM + i), sizeof(TFreezeAttCycleDepth), (BYTE *)&MinBakAttCycleDepth[i] );
		}
		//���·��Ӷ�����Ϣ
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo );	
		//ȫ��ָ��	
		InitMinFreezeInfo();
	}
	else
	{		
		//���������������
		api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );	
		//ȫ��ָ��	
		api_ClrSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo) );	
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ���ö��������������
//
//����: 
//		OI[in]:		0x5000~0x500B,0x5011 ������Ķ����ʶOI  
//  	Type[in]:	0--��Լ����	0xFF--��ʼ�����ã���Ҫ��OAD���е���
//  	pOad[in]:  	ָ��OAD Buffer��ָ��  2�ֽڶ�������+4�ֽ�OAD+2�ֽڴ洢���               
//  	OadNum[in]:	OAD�ĸ���
//
//����ֵ:	TRUE/FALSE
//
//��ע:	��֧������ĳ�����ԡ����е����¼ƣ����·���EEP�ռ� 		
//-----------------------------------------------
BOOL api_WriteFreezeAttribue( WORD OI, BYTE Type, BYTE *pOad, WORD OadNum )
{	
	BYTE i,k,tFreezeIndex,PlanIndex;
	WORD tw,tCycle,tRecordDepth,wLen,Result;
	DWORD dwOad;	
	TSectorInfo	SectorInfo;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//��֤�ȴ洢��ee�е����ݴ��еط����CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth	MinBakAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;  
	TFreezeMinInfo FreezeMinInfo;

	tFreezeIndex = api_GetFreezeIndex( OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}
	
	// ��ø������Ե�ַ
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return FALSE;
	}
	
	if( (OadNum>FreezeInfoTab[tFreezeIndex].MaxAttNum) || (OadNum==0) )
	{
		return FALSE;
	}

	if( (Type!=0) && (Type!=0xff) )
	{
		return FALSE;
	}

	// ���е�OAD
	memset( (BYTE *)&AttOad[0], 0x00, sizeof(AttOad));
	memset( (BYTE *)&AttCycleDepth[0], 0x00, sizeof(AttCycleDepth));
	memset( (BYTE *)&FreezeAllInfo, 0x00, sizeof(FreezeAllInfo));
	memset( (BYTE *)&FreezeMinInfo, 0x00, sizeof(FreezeMinInfo) );
	
	for( i = 0; i < OadNum; i++ )
	{
		//OAD
		memcpy( (BYTE *)&AttOad[i].Oad, pOad+2+i*FREEZE_ATTRIBUTE_LEN, 4);
		if( Type == 0xFF )
		{
			lib_InverseData((BYTE *)&AttOad[i].Oad,sizeof(DWORD));
		}
		//����
		memcpy( (BYTE *)&tCycle, pOad+i*FREEZE_ATTRIBUTE_LEN, 2);
		//���
		memcpy( (BYTE *)&tRecordDepth, pOad+6+i*FREEZE_ATTRIBUTE_LEN, 2);
		//�����Զ��᲻��������Ϊ0
		if((tFreezeIndex > eFREEZE_HOUR) && (tCycle == 0x0000) )	
		{
			return FALSE;
		}
		//���õĹ����������Ա��в��������Ϊ0
		if(tRecordDepth == 0x0000)
		{
			return FALSE;
		}
		//ȡ���������Ӷ�������������0
		if(tFreezeIndex == eFREEZE_MIN)
		{
			PlanIndex = CURR_PLAN(AttOad[i].Oad);
		}
		else
		{
			PlanIndex =0;
		}
		//�жϴ˷�����������Ȳ�Ϊ��
		if((AttCycleDepth[PlanIndex].Cycle == 0x0000) && ((AttCycleDepth[PlanIndex].Depth == 0x0000)))
		{
			AttCycleDepth[PlanIndex].Cycle = tCycle;
			AttCycleDepth[PlanIndex].Depth = tRecordDepth;
		}
		else
		{
			//�����ͬ������������Ȳ�ͬ����FALSE
			if( (tCycle!=AttCycleDepth[PlanIndex].Cycle) || (tRecordDepth!=AttCycleDepth[PlanIndex].Depth) )	
			{
				return FALSE;
			}
		}
		dwOad = AttOad[i].Oad;
		wLen = api_GetProOADLen( eGetRecordData, eMaxData, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return FALSE;
		}

		// ����Addr
		if( i == 0 )
		{						
			if( tFreezeIndex != eFREEZE_MIN )
			{
				AttOad[i].AttDataLen = wLen;
				FreezeAllInfo.AllAttDataLen = 4+wLen+4;//ʱ��+����+4�ֽ�CRC--20200407--��ا��
				AttOad[i].Offset = 4;					
			}
		}
		else
		{
			// ����������ͬ��OAD
			for( k = 0; k < i; k++ )
			{
				if( AttOad[k].Oad == AttOad[i].Oad )
				{
					return FALSE;
				}
			}
			
			if( tFreezeIndex != eFREEZE_MIN )
			{
				AttOad[i].AttDataLen = wLen;
				FreezeAllInfo.AllAttDataLen += wLen;
				AttOad[i].Offset = AttOad[i-1].Offset+AttOad[i-1].AttDataLen;

			}			
		}
	}
	
	FreezeAllInfo.NumofOad = OadNum;

	if( tFreezeIndex != eFREEZE_MIN )//���Ӷ����ڴ�֮ǰ�Ѿ��жϴ��泤��
	{
		if( FreezeAllInfo.AllAttDataLen > 4096 )//�����������д
		{
			return FALSE;
		}
		//�ն������ݳ�����E2�ռ��С������,���ȼ�4Ϊ��ȥEe�д洢�Ķ������кŵĳ���
		if((tFreezeIndex == eFREEZE_DAY) && (FreezeAllInfo.AllAttDataLen > (MAX_DAY_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
        {
			return FALSE;
		}
		//�����ն������ݳ�����E2�ռ��С������,���ȼ�4Ϊ��ȥEe�д洢�Ķ������кŵĳ���
		if((tFreezeIndex == eFREEZE_CLOSING) && (FreezeAllInfo.AllAttDataLen > (MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN-sizeof(DWORD))))
		{
			return FALSE;
		}
		if(GetSectorInfo( FreezeAllInfo.AllAttDataLen, AttCycleDepth[0].Depth, AttCycleDepth[0].Depth, &SectorInfo ) == FALSE )
		{
			return FALSE;
		}

        if( SectorInfo.dwSaveSpace > FreezeInfoTab[tFreezeIndex].dwDataLen )
        {
            ASSERT(0,1);
            return FALSE;           
        }
	}
	else
	{
		Result = DealMinFreezeInfo( FreezeAllInfo.NumofOad, (TFreezeAttOad*)&AttOad, (TFreezeAttCycleDepth*)&AttCycleDepth, FreezeAddrLen.dwDataAddr, &FreezeMinInfo );
		if( Result == FALSE )
		{
			return FALSE;
		}
		//������Ӷ��ᱸ������ �洢��Ϣ���������Ĵ洢��ȣ��Լ����ݵ�ַ
		DealMinBakFreezeInfo(FreezeAddrLen.dwDataAddr, AttCycleDepth, MinBakAttCycleDepth, &FreezeMinInfo);
	}
	
	api_VWriteSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
	api_VWriteSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);
	
	if( tFreezeIndex == eFREEZE_MIN )
	{
		//���������������
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
			api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (MAX_FREEZE_PLANNUM + i), sizeof(TFreezeAttCycleDepth), (BYTE *)&MinBakAttCycleDepth[i] );
		}
		//���·��Ӷ�����Ϣ
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo );	
		//ȫ��ָ��	
		InitMinFreezeInfo();
	}
	else
	{		
		//���������������
		api_VWriteSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );	
		//ȫ��ָ��	
		api_ClrSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo) );	
	}

	return TRUE;
}

//-----------------------------------------------
//��������: �����������������
//
//����: 
//	OI[in]:				0x5000~0x500B,0x5011 ������Ķ����ʶOI                 
//  ElementIndex[in]:	�������Զ�����Ԫ���������ڼ�������                
//  Len[in]:			��Լ�㴫�����ݳ��ȣ������˳��ȷ���FALSE	
//  pBuf[out]: 			�������ݻ���    
//
//��ע: �������ݳ���( �ڸ���Ϊ0ʱ�����1�ֽڵ�0 ���س���Ϊ1)
//-----------------------------------------------
WORD api_ReadFreezeAttribute( WORD OI, BYTE ElementIndex, WORD Len, BYTE *pBuf )
{	
	BYTE i,tFreezeIndex,PlanIndex;
	BYTE TempBuf[FREEZE_ATTRIBUTE_LEN];
	WORD wLenTemp;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//��֤�ȴ洢��ee�е����ݴ��еط����CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
    TFreezeAllInfoRom FreezeAllInfo;

	wLenTemp = 0;
	
	tFreezeIndex = api_GetFreezeIndex( OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return 0x8000;
	}
	
	// ��ø������Ե�ַ
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	//������������ĳ��ȼ��������Եĸ���
	api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		FreezeAllInfo.NumofOad = 0;
	}
	
	//�������еĹ������Զ��� Oad
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);	
	//�������еĹ������Զ��� �������
	if( tFreezeIndex == eFREEZE_MIN )
	{
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
		}
	}
	else
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );
	}

	if( ElementIndex == 0 )
	{
		for( i = 0; i < FreezeAllInfo.NumofOad; i++ )
		{
			// �ж��Ƿ񳬳�������󳤶�
			if( (wLenTemp+FREEZE_ATTRIBUTE_LEN) > Len )
			{
				return 0;
			}
			memcpy( TempBuf+2, (BYTE *)&AttOad[i].Oad, 4 );
			if( tFreezeIndex == eFREEZE_MIN )
			{
				PlanIndex = CURR_PLAN(AttOad[i].Oad);//ȡ�����������Եķ�����
			}
			else
			{
				PlanIndex = 0;
			}
			memcpy( TempBuf, (BYTE *)&AttCycleDepth[PlanIndex].Cycle, 2 );			
			memcpy( TempBuf+2+4, (BYTE *)&AttCycleDepth[PlanIndex].Depth, 2 );
			memcpy( pBuf+wLenTemp, TempBuf, FREEZE_ATTRIBUTE_LEN );
			wLenTemp += FREEZE_ATTRIBUTE_LEN;
		}

		if( wLenTemp == 0 )
		{
			pBuf[0] = 0;
			return 1;
		}
		else
		{
			return wLenTemp;			
		}
	}
	else
	{	
		if( ElementIndex >  FreezeAllInfo.NumofOad )
		{
			pBuf[0] = 0;
			return 1;
		}
		memcpy( pBuf+2, (BYTE *)&AttOad[ElementIndex-1].Oad, 4 );
		if( tFreezeIndex == eFREEZE_MIN )
		{
			PlanIndex= CURR_PLAN(AttOad[ElementIndex-1].Oad);//ȡ�����������Եķ�����
		}
		else
		{
			PlanIndex = 0;
		}	
		memcpy( pBuf, (BYTE *)&AttCycleDepth[PlanIndex].Cycle, 2 );
		memcpy( pBuf+2+4, (BYTE *)&AttCycleDepth[PlanIndex].Depth, 2 );
		
		return FREEZE_ATTRIBUTE_LEN;
	}
}

//-----------------------------------------------
//��������: �ö����־λ
//
//����:
//	inFreezeIndex[in]:		��������
//	Delay[in]:	˲ʱ�������ʱ������������0
//
//����ֵ:	TRUE/FALSE
//
//��ע:   �л�����,˲ʱ������� ������eFREEZE_CLOSING���豣֤�ǵ�һ����������ԭ������ȷ����ı�
//-----------------------------------------------
BOOL api_SetFreezeFlag( eFreezeType inFreezeIndex, WORD Delay )
{
	BYTE tFreezeIndex;

	tFreezeIndex = inFreezeIndex;
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}

	if( tFreezeIndex == eFREEZE_INSTANT )
	{
		if( Delay == 0 )
		{
			SetFreezeFlag(tFreezeIndex);
		}
		else
		{
			wInstantFreezeTimer = Delay;
			wInstantFreezeTimerBak = (WORD)(~wInstantFreezeTimer);
		}
	}
	else if( 	(tFreezeIndex == eFREEZE_TIME_ZONE_CHG)
		#if( PREPAY_MODE == PREPAY_LOCAL )
			||	(tFreezeIndex == eFREEZE_TARIFF_RATE_CHG)
			||	(tFreezeIndex == eFREEZE_LADDER_CHG)
			||	(tFreezeIndex == eFREEZE_LADDER_CLOSING)
		#endif
			||	(tFreezeIndex == eFREEZE_DAY_TIMETABLE_CHG)
			||	(tFreezeIndex == eFREEZE_CLOSING) )
	{
		SetFreezeFlag(tFreezeIndex);
		if( tFreezeIndex == eFREEZE_CLOSING )
		{
		    SetFreezeFlag(eFREEZE_MAX_NUM);//��һ�����ձ�־��������
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------
//��������: ��ȡ���Ӷ��� ������������
//
//����:	pRecordInfo[in] ���Ӷ��ᱸ�����������Ϣ
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
BOOL ReadMinBakRecordData(TReadFreezeRecordInfo	*pRecordInfo, BYTE * pBuf)
{
	TSectorInfo	SectorInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAttCycleDepth AttCycleDepth;
	DWORD Addr;
	BOOL Ret = FALSE;

	//��� ���Ӷ��ᱸ������ �������
	GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen );
	Ret = api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (pRecordInfo->MinPlan + MAX_FREEZE_PLANNUM), sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth );
	if(Ret == FALSE)
	{
		return FALSE;
	}
	GetSectorInfo(pRecordInfo->RecordLen, AttCycleDepth.Depth, pRecordInfo->RecordNo + 1, &SectorInfo);
	Addr = pRecordInfo->dwBakAddr + SectorInfo.dwAddr;
	api_ReadMemRecordData(Addr, pRecordInfo->RecordLen, pBuf);

	//���Ӷ����������� 4�ֽ�ʱ�� +  ���� + 4�ֽ�CRC
	//		   ������ 4�ֽ�ʱ�� +  ���� + 4�ֽڼ�¼��
	memcpy(&Addr, pBuf + pRecordInfo->RecordLen - sizeof(DWORD), sizeof(DWORD));
	Ret = FALSE;
	if(Addr == pRecordInfo->RecordNo)
	{
		Ret = TRUE;
	}

	return Ret;
}
//-----------------------------------------------
//��������: �������¼����
//
//����: 
//			pData[in]	������Ϣ
//			pBuf[out]	������ʱ�Ļ���
//			
//			
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:  ���ô˺�����ȡ�������ݵ���������,
//      �˺�����ȡ�������������б������������ݴ�������»���ñ������ݣ�  
//-----------------------------------------------
BOOL ReadFreezeRecordData(TReadFreezeRecordInfo	*pRecordInfo, BYTE * pBuf)
{
	DWORD dwE2DataAddr;
	DWORD dwDataNo;//E2�ڴ洢�����к�
	BOOL Ret = TRUE;

	if( pRecordInfo->FreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}
	if( pRecordInfo->RecordLen > 2000 )
	{
		return FALSE;
	}
	api_ReadMemRecordData(pRecordInfo->dwAddr,pRecordInfo->RecordLen,pBuf);
	//ֻ���ն��ᣬ�����ն�����CRCУ����������²Ŵ�Ee�ж�ȡ
	if(( (pRecordInfo->FreezeIndex == eFREEZE_DAY) ||(pRecordInfo->FreezeIndex == eFREEZE_CLOSING) )
	&&(lib_CheckSafeMemVerify(pBuf, pRecordInfo->RecordLen, UN_REPAIR_CRC) != TRUE))
	{
		//��¼�쳣�¼���¼
		api_WriteSysUNMsg(GET_FREEZE_RECORD_ERR);
		if(pRecordInfo->FreezeIndex == eFREEZE_DAY)
		{
			//�ն���E2�������ݵĻ���ַ
			dwE2DataAddr = 	GET_CONTINUE_ADDR( FreezeConRom.EeData.Day[0] );
		}
		else
		{
			//�����ն���E2�������ݵĻ���ַ
			dwE2DataAddr = 	GET_CONTINUE_ADDR( FreezeConRom.EeData.Closing[0] );
		}
		dwE2DataAddr += (DWORD)(pRecordInfo->RecordLen+sizeof(DWORD)) * (pRecordInfo->RecordNo% FreezeInfoTab[pRecordInfo->FreezeIndex].wEeDepth);

		//��ȡ���кţ��ж�Ee�������Ƿ�Ϊ����ȡ���ݣ����ǲ���ȡEe����
		api_ReadFromContinueEEPRom(dwE2DataAddr,sizeof(DWORD), (BYTE*)&dwDataNo);
		Ret = FALSE;
		if(dwDataNo == pRecordInfo->RecordNo )
		{
			dwE2DataAddr += sizeof(DWORD);
			api_ReadFromContinueEEPRom(dwE2DataAddr,pRecordInfo->RecordLen, pBuf);

			Ret = lib_CheckSafeMemVerify(pBuf, pRecordInfo->RecordLen, UN_REPAIR_CRC);
		}
	}
	else if( pRecordInfo->FreezeIndex == eFREEZE_MIN )
	{
		Ret = lib_CheckSafeMemVerify(pBuf, pRecordInfo->RecordLen, UN_REPAIR_CRC);
		if( Ret != TRUE)
		{
			Ret = ReadMinBakRecordData(pRecordInfo, pBuf);
		}
	}
	 
	return Ret;
}
//-----------------------------------------------
//��������: �������¼ʱ��
//
//����: 
//			pData[in]	������Ϣ
//			pBuf[out]	������ʱ�Ļ���
//			
//			
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:  ���ô˺�����ȡ�������ݵ�ʱ��,
//      �˺�����ȡ�������������б������������ݴ�������»���ñ������ݣ�  
//-----------------------------------------------
BOOL ReadFreezeRecordTime(TReadFreezeRecordInfo	*pRecordInfo, DWORD* Time)
{
	BOOL Flag = FALSE;
	BYTE  Buf[2030];//���벻��������
	
	if(pRecordInfo->RecordLen > 2000)
	{
		return Flag;
	}

	//������������
	if(ReadFreezeRecordData( pRecordInfo, Buf ) == TRUE)
	{
		Flag = TRUE;
	}
	memcpy( (BYTE *)Time, Buf, sizeof(DWORD) );
	
	return Flag;
	
}
//-----------------------------------------------
//��������: ��E2�����¼����
//
//����: 
//			pData[in]	������Ϣ
//			pBuf[out]	������ʱ�Ļ���
//			                  
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע: ��������������ʹ��  
//-----------------------------------------------
#if( MAX_PHASE == 1 )
BOOL ReadFreezeRecordEeData(TReadFreezeRecordInfo	*pRecordInfo, BYTE * pBuf)
{
	DWORD dwE2DataAddr;
	DWORD dwDataNo;//E2�ڴ洢�����к�
	if( pRecordInfo->FreezeIndex >= eFREEZE_MAX_NUM )
	{
        return FALSE;
	}
	if( pRecordInfo->RecordLen > 2000 )
	{
		return FALSE;
	}
	//ֻ���ն��ᣬ�����ն�����CRCУ����������²Ŵ�Ee�ж�ȡ
	if( (pRecordInfo->FreezeIndex == eFREEZE_DAY) ||(pRecordInfo->FreezeIndex == eFREEZE_CLOSING) )
	{
		if(pRecordInfo->FreezeIndex == eFREEZE_DAY)
		{
			//�ն���E2�������ݵĻ���ַ
			dwE2DataAddr = 	GET_CONTINUE_ADDR( FreezeConRom.EeData.Day[0] );
		}
		else
		{
			//�����ն���E2�������ݵĻ���ַ
			dwE2DataAddr = 	GET_CONTINUE_ADDR( FreezeConRom.EeData.Closing[0] );
		}
		dwE2DataAddr += (DWORD)(pRecordInfo->RecordLen+sizeof(DWORD)) * (pRecordInfo->RecordNo% FreezeInfoTab[pRecordInfo->FreezeIndex].wEeDepth);

		//��ȡ���кţ��ж�Ee�������Ƿ�Ϊ����ȡ���ݣ����ǲ���ȡEe����
		api_ReadFromContinueEEPRom(dwE2DataAddr,sizeof(DWORD), (BYTE*)&dwDataNo);
		if(dwDataNo == pRecordInfo->RecordNo )
		{
			dwE2DataAddr += sizeof(DWORD);
			api_ReadFromContinueEEPRom(dwE2DataAddr,pRecordInfo->RecordLen, pBuf);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}
#endif
//-----------------------------------------------
//��������: д����������,����ַУ��
//
//����: 
//			Addr[in]		Ҫд����ʼ��ַ
//			Length[in]		Ҫд�ĳ���
//			BaseAddr[in]	������¼�����Ļ���ַ
//			MemSpace[in]	������¼�������Ŀռ�
//			pBuf[in/out]	д����ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:�˺���ֻдFlash�¼�������
//-----------------------------------------------
BOOL WriteFreezeRecordData(DWORD Addr, WORD Length, DWORD BaseAddr, DWORD MemSpace, BYTE * pBuf)
{
	if((Addr >= BaseAddr) && ((Addr+Length) <= (BaseAddr+MemSpace)))
	{
		return api_WriteMemRecordData( Addr, Length, pBuf );
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
//��������: ������д�����¼�������
//
//����: 
//			Addr[in]		Ҫд����ʼ��ַ
//			Length[in]		Ҫд�ĳ���
//			pBuf[in/out]	д����ʱ�Ļ���
//                    
//����ֵ:  	TRUE--�����ɹ� FALSE--����ʧ��
//
//��ע:�˺���ֻ����������������--��Ϊ��������pBufָ��4K-Buf,��Ȼ�г��ڴ�����
//-----------------------------------------------
BOOL WriteFreezeRecordData_EraseFlash(DWORD Addr, WORD Length, BYTE * pBuf)
{
	DWORD dwSectorAddr;
	WORD Len;
	
	if( Length == 0)
	{
		dwSectorAddr = (Addr / SECTOR_SIZE) * SECTOR_SIZE;//д�ı���������ַ
		
		//����������ǰ������ݣ��ٲ�����ҳ�����������д��
		api_ReadMemRecordData( dwSectorAddr, Addr - dwSectorAddr, pBuf );
		
		Len = Addr - dwSectorAddr;//�������峤��
		return api_WriteMemRecordData( dwSectorAddr, Len, pBuf );
	}
	else
	{

		if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
		{
			return FALSE;
		}
		dwSectorAddr = (Addr / SECTOR_SIZE) * SECTOR_SIZE;//д�ı���������ַ
	
		//�Ȱѱ������ݷ���4k�ڴ�ĺ��棬�����ζ�������ݷ��ں���
		memmove( pBuf + (Addr - dwSectorAddr), pBuf, Length );
		//����������ǰ������ݣ��ٲ�����ҳ�����������д��
		api_ReadMemRecordData( dwSectorAddr, Addr - dwSectorAddr, pBuf );
		
		Len = Length + Addr - dwSectorAddr;//�������峤��
		if(Len > SECTOR_SIZE)
		{
			return FALSE;
		}
		//����ʱ��Ĵ�������ʼ��ַ��ʼд
		return api_WriteMemRecordData( dwSectorAddr, Len, pBuf );

	
	}
	
}
//--------------------------------------------------
//��������: ��¼��ŵ��˴���
//
//����:		No[in] ��Ҫ���˵������к�
//   	    Depth[in]  �������
//			Len[in] һ�������ݵĳ���
//			BaseAddr ��������Ļ���ַ�����Ӷ���Ϊÿ����������ʼ��ַ�� 
//			Buf[in] 
//����ֵ: ��
//
//��ע:No = 1ʱ��ֻ�ᱣ����1����¼(��������һ����¼)
//	   No = 2ʱ��ֻ�ᱣ����1,2����¼(������ǰ2����¼)
//--------------------------------------------------
void ProcFreezeNoRollback(DWORD No, DWORD Depth, DWORD Len, DWORD BaseAddr, BYTE *Buf)
{
	TSectorInfo	SectorInfo;
	DWORD Addr;

	if(Depth == 0)
	{
		return;
	}

	GetSectorInfo(Len, Depth, No, &SectorInfo);
	Addr = BaseAddr + SectorInfo.dwAddr;

 

	if( (Addr % SECTOR_SIZE) != 0)//��ַ����������ʼ��ַ
	{
		WriteFreezeRecordData_EraseFlash(Addr, 0, Buf);
	}
	else
	{
		//No��Ӧ�ĵ�ַ �պ�Ϊ������ʼ��ַʱ������Ҫ���⴦��дflash���Զ�������������
	}
}


//-----------------------------------------------
//��������: �嶳������
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void api_ClrFreezeData( void )
{
	BYTE i;
	TFreezeAddrLen FreezeAddrLen;
	
	for( i=0; i<eFREEZE_MAX_NUM; i++ )
	{
		// ��ø��ֶ����ַ
		GetFreezeAddrInfo( i, &FreezeAddrLen );
		
		api_ClrSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo) );	
	}
	
	//�����ָ��
	InitMinFreezeInfo();
}

//-----------------------------------------------
//��������: ��ȡ������ʱ��
//
//����: 	pBillingPara[in]: �������
//                    
//����ֵ:	��
//
//��ע:   	�Ѿ��ź���
//-----------------------------------------------
static BOOL GetClosingTime( TBillingPara *pBillingPara )
{
	BYTE i,j;
	//����ת�����
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ), sizeof(TBillingPara),(BYTE *)pBillingPara  ) != TRUE )
	{
		return FALSE;
	}

	//��飨����ΪHex��
	for( i = 0; i < MAX_MON_CLOSING_NUM; i++ )
	{
		if( (pBillingPara->MonSavePara[i].Day > MAX_VALID_DAY) || (pBillingPara->MonSavePara[i].Day == 0) || (pBillingPara->MonSavePara[i].Hour > MAX_VALID_HOUR) )
		{
			pBillingPara->MonSavePara[i].Day = 0xFF;
			pBillingPara->MonSavePara[i].Hour = 0xFF;
			continue;
		}
		for(j = 0;j < i;j++)
		{
			if((pBillingPara->MonSavePara[i].Day == pBillingPara->MonSavePara[j].Day) && (pBillingPara->MonSavePara[i].Hour == pBillingPara->MonSavePara[j].Hour))
			{
				pBillingPara->MonSavePara[i].Day = 0xFF;
				pBillingPara->MonSavePara[i].Hour = 0xFF;
			}
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ������ת����
//
//����:		��
//                    
//����ֵ:	��
//
//��ע:   	Сʱ�仯����
//-----------------------------------------------
static void CheckClosing( void )
{
	BYTE i,Day,Hour;
	TBillingPara Para;	

	if( GetClosingTime(&Para) == FALSE )
	{
		return;
	}

	api_GetRtcDateTime( DATETIME_DD, &Day );
	api_GetRtcDateTime( DATETIME_hh, &Hour );

	for( i = 0; i < MAX_MON_CLOSING_NUM; i++ )
	{
		if( (Para.MonSavePara[i].Day==Day) && (Para.MonSavePara[i].Hour==Hour) )
		{
			api_SwapEnergy();							// ת�����

			if( i == 0 )
			{
				SetFreezeFlag(eFREEZE_MAX_NUM);	//��һ�����ձ�־��������
			}

			SetFreezeFlag(eFREEZE_CLOSING);
			break;
		}
	}
}

//-----------------------------------------------
//��������: ���¶����ж��õ�ʱ��,������RecordNo
//
//����: 	InOldRelativeMin[in]:�޸�ʱ��֮ǰ�����ʱ��
//                    
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
void api_UpdateFreezeTime( DWORD InOldRelativeMin )
{
	BYTE i,PlanIndex;
	WORD Result;
	DWORD dwTemp,dwStartTime;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//��֤�ȴ洢��ee�е����ݴ��еط����CRC
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeDataInfo	DataInfo,SearchDataInfo[MAX_FREEZE_PLANNUM];
	TBinarySchInfo BinarySchInfo;
	TFreezeAllInfoRom FreezeAllInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;
	TFreezeMinInfo FreezeMinInfo;

	if( g_RelativeMin >= InOldRelativeMin )
	{
		return;
	}
	//�����Ӷ�����Ϣ
	Result = CheckMinFreezeInfo();

	#if( SLE_THIRD_MEM_BACKUP == YES )
	//�ϵ練дRTCʱ����鲢ˢ�¶����¼��ţ����Ӷ���ָ�
	if(( Result == FALSE )&&( InOldRelativeMin == 0xFFFFFFFF ))
	{
		RepairFreezeUseFlash();
	}
	#endif

	//�����޸�ʱ����Ӷ����־
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
	if( Result == FALSE )
	{
		memset( (BYTE*)&FreezeUpdateTimeFlag, 0x00, sizeof(TFreezeUpdateTimeFlag) );
	}
	
	//�ȴ���ʱ���²���������������
	for(i=eFREEZE_HOUR; i<eFREEZE_MIN; i++)
	{
		// ��ø������Ե�ַ
		GetFreezeAddrInfo( i, &FreezeAddrLen );		
		//�������еĹ������Զ���oad
		api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[i].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);
		//�����������Զ��� �������
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[0] );
		//����������RecordNo
		api_VReadSafeMem(FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo);
		//������������ĳ��ȼ��������Եĸ���
		api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
		
		if( FreezeAllInfo.AllAttDataLen > 2000 )
		{
			continue;
		}
		
		dwTemp = DataInfo.RecordNo;
		
		//��д������Ϣ
		BinarySchInfo.dwBaseAddr = FreezeAddrLen.dwDataAddr;	
		BinarySchInfo.dwDepth = AttCycleDepth[0].Depth;	
		BinarySchInfo.dwSaveDot = DataInfo.SaveDot;
		BinarySchInfo.dwRecordNo = DataInfo.RecordNo;		
		BinarySchInfo.Time = g_RelativeMin+1;//����Ҫ�ѵ�g_RelativeMinʱ�������ж���㣬�´��ٶ����©����ʱ�̵�����
		BinarySchInfo.pRecordNo = &DataInfo.RecordNo;
		BinarySchInfo.pTime = &dwStartTime;
		BinarySchInfo.wLen = FreezeAllInfo.AllAttDataLen;
		BinarySchInfo.FreezeIndex = i;
		if( BinSearchByTime(&BinarySchInfo) == TRUE )
		{			
			//����֮ǰ��RecordNo�����ѵ�RecordNo�����µ� SaveDot
			if(dwTemp > DataInfo.RecordNo)
			{
				dwTemp = dwTemp-DataInfo.RecordNo;
				if(DataInfo.SaveDot > dwTemp)
				{
					DataInfo.SaveDot = DataInfo.SaveDot-dwTemp;
					FreezeUpdateTimeFlag.wFreezeFlag |= (0x0001<<i);
				}				
				else
				{
					DataInfo.RecordNo = 0;
					DataInfo.SaveDot = 0;
				}
			}
			else if(dwTemp == DataInfo.RecordNo)
			{
				continue;
			}
			else
			{
				DataInfo.RecordNo = 0;
				DataInfo.SaveDot = 0;
			}
			api_VWriteSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo );
		}	

	}
	
	//������Ӷ���
	//��ø������Ե�ַ
    GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen );
	//��ȡ���Ӷ����ַ��Ϣ
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo ); 
	if( FreezeMinInfo.DataAddr[0] != FreezeAddrLen.dwDataAddr )//����0��ַ �����ַһ��
	{
		FreezeMinInfo.DataAddr[0] = FreezeAddrLen.dwDataAddr;
	}
	//������������ĳ��ȼ��������Եĸ���
	api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo);
	//�������еĹ������Զ���
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[eFREEZE_MIN].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);		
	//�������еĹ������Զ��� �������
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );
	}
	for(PlanIndex=0;PlanIndex<MAX_FREEZE_PLANNUM;PlanIndex++)
	{
		if( AttCycleDepth[PlanIndex].Depth == 0 )//0 ����˷����޶��� ����
		{
			continue;
		}
		
		dwTemp = MinInfo[PlanIndex].RecordNo;
		//��д������Ϣ
		BinarySchInfo.dwBaseAddr = FreezeMinInfo.DataAddr[PlanIndex];	
		BinarySchInfo.dwDepth = AttCycleDepth[PlanIndex].Depth;	
		BinarySchInfo.dwSaveDot = MinInfo[PlanIndex].SaveDot;
		BinarySchInfo.dwRecordNo = MinInfo[PlanIndex].RecordNo;		
		BinarySchInfo.Time = g_RelativeMin+1;//���õ�ʱ�仹���룬�������Ƿ�Ϊ0����������һ����
		BinarySchInfo.pRecordNo = &MinInfo[PlanIndex].RecordNo;
		BinarySchInfo.pTime = &dwStartTime;
		BinarySchInfo.wLen = FreezeMinInfo.AllAttDataLen[PlanIndex];
		BinarySchInfo.FreezeIndex = eFREEZE_MIN;
		BinarySchInfo.MinBakAddr = FreezeMinInfo.DataBakAddr[PlanIndex];
		BinarySchInfo.MinPlan = PlanIndex;
		if( BinSearchByTime(&BinarySchInfo) == TRUE )
		{			
			//����֮ǰ��RecordNo�����ѵ�RecordNo�����µ� SaveDot
			if(dwTemp > MinInfo[PlanIndex].RecordNo)
			{
				dwTemp = dwTemp-MinInfo[PlanIndex].RecordNo;
				if(MinInfo[PlanIndex].SaveDot > dwTemp)
				{
					MinInfo[PlanIndex].SaveDot = MinInfo[PlanIndex].SaveDot-dwTemp;
					FreezeUpdateTimeFlag.wMinFlag |= (0x01<<PlanIndex);
				}
				else
				{
					MinInfo[PlanIndex].RecordNo = 0;
					MinInfo[PlanIndex].SaveDot = 0;
				}
			}
			else if(dwTemp == MinInfo[PlanIndex].RecordNo)
			{
				continue;
			}
			else
			{
				MinInfo[PlanIndex].RecordNo = 0;
				MinInfo[PlanIndex].SaveDot = 0;
			}
			MinInfo[PlanIndex].CRC32 = lib_CheckCRC32( (BYTE*)&MinInfo[PlanIndex], sizeof(TFreezeDataInfo)-sizeof(DWORD) );
			api_VWriteSafeMem( FreezeAddrLen.wDataInfoEeAddr+PlanIndex*sizeof(TFreezeDataInfo), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[PlanIndex] );
		}	
		
	}
	
	if( (FreezeUpdateTimeFlag.wMinFlag!=0) || (FreezeUpdateTimeFlag.wFreezeFlag!=0) )
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
	}
}

//-----------------------------------------------
//��������: ��������
//
//����: 	PowerUpFlag[in]:������־��
//						0x55: ���� 0x00: ��������
//
//����ֵ:	��
//
//��ע:   ��ѭ�����������ã����Ӷ���ָ�룬��������Ϣ�������⴦������EEP
//-----------------------------------------------
void ProcFreezeTask(BYTE tFreezeIndex,BYTE  PowerUpFlag)
{
	BYTE i;
    WORD OI;
	DWORD dwAddr,dwBaseAddr;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];//��֤�ȴ洢��ee�е����ݴ��еط����CRC
	TSectorInfo	SectorInfo;
	TFreezeAttCycleDepth	AttCycleDepth;
	TFreezeDataInfo	DataInfo;
	TFreezeAddrLen FreezeAddrLen;
    TFreezeAllInfoRom FreezeAllInfo;
	BYTE FlashBuf[SECTOR_SIZE];
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;
	DWORD dwDataE2Addr,dwDataE2MaxAddr; //E2�ж������ݴ洢��ַ-�����ն��ᡢ�ն���,E2�������ݴ洢������ַ
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return;
	}
	// ��ø������Ե�ַ
    GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen );

	//������������ĳ��ȼ��������Եĸ���
	if( api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo) != TRUE )
	{
		return;
	}
	
	if( FreezeAllInfo.NumofOad == 0 )
	{
		return;
	}
	
	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return;
	}
	
	if( FreezeAllInfo.AllAttDataLen > 2000 )
	{
		return;
	}

	//�������еĹ������Զ���
	if(api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]) == FALSE )
	{
		return;
	}
	//�������еĹ������Զ��� �������
	if(api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth ) == FALSE )
	{
		return;
	}
	//����������RecordNo
	if( api_VReadSafeMem(FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo) != TRUE )
	{
		DataInfo.RecordNo = 0;
		DataInfo.SaveDot = 0;
	}
	//�������ַ��
	dwBaseAddr = FreezeAddrLen.dwDataAddr;
	//��������ݱ���ľ��Ե�ַ
	if(GetSectorInfo( FreezeAllInfo.AllAttDataLen, AttCycleDepth.Depth, DataInfo.RecordNo, &SectorInfo ) == FALSE )
	{
		return ;
	}
	FreezeAddrLen.dwDataAddr += SectorInfo.dwAddr; 	
	if(tFreezeIndex == eFREEZE_DAY)
	{
		//�ն���E2�������ݵĻ���ַ
		dwDataE2Addr = 	(DWORD)GET_CONTINUE_ADDR( FreezeConRom.EeData.Day[0] );
		//�ն���E2�������ݵ�ƫ�Ƶ�ַ
		dwDataE2Addr += (DWORD)(FreezeAllInfo.AllAttDataLen+sizeof(DWORD)) * (DataInfo.RecordNo% FreezeInfoTab[eFREEZE_DAY].wEeDepth);
		//�ն���E2������������ַ
		dwDataE2MaxAddr = (DWORD)GET_CONTINUE_ADDR( FreezeConRom.EeData.Day[0] ) + MAX_DAY_FREEZE_E2_DATA_LEN;
	}
	else if(tFreezeIndex == eFREEZE_CLOSING)
	{
		//�ն���E2�������ݵĻ���ַ
		dwDataE2Addr = (DWORD)	GET_CONTINUE_ADDR( FreezeConRom.EeData.Closing[0] );
		dwDataE2Addr += (DWORD)(FreezeAllInfo.AllAttDataLen+sizeof(DWORD)) * (DataInfo.RecordNo% FreezeInfoTab[eFREEZE_CLOSING].wEeDepth);
		//�����ն���E2������������ַ
		dwDataE2MaxAddr = (DWORD)GET_CONTINUE_ADDR( FreezeConRom.EeData.Closing[0] ) + MAX_CLOSING_FREEZE_E2_DATA_LEN;
	}
	

	if( tFreezeIndex >= eFREEZE_HOUR)
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag) == TRUE )
		{
			if( FreezeUpdateTimeFlag.wFreezeFlag & (0x0001<<tFreezeIndex) )
			{
				ProcFreezeNoRollback(DataInfo.RecordNo, AttCycleDepth.Depth, FreezeAllInfo.AllAttDataLen, dwBaseAddr, FlashBuf);
				FreezeUpdateTimeFlag.wFreezeFlag &= ~(0x0001<<tFreezeIndex);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);
			}
		}
	}

	memset( FlashBuf, 0xff, FreezeAllInfo.AllAttDataLen );
	//���ǲ������õ�ǰʱ����Է�����
	if(PowerUpFlag == 0x55)
	{
		//ʱ��
		memcpy( FlashBuf, (BYTE *)&PowerUpFreezeMin, sizeof(DWORD) );
	}
	else
	{
		//ʱ��
		memcpy( FlashBuf, (BYTE *)&g_RelativeMin, sizeof(DWORD) );
	}
	
	//��ȡ����
	for(i=0; i<FreezeAllInfo.NumofOad; i++)
	{
		api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&AttOad[i].Oad, NULL, AttOad[i].AttDataLen, FlashBuf+AttOad[i].Offset );
		if(PowerUpFlag == 0x55)
		{
			if( !(FreezeFlag & (0x0001<<eFREEZE_MAX_NUM)) )//����������
			{
				OI = (AttOad[i].Oad&0xff)*0x100+((AttOad[i].Oad>>8)&0xff);
				if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
				{
					//�˴��ж�Ŀ��Ϊ�ϵ粹��ʱ����δ����״̬�£��������������������Ϊȫ0
					//�����ղ����������һ������ʱ�������һ�����պ������Ѿ����㣬����Ҫ��ʶ
					//�������Ƿ�ȫF��Ҫ��ʶ�����ý����ղ���ʱ�˱�־��ʶ�Ƿ�ȫFF--��ا��
					if( tFreezeIndex != eFREEZE_CLOSING )
					{
						// ���������գ��������������ʱ���0x00
						memset( FlashBuf+AttOad[i].Offset, 0x00, AttOad[i].AttDataLen );
					}
					else
					{
						// ���������գ��������������ʱ���0xFF
						memset( FlashBuf+AttOad[i].Offset, 0xFF, AttOad[i].AttDataLen );
					}
				}
			}
		}
		else if( tFreezeIndex == eFREEZE_CLOSING )
		{
			if( !(FreezeFlag & (0x0001<<eFREEZE_MAX_NUM)) )//�ǵ�һ������
			{
				OI = (AttOad[i].Oad&0xff)*0x100+((AttOad[i].Oad>>8)&0xff);
				if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
				{
					// ����Ƿǵ�һ�����գ��������������ʱ���0xFF
					memset( FlashBuf+AttOad[i].Offset, 0xFF, AttOad[i].AttDataLen );
				}
			}
		}
	}
	//��Ӽ���4�ֽ�У�麯��--��ا��--200407
	lib_CheckSafeMemVerify(FlashBuf,(WORD)FreezeAllInfo.AllAttDataLen, REPAIR_CRC);

	//������������д��E2--��ا��-20407
	if( (FreezeInfoTab[tFreezeIndex].wEeDepth != 0) 
	&& ((dwDataE2Addr+FreezeAllInfo.AllAttDataLen +sizeof(DWORD)) <= dwDataE2MaxAddr)) //��Խ�缫��ֵ�ж�
	{
		//E2�ڵı������ݴ��������к�
		api_WriteToContinueEEPRom( dwDataE2Addr+sizeof(DWORD), FreezeAllInfo.AllAttDataLen, FlashBuf );
		api_WriteToContinueEEPRom( dwDataE2Addr, sizeof(DWORD), (BYTE*)&DataInfo.RecordNo );    
	}

	//д������������ݣ�дʧ�ܣ������Ȳ���д��������д����
	if(WriteFreezeRecordData( FreezeAddrLen.dwDataAddr, FreezeAllInfo.AllAttDataLen, dwBaseAddr, FreezeInfoTab[tFreezeIndex].dwDataLen, FlashBuf ) == FALSE)
	{
		WriteFreezeRecordData_EraseFlash(FreezeAddrLen.dwDataAddr, FreezeAllInfo.AllAttDataLen, FlashBuf);
	}
	
	DataInfo.RecordNo++;
	if( DataInfo.SaveDot < AttCycleDepth.Depth )
	{
		DataInfo.SaveDot++;
	}
	//���������RecordNo
	api_VWriteSafeMem(FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo);
}
//--------------------------------------------------
//��������:���Ӷ��� ����
//
//����: PlanIndex[in] ���Ӷ��᷽������
//		PlanBaseAddr[in]  ���Ӷ��᷽������ַ
//		No[in]	�������� ���Ӷ������к�
//		Len[in]	PlanIndex�������ݳ���
//		Buf[in] PlanIndex��������
//����ֵ:��
//
//��ע:�����¼��ΪNoʱ�����������洢�� No��ַ�������ݴ洢��No+1����
//	   Ϊ�˱����� ��������ͬʱ���������¶���ʱ�����
//--------------------------------------------------
static void ProcMinBakFreezeTask(DWORD PlanBaseAddr, DWORD Depth, DWORD No, WORD Len,  BYTE *Buf)
{
	BYTE EraseFlashBuf[10];
	DWORD MinBakBaseAddr;
	TSectorInfo	SectorInfo;
	TFreezeAddrLen FreezeAddrLen;

	if( (Depth == 0) || ( Len < sizeof(DWORD)) )
	{
		return;	//ÿһ����Ч���Ӷ���ĳ��ȱض���С��4( ʱ��4�ֽ�+ 4�ֽ�CRC/4�ֽ����к� + N�ֽ�����)
	}

	//��� ���Ӷ��ᱸ�ݵ� ��ʼ��ַ; ���Ӷ��������������� ���� ������
	GetFreezeAddrInfo(eFREEZE_MIN, &FreezeAddrLen);
	MinBakBaseAddr = FreezeAddrLen.dwDataAddr + MAX_MIN_FREEZE_DATA_LEN;

	if(No == 0)
	{
		memset(EraseFlashBuf, 0xFF, sizeof(EraseFlashBuf));
		WriteFreezeRecordData( PlanBaseAddr, sizeof(EraseFlashBuf), MinBakBaseAddr, MAX_MINBAK_FREEZE_DATA_LEN, EraseFlashBuf );
	}

	//������������ ����ĸ��ֽ�ΪCRC32, ����������ҪCRC���ö���������
	memcpy(EraseFlashBuf, Buf + Len - sizeof(DWORD), sizeof(DWORD));
	memcpy(Buf + Len - sizeof(DWORD), &No, sizeof(DWORD));
    GetSectorInfo(Len, Depth, No + 1, &SectorInfo);
   //дʧ�ܣ������Ȳ���д��������д����
	if(WriteFreezeRecordData( PlanBaseAddr+SectorInfo.dwAddr, Len, MinBakBaseAddr, MAX_MINBAK_FREEZE_DATA_LEN, Buf ) == FALSE)
	{
		WriteFreezeRecordData_EraseFlash(PlanBaseAddr+SectorInfo.dwAddr, Len, Buf);
	}
	memcpy(Buf + Len - sizeof(DWORD), EraseFlashBuf, sizeof(DWORD));	
}


//-----------------------------------------------
//��������: ������Ӷ�������
//
//����: 	��
//                    
//����ֵ:	��
//
//��ע:   
//-----------------------------------------------
void ProcMinFreezeTask(void)
{
	BYTE i,k,PlanIndex,bFlag;
	DWORD dwAddr,dwBaseAddr;
	TFreezeAttOad	AttOad[MAX_MINUTE_FREEZE_ATTRIBUTE+1];
	TSectorInfo	SectorInfo;
	TFreezeAttCycleDepth	AttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth	MinBakAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	BYTE FlashBuf[SECTOR_SIZE];
	TFreezeUpdateTimeFlag FreezeUpdateTimeFlag;
	TFreezeMinInfo FreezeMinInfo;

	// ��÷��Ӷ������Ե�ַ
	GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen );
	//��������Ĺ������Եĸ���
	if(api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo)== FALSE )
	{
		return;
	}	
	//�������еĹ������Զ���
	if(api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[eFREEZE_MIN].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]) == FALSE )
	{
		return;
	}
	//�������еĹ������Զ��� �������
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth[i] );	
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (i + MAX_FREEZE_PLANNUM), sizeof(TFreezeAttCycleDepth), (BYTE *)&MinBakAttCycleDepth[i] );
	}
	
	//��ȡ���Ӷ����ַ��Ϣ
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo );	
	//�����Ӷ���ָ�������
	CheckMinFreezeInfo();
	//���������޸�ʱ���־
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);	

	//����ַ���ڷ��Ӷ������ַ;
	dwBaseAddr =  FreezeAddrLen.dwDataAddr;
	if( FreezeMinInfo.DataAddr[0] != dwBaseAddr )//����0��ַ �����ַһ��
	{
		FreezeMinInfo.DataAddr[0] = dwBaseAddr;
	}
	
	for( PlanIndex = 0; PlanIndex < MAX_FREEZE_PLANNUM; PlanIndex++ )
	{
		CLEAR_WATCH_DOG;
		if( AttCycleDepth[PlanIndex].Cycle == 0 )//����Ϊ0����֧��
		{
			continue;
		}
		
		if( (g_RelativeMin%AttCycleDepth[PlanIndex].Cycle) == 0 )//��Ӧ���� ������ʱ��
		{
			if( FreezeMinInfo.AllAttDataLen[PlanIndex] > SECTOR_SIZE )
			{
				continue;
			}
			
			//��������ݱ���ľ��Ե�ַ
			if(GetSectorInfo( FreezeMinInfo.AllAttDataLen[PlanIndex], AttCycleDepth[PlanIndex].Depth, MinInfo[PlanIndex].RecordNo, &SectorInfo ) == FALSE )
			{
				continue;
			}

			dwAddr = (FreezeMinInfo.DataAddr[PlanIndex] + SectorInfo.dwAddr);
			
			bFlag = 0;
			//�жϱ��ݵ��޷��Ӷ����ʱ���־
			if( FreezeUpdateTimeFlag.wMinFlag & (0x0001<<PlanIndex) )
			{
				//���Ӷ��� ���������뱸���� ��¼��Ų�1��
				ProcFreezeNoRollback(MinInfo[PlanIndex].RecordNo, AttCycleDepth[PlanIndex].Depth, FreezeMinInfo.AllAttDataLen[PlanIndex], FreezeMinInfo.DataAddr[PlanIndex], FlashBuf);
				ProcFreezeNoRollback(MinInfo[PlanIndex].RecordNo+1, MinBakAttCycleDepth[PlanIndex].Depth, FreezeMinInfo.AllAttDataLen[PlanIndex], FreezeMinInfo.DataBakAddr[PlanIndex], FlashBuf);
				
				//������Ӷ����Ӧ�������޸Ķ����־
				FreezeUpdateTimeFlag.wMinFlag &= ~(0x0001<<PlanIndex);
				api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag),  (BYTE*)&FreezeUpdateTimeFlag);

				if( PlanIndex == 0 )
				{
					bFlag = 0x55;
				}
			}
			
			memset( FlashBuf, 0xff, FreezeMinInfo.AllAttDataLen[PlanIndex] );
			//��֡ʱ��
			memcpy( FlashBuf, (BYTE *)&g_RelativeMin, sizeof(DWORD) );
			
			for(i=0; i<FreezeAllInfo.NumofOad; i++)
			{	
				k = CURR_PLAN(AttOad[i].Oad);
				if( k != PlanIndex )
				{
					continue;
				}
				
				api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&AttOad[i].Oad, NULL, AttOad[i].AttDataLen, FlashBuf+AttOad[i].Offset );
			}

			lib_CheckSafeMemVerify(FlashBuf,(WORD)FreezeMinInfo.AllAttDataLen[PlanIndex], REPAIR_CRC);
			////д������������ݣ�дʧ�ܣ������Ȳ���д��������д����
			if(WriteFreezeRecordData( dwAddr, FreezeMinInfo.AllAttDataLen[PlanIndex], dwBaseAddr, FreezeInfoTab[eFREEZE_MIN].dwDataLen, FlashBuf ) == FALSE)
			{
				WriteFreezeRecordData_EraseFlash( dwAddr, FreezeMinInfo.AllAttDataLen[PlanIndex],FlashBuf);
			}
			ProcMinBakFreezeTask(FreezeMinInfo.DataBakAddr[PlanIndex], MinBakAttCycleDepth[PlanIndex].Depth, MinInfo[PlanIndex].RecordNo, FreezeMinInfo.AllAttDataLen[PlanIndex], FlashBuf);

			MinInfo[PlanIndex].RecordNo++;
			if( MinInfo[PlanIndex].SaveDot < AttCycleDepth[PlanIndex].Depth )
			{
				MinInfo[PlanIndex].SaveDot++;
			}
			MinInfo[PlanIndex].CRC32 = lib_CheckCRC32( (BYTE*)&MinInfo[PlanIndex], sizeof(TFreezeDataInfo)-sizeof(DWORD) );

			#if (SLE_THIRD_MEM_BACKUP == YES)
			if( PlanIndex == 0 )//���淽��0ʱ��ͬ������FLASH��������
			{
				SaveFlashMemData(MinInfo[0].RecordNo-1, bFlag, FlashBuf);
			}
			#endif
		}
	}
}
//--------------------------------------------------
//��������:  ��Զ���415��ʶ����Է��Ӷ�������⴦��
//         
//����:      ��
//         
//����ֵ:    ��
//         
//��ע:  �˺�������Է��Ӷ��ᣬ����api_FreezeTask��������
//--------------------------------------------------
void  DealMinFreeze( void )
{
	TRealTimer t;

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
	if((t.Sec > 0x05)&&(t.Sec < 0x09))
	{
		if(minFreezeFlag == 0xA5)		//�˴���־��Ҫ�ϵ��ʼ��minFreezeFlag == 0xA5
		{
			minFreezeFlag = 0x5A;
			SecondBufferTime = FREEZEBUFFERTIME;
			SetFreezeFlag(eFREEZE_MIN);
		}
	}
	if(SecondBufferTime > 0)
	{
		SecondBufferTime --;
	}
	if((SecondBufferTime == 0)&&(minFreezeFlag == 0x5A))
	{
		minFreezeFlag = 0xA5;	
	}
}
//-----------------------------------------------
//��������: ��������Ӷ���֮��Ķ�������
//
//����: 	��
//                    
//����ֵ:	��
//
//��ע:   
//-----------------------------------------------
void api_FreezeTask( void )
{
	BYTE i;
	TRealTimer t;
	
	// ������
	if( api_GetTaskFlag(eTASK_FREEZE_ID,eFLAG_SECOND) == TRUE )
	{		
		api_ClrTaskFlag(eTASK_FREEZE_ID,eFLAG_SECOND);
		
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
		
		if( (t.Min==0x01) && (t.Sec==(eTASK_FREEZE_ID*3+3+0)) )
		{
			//ÿСʱ����һ�� ÿСʱ�ĵ�һ���ӣ�24~26��֮��
			SaveMinFreezePoint();
		}

		if (t.Sec== 0x05)
		{
			DealMinFreeze();
		}
		
		if( wInstantFreezeTimer != 0 )
		{
			wInstantFreezeTimer--;
			wInstantFreezeTimerBak = (WORD)(~wInstantFreezeTimer);
			if( wInstantFreezeTimer == 0 )
			{
				SetFreezeFlag(eFREEZE_INSTANT);
			}
		}		
	}
	
	//��������
	if( api_GetTaskFlag(eTASK_FREEZE_ID,eFLAG_MINUTE) == TRUE )
	{	
		SetFreezeFlag(eFREEZE_MIN);
		api_ClrTaskFlag(eTASK_FREEZE_ID,eFLAG_MINUTE);
	}
	
	//Сʱ����
	if( api_GetTaskFlag(eTASK_FREEZE_ID,eFLAG_HOUR) == TRUE )
	{		
		api_ClrTaskFlag(eTASK_FREEZE_ID,eFLAG_HOUR);

		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
		
		// ��������,ת�����,�¶��õ���ת��
		CheckClosing();									
		
		#if( PREPAY_MODE == PREPAY_LOCAL ) 
		//��������: ���ݽ����ռ����ܣ����ڱ�����ݽ���ʱ���ܵ��룬���ڼ�������õ���
		if( api_CheckLadderClosing() == TRUE )
		{
			SetFreezeFlag(eFREEZE_LADDER_CLOSING);
		}	
		#endif
		
		//����Сʱ����
		SetFreezeFlag(eFREEZE_HOUR);
		//�����ն���
		if( t.Hour == 0 )
		{
			SetFreezeFlag(eFREEZE_DAY);
		}
		
		//�����¶���
		if( (t.Hour==0) && (t.Day==1) )
		{
			SetFreezeFlag(eFREEZE_MON);
		}
	}
	
	if( FreezeFlag != 0 )
	{
	    FunctionConst(FREEZE_RUN_TASK);
	    
		for(i=0; i<=eFREEZE_MAX_NUM; i++)
		{
			//�����������˴�����ʱ���ǵø��Ķ����ϵ紦������api_PowerUpFreeze����������Ӧ����----��ا��
			if( FreezeFlag & (0x0001<<i) )
			{
				if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )//������
				{
					break;
				}
				
				CLEAR_WATCH_DOG;//������Ź�
				ClrFreezeFlag(i);
				if( i == eFREEZE_MIN )
				{
					ProcMinFreezeTask();
				}
				else if( i == eFREEZE_MAX_NUM )//��һ�����ն��ᣬ��Ҫ������
				{
					//�������������еĶ��ᶼ�������������� 		
					// �½�����ܣ��������ۼƵ��ܼ���
					api_ClosingMonEnergy(); 				
					#if( MAX_PHASE == 3 )
					#if( SEL_DEMAND_2022 == NO )
					api_ClrDemand(eClearAllDemand);
					#else
					api_ClrDemand(eClearAllDemand,eDemandRun);
					#endif
					#endif
				}
				else
				{
					ProcFreezeTask(i,0x00);
					
					#if( PREPAY_MODE == PREPAY_LOCAL ) 
					if( i == eFREEZE_LADDER_CLOSING )
					{
						api_ClrCurLadderUseEnergy(); //�����ǰ���ݽ����õ���
					}
					#endif
				}
			}
		}
	}
}


//-----------------------------------------------
//��������: ��ȡ����/ָ��OAD�ĳ���
//
//����: 
//	Type[in]:	eALL_OAD_LEN/eSELECTED_OAD_LEN               
//  pData[in]:	
//                
//  ����ֵ:	���� ����/ָ��OAD�����ݳ���
//
//��ע:���س��Ȳ���sum�͵ĳ���	
//-----------------------------------------------
static WORD GetFreezeAttDataLen( BYTE Type, TFreezeData *pData )
{	
	BYTE i,j,OadIndex;
	WORD wLen,ReturnLen;
	DWORD dwOad,dwTemp;

	ReturnLen = 0;

	if( Type == eSELECTED_OAD_LEN )
	{
		for( i=0; i<pData->pDLT698RecordPara->OADNum; i++ )
		{
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );

			for( j=0; j<pData->pAllInfo->NumofOad; j++ )
			{
				if( 	(dwOad==pData->pAttOad[j].Oad)
					||	(dwOad==FREEZE_RECORD_NO_OAD)
					||	(dwOad==FREEZE_RECORD_TIME_OAD) )
				{
					break;
				}
			}			

			if( j < pData->pAllInfo->NumofOad )
			{
				wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );	
				if( wLen == 0x8000 )
				{
					return 0;
				}
			}
			else						
			{
				//�Ƿ��Ǵ�������0������0����
				wLen = 0;
				
				OadIndex = (BYTE)(dwOad>>24);	//ȡԪ������
				dwOad &= ~0xFF000000;			//��OAD��Ԫ��������Ϊ0

				for( j=0; j<pData->pAllInfo->NumofOad; j++ )
				{
					if( pData->pAttOad[j].Oad == dwOad )
					{
						break;
					}
				}

				if( j < pData->pAllInfo->NumofOad )
				{
					dwTemp = OadIndex;		//Ԫ������
					dwTemp = (dwTemp<<24);
					dwOad |= dwTemp;		//��ԭԭ����Ԫ��������OAD
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
					if( wLen == 0x8000 )
					{
						return 0;
					}
				}
				else
				{
					wLen = 1;	// ��֧�ֵ�OAD������һ��0,����Ϊ1
				}								
			}	
			
			ReturnLen += wLen;	
		}
	}
	else if( Type == eALL_OAD_LEN )
	{
		for( i=0; i<pData->pAllInfo->NumofOad; i++ )
		{
			dwOad = pData->pAttOad[i].Oad;
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )
			{
				return 0;
			}
			ReturnLen += wLen;		
		}
		dwOad = FREEZE_RECORD_NO_OAD;
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return 0;
		}
		ReturnLen += wLen;

		dwOad = FREEZE_RECORD_TIME_OAD;
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return 0;
		}
		ReturnLen += wLen;
	}

	return ReturnLen;
}

//-----------------------------------------------
//��������: 698��ȡ��������������Ա���
//
//����: 
// 	OAD[in]:		Ҫ���ҵ�OAD
//	Num[in]:		OAD����                   
//  AttOad[out]: 	ʵ�ʵĹ����������Ա�
//				
//����ֵ:	���ҵ���ƫ�� 0x80δ�鵽
//
//��ע:	
//-----------------------------------------------
BYTE DealEnergyOADFor698( DWORD OAD, BYTE Num, TFreezeAttOad *pAttOad )
{
	BYTE i;
	WORD OI;
	BYTE *pClassAttribute;
	TFreezeAttOad AttOad[MAX_FREEZE_ATTRIBUTE+1];

	lib_ExchangeData((BYTE*)&OI,(BYTE*)&OAD,2);
	if( OI > 0x1000 )//�ǵ���OAD������
	{
		return 0x80;
	}
	
	if( Num > MAX_FREEZE_ATTRIBUTE )
	{
		return 0x80;
	}

	memcpy( &AttOad, pAttOad, sizeof(TFreezeAttOad)*Num );
	
	for( i = 0; i < Num; i++ )
	{
		lib_ExchangeData((BYTE*)&OI,(BYTE*)&AttOad[i].Oad,2);
		pClassAttribute = (BYTE*)&AttOad[i].Oad+2;
		if( OI < 0x1000 )//���� ���߾��ȵ���ת��Ϊ��ͨ����
		{
			if( (*pClassAttribute&0x1f) == 0x04 )//�߾��ȵ���
			{
				*pClassAttribute = (*pClassAttribute&0xf8)+0x02;
			}
		}

		if( OAD == AttOad[i].Oad )//����ͨ�������
		{
			return i;
		}
	}

	return 0x80;
}

//-----------------------------------------------
//��������: ���ն�����Ų�������
//
//����:            
//  pData[in]:	������Ϣ
//                
//  ����ֵ:	�������ݳ��� 0--����ռ䲻�� 0x8000--�쳣
//
//��ע:	
//-----------------------------------------------
WORD ReadFreezeByRecordNo( TFreezeData	*pData )
{	
	BYTE i,j,k,OadIndex;
	WORD Offset,wLen,wTempLen,TempOffset,OI,m,Result;
	DWORD dwAddr,dwOad,dwTempOad,dwTemp1;
	BYTE FreezeBuf[2000];//��������
	TSectorInfo	SectorInfo;
	TRealTimer 	Time;
	TReadFreezeRecordInfo FreezeRecordInfo;
	
	Offset = 0;
	
	if( pData->pDLT698RecordPara->OADNum == 0 )
	{		
		wLen = GetFreezeAttDataLen( eALL_OAD_LEN, pData );
		if( wLen > pData->Len )
		{
			return 0;// �ռ䲻��
		}
		
		//���Ӷ���RCSD����Ϊ0����ˣ��˴������Ƿ��Ӷ���
		//��ȡ�������ݵĵ�ַ
		if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, pData->RecordNo, &SectorInfo ) == FALSE )
		{
			return 0x8000;
		}

		if(pData->pAllInfo->AllAttDataLen>2000)
		{
			return 0x8000;
		}
		//�ǵü���Ƿ����ͷŻ���
		FreezeRecordInfo.dwAddr = pData->pFreezeAddrLen->dwDataAddr+SectorInfo.dwAddr;//flash�洢��ַ
		FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//��������
		FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
		FreezeRecordInfo.RecordNo = pData->RecordNo;//�����¼���
		#if( MAX_PHASE == 1 )
		//���������E2��ȡ����
		if(( api_GetSysStatus(eSYS_STATUS_POWER_ON) != TRUE ) && (pData->pDLT698RecordPara->Ch == 0X55))
		{
			//��E2������������
			if(ReadFreezeRecordEeData( &FreezeRecordInfo, FreezeBuf ) != TRUE)
			{
				return 0x8000;
			}
		}
		else
		#endif	
		{
			//������������
			if(ReadFreezeRecordData( &FreezeRecordInfo, FreezeBuf ) != TRUE)
			{
				return 0x8000;
			}
		}
		//���RecordNo	
		dwOad = FREEZE_RECORD_NO_OAD;
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return 0x8000;
		}
		
		if( pData->Tag == eTagData )
		{
			api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, (BYTE *)&pData->RecordNo, 0xFF, pData->pBuf+Offset );	
		}
		else
		{
			memcpy( pData->pBuf+Offset, (BYTE *)&pData->RecordNo, wLen );
		}
		Offset += wLen;

		//���Time
		dwOad = FREEZE_RECORD_TIME_OAD;
		wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
		if( wLen == 0x8000 )
		{
			return 0x8000;
		}
		memcpy((BYTE *)&dwTemp1, FreezeBuf, sizeof(DWORD) );
		api_ToAbsTime( dwTemp1, (TRealTimer *)&Time );
		if( pData->Tag == eTagData )
		{
			api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, (BYTE *)&Time, 0xFF, pData->pBuf+Offset );	
		}
		else
		{
			memcpy( pData->pBuf+Offset, (BYTE *)&Time, wLen );
		}
		Offset += wLen;
		
		//��������������				
		for( i=0;  i<pData->pAllInfo->NumofOad; i++ )
		{
			if( pData->pAttCycleDepth->Depth == 0xFFFF )
			{
				break;					
			}
			
			CLEAR_WATCH_DOG;

			dwOad = pData->pAttOad[i].Oad;
			wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
			if( wLen == 0x8000 )
			{
				return 0x8000;
			}
			
            if( (Offset+ wLen) > pData->Len )//���ⳬ��
            {
                return 0; 	// ������	
            }
            
			ASSERT(pData->pAttOad[i].AttDataLen<2000, 0);
			//�������������ǽ����������ݣ�������Ϊff������Ϊnull
			m = 0xfffc;//����ֵ������Ϊ0��ffff

			#if( MAX_PHASE == 3 )
			if( ( dwOad == ZEROCURRRNT_OAD ) 
			|| ( dwOad == CURRRNTVECTORSUM_OAD ) )
			{
				for(m=0; m<pData->pAttOad[i].AttDataLen; m++)
				{
					if(FreezeBuf[pData->pAttOad[i].Offset+m] != 0xff)
					{
						break;
					}
				}
			}
			else 
			#endif
			if( pData->FreezeIndex == eFREEZE_CLOSING )
			{
				OI = (dwOad&0xff)*0x100+((dwOad>>8)&0xff);
				if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
				{
					for(m=0; m<pData->pAttOad[i].AttDataLen; m++)
					{
						if(FreezeBuf[pData->pAttOad[i].Offset+m] != 0xff)
						{
							break;
						}
					}
				}
			}
			
			//�������������ǽ����������ݣ�������Ϊff������Ϊnull
			if( m == pData->pAttOad[i].AttDataLen )
			{
				pData->pBuf[Offset] = 0;
				wLen = 1;
			}
			else
			{
				if( pData->Tag == eTagData )
				{
					api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, FreezeBuf+pData->pAttOad[i].Offset, 0xFF, pData->pBuf+Offset );	
				}
				else
				{
					//����ʹ�ö��᳤�ȴ��룬���ܽ���ת���� �ὫwLenת��Ϊʵ�ʳ���
					wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );
					//ǰ���жϳ����� ����ֱ�����
					Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, FreezeBuf+pData->pAttOad[i].Offset, pData->Len -Offset, pData->pBuf+Offset );
					if( (Result == 0x8000) || (Result == 0) )
					{
						return Result;
					}
				}
			}
			
			Offset += wLen;			
		}
	}
	else
	{		
		wLen = GetFreezeAttDataLen( eSELECTED_OAD_LEN, pData );
		if( wLen > pData->Len )
		{
			return 0;// �ռ䲻��
		}
		
		if( pData->FreezeIndex == eFREEZE_MIN )//���Ӷ����ַ��������
		{
			
			//��ȡ�������ݵĵ�ַ
			if(GetSectorInfo( pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex], pData->pAttCycleDepth->Depth, pData->RecordNo, &SectorInfo ) == FALSE )
			{
				return 0x8000;
			}
			
			FreezeRecordInfo.dwAddr = pData->pMinInfo->DataAddr[pData->MinPlanIndex]+SectorInfo.dwAddr;
			FreezeRecordInfo.RecordLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//��������
			FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
			FreezeRecordInfo.RecordNo = pData->RecordNo;//�����¼���
			FreezeRecordInfo.MinPlan = pData->MinPlanIndex;
			FreezeRecordInfo.dwBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];		// ���Ӷ��� ���ݵ�ַ
		}
		else
		{
			//��ȡ�������ݵĵ�ַ
			if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, pData->RecordNo, &SectorInfo ) == FALSE )
			{
				return 0x8000;
			}

			if(pData->pAllInfo->AllAttDataLen>2000)
			{
				return 0x8000;
			}
			//�ǵü���Ƿ����ͷŻ���
			FreezeRecordInfo.dwAddr = pData->pFreezeAddrLen->dwDataAddr+SectorInfo.dwAddr;//flash�洢��ַ
			FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//��������
			FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
			FreezeRecordInfo.RecordNo = pData->RecordNo;//�����¼���

		}

		#if( MAX_PHASE == 1 )
		//���������E2��ȡ����
		if(( api_GetSysStatus(eSYS_STATUS_POWER_ON) != TRUE ) && (pData->pDLT698RecordPara->Ch == 0X55))
		{
			//��E2������������
			if(ReadFreezeRecordEeData( &FreezeRecordInfo, FreezeBuf ) != TRUE)
			{
				return 0x8000;
			}
		}
		else
		#endif	
		{
			//������������
			if(ReadFreezeRecordData( &FreezeRecordInfo, FreezeBuf ) != TRUE)
			{
				return 0x8000;
			}
		}


		for( i=0;  i<pData->pDLT698RecordPara->OADNum; i++ )
		{
			CLEAR_WATCH_DOG;
			
			memcpy( (BYTE *)&dwOad, pData->pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
			
			// ����Ӧ��OAD
			dwTempOad = dwOad & (~0x00E00000);	
			if( (dwTempOad == FREEZE_RECORD_NO_OAD) || (dwTempOad == FREEZE_RECORD_TIME_OAD) )
			{
				dwOad = dwOad & (~0x00E00000);
				j = 0;
				
			}
			else
			{
				for( j=0; j<pData->pAllInfo->NumofOad; j++ )
				{												
					if( dwOad == pData->pAttOad[j].Oad )
					{
						break;
					}
				}
			}

			if( j == pData->pAllInfo->NumofOad )//����ǳ�����ͨ���ܣ���λoad������һ��
			{
				j = DealEnergyOADFor698( dwOad, pData->pAllInfo->NumofOad, pData->pAttOad );
				if( j == 0x80 )//δ�ѵ�
				{
					j = pData->pAllInfo->NumofOad;
				}
			}
			
			if( j < pData->pAllInfo->NumofOad )
			{
				
				if( dwOad == FREEZE_RECORD_NO_OAD )
				{
					//���RecordNo	
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
					if( wLen == 0x8000 )
					{
						return 0x8000;
					}

					if( (Offset+ wLen) > pData->Len )//���ⳬ��
					{
						return 0; 	// ������	
					}
                    
					if( pData->Tag == eTagData )
					{
						api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, (BYTE *)&pData->RecordNo, 0xFF, pData->pBuf+Offset );	
					}
					else
					{
						memcpy( pData->pBuf+Offset, (BYTE *)&pData->RecordNo, wLen );
					}
					Offset += wLen;
				}
				else if( dwOad == FREEZE_RECORD_TIME_OAD )
				{
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
					if( wLen == 0x8000 )
					{
						return 0x8000;
					}

					if( (Offset+ wLen) > pData->Len )//���ⳬ��
					{
						return 0; 	// ������	
					}

					memcpy((BYTE *)&dwTemp1, FreezeBuf, sizeof(DWORD) );
				
					api_ToAbsTime( dwTemp1, (TRealTimer *)&Time );
					if( pData->Tag == eTagData )
					{
						api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, (BYTE *)&Time, 0xFF, pData->pBuf+Offset );	
					}
					else
					{
						memcpy( pData->pBuf+Offset, (BYTE *)&Time, wLen );
					}
					Offset += wLen;
				}
				else
				{
					// �����˶��ٸ�����
					k = 0;
					memset((BYTE*)&tApplFrzDataFLASH[0].ApplClassID,0xFF,sizeof(tApplFrzDataFLASH));
					for (wTempLen = 0; wTempLen < NILM_EQUIPMENT_MAX_NO; wTempLen++)
					{
						if( memcmp( FreezeBuf + pData->pAttOad[j].Offset + (WORD)wTempLen*OI4800_04_SIZE, bConstTempBuf,OI4800_04_SIZE) != 0 )
						{
							k++;// ��ȫ0xff
							memcpy((BYTE*)&tApplFrzDataFLASH[wTempLen].ApplClassID,(BYTE *)&FreezeBuf[pData->pAttOad[j].Offset + (WORD)wTempLen*OI4800_04_SIZE],sizeof(T_ApplFrzData));//��������ٶ�ȡ���ᣬһЩ���ݲ���ʾ����
						}
						else
						{
							break;
						}
					}
					api_SetTagArrayNum(k);													
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );

					if( wLen == 0x8000 )
					{
						return 0x8000;
					}
					
					if( (Offset+ wLen) > pData->Len )//���ⳬ��
					{
						return 0; 	// ������	
					}
					m = 0xfffc;//����ֵ������Ϊ0��ffff

					#if( MAX_PHASE == 3 )
					if( ( dwOad == ZEROCURRRNT_OAD ) 
					|| ( dwOad == CURRRNTVECTORSUM_OAD ) )
					{
						for(m=0; m<pData->pAttOad[j].AttDataLen; m++)
						{
							if(FreezeBuf[pData->pAttOad[j].Offset+m] != 0xff)
							{
								break;
							}
						}
					}
					else 
					#endif
					//�������������ǽ����������ݣ�������Ϊff������Ϊnull						
					if( pData->FreezeIndex == eFREEZE_CLOSING )
					{
						OI = (dwOad&0xff)*0x100+((dwOad>>8)&0xff);
						if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
						{
							for(m=0; m<pData->pAttOad[j].AttDataLen; m++)
							{
								if(FreezeBuf[pData->pAttOad[j].Offset+m] != 0xff)
								{
									break;
								}
							}
						}
					}
						
									
					//�������������ǽ����������ݣ�������Ϊff������Ϊnull
					if( m == pData->pAttOad[j].AttDataLen )
					{
						pData->pBuf[Offset] = 0;
						wLen = 1;
					}
					else
					{
						if( pData->Tag == eTagData )
						{
							//��������λ��Ϊ����ַ��ƫ�Ƶ�ַ
							api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, FreezeBuf+pData->pAttOad[j].Offset, 0xFF, pData->pBuf+Offset );	
							
						}
						else
						{
							//����ʹ�ö��᳤�ȴ���
							wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );

							//ǰ���жϳ����� ����ֱ�����
							Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, FreezeBuf+pData->pAttOad[j].Offset,pData->Len -Offset,pData->pBuf+Offset );
							if( (Result == 0x8000) || (Result == 0) )
							{
								return Result;
							}
						}
					}
					
					Offset += wLen;	
				}	
				
			}
			else		
			{
				//ָ��OAD�ڹ������Զ������û���ҵ�����Ԫ��������Ϊ0������һ��
				OadIndex = (BYTE)(dwOad>>24);
				dwTempOad = dwOad & (~0xFF000000);
				for( j=0; j<pData->pAllInfo->NumofOad; j++ )
				{
					if( dwTempOad == pData->pAttOad[j].Oad )
					{
						break;
					}
				}
				
				if( j == pData->pAllInfo->NumofOad )//����ǳ�����ͨ���ܣ���λoad������һ��
				{
					j = DealEnergyOADFor698( dwTempOad, pData->pAllInfo->NumofOad, pData->pAttOad );
					if( j == 0x80 )//δ�ѵ�
					{
						j = pData->pAllInfo->NumofOad;
					}
				}
				
				//��������0������0����
				if( j < pData->pAllInfo->NumofOad )
				{					
					wLen = api_GetProOADLen( eGetNormalData, pData->Tag, (BYTE *)&dwOad, 0 );
					if( wLen == 0x8000 )
					{
						return 0x8000;
					}
					
					if( (Offset+ wLen) > pData->Len )//���ⳬ��
					{
						return 0; 	// ������	
					}

					//Ԫ�������Ϸ����ж�
					if( (OadIndex>VALID_OAD_INDEX) || (OadIndex==0) )
					{
						OadIndex = 1;	// ��ֹ����
					}
					TempOffset = 0;
		
					//����ָ��Ԫ��������Ӧ�ڴ洢OAD���ݵ�ƫ�Ƶ�ַpTemp
					for( k=1; k<OadIndex; k++ ) // pData->OadIndex ��1��ʼ
					{
						dwTemp1 = k;
						dwTempOad = ( (dwOad & (~0xFF000000)) | (dwTemp1<<24) );	
						wTempLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwTempOad, 0 );
						if( wTempLen == 0x8000 )
						{
							return 0x8000;
						}			
						TempOffset += wTempLen;
					}
					
					//�������������ǽ����������ݣ�������Ϊff������Ϊnull
					m = 0xfffe;//����ֵ������Ϊ0��ffff						
					if( pData->FreezeIndex == eFREEZE_CLOSING )
					{
						OI = (dwOad&0xff)*0x100+((dwOad>>8)&0xff);
						if( (OI>=MAX_DEMAND_START_OI) && (OI<=MAX_DEMAND_END_OI) )
						{
							for(m=0; m<wLen; m++)
							{
								if(FreezeBuf[pData->pAttOad[j].Offset+TempOffset+m] != 0xff)
								{
									break;
								}
							}
						}
					}
					if( m == wLen )
					{
						pData->pBuf[Offset] = 0;
						wLen = 1;
					}
					else
					{
						if( pData->Tag == eTagData )
						{
							//��������λ��Ϊ����ַ��ƫ�Ƶ�ַ
							api_GetProOadData( eGetRecordData, eAddTag, 0xff, (BYTE *)&dwOad, FreezeBuf+pData->pAttOad[j].Offset+TempOffset, 0xFF, pData->pBuf+Offset );

						}
						else
						{
							//����ʹ�ö��᳤�ȴ��룬���ܽ���ת���� �ὫwLenת��Ϊʵ�ʳ���
							wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwOad, 0 );

							//ǰ���жϳ����� ����ֱ�����
							Result = api_RecordDataTransform( (BYTE *)&dwOad, &wLen, FreezeBuf+pData->pAttOad[j].Offset+TempOffset, pData->Len -Offset, pData->pBuf+Offset );
							if( (Result == 0x8000) || (Result == 0) )
							{
								return Result;
							}
							
						}
					}
					
					Offset += wLen;	
				}
				else
				{
					pData->pBuf[Offset] = 0x00;
					Offset += 1;	
					continue;
				}
			}
		}
	}
	
	return Offset;
}

//-----------------------------------------------
//��������: ����N�ζ����¼
//
//����:            
//  pData[in]:	������Ϣ
//                
//  ����ֵ:	�������ݳ��� 0--����ռ䲻�� 0x8000--�쳣
//
//��ע:	
//-----------------------------------------------
WORD ReadFreezeByLastNum( TFreezeData	*pData )
{		
	if( pData->pDLT698RecordPara->TimeOrLast == 0 )
	{
		return 0x8000;
	}	
	
	//��Ҫ�����ϼ������Ѿ���������������ж�
	if( pData->pDLT698RecordPara->TimeOrLast > pData->pDataInfo->SaveDot )
	{
		return 0x8000;
	}
	
	//Ҫ�����ϼ��ζ�Ӧ��RecordNo
	pData->RecordNo = pData->pDataInfo->RecordNo - (pData->pDLT698RecordPara->TimeOrLast);	
	
	return ReadFreezeByRecordNo(pData);	
}

//-----------------------------------------------
//��������: ����ָ��ʱ����Ҷ����¼��
//
//����:            
//  pData[in]:	������Ϣ
//                
//  ����ֵ:	�������ݳ��� 0--����ռ䲻�� 0x8000--�쳣
//
//��ע:	
//-----------------------------------------------
BOOL SearchFreezeRecordNoByTime( TFreezeData	*pData )
{
	DWORD				i;
	DWORD				dwTemp,dwAddr,dwBaseAddr,dwEndAddr,dwTimeAddr,dwStartTime,dwEndTime,dwRecordNo;
	DWORD				SearchStartRecordNo,SearchEndRecordNo;
	TFreezeDataInfo		*pDataInfo;
	TSectorInfo			SectorInfo;
	TBinarySchInfo		BinarySchInfo;
	TReadFreezeRecordInfo	FreezeRecordInfo;
	pDataInfo = pData->pDataInfo;
	DWORD RealSaveDot;
	BYTE IsTRUE = TRUE;

	// �����ݵ����
	if( pDataInfo->SaveDot != 0 )
	{							
		if( pData->FreezeIndex < eFREEZE_HOUR )
		{
			//�����ڶ����࣬���޸�ʱ��ʱ��Ӱ��֮ǰ�����ݣ��������±��棬����ʱ����ܲ��ǵ��������ģ�
			//����ڰ���ʱ����ҵ�ʱ��������һ���¼���ʼ�Ƚϣ��ҵ�����ʵ�һ�ξ��ǣ����ټ���������
			dwBaseAddr = pData->pFreezeAddrLen->dwDataAddr;// ���ݻ���ַ
			dwStartTime = 0;
			dwEndTime = 0;
			SearchStartRecordNo = pDataInfo->RecordNo-1;
			SearchEndRecordNo = pDataInfo->RecordNo-1;
			
			//�����һ������ǰ��
			for(i=0; i<pDataInfo->SaveDot; i++)
			{
				CLEAR_WATCH_DOG;
				if(i >500)//��ֹ����for��,�������Զ���������Ϊ�����ն��ᣬ�����Ҫ12�Σ�������
				{
					break;
				}
				if( pDataInfo->RecordNo >= i )
				{
					SearchStartRecordNo = pDataInfo->RecordNo-i-1;
					
					if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, SearchStartRecordNo, &SectorInfo ) == FALSE)
					{
						break;
					}
					dwAddr = dwBaseAddr + SectorInfo.dwAddr;
					FreezeRecordInfo.dwAddr = dwAddr;//flash�洢��ַ
					FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//��������
					FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
					FreezeRecordInfo.RecordNo = SearchStartRecordNo;//�����¼���
					
					if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwStartTime )==FALSE)
					{
						//��������²�����
					}
					if( pData->pDLT698RecordPara->TimeOrLast == dwStartTime )
					{
						//�����������Ҫ���ҵĵ㣬����������RecordNo
						pData->RecordNo = SearchStartRecordNo;								// �ҵ������
						memcpy( (BYTE *)&(pData->SearchTime), (BYTE *)&dwStartTime, sizeof(DWORD) );	// �����ҵ���ʱ��
						break;
					}
					else if( 	(pData->pDLT698RecordPara->TimeOrLast > dwStartTime)
						 	&& 	(pData->pDLT698RecordPara->TimeOrLast <= dwEndTime) )
					{
						//Ҫ���ҵĵ�ʱ�����������ʱ�䣬��С�ڵ����ϴβ��ҵ��ʱ�䣬�����ϴε�ʱ�䣬iΪ0ʱ�϶�������
						if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, SearchEndRecordNo, &SectorInfo ) == FALSE)
						{
							return FALSE;
						}
						dwAddr = dwBaseAddr + SectorInfo.dwAddr;
						FreezeRecordInfo.dwAddr = dwAddr;//flash�洢��ַ
						FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//��������
						FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
						FreezeRecordInfo.RecordNo = SearchEndRecordNo;//�����¼���
						
						if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&(pData->SearchTime) )==FALSE)
						{
							//��������²�����
						}
						pData->RecordNo = SearchEndRecordNo;								// �ҵ������
						break;
					}
					else
					{
						//�������ݿ���ʱ�䶼һ����������������һ��ʱ��
						if( dwEndTime != dwStartTime )
						{
							dwEndTime = dwStartTime;
							SearchEndRecordNo = SearchStartRecordNo;
						}
					}														
				}												
			}
			
			//��Ч�㶼������Ҳ������������ģ����ж�һ��
			if( i == pDataInfo->SaveDot )
			{
				if(pData->pDLT698RecordPara->TimeOrLast < dwEndTime)
				{
					if(GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, SearchEndRecordNo, &SectorInfo ) == FALSE)
					{
						return FALSE;
					}
					dwAddr = dwBaseAddr + SectorInfo.dwAddr;
					FreezeRecordInfo.dwAddr = dwAddr;//flash�洢��ַ
					FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//��������
					FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
					FreezeRecordInfo.RecordNo = SearchEndRecordNo;//�����¼���
					
					if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&(pData->SearchTime) )==FALSE)
					{
						//�������������
					}
					pData->RecordNo = SearchEndRecordNo;									// �ҵ������
				}
				else
				{
					return FALSE;
				}
			}
		}
		else
		{
			//���ڶ����࣬���е�ʱ�궼�ǵ��������ģ����ö��ַ�����
			// �ҽ���ʱ�� ���һ������
			if( pData->FreezeIndex == eFREEZE_MIN )
			{
				//�õ�����ַ
				dwBaseAddr = pData->pMinInfo->DataAddr[pData->MinPlanIndex];
				//��������ݱ���ľ��Ե�ַ
				if(GetSectorInfo( pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex], pData->pAttCycleDepth->Depth, pDataInfo->RecordNo-1, &SectorInfo ) == FALSE )
				{
					return FALSE;
				}
				FreezeRecordInfo.RecordLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//��������
				FreezeRecordInfo.dwBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];
				FreezeRecordInfo.MinPlan = pData->MinPlanIndex;
			}
			else
			{
				// ���ݻ���ַ
				dwBaseAddr = pData->pFreezeAddrLen->dwDataAddr;
				GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, pDataInfo->RecordNo-1, &SectorInfo );
				FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//��������
				
			}
			dwEndAddr = dwBaseAddr + SectorInfo.dwAddr;

			FreezeRecordInfo.dwAddr = dwEndAddr;//flash�洢��ַ
			FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
			FreezeRecordInfo.RecordNo = pDataInfo->RecordNo-1;//�����¼���
			
			if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwEndTime )==FALSE)
			{
				//��������²�����
			}	
			
			// ����ʼʱ�� �ʼһ������
			if( pData->FreezeIndex == eFREEZE_MIN )
			{
				GetSectorInfo( pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex], pData->pAttCycleDepth->Depth, (pDataInfo->RecordNo-pDataInfo->SaveDot), &SectorInfo );
				FreezeRecordInfo.RecordLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//��������
				FreezeRecordInfo.dwBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];
				FreezeRecordInfo.MinPlan = pData->MinPlanIndex;
			}
			else
			{
				GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, (pDataInfo->RecordNo-pDataInfo->SaveDot), &SectorInfo );
				FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//��������
			}
			dwTimeAddr = dwBaseAddr + SectorInfo.dwAddr;
			FreezeRecordInfo.dwAddr = dwTimeAddr;//flash�洢��ַ
			FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
			FreezeRecordInfo.RecordNo = (pDataInfo->RecordNo-pDataInfo->SaveDot);//�����¼���
			
			if(ReadFreezeRecordTime( &FreezeRecordInfo,  (DWORD*)&dwStartTime  ) == FALSE)
			{
				//���������, ���������ĵ�һ�����ݷ���
				if(FreezeRecordInfo.FreezeIndex == eFREEZE_MIN)
				{
					TFreezeAddrLen FreezeAddrLen;
					TFreezeAttCycleDepth AttCycleDepth;

					//��� ���Ӷ��ᱸ������ �������
					GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen );
					if(api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * (FreezeRecordInfo.MinPlan + MAX_FREEZE_PLANNUM), sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth ) == TRUE)
					{
						if(pDataInfo->RecordNo >= AttCycleDepth.Depth)
						{
							IsTRUE = FALSE;
							FreezeRecordInfo.RecordNo = (pDataInfo->RecordNo - AttCycleDepth.Depth);
							ReadFreezeRecordTime( &FreezeRecordInfo,  (DWORD*)&dwStartTime  );
							RealSaveDot = AttCycleDepth.Depth;
						}
					}
				}
				else if(FreezeInfoTab[FreezeRecordInfo.FreezeIndex].wEeDepth != 0)
				{
					if(pDataInfo->RecordNo >= FreezeInfoTab[FreezeRecordInfo.FreezeIndex].wEeDepth)
					{
						IsTRUE = FALSE;
						FreezeRecordInfo.RecordNo = (pDataInfo->RecordNo - FreezeInfoTab[FreezeRecordInfo.FreezeIndex].wEeDepth);
						ReadFreezeRecordTime( &FreezeRecordInfo,  (DWORD*)&dwStartTime  );
						RealSaveDot = FreezeInfoTab[FreezeRecordInfo.FreezeIndex].wEeDepth;
					}
				}
			}

			dwTemp = 0;

			if( pData->pDLT698RecordPara->TimeOrLast <= dwStartTime )
			{
				//��Ҫ�ҵ�ʱ��С����ʼʱ�䣬�򽫵�һ�����ݷ���	
				if(IsTRUE == TRUE)
				{
					pData->RecordNo = (pDataInfo->RecordNo - pDataInfo->SaveDot);				// ��һ�����ݵ����
				}
				else
				{
					pData->RecordNo = FreezeRecordInfo.RecordNo;
				}
				
				memcpy((BYTE *)&(pData->SearchTime),(BYTE *)&dwStartTime,sizeof(DWORD));	// �����ҵ���ʱ��
				
			}
			else if( 	( pData->pDLT698RecordPara->TimeOrLast > dwStartTime ) 
					&& 	( pData->pDLT698RecordPara->TimeOrLast <= dwEndTime ) )
			{
				//���ҵ�ʱ������ʼʱ�������ʱ��֮�䣬����Ҫ���ַ�����
				if( pData->FreezeIndex == eFREEZE_MIN )
				{
					BinarySchInfo.wLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//��������;
					BinarySchInfo.MinBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];
					BinarySchInfo.MinPlan = pData->MinPlanIndex;
				}
				else
				{
					BinarySchInfo.wLen = pData->pAllInfo->AllAttDataLen;
				}
				BinarySchInfo.dwBaseAddr = dwBaseAddr;
				BinarySchInfo.dwDepth = pData->pAttCycleDepth->Depth;
				BinarySchInfo.dwRecordNo = pDataInfo->RecordNo;
				if( IsTRUE == TRUE )
				{
					BinarySchInfo.dwSaveDot = pDataInfo->SaveDot;
				}
				else
				{
					BinarySchInfo.dwSaveDot = RealSaveDot;
				}
				BinarySchInfo.Time = pData->pDLT698RecordPara->TimeOrLast;
				BinarySchInfo.pRecordNo = &dwRecordNo;
				BinarySchInfo.pTime = &dwTemp;
                BinarySchInfo.FreezeIndex = pData->FreezeIndex;
				if( BinSearchByTime( &BinarySchInfo ) == TRUE )
				{
					pData->RecordNo = dwRecordNo;											// �ҵ������
					memcpy((BYTE *)&(pData->SearchTime), (BYTE*)&dwTemp, sizeof(DWORD));	// �����ҵ���ʱ��
				}
				else
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}		
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��ָ��ʱ������
//
//����:            
//  pData[in]:	������Ϣ
//                
//  ����ֵ:	�������ݳ��� 0--����ռ䲻�� 0x8000--�쳣
//
//��ע:	
//-----------------------------------------------
WORD ReadFreezeByTime( TFreezeData	*pData )
{
	BOOL Result;
	
	Result = SearchFreezeRecordNoByTime(pData);
	
	if( Result == TRUE )
	{
		return ReadFreezeByRecordNo(pData);	
	}
	else
	{
		return 0x8000;
	}	
}

//-----------------------------------------------
//��������: ���ַ���Time��Ӧ�ĵ�ַ
//
//����: 
//	pBinSchInfo[in]:	ָ�򶳽����Ա��ָ��
//			DWORD		dwBaseAddr;			in �����Զ�Ӧ���ݵ��׵�ַ
//			DWORD		dwDepth;			in �������
//			DWORD		dwSaveDot;			in ��Ҫ����ʣ��Ķ����¼���������ж��ַ�����ʱ�õ�
//			DWORD		dwRecordNo;			in ���һ����ļ�¼��
//			DWORD		Time;				in Ҫ������ʱ��
//			DWORD		*pRecordNo;			out �����ҵ���RecordNo
//			DWORD		*pTime;				out �����ҵ���ʱ��
//			WORD		wLen;				in �����Ե������ݵĳ���
//                				
//����ֵ:	TRUE/FALSE
//
//��ע:	
//-----------------------------------------------
static WORD BinSearchByTime( TBinarySchInfo *pBinSchInfo )
{	
	DWORD dwTempTime,dwMidAddr;
	DWORD dwStartRecordNo,dwEndRecordNo,dwMidRecordNo;
	TSectorInfo	SectorInfo;
	TReadFreezeRecordInfo FreezeRecordInfo;
	WORD LoopNum;
	
	if( pBinSchInfo->dwRecordNo == 0 )
	{
		return FALSE;
	}
	
	if( pBinSchInfo->dwRecordNo < pBinSchInfo->dwSaveDot )
	{
		return FALSE;
	}
	
	//�ҽ�����¼��
	dwEndRecordNo = pBinSchInfo->dwRecordNo-1;
	
	//���㿪ʼ��¼��
	dwStartRecordNo = pBinSchInfo->dwRecordNo - pBinSchInfo->dwSaveDot;
	
	LoopNum = 0; 
	//���ַ�����
	while( dwStartRecordNo < dwEndRecordNo )
	{
		if((LoopNum++) >50)//��ֹ����while��,���Ӷ������28800�������Ҫ15�Σ�������
		{
			break;
		}
		//�����м��¼�ţ����ҵ���Ӧ��¼�����ݵ�ַ
		dwMidRecordNo = dwStartRecordNo + (dwEndRecordNo-dwStartRecordNo)/2;	
		GetSectorInfo( pBinSchInfo->wLen, pBinSchInfo->dwDepth, dwMidRecordNo, &SectorInfo );
		dwMidAddr = pBinSchInfo->dwBaseAddr+SectorInfo.dwAddr;
		FreezeRecordInfo.dwAddr = dwMidAddr;//flash�洢��ַ
		FreezeRecordInfo.RecordLen = pBinSchInfo->wLen;//��������
		FreezeRecordInfo.FreezeIndex = pBinSchInfo->FreezeIndex;//��������
		FreezeRecordInfo.RecordNo = dwMidRecordNo;//�����¼���
		FreezeRecordInfo.dwBakAddr = pBinSchInfo->MinBakAddr;
		FreezeRecordInfo.MinPlan = pBinSchInfo->MinPlan;
		//��ȡ�˼�¼�Ŷ�Ӧʱ��
		if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwTempTime )==FALSE)
		{
			if(dwStartRecordNo != dwMidRecordNo )
			{
				dwStartRecordNo = dwMidRecordNo ;//��֤�˻���һ������ƫ������֮һ����ȷ��--��ا��
			}
			else
			{
				dwStartRecordNo = dwEndRecordNo ;//��֤���ܹ������˳�--��ا��
			}
			continue;
		}
		if( dwTempTime == pBinSchInfo->Time )
		{
			//���ش����ݶ�Ӧ��¼�ż�ʱ��
			*pBinSchInfo->pRecordNo = dwMidRecordNo;
			*pBinSchInfo->pTime = dwTempTime;
			return TRUE;
		}
		else if( dwTempTime > pBinSchInfo->Time )
		{
			dwEndRecordNo = dwMidRecordNo;
		}
		else
		{
			if( dwMidRecordNo == dwStartRecordNo )
			{
				dwStartRecordNo = dwEndRecordNo;
			}
			else
			{  
				dwStartRecordNo = dwMidRecordNo ;
			}
		}
	}

	//�����ַ�û�ҵ���Ӧʱ�䣬��������ʼ��¼�Ŷ�Ӧʱ�����
	GetSectorInfo( pBinSchInfo->wLen, pBinSchInfo->dwDepth, dwStartRecordNo, &SectorInfo );
	dwMidAddr = pBinSchInfo->dwBaseAddr+SectorInfo.dwAddr;
	FreezeRecordInfo.dwAddr = dwMidAddr;//flash�洢��ַ
	FreezeRecordInfo.RecordLen = pBinSchInfo->wLen;//��������
	FreezeRecordInfo.FreezeIndex = pBinSchInfo->FreezeIndex;//��������
	FreezeRecordInfo.RecordNo = dwStartRecordNo;//�����¼���
	FreezeRecordInfo.dwBakAddr = pBinSchInfo->MinBakAddr;
	FreezeRecordInfo.MinPlan = pBinSchInfo->MinPlan;
	//��ȡ�˼�¼�Ŷ�Ӧʱ��
	if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwTempTime )==FALSE)
	{
		//���������������ȡ�������ݴ��������dwTempTime����¼����ɶֵ����ɶֵ--��ا��
	}
	//����ʱ�����Ҫ������ʱ�䣬�򷵻ش�ʱ���¼�ż����ݵ�ַ
	if( dwTempTime >= pBinSchInfo->Time )
	{
		*pBinSchInfo->pRecordNo = dwStartRecordNo;
		*pBinSchInfo->pTime = dwTempTime;
		return TRUE;
	}
	else
	{
		return FALSE;		
	}
}


//-----------------------------------------------
//��������: ���ռ��ʱ���ָ��ʱ������Ķ�������
//
//����:            
//  pData[in]:	������Ϣ
//                
//  ����ֵ:	�������ݳ��� 0--����ռ䲻�� 0x8000--�쳣
//
//��ע:	
//-----------------------------------------------
static WORD ReadFreezeByTimeInterval( TFreezeData *pData )
{
	BOOL Result;
	WORD wLen,ReturnLen,LoopNum,Mon;
	DWORD i,dwStartRecordNo,dwEndRecordNo;
	DWORD dwStartTime,dwEndTime;
	DWORD dwAddr,dwTimeInterval;
	TSectorInfo		SectorInfo;
	TRealTimer 	Time;
	TReadFreezeRecordInfo FreezeRecordInfo;
	//�������Զ��ᣬ�����Ϊ0����TI��Ϊ��������ʱ���ñ�־
	if( pData->FreezeIndex >= eFREEZE_HOUR )
	{
		//��AllCycle��־Ϊ55ʱ�����жϼ��
		if( pData->pDLT698RecordPara->TimeIntervalPara.AllCycle != 0x55 )
		{
			if( pData->pDLT698RecordPara->TimeIntervalPara.TI == 0 )
			{
				//��Ӧ�ó��������������Ϊ0ʱAllCycle��Ӧ��Ϊ55
				return 0x8000;
			}
		}
	}

	//����ʼʱ����ڽ�ֹʱ�䣬���ش���
	if( pData->pDLT698RecordPara->TimeIntervalPara.StartTime >= pData->pDLT698RecordPara->TimeIntervalPara.EndTime )
	{
		return 0x9000;
	}
	
	//�������Զ������������ʼʱ���Ϊ�ܱ����ʱ��������ʱ��
	if( pData->FreezeIndex < eFREEZE_HOUR )
	{
	}
	else
	{
		//�·�ʱ��������Ϊ0ʱ ���Ӽ�����շ��Ӷ������ڣ��������ڶ���Ĭ��Ϊ1
		if( pData->pDLT698RecordPara->TimeIntervalPara.AllCycle == 0x55 )
		{
			if( pData->FreezeIndex == eFREEZE_MIN )
			{
				pData->pDLT698RecordPara->TimeIntervalPara.TI = pData->pAttCycleDepth->Cycle;
			}
			else
			{
				pData->pDLT698RecordPara->TimeIntervalPara.TI = 1;
			}
		}
		
		//���ڶ���Ϊ���� Сʱ �� �¶���
		dwTimeInterval = 1;

		if( pData->FreezeIndex == eFREEZE_MON )
		{
			api_ToAbsTime( pData->pDLT698RecordPara->TimeIntervalPara.StartTime, &Time );
			Mon = (Time.wYear-2000)*12+Time.Mon-1;
			//�������²������£�����Ϊ��һ������ʱ��
			if(((Mon % pData->pDLT698RecordPara->TimeIntervalPara.TI) != 0)||
			  (Time.Day != 0x01)||(Time.Hour != 0x00)||(Time.Min != 0x00) )
			{
				Mon = (((Mon/pData->pDLT698RecordPara->TimeIntervalPara.TI)+1)*pData->pDLT698RecordPara->TimeIntervalPara.TI);
				Time.wYear = (2000+(Mon/12));//���¼�����
				Time.Mon = (Mon % 12)+1;
				Time.Day = 0x01;
				Time.Hour = 0x00;
				Time.Min = 0x00;
				pData->pDLT698RecordPara->TimeIntervalPara.StartTime = api_CalcInTimeRelativeMin( &Time );
			}
		}
		else
		{
			//��������ʱ�䲻�ܱ����ʱ��ķ���������������Ϊ��һ���ܱ����ʱ��������ʱ��
			if( pData->FreezeIndex == eFREEZE_MIN )
			{
				dwTimeInterval = pData->pDLT698RecordPara->TimeIntervalPara.TI;
			}
			else if( pData->FreezeIndex == eFREEZE_HOUR )
			{
				dwTimeInterval = 60L * pData->pDLT698RecordPara->TimeIntervalPara.TI;
			}
			else if( pData->FreezeIndex == eFREEZE_DAY )
			{
				dwTimeInterval = 1440L * pData->pDLT698RecordPara->TimeIntervalPara.TI;
			}
			
			if( (pData->pDLT698RecordPara->TimeIntervalPara.StartTime % dwTimeInterval) != 0 )
			{
				pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime / dwTimeInterval;
				pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime * dwTimeInterval;
				pData->pDLT698RecordPara->TimeIntervalPara.StartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime + dwTimeInterval;
			}
		}				
	}
	
	//������ʼʱ���Ӧ��RecordNo
	dwStartTime = pData->pDLT698RecordPara->TimeIntervalPara.StartTime;
	pData->pDLT698RecordPara->TimeOrLast = pData->pDLT698RecordPara->TimeIntervalPara.StartTime;	
	Result = SearchFreezeRecordNoByTime(pData);
	if( Result == FALSE )
	{
		return 0x8000;
	}
	dwStartRecordNo = pData->RecordNo;	
    
	//���ҽ�ֹʱ���Ӧ��RecordNo
	pData->pDLT698RecordPara->TimeOrLast = pData->pDLT698RecordPara->TimeIntervalPara.EndTime;	
	Result = SearchFreezeRecordNoByTime(pData);
	if(Result == TRUE)
	{
    	if( pData->RecordNo == dwStartRecordNo)//�����ʼ�ͽ���ʱ��������ͬһ���� ����ʧ��
    	{
            return 0x8000;
    	}
	}
	
	if( Result == FALSE )
	{
		//�����Ҳ�����һ���Ǵ��ڵ�ǰʱ�䣩��������һ����¼��
        if( pData->pDataInfo->RecordNo == 0 )
        {
            dwEndRecordNo = 0;
        }
        else
        {
            dwEndRecordNo = pData->pDataInfo->RecordNo-1;
        }
	}
	else
	{
		//����ʱ��Ҫ��ǰ�պ�
		if( pData->SearchTime < pData->pDLT698RecordPara->TimeIntervalPara.EndTime )
		{
			dwEndRecordNo = pData->RecordNo;
		}
		else
		{
			if( pData->RecordNo == 0 )
			{
				return 0x8000;
			}
			else
			{
				dwEndRecordNo = pData->RecordNo-1;
			}
		}
	}
	
	ReturnLen = 0;
	pData->pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum = 0;
	
	//�������Բ��жϼ��ʱ��   ���·���TIΪ0ʱ�м�����¼���ؼ�����¼
	if( (pData->FreezeIndex<eFREEZE_HOUR) || (pData->pDLT698RecordPara->TimeIntervalPara.AllCycle==0x55) )
	{
		for(i=dwStartRecordNo; i<=dwEndRecordNo; i++)
		{
			pData->RecordNo = i;		
			wLen = ReadFreezeByRecordNo(pData);		
			if( wLen == 0x8000 )
			{
				return 0x8000;
			}
			else if( wLen == 0 )
			{
				//��һ�����ݳ��ȾͲ���������8000
				if( i == dwStartRecordNo )
				{
					return 0;
				}
				else
				{
					break;
				}
			}
			else
			{
				pData->pBuf += wLen;
				ReturnLen += wLen;
				pData->Len -= wLen;
				pData->pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum++;
			}
		}
	}
	else
	{
		//�������ն����������Ϊ�����������ʱ���Ȱ���ʱ���ҳ�recordno���ҿ��ҳ���ʱ���Ƿ���Ҫ������ʱ�䣬���ǣ��򷵻����ݣ������ǣ�����null
		if( pData->FreezeIndex == eFREEZE_MIN )
		{
			//��ȡ�������ݵĵ�ַ
			if(GetSectorInfo( pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex], pData->pAttCycleDepth->Depth, dwEndRecordNo, &SectorInfo ) == FALSE )
			{
				return 0x8000;
			}
			
			dwAddr = pData->pMinInfo->DataAddr[pData->MinPlanIndex]+SectorInfo.dwAddr;
			FreezeRecordInfo.RecordLen = pData->pMinInfo->AllAttDataLen[pData->MinPlanIndex];//��������
			FreezeRecordInfo.dwBakAddr = pData->pMinInfo->DataBakAddr[pData->MinPlanIndex];
			FreezeRecordInfo.MinPlan = pData->MinPlanIndex;
		}
		else
		{
			GetSectorInfo( pData->pAllInfo->AllAttDataLen, pData->pAttCycleDepth->Depth, dwEndRecordNo, &SectorInfo );
			dwAddr = pData->pFreezeAddrLen->dwDataAddr+SectorInfo.dwAddr;
			FreezeRecordInfo.RecordLen = pData->pAllInfo->AllAttDataLen;//��������
		}
		FreezeRecordInfo.dwAddr = dwAddr;//flash�洢��ַ
		FreezeRecordInfo.FreezeIndex = pData->FreezeIndex;//��������
		FreezeRecordInfo.RecordNo = dwEndRecordNo;//�����¼���
		//��ȡ�˼�¼�Ŷ�Ӧʱ��
		if(ReadFreezeRecordTime( &FreezeRecordInfo, (DWORD*)&dwEndTime )==FALSE)
		{
			//��������²�����
		}
		
		LoopNum = 0;
		i = dwStartTime;
		while(i<=dwEndTime)
		{
			CLEAR_WATCH_DOG;
			//��ֹѭ��������
			LoopNum++;
			if(LoopNum > 500)
			{
				break;
			}
			pData->pDLT698RecordPara->TimeOrLast = i;
			SearchFreezeRecordNoByTime(pData);
			if( pData->SearchTime > dwEndTime )//��������ʱ��Ƚ���ʱ���
			{
				break;
			}
			if( pData->FreezeIndex == eFREEZE_MON )//�¶���
			{
				api_ToAbsTime( pData->SearchTime, &Time );
				Mon = (Time.wYear-2000)*12+Time.Mon-1;
				if( (pData->SearchTime != i)//������ָ��ʱ�� 
				&& ((Mon % pData->pDLT698RecordPara->TimeIntervalPara.TI) != 0) )//��������ʱ��ɱ�ʱ��������              
				{
					Mon = (((Mon/pData->pDLT698RecordPara->TimeIntervalPara.TI)+1)*pData->pDLT698RecordPara->TimeIntervalPara.TI);
					Time.wYear = (2000+(Mon/12));//���¼�����
					Time.Mon = (Mon % 12)+1;
					i  = api_CalcInTimeRelativeMin( &Time );
					continue;
				}
                else
                {
                    i = pData->SearchTime;
                }
			}
			else
			{
			//���û���ҵ���Ӧ��ʱ�䣬���ҵ���ʱ�丳����ǰ����ʱ�������������
				if(( pData->SearchTime != i ) && ((pData->SearchTime % dwTimeInterval) != 0) )//��������ʱ��ɱ�ʱ��������
				{
					i = ((pData->SearchTime/dwTimeInterval)+1)*dwTimeInterval;
					continue;
				}
				else
				{
					i = pData->SearchTime;
				}

			}
			wLen = ReadFreezeByRecordNo(pData);
			if( wLen == 0x8000 )
			{
				return 0x8000;
			}
			else if( wLen == 0 )
			{
				//��һ�����ݳ��ȾͲ���������8000
				if( pData->RecordNo == dwStartRecordNo )
				{
					return 0;
				}
				else
				{
					break;
				}
			}
			else
			{
				if( pData->Len < wLen )//��pData->Len -= wLen֮ǰ�����жϣ����ʱ��ζ�ȡʱ�����pData->LenתȦ����buf���޵�����
				{
					break;
				}
				pData->pBuf += wLen;
				ReturnLen += wLen;
				pData->Len -= wLen;
				pData->pDLT698RecordPara->TimeIntervalPara.ReturnFreezeNum++;
				
			}
			if( pData->FreezeIndex != eFREEZE_MON )
			{
				i += dwTimeInterval;
			}
			else
			{
				api_ToAbsTime(i, &Time);
				Mon = (Time.wYear-2000)*12+(Time.Mon-1);
				
				Mon += pData->pDLT698RecordPara->TimeIntervalPara.TI;
				Time.wYear = Mon/12+2000;
				Time.Mon = Mon%12 + 1;
				i = api_CalcInTimeRelativeMin(&Time);
			}
		}		
	}
	
	if( ReturnLen != 0 )
	{
		return ReturnLen;
	}
	else
	{
		//û���ҵ�������8000
		return 0x8000;
	}
}

//-----------------------------------------------
//��������: �������¼
//
//����: 
//	Tag[in]:	0 ����Tag 				1 ��Tag                
//	pDLT698RecordPara[in]: ��ȡ����Ĳ����ṹ��                
//  Len[in]�������buf���� �����ж�buf�����Ƿ���                
//  pBuf[out]: �������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0
//				
//	
//����ֵ:	bit15λ��1 �������޴����� ��0������������ȷ���أ� 
//			bit0~bit14 �����صĶ������ݳ���
//
//��ע:	
//-----------------------------------------------
WORD api_ReadFreezeRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf )
{
	BYTE i,tFreezeIndex,PlanIndex;
	DWORD dwOad,dwTempOad;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE+1];
	TFreezeAttCycleDepth MinAttCycleDepth[MAX_FREEZE_PLANNUM];
	TFreezeAttCycleDepth AttCycleDepth;
	TFreezeDataInfo	DataInfo;
	TFreezeData 	Data;
	TFreezeAddrLen FreezeAddrLen;
    TFreezeAllInfoRom FreezeAllInfo;
    TFreezeMinInfo FreezeMinInfo;
    
	PlanIndex = 0;
	memset( &FreezeMinInfo, 0x00, sizeof(FreezeMinInfo) );
	
	tFreezeIndex = api_GetFreezeIndex( pDLT698RecordPara->OI );
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
        return 0x8000;
	}

	// ��ø��ֵ�ַ
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}
	
	//������������ĳ��ȼ��������Եĸ���
	if( api_VReadSafeMem(FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo) != 	TRUE )
	{
		return 0x8000;
	}	

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return 0x8000;
	}
	
	//�������еĹ������Զ���
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad)*FreezeInfoTab[tFreezeIndex].MaxAttNum+sizeof(DWORD), (BYTE *)&AttOad[0]);		
	
	if( tFreezeIndex == eFREEZE_MIN )
	{
		//�������еĹ������Զ��� �������
		for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
		{
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&MinAttCycleDepth[i] );
		}
		
		//���Ӷ����·���RCSD����Ϊ0
		if( pDLT698RecordPara->OADNum == 0 )
		{
			return 0x9000;
		}
		//�ж��·���OAD�Ƿ�Ϊͬһ��������Ҫ�ж�Ԫ������Ϊ0��Ҫ���Ĳ�Ϊ0�����
		PlanIndex = 0xff;//�·�OAD�ķ�����
		for(i=0; i<pDLT698RecordPara->OADNum; i++)
		{
			memcpy( (BYTE*)&dwOad, pDLT698RecordPara->pOAD+sizeof(DWORD)*i, sizeof(DWORD) );
			dwTempOad = dwOad & (~0x00E00000);	
			if((dwTempOad == FREEZE_RECORD_NO_OAD) || (dwTempOad == FREEZE_RECORD_TIME_OAD))
			{
				if(CURR_PLAN(dwOad) == 0)
				{
					continue;
				}
			}
			if( PlanIndex== 0xff )
			{
				PlanIndex = CURR_PLAN(dwOad);
			}
			else
			{
				if( PlanIndex != CURR_PLAN(dwOad) )
				{
					return 0x9000;
				}	
			}
		}
		
		//��Ҫ��ȡ��ֻ��RecordNo��Time��Ĭ�ϰ�����0����
		if(PlanIndex == 0xff)
		{
			PlanIndex= 0;
		}
		
		//��ȡ���Ӷ����ַ��Ϣ
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo ); 
		if( FreezeMinInfo.DataAddr[0] != FreezeAddrLen.dwDataAddr )//����0��ַ �����ַһ��
		{
			FreezeMinInfo.DataAddr[0] = FreezeAddrLen.dwDataAddr;
		}

		//���ҷ��Ӷ����·��Ķ����Ӧ��RecordNo		
		memcpy( (BYTE *)&DataInfo, (BYTE *)&MinInfo[PlanIndex], sizeof(TFreezeDataInfo) );
		//���ҷ��Ӷ����·��Ķ����Ӧ���������		
		memcpy( (BYTE *)&AttCycleDepth, (BYTE *)&MinAttCycleDepth[PlanIndex], sizeof(AttCycleDepth) );
	}
	else
	{
		//�������еĹ������Զ��� �������
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth );
		//����������RecordNo
		if( api_VReadSafeMem(FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo) != TRUE )
		{
			return 0x8000;
		}
	}
	
	//��û�в�������
	if( DataInfo.RecordNo == 0 )
	{
		return 0x8000;
	}

	Data.MinPlanIndex = PlanIndex;	//ֻ�Է��Ӷ�������
	Data.pDLT698RecordPara = pDLT698RecordPara;
	Data.pAttOad = &AttOad[0];
	Data.pAttCycleDepth = &AttCycleDepth;
	Data.pDataInfo = &DataInfo;
	Data.pMinInfo = &FreezeMinInfo;
	Data.Tag = Tag;
	Data.pBuf = pBuf;
	Data.pFreezeAddrLen = &FreezeAddrLen;
	Data.pAllInfo = &FreezeAllInfo;
	Data.Len = Len;
	Data.pTime = NULL;
	Data.FreezeIndex = tFreezeIndex;

	if( pDLT698RecordPara->eTimeOrLastFlag == eFREEZE_TIME_FLAG )
	{
		//RSD ����1������ʱ������
		return ReadFreezeByTime( &Data );
	}
	else if( pDLT698RecordPara->eTimeOrLastFlag == eNUM_FLAG )
	{
		//RSD ����9�����մ�������
		return ReadFreezeByLastNum( &Data );
	}
	else if( (pDLT698RecordPara->eTimeOrLastFlag == eRECORD_TIME_INTERVAL_FLAG) 
	||	( pDLT698RecordPara->eTimeOrLastFlag == (eRECORD_TIME_INTERVAL_FLAG+eFREEZE_TIME_FLAG) ))
	{
		//RSD ����2������ʱ����������
		return ReadFreezeByTimeInterval( &Data );			
	}
	else
	{
		return 0x8000;
	}
}
//-----------------------------------------------
//��������: ������紦��
//
//����: 
//	��
//	
//  ����ֵ:	
//  ��
//
//��ע:	 ����192�ֽ�
//-----------------------------------------------
void api_PowerDownFreeze( void )
{
	BYTE i;
	BYTE Buf[sizeof(MinInfo)+sizeof(DWORD)];
	
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		// ���RAM��У��
		if( lib_CheckSafeMemVerify( (BYTE *)&MinInfo[i], sizeof(TFreezeDataInfo), UN_REPAIR_CRC ) == FALSE )
		{
			return;
		}
	}
	memcpy(Buf,&MinInfo[0],sizeof(MinInfo));
	// дEEP
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeInfoRom.MinDataInfo[0] ), sizeof(MinInfo)+sizeof(DWORD), Buf );
	
	memset( (BYTE*)&CompleteFreezeTime, 0x00, sizeof(TCompleteFreezeTime) );
	CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
}
//-----------------------------------------------
//��������: ���µ���ʱ�䱸��
//
//����: 
//	��
//	
//  ����ֵ:	
//  ��
//
//��ע:	 
//-----------------------------------------------
void api_ReflashFreeze_Ram( void )
{
	TRealTimer		PowerDownTime;
	
	//��ȡ����ʱ��
	api_GetPowerDownTime( (TRealTimer*)&PowerDownTime );
	memcpy( (BYTE*)&FreezePowerDownTimeBak, (BYTE*)&PowerDownTime, sizeof(TRealTimer) );//���±��ݵ��ʱ��
	//����һ�β�����ʱ������
	memset( (BYTE*)&CompleteFreezeTime, 0x00, sizeof(TCompleteFreezeTime) );
	CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
}

//-----------------------------------------------
//��������: �ϵ粹�ն���
//
//����: 	PowerDownTime[in] ����ʱ��
//	
//����ֵ:	��
//
//��ע:	 
//-----------------------------------------------
static BYTE PowerUpCompleteFreezeDay( TRealTimer PowerDownTime )
{
	BYTE i,m;
	BYTE TmpOADBuf[10];
	WORD wDownDay,wOnDay,Len;
	DWORD dwClosingTime,dwStartFreezeTime,dwLastFreezeTime,dwPowerDownTime;
	DWORD FreezeTime[7];
	TRealTimer tStartFreezeTime,DayClosingTime,LastFreezeTime;
	TBillingPara	BillingPara;
	TDLT698RecordPara TmpDLT698RecordPara;
	
	CLEAR_WATCH_DOG;
	
	//��ȡ��һ���ն���ʱ��
	memset( (BYTE *)&LastFreezeTime, 0xff, sizeof(TRealTimer) ); 
	TmpOADBuf[0] = 0x20;
	TmpOADBuf[1] = 0x21;
	TmpOADBuf[2] = 0x02;
	TmpOADBuf[3] = 0x00;
	TmpDLT698RecordPara.pOAD = TmpOADBuf;//pOADָ��
	TmpDLT698RecordPara.OADNum = 1;		//OAD����
	TmpDLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;//ѡ�����
	TmpDLT698RecordPara.TimeOrLast = 0x01;	//��һ��
	TmpDLT698RecordPara.Phase = ePHASE_ALL;
	TmpDLT698RecordPara.OI = 0x5004;					
	Len = api_ReadProRecordData( 0, &TmpDLT698RecordPara, sizeof(TRealTimer), (BYTE*)&LastFreezeTime );

	//����ܹ����ص���һ���ն���ʱ�䣬����һ���ն���ʱ��
	//û���ܹ������ص���һ���ն���ʱ����߶���ʱ��Ƿ����õ���ʱ��
	if( (Len == 1) || (api_CheckClock(&LastFreezeTime) == FALSE) )
	{
		if( api_CheckClock(&PowerDownTime) == FALSE )
		{
			return TRUE;
		}
		else
		{
			memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));	
		}
	}
	else
	{
		dwLastFreezeTime = api_CalcInTimeRelativeMin(&LastFreezeTime);
		dwPowerDownTime = api_CalcInTimeRelativeMin(&PowerDownTime);
		if( (dwLastFreezeTime > dwPowerDownTime ) //��һ���ն���ʱ����ڵ���ʱ�䣬����һ���ն���ʱ��
		|| ( dwPowerDownTime == 0xFFFFFFFF ))//����ʱ��ʱ��Ƿ�������һ���ն���ʱ��
		{
			memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&LastFreezeTime, sizeof(TRealTimer));
		}
		else
		{
			memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));	
		}
		
	}
	if( api_CheckClock(&PowerDownTime) == FALSE )
	{
		memcpy( (void*)&DayClosingTime, (void*)&tStartFreezeTime, sizeof(TRealTimer) );//��ȡ����ʱ��
	}
	else
	{
		memcpy( (void*)&DayClosingTime, (void*)&PowerDownTime, sizeof(TRealTimer) );//��ȡ����ʱ��
	}

	//��ʼ������ʱ����Է�����
	dwStartFreezeTime = api_CalcInTimeRelativeMin(&tStartFreezeTime);
	
	#if( SEL_STAT_V_RUN == YES )
	if( tStartFreezeTime.Mon != RealTimer.Mon )//�����µ�ѹ�ϸ���
	{
		api_ClearVRunTimeNow( BIT1 );
	}
	#endif

	//��ȡ������ʧ����1��0ʱ
	if( GetClosingTime( &BillingPara ) == FALSE )
	{
		BillingPara.MonSavePara[0].Day = 1;
		BillingPara.MonSavePara[0].Hour = 0;
	}
	DayClosingTime.Day = BillingPara.MonSavePara[0].Day;
	DayClosingTime.Hour =BillingPara.MonSavePara[0].Hour;
	DayClosingTime.Min = 0;
	dwClosingTime = api_CalcInTimeRelativeMin(&DayClosingTime);//���㵱ǰ�½�������Է�����

	
	if( dwStartFreezeTime >= dwClosingTime )//��ʼ������ʱ�����ǰ������
	{
		if( DayClosingTime.Mon == 12 )//�½�λ
		{
			DayClosingTime.wYear += 1;
			DayClosingTime.Mon = 1;
		}
		else
		{
			DayClosingTime.Mon += 1;
		}
		dwClosingTime = api_CalcInTimeRelativeMin(&DayClosingTime);//������һ�½�������Է�����

	}
	
	//��ʼ�����������
	wDownDay = dwStartFreezeTime/1440;
	//�ϵ������
	wOnDay = g_RelativeMin/1440;
	
    #if( SEL_STAT_V_RUN == YES )
	if( (wOnDay-wDownDay) > 7 )//����7�������յ�ѹ�ϸ���
	{
		api_ClearVRunTimeNow( BIT0 );
	}
    #endif
	//����������һ�����ն��᲻�ж������ڣ�ÿ�ζ���
	memset(&FreezeTime[0], 0x00, sizeof(FreezeTime));
		
	//���Ҵ�������Ҫ���ն���Ĵ�������Է�����������С�����������7��
	for(i = 0;i < 7;i++)
	{
		if( wOnDay <= wDownDay )
		{
			break;
		}
		FreezeTime[6-i] = (DWORD)wOnDay*1440;
		
		wOnDay--;
	}
	m=0;
	//��ʼ���ն���
	for(i=0; i<7; i++)
	{
		if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
		{	
			ClrFreezeFlag(eFREEZE_MAX_NUM);
			return FALSE;
		}
		
		if(FreezeTime[i] == 0)
		{
			continue;
		}	
		CLEAR_WATCH_DOG;
		if( FreezeTime[i] > dwClosingTime )//��������
		{
			ClrFreezeFlag(eFREEZE_MAX_NUM);
		}
		else
		{
			SetFreezeFlag(eFREEZE_MAX_NUM);
		}
		PowerUpFreezeMin = FreezeTime[i];
		ProcFreezeTask(eFREEZE_DAY,0x55);
		if( m == 0 )//������ն�����յ�ѹ�ϸ�������
		{  
            #if( SEL_STAT_V_RUN == YES )
			api_ClearVRunTimeNow( BIT0 );
            #endif
			m=0x55;
		}
		
	}
	return TRUE;
}

//-----------------------------------------------
//��������: �ϵ�����ն���
//
//����: 	PowerDownTime[in] ����ʱ��
//	
//����ֵ:	��
//
//��ע:	 
//-----------------------------------------------
static BYTE PowerUpCompleteFreezeClosing( TRealTimer PowerDownTime )
{
	BYTE i,j,k,Num,Flag;
	BYTE ClosingFlag[12],ClosingSn[3];
	WORD wDownMonNum,wUpMonNum;
	DWORD dwDownTime,dwFreezeTime, dwClosingTime, dwStartFreezeTime, dwLastFreezeTime ;
	DWORD FreezeTime[12];
	TRealTimer		tStartFreezeTime, DayClosingTime;
	TBillingPara	BillingPara;
	
	CLEAR_WATCH_DOG;
	
	//����ʱ��Ƿ��������в������˳�
	if( api_CheckClock(&PowerDownTime) == FALSE )
	{
		return TRUE;
	}
	if( api_CheckClock(&CompleteFreezeTime.CompleteFreezeClosingTime) == FALSE )
	{
		dwLastFreezeTime = 0;
	}
	else
	{
		dwLastFreezeTime = api_CalcInTimeRelativeMin(&CompleteFreezeTime.CompleteFreezeClosingTime);
	}
	//�������ʱ����Է�����
	dwDownTime = api_CalcInTimeRelativeMin(&PowerDownTime);
	//��һ�ν����ն���ʱ��С�ڵ���ʱ�䣬�ӵ���ʱ�俪ʼ����
	//��һ�ν����ն���ʱ���ڵ���ʱ���뵱ǰʱ��֮�䣬����һ�ν����ն���ʱ�俪ʼ����
	//��һ�ν����ն���ʱ����ڵ�ǰʱ�䣬�ӵ���ʱ�俪ʼ����
	if(dwLastFreezeTime < dwDownTime )
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));
	}
	else if(dwLastFreezeTime <= g_RelativeMin )	
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&CompleteFreezeTime.CompleteFreezeClosingTime, sizeof(TRealTimer));
	}
	else
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));
	}

	memcpy( (void*)&DayClosingTime, (void*)&PowerDownTime, sizeof(TRealTimer) );//��ȡ����ʱ��
	
	//��ʼ��������Է�����
	dwStartFreezeTime = api_CalcInTimeRelativeMin(&tStartFreezeTime);
	//����ʱ���������
	wDownMonNum = (tStartFreezeTime.wYear%2000)*12+tStartFreezeTime.Mon-1;
	//�ϵ�ʱ���������
	wUpMonNum = (RealTimer.wYear%2000)*12+RealTimer.Mon-1;
	
	//�����12�������� ��ȡ������
	if( GetClosingTime( &BillingPara ) == FALSE )
	{
		return TRUE;
	}

	DayClosingTime.Day = BillingPara.MonSavePara[0].Day;
	DayClosingTime.Hour =BillingPara.MonSavePara[0].Hour;
	DayClosingTime.Min = 0;
	dwClosingTime = api_CalcInTimeRelativeMin(&DayClosingTime);//���㵱ǰ�½�������Է�����

	
	if( dwStartFreezeTime >= dwClosingTime )//��ʼ����ʱ�����ǰ������
	{
		
		if( DayClosingTime.Mon == 12 )//�½�λ
		{
			DayClosingTime.wYear += 1;
			DayClosingTime.Mon = 1;
		}
		else
		{
			DayClosingTime.Mon += 1;
		}
		dwClosingTime = api_CalcInTimeRelativeMin(&DayClosingTime);//������һ�½�������Է�����
	}
	// �����գ�ʱ����ð������(0xFF�ŵ����)
	ClosingSn[0] = 0;
	ClosingSn[1] = 1;
	ClosingSn[2] = 2;
	for( i = (MAX_MON_CLOSING_NUM-1); i > 0; i-- )
	{
		Flag = 0;
		for( k = 0; k < i; k++ )
		{
			if( (BillingPara.MonSavePara[k].Day > BillingPara.MonSavePara[k+1].Day)||
				( (BillingPara.MonSavePara[k].Day == BillingPara.MonSavePara[k+1].Day)&&(BillingPara.MonSavePara[k].Hour > BillingPara.MonSavePara[k+1].Hour)))
			{
				// ����
				j = BillingPara.MonSavePara[k].Day;
				BillingPara.MonSavePara[k].Day = BillingPara.MonSavePara[k+1].Day;
				BillingPara.MonSavePara[k+1].Day = j;

				j = BillingPara.MonSavePara[k].Hour;
				BillingPara.MonSavePara[k].Hour = BillingPara.MonSavePara[k+1].Hour;
				BillingPara.MonSavePara[k+1].Hour = j;
				
				j = ClosingSn[k];
				ClosingSn[k] = ClosingSn[k+1];
				ClosingSn[k + 1] = j;

				Flag = 1;
			}
		}
		if( Flag == 0 )
		{
			break;			// �޽���
		}
	}

	//����һ�¼��ν���
	Num = 0;
	for( i = 0; i < 3; i++)
	{
		if( (BillingPara.MonSavePara[i].Day == 0xFF) || (BillingPara.MonSavePara[i].Hour == 0xFF) )
		{
		
		}
		else if( (BillingPara.MonSavePara[i].Day == 99) || (BillingPara.MonSavePara[i].Hour == 99) )
		{
		
		}
		else//��������ͬ�������޳�
		{
			if( i == 0 )
			{
				Num++;
			}
			else if( i == 1 )
			{
				if( (BillingPara.MonSavePara[1].Day == BillingPara.MonSavePara[0].Day) 
				&& ((BillingPara.MonSavePara[1].Hour == BillingPara.MonSavePara[0].Hour)))
				{

				}
				else
				{
					Num++;
				}
			}
			else if( i == 2 )
			{
				if( (BillingPara.MonSavePara[2].Day == BillingPara.MonSavePara[1].Day) 
				&& ((BillingPara.MonSavePara[2].Hour == BillingPara.MonSavePara[1].Hour)))
				{
				
				}
				else if( (BillingPara.MonSavePara[2].Day == BillingPara.MonSavePara[1].Day) 
				&& ((BillingPara.MonSavePara[2].Hour == BillingPara.MonSavePara[1].Hour)))
				{
				
				}
				else
				{
				Num++;
				}

			}
		}
	}

	memset( &FreezeTime[0], 0x00, sizeof(FreezeTime) );
	
	//������ʱ�䳬��12��������ʱ���޸ĵ���ʱ��Ϊ���13�������յ��¡�
	if( (wUpMonNum-wDownMonNum)*Num > 12 )
	{
		wDownMonNum = wUpMonNum-(12/Num+1);
		tStartFreezeTime.wYear = wDownMonNum/12+2000;
		tStartFreezeTime.Mon = wDownMonNum%12+1;
	}
	
	memset(ClosingFlag,0xff,12);
	
	// ��һ��һ��ʱ���������13���£���һ������ʱ���������7���£���һ������ʱ���������5����
	for( k = 0; k < (12/Num+2); k++ )
	{
		// һ����num��������
		for( i = 0; i < Num; i++ )
		{
			// �£��걣�ֲ���
			tStartFreezeTime.Sec = 0;
			tStartFreezeTime.Min = 0;
			tStartFreezeTime.Hour = BillingPara.MonSavePara[i].Hour;
			tStartFreezeTime.Day = BillingPara.MonSavePara[i].Day;

			dwFreezeTime = api_CalcInTimeRelativeMin( &tStartFreezeTime );

			if( (dwFreezeTime>dwStartFreezeTime) && (dwFreezeTime<=g_RelativeMin) )
			{
				// ����12���µĽ���ʱ��
				memmove( &FreezeTime[0], &FreezeTime[1], 11*sizeof(DWORD) );
				memcpy( &FreezeTime[11], &dwFreezeTime, sizeof(DWORD) );
				memcpy( ClosingFlag, &ClosingFlag[1], 11 );
				ClosingFlag[11] = ClosingSn[i];
			}
			else if( dwFreezeTime > g_RelativeMin )
			{
				k = 12/Num+1;
				break;
			}
		}
		
		//����һ������
		wDownMonNum += 1;
		tStartFreezeTime.wYear = wDownMonNum/12+2000;
		tStartFreezeTime.Mon = wDownMonNum%12+1;
	}
	Flag = 0;
	if (FreezeTime[0] > dwClosingTime  )	
	{						
		#if( MAX_PHASE == 3 )
		#if( SEL_DEMAND_2022 == NO )
		api_ClrDemand(eClearAllDemand);
		#else
		api_ClrDemand(eClearAllDemand,eDemandRun);
		#endif
		#endif
		// �½�����ܣ��������ۼƵ��ܼ���	
		api_ClosingMonEnergy();	
		Flag = 0x55;
	}
	for( i = 0; i < 12; i++ )
	{
		if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
		{
			ClrFreezeFlag(eFREEZE_MAX_NUM);
			return FALSE;
		}
		if( FreezeTime[i] == 0x00 )
		{			
			continue;
		}	
		CLEAR_WATCH_DOG;		
		//���ʱ��
		PowerUpFreezeMin=FreezeTime[i];
		
		//������
		if( (ClosingFlag[i]==1) || (ClosingFlag[i]==2) )
		{						
			ClrFreezeFlag(eFREEZE_MAX_NUM);
		}
		else
		{
			SetFreezeFlag(eFREEZE_MAX_NUM);
		}
		ProcFreezeTask(eFREEZE_CLOSING,0x55);
		//��¼�˴�����ʱ��
		api_ToAbsTime(PowerUpFreezeMin, &CompleteFreezeTime.CompleteFreezeClosingTime);
		CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
		
		if ((FreezeTime[i] >= dwClosingTime ) && (Flag == 0) )	
		{						
			#if( MAX_PHASE == 3 )
			#if( SEL_DEMAND_2022 == NO )
			api_ClrDemand(eClearAllDemand);
			#else
			api_ClrDemand(eClearAllDemand,eDemandRun);
			#endif
			#endif
			// �½�����ܣ��������ۼƵ��ܼ���	
			api_ClosingMonEnergy();	
			Flag = 0x55;
		}
	}
	ClrFreezeFlag(eFREEZE_MAX_NUM);
	return TRUE;
}	

//-----------------------------------------------
//��������: �ϵ粹���ݽ���
//
//����: 	PowerDownTime[in] ����ʱ��
//	
//����ֵ:	��
//
//��ע:	 
//-----------------------------------------------
#if( PREPAY_MODE == PREPAY_LOCAL )
static BYTE PowerUpCompleteFreezeLadderClosing( TRealTimer PowerDownTime  )
{
	
	BYTE i,j,m,Num,Flag;
	WORD wDownMonNum,wUpMonNum;
	DWORD dwDownTime,dwFreezeTime,dwStartFreezeTime, dwLastFreezeTime;
	DWORD FreezeTime[12];
	TRealTimer		tPowerDownTime ,tStartFreezeTime;	
	DWORD dwFreezeTime1;//����ʱ����
	TLadderBillingPara LadderBillingPara;
	BYTE LadderMode; //���ݽ���ģʽ��0x00�޽��ݽ��㣬0x55�½��㣬0xAA����㡣
	
	//����ʱ��Ƿ��������в������˳�
	if( api_CheckClock(&PowerDownTime) == FALSE )
	{
		return TRUE;
	}
	CLEAR_WATCH_DOG;
	
	if( api_CheckClock(&CompleteFreezeTime.CompleteFreezeLadderClosingTime) == FALSE )
	{
		dwLastFreezeTime = 0;
	}
	else
	{
		dwLastFreezeTime = api_CalcInTimeRelativeMin(&CompleteFreezeTime.CompleteFreezeLadderClosingTime);
	}
	//�������ʱ����Է�����
	dwDownTime = api_CalcInTimeRelativeMin(&PowerDownTime);
	//��һ�ν����ն���ʱ��С�ڵ���ʱ�䣬�ӵ���ʱ�俪ʼ����
	//��һ�ν����ն���ʱ���ڵ���ʱ���뵱ǰʱ��֮�䣬����һ�ν����ն���ʱ�俪ʼ����
	//��һ�ν����ն���ʱ����ڵ�ǰʱ�䣬�ӵ���ʱ�俪ʼ����
	if(dwLastFreezeTime < dwDownTime )
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));
	}
	else if(dwLastFreezeTime < g_RelativeMin )	
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&CompleteFreezeTime.CompleteFreezeLadderClosingTime, sizeof(TRealTimer));
	}
	else
	{
		memcpy((BYTE*)&tStartFreezeTime, (BYTE*)&PowerDownTime, sizeof(TRealTimer));
	}
	
	//��ʼ��������Է�����
	dwStartFreezeTime = api_CalcInTimeRelativeMin(&tStartFreezeTime);
	
	//��ȡ����ǰ���ݽ����ձ�
	LadderMode = api_GetPowerDownYearBill(&LadderBillingPara);
	//ֻ���½��ݺ����������²����ݶ���
	if((LadderMode == 0x55) ||(LadderMode == 0xAA))
	{
		//���㲹��ʱ���б�
		memset( (BYTE*)&FreezeTime[0], 0x00, sizeof(FreezeTime) );
		//�½��ݣ�ÿ��һ��
		if(LadderMode == 0x55)
		{
			//���¼������ʱ���������
			wDownMonNum = (tStartFreezeTime.wYear%2000)*12+tStartFreezeTime.Mon-1;
			//�ϵ�ʱ���������
			wUpMonNum = (RealTimer.wYear%2000)*12+RealTimer.Mon-1;
			//������ʱ�䳬��4���½��ݽ����ա�
			if( (wUpMonNum - wDownMonNum)> 5 )
			{
				wDownMonNum = wUpMonNum-5;
			}
			//��Ҫ��ȡ6�Σ��Ա�֤�ѵ������һ����
			for( i = 0; i < 6; i++ )
			{
				//��������������������ó�
				tPowerDownTime.wYear = wDownMonNum/12+2000;
				tPowerDownTime.Mon = wDownMonNum%12+1;
				tPowerDownTime.Sec = 0;
				tPowerDownTime.Min = 0;
				tPowerDownTime.Hour = LadderBillingPara.LadderSavePara[0].Hour;
				tPowerDownTime.Day = LadderBillingPara.LadderSavePara[0].Day;
			
				dwFreezeTime = api_CalcInTimeRelativeMin( &tPowerDownTime );

				if( (dwFreezeTime>dwDownTime) && (dwFreezeTime<=g_RelativeMin) )
				{
					// ����4���µĽ��ݽ���ʱ��
					memmove( &FreezeTime[0], &FreezeTime[1], 4*sizeof(DWORD) );
					memcpy( &FreezeTime[4], &dwFreezeTime, sizeof(DWORD) );
				}
				else if( dwFreezeTime > g_RelativeMin )
				{
					break;
				}
				//����һ������
				wDownMonNum += 1;
			}	
		}
		else if(LadderMode == 0xAA)//�����
		{
			
			//����һ�꼸�ν��ݽ���
			Num = 0;
			for( i = 0; i < MAX_YEAR_BILL; i++)
			{
				if ((LadderBillingPara.LadderSavePara[i].Mon == 0xFF) 
				|| (LadderBillingPara.LadderSavePara[i].Day == 0XFF) 
				|| (LadderBillingPara.LadderSavePara[i].Hour == 0XFF))  
				{
					break;
				}
	
			}
			Num = i;
			memset( &FreezeTime[0], 0x00, sizeof(FreezeTime) );
			//������ʱ�䳬��5��ʱ���޸ĵ���ʱ��Ϊ���5�ꡣ
            tPowerDownTime.wYear = tStartFreezeTime.wYear;
			if( (RealTimer.wYear - tStartFreezeTime.wYear) > 5 )
			{
				tPowerDownTime.wYear = RealTimer.wYear-5;
			}
			
			//���������㣬�������6�꼴��
			for( j = 0; j< 6; j++ )
			{
				// һ��num�����ݽ�����
				for( i = 0; i < Num; i++ )
				{
					// �£��걣�ֲ���
					tPowerDownTime.Sec = 0;
					tPowerDownTime.Min = 0;
					tPowerDownTime.Hour = LadderBillingPara.LadderSavePara[i].Hour;
					tPowerDownTime.Day = LadderBillingPara.LadderSavePara[i].Day;
					tPowerDownTime.Mon = LadderBillingPara.LadderSavePara[i].Mon;

					dwFreezeTime = api_CalcInTimeRelativeMin( &tPowerDownTime );

					if( (dwFreezeTime>dwDownTime) && (dwFreezeTime<=g_RelativeMin) )
					{
						for(m=5;m>0;m--)//����5���ڷ�Χ�ڵ������Է�����
						{
							//�����ʱ����ڴ洢ʱ�䣬����ʱ��ŵ���λ��
							if(dwFreezeTime > FreezeTime[m-1])
							{
								dwFreezeTime1 = FreezeTime[m-1];
								FreezeTime[m-1] = dwFreezeTime;
								dwFreezeTime = dwFreezeTime1;
							}
							else if(dwFreezeTime == FreezeTime[m-1])
							{
								break;
							}
						}	
					}
				}	
				//����һ������
				tPowerDownTime.wYear += 1;
			}	
		}
		//����Ľ��ݽ������Ƿ����4,����4�嵱ǰ���ݽ����õ���
		Flag = 0x00;
		if(FreezeTime[0] != 0)//�ж��Ƿ��5�����ݽ���
		{
			api_ClrCurLadderUseEnergy(); //�����ǰ���ݽ����õ���
			Flag = 0x55;
		}
		for( i = 1; i < 5; i++ )
		{
			if( api_CheckSysVol( eOnRunSpeedDetectUnsetFlag ) == FALSE )
			{
				return FALSE;
			}
			if( FreezeTime[i] == 0x00 )
			{			
				continue;
			}	
			CLEAR_WATCH_DOG;		
			//���ʱ��
			PowerUpFreezeMin=FreezeTime[i];
			
			ProcFreezeTask(eFREEZE_LADDER_CLOSING,0x55);
			//��¼�˴�����ʱ��
			//��¼�˴�����ʱ��
			api_ToAbsTime(PowerUpFreezeMin, &CompleteFreezeTime.CompleteFreezeLadderClosingTime);
			CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
			if (Flag == 0x00) 	
			{						
				api_ClrCurLadderUseEnergy(); //�����ǰ���ݽ����õ���
				Flag = 0xaa;
			}		
		}
	}
	return TRUE;
}
#endif//( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//��������: �ϵ粹����
//
//����: 	��
//	
//����ֵ:	��
//
//��ע:	 
//-----------------------------------------------
static void PowerUpCompleteFreeze( void )
{
	TRealTimer		PowerDownTime;
	DWORD dwDownTime;
	
	if( api_CheckClock(&RealTimer) == FALSE )
	{
		return;	
	}
	//��ȡ����ʱ��
	api_GetPowerDownTime( (TRealTimer*)&PowerDownTime );
	
	//����ʱ����Է�����
	dwDownTime = api_CalcInTimeRelativeMin(&PowerDownTime);
	//�жϵ���ʱ�����Է�����,������ڵ�ǰʱ�䣬�����в���
	if( api_CheckClock(&PowerDownTime) == TRUE )
	{
		if( ( dwDownTime >= g_RelativeMin ) || (g_RelativeMin > (dwDownTime+5256000)) )
		{
			return;
		}
	}
	
	//ʱ����Ȳ����з����ڶ��Ჹ���������쳣��λ����
	if( memcmp((BYTE*)&FreezePowerDownTimeBak, (BYTE*)&PowerDownTime, sizeof(TRealTimer)) == 0 )
	{
		return;
	}
	
	//������Ĳ�����ʱ��CRC���Բ���
	if( CompleteFreezeTime.CRC32 != lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD)))
	{
		memset( (BYTE*)&CompleteFreezeTime, 0x00, sizeof(TCompleteFreezeTime) );
		CompleteFreezeTime.CRC32 = lib_CheckCRC32( (BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
	}
	//�ն��Ჹ����
	if( PowerUpCompleteFreezeDay(PowerDownTime) == FALSE )
	{
		return;	
	}
	
	//�����ն��Ჹ����
	if( PowerUpCompleteFreezeClosing(PowerDownTime) == FALSE )
	{
		return;	
	}
	
	//���ݽ��㲹����
	#if( PREPAY_MODE == PREPAY_LOCAL )
	if( PowerUpCompleteFreezeLadderClosing( PowerDownTime ) == FALSE )
	{
		return;	
	}
	#endif
}

//-----------------------------------------------
//��������: �����ϵ紦��
//
//����: 	��
//	
//����ֵ:	��
//
//��ע:	 
//-----------------------------------------------
void api_PowerUpFreeze( void )
{
	if( wInstantFreezeTimerBak != (WORD)(~wInstantFreezeTimer) )//˲ʱ�����־У��
	{
		wInstantFreezeTimer = 0;
		wInstantFreezeTimerBak = 0xffff;
	}
	//�����Ӷ���ָ����Ϣ
	if( CheckMinFreezeInfo() == FALSE )
	{
		#if (SLE_THIRD_MEM_BACKUP == YES)
		RepairFreezeUseFlash();
		#endif
	}
	PowerUpCompleteFreeze();

	//�ϵ���������Ӷ����־
	minFreezeFlag = 0xA5;	
}



//-----------------------------------------------
//��������: ��ʼ���������
//
//����: 
//	��
//	
//  ����ֵ:	
//  ��
//
//��ע:	  ��ʼ������
//-----------------------------------------------
void api_FactoryInitFreeze( void )
{
	api_WriteFreezeAttribue( 0x5000, 0xFF, (BYTE *)&InstantFreeze[1], InstantFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5002, 0xFF, (BYTE *)&MinFreeze[1], MinFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5003, 0xFF, (BYTE *)&HourFreeze[1], HourFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5004, 0xFF, (BYTE *)&DayFreeze[1], DayFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5005, 0xFF, (BYTE *)&ClosingFreeze[1], ClosingFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5006, 0xFF, (BYTE *)&MonFreeze[1], MonFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5008, 0xFF, (BYTE *)&TZChgFreeze[1], TZChgFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5009, 0xFF, (BYTE *)&DTTChgFreeze[1], DTTChgFreeze[0].Cycle );
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	api_WriteFreezeAttribue( 0x500a, 0xFF, (BYTE *)&TariffRateChgFreeze[1], TariffRateChgFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x500b, 0xFF, (BYTE *)&LadderChgFreeze[1], LadderChgFreeze[0].Cycle );
	api_WriteFreezeAttribue( 0x5011, 0xFF, (BYTE *)&LadderClosingFreeze[1], LadderClosingFreeze[0].Cycle );
	#endif
	FreezeFlag = 0;//��ʼ���󶳽��ͷ��ʼ
	memset( (BYTE*)&FreezePowerDownTimeBak, 0x00, sizeof(TRealTimer) );//��ʱ��Ϊ0//��ʼ�����һ�β����������Ƿ����쳣��λ���Ჹ����
	memset( (BYTE*)&CompleteFreezeTime, 0x00, sizeof(TCompleteFreezeTime) );
	CompleteFreezeTime.CRC32 = lib_CheckCRC32((BYTE*)&CompleteFreezeTime, sizeof(CompleteFreezeTime)-sizeof(DWORD));
	//���޸�ʱ���־
//	#if(THIRD_MEM_CHIP != CHIP_NO)
	api_ClrSafeMem( GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeUpdateTimeFlag), sizeof(TFreezeUpdateTimeFlag) );
//	#endif
}

#if (SLE_THIRD_MEM_BACKUP == YES)
//-----------------------------------------------
// ��������: ��ȡ��������
//
// ����: 	pBuf[in/out]
//
// ����ֵ:	��
//
// ��ע:
//-----------------------------------------------
void api_GetFreezeFlashMemData(TFreezeDataInfo *pBuf)
{
	memcpy((BYTE *)pBuf, (BYTE *)&MinInfo, sizeof(MinInfo));
}

//-----------------------------------------------
// ��������:����FLASH�ĵ��ܼ�����ָ�뱸������
//
// ����: 	dwRecordNo[in] FLASH������ָ��
//        Type[in] 0x55 RecordNo�����ı䣬0x00 RecordNoδ�����ı�
//        pBuf[in]
//
// ����ֵ:	��
//
// ��ע:
//-----------------------------------------------
void SaveFlashMemData(DWORD dwRecordNo, BYTE Type, BYTE* pBuf)
{
	DWORD dwAddr;
	TSectorInfo SectorInfo = {0};
	TFlashMem* pFlashMemTemp; //ʹ��ָ�����ջ����

	// ��������ݱ���ľ��Ե�ַ
	if (GetSectorInfo(FLASH_MEM_DATA_LEN, FLASH_MEM_DEPTH, dwRecordNo, &SectorInfo) == FALSE)
	{
		return;
	}

	dwAddr = FLASH_MEM_BASE_ADDR + SectorInfo.dwAddr;
	pFlashMemTemp = (TFlashMem*)pBuf;
	// ��ȡ��������
	api_GetEnergyFlashMemData(&pFlashMemTemp->Energy);
	// ��ȡ��������
	api_GetFreezeFlashMemData(&pFlashMemTemp->MinDataInfo[0]);

	// �ж�������Ч��
	if (pFlashMemTemp->MinDataInfo[0].RecordNo != (dwRecordNo + 1))
	{
		return;
	}
	
	CLEAR_WATCH_DOG;

	// ��������
	if (Type == 0x55)
	{
		WriteFreezeRecordData_EraseFlash(dwAddr, FLASH_MEM_DATA_LEN, pBuf);
	}
	else
	{
		if (WriteFreezeRecordData(dwAddr, FLASH_MEM_DATA_LEN, FLASH_MEM_BASE_ADDR, FLASH_MEM_SIZE, pBuf) == FALSE)
		{
			WriteFreezeRecordData_EraseFlash(dwAddr, FLASH_MEM_DATA_LEN, pBuf);
		}
	}
	
	CLEAR_WATCH_DOG;
	
	// ����ָ��
	dwAddr = FLASH_MEM_RECORDNO_ADDR + (dwRecordNo % FLASH_MEM_RECORDNO_DEPTH) * FLASH_MEM_RECORDNO_LEN;

	memcpy( pBuf, (BYTE *)&dwRecordNo, FLASH_MEM_RECORDNO_LEN );

	// ������ŷ����ı䣬��FLASHָ������
	if (Type == 0x55)
	{
		InitFlashMem();
	}

	if (WriteFreezeRecordData(dwAddr, FLASH_MEM_RECORDNO_LEN, FLASH_MEM_RECORDNO_ADDR, FLASH_MEM_RECORDNO_SIZE, pBuf) == FALSE)
	{
		WriteFreezeRecordData_EraseFlash(dwAddr, FLASH_MEM_RECORDNO_LEN, pBuf);
	}
}

//-----------------------------------------------
// ��������: ����FLASH���������е����ֵ
//
// ����: pFlashMemNo[in/out] ���ض���ָ��
//
// ����ֵ:
//
// ��ע:����FLASH�����������ݣ�����ָ�����ֵ
//-----------------------------------------------
BOOL FindFlashMemDataMaxNo(DWORD *pFlashMemNo)
{
	BYTE FlashBuf[SECTOR_SIZE];
	DWORD j, TempNo, TempNoBack;

	CLEAR_WATCH_DOG;
	
	pFlashMemNo[0] = ILLEGAL_VALUE_8F;

	if (api_ReadMemRecordData(FLASH_MEM_RECORDNO_ADDR, SECTOR_SIZE, FlashBuf) != TRUE)
	{
		return FALSE;
	}

	if ( lib_IsAllAssignNum( FlashBuf, 0xFF, sizeof(FlashBuf) )== TRUE )
	{
		return FALSE;
	}
	
	TempNo = 0;
	TempNoBack = 0;

	//�������ֵ
	for (j = 0; j < FLASH_MEM_RECORDNO_DEPTH; j++)
	{
		memcpy((BYTE *)&TempNo, (BYTE *)&FlashBuf[(j * FLASH_MEM_RECORDNO_LEN)], sizeof(TempNo));

		if (TempNo == ILLEGAL_VALUE_8F)
		{
			continue;
		}

		if( TempNoBack < TempNo )
		{
			TempNoBack = TempNo;
		}
	}

	pFlashMemNo[0] = TempNoBack;

	return TRUE;
}

//-----------------------------------------------
// ��������: ����FLASH�����������1����Ч���ݵ�ָ��
//
// ����:pFlashMemNo[in]
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
BOOL api_FindFlashMemDataNo(DWORD *pFlashMemNo)
{
	BYTE i,Buf[4];
	DWORD dwAddr, FlashMemNo, Offset;
	TFreezeDataInfo TempMinInfo;
	TSectorInfo SectorInfo = {0};

	// ���RAM��У��
	if (lib_CheckSafeMemVerify((BYTE *)&MinInfo[0], sizeof(TFreezeDataInfo), UN_REPAIR_CRC) == TRUE)
	{
		if (MinInfo[0].RecordNo == 0)
		{
			return FALSE;
		}

		pFlashMemNo[0] = MinInfo[0].RecordNo - 1;

		return TRUE;
	}

	// ����FLASH�е����ָ��
	if (FindFlashMemDataMaxNo(&FlashMemNo) == FALSE)
	{
		FlashMemNo = ILLEGAL_VALUE_8F;
	}
	else
	{
		//������ݺϷ���
		if( api_GetFlashMemData(FlashMemNo, GET_STRUCT_ADDR(TFlashMem, MinDataInfo), 4, Buf) == FALSE )
		{
			FlashMemNo = ILLEGAL_VALUE_8F;
		}
	}

	// ��EEP�ָ�
	if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeInfoRom.MinDataInfo[0]), sizeof(TFreezeDataInfo), (BYTE *)&TempMinInfo) == FALSE)
	{
		TempMinInfo.RecordNo = ILLEGAL_VALUE_8F;
	}
	else
	{
		TempMinInfo.RecordNo -= 1;

		//������ݺϷ���
		if( api_GetFlashMemData(TempMinInfo.RecordNo, GET_STRUCT_ADDR(TFlashMem, MinDataInfo), 4, Buf) == FALSE )
		{
			TempMinInfo.RecordNo = ILLEGAL_VALUE_8F;
		}
	}

	pFlashMemNo[0] = ILLEGAL_VALUE_8F;

	if ((FlashMemNo == ILLEGAL_VALUE_8F) && (TempMinInfo.RecordNo == ILLEGAL_VALUE_8F))
	{
		// EE,FLASH������
	}
	else if ((FlashMemNo == ILLEGAL_VALUE_8F) && (TempMinInfo.RecordNo != ILLEGAL_VALUE_8F))
	{
		// EE��ȷ,FLASH����
		pFlashMemNo[0] = TempMinInfo.RecordNo;
	}
	else if ((FlashMemNo != ILLEGAL_VALUE_8F) && (TempMinInfo.RecordNo == ILLEGAL_VALUE_8F))
	{
		// EE����,FLASH��ȷ
		pFlashMemNo[0] = FlashMemNo;
		
		// ǿ��ˢ�·��Ӷ���ָ��
		RefreshFreezeUseFlash(pFlashMemNo[0]);
	}
	else
	{
		// EE,FLASH����ȷ��ʹ�ô�ֵ
		if (FlashMemNo > TempMinInfo.RecordNo)
		{
			// ����FLASH
			pFlashMemNo[0] = FlashMemNo;

			// ǿ��ˢ�·��Ӷ���ָ��
			RefreshFreezeUseFlash(pFlashMemNo[0]);
		}
		else
		{
			// ����EE
			pFlashMemNo[0] = TempMinInfo.RecordNo;
		}
	}

	if (pFlashMemNo[0] == ILLEGAL_VALUE_8F)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

//-----------------------------------------------
// ��������:��ȡFLASH�еı�������
//
// ����:dwRecordNo[in]
//		Offset[in]
//		Len[in]
//		pBuf[in/out]
//
// ����ֵ:	��
//
// ��ע:
//-----------------------------------------------
BOOL api_GetFlashMemData(DWORD dwRecordNo, WORD Offset, WORD Len, BYTE *pBuf)
{
	DWORD dwAddr,FlashRecordNo;
	TSectorInfo SectorInfo = {0};

	if ((Len > 1000)||(dwRecordNo == ILLEGAL_VALUE_8F))
	{
		return FALSE;
	}
	CLEAR_WATCH_DOG;
	
	// ��������ݱ���ľ��Ե�ַ
	if (GetSectorInfo(FLASH_MEM_DATA_LEN, FLASH_MEM_DEPTH, dwRecordNo, &SectorInfo) == FALSE)
	{
		return FALSE;
	}

	dwAddr = FLASH_MEM_BASE_ADDR + SectorInfo.dwAddr + GET_STRUCT_ADDR(TFlashMem, MinDataInfo[0].RecordNo);

	if (api_ReadMemRecordData(dwAddr, 4, (BYTE*)&FlashRecordNo) == FALSE)
	{
		return FALSE;
	}
	
	// ������ݺϷ��ԣ��ж�FLASH�е�Record�Ƿ�һ��
	if( FlashRecordNo != (dwRecordNo+1) )
	{
		return FALSE;
	}
	
	dwAddr = FLASH_MEM_BASE_ADDR + SectorInfo.dwAddr + Offset;

	if (api_ReadMemRecordData(dwAddr, Len, pBuf) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
// ��������: ͨ��FALSH�е�����ˢ�¶���ָ��
//
// ����:
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
BOOL RefreshFreezeUseFlash( DWORD RecordNo )
{
	BYTE i;
	TFreezeDataInfo TempMinDataInfo[MAX_FREEZE_PLANNUM] = {0};
	TFreezeDataInfo TempMinDataInfoEE = {0};
	
	// ��ȡ����
	if (api_GetFlashMemData(RecordNo, GET_STRUCT_ADDR(TFlashMem, MinDataInfo), sizeof(TempMinDataInfo), (BYTE *)&TempMinDataInfo) != TRUE)
	{
		return FALSE;
	}

	for (i = 0; i < MAX_FREEZE_PLANNUM; i++)
	{
		CLEAR_WATCH_DOG;
		
		// ���FLASH��ȷ��
		if (TempMinDataInfo[i].CRC32 != lib_CheckCRC32((BYTE *)&TempMinDataInfo[i], (sizeof(TFreezeDataInfo) - sizeof(DWORD))))
		{
			continue;
		}

		// ���EE��ȷ��
		if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeInfoRom.MinDataInfo[i]), sizeof(TFreezeDataInfo), (BYTE *)&TempMinDataInfoEE) == TRUE)
		{
			if(( i == 0 )&&( TempMinDataInfo[i].RecordNo > TempMinDataInfoEE.RecordNo ))
			{
				// ����0����FLASH > EE����ʹ��FLASH���ݸ���EE
			}
			else
			{
				continue;
			}
		}

		// �޸�RAM
		memcpy((BYTE *)&MinInfo[i], (BYTE *)&TempMinDataInfo[i], sizeof(TFreezeDataInfo));

		// �޸�EE
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(FreezeSafeRom.FreezeInfoRom.MinDataInfo[i]), sizeof(TFreezeDataInfo), (BYTE *)&MinInfo[i]);
	}

	api_WriteSysUNMsg(SYSUN_WRITE_FREEZE_EE);

	return TRUE;
}

//-----------------------------------------------
// ��������: ͨ��FALSH�е������޸�����ָ��
//
// ����:
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
BOOL RepairFreezeUseFlash(void)
{
	DWORD RecordNo;
	
	// ��ȡ����ָ��
	if (FindFlashMemDataMaxNo(&RecordNo) != TRUE)
	{
		return FALSE;
	}

	RefreshFreezeUseFlash( RecordNo );

	return TRUE;
}

//-----------------------------------------------
// ��������: ��ʼ��flash���ݲ���
//
// ����:
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
void InitFlashMem(void)
{
	BYTE Temp[4];
	DWORD i;

	memset(Temp, 0xFF, sizeof(Temp));

	for (i = 0; i < FLASH_MEM_RECORDNO_SECTOR_NUM; i++)
	{
		CLEAR_WATCH_DOG;
		
		api_WriteMemRecordData(FLASH_MEM_RECORDNO_ADDR + i * SECTOR_SIZE, sizeof(Temp), Temp);
	}
}
#endif // #if( SLE_THIRD_MEM_BACKUP == YES )
