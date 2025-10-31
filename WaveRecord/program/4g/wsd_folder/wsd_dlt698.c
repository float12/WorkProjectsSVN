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

typedef struct
{
	eReqMessageType_t Type;	// Apdu类型
	DWORD Oad;				// OAD或OMD
	TSA Sa;					// 服务器地址
	BYTE ClientAddr;		// 客户机地址
	BYTE Ctrl;				// 控制字
	ParaInfo_t ParaInfo;	// 参数
	eSecurityMode SafeMode;	// 报文使用的安全模式
	BYTE* pOutBuf;
	WORD *pOutBufLen;
}GenerateReqMessage_t;


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE InstantData698Frame[INSTANT_DATA_698_FRAME_LEN]; // 698帧缓冲
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
//一相的数据格式
eDataType698 InstantDataFormatTable[DATA_ITEM_NUM_PER_PHASE] = 
{
	efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,
	efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,
	efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,efloat32,efloat32
};
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------


// FCS lookup table as calculated by the table generator.
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
//-----------------------------------------------
// 函数功能: 计算FCS校验值
//
// 参数:		BYTE *cp  
//			WORD len   
// 返回值:	WORD	
// 备注:	
//-----------------------------------------------
WORD api_fcs16(BYTE *cp, WORD len)
{
	return fcs16(cp, len);
}
//--------------------------------------------------
// 功能描述:  瞬时量上传698组帧
//
// 参数:       buf 数据缓冲
//         	  len 数据长度
// 返回值:
//
// 备注:
//--------------------------------------------------
void  Compose_InstantData_698(void)
{
	TTwoByteUnion  Hcs,Fcs;
	WORD i = 0 , j = 0,k = 0;
	BYTE *pTmp = &PublishInstantData[0];
	
	memset(&Hcs, 0, sizeof(Hcs));
	memset(&Fcs, 0, sizeof(Fcs));
	memset(InstantData698Frame, 0, sizeof(InstantData698Frame));

	InstantData698Frame[i++] = 0x68;
	//长度域
	InstantData698Frame[i++] = LLBYTE(INSTANT_DATA_698_FRAME_LEN-2);
	InstantData698Frame[i++] = LHBYTE(INSTANT_DATA_698_FRAME_LEN-2);
	//控制域
	InstantData698Frame[i++] = 0xc3;
	//SA标志
	InstantData698Frame[i++] = 0x05;
	//表地址
	memcpy(&InstantData698Frame[i], &reverAddr[0], 6);
	i += 6;
	//客户机地址
	InstantData698Frame[i++] = 0x00;
	//校验
	Hcs.w = fcs16((BYTE *)&InstantData698Frame[1], 11);
	InstantData698Frame[i++] = Hcs.b[0];
	InstantData698Frame[i++] = Hcs.b[1];

	InstantData698Frame[i++] = 0x85;
	InstantData698Frame[i++] = 0x01;
	InstantData698Frame[i++] = 0x00;
	memset(&InstantData698Frame[i], 0xee, 4);//oad
	i += 4;

	InstantData698Frame[i++] = 0x01;
	//结构体 （数组+序号）
	InstantData698Frame[i++] = 0x02;
	InstantData698Frame[i++] = 0x02;
	//数组类型
	InstantData698Frame[i++] = 01;
	InstantData698Frame[i++] = 03;
	//三相结构体类型数据
	for( j = 0; j < MAX_PHASE_NUM; j++)
	{
		InstantData698Frame[i++] = 02;
		InstantData698Frame[i++] = DATA_ITEM_NUM_PER_PHASE;
		for (k = 0; k < DATA_ITEM_NUM_PER_PHASE; k++)
		{
			if (InstantDataFormatTable[k] == edouble_long)
			{
				InstantData698Frame[i++] = 5;
			}
			else if (InstantDataFormatTable[k] == efloat32)
			{
				InstantData698Frame[i++] = 23;
			}
			memcpy(&InstantData698Frame[i], pTmp, 4);
			i += 4;
			pTmp += 4;
		}
	}
	//序号
	InstantData698Frame[i++] = 5;
	memcpy(&InstantData698Frame[i], pTmp, 4);
	i += 4;
	//跟随上报和时间标签
	InstantData698Frame[i++] = 0x00;
	InstantData698Frame[i++] = 0x00;
	//校验
	Fcs.w = fcs16((BYTE *)&InstantData698Frame[1],i - 1);
	InstantData698Frame[i++] = Fcs.b[0];
	InstantData698Frame[i++] = Fcs.b[1];
	InstantData698Frame[i++] = 0x16;
	nwy_ext_echo("\r\n698len:%d\r\n", i);
	for (WORD j = 0; j < i; j++)
	{
		nwy_ext_echo("%02x ", InstantData698Frame[j]);
	}
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
void  Send_Heartbeatframe_698( tTranData *transdata,BYTE bLink )
{
	char MessageBuf[50];
	BYTE start[5] = {0x68,0x1e,0x00,0x81,0x05};
	BYTE datamid[] = {0x01,0x01};// Linkrequest
	WORD heartbeat = 60;
	TRealTimerDate time = {0};	
	TTwoByteUnion  Hrc,Fcs;
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
	nwy_put_msg_que(TranDataToServerMessageQueue, transdata, 0xffffffff);
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

				// //	此处调用698.45协议处理函数//ProcDlt698App();//ProMessage( p );
				// 	ProcMessageDlt698(p);				
				if (ProcUpgradeResponse698()) // 处理完成一帧
				{
				}
				else//处理其他698报文
				{
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