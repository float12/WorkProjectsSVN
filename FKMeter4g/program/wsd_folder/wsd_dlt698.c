//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
#include "wsd_dlt698.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define LOCKSET_PARA_MAX_LEN				128
#define OBJ_START_FLAG						0x68		//起始字符
#define PPPINITFCS16 						0xffff 		/* Initial FCS value */
#define PPPGOODFCS16 						0xf0b8 	    /* Good final FCS value */
//支持DL/T698.45面向对象协议
#define PRO_DLT698_45						5	

#define DATA_MAXITEM						100

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
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef union TTwoByteUnion_t
{
	// 字方式 
	WORD w;
	// 字节方式 
	BYTE  b[2];
	// 有符号字
	SWORD sw;

}TTwoByteUnion;

typedef enum 
{
	eLockset_GetReqNormal = 0,
	eLockset_GetReqRecord,
	eLockset_SetReqNormal,
	eLockset_ActionReqNormal,
	eLockset_ActionRespNormal,
	eLockset_MessageTypeMax
}eReqMessageType_t;

typedef struct
{
	BYTE Para[LOCKSET_PARA_MAX_LEN];
	WORD Len;
}ParaInfo_t;

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
   	eData = 0,         	//只获取数据
   	eTagData,    		//数据+TAG
   	eAddTag,        	//只加TAG
   	eProtocolData,      //协议数据 
   	eMaxData = 0X55,    //最大源数据长度
}eDataTagChoice;//数据加TAG标志

typedef struct 
{
	BYTE ResultType;
	BYTE DataType;
	BYTE ArrayNum;
} TArrayLen;

typedef struct 
{
	DWORD Oad;
	BYTE  Dot;
} Normal_698;

typedef struct 
{
	double MaxDemand;
	char  DemandTime[7];
} Normal_Demand;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
double RealTimeData[DATA_MAXITEM];
double FreezeData[DATA_MAXITEM];
Normal_Demand  Demand[MAX_RATIO_NUM];//最大需量存储,需量+时表
// char  Demand[2][7];//最大需量存储,需量+时表

//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
//FCS lookup table as calculated by the table generator.
static const WORD fcstab[256] = 
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

Normal_698  OadNormalLists[] = 
{
	{0x200F0200,2},//频率
	{0x20000200,1},//电压
	{0x20000200,1},//电压  先用相电压代替
	{0x20010200,3},//电流
	{0x20040200,1},//有功w
	{0x20050200,1},//无功w
	{0x20060200,1},//视在w
	{0x200A0200,3},//功率因数
	{0x00100200,2},//正向有功电能
	{0x00200200,2},//反向有功电能
	{0x00300201,2},//组合无功1电能
	{0x00400201,2},//组合无功2电能
};

Normal_698  OadRecordLists[] = 
{
	{0x00100200,2},//正向有功电能
	{0x00200200,2},//反向有功电能
	{0x00300201,2},//组合无功1电能
	{0x00400201,2},//组合无功2电能
};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: fcs校验函数
//
//参数:		BYTE *cp  
//			WORD len   
//返回值:	WORD
//		   
//备注:
//-----------------------------------------------
static WORD fcs16(BYTE *cp, WORD len)
{
	WORD cpLen;
	WORD fcs;
	
	fcs = PPPINITFCS16;
	cpLen = len;
	while (cpLen--)
	{
		fcs = ( (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	}

	fcs ^= 0xffff; // complement  //此为FCS值
	return fcs;
}
//--------------------------------------------------
//功能描述:  方法1读取冻结
//         
//参数:      DateTime[in] 指定时间 OadTable[in] 抄读OAD项
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_GetRequestRecordMethod1( BYTE *DateTime ,BYTE *OadTable)
{
	BYTE buf[512];//！！！512可能不够
	WORD  i = 0, wLen = 0,j = 0;
	TTwoByteUnion Hrc, Fcs;
	buf[i++] = 0x68;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0x43;
	buf[i++] = 0x05;
	lib_ExchangeData((BYTE*)&buf[i],(BYTE*)&bAddr[0],6);
	i = i+6;
	buf[i++] = 0xa0;
	
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0x05;
	buf[i++] = 0x03;
	buf[i++] = 0x00;
	//分钟冻结OAD
	buf[i++] = 0x50;
	buf[i++] = 0x02;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	//方法
	buf[i++] = 1;
	//RSD
	buf[i++] = 0x20;
	buf[i++] = 0x21;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	buf[i++] = 0x1C;//类型28
	memcpy((BYTE*)&buf[i],DateTime,7);
	i = i+7;
	//RCSD
	buf[i++] = sizeof(OadRecordLists)/sizeof(Normal_698);
	//OAD
	for(j=0;j<(sizeof(OadRecordLists)/sizeof(Normal_698));j++)
	{
		buf[i++] = 0;
		lib_ExchangeData((BYTE*)&buf[i],(BYTE*)&OadRecordLists[j].Oad,sizeof(OadRecordLists[j].Oad));
		i = i+sizeof(OadRecordLists[j].Oad);
	}	
	buf[i++] = 0;//timetag

	wLen = i+1;
	buf[1] = (BYTE)wLen; //长度
	buf[2] = (BYTE)(wLen>>8);

	Hrc.w = fcs16((BYTE *)&buf[1], 11);
	buf[12] = Hrc.b[0];
	buf[13] = Hrc.b[1];
	Fcs.w = fcs16((BYTE *)&buf[1], i-1);
	buf[i++] = Fcs.b[0];
	buf[i++] = Fcs.b[1];
	buf[i++] = 0x16;

	nwy_ext_echo("api_GetRequestRecord:");
	for (j = 0; j < i; j++)
	{
		nwy_ext_echo("%02x ",buf[j]);
	}
	nwy_uart_send_data(UART_HD, buf, i);
}
//--------------------------------------------------
//功能描述:  698读取若干个对象属性
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_GetRequestNormalList(void)
{
	BYTE buf[512];//！！！512可能不够
	WORD  i = 0, wLen = 0,j = 0;
	TTwoByteUnion Hrc, Fcs;
	buf[i++] = 0x68;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0x43;
	buf[i++] = 0x05;
	lib_ExchangeData((BYTE*)&buf[i],(BYTE*)&bAddr[0],6);
	i = i+6;
	buf[i++] = 0xa0;
	
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0x05;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	buf[i++] = sizeof(OadNormalLists)/sizeof(Normal_698);
	//OAD
	for(j=0;j<(sizeof(OadNormalLists)/sizeof(Normal_698));j++)
	{
		lib_ExchangeData((BYTE*)&buf[i],(BYTE*)&OadNormalLists[j].Oad,sizeof(OadNormalLists[j].Oad));
		i = i+sizeof(OadNormalLists[j].Oad);
	}	
	buf[i++] = 0;//timetag

	wLen = i+1;
	buf[1] = (BYTE)wLen; //长度
	buf[2] = (BYTE)(wLen>>8);

	Hrc.w = fcs16((BYTE *)&buf[1], 11);
	buf[12] = Hrc.b[0];
	buf[13] = Hrc.b[1];
	Fcs.w = fcs16((BYTE *)&buf[1], i-1);
	buf[i++] = Fcs.b[0];
	buf[i++] = Fcs.b[1];
	buf[i++] = 0x16;

	nwy_ext_echo("api_GetRequestNormalList:");
	for (j = 0; j < i; j++)
	{
		nwy_ext_echo("%02x ",buf[j]);
	}
	nwy_uart_send_data(UART_HD, buf, i);	
}
//--------------------------------------------------
//功能描述:  698 获取单个对象属性
//         
//参数:      OI   属性  索引
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_GetRequestNormal( WORD OI,BYTE attribute,BYTE index )
{
	BYTE i = 0;
	BYTE buf[60];
	TTwoByteUnion Hrc, Fcs;

	buf[i++] = 0x68;
	buf[i++] = 0x17;
	buf[i++] = 0x00;
	buf[i++] = 0x43;
	buf[i++] = 0x05;
	lib_ExchangeData((BYTE*)&buf[i],(BYTE*)&bAddr[0],6);
	i = i+6;
	buf[i++] = 0xa0;
	Hrc.w = fcs16((BYTE *)&buf[1], i-1);
	buf[i++] = Hrc.b[0];
	buf[i++] = Hrc.b[1];

	buf[i++] = 0x05;
	buf[i++] = 0x01;
	buf[i++] = 0x00;
	//OI
	lib_ExchangeData((BYTE*)&buf[i],(BYTE*)&OI,2);
	i = i+2;
	//属性
	buf[i++] = attribute;
	//索引
	buf[i++] = index;
	buf[i++] = 0;//timetag
	Fcs.w = fcs16((BYTE *)&buf[1], i-1);
	buf[i++] = Fcs.b[0];
	buf[i++] = Fcs.b[1];
	buf[i++] = 0x16;

	nwy_ext_echo("api_GetRequestNormal:");
	for (BYTE j = 0; j < i; j++)
	{
		nwy_ext_echo("%02x ",buf[j]);
	}
	nwy_uart_send_data(UART_HD, buf, i);	
}
//--------------------------------------------------
//功能描述:  心跳报文组帧
//         
//参数:       bLink 0 登录 1心跳
//         	  transdata 传入地址
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Send_Heartbeatframe_698( tTranData *transdata,BYTE bLink)
{
	char MessageBuf[50];
	BYTE start[5] = {0x68,0x1e,0x00,0x81,0x05};
	BYTE datamid[] = {0x01,0x01};// Linkrequest
	WORD heartbeat = 0;
	TRealTimerDate time = {0};	
	TTwoByteUnion  Hrc,Fcs;

	nwy_nw_get_signal_csq((BYTE*)&heartbeat);
	memset(MessageBuf,0,sizeof(MessageBuf));
	memcpy((char*)&MessageBuf,(char*)&start,sizeof(start));
	// memcpy((char*)&MessageBuf[5],(char*)&bAddr,sizeof(bAddr));
	lib_ExchangeData((BYTE*)&MessageBuf[5],(BYTE*)&bAddr[0],6);
	MessageBuf[11] = 0;//客户机地址
	Hrc.w= fcs16((BYTE*)&MessageBuf[1],11);
	MessageBuf[12] = Hrc.b[0];
	MessageBuf[13] = Hrc.b[1];	
	memcpy((char*)&MessageBuf[14],(char*)&datamid,sizeof(datamid));
	MessageBuf[16] = bLink;
	lib_ExchangeData((BYTE*)&MessageBuf[17],(BYTE*)&heartbeat,sizeof(heartbeat));
	GetTimeto698(&time);	
	memcpy((char*)&MessageBuf[19],(char*)&time,sizeof(time));
	Fcs.w = fcs16((BYTE*)&MessageBuf[1],29-1);
	MessageBuf[29] = Fcs.b[0];
	MessageBuf[30] = Fcs.b[1]; 
	MessageBuf[31] = 0x16; 
	nwy_ext_echo("\r\n 698 heart is\r\n");
	for (BYTE i = 0; i < 40; i++)
	{
		nwy_ext_echo("%02x",MessageBuf[i]);
	}
	memcpy(transdata->buf,MessageBuf,32);
	transdata->len = 32;
	nwy_put_msg_que(UartDataToTcpMsgQue, transdata, 0xffffffff);
}
//--------------------------------------------------
//功能描述:  进行698报文帧起始符68搜索
//         
//参数:      
//           TSerial * p[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void DoSearch_68_DLT698(TSerial *p)
{
	WORD i, wNum;
	wNum = p->ProStepDlt698_45;//3
	for(i = 1; i < wNum; i++)//i  1 2
	{
		// nwy_ext_echo("begin pos %d\r\n", p->BeginPosDlt698_45);
		if(p->ProBuf[(p->BeginPosDlt698_45 + i) % UART_BUFF_MAX_LEN] == 0x68)
		{
			if(p->ProStepDlt698_45 == 0)
			{
				p->ProStepDlt698_45 = UART_BUFF_MAX_LEN - 1;
			}
			else
			{
				p->ProStepDlt698_45--;
			}
			p->BeginPosDlt698_45 += i;
			if(p->BeginPosDlt698_45 >= UART_BUFF_MAX_LEN)
			{
				p->BeginPosDlt698_45 = 0;
			}
			break;
		}
	}
	if(i >= wNum)
	{	
		p->ProStepDlt698_45 = 0;
		// nwy_ext_echo("\r\n2 pro setp is set to 0");
	}
}
//-----------------------------------------------
// 函数功能: 判断校验是否正确
//
// 参数:		Type 0--HCS 1--FCS
//
// 返回值:
//
// 备注:一口多规约情况下，缓冲为循环缓冲，算校验前要先把缓冲中数据移到一个暂时缓冲
//-----------------------------------------------
 WORD JudgeDlt698_45_HCS_FCS(BYTE Type, TSerial *p)
{
	WORD i, wLen, wData;
	BYTE Buf[MAX_PRO_BUF + 30];

	if(Type == 0)
	{
		wLen = (4 + p->Addr_Len + 1);
	}
	else if(Type == 1)
	{
		wLen = p->wLen - 2;
	}
	else
	{
		return FALSE;
	}
	if(wLen >= (MAX_PRO_BUF - 1))
	{
		return FALSE;
	}

	for(i = 0; i < wLen; i++)
	{
		Buf[i] = p->ProBuf[(p->BeginPosDlt698_45 + 1 + i) % MAX_PRO_BUF];
	}

	// 68 L L 43 05 01 00 29 01 16 20 0A HCS_L HCS_H 10 00 08 05 01 01 40 01 02 00 00 00 85 01 02 03 06 12 34 56 78 90 12 04 12 34 56 78 FCS_L FCS_H 16
	wData = p->ProBuf[(p->BeginPosDlt698_45 + p->ProStepDlt698_45 - 1) % MAX_PRO_BUF] * 0x100 + p->ProBuf[(p->BeginPosDlt698_45 + p->ProStepDlt698_45 - 2) % MAX_PRO_BUF];
	if(wData != fcs16(Buf, wLen))   // 对报文进行校验
	{
		return FALSE;
	}

	return TRUE;
}
//功能描述:  获取基本数据类型长度
//         
//参数:      DataType[in]：eData/eTagData/eProtocolData 除eData外任何输入都是获取加TAG长度         
//           *Tag[in]		Tag[0]--数据类型 Tag[1]--数据长度
//
//返回值:    数据长度
//         
//备注内容:DataType ！=eProtocolData，tag的数据类型为Tag[0]--数据类型 Tag[1]--数据长度
//        DataType == eProtocolData，tag的数据类型为Tag[0]--数据类型+tag数据
//--------------------------------------------------
static WORD GetBasicTypeLen( BYTE DataType, BYTE Tag )
{
	WORD Len = 0;
	
	switch( Tag )
	{
		case NULL_698:
			// Len = ( (DataType==eData) ? 1 : 1 );
			break;
		case Boolean_698:
		case Integer_698:
		case Unsigned_698:
		case Enum_698:
			Len = ( (DataType==eData) ? 1 : 2 );
			break;	
		case Double_long_698:
		case Double_long_unsigned_698:
		case Float32_698:
		case OAD_698:
		case OMD_698:
			Len = ( (DataType==eData) ? 4 : 5 );
			break;						
		case Long_698:
		case Long_unsigned_698:
		case OI_698:
		case Scaler_Unit_698:
			Len = ( (DataType==eData) ? 2 : (2+1) );
			break;
		case Long64_698:
		case Long64_unsigned_698:
		case Float64_698:
			Len = ( (DataType==eData) ? 8 : (8+1) );
			break;
		case Date_time_698:
			Len = ( (DataType==eData) ? 10 : (10+1) );
			break;
		case Date_698:
		case COMDCB_698:
			Len = ( (DataType==eData) ? 5 : (5+1) );
			break;
		case Time_698:
		case TI_698:
			Len = ( (DataType==eData) ? 3 : (3+1) );
			break;
		case DateTime_S_698:
			Len = ( (DataType==eData) ? 7 : (7+1) );
			break;
	    default:
			Len = 0x8000;
			break;
	}

	return Len;
}
//--------------------------------------------------
//功能描述:  对于698 getNormalList 回复数据的解析
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL Rx_GetRequestNormalList(BYTE *pBuf)
{
	BYTE NormalNum, i, j;
	DWORD Oad;
	WORD wLen;
	TArrayLen GetResult;
	BYTE *OffsetBuf = pBuf;
	SDWORD dataIndex = 0,tempData = 0;  // Energy_test数组索引

	NormalNum = *OffsetBuf;
	OffsetBuf++;  // 指向第一个OAD开始位置
	
	nwy_ext_echo("\r\n NormalNum = %d", NormalNum);

	if (NormalNum == sizeof(OadNormalLists)/sizeof(Normal_698))
	{
		for (i = 0; i < NormalNum; i++)
		{
			// 解析OAD (4字节)
			lib_ExchangeData((BYTE*)&Oad, OffsetBuf, sizeof(DWORD));
			OffsetBuf += sizeof(DWORD);
			
			nwy_ext_echo("\r\n OAD[%d] = 0x%08X", i, Oad);
			
			// 检查OAD是否匹配预期列表
			if (Oad != OadNormalLists[i].Oad)
			{
				nwy_ext_echo("\r\n OAD dismatch! get: 0x%08X, return: 0x%08X",OadNormalLists[i].Oad, Oad);
				return FALSE;
			}
			// 解析结果类型描述符 (3字节)
			memcpy((BYTE*)&GetResult, OffsetBuf, 3);
			OffsetBuf += 3;
			
			nwy_ext_echo("\r\n ResultType=%d, DataType=%d, ArrayNum=%d",GetResult.ResultType, GetResult.DataType, GetResult.ArrayNum);

			if (GetResult.ResultType == 1)  // 数据结果
			{
				if (GetResult.DataType == Array_698)
				{
					// 解析数组元素数据类型 (1字节)
					BYTE elementDataType = *OffsetBuf;
					OffsetBuf++;
					
					nwy_ext_echo("\r\n Element DataType = 0x%02X", elementDataType);
					
					// 根据数据类型确定元素长度
					wLen = GetBasicTypeLen(eData,elementDataType);
					
					nwy_ext_echo("\r\n Element length = %d", wLen);
					
					// 解析数组数据
					for (j = 0; j < GetResult.ArrayNum && dataIndex < DATA_MAXITEM; j++)
					{
						// 根据实际长度拷贝数据
						{
							lib_ExchangeData((BYTE*)&tempData, OffsetBuf, wLen);
							//nwy_ext_echo("\r\n tempData[%d] = 0x%04X", dataIndex, tempData);
							RealTimeData[dataIndex] = tempData/(1.0 * pow(10, OadNormalLists[i].Dot));
							nwy_ext_echo("\r\n RealTimeData[%d] = %f", dataIndex, RealTimeData[dataIndex]);
						}
						OffsetBuf += (wLen+1);
						dataIndex++;
					}
				}
				else
				{
					//Structure_698 类型太麻烦，不考虑,只考虑一个数据项情况，索引不是0的情况
					nwy_ext_echo("\r\n Non-array data type: 0x%02X", GetResult.DataType);
					// 根据DataType跳过相应长度的数据
					wLen = GetBasicTypeLen(eData,GetResult.DataType);
					OffsetBuf--;
					lib_ExchangeData((BYTE*)&tempData, OffsetBuf, wLen);
					nwy_ext_echo("\r\n tempData[%d] = 0x%04X", dataIndex, tempData);
					RealTimeData[dataIndex] = tempData/(1.0 * pow(10, OadNormalLists[i].Dot));
					nwy_ext_echo("\r\n RealTimeData[%d] = %f", dataIndex, RealTimeData[dataIndex]);
					OffsetBuf += (wLen+1);
					dataIndex++;
				}
			}
			else
			{
				// 错误或其他结果类型处理
				nwy_ext_echo("\r\n ResultType error: %d", GetResult.ResultType);
				continue;//某项返回错误，对应偏移未计算 待处理！！！
			} 
			OffsetBuf--;
		}
		nwy_ext_echo("\r\n Total parsed %d values", dataIndex);
		return TRUE;
	}
	else
	{
		nwy_ext_echo("\r\n NormalNum error: get %d, return %d",sizeof(OadNormalLists)/sizeof(Normal_698), NormalNum);
		return FALSE;
	}
}
//--------------------------------------------------
//功能描述:  对于698 getRecord 回复数据的解析
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL Rx_GetRequestRecord(BYTE *pBuf)
{
	BYTE NormalNum, i;

	WORD wLen;
	BYTE *ReturnOad = NULL,*OffsetBuf = pBuf;
	DWORD FreezeOad = 0;
	SDWORD dataIndex = 0,tempData = 0;  // Energy_test数组索引

	lib_ExchangeData((BYTE*)&FreezeOad,OffsetBuf,4);
	nwy_ext_echo("\r\n FreezeOad = 0x%04lx", FreezeOad);

	OffsetBuf += 4;
	NormalNum = *OffsetBuf;
	nwy_ext_echo("\r\n NormalNum = %d", NormalNum);

	OffsetBuf++;
	ReturnOad = OffsetBuf;
	ReturnOad++;

	if (NormalNum == sizeof(OadRecordLists)/sizeof(Normal_698))
	{
		OffsetBuf = OffsetBuf + (4+1)*NormalNum;
		
		nwy_ext_echo("\r\n RecordType = %d", *OffsetBuf);
		//响应数据，1条记录
		if (*OffsetBuf == 1)//结果类型
		{
			OffsetBuf++;
			nwy_ext_echo("\r\n TotalLen = %d", *OffsetBuf);//1条记录
			OffsetBuf++;
			for (i = 0; i < NormalNum; i++)
			{
				lib_ExchangeData((BYTE*)&FreezeOad,ReturnOad,4);
				nwy_ext_echo("\r\n FreezeOad[%d] = 0x%4lx",dataIndex ,FreezeOad);
				
				BYTE elementDataType = *OffsetBuf;//若是结构体类型或者数据类型则是struct+len/array+len，若是数据类型+数据本身
				OffsetBuf++;
				BYTE DataLen = *OffsetBuf;

				if(elementDataType == Array_698)
				{
					OffsetBuf +=2;//获取成员变量

					for (BYTE j = 0; j < DataLen; j++)
					{
						lib_ExchangeData((BYTE*)&tempData, OffsetBuf, 4);//数组内成员变量，目前只使用冻结，长度先按固定四字节处理
						nwy_ext_echo("\r\n tempData[%d] = 0x%04X", dataIndex, tempData);
						FreezeData[dataIndex] = tempData/(1.0 * pow(10, OadNormalLists[i].Dot));
						nwy_ext_echo("\r\n FreezeData[%d] = %f", dataIndex, FreezeData[dataIndex]);

						OffsetBuf += (4+1);
						dataIndex++;
					}
					OffsetBuf -= 1;
				}
				else 
				{
					wLen = GetBasicTypeLen(eData,elementDataType);
					lib_ExchangeData((BYTE*)&tempData, OffsetBuf, wLen);
					nwy_ext_echo("\r\n tempData[%d] = 0x%04X", dataIndex, tempData);
					FreezeData[dataIndex] = tempData/(1.0 * pow(10, OadNormalLists[i].Dot));
					nwy_ext_echo("\r\n FreezeData[%d] = %f", dataIndex, FreezeData[dataIndex]);
					OffsetBuf += wLen;
					dataIndex++;
				}
				ReturnOad += 5;
			}
		}
	}

	return TRUE;
}
//--------------------------------------------------
//功能描述:  对于698 getRequsetNormal 回复数据的解析
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  Rx_GetRequestNormal( BYTE *pBuf )
{
	DWORD Oad = 0,wLen;
	BYTE *OffsetBuf = pBuf,bIndex;
	TArrayLen GetResult;

	lib_ExchangeData((BYTE*)&Oad,OffsetBuf,2);
	OffsetBuf = OffsetBuf + 2;
	// bAttribute = *OffsetBuf;
	OffsetBuf++;
	bIndex = *OffsetBuf;
	OffsetBuf++;
	// 解析结果类型描述符 (3字节)
	memcpy((BYTE*)&GetResult, OffsetBuf, 3);

	switch (Oad)
	{
		case 0x1010://正向有功最大需量
			if(GetResult.ResultType == 1)
			{
				if (bIndex == 00)
				{
					if (GetResult.DataType == Array_698)
					{
						for (BYTE i = 0; i < GetResult.ArrayNum; i++)
						{
							if (*OffsetBuf == Structure_698)
							{
								OffsetBuf += 3;
								wLen = GetBasicTypeLen(eData,*OffsetBuf);
								//解析数据
								OffsetBuf++;

								memcpy((BYTE*)&Demand[0].MaxDemand,OffsetBuf,wLen);
								OffsetBuf = OffsetBuf + wLen + 1;
								memcpy(Demand[0].DemandTime,OffsetBuf,7);
							}
							// OffsetBuf
						}
					}
				}
				else if (bIndex == 01)
				{
					if (GetResult.DataType == Structure_698)
					{
						nwy_ext_echo("\r\n Rx_GetRequestNormal");
						OffsetBuf = OffsetBuf+3;
						// for (BYTE i = 0; i < GetResult.ArrayNum; i++)
						{
							wLen = GetBasicTypeLen(eData,*OffsetBuf);
							//解析数据
							OffsetBuf++;
							// Demand[i].MaxDemand
							memcpy((BYTE*)&Demand[0].MaxDemand,OffsetBuf,wLen);
							// memcpy(&Demand[i][0],OffsetBuf,wLen);
				
							// for (BYTE j = 0; j < wLen; j++)
							// {
							// 	nwy_ext_echo("[%02x]",Demand[i][j]);
							// }
							OffsetBuf = OffsetBuf + wLen + 1;
							memcpy(Demand[0].DemandTime,OffsetBuf,7);
						}
					}
				}
			}
			else
			{
				return FALSE;
			}
			break;

		default:
			break;
	}
	return FALSE;
}
//--------------------------------------------------
//功能描述:  4G模块 对接受的报文解析(698规约)
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  RxUartMessage_Dlt698( TSerial *p )
{
	BYTE *buf = NULL;
	BYTE bReceiveBit = 0;
	BYTE bResponse;
	double *rtdataAddr = NULL,*fzdataAddr = NULL;
	TUartToMqttData UartToMqttData = {0};

	bResponse = p->ProBuf[p->Addr_Len+8];
	buf = &(p->ProBuf[p->Addr_Len+9]);
	rtdataAddr = &RealTimeData[0];
	fzdataAddr = &FreezeData[0];//目前用的一个一个buf，队列拥堵时会覆盖 待优化！！！


	switch (bResponse)
	{
		case 0x85:
			if (buf[0] == 0x01)//RX_Normal
			{
				if (Rx_GetRequestNormal(&buf[2]))
				{
					bReceiveBit = 1;
				}
			}
			else if (buf[0] == 0x02)//RX_NormalLists
			{
				if(Rx_GetRequestNormalList(&buf[2]))
				{
					UartToMqttData.Type = eRealTimeData;
					UartToMqttData.Data.RealTimeDataAddr = rtdataAddr;
					//上报实时数据
					if (nwy_put_msg_que(UartReplyToMqttMsgQue, &UartToMqttData, 0xffffffff)) // 组MQTT帧进行发送
					{
						nwy_ext_echo("\r\n report RealTimeData succcess");
					}
					bReceiveBit = 1;
				}
			}
			else if (buf[0] == 0x03)//RX_Record
			{
				if(Rx_GetRequestRecord(&buf[2]))
				{
					UartToMqttData.Type = eFreezeData;
					UartToMqttData.Data.FreezeDataAddr = fzdataAddr;
					//上报冻结数据
					if (nwy_put_msg_que(UartReplyToMqttMsgQue, &UartToMqttData, 0xffffffff)) // 组MQTT帧进行发送
					{
						nwy_ext_echo("\r\n report FreeezeData succcess");
					}
					bReceiveBit = 1;
				}
			}
			break;
		case 0x86:
			break;
		default:
			break;
	}
	if (bReceiveBit == 1)
	{
		ReceiveClearBitFlag();
	}
}
//--------------------------------------------------
// 功能描述:  进行698格式判断
//
// 参数:      BYTE ReceByte[in]
//
//           TSerial * p[in]
//
// 返回值:
//
// 备注内容:  无
//--------------------------------------------------
WORD DoReceProc_Dlt698_UART(BYTE ReceByte, TSerial *p)
{
	// 搜索同步头
	
	if(p->ProStepDlt698_45 == 0)
	{
		if(ReceByte == 0x68)
		{
			// Dlt698.45规约报文在Serial[].ProBuf中的开始位置
			p->BeginPosDlt698_45 = p->RXRPoint;

			// 操作指针
			p->ProStepDlt698_45++;
		}
	}
	else if(p->ProBuf[p->BeginPosDlt698_45] == 0x68)
	{
		// 操作指针
		p->ProStepDlt698_45++;
		if(p->ProStepDlt698_45 < 3)   // 如果没收到3字节，即长度域没收全，则不处理
		{
		}
		else if(p->ProStepDlt698_45 == 3)   // 已经收到第3个数了，即长度L
		{
			p->wLen = (ReceByte * 0x100 + p->ProBuf[p->BeginPosDlt698_45 + 1]);
			if(p->wLen > (UART_BUFF_MAX_LEN - 2))   //(UART_BUFF_MAX_LEN-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			if(p->wLen < (10 + 6))   // 长度域最小为多少，通信地址是6字节
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else if(p->ProStepDlt698_45 == 4)   // 已经收到第4个数了，即控制域C
		{
			if(!(ReceByte & 0x80))   // 如果不是服务器回复报文重搜起始符
			{
				DoSearch_68_DLT698(p);
				
				return FALSE;
			}
			else if(ReceByte == 0x01)
			{
				// 只判断是否 是链路管理 其余均返回false
			}
			else
			{
				return FALSE;
			}
			// //功能码判断，暂没做
			// //1	链路管理	链路连接管理（登录，心跳，退出登录）
			// //3	用户数据	应用连接管理及数据交换服务
			// if( (ReceByte & 0x07) != 0x03 )//最好应答 ERROR-Response ，但矩泉陈兴东不应答鼎信测试软件过了 2017-7-29 wlk
			// {
			// 	DoSearch_68_DLT698(p);
			// 	return FALSE;
			// }
		}
		else if(p->ProStepDlt698_45 == 5)   // 已经收到第5个数了，即服务器地址SA的第1字节
		{
			// a)	bit0…bit3：为地址的字节数，取值范围：0…15，对应表示1…16个字节长度；
			// b)	bit4…bit5：逻辑地址；
			// c)	bit6…bit7：为服务器地址的地址类型，0表示单地址，1表示通配地址，2表示组地址，3表示广播地址。
			p->Addr_Len = (ReceByte & 0xf) + 1;
			if((ReceByte / 0x40) <= 1)   // 如果是单地址、通配地址
			{
				if(p->Addr_Len != 6)   // 电表地址是6字节BCD，即12位BCD码
				{
					DoSearch_68_DLT698(p);
					return FALSE;
				}
			}
		}
		else if(p->ProStepDlt698_45 == (8 + p->Addr_Len))   // 地址收完了，HCS收完了，判断HCS是否正确
		{
			if(p->Addr_Len > 6)
			{
				DoSearch_68_DLT698(p);
				
				return FALSE;
			}
			// //一口多规约情况下，缓冲为循环缓冲，算校验前要先把缓冲中数据移到一个昨时缓冲
			if(JudgeDlt698_45_HCS_FCS(0, p) == FALSE)
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			for(BYTE i = 0; i < p->Addr_Len; i++)
			{
				if(reverAddr[i] != p->ProBuf[p->BeginPosDlt698_45 + 5 + i])
				{
					DoSearch_68_DLT698(p);
					nwy_ext_echo("\r\n addr is error \r\n");
					return FALSE;
				}
			}	
			// nwy_ext_echo("\r\n addr is correct \r\n");
			// //判断通信地址是否正确
			// if( JudgeRecMeterNo_Dlt698_45(p) == FALSE )
			// {

			// }
		}
		else if(p->ProStepDlt698_45 == (1 + p->wLen))   // 收到帧结束符 16H了，判断FCS是否正确
		{
			// 一口多规约情况下，缓冲为循环缓冲，算校验前要先把缓冲中数据移到一个昨时缓冲
			if(JudgeDlt698_45_HCS_FCS(1, p) == FALSE)
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else if(p->ProStepDlt698_45 == (2 + p->wLen))   // 收到帧结束符 16H了,暂没判断16H是否接收正确
		{
			if(ReceByte == 0x16)
			{
				// 	//把数据从报文开始位置 向 以p->ProBuf[0]开始处移
				api_DoReceMoveData( p,PROTOCOL_698);   // 最好放在 698.45协议处理函数 中

				if (ProcUpgradeResponse698()) // 处理完成一帧
				{
				}
				else//处理其他698报文
				{
					RxUartMessage_Dlt698(p);
				}
				// 处理完成
				InitPoint(p);
				// nwy_ext_echo("\r\n  698 RECEIVE process over \r\n");

				return TRUE;
			}
			else
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else
		{
			if(p->ProStepDlt698_45 >= UART_BUFF_MAX_LEN)   //(UART_BUFF_MAX_LEN-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
			{
				nwy_ext_echo("\r\n3 pro setp is set to 0 ");
				p->ProStepDlt698_45 = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}