//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���ĺ� κ����
//��������	2016.8.4
//����		DL/T 698.45�������Э��apiͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __Dlt698_45_API_H
#define __Dlt698_45_API_H

#if(SEL_DLT698_2016_FUNC == YES  )
//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define MAX_RN_SIZE				    (24+1) //�������󳤶�       3*8+1=25
//����غ궨����Ҫ�ⲿ����
/***************************DAR����*************************************/
#define	DAR_Success			 	0	//�ɹ�
#define	DAR_HardWare			1	//Ӳ��ʧЧ  //�����ڹ�Լ������ȷ������API��������ȷ������� ��������ʹ��
#define	DAR_TempFail			2	//��ʱʧЧ
#define	DAR_RefuseOp			3	//�ܾ�����
#define	DAR_Undefined			4	//����δ����
#define	DAR_ClassId				5	//����ӿ��಻����
#define	DAR_NoObject			6	//���󲻴���
#define	DAR_WrongType			7	//���Ͳ�ƥ��
#define	DAR_OverRegion			8	//Խ��
#define	DAR_DataBlock			9	//���ݿ鲻����
#define	DAR_CancelDiv			10	//��֡������ȡ��
#define	DAR_NotInDiv			11	//�����ڷ�֡����״̬
#define	DAR_CancelBlockWr		12	//��дȡ��
#define	DAR_NoInBlockWr			13	//�����ڿ�д״̬
#define	DAR_BlockSeq			14	//���ݿ������Ч
#define	DAR_PassWord			15	//�����/δ��Ȩ
#define	DAR_ChangeBaud			16	//ͨ�����ʲ��ܸ���
#define	DAR_OverZone			17	//��ʱ������
#define	DAR_OverSlot			18	//��ʱ������
#define	DAR_OverRAte			19	//��������
#define	DAR_MatchAuth			20	//��ȫ��֤��ƥ��
#define	DAR_Recharge			21	//�ظ���ֵ
#define	DAR_EsamFial			22	//ESAM��֤ʧ��
#define	DAR_AuthFail			23	//��ȫ��֤ʧ��
#define	DAR_CustomNo			24	//�ͻ���Ų�ƥ��
#define	DAR_ChargeTimes			25	//��ֵ��������
#define	DAR_OverHold			26	//���糬�ڻ�
#define	DAR_WrongAddr			27	//��ַ�쳣
#define	DAR_Symmetry			28	//�Գƽ��ܴ���
#define	DAR_Asymmetry			29	//�ǶԳƽ��ܴ���
#define	DAR_signature			30	//ǩ������
#define	DAR_HangUp				31	//���ܱ����
#define	DAR_TimeStamp			32	//ʱ���ǩ��Ч
#define	DAR_OverTime			33	//esam��P1P2����ȷ
#define	DAR_P1P2ERROW			34	//����ʱ
#define	DAR_LCERROW				35	//����ʱ
#define	DAR_OtherErr			255	//��������
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum
{
   	eData = 0,         	//ֻ��ȡ����
   	eTagData,    		//����+TAG
   	eAddTag,        	//ֻ��TAG
   	eProtocolData,      //Э������ 
   	eMaxData = 0X55,    //���Դ���ݳ���
}eDataTagChoice;//���ݼ�TAG��־

typedef enum
{
   	eFREEZE_TIME_FLAG = 0x01, 		//ʱ���־
   	eRECORD_TIME_INTERVAL_FLAG = 0x02, //ʱ�������־
   	eNUM_FLAG = 0x04,    				//������־
   	eEVENT_START_TIME_FLAG = 0x08,		//�¼���ʼʱ��
   	eEVENT_END_TIME_FLAG = 0x10,		//�¼�����ʱ��
}eRecoredFlagChoice;

typedef struct TTimeIntervalPara_t	//��ȡ��¼�����ṹ��
{
    DWORD StartTime;				//��ʼʱ��
    DWORD EndTime;					//����ʱ��
    WORD  TI;						//ʱ����
    WORD  ReturnFreezeNum;			//������������������ֵ��
    BYTE  AllCycle;                 //ȫ���ڱ�־ 0X55����ȫ���� 0x00 ��ȫ����
}TTimeIntervalPara;

typedef struct TDLT698RecordPara_t			//��ȡ��¼�����ṹ��
{
	WORD OI;								//���ͱ�ʶOI
	BYTE Ch;                                //��ȡͨ��
	ePHASE_TYPE Phase;						//ָ������ 0 ������ 1 ����A��  2 ����B��  3 ���� C��
	eRecoredFlagChoice eTimeOrLastFlag; 	//ʱ�������ı�־  0x00 ʱ��  0x01 ����	
	DWORD TimeOrLast;						//��ȡ�¼����¼��ʱ��
	TTimeIntervalPara TimeIntervalPara;     //ʱ��β���
	BYTE OADNum;							//Ҫ��ȡ���������OAD�ĸ��� OAD����Ϊ�� �򷵻�ȫ��
	BYTE *pOAD;								//Ҫ��ȡ���������OADָ��
}TDLT698RecordPara;

typedef enum
{
   	eConnectGeneral = 0, 	//һ�㽨��Ӧ������ ---ֵ����������-jwh
	eConnectMaster,  		//��վ����Ӧ������
	eConnectTerminal,		//�ն˽���Ӧ������
	eConnectModeMaxNum,		//������
}eConnectMode;//���ݼ�TAG��־

typedef enum
{
   	eGetNormalData = 0, 	//��ȡ��������
   	eGetRecordData, 		//��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
   							//���ص��ܵ�����������Ϊ5���ֽ� �洢����Ͷ�ȡ�������ݣ��ж϶�������ʱʹ�ã�����жϷ������ݳ���Ӧ��Normal
}eGetDataType;//���ݼ�TAG��־

typedef struct TConnectInfo_t   	//��·����Ӧ�ṹ��
{
	DWORD ConnectValidTime;				//698����Ӧ������ʱ��(ʵʱ)
	DWORD ConstConnectValidTime;	 	//698����Ӧ�ù̶�����ʱ��
}TConnectInfot;

typedef struct TDLT698APPConnect_t   	//��·����Ӧ�ṹ��
{
	TConnectInfot ConnectInfo[eConnectModeMaxNum];
	DWORD IRAuthTime;			        //������֤ʱ��(ʵʱ)
	DWORD TerminalAuthTime;				//�ն���֤ʱ��
}TDLT698APPConnect;
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern TSafeModePara SafeModePara;
extern TDLT698APPConnect APPConnect;
extern BYTE g_EnableAutoReportFlag;//ֻΪӦ�����Ų����������
extern __no_init BYTE g_PassWordErrCounter;//����������
extern BYTE g_byClrTimer_500ms;
//-----------------------------------------------
// 				��������
//-----------------------------------------------


//--------------------------------------------------
//��������:  698��ȡ����OAD���ݵĶ���ӿں���
//         
//����:	
//		DataType[in]:	eData/eTagData/eAddTag
//		Dot[in]:  		��ȡ���ݵ�С����λ����oxff--��ʾ��ԼĬ�ϵ�С��λ������Լ���ã�
//		pOAD[in]��		��ȡ���ݵ�OAD
//		InBuf[in]��		����Ҫ��Tag������(ע:������¼����е� �¼��ϱ�״̬(0x3300)����̶����б�(0x3302)���¼������б�(0x330c)ʱ����һ���ֽ�InBuf[0]��Ҫ���Array�ĸ���Number)
//		OutBufLen[in]��	����OutBufLen�ĳ���
//		OutBuf[out]:  	��ȡ���ݴ��OutBuf��
//
//����ֵ: �������ݳ���	0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD api_GetProOadData( eGetDataType GetDataType, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//��������: ��ȡ����OAD��Ӧ���ݵĳ���
//         
//����:    	GetDataType[in]	GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//			DataType[in]: 	eData/eTagData
//         	pOAD[in]����ȡ���ݳ��ȵ�OAD
//			Number[in]��Array�ĸ���  
//
//����ֵ:   ���ݳ���  0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����: Number:ֻ�л�ȡ�¼��೤��ʱ��Ҫ���ô˲�������ȡ�����೤��ʱ���˲���ֱ����д0
//--------------------------------------------------
WORD api_GetProOADLen( eGetDataType GetDataType, BYTE DataType, BYTE *pOAD, BYTE Number);

//--------------------------------------------------
//��������:  ��Լ��ȡ��¼����
//         
//����:      BYTE Tag[in] 						//�Ƿ��TAG��־;	0x01������Ҫ��TAG�� 	   	  0x00 ������TAG�� 
//							       
//           TDLT698RecordPara *DLT698RecordPara[in] 	//��ȡ��¼���¼��Ĳ����ṹ��
//         
//           WORD InBufLen[in]						//�����buf���� �����ж�buf�����Ƿ���
//         
//           BYTE *InBuf[in]						//����bufָ��
//         
//����ֵ:    WORD								// bit15λ��1 �����¼�򶳽��޴����� ��0������������ȷ���أ� 
//											bit0~bit14 �����صĶ�����¼����ֽ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD api_ReadProRecordData( BYTE Tag, TDLT698RecordPara *pDLT698RecordPara, WORD BufLen,BYTE *pBuf );

//-----------------------------------------------
//��������: ��¼�е�����ת��
//
//����:
//		pOAD[in]: ����OAD
//
//		Len[in]: ����bufg����
//
//		Buf[in]:ָ����ܵ�ָ�� ����hex
//
//		OutBufLen[in] ������ݳ���
//
//		OutBuf[in]	���buf
//
//����ֵ:
//
//��ע: ���ܽ����ݷ�д��Buf�У���Ϊ����ĳ��Ȳ������ܻᳬ�ޣ�ֱ�ӻ�дoutbuf---jwh
//-----------------------------------------------
WORD api_RecordDataTransform( BYTE* pOAD, WORD* BufLen , BYTE* Buf, WORD OutBufLen,BYTE* OutBuf );

void api_ClrProSafeModePara(BYTE byType);//������ʽ��ȫģʽ����

//--------------------------------------------------
//��������:  �ж�ʱ�α����Ƿ����15����
//         
//����  :   BYTE *pBuf[in] ʱ�α�BUFָ��
//
//����ֵ:    WORD TRUE ��Ч  FALSE ��Ч 
//
//��ע����:  ��������ն�ȡʧ�ܣ�Ĭ�Ϸ��سɹ�
//--------------------------------------------------
WORD api_JudgeTimeSegInterval(BYTE *pBuf);
//--------------------------------------------------
//��������:       �жϹ㲥Уʱ�Ƿ����ڽ���ʱ��ǰ��5����
//         
//����  :       ��
//
//����ֵ:        TRUE--����5min    
//			  FALSE--5min����
//��ע����: 
//--------------------------------------------------
BOOL JudgeBroadcastMeterTime( WORD Sec );
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
WORD api_GetMeterStatus( ePROTOCO_TYPE ProtocolType, BYTE Type, WORD BufLen, BYTE *Buf );
//--------------------------------------------------
//��������:�Ͽ�Ӧ������
//         
//����  :   ��
//
//����ֵ:    ��   
//         
//��ע����:  ��
//--------------------------------------------------
void api_Release698_Connect( void );
//--------------------------------------------------
//��������:   	���������ϱ��¼��б�
//         
//����: 
//			OutBufLen[out]��	buf����    
//         	OutBuf[out]��
//����ֵ:  	�������ݳ���
//         
//��ע����: ������ class7������2����class24������6~9��
//--------------------------------------------------
WORD AddActiveReportList(WORD OutBufLen, BYTE *OutBuf );
//--------------------------------------------------
//��������:  ��·��������Ӧ����
//         
//����:     pAPDU[IN]	 //apdu ָ��
//		  APDU_Len[IN]//apdu ���� ������м���ֵ�ж�
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void api_ActiveReportResponse(BYTE* pAPDU, WORD APDU_Len ,BYTE Type);

//--------------------------------------------------
//��������: ��ʱ�β��������ʱ����ʱ�������һ����Ч�Ĳ���
//         
//����:    BYTE byLen[in]            �������ݳ���
//         
//         BYTE byMaxLen[in]         ������ݳ���
//
//         BYTE *pData[in]           ����ָ��
//         
//����ֵ:     BYTE DAR��� 
//         
//��ע����:    ������ȫ�ֱ��� ProtocolBuf ��
//--------------------------------------------------
void api_AddTimeSegParaExtDataWithLastValid(BYTE byLen, BYTE byMaxLen, BYTE *pBuf );
//--------------------------------------------------
// ��������:  �����Ӷ�������ʱ�����ڴ����������ȷ������Ҫ��ȡʱ���м���
//
// ����:      [in]��Num
//
// ����ֵ:    ��
//
// ��ע����:  ��
//--------------------------------------------------
void api_SetTagArrayNum(BYTE Num);
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
WORD api_DealMoneyDataToESAM( DWORD Money, DWORD BuyTimes, BYTE *pCustomCode, BYTE *pSIDMAC );
#endif//#if( PREPAY_MODE == PREPAY_LOCAL)

#endif//(SEL_DLT698_2016_FUNC == YES )
#endif//__DLT698_45_2016_API_H