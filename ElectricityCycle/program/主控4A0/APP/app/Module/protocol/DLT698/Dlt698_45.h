//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���ĺ� κ����
//��������	2016.8.4
//����		DL/T 698.45�������Э��ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __DLT698_45_H
#define __DLT698_45_H

#include "Dlt645_2007.h"

#if(SEL_DLT698_2016_FUNC == YES  )
//-----------------------------------------------
//				�궨��
//-----------------------------------------------
// һ֡���Ĵ�������ݿ�Խ��һ����������Ŀռ�Խ�࣬Խ��Ҫ���ٸ���Ŀռ䣬Ŀǰ�Ȱ���һ֡400�ֽ����������ٿռ䣨200�ֽں�400�ֽ�Ч��һ����
#define MAX_SIZE_FOR_IAP						(512 * 1024)									// ���������Ŀռ�  
#define LEN_OF_ONE_FRAME						100												// һ֡���ĵĳ���
#define FRAMENUM_OF_ONE_SECTOR					(SECTOR_SIZE / LEN_OF_ONE_FRAME)				//һ�������ܹ���ı�����20
#define FRAMESIZE_OF_ONE_SECTOR					(FRAMENUM_OF_ONE_SECTOR * LEN_OF_ONE_FRAME) 	//һ�������ܹ����ֽ���
#define SECTOR_NUM_FOR_IAP						((MAX_SIZE_FOR_IAP / FRAMESIZE_OF_ONE_SECTOR)+1)// ��MAX_SIZE_FOR_IAP���������ݣ���Ҫ���ٸ�����132
#define IAP_CODE_ADDR							((2048 - SECTOR_NUM_FOR_IAP) * SECTOR_SIZE)		// ������ 0~2047
#define IAP_INFO_ADDR							((2048 - SECTOR_NUM_FOR_IAP- 1) * SECTOR_SIZE )	// ������ 0~2047  ����һ�������������������Ϣ
#define	IAP_JUDGE_ADDR							((2048 - SECTOR_NUM_FOR_IAP- 2) * SECTOR_SIZE )	//������ 0~2047	��ŵ�һ֡�ж���Ϣ
#define IAP_FLAG								0xA55AA55A
#define IAP_START								0x5AA55AA5	// ������������������
#define IAP_READY								0x5AA5A55A	// ������ִ����������
#define IAP_ALL									0x5AA5		// ȫ��������־

#define DEFAULT_MAX_PRO_BUF_485     512
#define DEFAULT_MAX_PRO_BUF_CR      512
#define DEFAULT_MAX_PRO_BUF_IR      200
#define MIN_CONSULT_BUF_LEN         45//ֵ����С��45


#define APP_BUF_SIZE			(MAX_PRO_BUF - DLT_LINK_DATA_HEAD -DLT_LINK_DATA_TAIL -6 -2)//-6 ͨѶ��ַ -2 ʱ���ǩ�͸����ϱ�
#define APP_APDU_BUF_SIZE		(MAX_APDU_SIZE - DLT_LINK_DATA_HEAD -DLT_LINK_DATA_TAIL -6 -2)
/**************************************��·���ʽ���ȶ���************************************/
#define	DLT_START_BYTE				1				//��ʼ�ַ�����
#define	DLT_LENTH					2				//�����򳤶�
#define	DLT_CONTROL_BYTE		    1				//�����򳤶�
#define	DLT_ADDRESS_ELSE_LEN		2				//��ַ����������
#define	DLT_CSLEN					2				//У�鳤��
#define	DLT_END_BYTE				1				//�����ֽڳ���
#define	DLT_LINK_DATA_HEAD			(DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+DLT_CSLEN)//������ʵ�ʵ�ַ����
#define	DLT_LINK_DATA_TAIL			( DLT_CSLEN + DLT_END_BYTE )
#define	DLT_FRAME_BYTE				2				//��ַ����������

#define COMM_TYPE_MASTER		(0xA0)				//��վͨ��
#define COMM_TYPE_TERMINAL		(0x00)				//�ն�ͨ��
#define COMM_TYPE_MASK			(0xF0)				//ͨ���豸����
#define CURR_COMM_TYPE(Ch) (LinkData[Ch].ClientAddress&COMM_TYPE_MASK)	//��ǰ��ͨ������ �ն˻�����վ

/**************************************Ӧ�ò㶨��********************************************/
#define	CONNECT_REQUEST				2	
#define	RELEASE_REQUEST				3			
#define	GET_REQUEST					5	
#define	SET_REQUEST					6	
#define	ACTION_REQUEST				7	
#define	REPORT_RESPONSE				8		
#define	PROXY_REQUEST				9	
#define	SECURITY_REQUEST			16
#define	ERROR_REQUEST			    110


#define	CONNECT_RESPONSE			130	
#define	RELEASE_RESPONSE			131		
#define	RELEASE_NOTIFICATION		132		
#define	GET_RESPONSE				133		
#define	SET_RESPONSE				134		
#define	ACTION_RESPONSE				135		
#define	REPORT_NOTIFICATION			136		
#define	PROXY_RESPONSE				137	
#define	SECURITY_RESPONSE			144
#define	ERROR_RESPONSE			    238

#define	GET_REQUEST_NORMAL			1	
#define	GET_REQUEST_NORMALLIST		2
#define	GET_REQUEST_RECORD			3
#define	GET_REQUEST_RECORDLIST		4
#define	GET_REQUEST_NEXT			5

#define	GET_RESPONSE_NORMAL			1	
#define	GET_RESPONSE_NORMALLIST		2
#define	GET_RESPONSE_RECORD			3
#define	GET_RESPONSE_RECORDLIST		4
#define	GET_RESPONSE_NEXT			5

#define	SET_REQUEST_NORMAL			1	
#define	SET_REQUEST_NORMALLIST		2
#define	SET_THEN_GET_REQUEST		3

#define	SET_RESPONSE_NORMAL			1	
#define	SET_RESPONSE_NORMALLIST		2
#define	SET_THEN_GET_RESPONSE		3

#define	ACTION_REQUEST_NORMAL		1	
#define	ACTION_REQUEST_NORMALLIST	2
#define	ACTION_THEN_GET_REQUEST		3
#define	REPORT_RESPONSE_LIST   		1
#define	REPORT_RESPONSE_RECORD_LIST 2

#define	ACTION_RESPONSE_NORMAL		1	
#define	ACTION_RESPONSE_NORMALLIST	2
#define	ACTION_THEN_GET_RESPONSE	3

#define PROXY_GET_REQUEST_LIST				1
#define PROXY_GET_REQUEST_RECORD			2
#define PROXY_SET_REQUEST_LIST				3
#define PROXY_SET_THEN_GET_REQUEST_LIST		4
#define PROXY_ACTION_REQUEST_LIST			5
#define PROXY_ACTION_THEN_GET_REQUEST_LIST	6
#define PROXY_TRANS_COMMAND_REQUEST			7
#define PROXY_INNER_TRANS_COMMAND_REQUEST	8

#define PROXY_GET_RESPONSE_LIST				1
#define PROXY_GET_RESPONSE_RECORD			2
#define PROXY_SET_RESPONSE_LIST				3
#define PROXY_SET_THEN_GET_RESPONSE_LIST	4
#define PROXY_ACTION_RESPONSE_LIST			5
#define PROXY_ACTION_THEN_GET_RESPONSE_LIST	6
#define PROXY_TRANS_COMMAND_RESPONSE		7
#define PROXY_INNER_TRANS_COMMAND_RESPONSE	8

//bit0 Ӧ�ò������ݷ��� bit1 ��ȫ���� bit2��·��֡ bit3 Ӧ�÷�֡

#define APP_HAVE_DATA				0X01  //Ӧ�ò������ݷ���
#define APP_NO_DATA					0X02  //Ӧ�ò�û�����ݷ���
#define APP_LINK_FRAME				0X04  //��·��֡
#define APP_ERR_DATA				0X08  //�쳣Ӧ��

/****************************************�������Ͷ���**********************************/
//		����		  				��ֵ		����                 �ֽ���
#define NULL_698					0
#define Array_698					1		//����			     ָ���������
#define Structure_698				2		//�ṹ��			 ָ����Ա
#define Boolean_698			        3		//������			 1
#define Bit_string_698				4		//�ַ���			 1
#define Double_long_698 			5		//�з��ų����� Integer32_698	 4  -ԭdlms�е�
#define Double_long_unsigned_698 	6		//�з��ų����� Integer32_698	 4  -698.45�е�
#define Octet_string_698			9		//16�����ַ���	     ָ������
#define Visible_string_698			10		//�ɼ��ַ�		     ָ������
#define UTF8_string_698				12		//16�����ַ���	     ָ������
#define BCD_698   					13		//BCD��				 1
#define Integer_698	    			15		//�з��Ŷ�����		 1 
#define Long_698					16		//�з�������Integer16_		 2  --698.45�е�
#define Unsigned_698				17		//�޷��Ŷ�����		 1
#define Long_unsigned_698			18		//�޷�������Unsigned16_		 2  --698.45�е�
#define Long64_698					20		//�з��ų�����Integer64_		 8  --698.45�е�
#define Long64_unsigned_698			21		//�޷��ų�����Unsigned64_		 8  --698.45�е�
#define Enum_698					22		//ö����			 1
#define Float32_698					23		//������			 4
#define Float64_698					24		//˫���ȸ�����		 8
#define Date_time_698				25		//����ʱ����		 12
#define Date_698					26		//����				 5
#define Time_698					27		//ʱ��				 4
#define DateTime_S_698				28		//����ʱ������		 6	
#define OI_698						80		//��������������     5  Object Attribute Descriptor
#define OAD_698						81		//���󷽷�����		 4  Object Method Descriptor
#define ROAD_698					82	
#define OMD_698						83	
#define TI_698						84
#define TSA_698						85
#define MAC_698						86
#define RN_698						87
#define Region_698					88
#define Scaler_Unit_698				89
#define RSD_698						90
#define CSD_698						91
#define MS_698						92
#define SID_698						93
#define SID_MAC_698					94	
#define COMDCB_698					95	
#define RCSD_698					96	

#define	U_a						 	1	//	a		ʱ��			��	
#define	U_mo					 	2   //	mo		ʱ��			��	
#define	U_wk						3	//	wk		ʱ��			��			7*24*60*60s
#define	UNIT_dd						4	//	d		ʱ��			��			24*60*60s
#define	UNIT_hh						5	//	h		ʱ��			Сʱ		60*60s
#define	UNIT_Min					6	//	min		ʱ��			��			60s
#define	UNIT_S						7	//	s		ʱ��(t)			��			s
#define	UNIT_0						8	//	��		(��)��			��			rad*180/��
#define	UNIT_0C						9	//	��		�¶ȣ�T��		���϶�		K-273.15
#define	UNIT_YUAN					10	//	����	(����)����		
#define	UNIT_M						11	//	m		����(l)			��			m
#define	UNIT_MPS					12	//	m/s		�ٶ�(v)			��/��		m/s
#define	UNIT_M3						13	//	m3		��(��)��(V)		������		m3
#define	UNIT_M3_T					14	//	m3		���������		������		m3
#define	UNIT_M3Ph					15	//	m3/h	����			������/h	m3/(60*60s)
#define	UNIT_M3Ph_T					16	//	m3/h	����������		������/h	m3/(60*60s)
#define	UNIT_M3Pd					17	//	m3/d	����			������/d	m3/(24*60*60s)
#define	UNIT_M3Pd_T					18	//	m3/d	����������		������/d	m3/(24*60*60s)
#define	UNIT_L						19	//	l		�ݻ�			��			10^-3m3
#define	UNIT_kg						20	//	kg		����(m)			ǧ��		kg
#define	UNIT_N						21	//	N		��(F)			ţ��		N
#define	UNIT_Nm						22	//	Nm		����			ţ����		J=Nm=Ws
#define	UNIT_P						23	//	P		ѹ��(p)			��˹��		N/m2
#define	UNIT_bar					24	//	bar		ѹ��(p)			��			10^-5N/m2
#define	UNIT_J						25	//	J		����			����		J=Nm=Ws
#define	UNIT_JPh					26	//	J/h		�ȹ�			��ÿСʱ	J/(60*60s)
#define	UNIT_W						27	//	W		�й�����(P)		��			W=J/s
#define	UNIT_KW						28	//	kW		�й�����(P)		ǧ��		kW=J/(s*1000)
#define	UNIT_VA						29	//	VA		���ڹ���(S)		����	
#define	UNIT_KVA					30	//	kVA		���ڹ���(S)		ǧ����	
#define	UNIT_VAR					31	//	var		�޹�����(Q)		��	
#define	UNIT_KVAR					32	//	kvar	�޹�����(Q)		ǧ��	
#define	UNIT_KWH					33	//	kWh		�й�����rw,		ǧ��-ʱ		kW*(60*60s)
#define	UNIT_KVAH					34	//	kVAh	��������rS,		ǧ��-��-СʱkVA*(60*60s)
#define	UNIT_KVARH					35	//	kvarh	�޹�����rB,		ǧ��-ʱ		kvar*(60*60s)
#define	UNIT_A						36	//	A		������I��		����		A
#define	UNIT_C						37	//	C		������Q��		����		C=As
#define	UNIT_V						38	//	V		��ѹ��U��		����		V
#define	UNIT_VPm					39	//	V/m		�糡ǿ�ȣ�E��	��ÿ��		V/m
#define	UNIT_F						40	//	F		���ݣ�C��		����		C/V=As/V
#define	UNIT_ohm					41	//	��		���裨R��		ŷķ		��=V/A
#define	UNIT_ohmPm					42	//	��m2/m	����ϵ�����ѣ�				��m
#define	UNIT_Wb						43	//	Wb		��ͨ��������		Τ��		Wb=Vs
#define	UNIT_T						44	//	T		��ͨ�ܶȣ�T��	̩˹��		Wb/m2
#define	UNIT_APm					45	//	A/m		�ų�ǿ�ȣ�H��	����ÿ��	A/m
#define	UNIT_H						46	//	H		��У�L��		����		H=Wb/A
#define	UNIT_Hz						47	//	Hz		Ƶ��			����		1/s
#define	UNIT_1PWh					48	//	1/(Wh)	�й���������������		imp/kWh
#define	UNIT_1Pvarh					49	//	1/(varh)�޹���������������		imp/kvarh
#define	UNIT_1PVAh					50	//	1/(VAh)	������������������		imp/kVAh	
#define	UNIT_PCT					51	//	%		�ٷֱ�			�ٷ�֮	
#define	UNIT_BYTE					52	//	byte	�ֽ�			�ֽ�	
#define	UNIT_DBM					53	//	dBm		�ֱ�����		
#define	UNIT_PRICE					54	//	Ԫ/kWh	���		
#define	UNIT_AH						55	//	Ah		��ʱ			��ʱ	
#define	UNIT_MS						56	//	Ah		��ʱ			��ʱ	
//							56��253	//	����
#define	UNIT_UNSUPO					250
#define	UNIT_OTHER					254	//	������λ
#define	UNIT_NO						255	//	�޵�λ��ȱ��λ������



/****************************************���������ֽ������ȶ���*********************************/
#define OAD_698_LENTH		 	4
//��ʾ��ȫе������ౣ�������,�����30��������

//then_get���֧��list����
#if( MAX_FREEZE_ATTRIBUTE > MAX_EVENT_OAD_NUM )
#define MAX_ATTRIBUTE MAX_FREEZE_ATTRIBUTE
#else
#define MAX_ATTRIBUTE MAX_EVENT_OAD_NUM
#endif

//-----------------------------------------------

typedef enum
{
	eNO_FRAME = 0,          	//�޷�֡
	eLINKDATA_FAIL,         	//ʧ�� 
	eDOWN_FRAMEING,     		//���з�֡������
	eDOWN_FRAME_END,    		//���з�֡����
	eUP_FRAMEING,         		//���з�֡������
	eUP_FRAME_END,        		//���з�֡����
	eAPP_OK                   	//Ӧ�ò�����OK   
}eFrameResultChoice;//Ӧ�ò�buf��ʽ���

typedef enum
{
   	eUNION_TYPE,         		//����������
   	ePROTOCOL_TYPE,    			//Э������
   	eESAM_TYPE,        			//ESAM����
   	eUNION_OFFSET_TYPE,       	//�����峤�ȷ���
	eUNION_BITSTRING_TYPE,
}eDataLenTypeChoice;//���ݳ������� ������������

typedef enum
{
	eADD_OK = 0,				//��ӳɹ�
	eAPP_LINKDATA_OK,         	//��·���֡������
	eAPP_NO_RESPONSE,			//��Ӧ��
	eAPP_ERR_RESPONSE,          //����Ӧ��
	eAPP_RETURN_DATA,			//ǿ�ƽ������ݷ���
}eAPPBufResultChoice;//������ݷ��ؽ��

typedef enum
{
	eNO_SECURITY =0,		//���ǰ�ȫ����
	eSECURITY_FAIL,			//��ȫ����ʧ��
	eRNMAC,					//�����+MAC
	eSECURITY_RN,
	eEXPRESS_MAC,		    //����+mac 
	eSECRET_TEXT,			//����
	eSECRET_MAC             //����+mac
} eSecurityMode;//��ȫ��������

typedef enum
{	
	eNO_TimeTag =0,              //��ʱ���ǩ
	eTime_True = 0x22,           //��ʱ���ǩ ��ʱ���ǩ��Ч
	eTime_Invalid = 0x55         //ʱ���ǩ��Ч
}eTimeTagFlag;//ʱ���ǩ��־

typedef enum
{	
	eclass7 =0,
	eclass8 =1,
	eclass17 =2,
	eclass21 =3,
	eclass22 =4,
	eclass24 = 0x20,
	eclass18 = 0x21,
	eclassTag = 0x55
}eClass;//OTHER ������ ���ö��

typedef enum
{	
	eREAD_MODE,
	eSET_MODE,
	eACTION_MODE,
	eOTHER_MODE  //������ģʽ����delay�Ƿ��ж�
}eRequestMode;//��������

typedef enum
{	
	eSINGLE_ADDRESS_TYPE = 0,	//����ַ
	eWILDCARD_ADDRESS_TYPE = 1,	//ͨ���ַ
	eGROUP_ADDRESS_TYPE = 2,	//���ַ
	eBROADCAST_ADDRESS_MODE = 3	//�㲥��ַ
}eAddressType;//��������
/*****************************��·��ṹ��***********************/
typedef struct TDLT698LinkFrame_t   	//��·��ṹ��
{
	TTwoByteUnion FrameType;       		//��֡��ʽ��(2���ֽ�)
	BYTE *pFrame;						//��֡APDUƬ����ʼ��ַ
	TTwoByteUnion FrameLen;        		//��֡APDUƬ�γ���
}TDLT698LinkFrame;


typedef struct TDLT698LinkData_t		//��·��ṹ��
{
	TTwoByteUnion  DataLen;				//���ݳ���
	BYTE ControlByte;					//�����ֽ�
	BYTE AddressLen;					//��ַ����
	BYTE ServerAddr[6];					//��������ַ
	BYTE ClientAddress;					//�ͻ�����ַ
	eAddressType AddressType;			//��ַ����
	eFrameResultChoice eLinkDataFlag;
	BYTE* pAPP;							//Ӧ�ò���ʼָ��
	TTwoByteUnion pAPPLen;				//Ӧ�ò㳤��
	TDLT698LinkFrame  DownLinkFrame;	//����ʹ��
	TDLT698LinkFrame  UpLinkFrame;		//����ʹ��	
}TDLT698LinkData;

/*****************************Ӧ�ò�ṹ��***********************/
typedef struct TDLT698APPData_t   		//Ӧ�ò�ṹ��
{
	BYTE APPFlag;						//Ӧ�ò��־ bit0 Ӧ�ò������ݷ��� bit1 ��ȫ���� bit2��·��֡
	eTimeTagFlag TimeTagFlag;			//ʱ���ǩ��־
	BYTE RequestType;					//����Requestģʽ 
	eSecurityMode eSafeMode;			//��ȫģʽ
	BYTE* pAPPBuf;						//Ӧ�ò�bufָ��
	TTwoByteUnion APPBufLen;			//Ӧ�ò� ����֡�������ݵ�buf����
	TTwoByteUnion APPCurrentBufLen;		//Ӧ�ò� ʵ������֡����buf����
	TTwoByteUnion APPMaxBufLen;			//Ӧ�ò����buf����
	TTwoByteUnion BufDealLen;			//���������Ѵ�������ݳ���(���list)(ֻ����·����ݣ�������˵buf���˶�������)	
}TDLT698APPData;

/*****************************����֡�ṹ��*********************************/
typedef struct TTagPara_t   	//��TAGʹ�õĲ���
{
	BYTE Array_Struct_Num;//array��struct�ĸ���
	TTwoByteUnion Lenth;
}TTagPara;

typedef struct TTagBuf_t   	//��TAGʱBufʹ�õĲ���
{
	WORD InBufLenth;
	WORD OutBufLenth;
}TTagBuf;

typedef struct TScaler_Unit_t//�����뵥λ�ṹ��
{
	signed char 	Scaler;		//����
	BYTE	 		Unit;		//��λ
}TScaler_Unit;	

typedef struct TCommonObj_t
{
	WORD				OI;				//698�����ʶ��OI�����ʶ
	const TScaler_Unit 	*Scaler_Unit3;	//���ܻ��㵥λ
	const BYTE*         Type2;			//���������ݱ�ʶ
}TCommonObj;

typedef struct TAPDUFlag_t
{
    BYTE Ch;    //��ǰͨ��
    BYTE Flag;  //true ��ռ�� false δռ��
}TAPDUFlag;

typedef struct TAPDUBufFlag_t
{
   TAPDUFlag Request;
}TAPDUBufFlag;

typedef struct TConsultData_t   	//��TAGʱBufʹ�õĲ���
{
	BYTE channel;
	WORD Len;
}TConsultData;

#pragma pack(1)
typedef struct TIap_t
{
	BYTE bDestFile[10];
	BYTE bSourceFile[10];
	DWORD dwFileSize;  // �����ļ���С��Byte��
	BYTE bFileAttr;	   // �ļ�����
	BYTE bFileVer[10]; // �ļ��汾
	BYTE bFileType;	   // �ļ����
	WORD wFrameSize;   // ������С��Ĭ��200�ֽ�
	WORD wAllFrameNo;  // �����ļ��ָ�����֡��
	WORD wFrameOfOneSector;  // һ���������Դ�ŵ�����֡��
	WORD wCrc16;	   // ���������ļ���CRC16У����
	WORD wAllUpdate;   // 1:ȫ������  0����������
	DWORD dwIapFlag;   // ������־λ  0xA55AA55A ��Ч��������Ч
	DWORD dwCrc32;	   // �ṹ��У��
} T_Iap;
typedef struct  {
	void* pgetcfun;
	DWORD oadd;
	WORD offset;
	BOOL FZtype;//����֡ʱʹ��
	BYTE GetrequsetNextType;//��֡����
	WORD SlicingIndex;
	BYTE IslastFrame;
	//BYTE pPara[MAX_APDU_SIZE+EXTRA_BUF];
	BYTE pPara[MAX_PRO_BUF+EXTRA_BUF];

} TGetRequestNext;
typedef struct TIap_Info
{
	DWORD dwTime;
	DWORD dwFlag;
	WORD wFacFlag;	  // 0:�Ƿ�  1���Ϸ�
	WORD wCurFrameNo; // ��ǰ����֡���
    WORD wCRC16;
} TIapInfo;
#pragma pack()
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern T_Iap tIap;
extern TIapInfo tIapInfo;
extern BYTE IapStatus[(MAX_SIZE_FOR_IAP / LEN_OF_ONE_FRAME) / 8 + 1]; // F001 ������4 ����״̬��������Ϊ����ʵ��һ֡����ĳ��ȿ��ٿռ�!!!!!!
extern __no_init BYTE g_PassWordErrCounter;//����������
extern BYTE RN[MAX_RN_SIZE];
extern TTagPara TagPara;
extern TDLT698LinkData LinkData[MAX_COM_CHANNEL_NUM];
extern TDLT698APPData APPData[MAX_COM_CHANNEL_NUM];
extern BYTE DLT698APDURequestBuf[MAX_APDU_SIZE+EXTRA_BUF];
extern BYTE ProtocolBuf[MAX_APDU_SIZE+EXTRA_BUF];
extern BYTE g_TimeTagBuf[];
extern TGetRequestNext pGetRequestNext[MAX_COM_CHANNEL_NUM];
extern const TScaler_Unit SU_1V;	
extern const TScaler_Unit SU_3A;	
extern const TScaler_Unit SU_10;
extern const TScaler_Unit SU_1W; 
extern const TScaler_Unit SU_1VAR; 	
extern const TScaler_Unit SU_1VA;
extern const TScaler_Unit SU_3; 
extern const TScaler_Unit SU_2;
extern const TScaler_Unit SU_2HZ; 
extern const TScaler_Unit SU_10C;
extern const TScaler_Unit SU_2v; 
extern const TScaler_Unit SU_0MIN; 
extern const TScaler_Unit SU_00;
extern const TScaler_Unit SU_4KW;	
extern const TScaler_Unit SU_4KVAR;	
extern const TScaler_Unit SU_4KVA;	
extern const TScaler_Unit SU_2PCT;
extern const TScaler_Unit SU_2AH;	
extern const TScaler_Unit SU_2kwh;	
extern const TScaler_Unit SU_0MS;	
extern const TScaler_Unit SU_01Kwh;	
extern const TScaler_Unit SU_01Kvar;
extern const TScaler_Unit SU_2Yuan;
extern const TScaler_Unit SU_4Yuan;
extern const TScaler_Unit SU_2S;

extern const BYTE T_ScalerUnit[];	//ScalerUnit���ݱ�ʶ
extern const BYTE T_Unsigned[];		//long_unsigned���ݱ�ʶ
extern const BYTE T_UNLong[];		//long_unsigned���ݱ�ʶ
extern const BYTE T_Long[];			//Long���ݱ�ʶ
extern const BYTE T_DoubleLong[];	//DoubleLong���ݱ�ʶ
extern const BYTE T_UNDoubleLong[];	//DoubleLong���ݱ�ʶ
extern const BYTE T_BitString16[];	//long_unsigned���ݱ�ʶ
extern const BYTE T_BitString32[];
extern const BYTE T_BitString8[];	//long_unsigned���ݱ�ʶ
extern const BYTE T_DateTimS[];		//DateTime_S���ݱ�ʶ
extern const BYTE T_Enum[];			//Enum���ݱ�ʶ
extern const BYTE T_VisString32[];	//VisString32���ݱ�ʶ
extern const BYTE T_VisString16[];	//VisString16���ݱ�ʶ
extern const BYTE T_VisString6[];	//VisString6���ݱ�ʶ
extern const BYTE T_VisString4[];	//VisString4���ݱ�ʶ
extern const BYTE T_OctetString5[];	//OctetString5���ݱ�ʶ
extern const BYTE T_OctetString7[];
extern const BYTE T_OctetString8[];	//OctetString8���ݱ�ʶ
extern const BYTE T_OctetString16[];//OctetString16���ݱ�ʶ
extern const BYTE T_OctetStringFF[];	//OctetStringFF���ݱ�ʶ
extern const BYTE T_TI[];			//TI���ݱ�ʶ
extern const BYTE T_Bool[];			//Bool���ݱ�ʶ
extern const BYTE T_Special[];		//���⴦��
extern TAPDUBufFlag APDUBufFlag;
extern BYTE ClientAddress;//�ͻ�����ַ
extern BYTE ConnectMode;  //����ģʽ
extern BYTE FrameOverTime;//��֡��ʱʱ��
extern __no_init BYTE RN_MAC[50]; //�ݴ�RN_MAC  -- ���粻�����岻�󣬵��ǿ��ܻ�����λ���յ�ȷ�ϱ���Ҳ����֤��
extern __no_init BYTE RN_MAC_BAK[50];   	//RN_MAC����

//-----------------------------------------------
// 				��������
//-----------------------------------------------

//--------------------------------------------------
//��������:  ��·����������
//         
//����:      BYTE Ch[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void Proc698LinkDataRequest( BYTE Ch );

//--------------------------------------------------
//��������:  ��·��������Ӧ����
//         
//����:      BYTE Ch[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
BYTE Proc698LinkDataResponse( BYTE Ch);

//-----------------------------------------------
//��������  : 698.45������
//����  : 	TSerial *p[in] 
//����ֵ: 
//��ע����  :  ��
//-----------------------------------------------
void ProcMessageDlt698(TSerial *p);

BOOL Pre_Dlt698(TSerial *p, BYTE *pAck);
BOOL IsInSysMeters(BYTE *pMeterAddr);
BOOL IsChargingPileFrame(BYTE *pBuf);
BYTE api_GetChargingPileCanID(BYTE* bAddr);
//-----------------------------------------------
//��������  :   ��ʼ��698����
//����  : 	��
//����ֵ: 
//��ע����  :  ��
//-----------------------------------------------
void FactoryInitDLT698(void);

//--------------------------------------------------
//��������:ʱЧ�������ж� �������Ͽ�Ӧ�����ӡ�����ʱЧ����ʱ��ȡ��
//         
//����  :   ��
//
//����ֵ:    ��   
//         
//��ע����:  ��
//--------------------------------------------------
void TimeLinessEventJudge( void );


//--------------------------------------------------
//��������:  ��ѭ�����㺯��
//         
//����  : ��
//
//����ֵ:    
//         
//��ע����:  ÿ500�������һ��
//--------------------------------------------------
void ProcClrTask(void);

//--------------------------------------------------
//��������:  �������������Tag
//         
//����:      *Tag[in]    Tag[0]--��������  Tag[1]--���ݳ��� Tag[n]--����   
//           *InBuf[in]	 ��Ҫ���tag������        
//           *OutBuf[in] ���tag�����������
//         
//����ֵ:    ���tag����������ݳ���
//         
//��ע����:  ��InBuf�е����ݰ���Tag��ǩ��Ҫ����ӱ�ǩ�������OutBuf 
//--------------------------------------------------
WORD GetRequestAddTag( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf );

//-----------------------------------------------
//��������: 698���ݳ��ȴ���
//
//����:		BYTE *buf  //���������
//			BYTE len   //��������ݳ���	
//			BYTE mode  // 0 ����requense ���ݳ��� 1����request���ݳ��� 0��len.wת�����Ϲ�Լ�ĸ�ʽ 1�ǽ���Լ����ת��Ϊlen.w		
//����ֵ:	��
//		   
//��ע:698���ݳ��ȴ�����Ҫ�����������ݳ��ȳ���һ���ֽڵ���� �����Ż�����,�˺��������Խ�����չ
//-----------------------------------------------
BYTE Deal_698DataLenth( BYTE *buf, BYTE *Datalen, eDataLenTypeChoice  eTypeChoice );


//--------------------------------------------------
//��������:  ��ȡ������
//����	:
//			pBuf[out]
//����ֵ:	TRUE	FALSE
//ע:����ʱ������ΪHEX
//--------------------------------------------------
WORD ReadSettlementDate(BYTE *pBuf);

//--------------------------------------------------
//��������:  698��ȡ����OAD����
//         
//����:	
//		GetDataType[in]	GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//		Ch[in]:			��Լͨ��
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
WORD GetProOadData( eGetDataType GetDataType, BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//��������:  �������ͻ�ȡ���ݳ���
//         
//����:      DataType[in]��eData/eTagData        
//           Tag[in]��
//         					Tag[0]--��������
//         					Tag[1]--��Ա����
//         					Tag[n]--����
//����ֵ:    �������ݳ���
//         
//��ע����: ֧��array �� array+struct+Normal��ʽ
//--------------------------------------------------
WORD GetTypeLen( BYTE DataType, const BYTE *Tag );

//--------------------------------------------------
//��������:  ��ȡ����
//         
//����:     
//			 GetDataType[in] GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//			 DataType[in]��	eData/eTagData/eAddTag        
//           Dot[in] :		С����        
//           *pOAD[in]��	    OAD         
//           *InBuf[in]��	��Ҫ��ӱ�ǩ������         
//           OutBufLen[in]�� ����OutBuf�����ݳ���         
//           *OutBuf[in]��	���صĵ�������
//         
//����ֵ:    ���ص��ܳ��� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ���ʸ������ܳ���12
//--------------------------------------------------
WORD GetRequestEnergy( eGetDataType GetDataType, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//��������:  ����TAG��ȡ���ܳ���
//         
//����:      
//			GetDataType[in]	GetNormalData��ȡ�������� GetRecordData 	 ��ȡ��¼����----ע����ܷ�������������������¼�ʹ�ã����������������-jwh
//			DataType[in]��	eData/eTagData
//			*pOAD[in]��		OAD
//         
//����ֵ:   ����OAD��Ӧ���ݵĳ���  0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetRequestEnergyLen( eGetDataType GetDataType, BYTE DataType, BYTE *pOAD);


//--------------------------------------------------
//��������:  ����Ԫ����������TCommonObj_t��Type2��ƫ�Ƶ�ַ
//         
//����:      ClassIndex[in]��	Ԫ������         
//           *Tag[in]��			��ǩ
//         
//����ֵ:    ���� TCommonObj �� Type2 ��ƫ��
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetTagOffsetAddr(BYTE ClassIndex, const BYTE *Tag);

#if( MAX_PHASE != 1 )
//--------------------------------------------------
//��������:  ��ȡ����
//         
//����:      DataType[in]��	eData/eTagData/eAddTag        
//           Dot[in] :		С����        
//           *pOAD[in]��	OAD         
//           *InBuf[in]��	��Ҫ��ӱ�ǩ������         
//           OutBufLen[in]������OutBuf�����ݳ���         
//           *OutBuf[in]��	���صĵ�������
//         
//����ֵ:    ������������ 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ���ʸ������ܳ���12
//--------------------------------------------------
WORD GetRequestDemand( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//��������:  ����TAG��ȡ��������
//         
//����:      
//			DataType[in]��	eData/eTagData
//			*pOAD[in]��		OAD
//         
//����ֵ:   ����OAD��Ӧ���ݵĳ���  0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetRequestDemandLen( BYTE DataType, BYTE* pOAD);
#endif

//--------------------------------------------------
//��������:  ����TAG��ȡ��������
//         
//����:      
//			DataType[in]��	eData/eTagData
//			*pOAD[in]��		OAD
//         
//����ֵ:   ����OAD��Ӧ���ݵĳ���  0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetRequestVariableLen( BYTE DataType, BYTE* pOAD);

//--------------------------------------------------
//��������:  ��ȡ����
//         
//����:      DataType[in]��	eData/eTagData/eAddTag        
//           Dot[in] :		С����        
//           *pOAD[in]��	OAD         
//           *InBuf[in]��	��Ҫ��ӱ�ǩ������         
//           OutBufLen[in]������OutBuf�����ݳ���         
//           *OutBuf[in]��	���صĵ�������
//         
//����ֵ:    ���ر������� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  
//--------------------------------------------------
WORD GetRequestVariable( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );
//--------------------------------------------------
//��������:  ��ȡ��չģ�����
//         
//����:      	DataType[in]��	eData/eTagData/eAddTag
//         	Dot[in]:		С����(�ݲ���)        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:    ���ر������� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  
//--------------------------------------------------
WORD GetModuleRequestVariable(BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );
//--------------------------------------------------
//��������:  ��ȡ�α�����class8������ OI=4XXX
//         
//����:  
//         	DataType[in]��	eData/eTagData/eAddTag
//         	Dot[in]:		С����(�ݲ���)        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:  	�������ݳ���
//         
//��ע����: 
//--------------------------------------------------
WORD GetRequestMeterPara( BYTE Ch,BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//��������: ��ȡ�α�����class8�����ݳ��� OI=4XXX
//         
//����:     
//         	DataType[in]��	eData/eTagData
//         	*pOAD[in]��		OAD   
//         
//����ֵ:   �������ݳ��� 
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetRequestMeterParaLen( BYTE DataType, BYTE* pOAD);

//--------------------------------------------------
//��������:  ��ȡ����
//         
//����:     
//         	DataType[in]��	eData/eTagData
//         	Dot[in]:		С����        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:  	�������ݳ���
//         
//��ע����: ֻ����class9������3 �����������Ա�
//--------------------------------------------------
WORD GetProFreezeRecord( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//��������:  ��ȡ�������¼���������
//         
//����: 
//			Ch[in]��		��Լͨ��    
//         	DataType[in]��	eData/eTagData
//         	Dot[in]:		С����        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:  	�������ݳ���
//         
//��ע����: ������ class7������2����class24������6~9��
//--------------------------------------------------
WORD GetProEventRecord( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//��������: ��ȡ�����ӿ�����������
//         
//����:
//         	DataType[in]��	eData/eTagData/eAddTag
//         	Dot[in]:		С����        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:   ���ݳ���
//         
//��ע����: OtherObj��֧�ֵ�����
//--------------------------------------------------
WORD GetOtherData( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//��������: ��ȡ�����ӿ�������ݳ���
//         
//����:     
//          DataType[in]��eData/eTagData
//          *pOAD[in]     OAD    
//         
//����ֵ:   �������ݳ���
//         
//��ע����: 
//--------------------------------------------------
WORD GetOtherDataLen( BYTE DataType, BYTE *pOAD);

//--------------------------------------------------
//��������: ��ȡֻ��TAG�¼������ݳ���
//         
//����:     
//          DataType[in]��eData/eTagData
//          *pOAD[in]     OAD    
//          Number[in]��  ��Ա��
//         
//����ֵ:   �������ݳ���
//         
//��ע����:	�¼��ϱ�״̬
//			��̶����б�
//			�����ڼ��������ֵ
//			�¼������б� 
//--------------------------------------------------
WORD GetProEventUnitLen( BYTE DataType, BYTE *pOAD, BYTE Number );

//--------------------------------------------------
//��������:  ����698֡��ʽ���ж�
//         
//����:      BYTE ReceByte[in]
//         
//           TSerial * p[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
WORD DoReceProc_DLT698(BYTE ReceByte, TSerial * p);
WORD DoReceProc_13762(BYTE ReceByte, TSerial * p);
//--------------------------------------------------
//��������: Э���л�ȡDATA��ʵ�����ݳ���
//         
//����:      Buf[in]��  data����
//
//����ֵ:     WORD  �������ݳ���
//         
//��ע����:ע��ݹ�ʹ�� �����޷�����
//--------------------------------------------------
WORD GetProtocolDataLen( BYTE *Tag );

//--------------------------------------------------
//��������:  698.45�ϵ纯��
//         
//����  :   ��
//
//����ֵ:    ��  
//         
//��ע����:  ��
//--------------------------------------------------
void PowerUpDlt698(void);
//-----------------------------------------------
//��������  :   ��ʼ��698��־
//
//����  : 	��
//
//����ֵ:      ��
//
//��ע����  :   ��
//-----------------------------------------------
void InitDLT698Flag( BYTE Ch );

//--------------------------------------------------
//��������: ��ʼ����698ͨ����صı���
//
//����:		TSerial *p  //ͨ�ſڵ�ָ��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void InitPoint_Dlt698(TSerial *p);
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
WORD AddProtocolAndFunctionConformance( BYTE Mode,BYTE *pBuf );
//--------------------------------------------------
//��������: APDUBufFlag�ϵ��ʼ��
//         
//����  :    ��
//
//����ֵ:     ��   
//         
//��ע����:    ��
//--------------------------------------------------
void PowerUpInitAPDUBufFlag( void );
//--------------------------------------------------
//��������: APDUBufFlag��ʼ��
//         
//����  :    ��
//
//����ֵ:     ��   
//         
//��ע����:    ��
//--------------------------------------------------
void InitAPDUBufFlag( BYTE Ch );

//--------------------------------------------------
//��������: ��ȡ��ͨ��Ĭ�����֡buf����
//         
//����  :    BYTE Ch[in] ͨ��
//
//����ֵ:     Ĭ�ϳ���   
//         
//��ע����:    ��
//--------------------------------------------------
DWORD GetConsultBufLen( BYTE Ch );
//--------------------------------------------------
//��������:  
//         
//����:      
//
//����ֵ:    
//         
//��ע:  ����һ�η�֡����
//--------------------------------------------------
BOOL  SetNextPara(  BYTE Ch,void* pgetcfun,DWORD oadd,WORD offset,BYTE *pPara,BYTE GetrequsetNextType);

#endif//(SEL_DLT698_2016_FUNC == YES )
#endif//__DLT698_45_2016_API_H
