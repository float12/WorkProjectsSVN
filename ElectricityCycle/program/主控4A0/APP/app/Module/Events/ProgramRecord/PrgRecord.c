//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.10.26
//����		�������̼�¼����̼�¼ģ��Դ�ļ�
//�޸ļ�¼	
//---------------------------------------------------------------------- 

#include "AllHead.h"
#include "PrgRecord.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//���ܱ����¼� 0x3012
static const DWORD ProgramOadConst[] =
{
	0,					// ����
};

#if( SEL_PRG_INFO_CLEAR_METER == YES )		//���ܱ������¼� 0x3013
static const DWORD ClearEnergyOadConst[] =
{
	#if( MAX_PHASE == 1)
	0x02,				// ����
	0x00102201,
	0x00202201
	#else
	0x0C,				// ����
	0x00102201,        //�������ǰ�����й��ܵ���
	0x00202201,        //�������ǰ�����й��ܵ���
	0x00502201,        //�������ǰ��һ�����޹��ܵ���
	0x00602201,        //�������ǰ�ڶ������޹��ܵ���
	0x00702201,        //�������ǰ���������޹��ܵ���
	0x00802201,        //�������ǰ���������޹��ܵ���
	0x00112201,        //�������ǰA�������й�����
	0x00212201,        //�������ǰA�෴���й�����
//	0x00512201,        //�������ǰA���һ�����޹�����
//	0x00612201,        //�������ǰA��ڶ������޹�����
//	0x00712201,        //�������ǰA����������޹�����
//	0x00812201,        //�������ǰA����������޹�����
	0x00122201,        //�������ǰB�������й�����
	0x00222201,        //�������ǰB�෴���й�����
//	0x00522201,        //�������ǰB���һ�����޹�����
//	0x00622201,        //�������ǰB��ڶ������޹�����
//	0x00722201,        //�������ǰB����������޹�����
//	0x00822201,        //�������ǰB����������޹�����
	0x00132201,        //�������ǰC�������й�����
	0x00232201,        //�������ǰC�෴���й�����
//	0x00532201,        //�������ǰC���һ�����޹�����
//	0x00632201,        //�������ǰC��ڶ������޹�����
//	0x00732201,        //�������ǰC����������޹�����
//	0x00832201,        //�������ǰC����������޹�����
	#endif
};
#endif

#if( SEL_PRG_INFO_CLEAR_MD == YES )			//���ܱ����������¼� 0x3014
static const DWORD ClearMDOadConst[] =
{
	6,					//����
	0x10102201,			//��������ǰ�����й����������������ʱ��
	0x10202201,			//��������ǰ�����й����������������ʱ��
	0x10502201,			//��������ǰ��һ�����޹����������������ʱ��
	0x10602201,			//��������ǰ�ڶ������޹����������������ʱ��
	0x10702201,			//��������ǰ���������޹����������������ʱ��
	0x10802201,			//��������ǰ���������޹����������������ʱ��
};
#endif

#if( SEL_PRG_INFO_CLEAR_EVENT == YES )		//���ܱ��¼������¼� 0x3015
static const DWORD ClearEventOadConst[] =
{
	0,					// ����
};
#endif

#if( SEL_PRG_INFO_ADJUST_TIME == YES )		//���ܱ�Уʱ�¼�	0x3016
static const DWORD AdjustTimeOadConst[] =
{
	0x02,				// ����
	0x00102201,		//Уʱǰ�����й��ܵ���
	0x00202201,		//Уʱǰ�����й��ܵ���
};
#endif

#if( SEL_PRG_INFO_TIME_TABLE == YES )		//���ܱ�ʱ�α����¼� 0x3017
static const DWORD TimeTableOadConst[] =
{
	0x00,				// ����
	//0x40162200,
	//0x40172200
};
#endif

#if( SEL_PRG_INFO_TIME_AREA == YES )		//���ܱ�ʱ�������¼� 0x3018
static const DWORD TimeAreaOadConst[] =
{
	0x01,				// ����
	0x40152200
};
#endif

#if( SEL_PRG_INFO_WEEK == YES )				//���ܱ������ձ���¼� 0x3019
static const DWORD WeekOadConst[] =
{
	0x02,				// ����
	0x40132200,			//���ǰ��������õ���ʱ�α��
};
#endif

#if( SEL_PRG_INFO_CLOSING_DAY == YES )		//���ܱ�����ձ���¼� 0x301a
static const DWORD ClosingDayOadConst[] =
{
	0x01,				// ����
	0x41162200
};
#endif

 
//���ܱ���բ�¼� ���ܱ��բ�¼� ��բ 0x301F ��բ 0x3020
static const DWORD OpenCloseRelayOadConst[] =
{
    #if( MAX_PHASE == 1)
	0x02,		   // ����
	0x00102201,    //�¼�����ʱ�������й��ܵ���
	0x00202201,    //�¼�����ʱ�̷����й��ܵ���
	#else
	0x06,		   // ����
	0x00102201,    //�¼�����ʱ�������й��ܵ���
	0x00202201,    //�¼�����ʱ�̷����й��ܵ���
	0x00502201,    //�¼�����ʱ�̵�һ�����޹��ܵ���
	0x00602201,    //�¼�����ʱ�̵ڶ������޹��ܵ���
	0x00702201,    //�¼�����ʱ�̵��������޹��ܵ���
	0x00802201,    //�¼�����ʱ�̵��������޹��ܵ���
	#endif
};

#if( SEL_PRG_INFO_HOLIDAY == YES )			// �ڼ��ձ�̼�¼ 0x3021
static const DWORD HolidayOadConst[] =
{
	0,					// ����
};
#endif

#if( SEL_PRG_INFO_P_COMBO == YES )			// �й���Ϸ�ʽ��̼�¼ 0x3022
static const DWORD PCombOadConst[] =
{
	1,					// ����
	0x41122200,			//�й����ģʽ��
};
#endif

#if( SEL_PRG_INFO_Q_COMBO == YES )			// �޹���Ϸ�ʽ��̼�¼ 0x3023
static const DWORD QCombOadConst[] =
{
	2,					// ����
	0x41132200,			//�޹����ģʽ��1
	0x41142200,			//�޹����ģʽ��2
};
#endif

#if( PREPAY_MODE == PREPAY_LOCAL )
//0x3024	���ܱ���ʲ��������¼�
static const DWORD TariffRateOadConst[] =
{
	2,					// ����
	0x40182200,			//40182200�������ǰ��ǰ�׷��ʵ�� 
	0x40192200,			//40192200�������ǰ�����׷��ʵ�� 
};

//0x3025	���ܱ���ݱ����¼�
static const DWORD LadderOadConst[] =
{
	2,					// ����
	0x401A2200,			//401A2200�������ǰ��ǰ�׽��ݵ�� 
	0x401B2200,			//401B2200�������ǰ�����׽��ݵ�� 
};
#endif//#if( PREPAY_MODE == PREPAY_LOCAL )

#if( SEL_PRG_UPDATE_KEY == YES )			//���ܱ���Կ�����¼� 0x3026
static const DWORD UpdateKeyOadConst[] =
{
	1,					// ����
	0xf1002400,			//�Գ���Կ�汾
};
#endif

#if( PREPAY_MODE == PREPAY_LOCAL )
//0x3027	���ܱ��쳣�忨�¼�
static const DWORD AbnormalCardOadConst[] =
{
	4,			   		// ����
	0x202C2202,	   		//����ǰ�ܹ������
	0x202C2201,	   		//����ǰʣ����
	0x00102201,    		//�¼�����ʱ�������й��ܵ���
	0x00202201,    		//�¼�����ʱ�̷����й��ܵ���
};
//0x3028	���ܱ����¼
static const DWORD BuyEnergyOadConst[] =
{
	6,					// ����
	0x202C8202,			//202C8202��������󹺵���� 
	0x202C2201,			//202C2201��������ǰʣ���� 
	0x202D2200,			//202D2200��������ǰ͸֧��� 
	0x202E2200,			//202E2200��������ǰ�ۼƹ����� 
	0x202C8201,			//202C8201���������ʣ����
	0x202E8200,			//202E8200����������ۼƹ�����
};
//0x3029	���ܱ��˷Ѽ�¼
static const DWORD ReturnMoneyOadConst[] =
{
	3,					// ����
	0x202C2202,	   		//�˷�ǰ�ܹ������
	0x202C2201,	   		//�˷�ǰʣ����
	0x202C8201,			//�˷Ѻ�ʣ����
};
#endif//#if( PREPAY_MODE == PREPAY_LOCAL )
#if( SEL_PRG_INFO_BROADJUST_TIME == YES )		//���ܱ�㲥Уʱ�¼�	0x303B
static const DWORD BroAdjustTimeOadConst[] =
{
	0x02,				// ����
	0x00102201,		//Уʱǰ�����й��ܵ���
	0x00202201,		//Уʱǰ�����й��ܵ��� 
};
#endif

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
BYTE PrgFlag;
#if(SEL_PRG_RECORD645 == YES)
BYTE PrgFlag645; 
BYTE ProRecord645DiNum; //645���һ����̼�¼����δ�����������¼�˼������ݱ�ʶ
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ��ȡ��̼�¼״̬
//
//����: 	Phase[in]             
//����ֵ:  	TRUE/FALSE
//
//��ע: 
//-----------------------------------------------
BYTE GetPrgStatus(BYTE Phase)
{
	if(Phase < 5)
	{
		return FALSE;
	}
	
	return TRUE;
}



//-----------------------------------------------
//��������: �ݴ��̼�¼֮ǰ�������OAD������
//
//����:  	Type[in]: 0--OAD	1--OMD
//			dwData[in]: Ҫ���õĲ���OAD ���ֽ���ǰ�����ֽ��ں�
//                  
//����ֵ: TRUE/FALSE
//
//��ע:��Լ���ñ�̼�¼ʱ�������ò���֮ǰҪ�ȵ��ô˺����� 
//-----------------------------------------------
BOOL api_WritePreProgramData( BYTE Type,DWORD dwData )
{
	WORD wLen,wLen1;
	DWORD dwTemp;
	BYTE Buf[MAX_PRG_CACHE_LEN+30];
	
	if(Type == 0)
	{
		wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE*)&dwData, 0 );
		
		if( (wLen+sizeof(DWORD)) > GET_STRUCT_MEM_LEN( TPreProgramData, Data ) )
		{
			return FALSE;
		}
		
		wLen1 = wLen+sizeof(DWORD);
		
		dwTemp = (dwData|0x00200000);
		memcpy( Buf, (BYTE*)&dwTemp, sizeof(DWORD) );
			
		dwTemp &= (dwData&(~0x00E00000));	
		api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwTemp, NULL, wLen, Buf+sizeof(DWORD) );			
	
		dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[0] );	
		api_WriteToContinueEEPRom( dwTemp, wLen+sizeof(DWORD), Buf );
	
	}
	else
	{
		dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[0] );	
		api_WriteToContinueEEPRom( dwTemp, sizeof(DWORD), (BYTE*)&dwData );
	}
	
	return TRUE;
}


//-----------------------------------------------
//��������: ���̼�¼֮ǰ�ݴ��������OAD������
//
//����:  Len[in]: ���ݳ���
//                
//����ֵ: ��
//
//��ע: 
//-----------------------------------------------
static void ClrPreProgramData( BYTE Len )
{	
	DWORD dwAddr;
	
	if( Len > GET_STRUCT_MEM_LEN( TPreProgramData, Data ) )
	{
		return;
	}
	
	dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[0] );
	
	api_ClrContinueEEPRom( dwAddr, Len );	
}


//-----------------------------------------------
//��������: д�޹����ģʽ�ֱ���¼�����Դ
//
//����:  OI[in]: 0x4113 �޹����ģʽ��1
//               0x4114 �޹����ģʽ��2
//		Pointer[in]:     
//����ֵ: ��
//
//��ע: 
//-----------------------------------------------
#if( SEL_PRG_INFO_Q_COMBO == YES )
static void WriteQcombSource(WORD OI, WORD Pointer)	
{
	DWORD dwTemp;
	BYTE Type;
	
	if(OI == 0x4113)
	{
		Type = 0;
	}
	else if(OI == 0x4114)
	{
		Type = 1;
	}
	else
	{
		return;
	}		

	//д��ǰ�޹����ģʽ�ֱ���¼�����Դ
	dwTemp = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.QcombSource[Pointer] );
	api_WriteToContinueEEPRom( dwTemp, 1, (BYTE *)&Type );	
}
//-----------------------------------------------
//��������: �޹���Ϸ�ʽ2�����ֱ�̴�����1
//
//����:  
//                    
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע: �¼��������7  �¼�����Դ+��������+�ۼ�ʱ��
//-----------------------------------------------
static void AddProgram_Q_Combo2_Num( void )
{	
	DWORD dwTemp;
	
	dwTemp = 0;
	if( api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.dwPrgQComBo2Num ),sizeof(DWORD), (BYTE *)&dwTemp ) == FALSE )
	{
		return ;
	}
	dwTemp++;
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.dwPrgQComBo2Num ),sizeof(DWORD), (BYTE *)&dwTemp );
}	
#endif

//-----------------------------------------------
//��������: �嵱ǰ��̼�¼״̬����Ϊδ��ʼ״̬
//
//����:  	��
//		  
//����ֵ: 	��
//
//��ע: 
//-----------------------------------------------
void ClearPrgRecordRam( void )
{
	PrgFlag = ePROGRAM_NOT_YET_START;
}

//-----------------------------------------------
//��������: ��ȡ��ǰ��̼�¼״̬
//
//����:  	��
//		  
//����ֵ: 	0	ePROGRAM_NOT_YET_START	��̻�δ��ʼ
//			1	ePROGRAM_STARTED		����Ѿ���ʼ
//
//��ע: 
//-----------------------------------------------
BYTE GetPrgRecord( void )
{
	return PrgFlag;
}

//-----------------------------------------------
//��������: �������¼���¼
//
//����: 
//	Type[in]:	EVENT_START:	��̿�ʼ����������ʱ�ÿ�ʼ
//				EVENT_END:		��̽������Ͽ�Ӧ�����ӻ��ߵ���ʱ�ý���
//  DIType[in]:	0x51--OAD	0X53--OMD
//	pOad[in]:	ָ��OAD��ָ��
// 
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע: ֻ���һ��OAD��ʱ��Ϊ��ʼʱ��
//		����������OADֻ����OAD�����ٴ�OAD��Ӧ����
//		���������¼���¼ʱ��pOad��Ч
//-----------------------------------------------
BOOL api_SaveProgramRecord( BYTE Type, BYTE DIType, BYTE *pOad )
{	
	BYTE 				i;
	WORD				wOffSet;
	DWORD 				dwOad,dwOadTemp,dwAddr,dwBaseAddr;
	TPrgProgramSubRom	OadList;
	TEventAttInfo		EventAttInfo;
	TEventOADCommonData	EventOADCommonData;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventSectorInfo	EventSectorInfo;
	BYTE Buf[EVENT_VALID_OADLEN+30];//���벻��������
	
	//�ȶ�ȡ��̼�¼��ָ����Ϣ Info
	EventAddrLen.EventIndex = ePRG_RECORD_NO;
	EventAddrLen.Phase = 0;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}
	
	if(api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo)== FALSE )
	{
		return FALSE;
	}	
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	if( EventAttInfo.AllDataLen > EVENT_VALID_OADLEN )
	{
		return FALSE;
	}
	//�xֵ����ַ
	dwBaseAddr = EventAddrLen.dwRecordAddr;
	if( Type == EVENT_START )//0: ��̿�ʼ
	{
		if( (DIType!=0x51) && (DIType!=0x53) )
		{
			return FALSE;
		}
		
		// ��̻�δ��ʼ
		if( PrgFlag	== ePROGRAM_NOT_YET_START )
		{
			// ��һ��OAD��Ҫ����ʱ�䣬OAD��Ӧ�������
			PrgFlag = ePROGRAM_STARTED;

			memcpy( (BYTE *)&dwOad, pOad, sizeof(DWORD) );

			//��ȡ�����¼���ƫ�Ƶ�ַ
			GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,EventDataInfo.RecordNo, &EventSectorInfo);
			
			EventDataInfo.RecordNo++;
			api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);	
			//------��̶������� FixData ��Ҫ��д�̶����� ���������㲻ִ��!!!
			memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) );
			//�¼���¼���
			EventOADCommonData.EventNo = EventDataInfo.RecordNo;
			//�¼�����ʱ��
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.BeginTime );
			//��������
			dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
			if(WriteEventRecordData( dwAddr, sizeof(TEventOADCommonData), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData ) == FALSE)
			{
				
			}
			//------��������Զ�����е����� Data
			if( EventAttInfo.NumofOad != 0 )
			{
				dwAddr += sizeof(TEventOADCommonData);
				// ��OAD��Ӧ����
				wOffSet = 0;
				
				if( EventAttInfo.OadValidDataLen > EVENT_VALID_OADLEN )
				{
					return FALSE;
				}
								
				memset(Buf,0xff,EventAttInfo.OadValidDataLen);
				for( i = 0; i < EventAttInfo.NumofOad; i++ )
				{
					if(EventAttInfo.Oad[i] != 0xffffffff)
					{
						dwOadTemp = EventAttInfo.Oad[i];
						dwOadTemp &= ~0x00E00000;				
						api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&dwOadTemp, NULL, 0xFF, Buf+wOffSet );
					}
					wOffSet += EventAttInfo.OadLen[i];
				}
				if(WriteEventRecordData( dwAddr, EventAttInfo.OadValidDataLen, dwBaseAddr, EVENT_RECORD_LEN, Buf ) == FALSE)
				{
				
				}
			}

			//�����ϱ�
			SetReportIndex( EventAddrLen.SubEventIndex, EVENT_START );	

			//-------���̶����б� 
			//ˢ���ݴ�ee�����10�����OAD ��OAD OMD��ʶ
			memset( (BYTE*)&OadList, 0x00, sizeof(OadList) );
			dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );
			OadList.SaveOadNum = 1;	
			memcpy((BYTE *)&OadList.Oad[0], pOad, sizeof(DWORD));
			OadList.Type[0] = DIType;			
			api_WriteToContinueEEPRom(dwAddr, sizeof(OadList), (BYTE*)&OadList);			
		}
		else// ����Ѿ���ʼ
		{
			if(EventDataInfo.RecordNo == 0)
			{
				return FALSE;
			}							
			
			//�ۼ��ݴ�ee�ı��OAD����
			dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );
			api_ReadFromContinueEEPRom(dwAddr, sizeof(OadList), (BYTE *)&OadList);
			if( OadList.SaveOadNum < MAX_PROGRAM_OAD_NUMBER )
			{
				OadList.SaveOadNum++;
			}			
			
			//ˢ���ݴ�ee�����10�����OAD
			memmove((BYTE *)&OadList.Oad+sizeof(DWORD), (BYTE *)&OadList.Oad, (MAX_PROGRAM_OAD_NUMBER-1)*sizeof(DWORD) );// ����sizeof(tEeOad)-sizeof(DWORD)�������˾������
			memcpy((BYTE *)&OadList.Oad, pOad, sizeof(DWORD));
			
			//ˢ���ݴ�ee�����10�����OAD OMD��ʶ
			memmove((BYTE *)&OadList.Type+1, (BYTE *)&OadList.Type, MAX_PROGRAM_OAD_NUMBER-1 );// ����sizeof(tEeType)-1�������˾������
			OadList.Type[0] = DIType;
			api_WriteToContinueEEPRom(dwAddr, sizeof(OadList), (BYTE *)&OadList);
		}
	}
	else if( Type == EVENT_END )//1����̽���
	{
		if( PrgFlag == ePROGRAM_STARTED )
		{
			if(EventDataInfo.RecordNo == 0)
			{
				return FALSE;
			}
			//��ȡ�����¼���ƫ�Ƶ�ַ
			GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,(EventDataInfo.RecordNo-1), &EventSectorInfo);
			
			// �ñ�̽�����־��д����ʱ��
			PrgFlag	= ePROGRAM_NOT_YET_START;

			// �����ʱ��
			dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
			//api_ReadMemRecordData( dwAddr, sizeof(EventOADCommonData), (BYTE *)&EventOADCommonData );
			dwAddr += GET_STRUCT_ADDR( TEventOADCommonData, EventTime.EndTime );

			if( pOad == NULL )
			{
				api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.EndTime );			// ��ǰʱ��
			}
			else
			{
				api_GetPowerDownTime( (TRealTimer*)&EventOADCommonData.EventTime.EndTime );								// ����ʱ��
			}
			if(WriteEventRecordData( dwAddr, sizeof(TRealTimer), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData.EventTime.EndTime ) == FALSE)
			{
			
			}
			
			//�����ݴ���ee�еı��OAD�������ٱ��浽��Ӧ�ı�̼�¼�С�
			dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.EeTempOadList );
			api_ReadFromContinueEEPRom(dwAddr, sizeof(OadList), (BYTE *)&OadList);
			dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;
			dwAddr += EventAttInfo.OadValidDataLen;
			if(WriteEventRecordData(dwAddr, sizeof(OadList), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&OadList) == FALSE)
			{
			
			}
			
			//�����ϱ�
			SetReportIndex( EventAddrLen.SubEventIndex, EVENT_END );
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
//��������: ���������ÿ���¼��ı�̼�¼������ܣ�ʱ��ʱ�α��̵ȣ�
//
//����:
//	inEventIndex[in]:	�������ö�ٺ�
//
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:
//-----------------------------------------------
BOOL api_SavePrgOperationRecord( eEVENT_INDEX inEventIndex )
{
	TEventOADCommonData	EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventSectorInfo	EventSectorInfo;
	DWORD 				dwPreOad,dwAddr,dwBaseAddr;
	TFourByteUnion 		tOAD;
	WORD 				twSavePointer,OffSet,wLen,wLen1;
	BYTE 				i;
	BYTE 				Buf[EVENT_VALID_OADLEN+30];
	BYTE 				PreBuf[EVENT_ALONE_OADLEN+30];
	
	EventAddrLen.EventIndex = inEventIndex;
	EventAddrLen.Phase = 0;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return FALSE;
	}

	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	if(api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo) == FALSE )
	{
		return FALSE;
	}

	if( EventAttInfo.AllDataLen > EVENT_VALID_OADLEN )
	{
		return FALSE;
	}
	
	if( EventAttInfo.NumofOad > MAX_EVENT_OAD_NUM )
	{
		return FALSE;
	}
	//�xֵ����ַ
	dwBaseAddr	= EventAddrLen.dwRecordAddr;
	//��ȡ�����¼���ƫ�Ƶ�ַ
	GetEventSectorInfo(EventAttInfo.AllDataLen,EventAttInfo.MaxRecordNo,EventDataInfo.RecordNo, &EventSectorInfo);

	twSavePointer = EventDataInfo.RecordNo%EventAttInfo.MaxRecordNo;
	EventDataInfo.RecordNo++;// һֱ����
	
	//�������֮ǰ�ݴ��OAD����Ӧ�����ݻ�OMD
	dwAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[0] );
	api_ReadFromContinueEEPRom( dwAddr, 4, (BYTE*)&dwPreOad );

	wLen = 0;
	wLen1 = 0;
	//������㡢�������㲻��Ҫ������֮ǰ������
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//��������¼
	if(EventAddrLen.EventIndex == ePRG_CLEAR_METER_NO)
	{
		//��Ϊ��ЧOAD
		dwPreOad = 0xffffffff;
		wLen = 1;
	}
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )//��������¼
	if(EventAddrLen.EventIndex == ePRG_CLEAR_MD_NO)
	{
		//��Ϊ��ЧOAD
		dwPreOad = 0xffffffff;
		wLen = 1;
	}
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//�¼������¼
	if(EventAddrLen.EventIndex == ePRG_CLEAR_EVENT_NO)
	{
		//dwPreOad�д�ŵ���OMD
		wLen = 1;
	}
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)		
	if(EventAddrLen.EventIndex == ePRG_BUY_MONEY_NO)//���ܱ����¼
	{
		//��Ϊ��ЧOAD
		dwPreOad = 0xffffffff;
		wLen = 16;//���淶�ౣ�湺��ǰ�Ĺ�������������û���չ������
		//202C2201��������ǰʣ���� 
		//202D2200��������ǰ͸֧��� 
		//202E2200��������ǰ�ۼƹ����� 	
	}		
	if(EventAddrLen.EventIndex == ePRG_ABR_CARD_NO)//���ܱ��쳣�忨�¼�
	{
		//��Ϊ��ЧOAD
		dwPreOad = 0xffffffff;
		wLen = 18;//����ͷ����ʱ���� CH,Cla,Ins,P1,P2,P31,P32,����CH:0x01--ESAM 0x00--CARD,esam����ͷΪCH���6�ֽڣ���ΪCH���5�ֽ�
	}
	if(EventAddrLen.EventIndex == ePRG_RETURN_MONEY_NO)//���ܱ��˷Ѽ�¼
	{
		//dwPreOad�д�ŵ����˷ѽ��
		wLen = 4;
	}
	#endif
	
	if( RelayTypeConst != RELAY_NO ) //����բ��¼
	{
		if( (EventAddrLen.EventIndex == ePRG_OPEN_RELAY_NO)||(EventAddrLen.EventIndex == ePRG_CLOSE_RELAY_NO) )
		{
			//dwPreOad�д�ŵ���OMD
			wLen = 1;
		}
	}
	
	if(wLen == 0)
	{
		wLen = api_GetProOADLen( eGetRecordData, eData, (BYTE *)&dwPreOad, 0 );	
		if( (wLen+sizeof(DWORD)) > GET_STRUCT_MEM_LEN(TPreProgramData,Data) )
		{
			return FALSE;
		}
	}
	
	api_ReadFromContinueEEPRom( dwAddr+sizeof(DWORD), wLen, PreBuf );
	// д����
	// ��Ҫ��дEventOADCommonData����ᵼ��flash���㲻ִ��
	//���¼���¼��ţ��¼�����ʱ��,�����û���¼�����ʱ��,�ϱ�״̬
	memset( (BYTE *)&EventOADCommonData, 0xff, sizeof(TEventOADCommonData) );

	EventOADCommonData.EventNo = EventDataInfo.RecordNo;
	if(( inEventIndex == ePRG_ADJUST_TIME_NO ) || (inEventIndex == ePRG_BROADJUST_TIME_NO))
	{
		memcpy((BYTE *)&EventOADCommonData.EventTime.BeginTime, PreBuf, sizeof(TRealTimer));
	}
	else
	{
		api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.BeginTime ); 
	}

	api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&EventOADCommonData.EventTime.EndTime );//��¼����ʱ��
			
	dwAddr = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
	if(WriteEventRecordData( dwAddr, sizeof(TEventOADCommonData), dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&EventOADCommonData ) == FALSE)
	{
		
	
	}	
	if( EventAttInfo.NumofOad != 0 )
	{

		memset(Buf,0xff,EventAttInfo.OadValidDataLen);

		// ��������
		OffSet = 0;
		for( i = 0; i < EventAttInfo.NumofOad; i++ )
		{
			tOAD.d = EventAttInfo.Oad[i];

			if( tOAD.d == 0xfffffff )
			{
			}
			#if(PREPAY_MODE == PREPAY_LOCAL)
			else if( (EventAddrLen.EventIndex == ePRG_RETURN_MONEY_NO)&&(dwPreOad == 0X06021033) )//���ܱ��˷Ѽ�¼
			{
				memcpy(Buf+OffSet, (BYTE *)&dwPreOad, EventAttInfo.OadLen[i]);
			}
            else if ((EventAddrLen.EventIndex == ePRG_RETURN_MONEY_NO)&&(tOAD.d == 0x01222C20))
           	{
				memcpy(Buf+OffSet, PreBuf, 4);
           	}  
			//202C2201��������ǰʣ���� tOAD.d=0x02822C20 tOAD.w[0]=0x2c20 tOAD.b[2]=0x22
			//202D2200��������ǰ͸֧��� 
			//202E2200��������ǰ�ۼƹ����� 	
			else if( (EventAddrLen.EventIndex == ePRG_BUY_MONEY_NO)&&((tOAD.w[0] == 0x2C20)||(tOAD.w[0] == 0x2D20)||(tOAD.w[0] == 0x2E20))
				&&((tOAD.b[2] == 0x22)||(tOAD.b[2] == 0x42)) )//���ܱ����¼ ����ǰ��ʣ���͸֧���ۼƹ�����
			{
				if(tOAD.w[0] == 0x2C20)
				{
					if(tOAD.b[3] == 0x02)//����ǰ���������//���淶�ౣ�湺��ǰ�Ĺ�������������û���չ������
					{
						wLen1 = 12;
					}
					else
					{
						wLen1 = 0; //����ǰʣ����
					}
				}
				else if(tOAD.w[0] == 0x2D20)
				{
					wLen1 = 4;
				}
				else//	||(tOAD.w[0] == 0x2E20)
				{
					wLen1 = 8;
				}	
				memcpy(Buf+OffSet, PreBuf+wLen1, 4);
			}
			#endif
			else if( tOAD.d == dwPreOad )//���ǰ�ݴ��OAD����Ӧ����
			{
				#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
				if(EventAddrLen.EventIndex == ePRG_CLEAR_EVENT_NO)
				{
					//�¼�����Ҫ����OMD
					memcpy(Buf+OffSet, (void*)&dwPreOad, sizeof(dwPreOad));
				}
				else
				#endif
				{
					memcpy(Buf+OffSet, PreBuf, wLen);
				}
			}
			else
			{
				tOAD.d &= ~0x00E00000;
				api_GetProOadData( eGetRecordData, eData, 0xff, (BYTE *)&tOAD.d, NULL, 0xFF, Buf+OffSet );			
			}

			OffSet += EventAttInfo.OadLen[i];
		}
		
		// д��¼����
		dwAddr+=sizeof(TEventOADCommonData);// �ӿ�ʼλ��ֱ��ƫ�ƹ̶����ݵĳ��Ⱦ���������
		if(WriteEventRecordData( dwAddr, EventAttInfo.OadValidDataLen, dwBaseAddr, EVENT_RECORD_LEN, Buf ) == FALSE)
		{
		
		}
	}
	// ʱ�α��̵Ļ�������ʱ�α�
	#if(SEL_PRG_INFO_TIME_TABLE==YES)
	if(EventAddrLen.EventIndex==ePRG_TIME_TABLE_NO)
	{
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;
		dwAddr += EventAttInfo.OadValidDataLen;// ƫ��
		tOAD.d = dwPreOad & (~0x00E00000);//ʱ�α�����
		memcpy(&PreBuf[wLen],(BYTE *)&tOAD.d,sizeof(DWORD));
		if(WriteEventRecordData( dwAddr, wLen+sizeof(DWORD), dwBaseAddr, EVENT_RECORD_LEN, PreBuf) == FALSE)
		{
		
		}
	}
	#endif
	// �ڼ��ձ�̣�������ǰ�ڼ���
	#if(SEL_PRG_INFO_HOLIDAY==YES)
	if(EventAddrLen.EventIndex==ePRG_HOLIDAY_NO)
	{
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;
		dwAddr += EventAttInfo.OadValidDataLen;// ƫ��
		tOAD.d = dwPreOad & (~0x00E00000);//�ڼ��գ�����
		memcpy(&PreBuf[wLen],(BYTE *)&tOAD.d,sizeof(DWORD));
		if(WriteEventRecordData( dwAddr, wLen+sizeof(DWORD), dwBaseAddr, EVENT_RECORD_LEN, PreBuf) == FALSE)
		{
		
		}
	}
	#endif
	
	#if(PREPAY_MODE == PREPAY_LOCAL)			
	if(EventAddrLen.EventIndex == ePRG_ABR_CARD_NO)//���ܱ��쳣�忨�¼�
	{
		//wLen = 18;//����ͷ����ʱ���� CH,Cla,Ins,P1,P2,P31,P32,����CH:0x01--ESAM 0x00--CARD,esam����ͷΪCH���6�ֽڣ���ΪCH���5�ֽ�
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;// ��ȷ��dwAddr�ϴε�λ�ã���Ҫ���¶�λ
		dwAddr += EventAttInfo.OadValidDataLen;// ƫ��
		if(WriteEventRecordData( dwAddr, wLen, dwBaseAddr, EVENT_RECORD_LEN, PreBuf ) == FALSE)
		{
		}
	}
	//ePRG_BUY_MONEY_NO,							//���ܱ����¼					
	if(EventAddrLen.EventIndex == ePRG_RETURN_MONEY_NO)//���ܱ��˷Ѽ�¼
	{
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;// ��ȷ��dwAddr�ϴε�λ�ã���Ҫ���¶�λ
		dwAddr += EventAttInfo.OadValidDataLen;// ƫ��
		if(WriteEventRecordData( dwAddr, wLen, dwBaseAddr, EVENT_RECORD_LEN, (BYTE *)&dwPreOad )== FALSE)//dwPreOad�д�ŵ����˷ѽ��
		{
		}
	}
	#endif
	
	//���� �¼������б�  array OMD ���˴�����ֻ֧��һ��OMD
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )
	if(EventAddrLen.EventIndex == ePRG_CLEAR_EVENT_NO)
	{
		dwAddr = EventAddrLen.dwRecordAddr + sizeof(TEventOADCommonData) + EventSectorInfo.dwSectorAddr;// ��ȷ��dwAddr�ϴε�λ�ã���Ҫ���¶�λ
		dwAddr += EventAttInfo.OadValidDataLen;// ƫ��
		if(WriteEventRecordData( dwAddr, sizeof(DWORD), dwBaseAddr, EVENT_RECORD_LEN, (BYTE*)&dwPreOad ) == FALSE)
		{
		
		}
	}
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )
	if(EventAddrLen.EventIndex == ePRG_Q_COMBO_NO)
	{
		memcpy( (BYTE*)&OffSet, (BYTE*)&dwPreOad, sizeof(WORD));
		lib_InverseData( (BYTE*)&OffSet, sizeof(WORD) );
		WriteQcombSource(OffSet, twSavePointer);
		if( OffSet == 0x4114 ) //4114	8	�޹���Ϸ�ʽ2������
		{
			AddProgram_Q_Combo2_Num();
			api_WriteToContinueEEPRom(GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PrgQCombo_CurrentEventStatus[1] ),sizeof(TRealTimer),(BYTE *)&(EventOADCommonData.EventTime.BeginTime) );
		}
		else
		{
			api_WriteToContinueEEPRom(GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PrgQCombo_CurrentEventStatus[0] ),sizeof(TRealTimer),(BYTE *)&(EventOADCommonData.EventTime.BeginTime) );
		}		
	}
	#endif
	
	api_VWriteSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	//���̼�¼֮ǰ�ݴ��������OAD������
	ClrPreProgramData(wLen+1);// �Ƿ��б�Ҫ���Ƿ��б�Ҫ�����ô��

	//�����¼�����¼���¼���ٴ���λ���е����ĵط�������λ
	if( (inEventIndex != ePRG_CLEAR_METER_NO) && (inEventIndex != ePRG_CLEAR_EVENT_NO) )
	{
		SetReportIndex( EventAddrLen.SubEventIndex, EVENT_START+EVENT_END );
	}
	
	return TRUE;
}

#if(SEL_PRG_RECORD645 == YES)
//���ܱ����¼�645
static BOOL GetPrg645RecordInfo( TEventAddrLen *pTEventAddrLen )
{
	//pTEventAddrLen->dwAttInfoEeAddr = GET_CONTINUE_ADDR( EventConRom.EventAttInfo[pTEventAddrLen->EventIndex] );
	pTEventAddrLen->dwDataInfoEeAddr = GET_SAFE_SPACE_ADDR( EventSafeRom.ProRecord645 );
	pTEventAddrLen->dwRecordAddr = GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PrgProgramRom645[0].Time );
//	pTEventAddrLen->wDataLen = sizeof(TPrgProgramRom645) * MAX_EVENTRECORD_NUMBER;//@@@@@��ا��

	return TRUE;
}

//-----------------------------------------------
//��������: ����645��Լ����¼���¼
//
//����:
//	Type[in]:	EVENT_START:	��̿�ʼ����������ʱ�ÿ�ʼ
//				EVENT_END:		��̽������Ͽ�Ӧ�����ӻ��ߵ���ʱ�ý���
//	Op[in]:		�����ߴ��� ����
//	pDI[in]:	ָ��DI��ָ�� ����
//
//  ����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:��������ʱTypeΪEVENT_START������֤�����ĵ���TypeΪEVENT_END
//-----------------------------------------------
BOOL api_SaveProgramRecord645( BYTE Type, BYTE *Op, BYTE *pDI )
{
	WORD				twSavePointer;
	DWORD 				dwAddr;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TRealTimer			t;
	TPrgProgramRom645	tPrgProgramRom645;
	TFourByteUnion dwDI;
	
	memcpy(dwDI.b,pDI,4);
		
	//����ֻͨ��OI����λ�����ϱ�״̬�����
	if( dwDI.d == 0x04001503 )
	{
		return FALSE;
	}

	memset( tPrgProgramRom645.Time, 0xff, sizeof(TPrgProgramRom645) );

	//�ȶ�ȡ��̼�¼��ָ����Ϣ Info	
	GetPrg645RecordInfo( &EventAddrLen );
	//��ȡRecordNo
	if(api_VReadSafeMem( EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo ) == FALSE )
	{
		return FALSE;
	}
	
	if( Type == EVENT_START ) //0: ��̿�ʼ
	{
		// ��̻�δ��ʼ
		if( PrgFlag645 == ePROGRAM_NOT_YET_START )
		{
			// ��һ��OAD��Ҫ����ʱ�䣬OAD��Ӧ�������
			PrgFlag645 = ePROGRAM_STARTED;
			
			twSavePointer = EventDataInfo.RecordNo % MAX_EVENTRECORD_NUMBER;
			dwAddr = EventAddrLen.dwRecordAddr + sizeof(TPrgProgramRom645) * twSavePointer; 
	
			EventDataInfo.RecordNo++;
			api_VWriteSafeMem( EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo );
	
			//�¼�����ʱ��
			api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&t );
			api_TimeFormat698To645( &t, tPrgProgramRom645.Time );
			lib_InverseData( tPrgProgramRom645.Time, 6 );
			//�����ߴ���
			memcpy( tPrgProgramRom645.Op, Op, 4 );
			//��������
			memcpy( tPrgProgramRom645.DI, pDI, 4 );
						
			api_WriteToContinueEEPRom( dwAddr, sizeof(TPrgProgramRom645), tPrgProgramRom645.Time );

			//�����ϱ�			
			SetReportIndex( eSUB_EVENT_PRG_RECORD, EVENT_START );	
			
			ProRecord645DiNum = 1;
		}
		else // ����Ѿ���ʼ
		{
			if( EventDataInfo.RecordNo == 0 )
			{
				return FALSE;
			}
			
			twSavePointer = (EventDataInfo.RecordNo-1) % MAX_EVENTRECORD_NUMBER;
			dwAddr = EventAddrLen.dwRecordAddr + sizeof(TPrgProgramRom645) * twSavePointer;
			
			api_ReadFromContinueEEPRom( dwAddr + 10, sizeof(TPrgProgramRom645) - 10, tPrgProgramRom645.DI ); 
			
			if( ProRecord645DiNum <= 10 )
			{
				memmove( tPrgProgramRom645.DI+4, tPrgProgramRom645.DI, (MAX_EVENTRECORD_NUMBER - 1) * 4 );
				memcpy( tPrgProgramRom645.DI, pDI, 4 );
			}
			else
			{
				return FALSE;
			}
					
			//����ʱ�估�����ߴ��벻���ظ�д
			api_WriteToContinueEEPRom( dwAddr+10, sizeof(TPrgProgramRom645)-10, tPrgProgramRom645.DI ); 
		}	
	}
	else
	{
		// ����ѿ�ʼ
		if( PrgFlag645 == ePROGRAM_STARTED )
		{
			SetReportIndex( eSUB_EVENT_PRG_RECORD, EVENT_END );
		}
		PrgFlag645 = ePROGRAM_NOT_YET_START;
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: 645��Լ����¼���¼��03300000~0330000a��
//
//����:
//	DI[in]:		��ȡ���ݵ����ݱ�ʶ
//				EVENT_END:		��̽������Ͽ�Ӧ�����ӻ��ߵ���ʱ�ý���
//	pOutBuf[out]:	���ص�����
//
//  ����ֵ:	���ݳ��� 8000�������
//
//��ע:֧�ֱ�̴��������10�α�̼�¼�Ķ�ȡ
//-----------------------------------------------
WORD api_ReadProgramRecord645( BYTE *DI, BYTE *pOutBuf )
{
	TFourByteUnion 		tdw;
	WORD				twSavePointer;
	DWORD 				dwAddr;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	
	memcpy( tdw.b, DI, 4 );
	if( (tdw.d > 0x0330000a)||(tdw.d < 0x03300000) )
	{
		return 0x8000;
	}
	
	//�ȶ�ȡ��̼�¼��ָ����Ϣ Info
	GetPrg645RecordInfo( &EventAddrLen );
	//��ȡRecordNo
	api_VReadSafeMem( EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo );
	
	if( tdw.d == 0x03300000 )
	{
		dwAddr = lib_DWBinToBCD( EventDataInfo.RecordNo );
		memcpy( pOutBuf, (BYTE *)&dwAddr, 3 );
		
		return 3;
	}
	else
	{
		if( tdw.b[0] > EventDataInfo.RecordNo )
		{
			memset( pOutBuf, 0x00, sizeof(TPrgProgramRom645) );
		}
		else
		{
			twSavePointer = (EventDataInfo.RecordNo - tdw.b[0])% MAX_EVENTRECORD_NUMBER;
			dwAddr = EventAddrLen.dwRecordAddr + sizeof(TPrgProgramRom645) * twSavePointer;
			
			api_ReadFromContinueEEPRom( dwAddr, sizeof(TPrgProgramRom645), pOutBuf );						
		}
	}
	
	return sizeof(TPrgProgramRom645); 
}
#endif//#if(SEL_PRG_RECORD645 == YES)

//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void FactoryInitPrgRecordPara( void )
{
	//���ܱ����¼� 0x3012
	api_WriteEventAttribute( 0x3012, 0xFF, (BYTE *)&ProgramOadConst[0], 		sizeof(ProgramOadConst)/sizeof(DWORD) );
	
	#if( SEL_PRG_INFO_CLEAR_METER == YES )		//���ܱ������¼� 0x3013
	api_WriteEventAttribute( 0x3013, 0xFF, (BYTE *)&ClearEnergyOadConst[0], 	sizeof(ClearEnergyOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_MD == YES )			//���ܱ����������¼� 0x3014
	api_WriteEventAttribute( 0x3014, 0xFF, (BYTE *)&ClearMDOadConst[0], 		sizeof(ClearMDOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )		//���ܱ��¼������¼� 0x3015
	api_WriteEventAttribute( 0x3015, 0xFF, (BYTE *)&ClearEventOadConst[0], 	sizeof(ClearEventOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )		//���ܱ�Уʱ�¼�	0x3016
	api_WriteEventAttribute( 0x3016, 0xFF, (BYTE *)&AdjustTimeOadConst[0], 	sizeof(AdjustTimeOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_INFO_TIME_TABLE == YES )		//���ܱ�ʱ�α����¼� 0x3017
	api_WriteEventAttribute( 0x3017, 0xFF, (BYTE *)&TimeTableOadConst[0], 	sizeof(TimeTableOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_INFO_TIME_AREA == YES )		//���ܱ�ʱ�������¼� 0x3018
	api_WriteEventAttribute( 0x3018, 0xFF, (BYTE *)&TimeAreaOadConst[0], 		sizeof(TimeAreaOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_WEEK == YES )				//���ܱ������ձ���¼� 0x3019
	api_WriteEventAttribute( 0x3019, 0xFF, (BYTE *)&WeekOadConst[0], 			sizeof(WeekOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )		//���ܱ�����ձ���¼� 0x301a
	api_WriteEventAttribute( 0x301a, 0xFF, (BYTE *)&ClosingDayOadConst[0], 	sizeof(ClosingDayOadConst)/sizeof(DWORD) );
	#endif
	
	//���ܱ���բ�¼� ���ܱ��բ�¼� ��բ 0x301F ��բ 0x3020
	api_WriteEventAttribute( 0x301F, 0xFF, (BYTE *)&OpenCloseRelayOadConst[0], sizeof(OpenCloseRelayOadConst)/sizeof(DWORD) );
	api_WriteEventAttribute( 0x3020, 0xFF, (BYTE *)&OpenCloseRelayOadConst[0], sizeof(OpenCloseRelayOadConst)/sizeof(DWORD) );
	
	#if( SEL_PRG_INFO_HOLIDAY == YES )			// �ڼ��ձ�̼�¼ 0x3021	
	api_WriteEventAttribute( 0x3021, 0xFF, (BYTE *)&HolidayOadConst[0], 		sizeof(HolidayOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_INFO_P_COMBO == YES )			// �й���Ϸ�ʽ��̼�¼ 0x3022	
	api_WriteEventAttribute( 0x3022, 0xFF, (BYTE *)&PCombOadConst[0], 		sizeof(PCombOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_INFO_Q_COMBO == YES )			// �޹���Ϸ�ʽ��̼�¼ 0x3023	
	api_WriteEventAttribute( 0x3023, 0xFF, (BYTE *)&QCombOadConst[0], 		sizeof(QCombOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( SEL_PRG_UPDATE_KEY == YES )			//���ܱ���Կ�����¼� 0x3026	
	api_WriteEventAttribute( 0x3026, 0xFF, (BYTE *)&UpdateKeyOadConst[0], 	sizeof(UpdateKeyOadConst)/sizeof(DWORD) );	
	#endif
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//0x3024	���ܱ���ʲ��������¼�
	api_WriteEventAttribute( 0x3024, 0xFF, (BYTE *)&TariffRateOadConst[0], 	sizeof(TariffRateOadConst)/sizeof(DWORD) );
	//0x3025	���ܱ���ݱ����¼�
	api_WriteEventAttribute( 0x3025, 0xFF, (BYTE *)&LadderOadConst[0], 	sizeof(LadderOadConst)/sizeof(DWORD) );
	//0x3027	���ܱ��쳣�忨�¼�
	api_WriteEventAttribute( 0x3027, 0xFF, (BYTE *)&AbnormalCardOadConst[0], 	sizeof(AbnormalCardOadConst)/sizeof(DWORD) );
	//0x3028	���ܱ����¼
	api_WriteEventAttribute( 0x3028, 0xFF, (BYTE *)&BuyEnergyOadConst[0], 	sizeof(BuyEnergyOadConst)/sizeof(DWORD) );
	//0x3029	���ܱ��˷Ѽ�¼
	api_WriteEventAttribute( 0x3029, 0xFF, (BYTE *)&ReturnMoneyOadConst[0], 	sizeof(ReturnMoneyOadConst)/sizeof(DWORD) );
	#endif//#if( PREPAY_MODE == PREPAY_LOCAL )
	#if( SEL_PRG_INFO_BROADJUST_TIME == YES )		//���ܱ�㲥Уʱ�¼�	0x303B
	api_WriteEventAttribute( 0x303C, 0xFF, (BYTE *)&BroAdjustTimeOadConst[0], 	sizeof(BroAdjustTimeOadConst)/sizeof(DWORD) );	
	#endif
}

