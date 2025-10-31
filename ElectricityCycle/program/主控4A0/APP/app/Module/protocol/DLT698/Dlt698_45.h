//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	姜文浩 魏灵坤
//创建日期	2016.8.4
//描述		DL/T 698.45面向对象协议头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __DLT698_45_H
#define __DLT698_45_H

#include "Dlt645_2007.h"

#if(SEL_DLT698_2016_FUNC == YES  )
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
// 一帧报文传输的数据块越大，一个扇区空余的空间越多，越需要开辟更大的空间，目前先按照一帧400字节数据来开辟空间（200字节和400字节效果一样）
#define MAX_SIZE_FOR_IAP						(512 * 1024)									// 用来升级的空间  
#define LEN_OF_ONE_FRAME						100												// 一帧报文的长度
#define FRAMENUM_OF_ONE_SECTOR					(SECTOR_SIZE / LEN_OF_ONE_FRAME)				//一个扇区能够存的报文数20
#define FRAMESIZE_OF_ONE_SECTOR					(FRAMENUM_OF_ONE_SECTOR * LEN_OF_ONE_FRAME) 	//一个扇区能够存字节数
#define SECTOR_NUM_FOR_IAP						((MAX_SIZE_FOR_IAP / FRAMESIZE_OF_ONE_SECTOR)+1)// 存MAX_SIZE_FOR_IAP的升级数据，需要多少个扇区132
#define IAP_CODE_ADDR							((2048 - SECTOR_NUM_FOR_IAP) * SECTOR_SIZE)		// 扇区号 0~2047
#define IAP_INFO_ADDR							((2048 - SECTOR_NUM_FOR_IAP- 1) * SECTOR_SIZE )	// 扇区号 0~2047  多用一个扇区用来存放升级信息
#define	IAP_JUDGE_ADDR							((2048 - SECTOR_NUM_FOR_IAP- 2) * SECTOR_SIZE )	//扇区号 0~2047	存放第一帧判断信息
#define IAP_FLAG								0xA55AA55A
#define IAP_START								0x5AA55AA5	// 接收完启动传输命令
#define IAP_READY								0x5AA5A55A	// 接收完执行升级命令
#define IAP_ALL									0x5AA5		// 全部升级标志

#define DEFAULT_MAX_PRO_BUF_485     512
#define DEFAULT_MAX_PRO_BUF_CR      512
#define DEFAULT_MAX_PRO_BUF_IR      200
#define MIN_CONSULT_BUF_LEN         45//值不能小于45


#define APP_BUF_SIZE			(MAX_PRO_BUF - DLT_LINK_DATA_HEAD -DLT_LINK_DATA_TAIL -6 -2)//-6 通讯地址 -2 时间标签和跟随上报
#define APP_APDU_BUF_SIZE		(MAX_APDU_SIZE - DLT_LINK_DATA_HEAD -DLT_LINK_DATA_TAIL -6 -2)
/**************************************链路层格式长度定义************************************/
#define	DLT_START_BYTE				1				//起始字符长度
#define	DLT_LENTH					2				//长度域长度
#define	DLT_CONTROL_BYTE		    1				//控制域长度
#define	DLT_ADDRESS_ELSE_LEN		2				//地址域其他长度
#define	DLT_CSLEN					2				//校验长度
#define	DLT_END_BYTE				1				//结束字节长度
#define	DLT_LINK_DATA_HEAD			(DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+DLT_CSLEN)//不包括实际地址长度
#define	DLT_LINK_DATA_TAIL			( DLT_CSLEN + DLT_END_BYTE )
#define	DLT_FRAME_BYTE				2				//地址域其他长度

#define COMM_TYPE_MASTER		(0xA0)				//主站通信
#define COMM_TYPE_TERMINAL		(0x00)				//终端通信
#define COMM_TYPE_MASK			(0xF0)				//通信设备掩码
#define CURR_COMM_TYPE(Ch) (LinkData[Ch].ClientAddress&COMM_TYPE_MASK)	//当前的通信类型 终端或者主站

/**************************************应用层定义********************************************/
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

//bit0 应用层有数据返回 bit1 安全传输 bit2链路分帧 bit3 应用分帧

#define APP_HAVE_DATA				0X01  //应用层有数据返回
#define APP_NO_DATA					0X02  //应用层没有数据返回
#define APP_LINK_FRAME				0X04  //链路分帧
#define APP_ERR_DATA				0X08  //异常应答

/****************************************数据类型定义**********************************/
//		类型		  				数值		名字                 字节数
#define NULL_698					0
#define Array_698					1		//数组			     指定对象个数
#define Structure_698				2		//结构体			 指定成员
#define Boolean_698			        3		//布尔型			 1
#define Bit_string_698				4		//字符型			 1
#define Double_long_698 			5		//有符号长整型 Integer32_698	 4  -原dlms中的
#define Double_long_unsigned_698 	6		//有符号长整型 Integer32_698	 4  -698.45中的
#define Octet_string_698			9		//16进制字符串	     指定长度
#define Visible_string_698			10		//可见字符		     指定长度
#define UTF8_string_698				12		//16进制字符串	     指定长度
#define BCD_698   					13		//BCD码				 1
#define Integer_698	    			15		//有符号短整型		 1 
#define Long_698					16		//有符号整型Integer16_		 2  --698.45中的
#define Unsigned_698				17		//无符号短整型		 1
#define Long_unsigned_698			18		//无符号整型Unsigned16_		 2  --698.45中的
#define Long64_698					20		//有符号长整型Integer64_		 8  --698.45中的
#define Long64_unsigned_698			21		//无符号长整型Unsigned64_		 8  --698.45中的
#define Enum_698					22		//枚举型			 1
#define Float32_698					23		//浮点数			 4
#define Float64_698					24		//双精度浮点数		 8
#define Date_time_698				25		//日期时间型		 12
#define Date_698					26		//日期				 5
#define Time_698					27		//时间				 4
#define DateTime_S_698				28		//日期时间无周		 6	
#define OI_698						80		//对象属性描述符     5  Object Attribute Descriptor
#define OAD_698						81		//对象方法描述		 4  Object Method Descriptor
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

#define	U_a						 	1	//	a		时间			年	
#define	U_mo					 	2   //	mo		时间			月	
#define	U_wk						3	//	wk		时间			周			7*24*60*60s
#define	UNIT_dd						4	//	d		时间			日			24*60*60s
#define	UNIT_hh						5	//	h		时间			小时		60*60s
#define	UNIT_Min					6	//	min		时间			分			60s
#define	UNIT_S						7	//	s		时间(t)			秒			s
#define	UNIT_0						8	//	°		(相)角			度			rad*180/π
#define	UNIT_0C						9	//	℃		温度（T）		摄氏度		K-273.15
#define	UNIT_YUAN					10	//	货币	(当地)货币		
#define	UNIT_M						11	//	m		长度(l)			米			m
#define	UNIT_MPS					12	//	m/s		速度(v)			米/秒		m/s
#define	UNIT_M3						13	//	m3		体(容)积(V)		立方米		m3
#define	UNIT_M3_T					14	//	m3		修正的体积		立方米		m3
#define	UNIT_M3Ph					15	//	m3/h	流量			立方米/h	m3/(60*60s)
#define	UNIT_M3Ph_T					16	//	m3/h	修正的流量		立方米/h	m3/(60*60s)
#define	UNIT_M3Pd					17	//	m3/d	流量			立方米/d	m3/(24*60*60s)
#define	UNIT_M3Pd_T					18	//	m3/d	修正的流量		立方米/d	m3/(24*60*60s)
#define	UNIT_L						19	//	l		容积			升			10^-3m3
#define	UNIT_kg						20	//	kg		质量(m)			千克		kg
#define	UNIT_N						21	//	N		力(F)			牛顿		N
#define	UNIT_Nm						22	//	Nm		能量			牛顿米		J=Nm=Ws
#define	UNIT_P						23	//	P		压力(p)			帕斯卡		N/m2
#define	UNIT_bar					24	//	bar		压力(p)			巴			10^-5N/m2
#define	UNIT_J						25	//	J		能量			焦耳		J=Nm=Ws
#define	UNIT_JPh					26	//	J/h		热功			焦每小时	J/(60*60s)
#define	UNIT_W						27	//	W		有功功率(P)		瓦			W=J/s
#define	UNIT_KW						28	//	kW		有功功率(P)		千瓦		kW=J/(s*1000)
#define	UNIT_VA						29	//	VA		视在功率(S)		伏安	
#define	UNIT_KVA					30	//	kVA		视在功率(S)		千伏安	
#define	UNIT_VAR					31	//	var		无功功率(Q)		乏	
#define	UNIT_KVAR					32	//	kvar	无功功率(Q)		千乏	
#define	UNIT_KWH					33	//	kWh		有功能量rw,		千瓦-时		kW*(60*60s)
#define	UNIT_KVAH					34	//	kVAh	视在能量rS,		千伏-安-小时kVA*(60*60s)
#define	UNIT_KVARH					35	//	kvarh	无功能量rB,		千乏-时		kvar*(60*60s)
#define	UNIT_A						36	//	A		电流（I）		安培		A
#define	UNIT_C						37	//	C		电量（Q）		库仑		C=As
#define	UNIT_V						38	//	V		电压（U）		伏特		V
#define	UNIT_VPm					39	//	V/m		电场强度（E）	伏每米		V/m
#define	UNIT_F						40	//	F		电容（C）		法拉		C/V=As/V
#define	UNIT_ohm					41	//	Ω		电阻（R）		欧姆		Ω=V/A
#define	UNIT_ohmPm					42	//	Ωm2/m	电阻系数（ρ）				Ωm
#define	UNIT_Wb						43	//	Wb		磁通量（Φ）		韦伯		Wb=Vs
#define	UNIT_T						44	//	T		磁通密度（T）	泰斯拉		Wb/m2
#define	UNIT_APm					45	//	A/m		磁场强度（H）	安培每米	A/m
#define	UNIT_H						46	//	H		电感（L）		亨利		H=Wb/A
#define	UNIT_Hz						47	//	Hz		频率			赫兹		1/s
#define	UNIT_1PWh					48	//	1/(Wh)	有功能量表常数或脉冲		imp/kWh
#define	UNIT_1Pvarh					49	//	1/(varh)无功能量表常数或脉冲		imp/kvarh
#define	UNIT_1PVAh					50	//	1/(VAh)	视在能量表常数或脉冲		imp/kVAh	
#define	UNIT_PCT					51	//	%		百分比			百分之	
#define	UNIT_BYTE					52	//	byte	字节			字节	
#define	UNIT_DBM					53	//	dBm		分贝毫瓦		
#define	UNIT_PRICE					54	//	元/kWh	电价		
#define	UNIT_AH						55	//	Ah		安时			安时	
#define	UNIT_MS						56	//	Ah		安时			安时	
//							56…253	//	保留
#define	UNIT_UNSUPO					250
#define	UNIT_OTHER					254	//	其他单位
#define	UNIT_NO						255	//	无单位、缺单位、计数



/****************************************数据类型字节数长度定义*********************************/
#define OAD_698_LENTH		 	4
//显示安全械参数最多保存多少项,最好是30的整数倍

//then_get最大支持list数量
#if( MAX_FREEZE_ATTRIBUTE > MAX_EVENT_OAD_NUM )
#define MAX_ATTRIBUTE MAX_FREEZE_ATTRIBUTE
#else
#define MAX_ATTRIBUTE MAX_EVENT_OAD_NUM
#endif

//-----------------------------------------------

typedef enum
{
	eNO_FRAME = 0,          	//无分帧
	eLINKDATA_FAIL,         	//失败 
	eDOWN_FRAMEING,     		//下行分帧进行中
	eDOWN_FRAME_END,    		//下行分帧结束
	eUP_FRAMEING,         		//上行分帧进行中
	eUP_FRAME_END,        		//上行分帧结束
	eAPP_OK                   	//应用层数据OK   
}eFrameResultChoice;//应用层buf格式结果

typedef enum
{
   	eUNION_TYPE,         		//共用体类型
   	ePROTOCOL_TYPE,    			//协议类型
   	eESAM_TYPE,        			//ESAM类型
   	eUNION_OFFSET_TYPE,       	//共用体长度返回
	eUNION_BITSTRING_TYPE,
}eDataLenTypeChoice;//数据长度类型 输入数据类型

typedef enum
{
	eADD_OK = 0,				//添加成功
	eAPP_LINKDATA_OK,         	//链路层分帧已填满
	eAPP_NO_RESPONSE,			//无应答
	eAPP_ERR_RESPONSE,          //错误应答
	eAPP_RETURN_DATA,			//强制进行数据返回
}eAPPBufResultChoice;//添加数据返回结果

typedef enum
{
	eNO_SECURITY =0,		//不是安全传输
	eSECURITY_FAIL,			//安全解密失败
	eRNMAC,					//随机数+MAC
	eSECURITY_RN,
	eEXPRESS_MAC,		    //明文+mac 
	eSECRET_TEXT,			//密文
	eSECRET_MAC             //密文+mac
} eSecurityMode;//安全传输类型

typedef enum
{	
	eNO_TimeTag =0,              //无时间标签
	eTime_True = 0x22,           //有时间标签 且时间标签有效
	eTime_Invalid = 0x55         //时间标签无效
}eTimeTagFlag;//时间标签标志

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
}eClass;//OTHER 数据中 类的枚举

typedef enum
{	
	eREAD_MODE,
	eSET_MODE,
	eACTION_MODE,
	eOTHER_MODE  //非正常模式用于delay非法判断
}eRequestMode;//请求类型

typedef enum
{	
	eSINGLE_ADDRESS_TYPE = 0,	//单地址
	eWILDCARD_ADDRESS_TYPE = 1,	//通配地址
	eGROUP_ADDRESS_TYPE = 2,	//组地址
	eBROADCAST_ADDRESS_MODE = 3	//广播地址
}eAddressType;//请求类型
/*****************************链路层结构体***********************/
typedef struct TDLT698LinkFrame_t   	//链路层结构体
{
	TTwoByteUnion FrameType;       		//分帧格式域(2个字节)
	BYTE *pFrame;						//分帧APDU片段起始地址
	TTwoByteUnion FrameLen;        		//分帧APDU片段长度
}TDLT698LinkFrame;


typedef struct TDLT698LinkData_t		//链路层结构体
{
	TTwoByteUnion  DataLen;				//数据长度
	BYTE ControlByte;					//控制字节
	BYTE AddressLen;					//地址长度
	BYTE ServerAddr[6];					//服务器地址
	BYTE ClientAddress;					//客户机地址
	eAddressType AddressType;			//地址类型
	eFrameResultChoice eLinkDataFlag;
	BYTE* pAPP;							//应用层起始指针
	TTwoByteUnion pAPPLen;				//应用层长度
	TDLT698LinkFrame  DownLinkFrame;	//下行使用
	TDLT698LinkFrame  UpLinkFrame;		//上行使用	
}TDLT698LinkData;

/*****************************应用层结构体***********************/
typedef struct TDLT698APPData_t   		//应用层结构体
{
	BYTE APPFlag;						//应用层标志 bit0 应用层有数据返回 bit1 安全传输 bit2链路分帧
	eTimeTagFlag TimeTagFlag;			//时间标签标志
	BYTE RequestType;					//保存Request模式 
	eSecurityMode eSafeMode;			//安全模式
	BYTE* pAPPBuf;						//应用层buf指针
	TTwoByteUnion APPBufLen;			//应用层 能组帧完整数据的buf长度
	TTwoByteUnion APPCurrentBufLen;		//应用层 实际已组帧数据buf长度
	TTwoByteUnion APPMaxBufLen;			//应用层最大buf长度
	TTwoByteUnion BufDealLen;			//下行数据已处理的数据长度(针对list)(只针对下发数据，而不是说buf添了多少数据)	
}TDLT698APPData;

/*****************************后续帧结构体*********************************/
typedef struct TTagPara_t   	//加TAG使用的参数
{
	BYTE Array_Struct_Num;//array或struct的个数
	TTwoByteUnion Lenth;
}TTagPara;

typedef struct TTagBuf_t   	//加TAG时Buf使用的参数
{
	WORD InBufLenth;
	WORD OutBufLenth;
}TTagBuf;

typedef struct TScaler_Unit_t//换算与单位结构体
{
	signed char 	Scaler;		//换算
	BYTE	 		Unit;		//单位
}TScaler_Unit;	

typedef struct TCommonObj_t
{
	WORD				OI;				//698对象标识：OI对象标识
	const TScaler_Unit 	*Scaler_Unit3;	//电能换算单位
	const BYTE*         Type2;			//电能类数据标识
}TCommonObj;

typedef struct TAPDUFlag_t
{
    BYTE Ch;    //当前通道
    BYTE Flag;  //true 已占用 false 未占用
}TAPDUFlag;

typedef struct TAPDUBufFlag_t
{
   TAPDUFlag Request;
}TAPDUBufFlag;

typedef struct TConsultData_t   	//加TAG时Buf使用的参数
{
	BYTE channel;
	WORD Len;
}TConsultData;

#pragma pack(1)
typedef struct TIap_t
{
	BYTE bDestFile[10];
	BYTE bSourceFile[10];
	DWORD dwFileSize;  // 程序文件大小（Byte）
	BYTE bFileAttr;	   // 文件属性
	BYTE bFileVer[10]; // 文件版本
	BYTE bFileType;	   // 文件类别
	WORD wFrameSize;   // 传输块大小，默认200字节
	WORD wAllFrameNo;  // 程序文件分割后的总帧数
	WORD wFrameOfOneSector;  // 一个扇区可以存放的数据帧数
	WORD wCrc16;	   // 整个程序文件的CRC16校验码
	WORD wAllUpdate;   // 1:全部升级  0：部分升级
	DWORD dwIapFlag;   // 升级标志位  0xA55AA55A 有效，其他无效
	DWORD dwCrc32;	   // 结构体校验
} T_Iap;
typedef struct  {
	void* pgetcfun;
	DWORD oadd;
	WORD offset;
	BOOL FZtype;//仅组帧时使用
	BYTE GetrequsetNextType;//分帧类型
	WORD SlicingIndex;
	BYTE IslastFrame;
	//BYTE pPara[MAX_APDU_SIZE+EXTRA_BUF];
	BYTE pPara[MAX_PRO_BUF+EXTRA_BUF];

} TGetRequestNext;
typedef struct TIap_Info
{
	DWORD dwTime;
	DWORD dwFlag;
	WORD wFacFlag;	  // 0:非法  1：合法
	WORD wCurFrameNo; // 当前升级帧序号
    WORD wCRC16;
} TIapInfo;
#pragma pack()
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern T_Iap tIap;
extern TIapInfo tIapInfo;
extern BYTE IapStatus[(MAX_SIZE_FOR_IAP / LEN_OF_ONE_FRAME) / 8 + 1]; // F001 的属性4 升级状态；后续改为按照实际一帧传输的长度开辟空间!!!!!!
extern __no_init BYTE g_PassWordErrCounter;//密码错误次数
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

extern const BYTE T_ScalerUnit[];	//ScalerUnit数据标识
extern const BYTE T_Unsigned[];		//long_unsigned数据标识
extern const BYTE T_UNLong[];		//long_unsigned数据标识
extern const BYTE T_Long[];			//Long数据标识
extern const BYTE T_DoubleLong[];	//DoubleLong数据标识
extern const BYTE T_UNDoubleLong[];	//DoubleLong数据标识
extern const BYTE T_BitString16[];	//long_unsigned数据标识
extern const BYTE T_BitString32[];
extern const BYTE T_BitString8[];	//long_unsigned数据标识
extern const BYTE T_DateTimS[];		//DateTime_S数据标识
extern const BYTE T_Enum[];			//Enum数据标识
extern const BYTE T_VisString32[];	//VisString32数据标识
extern const BYTE T_VisString16[];	//VisString16数据标识
extern const BYTE T_VisString6[];	//VisString6数据标识
extern const BYTE T_VisString4[];	//VisString4数据标识
extern const BYTE T_OctetString5[];	//OctetString5数据标识
extern const BYTE T_OctetString7[];
extern const BYTE T_OctetString8[];	//OctetString8数据标识
extern const BYTE T_OctetString16[];//OctetString16数据标识
extern const BYTE T_OctetStringFF[];	//OctetStringFF数据标识
extern const BYTE T_TI[];			//TI数据标识
extern const BYTE T_Bool[];			//Bool数据标识
extern const BYTE T_Special[];		//特殊处理
extern TAPDUBufFlag APDUBufFlag;
extern BYTE ClientAddress;//客户机地址
extern BYTE ConnectMode;  //连接模式
extern BYTE FrameOverTime;//分帧超时时间
extern __no_init BYTE RN_MAC[50]; //暂存RN_MAC  -- 掉电不清意义不大，但是可能会解决表复位后收到确认报文也会验证过
extern __no_init BYTE RN_MAC_BAK[50];   	//RN_MAC备份

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  链路层请求处理函数
//         
//参数:      BYTE Ch[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void Proc698LinkDataRequest( BYTE Ch );

//--------------------------------------------------
//功能描述:  链路层数据响应函数
//         
//参数:      BYTE Ch[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
BYTE Proc698LinkDataResponse( BYTE Ch);

//-----------------------------------------------
//功能描述  : 698.45主函数
//参数  : 	TSerial *p[in] 
//返回值: 
//备注内容  :  无
//-----------------------------------------------
void ProcMessageDlt698(TSerial *p);

BOOL Pre_Dlt698(TSerial *p, BYTE *pAck);
BOOL IsInSysMeters(BYTE *pMeterAddr);
BOOL IsChargingPileFrame(BYTE *pBuf);
BYTE api_GetChargingPileCanID(BYTE* bAddr);
//-----------------------------------------------
//功能描述  :   初始化698参数
//参数  : 	无
//返回值: 
//备注内容  :  无
//-----------------------------------------------
void FactoryInitDLT698(void);

//--------------------------------------------------
//功能描述:时效性任务判断 包括（断开应用链接、红外时效、延时读取）
//         
//参数  :   无
//
//返回值:    无   
//         
//备注内容:  无
//--------------------------------------------------
void TimeLinessEventJudge( void );


//--------------------------------------------------
//功能描述:  主循环清零函数
//         
//参数  : 无
//
//返回值:    
//         
//备注内容:  每500毫秒调用一次
//--------------------------------------------------
void ProcClrTask(void);

//--------------------------------------------------
//功能描述:  对输入数据添加Tag
//         
//参数:      *Tag[in]    Tag[0]--数据类型  Tag[1]--数据长度 Tag[n]--……   
//           *InBuf[in]	 需要添加tag的数据        
//           *OutBuf[in] 添加tag后输出的数据
//         
//返回值:    添加tag后输出的数据长度
//         
//备注内容:  对InBuf中的数据按照Tag标签的要求添加标签，输出到OutBuf 
//--------------------------------------------------
WORD GetRequestAddTag( const BYTE *Tag, BYTE *InBuf, BYTE *OutBuf );

//-----------------------------------------------
//函数功能: 698数据长度处理
//
//参数:		BYTE *buf  //输出的数据
//			BYTE len   //处理的数据长度	
//			BYTE mode  // 0 代表requense 数据长度 1代表request数据长度 0是len.w转化符合规约的格式 1是将规约长度转化为len.w		
//返回值:	无
//		   
//备注:698数据长度处理，主要用来处理数据长度超过一个字节的情况 遇到优化问题,此函数还可以进行扩展
//-----------------------------------------------
BYTE Deal_698DataLenth( BYTE *buf, BYTE *Datalen, eDataLenTypeChoice  eTypeChoice );


//--------------------------------------------------
//功能描述:  读取结算日
//参数	:
//			pBuf[out]
//返回值:	TRUE	FALSE
//注:返回时间数据为HEX
//--------------------------------------------------
WORD ReadSettlementDate(BYTE *pBuf);

//--------------------------------------------------
//功能描述:  698获取单个OAD数据
//         
//参数:	
//		GetDataType[in]	GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//		Ch[in]:			规约通道
//		DataType[in]:	eData/eTagData/eAddTag
//		Dot[in]:  		获取数据的小数点位数，oxff--表示规约默认的小数位数（规约调用）
//		pOAD[in]：		获取数据的OAD
//		InBuf[in]：		给定要加Tag的数据(注:如果是事件类中的 事件上报状态(0x3300)、编程对象列表(0x3302)、事件清零列表(0x330c)时，第一个字节InBuf[0]需要存放Array的个数Number)
//		OutBufLen[in]：	给定OutBufLen的长度
//		OutBuf[out]:  	获取数据存放OutBuf中
//
//返回值: 返回数据长度	0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetProOadData( eGetDataType GetDataType, BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述:  根据类型获取数据长度
//         
//参数:      DataType[in]：eData/eTagData        
//           Tag[in]：
//         					Tag[0]--数据类型
//         					Tag[1]--成员个数
//         					Tag[n]--……
//返回值:    返回数据长度
//         
//备注内容: 支持array 和 array+struct+Normal方式
//--------------------------------------------------
WORD GetTypeLen( BYTE DataType, const BYTE *Tag );

//--------------------------------------------------
//功能描述:  读取电能
//         
//参数:     
//			 GetDataType[in] GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//			 DataType[in]：	eData/eTagData/eAddTag        
//           Dot[in] :		小数点        
//           *pOAD[in]：	    OAD         
//           *InBuf[in]：	需要添加标签的数据         
//           OutBufLen[in]： 留给OutBuf的数据长度         
//           *OutBuf[in]：	返回的电能数据
//         
//返回值:    返回电能长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  费率个数不能超过12
//--------------------------------------------------
WORD GetRequestEnergy( eGetDataType GetDataType, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述:  根据TAG获取电能长度
//         
//参数:      
//			GetDataType[in]	GetNormalData获取正常数据 GetRecordData 	 获取记录数据----注意电能返回脉冲数，仅冻结和事件使用，避免出错！！！！！-jwh
//			DataType[in]：	eData/eTagData
//			*pOAD[in]：		OAD
//         
//返回值:   返回OAD对应数据的长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestEnergyLen( eGetDataType GetDataType, BYTE DataType, BYTE *pOAD);


//--------------------------------------------------
//功能描述:  根据元素索引查找TCommonObj_t中Type2的偏移地址
//         
//参数:      ClassIndex[in]：	元素索引         
//           *Tag[in]：			标签
//         
//返回值:    返回 TCommonObj 中 Type2 的偏移
//         
//备注内容:  无
//--------------------------------------------------
WORD GetTagOffsetAddr(BYTE ClassIndex, const BYTE *Tag);

#if( MAX_PHASE != 1 )
//--------------------------------------------------
//功能描述:  读取需量
//         
//参数:      DataType[in]：	eData/eTagData/eAddTag        
//           Dot[in] :		小数点        
//           *pOAD[in]：	OAD         
//           *InBuf[in]：	需要添加标签的数据         
//           OutBufLen[in]：留给OutBuf的数据长度         
//           *OutBuf[in]：	返回的电能数据
//         
//返回值:    返回需量长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  费率个数不能超过12
//--------------------------------------------------
WORD GetRequestDemand( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述:  根据TAG获取需量长度
//         
//参数:      
//			DataType[in]：	eData/eTagData
//			*pOAD[in]：		OAD
//         
//返回值:   返回OAD对应数据的长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestDemandLen( BYTE DataType, BYTE* pOAD);
#endif

//--------------------------------------------------
//功能描述:  根据TAG获取变量长度
//         
//参数:      
//			DataType[in]：	eData/eTagData
//			*pOAD[in]：		OAD
//         
//返回值:   返回OAD对应数据的长度  0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestVariableLen( BYTE DataType, BYTE* pOAD);

//--------------------------------------------------
//功能描述:  读取变量
//         
//参数:      DataType[in]：	eData/eTagData/eAddTag        
//           Dot[in] :		小数点        
//           *pOAD[in]：	OAD         
//           *InBuf[in]：	需要添加标签的数据         
//           OutBufLen[in]：留给OutBuf的数据长度         
//           *OutBuf[in]：	返回的电能数据
//         
//返回值:    返回变量长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  
//--------------------------------------------------
WORD GetRequestVariable( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );
//--------------------------------------------------
//功能描述:  读取拓展模块变量
//         
//参数:      	DataType[in]：	eData/eTagData/eAddTag
//         	Dot[in]:		小数点(暂不用)        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:    返回变量长度 0x0000:数据长度不够  0x8000: 出现错误
//         
//备注内容:  
//--------------------------------------------------
WORD GetModuleRequestVariable(BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );
//--------------------------------------------------
//功能描述:  读取参变量（class8）数据 OI=4XXX
//         
//参数:  
//         	DataType[in]：	eData/eTagData/eAddTag
//         	Dot[in]:		小数点(暂不用)        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:  	返回数据长度
//         
//备注内容: 
//--------------------------------------------------
WORD GetRequestMeterPara( BYTE Ch,BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述: 读取参变量（class8）数据长度 OI=4XXX
//         
//参数:     
//         	DataType[in]：	eData/eTagData
//         	*pOAD[in]：		OAD   
//         
//返回值:   返回数据长度 
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestMeterParaLen( BYTE DataType, BYTE* pOAD);

//--------------------------------------------------
//功能描述:  读取冻结
//         
//参数:     
//         	DataType[in]：	eData/eTagData
//         	Dot[in]:		小数点        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:  	返回数据长度
//         
//备注内容: 只处理class9（属性3 关联对象属性表）
//--------------------------------------------------
WORD GetProFreezeRecord( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述:  读取（处理）事件属性数据
//         
//参数: 
//			Ch[in]：		规约通道    
//         	DataType[in]：	eData/eTagData
//         	Dot[in]:		小数点        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:  	返回数据长度
//         
//备注内容: 不处理 class7（属性2），class24（属性6~9）
//--------------------------------------------------
WORD GetProEventRecord( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述: 获取其他接口类属性数据
//         
//参数:
//         	DataType[in]：	eData/eTagData/eAddTag
//         	Dot[in]:		小数点        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:   数据长度
//         
//备注内容: OtherObj中支持的数据
//--------------------------------------------------
WORD GetOtherData( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf );

//--------------------------------------------------
//功能描述: 获取其他接口类的数据长度
//         
//参数:     
//          DataType[in]：eData/eTagData
//          *pOAD[in]     OAD    
//         
//返回值:   返回数据长度
//         
//备注内容: 
//--------------------------------------------------
WORD GetOtherDataLen( BYTE DataType, BYTE *pOAD);

//--------------------------------------------------
//功能描述: 获取只加TAG事件的数据长度
//         
//参数:     
//          DataType[in]：eData/eTagData
//          *pOAD[in]     OAD    
//          Number[in]：  成员数
//         
//返回值:   返回数据长度
//         
//备注内容:	事件上报状态
//			编程对象列表
//			超限期间需量最大值
//			事件清零列表 
//--------------------------------------------------
WORD GetProEventUnitLen( BYTE DataType, BYTE *pOAD, BYTE Number );

//--------------------------------------------------
//功能描述:  进行698帧格式的判断
//         
//参数:      BYTE ReceByte[in]
//         
//           TSerial * p[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
WORD DoReceProc_DLT698(BYTE ReceByte, TSerial * p);
WORD DoReceProc_13762(BYTE ReceByte, TSerial * p);
//--------------------------------------------------
//功能描述: 协议中获取DATA的实际数据长度
//         
//参数:      Buf[in]：  data数据
//
//返回值:     WORD  返回数据长度
//         
//备注内容:注意递归使用 避免无法跳出
//--------------------------------------------------
WORD GetProtocolDataLen( BYTE *Tag );

//--------------------------------------------------
//功能描述:  698.45上电函数
//         
//参数  :   无
//
//返回值:    无  
//         
//备注内容:  无
//--------------------------------------------------
void PowerUpDlt698(void);
//-----------------------------------------------
//功能描述  :   初始化698标志
//
//参数  : 	无
//
//返回值:      无
//
//备注内容  :   无
//-----------------------------------------------
void InitDLT698Flag( BYTE Ch );

//--------------------------------------------------
//函数功能: 初始化和698通信相关的变量
//
//参数:		TSerial *p  //通信口的指针
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void InitPoint_Dlt698(TSerial *p);
//--------------------------------------------------
//功能描述:  	添加协议一致性和功能一致性BUF
//         
//参数  : 	BYTE Mode    0: 设置全部     1：设置协议一致性         2：设置功能一致性
//
//          BYTE *pBuf[in] 输入Buf
//
//返回值:    	WORD 数据长度
//         
//备注内容:     输入buf长度函数未进行判断，务必保证输入BUF长度超过24个字节，避免超限
//--------------------------------------------------
WORD AddProtocolAndFunctionConformance( BYTE Mode,BYTE *pBuf );
//--------------------------------------------------
//功能描述: APDUBufFlag上电初始化
//         
//参数  :    无
//
//返回值:     无   
//         
//备注内容:    无
//--------------------------------------------------
void PowerUpInitAPDUBufFlag( void );
//--------------------------------------------------
//功能描述: APDUBufFlag初始化
//         
//参数  :    无
//
//返回值:     无   
//         
//备注内容:    无
//--------------------------------------------------
void InitAPDUBufFlag( BYTE Ch );

//--------------------------------------------------
//功能描述: 获取各通道默认最大帧buf长度
//         
//参数  :    BYTE Ch[in] 通道
//
//返回值:     默认长度   
//         
//备注内容:    无
//--------------------------------------------------
DWORD GetConsultBufLen( BYTE Ch );
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//
//返回值:    
//         
//备注:  仅第一次分帧调用
//--------------------------------------------------
BOOL  SetNextPara(  BYTE Ch,void* pgetcfun,DWORD oadd,WORD offset,BYTE *pPara,BYTE GetrequsetNextType);

#endif//(SEL_DLT698_2016_FUNC == YES )
#endif//__DLT698_45_2016_API_H
