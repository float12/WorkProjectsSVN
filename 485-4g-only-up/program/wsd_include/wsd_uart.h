//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		UART头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __UART_H
#define __UART_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define SINGLE_LOOP_ITEM	23			//一回路总共抄读瞬时量数据项的个数
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef enum
{
	eUpgradeExeSuc = 0,			//执行升级成功
	eUpgrdeFrameTimeoutErr,		//升级帧超时
	eUpgradeCheckResultErr,		//检查结果错误
	eUpgradeVeriErr,			//校验错误
	eUpgradeDARErr,				//DAR升级错误
	eUpgradeWholeProTimeoutErr, //全过程超时错误
	eFtpLoginSuccess,			//登录ftp成功
	eFtpDownLoadSuccess,		//下载文件成功
	eFtpModuleUpgradeFail,		//模块升级失败
	eFtpModuleUpgradeVerifyFail,//模块升级校验失败
	eFtpFailExit,				//模块异常退出ftp流程
	e645FrameNormal,			//645组帧发送正常
} eUpgradeResult;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern int UART_HD;
extern int IntervalTime;			//采集间隔时间
extern TRealTimer tTimer;
extern DWORD MeterBaud;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
void  Uart_Task(void *parameter);
void InitPoint(TSerial * p);
BYTE api_CalRXD_CheckSum(WORD ProtocolType, TSerial * p);
void  api_DoReceMoveData( TSerial * p,BYTE ProtocolType);
#endif //#ifndef __UART_H
