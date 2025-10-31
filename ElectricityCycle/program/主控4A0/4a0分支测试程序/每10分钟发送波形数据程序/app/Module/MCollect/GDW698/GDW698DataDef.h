
#ifndef GDW698_DATA_TYPE_DEF_H_
#define GDW698_DATA_TYPE_DEF_H_
#if(MD_BASEUSER	== MD_GDW698)
#include <stdlib.h>
#define INVALID_LEN		0xFFFF //��ʾ��Ҫ�������

//�������� ��Ҫ�޸�
#define TT_DATA_CJ	1	
#define TT_EVENT_CJ	2	
#define TT_TRANS_CJ	3	
#define TT_REPORT	4

typedef enum TRANS_DIR{ TD12, TD21 } TRANS_DIR; //ת������: TD12:1�β�->2�β�; TD21:2->1
	
typedef enum OBJ_DATA_TYPE{ 
	dt_null = 0,
	dt_array,
	dt_structure,
	dt_boolean,
	dt_bit_string,
	dt_double_long,
	dt_double_long_unsigned,
	dt_octet_str = 9,
	dt_visible_string,
	dt_UTF8_string = 12,
	dt_bcd,
	dt_Integer = 15,
	dt_long,
	dt_unsigned,
	dt_long_unsigned,
	dt_long64  = 20,
	dt_long64_unsigned,
	dt_enum,
	dt_float32,
	dt_float64,
	dt_date_time,
	dt_date,
	dt_time,
	dt_datetime_s = 28,

	dt_OI       = 80,
	dt_OAD      = 81,	
	dt_ROAD     = 82,
	dt_OMD      = 83,

	dt_TI = 84,
	dt_TSA = 85,
	dt_MAC  = 86,
	dt_Rand  = 87,
	dt_Region = 88,
	dt_Scaler_Unit = 89,

	dt_RSD = 90,
	dt_CSD = 91,
	dt_MS  = 92,
	dt_ESAM_SID = 93,
	dt_SID_MAC = 94,
	dt_COMDCB  = 95,
	dt_RCSD = 96,

	dt_PIID = 128,				//PIID��������
	dt_PIID_ACD,
	dt_FN,
	dt_FN_PIID,
	dt_DAR,
	dt_SPECIFIC = 0xFF,
}OBJ_DATA_TYPE;

//APDU����
typedef enum APDU_TYPE{
	ATE_NULL = 0,
	LINK_Request = 0x01,		//Ԥ������Ӧ	
	CONNECT_Request,			//����Ӧ����������	
 	RELEASE_Request,			//�Ͽ�Ӧ����������

	GET_Request=0x5,			//��ȡ����
	SET_Request,				//��������	
	ACTION_Request,				//��������
	REPORT_Response,			//�ϱ�Ӧ��
	PROXY_Request,				//��������
 	Security_Request=0x10,		//��ȫ��֤����

    LINK_Response = 0x81,		//Ԥ��������
	CONNECT_Response,			//����Ӧ��������Ӧ
 	RELEASE_Response,			//�Ͽ�Ӧ��������Ӧ
  	RELEASE_Notification,		//�Ͽ�Ӧ������֪ͨ

    GET_Response=133,			//��ȡ��Ӧ
    SET_Response,				//������Ӧ
    ACTION_Response,			//������Ӧ
    REPORT_Notification,		//�ϱ�֪ͨ
    PROXY_Response,				//������Ӧ
    Security_Response=144,		//��ȫ��֤��Ӧ

	ATE_DATA_FN = 200,
	ATE_UNIT_DATA, ATE_ADDR, ATE_REP,ATE_TP, ATE_TAIL,ATE_TAIL_DOWN//������������ӣ�������������AFN TYPE
} APDU_TYPE;

typedef	WORD	OI;				// �����ʶ

// ���ݷ��ʽ��ö��
typedef enum Data_Access_Result{
	dar_success						= 0,			// �ɹ�
	dar_hardware_fault					= 1,			// Ӳ��ʧЧ
	dar_temporary_failure				= 2,			// ��ʱʧЧ
	dar_read_write_denied				= 3,			// ������д
	dar_object_undefined				= 4,			// ����δ����
	dar_object_class_inconsistent		= 5,			// ����ӿ��಻����
	dar_object_unavailable				= 6,			// ���󲻴���
	dar_type_unmatched					= 7,			// ���Ͳ�ƥ��
	dar_scope_of_access_violated		= 8,			// Խ��
	dar_data_block_unavailable			= 9,			// ���ݿ鲻����
	dar_long_get_aborted				= 10,			// ��֡������ȡ��
	dar_no_long_get_in_progress			= 11,			// �����ڷ�֡����״̬
	dar_long_set_aborted				= 12,			// ��дȡ��
	dar_no_long_set_in_progress			= 13,			// �����ڿ�д״̬
	dar_invalid_blockno					= 14,			// ���ݿ������Ч
	dar_password_failure				= 15,			// �����/δ��Ȩ
	dar_speed_noaccess					= 16,			// ͨ�����ʲ��ܸ���
	dar_timezone_over					= 17,			// ��ʱ������
	dar_timeperiod_over					= 18,			// ��ʱ������
	dar_feerate_over					= 19,			// ��������
	dar_security_unmatched				= 20,			// ��ȫ��֤��ƥ��
	dar_recharge_repeat					= 21,			// �ظ���ֵ
	dar_esam_failure					= 22,			// ESAM��֤ʧ��
	dar_security_failure				= 23,			// ��ȫ��֤ʧ��
	dar_customeno_inconsistent			= 24,			// �ͻ���Ų�ƥ��
	dar_recharge_num_error				= 25,			// ��ֵ��������
	dar_recharge_value_over				= 26,			// ���糬�ڻ�
	dar_address_exception				= 27,			// ��ַ�쳣
	dar_symmetric_decryption_failure	= 28,			// �Գƽ��ܴ���
	dar_asymmetric_decryption_failure	= 29,			// �ǶԳƽ��ܴ���
	dar_signature_failure				= 30,			// ǩ������
	dar_meter_suspend					= 31,			// ���ܱ����
	dar_timeflag_invalid				= 32,			// ʱ���ǩ��Ч
	dar_request_timeout					= 33,			// ����ʱ
	dar_esam_p1p2_error					= 34,			// ESAM��P1P2����ȷ
	dar_esam_lc_error					= 35,			// ESAM��LC����
	dar_compara_failure					= 36,			// �ȶ�ʧ��
	dar_other_reason					= 255			// ����
}DAR;

typedef	BYTE	octet_string;

#pragma pack(1)
// ����
typedef struct tag_Date{
	WORD	year;		// ��
	BYTE	month;		// ��
	BYTE	mday;		// ��
	BYTE	wday;		// ��
}Date;

// ʱ��
typedef struct tag_Time{
	BYTE	hour;		// ʱ
	BYTE	minute;		// ��
	BYTE	second;		// ��
}Time;

typedef struct tag_DATETIME{
	WORD	year;		// ��
	BYTE	month;		// ��
	BYTE	mday;		// ��
	BYTE	wday;		// ��
	BYTE	hour;		// ʱ
	BYTE	minute;		// ��
	BYTE	second;		// ��
	WORD	msec;		// ����
}DATETIME;

typedef struct tag_DATETIME_S{
	WORD	year;		// ��
	BYTE	month;		// ��
	BYTE	day;		// ��
	BYTE	hour;		// ʱ
	BYTE	minute;		// ��
	BYTE	second;		// ��
}DATETIME_S;

// ʱ����
typedef struct tag_Time_Interval{
	BYTE    unit;		// �����λ(enum{0=�룬1=�֣�2=ʱ��3=�գ�4=�£�5=��})
	WORD    value;		// ���ֵ
}TI;

// ʱ�����Ϣ
typedef struct tag_TimeTag{
	DATETIME_S	tmSend;
	TI			tmDelay;
}TimeTag;

typedef enum tag_AddrFlag{
	AF_Single    = 0,
	AF_Adapter   = 1,
	AF_Group     = 2,
	AF_Broadcast = 3
}TAddrFlag;

// ��������������
typedef union Object_Attribute_Descriptor{
	DWORD			value;				// OADֵ
	struct
	{
		OI			nObjID;				// �����ʶ
		struct
		{
			BYTE	attID  : 5;			// ����ID
			BYTE	attPro : 3;			// ���Ա�ʶ�������¼���
		};
		BYTE		nIndex;				// ������������
	};
}OAD;

typedef union Object_Method_Descriptor{
	DWORD		value;		// OMDֵ
	struct	{		
		OI		nObjID;		// �����ʶ
		BYTE	nModID;		// ����ID
		BYTE	nMode;		// ����ģʽ
	};
}OMD;

#define OBJ_MAX_OADS		(32)		// ��¼��ѡ��������������ʵ����չ��

typedef struct Cognate_Object_Attribute_Descriptor{
	BYTE	nNum;						// �����������Ը���
	OAD		oad[OBJ_MAX_OADS];			// �������������б�
}PACK COAD, OADS;

// ��¼�Ͷ�������������
typedef struct Record_Object_Attribute_Descriptor{
	OAD		oadMain;		// ��OAD
	COAD	oadCols;		// ������������
}ROAD;

typedef struct Column_Selection_Descriptor{
	BYTE		choice;		// ���ͱ�ʶ
	union
	{
		OAD		oad;		// ��OAD
		ROAD	road;		// ��¼��OAD
	};
}CSD;

typedef struct tag_ComDCB{
	BYTE	baud;
	BYTE	parity;
	BYTE	databits;
	BYTE	stopbits;
	BYTE	flowctrl;
}ComDCB;

typedef struct tag_Variant{
	BYTE				type;
	union	{
		char			cVal;
		BOOL			bVal;
		BYTE			byVal;
		BYTE            enumVal;
		DATETIME		dtVal;
		Date			dVal;
		Time			tVal;
		DATETIME_S		tmVal;
		long			nVal;
		WORD			wVal;
		short			sVal;
		DWORD			dwVal;
		DLONG			llVal;
		UDLONG			ullVal;
		float			fVal;
		double			dbVal;
		OI				oi;
		TI				ti;
		TSA				tsa;
		OAD				oad;
		OMD				omd;
		CSD				csd;
		ComDCB			dcb;
		DAR				darr;
	};
}Variant;

typedef struct tag_Selector0{
	BYTE	nullv;
}Selector0;

typedef struct tag_Selector1{
	OAD		oad;
	Variant	value;
}Selector1;

typedef struct tag_Selector2{
	OAD		oad;
	Variant	from;
	Variant	to;
	Variant span;
}Selector2;

#define MAX_SELECTOR2_NUM	(4)

typedef struct tag_Selector3{
	WORD		nNum;
	Selector2	item[MAX_SELECTOR2_NUM];
}Selector3;

// �����
#define OBJ_MAX_MSGROUP		(32)	// �������
#define OBJ_MAX_MSNO		(192)	// ����û����������(�ݶ�II�ͼ���������������)

// һ���û�����
typedef struct Meter_Type{
	WORD size;
	BYTE type[OBJ_MAX_MSGROUP];
}MSTYPE;

// һ���û���ַ
typedef struct Meter_Addr{
	WORD size;
	TSA  add[OBJ_MAX_MSGROUP];
}MSADDR;

// һ���û��������
typedef struct Meter_No{
	WORD size;
	WORD idx[OBJ_MAX_MSNO];
}MSNO;

// �û���������
typedef struct Meter_Region_Type{
	BYTE	nType;
	BYTE	start;
	BYTE	end;
}MRTYPE;

// �û���ַ����
typedef struct Meter_Region_TSA{
	BYTE	nType;
	TSA		start;
	TSA		end;
}MRTSA;

// �û������������
typedef struct Meter_Region_Index{
	BYTE	nType;
	WORD	start;
	WORD	end;
}MRIDX;

// һ���û���������
typedef struct Meter_Set_Region_Type{
	WORD		size;
	MRTYPE		region[OBJ_MAX_MSGROUP];
}MSRT;

// һ���û���ַ����
typedef struct Meter_Set_Region_TSA{
	WORD		size;
	MRTSA		region[OBJ_MAX_MSGROUP];
}MSRA;

// һ���û������������
typedef struct Meter_Set_Region_Index{
	WORD	size;
	MRIDX	region[OBJ_MAX_MSGROUP];
}MSRI;

typedef enum tag_MS_CHOICE{
	MS_NULL			= 0,
	MS_ALL			= 1,
	MS_TYPE			= 2,
	MS_TSA			= 3,
	MS_INDEX		= 4,
	MS_REGION_TYPE	= 5,
	MS_REGION_TSA	= 6,
	MS_REGION_IDX	= 7
}MS_CHOICE;

typedef struct Meter_Set{
	BYTE		choice;		// choice = 0���޵��ܱ�
							// choice = 1�����е��ܱ�
	union
	{
		WORD	size;
		MSTYPE		mst;		// choice = 2��һ���û����ͣ�
		MSADDR		msa;		// choice = 3��һ���û���ַ��
		MSNO		msi;		// choice = 4��һ���û�������ţ�
		MSRT	msrt;			// choice = 5��һ���û��������䣩
		MSRA	msra;			// choice = 6��һ���û���ַ���䣩
		MSRI	msri;			// choice = 7��һ���û�������䣩
	};
}MS;

typedef struct tag_Selector4{
	DATETIME_S	time;
	MS			ms;
}Selector4;

typedef struct tag_Selector5{
	DATETIME_S	time;
	MS			ms;
}Selector5;

typedef struct tag_Selector6{
	DATETIME_S	start;
	DATETIME_S	end;
	TI			ti;
	MS			ms;
}Selector6;

typedef struct tag_Selector7{
	DATETIME_S	start;
	DATETIME_S	end;
	TI			ti;
	MS			ms;
}Selector7;

typedef struct tag_Selector8{
	DATETIME_S	start;
	DATETIME_S	end;
	TI			ti;
	MS			ms;
}Selector8;

typedef struct tag_Selector9{
	BYTE	nLast;
}Selector9;

typedef struct tag_Selector10{
	BYTE	nTimes;
	MS		ms;
}Selector10;

typedef struct tag_RSD{
	BYTE			choice;
	union{
		Selector0	sel0;
		Selector1	sel1;
		Selector2	sel2;
		Selector3	sel3;
		Selector4	sel4;
		Selector5	sel5;
		Selector6	sel6;
		Selector7	sel7;
		Selector8	sel8;
		Selector9	sel9;
		Selector10	sel10;
	};
}RSD;	//sizeof(RSD) = 1639

//�ն˵�ַ
typedef struct _TOad40010200{
	TSA		addr;
}TOad40010200;

//���ַ
typedef struct _TOad40050200{
	BYTE	grp_num;
	TSA		addr_grp[8];
}TOad40050200;

//�ն˵�ѹ���������ò���
typedef struct _TOad300F0600{
	BYTE	delay;
}TOad300F0600;

//���ܱ�ʱ�ӳ������ò���
typedef struct _TOad31050600{
	WORD	threshold;			//����ֵ
	BYTE	task_no;			//���������
}TOad31050600;

//�ն�ͣ/�ϵ��¼����ò��� 1
typedef struct _TOad31060601{
	BYTE byFlagCJ;				//ͣ��ɼ���־ D0:1��Ч,D0:0��Ч��D1:0 ָ�������� D1:1���ѡ��
	BYTE byCJGap;				//�ɼ����  ��λ:Сʱ
	BYTE byCJTmLmt;				//ͣ���¼�����ʱ����ֵ  ��λ: min
	BYTE byTSANum;				//��ַ����	
	TSA	 mp[6];					//��ַ�б�
}TOad31060601;

//�ն�ͣ/�ϵ��¼����ò��� 2
typedef struct _TOad31060602{
	WORD wTDTMMinGap;			//ͣ��ʱ����С��Ч���  ��λ������
    WORD wTDTMMaxGap;			//�����Ч���  ��λ������
    WORD wTMBELmt;				//��ֹʱ��ƫ����ֵ ��λ������
    WORD wTmMinLmt;				//ʱ��������ֵ   ��λ������
	WORD VOnLmt_1;
	WORD VOffLmt_1;				//��λ: V 1λС����
}TOad31060602;

typedef struct _TOad31060600{
	TOad31060601 oad1;
	TOad31060602 oad2;
}TOad31060600;
//���ܱ�ʾ���½����ò���
typedef struct _TOad310B0600{
	BYTE	task_no;			//���������
}TOad310B0600;

//�����������¼����ò���
typedef struct _TOad310C0600{
	DWORD	threshold;			//����ֵ
	BYTE	task_no;			//���������
}TOad310C0600;
//�����������¼����ò���
typedef TOad310C0600 TOad310D0600;
//������ͣ���¼����ò���
typedef struct _TOad310E0600{
	TI		ti;			//����ֵ
	BYTE	task_no;			//���������
}TOad310E0600;
//�ն˳���ʧ���¼����ò���
typedef struct _TOad310F0600{
	BYTE	byTryNum;			//�����ִ�
	BYTE	task_no;			//���������
}TOad310F0600;
//��ͨ�����������¼����ò���
typedef struct _TOad31100600{
	DWORD	dwGPRSFlow;			//ͨ����������
}TOad31100600;
//���ܱ����ݱ����ؼ�¼�¼����ò���
typedef TOad310B0600 TOad311C0600;

#define MAX_EVENT_PARA3  6
typedef struct _TOadEventClass7{
	BYTE	byOADNum;			//�ն�״̬����λ�¼�
	OAD		oadEvent[MAX_EVENT_PARA3];
}TOadEventClass7;

typedef TOadEventClass7 TOad31010300; //�ն˰汾����¼�

typedef TOadEventClass7 TOad31040300; //�ն�״̬����λ�¼�

typedef TOadEventClass7 TOad31090300; //��Ϣ��֤�����¼

typedef TOadEventClass7 TOad310B0300; //���ܱ�ʾ���½�

typedef TOad310B0300 TOad310C0300; //����������

typedef TOad310B0300 TOad310D0300; //���ܱ����

typedef TOad310B0300 TOad310E0300; //���ܱ�ͣ��

typedef TOadEventClass7 TOad310F0300; //����ʧ��

typedef TOadEventClass7 TOad31100300; //�ն�����վͨ������������

typedef TOadEventClass7 TOad31140300; //��ʱ�¼���¼

typedef TOadEventClass7 TOad31190300; //�ն˵�����·�쳣�¼�

typedef TOadEventClass7 TOad32000300; //������բ��¼

typedef TOadEventClass7 TOad32010300; //�����բ��¼

typedef TOadEventClass7 TOad32020300; //����������ü�¼

#define MAX_EVENT_PARA24 40

typedef struct _TOadEventClass24{
	BYTE	byOADNum;			//�ն�״̬����λ�¼�
	OAD		oadEvent[MAX_EVENT_PARA24];
}TOadEventClass24;

typedef TOadEventClass24 TOad30000200; //���ܱ�ʧѹ�¼�

typedef TOadEventClass24 TOad30010200; //���ܱ�Ƿѹ�¼�

typedef TOadEventClass24 TOad30020200; //���ܱ��ѹ�¼�

typedef TOadEventClass24 TOad30030200; //���ܱ�����¼�

typedef TOadEventClass24 TOad30040200; //���ܱ�ʧ���¼�

typedef TOadEventClass24 TOad30050200; //���ܱ�����¼�

typedef TOadEventClass24 TOad30060200; //���ܱ�����¼�

//Уʱģʽ
typedef struct _TOad40000300{	
	BYTE byMode;				//0-��վ��ʱ��1-�ն˾�ȷУʱ
}TOad40000300; 

//��׼Уʱ����
typedef struct _TOad40000400{
	BYTE byHeartNum;			//�������ʱ���ܸ���   
	BYTE byMaxDelNum;			//���ֵ�޳�����       
	BYTE byMinDelNum;			//��Сֵ�޳�����       
	BYTE byDelayTime;			//ͨѶ��ʱ��ֵ          
	BYTE byValidNum;			//������Ч����         
}TOad40000400; 
//����λ��
typedef struct _position{
	BYTE byFangWei;				//��λ
	BYTE byDu;					//��  
	BYTE byFen;					//�� 
	BYTE bySec;					//��
}position;

typedef struct _TOad40040200{
	position jing;				//����  
	position wei;				//γ��   
	DWORD high;					//�߶� cm
}TOad40040200;

//LCD����
typedef struct _TOad40070200{
	BYTE byLightTime;			//�ϵ�ȫ��ʱ��
	WORD wBlackTime;			//�������ʱ��
	WORD wShowTime;				//��ʾ�鿴�������ʱ��
	WORD wKeyTime;				//�޵簴����Ļפ�����ʱ��
	BYTE byEnergyNum;			//��ʾ����С��λ��
	BYTE byPowerNum;			//��ʾ����(�������)
	BYTE byLcdType;				//Һ���٢���������
}TOad40070200;

//��ѹ�ϸ��ʲ���
typedef struct _TOad40300200{
	WORD wVolLimitUp;			//��ѹ�������� 
	WORD wVolLimitDown;			//��ѹ�������� 
	WORD wVolUp;				//��ѹ�ϸ����� 
	WORD wVolDown;				//��ѹ�ϸ����� 
}TOad40300200;

typedef struct _TOad42020200{
	BOOL bValid;				//������־            bool��
	DWORD dwOAD;				//����ͨ�Ŷ˿ں�      OAD��
	WORD wTimeOut;				//�ܵȴ���ʱ��10ms��  long-unsigned��
	WORD wCharTimeOut;			//�ֽڳ�ʱ��10ms��    long-unsigned��		
	BYTE byRetryNum;			//�ط����� 	
	BYTE interval_cjMins;		//Ѳ�����ڣ�min��     unsigned��
	BYTE byJLPortNum;			//�����������˿���    unsigned��
	BYTE bysaNum;	
	TSA	sa[4];					//�����������ն˵�ַ  array TSA
}TOad42020200;

//�ն˹㲥Уʱ
typedef struct _TOad42040200{
	Time  tTime;
	BOOL  bUse;		  			//�Ƿ�����
}TOad42040200;		
typedef struct _TOad42040300{
	char maxErrTime;			//ʱ�������ֵ
	Time  tTime;	
	BOOL  bUse;					//�Ƿ�����
}TOad42040300;		

//�ɼ����������Ʋ���
/*
typedef struct _TOad42050300{
	BYTE byBroadNum;			//����һ�������㲥�ִ���      
    BYTE byDays;				//����һ��������Ե㲹������  
}TOad42050300;
*/
//���߹���ͨ�Žӿ�ͨ������
typedef struct _TOad45000200{
	BYTE byWorkMode;			//����ģʽ  enum{���ģʽ��0�����ͻ���ģʽ��1����������ģʽ��2��}��
	BYTE byVMode;				//���߷�ʽ  enum{�������ߣ�0�����������1��}��
	BYTE byTCPUDPMode;			//���ӷ�ʽ  enum{TCP��0����UDP��1��}��
	BYTE byLinkMode;			//����Ӧ�÷�ʽenum{����ģʽ��0����������ģʽ��1��}��
	BYTE byListenPortNum;
	WORD wListenPort[3];    	//�����˿��б�array long-unsigned��
	char szAPN[33];				//visible-string��
	char szUser[33];			//�û���    visible-string��
	char szPass[33];			//����      visible-string��
	BYTE byProxyServerIPLen;
	DWORD dwProxyServerIP;		//�����������ַ  octet-string��
	WORD wProxyProt; 	    	//����˿�  long-unsigned��
	BYTE byRetryTime;	    	//��ʱʱ�䣨�룩����ʱʱ�估bit1~bit0���ط�����
	WORD wSecs_Tick;	        //��������(��)  long-unsigned
}TOad45000200;
//���߹���ͨ�Žӿ���վͨ�Ų�����
typedef struct _TStaionConfig{
	BYTE byIPServerLen;
	DWORD IPServer;		    	//IP ��ַ  octet-string��
	WORD  wPort;	        	//�˿�  octet-string��
}TStaionConfig;

typedef struct _TOad45000300{
	BYTE byNum;
	TStaionConfig stationConfig[2];
}TOad45000300;
//���߹���ͨ�Žӿڶ���ͨ�Ų���
typedef struct _TOad45000400{
	char szSMS[17];	 			//��������  visible-string��
	BYTE byPhoneNum;
	char szPhone[3][17];		//��վ����   visible-string��
	BYTE byAlmPhoneNum;
	char szAlmPhone[3][17];		//����֪ͨĿ����      visible-string��
}TOad45000400;	
//���߹���ͨ�Žӿڰ汾��Ϣ
typedef struct _TOad45000500{
	char  productCode[4];		//���̴���       visible-string(SIZE (4))��
	char  softVer[4];			//����汾��     visible-string(SIZE (4))��
	char  softDate[6];			//����汾����   visible-string(SIZE (6))��
	char  hardVer[4];			//Ӳ���汾��     visible-string(SIZE (4))��
	char  hardDate[6];			//Ӳ���汾����   visible-string(SIZE (6))��
	char  productorInfo[8];		//������չ��Ϣ   visible-string(SIZE (8))
}TOad45000500;

//��̫��ͨ��ͨ������
typedef struct _TOad45100200{
	BYTE byWorkMode;			//����ģʽ  enum{���ģʽ��0�����ͻ���ģʽ��1����������ģʽ��2��}��
	BYTE byTCPUDPMode;			//���ӷ�ʽ  enum{TCP��0����UDP��1��}��
	BYTE byLinkMode;			//����Ӧ�÷�ʽenum{����ģʽ��0����������ģʽ��1��}��
	BYTE byListenPortNum;
	WORD wListenPort[3];    	//�����˿��б�array long-unsigned��
	BYTE byProxyServerIPLen;
	DWORD dwProxyServerIP;		//�����������ַ  octet-string��
	WORD wProxyProt;			//����˿�  long-unsigned��
	BYTE byRetryTime;	    	//��ʱʱ�䣨�룩����ʱʱ�估bit1~bit0���ط�����
	WORD wSecs_Tick;			//��������(��)  long-unsigned
}TOad45100200;

//��̫��ͨ����վͨ�Ų�����
typedef struct _TOad45100300{
	BYTE byNum;
	TStaionConfig stationConfig[2];
}TOad45100300;
//��̫��ͨ���ն�IP
typedef struct _TOad45100400{
	BYTE byIPMode;				//IP���÷�ʽenum{DHCP��0��,��̬��1����PPPoE��2��}��
	BYTE byNetLocalIPLen;
	DWORD dwNetLocalIP;			//IP��ַ       octet-string��
	BYTE byNetMaskLen;
	DWORD dwNetMask;			//��������     octet-string��
	BYTE byNetGateLen;
	DWORD dwNetGate;			//���ص�ַ     octet-string��
	char PPoEUser[33];			//PPPoE�û���  octet-string,
	char PPoEPass[33];			//PPPoE����    octet-string
}TOad45100400;

//��̫��ͨ��MAC��ַ
typedef struct _TOad45100500{
	char strMac[7];
}TOad45100500;

//��ȫģʽѡ��
typedef struct _TOadF1010200{
	BYTE bySafeUser;
}TOadF1010200;

typedef struct  _TSafeMode{
	 OI oi;						//�����ʶ   OI��
	 WORD wMode;				//��ȫģʽ   long-unsigned
}TSafeMode;
#define SAFE_MODE_NUM 30
//��ʽ��ȫģʽ����
typedef struct _TOadF1010300{
	BYTE byModeNum;
	TSafeMode safeMode[SAFE_MODE_NUM];
}TOadF1010300;

typedef struct _TOadF2030400{
	BYTE byFlag;				//�����־		
	BYTE byAttrib;				//���Ա�־
}TOadF2030400;

typedef struct _TOadF2010200_2{
	BYTE byPortNo;	
	char attrStr[16];			//����������
	BYTE attrStr_len;			//���Գ���
	ComDCB com_dcb;				//�˿ڲ���
	BYTE type_fun;				//��������
}TOadF2010200_2;

typedef struct _TOadF2010200{
	BYTE byPortNum;	
	TOadF2010200_2 rs485[3];
}TOadF2010200;

typedef struct _TOadF2050200{
	BYTE relay_type[2];			//�̵�������
}TOadF2050200;

typedef struct _TOadF2060400_2{
	Time begin;					//��ʼʱ��
	Time end;					//����ʱ��
}TOadF2060400_2;

typedef struct _TOadF2060400{
	BYTE alm_tm_num;			//�澯ʱ��
	TOadF2060400_2 alm_tm[24]; 
}TOadF2060400;

typedef struct _TChinaInfo{
	DWORD	dwTime;
	BYTE	byType;
	BYTE	byBNo;
	BYTE	bRead;
	BYTE	byValid;
	char	Info[201];
}TChinaInfo;

#define OBJ_PWD_LEN			(8)				// �����򳤶�
#define OBJ_MAX_ANNEX_LEN	(10)			// ��󸽼���Ϣ�򳤶�
#define OBJ_MAX_ANNEX_NUM	(3)				// ��󸽼���Ϣ������
typedef struct tag_Meter_Basic{
	TSA			tsa;						// ��Ƶ�ַ
	BYTE		baud;						// ������
	BYTE		protocol;					// ��Լ����
	OAD			port;						// �˿�
	char		password[OBJ_PWD_LEN];		// ͨ������
	BYTE		pwdLen;						// ���볤��
	BYTE		rateNum;					// ���ʸ���
	BYTE		userType;					// �û�����
	BYTE		conType;					// ���߷�ʽ
	WORD		stVolt;						// ���ѹ
	WORD		stI;						// �����
}Meter_Basic;

typedef struct tag_Meter_Extended{
	TSA		         add;			    // �ɼ�����ַ
	BYTE			 astLen;
	BYTE			 assetNo[32];		// �ʲ���
	WORD	         PT;				// PT
	WORD	         CT;				// CT
}Meter_Extended;

typedef struct tag_AnnexObject{
	OAD 		oad;
	BYTE		nLen;
	BYTE		value[OBJ_MAX_ANNEX_LEN];
}AnnexObject;

typedef struct tag_Meter_Annex{
	BYTE			nNum;
	AnnexObject		annexObj[OBJ_MAX_ANNEX_NUM];
}Meter_Annex;

typedef struct TOad60010200{	//sizeof(TOad60010200) = 149
	WORD			nIndex;		// ������� 2
	Meter_Basic		basic;		// ������Ϣ 32
	Meter_Extended	extend;		// ��չ��Ϣ 23
	Meter_Annex		annex;		// ������Ϣ 63 14��
	BYTE MeterNo[8];			// �����(�̶�8�ֽ�,ESAM����)
}TOad60010200;

typedef struct tag_OBJM_METER_129{
	WORD		nIndex;
	Meter_Basic	basic;
}OBJM_METER_129;

typedef struct tag_OBJM_METER_130{
	WORD			nIndex;
	Meter_Extended	ext;
	Meter_Annex		annex;
}OBJM_METER_130;

typedef struct tag_OBJM_METER_133{
	TSA		add;
	OAD		port;
}OBJM_METER_133;


typedef struct _TOad60020800{
	BOOL bDaySearchUse;			//�Ƿ�����ÿ�������ѱ�   
	BOOL bUpdateAD;				//�Զ����²ɼ�����       
	BOOL bsearchEvent;			//�Ƿ�����ѱ�����¼�   
	BYTE byResult;				//����ѱ���ѡ��       
}TOad60020800;

typedef struct _TOad60020900{
	Time tStartTime;	 		//��ʼʱ��
	WORD wSearchMin;	 		//�ѱ�ʱ��      
}TOad60020900;

typedef struct TaskSegTime{
	BYTE byBeginHour;			//ʱ
	BYTE byBeginMin;			//��
	BYTE byEndHour;				//ʱ
	BYTE byEndMin;				//��
}TaskSegTime;

//�������õ�Ԫ
//ע�⣺�����ṹʱ�طֿ���TTask698ParaCfg�ṹ���й��ڴ�
typedef struct _TOad60130200{
	WORD	validflag;			//0x5aa5Ϊ������Ч
	BYTE	byTaskID;			//����ID    unsigned
	TI		intelTI;			//ִ��Ƶ��  TI��
	BYTE	byTaskType;			//��������  enum ��ͨ�ɼ�����1,�¼��ɼ�����2
								//͸������ 3 �ϱ����� 4 �ű�����5 ʵʱ��ط���6
	BYTE	byShemeNo;			//������� unsigned��
	DWORD	dwBeginTime;		//��ʼʱ��    ObjDateTime_s ��
	DWORD	dwEndTime;	    	//����ʱ��    ObjDateTime_s ��
	TI		dealyTI;			//��ʱ        TI ��
	BYTE	byPrior;			//ִ�����ȼ�  enum{��Ҫ��1������Ҫ��2������Ҫ��3�������ܣ�4��}��
	BYTE	byStuts;			//״̬        enum{������1����ͣ�ã�2��}��
	WORD    wBeginId;			//����ʼǰ�ű�id   long-unsigned��
	WORD    wEndId;		    	//������ɺ�ű�id   long-unsigned��
	BYTE	byRunType;			//��������ʱ������
	BYTE	bySegNum;			//��������ʱ����
	TaskSegTime segTime[MAX_TASK_TIMEREG_NUM];	
}TOad60130200;	

typedef union _UCjInfo{
	BYTE	_N;
	TI		tii;
	struct {
		TI tii;
		WORD lastNums;
	}retryMeter;
}UCjInfo;
//��ͨ�ɼ�����
typedef struct _TOad60150200{	//sizeof(TOad60150200) = 1652
	WORD	valid_flag;			//0x5aA5
	BYTE	byShemeNo;			//������� unsigned��
	WORD	wSaveDeep;			//�洢���
	BYTE    byCJType;			//0 ��ǰ���� 1�ɼ���n������ 2������ʱ��ɼ� 3��ʱ�����ɼ�
	UCjInfo CJContent;			//�ɼ�����
	BYTE	byColNum;			//�ɼ�����Ŀ
	BYTE	csdBuf[512];		//��������
	MS		objMS;				//���ܱ���
	BYTE	bySaveFlag;			//�洢ʱ��ѡ�� δ����0������ʼʱ��1����Ե���0��0��2��
								//�������23��59��3���������0��0��4����Ե���1��0��0�� 5
	DWORD   dwValidTime;		//������Чʱ��
}TOad60150200;	
	
//�¼��ɼ�����	
typedef struct _TOad60170200{	//sizeof(TOad60170200) = 1634
	WORD	valid_flag;			//0x5aA5
	BYTE	byEventNo;			//�������unsigned��		
	BYTE    byCJType;			//�ɼ�����
	BOOL    bReport;			//�ϱ���ʶ 
	WORD	wSaveDeep;			//�洢��� 
	//.ROAD	objRoad[20];		//�ɼ����¼�����
	BYTE	byCJNum;
	BYTE	EventROADS[500];	//�¼� ROAD	
	MS		objMS;				//���ܱ���
}TOad60170200;	

//�ϱ�����
typedef struct _TOad601D0200{	//sizeof(TOad601D0200) = 512
	WORD	valid_flag;			//0x5aA5
	BYTE	byReportNo;	 	 	//�������
	BYTE	port_num;			//�ϱ�ͨ������
	OAD		oadReport[3];      	//�ϱ�ͨ��
	TI		reportTI;        	//�ϱ���Ӧ��ʱʱ��
	BYTE    byReportNum;	 	//����ϱ�����
	BYTE	byReportType;	 	//�ϱ�����
	BYTE	Report[500];		//�ϱ�����  0 ������������ 1 �ϱ���¼�Ͷ�������
}TOad601D0200;	
	
//͸������
typedef struct _TTransFrame{
	BYTE    byFrameNo;  //�������
	WORD    wFrameLen;  //���ĳ���
	WORD    wFmOffset;  //�����ڻ���������ʼƫ��
}TTransFrame;

typedef struct _TTransContent{
	BYTE    use_flag;    
	WORD    contentNo;   //�����������
    TSA     tsa;         //ͨ�ŵ�ַ
	WORD    bId;         //��ʼǰ�ű�id
	WORD    eId;         //��ɺ�ű�id
	BOOL    bRptAndWait; //���Ʊ�־-�ϱ�͸������������ȴ���������
	WORD    waitTime;    //���Ʊ�־-�ȴ��������ĳ�ʱʱ��
    BYTE    cmp_flag;     //���Ʊ�־-����ȶԱ�ʶ
	BYTE    cmp_Byte;    //���Ʊ�־-����ȶ�-�����ֽ�
	WORD    cmp_startpos;//���Ʊ�־-����ȶ�-��ȡ��ʼ
	WORD    cmp_lenth;   //���Ʊ�־-����ȶ�-��ȡ����
    BYTE    byFrameNum;  //���ĸ���
	TTransFrame tranFrames[6]; //��������
	BYTE    FmBuffer[200];     //�������ݻ�����
}TTransContent;

typedef struct _TOad60190200{
	WORD	valid_flag;			//0x5aA5
	BYTE	byTransNo;			//�������unsigned��		
	BYTE	byContentNum;       //�������ݼ�����
	TTransContent Contents[5];  //�������ݼ������֧��5����
	WORD    wSaveDeep;          //�洢���
}TOad60190200;

typedef struct _TSysCtrlPara{
	//23��30-24��00	23��00-23��30	22��30-23��00	22��00-22��30	��12�ֽ�
	//ÿ��Сʱ����λ�����ʾ4�ֿ���״̬��ȡֵ0��3���α�ʾ�����ơ�����1������2��������
	//����״̬��ʶ��Ӧ�ù�������ʱ�䵥Ԫ������ͬ����״̬��ʶ��ʾͬһ����ʱ�Σ�����ʱ�䵥Ԫ���в�ͬ����״̬��ʶ����ʾ���ڵ�����ʱ�Σ�����1�����2�������־߱�2����ͬ�Ķ�ֵ������ʱ�Σ�������״̬��ʶ�����仯ʱ����ʾǰһ����ʱ�ν�������һ����ʱ�ο�ʼ�����ڲ������Ŀ���ʱ�ο����ÿ���1�����2��ʾ��
	BYTE	PKTM[12];			//����ʱ��
	long	nPower_Safe;		//������ֵ
	BYTE	Mins_Lun[2];		//���2��
}TSysCtrlPara;
//�ܼ������
typedef struct _TOad81090200{
	BYTE	valid_flag;			//������Ч��ʶ
	char	pkcoft;				//����ϵ��
	BYTE	tm_flag[3];			//ʱ�κ�
	long	fixval[3][8];		//ʱ�ζ�ֵ
}TOad81090200;

//����
typedef struct _TOad810A0200{
	TTime   stXDTime;
	WORD	wMins_XD;			//�޵�ʱ��
	BYTE	byFlag_FRXD;		//�����޵��־
	long	nFixVal;			//���ݶ�ֵ
}TOad810A0200;

typedef struct _TOad810B0200{
	TTime	tBTSTime;
	TTime	tBTETime;	
	long	nFixVal;			//��ͣ��ֵ
}TOad810B0200;


typedef struct _TGRPCtrlPara{
	BYTE	byTmPKSchNo;		//ʱ�ι��ط�����
	BYTE	byFlag_TMPK;		//ʱ�ι���Ͷ���ʶ
	BYTE	byFlag_PK;			//����Ͷ���־
	BYTE	byFlag_EK;			//���Ͷ���־
	
	long	nPower_PDPKFix;		//�¸��ض�ֵ
	BYTE	byMin_PDPKSlide;	//�¸��ػ���ʱ��
	BYTE	byCoft_PDPK;		//�¸��ظ���ϵ��
	BYTE	byMin_PDDelay;		//�¸��غ���ʱ
	BYTE	byCoftDir_PDPK;		//�¸���ϵ������
	WORD	wMins_PDXD;			//��ʱ�޵�ʱ��
	BYTE	Min_PDAlm[4];		//�¸����ִθ澯ʱ��,����

	DWORD	dwPDOnTime;			//�¸���Ͷ�����ʼʱ��
	long	nPower_PDFrz;		//�¸��ؿغ󶳽�ֵ
	BYTE	byEKType;			//0-�����أ�1-��ѿ�
	BYTE byHCTime;				//���ʿ��ƵĹ��ʼ��㻬��ʱ�䣨��Χ��1~60��BIN min 1
	BYTE byPKLCFlag;			//�����ִα�־λ BS8 1
	BYTE byDKLCFlag;			//����ִα�־λ BS8 1
	BYTE unit_valid;			//��Ԫ��Ч��ʶ	TOad81090200������
}PACK TGRPCtrlPara;

typedef struct _GB698_GRP_CFG{
	TGRPCtrlPara  tGRPPara;
	TOad81090200  para8109;		//ʱ�ι��ز���
	TOad810A0200  para810A;		//���ݿز���	
	TOad810B0200  para810B;		//��ͣ�ز���
//	TOad810C0200  para810C;		//����ز���
//	TOad810D0200  para810D;		//�µ����ز���
}PACK GB698_GRP_CFG;

typedef struct _TEsamSafeMode{
	OI oii;
	WORD wSafeMode;
}PACK TEsamSafeMode;

#define ESAM_SAFE_NUM	20//���ܳ���127��������Ķ�д��������
typedef struct _TsysCfgOther{	//�ܹ�300���ֽ� 
	WORD wFlag_Ctrl;
	DWORD dwMins_BD;			//�˱�־����ֽ�����II�ͼ�����ʵʱת�ն������ 0x5A-ʵʱת�ն��� ����-�����
	DWORD dwSecs_BDStart;
	BYTE  byBDTmNum;			//����ʱ�θ���
	WORD  BDTm[24];				//���ֽڿ�ʼ�����ֽڽ���

	DWORD dwLcdPass;
	BYTE  link_authentication;	//������֤ģʽ(��������   (0)��  ��ͨ����   (1)��  �ԳƼ���   (2)��  ����ǩ��   (3))

	DWORD dwDisplayCtrl;		//Ѳ������	
	BYTE  byDisplayInterval;	//Ѳ�Լ��
	BYTE byNetType;				//0x55 ���߿��������-��̫��	

	BYTE  CFHourFlag[3];		//�߷�Сʱ��ʶ
	char  Info[139];			//�߷���Ϣ
	BYTE	szSIMCard[17];
	BYTE	szPassWord[32];		//��������
	BYTE	byUpPortNum;		//�ϱ�ͨ������
	DWORD	UpPortOAD[3];		//�ϱ�ͨ��	
	WORD max_no_data_mins;		//��������վ�����ͨ��ʱ�������ӣ���0��ʾ���Զ����硣
	WORD max_power_delay_mins;	//�ϵ��Զ�����ʱ�������ӣ���0��ʾ�ϵ粻�Զ����硣
	BYTE safeMode_ctrl;			//BIT0-�Ƿ����ð�ȫģʽ,BIT1-�Ƿ�����SAL��ȫӦ��������·�����
	BYTE valid_safemode[(ESAM_SAFE_NUM+7)/8];	
	TEsamSafeMode safeMode[ESAM_SAFE_NUM];
	BYTE EventFlag[88];			//BIT0~BIT1-�ϱ���ʶ��BIT2-��Ч��ʶ
	TSysCtrlPara sysCtrl;
	DWORD threshold_Relay;
	WORD mins_OverI;
	BYTE byGNetMode; //G��ѡ��(BIT1~0, 0:����ѡ�� 1:2G 2:3G 3:4G )BIT2: 1 ������ƴ�,0 �ر�, BIT3:��Ȩ����
}PACK TsysCfgOther;

typedef struct _TPkTM{
	BYTE	byTmNum;			//ʱ�θ���
	WORD	Mins_TMEnd[19];		//ʱ��(�����0��Ľ���ʱ��)
	BYTE	Type_TM[19];		//0-������ 1-���� 2-����2
	TTime	TimeStart;			//ʱ��ִ�п�ʼʱ��
	TTime	TimeEnd;			//ʱ��ִ�н���ʱ��
	BYTE	byFreq;				//ִ��Ƶ��
	DWORD	dwFlag;				//��־omm
}TPkTM;

typedef struct _TPulseCfg{
	WORD wConst;
	BYTE byPulseNo;				//��������˿ں�
	BYTE attrib;
}TPulseCfg;

typedef struct _TOad24010200{	//�������������
	//.BYTE Addr[17];		
	TSA sa;
	BYTE byPulseNum;	
	WORD wPT;
	WORD wCT;
	TPulseCfg Cfg[4];
}TOad24010200;

typedef struct _TFrz698Cfg{
	WORD	interval;			//��������
	DWORD	oadd;				//����oadd
	WORD	wSaveDeep;			//�������
}TFrz698Cfg;

#define MAX_FRZ_CFG_NUM	50 //60 		
typedef struct _TOad50020300{	//���Ӷ�������	
	BYTE byNum;
	TFrz698Cfg FrzCfg[MAX_FRZ_CFG_NUM];	
}TOad50020300;

typedef TOad50020300 TOad50030300;
typedef TOad50020300 TOad50040300;
typedef TOad50030300 TOad50060300;

typedef struct _TOad41030200{		//�ʲ��������	
	BYTE byNum;
	BYTE asetNo[32];	
}TOad41030200;

#pragma pack()

#endif //#if(MD_OBJECT==YES)
#endif //_NXCJ_DATA_TYPE_DEF_H_

