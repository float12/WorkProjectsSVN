//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	κ���� ������
//��������	2016.10.26
//����		698��Լ�ϱ�
//�޸ļ�¼	
//---------------------------------------------------------------------- 

#include "AllHead.h"

#if ( SEL_DLT698_2016_FUNC == YES)

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define LostPowerEndTimeThreshold	3540//�����¼��ϱ����¼�����ʱ����ֵ
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern const TSubEventInfoTab	SubEventInfoTab[];
extern const TEventInfoTab EventInfoTab[];
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
__no_init TReportMode		ReportMode;		//�¼��ϱ���ʶ bit:�¼���Ч��ʶ

__no_init TReportData 		ReportData[MAX_COM_CHANNEL_NUM];//���������ͨ��
__no_init TReportData 		ReportDataBak[MAX_COM_CHANNEL_NUM];//����������ͨ�����ݣ������¼��б�ֻ�ز�ͨ������
__no_init TReportTimes 		ReportTimes;//�����ϱ������������E2

TReportTimes 		LostPowerReportTimes;//���������ϱ��������ϵ����㣬����E2

WORD ReportOverTime;//�����ϱ���ʱ
WORD ReportPowerDownTime;//�����¼������ϱ���ʱ

WORD EventLostPowerEndTime; //�����¼�����ʱ�䣬�ۼӣ���λ�룩

BYTE LostPowerReportBuf[MAX_PRO_BUF+30];
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//��������: 	�ϱ�������1
//
//����: 		��
//        	
//����ֵ:		��
//			
//��ע:
//-----------------------------------------------
static void ReportTimesReduce1( eActiveReportMode ActiveReportMode  )
{
	if( ActiveReportMode == eActiveReportPowerDown )         
	{
		if( LostPowerReportTimes.Times != 0 )
		{
			if( LostPowerReportTimes.Times > 3 )//����ֵ�ж�
			{
				LostPowerReportTimes.Times = 0;
			}
			else if( LostPowerReportTimes.Times != 0 )
			{
				LostPowerReportTimes.Times --; 
			}
			LostPowerReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD));

			//����Ϊ0��ʱ�����buf
			if( LostPowerReportTimes.Times == 0 )
			{
				memset(LostPowerReportBuf,0x00,sizeof(LostPowerReportBuf));
			}
		}
	}
	else
	{
		if( ReportTimes.Times != 0 )
		{
			if( ReportTimes.Times > 3 )//����ֵ�ж�
			{
				ReportTimes.Times = 0;
			}
			else if( ReportTimes.Times != 0 )
			{
				ReportTimes.Times --; 
			}
			ReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
		}
	}

	

}

//-----------------------------------------------
//��������: 	�����ϱ�����
//
//����: 		Mode[in] :	0x00:������ϱ�����
//						0x01:�������ϱ�����
//						0xFF:���ϱ�����
//        	
//����ֵ:		��
//			
//��ע: 
//-----------------------------------------------
static void ClearReportTimes(BYTE Mode )
{
	if( Mode == eActiveReportPowerDown )
	{
		memset( &LostPowerReportTimes, 0x00, sizeof(LostPowerReportTimes) );
		LostPowerReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
		memset(LostPowerReportBuf,0x00,sizeof(LostPowerReportBuf));
	}
	else if( Mode == eActiveReportNormal )
	{
		memset( &ReportTimes, 0x00, sizeof(ReportTimes) );
		ReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
	}
	else
	{
		memset( &ReportTimes, 0x00, sizeof(ReportTimes) );
		ReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
		memset( &LostPowerReportTimes, 0x00, sizeof(LostPowerReportTimes) );
		LostPowerReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
		memset(LostPowerReportBuf,0x00,sizeof(LostPowerReportBuf));
	}
	
}

//-----------------------------------------------
//��������: 	�����ϱ����� 3��
//
//����: 		Type 0
//        	
//����ֵ:		��
//			
//��ע:
//-----------------------------------------------
static void ResetReportTimes( eActiveReportMode ActiveReportMode )
{
	if( ActiveReportMode == eActiveReportPowerDown)
	{	
		ReportPowerDownTime = 0;
		LostPowerReportTimes.Times = 3; 
		LostPowerReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
	}
	else
	{
		ReportOverTime = 0;//��ʼ���ϱ���ʱʱ��
		ReportTimes.Times = 3; 
		ReportTimes.CRC32 = lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD));
	}

	
}

//-----------------------------------------------
//��������: 	�����ϱ�ʱ�����
//
//����: 		��
//        	
//����ֵ:		��
//			
//��ע:�����ϱ�ʱ�䴦������systick�ж��У�Խ����Խ��
//-----------------------------------------------
void ReportTimeMinus( )
{
	 if( ReportOverTime > 0 )//�����ϱ���ʱ
	{
		ReportOverTime --;
	}

	if( ReportPowerDownTime > 0 )//���������ϱ���ʱ
	{
		ReportPowerDownTime --;
	}

}
//-----------------------------------------------
//��������: 	�����ϱ�ram���ݰ����ϱ�����-�ϱ����ݱ���
//
//����: 		BYTE Ch[in] ͨ�� 0xff ȫͨ�� 
//����ֵ:		��
//			
//��ע:
//-----------------------------------------------
static void ClearReportDataRam( BYTE Ch )
{
	BYTE i;
	
	//�����ϱ�����
	for( i=0 ; i < MAX_COM_CHANNEL_NUM; i++ )
	{
		if( (Ch == 0xff) || (i == Ch) )
		{
			//�����Ӧ���ϱ�����
			memset((BYTE *)&(ReportData[i]), 0x00, sizeof(TReportData));
			memset((BYTE *)&(ReportData[i].Index), 0xFF, sizeof(ReportData[0].Index));
			ReportData[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[i],sizeof(TReportData)-sizeof(DWORD));		
			//�����Ӧ�ĸ����ϱ�״̬�ֱ���
			memset((BYTE *)&(ReportDataBak[i]), 0x00, sizeof(TReportData));
			//�����ϱ����ݱ���
			memset((BYTE *)&(ReportDataBak[i].Index), 0xFF, sizeof(ReportDataBak[0].Index));
			ReportDataBak[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD));
		}

	}
}


//-----------------------------------------------
//��������: 	�ϱ�ram����У��
//
//����: 		BYTE Ch[in] У��ָ��ͨ��������
//        	
//����ֵ:		��
//			
//��ע:
//-----------------------------------------------
static void ReportDataCheck( BYTE Ch )
{
	BYTE i;
	
	//�����ϱ�����
	for( i=0 ; i < MAX_COM_CHANNEL_NUM; i++ )
	{
		if( (Ch == 0xff) || (i == Ch) )
		{
			//�ϱ�����crc���� �������� ����e2�ָ� ���e2�ָ��п��ܻᵼ���ظ��ϱ�
			if( ReportData[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportData[i],sizeof(TReportData)-sizeof(DWORD) ))
			{
				ClearReportDataRam( i );
			}
			
			//�����ϱ����ݴ��� ������������
			if( ReportDataBak[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD) ))
			{
				//�����ϱ����ݱ���
				memset((BYTE *)&(ReportDataBak[i]), 0x00, sizeof(TReportData));
				memset((BYTE *)&(ReportDataBak[i].Index), 0xFF, sizeof(ReportDataBak[0].Index));
				ReportDataBak[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD));
			}

			if( i == eCR)//�ز�ͨ�������ϱ��������
			{
				//�ϱ��������� �����ϱ�����
				if( ReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD) ))
				{
					ClearReportTimes(eActiveReportNormal);
				}
				//�ϱ��������� �����ϱ�����
				if( LostPowerReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD) ))
				{
					ClearReportTimes(eActiveReportPowerDown);
				}

			}
		}
	}
}

//-----------------------------------------------
//��������: 	�ϱ�����У��
//
//����: 		BYTE Ch[in] У��ָ��ͨ��������
//        	
//����ֵ:		��
//			
//��ע:
//-----------------------------------------------
static void ReportParaCheck( void )
{
	BYTE i,j,k;
	WORD Result;
	
	//ģʽ��У��
	if( ReportMode.CRC32 != lib_CheckCRC32((BYTE*)&ReportMode,sizeof(ReportMode)-sizeof(DWORD)) )
	{
		Result = api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode);
		if( Result == FALSE )
		{
			ReportMode.bMeterReportFlag = MeterReportFlagConst;
			ReportMode.bMeterActiveReportFlag = MeterActiveReportFlagConst;
			//�����ϱ�״̬�� �ϱ���ʽ
			ReportMode.byReportStatusMethod = MeterReportStatusFlagConst;
			//��ʼ��ΪĬ�ϸ����ϱ�
			memset( &ReportMode.byReportMethod, 0xff, sizeof(ReportMode.byReportMethod) );
			//�¼������ϱ���ʽ
			for( i = 0; i< ReportActiveMethodConst[0]; i++ )
			{
				j = (ReportActiveMethodConst[i+1]%eNUM_OF_EVENT_PRG) / 8;
				k = ReportActiveMethodConst[i+1] % 8;
				ReportMode.byReportMethod[j] &= ~(0x01<<k);
			}

			memcpy( (void *)&(ReportMode.byFollowReportMode[0]), FollowReportModeConst, GET_STRUCT_MEM_LEN(TReportMode, byFollowReportMode) );//20150400 ���ܱ�����ϱ�ģʽ��
			memset( (void *)&(ReportMode.ReportChannelOAD[0]), 0x00, (sizeof(DWORD)*MAX_COM_CHANNEL_NUM) );

			//д�ϱ�ͨ��
			for(i=0;i<FollowReportChannelConst[0];i++)
			{
				ReportMode.ReportChannelOAD[i]=ChannelOAD[FollowReportChannelConst[i+1]];
			}
			memset( &ReportMode.byReportMode, 0x00, sizeof(ReportMode.byReportMode) );
			//�¼��ϱ�ʱ��
			for( i = 0; i< ReportModeConst[0].EventIndex; i++ )
			{
				ReportMode.byReportMode[ReportModeConst[i+1].EventIndex] = ReportModeConst[i+1].ReportMode;
			}
			ReportMode.CRC32 = lib_CheckCRC32((BYTE*)&ReportMode,sizeof(ReportMode)-sizeof(DWORD));		
		}
	}

}

//-----------------------------------------------
//��������: ��λ�����ϱ�״̬�ֶ�λ��״̬λ
//
//����: 
//			StatusNo[in]		ϵͳ״̬��
//                    
//����ֵ:  	��
//
//��ע: ֻ���ڸ�״̬λ��0��1ʱ�ŵ��ñ�����  
//-----------------------------------------------
void api_SetFollowReportStatus(BYTE StatusNo)
{
	BYTE i, j,Ch;
	
	ASSERT( StatusNo < eMAX_FOLLOW_REPORT_STATUS_NUM, 0 );
	
	i = (StatusNo%eMAX_FOLLOW_REPORT_STATUS_NUM) / 8;
	j = StatusNo % 8;
	
	if( ReportMode.byFollowReportMode[i] & (0x01<<j) )
	{
		ReportDataCheck( 0xff );
		
		for( Ch=0; Ch < MAX_COM_CHANNEL_NUM; Ch++ )
		{
//			if( Ch == eIR )
//			{
//				continue;
//			}
			
			ReportData[Ch].Status[i] |= (0x01<<j);
			ReportData[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[Ch],sizeof(TReportData)-sizeof(DWORD));
		}
		
		if( ReportMode.byReportStatusMethod == 0 )//�ϱ�״̬���ϱ���ʽΪ�����ϱ�
		{
			//�����ϱ����� ���½����ϱ�
			ResetReportTimes( eActiveReportNormal );
		}
	}
}

//-----------------------------------------------
//��������: ���������¼��б�
//
//����: 
//		inSubEventIndex[in]:	ʱ��������
//  	BeforeAfter[in]:		EVENT_START	BIT0	
//  	                		EVENT_END	BIT1
//����ֵ:	��
//			
//��ע:3320������2�����Ƿ����������߱������� EVENT_START+EVENT_END
//-----------------------------------------------
void SetReportIndex( BYTE inSubEventIndex, BYTE BeforeAfter )
{
	BYTE i,j,n,m,k,tEventIndex;
	BYTE Index[SUB_EVENT_INDEX_MAX_NUM+10];

	
	if( inSubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
	{
		return;
	}

    if( ( inSubEventIndex == eSUB_EVENT_PRG_CLEAR_METER )//��Կ�µ�����㲻�����ϱ�
    && ( api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == TRUE ) )
    {
		return;
    }
    
    tEventIndex = SubEventInfoTab[inSubEventIndex].EventIndex;

	//�Ƿ�Ϊ���ϱ�OI
	if( (ReportMode.byReportMode[tEventIndex]&BeforeAfter) == 0x00 )
	{
		return;
	}
	
	ReportDataCheck( 0xff );
	
	m = ( tEventIndex % eNUM_OF_EVENT_PRG ) / 8;
	k = ( tEventIndex % 8 );

	//��������¼��б�Ҫ�жϸ����ϱ���־���¼��ϱ���ʶ				
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		n = 1;
		
	    if( i == eIR )//����ͨ����������λ��ֱ������
	    {
           continue;
	    }
	    if((inSubEventIndex == eSUB_EVENT_LOST_POWER) //ֱͨ��ĵ����¼��¼���ʼ�����ϱ��������3320��
	    && (BeforeAfter == EVENT_START) && ( MeterVoltageConst == METER_220V )
	    &&( (ReportMode.byReportMethod[m] & (0x01<<k)) == 0 ))
	    {
			continue;
	    }

		if(	  (inSubEventIndex == eSUB_EVENT_LOST_POWER)&&(BeforeAfter == EVENT_END)
			&&( (ReportMode.byReportMethod[m] & (0x01<<k)) == 0 )
			&&( api_GetRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG) == FALSE) )
		{
			continue;
		}

	    memcpy( Index, ReportData[i].Index, SUB_EVENT_INDEX_MAX_NUM );//��������
		memset( ReportData[i].Index, 0xff, sizeof(ReportData[0].Index) );//��ʼ�����½�������
		ReportData[i].Index[0] = inSubEventIndex;
		
		for(j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++)
		{

			if((Index[j] != 0xff) && (Index[j] != inSubEventIndex ) )//��ǰ������������
			{
				if( n < SUB_EVENT_INDEX_MAX_NUM )//��ֹ���ݳ��� ���۲������
				{
					ReportData[i].Index[n] = Index[j];
					n++;
				}
			}
			
		}
		
	    ReportData[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[i],sizeof(TReportData)-sizeof(DWORD));		

	    #if( SEL_DLT645_2007 == YES )
        api_AddReportTimes( i,0x00, inSubEventIndex );
        #endif
	}
	
	if( (ReportMode.byReportMethod[m] & (0x01<<k)) == 0 )//Ϊ�����ϱ�
	{
		if( inSubEventIndex == eSUB_EVENT_LOST_POWER )
		{
			if( BeforeAfter & EVENT_END )//��������б�־
			{
				
				//���緢���ϱ����õ��ǻָ�֮ǰ��ʱ�䣬�õ��粻��ʧ��־��
				if(api_GetRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG) == TRUE)
				{
					api_ClrRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG);//�������־
					ResetReportTimes( eActiveReportPowerDown);	//���õ����ϱ����������ϱ�

				}
				//�����¼�����ʱ����Ҫ��������ʱ��
				EventLostPowerEndTime =0;
			}
			else if( BeforeAfter & EVENT_START )//���緢����ʱ��
			{
				if( MeterVoltageConst == METER_220V )//ֻ��220V�������ϱ�
				{
					ClearReportTimes( eActiveReportPowerDown);	//���õ����ϱ����������ϱ�
				
					//�ж��Ƿ����1Сʱ�ϱ�
					//��������ϱ���ʱ�䣬����ΪҪ��ֹ�˳��͹��ĺ��ϵ�ʱ�����ϱ�������
					if(EventLostPowerEndTime > LostPowerEndTimeThreshold)
					{
						ResetReportTimes( eActiveReportPowerDown);	//���õ����ϱ����������ϱ�
						
						ReportPowerDownTime = 501;//���粻����͹��ĵ��ϱ�����5����ʱ,5010ms
					}
				}    
			}
		}
		else
		{
			ResetReportTimes( eActiveReportNormal );	//���������ϱ����������ϱ���
		}
	}
}

//-----------------------------------------------
//��������: ��ָ��ͨ�������¼��б��е�ָ���¼�
//
//����: 
//		Ch[in]:				ͨ����
//  	inSubEventIndex[in]:�¼�������	
//  	                	
//����ֵ:	��
//			
//��ע:��ͨ��
//-----------------------------------------------
void ClrReportIndexChannel( BYTE Ch, BYTE inSubEventIndex )
{
	BYTE j,n;
	BYTE Index[SUB_EVENT_INDEX_MAX_NUM+10];
	
	if( inSubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
	{
		return;
	}
	
	if( Ch >= MAX_COM_CHANNEL_NUM )
	{
		return;
	}
	
	ReportDataCheck( Ch );
		
	//������������
	memcpy( Index, ReportData[Ch].Index, SUB_EVENT_INDEX_MAX_NUM );//��������
	memset( ReportData[Ch].Index, 0xff, sizeof(ReportData[0].Index) );//��ʼ�����½�������
	n = 0;
	
	for(j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++)
	{
	
		if((Index[j] != 0xff) && (Index[j] != inSubEventIndex ) )//��ǰ������������
		{
			ReportData[Ch].Index[n] = Index[j];
			n++;
		}
	}
	
	//���645���ϱ�����
	#if( SEL_DLT645_2007 == YES )
	ClearReport645Times( Ch, 0x00, inSubEventIndex );
	#endif

	ReportData[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[Ch],sizeof(TReportData)-sizeof(DWORD));		
}
//-----------------------------------------------
//��������: ������ͨ�������¼��б��е�ָ���¼�
//
//����: 
//  	inSubEventIndex[in]:�¼�������	
//  	                	
//����ֵ:	��
//			
//��ע:����ͨ��
//-----------------------------------------------
void ClrReportAllChannelIndex(  BYTE inEventIndex )
{
	BYTE i,Ch;
	BYTE tSubEventIndex;

	if(inEventIndex >= eNUM_OF_EVENT_PRG)//�˴�����ӦΪ�¼��������¼�������
	{
		return;
	}
	for( Ch = 0 ;Ch < MAX_COM_CHANNEL_NUM ; Ch++ )
	{
//		if( Ch == eIR )
//		{
//			continue;
//		}
		//����EventIndex��Ӧ��SubEventIndex
		for( i = 0 ; i < SUB_EVENT_INDEX_MAX_NUM ; i++ )
		{
			if( inEventIndex == SubEventInfoTab[i].EventIndex )
			{
				tSubEventIndex = SubEventInfoTab[i].SubEventIndex;
				//�嵱ǰͨ����ָ�������¼��б��е�ָ���¼���
				ClrReportIndexChannel( Ch, tSubEventIndex );
			}

		}
	}		
}

//-----------------------------------------------
//��������: ���ϱ�����
//
//����: 	��
//  	                	
//����ֵ:	��
//			
//��ע:������ͨ���������¼��б�ָ��
//-----------------------------------------------
void ClearReport( BYTE Type )
{
	ClearReportDataRam( 0xff );
	ClearReportTimes( 0xff );//�����ϱ�����
	
	//���645���ϱ�����
	#if( SEL_DLT645_2007 == YES )
	ClearReport645Times( 0, 0x55, 0 ); 
	#endif

    if( Type == eEXCEPT_CLEAR_METER )//���������� �����µĵ�������ϱ�
    {
        #if( SEL_PRG_INFO_CLEAR_METER == YES ) //���ܱ������¼�
        SetReportIndex( eSUB_EVENT_PRG_CLEAR_METER, EVENT_START+EVENT_END );
        #endif
    }
    else if( Type == eEXCEPT_CLEAR_METER_CLEAR_EVENT )
    {
        #if( SEL_PRG_INFO_CLEAR_EVENT == YES )//���ܱ��¼������¼�
        SetReportIndex( eSUB_EVENT_PRG_CLEAR_EVENT, EVENT_START+EVENT_END );
        #endif
    }
}

//-----------------------------------------------
//��������: �����¼������ż����ӦOAD
//
//����: 	
//		Index[in]:	�¼�������
//		Buf[out]��	������ĵ���OAD 
//           	
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------
static void CalReportOad(BYTE Index, BYTE *Buf)
{
	BYTE Phase;
	TFourByteUnion tdw;
	
	if(Index >= SUB_EVENT_INDEX_MAX_NUM)
	{
		return;
	}
	
	Phase = SubEventInfoTab[Index].Phase;
	tdw.w[1] = EventInfoTab[ SubEventInfoTab[Index].EventIndex].OI;
	FindThisEventReallyOI(&tdw.w[1]);
	if( (tdw.w[1]>0x3008) && (tdw.w[1]!=0x300b) && (tdw.w[1]!=0x303B) )
	{
		//����7
		tdw.b[1] = 0x02;
	}
	else
	{
		//����24
		tdw.b[1] = Phase+0x06;
	}
	
	tdw.b[0] = 0x00;
	
	lib_ExchangeData(Buf,tdw.b,sizeof(DWORD));
}

//-----------------------------------------------
//��������: ��ȡ�����ϱ�ͨ����־
//
//����: 	   Ch[in]:		ͨ����
//           	
//����ֵ:	   TRUE-��ͨ��֧���ϱ�	FALSE--��ͨ����֧���ϱ�
//			
//��ע:
//-----------------------------------------------
BOOL api_GetReportChannelStatus( BYTE Ch )
{
	BYTE i;
	
	//�ж�ͨ��Ch�Ƿ�����4300�����豸������10. �ϱ�ͨ��array OAD
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		//0x000209F2,	//eCR 
		if( ((ReportMode.ReportChannelOAD[i]<<8) == 0x0209f200) &&( (ChannelOAD[Ch]<<8) == 0x0209f200) )
		{
			break;
		}	
		
		if( ReportMode.ReportChannelOAD[i] == ChannelOAD[Ch] )
		{
			break;
		}
	}

	if( i >= MAX_COM_CHANNEL_NUM )
	{
		return FALSE;		
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: �����ϱ��б�
//
//����: 	
//           	
//����ֵ:	
//			
//��ע:
//-----------------------------------------------
void api_BackupReportIndex( void )
{
	BYTE i,j,k,Offset,n;
	eEVENT_INDEX EventIndex;

	n = 0;
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		
		if( ReportData[eCR].Index[i] != 0xff )
		{
			EventIndex = SubEventInfoTab[ReportData[eCR].Index[i]].EventIndex;
			j = ( EventIndex % eNUM_OF_EVENT_PRG ) / 8;
			k = ( EventIndex % 8 );
			
			if( (ReportMode.byReportMethod[j] & (0x01<<k)) == 0 )//Ϊ�����ϱ�
			{
				ReportDataBak[eCR].Index[n] = ReportData[eCR].Index[i];
				n++;
			}	

		}
		else//SetReportIndex ���Ѿ���֤����Ч0xff�������ڽ�β ,��������һ������0xff��Ӧ���˳�ѭ��
		{
			break;
		}
	}
	//����crc
	ReportDataBak[eCR].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[eCR],sizeof(TReportData)-sizeof(DWORD));

}


//-----------------------------------------------
//��������: ��ȡ�����ϱ�OAD
//
//����: 	
//		Type[in]:	0--�����ϱ�����	1--EVENTOUTʹ��
//		Ch[in]:		ͨ����
//		Buf[out]��	������ĵ���OAD 
//           	
//����ֵ:	TRUE--�и����ϱ�	FALSE--�޸����ϱ�
//			
//��ע:
//-----------------------------------------------
BOOL GetFirstReportOad(BYTE Type, BYTE Ch, BYTE *Buf)
{
	BYTE i,j,k;
	eEVENT_INDEX EVENT_INDEX;
	
	ReportDataCheck(Ch);//�ϱ�����У��
	ReportParaCheck();//�ϱ�����У��
	
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		if( ReportData[Ch].Index[i] != 0xff )
		{
			EVENT_INDEX = SubEventInfoTab[ReportData[Ch].Index[i]].EventIndex;
			j = ( EVENT_INDEX % eNUM_OF_EVENT_PRG ) / 8;
			k = ( EVENT_INDEX % 8 );

			if( ReportMode.byReportMethod[j] & (0x01<<k) )//��1�����ϱ�
			{
				if(Type == 0)
				{
					CalReportOad(ReportData[Ch].Index[i], Buf);
				}
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
//-----------------------------------------------
//��������: ��ȡ�����ϱ�״̬�֡�ģʽ��
//
//����:		BYTE Ch[in]  ͨ��ѡ��
//			BYTE Mode[in]��	   ģʽ 0�� �����ȡ���״̬��           1����������ģʽ�� 2:���������жϸ����ϱ���ʶ-���ڸ����ϱ�
//          BYTE *pBuf[in]��    ����buf      
//         
//����ֵ:    ���ر������� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//			
//��ע: 
//-----------------------------------------------
WORD api_ReadFollowReportStatus_Mode( BYTE Ch, BYTE Mode,BYTE *pBuf )
{
	if( Ch == 0x55 )//Ϊ�����ϱ�׼�� �ڲ���ȡ�ز�ͨ��
	{
		Ch = eCR;
	}
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return 0x8000;
    }
    
	if( Mode == 0 )
	{
		ReportDataCheck( Ch );
		memcpy(pBuf, (BYTE *)&(ReportData[Ch].Status), sizeof(ReportData[Ch].Status));
		//�����ϱ�����
		memcpy( ReportDataBak[Ch].Status, ReportData[Ch].Status, sizeof(ReportData[Ch].Status) );
		ReportDataBak[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[Ch],sizeof(TReportData)-sizeof(DWORD));
        return 4;
	}
	else if( Mode == 1 )
	{
		memcpy(pBuf, (BYTE *)&(ReportMode.byFollowReportMode[0]), 4 );//20150400 ���ܱ�����ϱ�ģʽ��
        return 4;
	}
	else
	{
        return 0x8000;
	}
}

//-----------------------------------------------
//��������: ��ȡ�����¼��б�OAD
//
//����: 	
//		GetReportListType[in]	0:ȫ��		1�������ϱ�
//		Ch[in]:					ͨ����
//		Len[in]:				�������ݳ���
//		Buf[out]��				�������ݵĻ���
//           	
//����ֵ:	�������ݳ��� 0x8000--����Buf���Ȳ���
//			
//��ע:�����¼��б� ����2
//-----------------------------------------------
WORD GetReportOadList( eGetReportListType GetReportListType, BYTE Ch, WORD Len, BYTE *Buf)
{
	BYTE i,j,k;
	BYTE Offset;
	eEVENT_INDEX EventIndex;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return 0x8000;
    }
    
	ReportDataCheck(Ch);//�ϱ�����У��
	ReportParaCheck();//�ϱ�����У��
	
	Offset = 0;
	
	for(i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++)
	{
		if( (Offset+sizeof(DWORD)) > Len )
		{
			return 0x8000;
		}
		
		if( ReportData[Ch].Index[i] != 0xff )
		{
			if( GetReportListType == eGetReportActiveList )
			{
				EventIndex = SubEventInfoTab[ReportData[Ch].Index[i]].EventIndex;
				j = ( EventIndex % eNUM_OF_EVENT_PRG ) / 8;
				k = ( EventIndex % 8 );
				
				if( (ReportMode.byReportMethod[j] & (0x01<<k)) == 0 )//Ϊ�����ϱ�
				{
					CalReportOad(ReportData[Ch].Index[i], Buf+Offset);
					Offset += sizeof(DWORD);
				}	
			}
			else
			{
				CalReportOad(ReportData[Ch].Index[i], Buf+Offset);
				Offset += sizeof(DWORD);
			}
		}
		else//  SetReportIndex ���Ѿ���֤����Ч0xff�������ڽ�β ,��������һ������0xff��Ӧ���˳�ѭ��
		{
			break;
		}
	}
	
	return Offset;
}

//-----------------------------------------------
//��������: ��ȡ���ϱ��¼������б� OI
//
//����: 	
//		Len[in]:	�������ݳ���
//		Buf[out]��	array OI 
//           	
//����ֵ:	�������ݳ��� 0x8000--����Buf���Ȳ���
//			
//��ע:�����¼��б�3320 ����3 ���ϱ��¼������б�
//-----------------------------------------------
WORD GetReportOIList(WORD Len, BYTE *Buf)
{
	BYTE i;
	WORD Offset;
	TTwoByteUnion tw;
	
	Offset = 0;
	
	for(i=0; i<eNUM_OF_EVENT_PRG; i++)
	{
		if(ReportMode.byReportMode[i] & 0x03)
		{
			if( (Offset+sizeof(DWORD)) > Len )
			{
				return 0x8000;
			}
			
			tw.w = EventInfoTab[i].OI;
			FindThisEventReallyOI(&tw.w);
			lib_ExchangeData(Buf+Offset, tw.b, sizeof(WORD));
			Offset += sizeof(WORD);
		}
	}
	
	return Offset;
}

//-----------------------------------------------
//��������: �����ϱ�ͨ��
//
//����: 	
//		byIndex[in]:	ͨ����������ӦReportChannelOAD[0]�����±꣬0--�����ϱ�ͨ�� 1--��1���ϱ�ͨ��
//		Num[in]:		���ü����ϱ�ͨ��,���byIndex��Ϊ0����ֻ������1���ϱ�ͨ��
//		pBuf[in]:		array OAD
//       	
//����ֵ:	TRUE/FALSE
//			
//��ע:4300�����豸������10 �ϱ�ͨ��array OAD
//-----------------------------------------------
BOOL SetReportChannel( BYTE byIndex, BYTE Num, BYTE *pBuf )
{
	BYTE i, Flag, bySN;
	
	if( Num > MAX_COM_CHANNEL_NUM )
	{
		return FALSE;
	}
		
	Flag = 0;
	bySN = 0;
	if( byIndex != 0 )
	{
		bySN = byIndex-1;	
		Num = 1;
	}	
	if( memcmp( (void *)pBuf, (void*)&(ReportMode.ReportChannelOAD[bySN]), (Num*4) ) == 0 )//������ͬ
	{
		if( byIndex == 0 )//�����һ�����������еļ����ϱ�ͨ��
		{
			if( Num != MAX_COM_CHANNEL_NUM )
			{
				for( i=Num;i<MAX_COM_CHANNEL_NUM; i++ )
				{
					if( ReportMode.ReportChannelOAD[i] != 0 )
					{
						ReportMode.ReportChannelOAD[i] = 0;//������ϱ�ͨ��OAD��0
						Flag = 1;
					}	
				} 
			}
		}	
	}
	else
	{
		if( byIndex == 0 )
		{
			//4300	�����豸 ������10	�ϱ�ͨ��	array OAD	
			memset( (void *)&(ReportMode.ReportChannelOAD[0]), 0x00, (sizeof(DWORD)*MAX_COM_CHANNEL_NUM) );		
		}
		Flag = 1;
		memcpy( (void *)&(ReportMode.ReportChannelOAD[bySN]), (void *)pBuf, (Num*4) );
	}		
	if( Flag != 0 )
	{
		return api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode);
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��ȡ�ϱ�ͨ��
//
//����: 		
//			pBuf[in]:		array OAD
//       	
//����ֵ:	���ݳ���
//			
//��ע:4300�����豸������10 �ϱ�ͨ��array OAD
//-----------------------------------------------
WORD ReadReportChannel( BYTE *pBuf )
{
	BYTE i;
	
	for( i=0;i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( ReportMode.ReportChannelOAD[i] == 0 )
		{
			break;					
		}	
	}
	if( i == 0 )		 
	{
		return FALSE;
	}
	memcpy( (void *)pBuf, (BYTE * )&(ReportMode.ReportChannelOAD[0]),(sizeof(DWORD)*i) );

	return (sizeof(DWORD)*i);
}

//-----------------------------------------------
//��������: ���������ϱ���־
//
//����: 
//  		bMeterReportFlag[in]:	TRUE/FALSE
//			Type					eFollowReport/eActiveReport
//����ֵ:	TRUE/FALSE
//			
//��ע:4300�����豸 ����7.8 ��������ϱ�,���������ϱ�
//-----------------------------------------------
BOOL SetReportFlag( BYTE bMeterReportFlag,eReportType Type)
{
	if( Type == eFollowReport )
	{
		if( bMeterReportFlag == ReportMode.bMeterReportFlag )
		{
			return TRUE;
		}	
				
		ReportMode.bMeterReportFlag = bMeterReportFlag;
		
		if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode) == FALSE )
		{
			return FALSE;
		}	
		
		return TRUE;
	}
	else if( Type == eActiveReport )
	{
		if( bMeterReportFlag == ReportMode.bMeterActiveReportFlag )
		{
			return TRUE;
		}	
				
		ReportMode.bMeterActiveReportFlag = bMeterReportFlag;
		
		if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode) == FALSE )
		{
			return FALSE;
		}	
		
		return TRUE;		
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
//��������: ��ȡ�����ϱ���־
//
//����: 	Type		eFollowReport/eActiveReport/eFollowStatusReportMethod
//
//����ֵ:	TRUE--�����ϱ�	FALSE--�������ϱ�
//		eFollowStatusReportMethod   0�������ϱ�  	1�������ϱ�	
//��ע:4300�����豸 ����7.8 ��������ϱ�,���������ϱ�
//-----------------------------------------------
BOOL ReadReportFlag( eReportType Type )
{
	BYTE Result = FALSE;
	if( Type == eFollowReport )
	{
		Result = ReportMode.bMeterReportFlag;
	}
	else if( Type == eActiveReport )
	{
		Result = ReportMode.bMeterActiveReportFlag;
	}
	else if( Type == eFollowStatusReportMethod)
	{
		Result = ReportMode.byReportStatusMethod;
	}
	
	return Result;	
}
//-----------------------------------------------
//��������: �����¼��ϱ���֡
//
//����:	inSubEventIndex[in]: eSUB_EVENT_LOST_POWER�������¼���
//							����(�����¼�����)
//
//		RecordNo[in]: 		��¼��ţ�д��buf�У����緢���ϱ���
//							
//����ֵ:  
//			
//��ע: 
//-----------------------------------------------
void api_LostPowerReportFraming( BYTE inSubEventIndex ,DWORD RecordNo )
{
	WORD Result,i,Num;
	BYTE Buf[MAX_PRO_BUF+30],OADBuf[200];
	TTwoByteUnion Len;
	TDLT698RecordPara DLT698RecordPara;
	
	//�ǵ����¼��˳�
	if(inSubEventIndex != eSUB_EVENT_LOST_POWER )
	{
		return ;
	}
	if( MeterVoltageConst != METER_220V )//���������ϱ�ֻ��ֱͨ��
	{
		return ;
	}
	//�ж��Ƿ��Ѿ���λ
	memset(LostPowerReportBuf,0x00,sizeof(LostPowerReportBuf));
	
	ReportParaCheck( );
	
	if( ReportMode.bMeterActiveReportFlag == FALSE )//�����ϱ��ܿ���δ�������ϱ�
	{
		return ;
	}
	
	if( api_GetReportChannelStatus(eCR) == FALSE )//�ϱ�ͨ�����ز�ͨ��
	{
		return ;
	}

	//�����¼����ϱ���ʽΪ�����ϱ�
	if((ReportMode.byReportMethod[eEVENT_LOST_POWER_NO/8]&(BYTE)(0x01<<(eEVENT_LOST_POWER_NO%8))) != 0x00)
	{
		return ;
	}

	memset( Buf, 0x00, sizeof( Buf ) );//����Ƚ�buf

	Buf[0] = 0x88;//136-�ϱ�
	Buf[1] = 0x02;
	Buf[2] = 0x00;
	Buf[3] = 0x01;
	Buf[4] = 0x30;
	Buf[5] = 0x11;
	Buf[6] = 0x02;
	Buf[7] = 0x01;

	//��ȡ�����������Ա�
	Len.w = api_ReadEventAttribute( 0x3011,0,sizeof(OADBuf), OADBuf);
	if( (Len.w & 0x8000) ||(Len.w < 4))//���ش�����߳���С��1��oad����
	{
		Num =0;
	}
	else
	{
		Num = Len.w/4;
	}

	if( Num > MAX_EVENT_OAD_NUM )//���м���ֵ���ж�
	{
		return ;
	}

	Buf[8] = Num+5;
	Len.w =9;
	
	for( i = 0; i < 5; i++ )//��ӹ̶���
	{
		Buf[Len.w] = 0x00;
		memcpy( &Buf[Len.w+1] ,&FixedColumnOadTab[i], 4 );
		Len.w += 5;
	}

	for( i=0; i < Num; i++ )//��ӹ�����������
	{
		Buf[Len.w] = 0x00;
		memcpy( &Buf[Len.w+1] ,&OADBuf[4*i], 4 );
		Len.w += 5;
	}

	Buf[Len.w] = 0x01;//data
	Buf[Len.w+1] = 0x01;//1����¼
	Len.w += 2;

	DLT698RecordPara.OI = 0x3011;                   //����OI
	DLT698RecordPara.Ch = 0x55;                        //�����ϱ� �̶�ͨ������
	DLT698RecordPara.pOAD = OADBuf;                 //pOADָ��
	DLT698RecordPara.OADNum = 0;               		//OAD����
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //ѡ�����
	DLT698RecordPara.TimeOrLast = 1;           		//ȡ��1�ε����¼
	DLT698RecordPara.Phase = ePHASE_ALL;

	Result = ReadEventRecordByNo( 1, &DLT698RecordPara, MAX_PRO_BUF-Len.w, Buf+Len.w );
	if( (Result == 0x8000) || (Result == 0) )
	{
		return;
	}
	Len.w += Result;

	Buf[Len.w] = 0x00;//�����ϱ�
	Buf[Len.w+1] = 0x00;//ʱ���ǩ
	Len.w += 2;
	if( Len.w > 450 )//����ֵ�ж�67*5+10+2+14
	{
		return ;
	}
	memcpy(LostPowerReportBuf,(BYTE*)&RecordNo,0x04);//�����ȱ��浽ǰ��λ
	memcpy(LostPowerReportBuf+4,(BYTE*)&Len,0x02);//�����ȱ��浽ǰ��λ
	memcpy(LostPowerReportBuf+6,(BYTE*)&Buf,Len.w);//�����ȱ��浽ǰ��λ
	lib_CheckSafeMemVerify( LostPowerReportBuf, sizeof(LostPowerReportBuf),0x01);//����У��

}

//-----------------------------------------------
//��������: ���������ϱ���֡
//
//����:	 
//         
//����ֵ:   
//			
//��ע: 
//	�ϱ�����		�¼�δ����	�¼��ѷ���
//	�ϱ�����=0		���ϱ�			��֡�ϱ�
//	�ϱ�����!=0		�����ϱ�		���ϱ��������ϱ�
//-----------------------------------------------
void LostPowerStartReport( void )
{
	TFourByteUnion OAD;
	WORD Result,i,Num;
	BYTE Temp,Buf[MAX_PRO_BUF+30],OADBuf[200];
	TTwoByteUnion Len;
	TDLT698RecordPara DLT698RecordPara;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	DWORD dwOad,RecordNo;
	TRealTimer LostPowerTime;
	
	ReportParaCheck( );
	if( MeterVoltageConst != METER_220V )//���������ϱ�ֻ��ֱͨ��
	{
		return ;
	}
	
	if( ReportMode.bMeterActiveReportFlag == FALSE )//�����ϱ��ܿ���δ�������ϱ�
	{
		return ;
	}
	
	if( api_GetReportChannelStatus(eCR) == FALSE )//�ϱ�ͨ�����ز�ͨ��
	{
		return ;
	}
	
	if((ReportMode.byReportMode[eEVENT_LOST_POWER_NO] & 0x01) != 0x01)//�����¼������ϱ�
	{
		return ;
	}
	
	if((ReportMode.byReportMethod[eEVENT_LOST_POWER_NO/8]&(BYTE)(0x01<<(eEVENT_LOST_POWER_NO%8))) != 0x00)//�����¼����ϱ���ʽΪ�����ϱ�
	{
		return ;
	}
	
	//�ϱ��Ѿ������Ҵ���Ϊ�㣬�����Ѿ��ϱ���ɣ�����Ҫ�ϱ����˳�����
	if( ( api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 1 )
	&& ( LostPowerReportTimes.Times == 0 ) )
	{
		return ;
	}

	//�ϱ�û�з����Ҵ�����Ϊ�㣬�����Ѿ��¼������ϱ�δ��ɣ����ϱ����˳�����
	if( ( api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 0 )
	&& ( LostPowerReportTimes.Times != 0 ) )
	{
		ClearReportTimes( eActiveReportPowerDown );//���ϱ�����
		return ;
	}
	
	//�ж��Ƿ����1Сʱ�ϱ�
	if(EventLostPowerEndTime < LostPowerEndTimeThreshold)
	{
		ClearReportTimes( eActiveReportPowerDown );//���ϱ�����
		return ;
	}
	//�¼�״̬λΪδ������������֡�ϱ�
	if( ( api_DealEventStatus( eGET_EVENT_STATUS , eSUB_EVENT_LOST_POWER ) == 0 )
	|| (lib_CheckSafeMemVerify( LostPowerReportBuf, sizeof(LostPowerReportBuf),0x00) != TRUE ) )
	{
		memset( Buf, 0x00, sizeof( Buf ) );//����Ƚ�buf
		memset( LostPowerReportBuf , 0x00 , sizeof( LostPowerReportBuf ) );
		
		Buf[0] = 0x88;//136-�ϱ�
		Buf[1] = 0x02;
		Buf[2] = 0x00;
		Buf[3] = 0x01;
		Buf[4] = 0x30;
		Buf[5] = 0x11;
		Buf[6] = 0x02;
		Buf[7] = 0x01;

		//��ȡ�����������Ա�
		Len.w = api_ReadEventAttribute( 0x3011 , 0 , sizeof( OADBuf ), OADBuf );
		if( ( Len.w & 0x8000 ) ||( Len.w < 4))//���ش�����߳���С��1��oad����
		{
			Num =0;
		}
		else
		{
			Num = Len.w / 4;
		}
		
		if( (Num + 4) > MAX_EVENT_OAD_NUM )//���м���ֵ���ж�
		{
			ClearReportTimes( eActiveReportPowerDown);
			return;
		}
		
		Buf[8] = Num + 5;
		Len.w = 9;

		for( i = 0; i < 5; i++ )//��ӹ̶���
		{
			Buf[Len.w] = 0x00;
			memcpy( &Buf[Len.w+1] ,&FixedColumnOadTab[i], 4 );
			Len.w += 5;
		}
		
		for( i=0; i < Num; i++ )//��ӹ�����������
		{
			Buf[Len.w] = 0x00;
			memcpy( &Buf[Len.w+1] ,&OADBuf[4*i], 4 );
			Len.w += 5;
		}
		Buf[Len.w] = 0x01;//data
		Buf[Len.w+1] = 0x01;//1����¼
		Len.w += 2;
		EventAddrLen.EventIndex = eEVENT_LOST_POWER_NO ;
		EventAddrLen.Phase = ePHASE_ALL;
		if( GetEventInfo( &EventAddrLen ) == FALSE )
		{
			ClearReportTimes( eActiveReportPowerDown);
			return;
		}
		
		// ��ǰ��¼��
		api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		RecordNo = EventDataInfo.RecordNo;
		//�¼��Ѿ����������--
		if( api_DealEventStatus( eGET_EVENT_STATUS , eSUB_EVENT_LOST_POWER ) == 1 )
		{
			RecordNo--;
		}
		
		Buf[Len.w] = 0x06;//Double_long_unsigned
		lib_ExchangeData((BYTE*)&Buf[Len.w+1] ,(BYTE*)&RecordNo, 4 );

		//���ʱ��
		Len.w += 5;
		Buf[Len.w] = 0x1C;//Date_Time_S
		// ��E2�д�Ľ������ʱ����Ϊ�¼�����ʱ�䣬���ϵ�󱣴�һ��
		api_GetPowerDownTime( (TRealTimer*)&LostPowerTime );
		memcpy( Buf+Len.w+1 ,&LostPowerTime, 7 );
		lib_ExchangeData((BYTE*)&Buf[Len.w+1] ,(BYTE*)&Buf[Len.w+1], 2 );
		Len.w += 8;
		Buf[Len.w] = 0x00;
		Buf[Len.w+1] = 0x00;
		Buf[Len.w+2] = 0x00;
		Len.w += 3;
		//����¼�����������������
		for( i = 0; i < Num; i++ )
		{
			memcpy( (BYTE*)&dwOad ,&OADBuf[4*i], 4 );
			Temp = (BYTE)((dwOad&0x00E00000)>>21);
			dwOad &= ~0x00E00000;

			//OAD��������bit5~7:1���¼�����ǰ 2���¼������� 3���¼�����ǰ 4���¼�������
			if( (Temp != 3) && (Temp != 4) )//�������������Ϊ3,4��Ĭ�ϰ���1,2,���д�������������
			{
				Result = api_GetProOadData( eGetNormalData, eTagData, 0xff, (BYTE *)&dwOad, NULL, 0xFF, Buf+Len.w );
				if( (Result == 0x8000) || (Result == 0) )
				{
					ClearReportTimes( eActiveReportPowerDown);
					return;
				}
				Len.w += Result;
			}
			else
			{
				Buf[Len.w] = 0x00;
				Len.w += 1;
			}	
		}
		Buf[Len.w] = 0x00;//�����ϱ�
		Buf[Len.w+1] = 0x00;//ʱ���ǩ
		Len.w += 2;
		if( Len.w > 450 )//����ֵ�ж�67*5+10+2+14
		{
			ClearReportTimes( eActiveReportPowerDown);
			return;
		}
		
	
		memcpy(LostPowerReportBuf,(BYTE*)&RecordNo,0x04);//��RecordNo���浽ǰ��λ
		memcpy(LostPowerReportBuf+4,(BYTE*)&Len,0x02);//�����ȱ��浽ǰ��λ
		memcpy(LostPowerReportBuf+6,(BYTE*)&Buf,Len.w);//��buf����
		lib_CheckSafeMemVerify( LostPowerReportBuf, sizeof(LostPowerReportBuf),0x01);//����У��
		ResetReportTimes( eActiveReportPowerDown);//���ϱ�����
	}
	else
	{
		EventAddrLen.EventIndex = eEVENT_LOST_POWER_NO ;
		EventAddrLen.Phase = ePHASE_ALL;
		if( GetEventInfo( &EventAddrLen ) == FALSE )
		{
			ClearReportTimes( eActiveReportPowerDown);
			return;
		}
	
		// ��ǰ��¼��
		api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
		//����ǰ�ж�buf�Բ���
		memcpy((BYTE*)&RecordNo,(BYTE*)&LostPowerReportBuf,0x04);//����¼��Ŷ����ж�һ��
		//����������뻺���ڵ�buf��ͬ���¶�flash��֡
		if(RecordNo != EventDataInfo.RecordNo)
		{
			ClearReportTimes( eActiveReportPowerDown);//���ϱ�����
			return;
		}
		
		
		//�ϱ���������3�������ϱ�Ϊ3��
		if(LostPowerReportTimes.Times > 3)
		{
			ResetReportTimes( eActiveReportPowerDown);//���ϱ�����
		}
		
	}
}
//-----------------------------------------------
//��������: ���������ϱ�����
//
//����:	 
//         
//����ֵ:   
//			
//��ע: �͹��Ĵ�ѭ������
//-----------------------------------------------
void ProcLowPowerMeterReport( void)
{
	WORD i;
	TTwoByteUnion Len;
	if( MeterVoltageConst != METER_220V )//���������ϱ�ֻ��ֱͨ��
	{
		return ;
	}
	if(LostPowerReportTimes.Times == 0)
	{
		return ;
	}
	//�ϱ��������� �����ϱ�����
	if( LostPowerReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD) ))
	{
		ClearReportTimes( eActiveReportPowerDown );
		return ;
	}
	if((LostPowerReportTimes.Times > 0)&&(LostPowerReportTimes.Times <= 3))
	{
		
		api_ReportCRWakeUpConfig(0x00);
		if(lib_CheckSafeMemVerify( LostPowerReportBuf, sizeof(LostPowerReportBuf),0x00)== TRUE)
		{
			memcpy((BYTE*)&Len,LostPowerReportBuf+4 ,2);
			api_ActiveReportResponse( LostPowerReportBuf+6, Len.w , 0x55);
			SerialMap[eCR].SCIBeginSend(SerialMap[eCR].SCI_Ph_Num);
			CLEAR_WATCH_DOG;//���忴�Ź��ٷ�
			for(i=0;i<500;i++)//�������﷢��
			{
				if( Serial[eCR].TXPoint >= Serial[eCR].SendLength )
				{
					break;
				}
				api_Delayms(2);

			}
			api_Delayms(5);//2400�Ĳ�������һ���ֽ����۷���ʱ��Ϊ4.1ms,�˴���ʱ5ms
			ReportTimesReduce1(eActiveReportPowerDown);
			if(LostPowerReportTimes.Times == 0)//�ϱ������Buf
			{
				api_EnterLowPower( eFromDetectEnterDownMode );//���µ��ý���͹��ĺ���
			}
		}
		else
		{
			ClearReportTimes( eActiveReportPowerDown );
		}
		api_ReportCRWakeUpConfig(0x55);
	}
	else if(LostPowerReportTimes.Times > 3)
	{
		ClearReportTimes( eActiveReportPowerDown );
	}
	
}

//-----------------------------------------------
//��������: �����¼��ϱ���ʶ
//
//����: 	
//			eReportClass ReportClass
//			OI[in]:				�¼�OI
//			byReportMode[in]:	eReportMode 	�¼��ϱ���ʶ{���ϱ�(0),�¼������ϱ�(1),�¼��ָ��ϱ�(2),�¼������ָ����ϱ�(3)}
//								eReportMethod 	0:�����ϱ� 1�������ϱ�
//
//����ֵ:	TRUE/FALSE
//			
//��ע:�ӿ���7������8���ӿ���24������11
//-----------------------------------------------
BOOL api_SetEventReportMode( eReportClass ReportClass, WORD OI, BYTE byReportMode )
{
	BYTE i,j,k;

	if( ReportClass > eReportMethod )
	{
		return FALSE;
	}
	
	if( OI == 0x2015 )//�����ϱ���ʽ
	{
		if( ReportClass != eReportMethod )
		{
			return FALSE;
		}
		
		if( byReportMode > 1 )
		{
			return FALSE;
		}
		
		if( byReportMode == ReportMode.byReportStatusMethod )//����ֵ����ֵһ��
		{
			return TRUE;
		}
		
		ReportMode.byReportStatusMethod = byReportMode;
	}
	else
	{
		if( GetEventIndex( OI, &i ) == FALSE )
		{
			return FALSE;
		}
		
		if( ReportClass == eReportMode )//�ϱ�ģʽ
		{
			if( byReportMode > 3 )
			{
				return FALSE;
			}
			
			if( byReportMode == ReportMode.byReportMode[i] )//���Ҫ����ֵ��RAM��һ����ʵ��Ҳ��EEPROM��һ��
			{
				return TRUE;	 
			}

			ReportMode.byReportMode[i] = byReportMode;
		}
		else//�ϱ�����
		{
			if( byReportMode > 1 )
			{
				return FALSE;
			}
			
			j = (i%eNUM_OF_EVENT_PRG) / 8;
			k = i % 8;

			if( byReportMode == 1 )
			{
				if( ReportMode.byReportMethod[j] & (0x01<<k) )//��λ�Ѿ���1������Ƶ��дEEPROM
				{
					return TRUE;
				}	
				
				ReportMode.byReportMethod[j] |= (0x01<<k);

			}
			else
			{
				if( (ReportMode.byReportMethod[j] & (0x01<<k)) == 0 )//��λ�Ѿ����㣬����Ƶ��дEEPROM
				{
					return TRUE;
				}	
				ReportMode.byReportMethod[j] &= ~(0x01<<k);
			}	
		}
	}
		
	return api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode);
	
}

//-----------------------------------------------
//��������: ��ȡ�¼��ϱ���ʶ
//
//����: 	
//			eReportClass ReportClass
//			OI[in]:				�¼�OI
//			byReportMode[in]:	eReportMode 	�¼��ϱ���ʶ{���ϱ�(0),�¼������ϱ�(1),�¼��ָ��ϱ�(2),�¼������ָ����ϱ�(3)}
//								eReportMethod 	0:�����ϱ� 1�������ϱ�
//
//����ֵ:	TRUE/FALSE
//			
//��ע:�ӿ���7������8���ӿ���24������11
//-----------------------------------------------
BOOL api_ReadEventReportMode( eReportClass ReportClass, WORD OI, BYTE *pBuf )
{
	BYTE i,j,k;

	if( ReportClass > eReportMethod )
	{
		return FALSE;
	}
	
	if( OI == 0x2015 )//�����ϱ�
	{
		if( ReportClass != eReportMethod )
		{
			return FALSE;
		}
		
		pBuf[0] = ReportMode.byReportStatusMethod;
	}
	else
	{
		if( GetEventIndex( OI, &i ) == FALSE )
		{
			return FALSE;
		}

		if( ReportClass == eReportMode )//�ϱ�ģʽ
		{
			pBuf[0] = ReportMode.byReportMode[i];
		}
		else
		{
			j = (i%eNUM_OF_EVENT_PRG) / 8;
			k = i % 8;
			
			if( ReportMode.byReportMethod[j] & (0x01<<k) )
			{
				pBuf[0] = 1;
			}
			else
			{
				pBuf[0] = 0;
			}
			
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: �����ϱ��ж�
//
//����:	   ��
//         
//����ֵ:     TRUE�����������ϱ�         FLASE:�������ϱ�
//			
//��ע: 
//-----------------------------------------------
WORD api_JudgeActiveReport( void )
{
	TReportData tmpReportData;
	TFourByteUnion OAD;
	WORD StatusResult,IndexResult,Result,i;
	BYTE Buf[MAX_PRO_BUF+30],OADBuf[200];
	TTwoByteUnion Len;
	TEventDataInfo	EventDataInfo;
	TEventAddrLen	EventAddrLen;
	DWORD RecordNo;

	StatusResult = 0;
	IndexResult = 0;
	
	if( ReportMode.bMeterActiveReportFlag == FALSE )//�����ϱ��ܿ���δ�������ϱ�
	{
		return FALSE;
	}
	
	if( api_GetReportChannelStatus(eCR) == FALSE )
	{
		return FALSE;
	}
	
	if( ((ReportTimes.Times != 0) && (ReportOverTime == 0))
	|| ((LostPowerReportTimes.Times != 0) && (ReportPowerDownTime == 0)) )//���ϱ���Ҫ�ϱ�
	{
		
		ReportDataCheck( eCR );
		ReportParaCheck( );

		if( ReportMode.byReportStatusMethod == 0 )//�ϱ�״̬���ϱ���ʽΪ�����ϱ�
		{
			//�ϱ�״̬�ֱȽ�
			memset( (BYTE*)&tmpReportData, 0x00, sizeof( tmpReportData ) );//����Ƚ�buf
			StatusResult = memcmp( ReportData[eCR].Status, tmpReportData.Status, sizeof(ReportData[0].Status));
		}

		//�ϱ��¼��б�Ƚ�
		Result = GetReportOadList( eGetReportActiveList, eCR, MAX_PRO_BUF, Buf );
		if( (Result != 0) && (Result != 0x8000) )
		{
			IndexResult = 1;//�������ϱ�
		}

		if((LostPowerReportTimes.Times != 0) && (ReportPowerDownTime == 0))//�ϱ������¼
		{
			//�����¼�������3320����û��
			if(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 0)
			{
				for( i = 0; i < (Result/4); i++ )
				{
					lib_ExchangeData( OAD.b, &Buf[4*i], 4 );
					if( (OAD.d&0xffff0000) == 0x30110000 )
					{
						break;
					}
				}
				if( i == (Result/4) )//�ϱ��б����޵����¼��������ϱ�����
				{
					ClearReportTimes( eActiveReportPowerDown);
					return FALSE;
				}
			}

			EventAddrLen.EventIndex = eEVENT_LOST_POWER_NO ;
			EventAddrLen.Phase = ePHASE_ALL;
			if( GetEventInfo( &EventAddrLen ) == FALSE )
			{
				return FALSE;
			}
			
			// ��ǰ��¼��
			api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
			memcpy((BYTE*)&RecordNo,LostPowerReportBuf,0x04);//ȡ��buf�д洢��RecordNo
			
			if( ( lib_CheckSafeMemVerify( LostPowerReportBuf , sizeof( LostPowerReportBuf ), 0x00 )== TRUE )
			&& (RecordNo == EventDataInfo.RecordNo) )
			{
				memcpy((BYTE*)&Len,LostPowerReportBuf+4,0x02);//�����ȱ��浽ǰ��λ
				if( Len.w > sizeof(Buf))//��ֹ����Խ��
				{
					return FALSE;
				}
				memcpy((BYTE*)&Buf,LostPowerReportBuf+6,Len.w);//�����ݰᵽbuf��
			}
			else
			{
				ClearReportTimes( eActiveReportPowerDown);
				return FALSE;
			}
			//�����¼������ϱ���ʱΪ1S
			if(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 1)
			{
				ReportPowerDownTime = 101;//�����ϱ���ʱ+����ʱ�� 1010ms
				api_ActiveReportResponse( Buf, Len.w  , 0x55);
			}
			else
			{
				ReportPowerDownTime = 1010;//�����ϱ���ʱ+����ʱ�� 10100ms
				api_ActiveReportResponse( Buf, Len.w  , 0x00);
			}
			

			ReportTimesReduce1( eActiveReportPowerDown );//�ϱ�������1
			

		}
		else
		{
			if((StatusResult != 0) ||(IndexResult != 0) )//�и����ϱ� ����������֡
			{
			
				memset( Buf, 0x00, sizeof( Buf ) );//����Ƚ�buf
				
				Buf[0] = 136;
				Buf[1] = 0x01;
				Buf[2] = 0x00;
				if((StatusResult != 0) && (IndexResult != 0) )//A-ResultNormal����
				{
					Buf[3] = 0x02;
				}
				else
				{
					Buf[3] = 0x01;
				}
				
				Len.w =4;
	
				if( IndexResult != 0 )
				{
					Buf[Len.w] = 0x33;//�����ϱ��б�OAD
					Buf[Len.w+1] = 0x20;
					Buf[Len.w+2] = 0x02;
					Buf[Len.w+3] = 0x00;
					Buf[Len.w+4] = 0x01;
					Len.w += 5;
					Result = AddActiveReportList( MAX_PRO_BUF-Len.w, Buf+Len.w );
					if( Result == 0x8000 )
					{
						ReportTimesReduce1( eActiveReportNormal );//���ִ��� �ϱ�������1 �������һֱ����һֱ�жϵ����
						return FALSE;
					}
					Len.w += Result;
				}
	
				if( StatusResult != 0 )
				{
					Buf[Len.w] = 0x20;	//�����ϱ�״̬��OAD
					Buf[Len.w+1] = 0x15;
					Buf[Len.w+2] = 0x02;
					Buf[Len.w+3] = 0x00;
					Buf[Len.w+4] = 0x01;
					Len.w +=5;
					Result = api_GetProOadData( eGetNormalData, eTagData, 0XFF, (BYTE*)&Buf[Len.w-5], NULL, sizeof(Buf)-Len.w, Buf+Len.w);
					if( Result == 0x8000 )
					{
						ReportTimesReduce1( eActiveReportNormal );//���ִ��� �ϱ�������1 �������һֱ����һֱ�жϵ����
						return FALSE;
					}
					
					Len.w += Result;
				}
				Buf[Len.w] = 0x00;//�����ϱ�
				Buf[Len.w+1] = 0x00;//ʱ���ǩ
				Len.w += 2;
				if( Len.w > 450 )//����ֵ�ж�67*5+10+2+14
				{
					ReportTimesReduce1( eActiveReportNormal );//���ִ��� �ϱ�������1 �������һֱ����һֱ�жϵ����
					return FALSE;
				}
				
				ReportOverTime = 1010;//�����ϱ���ʱ+����ʱ�� 10100ms
				api_ActiveReportResponse( Buf, Len.w  , 0x00);
	
				ReportTimesReduce1( eActiveReportNormal );//�ϱ�������1
			}
			else
			{
				ClearReportTimes(0xff);
				return FALSE;
			}
		}
		return TRUE;
	}

	return FALSE;
}
//-----------------------------------------------
//��������: ȷ�������ϱ�
//
//����:	   BYTE Type[in] 0x00: ȷ�������¼��б� 0x55��ȷ�ϸ����ϱ�״̬�� 0xAA��ȷ�ϵ����¼�
//         
//����ֵ:     TRUE�����������ϱ�         FLASE:�������ϱ�
//			
//��ע: 
//-----------------------------------------------
void api_ResponseActiveReport( BYTE Type )
{
	BYTE i,j,n,Index[SUB_EVENT_INDEX_MAX_NUM+10];

	//��������У��
	ReportDataCheck( eCR );
	
	if( Type == 0 )
	{
		for( i=0; i<SUB_EVENT_INDEX_MAX_NUM; i++ )
		{
			if( ReportDataBak[eCR].Index[i] != 0xff)
			{
				n = 0;
				memcpy( Index, ReportData[eCR].Index, sizeof(ReportData[eCR].Index) );
				memset( ReportData[eCR].Index, 0xff, sizeof(ReportData[eCR].Index));//��ն�Ӧbuf
				for( j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++ )
				{
					if( Index[j] == 0xff )//��ѯ��0xff�˳�
					{
						break;
					}
					
					if( Index[j] != ReportDataBak[eCR].Index[i] )
					{

						ReportData[eCR].Index[n] = Index[j];
						n++;
					}
				}
			}
			else
			{
				break;
			}

		}

		//�����ϱ����ݱ���
		memset((BYTE *)&(ReportDataBak[eCR].Index), 0xFF, sizeof(ReportDataBak[0].Index));
		ReportDataBak[eCR].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[eCR],sizeof(TReportData)-sizeof(DWORD));
	}
	else if( Type == 0xAA )//ȷ�ϵ���
	{
		n = 0;
		memcpy( Index, ReportData[eCR].Index, sizeof(ReportData[eCR].Index) );
		memset( ReportData[eCR].Index, 0xff, sizeof(ReportData[eCR].Index));//��ն�Ӧbuf
		for( j=0; j<SUB_EVENT_INDEX_MAX_NUM; j++ )
		{
			if( Index[j] == 0xff )//��ѯ��0xff�˳�
			{
				break;
			}
			
			if( Index[j] != eSUB_EVENT_LOST_POWER )
			{
				ReportData[eCR].Index[n] = Index[j];
				n++;
			}
		}
		ClearReportTimes( eActiveReportPowerDown);
		
	}
	else if( Type == 0x55 )
	{
		for( i=0; i<4; i++ )
		{
			//�����λ���ϱ���������Ҫ����λ���������λ
			ReportData[eCR].Status[i] &= ~ReportDataBak[eCR].Status[i];
		}

		//�����Ӧ�ĸ����ϱ�״̬�ֱ���
		memset((BYTE *)&(ReportDataBak[eCR].Status), 0x00, sizeof(ReportDataBak[eCR].Status));
		ReportDataBak[eCR].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[eCR],sizeof(TReportData)-sizeof(DWORD));
	}
	else
	{}
	
	ReportData[eCR].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[eCR],sizeof(TReportData)-sizeof(DWORD));		

}

//-----------------------------------------------
//��������: ���ø����ϱ�ģʽ��
//
//����:    
//			pBuf[in]��	�������ݵĻ���
//����ֵ:	TRUE/FALSE  
//			
//��ע: 
//-----------------------------------------------
WORD api_SetFollowReportMode( BYTE *pBuf )
{
	memcpy( (BYTE*)&(ReportMode.byFollowReportMode[0]), pBuf, 4);

	if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode) == FALSE )
	{
		return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------
//��������: �ϱ�������
//
//����: 	��
//
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------
void ReportSecTask(void)
{
	BYTE Status;
	BYTE *pByte;

	//�������ʱ��
	if(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 0)
	{
		if(EventLostPowerEndTime <= LostPowerEndTimeThreshold)
		{
			EventLostPowerEndTime++;
		}
		else
		{
			if(api_GetRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG) != TRUE)
			{
				api_SetRunHardFlag(eRUN_HARD_POWERDOWN_REPORT_FLAG);
			}	
		}
	}
	
	//�ز�ͨ����֧���ϱ�������ϱ�δ����               ������eventout�ܽŵ���λ
	if( (api_GetReportChannelStatus( eCR ) == FALSE) || (ReadReportFlag(eFollowReport) == FALSE))
	{
		CANCEL_EVENTOUT;
		return ;
	}

	//�ز�ͨ�����и����ϱ�������EVENTOUTΪ����̬
	Status = GetFirstReportOad(1, eCR, NULL);

	//û���¼��ϱ�ʱ�ж����޸����ϱ�״̬��
	if( Status == FALSE )
	{
		//�����ϱ�״̬���ϱ���ʽΪ�����ϱ�
		if(ReportMode.byReportStatusMethod == 1)
		{
			if( (ReportData[eCR].Status[0] ==0x00) && (ReportData[eCR].Status[1] ==0x00) && (ReportData[eCR].Status[2] ==0x00) && (ReportData[eCR].Status[3] ==0x00) )
			{
			}
			else
			{
				Status = TRUE;
			}
		}
	}	
	
	if(Status == TRUE)
	{
		NOTICE_EVENTOUT;
	}
	else
	{
		CANCEL_EVENTOUT;
	}
}

//-----------------------------------------------
//��������: �ϱ�Сʱ����
//
//����: 	��
//
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------
void ReportHourTask(void)
{
	//�ϱ�����У��
	ReportParaCheck();
	//�ϱ�����У��
	ReportDataCheck( 0xff );

	//�ϱ�������ʱУ��
	if( ReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD)))
	{
		ClearReportTimes(eActiveReportNormal);
	}
	//�ϱ�������ʱУ��
	if( LostPowerReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&LostPowerReportTimes,sizeof(TReportTimes)-sizeof(DWORD)))
	{
		ClearReportTimes(eActiveReportPowerDown);
	}
}

//-----------------------------------------------
//��������: �ϱ��ϵ�����
//
//����: 	��
//
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------		
void PowerUpReport(void)
{
    BYTE i,j,k;

    //ģʽ��У��
	ReportParaCheck();

	ClearReportDataRam( eIR );//��պ����ϱ�����
	
	for( i=0; i < MAX_COM_CHANNEL_NUM; i++ )
	{
		if( i== eIR )
		{
			continue;
		}

		//�ϱ�����У��
		if( ReportData[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportData[i],sizeof(TReportData)-sizeof(DWORD) ))
		{
			//��ȡʧ������
			if( api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(ReportConRom.ReportData[i]), sizeof(ReportData[0]), (BYTE *)&ReportData[i] ) == FALSE)
			{
				ClearReportDataRam( i );
			}
		}
		
		//�����ϱ����ݴ��� ������������
		if( ReportDataBak[i].CRC32 != lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD) ))
		{
			memset((BYTE *)&(ReportDataBak[i]), 0x00, sizeof(TReportData));
			memset((BYTE *)&(ReportDataBak[i].Index), 0xFF, sizeof(ReportDataBak[0].Index));
			ReportDataBak[i].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[i],sizeof(TReportData)-sizeof(DWORD));
		}
	}

	//�ϱ��������� �����ϱ�����
	if( ReportTimes.CRC32 != lib_CheckCRC32((BYTE*)&ReportTimes,sizeof(TReportTimes)-sizeof(DWORD) ))
	{	
		//��ȡʧ������
		if( api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportTimes), sizeof(ReportTimes), (BYTE *)&ReportTimes ) == FALSE )
		{
			ClearReportTimes(eActiveReportNormal);
		}
	}
	//�ϵ�������ϱ�buf
	ClearReportTimes(eActiveReportPowerDown);

    #if( SEL_DLT645_2007 == YES )
    api_PowerUpReport645();
    #endif
}   

//-----------------------------------------------
//��������: �ϱ���������
//
//����: 	��
//
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------	
void PowerDownReport(void)
{
    BYTE i;

    //�ϱ�����У��
	ReportDataCheck(0xff);
	//�����ϱ��������ϱ����ݡ���������
	for( i=0; i < MAX_COM_CHANNEL_NUM; i++ )
	{
//		if( i== eIR )
//		{
//			continue;
//		}

 		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(ReportConRom.ReportData[i]), sizeof(ReportData[0]), (BYTE *)&ReportData[i] );
	}

	api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportTimes), sizeof(ReportTimes), (BYTE *)&ReportTimes );
	//��λ�����¼������ϱ�����֡�����ϱ�����
	LostPowerStartReport();
    #if( SEL_DLT645_2007 == YES )
    api_PowerDownReport645();
    #endif
}


//-----------------------------------------------
//��������: �ϱ�����ʼ��
//
//����: 	��	
//           	
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------
void FactoryInitReport(void)
{
	BYTE i,j,k;

	memset( &ReportMode, 0x00, sizeof(ReportMode) );
	
	//Ĭ�ϲ���������ϱ� 4300	�����豸 ������7. ��������ϱ�
	ReportMode.bMeterReportFlag = MeterReportFlagConst;
	ReportMode.bMeterActiveReportFlag = MeterActiveReportFlagConst;
	//�����ϱ�״̬�� �ϱ���ʽ
	ReportMode.byReportStatusMethod = MeterReportStatusFlagConst;
	
	//��ʼ��ΪĬ�ϸ����ϱ�
	memset( &ReportMode.byReportMethod, 0xff, sizeof(ReportMode.byReportMethod) );
	//�¼������ϱ���ʽ
	for( i = 0; i< ReportActiveMethodConst[0]; i++ )
	{
		j = (ReportActiveMethodConst[i+1]%eNUM_OF_EVENT_PRG) / 8;
		k = ReportActiveMethodConst[i+1] % 8;
		ReportMode.byReportMethod[j] &= ~(0x01<<k);;
	}
	
    memcpy( (void *)&(ReportMode.byFollowReportMode[0]), FollowReportModeConst, GET_STRUCT_MEM_LEN(TReportMode, byFollowReportMode) );//20150400 ���ܱ�����ϱ�ģʽ��

	//4300	�����豸 ������10	�ϱ�ͨ��	array OAD
	memset( (void *)&(ReportMode.ReportChannelOAD[0]), 0x00, (sizeof(DWORD)*MAX_COM_CHANNEL_NUM) );
	//д�ϱ�ͨ��
	for(i=0;i<FollowReportChannelConst[0];i++)
	{
		ReportMode.ReportChannelOAD[i]=ChannelOAD[FollowReportChannelConst[i+1]];
	}
		
	memset( &ReportMode.byReportMode, 0x00, sizeof(ReportMode.byReportMode) );
	//�¼��ϱ�ʱ��
	for( i = 0; i< ReportModeConst[0].EventIndex; i++ )
	{
		ReportMode.byReportMode[ReportModeConst[i+1].EventIndex] = ReportModeConst[i+1].ReportMode;
	}
	api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportMode), sizeof(ReportMode), (BYTE *)&ReportMode);

    #if( SEL_DLT645_2007 == YES )
    api_FactoryInitReport645( );
    #endif
    
	ClearReportDataRam( 0xff );
	//�����ϱ�����
	ClearReportTimes( 0xff );
	
	//��ʼ���ϱ�����
	api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,ReportSafeRom.ReportTimes), sizeof(ReportTimes), (BYTE *)&ReportTimes );
   	//��ʼ���ϱ�����
	for( i=0 ; i < MAX_COM_CHANNEL_NUM; i++ )
	{
//		if( i== eIR )
//		{
//			continue;
//		}
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(ReportConRom.ReportData[i]), sizeof(TReportData), (BYTE *)&ReportData[i] );
	}

}


#endif//#if ( SEL_DLT698_2016_FUNC == YES)
