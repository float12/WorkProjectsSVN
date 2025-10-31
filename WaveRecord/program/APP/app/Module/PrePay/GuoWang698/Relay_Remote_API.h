//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.9.3
//描述		继电器 Api头文件
//修改记录	

//
//----------------------------------------------------------------------
#ifndef __RELAY_Remote_API_H
#define __RELAY_Remote_API_H

#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#if(PREPAY_MODE == PREPAY_REMOTE)

//-----------------------------------------------
//				宏定义
//-----------------------------------------------						
//控制命令执行状态字
#define R_OK_DIRECT_ON				0x0001	//直接合闸
#define R_OK_ALLOW_ON				0x0002	//允许合闸
#define R_OK_DIRECT_OFF				0x0004	//直接跳闸
#define R_OK_DELAY_OFF				0x0008	//延时跳闸(跳闸延时时间)
#define R_OK_OFF_AUTO_ON			0x0010	//跳闸自动恢复
#define R_OK_OVER_I_OFF				0x0020	//延时跳闸（大电流）
#define R_OK_PROTECT				0x0100	//保电
#define R_OK_UNPROTECT				0x0200	//保电解除
#define R_OK_WARN					0x0400	//报警
#define R_OK_UNWARN					0x0800	//报警解除
//控制命令错误状态字
#define R_ERR_HUNGUP				0x8001	//电表挂起
#define R_ERR_MAC					0x8002	//MAC错误
#define R_ERR_PASSWORD				0x8004	//密码错误/未授权
#define R_ERR_AUTO_OVERTIME			0x8008	//安全认证超时
#define R_ERR_TIME					0x8010	//时间标签无效
#define R_ERR_OFF_PROTECT			0x8020	//跳闸失败(保电)
#define R_ERR_AUTOON_PROTECT		0x8040	//跳闸自动恢复命令执行失败(保电)
#define R_ERR_AUTOON_TIME			0x8080	//跳闸自动恢复时间无效
#define R_ERR_AUTOON_OFF			0x8100	//跳闸自动恢复命令执行失败(跳闸)
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//继电器命令定义
enum
{
	eCMD_RelayOff,				//拉闸
	eCMD_IndirOn,				//允许合闸
	eCMD_DirectOn,				//直接合闸
	eCMD_AdvOff1,				//预跳闸1(时间到直接合闸)
	eCMD_AdvOff2,				//预跳闸2(时间到合闸允许)
	eCMD_InKeep,				//保电
	eCMD_OutKeep,				//保电解除
	eCMD_PowerOn,				//上电
	eCMD_InAlarm,				//报警
	eCMD_OutAlarm,				//报警解除	
	eCMD_RemoteNum			
};							

//继电器状态定义，实际跳闸状态在上面，合闸状态在下面，顺序不要乱
enum
{
	//实际拉闸状态
	eST_RlyOffR = 1,			//远程拉闸
	eST_AdvOff1R,				//远程预跳闸1（时间到直接合闸）
	eST_AdvOff2R,				//远程预跳闸2（时间到合闸允许）		内置表
	//外置：实际合闸状态
	eST_AllowOn,				//合闸允许							内置表
	eST_AllowOnKp,				//合闸允许保电						内置表
	//内置：实际合闸状态
	eST_RelayOn,				//合闸
	eST_RelayOnKp,				//合闸保电
	eST_WaitOffR,				//远程等待拉闸
	eST_AdvWtOff1R,				//远程预跳闸1等待拉闸
	eST_AdvWtOff2R,				//远程预跳闸2等待拉闸
	eST_RlyOffGdR,				//远程拉闸保护						内置表
	eST_AdvOff1GdR,				//远程预跳闸1保护					内置表	
	eST_AdvOff2GdR,				//远程预跳闸2保护					内置表
	eST_Error,					//错误状态
};

typedef struct TRelayCmd_t
{
	BYTE	Command;			//继电器命令状态
	BYTE	Alarm;				//继电器报警状态	TRUE:报警，FASLE：不报警
	#if( SEL_DLT645_2007 == YES )
	WORD	wRelayWaitOffTime;	//跳闸延时时间（分钟）
	#endif
	WORD 	wOverIProtectTime;	//超电流门限保护延时时间	单位：分钟
	BYTE	CtrlMode;			//继电器控制方式	继电器输出开关属性 0--脉冲 1--保持
	DWORD	dwRelayProtectI;		//继电器拉闸电流门限值	单位：A，换算-4	
	DWORD	CRC32;
}TRelayCmd;

typedef struct TRelayPara_t
{
	DWORD	dwOffWarnTime;		//暂存跳闸自恢复时间 单位：秒	
	DWORD	CRC32;
}TRelayPara;

//外置继电器为合闸时，需不需要继续检测继电器实际状态
typedef struct TRelayDetectFlag_t
{
	BOOL	Flag;				//TRUE--需要检测  FALSE--不需要检测
	DWORD	CRC32;				
}TRelayDetectFlag;
#if( SEL_EVENT_RELAY_ERR == YES  )
typedef struct TRelayErrFlag_t
{
	//一次跳合闸后只能记录一次负荷开关误动作或拒动事件记录
	//AAAA误动作已经记录了结束--进行了一次完整的误动作事件了，下次操作继电器之前不再记录和上报
	//0000未开始
	//A000发生合闸误动作
	//0A00发生跳闸误动作
	WORD EventRelayErrRecordFlag;	
	//校验
	DWORD CRC32;
	
}TRelayErrFlag;
#endif

typedef struct TRelaySafeMem_t
{
	TRelayCmd	RelayCmd;
	TRelayPara	RelayPara;
	TRelayDetectFlag RelayDetectFlag;
	#if( SEL_EVENT_RELAY_ERR == YES  )
	TRelayErrFlag RelayErrFlag;
	#endif
}TRelaySafeRom;

typedef struct TRelayContinueMem_t
{
	BYTE    Op[4];				//操作者代码	645规约用
}TRelayConRom;



//-----------------------------------------------
//				变量声明
//-----------------------------------------------


//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 继电器任务上电初始化
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void PowerUpRelay(void);

//-----------------------------------------------
//函数功能: 继电器大循环任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void RelayTask(void);

//-----------------------------------------------
//函数功能: 电表清零任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void Task_ClearMeter_Relay(void);

//-----------------------------------------------
//函数功能: 厂内电表初始化任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void FactoryInitRelay(void);

//-----------------------------------------------
//函数功能: 远程命令调用函数
//
//参数:		Cmd[in]	0拉闸 1允许合闸 2直接合闸 3预跳闸1(时间到直接合闸) 4预跳闸2(时间到合闸允许)
//					5保电 6保电解除 7上电 8报警 9报警解除	
//			RelayWaitOffTime[in] 告警延时 unsigned（单位：分钟，换算：0）,为兼容645，此处为WORD
//			RelayOffWarnTime[in] 限电延时 long-unsigned（单位：分钟，换算：0；值为0表示永久限电）				
//返回值:	返回规约要求的应答状态字，当bit15为1时，代表错误状态字
//		   
//备注:
//-----------------------------------------------
WORD api_Set_RemoteRelayCmd(BYTE Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime);


//-----------------------------------------------
//函数功能: 获得继电器状态
//
//参数:		Type[in]	0--继电器命令状态	返回 0-FALSE--命令合闸	1-TRUE--命令跳闸  2--命令跳闸延时或者大电流延时
//						1--继电器实际状态	返回 0-FALSE--合闸状态	1-TRUE--跳闸状态  2--合闸允许状态
//						2--保电状态			返回 0-FALSE--非保电		1-TRUE--保电状态
//						3--报警状态			返回 0-FALSE--非远程报警	1-TRUE--远程报警
//返回值:	0/1/2 WORD型
//备注:

//备注:
//-----------------------------------------------
WORD api_GetRelayStatus(BYTE Type);

//-----------------------------------------------
//函数功能: 获取控制命令执行、错误状态字
//
//参数:		Type[in] 	0--控制命令执行状态字
//						1--控制命令错误状态字 
//									
//返回值:	状态字
//		   
//备注:
//-----------------------------------------------
WORD GetRelayReturnStatus(BYTE Type);

//-----------------------------------------------
//函数功能: 读出继电器参数
//
//参数:		Type[in] 	0--继电器拉闸电流门限值	
//						1--超电流门限保护延时时间   
//						2--跳闸延时时间
//						3--继电器输出开关属性 0--脉冲 1--保持
//			Buf[out] 返回数据缓存			
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
WORD ReadRelayPara(BYTE Type,BYTE *Buf);

//-----------------------------------------------
//函数功能: 写继电器参数
//
//参数:		Type[in] 	0--继电器拉闸电流门限值	
//						1--超电流门限保护延时时间   
//						2--跳闸延时时间
//						3--继电器输出开关属性 0--脉冲 1--保持
//			Buf[in] 写入数据			
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
WORD WriteRelayPara(BYTE Type,BYTE *Buf);

//-----------------------------------------------
//函数功能: 规约置继电器错误状态字
//
//参数:		Type[in] 	0x01--电表挂起
//						0x02--MAC错误
//						0x04--密码错误/未授权
//						0x08--安全认证超时
//						0x10--时间标签无效
//返回值:	TRUE/FALSE
//
//备注:有些错误状态在规约层就返回了，所以在规约层需要先置，后面跑到Set_RelayCmdR函数，前面的也就可以不关心了
//-----------------------------------------------
void api_SetRelayRelayErrStatus( BYTE Type );

#if( SEL_EVENT_RELAY_ERR == YES  )
//-----------------------------------------------
//函数功能: 写入、读取、清除负荷开关误动状态标志
//
//参数:		Option 0x00读取x01写入0x02清除	Status 要写入的负荷开关误动状态标志
//						
//返回值:	读取才会返回，其他状态不返回。
//		   
//备注:		XXAA误动作已经记录了结束--进行了一次完整的误动作事件了，下次操作继电器之前不再记录和上报
//			0000未开始
//			A000发生合闸误动作
//			0A00发生跳闸误动作
//-----------------------------------------------
WORD api_DealRelayErrFlag(BYTE Operate,WORD Status);
#endif

#endif
#endif
#endif//__RELAY_API_H







