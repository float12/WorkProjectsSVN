//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	κ���� ������
//��������	2016.10.26
//����		698��Լ�ϱ�
//�޸ļ�¼	
//---------------------------------------------------------------------- 

#ifndef __REPORT698_API_H
#define __REPORT698_API_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

//�����ϱ�״̬���и���״̬λ������˳����698.45Э���ص���ֽ���ǰ
typedef enum
{
	eReserve0 = 0,			//����ֽڵ�bit0����Э���и����ϱ�״̬�ֵ�bit24
	eReserve1,
	eReserve2,
	eReserve3,	
	eReserve4,
	eReserve5,
	eReserve6,	
	eReserve7,
	eReserve8,			//��3���ֽڵ�bit0����Э���и����ϱ�״̬�ֵ�bit16
	eReserve9,
	eReserve10,
	eReserve11,	
	eReserve12,
	eReserve13,
	eReserve14,	
	eReserve15,	
	eSTATUS_ReadMeterBatteryUnderVol,   //��2���ֽڵ�bit0����Э���и����ϱ�״̬�ֵ�bit8
	eSTATUS_TickFlag,					//��2���ֽڵ�bit1����Э���и����ϱ�״̬�ֵ�bit9--͸֧״̬ TickFlag
	eReserve18,
	eReserve19,
	eReserve20,
	eReserve21,
	eSTATUS_OpenRelaySucceed,
	eSTATUS_CloseRelaySucceed,		
	eReserve24,							//��1���ֽڵ�bit0����Э���и����ϱ�״̬�ֵ�bit0
	eSTATUS_ESAM_Error,					//��1���ֽڵ�bit1����Э���и����ϱ�״̬�ֵ�bit0
	eReserve26,
	eSTATUS_ClockBatteryUnderVol,
	eReserve28,
	eSTATUS_EEPROM_Error,//bit5--�洢�����ϻ���
	eReserve30,
	eReserve31,
	eMAX_FOLLOW_REPORT_STATUS_NUM,
}eFollowReportStatus;

typedef enum
{
	eFollowReport = 0,
	eActiveReport,
	eFollowStatusReportMethod,
}eReportType;

typedef enum
{
	eReportMode = 0,	//�ϱ�ģʽ
	eReportMethod,		//�ϱ���ʽ ---�̶����һ��--jwh
}eReportClass;

typedef enum
{
	eGetReportAllList = 0,		//ȫ�������б�
	eGetReportActiveList,		//�����ϱ������б�
}eGetReportListType;

typedef enum
{
	eActiveReportPowerDown = 0,	//�ϱ�����
	eActiveReportNormal,		//�����ϱ�	
}eActiveReportMode;

#pragma pack(1)

typedef struct TReportMode_t
{
	//4300	�����豸 ������7. ��������ϱ�
	BOOL 	bMeterReportFlag;
	//4300	�����豸 ������8�����������ϱ�
	BOOL	bMeterActiveReportFlag;
	BYTE 	byFollowReportMode[4];//20150400 ���ܱ�����ϱ�ģʽ��
	//�¼��ϱ���ʶ�����ϱ���0�����¼������ϱ���1�����¼��ָ��ϱ���2�����¼������ָ����ϱ���3��
	BYTE 	byReportMode[eNUM_OF_EVENT_PRG];
	//�¼��ϱ���ʽ 0�������ϱ� 1�������ϱ�
	BYTE 	byReportMethod[(eNUM_OF_EVENT_PRG/8)+1];
	BYTE 	byReportStatusMethod;
	//4300	�����豸 ������10	�ϱ�ͨ��	array OAD
	DWORD 	ReportChannelOAD[MAX_COM_CHANNEL_NUM];
	DWORD	CRC32;
}TReportMode;

typedef struct TReportData_t
{
	BYTE Status[4];							//�����ϱ�״̬��
	BYTE Index[SUB_EVENT_INDEX_MAX_NUM];	//�����¼��б�
	DWORD CRC32;
}TReportData;

typedef struct TReportTimes_t
{
	BYTE Times;					//�ϱ�����
	DWORD CRC32;
}TReportTimes;
#pragma pack()

typedef struct TReportSafeRom_t
{
	TReportMode		ReportMode;
	#if( SEL_DLT645_2007 == YES )
	TReport645ResetTime Report645ResetTime;
	TReport645 Report645;
	#endif
	TReportTimes ReportTimes;
}TReportSafeRom;

typedef struct TReport_t
{
   TReportData ReportData[MAX_COM_CHANNEL_NUM];//�ϱ�����
	
}TReportConRom;
//-----------------------------------------------
//				��������
extern __no_init TReportData ReportData[MAX_COM_CHANNEL_NUM];//���ܱ�����ϱ�״̬��
extern __no_init TReportData ReportDataBak[MAX_COM_CHANNEL_NUM];
extern WORD EventLostPowerEndTime; //�����¼�����ʱ�䣬�ۼӣ���λ�룩


//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: 	�����ϱ�ʱ�����
//
//����: 		��
//        	
//����ֵ:		��
//			
//��ע:�����ϱ�ʱ�䴦������systick�ж��У�Խ����Խ�ã�
//-----------------------------------------------
void ReportTimeMinus( );

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
void SetReportIndex( BYTE inSubEventIndex, BYTE BeforeAfter );

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
void ClrReportIndexChannel( BYTE Ch, BYTE inSubEventIndex );
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
void ClrReportAllChannelIndex(  BYTE inEventIndex );

//-----------------------------------------------
//��������: ���ϱ�����
//
//����: 	��
//  	                	
//����ֵ:	��
//			
//��ע:������ͨ���������¼��б�ָ��
//-----------------------------------------------
void ClearReport( BYTE Type );

//-----------------------------------------------
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
BOOL GetFirstReportOad(BYTE Type, BYTE Ch, BYTE *Buf);//��������: ��ȡ�����¼��б�OAD
//
//����: 	
//		Len[in]:	�������ݳ���
//		Buf[out]��	array OAD 
//           	
//����ֵ:	�������ݳ��� 0x8000--����Buf���Ȳ���
//			
//��ע:�����¼��б� ����2
//-----------------------------------------------
WORD GetReportOadList( eGetReportListType GetReportListType, BYTE Ch, WORD Len, BYTE *Buf);

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
void api_LostPowerReportFraming( BYTE inSubEventIndex ,DWORD RecordNo );

//-----------------------------------------------
//��������: �����ϱ��ж�
//
//����:	   ��
//         
//����ֵ:     TRUE�����������ϱ�         FLASE:�������ϱ�
//			
//��ע: 
//-----------------------------------------------
WORD api_JudgeActiveReport( void );
//-----------------------------------------------
//��������: ȷ�������ϱ�
//
//����:	   BYTE Type[in] 0x00: ȷ�������¼��б� 0x55��ȷ�ϸ����ϱ�״̬��
//         
//			
//��ע: 
//-----------------------------------------------
void api_ResponseActiveReport( BYTE Type );

//-----------------------------------------------
//��������: �����ϱ��б�
//
//����: 	
//           	
//����ֵ:	
//			
//��ע:
//-----------------------------------------------
void api_BackupReportIndex( void );

//-----------------------------------------------
//��������: ��ȡ���ϱ��¼������б� OI
//
//����: 	
//		Len[in]:	�������ݳ���
//		Buf[out]��	array OI 
//           	
//����ֵ:	�������ݳ��� 0x8000--����Buf���Ȳ���
//			
//��ע:�����¼��б� ����3 ���ϱ��¼������б�
//-----------------------------------------------
WORD GetReportOIList(WORD Len, BYTE *Buf);

//-----------------------------------------------
//��������: �ϱ�����ʼ��
//
//����: 	��	
//           	
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------
void FactoryInitReport(void);

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
BOOL SetReportChannel( BYTE byIndex, BYTE Num, BYTE *pBuf );
//-----------------------------------------------
//��������: ��ȡ�����ϱ�ͨ����־
//
//����: 	   Ch[in]:		ͨ����
//           	
//����ֵ:	   TRUE-��ͨ��֧���ϱ�	FALSE--��ͨ����֧���ϱ�
//			
//��ע:
//-----------------------------------------------
BOOL api_GetReportChannelStatus( BYTE Ch );
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
WORD ReadReportChannel( BYTE *pBuf );


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

BOOL SetReportFlag( BYTE bMeterReportFlag,eReportType Type );

//-----------------------------------------------
//��������: ��ȡ�����ϱ���־
//
//����: 	Type		eFollowReport/eActiveReport
//
//����ֵ:	TRUE--�����ϱ�	FALSE--�������ϱ�
//			
//��ע:4300�����豸 ����7.8 ��������ϱ�,���������ϱ�
//-----------------------------------------------

BOOL ReadReportFlag( eReportType Type );

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
BOOL api_SetEventReportMode( eReportClass ReportClass, WORD OI, BYTE byReportMode );

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
BOOL api_ReadEventReportMode( eReportClass ReportClass, WORD OI, BYTE *pBuf );


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
void api_SetFollowReportStatus(BYTE StatusNo);
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
WORD api_ReadFollowReportStatus_Mode( BYTE Ch, BYTE Mode, BYTE *pBuf );

//-----------------------------------------------
//��������: �ϱ�Сʱ����
//
//����: 	��
//
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------
void ReportHourTask(void);

//-----------------------------------------------
//��������: ���ø����ϱ�ģʽ��
//
//����:    
//			pBuf[in]��	�������ݵĻ���
//����ֵ:	TRUE/FALSE  
//			
//��ע: 
//-----------------------------------------------
WORD api_SetFollowReportMode( BYTE *pBuf );
//-----------------------------------------------
//��������: �ϱ�������
//
//����: 	��
//
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------
void ReportSecTask(void);

//-----------------------------------------------
//��������: �ϱ��ϵ�����
//
//����: 	��
//
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------		
void PowerUpReport(void);

//-----------------------------------------------
//��������: �ϱ���������
//
//����: 	��
//
//����ֵ:	��
//			
//��ע:
//-----------------------------------------------	
void PowerDownReport(void);
//-----------------------------------------------
//��������: ���������ϱ�����
//
//����:	 
//         
//����ֵ:   
//			
//��ע: �͹��Ĵ�ѭ������
//-----------------------------------------------
void ProcLowPowerMeterReport( void);


#endif//��Ӧ�ļ���ǰ��� #define __REPORT698_API_H
