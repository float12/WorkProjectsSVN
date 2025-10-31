//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		SYSEVENT头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __SYSEVENT_H
#define __SYSEVENT_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define MAX_SYS_UN_MSG		256
//事件记录

//模块上电
#define SYSUN_POWER_ON			0x8A01
//模块掉电
#define SYSUN_POWER_DOWN		0x8A02
//mqtt服务器关闭
#define SYSUN_MQTT_DISCON		0x8A03
//tcp 服务器关闭
#define SYSUN_TCP_CLOSE			0x8A04    //连接pn 服务器关闭较频繁 不建议记录异常事件
//tcp 服务器连接失败
#define SYSUN_TCP_CONERR		0x8A05  
//网络异常 
#define SYSUN_NETWORK_ABNORMAL  0x8A06
//接收波形串口队列变化
#define QUEUE_WAVE				0x8A07
//数据不连续
#define DATA_LOST				0x8A08
//写文件超过写间隔时间
#define WRITE_TF_MS				0x8A09
//波形数据接受满
#define WAVE_QUEUE_FULL			0x8A10
//波形数据队列 剩余空间发生变化
#define WAVE_QUEUE_SPACE_CHANGE	0x8A11
//接收非整帧字节
#define UART_RECV_NO_815		0x8A13
//接收大于4096字节
#define UART_RECV_EXCEED_4096	0x8A14
//tcp重试发送波形数据失败
#define TCP_RETRY_SEND_DATA_ERR	0x8A15
//信号差
#define BAD_SIGNAL				0x8A16
//发送数据检测tcp断开
#define SEND_DATA_DISCONNECT	0x8A17
//tcp 队列读数据检测tcp断开
#define READ_DATA_DISCONNECT	0x8A18
//重连后发送失败
#define RECON_SEND_DATA_ERR		0x8A19
//tcp 未释放socket资源
#define TCP_NOFREE_SOCKET		0x8A2A
//mqtt 未释放socket资源
#define MQTT_NOFREE_SOCKET		0x8A2B
//模块掉电
#define POWER_DOWN				0x8AFF

//
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//事件记录格式
typedef struct TSysUNMsg_t
{
	//事件类型
	WORD EventType;
	//发生时间，要记录到年：月：日：时：分：秒（5、4、3、2、1、0）
	BYTE EventTime[6];

}TSysUNMsg;

typedef struct TSaveSysMsg_t
{
	WORD SysUnMsgPoint;
	TSysUNMsg SysUNMsg[MAX_SYS_UN_MSG];
}TSysAbrEventConRom;

//连续空间结构定义
typedef struct TConMem_t
{		
	//异常信息
	TSysAbrEventConRom 	SysAbrEventConRom;
}TConMem;
//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 记录异常事件
//
//参数: 
//			Msg[in]		异常事件类型
//                    
//返回值:  	无
//
//备注: 
//-----------------------------------------------
void api_WriteSysUNMsg(WORD Msg);
//-----------------------------------------------
//函数功能: 读异常事件
//
//参数: 
//			Num[in]		上Num次异常事件记录
//          Buf[out]	输出缓冲	          
//返回值:  	返回数据长度
//
//备注:   
//-----------------------------------------------
WORD api_ReadSysUNMsg(WORD Num,BYTE *Buf);
//--------------------------------------------------
//功能描述:  上电创建或打开时间记录文件
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_PowerOnCreatSyseventTable( void );
BOOL  api_DeleteSysEventFile( void );
#endif //#ifndef __SYSEVENT_H
