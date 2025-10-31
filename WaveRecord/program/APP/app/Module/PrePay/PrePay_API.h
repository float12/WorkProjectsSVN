//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.16
//����		Ԥ���� Apiͷ�ļ�
//�޸ļ�¼	

//
//----------------------------------------------------------------------
#ifndef __PREPAY_API_H
#define __PREPAY_API_H
//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//��ʾ��ҲҪ�øĶ���
#define		OK_CARD_PROC		0xff	//��Ƭ�����ɹ�
#if( PREPAY_STANDARD == PREPAY_NANWANG_15 )//����
	#define		USER_CARD_COMMAND			0x03		//�û���������
	#define		PRESET_CARD_COMMAND			0x02		//Ԥ�ƿ�������
#else//( PREPAY_STANDARD == GUO_WANG_STANDARD )//����
	#define		USER_CARD_COMMAND			0x01		//�û���������
	#define		PRESET_CARD_COMMAND			0x06		//Ԥ�ƿ�������
#endif

#define	REMOTE_MODE				1				//Զ��ģʽ
#define	LOCAL_MODE				0				//����ģʽ
#define LADDER_MONTH_MODE		0x00			//�½���ģʽ
#define LADDER_YEAR_MODE		0x55			//�����ģʽ

#define READ_METER_RN	0x01		//����+RN���������� / ����+ MAC�ظ�

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
//����״̬
typedef enum
{
	eNoOpenAccount = 0,	    //δ����״̬
	eOpenAccount = 0x55,	//�ѿ���״̬
}eCardFlag;

//����״̬
typedef enum
{
	eNoAlarm = 0,	//����״̬
	eAlarm1,     	//���ڱ������1״̬
	eAlarm2,     	//���ڱ������2״̬
	eOvrZero,    	//���ڹ���״̬
	eOvrTick,    	//����͸֧״̬
}eLOCAL_STATUS;

typedef enum 
{
	eReturnMoneyMode = 0,		//�ۼ����˷Ѳ���
	eChargeMoneyMode,	        //��ֵ����
	eSetParaMode,				//���Ĳ�������
	eInitMoneyMode,	            //Ԥ�Ʋ���
}eUpdataMoneyMode;

//��۱仯��־
typedef enum
{
	eNoUpdatePrice = 0,			//����ˢ�µ��
	ePriceChangeEnergy,         //���δ�����仯 ����Ҫˢ�µ�ǰ���
	eChangeCurRatePrice,	    //�ı��˵�ǰ���ʵ��
	eChangeCurLadderPrice,		//�ı��˵�ǰ���ݵ��
}eChangeCurPriceFlag;

typedef enum
{
	eInfoCurLadder,				//��ǰ��������
	eInfoBackupRateFalg,		//�Ƿ��з��ʵ�۱�����
	eInfoBackupLadderFlag,		//�Ƿ��н��ݵ�۱�����
}eLocalInfo;

typedef enum
{
	GENERATE_RN = 0,		    //���������
	VERIFY_MAC,	                //����+RN������֤MAC
}eProcessMacRN;


//Ԥ���Ѳ���
typedef enum//W-��д R-�ɶ�
{
	eKeyNum = 0,                                    //��Կ����-WR
	eCTCoe,                                         //CT���-WR
	ePTCoe,                                         //PT���-WR
	eIRTime,                                        //������֤ʱ��-WR
	eRemoteTime,									//�����֤ʱ��-WR(Э�鲻�ɶ�)
#if( PREPAY_MODE == PREPAY_LOCAL)
	eLadderNum,										//������-WR			
	eTickLimit,										//͸֧�������-WR     
	eRegrateLimit,                                  //�ڻ��������-WR     
	eCloseLimit,                                    //��բ����������-WR  
	eAlarm_Limit1,									//�������1-WR  
	eAlarm_Limit2,                                  //�������2-WR  
	eCurPice,										//��ǰ���-R   
	eCurRatePrice,                                  //��ǰ���ʵ��-R
	eCurLadderPrice,                                //��ǰ���ݵ��-R
	eCurRateTable,									//��ǰ�׷��ʱ�-WR(Ԥ�ÿ��Ϳ�������д)
	eBackupRateTable = (eCurRateTable + 0x80),		//�����׷��ʱ�-WR	
	eCurLadderTable,								//��ǰ�׽��ݱ�-WR(Ԥ�ÿ��Ϳ�������д)
	eBackupLadderTable = (eCurLadderTable + 0x80),	//�����׽��ݱ�-WR 
	eRemainMoney,									//ʣ����-R
	eBuyTimes,										//�������-R
	eLocalAccountFlag,								//���ؿ���״̬-WR
	eRemoteAccountFlag,                             //Զ�̿���״̬-WR
	eSerialNo,										//�����к�-WR
	eIllegalCardNum,								//�Ƿ��忨����-WR
	eLadderUseEnergy,								//��ǰ���ݽ����õ���
#endif//#if( PREPAY_MODE == PREPAY_LOCAL)
}ePrePayParaType;

#if( PREPAY_MODE == PREPAY_LOCAL)
typedef struct TLocalStatus_t
{
	eLOCAL_STATUS  CurLocalStatus;     //��ǰ����״̬
	eLOCAL_STATUS  LastLocalStatus;    //�ϴα���״̬ ���û�����ά����������
	DWORD          CRC32;
}TLocalStatus;

//�˽ṹ��Ҫpack��������8����
#pragma pack(1)
typedef struct Tmoney_t
{
	TEightByteUnion  Remain;    //ʣ����
	DWORD BuyTimes;             //�������
}TMoney;

typedef struct TLadderUserEnergy_t
{
	DWORD ForwardEnergy;    //��������õ���
	DWORD BackwardEnergy;   //��������õ���
}TLadderUserEnergy;

//TMoney����pack��1����CRC32���������4���ֽڣ�����У��ʱ�ͳ�����
typedef struct TCurMoneyBag_t 
{
    TMoney  			Money;              //ʣ����
	TLadderUserEnergy	UserEnergy;          //�õ���
	DWORD          CRC32; 
}TCurMoneyBag;

typedef struct TRatePrice_t
{
	DWORD Price[MAX_RATIO];     //���ʵ��
	DWORD CRC32;                //У��
}TRatePrice;
typedef struct TLadderSavePara_t
{
	//hex�� ������� �����ר��
	BYTE Mon;
	//hex�룬��Ϊ0xFF��Ч��
	BYTE Day;
	//����Сʱ hex�룬��ΪFF��Ч
	BYTE Hour;
	
}TLadderSavePara;
typedef struct TLadderBillingPara_t
{
	//���ݽ������Ϊ4
	TLadderSavePara LadderSavePara[MAX_YEAR_BILL];
	
}TLadderBillingPara;

typedef struct TLadderPrice_t //����74���ֽ�
{
	DWORD Ladder_Dl[MAX_LADDER];        //���ݵ���
	DWORD Price[MAX_LADDER + 1];        //���ݵ��
	BYTE  YearBill[MAX_YEAR_BILL][3];   //�������	//@@@@������ո�������ṹ��
	DWORD CRC32;                        //У��
}TLadderPrice;

typedef struct TBackupYearBill_t
{
	BYTE  YearBill[MAX_YEAR_BILL][3];//����������� �����жϽ����л�ʱ�������Ƿ�ı�
	DWORD CRC32;//У��
}TBackupYearBill;

//��۲���
typedef struct TPricePara_t
{
	TRatePrice CurrentRate;         //��ǰ�׷���
	TLadderPrice CurrentLadder;     //��ǰ�׽���
	TRatePrice BackupRate;        	//�����׷���
	TLadderPrice BackupLadder;      //�����׽���
	DWORD		   CRC32;
}TPricePara;

typedef struct TCardInfo_t
{
    BYTE LocalFlag;     //���ؿ�����־
    BYTE RemoteFlag;    //Զ�̿�����־
    BYTE SerialNo[10];  //�����к�
	TFourByteUnion IllegalCardNum;    //�Ƿ��忨�ܴ���
    DWORD CRC32;                    //У��
}TCardInfo;

typedef struct TsafeLimitMoney_t
{
	TFourByteUnion TickLimit;		//͸֧�������
	TFourByteUnion RegrateLimit;	//�ڻ��������
	TFourByteUnion CloseLimit;		//��բ����������
	TFourByteUnion Alarm_Limit1;	//�������1
	TFourByteUnion Alarm_Limit2;	//�������2		
	DWORD		   CRC32;
}TLimitMoney;

#endif

typedef struct TPrePayPara_t
{
	//��Կ����
	BYTE ScretKeyNum;
	//������
	BYTE LadderNum;   	
	// CT���
	DWORD CTCoe;
	// PT���
	DWORD PTCoe;
	//������֤ʱ��
	DWORD IRTime;
	//Զ��645�����֤ʱЧ
	WORD Remote645AuthTime;
	//У��
	DWORD CRC32;
	
}TPrePayPara;

typedef struct TPrePaySafeRom_t
{
    TPrePayPara 			PrePayPara;
    #if( PREPAY_MODE == PREPAY_LOCAL)
    TCurMoneyBag    		CurMoneyBag;		//ʣ�������ѿ۷ѵ���
	TLimitMoney             LimitMoney;         //������޲��� - ͸֧���ڻ�����բ�����������1��2
	TPricePara              Price;              //��ǰ�׺ͱ����׵���ļ�
	TBackupYearBill 		BackupYearBill; 	//����������� �����жϽ����л�ʱ�������Ƿ�ı�	
	TLocalStatus            LocalStatus;        //����״̬
	TCardInfo               CardInfo;           //����Ϣ������״̬�������кţ�
	#endif
}TPrePaySafeRom;


// �������ռ��е�Ԥ������Ϣ
typedef struct TPrePayConMem_t
{
	#if( PREPAY_MODE == PREPAY_LOCAL )
	DWORD dwIllegalCardNum;	
	DWORD dwTotalBuyMoney;		//�ۼƹ����� Ҫ��Ҫ���� TMoney or TCurMoneyBag ��wlk
	#else
	BYTE byTemp;
	#endif
}TPrePayConMem;
#pragma pack()

typedef struct 
{
    BYTE *rand;				//���������+�����
	BYTE *apduMacBuf;		//APDU+MAC buf
	BYTE mailAddr[6];		//ͨ�ŵ�ַ
    WORD apduMacBufLen; 	//APDU+MAC�ĳ���
}verifyMacInfo_t;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern BYTE    s_ReadMeterMAC[];
//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������:  �������ͷ����
//         
//����:      BYTE* pbuf[in]       
//           BYTE inc[in]        
//           BYTE com[in]        
//           BYTE P1[in]       
//           BYTE P2[in]       
//           BYTE len1[in]        
//           BYTE len2[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void api_ProcPrePayCommhead(BYTE* pbuf,BYTE inc, BYTE com, BYTE P1, BYTE P2, BYTE len1,BYTE len2);

//-----------------------------------------------
//��������: ��ȡ���ȫоƬ��Ϣ
//
//����:		Type[in]	ff--ȫ����Ϣ
//			OutBuf[out]	����Ҫ��ȡ�ĳ��ȼ����ݣ��������ֽڣ�����ǰ�����ں�
//����ֵ:	���ݳ��� 8000�������
//
//��ע:OutBuf���ʵ�ʼ���ĳ��ȶ������ֽڣ����ں������Ѿ��������ص�����ǰ�治������
//-----------------------------------------------
WORD api_GetEsamInfo( BYTE Type, BYTE *OutBuf );

//-----------------------------------------------
//��������: ����Ӧ������
//
//����:		Buf[in/out]	����ʱ32�ֽ�����1+4�ֽڿͻ���ǩ��1
//						���ʱ����+48�ֽ������+4�ֽڿͻ���ǩ�����������ֽڣ�����ǰ�����ں�
//����ֵ:	TRUE/FALSE
//
//��ע:��698��Լ�н���Ӧ������ʱʹ��,Bufǰ��2�ֽڴ������ݳ���
//-----------------------------------------------
//BOOL api_ConnectMechanismInfo( eConnectMode ConnectModeMode, BYTE *Buf );

//-----------------------------------------------
//��������: ��֤��վ�·������ݲ����ؽ����������
//
//����:		
//			Buf[in/out]����ʱ��	��������֤��ϢΪSID_MAC
//							4�ֽڱ�ʶ+��������AttachData+Data+MAC
//								��������֤��ϢΪSID
//							4�ֽڱ�ʶ+��������AttachData+Data
//						���ʱ�����ȼ����ݣ��������ֽڣ�����ǰ�����ں�
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL api_AuthDataSID(BYTE *Buf);

//-----------------------------------------------
//��������: �������֯������д��ESAM�����mac,������ϴ�
//
//����:
//			Len[in] ��������ݳ���
//			Buf[in/out]	��������
//					���ݴ���������ʱ ���������ֽڣ�����ǰ�����ں�
//						�������+mac
//
//����ֵ:	TRUE/FALSE
//
//��ע:Bufǰ��2�ֽڴ������ݳ���
//-----------------------------------------------
BOOL api_PackDataWithSafe(WORD Len,BYTE *Buf);

//-----------------------------------------------
//��������: ������д��ESAM,������ϴ�
//
//����:		EncryptMode[in] ���ܷ�ʽ 0x00�� ��վ      0x55���ն�
//			Type[in] 1--����+MAC	2--����	3--����+mac
//			Len[in] ��������ݳ���
//			Buf[in/out]	��������
//					���ݴ���������ʱ ���������ֽڣ�����ǰ�����ں�
//						TypeΪ1ʱ���������+mac
//						TypeΪ2ʱ���������+����
//						TypeΪ3ʱ���������+����+mac
//
//����ֵ:	TRUE/FALSE
//
//��ע:Bufǰ��2�ֽڴ����س���
//-----------------------------------------------
BOOL api_PackEsamDataWithSafe(BYTE EncryptMode, BYTE Type,WORD Len,BYTE *Buf);

//-----------------------------------------------
//��������: ��ȡESAM�ļ���Ϣ
//
//����:		FID[in] 	�ļ���ʶ
//			Offset[in] 	ƫ�Ƶ�ַ
//			Len[in] 	��ȡ����
//			Buf[out]	�������+���ݣ��������ֽڣ�����ǰ�����ں�
//						
//����ֵ:	TRUE/FALSE
//		   
//��ע:��Ҫȷ����վ�·���ʱ���Ƿ���cmd
//-----------------------------------------------
BOOL api_ReadEsamData(WORD FID,WORD Offset,WORD Len,BYTE *Buf);

//-----------------------------------------------
//��������: ȡESAM�����
//
//����:		Len[in]		��������� 4/8/10
//			Buf[out]	�������+���ݣ��������ֽڣ�����ǰ�����ں�					
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL api_ReadEsamRandom(BYTE Len,BYTE *Buf);

//--------------------------------------------------
//��������: ��֤MAC
//
//����:		Type[in]:��������֤MAC
//				READ_METER_RN: �����¼������ݣ����� + MAC
//			MeterNo[in]: 6�ֽڱ��
//			rand[in]:�����
//			Len[in]: Dat����
//			Dat[in]: ����
//����ֵ: FALSE / TRUE
//
//��ע:
//--------------------------------------------------
WORD api_VerifyEsamSafeMAC(BYTE Type, BYTE *MeterNo, BYTE *rand,WORD Len, BYTE *Dat);

//--------------------------------------------------
//��������: ��������
//
//����: Type[in]: ��������ͣ��������;
//		rand[out]:�����
//
//����ֵ: FALSE/TRUE
//
//��ע:
//--------------------------------------------------
WORD api_GetEsamSafeRN(BYTE Type,BYTE *rand);

//-----------------------------------------------
//��������:     ������ָ֤���ȡ�����1���ģ�
//
//����:	    INBuf[in]     //�������������	
//						
//����ֵ:	    TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL api_EsamIRAuth( BYTE *INBuf );
//-----------------------------------------------
//��������: ���������ϱ����ݵ�rn_mac
//
//����:		
//			Len[in] 	���볤��
//			Buf[in]		��������
//			OutBuf[out]	������� rn+mac		
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
WORD api_DealEsamSafeReportRN_MAC( WORD Len,BYTE *Buf,BYTE *OutBuf );

//-----------------------------------------------
//��������: ��֤�����ϱ�Ӧ����
//
//����:		
//			Len[in] 	���볤��
//			Buf[in]		��������
//			OutBuf[out]	������� rn+mac		
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
WORD api_VerifyEsamSafeReportMAC( BYTE DataLen,BYTE *DataBuf,BYTE *RN_MAC );

//-----------------------------------------------
//��������:     �����ѯָ���ȡ�����1���ģ�
//
//����:	    RNBuf[in]     //��������� RNBuf[0](���������)+RNBuf[1]�������������
//	        OutBuf[in/out] //������� ��8�ֽ� �������ȣ�
//		
//						
//����ֵ:	    TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL api_EsamIRRequest( BYTE *RNBuf, BYTE * OutBuf );
BOOL api_EsamTerminalRequest( BYTE *RNBuf, BYTE *OutBuf );

//--------------------------------------------------
//��������:  ����ȶ�����
//
//����:      BYTE P2[in]//��Կ��ʶ
//
//           BYTE Len[in]//���ݳ���
//
//           BYTE *Buf[in]//����bufָ��
//
//����ֵ:
//
//��ע����:  Bufǰ��2�ֽڴ�����
//--------------------------------------------------
BOOL api_SoftwareComparisonEsam(BYTE P2,WORD Len,BYTE *Buf);

//-----------------------------------------------
//��������: ��ȡ��Կ״̬
//
//����:		��					
//����ֵ:	��
//		   
//��ע:�ϵ��ʱ�����esam�е�״̬ˢ��ee�е���Կ״̬
//-----------------------------------------------
void api_FlashKeyStatus(void);

//---------------------------------------------------------------
//��������: ˢ�������֤Ȩ�ޱ�־
//
//����: 
//                   
//����ֵ:  
//
//��ע:   ˢ�������֤Ȩ�� ��־
//---------------------------------------------------------------
void api_FlashIdentAuthFlag( void );

//-----------------------------------------------
//��������: д������ESAM �еĲ���
//
//����:		Type[in]	0x04--������Ϣ�ļ�����Ĳ�������
//						0x85--��ǰ�׵�۸���
//						0x86--�����׵�۸���
//			P2[in]		ƫ�Ƶ�ַ
//			LC[in]		����InData����
//			InData[in]	Data+Mac����Ϊ����					
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		   
//��ע:698�汾ESAM ��645��Լд����(��������ǰ�ס�������)����
//-----------------------------------------------
WORD api_DecipherSecretData(BYTE Type, WORD LC, BYTE *InData, BYTE *OutBuf);

//-----------------------------------------------
//��������: 645д���ʵ�ۡ����ݵ�ۡ������л�ʱ�䡢�����л�ʱ��
//
//����:		Type[in]	0x82--Զ�̿���
//						0x83--�����������
//						0x84--��������
//						0x85--�¼�/��������
//			LC[in]		����InData����
//			InData[in]	����+Mac����Ϊ����
//			OutBuf[out]	���ܺ������				
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		   
//��ע:698�汾ESAM ��645��Լдһ�����
//-----------------------------------------------
BYTE api_Proc_uart_Write_FirstData_645( BYTE *pBuf );

//-----------------------------------------------
//��������: ��ȫ������
//
//����:		
//		ProBuf[in/out]		in--���뻺��645�ĵ�һ���ֽ�68��ͷ
//								out--������ݣ��Ѿ�����					
//����ֵ:	���ݳ���
//		   
//��ע:698�汾ESAM ��645��Լ03�����ֵ���
//-----------------------------------------------
WORD api_ProcProPrePay(eSERIAL_TYPE PortType, BYTE *ProBuf ,BYTE *OutBuf );

//-----------------------------------------------
//��������: �̵��������������
//
//����:	ProBuf[in/out]	645����		
//		
//����ֵ:	ִ��״̬�������ֽ�
//		   
//��ע:698�汾ESAM ��645��Լ1c�����ֵ���
//-----------------------------------------------
WORD api_ProcRemoteRelayCommand645(BYTE *ProBuf);

//-----------------------------------------------
//��������: Ԥ���������ϵ��ʼ��
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_PowerUpPrePay(void);
//-----------------------------------------------
//��������: Ԥ��������͹��Ļ�������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_LowPowerWakeUpPrePay(void);

//-----------------------------------------------
//��������: Ԥ���ѵ��紦��
//
//����:		�� 
//                 
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_PowerDownPrePay(void);

//-----------------------------------------------
//��������: Ԥ���Ѵ�ѭ������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_PrePayTask(void);

//-----------------------------------------------
//��������: ���ڵ���ʼ������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_FactoryInitPrePay(void);

//-----------------------------------------------
//��������: �������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_ClrPrePay(void);

//--------------------------------------------------
//��������:  ��λESAMspi
//         
//����  :   ��
//
//����ֵ:    ��  
//         
//��ע����:  ��
//--------------------------------------------------
void api_ResetEsamSpi( void );

//-----------------------------------------------
//��������: ��Ԥ���Ѳ���
//
//����:
//          ePrePayPara Type[in]	��д���� ���ö�ٶ���
//
//			Buf[out]	�������
//����ֵ:	    ���ݳ���   ����Ϊ0 ����
//
//��ע:
//-----------------------------------------------
WORD api_ReadPrePayPara( ePrePayParaType Type, BYTE *Buf );

//-----------------------------------------------
//��������: дԤ���Ѳ���
//
//����:		ePrePayPara Type[in]	��д���� ���ö�ٶ���
//
//			Buf[in]	   ���뻺�� ��ΪHEX��ʽ
//����ֵ:	    TRUE/FALSE
//
//��ע:    �˺������ķ��ʽ��ݵ�ۿ��Զ������жϵ��
//-----------------------------------------------
BOOL api_WritePrePayPara( ePrePayParaType Type, BYTE *Buf );
//-----------------------------------------------
//��������:	��������ɣ���֤MAC
//	
//����:	 type	    		ִ�е�����
//		 verifyMacInfo 		��Ҫ�Ĳ���
//����ֵ:	TRUE FALSE
//	
//��ע:   
//-----------------------------------------------
BOOL api_processMacRN(eProcessMacRN type,verifyMacInfo_t *verifyMacInfo);
#if( PREPAY_MODE == PREPAY_LOCAL)

//-----------------------------------------------
//��������: ��ѿۼ�
//
//����: 	EnergyDirection[in]		���ܷ���
//			inTotalReduceEnergyNum[in]	���ۼ��ܵ��� ��λ0.01kwh
// 			inRatioReduceEnergyNum[in]	���ۼӵ�ǰ���ʵ��� ��λ0.01kwh
//����ֵ:   ��
//
//��ע:
//-----------------------------------------------
void api_ReduceRemainMoney( BYTE EnergyDirection, BYTE inTotalReduceEnergyNum, BYTE inRatioReduceEnergyNum );

//-----------------------------------------------
//��������:     ���õ��ˢ�±�־λ
//
//����: 	    PriceFlag[in]
//			    eNoUpdatePrice		        ���δ�����仯
//			    ePriceChangeEnergy,			���δ�����仯 ����Ҫˢ�µ�ǰ���
//			    eChangeCurRatePrice,	    �ı��˵�ǰ���ʵ��
//			    eChangeCurLadderPrice,		�ı��˵�ǰ���ݵ��
//
//����ֵ:   ��
//
//��ע:
//-----------------------------------------------
void api_SetUpdatePriceFlag( eChangeCurPriceFlag  PriceFlag );

//-----------------------------------------------
//��������: �õ���ǰ����״̬
//
//����: 	��
//                   
//����ֵ:  	���ر���״̬
//				eNoAlarm,	 ����״̬
//				eAlarm1,     ���ڱ������1״̬
//				eAlarm2,     ���ڱ������2״̬
//				eOvrZero,    ���ڹ���״̬
//				eOvrTick,    ����͸֧״̬
//
//��ע:   	���ڼ̵�������״̬��ѯ
//-----------------------------------------------
eLOCAL_STATUS api_GetLocalStatus( void );

//-----------------------------------------------
//��������:     ��ֵ�����ʣ����
//
//����: 	    Type[in] 0x00: ��ֵ���    0x55: Ԥ�ƽ��                
// 		    ReduceValue[in] �۳����
// 			
//                   
//����ֵ:    ��
//
//��ע:   
//-----------------------------------------------
BOOL api_UpdataRemainMoney( eUpdataMoneyMode Type, DWORD ReduceValue, DWORD BuyTimes );

//-----------------------------------------------
//��������: ���ݽ����ռ����ܣ����ڱ�����ݽ���ʱ���ܵ��룬���ڼ�������õ���
//
//����:  	��
//	
//����ֵ:	TRUE--��ǰ�¡��� ��ʱ���ô��ڽ��ݽ����� FALSE---��ǰ�¡��� ��ʱ�����ڽ��ݽ�����
//
//��ע:   	Сʱ�仯����
//-----------------------------------------------
BOOL api_CheckLadderClosing( void );

//---------------------------------------------------------------
//��������: �嵱ǰ�õ���
//
//����:   ��
//
//����ֵ: ��
//
//��ע:  �������������½���ʱ���㵱ǰ�õ���
//---------------------------------------------------------------
void api_ClrCurLadderUseEnergy( void );

//-----------------------------------------------
//��������: ת���������ESAMǮ��ͬ����
//
//����: 	��
//                   
//����ֵ:  	��
//
//��ע: 
//-----------------------------------------------
void api_SwapMoney( void );

//---------------------------------------------------------------
//��������: ��ȡ��ǰ���ڵڼ����ݡ���ǰ�Ƿ��б����׽��ݱ���ǰ�Ƿ��б����׷���
//
//����: 	Type
//            eInfoCurLadder - ��ȡ��ǰ��������
//            eInfoBackupRateFalg - �Ƿ��б����׷���
//            eInfoBackupLadderFlag - �Ƿ��б����׽���
//
//����ֵ:  		  0 	���������ڵĽ���  0xff����ʾ���ݷ���
//				1  ����0xff����ʾ true������ FALSE ������
//           	2  ����0xff����ʾ true������ FALSE ������
//
//
//��ע:   0xff��ʾԶ��ģʽ����ʾ���н��ݡ�������صķ���
//---------------------------------------------------------------
BYTE api_GetLocalInfo( eLocalInfo Type );

//-----------------------------------------------
//��������: ���Ƿ��忨����
//
//����:  		Buf[out] ����Ƿ��忨���� 
//
//����ֵ:		��
//		   
//��ע:			
//-----------------------------------------------
void api_ReadIllegalCardNum(BYTE *Buf);

//-----------------------------------------------
//��������: ���ۼƹ�����
//
//����:  		Buf[out] ����ۼƹ�����
//
//����ֵ:		��
//		   
//��ע:			
//-----------------------------------------------
void api_ReadTotalBuyMoney(BYTE *Buf);
//-----------------------------------------------
//��������: ��ȡ�忨״̬��
//
//����: 	��
//                   
//����ֵ:   01 �ɹ�   02 ʧ�� 
//
//��ע:   
//-----------------------------------------------
void api_GetCardInsertStatus(BYTE *OutBuf);
//-----------------------------------------------
//��������:     ESAM����֤
//
//����:		��
//			
//����ֵ:	    �ɹ�/ʧ��
//		   
//��ע:
//		
//-----------------------------------------------
BYTE api_TestEsamCard( void );

//-----------------------------------------------
//��������: ��ȡ��ǰ�Ľ��ݽ����ձ�
//
//����:  		t[in] ��ǰʱ��
//
//����ֵ:		����ģʽ
//			eNoLadderMode	--�޽���ģʽ
//			eLadderMonMode	--�½���ģʽ
//			eLadderYearMode	--�����ģʽ
//
//��ע:
//-----------------------------------------------
BYTE api_GetCurLadderYearBillInfo(TLadderBillingPara* LadderBillingPara);
//-----------------------------------------------
//��������: ��ȡ����ǰ�Ľ��ݽ����ձ�
//
//����: 		LadderBillingPara[out]:���ݽ����ձ�
//
//����ֵ:		����ģʽ
//			eNoLadderMode	--�޽���ģʽ
//			eLadderMonMode	--�½���ģʽ
//			eLadderYearMode	--�����ģʽ
//
//��ע:
//-----------------------------------------------
BYTE api_GetPowerDownYearBill(TLadderBillingPara* LadderBillingPara);
//-----------------------------------------------
//��������: ��E2��ȡ��ǰ���ݱ�����ǰ���ݱ�
//
//����: 	
//
//����ֵ:	

//
//��ע:ֻ�������Y�ã��ڳ�ʼ������ǰ��ȡ����ֹ��������ݽ��㽫E2�е�ǰ���ݱ���
//-----------------------------------------------
void api_ReadPowerLadderTable(void);



#if(SEL_DLT698_2016_FUNC == YES )
BYTE api_RemoteActionMoneybag( BYTE Type, DWORD Money, DWORD BuyTimes, BYTE *pCustomCode, BYTE *pSIDMAC, BYTE *pMeterNo );
#endif

#endif//#if( PREPAY_MODE == PREPAY_LOCAL)
#endif//__PREPAY_API_H
