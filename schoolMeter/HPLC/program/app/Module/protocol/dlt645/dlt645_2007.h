//------------------------------------------------------------------------------------------
//	��    �ƣ� dlt645_2007.h
//	��    ��:  DL/T645-2007 ��չ�ṹ�����ļ�
//	��    ��:  κ����
//	����ʱ��:  2007-11-6 19:54
//	����ʱ��:
//	��    ע:	���ѽṹ�����
//	�޸ļ�¼:
//------------------------------------------------------------------------------------------
#ifndef __Dlt645_2007_H
#define __Dlt645_2007_H

#if( SEL_DLT645_2007 == YES )


//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define DLT_CLOSEING_NO				0X0C		//������֧�ֵ�������

//Э�����붨��
#define CLASS_2_PASSWORD_LEVEL				0x98//�������ݼ��ܲ������뼶�����Ϊ98H
#define CLASS_1_PASSWORD_LEVEL				0x99//һ�����ݼ��ܲ������뼶�����Ϊ99H
#define MINGWEN_H_PASSWORD_LEVEL			0x02
#define MINGWEN_L_PASSWORD_LEVEL			0x04
#define SYSTEM_UNLOCK_PASSWORD_LEVEL		0xAB

//��Լ�����ݺ���������Ϣ�����ɹ����ض����ݳ���
#define DLT645_OK_00							0x0000		//�޴���
#define DLT645_ERR_DATA_01						0x8001		//�Ƿ�����
#define DLT645_ERR_ID_02						0x8002		//���ݱ�ʶ���
#define DLT645_ERR_PASSWORD_04					0x8004		//�����
#define DLT645_CANNOT_CHANGE_BAUD_08			0x8008		//DL/T645-2007 ͨ�����ʲ��ܸ���
#define DLT645_ERR_OVER_AREA_10					0x8010		//��ʱ������
#define DLT645_ERR_OVER_SEG_20					0x8020		//��ʱ������
#define DLT645_ERR_OVER_RATIO_40				0x8040		//��������
#define DLT645_ERR_PRG_KEY_80					0x8080		//��̰���û�а���
#define DLT645_NO_RESPONSE						0x80FF		//����ظ�

//================================================Ԥ�������ʶ=====================================================
#define DLT645_SPECIAL_FLAG_00000001H			(0x00000001)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000002H			(0x00000002)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000004H			(0x00000004)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000008H			(0x00000008)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000010H			(0x00000010)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000020H			(0x00000020)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000040H			(0x00000040)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000080H			(0x00000080)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000100H			(0x00000100)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000200H			(0x00000200)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000400H			(0x00000400)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00000800H			(0x00000800)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00001000H			(0x00001000)//Ԥ��1
#define DLT645_SPECIAL_FLAG_00002000H			(0x00002000)//Ԥ��1
//================================================����ģʽ��ʶ=====================================================
#define DLT645_ENERGY_00H						(0X00)		//DI3 ���ܱ�ʶ
#define DLT645_DEMAND_01H						(0X01)		//DI3 ��������뷢��ʱ���ʶ
#define DLT645_VARIABLE_02H						(0X02)		//DI3 ������ʶ
#define DLT645_PARA_04H							(0X04)		//DI3 �α�����ʶ
#define DLT645_FREEZE_05H						(0X05)		//DI3 �������ݱ�ʶ
#define DLT645_LOAD_06H							(0X06)		//DI3 ���ɼ�¼��ʶ
#define DLT645_EVENT_03H						(0X03)		//DI3 ��Կ���¼�¼���쳣�忨�¼���ʶ
#define DLT645_EVENT_10H						(0X10)		//DI3 ʧѹ�¼���ʶ
#define DLT645_EVENT_11H						(0X11)		//DI3 Ƿѹ�¼���ʶ
#define DLT645_EVENT_12H						(0X12)		//DI3 ��ѹ�¼���ʶ
#define DLT645_EVENT_13H						(0X13)		//DI3 �����¼���ʶ
#define DLT645_EVENT_14H						(0X14)		//DI3 ��ѹ�������¼���ʶ
#define DLT645_EVENT_15H						(0X15)		//DI3 �����������¼���ʶ
#define DLT645_EVENT_16H						(0X16)		//DI3 ��ѹ��ƽ���¼���ʶ
#define DLT645_EVENT_17H						(0X17)		//DI3 ������ƽ���¼���ʶ
#define DLT645_EVENT_18H						(0X18)		//DI3 ʧ���¼���ʶ
#define DLT645_EVENT_19H						(0X19)		//DI3 �����¼���ʶ
#define DLT645_EVENT_1AH						(0X1A)		//DI3 �����¼���ʶ
#define DLT645_EVENT_1BH						(0X1B)		//DI3 ���������¼���ʶ
#define DLT645_EVENT_1CH						(0X1C)		//DI3 �����¼���ʶ
#define DLT645_EVENT_1DH						(0X1D)		//DI3 ��բ�¼���ʶ
#define DLT645_EVENT_1EH						(0X1E)		//DI3 ��բ�¼���ʶ
#define DLT645_EVENT_1FH						(0X1F)		//DI3 �ܹ��������¼���ʶ
#define DLT645_EVENT_20H						(0X20)		//DI3 �������ز�ƽ���¼���ʶ
#define DLT645_EVENT_21H						(0X21)		//DI3 ���ʷ���ı��¼���ʶ
#define DLT645_EVENT_22H						(0X22)		//DI3 ʱ�ӵ��Ƿѹ�¼���ʶ
#define DLT645_EVENT_23H						(0X23)		//DI3 ͣ�糭����Ƿѹ�¼���ʶ

#define DLT645_FOLLOW_LEN			(196)//����֡���ݳ��� ���������ݱ�ʶ Ϊʲô��196 ����Ϊ�����ݱ�ʶ����200���ֽ�


#define DLT645_FACTORY_ID						(0x04dfdfdb)

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum{
	ePROTOCOL_DLT645_07=0,		// 07��Լ 
	ePROTOCOL_DLT645_97,  		//97��Լ
	ePROTOCOL_DLT645_FACTORY,	//���ڹ�Լ
	
	ePROTOCOLL_MAX
}eTYPE_PROTOCOL;

typedef enum{
	eBYTE_MODE = 0,      //���ֽ�ģʽ
	eCOMB_MODE			//���ģʽ ���糤��Ϊ2 ����BYTE��ϳ�һ��WORD
}eTYPE_COVER_DATA;		//HEX��BCDת��ģʽ

typedef enum{
	eNO_FOLLOW=0,		//�޺���֡
	eNORMAL_FOLLOW,  	//��������֡
	eLOAD_FOLLOW,		//���ɼ�¼����֡
	eLOAD_FOLLOW_END,   //����֡������־
	eREPORT_FOLLOW,		//�����ϱ�����֡
}eFOLLOW_FLAG;

typedef enum{
	eTYPE_ADDR_TYPE_88=0,	//ȫ8��ַ��ͨ�ñ��ַ
	eTYPE_ADDR_TYPE_99,		//ȫ9��ַ
	eTYPE_ADDR_TYPE_ALLAA,	//ȫA��ַ
	eTYPE_ADDR_TYPE_AA,		//ͨ���ַ
	eTYPE_ADDR_TYPE_XX,		//��ͨ��ַ
	
	eTYPE_ADDR_TYPE_MAX
}eTYPE_ADDR_TYPE;

typedef enum{
    eAUTHORITY_TYPE_INSIDE_FACTORY=BIT0,    //����Ȩ��
	eAUTHORITY_TYPE_CLASS_2=BIT1,	        //��������Ȩ��
	eAUTHORITY_TYPE_CLASS_1=BIT2,		    //һ������Ȩ��
	eAUTHORITY_TYPE_MINGWEN_H=BIT3,	        //���ĸ߼�Ȩ��
	eAUTHORITY_TYPE_MINGWEN_L=BIT4,		    //���ĵͼ�Ȩ��
    eAUTHORITY_TYPE_MAC_OK=BIT5,            //macδ����Ȩ��
    eAUTHORITY_TYPE_IR_ALLOW=BIT6           //������֤Ȩ��
}eAUTHORITY_TYPE;


typedef enum{
	eCONTROL_RECE_01=0x1,	//Dlt645��Լ������
	eCONTROL_RECE_03=0x3,	//Ԥ��������
	eCONTROL_RECE_08=0x8,	//�㲥Уʱ
	eCONTROL_RECE_09=0x9,	//����Ѱ������
	eCONTROL_RECE_10=0x10,	//������
	eCONTROL_RECE_11=0x11,	//��Լ������
	eCONTROL_RECE_12=0x12,	//��Լ������֡����
	eCONTROL_RECE_13=0x13,	//��Լ�����ͨ�ŵ�ַ
	eCONTROL_RECE_14=0x14,	//��Լд����
	eCONTROL_RECE_15=0x15,	//��Լдͨ�ŵ�ַ
	eCONTROL_RECE_16=0x16,	//������ܱ�����
	eCONTROL_RECE_17=0x17,	//����ͨ������
	eCONTROL_RECE_19=0x19,	//��ǰ�������������ʱ����������
	eCONTROL_RECE_1A=0x1a,	//��յ��ܱ��ڵ��������������������ʱ�䡢���������¼���¼�����ɼ�¼������
	eCONTROL_RECE_1B=0x1b,	//��յ��ܱ��ڴ洢��ȫ����ĳ���¼���¼����
	eCONTROL_RECE_1C=0x1c,	//��բ����բ�����������������������ͱ�����
	eCONTROL_RECE_1D=0x1D	//���ö๦�ܶ�������ź����
}eTYPE_CONTROL_RECE;

//
typedef struct{
	BYTE *pMessageAddr;					//���Ĵ����ַ
	eTYPE_PROTOCOL eProtocolType;		//��Լ����
	eTYPE_ADDR_TYPE eAddrType;			//�·����ĵĵ�ַ���ͣ�99��88��AA,����
	WORD wSerialDataLen;				//�������ݳ���
	TFourByteUnion dwDataID;			//���ݱ�ʶ
	BYTE bIDLen;						//���ݱ�ʶ����
	BYTE byReceiveControl;				//���ձ��Ŀ�����
//	BYTE bSendControl;					//���ͱ��Ŀ�����  //���ͱ��Ŀ�������ɽ��ܱ��Ŀ�����ת�� �Ƿ����ȥ��!!!!
	BYTE *pDataAddr;					//��������ַ
	WORD wDataLen;						//����������		
//	BYTE byFollowUpFrameExist;			//�Ƿ��к�������
	BYTE bSEQ;							//����֡��� ������������֡��Ӱ�� �ں���֡����֡��֡�����Ϊ0
//	BYTE bAddr[6];						//ͨ�ŵ�ַ
	BYTE byOperatorCode[4];				//�����ߴ���
	BYTE bLevel;						//����ȼ�
	BYTE bPassword[3];					//�·�������
	WORD wErrType;						//��������
	DWORD dwSpecialFlag;				//�����־������BIT0��1������ģʽ
	//T_FUNDLT645 fun;//�ص�����
}T_DLT645; 

typedef struct TDLT645APPFollow_t   	//��·��ṹ��
{
	eFOLLOW_FLAG FollowFlag;			//����֡��־
	BYTE bLastNum;						//list �Ѵ������ݸ���
	DWORD dwLastTime;					//���濪ʼʱ��ľ��Է�����
	DWORD dwReadTime;//���ɼ�¼��ȡ��ʱ��
	DWORD dwRecordNo;//���ɼ�¼�´ο�ʼ���ļ�¼���
	BYTE byLpfBlockNum;//���ɼ�¼ʣ�����/ʣ�����
	BYTE Buf[17];//���ڱ�����������Ա����������ʱ��������,[0]:Len [1~16]:Data
}TDLT645APPFollow;

typedef struct TDLT645Sendbuf_t			//645���ͻ��� 
{
	BYTE *DataBuf;
	WORD DataLen;					//������
}TDLT645Sendbuf;

typedef struct TMeteringVaribleTab_t
{
	WORD ID;		//���ݱ�ʶ DI1 DI2
	WORD Type;		//�������� ��ѹ ���� ���ʵ�
	WORD Phase;		//�� �� A B C ���ݿ�
	BYTE Lenth;		//����
	BYTE Dot;		//С��λ��
}TMeteringVaribleTab;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern TDLT645APPFollow 	DLT645APPFollow[MAX_COM_CHANNEL_NUM];
extern T_DLT645 			gPr645[MAX_COM_CHANNEL_NUM];
extern TDLT645Sendbuf 		DLT645Sendbuf[MAX_COM_CHANNEL_NUM];
extern __no_init BYTE 		g_645PassWordErrCounter[2];

//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������


//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
//PareMeter645.c
//---------------------------------------------------------------
//��������: �����α�������
//
//����:
//                   PortType[in] - ͨ��
//		    ID[in]      - �������ݱ�ʶ
//		    OutBufLen   - ���뻺�泤��
//			OutBuf[out] - ��������
//
//����ֵ:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//
//��ע:
//---------------------------------------------------------------
WORD GetPro645RequestMeterPara( eSERIAL_TYPE PortType,BYTE *DI, WORD OutBufLen, BYTE *OutBuf );

//dlt645_2007Set_14H.c
WORD DealSetDlt645_2007Data0X14( eSERIAL_TYPE PortType );

//Variable645.c
//---------------------------------------------------------------
//��������: ��ȡ����
//
//����: 
//                   
//����ֵ: 
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//
//��ע:   
//---------------------------------------------------------------
WORD Get645RequestVariable( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf );
//event645.c
//--------------------------------------------------
//��������:     ��ȡ645�¼��ӿں���     
//��     ��:
//          pDI[in]	    ���ݱ�ʶ   
//          OutBuf[in]  ����Buf
//����ֵ    
//��ע����:     WORD  ���ݳ��� 0x8000�������ݳ���
//--------------------------------------------------
WORD api_ReadEventRecord645( BYTE* pDI , BYTE* OutBuf );


//Energy.c
//--------------------------------------------------
//��������:  ��ȡ����       
//��     ��:BYTE Dot[in]		С��λ��       
//         BYTE *DI[in]		���ݱ�ʶ                       
//         WORD OutBuf[in]  ��������        
//         WORD Lenth[in]	��ȡ�ĳ���        
//����    ֵ:����״̬ �ɹ���8000; ʧ�ܣ�800X           
//��ע����:  ��
//--------------------------------------------------
WORD GetRequest645Energy( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf );

//dlt645_2007APPData.c
WORD GetPro645IDData( eSERIAL_TYPE PortType, BYTE Dot, BYTE *ID, WORD OutBufLen, BYTE *OutBuf );
//---------------------------------------------------------------
//��������: ͨ��ר��HEXתBCD һ�������ģʽ
//
//����: 
//		  Type[in]
// 			  eBYTE_MODE - ���ֽ�ģʽ
//			  eCOMB_MODE - ���ģʽ ���糤��Ϊ2 ����BYTE��ϳ�һ��WORD
// 		  Lenth[in] -  �������ݳ���
// 		  Buf[in] - ���뻺��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL ProBinToBCD645( eTYPE_COVER_DATA Type, BYTE Lenth, BYTE *Buf );

//--------------------------------------------------
//��������:  ����������������
//         
//����:
//         
//����ֵ:    DLT645_OK_00--�ɹ� ����--����ʧ������
//         
//��ע����: 
//--------------------------------------------------
WORD ProIap( eSERIAL_TYPE PortType );
//---------------------------------------------------------------
//��������: ͨ��ר��HEXתBCD
//
//����: 
//		  Type[in]
// 			  eBYTE_MODE - ���ֽ�ģʽ
//			  eCOMB_MODE - ���ģʽ ���糤��Ϊ2 ����BYTE��ϳ�һ��WORD
// 		  Lenth[in] -  �������ݳ���
// 		  Buf[in] - ���뻺��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL ProBinToBCD( eTYPE_COVER_DATA Type, BYTE Lenth, BYTE *Buf );


//dlt645_2007APP.c
void FactoryInitDLT645( void );
//-----------------------------------------------
// ��������  : 645��ԼԤ����
//
// ����  :    TSerial *p[in]
//
// ����ֵ:     BOOL ��TRUE-��Ҫ��ת�����߱������鴦��
// ��ע����  :  ��
//-----------------------------------------------
BOOL Pre_Dlt645(TSerial *p);
//-----------------------------------------------
//��������  :   ��ʼ��698��־
//
//����  : 	��
//
//����ֵ:      ��
//
//��ע����  :   ��
//-----------------------------------------------
void InitDLT645Flag( eSERIAL_TYPE PortType );
//--------------------------------------------------
//��������:  698.45�ϵ纯��
//         
//����  :   ��
//
//����ֵ:    ��  
//         
//��ע����:  ��
//--------------------------------------------------
void PowerUpDlt645(void);

//Demand645.c
//--------------------------------------------------
//��������:  ��ȡ����       
//��     ��:BYTE Dot[in]		С��λ��       
//         BYTE *DI[in]		���ݱ�ʶ                       
//         WORD OutBuf[in]  ��������        
//         WORD Lenth[in]	��ȡ�ĳ���        
//����    ֵ:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���    
//��ע����:  ��
//--------------------------------------------------
WORD GetRequest645Demand( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf );
//---------------------------------------------------------------
//��������: ��ȡ���ɼ�¼����
//
//����: 
//			PortType[in]-ͨѶ��
//		    DI[in]     - �������ݱ�ʶ
//		    OutBufLen  - ���뻺�泤��	
// 			OutBuf[out]- ������ݻ���                  
//����ֵ:  
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//��ע:   
//---------------------------------------------------------------
WORD GetRequest645Lpf( eSERIAL_TYPE PortType, BYTE *DI, WORD OutBufLen, BYTE *OutBuf );

///////////////////////////////DLT645_2007Link.c/////////////////////////////////////////////////
//--------------------------------------------------
//��������:  ��Ӧ���Ĵ���
//         
//����:      TYPE_PORT PortType[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
void Proc645LinkDataRequest( eSERIAL_TYPE PortType, BYTE *pBuf );
//--------------------------------------------------
//��������:  ��Ӧ���Ĵ���
//         
//����:      TYPE_PORT PortType[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
void Proc645LinkDataResponse( eSERIAL_TYPE PortType, WORD wResult);
//-----------------------------------------------
// ��������  : 645��ԼԤ����
//
// ����  :    TSerial *p[in]
//
// ����ֵ:     BOOL ��TRUE-��Ҫ��ת�����߱������鴦��
// ��ע����  :  ��
//-----------------------------------------------
void ProMessage_Dlt645( TSerial *p );
//---------------------------------------------------------------
//��������: �ж��·������Ƿ�㲥��ַ�����б�ĵط����ô˺���������ȡ��
//
//����: 	pBuf[in] - ����ͨ�ŵ�ַ
//                   
//����ֵ:   TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
WORD JudgeBroadcastMeterNo(BYTE *pBuf);
WORD JudgeRecMeterNo_645(BYTE *pBuf);
#endif//#if( SEL_DLT645_2007 == YES )

#endif//��Ӧ�ļ���ʼ�� __DLT_645_2007_STRUCT


