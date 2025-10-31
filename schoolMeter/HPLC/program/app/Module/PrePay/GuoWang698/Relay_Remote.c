//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.9.3
//描述		继电器处理文件
//				此文件要考虑 单相内置继电器 脉冲控制,具有管脚检测脉冲
//							 单相外置继电器 脉冲控制,具有管脚检测脉冲
//							 三相内置继电器 脉冲控制,具有管脚检测电平
//							 三相外置继电器	脉冲/电平控制,无管脚检测
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#if(PREPAY_MODE == PREPAY_REMOTE)

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//继电器实际状态定义
#define ST_RELAY_OPEN				0xaa	//拉闸状态
#define ST_RELAY_CLOSE				0x55	//合闸状态
#define CMD_RELAY_OPEN				0xaa	//拉闸命令
#define CMD_RELAY_CLOSE				0x55	//合闸命令

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

typedef struct TRelayTaskFlag_t
{
	BOOL	OpFlag;				//操作继电器标志
	BYTE	KeyDown3sTimer;		//按键持续时间计数器 
}TRelayTaskFlag;

//继电器命令定义
typedef enum 
{
    eCMD_Relay_NotReport = 0x0000,				//不上报
	eCMD_RelayOn_Report  = 0x5555,				//合闸上报
	eCMD_RelayOff_Report = 0xAAAA,				//跳闸上报			
}e_Relay_Report;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
#if( SEL_EVENT_RELAY_ERR == YES  )
TRelayErrFlag RelayErrFlag;
#endif


//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static				BYTE	g_RelayFlg;				//刷新继电器实际状态标志
static	__no_init	BYTE	g_RelaySta;				//继电器实际状态（=ST_RELAY_OPEN:拉闸/=ST_RELAY_CLOSE:合闸）
static	__no_init	DWORD	g_RelayTmr;				//继电器状态计时器
static				BOOL	g_RelayBlk;				//操作阻塞标志
static				BYTE	g_RelayErrCounter;		//继电器故障秒计数器
static	__no_init	TRelayCmd		RelayCmd;
static				TRelayTaskFlag	RelayTaskFlag;
static				WORD			RelayOkStatus;
static				WORD			RelayErrStatus;
static	__no_init	BOOL			bRelayDetectFlag;//外置继电器合闸时是否继续检测继电器状态标志
static  __no_init	e_Relay_Report	g_RelayReportflag;		//控制命令上报标志
static  __no_init   e_Relay_Report  RelayOperateNeedRepoted;//跳合闸是否需要上报标志（0x00-无需上报过合闸跳闸、0x55需要上报合闸\0xAA需要上报跳闸）

//继电器状态转换表，数组竖标为接收命令，横标为当前状态，其中元素为转换后状态
//内置,远程表
static const BYTE TAB_RelayStaInside[8][13] =
{
//	eST_RlyOffR		eST_AdvOff1R	eST_AdvOff2R	eST_AllowOn		eST_AllowOnKp	eST_RelayOn		eST_RelayOnKp	eST_WaitOffR	eST_AdvWtOff1R	eST_AdvWtOff2R	eST_RlyOffGdR	eST_AdvOff1GdR	eST_AdvOff2GdR	
//	远程拉闸		远程预跳闸1		远程预跳闸2		合闸允许		合闸允许保电	合闸			合闸保电		远程等待拉闸	预跳闸1等待拉闸	预跳闸2等待拉闸	远程拉闸保护	远程预跳闸1保护	 远程预跳闸2保护
	{eST_RlyOffR,	eST_RlyOffR,	eST_RlyOffR,	eST_RlyOffR,	eST_Error,		(eST_WaitOffR),	eST_Error,		(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	},	//拉闸命令
	{eST_AllowOn,	eST_AllowOn,	eST_AllowOn,	eST_AllowOn,	eST_AllowOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	},	//允许合闸命令
	{eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	},	//直接合闸命令
	{eST_Error,		(eST_AdvOff1R),	(eST_AdvOff1R),	eST_Error,		eST_Error,		(eST_AdvWtOff1R),eST_Error,		eST_Error,		(eST_AdvWtOff1R),(eST_AdvWtOff1R),eST_Error,	(eST_AdvWtOff1R),(eST_AdvWtOff1R),},//预跳闸1命令
	{eST_Error,		(eST_AdvOff2R),	(eST_AdvOff2R),	eST_Error,		eST_Error,		(eST_AdvWtOff2R),eST_Error,		eST_Error,		(eST_AdvWtOff2R),(eST_AdvWtOff2R),eST_Error,	(eST_AdvWtOff2R),(eST_AdvWtOff2R),},//预跳闸2命令
	{eST_AllowOnKp,	eST_AllowOnKp,	eST_AllowOnKp,	eST_AllowOnKp,	eST_AllowOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	},	//保电命令
	{eST_RlyOffR,	eST_AdvOff1R,	eST_AdvOff2R,	eST_AllowOn,	eST_AllowOn,	eST_RelayOn,	eST_RelayOn,	eST_WaitOffR,	eST_AdvWtOff1R,	eST_AdvWtOff2R,	eST_RlyOffGdR,	eST_AdvOff1GdR,	eST_AdvOff2GdR,	},	//解除保电命令
	{eST_RlyOffR,	eST_RelayOn,	eST_AllowOn,	eST_AllowOn,	eST_AllowOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	},	//上电
};
//控制命令执行状态字 控制命令错误状态字
static const WORD TAB_RelayReturnStatusInside[7][13] =
{
//		eST_RlyOffR		eST_AdvOff1R		eST_AdvOff2R		eST_AllowOn			eST_AllowOnKp		    eST_RelayOn			eST_RelayOnKp		eST_WaitOffR		eST_AdvWtOff1R		eST_AdvWtOff2R		eST_RlyOffGdR		eST_AdvOff1GdR		eST_AdvOff2GdR	
//		远程拉闸		远程预跳闸1			远程预跳闸2			合闸允许			合闸允许保电		    合闸				合闸保电			远程等待拉闸		预跳闸1等待拉闸		预跳闸2等待拉闸		远程拉闸保护		远程预跳闸1保护		远程预跳闸2保护
	{R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	R_ERR_OFF_PROTECT,	    (R_OK_DELAY_OFF),	R_ERR_OFF_PROTECT,	(R_OK_DELAY_OFF),	(R_OK_DELAY_OFF),	(R_OK_DELAY_OFF),	R_OK_OVER_I_OFF,	R_OK_OVER_I_OFF,	R_OK_OVER_I_OFF,	},//拉闸命令
	{R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		    R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		},//允许合闸命令
	{R_OK_DIRECT_ON,	R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		    R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		},//直接合闸命令
	{R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_OFF,	R_ERR_AUTOON_PROTECT,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,R_ERR_AUTOON_OFF,	(R_OK_DELAY_OFF),	(R_OK_DELAY_OFF),	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	},//预跳闸1命令
	{R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_OFF,	R_ERR_AUTOON_PROTECT,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,R_ERR_AUTOON_OFF,	(R_OK_DELAY_OFF),	(R_OK_DELAY_OFF),	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	},//预跳闸2命令
	{R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		    R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		},//保电命令
	{R_OK_UNPROTECT,	R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		    R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		},//解除保电命令
};

//外置,远程表
static const BYTE TAB_RelayStaOutside[8][13] =
{
//	外置表不支持的状态按照此状态执行eST_AdvOff1R	eST_RelayOn		eST_RelayOnKp																					eST_WaitOffR	eST_AdvWtOff1R	eST_AdvWtOff2R
//	eST_RlyOffR		eST_AdvOff1R	eST_AdvOff2R	eST_AllowOn		eST_AllowOnKp	eST_RelayOn		eST_RelayOnKp	eST_WaitOffR	eST_AdvWtOff1R	eST_AdvWtOff2R  eST_RlyOffGdR	eST_AdvOff1GdR	eST_AdvOff2GdR	                   
//	远程拉闸		远程预跳闸1		远程预跳闸2		合闸允许		合闸允许保电	合闸			合闸保电		等待拉闸		预跳闸1等待拉闸	预跳闸2等待拉闸 远程拉闸保护	远程预跳闸1保护	 远程预跳闸2保护                   
	{eST_RlyOffR,	eST_RlyOffR,	eST_RlyOffR,	(eST_WaitOffR),	eST_Error,		(eST_WaitOffR),	eST_Error,		(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	},	//拉闸命令     
	{eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	},	//允许合闸命令 
	{eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	},	//直接合闸命令 
	{eST_Error,		(eST_AdvOff1R),	(eST_AdvOff1R),	(eST_AdvOff1R),	eST_Error,		(eST_AdvOff1R),	eST_Error,		eST_Error,		(eST_AdvWtOff1R),(eST_AdvWtOff1R),eST_Error,	(eST_AdvWtOff1R),(eST_AdvWtOff1R),},//预跳闸1命令  
	{eST_Error,		(eST_AdvOff1R),	(eST_AdvOff1R),	(eST_AdvOff1R),	eST_Error,		(eST_AdvOff1R),	eST_Error,		eST_Error,		(eST_AdvWtOff1R),(eST_AdvWtOff1R),eST_Error,	(eST_AdvWtOff1R),(eST_AdvWtOff1R),},//预跳闸2命令  
	{eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	},	//保电命令     
	{eST_RlyOffR,	eST_AdvOff1R,	eST_AdvOff1R,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_WaitOffR,	eST_AdvWtOff1R,	eST_AdvWtOff1R,	eST_WaitOffR,	eST_AdvWtOff1R,	eST_AdvWtOff1R,	},	//解除保电命令 
	{eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	},	//上电         
};
//控制命令执行状态字 控制命令错误状态字
static const WORD TAB_RelayReturnStatusOutside[7][13] =
{
//	外置表不支持的状态按照此状态执行		eST_AdvOff1R		eST_RelayOn			eST_RelayOnKp																													eST_WaitOffR		eST_AdvWtOff1R		eST_AdvWtOff2R
//		eST_RlyOffR		eST_AdvOff1R		eST_AdvOff2R		eST_AllowOn			eST_AllowOnKp			eST_RelayOn			eST_RelayOnKp			eST_WaitOffR		eST_AdvWtOff1R		eST_AdvWtOff2R		eST_RlyOffGdR		eST_AdvOff1GdR		eST_AdvOff2GdR	
//		远程拉闸		远程预跳闸1			远程预跳闸2			合闸允许			合闸允许保电			合闸				合闸保电				远程等待拉闸		预跳闸1等待拉闸		预跳闸2等待拉闸		远程拉闸保护		远程预跳闸1保护		远程预跳闸2保护
	{R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	(R_OK_DELAY_OFF),	R_ERR_OFF_PROTECT,		(R_OK_DELAY_OFF),	R_ERR_OFF_PROTECT,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,	},//拉闸命令    
	{R_OK_DIRECT_ON,	R_OK_DIRECT_ON,		R_OK_ALLOW_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,			R_OK_DIRECT_ON,		R_OK_DIRECT_ON,			R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,	},//允许合闸命令
	{R_OK_DIRECT_ON,	R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,			R_OK_DIRECT_ON,		R_OK_DIRECT_ON,			R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,	},//直接合闸命令
	{R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,},//预跳闸1命令
	{R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,},//预跳闸2命令
	{R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,			R_OK_PROTECT,		R_OK_PROTECT,			R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,	},//保电命令    
	{R_OK_UNPROTECT,	R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,			R_OK_UNPROTECT,		R_OK_UNPROTECT,			R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,	},//解除保电命令
};

//外置继电器若有无效的状态，置为以下默认状态
static const BYTE ST_OutSideRelay[] =
{
	0,
	eST_RlyOffR,	//远程拉闸
	eST_AdvOff1R,	//远程预跳闸1（时间到直接合闸）
	eST_AdvOff1R,	//远程预跳闸2（时间到合闸允许）	外置表	强制为 远程预跳闸1
	eST_RelayOn,	//合闸允许						外置表	强制为 合闸
	eST_RelayOnKp,	//合闸允许保电					外置表	强制为 合闸保电
	eST_RelayOn,	//合闸
	eST_RelayOnKp,	//合闸保电
	eST_WaitOffR,	//远程等待拉闸
	eST_AdvWtOff1R,	//远程预跳闸1等待拉闸
	eST_AdvWtOff2R,	//远程预跳闸2等待拉闸
	eST_WaitOffR,	//远程拉闸保护					外置表	强制为 远程等待拉闸
	eST_AdvWtOff1R,	//远程预跳闸1保护				外置表	强制为 远程预跳闸1等待拉闸
	eST_AdvWtOff2R,	//远程预跳闸2保护				外置表	强制为 远程预跳闸2等待拉闸
	eST_Error,		//错误状态
};


//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static WORD ProcRelayCmdR(BYTE Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 继电器驱动函数 闭合继电器
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
static void Drv_CloseRelay(void)
{
	if(RelayTypeConst == RELAY_INSIDE)
	{
		CLOSE_POWER_RELAY_PLUS_INSIDE;
	}
	else if(RelayTypeConst == RELAY_OUTSIDE)
	{
		if(RelayCmd.CtrlMode == 0 )//脉冲方式
		{
		}
		else
		{
			CLOSE_POWER_RELAY_LEVEL_OUTSIDE;
		}
	}
}


//-----------------------------------------------
//函数功能: 继电器驱动函数 断开继电器
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
static void Drv_OpenRelay(void)
{
	if(RelayTypeConst == RELAY_INSIDE)
	{
		OPEN_POWER_RELAY_PLUS_INSIDE;
	}
	else if(RelayTypeConst == RELAY_OUTSIDE)
	{
		if(RelayCmd.CtrlMode == 0 )//脉冲方式
		{
			OPEN_POWER_RELAY_PLUS_OUTSIDE;
		}
		else
		{
			OPEN_POWER_RELAY_LEVEL_OUTSIDE;
		}
	}
	
	CLEAR_WATCH_DOG;
}


//-----------------------------------------------
//函数功能: 获得继电器命令
//
//参数:		Type[in]--命令号
//						
//返回值:	CMD_RELAY_OPEN(0XAA)/CMD_RELAY_CLOSE(0X55)
//		   
//备注:
//-----------------------------------------------
static BYTE GetRelayCMD(BYTE Type)
{
	BYTE CloseRelayNo;
	
	if(RelayTypeConst == RELAY_INSIDE)
	{
		CloseRelayNo = eST_RelayOn;
	}
	else
	{
		CloseRelayNo = eST_AllowOn;
	}
	
	if(Type < CloseRelayNo)
	{
		return CMD_RELAY_OPEN;
	}
	
	return CMD_RELAY_CLOSE;
}

//---------------------------------------------------------------
//函数功能: 校验继电器命令和参数
//
//参数:   无
//                   
//返回值:  无
//
//备注:   
//---------------------------------------------------------------
void CheckRelayCmd( void )
{
	WORD wResult;
	
	//校验继电器命令
	if( RelayCmd.CRC32 != lib_CheckCRC32( (BYTE *)&RelayCmd, sizeof(TRelayCmd) - sizeof(DWORD)) )
	{
		wResult = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( RelaySafeRom.RelayCmd ), sizeof(TRelayCmd), (BYTE *)&RelayCmd );
		
		if( wResult == FALSE )
		{
			RelayCmd.Command = eST_RelayOn;
			RelayCmd.Alarm = FALSE;
			#if( SEL_DLT645_2007 == YES )
			RelayCmd.wRelayWaitOffTime = wRelayWaitOffTime_Def;
			#endif
			RelayCmd.wOverIProtectTime = OverIProtectTime_Def;
			RelayCmd.CtrlMode = RelayCtrlMode_Def;
			RelayCmd.dwRelayProtectI = RelayProtectI_Def;
		}
	}
	
	//Command、Alarm、CtrlMode任意一个是非法值 修正为默认值
	if( (RelayCmd.Command >= eST_Error) 
	 || (RelayCmd.Alarm > TRUE)		//报警状态只能为TRUE或者FALSE
	 || (RelayCmd.CtrlMode > 1) )	//继电器控制方式只能为0或1
	{
		RelayCmd.Command = eST_RelayOn;
		RelayCmd.Alarm = FALSE;
		#if( SEL_DLT645_2007 == YES )
		RelayCmd.wRelayWaitOffTime = wRelayWaitOffTime_Def;
		#endif
		RelayCmd.wOverIProtectTime = OverIProtectTime_Def;
		RelayCmd.CtrlMode = RelayCtrlMode_Def;
		RelayCmd.dwRelayProtectI = RelayProtectI_Def;
		//经过校验还是错误 认为EEPRPOM的值也不对了 恢复CRC 防止一直读取EEPROM
		RelayCmd.CRC32 = lib_CheckCRC32( (BYTE *)&RelayCmd, sizeof(TRelayCmd) - sizeof(DWORD) ); 
	}
}
//-----------------------------------------------
//函数功能: 判断继电器命令状态合法性
//
//参数:		无
//						
//返回值:	无
//		   
//备注:对于外置继电器有些无效的状态，置为有效的状态
//-----------------------------------------------
void JudgeRelayCmd(void)
{
	if(RelayTypeConst == RELAY_OUTSIDE)
	{
		if(RelayCmd.Command != ST_OutSideRelay[RelayCmd.Command])
		{
			ASSERT(0,0);
			RelayCmd.Command = ST_OutSideRelay[RelayCmd.Command];
			RelayCmd.CRC32 = lib_CheckCRC32((BYTE*)&RelayCmd,sizeof(TRelayCmd)-sizeof(DWORD));
		}
	}
}

//-----------------------------------------------
//函数功能: 将继电器新命令与旧命令比较，判断是否重新驱动继电器
//
//参数:		NewSta[in]	继电器新命令
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
static void JudgeRelayCmdChange(BYTE NewSta)
{		
	TRelayDetectFlag RelayDetectFlag;
	
	if(NewSta != RelayCmd.Command)
	{
		//继电器命令发生反转 （原命令合闸->新命令跳闸）&&（原命令跳闸->新命令合闸）
		if( ((GetRelayCMD(RelayCmd.Command)==CMD_RELAY_CLOSE)&&(GetRelayCMD(NewSta)==CMD_RELAY_OPEN))	
		|| ((GetRelayCMD(RelayCmd.Command)==CMD_RELAY_OPEN)&&(GetRelayCMD(NewSta)==CMD_RELAY_CLOSE)) )
		{
			if(GetRelayCMD(NewSta) == CMD_RELAY_OPEN)			
			{
				//aa，需要左移4次才出现1111，即经过4秒才能判断出是继电器合闸状态。
				//	需要左移3次出现0000，即经过3秒才能判断出是继电器跳闸状态。
				g_RelayFlg = 0xAA;
				RelayOperateNeedRepoted = eCMD_RelayOff_Report;
				api_SavePrgOperationRecord( ePRG_OPEN_RELAY_NO );
			}
			else
			{
				//55，需要左移4次才出现0000，即经过4秒才能判断出是继电器跳闸状态。
				//	需要左移3次出现1111，即经过3秒才能判断出是继电器合闸状态。
				g_RelayFlg = 0x55;
				RelayOperateNeedRepoted = eCMD_RelayOn_Report;
				api_SavePrgOperationRecord( ePRG_CLOSE_RELAY_NO );
				#if( MAX_PHASE == 3)
				if(RelayTypeConst == RELAY_OUTSIDE)
				{
					if(bRelayDetectFlag != TRUE)//if(bRelayDetectFlag == FALSE)
					{
						bRelayDetectFlag = TRUE;
						RelayDetectFlag.Flag = bRelayDetectFlag;
						api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);
					}
				}
				#endif
			}
			RelayTaskFlag.OpFlag = TRUE;
		}
		//命令更新就要记录
		RelayCmd.Command = NewSta;		
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	}
}
#if( SEL_EVENT_RELAY_ERR == YES  )

//-----------------------------------------------
//函数功能: 写入、读取负荷开关误动状态标志
//
//参数:		Option READ读取WRITE写入CLEAR清除	Status 要写入的负荷开关误动状态标志
//						
//返回值:	读取才会返回，其他状态不返回。写入才需要Status，其他不需要
//		   
//备注:		XXAA误动作已经记录了结束--进行了一次完整的误动作事件了，下次操作继电器之前不再记录和上报
//			0000未开始
//			A000发生合闸误动作
//			0A00发生跳闸误动作
//-----------------------------------------------
WORD api_DealRelayErrFlag(BYTE Operate,WORD Status)
{
	WORD Result;
	Result = 0x0000;

	//读取和更改状态时，要校验一下是否正确，不正确则从EE恢复，恢复失败就清零
	if((Operate == READ)||(Operate == WRITE))
	{
		if( RelayErrFlag.CRC32 != lib_CheckCRC32((BYTE*)&RelayErrFlag,sizeof(TRelayErrFlag)-sizeof(DWORD)) )
		{
			if( api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayErrFlag),sizeof(TRelayErrFlag),(BYTE*)&RelayErrFlag) == FALSE )
			{
				RelayErrFlag.EventRelayErrRecordFlag = 0;
				RelayErrFlag.CRC32 = lib_CheckCRC32((BYTE*)&RelayErrFlag, sizeof(TRelayErrFlag)-sizeof(DWORD)) ;
			}
		}
	}

	if(Operate == READ)//读取
	{
		Result = RelayErrFlag.EventRelayErrRecordFlag;
	}
	else if(Operate == WRITE)//写入
	{
		if(Status == 0x00AA)
		{
			if((RelayErrFlag.EventRelayErrRecordFlag == 0xA000)||(RelayErrFlag.EventRelayErrRecordFlag == 0x0A00))
			{	//如果已经开始，可以置为结束
				RelayErrFlag.EventRelayErrRecordFlag = (RelayErrFlag.EventRelayErrRecordFlag|Status);
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayErrFlag),sizeof(TRelayErrFlag),(BYTE*)&RelayErrFlag);
			}
			else
			{
				//无开始标志，不可写入结束标志;已写过结束，没必要重新写入
			}
		}
		else if((Status == 0xA000)||(Status == 0x0A00))
		{
			if(	RelayErrFlag.EventRelayErrRecordFlag != 0x0000)
			{
				//已开始另一误动，不可写入。已开始本误动，没必要重新写入。已结束不可写入
			}
			else
			{
				//未开始，可以设置为开始
				RelayErrFlag.EventRelayErrRecordFlag = Status;
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayErrFlag),sizeof(TRelayErrFlag),(BYTE*)&RelayErrFlag);
			}
		}
	}
	else if(Operate == CLEAR)//清零--清零或下次继电器动作都要清零
	{
		RelayErrFlag.EventRelayErrRecordFlag = 0;
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayErrFlag),sizeof(TRelayErrFlag),(BYTE*)&RelayErrFlag);
	}

	return Result;
}
#endif

//-----------------------------------------------
//函数功能: 继电器任务上电初始化
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void PowerUpRelay(void)
{	
	WORD wResult;
	TRelayDetectFlag RelayDetectFlag;
	
	//上电效验继电器命令及参数
	CheckRelayCmd();
	
	#if( MAX_PHASE == 3)
	if(RelayTypeConst == RELAY_OUTSIDE)
	{
		wResult = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);
		if( wResult == FALSE )
		{
			bRelayDetectFlag = TRUE;
		}
		else
		{
			bRelayDetectFlag = RelayDetectFlag.Flag;
		}
	}
	#endif
	
	if(RelayTypeConst == RELAY_NO)
	{
		if(RelayCmd.Command != eST_RelayOn)
		{
			RelayCmd.Command = eST_RelayOn;
		}
	}
	
	//判断外置继电器状态的有效性
	JudgeRelayCmd();
	
	//查看继电器状态是否为非法值,若不为非法值，继续沿用掉电之前的状态并继续判断刷新。
	if( 	((g_RelaySta!=ST_RELAY_CLOSE)&&(g_RelaySta!=ST_RELAY_OPEN)) 
		|| 	(api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR)==TRUE) )
	{
		//若为非法值，上电时将继电器检测状态置为命令状态
		if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )	
		{
			g_RelaySta = ST_RELAY_OPEN;
		}
		else
		{
			g_RelaySta = ST_RELAY_CLOSE;
		}
		
		g_RelayTmr = 0;
	}
	
	//上电处理继电器状态
	ProcRelayCmdR(eCMD_PowerOn, 0, 0);


    if(((RelayOperateNeedRepoted != eCMD_RelayOff_Report)
    &&(RelayOperateNeedRepoted != eCMD_RelayOn_Report)
    &&(RelayOperateNeedRepoted != eCMD_Relay_NotReport))//RelayOperateNeedRepoted为非法值
    || (api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR) == TRUE) )//上电RAM乱
    {
        RelayOperateNeedRepoted = eCMD_Relay_NotReport;
    }
	//上电初始化此标志位55,3~4秒能够判断出继电器的实际状态
	g_RelayFlg = 0x55;		
	//阻塞标志:不阻塞
	g_RelayBlk = FALSE;		
	//继电器故障秒计数器
	g_RelayErrCounter = 0;												

}


//-----------------------------------------------
//函数功能: 获取继电器电压电流判断状态
//
//参数:		Type[in]	REMOTE_I
//						REMOTE_U
//返回值:	TRUE/FALSE
//		    REMOTE_I	TRUE--任一相电流大于继电器拉闸电流门限值
//			REMOTE_U	TRUE--三相电压有一相大于0.75Un
//备注:
//-----------------------------------------------
static BOOL GetRemoteValueRelayStatus(WORD Type)
{
	WORD i,wMaxPhase;
	DWORD tU;
	DWORD tdw;
	
	#if( MAX_PHASE == 3)
	wMaxPhase = 3;
	#else
	wMaxPhase = 1;
	#endif
	
	if(Type == REMOTE_I)
	{
		for(i=0;i<wMaxPhase;i++)
		{
			if( RelayCmd.dwRelayProtectI != 0x0 )
			{
				api_GetRemoteData(PHASE_A+REMOTE_I+0x1000*i, DATA_HEX, 4, 4, (BYTE*)&tdw);
				if(tdw > RelayCmd.dwRelayProtectI)	
				{
				    break;
				}
			}
		}
		
		//任一相电流大于继电器拉闸电流门限值，返回TRUE
		if(i < wMaxPhase)
		{
			return TRUE;
		}
	}
	else if(Type == REMOTE_U)
	{
		//三相电压有一相大于0.75Un，返回TRUE
		tU = wStandardVoltageConst * 75 / 100;
		
		for(i=0;i<wMaxPhase;i++)
		{
			api_GetRemoteData(PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, 4, (BYTE*)&tdw);
			
            if(tdw > tU) //0.75Un
			{
				break;
			}
		}
		
		if(i < wMaxPhase)
		{
			return TRUE;	
		}
	}
	
	return FALSE;	
}


//-----------------------------------------------
//函数功能: 继电器秒任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
static void Task_Sec_Relay(void)
{
	BYTE 	i,j,k,NewSta,RelayErrCheckFlag;
	WORD	wResult;
	BOOL 	RelayProtectIFlag;
	TRelayPara	RelayPara;
	TRelayDetectFlag RelayDetectFlag;
	
	FunctionConst(RELAY_TASK);

	RelayErrCheckFlag = FALSE;
	
	
	if(RelayTypeConst == RELAY_NO)
	{
		//没有继电器的表 运行过程中还是一分钟检查一下参数
		api_GetRtcDateTime(DATETIME_ss,&NewSta);
		if( NewSta == (eTASK_RELAY_ID*3+3) )
		{	
			CheckRelayCmd(); 
		}
		if(RelayCmd.Command != eST_RelayOn)
		{
			RelayCmd.Command = eST_RelayOn;
		}
		
		return;
	}
	
	CheckRelayCmd(); 
	
	//判断外置继电器状态的有效性
	JudgeRelayCmd();
	
	//按键按下持续3s
	if( (DOWN_KEY_PRESSED) || (UP_KEY_PRESSED) )
	{
		if( RelayTaskFlag.KeyDown3sTimer < 5 )
		{
			RelayTaskFlag.KeyDown3sTimer++;
		}
		
		//按下按键持续三秒
		if( RelayTaskFlag.KeyDown3sTimer == 3 )
		{
			NewSta = RelayCmd.Command;
			if(RelayCmd.Command == eST_AllowOn)
			{
				NewSta = eST_RelayOn;
			}
			else if(RelayCmd.Command == eST_AllowOnKp)
			{
				NewSta = eST_RelayOnKp;
			}
			
			JudgeRelayCmdChange(NewSta);			
		}
	}
	else
	{
		RelayTaskFlag.KeyDown3sTimer = 0;
	}
		
	//若有阻塞标志,重新再置判断继电器标志		
	if(g_RelayBlk == TRUE)											
	{
		if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )
		{
			g_RelayFlg = 0xAA;
		}
		else
		{
			g_RelayFlg = 0x55;
		}
		RelayTaskFlag.OpFlag = TRUE;
	}
			
	//检测继电器实际状态
	{
		g_RelayFlg <<= 1;
		
		j = 0;
		k = 0;
		
		#if(MAX_PHASE == 1)
		for(i=0;i<240;i++)//约668个时钟（32768）20.4ms 和高低电平状态无关
		{
			if(RELAY_STATUS_OPEN)
			{
				j++;
			}
			else
			{
				k++;
			}
			api_Delay100us(1);
		}
		#else
		if(RelayTypeConst == RELAY_INSIDE)
		{
			if(!RELAY_STATUS_OPEN)
			{
				j = 70;
			}
		}
		else if( (RelayTypeConst == RELAY_OUTSIDE) && (MeterTypesConst == METER_ZT) )//直通外置表
		{
			for( i = 0; i < 240; i++ )//约668个时钟（32768）20.4ms 和高低电平状态无关
			{
				if(RELAY_STATUS_OPEN)
				{
					j++;
				}
				else
				{
					k++;
				}
				api_Delay100us(1);
			}
		}
		else
		{
			//三相外置继电器
			if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )
			{
				j = 0;				
			}
			else
			{
				if( bRelayDetectFlag == TRUE )
				{
					j = 0;
				}
				else
				{
					j = 70;
				}
			}
		}
		#endif

		//0x20--起动潜动状态	1--潜动		0--起动
		if( (api_GetSampleStatus(0x40)==0) || (j>30) )
		{
			g_RelayFlg |= 0x01;
		}
		if( (g_RelayFlg&0x0F) == 0x0F )			//4秒
		{
			g_RelaySta = ST_RELAY_CLOSE;		//实际合闸
			RelayErrCheckFlag = TRUE;
		}
		else if( (g_RelayFlg&0x0F) == 0x00 )	//4秒
		{			
			if( RelayTypeConst == RELAY_INSIDE )
			{
				g_RelaySta = ST_RELAY_OPEN;		//实际拉闸
				RelayErrCheckFlag = TRUE;
			}
			else
			{
				if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )	//命令拉闸
				{
					g_RelaySta = ST_RELAY_OPEN;		//实际拉闸
					RelayErrCheckFlag = TRUE;
				}
				else
				{
					#if (SEL_EVENT_RELAY_ERR == YES)
					if( (api_DealRelayErrFlag(READ,0x0000) & 0x00AA) == 0x00AA )
					{
						api_DealRelayErrFlag(CLEAR,0x0000);//清负荷开关误动
					}
					#endif
				}
			}					
		}
		else
		{
		}
				
		#if(MAX_PHASE == 3)
		if(RelayTypeConst == RELAY_OUTSIDE)
		{
			if(g_RelaySta == ST_RELAY_CLOSE)
			{
			    if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_CLOSE )//命令合闸
			    {
    				if( bRelayDetectFlag != FALSE )//if( bRelayDetectFlag == TRUE )
    				{
    					bRelayDetectFlag = FALSE;
    					RelayDetectFlag.Flag = bRelayDetectFlag;
    					api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);
    				} 
			    }
			}
		}
		#endif
	}

	if( RelayErrCheckFlag != FALSE )//刷新出实际状态后进行继电器误动检测
	{
        //负荷开关误动计数 第60s，120s，180秒重新驱动继电器三次，再往后不再处理
        if( GetRelayCMD(RelayCmd.Command) != g_RelaySta )
        {
            if(g_RelayErrCounter < 230)
            {
                g_RelayErrCounter++;
            }
            
            if( (g_RelayErrCounter%60) == 0 )
            {
                RelayTaskFlag.OpFlag = TRUE;
            }
            #if( SEL_EVENT_RELAY_ERR == YES  )
			//如果本次操作继电器之后还未记录\上报过负荷开关误动，则需要记录和上报。同时置已记录事件标志
			if((api_DealRelayErrFlag(READ,0x0000) == 0x0000)&&(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_RELAY_ERR )== TRUE ))
			{
				if(GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )//跳闸引起的负荷开关误动--宋琛
				{
					api_DealRelayErrFlag(WRITE,0x0A00);
            	}
				else//合闸引起的负荷开关误动--宋琛
				{
					api_DealRelayErrFlag(WRITE,0xA000);
				}
			}
			#endif
            api_SetSysStatus(eSYS_STATUS_RELAY_ERR);
        }
        else
        {
            g_RelayErrCounter = 0;
            api_ClrSysStatus(eSYS_STATUS_RELAY_ERR);
    
    
            //上报跳合闸成功
            if( (g_RelayReportflag == eCMD_RelayOn_Report) && (g_RelaySta == 0x55) ) 
            {
				#if( SEL_EVENT_RELAY_ERR == YES  )
				//继电器异常由跳闸引起，合闸则清除状态标志
				if(api_DealRelayErrFlag(READ,0x0000) & 0x0A00)
				{
					api_DealRelayErrFlag(CLEAR,0x0000);
				}
				#endif
		
				api_SetFollowReportStatus(eSTATUS_CloseRelaySucceed);
                g_RelayReportflag = eCMD_Relay_NotReport;
            }
            else if( (g_RelayReportflag == eCMD_RelayOff_Report) && (g_RelaySta == 0xAA) )
            {
				#if( SEL_EVENT_RELAY_ERR == YES  )
				//继电器异常由合闸引起，跳闸则清除状态标志
				if(api_DealRelayErrFlag(READ,0x0000) & 0xA000)
				{
					api_DealRelayErrFlag(CLEAR,0x0000);
				}
				#endif
				
				api_SetFollowReportStatus(eSTATUS_OpenRelaySucceed);
                g_RelayReportflag = eCMD_Relay_NotReport;
            }
            else
            {
            }
        }
	}
			
	//倒计时处理，跳闸延时，大电流延时，跳闸自恢复延时
	{
		NewSta = RelayCmd.Command;	
		//当前电流
		RelayProtectIFlag = GetRemoteValueRelayStatus(REMOTE_I);
		
		switch (RelayCmd.Command)
		{
		case eST_WaitOffR://待拉状态
			if( g_RelayTmr )
			{
				g_RelayTmr--;
				if( g_RelayTmr == 0 )
				{
					//拉闸保护状态（无拉闸延时，且电流>保护值）
					if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)											
					{
						NewSta = eST_RlyOffGdR;									
						g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
						if(g_RelayTmr == 0)
						{
							//拉闸状态
							NewSta = eST_RlyOffR;								
						}
					}
					else
					{
						//拉闸状态（无拉闸延时，且电流<保护值）
						NewSta = eST_RlyOffR;									
					}
				}
			}
			break;
		case eST_AdvWtOff1R://远程预跳闸1等待拉闸
			if( g_RelayTmr )
			{
				g_RelayTmr--;
				if( g_RelayTmr == 0 )
				{
					//拉闸保护状态（无拉闸延时，且电流>保护值）
					if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)
					{
						NewSta = eST_AdvOff1GdR;									
						g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
						if(g_RelayTmr == 0)
						{
							//预跳闸状态
							NewSta = eST_AdvOff1R;								
						}
					}
					else
					{
						//预跳闸状态（无预跳闸延时，且电流<保护值）
						NewSta = eST_AdvOff1R;							
    				}
    				
    				if( NewSta == eST_AdvOff1R )
    				{
    				    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
                        if( RelayPara.dwOffWarnTime == 0 )
                        {
                            RelayPara.dwOffWarnTime = 300;    //无有效值时默认按照5分钟
                        }
    					g_RelayTmr = RelayPara.dwOffWarnTime;
    				}
				}
			}
			break;
		case eST_AdvOff1R://预跳闸1状态 
			if( g_RelayTmr )
			{
				g_RelayTmr--;
			}
			//需要把这个拿出来 防止g_RelayTmr变成0后电表一致无法合闸
			if( g_RelayTmr == 0 )
			{
				NewSta = eST_RelayOn;
			}
			break;
		case eST_AdvWtOff2R://远程预跳闸2等待拉闸
			if( g_RelayTmr )
			{
				g_RelayTmr--;
				if( g_RelayTmr == 0 )
				{
					//拉闸保护状态（无拉闸延时，且电流>保护值）
					if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)					
					{
						NewSta = eST_AdvOff2GdR;									
						g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
						if(g_RelayTmr == 0)
						{
							//预跳闸状态
							NewSta = eST_AdvOff2R;								
						}
					}
					else
					{
						//预跳闸状态（无预跳闸延时，且电流<保护值）
						NewSta = eST_AdvOff2R;	
						
    				}
    				if( NewSta == eST_AdvOff2R )
    				{
    				    //读出暂存的跳闸自恢复时间 单位：秒
    					api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
                        if( RelayPara.dwOffWarnTime == 0 )
                        {
                            RelayPara.dwOffWarnTime = 300;    //无有效值时默认按照5分钟
                        }
    					g_RelayTmr = RelayPara.dwOffWarnTime;
    				}
				}
			}
			break;
		case eST_AdvOff2R://预跳闸2状态
			if( g_RelayTmr )
			{
				g_RelayTmr--;
			}
			//需要把这个拿出来 防止g_RelayTmr变成0后电表一致无法合闸
			if( g_RelayTmr == 0 )
			{
				NewSta = eST_AllowOn;
			}
			break;
		case eST_RlyOffGdR://拉闸保护状态
		case eST_AdvOff1GdR://预跳闸1保护状态
		case eST_AdvOff2GdR://预跳闸2保护状态
			if( g_RelayTmr )
			{
				g_RelayTmr--;
			}
			if( (g_RelayTmr==0) || (RelayProtectIFlag==FALSE) )//24小时保护时间到或小于拉闸保护电流				
			{
				if( RelayCmd.Command == eST_RlyOffGdR )
				{
					NewSta = eST_RlyOffR;
				}
				else if( RelayCmd.Command == eST_AdvOff1GdR )
				{
					NewSta = eST_AdvOff1R;
				}
				else
				{
					NewSta = eST_AdvOff2R;
				}
				
				if ( (NewSta==eST_AdvOff1R) || (NewSta==eST_AdvOff2R) )			
				{
					//读出暂存的跳闸自恢复时间 单位：秒
					api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
                    if( RelayPara.dwOffWarnTime == 0 )
                    {
                        RelayPara.dwOffWarnTime = 300;    //无有效值时默认按照5分钟
                    }
					g_RelayTmr = RelayPara.dwOffWarnTime;
				}
			}
			break;
		default:
			break;
		}
		
		JudgeRelayCmdChange(NewSta);
	}
}

//-----------------------------------------------
//函数功能: 继电器小时任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
static void Task_Hour_Relay(void)
{
	//每小时直接驱动继电器，此时碰到误动计数器在55~220之间不再驱动，防止短时间内频繁操作。
	if( (g_RelayErrCounter<55) || (g_RelayErrCounter>220) )
	{
		RelayTaskFlag.OpFlag = TRUE;
	}
}

//-----------------------------------------------
//函数功能: 继电器大循环任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void RelayTask(void)
{	
	BOOL RelayVolFlag;
	
	//掉电判断
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )	
	{
		return;
	}
	
	//秒任务
	if( api_GetTaskFlag(eTASK_RELAY_ID,eFLAG_SECOND) == TRUE )
	{		
		Task_Sec_Relay();
		api_ClrTaskFlag(eTASK_RELAY_ID,eFLAG_SECOND);
	}
	
	if(RelayTypeConst != RELAY_NO)	
	{	
		//小时任务
		if( api_GetTaskFlag(eTASK_RELAY_ID,eFLAG_HOUR) == TRUE )
		{
			Task_Hour_Relay();
			api_ClrTaskFlag(eTASK_RELAY_ID,eFLAG_HOUR);
		}
			
		//收到继电器控制消息
		if(RelayTaskFlag.OpFlag == TRUE)
		{
			RelayVolFlag = GetRemoteValueRelayStatus(REMOTE_U);
			
			//((模块未插||停止通讯)&&大于65%Un)
			if( (RelayVolFlag==TRUE) || (RelayTypeConst == RELAY_OUTSIDE) )						
			{
				if(GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN)	
				{
					Drv_OpenRelay();
					g_RelayFlg = 0xAA;//10101010
                    //是否上报标志
                    if(RelayOperateNeedRepoted == eCMD_RelayOff_Report)
                    {
					    //非厂内模式下上报,防止厂内初始化后产生上报
					    if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )
					
						{
							g_RelayReportflag = eCMD_RelayOff_Report;//要上报跳闸  
						}	
					}
					RelayOperateNeedRepoted = eCMD_Relay_NotReport;
				}
				else
				{
					Drv_CloseRelay();
					g_RelayFlg = 0x55;//01010101
					//是否上报标志
					if(RelayOperateNeedRepoted == eCMD_RelayOn_Report)
                    {
					    //非厂内模式下上报,防止厂内初始化后产生上报
					    if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )
					
						{
							g_RelayReportflag = eCMD_RelayOn_Report;//要上报跳闸  
						}	
					}
					RelayOperateNeedRepoted = eCMD_Relay_NotReport;
				}
				
				//不阻塞
				g_RelayBlk = FALSE;											
			}
			else
			{
				//阻塞
				g_RelayBlk = TRUE;											
			}
			
			RelayTaskFlag.OpFlag = FALSE;
		}	
	}	
}


//-----------------------------------------------
//函数功能: 电表清零任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void Task_ClearMeter_Relay(void)
{	
	WORD wResult;
	TRelayDetectFlag RelayDetectFlag;
	
	//效验继电器命令
	CheckRelayCmd(); 

	//继电器命令状态
	if(RelayCmd.Command == eST_AllowOnKp)
	{
		RelayCmd.Command = eST_RelayOnKp;
	}
	else if(RelayCmd.Command != eST_RelayOnKp)
	{
		RelayCmd.Command = eST_RelayOn;	
	}
	
	//外置继电器合闸时是否继续检测继电器状态标志
	bRelayDetectFlag = TRUE;
	RelayDetectFlag.Flag = TRUE;
	//继电器报警状态	TRUE:报警，FASLE：不报警
	RelayCmd.Alarm = FALSE;	
	//保存命令状态
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	//保存是否继续检测继电器状态标志
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);

	RelayTaskFlag.OpFlag = TRUE;
	RelayTaskFlag.KeyDown3sTimer = 0;
	g_RelayReportflag = eCMD_Relay_NotReport;		//上报标志清零
	RelayOperateNeedRepoted = eCMD_Relay_NotReport;          //初始化清需要上报标识
	g_RelayFlg = 0x55;				//检测标志：合闸	
	g_RelaySta = ST_RELAY_CLOSE;	//实际状态：合闸
	g_RelayBlk = FALSE;				//阻塞标志:不阻塞	
	g_RelayTmr = 0;
	g_RelayErrCounter = 0;
	#if( SEL_EVENT_RELAY_ERR == YES  )
	api_DealRelayErrFlag(CLEAR,0x0000);
	#endif
	//清零负荷开关勿动标志
	api_ClrSysStatus(eSYS_STATUS_RELAY_ERR);
}


//-----------------------------------------------
//函数功能: 厂内电表初始化任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void FactoryInitRelay(void)
{
	TRelayPara	RelayPara;
	TRelayDetectFlag RelayDetectFlag;
	
	//继电器命令状态
	if( RelayKeepPowerFlag == TRUE )//启用保电
	{
		RelayCmd.Command = eST_RelayOnKp;
	}
	else
	{
		RelayCmd.Command = eST_RelayOn;
	}
	
	//继电器报警状态	TRUE:报警，FASLE：不报警
	RelayCmd.Alarm = FALSE;	
	//跳闸延时时间（分钟）
	#if( SEL_DLT645_2007 == YES )
	RelayCmd.wRelayWaitOffTime = wRelayWaitOffTime_Def;	
	#endif
	//超电流门限保护延时时间	单位：分钟
	RelayCmd.wOverIProtectTime = OverIProtectTime_Def;	
	//继电器控制方式	继电器输出开关属性 0--脉冲 1--保持
	RelayCmd.CtrlMode = RelayCtrlMode_Def;	
	//继电器拉闸电流门限值	单位：A，换算-4	
	RelayCmd.dwRelayProtectI = RelayProtectI_Def;
	//保存命令状态
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	//暂存跳闸自恢复时间 单位：秒	
	RelayPara.dwOffWarnTime = 300;	
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
}


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
static WORD ProcRelayCmdR(BYTE Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime)
{
	BYTE	NewSta;
	WORD	wReturnStatus;
	BOOL 	RelayProtectIFlag;
	TRelayPara	RelayPara;
	
	wReturnStatus = R_OK_WARN;
	
	if (Cmd >= eCMD_RemoteNum)
	{	
		//无效命令
		return R_ERR_PASSWORD;
	}
	
	CheckRelayCmd(); 
		
	//报警命令处理
	if( (Cmd==eCMD_InAlarm) || (Cmd==eCMD_OutAlarm) )												//报警，报警解除命令
	{		
		if(Cmd == eCMD_InAlarm)
		{
			RelayCmd.Alarm = TRUE;
			wReturnStatus = R_OK_WARN;
		}
		else
		{
			RelayCmd.Alarm = FALSE;
			wReturnStatus = R_OK_UNWARN;
		}
		
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
		
		return wReturnStatus;
	}
	
	if(RelayTypeConst == RELAY_NO)
	{
		return R_ERR_PASSWORD;
	}
	
	//判断外置继电器状态的有效性
	JudgeRelayCmd();	
	
	//获取继电器转换状态
	if(RelayTypeConst == RELAY_INSIDE)
	{
		NewSta = TAB_RelayStaInside[Cmd][RelayCmd.Command-1];
	}
	else
	{
		NewSta = TAB_RelayStaOutside[Cmd][RelayCmd.Command-1];
	}
	//获得远程返回状态
	if(Cmd < eCMD_PowerOn)
	{
		if(RelayTypeConst == RELAY_INSIDE)
		{
			wReturnStatus = TAB_RelayReturnStatusInside[Cmd][RelayCmd.Command-1];
		}
		else
		{
			wReturnStatus = TAB_RelayReturnStatusOutside[Cmd][RelayCmd.Command-1];
			//若为外置表，发送合闸允许命令，且返回为直接合闸状态字时，改为返回允许合闸状态
			if((Cmd == eCMD_IndirOn)&&(wReturnStatus == R_OK_DIRECT_ON))
			{
				wReturnStatus = R_OK_ALLOW_ON;
			}
		}
	}

    //如果命令状态返回错误，在此处直接返回
    if( (NewSta == 0) || (NewSta >= eST_Error) )
    {
        return wReturnStatus;
    }
    
	//当前电流
	RelayProtectIFlag = GetRemoteValueRelayStatus(REMOTE_I);
	
	//表中带括号的状态需要进一步判断
	if( Cmd == eCMD_RelayOff )												
	{
		if( NewSta == eST_WaitOffR )											
		{
			g_RelayTmr = (DWORD)RelayWaitOffTime*60;
			
			if( g_RelayTmr == 0 )
			{				
				//拉闸保护状态（无拉闸延时，且电流>保护值）
				if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)					
				{
					NewSta = eST_RlyOffGdR;									
					g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
					if (g_RelayTmr == 0)
					{
						//拉闸状态
						NewSta = eST_RlyOffR;								
					}
				}
				else
				{
					//拉闸状态（无拉闸延时，且电流<保护值）
					NewSta = eST_RlyOffR;									
				}
			}
		}
		
		//根据新状态 返回状态也需要进一步判断
		if( NewSta == eST_RlyOffGdR )
		{
			wReturnStatus = R_OK_OVER_I_OFF;
		}
		else if( NewSta == eST_RlyOffR )
		{
			wReturnStatus = R_OK_DIRECT_OFF;
		}
		else
		{
		}
	}
	else if( (Cmd==eCMD_AdvOff1) || (Cmd==eCMD_AdvOff2) )
	{
		//收到预跳闸1命令,预跳闸2命令
		//645考虑delay为0返回错误的情况，为0时应该下发的是拉闸命令 
        if( RelayOffWarnTime == 0 )
        {
            wReturnStatus = R_ERR_AUTOON_TIME;
            return wReturnStatus;
        }
//		RelayPara.dwOffWarnTime = (DWORD)RelayOffWarnTime*5*60;   //以5分钟为时间单位-645的要求，698无此要求

		RelayPara.dwOffWarnTime = (DWORD)RelayOffWarnTime*60;  

        if( (NewSta == eST_AdvWtOff1R) || (NewSta == eST_AdvWtOff2R) )
		{
		    g_RelayTmr = (DWORD)RelayWaitOffTime*60;       
		    if( g_RelayTmr == 0 )
            {
    			//拉闸保护状态（无拉闸延时，且电流>保护值）
    			if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)						
    			{
    			    if( NewSta == eST_AdvWtOff1R )
    			    {
    				    NewSta = eST_AdvOff1GdR;
    				}
    				else
    				{
    				    NewSta = eST_AdvOff2GdR;
    				}

    				g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
    				if(g_RelayTmr == 0)
    				{
    					//预跳闸状态
    					if( NewSta == eST_AdvOff1GdR )
        			    {
        				    NewSta = eST_AdvOff1R;
        				}
        				else
        				{
        				    NewSta = eST_AdvOff2R;
        				}
        				g_RelayTmr = RelayPara.dwOffWarnTime;
    				}
    			}
    			else
    			{
    				//预跳闸状态（无预跳闸延时，且电流<保护值）
    				if( NewSta == eST_AdvWtOff1R )
    				{
    				    NewSta = eST_AdvOff1R;
    				}
    				else
    				{
    				    NewSta = eST_AdvOff2R;
    				}
    				g_RelayTmr = RelayPara.dwOffWarnTime;
    			}
    		}
    		else
    		{
    		}
		}
		else
		{
			g_RelayTmr = RelayPara.dwOffWarnTime;
		}

		//根据新状态 返回状态也需要进一步判断
		if( (NewSta == eST_AdvWtOff1R) || (NewSta == eST_AdvWtOff2R) )
		{
			wReturnStatus = R_OK_DELAY_OFF;
		}
		else if( (NewSta == eST_AdvOff1GdR) || (NewSta == eST_AdvOff2GdR) )
		{
			wReturnStatus = R_OK_OVER_I_OFF;
		}
		else
		{
		}
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
	}
	else if( Cmd == eCMD_PowerOn )//上电不进行状态转换的判断
	{
		RelayTaskFlag.OpFlag = TRUE;
	}
		
	//状态有转换
	JudgeRelayCmdChange(NewSta);
		
	return wReturnStatus;
}
	
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
WORD api_Set_RemoteRelayCmd(BYTE Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime)
{
	WORD tw;
	
	RelayOkStatus = 0;
	RelayErrStatus = 0;

	//限电延时有效 则跳闸延时失效
	if( RelayOffWarnTime )
	{
		RelayWaitOffTime = 0;
	}
	tw = ProcRelayCmdR(Cmd, RelayWaitOffTime, RelayOffWarnTime);
	
	if(tw & 0x8000)
	{
		RelayErrStatus = ( tw&(~0x8000) );
	}
	else
	{
		RelayOkStatus = tw;
	}
	
	return tw;
}

//-----------------------------------------------
//函数功能: 获得继电器状态
//
//参数:		Type[in]	0--继电器命令状态	返回 0-FALSE--命令合闸	1-TRUE--命令跳闸  2--命令跳闸延时或者大电流延时
//						1--继电器实际状态	返回 0-FALSE--合闸状态	1-TRUE--跳闸状态  2--合闸允许状态
//						2--保电状态			返回 0-FALSE--非保电		1-TRUE--保电状态
//						3--报警状态			返回 0-FALSE--非远程报警	1-TRUE--远程报警
//返回值:	0/1/2 WORD型
//备注:
//-----------------------------------------------
WORD api_GetRelayStatus(BYTE Type)
{
	WORD wStatus;	
	wStatus = 0;
	
	switch(Type)
	{
		case 0x00:
			//继电器命令状态	0-FALSE--命令合闸	1-TRUE--命令跳闸  2--跳闸延时和大电流延时
			if( 	(RelayCmd.Command==eST_RlyOffR)
				||	(RelayCmd.Command==eST_AdvOff1R) 
				||	(RelayCmd.Command==eST_AdvOff2R) )
			{
				wStatus = 1;
			}
			else if( 	(RelayCmd.Command==eST_WaitOffR)
					||	(RelayCmd.Command==eST_AdvWtOff1R)
					||	(RelayCmd.Command==eST_AdvWtOff2R)
					||	(RelayCmd.Command==eST_RlyOffGdR)
					||	(RelayCmd.Command==eST_AdvOff1GdR)
					||	(RelayCmd.Command==eST_AdvOff2GdR) )
			{
			    wStatus = 2;
			}
			else
			{
				wStatus = 0;
			}
			break;
		case 0x01:
			//继电器实际状态	0-FALSE--合闸状态	1-TRUE--跳闸状态  2-合闸允许状态
			if(g_RelaySta == ST_RELAY_OPEN)
			{
			    if((RelayCmd.Command==eST_AllowOn) || (RelayCmd.Command==eST_AllowOnKp))
    			{
    				wStatus = 2;
    			}
				else
				{
				    wStatus = 1;
				}
			}
			else
			{
			    wStatus = 0;
			}
			break;
		case 0x02:
			//保电状态			FALSE--非保电		TRUE--保电状态
			if( (RelayCmd.Command==eST_RelayOnKp) || (RelayCmd.Command==eST_AllowOnKp) )
			{
				wStatus = 1;
			}
			break;
		case 0x03:
			//报警状态			FALSE--非远程报警	TRUE--远程报警
			if(RelayCmd.Alarm == TRUE)
			{
				wStatus = 1;
			}
			break;
		default:
			break;
	}
		
	return wStatus;
}


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
WORD GetRelayReturnStatus(BYTE Type)
{
	if(Type == 0)
	{
		return RelayOkStatus;
	}
	else
	{
		return RelayErrStatus;
	}
}


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
WORD ReadRelayPara(BYTE Type,BYTE *Buf)
{
	if(Type == 0)
	{
		//继电器拉闸电流门限值    double-long-unsigned（单位：A，换算-4）
		memcpy(Buf,(BYTE*)&RelayCmd.dwRelayProtectI,4);
	}
	else if(Type == 1)
	{
		//超电流门限保护延时时间  long-unsigned（单位：分钟，换算0）
		memcpy(Buf,(BYTE*)&RelayCmd.wOverIProtectTime,2);
	}
	#if( SEL_DLT645_2007 == YES )
	else if( Type == 2 )
	{
		Buf[0] = RelayCmd.wRelayWaitOffTime;//跳闸延时时间
		Buf[1] = RelayCmd.wRelayWaitOffTime/0x100;
	}
	#endif	
	else if( Type == 3 )
	{
		Buf[0] = RelayCmd.CtrlMode;
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
	
}

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
WORD WriteRelayPara(BYTE Type,BYTE *Buf)
{
	if(Type == 0)
	{
		//继电器拉闸电流门限值    double-long-unsigned（单位：A，换算-4）
		memcpy((BYTE*)&RelayCmd.dwRelayProtectI,Buf,4);
	}
	else if(Type == 1)
	{
		//超电流门限保护延时时间  long-unsigned（单位：分钟，换算0）
		memcpy((BYTE*)&RelayCmd.wOverIProtectTime,Buf,2);
	}
	#if( SEL_DLT645_2007 == YES )
	else if( Type == 2 )
	{
		RelayCmd.wRelayWaitOffTime = Buf[1]*0x100+Buf[0]; 
	}
	#endif	
	else if( Type == 3 )
	{
		if(Buf[0] == 1)
		{
			RelayCmd.CtrlMode = 1;
		}
		else
		{
			RelayCmd.CtrlMode = 0;
		}
	}
	else
	{
		return FALSE;
	}
	
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	
	return TRUE;
}

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
void api_SetRelayRelayErrStatus(BYTE Type)
{
	RelayErrStatus = 0;
	RelayOkStatus = 0;
	
	if(Type == 0x01)
	{
		RelayErrStatus = R_ERR_HUNGUP; //电表挂起
	}
	else if(Type == 0x02)
	{
		RelayErrStatus = R_ERR_MAC; //MAC错误
	}
	else if(Type == 0x04)
	{
		RelayErrStatus = R_ERR_PASSWORD; //密码错误/未授权
	}
	else if(Type == 0x08)
	{
		RelayErrStatus = R_ERR_AUTO_OVERTIME; //安全认证超时
	}
	else if(Type == 0x10)
	{
		RelayErrStatus = R_ERR_TIME; //时间标签无效
	}
	
	RelayErrStatus &= ~(0x8000);
}
#endif
#endif//#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )


