//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人
//创建日期
//描述		DLT645头文件
//修改记录
//----------------------------------------------------------------------
#ifndef __DLT645_H
#define __DLT645_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define TCP_REQ_NET 		0x040013D1		//TCP IP地址和端口号
#define TCP_REQ_USER_PD 	0x040013D6  	//TCP 用户名和密码
#define MQTT_REQ_NET 		0x040013E1		//MQTT IP地址和端口号
#define MQTT_REQ_USER_PD 	0x040013E6 		//MQTT 用户名和密码
#define DAY_TIME 			0x0400010C		//日期和时间
#define USED_CHANNEL_NUM 	0x04A00100 		//测量点使用回路数
#define READ_RELAY_STATUS 			0x1126		    //检查继电器状态
#define TIME_ZONE_NUM		0x04000201		//年时区数
#define TIME_SEGTABLE_NUM	0X04000202		//日时段表数
#define TIME_SEG_NUM		0X04000203		//日时段数
#define TIME_RATIO			0X04000204		//费率数据
#define TIME_SEG_TABLE_DAY1		0x04020001		//1-日时段表叔
#define TIME_ZONE_TABLE		0x04020000		//时区表数据
#define TIME_ZONE_TABLE_CHANGE_TIME 0x04000106		//时区表切换时间
#define TIME_SEG_TABLE_CHANGE_TIME  0x04000107		//时段表切换时间
#define SET_METER_CONTROL_BYTE 		0x14			//设置表控制字
#define READ_METER_CONTROL_BYTE 	0x11			//读表控制字
#define CONTROL_RELAY_CONTROL_BYTE 	0x1C			//继电器控制字

#if( EVENT_REPORT ==  YES)
#define WARNING_REPORT		0x04A10001		//消防告警状态字
#endif

#define DLT645_FACTORY_ID (0x04dfdfdb)
#define LLBYTE(w) ((BYTE)(w))				 //取一个DWORD的最低字节
#define LHBYTE(w) ((BYTE)((WORD)(w) >> 8))   //取一个DWORD的次低字节
#define HLBYTE(w) ((BYTE)((DWORD)(w) >> 16)) //取一个DWORD的次高字节
#define HHBYTE(w) ((BYTE)((DWORD)(w) >> 24)) //取一个DWORD的最高字节

#define MAX_READMETER (CycleReadMeterNum + eBIT_TOTAL) //最大抄表数,遍历的bit位数
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

typedef struct
{
	BYTE *pMessageAddr;		//报文传入地址
	WORD wSerialDataLen;	//串口数据长度
	BYTE bIDLen;			//数据标识长度
	BYTE byReceiveControl;  //接收报文控制码
	BYTE *pDataAddr;		//数据区地址
	WORD wDataLen;			//数据区长度
	BYTE bSEQ;				//后续帧序号 解析但对于组帧无影响 在后续帧的首帧将帧序号置为0
	BYTE byOperatorCode[4]; //操作者代码
	BYTE bLevel;			//密码等级
	BYTE bPassword[3];		//下发的密码
	WORD wErrType;			//错误类型
	DWORD dwSpecialFlag;	//特殊标志，保留BIT0：1：厂内模式
} T_DLT645;

//645结构
typedef struct TDlt645Head_t
{
	BYTE Sync1;
	BYTE ID[6];
	BYTE Sync2;
	BYTE Control;
	BYTE Length;
} TDlt645Head;

typedef struct
{
	BYTE byS68;		//起始68字符
	BYTE MAddr[6];  //地址
	BYTE byE68;		//结束68字符
	BYTE byCtrl;	//控制字
	BYTE byDataLen; //数据域长度
	DWORD dwDI;
} T645FmRx;

typedef struct TOtherPara_t
{
	DWORD dwCTPrim; // CT一次
	DWORD dwPTPrim; // PT一次
	BYTE bPTSecond; // PT二次
	BYTE bCTSecond; // CT二次
} TOtherPara;

typedef struct TSerial_t
{

	//公共缓冲，在RAM中必须是偶地址
	BYTE ProBuf[UART_BUFF_MAX_LEN];

	WORD ProStepDlt645;
	//Dlt645规约报文在Serial[].ProBuf中的开始位置
	WORD BeginPosDlt645;

	//645报文数据域数据长度
	WORD Dlt645DataLen;

	//DLT698_45规约接收过程计数
	WORD ProStepDlt698_45;
	//DLT698_45规约报文在Serial[].ProBuf中的开始位置
	WORD BeginPosDlt698_45;
	WORD wLen;
	BYTE Addr_Len; //通信报文中服务器地址SA长度，地址长度

	WORD RXWPoint;
	WORD RXRPoint;

	WORD TXPoint;
	WORD SendLength; //需要发送的数据长度

	//串口监视时间
	BYTE WatchSciTimer;

	//对485通讯的几个时间处理，全部采用大循环计数方式
	//接受超时时间，若收到最后一个字节后，等待此时间没有新的数据收到，无论是接受到多少数据，都要初始化指针。
	WORD RxOverCount;

	BYTE byLastRXDCon; //上次通信电表应答控制码
	
	#if (CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	//212格式判断
	WORD ProStep212;
	WORD BeginPos212;
	WORD Hb212DataLen; //QN-&&&& 不包含起始和校验
	#endif

} TSerial;

// PT CT
typedef enum
{
	eType_PT = 0, //pt
	eType_CT,	 //ct
	eType_PT_CT,  //pt_ct
} eTYPE_PT_CT_DATA;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

extern char bAddr[6]; //电表通信地址
extern char reverAddr[6];
extern DWORD PowerOnReadMeterFlag; //上电抄表成功置bit位 目前七个
extern char LtuAddr[30];
extern char MeterVersion[35];
extern TSerial Serial;
extern BYTE bHisReadMeter[MAX_SAMPLE_CHIP]; //上电初始化成FF
extern QWORD qwReadMeterFlag[MAX_SAMPLE_CHIP];
extern BYTE bReadMeterRetry;
extern BYTE bUsedChannelNum; //被使用通道个数 上电需探测
extern BYTE g_EpTcpUserNum;
extern BYTE g_EpTcpUserChannel;//环保协议用户端主站通道bit
extern TRatioPara RatioPara;
extern BYTE g_Date ;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  645 通用规约读取
//
//参数:      dwID 数据标识
//
//返回值:
//
//备注:
//--------------------------------------------------
void Dlt645_Tx_Read(DWORD dwID);
//--------------------------------------------------
//功能描述:  645 通用规约设置
//
//参数:      dwDI 数据
//           bLen
//           pBuf 设置数据内容
//			Control 控制字
//返回值:
//
//备注:
//--------------------------------------------------
void Dlt645_Tx_Write(DWORD dwID, BYTE bDataLen, BYTE *pBuf,BYTE Control);
//--------------------------------------------------
//功能描述: 其他参数抄读任务
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void api_ParaPowerOnTask(void);
//-----------------------------------------------
//函数功能: 计算单字节累加和，超出一个字节的部分舍弃
//
//参数:
//	ptr[in]:	输入缓冲
//
//	Length[in]:	缓冲长度
//
//返回值:	单字节累加和
//
//备注:
//-----------------------------------------------
BYTE lib_CheckSum(BYTE *ptr, WORD Length);
//-----------------------------------------------
//函数功能:	PT CT计算
//
//参数:	bType[in]: 获取类型
//
//返回值:	数据
//
//备注:
//-----------------------------------------------
double api_GetPtandCT(BYTE bType);
//---------------------------------------------------------------
//函数功能: uart通信645格式判断
//
//参数: 	portType[in] - 通讯端口
//
//返回值:  FALSE: 报文没有收完
//		  TRUE : 报文已收完且InitPoint()
//
//备注:
//---------------------------------------------------------------
WORD DoReceProc_Dlt645_UART(BYTE ReceByte, TSerial *p);
//--------------------------------------------------
//功能描述:  抄表任务
//
//参数:      bBtep 传入标志bit位
//
//返回值:
//
//备注:
//--------------------------------------------------
// void api_ReadMeterTask645(BYTE bStep, BYTE *bAddr);
#if (GPRS_POSITION == YES)
//----------------------------------------
//功能描述:  扩展规约检测或设置地理位置
//
//参数:      bMode  1设置地理位置；0 检测
//
//返回值:
//
//备注:
//----------------------------------------
BOOL api_CheckorSetPosition(BYTE bMode);
#endif
void Adrr_Txd(void);
void Dlt645_Tx_Read_Version(void);
BOOL CommWithMeter_DBDF(BYTE Type, BYTE bDataLen, BYTE *ID, BYTE *pBuf);
void  OpenSystemProgram(void);
void ReceiveClearBitFlag(void);
//功能描述:  多主站查询查表 查找对应关系
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE  api_GetTcpChannelTable( MeterorModu Para,BYTE bID );
//-----------------------------------------
//函数功能: 数组成员翻转函数
//
//参数:		Buf[in]	输入数据
//			Len[in]	数据长度
//返回值: 	无
//		   
//备注:   	
//-----------------------------------------
void lib_InverseData( BYTE *Buf, WORD Len );//放着有点乱 ！！！
#endif //#ifndef __DLT645_H
