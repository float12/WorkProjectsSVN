/*****************************************************************************/
/*�ļ�����Protocol.h
 *Copyright (c) 2008 ��̨������˼�ٵ������޹�˾������Ʒ������
 *All rights reserved
 *
 *�����ˣ�κ����
 *��  �ڣ�2008-9-21
 *�޸��ˣ�κ����
 *��  �ڣ�2009-3-26
 *��  ����DTSD/DSSD178ȫ����ʽ�๦�ܵ��ܱ�ͨѶ��Լͷ�ļ�
 *
 *��  ����
 *˵  ����������������������ɹ�LCD���¼������������ߡ�������ã�ֻҪ�����ǹ�Լ��ʽ�����Ե��ô˺���
 *		  ���������ڲ����� BYTE *ProBuf����ProBuf�ǹ�Լ����TSerial�е�ProBuf��
 *		  ���������ڲ����� BYTE *pBuf���� pBufȡ���ڵ���ʱ���������
 */
/*****************************************************************************/

#ifndef __PROTOCOL_INCLUDE
#define __PROTOCOL_INCLUDE

	//ͨ�Ż�������󳤶ȶ���
	#define	MAX_PRO_BUF			(512)	//֡���� ͨѶ����apdu�����ߴ�
	#define EXTRA_BUF			(30)	//���������buf ������ֹ����bufʹ��
	#define MAX_APDU_SIZE		2000   //apdu�����ߴ�  
	#define MAX_RECE_DATA		(MAX_PRO_BUF)
	#define MAX_SEND_DATA		MAX_PRO_BUF

	#define MAX_PRO_BUF_REAL	(8192)


	//���ܳ�ʱʱ�䣬��λ�����룬Ҳ������100�������û�н��ܵ��µ����ݣ�������ܻ���, �ѷŵ�������
	//#define MAX_RX_OVER_TIME	100
	//���͵ȴ�ʱ�䣬��λ���룬�ڵȴ�������л�������״̬, �ѷŵ�������
	//#define MAX_TX_WAIT_TIME	40//20  ���Ѳ���
	//485���ż��ʱ�䣬��λ���룬�յ������źţ���û����Ч���ݣ��ڴ�ʱ�����л���232״̬, �ѷŵ�������
	//#define WATCH_485_TIME		100  ���Ѳ���
	//232��ֹ���485����ʱ����λ����, �ѷŵ�������
	//#define MAX_232_TIME		30000   ���Ѳ���
	//���յ����͵���ʱ������Ϊ��λ�����յ���Ч֡����ʱ��ʱ���ڷ��ͣ���Ϊ�������ķ�ʱ�䣩, �ѷŵ�������
	//#define RECE_TO_SEND_DELAY 	25
	#define CR_OVER_TIME		1440
	#if(CR_OVER_TIME < 120)
	#error"���ô���"
	#endif
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

typedef struct TSerial_t
{

	// //�������壬��RAM�б�����ż��ַ
	// BYTE ProBuf[MAX_PRO_BUF+EXTRA_BUF];

	//�������壬��RAM�б�����ż��ַ
	BYTE ProBuf[MAX_PRO_BUF_REAL+EXTRA_BUF];

	WORD ProStepDlt645;
	//Dlt645��Լ������Serial[].ProBuf�еĿ�ʼλ��
	WORD BeginPosDlt645;

	//645�������������ݳ���
	WORD Dlt645DataLen;

	#if(SEL_DLT698_2016_FUNC == YES )
	//DLT698_45��Լ���չ��̼���
	WORD ProStepDlt698_45;
	//DLT698_45��Լ������Serial[].ProBuf�еĿ�ʼλ��
	WORD BeginPosDlt698_45;
	WORD wLen;
	BYTE Addr_Len;//ͨ�ű����з�������ַSA���ȣ���ַ����
	#endif
	
#if (SEL_1376_2_FUNC == YES)
	WORD ProStep3762;
	WORD BeginPos3762;
	WORD w3762Len;
#endif
	WORD RXWPoint;
	WORD RXRPoint;

	WORD TXPoint;
	WORD SendLength;//��Ҫ���͵����ݳ���

	//���ڼ���ʱ��
	BYTE WatchSciTimer;

	BYTE BroadCastFlag;//����Ӧ���־

	BYTE BeginPoint;//��ЧֵΪ0xff��ÿ�������0xff

	//��485ͨѶ�ļ���ʱ�䴦��ȫ�����ô�ѭ��������ʽ
	//���ܳ�ʱʱ�䣬���յ����һ���ֽں󣬵ȴ���ʱ��û���µ������յ��������ǽ��ܵ��������ݣ���Ҫ��ʼ��ָ�롣
	WORD RxOverCount;

	//���յ����͵���ʱ
	WORD ReceToSendDelay;

	BYTE byLastRXDCon;//�ϴ�ͨ�ŵ��Ӧ�������
	
	//bit0 ��Ӧͨ�����޸Ĳ����ʵĲ���
	BYTE OperationFlag;

	//���͵����͵���ʱ���ƣ������˿�ʹ�ã�
	WORD SendToSendDelay;
}TSerial;

//645�ṹ
typedef struct TDlt645Head_t
{
	BYTE Sync1;
	BYTE ID[6];
	BYTE Sync2;
	BYTE Control;
	BYTE Length;
}TDlt645Head;

//698.45��ȫе
typedef struct  TSafeModeParaSub_t
{
	WORD OI;//����OIΪ0x0010 ���ֽڱ���:0x10 ���ֽڱ���:0x00 wlk
	WORD wSafeMode;//����wSafeModeΪ0x8000�����ֽڱ���:0x00,���ֽڱ���0x80 wlk
}TSafeModeParaSub;

#pragma pack(1)
typedef struct TSafeModePara_t
{
	BYTE Num;		                         		//��ʾ��ȫģʽ��������num
	BYTE Flag;   	                            	//��ȫģʽ����flag:0-������ 1-����
	TSafeModeParaSub Sub[MAX_SAFE_MODE_PARA_NUM];
	DWORD CRC32;              						// У��
}TSafeModePara;

typedef struct TTimeBroadCastPara_t
{
	WORD TimeMinLimit;		                         	//�㲥Уʱ��С��Ӧ���� ��λ����
	WORD TimeMaxLimit;									//�㲥Уʱ�����Ӧ���� ��λ����
	DWORD CRC32;              							// У��
}TTimeBroadCastPara;

typedef struct
{
	BOOL AllowUnknownAddrTrans;	// ����δ֪��ַ͸����485I
	BYTE Baud;					// ת����485�˿�ʹ�õĲ�����
	BYTE Timeout;				// ͸����ʱʱ������(s)
	DWORD CRC32;				// У��
}MessageTransPara_t;

//Э������Ҫ���浽EEPROM�Ĳ���
typedef struct TProtocol_Para_t
{
	#if ( SEL_DLT698_2016_FUNC == YES  )
	TSafeModePara SafeModePara;
	TTimeBroadCastPara TimeBroadCastPara;
	#else
	BYTE byNoUse;	
	#endif
	TCanBusPara CanBusPara;
	MessageTransPara_t MessageTransPara;
}TProSafeRom;
#pragma pack()

typedef enum
{
	eCLEAR_METER   = 0x1111,		//�������
	eCLEAR_FACTORY = 0x2222,		//����ʼ��
	eCLEAR_EVENT   = 0x4444,		//�¼�����
	eCLEAR_MONEY   = 0x6666,		//Ǯ����ʼ�������Ԥ�ÿ�
	eCLEAR_PARA    = 0x7777,		//��ʼ������������ʼ�����ᡢ�¼�����ʾ�б�
	eCLEAR_DEMAND  = 0x8888,		//��������
}eCLEAR_METER_TYPE;

typedef enum
{
	ePROTOCO_698 = 0,   //698Э��
	ePROTOCO_645,       //645Э��
}ePROTOCO_TYPE;

extern TSerial Serial[MAX_COM_CHANNEL_NUM];

extern BYTE Reply;//��ԼͨѶ�Ƿ�ҪӦ�� TRUE--ҪӦ�� FALSE--��ҪӦ��
extern BYTE g_byUsePasswordP2;//P2=01H:�����򣺿�������ִ��״̬�� P2��01H�����������ǰ����Ӧ��
extern WORD g_wRelayReplyStauts;//��������Ӧ��״̬��
extern WORD FactoryTime;//�򿪳���ģʽʱ��
extern WORD LowFactoryTime;//�򿪵͵ȼ�����ģʽʱ��

extern BYTE APDUResponseBuf[MAX_COM_CHANNEL_NUM][MAX_APDU_SIZE+EXTRA_BUF];
extern BYTE bBleChannel;
extern DWORD JTOverTime;//��̬��ʱʱ��
extern MessageTransPara_t MessageTransParaRam;
//���ϰ幫������
void InitPoint(TSerial * p);
void api_InitSubProtocol(WORD);
//-----------------------------------------------
//��������  :   ���ڳ�ʼ��Э�����
//����  : 	��
//����ֵ: 
//��ע����  :  ��
//-----------------------------------------------
void FactoryInitProtocol( void );


//�°��ʼ������ͨѶָ��
void api_InitSciStatus( BYTE i );
//�򿪽��գ�ʹ���ڽ���״̬
void api_EnableRece(TSerial * p);
//��ֹ����
void api_DisableRece(TSerial * p);

BYTE CalRXD_CheckSum(WORD ProtocolType, TSerial * p);
//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ����
void DoReceMoveData(WORD ProtocolType, TSerial * p);
//645��Լ������
WORD DoReceProc_Dlt645(BYTE ReceByte, TSerial * p);


//ȡ�û�����TSerial�е��ĸ�����
//����: FALSE--���Ǵ��ڻ��� ����--���ص��Ǵ���Serial[]�ĵ�ַ
TSerial *GetSerial(BYTE *pBuf);

//---------------------------------------------------------------
//��������: �жϽ��ձ����е�ͨ�ŵ�ַ�Ƿ�ȫ���Ǹ���������
//
//����:    bySpecialData[in]:
//			99--�ж��·������Ƿ�㲥��ַ
//			88--�ж��·������Ƿ��Ƕ�����չ��6��88����12��8
//        pBuf[in] - ����ͨ�ŵ�ַ
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
WORD JudgeRecMeterNoOnlyOneData(BYTE bySpecialData,BYTE *pBuf);


//#if( SEL_DLT645_2007 == YES )
//�����¹�Լ���� ��645 ��645(DL/T645-2007) ��  �й�
void InitDlt645_2007(void);


WORD JudgeRecMeterNo(BYTE *pBuf);
void Proc645( WORD i );
void api_ProtocolTask(void);
void api_PowerUpProtocol( void );

//--------------------------------------------------
//��������:     �����������Buf
//
//����  :     BYTE Ch[in]                     ͨ��ѡ��
//          ePROTOCO_TYPE ProtocolType[in]  Э������
//
//����ֵ:      BYTE* bufָ��
//
//��ע����:  ��
//--------------------------------------------------
BYTE* AllocResponseBuf( BYTE Ch, ePROTOCO_TYPE ProtocolType );

//-----------------------------------------------
//��������: ��չ��Լ������
//
//����:		pBuf[in]	0--��չDI0 1--DI1 2--0XDF 3--0XDB	
//						
//����ֵ:	���ض�ȡ���ݳ���
//		   
//��ע:��Լ����������ƽ̨��չ��Լ��
//-----------------------------------------------
WORD ReadFactoryExtPro(BYTE *pBuf);

//-----------------------------------------------
//��������: ��չ��Լд����
//
//����:		pBuf[in]	0--��չDI0 1--DI1 2--0XDF 3--0XDB	
//						4,5,6,7--��Է�����
//����ֵ:	����ֵbit15Ϊ1Ϊ��������Ϊ��ȷ
//		   
//��ע:��Լ����������ƽ̨��չ��Լ��
//-----------------------------------------------
WORD WriteFactoryExtPro(BYTE *pBuf);

//---------------------------------------------------------------
//��������: ���������־
//
//����: 
//			eCLEAR_METER_TYPE - �����־λ
// 					eCLEAR_METER   = 0x1111,	//�������
//					eCLEAR_FACTORY = 0x2222		//����ʼ��
//					eCLEAR_EVENT   = 0x4444,	//�¼�����
//					eCLEAR_MONEY   = 0x6666,	//Ǯ����ʼ�������Ԥ�ÿ�
//					eCLEAR_DEMAND  = 0x8888,	//��������
// 			BYTE DelayTime_500ms - ������ʱʱ�� 500msΪ��λ������Ϊ1��ʾ��ʱ500ms
//                   
//����ֵ:   TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL api_SetClearFlag( eCLEAR_METER_TYPE ClearType, BYTE DelayTime_500ms );


//--------------------------------------------------
//��������:     4Gģ�鸴λ
//
//����  :     
//
//����ֵ:      
//
//��ע����:  ��
//--------------------------------------------------
void api_ResetModule4g( void );
#endif//��Ӧ�ļ���ǰ��� #ifndef __PROTOCOL_INCLUDE


