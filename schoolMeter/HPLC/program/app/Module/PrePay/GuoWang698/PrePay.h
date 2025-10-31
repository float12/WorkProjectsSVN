//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.23
//����		Ԥ���� �ڲ�ͷ�ļ�
//�޸ļ�¼	

//
//----------------------------------------------------------------------
#ifndef __PREPAY_H
#define __PREPAY_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------						
#define MAX_PREPAY_LEN					2000

typedef struct TSafeAuPara_t
{
	BYTE	EsamRst8B[8];				//ESAM���к�
	BYTE	EsamRand[8];				//ESAM�����
	BYTE    CardRst8B[8];               //����� ��ɢ����	
	BYTE	CardRand[8];				//�������
	BYTE    PcRst8B[8];				    //PCԶ���·��� ��ɢ���� 8�ֽ�
	BYTE    PcRand[8];				    //PCԶ���·��� �����1  8�ֽ�
}TSafeAuPara;	//��ȫ��֤��ز���


#if( PREPAY_MODE == PREPAY_LOCAL)
#define DEFAULT_UPDATE_MONEY_TIME		60				//ESAMͬ��ʱ������

//дһ������Ƿ��MAC
#define WITH_MAC						0x55
#define NO_MAC							0x00

//дһ�������ʽ
#define CARD_MODE						0x55
#define TERMINAL_MODE					0xA0

//һ��������±�־λ
#define UPDATE_NO_PARA					0x00			//�������κβ���
#define UPDATE_PARA_FILE				0x80			//������Ϣ�ļ�
#define UPDATE_CUR_RATE					0x01			//��ǰ�׷���
#define UPDATE_BAK_RATE					0x02			//�����׷���
#define UPDATE_CUR_LADDER				0x04			//��ǰ�׽���
#define UPDATE_BAK_LADDER				0x08			//�����׽���

//�û�������
#define OPEN_CARD						0x01			//������
#define BUY_CARD						0x02			//���翨
#define LOST_CARD						0x03			//����
#define CLOSE_CARD						0x00			//��բ���翨

#define		ESAM_RUN_FILE_LENTH			49			//ESAM������Ϣ�ļ����ȣ��û�����д��Ϣ�ļ�����
#define		ESAM_RUN_INFO_FILE			0x07		//ESAM������Ϣ�ļ�


#define		UserCardLength		39	//�û�������
#define		PresetCardLength	26	//Ԥ�ÿ�����


#define		OK_CARD_PROC		0xff	//��Ƭ�����ɹ� 
#define		IC_VLOW_ERR			0x01	//��Ƶ�ѹ����176V(80%Un) err31
#define		IC_ESAM_RDWR_ERR	0x02	//1����ESAM����;	err31
#define		IC_ESAM_RST_ERR		0x03	//1ESAM��λ����	err31
#define		IC_IC_RST_ERR		0x04	//��Ƭ��λ����:���忨�����Ƭ	err32
#define		IC_AUTH_ERR			0x05	//1�����֤����(ͨ�ųɹ������Ĳ�ƥ��)	err32
#define		IC_EXAUTH_ERR		0x06	//1�ⲿ�����֤����(ͨ�ųɹ�����֤��ͨ��);	err32
#define		IC_NOFX_ERR			0x07	//δ���еĿ�Ƭ(����Ƭʱ����6B00);	err32
#define		IC_TYPE_ERR			0x08	//�����ʹ���	err32
#define		IC_KEY_ERR			0x09	//��Ƭ����δ��Ȩ����Կ״̬��Ϊ��Կ�ǲ����Ԥ�ƿ�	err32
#define		IC_MAC_ERR			0x10	//1MACУ�����(6988);	err32
#define		IC_ID_ERR			0x11	//��Ų�һ��	err33
#define		IC_HUHAO_ERR		0x12	//�ͻ���Ų�һ��	err33
#define		IC_SERIAL_ERR		0x13	//����Ų�һ��	err33
#define		IC_FORMAT_ERR		0x14	//��Ƭ�ļ���ʽ���Ϸ�	err34
#define		IC_NOOPEN_ERR		0x15	//���翨����δ�������	err34
#define		IC_NOOPEN2_ERR		0x16	//��������δ�������	err34
#define		IC_TIMES_ERR		0x17	//�����������	err34
#define		IC_NONULL_ERR		0x18	//�û�����д��Ϣ�ļ���Ϊ��	err34
#define		IC_NOIN_ERR			0x19	//1������ƬͨѶ����(��д��Ƭʧ��);	err35
#define		IC_PREIC_ERR		0x20	//��ǰ�ο�	err35
#define		IC_OVERMAX_ERR		0x21	//ʣ����ڻ�	err36

#define UPDATE_FLAG_LENGTH			(1+4)//�������±�־λ���� 1���ֽ�����+4���ֽ�MAC
#define PARA_BUF_LENGTH				(14+4)
#define RATE_BUF_LENGTH				(4+32*4+4)//ͷ+����+MAC
#define LADDER_BUF_LENGTH			(64+4)
#define TIME_BUF_LENGTH				(5+4)

#define		MAX_RATE_PRICE					32			//�������������ʵ����

#define		ESAM_PARA_FILE				0x84		//ESAM������Ϣ�ļ�
#define		ESAM_CUR_RATE_FILE			0x85		//ESAM��ǰ�׵���ļ�
#define		ESAM_BAK_RATE_FILE			0x86		//ESAM�����׵���ļ�

#define		PRESET_CARD_PARA_FILE		0X81		//Ԥ�ÿ�������Ϣ�ļ�
#define		PRESET_CARD_MONEY_FILE		0X85		//Ԥ�ÿ�������Ϣ�ļ�
#define		PRESET_CARD_CUR_RATE_FILE	0X83		//Ԥ�ÿ���ǰ�׵���ļ�
#define		PRESET_CARD_BAK_RATE_FILE	0X84		//Ԥ�ÿ������׵���ļ�

#define		USER_CARD_PARA_FILE			0X81		//�û���������Ϣ�ļ�
#define		USER_CARD_MONEY_FILE		0X82		//�û���������Ϣ�ļ�
#define		USER_CARD_CUR_RATE_FILE		0X83		//�û�����ǰ�׵���ļ�
#define		USER_CARD_BAK_RATE_FILE		0X84		//�û��������׵���ļ�	
#define		USER_CARD_RETURN_WRITE_FILE	0X85		//�û�����д��Ϣ�ļ�
	

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

//����۱�־
typedef enum
{
	eCurRate = 0,           //��ǰ�׷��ʵ��
	eBackRate,              //�����׷��ʵ��
	eCurLadder,             //��ǰ�׽��ݵ��
	eBackLadder,            //�����׽��ݵ��
}ePriceType; 


typedef struct TCurrentPrice_t
{
	TFourByteUnion		Current_L;			//��ǰ���ݵ��
	TFourByteUnion		Current_Lx;			//��ǰ���ݵ�۳��Ա�Ⱥ���
	TFourByteUnion		Current_F;			//��ǰ���ʵ��
	TFourByteUnion		Current_Fx;			//��ǰ���ʵ�۳��Ա�Ⱥ���
	TFourByteUnion		Current;			//��ǰ���
	BYTE				Current_Ladder;		//��ǰ����
	BYTE				Ladder_Mode;		//����ģʽ��,0x00Ϊ�½���,0x55Ϊ�����
}TCurPrice;

typedef struct TUserCardMoney_t
{
	BYTE	MeterNo[6];			//���
	BYTE 	CustomCode[6+4];	//����+mac
	BYTE 	ReturnFileStatus;	//�û�����д״̬	1��Ϊ�գ�0Ϊ��
	BYTE 	UserCardType;		//�û�������
	BYTE 	BuyTimesFlag;		//���������ֵ 0�����, 1����1, 2�����ڵ���2, 0xff��С��;
}TUserCardMoney;

//449�ֽ�
typedef struct TCardParaUpdate_t
{
	BYTE FlagBuf[UPDATE_FLAG_LENGTH];		//�������в������±�־λ
	BYTE ParaBuf[PARA_BUF_LENGTH];			//�������в����ļ�����
	BYTE CurRateBuf[RATE_BUF_LENGTH];		//�������е�ǰ�׷��ʵ�۱���
	BYTE CurLadderBuf[LADDER_BUF_LENGTH];	//�������е�ǰ�׽��ݱ���
	BYTE BakRateBuf[RATE_BUF_LENGTH];		//�������б����׷��ʵ�۱���
	BYTE BakLadderBuf[LADDER_BUF_LENGTH];	//�������б����׽��ݱ���
	BYTE BakRateTimeBuf[TIME_BUF_LENGTH];	//�����׷�������ʱ��
	BYTE BakLadderTimeBuf[TIME_BUF_LENGTH];	//�����׽�������ʱ��
}TCardParaUpdate;
#endif

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern BYTE	PrePayBuf[MAX_PREPAY_LEN];

extern __no_init BYTE		MacErrCounter;
extern	TSafeAuPara         SafeAuPara;			//��ȫ��֤��ز���
extern TPrePayPara          PrePayPara;          //��������
#if( PREPAY_MODE == PREPAY_LOCAL)
extern TRatePrice			CurRateTable;		//��ǰ���ʵ�۱�
extern TLadderPrice			CurLadderTable;		//��ǰ���ݵ�۱�
extern TLimitMoney		    LimitMoney;         //������޲���
extern TCurPrice		    CurPrice;			//��ǰ��� ���ڿ۷�
extern TCardInfo   CardInfo; //����Ϣ������״̬�������кţ�
extern __no_init TCurMoneyBag CurMoneyBag;//Ǯ���ļ����ѿ۷������� 
extern BYTE	Err_Sw12[2];					//�ݴ��������ص�SW1��SW2

#endif
//-----------------------------------------------
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ��дESAM����
//
//����:		CmdLen[in]	����ͷ���ݳ���
//			Cmd[in]		����ͷ
//			BufLen[in]	�������ݳ���
//			InBuf[in]	��������
//			OutBuf[out]	������� ������ֽڳ��ȼ����ݣ����ȸ���ǰ�����ں�		
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL Link_ESAM_TxRx(BYTE CmdLen, BYTE *Cmd, WORD BufLen, BYTE *InBuf, BYTE *OutBuf);

#if( PREPAY_MODE == PREPAY_LOCAL )
/********************money.c**************************/

//-----------------------------------------------
//��������: �ϵ����
//
//����: 	    ePOWER_MODE Type
//                   
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void PowerUpMoney( ePOWER_MODE Type );
//-----------------------------------------------
//��������: �������
//
//����: 	    ��
//                   
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void PowerDownMoney( void );

//-----------------------------------------------
//��������:     ����������
//
//����: 	    ��
//                   
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void MoneyMinuteTask( void );
//-----------------------------------------------
//��������:     ���������
//
//����: 	    ��
//                   
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void MoneySecTask( void );

/********************card.c**************************/

//-----------------------------------------------
//��������: ��дCPU������
//
//����:		Cla	Ins P1 P2 P3 ����ͷ
//			RW[in]		����READ,д��WRITE
//			peerom[in]	д����߶���������ָ��
//			
//					
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL Link_Card_TxRx(BYTE Cla,BYTE Ins,BYTE P1,BYTE P2,BYTE P3,BYTE RW,BYTE *peerom);

//-----------------------------------------------
//��������: ��λ��Ƭ,��ȡ��λ��Ϣ
//
//����:		Buf���ظ�λ��Ϣ
//						
//����ֵ:		TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL Link_CardReset( BYTE *Buf );

//-----------------------------------------------
//��������: ȡCARD�����
//
//����:		Len[in]		��������� 4/8/10
//			Buf[out]	�������+���ݣ��������ֽڣ�����ǰ�����ں�					
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL ReadRandomCARD(BYTE Len,BYTE *peeerom);

//-----------------------------------------------
//��������: ��MAC��ȡ��Ƭ�ж���������
//
//����:		CH[in]��01:��ȡ�û�����Ԥ�ÿ��г���Ǯ��֮����ļ�  21:��ȡԤ�ÿ���Ǯ���ļ�  31:	��ȡ�û�����Ǯ���ļ�
//			P1[in],P2[in],M1[in],M2[in],ָ�����ĸ�ʽ���������ֲ�˵��
//			Len[in]�ӿ��ж������ݵĳ���+4�ֽ�mac�ĳ���(Lc+4)
//����ֵ:	�ɹ�/ʧ��
//		   
//��ע:
//		
//-----------------------------------------------
BYTE ReadBinWithMacCard( BYTE CH, BYTE P1, BYTE P2, BYTE M1, BYTE M2, BYTE Len ,BYTE *peerom);

//-----------------------------------------------
//��������: ѡ���ļ�
//
//����:		
//			F1[in]	�ļ����
//			F2[in]	�ļ����
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BYTE SelectFile( BYTE F1, BYTE F2 );

//-----------------------------------------------
//��������: ������ESAM�ж������ļ�
//	
//����:		P1[in],P2[in],P3[in] ָ�����ĸ�ʽ���������ֲ�˵��
//			peeerom[in] 		 ��Ƭ��λ��Ϣ
//
//����ֵ:		��
//		   
//��ע:
//----------------------------------------------
BYTE ReadBinFile(BYTE P1, BYTE P2, BYTE P3,BYTE *peeerom);

//-----------------------------------------------
//��������: �忨�������
//	
//����:		��
//
//����ֵ:		��
//		   
//��ע:
//----------------------------------------------
void Card_Proc( void );

//-----------------------------------------------
//��������: �ϵ����ݱ����л����³�ʼ�����ݱ�����ģʽ
//
//����: 	��
//                   
//����ֵ:   ��
//
//
//��ע:		����ģʽ�л� ����� �½���
//			��Ҫ�������½���ģʽ�л������
//			1�����������ն�ͨ���û����������õ�ǰ�׽��ݱ�ʱҪ�ж�
//			2���ѿ�ģʽ�����л�ʱҪ�ж�
//			3�����ñ����׽���ʱ����ʱ����Ҫ�жϲ��õ�ǰ�ڼ��׽��ݱ����ж����½����л�
//			4�����ñ����׽��ݱ�ʱ����Ҫ�ж����½����л�
//			5������ʱ�����ڲ������л�ʱ���ҵ���ǰ���ݲ������ױ����ж����½����л�
//			6�����õ�ǰ�׽���ʱ����ʱ����Ҫ�ҵ���ǰ���ݲ������ױ����ж����½����л�
//			7�����ñ����׽���ʱ���������׽��ݱ�ʱ�����ݴ�����������ж��Ƿ�������½����л�
//			8: ������շ����仯����������ս����л�
//-----------------------------------------------
void ProcLadderModeSwitch(void);//���㷽ʽ�任�жϼ�����

//-----------------------------------------------
//��������: ��ʼ�����ݱ�����ģʽ
//
//����: 	��
//                   
//����ֵ:   ��
//
//��ע:     �����ϵ��ʼ����Ԥ�ÿ����õ�ǰ�׽����������
//			��������Ҫ�����ϵ��ʼ��BasicPara�ĺ����У�����������У�����Ҫ����дee��ram�ĺ���
//-----------------------------------------------
void InitJudgeBillMode(void);

//-----------------------------------------------
//��������: �忨��Ϣ
//
//����:  		��
//
//����ֵ:		��
//
//��ע:
//-----------------------------------------------
void ClrCardInfo( void );

//-----------------------------------------------
//��������: ��ȡǮ��
//
//����:		Buf[out]	�������+���ݣ��������ֽڣ�����ǰ�����ں�
//
//����ֵ:	TRUE/FALSE
//
//��ע:���س���+�����4��+���������4��+�ͻ���ţ�6��
//-----------------------------------------------
BOOL ReadEsamMoneyBag( BYTE *Buf ); 

//-----------------------------------------------
//��������: ����Ǯ��ͬ��
//
//����:     ��
//
//����ֵ:   ��
//
//��ע:
//-----------------------------------------------
void MeteringUpdateMoneybag( void ); 

//-----------------------------------------------
//��������: ����״̬�ж�
//
//����:    Type[in]
//           eReturnMoneyMode  -- �۷ѻ����˷Ѳ���
//			 eChargeMoneyMode  -- ��ֵ����
//           eSetParaMode      -- ���Ĳ�������
//			 eInitMoneyMode     -- Ԥ�ò���
//        ParaType[in]         -- ����Ǹ��Ĳ��� ����������� �Ǹ��Ĳ�����������FF
//			 eTickLimit		   -- ͸֧�������
// 			 eAlarm_Limit1	   -- �������1
// 			 eAlarm_Limit2	   -- �������2
//����ֵ:  ����״̬
//
//��ע:
//-----------------------------------------------
void JudgeLocalStatus( eUpdataMoneyMode Type, BYTE ParaType );

//---------------------------------------------------------------
//��������: ��õ�ǰ�����õ���
//
//����: 	��
//
//����ֵ:	��ǰ�����õ���
//
//��ע:   �������ģʽ�ַ���
//---------------------------------------------------------------
DWORD GetCurLadderUserEnergy( void );
//---------------------------------------------------------------
//��������: �ж��л�ʱ��Ϸ��� - ��ʱ�ŵ�����
//
//����: 	Buf[in]  ������ʱ��
//                   
//����ֵ:   
// 		   0x00 - ȫ0
// 		   0x01 - ʱ�����
//		   0x02 - ʱ����ȷ
// 		   0x03 - ȫ9
//
//��ע:   
//---------------------------------------------------------------
BYTE CheckSwitchTime( BYTE *Buf );

#endif

#endif//__PREPAY_H






