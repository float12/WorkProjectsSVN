//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.8.27
//����		����ģ��Apiͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __FREEZE_API_H
#define __FREEZE_API_H

////�洢�ռ����궨��
//#define COLLECT_SAVE_START_SECTORS				2048		//��ʼ�洢�ɼ���������
//#define COLLECT_SAVE_SECTORS_NUM				( 8 * 1024 - COLLECT_SAVE_START_SECTORS )	//���ô洢������
//#define COLLECT_SAVE_REGION_NUM					48			//��Ϊ���ٷ�
//#define COLLECT_ONE_REGION_SIZEOF				( (COLLECT_SAVE_SECTORS_NUM / COLLECT_SAVE_REGION_NUM ) + 1)	//ÿ�ݴ洢�ռ��С		
//
////��������������
//#define COLLECT_SCHEME_NUM						12			//�ɼ�������
//
//#pragma pack(1)
////�ɼ��洢��Ϣ�ṹ��
//typedef struct TCollectSaveInfo_t   	
//{
//	WORD wOneRecordLen;										//һ����¼��󳤶�
//	WORD dwRecordNum[SEARCH_METER_MAX_NUM];				//һ����¼����
//	BYTE bMeterNo[6][SEARCH_METER_MAX_NUM];				//�÷�����Ӧ�ı�
//	BYTE bMeterNum;											//�÷�������
//	BYTE dwSaveLump[COLLECT_ONE_REGION_SIZEOF];			//�洢�飬һ��BITλһ���飬�����Է�
//	DWORD dwCRC;											//У��
//}TCollectSaveInfo;
//#pragma pack()
//
////ÿ��ȷ�Ϻ��ĸ�������Ҫ����ʱ�����洢��Ϣ��ȡ�������������ݴ洢����
//TCollectSaveInfo CollectSaveInfo;

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
//���õ���Ƭ�洢��ʱ���������ݵĻ���ַ
#define FLASH_FREEZE_BASE					(FLASH_EVENT_BASE+SUB_EVENT_INDEX_MAX_NUM*EVENT_RECORD_LEN + EVENT_RECORD_EXT_LEN)

//��������
#if( MAX_PHASE == 1)
#define FLASH_SECTOR_NUM					(6)		//���ա��¡������ⶳ��������
#define MIN_FLASH_SECTOR_NUM				(600)	//���Ӷ���������
#define MINBAK_FLASH_SECTOR_NUM				(100)	//���Ӷ�����������
#define DAY_FLASH_SECTOR_NUM				(30)	//�ն���������
#define MON_FLASH_SECTOR_NUM				(6)		//�¶���������

#else
//�����Ӷ�����ն�������ÿ�ֶ���ռ�ü�������
#define FLASH_SECTOR_NUM					(6)		//���ա��¡������ⶳ��������
#define MIN_FLASH_SECTOR_NUM				(1200)	//���Ӷ���������
#define MINBAK_FLASH_SECTOR_NUM				(80)	//���Ӷ�����������
#define DAY_FLASH_SECTOR_NUM				(150)	//�ն���������
#define MON_FLASH_SECTOR_NUM				(20)	//�¶���������
#endif


//���Ӷ������
#if( MAX_PHASE == 1)
#define MAX_FREEZE_MIN_DEPTH				35040
#else
#define MAX_FREEZE_MIN_DEPTH				35040
#endif

#if( MAX_PHASE == 1)
#define MAX_FREEZE_ATTRIBUTE				10// ���ж�����֧�ֵ����Ե������
#else
#define MAX_FREEZE_ATTRIBUTE				32	
#endif
#define MAX_FREEZE_PLANNUM					8// ���ж�����֧�ֵķ������������ֻ�з��Ӷ����ж�


// ˲ʱ
#if( MAX_PHASE == 1)
#define MAX_INSTANT_FREEZE_ATTRIBUTE		(3+2)// Ĭ�� �������й����ܣ��й����� 3��
#else
#define MAX_INSTANT_FREEZE_ATTRIBUTE		(12+4+4)// Ĭ�� ����*8�����������������ʱ��*2���й������޹�����	һ��12��//�����üӵ�20��
#endif																		
#define MAX_INSTANT_FREEZE_DATA_LEN			(FLASH_SECTOR_NUM*SECTOR_SIZE) 	//((4+20*8+55*2+16*2)*3+105*3)


// ����
#if( MAX_PHASE == 1)
#define MAX_MINUTE_FREEZE_ATTRIBUTE			(7+3)
#define MAX_MIN_FREEZE_DATA_LEN				(MIN_FLASH_SECTOR_NUM*SECTOR_SIZE)	//@@@@@@��1����20�쿼��			// (4�ֽ�Time+4�ֽ�����)*288*6+ (4+4)*288*2+ (4+2*3)*288+(4+4*4)*288+(4+4*4)*288+ (4+2*4)*288
#else
#define MAX_MINUTE_FREEZE_ATTRIBUTE			(17+6)// Ĭ�� �������й�,1,2,3,4�����޹��ܵ��ܣ��й��������޹���������ѹ��������      �й����ʣ��������� 12��	
#define MAX_MIN_FREEZE_DATA_LEN				(MIN_FLASH_SECTOR_NUM*SECTOR_SIZE)	//@@@@@@��1����20�쿼��			// (4�ֽ�Time+4�ֽ�����)*288*6+ (4+4)*288*2+ (4+2*3)*288+(4+4*4)*288+(4+4*4)*288+ (4+2*4)*288
#endif	
#define MAX_MINBAK_FREEZE_DATA_LEN			(MINBAK_FLASH_SECTOR_NUM*SECTOR_SIZE)													


// Сʱ
#define MAX_HOUR_FREEZE_ATTRIBUTE			(2+1)	// Ĭ�� �������й��ܵ��� 2��
#define MAX_HOUR_FREEZE_DATA_LEN			(FLASH_SECTOR_NUM*SECTOR_SIZE)// (4�ֽ�Time+4�ֽ�����)*254*2

// ��
#if( MAX_PHASE == 1)
	#if( PREPAY_MODE == PREPAY_LOCAL )
	#define MAX_DAY_FREEZE_ATTRIBUTE		(5+2)	// Ĭ�� �������й��ܣ����ƽ�ȵ���,�й�������+A 3��
	#define MAX_DAY_FREEZE_E2_ONCE_DATA_LEN	300//(4L+65*2+8+8+4+4)���г���158��E2�ռ䰴��300���ֽڿ�
	#else
	#define MAX_DAY_FREEZE_ATTRIBUTE		(3+2)	// Ĭ�� �������й��ܣ����ƽ�ȵ���,�й�������+A 3��
	#define MAX_DAY_FREEZE_E2_ONCE_DATA_LEN	300//(4L+65*2+8+8+4+4)���г���158��E2�ռ䰴��300���ֽڿ�
	#endif
	//�ն���ռ䲻����������Ϊ4��----��ا��
	#define MAX_DAY_FREEZE_DATA_LEN			(DAY_FLASH_SECTOR_NUM*SECTOR_SIZE)// (4�ֽ�Time+20�ֽ�����)*62*8+       (4+11*5)*62*2+      (4+4*4)*2*62+     (4+16)*3*62
#else
	#define MAX_DAY_FREEZE_ATTRIBUTE		(1)	// Ĭ�� �������й�,����޹�1,2,��1,2,3,4�����޹��ܣ����ƽ�ȵ���,�������й��������������ʱ�䣻�й��޹�����,ABC��ѹ�ϸ��ʡ����û��
	//ʱ��4�ֽڡ��ܵ���4�ֽڡ�12���ʵ���52�ֽڡ�12����143�ֽڡ�����16�ֽڡ���ѹ�ϸ���16�ֽڡ�ʣ����8�ֽڡ�͸֧���4�ֽڣ�Ԥ��240�ֽڣ�CRCУ��4�ֽ�
	#define MAX_DAY_FREEZE_E2_ONCE_DATA_LEN	1000//(4L+4*6+52*8+2*143+16*2+16*3+8+4+4)���г���936��E2�ռ䰴��1300���ֽڿ�
	//�ն���ռ䲻����������Ϊ23��----��ا��
	#define MAX_DAY_FREEZE_DATA_LEN			(DAY_FLASH_SECTOR_NUM*SECTOR_SIZE)// (4�ֽ�Time+20�ֽ�����)*62*8+       (4+11*5)*62*2+      (4+4*4)*2*62+     (4+16)*3*62
#endif
#define MAX_DAY_FREEZE_E2_DEPTH				2													//E2���ݵ��ն����������
#define MAX_DAY_FREEZE_E2_DATA_LEN			(MAX_DAY_FREEZE_E2_ONCE_DATA_LEN*MAX_DAY_FREEZE_E2_DEPTH)

// ��
#if( MAX_PHASE == 1)
#define MAX_MON_FREEZE_ATTRIBUTE			(3+2)	// Ĭ�� ����й����������й��ܣ����ƽ�ȵ��ܣ��¶��õ���(����й���) 4��
#else
#define MAX_MON_FREEZE_ATTRIBUTE			(20+7)	// Ĭ�� 35�����ܣ�  �������й��������������ʱ��  ���¶��õ�������ABC��ѹ�ϸ���  42��
#endif
#define MAX_MON_FREEZE_DATA_LEN				(MON_FLASH_SECTOR_NUM*SECTOR_SIZE)	// (4+20)*12*35 + (4+55)*12*2 + (4+16)*12*3 + (4+4)*12*1= 

// ʱ�����л�
#if( MAX_PHASE == 1)
#define MAX_TIME_ZONE_FREEZE_ATTRIBUTE		(3+2)// Ĭ�� �������й��ܣ����ƽ�ȵ��ܣ��й����� 3��
#else
#define MAX_TIME_ZONE_FREEZE_ATTRIBUTE		(12+4+4)// Ĭ�� ����8��	��������Լ�����ʱ��2��	�й�/�޹�����2��	   һ��12��	
#endif
#define MAX_TIME_ZONE_FREEZE_DATA_LEN		(FLASH_SECTOR_NUM*SECTOR_SIZE)					// (4�ֽ�Time+20�ֽ�����)*8*2+(4+55)*2+(4+4*4)*2 

// ��ʱ�α��л�
#if( MAX_PHASE == 1)
#define MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE	(3+2)	// Ĭ�� �������й��ܣ����ƽ�ȵ��ܣ��й����� 3��
#else
#define MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE	(12+4+4)	// Ĭ�� ����8��	��������Լ�����ʱ��2��	�й�/�޹�����2��	   һ��12��
#endif
#define MAX_DAY_TIMETABLE_FREEZE_DATA_LEN	(FLASH_SECTOR_NUM*SECTOR_SIZE)// (4�ֽ�Time+20�ֽ�����)*8*2+(4+11*5)*2*2+(4+4*4)*2*2

// �����ն���
#if( MAX_PHASE == 1)
#define MAX_CLOSING_FREEZE_ATTRIBUTE		(4+2)	// Ĭ�� �������й��ܣ����ƽ�ȵ��� 2�����¶��õ���
#define MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN	300//(4L+65*3+8+4)���г���207��E2�ռ䰴��300���ֽڿ�

#else
#define MAX_CLOSING_FREEZE_ATTRIBUTE		(15+5)	// 
//ʱ��4�ֽڡ��ܵ���4�ֽڡ�12���ʵ���52�ֽڡ�12��������143�ֽڡ��¶��õ���4�ֽڣ�Ԥ��240�ֽڣ�CRCУ��4�ֽ�
#define MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN	1000 //(4L+4*3+52*9+143*2+4+240+4)���г���898��E2�ռ䰴��1300���ֽڿ�

#endif
#define MAX_CLOSING_FREEZE_DATA_LEN			(FLASH_SECTOR_NUM*SECTOR_SIZE)						// (4�ֽ�Time+20�ֽ�����)*12*2= 576 �ֽ�
#define MAX_CLOSING_FREEZE_E2_DEPTH			2												//E2���ݵĽ����ն����������
#define MAX_CLOSING_FREEZE_E2_DATA_LEN		(MAX_CLOSING_FREEZE_E2_ONCE_DATA_LEN*MAX_CLOSING_FREEZE_E2_DEPTH)

#if( PREPAY_MODE == PREPAY_LOCAL )
// ���ʵ�۱��л�
#if( MAX_PHASE == 1)
#define MAX_TARIFF_RATE_FREEZE_ATTRIBUTE	(3+2)	// Ĭ�� �������й��ܣ����ƽ�ȵ��ܣ��й����� 3��
#else
#define MAX_TARIFF_RATE_FREEZE_ATTRIBUTE	(12+4)	// Ĭ�� ����8��	��������Լ�����ʱ��2��	�й�/�޹�����2��	   һ��12��
#endif
#define MAX_TARIFF_RATE_FREEZE_DATA_LEN		(FLASH_SECTOR_NUM*SECTOR_SIZE)	// (4�ֽ�Time+20�ֽ�����)*8*2+(4+55)*2+(4+4*4)*2 


// ���ݵ�۱��л�
#if( MAX_PHASE == 1)
#define MAX_LADDER_FREEZE_ATTRIBUTE			(3+2)	// Ĭ�� �������й��ܣ����ƽ�ȵ��ܣ��й����� 3��
#else
#define MAX_LADDER_FREEZE_ATTRIBUTE			(12+4)	// Ĭ�� ����8��	��������Լ�����ʱ��2��	�й�/�޹�����2��	   һ��12��
#endif
#define MAX_LADDER_FREEZE_DATA_LEN			(FLASH_SECTOR_NUM*SECTOR_SIZE) // (4�ֽ�Time+20�ֽ�����)*8*2+(4+55)*2+(4+4*4)*2 


// ���ݽ��㶳��
#if( MAX_PHASE == 1)
#define MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE	(1+1)	// Ĭ�� �������й��ܣ����ƽ�ȵ��ܣ��й����� 3��
#else
#define MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE	(1+1)	// Ĭ�� ����8��	��������Լ�����ʱ��2��	�й�/�޹�����2��	   һ��12��
#endif
#define MAX_LADDER_CLOSING_FREEZE_DATA_LEN	(FLASH_SECTOR_NUM*SECTOR_SIZE)	// (4�ֽ�Time+20�ֽ�����)*8*2+(4+55)*2+(4+4*4)*2 
#endif
#if( MAX_INSTANT_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_MINUTE_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_HOUR_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_DAY_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_MON_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_TIME_ZONE_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_CLOSING_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_TARIFF_RATE_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_LADDER_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_LADDER_CLOSING_FREEZE_ATTRIBUTE > MAX_FREEZE_ATTRIBUTE )
���ô���
#endif

#if( MAX_FREEZE_ATTRIBUTE > 34 )
���ô���--��ȫ�ռ䳤������Ϊ350�ֽ�--MAX_FREEZE_ATTRIBUTE*sizeof(TFreezeAttOad);
#endif


//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//���Ӷ���ռ�ÿռ���࣬���������
//���ű��˳��Ҫ��FreezeInfoTab���涨���˳��һ��
typedef enum
{
	//�����ڶ����� eFREEZE_CLOSING�̶�Ϊ��һ��
	eFREEZE_CLOSING,			//�����ն���
	#if( PREPAY_MODE == PREPAY_LOCAL )
	eFREEZE_TARIFF_RATE_CHG,	//���ʵ���л�
	eFREEZE_LADDER_CHG,		//�����л� 0X0B
	eFREEZE_LADDER_CLOSING,	//���ݽ���//�����л���������ݽ��㶳��˳�����޸�
	#endif
	eFREEZE_TIME_ZONE_CHG,		//ʱ�����л�
	eFREEZE_DAY_TIMETABLE_CHG,	//��ʱ�α��л�	
	eFREEZE_INSTANT,			//˲ʱ����		
	
	//���ڶ����� eFREEZE_HOUR�̶�Ϊ���ڶ���ĵ�һ����eFREEZE_MIN�̶�Ϊ���һ��
	eFREEZE_HOUR,				//Сʱ����
	eFREEZE_MON,				//�¶���	
	eFREEZE_DAY,				//�ն���	
	eFREEZE_MIN,				//���Ӷ���(���������)
	eFREEZE_MAX_NUM				//����������������          ��1�����һ�����ս���
}eFreezeType;

typedef enum
{
	eDELETE_ATT_BY_OAD,	// ɾ��ָ��OAD
	eDELETE_ATT_ALL		// ����OIɾ������OAD
}eDELETE_ATT_TYPE;

#pragma pack(1)
typedef struct  TFreezePara_t
{
    WORD Cycle;
    DWORD OAD;
    WORD Depth;
}TFreezePara;

typedef struct TFreezeAttOad_t
{
	DWORD	Oad;			// OAD  ���Ա�ʶ������bit5~bit7��Ч
	WORD	AttDataLen;		// �����Զ�Ӧһ�����ݵĳ���,���Ӷ������ʱ�곤��
	DWORD	Offset;			// �Է��Ӷ��ᣬ����������Ϊ��Ӧ������¼�е�����ƫ��
							// �����������ᣬ�������������������������ƫ�Ƶ�ַ������ʱ�꣩
}TFreezeAttOad;

#pragma pack()

typedef struct TFreezeAttCycleDepth_t
{
	WORD	Cycle;			// �������� 698 long unsigned Ϊ16bit
	WORD	Depth;			// �洢��� �涨���
	DWORD	CRC32;
}TFreezeAttCycleDepth;

typedef struct TFreezeDataInfo_t
{
	DWORD	RecordNo;					//��¼���(��������)	
	DWORD	SaveDot;					//��������ʣ��Ķ���������޸�ʱ��ʱ����UpdataFreezeTime������Ҫ�޸�
	DWORD	CRC32;
}TFreezeDataInfo;

typedef struct TFreezeAllInfoRom_t
{
	BYTE	NumofOad;			//OAD�ĸ���
	BYTE	Rsv;
	WORD	AllAttDataLen;		//�����Ӷ���
	DWORD	CRC32;
}TFreezeAllInfoRom;

typedef struct TFreezeMinInfo_t
{
	DWORD	DataAddr[MAX_FREEZE_PLANNUM];		//���Ӷ��� ��������
	DWORD	DataBakAddr[MAX_FREEZE_PLANNUM];	//���Ӷ��� ���ݱ�����
	WORD	AllAttDataLen[MAX_FREEZE_PLANNUM];
	DWORD	CRC32;
}TFreezeMinInfo;

typedef struct TFreezeUpdateTimeFlag_t
{
	WORD 	wFreezeFlag;	//�޸�ʱ��ʱ���¶��Ჹ����־
	WORD 	wMinFlag;		//�޸�ʱ����Ӷ�������Բ�����־
	DWORD	CRC32;
}TFreezeUpdateTimeFlag;

// ����OAD��Ϣ
typedef struct TFreezeAttCycleDepthRom_t
{
	TFreezeAttCycleDepth	Freeze[eFREEZE_MIN];		//�����Ӷ����⣬ÿ�ֶ���ֻ��һ��
	TFreezeAttCycleDepth	Min[MAX_FREEZE_PLANNUM];	//���Ӷ���ÿ�����Զ���һ��
	TFreezeAttCycleDepth	MinBak[MAX_FREEZE_PLANNUM];	//Min��MinBak�м䲻Ҫ�������ݣ�������
}TFreezeAttCycleDepthRom;

//�����ڰ�ȫ�ռ�����Ĺ���������Ϣ�����8���ֽ��Ƿ�CRC�ģ�Ϊ����������⣬���Կ���8���ֽڡ�
//�ڴ洢ʱ����һ���ǰ������������棬���crc���ڽ������ݺ��棬��һ���ں�8���ֽ�����
typedef struct TFreezeAttOadRom_t
{
	BYTE		Closing[sizeof(TFreezeAttOad)*MAX_CLOSING_FREEZE_ATTRIBUTE+8];	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	BYTE		TariffRateChg[sizeof(TFreezeAttOad)*MAX_TIME_ZONE_FREEZE_ATTRIBUTE+8];	
	BYTE		LadderChg[sizeof(TFreezeAttOad)*MAX_TIME_ZONE_FREEZE_ATTRIBUTE+8];	
	BYTE		LadderClosing[sizeof(TFreezeAttOad)*MAX_TIME_ZONE_FREEZE_ATTRIBUTE+8];	
	#endif	
	BYTE		TimeZone[sizeof(TFreezeAttOad)*MAX_TIME_ZONE_FREEZE_ATTRIBUTE+8];	
	BYTE		DayTimeTab[sizeof(TFreezeAttOad)*MAX_DAY_TIMETABLE_FREEZE_ATTRIBUTE+8];	
	BYTE		Instant[sizeof(TFreezeAttOad)*MAX_INSTANT_FREEZE_ATTRIBUTE+8];		
	BYTE		Hour[sizeof(TFreezeAttOad)*MAX_HOUR_FREEZE_ATTRIBUTE+8];		
	BYTE		Mon[sizeof(TFreezeAttOad)*MAX_MON_FREEZE_ATTRIBUTE+8];	
	BYTE		Day[sizeof(TFreezeAttOad)*MAX_DAY_FREEZE_ATTRIBUTE+8];
	BYTE		Min[sizeof(TFreezeAttOad)*MAX_MINUTE_FREEZE_ATTRIBUTE+8];	
}TFreezeAttOadRom;

// ָ��
typedef struct TFreezeInfoRom_t
{
	TFreezeDataInfo	DataInfo[eFREEZE_MIN];						//�����Ӷ����⣬ÿ�ֶ�����һ��RecordNo	
	TFreezeDataInfo	MinDataInfo[MAX_FREEZE_PLANNUM];			//���Ӷ��ᣬÿ��������һ��RecordNo
	DWORD			CRC32;										//�����籣����Ӷ���ָ���ã�һ��д��
}TFreezeInfoRom;

//��ַӳ��ṹ��
//����
typedef struct TFreezeEeData_t
{

	BYTE	Closing[MAX_CLOSING_FREEZE_E2_DATA_LEN];//�����ն��ᱸ������E2�洢�ռ�	
	BYTE	Day[MAX_DAY_FREEZE_E2_DATA_LEN];//�ն��ᱸ������E2�洢�ռ�

}TFreezeEeData;


typedef struct TFreeze_t
{
	TFreezeEeData			EeData;		// ��������
}TFreezeConRom;


typedef struct TFreezeSafe_t
{
	TFreezeInfoRom			FreezeInfoRom;						//ָ����Ϣ
	TFreezeAllInfoRom		FreezeAllInfoRom[eFREEZE_MAX_NUM];	//������Ϣ
	TFreezeUpdateTimeFlag	FreezeUpdateTimeFlag;				//��ʱ���־
	TFreezeAttCycleDepthRom	AttCycleDepthRom;					//���� ���
	TFreezeAttOadRom		AttOadRom;							// OAD��Ϣ
	TFreezeMinInfo			FreezeMinInfo;						//���Ӷ�����Ϣ
}TFreezeSafeRom;

#define FLASH_FREEZE_END_ADDR (FLASH_FREEZE_BASE+(DWORD)eFREEZE_MON*FLASH_SECTOR_NUM*SECTOR_SIZE+MAX_MON_FREEZE_DATA_LEN+MAX_DAY_FREEZE_DATA_LEN+MAX_MIN_FREEZE_DATA_LEN+MAX_MINBAK_FREEZE_DATA_LEN)

// ����Flash��������
// FLASH_FREEZE_BASE��ʼ����ų��˷��Ӷ�������������������ݣ�ÿ��OAD������������������OAD�ĸ�����(SECTOR_SIZE*2*���OAD����)
// ���Ӷ������󣬰���ÿ��OADռ��������ȡ����1��ţ�(1180L*SECTOR_SIZE)
// ���Ӷ���ĺ���ŵ��Ƕ���N�����ݶ�Ӧ��ʱ����Ϣ:
// ����һ�����������Դ��1024�����ݶ�Ӧ��ʱ��,���˷��Ӷ��ᣬ��������һ�ֶ���ռ����������
// ���Ӷ����28800���㣬Ҫռ��[28800/1024]+1=30������
//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
// 				��������
//-----------------------------------------------

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
BYTE api_GetFreezeIndex( WORD OI );

//-----------------------------------------------
//��������: ��Ӷ��������������
//
//����: 
//	OI[in]:		0x5000~0x500B,0x5011 ������Ķ����ʶOI  
//	pOad[in]:	ָ��OAD Buffer��ָ��  2�ֽڶ�������+4�ֽ�OAD+2�ֽڴ洢���                    
//  OadNum[in]: �������OAD�ĸ���
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:   ���ɾ����������
//-----------------------------------------------
BOOL api_AddFreezeAttribue( WORD OI, BYTE *pOad ,BYTE OadNum );

//-----------------------------------------------
//��������: ɾ�����������������
//
//����: 
//	OI[in]:	0x5000~0x500B,0x5011 ������Ķ����ʶOI
//  
//	Type[in]:	eDELETE_ATT_TYPE
//  
//	pOad[in]:	Ҫɾ����OAD,���Ա�ʶ��������bit5~bit7������ඳ��Ķ��᷽��
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:   ɾ��һ��OAD�����е����¼ƣ����·���EEP�ռ�
//-----------------------------------------------
BOOL api_DeleteFreezeAttribue( WORD OI, BYTE Type, BYTE *pOad );

//-----------------------------------------------
//��������: ���ö��������������
//
//����: 
//	FreezeOI[in]:	0x5000~0x500B,0x5011 ������Ķ����ʶOI
//  
//  OadType[in]:
//			0 ~ �������е�OAD
//			N ~ �����б���ĵ�N��OAD
//	
//  pOad[in]:  ָ��OAD Buffer��ָ��  2�ֽڶ�������+4�ֽ�OAD+2�ֽڴ洢���
//                
//  OadNum[in]:	OAD�ĸ���
//
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:	���е����¼ƣ����·���EEP�ռ�
//-----------------------------------------------
BOOL api_WriteFreezeAttribue( WORD FreezeOI, BYTE OadType, BYTE *pOad, WORD OadNum );

//-----------------------------------------------
//��������: �����������������
//
//����: 
//	Type[in]:	0x5000~0x500B,0x5011 ������Ķ����ʶOI
//                 
//  No[in]:	0   �����з���������OAD	
//			1~N ��8�����������No��OAD	
//                
//  Len[in]:��Լ�㴫�����ݳ��ȣ������˳��ȷ���FALSE	
//
//  pBuf[out]: �������ݵ�ָ��
//				���ն������ڣ�OAD���洢���˳�򷵻أ�ÿ��DWORD���ܹ�3*4�ֽ�
//
//  ����ֵ:	
//    ֵΪ0x8000 ��������ִ��� OI��֧�֣���ȡEpprom��������⣩
//    ֵΪ0x0000 ������buf���Ȳ���
//    ����:     �������ݳ���( �ڸ���Ϊ0ʱ�����1�ֽڵ�0 ���س���Ϊ1)
//
//��ע: 
//-----------------------------------------------
WORD api_ReadFreezeAttribute( WORD Type, BYTE No, WORD Len, BYTE *pBuf );

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
BOOL api_SetFreezeFlag( eFreezeType inFreezeIndex, WORD Delay );

//-----------------------------------------------
//��������: �嶳������
//
//����: 
//
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ��������
//
//��ע:   
//-----------------------------------------------
void api_ClrFreezeData( void );

//-----------------------------------------------
//��������: ��������
//
//����: 
//	��
//                    
//����ֵ:
//	��
//
//��ע:   ��ѭ������
//-----------------------------------------------
void api_FreezeTask( void );

//-----------------------------------------------
//��������: �������¼
//
//����: 
//	Tag[in]:	0 ����Tag 
//				1 ��Tag
//                
//	pDLT698RecordPara[in]: ��ȡ����Ĳ����ṹ��
//                
//  Len[in]�������buf���� �����ж�buf�����Ƿ���
//                
//  pBuf[out]: �������ݵ�ָ�룬�еĴ����ݣ�û�еĲ�0
//				
//	
//  ����ֵ:	bit15λ��1 �������޴����� ��0������������ȷ���أ� 
//			bit0~bit14 �����صĶ������ݳ���
//
//��ע:	
//-----------------------------------------------
//WORD api_ReadFreezeRecord( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD Len, BYTE *pBuf );

//-----------------------------------------------
//��������: �����ϵ紦��
//
//����: 
//	��
//	
//  ����ֵ:	
//  ��
//
//��ע:	 
//-----------------------------------------------
void api_PowerUpFreeze( void );

//-----------------------------------------------
//��������: ������紦��
//
//����: 
//	��
//	
//  ����ֵ:	
//  ��
//
//��ע:	 
//-----------------------------------------------
void api_PowerDownFreeze( void );

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
void api_FactoryInitFreeze( void );

//-----------------------------------------------
//��������: ���¶����ж��õ�ʱ��,������RecordNo
//
//����: InOldRelativeMin[in]:�޸�ʱ��֮ǰ�����ʱ��
//                    
//����ֵ:
//	��
//
//��ע:ֻ������Ӷ���
//-----------------------------------------------
void api_UpdateFreezeTime( DWORD InOldRelativeMin );

//-----------------------------------------------
//��������: �������¼645�����ܽ��㡢�������㣩
//
//����:
//				DI[in]:	���ݱ�ʶ
//  			pOutBuf[out]: ��������
//
//����ֵ:		bit15λ��1 �������޴����� ��0������������ȷ���أ�
//				bit0~bit14 �����صĶ������ݳ���
//
//��ע:��֧�����ݿ�
//-----------------------------------------------
WORD api_ReadClosingRecord645( BYTE *DI, BYTE *pOutBuf );
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
WORD api_ReadFreezeRecord645( BYTE *DI,BYTE *pOutBuf );
//-----------------------------------------------
//��������: �����ɼ�¼645������4��
//
//����:
//				Ch[in]:	��Լͨ��
// 				ReadCmd[in]:�������� 0x11--������	0x12--����֡��ʽ��
//				DI[in]:	���ݱ�ʶ
// 				ReadTime[in]:��Լ�·���ʱ�� ��ʱ������
// 				DotNum[in]:��Լ�·�Ҫ���ĵ���
//  			pOutBuf[out]: ��������
//
//����ֵ:		bit15λ��1 �������޴����� ��0������������ȷ���أ�
//				bit14��1  �����к���֡��0Ϊû�к���֡
//				bit0~bit13 �����صĶ������ݳ���
//
//��ע:ֻ֧�ֲ���4Ҫ�� pOutBuf�������Ϊ��200-4=196���������˳�����Ҫ��֡
//-----------------------------------------------
WORD api_ReadLpfRecord645( eSERIAL_TYPE Ch, BYTE ReadCmd, BYTE *DI, BYTE *ReadTime, BYTE DotNum, BYTE *pOutBuf );

//-----------------------------------------------
//��������: �����ɼ�¼���ʱ��
//
//����:
//				DI[in]:	���ݱ�ʶ
//  			pOutBuf[out]: ��������
//
//����ֵ:		bit15λ��1 �������޴����� ��0������������ȷ���أ�
//				bit0~bit14 �����صļ��ʱ�䳤��
//
//��ע:����645 ���ɼ�¼���ʱ���698������еļ��ʱ�������һ����Ӧ���˴�ֻ��Ϊ�˽��DB1�������������ȶ����TIME����ʵ���Բ�֧��
//-----------------------------------------------
WORD api_ReadFreezeLpf645IntervalTime( BYTE *DI, BYTE *pOutBuf );
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
void api_ReflashFreeze_Ram( void );
#endif//__FREEZE_API_H

