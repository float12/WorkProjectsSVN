//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ� κ����
//��������	2016.8.4
//����		DL/T 698.45�������Э��C�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "GDW698/GDW698.h"
#include "MCollect.h"
#include "bsp_cpu_flash.h"
#if ( SEL_DLT698_2016_FUNC == YES  )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
// ������Ϣ��EEPROM�еĵ�ַ��Χ


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
//-----------------------------------------------
//				ȫ��ʹ�õı���������
__no_init BYTE g_PassWordErrCounter;//����������
BYTE RN[MAX_RN_SIZE];
BYTE g_TimeTagBuf[12];
BYTE g_byClrTimer_500ms;
BYTE ClientAddress;//�ͻ�����ַ
BYTE ConnectMode;  //����ģʽ---ֻ������λ����ȡ���ӻ���-����������
TGetRequestNext pGetRequestNext[MAX_COM_CHANNEL_NUM];




TDLT698LinkData LinkData[MAX_COM_CHANNEL_NUM];
TDLT698APPData APPData[MAX_COM_CHANNEL_NUM];
BYTE DLT698APDURequestBuf[MAX_APDU_SIZE+EXTRA_BUF];
BYTE ProtocolBuf[MAX_APDU_SIZE+EXTRA_BUF];// MAX_APDU_SIZE = 2000, EXTRA_BUF = 30
BYTE ProtocolBufBack[MAX_APDU_SIZE+EXTRA_BUF];// MAX_APDU_SIZE = 2000, EXTRA_BUF = 30
__no_init BYTE SecurityRequestBuf[50+EXTRA_BUF];//�·��������ݴ��
__no_init BYTE SecurityResponseBuf[50+EXTRA_BUF];//���ؽ������ݴ��

TDLT698APPConnect APPConnect;
TSafeModePara SafeModePara;
TAPDUBufFlag APDUBufFlag;
TConsultData ConsultData;
T_Iap tIap;
BYTE IapStatus[(MAX_SIZE_FOR_IAP/LEN_OF_ONE_FRAME)/8+1];// F001 ������4 ����״̬��������Ϊ����ʵ��һ֡����ĳ��ȿ��ٿռ�!!!!!!
TIapInfo tIapInfo;// ��ʱʹ�ã����豣��
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
const DWORD TIMinutes[] = { 1,1,60,1440,43200,525600 };//ʱ���� �� �� ʱ �� �������� �����⴦��
const DWORD FollowReportOAD[] = { 0x201e0200, 0x20200200 };//�����ϱ���Ҫ��ȡ��¼������ �¼�����ʱ�� �¼�����ʱ��
const DWORD FixedFreezeOAD[] = { 2,0x00022320, 0x00022120 };//OAD����  �����¼���OAD  ���ݶ���ʱ��OAD

const DWORD StandardEventUnit[6][2] =		
{ 	//��׼�¼���¼��Ԫ
	{0X00000005,0x00000000 },//����
	{0X01020133,0x00022220 },//�¼���¼���
	{0X02020133,0x00021e20 },//�¼�����ʱ��
	{0X03020133,0x00022020 },//�¼�����ʱ��
	{0X04020133,0x00022420 },//�¼�����Դ
	{0X05020133,0x00020033 },//�¼��ϱ�״̬
}; 
									
const DWORD ProgEventUnit[7][2] =			
{ 	//����¼���¼��Ԫ
	{0X00000006,0x00000000 },//����
	{0X01020233,0x00022220 },//�¼���¼���
	{0X02020233,0x00021e20 },//�¼�����ʱ��
	{0X03020233,0x00022020 },//�¼�����ʱ��
	{0X04020233,0x00022420 },//�¼�����Դ
	{0X05020233,0x00020033 },//�¼��ϱ�״̬
	{0X06020233,0X06020233 },//��̶����б�
} ;
									
const DWORD OverDemandEventUnit[8][2] =		
{ 	//���������¼���¼��Ԫ
	{0X00000007,0x00000000 },//����
	{0X01020833,0x00022220 },//�¼���¼���
	{0X02020833,0x00021e20 },//�¼�����ʱ��
	{0X03020833,0x00022020 },//�¼�����ʱ��
	{0X04020833,0x00022420 },//�¼�����Դ
	{0X05020833,0x00020033 },//�¼��ϱ�״̬
	{0X06020833,0X06020833 },//�����ڼ������й��������ֵ
	{0X07020833,0X07020833 },//�����ڼ��������ֵ����ʱ��
}; 
										
const DWORD EventClearUnit[7][2] =			
{ 	//�¼������¼��Ԫ
	{0X00000006,0x00000000 },//����
	{0X01020c33,0x00022220 },//�¼���¼���
	{0X02020c33,0x00021e20 },//�¼�����ʱ��
	{0X03020c33,0x00022020 },//�¼�����ʱ��
	{0X04020c33,0x00022420 },//�¼�����Դ
	{0X05020c33,0x00020033 },//�¼��ϱ�״̬
	{0X06020c33,0X06020c33 },//�¼������б�
} ;	
											
const DWORD EventPhaseUnit[8][2] =			
{ 	//�����¼���¼��Ԫ
	{0X00000004,0x00000000 },//����
	{0x00022220,0x00022220 },//�¼���¼���
	{0x00021e20,0x00021e20 },//�¼�����ʱ��
	{0x00022020,0x00022020 },//�¼�����ʱ��
	{0x00020033,0x00020033 },//�¼��ϱ�״̬
} ;		

const DWORD EventSegTableUnit[8][2]=
{// ʱ�α��̼�¼��Ԫ0x3314
	{0X00000007,0x00000000 },//����
	{0X01021433,0x00022220 },//�¼���¼���
	{0X02021433,0x00021e20 },//�¼�����ʱ��
	{0X03021433,0x00022020 },//�¼�����ʱ��
	{0X04021433,0x00022420 },//�¼�����Դ
	{0X05021433,0x00020033 },//�¼��ϱ�״̬
	{0X06021433,0X06021433 },//�����ʱ�ζ���OAD
	{0X07021433,0X07021433 },//�����ʱ�α�
};	

const DWORD EventHolidayUnit[8][2]=
{// �ڼ��ձ�̼�¼��Ԫ0x3315
	{0X00000007,0x00000000 },//����
	{0X01021533,0x00022220 },//�¼���¼���
	{0X02021533,0x00021e20 },//�¼�����ʱ��
	{0X03021533,0x00022020 },//�¼�����ʱ��
	{0X04021533,0x00022420 },//�¼�����Դ
	{0X05021533,0x00020033 },//�¼��ϱ�״̬
	{0X06021533,0X06021533 },//��̽ڼ��ն���OAD
	{0X07021533,0X07021533 },//��̽ڼ�������
};	

const DWORD ABR_CARDUnit[10][2] =			
{ 	//�쳣�忨�¼���¼��Ԫ0x3310
	{0X00000009,0x00000000 },//����
	{0X01021033,0x00022220 },//�¼���¼���double-long-unsigned��
	{0X02021033,0x00021e20 },//�¼�����ʱ�� date_time_s��
	{0X03021033,0x00022020 },//�¼�����ʱ��date_time_s��
	{0X04021033,0x00022420 },//�¼�����Դ2017��9�Ų���:�����쳣�忨�¼���¼��Ԫ����OI��3310�����¼�����Դ����ΪNULL��
	{0X05021033,0x00020033 },//�¼��ϱ�״̬
	{0X06021033,0X06021033 },//�����к�	octet-string��
	{0X07021033,0X07021033 },//�忨������Ϣ��	unsigned��
	{0X08021033,0X08021033 },//�忨��������ͷ	octet-string��
	{0X09021033,0X09021033 },//�忨������Ӧ״̬  long-unsigned��
} ;	

const DWORD ReturnMoneyUnit[7][2] =			
{ 	//�˷��¼���¼��Ԫ0x3311
	{0X00000006,0x00000000 },//����
	{0X01021033,0x00022220 },//�¼���¼���double-long-unsigned��
	{0X02021033,0x00021e20 },//�¼�����ʱ�� date_time_s��
	{0X03021033,0x00022020 },//�¼�����ʱ��date_time_s��
	{0X04021033,0x00022420 },//�¼�����ԴNULL��
	{0X05021033,0x00020033 },//�¼��ϱ�״̬
	{0X06021033,0X06021133 },//�˷ѽ��      double-long-unsigned����λ��Ԫ�����㣺-2����
} ;	

const DWORD ChargingWARNINGUnit[8][2] =			
{ 	//���׮�¼���չ��Ԫ
	{0X00000007,0x00000000 },//����
	{0X01022A33,0x00022220 },//�¼���¼���
	{0X02022A33,0x00021e20 },//�¼�����ʱ��
	{0X03022A33,0x00022020 },//�¼�����ʱ��
	{0X04022A33,0x00022420 },//�¼�����Դ
	{0X05022A33,0x00020033 },//�¼��ϱ�״̬
	{0X06022A33,0X06022A33 },//��������  bit-string
	{0X07022A33,0X07022A33 },//��������  bit-string
} ;	
const DWORD ChargingERRUnit[8][2] =			
{ 	//���׮�¼���չ��Ԫ
	{0X00000007,0x00000000 },//����
	{0X01022B33,0x00022220 },//�¼���¼���
	{0X02022B33,0x00021e20 },//�¼�����ʱ��
	{0X03022B33,0x00022020 },//�¼�����ʱ��
	{0X04022B33,0x00022420 },//�¼�����Դ
	{0X05022B33,0x00020033 },//�¼��ϱ�״̬
	{0X06022B33,0X06022B33 },//��������  bit-string
	{0X07022B33,0X07022B33 },//��������  bit-string
} ;	
const DWORD ChargingCOM_EXCTIONUnit[8][2] =			
{ 	//���׮�¼���չ��Ԫ
	{0X00000007,0x00000000 },//����
	{0X01022C33,0x00022220 },//�¼���¼���
	{0X02022C33,0x00021e20 },//�¼�����ʱ��
	{0X03022C33,0x00022020 },//�¼�����ʱ��
	{0X04022C33,0x00022420 },//�¼�����Դ
	{0X05022C33,0x00020033 },//�¼��ϱ�״̬
	{0X06022C33,0X06022C33 },//��������  bit-string
	{0X07022C33,0X07022C33 },//��������  bit-string
} ;	
const DWORD ModuleEventUnit[][2]=
{	//��չģ���¼���Ԫ
	{0X00000007,0x00000000 },//����
	{0X0102E033,0x00022220 },//�¼���¼���
	{0X0202E033,0x00021e20 },//�¼�����ʱ��
	{0X0302E033,0x00022020 },//�¼�����ʱ��
	{0X0402E033,0x00022420 },//�¼�����Դ
	{0X0502E033,0x00020033 },//�¼��ϱ�״̬
	{0X0602E033,0X0602E033 },//��������  octet-string
	{0X0702E033,0X0702E033 },//��������  octet-string
};	

#if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
const DWORD FindUnknownMeterUnit[7][2] =			
{ 	
	{0X00000006,0x00000000 },//����
	{0X01020333,0x00022220 },//�¼���¼���
	{0X02020333,0x00021e20 },//�¼�����ʱ��
	{0X03020333,0x00022020 },//�¼�����ʱ��
	{0X04020333,0x00022420 },//�¼�����Դ
	{0X05020333,0x00020033 },//�¼��ϱ�״̬
	{0X06020333,0X00020260 },//�ѱ��� Array
} ;	
#endif

#if (SEL_TOPOLOGY == YES )
const DWORD TopoUnit[7][2] =			
{ 	
	{0X00000006,0x00000000 },//����
	{0X0102833E,0x00022220 },//�¼���¼���
	{0X0202833E,0x00021e20 },//�¼�����ʱ��
	{0X0302833E,0x00022020 },//�¼�����ʱ��
	{0X0402833E,0x00022420 },//�¼�����Դ
	{0X0502833E,0x00020033 },//�¼��ϱ�״̬
	{0X0602833E,0X0602833E },//ʶ���� Array
} ;
#endif

const DWORD (*pEventUnit[])[2] = 		
{
	EventPhaseUnit,			//3000	���ܱ�ʧѹ�¼�
	EventPhaseUnit,			//3001	���ܱ�Ƿѹ�¼�
	EventPhaseUnit,			//3002	���ܱ��ѹ�¼�
	EventPhaseUnit,			//3003	���ܱ�����¼�
	EventPhaseUnit,			//3004	���ܱ�ʧ���¼�
	EventPhaseUnit,			//3005	���ܱ�����¼�
	EventPhaseUnit,			//3006	���ܱ�����¼�
	EventPhaseUnit,			//3007	���ܱ��ʷ����¼�
	EventPhaseUnit,			//3008	���ܱ�����¼�
	OverDemandEventUnit,	//3009	���ܱ������й����������¼�
	OverDemandEventUnit,	//300A	���ܱ����й����������¼�
	OverDemandEventUnit,	//300B	���ܱ��޹����������¼�
	StandardEventUnit,		//300C	
	StandardEventUnit,		//300D	���ܱ�ȫʧѹ�¼�
	StandardEventUnit,		//300E	���ܱ�����Դ�����¼�
	StandardEventUnit,		//300F	���ܱ��ѹ�������¼�
	StandardEventUnit,		//3010	���ܱ�����������¼�
	StandardEventUnit,		//3011	���ܱ�����¼�
	ProgEventUnit,			//3012	���ܱ����¼�
	StandardEventUnit,		//3013	���ܱ������¼�
	StandardEventUnit,		//3014	���ܱ����������¼�
	EventClearUnit,			//3015	���ܱ��¼������¼�
	StandardEventUnit,		//3016	���ܱ�Уʱ�¼�
	EventSegTableUnit,		//3017	���ܱ�ʱ�α����¼�
	StandardEventUnit,		//3018	���ܱ�ʱ�������¼�
	StandardEventUnit,		//3019	���ܱ������ձ���¼�
	StandardEventUnit,		//301A	���ܱ�����ձ���¼�
	StandardEventUnit,		//301B	���ܱ����¼�
	StandardEventUnit,		//301C	���ܱ���ť���¼�
	StandardEventUnit,		//301D	���ܱ��ѹ��ƽ���¼�
	StandardEventUnit,		//301E	���ܱ������ƽ���¼�
	StandardEventUnit,		//301F	���ܱ���բ�¼�
	StandardEventUnit,		//3020	���ܱ��բ�¼�
	EventHolidayUnit,		//3021	���ܱ�ڼ��ձ���¼�
	StandardEventUnit,		//3022	���ܱ��й���Ϸ�ʽ����¼�
	StandardEventUnit,		//3023	���ܱ��޹���Ϸ�ʽ����¼�
	StandardEventUnit,		//3024	���ܱ���ʲ��������¼�
	StandardEventUnit,		//3025	���ܱ���ݱ����¼�
	StandardEventUnit,		//3026	���ܱ���Կ�����¼�
	ABR_CARDUnit,			//3027	�쳣�忨��¼	
	StandardEventUnit,		//3028	���ܱ����¼
	ReturnMoneyUnit,		//3029	�˷Ѽ�¼		
	StandardEventUnit,		//302A	���ܱ�㶨�ų������¼�
	StandardEventUnit,		//302B	���ܱ��ɿ��������¼�
	StandardEventUnit,		//302C	���ܱ��Դ�쳣�¼�
 	StandardEventUnit,		//302D	���ܱ�������ز�ƽ���¼�	
 	StandardEventUnit,		//302E	���ܱ�ʱ�ӹ����¼�
 	StandardEventUnit,		//302F	���ܱ����оƬ�����¼�
 	EventPhaseUnit,			//303B	���ܱ��������������¼�
 	StandardEventUnit,		//303C	���ܱ�㲥Уʱ�¼�
 	StandardEventUnit,		//3040	���ܱ����ߵ����쳣
 	StandardEventUnit,		//3040	״̬����λ�¼�
};

static const BYTE IapFacCode[16] = {"WisdomWaveRecord"};
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void ProcDlt698App( BYTE Ch);
static void DealGet_Request(BYTE Ch,BYTE *pAPDU);
static WORD AnalyseDlt698ApduLen( BYTE Ch);
//-----------------------------------------------
//				��������
//-----------------------------------------------
#if (SEL_TOPOLOGY == YES )
eAPPBufResultChoice ActionRequestTopology( BYTE Ch, BYTE *pOMD);
#endif

extern WORD JudgeRecMeterNo_Dlt698_45(TSerial *p);
//--------------------------------------------------
//��������:  ������㺯��
//         
//����:     ��
//         
//����ֵ:    �� 
//         
//��ע����:  ��
//--------------------------------------------------
static void ClearMeter( void )
{
	api_ClrFreezeData();
	api_ClrEnergy();
	api_ClrAllEvent( eEXCEPT_CLEAR_METER );
	api_ClrPrePay();
	Task_ClearMeter_Relay();
	
	#if( MAX_PHASE == 3 )
	#if( SEL_DEMAND_2022 == NO )//����������
	api_ClrDemand( eClearAllDemand );
	#else
	api_ClrDemand( eClearAllDemand, eDemandPause );
	#endif
	#endif
	
	#if( SEL_AHOUR_FUNC == YES)
	api_ClrAHour();
	#endif
	#if( SEL_STAT_V_RUN == YES )
	api_ClearVRunTimeNow( 0xff );
	#endif
	    
	api_ClrRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW );
	api_ClrRunHardFlag( eRUN_HARD_READ_BAT_LOW );
	api_ClrRunHardFlag( eRUN_HARD_ERR_RTC_FLAG );
	api_ClrRunHardFlag( eRUN_HARD_BROADCAST_ERR_FLAG);
	api_ClrRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
	api_ClrRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );
	g_PassWordErrCounter = 0;
	memset( g_645PassWordErrCounter, 0x00, sizeof(g_645PassWordErrCounter) );
    
    #if (SEL_SEARCH_METER == YES )
    api_ClearSchMeter();
    #endif
}

//--------------------------------------------------
//��������:  698.45�ϵ纯��
//         
//����  :   ��
//
//����ֵ:    ��  
//         
//��ע����:  ��
//--------------------------------------------------
void PowerUpDlt698(void)
{
	BYTE i;
	
	memset( &APPConnect,0x00,sizeof( APPConnect ));//��ʼ������Ӧ�����ӽṹ��
	g_byClrTimer_500ms = 0;
	FactoryTime = 0;//����esam�򿪳���ģʽʱ��
	api_ClrSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
	api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
	api_ClrSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);

	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		Serial[i].Addr_Len = 6;//ͨ�ŵ�ַĬ��6�ֽ�
	}
		
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE*)&SafeModePara );	
	PowerUpInitAPDUBufFlag();
}

//-----------------------------------------------
//��������  :   ��ʼ��698����
//
//����  : 	��
//
//����ֵ:      ��
//
//��ע����  :   ��
//-----------------------------------------------
void FactoryInitDLT698(void)
{
	TTimeBroadCastPara tmpTimeBroadCastPara;
	//��ʼ���㲥Уʱ����
	tmpTimeBroadCastPara.TimeMinLimit = TimeBroadCastTimeMinLimit;
	tmpTimeBroadCastPara.TimeMaxLimit = TimeBroadCastTimeMaxLimit;
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara );

    g_PassWordErrCounter = 0;
}

//-----------------------------------------------
//��������  :   ��ʼ��698��־
//
//����  : 	��
//
//����ֵ:      ��
//
//��ע����  :   ��
//-----------------------------------------------
void InitDLT698Flag( BYTE Ch )
{
    APPData[Ch].APPFlag = 0;
}

//-----------------------------------------------
//��������  : 698.45������
//
//����  :    TSerial *p[in]
//
//����ֵ:     ��
//��ע����  :  ��
//-----------------------------------------------
void ProcMessageDlt698(TSerial *p)
{
	BYTE i;
	
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( p == &Serial[i] )
		{
			FunctionConst(PROTOCOL698_TASK1);
			Proc698LinkDataRequest(i);		//��·����������
			FunctionConst(PROTOCOL698_TASK2);
			ProcDlt698App(i);             	//Ӧ�ò㴦����
			FunctionConst(PROTOCOL698_TASK3);
			Proc698LinkDataResponse(i);		//��·����Ӧ������
			break;
		}
	}
}
//-----------------------------------------------
//��������  : 698.45��ԼԤ����
//
//����  :    TSerial *p[in]
//
//����ֵ:     BOOL ��TRUE-����͸�������� ���� �ɼ����մ���
//��ע����  :  ��
//-----------------------------------------------
BOOL Pre_Dlt698(TSerial *p, BYTE *pAck)
{
	BYTE i = 0;
	BOOL bRc = FALSE;
	BYTE IsMyMeteraddr;

	if( JudgeRecMeterNo_Dlt698_45(p) == FALSE )
	{
		IsMyMeteraddr = FALSE;
	}
	else
	{
		IsMyMeteraddr = TRUE;
	}
	
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( p == &Serial[i] )
		{
			if (IsUpCommuFrame(PRO_DLT698_45,Serial[i].ProBuf))
			{
				if ( IsJLing(i) ) /* ����Ӧ�� */
				{		
					JLWriteRx(i,Serial[i].ProBuf,Serial[i].ProStepDlt698_45);
					bRc = TRUE;
				}
				else  /* ����Ӧ�� */
				{
					GyRxMonitor(i,PRO_DLT698_45);
					bRc = TRUE;
				}
			} 
			else if( ( (MessageTransParaRam.AllowUnknownAddrTrans == TRUE) && (IsMyMeteraddr== FALSE) )
					|| (IsInSysMeters(&p->ProBuf[5]))
				//  && (IsChargeMeters(&p->ProBuf[5]) == FALSE)
			)//�ɼ���ģʽ����
			{
				CollectorWriteTx(i,Serial[i].ProBuf,&p->ProBuf[5],Serial[i].ProStepDlt698_45);
				bRc = TRUE;
			}

			break;
		}
	}
	return bRc;
}
//--------------------------------------------------
//��������:�Ͽ�Ӧ������
//         
//����  :   ��
//
//����ֵ:    ��   
//         
//��ע����:  ��
//--------------------------------------------------
void api_Release698_Connect( void )
{
	//������֤ʱ��698��645��ͬһ������ �˴��������������֤ʱ�� ���������֤�������֤ ������֤ʱ���������
	memset( &APPConnect.ConnectInfo, 0x00, sizeof(APPConnect.ConnectInfo) );
	APPConnect.TerminalAuthTime = 0;
	
	api_ClrSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
	api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
	api_ClrSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);
}
//--------------------------------------------------
//��������:ʱЧ�������ж� �������Ͽ�Ӧ�����ӡ�����ʱЧ����ʱ��ȡ��
//         
//����  :   ��
//
//����ֵ:    ��   
//         
//��ע����:  ��
//--------------------------------------------------
void TimeLinessEventJudge( void )
{
    BYTE Result,pBuf[20],i;
    TFourByteUnion Time;
	WORD EsamLen;
	eConnectMode ConnectMode;

	for( i = 0; i < eConnectModeMaxNum; i++ )
	{
		if( APPConnect.ConnectInfo[i].ConnectValidTime != 0 )
		{
			if( i == eConnectGeneral )//һ��������м���ֵ�ж� ��������Ӧ������ esam�ᱣ֤����˲�����
			{
				if( APPConnect.ConnectInfo[i].ConnectValidTime > 604800)//ʱ�䳬��7����Ϊ���ݷǷ�
				{
					APPConnect.ConnectInfo[i].ConnectValidTime = 1;
				}
			}
			
			if( i > eConnectGeneral )//�ԳƼ���ģʽ
			{
				if(((APPConnect.ConnectInfo[i].ConnectValidTime%60)==0)
				&&(   (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE)
					||(api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE)) )//ÿ���ӽ��лỰʣ��ʱ���ж�-���ʣ��ʱ��Ϊ�������ǰ����
				{
					if( i == eConnectTerminal )
					{
						EsamLen = api_GetEsamInfo( 0x14, pBuf ); //��f1000500 ESAM���������14���ỰʱЧ����
					}
					else
					{
						EsamLen = api_GetEsamInfo( 5, pBuf ); //��f1000500 ESAM���������5���ỰʱЧ����
					}
					if( EsamLen > (sizeof(pBuf) - 2) )
					{
						APPConnect.ConnectInfo[i].ConnectValidTime = 1;
					}
					
					lib_ExchangeData( Time.b, pBuf+4, 4 );

					if( Time.d == 0 )
					{
						APPConnect.ConnectInfo[i].ConnectValidTime = 1;
					}	
				}
			}

			APPConnect.ConnectInfo[i].ConnectValidTime -= 1;

			if( APPConnect.ConnectInfo[i].ConnectValidTime == 0 )
			{
				if( (APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime == 0)//��վ���նˡ�һ�㽨��Ӧ������ͬʱΪ0ʱ�Ḵλesam
				&& (APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime == 0)
				&& (APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime == 0) )
				{
					if( (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE)
					||(api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE) 
					||(api_GetSysStatus( eSYS_STATUS_ID_698GENERAL_AUTH ) == TRUE))
					{
						api_SaveProgramRecord( EVENT_END, 0x51, (BYTE *)NULL);
					}
					else if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE )
					{
						api_SaveProgramRecord645(EVENT_END, (BYTE *)NULL, (BYTE *)NULL );
					}
					else
					{

					}
					api_ResetEsamSpi();
					ESAMSPIPowerDown( ePowerOnMode );//�ر�esam��Դ
					api_ClrSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
					api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
					api_ClrSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);
					api_ClrSysStatus(eSYS_STATUS_ID_645AUTH);
					ConsultData.channel = 0X55;//�ָ�Ĭ��ֵ
					ConsultData.Len = MAX_PRO_BUF;
				}	

				if( i == eConnectGeneral )//һ������ʱ�䵽
				{
					api_ClrSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);
				}
				else if( i == eConnectMaster )//��վӦ������ʱ�䵽
				{
					api_ClrSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
					api_ClrSysStatus(eSYS_STATUS_ID_645AUTH);
				}
				else//�ն�Ӧ������ʱ�䵽
				{
					api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
				}
				
				APPConnect.ConnectInfo[i].ConstConnectValidTime = 0;
			}

		}

	}

	if( APPConnect.IRAuthTime != 0 )
	{
		if( APPConnect.IRAuthTime > 604800 )//ʱ�䳬��7�� ��Ϊʱ��Ƿ�
		{
			APPConnect.IRAuthTime = 1;
		}
		
        APPConnect.IRAuthTime = (APPConnect.IRAuthTime -1);
        if( APPConnect.IRAuthTime == 0 )
        {
            api_ClrSysStatus( eSYS_IR_ALLOW_PRG );//ʱ�䵽 �������֤��־
        }
	}
	
	if( APPConnect.TerminalAuthTime != 0 )
	{
		if( APPConnect.TerminalAuthTime > 604800 )//ʱ�䳬��7�� ��Ϊʱ��Ƿ�
		{
			APPConnect.TerminalAuthTime = 1;
		}
		
        APPConnect.TerminalAuthTime = (APPConnect.TerminalAuthTime -1);
        if( APPConnect.TerminalAuthTime == 0 )
        {
            api_ClrSysStatus( eSYS_TERMINAL_ALLOW_PRG );//ʱ�䵽 �������֤��־
        }
	}

	if( FactoryTime != 0 )//����ģʽʱ����пۼ�
	{
        FactoryTime --;
	}

	if( FrameOverTime != 0 )//��֡��ʱ�ۼ�
	{
		FrameOverTime --;
	}
}

//--------------------------------------------------
//��������: APDUBufFlag�ϵ��ʼ��
//         
//����  :    ��
//
//����ֵ:     ��   
//         
//��ע����:    ��
//--------------------------------------------------
void PowerUpInitAPDUBufFlag( void )
{
    memset((BYTE*)&APDUBufFlag.Request.Ch,0x00, sizeof(APDUBufFlag));
    APDUBufFlag.Request.Flag = FALSE;
}

//--------------------------------------------------
//��������: APDUBufFlag��ʼ��
//         
//����  :    ��
//
//����ֵ:     ��   
//         
//��ע����:    ��
//--------------------------------------------------
void InitAPDUBufFlag( BYTE Ch )
{
    if( APDUBufFlag.Request.Ch == Ch )
    {
        APDUBufFlag.Request.Flag = FALSE;
    }
}

//--------------------------------------------------
//��������: ��ȡ��ͨ��Ĭ�����֡buf����
//         
//����  :    BYTE Ch[in] ͨ��
//
//����ֵ:     Ĭ�ϳ���   
//         
//��ע����:    ��
//--------------------------------------------------
DWORD GetDefaultConsultBufLen( BYTE Ch )
{
    if( Ch == eCR )//��ǰͨ��Ϊ�ز�ͨ��
    {
       return DEFAULT_MAX_PRO_BUF_CR;
    }
    else if( Ch == eIR )//��ǰͨ��Ϊ����ͨ��
    {
      return DEFAULT_MAX_PRO_BUF_IR;
    }
    else//��ǰͨ��Ϊ485ͨ��
    {
       return DEFAULT_MAX_PRO_BUF_485;
    }
}

//--------------------------------------------------
//��������: ��ȡ��ͨ��Ĭ�����֡buf����
//         
//����  :    BYTE Ch[in] ͨ��
//
//����ֵ:     Э�̳���   
//         
//��ע����:    ��
//--------------------------------------------------
DWORD GetConsultBufLen(   BYTE Ch )
{
   DWORD ConsultLen;
   
   ConsultLen = GetDefaultConsultBufLen(Ch);
   
    if( (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE)
	||(api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE) 
	||(api_GetSysStatus( eSYS_STATUS_ID_698GENERAL_AUTH ) == TRUE))//ͨ������ҽ���Ӧ�����ӳɹ�
    {
        if( ConsultData.channel == Ch )//��������¡�ͨ���϶����
        {
            if( ConsultData.Len >= MAX_PRO_BUF )//��������ռ䳤��ʹ����󳤶�
            {
                return MAX_PRO_BUF;
            }
            else
            {
                if( ConsultData.Len < MIN_CONSULT_BUF_LEN )//��Ϊͨ��Э�̳��Ȳ��ܵ���100������100ʹ��Ĭ�ϳ���
                {
                    ConsultData.Len = ConsultLen;
                }
                
                return ConsultData.Len;
            }
        }
        else//�쳣���
        {
           return ConsultLen;
        }
    }
    else
    {
       return ConsultLen;
    }
}

//--------------------------------------------------
//��������:  ��ѭ�����㺯��
//         
//����  :   ��
//
//����ֵ:    ��   
//         
//��ע����:  ÿ500�������һ��
//--------------------------------------------------
void ProcClrTask(void)
{
	WORD wTmpClearMeterType;

	//wClearMeterType //0x1111--�������(���ݳ�ʼ��) 0x2222--�ָ��������� 0x4444--�¼����� 0x8888--��������
	if( FPara1->wClearMeterType == 0 )
	{
		return;
	}
	
	if( g_byClrTimer_500ms != 0 )//����500���붨ʱ�����ϵ���0
	{
		g_byClrTimer_500ms--;
		return;
	}
	
	//���������1У��������˳�����ִ�����㣬��ֹ��EEPROM�ϵ���������
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara1), sizeof(TFPara1), UN_REPAIR_CRC ) == FALSE )
	{	
		return;
	}
	
	wTmpClearMeterType = FPara1->wClearMeterType;

	// �������ʱ������˳�������--������--�嶳������--�帺������--���¼�
	// ���¼����������������Ϊ�˼����¼���������

	//��Ԥ�ÿ����㣬�����㣬������д��ʱ�������ݶ���0����������˴������������
	//Զ�̱�û�п���д���ܣ���Ӧ�𣬺����㣬���طѿر���Ǯ����ʼ������ʱû�з�д��Ҳ��Ӧ�𣬺�����
	
	if( (wTmpClearMeterType == eCLEAR_METER) || (wTmpClearMeterType == eCLEAR_MONEY) )//����3. ���ݳ�ʼ�� ����ǰ ������� ��
	{
		ClearMeter( );	
		CLEAR_WATCH_DOG;
		#if( PREPAY_MODE == PREPAY_LOCAL )
		if(wTmpClearMeterType == eCLEAR_MONEY)//eCLEAR_MONEY   = 0x6666,		//Ǯ����ʼ�������Ԥ�ÿ�
		{
			api_SavePrgOperationRecord( ePRG_BUY_MONEY_NO ); //�����¼�й���ǰ���ݡ���������ݣ��ܲ��ܲο�����������������ǰ�����������Ƚ���
		}		
		#endif
	}
	else if( wTmpClearMeterType == eCLEAR_FACTORY )//��ʼ������
	{
		ClearMeter( );		
		InitPara( 1 );
		api_FactoryInitTask( );
	}
	else if( wTmpClearMeterType == eCLEAR_EVENT )//����5. �¼���ʼ�� �����¼������¼�������¼ �����¼������б� �ݽ��¼�������䵽�����������ȥ
	{
		api_ClrAllEvent(eEXCEPT_CLEAR_METER_CLEAR_EVENT);
	}	
	else if( wTmpClearMeterType == eCLEAR_DEMAND )//����6. ������ʼ�� ����ǰ �������� ��
	{
	
		#if( MAX_PHASE == 3 )
		#if( SEL_DEMAND_2022 == NO )
		api_ClrDemand(eClearAllDemand);
		#else
		api_ClrDemand(eClearAllDemand,eDemandPause);
		#endif
		#endif
	}
	
	wTmpClearMeterType = 0;
		
	api_WritePara(0, GET_STRUCT_ADDR(TFPara1, wClearMeterType), sizeof(WORD), (BYTE *)&wTmpClearMeterType );
}

//--------------------------------------------------
//��������:    ����ʱ����ת��������
//         
//����:      DWORD AbsMin[in]	            //��ʼ������
//         
//         BYTE TIIndex[in]			    //TI�ļ��
//         
//         WORD TI[in]				    //���ֵ
//         
//����ֵ:     DWORD ����ʱ�������
//         
//��ע����:    �˴�TIIndexδ�жϱ���С��6 ʹ��ʱ��ر�֤С��6���������
//--------------------------------------------------
static DWORD TimeIntervalToAbsMin( DWORD AbsMin, BYTE TIIndex, WORD TI )
{
	TRealTimer  TmpRealTimer ;
	WORD Result;
	DWORD AbsTime698;
	
	AbsTime698 = 0;
	
	if( TIIndex == 0 )	//ʱ�������룬��ֱ�ӷ��ط����������᲻�����붳��
	{
		return AbsMin+(TI/60);
	}
	else if( TIIndex < 4)	//ʱ������ �� ʱ �� ʱ �������ݷ���ʱ����
	{
		return (AbsMin+TIMinutes[TIIndex]*TI);
	}
	else if( TIIndex  == 4) //ʱ����Ϊ��
	{
		Result = api_ToAbsTime( AbsMin, &TmpRealTimer);//������ʱ��ת��Ϊ���ʱ��
		if( Result == TRUE )
		{
			if( (TmpRealTimer.Mon + TI) > 12 )//����Ҫ��λ
			{
				TmpRealTimer.wYear += (TmpRealTimer.Mon + TI)/12;//����������£�˳���ܸ���
				TmpRealTimer.Mon = (TmpRealTimer.Mon + TI)%12;
			
			}
			else
			{
				TmpRealTimer.Mon = TmpRealTimer.Mon + TI;
			}
			
			AbsTime698 = api_CalcInTimeRelativeMin( &TmpRealTimer );//ת��Ϊ����ʱ��
		}
	}
	else if( TIIndex == 5 ) //ʱ����Ϊ��
	{
		Result = api_ToAbsTime( AbsMin, &TmpRealTimer ); //������ʱ��ת��Ϊ���ʱ��
		if( Result == TRUE )
		{
			TmpRealTimer.wYear = TmpRealTimer.wYear + TI;
			AbsTime698 = api_CalcInTimeRelativeMin( &TmpRealTimer );//ת��Ϊ����ʱ��
		}
	}
	else
	{
        AbsTime698 = 0;
	}

	return AbsTime698;
}

//--------------------------------------------------
//��������:    ����ʱ����ת��������
//         
//����:      DWORD AbsMin[in]	            //��ʼ������
//         
//         BYTE TIIndex[in]			    //TI�ļ��
//         
//         WORD TI[in]				    //���ֵ
//         
//����ֵ:     DWORD ����ʱ�������
//         
//��ע����:    �˴�TIIndexδ�жϱ���С��6 ʹ��ʱ��ر�֤С��6���������
//--------------------------------------------------
static DWORD TimeIntervalToAbsSec( DWORD AbsSec, BYTE TIIndex, WORD TI )
{
	TRealTimer  TmpRealTimer ;
	WORD Result;
	DWORD AbsTime698,AbsMin,Sec;
	
	AbsTime698 = 0;
	AbsMin = (AbsSec/60);	//��ȡ��Է�����
	Sec = (AbsSec%60);		//��ȡ����
	
	if( TIIndex == 0 )	//ʱ�������룬��ֱ�ӷ��ط����������᲻�����붳��
	{
		return AbsSec+TI;
	}
	else if( TIIndex < 4)	//ʱ������ �� ʱ �� ʱ �������ݷ���ʱ����
	{
		return (AbsSec+(TIMinutes[TIIndex]*TI)*60);
	}
	else if( TIIndex == 4) //ʱ����Ϊ��
	{
		Result = api_ToAbsTime( AbsMin, &TmpRealTimer);//������ʱ��ת��Ϊ���ʱ��
		if( Result == TRUE )
		{
			if( (TmpRealTimer.Mon + TI) > 12 )//����Ҫ��λ
			{
				TmpRealTimer.wYear += (TmpRealTimer.Mon + TI - 1) / 12;//����������£�˳���ܸ���
				TmpRealTimer.Mon = (TmpRealTimer.Mon + TI - 1) % 12 + 1;
				if( TmpRealTimer.wYear > 2099 )//����ֵ�жϣ���������99��
				{
					TmpRealTimer.wYear = 2099;
				}
			
			}
			else
			{
				TmpRealTimer.Mon = TmpRealTimer.Mon + TI;
			}
			TmpRealTimer.Sec =Sec;
			AbsTime698 = api_CalcInTimeRelativeSec( &TmpRealTimer );//ת��Ϊ����ʱ��
		}
	}
	else if( TIIndex == 5 ) //ʱ����Ϊ��
	{
		Result = api_ToAbsTime( AbsMin, &TmpRealTimer ); //������ʱ��ת��Ϊ���ʱ��
		if( Result == TRUE )
		{
			TmpRealTimer.wYear = TmpRealTimer.wYear + TI;
			if( TmpRealTimer.wYear > 2099 )//����ֵ�жϣ���������99��
			{
				TmpRealTimer.wYear = 2099;
			}
			
			TmpRealTimer.Sec =Sec;
			AbsTime698 = api_CalcInTimeRelativeSec( &TmpRealTimer );//ת��Ϊ����ʱ��
		}
	}
	else
	{
        AbsTime698 = 0;
	}

	return AbsTime698;
}

//--------------------------------------------------
//��������:       �жϹ㲥Уʱ�Ƿ��������
//         
//����  :       AbsSetSecTime:�㲥Уʱ�����ʱ��
//
//����ֵ:        TRUE
//			  FALSE
//��ע����: 
//--------------------------------------------------
BOOL JudgeBroadcastTime( DWORD AbsSetSecTime)
{
	BYTE i;
	DWORD RealSecTime,CheckSecTime;
	WORD Result;
	TBillingPara BillingPara;
	TRealTimer CheckTimer;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	TLadderBillingPara LadderBillingPara;//��ǰ���ݵ�۱�

	api_GetCurLadderYearBillInfo( &LadderBillingPara );
	#endif
	
	Result = ReadSettlementDate((BYTE *)&BillingPara);	//������
	if( Result == FALSE )
	{
		return TRUE;
	}
	
	for(i = 0;i < MAX_MON_CLOSING_NUM;i++)
	{
		if( 	( BillingPara.MonSavePara[i].Day <= 28)
			&&	( BillingPara.MonSavePara[i].Hour <= 23 ) )
		{
			CheckTimer.Sec = 0;
			CheckTimer.Min = 0;
			CheckTimer.Day = BillingPara.MonSavePara[i].Day;
			CheckTimer.Hour = BillingPara.MonSavePara[i].Hour;
			CheckTimer.Mon = RealTimer.Mon;
			CheckTimer.wYear = RealTimer.wYear;
			RealSecTime = api_CalcInTimeRelativeSec( &RealTimer );
			CheckSecTime = api_CalcInTimeRelativeSec(&CheckTimer);
			//�������жϷ�ʽ�����ܲ��÷���,������̫��
			if((AbsSetSecTime>RealSecTime))
			{
				if (  (CheckSecTime>RealSecTime)
					&&(CheckSecTime<=AbsSetSecTime))
				{
					return FALSE;
				}
			}
			else
			{
				if (  (CheckSecTime>AbsSetSecTime)
					&&(CheckSecTime<=RealSecTime))
				{
					return FALSE;
				}
			}
		}
	}


	#if( PREPAY_MODE == PREPAY_LOCAL )
	for(i=0;i<MAX_YEAR_BILL;i++)
	{
		if(  ( LadderBillingPara.LadderSavePara[i].Day <= 28 )
		&&	( LadderBillingPara.LadderSavePara[i].Hour <= 23 ) )
		{
			CheckTimer.Sec = 0;
			CheckTimer.Min = 0;
			CheckTimer.Day = LadderBillingPara.LadderSavePara[i].Day;
			CheckTimer.Hour = LadderBillingPara.LadderSavePara[i].Hour;
			
			if( LadderBillingPara.LadderSavePara[i].Mon > 12 )//�Ƿ�ֵ---�½���
			{
				CheckTimer.Mon = RealTimer.Mon;
			}
			else
			{
				CheckTimer.Mon = LadderBillingPara.LadderSavePara[i].Mon;
			}

			CheckTimer.wYear = RealTimer.wYear;
			RealSecTime = api_CalcInTimeRelativeSec( &RealTimer );
			CheckSecTime = api_CalcInTimeRelativeSec(&CheckTimer);
			//�������жϷ�ʽ�����ܲ��÷���,������̫��
			if((AbsSetSecTime>RealSecTime))
			{
				if (  (CheckSecTime>RealSecTime)
					&&(CheckSecTime<=AbsSetSecTime))
				{
					return FALSE;
				}
			}
			else
			{
				if (  (CheckSecTime>AbsSetSecTime)
					&&(CheckSecTime<=RealSecTime))
				{
					return FALSE;
				}
			}
			
			if( (i == 0) && (LadderBillingPara.LadderSavePara[i].Mon > 12) )
			{
				break;
			}
			
		}
	}
	#endif

	return TRUE;
}


//--------------------------------------------------
//��������:       �жϹ㲥Уʱ�Ƿ����ڽ���ʱ��ǰ��5����
//         
//����  :       ��
//
//����ֵ:        TRUE--����5min    
//			  FALSE--5min����
//��ע����: 
//--------------------------------------------------
BOOL JudgeBroadcastMeterTime( WORD Sec )
{
	BYTE i;
	DWORD TempSecCheck;
	WORD Result;
	TBillingPara BillingPara;
	TRealTimer CheckTimer;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	TLadderBillingPara LadderBillingPara;//��ǰ���ݵ�۱�

	api_GetCurLadderYearBillInfo( &LadderBillingPara );
	#endif
	
	Result = ReadSettlementDate((BYTE *)&BillingPara);	//������
	if( Result == FALSE )
	{
		return TRUE;
	}
	
	for(i = 0;i < MAX_MON_CLOSING_NUM;i++)
	{
		if( 	( BillingPara.MonSavePara[i].Day <= 28)
			&&	( BillingPara.MonSavePara[i].Hour <= 23 ) )
		{
			CheckTimer.Sec = 0;
			CheckTimer.Min = 0;
			CheckTimer.Day = BillingPara.MonSavePara[i].Day;
			CheckTimer.Hour = BillingPara.MonSavePara[i].Hour;
			CheckTimer.Mon = RealTimer.Mon;
			CheckTimer.wYear = RealTimer.wYear;
			TempSecCheck = api_CalcInTimeRelativeSec( &RealTimer );
			//�������жϷ�ʽ�����ܲ��÷���,������̫��
			if( labs(TempSecCheck - api_CalcInTimeRelativeSec(&CheckTimer)) < Sec)
			{
				return FALSE;
			}
		}
	}


	#if( PREPAY_MODE == PREPAY_LOCAL )
	for(i=0;i<MAX_YEAR_BILL;i++)
	{
		if(  ( LadderBillingPara.LadderSavePara[i].Day <= 28 )
		&&	( LadderBillingPara.LadderSavePara[i].Hour <= 23 ) )
		{
			CheckTimer.Sec = 0;
			CheckTimer.Min = 0;
			CheckTimer.Day = LadderBillingPara.LadderSavePara[i].Day;
			CheckTimer.Hour = LadderBillingPara.LadderSavePara[i].Hour;
			
			if( LadderBillingPara.LadderSavePara[i].Mon > 12 )//�Ƿ�ֵ---�½���
			{
				CheckTimer.Mon = RealTimer.Mon;
			}
			else
			{
				CheckTimer.Mon = LadderBillingPara.LadderSavePara[i].Mon;
			}

			CheckTimer.wYear = RealTimer.wYear;
			TempSecCheck = api_CalcInTimeRelativeSec( &RealTimer );
			//�������жϷ�ʽ�����ܲ��÷���,������̫��
			if( labs(TempSecCheck - api_CalcInTimeRelativeSec(&CheckTimer)) < Sec )
			{
				return FALSE;
			}
			
			if( (i == 0) && (LadderBillingPara.LadderSavePara[i].Mon > 12) )
			{
				break;
			}
			
		}
	}
	#endif

	return TRUE;
}

//--------------------------------------------------
//��������:  �ж���698Ϊ���ļܹ������ʱ�α����Ƿ����15����
//         
//����  :   BYTE *pBuf[in] ʱ�α�BUFָ��
//			pBuf[0]--��ǰʱ�ε�ʱ pBuf[1]--��ǰʱ�εķ� pBuf[2]--��ǰʱ�εķ��ʺ�
//			pBuf[3]--��һʱ�ε�ʱ pBuf[4]--��һʱ�εķ� pBuf[5]--��һʱ�εķ��ʺ�
//
//
//����ֵ:    WORD TRUE ��Ч  FALSE ��Ч 
//
//��ע����:  ��698Ϊ���ļܹ������ʱ�δ���˳��:pBuf[0]--ʱ��ʱ pBuf[1]--ʱ�η� pBuf[2]--ʱ�η��ʺţ�˫Э�����645���ñ�����ǰҲ���밴��˳��
//			 ע����ǰ��645Ϊ���ļܹ������ʱ�δ���˳�����ú�698˳���෴���˺�������ǰ��645Ϊ���ļܹ��������˳��ͬ
//			 ����698Ϊ���ļܹ������ʱ������˳��:pBuf[0]--ʱ���� pBuf[1]--ʱ���� pBuf[2]--ʱ�α�ţ�Ҳ����ǰ��645Ϊ���ļܹ��������˳��ͬ
//--------------------------------------------------
WORD api_JudgeTimeSegInterval(BYTE *pBuf)
{
	WORD wIntervalRightValue;
	int iInterval;
	
	wIntervalRightValue = 15;//ʱ�μ������С��15����

	iInterval = (pBuf[3]-pBuf[0]);//��һʱ�κ͵�ǰ���ʱ�β��Сʱ

	iInterval *= 60;//��һʱ�κ͵�ǰ���ʱ�μ��ת��Ϊ�Է���Ϊ��λ

	iInterval = (iInterval + pBuf[4]-pBuf[1]) ;//ʱ�μ��������һʱ�κ͵�ǰ���ʱ�εķ��Ӳ�

	#if( MAX_PHASE != 1)
	//�¹���Ҫ����Сʱ�β���С����������
	wIntervalRightValue = (FPara2->EnereyDemandMode.DemandPeriod);
	if( wIntervalRightValue < 15 )
	{
		wIntervalRightValue = 15;
	}
	#endif

	//���ʱ�μ��Ϊ0������Ϊ�ǲ���ģ�Ҫ����һʱ�κ͵�ǰ���ʱ�εķ���Ҳһ��
	if( ( iInterval == 0 )&&(pBuf[2] == pBuf[5] ) )
	{
		return TRUE;
	}
	else if( iInterval >= wIntervalRightValue )//�˴�int��WORD�Ƚϣ��Ƚ�WORDת��int�ٱȽϡ�����ط��ȽϷ������Ƽ�
	{
		return TRUE;
	}
	else//������� < 15����� iInterval < 0 �������ܵ�һʱ��10�㣬�ڶ���ʱ��9�㣩
	{
		return FALSE;
	}
}

//--------------------------------------------------
//��������:    ����Ӧ������ʱЧ�ж�
//         
//����:      BYTE Ch[in]		        //ͨ��ѡ��
//         eRequestMode eType[in]	//0x00: ����SET  0x55: ����ACTION
//
//����ֵ:     BOOL  TRUE ��Ч  FALSE ��Ч
//         
//��ע����:  ��
//--------------------------------------------------
static BOOL JudgeConnectValid( BYTE Ch,eRequestMode eType )
{
    //if( Ch == eIR )
    //{
    //    if( api_GetSysStatus( eSYS_IR_ALLOW_PRG ) != TRUE )//������֤��ͨ�����ɽ��к�������
    //    {
    //        return FALSE;
    //    }
    //}
		
	return TRUE;
}
//--------------------------------------------------
//��������:����˿�Ȩ���ж�
//         
//����:      BYTE Ch[in]		        //ͨ��ѡ��
//         eRequestMode eType[in]	//����ģʽ
//         WORD OI                  //��Ҫ�жϵ�OI
//����ֵ:     BOOL  TRUE ��Ч  FALSE ��Ч //FALSE����ʧ�ܣ�������Ҫ�������к������ж�
//         
//��ע����:  ��
//--------------------------------------------------
static BOOL JudgeIRAuthority( BYTE Ch,eRequestMode eType,BYTE* pOMD )
{
    TFourByteUnion OMD;

    lib_ExchangeData( OMD.b, pOMD, 4 );//��ȡOMD 
    if( eType == eACTION_MODE )//֧������ͨ�� ������������֤��������Ӧ�����Ӳ���
    {
        //������������֤��������Ӧ�����Ӳ���
        //����8λ ���ж�OMD�Ĳ���ģʽ
        if( ((OMD.d >>8) == 0X00F1000B) ||((OMD.d >>8) == 0X00F1000C) )
        {
            return TRUE;
        }
    }
    /*if( Ch == eIR )
    {
        if( eType == eREAD_MODE )
        {
            if( api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE )//������Ӧ������֧�ֶ�ȡ��������
            {
                if(    (OMD.w[1] == 0x4002) //���
                    || (OMD.w[1] == 0x4001) //ͨ�ŵ�ַ
                    || (OMD.w[1] == 0x4111) //������
                    || (OMD.w[1] == 0x4000) //��ǰ����ʱ��
                    || (OMD.w[1] == 0x202C))//��ǰǮ���ļ�
                 {
                    return TRUE;
                 }

                if( (OMD.b[3] >= 0x00) && (OMD.b[3] <= 0x05) ) //֧�ֶ�ȡ���е���
                {
                    return TRUE;
                }
            }
            
        }
    }*/
   
    return FALSE;
}

//--------------------------------------------------
//��������:    ��ȫģʽ�����ж�
//         
//����:      BYTE Ch[in]			//ͨ��ѡ��
//         eRequestMode eType	//����ʽ
//         BYTE *pOI[in] 		//OIָ��
//         
//����ֵ:     WORD  TRUE ��Ч   FALSE ��Ч
//         
//��ע����:    ��
//--------------------------------------------------
static WORD JudgeSafeModePara(BYTE Ch,eRequestMode eType , BYTE *pOI)
{
	BYTE i, OIA;
	BYTE Cmd, SecurityMode;
	WORD wSafeMode;
	TTwoByteUnion OI;

	if( eType == eREAD_MODE )//��������Ĳ������ͽ����ж�cmd�����actionthengetȨ���жϲ��Ե�����2
	{
		Cmd = 0x05;
	}
	else if( eType == eSET_MODE )
	{
		Cmd = 0x06;
	}
	else if( eType == eACTION_MODE )
	{
		Cmd = 0x07;
	}
	else
	{
		Cmd = LinkData[Ch].pAPP[0];
	}
	
	SecurityMode = APPData[Ch].eSafeMode;
	lib_ExchangeData( OI.b, pOI, 2);
	
	//�ж���ʾ��ȫģʽ����	
	for( i=0; i<SafeModePara.Num; i++ )
	{
		if( OI.w == SafeModePara.Sub[i].OI )
		{
			//���OI����ʾ��ȫģʽ�����У����ж�Ȩ���Ƿ�һ��
			switch(Cmd)
			{
			case 0x05://DealGet_Request
			case 0x06://DealSet_Request
			case 0x07://DealAction_Request
				wSafeMode = SafeModePara.Sub[i].wSafeMode;
				wSafeMode = ((wSafeMode>>((8-Cmd)*4))&0x0f);
				//����safemode��bit3~bit0:���ķ�ʽ����	����+������֤�����	���ķ�ʽ����	����+������֤�����
				if( (wSafeMode==0) || (wSafeMode==0xf) )//��ȫģʽͬʱ�������ġ�����+MAC�����ġ�����+MAC���֣�����Ϊ��ȫģʽ�ң����ж�
				{
					return TRUE;
				}
				
				if( SecurityMode == eNO_SECURITY )
				{
					if( wSafeMode & 0x8 )
					{
						return TRUE;
					}	
				}		
				else if( (SecurityMode==eEXPRESS_MAC) || (SecurityMode==eSECURITY_RN) )
				{
					if( Ch != eIR )//����ͨ�������������֤���ñ�־���ж�
					{
						if( (api_GetRunHardFlag(eRUN_HARD_ESAM_ERR) == FALSE)&&(Cmd == 0x05)&&(wSafeMode&0x04)&&(api_GetRunHardFlag( eRUN_HARD_EN_IDENTAUTH ) == TRUE) )
						{
							//ESAM����״̬�£�GET�İ�ȫģʽΪ����+������֤�룬���������֤���ã���Ҫ����Ӧ�����ӻ����ն������֤��������
							if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL )
							{
								if( (api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)&&(api_GetSysStatus( eSYS_TERMINAL_ALLOW_PRG ) == FALSE) )
								{
									return FALSE;
								}
							}
							else if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )
							{
								return FALSE;
							}
							
							return TRUE;
						}
					}

					if( wSafeMode&0xc ) //���ġ�����+MAC
					{
						return TRUE;
					}	
				}
				else if( SecurityMode == eSECRET_TEXT )
				{
					if( wSafeMode & 0xe )//���ġ�����+MAC������
					{
						return TRUE;
					}
				}
				else if(  SecurityMode == eSECRET_MAC )
				{
					//if( wSafeMode & 0xf )
					{
						return TRUE;
					}	
				}	
				break;	
			case 0x10://DealSecurity_Request
				return TRUE;
				break;		
			default://��������
				return TRUE;
				break;
			}
			return FALSE;
		}	
	}
	
	OIA = (OI.w/0x100);
	
	//�ж�Ĭ�ϰ�ȫģʽ����,
	switch(Cmd)
	{
	case 0x05://DealGet_Request
		//if( wResult = );
		if( (OI.w/0x1000) == 0 )//����Ĭ�����Ŀɶ�
		{
			return TRUE;
		}
		if( OI.w == 0x202c )//��ǰǮ���ļ�
		{
			return TRUE;
		}
	
		//4000	����ʱ��	�� 4001	ͨ�ŵ�ַ	�� 4002	���	��
		if( (OI.w >= 0x4000)&&(OI.w <= 0x4002) )
		{
			return TRUE;
		}
		if (OI.w == 0xF221)
		{
			return TRUE;
		}
		if( (OI.w >= 0x6000)&&(OI.w <= 0x60FF) )
		{
			return TRUE;
		}
		/*
		F000	��֡����	��
		F001	�ֿ鴫��	��
		F002	��չ����	��
		F100	ESAM	��
		F101	��ȫģʽ����	��
		F2ZZ	��������ӿ��豸	��
		FFZZ	�Զ���	��
		*/
		if( OI.w == 0x4111 )//������
		{
			return TRUE;
		}

		if( (OIA == 0xf0)||(OIA == 0xf1)||(OIA == 0xf2)||(OIA == 0xff) )
		{
			return TRUE;
		}
		//���������Ŀɶ��⣬�������ݶ���Ȩ�޴��ڵ�������+MAC�ɶ���
		//��ȡȨ���ɵ͵��ߣ����� ����+MAC ���� ����+MAC	��������ʱ�������÷�ʽ������ָ���ģ�������ֻ��������+MAC���ã�������+MAC��ʽ���ñ��ESAM���ܲ�֧�����õ�ESAM��ȥ
		if( Ch != eIR )//����ͨ�������������֤���ñ�־���ж�
		{
			if( (api_GetRunHardFlag(eRUN_HARD_ESAM_ERR) == FALSE)&&(SecurityMode == eSECURITY_RN )&&(api_GetRunHardFlag( eRUN_HARD_EN_IDENTAUTH ) == TRUE))
			{
				if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL)
				{
					if( (api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)&&(api_GetSysStatus( eSYS_TERMINAL_ALLOW_PRG ) == FALSE) )
					{
						return FALSE;
					}
				}
				else if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )
				{
					return FALSE;
				}
			}
		}

		if( SecurityMode >= eSECURITY_RN )
		{
			return TRUE;
		}
		break;	
	case 0x06://DealSet_Request
		//4002	���, 4003 �ͻ����, F100 ESAM Ĭ�����÷�ʽ������+MAC 
		if( (OI.w == 0x4002)||(OI.w == 0x4003)||(OI.w == 0xf100) )
		{
			if( SecurityMode >= eEXPRESS_MAC )
			{
				return TRUE;
			}
		}
		else if( (OIA == 0xf2)||(OIA == 0xff) )//F2ZZ	��������ӿ��豸	�� 	FFZZ	�Զ���
		{
			return TRUE;
		}
		else if( (OI.w >= 0x6000)&&(OI.w <= 0x60FF) )
		{
			return TRUE;
		}
		//�������ݶ���Ȩ�޵�������+MAC������	
		if( SecurityMode == eSECRET_MAC )
		{
			return TRUE;
		}		
		break;	
	case 0x07://DealAction_Request
		if( (OI.w == 0x4000) || (OI.w == 0x2015) )//4000 ����ʱ�� 2015 �����ϱ�״̬��
		{
			return TRUE;
		}
		else if( OI.w == 0x5000 )//˲ʱ���ᣬ����+MAC
		{
			if( SecurityMode >= eEXPRESS_MAC )
			{
				return TRUE;
			}
		}
        // else if( OI.w == 0xf001 )//F001	�ļ��ֿ鴫��
		// {
		// 	return TRUE;
		// }
		else if( OI.w == 0xf002 )//F002	��չ����
		{
			return TRUE;
		}
		else if( (OIA >= 0x20) && (OIA <= 0x2f) )//2ZZZ	���������
		{
		    if( SecurityMode >= eEXPRESS_MAC )
			{
				return TRUE;
			}
		}
		else if( (OIA == 0xf2)||(OIA == 0xff) )//F2ZZ	��������ӿ��豸	�� 	FFZZ	�Զ���
		{
			return TRUE;
		}
		else if( (OI.w == 0xf001)||(OI.w == 0xf101) )//������ʽ������ ���� F001	�ֿ鴫�� F101	��ȫģʽ���� 
		{
			if( SecurityMode >= eSECRET_TEXT )
			{
				return TRUE;
			}
		}
		else if( (OI.w >= 0x6000)&&(OI.w <= 0x60FF) )
		{
			return TRUE;
		}
		else if(OI.w ==0xfefe )
		{
			return TRUE;
		}
		//���Ȩ�޵�������+MAC���ɲ���	
        if( SecurityMode == eSECRET_MAC )
        {
          return TRUE;
        }
		break;	
	//case 0x10://DealSecurity_Request
	//	break;		
	default:
		break;
	}
	
	return FALSE;
}

//--------------------------------------------------
//��������:  ����Ӧ������Ȩ���ж�
//         
//����:     BYTE Ch[in]		        //ͨ��ѡ��
//        eRequestMode eType[in]	//0x00: ����SET  0x55: ����ACTION
//        BYTE* pOI                 //OIָ��
//
//����ֵ:    WORD  TRUE ��Ч  FALSE ��Ч
//         
//��ע����:  ��
//--------------------------------------------------
WORD JudgeTaskAuthority( BYTE Ch, eRequestMode eType, BYTE *pOI )
{
	TTwoByteUnion OI;
	WORD Result;
	BYTE ClassAttribute;
	
	if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ) 
	{
		return TRUE;
	}

	//if( (CURR_COMM_TYPE( Ch ) == COMM_TYPE_TERMINAL)&&(Ch == eIR) )
	//{
	//	//eIR�Ŀͻ�����ַֻ������վ��ַ
	//	return FALSE;
	//}
	
	lib_ExchangeData( OI.b, pOI, 2 ); //��ȡOI
	ClassAttribute = pOI[2];	  //��ȡ����
		
	//����ʱ��(�㲥Уʱ)�������๦�ܶ��ӡ�������ʱ������Բ�����Ӧ������ֱ�Ӳ���
	//������֤��֧��
	if( (eType==eACTION_MODE) && (LinkData[Ch].AddressType != eWILDCARD_ADDRESS_TYPE) )
	{
		if(		(OI.w == 0X4000)//����ʱ�� 
			|| 	(( OI.w & 0xFF00) == 0xF200)//�๦�ܶ��� 
			//|| 	(( OI.w & 0xFF00) == 0xFE00) 
			|| 	(( OI.w & 0xFF00) == 0x6000)//��������
			|| 	( (OI.w==0x5000) && (ClassAttribute==3) )//�㲥����
			|| 	( (OI.w==0xF300) && ((ClassAttribute==5)||(ClassAttribute==6)) ) //��ʾ�鿴��ȫ������
			|| 	( (OI.w==0xF301) && ((ClassAttribute==5)||(ClassAttribute==6)) )//��ʾ�鿴��ȫ������
            || 	( (OI.w==0xF001) && (ClassAttribute==8))) 
		{
		    //if( (Ch == eIR) && (api_GetSysStatus( eSYS_IR_ALLOW_PRG ) != TRUE) )//���ⲻ���к�����֤��֧�ֲ���
    		//{
            //    return FALSE;
    		//}
            if( (OI.w == 0X4000) 
              && (APPData[Ch].eSafeMode != eNO_SECURITY) 
              && (api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE) 
            )
            {
                //���ܱ��ڹ���ʱ��������ʹ�ð�ȫģʽ�㲥Уʱ
                return FALSE;
            }
    		
			return TRUE;
		}
	}

    if( (eType != eREAD_MODE) && (LinkData[Ch].AddressType == eSINGLE_ADDRESS_TYPE) )//���ĺ�բֻ֧�ֵ���ַ
    {
        if( eType == eACTION_MODE ) 
        {
            if( (OI.w==0x8000) && (ClassAttribute==131) ) //���ĺ�բ ֧������
            {
                return TRUE;
            }
            else if( (OI.w==0x2015) && (ClassAttribute==127) )//ȷ�ϸ����ϱ�״̬��
            {
                //if( (Ch == eIR) && (api_GetSysStatus( eSYS_IR_ALLOW_PRG ) != TRUE) )//���ⲻ���к�����֤��֧�ֲ���
                //{
                //    return FALSE;
               // }
                
                return TRUE;
            }
            else{} 
        }
        else
        {
            if( (OI.w==0x4500) || (OI.w==0x4501) )//���ù���ͨ��ģ��1��2
            {
                if( Ch == eCR )
                {
                    return TRUE;
                }
            }
        }

    }
    
    if( (api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE)//�����������������
    && (eType!=eREAD_MODE) )
    {
		return FALSE;
    }
    
	//������ַ��ֻ��������� ����������˳���ܵߵ�!!!!!!
	if( (LinkData[Ch].AddressType!=eSINGLE_ADDRESS_TYPE) && (eType!=eREAD_MODE) )
	{
		return FALSE;
	}
	
	Result = JudgeIRAuthority( Ch, eType, pOI );
	if( Result == TRUE )//ʧ�ܴ����������� ��Ҫ�������к����ж�
	{
        return TRUE;
	}
	
	Result = JudgeConnectValid( Ch, eType );
	if( Result  == FALSE )
	{
		return FALSE;
	}
#if((SEL_JUDGE_SAFE_MODE == YES) &&(SEL_ESAM_TYPE != 0))
	Result = JudgeSafeModePara( Ch, eType ,pOI);

	return Result;
#else
	return TRUE;
#endif
}

//--------------------------------------------------
//��������:  �ж�ʱ���ǩ����
//         
//����:     BYTE Ch[in]
//         
//����ֵ:   TRUE ��Ч  FALSE ��Ч  
//         
//��ע����:  ��
//--------------------------------------------------
BOOL JudgeTimeTagValid(BYTE Ch)
{
	DWORD dwTime,dwRealTime;//unsigned long int
	BYTE TIIndex;
	WORD wTI,Len;
	BYTE *pBuf;
	TRealTimer TimeTag,tmpRealTimer;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return FALSE;
    }

	Len = AnalyseDlt698ApduLen( Ch );
	if( Len == 0x8000 )
	{
       return FALSE;
	}
	
	pBuf = (BYTE *)&(LinkData[Ch].pAPP[Len]);
	LinkData[Ch].pAPPLen.w -= 1;//�޳�OPTIONAL 1���ֽ�

	if( Len > LinkData[Ch].pAPPLen.w )//��ֹ�ֽ����������޵���ʱ���ǩ����
	{
	    APPData[Ch].TimeTagFlag = eNO_TimeTag;//Ĭ����ʱ���ǩ
        return TRUE;
	}
	
	if( pBuf[0] != 0x01 )//OPTINAL Ϊ0 ������ʱ���ǩ
	{
		APPData[Ch].TimeTagFlag = eNO_TimeTag;//Ĭ����ʱ���ǩ
	}
	else
	{
	    memcpy( g_TimeTagBuf, pBuf+1, 10 );//����ʱ���ǩ
	    
	    if( (LinkData[Ch].pAPPLen.w -Len) < 10 )//ʣ���ֽڲ���ʱ���ǩ����
	    {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//ʱ���ǩ��Ч
            return TRUE;
	    }
	    
	    LinkData[Ch].pAPPLen.w -= (LinkData[Ch].pAPPLen.w -Len);//Ӧ�ò㳤�� �޳�ʱ���ǩ
       
        APPData[Ch].TimeTagFlag = eTime_Invalid;//ʱ���ǩ��Ч
        
        memcpy( (BYTE*)&TimeTag.wYear, (BYTE*)&pBuf[1],7);
        lib_InverseData((BYTE*)&TimeTag.wYear, 2);
        
        if( api_GetRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW ) == TRUE )//��ص�ѹ��Ĭ��ʱ���ǩ��Ч
        {
            APPData[Ch].TimeTagFlag = eTime_True;//ʱ���ǩ��Ч
            return TRUE;
        }
        
        if( api_CheckClock( (TRealTimer*)&TimeTag ) == FALSE )//ʱ���ʽ����
        {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//ʱ���ǩ��Ч
            return TRUE;
        }
        
        TIIndex = pBuf[1+7];
        if( TIIndex > 5 )
        {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//ʱ���ǩ��Ч
            return TRUE;
        }
        
        lib_ExchangeData((BYTE*)&wTI, (BYTE*)&pBuf[1+7+1], 2);
        
        api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&tmpRealTimer );
		dwRealTime = api_CalcInTimeRelativeSec(&tmpRealTimer );//��ȡ��ǰʱ��������������߾���
        if( dwRealTime == ILLEGAL_VALUE_8F )//��ǰʱ����������Ĭ��ʱ����Ч
        {
            APPData[Ch].TimeTagFlag = eTime_True;//ʱ����Ч;
            return TRUE;
        } 
        
        dwTime = api_CalcInTimeRelativeSec(&TimeTag);//��ȡ�·�ʱ�������������߾���
        if( dwTime == ILLEGAL_VALUE_8F )//CalcTime �ڵ��� api_CheckClock ûͨ��
        {
            APPData[Ch].TimeTagFlag = eTime_Invalid;//ʱ����Ч;
            return TRUE;
        }   
        if( dwTime >= dwRealTime )
        {
            APPData[Ch].TimeTagFlag = eTime_True;//��ʱ���ǩ;
            return TRUE;
        }
        
        dwTime = TimeIntervalToAbsSec( dwTime, TIIndex, wTI );
        
        if( dwTime >= dwRealTime )
        {
            APPData[Ch].TimeTagFlag = eTime_True;//��ʱ���ǩ;
            return TRUE;
        }
	}
	
	return TRUE;
}

//--------------------------------------------------
//��������:  698һ�������ж�
//         
//����:     BYTE* pBuf[in]
//         
//����ֵ:    BOOL  TRUE ��Ч  FALSE ��Ч   
//         
//��ע����:  ��
//--------------------------------------------------
static BOOL Judge698PassWord( BYTE *pBuf )
{
	TMuchPassword  pPwd;
	BYTE i;
	
	//�ǳ���ģʽ������������򷵻�ʧ��	
	if( 	( api_GetRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS) == TRUE ) 
		&& 	( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE ) )
	{
        return FALSE;
	}
	
	//����EEPROM�б�������� ���ܶ����Ƿ�ɹ����涼д������ֻ��һ��д���ɹ�
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)pPwd.Password645 ) != TRUE )
	{
		return TRUE;
	}	

	for( i=0; i< MAX_698PASSWORD_LENGTH; i++ )
	{
		if( pBuf[i] != pPwd.Password698[i] )
		{
		    g_PassWordErrCounter ++;
		    if( g_PassWordErrCounter >= 3 )
		    {
                api_SetRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);
		    }
			return FALSE;
		}
	}
	
    g_PassWordErrCounter = 0;

	return TRUE;
}

//--------------------------------------------------
//��������:    �жϼ�¼OAD�ĺϷ���
//         
//����:      WORD OI[in]
//         
//         BYTE ClassAttribute[in]
//         
//����ֵ:     BOOL  TRUE ��Ч  FALSE ��Ч   
//         
//��ע����:    ��
//--------------------------------------------------
static BOOL JudgeRecordOAD( WORD OI,BYTE ClassAttribute )
{
	//������Դ�����¼��ں궨��δ��ʱ���ܽ��ж�ȡ
	if( (SelSecPowerConst==NO) && (OI==0x300e) )
	{
		return FALSE;
	}
	
	//�ӿ���24
	if( ((OI>=0x3000)&&(OI<=0x3008)) || (OI==0x300b) || (OI==0x303B) ) 
	{
		if( 	( ClassAttribute != 6 ) 
			&& 	( ClassAttribute != 7 ) 
			&& 	( ClassAttribute != 8 ) 
			&& 	( ClassAttribute != 9 ) )
		{
			return FALSE;
		}
	}
	else if( ClassAttribute != 2 )
	{
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------
//��������:    �ж���֡���� ȥ�����ⲻ����֡�ĳ���
//         
//����:      BYTE Ch[in]
//         
//����ֵ:     BOOL  TRUE ��Ч  FALSE ��Ч   
//         
//��ע����:    ��
//--------------------------------------------------
static void JudgeAppBufLen( BYTE Ch )
{
    if( APPData[Ch].APPCurrentBufLen.w > APPData[Ch].APPBufLen.w )//ǿ��ʹ���ݳ�����ȣ���Ӱ��������ݴ���
    {
        //ASSERT(FALSE, 0);
        APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
    }
    else
    {
	}
}

//--------------------------------------------------
//��������:  	���Э��һ���Ժ͹���һ����BUF
//         
//����  : 	BYTE Mode    0: ����ȫ��     1������Э��һ����         2�����ù���һ����
//
//          BYTE *pBuf[in] ����Buf
//
//����ֵ:    	WORD ���ݳ���
//         
//��ע����:     ����buf���Ⱥ���δ�����жϣ���ر�֤����BUF���ȳ���24���ֽڣ����ⳬ��
//--------------------------------------------------
WORD AddProtocolAndFunctionConformance( BYTE Mode,BYTE *pBuf )
{
	BYTE Len;
	
	Len = 0;
	
    if( (Mode == 0) || (Mode == 1) )
    {
        memset( (void *)pBuf, 0x00, 8);
        //Э��һ����(8�ֽ�)
        /*
        pBuf[0] = 0xf3;//(4)��(5)��Ӧ��1�ֽ���bit3bit2����
        pBuf[1] = 0xce;//(10)(11)(15)��Ӧ��2�ֽ���bit5bit4bit0����
        pBuf[2] = 0x2f;//(16)(17)(19)��Ӧ��3�ֽ���bit7bit6bit4����
        pBuf[3] = 0xe3;//(27)(28)(29)��Ӧ��4�ֽ���bit4bit3bit2����
        pBuf[4] = 0x80;
        */
        //����Ҫ����:������Э��һ���Կ�	FFFFFFFFC0000000H �C ȫ��֧��(bit0~bit33Ϊ1),ͬʱҪ��:���Э��һ���Կ顢����һ���Կ飬��Ĭ�ϲ���Ӧ���ݲ�Ҫ��Э�顢���ܶ�Ӧ��
        pBuf[3] = 0x03;
        pBuf[4] = 0xff;
        pBuf[5] = 0xff;
        pBuf[6] = 0xff;
        pBuf[7] = 0xff;
      
        pBuf += 8;
        Len += 8;
    }
    
    if( (Mode == 0) || (Mode == 2) )
    {
        memset( (void *)pBuf, 0x00, 16);
        //����һ����
        /*
        #if( MAX_PHASE == 1 )
        pBuf[0] = 0xc1;
        #else
        pBuf[0] = 0xed;//����������(0) ˫���й�����(1) �޹����ܼ���(2) �й�����(4) �޹�����(5) ������(7)
        #endif      
        
        #if( SEL_APPARENT_ENERGY == YES )
        pBuf[0] |= 0x12;//���ڵ��ܼ��� ��������
        #endif
        
        #if(SEL_RELAY_FUNC == YES)  
        pBuf[1] = 0x81;
        #if( PREPAY_MODE == PREPAY_LOCAL )
        pBuf[1] |= 0x40;
        #elif( PREPAY_MODE == PREPAY_REMOTE )
        pBuf[1] |= 0x20;
        #endif
        #else
        pBuf[1] = 0x01;
        #endif
        
        pBuf[2] = 0xf4; 
        
        */
        //����Ҫ��:�̶��Ĺ���һ���Կ�	FFFEC400000000000000000000000000H �C bit0-bit14��bit16��bit17��bit21
		pBuf[13] = 0x23;
		pBuf[14] = 0x7f;
		pBuf[15] = 0xff;
        Len += 16;
    }

    return Len;
}

//--------------------------------------------------
//��������:    ʱ��ʱ�α�ṹ�����ݴ���
//         
//����:      *pData[in] //pdata Ϊ��array�ĸ�������ʼ
//         
//����ֵ:     BOOL  TRUE ��Ч  FALSE ��Ч   
//         
//��ע����:    δ���м���ֵ�ж� ��ر�֤����������ȷ ʱ+��+���ʺ�
//--------------------------------------------------
static BOOL DealTimeZoneSegTableData( BYTE *pData )//ʱ��ʱ�α�ṹ�����ݴ���
{
	BYTE i;
	BYTE *pSTData;//�ṹ������ָ��
	
	for( i=0; i<pData[0]; i++ )
	{
		pSTData = &pData[1+8*i];

		if( pSTData[0] == Structure_698 )
		{
			if( pSTData[1] == 3)
			{
				if( pSTData[2] == Unsigned_698 )
				{
					ProtocolBuf[0+3*i] = pSTData[3];
					if( pSTData[4] == Unsigned_698 )
					{
						ProtocolBuf[1+3*i] = pSTData[5];
						if( pSTData[6] == Unsigned_698 )
						{
							ProtocolBuf[2+3*i] = pSTData[7];
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	if( i == pData[0] )
	{
		//���油�ϵ�1ʱ�Σ���Сʱ��24��Ϊ�ж����һʱ�����һʱ�μ���Ƿ񳬹�15min�ã�ʱ����Ҳ��ô�������治�����жϡ�
		ProtocolBuf[pData[0]*3]=(ProtocolBuf[0]+24);
		ProtocolBuf[pData[0]*3+1]=ProtocolBuf[1];	
		ProtocolBuf[pData[0]*3+2]=ProtocolBuf[2];
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------
//��������:     698���ݳ��ȴ���
//
//����:		BYTE *buf[in]                   //����BUF
//			BYTE len[in]                    //��������ݳ���
//			eDataLenTypeChoice eTypeChoice[in]�� 
//              eUNION_TYPE          ������ģʽ          ��Datalen�����������ݣ�ת��Ϊ����Э�鳤�ȵ�buf
//              ePROTOCOL_TYPE       Э��ģʽ           ������Э�鳤�ȵ�bufת��ΪDatalen�����������ݣ�
//              eESAM_TYPE           ESAMģʽ         ������esam���ȵ�Datalenת��Ϊ����Э�鳤�ȵ�buf
//              e_UNION_OFFSET_TYPE  ������ƫ��ģʽ        ���ظ���Datalen����ת��Ϊ����Э�鳤�ȵ�buf�����ݳ��ȣ�buf�������ݣ�
//         
//����ֵ:	    BYTE ���ݳ���
//		   
//��ע:       698���ݳ��ȴ�����Ҫ�����������ݳ��ȳ���һ���ֽڵ���� �����Ż�����,�˺��������Խ�����չ
//-----------------------------------------------
BYTE Deal_698DataLenth( BYTE *buf, BYTE *Datalen, eDataLenTypeChoice eTypeChoice )
{
	TTwoByteUnion Len;
	
	if( (buf == NULL) && (eTypeChoice != eUNION_OFFSET_TYPE))
	{
		return 0;
	}
	
	if( eTypeChoice == eUNION_TYPE )//������ݳ���
	{
		memcpy( Len.b, Datalen, 2);
		if( Len.w < 0x7f )
		{
			buf[0] = Datalen[0];
			return 1;
		}
		else if( Len.w < 0xff )
		{
			buf[0] = 0x81;
			buf[1]  = Datalen[0];
			return 2;
		}
		else
		{
			buf[0] = 0x82;
			buf[1]  = Datalen[1];
			buf[2]  = Datalen[0];	

			memcpy( Len.b, Datalen, 2);//���м���ֵ�ж� �������鳬��
			if( Len.w > MAX_APDU_SIZE )
			{
                memset( Datalen, 0x00, 2 );
			}
			
			return 3;
		}
	}
	else if( eTypeChoice == ePROTOCOL_TYPE )//�������ݳ���
	{	
		if( buf[0] == 0x82 )
		{
			Datalen[0] = buf[2];
			Datalen[1] = buf[1];
			
			memcpy( Len.b, Datalen, 2);//���м���ֵ�ж� �������鳬��
			if( Len.w > MAX_APDU_SIZE )
			{
                memset( Datalen, 0x00, 2 );
			}
			
			return 3;
		}
		else if( buf[0] == 0x81 )
		{
			Datalen[0] = buf[1];
			Datalen[1]  = 0x00 ;	
			return 2;
		}
		else
		{
			Datalen[0] = buf[0];
			Datalen[1]  = 0x00 ;	
			return 1;
		}
	}
    else if( eTypeChoice == eESAM_TYPE )
    {
    	lib_ExchangeData( Len.b, Datalen, 2);
		if( Len.w < 0x7f )
		{
			buf[0] = Datalen[0];
			return 1;
		}
		else if( Len.w < 0xff )
		{
			buf[0] = 0x81;
			buf[1]  = Datalen[0];
			return 2;
		}
		else
		{
			buf[0] = 0x82;
			buf[1]  = Datalen[1];
			buf[2]  = Datalen[0];
			
			memcpy( Len.b, Datalen, 2);//���м���ֵ�ж� �������鳬��
			if( Len.w > MAX_APDU_SIZE )
			{
                memset( Datalen, 0x00, 2 );
			}
			
			return 3;
		}
    }
   else if( eTypeChoice == eUNION_OFFSET_TYPE )
   {
		memcpy( Len.b, Datalen, 2);

		//���ݳ��ȼ��� �ó���ֵ ��698Э���е���ռ���ֽ�����
		//�ڵ��øýӿڶ�APDU����ʱ��MOû�з������ݣ�MIȴ����ȷ���գ����Ҹ�ʽУ����ȫ��ȷ�����س���Ϊ1����ǰ���ȼ�ȥ4�ֽ�MAC
		//�������ĳ��ȱ�Ϊ��ֵ�����ڴ档
		if( Len.w > MAX_APDU_SIZE )	
		{
			memset( Datalen, 0x00, 2 );
			Len.w = 0;
		}

		if( Len.w < 0x7f )
		{ 
			return 1;
		}
		else if( Len.w < 0xff )
		{ 
			return 2;
		}
		else
		{ 
			return 3;
		}
    }
	else if (eTypeChoice == eUNION_BITSTRING_TYPE) // ������ݳ���
	{
		memcpy(Len.b, Datalen, 2);
		if (Len.w < 0x7f)
		{
			buf[0] = Datalen[0];
			return 1;
		}
		else if (Len.w < 0xff)
		{
			buf[0] = 0x81;
			buf[1] = Datalen[0];
			return 2;
		}
		else
		{
			buf[0] = 0x82;
			buf[1] = Datalen[1];
			buf[2] = Datalen[0];

			return 3;
		}
	}
	
    return 0;
}

//--------------------------------------------------
//��������:  ���ɾ����ȫģʽ��������
//         
//����:     BYTE byType[in]	//byType:0--Add 1--Delete
//         
//        BYTE *pBuf[in]	//����bufָ��
//         
//����ֵ:    �� 
//         
//��ע����:  ��
//--------------------------------------------------
static void AlterSafeModePara(BYTE byType, BYTE *pBuf )//����Ļ���� OI_698 ��ʼ
{
	BYTE i;
	BYTE Flag;
	WORD OI,wSafeMode;
	
	if( byType > 1 )
	{
		return;
	}
	Flag = 0;	
	OI = pBuf[1]*0x100+pBuf[2];
	wSafeMode = pBuf[4]*0x100+pBuf[5];
	for( i=0; i<SafeModePara.Num; i++ )
	{
		if( OI == SafeModePara.Sub[i].OI )
		{
			if( byType == 0 )//add ������OI�Ѿ����ڣ��򸲸�safemode
			{
				if( SafeModePara.Sub[i].wSafeMode != wSafeMode )
				{
					SafeModePara.Sub[i].wSafeMode = wSafeMode; 
					Flag = 0xa5;
				}	 
			}
			else//delete
			{
				if( SafeModePara.Num != 0 )
				{
					if( i == (SafeModePara.Num-1) )//����������һ������ȫFF 
					{
						memset( (void *)&SafeModePara.Sub[i], 0xff, sizeof(SafeModePara.Sub[i]) );//TSafeModeParaSub);
					}
					else//�����һ�����������ǵ�
					{
						SafeModePara.Sub[i] = SafeModePara.Sub[SafeModePara.Num-1];
                        memset( (void *)&SafeModePara.Sub[SafeModePara.Num-1], 0xff, sizeof(SafeModePara.Sub[i]) );
					}
					SafeModePara.Num--;
					Flag = 0xa5;
				}	
			}
			break;
		}
	}
	if( byType == 0 )//���ӵ��������OI�� SafeModePara ��û�ҵ�
	{
		if( i >= SafeModePara.Num )
		{
			SafeModePara.Sub[SafeModePara.Num].OI = OI;
			SafeModePara.Sub[SafeModePara.Num].wSafeMode = wSafeMode;
			SafeModePara.Num++;	
			Flag = 0xa5;		
		}	
	}
	
	if( Flag == 0xa5 )
	{
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
	}				
}
//--------------------------------------------------
//��������:   ������ʾ��ȫģʽ����
//         
//����:     byType[in] byType bit0--�� SafeModePara.Flag bit1--�� SafeModePara.Num
//         
//����ֵ:    �� 
//         
//��ע����:  ��
//--------------------------------------------------
void api_ClrProSafeModePara(BYTE byType)//������ʽ��ȫģʽ����
{
	if( (byType & 0x03) == 0 )
	{
		return;
	}	
	if( byType & 0x01 )
	{
		SafeModePara.Flag = 0;//SafeModePara.Flag = 1;  @@@@@@Ϊ�˲��Է��㽫��ȫģʽ��Ϊ�ر�
	}
	if( byType & 0x02 )
	{	
		SafeModePara.Num = 0;
		memset( (BYTE*)SafeModePara.Sub, 0xff, sizeof(TSafeModeParaSub)*MAX_SAFE_MODE_PARA_NUM );
	}
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );	
}
//--------------------------------------------------
//��������:    698buf��ӳ����ж� ��������ӳ���
//         
//����:      BYTE Ch[in]	     ͨ��ѡ��
//         
//         BYTE Mode[in]     ģʽѡ�� 0X00 ����������֡����ģʽ 
//                                   0x55 ��������֡����ģʽ
//                                   0x77 �����ϱ���������֡����ģʽ
//         
//         BYTE Len[in]      ��Ҫ��ӵ����ݳ���
//         
//����ֵ:     eAPPBufResultChoice buf�������
//         
//��ע����:    ��ӷ������ݳ���ʱ���ʹ�ô˺����������ϱ���Ԥ���ռ��赥���ж�
//--------------------------------------------------
static eAPPBufResultChoice  DLT698AddBufLen(BYTE Ch, BYTE Mode,WORD Len)
{

	if( (Mode == 0x00) ||(Mode == 0x55) )//�������buf����
	{
        if( ((APPData[Ch].APPCurrentBufLen.w+Len) <= APPData[Ch].APPMaxBufLen.w) && ((APPData[Ch].APPCurrentBufLen.w+Len) <= MAX_APDU_SIZE) )//С��apdu���ߴ�
        {    
            APPData[Ch].APPCurrentBufLen.w += Len;//ʵ����֡���ݳ���
        }
        else//�������ߴ�
        {
            return eAPP_LINKDATA_OK;
        }       

		if( Mode == 0x55 )
		{
			APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
		}		
	}
	else if( Mode == 0x77 )
	{
        if( (APPData[Ch].APPCurrentBufLen.w+ Len) <= MAX_APDU_SIZE)//С��apdu���ߴ�
        {
            APPData[Ch].APPCurrentBufLen.w += Len;//ʵ����֡���ݳ���
        }
        else//�������ߴ�
        {
            return eAPP_LINKDATA_OK;
        }
        
        APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
	}
	else
	{}

	return eADD_OK;
}
//-----------------------------------------------
//��������:     ���ָ���������ֽڵ�Ӧ�ò�buf
//
//����:		Ch[in]       ͨ��ѡ��
//			Mode[in]     ���ģʽѡ�� 	 0X00 ����������֡����ģʽ 
//									 0x55 ��������֡����ģʽ 
//                                   0x77 ����ʱ���ǩ�������ϱ���������� 
//			*Buf[in]     ����bufָ��
//			Len[in]      ����
//����ֵ:	    eAPPBufResultChoice ������ӽ��
//		   
//��ע:       ��ӷ�������ʱ���ʹ�ô˺����������ϱ���Ԥ���ռ��赥���ж�
//-----------------------------------------------
static eAPPBufResultChoice  DLT698AddBuf(BYTE Ch, BYTE Mode,BYTE *Buf,WORD Len)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }

	if( (Mode == 0x00) ||(Mode == 0x55) )//�������buf����
	{
		if( ((APPData[Ch].APPCurrentBufLen.w+ Len) <= APPData[Ch].APPMaxBufLen.w) && ((APPData[Ch].APPCurrentBufLen.w+Len) <= MAX_APDU_SIZE))//С��apdu���ߴ�
		{
			memcpy( &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],Buf,Len);
			APPData[Ch].APPCurrentBufLen.w += Len;//ʵ����֡���ݳ���
		}
		else//�������ߴ�
		{
			return eAPP_LINKDATA_OK;
		}
		
		if( Mode == 0x55 )
		{
			APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
		}
	}
	else if( Mode == 0x77 )
	{
        if( (APPData[Ch].APPCurrentBufLen.w+ Len) <= MAX_APDU_SIZE)//С��apdu���ߴ�
        {
            memcpy( &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],Buf,Len);
            APPData[Ch].APPCurrentBufLen.w += Len;//ʵ����֡���ݳ���
        }
        else//�������ߴ�
        {
            return eAPP_LINKDATA_OK;
        }
        
        APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
	}
	else
	{}
	
	return eADD_OK;
}
//--------------------------------------------------
//��������:  ��������ĺ���
//         
//����:		Ch[in]       ͨ��ѡ��
//			Mode[in]     ���ģʽѡ�� 	 0X00 ����������֡����ģʽ 
//									 0x55 ��������֡����ģʽ 
//                                   0x77 ����ʱ���ǩ�������ϱ���������� 
//			*Buf[in]     ����bufָ��
//			Len[in]      ����
//����ֵ:	    eAPPBufResultChoice ������ӽ��
//��ע:  
//--------------------------------------------------
BYTE  api_DLT698AddBuf(BYTE Ch, BYTE Mode,BYTE *Buf,WORD Len)
{
	return DLT698AddBuf(Ch,Mode,Buf,Len);
}
//-----------------------------------------------
//��������:     ���1���ֽڵ�Ӧ�ò�buf
//
//����:		Ch[in]    	ͨ��ѡ��
//			Mode[in]    ���ģʽѡ�� 	0X00 ����������֡����ģʽ 
//										0x55 ��������֡����ģʽ 
//										0x22 �����ϱ�����������֡����ģʽ 
//										0x77 �����ϱ���������֡����ģʽ
//			Data[in]   	�����1���ֽ�
//����ֵ:      eAPPBufResultChoice  ������ӽ��
//		   
//��ע:       ��ӷ�������ʱ���ʹ�ô˺����������ϱ���Ԥ���ռ��赥���ж�
//-----------------------------------------------
static eAPPBufResultChoice  DLT698AddOneBuf( BYTE Ch,BYTE Mode,BYTE Data)//���һ������
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }

	if( (Mode == 0x00) ||(Mode == 0x55) )//�������buf����
	{	
		if( ((APPData[Ch].APPCurrentBufLen.w+ 1) <= APPData[Ch].APPMaxBufLen.w) && ((APPData[Ch].APPCurrentBufLen.w+1) <= MAX_APDU_SIZE))//С��apdu���ߴ�
		{
			APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w] = Data;
			APPData[Ch].APPCurrentBufLen.w += 1;//ʵ����֡���ݳ���
		}
		else//�������ߴ�
		{
			return eAPP_LINKDATA_OK;
		}				
		
		if( Mode == 0x55 )
		{
			APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
		}
	}
	else if( Mode == 0x77 )
	{
        if( (APPData[Ch].APPCurrentBufLen.w+ 1) <= MAX_APDU_SIZE)//С��apdu���ߴ�
        {
           APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w] = Data;
			APPData[Ch].APPCurrentBufLen.w += 1;//ʵ����֡���ݳ���
        }
        else//�������ߴ�
        {
            return eAPP_LINKDATA_OK;
        }
        
        APPData[Ch].APPBufLen.w = APPData[Ch].APPCurrentBufLen.w;
	}
	else
	{}
	
	return eADD_OK;
}
//--------------------------------------------------
//��������:    ͨ�����DAR����
//         
//����:      BYTE Ch[in]
//         
//         BYTE DAR[in]//��Ӧ����   CHOICE  {     ������Ϣ     [0] DAR��   M����¼     [1] SEQUENCE OF A-RecordRow}
//         
//����ֵ:     eAPPBufResultChoice  ������ӽ��
//         
//��ע����:    ��
//--------------------------------------------------
static eAPPBufResultChoice  DLT698AddBufDAR( BYTE Ch,BYTE DAR )
{
	eAPPBufResultChoice eResultChoice;

    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }

	eResultChoice = DLT698AddOneBuf(Ch,0, 0);
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//DAR
	return eResultChoice;
}

//-----------------------------------------------
//��������:     698Э����sequence of num�Ĵ���
//
//����:       BYTE Ch[in]                     //ͨ��		
//          WORD NoAddress[in]              //NUM���ڵĵ�ַ
//			BYTE *No[in]                    //ʵ������
//
//����ֵ:	    BYTE ���ݳ���
//		   
//��ע:       698���ݳ��ȴ�����Ҫ�����������ݳ��ȳ���һ���ֽڵ���� �����Ż�����,�˺��������Խ�����չ
//-----------------------------------------------
void Deal_698SequenceOfNumData( BYTE Ch, WORD NoAddress, BYTE *No )
{
    BYTE LenOffest;
    TTwoByteUnion TmpNo,Len;
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }

    memcpy( TmpNo.b, No, sizeof(TTwoByteUnion));
    LenOffest = Deal_698DataLenth( NULL, TmpNo.b, eUNION_OFFSET_TYPE);

    if( LenOffest != 1 )
    {
        if( APPData[Ch].APPBufLen.w >= NoAddress)//���м���ֵ�ж�
        {
            Len.w = APPData[Ch].APPBufLen.w -NoAddress;
        }
        else if( APPData[Ch].APPCurrentBufLen.w >= NoAddress )
        {
            Len.w = APPData[Ch].APPBufLen.w -NoAddress;
        }
        else
        {
            Len.w = 0;
        }
        memmove( (BYTE*)&APPData[Ch].pAPPBuf[NoAddress+LenOffest-1],(BYTE*)&APPData[Ch].pAPPBuf[NoAddress], Len.w);
        Deal_698DataLenth( (BYTE*)&APPData[Ch].pAPPBuf[NoAddress-1], TmpNo.b, eUNION_TYPE);
        DLT698AddBufLen(Ch, 0x77, (LenOffest-1) );//��¼����buf���ȸı�-������Ԥ��ֱ����Ӽ���
    }
    else
    {
        APPData[Ch].pAPPBuf[(NoAddress-1)] = TmpNo.w;
    }
}

//-----------------------------------------------
//��������:     ��������SID_MAC����
//
//����:		
//			BYTE Mode[in]     //mode:0x00 ��ͨ��sid��֡ģʽ  0x55 ���ݸ��µ�����ģʽ ��Ҫ���������OAD+len 5���ֽ�ȥ��
//			BYTE *psmbuf[in]  //sid_mac������ָ�� �������ݳ���
//			BYTE *pdata[in]   //��Ҫ��SID_MAC������ָ�룬�������ݳ���
//          BYTE *OutBuf[in]  //���buf
//����ֵ:		WORD   �����������ݳ���
//		   
//��ע:       ��
//-----------------------------------------------
static WORD Deal_SIDMACDate( BYTE Mode, BYTE *psmbuf, BYTE *pdata, BYTE *OutBuf )
{
	TTwoByteUnion SMLen,DataLen,MacLen;
	BYTE SMOffest,DataOffest,MacOffest;
	
	memcpy( OutBuf, psmbuf, 04);//��ǰ4���ֽ�
	
	SMOffest = Deal_698DataLenth( &psmbuf[4], SMLen.b, ePROTOCOL_TYPE);
	memcpy( &OutBuf[4], &psmbuf[4+SMOffest], SMLen.w);//����������

	DataOffest = Deal_698DataLenth( pdata, DataLen.b, ePROTOCOL_TYPE);
	if( Mode == 0x55 )
	{
	    DataLen.w = DataLen.w-5;//������OAD+����len
		memcpy( &OutBuf[4+SMLen.w], &pdata[DataOffest+5], DataLen.w );//������
	}
	else
	{
		memcpy( &OutBuf[4+SMLen.w], &pdata[DataOffest], DataLen.w);//������
	}


	MacOffest = Deal_698DataLenth( &psmbuf[4+SMOffest+SMLen.w], MacLen.b, ePROTOCOL_TYPE);
	memcpy( &OutBuf[4+SMLen.w+DataLen.w], &psmbuf[4+SMOffest+SMLen.w+MacOffest], MacLen.w);//������
		
	return ( 4+SMOffest+SMLen.w+DataOffest+DataLen.w+MacOffest+MacLen.w);
}

//-----------------------------------------------
//��������:     ��������SID����
//
//����:		BYTE Mode[in]     0x00 ������Ҫ������ 0x55������Ҫ������ //0x00�����ֻ��sid�޺������ݵ����
//			BYTE *psmbuf[in]  sid������ָ�� �������ݳ���
//			BYTE *pdata[in]   ��Ҫ��SID������ָ�룬�������ݳ���
//			BYTE *OutBuf[in]  ���buf
//
//����ֵ:		WORD  �����������ݳ���
//		   
//��ע:       ��
//-----------------------------------------------
static WORD Deal_SIDDate(BYTE Mode,BYTE *psmbuf,BYTE *pdata,BYTE *OutBuf )
{
	TTwoByteUnion SMLen,DataLen;
	BYTE SMOffest,DataOffest;
	
	memcpy( OutBuf, psmbuf, 04);//��ǰ4���ֽ�
	
	SMOffest = Deal_698DataLenth( &psmbuf[4], SMLen.b, ePROTOCOL_TYPE);
	memcpy( &OutBuf[4], &psmbuf[4+SMOffest], SMLen.w);//����������

	if( Mode == 0x55)
	{
		DataOffest = Deal_698DataLenth( pdata, DataLen.b, ePROTOCOL_TYPE);
		memcpy( &OutBuf[4+SMLen.w], &pdata[DataOffest], DataLen.w);//������
		return ( 4+SMOffest+SMLen.w+DataOffest+DataLen.w);
	}
	return ( 4+SMOffest+SMLen.w);		
}
//--------------------------------------------------
//��������:  ����Ԫ��������Ϊ�������¶����ݽ��д���
//         
//����:      BYTE   *pData[in] ��Ҫ�жϵ�����ָ��
//         
//����ֵ:     BYTE*  ���ش���������ָ��
//         
//��ע����:    ��
//--------------------------------------------------
static BYTE *DealArrayClassIndexNotZeroData( BYTE *pData )
{
	pData = pData -2;//������ָ��ƫ������ָ��
	pData[0] = Array_698;
	pData[1] = 1;
	return pData;
}
//--------------------------------------------------
//��������:  �ṹ��Ԫ��������Ϊ�������¶����ݽ��д���
//         
//����:     BYTE * pData[in]
//         
//����ֵ:    BYTE*  ���ش���������ָ��
//         
//��ע����:  ��
//--------------------------------------------------
static BYTE *DealStructureClassIndexNotZeroData( BYTE *pData )
{
	pData = pData -2;//������ָ��ƫ������ָ��
	pData[0] = Structure_698;
	pData[1] = 1;
	return pData;
}
//-----------------------------------------------
//��������:     ��ȫ�������ݴ���
//
//����:		BYTE Ch[in]   //ͨ��ѡ��

//����ֵ:		eSecurityMode ���ذ�ȫģʽ
//		   
//��ע:       ��
//-----------------------------------------------
static eSecurityMode ProcSecurityRequestData( BYTE Ch )
{
	TTwoByteUnion DataLen,Len,Len1;
	BYTE  DataOffest,SecurityMode,LenOffest,Len1Offest;
	WORD Result;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eSECURITY_FAIL;
    }

	//֧�ְ�ȫ������ӦӦ��
	if( (LinkData[Ch].pAPP[0] != SECURITY_REQUEST) && (LinkData[Ch].pAPP[0] != SECURITY_RESPONSE) )
	{
		return eNO_SECURITY;
	}
	
	LenOffest = Deal_698DataLenth( &LinkData[Ch].pAPP[2], Len.b, ePROTOCOL_TYPE);//��ȡ���ݳ���
	Len.w += LenOffest+2;

	SecurityMode =LinkData[Ch].pAPP[1];
	
	if( SecurityMode == 0xff )
	{
		return eSECURITY_FAIL;
	}
	else if( LinkData[Ch].pAPP[0] == SECURITY_REQUEST )
	{
		if( (SecurityMode == 0x00) || (SecurityMode == 0x01) )//���ķ�ʽ
		{
			if( LinkData[Ch].pAPP[Len.w] == 0x00 )//�ж��Ƿ�ѡ��sid_mac
			{	
				if( LinkData[Ch].AddressType != eBROADCAST_ADDRESS_MODE)//ֻ�й㲥����ж��Ƿ���Ӧ������
				{
					if( (CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL ) )
					{
						if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)//�ն˼���
						{
							return eSECURITY_FAIL;
						}
					}
					else
					{
						if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )//��վ����
						{
							return eSECURITY_FAIL;
						}
					}
				}
				
				Deal_SIDMACDate(0x00, &LinkData[Ch].pAPP[Len.w+1],&LinkData[Ch].pAPP[2],ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				
				if( Result == TRUE )
				{
					DataLen.b[0] = ProtocolBuf[1] ;
					DataLen.b[1] = ProtocolBuf[0] ;
					memcpy( LinkData[Ch].pAPP, &ProtocolBuf[2], DataLen.w);
					memcpy( SecurityRequestBuf, &ProtocolBuf[2], 50);//�����������
					LinkData[Ch].pAPPLen.w = DataLen.w;
	
					if( SecurityMode == 0x01 )//����ģʽ
					{	
						return eSECRET_MAC;
					}
					else
					{
						return eEXPRESS_MAC;
					}
					
				}
				else
				{
					return eSECURITY_FAIL;
				}
			}
			else if( LinkData[Ch].pAPP[Len.w] == 0x01 )//����� Ĭ����������ᳬ��0xff���ֽ�
			{
				Len1Offest = Deal_698DataLenth( &LinkData[Ch].pAPP[Len.w+1], Len1.b, ePROTOCOL_TYPE);//��ȡƫ�Ƴ���
				if( Len1.w > MAX_RN_SIZE )//���м���ֵ�ж�
				{
					return eSECURITY_FAIL;
				}
				
				RN[0] = Len1.w;
				
				memcpy( &RN[1], &LinkData[Ch].pAPP[Len.w+1+Len1Offest], RN[0] );
				
				DataOffest = Deal_698DataLenth( &LinkData[Ch].pAPP[2], DataLen.b, ePROTOCOL_TYPE);//��ȡƫ�Ƴ���
				LinkData[Ch].pAPP = &LinkData[Ch].pAPP[2+DataOffest ];//�ı�ָ�� ������apdu��
	
				LinkData[Ch].pAPPLen.w = DataLen.w;//�ı䳤��
				return eSECURITY_RN;
			}
			else if( LinkData[Ch].pAPP[Len.w] == 0x02 )//�����+mac δʵ�� ֻΪ��������ϱ�ʵ��
			{
				return eSECURITY_FAIL;
			}
			else if( LinkData[Ch].pAPP[Len.w] == 0x03 )//SID
			{
				if( LinkData[Ch].AddressType != eBROADCAST_ADDRESS_MODE)//ֻ�й㲥����ж��Ƿ���Ӧ������
				{
					if( (CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL ) )
					{
						if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)//�ն˼���
						{
							return eSECURITY_FAIL;
						}
					}
					else
					{
						if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )//��վ����
						{
							return eSECURITY_FAIL;
						}
					}
				}

				Deal_SIDDate(0x55,&LinkData[Ch].pAPP[Len.w+1] ,&LinkData[Ch].pAPP[2],ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					if( SecurityMode == 0x01)//����ģʽ
					{
						DataLen.b[0] =ProtocolBuf[1] ;
						DataLen.b[1] = ProtocolBuf[0] ;
						memcpy( LinkData[Ch].pAPP, &ProtocolBuf[2], DataLen.w);
						memcpy( SecurityRequestBuf, &ProtocolBuf[2], 50);//�����������
						LinkData[Ch].pAPPLen.w = DataLen.w;
						return eSECRET_TEXT;
					}
					else
					{
						return eSECURITY_FAIL;//����������+sid�����
					}
					
				}
				else
				{
					return eSECURITY_FAIL;
				}
			}
		}
		else
		{
			return eSECURITY_FAIL;
		}
	}
	else
	{
		if( SecurityMode == 0xff )
		{
			return eSECURITY_FAIL;
		}
		if( LinkData[Ch].pAPP[0] == SECURITY_RESPONSE )
		{
			if( SecurityMode == 0x00 )//���ķ�ʽ
			{
				if( LinkData[Ch].pAPP[Len.w] != 0x00 )//OPTIONAL ѡ��
				{
					if( LinkData[Ch].pAPP[Len.w+1] == 0x00 )//ѡ��MAC
					{
						LenOffest = Deal_698DataLenth( &LinkData[Ch].pAPP[2], Len.b, ePROTOCOL_TYPE);//��ȡƫ�Ƴ���
						if( Len.w > MAX_APDU_SIZE )//���м���ֵ�ж�
						{
							return eSECURITY_FAIL;
						}

						memcpy( ProtocolBuf, &LinkData[Ch].pAPP[2+LenOffest],Len.w );
						
						Len1Offest = Deal_698DataLenth( &LinkData[Ch].pAPP[2+LenOffest+Len.w+2], Len1.b, ePROTOCOL_TYPE);//��ȡƫ�Ƴ���
						if( Len1.w > MAX_RN_SIZE )//���м���ֵ�ж�
						{
							return eSECURITY_FAIL;
						}
						
						memcpy( ProtocolBuf+Len.w, &LinkData[Ch].pAPP[2+LenOffest+Len.w+2+Len1Offest],Len1.w );
					
						Result = api_VerifyEsamSafeReportMAC( Len1.w+Len.w, ProtocolBuf , RN_MAC);
						if( Result == TRUE )
						{
							memcpy( LinkData[Ch].pAPP, ProtocolBuf, Len.w);
							LinkData[Ch].pAPPLen.w = Len.w;
							return eRNMAC;
						}
						else
						{
							//ʹ�ñ��ݽ���ȷ��
							Result = api_VerifyEsamSafeReportMAC( Len1.w+Len.w, ProtocolBuf , RN_MAC_BAK);
							if( Result == TRUE )
							{
								memcpy( LinkData[Ch].pAPP, ProtocolBuf, Len.w);
								LinkData[Ch].pAPPLen.w = Len.w;
								return eRNMAC;
							}
							
							return eSECURITY_FAIL;
						}
	
					}
				}
			}
		}
	}
	
	return eSECURITY_FAIL;
}

//--------------------------------------------------
//��������: �����ն˷�֧�����ݽ���
//
//����:		APDU[in]: ����
//			CommType[in]: �ն���վ
//			Out[out]:
//����ֵ: ��ǰ�ļ��ܷ�ʽ
//
//��ע:
//--------------------------------------------------
BYTE TremainProcSecurityRequestData( BYTE *APDU, BYTE CommType, BYTE *Out ,WORD* wDataLen)
{
	TTwoByteUnion DataLen, Len, Len1;
	BYTE DataOffest, SecurityMode, LenOffest, Len1Offest;
	WORD Result;

	//֧�ְ�ȫ������ӦӦ��
	if( (APDU[0] != SECURITY_REQUEST) && (APDU[0] != SECURITY_RESPONSE) )
	{
		return eNO_SECURITY;
	}
	
	LenOffest = Deal_698DataLenth( &APDU[2], Len.b, ePROTOCOL_TYPE);//��ȡ���ݳ���
	Len.w += LenOffest + 2;

	SecurityMode = APDU[1];
	
	if( SecurityMode == 0xff )
	{
		return eSECURITY_FAIL;
	}
	else if( APDU[0] == SECURITY_REQUEST )
	{
		if( (SecurityMode == 0x00) || (SecurityMode == 0x01) )//���ķ�ʽ
		{
			if( APDU[Len.w] == 0x00 )//�ж��Ƿ�ѡ��sid_mac
			{
				if( CommType == COMM_TYPE_TERMINAL )
				{
					if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)//�ն˼���
					{
						return eSECURITY_FAIL;
					}
				}
				else if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )//��վ����
				{
					return eSECURITY_FAIL;
				}
				
				Deal_SIDMACDate(0x00, &APDU[Len.w + 1], &APDU[2], ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					DataLen.b[0] = ProtocolBuf[1] ;
					DataLen.b[1] = ProtocolBuf[0] ;
					memcpy( Out, &ProtocolBuf[2], DataLen.w);
					memcpy( SecurityRequestBuf, &ProtocolBuf[2], 50);//�����������
					
					*wDataLen = DataLen.w;
	
					if( SecurityMode == 0x01 )//����ģʽ
					{	
						return eSECRET_MAC;
					}
					else
					{
						return eEXPRESS_MAC;
					}
				}
			}
			else if( APDU[Len.w] == 0x01 )//����� Ĭ����������ᳬ��0xff���ֽ�
			{
				Len1Offest = Deal_698DataLenth( &APDU[Len.w+1], Len1.b, ePROTOCOL_TYPE);//��ȡƫ�Ƴ���
				if( Len1.w > MAX_RN_SIZE )//���м���ֵ�ж�
				{
					return eSECURITY_FAIL;
				}
				
				RN[0] = Len1.w;
				memcpy( &RN[1], &APDU[Len.w+1+Len1Offest], RN[0] );
				
				DataOffest = Deal_698DataLenth( &APDU[2], DataLen.b, ePROTOCOL_TYPE);//��ȡƫ�Ƴ���
				memmove(Out, &APDU[2 + DataOffest], DataLen.w);

				*wDataLen = DataLen.w;

				return eSECURITY_RN;
			}
			else if( APDU[Len.w] == 0x02 )//�����+mac δʵ�� ֻΪ��������ϱ�ʵ��
			{
				return eSECURITY_FAIL;
			}
			else if( APDU[Len.w] == 0x03 )//SID
			{
				if( CommType == COMM_TYPE_TERMINAL )
				{
					if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == FALSE)//�ն˼���
					{
						return eSECURITY_FAIL;
					}
				}
				else if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == FALSE )//��վ����
				{
					return eSECURITY_FAIL;
				}

				Deal_SIDDate(0x55, &APDU[Len.w + 1], &APDU[2], ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					if( SecurityMode == 0x01)//����ģʽ
					{
						DataLen.b[0] =ProtocolBuf[1] ;
						DataLen.b[1] = ProtocolBuf[0] ;
						memcpy( Out, &ProtocolBuf[2], DataLen.w);
						memcpy( SecurityRequestBuf, &ProtocolBuf[2], 50);//�����������
						*wDataLen = DataLen.w;

						return eSECRET_TEXT;
					}	
				}
			}
		}
	}
	
	return eSECURITY_FAIL;
}

//--------------------------------------------------
//��������: ���ݼ���
//
//����:	SafeMode[in]: TremainProcSecurityRequestData()����ֵ
//		APDU[in/out]: �����ܵ�����
//		CommType[in]: �ն� ��վ
//		APDULen[in]: �������ݵĳ���
//����ֵ: 0x8000���ȳ��ޣ� ���ĳ���
//
//��ע:��
//--------------------------------------------------
WORD TremainProcSecurityResponseData ( BYTE SafeMode, BYTE *APDU, BYTE CommType, WORD APDULen)
{
	TTwoByteUnion Len;
	BYTE BufIndex, *Respon = APDU;
	WORD Result = FALSE;
    
	if(SafeMode == eNO_SECURITY)	//���Ĳ�����
	{
		return APDULen;
	}

	if( SafeMode != eSECURITY_FAIL )
	{
		if( ( SafeMode == eSECRET_MAC ) ||( SafeMode == eSECRET_TEXT )||(SafeMode == eEXPRESS_MAC ))//P2��11������+MAC 12������ 13�� ����+MAC
		{
			if( APDULen > sizeof(ProtocolBuf) )//���ݳ����������
			{
				return 0x8000;
			}
			memcpy(ProtocolBuf, APDU, APDULen);
			memcpy( SecurityResponseBuf, APDU, 50);//�����������
			if( CommType == COMM_TYPE_TERMINAL )//�ն˼���
			{
				Result = api_PackEsamDataWithSafe(0x55, (SafeMode - 3), APDULen, ProtocolBuf);
			}
			else//��վ����
			{
				Result = api_PackEsamDataWithSafe(0x00, (SafeMode - 3), APDULen, ProtocolBuf);
			}
			
			if( Result == TRUE )
			{
				Len.b[0] = ProtocolBuf[1];
				Len.b[1] = ProtocolBuf[0];
		
				if( (SafeMode == eSECRET_MAC) ||(SafeMode == eEXPRESS_MAC)  ) 
				{
					Len.w = (Len.w-4);
					BufIndex = Deal_698DataLenth( (BYTE*)NULL, Len.b,eUNION_OFFSET_TYPE);//��ȡ���������ݵ�ƫ�Ƴ���
					BufIndex = BufIndex+2;
					
					memcpy( &Respon[BufIndex], &ProtocolBuf[2], Len.w );
					Respon[ Len.w+BufIndex ] = 0x01;			//��������֤��Ϣ
					Respon[ Len.w+BufIndex+1 ] = 0x00;			//ѡ��MAC
					Respon[ Len.w+BufIndex+1+1 ] = 0x04;		//MAC ��4�ֽ�
					memcpy( &Respon[ Len.w+BufIndex+1+1+1],&ProtocolBuf[2+Len.w],0x04);	//4�ֽ�MAC

					Respon[0] = SECURITY_RESPONSE;//�鰲ȫ��������ͷ
					if( SafeMode == eSECRET_MAC )//ѡ������
					{
						Respon[1] = 0x01;
					}
					else//ѡ������
					{
						Respon[1] = 0x00;
					}
			
					Deal_698DataLenth( &Respon[2], Len.b,eUNION_TYPE);	//���ĳ��ȣ�������MAC
					APDULen = (Len.w+BufIndex+7);//+��ȫ����ͷ+mac
				}
				else	//�·� ���� + SID ����������  
				{
					BufIndex = Deal_698DataLenth( (BYTE*)NULL, Len.b,eUNION_OFFSET_TYPE);
					BufIndex = BufIndex+2;
					
					memcpy( &Respon[BufIndex], &ProtocolBuf[2], Len.w);//��������
					Respon[ Len.w+BufIndex ] = 0x00;//optional ��ѡ������
					
					Respon[0] =SECURITY_RESPONSE;//�鰲ȫ��������ͷ
					Respon[1] = 0x01;
					Deal_698DataLenth( &Respon[2], Len.b,eUNION_TYPE);
					APDULen = (Len.w+BufIndex+1);//+��ȫ����ͷ+mac
				}
			}
		}
		else if( SafeMode == eSECURITY_RN )
		{
			memcpy(&ProtocolBuf, &RN[1], RN[0]);
			memcpy(&ProtocolBuf[RN[0]], APDU, APDULen);
			Result = api_PackDataWithSafe((APDULen + RN[0]), ProtocolBuf);
			if( Result == TRUE )
			{				
				BufIndex = Deal_698DataLenth( (BYTE*)NULL, (BYTE *)&APDULen,eUNION_OFFSET_TYPE);//��ȡ���������ݵ�ƫ�Ƴ���
				BufIndex = BufIndex+2;

				memmove(&Respon[BufIndex], APDU, APDULen);
				Respon[ APDULen+BufIndex ] = 0x01;
               	Respon[ APDULen+BufIndex+1 ] = 0x00;
                Respon[ APDULen+BufIndex+1+1 ] = 0x04;
                memcpy( &Respon[ APDULen+BufIndex+1+1+1],&ProtocolBuf[2],4);

                Respon[0] =SECURITY_RESPONSE;//�鰲ȫ��������ͷ
                Respon[1] = 0x00;
                               
                Deal_698DataLenth( &Respon[2],(BYTE *)&APDULen,eUNION_TYPE);
                APDULen = (4+APDULen+BufIndex+3);//+��ȫ����ͷ+mac
			}
		}
	}

	if(Result == FALSE)
	{
		Respon[0] =SECURITY_RESPONSE;
		Respon[1] = 0x02;
		Respon[2] = DAR_Symmetry;//DAR
		Respon[3] = 00;
		APDULen = 4;//+��ȫ����ͷ+mac
	}

	return APDULen;
}

//-----------------------------------------------
//��������:     ��ȫ��Ӧ���ݴ���
//
//����:		BYTE Ch[in]   //ͨ��ѡ��
//
//����ֵ:		��
//		   
//��ע:       ��
//-----------------------------------------------
static void ProcSecurityResponseData ( BYTE Ch)
{
	TTwoByteUnion Len;
	BYTE BufIndex;
	WORD Result;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
	if( (APPData[Ch].eSafeMode != eSECURITY_FAIL) && (APPData[Ch].eSafeMode != eNO_SECURITY)  )
	{
		if( ( APPData[Ch].eSafeMode == eSECRET_MAC ) ||( APPData[Ch].eSafeMode == eSECRET_TEXT )||( APPData[Ch].eSafeMode == eEXPRESS_MAC ))//P2��11������+MAC 12������ 13�� ����+MAC
		{
			if( APPData[Ch].APPBufLen.w > sizeof(ProtocolBuf) )//���ݳ����������
			{
				return;
			}
			memcpy( ProtocolBuf,APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);//�������ݵİ���
			memcpy( SecurityResponseBuf, APPData[Ch].pAPPBuf, 50);//�����������
			if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL )//�ն˼���
			{
				Result = api_PackEsamDataWithSafe( 0x55, (APPData[Ch].eSafeMode-3),APPData[Ch].APPBufLen.w,ProtocolBuf);
			}
			else//��վ����
			{
				Result = api_PackEsamDataWithSafe( 0x00, (APPData[Ch].eSafeMode-3),APPData[Ch].APPBufLen.w,ProtocolBuf);
			}
			
			if( Result == TRUE )
			{
				Len.b[0] = ProtocolBuf[1];
				Len.b[1] = ProtocolBuf[0];
		
				if( (APPData[Ch].eSafeMode == eSECRET_MAC) ||(APPData[Ch].eSafeMode == eEXPRESS_MAC)  ) 
				{
					Len.w = (Len.w-4);
					BufIndex = Deal_698DataLenth( (BYTE*)NULL, Len.b,eUNION_OFFSET_TYPE);//��ȡ���������ݵ�ƫ�Ƴ���
					BufIndex = BufIndex+2;
					
					memcpy( &APPData[Ch].pAPPBuf[BufIndex], &ProtocolBuf[2], Len.w );
					APPData[Ch].pAPPBuf[ Len.w+BufIndex ] = 0x01;
					APPData[Ch].pAPPBuf[ Len.w+BufIndex+1 ] = 0x00;
					APPData[Ch].pAPPBuf[ Len.w+BufIndex+1+1 ] = 0x04;
					memcpy( &APPData[Ch].pAPPBuf[ Len.w+BufIndex+1+1+1],&ProtocolBuf[2+Len.w],0x04);

					APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;//�鰲ȫ��������ͷ
					if( APPData[Ch].eSafeMode == eSECRET_MAC )//ѡ������
					{
						APPData[Ch].pAPPBuf[1] = 0x01;
					}
					else//ѡ������
					{
						APPData[Ch].pAPPBuf[1] = 0x00;
					}
			
					Deal_698DataLenth( &APPData[Ch].pAPPBuf[2], Len.b,eUNION_TYPE);
					APPData[Ch].APPBufLen.w = (Len.w+BufIndex+7);//+��ȫ����ͷ+mac
					APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
				}
				else
				{
					BufIndex = Deal_698DataLenth( (BYTE*)NULL, Len.b,eUNION_OFFSET_TYPE);
					BufIndex = BufIndex+2;
					
					memcpy( &APPData[Ch].pAPPBuf[BufIndex], &ProtocolBuf[2], Len.w);//��������
					APPData[Ch].pAPPBuf[ Len.w+BufIndex ] = 0x00;//optional ��ѡ������
					
					APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;//�鰲ȫ��������ͷ
					APPData[Ch].pAPPBuf[1] = 0x01;
					Deal_698DataLenth( &APPData[Ch].pAPPBuf[2], Len.b,eUNION_TYPE);
					APPData[Ch].APPBufLen.w = (Len.w+BufIndex+1);//+��ȫ����ͷ+mac
					APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
				}
			}
			else//����ʧ��
			{
				APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;
				APPData[Ch].pAPPBuf[1] = 0x02;
				APPData[Ch].pAPPBuf[2] = DAR_Symmetry;//DAR
				APPData[Ch].pAPPBuf[3] = 00;
				APPData[Ch].APPBufLen.w = 4;//+��ȫ����ͷ+mac
				APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
			}
		}
		else if( APPData[Ch].eSafeMode == eSECURITY_RN )
		{
			memcpy( &ProtocolBuf, &RN[1],RN[0]);
			memcpy( &ProtocolBuf[RN[0]], APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);
			if(api_PackDataWithSafe((APPData[Ch].APPBufLen.w+RN[0]),ProtocolBuf) == FALSE)
			{
				memset(ProtocolBuf, 0xFF, 6);		//ESAM ͨ��ʧ�ܣ�MAC����ȫFF�������ضԳƽ��ܴ��󣬷�ֹ�ֳ����ݳ�����������
			}
				
			BufIndex = Deal_698DataLenth( (BYTE*)NULL, APPData[Ch].APPBufLen.b,eUNION_OFFSET_TYPE);//��ȡ���������ݵ�ƫ�Ƴ���
			BufIndex = BufIndex+2;
							
			memmove( &APPData[Ch].pAPPBuf[BufIndex],APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);
			APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex ] = 0x01;
			APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex+1 ] = 0x00;
			APPData[Ch].pAPPBuf[ APPData[Ch].APPBufLen.w+BufIndex+1+1 ] = 0x04;

			memcpy(&APPData[Ch].pAPPBuf[APPData[Ch].APPBufLen.w + BufIndex + 1 + 1 + 1], &ProtocolBuf[2], 4);

			APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;//�鰲ȫ��������ͷ
			APPData[Ch].pAPPBuf[1] = 0x00;
							
			Deal_698DataLenth( &APPData[Ch].pAPPBuf[2],APPData[Ch].APPBufLen.b,eUNION_TYPE);
			APPData[Ch].APPBufLen.w = (4+APPData[Ch].APPBufLen.w+BufIndex+3);//+��ȫ����ͷ+mac
			APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
		}
	}
	else if( APPData[Ch].eSafeMode == eSECURITY_FAIL )
	{
		if( LinkData[Ch].pAPP[0] == SECURITY_RESPONSE )//�����Ӧ����ʧ�ܲ���������
		{
			APPData[Ch].APPFlag = APP_NO_DATA;
		}
		else
		{
			APPData[Ch].APPFlag = APP_HAVE_DATA;
			APPData[Ch].pAPPBuf[0] =SECURITY_RESPONSE;
			APPData[Ch].pAPPBuf[1] = 0x02;
			APPData[Ch].pAPPBuf[2] = DAR_Symmetry;//DAR
			APPData[Ch].pAPPBuf[3] = 00;
			APPData[Ch].APPBufLen.w = 4;//+��ȫ����ͷ+mac
			APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;	
		}
	}
	
}
//-----------------------------------------------
//��������:    ��֡�жϽ���
//
//����:		BYTE Ch[in]   //ͨ��ѡ��
//
//����ֵ:		��
//		   
//��ע:       ��
//-----------------------------------------------
static void ProFollowJudge ( BYTE Ch)
{
    DWORD ConsultLen;

    ConsultLen = GetConsultBufLen( Ch );//��ȡ��ǰͨ��Э�̳���
    
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }

    if( (APPData[Ch].APPBufLen.w+LinkData[Ch].AddressLen+11) > ConsultLen )//�ж��Ƿ���Ҫ���÷�֡ + ֡ͷ֡β
    {
        APPData[Ch].APPFlag |= APP_LINK_FRAME;
    }
}

//--------------------------------------------------
//��������:  ����Ӧ��������֤����
//         
//����:    	Mode[in] 0x55: �ն� ��������վ
//			BYTE *pBuf[in]
//         
//����ֵ:	 	TRUE/FALSE
//         
//��ע����:  ��
//--------------------------------------------------
static WORD SecurityMechanismInfo( eConnectMode* ConnectModeMode, BYTE *pBuf )
{
	BYTE result;
	BYTE LenOffset,Len1Offest;
	TTwoByteUnion Len,Len1;
	if( pBuf[0] == 0 )
	{
	    ConnectMode = 0;//���潨��Ӧ�����ӵ�ģʽ
	    *ConnectModeMode = eConnectGeneral;//����һ��Ӧ������
		return TRUE;//��������������
	}	
	else if( pBuf[0] == 1 )
	{
		if( pBuf[1] != 8 )//һ���������ĳ��� ���볤�Ȳ��ܴ���7
        {
			return FALSE;
        }  
        
        ConnectMode = 1;//���潨��Ӧ�����ӵ�ģʽ
		*ConnectModeMode = eConnectGeneral;//����һ��Ӧ������

		return Judge698PassWord( (BYTE*)&pBuf[2] );
	}
	else if( pBuf[0] == 2 )//�ԳƼ���    [2] SymmetrySecurity��
	{

		if( (APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime == 0) //�����վ���ն˶�û����Ӧ�����ӽ��и�λ
		&& (APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime == 0) )//��������жϹ��ڸ��Ӳ����и�λ
		{
	   		api_ResetEsamSpi();//���ü���ǰ��λesam
		}

	    LenOffset = Deal_698DataLenth( (BYTE*)&pBuf[1], Len.b, ePROTOCOL_TYPE );
	    Len1Offest = Deal_698DataLenth( (BYTE*)&pBuf[1+LenOffset+Len.w], Len1.b, ePROTOCOL_TYPE );
        if( Len.w > sizeof(ProtocolBuf) )//��ֹ����Խ��
        {
        	return FALSE;
        }
        memcpy( ProtocolBuf, &pBuf[LenOffset+1],Len.w );// ����1        octet-string��
        if( Len1.w > ( sizeof(ProtocolBuf) - Len.w ) )//��ֹ����Խ��
        {
        	return FALSE;
        }
        memcpy( &ProtocolBuf[Len.w], &pBuf[(1+LenOffset+Len.w+Len1Offest)], Len1.w );// ����1        octet-string��

		result = api_ConnectMechanismInfo( *ConnectModeMode, ProtocolBuf );//*ConnectModeModeȡָ������ ����Ӧ������
		ConnectMode = 2;//���潨��Ӧ�����ӵ�ģʽ
		//���ú���
		//Э�����ӳɹ�����������²���
		return result;//TRUE; 
	}
	return FALSE;//FALSE;//�����ַ�ʽ		
}

//--------------------------------------------------
//��������:    698��������
//����:      BYTE Ch[in]          ����ͨ��
//         
//         BYTE *pAPDU[in]      apdu��bufָ��
//         
//         BYTE *pBuf[in]       ��Ӧbuf
//         
//����ֵ:     ��
//         
//��ע����:    ��
//--------------------------------------------------
static void DealConnect_Request(BYTE Ch,BYTE *pAPDU,BYTE *pBuf)
{
	WORD Result,EsamLen;
	DWORD dwData;
	BYTE *pbakbuf;
	pbakbuf = pBuf;
	TTwoByteUnion Len,BufLen;
	TFourByteUnion TmpTime,TmpTime1;
	BYTE Buf[20];
	eConnectMode eConnectMode;
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }

	APPData[Ch].APPFlag = APP_HAVE_DATA;
	EsamLen = 0x8000;

	if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL )//�ն˽���Ӧ������-һ���������ӻ���SecurityMechanismInfo�н����ж�-jwh
	{
		eConnectMode = eConnectTerminal;

	}
	else
	{
		eConnectMode = eConnectMaster;
	}
	
	memcpy( (void *)pBuf, (void *)pAPDU, 2);
	pBuf += 2;
	
	lib_ExchangeData(BufLen.b,pAPDU+30, 2);//ȡ�ͻ�������֡���ߴ�
    ConsultData.channel = Ch;
    ConsultData.Len = BufLen.w;
    
    if( ConsultData.Len >= MAX_PRO_BUF )//��������ռ䳤��ʹ����󳤶�
    {
        ConsultData.Len = MAX_PRO_BUF;
    }
    else
    {
        if( ConsultData.Len < MIN_CONSULT_BUF_LEN )//��Ϊͨ��Э�̳��Ȳ��ܵ���100������100ʹ��Ĭ�ϳ���
        {
            ConsultData.Len = GetDefaultConsultBufLen(Ch);
        }
    }
    
	lib_ExchangeData(TmpTime.b,pAPDU+35, 4);//ȡ��Чʱ��
	JTOverTime=TmpTime.d;
	//if( Ch == eIR )// ����ֻ������֤����ͨ���� ���ɽ���Ӧ������
	//{
    //    if( api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == TRUE )
    //    {
    //        Result = SecurityMechanismInfo( &eConnectMode , pAPDU+39 );//1+2+8+16+2+2+1+2+4   
    //    }
    //    else
    //    {	
    //        Result = FALSE;
    //    }
    //}
	//else
	{
        Result = SecurityMechanismInfo( &eConnectMode, pAPDU+39 );//1+2+8+16+2+2+1+2+4   
	}
	/*	
	FactoryVersion��=SEQUENCE
	{
	���̴���         visible-string(SIZE (4))��
	����汾��      visible-string(SIZE (4))��
	����汾����   visible-string(SIZE (6))��
	Ӳ���汾��      visible-string(SIZE (4))��
	Ӳ���汾����   visible-string(SIZE (6))��
	������չ��Ϣ   visible-string(SIZE (8))
	}
	*/
	dwData = 0x00030043;	//�ݴ�OAD 43000300
	//memset( (void *)pBuf, 0x00, 32);//���������̰汾��Ϣ
	api_GetProOadData( eGetNormalData, eData, 0xff, (BYTE *)&dwData, NULL, (4+4+6+4+6+8+4), pBuf );
	pBuf += 32;
	
	//pBuf[0] = pAPDU[2];//Э��汾�ŷ�����վ�·�����
	//pBuf[1] = pAPDU[3];
	//������Ҫ�����Ƿ��ر��Լ���Э��汾�ţ�����΢Ҳ���ص�����õ�
	api_ProcMeterTypePara( READ, eMeterProtocolVersion, pBuf );//698.45Э��汾��(��������:WORD)
	lib_InverseData( pBuf, 2 );
	pBuf += 2;
	
	memcpy( (void *)pBuf, (void *)&(pAPDU[4]), 24 );//Э��һ�����빦��һ���Է�����վ������ wlk
	pBuf += 24;
	//pBuf += AddProtocolAndFunctionConformance( 0, pBuf );//���Э��һ�����빦��һ����Buf
	
	Len.w= ConsultData.Len;//����������֡���ߴ�    long-unsigned
	pBuf[0] = Len.b[1];//Ӧ�ò��ȴ����ֽ�
	pBuf[1] = Len.b[0];
	pBuf += 2;
	
	Len.w = MAX_PRO_BUF;//����������֡���ߴ�    long-unsigned
	pBuf[0] = Len.b[1];//Ӧ�ò��ȴ����ֽ�
	pBuf[1] = Len.b[0];
	pBuf += 2;
	
	*(pBuf++) = 1;	//���������ߴ�֡����    unsigned
	
	Len.w = MAX_APDU_SIZE;//���������ɴ���apdu���ߴ�    long-unsigned
	pBuf[0] = Len.b[1];//Ӧ�ò��ȴ����ֽ�
	pBuf[1] = Len.b[0];
	pBuf += 2;
	
	//�̶���Ӧ�����ӳ�ʱʱ�� double-long-unsigned��
	if( pAPDU[39] == 0 )//��������
	{
		memcpy( (void *)pBuf,(void *)(pAPDU+35), 4);//�̶���Ӧ�����ӳ�ʱʱ�� double-long-unsigned��ȡ Connect-Request �е� ������Ӧ�����ӳ�ʱʱ�� double-long-unsigned
	}	
	else//����
	{	
		if( eConnectMode == eConnectTerminal )
		{
			EsamLen = api_GetEsamInfo( 0x14, Buf ); //��f1000500 ESAM���������5���ỰʱЧ����
		}
		else
		{
			EsamLen = api_GetEsamInfo( 5, Buf ); //��f1000500 ESAM���������5���ỰʱЧ����
		}
	}

	//��ȫ��֤��Ӧ����        SecurityResponseInfo	
	//*(pBuf++) = Result; //Ӧ������������֤���
	if( LinkData[Ch].AddressType!=eSINGLE_ADDRESS_TYPE )
	{
		memcpy( (void *)pBuf, (void *)(pAPDU+35), 4 );//��ӻỰӦ�����ӳ�ʱʱ��
	    pBuf += 4;	
        *(pBuf++) = 0xff;//��֤���      ConnectResult �� �Գƽ��ܴ���         ��2����
        *(pBuf++) = 0;//��������ʱ���ǩ����
	}	
	else if( ConnectMode == 0 )//��������Ӧ������
	{
        memcpy( (void *)pBuf, (void *)(pAPDU+35), 4 );//��ӻỰӦ�����ӳ�ʱʱ��
        pBuf += 4;  
        *(pBuf++) = 0;//��֤���      ConnectResult �� �Գƽ��ܴ���         ��2����
        *(pBuf++) = 0;
	}
	else if( (Result == FALSE) || ( APPData[Ch].TimeTagFlag == eTime_Invalid) )
	{
	    memcpy( (void *)pBuf, (void *)(pAPDU+35), 4 );//��ӻỰӦ�����ӳ�ʱʱ��
	    pBuf += 4;	

	    if( Result == FALSE )
	    {
            if(pAPDU[39] == 2 )//�ԳƼ���    [2] SymmetrySecurity��
            {
                *(pBuf++) = 02;//��֤���      ConnectResult �� �Գƽ��ܴ���         ��2����
                *(pBuf++) = 0;
            }
            else
            {
                *(pBuf++) = 01;//��֤���      ConnectResult �� �������             ��1����
                *(pBuf++) = 0;
            }
	    }
	    else
	    {
            *(pBuf++) = 0xff;//��֤���      ConnectResult �� �Գƽ��ܴ���         ��2����
            *(pBuf++) = 0;//��������ʱ���ǩ����
	    }
	}
	else
	{	
		#if( SEL_DLT645_2007 == YES )
		api_Release645_Auth();//�Ͽ�645�����֤-һ�����뽨��Ӧ������Ҳ��Ͽ�
		#endif
		api_SaveProgramRecord( EVENT_END, 0x51, (BYTE *)NULL);
		
		if( eConnectMode > eConnectModeMaxNum )//����ֵ�ж� Ĭ��Ϊ��վ
		{
			eConnectMode = eConnectMaster;

		}
		
		if( EsamLen == 0x8000 ) //��ȡ����Ӧ��������Чʱ��
		{
			APPConnect.ConnectInfo[eConnectMode].ConnectValidTime = TmpTime.d;
		}
		else
		{
			lib_ExchangeData(TmpTime1.b, Buf, 4);
			TmpTime1.d = TmpTime1.d*60;//��λ����,ת��Ϊ��
			if( TmpTime.d > TmpTime1.d )
			{
				APPConnect.ConnectInfo[eConnectMode].ConnectValidTime = TmpTime1.d;
			}
			else
			{
				APPConnect.ConnectInfo[eConnectMode].ConnectValidTime = TmpTime.d;
			}
		}
		
        if( eConnectMode != eConnectGeneral )//����һ������ ����һ��������֤ʱ��--����ģʽ�ɹ���
        {
            APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 0;
            APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 0;

        }
        
		if( eConnectMode == eConnectGeneral )
		{
			api_SetSysStatus(eSYS_STATUS_ID_698GENERAL_AUTH);
		}
		else if( eConnectMode == eConnectTerminal )
		{
			api_SetSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
			api_ClrSysStatus( eSYS_TERMINAL_ALLOW_PRG );		//20�淶Ҫ����ߴ���һ��
			APPConnect.TerminalAuthTime = 1;//1s��ر��ն���֤
		}
		else
		{
			api_SetSysStatus(eSYS_STATUS_ID_698MASTER_AUTH);
		}

		ClientAddress = LinkData[Ch].ClientAddress;
		APPConnect.ConnectInfo[eConnectMode].ConstConnectValidTime = APPConnect.ConnectInfo[eConnectMode].ConnectValidTime ;
		
		lib_ExchangeData( pBuf, (BYTE*)&APPConnect.ConnectInfo[eConnectMode].ConstConnectValidTime, 4 );//��ӻỰӦ�����ӳ�ʱʱ��
	    pBuf += 4;
	    
		*(pBuf++) = 0;// ConnectResult��=ENUMERATED {  ������Ӧ������     ��0����
		if(pAPDU[39] == 2 )//�Գ����� //�ԳƼ���    [2] SymmetrySecurity��
		{
			*(pBuf++) = 0x01;//ѡ������
			lib_ExchangeData(Len.b, ProtocolBuf, 2 );
			if( (Len.w > 0xff) || (Len.w < 4) )//����ֵ����
			{
				Len.w = 52;
			}
			
			*(pBuf++) = (Len.w-4);//RN����
			memcpy( (void *)pBuf, &ProtocolBuf[2],Len.w-4);//�ϱ�esam��֤���� data
			pBuf += (Len.w-4);

			*(pBuf++) = 4;//ǩ������
			memcpy( (void *)pBuf, &ProtocolBuf[2+(Len.w-4)],4);//�ϱ�MAC 
			pBuf += 4;
		}
		else
		{
		    //api_ResetEsamSpi( );//һ�����븴λһ��esam��ʱ��Կ���ָ���
			*(pBuf++) = 0;//�ǶԳƼ���û�С���֤������Ϣ  SecurityData  OPTIONAL��
			//�������˵һ�����뱣���Ժ��ã����ڲ���
			//APPConnect.ConnectValidTime = TmpTime.d;
		}		
	}

	APPData[Ch].APPBufLen.w = (WORD)(pBuf - pbakbuf);
    APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
	APPData[Ch].APPFlag = APP_HAVE_DATA;
	APPData[Ch].BufDealLen.w = LinkData[Ch].pAPPLen.w;
}
//--------------------------------------------------
//��������:  698���ӶϿ�����
//����:     BYTE Ch[in]
//         
//����ֵ:    void�޷���ֵ 
//         
//��ע����:  ��
//--------------------------------------------------
static void DealRelease_Request(BYTE Ch)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,2);					//�������ͷ
	DLT698AddOneBuf(Ch,0x55,0 );							    //��ӽ�� �ɹ�
	
	if( CURR_COMM_TYPE(Ch) ==  COMM_TYPE_TERMINAL )
	{
		api_ClrSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH );
		APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime = 1;
		APPConnect.ConnectInfo[eConnectTerminal].ConstConnectValidTime = 1;//1s��ر��ն�Э��
	}
	else
	{
		api_ClrSysStatus( eSYS_STATUS_ID_698MASTER_AUTH );
		APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime = 1;
		APPConnect.ConnectInfo[eConnectMaster].ConstConnectValidTime = 1;//1s��ر��ն�Э��
	}

	api_ClrSysStatus( eSYS_STATUS_ID_698GENERAL_AUTH );
	
	APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 1;
	APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 1;//1s��ر�һ������ ��κ���������ۿͻ���Ϊʲô���ر�һ������
	
	APPData[Ch].APPFlag = APP_HAVE_DATA;
	APPData[Ch].APPBufLen.w = 4;
	APPData[Ch].BufDealLen.w = LinkData[Ch].pAPPLen.w;
}

//--------------------------------------------------
//��������:    ��ȡ�������״̬��1
//         
//����:      BYTE* Buf[in]
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
void GetMeterStatus1( BYTE* Buf)
{
	/**********************����************************/
	//bit15	ʱ�ӹ���
	//bit14	͸֧״̬
	//bit13	�洢�����ϻ���
	//bit12	�ڲ��������
	//bit11	����
	//bit10	����
	//bit9	esam����
	//bit8	���ƻ�·����
	//bit7	����
	//bit6	����оƬ����
	//bit5	�޹����ʷ���( 0 ����1����)
	//bit4	�й����ʷ���( 0 ����1����)
	//bit3	ͣ�糭����( 0 ����, 1Ƿѹ)
	//bit2	ʱ�ӵ��( 0 ����, 1Ƿѹ)
	//bit1	�������㷽ʽ( 0 ����, 1 ����)
	//bit0	����
	/**********************���ֽ�************************/
	if( ( api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )					//BIT7	ʱ�ӹ���	
	|| ( api_GetRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG) == TRUE))
	{
		Buf[0] |= BIT7;
	}

	if( api_GetSysStatus( eSYS_STATUS_PRE_USE_MONEY ) == TRUE )	//BIT6	͸֧״̬	
	{
		Buf[0] |= BIT6;
	}

    if((api_CheckError(ERR_WRITE_EEPROM1) == TRUE)//e2     
    ||(api_CheckError(ERR_WRITE_EEPROM2) == TRUE)       //BIT5	�洢�����ϻ���	
    ||(api_CheckError(ERR_WRITE_EEPROM3) == TRUE))      //BIT4	�ڲ��������	 											     	
    {
       Buf[0] |= BIT5;
    }
                                                            //BIT4	�ڲ��������
															//BIT3	����			
															//BIT2	����			
	if( api_GetRunHardFlag( eRUN_HARD_ESAM_ERR ) == TRUE )		//BIT1	esam����		
	{
		Buf[0] |= BIT1;
	}	

	if( api_GetSysStatus( eSYS_STATUS_RELAY_ERR ) == TRUE )		//BIT0	���ƻ�·����		
	{
		Buf[0] |= BIT0;
	}	
	/**********************���ֽ�************************/
															//BIT7	����			
	if( api_CheckError( ERR_CHECK_5460_1 ) == TRUE )		//BIT6	����оƬ����		
	{
		Buf[1] |= BIT6;
	}
	
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q ) == TRUE )		//BIT5	�޹����ʷ���( 0 ����1����)		
	{
		Buf[1] |= BIT5;
	}	

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P ) == TRUE )		//BIT4	�й����ʷ���( 0 ����1����)		
	{
		Buf[1] |= BIT4;
	}

	//if( api_GetRunHardFlag( eRUN_HARD_READ_BAT_LOW ) == TRUE )	//BIT3	ͣ�糭����( 0 ����, 1Ƿѹ)		
	//{
	//	Buf[1] |= BIT3;
	//}

	if( api_GetRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW ) == TRUE )	//BIT2	ʱ�ӵ��( 0 ����, 1Ƿѹ)			
	{
		Buf[1] |= BIT2;
	}

															//BIT1	�������㷽ʽ( 0 ����, 1 ����)		
															//BIT0	����		
}

//--------------------------------------------------
//��������:    ��ȡ�������״̬��2
//         
//����:      BYTE* Buf[in]
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
void GetMeterStatus2( BYTE *Buf)
{
	/**********************����************************/
	//bit15	����
	//bit14	����
	//bit13	����
	//bit12	����
	//bit11	����
	//bit10	����
	//bit9	����
	//bit8	����
	//bit7	����
	//bit6	C���޹����ʷ���( 0 ����1����)
	//bit5	B���޹����ʷ���( 0 ����1����)
	//bit4	A���޹����ʷ���( 0 ����1����)
	//bit3	����
	//bit2	C���й����ʷ���( 0 ����1����)
	//bit1	B���й����ʷ���( 0 ����1����)
	//bit0	A���й����ʷ���( 0 ����1����)
	/**********************���ֽ�************************/
	
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q_C ) == TRUE )		//BIT6	C���޹����ʷ���( 0 ����1����)		
	{
		Buf[1] |= BIT6;
	}	

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q_B ) == TRUE )		//BIT5	B���޹����ʷ���( 0 ����1����)		
	{
		Buf[1] |= BIT5;
	}

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_Q_A ) == TRUE )		//BIT4	A���޹����ʷ���( 0 ����1����)		
	{
		Buf[1] |= BIT4;
	}
															//bit3	����
	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_C ) == TRUE )		//BIT2	C���޹����ʷ���( 0 ����1����)		
	{
		Buf[1] |= BIT2;
	}	

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_B ) == TRUE )		//BIT1	BIT1B���޹����ʷ���( 0 ����1����)		
	{
		Buf[1] |= BIT1;
	}

	if( api_GetSysStatus( eSYS_STATUS_OPPOSITE_P_A ) == TRUE )		//BIT0	A���޹����ʷ���( 0 ����1����)		
	{
		Buf[1] |= BIT0;
	}
}

//--------------------------------------------------
//��������:  ��ȡ�������״̬��3
//         
//����:     BYTE* Buf[in]
//         
//����ֵ:    ��  
//         
//��ע����:  ��
//--------------------------------------------------
void GetMeterStatus3( ePROTOCO_TYPE ProtocolType, BYTE *Buf)
{	
    BYTE DataBuf[4];
    
    memset( Buf, 0x00, 2 );
	//���ֽ�
	//bit1//bit0	���ܱ����� 00 ��Ԥ���ѱ� 01������Ԥ���ѱ� 10�����Ԥ���ѱ�
	if( PREPAY_MODE == PREPAY_LOCAL )
	{	
		Buf[0] |= BIT1; 
	}
	//bit7		Զ�̿���(0����,1δ����)
	//bit6		���ؿ���(0����,1δ����)
    #if( PREPAY_MODE == PREPAY_LOCAL )
    Buf[0] |= BIT6+BIT7;
    if( api_ReadPrePayPara( eLocalAccountFlag, DataBuf ) != 0 )
    {
        if( DataBuf[0] == eOpenAccount )//bit6		���ؿ���(0����,1δ����)			//bit14
		{
			Buf[0] &= ~BIT6;
		}
    }
	if( api_ReadPrePayPara( eRemoteAccountFlag, DataBuf ) != 0 )
    {
        if( DataBuf[0] == eOpenAccount )//bit6		Զ�̿���(0����,1δ����)			//bit14
		{
			Buf[0] &= ~BIT7;
		}
    }
    #endif//#if( PREPAY_MODE == PREPAY_LOCAL )
    
	if( ProtocolType == ePROTOCO_645 ) //645Э���ȡ
	{
    	if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE)//bit5		��ȫ��֤״̬(0ʧЧ,1��Ч)
    	{
    		Buf[0] |= BIT5;
    	}
	}
	else//698Э���ȡ
	{
    	if( api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE )//bit5 ��վ��ȫ��֤״̬(0ʧЧ,1��Ч)
    	{
    		Buf[0] |= BIT5;
    	}

    	if( api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE)//bit3 �ն˰�ȫ��֤״̬(0ʧЧ,1��Ч)
    	{
    		Buf[0] |= BIT3;
    	}
	}

	if( api_GetSysStatus( eSYS_TERMINAL_ALLOW_PRG ) == TRUE )//bit2 �ն������֤״̬
	{
		Buf[0] |= BIT2;
	}

	if( RelayTypeConst != RELAY_NO )
	{
		if( api_GetRelayStatus( 2 ) == TRUE )    //bit4		����״̬(0�Ǳ���,1����)
		{
			Buf[0] |= BIT4;
		}
												//bit3		����
												//bit2		����
		
		#if( PREPAY_MODE == PREPAY_LOCAL )
		if( api_GetRelayStatus(7) != 0 )		//bit6		�̵�������״̬(0ͨ��1��)
		{
			Buf[1] |= BIT6;
		}
		#else
		if( api_GetRelayStatus(0) != 0 )		//bit6		�̵�������״̬(0ͨ��1��)
		{
			Buf[1] |= BIT6;
		}
		#endif

												//bit5		����
		if( api_GetRelayStatus(1) != 0 )		//bit4		�̵���ʵ��״̬(0ͨ��1��)
		{
			Buf[1] |= BIT4;
		}
	}
	
	//���ֽ�
	if( api_GetRelayStatus(3) == TRUE )			//bit7		Ԥ��բ����״̬(0�ޣ�1��)
	{
		Buf[1] |= BIT7;
	}

	if( api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == TRUE )			//bit3		�������״̬(0ʧЧ,1��Ч)	
	{
		Buf[1] |= BIT3;
	}
	
    if( SelSecPowerConst==NO )
    {
        if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) != TRUE )			//bit2//bit1	���緽ʽ(00����Դ,02������Դ��01��ع���)	
    	{
    		Buf[1] |= BIT1;
    	}
    }
    else
    {
        if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) != TRUE )			//bit2//bit1	���緽ʽ(00����Դ,01������Դ��10��ع���)	
    	{
    		Buf[1] |= BIT2;
    	}
    	else
    	{
    	    #if( SEL_EVENT_LOST_SECPOWER == YES )//36
        	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_SECPOWER ) == 0 )	//������Դʧ��
    		{
    			Buf[1] |= BIT1;
    		}
    		#endif
    	}
    }
}

//--------------------------------------------------
//��������:  ��ȡ�������״̬��4,5,6
//         
//����:     BYTE Phase[in] 0:A��, 1:B��, 2:C��
//         
//        BYTE* Buf[in]
//         
//����ֵ:    ��  
//         
//��ע����:  ��
//--------------------------------------------------
void GetMeterStatus4_5_6( BYTE Phase,BYTE *Buf)
{
#if( MAX_PHASE == 1 )
    if( Phase > 0 )
    {
        return;
    }
#endif
    /**********************����************************/
	//bit15		����
	//bit14		����
	//bit13		����
	//bit12		����
	//bit11		����
	//bit10		����
	//bit9		����
	//bit8		����
	//bit7		����
	//bit6		���ʷ���
	//bit5		����
	//bit4		����
	//bit3		ʧ��
	//bit2		��ѹ
	//bit1		Ƿѹ
	//bit0		ʧѹ
#if( SEL_EVENT_BREAK_I == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_BREAK_I_A+Phase) == 1 )		//bit1		A����
	{
		Buf[0] |= BIT0;
	}
#endif
	/**********************���ֽ�************************/
#if( SEL_EVENT_BREAK == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_BREAK_A+Phase) == 1 )		//bit7		A����
	{
		Buf[1] |= BIT7;
	}
#endif

#if( SEL_EVENT_BACKPROP == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_POW_BACK_A+Phase) == 1 )	//bit6		A���ʷ���
	{
		Buf[1] |= BIT6;
	}
#endif			

#if( SEL_EVENT_OVERLOAD == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_OVERLOAD_A+Phase) == 1 )	//bit5		A����
	{
		Buf[1] |= BIT5;
	}
#endif	

#if( SEL_EVENT_OVER_I == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_OVER_I_A+Phase) == 1 )		//bit4		A����
	{
		Buf[1] |= BIT4;
	}
#endif	

#if( SEL_EVENT_LOST_I == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOSTI_A+Phase) == 1 )		//bit3		Aʧ��
	{
		Buf[1] |= BIT3;
	}
#endif

#if( SEL_EVENT_OVER_V == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_OVER_V_A+Phase) == 1 )		//bit2		A��ѹ
	{
		Buf[1] |= BIT2;
	}
#endif

#if( SEL_EVENT_WEAK_V == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_WEAK_V_A+Phase) == 1 )		//bit1		AǷѹ
	{
		Buf[1] |= BIT1;
	}
#endif

#if( SEL_EVENT_LOST_V == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOSTV_A+Phase) == 1 )		//bit0		Aʧѹ
	{
		Buf[1] |= BIT0;
	}
#endif
}

//--------------------------------------------------
//��������:    ��ȡ�������״̬��7
//         
//����:      BYTE* Buf[in]
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
void GetMeterStatus7( BYTE *Buf )
{
    BYTE i;
	/**********************����************************/
	//bit15		����
	//bit14		����
	//bit13		����
	//bit12		����
	//bit11		����
	//bit10		����ť��
	//bit9		�����
	//bit8		�������ز�ƽ��
	//bit7		�ܹ�������������
	//bit6		��������
	//bit5		����
	//bit4		������Դʧ��
	//bit3		������ƽ��
	//bit2		��ѹ��ƽ��
	//bit1		����������
	//bit0		��ѹ������
	/**********************���ֽ�************************/
#if( SEL_EVENT_BUTTONCOVER == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_BUTTON_COVER ) == 1 )	//bit2		����ť��
	{
		Buf[0] |= BIT2;
	}
#endif

#if( SEL_EVENT_METERCOVER == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_METER_COVER ) == 1 )	//bit1		�����
	{
		Buf[0] |= BIT1;
	}
#endif

#if( SEL_EVENT_I_S_UNBALANCE == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_S_I_UNBALANCE ) == 1 )	//bit0		�������ز�ƽ��
	{
		Buf[0] |= BIT0;
	}
#endif

#if( SEL_EVENT_COS_OVER == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_COS_OVER_ALL ) == 1 )	//bit7		�ܹ�������������
	{
		Buf[1] |= BIT7;
	}
#endif

#if( SEL_DEMAND_OVER == YES )
    for( i=0; i < 6; i++ )//ֻҪ��һ���������޾���Ϊ��������
    {
        if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_PA_DEMAND_OVER+i ) == 1 )    //bit6      ��������
        {
            Buf[1] |= BIT6;
            break;
        }
    }
#endif

#if( SEL_EVENT_LOST_POWER == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER ) == 1 )	//bit5		����
	{
		Buf[1] |= BIT5;
	}
#endif

#if( SEL_EVENT_LOST_SECPOWER == YES )
	if( SelSecPowerConst == YES )
	{
		if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_SECPOWER ) == 1 )	//bit4		������Դʧ��
		{
			Buf[1] |= BIT4;
		}
	}
#endif

#if( SEL_EVENT_I_UNBALANCE == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_I_UNBALANCE ) == 1 )		//bit3		������ƽ��
	{
		Buf[1] |= BIT3;
	}
#endif

#if( SEL_EVENT_V_UNBALANCE == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_V_UNBALANCE ) == 1 )	//bit2		��ѹ��ƽ��
	{
		Buf[1] |= BIT2;
	}
#endif

#if( SEL_EVENT_I_REVERSAL == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_I_REVERSAL ) == 1 )		//bit1		����������
	{
		Buf[1] |= BIT1;
	}
#endif

#if( SEL_EVENT_V_REVERSAL == YES )
	if( api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_V_REVERSAL ) == 1 )		//bit0		��ѹ������
	{
		Buf[1] |= BIT0;
	}
#endif
}
//--------------------------------------------------
//��������:    ��ȡ�������״̬��
//         
//����:      BYTE Type[in]   Type 0 ȫ���������״̬��  
//         		           Type 1 ~ 7 ����������״̬�� 1 ~ 7
//         BYTE BufLen[in]
//         
//         BYTE Buf[in]    ÿ�����ֽڴ���1�� Buf[0] ����bit15 ~ bit8�� Buf[1] ����bit8 ~ bit0
//         
//����ֵ:     WORD  0x8000    ������ִ��� 0x0000����buf���Ȳ��� ���������������س���
//         
//��ע����:    ��
//--------------------------------------------------
WORD api_GetMeterStatus( ePROTOCO_TYPE ProtocolType, BYTE Type, WORD BufLen, BYTE *Buf )
{
	WORD Len;

	if( Type > 7 )
	{
		return 0x8000;
	}
	
	if( ( Type ==0 ) && ( BufLen < 14) )
	{		
		return 0;
	}
	else if( BufLen < 2)
	{
		return 0;
	}
	
	if( BufLen < 14 )
	{
		memset( Buf,0x00,BufLen);	//��Buf�����������
	}
	else
	{
		memset( Buf,0x00,14);		//��Buf�����������
	}	
	
	switch( Type )
	{
		case 0://�������״̬��
			GetMeterStatus1( Buf );
			GetMeterStatus2( Buf+2 );
			GetMeterStatus3( ProtocolType, Buf + 4 );
			GetMeterStatus4_5_6(0,Buf +6);
			GetMeterStatus4_5_6(1,Buf +8);
			GetMeterStatus4_5_6(2,Buf +10);
			GetMeterStatus7( Buf+12 );
			Len = 14;
			break;
		case 1://�������״̬��1
			GetMeterStatus1( Buf );	
			Len = 2;
			break;			
		case 2://�������״̬��2
			GetMeterStatus2( Buf );
			Len = 2;
			break;			
		case 3://�������״̬��3
			GetMeterStatus3( ProtocolType, Buf );
			Len = 2;
			break;	
		case 4://�������״̬��4
		case 5://�������״̬��5
		case 6://�������״̬��6
			GetMeterStatus4_5_6( (Type -4) ,Buf );
			Len = 2;
			break;	
		case 7://�������״̬��7
			GetMeterStatus7(Buf);
			Len = 2;
			break;
	}

    return Len;
}
//--------------------------------------------------
//��������:    ��ȡNormal�������
//         
//����:      BYTE Ch[in]   ͨ��ѡ��
//         
//         WORD Len[in]  ���ݳ���
//         
//����ֵ:     eAPPBufResultChoice ����buf��ӽ��
//         
//��ע����:    ��
//--------------------------------------------------
static eAPPBufResultChoice DealGetRequestNormalResult( BYTE Ch,WORD Len)
{
	eAPPBufResultChoice eResultChoice;
	
	if( (!(Len & 0x8000)) && ( Len!= 0) )//���س���bit7��Ϊ1
	{
		eResultChoice = DLT698AddOneBuf(Ch,0, 1);//��Ӧ����   CHOICE  {     ������Ϣ     [0] DAR��   M����¼     [1] SEQUENCE OF A-RecordRow}
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		eResultChoice = DLT698AddBufLen( Ch, 0x55, Len);
	}
	else//��ʾ�ϲ���з�֡
	{
		if( Len == 0)//buf���Ȳ���
		{
			eResultChoice = DLT698AddBufDAR( Ch, DAR_OverRegion );
		}
		else
		{
			eResultChoice = DLT698AddBufDAR( Ch, DAR_Undefined );
		}	
	}
	
	return eResultChoice;
}

//--------------------------------------------------
//��������:    ��ȡNormal����
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OADָ��
//         
//����ֵ:     eAPPBufResultChoice ����buf��ӽ��
//         
//��ע����:    ��
//--------------------------------------------------
static eAPPBufResultChoice GetRequestNormal( BYTE Ch,BYTE *pOAD)
{
	WORD Len;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion OI;

	lib_ExchangeData(OI.b,pOAD,2);
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
	eResultChoice = DLT698AddBuf(Ch,0, pOAD, 4);//���OAD���� ������������
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    APPData[Ch].BufDealLen.w +=4;//�Ѵ���4�ֽ�oad

    if( APPData[Ch].TimeTagFlag == eTime_Invalid )
    {
		eResultChoice = DLT698AddBufDAR( Ch,DAR_TimeStamp );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
    }
	else if(JudgeTaskAuthority( Ch, eREAD_MODE, pOAD)== FALSE )//�жϰ�ȫģʽ����
	{
		eResultChoice = DLT698AddBufDAR( Ch,DAR_PassWord );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
	}
	else
	{
		if( (APPData[Ch].APPMaxBufLen.w <= (APPData[Ch].APPCurrentBufLen.w+1)) //���м���ֵ�ж�
	    ||(APPData[Ch].APPMaxBufLen.w > MAX_APDU_SIZE))
		{
            Len = 0;
		}
		else
		{
			Len = GetProOadData(eGetNormalData, Ch, 1, 0XFF, pOAD, (BYTE *)NULL, (APPData[Ch].APPMaxBufLen.w - APPData[Ch].APPCurrentBufLen.w - 1),
			                    &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w + 1]);   // �������buf
		}

		eResultChoice = DealGetRequestNormalResult( Ch, Len);
	}
	
	return eResultChoice;
}

//--------------------------------------------------
//��������:    ��ȡNormalList����
//         
//����:      BYTE Ch[in]     ͨ��ѡ��   
//         
//����ֵ:     eAPPBufResultChoice ����buf��ӽ��
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice GetRequestNormalList( BYTE Ch)
{	
	WORD i,NoAddress,LenOffest;
    TTwoByteUnion Num,No;
	eAPPBufResultChoice eResultChoice;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }

    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
	APPData[Ch].BufDealLen.w +=LenOffest;
	
	No.w = 0;
	
	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//��Ӹ��� ��������֡����ȷ
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    NoAddress = APPData[Ch].APPCurrentBufLen.w;
    
	for(i = 0;i < Num.w;i++)//�����ȡ��OAD̫�࣬Ҫ�ú���֡ for debug
	{
		eResultChoice = GetRequestNormal( Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);

        if( eResultChoice != eADD_OK) 
		{
			break;
		}
		
		No.w++;//sequence of  a_result_normal ��1	
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//�Ѵ������ݴ����·������ݽ��з��أ���������ĵĸ���
		{
			break;
		}
	}	
	
    JudgeAppBufLen(Ch);

    Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
    
	return eResultChoice;
}

//--------------------------------------------------
//��������:     ��Լ��ȡ��¼����
//         
//����:       BYTE Tag[in] 						 �Ƿ��TAG��־;	0x02������Ҫ��ʾ��Ҫ��ȡ�� 0x01������Ҫ��TAG�� 	   	  0x00 ������TAG�� 
//							       
//          TDLT698RecordPara *DLT698RecordPara[in] ��ȡ��¼���¼��Ĳ����ṹ��
//         
//          WORD BufLen[in]				     �����buf���� �����ж�buf�����Ƿ���
//         
//          BYTE *pBuf[in]						 ����bufָ��
//         
//����ֵ:      WORD								 bit15λ��1 �����¼�򶳽��޴����� ��0������������ȷ���أ� 
//											     bit0~bit14 �����صĶ�����¼����ֽ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD ReadProRecordData( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD BufLen,BYTE *pBuf )
{
	WORD Len;
	Len = 0x8000;
	
	if( ( (pDLT698RecordPara->OI >= 0x3000) && (pDLT698RecordPara->OI <= 0x310f) )
    #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
    || (pDLT698RecordPara->OI == 0x3111) //����δ֪���ܱ��¼�
    #endif
    #if (SEL_TOPOLOGY == YES )
    || (pDLT698RecordPara->OI == 0x3E03) //���������ź�ʶ�����¼�
    #endif
	)//�����¼�����
	{
	    if( ((pDLT698RecordPara->Phase == ePHASE_B) && (MeterTypesConst == METER_3P3W)) 
	    && (pDLT698RecordPara->OI != 0x300b) )//�������߱��0x300b�޹����������ⲻ֧��B��
	    {
            Len = 0x8000;
	    }
	    else
	    {
            Len = api_ReadEventRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
	    }
	}
	else if((pDLT698RecordPara->OI >= 0x5000) && (pDLT698RecordPara->OI <= 0x5011))//���ö��ắ��
	{
		Len = api_ReadFreezeRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
	}
	else if ((pDLT698RecordPara->OI >= 0xBD00) && (pDLT698RecordPara->OI <= 0xBD09))
	{
		Len = api_ReadEventRecord( Tag, pDLT698RecordPara, BufLen, pBuf);
	}

	return Len;//�������ݶ��󲻴���
}

//--------------------------------------------------
//��������:     ��Լ��ȡ��¼����
//         
//����:       BYTE Tag[in] 						 �Ƿ��TAG��־;	0x02������Ҫ��ʾ��Ҫ��ȡ�� 0x01������Ҫ��TAG�� 	   	  0x00 ������TAG�� 
//							       
//          TDLT698RecordPara *DLT698RecordPara[in] ��ȡ��¼���¼��Ĳ����ṹ��
//         
//          WORD BufLen[in]				     �����buf���� �����ж�buf�����Ƿ���
//         
//          BYTE *pBuf[in]						 ����bufָ��
//         
//����ֵ:      WORD								 bit15λ��1 �����¼�򶳽��޴����� ��0������������ȷ���أ� 
//											     bit0~bit14 �����صĶ�����¼����ֽ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD api_ReadProRecordData( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD BufLen,BYTE *pBuf )
{	
    const DWORD (* pEvent)[2];
	TFourByteUnion OAD;
	BYTE i;

	if( Tag == 2 )//��ʾ��ȡ 
	{
		if( (((pDLT698RecordPara->OI) >= 0x3000) && ((pDLT698RecordPara->OI) < 0x3030) && ((pDLT698RecordPara->OI) != 0x300C))
		  ||((pDLT698RecordPara->OI) == 0x303B)||((pDLT698RecordPara->OI) == 0x303C)  ||((pDLT698RecordPara->OI) == 0x3040)
		  ||((pDLT698RecordPara->OI) == 0x30E0)||((pDLT698RecordPara->OI) == 0x30E1)||((pDLT698RecordPara->OI) == 0x30E2)
		  ||((pDLT698RecordPara->OI) == 0x3053)||((pDLT698RecordPara->OI) == 0x310f)
		  ||((pDLT698RecordPara->OI) == 0x3104) 
          #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
          || (pDLT698RecordPara->OI == 0x3111) //����δ֪���ܱ��¼�
          #endif
          #if (SEL_TOPOLOGY == YES )
          || (pDLT698RecordPara->OI == 0x3E03) //���������ź�ʶ�����¼�
          #endif
		  ) //�¼�RCSD 		
		  {
			if ((pDLT698RecordPara->OI) == 0x303B)
			{
				pEvent = pEventUnit[0x30];//��ȡ��Ӧ���¼���¼��
			}
			else if((pDLT698RecordPara->OI) == 0x303C)
			{
				pEvent = pEventUnit[0x31];//��ȡ��Ӧ���¼���¼��
			}
			else if ((pDLT698RecordPara->OI) == 0x3040)
			{
				pEvent = pEventUnit[0x32];//��ȡ��Ӧ���¼���¼��
			}
			else if ((pDLT698RecordPara->OI) ==0x30E0)
			{
				pEvent=ChargingWARNINGUnit;
			}
			else if ((pDLT698RecordPara->OI) ==0x30E1)
			{
				pEvent=ChargingERRUnit;
			}
			else if ((pDLT698RecordPara->OI) ==0x30E2)
			{
				pEvent=ChargingCOM_EXCTIONUnit;
			}
			else if(((pDLT698RecordPara->OI) == 0x3053) || ((pDLT698RecordPara->OI) == 0x310f))
			{
				pEvent = StandardEventUnit;
			}
			else if ((pDLT698RecordPara->OI) == 0x3104)
			{
				pEvent = pEventUnit[0x33];//��ȡ��Ӧ���¼���¼��
			}
            #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
            else if( (pDLT698RecordPara->OI == 0x3111) )//����δ֪���ܱ��¼�
            {
                pEvent = FindUnknownMeterUnit;
            }
            #endif
            #if (SEL_TOPOLOGY == YES )
            else if( (pDLT698RecordPara->OI == 0x3E03) )//���������ź�ʶ�����¼�
            {
                pEvent = TopoUnit;
            }
            #endif
			else
			{
				pEvent = pEventUnit[(pDLT698RecordPara->OI)-0x3000];//��ȡ��Ӧ���¼���¼��
			}

			if( (pDLT698RecordPara->OADNum) != 1 )
			{
				return 0x8000;
			}

			memcpy( OAD.b,pDLT698RecordPara->pOAD,4);
			for( i = 0; i < pEvent[0][0]; i++ )
			{
				if( OAD.d == pEvent[i+1][0] )//���
				{
					memcpy(pDLT698RecordPara->pOAD,&pEvent[i+1][1],4);//��֡RCSD
					break;
				}
			
			}
		}
		Tag = 0;//TAG�óɲ���Ҫ��TAG��
	}
	
    pDLT698RecordPara->Ch = 0x55;//�ⲿ����ʱ ͳһ��Ϊ0x55 ����Ӱ������ϱ�
    
    return ReadProRecordData( Tag, pDLT698RecordPara, BufLen, pBuf);

}

//--------------------------------------------------
//��������:    ����RCSDΪ��ʱ���ݴ���
//         
//����:      BYTE Ch[in]   ͨ��ѡ��
//         
//         DWORD OI[in]  ��¼OI
//
//         BYTE *DAR[out] ����DAR
//         
//����ֵ:     eAPPBufResultChoice  �������BUF���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice DealRCSDIsZeroData( BYTE Ch, WORD OI, BYTE *DAR)
{
	WORD Len;
	BYTE Num;
	BYTE *pRCSDNum;
	WORD i;
	eAPPBufResultChoice eResultChoice;
	const DWORD (*pEvent)[2];
	
	pRCSDNum = &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w];//ȡ��rcsd������ָ��
	eResultChoice = DLT698AddOneBuf( Ch, 0, 1);				//���rcsd�� ����
	
	if( ( (OI >= 0x3000) && ( OI < 0x3030) && ( OI != 0x300C))	
	|| (OI == 0x303B) || (OI == 0x303C) || ( OI == 0x3040)|| ( OI == 0x30E0)
	|| ( OI == 0x30E1)|| ( OI == 0x30E2)||((OI >= 0xBD00)&&(OI<=0xBD09) )
	|| ( OI == 0x3053)|| ( OI == 0x310f)|| ( OI == 0x3104)
    #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
    || (OI == 0x3111) //����δ֪���ܱ��¼�
    #endif
    #if (SEL_TOPOLOGY == YES )
    || (OI == 0x3E03) //���������ź�ʶ�����¼�
    #endif
	)//�¼�RCSD 
	{
		if(OI == 0x303B)
		{
			pEvent = pEventUnit[0x30];		//��ȡ��Ӧ���¼���¼��
		}
		else if(OI == 0x303C)
		{
			pEvent = pEventUnit[0x31];		//��ȡ��Ӧ���¼���¼��
		}
		else if( OI == 0x3040)
		{
			pEvent = pEventUnit[0x32];		//��ȡ��Ӧ���¼���¼��
		}
		else if (OI ==0x30E0)
		{
			pEvent=ChargingWARNINGUnit;
		}
		else if (OI ==0x30E1)
		{
			pEvent=ChargingERRUnit;
		}
		else if (OI ==0x30E2)
		{
			pEvent=ChargingCOM_EXCTIONUnit;
		}
		else if( OI == 0x3104)
		{
			pEvent = pEventUnit[0x33];		//��ȡ��Ӧ���¼���¼��
		}
		else if ((OI >= 0xBD00)&&(OI<=0xBD09))
		{
			pEvent = ModuleEventUnit;
		}
		else if((OI == 0x3053) || (OI == 0x310f))
		{
			pEvent = StandardEventUnit;
		}
        #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
        else if(OI == 0x3111) //����δ֪���ܱ��¼�
        {
            pEvent = FindUnknownMeterUnit;
        }
        #endif
        #if (SEL_TOPOLOGY == YES )
        else if( OI == 0x3E03 )//���������ź�ʶ�����¼�
        {
            pEvent = TopoUnit;
        }
        #endif
		else
		{
			pEvent = pEventUnit[OI-0x3000];		//��ȡ��Ӧ���¼���¼��
		}
		
		
		for( i=0; i< pEvent[0][0]; i++ )		//��ӹ̶���
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0, 0);//���rcsd�� choice
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}

			eResultChoice = DLT698AddBuf( Ch, 0, (BYTE*)& pEvent[i+1][1], 4);
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
		}
	
		Len = api_ReadEventAttribute(OI,0,MAX_APDU_SIZE, ProtocolBuf);//���ؽ���޷����� ��˲��ж�

		if( (Len & 0x8000) ||(Len < 4))//���ش�����߳���С��1��oad����
		{
			Num =0;
		}
		else
		{
			Num = Len/4;
		}
		
		if( Num > MAX_EVENT_OAD_NUM )//���м���ֵ���ж�
		{
			Num = MAX_EVENT_OAD_NUM;
		}
		
		for( i=0; i < Num; i++ )//��ӹ�����������
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0, 0);//���rcsd�� choice
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			
			eResultChoice = DLT698AddBuf( Ch, 0, &ProtocolBuf[4*i], 4);
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
		}

		pRCSDNum[0] = (Num+pEvent[0][0]);
	}
	else if(( OI >= 0x5000) && ( OI <= 0x5011))//����RCSD 
	{
		for( i=0; i< FixedFreezeOAD[0]; i++ )//��ӹ̶���
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0, 0);//���rcsd�� choice
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			
			eResultChoice = DLT698AddBuf( Ch, 0, (BYTE *)&FixedFreezeOAD[1+i], 4);
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
		}
		
		Len = api_ReadFreezeAttribute(OI,0,MAX_APDU_SIZE, ProtocolBuf);//���ؽ���޷����� ��˲��ж�

		if( (Len & 0x8000) ||(Len < 8))//���ش�����߳���С��1��oad����
		{
			Num =0;
		}
		else
		{
			Num = Len/8;
		}
		
		if( Num > MAX_FREEZE_ATTRIBUTE )//���м���ֵ���ж�
		{
			Num = MAX_FREEZE_ATTRIBUTE;
		}
		
		for( i=0; i < Num; i++ )//��ӹ�����������
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0, 0);//���rcsd�� choice
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			
			eResultChoice = DLT698AddBuf( Ch, 0, &ProtocolBuf[2+8*i], 4);
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
		}

		pRCSDNum[0] = (Num+FixedFreezeOAD[0]);
	}
	else//����ǲ�֧�ֵĶ�������¼� �޷���ȡ�����������ԡ��������ȫ��
	{
        pRCSDNum[0] = 0;
        DAR[0] = DAR_WrongType;
	}

	return eResultChoice;
}

//--------------------------------------------------
//��������:    ����RCSD���ݴ���
//         
//����:      BYTE Ch[in]	     ͨ��ѡ��
//         
//         WORD OI[in]	     OI �ж��¼� ����
//         
//         BYTE *pRCSD[in]	 RCSDָ��		
//         
//         BYTE *OutBuf[in]  ���bufָ��
//         
//         BYTE *DAR[out]	 �����־
//         
//����ֵ:     eAPPBufResultChoice ������ݽ��
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice DealRCSDData( BYTE Ch, WORD OI,BYTE *pRCSD, BYTE *OutBuf,BYTE *DAR)
{
	BYTE i,n,LenOffest;
	TTwoByteUnion Len;
	eAPPBufResultChoice eResultChoice;
	const DWORD (* pEvent)[2];
	TFourByteUnion OAD;
	
	eResultChoice = eADD_OK;
	
	if( pRCSD[0] == 0 )
	{	
        eResultChoice = DealRCSDIsZeroData( Ch, OI, DAR);     
	}
	else
	{
	    LenOffest =  Deal_698DataLenth( pRCSD, Len.b, ePROTOCOL_TYPE ); 
	    if( Len.w > (MAX_EVENT_OAD_NUM+10) )//�·���oad�������ܳ���Buf����
	    {
            DAR[0] = DAR_WrongType;
            return eResultChoice;
	    }
	    
		eResultChoice = DLT698AddBuf(Ch,0, pRCSD, (Len.w*5+LenOffest));//���RCSD���� ������������
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
		if( ( (OI >= 0x3000) && ( OI < 0x3030) && ( OI != 0x300C))
		 || (OI == 0x303B)  || (OI == 0x303C) || ( OI == 0x3040) || ( OI == 0x3104)
		 ||((OI >= 0xBD00) && ( OI < 0xBD09))
		 || ( OI == 0x30E0)||( OI == 0x30E1)|| ( OI == 0x30E2)
		 ||((OI == 0x3053) || ( OI == 0x310f)) 
         #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
         ||(OI == 0x3111)
         #endif
         #if (SEL_TOPOLOGY == YES )
         ||(OI == 0x3E03)
         #endif
         )//�¼�RCSD 
		{
			if(OI == 0x303B)
			{
				pEvent = pEventUnit[0x30];	//��ȡ��Ӧ���¼���¼��
			}
			else if(OI == 0x303C)
			{
				pEvent = pEventUnit[0x31];	//��ȡ��Ӧ���¼���¼��
			}
			else if( OI == 0x3040)
			{
				pEvent = pEventUnit[0x32];	//��ȡ��Ӧ���¼���¼��
			}
			else if( OI == 0x3104)
			{
				pEvent = pEventUnit[0x33];	//��ȡ��Ӧ���¼���¼��
			}
			else if (OI ==0x30E0)
			{
				pEvent=ChargingWARNINGUnit;
			}
			else if (OI ==0x30E1)
			{
				pEvent=ChargingERRUnit;
			}
			else if (OI ==0x30E2)
			{
				pEvent=ChargingCOM_EXCTIONUnit;
			}
			else if( (OI >= 0xBD00) && ( OI < 0xBD09) )//������Ϣ
			{
				pEvent = ModuleEventUnit;
			}
			else if((OI == 0x3053) || (OI == 0x310f))
			{
				pEvent = StandardEventUnit;
			}
            #if (SEL_EVENT_FIND_UNKNOWN_METER == YES )
            else if(OI == 0x3111)
            {
                pEvent = FindUnknownMeterUnit;
            }
            #endif
            #if (SEL_TOPOLOGY == YES )
            else if(OI == 0x3E03)
            {
                pEvent = TopoUnit;
            }
            #endif
			else
			{
				pEvent = pEventUnit[OI-0x3000];	//��ȡ��Ӧ���¼���¼��
			}

			for( i=0; i<Len.w;i++)			//��ȡ���ú�����OAD
			{
			    if( pRCSD[LenOffest+2+5*i-2] != 0)
			    {
                    DAR[0] = DAR_WrongType;
                    break;
			    }
			    
				memcpy( OAD.b,&pRCSD[LenOffest+2+5*i-1],4);
				for( n = 0; n < pEvent[0][0]; n++ )
				{
					if( OAD.d == pEvent[n+1][0] )//���
					{
						memcpy(&OutBuf[4*i],&pEvent[n+1][1],4);//��֡RCSD
						break;
					}					
				}
				
				if( n == pEvent[0][0] )
				{
					memcpy(&OutBuf[4*i],&pRCSD[LenOffest+2+5*i-1],4);//��֡RCSD
				}
			}
		
		}
		else if( (OI>=0x5000) && (OI<=0x5011) )
		{
			for( i=0; i<Len.w;i++)//��ȡ���ú�����OAD
			{
			    if( pRCSD[LenOffest+2+5*i-2] != 0)
			    {
                    DAR[0] = DAR_WrongType;
                    break;
			    }
			    
				memcpy(&OutBuf[4*i],&pRCSD[LenOffest+2+5*i-1],4);//��֡RCSD
			}
		}
		else
		{
            DAR[0] = DAR_WrongType;
		}
	}

	return eResultChoice;

}
//--------------------------------------------------
//��������:    ��ȡ��¼ʱ�������ܡ�A��B��C���ж�
//         
//����:      WORD OI[in]				            OI
//         
//         BYTE ClassAttribute[in]		        ����
//         
//         TDLT698RecordPara *DLT698RecordPara[in] ��¼�ṹ��ָ��
//         
//����ֵ:     BOOL  TRUE �ɹ�   FALSE ʧ��
//         
//��ע����:    ��
//--------------------------------------------------
BOOL RecordJudgePhase( WORD OI, BYTE ClassAttribute, TDLT698RecordPara *DLT698RecordPara)
{
	if( ClassAttribute == 2 )//����¼��������ж�
	{
		DLT698RecordPara->Phase = ePHASE_ALL;
	}
	else if( ClassAttribute >= 6 )
	{
		if( ClassAttribute == 6 )//����9Ϊ��
		{
			if( (OI == 0x3007) || (OI == 0x300B) || (OI == 0x303B) )//OI���ǳ���������޹����� �����¼�ֻ��A,B,C��
			{
				DLT698RecordPara->Phase = ePHASE_ALL;
			}
			else
			{
				return FALSE;
			}

		}
		else
		{
			DLT698RecordPara->Phase = (ePHASE_TYPE)(ClassAttribute -6);		
			//���޲��ܳ���4�����ڵ���ʱ ���ܳ���B��C��
			if( ((DLT698RecordPara->Phase) > 4) ||( (MAX_PHASE == 1) && ((DLT698RecordPara->Phase) > 1)) )
			{
				return FALSE;
			}
		}

	}
	else
	{
		DLT698RecordPara->Phase = ePHASE_ALL;
	}	
	
	return TRUE;
}

//--------------------------------------------------
//��������:    ��ȡ��¼����������
//         
//����:      BYTE Ch[in]  	ͨ��
//         
//         WORD Len[in]	    ��¼���صĳ���
//         
//����ֵ:     eAPPBufResultChoice   �������buf���
//         
//��ע����:  ��
//--------------------------------------------------
eAPPBufResultChoice DealGetRequestRecordResult( BYTE Ch,WORD Len)
{
	eAPPBufResultChoice eResultChoice ;
	
	if((!(Len & 0x8000)) && (Len !=0) )//�����������¼����ǰ��û�г��� //�Ѿ��ж������Ƿ����� �����ж�
	{
		eResultChoice = DLT698AddOneBuf(Ch,0, 1);//��Ӧ����   CHOICE  {     ������Ϣ     [0] DAR��   M����¼     [1] SEQUENCE OF A-RecordRow}
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}

		eResultChoice = DLT698AddOneBuf(Ch,0, 1);// M����¼��M=1
		if( eResultChoice != eADD_OK )//
		{
			return eResultChoice;
		}
		
		eResultChoice = DLT698AddBufLen( Ch, 0x55, Len);
	}
	else
	{
		if( Len == 0 )//���buf���Ȳ��� ����Ӧ�ò��֡
		{
			return eAPP_LINKDATA_OK;
		}
		else if( Len == 0x8000 )
		{
            //eResultChoice = DLT698AddBufDAR( Ch, DAR_NoObject );  //@@@@@@@  ����������Խ�����ĵ�
            eResultChoice = DLT698AddOneBuf(Ch,0, 1);//��Ӧ����   CHOICE  {     ������Ϣ     [0] DAR��   M����¼     [1] SEQUENCE OF A-RecordRow}
            if( eResultChoice != eADD_OK )
            {
                return eResultChoice;
            }
            
            eResultChoice = DLT698AddOneBuf(Ch,0X55, 0);// M����¼��M=0
            if( eResultChoice != eADD_OK )//
            {
                return eResultChoice;
            }
	    }
	    else
	    {
            eResultChoice = DLT698AddBufDAR( Ch,DAR_RefuseOp );
    		if( eResultChoice != eADD_OK )
    		{
    			return eResultChoice;
    		}
	    }
	}
	return eResultChoice;
}

//--------------------------------------------------
//��������:    RSD���ݴ���
//         
//����:      BYTE *pRSD[in] RSDָ��
//         
//         TDLT698RecordPara *DLT698RecordPara[in] ��¼�ṹ��ָ��
//         
//����ֵ:     BOOL TRUE �ɹ�  FALSE ʧ��  
//         
//��ע����:    ��
//--------------------------------------------------
BOOL DealRSDData( BYTE *pRSD, TDLT698RecordPara *DLT698RecordPara )
{
	TTwoByteUnion OI;

	lib_ExchangeData( OI.b, &pRSD[1], 2 );

	if( (OI.w == 0x2021) && (pRSD[5]==28) )
	{
		DLT698RecordPara->eTimeOrLastFlag = eFREEZE_TIME_FLAG;
	}
	else if( (OI.w == 0x201e) && (pRSD[5]==28) )
	{
		DLT698RecordPara->eTimeOrLastFlag = eEVENT_START_TIME_FLAG;
	}
	else if( (OI.w == 0x2020) && (pRSD[5]==28) )
	{
		DLT698RecordPara->eTimeOrLastFlag = eEVENT_END_TIME_FLAG;
	}
	else
	{
		return FALSE;
	}

	return TRUE;	
}
//--------------------------------------------------
//��������:    ����1��ȡ��¼
//         
//����:      BYTE Ch[in]              ͨ��ѡ��
//         
//         BYTE *pRSD[in]           RSD��ָ��
//         
//         WORD OI[in]              ����OI
//         
//         BYTE ClassAttribute[in]  ����
//         
//����ֵ:     eAPPBufResultChoice      ���BUF�������
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordDataMethod1(BYTE Ch, BYTE *pRSD,WORD OI,BYTE ClassAttribute)
{
	BYTE *pRCSD,LenOffest,DAR;
	BYTE TimeBuf[7];
	BYTE Buf[(MAX_ATTRIBUTE+10)*4+20];
	WORD Len,Result,DataLen;
	DWORD AbsTime698;
	TTwoByteUnion TmpLen;
	eAPPBufResultChoice eResultChoice;
    TDLT698RecordPara DLT698RecordPara;
        
    DAR = DAR_Success;
	Len = 0;
	DataLen = 0;
	AbsTime698 = ILLEGAL_VALUE_8F;
	
	Result = DealRSDData( pRSD, ( TDLT698RecordPara * )&DLT698RecordPara);
	if( Result == FALSE )
	{	
        DAR = DAR_WrongType;	   
	}

    DataLen = GetProtocolDataLen( (BYTE*)&pRSD[5] );//��ȡDATA�����ݳ���
    if( DataLen == 0x8000 )//�����ȡ����ʧ�ܣ���ʱ��Ϊ������Ӧ��
    {
        return eAPP_ERR_RESPONSE;    
    }
    
    pRCSD = pRSD+5+DataLen;
    //ȡ�·�ʱ��
    if( pRSD[5] == DateTime_S_698 )//�·���ʱ���ʽ������DateTime_S_698
    {
    	memcpy(TimeBuf,pRSD+6,7);
    	lib_InverseData(TimeBuf, 2);
    	
    	if( (OI >= 0x5000) && (OI <= 0x5011) )//�����·�Ϊ������
		{
        	AbsTime698 = api_CalcInTimeRelativeMin((TRealTimer *)TimeBuf);//������ʼʱ��
        	if( AbsTime698 == ILLEGAL_VALUE_8F )
        	{
                DAR = DAR_OverRegion;
        	}
            else if( TimeBuf[6] != 0 )//�벻Ϊ��ʱ���·�����Է�������1���ӣ�����ʱ���Ӧ���ϵ�����
    		{
                AbsTime698 += 1;
    		}
		}
		else
		{
            AbsTime698 = api_CalcInTimeRelativeSec((TRealTimer *)TimeBuf);//������ʼʱ��
            if( AbsTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
		}
    }
    else //�������ݳ���
    {
    	DAR = DAR_WrongType;
    }

    //RCSD
    LenOffest =  Deal_698DataLenth( pRCSD, TmpLen.b, ePROTOCOL_TYPE );
    eResultChoice = DealRCSDData( Ch, OI, pRCSD,Buf, (BYTE *)&DAR);
    if( eResultChoice != eADD_OK )
    {
    	return eResultChoice;
    }
    
    if( APPData[Ch].TimeTagFlag == eTime_Invalid )//ʱ���ǩ����
    {
        DAR = DAR_TimeStamp;
    }
    else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//�жϰ�ȫģʽ���� ���ȼ����
    {			
        DAR = DAR_PassWord;
    }
    else if( JudgeRecordOAD(OI,ClassAttribute) == FALSE )//�жϼ�¼OAD
    {			
        DAR = DAR_RefuseOp;

    }
    else
    {
        DLT698RecordPara.OI = OI;                   //OI����
        DLT698RecordPara.Ch = Ch;
        DLT698RecordPara.pOAD = Buf;                //pOADָ��
        DLT698RecordPara.OADNum = TmpLen.w;     //OAD����
        DLT698RecordPara.TimeOrLast = AbsTime698;   //����ʱ����
        Result = RecordJudgePhase( OI, ClassAttribute, &DLT698RecordPara);
        if( Result == FALSE )
        {
            DAR = DAR_RefuseOp;
        }   
    }
  
	if( DAR == DAR_Success )//�޴��� ���ж�ȡ��¼
	{
		//���ú��� ��ȡ��¼ �������� 
		if( (APPData[Ch].APPMaxBufLen.w <= (APPData[Ch].APPCurrentBufLen.w+2)) //���м���ֵ�ж�
	    ||(APPData[Ch].APPMaxBufLen.w > MAX_APDU_SIZE))
		{
            Len = 0;
		}
		else
		{
		    Len = ReadProRecordData( 0x01, &DLT698RecordPara, (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-2), &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+2]);
		}

		eResultChoice = DealGetRequestRecordResult( Ch, Len );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
	}
	else//���DAR
	{
        eResultChoice = DLT698AddBufDAR( Ch, DAR );
        if( eResultChoice != eADD_OK )
        {
            return eResultChoice;
        }
	}
		
	APPData[Ch].BufDealLen.w += (4+1+4+DataLen+LenOffest+TmpLen.w*5);// ����Ѵ�������buf����4 oad+1���� +4 oad + 8 ʱ�� + 1rcsd���� + rcsd

	return eResultChoice;
}

//--------------------------------------------------
//��������:    ����2��ȡ��¼
//         
//����:      BYTE Ch[in]                  //ͨ��ѡ��
//         
//         BYTE *pRSD[in]               //RSDָ��
//         
//         WORD OI[in]                  //��ȡ�ļ�¼OI
//         
//         BYTE ClassAttribute[in]      //��ȡ������
//         
//����ֵ:     eAPPBufResultChoice          //���BUF�������
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordDataMethod2(BYTE Ch, BYTE *pRSD,WORD OI,BYTE ClassAttribute)
{
	BYTE *pRCSD,DAR,LenOffest;
	BYTE StartTimeBuf[7],EndTimeBuf[7];
	BYTE Buf[(MAX_ATTRIBUTE+10)*4+20];
	WORD Len,Result,DataLen,DataLen1,DataLen2,TotalLen;
	DWORD AbsStartTime698,AbsEndTime698;
	BYTE TIIndex;
	TTwoByteUnion TIData,TmpLen,Num;
	WORD NumAddress;
	eAPPBufResultChoice eResultChoice;
    TDLT698RecordPara DLT698RecordPara;

	DAR = DAR_Success;
	DLT698RecordPara.TimeIntervalPara.AllCycle = 0;
	TIIndex = 0;
	AbsStartTime698 = 0;
	AbsEndTime698 = 0;
	
	Result = DealRSDData( pRSD, ( TDLT698RecordPara * )&DLT698RecordPara);
	if( Result == FALSE )
	{
		DAR = DAR_WrongType;
	}
 
    DataLen = GetProtocolDataLen( pRSD+5 );//��ȡ��ʼֵ����
    if( DataLen == 0x8000 )//�����ȡ����ʧ�ܣ���ʱ��Ϊ������Ӧ��
    {
        return eAPP_ERR_RESPONSE;
    }

    DataLen1 = GetProtocolDataLen( pRSD+5+DataLen );//��ȡ����ֵ����
    if( DataLen1 == 0x8000 )//�����ȡ����ʧ�ܣ���ʱ��Ϊ������Ӧ��
    {
        return eAPP_ERR_RESPONSE;
    }

    DataLen2 = GetProtocolDataLen( pRSD+(5+DataLen+DataLen1) );//��ȡ���ݼ��
    if( DataLen2 == 0x8000 )//�����ȡ����ʧ�ܣ���ʱ��Ϊ������Ӧ��
    {
        return eAPP_ERR_RESPONSE;
    }

    TotalLen = DataLen+DataLen1+DataLen2;//��ʼֵ����+����ֵ����
    pRCSD = pRSD+5+TotalLen;
    
    if( pRSD[(5+DataLen+DataLen1)] == TI_698 )//ѡ��ti
    {
    	TIIndex = pRSD[6+DataLen+DataLen1];//ȡƫ��
    	TIData.b[1] =  pRSD[7+DataLen+DataLen1];
    	TIData.b[0] =  pRSD[8+DataLen+DataLen1];
    }
    else if( pRSD[(5+DataLen+DataLen1)] == NULL_698 )
    {
        DLT698RecordPara.TimeIntervalPara.AllCycle = 0x55;
    }
    else
    {
        DAR = DAR_WrongType;
    }

    //ȡ��ʼʱ��
	if( pRSD[5] == DateTime_S_698 )//�·���ʱ���ʽ������DateTime_S_698
	{
		memcpy(StartTimeBuf,pRSD+6,7);
		lib_InverseData(StartTimeBuf, 2);
		if( (OI >= 0x5000) && (OI <= 0x5011) )//�����·�Ϊ������
		{
            AbsStartTime698 = api_CalcInTimeRelativeMin((TRealTimer *)StartTimeBuf);//������ʼʱ��
            if( AbsStartTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
            else if( StartTimeBuf[6] != 0 )//�벻Ϊ��ʱ���·�����Է�������1���ӣ�����ʱ���Ӧ���ϵ�����
    		{
                AbsStartTime698 += 1;
    		}
		}
		else//�¼��·�Ϊ��
		{
            AbsStartTime698 = api_CalcInTimeRelativeSec((TRealTimer *)StartTimeBuf);//������ʼʱ��
            if( AbsStartTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
		}
	}
	else
	{
		DAR = DAR_WrongType;
	}
	
	//ȡ����ʱ��
	if( pRSD[13] == DateTime_S_698 )//�·���ʱ���ʽ������DateTime_S_698
	{
		memcpy(EndTimeBuf,pRSD+14,7);
		lib_InverseData(EndTimeBuf, 2);
		if( (OI >= 0x5000) && (OI <= 0x5011) )//�����·�Ϊ������
		{
            AbsEndTime698 = api_CalcInTimeRelativeMin((TRealTimer *)EndTimeBuf);//������ʼʱ��
            if( AbsEndTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
            else if( EndTimeBuf[6] != 0 )//�벻Ϊ��ʱ���·�����Է�������1���ӣ�����ʱ���Ӧ���ϵ�����
    		{
                AbsEndTime698 += 1;
    		}
		}
		else//�¼��·�Ϊ��
		{
            AbsEndTime698 = api_CalcInTimeRelativeSec((TRealTimer *)EndTimeBuf);//������ʼʱ��
            if( AbsEndTime698 == ILLEGAL_VALUE_8F )
            {
                DAR = DAR_OverRegion;
            }
		}
	}
	else
	{
		DAR = DAR_WrongType;
	}
	
	if( ((OI >= 0x5002) && (OI <= 0x5006)) && ( OI != 0x5005) )//�����Ķ��᲻��ʱ����
	{
	    if( DLT698RecordPara.TimeIntervalPara.AllCycle != 0x55 )
	    {
            if( OI == 0x5006)//�¶���ʱ���������ж�
            {
                if( TIIndex != 4 )
                {
                    DAR =DAR_RefuseOp;
                }       
            }         
            else if( (OI-0x5001) != TIIndex )//���ʱ������λ���ԣ����ش���
            {
                DAR =DAR_RefuseOp;
            }
	    }
	}

    if( TIData.w == 0 )//���ʱ����Ϊ�����ȫʱ���
    {
        DLT698RecordPara.TimeIntervalPara.AllCycle = 0x55;
    }
    
	//RCSD
	LenOffest =  Deal_698DataLenth( pRCSD, TmpLen.b, ePROTOCOL_TYPE );
	eResultChoice = DealRCSDData( Ch, OI, pRCSD,Buf, (BYTE *)&DAR);
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
    
    if( APPData[Ch].TimeTagFlag == eTime_Invalid )//ʱ���ǩ����
    {
        DAR = DAR_TimeStamp;
    }
	else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//�жϰ�ȫģʽ����
	{			
		DAR = DAR_PassWord;
	}
	else if( JudgeRecordOAD(OI,ClassAttribute) == FALSE )//�жϼ�¼OAD
	{			
		DAR = DAR_WrongType;
	}
	else
	{
        DLT698RecordPara.OI = OI;                                       //OI����
        DLT698RecordPara.Ch = Ch;
        DLT698RecordPara.pOAD = Buf;                                    //pOADָ��
        DLT698RecordPara.OADNum = TmpLen.w;                             //OAD����
        DLT698RecordPara.eTimeOrLastFlag += eRECORD_TIME_INTERVAL_FLAG;  //ʱ�������־
        DLT698RecordPara.TimeOrLast = 0;                                //����ʱ����
        DLT698RecordPara.TimeIntervalPara.StartTime = AbsStartTime698;  //��ʼʱ��
        DLT698RecordPara.TimeIntervalPara.EndTime = AbsEndTime698;      //����ʱ��
        DLT698RecordPara.TimeIntervalPara.TI = TIData.w;                //ʱ����
        
        Result = RecordJudgePhase( OI, ClassAttribute, &DLT698RecordPara);
        if( Result == FALSE )
        {
            DAR = DAR_WrongType;
        }
	}
	
	if( DAR == DAR_Success )//�޴���������ж�ȡ��¼
	{
		if( (APPData[Ch].APPMaxBufLen.w <= (APPData[Ch].APPCurrentBufLen.w+2)) //���м���ֵ�ж�
	    ||(APPData[Ch].APPMaxBufLen.w > MAX_APDU_SIZE))
		{
            Len = 0;
		}
		else
		{
            //���ú��� ��ȡ��¼ ��������
            Len = ReadProRecordData( 0x01, &DLT698RecordPara, (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-2), &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+2]);      
		}
		
        NumAddress = APPData[Ch].APPCurrentBufLen.w+2;

		eResultChoice = DealGetRequestRecordResult( Ch, Len );//�������ͷ  ��ʹ��һ���ǽ���������ͨ��Len=0x8000���н������
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
        Num.w = DLT698RecordPara.TimeIntervalPara.ReturnFreezeNum;
        if( Len < 0x8000 )
        {
            Deal_698SequenceOfNumData( Ch, NumAddress, Num.b);//�Լ�¼�������д���
        }
	}
	else//���DAR
	{
        eResultChoice = DLT698AddBufDAR( Ch, DAR );
        if( eResultChoice != eADD_OK )
        {
            return eResultChoice;
        }
	}
	
	APPData[Ch].BufDealLen.w += (4+1+4+DataLen+DataLen1+DataLen2+LenOffest+TmpLen.w*5);// ����Ѵ�������buf���� 4 oad + 1���� +  4 oad + 8 ʱ�� + 8 ʱ�� +4 ʱ���� +  1rcsd����+ rcsd

	return eResultChoice;
}

//--------------------------------------------------
//��������:    ����9��ȡ��¼
//         
//����:      BYTE Ch[in]              //ͨ��
//         
//         BYTE *pRSD[in]           //RSDָ��
//         
//         WORD OI[in]              //��¼OI
//         
//         BYTE ClassAttribute[in]  //��ȡ������
//         
//����ֵ:     eAPPBufResultChoice �������buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordDataMethod9(BYTE Ch, BYTE *pRSD,WORD OI,BYTE ClassAttribute)
{
	BYTE LenOffest,DAR;
	BYTE Buf[(MAX_ATTRIBUTE+10)*4+20];
	WORD Len;
	WORD No,Result;
	TTwoByteUnion TmpLen;
	BYTE *pRCSD;
	eAPPBufResultChoice eResultChoice;
    TDLT698RecordPara DLT698RecordPara;
	
	No =1;
	DAR = DAR_Success;
	pRCSD = pRSD+2;
	No = pRSD[1];//��ȡ��ȡ�ϼ���
		
	//RCSD
	LenOffest =  Deal_698DataLenth( pRCSD, TmpLen.b, ePROTOCOL_TYPE ); 
	eResultChoice = DealRCSDData( Ch, OI, pRCSD,Buf, (BYTE *)&DAR);
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	if( APPData[Ch].TimeTagFlag == eTime_Invalid )//ʱ���ǩ����
	{
		DAR = DAR_TimeStamp;
	}
	else if( JudgeTaskAuthority( Ch, eREAD_MODE, pRSD-4) == FALSE )//�жϰ�ȫģʽ����
	{			
		DAR = DAR_PassWord;
	}
	else if( JudgeRecordOAD(OI,ClassAttribute) == FALSE )//�жϼ�¼OAD
	{			
		DAR = DAR_WrongType;
	}
	else
	{
        DLT698RecordPara.OI = OI;                   //OI����
        DLT698RecordPara.Ch = Ch;
        DLT698RecordPara.pOAD = Buf;                //pOADָ��
        DLT698RecordPara.OADNum = TmpLen.w;     //OAD����
        DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //ѡ�����
        DLT698RecordPara.TimeOrLast = No;           //����ʱ����
        Result = RecordJudgePhase( OI, ClassAttribute, &DLT698RecordPara);
        if( Result == FALSE )
        {
            DAR = DAR_WrongType;
        }
	}
	
	if( DAR == DAR_Success )//�޴��� ��ȡ��¼
	{
		if( (APPData[Ch].APPMaxBufLen.w <= (APPData[Ch].APPCurrentBufLen.w+2)) //���м���ֵ�ж�
	    ||(APPData[Ch].APPMaxBufLen.w > MAX_APDU_SIZE))
		{
            Len = 0;
		}
		else
		{
    		//���ú��� ��ȡ��¼ ��������
    		Len = ReadProRecordData( 0x01, &DLT698RecordPara, (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-2), &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+2]);
		}
		
		eResultChoice = DealGetRequestRecordResult( Ch, Len );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
	}
	else//���DAR
	{
		eResultChoice = DLT698AddBufDAR( Ch, DAR );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
	}
	
	APPData[Ch].BufDealLen.w += 4+2+LenOffest+TmpLen.w*5;// ����Ѵ�������buf���� 4 oad + 2 ����9 + 1���� +rcsd

	return eResultChoice;
}
//--------------------------------------------------
//��������:  ��ȡ��ǰ�¼��Ƿ�û�н���  
//         
//����:      OI  
//			Phase
//         
//����ֵ:    TRUE  ������
//			FALSE  û����Ҫ��
//         
//��ע:    ���ڲ������ж�
//--------------------------------------------------
BOOL api_GetFlashLastEventEnd(WORD OI,BYTE Phase)
{
	TEventOADCommonData EventOADCommonData;
	TEventAttInfo		EventAttInfo;
	TEventDataInfo		EventDataInfo;
	TEventAddrLen		EventAddrLen;
	TEventSectorInfo	EventSectorInfo;
	DWORD dwTemp;

	if( GetEventIndex( OI, &EventAddrLen.EventIndex ) == FALSE )
	{
		return TRUE;
	}

	EventAddrLen.Phase = Phase;
	if( GetEventInfo( &EventAddrLen ) == FALSE )
	{
		return TRUE;
	}
	
	api_ReadFromContinueEEPRom(EventAddrLen.dwAttInfoEeAddr, sizeof(TEventAttInfo), (BYTE *)&EventAttInfo);
	api_VReadSafeMem(EventAddrLen.dwDataInfoEeAddr, sizeof(TEventDataInfo), (BYTE *)&EventDataInfo);
	
	//���ݵ�ǰ��¼��Ϊ0���ҵ�ǰ״̬Ϊ�¼�û����
	if( EventDataInfo.RecordNo == 0 )
	{
		return TRUE;
	}

	EventDataInfo.RecordNo -= 1;	//��ȡ��һ��
	//���������¼ƫ�Ƶ�ַ
	GetEventSectorInfo(EventAttInfo.AllDataLen, EventAttInfo.MaxRecordNo, EventDataInfo.RecordNo, &EventSectorInfo);
	
	dwTemp = EventAddrLen.dwRecordAddr + EventSectorInfo.dwSectorAddr;
	
	api_ReadMemRecordData( dwTemp, sizeof(TEventOADCommonData), (BYTE *)&EventOADCommonData );
	if (lib_IsAllAssignNum( (BYTE *)&EventOADCommonData.EventTime.EndTime, 0xff, sizeof(TRealTimer) ) == TRUE)
	{
		return FALSE;
	}
	

	return TRUE;
}
//--------------------------------------------------
//��������:    ��ȡ���������ݱ�
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pData[in]      Dataָ��
//         
//����ֵ:     eAPPBufResultChoice  �������buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordDataTable(BYTE Ch, BYTE *pData )
{
	TTwoByteUnion OI;
	BYTE *pRSD, ClassAttribute;
	eAPPBufResultChoice eResultChoice;
	BYTE Outdata[2048];
	BOOL bNext=FALSE;
	WORD Len,Firstoffset;
	DWORD OAD=0;

	eResultChoice = eAPP_ERR_RESPONSE;
	lib_ExchangeData((BYTE*)&OAD,pData,4);//��ȡ����OI
	lib_ExchangeData(OI.b,pData,2);//��ȡ����OI
	ClassAttribute = pData[2];
	pRSD = pData + 4;//��ȡRSDָ��
	FindThisEventSaveOI(&OI.w,0,eEventMap);
	if(OI.w==0x6000)
	{
		Len=Class11_Get_rec(Ch,OAD,pRSD,0,&APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],(APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w),&bNext,&Firstoffset);
		if (Len)
		{
			APPData[Ch].APPCurrentBufLen.w+=Len;
			eResultChoice=eADD_OK;
			if (bNext)
			{
				SetNextPara(Ch,(void*)Class11_Get_rec,OAD,Firstoffset,pData,2);
			}
			return eResultChoice;
		}
		else
		{
			// Len=Class10_Get_rec(Ch,OAD,pRSD,0,&APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],(APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w), &bNext,&Firstoffset);
			// if (Len)
			// {
			// 	APPData[Ch].APPCurrentBufLen.w+=Len;
            //     eResultChoice=eADD_OK;
			// 	if (bNext)
			// 	{
			// 		SetNextPara(Ch,(void*)Class10_Get_rec,OAD,Firstoffset,pData,2);
			// 	}
			// 	return eResultChoice;
			// }
			// else
			{
				return eAPP_ERR_RESPONSE;
			}
		}
	}

	
	switch(pRSD[0])//ѡ�񷽷�
	{
		case 0:
			break;
			
		case 1://ѡ�񷽷�1
		
			eResultChoice = GetRequestRecordDataMethod1( Ch, pRSD, OI.w,ClassAttribute);
			
			break;
		case 2://ѡ�񷽷�2 Ϊָ�������������������ֵ 
	
			eResultChoice = GetRequestRecordDataMethod2( Ch, pRSD, OI.w,ClassAttribute);

			break;
	
		case 9://ѡ�񷽷�9 ��ȡ�ϼ���
		
			eResultChoice = GetRequestRecordDataMethod9( Ch, pRSD, OI.w,ClassAttribute);
			
			break;
			
		default:
			return  eAPP_ERR_RESPONSE;
			break;
	}
	
	return eResultChoice;
}


//--------------------------------------------------
//��������:    ��ȡ��¼������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOAD[in]       OADָ��
//         
//����ֵ:     eAPPBufResultChoice  ���Buf���ؽ��
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecord( BYTE Ch,BYTE *pOAD)
{
	eAPPBufResultChoice eResultChoice;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }

	eResultChoice = DLT698AddBuf(Ch,0, pOAD, 4);//���OAD���� ������������
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}

	eResultChoice = GetRequestRecordDataTable(Ch, pOAD);//��ȡ��¼��
	
	return eResultChoice;
}

//--------------------------------------------------
//��������:    ��ȡ��¼list������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//����ֵ:     eAPPBufResultChoice  ���Buf���ؽ�� 
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice GetRequestRecordList( BYTE Ch )
{
	BYTE i,LenOffest;
    WORD NoAddress;
	TTwoByteUnion Num,No;
	eAPPBufResultChoice eResultChoice;	
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;

	No.w = 0;
	
	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//��Ӹ��� ��������֡����ȷ
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    NoAddress = APPData[Ch].APPCurrentBufLen.w;

	for(i = 0; i < Num.w;i++)
	{
		eResultChoice = GetRequestRecord(Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);

        if( eResultChoice != eADD_OK) 
		{
			break;
		}

		No.w ++;//sequence of record �ĸ�����1 
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//�Ѵ������ݴ����·������ݽ��з��أ���������ĵĸ���
		{
			break;
		}
	}
	
    JudgeAppBufLen(Ch);

    Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
    
	return eResultChoice;
}
//-----------------------------------------------
//��������: ���ø����ϱ�ģʽ��
//
//����:      Buf[in]��	�������ݵĻ���
//			
//����ֵ:	   ���ض����ݳ���
//			
//��ע:      eAPPBufResultChoice  
//-----------------------------------------------
BYTE SetRequestFollowReportMode( BYTE *pData )
{
	BYTE DAR;
	BYTE Buf[10];

	DAR = DAR_WrongType;	
	
	if( (pData[0] != Bit_string_698)||(pData[1] != 32) )//����������Ͳ��ԣ����ݳ��Ȳ���
	{
		DAR = DAR_OtherErr;		
	}
	else
	{	
		lib_ExchangeBit( Buf, (BYTE*)&pData[2], 4);
	
		if( api_SetFollowReportMode(Buf) == FALSE )
		{
			DAR = DAR_OtherErr;
		}
		else
		{
			DAR = DAR_Success;
		}
	}	
	
    return DAR;
}
//--------------------------------------------------
//��������:    �����¼������������Ա�
//         
//����:      WORD OI[in]            ����OI 
//         
//         BYTE ClassIndex[in]    Ԫ��ƫ��
//         
//         BYTE *pData[in]        dataָ��
//                 
//����ֵ:     BYTE  DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventRelationTable( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[MAX_EVENT_OAD_NUM*4+30],i,OadType;
	WORD Result;
        
	Result = FALSE;
	DAR = DAR_WrongType;
	OadType = 0;
	i  = 0;
	if( ClassIndex != 0 )//���õ�������
	{
		return DAR_RefuseOp;
	}
	
	if( pData[0] == Array_698 )
	{
	    if( pData[1] <= MAX_EVENT_OAD_NUM )
	    {
            for( i=0; i< pData[1]; i++ )
            {
                if( pData[2+5*i] == OAD_698 )
                {
                    memcpy( &Buf[4*i], &pData[2+5*i+1],0x04);
                }
                else
                {
                    break;
                }
            }

            if( i == pData[1] )//û�д���
            {
                DAR = DAR_Success;
            }
            else
            {
                DAR = DAR_WrongType;
            }
	    }
	}

	if( DAR == DAR_Success )
	{
		Result =  api_WriteEventAttribute( OI, OadType,Buf,pData[1]);
		if( Result == FALSE )
        {
            DAR = DAR_RefuseOp;//��Ϊ�̶����󷵻أ�����������ģ����ϲ���ú����й�
        }
        else
        {
            DAR = DAR_Success;
        }
	}
        
	return DAR;
}

//--------------------------------------------------
//��������:    �����¼������������Ա�
//         
//����:      BYTE *InBuf[in]            ����buf
//         
//         BYTE *OutBuf[out]          ���buf   
//
//         BYTE Len                   Ҫת���ĳ��ȣ�ֻ����2����4
//         
//��ע����:    ��
//--------------------------------------------------
WORD DoubleLongOrLongToAbs(         BYTE *InBuf, BYTE *OutBuf, BYTE Len )
{
    TFourByteUnion LLValue;
    TTwoByteUnion LValue;

    LLValue.l = 0;
    LValue.sw = 0;

    if( (Len != 4) && (Len != 2) )
    {
        return FALSE;
    }

    if( Len == 4 )
    {
        lib_ExchangeData( LLValue.b, InBuf, Len );

        if( LLValue.l < 0 )
        {
           LLValue.l = ~LLValue.l+1;
        }

        memcpy( OutBuf, LLValue.b, Len);
    }
    else
    {
        lib_ExchangeData( LValue.b, InBuf, Len );

        if( LValue.sw < 0 )
        {
           LValue.sw = ~LValue.sw+1;
        }

        memcpy( OutBuf, LValue.b, Len);
    }

    
    return TRUE;
}

#if( MAX_PHASE != 1 )	
//--------------------------------------------------
//��������:    ����ʧѹ�¼���¼���ò���
//         
//����:      WORD OI[in]            ����OI
//         
//         BYTE ClassIndex[in]    Ԫ������
//         
//         BYTE *pData[in]        DATA����ָ��                
//         
//����ֵ:     BYTE  DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventLost_V( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 4 )
	{
		return DAR ;
	}

	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 4 ) // 4��Ԫ��
			{
				if( pData[2] == Long_unsigned_698 )//��ѹ��������
				{
					lib_ExchangeData(Buf, &pData[3], 2);
					if( pData[5] == Long_unsigned_698 )//��ѹ�ָ�����
					{
						lib_ExchangeData( &Buf[2],  &pData[6], 2);
						if( pData[8] == Double_long_698 )//������������
						{
							//lib_ExchangeData( &Buf[4],  &pData[9], 4);
							DoubleLongOrLongToAbs(&pData[9], &Buf[4], 4 );
							if( pData[13] == Unsigned_698 )//�ж���ʱʱ��
							{
								Buf[8] = pData[14];
								DAR = DAR_Success;
								
							}
						}
					}
				}
			}		
		}

	}
	else
	{
		if( ClassIndex < 3 )
		{
			if( pData[0] == Long_unsigned_698 )// ��ѹ�������� ���� ��ѹ�ָ�����
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				DAR = DAR_Success;
			}
		}
		else if( ClassIndex == 3 )
		{
			if( pData[0] == Double_long_698 )//������������
			{
				//lib_ExchangeData( Buf,  &pData[3], 4);
				DoubleLongOrLongToAbs(&pData[1], Buf, 4 );
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//�ж���ʱʱ��
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara(OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//��������:    ����Ƿѹ���ѹ�¼���¼���ò���
//         
//����:      WORD OI[in]             ����OI
//         
//         BYTE ClassIndex[in]     Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                 
//����ֵ:     BYTE  DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventWeakOrOver_V( WORD OI,BYTE ClassIndex,BYTE *pData )//Ƿѹ���߹�ѹ
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}


	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 2 ) // 4��Ԫ��
			{
				if( pData[2] == Long_unsigned_698)//��ѹ�������� ������
				{
					lib_ExchangeData( Buf,  &pData[3], 2);
					if( pData[5] == Unsigned_698 )//�ж���ʱʱ��
					{
						Buf[2] = pData[6];
						DAR = DAR_Success;
					}
				}
			}		
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Long_unsigned_698 )//��ѹ�������� ������
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//�ж���ʱʱ��
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//��������:    ���ö��������¼���¼���ò���
//         
//����:      WORD OI[in]             ����OI
//         
//         BYTE ClassIndex[in]     Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                  
//����ֵ:     BYTE  DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventBreakOrBreak_I( WORD OI,BYTE ClassIndex,BYTE *pData )//������߶���
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 3 )//���ࡢ������3����������ѹ��������, ������������,�ж���ʱʱ�� wlk
	{
		return DAR;
	}

	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 3 ) // 4��Ԫ��
			{
				if( pData[2] == Long_unsigned_698)//��ѹ�������� ������
				{
					lib_ExchangeData( Buf,  &pData[3], 2);
					if( pData[5] == Double_long_698 )//��������������
					{
						//lib_ExchangeData( &Buf[2],  &pData[6], 4);
						DoubleLongOrLongToAbs(&pData[6], &Buf[2], 4);
						if( pData[10] == Unsigned_698 )//�ж���ʱʱ��
						{
							Buf[6] = pData[11];
							DAR = DAR_Success;
						}
					}
				}
			}
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Long_unsigned_698 )//��ѹ�������� ������
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				DAR = DAR_Success;
			}
		}
		else if( ClassIndex == 2 ) 
		{
			if( pData[0] == Double_long_698 )//��������������
			{
				//lib_ExchangeData( Buf,  &pData[3], 4);
				DoubleLongOrLongToAbs(&pData[1], Buf, 4);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//�ж���ʱʱ��
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ����ʧ���¼���¼���ò���
//         
//����:      WORD OI[in]             ����OI
//         
//         BYTE ClassIndex[in]     Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                 
//����ֵ:     BYTE  DAR���     
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventLost_I( WORD OI,BYTE ClassIndex,BYTE *pData )//ʧ��
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 4 )
	{
		return DAR ;
	}
	
	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 4 ) // 4��Ԫ��
			{
				if( pData[2] == Long_unsigned_698 )//��ѹ��������
				{
					lib_ExchangeData(Buf,  &pData[3], 2);
					if( pData[5] == Double_long_698)//��ѹ�ָ�����
					{
						//lib_ExchangeData( &Buf[2],  &pData[6], 4);
						DoubleLongOrLongToAbs(&pData[6], &Buf[2], 4);
						if( pData[10] == Double_long_698 )//������������
						{
							//lib_ExchangeData( &Buf[6],  &pData[11], 4);
							DoubleLongOrLongToAbs(&pData[11], &Buf[6], 4);
							if( pData[15] == Unsigned_698 )//�ж���ʱʱ��
							{
								Buf[10] = pData[16];
								DAR = DAR_Success;
							}
						}
					}
				}
			}		
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Long_unsigned_698 )// ��ѹ�������� ���� ��ѹ�ָ�����
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				DAR = DAR_Success;
			}
		}
		else if( ClassIndex < 4 )
		{
			if( pData[0] == Double_long_698 )//������������
			{
				//lib_ExchangeData( Buf,  &pData[3], 4);
				DoubleLongOrLongToAbs(&pData[1], Buf, 4);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//�ж���ʱʱ��
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ���ù������������򡢹����¼���¼�����ò���
//         
//����:      WORD OI[in]             ����OI
//         
//         BYTE ClassIndex[in]     Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                  
//����ֵ:     BYTE  DAR���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventDemandOver( WORD OI,BYTE ClassIndex,BYTE *pData )//���� �������� ����
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}
	
	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 2 ) // 2��Ԫ��
			{
				if( pData[2] == Double_long_unsigned_698)//��ѹ�������� ������
				{
					lib_ExchangeData( Buf,  &pData[3], 4);
					if( pData[7] == Unsigned_698 )//�ж���ʱʱ��
					{
						Buf[4] = pData[8];
						DAR = DAR_Success;
					}
				}
			}	
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Double_long_unsigned_698 )//��ѹ�������� ������
			{
				lib_ExchangeData( Buf,  &pData[1], 4);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//�ж���ʱʱ��
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}

//--------------------------------------------------
//��������:    ���û�������Ȧ����¼���¼���ò���
//         
//����:      WORD OI[in]            ����OI
//         
//         BYTE ClassIndex[in]    Ԫ������
//         
//         BYTE *pData[in]        DATA����ָ��                
//         
//����ֵ:     BYTE  DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventRogowski_Change( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}

	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 2 ) // 2��Ԫ��
			{
				if( pData[2] == Long_unsigned_698 )//�����ж���ʱ
				{
					lib_ExchangeData(Buf, &pData[3], 2);
					if( pData[5] == Long_unsigned_698 )//�Ͽ��ж���ʱ
					{
						lib_ExchangeData( &Buf[2],  &pData[6], 2);
						DAR = DAR_Success;
					}
				}
			}		
		}

	}
	else
	{
		if( pData[0] == Long_unsigned_698 )// �����ж���ʱ ���� �Ͽ��ж���ʱ
		{
			lib_ExchangeData( Buf,  &pData[1], 2);
			DAR = DAR_Success;
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara(OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}

//--------------------------------------------------
//��������:    �����ն˳���ʧ���¼���¼���ò���
//         
//����:      WORD OI[in]            ����OI
//         
//         BYTE ClassIndex[in]    Ԫ������
//         
//         BYTE *pData[in]        DATA����ָ��                
//         
//����ֵ:     BYTE  DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventRogowskiComm_Fail( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}

	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 2 ) // 2��Ԫ��
			{
				if( pData[2] == Unsigned_698 )//���Դ���
				{
					Buf[0] = pData[3];
					if( (pData[4] == Unsigned_698) || (pData[4] == NULL_698) )//�����ɼ������ ��ȡ����
					{
						// Buf[1] = pData[5];
						DAR = DAR_Success;
					}
				}
			}		
		}

	}
	else
	{
		if( (pData[0] == Unsigned_698) && ( ClassIndex == 1 ) )
		{
			Buf[0] = pData[1];
			DAR = DAR_Success;
		}
		else if( ((pData[4] == Unsigned_698) || (pData[4] == NULL_698)) && ( ClassIndex == 2 ) )//�����ɼ������ ��ȡ����
		{
			DAR = DAR_Success;
		}

	}
	
	if( DAR == DAR_Success)
	{
		if(ClassIndex == 2)
		{
			Result = TRUE;
		}
		else
		{	
			Result = api_WriteEventPara(OI, 1, Buf); //ֻд��һ��Ԫ������
		}
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//��������:    ����long_unsigned�ṹ�������¼���¼�����ò���
//         
//����:      WORD OI[in]             ����OI
//         
//         BYTE ClassIndex[in]   Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                 
//����ֵ:     BYTE  DAR���    
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventLong_Unsigned_Para( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}
	

	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 2 ) // 2��Ԫ��
			{
				if( pData[2] == Long_698)//��ѹ�������� ������
				{
					//lib_ExchangeData( Buf,  &pData[3], 2);
					DoubleLongOrLongToAbs(&pData[3], Buf, 2);
					if( pData[5] == Unsigned_698 )//�ж���ʱʱ��
					{
						Buf[2] = pData[6];
						DAR = DAR_Success;
					}
				}
			}		
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Long_698 )//��ѹ�������� ������
			{
				//lib_ExchangeData( Buf,  &pData[3], 2);
				DoubleLongOrLongToAbs(&pData[1], Buf, 2);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//�ж���ʱʱ��
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
#endif//if( MAX_PHASE != 1 )
//--------------------------------------------------
//��������:    ����long_unsigned�ṹ�������¼���¼�����ò���
//         
//����:      WORD OI[in]             ����OI
//         
//         BYTE ClassIndex[in]   Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                 
//����ֵ:     BYTE  DAR���    
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventDouble_Long_Unsigned_Para( WORD OI,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 3 )//���ߵ����쳣��3������
	{
		return DAR;
	}

	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 3 ) // 3��Ԫ��
			{
				if( pData[2] == Double_long_698)
				{
					DoubleLongOrLongToAbs(&pData[3], &Buf[0], 4);
					//lib_ExchangeData( Buf,  &pData[3], 4);
					if( pData[7] == Long_unsigned_698 )
					{
						lib_ExchangeData( &Buf[4],  &pData[8], 2);
						//DoubleLongOrLongToAbs(&pData[8], &Buf[4], 2);
						if( pData[10] == Unsigned_698 )
						{
							Buf[6] = pData[11];
							DAR = DAR_Success;
						}
					}
				}
			}
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Double_long_698 )//��ѹ�������� ������
			{
				DoubleLongOrLongToAbs( &pData[1], Buf , 4);
				DAR = DAR_Success;
			}
		}
		else if( ClassIndex == 2 ) 
		{
			if( pData[0] == Long_unsigned_698 )//��������������
			{
				lib_ExchangeData( Buf,  &pData[1], 2);
				//DoubleLongOrLongToAbs(&pData[1], Buf, 2);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//�ж���ʱʱ��
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}

//--------------------------------------------------
//��������:    ����Double_unsigned�������¼���¼�����ò���
//         
//����:      WORD OI[in]             ����OI
//         
//         BYTE ClassIndex[in]     Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                  
//����ֵ:     BYTE  DAR���   
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEvent_Double_unsigned_Para( WORD OI,BYTE ClassIndex,BYTE *pData )//���� �������� ����
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	
	if( ClassIndex > 2 )
	{
		return DAR ;
	}
		
	if( ClassIndex == 0 )//������������
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if( pData[1] == 2 ) // 2��Ԫ��
			{
				if( pData[2] == Double_long_698)//��ѹ�������� ������
				{
					//lib_ExchangeData( Buf,  &pData[3], 4);
					DoubleLongOrLongToAbs(&pData[3], Buf, 4);
					if( pData[7] == Unsigned_698 )//�ж���ʱʱ��
					{
						Buf[4] = pData[8];
						DAR = DAR_Success;
					}
				}
			}	
		}
	}
	else
	{
		if( ClassIndex == 1 )
		{
			if( pData[0] == Double_long_698 )//��ѹ�������� ������
			{
				//lib_ExchangeData( Buf,  &pData[3], 4);
			    DoubleLongOrLongToAbs(&pData[1], Buf, 4);
				DAR = DAR_Success;
			}
		}
		else
		{
			if( pData[0] == Unsigned_698 )//�ж���ʱʱ��
			{
				Buf[0] = pData[1];
				DAR = DAR_Success;
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ����unsigned�ṹ�������¼���¼�����ò���
//         
//����:      WORD OI[in]             ����OI
//         
//         BYTE ClassIndex[in]   Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                  
//����ֵ:     BYTE  DAR���     
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEventUnsignedUnion( WORD OI,BYTE ClassIndex,BYTE *pData )//������Դ����  ��ѹ������  ����������  �����¼�
{
	BYTE DAR,Buf[30];
	BOOL Result;
	
	DAR = DAR_WrongType;
	if( (ClassIndex ==0) || (ClassIndex == 1) )//clasindexes ����Ϊ0����1
	{
		if( ClassIndex != 0 )
		{
			pData = DealStructureClassIndexNotZeroData(pData);
		}	
		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 1 )
			{
				if( pData[2] == Unsigned_698 )
				{
					Buf[0] = pData[3];
					DAR = DAR_Success;
				}
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf);

		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//��������:  �����¼����ò���
//         
//����:      WORD Type[in]           ����OI
//         
//         BYTE ClassIndex[in]     Ԫ������
//         
//         BYTE *pData[in]         DATA����ָ��
//                 
//����ֵ:     BYTE  DAR���    
//         
//��ע����:  ��
//--------------------------------------------------
BYTE SetEventConfigPara( WORD Type,BYTE ClassIndex,BYTE *pData )
{	
	BYTE DAR;
        
    DAR = DAR_WrongType;
        
	switch( Type )
	{
#if( MAX_PHASE != 1 )	
	case 0x3000://���ܱ�ʧѹ�¼�
		DAR = SetEventLost_V( Type,ClassIndex, pData);
		break;

	case 0x3001://���ܱ�Ƿѹ�¼�
	case 0x3002://���ܱ��ѹ�¼�
		DAR = SetEventWeakOrOver_V( Type, ClassIndex, pData);
		break;

	case 0x3003://���ܱ�����¼�
	case 0x3006://���ܱ�����¼�
		DAR = SetEventBreakOrBreak_I( Type, ClassIndex, pData);
		break;

	case 0x3004://���ܱ�ʧ���¼�
		DAR = SetEventLost_I( Type,ClassIndex, pData);
		break;
		
	case 0x3007://���ܱ��ʷ����¼�
	case 0x3008://���ܱ�����¼�
		DAR = SetEvent_Double_unsigned_Para( Type,ClassIndex, pData);
		break;
		
	case 0x3009://���ܱ������й����������¼�
	case 0x300a://���ܱ����й����������¼�
	case 0x300b://���ܱ��޹����������¼�
		DAR = SetEventDemandOver( Type, ClassIndex, pData);
		break;
		
	case 0x301D://���ܱ��ѹ��ƽ���¼�
	case 0x301E://���ܱ������ƽ���¼�
	case 0x303B://���ܱ��������������¼�
	case 0x302D://���ܱ�������ز�ƽ��
		DAR = SetEventLong_Unsigned_Para( Type, ClassIndex, pData);
		break;
	case 0x3053://��������Ȧ����¼�
		DAR = SetEventRogowski_Change( Type, ClassIndex, pData);
		break;
	case 0x310F://�ն˳���ʧ���¼�
		DAR = SetEventRogowskiComm_Fail( Type, ClassIndex, pData);
		break;
#endif
	case 0x3005://���ܱ�����¼�
		DAR = SetEvent_Double_unsigned_Para( Type,ClassIndex, pData);
		break;
#if( MAX_PHASE != 1 )	
	case 0x300f://���ܱ��ѹ�������¼�
	case 0x3010://���ܱ�����������¼�
	case 0x302C://���ܱ��Դ�쳣
	case 0x300e://���ܱ�����Դ�����¼��¼�
	 	DAR = SetEventUnsignedUnion( Type,ClassIndex, pData);
	    break;
#endif	
	case 0x302F://���ܱ����оƬ�����¼�
	    DAR = SetEventUnsignedUnion( Type,ClassIndex, pData);
	    break;
	case 0x3040://���ܱ����ߵ����쳣�¼�
	    DAR = SetEventDouble_Long_Unsigned_Para( Type,ClassIndex, pData);
	    break;
		
	case 0x3011://���ܱ�����¼�
		DAR = DAR_RefuseOp;
		break;
		
	default:
		break;
	}
    return DAR;
}
//--------------------------------------------------
//��������:    �����¼��ϱ���ʶ
//         
//����:      WORD OI[in]      ����OI
//         
//         BYTE *pData[in]  ����ָ��
//                  
//����ֵ:     BYTE DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetRequestEventReportMode( eReportClass ReportClass, WORD OI, BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	
	DAR = DAR_WrongType;
	
	if( pData[0] == Enum_698)
	{
		Result = api_SetEventReportMode( ReportClass, OI, pData[1] );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
		
	}

	return DAR;
}
//--------------------------------------------------
//��������:    �����¼���Ч��ʶ
//         
//����:      WORD OI[in]      ����OI
//         
//         BYTE *pData[in]  ����ָ��
//                          
//����ֵ:     BYTE DAR���
//         
//��ע����: ��
//--------------------------------------------------
BYTE SetRequestEventValidFlag(           WORD OI, BYTE *pData )
{
	BYTE DAR;

	
	DAR = DAR_WrongType;
	
	if( pData[0] == Boolean_698 )
	{
	    if( pData[1] == 1 )//ֻ��������Ϊ��Ч
	    {
            DAR = DAR_Success;
	    }
	    else
	    {
            DAR = DAR_RefuseOp;
	    }		
	}

	return DAR;
}
//--------------------------------------------------
//��������: �����¼�����
//         
//����:      BYTE Ch[in]   ͨ��ѡ��
//         
//         BYTE *pOAD[in] oadָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע:
//--------------------------------------------------
eAPPBufResultChoice SetRequestEvent( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType; 

    if((OI.w == 0x3013) && (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE) 
    && ( ClassAttribute == 3) )//˽Կ״̬�²���������¼���������������Ա�
    {
        DAR = DAR_RefuseOp; 
    }
	else if( ((OI.w >=0x3000) && (OI.w <= 0x3008)) ||( OI.w == 0x300b)||( OI.w == 0x303b) ) //���ò��� ֮����������������Ϊ���Բ�һ��
	{
		switch( ClassAttribute )
		{
			case 2://�����������Ա�
				DAR = SetEventRelationTable( OI.w, ClassIndex, pData);
				break;

			case 5://���ò���
				DAR = SetEventConfigPara( OI.w,  ClassIndex, pData);
				break;

			case 11://�ϱ���ʶ
				DAR = SetRequestEventReportMode( eReportMode, OI.w, pData);
				break;

			case 12://��Ч��ʶ
				DAR = SetRequestEventValidFlag( OI.w, pData);
				break;
			case 15://�ϱ���ʽ
				DAR = SetRequestEventReportMode( eReportMethod, OI.w, pData);
				break;
		}
		
	}
	else
	{
		switch( ClassAttribute )
		{
			case 3://�����������Ա�
				DAR = SetEventRelationTable( OI.w, ClassIndex, pData);
				break;

			case 6://���ò���
				DAR = SetEventConfigPara( OI.w,  ClassIndex, pData);
				break;

			case 8://�ϱ���ʶ
				DAR = SetRequestEventReportMode( eReportMode, OI.w, pData);
				break;

			case 9://��Ч��ʶ
				DAR = SetRequestEventValidFlag( OI.w, pData);
				break;
			case 11://�ϱ���ʽ
				DAR = SetRequestEventReportMode( eReportMethod, OI.w, pData);
				break;
		}
	}

	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
    return eResultChoice;
}
//--------------------------------------------------
//��������:    ���ö����������
//         
//����:      WORD Type[in]		    ��������OI
//         
//         BYTE ClassAttribute[in]	��������
//         
//         BYTE ClassIndex[in]	    ����Ԫ������
//         
//         BYTE *pData[in]          ����ָ��
//                  
//����ֵ:     BYTE  DAR��� 
//         
//��ע����:  ��
//--------------------------------------------------
BYTE SetFreezeRelationTable( WORD Type,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[MAX_FREEZE_ATTRIBUTE*8+30],i,OadType;		//���Ŀ����õĹ����������Ը�����54����Ӧ�����㹻�Ŀռ������ڳ��ڴ�����--ml
    WORD Result;
        
	Result = FALSE;
	DAR = DAR_WrongType;
	OadType = 0;
	i = 0;
	
	if( ClassAttribute != 0x03)//ֻ����������3
	{
		return DAR_Undefined;
	}
	
	if( ClassIndex != 0 )//���õ�������
	{
		return DAR_RefuseOp;
	}
	
	if( pData[0] == Array_698 )
	{
	    if( pData[1] <= MAX_FREEZE_ATTRIBUTE )//���ܳ����������������
	    {
            for( i=0; i< pData[1]; i++ )
            {
                if( pData[2+13*i] == Structure_698 )
                {
                    if( pData[3+13*i] == 3 )
                    {
                        lib_ExchangeData( &Buf[8*i], &pData[3+2+13*i],2);//��������
                        memcpy( &Buf[2+8*i], &pData[3+2+2+1+13*i],4);//OAD
                        lib_ExchangeData( &Buf[6+8*i], &pData[3+2+2+1+4+1+13*i],2);//�������
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            if( i == pData[1] )//û�д���
            {
                DAR = DAR_Success;
            }
	    }
	    else
	    {
            DAR = DAR_OverRegion;
	    }
	}

	if( DAR == DAR_Success)
	{
		Result = api_WriteFreezeAttribue( Type, OadType,Buf, pData[1]);
		if( Result == FALSE )
		{
		    DAR = DAR_RefuseOp;
		}
		else
		{
		    DAR = DAR_Success;
		}
	}
        
    return DAR;
}
//--------------------------------------------------
//��������:    ���ö�������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOAD[in]       OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetRequestFreeze( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType; 
	
	DAR = SetFreezeRelationTable( OI.w, ClassAttribute, ClassIndex, pData);
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
	
    return eResultChoice;
}
//--------------------------------------------------
//��������:    ���ÿ����������־
//         
//����:      
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetDIInputFlag(BYTE Ch, BYTE *pOAD )
{
	BYTE Buf[20],DAR,i,ClassAttribute,ClassIndexes,bInputState,bAttributeState;
	BYTE *pData;
	WORD MaxNum,Result;
	TTwoByteUnion OI,Len;
	eAPPBufResultChoice eResultChoice;

	Result = FALSE;
	DAR = DAR_WrongType;
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndexes = pOAD[3];
	Len.w = 0;

	switch ( ClassAttribute )
	{
		case 0x04:
			if( OI.w != 0xF203)
			{
				DAR = DAR_Undefined;
			}
			else if(ClassIndexes == 0)
			{
				if(pData[Len.w] == Structure_698)
				{
					Len.w += 1;
					if(pData[Len.w] == 2)//����Ԫ��
					{
						Len.w += 1;
						if(pData[Len.w] == Bit_string_698)
						{
							Len.w += 2;
							lib_ExchangeBit((BYTE*)&bInputState,( pData + Len.w),1);
							if(bInputState > 1)
							{
								DAR = DAR_OverRegion;
							}
							else
							{
								Len.w += 1;
								if(pData[Len.w] == Bit_string_698)
								{
									Len.w += 2;
									lib_ExchangeBit((BYTE*)&bAttributeState,( pData + Len.w),1);	
									if(bAttributeState > 1)
									{
										DAR = DAR_OverRegion;
									}
									else
									{
										api_WriteDIPara(1,(BYTE*)&bInputState);
										api_WriteDIPara(2,(BYTE*)&bAttributeState);
										DAR = DAR_Success;
									}
								}
							}
						}
						
					}
				}
			}
			break;
		default :
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������

	return eResultChoice;
}

//--------------------------------------------------
//��������:    ����LCD����
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetRequestLCD( BYTE Ch, BYTE *pOAD )
{
	BYTE Buf[20],DAR,i,ClassAttribute,ClassIndexes,ClassIndexesBak,Type,Num,LCDSequence;
	BYTE *pData;
	WORD MaxNum,Result;
	TTwoByteUnion OI,Len;
	eAPPBufResultChoice eResultChoice;

	Result = FALSE;
	DAR = DAR_WrongType;
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndexes = pOAD[3];
	Len.w = 0;

	ClassIndexesBak = ClassIndexes;
	
	switch ( ClassAttribute )
	{
		case 2://������ʾ�����б� 
			if( OI.w == 0xf300)//ȡ�����������
			{
				MaxNum =  MAX_LCD_LOOP_ITEM;
			}
			else
			{
				MaxNum =  MAX_LCD_KEY_ITEM;
			}

			if( ClassIndexes != 0 )
			{ 
				ClassIndexes = ClassIndexes-1;
				pData = DealArrayClassIndexNotZeroData(pData);//�Ը�ʽΪ���ݵ�Ԫ�� ��Ԫ��������Ϊ��ʱ �������ݴ���
			}
			
			if( pData[1] <= MaxNum )
			{
				if( pData[0] == Array_698 )
				{
					for( i=0; i<pData[1]; i++)
					{
						if( pData[2+Len.w] == Structure_698 )
						{
							if( pData[3+Len.w] == 2 )
							{
								if( pData[4+Len.w] == CSD_698 )
								{
									if( pData[5+Len.w] == 0)//ѡ��OAD
									{
										memcpy( Buf, &pData[6+Len.w],4);//ȡOAD
										Num = 1;
										LCDSequence = pData[11+Len.w];
										DAR = DAR_Success; 
										Len.w += 10;//�Ѵ������ݳ���
									}
									else if( pData[5+Len.w] == 1)//ѡ��ROAD
									{
										memcpy( Buf, &pData[6+Len.w], 4);
										memcpy( &Buf[4], &pData[11+Len.w], 4);
										
										#if(MAX_PHASE == 1)
										//�ն���(5004)�Լ������ն���(5005) ��E2�б���
										//����͹�������ʾ,���ڵ͹��Ĳ���flash��������spi�����¿��Ź���λ
										if( (Buf[0] == 0x50)&&((Buf[1] == 0x04)||(Buf[1] == 0x05)) )
										{}
										else
										{
											DAR = DAR_WrongType;
											break;
										}
										#endif
										
										LCDSequence = pData[11+Len.w+4+1];
                                        Num = 2;
										DAR = DAR_Success; 
										Len.w += 1+1+1+1+4+1+4+1+1;//�Ѵ������ݳ��� ������OAD
									}
									else
									{
										DAR = DAR_WrongType;
										break;
									}
									
									if( DAR == DAR_Success )
									{
										Result = api_WriteLcdItem( (OI.w -0xf300) , i+ClassIndexes, Num, Buf,LCDSequence);
										if( Result == FALSE )
										{
											DAR = DAR_HardWare;//�̶����� ����������� 
											break;
										}
										else
										{
											DAR = DAR_Success;
										}
									}
								}
							}
						}
					}

					if( (DAR == DAR_Success) && (ClassIndexesBak == 0) )//ֻ��������ȫ����ʾ��ʱ�ſ��Ըı���ʾ����
					{
                        Type = (OI.w -0xf300);
        				Result = api_WritePara(0, GET_STRUCT_ADDR( TFPara1,LCDPara.DispItemNum[0])+Type, 1, (BYTE*)&pData[1] );
        				if( Result == FALSE )
        				{
        					DAR = DAR_HardWare;
        				}
					}
				}
			}	
			break;
		
		case 3://ÿ��������ʾʱ��
			if( ClassIndexes < 1 )
			{
				if( pData[0] == Long_unsigned_698 )
				{
					lib_ExchangeData( Buf, &pData[1], 2);
					DAR = DAR_Success;
				}
			}
			
			Type = (OI.w -0xf300);
			
			if( Type == 0 )//ѭ�Խ��м���ֵ�ж�
			{
                if( (pData[2]<5) || (pData[2]>20) )
                {
                    DAR = DAR_OverRegion;
                }
			}

            
			if( DAR == DAR_Success )
			{
				
				if( Type == 0 )
				{
					Result = api_WritePara(0, GET_STRUCT_ADDR( TFPara1,LCDPara.LoopDisplayTime), 2, Buf );
				}
				else
				{
					Result = api_WritePara(0, GET_STRUCT_ADDR( TFPara1,LCDPara.KeyDisplayTime), 2, Buf );
				}
				
				if( Result == FALSE )
				{
					DAR = DAR_HardWare;
				}
					
			}
			break;
		
		case 4://��ʾ����
		
			if( OI.w == 0xf300)//ȡ�����������
			{
				MaxNum =  MAX_LCD_LOOP_ITEM;
			}
			else
			{
				MaxNum =  MAX_LCD_KEY_ITEM;
			}
			
			if( ClassIndexes < 3 )
			{
                if( ClassIndexes == 0 )
                {
                    if( pData[0] == Structure_698 )
                    {
                        if( pData[1] == 2 )
                        {
                            if( pData[2] == Unsigned_698 )
                            {
                                if( pData[4] == Unsigned_698 )
                                {
                                    if( pData[3] <= MaxNum )
                                    {
                                        Buf[0] = pData[3];
                                        DAR = DAR_Success; 
                                    }
                                    else
                                    {
                                        DAR = DAR_OverRegion;
                                    }
                                    
                                }
                            }
                        }
                    }
                }
                else if( ClassIndexes == 1 )//������ʾ����
                {
                    if( pData[0] == Unsigned_698 )
                    {
                        if( pData[1] <= MaxNum )
                        {
                            Buf[0] = pData[1];
                            DAR = DAR_Success; 
                        }
                        else
                        {
                            DAR = DAR_OverRegion;
                        }
                    }
                }
                else
                {
                    DAR = DAR_RefuseOp;
                }
				
			}

			if( DAR == DAR_Success )
			{
				Type = (OI.w -0xf300);
				Result = api_WritePara(0, GET_STRUCT_ADDR( TFPara1,LCDPara.DispItemNum[0])+Type, 1, Buf );
				if( Result == FALSE )
				{
					DAR = DAR_HardWare;
				}
				else
				{					
					//api_ResetBorrDotNum(0);
				}
			}
			break;
		
		default :
			break;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
	
	return eResultChoice;      
}

//--------------------------------------------------
//��������:    ����Զ�̿��Ʋ���
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//                
//����ֵ:     BYTE DAR���
//         
//��ע����:  ��
//--------------------------------------------------
BYTE SetRemoteControlPara(           BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[10];
	WORD Result;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	
	if( ClassAttribute == 2)
	{
		if( ClassIndex == 0)
		{
			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 2)
				{
					if( pData[2] == Double_long_unsigned_698 )
					{
						lib_ExchangeData( Buf, &pData[3],4);
						Result = WriteRelayPara(0,Buf);
						if( Result == TRUE )
						{
							if( pData[7] == Long_unsigned_698 )
							{
								lib_ExchangeData( Buf, &pData[8],2);
								Result= WriteRelayPara(1,Buf);
								if( Result == FALSE )
								{
									DAR = DAR_HardWare; //�̶����� ����������� ���ϲ����
								}
								else
								{
									DAR = DAR_Success; 
								}
							}
						}
						else
						{
							DAR = DAR_HardWare; //�̶����� ����������� ���ϲ����
						}
					}
				}
			}
		}
		else if( ClassIndex == 1 )
		{
			pData = DealStructureClassIndexNotZeroData(pData);

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 1)
				{
					if( pData[2] == Double_long_unsigned_698 )
					{
						lib_ExchangeData( Buf, &pData[3],4);
						Result = WriteRelayPara(0,Buf);
						if( Result == FALSE )
						{
							DAR = DAR_HardWare; //�̶����� ����������� ���ϲ����
						}
						else
						{
							DAR = DAR_Success; 
						}
					}
				}
			}
		}
		else if( ClassIndex == 2 )
		{
			pData = DealStructureClassIndexNotZeroData(pData);

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 1)
				{
					if( pData[2] == Long_unsigned_698 )
					{
						lib_ExchangeData( Buf, &pData[3],2);
						Result = WriteRelayPara(1,Buf);
						if( Result == FALSE )
						{
							DAR = DAR_HardWare; //�̶����� ����������� ���ϲ����
						}
						else
						{
							DAR = DAR_Success; 
						}
					}
				}
			}
		}
	}
	
    return DAR;
}
//--------------------------------------------------
//��������:    ����Զ�̿�������
//         
//����:      BYTE Ch[in]		ͨ��
//         
//         BYTE *pOAD[in]	OADָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf��� 
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetRequestControl( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;

	DAR = DAR_WrongType;
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	
	if( OI.w == 0x8000 )//ң��
	{
		DAR = SetRemoteControlPara( ClassAttribute, ClassIndex, pData);
	}
	else//���� ��δ֧��
	{
		
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
	
    return eResultChoice;
        
}

//--------------------------------------------------
//��������:    ����LCD����
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetLCDPara( BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData )
{
	BYTE Buf[20],DAR,Length;
	WORD Result;
	BYTE lcdParaOffset;

	DAR = DAR_WrongType;
	Result = FALSE;
	
	if( ClassAttribute == 2)
	{
		if( ClassIndexes == 0)
		{
			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 7)
				{
					if( pData[2] == Unsigned_698 )
					{
					    if( (pData[3] < 5) ||(pData[3] > 30) )//�ϵ�ȫ��ʱ�䷶Χ�ж�
					    {
                            return DAR_OverRegion;
					    }
						Buf[0] = pData[3];//�ϵ�ȫ��ʱ��
						if( pData[4] == Long_unsigned_698 )
						{
							lib_ExchangeData( &Buf[1] , &pData[5], 2);//�������ʱ��
							if( pData[7] == Long_unsigned_698 )
							{
								lib_ExchangeData( &Buf[3] , &pData[8], 2);//��ʾ�鿴�������ʱ��
								if( pData[10] == Long_unsigned_698 )
								{
									lib_ExchangeData( &Buf[5] , &pData[11], 2);//�޵簴����Ļפ�����ʱ��
									if( pData[13] == Unsigned_698 )
									{
                                        if( pData[14] > 4 )//��ʾ����С��λ����Χ�ж�
                                        {
                                            return DAR_OverRegion;
                                        }
										Buf[7] = pData[14];//��ʾ����С��λ��
										if( pData[15] == Unsigned_698 )
										{
											Buf[8] = pData[16];//��ʾ����С��λ��
											if( pData[17] == Unsigned_698 )//Һ��1,2��������
											{
												Buf[9] = pData[18];
												if( Buf[9] > 1 )//Һ���٢��������壺0��ʾ��ǰ�ס�������ʱ�Σ�1��ʾ��ǰ�ס������׷��ʡ�
												{
													return DAR_OverRegion;
												}
											}
											Length = 10;
											lcdParaOffset = 0;
											DAR = DAR_Success;
										}
									}			
								}
							}
						}
					}
				}
			}
		}
		else if( ClassIndexes < 8 )
		{

			pData = DealStructureClassIndexNotZeroData(pData);

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 01 )
				{
					if( (ClassIndexes >= 2) && ( ClassIndexes <= 4))
					{
						if( pData[2] == Long_unsigned_698 )
						{
							lib_ExchangeData( Buf, &pData[3], 2);//��ȡ2���ֽ�
							Length = 2;
							DAR = DAR_Success;
						}
						
					}
					else
					{
						if( pData[2] == Unsigned_698 )
						{
                            if( ClassIndexes == 1 )
                            {
                                if( (pData[3] < 5) ||(pData[3] > 30) )//�ϵ�ȫ��ʱ�䷶Χ�ж�
                                {
                                    return DAR_OverRegion;
                                }
                            }
                            else if( ClassIndexes == 5 )
                            {
                                if( pData[3] > 4 )//��ʾ����С��λ����Χ�ж�
                                {
                                    return DAR_OverRegion;
                                }
                            }
                            
							Buf[0] = pData[3];
							Length = 1;
							DAR = DAR_Success;
						}
					}

					if( ClassIndexes == 1 )
					{
						lcdParaOffset = 0;
					}
					else if( ClassIndexes < 5 )
					{
						lcdParaOffset = (1 + ( ClassIndexes-2)*2);
					}
					else
					{
						lcdParaOffset = (1 + 3*2 + (ClassIndexes - 5));
					}
				}
			}
		}
	}

	if( DAR == DAR_Success)//�޴��������½������ò���
	{
		Result = api_WritePara( 0,  GET_STRUCT_ADDR(TFPara1, LCDPara.PowerOnDispTimer)+lcdParaOffset, Length, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//�˷��ؽ������������ģ�����ʱ�����ϲ�������
		}
		else
		{
			DAR = DAR_Success;
			//�޸��˵���С��λ��
			if((ClassIndexes == 0) || ( ClassIndexes == 5 ))
			{
				//�ѵ���С��λ��д���λ���ܵ���С��λ��
				//api_ResetBorrDotNum(1);
			}
		}
	}

	return DAR;
        
}
//--------------------------------------------------
//��������:    ���õ���λ����Ϣ
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetPsitionInfo( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,OffestLen,Buf[20];
	WORD Result;
	
	DAR = DAR_WrongType;
	Result = FALSE;
	
	if( ClassAttribute != 0x02)//ֻ����������2
	{
		return DAR_Undefined;
	}
	Result = api_ProcMeterTypePara( READ, eMeterMeterPosition, Buf);
 	if( Result == FALSE)
	{
		return DAR_HardWare;//DAR Ӳ��ʧЧ
	}
	
	if( ClassIndex == 0)
	{
		if( pData[0] == Structure_698 )//�ṹ��
		{
			if(pData[1] == 3)           //����
			{
				if( pData[2] == Structure_698 )//���Ƚṹ��
				{
					if( pData[3] == 3 )//����
					{
						if(pData[4] == Enum_698 )//��λ
						{
							Buf[0] = pData[5]; 
							if( Buf[0] > 1 )//���ȷ�λ   enum{E��0����W��1��}��
							{
								return DAR_OverRegion;
							}	
							if( pData[6] == Long_unsigned_698 )//��
							{
								lib_ExchangeData( &Buf[1],&pData[7],2);
								if( pData[9] == Float32_698 )//��
								{
									lib_ExchangeData( &Buf[3],&pData[10],4);
									if( pData[14] == Structure_698 )//γ�Ƚṹ��
									{
										if( pData[15] == 3 )//����
										{
											if(pData[16] == Enum_698 )//��λ
											{
												Buf[7] = pData[17]; 
												if( Buf[7] > 1 )//γ�ȷ�λ   enum{S��0����N��1��}��
												{
													return DAR_OverRegion;
												}
												if( pData[18] == Long_unsigned_698 )//��
												{ 
													lib_ExchangeData( &Buf[8],&pData[19],2);
													if( pData[21] == Float32_698 )//��
													{
														lib_ExchangeData( &Buf[10],&pData[22],4);
														if( pData[26] == Double_long_698)
														{	
															lib_ExchangeData(&Buf[14], &pData[27], 4);
															DAR = DAR_Success;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if(( ClassIndex == 1) || ( ClassIndex == 2))
	{
		OffestLen = (ClassIndex == 2)? 7:0;//ȷ��ƫ�Ƶ�ַ
		
		if( pData[0] == Structure_698 )//���Ƚṹ��
		{
			if( pData[1] == 3 )//����
			{
				if(pData[2] == Enum_698 )//��λ
				{
					Buf[OffestLen] = pData[3]; 
					if( Buf[OffestLen] > 1 )//���ȷ�λ   enum{E��0����W��1��}��//γ�ȷ�λ   enum{S��0����N��1��}��
					{
						return DAR_OverRegion;
					}	
					if( pData[4] == Long_unsigned_698 )//��
					{
						lib_ExchangeData( &Buf[OffestLen+1],&pData[5],2);
						if( pData[7] == Float32_698 )//��
						{
							lib_ExchangeData( &Buf[OffestLen+3],&pData[8],4);
							DAR = DAR_Success;
						}
					}
				}
			}
		} 
	}
	else if( ClassIndex == 3)
	{
		if( pData[0] == Double_long_698)
		{	
			lib_ExchangeData(&Buf[14], &pData[1], 4);
			DAR = DAR_Success;
		} 
	}

	if( DAR == DAR_Success)//����������������
	{
 		Result = api_ProcMeterTypePara( WRITE, eMeterMeterPosition, Buf);
	 	if( Result == FALSE)
		{
			DAR = DAR_HardWare;//DAR Ӳ��ʧЧ
		}
		else
		{			
			DAR = DAR_Success;//DAR �ɹ�
		}
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ����ʱ��ʱ�α���
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE Set_Timezone_TimeInterval_Num( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Buf[10],Length;
	WORD Result;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	Length = 0;
	
	if( ClassAttribute != 0x02)//ֻ����������2
	{
		return DAR_Undefined;
	}

	if( ClassIndex == 0)//Ԫ������Ϊ��
	{
		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 0x05 )
			{
				if( pData[2] == Unsigned_698 )//��ʱ���� ������Ϊ0������
				{
					if((pData[3] <= MAX_TIME_AREA) && (pData[3] > 0) )
					{
						Buf[0] = pData[3];
						if( pData[4] == Unsigned_698 )//��ʱ�α��� ������Ϊ0������
						{
							if( (pData[5] <= MAX_TIME_SEG_TABLE) && (pData[5] > 0) )
							{
								Buf[1] = pData[5];
								if( pData[6]== Unsigned_698 )//��ʱ���� ������Ϊ0������
								{
									if( (pData[7] <= MAX_TIME_SEG) && (pData[7] > 0) )
									{
										Buf[2] = pData[7];
										if( pData[8] == Unsigned_698 )//������
										{
											if( pData[9] <=  MAX_RATIO )
											{
												Buf[3] = pData[9];
												if( pData[10] == Unsigned_698 )//����������
												{
													if( pData[11] <=  MAX_HOLIDAY )
													{
														Buf[4] = pData[11];
														Length = 5;
														DAR = DAR_Success;
														ClassIndex = 1;
													}
													else
													{
                                                        DAR = DAR_OverRegion;
													}
												}
											}
											else
											{
												DAR = DAR_OverRAte;
											}
										}
									}
									else
									{
										DAR = DAR_OverSlot;
									}
								}
							}
							else
							{
                                DAR = DAR_OverRegion;
							}
						}
					}
					else
					{
						DAR = DAR_OverZone;
					}
					
				}
			}
		}	
	}
	else//Ԫ��������Ϊ��
	{
		if( ClassIndex > 5)
		{
			return DAR;
		}

		pData = DealStructureClassIndexNotZeroData(pData);

		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 0x01 )
			{
				if( pData[2] == Unsigned_698 )//��ʱ����
				{
				    Buf[0] = pData[3];
					Length = 1;
					
				    if( ClassIndex == 1 )//��ʱ����
				    {
                        if( (Buf[0] <= MAX_TIME_AREA) && (Buf[0] > 0) )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverZone;
                        }
				    }
				    else if( ClassIndex == 2 )//��ʱ�α���
				    {
                        if( (Buf[0] <= MAX_TIME_SEG_TABLE) && (Buf[0] > 0) )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverRegion;
                        }
				    }
                    else if( ClassIndex == 3 )//��ʱ����
                    {
                        if( (Buf[0] <= MAX_TIME_SEG) && (Buf[0] > 0) )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverSlot;
                        }
                    }
                    else if( ClassIndex == 4 )//������
                    {
                        if( Buf[0] <= MAX_RATIO )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverRAte;
                        }
                    }
                    else//����������
                    {
                        if( Buf[0] <= MAX_HOLIDAY )
                        {
                            DAR = DAR_Success;
                        }
                        else
                        {
                            DAR = DAR_OverRegion;
                        }
                    }

				}
			}
		}
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_WritePara( 0, (GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.TimeZoneNum) +(ClassIndex-1)), Length, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare; //��Ϊ�̶����أ������������ �����ϴν������ ---jwh
		}
		else
		{
			DAR = DAR_Success; 
		}
	}

	return DAR;
}

#if( PREPAY_MODE == PREPAY_LOCAL)
//--------------------------------------------------
//��������:    ���ý�����
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//         
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE Set_LadderNum( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	
	if( ClassAttribute != 0x02)//ֻ����������2
	{
		return DAR_Undefined;
	}
	
    if( ClassIndex != 0)//Ԫ������Ϊ��
    {
        return DAR_Undefined;
    }

    if( pData[0] == Unsigned_698 )
    {
        Result = api_WritePrePayPara( eLadderNum, (BYTE*)&pData[1] );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare; //��Ϊ�̶����أ������������ �����ϴν������ ---jwh
		}
		else
		{
			DAR = DAR_Success; 
		}
    }

	return DAR;
}
#endif

//--------------------------------------------------
//��������:    ������Կ������
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//         
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetEsamKeyTotalNum( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,KeyTotalNum;
	WORD Result;
	
	Result = FALSE;
	DAR = DAR_WrongType;

	if( (ClassAttribute != 2) || ( ClassIndex != 0 ) )
	{
		return DAR_WrongType;
	}

	if( pData[0] == Unsigned_698 )
	{
		KeyTotalNum = pData[1];//��Կ����
		DAR = DAR_Success;
	}

	if( DAR == DAR_Success)
	{
		Result = api_WritePrePayPara(eKeyNum, (BYTE *)&KeyTotalNum);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare; //��Ϊ�̶����أ������������ �����ϴν������ ---jwh
		}
		else
		{
			DAR = DAR_Success; 
		}
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ���ù������ձ�
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                         
//����ֵ:     BYTE DAR���   
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetTimeHolidayTable( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{	
	BYTE DAR,Buf[10],i;
	WORD Result;
	TFourByteUnion OAD;
	TRealTimer t1;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//��ȡOAD

	if( ClassAttribute != 0x02)//ֻ����������2
	{
		return DAR_Undefined;
	}

	if( ClassIndex == 0 )//Ԫ������������������ �������ݴ���
	{
		return DAR_RefuseOp;
	}
	else//��Ӧif( ClassIndex == 0 ):Ԫ������������������ �������ݴ���
	{	
		pData = DealArrayClassIndexNotZeroData(pData);
		if( ClassIndex > MAX_HOLIDAY )
		{
			return DAR_OverRegion;
		}
	}
	if( pData[0] == Array_698 )
	{
		if( pData[1] <= MAX_HOLIDAY)//�ж���๫��������
		{
			for( i=0;i < pData[1];i++)
			{
				if( pData[2+i*10] == Structure_698 )
				{
					if( pData[3+i*10] == 02 )
					{
						if( pData[4+i*10] == Date_698 )	//ȡ�������� 5���ֽ�
						{
							memset( (void *)&t1, 0x00, sizeof(t1) );
							memcpy( (void *)&(t1.wYear), (void *)&(pData[5+i*10]), 4);
							if( t1.wYear == 0xffff )
							{
								memcpy( Buf, &pData[5+i*10],5 );
							}
							else
							{		
								lib_InverseData((BYTE *)&t1.wYear, 2);
								if( api_CheckMonDay( t1.wYear, t1.Mon, t1.Day ) == FALSE )
								{
									return DAR_OverRegion;
								}
								if( pData[9+i*10] != ( ((api_CalcInTimeRelativeMin(&t1)/1440)+6)%7) )
								{
									return DAR_OverRegion;
								}
								memcpy( Buf, &t1.wYear, 4 );
								Buf[4]= pData[9+i*10];
							}

							/*
							if( api_CheckMonDay( (pData[5+i*10]*0x100+pData[6+i*10]), pData[7+i*10], pData[8+i*10] ) == FALSE )
							{
								return DAR_OverRegion;
							}
							lib_ExchangeData( Buf, &pData[5+i*10], 2 );
							memcpy( Buf+2, &pData[7+i*10],3 );
							*/
							if( pData[10+i*10] == Unsigned_698 )
							{
								Buf[5] = pData[11+i*10];

								// д�������ձ�֮ǰ�ȴ�֮ǰ�ı�
								if(OAD.b[3]==0)// дȫ���Ļ����
								{
									OAD.b[3]=i+1;
									api_WritePreProgramData( 0,OAD.d );
									OAD.b[3]=0;
								}
								else
								{
									api_WritePreProgramData( 0,OAD.d );
								}
								
								if( ClassIndex == 0 )
								{
									Result = api_WriteTimeTable( 2, GET_STRUCT_ADDR(TTimeHolidayTable, TimeHoliday[i]), 6, Buf);
									if( Result == FALSE )//ʧ���˳�
									{
										DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ����ϲ����
										break;
									}
								}	
								else
								{
									if( pData[1] != 1)
									{
										break;
									}
									Result = api_WriteTimeTable( 2, GET_STRUCT_ADDR(TTimeHolidayTable, TimeHoliday[ClassIndex-1]), 6, Buf);
									if( Result == FALSE )//ʧ���˳�
									{
										DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ����ϲ����
										break;
									}
								}

								api_SavePrgOperationRecord( ePRG_HOLIDAY_NO );
							}
						}
						else
						{
							break;
						}
					}
				}

			}
		}
	}
	
	if( Result == TRUE)
	{
		DAR = DAR_Success;
		api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
	}

	return DAR;
}


//--------------------------------------------------
//��������: ��ʱ�β��������ʱ����ʱ�������һ����Ч�Ĳ���
//         
//����:      BYTE byLen[in]            �������ݳ���
//         
//         BYTE byMaxLen[in]         ������ݳ���
//
//         BYTE *pBuf[in]            ����ָ��
//         
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ������ȫ�ֱ��� ProtocolBuf ��
//--------------------------------------------------
void api_AddTimeSegParaExtDataWithLastValid(BYTE byLen, BYTE byMaxLen, BYTE *pBuf )
{
	BYTE i;
	//�����ȡ��ʱ����ʱ�����ݴﲻ����������һ�����ϣ���д��ʱ��������󳤶�
	if( byLen < byMaxLen )
	{
		// ������ʱ����ʱ������14����Ӧʹ�����һ������
		for(i=0; i<(( byMaxLen - byLen)/3); i++)
		{
			memcpy(&pBuf[byLen + 3*i], &pBuf[byLen - 3], 3);
		}
	}
}
//--------------------------------------------------
//��������:    ����ʱ����
//         
//����:      BYTE TYPE[in]             ����OI
//         
//         BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//                 
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetTimezoneTable( BYTE Type,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//ʱ��
{
	BYTE DAR,i;
	WORD Result;
	TFourByteUnion OAD;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	OAD.d = 0x00021540;
	
	if( (api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE) && (Type == 0x01) )//�ǳ���ģʽ�²���������ʱ����
	{
		return DAR_RefuseOp;//DAR_OtherErr;
	}
	
	if( ClassAttribute != 0x02)//ֻ����������2
	{
		return DAR_Undefined;
	}
	if( ClassIndex != 0)
	{
		pData = DealArrayClassIndexNotZeroData(pData);//Ԫ��������Ϊ��ʱ�����ݽ��д���
	
		if( ClassIndex > MAX_TIME_AREA)//ֻ����������2
		{
			return DAR_OverRegion;
		}
	}

	if( pData[0] == Array_698 )
	{
		if( pData[1] > MAX_TIME_AREA)
		{
			pData[1] = MAX_TIME_AREA;
		}
		
		Result = DealTimeZoneSegTableData( &pData[1] );
	}
	
	if( Result == TRUE )//�Ѿ��жϵ����һ��ʱ��������δ����
	{
		for( i=0; i<pData[1]; i++ )
		{
			if( api_CheckMonDay( RealTimer.wYear, ProtocolBuf[i*3], ProtocolBuf[i*3+1] ) == FALSE )
			{
				return DAR_OverRegion;
			}
		}
		
		api_WritePreProgramData( 0,OAD.d );
		if( ClassIndex == 0)
		{
			api_AddTimeSegParaExtDataWithLastValid(3*pData[1], (MAX_TIME_AREA*3), ProtocolBuf);
			Result = api_WriteTimeTable( Type, GET_STRUCT_ADDR(TTimeAreaTable, TimeArea[0][0]) ,(MAX_TIME_AREA*3), ProtocolBuf);
			if( Result == FALSE )
			{
				DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ���Ҫ�ϲ����
			}
			
		}
		else
		{
			if( pData[1] != 1)
			{
				Result = FALSE;
			}
			else
			{
				Result = api_WriteTimeTable( Type, GET_STRUCT_ADDR(TTimeAreaTable, TimeArea[ClassIndex-1][0]) ,3, ProtocolBuf);
				if( Result == FALSE )
				{
					DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ���Ҫ�ϲ����
				}
			}
		}
	}
	
	if( Result == TRUE)
	{
	    //api_WritePara( 0, (GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.TimeZoneNum)), 1, (BYTE*)&pData[1]);//����ʱ����
		api_SavePrgOperationRecord( ePRG_TIME_AREA_NO ); //�����̼�¼
		DAR = DAR_Success;
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ����ʱ��
//         
//����:      BYTE TYPE[in]             ����OI
//         
//         BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetTimeTable( BYTE Type,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//ʱ��
{
	BYTE DAR,b;
	WORD Result;
	TFourByteUnion OAD;
	
	Result = FALSE;
	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//��ȡOAD	
	
	if( (api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE) && (Type == 0x03) )//�ǳ���ģʽ�²���������ʱ�α�
	{
		return DAR_OtherErr;
	}
	
	if( ClassAttribute != 0x02)//ֻ����������2
	{
		return DAR_Undefined;
	}
	//��Ӧ������������ݽ���Э��ĵ��ܱ��ɼ��ն���ؼ���Ҫ��20170412��������Ҫ����"5)���ܱ�ʱ�α��̣�һ��ֻ������һ��ʱ�α�"
	if( ClassIndex == 0 )
	{
        return DAR_WrongType;
    }
    
	//pData = DealArrayClassIndexNotZeroData(pData);//Ԫ��������Ϊ��ʱ�����ݽ��д���
	if( ClassIndex > MAX_TIME_SEG_TABLE)//ֻ����������2
	{
		return DAR_OverRegion;
	}		

	//api_WritePreProgramData( 0,OAD.d );
	
	if( pData[0] == Array_698 )
	{
		if( pData[1] > MAX_TIME_SEG )
		{
			pData[1] = MAX_TIME_SEG;
		}

		Result = DealTimeZoneSegTableData( &pData[1] );
		
		if( Result == TRUE )//��ʱ�α���ӦС��15���ӽ����ж�
		{
			for( b=0; b<(3*pData[1]); b+=3 )
			{
				if( b <= (3*pData[1]-3) )//���һ��ʱ�κ͵�һ��ʱ�εļ��ҲҪ�ж� 2014-5-8
				{
					if(api_JudgeTimeSegInterval((BYTE*)&ProtocolBuf[b]) == FALSE )
					{
						Result = FALSE;
					}
				}

				if( ProtocolBuf[b] > 23 )//ʱ
				{
					Result = FALSE;
				}
				else if( ProtocolBuf[b+1] > 59 )//��
				{
					Result = FALSE;
				}
				else if( 	( ProtocolBuf[b+2] == 0x00 )
						||	( ProtocolBuf[b+2] > MAX_RATIO) )
				{
					Result = FALSE;
				}
			}
		}
		if( Result == TRUE )// 1��ʱ�α�����ɹ�
		{	
		    //api_WritePara( 0, (GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.TimeSegNum)), 1, (BYTE*)&pData[1]);//������������ʱ�α���
			// дʱ�α�֮ǰ�ȴ�֮ǰ�ı�
			api_WritePreProgramData( 0,OAD.d );

			api_AddTimeSegParaExtDataWithLastValid(3*pData[1], (MAX_TIME_SEG*3), ProtocolBuf);
			Result = api_WriteTimeTable( (Type+ClassIndex-1), GET_STRUCT_ADDR(TTimeSegTable, TimeSeg[0][0]),(MAX_TIME_SEG*3), ProtocolBuf);
			if( Result == FALSE )
			{
				DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ���Ҫ�ϲ����
			}
			else
			{
				api_SavePrgOperationRecord( ePRG_TIME_TABLE_NO ); // ���ڲ�ֵ�ʱ�α��̣�ÿ��ʱ�α���һ����¼
			}
			
		}
		else
		{
			DAR = DAR_WrongType;//��Ϊ�̶����أ�����������ģ���Ҫ�ϲ����
		}

	}	
	
	if( Result == TRUE)
	{
		DAR = DAR_Success;
		//api_SavePrgOperationRecord(ePRG_TIME_TABLE_NO);
	}

	return DAR;

}
//--------------------------------------------------
//��������:    ���ý�����
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetBillingPara( BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData )//��δ����ClassIndex��Ϊ������
{
	BYTE byNeedSwitchSave;
	BYTE DAR,i;	
	WORD Result;
	TBillingPara TmpBillingPara;
	TFourByteUnion OAD;
	
	//memcpy(OAD.b,pData-4,4);//��ȡOAD
	OAD.d = 0x00021641;

	Result = FALSE;
	DAR = DAR_WrongType;
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE *)&TmpBillingPara);

	if( ClassIndexes != 0 )
	{
		pData = DealArrayClassIndexNotZeroData(pData);//Ԫ��������Ϊ��ʱ�����ݽ��д���
		if( ClassIndexes > MAX_MON_CLOSING_NUM )
		{
			return DAR_OverRegion;
		}
		ClassIndexes = ClassIndexes-1;
	}
	byNeedSwitchSave = 0;
	if( ClassAttribute == 2 )
	{
		if( pData[0] == Array_698 )
		{
			if( pData[1] <= MAX_MON_CLOSING_NUM )
			{
				for( i=0; i <pData[1]; i++ )
				{
					if( pData[2+6*i] == Structure_698 )
					{
						if(( pData[4+6*i] == Unsigned_698 ) && (pData[6+6*i] == Unsigned_698))
						{
							//�жϽ����գ���ʱӦ����һ���ж�
							if(((pData[5+6*i] > 0) && (pData[5+6*i] <= 28) && (pData[7+6*i] <= 23) )//�շ�ΧΪ1��28,СʱΪ0��23
							||((pData[5+6*i] == 0x99) && (pData[7+6*i] == 0x99))//������ʱ��Ϊ0x99,�˽�����Ϊ��Ч��������
							||((pData[5+6*i] == 0xFF) && (pData[7+6*i] == 0xFF))//������ʱ��Ϊ0xff,�˽�����Ϊ��Ч��������
							||((pData[5+6*i] == 99) && (pData[7+6*i] == 99)))//������ʱ��Ϊ99,�˽�����Ϊ��Ч��������
							{
								if( (ClassIndexes == 0) && (i == 0) )
								{
									if( (TmpBillingPara.MonSavePara[0].Hour != pData[7+6*i])||(TmpBillingPara.MonSavePara[0].Day != pData[5+6*i]) )
									{
										byNeedSwitchSave = 1;//����ı��һ�����գ���Ҫת���¶��õ����������յ���
									}
								}	
								TmpBillingPara.MonSavePara[i+ClassIndexes].Day = pData[5+6*i];
								TmpBillingPara.MonSavePara[i+ClassIndexes].Hour = pData[7+6*i];
							}
							else
							{
								DAR = DAR_OverRegion;//Խ��
								break;//����ѭ��
							}

						}
						else
						{
                            break;
						}
					}
					else
					{
                        break;
					}
				}

				if( i == pData[1] )
				{
					DAR = DAR_Success;
				}
			}
		}
	}

	if( DAR == DAR_Success )
	{	
		api_WritePreProgramData( 0,OAD.d );
		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE *)&TmpBillingPara);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;			
			if( byNeedSwitchSave != 0 )//����ı��һ�����գ���Ҫת���¶��õ����������յ��� 
			{
				api_SetFreezeFlag( eFREEZE_CLOSING, 0 ); //���㶳��
			}
			api_SavePrgOperationRecord( ePRG_CLOSING_DAY_NO );
		}
	}

    return DAR;
}
//--------------------------------------------------
//��������:    ����ͨѶ��ַ����š��ͻ���Ų���
//         
//����:      WORD OI[in]               ����OI
//         
//         BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��  
//                           
//����ֵ:     BYTE DAR���   
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetCommAddrOrMeterNoOrCustomCode( WORD OI,BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData ,BYTE Ch)//��δ����ClassIndexes��Ϊ������
{
	WORD Result;
	BYTE Buf[10],DAR;

	DAR = DAR_WrongType;
	Result = FALSE;
	
	if( (ClassAttribute != 0x02) ||( ClassIndexes != 0 ))//ֻ����������2
	{
		return DAR_Undefined;
	}
	if( (pData[0] != Octet_string_698) || (pData[1] != 6) )
	{
		return DAR_Undefined;
	}
	memset(Buf,0x00,6);
	memcpy( Buf,&pData[2],6);
	if( OI == 0x4001)//ͨѶ��ַ
	{
		Result =api_ProcMeterTypePara( WRITE, eMeterCommAddr, Buf);
		lib_ExchangeData( LinkData[Ch].ServerAddr, Buf, 6 );
	}
	else if( OI == 0x4002 )//���
	{
	    if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE )//����ģʽ�� �������ñ��
	    {
           Result =api_ProcMeterTypePara( WRITE, eMeterMeterNo, Buf);
	    }	
	}
	else
	{
		if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE )//����ģʽ�� �������ñ��
	    {
		    Result = api_ProcMeterTypePara( WRITE, eMeterCustomCode, Buf );
		}
	}
	if( Result == FALSE)
	{
		DAR = DAR_HardWare;//��Ϊ�̶����󷵻أ�����������ģ����ϲ���ú����й�
	}
	else
	{			
		DAR = DAR_Success;//DAR �ɹ�
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ����ʱ��
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR���    
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetRTC( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//��δ����ClassIndex��Ϊ������
{
	BYTE DAR;
	WORD Result;
	TRealTimer TmpRealTimer;//ʱ��ṹ��
	TFourByteUnion OAD;
	TTimeBroadCastPara tmpTimeBroadCastPara;
	
	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//��ȡOAD

	if( ClassAttribute == 0x02 )//����ʱ��
	{
		if( ClassIndex != 0 )//ֻ����������2
		{
			return DAR_Undefined;
		}
			
		if( pData[0] != DateTime_S_698 )
		{
			return DAR_Undefined;
		}
		
		api_WritePreProgramData( 0,OAD.d );
		lib_InverseData(&pData[1], 2);
		memcpy((BYTE*)&(TmpRealTimer.wYear),&pData[1],sizeof( TRealTimer ));
		Result = api_WriteMeterTime(&TmpRealTimer);
		
		if( Result == FALSE )
		{
			DAR= DAR_HardWare;//��Ϊ�̶����󷵻أ�����������ģ����ϲ���ú����й�
		}
		else
		{	
			api_SetAllTaskFlag( eFLAG_TIME_CHANGE );
			api_SavePrgOperationRecord( ePRG_ADJUST_TIME_NO );
			#if( SEL_DEMAND_2022 == NO )//�����������㷽ʽ
			#if( MAX_PHASE != 1)		
			api_InitDemand( );//���óɹ������½��������ļ���
			#endif
			#endif
			DAR= DAR_Success;//DAR �ɹ�
		}
	}
	else if( ClassAttribute == 0x05 )//����5 �㲥Уʱ����
	{
		if( ClassIndex > 2 )
		{
			return DAR_RefuseOp;
		}
		
		//��ȡ�㲥Уʱ����
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara );

		if( ClassIndex == 0 )
		{
			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 2 )
				{
					if( pData[2] == Long_unsigned_698 )//�㲥Уʱ��С��Ӧ��ֵ
					{
						lib_ExchangeData( (BYTE*)&tmpTimeBroadCastPara.TimeMinLimit, pData+3, 2 );	
						if( pData[5] == Long_unsigned_698 )//�㲥Уʱ�����Ӧ��ֵ
						{
							lib_ExchangeData( (BYTE*)&tmpTimeBroadCastPara.TimeMaxLimit, pData+6, 2 );
							if( tmpTimeBroadCastPara.TimeMaxLimit > tmpTimeBroadCastPara.TimeMinLimit )//Э��Ҫ�����ֵ������Сֵ
							{
								DAR= DAR_Success;//DAR �ɹ�
								api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara );
							}
							else
							{
								DAR= DAR_OverRegion;//DAR Խ��
							}
							
						}
					}
				}
			}
		}
		else
		{
			if( pData[0] == Long_unsigned_698 )
			{
				lib_ExchangeData( (BYTE*)&tmpTimeBroadCastPara.TimeMinLimit+(ClassIndex-1)*2, pData+1, 2 );
				if( tmpTimeBroadCastPara.TimeMaxLimit > tmpTimeBroadCastPara.TimeMinLimit )//Э��Ҫ�����ֵ������Сֵ
				{
					DAR= DAR_Success;//DAR �ɹ�
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara );	
				}
				else
				{
					DAR= DAR_OverRegion;//DAR Խ��
				}

			}
		}
		
	}
	else
	{
		DAR = DAR_RefuseOp;
	}
	


    return DAR;
}

//--------------------------------------------------
//��������: 	�л�ʱ���ʽ�ж�
//       
//����:     	Mode[in] ����ģʽ0x00������ģʽ         0x55��esam���ݸ���
//			BYTE *pData[in] ʱ������
//			TmpRealTimer[out] ���ʱ��
//         
//����ֵ:     	BYTE DAR���      
//         
//��ע����:    	��
//--------------------------------------------------
BYTE JudgeSwitchTimeformat( BYTE Mode, BYTE *pData, TRealTimer *RealTimer )
{
	BYTE Len;
	WORD Year;
	TRealTimer tmpRealTimer;

	memset( &tmpRealTimer, 0x00, sizeof(TRealTimer) );
	Year = 0;
	
	if( Mode == 0x00 )
	{
		Len = 6;
	}
	else
	{
		Len = 5;
	}
	
	if( lib_IsAllAssignNum(pData, 0x00, Len) == TRUE )
    {
    	memset( (BYTE*)&tmpRealTimer.wYear, 0x00, 6);
    }
    else if( lib_IsAllAssignNum(pData, 0x99, Len) == TRUE )
    {
    	memset( (BYTE*)&tmpRealTimer.wYear, 0x99, 6);
    }
    else if( lib_IsAllAssignNum(pData, 0xff, Len) == TRUE )
    {
    	memset( (BYTE*)&tmpRealTimer.wYear, 0xff, 6);
    }
    else
    {
		if( Mode == 0x55 )//esam���ݸ���
		{
			Year = 2000;		
			lib_MultipleBBCDToBin( pData, pData, 5 );
			Year = Year+pData[0];//��ȡ��ʱ��
			tmpRealTimer.wYear = Year;
			memcpy( &tmpRealTimer.Mon, pData+1, 4 );
		}
		else
		{
			lib_InverseData(pData, 2);
			memcpy( (BYTE*)&tmpRealTimer.wYear, pData, 6);//��ʱ��ת��Ϊ�ṹ�庯��
		}

		if( api_CheckMonDay( tmpRealTimer.wYear, tmpRealTimer.Mon, tmpRealTimer.Day ) == FALSE )
		{
			return DAR_OverRegion;
		}
		
		if( tmpRealTimer.Hour > 23 )
		{
			return DAR_OverRegion;
		}
		if( tmpRealTimer.Min > 59 )	
		{
			return DAR_OverRegion;
		}
    }
    
    memcpy( RealTimer, &tmpRealTimer, sizeof(TRealTimer) );
    
	return DAR_Success;
}


//--------------------------------------------------
//��������:    ����ʱ��ʱ�α���л�ʱ��
//       
//����:      BYTE Mode[in]             ����ģʽ0x00������ģʽ         0x55��esam���ݸ���
//         
//         WORD OI[in]               ����OI
//
//         BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndexes[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//         
//����ֵ:     BYTE DAR���      
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetSwitchTime( BYTE Mode, WORD OI,BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData )//��δ����ClassIndex��Ϊ������
{
	BYTE DAR,Type;		
	WORD Result;
	TRealTimer TmpRealTimer;//ʱ��ṹ��
	DWORD RelativeTime;
		
	Type = OI - 0x4008;//��ȡƫ��
	DAR = DAR_WrongType;
		
	if( (ClassAttribute != 0x02) || ( ClassIndexes != 0 ))//ֻ����������2 �Ҳ�֧�����Բ�Ϊ0������
	{
		return DAR_Undefined;
	}

	if( Mode == 0x00 )
	{

    	if( pData[0] != DateTime_S_698 )//������DateTime_S
    	{
    		return DAR_Undefined;
    	}
    	
		DAR = JudgeSwitchTimeformat( Mode, pData+1, &TmpRealTimer );
		if( DAR != DAR_Success )
		{
			return DAR;
		}
		
	}
	else
	{
	    if( pData[0] != 5 )
	    {
            return DAR_RefuseOp;
	    }
	    else
	    {
			DAR = JudgeSwitchTimeformat( Mode, pData+1, &TmpRealTimer );
			if( DAR != DAR_Success )
			{
				return DAR;
			}
	    }

	    //��֤esam
		Result = api_AuthDataSID( ProtocolBuf );
		if( Result == FALSE )
		{
			return DAR_EsamFial;
		}
	}
	
	Result = api_ReadAndWriteSwitchTime( WRITE, Type, (BYTE *)&TmpRealTimer );

	if( Result == FALSE )
	{
		DAR = DAR_OverRegion;//��Ϊ�̶����󷵻أ�����������ģ����ϲ���ú����й�
	}
	else
	{
		DAR = DAR_Success;
	}

    return DAR;
}
#if( MAX_PHASE != 1 )	
//--------------------------------------------------
//��������:    ������������
//         
//����:      WORD OI[in]               ����OI
//         
//         BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR���      
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetDemandPara( WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE Buf[10],Type,DAR;
	WORD Result;
	
	DAR = DAR_WrongType;
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//ֻ����������2 �Ҳ�֧�����Բ�Ϊ0������
	{
		return DAR_Undefined;
	}
	if( pData[0] == Unsigned_698 )
	{
		Buf[0] = pData[1];
	}
	else
	{
		return DAR_Undefined;
	}

	Type  = (OI - 0x4100);
	Result = api_WritePara( 1,  GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.DemandPeriod)+Type, 1, Buf);
	
	if( Result == FALSE )
	{
		DAR = DAR_HardWare;//��Ϊ�̶����󷵻أ�����������ģ����ϲ���ú����й�
	}
	else
	{
		DAR = DAR_Success;
		#if( MAX_PHASE != 1)//��������-��������-�����������������½��м�������		
		api_InitDemand( );//���óɹ������½��������ļ���
		#endif		
	}

    return DAR;
}
#endif//( MAX_PHASE != 1 )	
//--------------------------------------------------
//��������:    ����������������
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                
//����ֵ:     BYTE DAR���   
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetWeekStatus( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE Buf[10],DAR;
	WORD Result;
	TFourByteUnion OAD;
	
	memcpy(OAD.b,pData-4,4);//��ȡOAD
		
	DAR = DAR_WrongType;
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//ֻ����������2 �Ҳ�֧�����Բ�Ϊ0������
	{
		return DAR_Undefined;
	}
	if( pData[0] == Bit_string_698 )
	{
		//Buf[0] = pData[2] ;
		lib_ExchangeBit( (BYTE*)&Buf[0], (BYTE*)&pData[2], 1);
		DAR = DAR_Success;
	}
	else
	{
		return DAR_Undefined;
	}
	
	if( DAR == DAR_Success)
	{
		api_WritePreProgramData( 0,OAD.d );	
		Result = api_WritePara(0, GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.WeekStatus), 1, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ����ϲ����
		}
		else
		{			
			DAR = DAR_Success;//DAR �ɹ�
			api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
			api_SavePrgOperationRecord( ePRG_WEEKEND_NO );
		}	
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ����������ʱ��
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR���     
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetWeekSeg( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE Buf[10],DAR;
	WORD Result;
	TFourByteUnion OAD;

	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//��ȡOAD
	
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//ֻ����������2 �Ҳ�֧�����Բ�Ϊ0������
	{
		return DAR_Undefined;
	}

	if( pData[0] == Unsigned_698 )
	{
		Buf[0] = pData[1] ;
		DAR = DAR_Success;
	}
	else
	{
		return DAR_Undefined;
	}

	if( DAR == DAR_Success)
	{
		api_WritePreProgramData( 0,OAD.d );
		Result = api_WritePara(0, GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.WeekSeg), 1, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//DAR Ӳ��ʧЧ
		}
		else
		{			
			DAR = DAR_Success;//DAR �ɹ�
			api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
			api_SavePrgOperationRecord( ePRG_WEEKEND_NO );
		}
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ����PTCT
//         
//����:      WORD OI[in]               ����OI
//
//         BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//         
//         
//����ֵ:     BYTE DAR���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetPTCT(     WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR;
	WORD Result;
	DWORD PTCT;
	
	DAR = DAR_WrongType;
    PTCT = 0;
    
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//ֻ����������2 �Ҳ�֧�����Բ�Ϊ0������
	{
		return DAR_Undefined;
	}
	
	if( pData[0] == 3 )
	{
	    lib_ExchangeData( (BYTE*)&PTCT, pData+1, 3 );
	    PTCT = lib_DWBCDToBin( PTCT );
		DAR = DAR_Success;
	}
	else
	{
		return DAR_Undefined;
	}
	
    //��֤esam
	Result = api_AuthDataSID( ProtocolBuf );
	if( Result == FALSE )
	{
		return DAR_EsamFial;
	}
	
	if( DAR == DAR_Success)
	{
		if( OI == 0x401c)
		{
			Result = api_WritePrePayPara(eCTCoe, (BYTE*)&PTCT);
		}
		else
		{
			Result = api_WritePrePayPara(ePTCoe, (BYTE*)&PTCT);
		}
		
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ������ϲ�������
		}
		else
		{			
			DAR = DAR_Success;//DAR �ɹ�
		}
	}
	
	return DAR;
}
#if( PREPAY_MODE == PREPAY_LOCAL)

//--------------------------------------------------
//��������:    ���ñ��������ֵ
//         
//����:      BYTE Ch[in]               ͨ��ѡ��
//         
//         WORD OI[in]               ����OI
//
//         BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//         
//����ֵ:     BYTE DAR���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetLimitMoney( BYTE Mode, WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Num,Num1,i,Type;
	WORD Result;
	DWORD LimitMoney1[4];
	DAR = DAR_WrongType;
	
	if( ClassAttribute != 0x02 )//ֻ����������2
	{
		return DAR_RefuseOp;
	}
	
    if( OI == 0x401e )
    {
        Num = 2;
        Type = eAlarm_Limit1;//�������1-WR
    }
    else if( OI == 0x401f )
    {
        Num = 3;
        Type = eTickLimit;//͸֧�������-WR
    }
    else
    {
		return DAR_Undefined;
    }

    if( Mode == 0 )//����ģʽ
    {
    	if( ClassIndex == 0 )
    	{
            if( pData[0] == Structure_698 )
            {
                if( pData[1] == Num )
                {
                    for( i = 0; i < Num; i++ )
                    {
                        if( pData[2+5*i] == Double_long_unsigned_698 )
                        {
                            lib_ExchangeData( (BYTE*)&LimitMoney1[i], &pData[3+5*i], 4);
                        }
                    }

                    if( i == Num )
                    {
                        DAR = DAR_Success;
                    }
                }
            }
    	}
    	else
    	{
            if( pData[0] == Double_long_unsigned_698 )
            {
                lib_ExchangeData( (BYTE*)&LimitMoney1[0], &pData[1], 4);
                DAR = DAR_Success;
            }
    	}

    }
    else//esam���ݸ���
    {
        if( OI != 0x401e )//esam���ݸ���ֻ֧�ֱ��������ֵ����
        {
            return DAR_Undefined;
        }

        Num1 = ((ClassIndex==0) ? Num : 1);

        if( pData[0] != Num*4 )
        {
            return DAR_OverRegion;
        }
        
        for( i = 0; i < Num1; i++ )
        {
            //lib_MultipleBBCDToBin( (BYTE*)&pData[1+4*i], (BYTE*)&pData[1+4*i], 4 );
            lib_ExchangeData( (BYTE*)&LimitMoney1[i], (BYTE*)&pData[1+4*i], 4 );
            LimitMoney1[i] = lib_DWBCDToBin(LimitMoney1[i]);
        }

        if( i == Num1 )
        {
             DAR = DAR_Success;
        }

        //��֤esam
		Result = api_AuthDataSID( ProtocolBuf );
		if( Result == FALSE )
		{
			return DAR_EsamFial;
		}
    }
	
	if( DAR == DAR_Success)
	{
	    Type = ((ClassIndex==0) ? Type : (Type+ClassIndex-1));
	    Num = ((ClassIndex==0) ? Num : 1);

	    for( i = 0; i < Num; i++ )
	    {
            Result = api_WritePrePayPara( (ePrePayParaType)(Type+i), (BYTE*)&LimitMoney1[i] );
            if( Result == FALSE )
            {
                DAR = DAR_RefuseOp;
                break;
            }
	    }
	}
	
	return DAR;
}

//--------------------------------------------------
//��������:    ���ñ����׷��ʵ��
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//         
//����ֵ:     BYTE DAR���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetBackupRate( BYTE ClassAttribute,BYTE ClassIndex, BYTE *pData )
{
    BYTE RateNum,i,DAR;
    BOOL Result;
    TRatePrice RatePrice;
    TFourByteUnion Rate;

    DAR = DAR_Undefined;

    if( (ClassAttribute != 2) || (ClassIndex != 0))//ֻ֧������2��ȫ������
    {
        return DAR_RefuseOp;
    }

    RateNum = (pData[0]/4);

    for( i=0; i<MAX_RATIO; i++ )
    {
        if( i < RateNum )
        {
            lib_ExchangeData( Rate.b, (BYTE*)&pData[1+4*i], 4 );//��ȡ���
            RatePrice.Price[i] = lib_DWBCDToBin(Rate.d);//BCDת��ΪHex
        }
        else//�Է��ʵ�۽��в�λ �����ķ���
        {
            lib_ExchangeData( Rate.b, (BYTE*)&pData[1+4*(RateNum-1)], 4 );//��ȡ���
            RatePrice.Price[i] = lib_DWBCDToBin(Rate.d);//BCDת��ΪHex
        }
    }
    
    //��֤esam
	Result = api_AuthDataSID( ProtocolBuf );
	if( Result == FALSE )
	{
		return DAR_EsamFial;
	}
	
    api_WritePreProgramData( 0,0x00221940 );//40192200�������ǰ�����׷��ʵ�� 
   

    Result = api_WritePrePayPara( eBackupRateTable, (BYTE*)RatePrice.Price );
    if( Result == TRUE )
    {
		api_SavePrgOperationRecord( ePRG_TARIFF_TABLE_NO ); //���ܱ���ʲ��������¼�
        DAR = DAR_Success;
    }

    return DAR;
}

BYTE SetBackupLadder( BYTE ClassAttribute,BYTE ClassIndex, BYTE *pData )
{
    BYTE i,b,DAR;
    BOOL Result;
    TLadderPrice LadderPrice;
    TFourByteUnion Ladeer;

    DAR = DAR_Undefined;

    if( (ClassAttribute != 2) || (ClassIndex != 0))//ֻ֧������2��ȫ������
    {
        return DAR_RefuseOp;
    }

    for( i=0; i<MAX_LADDER; i++ )//���ݵ���
    {
        if( i < MAX_ESAM_LADDER )
        {
            lib_ExchangeData( Ladeer.b, (BYTE*)&pData[1+4*i], 4 );//��ȡ���ݵ���
            LadderPrice.Ladder_Dl[i] = lib_DWBCDToBin(Ladeer.d);//BCDת��ΪHex
        }
        else//�Է��ʵ�۽��в�λ �����ķ���
        {
            lib_ExchangeData( Ladeer.b, (BYTE*)&pData[1+4*5], 4 );//��ȡ���ݵ���
            LadderPrice.Ladder_Dl[i] = lib_DWBCDToBin(Ladeer.d);//BCDת��ΪHex
        }
    }

    for( i=0; i<MAX_LADDER+1; i++ )//���ݵ��
    {
        if( i < MAX_ESAM_LADDER+1 )
        {
            lib_ExchangeData( Ladeer.b, (BYTE*)&pData[1+4*MAX_ESAM_LADDER+4*i], 4 );//��ȡ���ݵ���
            LadderPrice.Price[i] = lib_DWBCDToBin(Ladeer.d);//BCDת��ΪHex
        }
        else//�Է��ʵ�۽��в�λ �����ķ���
        {
            lib_ExchangeData( Ladeer.b, (BYTE*)&pData[1+4*5], 4 );//��ȡ���ݵ���
            LadderPrice.Price[i] = lib_DWBCDToBin(Ladeer.d);//BCDת��ΪHex
        }
    }

    memset( (BYTE*)LadderPrice.YearBill, 0x99, sizeof(LadderPrice.YearBill) );//���ý�����Ϊ��Ч

    for( i=0; i<MAX_YEAR_BILL; i++ )
    {
        if( i < MAX_ESAM_YEAR_BILL )
        {
            for( b=0; b<3; b++ )
            {
                LadderPrice.YearBill[i][b] = pData[1+4*MAX_ESAM_LADDER+4*(MAX_ESAM_LADDER+1)+3*i+b];
                if( lib_IsBCD(LadderPrice.YearBill[i][b]) == TRUE )
                {
                    LadderPrice.YearBill[i][b] = lib_BBCDToBin( LadderPrice.YearBill[i][b] );
                }
            }
        }
        else
        {
            memset( (BYTE*)&LadderPrice.YearBill[i][0], 99, 3);
        }

        if( (LadderPrice.YearBill[i][0] > 12) && (LadderPrice.YearBill[i][0] != 99) && (LadderPrice.YearBill[i][0] != 0xFF) )//Mon
        {
            return DAR_OverRegion;
        }

        if( (LadderPrice.YearBill[i][1] > 28) && (LadderPrice.YearBill[i][1] != 99) && (LadderPrice.YearBill[i][1] != 0xFF) )//Day
        {
            return DAR_OverRegion;
        }

        if( (LadderPrice.YearBill[i][2] > 23) && (LadderPrice.YearBill[i][2] != 99) && (LadderPrice.YearBill[i][2] != 0xFF) )//Hour
        {
            return DAR_OverRegion;
        }
    }
    
    //��֤esam
	Result = api_AuthDataSID( ProtocolBuf );
	if( Result == FALSE )
	{
		return DAR_EsamFial;
	}
	
    api_WritePreProgramData( 0,0x00221B40 );//401B2200�������ǰ�����׽��ݵ�� 

    Result = api_WritePrePayPara( eBackupLadderTable, (BYTE*)&LadderPrice.Ladder_Dl );
    if( Result == TRUE )
    {
		api_SavePrgOperationRecord( ePRG_LADDER_TABLE_NO ); //���ܱ���ݱ����¼�
        DAR = DAR_Success;
    }

    return DAR;
}
#endif
//--------------------------------------------------
//��������:    �����ʲ��������
//         
//
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR���      
//            
//��ע����:    ��
//--------------------------------------------------
BYTE SetPropertyCode( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//�ʲ��������
{
	BYTE DAR;
	WORD Result;
	DAR = DAR_WrongType;
	
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//ֻ����������2 �Ҳ�֧�����Բ�Ϊ0������
	{
		return DAR_Undefined;
	}
	
	if( (pData[0] == Visible_string_698) && (pData[1] == 32) )
	{
		DAR = DAR_Success;
	}
	else
	{
		return DAR_Undefined;
	}
	
	if( DAR == DAR_Success)
	{
		Result = api_ProcMeterTypePara( WRITE, eMeterPropertyCode, pData+2 );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ������ϲ�������
		}
		else
		{			
			DAR = DAR_Success;//DAR �ɹ�
		}	
	}
	
	return DAR;
}
//--------------------------------------------------
//��������:    �������������
//         
//����:      WORD OI[in]               ����OI
//         
//         BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetCOMStatus( WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//���ģʽ��
{
	BYTE DAR,Buf[10],Type;
	WORD Result;
	TFourByteUnion OAD;

	DAR = DAR_WrongType;
	memcpy(OAD.b,pData-4,4);//��ȡOAD	
	
	if( (ClassAttribute != 0x02) || ( ClassIndex != 0 ))//ֻ����������2 �Ҳ�֧�����Բ�Ϊ0������
	{
		return DAR_Undefined;
	}
	
	if( pData[0] == Bit_string_698 )
	{
		if( pData[1] == 8 )
		{
			lib_ExchangeBit( (BYTE*)&Buf[0], (BYTE*)&pData[2], 1);
			DAR = DAR_Success;
		}
		else
		{
			return DAR_Undefined;
		}
	}
	else
	{	
		return DAR_Undefined;;
	}
	
	Type = (OI - 0x4112);
	
	if( DAR == DAR_Success )
	{	
		api_WritePreProgramData( 0,OAD.d );
		Result = api_WritePara(1, GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.byActiveCalMode)+Type, 1, Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ������ϲ�������
		}
		else
		{
			DAR = DAR_Success;

			#if( PREPAY_MODE == PREPAY_LOCAL )
			//�����й���Ϸ�ʽ�����֣���Ҫˢ��һ�½���ֵ�ͽ��ݵ��
			api_SetUpdatePriceFlag( ePriceChangeEnergy );
			#endif
			
			if( Type == 0 )//�й���ϱ��
			{
				api_SavePrgOperationRecord( ePRG_P_COMBO_NO );
			}
			else
			{
				#if( SEL_PRG_INFO_Q_COMBO == YES )
				api_SavePrgOperationRecord( ePRG_Q_COMBO_NO );
				#endif
			}
		}
	}
	
	return DAR;
}
//--------------------------------------------------
//��������:    ���õ�ѹ�ϸ��ʲ���
//         
//����:      WORD OI[in]               ����OI
//         
//         BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��         
//         
//����ֵ:     BYTE DAR���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetVoltageRatePara( WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//��ѹ�ϸ��ʲ���
{
	BYTE DAR,Buf[10],i;
	WORD Result;
	
	DAR = DAR_WrongType;
	
	if( (ClassAttribute != 0x02) || ( ClassIndex > 4 ))//ֻ����������2 �Ҳ�֧�����Դ���4������
	{
		return DAR_Undefined;
	}
	
	if( ClassIndex == 0 )
	{
		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 4 )
			{
				for( i=0; i<4; i++ )
				{
					if( pData[2+3*i] == Long_unsigned_698 )
					{
						lib_ExchangeData(&Buf[2*i], &pData[2+3*i+1] , 2);
					}
					else
					{
						break;
					}
				}

				if( i == 4 )
				{
					DAR = DAR_Success;
				}
			}
		}
	}
	else
	{
		pData = DealStructureClassIndexNotZeroData(pData);

		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 1 )
			{
				if( pData[2] == Long_unsigned_698 )
				{
					lib_ExchangeData( Buf, &pData[3] , 2);
					DAR = DAR_Success;
				}
			}
		}
	}

	if( DAR == DAR_Success )
	{
		Result = api_WriteEventPara( OI, ClassIndex, Buf );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ������ϲ�������
		}
		else
		{
			DAR = DAR_Success;
		}
	}
	
	return DAR;
}
#if( MAX_PHASE != 1 )	
//--------------------------------------------------
//��������:    �����ڼ�������������
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR���   
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetIntervalDemandFreezingPeriod( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//���ģʽ��
{
	BYTE DAR,Buf[10];
	WORD Result;
	
	DAR = DAR_WrongType;

	if( (ClassAttribute != 2) || ( ClassIndex != 0) )
	{
		return DAR_WrongType;
	}

	if( pData[0] == TI_698 )
	{
		Buf[0] = pData[1];
		lib_ExchangeData( &Buf[1], &pData[2], 2 );
		DAR = DAR_Success;
	}

	if( DAR == DAR_Success )
	{
		Result = api_WritePara(1, GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.IntervalDemandFreezePeriod), sizeof( TIntervalDemandFreezePeriod ), Buf);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ������ϲ�������
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
#endif//#if( MAX_PHASE != 1 )	
//--------------------------------------------------
//��������:    ���õ�����
//         
//����:      BYTE ClassIndex[in]     ��������
//
//         BYTE *pData[in]         ����ָ��
//                  
//����ֵ:     BYTE DAR���       
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetMeterVersionInformation( BYTE ClassIndex,BYTE *pData )//���õ��汾��Ϣ
{
	BYTE DAR,i,Buf[40];
	WORD Result;
	WORD Len;

	Result = FALSE;
	DAR = DAR_WrongType;

	if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//ֻ֧�ֳ���ģʽ�µ����ð汾��Ϣ
	{
		return DAR_RefuseOp;
	}

	if( ClassIndex == 0 )
	{
		if( pData[0] == Structure_698 )
		{
			if( pData[1] == 6 )
			{
				if( pData[2] == Visible_string_698 )
				{
					if( pData[3] == 4 )
					{
						memcpy( Buf, pData+4, 4 );//���̴���
						if( pData[8] == Visible_string_698 )
						{
							if( pData[9] == 4 )
							{
								memcpy( Buf+4, pData+10, 4 );//����汾��
								if( pData[14] == Visible_string_698 )
								{
									if( pData[15] == 6 )
									{
										memcpy( Buf+8, pData+16, 6 );//����汾����
										if( pData[22] == Visible_string_698 )
										{
											if( pData[23] == 4 )
											{
												memcpy( Buf+14, pData+24, 4 );//Ӳ���� ����
												if( pData[28] == Visible_string_698 )
												{
													if( pData[29] == 6 )
													{
														memcpy( Buf+18, pData+30, 6 );//Ӳ���汾����
														DAR = DAR_Success;
													}
												}
											}											
										}
									}
								}
							}
						}						
					}
				}
			}
		}

		if( DAR == DAR_Success )
		{
			Len = 0;
			for( i=0; i < 5; i++ )
			{
				Result = api_ProcMeterTypePara(WRITE, eMeterFactoryCode+i, Buf+Len );
				if( Result == FALSE )
				{
					DAR = DAR_HardWare;//Ӳ������
					break;
				}
				
				if( (i == 2) || (i == 4) )
				{
					Len += 6;
				}
				else
				{
					Len += 4;
				}
			}
		}
	}
	else
	{
		if( ClassIndex > 6 )
		{
			return DAR_OverRegion;//Խ��
		}

		pData = DealStructureClassIndexNotZeroData(pData);

		if( pData[2] == Visible_string_698 )
		{
			if( (ClassIndex == 2) || ( ClassIndex == 4 ) )
			{
				Len = 6;
			}
			else
			{
				Len = 4;
			}

			if( pData[3] == Len )
			{
				memcpy( Buf, pData+4, Len );
				DAR= DAR_Success;
			}

			if( DAR == DAR_Success )
			{
				if( ClassIndex < 6 )
				{
					Result = api_ProcMeterTypePara(WRITE, eMeterFactoryCode+ClassIndex-1, Buf+Len );
					if( Result == FALSE )
					{
						DAR = DAR_HardWare;//Ӳ������
					}
				}
			}
		}
	}
	
	return DAR;
}

//--------------------------------------------------
//��������:    ���õ�����
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                 
//����ֵ:     BYTE DAR���      
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetMeter( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//�����豸
{
	BYTE DAR,i,Buf[40];
	WORD Result;

	Result = FALSE;
	DAR = DAR_WrongType;
	
	switch( ClassAttribute )
	{
		case 3://���ð汾��Ϣ
			if(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//��֧�ֳ���ģʽ�µ�����
			{
				return DAR_RefuseOp;
			}
			DAR = SetMeterVersionInformation( ClassIndex, pData );
			break;
		case 4://��������
			if(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//��֧�ֳ���ģʽ�µ�����
			{
				return DAR_RefuseOp;
			}
			if( ClassIndex == 0 )
			{
				if( pData[0] == DateTime_S_698 )
				{
					if( api_CheckMonDay( (pData[1]*0x100+pData[2]), pData[3], pData[4] ) == FALSE )
					{
						return DAR_OverRegion;
					}
					if( (pData[5] > 23) && (pData[6] > 59) )
					{
						return DAR_OverRegion;
					}
					lib_ExchangeData( Buf, &pData[1], 2 );//�� ���е���
					memcpy( &Buf[2],&pData[3],5); //�¡��ա�ʱ���֡���
					DAR = DAR_Success;
					Result = api_ProcMeterTypePara(WRITE, eMeterProduceDate, Buf );
				}
			}
			break;
		case 7://��������ϱ�
			if( ClassIndex == 0 )
			{
				if( (pData[0] == Boolean_698) && (pData[1] <= 1) )
				{
					DAR = DAR_Success;
					Result = SetReportFlag(pData[1],eFollowReport);
				}
			}
			break;
			
		case 8://���������ϱ�
			if( ClassIndex == 0 )
			{
				if( (pData[0] == Boolean_698) && (pData[1] <= 1) )
				{
					DAR = DAR_Success;
					Result = SetReportFlag(pData[1],eActiveReport);
				}
			}
			break;


		case 10://�ϱ�ͨ��
			if( ClassIndex == 0 )
			{
				if( pData[0] == Array_698 )
				{
					if( pData[1] < MAX_COM_CHANNEL_NUM )
					{
						for( i=0; i < pData[1]; i++ )
						{
							if( pData[2+5*i] == OAD_698 )
							{
								memcpy( Buf+4*i,(BYTE*)&pData[2+5*i+1],4);
							}
							else
							{
								break;
							}
						}

						if( i == pData[1] )
						{
							Result = SetReportChannel( 0, pData[1], Buf);
							DAR = DAR_Success;
						}
					}
				}
			}
			else if( ClassIndex <= MAX_COM_CHANNEL_NUM )
			{
				if( pData[0] == OAD_698 )
				{
					memcpy( Buf,(BYTE*)&pData[1],4);
					Result = SetReportChannel( ClassIndex, 1 , Buf);
					DAR = DAR_Success;
				}
			}
			break;

		default:
			break;
	}

	if( DAR == DAR_Success )
	{
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ������ϲ�������
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ����һ������
//         
//����:      BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]     ��������
//         
//         BYTE *pData[in]           ����ָ��
//                  
//����ֵ:     BYTE DAR���      
//         
//��ע����:    ��
//--------------------------------------------------

BYTE SetPassWord( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )//��������
{
	BYTE DAR,Offest;
	WORD Result;
	TTwoByteUnion len;
	TMuchPassword TmpMuchPassword;
	
	DAR = DAR_WrongType;

	if( (ClassAttribute != 2) || ( ClassIndex != 0) )
	{
		return DAR_WrongType;
	}

	if( pData[0] == Visible_string_698 )
	{
	    Offest = Deal_698DataLenth( (BYTE*)&pData[1], len.b, ePROTOCOL_TYPE );
		if( len.w != 8 )//һ���������ĳ��� С��7
        {
			return DAR_OverRegion;
        }  
        
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)TmpMuchPassword.Password645) != TRUE )
		{
			return DAR_HardWare;
		}
		
		memcpy( TmpMuchPassword.Password698, &pData[Offest+1], MAX_698PASSWORD_LENGTH);
		DAR = DAR_Success;
	}

	if( DAR == DAR_Success )
	{
		//д�����룬�Լ�����У��
		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.MuchPassword ), sizeof(TMuchPassword), (BYTE*)&TmpMuchPassword);
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;//��Ϊ�̶����أ�����������ģ������ϲ�������
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}

//--------------------------------------------------
//��������:      �����ź�ǿ��
//         
//����:        BYTE ClassAttribute[in]   Ԫ������
//
//           BYTE ClassIndexes[in]     ��������
//         
//           BYTE *pData[in]           ����ָ��       
//         
//����ֵ:     BYTE DAR���      
//         
//��ע����:    ��
//--------------------------------------------------

BYTE SetGPRSdBm( BYTE ClassAttribute,BYTE ClassIndexes,BYTE *pData )//��������
{
	BYTE DAR,dBm;
	short Value;
	
	DAR = DAR_WrongType;

	if( (ClassAttribute != 9) || ( ClassIndexes != 0) )
	{
		return DAR_WrongType;
	}

	if( pData[0] == Long_698 )
	{
        lib_ExchangeData( (BYTE*)&Value, (BYTE*)&pData[1], 2 );

        if( Value >0 )
        {
            DAR = DAR_OverRegion;
        }
        else
        {
            DAR = DAR_Success;
            
            if( (labs(Value) > 107) || (Value == 0) )
            {
                dBm = 0;
            }
            else if( labs(Value) > 93 )
            {
                dBm = 1;
            }
            else if( labs(Value) > 71 )
            {
                dBm = 2;
            }
            else if( labs(Value) > 69 )
            {
                dBm = 3;
            }
            else
            {
                dBm = 4;
            }
            
            api_ProcLcdWirelessPara( WRITE, (BYTE*)&dBm);
        }
	}

	return DAR;
}

//--------------------------------------------------
//��������:    ���õ�������Ϣ����( �й��������޹�������)  0x4104~410b
//         
//����:      WORD OI[in]               ����OI
//
//         BYTE ClassAttribute[in]   Ԫ������
//         
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��
//         
//����ֵ:     BYTE DAR���    
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetMeterBasicPara( WORD OI,BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{	
	BYTE DAR,Buf[35],Type;
	WORD Result;
	WORD len;

	Result = FALSE;
	DAR = DAR_WrongType;
	
	if(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//��֧�ֳ���ģʽ�µ�����
	{
		return DAR_RefuseOp;
	}
	if(ClassAttribute == 0x02)
	{
		if( (OI == 0x4109) ||(OI == 0x410A) )//���ܱ����޹�����
		{
			Type = (OI - 0x4109);
			if( pData[0] == Double_long_unsigned_698 )
			{
				DAR = DAR_Success;
				lib_ExchangeData(Buf, &pData[1], 4);
				Result = api_ProcMeterTypePara(WRITE, eMeterActiveConstant+Type, Buf );
			}
		}
		else
		{
			if( OI == 0x410B )//���ܱ��ͺ���32���ֽ�
			{
				len = 32;
				Type = eMeterMeterModel;
			}
			else if( (OI == 0x4107) ||(OI == 0x4108) )//�й��޹�׼ȷ�ȼ���4���ֽ�
			{
				len = 4;
				Type = OI- 0x4107 + eMeterPPrecision;
			}
			else if( OI == 0x4111 )//���������
			{
				len = 16;
				Type = eMeterSoftRecord;
			}
			else//���� 6���ֽ�
			{
				len = 6;
				Type = (OI - 0x4104) +eMeterRateVoltage;
			}
			if( pData[0] == Visible_string_698 )
			{
				if( pData[1] == len )
				{
					DAR = DAR_Success;
					memcpy(Buf, &pData[2], len);
					Result = api_ProcMeterTypePara(WRITE, Type, Buf );
				}
			}
		}
	}
	else
	{
		if (OI == 0x4106)//4106����2Ϊ������������3��С����������4ת�۵���
			{
				if( pData[0] == Visible_string_698 )
				{
					if(pData[1] < 10)
					{
						if(ClassAttribute == 0x03)
						{
							Type = eMeterMinCurrent;
							DAR = DAR_Success;
							memcpy(Buf, &pData[1], pData[1]+1);
							Result = api_ProcMeterTypePara(WRITE, eMeterMinCurrent, Buf );
						}
						else if(ClassAttribute == 0x04)
						{
							DAR = DAR_Success;
							memcpy(Buf, &pData[1], pData[1]+1);
							Result = api_ProcMeterTypePara(WRITE, eMeterTurningCurrent, Buf );

						}
						else
						{
						}
					}

				}
			}
	}
	
	if( DAR == DAR_Success )
	{
		if( Result != FALSE )
		{
			DAR = DAR_Success;
		}
		else
		{
			DAR = DAR_HardWare;
		}
	}
	
	return DAR;
}

//--------------------------------------------------
//��������:    ����MCU����������F20B
//����:      WORD OI[in]               ����OI
//         
//         BYTE ClassAttribute[in]   Ԫ������
//
//         BYTE ClassIndex[in]       ��������
//         
//         BYTE *pData[in]           ����ָ��  
//                           
//����ֵ:     BYTE DAR���   
//         
//��ע����:    ��
//--------------------------------------------------
// static BYTE SetMcuF20B( BYTE Ch, BYTE ClassAttribute, BYTE ClassIndex, BYTE *pData )
// {
// 	BYTE i, iMax, Offset;
// 	TTwoByteUnion Len;
// 	BYTE TmpBuf[250] = {0};
// 	BYTE NumOfMaster, NumOfSlave;
// 	TSafeMem_dev_work dev_work;

// 	iMax = 0;
// 	i = 0;
// 	Len.w = 0;
// 	NumOfMaster = 0;
// 	NumOfSlave = 0;
	
// 	//F20B������4���豸�б������6��������֧�����ã�����5��֧�ֱ�BLE����
// 	if( (ClassAttribute == 4) || (ClassAttribute == 7) )	//����4 ���豸�б�����7 �����б�
// 	{
// 		Offset = 0;
// 		if( ClassIndex == 0 )//����ȫ�����豸�б�
// 		{
// 			if( pData[Offset] != Array_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 3 )//���豸�б����3���ṹ��
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			iMax = 3;
// 			i = 0;
// 		}
// 		else
// 		{
// 			if( ClassIndex > 3 )//���豸�������3��
// 			{
// 				return DAR_OverRegion;
// 			}
// 			iMax = ClassIndex;
// 			i = ClassIndex - 1;
// 		}
		
// 		for( ; i < iMax; i++ )
// 		{
// 			if( pData[Offset] != Structure_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset ++;		
// 			if( pData[Offset] != 2 )//���豸�ṹ�����2��Ԫ��
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			if( pData[Offset] != Octet_string_698 )
// 			{
// 				return 	DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w != 6 )//���豸mac��ַΪ6�ֽ�
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;

// 			//memcpy( (BYTE *)&TmpBuf[0], (BYTE *)&pData[Offset], Len.w );//���豸����mac��ַ
// 			if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
// 			{
// 				return DAR_RefuseOp;
// 			}
// 			memcpy( (BYTE *)&dev_work.tDownMessage[i].DownMac, (BYTE *)&pData[Offset], Len.w );//���豸����mac��ַ
// 			Offset += Len.w;
// 			if( pData[Offset] != Octet_string_698 )
// 			{
// 				return 	DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			//if( Len.w > MAX_SLAVE_BLE_PROPERTY_LEN )//���豸�ʲ����MAX_SLAVE_BLE_PROPERTY_LEN�ֽ�
// 			//{
// 			//	return DAR_OverRegion;
// 			//}
// 			Offset++;

// 			//memcpy( (BYTE *)&TmpBuf[6+MAX_SLAVE_BLE_PROPERTY_LEN-Len.w], (BYTE *)&pData[Offset], Len.w );//���豸�ʲ���
			
// 			//����б���Ĵ��豸�б�����󳤶ȱ��棬�����ݵ�λ�ò�0��TmpBuf��ʼ��Ϊȫ0��
// 			memset( (BYTE *)&TmpBuf[0], 0x00, sizeof(TmpBuf) );

// 			memcpy( (BYTE *)&TmpBuf[0], (BYTE *)&pData[Offset], sizeof(TmpBuf) );
// 			memcpy( (BYTE *)&dev_work.tDownMessage[i].DownID, (BYTE *)&TmpBuf[0], 16 );
// 			if (api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
// 			{
// 				return DAR_RefuseOp;
// 			}
// 			Offset += Len.w;
// 		}
// 	}
// 	else if( ClassAttribute == 5 )
// 	{
// 		//ֻ������ͨ�����ͻ�����ַΪ0xFEʱ����ʾ���ò�������BLE�����ǵ������豸����������������Ϣ���������Ϊֻ��

// 		Offset = 0;
// 		if( ClassIndex == 0 )
// 		{
// 			if( pData[Offset] != Array_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 1 )//������Ϣֻ��1���ṹ�壬��Ϊ���ֻ��1������
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 		}
// 		else
// 		{
// 			if( ClassIndex != 1 )//������Ϣֻ��1���ṹ�壬��Ϊ���ֻ��1������
// 			{
// 				return DAR_OverRegion;
// 			}
// 		}

// 		if( pData[Offset] != Structure_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		if( pData[Offset] != 3 )//������Ϣ�ṹ�����3��Ԫ��
// 		{
// 			return DAR_OverRegion;
// 		}
// 		Offset++;
// 		if( pData[Offset] != OAD_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[0], (BYTE *)&pData[Offset], 4 );//�˿ںţ�ע����ȡ���Ķ˿ں�ʱ�̶��Է���0xF20B0201
// 		Offset += 4;
		
// 		//���ӵ����豸
// 		if( pData[Offset] != Array_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		//if( pData[Offset] > MAX_MASTER_NUM )//�������2�����豸
// 		//{
// 		//	return DAR_OverRegion;
// 		//}
// 		NumOfMaster = pData[Offset];//��array����Ϊ0��˵�����ӵ����豸����Ϊ0
// 		Offset++;
// 		for( i = 0; i < NumOfMaster; i++ )
// 		{
// 			if( pData[Offset] != Structure_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 2 )
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			if( pData[Offset] != Visible_string_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w > 32 )//�����������32�ֽ�
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			memcpy( (BYTE *)&TmpBuf[4+i*38], (BYTE *)&pData[Offset], Len.w );//���豸����������
// 			Offset += Len.w;
// 			if( pData[Offset] != Octet_string_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w != 6 )//mac��ַ�̶�6�ֽ�
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			memcpy( (BYTE *)&TmpBuf[4+i*38+32], (BYTE *)&pData[Offset], Len.w );//���豸����MAC��ַ
// 			Offset += Len.w;
// 		}

// 		//���ӵĴ��豸
// 		if( pData[Offset] != Array_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		//if( pData[Offset] > MAX_SLAVE_NUM )//�������3�����豸
// 		//{
// 		//	return DAR_OverRegion;
// 		//}
// 		NumOfSlave = pData[Offset];//��array����Ϊ0��˵�����ӵĴ��豸����Ϊ0
// 		Offset++;
// 		for( i = 0; i < NumOfSlave; i++ )
// 		{
// 			if( pData[Offset] != Structure_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 2 )
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			if( pData[Offset] != Visible_string_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w > 32 )//�����������32�ֽ�
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			memcpy( (BYTE *)&TmpBuf[4+NumOfMaster*38+i*38], (BYTE *)&pData[Offset], Len.w );//���豸����������
// 			Offset += Len.w;
// 			if( pData[Offset] != Octet_string_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			Deal_698DataLenth( (BYTE *)&pData[Offset], Len.b, ePROTOCOL_TYPE );
// 			if( Len.w != 6 )//mac��ַ�̶�6�ֽ�
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 			memcpy( (BYTE *)&TmpBuf[4+NumOfMaster*38+i*38+32], (BYTE *)&pData[Offset], Len.w );//���豸����MAC��ַ
// 			Offset += Len.w;
// 		}
// 	}
// 	else if( ClassAttribute == 6 )
// 	{
// 		Offset = 0;
// 		if( ClassIndex == 0 )
// 		{
// 			if( pData[Offset] != Array_698 )
// 			{
// 				return DAR_WrongType;
// 			}
// 			Offset++;
// 			if( pData[Offset] != 1 )//��������ֻ��1���ṹ�壬��Ϊ���ֻ��1������
// 			{
// 				return DAR_OverRegion;
// 			}
// 			Offset++;
// 		}
// 		else
// 		{
// 			if( ClassIndex != 1 )//��������ֻ��1���ṹ�壬��Ϊ���ֻ��1������
// 			{
// 				return DAR_OverRegion;
// 			}
// 		}

// 		if( pData[Offset] != Structure_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		if( pData[Offset] != 4 )//���������ṹ�����4��Ԫ��
// 		{
// 			return DAR_OverRegion;
// 		}
// 		Offset++;
// 		if( pData[Offset] != OAD_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[0], (BYTE *)&pData[Offset], 4 );//�˿ںţ�ע����ȡ���Ķ˿ں�ʱ�̶��Է���0xF20B0201
// 		Offset += 4;
// 		if( pData[Offset] != Unsigned_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[4], (BYTE *)&pData[Offset], 1 );//���书�ʵ�
// 		Offset++;
// 		if( pData[Offset] != Long_unsigned_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[5], (BYTE *)&pData[Offset], 2 );//�㲥���
// 		Offset += 2;
// 		if( pData[Offset] != Long_unsigned_698 )
// 		{
// 			return DAR_WrongType;
// 		}
// 		Offset++;
// 		memcpy( (BYTE *)&TmpBuf[7], (BYTE *)&pData[Offset], 2 );//ɨ����
// 		Offset += 2;

// 		if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
// 		{
// 			return DAR_RefuseOp;
// 		}
// 		memcpy(&dev_work.work_para,&TmpBuf[4],sizeof(dev_work.work_para));
// 		if (api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work),sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
// 		{
// 			return DAR_RefuseOp;
// 		}
// 		api_delayinit_ble();
// 	}
// 	else
// 	{
// 		return DAR_RefuseOp;
// 	}

// 	return DAR_Success;
// }

//--------------------------------------------------
//��������:    ������������豸����
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���        
//         
//��ע����:
//--------------------------------------------------
eAPPBufResultChoice SetRequestInOuputPara( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
    
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	
	switch(OI.w)
	{
		// case 0xF20B://����
	    //     DAR = SetMcuF20B( Ch, ClassAttribute,  ClassIndex, pData);
		// 	break;
		default:
			DAR = DAR_WrongType;
			break;		
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
	
	return eResultChoice;
}

//--------------------------------------------------
//��������:    ���ð�ȫģʽ����
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetRequestSafeMode( BYTE Ch, BYTE *pOAD )
{
    BYTE i;
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;		

	if( ClassAttribute == 3 )
	{
		if( ClassIndex != 0 )
		{
			if( ClassIndex > SafeModePara.Num )//��������һ��ʱֻ������������õ�
			{
				 DAR = DAR_OverRegion;
			}
			else
			{
				if( pData[0] == Structure_698 )
				{
					if( pData[1] == 2 )
				    {
				    	if( pData[2] == OI_698 )
				       	{
				        	lib_ExchangeData((BYTE*)&SafeModePara.Sub[ClassIndex-1].OI, (BYTE*)&pData[3], 2);
				        	
				            if( pData[5] == Long_unsigned_698 )
				            {
				            	lib_ExchangeData((BYTE*)&SafeModePara.Sub[ClassIndex-1].wSafeMode, (BYTE*)&pData[6], 2);
				            	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
								DAR = DAR_Success;
				            }
				        }
				    }
				}
			}
		}
		else
		{
			if( pData[0] == Array_698 )
			{
				if( pData[1] > MAX_SAFE_MODE_PARA_NUM )//���õĸ������ܳ�������������
				{
					DAR = DAR_OverRegion;
				}
				else
				{
					for( i = 0; i < pData[1]; i++ )
					{
						if( pData[2+8*i] == Structure_698 )
						{
							if( pData[3+8*i] == 2 )
							{
								if( pData[4+8*i] == OI_698 )
								{
									lib_ExchangeData((BYTE*)&SafeModePara.Sub[i].OI, (BYTE*)&pData[5+8*i], 2);
									if( pData[7+8*i] == Long_unsigned_698 )
									{
										DAR = DAR_Success;
										lib_ExchangeData((BYTE*)&SafeModePara.Sub[i].wSafeMode, (BYTE*)&pData[8+8*i], 2);
									}
									else
									{
										DAR = DAR_WrongType;
										break;
									}
								}
								else
								{
									DAR = DAR_WrongType;
									break;
								}
							}
							else
							{
								DAR = DAR_WrongType;
								break;
							}
						}
						else
						{
							DAR = DAR_WrongType;
							break;
						}
					}
				}

				if( DAR == DAR_Success )
				{
					SafeModePara.Num = pData[1]; 
					api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProSafeRom.SafeModePara), sizeof(TSafeModePara), (BYTE *)&SafeModePara );
				}
			}
	   }
	}
	else
	{
		DAR = DAR_WrongType;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
    return eResultChoice;
}

//--------------------------------------------------
//��������:    ����esam����
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetRequestESAM( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex,Buf[10];
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	WORD Result;
	
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_RefuseOp;		

    if( ClassIndex == 0 )
    {
    	if( (ClassAttribute == 0X0E)&&( CURR_COMM_TYPE( Ch ) != COMM_TYPE_TERMINAL ) )//���ú�����֤ʱЧ
    	{
    	    if( pData[0] == Double_long_unsigned_698 )
    	    {
    	        lib_ExchangeData( Buf, (BYTE*)&pData[1], 4 );//�������ݵ���
				Result = api_WritePrePayPara( eIRTime, Buf );
			   
			   if( Result == TRUE )
               {
                    DAR = DAR_Success;
               }
    	    }
    	}	
    }
    
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
	
    return eResultChoice;

}

//--------------------------------------------------
//��������:    ���òα������
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���        
//         
//��ע����:
//--------------------------------------------------
eAPPBufResultChoice SetRequestVariable( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR;
	BYTE *pData;
	BOOL Result;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	DAR = DAR_RefuseOp;


    if( OI.w == 0x2015 )//2015
    {
    	if( pOAD[2] == 4 )//����4�����ܱ�����ϱ�ģʽ�֣���= bit-string(SIZE(32))
    	{
        	DAR = SetRequestFollowReportMode( pData );
    	}
    	else if( pOAD[2] == 5 )//����5 �ϱ���ʽ
    	{
			if( pData[0] == Enum_698 )
			{
				Result = api_SetEventReportMode( eReportMethod, OI.w, pData[1] );
				if(Result == FALSE )
				{
					DAR = DAR_HardWare;
				}
				else
				{
					DAR = DAR_Success;
				}
			}
    	}
    	else
    	{}

    }

    eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
	
	return eResultChoice;
}

//--------------------------------------------------
//��������:    ���ü�����·��
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE SetSampleChip( BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData )
{
	BYTE DAR,Result;
	eAPPBufResultChoice eResultChoice;
	BYTE chipNo;
	
	if( (ClassAttribute != 2) || ( ClassIndex != 0 ) )
	{
		return DAR_WrongType;
	}	

	DAR = DAR_WrongType;

    if( ( ClassIndex == 0 ) && (ClassAttribute == 0x02) )
    {
	    if( pData[0] == Unsigned_698 )
	    {
			for(chipNo = 0; chipNo < SAMPLE_CHIP_NUM; chipNo++)
			{
				Result = api_SetSampleChip(pData[1], chipNo);
			}
		   
			if( Result == TRUE )
			{
				DAR = DAR_Success;
			}
	    }	
    }
	
    return DAR;

}
//--------------------------------------------------
//��������:  
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BYTE  SetShakeTime(BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData)
{
	if ((ClassAttribute!=0x02)||(ClassIndex!=0x00)||((*pData)!=0x06))
	{
		return DAR_WrongType;
	}
	else
	{
		lib_ExchangeData(pData+1, pData+1,4);
		if (api_WriteDIPara(0,pData+1)==FALSE)
		{
			return DAR_RefuseOp;
		}
		else
		{
			return DAR_Success;
		}

	}
	
}
//--------------------------------------------------
//��������:    ���òα������
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���        
//         
//��ע����:
//--------------------------------------------------
eAPPBufResultChoice SetRequestParameter( BYTE Ch, BYTE *pOAD )
{
	BYTE DAR,ClassAttribute,ClassIndex;
	BYTE *pData;
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOAD[4];//��ȡ����ָ��
	lib_ExchangeData(OI.b,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];
	DAR = DAR_WrongType;
	
	switch(OI.w)
	{
	case 0x4000://����ʱ��
		DAR = SetRTC( ClassAttribute, ClassIndex, pData);
		break;
	case 0x4001://ͨ�ŵ�ַ
	case 0x4002://���
	case 0x4003://�ͻ����
		DAR = SetCommAddrOrMeterNoOrCustomCode( OI.w, ClassAttribute,ClassIndex, pData, Ch);
		break;
	case 0x4004://�豸����λ��	
		DAR = SetPsitionInfo( ClassAttribute, ClassIndex,  pData);
		break;
	// case 0x4007://lcd��ʾ����
	// 	DAR = SetLCDPara( ClassAttribute, ClassIndex, pData);
	// 	break;
	case 0x4008://ʱ���л�ʱ��
	case 0x4009://ʱ�α��л�ʱ��
//	#if( PREPAY_MODE == PREPAY_LOCAL )//����Ԥ����֧��
//	case 0x400A://�����л�ʱ��
//	case 0x400B://�����л�ʱ��
//	#endif
        DAR = SetSwitchTime( 0X00, OI.w, ClassAttribute, ClassIndex, pData);
			
		break;
	case 0x400c://ʱ��ʱ����
		DAR = Set_Timezone_TimeInterval_Num( ClassAttribute,  ClassIndex, pData);
		break;
    #if( PREPAY_MODE == PREPAY_LOCAL )//����֧������
    case 0x400D://������
	    DAR = Set_LadderNum( ClassAttribute,  ClassIndex, pData);
	    break;
	#endif
	case 0x400f://��Կ������
		DAR = SetEsamKeyTotalNum( ClassAttribute,  ClassIndex, pData);
		break;
	case 0x4010://������·��
		DAR = SetSampleChip( ClassAttribute,  ClassIndex, pData);
		break;
#if( MAX_PHASE != 1 )	
	case 0x4100://�����������
	case 0x4101://����ʱ��
		DAR = SetDemandPara( OI.w, ClassAttribute, ClassIndex, pData);
		break;		
#endif//#if( MAX_PHASE != 1 )	
	case 0x4104://���ѹ   //�ʲ�������뵥������
	case 0x4105://�����/��������
	case 0x4106://������
	case 0x4107://�й�׼ȷ�ȵȼ�
	case 0x4108://�޹�׼ȷ�ȵȼ�
	case 0x4109://���ܱ��й�����
	case 0x410A://���ܱ��޹�����
	case 0x410B://���ܱ��ͺ�
	case 0x4111://���������
		DAR = SetMeterBasicPara( OI.w, ClassAttribute, ClassIndex, pData);
		break;
	case 0x4011://�������ձ�
		DAR = SetTimeHolidayTable( ClassAttribute,  ClassIndex, pData);
		break;
	case 0x4012://������������
		DAR = SetWeekStatus( ClassAttribute, ClassIndex, pData);
		break;
	case 0x4013://�����ղ��õ���ʱ�α��
		DAR = SetWeekSeg( ClassAttribute, ClassIndex, pData);
		break;

	case 0x4014://��ǰ��ʱ��
		DAR = SetTimezoneTable( 0x01, ClassAttribute, ClassIndex,  pData);
		break;
		
	case 0x4015://������ʱ��
		DAR = SetTimezoneTable( 0x81,ClassAttribute, ClassIndex,  pData);
		break;
		
	case 0x4016://��ǰ��ʱ�α�
		DAR = SetTimeTable( 0x03, ClassAttribute, ClassIndex,  pData);
		break;
		
	case 0x4017://������ʱ�α�
		DAR = SetTimeTable( 0x83, ClassAttribute, ClassIndex,  pData);
		break;

/*	case 0x401C://�������������
	case 0x401D:
	    if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE )
	    {
            DAR = SetPTCT( OI.w, ClassAttribute, ClassIndex, pData);
	    }
	    else
	    {
            DAR = DAR_RefuseOp;
	    }
		
		break;*/
		
    #if( PREPAY_MODE == PREPAY_LOCAL )//����֧������
	//case 0x401e://���������ֵ
    case 0x401f://���������ֵ
        DAR = SetLimitMoney( 0x00, OI.w, ClassAttribute, ClassIndex, pData );
	    break;
	#endif
	
	case 0x4030://��ѹ�ϸ��ʲ���
		DAR = SetVoltageRatePara( OI.w, ClassAttribute,  ClassIndex, pData);
		break;
	case 0x4103:
		DAR = SetPropertyCode( ClassAttribute, ClassIndex, pData);
		break;
		
	case 0x4112://�й����ģʽ��
#if( MAX_PHASE != 1 )
	case 0x4113://�޹����ģʽ��1
	case 0x4114://�޹����ģʽ��2
#endif//#if( MAX_PHASE != 1 )	
		DAR = SetCOMStatus( OI.w, ClassAttribute, ClassIndex, pData);
		break;
#if( MAX_PHASE != 1 )	
	case 0x4117://�ڼ�������������
		DAR = SetIntervalDemandFreezingPeriod( ClassAttribute,  ClassIndex, pData);	
		break;	
#endif//#if( MAX_PHASE != 1 )	
	case 0x4116://������
		DAR = SetBillingPara( ClassAttribute, ClassIndex, pData);
		break;
	
	case 0x4300://�����豸
		DAR = SetMeter( ClassAttribute,  ClassIndex, pData);
		break;
		
	case 0x4401://Ӧ����������
		DAR = SetPassWord( ClassAttribute, ClassIndex, pData );
		break;
	case 0x4500://����ͨ��ģ��1
	case 0x4501://����ͨ��ģ��2
		DAR = SetGPRSdBm( ClassAttribute, ClassIndex, pData );
		break;
	case 0x4908:
		DAR = SetShakeTime( ClassAttribute, ClassIndex, pData );
        break;
    #if (SEL_TOPOLOGY == YES )	    
    case 0x4E06:
		DAR = api_SetTopoPara698( ClassAttribute, ClassIndex, pData );
        break;
    #endif
    #if (SEL_SEARCH_METER == YES )	
    case 0x6002://�ѱ����
        DAR = api_SetSchedMeter698( ClassAttribute, ClassIndex, pData );
        break;
    #endif
	default:
		DAR = DAR_WrongType;
		break;		
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR);//����������
	
	return eResultChoice;
}
//--------------------------------------------------
//��������:    ���ò�������ͨ����
//         
//����:      BYTE Ch[in]     ͨ��ѡ��
//         
//         BYTE *pOAD[in]  OAD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���        
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetRequestNormal( BYTE Ch,BYTE *pOAD)
{
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	BYTE OAD[4],dar;
	WORD DataLen;
	DWORD oadd=0;


    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
	lib_ExchangeData(OI.b,pOAD,2);
	lib_ExchangeData((BYTE*)&oadd,pOAD,4);
	eResultChoice = DLT698AddBuf(Ch,0, pOAD, 4);//���OAD���� ������������
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	memcpy( OAD, pOAD, 4);//������OAD;
	
    DataLen = GetProtocolDataLen( pOAD+4);//��ȡData���� DataLen�Ĵ��󳤶��ں���������ж�

	if( APPData[Ch].TimeTagFlag == eTime_Invalid )//ʱ���ǩ����
    {
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_TimeStamp);
    }
	else if(JudgeTaskAuthority( Ch, eSET_MODE, pOAD) == FALSE)//�жϰ�ȫģʽ�������Ƿ���Ӧ������
	{	
		if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_HangUp);
		}
		else
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_PassWord);
		}
	}	
    else if( (OI.w >= 0x0000) && (OI.w <= 0x2133) )//�����ࡢ�����ࡢ������ ��֧������ ͳһ���ؾܾ�����
    {
        eResultChoice = SetRequestVariable( Ch, pOAD );
    }
	else if( (OI.w >= 0x3000) && (OI.w <= 0x330e) 
            #if( SEL_TOPOLOGY == YES )
            || (OI.w ==0x3E03)
            #endif
     )//�����¼������
	{
		eResultChoice = SetRequestEvent( Ch, pOAD );
	}

	else if( ((OI.w >= 0x4000) && (OI.w <= 0x4517)) ||(OI.w == 0x4908)
	  #if (SEL_SEARCH_METER == YES )
	  ||(OI.w == 0x6002)
	  #endif
      #if (SEL_TOPOLOGY == YES )
	  ||(OI.w == 0x4E06)
	  #endif
	)//���òα��� 
	{
		eResultChoice = SetRequestParameter( Ch, pOAD );
	}
	else if( (OI.w >= 0x5000) && (OI.w <= 0x5011))//���ö���
	{
		eResultChoice = SetRequestFreeze( Ch, pOAD );
	}
	else if(OI.w == 0x6000)//���òɼ�
	{
		Class11_Set(oadd,pOAD+4,(DAR*)&dar);
		eResultChoice = DLT698AddOneBuf(Ch,0x55, dar);
	}
	else if( (OI.w >= 0x8000) && (OI.w <= 0x8001))//���ÿ��Ʋ���
	{
		eResultChoice = SetRequestControl( Ch, pOAD );
	}
	#if(SEL_ESAM_TYPE != 0)
	else if( OI.w == 0xf100 )//����ESAM��ز���
	{
		eResultChoice = SetRequestESAM( Ch, pOAD );
	}
	else if( OI.w == 0xf101 )//���ð�ȫģʽ����
	{
		eResultChoice = SetRequestSafeMode( Ch, pOAD );
	}
	#endif
	else if( OI.w == 0xF203 )//���ÿ����������־
	{
		eResultChoice = SetDIInputFlag(Ch, pOAD );
	}
	// else if( OI.w == 0xF20B )//�����ⲿ�豸
	// {
	// 	eResultChoice = SetRequestInOuputPara(Ch, pOAD );
	// }
	else if( OI.w == 0xf221 )//���ó��׮
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x55, SetCharging( pOAD, pOAD+4));
	}
	// else if( (OI.w >= 0xF300) && (OI.w <= 0xF301))//����Һ��
	// {
	// 	eResultChoice = SetRequestLCD( Ch, pOAD);
	// }
	else//����
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x55, DAR_Undefined);//����������
	}
	
	if( eResultChoice == eADD_OK )
	{
	    if( APPData[Ch].pAPPBuf[(APPData[Ch].APPCurrentBufLen.w-1)] == DAR_Success )//��Buf����1���ֽ� �ж�DAR ���óɹ���¼��̼�¼
	    {
            api_SaveProgramRecord( EVENT_START,0x51, OAD);
	    }		
	}
	//Ϊ�����ֳ�һֱ���ù���ͨ��ģ���ź�ǿ�ȣ����ù���ͨ��ģ�鲻��¼�����¼�
	if((OI.w != 0x4500) && (OI.w != 0x4501))
	{
		api_WriteFreeEvent( (EVENT_PROGRAM_698 << 8) + OAD[2], OI.w );
	}
	
	if( DataLen == 0x8000 )//���Ȳ����������������ٽ���list���ݴ���
	{
        return eAPP_RETURN_DATA;
	}
	
	APPData[Ch].BufDealLen.w += (DataLen+4);
	return eResultChoice;
}
//--------------------------------------------------
//��������:    �����������
//         
//����:      BYTE Ch[in]  ͨ��ѡ��
//         
//����ֵ:     eAPPBufResultChoice �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetRequestNormalList( BYTE Ch)
{
	BYTE i,LenOffest;
	WORD NoAddress;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion Num,No;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;

	No.w = 0;

	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//��Ӹ��� ��������֡����ȷ
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	NoAddress = APPData[Ch].APPCurrentBufLen.w;
	
	for(i=0;i<Num.w;i++)
	{
		eResultChoice = SetRequestNormal( Ch, &LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);
		
		if( eResultChoice != eADD_OK )//��Ӳ��ɹ��ͷ���
		{
			return eResultChoice;
		}
		
		No.w++; //sequence of set_result
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//�Ѵ������ݴ����·������ݽ��з��أ���������ĵĸ���
		{
			break;
		}
	}
	
	JudgeAppBufLen(Ch);
	
	Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
	
	return eResultChoice;
}
//--------------------------------------------------
//��������:    ���ú��ȡ-�б�
//         
//����:      BYTE Ch[in]  ͨ��ѡ��
//         
//����ֵ:     eAPPBufResultChoice �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice SetThenGetRequestNormalList( BYTE Ch )
{
	BYTE i,LenOffest;
	WORD NoAddress;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion Num,No;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;
	No.w = 0;
	
	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//��Ӹ��� ��������֡����ȷ
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}

	NoAddress = APPData[Ch].APPCurrentBufLen.w;
	
	for(i=0;i<Num.w;i++)
	{
		//��������
		eResultChoice = SetRequestNormal( Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w] );//
		if( eResultChoice != eADD_OK )//��Ӳ��ɹ��ͷ���
		{
			return eResultChoice;
		}
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//�Ѵ������ݴ����·������ݽ��з��أ���������ĵĸ���
		{
			break;
		}
		//��ȡ����
		eResultChoice= GetRequestNormal( Ch, &LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);
		if( eResultChoice != eADD_OK )//��Ӳ��ɹ��ͷ���
		{
			return eResultChoice;
		}
		
		APPData[Ch].BufDealLen.w += 1;//��ȡ��ʱ
		 
		No.w ++;//sequence of set_then_get_result
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//�Ѵ������ݴ����·������ݽ��з��أ���������ĵĸ���
		{
			break;
		}
	}
	
	JudgeAppBufLen(Ch);
	
	Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
	
    return eResultChoice;
}

//--------------------------------------------------
//��������:    esam���ݸ��£����µ�epprom�У�
//         
//����:      BYTE* pOAD[in]    OADָ��
//         
//         BYTE *pData       ����ָ��
//
//����ֵ:     BYTE DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE EsamDataUpdate( BYTE* pOAD, BYTE *pData )
{
    WORD OI,Result,Type;
    BYTE ClassAttribute,ClassIndex,DAR;
    
    DAR = DAR_RefuseOp;//AR �ɹ�   
    lib_ExchangeData((BYTE*)&OI,pOAD,2);//��ȡOI
	ClassAttribute = (pOAD[2]&0x1f);
	ClassIndex = pOAD[3];

    switch( OI )
    {
        case 0x4002://���
        case 0x4003://�ͻ����
            if( api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE )
		    {
                return DAR_PassWord;
		    }

            if( OI == 0x4002 )
            {
                if( pData[0] != 8 )//���ݳ��ȱ���Ϊ8
                {
                    return DAR_RefuseOp;
                }
                Type = eMeterMeterNo;
                pData = pData+2;
            }
            else
            {                
                if( pData[0] != 6 )//���ݳ��ȱ���Ϊ6
                {
                    return DAR_RefuseOp;
                }
                
                Type = eMeterCustomCode;
            }
            
            //��֤esam
            Result = api_AuthDataSID( ProtocolBuf );
			if( Result == FALSE )
			{
			   return DAR_EsamFial;
			}

            Result =api_ProcMeterTypePara( WRITE, Type, pData+1 );//ESAM���8���ֽ� epprom 6���ֽ�
            if( Result != FALSE )
            {
                DAR = DAR_Success;
            }
            break;
        #if( PREPAY_MODE == PREPAY_LOCAL)
        case 0x400A://�����׷�ʱ�����л�ʱ��
        case 0x400B://�����׽��ݵ���л�ʱ��
            DAR = SetSwitchTime( 0x55, OI, ClassAttribute, ClassIndex, pData );
            break;
        case 0x401e://�����������
            DAR = SetLimitMoney( 0x55, OI, ClassAttribute, ClassIndex, pData );
            break;
		case 0x4019://�����׵��
            DAR = SetBackupRate( ClassAttribute, ClassIndex, pData );
            break;
        case 0x401B://�����׽���
            DAR = SetBackupLadder( ClassAttribute, ClassIndex, pData );
            break;
        #endif 
            
        case 0x401c://�������������
        case 0x401d://��ѹ���������
            DAR = SetPTCT( OI, ClassAttribute, ClassIndex, pData);
            break;
            
        default:
        	DAR = DAR_Success;
            break;
    }
    
    if( DAR == DAR_Success )
    {
        api_SaveProgramRecord( EVENT_START, 0x53, pOAD);//���ݸ��±���OAD 
    }

    return DAR;
}

//--------------------------------------------------
//��������:    Զ�̿��Ʋ���
//         
//����:      BYTE Ch[in]        ͨ��ѡ��
//         
//         BYTE MethodNo[in]  ������
//         
//         BYTE *pData[in]    ����ָ��
//         
//����ֵ:     BYTE DAR���       
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionRemoteControl(BYTE Ch, BYTE MethodNo,BYTE *pData )
{
	BYTE DAR,RelayOpType,RelayWaitOffTime;
	WORD Result,Offest;
	TTwoByteUnion RelayOffWarnTime,Len;
	
	Result = 0x8000;
	DAR = DAR_WrongType;
	RelayWaitOffTime = 0;
	RelayOffWarnTime.w = 0;
	RelayOpType = 0;
	
	switch( MethodNo )
	{
		case 127://��������
		    if( pData[0] == NULL_698 )//����ΪNull
		    {
                RelayOpType = eCMD_InAlarm;
                DAR = DAR_Success;
                if( (APPData[Ch].eSafeMode >= eEXPRESS_MAC) && (APPData[Ch].eSafeMode <= eSECRET_MAC) )//Ȩ���޶�������+mac�����ģ�����+mac
                {
                    FactoryTime = (10*60);
                }
		    }
			break;
		case 128://�������
		    if( pData[0] == NULL_698 )//����ΪNull
		    {
				RelayOpType = eCMD_OutAlarm;
			    DAR = DAR_Success;	
				FactoryTime = 0;    
		    }
			break;
		case 129://��բ
			if( pData[0] == Array_698 )
			{
				if( pData[1] == 0x01)
				{
					if( pData[2] == Structure_698 )
					{
						if( pData[2+7] == Unsigned_698 )//�澯��ʱ
						{
							RelayWaitOffTime = pData[2+8];
							if( pData[2+9] == Long_unsigned_698 )//�޵���ʱ
							{
								RelayOffWarnTime.b[1] = pData[2+10] ;
								RelayOffWarnTime.b[0] = pData[2+11] ;
								if( RelayOffWarnTime.w == 0)//�޵���ʱΪ����Ϊ�����޵�
								{
									RelayOpType = eCMD_RelayOff;
									DAR = DAR_Success;
								}
								else if( pData[2+12] == Boolean_698 )//�Զ���բ
								{
									if( pData[2+13] ==  1 )//�Զ���բ
									{
										RelayOpType = eCMD_AdvOff1;//Ԥ��բ1
									}
									else
									{
										RelayOpType = eCMD_AdvOff2;//Ԥ��բ2
									}

									DAR = DAR_Success;
								}
							}
						}
					}
				}
			}
		
			break;
		case 130://��բ                  
			if( pData[0] == Array_698 )
			{
				if( pData[1] == 0x01 )
				{
	                if( pData[2] == Structure_698 )
	                {
	                	if( pData[3] == 2 )// 2����������
	                	{
							if( pData[3+6] == Enum_698 )
							{
								if( pData[4+6] == 0)
								{
									RelayOpType = eCMD_IndirOn;
								}
								else
								{
									RelayOpType = eCMD_DirectOn;
								}
								DAR = DAR_Success;
							}
                    	}
                    }
				}
			}			
			break;

		case 131://�����������բ
			if( pData[0] == Array_698 )
			{
				if( pData[1] == 01 )
				{
					if( pData[2] == Structure_698 )
					{
						if( pData[3] == 3 )// 3��Ԫ��
						{
							if( pData[3+6] == Enum_698 )
							{
								if( pData[3+7] == 0 )
								{
									RelayOpType = eCMD_IndirOn;
								}
								else
								{
									RelayOpType = eCMD_DirectOn;
								}

								if(  pData[3+8] == Visible_string_698 )
								{
									Offest = Deal_698DataLenth( (BYTE*)&pData[3+9], Len.b, ePROTOCOL_TYPE );
									Result = Judge698PassWord( (BYTE*)&pData[3+9+Offest] );
									if( Result == FALSE )
									{
										//�ü̵�������״̬��
										api_SetRelayRelayErrStatus( 0x04 );
										DAR = DAR_PassWord;
									}
									else
									{
                                        DAR = DAR_Success;
									}
								}
							}
						}
					}
				}
			}
			break;
		default:
			DAR = DAR_Undefined;//AR Ӳ��ʱЧ	
			break;
	}

	if( DAR == DAR_Success )
	{	
		Result = api_Set_RemoteRelayCmd( RelayOpType, RelayWaitOffTime, RelayOffWarnTime.w);
		if( Result & 0x8000 )
		{
			api_WriteFreeEvent(EVENT_RELAY_ERR, Result);
			DAR = DAR_RefuseOp;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR;
}
#if (SEL_SEARCH_METER == YES )
//--------------------------------------------------
//��������:    �ѱ����
//         
//����:      BYTE Ch[in]        ͨ��ѡ��
//         
//         BYTE MethodNo[in]  ������
//         
//         BYTE *pData[in]    ����ָ��
//         
//����ֵ:     BYTE DAR���       
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionSchelMeter( BYTE Ch,BYTE MethodNo,BYTE *pData )
{
	BYTE DAR,RelayOpType,i;
	WORD Result;
    WORD SearchMaxtime;
	
	Result = 0x8000;
	DAR = DAR_WrongType;

	switch( MethodNo )
	{
	case 127://�ѱ�����(�ѱ�ʱ��)
	    if( pData[0] == Long_unsigned_698 )//����ΪLong_unsigned_698
	    {
            lib_ExchangeData((BYTE*)&SearchMaxtime,&pData[1],2);
            if(SKMeter.byAutoCheck != 0)
            {
                api_StopSchMeter(); 
            }
            api_StartSearchMeter(SearchMaxtime);
            DAR = DAR_Success;
	    }
		break;
	
	case 128://����ѱ���
		if( pData[0] == NULL_698 )//����ΪNull
		{
            if(SKMeter.byAutoCheck != 0)
            {
                api_StopSchMeter(); 
            }
			api_ClearSchMeter();
			DAR = DAR_Success;
		}
		break;

	case 129://��տ�̨���ѱ���
		if( pData[0] == NULL_698 )//����ΪNull
	    {
			//��ʱδ��
			DAR = DAR_Undefined;
	    }
		break;

	default:
		break;
	}

	return DAR ;
}
#endif //#if (SEL_SEARCH_METER == YES )
//--------------------------------------------------
//��������:    Զ�̱������
//         
//����:      BYTE Ch[in]        ͨ��ѡ��
//         
//         BYTE MethodNo[in]  ������
//         
//         BYTE *pData[in]    ����ָ��
//         
//����ֵ:     BYTE DAR���       
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionPowerSupply( BYTE Ch,BYTE MethodNo,BYTE *pData )
{
	BYTE DAR,RelayOpType;
	WORD Result;
	
	Result = 0x8000;
	DAR = DAR_WrongType;

	if( APPData[Ch].TimeTagFlag == eNO_TimeTag )//�����ʱ���ǩ ����ʱ���ǩ��Ч
	{
		return DAR_TimeStamp;
	}
	
	switch( MethodNo )
	{
	case 127://��������
	    if( pData[0] == NULL_698 )//����ΪNull
	    {
            RelayOpType = eCMD_InKeep;
            DAR = DAR_Success;
	    }
		break;
	
	case 128://�������
		if( pData[0] == NULL_698 )//����ΪNull
	    {
            RelayOpType = eCMD_OutKeep;
            DAR = DAR_Success;
	    }
		break;

	default:
		break;
	}
	
	if( DAR == DAR_Success)
	{	
		Result = api_Set_RemoteRelayCmd( RelayOpType, 0, 0);
		if(  (Result & 0x8000) )
		{
			api_WriteFreeEvent(EVENT_RELAY_ERR, Result);
			DAR = DAR_HardWare;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	return DAR ;
}
//--------------------------------------------------
//��������:    ������������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestControl( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��

	if( APPData[Ch].TimeTagFlag == eNO_TimeTag )
	{
		api_SetRelayRelayErrStatus( 0x10 );
		DAR = DAR_TimeStamp;//ʱ���ǩ����

		api_WriteFreeEvent(EVENT_RELAY_ERR, R_ERR_TIME);
	}
    else
    {
        if( OI.w == 0x8000 )
    	{
    		DAR = ActionRemoteControl( Ch, MethodNo, pData);
    	}
    	else
    	{
    		DAR = ActionPowerSupply( Ch, MethodNo, pData);
    	}
    }

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	return eResultChoice;
	
}
#if (SEL_SEARCH_METER == YES )
//--------------------------------------------------
//��������:    �����ѱ��������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestSchelControl( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��

//	zh���20230505 ���Ͳ�����
//	if( APPData[Ch].TimeTagFlag == eNO_TimeTag )
//	{
//		api_SetRelayRelayErrStatus( 0x10 );
//		DAR = DAR_TimeStamp;//ʱ���ǩ����
//	}
//    else
    {
		DAR = ActionSchelMeter( Ch, MethodNo, pData);
    }

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	return eResultChoice;
	
}
#endif //#if (SEL_SEARCH_METER == YES )
//--------------------------------------------------
//��������:    �����豸�����
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf��� 
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestMeter( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	TFourByteUnion OMD;
	
	DAR = DAR_WrongType;
    lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��
	
	if( APPData[Ch].TimeTagFlag == eNO_TimeTag )//����֧�ִ�ʱ���ǩ������
	{
		DAR = DAR_TimeStamp;//ʱ���ǩ��Ч
	}
	else if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )//�����������Զ�̿���
	{
		DAR = DAR_HangUp;
	}
	else
    {
        if( pData[0] == NULL_698 )//����ΪNULL
        {
            switch( pOMD[2] )
            {
                case 3://���ݳ�ʼ��
                	if( api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE )//˽Կ�²��������
                	{
						DAR = DAR_RefuseOp;
						break;
                	}
                	
                	#if( PREPAY_MODE == PREPAY_LOCAL )
                	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )//���طѿر�ֻ�г���ģʽ�ſ���ʹ�õ�����������4300�ķ���3:���ݳ�ʼ��
                	{
                		DAR = DAR_RefuseOp;
                		break;
                	}	
                	#endif
                	
                    api_SetClearFlag( eCLEAR_METER, 1 );
                    api_WriteFreeEvent(EVENT_DATA_INIT_698,OI.w);
					api_SavePrgOperationRecord( ePRG_CLEAR_METER_NO );
                    //Ĭ����ʱ500ms~1s������ʵ����ʱ��Χ500~1500ms,1200bps ��������20�ֽ�ʱ��Ϊ 183ms
                    DAR = DAR_Success;
                break;
                
                case 4://�ָ���������
                    DAR = DAR_RefuseOp;//Ĭ��ʧ��
                break;
                
                case 5://�¼���ʼ��
                    memcpy(OMD.b,pOMD,4);//��ȡOAD
                    api_SetClearFlag( eCLEAR_EVENT, 1 );
                    //Ĭ����ʱ500ms~1s������ʵ����ʱ��Χ500~1500ms,1200bps ��������20�ֽ�ʱ��Ϊ 183ms
                    api_WritePreProgramData( 1, OMD.d );
                    api_WriteFreeEvent(EVENT_EVENT_INIT_698,OI.w);
					api_SavePrgOperationRecord( ePRG_CLEAR_EVENT_NO );
                    DAR = DAR_Success;
                break;
                
                case 6://������ʼ��
                    #if( MAX_PHASE != 1)        
					api_SavePrgOperationRecord( ePRG_CLEAR_MD_NO );
                    api_SetClearFlag( eCLEAR_DEMAND, 1 );
                    //Ĭ����ʱ500ms~1s������ʵ����ʱ��Χ500~1500ms,1200bps ��������20�ֽ�ʱ��Ϊ 183ms
                    DAR = DAR_Success ;
                    #else
                    DAR = DAR_Undefined ;
                    #endif
                    api_WriteFreeEvent(EVENT_DEMAND_INIT_698,OI.w);
                break;
                
                default:
                     DAR = DAR_WrongType;//Ĭ��ʧ��
                break;
            }
        }

    }   
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
	return eResultChoice;	
}
//--------------------------------------------------
//��������:    698���������¼�
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf��� 
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestEvent( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	TFourByteUnion OAD,OMD;
	BYTE DAR,MethodNo;
	BYTE *pData;
	WORD Result;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��
	Result = FALSE;
	
	switch ( MethodNo )
	{
		case  1://��λ
			if( APPData[Ch].TimeTagFlag == eNO_TimeTag )//����֧�ִ�ʱ���ǩ������
        	{
        		DAR = DAR_TimeStamp;//ʱ���ǩ��Ч
        	}       	
		    else if( (OI.w == 0x3015) || (OI.w == 0x3013) )//�¼�����͵�����㲻������������
            {
                DAR = DAR_RefuseOp;
            }
            else if( pData[0] == Integer_698 )//����ΪInteger_698
            {
                DAR = DAR_Success;
                memcpy(OMD.b,pOMD,4);//��ȡOAD
                Result = api_ClrSepEvent(OI.w);
                api_WritePreProgramData( 1, OMD.d );
				api_SavePrgOperationRecord( ePRG_CLEAR_EVENT_NO );
				//�¼���λ��Ҫ���������¼��б�
				SetReportIndex( eSUB_EVENT_PRG_CLEAR_EVENT, EVENT_START+EVENT_END );
            }
			break;
		case  2://ִ��
			DAR =DAR_WrongType;
			break;
		case  3://��������		
			break;
		case  4://���һ�������������		
			if( pData[0] == OAD_698 )
			{
				memcpy(OAD.b,&pData[1],4);//ȡOAD
				DAR = DAR_Success;
			}
			else
			{
				break;
			}
			
			if((OI.w == 0x3013) && (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE) )//˽Կ״̬�²���������¼���������������Ա�
            {
                DAR = DAR_RefuseOp; 
            }
			else if( DAR == DAR_Success )
			{
				Result = api_AddEventAttribute(OI.w, OAD.b);
			}
			break;

		case  5://ɾ��һ�������������		
			if( pData[0] == OAD_698 )
			{
				memcpy(OAD.b,&pData[1],4);//ȡOAD
				DAR = DAR_Success;
			}
			else
			{
				break;
			}
            if((OI.w == 0x3013) && (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE) )//˽Կ״̬�²���������¼���������������Ա�
            {
                DAR = DAR_RefuseOp; 
            }
			else if( DAR == DAR_Success )
			{
				Result = api_DeleteEventAttribute(OI.w,OAD.b );
			}
			break;		
		default:
			break;
	}
	
	if( DAR == DAR_Success )//���ݳɹ��� �жϲ������
	{
		if( Result == FALSE )
		{
			DAR = DAR_RefuseOp;
		}
		else
		{
			DAR = DAR_Success;
		}
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
	return eResultChoice;
}
//--------------------------------------------------
//��������:    698�������һ�����Զ�������
//         
//����:      BYTE Type[in]    ����OI
//         
//         BYTE *pData[in]  ����ָ��
//         
//����ֵ:     BYTE DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionAddFreeze( WORD Type,BYTE *pData )
{
	BYTE DAR,Buf[10];
	WORD Result;
        
	Result = FALSE;
	DAR = DAR_WrongType;

	if( (pData[0] == Structure_698) &&  (pData[1] == 0x03))//�ж�����
	{
		if( pData[2] ==Long_unsigned_698)//��������
		{
			lib_ExchangeData(Buf,&pData[3],2);//ȡCYCLE
			if( pData[5] == OAD_698 )
			{
				memcpy(Buf+2,&pData[6],4);//ȡOAD	
				if( pData[10] == Long_unsigned_698 )
				{
					lib_ExchangeData(Buf+6,&pData[11],2);//ȡCYCLE
					DAR = DAR_Success;
				}
			}
		}
	}
	
	if( DAR ==DAR_Success )
	{
		Result = api_AddFreezeAttribue( Type, Buf, 1 );
		if( Result == TRUE )
		{
			DAR = DAR_Success;
		}
		else
		{
			DAR = DAR_RefuseOp;
		}
	}
        
    return DAR;
}

//--------------------------------------------------
//��������:    698������ӹ�����������
//         
//����:      BYTE Type[in]    ����OI
//         
//         BYTE *pData[in]  ����ָ��
//                 
//����ֵ:     BYTE DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionAddFreezeList( WORD Type,BYTE *pData )
{
	BYTE DAR,Buf[MAX_FREEZE_ATTRIBUTE*8+30],i;		//���Ŀ����õĹ����������Ը�����54����Ӧ�����㹻�Ŀռ������ڳ��ڴ�����--ml
    WORD Result;
        
	Result = FALSE;
	DAR = DAR_WrongType;

	if( pData[0] == Array_698 )
	{
	    if( pData[1] <= MAX_FREEZE_ATTRIBUTE )//���ܳ����������������
	    {
            for( i=0; i< pData[1]; i++ )
            {
                if( pData[2+13*i] == Structure_698 )
                {
                    if( pData[3+13*i] == 3 )
                    {
                        lib_ExchangeData( &Buf[8*i], &pData[3+2+13*i],2);//��������
                        memcpy( &Buf[2+8*i], &pData[3+2+2+1+13*i],4);//OAD
                        lib_ExchangeData( &Buf[6+8*i], &pData[3+2+2+1+4+1+13*i],2);//�������
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            if( i == pData[1] )//û�д���
            {
                DAR = DAR_Success;
            }
	    }
	    else
	    {
            DAR = DAR_OverRegion;
	    }
	}

	if( DAR == DAR_Success)
	{
		Result = api_AddFreezeAttribue( Type, Buf, pData[1]);
        if( Result == FALSE )
        {
            DAR = DAR_RefuseOp;
        }
        else
        {
            DAR = DAR_Success;
        }
	}
        
    return DAR;
}

//--------------------------------------------------
//��������:    698������������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���     
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestFreeze( BYTE Ch, BYTE *pOMD)
{
	TFourByteUnion OAD;
	TTwoByteUnion OI;
	BYTE DAR,MethodNo,FreezeIndex;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	WORD Delay,Result;
	
	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��
	//Type = OI.w  -0x5000;
	
	switch ( MethodNo )
	{
		case  1://��λ
			DAR =DAR_RefuseOp;
			break;
		case  2://ִ��		
			break;
		case  3://��������
		    if( OI.w != 0x5000 )//����˲ʱ����
		    {
                DAR = DAR_RefuseOp;//�ܾ�����
		    }
		    else
		    {
    			if( pData[0] == Long_unsigned_698 )
    			{
    				lib_ExchangeData( (BYTE*)&Delay, &pData[1], 2);
					FreezeIndex = api_GetFreezeIndex( OI.w );
					Result = api_SetFreezeFlag( (eFreezeType)FreezeIndex, Delay );
    				if( Result == TRUE )//���ؽ����ȷ
    				{
    					DAR = DAR_Success;
    				}
    				else
    				{
    					DAR = DAR_HardWare;//Ϊ�̶����ز����������
    				}
    			}	
		    }
	
			break;
		case  4://���һ�������������
			DAR = ActionAddFreeze( OI.w, pData);
			break;
		case  5://ɾ��һ�������������
			if( pData[0] == OAD_698 )
			{
				memcpy(OAD.b,&pData[1],4);//ȡOAD	
				Result = api_DeleteFreezeAttribue( OI.w, eDELETE_ATT_BY_OAD,OAD.b );
				if( Result == TRUE )
				{
					DAR = DAR_Success;
				}
				else
				{
					DAR = DAR_HardWare;
				}
			}	
			break;
		case  7://������Ӷ����������
            DAR = ActionAddFreezeList( OI.w, pData );
			break;		
		case  8://ɾ�����������������
		    if( pData[0] == NULL_698 )
		    {
                Result = api_DeleteFreezeAttribue( OI.w, eDELETE_ATT_ALL,OAD.b );
                if( Result == TRUE )
                {
                    DAR = DAR_Success;
                }
                else
                {
                    DAR = DAR_HardWare;
                }
		    }

			break;
		default:
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
	return eResultChoice;	
}
//--------------------------------------------------
//��������:    ESAM���ݻس�
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pData[in]      ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������BUF���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionEsamDataRead( BYTE Ch,  BYTE *pData)//���ݻس�
{
	WORD Result;
	BYTE Buf[10];
	TTwoByteUnion Len,Leninpage;
	eAPPBufResultChoice eResultChoice;
		
	if( pData[0] == SID_698 )
	{
		Deal_SIDDate(0x00,&pData[1], &pData[1],ProtocolBuf);
		Result = api_AuthDataSID( ProtocolBuf );
		if( Result == TRUE )
		{
			Len.b[0] = ProtocolBuf[1];//��ȡ����
			Len.b[1] = ProtocolBuf[0];
			
			Leninpage.w = Deal_698DataLenth( NULL, Len.b, eUNION_OFFSET_TYPE );
			memmove( &ProtocolBuf[3+Leninpage.w],&ProtocolBuf[2],Len.w);
			ProtocolBuf[0] = DAR_Success;
			ProtocolBuf[1] = 0x01;
			ProtocolBuf[2] = Octet_string_698;
			Leninpage.w = Deal_698DataLenth( &ProtocolBuf[3], Len.b, eUNION_TYPE );
           // memcpy( &Buf[3+Leninpage.w], &ProtocolBuf[2],Len.w );
			eResultChoice = DLT698AddBuf(Ch, 0x55,ProtocolBuf ,(3+Leninpage.w+Len.w));
		}
		else
		{
			Buf[0] = DAR_EsamFial;//AR �ɹ�	
			Buf[1] = 0;
			eResultChoice = DLT698AddBuf(Ch, 0x55,Buf ,2);
		}

	}
	else
	{
		Buf[0] = DAR_WrongType;//AR �ɹ�	
		Buf[1] = 0;
		eResultChoice = DLT698AddBuf(Ch, 0x55,Buf ,2);
	}

	return eResultChoice;
}
//--------------------------------------------------
//��������:    ESAM���ݸ���
//         
//����:      BYTE *pData[in]      ����ָ��         
//         
//����ֵ:     BYTE DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionEsamDataUpdate( BYTE *pData)//���ݸ���
{
	WORD Result,OI;
	BYTE i,DAR,Type,OAD[4];
	TTwoByteUnion Len,Leninpage;
	
	Result = FALSE;
	Type = 0;
	
	if( pData[0] == Structure_698 )//�ṹ��
	{
		if( pData[2] == Octet_string_698)//oi
		{		    
			Leninpage.w = Deal_698DataLenth( &pData[3], Len.b, ePROTOCOL_TYPE);//8������
			lib_ExchangeData((BYTE *)&OI, &pData[3+Leninpage.w], 2);//��ȡoi
			memcpy( OAD, &pData[3+Leninpage.w],4 );
			
			if( OI == 0x4002)//���-- ˽Կ�²�������±��
			{
				if( api_GetRunHardFlag(eRUN_HARD_COMMON_KEY) == FALSE )
			    {
	                return DAR_PassWord;
			    }
			    
				Type = 0;
			}
			else
			{
				Type = 0x55;
			}
			
			Deal_SIDMACDate(Type, &pData[3+Leninpage.w+Len.w+1], &pData[3],ProtocolBuf);
            if( OI == 0x401B)//�����׽���
			{
				#if( PREPAY_MODE == PREPAY_LOCAL )
				for( i=0; i<4; i++ ) 
                {	
                    lib_InverseData(ProtocolBuf+9+52+3*i,3);//��������ֽڽ���˳��
                }
                if( (lib_IsAllAssignNum( ProtocolBuf+9, 0x00,24 )==FALSE) 
                    && (api_JudgeYearClock( ProtocolBuf+9+52 ) == FALSE) ) 
                {	
                    return DAR_WrongType; //����ֵ��ȫ0����4�����ݽ�����ȫ����Ч����������
                }
                for( i=0; i<4; i++ ) 
                {	
                    lib_InverseData(ProtocolBuf+9+52+3*i,3);//��������ֽڽ���˳��
                }
                #else
				return DAR_NoObject;
                #endif
			}
			//������esam��֤������֤�ŵ����жϸ�ʽ������У��˴���������Ŀ��ܻ������⣬�븴��΢ȷ��Ҳ�����---jwh
			DAR = EsamDataUpdate( OAD, (BYTE*)&pData[3+Leninpage.w+4]);
			
		}
		else
		{
			DAR = DAR_WrongType;//AR Ӳ��ʱЧ	
		}

	}
	else
	{
		DAR = DAR_WrongType;//AR Ӳ��ʱЧ	
	}

	return DAR;
}
#if( PREPAY_MODE == PREPAY_LOCAL)
//--------------------------------------------------
//��������:    Ǯ������esam��֤
//         
//����:     WORD Money[in]         ��ֵ���
//         
//        WORD BuyTimes[in]      �������
//         
//        BYTE *pCustomCode[in]  �ͻ����
//
//        BYTE *pSIDMAC[in]      ��ָ֤��
//
//����ֵ:    WORD �������
//         
//��ע����:    ��
//--------------------------------------------------
WORD api_DealMoneyDataToESAM( DWORD Money, DWORD BuyTimes, BYTE *pCustomCode, BYTE *pSIDMAC )
{
    BYTE Buf[100];
    BYTE SMOffest,SMOffest1;
    TTwoByteUnion SMLen,SMLen1;
    WORD Result;
    
    memcpy( Buf, pSIDMAC, 4);//��ǰ4���ֽ�
    SMOffest = Deal_698DataLenth( pSIDMAC+4, SMLen.b, ePROTOCOL_TYPE);
    if( SMLen.w > ( sizeof(Buf) - 4 ) )//��ֹ����Խ��
    {
    	return FALSE;
    }
    memcpy( (BYTE*)&Buf[4], pSIDMAC+4+SMOffest, SMLen.w);//����������
    
    lib_ExchangeData( (BYTE*)&Buf[4+SMLen.w], (BYTE*)&Money, 4 );//��4�ֽڽ��
    lib_ExchangeData( (BYTE*)&Buf[4+SMLen.w+4], (BYTE*)&BuyTimes, 4 );//��4�ֽڽ��
    memcpy( (BYTE*)&Buf[4+SMLen.w+4+4], pCustomCode, 6 );//��4�ֽڽ��
    
    SMOffest1 = Deal_698DataLenth( pSIDMAC+4+SMOffest+SMLen.w, SMLen1.b, ePROTOCOL_TYPE);
    memcpy( (BYTE*)&Buf[4+SMLen.w+4+4+6], pSIDMAC+4+SMOffest+SMLen.w+SMOffest1, SMLen1.w );//��֡MAC

    Result = api_AuthDataSID( Buf );
    if( Result == FALSE )
    {
        return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------
//��������:    ESAMǮ������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pData[in]      ����ָ��
//         
//����ֵ:     BYTE
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionEsamMoney(BYTE *pData)//Ǯ������
{
    BYTE Type,Offest,Offest1;
    BYTE pCustomCode[16],pMeterNo[16],*pSIDMAC;
    TTwoByteUnion Len,Len1,Len2;
    DWORD Money,BuyTimes;
    WORD DAR;

    Len.w = 0;
    Len1.w = 0;
    Len2.w = 0;
    
	if( pData[0] == Structure_698)//�ṹ��
	{
		if( pData[1] == 0x06 )//sequence of data
		{
			if( pData[2] == Integer_698 )//����
			{
			    Type = pData[3];
				if( pData[4] == Double_long_unsigned_698 )//������
				{
                    lib_ExchangeData( (BYTE *)&Money, (BYTE*)&pData[5], 4 );
                    if( pData[5+4] == Double_long_unsigned_698 )
                    {
                        lib_ExchangeData( (BYTE*)&BuyTimes, (BYTE*)&pData[5+4+1], 4 );//�������
                        if( pData[5+4+1+4] == Octet_string_698 )
                        {
                            Offest = Deal_698DataLenth( (BYTE*)&pData[5+4+1+4+1], Len.b, ePROTOCOL_TYPE);
                            if( Len.w != 6 )
                            {
                                return DAR_OverRegion;
                            }
                            
                            memcpy( pCustomCode, (BYTE*)&pData[5+4+1+4+1+Offest], Len.w );//��ȡ���

                            if( pData[5+4+1+4+1+Offest+Len.w] == SID_MAC_698 )
                            {
                                Len1.w = GetProtocolDataLen( (BYTE*)&pData[5+4+1+4+1+Offest+Len.w] );
                                pSIDMAC = (BYTE*)&pData[5+4+1+4+1+Offest+Len.w+1];
                                if( pData[5+4+1+4+1+Offest+Len.w+Len1.w] == Octet_string_698 )
                                {
                                    Offest1 = Deal_698DataLenth( (BYTE*)&pData[15+Offest+Len.w+Len1.w+1], Len2.b, ePROTOCOL_TYPE);
                                    if( Len2.w != 6 )
                                    {
                                        return DAR_OverRegion;
                                    }

                                    memcpy( pMeterNo, (BYTE*)&pData[15+Offest+Len.w+Len1.w+1+Offest1],Len2.w );
                                    DAR = api_RemoteActionMoneybag( Type, Money, BuyTimes, pCustomCode, pSIDMAC, pMeterNo);
                                    return DAR;
                                }
                            }
                        }
                    }
				}
				
			}
		}
	}
    return FALSE;
}
//--------------------------------------------------
//��������:    ESAM��ʼ������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pData[in]      ����ָ��
//         
//����ֵ:     BYTE
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionEsamIntMoney( BYTE Ch, BYTE *pData)//Ǯ������
{
    DWORD Money;
    WORD Result,SMOffest,SMOffest1,wTmpClearMeterType;
    TTwoByteUnion SMLen,SMLen1,OI;
    BYTE Buf[100];
    
    OI.w = 0XF100;
    
    if( api_GetRunHardFlag( eRUN_HARD_COMMON_KEY ) != TRUE )//˽Կ������Ǯ����ʼ��
    {
        return DAR_PassWord;
    }
    
    if( pData[0] == Structure_698 )
    {
        if( pData[1] == 2 )
        {
            if( pData[2] == Double_long_unsigned_698 )
            {
                lib_ExchangeData( (BYTE*)&Money, (BYTE*)&pData[3], 4 );//��ȡʣ����
                if( pData[3+4] == SID_MAC_698 )
                {
                    memcpy( Buf, (BYTE*)&pData[3+4+1], 4);//��ǰ4���ֽ�
                    SMOffest = Deal_698DataLenth( (BYTE*)&pData[3+4+1+4], SMLen.b, ePROTOCOL_TYPE);
                    if( SMLen.w > (sizeof(Buf) - 4) )//��ֹ����Խ��
                    {
                    	return DAR_OverRegion;
                    }
                    memcpy( (BYTE*)&Buf[4], &pData[3+4+1+4+SMOffest], SMLen.w);//����������
                    memcpy( (BYTE*)&Buf[4+SMLen.w], (BYTE*)&pData[3], 4 );//��4�ֽڽ��
                    SMOffest1 = Deal_698DataLenth( (BYTE*)&pData[3+4+1+4+SMOffest+SMLen.w], SMLen1.b, ePROTOCOL_TYPE);
                    if( SMLen1.w > ( sizeof(Buf)-4-SMLen.w-4 ) )//��ֹ����Խ��
                    {
                    	return DAR_OverRegion;
                    }
                    memcpy( (BYTE*)&Buf[4+SMLen.w+4], &pData[3+4+1+4+SMOffest+SMLen.w+SMOffest1], SMLen1.w );//��֡MAC

                    Result = api_AuthDataSID( Buf );
                    if( Result == FALSE )
                    {
                        return DAR_Symmetry;
                    }
                    
                    api_UpdataRemainMoney( eInitMoneyMode, Money, 0 );

                    //wTmpClearMeterType = 0x1111;eCLEAR_FACTORY
                    //api_WritePara(0, GET_STRUCT_ADDR(TFPara1, wClearMeterType), sizeof(WORD), (BYTE *)&wTmpClearMeterType );
                    api_SetClearFlag( eCLEAR_MONEY, 0 ); //���������־,698.45�������־ͳһ�� api_SetClearFlag wlk
                    api_WriteFreeEvent(EVENT_DATA_INIT_698,OI.w);
					api_SavePrgOperationRecord( ePRG_CLEAR_METER_NO );
                    g_byClrTimer_500ms = 2;//Ĭ����ʱ1�룬����ʵ����ʱ��Χ500~1500ms,1200bps ��������20�ֽ�ʱ��Ϊ 183ms
                }
            }
        }
    }

    return DAR_Success;
}
#endif
//--------------------------------------------------
//��������:    ESAM��Կ����
//         
//����:      BYTE *pData[in]      ����ָ��
//                  
//����ֵ:     eAPPBufResultChoice  �������BUF���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionUpdateEsamKey( BYTE *pData )//��Կ����
{	
	WORD Result;
	BYTE DAR;
	TTwoByteUnion Len,Leninpage;
	TFourByteUnion OAD;
	
	DAR = DAR_WrongType;
	OAD.d = 0x000400F1;
	
	if( pData[0] == Structure_698 )//�ṹ��
	{
		if( pData[1] == 0x02 )//sequence of data
		{
			if( pData[2] == Octet_string_698)
			{
			    api_WritePreProgramData( 0,OAD.d );
				Leninpage.w = Deal_698DataLenth( &pData[3], Len.b, ePROTOCOL_TYPE);
				Deal_SIDMACDate(0x00, &pData[3+Leninpage.w+Len.w+1],&pData[3],ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					DAR = DAR_Success;//AR �ɹ�
					api_SavePrgOperationRecord( ePRG_UPDATE_KEY_NO ); //��Կ�����¼�
					api_FlashKeyStatus();
				}
				else
				{
					DAR = DAR_EsamFial;//AR Ӳ��ʱЧ	
				}			
			}		
		}
		else
		{
			DAR = DAR_WrongType;//AR Ӳ��ʱЧ	
		}
	}
	else
	{
		DAR = DAR_WrongType;//AR Ӳ��ʱЧ	
	}
	
	return DAR;
}
//--------------------------------------------------
//��������:    ����ESAMʱЧ����
//         
//����:      BYTE *pData[in]      ����ָ��
//                  
//����ֵ:     BYTE DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionEsamConsultationTime( BYTE *pData )//����ʱЧ����
{
	WORD Result;
	BYTE DAR;
	TTwoByteUnion Len,Leninpage;
	
	DAR = DAR_WrongType;
	
	if( pData[0] == Structure_698 )//�ṹ��
	{
		if( pData[1] == 0x02 )//sequence of data
		{
			if( pData[2] == Octet_string_698)
			{
				Leninpage.w = Deal_698DataLenth( &pData[3], Len.b, ePROTOCOL_TYPE);
				Deal_SIDDate(0x55,&pData[3+Leninpage.w+Len.w+1],&pData[3],ProtocolBuf);
				Result = api_AuthDataSID( ProtocolBuf );
				if( Result == TRUE )
				{
					DAR = DAR_Success;//AR �ɹ�	
				}
				else
				{
					DAR = DAR_HardWare;//AR Ӳ��ʱЧ	
				}
			
			}
		}
		else
		{
			DAR = DAR_WrongType;//AR Ӳ��ʱЧ	
		}
	}
	else
	{
		DAR = DAR_WrongType;//AR Ӳ��ʱЧ	
    }

	return DAR;
}
//--------------------------------------------------
//��������:    ����ESAMЭ��ʧЧ
//         
//����:      BYTE Ch[in]      ͨ��ѡ��
//      
//         BYTE *pData[in]  ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������BUF��� 
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionEsamDisableConsultation( BYTE Ch, BYTE *pData)//Э��ʧЧ
{
	BYTE Buf[20];
	eAPPBufResultChoice eResultChoice;
	
	if( pData[0] == NULL_698 )//����ΪNull
    {		
		if( CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL )
		{
			api_ClrSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH );
			APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime = 1;	//1���ر�esam
			APPConnect.ConnectInfo[eConnectTerminal].ConstConnectValidTime = 1;
		}
		else
		{
			api_ClrSysStatus( eSYS_STATUS_ID_698MASTER_AUTH );
			APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime = 1;	//1���ر�esam
			APPConnect.ConnectInfo[eConnectMaster].ConstConnectValidTime = 1;
		}

		api_ClrSysStatus( eSYS_STATUS_ID_698GENERAL_AUTH );
      
        Buf[0] = DAR_Success;//AR �ɹ�    
        Buf[1] = 0x01;//choice data
        Buf[2] = DateTime_S_698;//���ص�ǰʱ��
        api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss,&Buf[3]);
        lib_InverseData( &Buf[3],  2 );
        eResultChoice = DLT698AddBuf(Ch, 0x55,Buf ,10);
    }
    else
    {
        eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_WrongType );//��Ӵ���
        if( eResultChoice != eADD_OK )
    	{
    		return eResultChoice;
    	}
        eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
    }

	return eResultChoice;
}

//--------------------------------------------------
//��������: ��������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pData[in]      ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  ���buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionIRRequest( BYTE Ch, BYTE *pData)//����ʱЧ����
{
	WORD Result;
	BYTE DAR;
	BYTE Tmp_RN[MAX_RN_SIZE+3],OutBuf[15];
	eAPPBufResultChoice eResultChoice;

	DAR = DAR_WrongType;
	//�Ͽ��ѽ�����Ӧ������
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )//�͹��Ĳ���ִ��
	{
		DAR = DAR_PassWord;
	}
	else
	{	
		DAR = DAR_Success;
		
		//if( (Ch == eIR)&&(CURR_COMM_TYPE(Ch) != COMM_TYPE_TERMINAL) )
		//{
		//	//����ͨ������IRͨ�����ͻ�����ַ��������վ
		//	api_ClrSysStatus( eSYS_IR_ALLOW_PRG ); //�������֤��־
		//}
		//else if( (Ch != eIR)&&(CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL) )
		if(CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL)
		{
			api_ClrSysStatus( eSYS_TERMINAL_ALLOW_PRG );//���ն������֤��־
		}
		else
		{
			DAR = DAR_RefuseOp;
		}

		if( DAR == DAR_Success )
		{
			//api_ResetEsamSpi( );//��λESAM
			APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = 1024;//��esam 1024��
			APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = 1024;//��esam 1024��
			
            if( pData[0] == RN_698 )//�����
            {
               if( pData[1] < MAX_RN_SIZE )
               {
                    Tmp_RN[0] = pData[1];
                    memcpy( (BYTE*)&Tmp_RN[1], (BYTE*)&pData[2], pData[1] );//��ȡ�����
                    DAR = DAR_Success;
               }
            }
            else
            {
                DAR = DAR_WrongType;//AR Ӳ��ʱЧ   
            }
            
            if( DAR == DAR_Success )
            {
				//if( Ch == eIR )
				//{
				//	Result = api_EsamIRRequest( Tmp_RN, OutBuf ); //�����ѯ ��ȡ8�ֽ������1����
				//}
				//else
				{
					Result = api_EsamTerminalRequest( Tmp_RN, OutBuf ); //�ն������֤ ��ȡ8�ֽ������1����
				}
				if( Result == FALSE )
				{
					DAR = DAR_EsamFial;
				}
				else
				{
				    DAR = DAR_Success;
				}

				api_ReadEsamRandom( 8, Tmp_RN );
				memmove( Tmp_RN, Tmp_RN+2, 8);
			}
		}
		else
		{
			DAR = DAR_RefuseOp;   
		}
	}

	if( DAR == DAR_Success )
	{
		ProtocolBuf[0] = DAR_Success;//AR �ɹ�	
		ProtocolBuf[1] = 0x01;//choice data
		ProtocolBuf[2] = Structure_698;//no data
		
		ProtocolBuf[3] = 04;

		ProtocolBuf[4] = Octet_string_698;//���
		ProtocolBuf[5] = 6;
		api_ReadEsamData(0x0001, 0x0007, 0x0006, ProtocolBuf+6);//��ȡ���(6���ֽ�) ����easmͨ�ųɹ�����Ϊesamû����

		ProtocolBuf[12] = Octet_string_698;//ESAM���к�
		ProtocolBuf[13] = 8;
		api_GetEsamInfo( 2, ProtocolBuf+14 );//8���ֽ� ����easmͨ�ųɹ�����Ϊesamû����

		ProtocolBuf[22] = Octet_string_698;//�����1����
		ProtocolBuf[23] = 8;
		memcpy(ProtocolBuf+24, OutBuf, 8);

		ProtocolBuf[32] = RN_698;//�����2
		ProtocolBuf[33] = 8;
		memcpy( ProtocolBuf+34, Tmp_RN, 8);

		eResultChoice = DLT698AddBuf(Ch, 0x55,ProtocolBuf ,42 );
	}
	else
	{
		ProtocolBuf[0] = DAR;
		ProtocolBuf[1] = 0;
		eResultChoice = DLT698AddBuf(Ch, 0x55,ProtocolBuf ,2);
	}

    return eResultChoice;
}
//--------------------------------------------------
//��������: ������֤
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pData[in]      ����ָ��
//         
//����ֵ:     byte �������DAR
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionIRAuth( BYTE Ch, BYTE *pData)//����ʱЧ����
{
    BYTE DAR,Result;
    DWORD tdw;

    DAR = DAR_WrongType;
    
    if(  api_GetSysStatus(eSYS_STATUS_POWER_ON) != FALSE )//��֧�ֺ���ͨ�� �͹��Ĳ���ִ��
    {
        if( pData[0] == Octet_string_698 )//��ȡ�����2���� 
        {
            memcpy( ProtocolBuf, (BYTE*)&pData[2], pData[1] );
            Result = api_EsamIRAuth( ProtocolBuf );
            if( Result == FALSE )
            {
                DAR = DAR_EsamFial;
            }
            else
            {
				DAR = DAR_Success;
				//if( (Ch == eIR)&&(CURR_COMM_TYPE(Ch) != COMM_TYPE_TERMINAL) )
				//{
				//	api_ReadPrePayPara( eIRTime, (BYTE*)&tdw );
				//	APPConnect.IRAuthTime = (tdw*60);//��ʱЧ���Ӹ�Ϊ��
				//	api_SetSysStatus( eSYS_IR_ALLOW_PRG );//��λ����������
				//	
				//	APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = (APPConnect.IRAuthTime+2);//��֤�ڼ䲻����esam��λ��һ�������ʱ��Ⱥ���ʱ���2��
				//	APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = (APPConnect.IRAuthTime+2);

				//	//�Ͽ���վӦ������
				//	APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime = 1;
				//	APPConnect.ConnectInfo[eConnectMaster].ConstConnectValidTime = 1;//1s��ر��ն�Э��
				//	api_ClrSysStatus( eSYS_STATUS_ID_698MASTER_AUTH );
				//}
				//else if( (Ch != eIR)&&(CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL) )
				if(CURR_COMM_TYPE(Ch) == COMM_TYPE_TERMINAL)
				{
					api_ReadPrePayPara( eIRTime, (BYTE*)&tdw );
					APPConnect.TerminalAuthTime = (tdw*60);		//��ʱЧ���Ӹ�Ϊ��
					api_SetSysStatus( eSYS_TERMINAL_ALLOW_PRG );//��λ�ն������֤�ɹ���־

					APPConnect.ConnectInfo[eConnectGeneral].ConnectValidTime = (APPConnect.TerminalAuthTime+2);//��֤�ڼ䲻����esam��λ��һ�������ʱ��Ⱥ���ʱ���2��
					APPConnect.ConnectInfo[eConnectGeneral].ConstConnectValidTime = (APPConnect.TerminalAuthTime+2);

					//�Ͽ��ն�Ӧ������
					APPConnect.ConnectInfo[eConnectTerminal].ConnectValidTime = 1;		//�ο� Э��ʧЧESAM�ӿ��� ����5
					APPConnect.ConnectInfo[eConnectTerminal].ConstConnectValidTime = 1;
					api_ClrSysStatus(eSYS_STATUS_ID_698TERMINAL_AUTH);
					
				}
				else
				{
					DAR = DAR_RefuseOp;
				}
            }
        }
    }
    else
    {
        DAR = DAR_RefuseOp;
    }

    return DAR;
}

//-----------------------------------------------
//��������  :    ����ESAM
//����:  
//           BYTE Ch[in]          ͨ��ѡ��   
//
//           BYTE *pOMD[in]       ����ָ��
//
//����ֵ:       eAPPBufResultChoice  �������Buf���
//
//��ע����  :    �����ݷ��ص�ֱ��return
//-----------------------------------------------
eAPPBufResultChoice ActionRequestEsam( BYTE Ch,  BYTE *pOMD)
{
	BYTE DAR;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	pData = &pOMD[4];
	DAR = DAR_WrongType;//Ĭ��ʧ��
	
	switch( pOMD[2] )
	{
	case 0x03://���ݻس�
		eResultChoice = ActionEsamDataRead( Ch, pData );
		return eResultChoice;
		break;
	case 0x04://���ݸ���
		DAR = ActionEsamDataUpdate( pData );
		break;
	case 0x05://Э��ʧЧ
		eResultChoice = ActionEsamDisableConsultation( Ch, pData );
		return eResultChoice;
		break;
	#if( PREPAY_MODE == PREPAY_LOCAL)
	case 0x06://Ǯ������---δ��
		DAR = ActionEsamMoney( pData );
		break;
	#endif
	case 0x07://��Կ����
		DAR = ActionUpdateEsamKey( pData );
		break;
	case 0x08://֤�����		
		break;
	case 0x09://����ESAM ����
		DAR = ActionEsamConsultationTime( pData );
		if( DAR == DAR_Success )
		{
			api_FlashIdentAuthFlag();   //ˢ�������֤Ȩ�ޱ�־
		}
		break;
	#if( PREPAY_MODE == PREPAY_LOCAL)	
	case 0x0A://Ǯ����ʼ��
	    DAR = ActionEsamIntMoney( Ch, pData );
		break;	
	#endif
	case 0x0B://������֤����
	    eResultChoice = ActionIRRequest( Ch, pData );
	    return eResultChoice;
		break; 
	case 0x0C://������ָ֤��
	    DAR = ActionIRAuth(Ch, pData );
		break;
	default:		
		break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
    return eResultChoice;
}
//--------------------------------------------------
//��������:    �㲥Уʱ
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���      
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestTimeBroadCast( BYTE Ch,  BYTE *pOMD)
{
	BYTE DAR;
	eAPPBufResultChoice eResultChoice;
	DWORD AbsMeterSecTime,AbsSetSecTime,AbsZeroHourSecTime;
	BYTE *pData;
	TRealTimer TmpRealTimer;
	TTimeBroadCastPara tmpTimeBroadCastPara;

	pData = &pOMD[4];//ǿ�ƽ������ݷ���,��Ĭ������
    DAR = DAR_WrongType;

	switch( pOMD[2] )//Method )
	{
		case 127://�㲥Уʱ
			if( pData[0] != DateTime_S_698 )
			{
				break;
			}
			
			if( (api_GetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG ) == TRUE)
			&&(APPData[Ch].eSafeMode == eNO_SECURITY) )//���Ĺ㲥Уʱһ��һ��
			{
			    DAR = DAR_TempFail;
				break;
			}
			
			api_GetRtcDateTime(DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpRealTimer );//��ȡ��ǰʱ��
			AbsMeterSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//���㵱ǰʱ���������

			TmpRealTimer.Hour = 0;//��ȡ�������
			TmpRealTimer.Min = 0;
			TmpRealTimer.Sec = 0;
			AbsZeroHourSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//���㵱ǰʱ���������
			
			lib_InverseData(&pData[1], 2);
			memcpy((BYTE*)&(TmpRealTimer.wYear),&pData[1],sizeof( TRealTimer ));//��ȡ�㲥Уʱʱ��
			AbsSetSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//���㵱ǰʱ���������
			//���·���ʱ����кϷ�ֵ�жϣ����Ϸ���ʱ�䲻����Уʱ
			if(AbsSetSecTime == 0xffffffff)
			{
				DAR = DAR_OverRegion;
				break;
			}
			if( (api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG)==FALSE) && (api_GetRunHardFlag(eRUN_HARD_CLOCK_BAT_LOW)==FALSE) )	//ʱ�������͵�����������ж�
			{
				//��ȡ�㲥Уʱ����
				if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.TimeBroadCastPara), sizeof(TTimeBroadCastPara), (BYTE*)&tmpTimeBroadCastPara ) == FALSE )
				{
					tmpTimeBroadCastPara.TimeMinLimit = TimeBroadCastTimeMinLimit; //��Ĭ��ֵ
					tmpTimeBroadCastPara.TimeMaxLimit = TimeBroadCastTimeMaxLimit; 
				}
				
				if( APPData[Ch].eSafeMode == eSECRET_MAC )
				{
					//С�ڹ㲥Уʱ��ֵ ����Уʱ
					if( labs(AbsMeterSecTime-AbsSetSecTime) < tmpTimeBroadCastPara.TimeMinLimit )
					{
					    DAR = DAR_OverRegion;
						break;
					}
				}
				else if( APPData[Ch].eSafeMode == eNO_SECURITY )//����ģʽ
				{
					//С��Уʱ��Сʱ�����Ʋ���Уʱ
					if( labs(AbsMeterSecTime-AbsSetSecTime) < tmpTimeBroadCastPara.TimeMinLimit )
					{
						DAR = DAR_OverRegion;
						break;
					}
					//С��-�������ʱ�����ƿ���Уʱ
					if( labs(AbsMeterSecTime-AbsSetSecTime) > tmpTimeBroadCastPara.TimeMaxLimit )
					{
					    if( api_GetRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG ) == FALSE )
						{
							//��RTC�����־
							api_SetRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG);
							//�����Ĺ㲥Уʱ���ϴ����־
							api_SetRunHardFlag( eRUN_HARD_ALREADY_PLAINTEXT_BROADCAST_ERR_FLAG );
						}
						DAR = DAR_OverRegion;
						break;
					}
					//���Ĺ㲥Уʱ���������
					if ((AbsSetSecTime<AbsZeroHourSecTime)||(AbsSetSecTime>=(AbsZeroHourSecTime+24*60*60)))
					{
						DAR = DAR_OverRegion;
						break;
					}
					//���ܿ������Уʱ
					if( JudgeBroadcastTime( AbsSetSecTime ) == FALSE )
					{
					    DAR = DAR_OverRegion;
						break;
					}
					
				}
				else
				{
					DAR = DAR_RefuseOp;
					break;
				}
				api_WritePreProgramData( 0, 0x00020040 );//ʱ��
				if( api_WriteMeterTime(&TmpRealTimer) == TRUE )
				{
					#if( SEL_DEMAND_2022 == NO )
				    #if( MAX_PHASE != 1)		
            		api_InitDemand( );//���óɹ������½��������ļ���
            		#endif
            		#endif
            		if( APPData[Ch].eSafeMode == eNO_SECURITY )//���Ĺ㲥Уʱһ��һ��
            		{
						api_SetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
            		}
					api_SavePrgOperationRecord( ePRG_BROADJUST_TIME_NO );					
					//��¼698�㲥Уʱ�����¼�
					api_WriteFreeEvent(EVENT_BROADCAST_WRITE_TIME, 0x0698);
					DAR = DAR_Success;
				}
			}	
			else
			{
				api_WritePreProgramData( 0, 0x00020040 );  //ʱ��
				if( api_WriteMeterTime(&TmpRealTimer) == TRUE )
				{
					#if( SEL_DEMAND_2022 == NO )
				    #if( MAX_PHASE != 1)		
            		api_InitDemand( );//���óɹ������½��������ļ���
            		#endif
            		#endif
					//����㲥Уʱ�¼���¼
					api_SavePrgOperationRecord( ePRG_BROADJUST_TIME_NO );
					if( APPData[Ch].eSafeMode == eNO_SECURITY )//���Ĺ㲥Уʱһ��һ��
					{
						api_SetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
					}
					//��¼698�㲥Уʱ�����¼�
					api_WriteFreeEvent(EVENT_BROADCAST_WRITE_TIME, 0x0698);
					DAR = DAR_Success;
				}
			}
		
			break;		
		default:
			break;
	}
	if( LinkData[Ch].AddressType == eBROADCAST_ADDRESS_MODE )//����ǹ㲥��ַ����Ӧ��
    {
    	return eAPP_NO_RESPONSE;
    }
    else
    {
		if( DAR == DAR_Success )
		{
			eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR_Success );//���DAR
		}
		else
		{
			eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR );//���DAR
		}

		eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
    	return eResultChoice;
    }	
}
//--------------------------------------------------
//��������:    �����˿�
//         
//����:      
//			Mode[in] 0:����485	1�������ز�
//			BYTE 	*pData[in]      ����ָ��
//                  
//����ֵ:     	BYTE DAR���
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionCOMRate( BYTE Mode, BYTE *pData )//�����˿ڲ�����
{	
	TFourByteUnion OAD;
	BYTE TmpCommPara0,DAR;
	WORD Result;
	BYTE Channel;
	
	DAR = DAR_ChangeBaud;//DAR_WrongType;

	if( pData[0] == Structure_698 )
	{
		if( (pData[1] == 3) ||( pData[1] == 2 ))
		{
			if( pData[2] == OAD_698 )
			{
				lib_ExchangeData( OAD.b, &pData[3], 4);
				if( Mode == 0 )
				{
					if( (OAD.d == 0xf2010200) || (OAD.d == 0xf2010201) || (OAD.d == 0xf2010202))
					{
						if( OAD.d == 0xf2010202 )
						{
							Channel = 2;
							
						}
						else
						{
							Channel = 0;
						}
						
						if( pData[7] == COMDCB_698 )
						{
							if( (pData[8] < 11)&&(pData[8] != 5)&&(pData[8] > 1) )//bit0~bit2 ��ʾ������ֻ�ܵ�7
							{
								//1200bps��2����2400bps��3����4800bps��4����7200bps��5����9600bps��6����19200bps��7����
								TmpCommPara0 = pData[8];//��ʾ������
	
								if( pData[9] < 3)//У��λ 
								{
									TmpCommPara0 |= (pData[9] <<  5);
	
									if( pData[10] == 8 )//ֻ֧������λΪ8
									{
										if( (pData[11] == 1) || (pData[11] == 2) )//֧��ֹͣλΪ1��2
										{
											if( pData[11] == 1 )
											{
												TmpCommPara0 &= (~BIT4);
											}
											else
											{
												TmpCommPara0 |= BIT4;
											}
											
											if( pData[12] < 3 )//����
											{
												TmpCommPara0 |= (pData[12] << 7);
												Result = api_WritePara(1, (GET_STRUCT_ADDR(TFPara2, CommPara.I485)+Channel), 1, &TmpCommPara0);
												if( Result == FALSE )
												{
													DAR = DAR_HardWare;//��Ϊ�̶����أ������������ ���ϲ�������
												}
												else
												{
													if( Channel == eRS485_I )
													{
														Serial[eRS485_I].OperationFlag = 1;
													}
													else
													{
														//#if(MAX_COM_CHANNEL_NUM == 4 )//4��ͨ��ʱ֧��485-2
														//Serial[eRS485_II].OperationFlag = 1;
														//#endif
													}
													
													DAR = DAR_Success;
												}
											}
										}
									}	
								}
							}
						}
					}
				}
				else
				{
					if(( OAD.d == 0xf20902FD ) || ( OAD.d == 0xf2090201 ))
					{
						if( pData[7] == COMDCB_698 )
						{
							if( (pData[8] < 11)&&(pData[8] != 5)&&(pData[8] > 1) )//bit0~bit2 ��ʾ������ֻ�ܵ�10
							{
								
								TmpCommPara0 = (FPara2->CommPara.ComModule & 0xF0);//����ֵ������
								//1200bps��2����2400bps��3����4800bps��4����7200bps��5����9600bps��6����19200bps��7����
								TmpCommPara0 |= (pData[8]&0x0F);//��ʾ������
								if( TmpCommPara0 != FPara2->CommPara.ComModule )//Э�̲������
								{
									Result = api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.ComModule), 1, &TmpCommPara0);
									if( Result == FALSE )
									{
										DAR = DAR_HardWare;//��Ϊ�̶����أ������������ ���ϲ�������
									}
									else
									{
										Serial[eCR].OperationFlag = 1;
										DAR = DAR_Success;
									}
								}
								else
								{
									DAR = DAR_Success;
								}
							}
						}
					}
				}
			}
		}
	}

	return DAR;
}
//--------------------------------------------------
//��������:    ������������豸�ӿ���
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���   
//         
//��ע����:    �� 
//--------------------------------------------------
eAPPBufResultChoice ActionRequestDevice( BYTE Ch,  BYTE *pOMD)
{	
	TTwoByteUnion OI;
	TFourByteUnion OAD;
	BYTE DAR,MethodNo,Mode,i;
	BYTE *pData,CanNode[1];
	WORD Result, Offset = 0;
	eAPPBufResultChoice eResultChoice;
	// TSafeMem_dev_work dev_work;

	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��
	Result = FALSE;
	
	switch( OI.w )
	{
		case 0xf201://RS485	
		case 0xf209://�ز�
			if( OI.w == 0xf201 )
			{
				if( MethodNo != 127 )
				{
					break;
				}
				Mode = 0;
			}
			else
			{
				if(MethodNo != 128)
				{
					break;
				}
				Mode  = 1;
			}
			
			DAR = ActionCOMRate( Mode,pData );
			break;
		case 0xf205://�̵������
			if( MethodNo != 127 )
			{
				break;
			}

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 2 )
				{
					if( pData[2] == OAD_698 )//�̵������
					{
						lib_ExchangeData( OAD.b, &pData[3], 4);
						if( (OAD.d == 0xf2050200) || (OAD.d == 0xf2050201))
						{
							if( pData[7] == Enum_698 )//��������
							{
								if( pData[8] < 2 )
								{
									Result = WriteRelayPara( 3, &pData[8]);
									if( Result == FALSE )
									{
										DAR = DAR_HardWare;
									}
									else
									{
										DAR = DAR_Success;
									}
									
								}
							}
						}
					}
				}
			}
			break;
		case 0xf207://�๦�ܶ���
			if( MethodNo != 127 )
			{
				break;
			}

			if( pData[0] == Structure_698 )
			{
				if( pData[1] == 2 )
				{
					if( pData[2] == OAD_698 )//�๦�ܶ���
					{
						lib_ExchangeData( OAD.b, &pData[3], 4);
						if( (OAD.d == 0xf2070200) || (OAD.d == 0xf2070201))
						{
							if( pData[7] == Enum_698 )//����ģʽ
							{
							    // �๦�ܶ��ӳ�ʼ��
							    if( api_MultiFunPortSet(pData[8]))
							    {
								    DAR = DAR_Success;
							    }
							    else
							    {
								    DAR = DAR_RefuseOp;
							    }
						    }
						}
					}
				}
			}
			break;
		case 0xF20B:
			// if(MethodNo == 3)		//�˿���Ȩ����
			// {
			// 	if((pData[Offset++] == Structure_698) && (pData[Offset++] == 2))
			// 	{
			// 		if(pData[Offset++] == Unsigned_698)
			// 		{
			// 			pData[Offset++];	//�˿����

			// 			if(pData[Offset++] == Unsigned_698)
			// 			{
			// 				//pData[Offset];	//��Ȩ��ʼʱ��
			// 				DAR = DAR_Success;
			// 			}
			// 		}
			// 	}
			// }
			// else if(MethodNo == 129)	//�����������
			// {
			// 	DAR = DAR_Success;
			// 	if((pData[Offset++] == Structure_698) && (pData[Offset++] == 2))
			// 	{
			// 		if(pData[Offset++] == OAD_698)
			// 		{
			// 			//pData[Offset];	//�˿ں�OAD

			// 			Offset += 4;
			// 			if(pData[Offset++] == Visible_string_698)
			// 			{
			// 				Offset += Deal_698DataLenth( &pData[Offset], (BYTE *)&Result, ePROTOCOL_TYPE);
			// 				if ((Result!=6)&&(Result!=0))
			// 				{
			// 					DAR = DAR_HardWare;
			// 				}
			// 				else
			// 				{
			// 					if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work), sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
			// 					{
			// 						DAR = DAR_HardWare;
			// 					}
			// 					else
			// 					{
			// 						for (i = 0; i < Result; i++)
			// 						{
			// 							if (pData[Offset + i] > 9)
			// 							{
			// 								DAR = DAR_HardWare;
			// 								break;
			// 							}
			// 							pData[Offset + i] += 0x30; // ת��ΪASCII�洢
			// 						}
			// 						if (DAR == DAR_Success)
			// 						{
			// 							memcpy(&dev_work.Mac, &pData[Offset], Result);
			// 							dev_work.MacLen = Result;
			// 							if (api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SafeMem_dev_work), sizeof(TSafeMem_dev_work), (BYTE *)&dev_work) == FALSE)
			// 							{
			// 								DAR = DAR_HardWare;
			// 							}
			// 							else
			// 							{
			// 								api_delayinit_ble();
			// 							}
										
			// 						}
			// 					}
			// 				}
			// 			}
			// 		}
			// 	}
			// }
			break;
		case 0xF221:
			Result=FALSE;
			if(MethodNo == 127)
			{
				if(pData[0]==Structure_698)
				{
					if(pData[1]==2)
					{
						if( pData[2] == OAD_698 )
						{
							if( pData[7] == COMDCB_698 )
							{
								if(pData[8]<17 && pData[8]>12)
								{
									Result = api_WritePara(1, (GET_STRUCT_ADDR(TFPara2, CommPara.CanBaud)), 1, &pData[8]);
									if( Result == FALSE )
									{
										DAR = DAR_HardWare;//��Ϊ�̶����أ������������ ���ϲ�������
									}
									else
									{
//										Serial[eCAN].OperationFlag = 1;
										DAR = DAR_Success;
									}
								}
								else
								{
									DAR = DAR_ChangeBaud;
									eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
									if( eResultChoice != eADD_OK )
									{
										return eResultChoice;
									}
									eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
									
									return eResultChoice;
								}
								
							}
						}
					}
				}
			}
			else if (MethodNo==128)
			{
				if (pData[0]==Structure_698)
				{
					if (pData[2]==OAD_698)
					{
						if (pData[7]==Array_698)
						{
						    if(pData[8] > MAX_CANBUS_ID_NUM)
						    {
							    Result = FALSE;
						    }
						    else
						    {
							    for(i = 0; i < pData[8]; i++)
							    {
								    // ��ӽڵ�
								    if(pData[9 + i * 2] == Unsigned_698)
								    {
									    CanNode[i] = pData[10 + 2 * i];
								    }
							    }
//							    Result = ChangeID(eADDID, CanNode, pData[8]);
							    if(Result == FALSE)
							    {
								    break;
							    }
						    }
					    }
				    }
			    }
				
			}
			else if (MethodNo==129)
			{
				if (pData[0]==Structure_698)
				{
					if (pData[2]==OAD_698)
					{
					    if(pData[8] > MAX_CANBUS_ID_NUM)
					    {
						    Result = FALSE;
					    }
					    else
					    {
						    for(i = 0; i < pData[8]; i++)
						    {
							    // ��ӽڵ�
							    if(pData[9 + i * 2] == Unsigned_698)
							    {
								    CanNode[i] = pData[10 + 2 * i];
							    }
						    }
						    Result = ChangeID(eDELID, CanNode, pData[8]);
						    if(Result == FALSE)
						    {
							    break;
						    }
					    }
				    }
			    }
				
			}
			else if (MethodNo==130)
			{
				if (pData[0]==Structure_698)
				{
					if (pData[2]==OAD_698)
					{
//						Result|=ChangeID(eCLEANID,NULL,0);
					}
					
				}
				
			}
			if (Result==TRUE)
			{
				DAR = DAR_Success;
			}
			else
			{
				DAR = DAR_RefuseOp;
			}
			
			
		default:
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
    return eResultChoice;
}
//--------------------------------------------------
//��������:    ����ESAM��ȫģʽ
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       OMDָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���       
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestEsamSafeMode( BYTE Ch,  BYTE *pOMD)//����esam��ȫģʽ
{
	BYTE DAR,i;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;

	pData = &pOMD[4];//ȡ����ָ��
	DAR = DAR_WrongType;
	
	switch( pOMD[2] )
	{
	case 0x01://��λ
	    if( pData[0] == Integer_698 )//����ΪInteger_698
	    {
            api_ClrProSafeModePara(2);
            DAR= DAR_Success;//AR �ɹ�
	    }
		break;
	case 127://����127��������ʽ��ȫģʽ�����������ʶ��Ȩ�ޣ�,����������Ҫ��һ���ṹ��װ�´�
		if( pData[0] == Structure_698 )
		{
			AlterSafeModePara(0, (void *)&(pData[2]) );//����Ļ���� OI_698 ��ʼ
			DAR= DAR_Success;//AR �ɹ�	
		}
		break;
	case 128://ɾ����ʾ��ȫ����
		if( pData[0] == OI_698)
		{
			AlterSafeModePara(1, (void *)&(pData[0]) );//����Ļ���� OI_698 ��ʼ
			DAR = DAR_Success;//AR �ɹ�	
		}
		break;	
	case 129://����������ʾ��ȫ����
		if( pData[0] == Array_698 )
		{
			if( pData[1] <= MAX_SAFE_MODE_PARA_NUM )
			{
				for( i=0; i<pData[1]; i++ )
				{
					if( pData[4+8*i] == OI_698 )
					{
						AlterSafeModePara(0, (void *)&(pData[4+8*i]) );//����Ļ���� OI_698 ��ʼ
					}
					else
					{
						DAR = DAR_WrongType;
						break;
					}

				}

				if( i == pData[1] )
				{
					DAR = DAR_Success;//AR �ɹ�	
				}
			}
		}
		break;	
	default:		
		break;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 

    return eResultChoice;
}

//--------------------------------------------------
//��������: �����ļ���Ϣ
//
//����:	Ch[i]:
//		pData[in]: 698�ļ��ṹ
//����ֵ: ���Ľ�������
//
//��ע:
//--------------------------------------------------
static WORD DealFileInfo(BYTE Ch, BYTE *Buf)
{
	WORD Offset = 0, Ret = 0x8000;
	WORD Len, LenOffset;
	BYTE *pDat;

	if( (Buf[Offset++] == Structure_698) && (Buf[Offset++] == 6))
	{
		if ((Buf[Offset++] == Structure_698) && (Buf[Offset++] == 6))
		{
			if (Buf[Offset] == Visible_string_698) // Դ�ļ�
			{
                Offset++;
				LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
				pDat = &Buf[Offset + LenOffset];

				Offset = Offset + LenOffset + Len;
			}
			else if (Buf[Offset] == NULL_698)
			{
				Offset++;
			}
			else
			{
				return 0x8000;
			}

			if (Buf[Offset] == Visible_string_698) // Ŀ���ļ�
			{
                Offset++;
				LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
				pDat = &Buf[Offset + LenOffset];

				Offset = Offset + LenOffset + Len;
			}
			else if (Buf[Offset] == NULL_698)
			{
				Offset++;
			}
			else
			{
				return 0x8000;
			}
		}
		else
		{
			return 0x8000;
		}

		if(Buf[Offset++] == Double_long_unsigned_698) //�ļ���С
		{
			lib_ExchangeData((BYTE *)&tIap.dwFileSize, (BYTE *)&Buf[Offset], 4);

			if( tIap.dwFileSize == 0 )
			{
				return 0x8000;
			}
			Offset += 4;
		}
		else
		{
			return 0x8000;
		}

		if(Buf[Offset++] == Bit_string_698)	//�ļ�����
		{
			LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
			tIap.bFileAttr = Buf[Offset + LenOffset];
			Len = (Len + 7) / 8;	//BIT String 8Bit һ���ֽڣ�

			Offset = Offset + LenOffset + Len;
		}
		else
		{
			return 0x8000;
		}

		if(Buf[Offset++] == Visible_string_698)	//�ļ��汾
		{
			LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
			pDat = &Buf[Offset + LenOffset];

			memcpy((BYTE *)&tIap.bFileVer[0], pDat, Len);
			Offset = Offset + LenOffset + Len;
		}
		else
		{
			return 0x8000;
		}

		if(Buf[Offset++] == Enum_698)	//�ļ����
		{
			tIap.bFileType = Buf[Offset++];
		}
		else
		{
			return 0x8000;
		}
	}
	else
	{
		return 0x8000;
	}

	return Offset;
}
//--------------------------------------------------
//��������: ������������
//
//����: Ch[in]: 
//		Buf[in]:
//����ֵ: DAR 
//
//��ע:
//--------------------------------------------------
BYTE ActionStartTransfe(BYTE Ch, BYTE *Buf)
{
	DWORD dwTemp;
	WORD Offset, Len, LenOffset;
	BYTE *pDat, i, Num;

	memset( (BYTE *)&tIap, 0x00, sizeof(T_Iap));
	memset((BYTE *)&tIapInfo, 0x00, sizeof(TIapInfo));
	memset((BYTE *)&IapStatus[0], 0x00, sizeof(IapStatus));

	Offset = DealFileInfo(Ch, Buf);	//�ļ���Ϣ
	if(Offset == 0x8000)
	{
		return DAR_WrongType;
	}

	if((Buf[Offset] == Long_unsigned_698)||(Buf[Offset] == Double_long_unsigned_698))	//�����ļ����С
	{
		if ((Buf[Offset] == Double_long_unsigned_698))
		{
            Offset++;
			lib_ExchangeData((BYTE *)&dwTemp, (BYTE *)&Buf[Offset], sizeof(DWORD));
			if (dwTemp>0xffff)
			{
				return DAR_WrongType;
			}
			Offset += 2;
		}
        else
        {
            Offset++;
        }
		lib_ExchangeData((BYTE *)&tIap.wFrameSize, (BYTE *)&Buf[Offset], sizeof(WORD));
		Offset += 2;
		if((tIap.wFrameSize != 0) && (LEN_OF_ONE_FRAME <= tIap.wFrameSize) && (SECTOR_SIZE >= tIap.wFrameSize))
		{
			tIap.wAllFrameNo = tIap.dwFileSize / tIap.wFrameSize;
			if(tIap.dwFileSize % tIap.wFrameSize)
			{
				tIap.wAllFrameNo++;
			}

			tIap.wFrameOfOneSector = SECTOR_SIZE / tIap.wFrameSize;

			// ���ļ���С��֡��С�������Ƿ���ȫ������
			dwTemp = sizeof(IapFacCode) + FLASH_APP_END_ADDR_ROM1 - FLASH_APP_START_ADDR;
			// ��ֹԽ��
			if(ceil(1.0 * tIap.dwFileSize / tIap.wFrameSize) / (SECTOR_SIZE / tIap.wFrameSize) > SECTOR_NUM_FOR_IAP)
			{
				return DAR_WrongType;
			}

			if(dwTemp < tIap.dwFileSize)   // ����������
			{
				tIap.wAllUpdate = IAP_ALL;
			}
			else
			{
				tIap.wAllUpdate = 0;
			}
		}
		else
		{
			return DAR_WrongType;
		}
	}
	else
	{
		return DAR_WrongType;
	}

	if( (Buf[Offset++] == Structure_698) && ((Buf[Offset++] == 2)))	//У��
	{
		if(Buf[Offset++] == Enum_698)	//У������
		{
			pDat = &Buf[Offset++];

			if(*pDat == 0 )
			{
				if (Buf[Offset++] == Octet_string_698) // У��ֵ
				{
					LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
					pDat = &Buf[Offset + LenOffset];

					lib_ExchangeData((BYTE *)&tIap.wCrc16, pDat, sizeof(WORD));

					Offset = Offset + LenOffset + Len;
				}
			}
			else
			{
				return DAR_WrongType;
			}


		}
		else
		{
			return DAR_WrongType;
		}
	}
	else
	{	
		return DAR_WrongType;
	}

	if(Buf[Offset] == Array_698)	//���ݵ�����汾��
	{
		Offset++;
		Num = Buf[Offset++];

		for (i = 0; i < Num; i++)
		{
			if(Buf[Offset++] == Visible_string_698)
			{
				LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
				pDat = &Buf[Offset + LenOffset];
				if(Len > 16)	//SIZE 0 - 16
				{
					return DAR_OverRegion;
				}

				Offset = Offset + LenOffset + Len;
			}			
		}
	}
	else if (Buf[Offset] == NULL_698)
	{
		Offset++;
	}
	else
	{	
		return DAR_WrongType;
	}

	if(Buf[Offset] == Array_698)	//���ݵ�Ӳ���汾��
	{
		Offset++;
		Num = Buf[Offset++];

		for (i = 0; i < Num; i++)
		{
			if(Buf[Offset++] == Visible_string_698)
			{
				LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
				pDat = &Buf[Offset + LenOffset];
				if(Len > 16)	//SIZE 0 - 16
				{
					return DAR_OverRegion;
				}

				Offset = Offset + LenOffset + Len;
			}
		}
	}
	else if (Buf[Offset] == NULL_698)
	{
		Offset++;
	}
	else
	{
		return DAR_WrongType;
	}

	if(Buf[Offset] == Visible_string_698)		//���ط���ʶ
	{
		Offset++;
		LenOffset = Deal_698DataLenth( &Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
		pDat = &Buf[Offset + LenOffset];
		if(Len > 32)	//SIZE 2 -32
		{
			return DAR_OverRegion;
		}

		Offset = Offset + LenOffset + Len;
	}
	else if (Buf[Offset] == NULL_698)
	{
		Offset++;
	}
	tIapInfo.dwFlag = IAP_START; // �������������������ʽ��ȷ

	return DAR_Success;
}

//--------------------------------------------------
//��������: д�ļ�
//
//����: Ch[in]: 
//		Buf[in]:
//����ֵ: DAR 
//
//��ע:
//--------------------------------------------------
BYTE ActionBlockTransfe(BYTE Ch, BYTE *Buf)
{
	DWORD dwAddr, i;
	WORD Offset = 0, Len, LenOffset, wTemp, wFlag;
	BOOL boResult;
	BYTE tmp[SECTOR_SIZE];
	
	if((Buf[Offset++] == Structure_698) && (Buf[Offset++] == 2))
	{
		if(Buf[Offset++] == Long_unsigned_698)   // �����
		{
			lib_ExchangeData((BYTE *)&tIapInfo.wCurFrameNo, &Buf[Offset], sizeof(WORD));

			if(tIapInfo.wCurFrameNo >= (MAX_SIZE_FOR_IAP / tIap.wFrameSize))
			{
				return DAR_BlockSeq;   // ���ݿ���Ŵ���
			}
			if(IapStatus[tIapInfo.wCurFrameNo / 8] & (0x01 << (tIapInfo.wCurFrameNo % 8)))   // ֡�����־�����
			{
				return DAR_Success;
				//return DAR_BlockSeq;   // ���ݿ���Ŵ���
			}

			Offset += 2;
			if(Buf[Offset++] == Octet_string_698)   // ������
			{
				LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);

				boResult = FALSE;

				if(tIapInfo.wCurFrameNo == 0)   // ��һ֡Ϊȷ��֡����ֹ�յ�����˼�ٵĲ�Ʒ��������
				{
					if(memcmp(&Buf[Offset + LenOffset], &IapFacCode[0], sizeof(IapFacCode)) == 0)
					{
						tIapInfo.wFacFlag = 1;
					}
				}
				if(tIapInfo.wFacFlag == 0)
				{
					return DAR_RefuseOp;   // ���������˼�ٵ��������������
				}
				for(i = 0; i < tIap.wFrameOfOneSector; i++)
				{
					if(IapStatus[(((tIapInfo.wCurFrameNo / tIap.wFrameOfOneSector) * tIap.wFrameOfOneSector) + i) / 8]
					   & (0x01 << (((tIapInfo.wCurFrameNo / tIap.wFrameOfOneSector) * tIap.wFrameOfOneSector) + i) % 8))
					{
						break;
					}
				}
				if(i == tIap.wFrameOfOneSector)
				{
					api_UpdateEraseFlash(IAP_CODE_ADDR + ((tIapInfo.wCurFrameNo) / tIap.wFrameOfOneSector) * SECTOR_SIZE);
				}
				// �ڶ�֡��ʼ�����IAP_CODE_ADDR��ʼ��flash����
				dwAddr = IAP_CODE_ADDR + ((tIapInfo.wCurFrameNo) / tIap.wFrameOfOneSector) * SECTOR_SIZE + ((tIapInfo.wCurFrameNo) % tIap.wFrameOfOneSector) * tIap.wFrameSize;
				boResult = api_WriteFlashDataUpDate(dwAddr, Len, &Buf[Offset + LenOffset]);
				if(boResult == FALSE)
				{
					api_ReadMemRecordData((IAP_CODE_ADDR + ((tIapInfo.wCurFrameNo) / tIap.wFrameOfOneSector) * SECTOR_SIZE), SECTOR_SIZE, tmp);
					memcpy(tmp + (((tIapInfo.wCurFrameNo) % tIap.wFrameOfOneSector) * tIap.wFrameSize), &Buf[Offset + LenOffset], tIap.wFrameSize);
					boResult = api_WriteMemRecordData((IAP_CODE_ADDR + ((tIapInfo.wCurFrameNo) / tIap.wFrameOfOneSector) * SECTOR_SIZE), SECTOR_SIZE, tmp);
				}

				if(boResult == TRUE)
				{
					IapStatus[tIapInfo.wCurFrameNo / 8] |= 0x01 << (tIapInfo.wCurFrameNo % 8);   // ��֡�����־
					for(wTemp = 0; wTemp < tIap.wAllFrameNo; wTemp++)
					{
						if((IapStatus[wTemp / 8] & (0x01 << (wTemp % 8))) == 0)
						{
							break;
						}
					}
					if(wTemp >= tIap.wAllFrameNo)
					{
						// �㲥��ʽ��֡����ȫ������Ը�λִ������
						if(LinkData[Ch].AddressType == eBROADCAST_ADDRESS_MODE)
						{
							tIap.dwIapFlag = IAP_FLAG;
							tIap.dwCrc32 = lib_CheckCRC32((BYTE *)&tIap.bDestFile[0], sizeof(tIap) - sizeof(DWORD));   // ����У��

							boResult = api_WriteMemRecordData(IAP_INFO_ADDR, sizeof(tIap), (BYTE *)&tIap.bDestFile[0]);
							if(boResult == TRUE)
							{
								// ��ʱ1s��׼������
								tIapInfo.dwFlag = IAP_READY;
								tIapInfo.dwTime = 1;
							}
						}
					}
					return DAR_Success;
				}
				else
				{
					return DAR_WrongType;
				}
			}
		}
	}

	return DAR_WrongType;
}
//--------------------------------------------------
// ��������:	��������ʱ
//
// ����:	
// ����ֵ: 
//
// ��ע:
//--------------------------------------------------
void api_IapCountDown( void )
{
	if (tIapInfo.dwTime)
	{
		tIapInfo.dwTime--;
		if (tIapInfo.dwTime == 0)
		{
			if (tIapInfo.dwFlag == IAP_READY)
			{
				tIapInfo.dwFlag = 0;
				 
				// api_PowerDownEnergy();
				Reset_CPU();
				// api_EnterLowPower(eFromOnRunEnterDownMode);
			}
		}
	}
}
//--------------------------------------------------
//��������:	ִ������
//
//����:	Ch[in]: 
//		Buf[in]:
//����ֵ: DAR
//
//��ע:
//--------------------------------------------------
BYTE ActionExeUpdate(BYTE Ch, BYTE *Buf)
{
	WORD Offset = 0, Len, LenOffset, wLen, w, wCRC16;
	BYTE *pDat, i, Num;
	DWORD dwAddr, dwTemp1, dwTemp2, dwExFlashAddr;
	TRealTimer TmpRealTimer;
	BOOL boResult;
	BYTE TmBuf[7];

	if ((Buf[Offset++] == Structure_698) && (Buf[Offset++] == 2))
	{
		if(Buf[Offset++] == DateTime_S_698)	//ִ������ʱ��
		{
			memset(TmBuf,0xff,sizeof(TmBuf));
			if(memcmp(&Buf[Offset],TmBuf,sizeof(TmBuf)) == 0)
			{
				tIapInfo.dwTime = 2;
			}
			else
			{
				lib_InverseData(&Buf[Offset], 2);
				memcpy((BYTE *)&(TmpRealTimer.wYear), &Buf[Offset], sizeof(TRealTimer));
				dwTemp1 = api_CalcInTimeRelativeSec(&TmpRealTimer);
				dwTemp2 = api_CalcInTimeRelativeSec(&RealTimer);
				if (dwTemp1 > dwTemp2)
				{
					tIapInfo.dwTime = dwTemp1-dwTemp2;
				}
				else
				{
					return DAR_RefuseOp;
				}
			}
			
			Offset += 7;
			i = 0;
			if (Buf[Offset] == Octet_string_698 ) // ���ע����
			{
				LenOffset = Deal_698DataLenth(&Buf[Offset], (BYTE *)&Len, ePROTOCOL_TYPE);
			}
			else if (Buf[Offset] == NULL_698)
			{
				i = 1;
			}

			if( i == 1 )
			{
				tIap.dwIapFlag = IAP_FLAG;
				tIap.dwCrc32 = lib_CheckCRC32((BYTE *)&tIap.bDestFile[0], sizeof(tIap) - sizeof(DWORD)); // ����У��

				boResult = api_WriteMemRecordData(IAP_INFO_ADDR, sizeof(tIap), (BYTE *)&tIap.bDestFile[0]);
				if( boResult == TRUE )
				{
					tIapInfo.dwFlag = IAP_READY;

					return DAR_Success;
				}
				else 
				{
					tIapInfo.dwTime = 0;
					
					return DAR_BlockSeq;
				}
			}
		}
	}
	
	tIapInfo.dwTime = 0;
	return DAR_WrongType;
}
//--------------------------------------------------
//��������:    ��������ȶ�
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pData[in]      pDataָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���    
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionSoftwareComparison( BYTE Ch, BYTE *pData)
{
	TTwoByteUnion Len,Leninpage;
	BYTE DAR,KeyOffest,i,factor[32],TmpBuf[32],EsamRand[100];
	BYTE *pCode_Flash,*ptr;
	WORD Result,KeyLength,Length,XorLenth,TotalLenth;
	DWORD factorAddress,DataAddress,Address;
	eAPPBufResultChoice eResultChoice;

	DAR = DAR_WrongType;
	Result = FALSE;
	KeyLength = 0;
	factorAddress = 0;
	DataAddress = 0;
	KeyOffest = 0;
	
	if( pData[0] == Structure_698 )
	{
		if( pData[1] == 5 )
		{
			if( pData[2] == Unsigned_698 )//cpu���
			{
				if( pData[3] == 0 )//cpu��ű���Ϊ��
				{
					if( pData[4] == Unsigned_698 )//��Կ����
					{
						KeyOffest = pData[5];
						if( pData[6] == Double_long_unsigned_698 )//������ʼ��ַ
						{
							lib_ExchangeData( (BYTE*)&factorAddress, &pData[7], 4);
							if( pData[11] == Double_long_unsigned_698 )//������ʼ��ַ
							{
								lib_ExchangeData( (BYTE*)&DataAddress, &pData[12], 4);
								if( pData[16] == Long_unsigned_698 )//���������ݳ���
								{
									lib_ExchangeData( (BYTE*)&KeyLength, &pData[17], 2);
									DAR = DAR_Success;
								}
							}
						}
					}
				}
			}
		}
	}
    if( DAR == DAR_Success )
    {
    	XorLenth = (KeyLength/4);//��ȡ�������ݳ���
    	
        if( XorLenth > MAX_APDU_SIZE )
        {
            DAR =  DAR_OverRegion;
        }
    	else if( (KeyLength%64) != 0 )//��64�ı������ش���
    	{
            DAR = DAR_RefuseOp; 
    	}
    	else
    	{
            for( i=0; i < 2; i++ )
            {
                if( i == 0 )
                {
                    Address= factorAddress;
                    ptr = factor;                   //�ȶ�����Bufָ��
                    pCode_Flash = (BYTE *)factorAddress;//�ȶ�����ָ��
                    Length = 16;                    //���ݳ���
                }
                else
                {
                    Address= DataAddress;
                    ptr = ProtocolBuf+24;                   //����Bufָ��
                    pCode_Flash = (BYTE *)DataAddress;  //������ʼָ��
                    Length = KeyLength;             //���ݳ���
                    
                    if( Length > (sizeof(ProtocolBuf) - 24) )//��ֹԽ��
	                {
	                    break;
	                }
                }
                
                if( Address > PROGRAMEND )//����RAM������ַ����
                {
                    break;
                }
            
                if( (Address+Length) > (PROGRAMEND+1) ) //������ַ��0x80
                {
                    memset( ptr, 0x00, Length);
                    memcpy( ptr, pCode_Flash, PROGRAMEND+1-Address );
                    ptr[PROGRAMEND+1-Address] = 0x80;
                }
                else if( (Address < (PROGRAMPROZ)) && ((Address+Length) > (PROGRAMPROZ)) )  //����������ַ��0x80
                {
                    memset( ptr, 0x00, Length);
                    memcpy( ptr, pCode_Flash, PROGRAMPROZ-Address );
                    ptr[PROGRAMPROZ-Address] = 0x80;
                }
    			else if( (Address < (PROGRAMPROZ1)) && ((Address+Length) > (PROGRAMPROZ1)) )  //����������ַ��0x80
                {
                    memset( ptr, 0x00, Length);
                    memcpy( ptr, pCode_Flash, PROGRAMPROZ1-Address );
                    ptr[PROGRAMPROZ1-Address] = 0x80;
                }
                else
                {
                    memcpy( ptr, pCode_Flash, Length );
                }

                FunctionConst(SOfTWARECOMPARISON_TASK);
            }
            
            if( i == 2)
            {
                //��ROM����,��������
                lib_XorData( factor, factor+8, 8 );//�õ��ȶ�����
                
                memcpy( EsamRand, ProtocolBuf+24, 64 );
                for( i=1; i<4; i++ )
                {
                    lib_XorData( EsamRand, EsamRand+16*i, 16 );//�õ�data1
                }
            
                for( i=1; i<4; i++ )
                {
                    lib_XorData( ProtocolBuf+24, ProtocolBuf+24+XorLenth*i, XorLenth );//�õ�data1
                }


                
                TotalLenth = XorLenth+24;//���ݳ���+��ɢ����(8)+�����(8)
                memcpy( ProtocolBuf, factor, 8 );//��ɢ����
                memcpy( ProtocolBuf+8, EsamRand, 16 );//�����
                
                Result = api_SoftwareComparisonEsam( KeyOffest, TotalLenth, ProtocolBuf );
                if( Result == TRUE )
                {
                    DAR = DAR_Success;
                    Len.b[0] = ProtocolBuf[1];//��ȡ����
                    Len.b[1] = ProtocolBuf[0];
                    
                }

                if( Len.w < MAX_APDU_SIZE )
                {
                    memmove( ProtocolBuf, ProtocolBuf+2, Len.w );
                }
                else
                {
                    DAR = DAR_OverRegion;
                }   
                
            }
            else
            {
                DAR = DAR_WrongType;
            }
    	}
    }
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
	if( DAR == DAR_Success )
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x00, 01);//OPTIONAL  CHOICE DATA 
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
		TmpBuf[0] = Structure_698;
		TmpBuf[1] = 1;
		TmpBuf[2] = Octet_string_698;
		Leninpage.w = Deal_698DataLenth( &TmpBuf[3], Len.b, eUNION_TYPE );//������ݳ��� ��ȡ����ռ�ݵ��ֽ���
		eResultChoice = DLT698AddBuf( Ch, 0x00, TmpBuf, 3+Leninpage.w );
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		
		eResultChoice = DLT698AddBuf( Ch, 0x55, ProtocolBuf, Len.w );
	}
	else
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x00, 00);//OPTIONAL NO  CHOICE DATA 
	}
	
    return eResultChoice;

}
//--------------------------------------------------
//��������:    �����ļ��ֿ鴫�����
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE MethodNo[in]    ������
//         
//         BYTE *pData[in]      ����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionFileBlockTransfer( BYTE Ch,  BYTE MethodNo, BYTE *pData)
{
	eAPPBufResultChoice eResultChoice;
	BYTE DAR = 0xFF;

	eResultChoice= eAPP_ERR_RESPONSE;

    
	switch( MethodNo )
	{
		case 7://��������	
			DAR = DAR_NoObject;
			break;		
		case 8://д�ļ�
			if (tIapInfo.dwFlag == IAP_START)
			{
				DAR = ActionBlockTransfe(Ch, pData);
			}
			else
			{
				DAR = DAR_RefuseOp; // δ�յ���ȷ��������������ܾ����������ļ�
			}

			if (LinkData[Ch].AddressType == eBROADCAST_ADDRESS_MODE) // ����ǹ㲥��ַ����Ӧ��
			{
				return eAPP_NO_RESPONSE;
			}
			break;
		case 9://���ļ�
			DAR = DAR_NoObject;
			break;
		case 10://����ȶ�
			eResultChoice = ActionSoftwareComparison( Ch, pData);
            break;
		case 13://ִ������
			DAR = ActionExeUpdate( Ch, pData);
			break;
		case 14: //��������
			DAR = ActionStartTransfe( Ch, pData);
			break;
		default:
			DAR = DAR_WrongType;
			break;
	}

	if(DAR != 0xFF)		//action û�з������ݣ�ͳһ���DAR���Լ�ѡ��û������
	{
		eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}

		eResultChoice = DLT698AddOneBuf(Ch,0x55, 0x00);//Optional 00 ������
	}

	return eResultChoice;
}
//--------------------------------------------------
//��������:    �����ļ�������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       OMDָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���     
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestFileTransfer( BYTE Ch,  BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE MethodNo;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
    eResultChoice = eAPP_ERR_RESPONSE;

	switch( OI.w )
	{
		case 0xf000://�ļ���֡�������
			break;
		case 0xf001://�ļ��ֿ鴫�����
			eResultChoice = ActionFileBlockTransfer( Ch,  MethodNo, pData);
			break;
		case 0xf002://�ļ���չ�������
			break;
		default:
			eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR_WrongType);//���DAR
            if( eResultChoice != eADD_OK )
            {
                return eResultChoice;
            }
            
            eResultChoice = DLT698AddOneBuf(Ch,0x55, 0x00);//Optional 00 ������
			break;
	}
	
	return eResultChoice;
}
//--------------------------------------------------
//��������:    ����������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       OMDָ��
//         
//����ֵ:     eAPPBufResultChoice  �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestVariable( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo,Type;
    BYTE i,Buf[4];
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	
	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��
	
	switch( OI.w )
	{
		case 0x2029://��ʱֵ����
			if( (MethodNo == 1) && ( pData[0] == Integer_698) )//��������1������ΪInteger_698
			{
				#if( SEL_AHOUR_FUNC == YES)
				api_ClrAHour();
				DAR = DAR_Success;
				#endif
			}
			break;
		case 0x2015://�����ϱ�״̬�ֵķ���127��ȷ�ϵ��ܱ�����ϱ�״̬�֣���= bit-string(SIZE(32))	
			if( (MethodNo == 127) && (pData[0] == Bit_string_698) && (pData[1] == 32) ) 
			{
				lib_ExchangeBit( (BYTE*)&Buf[0], (BYTE*)&pData[2], 4);
				for( i=0; i<4; i++ )
				{
					//�����λ���ϱ���������Ҫ����λ���������λ
					Type = Buf[i]&ReportDataBak[Ch].Status[i];
					ReportData[Ch].Status[i] &= ~Type;
				}
				//���㱸������
				memset(ReportDataBak[Ch].Status, 0x00, sizeof(ReportDataBak[0].Status) );
				ReportDataBak[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportDataBak[Ch],sizeof(TReportData)-sizeof(DWORD));
				ReportData[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[Ch],sizeof(TReportData)-sizeof(DWORD));

				DAR = DAR_Success;
			}
			break;
		default:
			DAR = DAR_WrongType;
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
    return eResultChoice;
}
//--------------------------------------------------
//��������:    698�������һ�����Զ�������
//         
//����:      BYTE *pData[in]      ����ָ��
//                  
//����ֵ:     BYTE DAR���  
//         
//��ע����:    ��
//--------------------------------------------------
BYTE ActionLCDDisplay( BYTE *pData )
{
	BYTE DAR,Buf[30],LCDSequence,Num;
	WORD Result,Time;
        
	Result = FALSE;
	DAR = DAR_WrongType;
	if( pData[0] == Structure_698 )
	{
		if( pData[1] == 3 )
		{
			if( pData[2] == CSD_698 )
			{
				if( pData[3] == 0)//ѡ��OAD
				{
					memcpy( Buf, &pData[4],4);//ȡOAD
					Num = 1;
					if( pData[8] == Unsigned_698 )
					{
						LCDSequence = pData[9];
						if( pData[10] == Long_unsigned_698 )
						{
							lib_ExchangeData( (BYTE*)&Time, &pData[11], 2);
							DAR = DAR_Success;
						}
					}					
				}
				else if( pData[3] == 1)//ѡ��ROAD
				{					
					memcpy( Buf, &pData[4], 4);

					memcpy( &Buf[4], &pData[9], 4);
					Num = 2;
					if( pData[9+4] == Unsigned_698 )
					{
						LCDSequence = pData[9+4+1];
						if( pData[9+4+2] == Long_unsigned_698 )
						{
							lib_ExchangeData( (BYTE*)&Time, &pData[9+4+3], 2);
							DAR = DAR_Success;
						}
					}
				}
			}
		}
	}

	if( DAR == DAR_Success )
	{
		Result = api_WriteLcdEchoInfo( Num, Buf, LCDSequence, Time );
		if( Result == FALSE )
		{
			DAR = DAR_HardWare;
		}
	}
	return DAR;
}
//-----------------------------------------------
//��������  :    ����Һ����ʾ
//����:  
//           BYTE Type[in]        0 ѭ��   1����
//
//           BYTE Ch[in]          ͨ��ѡ��
//
//           BYTE *pOMD[in]       OMDָ��
//
//����ֵ:       eAPPBufResultChoice  �������Bufָ��
//��ע����  :    �����ݷ��ص�ֱ��return
//-----------------------------------------------
eAPPBufResultChoice ActionRequestLCD( BYTE Type,BYTE Ch,  BYTE *pOMD)
{
	BYTE DAR;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	TFourByteUnion OAD;
	WORD Result,Time;
	
	pData = &pOMD[4];
	DAR = DAR_WrongType;//Ĭ��ʧ��
	
	switch( pOMD[2] )
	{
		case 0x03://�·�
		    if( pData[0] == NULL_698 )//����Ϊnull
		    {
                if( Type == 1 )
                {
                    api_ProcLcdCommKey( eDownKey ); 
                    DAR = DAR_Success;
                }
                else
                {
                    DAR = DAR_HardWare;
                }
		    }

			break;	
		case 0x04://�Ϸ�
            if( pData[0] == NULL_698 )//����Ϊnull
            {
                if( Type == 1 )
                {
                    api_ProcLcdCommKey( eUpKey ); 
                    DAR = DAR_Success;
                }
                else
                {
                    DAR = DAR_HardWare;
                }
            }
		
			break;		
		case 0x05://��ʾ�鿴
			DAR = ActionLCDDisplay( pData);
			break;		
		case 0x06://ȫ��
			if( pData[0] == Long_unsigned_698 )
			{
				lib_ExchangeData( (BYTE*)&Time, &pData[1], 2 );
				OAD.d = 0XDFDFDFDF;
				Result = api_WriteLcdEchoInfo(1, OAD.b, 0, Time);
				if( Result == FALSE )
				{
					DAR = DAR_HardWare; 
				}
				else
				{
					DAR = DAR_Success; 
				}
			}
			break;
		default:
			break;
	}
	
	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	
	return eResultChoice;
}
#if( SEL_TOPOLOGY == YES )
eAPPBufResultChoice ActionRequestTopology( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	DWORD Inpara = 0;
	
	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��


	switch( MethodNo )
	{
		case 129://������������ź�ʶ����
			if( pData[0] == NULL_698 )
			{
				api_WriteEventAttribute(0x4E07, 0xff, (BYTE *)&Inpara, 0);
				DAR = DAR_Success;
			}
			else
			{
				DAR = DAR_OverRegion;
			}
			break;
		default:
			break;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	return eResultChoice;
}

eAPPBufResultChoice ActionRequestTopologySendSignal( BYTE Ch, BYTE *pOMD)
{
	TTwoByteUnion OI;
	BYTE DAR,MethodNo,Index;
	BYTE *pData;
	eAPPBufResultChoice eResultChoice;
	DWORD Inpara = 0;
	
	lib_ExchangeData(OI.b,pOMD,2);//ȡoi
	MethodNo = pOMD[2];//ȡ����
	pData = &pOMD[4];//ȡdata
	DAR = DAR_WrongType;//Ĭ��ʧ��


	switch( MethodNo )
	{
		case 127:	//������������
			DAR = api_SetTopoModulatSignalInfo698(0,pData,NULL);
			if( DAR == DAR_Success)
			{
				if(api_TopoSignalSendStart() == FALSE)
				{
					DAR = DAR_TempFail;
				}
				else
				{
					DAR = DAR_Success;
				}
			}
			break;
		case 128:	//��ʱ��������
			if( (pData[0] != Structure_698) ||(pData[1] != 2) )
			{
				DAR = DAR_WrongType;
				break;
			}
			pData += 2;
			// �жϵ�����Ϣ�Ƿ�Ϸ�
			if( api_SetTopoModulatSignalInfo698(0,&pData[0],&Index) == DAR_Success)
			{
				// �ж�ʱ���Ƿ�Ϸ�
				if(pData[Index++] != DateTime_S_698)
				{
					DAR = DAR_WrongType;
					break;
				}

				lib_InverseData(&pData[Index],2);
				if( (api_CheckClock((TRealTimer*)&pData[Index]) == FALSE)
				 || (api_CalcInTimeRelativeSec((TRealTimer*)&pData[Index]) < api_CalcInTimeRelativeSec(&RealTimer))
				 || ( (api_CalcInTimeRelativeSec((TRealTimer*)&pData[Index]) - api_CalcInTimeRelativeSec(&RealTimer)) <= (TOPO_SIGNAL_SEND_INTERVAL - TopoSignalSendIntervalCnt))
				)
				{
					DAR = DAR_OverRegion;
					break;
				}
				// ���õ�����Ϣ
				DAR = api_SetTopoModulatSignalInfo698(0,&pData[0],NULL);
				if( DAR != DAR_Success)
				{
					break;
				}
				// ���÷���ʱ��
				memcpy(&ModulatSignalInfoRAM.Time,&pData[Index],sizeof(TRealTimer));
				if(api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ModulatSignalInfoSafeRom), sizeof(ModulatSignalInfo_t), (BYTE *)&ModulatSignalInfoRAM) == TRUE)
				{
					DAR = DAR_Success;
				}
				else
				{
					DAR = DAR_HardWare;
				}
			}
			break;
		default:
			break;
	}

	eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR);//���DAR
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
	return eResultChoice;
}
#endif
//-----------------------------------------------
//��������: ��������
//����:     
//		   BYTE Ch[in]       ͨ��ѡ��
//
//         BYTE *pOMD[in]    ����OMDָ��
//  
//����ֵ:     eAPPBufResultChoice �������Buf���
//
//��ע����:    �����ݵ�ֱ��RETRUN �������ݵ������� δ�����д������� list������ ��������ʱ ��¼���ݴ�����
//-----------------------------------------------
eAPPBufResultChoice  ActionRequest( BYTE Ch,BYTE *pOMD)
{
	TTwoByteUnion OI;
	eAPPBufResultChoice eResultChoice;
	WORD DataLen;
	BYTE OMD[4],Method = 0,Mode = 0,bResult;
	TFourByteUnion OMD2;
    BYTE DTTemp[50];
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
	lib_ExchangeData(OI.b,pOMD,2);
	Method = pOMD[2];
	memcpy( OMD, pOMD, 4 );
	
	eResultChoice = DLT698AddBuf(Ch,0, pOMD, 4);//���OAD���� ������������
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    DataLen = GetProtocolDataLen( pOMD+4);//��ȡData���� DataLen�Ĵ��󳤶��ں���������ж�
	//if(IsModuleOI(OI.w,eModuleParaOI)||IsModuleOI(OI.w,eModuleEventOI))
	if(IsModuleOI(OI.w,eModuleAllOI))
	{
		eResultChoice = (eAPPBufResultChoice)ModuleEventFunc(Ch,pOMD);
		
	}
	else if( APPData[Ch].TimeTagFlag == eTime_Invalid )//ʱ���ǩ����
    {
		eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_TimeStamp);//DAR
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL 
    }
	else if(JudgeTaskAuthority( Ch, eACTION_MODE, pOMD) == FALSE)//�жϰ�ȫģʽ�������Ƿ���Ӧ������
	{	
		if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )//����������в���
		{	
			if( (OI.w >= 0x8000) && (OI.w <= 0x8001) )
			{
				api_SetRelayRelayErrStatus( 0x01 );
			}
			
		    eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_HangUp);//DAR
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL 
		}
		else
		{
			if( (OI.w >= 0x8000) && (OI.w <= 0x8001) )
			{
				api_SetRelayRelayErrStatus( 0x04 );
			}
			
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_PassWord);//DAR
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL 
		}
	}
	// else if (IsModuleOI(OI.w,eModuleOrdOI ))
	// {
	// 	eResultChoice = (eAPPBufResultChoice)ModuleEventFunc( Ch, pOMD);
	// }
	
	else if( (OI.w >= 0x2000) && (OI.w <= 0x2506) )//�������������
	{
		eResultChoice = ActionRequestVariable( Ch, pOMD);
	}
	else if( (OI.w >= 0x3000) && (OI.w <= 0x330e) )//�����¼������
	{
		eResultChoice = ActionRequestEvent( Ch, pOMD);

		if( (Method == 4) || ( Method == 5 ) )//�޸Ĺ������������ж�
		{
            Mode = 0x55;
		}
	}
	else if( OI.w == 0x4000 )//�㲥Уʱ
	{
		eResultChoice = ActionRequestTimeBroadCast( Ch,  pOMD);
	}
	else if( OI.w == 0x4300 )//���������豸
	{
		eResultChoice = ActionRequestMeter( Ch,  pOMD);
	}
	#if (SEL_TOPOLOGY == YES )
	else if( OI.w == 0x4E06 )//���������źŷ��Ͳ���
	{
		eResultChoice = ActionRequestTopologySendSignal(Ch, pOMD);
		if( (Method == 127) || (Method == 128) )
		{
			Mode = 0x55;
		}
	}
	else if( OI.w == 0x4E07 )//��������
	{
		eResultChoice = ActionRequestTopology(Ch, pOMD);
		if(Method == 129)
		{
			Mode = 0x55;
		}
	}
	#endif
	else if( (OI.w >= 0x5000) && (OI.w <= 0x5011))//������������� 
	{
		eResultChoice = ActionRequestFreeze( Ch, pOMD);

		if( ((Method >= 4) && (Method <= 8 )) && ( Method != 6) )//�޸Ĺ������������ж�
		{
            Mode = 0x55;
		}
	}
	else if (OI.w == 0x6000)
	{

		lib_ExchangeData(OMD2.b,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w],4);
		
		eResultChoice=eADD_OK;
		// if (OI.w == 0x6012)
		// {
		// 	bResult = Class10_action(OMD2.d,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]+4,DTTemp+10,(unsigned short *)DTTemp);
		// }
		// else
		{
			bResult = Class11_action(OMD2.d,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]+4,DTTemp+10,(unsigned short *)DTTemp);
		}

		if(bResult == TRUE)
		{
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, 00);
			eResultChoice = DLT698AddOneBuf( Ch, 0x55, 00);			
		}
		else
		{
			eResultChoice = DLT698AddOneBuf(Ch,0x00, DAR_TempFail);//���DAR
			if( eResultChoice != eADD_OK )
			{
				return eResultChoice;
			}
			eResultChoice = DLT698AddOneBuf(Ch,0x55, 00);//OPTIONAL NO CHOICE DATA 
		}
	}
    #if (SEL_SEARCH_METER == YES )
	else if( (OI.w == 0x6002))//�����ѱ�
	{
		eResultChoice = ActionRequestSchelControl( Ch, pOMD);
        if( (Method == 127) || (Method == 128) )
		{
			Mode = 0x55;
		}
	}
    #endif
	else if( (OI.w >= 0x8000) && (OI.w <= 0x8001) )//�̵�������
	{
		eResultChoice = ActionRequestControl( Ch, pOMD);
	}
	else if( (OI.w >= 0xf000) && (OI.w <= 0xf002) )//�����ļ�������
	{
		eResultChoice = ActionRequestFileTransfer( Ch, pOMD);
	}
	#if(SEL_ESAM_TYPE != 0)
	else if( OI.w == 0xf100 )//����esam �ӿ���
	{
		eResultChoice = ActionRequestEsam( Ch, pOMD);
		//���ݸ��� ��Կ���� ����Э��ʱЧ ������ֵ Ǯ����ʼ��
		if( (Method == 7) ||(Method == 9) ||(Method == 6)||(Method == 0X0A) )
		{
            Mode = 0x55;
		}
	}
	#endif
	else if( (OI.w == 0xf300) || (OI.w == 0xf301) )//��ʾ�ӿ���
	{
		eResultChoice = ActionRequestLCD( (OI.w-0xf300), Ch, pOMD);
	}
	#if(SEL_ESAM_TYPE != 0)
	else if ( OI.w == 0xf101 )
	{
		eResultChoice = ActionRequestEsamSafeMode( Ch, pOMD);
		if( (Method == 127) || (Method == 129) )//���� �������Ӱ�ȫģʽ����
		{
            Mode = 0x55;
		}
	}
	#endif
	else if( (OI.w >= 0xf200) && (OI.w <= 0xf221 ))//��������豸��
	{
		eResultChoice = ActionRequestDevice( Ch, pOMD);
		if( (OI.w == 0xf201) || (OI.w == 0xf205) )
		{
			Mode = 0x55;
		}
	}
	else//������� ����DAR
	{
		eResultChoice = DLT698AddOneBuf( Ch, 0x55, DAR_NoObject);//DAR
		if( eResultChoice != eADD_OK )
		{
			return eResultChoice;
		}
		eResultChoice = DLT698AddOneBuf( Ch, 0x55, 0);//DATA OPTINAL 
	}

    if( eResultChoice == eADD_OK )
	{
	    if( Mode == 0x55 )
	    {
            if( APPData[Ch].pAPPBuf[(APPData[Ch].APPCurrentBufLen.w-2)] == DAR_Success )//��Buf����2���ֽ� �ж�DAR ���óɹ���¼��̼�¼
            {
                api_SaveProgramRecord( EVENT_START, 0x53, OMD);
            }   
	    }
	}

	memcpy( OMD2.b, pOMD, 4 );
	if((OI.w != 0x4000)&&(OMD2.d !=0x000801F0))//Ϊ�����ֳ��ظ��㲥Уʱ���㲥Уʱ����¼�����¼�,�������ݰ����Ǽ�¼
	{
		api_WriteFreeEvent( (EVENT_METHOD_698 << 8) + pOMD[2], OI.w );
	}
    if( DataLen == 0x8000 )//������ݽ������󣬲����к���List�Ľ���
    {
        return eAPP_RETURN_DATA;
    }

    APPData[Ch].BufDealLen.w += (DataLen+4);
    
	return eResultChoice;	
}
//--------------------------------------------------
//��������:    698���������������
//         
//����:      BYTE Ch[in]       ͨ��ѡ��
//         
//         BYTE *pOMD[in]    OMD����ָ��
//         
//����ֵ:     eAPPBufResultChoice �������Buf���
//         
//��ע����:    ��
//--------------------------------------------------
eAPPBufResultChoice ActionRequestList( BYTE Ch,BYTE *pOMD)
{
	BYTE i,LenOffest;
	WORD NoAddress;
	eAPPBufResultChoice eResultChoice;
	TTwoByteUnion Num,No;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;
	No.w = 0;
	
	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//��Ӹ��� ��������֡����ȷ
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}

	NoAddress = APPData[Ch].APPCurrentBufLen.w;
	
	for(i=0;i<Num.w;i++)
	{
		eResultChoice = ActionRequest( Ch, &LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);
		if( eResultChoice != eADD_OK )//��Ӳ��ɹ��ͷ���
		{
			return eResultChoice;
		}
		
		No.w ++;//sequence of action_result
				
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//�Ѵ������ݴ����·������ݽ��з��أ���������ĵĸ���
		{
			break;
		}
	}
	
	JudgeAppBufLen(Ch);
	
	Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
	return eResultChoice;
}
//--------------------------------------------------
//��������:    698�������ȡ�������
//         
//����:      BYTE Ch[in]          ͨ��ѡ��
//         
//         BYTE *pOMD[in]       OMD����ָ��
//         
//����ֵ:     eAPPBufResultChoice  �������BUF���
//         
//��ע����:  ��
//--------------------------------------------------
eAPPBufResultChoice ActionThenGetRequestNormalList( BYTE Ch,BYTE *pOMD)
{
	BYTE i,LenOffest;
	WORD NoAddress;
	eAPPBufResultChoice eResultChoice;
    TTwoByteUnion Num,No;
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
    LenOffest = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w], Num.b, ePROTOCOL_TYPE ); 
    APPData[Ch].BufDealLen.w +=LenOffest;
    No.w = 0;

	eResultChoice = DLT698AddOneBuf(Ch,0, Num.b[0] );//��Ӹ��� ��������֡����ȷ
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    NoAddress = APPData[Ch].APPCurrentBufLen.w;

	for(i=0;i<Num.w;i++)
	{
		//��������
		eResultChoice = ActionRequest( Ch,&LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w] );//
		if( eResultChoice != eADD_OK )//��Ӳ��ɹ��ͷ���
		{
			return eResultChoice;
		}
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//�Ѵ������ݴ����·������ݽ��з��أ���������ĵĸ���
		{
			break;
		}
		//��ȡ����
		eResultChoice= GetRequestNormal( Ch, &LinkData[Ch].pAPP[APPData[Ch].BufDealLen.w]);
		if( eResultChoice != eADD_OK )//��Ӳ��ɹ��ͷ���
		{
			return eResultChoice;
		}
		
        APPData[Ch].BufDealLen.w += 1;//��ȡ��ʱ
        
		No.w++;// sequence of action_then_get_result
		
		if( APPData[Ch].BufDealLen.w >= LinkData[Ch].pAPPLen.w )//�Ѵ������ݴ����·������ݽ��з��أ���������ĵĸ���
		{
			break;
		}
	}

    JudgeAppBufLen(Ch);
    
	Deal_698SequenceOfNumData( Ch, NoAddress, No.b);
	
    return eResultChoice;
}
//--------------------------------------------------
//��������:    698��������
//         
//����:      BYTE Ch[in]      ͨ��ѡ��
//         
//         BYTE *pAPDU[in]  APDUָ��
//         
//����ֵ:     ��  
//         
//��ע����:    ��
//--------------------------------------------------
void DealSet_Request(BYTE Ch,BYTE *pAPDU )
{
	eAPPBufResultChoice eResultChoice;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//�������ͷ
	APPData[Ch].BufDealLen.w = 3;
    eResultChoice = eAPP_ERR_RESPONSE;

	// �͹�������ģʽ
	if(api_GetSysStatus(eSYS_STATUS_RUN_LOW_POWER) == TRUE)
	{
		APPData[Ch].APPFlag = APP_ERR_DATA;
		return;
	}
	switch(pAPDU[1])
	{		
	case SET_REQUEST_NORMAL:
		eResultChoice = SetRequestNormal( Ch, (BYTE  *)&(pAPDU[3]));
		break;
	case SET_REQUEST_NORMALLIST:	
		eResultChoice = SetRequestNormalList( Ch );
		break;
	case SET_THEN_GET_REQUEST:
		eResultChoice = SetThenGetRequestNormalList(Ch);
		break;
	default:
		break;		
	}
	
    if( eResultChoice == eAPP_NO_RESPONSE )
	{
		APPData[Ch].APPFlag = APP_NO_DATA;
	}
	else if( eResultChoice == eAPP_ERR_RESPONSE )
	{
	    APPData[Ch].APPFlag = APP_ERR_DATA;
	}
	else
	{
		APPData[Ch].APPFlag = APP_HAVE_DATA;
	}
}
//--------------------------------------------------
//��������:  
//         
//����:      
//
//����ֵ:    
//         
//��ע:  ����һ�η�֡����
//--------------------------------------------------
BOOL  SetNextPara(  BYTE Ch,void* pgetcfun,DWORD oadd,WORD offset,BYTE *pPara,BYTE GetrequsetNextType)
{
    memset(&pGetRequestNext[Ch],0x00,sizeof(TGetRequestNext));
	if (pPara!=NULL)
	{
		memcpy(pGetRequestNext[Ch].pPara,pPara,sizeof(pGetRequestNext[Ch].pPara));
	}
	pGetRequestNext[Ch].pgetcfun=pgetcfun;
	pGetRequestNext[Ch].offset=offset;
	pGetRequestNext[Ch].oadd=oadd;
	pGetRequestNext[Ch].GetrequsetNextType=GetrequsetNextType;
	pGetRequestNext[Ch].FZtype=TRUE;
	return TRUE;
}
//--------------------------------------------------
//��������:  GetRequestNext
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
eAPPBufResultChoice  GetRequestNext( BYTE Ch)
{
	WORD Len;
	eAPPBufResultChoice eResultChoice;
	BOOL bNext=FALSE;
	WORD PwRNum;
	BYTE Tempoad[4];

	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return eAPP_ERR_RESPONSE;
    }
    
	pGetRequestNext[Ch].FZtype=TRUE;

	lib_ExchangeData(Tempoad,(BYTE*)&pGetRequestNext[Ch].oadd,4);
	eResultChoice = DLT698AddBuf(Ch,0,Tempoad, 4);//���OAD���� ������������
	if( eResultChoice != eADD_OK )
	{
		return eResultChoice;
	}
	
    APPData[Ch].BufDealLen.w +=4;//�Ѵ���4�ֽ�oad

	if( (pGetRequestNext[Ch].pgetcfun==(void*)Class11_Get_rec)
//	   ||(pGetRequestNext[Ch].pgetcfun==(void*)Class10_Get_rec)
	   )
	{
		Len = ((FW_BDHWHWHH)pGetRequestNext[Ch].pgetcfun)(Ch, pGetRequestNext[Ch].oadd,pGetRequestNext[Ch].pPara+4,
														 pGetRequestNext[Ch].offset,&APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w],
														 (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w),&bNext,&PwRNum );
		pGetRequestNext[Ch].GetrequsetNextType = 2;
	}
	else if( (pGetRequestNext[Ch].pgetcfun==(void*)Class11_Get)
//			||(pGetRequestNext[Ch].pgetcfun==(void*)Class10_Get)
			)
	{
		Len = ((FW_DWHWH)pGetRequestNext[Ch].pgetcfun)( pGetRequestNext[Ch].oadd,pGetRequestNext[Ch].offset,
													   &APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+1],
													   (APPData[Ch].APPMaxBufLen.w- APPData[Ch].APPCurrentBufLen.w-1),&bNext );
		PwRNum=APPData[Ch].pAPPBuf[APPData[Ch].APPCurrentBufLen.w+2] + pGetRequestNext[Ch].offset;
		pGetRequestNext[Ch].GetrequsetNextType = 1;
		if (Len)
		{
			eResultChoice = DLT698AddOneBuf(Ch,0, 1);
		}
		
	}
	else
	{
		return eAPP_ERR_RESPONSE;
	}
	if(Len)
	{
		pGetRequestNext[Ch].SlicingIndex++;
		pGetRequestNext[Ch].offset = PwRNum;
		pGetRequestNext[Ch].IslastFrame = (!bNext);
		APPData[Ch].BufDealLen.w += Len;
		APPData[Ch].APPCurrentBufLen.w+=Len;
		return eADD_OK;
	}
	else
	{
		return eAPP_ERR_RESPONSE;
	}
	
	
	
}
//--------------------------------------------------
//��������:    698��ȡ����
//         
//����:      BYTE Ch[in]       ͨ��ѡ��
//         
//         BYTE *pAPDU[in]   APDUָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static void DealGet_Request(BYTE Ch,BYTE *pAPDU)
{
	eAPPBufResultChoice eResultChoice;

	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }

	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//�������ͷ
	APPData[Ch].BufDealLen.w = 3;
	APPData[Ch].APPCurrentBufLen.w+=5;
	eResultChoice = eAPP_ERR_RESPONSE;
	
	switch(pAPDU[1])
	{		
	case GET_REQUEST_NORMAL:
		eResultChoice = GetRequestNormal(Ch,&pAPDU[3]);
		break;
	case GET_REQUEST_NORMALLIST:	
		eResultChoice = GetRequestNormalList(Ch);
		break;
	case GET_REQUEST_RECORD:
		eResultChoice = GetRequestRecord(Ch,&pAPDU[3]);
		break;
	case GET_REQUEST_RECORDLIST:	 
		eResultChoice = GetRequestRecordList(Ch);
		break;
	case GET_REQUEST_NEXT:
		eResultChoice = GetRequestNext(Ch);
		break;
	default:
		break;
	}
	if (pGetRequestNext[Ch].FZtype==TRUE)
	{
		pGetRequestNext[Ch].FZtype=FALSE;
		APPData[Ch].pAPPBuf[1]=0x05;//��֡��Ӧһ�����ݿ�
		APPData[Ch].pAPPBuf[3]=pGetRequestNext[Ch].IslastFrame;
		APPData[Ch].pAPPBuf[4]=pGetRequestNext[Ch].SlicingIndex/0x100;
		APPData[Ch].pAPPBuf[5]=pGetRequestNext[Ch].SlicingIndex%0x100;
		APPData[Ch].pAPPBuf[6]=pGetRequestNext[Ch].GetrequsetNextType;
		APPData[Ch].pAPPBuf[7]=1;//�ݹ̶�Ϊ1  ��֧��list��Ӧ�ò��֡
		if (pGetRequestNext[Ch].IslastFrame)
		{
			memset(&pGetRequestNext[Ch],0x00,sizeof(TGetRequestNext));
		}
	}
	else
	{
		APPData[Ch].APPCurrentBufLen.w -=5;
		memcpy( &APPData[Ch].pAPPBuf[3],&APPData[Ch].pAPPBuf[3+5],APPData[Ch].APPCurrentBufLen.w-3);
		memset(&pGetRequestNext[Ch],0x00,sizeof(TGetRequestNext));
	}
	
	
    if( eResultChoice == eAPP_NO_RESPONSE )
	{
		APPData[Ch].APPFlag = APP_NO_DATA;
	}
	else if( eResultChoice == eAPP_ERR_RESPONSE )
	{
	    APPData[Ch].APPFlag = APP_ERR_DATA;
	}
	else
	{
		APPData[Ch].APPFlag = APP_HAVE_DATA;
	}
}
//--------------------------------------------------
//��������:    698����������
//         
//����:      BYTE Ch[in]    ͨ��ѡ��
//         
//         BYTE *pAPDU[in] apduָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
void DealAction_Request(BYTE Ch,BYTE *pAPDU)
{
	eAPPBufResultChoice 	eResultChoice;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
	{
        return;
    }

	DLT698AddBuf(Ch, 0, LinkData[Ch].pAPP,3);//�������ͷ
	APPData[Ch].BufDealLen.w = 3;
	eResultChoice = eAPP_ERR_RESPONSE;
	
	// �͹�������ģʽ
	if(api_GetSysStatus(eSYS_STATUS_RUN_LOW_POWER) == TRUE)
	{
		APPData[Ch].APPFlag = APP_ERR_DATA;
		return;
	}

	switch(pAPDU[1])
	{		
	case ACTION_REQUEST_NORMAL:
		eResultChoice = ActionRequest( Ch,(BYTE  *)&(pAPDU[3]) );
		break;
	case ACTION_REQUEST_NORMALLIST:	
		eResultChoice = ActionRequestList( Ch,(BYTE  *)&(pAPDU[3]) );
		break;
	case ACTION_THEN_GET_REQUEST:
		eResultChoice = ActionThenGetRequestNormalList( Ch,(BYTE  *)&(pAPDU[3]));
		break;	
	default:
		break;		
	}
	
    if( eResultChoice == eAPP_NO_RESPONSE )
	{
		APPData[Ch].APPFlag = APP_NO_DATA;
	}
	else if( eResultChoice == eAPP_ERR_RESPONSE )
	{
	    APPData[Ch].APPFlag = APP_ERR_DATA;
	}
	else
	{
		APPData[Ch].APPFlag = APP_HAVE_DATA;
	}
}

//--------------------------------------------------
//��������:    698�ϱ�ȷ�Ϻ���
//         
//����:      BYTE Ch[in]    ͨ��ѡ��
//         
//         BYTE *pAPDU[in] apduָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
void DealReportResponse_Request(BYTE Ch,BYTE *pAPDU)
{
	BYTE i, Num;
	TFourByteUnion OAD;
	
	if( Ch != eCR )//���ز�ͨ�������д���
	{
		return ;
	}

	switch(pAPDU[1])
	{		
		case REPORT_RESPONSE_LIST:
		case REPORT_RESPONSE_RECORD_LIST:
		Num = pAPDU[3];//ȡ����
		if( Num < 10 )//���֧��ȷ��10��OAD
		{
			for( i=0; i < Num; i++ )
			{
				lib_ExchangeData( OAD.b, &pAPDU[4+4*i], 4 );
				if( OAD.d == 0x33200200 )//ȷ���¼������б�
				{
					api_ResponseActiveReport( 0x00 );
				}
				else if( OAD.d == 0x20150200 )//ȷ�ϸ����ϱ�״̬��
				{
					api_ResponseActiveReport( 0x55 );
				}
				else if( OAD.d == 0x30110201 )//ȷ�ϵ����¼�
				{
					api_ResponseActiveReport( 0xAA );
				}
				else
				{}
			}
		}
		
		break;
		
		default:
		break;
	}

	APPData[Ch].APPFlag = APP_NO_DATA;
	
}

//--------------------------------------------------
//��������:  698����������
//         
//����:      BYTE Ch[in]    ͨ��ѡ��
//         
//           BYTE *pAPDU[in] apduָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
// void DealProxy_Request( BYTE Ch,BYTE *pAPDU )
// {
// 	BOOL result = FALSE;

// 	if((Ch >= MAX_COM_CHANNEL_NUM)
// 	||(pAPDU == NULL)
// 	||(JLTxBuf.byValid != JLS_IDLE))
// 	{
// 		APPData[Ch].APPFlag  = APP_NO_DATA;
//         return;
//     }

// 	JLTxBufInit(&JLTxBuf);

// 	switch(pAPDU[1])
// 	{		
//         case PROXY_GET_REQUEST_LIST:
//             result = ProxyGetRequestList( Ch,&JLTxBuf );
//             break;
//         case PROXY_GET_REQUEST_RECORD:	
//             result = ProxyGetRequestRecord( Ch,&JLTxBuf );
//             break;
//         case PROXY_SET_REQUEST_LIST:
//             result = FALSE;
//             break;	
//         case PROXY_SET_THEN_GET_REQUEST_LIST:
//             result = FALSE;
//             break;
//         case PROXY_ACTION_REQUEST_LIST:
//             result = FALSE;
//             break;
//         case PROXY_ACTION_THEN_GET_REQUEST_LIST:
//             result = FALSE;
//             break;
//         case PROXY_TRANS_COMMAND_REQUEST:
//             result = ProxyTransCommandRequest( Ch,&JLTxBuf );
//             break;
//         case PROXY_INNER_TRANS_COMMAND_REQUEST:
//             result = FALSE;
//             break;
//         default:
//             result = FALSE;
//             break;		
// 	}
	
//     if(result == TRUE)
//     {
//         JLTxBuf.bySourceNo   = Ch;
//         JLTxBuf.wRxWptr	     = 0;
//         JLTxBuf.SafeMode     = APPData[Ch].eSafeMode;
//         JLTxBuf.byValid      = JLS_TX;
//         APPData[Ch].APPFlag  = APP_NO_DATA;
//         //��ʱʱ�����
//         if(JLTxBuf.wTimeOut == 0)
//         {
//             JLTxBuf.wTimeOut = 5;
//         }
//         else if( (JLTxBuf.byDestNo == eCR) && (JLTxBuf.wTimeOut > 30) )
//         {
//             JLTxBuf.wTimeOut = 30;
//         }
//         else if((JLTxBuf.byDestNo != eCR) &&(JLTxBuf.wTimeOut > 10) )
//         {
//             JLTxBuf.wTimeOut = 10;
//         }
//     }
//     else
//     {
//         JLTxBufInit(&JLTxBuf);
//         APPData[Ch].APPFlag  = APP_ERR_DATA;
//     }
// }
static WORD AnalyseGetRequestRecordLen( BYTE *pOAD )
{
    WORD Len,Len1;
    BYTE i;
    Len = 4;
    
    if( pOAD[Len] == 1 )
    {
        Len += 5;
        Len1 = GetProtocolDataLen( (BYTE*)&pOAD[Len] );//��ȡDATA�����ݳ���
        if( Len1 == 0x8000 )//�����ȡ����ʧ�ܣ���ʱ��Ϊ������Ӧ��
        {
            return 0X8000;
        }
        Len += Len1;
        
        if( pOAD[Len] == 0 )//RCSD
        {
            Len +=1;
        }
        else
        {
            if( pOAD[Len+1] == 0 )
            {
                Len += (5*pOAD[Len]+1);
            }
            else if( pOAD[Len+1] == 1 )
            {
                Len += ((1+4+1+pOAD[Len+6]*4)*pOAD[Len]+1);
            }
            else
            {
                return 0X8000;
            }
        }
    }
    else if( pOAD[Len] == 2 )
    {
        Len += 5;

        Len1 = GetProtocolDataLen( (BYTE*)&pOAD[Len] );//��ȡ��ʼֵ����
        if( Len1 == 0x8000 )//�����ȡ����ʧ�ܣ���ʱ��Ϊ������Ӧ��
        {
            return 0x8000;
        }
        
        Len += Len1; 
        
        Len1 = GetProtocolDataLen( (BYTE*)&pOAD[Len] );//��ȡ����ֵ����
        if( Len1 == 0x8000 )//�����ȡ����ʧ�ܣ���ʱ��Ϊ������Ӧ��
        {
            return 0x8000;
        }
        
        Len += Len1; 

        Len1 = GetProtocolDataLen( (BYTE*)&pOAD[Len] );//��ȡ���ݼ��
        if( Len1 == 0x8000 )//�����ȡ����ʧ�ܣ���ʱ��Ϊ������Ӧ��
        {
            return 0x8000;
        }
        
        Len += Len1; 
        
        if( pOAD[Len] == 0 )//RCSD
        {
            Len +=1;
        }
        else
        {
            if( pOAD[Len+1] == 0 )
            {
                Len += (5*pOAD[Len]+1);
            }
            else if( pOAD[Len+1] == 1 )
            {
                Len += ((1+4+1+pOAD[Len+6]*4)*pOAD[Len]+1);
            }
            else
            {
                return 0X8000;
            }
        }

    }
	else if(pOAD[Len] == 7)
	{
		Len += 1;
		Len += 18;
		if(pOAD[Len] == 0)   // RCSD
		{
			Len += 1;
		}
		else
		{
			Len1 = Len + 1;
			for ( i = 0; i < pOAD[Len]; i++)
			{
				if(pOAD[Len1] == 0)
				{
					Len1 += 5;
				}
				else if(pOAD[Len1] == 1)
				{
					Len1 += (1 + 4 + 1 + pOAD[Len1 + 5] * 4);
				}
				else
				{
					return 0X8000;
				}
			}
			Len = Len1;
		}
	}
	else if( pOAD[Len] == 9 )
    {
        Len += 2;

        if( pOAD[Len] == 0 )//RCSD
        {
            Len +=1;
        }
        else
        {
            if( pOAD[Len+1] == 0 )
            {
                Len += (5*pOAD[Len]+1);
            }
            else if( pOAD[Len+1] == 1 )
            {
                Len += ((1+4+1+pOAD[Len+6]*4)*pOAD[Len]+1);
            }
            else
            {
                return 0X8000;
            }
        }

    }
    
    return Len;
}

//--------------------------------------------------
//��������:    698��ȡ����
//         
//����:      BYTE Ch[in]       ͨ��ѡ��
//         
//         BYTE *pAPDU[in]   APDUָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static WORD AnalyseGetRequestLen( BYTE *pAPDU )
{
    WORD Len,Len1;
    BYTE i;
    RSD rsd;

	Len = 3;

	switch(pAPDU[1])
	{		
    case GET_REQUEST_NEXT:
		Len += 2;
		break;
	case GET_REQUEST_NORMAL:
		Len += 4;
		break;
	case GET_REQUEST_NORMALLIST:	
		Len += pAPDU[3]*4+1;
		break;
	case GET_REQUEST_RECORD:
		Len += 4;
		// 1��ѡ�񷽷�
		Len1 = gdw698buf_RSD(&pAPDU[Len], &rsd, TD12);
		// 2��ѡ������
		Len1 += objGetDataLen(dt_RCSD, &pAPDU[Len + Len1], MAX_NUM_NEST);
//		 Len1 = AnalyseGetRequestRecordLen( &pAPDU[Len] );
		if((Len1 == 0) || (Len1 == 0x8000))
		{
			return 0x8000;
		}
		Len += Len1;
		break;
	case GET_REQUEST_RECORDLIST:	 
	    Len += 1;
	    for( i = 0; i < pAPDU[3]; i++ )
	    {
            Len1 = AnalyseGetRequestRecordLen( &pAPDU[Len] );
            if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len += Len1;
	    }
		break;
	default:
		return 0x8000;
	}

	return Len;
}
//--------------------------------------------------
//��������:    698��ȡ����
//         
//����:      BYTE Ch[in]       ͨ��ѡ��
//         
//         BYTE *pAPDU[in]   APDUָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static WORD AnalyseSetRequestNormalLen( BYTE *pOAD )
{ 
    WORD Len,Len1;

    Len = 4;
    Len1 = GetProtocolDataLen( pOAD+Len);//��ȡData���� DataLen�Ĵ��󳤶��ں���������ж�
    if( Len1 == 0x8000 )
    {
        return 0x8000;
    }

    Len += Len1;

    return Len;
}

//--------------------------------------------------
//��������:    698��ȡ����
//         
//����:      BYTE Ch[in]       ͨ��ѡ��
//         
//         BYTE *pAPDU[in]   APDUָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static WORD AnalyseSetRequestLen( BYTE *pAPDU )
{
    WORD Len,Len1;
    BYTE i;
    
	Len = 3;

	switch(pAPDU[1])
	{		
	case SET_REQUEST_NORMAL:
		Len += AnalyseSetRequestNormalLen( (BYTE  *)&(pAPDU[3]));
		break;
	case SET_REQUEST_NORMALLIST:
	    Len += 1;
	    for( i = 0; i< pAPDU[3]; i++ )
	    {
            Len1 = AnalyseSetRequestNormalLen( (BYTE  *)&(pAPDU[Len]));
            if( Len1 == 0x8000 )
            {
                return 0x8000;
            }

            Len += Len1;
	    }
		break;
	case SET_THEN_GET_REQUEST:
	    Len += 1;
	    for( i = 0; i< pAPDU[3]; i++ )
	    {
            Len1 = AnalyseSetRequestNormalLen( (BYTE  *)&(pAPDU[Len]));
            if( Len1 == 0x8000 )
            {
                return 0x8000;
            }

            Len += Len1+5;
	    }
		break;
	default:
		break;		
	}

    return Len;
}

//--------------------------------------------------
//��������:    698�ϱ���Ӧ
//         
//����:      BYTE Ch[in]       ͨ��ѡ��
//         
//         BYTE *pAPDU[in]   APDUָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static WORD AnalyseResponseRequestLen( BYTE *pAPDU )
{
	WORD Len;

	Len = 1;
	
	switch(pAPDU[1])
	{		
	case REPORT_RESPONSE_LIST:
	case REPORT_RESPONSE_RECORD_LIST:
	    Len += 4;
	    Len += (pAPDU[3]*4);
		break;
	default:
		break;		
	}

    return Len;
}

//--------------------------------------------------
//��������:  ��ȡ698��������ʱ�����λ��
//         
//����:      BYTE *pAPDU[in]   APDUָ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static WORD AnalyseProxyRequestLen( BYTE *pAPDU )
{
	BYTE requestItemNum  = 0;
	WORD Len = 0x8000;
	
	if(pAPDU == NULL)
	{
		Len = 0x8000;
		return Len;
	}

	switch(pAPDU[1])
	{		
		case PROXY_GET_REQUEST_LIST:
			Len = 5;
			requestItemNum = pAPDU[Len++];
			for(BYTE i = 0;i<requestItemNum;i++)
			{
				Len += (1+pAPDU[Len]);		//TSA
				Len += 2;					//Server TimeOut
				Len += (1+pAPDU[Len]*4);	//OAD
			}
			break;
		case PROXY_GET_REQUEST_RECORD:	
			Len = 5;
			Len += (1+pAPDU[Len]);			//TSA
            Len += 4;                       //OAD
			Len += objGetDataLen(dt_RSD,&pAPDU[Len],MAX_NUM_NEST);
			Len += objGetDataLen(dt_RCSD,&pAPDU[Len],MAX_NUM_NEST);
			break;
		case PROXY_SET_REQUEST_LIST:
			Len = 0x8000;
			break;	
		case PROXY_SET_THEN_GET_REQUEST_LIST:
			Len = 0x8000;
			break;
		case PROXY_ACTION_REQUEST_LIST:
			Len = 0x8000;
			break;
		case PROXY_ACTION_THEN_GET_REQUEST_LIST:
			Len = 0x8000;
			break;
		case PROXY_TRANS_COMMAND_REQUEST:
			Len = 3;
			Len += 4;						//OAD
			Len += 5;						//COMDCB
			Len += 2;						//�ȴ����ܱ��ĳ�ʱʱ�䣨s��
			Len += 2;						//�ȴ������ֽڳ�ʱʱ�䣨ms��
			if(pAPDU[Len] < 0x7F)			//���ݳ���С��127
            {
                Len += (pAPDU[Len]+1);
            }
            else if(pAPDU[Len] == 0x81)		//���ȴ���128С��255
            {   
				Len += (pAPDU[Len+1]+2);
            }
            else if(pAPDU[Len] == 0x82)		//���ȴ���255
            {
                Len += (pAPDU[Len+1]<<8 + pAPDU[Len+2] + 3);
            }
			else
			{
				Len = 0x8000;
			}
			break;
		case PROXY_INNER_TRANS_COMMAND_REQUEST:
			Len = 0x8000;
			break;
		default:
			Len = 0x8000;
			break;		
	}

    return Len;
}
//--------------------------------------------------
//��������: �ݹ�APDU�����ݳ���
//         
//����:      BYTE Ch[in]       ͨ��ѡ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static WORD AnalyseDlt698ApduLen( BYTE Ch)
{
    WORD Len = 0,Len1 = 0;
    TTwoByteUnion TmpLen,TmpLen1;
    
	if( APPData[Ch].eSafeMode != eSECURITY_FAIL )//����ʧ�ܲ����д���
	{
		switch( LinkData[Ch].pAPP[0] ) //cmd
		{
		case CONNECT_REQUEST:
		    if( LinkData[Ch].pAPP[39] == 0 )//��������
		    {
                Len = 39+1;
		    }
		    else if( LinkData[Ch].pAPP[39] == 1 )//һ������
		    {
		        /*Len1 = GetProtocolDataLen((BYTE*)&LinkData[Ch].pAPP[40]);//visible-string
		        if( Len1 == 0x8000 )
		        {
                    return 0x8000;
		        }*/
		        Len1 = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[40], TmpLen.b, ePROTOCOL_TYPE ); 
                Len = 39+1+Len1+TmpLen.w;
		    }
		    else if( LinkData[Ch].pAPP[39] == 2 )//�ԳƼ���
		    {
		        Len = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[40], TmpLen.b, ePROTOCOL_TYPE );
		        Len1 = Deal_698DataLenth( (BYTE*)&LinkData[Ch].pAPP[40+Len+TmpLen.w], TmpLen1.b, ePROTOCOL_TYPE );
                Len = 39+1+Len+TmpLen.w+Len1+TmpLen1.w;//�ԳƼ���
		    }
		    else
		    {
               return 0x8000;
		    }
			break;
		case RELEASE_REQUEST:
			Len = 2;
			break;
		case GET_REQUEST:
			Len1 = AnalyseGetRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
            if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len = Len1;
			break;	
		case SET_REQUEST:
		case ACTION_REQUEST:
			Len1 = AnalyseSetRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
			if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len = Len1;
			break;		
		case REPORT_RESPONSE:
			Len1 = AnalyseResponseRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
			if( Len1 == 0x8000 )
            {
                return 0x8000;
            }
            Len = Len1;
			
            break;
        // case PROXY_REQUEST:
		// 	Len1 = AnalyseProxyRequestLen( (BYTE*)&LinkData[Ch].pAPP[0] );
		// 	if( Len1 == 0x8000 )
        //     {
        //         return 0x8000;
        //     }
        //     Len = Len1;
			
        //     break;
		default:
		    return 0x8000;
		}
	}

	return Len;
}
//--------------------------------------------------
//��������:    ��������ϱ�״̬��
//         
//����:      BYTE Ch[in]      ͨ��ѡ��
//         	
//		   BYTE *pBuf[out]	�������ݵĻ���
//
//����ֵ:     �� 
//         
//��ע����:    ���������Ƿ����ɹ�������з���
//--------------------------------------------------
WORD DealFollowReportStatus( BYTE Ch, BYTE *pBuf)
{
	BYTE *pByte;
    WORD Result;
    
    if( ReadReportFlag( eFollowStatusReportMethod ) == 0 )//�ϱ�״̬���ϱ���ʽΪ�����ϱ�
    {
		return FALSE;
    }
    
	pByte = (BYTE *)&(ReportData[Ch].Status[0]);
	if( (pByte[0] ==0x00) && (pByte[1] ==0x00) && (pByte[2] ==0x00) && (pByte[3] ==0x00) )
	{
		return FALSE;
	}

	pBuf[0] = 0x20;//�����ϱ�״̬��OAD
	pBuf[1] = 0x15;
	pBuf[2] = 0x02;
	pBuf[3] = 0x00;
	pBuf[4] = 0x01;//data
	pBuf[5] = Bit_string_698;
	pBuf[6] = 32;
	Result = api_ReadFollowReportStatus_Mode(Ch, 0, (BYTE*)&pBuf[7]);
	if( Result == 0x8000 )
	{
        return FALSE;
	}
	lib_ExchangeBit( (BYTE*)&pBuf[7], (BYTE*)&pBuf[7], 4 );
	
	return TRUE;
}
//--------------------------------------------------
//��������:    ��������ϱ�
//         
//����:      BYTE Ch[in]  ͨ��ѡ��
//         
//����ֵ:     �� 
//         
//��ע����:    ���������Ƿ����ɹ�������з���
//--------------------------------------------------
void DealFollowReport( BYTE Ch)
{
	BYTE Buf[30];
    WORD Len,Result;
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
    //����������ϱ�������ͨ������֧���ϱ�ͨ�� ����Ӹ����ϱ�
	if( (ReadReportFlag(eFollowReport) == FALSE)
	  // ||( Ch == eIR) 
	   ||(api_GetReportChannelStatus(Ch) == FALSE) )
	{
		 DLT698AddOneBuf(Ch,0x77, 00);//���00 �������ϱ�
		 return;
	}

	Len = ( MAX_APDU_SIZE -APPData[Ch].APPCurrentBufLen.w);//�ж�ʣ��buf���� ʹ����󳤶Ƚ����ж� ������13���ֽڴ������������ϱ�

	if( Len >= (13+11))//ʣ�೤�ȿɽ��е�һ�ε���֡
	{
		Buf[0] = 1;//optional
		Buf[1] = 1;//choice of sequence of  A-ResultNormal
		Buf[2] = 1;//sequence of  A-ResultNormal
		Buf[3] = 0x33;//�����ϱ�OAD
		Buf[4] = 0x20;
		Buf[5] = 0x02;
		Buf[6] = 0x01;
		Buf[7] = 0x01;//data
		Buf[8] = OAD_698;//OAD��������
		Result = GetFirstReportOad( 0, Ch, Buf+9 );//OAD
		if( Result == FALSE )
		{			
			if( DealFollowReportStatus(Ch, Buf+3) == FALSE )
			{
				DLT698AddOneBuf(Ch,0x77, 00);//���00 �������ϱ�
			}
			else
			{
				DLT698AddBuf( Ch, 0x77, Buf, (3+11) );//��Ӹ����ϱ��������ϱ������ϱ�״̬�֣�����3+11=14�ֽ�
			}
		}
		else
	    {
	    	if( DealFollowReportStatus(Ch, Buf+13) == TRUE )
	    	{
	    		Buf[2] = 2;//sequence of  A-ResultNormal	
            	DLT698AddBuf( Ch, 0x77, Buf, (13+11) );//��Ӹ����ϱ�33200201�������ϱ�״̬��
            }
            else
            {
            	DLT698AddBuf( Ch, 0x77, Buf, 13 );//��Ӹ����ϱ�33200201
            }	
	    }
	}
	else
	{
       DLT698AddOneBuf(Ch,0x77, 00);//���00 �������ϱ�
	}    
}

//--------------------------------------------------
//��������:    ����ʱ���ǩ
//         
//����:      BYTE Ch[in]  ͨ��ѡ��
//         
//����ֵ:     �� 
//         
//��ע����:
//--------------------------------------------------
void DealTimeTag( BYTE Ch)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
	if( APPData[Ch].TimeTagFlag != eNO_TimeTag )//�����ʱ���ǩ
	{
		DLT698AddOneBuf( Ch,0x77,1);//OPTIONAL ��Ϊ1
		DLT698AddBuf( Ch, 0x77, g_TimeTagBuf, 10 );//���ؿͻ����·���ʱ�䰴��ǩ
	}
	else
	{
		DLT698AddOneBuf( Ch,0x77,0);//���ʱ���ǩ
	}
}
//--------------------------------------------------
//��������:    ����ʱ���ǩ������ϱ�����
//         
//����:      BYTE Ch[in]  ͨ��ѡ��
//         
//����ֵ:     ��   
//         
//��ע����:    ��
//--------------------------------------------------
void ProcFollowReportAndTimeFlag( BYTE Ch)//�����ϱ���Ϣ��FollowReport��  OPTIONAL
{	 
	DealFollowReport( Ch );
	DealTimeTag( Ch);//���ʱ���ǩ
}

//--------------------------------------------------
//��������:    Ӧ�ò�apdu������
//         
//����:      BYTE Ch[in]  ͨ��ѡ��
//         
//����ֵ:     ��   
//         
//��ע����:    ��
//--------------------------------------------------
void ProcDlt698Apdu( BYTE Ch)
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
	if( APPData[Ch].eSafeMode != eSECURITY_FAIL )//����ʧ�ܲ����д���
	{
		switch( LinkData[Ch].pAPP[0] ) //cmd
		{
		case CONNECT_REQUEST:
			DealConnect_Request( Ch,&LinkData[Ch].pAPP[0],&APPData[Ch].pAPPBuf[0]);
			break;
		case RELEASE_REQUEST:
			DealRelease_Request(Ch);
			break;
		case GET_REQUEST:
			DealGet_Request( Ch,&LinkData[Ch].pAPP[0] );
			break;	
		case SET_REQUEST:
			DealSet_Request( Ch,&LinkData[Ch].pAPP[0] );
			break;	
		case ACTION_REQUEST:
			DealAction_Request( Ch,&LinkData[Ch].pAPP[0] );
			break;	
		case REPORT_RESPONSE:
			DealReportResponse_Request( Ch, &LinkData[Ch].pAPP[0] );
			break;
		// case PROXY_REQUEST:
		// 	DealProxy_Request( Ch, &LinkData[Ch].pAPP[0] );
		// 	break;
		case SECURITY_REQUEST:
			APPData[Ch].APPFlag = APP_ERR_DATA;
			break;		
		default:
			APPData[Ch].APPFlag = APP_ERR_DATA;
			break;
		}
		if( (APPData[Ch].APPFlag&APP_ERR_DATA) == APP_ERR_DATA )
		{
		    APPData[Ch].APPFlag = APP_HAVE_DATA;
		    APPData[Ch].APPBufLen.w = 0;
		    APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
            DLT698AddOneBuf( Ch, 0, ERROR_RESPONSE);
            DLT698AddOneBuf( Ch, 0, LinkData[Ch].pAPP[1]);
            DLT698AddOneBuf( Ch, 0x55, 1);//apdu�޷�����
		}
		else
		{
            APPData[Ch].pAPPBuf[0] |= 0x80;//�������ó���Ӧ
		}
	}	
}
//--------------------------------------------------
//��������:    ��֡ERROW ResponseӦ��
//         
//����:      BYTE Ch[in]  ͨ��ѡ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static void DealErrowResponse( BYTE Ch )
{
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
    APPData[Ch].APPFlag = APP_HAVE_DATA;
    APPData[Ch].APPBufLen.w = 0;
    APPData[Ch].APPCurrentBufLen.w = APPData[Ch].APPBufLen.w;
    DLT698AddOneBuf( Ch, 0, ERROR_RESPONSE);
    DLT698AddOneBuf( Ch, 0, LinkData[Ch].pAPP[1]);
    DLT698AddOneBuf( Ch, 0x55, 1);//apdu�޷�����
}
//--------------------------------------------------
//��������:    ��Ӧ�ò�ṹ�����ݽ��г�ʼ������
//         
//����:      BYTE Ch[in]  ͨ��ѡ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
void InitDlt698APP( BYTE Ch)
{
    BYTE BufIndex;//Ӧ�ò�֡buf ��Ҫ��ǰ���ǵ����ݳ���
    
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }

    BufIndex = (LinkData[Ch].AddressLen+11);//��ʼ��

    APPData[Ch].RequestType = LinkData[Ch].pAPP[0];//��������ģʽ

    if( APPData[Ch].eSafeMode != eNO_SECURITY )//����ǰ�ȫ����
    {
    	if( (APPData[Ch].eSafeMode == eEXPRESS_MAC) ||(APPData[Ch].eSafeMode == eSECURITY_RN) )//����ǰ�ȫ����
    	{
    		BufIndex +=12;
    	}
    	else if( APPData[Ch].eSafeMode == eSECRET_MAC )
    	{
    		BufIndex +=(12+15);//��ȫ�������� + ���Ŀ��ܳ��ֵ���ಹ����
    	}
    	else if( APPData[Ch].eSafeMode == eSECRET_TEXT )
    	{
    		BufIndex += 15;//��ȫ�������� + ���Ŀ��ܳ��ֵ���ಹ����
    	}
    }

    if( APPData[Ch].TimeTagFlag != eNO_TimeTag )//�������ʱ���ǩԤ��13�ֽڵ����ݱ�ǩ����
    {
    	BufIndex += 13;
    }
    
    BufIndex += 2;//num����Ԥ���ռ�
    if( APDUBufFlag.Request.Ch == Ch )
    {
        APDUBufFlag.Request.Flag = FALSE;//����Ӧ�ò� ˵����·���֡�ѽ���
    }
    
    APPData[Ch].pAPPBuf = AllocResponseBuf( Ch, ePROTOCO_698 );//��ʼ��Ӧ�ò�bufָ��
    APPData[Ch].APPMaxBufLen.w = (APP_APDU_BUF_SIZE - BufIndex);
}
//--------------------------------------------------
//��������:    Ӧ�ò����ݴ�����
//         
//����:      BYTE Ch[in]   ͨ��ѡ��
//         
//����ֵ:     �� 
//         
//��ע����:    ��
//--------------------------------------------------
static void ProcDlt698App( BYTE Ch )
{ 
	BYTE i = 0;
	BOOL Result = FALSE;
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
 	if( LinkData[Ch].eLinkDataFlag == eAPP_OK )//��·������׼����
 	{
 	    memset( &APPData[Ch].APPFlag, 0x00, sizeof(TDLT698APPData));//��ʼ��Ӧ�ò�ṹ��
 	    	
        if( (api_GetSysStatus( eSYS_STATUS_ID_698MASTER_AUTH ) == TRUE)
        || (api_GetSysStatus( eSYS_STATUS_ID_698TERMINAL_AUTH ) == TRUE))//��������ѽ���Ӧ������ͨѶ��ˢ�½���Ӧ������ʱ��
        {
        	for( i = 0; i < eConnectModeMaxNum; i++ )
			{
				if( i == eConnectGeneral )//һ�����벻ά������
				{
					continue;
				}
				
				if( APPConnect.ConnectInfo[i].ConnectValidTime != 0 )
				{
					APPConnect.ConnectInfo[i].ConnectValidTime = APPConnect.ConnectInfo[i].ConstConnectValidTime;
				}
			}
        }
        
		#if(SEL_ESAM_TYPE != 0)
 		APPData[Ch].eSafeMode = ProcSecurityRequestData( Ch );//�Ƚ��а�ȫ�����ж�
 		#else
		APPData[Ch].eSafeMode = eNO_SECURITY;
		#endif

	 	Result = JudgeTimeTagValid( Ch );//�������ݺ����ʱ���ǩ�ж� ���������а���ʱ���ǩ

	 	InitDlt698APP(Ch);//��Ӧ�ò�ṹ��Ƚ��г�ʼ��
//		if(Result == TRUE)
//		{
//			if( JudgeChargingPilePro(Ch) == FALSE)
//			{
//				api_InitSci( Ch );
//				return;
//			}
//		}

 		if( Result == FALSE )//����apdu���ִ���
 		{
            DealErrowResponse( Ch );
            
            ProcFollowReportAndTimeFlag( Ch);//���ʱ���ǩ�������ϱ�
            
            ProcSecurityResponseData( Ch);//��ȫ���ش���
    
            ProFollowJudge(Ch);//��·���֡�ж�
 		}
 		else
 		{            
            ProcDlt698Apdu(Ch);//����apdu�Ĵ���
            
            ProcFollowReportAndTimeFlag( Ch);//���ʱ���ǩ�������ϱ�
            
            ProcSecurityResponseData( Ch);//��ȫ���ش���
    
            ProFollowJudge(Ch);//��·���֡�ж�
 		}
 	}
}

#endif//(SEL_DLT698_2016_FUNC == YES )
